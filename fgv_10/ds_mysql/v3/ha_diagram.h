/* Copyright (c) 2003, 2011, Oracle and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA */

#include <sys/types.h>
#include <sys/stat.h>
#include <my_dir.h>
#include "transparent_file.h"
#include "pila.h"


#define DEFAULT_CHAIN_LENGTH 512
/*
  Version for file format.
  1 - Initial Version. That is, the version when the metafile was introduced.
*/

#define DIAGRAM_VERSION 1

typedef struct st_diagram_share {
  char *table_name;
  char data_file_name[FN_REFLEN];
  uint table_name_length, use_count;
  /*
    Below flag is needed to make log tables work with concurrent insert.
    For more details see comment to ha_diagram::update_status.
  */
  my_bool is_log_table;
  /*
    Here we save the length of the file for readers. This is updated by
    inserts, updates and deletes. The var is initialized along with the
    share initialization.
  */
  my_off_t saved_data_file_length;
  mysql_mutex_t mutex;
  THR_LOCK lock;
  bool update_file_opened;
  bool diagram_write_opened;
  File meta_file;           /* Meta file we use */
  File diagram_write_filedes;  /* File handler for readers */
  bool crashed;             /* Meta file is crashed */
  ha_rows rows_recorded;    /* Number of rows in tables */
  uint data_file_version;   /* Version of the data file used */
} DIAGRAM_SHARE;

struct diagram_set {
  my_off_t begin;
  my_off_t end;
};


extern "C" {

typedef struct my_dspars {
	char id;
	int len;
	void *handle;
	char *name;
    int open_mode; 
	unsigned int open_options;
	char *dsbuf;
    int pos;
	int nc;
	char **names;
	int *offc;
	unsigned int *lenc;
    unsigned int records;
	unsigned int deleted;
	int flag;
	int cindex;
	int find_flag;
	int clave[16][16];
	int trozo;
	unsigned int *origtipes;
    double valor_max[16][16];
    double valor_min[16][16];
} MY_DSPARS;

};

class ha_diagram: public handler
{
  THR_LOCK_DATA lock;      /* MySQL lock */
  DIAGRAM_SHARE *share;       /* Shared lock info */
  my_off_t current_position;  /* Current position in the file during a file scan */
  my_off_t next_position;     /* Next position in the file scan */
  my_off_t local_saved_data_file_length; /* save position for reads */
  my_off_t temp_file_length;
  uchar byte_buffer[IO_SIZE];
  Transparent_file *file_buff;
  File data_file;                   /* File handler for readers */
  File update_temp_file;
  String buffer;
  /*
    The chain contains "holes" in the file, occured because of
    deletes/updates. It is used in rnd_end() to get rid of them
    in the end of the query.
  */
  diagram_set chain_buffer[DEFAULT_CHAIN_LENGTH];
  diagram_set *chain;
  diagram_set *chain_ptr;
  uchar chain_alloced;
  uint32 chain_size;
  uint local_data_file_version;  /* Saved version of the data file used */
  bool records_is_known;
  MEM_ROOT blobroot;

  MY_DSPARS ds;
  int cindex;
  bool no_next;
  int indice_tempo;

private:
  bool get_write_pos(my_off_t *end_pos, diagram_set *closest_hole);
  int open_update_temp_file_if_needed();
  int init_diagram_writer();
  int init_data_file();

  int carga_buf(uchar *buf);
  Pila pila_dbug;
public:
  ha_diagram(handlerton *hton, TABLE_SHARE *table_arg);
  ~ha_diagram()
  {
	ha_libera_ds();
    if (chain_alloced)
      my_free(chain);
    if (file_buff)
      delete file_buff;
    free_root(&blobroot, MYF(0));
  }
  /** @brief
    The name that will be used for display purposes.
   */
  const char *table_type() const { return "DIAGRAM"; }

  /** @brief
    The name of the index type that will be used for display.
    Don't implement this method unless you really have indexes.
   */
  const char *index_type(uint inx) { return "BTREE"; }

  /** @brief
    The file extensions.
   */
  const char **bas_ext() const;

  /** @brief
    This is a list of flags that indicate what functionality the storage engine
    implements. The current table flags are documented in handler.h
  */
  ulonglong table_flags() const
  {
    return HA_NULL_IN_KEY | HA_FILE_BASED | HA_AUTO_PART_KEY | HA_NO_TRANSACTIONS | HA_CAN_REPAIR | HA_BINLOG_STMT_CAPABLE;
		
		
		/*| HA_CAN_FULLTEXT | HA_CAN_SQL_HANDLER |
                  HA_BINLOG_ROW_CAPABLE | HA_BINLOG_STMT_CAPABLE |
                  HA_DUPLICATE_POS | HA_CAN_INDEX_BLOBS | HA_AUTO_PART_KEY |
                  | HA_CAN_GEOMETRY | HA_NO_TRANSACTIONS |
                  HA_CAN_INSERT_DELAYED | HA_CAN_BIT_FIELD | HA_CAN_RTREEKEYS |
                  HA_HAS_RECORDS | HA_STATS_RECORDS_IS_EXACT | HA_CAN_REPAIR | HA_DROP_BEFORE_CREATE*/;
    ///return HA_BINLOG_STMT_CAPABLE;
  }

