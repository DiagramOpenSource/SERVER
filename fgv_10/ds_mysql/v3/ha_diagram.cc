/* 
*/

#ifdef USE_PRAGMA_IMPLEMENTATION
#pragma implementation        // gcc: Class implementation
#endif

#include "my_global.h"
#include "sql_priv.h"
#include "sql_class.h"                          // SSV
#include <mysql/plugin.h>
#include <mysql/psi/mysql_file.h>
#include "ha_diagram.h"
#include "probes_mysql.h"


/*
  uchar + uchar + ulonglong + ulonglong + ulonglong + ulonglong + uchar
*/
#define META_BUFFER_SIZE sizeof(uchar) + sizeof(uchar) + sizeof(ulonglong) \
  + sizeof(ulonglong) + sizeof(ulonglong) + sizeof(ulonglong) + sizeof(uchar)
#define DIAGRAM_CHECK_HEADER 254 // The number we use to determine corruption
#define BLOB_MEMROOT_ALLOC_SIZE 8192

/* The file extension */
#define DIAGRAM_EXT ".DIAGRAM"               // The data file
#define CSN_EXT ".DSCSN"               // Files used during repair and update
#define CSM_EXT ".DSCSM"               // Meta file


static DIAGRAM_SHARE *get_share(const char *table_name, TABLE *table);
static int free_share(DIAGRAM_SHARE *share);
static int read_meta_file(File meta_file, ha_rows *rows);
static int write_meta_file(File meta_file, ha_rows rows, bool dirty);

extern "C" void diagram_get_status(void* param, int concurrent_insert);
extern "C" void diagram_update_status(void* param);
extern "C" my_bool diagram_check_status(void* param);

/* Stuff for shares */
mysql_mutex_t diagram_mutex;
static HASH diagram_open_tables;
static handler *diagram_create_handler(handlerton *hton,
                                    TABLE_SHARE *table, 
                                    MEM_ROOT *mem_root);

static void *plib = (void *)0;

char opt_plugin_dir[FN_REFLEN];

Pila pila_tmp = NULL;

static char buf_pila[1024];

static int dentro_counter = 0;

#define DSMYSQL(a, b, c) (*fndsmysql__)(const_cast<char*>(a), b, c)
#define LOGDEBUG(a, args...) mylog(4, const_cast<char*>(a), args)
#define LOGINFO(a, args...) mylog(5, const_cast<char*>(a), args)
#define LOGWARN(a, args...) mylog(6, const_cast<char*>(a), args)
#define LOGERROR(a, args...) mylog(7, const_cast<char*>(a), args)

extern "C" {
int (*fndsmysql__)(char *ejecuta, MY_DSPARS *pars, void *callback) = NULL;
void (*mylog)(int nivel, char *formato, ...) = NULL;
void (*mylog_ini)(int argc, char** argv,
		void(*f1)(const char *format, ...),
		void(*f2)(const char *format, ...),
		void(*f3)(const char *format, ...)) = NULL;
};

#define DS_DBUG_RETURN(a) do\
{	int ds_dbug_ret = a;\
	dentro_counter--;\
	Pop(&pila_dbug, buf_pila);\
	LOGDEBUG("[%d]%s:return(%d)", dentro_counter, buf_pila, ds_dbug_ret);\
	DBUG_RETURN(ds_dbug_ret);\
}while(0)

#define DS_DBUG_ENTER(nom, fmt, args...)\
	Push(&pila_dbug, nom+12);\
	dentro_counter ++;\
	DBUG_ENTER(nom);\
	sprintf(buf_pila,"[%d]%s:%s", dentro_counter, nom+12, fmt);\
	LOGDEBUG(buf_pila, args)

typedef unsigned char Uchar;

static int ldint( Uchar *s )                 // create an int from an MIF 
{
    union { short int r;
    Uchar p[2];
    } x;
    
    x.r = 0;
    x.p[1] = *s; *x.p = s[1];
    return (int)x.r;
}
                
static int stint( short int i,Uchar *p )                   // create 2 byte MIF from an int 
{
    Uchar *q;
    
    q = ( Uchar *)&i;
    *p = q[1]; p[1] = *q;
    return( 0 );
}
/*                
static long ldlong( Uchar *s )                       // create a long from an MIF 
{
    union { long r;
    Uchar p[4];
    } x;
    x.r = 0;
    
    *x.p = s[3];
    x.p[1] = s[2];
    x.p[2] = s[1];
    x.p[3] = *s;
    return x.r;
}
                                
static int stlong( long l,Uchar *p )              // make MIF from a long 
{
    register Uchar *q;
    
    q = ( Uchar *)&l;
    *p = q[3];
    p[1] = q[2];
    p[2] = q[1];
    p[3] = *q;
    return( 0 );
}
*/

static void memcpy_oem_1250(unsigned char *c, const unsigned char *orig, int len) {
 while(len > 0) {
	len--;
	*c = *orig;
   switch (*c)
   {
   case     130 :*c=233;break;
   case     129 :*c=252;break;
   case     160 :*c=225;break;
   case     161 :*c=237;break;
   case     162 :*c=243;break;
   case     163 :*c=250;break;
   case     164 :*c=241;break;
   case     165 :*c=209;break;
   case     166 :*c=170;break;
   case     167 :*c=186;break;
   case     168 :*c=191;break;
   case     239 :*c=146;break;
   case     241 :*c=177;break;
   case     246 :*c=247;break;
   case     128 :*c=199;break;
   case     131 :*c=226;break;
   case     132 :*c=228;break;
   case     133 :*c=224;break;
   case     134 :*c=229;break;
   case     135 :*c=231;break;
   case     136 :*c=234;break;
   case     137 :*c=235;break;
   case     138 :*c=232;break;
   case     139 :*c=239;break;
   case     140 :*c=238;break;
   case     141 :*c=236;break;
   case     142 :*c=196;break;
   case     143 :*c=197;break;
   case     144 :*c=201;break;
   case     145 :*c=230;break;
   case     146 :*c=198;break;
   case     147 :*c=244;break;
   case     148 :*c=245;break;
   case     149 :*c=242;break;
   case     150 :*c=251;break;
   case     151 :*c=249;break;
   case     152 :*c=255;break;
   case     153 :*c=214;break;
   case     154 :*c=220;break;
   case     155 :*c=248;break;
   case     156 :*c=163;break;
   case     157 :*c=216;break;
   case     158 :*c=215;break;
   case     159 :*c=131;break;
   case     169 :*c=174;break;
   case     170 :*c=172;break;
   case     171 :*c=189;break;
   case     172 :*c=188;break;
   case     173 :*c=161;break;
   case     174 :*c=171;break;
   case     175 :*c=187;break;
   case     176 :*c=128;break;
   case     177 :*c=20; break;
   case     178 :*c=21; break;
   case     179 :*c=124;break;
   case     180 :*c=22; break;
   case     181 :*c=193;break;
   case     182 :*c=194;break;
   case     183 :*c=192;break;
   case     184 :*c=169;break;
   case     185 :*c=23; break;
   case     186 :*c=24; break;
   case     187 :*c=25; break;
   case     188 :*c=26; break;
   case     189 :*c=162;break;
   case     190 :*c=165;break;
   case     198 :*c=227;break;
   case     199 :*c=195;break;
   case     196 :*c=151;break;
   /*case     196 :*c='-';break;*/
   case     207 :*c=156;break;
   case     210 :*c=202;break;
   case     211 :*c=203;break;
   case     212 :*c=200;break;
   case     214 :*c=205;break;
   case     224 :*c=211;break;
   case     225 :*c=223;break;
   case     227 :*c=210;break;
   case     230 :*c=181;break;
   case     233 :*c=218;break;
   case     235 :*c=217;break;  
   }
   c++;
   orig++;
 }
}

static void memcpy_1250_oem(unsigned char *c, const unsigned char *orig, int len) {
 while(len > 0) {
	len--;
	*c = *orig;
   switch (*c)
   {
   /* sin conversion 'atras'*/
   case     193:*c ='A';break;
   case		201:*c ='E';break;
   case		205:*c ='I';break;
   case		211:*c ='O';break;
   case		218:*c ='U';break;
   case		192:*c ='A';break;
   case		200:*c ='E';break;
   case		204:*c ='I';break;
   case		210:*c ='O';break;
   case		217:*c ='U';break;   
   case	    180:*c = 39;break; /* apostrofo */
   /* reversibles (mostly) */
   case     233:*c =130;break;
   case     252:*c =129;break;
   case     225:*c =160;break;
   case     237:*c =161;break;
   case     243:*c =162;break;
   case     250:*c =163;break;
   case     241:*c =164;break;
   case     209:*c =165;break;
   case     170:*c =166;break;
   case     186:*c =167;break;
   case     191:*c =168;break;
   case     146:*c =239;break;
   case     177:*c =241;break;
   case     247:*c =246;break;
   case     199:*c =128;break;
   case     226:*c =131;break;
   case     228:*c =132;break;
   case     224:*c =133;break;
   case     229:*c =134;break;
   case     231:*c =135;break;
   case     234:*c =136;break;
   case     235:*c =137;break;
   case     232:*c =138;break;
   case     239:*c =139;break;
   case     238:*c =140;break;
   case     236:*c =141;break;
   case     196:*c =142;break;
   case     197:*c =143;break;
   /*case     201:*c =144;break;*/
   case     230:*c =145;break;
   case     198:*c =146;break;
   case     244:*c =147;break;
   case     245:*c =148;break;
   case     242:*c =149;break;
   case     251:*c =150;break;
   case     249:*c =151;break;
   case     255:*c =152;break;
   case     214:*c =153;break;
   case     220:*c =154;break;
   case     248:*c =155;break;
   case     163:*c =156;break;
   case     216:*c =157;break;
   case     215:*c =158;break;
   case     131:*c =159;break;
   case     174:*c =169;break;
   case     172:*c =170;break;
   case     189:*c =171;break;
   case     188:*c =172;break;
   case     161:*c =173;break;
   case     171:*c =174;break;
   case     187:*c =175;break;
   case     128:*c =176;break;
   case     20: *c =177;break;
   case     21: *c =178;break;
   /*case     124:*c =179;break;*/
   case     22: *c =180;break;
   /*case     193:*c =181;break;*/
   case     194:*c =182;break;
   /*case     192:*c =183;break;*/
   case     169:*c =184;break;
   case     23: *c =185;break;
   case     24: *c =186;break;
   case     25: *c =187;break;
   case     26: *c =188;break;
   case     162:*c =189;break;
   case     165:*c =190;break;
   case     227:*c =198;break;
   case     195:*c =199;break;
   
   case     156:*c =207;break;
   case     202:*c =210;break;
   case     203:*c =211;break;
   /*case     200:*c =212;break;*/
   /*case     205:*c =214;break;*/
   /*case     211:*c =224;break;*/
   case     223:*c =225;break;
   /*case     210:*c =227;break;*/
   case     181:*c =230;break;
   /*case     218:*c =233;break;*/
   /*case     217:*c =235;break;*/
   }
   c++;
   orig++;
   }
}



/*****************************************************************************
 ** DIAGRAM tables
 *****************************************************************************/

/*
  Used for sorting chains with qsort().
*/
int sort_set (diagram_set *a, diagram_set *b)
{
  /*
    We assume that intervals do not intersect. So, it is enought to compare
    any two points. Here we take start of intervals for comparison.
  */
  return ( a->begin > b->begin ? 1 : ( a->begin < b->begin ? -1 : 0 ) );
}

static uchar* diagram_get_key(DIAGRAM_SHARE *share, size_t *length,
                          my_bool not_used __attribute__((unused)))
{
  *length=share->table_name_length;
  return (uchar*) share->table_name;
}

#ifdef HAVE_PSI_INTERFACE

static PSI_mutex_key diagram_key_mutex_diagram, diagram_key_mutex_DIAGRAM_SHARE_mutex;

static PSI_mutex_info all_diagram_mutexes[]=
{
  { &diagram_key_mutex_diagram, "diagram", PSI_FLAG_GLOBAL},
  { &diagram_key_mutex_DIAGRAM_SHARE_mutex, "DIAGRAM_SHARE::mutex", 0}
};

static PSI_file_key diagram_key_file_metadata, diagram_key_file_data,
  diagram_key_file_update;

static PSI_file_info all_diagram_files[]=
{
  { &diagram_key_file_metadata, "metadata", 0},
  { &diagram_key_file_data, "data", 0},
  { &diagram_key_file_update, "update", 0}
};

static void init_diagram_psi_keys(void)
{
  const char* category= "diagram";
  int count;

  if (PSI_server == NULL)
    return;

  count= array_elements(all_diagram_mutexes);
  PSI_server->register_mutex(category, all_diagram_mutexes, count);

  count= array_elements(all_diagram_files);
  PSI_server->register_file(category, all_diagram_files, count);
}
#endif /* HAVE_PSI_INTERFACE */

static int diagram_init_func(void *p)
{
  handlerton *diagram_hton;

#ifdef HAVE_PSI_INTERFACE
  init_diagram_psi_keys();
#endif

  diagram_hton= (handlerton *)p;
  mysql_mutex_init(diagram_key_mutex_diagram, &diagram_mutex, MY_MUTEX_INIT_FAST);
  (void) my_hash_init(&diagram_open_tables,system_charset_info,32,0,0,
                      (my_hash_get_key) diagram_get_key,0,0);
  diagram_hton->state= SHOW_OPTION_YES;
  diagram_hton->db_type= DB_TYPE_ISAM;
  diagram_hton->create= diagram_create_handler;
  diagram_hton->flags= (HTON_CAN_RECREATE | HTON_SUPPORT_LOG_TABLES | 
                     HTON_NO_PARTITION);

  //plib = dlopen("/usr/lib/mysql/plugin/diagramx_mysql.lx.so",RTLD_LAZY);
  char dlpath[FN_REFLEN];
  strxnmov(dlpath, sizeof(dlpath) - 1, opt_plugin_dir, "/", "diagramx_mysql.lx.so", NullS);
  plib = dlopen(dlpath, RTLD_LAZY);
	if (plib)
	{
		fndsmysql__ = (int (*)(char *,MY_DSPARS *,void *))dlsym(plib, "fndsmysql");
		mylog = (void (*)(int, char*, ...))dlsym(plib, "mylog");
		mylog_ini = (void (*)(int, char**,
					void(*f1)(const char *format, ...),
					void(*f2)(const char *format, ...),
					void(*f3)(const char *format, ...)))dlsym(plib, "mylog_ini");

		if (!fndsmysql__ || !mylog || !mylog_ini)
		{
			sql_print_error("No encontrado [fndsmysq/mylog/mylog_ini] en libreria:%s", dlpath);
			return -1;
		}
		//pre_inicializa_systema = (int (*)(int *,char *[]))dlsym(plib, "pre_inicializa_systema");
	}
	else
	{
		sql_print_error("No puedo abrir la libreria:%s", dlerror());
		return -1;
	}

	const char *load_diagram_groups[]= {"diagram", "server", MYSQL_BASE_VERSION, 0, 0};
	char **argv = orig_argv;
	int argc = orig_argc;
	if(!load_defaults(MYSQL_CONFIG_NAME, load_diagram_groups, &argc, &argv))
		mylog_ini(argc, argv, sql_print_error, sql_print_warning, sql_print_information);
	else
		mylog_ini(0, argv, sql_print_error, sql_print_warning, sql_print_information);

	LOGDEBUG("diagram_init_func:%s", dlpath);
	return 0;
}

static int diagram_done_func(void *p)
{
  my_hash_free(&diagram_open_tables);
  mysql_mutex_destroy(&diagram_mutex);
  if (plib) {
	  dlclose(plib);
	  plib = NULL;
  }

  return 0;
}


