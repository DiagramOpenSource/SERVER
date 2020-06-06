/* Copyright (c) 2004, 2011, Oracle and/or its affiliates. All rights reserved.

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

/**
  @file ha_diagram.cc

  @brief
  The ha_diagram engine is a stubbed storage engine for diagram purposes only;
  it does nothing at this point. Its purpose is to provide a source
  code illustration of how to begin writing new storage engines; see also
  /storage/diagram/ha_diagram.h.

  @details
  ha_diagram will let you create/open/delete tables, but
  nothing further (for diagram, indexes are not supported nor can data
  be stored in the table). Use this diagram as a template for
  implementing the same functionality in your own storage engine. You
  can enable the diagram storage engine in your build by doing the
  following during your build process:<br> ./configure
  --with-diagram-storage-engine

  Once this is done, MySQL will let you create tables with:<br>
  CREATE TABLE <table name> (...) ENGINE=DIAGRAM;

  The diagram storage engine is set up to use table locks. It
  implements an diagram "SHARE" that is inserted into a hash by table
  name. You can use this to store information of state that any
  diagram handler object will be able to see when it is using that
  table.

  Please read the object definition in ha_diagram.h before reading the rest
  of this file.

  @note
  When you create an DIAGRAM table, the MySQL Server creates a table .frm
  (format) file in the database directory, using the table name as the file
  name as is customary with MySQL. No other files are created. To get an idea
  of what occurs, here is an diagram select that would do a scan of an entire
  table:

  @code
  ha_diagram::store_lock
  ha_diagram::external_lock
  ha_diagram::info
  ha_diagram::rnd_init
  ha_diagram::extra
  ENUM HA_EXTRA_CACHE        Cache record in HA_rrnd()
  ha_diagram::rnd_next
  ha_diagram::rnd_next
  ha_diagram::rnd_next
  ha_diagram::rnd_next
  ha_diagram::rnd_next
  ha_diagram::rnd_next
  ha_diagram::rnd_next
  ha_diagram::rnd_next
  ha_diagram::rnd_next
  ha_diagram::extra
  ENUM HA_EXTRA_NO_CACHE     End caching of records (def)
  ha_diagram::external_lock
  ha_diagram::extra
  ENUM HA_EXTRA_RESET        Reset database to after open
  @endcode

  Here you see that the diagram storage engine has 9 rows called before
  rnd_next signals that it has reached the end of its data. Also note that
  the table in question was already opened; had it not been open, a call to
  ha_diagram::open() would also have been necessary. Calls to
  ha_diagram::extra() are hints as to what will be occuring to the request.

  A Longer Example can be found called the "Skeleton Engine" which can be 
  found on TangentOrg. It has both an engine and a full build environment
  for building a pluggable storage engine.

  Happy coding!<br>
    -Brian
*/

#ifdef USE_PRAGMA_IMPLEMENTATION
#pragma implementation        // gcc: Class implementation
#endif

#include <dlfcn.h>
#include "sql_priv.h"
#include "sql_class.h"           // MYSQL_HANDLERTON_INTERFACE_VERSION
#include "ha_diagram.h"
#include "probes_mysql.h"
#include "sql_plugin.h"


extern "C" {
int (*fndsmysql__)(char *ejecuta,MY_DSPARS *pars,void *callback) = NULL;
};

static handler *diagram_create_handler(handlerton *hton,
                                       TABLE_SHARE *table, 
                                       MEM_ROOT *mem_root);

handlerton *diagram_hton;

/* Enlace con diagram */
static void *plib = (void *)0;

/* Variables for diagram share methods */

/* 
   Hash used to track the number of open tables; variable for diagram share
   methods
*/
static HASH diagram_open_tables;

/* The mutex used to init the hash; variable for diagram share methods */
mysql_mutex_t diagram_mutex;

/**
  @brief
  Function we use in the creation of our hash to get key.
*/

static uchar* diagram_get_key(DIAGRAM_SHARE *share, size_t *length,
                             my_bool not_used __attribute__((unused)))
{
  *length=share->table_name_length;
  return (uchar*) share->table_name;
}

#ifdef HAVE_PSI_INTERFACE
static PSI_mutex_key ex_key_mutex_diagram, ex_key_mutex_DIAGRAM_SHARE_mutex;

static PSI_mutex_info all_diagram_mutexes[]=
{
  { &ex_key_mutex_diagram, "diagram", PSI_FLAG_GLOBAL},
  { &ex_key_mutex_DIAGRAM_SHARE_mutex, "DIAGRAM_SHARE::mutex", 0}
};

static void init_diagram_psi_keys()
{
  const char* category= "diagram";
  int count;

  if (PSI_server == NULL)
    return;

  count= array_elements(all_diagram_mutexes);
  PSI_server->register_mutex(category, all_diagram_mutexes, count);
}
#endif


static int diagram_init_func(void *p)
{
  DBUG_ENTER("diagram_init_func");

#ifdef HAVE_PSI_INTERFACE
  init_diagram_psi_keys();
#endif

  diagram_hton= (handlerton *)p;
  mysql_mutex_init(ex_key_mutex_diagram, &diagram_mutex, MY_MUTEX_INIT_FAST);
  (void) my_hash_init(&diagram_open_tables,system_charset_info,32,0,0,
                      (my_hash_get_key) diagram_get_key,0,0);

  diagram_hton->state=   SHOW_OPTION_YES;
  diagram_hton->create=  diagram_create_handler;
  diagram_hton->flags=   HTON_CAN_RECREATE;

  dlerror();
  plib = dlopen("/usr/lib/mysql/plugin/diagramx_mysql.lx.so",RTLD_LAZY); 
  if (plib)
  {
      dlerror();
	  
     fndsmysql__ = (int (*)(char *,MY_DSPARS *,void *))dlsym(plib, "fndsmysql");
	 if (!fndsmysql__)
		DBUG_RETURN(-1);

     //pre_inicializa_systema = (int (*)(int *,char *[]))dlsym(plib, "pre_inicializa_systema");
  }
  else
     DBUG_RETURN(-1);


  DBUG_RETURN(0);
}


static int diagram_done_func(void *p)
{
  int error= 0;
  DBUG_ENTER("diagram_done_func");

  if (diagram_open_tables.records)
    error= 1;
  my_hash_free(&diagram_open_tables);
  mysql_mutex_destroy(&diagram_mutex);

  if (plib) {
	  dlclose(plib);
	  plib = NULL;
  }
  DBUG_RETURN(error);
}


/**
  @brief
  Example of simple lock controls. The "share" it creates is a
  structure we will pass to each diagram handler. Do you have to have
  one of these? Well, you have pieces that are used for locking, and
  they are needed to function.
*/