  /** @brief
    This is a bitmap of flags that indicates how the storage engine
    implements indexes. The current index flags are documented in
    handler.h. If you do not implement indexes, just return zero here.

      @details
    part is the key part to check. First key part is 0.
    If all_parts is set, MySQL wants to know the flags for the combined
    index, up to and including 'part'.
  */
  ulong index_flags(uint inx, uint part, bool all_parts) const
  {
    return HA_READ_NEXT | HA_READ_PREV | /*HA_READ_RANGE |*/
            HA_READ_ORDER /*| HA_KEYREAD_ONLY*/;
  }

  /** @brief
    unireg.cc will call max_supported_record_length(), max_supported_keys(),
    max_supported_key_parts(), uint max_supported_key_length()
    to make sure that the storage engine can handle the data it is about to
    send. Return *real* limits of your storage engine here; MySQL will do
    min(your_limits, MySQL_limits) automatically.
   */
  uint max_supported_record_length() const { return HA_MAX_REC_LENGTH; }

  /** @brief
    unireg.cc will call this to make sure that the storage engine can handle
    the data it is about to send. Return *real* limits of your storage engine
    here; MySQL will do min(your_limits, MySQL_limits) automatically.

      @details
    There is no need to implement ..._key_... methods if your engine doesn't
    support indexes.
   */
  uint max_supported_keys()          const { return 16; }

  /** @brief
    unireg.cc will call this to make sure that the storage engine can handle
    the data it is about to send. Return *real* limits of your storage engine
    here; MySQL will do min(your_limits, MySQL_limits) automatically.

      @details
    There is no need to implement ..._key_... methods if your engine doesn't
    support indexes.
   */
  uint max_supported_key_parts()     const { return 16; }

  /** @brief
    unireg.cc will call this to make sure that the storage engine can handle
    the data it is about to send. Return *real* limits of your storage engine
    here; MySQL will do min(your_limits, MySQL_limits) automatically.

      @details
    There is no need to implement ..._key_... methods if your engine doesn't
    support indexes.
   */
  uint max_supported_key_length()    const { return HA_MAX_REC_LENGTH; }
  /*
     Called in test_quick_select to determine if indexes should be used.
   */
  virtual double scan_time() { return (double) (stats.records+stats.deleted) / 20.0+10; }
  /* The next method will never be called */
  virtual bool fast_key_read() { return 1;}

  int index_read(uchar * buf, const uchar * key, uint key_len,
                         enum ha_rkey_function find_flag);

  int index_read_map(uchar *buf, const uchar *key,
                     key_part_map keypart_map, enum ha_rkey_function find_flag);

  int index_read_idx_map(uchar *buf, uint index, const uchar *key,
                         key_part_map keypart_map,
                         enum ha_rkey_function find_flag);
  int index_read_last_map(uchar *buf, const uchar *key, key_part_map keypart_map);

  int index_next_same(uchar *buf, const uchar *key, uint keylen);

  int comprueba_same(const uchar *key,uint length);

  int index_init(uint inx, bool sorted);
  int index_end();
  /** @brief
    We implement this in ha_diagram.cc. It's not an obligatory method;
    skip it and and MySQL will treat it as not implemented.
  */
  int index_next(uchar *buf);

  /** @brief
    We implement this in ha_diagram.cc. It's not an obligatory method;
    skip it and and MySQL will treat it as not implemented.
  */
  int index_prev(uchar *buf);

  /** @brief
    We implement this in ha_diagram.cc. It's not an obligatory method;
    skip it and and MySQL will treat it as not implemented.
  */
  int index_first(uchar *buf);

  /** @brief
    We implement this in ha_diagram.cc. It's not an obligatory method;
    skip it and and MySQL will treat it as not implemented.
  */
  int index_last(uchar *buf);


  /* 
    TODO: return actual upper bound of number of records in the table.
    (e.g. save number of records seen on full table scan and/or use file size
    as upper bound)
  */
  ha_rows estimate_rows_upper_bound() { return HA_POS_ERROR; }

  int open(const char *name, int mode, uint open_options);
  int close(void);
  int write_row(uchar * buf);
  int update_row(const uchar * old_data, uchar * new_data);
  int delete_row(const uchar * buf);
  int rnd_init(bool scan=1);
  int rnd_next(uchar *buf);
  int rnd_pos(uchar * buf, uchar *pos);
  bool check_and_repair(THD *thd);
  int check(THD* thd, HA_CHECK_OPT* check_opt);
  bool is_crashed() const;
  int rnd_end();
  int repair(THD* thd, HA_CHECK_OPT* check_opt);
  /* This is required for SQL layer to know that we support autorepair */
  bool auto_repair() const { return 1; }
  void position(const uchar *record);
  int info(uint);
  int extra(enum ha_extra_function operation);
  int delete_all_rows(void);
  int create(const char *name, TABLE *form, HA_CREATE_INFO *create_info);
  bool check_if_incompatible_data(HA_CREATE_INFO *info,
                                  uint table_changes);

  THR_LOCK_DATA **store_lock(THD *thd, THR_LOCK_DATA **to,
      enum thr_lock_type lock_type);
  int delete_table(const char *from);
  int rename_table(const char * from, const char * to);
  ha_rows records_in_range(uint inx, key_range *min_key,
                           key_range *max_key);

  /*
    These functions used to get/update status of the handler.
    Needed to enable concurrent inserts.
  */
  void get_status();
  void update_status();

  /* The following methods were added just for DIAGRAM */
  int encode_quote(uchar *buf);
  int find_current_row(uchar *buf);
  int chain_append();
  void ha_libera_ds();
};

