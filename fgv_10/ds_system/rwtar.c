
#include <fgvsys.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef O_BINARY
#include <io.h>
#endif
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <string.h>
#include <memory.h>
#include <errno.h>
#include <time.h>
#include <sys/timeb.h>
#ifdef O_BINARY
#include <sys/utime.h>
#else
#define O_BINARY 0
#include <utime.h>
#endif

#ifdef FGVWIN
#include <fgvwin.h>
#endif

#ifdef FGVWIN
int instalacion_en_grafico = 1;
#else
int instalacion_en_grafico = 0;
#endif

#ifdef FGVDOS
#include <commctrl.h>
static HWND dlg = NULL;
static HWND wPadre = NULL;
static int empieza_dlg = 0;
static HINSTANCE hInstacia = NULL;
#ifndef FGVWIN
#include "../rci/resource.h"
#endif
#endif


#include <rglobal.h>
#include <rtermin.h>
#define _NUEVODEF
#include <rdef.h>

extern char *quita_path();
/* extern char filtro[81]; */

extern char nmen0[MAXPATH];

#ifdef index
#undef index
#endif
#ifdef rindex
#undef rindex
#endif
#define index strchr
#define rindex strrchr
#define bcopy(s, d, n) memcpy(d, s, n)
#define bzero(s, n) memset(s, 0, n)
#define bcmp memcmp

#ifdef FGVSCO
#undef S_IFSOCK
#undef S_IFCTG
#endif

#ifndef S_ISREG			/* Doesn't have POSIX.1 stat stuff. */
#define mode_t unsigned short
#endif
#if !defined(S_ISBLK) && defined(S_IFBLK)
#define	S_ISBLK(m) (((m) & S_IFMT) == S_IFBLK)
#endif
#if !defined(S_ISCHR) && defined(S_IFCHR)
#define	S_ISCHR(m) (((m) & S_IFMT) == S_IFCHR)
#endif
#if !defined(S_ISDIR) && defined(S_IFDIR)
#define	S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#endif
#if !defined(S_ISREG) && defined(S_IFREG)
#define	S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#endif
#if !defined(S_ISFIFO) && defined(S_IFIFO)
#define	S_ISFIFO(m) (((m) & S_IFMT) == S_IFIFO)
#define mkfifo(path, mode) (mknod ((path), (mode) | S_IFIFO, 0))
#endif
#if !defined(S_ISLNK) && defined(S_IFLNK)
#define	S_ISLNK(m) (((m) & S_IFMT) == S_IFLNK)
#endif
#if !defined(S_ISSOCK) && defined(S_IFSOCK)
#define	S_ISSOCK(m) (((m) & S_IFMT) == S_IFSOCK)
#endif
#if !defined(S_ISMPB) && defined(S_IFMPB)	/* V7 */
#define S_ISMPB(m) (((m) & S_IFMT) == S_IFMPB)
#define S_ISMPC(m) (((m) & S_IFMT) == S_IFMPC)
#endif
#if !defined(S_ISNWK) && defined(S_IFNWK)	/* HP/UX */
#define S_ISNWK(m) (((m) & S_IFMT) == S_IFNWK)
#endif
#if !defined(S_ISCTG) && defined(S_IFCTG)	/* contiguous file */
#define S_ISCTG(m) (((m) & S_IFMT) == S_IFCTG)
#endif
#if !defined(S_ISVTX)
#ifdef WIN32
#define	S_ISUID		0
#define	S_ISGID		0
#define	S_ISVTX		0
#else
#define S_ISVTX 0001000
#endif
#endif


#ifndef O_NDELAY
#define O_NDELAY 0
#endif

#define	RECORDSIZE	512
#define	NAMSIZ		100
#define	TUNMLEN		32
#define	TGNMLEN		32

union record
{
   char charptr[RECORDSIZE];
   struct header
   {
      char arch_name[NAMSIZ];
      char mode[8];
      char uid[8];
      char gid[8];
      char size[12];
      char mtime[12];
      char chksum[8];
      char linkflag;
      char arch_linkname[NAMSIZ];
      char magic[8];
      char uname[TUNMLEN];
      char gname[TGNMLEN];
      char devmajor[8];
      char devminor[8];
      /* these following fields were added by JF for gnu */
      /* and are NOT standard */
      char atime[12];
      char ctime[12];
      char offset[12];
      char longnames[4];
#ifdef NEEDPAD
      char pad;
#endif
   }
   header;
};



#define	CHKBLANKS	"        "	/* 8 blanks, no null */

/* The magic field is filled with this if uname and gname are valid. */
#define	TMAGIC		"ustar  "	/* 7 chars and a null */

/* The linkflag defines the type of file */
#define	LF_OLDNORMAL	'\0'	/* Normal disk file, Unix compat */
#define	LF_NORMAL	'0'	/* Normal disk file */
#define	LF_LINK		'1'	/* Link to previously dumped file */
#define	LF_SYMLINK	'2'	/* Symbolic link */
#define	LF_CHR		'3'	/* Character special file */
#define	LF_BLK		'4'	/* Block special file */
#define	LF_DIR		'5'	/* Directory */
#define	LF_FIFO		'6'	/* FIFO special file */
#define	LF_CONTIG	'7'	/* Contiguous file */
/* Further link types may be defined later. */

/* Note that the standards committee allows only capital A through
capital Z for user-defined expansion.  This means that defining something
as, say '8' is a *bad* idea. */
#define LF_LONGLINK	'K'	/* Identifies the NEXT file on the tape
as having a long linkname */
#define LF_LONGNAME	'L'	/* Identifies the NEXT file on the tape
as having a long name. */
/* Ignore it on extraction */

/*
* Exit codes from the "tar" program
*/
#define	EX_SUCCESS	0	/* success! */
#define	EX_ARGSBAD	1	/* invalid args */
#define	EX_BADFILE	2	/* invalid filename */
#define	EX_BADARCH	3	/* bad archive */
#define	EX_SYSTEM	4	/* system gave unexpected error */
#define EX_BADVOL	5	/* Special error code means
Tape volume doesn't match the one
specified on the command line */


#ifndef PATH_MAX
#define PATH_MAX 250
#endif

union record *findrec ();
void userec ();
union record *endofrecs ();

#ifdef FGVDOS
extern BOOL fIsTNT;
#endif

union record *ar_block = NULL;	/* Start of block of archive */
union record *ar_record = NULL;	/* Current record of archive */
union record *ar_last = NULL;	/* Last+1 record of archive block */
char ar_reading;	/* 0 writing, !0 reading archive */
int blocking = 1;	/* Size of each block, in records */
int blocksize = RECORDSIZE;	/* Size of each block, in bytes */


int instalacion_silenciosa = 0;
static char **name_matchs = NULL;
static int nname_matchs   = 0;
static int es_instalacion = 0;
static int instalacion_9 = 0;
static int marcada_la_ayuda = 0;
static int hay_ins_continuacion = 0;
static int posible_ins_continuacion = 0;
static char ins_continuacion[10][30];
static int en_minusculas  = 0;
static int es_sacarwi     = 0;
static int rwi_parte = 0;
static int pregunta_insta = 0;

static int match = 0;

char *ar_files[1];	/* list of tape drive names */
int n_ar_files = 0;	/* number of tape drive names */
int cur_ar_file = 0;	/* tape drive currently being used */
char current_file_name[260],current_link_name[260]; 

time_t new_time;

int f_oldarch = 0;	/* -o */
int f_absolute_paths = 0;/* -P */
int f_exclude = 0;	/* -X */
int f_remove_files = 0;	/* --remove-files */
int f_atime_preserve = 0;/* --atime-preserve */
int f_unlink = 0;	/* --unlink */
int f_keep = 0;		/* -k */
int f_do_chown = 0;	/* --do-chown */
int f_new_files = 0;	/* -N */

int sin_iconos = 0;

int archive = -1;		/* File descriptor for archive file */

long baserec = 0L;

int time_to_start_writing = 0;	/* We've hit the end of the old stuff,*/

extern int rtme_version1;
extern int rtme_version2;
extern char rtme_versionletra;

static int hit_eof = 0;

struct stat hstat;	/* Stat struct corresponding */

union record *head = NULL;	/* Points to current tape header */
int head_standard = 0;	/* Tape header is in ANSI format */

static time_t now = 0;		/* Current time */
static we_are_root = 0;		/* True if our effective uid == 0 */
static int notumask = ~0;	/* Masks out bits user doesn't want */


static union record **save_rec = NULL;
union record record_save_area;
static long saved_recno = NULL;

long save_totsize = NULL;		/* total size of file we are writing.  Only
valid if save_name is non_zero */
long save_sizeleft = NULL;		/* Where we are in the file we are writing.*/

struct link
{
   struct link *next;
   dev_t dev;
   ino_t ino;
   short linkcount;
   char name[1];
};

struct link *linklist = NULL;		/* Points to first link in list */

static int nolinks = 0;			/* Gets set if we run out of RAM */


struct saved_dir_info
{
   char *path;
   int mode;
   int atime;
   int mtime;
   struct saved_dir_info *next;
};

struct saved_dir_info *saved_dir_info_head = NULL;

static char *next_long_name = NULL, *next_long_link = NULL;

#ifndef S_ISLNK
#define lstat stat
#endif


#define		f_standard		(!f_oldarch)
#define	isodigit(c)	( ((c) >= '0') && ((c) <= '7') )

#ifndef FGVBSD
#define msg() /* */
#define msg_perror() /* */
#else
msg()
{
}
msg_perror()
{
}
#endif

static char *otro_destino = NULL;

#ifdef WIN32


#define mkdir(a,b) mkdir(a)

#else

#ifdef ESXENIX
#include <sys/ndir.h>
#else
#include <dirent.h>
#endif

#endif


#define NLENGTH(direct) (strlen((direct)->d_name))

void write_long ();



int (*Eliminador_Diagram)(char *) = NULL;

extern void (*write_interno)();
extern unsigned (*read_interno)();


extern int os_subtipo;

extern char server_request[128];

extern long total_out;
extern int  abandonalo_ya;
static int  gzip_aborta = 0;
static int  gzip_a_salir = 0;
static int  gzip_unsave_rec = 0;
static long gzip_continuacion = 0L;
static long gzip_realsize = 0L;
static int  gzip_continuafd = 0;
static int  gzip_inicio = 0;
static int  estoy_en_gzip = 0;
static char *gzip_buf;
static int  gzip_buf_len = 0;
static int  gzip_buf_punto = 0;
static char gzip_parcial[RECORDSIZE];
static int  gzip_parcial_len = 0;
static long gzip_skip = 0L;


extern char *act_key;
extern char *act_red;
extern char *act_instalacion[52];
extern char *codigo_de_sistema1;
extern char *codigo_de_sistema2;
extern char *codigo_de_sistema3;
extern char *codigo_de_sistema4;
extern char *act_servidor;


static int puede_grabar = 0;

static int instalacion_ok = 0;

static char **instalar_fijos      = NULL;
static int  ninstalar_fijos       = 0;

static char **instalar_varia      = NULL;
static int  ninstalar_varia       = 0;

static char **instalar_iconos     = NULL;
static int  ninstalar_iconos      = 0;

static char **instalar_submenu  = NULL;
static int  ninstalar_submenu   = 0;

static char **instalar_postsystem = NULL;
static int  ninstalar_postsystem  = 0;

static char **instalar_limpia     = NULL;
static int  ninstalar_limpia      = 0;

static char **textof_instalacion = NULL;
static int ntextof_instalacion = 0;

static char **textoi_instalacion = NULL;
static int ntextoi_instalacion = 0;

static char **variables_instalacion = NULL;
static int nvariables_instalacion = 0;

static char **stdini_instalacion = NULL;
static int nstdini_instalacion = 0;

static char **registro_instalacion = NULL;
static int nregistro_instalacion = 0;


extern char *dame_codigo_de_sistema();
extern char *dame_codigo_de_paquete();

static int p_version1 = 0;
static int p_version2 = 0;
static char p_versionletra = 'a';

extern char *lee_linea();

static char *memoria_instalacion[10] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};

#include <rinstal.h>

struct paquetes Paq[MAXIMO_PAQUETES];
int nPaq = 0;

int de_new_code(unsigned char *,unsigned short,unsigned int);

static struct info_paquete infp;
static long instalar_progreso     = 0L;

static char rellena_resto[512];
static int  rellena_modo          = 0;
static long rellena_size          = 0L;
int ultima_instalacion = 0;

#ifdef RWXARXA
#ifdef RWSERVER
extern int en_conexion_externa;
extern int en_conexion_externa_stop;
extern int remoto_ver1;
extern int remoto_ver2;
extern char remoto_verletra;
extern int remoto_que_sistema_es;
extern int remoto_es_sistema_grafico;
#endif
#endif


/********************************************************************************************/


#ifdef WIN32

int
mknod (path, mode, dev)
char *path;
unsigned short mode;
dev_t dev;
{
   int fd;
   
   errno = ENXIO;		
   return -1;			
}

int
link (path1, path2)
char *path1;
char *path2;
{
   char buf[256];
   int ifd, ofd;
   int nrbytes;
   int nwbytes;
   
   if ((ifd = rw_open_ (path1, O_RDONLY | O_BINARY)) < 0)
      return -1;
   if ((ofd = rw_open_ (path2, O_RDWR | O_BINARY | O_CREAT | O_TRUNC,S_IREAD | S_IWRITE)) < 0)
      return -1;  
   while ((nrbytes = rw_read_ (ifd, buf, sizeof (buf))) > 0)
			{
      if ((nwbytes = rw_write_ (ofd, buf, nrbytes)) != nrbytes)
      {
         nrbytes = -1;
         break;
      }
			}
   if ((nrbytes < 0) | (0 != rw_close_ (ifd)) | (0 != rw_close_ (ofd)))
			{
      unlink (path2);
      return -1;
			}
   return 0;
}


int
chown (path, uid, gid)
char *path;
int uid;
int gid;
{
			return 0;
}

int
geteuid ()
{
   return 0;
}

#endif


int
is_dot_or_dotdot (p)
char *p;
{
   return (p[0] == '.' && (p[1] == '\0' || (p[1] == '.' && p[2] == '\0')));
}


void fl_read ()
{
   int err;			
   
   
   if (estoy_en_gzip)
   {
						if (gzip_buf_len < blocksize)
                  {
                     if (gzip_parcial_len)
                     {
                        insta_aviso(0,"atencion tgz 1");
                     }
                     memcpy(gzip_parcial,gzip_buf+gzip_buf_punto,gzip_buf_len);
                     gzip_parcial_len = gzip_buf_len;
                     gzip_buf_len = 0;
                     gzip_buf_punto = 0;
                     gzip_a_salir = 1;	   
                     return;
                  }	 
                  if (gzip_parcial_len > blocksize)
                  {
                     insta_aviso(0,"atencion tgz 2");
                  }
                  err = 0;
                  if (gzip_parcial_len)
                  {
                     memcpy(ar_block->charptr,gzip_parcial,gzip_parcial_len);
                     err = gzip_parcial_len;
                     gzip_parcial_len = 0;
                  }
                  memcpy(ar_block->charptr+err,gzip_buf+gzip_buf_punto,blocksize-err);
                  gzip_buf_punto += (blocksize-err);
                  gzip_buf_len -= (blocksize-err);     
                  if (es_instalacion)
                     presenta_progreso(blocksize);
   }
   else
   {
						err = rw_read_(archive, ar_block->charptr, (int) blocksize);
                  if (err == blocksize)
                  {
                     if (es_instalacion)
                        presenta_progreso(blocksize);
                     return;
                  }
   }
   
   /* controlar nuevo volumen */  
   
   return;
}



void fl_write ()
{
   int err; 
   
   err = rw_write_(archive, ar_block->charptr, (int) blocksize);
   if (err != blocksize)
   {
						/* multi volumen */
   }
   
}


void
saverec (pointer)
union record **pointer;
{
   long offset;
   
   save_rec = pointer;
   offset = ar_record - ar_block;
   saved_recno = baserec + offset;
}


long
from_oct (digs, where)
register int digs;
register char *where;
{
   register long value;
   
   while (*where == ' ')
   {				
      where++;
      if (--digs <= 0)
         return -1;	
   }
   value = 0;
   while (digs > 0 && isodigit (*where))
   {			
      value = (value << 3) | (*where++ - '0');
      --digs;
   }
   
   if (digs > 0 && *where && *where != ' ')
      return -1;	
   
   return value;
}


void to_oct (value, digs, where)
register long value;
register int digs;
register char *where;
{
   
   --digs;			/* Trailing null slot is left alone */
   where[--digs] = ' ';		/* Put in the space, though */
   
   /* Produce the digits -- at least one */
   do
   {
      where[--digs] = '0' + (char) (value & 7);	/* one octal digit */
      value >>= 3;
   }
   while (digs > 0 && value != 0);
   
   /* Leading spaces, if necessary */
   while (digs > 0)
      where[--digs] = ' ';
   
}


/*
* Call to_oct (), then return nonzero iff the conversion failed.
*/
int checked_to_oct (value, digs, where)
register long value;
register int digs;
register char *where;
{
   long from_oct ();
   
   to_oct (value, digs, where);
   return from_oct (digs, where) != value;
}


union record *
endofrecs ()
{
   return ar_last;
}


void userec (rec)
union record *rec;
{
   while (rec >= ar_record)
      ar_record++;
      /*
      * Do NOT flush the archive here.  If we do, the same
      * argument to userec() could mean the next record (if the
      * input block is exactly one record long), which is not what
      * is intended.
   */
   if (ar_record > ar_last)
      abort ();
}


union record *
start_header (name, st)
char *name;
register struct stat *st;
{
   register union record *header;
   
   if (strlen (name) >= NAMSIZ)
      write_long (name, LF_LONGNAME);
   
   header = (union record *) findrec ();
   bzero (header->charptr, sizeof (*header));	/* XXX speed up */
   
                                                /*
                                                * Check the file name and put it in the record.
   */
   if (!f_absolute_paths)
   {
      static int warned_once = 0;
#ifdef WIN32
      if (name[1] == ':')
      {
         name += 2;
         if (!warned_once++)
            /*msg ("Removing drive spec from names in the archive")*/;
      }
#endif
      while ('/' == *name)
      {
         name++;		/* Force relative path */
         if (!warned_once++)
            /*msg ("Removing leading / from absolute path names in the archive.")*/;
      }
   }
   strcpy(current_file_name,name);
   strncpy (header->header.arch_name, name, NAMSIZ);
   header->header.arch_name[NAMSIZ - 1] = '\0';
   
   to_oct ((long) (f_oldarch ? (st->st_mode & 07777) : st->st_mode),
      8, header->header.mode);
   to_oct ((long) st->st_uid, 8, header->header.uid);
   to_oct ((long) st->st_gid, 8, header->header.gid);
   to_oct ((long) st->st_size, 1 + 12, header->header.size);
   to_oct ((long) st->st_mtime, 1 + 12, header->header.mtime);
   sprintf(header->header.uname,"%d",st->st_uid);
   sprintf(header->header.gname,"%d",st->st_gid);    
   if (f_standard)
   {
      header->header.linkflag = LF_NORMAL;	/* New default */
      strcpy (header->header.magic, TMAGIC);	/* Mark as Unix Std */
   }
   return header;
}

/*
* Finish off a filled-in header block and write it out.
* We also print the file name and/or full info if verbose is on.
*/
void
finish_header (header)
register union record *header;
{
   register int i, sum;
   register char *p;
   
   bcopy (CHKBLANKS, header->header.chksum, sizeof (header->header.chksum));
   
   sum = 0;
   p = header->charptr;
   for (i = sizeof (*header); --i >= 0;)
   {
   /*
   * We can't use unsigned char here because of old compilers,
   * e.g. V7.
      */
      sum += 0xFF & *p++;
   }
   
   /*
   * Fill in the checksum field.  It's formatted differently
   * from the other fields:  it has [6] digits, a null, then a
   * space -- rather than digits, a space, then a null.
   * We use to_oct then write the null in over to_oct's space.
   * The final space is already there, from checksumming, and
   * to_oct doesn't modify it.
   *
   * This is a fast way to do:
   * (void) sprintf(header->header.chksum, "%6o", sum);
   */
   to_oct ((long) sum, 8, header->header.chksum);
   header->header.chksum[6] = '\0';	/* Zap the space */
   
   userec (header);
   
   return;
}


void
write_long (p, type)
char *p;
char type;
{
   int size = strlen (p) + 1;
   int bufsize;
   union record *header;
   struct stat foo;
   
   
   bzero (&foo, sizeof foo);
   foo.st_size = size;
   
   header = start_header ("././@LongLink", &foo);
   header->header.linkflag = type;
   finish_header (header);
   
   header = findrec ();
   
   bufsize = endofrecs ()->charptr - header->charptr;
   
   while (bufsize < size)
   {
      bcopy (p, header->charptr, bufsize);
      p += bufsize;
      size -= bufsize;
      userec (header + (bufsize - 1) / RECORDSIZE);
      header = findrec ();
      bufsize = endofrecs ()->charptr - header->charptr;
   }
   bcopy (p, header->charptr, size);
   bzero (header->charptr + size, bufsize - size);
   userec (header + (size - 1) / RECORDSIZE);
}



void flush_archive ()
{
   int c;
   long _baserec = baserec;
   union record *_ar_record = ar_record;
   union record *_ar_last = ar_last;
   
   baserec += ar_last - ar_block;/* Keep track of block #s */
   ar_record = ar_block;		/* Restore pointer to start */
   ar_last = ar_block + blocking;/* Restore pointer to end */
   
   if (ar_reading)
   {
      if (time_to_start_writing)
      {
         time_to_start_writing = 0;
         ar_reading = 0;
         fl_write ();
      }
      else
      {
         fl_read();
         if (gzip_a_salir)
         {
            baserec = _baserec;
            ar_record = _ar_record;
            ar_last = _ar_last;
            return;
         }  
      }
   }
   else
   {
      fl_write();
   }
}


union record *findrec ()
{
   if (ar_record == ar_last)
   {
      if (hit_eof)
         return (union record *) NULL;	/* EOF */
      flush_archive ();
      if (gzip_a_salir)
      {
         return(NULL);
      }
      if (ar_record == ar_last)
      {
         hit_eof++;
         return (union record *) NULL;	/* EOF */
      }
   }
   return ar_record;
}