#define MAX_TEMPOS 512
static int ntempos = 0;
static MY_DSPARS *tempos[MAX_TEMPOS] = {NULL,NULL,NULL,};

static int 	es_temporalizacion(MY_DSPARS *pars) {
	int i,j;
	for (i = 0;i < ntempos;i++) {
		if (tempos[i]) {
			if (tempos[i]->nc == pars->nc) {
				for (j = 0;j < pars->nc;j++) {
					if (tempos[i]->origtipes[j] != pars->origtipes[j]) {
						break;
					}
				}
				if (j >= pars->nc)
					return 1;
			}
		}
	}
	return 0;
}

/*
  Simple lock controls.
*/
static DIAGRAM_SHARE *get_share(const char *table_name, TABLE *table)
{
  DIAGRAM_SHARE *share;
  char meta_file_name[FN_REFLEN];
  MY_STAT file_stat;                /* Stat information for the data file */
  char *tmp_name;
  uint length;


  int especial = 0;
  const char *pp = table_name;
  while(*pp++);
  while(*--pp != '/');
  if (*pp == '/')
	  pp++;
  if (*pp == '#') {
	  especial = 1;
  }

  mysql_mutex_lock(&diagram_mutex);
  length=(uint) strlen(table_name);

  /*
    If share is not present in the hash, create a new share and
    initialize its members.
  */
  if (!(share=(DIAGRAM_SHARE*) my_hash_search(&diagram_open_tables,
                                           (uchar*) table_name,
                                           length)))
  {
    if (!my_multi_malloc(MYF(MY_WME | MY_ZEROFILL),
                         &share, sizeof(*share),
                         &tmp_name, length+1,
                         NullS))
    {
      mysql_mutex_unlock(&diagram_mutex);
      return NULL;
    }

    share->use_count= 0;
    share->is_log_table= FALSE;
    share->table_name_length= length;
    share->table_name= tmp_name;
    share->crashed= FALSE;
    share->rows_recorded= 0;
    share->update_file_opened= FALSE;
    share->diagram_write_opened= FALSE;
    share->data_file_version= 0;
    strmov(share->table_name, table_name);
    fn_format(share->data_file_name, table_name, "", DIAGRAM_EXT,
              MY_REPLACE_EXT|MY_UNPACK_FILENAME);
    fn_format(meta_file_name, table_name, "", CSM_EXT,
              MY_REPLACE_EXT|MY_UNPACK_FILENAME);

	if (especial) {
		
		if (mysql_file_stat(diagram_key_file_data,
							share->data_file_name, &file_stat, MYF(MY_WME)) != NULL) {
			share->saved_data_file_length= file_stat.st_size;
		}
		
	}

	if (my_hash_insert(&diagram_open_tables, (uchar*) share))
		goto error;
	
    thr_lock_init(&share->lock);
    mysql_mutex_init(diagram_key_mutex_DIAGRAM_SHARE_mutex,
                     &share->mutex, MY_MUTEX_INIT_FAST);

    /*
      Open or create the meta file. In the latter case, we'll get
      an error during read_meta_file and mark the table as crashed.
      Usually this will result in auto-repair, and we will get a good
      meta-file in the end.
    */
	if (especial) {
		
		if (((share->meta_file= mysql_file_open(diagram_key_file_metadata,
												meta_file_name,
												O_RDWR|O_CREAT,
												MYF(MY_WME))) == -1) ||
			read_meta_file(share->meta_file, &share->rows_recorded))
		  share->crashed= TRUE;
		 
	}
	else
		  share->meta_file = -1;
  }

  share->use_count++;
  mysql_mutex_unlock(&diagram_mutex);

  return share;

error:
  mysql_mutex_unlock(&diagram_mutex);
  my_free(share);

  return NULL;
}

/*
  Read DIAGRAM meta-file

  SYNOPSIS
    read_meta_file()
    meta_file   The meta-file filedes
    ha_rows     Pointer to the var we use to store rows count.
                These are read from the meta-file.

  DESCRIPTION

    Read the meta-file info. For now we are only interested in
    rows counf, crashed bit and magic number.

  RETURN
    0 - OK
    non-zero - error occurred
*/

static int read_meta_file(File meta_file, ha_rows *rows)
{
  uchar meta_buffer[META_BUFFER_SIZE];
  uchar *ptr= meta_buffer;


  DBUG_ENTER("ha_diagram::read_meta_file");

  mysql_file_seek(meta_file, 0, MY_SEEK_SET, MYF(0));
  if (mysql_file_read(meta_file, (uchar*)meta_buffer, META_BUFFER_SIZE, 0)
      != META_BUFFER_SIZE)
    DBUG_RETURN(HA_ERR_CRASHED_ON_USAGE);

  /*
    Parse out the meta data, we ignore version at the moment
  */

  ptr+= sizeof(uchar)*2; // Move past header
  *rows= (ha_rows)uint8korr(ptr);
  ptr+= sizeof(ulonglong); // Move past rows
  /*
    Move past check_point, auto_increment and forced_flushes fields.
    They are present in the format, but we do not use them yet.
  */
  ptr+= 3*sizeof(ulonglong);

  /* check crashed bit and magic number */
  if ((meta_buffer[0] != (uchar)DIAGRAM_CHECK_HEADER) ||
      ((bool)(*ptr)== TRUE))
    DBUG_RETURN(HA_ERR_CRASHED_ON_USAGE);

  mysql_file_sync(meta_file, MYF(MY_WME));

  DBUG_RETURN(0);
}


/*
  Write DIAGRAM meta-file

  SYNOPSIS
    write_meta_file()
    meta_file   The meta-file filedes
    ha_rows     The number of rows we have in the datafile.
    dirty       A flag, which marks whether we have a corrupt table

  DESCRIPTION

    Write meta-info the the file. Only rows count, crashed bit and
    magic number matter now.

  RETURN
    0 - OK
    non-zero - error occurred
*/

static int write_meta_file(File meta_file, ha_rows rows, bool dirty)
{
  uchar meta_buffer[META_BUFFER_SIZE];
  uchar *ptr= meta_buffer;


  DBUG_ENTER("ha_diagram::write_meta_file");

  *ptr= (uchar)DIAGRAM_CHECK_HEADER;
  ptr+= sizeof(uchar);
  *ptr= (uchar)DIAGRAM_VERSION;
  ptr+= sizeof(uchar);
  int8store(ptr, (ulonglong)rows);
  ptr+= sizeof(ulonglong);
  memset(ptr, 0, 3*sizeof(ulonglong));
  /*
     Skip over checkpoint, autoincrement and forced_flushes fields.
     We'll need them later.
  */
  ptr+= 3*sizeof(ulonglong);
  *ptr= (uchar)dirty;

  mysql_file_seek(meta_file, 0, MY_SEEK_SET, MYF(0));
  if (mysql_file_write(meta_file, (uchar *)meta_buffer, META_BUFFER_SIZE, 0)
      != META_BUFFER_SIZE)
    DBUG_RETURN(-1);

  mysql_file_sync(meta_file, MYF(MY_WME));

  DBUG_RETURN(0);
}

bool ha_diagram::check_and_repair(THD *thd)
{
  HA_CHECK_OPT check_opt;


  DBUG_ENTER("ha_diagram::check_and_repair");

  
  LOGDEBUG("%s", "check_and_repair");

  check_opt.init();

  DBUG_RETURN(repair(thd, &check_opt));
}

void ha_diagram::ha_libera_ds() {

   LOGDEBUG("%s", "ha_libera_ds"); 

  DSMYSQL("close",&(ds),NULL);

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
  if (ds.origtipes)
	  my_free((void *)ds.origtipes);
  ds.handle = NULL;
  ds.name = NULL;
  ds.offc = NULL;
  ds.lenc = NULL;
  ds.names = NULL;
  ds.origtipes = NULL;
}

int ha_diagram::init_diagram_writer()
{
  DBUG_ENTER("ha_diagram::init_diagram_writer");

  LOGDEBUG("init_diagram_writer::flag:%d:name:%s",ds.flag,ds.name);
  

  if ((ds.flag & 1)) {
	  /*
		Mark the file as crashed. We will set the flag back when we close
		the file. In the case of the crash it will remain marked crashed,
		which enforce recovery.
	  */
	  (void)write_meta_file(share->meta_file, share->rows_recorded, TRUE);

	  if ((share->diagram_write_filedes=
			mysql_file_open(diagram_key_file_data,
							share->data_file_name, O_RDWR|O_APPEND,
							MYF(MY_WME))) == -1)
	  {
		DBUG_PRINT("info", ("Could not open diagram file writes"));
		share->crashed= TRUE;
		DBUG_RETURN(my_errno ? my_errno : -1);
	  }
	  share->diagram_write_opened= TRUE;
  }

  DBUG_RETURN(0);
}


bool ha_diagram::is_crashed() const
{
  
  DBUG_ENTER("ha_diagram::is_crashed");
  
  LOGDEBUG("%s", "is_crashed");
  

  DBUG_RETURN(share->crashed);
}

/*
  Free lock controls.
*/
static int free_share(DIAGRAM_SHARE *share)
{


  DBUG_ENTER("ha_diagram::free_share");
  mysql_mutex_lock(&diagram_mutex);
  int result_code= 0;

  int especial = 0;
  const char *pp = share->table_name;
  while(*pp++);
  while(*--pp != '/');
  if (*pp == '/')
	  pp++;
  if (*pp == '#') {
	  especial = 1;
  }

  if (!--share->use_count){

	if (especial && share->meta_file != -1) {
		/* Write the meta file. Mark it as crashed if needed. */
		(void)write_meta_file(share->meta_file, share->rows_recorded,
							  share->crashed ? TRUE :FALSE);
		if (mysql_file_close(share->meta_file, MYF(0)))
		  result_code= 1;
		if (share->diagram_write_opened)
		{
		  if (mysql_file_close(share->diagram_write_filedes, MYF(0)))
			result_code= 1;
		  share->diagram_write_opened= FALSE;
		}
	}

    my_hash_delete(&diagram_open_tables, (uchar*) share);
    thr_lock_delete(&share->lock);
    mysql_mutex_destroy(&share->mutex);
    my_free(share);
  }
  mysql_mutex_unlock(&diagram_mutex);

  DBUG_RETURN(result_code);
}


/*
  This function finds the end of a line and returns the length
  of the line ending.

  We support three kinds of line endings:
  '\r'     --  Old Mac OS line ending
  '\n'     --  Traditional Unix and Mac OS X line ending
  '\r''\n' --  DOS\Windows line ending
*/

my_off_t find_eoln_buff(Transparent_file *data_buff, my_off_t begin,
                     my_off_t end, int *eoln_len)
{
  *eoln_len= 0;

  for (my_off_t x= begin; x < end; x++)
  {
    /* Unix (includes Mac OS X) */
    if (data_buff->get_value(x) == '\n')
      *eoln_len= 1;
    else
      if (data_buff->get_value(x) == '\r') // Mac or Dos
      {
        /* old Mac line ending */
        if (x + 1 == end || (data_buff->get_value(x + 1) != '\n'))
          *eoln_len= 1;
        else // DOS style ending
          *eoln_len= 2;
      }

    if (*eoln_len)  // end of line was found
      return x;
  }

  return 0;
}


static handler *diagram_create_handler(handlerton *hton,
                                    TABLE_SHARE *table, 
                                    MEM_ROOT *mem_root)
{
  return new (mem_root) ha_diagram(hton, table);
}


ha_diagram::ha_diagram(handlerton *hton, TABLE_SHARE *table_arg)
  :handler(hton, table_arg),
  /*
    These definitions are found in handler.h
    They are not probably completely right.
  */
  current_position(0), next_position(0), local_saved_data_file_length(0),
  file_buff(0), chain_alloced(0), chain_size(DEFAULT_CHAIN_LENGTH),
  local_data_file_version(0), records_is_known(0)
{
  /* Set our original buffers from pre-allocated memory */
  buffer.set((char*)byte_buffer, IO_SIZE, &my_charset_bin);
  chain= chain_buffer;
  file_buff= new Transparent_file();
  init_alloc_root(&blobroot, BLOB_MEMROOT_ALLOC_SIZE, 0);;

	memset(&ds,0,sizeof(ds));
	ds.id = 0x17;
	ds.len = sizeof(ds);
	cindex = 0;
	ds.cindex = 0;
	ds.trozo = 0;
	no_next = false;
	indice_tempo = -1;
	for (int i = 0;i < 16;i++)
		ds.clave[i][0] = -1;
}


/*
  Encode a buffer into the quoted format.
*/

int ha_diagram::encode_quote(uchar *buf)
{
  char attribute_buffer[1024];
  String attribute(attribute_buffer, sizeof(attribute_buffer),
                   &my_charset_bin);

  my_bitmap_map *org_bitmap= dbug_tmp_use_all_columns(table, table->read_set);
  buffer.length(0);

  for (Field **field=table->field ; *field ; field++)
  {
    const char *ptr;
    const char *end_ptr;
    const bool was_null= (*field)->is_null();

    /*
      assistance for backwards compatibility in production builds.
      note: this will not work for ENUM columns.
    */
    if (was_null)
    {
      (*field)->set_default();
      (*field)->set_notnull();
    }

    (*field)->val_str(&attribute,&attribute);

    if (was_null)
      (*field)->set_null();

    if ((*field)->str_needs_quotes())
    {
      ptr= attribute.ptr();
      end_ptr= attribute.length() + ptr;

      buffer.append('"');

      for (; ptr < end_ptr; ptr++)
      {
        if (*ptr == '"')
        {
          buffer.append('\\');
          buffer.append('"');
        }
        else if (*ptr == '\r')
        {
          buffer.append('\\');
          buffer.append('r');
        }
        else if (*ptr == '\\')
        {
          buffer.append('\\');
          buffer.append('\\');
        }
        else if (*ptr == '\n')
        {
          buffer.append('\\');
          buffer.append('n');
        }
        else
          buffer.append(*ptr);
      }
      buffer.append('"');
    }
    else
    {
      buffer.append(attribute);
    }

    buffer.append(',');
  }
  // Remove the comma, add a line feed
  buffer.length(buffer.length() - 1);
  buffer.append('\n');

  //buffer.replace(buffer.length(), 0, "\n", 1);

  dbug_tmp_restore_column_map(table->read_set, org_bitmap);
  return (buffer.length());
}

/*
  chain_append() adds delete positions to the chain that we use to keep
  track of space. Then the chain will be used to cleanup "holes", occurred
  due to deletes and updates.
*/
int ha_diagram::chain_append()
{
  if ( chain_ptr != chain && (chain_ptr -1)->end == current_position)
    (chain_ptr -1)->end= next_position;
  else
  {
    /* We set up for the next position */
    if ((off_t)(chain_ptr - chain) == (chain_size -1))
    {
      my_off_t location= chain_ptr - chain;
      chain_size += DEFAULT_CHAIN_LENGTH;
      if (chain_alloced)
      {
        /* Must cast since my_malloc unlike malloc doesn't have a void ptr */
        if ((chain= (diagram_set *) my_realloc((uchar*)chain,
                                            chain_size, MYF(MY_WME))) == NULL)
          return -1;
      }
      else
      {
        diagram_set *ptr= (diagram_set *) my_malloc(chain_size * sizeof(diagram_set),
                                              MYF(MY_WME));
        memcpy(ptr, chain, DEFAULT_CHAIN_LENGTH * sizeof(diagram_set));
        chain= ptr;
        chain_alloced++;
      }
      chain_ptr= chain + location;
    }
    chain_ptr->begin= current_position;
    chain_ptr->end= next_position;
    chain_ptr++;
  }

  return 0;
}