static DIAGRAM_SHARE *get_share(const char *table_name, TABLE *table)
{
  DIAGRAM_SHARE *share;
  uint length;
  char *tmp_name;

  mysql_mutex_lock(&diagram_mutex);
  length=(uint) strlen(table_name);

  if (!(share=(DIAGRAM_SHARE*) my_hash_search(&diagram_open_tables,
                                              (uchar*) table_name,
                                              length)))
  {
    if (!(share=(DIAGRAM_SHARE *)
          my_multi_malloc(MYF(MY_WME | MY_ZEROFILL),
                          &share, sizeof(*share),
                          &tmp_name, length+1,
                          NullS)))
    {
      mysql_mutex_unlock(&diagram_mutex);
      return NULL;
    }

    share->use_count=0;
    share->table_name_length=length;
    share->table_name=tmp_name;
    strmov(share->table_name,table_name);
    if (my_hash_insert(&diagram_open_tables, (uchar*) share))
      goto error;
    thr_lock_init(&share->lock);
    mysql_mutex_init(ex_key_mutex_DIAGRAM_SHARE_mutex,
                     &share->mutex, MY_MUTEX_INIT_FAST);
  }
  share->use_count++;
  mysql_mutex_unlock(&diagram_mutex);

  return share;

error:
  mysql_mutex_destroy(&share->mutex);
  my_free(share);

  return NULL;
}


/**
  @brief
  Free lock controls. We call this whenever we close a table. If the table had
  the last reference to the share, then we free memory associated with it.
*/

static int free_share(DIAGRAM_SHARE *share)
{
  mysql_mutex_lock(&diagram_mutex);
  if (!--share->use_count)
  {
    my_hash_delete(&diagram_open_tables, (uchar*) share);
    thr_lock_delete(&share->lock);
    mysql_mutex_destroy(&share->mutex);
    my_free(share);
  }
  mysql_mutex_unlock(&diagram_mutex);

  return 0;
}

static handler* diagram_create_handler(handlerton *hton,
                                       TABLE_SHARE *table, 
                                       MEM_ROOT *mem_root)
{
  return new (mem_root) ha_diagram(hton, table);
}

ha_diagram::ha_diagram(handlerton *hton, TABLE_SHARE *table_arg)
  :handler(hton, table_arg)
{
	memset(&ds,0,sizeof(ds));
	ds.id = 0x17;
	ds.len = sizeof(ds);
}

ha_diagram::~ha_diagram() {
	ha_libera_ds();
}

void ha_diagram::ha_libera_ds() {

  (*fndsmysql__)("close",&(ds),NULL);

  if (ds.names) {
	  for (int i = 0;i < ds.nc;i++) {
		  if (ds.names[i])
			 my_free(ds.names[i]);
	  }
	  my_free((void *)ds.names);
  }
  if (ds.name)
	  my_free((void *)ds.name);
  if (ds.offc)
	  my_free((void *)ds.offc);
  if (ds.lenc)
	  my_free((void *)ds.lenc);
  ds.handle = NULL;
  ds.name = NULL;
  ds.offc = NULL;
  ds.lenc = NULL;
  ds.names = NULL;
}

/**
  @brief
  If frm_error() is called then we will use this to determine
  the file extensions that exist for the storage engine. This is also
  used by the default rename_table and delete_table method in
  handler.cc.

  For engines that have two file name extentions (separate meta/index file
  and data file), the order of elements is relevant. First element of engine
  file name extentions array should be meta/index file extention. Second
  element - data file extention. This order is assumed by
  prepare_for_repair() when REPAIR TABLE ... USE_FRM is issued.

  @see
  rename_table method in handler.cc and
  delete_table method in handler.cc
*/

static const char *ha_diagram_exts[] = {
	".ixx",
	".dat",
  NullS
};

const char **ha_diagram::bas_ext() const
{
  return ha_diagram_exts;
}


/**
  @brief
  Used for opening tables. The name will be the name of the file.

  @details
  A table is opened when it needs to be opened; e.g. when a request comes in
  for a SELECT on the table (tables are not open and closed for each request,
  they are cached).

  Called from handler.cc by handler::ha_open(). The server opens all tables by
  calling ha_open() which then calls the handler specific open().

  @see
  handler::ha_open() in handler.cc
*/

int ha_diagram::open(const char *name, int open_mode, uint open_options/*test_if_locked*/)
{
  DBUG_ENTER("ha_diagram::open");

  FILE *fff = fopen("/tmp/debugds.txt","a");
  fprintf(fff,"open::%s::%d::%d\r\n",name,open_mode,open_options);
  fclose(fff);

  if (!(share = get_share(name, table)))
    DBUG_RETURN(1);
  thr_lock_data_init(&share->lock,&lock,NULL);

  ha_libera_ds();

  int count = 0;//table->s->fields;  
  Field *field;

  while(table->field[count]) count++;

  ds.nc = count;
  ds.open_mode = open_mode;
  ds.open_options = open_options;
  ds.names = (char **)my_malloc(sizeof(char *)*count,MYF(MY_WME));
  ds.offc = (int *)my_malloc(sizeof(int *)*count,MYF(MY_WME));
  ds.lenc = (unsigned int *)my_malloc(sizeof(int *)*count,MYF(MY_WME));

  fff = fopen("/tmp/debugds.txt","a");
  fprintf(fff,"open::count:%d\r\n",count);
  fclose(fff);

  for (int i = 0;i < count;i++) {	  
	  field = table->field[i];
	  if (!field) {
		  ds.nc = i;

		  fff = fopen("/tmp/debugds.txt","a");
		  fprintf(fff,"open::nullfield:%d\r\n",i);
		  fclose(fff);
		  break;
	  }
	  ds.names[i] = (char *)my_malloc(strlen(field->field_name)+1,MYF(MY_WME));
	  strcpy(ds.names[i],field->field_name);
	  ds.offc[i] = 0;
	  ds.lenc[i] = 0;
  }
  ds.name = (char *)my_malloc(strlen(name)+1,MYF(MY_WME));
  strcpy(ds.name,name);

  if ((*fndsmysql__)("open",&ds,NULL) < 0) {
	  ha_libera_ds();
      DBUG_RETURN(-1);
  }

  DBUG_RETURN(0);
}


/**
  @brief
  Closes a table. We call the free_share() function to free any resources
  that we have allocated in the "shared" structure.

  @details
  Called from sql_base.cc, sql_select.cc, and table.cc. In sql_select.cc it is
  only used to close up temporary tables or during the process where a
  temporary table is converted over to being a myisam table.

  For sql_base.cc look at close_data_tables().

  @see
  sql_base.cc, sql_select.cc and table.cc
*/

int ha_diagram::close(void)
{
  DBUG_ENTER("ha_diagram::close");
  FILE *fff = fopen("/tmp/debugds.txt","a");
  fprintf(fff,"close::\r\n");
  fclose(fff);
  ha_libera_ds();
  DBUG_RETURN(free_share(share));
}