int open_archive (reading)
int reading;
{
   /* msg_file = f_exstdout ? stderr : stdout; !!!!!*/
   
   archive = -1;
   if (blocksize == 0)
   {
      /*msg ("invalid value for blocksize");*/
      return(-1);
   }
   
   if (n_ar_files == 0)
   {
      /*msg ("No archive name given, what should I do?");*/
      return(-1);
   }
   
   ar_block = (union record *) malloc ((unsigned) blocksize);
   if (!ar_block)
   {
      /*msg ("could not allocate memory for blocking factor %d",blocking);*/
      return(-1);
   }
   
   ar_record = ar_block;
   ar_last = ar_block + blocking;
   ar_reading = reading;
   
   
   if (!estoy_en_gzip)
   {
      if (reading == 2)
      {
         archive = rw_open_(ar_files[0], O_RDWR | O_CREAT | O_BINARY, 0666);
      }
      else if (reading)
      {
         archive = rw_open_(ar_files[0], O_RDONLY | O_BINARY, 0666);
      }
      else
      {
         archive = rw_open_(ar_files[0],  O_RDWR | O_BINARY | O_CREAT | O_TRUNC, 0666);
      }
      if (archive < 0)
      {
         /*msg_perror ("can't open %s", ar_files[0]);*/
         return(-1);
      }
   }
   else
      archive = 31000;
   
   if (reading)
   {
      ar_last = ar_block;	/* Set up for 1st block = # 0 */
      (void) findrec ();	/* Read it in, check for EOF */
   }
   return(0);
}

void
write_eot ()
{
   union record *p;
   int bufsize;
   
   p = findrec ();
   if (p)
   {
      bufsize = endofrecs ()->charptr - p->charptr;
      bzero (p->charptr, bufsize);
      userec (p);
   }
}


void
dump_file (inip, curdev, toplevel)
char *inip;			/* File name to dump */
int curdev;		/* Device our parent dir was on */
int toplevel;		/* Whether we are a toplevel call */
{
   union record *header;
   char type;
   extern char *save_name;	/* JF for multi-volume support */
   extern long save_sizeleft;
   union record *exhdr;
   char save_linkflag;
   extern time_t new_time;
   int critical_error = 0;
   struct utimbuf restore_times;
   char p[260];
   
   
   
   while (!f_absolute_paths && *inip == '/')
   {
      inip++;
   }
   
   if (otro_destino)
   { 
      strcpy(p,otro_destino);
   }
   else
   {
      p[0] = 0;
   }
   strcat(p,inip);
   
   stat (p, &hstat);
   restore_times.actime = hstat.st_atime;
   restore_times.modtime = hstat.st_mtime;
   
   /*
   * Check for multiple links.
   *
   * We maintain a list of all such files that we've written so
   * far.  Any time we see another, we check the list and
   * avoid dumping the data again if we've done it once already.
   */
   if (hstat.st_nlink > 1
      && (S_ISREG (hstat.st_mode)
#ifdef S_ISCTG
      || S_ISCTG (hstat.st_mode)
#endif
#ifdef S_ISCHR
      || S_ISCHR (hstat.st_mode)
#endif
#ifdef S_ISBLK
      || S_ISBLK (hstat.st_mode)
#endif
#ifdef S_ISFIFO
      || S_ISFIFO (hstat.st_mode)
#endif
      ))
   {
      register struct link *lp;
      
      /* First quick and dirty.  Hashing, etc later FIXME */
      ;	  
      for (lp = linklist; lp; lp = lp->next)
      {
         if (lp->ino == hstat.st_ino &&
            lp->dev == hstat.st_dev)
         {
            char *link_name = lp->name;
            
            /* We found a link. */
            
            if (link_name - lp->name >= NAMSIZ)
               write_long (link_name, LF_LONGLINK);
            strcpy(current_link_name,link_name);
            
            hstat.st_size = 0;
            header = start_header (p, &hstat);
            if (header == NULL)
            {
               critical_error = 1;
               goto badfile;
            }
            strncpy (header->header.arch_linkname,
               link_name, NAMSIZ);
            
            /* Force null truncated */
            header->header.arch_linkname[NAMSIZ - 1] = 0;
            
            header->header.linkflag = LF_LINK;
            finish_header (header);
            /* FIXME: Maybe remove from list after all links found? */
            if (f_remove_files)
            {
               if (unlink (p) == -1)
                  /*msg_perror ("cannot remove %s", p)*/;
            }
            return;		/* We dumped it */
         }
      }
      
      /* Not found.  Add it to the list of possible links. */
      lp = (struct link *) malloc ((unsigned) (sizeof (struct link) + strlen (p)));
      if (!lp)
      {
         if (!nolinks)
         {
         /*msg (
            "no memory for links, they will be dumped as separate files");*/
            nolinks++;
         }
      }
      lp->ino = hstat.st_ino;
      lp->dev = hstat.st_dev;
      strcpy (lp->name, p);
      lp->next = linklist;
      linklist = lp;
   }
   
   /*
   * This is not a link to a previously dumped file, so dump it.
   */
   if (S_ISREG (hstat.st_mode)
#ifdef S_ISCTG
      || S_ISCTG (hstat.st_mode)
#endif
      )
   {
      int f;			/* File descriptor */
      long bufsize, count;
      long sizeleft;
      register union record *start;      
      
      /*		int	end_nulls = 0; */      
      
      
      sizeleft = hstat.st_size;
      /* Don't bother opening empty, world readable files. */
      if (sizeleft > 0 || 0444 != (0444 & hstat.st_mode))
      {
         f = rw_open_ (p, O_RDONLY | O_BINARY);
         if (f < 0)
            goto badperror;
      }
      else
      {
         f = -1;
      }
      
      /* If the file is sparse, we've already taken care of this */
      header = start_header (inip, &hstat);
      if (header == NULL)
      {
         if (f >= 0)
            (void) rw_close_ (f);
         critical_error = 1;
         goto badfile;
      }
      
#ifdef S_ISCTG
      /* Mark contiguous files, if we support them */
      if (f_standard && S_ISCTG (hstat.st_mode))
      {
         header->header.linkflag = LF_CONTIG;
      }
#endif    
      save_linkflag = header->header.linkflag;
      finish_header (header);
      
      while (sizeleft > 0)
      {
         start = findrec ();
         bufsize = endofrecs ()->charptr - start->charptr;
         if (sizeleft < bufsize)
         {
            /* Last read -- zero out area beyond */
            bufsize = (int) sizeleft;
            count = bufsize % RECORDSIZE;
            if (count)
               bzero (start->charptr + sizeleft,(int) (RECORDSIZE - count));
         }
         
         count = rw_read_ (f, start->charptr, bufsize);
         
         if (count < 0)
         {
         /*msg_perror ("read error at byte %ld, reading\
            %d bytes, in file %s", (long) hstat.st_size - sizeleft, bufsize, p);*/
            goto padit;
         }
         sizeleft -= count;
         
         /* This is nonportable (the type of userec's arg). */
         userec (start + (count - 1) / RECORDSIZE);
         
         if (count == bufsize)
            continue;
         /*msg ("file %s shrunk by %d bytes, padding with zeros.", p, sizeleft);*/
         goto padit;		/* Short read */
      }
      
      if (f >= 0)
         (void) rw_close_ (f);
      
      if (f_remove_files)
      {
         if (unlink (p) == -1)
            /*msg_perror ("cannot remove %s", p)*/;
      }
      if (f_atime_preserve)
         utime (p, &restore_times);
      return;
      /*
      * File shrunk or gave error, pad out tape to match
      * the size we specified in the header.
      */
padit:
      while (sizeleft > 0)
      {
         save_sizeleft = sizeleft;
         start = findrec ();
         bzero (start->charptr, RECORDSIZE);
         userec (start);
         sizeleft -= RECORDSIZE;
      }
      if (f >= 0)
         (void) rw_close_ (f);
      if (f_atime_preserve)
         utime (p, &restore_times);
      return;
    }
#ifdef COMENTARIO
#ifdef S_ISLNK
    else if (S_ISLNK (hstat.st_mode))
    {
       int size;
       char *buf = alloca (PATH_MAX + 1);
       
       size = readlink (p, buf, PATH_MAX + 1);
       if (size < 0)
          goto badperror;
       buf[size] = '\0';
       if (size >= NAMSIZ)
          write_long (buf, LF_LONGLINK);
       strcpy(current_link_name,buf);
       
       hstat.st_size = 0;	/* Force 0 size on symlink */
       header = start_header (inip, &hstat);
       if (header == NULL)
       {
          critical_error = 1;
          goto badfile;
       }
       strncpy (header->header.arch_linkname, buf, NAMSIZ);
       header->header.arch_linkname[NAMSIZ - 1] = '\0';
       header->header.linkflag = LF_SYMLINK;
       finish_header (header);	/* Nothing more to do to it */
       if (f_remove_files)
       {
          if (unlink (p) == -1)
             /*msg_perror ("cannot remove %s", p)*/;
       }
       return;
    }
#endif
#endif
    else if (S_ISDIR (hstat.st_mode))
    {
       register DIR *dirp;
#ifdef ESXENIX
       register struct direct *d;
#else
       register struct dirent *d;
#endif
       char *namebuf;
       int buflen;
       register int len;
       int our_device = hstat.st_dev;
       
       /* Build new prototype name */
       len = strlen (inip);
       buflen = len + NAMSIZ;
       namebuf = malloc (buflen + 1);
       strncpy (namebuf, inip, buflen);
       while (len >= 1 && '/' == namebuf[len - 1])  len--;			/* Delete trailing slashes */
       namebuf[len++] = '/';	/* Now add exactly one back */
       namebuf[len] = '\0';	/* Make sure null-terminated */
       
                              /*
                              * Output directory header record with permissions
                              * FIXME, do this AFTER files, to avoid R/O dir problems?
                              * If old archive format, don't write record at all.
       */
       if (!f_oldarch)
       {
          hstat.st_size = 0;	/* Force 0 size on dir */
                              /*
                              * If people could really read standard archives,
                              * this should be:		(FIXME)
                              header = start_header(f_standard? p: namebuf, &hstat);
                              * but since they'd interpret LF_DIR records as
                              * regular files, we'd better put the / on the name.
          */
          header = start_header (namebuf, &hstat);
          if (header == NULL)
          {
             critical_error = 1;
             goto badfile;	/* eg name too long */
          }
          
          if (f_standard)
             header->header.linkflag = LF_DIR;	    
          
          finish_header (header);	/* Done with directory header */
       } 
       
       /* Now output all the files in the directory */
       /*
       * See if we are crossing from one file system to another,
       * and avoid doing so if the user only wants to dump one file system.
       */
       errno = 0;
       dirp = opendir (p);
       if (!dirp)
       {
          if (errno)
          {
             /*msg_perror ("can't open directory %s", p);*/
          }
          else
          {
          /*msg ("error opening directory %s",
             p);*/
          }
          return;
       }
       
       /* Hack to remove "./" from the front of all the file names */
       if (len == 2 && namebuf[0] == '.' && namebuf[1] == '/')
          len = 0;
       
       /* Should speed this up by cd-ing into the dir, FIXME */
       while (NULL != (d = readdir (dirp)))
       {
          /* Skip . and .. */
          if (is_dot_or_dotdot (d->d_name))
             continue;
          
          if (NLENGTH (d) + len >= buflen)
          {
             buflen = len + NLENGTH (d);
             namebuf = realloc (namebuf, buflen + 1);
             /* namebuf[len]='\0';
             msg("file name %s%s too long",
             namebuf, d->d_name);
             continue; */
          }
          strcpy (namebuf + len, d->d_name);
          if (f_exclude && check_exclude (namebuf))
             continue;
          dump_file (namebuf, our_device, 0);
       }
       
       closedir (dirp);
       free (namebuf);
       if (f_atime_preserve)
          utime (p, &restore_times);
       return;
    }
#ifdef S_ISCHR
    else if (S_ISCHR (hstat.st_mode))
    {
       type = LF_CHR;
    }
#endif
#ifdef S_ISBLK
    else if (S_ISBLK (hstat.st_mode))
    {
       type = LF_BLK;
    }
#endif
    /* Avoid screwy apollo lossage where S_IFIFO == S_IFSOCK */
#if (_ISP__M68K == 0) && (_ISP__A88K == 0) && defined(S_ISFIFO)
    else if (S_ISFIFO (hstat.st_mode))
    {
       type = LF_FIFO;
    }
#endif
#ifdef S_ISSOCK
    else if (S_ISSOCK (hstat.st_mode))
    {
       type = LF_FIFO;
    }
#endif
    else
       goto unknown;
    
    if (!f_standard)
       goto unknown;
    
    hstat.st_size = 0;		/* Force 0 size */
    header = start_header (p, &hstat);
    if (header == NULL)
    {
       critical_error = 1;
       goto badfile;		/* eg name too long */
    }
    header->header.linkflag = type;
    finish_header (header);
    if (f_remove_files)
    {
       if (unlink (p) == -1)
          /*msg_perror ("cannot remove %s", p)*/;
    }
    return;
    
unknown:
badperror:
badfile:
    /*msg ("%s: Unknown file type; file ignored.", p)*/;
}



void close_archive()
{  
   int c;
   register struct link *lp,*lpn;
   
   if (archive != -1)
   {
      if (time_to_start_writing || !ar_reading)
         flush_archive ();
      
      if (!estoy_en_gzip)
      {
         if ((c = rw_close_(archive)) < 0)
            /*msg_perror ("Warning: can't close %s(%d,%d)", ar_files[cur_ar_file], archive, c)*/;
      }
   }
   
   if (ar_block)
      free(ar_block);
   
   if (linklist)
   {
      lp = linklist;
      lpn = linklist->next;
      while(lp)
      {
         lpn = lp->next;
         free(lp);
         lp = lpn;
      }    
   }
   linklist = NULL;
   
   if (next_long_link) free(next_long_link);
   if (next_long_name) free(next_long_name);
   next_long_link = next_long_name = NULL;
   
}



int check_exclude ()
{
   return(0);
}



void
decode_header (header, st, stdp, wantug)
register union record *header;
register struct stat *st;
int *stdp;
int wantug;
{
   long from_oct ();
   
   st->st_mode = from_oct (8, header->header.mode);
   st->st_mode &= 07777;
   st->st_mtime = from_oct (1 + 12, header->header.mtime);
   
   if (0 == strcmp (header->header.magic, TMAGIC))
   {
      /* Unix Standard tar archive */
      *stdp = 1;
      if (wantug)
      {
         st->st_uid = from_oct (8, header->header.uid);
         st->st_gid = from_oct (8, header->header.gid);
      }
   }
   else
   {
      /* Old fashioned tar archive */
      *stdp = 0;
      st->st_uid = from_oct (8, header->header.uid);
      st->st_gid = from_oct (8, header->header.gid);
      st->st_rdev = 0;
   }
}


void
skip_file (size)
register long size;
{
   union record *x;
   extern long save_totsize;
   extern long save_sizeleft;
   
   while (size > 0)
			{ 
      x = findrec ();
      if (gzip_a_salir)
      {
         if ( (size%RECORDSIZE) )
         {
            gzip_skip = size + RECORDSIZE - (size%RECORDSIZE);
         }
         else
            gzip_skip = size;
         return;
      }
      if (x == NULL)
      {			/* Check it... */
         /*msg ("Unexpected EOF on archive file");*/
         return;
      }
      if (es_instalacion == 1)
      {
         rellena_instalacion(x->charptr);
      }
      userec (x);
      size -= RECORDSIZE;
			}
}


int
make_dirs (pathname)
char *pathname;
{
			char *p;			/* Points into path */
         int madeone = 0;		/* Did we do anything yet? */
         int save_errno = errno;	/* Remember caller's errno */
         int check;
         
         /*
         if (errno != ENOENT)
         return 0;			/* Not our problem */
         
         for (p = index (pathname, '/'); p != NULL; p = index (p + 1, '/'))
         {
            /* Avoid mkdir of empty string, if leading or double '/' */
            if (p == pathname || p[-1] == '/')
               continue;
            /* Avoid mkdir where last part of path is '.' */
            if (p[-1] == '.' && (p == pathname + 1 || p[-2] == '/'))
               continue;
            if (p[-1] == ':')
               continue;
            *p = 0;			/* Truncate the path there */
            check = mkdir (pathname, 0777);	/* Try to create it as a dir */
            if (check == 0)
            {
               /* Fix ownership */
               if (we_are_root)
               {
                  if (chown (pathname, hstat.st_uid,
                     hstat.st_gid) < 0)
                  {
                     /*msg_perror ("cannot change owner of %s to uid %d gid %d", pathname, hstat.st_uid, hstat.st_gid);*/
                  }
               }	  
               madeone++;		/* Remember if we made one */
               *p = '/';
               continue;
            }
            *p = '/';
            if (errno == EEXIST)	/* Directory already exists */
               continue;
               /*
               * Some other error in the mkdir.  We return to the caller.
            */
            break;
         }
         
         errno = save_errno;		/* Restore caller's errno */
         return madeone;		/* Tell them to retry if we made one */
}



int
read_header ()
{
   register int i;
   register long sum, signed_sum, recsum;
   register char *p;
   register union record *header;
   long from_oct ();
   char **longp;
   char *bp, *data;
   int size, written;  
   char *name;
   
recurse:
   
   header = findrec ();
   if (gzip_a_salir)
   {
      return(EOF);
   }
   head = header;		/* This is our current header */
   if (NULL == header)
      return EOF;
   
   recsum = from_oct (8, header->header.chksum);
   
   signed_sum = sum = 0;
   p = header->charptr;
   for (i = sizeof (*header); --i >= 0;)
   {
   /*
   * We can't use unsigned char here because of old compilers,
   * e.g. V7.
      */
      signed_sum += *p;
      sum += 0xFF & *p++;
   }
   
   /* Adjust checksum to count the "chksum" field as blanks. */
   for (i = sizeof (header->header.chksum); --i >= 0;)
   {
      sum -= 0xFF & header->header.chksum[i];
      signed_sum -= (char) header->header.chksum[i];
   }
   sum += ' ' * sizeof header->header.chksum;
   signed_sum += ' ' * sizeof header->header.chksum;
   
   if (sum == 8 * ' ')
   {
   /*
   * This is a zeroed record...whole record is 0's except
   * for the 8 blanks we faked for the checksum field.
      */
      return 2;
   }
   
   if (sum != recsum && signed_sum != recsum)
   {
      return 0;
   }
   
   /*
   * Good record.  Decode file size and return.
   */
   if (header->header.linkflag == LF_LINK)
      hstat.st_size = 0;		/* Links 0 size on tape */
   else
      hstat.st_size = from_oct (1 + 12, header->header.size);
   
   header->header.arch_name[NAMSIZ - 1] = '\0';
   if (header->header.linkflag == LF_LONGNAME
      || header->header.linkflag == LF_LONGLINK)
   {
      longp = ((header->header.linkflag == LF_LONGNAME)
         ? &next_long_name
         : &next_long_link);
      
      userec (header);
      if (*longp)
         free (*longp);
      bp = *longp = (char *) malloc (hstat.st_size);
      
      for (size = hstat.st_size;
      size > 0;
      size -= written)
						{
         data = findrec ()->charptr;
         if (data == NULL)
         {
            /*msg ("Unexpected EOF on archive file");*/
            break;
         }
         written = endofrecs ()->charptr - data;
         if (written > size)
            written = size;
         
         bcopy (data, bp, written);
         bp += written;
         userec ((union record *) (data + written - 1));
						}
      goto recurse;
   }
   else
   {
      name = (next_long_name
         ? next_long_name
         : head->header.arch_name);
      strcpy (current_file_name, name);
      name = (next_long_link
         ? next_long_link
         : head->header.arch_linkname);      
      strcpy (current_link_name, name);
      if (next_long_link) free(next_long_link);
      if (next_long_name) free(next_long_name);
      next_long_link = next_long_name = NULL;
      return 1;
   }
}


static int instala_font_(fname)
char *fname;
{
#ifdef FGVDOS
#ifndef RWSERVER
	char tmp[512];
	char *p;
	int i;
	struct stat st1;

	if (!strlen(quita_path(fname)))
		return 0;

	if (stat (fname, &st1) == -1)
	{
		return(0);
	}

	if (S_ISDIR (st1.st_mode))
	{
		return 0;
	}

	for (i = 0;fname[i];i++)
	{
		if (fname[i] == '/')
			tmp[i] = '\\';
		else
			tmp[i] = fname[i];
	}
	tmp[i] = 0;

	strcpy(tmp,"C:\\WINDOWS");
	p = getenv("windir");
	if (p)
		strcpy(tmp,p);
	strcat(tmp,"\\Fonts\\");
	strcat(tmp,quita_path(fname));
	copiafich_win_(fname,tmp,1);
#endif
#endif
	return 0;
}