/*
  Scans for a row.
*/
int ha_diagram::find_current_row(uchar *buf)
{
  my_off_t end_offset, curr_offset= current_position;
  int eoln_len;
  my_bitmap_map *org_bitmap;
  int error;
  bool read_all;


  DS_DBUG_ENTER("ha_diagram::find_current_row", "%s", "");


  free_root(&blobroot, MYF(0));

  /*
    We do not read further then local_saved_data_file_length in order
    not to conflict with undergoing concurrent insert.
  */
  if ((end_offset=
        find_eoln_buff(file_buff, current_position,
                       local_saved_data_file_length, &eoln_len)) == 0)
    DS_DBUG_RETURN(HA_ERR_END_OF_FILE);

  /* We must read all columns in case a table is opened for update */
  read_all= !bitmap_is_clear_all(table->write_set);
  /* Avoid asserts in ::store() for columns that are not going to be updated */
  org_bitmap= dbug_tmp_use_all_columns(table, table->write_set);
  error= HA_ERR_CRASHED_ON_USAGE;

  memset(buf, 0, table->s->null_bytes);

  /*
    Parse the line obtained using the following algorithm
   
    BEGIN
      1) Store the EOL (end of line) for the current row
      2) Until all the fields in the current query have not been 
         filled
         2.1) If the current character is a quote
              2.1.1) Until EOL has not been reached
                     a) If end of current field is reached, move
                        to next field and jump to step 2.3
                     b) If current character is a \\ handle
                        \\n, \\r, \\, \\"
                     c) else append the current character into the buffer
                        before checking that EOL has not been reached.
          2.2) If the current character does not begin with a quote
               2.2.1) Until EOL has not been reached
                      a) If the end of field has been reached move to the
                         next field and jump to step 2.3
                      b) If current character begins with \\ handle
                        \\n, \\r, \\, \\"
                      c) else append the current character into the buffer
                         before checking that EOL has not been reached.
          2.3) Store the current field value and jump to 2)
    TERMINATE
  */  

  for (Field **field=table->field ; *field ; field++)
  {
    char curr_char;
    
    buffer.length(0);
    if (curr_offset >= end_offset)
      goto err;
    curr_char= file_buff->get_value(curr_offset);
    /* Handle the case where the first character is a quote */
    if (curr_char == '"')
    {
      /* Increment past the first quote */
      curr_offset++;

      /* Loop through the row to extract the values for the current field */
      for ( ; curr_offset < end_offset; curr_offset++)
      {
        curr_char= file_buff->get_value(curr_offset);
        /* check for end of the current field */
        if (curr_char == '"' &&
            (curr_offset == end_offset - 1 ||
             file_buff->get_value(curr_offset + 1) == ','))
        {
          /* Move past the , and the " */
          curr_offset+= 2;
          break;
        }
        if (curr_char == '\\' && curr_offset != (end_offset - 1))
        {
          curr_offset++;
          curr_char= file_buff->get_value(curr_offset);
          if (curr_char == 'r')
            buffer.append('\r');
          else if (curr_char == 'n' )
            buffer.append('\n');
          else if (curr_char == '\\' || curr_char == '"')
            buffer.append(curr_char);
          else  /* This could only happed with an externally created file */
          {
            buffer.append('\\');
            buffer.append(curr_char);
          }
        }
        else // ordinary symbol
        {
          /*
            If we are at final symbol and no last quote was found =>
            we are working with a damaged file.
          */
          if (curr_offset == end_offset - 1)
            goto err;
          buffer.append(curr_char);
        }
      }
    }
    else 
    {
      for ( ; curr_offset < end_offset; curr_offset++)
      {
        curr_char= file_buff->get_value(curr_offset);
        /* Move past the ,*/
        if (curr_char == ',')
        {
          curr_offset++;
          break;
        }
        if (curr_char == '\\' && curr_offset != (end_offset - 1))
        {
          curr_offset++;
          curr_char= file_buff->get_value(curr_offset);
          if (curr_char == 'r')
            buffer.append('\r');
          else if (curr_char == 'n' )
            buffer.append('\n');
          else if (curr_char == '\\' || curr_char == '"')
            buffer.append(curr_char);
          else  /* This could only happed with an externally created file */
          {
            buffer.append('\\');
            buffer.append(curr_char);
          }
        }
        else
        {
          /*
             We are at the final symbol and a quote was found for the
             unquoted field => We are working with a damaged field.
          */
          if (curr_offset == end_offset - 1 && curr_char == '"')
            goto err;
          buffer.append(curr_char);
        }
      }
    }

    if (read_all || bitmap_is_set(table->read_set, (*field)->field_index))
    {
      bool is_enum= ((*field)->real_type() ==  MYSQL_TYPE_ENUM);
      /*
        Here CHECK_FIELD_WARN checks that all values in the diagram file are valid
        which is normally the case, if they were written  by
        INSERT -> ha_diagram::write_row. '0' values on ENUM fields are considered
        invalid by Field_enum::store() but it can store them on INSERT anyway.
        Thus, for enums we silence the warning, as it doesn't really mean
        an invalid value.
      */
      if ((*field)->store(buffer.ptr(), buffer.length(), buffer.charset(),
                          is_enum ? CHECK_FIELD_IGNORE : CHECK_FIELD_WARN))
      {
        if (!is_enum)
          goto err;
      }
      if ((*field)->flags & BLOB_FLAG)
      {
        Field_blob *blob= *(Field_blob**) field;
        uchar *src, *tgt;
        uint length, packlength;

        packlength= blob->pack_length_no_ptr();
        length= blob->get_length(blob->ptr);
        memcpy(&src, blob->ptr + packlength, sizeof(char*));
        if (src)
        {
          tgt= (uchar*) alloc_root(&blobroot, length);
          bmove(tgt, src, length);
          memcpy(blob->ptr + packlength, &tgt, sizeof(char*));
        }
      }
    }
  }
  next_position= end_offset + eoln_len;
  error= 0;

err:
  dbug_tmp_restore_column_map(table->write_set, org_bitmap);

  DS_DBUG_RETURN(error);
}
/*********************************************************************/

int ha_diagram::carga_buf(uchar *buf)
{
	  int err = 0;
	  float v_float;
	  int v_int;
	  const unsigned char *ptr = (const unsigned char *)ds.dsbuf;
	  unsigned char *ptrd;

	  //memset(buf, 0, 1);
      LOGDEBUG("%s", "carga_buf");

	  for (int i = 0;i < ds.nc;i++) 
	  {
		  Field *field = table->field[i];
		  int off = ds.offc[i];
		  unsigned int len = ds.lenc[i];
		  int tipo = 0;
		  if (!len || !field) {
			  
			  LOGDEBUG("carga_buf_camponulo::%d:%d",i,len);
			  continue;
		  }
		  if (field->is_null())
		  {
				field->set_default();
				field->set_notnull();
		  }

		  tipo = (int)((len >> 8) & 0xff);
		  len &= 0xff;
		  switch(field->type()) {
			case MYSQL_TYPE_VARCHAR:
			case MYSQL_TYPE_VAR_STRING:
				if (tipo == MYSQL_TYPE_VARCHAR || tipo == MYSQL_TYPE_DATE ) {
					ptrd = buf + field->offset(table->record[0]);
					len = min(field->max_data_length(), len);
					*ptrd = len;
					ptrd++;
					memcpy_oem_1250(ptrd,ptr+off,len);
				}
				break;
			case MYSQL_TYPE_STRING:
				if (tipo == MYSQL_TYPE_VARCHAR || tipo == MYSQL_TYPE_DATE) {
					len = min(field->max_data_length(), len);
					memcpy_oem_1250(buf + field->offset(table->record[0]),ptr+off,len);
				}
				//field->pack(buf + field->offset(table->record[0]), ptr+off, len, false);
				break;
			case MYSQL_TYPE_SHORT:
				if (tipo == MYSQL_TYPE_SHORT) {
					short int ss;
					ss = (short int)ldint((Uchar *)(ptr+off));
					//field->pack(buf + field->offset(table->record[0]), (const unsigned char*)&ss, len, false);
					memcpy(buf + field->offset(table->record[0]), (const unsigned char*)&ss, len);
				}
				break;
			case MYSQL_TYPE_INT24:
			case MYSQL_TYPE_LONGLONG:
			case MYSQL_TYPE_NEWDECIMAL:
			case MYSQL_TYPE_DECIMAL:
				break;
			case MYSQL_TYPE_LONG:
				if (tipo == MYSQL_TYPE_DOUBLE) {
					v_int = (int)(*(double *)(ptr+off));//ldlong((Uchar *)(ptr+off));
					//field->pack(buf + field->offset(table->record[0]), (const unsigned char*)&v_int, sizeof(v_int), true);
					memcpy(buf + field->offset(table->record[0]), (const unsigned char*)&v_int, sizeof(v_int));
				}
				break;
			case MYSQL_TYPE_FLOAT:
				if (tipo == MYSQL_TYPE_DOUBLE) {
					v_float = (float)(*(double *)(ptr+off));
					//field->pack(buf + field->offset(table->record[0]), (const unsigned char*)&v_float, sizeof(v_float), true);
					memcpy(buf + field->offset(table->record[0]), (const unsigned char*)&v_float, sizeof(v_float));
				}
				break;
			case MYSQL_TYPE_DOUBLE:
				if (tipo == MYSQL_TYPE_DOUBLE) {
					//field->pack(buf + field->offset(table->record[0]), ptr+off, len, false);
					memcpy(buf + field->offset(table->record[0]), ptr+off, len);
				}
				break;
			case MYSQL_TYPE_DATE:
				if (tipo == MYSQL_TYPE_DATE || (tipo == MYSQL_TYPE_VARCHAR && len == 8) ) {
					// 3 byte int YYYY�16�32 + MM�32 + DD
					unsigned int fecha = 0;
					char tmp[10];
					memcpy(tmp, ptr+off, 4);
					tmp[4] = 0;
					fecha = atoi(tmp)*16*32;
					memcpy(tmp, ptr+off+4, 2);
					tmp[2] = 0;
					fecha += atoi(tmp)*32;
					memcpy(tmp, ptr+off+6, 2);
					tmp[2] = 0;
					fecha += atoi(tmp);					
					memcpy(buf + field->offset(table->record[0]),(const unsigned char*)&fecha,3);
				}
				break;
			default:
				//field->type()
				//my_error(ER_CHECK_NOT_IMPLEMENTED, MYF(0), "unsuperted field type");
				//err = 1;
				break;
		  };
	}
	return err;
}
/*
  If frm_error() is called in table.cc this is called to find out what file
  extensions exist for this handler.
*/
static const char *ha_diagram_exts[] = {
  DIAGRAM_EXT,
  CSM_EXT,
  NullS
};


const char **ha_diagram::bas_ext() const
{
  return ha_diagram_exts;
}

/*
  Three functions below are needed to enable concurrent insert functionality
  for DIAGRAM engine. For more details see mysys/thr_lock.c
*/

void diagram_get_status(void* param, int concurrent_insert)
{
  LOGDEBUG("%s", "diagram_get_status");

  ha_diagram *diagram= (ha_diagram*) param;
  diagram->get_status();
}

void diagram_update_status(void* param)
{
  LOGDEBUG("%s", "diagram_update_status");
  
  ha_diagram *diagram= (ha_diagram*) param;
  diagram->update_status();
}

/* this should exist and return 0 for concurrent insert to work */
my_bool diagram_check_status(void* param)
{
  
  LOGDEBUG("%s", "diagram_check_status");
  
  return 0;
}

/*
  Save the state of the table

  SYNOPSIS
    get_status()

  DESCRIPTION
    This function is used to retrieve the file length. During the lock
    phase of concurrent insert. For more details see comment to
    ha_diagram::update_status below.
*/

void ha_diagram::get_status()
{
  
  LOGDEBUG("%s", "get_status");
  

  if (share->is_log_table)
  {
    /*
      We have to use mutex to follow pthreads memory visibility
      rules for share->saved_data_file_length
    */
    mysql_mutex_lock(&share->mutex);
    local_saved_data_file_length= share->saved_data_file_length;
    mysql_mutex_unlock(&share->mutex);
    return;
  }
  local_saved_data_file_length= share->saved_data_file_length;
}


/*
  Correct the state of the table. Called by unlock routines
  before the write lock is released.

  SYNOPSIS
    update_status()

  DESCRIPTION
    When we employ concurrent insert lock, we save current length of the file
    during the lock phase. We do not read further saved value, as we don't
    want to interfere with undergoing concurrent insert. Writers update file
    length info during unlock with update_status().

  NOTE
    For log tables concurrent insert works different. The reason is that
    log tables are always opened and locked. And as they do not unlock
    tables, the file length after writes should be updated in a different
    way. For this purpose we need is_log_table flag. When this flag is set
    we call update_status() explicitly after each row write.
*/

void ha_diagram::update_status()
{

  LOGDEBUG("%s", "update_status");
  
  /* correct local_saved_data_file_length for writers */
  share->saved_data_file_length= local_saved_data_file_length;
}


/*
  Open a database file. Keep in mind that tables are caches, so
  this will not be called for every request. Any sort of positions
  that need to be reset should be kept in the ::extra() call.
*/
int ha_diagram::open(const char *name, int mode, uint open_options)
{


  DS_DBUG_ENTER("ha_diagram::open", "%s::%d::%d:",name,mode,open_options);


  if (!(share= get_share(name, table)))
    DS_DBUG_RETURN(HA_ERR_OUT_OF_MEM);

  if (share->crashed && !(open_options & HA_OPEN_FOR_REPAIR))
  {
    free_share(share);
    DS_DBUG_RETURN(HA_ERR_CRASHED_ON_USAGE);
  }



  int especial = 0;
  const char *pp = name;
  while(*pp++);
  while(*--pp != '/');
  if (*pp == '/')
	  pp++;
  if (*pp == '#') {
	  especial = 1;
  }

  
  LOGDEBUG("open::especial:%d",especial);
  

  ha_libera_ds();
	int count = 0;//table->s->fields;  
	Field *field;

	while(table->field[count]) count++;
  if (especial == 1) {
	  ds.flag |= 1;
	  ds.nc = count;
	  ds.open_mode = mode;
	  ds.open_options = open_options;
	  ds.names = (char **)my_malloc(sizeof(char *)*count,MYF(MY_WME));
	  ds.offc = (int *)my_malloc(sizeof(int *)*count,MYF(MY_WME));
	  ds.lenc = (unsigned int *)my_malloc(sizeof(int *)*count,MYF(MY_WME));
	  ds.origtipes = (unsigned int *)my_malloc(sizeof(int *)*count,MYF(MY_WME));
	  for (int i = 0;i < count;i++) {	  
		  field = table->field[i];
		  if (!field) {
			  ds.nc = i;
			  
			  LOGDEBUG("open::nullfield:%d",i);
			  
			  break;
		  }
		  ds.names[i] = (char *)my_malloc(strlen(field->field_name)+1,MYF(MY_WME));
		  strcpy(ds.names[i],field->field_name);
		  ds.offc[i] = 0;
		  ds.lenc[i] = 0;
		  ds.origtipes[i] = ((((unsigned int)field->type()) << 16)&0xffff0000) | (field->pack_length()&0xffff);
	  }
	  ds.name = (char *)my_malloc(strlen(name)+1,MYF(MY_WME));
	  strcpy(ds.name,name);

	  
	  local_data_file_version= share->data_file_version;
	  if ((data_file= mysql_file_open(diagram_key_file_data,
									  share->data_file_name,
									  O_RDONLY, MYF(MY_WME))) == -1)
	  {
		free_share(share);
		ha_libera_ds();
		DS_DBUG_RETURN(my_errno ? my_errno : -1);
	  }
	  
	  if (ntempos < MAX_TEMPOS) {
		  indice_tempo = ntempos;
		  tempos[ntempos++] = &ds;
	  }
  }
  else {

	  ds.nc = count;
	  ds.open_mode = mode;
	  ds.open_options = open_options;
	  ds.names = (char **)my_malloc(sizeof(char *)*count,MYF(MY_WME));
	  ds.offc = (int *)my_malloc(sizeof(int *)*count,MYF(MY_WME));
	  ds.lenc = (unsigned int *)my_malloc(sizeof(int *)*count,MYF(MY_WME));
	  ds.origtipes = (unsigned int *)my_malloc(sizeof(int *)*count,MYF(MY_WME));

	  
	  LOGDEBUG("open::count:%d",count);
	  

	  for (int i = 0;i < count;i++) {	  
		  field = table->field[i];
		  if (!field) {
			  ds.nc = i;
			  
			  LOGDEBUG("open::nullfield:%d",i);
			  
			  break;
		  }
		  ds.names[i] = (char *)my_malloc(strlen(field->field_name)+1,MYF(MY_WME));
		  strcpy(ds.names[i],field->field_name);
		  ds.offc[i] = 0;
		  ds.lenc[i] = 0;
		  ds.origtipes[i] = ((((unsigned int)field->type()) << 16)&0xffff0000) | (field->pack_length()&0xffff);
	  }
	  ds.name = (char *)my_malloc(strlen(name)+1,MYF(MY_WME));
	  strcpy(ds.name,name);

  
	  if (DSMYSQL("open",&ds,NULL) < 0) {
		  
		  LOGDEBUG("open::error:name:%s::%d::%d",name,mode,open_options);
		  
		  ha_libera_ds();
		  DBUG_RETURN(-1);
	  }

	  if (!(ds.handle)) {
			
			  LOGDEBUG("open::error no handle:name:%s::%d::%d",name,mode,open_options);
			  
			  ha_libera_ds();
			  DS_DBUG_RETURN(-1);
	  }

	  stats.records = ds.records;
	  stats.deleted = ds.deleted;
  }

  /*
    Init locking. Pass handler object to the locking routines,
    so that they could save/update local_saved_data_file_length value
    during locking. This is needed to enable concurrent inserts.
  */
  thr_lock_data_init(&share->lock, &lock, (void*) this);
  ref_length= sizeof(my_off_t);

  share->lock.get_status= diagram_get_status;
  share->lock.update_status= diagram_update_status;
  share->lock.check_status= diagram_check_status;

  DS_DBUG_RETURN(0);
}