/**
  @brief
  write_row() inserts a row. No extra() hint is given currently if a bulk load
  is happening. buf() is a byte array of data. You can use the field
  information to extract the data from the native byte array type.

  @details
  Example of this would be:
  @code
  for (Field **field=table->field ; *field ; field++)
  {
    ...
  }
  @endcode

  See ha_tina.cc for an diagram of extracting all of the data as strings.
  ha_berekly.cc has an diagram of how to store it intact by "packing" it
  for ha_berkeley's own native storage type.

  See the note for update_row() on auto_increments and timestamps. This
  case also applies to write_row().

  Called from item_sum.cc, item_sum.cc, sql_acl.cc, sql_insert.cc,
  sql_insert.cc, sql_select.cc, sql_table.cc, sql_udf.cc, and sql_update.cc.

  @see
  item_sum.cc, item_sum.cc, sql_acl.cc, sql_insert.cc,
  sql_insert.cc, sql_select.cc, sql_table.cc, sql_udf.cc and sql_update.cc
*/

int ha_diagram::write_row(uchar *buf)
{
  DBUG_ENTER("ha_diagram::write_row");

  FILE *fff = fopen("/tmp/debugds.txt","a");
  fprintf(fff,"write_row::\r\n");
  fclose(fff);

  ha_statistic_increment(&SSV::ha_write_count);

  if (table->timestamp_field_type & TIMESTAMP_AUTO_SET_ON_INSERT)
    table->timestamp_field->set_time();


  int err = 0;
  double v_double;
  unsigned char *ptr = (unsigned char *)ds.dsbuf;
  unsigned char *ptrd;
  for (int i = 0;i < ds.nc;i++) 
  {
	  Field *field = table->field[i];
	  int off = ds.offc[i];
	  unsigned int len = ds.lenc[i];
	  int tipo = 0;
	  if (!len || !field) {
		  FILE *fff = fopen("/tmp/debugds.txt","a");
		  fprintf(fff,"write_row_camponulo::%d:%X\r\n",i,len);
		  fclose(fff);
		  continue;
	  }
	  tipo = (int)((len >> 8) & 0xff);
	  len &= 0xff;
	  switch(field->type()) {
		case MYSQL_TYPE_VARCHAR:
		case MYSQL_TYPE_VAR_STRING:
			if (tipo == MYSQL_TYPE_VARCHAR) {
				memset(ptr+off,' ',len);
				ptrd = buf + field->offset(table->record[0]);
				len = min((unsigned int)*ptrd, len);
				//*ptrd = len;
				ptrd++;
				memcpy(ptr+off,ptrd,len);
			  FILE *fff = fopen("/tmp/debugds.txt","a");
			  fprintf(fff,"write_row::len:%d:varstring:->%-7.7s<-\r\n",len,ptr+off);
			  fclose(fff);

			}
			break;
		case MYSQL_TYPE_STRING:
			if (tipo == MYSQL_TYPE_VARCHAR)
				field->pack(ptr+off, (const unsigned char*)buf + field->offset(table->record[0]), len,false);
			break;
		case MYSQL_TYPE_SHORT:
			if (tipo == MYSQL_TYPE_SHORT)
				field->pack(ptr+off, (const unsigned char*)buf + field->offset(table->record[0]), len, false);
			break;
		case MYSQL_TYPE_INT24:
		case MYSQL_TYPE_LONGLONG:
		case MYSQL_TYPE_NEWDECIMAL:
		case MYSQL_TYPE_DECIMAL:
			break;
		case MYSQL_TYPE_LONG:
			if (tipo == MYSQL_TYPE_DOUBLE) {
				v_double = (double)(*(int *)(buf + field->offset(table->record[0])));
				field->pack(ptr+off,(const unsigned char*)&v_double,  sizeof(v_double), true);
			}
			break;
		case MYSQL_TYPE_FLOAT:
			if (tipo == MYSQL_TYPE_DOUBLE) {
				v_double = (double)(*(float *)(buf + field->offset(table->record[0])));
				field->pack(ptr+off, (const unsigned char*)&v_double, sizeof(v_double), true);
			}
			break;
		case MYSQL_TYPE_DOUBLE:
			if (tipo == MYSQL_TYPE_DOUBLE)
				field->pack(ptr+off,(const unsigned char*)buf + field->offset(table->record[0]), len, false);
			break;
		case MYSQL_TYPE_DATE:		
			break;
		default:
			//my_error(ER_CHECK_NOT_IMPLEMENTED, MYF(0), "unsuperted field type");
			err = 1;
			break;
	  };
  }

  if ((*fndsmysql__)("write_row",&ds,NULL) < 0)
      DBUG_RETURN(-1);
  /*
    Example of a successful write_row. We don't store the data
    anywhere; they are thrown away. A real implementation will
    probably need to do something with 'buf'. We report a success
    here, to pretend that the insert was successful.
  */
  DBUG_RETURN(0);
}