static int instala_pon_en_su_sitio(fname)
char *fname;
{
   int skipcrud,i;
   int file,fileo,nrbytes,nwbytes,f1,ann;
   long numero,t1,data2,data,midata;
   unsigned long signature = 0x777;
   char buf1[258];
   char tmp_c[258];
   char tmp_b[258];
   struct stat st1;
   struct stat buffer;
   struct utimbuf tb;
   struct tm *lc;
   
   
   if (otro_destino)
   {
      if ((i = strlen(fname)))
      {
         i--;
         if (fname[i] == '/' || fname[i] == '\\')
         {
            fname[i] = 0; /* esto no importa que se quede asi en este caso!!! */
         }
         if (i < 4)
            return(0);
      }	 
      if (stat (fname, &st1) == -1)
      {
         return(0);
      }
      skipcrud = strlen(otro_destino)+4;
      strcpy(tmp_c,otro_destino);
      strcat(tmp_c,fname+skipcrud);
      
      if (S_ISDIR (st1.st_mode))
      {
         if (mkdir(tmp_c,(we_are_root ? 0 : 0300) | (int) hstat.st_mode))
         {
            make_dirs (tmp_c);
            mkdir(tmp_c,(we_are_root ? 0 : 0300) | (int) hstat.st_mode);
         }
         return(1);
      }
      
      file = rw_open_(tmp_c,O_BINARY | O_RDONLY);
      if (file >= 0)
      {
         rw_close_(file);
         return(0);
      } 
      fileo = rw_open_(fname,O_BINARY | O_RDONLY);
      if (fileo < 0)
      {
         return(0);
      }
      do 
      {
         file = rw_open_(tmp_c,O_BINARY | O_RDWR | O_CREAT | O_TRUNC,S_IREAD | S_IWRITE);
         if (file < 0)
         {
            if (make_dirs(tmp_c)) continue;
            return(0);
         }
         break;
      } while(-1);
      while ((nrbytes = rw_read_ (fileo, tmp_b, sizeof (tmp_b))) > 0)
      {
         if ((nwbytes = rw_write_ (file, tmp_b, nrbytes)) != nrbytes)
         {
            nrbytes = -1;
            break;
         }
      }
      rw_close_(file);
      rw_close_(fileo);     
      chmod(tmp_c,st1.st_mode);
      if (strbusca(tmp_c,"ibm80.bak") > -1)
      {        
         f1 = rw_open_(tmp_c,O_RDWR | O_BINARY);
         if (f1 >= 0)
         {   
            
            fstat( f1, &buffer );
            midata = buffer.st_mtime;        
            lc = localtime(&midata);
            if (lc) 
            {
               ann = lc->tm_year;
               if (ann > 50)
                  ann += 1900;
               else
                  ann += 2000;
               sprintf(buf1,"%02d%02d%04d",lc->tm_mday,(lc->tm_mon)+1 ,ann);  
               midata = atol(buf1);
            }
            numero = 0;
            data2 = 0;
            data = 0; /* siempre la fecha del sistema */
            rw_lseek_(f1,0L,SEEK_SET);
            rw_write_(f1,&signature,sizeof(long));
            t1 = numero ^ midata;
            rw_write_(f1,&t1,sizeof(long));
            t1 = data2 ^ midata;
            rw_write_(f1,&t1,sizeof(long));
            t1 = data ^ midata;
            rw_write_(f1,&t1,sizeof(long));
            t1 = numero ^ data;
            rw_write_(f1,&t1,sizeof(long));
            rw_close_(f1);
            tb.modtime = buffer.st_mtime;
            tb.actime = buffer.st_mtime;
            utime( tmp_c, &tb );
         }
      }
      return(1);
   }
   
   return(0);
}


#ifdef FGVDOS
BOOL CALLBACK _INSTALACION(HWND hWnd, UINT message,WPARAM wParam, LPARAM lParam)
{
   
   
   
   switch (message)
   {
   case WM_SYSCOMMAND:      
      break;
   case WM_CHAR:
      break;
   case WM_COMMAND:
      switch(wParam)
      {
      case IDOK:
         empieza_dlg = 1;
         return FALSE;
      case IDCANCEL:
         empieza_dlg = 2;
         return(FALSE);
      }	 
      break;
      case WM_INITDIALOG:
         return(FALSE);
         break;
      case WM_DESTROY:
         if (wPadre)
         {
            SetActiveWindow(wPadre);
            SetFocus(wPadre);
         }
         dlg = NULL;
         break;
      default:
         break;
   }
   
   return FALSE; 
}
#endif

int insta_mensaje(int p,char *mensa)
{
#ifdef FGVDOS
   if (dlg)
   {
      SetWindowText(dlg,mensa);
      return(0);
   }
#else
   return(mensaje(p,mensa));
#endif
}

int insta_aviso(int p,char *mensa)
{
#ifdef FGVDOS
   if (dlg)
   {
      FlashWindow(dlg,TRUE);  
      MessageBox(NULL,mensa,"AVISO",MB_OK | MB_ICONHAND);   
      FlashWindow(dlg,FALSE);
      return(0);
   }
#else
   return(aviso(p,mensa));
#endif
}


int insta_confirmar(int p,char *mensa,char *def)
{
#ifdef FGVDOS
   if (dlg)
   {
      int modo = MB_YESNO | MB_ICONHAND;
      if (def[0] != 'S' && def[0] != 's')
         modo |= MB_DEFBUTTON2;
      if (MessageBox(NULL,mensa,"ATENCION",modo) == IDYES)
      {
         return(0);
      }
      return(-1);
   }
#endif
   return(confirmar(p,mensa,def));
}


#ifdef FGVDOS
static int crea_i_dialogo()
{
   if (dlg) {DestroyWindow(dlg);}
   hInstacia = GetModuleHandle (NULL);

   dlg = CreateDialogParam(  hInstacia,  // handle to application instance
      "INSTALACION",  // identifies dialog box template
      NULL,      // handle to owner window
      _INSTALACION, // pointer to dialog box procedure
      (LPARAM)NULL);   // initialization value
   BringWindowToTop(dlg);
	  SetForegroundWindow(dlg);
}
#endif

static int presenta_progreso(progreso)
int progreso;
{
   int i,por;
   char tmp[81];
   static int ult_por;
   static int para_pintar = 0;
   
   
   
   if (total_out < 1L || !progreso || es_sacarwi) return(0);
   
   if (instalacion_en_grafico)
   {
#ifdef FGVDOS
   MSG msg;
      
      if (progreso < 0) 
      {
         if (dlg)
         {
            DestroyWindow(dlg);
            dlg = NULL;
         }
         return(0);
      }
      
      if (!instalar_progreso && instalacion_silenciosa < 2)
      {
         if (!dlg)
         {      
            crea_i_dialogo();
         }   
         para_pintar = 1;
         ult_por = -1;
      }
      
      if (es_instalacion > 1 && para_pintar == 1 && instalacion_silenciosa < 2)
      {
         if (dlg)
            SetDlgItemText(dlg,IDC_VER1,infp.descrip);
         para_pintar = 0;
      }
      
      instalar_progreso += progreso;
      
      if (instalacion_silenciosa >= 2) return(0);
      
      por = (int)(long)(((instalar_progreso/100L) * 100L) / (total_out/100L));
      if (por == ult_por) return(0);
      
      SendMessage(GetDlgItem(dlg,IDC_PROGRESO),PBM_SETPOS,(WPARAM)por,(LPARAM)NULL);

      while (PeekMessage((LPMSG)&msg,dlg,0,0,PM_NOREMOVE) && GetMessage((LPMSG)&msg,dlg,0,0))
      {
        if (!IsDialogMessage(dlg,&msg))
        {
           TranslateMessage((LPMSG)&msg);
           DispatchMessage((LPMSG)&msg);
        }	     
      }         

      ult_por = por;
      
#endif
      
   }
   else
   {
      
      if (progreso < 0) return(0);
      
      if (!instalar_progreso && instalacion_silenciosa < 2)
      {
#ifdef FGVWIN
         Nor();
         zonablan(1805,2275);
         cuadro(1805,2275);  
#else
#ifndef RWSERVER
         Color(7,1);
#endif
         Nor();  
         zonablan(1805,2275);  
#ifndef RWSERVER
         Color(7,1);
#endif
         Nor();
         tmp[1] = 0;
         tmp[0] = 218;
         pprint(1805,tmp);
         cuadro(1905,2105);
         tmp[1] = 0;
         tmp[0] = 192;  
         pprint(2205,tmp);
         cuadro(1806,1874);
         tmp[1] = 0;
         tmp[0] = 191;
         pprint(1875,tmp);  
#ifndef RWSERVER
         Color(0,1);
#endif
         Nor();
         cuadro(2206,2274);
         tmp[1] = 0;
         tmp[0] = 217;  
         pprint(2275,tmp);
         cuadro(2175,1975);
         Nor();
#endif
#ifdef FGVWIN
         memset(tmp,' ',60);
#else   
         memset(tmp,178,60);
#endif
         tmp[60] = 0;
         para_pintar = 1;
#ifndef RWSERVER
         Color(7,4);
#endif
         Ar();
         pprint(2010,tmp);
         Nor();
         ult_por = -1;
      }
      
      if (es_instalacion > 1 && para_pintar == 1 && instalacion_silenciosa < 2)
      {
         Ai();
         pprint(1910,infp.descrip);
         Bi();
         para_pintar = 0;
      }
      instalar_progreso += progreso;
      
      if (instalacion_silenciosa >= 2) return(0);
      
      por = (int)(long)(((instalar_progreso/100L) * 100L) / (total_out/100L));
      if (por == ult_por) return(0);
      ult_por = por;
      
      for (i = 0;i < (int)(long)(((instalar_progreso/100L) * 60L) / (total_out/100L));i++)
      {
#ifdef FGVWIN
         tmp[i] = ' ';
#else
         tmp[i] = 178;
#endif
      }
      tmp[i] = 0;
#ifndef RWSERVER
      Color(7,2);
#endif
      Nor();
      pprint(2010,tmp);
      Nor();
      sprintf(tmp,"%3d%%",por);
      pprint(2137,tmp);
#ifdef FGVUNIX
      fflush(stdout);
#endif
   } /* instalacion en grafico*/
   
   return(0);
}





static int cambia_por_destino(ori,destino)
char *ori;
char *destino;
{
   int i,j,ti,n;
   int moder = 0;
   char tmp[256];
   char tmp2[128];
   
   for (j = 0,i = 0;ori[i];i++)
   {
      if (ori[i] == '%')
      {
         if (ori[i+1] == '!')
         {
            if (destino)	    
            {
               strcpy(tmp2,destino);
               for (ti = 0;tmp2[ti];ti++)
               {
                  if (tmp2[ti] == '/') 
                     tmp2[ti] = '\\';
               }        
               if (ti)
               {
                  ti--;
                  if (tmp2[ti] == '\\')
                  {
                     tmp2[ti] = 0;
                  }
               }
               strcpy(tmp+j,tmp2);
               j += strlen(tmp2);	    
            }
            else
            {
#ifdef FGVDOS
               strcpy(tmp+j,"\\ds");
#else
               strcpy(tmp+j,"/ds");
#endif
               j += 4;
            } 
         }
         else
            if (ori[i+1] == '.')
            {
               if (destino)
               {
                  strcpy(tmp2,destino);
                  cambia_barra(tmp2);
                  ti = strlen(tmp2);
                  if (ti)
                  {
                     ti--;
                     if (tmp2[ti] == '/')
                        tmp2[ti] = 0;
                  }
                  strcpy(tmp+j,tmp2);
                  j += strlen(tmp2);	    
               }
               else
               {
                  strcpy(tmp+j,DEFAULTDIRBASE);
                  j += 4;
               } 
            }
            else
               if (ori[i+1] == '%')
               {
                  tmp[j] = '%';
                  j++;
               }
               else
                  if (ori[i+1] == 'V')
                  {
                     sprintf(tmp2,"%d.%02d%c",infp.version1,infp.version2,infp.versionletra);
                     strcpy(tmp+j,tmp2);
                     j += strlen(tmp2);
                  }
                  else
                     if (ori[i+1] == 'I') /* ojo: default internet address */
                     {
                        tmp2[0] = 0;
#if defined(RWXARXA) || defined(RWINSFGV)	   
                        dame_ip_por_defecto(tmp2,sizeof(tmp2));
#endif
                        strcpy(tmp+j,tmp2);
                        j += strlen(tmp2);        
                     }
                     else
                        if (ori[i+1] == 'A') /* clave de activacion */
                        {
                           tmp2[0] = 0;
                           if (act_key)
                           {
                              strcpy(tmp2,act_key);
                           }
                           strcpy(tmp+j,tmp2);
                           j += strlen(tmp2);
                           moder = 1;
                        }
                        else
                           if (ori[i+1] == 'R') /* clave de accesos */
                           {
                              tmp2[0] = 0;
                              if (act_red)
                              {
                                 strcpy(tmp2,act_red);
                              }
                              strcpy(tmp+j,tmp2);
                              j += strlen(tmp2);
                              moder = 2;
                           }
                           else
                              if (ori[i+1] == 'S') /* clave de sistema */
                              {
                                 tmp2[0] = 0;
                                 if (dame_codigo_de_sistema(0))
                                 {
                                    strcpy(tmp2,dame_codigo_de_sistema(0));
                                 }	     
                                 strcpy(tmp+j,tmp2);
                                 j += strlen(tmp2);
                                 moder = 3;
                              }
                              else
                                 if (ori[i+1] == 'G') /* clave de sistema grafico*/
                                 {
                                    tmp2[0] = 0;
                                    if (dame_codigo_de_sistema(1))
                                    {
                                       strcpy(tmp2,dame_codigo_de_sistema(1));
                                    }	     
                                    strcpy(tmp+j,tmp2);
                                    j += strlen(tmp2);
                                    moder = 4;
                                 }
                                 else
                                    if (ori[i+1] == 'T') /* clave de sistema texto */
                                    {
                                       tmp2[0] = 0;
                                       if (dame_codigo_de_sistema(2))
                                       {
                                          strcpy(tmp2,dame_codigo_de_sistema(2));
                                       }	   
                                       strcpy(tmp+j,tmp2);
                                       j += strlen(tmp2);
                                       moder = 5;
                                    }
                                    else
                                       if (ori[i+1] == 's') /* clave de servidor */
                                       {
                                          tmp2[0] = 0;
                                          if (act_servidor)
                                          {
                                             strcpy(tmp2,act_servidor);
                                          }
                                          strcpy(tmp+j,tmp2);
                                          j += strlen(tmp2);
                                          moder = 6;
                                       }
                                       else
                                          if (ori[i+1] == 'P') /* clave de paquete */
                                          {       
                                             tmp2[0] = 0;
                                             if (dame_codigo_de_paquete(infp.num_interno,infp.ver_interna))
                                             {
                                                strcpy(tmp2,dame_codigo_de_paquete(infp.num_interno,infp.ver_interna));
                                             }	   
                                             strcpy(tmp+j,tmp2);
                                             j += strlen(tmp2);
                                             moder = 7;
                                          }
                                          else	
                                             if (ori[i+1] == 'c') /* Parametro de NO cliente!!! */
                                             {
                                                if (server_request[0] == '@' && server_request[1] == '(' && 
                                                   (server_request[2] == 'C' || server_request[2] == '#'))
                                                {
                                                   moder = 8;
                                                }       
                                             }
                                             else	
                                                if (ori[i+1] == 'C') /* Parametro de cliente!!! */
                                                {	   
                                                   tmp2[0] = 0;
                                                   if (server_request[0] == '@' && server_request[1] == '(' && 
                                                      (server_request[2] == 'C' || server_request[2] == '/'))
                                                   {
                                                      server_request[2] = 'C';		  
                                                      strcpy(tmp2,server_request);		  
                                                      cambia_s_req(tmp2,0);
                                                   }
                                                   else
                                                      moder = 8;
                                                   strcpy(tmp+j,tmp2);
                                                   j += strlen(tmp2);
                                                }
                                                else	
                                                   if (ori[i+1] == 'X') /* Parametro de servidor!!! */
                                                   {
                                                      tmp2[0] = 0;
                                                      if (server_request[0] == '@' && server_request[1] == '(' && 
                                                         (server_request[2] == 'S' || server_request[2] == '-'))
                                                      {
                                                         server_request[2] = 'S';		  
                                                         strcpy(tmp2,server_request);
                                                         cambia_s_req(tmp2,0);
                                                      }
                                                      strcpy(tmp+j,tmp2);
                                                      j += strlen(tmp2);
                                                   }
                                                   else
                                                      if (ori[i+1] >= '0' && ori[i+1] <= '9')
                                                      {
                                                         ti = ori[i+1] - 48;
                                                         if (memoria_instalacion[ti])
                                                         {
                                                            strcpy(tmp+j,memoria_instalacion[ti]);
                                                            j += strlen(memoria_instalacion[ti]);
                                                         }
                                                      }
                                                      i++;
    }
    else
    {
       tmp[j] = ori[i];
       j++;
    }
  }
  tmp[j] = 0;
  strcpy(ori,tmp);
  return(moder);
}