/*
  Close a database file. We remove ourselves from the shared strucutre.
  If it is empty we destroy it.
*/
int ha_diagram::close(void)
{
  int rc= 0;


  DS_DBUG_ENTER("ha_diagram::close", "%s", "");



  if ((ds.flag & 1)) {
	  rc= mysql_file_close(data_file, MYF(0));
  }
  
  if (indice_tempo > -1) {
	 if (tempos[indice_tempo] == &ds) {
		ntempos--;
		tempos[indice_tempo] = tempos[ntempos];
		tempos[ntempos] = NULL;
	 }
	 else {
		  
		  LOGDEBUG("close::error_control_tempo:%d:%d,%X:%X",indice_tempo,ntempos,tempos[indice_tempo],&ds);
		  
	 }
	 indice_tempo = -1;
  }
  
  if (!(ds.flag & 1))
	    ha_libera_ds();  
  //DBUG_RETURN(rc);
  DS_DBUG_RETURN(free_share(share) || rc);
}

/*
  This is an INSERT. At the moment this handler just seeks to the end
  of the file and appends the data. In an error case it really should
  just truncate to the original position (this is not done yet).
*/
int ha_diagram::write_row(uchar * buf)
{
  int size;


  DS_DBUG_ENTER("ha_diagram::write_row", "%s", "");

  
  if (share->crashed)
      DS_DBUG_RETURN(HA_ERR_CRASHED_ON_USAGE);

  ha_statistic_increment(&SSV::ha_write_count);

  if (table->timestamp_field_type & TIMESTAMP_AUTO_SET_ON_INSERT)
    table->timestamp_field->set_time();

  if ((ds.flag & 1)) {

	  size= encode_quote(buf);

	  if (!share->diagram_write_opened)
		if (init_diagram_writer())
		  DBUG_RETURN(-1);

	   /* use pwrite, as concurrent reader could have changed the position */
	  if (mysql_file_write(share->diagram_write_filedes, (uchar*)buffer.ptr(), size,
						   MYF(MY_WME | MY_NABP))) {
		  
		  LOGDEBUG("write_row_error_especial::name:%s",ds.name);
		  
		DS_DBUG_RETURN(-1);
	  }

	  /* update local copy of the max position to see our own changes */
	  local_saved_data_file_length+= size;

	  /* update shared info */
	  mysql_mutex_lock(&share->mutex);
	  share->rows_recorded++;
	  /* update status for the log tables */
	  if (share->is_log_table)
		update_status();
	  mysql_mutex_unlock(&share->mutex);

	  stats.records++;
  }
  else {
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
			  
			  LOGDEBUG("write_row_camponulo::%d:%X",i,len);
			  
			  continue;
		  }
		  if (field->is_null())
		  {
				field->set_default();
				field->set_notnull();
		  }
		  tipo = (int)((len >> 8) & 0xff);
		  len &= 0xff;
		  switch(field->type()) {
			case MYSQL_TYPE_VARCHAR:
			case MYSQL_TYPE_VAR_STRING:
				if (tipo == MYSQL_TYPE_VARCHAR || tipo == MYSQL_TYPE_DATE) {
					memset(ptr+off,' ',len);
					ptrd = buf + field->offset(table->record[0]);
					len = min((unsigned int)*ptrd, len);
					//*ptrd = len;
					ptrd++;
					memcpy_1250_oem(ptr+off,ptrd,len);
				  
				  LOGDEBUG("write_row::len:%d:varstring:->%-7.7s<-",len,ptr+off);
				  
				}
				break;
			case MYSQL_TYPE_STRING:
				if (tipo == MYSQL_TYPE_VARCHAR || tipo == MYSQL_TYPE_DATE)
					//field->pack(ptr+off, (const unsigned char*)buf + field->offset(table->record[0]), len,false);
					memcpy_1250_oem(ptr+off, (const unsigned char*)buf + field->offset(table->record[0]), len);
				break;
			case MYSQL_TYPE_SHORT:
				if (tipo == MYSQL_TYPE_SHORT) {
					stint(*((short int *)((const unsigned char*)buf + field->offset(table->record[0]))),(Uchar *)(ptr+off));
				}
				break;
			case MYSQL_TYPE_INT24:
			case MYSQL_TYPE_LONGLONG:
			case MYSQL_TYPE_NEWDECIMAL:
			case MYSQL_TYPE_DECIMAL:
				break;
			case MYSQL_TYPE_LONG:
				if (tipo == MYSQL_TYPE_DOUBLE) {
					v_double = (double)(*(int *)(buf + field->offset(table->record[0])));
					//field->pack(ptr+off,(const unsigned char*)&v_double,  sizeof(v_double), true);
					memcpy(ptr+off,(const unsigned char*)&v_double,  sizeof(v_double));
				}
				break;
			case MYSQL_TYPE_FLOAT:
				if (tipo == MYSQL_TYPE_DOUBLE) {
					v_double = (double)(*(float *)(buf + field->offset(table->record[0])));
					//field->pack(ptr+off, (const unsigned char*)&v_double, sizeof(v_double), true);
					memcpy(ptr+off, (const unsigned char*)&v_double, sizeof(v_double));
				}
				break;
			case MYSQL_TYPE_DOUBLE:
				if (tipo == MYSQL_TYPE_DOUBLE)
					//field->pack(ptr+off,(const unsigned char*)buf + field->offset(table->record[0]), len, false);
					memcpy(ptr+off,(const unsigned char*)buf + field->offset(table->record[0]), len);
				break;
			case MYSQL_TYPE_DATE:
				if (tipo == MYSQL_TYPE_DATE || (tipo == MYSQL_TYPE_VARCHAR && len == 8) ) {
					// 3 byte int YYYY�16�32 + MM�32 + DD
					unsigned int fecha = 0;
					unsigned int mes,dia;
					char tmp[10];
					memcpy((char *)&fecha,(const unsigned char*)buf + field->offset(table->record[0]), 3);
					dia = fecha%32;
					fecha /= 32;
					mes = fecha%16;
					fecha /= 16;
					sprintf(tmp,"%04d%02d%02d",fecha,mes,dia);
					memcpy(ptr+off,(const unsigned char*)tmp, 8);
				}
				break;
			default:
				//my_error(ER_CHECK_NOT_IMPLEMENTED, MYF(0), "unsuperted field type");
				//err = 1;
				break;
		  };
	  }

	  if (DSMYSQL("write_row",&ds,NULL) < 0) {
		  
		  LOGDEBUG("write_row_error::name:%s",ds.name);
		  
		  DS_DBUG_RETURN(-1);
	  }
  }
  DS_DBUG_RETURN(0);
}


int ha_diagram::open_update_temp_file_if_needed()
{
  char updated_fname[FN_REFLEN];


  
  LOGDEBUG("%s", "open_update_temp_file_if_needed");
  

  if (!share->update_file_opened)
  {
    if ((update_temp_file=
           mysql_file_create(diagram_key_file_update,
                             fn_format(updated_fname, share->table_name,
                                       "", CSN_EXT,
                                       MY_REPLACE_EXT | MY_UNPACK_FILENAME),
                             0, O_RDWR | O_TRUNC, MYF(MY_WME))) < 0)
      return 1;
    share->update_file_opened= TRUE;
    temp_file_length= 0;
  }
  return 0;
}

/*
  This is called for an update.
  Make sure you put in code to increment the auto increment, also
  update any timestamp data. Currently auto increment is not being
  fixed since autoincrements have yet to be added to this table handler.
  This will be called in a table scan right before the previous ::rnd_next()
  call.
*/
int ha_diagram::update_row(const uchar * old_data, uchar * new_data)
{
  int size;
  int rc= 0;


  DS_DBUG_ENTER("ha_diagram::update_row", "%s", "");
  
  ha_statistic_increment(&SSV::ha_update_count);

  if (table->timestamp_field_type & TIMESTAMP_AUTO_SET_ON_UPDATE)
    table->timestamp_field->set_time();

  size= encode_quote(new_data);

  if ((ds.flag & 1)) {

	  /*
		During update we mark each updating record as deleted 
		(see the chain_append()) then write new one to the temporary data file. 
		At the end of the sequence in the rnd_end() we append all non-marked
		records from the data file to the temporary data file then rename it.
		The temp_file_length is used to calculate new data file length.
	  */
	  rc = -1;
	  if (chain_append())
		goto err;

	  if (open_update_temp_file_if_needed())
		goto err;

	  if (mysql_file_write(update_temp_file, (uchar*)buffer.ptr(), size,
						   MYF(MY_WME | MY_NABP)))
		goto err;
	  temp_file_length+= size;
	  rc= 0;

	  /* UPDATE should never happen on the log tables */
	  DBUG_ASSERT(!share->is_log_table);
  }
  else {
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
			  
			  LOGDEBUG("update_row_camponulo::%d:%X",i,len);
			  
			  continue;
		  }
		  if (field->is_null())
		  {
				field->set_default();
				field->set_notnull();
		  }
		  tipo = (int)((len >> 8) & 0xff);
		  len &= 0xff;
		  switch(field->type()) {
			case MYSQL_TYPE_VARCHAR:
			case MYSQL_TYPE_VAR_STRING:
				if (tipo == MYSQL_TYPE_VARCHAR || tipo == MYSQL_TYPE_DATE ) {
					memset(ptr+off,' ',len);
					ptrd = buf + field->offset(table->record[0]);
					len = min((unsigned int)*ptrd, len);
					//*ptrd = len;
					ptrd++;
					memcpy_1250_oem(ptr+off,ptrd,len);
				  
				  LOGDEBUG("update_row::len:%d:varstring:->%-7.7s<-",len,ptr+off);
				  

				}
				break;
			case MYSQL_TYPE_STRING:
				if (tipo == MYSQL_TYPE_VARCHAR || tipo == MYSQL_TYPE_DATE)
					//field->pack(ptr+off, (const unsigned char*)buf + field->offset(table->record[0]), len, false);
					memcpy_1250_oem(ptr+off, (const unsigned char*)buf + field->offset(table->record[0]), len);
				break;
			case MYSQL_TYPE_SHORT:
				if (tipo == MYSQL_TYPE_SHORT)
					stint(*((short int *)((const unsigned char*)buf + field->offset(table->record[0]))),(Uchar *)(ptr+off));
				break;
			case MYSQL_TYPE_INT24:
			case MYSQL_TYPE_LONGLONG:
			case MYSQL_TYPE_NEWDECIMAL:
			case MYSQL_TYPE_DECIMAL:
				break;
			case MYSQL_TYPE_LONG:
				if (tipo == MYSQL_TYPE_DOUBLE) {
					v_double = (double)(*(int *)(buf + field->offset(table->record[0])));
					//field->pack(ptr+off,(const unsigned char*)&v_double,  sizeof(v_double), true);
					memcpy(ptr+off,(const unsigned char*)&v_double,  sizeof(v_double));
				}
				break;
			case MYSQL_TYPE_FLOAT:
				if (tipo == MYSQL_TYPE_DOUBLE) {
					v_double = (double)(*(float *)(buf + field->offset(table->record[0])));
					//field->pack(ptr+off, (const unsigned char*)&v_double, sizeof(v_double), true);
					memcpy(ptr+off, (const unsigned char*)&v_double, sizeof(v_double));
				}
				break;
			case MYSQL_TYPE_DOUBLE:
				if (tipo == MYSQL_TYPE_DOUBLE)
					//field->pack(ptr+off,(const unsigned char*)buf + field->offset(table->record[0]), len, false);
					memcpy(ptr+off,(const unsigned char*)buf + field->offset(table->record[0]), len);
				break;
			case MYSQL_TYPE_DATE:		
				if (tipo == MYSQL_TYPE_DATE || (tipo == MYSQL_TYPE_VARCHAR && len == 8) ) {
					// 3 byte int YYYY�16�32 + MM�32 + DD
					unsigned int fecha = 0;
					unsigned int mes,dia;
					char tmp[10];
					memcpy((char *)&fecha,(const unsigned char*)buf + field->offset(table->record[0]), 3);
					dia = fecha%32;
					fecha /= 32;
					mes = fecha%16;
					fecha /= 16;
					sprintf(tmp,"%04d%02d%02d",fecha,mes,dia);
					memcpy(ptr+off,(const unsigned char*)tmp, 8);
				}
				break;
			default:
				//my_error(ER_CHECK_NOT_IMPLEMENTED, MYF(0), "unsuperted field type");
				//err = 1;
				break;
		  };
	  }

	  int pos = ds.pos;
	  if (DSMYSQL("lock_reg",&ds,NULL) < 0) {

			LOGDEBUG("%s", "update_row:-1:lock!!!");

		  if (DSMYSQL("lock_reg",&ds,NULL) < 0) {
			  DS_DBUG_RETURN(-1);
		  }
			
     	  LOGDEBUG("%s", "update_row:0:lock recuperado");
			
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
			  
			  LOGDEBUG("update_row_camponulo::%d:%X",i,len);
			  
			  continue;
		  }
		  tipo = (int)((len >> 8) & 0xff);
		  len &= 0xff;
		  switch(field->type()) {
			case MYSQL_TYPE_VARCHAR:
			case MYSQL_TYPE_VAR_STRING:
				if (tipo == MYSQL_TYPE_VARCHAR || tipo == MYSQL_TYPE_DATE) {
					memset(ptr+off,' ',len);
					ptrd = buf + field->offset(table->record[0]);
					len = min((unsigned int)*ptrd, len);
					//*ptrd = len;
					ptrd++;
					memcpy_1250_oem(ptr+off,ptrd,len);
				  
				  LOGDEBUG("update_row::len:%d:varstring:->%-7.7s<-",len,ptr+off);
				  

				}
				break;
			case MYSQL_TYPE_STRING:
				if (tipo == MYSQL_TYPE_VARCHAR || tipo == MYSQL_TYPE_DATE)
					//field->pack(ptr+off, (const unsigned char*)buf + field->offset(table->record[0]), len,false);
					memcpy_1250_oem(ptr+off, (const unsigned char*)buf + field->offset(table->record[0]), len);
				break;
			case MYSQL_TYPE_SHORT:
				if (tipo == MYSQL_TYPE_SHORT)
					stint(*((short int *)((const unsigned char*)buf + field->offset(table->record[0]))),(Uchar *)(ptr+off));
				break;
			case MYSQL_TYPE_INT24:
			case MYSQL_TYPE_LONGLONG:
			case MYSQL_TYPE_NEWDECIMAL:
			case MYSQL_TYPE_DECIMAL:
				break;
			case MYSQL_TYPE_LONG:
				if (tipo == MYSQL_TYPE_DOUBLE) {
					v_double = (double)(*(int *)(buf + field->offset(table->record[0])));
					//field->pack(ptr+off,(const unsigned char*)&v_double,  sizeof(v_double), true);
					memcpy(ptr+off,(const unsigned char*)&v_double,  sizeof(v_double));
				}
				break;
			case MYSQL_TYPE_FLOAT:
				if (tipo == MYSQL_TYPE_DOUBLE) {
					v_double = (double)(*(float *)(buf + field->offset(table->record[0])));
					//field->pack(ptr+off, (const unsigned char*)&v_double, sizeof(v_double), true);
					memcpy(ptr+off, (const unsigned char*)&v_double, sizeof(v_double));
				}
				break;
			case MYSQL_TYPE_DOUBLE:
				if (tipo == MYSQL_TYPE_DOUBLE)
					//field->pack(ptr+off,(const unsigned char*)buf + field->offset(table->record[0]), len, false);
					memcpy(ptr+off,(const unsigned char*)buf + field->offset(table->record[0]), len);
				break;
			case MYSQL_TYPE_DATE:		
				if (tipo == MYSQL_TYPE_DATE || (tipo == MYSQL_TYPE_VARCHAR && len == 8) ) {
					// 3 byte int YYYY�16�32 + MM�32 + DD
					unsigned int fecha = 0;
					unsigned int mes,dia;
					char tmp[10];
					memcpy((char *)&fecha,(const unsigned char*)buf + field->offset(table->record[0]), 3);
					dia = fecha%32;
					fecha /= 32;
					mes = fecha%16;
					fecha /= 16;
					sprintf(tmp,"%04d%02d%02d",fecha,mes,dia);
					memcpy(ptr+off,(const unsigned char*)tmp, 8);
				}
				break;
			default:
				//my_error(ER_CHECK_NOT_IMPLEMENTED, MYF(0), "unsuperted field type");
				//err = 1;
				break;
		  };
	  }

	  if (DSMYSQL("update_row",&ds,NULL) < 0) {
		  

		  LOGDEBUG("%s", "update_row:-1:fallo!!!");			
        
		  DS_DBUG_RETURN(-1);
	  }

  }

