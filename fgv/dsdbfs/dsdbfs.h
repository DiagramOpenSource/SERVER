
/*
    Filesystem de base de datos ds.	
 */

#define IDDSDB 0x19670403 /* Identifica el fichero y el bloque de cabecera */
#define IDBIDB 0x19960117 /* Identifica bloque inicial de fichero */
#define IDBSDB 0x19650811 /* Identifica bloque extra de fichero */
#define IDBFDB 0x19890429 /* Identifica bloque libre */
#define IDBDDB 0x19890228 /* Identifica bloque de directorio */
#define IDBMPB 0x20030403 /* Identifica bloque de mapa de fichero */

#define DSDB_MIN_VERSION 0x00010001
#define DSDB_VERSION 0x00010002
#define DSDB_RANDON dsdb_/*dsdb_random: recordar rellebar con basura todos los huecos*/
#define DSDB_FLAG_CRYPTO 0x1
#define DSDB_FLAG_COPIA 0x2

#define DSBDBMAXTABLES 0xfffff

#ifndef DS_OFF_DEFINIDO
#define DS_OFF_DEFINIDO
#ifdef WIN32
typedef __int64 ds_off;
#else
typedef long long ds_off;
#endif
#endif


typedef unsigned long ulong;
typedef unsigned char uchar;

#define MAXNAMEDB 64

#define DEFDBBLOCKSIZE 512
extern ulong dsdb_blocksize; /* tamaño del bloque (para el caso de crear el fichero) */

#define MAX_OPEN_BASES 128
#define MAX_OPEN_FILES 1024

#define MAXDSDBBLOQUEOS 256

typedef struct
{	
	ulong blocksize;
	ulong ntotalblocks;
	ulong pfreeblock;
	ulong pdirectory;
	ulong newfileid;
} DBHeader;

typedef struct
{	
    ulong id;
	ulong ssize;
	ulong version;	
	ulong nupdate;	
	DBHeader header;
	unsigned short openid; 
	unsigned short alineamiento;
	ulong flags;
	ulong test1;
	ulong test2;
} DBFullHeader;

typedef struct
{	
    ulong id;
	ulong ssize;
	ulong version;	
	ulong nupdate;	
	DBHeader header;
	unsigned short openid; 
} DBFullHeader1;


#define OFFSET_NTOTALBLOCKS (sizeof(ulong)*5)
#define OFFSET_PFREEBLOCK   (sizeof(ulong)*6)
#define OFFSET_PDIRECTORY   (sizeof(ulong)*7)
#define OFFSET_PFILEID      (sizeof(ulong)*8)
#define OFFSET_OPENID       (sizeof(ulong)*4 + sizeof(DBHeader))
#define OFFSET_PADDING (sizeof(ulong) * 4 + sizeof(ds_off))
#define OFFSET_IDENTRY (sizeof(ulong) * 3)
#define OFFSET_INIDATASIZE (sizeof(ulong) * 4)


typedef struct
{
	uchar name[MAXNAMEDB];	
	ulong filetype;
	ulong pinidata;
} DBDirEntry;

typedef struct
{
	ds_off pos;	
	ulong pbloque;
} DBMapEntry;

/* bloque inicial de datos de un fichero */
typedef struct
{
	ulong  id;	
	ulong  pnext;
	ulong  reservado; /* */
	ulong  identry; /* identificador "unico"(el numero de entrada de directorio) que le damos al fichero */	
	ds_off size;
	ulong  padding; /* se usa para bloqueo */
	ulong  ssize;
	uchar  name[MAXNAMEDB];	
	ulong  pmapa;
	ulong  ctime;
	ulong  mtime;
	ulong  atime;
	ulong  nupdate;	
	ulong  filetype;
} DBIniData;