static int rellena_instalacion(bloque)
char *bloque;
{
   int i,j,punto,inipunto;
   int lacoma;
   char tmp[256];
   char tmp2[256];
   
   
   inipunto = 0;
   for (punto = 0;punto < blocksize && rellena_size;punto++,rellena_size--)
   { 
      if (bloque[punto] == 10 || rellena_size == 1L)
      {
         if (rellena_size == 1L)
         {
            j = bloque[punto+1];
            bloque[punto+1] = 0;
         }
         else
            bloque[punto] = 0;
         strcpy(tmp,rellena_resto);
         rellena_resto[0] =0;
         strcat(tmp,bloque+inipunto);
         if (rellena_size == 1L)
            bloque[punto+1] = j;
         else
            bloque[punto] = 10;
         inipunto = punto+1;
      }
      else
         continue;
      for (i = 0;tmp[i];i++)
      {
         if (tmp[i] == ';')
         {
            tmp[i] = 0;
            break;
         }
      }
      quitab(tmp);
      quitan(tmp);
      quitab(tmp);
      quitabp(tmp);
      if (tmp[0] == '[' || rellena_modo == 1 || rellena_modo == 2 || rellena_modo == 5)
         strminus(tmp);
      
      if (tmp[0])
      {
         if (!strcmp(tmp,"[fijos]"))
         { 
            rellena_modo  = 1;
            continue;
         }	  
         else
            if (!strcmp(tmp,"[variables]"))
            { 
               rellena_modo  = 2;
               continue;
            }
            else
               if (!strcmp(tmp,"[version]"))
               {
                  rellena_modo  = 3;
                  continue;
               }	  
               else
                  if (!strcmp(tmp,"[control]"))
                  { 
                     rellena_modo  = 4;
                     continue;
                  }
                  else
                     if (!strcmp(tmp,"[submenu]"))
                     { 
                        rellena_modo  = 5;
                        continue;
                     }	  
                     else
                        if (!strcmp(tmp,"[creaiconos]"))
                        { 
                           rellena_modo  = 6;	 
                           if (ninstalar_iconos)
                           {
                              strcpy(tmp,".");
                           }
                           else
                              continue;
                        }	  
                        else    
                           if (!strcmp(tmp,"[preejecuta]"))
                           { 
                              rellena_modo  = 7;
                              continue;
                           }	
                           else
                              if (!strcmp(tmp,"[postejecuta]"))
                              { 
                                 rellena_modo  = 8;
                                 continue;
                              }	
                              else
                                 if (!strcmp(tmp,"[limpia]"))
                                 { 
                                    rellena_modo  = 9;
                                    continue;
                                 }	
                                 else
                                    if (!strcmp(tmp,"[textoinicial]") && !rwi_parte)
                                    { 
                                       rellena_modo  = 10;
                                       continue;
                                    }	
                                    else
                                       if (!strcmp(tmp,"[textofinal]") && !rwi_parte)
                                       { 
                                          rellena_modo  = 11;
                                          continue;
                                       }	
                                       else
                                          if (!strcmp(tmp,"[preguntas]") && !rwi_parte)
                                          { 
                                             rellena_modo  = 12;
                                             continue;
                                          }	
                                          else
                                             if (!strcmp(tmp,"[ponstdini]") && !rwi_parte)
                                             { 
                                                rellena_modo  = 13;
                                                continue;
                                             }	
                                             else
                                                if (!strcmp(tmp,"[continuacion]") && !rwi_parte)
                                                { 
                                                   rellena_modo  = 14;
                                                   continue;
                                                }	
                                                else
                                                   if (!memcmp(tmp,"[build:",7))
                                                   {
                                                      infp.build_numero = atol(tmp+7);
                                                      rellena_modo  = 0;
                                                      continue;
                                                   }	
                                                   else
                                                      if (!memcmp(tmp,"[ayuda:",7))
                                                      { 
                                                         strcpy(tmp2,tmp+7);
                                                         quita_blancos(tmp2);
                                                         if (tmp2[0]) /* debe de haber un ']' final */
                                                            tmp2[strlen(tmp2)-1] = 0;
                                                         tmp2[8] = 0; /* limitado a 8 caracteres */
                                                         strcpy(infp.es_ayuda,tmp2);
                                                         rellena_modo  = 0;
                                                         continue;
                                                      }	
                                                      else
                                                         if (!memcmp(tmp,"[registro:",10))
                                                         {
                                                            quitab(tmp+10);
                                                            quitabp(tmp+10);	  
                                                            j = strlen(tmp) - 1;
                                                            if (j >= 0 && tmp[j] == ']') {tmp[j] = 0;quitab(tmp+10);}
                                                            if (!tmp[0])
                                                            {
                                                               continue;
                                                            }
                                                            if (!registro_instalacion)
                                                            {
                                                               registro_instalacion = (char **)malloc(sizeof(char *) * 10);
                                                               nregistro_instalacion = 0;
                                                            }
                                                            if ((nregistro_instalacion%10) == 9)
                                                            {		
                                                               registro_instalacion = (char **)realloc(registro_instalacion,sizeof(char *) * (11+nregistro_instalacion));
                                                            }
                                                            registro_instalacion[nregistro_instalacion] = malloc(strlen(tmp+10)+1);
                                                            strcpy(registro_instalacion[nregistro_instalacion],tmp+10);
                                                            nregistro_instalacion++;
                                                            continue;
                                                         }
                                                         else
                                                            if (tmp[0] == '[')
                                                            {
                                                               rellena_modo = 0;
                                                               continue;
                                                            }
                                                            
                                                            
                                                            /**********************/
                                                            if (tmp[0] == '(' && tmp[3] == ')')
                                                            {
                                                               if (tmp[1] > '0')
                                                               {
                                                                  tmp[1] -= 48;
                                                                  if (tmp[1] == 1 && QUE_SISTEMA_SOY != 11 && QUE_SISTEMA_SOY != 13)
                                                                  {
                                                                     continue;
                                                                  }
                                                                  if (tmp[1] == 2 && QUE_SISTEMA_SOY != 7)
                                                                  {
                                                                     continue;
                                                                  }
                                                                  if (tmp[1] == 3 && QUE_SISTEMA_SOY != 23)
                                                                  {
                                                                     continue;
                                                                  }
                                                                  if (tmp[1] > 3 && (QUE_SISTEMA_SOY == 11 || QUE_SISTEMA_SOY == 13 || QUE_SISTEMA_SOY == 7 || QUE_SISTEMA_SOY == 23))
                                                                  {
                                                                     continue;
                                                                  }
                                                               }
                                                               if (tmp[2] > '0')
                                                               {
                                                                  tmp[2] -= 48;	  
                                                                  if (tmp[2] != os_subtipo)
                                                                  {
                                                                     continue;
                                                                  }
                                                               }
                                                               strcpy(tmp,tmp+4);
                                                            }	  
                                                            
                                                            if (rellena_modo  == 1)
                                                            {
                                                               if (!instalar_fijos)
                                                               {
                                                                  instalar_fijos = (char **)malloc(sizeof(char *) * 100);
                                                                  ninstalar_fijos = 0;
                                                               }
                                                               else
                                                                  if ((ninstalar_fijos % 100) == 99)
                                                                  {
                                                                     instalar_fijos = (char **)realloc(instalar_fijos,sizeof(char *) * (101 + ninstalar_fijos));
                                                                  }
                                                                  instalar_fijos[ninstalar_fijos] = malloc(strlen(tmp)+1);
                                                                  strcpy(instalar_fijos[ninstalar_fijos],tmp);
                                                                  ninstalar_fijos++;
                                                            }
                                                            else
                                                               if (rellena_modo  == 2)
                                                               {
                                                                  if (!instalar_varia)
                                                                  {
                                                                     instalar_varia = (char **)malloc(sizeof(char *) * 100);
                                                                     ninstalar_varia = 0;
                                                                  }
                                                                  else
                                                                     if ((ninstalar_varia % 100) == 99)
                                                                     {
                                                                        instalar_varia = (char **)realloc(instalar_varia,sizeof(char *) * (101 + ninstalar_varia));
                                                                     }
                                                                     instalar_varia[ninstalar_varia] = malloc(strlen(tmp)+1);
                                                                     strcpy(instalar_varia[ninstalar_varia],tmp);
                                                                     ninstalar_varia++;
                                                               }
                                                               else
                                                                  if (rellena_modo  == 3)
                                                                  {
                                                                     comas_linea(tmp,0,1,&lacoma);
                                                                     quita_blancos(tmp);
                                                                     infp.tipo         = tmp[0];
                                                                     j = comas_linea(tmp,1,1,&lacoma);
                                                                     quita_blancos(tmp+j);
                                                                     strncpy(infp.sistema,tmp+j,15);
                                                                     infp.version1     = atoi(tmp+comas_linea(tmp,1,1,&lacoma));
                                                                     infp.version2     = atoi(tmp+comas_linea(tmp,1,1,&lacoma));
                                                                     j = comas_linea(tmp,1,1,&lacoma);
                                                                     quita_blancos(tmp+j);	
                                                                     if (!tmp[j])
                                                                        infp.versionletra = ' ';
                                                                     else
                                                                        infp.versionletra = tmp[j];
                                                                     j = comas_linea(tmp,1,1,&lacoma);
                                                                     quita_blancos(tmp+j);	
                                                                     strncpy(infp.fecha,tmp+j,20);
                                                                     j = comas_linea(tmp,1,1,&lacoma);
                                                                     quitabp(tmp+j);
                                                                     quitab(tmp+j);
                                                                     strncpy(infp.descrip,tmp+j,30);      
                                                                  }
                                                                  else
                                                                     if (rellena_modo == 4)
                                                                     {
                                                                        comas_linea(tmp,0,1,&lacoma);
                                                                        infp.ver_interna = atoi(tmp);
                                                                        infp.num_interno = atoi(tmp+comas_linea(tmp,1,1,&lacoma));
                                                                        infp.rdepende1 = atoi(tmp+comas_linea(tmp,1,1,&lacoma));
                                                                        infp.rdepende2 = atoi(tmp+comas_linea(tmp,1,1,&lacoma));
                                                                        j = comas_linea(tmp,1,1,&lacoma);
                                                                        quita_blancos(tmp+j);
                                                                        infp.rdependeletra = tmp[j];	  
                                                                        infp.rdependemodo = atoi(tmp+comas_linea(tmp,1,1,&lacoma));
                                                                        infp.pdepende1 = atoi(tmp+comas_linea(tmp,1,1,&lacoma));
                                                                        infp.pdepende2 = atoi(tmp+comas_linea(tmp,1,1,&lacoma));
                                                                        j = comas_linea(tmp,1,1,&lacoma);
                                                                        quita_blancos(tmp+j);
                                                                        infp.pdependeletra = tmp[j];	  
                                                                        infp.pdependemodo = atoi(tmp+comas_linea(tmp,1,1,&lacoma));
                                                                        infp.dependesubsistema = atoi(tmp+comas_linea(tmp,1,1,&lacoma));
                                                                        j = comas_linea(tmp,1,1,&lacoma);
                                                                        quita_blancos(tmp+j);
                                                                        strncpy(infp.origen,tmp+j,20);
                                                                     }
                                                                     else
                                                                        if (rellena_modo == 5)
                                                                        {
                                                                           quita_blancos(tmp);	  
                                                                           if (!instalar_submenu)
                                                                           {
                                                                              instalar_submenu = (char **)malloc(sizeof(char *) * 10);
                                                                              ninstalar_submenu = 0;
                                                                           }
                                                                           if ((ninstalar_submenu%10) == 9)
                                                                           {		
                                                                              instalar_submenu = (char **)realloc(instalar_submenu,sizeof(char *) * (11+ninstalar_submenu));
                                                                           }
                                                                           instalar_submenu[ninstalar_submenu] = malloc(strlen(tmp)+1);
                                                                           strcpy(instalar_submenu[ninstalar_submenu],tmp);
                                                                           ninstalar_submenu++;
                                                                        }
                                                                        else
                                                                           if (rellena_modo == 6)
                                                                           {
                                                                              if (!instalar_iconos)
                                                                              {
                                                                                 instalar_iconos = (char **)malloc(sizeof(char *) * 10);
                                                                                 ninstalar_iconos = 0;
                                                                              }
                                                                              if ((ninstalar_iconos%10) == 9)
                                                                              {		
                                                                                 instalar_iconos = (char **)realloc(instalar_iconos,sizeof(char *) * (11+ninstalar_iconos));
                                                                              }
                                                                              instalar_iconos[ninstalar_iconos] = malloc(strlen(tmp)+1);
                                                                              strcpy(instalar_iconos[ninstalar_iconos],tmp);
                                                                              ninstalar_iconos++;
                                                                           }
                                                                           else
                                                                              if (rellena_modo == 7)
                                                                              {
                                                                                 if (!es_sacarwi)
                                                                                 {
                                                                                    cambia_por_destino(tmp,otro_destino);
                                                                                    fin_sig();
                                                                                    agi_system(tmp);
                                                                                    ini_sig();
                                                                                 }
                                                                                 /**
                                                                                 if (!instalar_presystem)
                                                                                 {
                                                                                 instalar_presystem = (char **)malloc(sizeof(char *) * 10);
                                                                                 ninstalar_presystem = 0;
                                                                                 }
                                                                                 if ((ninstalar_presystem%10) == 9)
                                                                                 {		
                                                                                 instalar_presystem = (char **)realloc(instalar_presystem,sizeof(char *) * (11+ninstalar_presystem));
                                                                                 }
                                                                                 instalar_presystem[ninstalar_presystem] = malloc(strlen(tmp)+1);
                                                                                 strcpy(instalar_presystem[ninstalar_presystem],tmp);
                                                                                 ninstalar_presystem++;	  
                                                                                 */
                                                                              }
                                                                              else
                                                                                 if (rellena_modo == 8)
                                                                                 {
                                                                                    if (!instalar_postsystem)
                                                                                    {
                                                                                       instalar_postsystem = (char **)malloc(sizeof(char *) * 10);
                                                                                       ninstalar_postsystem = 0;
                                                                                    }
                                                                                    if ((ninstalar_postsystem%10) == 9)
                                                                                    {		
                                                                                       instalar_postsystem = (char **)realloc(instalar_postsystem,sizeof(char *) * (11+ninstalar_postsystem));
                                                                                    }
                                                                                    instalar_postsystem[ninstalar_postsystem] = malloc(strlen(tmp)+1);
                                                                                    strcpy(instalar_postsystem[ninstalar_postsystem],tmp);
                                                                                    ninstalar_postsystem++;
                                                                                 }
                                                                                 else
                                                                                    if (rellena_modo == 9)
                                                                                    {
                                                                                       if (!instalar_limpia)
                                                                                       {
                                                                                          instalar_limpia = (char **)malloc(sizeof(char *) * 10);
                                                                                          ninstalar_limpia = 0;
                                                                                       }
                                                                                       if ((ninstalar_limpia%10) == 9)
                                                                                       {		
                                                                                          instalar_limpia = (char **)realloc(instalar_limpia,sizeof(char *) * (11+ninstalar_limpia));
                                                                                       }
                                                                                       instalar_limpia[ninstalar_limpia] = malloc(strlen(tmp)+1);
                                                                                       strcpy(instalar_limpia[ninstalar_limpia],tmp);
                                                                                       ninstalar_limpia++;
                                                                                    }
                                                                                    else
                                                                                       if (rellena_modo == 10)
                                                                                       {
                                                                                          if (!textoi_instalacion)
                                                                                          {
                                                                                             textoi_instalacion = (char **)malloc(sizeof(char *) * 10);
                                                                                             ntextoi_instalacion = 0;
                                                                                          }
                                                                                          if ((ntextoi_instalacion%10) == 9)
                                                                                          {		
                                                                                             textoi_instalacion = (char **)realloc(textoi_instalacion,sizeof(char *) * (11+ntextoi_instalacion));
                                                                                          }
                                                                                          textoi_instalacion[ntextoi_instalacion] = malloc(strlen(tmp)+1);
                                                                                          strcpy(textoi_instalacion[ntextoi_instalacion],tmp);
                                                                                          ntextoi_instalacion++;
                                                                                       }
                                                                                       else
                                                                                          if (rellena_modo == 11)
                                                                                          {
                                                                                             if (!textof_instalacion)
                                                                                             {
                                                                                                textof_instalacion = (char **)malloc(sizeof(char *) * 10);
                                                                                                ntextof_instalacion = 0;
                                                                                             }
                                                                                             if ((ntextof_instalacion%10) == 9)
                                                                                             {		
                                                                                                textof_instalacion = (char **)realloc(textof_instalacion,sizeof(char *) * (11+ntextof_instalacion));
                                                                                             }
                                                                                             textof_instalacion[ntextof_instalacion] = malloc(strlen(tmp)+1);
                                                                                             strcpy(textof_instalacion[ntextof_instalacion],tmp);
                                                                                             ntextof_instalacion++;
                                                                                          }
                                                                                          else
                                                                                             if (rellena_modo == 12)
                                                                                             {
                                                                                                if (tmp[0] >= '0' && tmp[0] <= '9' && (tmp[1] == '!' || tmp[1] == '?') && strlen(tmp) > 2)
                                                                                                {	    
                                                                                                   if (!variables_instalacion)
                                                                                                   {
                                                                                                      variables_instalacion = (char **)malloc(sizeof(char *) * 10);
                                                                                                      nvariables_instalacion = 0;
                                                                                                   }
                                                                                                   if ((nvariables_instalacion%10) == 9)
                                                                                                   {
                                                                                                      variables_instalacion = (char **)realloc(variables_instalacion,sizeof(char *) * (11+nvariables_instalacion));
                                                                                                   }
                                                                                                   variables_instalacion[nvariables_instalacion] = malloc(strlen(tmp)+1);
                                                                                                   strcpy(variables_instalacion[nvariables_instalacion],tmp);
                                                                                                   nvariables_instalacion++;
                                                                                                }
                                                                                             }
                                                                                             else
                                                                                                if (rellena_modo == 13)
                                                                                                {
                                                                                                   if (!stdini_instalacion)
                                                                                                   {
                                                                                                      stdini_instalacion = (char **)malloc(sizeof(char *) * 10);
                                                                                                      nstdini_instalacion = 0;
                                                                                                   }
                                                                                                   if ((nstdini_instalacion%10) == 9)
                                                                                                   {		
                                                                                                      stdini_instalacion = (char **)realloc(stdini_instalacion,sizeof(char *) * (11+nstdini_instalacion));
                                                                                                   }
                                                                                                   stdini_instalacion[nstdini_instalacion] = malloc(strlen(tmp)+1);
                                                                                                   strcpy(stdini_instalacion[nstdini_instalacion],tmp);
                                                                                                   nstdini_instalacion++;
                                                                                                }
                                                                                                else
                                                                                                   if (rellena_modo == 14)
                                                                                                   {
                                                                                                      if (posible_ins_continuacion && strlen(tmp) < 29 && hay_ins_continuacion < 10)
                                                                                                      {
                                                                                                         strcpy(ins_continuacion[hay_ins_continuacion],tmp);
                                                                                                         hay_ins_continuacion++;
                                                                                                      }
                                                                                                   }
    }
  }
  if (rellena_size > 0L && inipunto < blocksize)
  {
     memcpy(tmp,bloque+inipunto,blocksize-inipunto);
     tmp[blocksize-inipunto] = 0;
     strcpy(rellena_resto,tmp);
  }
  return(0);
}


static int control_key(char *key,int tipo)
{
   unsigned short potencia;
   int i,d,len;
   char buf[257];
   
   strcpy(buf,key);
   switch(tipo)
   {   
   case 1:
   case 6:
      len = 12;
      break;
   case 2:
      len = 9;
      break;
   case 3:
   case 4:
   case 5:
      len = 5;
      break;
   case 7:
      len = 6;
      break;
   default:
      len = 0;
      break;
   }
   if (de_new_code(buf,99,len) < 0)
   {
      return(-1);
   }
   
   return(0);
}


int comprueba_ins_sistema(char *sistema,int dependesubsistema)
{
   if (!strcmp(sistema,"X"))        return(0);
   if (strcmp(NAME_SYSTEM,sistema)) return(-1);
   if (!dependesubsistema)          return(0);
   if (dependesubsistema == os_subtipo) return(0);
   return(-1);
}

static int pregunta_instalacion()
{
   int i,np,pp,m,j,inip,pagina,n,qj,ppi;
   int coma,qc;
   char minimo[81];
   char maximo[81];
   char limpia[81];
   char qo[257];
   int numero,modo;
   char tmp[512];
   
   if (pregunta_insta && instalacion_silenciosa < 2)
   {
      sprintf(tmp,"Instalando %s",infp.descrip);
      insta_mensaje(400,tmp);
   }
   
   /* verificar sistema operatico y si no coincide salir y darla por buena !!! */
   if (comprueba_ins_sistema(infp.sistema,infp.dependesubsistema))	
   {
      instalacion_ok = 3;
      return(-1);
   }
   
   if (puede_grabar != 1 && (infp.tipo == 'C' || infp.tipo == 'S'))
   {
      instalacion_ok = 2;
      return(-1);
   }
   
   
   if (infp.rdependemodo)
   {
      if (infp.rdepende1 > rtme_version1 || 
         (infp.rdepende1 == rtme_version1 && infp.rdepende2 > rtme_version2) ||
         (infp.rdepende1 == rtme_version1 && infp.rdepende2 == rtme_version2 && infp.rdependeletra > rtme_versionletra))
      {	
         if (!instalacion_silenciosa)
         {
            sprintf(tmp,"Esta Instalacion requiere Diagram %d.%02d%c o superior",infp.rdepende1,infp.rdepende2,infp.rdependeletra);
            insta_aviso(0,tmp);
            if (infp.rdependemodo == 1)
            {
               if (insta_confirmar(2440,"Desea Efectuar la Instalacion (S/N)? ","S"))
               {
                  return(-1);
               }		 
            }
            else
            {
               return(-1);
            }
         }
         else
         {
            if (infp.rdependemodo != 1 || (infp.tipo != 'C' && infp.tipo != 'S'))
            {	     
               instalacion_ok = 2;
               return(-1);
            }
         }      
      }
   }
   
   
   
   if (infp.pdependemodo)
   {
      if (infp.pdepende1 < p_version1 || 
         (infp.pdepende1 == p_version1 && infp.pdepende2 < p_version2) ||
         (infp.pdepende1 == p_version1 && infp.pdepende2 == p_version2 && infp.pdependeletra < p_versionletra))
      {	
         if (!instalacion_silenciosa)
         {
            sprintf(tmp,"Esta Instalacion requiere Version Previa %d.%02d%c o superior",infp.pdepende1,infp.pdepende2,infp.pdependeletra);
            insta_aviso(0,tmp);
            if (infp.pdependemodo == 1)
            {
               if (insta_confirmar(2440,"Desea Efectuar la Instalacion (S/N)? ","S"))
               {
                  return(-1);
               }		 
            }
            else
            {
               return(-1);
            }
         }
         else
         {
            if (infp.pdependemodo != 1 || (infp.tipo != 'C' && infp.tipo != 'S'))
            {	     
               instalacion_ok = 2;
               return(-1);
            }
         }      
      }
   }
   
   if (!pregunta_insta)
   { 
#ifdef FGVDOS
      if (dlg)
      {	 
         MSG msg;
         for (i = 0;i < ntextoi_instalacion;i++)
         {
            strcpy(tmp,textoi_instalacion[i]);
            cambia_por_destino(tmp,otro_destino);
            CreateWindowEx(  /*WS_EX_CLIENTEDGE |*/ WS_EX_CONTROLPARENT,"EDIT",tmp,WS_VISIBLE | WS_CHILD | ES_LEFT | ES_READONLY,17,20+i*21,492,20,dlg,1020+i,hInstacia,NULL);	
         }
         //#ifndef FGVWIN
         while (PeekMessage((LPMSG)&msg,dlg,0,0,PM_NOREMOVE) && GetMessage((LPMSG)&msg,dlg,0,0))
         {
            if (!IsDialogMessage(dlg,&msg))
            {
               TranslateMessage((LPMSG)&msg);
               DispatchMessage((LPMSG)&msg);
            }	     
         }         
         //#endif	  
      }
#endif
      return(0);
   }
   
   memset(limpia,' ',68);
   limpia[68] = 0;
   
   for (i = 0;i < 10;i++)
   {
      if (memoria_instalacion[i])
      {
         free(memoria_instalacion[i]);
         memoria_instalacion[i] = NULL;
      }
   }
   
   if (instalacion_silenciosa < 2)
   {
      if (instalacion_en_grafico)
      {
#ifdef FGVDOS
#ifdef FGVWIN
         wPadre = principal_w;
#endif
         crea_i_dialogo();
#endif
      }
      else
      {
#ifdef FGVWIN
         Nor();
         zonablan(705,1675);
         cuadro(705,1675);
#else
#ifndef RWSERVER
         Color(7,1);
#endif
         Nor();  
         zonablan(705,1675);
         /* cuadro(705,1675); */
#ifndef RWSERVER
         Color(7,1);
#endif
         Nor();
         tmp[1] = 0;
         tmp[0] = 218;
         pprint(705,tmp);
         cuadro(805,1505);
         tmp[1] = 0;
         tmp[0] = 192;  
         pprint(1605,tmp);
         cuadro(706,774);
         tmp[1] = 0;
         tmp[0] = 191;
         pprint(775,tmp);  
#ifndef RWSERVER
         Color(0,1);
#endif
         Nor();
         cuadro(1606,1674);
         tmp[1] = 0;
         tmp[0] = 217;  
         pprint(1675,tmp);
         cuadro(1575,875);
         Nor();
#endif
      }
      
      for (i = 0;i < ntextoi_instalacion;i++)
      {
         strcpy(tmp,textoi_instalacion[i]);
         cambia_por_destino(tmp,otro_destino);
         if (instalacion_en_grafico) 
         {   
#ifdef FGVDOS
            CreateWindowEx(  /*WS_EX_CLIENTEDGE |*/ WS_EX_CONTROLPARENT,"EDIT",tmp,WS_VISIBLE | WS_CHILD | ES_LEFT | ES_READONLY,17,20+i*21,492,20,dlg,1020+i,hInstacia,NULL);
#endif
         }
         else
         {
            pprint(810+i*100,tmp);
         }
         if (i >= 6) {i++;break;}
      }
      
   }
   
   inip = i;
   np = 7 - i;
   if (np > nvariables_instalacion) 
   {
      inip++; /* dejar una linea en blanco */
      np = nvariables_instalacion;
   }
   pp = 0;
   pagina = 0;
   
   if (instalacion_en_grafico)    
   {   
#ifdef FGVDOS;
      MSG msg;
      HWND pw = NULL,mw;
      
      for (pp  = 0;pp < nvariables_instalacion;pp++)
      {
         numero = variables_instalacion[pp][0] - 48;
         modo   = variables_instalacion[pp][1] - 48; /* el modo vamos ignorarlo de momento */
         strcpy(tmp,variables_instalacion[pp]+2);
         comas_linea(tmp,0,1,&coma);
         qc     = atoi(tmp+comas_linea(tmp,1,1,&coma));
         strcpy(minimo,tmp+comas_linea(tmp,1,1,&coma));
         quitab(minimo);
         strcpy(maximo,tmp+comas_linea(tmp,1,1,&coma));
         quitab(maximo);
         j      = comas_linea(tmp,1,1,&coma);
         m      = cambia_por_destino(tmp+j,otro_destino);
         cambia_por_destino(tmp,otro_destino);
         if (qc <= 0 && !strlen(minimo) && !strlen(maximo))
         {
            qc = 1;
         }  	  
         CreateWindowEx(WS_EX_CONTROLPARENT,"EDIT",tmp,WS_VISIBLE | WS_CHILD | ES_LEFT | ES_READONLY,17,30+(inip+pp)*22,210,20,dlg,1020+inip+pp*2,hInstacia,NULL);
         if (memoria_instalacion[numero])
         {
            strcpy(tmp+j,memoria_instalacion[numero]);
            free(memoria_instalacion[numero]);
            memoria_instalacion[numero] = NULL;
         }
         qj = 0;
         strcpy(qo,tmp+j);	  
         if (instalacion_silenciosa)
#ifdef FGVDOS
            CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT",qo,WS_VISIBLE | WS_CHILD | ES_LEFT | ES_READONLY,245,30+(inip+pp)*22,240,20,dlg,1020+1+inip+pp*2,hInstacia,NULL);
#endif
         else
         {
            mw = CreateWindowEx(WS_EX_CLIENTEDGE,"EDIT",qo,WS_GROUP | WS_VISIBLE | WS_TABSTOP | WS_CHILD | ES_LEFT,245,30+(inip+pp)*22,240,20,dlg,1020+inip+1+pp*2,hInstacia,NULL);
            if (!pw)
               pw = mw;
            //GetwindowLong
            //SetWindowLong(mw, GWL_WNDPROC,(DWORD)CtrlWndProc);
         }
      }
      
      if (pw) SetFocus(pw);
      
      if (instalacion_silenciosa)
      {
         empieza_dlg = 1;	  
         //#ifndef FGVWIN
         while (PeekMessage((LPMSG)&msg,dlg,0,0,PM_NOREMOVE) && GetMessage((LPMSG)&msg,dlg,0,0))
         {
            if (!IsDialogMessage(dlg,&msg))
            {
               TranslateMessage((LPMSG)&msg);
               DispatchMessage((LPMSG)&msg);
            }	     
         }         
         //#endif	  
      }
      else
      {
         empieza_dlg = 0;
         EnableWindow(GetDlgItem(dlg,IDOK),TRUE);
         EnableWindow(GetDlgItem(dlg,IDCANCEL),TRUE);
         ShowWindow(GetDlgItem(dlg,IDOK),SW_SHOW);
         ShowWindow(GetDlgItem(dlg,IDCANCEL),SW_SHOW);
      }
      while (empieza_dlg == 0 && dlg && GetMessage((LPMSG)&msg,dlg,0,0))
      {
         if (!IsDialogMessage(dlg,&msg))
         {
            TranslateMessage((LPMSG)&msg);
            DispatchMessage((LPMSG)&msg);
         }	     
      }   
      EnableWindow(GetDlgItem(dlg,IDOK),FALSE);
      EnableWindow(GetDlgItem(dlg,IDCANCEL),FALSE);
      
      if (!dlg || empieza_dlg == 2)
      {     
         if (dlg)
         {
            DestroyWindow(dlg);
            dlg = NULL;
         }
         if (wPadre)
         {
            SetActiveWindow(wPadre);
            SetFocus(wPadre);
         }
         return(-1);
      }
      
      
      for (pp  = 0;pp < nvariables_instalacion;pp++)
      {
         numero = variables_instalacion[pp][0] - 48;
         modo   = variables_instalacion[pp][1] - 48; /* el modo vamos ignorarlo de momento */
         strcpy(tmp,variables_instalacion[pp]+2);
         comas_linea(tmp,0,1,&coma);
         qc     = atoi(tmp+comas_linea(tmp,1,1,&coma));
         strcpy(minimo,tmp+comas_linea(tmp,1,1,&coma));
         quitab(minimo);
         strcpy(maximo,tmp+comas_linea(tmp,1,1,&coma));
         quitab(maximo);
         j      = comas_linea(tmp,1,1,&coma);
         m      = cambia_por_destino(tmp+j,otro_destino);
         cambia_por_destino(tmp,otro_destino);
         if (qc <= 0 && !strlen(minimo) && !strlen(maximo))
         {
            qc = 1;
         }
         qo[0] = 0;
         GetDlgItemText(dlg,1020+inip+1+pp*2,qo,sizeof(qo)-1);
         if (m)
            quita_blancos(qo);
         else
            quitab(qo);
         if (m && strlen(qo) && qc && !instalacion_silenciosa)
         {
            if (control_key(qo,m)) /* verificacion somera de la clave */
            { 
               MessageBox(dlg,"A ENTRADO UNA CLAVE INCORRECTA","ATENCION",MB_OK | MB_ICONHAND);
               return(-1);
            }
         }
         if (memoria_instalacion[numero])
            free(memoria_instalacion[numero]);
         memoria_instalacion[numero] = malloc(strlen(qo)+1);
         strcpy(memoria_instalacion[numero],qo);
      }
      
#endif
   }
   else
   {
      
      if (instalacion_silenciosa < 2)
      {
         for (i = 0;i < np;i++)
         {
            pprint(906+(inip+i)*100,limpia);  
         } 
      }
      i = 0;
      while (pp < nvariables_instalacion)
      {
         if (pagina)
         {
            pagina = 0;	
            if (instalacion_silenciosa < 2)
            {
               v_scroll(-1,-1,906+inip*100,1574,ppi);	
            }
         }
         numero = variables_instalacion[pp][0] - 48;
         modo   = variables_instalacion[pp][1] - 48; /* el modo vamos ignorarlo de momento */
         strcpy(tmp,variables_instalacion[pp]+2);
         comas_linea(tmp,0,1,&coma);
         qc     = atoi(tmp+comas_linea(tmp,1,1,&coma));
         strcpy(minimo,tmp+comas_linea(tmp,1,1,&coma));
         quitab(minimo);
         strcpy(maximo,tmp+comas_linea(tmp,1,1,&coma));
         quitab(maximo);
         j      = comas_linea(tmp,1,1,&coma);
         m      = cambia_por_destino(tmp+j,otro_destino);
         cambia_por_destino(tmp,otro_destino);
         if (qc <= 0 && !strlen(minimo) && !strlen(maximo))
         {
            qc = 1;
         }  
         pprint(910+(inip+i)*100,tmp);
         if (memoria_instalacion[numero])
         {
            strcpy(tmp+j,memoria_instalacion[numero]);
            free(memoria_instalacion[numero]);
            memoria_instalacion[numero] = NULL;
         }
         do {
            if (instalacion_silenciosa)
            {
               qj = 0;
               strcpy(qo,tmp+j);
               if (instalacion_silenciosa < 2)
                  pprint(910+(inip+i)*100+strlen(tmp)+1,qo);
            }
            else
            {
               qj = entra(910+(inip+i)*100+strlen(tmp)+1,qc,minimo,maximo,tmp+j,1,qo);
            }
            if (qj == 1 || qj == 7)
            {
               if (insta_confirmar(2440,"Desea Efectuar la Instalacion (S/N)? ","S"))
               {
                  return(-1);
               }
               pprint(2440,"                                 ");
               continue;
            }
            if (qj != 0 && qj != 2 && qj != 3)
            {
               continue;
            }     
            if (m)
               quita_blancos(qo);
            else
               quitab(qo);
            if (m && strlen(qo) && qc && !instalacion_silenciosa)
            {
               if (control_key(qo,m)) /* verificacion somera de la clave */
               { 
                  insta_aviso(0,"CLAVE INCORRECTA");
                  continue;
               }
            }
            break;
         } while(-1);
         memoria_instalacion[numero] = malloc(strlen(qo)+1);
         strcpy(memoria_instalacion[numero],qo);
         
         if (qj == 2)
         {
            if (pp)
            {
               if (i)
               {
                  i--;
               }
               else
               {
                  pagina = 1;
                  ppi = -1;
               }
               pp--;
            }
            continue;
         }
         i++;
         if (i >= np)
         {
            pagina = 1;
            i = np - 1;
            ppi = 1;
         }
         pp++;
      }
      if (!instalacion_silenciosa && insta_confirmar(2440,"Desea Efectuar la Instalacion (S/N)? "," "))
      {
         return(-1);
      }
 } /* instalacion en grafico */
 
 return(0);
}