/**
  @brief
  Yes, update_row() does what you expect, it updates a row. old_data will have
  the previous row record in it, while new_data will have the newest data in it.
  Keep in mind that the server can do updates based on ordering if an ORDER BY
  clause was used. Consecutive ordering is not guaranteed.

  @details
  Currently new_data will not have an updated auto_increament record, or
  and updated timestamp field. You can do these for diagram by doing:
  @code
  if (table->timestamp_field_type & TIMESTAMP_AUTO_SET_ON_UPDATE)
    table->timestamp_field->set_time();
  if (table->next_number_field && record == table->record[0])
    update_auto_increment();
  @endcode

  Called from sql_select.cc, sql_acl.cc, sql_update.cc, and sql_insert.cc.

  @see
  sql_select.cc, sql_acl.cc, sql_update.cc and sql_insert.cc
*/
int ha_diagram::update_row(const uchar *old_data, uchar *new_data)
{

  DBUG_ENTER("ha_diagram::update_row");

  {
	FILE *fff = fopen("/tmp/debugds.txt","a");
	fprintf(fff,"update_row:1:\r\n");
	fclose(fff);
  }


  ha_statistic_increment(&SSV::ha_update_count);
   if (table->timestamp_field_type & TIMESTAMP_AUTO_SET_ON_UPDATE)
     table->timestamp_field->set_time();

  int err = 0,i;
  double v_double;
  unsigned char *ptr = (unsigned char *)ds.dsbuf;
  unsigned char *ptrd;
  unsigned char *buf = (unsigned char *)old_data;
  for (i = 0;i < ds.nc;i++) 
  {
	  Field *field = table->field[i];
	  int off = ds.offc[i];
	  unsigned int len = ds.lenc[i];
	  int tipo = 0;
	  if (!len || !field) {
		  FILE *fff = fopen("/tmp/debugds.txt","a");
		  fprintf(fff,"update_row_camponulo::%d:%X\r\n",i,len);
		  fclose(fff);
		  continue;
	  }
	  tipo = (int)((len >> 8) & 0xff);
	  len &= 0xff;
	  switch(field->type()) {
		case MYSQL_TYPE_VARCHAR:
		case MYSQL_TYPE_VAR_STRING:
			if (tipo == MYSQL_TYPE_VARCHAR) {
				memset(ptr+off,' ',len);
				ptrd = buf + field->offset(table->record[0]);
				len = min((unsigned int)*ptrd, len);
				//*ptrd = len;
				ptrd++;
				memcpy(ptr+off,ptrd,len);
			  FILE *fff = fopen("/tmp/debugds.txt","a");
			  fprintf(fff,"update_row::len:%d:varstring:->%-7.7s<-\r\n",len,ptr+off);
			  fclose(fff);

			}
			break;
		case MYSQL_TYPE_STRING:
			if (tipo == MYSQL_TYPE_VARCHAR)
				field->pack(ptr+off, (const unsigned char*)buf + field->offset(table->record[0]), len,false);
			break;
		case MYSQL_TYPE_SHORT:
			if (tipo == MYSQL_TYPE_SHORT)
				field->pack(ptr+off, (const unsigned char*)buf + field->offset(table->record[0]), len, false);
			break;
		case MYSQL_TYPE_INT24:
		case MYSQL_TYPE_LONGLONG:
		case MYSQL_TYPE_NEWDECIMAL:
		case MYSQL_TYPE_DECIMAL:
			break;
		case MYSQL_TYPE_LONG:
			if (tipo == MYSQL_TYPE_DOUBLE) {
				v_double = (double)(*(int *)(buf + field->offset(table->record[0])));
				field->pack(ptr+off,(const unsigned char*)&v_double,  sizeof(v_double), true);
			}
			break;
		case MYSQL_TYPE_FLOAT:
			if (tipo == MYSQL_TYPE_DOUBLE) {
				v_double = (double)(*(float *)(buf + field->offset(table->record[0])));
				field->pack(ptr+off, (const unsigned char*)&v_double, sizeof(v_double), true);
			}
			break;
		case MYSQL_TYPE_DOUBLE:
			if (tipo == MYSQL_TYPE_DOUBLE)
				field->pack(ptr+off,(const unsigned char*)buf + field->offset(table->record[0]), len, false);
			break;
		case MYSQL_TYPE_DATE:		
			break;
		default:
			//my_error(ER_CHECK_NOT_IMPLEMENTED, MYF(0), "unsuperted field type");
			err = 1;
			break;
	  };
  }
  {
	FILE *fff = fopen("/tmp/debugds.txt","a");
	fprintf(fff,"update_row:2:\r\n");
	fclose(fff);
  }

  int pos = ds.pos;
  if ((*fndsmysql__)("lock_reg",&ds,NULL) < 0)
      DBUG_RETURN(-1);
  {
	FILE *fff = fopen("/tmp/debugds.txt","a");
	fprintf(fff,"update_row:3:\r\n");
	fclose(fff);
  }
  ds.pos = pos;

  err = 0;
  ptr = (unsigned char *)ds.dsbuf;
  buf = new_data;
  for (i = 0;i < ds.nc;i++) 
  {
	  Field *field = table->field[i];
	  int off = ds.offc[i];
	  unsigned int len = ds.lenc[i];
	  int tipo = 0;
	  if (!len || !field) {
		  FILE *fff = fopen("/tmp/debugds.txt","a");
		  fprintf(fff,"update_row_camponulo::%d:%X\r\n",i,len);
		  fclose(fff);
		  continue;
	  }
	  tipo = (int)((len >> 8) & 0xff);
	  len &= 0xff;
	  switch(field->type()) {
		case MYSQL_TYPE_VARCHAR:
		case MYSQL_TYPE_VAR_STRING:
			if (tipo == MYSQL_TYPE_VARCHAR) {
				memset(ptr+off,' ',len);
				ptrd = buf + field->offset(table->record[0]);
				len = min((unsigned int)*ptrd, len);
				//*ptrd = len;
				ptrd++;
				memcpy(ptr+off,ptrd,len);
			  FILE *fff = fopen("/tmp/debugds.txt","a");
			  fprintf(fff,"update_row::len:%d:varstring:->%-7.7s<-\r\n",len,ptr+off);
			  fclose(fff);

			}
			break;
		case MYSQL_TYPE_STRING:
			if (tipo == MYSQL_TYPE_VARCHAR)
				field->pack(ptr+off, (const unsigned char*)buf + field->offset(table->record[0]), len,false);
			break;
		case MYSQL_TYPE_SHORT:
			if (tipo == MYSQL_TYPE_SHORT)
				field->pack(ptr+off, (const unsigned char*)buf + field->offset(table->record[0]), len, false);
			break;
		case MYSQL_TYPE_INT24:
		case MYSQL_TYPE_LONGLONG:
		case MYSQL_TYPE_NEWDECIMAL:
		case MYSQL_TYPE_DECIMAL:
			break;
		case MYSQL_TYPE_LONG:
			if (tipo == MYSQL_TYPE_DOUBLE) {
				v_double = (double)(*(int *)(buf + field->offset(table->record[0])));
				field->pack(ptr+off,(const unsigned char*)&v_double,  sizeof(v_double), true);
			}
			break;
		case MYSQL_TYPE_FLOAT:
			if (tipo == MYSQL_TYPE_DOUBLE) {
				v_double = (double)(*(float *)(buf + field->offset(table->record[0])));
				field->pack(ptr+off, (const unsigned char*)&v_double, sizeof(v_double), true);
			}
			break;
		case MYSQL_TYPE_DOUBLE:
			if (tipo == MYSQL_TYPE_DOUBLE)
				field->pack(ptr+off,(const unsigned char*)buf + field->offset(table->record[0]), len, false);
			break;
		case MYSQL_TYPE_DATE:		
			break;
		default:
			//my_error(ER_CHECK_NOT_IMPLEMENTED, MYF(0), "unsuperted field type");
			err = 1;
			break;
	  };
  }

  {
	FILE *fff = fopen("/tmp/debugds.txt","a");
	fprintf(fff,"update_row:4:\r\n");
	fclose(fff);
  }

  if ((*fndsmysql__)("update_row",&ds,NULL) < 0)
      DBUG_RETURN(-1);


  {
	FILE *fff = fopen("/tmp/debugds.txt","a");
	fprintf(fff,"update_row:5:\r\n");
	fclose(fff);
  }

  DBUG_RETURN(0);
}


/**
  @brief
  This will delete a row. buf will contain a copy of the row to be deleted.
  The server will call this right after the current row has been called (from
  either a previous rnd_nexT() or index call).

  @details
  If you keep a pointer to the last row or can access a primary key it will
  make doing the deletion quite a bit easier. Keep in mind that the server does
  not guarantee consecutive deletions. ORDER BY clauses can be used.

  Called in sql_acl.cc and sql_udf.cc to manage internal table
  information.  Called in sql_delete.cc, sql_insert.cc, and
  sql_select.cc. In sql_select it is used for removing duplicates
  while in insert it is used for REPLACE calls.

  @see
  sql_acl.cc, sql_udf.cc, sql_delete.cc, sql_insert.cc and sql_select.cc
*/