err:
  DBUG_PRINT("info",("rc = %d", rc));
  DS_DBUG_RETURN(rc);
}


/*
  Deletes a row. First the database will find the row, and then call this
  method. In the case of a table scan, the previous call to this will be
  the ::rnd_next() that found this row.
  The exception to this is an ORDER BY. This will cause the table handler
  to walk the table noting the positions of all rows that match a query.
  The table will then be deleted/positioned based on the ORDER (so RANDOM,
  DESC, ASC).
*/
int ha_diagram::delete_row(const uchar * _buf)
{


  DS_DBUG_ENTER("ha_diagram::delete_row", "%d", cindex);


  ha_statistic_increment(&SSV::ha_delete_count);

  unsigned char *buf = (unsigned char *)_buf;

  if ((ds.flag & 1)) {

	  if (chain_append())
		DS_DBUG_RETURN(-1);

	  stats.records--;
	  /* Update shared info */
	  DBUG_ASSERT(share->rows_recorded);
	  mysql_mutex_lock(&share->mutex);
	  share->rows_recorded--;
	  mysql_mutex_unlock(&share->mutex);

	  /* DELETE should never happen on the log table */
	  DBUG_ASSERT(!share->is_log_table);
  }
  else {
	  int i;
	  double v_double;
	  unsigned char *ptr = (unsigned char *)ds.dsbuf;
	  unsigned char *ptrd;
	  for (i = 0;i < ds.nc;i++) 
	  {
		  Field *field = table->field[i];
		  int off = ds.offc[i];
		  unsigned int len = ds.lenc[i];
		  int tipo = 0;
		  if (!len || !field) {
			  
			  LOGDEBUG("delete_row_camponulo::%d:%X",i,len);
			  
			  continue;
		  }
		  tipo = (int)((len >> 8) & 0xff);
		  len &= 0xff;
		  switch(field->type()) {
			case MYSQL_TYPE_VARCHAR:
			case MYSQL_TYPE_VAR_STRING:
				if (tipo == MYSQL_TYPE_VARCHAR || tipo == MYSQL_TYPE_DATE) {
					memset(ptr+off,' ',len);
					ptrd = buf + field->offset(table->record[0]);
					len = min((unsigned int)*ptrd, len);
					//*ptrd = len;
					ptrd++;
					memcpy_1250_oem(ptr+off,ptrd,len);
				  
				  LOGDEBUG("delete_row::len:%d:varstring:->%-7.7s<-",len,ptr+off);
				  

				}
				break;
			case MYSQL_TYPE_STRING:
				if (tipo == MYSQL_TYPE_VARCHAR || tipo == MYSQL_TYPE_DATE)
					//field->pack(ptr+off, (const unsigned char*)buf + field->offset(table->record[0]), len,false);
					memcpy_1250_oem(ptr+off, (const unsigned char*)buf + field->offset(table->record[0]), len);
				break;
			case MYSQL_TYPE_SHORT:
				if (tipo == MYSQL_TYPE_SHORT)
					stint(*((short int *)((const unsigned char*)buf + field->offset(table->record[0]))),(Uchar *)(ptr+off));
				break;
			case MYSQL_TYPE_INT24:
			case MYSQL_TYPE_LONGLONG:
			case MYSQL_TYPE_NEWDECIMAL:
			case MYSQL_TYPE_DECIMAL:
				break;
			case MYSQL_TYPE_LONG:
				if (tipo == MYSQL_TYPE_DOUBLE) {
					v_double = (double)(*(int *)(buf + field->offset(table->record[0])));
					//field->pack(ptr+off,(const unsigned char*)&v_double,  sizeof(v_double), true);
					memcpy(ptr+off,(const unsigned char*)&v_double,  sizeof(v_double));
				}
				break;
			case MYSQL_TYPE_FLOAT:
				if (tipo == MYSQL_TYPE_DOUBLE) {
					v_double = (double)(*(float *)(buf + field->offset(table->record[0])));
					//field->pack(ptr+off, (const unsigned char*)&v_double, sizeof(v_double), true);
					memcpy(ptr+off, (const unsigned char*)&v_double, sizeof(v_double));
				}
				break;
			case MYSQL_TYPE_DOUBLE:
				if (tipo == MYSQL_TYPE_DOUBLE)
					//field->pack(ptr+off,(const unsigned char*)buf + field->offset(table->record[0]), len, false);
					memcpy(ptr+off,(const unsigned char*)buf + field->offset(table->record[0]), len);
				break;
			case MYSQL_TYPE_DATE:		
				if (tipo == MYSQL_TYPE_DATE || (tipo == MYSQL_TYPE_VARCHAR && len == 8) ) {
					// 3 byte int YYYY�16�32 + MM�32 + DD
					unsigned int fecha = 0;
					unsigned int mes,dia;
					char tmp[10];
					memcpy((char *)&fecha,(const unsigned char*)buf + field->offset(table->record[0]), 3);
					dia = fecha%32;
					fecha /= 32;
					mes = fecha%16;
					fecha /= 16;
					sprintf(tmp,"%04d%02d%02d",fecha,mes,dia);
					memcpy(ptr+off,(const unsigned char*)tmp, 8);
				}
				break;
			default:
				//my_error(ER_CHECK_NOT_IMPLEMENTED, MYF(0), "unsuperted field type");
				//err = 1;
				break;
		  };
	  }

	  if (DSMYSQL("delete_row",&ds,NULL) < 0)
		  DS_DBUG_RETURN(-1);
  }

  DS_DBUG_RETURN(0);
}


int ha_diagram::index_init(uint inx, bool sorted) {
	
	LOGDEBUG("index_init::index:%d:sorted:%d",inx,(int)sorted);
	
	ds.cindex = inx;
	cindex = inx;
	ds.trozo = 0;
	active_index= inx;
	//ds.sorted = (int)sorted;
    DBUG_RETURN(0);
}

int ha_diagram::index_end() {


	
	LOGDEBUG("%s", "index_end");
	
	active_index= MAX_KEY;
    DBUG_RETURN(0);
}

/**
  @brief
  Positions an index cursor to the index specified in the handle. Fetches the
  row if available. If the key value is null, begin at the first key of the
  index.
*/
/*
 *  Versión original
 * 
int ha_diagram::index_read_map(uchar *_buf, const uchar *key,
                               key_part_map keypart_map,
                               enum ha_rkey_function find_flag)
{
  int rc = 0;


  DS_DBUG_ENTER("ha_diagram::index_read", "index:%d:find_flag:%d",cindex,find_flag);
  
 
  MYSQL_INDEX_READ_ROW_START(table_share->db.str, table_share->table_name.str);

  int err = 0;
  int comparaigual = 0;

  if (key)
  {
	int i,kk;
	double v_double;
	unsigned char *ptr = (unsigned char *)ds.dsbuf;
	unsigned char *ptrd;
	unsigned char *buf = (unsigned char *)key;
	int offs = 0;
	int filtramap = 1;
	ds.trozo = 0;
	for (kk = 0;kk < 16;kk++) 
	{
		if (ds.clave[cindex][kk] < 0)
			break;
		if (!(keypart_map & filtramap)) {
			
			
			LOGDEBUG("index_read_map_truncado::%d:%X",kk,keypart_map);
			
			
			ds.trozo = kk;
			if (find_flag == HA_READ_KEY_EXACT) {
				find_flag = HA_READ_KEY_OR_NEXT;
				comparaigual = 1;
			}		
			break;
		}
		filtramap <<= 1;
		i = (ds.clave[cindex][kk] & 0xffff);
		Field *field = table->field[i];
		int off = ds.offc[i];
		unsigned int len = ds.lenc[i];
		int tipo = 0;	
		if (!len || !field) {
			continue;
		}
		tipo = (int)((len >> 8) & 0xff);
		len &= 0xff;
		offs++;
		switch(field->type()) {
		case MYSQL_TYPE_VARCHAR:
		case MYSQL_TYPE_VAR_STRING:
			if (tipo == MYSQL_TYPE_VARCHAR) {
				if (!buf[offs] && ds.find_flag == find_flag) {
                    find_flag  = HA_READ_KEY_OR_NEXT;
				}
				memset(ptr+off,' ',len);
				ptrd = buf + offs;
				len = min((unsigned int)*ptrd, len);
				// *ptrd = len;
				ptrd++;
				memcpy_1250_oem(ptr+off,ptrd,len);
			}
			break;
		case MYSQL_TYPE_STRING:
			if (tipo == MYSQL_TYPE_VARCHAR || tipo == MYSQL_TYPE_DATE) {
				if (!buf[offs] && ds.find_flag == find_flag) {
                    find_flag  = HA_READ_KEY_OR_NEXT;
				}
				//field->pack(ptr+off, (const unsigned char*)buf + offs, len, false);
				memcpy_1250_oem(ptr+off, (const unsigned char*)buf + offs, len);
			}
			break;
		case MYSQL_TYPE_SHORT:
			if (tipo == MYSQL_TYPE_SHORT)
				stint(*((short int *)((const unsigned char*)buf + offs)),(Uchar *)(ptr+off));
			break;
		case MYSQL_TYPE_INT24:
		case MYSQL_TYPE_LONGLONG:
		case MYSQL_TYPE_NEWDECIMAL:
		case MYSQL_TYPE_DECIMAL:
			break;
		case MYSQL_TYPE_LONG:
			if (tipo == MYSQL_TYPE_DOUBLE) {
				v_double = (double)(*(int *)(buf + offs));
				//field->pack(ptr+off,(const unsigned char*)&v_double,  sizeof(v_double), true);
				memcpy(ptr+off,(const unsigned char*)&v_double,  sizeof(v_double));
			}
			break;
		case MYSQL_TYPE_FLOAT:
			if (tipo == MYSQL_TYPE_DOUBLE) {
				v_double = (double)(*(float *)(buf + offs));
				//field->pack(ptr+off, (const unsigned char*)&v_double, sizeof(v_double), true);
				memcpy(ptr+off, (const unsigned char*)&v_double, sizeof(v_double));
			}
			break;
		case MYSQL_TYPE_DOUBLE:
			if (tipo == MYSQL_TYPE_DOUBLE) {
				//field->pack(ptr+off,(const unsigned char*)buf + offs, len, false);
				memcpy(ptr+off,(const unsigned char*)buf + offs, len);
			}
			break;
		case MYSQL_TYPE_DATE:		
				if (tipo == MYSQL_TYPE_DATE || (tipo == MYSQL_TYPE_VARCHAR && len == 8) ) {
					// 3 byte int YYYY�16�32 + MM�32 + DD
					unsigned int fecha = 0;
					unsigned int mes,dia;
					char tmp[10];
					memcpy((char *)&fecha,(const unsigned char*)buf + offs, 3);
					dia = fecha%32;
					fecha /= 32;
					mes = fecha%16;
					fecha /= 16;
					sprintf(tmp,"%04d%02d%02d",fecha,mes,dia);
					memcpy(ptr+off,(const unsigned char*)tmp, 8);
				}
			break;
		default:
			//my_error(ER_CHECK_NOT_IMPLEMENTED, MYF(0), "unsuperted field type");
			//err = 1;
			break;
		};
		offs += field->pack_length();
	}
	if (!kk)
		ds.find_flag  = 9999;
	else
		ds.find_flag = (int)find_flag;
  }
  else {
	ds.find_flag  = 9999;
  }

  if (DSMYSQL("index_read_map",&ds,NULL) < 0) {
		
		LOGDEBUG("%s", "index_read_map::HA_ERR_KEY_NOT_FOUND");
		

		rc= HA_ERR_KEY_NOT_FOUND ;
		MYSQL_INDEX_READ_ROW_DONE(rc);
		DS_DBUG_RETURN(rc);
  }
  if (comparaigual && ds.trozo) {
	    rc = comprueba_same(key,999999999);
		if (rc != 0) {
			rc= HA_ERR_KEY_NOT_FOUND ;
			MYSQL_INDEX_READ_ROW_DONE(rc);
			DS_DBUG_RETURN(rc);
		}
  }
  err = carga_buf(_buf);
  MYSQL_INDEX_READ_ROW_DONE(rc);

  
  LOGDEBUG("index_read_map::OK:%d:",err);

  
  
  DS_DBUG_RETURN(err);
}
*/