static int name_match (file_name)
char *file_name;
{
   int i;
   int r = 0;
   char filtro[81];
   
   if (es_instalacion)
   {   
      for (i = 0;i < ninstalar_fijos;i++)
      {
         strcpy(filtro,instalar_fijos[i]);
			if (coincide_(file_name,filtro)) 
			{
				if (coincide_(file_name,"ifonts/*")) 
					r = 4;
				else
					r = 1;
				break;
			}

      }
      if (!r)
      {
         for (i = 0;i < ninstalar_varia;i++)
         {
            strcpy(filtro,instalar_varia[i]);
            if (coincide_(file_name,filtro)) 
            {
               r = 3;
               break;
            }
         }
      }
      return(r);
   }
   
   if (nname_matchs < 1) return(1);
   
   for (i = 0;i < nname_matchs;i++)
   {
      strcpy(filtro,name_matchs[i]);
      quitab(filtro);
      if (coincide_(file_name,filtro)) {r = 1;break;}
   }
   
   
   if (r == 1 && nname_matchs == 1 && !strcmp(file_name,name_matchs[0]))
      r = 2; /* encontrado y al ser unico ... acabar */
   
   return(r);
}





void
read_and (do_something)
void (*do_something) ();
{
   int status = 3;		/* Initial status at start of archive */
   int prev_status;
   extern time_t new_time;
   char save_linkflag;
   
   
   if (!gzip_inicio)
			{
      open_archive (1);		/* Open for reading */
      if (gzip_a_salir)
      {
         close_archive();
         return;
      }
      if (estoy_en_gzip)
      {
         gzip_inicio = 1;
      }
			}
   
   
   for (;;)
			{
      prev_status = status;
      status = read_header ();
      if (gzip_a_salir)
      {
         return;
      }
      if (en_minusculas)
         strminus(current_file_name);	  
      /* check if the namelist got emptied during the course of reading */
      /* the tape, if so stop by setting status to EOF */
      switch (status)
      {
         
      case 1:		/* Valid header */
         /* We should decode next field (mode) first... */
         /* Ensure incoming names are null terminated. */
         if (es_instalacion == 1)
         {
            match = strlen(current_file_name) - 4;
            if (match < 1 || (strcmp(current_file_name+match,".rwi") && strcmp(current_file_name+match,".dsi")))
            {
               match = 2; /* abortada la instalacion por mala configuracion */
               break;
            }
            if (!strcmp(current_file_name+match,".dsi"))
            {
               instalacion_9 = 1;
            }
            else
            {
               instalacion_9 = 0;
            }
            rellena_resto[0] = 0;
            rellena_modo = 0;
            rellena_size = hstat.st_size;
            userec (head);
            skip_file ((long) hstat.st_size);
            if (gzip_a_salir)
            {
               return;
            }
            es_instalacion++;
            if (!es_sacarwi && pregunta_instalacion())
            {
               match = 2;
               break;
            }		  
            if (es_sacarwi)
            {
               instalacion_ok = 2;
               match = 2;
               break;
            }
            instalacion_ok = 1;		  
            continue;
         }		
         
         if (!(match = name_match (current_file_name))
            || (f_new_files && hstat.st_mtime < new_time)
            || (f_exclude && check_exclude (current_file_name)))
         {     
            /* Skip past it in the archive */
            save_linkflag = head->header.linkflag;
            userec (head);
            /* Skip to the next header on the archive */
            if (save_linkflag != LF_DIR)
               skip_file ((long) hstat.st_size);
            if (gzip_a_salir)
            {
               return;
            }
            continue;
         }	  
         (*do_something) ();	  
         if (gzip_a_salir)
         {		
            return;
         }
         if (match == 2) break;
         if (match == 3) 
         {
            /* caso especial instalacion */
            instala_pon_en_su_sitio(current_file_name);
         }
		if (match == 4)
		{ /* Instalar fonts */
			instala_font_(current_file_name);
		}
         continue;
         
         /*
         * If the previous header was good, tell them
         * that we are skipping bad ones.
         */
      case 0:		/* Invalid header */
         userec (head);
         switch (prev_status)
         {
         case 3:		/* Error on first record */
            /*msg ("Hmm, this doesn't look like a tar archive.");*/
            /* FALL THRU */
         case 2:		/* Error after record of zeroes */
         case 1:		/* Error after header rec */
            /*msg ("Skipping to next file header...");*/
         case 0:		/* Error after error */
            break;
         }
         continue;
         
         case 2:		/* Record of zeroes */
            userec (head);
            status = prev_status;	/* If error after 0's */
            /* FALL THRU */
         case EOF:		/* End of archive */
            break;
      }
      break;
    };
    
    /* restore_saved_dir_info (); */
    close_archive ();
    /* names_notfound ();		/* Print names not found */
}


void
extr_init ()
{
   int ourmask;
   
   now = time ((time_t *) 0);
   if (geteuid () == 0)
      we_are_root = 1;
   
      /*
      * We need to know our umask.  But if f_use_protection is set,
      * leave our kernel umask at 0, and our "notumask" at ~0.
   */
   ourmask = umask (0);		/* Read it */
}

/*
* Extract a file from the archive.
*/
void extract_archive ()
{
   register char *data;
   int fd, check, namelen, written, openflag;
   long size;
   struct utimbuf acc_upd_times;
   register int skipcrud;
   register int i;
   /*	int sparse_ind = 0;*/
   union record *exhdr;  
   char tmp_c[260];
   int reintento = 0;
   /*	int end_nulls; */
   
   
   saverec (&head);		/* Make sure it sticks around */
   userec (head);		/* And go past it in the archive */
   decode_header (head, &hstat, &head_standard, 1);	/* Snarf fields */
                                                      /*
                                                      * Check for fully specified pathnames and other atrocities.
                                                      *
                                                      * Note, we can't just make a pointer to the new file name,
                                                      * since saverec() might move the header and adjust "head".
                                                      * We have to start from "head" every time we want to touch
                                                      * the header record.
   */
   skipcrud = 0;
   while (!f_absolute_paths
      && '/' == current_file_name[skipcrud])
   {
      static int warned_once = 0;
      
      skipcrud++;		/* Force relative path */
      if (!warned_once++)
      {
         /*msg ("Removing leading / from absolute path names in the archive.");*/
      }
   }
   
   if (otro_destino)
   {
      strcpy(tmp_c,otro_destino);
      strcat(tmp_c,current_file_name+skipcrud);
      strcpy(current_file_name,tmp_c);
      skipcrud = 0;
   }
   
   if (es_instalacion && !marcada_la_ayuda)
   {
      int p1 = strbusca(current_file_name,"/ayuda/") ;
      int p2 = strbusca(current_file_name,".txt");
      if (p1 >= 0 && p2 >= 0 && p2 > p1)   
      {
         char tmp[512];
         FILE *f;
         
         marcada_la_ayuda = 1;
         strcpy(tmp,current_file_name);
         *(quita_path(tmp)) = 0;
         strcat(tmp,"flag.upd");
         f = fopen(tmp,"w");
         if (f)
         {
            fprintf(f,"SE DEBE ACTUALIZAR EL HLP!\n");
            fclose(f);
         }
      }
   }
   
   
   switch (head->header.linkflag)
   {
      
   default:
   /*msg ("Unknown file type '%c' for %s, extracted as normal file",
      head->header.linkflag, skipcrud + current_file_name);*/
      /* FALL THRU */
      /*
      * JK - What we want to do if the file is sparse is loop through
      * the array of sparse structures in the header and read in
      * and translate the character strings representing  1) the offset
      * at which to write and 2) how many bytes to write into numbers,
      * which we store into the scratch array, "sparsearray".  This
      * array makes our life easier the same way it did in creating
      * the tar file that had to deal with a sparse file.
      *
      * After we read in the first five (at most) sparse structures,
      * we check to see if the file has an extended header, i.e.,
      * if more sparse structures are needed to describe the contents
      * of the new file.  If so, we read in the extended headers
      * and continue to store their contents into the sparsearray.
      */
      /* FALL THRU */
   case LF_OLDNORMAL:
   case LF_NORMAL:
   case LF_CONTIG:
   /*
   * Appears to be a file.
   * See if it's really a directory.
      */
      namelen = strlen (skipcrud + current_file_name) - 1;
      if (current_file_name[skipcrud + namelen] == '/')
         goto really_dir;
      
      /* FIXME, deal with protection issues */
again_file:
      openflag = (f_keep ?
         O_BINARY | O_NDELAY | O_WRONLY | O_CREAT | O_EXCL :
      O_BINARY | O_NDELAY | O_WRONLY | O_CREAT | O_TRUNC);
      /* | ((head->header.linkflag == LF_SPARSE) ? 0 : O_APPEND); */
      /*
      * JK - The last | is a kludge to solve the problem
      * the O_APPEND flag  causes with files we are
      * trying to make sparse:  when a file is opened
      * with O_APPEND, it writes  to the last place
      * that something was written, thereby ignoring
      * any lseeks that we have done.  We add this
      * extra condition to make it able to lseek when
      * a file is sparse, i.e., we don't open the new
      * file with this flag.  (Grump -- this bug caused
      * me to waste a good deal of time, I might add)
      */
      
      if (f_unlink && !f_keep) {
         if (unlink(skipcrud + current_file_name) == -1)
            if (errno != ENOENT)
            /*msg_perror ("Could not unlink %s",
            skipcrud + current_file_name)*/;
      }
#ifdef O_CTG
      /*
      * Contiguous files (on the Masscomp) have to specify
      * the size in the open call that creates them.
      */
      if (head->header.linkflag == LF_CONTIG)
         fd = rw_open_ ((longname ? longname : head->header.name)
         + skipcrud,
         openflag | O_CTG,
         hstat.st_mode, hstat.st_size);
      else
#endif
      {
      /*
      * With 3-arg open(), we can do this up right.
         */
         fd = rw_open_ (skipcrud + current_file_name,
            openflag, hstat.st_mode);
         
      }
      
      if (fd < 0)
      {
         if (make_dirs (skipcrud + current_file_name))
            goto again_file;
         if (es_instalacion)
         {
            if (reintento < 20)
            {
               if (!reintento && Eliminador_Diagram)
               {
                  if ((*Eliminador_Diagram)(current_file_name))
                  {
                     sleep(1);
                     reintento++;
                     goto again_file;
                  }
               }
               reintento++;
               if (reintento > 3 && instalacion_silenciosa < 2)
               {
                  sprintf(tmp_c,"Reintentando escribir %s",current_file_name); 
                  insta_mensaje(0,tmp_c);
               }
               sleep(1);
               goto again_file;
            }
            gzip_aborta = 1;
            abandonalo_ya = 1;		 
            gzip_a_salir = 1;
            instalacion_ok = 0;
            if (instalacion_silenciosa < 2)
            {
               sprintf(tmp_c,"No se puede escribir %s!",current_file_name);
               if (!instalacion_silenciosa)
               {			   
                  insta_aviso(0,tmp_c);
               }
               else
               {
                  insta_mensaje(0,tmp_c);
                  pito(77);pito(78);pito(79);pito(80);
               }
            }
            return;
         }
         /*msg_perror ("Could not create file %s",
         skipcrud + current_file_name);*/
         skip_file ((long) hstat.st_size);
         if (gzip_a_salir)
         {
            gzip_unsave_rec = 1;
            return;
         }
         goto quit;
      }
      
      if (es_instalacion && reintento > 3 && instalacion_silenciosa < 2)
      {
         Bl(4);
      }
extract_file:
      for (size = hstat.st_size;
      size > 0;
      size -= written)
      {
         
      /*			long	offset,
         numbytes;*/
         
         
         /*
         * Locate data, determine max length
         * writeable, write it, record that
         * we have used the data, then check
         * if the write worked.
         */
         data = findrec ()->charptr;
         if (gzip_a_salir)
         {
            gzip_unsave_rec = 1;
            gzip_continuacion = size;
            if ( (gzip_continuacion%blocksize) )
            {
               gzip_continuacion += (blocksize - gzip_continuacion%blocksize);
            }
            gzip_realsize = size;
            gzip_continuafd = fd;
            return;
         }
         if (data == NULL)
         {			/* Check it... */
            /*msg("Unexpected EOF on archive file");*/
            break;
         }
         /*
         * JK - If the file is sparse, use the sparsearray
         * that we created before to lseek into the new
         * file the proper amount, and to see how many
         * bytes we want to write at that position.
         */
         /*	if (head->header.linkflag == LF_SPARSE) {
         off_t pos;
         
          pos = lseek(fd, (off_t) sparsearray[sparse_ind].offset, 0);
          printf("%d at %d\n", (int) pos, sparse_ind);
          written = sparsearray[sparse_ind++].numbytes;
      } else*/
         written = endofrecs ()->charptr - data;
         if (written > size)
            written = size;
         errno = 0;
         check = rw_write_ (fd, data, written);
         /*
         * The following is in violation of strict
         * typing, since the arg to userec
         * should be a struct rec *.  FIXME.
         */
         userec ((union record *) (data + written - 1));
         if (check == written)
            continue;
            /*
            * Error in writing to file.
            * Print it, skip to next file in archive.
         */
         if (check < 0)
            /*msg_perror ("couldn't write to file %s",skipcrud + current_file_name)*/;
            else
            /*msg ("could only write %d of %d bytes to file %s",
            check, written, skipcrud + current_file_name);*/
            skip_file ((long) (size - written));
         break;		/* Still do the close, mod time, chmod, etc */
      }
      
      check = rw_close_ (fd);
      if (check < 0)
      {
      /*msg_perror ("Error while closing %s",
         skipcrud + current_file_name);*/
      }
      
      
set_filestat:
      
/*
* If we are root, set the owner and group of the extracted
* file.  This does what is wanted both on real Unix and on
* System V.  If we are running as a user, we extract as that
* user; if running as root, we extract as the original owner.
      */
      if (we_are_root || f_do_chown)
						{
         if (chown (skipcrud + current_file_name,
            hstat.st_uid, hstat.st_gid) < 0)
         {
         /*msg_perror ("cannot chown file %s to uid %d gid %d",
         skipcrud + current_file_name,
            hstat.st_uid, hstat.st_gid);*/
         }
						}
      
                  /*
                  * Set the modified time of the file.
                  *
                  * Note that we set the accessed time to "now", which
                  * is really "the time we started extracting files".
                  * unless f_gnudump is used, in which case .st_atime is used
      */
      /* We do the utime before the chmod because some versions of
      utime are broken and trash the modes of the file.  Since
      we then change the mode anyway, we don't care. . . */
      
      /*
      * If '-k' is not set, open() or creat() could have saved
      * the permission bits from a previously created file,
      * ignoring the ones we specified.
      * Even if -k is set, if the file has abnormal
      * mode bits, we must chmod since writing or chown() has
      * probably reset them.
      *
      * If -k is set, we know *we* created this file, so the mode
      * bits were set by our open().   If the file is "normal", we
      * skip the chmod.  This works because we did umask(0) if -p
      * is set, so umask will have left the specified mode alone.
      */
      if ((!f_keep)
         || (hstat.st_mode & (S_ISUID | S_ISGID | S_ISVTX)))
						{
         if (chmod (skipcrud + current_file_name,
            notumask & (int) hstat.st_mode) < 0)
         {
         /*msg_perror ("cannot change mode of file %s to %ld",
         skipcrud + current_file_name,
            notumask & (int) hstat.st_mode);*/
         }
						}
      if (gzip_a_salir)
						{		   
         gzip_unsave_rec = 1;
         return;
						}
      
quit:
                  break;
                  
    case LF_LINK:
again_link:
       {
          struct stat st1, st2;
          
          if (f_unlink && !f_keep) {
             if (unlink(skipcrud + current_file_name) == -1)
                if (errno != ENOENT)
                /*msg_perror ("Could not unlink %s",
                skipcrud + current_file_name)*/;
          }
          
          check = link (current_link_name, skipcrud + current_file_name);
          
          if (check == 0)
             break;
          if (make_dirs (skipcrud + current_file_name))
             goto again_link;
          if (stat (current_link_name, &st1) == 0
             && stat (current_file_name + skipcrud, &st2) == 0
             && st1.st_dev == st2.st_dev
             && st1.st_ino == st2.st_ino)
             break;
             /*msg_perror ("Could not link %s to %s",
             skipcrud + current_file_name,
          current_link_name);*/
       }
       break;
       
#ifdef COMENTARIO
#ifdef S_ISLNK
    case LF_SYMLINK:
again_symlink:
       if (f_unlink && !f_keep) {
          if (unlink(skipcrud + current_file_name) == -1)
             if (errno != ENOENT)
             /*msg_perror ("Could not unlink %s",
             skipcrud + current_file_name)*/;
       }
       
       check = symlink (current_link_name,
          skipcrud + current_file_name);
       /* FIXME, don't worry uid, gid, etc... */
       if (check == 0)
          break;
       if (make_dirs (current_file_name + skipcrud))
          goto again_symlink;
          /*msg_perror ("Could not create symlink to %s",
       current_link_name);*/
       break;
#endif
#endif
       
#ifdef S_IFCHR
    case LF_CHR:
       hstat.st_mode |= S_IFCHR;
       goto make_node;
#endif
       
#ifdef S_IFBLK
    case LF_BLK:
       hstat.st_mode |= S_IFBLK;
#endif
#if defined(S_IFCHR) || defined(S_IFBLK)
make_node:
       if (f_unlink && !f_keep) {
          if (unlink(skipcrud + current_file_name) == -1)
             if (errno != ENOENT)
             /*msg_perror ("Could not unlink %s",
             skipcrud + current_file_name)*/;
       }
       
       check = mknod (current_file_name + skipcrud,
          (int) hstat.st_mode, (int) hstat.st_rdev);
       if (check != 0)
       {
          if (make_dirs (skipcrud + current_file_name))
             goto make_node;
             /*msg_perror ("Could not make %s",
          current_file_name + skipcrud);*/
          break;
       };
       goto set_filestat;
#endif
       
#ifdef S_ISFIFO
       /* If local system doesn't support FIFOs, use default case */
    case LF_FIFO:
make_fifo:
       if (f_unlink && !f_keep) {
          if (unlink(skipcrud + current_file_name) == -1)
             if (errno != ENOENT)
             /*msg_perror ("Could not unlink %s",
             skipcrud + current_file_name)*/;
       }
       
       check = mkfifo (current_file_name + skipcrud,
          (int) hstat.st_mode);
       if (check != 0)
       {
          if (make_dirs (current_file_name + skipcrud))
             goto make_fifo;
             /*msg_perror ("Could not make %s",
          skipcrud + current_file_name);*/
          break;
       };
       goto set_filestat;
#endif
       
    case LF_DIR:
       namelen = strlen (current_file_name + skipcrud) - 1;
really_dir:
       /* Check for trailing /, and zap as many as we find. */
       while (namelen && current_file_name[skipcrud + namelen] == '/')
          current_file_name[skipcrud + namelen--] = '\0';
       
again_dir:
       check = mkdir (skipcrud + current_file_name,(we_are_root ? 0 : 0300) | (int) hstat.st_mode);
       if (check != 0)
       {
          struct stat st1;
          
          if (make_dirs (skipcrud + current_file_name))
             goto again_dir;
          /* If we're trying to create '.', let it be. */
          if (current_file_name[skipcrud + namelen] == '.' &&
             (namelen == 0 ||
             current_file_name[skipcrud + namelen - 1] == '/'))
             goto check_perms;
          if (errno == EEXIST
             && stat (skipcrud + current_file_name, &st1) == 0
             && (S_ISDIR (st1.st_mode)))
             break;
          /*msg_perror ("Could not create directory %s", skipcrud + current_file_name);*/
          break;
       }
       
check_perms:
       if (!we_are_root && 0300 != (0300 & (int) hstat.st_mode))
       {
          hstat.st_mode |= 0300;
          /*msg ("Added write and execute permission to directory %s",
          skipcrud + current_file_name);*/
       }
       
       /*
       * If we are root, set the owner and group of the extracted
       * file.  This does what is wanted both on real Unix and on
       * System V.  If we are running as a user, we extract as that
       * user; if running as root, we extract as the original owner.
       */
       if (we_are_root || f_do_chown)
       {
          if (chown (skipcrud + current_file_name,
             hstat.st_uid, hstat.st_gid) < 0)
          {
          /*msg_perror ("cannot chown file %s to uid %d gid %d",
          skipcrud + current_file_name,
             hstat.st_uid, hstat.st_gid);*/
          }
       }
       
       
       /* This functions exactly as the code for set_filestat above. */
       if ((!f_keep)
          || (hstat.st_mode & (S_ISUID | S_ISGID | S_ISVTX)))
       {
          if (chmod (skipcrud + current_file_name,
             notumask & (int) hstat.st_mode) < 0)
          {
          /*msg_perror ("cannot change mode of file %s to %ld",
          skipcrud + current_file_name,
             notumask & (int) hstat.st_mode);*/
          }
       }
       
       break;
          case LF_LONGNAME:
          case LF_LONGLINK:
             /*msg ("Visible long name error\n");*/
             skip_file ((long) hstat.st_size);
             if (gzip_a_salir)
             {		
                gzip_unsave_rec = 1;
                return;
             }	  
             break;
    }
    /* We don't need to save it any longer. */
    saverec ((union record **) 0);/* Unsave it */
}