int ha_diagram::delete_row(const uchar *_buf)
{
  DBUG_ENTER("ha_diagram::delete_row");

  ha_statistic_increment(&SSV::ha_delete_count);

  int err = 0,i;
  double v_double;
  unsigned char *ptr = (unsigned char *)ds.dsbuf;
  unsigned char *ptrd;
  unsigned char *buf = (unsigned char *)_buf;
  for (i = 0;i < ds.nc;i++) 
  {
	  Field *field = table->field[i];
	  int off = ds.offc[i];
	  unsigned int len = ds.lenc[i];
	  int tipo = 0;
	  if (!len || !field) {
		  FILE *fff = fopen("/tmp/debugds.txt","a");
		  fprintf(fff,"delete_row_camponulo::%d:%X\r\n",i,len);
		  fclose(fff);
		  continue;
	  }
	  tipo = (int)((len >> 8) & 0xff);
	  len &= 0xff;
	  switch(field->type()) {
		case MYSQL_TYPE_VARCHAR:
		case MYSQL_TYPE_VAR_STRING:
			if (tipo == MYSQL_TYPE_VARCHAR) {
				memset(ptr+off,' ',len);
				ptrd = buf + field->offset(table->record[0]);
				len = min((unsigned int)*ptrd, len);
				//*ptrd = len;
				ptrd++;
				memcpy(ptr+off,ptrd,len);
			  FILE *fff = fopen("/tmp/debugds.txt","a");
			  fprintf(fff,"delete_row::len:%d:varstring:->%-7.7s<-\r\n",len,ptr+off);
			  fclose(fff);

			}
			break;
		case MYSQL_TYPE_STRING:
			if (tipo == MYSQL_TYPE_VARCHAR)
				field->pack(ptr+off, (const unsigned char*)buf + field->offset(table->record[0]), len,false);
			break;
		case MYSQL_TYPE_SHORT:
			if (tipo == MYSQL_TYPE_SHORT)
				field->pack(ptr+off, (const unsigned char*)buf + field->offset(table->record[0]), len, false);
			break;
		case MYSQL_TYPE_INT24:
		case MYSQL_TYPE_LONGLONG:
		case MYSQL_TYPE_NEWDECIMAL:
		case MYSQL_TYPE_DECIMAL:
			break;
		case MYSQL_TYPE_LONG:
			if (tipo == MYSQL_TYPE_DOUBLE) {
				v_double = (double)(*(int *)(buf + field->offset(table->record[0])));
				field->pack(ptr+off,(const unsigned char*)&v_double,  sizeof(v_double), true);
			}
			break;
		case MYSQL_TYPE_FLOAT:
			if (tipo == MYSQL_TYPE_DOUBLE) {
				v_double = (double)(*(float *)(buf + field->offset(table->record[0])));
				field->pack(ptr+off, (const unsigned char*)&v_double, sizeof(v_double), true);
			}
			break;
		case MYSQL_TYPE_DOUBLE:
			if (tipo == MYSQL_TYPE_DOUBLE)
				field->pack(ptr+off,(const unsigned char*)buf + field->offset(table->record[0]), len, false);
			break;
		case MYSQL_TYPE_DATE:		
			break;
		default:
			//my_error(ER_CHECK_NOT_IMPLEMENTED, MYF(0), "unsuperted field type");
			err = 1;
			break;
	  };
  }

  if ((*fndsmysql__)("delete_row",&ds,NULL) < 0)
      DBUG_RETURN(-1);

  DBUG_RETURN(0);
}


/**
  @brief
  Positions an index cursor to the index specified in the handle. Fetches the
  row if available. If the key value is null, begin at the first key of the
  index.
*/

int ha_diagram::index_read_map(uchar *buf, const uchar *key,
                               key_part_map keypart_map __attribute__((unused)),
                               enum ha_rkey_function find_flag
                               __attribute__((unused)))
{
  int rc;
  DBUG_ENTER("ha_diagram::index_read");
  MYSQL_INDEX_READ_ROW_START(table_share->db.str, table_share->table_name.str);
  rc= HA_ERR_WRONG_COMMAND;
  MYSQL_INDEX_READ_ROW_DONE(rc);
  DBUG_RETURN(rc);
}


/**
  @brief
  Used to read forward through the index.
*/

int ha_diagram::index_next(uchar *buf)
{
  int rc;
  DBUG_ENTER("ha_diagram::index_next");
  MYSQL_INDEX_READ_ROW_START(table_share->db.str, table_share->table_name.str);
  rc= HA_ERR_WRONG_COMMAND;
  MYSQL_INDEX_READ_ROW_DONE(rc);
  DBUG_RETURN(rc);
}


/**
  @brief
  Used to read backwards through the index.
*/

int ha_diagram::index_prev(uchar *buf)
{
  int rc;
  DBUG_ENTER("ha_diagram::index_prev");
  MYSQL_INDEX_READ_ROW_START(table_share->db.str, table_share->table_name.str);
  rc= HA_ERR_WRONG_COMMAND;
  MYSQL_INDEX_READ_ROW_DONE(rc);
  DBUG_RETURN(rc);
}


/**
  @brief
  index_first() asks for the first key in the index.

  @details
  Called from opt_range.cc, opt_sum.cc, sql_handler.cc, and sql_select.cc.

  @see
  opt_range.cc, opt_sum.cc, sql_handler.cc and sql_select.cc
*/
int ha_diagram::index_first(uchar *buf)
{
  int rc;
  DBUG_ENTER("ha_diagram::index_first");
  MYSQL_INDEX_READ_ROW_START(table_share->db.str, table_share->table_name.str);
  rc= HA_ERR_WRONG_COMMAND;
  MYSQL_INDEX_READ_ROW_DONE(rc);
  DBUG_RETURN(rc);
}


/**
  @brief
  index_last() asks for the last key in the index.

  @details
  Called from opt_range.cc, opt_sum.cc, sql_handler.cc, and sql_select.cc.

  @see
  opt_range.cc, opt_sum.cc, sql_handler.cc and sql_select.cc
*/
int ha_diagram::index_last(uchar *buf)
{
  int rc;
  DBUG_ENTER("ha_diagram::index_last");
  MYSQL_INDEX_READ_ROW_START(table_share->db.str, table_share->table_name.str);
  rc= HA_ERR_WRONG_COMMAND;
  MYSQL_INDEX_READ_ROW_DONE(rc);
  DBUG_RETURN(rc);
}


/**
  @brief
  rnd_init() is called when the system wants the storage engine to do a table
  scan. See the diagram in the introduction at the top of this file to see when
  rnd_init() is called.

  @details
  Called from filesort.cc, records.cc, sql_handler.cc, sql_select.cc, sql_table.cc,
  and sql_update.cc.

  @see
  filesort.cc, records.cc, sql_handler.cc, sql_select.cc, sql_table.cc and sql_update.cc
*/
int ha_diagram::rnd_init(bool scan)
{
  DBUG_ENTER("ha_diagram::rnd_init");
  FILE *fff = fopen("/tmp/debugds.txt","a");
  fprintf(fff,"rd_init::%d\r\n",(int)scan);
  fclose(fff);
  if ((*fndsmysql__)("rnd_init",&ds,NULL) < 0) {
      DBUG_RETURN(-1);
  }
  DBUG_RETURN(0);
}