/* 
 * 
 * Versión modificada, con busqueda último 
 * 
 * */
int ha_diagram::index_read_map(uchar *_buf, const uchar *key,
                               key_part_map keypart_map,
                               enum ha_rkey_function find_flag)
{
	int rc = 0;

	DS_DBUG_ENTER("ha_diagram::index_read_map", "index:%d:find_flag:%d", cindex, find_flag);

	MYSQL_INDEX_READ_ROW_START(table_share->db.str, table_share->table_name.str);

	int err = 0;
	int comparaigual = 0;
	int maximo = 0;

	if (key)
	{
		int i,kk;
		double v_double;
		unsigned char *ptr = (unsigned char *)ds.dsbuf;
		unsigned char *ptrd;
		unsigned char *buf = (unsigned char *)key;
		int offs = 0;
		int filtramap = 1;
		ds.trozo = 0;
		for (kk = 0;kk < 16;kk++)
		{
			if (ds.clave[cindex][kk] < 0)
				break;

			if (!(keypart_map & filtramap))
			{
				ds.trozo = kk;
/*
					find_flag == HA_READ_KEY_OR_NEXT 			//1
					find_flag == HA_READ_PREFIX   				//5
					find_flag == HA_READ_PREFIX_LAST 			//6
					find_flag == HA_READ_PREFIX_LAST_OR_PREV	//7
*/
				if (find_flag == HA_READ_PREFIX_LAST_OR_PREV)
				{
					ds.trozo = 8888;
					if (!maximo)
						LOGDEBUG("index_read_map(MAXIMO):ncampo:%d:keypart_map:%lu:find_flag:%d", kk, keypart_map, find_flag);
					maximo = 1;
				}

				if(!maximo)
				{
					if (find_flag == HA_READ_KEY_EXACT)  //0
					{
						find_flag = HA_READ_KEY_OR_NEXT; //1
						comparaigual = 1;
					}
					break;
					LOGDEBUG("index_read_map(truncado):ncampo:%d:keypart_map:%lu:compara:%d", kk, keypart_map, comparaigual);
				}
			}
			filtramap <<= 1;
			i = (ds.clave[cindex][kk] & 0xffff);
			Field *field = table->field[i];
			int off = ds.offc[i];
			unsigned int len = ds.lenc[i];
			int tipo = 0;
			if (!len || !field)
				continue;

			tipo = (int)((len >> 8) & 0xff);
			len &= 0xff;
			offs++;
			switch(field->type()) {
			case MYSQL_TYPE_VARCHAR:
			case MYSQL_TYPE_VAR_STRING:
				if (tipo == MYSQL_TYPE_VARCHAR)
				{
					if (!buf[offs] && ds.find_flag == find_flag) {
						find_flag  = HA_READ_KEY_OR_NEXT;
					}
					memset(ptr+off,' ',len);
					ptrd = buf + offs;
					len = min((unsigned int)*ptrd, len);
					//*ptrd = len;
					ptrd++;
					if (maximo)
						memset(ptr+off, 'z', len);
					else
						memcpy_1250_oem(ptr+off, ptrd, len);
				}
				break;
			case MYSQL_TYPE_STRING:
				if (tipo == MYSQL_TYPE_VARCHAR || tipo == MYSQL_TYPE_DATE)
				{
					if (!buf[offs] && ds.find_flag == find_flag) {
						find_flag  = HA_READ_KEY_OR_NEXT;
					}
					//field->pack(ptr+off, (const unsigned char*)buf + offs, len, false);
					if (maximo)
						memset(ptr+off, 'z', len);
					else
						memcpy_1250_oem(ptr+off, (const unsigned char*)buf + offs, len);
				}
				break;
			case MYSQL_TYPE_SHORT:
				if (tipo == MYSQL_TYPE_SHORT)
				{
					short int max = (short int)ds.valor_max[cindex][kk];
					ptrd = buf + offs;
					if (maximo)
						ptrd = (unsigned char*)&max;
					stint(*((short int *)((const unsigned char*)ptrd)),(Uchar *)(ptr+off));
				}
				break;
			case MYSQL_TYPE_INT24:
			case MYSQL_TYPE_LONGLONG:
			case MYSQL_TYPE_NEWDECIMAL:
			case MYSQL_TYPE_DECIMAL:
				break;
			case MYSQL_TYPE_LONG:
				if (tipo == MYSQL_TYPE_DOUBLE)
				{
					v_double = (double)(*(int *)(buf + offs));
					if (maximo)
						v_double = ds.valor_max[cindex][kk];
					//field->pack(ptr+off,(const unsigned char*)&v_double,  sizeof(v_double), true);
					memcpy(ptr+off,(const unsigned char*)&v_double,  sizeof(v_double));
				}
				break;
			case MYSQL_TYPE_FLOAT:
				if (tipo == MYSQL_TYPE_DOUBLE)
				{
					v_double = (double)(*(float *)(buf + offs));
					if (maximo)
						v_double = ds.valor_max[cindex][kk];
					//field->pack(ptr+off, (const unsigned char*)&v_double, sizeof(v_double), true);
					memcpy(ptr+off, (const unsigned char*)&v_double, sizeof(v_double));
				}
				break;
			case MYSQL_TYPE_DOUBLE:
				if (tipo == MYSQL_TYPE_DOUBLE)
				{
					v_double = ds.valor_max[cindex][kk];
					//field->pack(ptr+off,(const unsigned char*)buf + offs, len, false);
					ptrd = buf + offs;
					if (maximo)
						ptrd = (unsigned char*)&v_double;
					memcpy(ptr+off,(const unsigned char*)ptrd, len);
				}
				break;
			case MYSQL_TYPE_DATE:
				if (tipo == MYSQL_TYPE_DATE || (tipo == MYSQL_TYPE_VARCHAR && len == 8) )
				{
					// 3 byte int YYYY�16�32 + MM�32 + DD
					unsigned int fecha = 0;
					unsigned int mes,dia;
					char tmp[10];
					memcpy((char *)&fecha,(const unsigned char*)buf + offs, 3);
					dia = fecha%32;
					fecha /= 32;
					mes = fecha%16;
					fecha /= 16;
					if (maximo)
						sprintf(tmp,"%04d%02d%02d", 9999, 31, 12);
					else
						sprintf(tmp,"%04d%02d%02d", fecha, mes, dia);
					memcpy(ptr+off,(const unsigned char*)tmp, 8);
				}
				break;
			default:
				//my_error(ER_CHECK_NOT_IMPLEMENTED, MYF(0), "unsuperted field type");
				//err = 1;
				break;
			};
			offs += field->pack_length();
		}
		if (!kk)
			ds.find_flag  = 9999;
		else
			ds.find_flag = (int)find_flag;
	}
	else {
		ds.find_flag  = 9999;
	}

	if (DSMYSQL("index_read_map",&ds,NULL) < 0)
	{
		LOGDEBUG("index_read_map:%s:%s", "HA_ERR_KEY_NOT_FOUND_1", ds.name);
		rc= HA_ERR_KEY_NOT_FOUND ;
		MYSQL_INDEX_READ_ROW_DONE(rc);
		DS_DBUG_RETURN(rc);
	}
	if (comparaigual && ds.trozo)
	{
		rc = comprueba_same(key,999999999);
		if (rc != 0)
		{
			LOGDEBUG("index_read_map:%s:%s", "HA_ERR_KEY_NOT_FOUND_2", ds.name);
			rc= HA_ERR_KEY_NOT_FOUND ;
			MYSQL_INDEX_READ_ROW_DONE(rc);
			DS_DBUG_RETURN(rc);
		}
	}
	err = carga_buf(_buf);
	MYSQL_INDEX_READ_ROW_DONE(rc);

	/*
  rc= HA_ERR_WRONG_COMMAND;
  MYSQL_INDEX_READ_ROW_DONE(rc);
  DS_DBUG_RETURN(rc);
	 */

	DS_DBUG_RETURN(err);
}


int ha_diagram::index_read(uchar *_buf, const uchar *key,uint key_len,                               
                               enum ha_rkey_function find_flag)
{
    LOGDEBUG("%s", "index_read");
    
	return index_read_map(_buf,key,(key_part_map)0xffffffff,find_flag);
}


int ha_diagram::comprueba_same(const uchar *key,uint length)
{
 int rc = 0;
 
 LOGDEBUG("%s", "comprueba_same");
 
 if (key)
  {
	int i,kk;
	unsigned char *ptr = (unsigned char *)ds.dsbuf;
	unsigned char *ptrd;
	unsigned char *buf = (unsigned char *)key;
	int offs = 0;
	double v_double;
	for (kk = 0;kk < 16;kk++) 
	{
		if (ds.clave[cindex][kk] < 0 || length <= 0) {
			break;
		}
		if (ds.trozo && kk >= ds.trozo) {
			break;
		}

		i = (ds.clave[cindex][kk] & 0xffff);
		Field *field = table->field[i];
		int off = ds.offc[i];
		unsigned int len = ds.lenc[i];
		int tipo = 0;	

		if (!len || !field) {
			continue;
		}
		tipo = (int)((len >> 8) & 0xff);
		len &= 0xff;
		if (len > length) {
			break;
		}

		offs++;
		switch(field->type()) {
		case MYSQL_TYPE_VARCHAR:
		case MYSQL_TYPE_VAR_STRING:
			if (tipo == MYSQL_TYPE_VARCHAR) {
				//memset(ptr+off,' ',len);
				ptrd = buf + offs;
				//len = min((unsigned int)*ptrd, len);
				//*ptrd = len;
				ptrd++;
				if (memcmp(ptr+off,ptrd,len)) {
					rc= HA_ERR_END_OF_FILE;
				}
						
                LOGDEBUG("comprueba_same:%d:len:%d:pack_length:%d:ptr+off:%-10.10s:buf+offs:%-10.10s",rc,len,field->pack_length(),ptr+off,buf + offs);
						

			}
			break;
		case MYSQL_TYPE_STRING:
			if (tipo == MYSQL_TYPE_VARCHAR || tipo == MYSQL_TYPE_DATE) {
				//field->pack(ptr+off, (const unsigned char*)buf + offs, len, false);
				if (memcmp(ptr+off, (const unsigned char*)buf + offs, len)) {
					rc= HA_ERR_END_OF_FILE;
				}
						
                LOGDEBUG("comprueba_same:%d:len:%d:pack_length:%d:ptr+off:%-10.10s:buf+offs:%-10.10s",rc,len,field->pack_length(),ptr+off,buf + offs);
						
			}
			break;
		case MYSQL_TYPE_SHORT:
			if (tipo == MYSQL_TYPE_SHORT) {
				if ((short int)ldint((Uchar *)(ptr+off)) != *((short int *)((const unsigned char*)buf + offs))) {
					rc= HA_ERR_END_OF_FILE;
				}
			}
			break;
		case MYSQL_TYPE_INT24:
		case MYSQL_TYPE_LONGLONG:
		case MYSQL_TYPE_NEWDECIMAL:
		case MYSQL_TYPE_DECIMAL:
			break;
		case MYSQL_TYPE_LONG:
			if (tipo == MYSQL_TYPE_DOUBLE) {
				v_double = (double)(*(int *)(buf + offs));
				//field->pack(ptr+off,(const unsigned char*)&v_double,  sizeof(v_double), true);
				if (memcmp(ptr+off,(const unsigned char*)&v_double,  sizeof(v_double))) {
					rc= HA_ERR_END_OF_FILE;
				}
			}
			break;
		case MYSQL_TYPE_FLOAT:
			if (tipo == MYSQL_TYPE_DOUBLE) {
				v_double = (double)(*(float *)(buf + offs));
				//field->pack(ptr+off, (const unsigned char*)&v_double, sizeof(v_double), true);
				if (memcmp(ptr+off,(const unsigned char*)&v_double,  sizeof(v_double))) {
					rc= HA_ERR_END_OF_FILE;
				}
			}
			break;
		case MYSQL_TYPE_DOUBLE:
			if (tipo == MYSQL_TYPE_DOUBLE) {
				//field->pack(ptr+off,(const unsigned char*)buf + offs, len, false);
				if (memcmp(ptr+off,(const unsigned char*)buf + offs, len)) {
					rc= HA_ERR_END_OF_FILE;
				}
			}
			break;
		case MYSQL_TYPE_DATE:	
				if (tipo == MYSQL_TYPE_DATE || (tipo == MYSQL_TYPE_VARCHAR && len == 8) ) {
					// 3 byte int YYYY�16�32 + MM�32 + DD
					unsigned int fecha = 0;
					unsigned int mes,dia;
					char tmp[10];
					memcpy((char *)&fecha,(const unsigned char*)buf + offs, 3);
					dia = fecha%32;
					fecha /= 32;
					mes = fecha%16;
					fecha /= 16;
					sprintf(tmp,"%04d%02d%02d",fecha,mes,dia);
					if (memcmp(ptr+off,(const unsigned char*)tmp, 8)) {
						rc = HA_ERR_END_OF_FILE;
					}
					
						
                    LOGDEBUG("comprueba_same:%d:len:%d:pack_length:%d:ptr+off:%-8.8s:tmp:%-8.8s",rc,len,field->pack_length(),ptr+off,tmp);
						
					
				}
				break;
		default:
			//my_error(ER_CHECK_NOT_IMPLEMENTED, MYF(0), "unsuperted field type");
			rc= HA_ERR_END_OF_FILE;
			break;
		};
		offs += field->pack_length();
		length -= field->pack_length();
		if (rc)
			break;
	}
  }
  else
	  rc= HA_ERR_END_OF_FILE;

  if (rc != 0) {
				
				LOGDEBUG("index_next_same::rc=%d",rc);
				
  }

  return rc;
}

int ha_diagram::index_read_idx_map(uchar *buf, uint index, const uchar *key,
                                  key_part_map keypart_map,
                                  enum ha_rkey_function find_flag)
{

  DS_DBUG_ENTER("ha_diagram::index_read_idx", "%s", "");
  
  cindex = index;
  ds.cindex = index;
  ds.trozo = 0;
  
  DS_DBUG_RETURN(index_read_map(buf,key,keypart_map,find_flag));
}

int ha_diagram::index_read_last_map(uchar *_buf, const uchar *key,
                                   key_part_map keypart_map)
{
/*
  int rc;


  MYSQL_INDEX_READ_ROW_START(table_share->db.str, table_share->table_name.str);
	
  LOGDEBUG("index_read_last_map::index:%d",cindex);
	
  DBUG_ENTER("ha_myisam::index_read_last");
  DBUG_ASSERT(inited==INDEX);
  ha_statistic_increment(&SSV::ha_read_key_count);

  rc= HA_ERR_WRONG_COMMAND;
  MYSQL_INDEX_READ_ROW_DONE(rc);
  DBUG_RETURN(rc);
*/
	return index_read_map(_buf, key, keypart_map, HA_READ_PREFIX_LAST_OR_PREV);

}