int agi_atar(pp,destino)
char *pp;
char *destino;
{
   char *argv[250];
   int argc,i;
   char filtro[81];
   char tmp[260];
   char p[260];
   register DIR *dirp;
#ifdef ESXENIX
   register struct direct *d;
#else
   register struct dirent *d;
#endif
   strcpy(p,pp);
   
   argc = _separa(p,argv,1,249);
   if (!argc) return(-1);
   
   
   ar_files[0] = p;
   otro_destino = destino;
   n_ar_files = 1;
   cur_ar_file = 0;
   
   if (!open_archive (0))
   {
      for (i = 0;i < argc;i++)
      {
         if (index(argv[i],'*'))
         {
            strcpy(filtro,quita_path(argv[i]));
            *quita_path(argv[i]) = 0;
            
            if (otro_destino)
            {
               strcpy(tmp,otro_destino);
            }
            else
               tmp[0] = 0;
            if (tmp[0] && tmp[strlen(tmp)-1] != '/' && tmp[strlen(tmp)-1] != '\\')
            {
               strcat(tmp,"/");
            }
            strcat(tmp,argv[i]);
            if (!strlen(tmp))
            {
               strcpy(tmp,"./");
            }
            dirp = opendir (tmp);
            if (dirp)
            {
               while( (d = readdir(dirp)) )
               {
                  if (coincide_(d->d_name,filtro))
                  {
                     strcpy(tmp,argv[i]);
                     strcat(tmp,d->d_name);
                     dump_file (tmp, -1, 1); 
                  }
               }
               closedir(dirp);
            }
         }
         else
         {
            dump_file (argv[i], -1, 1); 
         }
      }
      write_eot ();
   }
   close_archive ();
   
   otro_destino = NULL;
   n_ar_files = 0;
   cur_ar_file = 0;
   
   return(0);
}


static unsigned read_gunzip_tar(pfd,buf,tam)
int *pfd;
unsigned char *buf;
unsigned tam;
{
   if (gzip_aborta)
			{
      return(0);
			}
   return(rw_read_(*pfd,buf,tam));
}

static void write_gunzip_tar(fd,buf,tam)
int fd;
unsigned char *buf;
unsigned tam;
{
			long real_size;
         if (gzip_aborta) return;
         
#ifdef FGVWIN
         agi_cola_win();
#endif
         
         if (gzip_skip)
         {
            if (es_instalacion == 1)
            {
               while(gzip_skip && tam)
               {
                  if ( (tam+gzip_parcial_len) < blocksize)
                  {
                     memcpy(gzip_parcial+gzip_parcial_len,buf,tam);
                     gzip_parcial_len += tam;
                     return;
                  }
                  memcpy(gzip_parcial+gzip_parcial_len,buf,blocksize-gzip_parcial_len);
                  buf += (blocksize-gzip_parcial_len);
                  tam -= (blocksize-gzip_parcial_len);
                  gzip_skip -= blocksize;
                  rellena_instalacion(gzip_parcial);		
                  gzip_parcial_len = 0;
                  if (gzip_skip < 0L)
                     gzip_skip = 0L;
               }
               if (!gzip_skip)
               {
                  es_instalacion++;		
                  if (!es_sacarwi && pregunta_instalacion())
                  {
                     gzip_aborta  = 1;
                     abandonalo_ya = 1;
                     return;
                  }
                  instalacion_ok = 1;
                  if (es_sacarwi)
                  {
                     gzip_aborta  = 1;
                     abandonalo_ya = 1;
                     return;
                  }		
               }
               if (!tam)
               {
                  return;
               }
            }
            
            if (gzip_parcial_len)
            {
               if (gzip_parcial_len <= gzip_skip)
               {
                  gzip_skip -= gzip_parcial_len;
                  gzip_parcial_len = 0;
               }
               else
               {
                  gzip_parcial_len -= gzip_skip;
                  memcpy(gzip_parcial,gzip_parcial+gzip_skip,gzip_parcial_len);
                  gzip_skip = 0L;	   
               }
            }
            if (gzip_skip)
            {
               if (gzip_skip <= tam)
               {
                  buf += gzip_skip;
                  tam -= gzip_skip;
                  if (es_instalacion)
                  {
                     presenta_progreso(gzip_skip);
                  }
                  gzip_skip = 0L;
               }
               else
               {
                  gzip_skip -= tam;
                  if (es_instalacion)
                  {
                     presenta_progreso(tam);
                  }
                  return;
               }
            }
         }
         if (gzip_continuacion)
         {
            if (gzip_continuacion <= tam)
            {
               if (gzip_realsize)
               {
                  rw_write_ (gzip_continuafd, buf,gzip_realsize);
                  gzip_realsize = 0L;
               }
               tam -= gzip_continuacion;
               buf += gzip_continuacion;
               if (es_instalacion)
               {
                  presenta_progreso(gzip_continuacion);
               }
               gzip_continuacion = 0L;
            }
            else
            {
               if (tam <= gzip_realsize)
               {
                  rw_write_ (gzip_continuafd, buf,tam);	  
                  gzip_realsize -= tam;
               }
               else
               {	
                  rw_write_ (gzip_continuafd, buf,gzip_realsize);
                  gzip_realsize = 0L;
               }
               gzip_continuacion -= tam;
               if (es_instalacion)
               {
                  presenta_progreso(tam);
               }
               tam = 0;
            }  
            
            if (!gzip_continuacion)
            {
               rw_close_ (gzip_continuafd);    
               if (we_are_root || f_do_chown)
               {
                  chown (current_file_name,hstat.st_uid, hstat.st_gid);
               }	
               if ((!f_keep) || (hstat.st_mode & (S_ISUID | S_ISGID | S_ISVTX)))
               {
                  chmod (current_file_name,notumask & (int) hstat.st_mode);
               }
               if (match == 3) 
               {
                  /* caso especial instalacion */
                  instala_pon_en_su_sitio(current_file_name);
               }
				if (match == 4)
				{ /* Instalar fonts */
					instala_font_(current_file_name);
				}
            }
         }
         if (!tam)
         {
            return;
         }
         gzip_buf = buf;
         gzip_buf_len = tam;
         gzip_buf_punto = 0;
         if (gzip_unsave_rec)
         {
            saverec ((union record **) 0);
            gzip_unsave_rec = 0;
         }
         gzip_a_salir = 0;
         gzip_aborta = 0;
         read_and (extract_archive);
         if (!gzip_a_salir)
         {
            gzip_aborta = 1;
            abandonalo_ya = 1;
         }
         return;
}

int agi_detar(pp,destino)
char *pp;
char *destino;
{ 
   char *argv[250];
   int argc,i,ant_compre;
   int solo_opc = 0;
   char fichero[260];
   void (*_write_interno)();
   unsigned (*_read_interno)();
   
   strcpy(fichero,pp);
   
   es_instalacion = 0;

   argc = _separa(fichero,argv,1,249);
   
   es_sacarwi = 0;    
   if (argc == 1 && (!(es_sacarwi = strcmp(argv[0],"[INSTALACION]")) || !strcmp(argv[0],"[CONTROLRWI]")))
   {
      marcada_la_ayuda = 0;
      es_instalacion = 1;
      nname_matchs = 0;
      name_matchs = argv;
      en_minusculas = 1;
      instalar_progreso     = 0L;
      memset(&infp,0,sizeof(infp));	  
      instalacion_ok = 0;
      rellena_modo = 0;
      if (hay_ins_continuacion)
      {
         hay_ins_continuacion--;
         rwi_parte = 1;
         pregunta_insta = 0;
      }
      else
      {
         rwi_parte = 0;
         pregunta_insta = 1;
      }
   }	  
   else
   {
      nname_matchs = argc;
      name_matchs = argv;
   }
   
   quitab(fichero);
   ar_files[0] = fichero;
   otro_destino = destino;
   n_ar_files = 1;
   cur_ar_file = 0;
   extr_init ();
   i = strlen(fichero) - 4;
   if (i > 0 && (!strcmp(fichero+i,".tgz") || !strcmp(fichero+i,".taz") || 
      !strcmp(fichero+i+2,".Z") || !strcmp(fichero+i+2,".z") ||
				  !strcmp(fichero+i,".tgZ") || !strcmp(fichero+i,".taZ") ))
   {	  
      i = rw_open_(fichero,O_BINARY | O_RDONLY);
      if (i >= 0)
      {
#ifdef RWSERVER
         /* no se puede usar el gzip dos veces al mismo tiempo */
         if (en_conexion_externa)
         {
            ant_compre = net_dame_compresion();
            net_reasigna_compresion(0);
         }
#endif
         _write_interno = write_interno;
         _read_interno  = read_interno;
         write_interno  = write_gunzip_tar;
         read_interno   = read_gunzip_tar;
         gzip_a_salir = 0;
         gzip_unsave_rec = 0;
         gzip_continuacion = 0L;
         gzip_inicio = 0;
         estoy_en_gzip = 1;
         gzip_parcial_len = 0;
         gzip_skip = 0L;
         gzip_aborta = 0;
         ds___gzip(i,0,1,6);
         abandonalo_ya = 0;
         rw_close_(i);
         estoy_en_gzip = 0;
         gzip_a_salir = 0;
         gzip_continuacion = 0L;		
         gzip_inicio = 0;
         write_interno  = _write_interno;
         read_interno   = _read_interno;
#ifdef RWSERVER
         if (en_conexion_externa)
         {			 
            net_reasigna_compresion(ant_compre);
         }
#endif
      } 
   }
   else
   {
      gzip_skip = 0L;
      gzip_a_salir = 0;
      gzip_continuacion = 0L;
      gzip_inicio = 0;
      estoy_en_gzip = 0;
      read_and (extract_archive);
   }
   otro_destino = NULL;
   n_ar_files = 0;
   cur_ar_file = 0;
   
   if (es_instalacion)
   {	 	 
      if (instalar_fijos)
      {
         while(ninstalar_fijos)
         {
            ninstalar_fijos--;
            free(instalar_fijos[ninstalar_fijos]);
         }
         free(instalar_fijos);
         instalar_fijos = NULL;
      }
      if (instalar_varia)
      {
         while(ninstalar_varia)
         {
            ninstalar_varia--;
            free(instalar_varia[ninstalar_varia]);
         }
         free(instalar_varia);
         instalar_varia = NULL;
      }
      
      if (instalacion_ok != 1 && instalacion_ok != 3) hay_ins_continuacion = 0;
      
      if (instalar_submenu)
      {
#ifdef COMENTARIO
         if (!es_sacarwi && instalacion_ok == 1)
         {
            solo_opc = 1;
            for (i = 0;i < ninstalar_submenu;i++)
            {
               if (strbusca(instalar_submenu[i],".opc") < 0)
               {
                  solo_opc = 0;
                  break;
               }
            }        
            for (i = 0;i < ninstalar_submenu;i++)
            {
               modmenu(destino,instalar_submenu[i],solo_opc);
            }
         }
#endif
         while(ninstalar_submenu)
         {
            ninstalar_submenu--;
            free(instalar_submenu[ninstalar_submenu]);
         }
         free(instalar_submenu);
         instalar_submenu = NULL;
      }
      
      if (registro_instalacion)
      {
         for (i = 0;i < nregistro_instalacion;i++)
         {
            item_registro(registro_instalacion[i]);
         }
         while(nregistro_instalacion)
         {
            nregistro_instalacion--;
            free(registro_instalacion[nregistro_instalacion]);
         }
         free(registro_instalacion);
         registro_instalacion = NULL;
         graba_agiregistro();
      }
      
      
      if (instalar_postsystem && !hay_ins_continuacion)
      {
         if (!es_sacarwi && instalacion_ok == 1)
         {
            for (i = 0;i < ninstalar_postsystem;i++)
            {
               fin_sig();
               strcpy(fichero,instalar_postsystem[i]);
               cambia_por_destino(fichero,destino);
               agi_system(fichero);
               ini_sig();
            }
         }
         while(ninstalar_postsystem)
         {
            ninstalar_postsystem--;
            free(instalar_postsystem[ninstalar_postsystem]);
         }
         free(instalar_postsystem);
         instalar_postsystem = NULL;
      }
      if (instalar_iconos && !hay_ins_continuacion)
      {
#ifdef FGVDOS	  
         if (!es_sacarwi && instalacion_ok == 1 && !sin_iconos)
            Pon_En_Icono_(instalar_iconos,ninstalar_iconos,destino);
#endif
         while(ninstalar_iconos)
         {
            ninstalar_iconos--;
            free(instalar_iconos[ninstalar_iconos]);
         }
         free(instalar_iconos);
         instalar_iconos = NULL;
      }
      if (textoi_instalacion && !hay_ins_continuacion)
      {
         while(ntextoi_instalacion)
         {
            ntextoi_instalacion--;
            free(textoi_instalacion[ntextoi_instalacion]);
         }	    
         free(textoi_instalacion);
         textoi_instalacion = NULL;
      }
      if (textof_instalacion  && !hay_ins_continuacion)
      {
         while(ntextof_instalacion)
         {
            ntextof_instalacion--;
            free(textof_instalacion[ntextof_instalacion]);
         }	    
         free(textof_instalacion);
         textof_instalacion = NULL;
      }
      if (variables_instalacion  && !hay_ins_continuacion)
      {
         while(nvariables_instalacion)
         {
            nvariables_instalacion--;
            free(variables_instalacion[nvariables_instalacion]);
         }	    
         free(variables_instalacion);
         variables_instalacion = NULL;
      }
      if (stdini_instalacion  && !hay_ins_continuacion)
      {
         if (!es_sacarwi && instalacion_ok == 1)
         {
            for (i = 0;i < nstdini_instalacion;i++)
            {
               strcpy(fichero,stdini_instalacion[i]);
               cambia_por_destino(fichero,destino);
               pon_en_ini(fichero);
            }
            graba_fgv(1);
         }
         while(nstdini_instalacion)
         {
            nstdini_instalacion--;
            free(stdini_instalacion[nstdini_instalacion]);
         }	    
         free(stdini_instalacion);
         stdini_instalacion = NULL;
      }
      if (instalar_limpia  && !hay_ins_continuacion)
      {
         while(ninstalar_limpia)
         {
            ninstalar_limpia--;
            if (!es_sacarwi && instalacion_ok == 1)
            {  
               strcpy(fichero,instalar_limpia[ninstalar_limpia]);
               cambia_por_destino(fichero,destino);
               unlink(fichero);
            }
            free(instalar_limpia[ninstalar_limpia]);
         }	    
         free(instalar_limpia);
         instalar_limpia = NULL;
      }
      if (!hay_ins_continuacion)
      {
         for (i = 0;i < 10;i++)
         {
            if (memoria_instalacion[i])
            {
               free(memoria_instalacion[i]);
               memoria_instalacion[i] = NULL;
            }
         }
      }
      presenta_progreso(total_out-instalar_progreso);
    }
    
    en_minusculas = 0;
    rwi_parte = 0;
    
    return(0);
}

struct info_paquete *datos_paquete(paquete,continua)
char *paquete;
char *continua;
{
   char tmp[256];
   if (!paquete)
   { 
      hay_ins_continuacion = 0;
      return(NULL);
   }
   
   if (!continua)
   {
      posible_ins_continuacion = 0;
      hay_ins_continuacion = 0;
   }
   else
      posible_ins_continuacion = 1;
   
   sprintf(tmp,"%s%s.tgz [CONTROLRWI]",rw_directorio("master/"),paquete);
   agi_detar(tmp,"");
   
   posible_ins_continuacion = 0;
   if (instalacion_ok)
   {
      ultima_instalacion = 2; /* fijo a 2 */
      if (continua)
      {
         if (hay_ins_continuacion > 0)
         {
            strcpy(continua,ins_continuacion[hay_ins_continuacion-1]);
         }
         else
            hay_ins_continuacion = 0;
      }
      else
         hay_ins_continuacion = 0;  
      return(&infp);
   }
   else
      return(NULL);
}