int ha_diagram::rnd_end()
{
  DBUG_ENTER("ha_diagram::rnd_end");
  FILE *fff = fopen("/tmp/debugds.txt","a");
  fprintf(fff,"rnd_end::\r\n");
  fclose(fff);
  if ((*fndsmysql__)("rnd_end",&ds,NULL) < 0) {
      DBUG_RETURN(-1);
  }
  DBUG_RETURN(0);
}


/**
  @brief
  This is called for each row of the table scan. When you run out of records
  you should return HA_ERR_END_OF_FILE. Fill buff up with the row information.
  The Field structure for the table is the key to getting data into buf
  in a manner that will allow the server to understand it.

  @details
  Called from filesort.cc, records.cc, sql_handler.cc, sql_select.cc, sql_table.cc,
  and sql_update.cc.

  @see
  filesort.cc, records.cc, sql_handler.cc, sql_select.cc, sql_table.cc and sql_update.cc
*/
int ha_diagram::rnd_next(uchar *buf)
{
  int rc;
  DBUG_ENTER("ha_diagram::rnd_next");
  MYSQL_READ_ROW_START(table_share->db.str, table_share->table_name.str,
                       TRUE);
  
  /*
  FILE *fff = fopen("/tmp/debugds.txt","a");
  fprintf(fff,"rnd_next::pos:%d\r\n",ds.pos);
  fclose(fff);
  */
  memset(buf, 0, table->s->reclength);

  if ((*fndsmysql__)("rnd_next",&ds,NULL) < 0) {
	  FILE *fff = fopen("/tmp/debugds.txt","a");
	  fprintf(fff,"rnd_next::HA_ERR_END_OF_FILE\r\n");
	  fclose(fff);

	  rc= HA_ERR_END_OF_FILE;
	  MYSQL_READ_ROW_DONE(rc);
	  DBUG_RETURN(rc);
  }

  int err = 0;
  float v_float;
  int v_int;
  const unsigned char *ptr = (const unsigned char *)ds.dsbuf;
  unsigned char *ptrd;
  for (int i = 0;i < ds.nc;i++) 
  {
	  Field *field = table->field[i];
	  int off = ds.offc[i];
	  unsigned int len = ds.lenc[i];
	  int tipo = 0;
	  if (!len || !field) {
		  FILE *fff = fopen("/tmp/debugds.txt","a");
		  fprintf(fff,"rnd_next_camponulo::%d:%X\r\n",i,len);
		  fclose(fff);
		  continue;
	  }
	  tipo = (int)((len >> 8) & 0xff);
	  len &= 0xff;
	  switch(field->type()) {
		case MYSQL_TYPE_VARCHAR:
		case MYSQL_TYPE_VAR_STRING:
			if (tipo == MYSQL_TYPE_VARCHAR) {
				ptrd = buf + field->offset(table->record[0]);
				len = min(field->max_data_length(), len);
				*ptrd = len;
				ptrd++;
				memcpy(ptrd,ptr+off,len);
			}
			break;
		case MYSQL_TYPE_STRING:
			if (tipo == MYSQL_TYPE_VARCHAR)
				field->pack(buf + field->offset(table->record[0]), ptr+off, len, false);
			break;
		case MYSQL_TYPE_SHORT:
			if (tipo == MYSQL_TYPE_SHORT)
				field->pack(buf + field->offset(table->record[0]), ptr+off, len, false);
			break;
		case MYSQL_TYPE_INT24:
		case MYSQL_TYPE_LONGLONG:
		case MYSQL_TYPE_NEWDECIMAL:
		case MYSQL_TYPE_DECIMAL:
			break;
		case MYSQL_TYPE_LONG:
			if (tipo == MYSQL_TYPE_DOUBLE) {
				v_int = (int)(*(double *)(ptr+off));
				field->pack(buf + field->offset(table->record[0]), (const unsigned char*)&v_int, sizeof(v_int), true);
			}
			break;
		case MYSQL_TYPE_FLOAT:
			if (tipo == MYSQL_TYPE_DOUBLE) {
				v_float = (float)(*(double *)(ptr+off));
				field->pack(buf + field->offset(table->record[0]), (const unsigned char*)&v_float, sizeof(v_float), true);
			}
			break;
		case MYSQL_TYPE_DOUBLE:
			if (tipo == MYSQL_TYPE_DOUBLE)
				field->pack(buf + field->offset(table->record[0]), ptr+off, len, false);
			break;
		case MYSQL_TYPE_DATE:		
			break;
		default:
			//my_error(ER_CHECK_NOT_IMPLEMENTED, MYF(0), "unsuperted field type");
			err = 1;
			break;
	  };


/*		  FILE *fff = fopen("/tmp/debugds.txt","a");
		  fprintf(fff,"rnd_next_i_len::%d:%X\r\n",i,len);
		  fclose(fff);
		  
		  fff = fopen("/tmp/debugds.txt","a");
		  fprintf(fff,"rnd_next_offset::%d\r\n",c_offset[i]);
		  fclose(fff);
 	  
	  switch((*field)->type()) {
		case MYSQL_TYPE_VARCHAR:
		case MYSQL_TYPE_VAR_STRING:
		case MYSQL_TYPE_STRING:

			//(*field)->max_display_length();
			break;
		case MYSQL_TYPE_SHORT:
			break;
		case MYSQL_TYPE_INT24:
		case MYSQL_TYPE_LONG:
		case MYSQL_TYPE_LONGLONG:
		case MYSQL_TYPE_NEWDECIMAL:
		case MYSQL_TYPE_DECIMAL:
		case MYSQL_TYPE_FLOAT:
		case MYSQL_TYPE_DOUBLE:
			break;
		case MYSQL_TYPE_DATE:
			break;
		default:
			//my_error(ER_CHECK_NOT_IMPLEMENTED, MYF(0), "unsuperted field type");
			err = 1;
			break;
	  };
	  */
		  

	  /*
	  FILE *fff = fopen("/tmp/debugds.txt","a");
	  fprintf(fff,"rnd_next::field:%d:off:%d:len:%d:%-10.10s\r\n",i,off,len,ptr+off);
	  fclose(fff);
	  

	  //ptr+off, len
	  
      if (field->store("PRUEBA", 6,  system_charset_info)) // ,CHECK_FIELD_WARN is_enum ? CHECK_FIELD_IGNORE : CHECK_FIELD_WARN
      {
        //if (!is_enum)
        //  goto err;
		  FILE *fff = fopen("/tmp/debugds.txt","a");
		  fprintf(fff,"rnd_next::field_error\r\n");
		  fclose(fff);
		  err = 1;
		  break;
      }
	    const uchar *ptr= record_buffer->buffer;

	  
	  switch((*field)->type()) {
		case MYSQL_TYPE_VARCHAR:
		case MYSQL_TYPE_VAR_STRING:
		case MYSQL_TYPE_STRING:

			//(*field)->max_display_length();
			break;
		case MYSQL_TYPE_SHORT:
			break;
		case MYSQL_TYPE_INT24:
		case MYSQL_TYPE_LONG:
		case MYSQL_TYPE_LONGLONG:
		case MYSQL_TYPE_NEWDECIMAL:
		case MYSQL_TYPE_DECIMAL:
		case MYSQL_TYPE_FLOAT:
		case MYSQL_TYPE_DOUBLE:
			break;
		case MYSQL_TYPE_DATE:
			break;
		default:
			//my_error(ER_CHECK_NOT_IMPLEMENTED, MYF(0), "unsuperted field type");
			err = 1;
			break;
	  };
	  */
  }
  
  if (err) {
	  FILE *fff = fopen("/tmp/debugds.txt","a");
	  fprintf(fff,"rnd_next::HA_ERR_UNSUPPORTED\r\n");
	  fclose(fff);
	  DBUG_RETURN(HA_ERR_UNSUPPORTED);
  }

  DBUG_RETURN(0);
}