int ha_diagram::index_next_same(uchar *buf,
			       const uchar *key,
			       uint length)
{
  int rc = 0;

  DBUG_ASSERT(inited==INDEX);
 
  
	
	LOGDEBUG("index_read_same::index:%d:length:%d:key:%x",cindex,length,key);
	


  MYSQL_INDEX_READ_ROW_START(table_share->db.str, table_share->table_name.str);
  ha_statistic_increment(&SSV::ha_read_next_count);
  /*
  do
  {
    error= mi_rnext_same(file,buf);
  } while (error == HA_ERR_RECORD_DELETED);
  table->status=error ? STATUS_NOT_FOUND: 0;
  MYSQL_INDEX_READ_ROW_DONE(error);
  return error;
  */
  if (DSMYSQL("index_next_same",&ds,NULL) < 0) {
		
		LOGDEBUG("%s", "index_next_same::HA_ERR_END_OF_FILE");
		

		rc= HA_ERR_END_OF_FILE;
		MYSQL_INDEX_READ_ROW_DONE(rc);
		DBUG_RETURN(rc);
  }

  rc = comprueba_same(key,length);

  if (!rc)
	 rc = carga_buf(buf);

  
    
	
	LOGDEBUG("index_next_same::OK:%d",rc);
	
  
 

  MYSQL_INDEX_READ_ROW_DONE(rc);
  DBUG_RETURN(rc);

}

/**
  @brief
  Used to read forward through the index.
*/

int ha_diagram::index_next(uchar *buf)
{
  int rc = 0;


  DS_DBUG_ENTER("ha_diagram::index_next", "%s", "");
  MYSQL_INDEX_READ_ROW_START(table_share->db.str, table_share->table_name.str);

  if (DSMYSQL("index_next",&ds,NULL) < 0) {
		
		LOGDEBUG("%s", "index_next::HA_ERR_END_OF_FILE");
		

		rc= HA_ERR_END_OF_FILE;
		MYSQL_INDEX_READ_ROW_DONE(rc);
		DS_DBUG_RETURN(rc);
  }
  MYSQL_INDEX_READ_ROW_DONE(rc);

  int err = carga_buf(buf);
  DS_DBUG_RETURN(err);
}


/**
  @brief
  Used to read backwards through the index.
*/

int ha_diagram::index_prev(uchar *buf)
{
  int rc = 0;


  DS_DBUG_ENTER("ha_diagram::index_prev", "%s", "");

  MYSQL_INDEX_READ_ROW_START(table_share->db.str, table_share->table_name.str);
  if (DSMYSQL("index_prev",&ds,NULL) < 0) {

		LOGDEBUG("%s", "rnd_prev::HA_ERR_END_OF_FILE");
		

		rc= HA_ERR_END_OF_FILE;
		MYSQL_INDEX_READ_ROW_DONE(rc);
		DS_DBUG_RETURN(rc);
  }
  MYSQL_INDEX_READ_ROW_DONE(rc);

  int err = carga_buf(buf);
  DS_DBUG_RETURN(err);
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
  int rc = 0;


  DS_DBUG_ENTER("ha_diagram::index_first", "%s", "");
	
  LOGDEBUG("index_first::index:%d",cindex);
	
  MYSQL_INDEX_READ_ROW_START(table_share->db.str, table_share->table_name.str);
  if (DSMYSQL("index_first",&ds,NULL) < 0) {
		
		LOGDEBUG("%s", "rnd_first::HA_ERR_END_OF_FILE");
		

		rc= HA_ERR_END_OF_FILE;
		MYSQL_INDEX_READ_ROW_DONE(rc);
		DS_DBUG_RETURN(rc);
  }
  MYSQL_INDEX_READ_ROW_DONE(rc);

  int err = carga_buf(buf);
  DS_DBUG_RETURN(err);
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
  int rc = 0;


  DS_DBUG_ENTER("ha_diagram::index_last", "%s", "");
	
	LOGDEBUG("index_last::index:%d",cindex);
	
  MYSQL_INDEX_READ_ROW_START(table_share->db.str, table_share->table_name.str);
  if (DSMYSQL("index_last",&ds,NULL) < 0) {
		
		LOGDEBUG("%s", "rnd_last::HA_ERR_END_OF_FILE");
		

		rc= HA_ERR_END_OF_FILE;
		MYSQL_INDEX_READ_ROW_DONE(rc);
		DS_DBUG_RETURN(rc);
  }
  MYSQL_INDEX_READ_ROW_DONE(rc);

  int err = carga_buf(buf);
  DS_DBUG_RETURN(err);
}


/**
  @brief Initialize the data file.
  
  @details Compare the local version of the data file with the shared one.
  If they differ, there are some changes behind and we have to reopen
  the data file to make the changes visible.
  Call @c file_buff->init_buff() at the end to read the beginning of the 
  data file into buffer.
  
  @retval  0  OK.
  @retval  1  There was an error.
*/

int ha_diagram::init_data_file()
{

  
  LOGDEBUG("init_data_file::flag:%d:name:%s",ds.flag,ds.name);
  

  if (local_data_file_version != share->data_file_version)
  {
    local_data_file_version= share->data_file_version;
    if (mysql_file_close(data_file, MYF(0)) ||
        (data_file= mysql_file_open(diagram_key_file_data,
                                    share->data_file_name, O_RDONLY,
                                    MYF(MY_WME))) == -1)
      return my_errno ? my_errno : -1;
  }
  file_buff->init_buff(data_file);
  return 0;
}


/*
  All table scans call this first.
  The order of a table scan is:

  ha_diagram::store_lock
  ha_diagram::external_lock
  ha_diagram::info
  ha_diagram::rnd_init
  ha_diagram::extra
  ENUM HA_EXTRA_CACHE   Cash record in HA_rrnd()
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
  ENUM HA_EXTRA_NO_CACHE   End cacheing of records (def)
  ha_diagram::external_lock
  ha_diagram::extra
  ENUM HA_EXTRA_RESET   Reset database to after open

  Each call to ::rnd_next() represents a row returned in the can. When no more
  rows can be returned, rnd_next() returns a value of HA_ERR_END_OF_FILE.
  The ::info() call is just for the optimizer.

*/

int ha_diagram::rnd_init(bool scan)
{

  DS_DBUG_ENTER("ha_diagram::rnd_init", "%s", "");
  
  LOGDEBUG("rd_init::%d:flag:%d:count:%d:name:%s",(int)scan,ds.flag,ds.nc,ds.name);
  

  if ((ds.flag & 1)) {

	  /* set buffer to the beginning of the file */
	  if (share->crashed || init_data_file())
		DS_DBUG_RETURN(HA_ERR_CRASHED_ON_USAGE);

	  current_position= next_position= 0;
	  stats.records= 0;
	  records_is_known= 0;
	  chain_ptr= chain;
  }
  else {
	  if (es_temporalizacion(&ds)) {
		  no_next = true;
	  }
	  if (DSMYSQL("rnd_init",&ds,NULL) < 0) {
		  DS_DBUG_RETURN(-1);
	  }
  }

  DS_DBUG_RETURN(0);
}


/*
  ::rnd_next() does all the heavy lifting for a table scan. You will need to
  populate *buf with the correct field data. You can walk the field to
  determine at what position you should store the data (take a look at how
  ::find_current_row() works). The structure is something like:
  0Foo  Dog  Friend
  The first offset is for the first attribute. All space before that is
  reserved for null count.
  Basically this works as a mask for which rows are nulled (compared to just
  empty).
  This table handler doesn't do nulls and does not know the difference between
  NULL and "". This is ok since this table handler is for spreadsheets and
  they don't know about them either :)
*/
int ha_diagram::rnd_next(uchar *buf)
{
  int rc = 0;

  DS_DBUG_ENTER("ha_diagram::rnd_next", "%s", "");
  

  MYSQL_READ_ROW_START(table_share->db.str, table_share->table_name.str,
                       TRUE);

  int err = 0;

  
  if (no_next) {
	  no_next = false;
	  rc= HA_ERR_END_OF_FILE;
	  goto end;
  }
  

  if (share->crashed)
  {
    rc= HA_ERR_CRASHED_ON_USAGE;

    LOGDEBUG("%s", "HA_ERR_CRASHED_ON_USAGE::2");
    
    goto end;
  }

  ha_statistic_increment(&SSV::ha_read_rnd_next_count);

  if ((ds.flag & 1)) {

	  rc = -1;
	  current_position= next_position;

      
      LOGDEBUG("%s", "rnd_next::especial:1");
      

	  /* don't scan an empty file */
	  if (!local_saved_data_file_length)
	  {
		rc= HA_ERR_END_OF_FILE;
		goto end;
	  }

	  if ((rc= find_current_row(buf))) {
	    rc= HA_ERR_END_OF_FILE;
		goto end;
	  }

	  stats.records++;
	  rc= 0;
  }
  else  {

	  if (DSMYSQL("rnd_next",&ds,NULL) < 0) {
		  
		  LOGDEBUG("%s", "rnd_next::HA_ERR_END_OF_FILE");
		  

		  rc= HA_ERR_END_OF_FILE;
		  MYSQL_READ_ROW_DONE(rc);
		  DBUG_RETURN(rc);
	  }
      
	  err = carga_buf(buf);

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
	  
  
	  if (err) {
		  MYSQL_READ_ROW_DONE(rc);
		  
		  LOGDEBUG("%s", "rnd_next::HA_ERR_UNSUPPORTED");
		  
		  DS_DBUG_RETURN(HA_ERR_UNSUPPORTED);
	  }
  }
end:
  if (rc < 0) {
     
     LOGDEBUG("%s", "rnd_next::-1");
     
     rc= HA_ERR_END_OF_FILE;
	}
  MYSQL_READ_ROW_DONE(rc);
  DS_DBUG_RETURN(rc);
}

/*
  In the case of an order by rows will need to be sorted.
  ::position() is called after each call to ::rnd_next(),
  the data it stores is to a byte array. You can store this
  data via my_store_ptr(). ref_length is a variable defined to the
  class that is the sizeof() of position being stored. In our case
  its just a position. Look at the bdb code if you want to see a case
  where something other then a number is stored.
*/
void ha_diagram::position(const uchar *record)
{
  
  DBUG_ENTER("ha_diagram::position");


  if ((ds.flag & 1)) {

	my_store_ptr(ref, ref_length, current_position);

  }
  else  {
    LOGDEBUG("%s:%d:%d", "position", ref_length, ds.pos);

    my_store_ptr(ref, ref_length, (my_off_t)ds.pos); 

  }
  DBUG_VOID_RETURN;
}


/*
  Used to fetch a row from a posiion stored with ::position().
  my_get_ptr() retrieves the data for you.
*/

int ha_diagram::rnd_pos(uchar * buf, uchar *pos)
{
  int rc;

  DS_DBUG_ENTER("ha_diagram::rnd_pos:", "%s", "");


  MYSQL_READ_ROW_START(table_share->db.str, table_share->table_name.str,
                       FALSE);
  ha_statistic_increment(&SSV::ha_read_rnd_count);

  if ((ds.flag & 1)) {

	  current_position= my_get_ptr(pos,ref_length);
	  rc= find_current_row(buf);
  }
  else {

	  ds.pos = ((int)my_get_ptr(pos,ref_length));

	  if (DSMYSQL("rnd_cur",&ds,NULL) < 0) {
		  
		  LOGDEBUG("%s", "rnd_cur::HA_ERR_END_OF_FILE");

		  rc= HA_ERR_END_OF_FILE;
	  }
	  else {
		  rc = 0;
		  carga_buf(buf);
	  }

  }
  MYSQL_READ_ROW_DONE(rc);
  DS_DBUG_RETURN(rc);
}

/*
  ::info() is used to return information to the optimizer.
  Currently this table handler doesn't implement most of the fields
  really needed. SHOW also makes use of this data
*/
int ha_diagram::info(uint flag)
{

  DBUG_ENTER("ha_diagram::info");
  
  LOGDEBUG("info::%d",(int)flag);
  

  /* This is a lie, but you don't want the optimizer to see zero or 1 */
  if (!records_is_known && stats.records < 2) 
    stats.records= 2;
  DBUG_RETURN(0);
}

/*
  Grab bag of flags that are sent to the able handler every so often.
  HA_EXTRA_RESET and HA_EXTRA_RESET_STATE are the most frequently called.
  You are not required to implement any of these.
*/
int ha_diagram::extra(enum ha_extra_function operation)
{

  DS_DBUG_ENTER("ha_diagram::extra", "%s", "");
  

  if (operation == HA_EXTRA_CACHE) {  /* Cache record in HA_rrnd() */
	
  }
  else if (operation == HA_EXTRA_NO_CACHE) {                  /* End caching of records (def) */
	
  }
  else if (operation == HA_EXTRA_NO_READCHECK) { //on update
	
  }
  else if (operation == HA_EXTRA_PREPARE_FOR_UPDATE) {
	
  }
  else if (operation == HA_EXTRA_PREPARE_FOR_DROP) {
	
  }
  else if (operation == HA_EXTRA_NO_IGNORE_DUP_KEY) {
	
  }
  else if (operation == HA_EXTRA_PREPARE_FOR_RENAME) {
	
  }
  else if (operation ==  HA_EXTRA_ADD_CHILDREN_LIST) { // Va a crear un temporal
  }
  else if (operation == HA_EXTRA_MARK_AS_LOG_TABLE)
  {
    LOGDEBUG("extra:%s", "HA_EXTRA_MARK_AS_LOG_TABLE");
     mysql_mutex_lock(&share->mutex);
     share->is_log_table= TRUE;
     mysql_mutex_unlock(&share->mutex);
  }
  else {

  }

  DS_DBUG_RETURN(0);
}


/*
  Set end_pos to the last valid byte of continuous area, closest
  to the given "hole", stored in the buffer. "Valid" here means,
  not listed in the chain of deleted records ("holes").
*/
bool ha_diagram::get_write_pos(my_off_t *end_pos, diagram_set *closest_hole)
{
  
  LOGDEBUG("%s", "get_write_pos");
  
  if (closest_hole == chain_ptr) /* no more chains */
    *end_pos= file_buff->end();
  else
    *end_pos= min(file_buff->end(),
                  closest_hole->begin);
  return (closest_hole != chain_ptr) && (*end_pos == closest_hole->begin);
}