struct info_paquete *instala_paquete(paquete,continua)
char *paquete;
char *continua;
{
   int i;
   char tmp[256];
   char destino[256];
   
   if (!paquete)
			{ 
      hay_ins_continuacion = 0;
      return(NULL);
			}
   
   if (!continua)
			{
      posible_ins_continuacion = 0;
      hay_ins_continuacion = 0;
			}
   else
      posible_ins_continuacion = 1;
   
   strcpy(destino,rw_directorio(""));
   sprintf(tmp,"%s%s.tgz [INSTALACION]",rw_directorio("master/"),paquete);
   p_version1 = 0;
   p_version2 = 0;
   p_versionletra = 'a';
   for (i = 0;i < nPaq;i++)
			{ 
      if (!strcmp(paquete,Paq[i].nompaq))
      {
         p_version1 = Paq[i].i.version1;
         p_version2 = Paq[i].i.version2;
         p_versionletra = Paq[i].i.versionletra;
         break;
      }
			}
   agi_detar(tmp,destino);
   
   if (instalacion_ok && strcmp(paquete,"agitool") && strcmp(paquete,"dstool") && strcmp(paquete,"dsimpre") && (infp.tipo == 'P' || infp.tipo == 'I' || infp.tipo == 'p'))
   {
      sprintf(tmp,"%s%s.rwi",rw_directorio("master/"),paquete);
      unlink(tmp);
      sprintf(tmp,"%s%s.tgz",rw_directorio("master/"),paquete);
      unlink(tmp);
   }
   
   posible_ins_continuacion = 0;
   if (instalacion_ok)
			{
      ultima_instalacion = instalacion_ok;
      if (continua)
      {
         if (hay_ins_continuacion > 0)
         {
            strcpy(continua,ins_continuacion[hay_ins_continuacion-1]);
         }
         else
            hay_ins_continuacion = 0;
      }
      else
         hay_ins_continuacion = 0;
         /*
         if (infp.es_ayuda[0])
         {
         strcpy(destino,rw_directorio(infp.es_ayuda));
         Ap();
         pprint(2120," *PREPARANDO AYUDAS EN FORMATO HTML* ");
         Cp();
         ds_help(destino); 
         }
      *//* esta en htmhelp.c */
      
      compon_agi_menu();
      
      presenta_progreso(-1);
      return(&infp);
			}
   else
			{
      presenta_progreso(-1);
      return(NULL);
			}
}

static int fd = -1;
extern long dame_mi_numero();


static int bloquea_region_sys_1(fd,nume,debe)
int fd;
long nume;
int debe;
{
			int r = 0;
         long inip = 50002L;
         long tam1 = 998L;
         long tam2 = 0L;
         
         if (debe) inip = 50052L;
         
         if (nume >= inip)
         {
            tam1 = nume - inip;
            tam2 = 50999L - nume;
         }
         if (tam1)
         {
            rw_lseek_(fd,inip,SEEK_SET);
            r = locking(fd,2,tam1);
            rw_lseek_(fd,inip,SEEK_SET);
            locking(fd,0,tam1);
            if (r) return(r);
         }
         if (!tam2) return(r);
         rw_lseek_(fd,inip+tam1+1L,SEEK_SET);
         r = locking(fd,2,tam2);
         rw_lseek_(fd,inip+tam1+1L,SEEK_SET);
         locking(fd,0,tam2);
         return(r);
}

static int bloquea_region_sys(fd,nume,debe)
int fd;
long nume;
int debe;
{
   int r = bloquea_region_sys_1(fd,nume,1);
   
   if (r)
      return(-1);
   if (debe)
   {
      return(1);
   }
   r = bloquea_region_sys_1(fd,nume,0);
   if (r)
      return(1); /* solo paquetes */
   return(0);
}

int carga_master(debe_grabar)
int debe_grabar;
{
   char tmp[512];
   char tmp2[81];
   FILE *file;
   int coma;
   int i,j,ri;
   long nume;
   struct info_paquete *info;
   DIR *dd;
#ifdef ESXENIX
   struct direct *_buf;
#else
   struct dirent *_buf;
#endif

   /* SININSTALACION */
   return 0;
   
   nPaq = 0;
   puede_grabar = 0;  
   
   nume = dame_mi_numero();
   fd = agi_control_fildes;
   if (fd == -1)
   {
				  strcpy(tmp,rw_directorio("informa.pan"));
                  fd = rw_open_(tmp,O_BINARY | O_RDWR);
   }
   if (fd >= 0) 
   {  
	 			  rw_lseek_(fd,50001L,SEEK_SET);
                  if (locking(fd,2,1L))
                  {
                     insta_mensaje(400,"Esperando a tener el control de las instalaciones ...");
                     for (i = 0;i < 100;i++)
                     {
                        rw_lseek_(fd,50001L,SEEK_SET);
                        if (!locking(fd,1,1L)) break;
                     }
                     Bl(4);
                     if (i >= 100)
                     {
                        if (agi_control_fildes == -1)
                        {
                           rw_close_(fd);
                           fd = -1;
                        }
                        return(-1);
                     }
                  }
                  
                  if ((ri = bloquea_region_sys(fd,nume,debe_grabar)) != -1)
                  {
                     rw_lseek_(fd,50000L,SEEK_SET);
                     if (locking(fd,2,1L)) 
                     {	   
                        if (debe_grabar)
                        {		  
                           rw_lseek_(fd,50001L,SEEK_SET);
                           locking(fd,0,1L);
                           if (agi_control_fildes == -1)
                           {
                              rw_close_(fd);
                              fd = -1;
                           }
                           return(-1);
                        }
                     }
                     else
                     {
                        puede_grabar = 1 + ri;
                     }
                  }
                  else
                  {
                     rw_lseek_(fd,50000L,SEEK_SET);
                     if (locking(fd,2,1L)) 
                     {
                        /* alguien se ha colado en instalacion */		  
                        insta_mensaje(0,"SE ESTA INSTALANDO ALGUN PROGRAMA!!");
						/*
                        pito(81);pito(82);
                        fin_sig();
                        Terfinal();
                        rw_lseek_(fd,50000L,SEEK_SET);
                        locking(fd,0,1L);
                        rw_lseek_(fd,50001L,SEEK_SET);
                        locking(fd,0,1L);
                        rw_close_(fd);
                        exit(2);
						*/
						rw_lseek_(fd,50000L,SEEK_SET);
						locking(fd,0,1L);
						return 0;
                     }
                     rw_lseek_(fd,50000L,SEEK_SET);
                     locking(fd,0,1L);
                  }
   }     
   else
						puede_grabar = 1;  
   
   strcpy(tmp,rw_directorio("master/"));
   crea_dir(tmp); /* por si acaso !!! */
   
                  /*
                  if (!Paq)
                  {
                  Paq = (struct paquetes *)malloc(sizeof(struct paquetes) * MAXIMO_PAQUETES); 
                  memset((char *)Paq,0,sizeof(struct paquetes) * MAXIMO_PAQUETES);
                  }
   */
   
   strcpy(tmp,rw_directorio("master/agilista.rwp"));
   file = fopen(tmp,"rt");
   if (file) 
   {
						while(lee_linea(tmp,500,file))
                  {		
                     comas_linea(tmp,0,1,&coma);
                     quita_blancos(tmp);
                     i = strlen(tmp);	
                     if (i >= sizeof(Paq[nPaq].nompaq))
                        continue;
                     
                     memset(&(Paq[nPaq]),0,sizeof(Paq[nPaq]));
                     
                     strcpy(Paq[nPaq].nompaq,tmp);
                     strminus(Paq[nPaq].nompaq);
                     
                     j = comas_linea(tmp,1,1,&coma);
                     quita_blancos(tmp+j);
                     Paq[nPaq].i.tipo = tmp[j];
                     j = comas_linea(tmp,1,1,&coma);
                     quita_blancos(tmp+j);
                     if (strlen(tmp+j) < 16)
                     {
                        strcpy(Paq[nPaq].i.sistema,tmp+j);
                     }
                     else
                     {
                        strcpy(Paq[nPaq].i.sistema,"DESCONOCIDO");
                     }
                     Paq[nPaq].i.version1 = atoi(tmp+comas_linea(tmp,1,1,&coma));
                     Paq[nPaq].i.version2 = atoi(tmp+comas_linea(tmp,1,1,&coma));
                     j = comas_linea(tmp,1,1,&coma);
                     quita_blancos(tmp+j);
                     if (!tmp[j])
                        Paq[nPaq].i.versionletra = ' ';
                     else
                        Paq[nPaq].i.versionletra = tmp[j];
                     Paq[nPaq].i.autoversion = atoi(tmp+comas_linea(tmp,1,1,&coma)); 
                     j = comas_linea(tmp,1,1,&coma);
                     quita_blancos(tmp+j);
                     if (strlen(tmp+j) > 20)
                     {
                        tmp[j+20] = 0;
                     }
                     strcpy(Paq[nPaq].i.fecha,tmp+j);
                     j = comas_linea(tmp,1,1,&coma);
                     quitabp(tmp+j);
                     quitab(tmp+j);
                     if (strlen(tmp+j) > 30)
                     {
                        tmp[j+30] = 0;
                     }
                     strcpy(Paq[nPaq].i.descrip,tmp+j);
                     Paq[nPaq].i.ver_interna = atoi(tmp+comas_linea(tmp,1,1,&coma)); 
                     Paq[nPaq].i.num_interno = atoi(tmp+comas_linea(tmp,1,1,&coma));
                     Paq[nPaq].i.rdepende1 = atoi(tmp+comas_linea(tmp,1,1,&coma)); 
                     Paq[nPaq].i.rdepende2 = atoi(tmp+comas_linea(tmp,1,1,&coma)); 
                     j = comas_linea(tmp,1,1,&coma);
                     quita_blancos(tmp+j);
                     Paq[nPaq].i.rdependeletra = tmp[j];	
                     Paq[nPaq].i.rdependemodo = atoi(tmp+comas_linea(tmp,1,1,&coma)); 
                     Paq[nPaq].i.pdepende1 = atoi(tmp+comas_linea(tmp,1,1,&coma)); 
                     Paq[nPaq].i.pdepende2 = atoi(tmp+comas_linea(tmp,1,1,&coma)); 
                     j = comas_linea(tmp,1,1,&coma);
                     quita_blancos(tmp+j);
                     Paq[nPaq].i.pdependeletra = tmp[j];	
                     Paq[nPaq].i.pdependemodo = atoi(tmp+comas_linea(tmp,1,1,&coma)); 
                     Paq[nPaq].i.dependesubsistema = atoi(tmp+comas_linea(tmp,1,1,&coma)); 
                     j = comas_linea(tmp,1,1,&coma);
                     quita_blancos(tmp+j);
                     if (strlen(tmp+j) > 20)
                     {
                        tmp[j+20] = 0;
                     }
                     strcpy(Paq[nPaq].i.origen,tmp+j);
                     Paq[nPaq].control_utime = atol(tmp+comas_linea(tmp,1,1,&coma));	
					 if (!Paq[nPaq].control_utime)
					 {
						 Paq[nPaq].flag = 2;
					 }
					 else
					 {
						 Paq[nPaq].flag = 0;
					 }
                     nPaq++;
                  }
                  fclose(file);
   }  
   
   /* leer directorio y crear entradas en base a paquete.rwi */
   j = 0;
   strcpy(tmp,rw_directorio("master/"));
   dd = opendir(tmp);
   if (dd)
   {
						while( (_buf = readdir(dd)) )
                  {
                     strcpy(tmp2,_buf->d_name);
                     
                     i = strlen(tmp);		   
                     strcat(tmp,_buf->d_name);
#ifdef FGVDOS
                     SetFileAttributes(tmp,FILE_ATTRIBUTE_NORMAL);
#else
                     chmod(tmp,0777);
#endif      
                     tmp[i] = 0;
                     
                     
                     quitab(tmp2);
                     quitabp(tmp2);
                     strminus(tmp2);
                     if (coincide_(tmp2,"*.tgz"))
                     {
                        tmp2[strlen(tmp2)-4] = 0;
                        for (i = 0;i < nPaq;i++)
                        {
                           if (!strcmp(Paq[i].nompaq,tmp2))
                           {
                              break;
                           }
                        }
                        if (i >= nPaq)
                        {
                           /* paquete nuevo */
                           if (!j)
                           {
                              if (instalacion_silenciosa < 2)
                              {
                                 insta_mensaje(400,"Registrando nuevas aplicaciones encontradas en master ...");
                                 pito(83);
                              }
                              j = 1;
                           }
                           info = datos_paquete(tmp2,NULL);
                           if (info)
                           {
                              strcpy(Paq[nPaq].nompaq,tmp2);
                              Paq[nPaq].control_utime = 0L;
                              memcpy(&(Paq[nPaq].i),info,sizeof(Paq[nPaq].i));
                              Paq[nPaq].flag = 2;
                              nPaq++;
                           } 
                           else
                           {
                              i = strlen(tmp);			 
                              strcat(tmp,_buf->d_name);
                              unlink(tmp);
                              tmp[i] = 0;
                           }
                        }
                     }
                     else
                     {
						 /*
                        if (strcmp(tmp2,".") && strcmp(tmp2,"..") && strcmp(tmp2,"agilista.rwp") && 
                           memcmp(tmp2,"instalar",8))
                        {
                           i = strlen(tmp);		   
                           strcat(tmp,_buf->d_name);
                           unlink(tmp);
                           tmp[i] = 0;		   
                        }
						*/
                     }
                  }
                  closedir(dd);
                  if (j)
                  {
                     Bl(4);
                  }
   }
   
   if (fd >= 0) 
   {  
						rw_lseek_(fd,50001L,SEEK_SET);
                  locking(fd,0,1L);	 
   }
   return(puede_grabar);
}

int descarga_master()
{
   char tmp[512];
   FILE *file;
   int i;

   /* SININSTALACION */
   return 0;

   strcpy(tmp,rw_directorio("master/agilista.rwp"));
   
   if (nPaq)
   {
      for (i = 0;i < nPaq;i++)
      {
         if (Paq[i].flag >= 2) break;
      }
      if (i < nPaq)
      {	       
         if (fd >= 0) 
         {  
            rw_lseek_(fd,50001L,SEEK_SET);
            if (locking(fd,2,1L))
            { 
               for (i = 0;i < 100;i++)
               {
                  rw_lseek_(fd,50001L,SEEK_SET);
                  if (!locking(fd,1,1L)) break;
               }
            }
         }
         file = fopen(tmp,"wt");
         if (file) 
         {
            for (i = 0;i < nPaq;i++)
            {
               if (Paq[i].flag == -1 || !(Paq[i].control_utime)) continue;
               fprintf(file,"%s,%c,%s,%d,%02d,%c,%d,%s,%s,%d,%d,%d,%02d,%c,%d,%d,%02d,%c,%d,%d,%s,%ld\n",
                  Paq[i].nompaq,
                  Paq[i].i.tipo,
                  Paq[i].i.sistema,
                  Paq[i].i.version1,
                  Paq[i].i.version2,
                  Paq[i].i.versionletra,
                  Paq[i].i.autoversion,
                  Paq[i].i.fecha,
                  Paq[i].i.descrip,
                  Paq[i].i.ver_interna, 
                  Paq[i].i.num_interno,
                  Paq[i].i.rdepende1,
                  Paq[i].i.rdepende2,
                  Paq[i].i.rdependeletra,					  
                  Paq[i].i.rdependemodo,
                  Paq[i].i.pdepende1,
                  Paq[i].i.pdepende2,
                  Paq[i].i.pdependeletra,					  
                  Paq[i].i.pdependemodo,
                  Paq[i].i.dependesubsistema,
                  Paq[i].i.origen,				  
                  Paq[i].control_utime);
               if (Paq[i].flag == 3)
                  Paq[i].flag = 0;
            }
            fclose(file);
         }    
         if (fd >= 0) 
         {  
            rw_lseek_(fd,50001L,SEEK_SET);
            locking(fd,0,1L);	 
         }
      }
   }
   
   if (fd >= 0 && puede_grabar) 
   {  
      rw_lseek_(fd,50000L,SEEK_SET);
      locking(fd,0,1L);
      if (agi_control_fildes == -1)
      {
         rw_close_(fd);
         fd = -1;
      }	
   }
   puede_grabar = 0;
   
   if (!nPaq)
   {
      unlink(tmp);
   }
   
   
   return(0);
}




#ifdef FGVDOS


/*****************************************/
/*
*  BitmapToDIB()        - Creates a DIB from a bitmap
*  ChangeBitmapFormat() - Changes a bitmap to a specified DIB format
*  ChangeDIBFormat()    - Changes a DIB's BPP and/or compression format
*  CopyScreenToBitmap() - Copies entire screen to a standard Bitmap
*  CopyScreenToDIB()    - Copies entire screen to a DIB
*  CopyWindowToBitmap() - Copies a window to a standard Bitmap
*  CopyWindowToDIB()    - Copies a window to a DIB
*  CreateDIBPalette()   - Creates a palette from a DIB
*  CreateDIB()          - Creates a new DIB
*  DestroyDIB()         - Deletes DIB when finished using it
*  DIBError()           - Displays message box with error message
*  DIBHeight()          - Gets the DIB height
*  DIBNumColors()       - Calculates number of colors in the DIB's color table
*  DIBToBitmap()        - Creates a bitmap from a DIB
*  DIBWidth()           - Gets the DIB width
*  FindDIBBits()        - Sets pointer to the DIB bits
*  GetSystemPalette()   - Gets the current palette
*  LoadDIB()            - Loads a DIB from a file
*  PaintBitmap()        - Displays standard bitmap in the specified DC
*  PaintDIB()           - Displays DIB in the specified DC
*  PalEntriesOnDevice() - Gets the number of palette entries
*  PaletteSize()        - Calculates the buffer size required by a palette
*  PrintDIB()           - Prints the specified DIB
*  PrintScreen()        - Prints the entire screen
*  PrintWindow()        - Prints all or part of a window
*  SaveDIB()            - Saves the specified dib in a file
*/

/*
HDIB APIENTRY BitmapToDIB (HBITMAP hBitmap, HPALETTE hPal);
HDIB APIENTRY ChangeBitmapFormat (HBITMAP	hBitmap,
WORD     wBitCount,
DWORD    dwCompression,
HPALETTE hPal);
HDIB APIENTRY ChangeDIBFormat (HDIB hDIB, WORD wBitCount,
DWORD dwCompression);
HBITMAP APIENTRY CopyScreenToBitmap (LPRECT);
HDIB APIENTRY CopyScreenToDIB (LPRECT);
HBITMAP APIENTRY CopyWindowToBitmap (HWND, WORD);
HDIB APIENTRY CopyWindowToDIB (HWND, WORD);
HPALETTE APIENTRY CreateDIBPalette (HDIB);
HDIB APIENTRY CreateDIB(DWORD, DWORD, WORD);
WORD APIENTRY DestroyDIB (HDIB);
void APIENTRY DIBError (int ErrNo);
DWORD APIENTRY DIBHeight (LPSTR lpDIB);
WORD APIENTRY DIBNumColors (LPSTR lpDIB);
HBITMAP APIENTRY DIBToBitmap (HDIB hDIB, HPALETTE hPal);
DWORD APIENTRY DIBWidth (LPSTR lpDIB);
LPSTR APIENTRY FindDIBBits (LPSTR lpDIB);
HPALETTE APIENTRY GetSystemPalette (void);
HDIB APIENTRY LoadDIB (LPSTR);
BOOL APIENTRY PaintBitmap (HDC, LPRECT, HBITMAP, LPRECT, HPALETTE);
BOOL APIENTRY PaintDIB (HDC, LPRECT, HDIB, LPRECT, HPALETTE);
int APIENTRY PalEntriesOnDevice (HDC hDC);
WORD APIENTRY PaletteSize (LPSTR lpDIB);
WORD APIENTRY PrintDIB (HDIB, WORD, WORD, WORD, LPSTR);
WORD APIENTRY PrintScreen (LPRECT, WORD, WORD, WORD, LPSTR);
WORD APIENTRY PrintWindow (HWND, WORD, WORD, WORD, WORD, LPSTR);
WORD APIENTRY SaveDIB (HDIB, LPSTR);
*/


#define HDIB HANDLE


/* Print Area selection */
#define PW_WINDOW        1
#define PW_CLIENT        2


/* Print Options selection */
#define PW_BESTFIT       1
#define PW_STRETCHTOPAGE 2
#define PW_SCALE         3


/*
FARPROC BitmapToDIB (HBITMAP hBitmap, HPALETTE hPal) = NULL;
FARPROC ChangeBitmapFormat (HBITMAP	hBitmap,
WORD     wBitCount,
DWORD    dwCompression,
HPALETTE hPal);
FARPROC ChangeDIBFormat (HDIB hDIB, WORD wBitCount,
DWORD dwCompression);
FARPROC CopyScreenToBitmap (LPRECT);
FARPROC CopyScreenToDIB (LPRECT);
FARPROC CopyWindowToBitmap (HWND, WORD);
FARPROC CopyWindowToDIB (HWND, WORD);
FARPROC CreateDIBPalette (HDIB);
FARPROC CreateDIB(DWORD, DWORD, WORD);
FARPROC DestroyDIB (HDIB);
FARPROC DIBError (int ErrNo);
FARPROC DIBHeight (LPSTR lpDIB);
FARPROC DIBNumColors (LPSTR lpDIB);
FARPROC DIBToBitmap (HDIB hDIB, HPALETTE hPal);
FARPROC DIBWidth (LPSTR lpDIB);
FARPROC FindDIBBits (LPSTR lpDIB);
FARPROC GetSystemPalette (void);
FARPROC LoadDIB (LPSTR);
FARPROC PaintBitmap (HDC, LPRECT, HBITMAP, LPRECT, HPALETTE);
FARPROC PaintDIB (HDC, LPRECT, HDIB, LPRECT, HPALETTE);
FARPROC PalEntriesOnDevice (HDC hDC);
FARPROC PaletteSize (LPSTR lpDIB);
FARPROC PrintDIB (HDIB, WORD, WORD, WORD, LPSTR);
FARPROC PrintScreen (LPRECT, WORD, WORD, WORD, LPSTR);
FARPROC PrintWindow (HWND, WORD, WORD, WORD, WORD, LPSTR);
FARPROC SaveDIB (HDIB, LPSTR);
FARPROC InstallHook (HWND hWnd, BOOL bCode );
*/