/**
  @brief
  position() is called after each call to rnd_next() if the data needs
  to be ordered. You can do something like the following to store
  the position:
  @code
  my_store_ptr(ref, ref_length, current_position);
  @endcode

  @details
  The server uses ref to store data. ref_length in the above case is
  the size needed to store current_position. ref is just a byte array
  that the server will maintain. If you are using offsets to mark rows, then
  current_position should be the offset. If it is a primary key like in
  BDB, then it needs to be a primary key.

  Called from filesort.cc, sql_select.cc, sql_delete.cc, and sql_update.cc.

  @see
  filesort.cc, sql_select.cc, sql_delete.cc and sql_update.cc
*/
void ha_diagram::position(const uchar *record)
{
  DBUG_ENTER("ha_diagram::position");
  FILE *fff = fopen("/tmp/debugds.txt","a");
  fprintf(fff,"position::\r\n");
  fclose(fff);

  my_store_ptr(ref, ref_length, (my_off_t)ds.pos); 
  DBUG_VOID_RETURN;
}


/**
  @brief
  This is like rnd_next, but you are given a position to use
  to determine the row. The position will be of the type that you stored in
  ref. You can use ha_get_ptr(pos,ref_length) to retrieve whatever key
  or position you saved when position() was called.

  @details
  Called from filesort.cc, records.cc, sql_insert.cc, sql_select.cc, and sql_update.cc.

  @see
  filesort.cc, records.cc, sql_insert.cc, sql_select.cc and sql_update.cc
*/
int ha_diagram::rnd_pos(uchar *buf, uchar *pos)
{
  int rc;
  DBUG_ENTER("ha_diagram::rnd_pos");
  MYSQL_READ_ROW_START(table_share->db.str, table_share->table_name.str,
                       TRUE);
  FILE *fff = fopen("/tmp/debugds.txt","a");
  fprintf(fff,"rnd_pos::\r\n");
  fclose(fff);

  ds.pos = (int)my_get_ptr(pos,ref_length);
  rc= ds.pos;
  if (rnd_next(buf) < 0)
	  rc = -1;
  MYSQL_READ_ROW_DONE(rc);
  DBUG_RETURN(rc);
}


/**
  @brief
  ::info() is used to return information to the optimizer. See my_base.h for
  the complete description.

  @details
  Currently this table handler doesn't implement most of the fields really needed.
  SHOW also makes use of this data.

  You will probably want to have the following in your code:
  @code
  if (records < 2)
    records = 2;
  @endcode
  The reason is that the server will optimize for cases of only a single
  record. If, in a table scan, you don't know the number of records, it
  will probably be better to set records to two so you can return as many
  records as you need. Along with records, a few more variables you may wish
  to set are:
    records
    deleted
    data_file_length
    index_file_length
    delete_length
    check_time
  Take a look at the public variables in handler.h for more information.

  Called in filesort.cc, ha_heap.cc, item_sum.cc, opt_sum.cc, sql_delete.cc,
  sql_delete.cc, sql_derived.cc, sql_select.cc, sql_select.cc, sql_select.cc,
  sql_select.cc, sql_select.cc, sql_show.cc, sql_show.cc, sql_show.cc, sql_show.cc,
  sql_table.cc, sql_union.cc, and sql_update.cc.

  @see
  filesort.cc, ha_heap.cc, item_sum.cc, opt_sum.cc, sql_delete.cc, sql_delete.cc,
  sql_derived.cc, sql_select.cc, sql_select.cc, sql_select.cc, sql_select.cc,
  sql_select.cc, sql_show.cc, sql_show.cc, sql_show.cc, sql_show.cc, sql_table.cc,
  sql_union.cc and sql_update.cc
*/
int ha_diagram::info(uint flag)
{
  DBUG_ENTER("ha_diagram::info");
  //FILE *fff = fopen("/tmp/debugds.txt","a");
  //fprintf(fff,"info::%d\r\n",flag);
  //fclose(fff);
  DBUG_RETURN(0);
}


/**
  @brief
  extra() is called whenever the server wishes to send a hint to
  the storage engine. The myisam engine implements the most hints.
  ha_innodb.cc has the most exhaustive list of these hints.

    @see
  ha_innodb.cc
*/
int ha_diagram::extra(enum ha_extra_function operation)
{
  DBUG_ENTER("ha_diagram::extra");
  FILE *fff = fopen("/tmp/debugds.txt","a");
  fprintf(fff,"extra::\r\n");
  fclose(fff);
  DBUG_RETURN(0);
}


/**
  @brief
  Used to delete all rows in a table, including cases of truncate and cases where
  the optimizer realizes that all rows will be removed as a result of an SQL statement.

  @details
  Called from item_sum.cc by Item_func_group_concat::clear(),
  Item_sum_count_distinct::clear(), and Item_func_group_concat::clear().
  Called from sql_delete.cc by mysql_delete().
  Called from sql_select.cc by JOIN::reinit().
  Called from sql_union.cc by st_select_lex_unit::exec().

  @see
  Item_func_group_concat::clear(), Item_sum_count_distinct::clear() and
  Item_func_group_concat::clear() in item_sum.cc;
  mysql_delete() in sql_delete.cc;
  JOIN::reinit() in sql_select.cc and
  st_select_lex_unit::exec() in sql_union.cc.
*/
int ha_diagram::delete_all_rows()
{
  DBUG_ENTER("ha_diagram::delete_all_rows");
  DBUG_RETURN(HA_ERR_WRONG_COMMAND);
}


/**
  @brief
  Used for handler specific truncate table.  The table is locked in
  exclusive mode and handler is responsible for reseting the auto-
  increment counter.

  @details
  Called from Truncate_statement::handler_truncate.
  Not used if the handlerton supports HTON_CAN_RECREATE, unless this
  engine can be used as a partition. In this case, it is invoked when
  a particular partition is to be truncated.

  @see
  Truncate_statement in sql_truncate.cc
  Remarks in handler::truncate.
*/
int ha_diagram::truncate()
{
  DBUG_ENTER("ha_diagram::truncate");
  DBUG_RETURN(HA_ERR_WRONG_COMMAND);
}