typedef struct
{
	ulong  id;
	ulong  pnext;	
	ulong  reservado; /* numero de orden de este bloque */
	ulong  identry;	/* con esto y el blockorder se nos facilita la recuperacion */	
	ds_off grabadoyorder;
	/* IMPORTANTISIMO: TIENE QUE SER IGUAL QUE EL COMIENZO DE BDIniData 
	ulong  grabado;
	ulong  blockorder;
	*/
} DBMoreData;

typedef struct
{
	ulong id;	
	ulong pnext;
	ulong nextrablocks; /* bloques contiguos como un solo bloque */
} DBLibre;

typedef struct
{
	ulong id;	
	ulong level;	
	ulong nitems;	
} DBArbol;

typedef struct
{
	DBDirEntry direntry;    	
	ulong  flags;
	ulong  bloquecorriente;	
	ds_off de;
	ds_off hasta;
	ds_off posicion;
	ds_off size;
	ulong pmapa;
	ulong identry;
	int   base_fd;
	int   free_file;	
	unsigned char *enigma;
	int maxlmapa;
	unsigned long *lmapa;
	unsigned long acceso;
	
	int nbloqueos;
	ds_off bloqueo[MAXDSDBBLOQUEOS];
	ds_off lenbloqueo[MAXDSDBBLOQUEOS];
	
	
} DBFile;


#ifndef WIN32

/*
#ifdef FGVLINUX

int _sopen_(const char *path,int oflag,int shflag,...);
#define dsdb_sabre          _sopen_
#define dsdb_abre           open64
#define dsdb_lee            read
#define dsdb_graba          write
#define dsdb_lseek(a,b,c)   ((ds_off)lseek64(a,((off_t)b),c))
#define dsdb_cierra         close
#define dsdb_locking(a,b,c)        lockf64(a,b,((off_t)c))
#define dsdb_locking64(a,b,c) 	   lockf64(a,b,((off_t)c))

#else

int _sopen_(const char *path,int oflag,int shflag,...);
#define dsdb_sabre          _sopen_
#define dsdb_abre           open
#define dsdb_lee            read
#define dsdb_graba          write
#define dsdb_lseek(a,b,c)   ((ds_off)lseek(a,((off_t)b),c))
#define dsdb_cierra         close
#define dsdb_locking(a,b,c)        lockf(a,b,((off_t)c))
#define dsdb_locking64(a,b,c) 	   lockf(a,b,((off_t)c))

#endif
*/

long long dsdb_lseek(int fh, long long distance, unsigned long MoveMethod);
unsigned int dsdb_lee(int fh,void *buf,unsigned cnt);
int dsdb_cierra(int fh);
int dsdb_sabre(char *path,int oflag,int shflag,...);
int dsdb_abre(char *path,int oflag,...);        
int dsdb_locking(int fh,int lmode,long nbytes);
int dsdb_locking64(int fh,int lmode,ds_off nbytes);
unsigned int dsdb_graba(int fh,void *buf,unsigned cnt);


#else

__int64 dsdb_lseek(int fh, __int64 distance, unsigned long MoveMethod);
int dsdb_lee(int fh,void *buf,unsigned int cnt);
int dsdb_cierra(int fh);
int dsdb_sabre(char *path,int oflag,int shflag,...);
int dsdb_abre(char *path,int oflag,...);        
int dsdb_locking(int fh,int lmode,long nbytes);
int dsdb_locking64(int fh,int lmode,ds_off nbytes);
int dsdb_graba(int fh,void *buf,unsigned int cnt);

#endif


/* BASIC APIS */
int dsdb_sopen_(char *filename, int oflag, int shflag );
int dsdb_open_(char *filename, int oflag , int pmode);
ds_off dsdb_lseek_(int handle,ds_off offset,int origin);
int dsdb_lockf_(int handle, int mode, long nbytes);
int dsdb_read_(int handle, void *buffer, unsigned int count);
int dsdb_write_(int handle, void *buffer, unsigned int count);
int dsdb_close_(int handle);