static HINSTANCE libreria = NULL;
static FARPROC 
BitmapToDIB        = NULL,
ChangeBitmapFormat = NULL,     
ChangeDIBFormat    = NULL,
CopyScreenToBitmap = NULL,     
CopyScreenToDIB    = NULL,
CopyWindowToBitmap = NULL,      
CopyWindowToDIB    = NULL,      
CreateDIBPalette   = NULL,
CreateDIB          = NULL,       
DestroyDIB         = NULL,         
DIBError           = NULL,         
DIBHeight          = NULL,         
DIBNumColors       = NULL,         
DIBToBitmap        = NULL,         
DIBWidth           = NULL,         
FindDIBBits        = NULL,         
GetSystemPalette   = NULL,
LoadDIB            = NULL,       
PaintBitmap        = NULL,       
PaintDIB           = NULL,       
PalEntriesOnDevice = NULL,     
PaletteSize        = NULL,   
PrintDIB           = NULL,     
PrintScreen        = NULL,     
PrintWindow        = NULL,     
SaveDIB            = NULL,
InstallHook        = NULL;


static int carga_libreria()
{
   libreria = LoadLibrary("agirwmm1");
   if (libreria)
   {           	 
      BitmapToDIB        = GetProcAddress(libreria,"BitmapToDIB");
      ChangeBitmapFormat = GetProcAddress(libreria,"ChangeBitmapFormat");
      ChangeDIBFormat    = GetProcAddress(libreria,"ChangeDIBFormat");
      CopyScreenToBitmap = GetProcAddress(libreria,"CopyScreenToBitmap");
      CopyScreenToDIB    = GetProcAddress(libreria,"CopyScreenToDIB");
      CopyWindowToBitmap = GetProcAddress(libreria,"CopyWindowToBitmap");
      CopyWindowToDIB    = GetProcAddress(libreria,"CopyWindowToDIB");
      CreateDIBPalette   = GetProcAddress(libreria,"CreateDIBPalette"); 
      CreateDIB          = GetProcAddress(libreria,"CreateDIB");
      DestroyDIB         = GetProcAddress(libreria,"DestroyDIB");
      DIBError           = GetProcAddress(libreria,"DIBError");
      DIBHeight          = GetProcAddress(libreria,"DIBHeight");
      DIBNumColors       = GetProcAddress(libreria,"DIBNumColors");
      DIBToBitmap        = GetProcAddress(libreria,"DIBToBitmap");
      DIBWidth           = GetProcAddress(libreria,"DIBWidth");
      FindDIBBits        = GetProcAddress(libreria,"FindDIBBits");
      GetSystemPalette   = GetProcAddress(libreria,"GetSystemPalette");
      LoadDIB            = GetProcAddress(libreria,"LoadDIB");
      PaintBitmap        = GetProcAddress(libreria,"PaintBitmap");
      PaintDIB           = GetProcAddress(libreria,"PaintDIB");
      PalEntriesOnDevice = GetProcAddress(libreria,"PalEntriesOnDevice");
      PaletteSize        = GetProcAddress(libreria,"PaletteSize");
      PrintDIB           = GetProcAddress(libreria,"PrintDIB");
      PrintScreen        = GetProcAddress(libreria,"PrintScreen");
      PrintWindow        = GetProcAddress(libreria,"PrintWindow");
      SaveDIB            = GetProcAddress(libreria,"SaveDIB");
      InstallHook        = GetProcAddress(libreria,"InstallHook");;
   }
   return(0);
}

static int descarga_libreria()
{
   FreeLibrary(libreria);
   libreria = NULL;
   BitmapToDIB        = NULL;
   ChangeBitmapFormat = NULL;     
   ChangeDIBFormat    = NULL;
   CopyScreenToBitmap = NULL;     
   CopyScreenToDIB    = NULL;
   CopyWindowToBitmap = NULL;      
   CopyWindowToDIB    = NULL;       
   CreateDIBPalette   = NULL; 
   CreateDIB          = NULL;       
   DestroyDIB         = NULL;         
   DIBError           = NULL;         
   DIBHeight          = NULL;         
   DIBNumColors       = NULL;         
   DIBToBitmap        = NULL;         
   DIBWidth           = NULL;         
   FindDIBBits        = NULL;         
   GetSystemPalette   = NULL;
   LoadDIB            = NULL;       
   PaintBitmap        = NULL;       
   PaintDIB           = NULL;       
   PalEntriesOnDevice = NULL;     
   PaletteSize        = NULL;     
   PrintDIB           = NULL;     
   PrintScreen        = NULL;     
   PrintWindow        = NULL;     
   SaveDIB            = NULL;
   InstallHook        = NULL;
   return(0);
}


/*****************************************/
#ifndef FGVWIN
extern HWND GetConsoleHwnd(void);
#endif

/* especifico de entorno NT  */

#ifdef RWSERVER
extern int en_conexion_externa;
#endif


int en_daclaves(clave_buffer)
char *clave_buffer;
{													   
   HINSTANCE libreria = NULL;
   FARPROC claves = NULL;
   
   libreria = LoadLibrary("daclaves.dll");
   if (libreria)
   {           	 
      claves = GetProcAddress(libreria,"claves");
   }
   if (!claves)
   {
      if (libreria)
         FreeLibrary(libreria);
      aviso(0,"No se encuentra o no se puede ejecutar daclaves.dll");
      return(-1);
   }
   
   claves((LPSTR)clave_buffer);
   
   FreeLibrary(libreria);
   return(0);
}


int graba_bmp_pantalla(tmp2)
char *tmp2;
{
   HWND ww;
   HWND ww2;
   HDIB dib;
   WORD area;
   area = PW_WINDOW;
#ifdef FGVWIN
   ww = principal_w;
#else
   ww = GetConsoleHwnd();
   Sleep(200);
#endif
   
#ifdef RWSERVER
   if (en_conexion_externa)
			{
      return(funcion_windows_cliente(0,tmp2,strlen(tmp2)+1));
			}
#endif
   
   if (fIsTNT == TRUE)
			{
      return(-1);
			}
   
   if (!libreria)
			{
      carga_libreria();
			}
   if (!libreria)
			{
      aviso(0,"No se puede acceder a agirwmm1.dll adecuadamente.");
      return(-1);
			}
   
   ww2 = ww;
   do
			{
            ww = ww2;
            ww2 = GetParent(ww);
         } while(ww2);
         
         
         dib = (HDIB)CopyWindowToDIB (ww,area);
         if (dib)
         {
            SaveDIB (dib,(LPSTR)tmp2);			 
            DestroyDIB (dib);	     
         }  
         else
         {
            pito(84);	   
            return(-1);
         }
         
         return(0);
}



BOOL Pon_En_Icono_(icons,nicons,destino) 
char *icons[];
int nicons;
char *destino;
{	
	int i,j;
	int pico;
	char *Paths,_Pat[256];
	char tmp[256];
	char *Programas;
	struct stat statt;
	

	if (!nicons || !icons) return(FALSE);

	CoInitialize(NULL);

	for (pico = 0;pico < nicons;pico++)
	{	    
		for (i = pico+1;i < nicons;i++)
		{
			Paths = icons[i];
			if (!strcmp(Paths,"."))
			{				
				break;
			}
			for (j = 0;Paths[j];j++)
			{
				if (Paths[j] == ',' && Paths[j+1] != ',')
				{
					Paths[j] = 0;
					j++;
					break;
				}
				else
					if (Paths[j] == ',' && Paths[j+1] == ',')
						j++;
			}
			Programas = Paths+j;		
			strcpy(_Pat,Paths);
			if (cambia_por_destino(_Pat,destino) == 8) continue;
			for (j = 0;_Pat[j] && _Pat[j] != ' ';j++)
			{
				tmp[j] = _Pat[j];
			}
			tmp[j] = 0;
			if (stat(tmp,&statt) != -1)
			{
				j++;
				while(_Pat[j] == ' ') j++;
				CreateShortcut(Programas,tmp,_Pat+j,icons[pico]);
			}
		}
		pico = i;
	}

	CoUninitialize( );

   return TRUE;
}



HDDEDATA CALLBACK GroupDDECallback (UINT uiType, UINT uiFmt, HANDLE hConv,
                                    HSZ sz1, HSZ sz2, HDDEDATA hData, LONG lData1, LONG lData2) {
			switch (uiType) {
            
         }
         return ((HDDEDATA) NULL);
}


#ifdef CONDDE

BOOL Pon_En_Icono_(icons,nicons,destino) 
char *icons[];
int nicons;
char *destino;
{
			char *Grupo;
         char *Programas;
         char *Paths,_Pat[256];
         char tmp[256];
         LPBYTE     lpByte;
         LPSTR      szGroups;
         LPSTR      szToken;
         LPSTR      szMem;
         LPSTR      szToken2;
         long       lResult;
         LONG       lIdLocal;
         // CRITICAL_SECTION lpCritical;
         HCONV      hConv;
         HDDEDATA   hDdeData;
         HSZ        szProgMan;
         HSZ        szTopic;
         BOOL		  hay_grupo;
         HDDEDATA   hData;
         LPSTR      szCommand;
         int i,j,r,pico,mip,k;
         struct stat statt;
         CONVCONTEXT   CCFilter = { sizeof (CONVCONTEXT), 0, 0, 0, 0L, 0L };
         
         
         if (fIsTNT == TRUE)
         {
            return(TRUE);
         }
         
         
         
         if (!nicons || !icons) return(FALSE);
         
         
         lIdLocal = 0L;
         szGroups = NULL;
         
         
         if (DdeInitialize (&lIdLocal, (PFNCALLBACK) GroupDDECallback,
            (DWORD) APPCMD_CLIENTONLY, 0L)) 
         {	  
            return (FALSE);
         }
         mip = GetThreadPriority(GetCurrentThread());
         SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_BELOW_NORMAL);
         
         szProgMan = DdeCreateStringHandle (lIdLocal, "PROGMAN", CP_WINANSI);
         
         
         hConv = DdeConnect (lIdLocal, szProgMan, szProgMan, &CCFilter);
         
         
         szTopic = DdeCreateStringHandle (lIdLocal, "Groups", CP_WINANSI);
         
         
         hDdeData = DdeClientTransaction (NULL, 0L, hConv, szTopic, CF_TEXT,
            XTYP_REQUEST, 10000, &lResult);
         
         
         DdeFreeStringHandle (lIdLocal, szProgMan);
         DdeFreeStringHandle (lIdLocal, szTopic);
         
         
         szMem = NULL;
         if (hDdeData && (hDdeData != (HDDEDATA) TRUE)) {   
            lpByte = DdeAccessData (hDdeData, &lResult);
            szGroups = szMem = VirtualAlloc (NULL, lResult, MEM_COMMIT,
               PAGE_READWRITE);
            if (szMem) 
            {
               memcpy (szGroups, lpByte, lResult);
            }
            DdeUnaccessData (hDdeData);
            DdeFreeDataHandle (hDdeData);
         }
         
         
         
         pico = 0;
         szCommand = VirtualAlloc (NULL, 512, MEM_COMMIT, PAGE_READWRITE);
         
         while(szMem && szCommand && pico < nicons)
         {
            Grupo = icons[pico];   
            hay_grupo = FALSE;
            szToken = strtok (szGroups, "\n");
            while (szToken) 
            {   
               if (szToken2 = strrchr (szToken, 0x0d)) 
               {
                  *szToken2 = '\0';
               }
               if (!strcmp(szToken,Grupo))
               {
                  hay_grupo = TRUE;
                  break;
               }
               szToken = strtok (NULL, "\n");
            } 	  	  
            
            Sleep(200);
            for (k = 0;k < 5 && (!k || hay_grupo == FALSE);k++)
            {
               if (hay_grupo == FALSE) 
               {
                  sprintf (szCommand, "[CreateGroup(%s)]", Grupo);
                  
                  hData = DdeCreateDataHandle (lIdLocal, szCommand,
                     strlen (szCommand) + 1, 0, (HSZ) NULL, CF_TEXT, 0L);
                  if (DdeClientTransaction ((LPBYTE) hData, 0xFFFFFFFF, hConv,
                     (HSZ) NULL, 0, XTYP_EXECUTE, 10000, &lResult)) 
                  {
                     hay_grupo = TRUE;			  
                  }
                  Sleep(100);
                  DdeFreeDataHandle(hData);
                  Sleep(100);
               }
               else {
                  sprintf (szCommand, "[ShowGroup(%s,1)]", Grupo);
                  
                  hData = DdeCreateDataHandle (lIdLocal, szCommand, strlen (szCommand),
                     0, (HSZ) NULL, CF_TEXT, 0L);
                  if (!DdeClientTransaction ((LPBYTE) hData, 0xFFFFFFFF, hConv,
                     (HSZ) NULL, 0, XTYP_EXECUTE, 10000, &lResult)) 
                  {               
                     hay_grupo = FALSE;			 
                  }
                  Sleep(100);
                  DdeFreeDataHandle(hData);
                  Sleep(100);
               }
            }
            
            
            if (hay_grupo == TRUE)
            {
               pico++;
               for (i = pico;i < nicons;i++,pico++)
               {
                  Paths = icons[i];
                  if (!strcmp(Paths,"."))
                  {
                     pico = i + 1;
                     Sleep(200);
                     break;
                  }
                  for (j = 0;Paths[j];j++)
                  {
                     if (Paths[j] == ',' && Paths[j+1] != ',')
                     {
                        Paths[j] = 0;
                        j++;
                        break;
                     }
                     else
                        if (Paths[j] == ',' && Paths[j+1] == ',')
                           j++;
                  }
                  Programas = Paths+j;		
                  strcpy(_Pat,Paths);
                  if (cambia_por_destino(_Pat,destino) == 8) continue;
                  for (j = 0;_Pat[j] && _Pat[j] != ' ';j++)
                  {
                     tmp[j] = _Pat[j];
                  }
                  tmp[j] = 0;
                  if (stat(tmp,&statt) != -1)
                  {
                     for (k = 0;k < 10;k++)
                     {
                        Sleep(100);
                        sprintf (szCommand, "[AddItem(%s,%s)]", _Pat,
                           (LPARAM) Programas);
                        hData = DdeCreateDataHandle (lIdLocal, szCommand,
                           strlen (szCommand) + 1, 0, (HSZ) NULL, CF_TEXT, 0L);
                        if (DdeClientTransaction ((LPBYTE) hData, 0xFFFFFFFF,
                           hConv, (HSZ) NULL, 0, XTYP_EXECUTE, 10000, &lResult))
                        {			 
                           k = 20;
                        }
                        Sleep(100);
                        DdeFreeDataHandle(hData);		  
                     }
                  }
               }
            }
            else
               break;
   } /* while */
   
   
   Sleep(500);
   
   VirtualFree (szMem, lResult, MEM_DECOMMIT);   
   VirtualFree (szCommand, 512, MEM_DECOMMIT);
   
   
   DdeDisconnect (hConv);
   
   SetThreadPriority(GetCurrentThread(),mip);
   
   Sleep(500);
   DdeUninitialize (lIdLocal);
   Sleep(100);
   
   
#ifdef FGVWIN
   SetForegroundWindow(principal_w);
#else
   SetForegroundWindow(GetConsoleHwnd());   
#endif
   
   
   return (TRUE);
}

#endif


static HCONV      hConv = NULL;
static LONG       lIdLocal = 0L;
static char       _item[257];
static char       _topic[257];

int _dde_init(programa,item,topic,modo)
char *programa;
char *item;
char *topic;
int modo;
{
   UINT e;
   int i;
   char buf[256];
   HSZ        szProgMan;
   HSZ        szTopic;
   
   if (lIdLocal) 
   {
      if (hConv);
      DdeDisconnect (hConv);
      hConv = NULL;
      DdeUninitialize (lIdLocal); 
      lIdLocal = 0L;
   }
   
   
   if (DdeInitialize (&lIdLocal, (PFNCALLBACK) GroupDDECallback,
      (DWORD) APPCMD_CLIENTONLY, 0L)) 
   {
      lIdLocal = 0L;
      return (-1);
   } 
   
   
   szProgMan = DdeCreateStringHandle (lIdLocal, item, CP_WINANSI);
   szTopic = DdeCreateStringHandle (lIdLocal,topic, CP_WINANSI);  
   
   i = 0;
   do {
      hConv = DdeConnect (lIdLocal, szProgMan, szTopic,NULL);
      if (!hConv)
      {
         /*e = DdeGetLastError(  lIdLocal );*/
         if (!i)
         {		 
            if (!modo)
               e = WinExec(programa, SW_SHOWMINNOACTIVE);
            else
               e = WinExec(programa, SW_SHOWNORMAL);
            if (e < 32)
            {
               DdeFreeStringHandle (lIdLocal, szProgMan);   
               DdeFreeStringHandle (lIdLocal, szTopic);  
               DdeUninitialize (lIdLocal); 
               lIdLocal = 0L;
               return(-1);
            }
            if (!modo)
            {
#ifdef FGVWIN
               BringWindowToTop(principal_w);   
#else
               BringWindowToTop(GetConsoleHwnd());   
#endif
            }
         }
         Sleep(250);
         i++;
      }
   } while(!hConv && i < 30);
   DdeFreeStringHandle (lIdLocal, szProgMan);   
   DdeFreeStringHandle (lIdLocal, szTopic);  
   strcpy(_item,item);
   strcpy(_topic,topic);
   return(0);
}

static int topic_reconect(topic)
char *topic;
{
   int i;
   HSZ        szProgMan;
   HSZ        szTopic;
   
   if (strcmp(topic,_topic))
			{
      DdeDisconnect (hConv);
      szProgMan = DdeCreateStringHandle (lIdLocal, _item, CP_WINANSI);
      szTopic = DdeCreateStringHandle (lIdLocal,topic, CP_WINANSI);  
      i = 0;
      do {
         hConv = DdeConnect (lIdLocal, szProgMan, szTopic,NULL);
         if (!hConv)
         {       
            Sleep(100);
            i++;
         }
      } while(!hConv && i < 100);
      DdeFreeStringHandle (lIdLocal, szProgMan);   
      DdeFreeStringHandle (lIdLocal, szTopic);  
      if (!hConv)
      {
         DdeUninitialize (lIdLocal); 
         lIdLocal = 0L;
         return(-1);
      }
      strcpy(_topic,topic);
			}
   return(0);
}


static int _dde_error()
{
			UINT e;
         e = DdeGetLastError(lIdLocal);
         switch(e)
         {
         case DMLERR_ADVACKTIMEOUT:
            return(-2); 
         case DMLERR_BUSY:
            return(-3);
         case DMLERR_DATAACKTIMEOUT:
            return(-4);
         case DMLERR_DLL_NOT_INITIALIZED:
            return(-5);
         case DMLERR_EXECACKTIMEOUT:
            return(-6);
         case DMLERR_INVALIDPARAMETER:
            return(-7);
         case DMLERR_MEMORY_ERROR:
            return(-8);
         case DMLERR_NO_CONV_ESTABLISHED:
            return(-9);
         case DMLERR_NO_ERROR:
            return(-10);
         case DMLERR_NOTPROCESSED:
            return(-11);
         case DMLERR_POKEACKTIMEOUT:
            return(-12);
         case DMLERR_POSTMSG_FAILED:
            return(-13);
         case DMLERR_REENTRANCY:
            return(-14);
         case DMLERR_SERVER_DIED:
            return(-15);
         case DMLERR_UNADVACKTIMEOUT:
            return(-16);
         default:
            return(-9999);
         }
         return(0);
}

int _dde_exec(par,topic)
char *par;
char *topic;
{
			long       lResult;
         
         
         if (!lIdLocal || !hConv) 
         {
            return(-1);
         }
         if (topic_reconect(topic)) return(-1);
         
         if (!DdeClientTransaction ((LPBYTE)par, strlen (par) + 1, hConv,(HSZ)0,CF_TEXT,XTYP_EXECUTE ,/*TIMEOUT_ASYNC*/ 30000,&lResult)) 
         {
            return(_dde_error());
         }
         
         return(0);    
}


int _dde_poke(par,item,topic)
char *par;
char *item;
char *topic;
{
   long       lResult;
   HSZ        hCommand;
   
   if (!lIdLocal || !hConv) 
   {
      return(-1);
   }
   if (topic_reconect(topic)) return(-1);
   
   
   hCommand = DdeCreateStringHandle (lIdLocal, item , CP_WINANSI);     
   if (!DdeClientTransaction ((LPBYTE)par, strlen (par) + 1, hConv,hCommand,CF_TEXT, XTYP_POKE ,/* TIMEOUT_ASYNC */ 30000, &lResult))
   {
      DdeFreeStringHandle (lIdLocal, hCommand);  
      return(_dde_error());
   }
   DdeFreeStringHandle (lIdLocal, hCommand);
   return(0);    
}


int _dde_peek(par,item,topic)
char **par;
char *item;
char *topic;
{
   long       lResult;
   HSZ        hCommand;
   HDDEDATA   hDdeData;
   LPBYTE     lpByte;
   
   if (!lIdLocal || !hConv) 
   {
						return(-1);
   }
   if (topic_reconect(topic)) return(-1);
   
   
   hCommand = DdeCreateStringHandle (lIdLocal, item , CP_WINANSI);
   hDdeData = DdeClientTransaction (NULL, 0L, hConv, hCommand, CF_TEXT,XTYP_REQUEST, 30000/* TIMEOUT_ASYNC */, &lResult);
   DdeFreeStringHandle (lIdLocal, hCommand);
   
   if (hDdeData && (hDdeData != (HDDEDATA) TRUE)) 
   {
						lpByte = DdeAccessData (hDdeData, &lResult);
                  if (lResult > 250)
                     lResult = 250;
                  if (!(*par) || strlen(*par) < lResult)
                  {
                     if (*par)
                        free(*par);	  
                     *par = malloc(lResult+1);
                  }
                  memcpy(*par,lpByte, lResult);      
                  *((*par)+lResult) = 0;
                  DdeUnaccessData (hDdeData);
                  DdeFreeDataHandle (hDdeData);
   }
   else
   {
						return(_dde_error());
   }
   return(0);    
}


int _dde_fin()
{
   if (!lIdLocal || !hConv) 
   {
      return(-1);
   }
   DdeDisconnect (hConv);
   hConv = NULL;
   DdeUninitialize (lIdLocal); 
   lIdLocal = 0L;
   return(0);
}

#else

#ifdef RWSERVER
extern int en_conexion_externa;
#endif


int graba_bmp_pantalla(tmp2)
char *tmp2;
{
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      return(funcion_windows_cliente(0,tmp2,strlen(tmp2)+1));
   }
#endif
   return(-1);
}

#endif