/**
  @brief
  This create a lock on the table. If you are implementing a storage engine
  that can handle transacations look at ha_berkely.cc to see how you will
  want to go about doing this. Otherwise you should consider calling flock()
  here. Hint: Read the section "locking functions for mysql" in lock.cc to understand
  this.

  @details
  Called from lock.cc by lock_external() and unlock_external(). Also called
  from sql_table.cc by copy_data_between_tables().

  @see
  lock.cc by lock_external() and unlock_external() in lock.cc;
  the section "locking functions for mysql" in lock.cc;
  copy_data_between_tables() in sql_table.cc.
*/
int ha_diagram::external_lock(THD *thd, int lock_type)
{
  DBUG_ENTER("ha_diagram::external_lock");
  DBUG_RETURN(0);
}


/**
  @brief
  The idea with handler::store_lock() is: The statement decides which locks
  should be needed for the table. For updates/deletes/inserts we get WRITE
  locks, for SELECT... we get read locks.

  @details
  Before adding the lock into the table lock handler (see thr_lock.c),
  mysqld calls store lock with the requested locks. Store lock can now
  modify a write lock to a read lock (or some other lock), ignore the
  lock (if we don't want to use MySQL table locks at all), or add locks
  for many tables (like we do when we are using a MERGE handler).

  Berkeley DB, for diagram, changes all WRITE locks to TL_WRITE_ALLOW_WRITE
  (which signals that we are doing WRITES, but are still allowing other
  readers and writers).

  When releasing locks, store_lock() is also called. In this case one
  usually doesn't have to do anything.

  In some exceptional cases MySQL may send a request for a TL_IGNORE;
  This means that we are requesting the same lock as last time and this
  should also be ignored. (This may happen when someone does a flush
  table when we have opened a part of the tables, in which case mysqld
  closes and reopens the tables and tries to get the same locks at last
  time). In the future we will probably try to remove this.

  Called from lock.cc by get_lock_data().

  @note
  In this method one should NEVER rely on table->in_use, it may, in fact,
  refer to a different thread! (this happens if get_lock_data() is called
  from mysql_lock_abort_for_thread() function)

  @see
  get_lock_data() in lock.cc
*/
THR_LOCK_DATA **ha_diagram::store_lock(THD *thd,
                                       THR_LOCK_DATA **to,
                                       enum thr_lock_type lock_type)
{
  if (lock_type != TL_IGNORE && lock.type == TL_UNLOCK)
    lock.type=lock_type;
  *to++= &lock;
  return to;
}


/**
  @brief
  Used to delete a table. By the time delete_table() has been called all
  opened references to this table will have been closed (and your globally
  shared references released). The variable name will just be the name of
  the table. You will need to remove any files you have created at this point.

  @details
  If you do not implement this, the default delete_table() is called from
  handler.cc and it will delete all files with the file extensions returned
  by bas_ext().

  Called from handler.cc by delete_table and ha_create_table(). Only used
  during create if the table_flag HA_DROP_BEFORE_CREATE was specified for
  the storage engine.

  @see
  delete_table and ha_create_table() in handler.cc
*/
int ha_diagram::delete_table(const char *name)
{
  DBUG_ENTER("ha_diagram::delete_table");
  /* This is not implemented but we want someone to be able that it works. */
  DBUG_RETURN(-1);
}


/**
  @brief
  Renames a table from one name to another via an alter table call.

  @details
  If you do not implement this, the default rename_table() is called from
  handler.cc and it will delete all files with the file extensions returned
  by bas_ext().

  Called from sql_table.cc by mysql_rename_table().

  @see
  mysql_rename_table() in sql_table.cc
*/
int ha_diagram::rename_table(const char * from, const char * to)
{
  DBUG_ENTER("ha_diagram::rename_table ");
  DBUG_RETURN(HA_ERR_WRONG_COMMAND);
}


/**
  @brief
  Given a starting key and an ending key, estimate the number of rows that
  will exist between the two keys.

  @details
  end_key may be empty, in which case determine if start_key matches any rows.

  Called from opt_range.cc by check_quick_keys().

  @see
  check_quick_keys() in opt_range.cc
*/
ha_rows ha_diagram::records_in_range(uint inx, key_range *min_key,
                                     key_range *max_key)
{
  DBUG_ENTER("ha_diagram::records_in_range");
  DBUG_RETURN((*fndsmysql__)("records_in_range",&ds,NULL));
}


/**
  @brief
  create() is called to create a database. The variable name will have the name
  of the table.

  @details
  When create() is called you do not need to worry about
  opening the table. Also, the .frm file will have already been
  created so adjusting create_info is not necessary. You can overwrite
  the .frm file at this point if you wish to change the table
  definition, but there are no methods currently provided for doing
  so.

  Called from handle.cc by ha_create_table().

  @see
  ha_create_table() in handle.cc
*/

int ha_diagram::create(const char *name, TABLE *table_arg,
                       HA_CREATE_INFO *create_info)
{
  DBUG_ENTER("ha_diagram::create");
  
  int err = 0;
  //int count = table_arg->s->fields;  
  Field **field;
  
  for (field= table_arg->s->field; *field; field++)
  {
	  switch((*field)->type()) {
		case MYSQL_TYPE_VARCHAR:
		case MYSQL_TYPE_VAR_STRING:
		case MYSQL_TYPE_STRING:
			//(*field)->max_display_length();
			break;
		case MYSQL_TYPE_SHORT:
			break;
		case MYSQL_TYPE_INT24:
		case MYSQL_TYPE_LONG:
		case MYSQL_TYPE_LONGLONG:
		case MYSQL_TYPE_NEWDECIMAL:
		case MYSQL_TYPE_DECIMAL:
		case MYSQL_TYPE_FLOAT:
		case MYSQL_TYPE_DOUBLE:
			break;
		case MYSQL_TYPE_DATE:
			break;
		default:
			//my_error(ER_CHECK_NOT_IMPLEMENTED, MYF(0), "unsuperted field type");
			err = 1;
			break;
	  };
  }
  
  if (err)
	  DBUG_RETURN(HA_ERR_UNSUPPORTED);
  
  if ((*fndsmysql__)("create",&ds,NULL) < 0)
      DBUG_RETURN(-1);

  DBUG_RETURN(0);
}


struct st_mysql_storage_engine diagram_storage_engine=
{ MYSQL_HANDLERTON_INTERFACE_VERSION };


mysql_declare_plugin(diagram)
{
  MYSQL_STORAGE_ENGINE_PLUGIN,
  &diagram_storage_engine,
  "DIAGRAM",
  "Diagram Software",
  "Interface mysql pre-version",
  PLUGIN_LICENSE_GPL,
  diagram_init_func,                            /* Plugin Init */
  diagram_done_func,                            /* Plugin Deinit */
  0x0001 /* 0.1 */,
  NULL,                                  /* status variables */
  NULL,                     /* system variables */
  NULL,                                         /* config options */
  0,                                            /* flags */
}
mysql_declare_plugin_end;