/*
  Called after each table scan. In particular after deletes,
  and updates. In the last case we employ chain of deleted
  slots to clean up all of the dead space we have collected while
  performing deletes/updates.
*/
int ha_diagram::rnd_end()
{
  char updated_fname[FN_REFLEN];
  my_off_t file_buffer_start= 0;


  DS_DBUG_ENTER("ha_diagram::rnd_end:", "flag:%d:name:%s",ds.flag,ds.name);

  

  if ((ds.flag & 1)) {

	  free_root(&blobroot, MYF(0));
	  records_is_known= 1;

	  if ((chain_ptr - chain)  > 0)
	  {
		diagram_set *ptr= chain;

		/*
		  Re-read the beginning of a file (as the buffer should point to the
		  end of file after the scan).
		*/
		file_buff->init_buff(data_file);

		/*
		  The sort is needed when there were updates/deletes with random orders.
		  It sorts so that we move the firts blocks to the beginning.
		*/
		my_qsort(chain, (size_t)(chain_ptr - chain), sizeof(diagram_set),
				 (qsort_cmp)sort_set);

		my_off_t write_begin= 0, write_end;

		/* create the file to write updated table if it wasn't yet created */
		if (open_update_temp_file_if_needed())
		  DS_DBUG_RETURN(-1);

		/* write the file with updated info */
		while ((file_buffer_start != (my_off_t)-1))     // while not end of file
		{
		  bool in_hole= get_write_pos(&write_end, ptr);
		  my_off_t write_length= write_end - write_begin;

		  /* if there is something to write, write it */
		  if (write_length)
		  {
			if (mysql_file_write(update_temp_file,
								 (uchar*) (file_buff->ptr() +
										   (write_begin - file_buff->start())),
								 (size_t)write_length, MYF_RW))
			  goto error;
			temp_file_length+= write_length;
		  }
		  if (in_hole)
		  {
			/* skip hole */
			while (file_buff->end() <= ptr->end &&
				   file_buffer_start != (my_off_t)-1)
			  file_buffer_start= file_buff->read_next();
			write_begin= ptr->end;
			ptr++;
		  }
		  else
			write_begin= write_end;

		  if (write_end == file_buff->end())
			file_buffer_start= file_buff->read_next(); /* shift the buffer */

		}

		if (mysql_file_sync(update_temp_file, MYF(MY_WME)) ||
			mysql_file_close(update_temp_file, MYF(0)))
		  DS_DBUG_RETURN(-1);

		share->update_file_opened= FALSE;

		if (share->diagram_write_opened)
		{
		  if (mysql_file_close(share->diagram_write_filedes, MYF(0)))
			DS_DBUG_RETURN(-1);
		  /*
			Mark that the writer fd is closed, so that init_diagram_writer()
			will reopen it later.
		  */
		  share->diagram_write_opened= FALSE;
		}

		/*
		  Close opened fildes's. Then move updated file in place
		  of the old datafile.
		*/
		if (mysql_file_close(data_file, MYF(0)) ||
			mysql_file_rename(diagram_key_file_data,
							  fn_format(updated_fname, share->table_name,
										"", CSN_EXT,
										MY_REPLACE_EXT | MY_UNPACK_FILENAME),
							  share->data_file_name, MYF(0)))
		  DS_DBUG_RETURN(-1);

		/* Open the file again */
		if (((data_file= mysql_file_open(diagram_key_file_data,
										 share->data_file_name,
										 O_RDONLY, MYF(MY_WME))) == -1))
		  DS_DBUG_RETURN(my_errno ? my_errno : -1);
		/*
		  As we reopened the data file, increase share->data_file_version 
		  in order to force other threads waiting on a table lock and  
		  have already opened the table to reopen the data file.
		  That makes the latest changes become visible to them.
		  Update local_data_file_version as no need to reopen it in the 
		  current thread.
		*/
		share->data_file_version++;
		local_data_file_version= share->data_file_version;
		/*
		  The datafile is consistent at this point and the write filedes is
		  closed, so nothing worrying will happen to it in case of a crash.
		  Here we record this fact to the meta-file.
		*/
		(void)write_meta_file(share->meta_file, share->rows_recorded, FALSE);
		/* 
		  Update local_saved_data_file_length with the real length of the 
		  data file.
		*/
		local_saved_data_file_length= temp_file_length;
	  }
  }
  else {

	  if (DSMYSQL("rnd_end",&ds,NULL) < 0) {
		  DS_DBUG_RETURN(-1);
	  }
  }

  DS_DBUG_RETURN(0);
error:
  LOGDEBUG("%s", "rnd_end:error");
  
  
  mysql_file_close(update_temp_file, MYF(0));
  share->update_file_opened= FALSE;
  DS_DBUG_RETURN(-1);
}


/*
  Repair DIAGRAM table in the case, it is crashed.

  SYNOPSIS
    repair()
    thd         The thread, performing repair
    check_opt   The options for repair. We do not use it currently.

  DESCRIPTION
    If the file is empty, change # of rows in the file and complete recovery.
    Otherwise, scan the table looking for bad rows. If none were found,
    we mark file as a good one and return. If a bad row was encountered,
    we truncate the datafile up to the last good row.

   TODO: Make repair more clever - it should try to recover subsequent
         rows (after the first bad one) as well.
*/

int ha_diagram::repair(THD* thd, HA_CHECK_OPT* check_opt)
{
  char repaired_fname[FN_REFLEN];
  uchar *buf;
  File repair_file;
  int rc;
  ha_rows rows_repaired= 0;
  my_off_t write_begin= 0, write_end;


  DS_DBUG_ENTER("ha_diagram::repair", "flag:%d:name:%s",ds.flag,ds.name);


  if (!(ds.flag & 1)) 
	    DS_DBUG_RETURN(0);



  /* empty file */
  if (!share->saved_data_file_length)
  {
    share->rows_recorded= 0;
    goto end;
  }

  /* Don't assert in field::val() functions */
  table->use_all_columns();
  if (!(buf= (uchar*) my_malloc(table->s->reclength, MYF(MY_WME))))
    DS_DBUG_RETURN(HA_ERR_OUT_OF_MEM);

  /* position buffer to the start of the file */
  if (init_data_file())
    DS_DBUG_RETURN(HA_ERR_CRASHED_ON_REPAIR);

  /*
    Local_saved_data_file_length is initialized during the lock phase.
    Sometimes this is not getting executed before ::repair (e.g. for
    the log tables). We set it manually here.
  */
  local_saved_data_file_length= share->saved_data_file_length;
  /* set current position to the beginning of the file */
  current_position= next_position= 0;

  /* Read the file row-by-row. If everything is ok, repair is not needed. */
  while (!(rc= find_current_row(buf)))
  {
    thd_inc_row_count(thd);
    rows_repaired++;
    current_position= next_position;
  }

  free_root(&blobroot, MYF(0));

  my_free(buf);

  if (rc == HA_ERR_END_OF_FILE)
  {
    /*
      All rows were read ok until end of file, the file does not need repair.
      If rows_recorded != rows_repaired, we should update rows_recorded value
      to the current amount of rows.
    */
    share->rows_recorded= rows_repaired;
    goto end;
  }

  /*
    Otherwise we've encountered a bad row => repair is needed.
    Let us create a temporary file.
  */
  if ((repair_file= mysql_file_create(diagram_key_file_update,
                                      fn_format(repaired_fname,
                                                share->table_name,
                                                "", CSN_EXT,
                                                MY_REPLACE_EXT|MY_UNPACK_FILENAME),
                                      0, O_RDWR | O_TRUNC, MYF(MY_WME))) < 0)
    DS_DBUG_RETURN(HA_ERR_CRASHED_ON_REPAIR);

  file_buff->init_buff(data_file);


  /* we just truncated the file up to the first bad row. update rows count. */
  share->rows_recorded= rows_repaired;

  /* write repaired file */
  while (1)
  {
    write_end= min(file_buff->end(), current_position);
    if ((write_end - write_begin) &&
        (mysql_file_write(repair_file, (uchar*)file_buff->ptr(),
                          (size_t) (write_end - write_begin), MYF_RW)))
      DS_DBUG_RETURN(-1);

    write_begin= write_end;
    if (write_end== current_position)
      break;
    else
      file_buff->read_next(); /* shift the buffer */
  }

  /*
    Close the files and rename repaired file to the datafile.
    We have to close the files, as on Windows one cannot rename
    a file, which descriptor is still open. EACCES will be returned
    when trying to delete the "to"-file in mysql_file_rename().
  */
  if (share->diagram_write_opened)
  {
    /*
      Data file might be opened twice, on table opening stage and
      during write_row execution. We need to close both instances
      to satisfy Win.
    */
    if (mysql_file_close(share->diagram_write_filedes, MYF(0)))
      DS_DBUG_RETURN(my_errno ? my_errno : -1);
    share->diagram_write_opened= FALSE;
  }
  if (mysql_file_close(data_file, MYF(0)) ||
      mysql_file_close(repair_file, MYF(0)) ||
      mysql_file_rename(diagram_key_file_data,
                        repaired_fname, share->data_file_name, MYF(0)))
    DS_DBUG_RETURN(-1);

  /* Open the file again, it should now be repaired */
  if ((data_file= mysql_file_open(diagram_key_file_data,
                                  share->data_file_name, O_RDWR|O_APPEND,
                                  MYF(MY_WME))) == -1)
     DBUG_RETURN(my_errno ? my_errno : -1);

  /* Set new file size. The file size will be updated by ::update_status() */
  local_saved_data_file_length= (size_t) current_position;

end:
  share->crashed= FALSE;
  DS_DBUG_RETURN(HA_ADMIN_OK);
}

/*
  DELETE without WHERE calls this
*/

int ha_diagram::delete_all_rows()
{
  int rc;

  DS_DBUG_ENTER("ha_diagram::delete_all_rows", "%s", "");

  if (!(ds.flag & 1)) 
  {
       DSMYSQL("delete_all_rows",&ds,NULL);
	   DS_DBUG_RETURN(0);
  }


  if (!records_is_known)
    DS_DBUG_RETURN(my_errno=HA_ERR_WRONG_COMMAND);

  if (!share->diagram_write_opened)
    if (init_diagram_writer())
      DS_DBUG_RETURN(-1);

  /* Truncate the file to zero size */
  rc= mysql_file_chsize(share->diagram_write_filedes, 0, 0, MYF(MY_WME));

  stats.records=0;
  /* Update shared info */
  mysql_mutex_lock(&share->mutex);
  share->rows_recorded= 0;
  mysql_mutex_unlock(&share->mutex);
  local_saved_data_file_length= 0;
  DS_DBUG_RETURN(rc);
}

/*
  Called by the database to lock the table. Keep in mind that this
  is an internal lock.
*/
THR_LOCK_DATA **ha_diagram::store_lock(THD *thd,
                                    THR_LOCK_DATA **to,
                                    enum thr_lock_type lock_type)
{
  
  LOGDEBUG("%s", "store_lock");
  
  
  return to;
  /*
  if (lock_type != TL_IGNORE && lock.type == TL_UNLOCK)
    lock.type=lock_type;
  *to++= &lock;
  return to;
  */
}

/* 
  Create a table. You do not want to leave the table open after a call to
  this (the database will call ::open() if it needs to).
*/

int ha_diagram::delete_table(const char *name)
{
  
  LOGDEBUG("delete_table::%s",name);
  
  
  if (ds.name) {
	  if (!strcmp(ds.name,name))
		close();
  }

  return handler::delete_table(name);
  /*

  int especial = 0;
  const char *pp = name;
  while(*pp++);
  while(*--pp != '/');
  if (*pp == '/')
	  pp++;
  if (*pp == '#') {
	  especial = 1;
  }
  if (especial) {
	  return handler::delete_table(name);
  }

  DBUG_RETURN(0);
  */
}

int ha_diagram::rename_table(const char * from, const char * to)
{

  DBUG_ENTER("ha_diagram::rename_table ");
  
  LOGDEBUG("rename_table::%s:%s",from,to);
  

  //DBUG_RETURN(0);
  return handler::rename_table(from,to);

  /*

  int especial = 0;
  const char *pp = from;
  while(*pp++);
  while(*--pp != '/');
  if (*pp == '/')
	  pp++;
  if (*pp == '#') {
	  especial = 1;
  }
  if (especial) {
	  if (!strcmp(to,table_share->table_name.str)) {
			  fff = fopen("/tmp/debugds.txt","a");
			  fprintf(fff,"rename_table::delete_from:1:\r\n");
			  fclose(fff);
		  	  handler::delete_table(from);
			  fff = fopen("/tmp/debugds.txt","a");
			  fprintf(fff,"rename_table::delete_from:2:\r\n");
			  fclose(fff);
	  }
	  else {
		  return handler::rename_table(from,to);
	  }
  }
  DBUG_RETURN(0);//HA_ERR_WRONG_COMMAND
  */
}


ha_rows ha_diagram::records_in_range(uint inx, key_range *min_key,
                                     key_range *max_key)
{
  DS_DBUG_ENTER("ha_diagram::records_in_range", "%s", "");
  

  DS_DBUG_RETURN(DSMYSQL("records_in_range",&ds,NULL));
}

int ha_diagram::create(const char *name, TABLE *table_arg,
                    HA_CREATE_INFO *create_info)
{
  char name_buff[FN_REFLEN];
  File create_file;

 
  DS_DBUG_ENTER("ha_diagram::create", "%s", name);

  
  

  int especial = 0;
  const char *pp = name;
  while(*pp++);
  while(*--pp != '/');
  if (*pp == '/')
	  pp++;
  if (*pp == '#') {
	  especial = 1;
  }

  LOGDEBUG("create::%d",especial);


  /*
    check columns
  */
  /*
  for (Field **field= table_arg->field; *field; field++)
  {
    if ((*field)->real_maybe_null())
    {
      my_error(ER_CHECK_NOT_IMPLEMENTED, MYF(0), "nullable columns");
      DBUG_RETURN(HA_ERR_UNSUPPORTED);
    }
  }
  */
  
  

  if (especial) {
	  if ((create_file= mysql_file_create(diagram_key_file_metadata,
										  fn_format(name_buff, name, "", CSM_EXT,
													MY_REPLACE_EXT|MY_UNPACK_FILENAME),
										  0, O_RDWR | O_TRUNC, MYF(MY_WME))) < 0)
		DS_DBUG_RETURN(-1);
  }
  else {
	  if (DSMYSQL("create",&ds,NULL) < 0)
		  DS_DBUG_RETURN(-1);
  }


  if (especial) 
	write_meta_file(create_file, 0, FALSE);
  

  if (especial) 
	mysql_file_close(create_file, MYF(0));


  if (especial) {
	  if ((create_file= mysql_file_create(diagram_key_file_data,
										  fn_format(name_buff, name, "", DIAGRAM_EXT,
													MY_REPLACE_EXT|MY_UNPACK_FILENAME),
										  0, O_RDWR | O_TRUNC, MYF(MY_WME))) < 0)
		DS_DBUG_RETURN(-1);
  }

  if (especial) 
	mysql_file_close(create_file, MYF(0));


  DS_DBUG_RETURN(0);
}

int ha_diagram::check(THD* thd, HA_CHECK_OPT* check_opt)
{
  int rc= 0;
  uchar *buf;
  const char *old_proc_info;
  ha_rows count= share->rows_recorded;
  DBUG_ENTER("ha_diagram::check");

  LOGDEBUG("%s", "check");
  

  if (0) {
	  old_proc_info= thd_proc_info(thd, "Checking table");
	  if (!(buf= (uchar*) my_malloc(table->s->reclength, MYF(MY_WME))))
		DBUG_RETURN(HA_ERR_OUT_OF_MEM);

	  /* position buffer to the start of the file */
	   if (init_data_file())
		 DBUG_RETURN(HA_ERR_CRASHED);

	  /*
		Local_saved_data_file_length is initialized during the lock phase.
		Check does not use store_lock in certain cases. So, we set it
		manually here.
	  */
	  local_saved_data_file_length= share->saved_data_file_length;
	  /* set current position to the beginning of the file */
	  current_position= next_position= 0;

	  /* Read the file row-by-row. If everything is ok, repair is not needed. */
	  while (!(rc= find_current_row(buf)))
	  {
		thd_inc_row_count(thd);
		count--;
		current_position= next_position;
	  }

	  free_root(&blobroot, MYF(0));

	  my_free(buf);
	  thd_proc_info(thd, old_proc_info);

	  if ((rc != HA_ERR_END_OF_FILE) || count)
	  {
		share->crashed= TRUE;
		DBUG_RETURN(HA_ADMIN_CORRUPT);
	  }
  }

  DBUG_RETURN(HA_ADMIN_OK);
}


bool ha_diagram::check_if_incompatible_data(HA_CREATE_INFO *info,
					   uint table_changes)
{
  
  LOGDEBUG("%s", "check_if_incompatible_data");
  
  return COMPATIBLE_DATA_YES;
}

struct st_mysql_storage_engine diagram_storage_engine=
{ MYSQL_HANDLERTON_INTERFACE_VERSION };

mysql_declare_plugin(diagram)
{
  MYSQL_STORAGE_ENGINE_PLUGIN,
  &diagram_storage_engine,
  "diagram",
  "diagram soft",
  "DIAGRAM storage engine",
  PLUGIN_LICENSE_GPL,
  diagram_init_func, /* Plugin Init */
  diagram_done_func, /* Plugin Deinit */
  0x0100 /* 1.0 */,
  NULL,                       /* status variables                */
  NULL,                       /* system variables                */
  NULL,                       /* config options                  */
  0,                          /* flags                           */
}
mysql_declare_plugin_end;

