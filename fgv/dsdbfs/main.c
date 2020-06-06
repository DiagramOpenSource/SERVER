#include <errno.h>
#include <time.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <signal.h>
#ifdef WIN32


#include <process.h>
#include <windows.h>            
#include <windowsx.h>

#include <winsock.h>
#include <nspapi.h>

#define usleep(a) Sleep(a/1000)

#define pthread_t unsigned long
#define pthread_create(a,b,c,d) *(a) = _beginthread(c,(unsigned)0,d)
#define ERROR_THREAD(a) ((a) == -1)
#define pthread_kill(a,b) ResumeThread(a)
#define DSsigwait(a,b,c)  SuspendThread(a)
#define sigemptyset(a) /* */
#define sigaddset(a,b) /* */

#ifndef SIGUSR1
#define SIGUSR1     16
#endif
#ifndef SIGUSR2
#define SIGUSR2     17  /* User defined signal 2 */
#endif
#ifndef SIGUSR3
#define SIGUSR3     20  /* User defined signal 3 */
#endif
#define sigset_t int
#define ERRORWAIT(a) (a == 0xFFFFFFFF)

int fork()
{
  return -1;
}

#else

#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/filio.h>
#include <netdb.h>
#include <sys/time.h>
#ifdef WATCOMNLM
#include <io.h>
#else
#include <unistd.h>
#endif

#include <time.h>
#include <sys/timeb.h>


#define DSsigwait(a,b,c)      sigwait(b,c)
#define ERRORWAIT(a) (a != 0)

#define SOCKET int
#define SOCKET_ERROR -1
#define INVALID_HANDLE_VALUE -1
#define closesocket close
#define ioctlsocket ioctl


#define CRITICAL_SECTION pthread_rwlock_t
#define InitializeCriticalSection(a) pthread_rwlock_init(a,NULL)
#define EnterCriticalSection pthread_rwlock_wrlock
#define LeaveCriticalSection pthread_rwlock_unlock
#define DeleteCriticalSection pthread_rwlock_destroy


static pthread_attr_t t_attr;

/*
#define CRITICAL_SECTION int
#define InitializeCriticalSection(a) InitBasura(a)
#define EnterCriticalSection EntraBasura
#define LeaveCriticalSection SalBasura
#define DeleteCriticalSection FinBasura
*/

#define ERROR_THREAD(a) ((a) != 0)


#define FAR                 far
#define NEAR                near
#ifndef CONST
#define CONST               const
#endif

typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef float               FLOAT;
typedef int                 INT;
typedef unsigned int        UINT;

#ifndef FALSE
#define FALSE               0
#endif

#ifndef TRUE
#define TRUE                1
#endif

#endif

#ifndef MSG_WAITALL
#define MSG_WAITALL 0
#endif

/*

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
*/

#ifdef WIN32
#include <sys/locking.h>
#include <share.h>
#include <io.h>
#include <windows.h>
#define sleep(a) Sleep((long)(((long)a)*1000L));

#ifndef LK_UNLCK
#define LK_UNLCK _LK_UNLCK
#endif

#ifndef LK_LOCK
#define LK_LOCK  _LK_LOCK
#endif

#ifndef LK_WAIT
#define LK_WAIT  _LK_LOCK
#endif

#ifndef LK_NBLCK
#define LK_NBLCK _LK_NBLCK
#endif

#else

#define dsdb_usleep usleep
#define SH_DENYNO 0
#define SH_DENYRW 2
#define MAX_PATH 128
#define O_BINARY 0
#define LK_WAIT LK_LOCK

#endif

#include <errno.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef WIN32
#include <conio.h>
#include <ctype.h>
#include <direct.h>
#else
#include <dirent.h>
#endif

#include <math.h>
#include <time.h>
#include <sys/timeb.h>
#include "dsdbfs.h"

/* elemenotos para dsdb */
int dsdb_base_abre(char *base);
int dsdb_base_cierra(int base_fd);
int dsdb_lee_dir(int base_fd,DBDirEntry **_direntry,int (*callback)(DBDirEntry *));
int dsdb_u_stat_entry(int base_fd,DBDirEntry *pdirentry,DBIniData *pinidata);

int rw_usa_dsdb = 1; /* fichero "unico" para datos */
int rw_dsdb_tambloque = 8;
int rw_dsdb_bloquesdefecto = 0;
int rw_dsdb_bloquesaumento = 0;
extern int dsdb_sin_errores;

int (*ixxdat_aviso)(int,long,char *) = NULL;
char *rw_dsdb_filtro = NULL;

char *dsdb_serverip = NULL;

static char __dsdb_serverip[128] = "";

extern char *dsdb_;
/*****/

/* parametros */
int comando = -1;
int verbose = 0;
int force = 0;
/*****/


#ifndef IPTOS_LOWDELAY
#define IPTOS_LOWDELAY 0x10
#endif

static void FastSocket(SOCKET sock)
{
	int i;

#ifdef IP_TOS
	i = IPTOS_LOWDELAY;
	setsockopt(sock,IPPROTO_IP,IP_TOS,(const char *)&i,sizeof(i));
#endif

	i = 1;
	setsockopt(sock,SOL_SOCKET,SO_OOBINLINE,(const char *)&i,sizeof(i));

	i = -1;
	setsockopt(sock,IPPROTO_TCP,TCP_NODELAY,(const char *)&i,sizeof(i));
}

extern int dsdb_usa_crypto;

void cambia_barra(s)
char *s;
{
   while(*s)
   {
      if (*s == '\\') *s = '/';
      s++;
   }
}


#ifdef WIN32

/*extern BOOL fIsTNT;*/
struct dirent
{
   unsigned short d_ino;		/* a bit of a farce */
   int d_reclen;		/* more farce */
   int d_namlen;		/* length of d_name */
   char d_name[256];	/* garentee null termination */
};

struct _dircontents
{
   char *_d_entry;
   struct _dircontents *_d_next;
};

typedef struct _dirdesc
{
   int dd_id;			/* uniquely identify each open directory */
   long dd_loc;		/* where we are in directory entry is this */
   struct _dircontents *dd_contents;	/* pointer to contents of dir */
   struct _dircontents *dd_cp;	/* pointer to current position */
} DIR;


DIR *opendir (p)
char *p;
{
   DIR *dd;
   struct dirent *dr;
   char tmp[260];
   int i;
   WIN32_FIND_DATA __dir;
   HANDLE _busqueda = INVALID_HANDLE_VALUE;
   
   strcpy(tmp,p);
   i = strlen(tmp);
   if (!i) return(NULL);
   i--;
   if (tmp[i] != '/')
      strcat(tmp,"/*");
   else
      strcat(tmp,"*");
      /*
      if (fIsTNT == TRUE)
      {
      strcat(tmp,".*");
      }
   */
   dd = (DIR *)malloc(sizeof(DIR));
   dr = (struct dirent *)malloc(sizeof(struct dirent));
   memset(dd,0,sizeof(DIR));
   memset(dr,0,sizeof(struct dirent));
   _busqueda = FindFirstFile(tmp,&__dir);
   dd->dd_id = (int)_busqueda;
   dd->dd_loc = -1L;
   dd->dd_contents = (struct _dircontents *)dr;
   if (_busqueda != INVALID_HANDLE_VALUE)
   {
      strcpy(dr->d_name,__dir.cFileName);
      strlwr(dr->d_name);
      dr->d_namlen = strlen(__dir.cFileName);
   }
   return(dd);
}

struct dirent *readdir (dirp)
DIR *dirp;
{
   HANDLE _busqueda = INVALID_HANDLE_VALUE;
   struct dirent *dr = NULL;
   WIN32_FIND_DATA __dir;
   
   if (dirp) 
			{
      _busqueda = (HANDLE)dirp->dd_id;
      if (_busqueda == INVALID_HANDLE_VALUE) return(NULL);
      if (dirp->dd_loc == -1L)
      {
         dr = (struct dirent *)dirp->dd_contents;
         dirp->dd_loc = 0L;		
      }
      else
      {
         if (FindNextFile(_busqueda,&__dir) == TRUE) 
         { 
            dr = (struct dirent *)dirp->dd_contents;
            strcpy(dr->d_name,__dir.cFileName);
            strlwr(dr->d_name);
            dr->d_namlen = strlen(__dir.cFileName);
         } 
      }
			}
   return(dr);
}

void closedir(dirp)
DIR *dirp;
{
			struct dirent *dr = NULL;
         HANDLE _busqueda = INVALID_HANDLE_VALUE;
         if (dirp)
         {
            _busqueda = (void *)dirp->dd_id;  
            dr = (struct dirent *)dirp->dd_contents;
            if (dr)
               free(dr);
            free(dirp);
            if (_busqueda != INVALID_HANDLE_VALUE)
               FindClose(_busqueda);
         }
}

#endif


static char **lee_direct_interno(char *pdir,int flags)
{
   int op,r,contama = flags / 100;
   int extratam = 0;
   char **dir_ = NULL;
   char **tmp2;
#ifdef ESXENIX
   struct direct *buf;
#else
   struct dirent *buf;
#endif
   DIR *dd;
   char nom[256];
   char dir[256];
   char path[256];
   struct stat b_stat;      

   /*lee_direct_tot_tama = 0L;*/
   
   flags %= 100;
   if (contama)
   {
	   extratam = 35;
   }

   strcpy(dir,pdir);
   cambia_barra(dir);
   op = strlen(dir);
   if (op)
   {
      op--;
      while(op && dir[op] != '/') op--;
      if (dir[op] != '/')
      {
         strcpy(nom,dir);
         strcpy(dir,"./");
      }
      else
      {
         if (dir[op+1])
         {
            strcpy(nom,dir+op+1);
         }
         else
         {
            strcpy(nom,"*");
         }
         dir[op+1] = 0;
      }
   }
   else
      return(NULL);
   
   op = 0;
   r = 100;
   dir_  = (char **)malloc(sizeof(char *) * (r+1));
   if ((dd = opendir(dir))) 
   {
      while(-1) 
      {
         buf = readdir(dd);
         if (!buf) break;
         strcpy(path,dir);
         strcat(path,buf->d_name);
         stat(path,&b_stat); /* obtengo todos los datos refentes al fichero */
         if (!flags)
         {
#ifdef WIN32
            if ((b_stat.st_mode & _S_IFDIR)) continue; /* es directorio */
#else
            if ((b_stat.st_mode & 0x4000)) continue; /* es directorio */
#endif
         }
         else
            if (flags == 2)
            {
#ifdef WIN32
               if (!(b_stat.st_mode & _S_IFDIR)) continue; /* no es directorio */
#else
               if (!(b_stat.st_mode & 0x4000)) continue; /* no es directorio */
#endif
            }
            
            if (!coincide_(buf->d_name,nom)) continue;
            if (op == r) 
            {
               tmp2 = (char **)malloc(sizeof(char *) * (r+101));
               memcpy(tmp2,dir_ ,sizeof(char *) * r);
               free((char *)dir_ );
               dir_  = tmp2;
               r += 100;
            }
            *(dir_  + op) = malloc(strlen(buf->d_name)+3+extratam);
            if (flags == 3)
            {
               memcpy(*(dir_  + op),"  ",2);
#ifdef WIN32
               if ((b_stat.st_mode & _S_IFDIR))
#else
                  if ((b_stat.st_mode & 0x4000))
#endif
																					memcpy(*(dir_  + op),"* ",2);
                  
                  strcpy(*(dir_  + op)+2,buf->d_name);
            }
            else
               strcpy(*(dir_  + op),buf->d_name);
			if (contama)
			{
				sprintf(*(dir_  + op)+strlen(*(dir_  + op))+1,"%ld",b_stat.st_size);
			}
			/*lee_direct_tot_tama += b_stat.st_size;*/
            op++;
      }
      closedir(dd);
   }
   if (!op)
   {
      free(dir_);
      dir_ = NULL;
   }
   else
      *(dir_  + op) = NULL;
   return(dir_);
}

void libera_dir(char **dir_)
{
   int op = 0;
   if (!dir_) return;
   
   while(*(dir_  + op)) {
      free(*(dir_  + op));
      op++;
   }
   free((char *)dir_);
}


int strcatd(resultado,c)
char *resultado;
char *c;
{
   int c0,ps;
   c0=strlen(c);
   for ( ps=strlen(resultado);ps>=0;ps-- )
      *(resultado+ps+c0)=*(resultado+ps);
   for ( ps=0;ps<c0;ps++ )
      *(resultado+ps)=*(c+ps);
   return(0);
}

/* Evita importaciones en el dsdb */
char **lee_direct(char *pdir,int flags)
{
	return NULL;
}

char caracter_de_fecha = '/';
char rw_user[11] = "DSDB";
char nomapli[25] = "DSDB";
char modulo[9] = "DSDB";

char *rw_directorio(char *dir)
{
	static char tmp1[512];
	tmp1[0] = '/';
	tmp1[1] = 0;
	if (dir)
	{
		strcat(tmp1,dir);
	}
	return tmp1;
}

int guarda_incidencia(int cod,char *buf)
{
   return 0;
}

int strbusca(s1,s2)
char *s1;
char *s2;
{
/*#ifdef ESDOS
			char *r;
         if ((r = strstr(s1,s2)) != NULL) return((int)(r - s1));
#else
*/
         int i = 0,j;
         while (*s1 != '\0') {
            j = 0;
            while (*(s1+j) != '\0') {
               if (*(s1+j) != *(s2+j)) break;
               j++;
               if (*(s2+j) == '\0') return(i);
            }
            i++;
            s1++;
         }
/*         
#endif*/
         return(-1);
}

int coincide_(char *buf,char *filtro)
{
   int i,j;
   int n2 = strlen(filtro);
   char _p1[256],_p2[256];
   int comienzo = 0;
   char *p1 = _p1,*p2 = _p2;
   
   if (n2 > 128) return(0);
   
   strcpy(p1,filtro);
   strcpy(p2,buf);
   
   if (!strcmp(buf,".") || !strcmp(buf,"..")) return(0);
   if (!n2 || (n2 == 1 && filtro[0] == '*')) return(1);
   
   if (*p1 == '*') 
   {
      comienzo = 1;
      p1++;
   }
   
   for (i = 0;p1[i];i++)
   {
      if (p1[i] == '*')
      {
         p1[i] = 0;
         if (comienzo)
         {
            if ((j = strbusca(p2,p1)) < 0)
            {
               return(0);
            }
			p2 += j;
         }
         else
         {
            if (strlen(p2) < strlen(p1))
               return(0);
            if (memcmp(p2,p1,strlen(p1)))
               return(0);
            comienzo = 1;
         }
         p2 += strlen(p1);
         p1 += i;
         p1++;
         i = -1;
      }
   }
   if (i)
   {
      if (comienzo)
      {
         if (strlen(p2) < strlen(p1))
            return(0);
         if (memcmp(p2+strlen(p2)-strlen(p1),p1,strlen(p1)))
            return(0);
      }
      else
      {
         if (strcmp(p2,p1))
            return(0);
      }
   }
   return(1);
}


char *quita_path(char *ff)
{
   char *f0 = ff;
   ff += strlen(ff);
   while (ff >= f0 && *ff != '\\' && *ff != '/') ff--;
   ff++;
   if (ff == f0 && *f0 && *(f0+1) == ':')
   {
      ff += 2;
   }
   return(ff);
}

int aviso(int pos,char *mensa)
{
	printf("%s\r\n",mensa);
	return 0;
}

int pulsatecla()
{
	return -1;
}

void uso()
{
	printf("uso: dsdb c(crear) o u(actualizar) o x(extraer) o t(listar) o d(duplicar) o R(recuperar) o O(defragmentar) [p[password](corchetes obligatorios, nuevos dsdb encriptados) s[cacheip] m[path txt parametros hasp] f fichero.dsb -force -block=n (n=multiplos de 512) -size=n -factori=n (n = numero de bloques)] [tablas o database para 'd'...(siempre detras de los parametros)]\r\n");
}


extern unsigned long ip_2key;
extern char **lista_emprecopias;
extern unsigned char copia_key[248];

/* de uso de dsdb */
ds_off total;
char **tablas = NULL;
int ntablas  = 0;
char *basename = NULL;
char *basename2 = NULL;
int base_fd = -1;
int base_fd2 = -1;
char tmp0[MAX_PATH];
char tmp1[100000];
int nfichs = 0;
char **fichs = NULL;
/******/


int pintadir(DBDirEntry *pdirentry)
{
	DBIniData inidata;
	struct tm *lc;	
	int ann;
	memset((void *)&inidata,0,sizeof(inidata));
	dsdb_u_stat_entry(base_fd,pdirentry,&inidata);
	lc = localtime(&inidata.mtime);
	if (!lc)
	{
		 ann = 0;
         printf("%02d%c%02d%c%04d ",
            0 ,caracter_de_fecha,
            0 ,caracter_de_fecha,
            ann);            
         printf("%02d:%02d:%02d\t",
            0 ,
            0  ,
            0);            
	}
	else
	{
         ann = lc->tm_year;
         if (ann > 50)
            ann += 1900;
         else
            ann += 2000;
         printf("%02d%c%02d%c%04d ",
            lc->tm_mday ,caracter_de_fecha,
            (lc->tm_mon)+1 ,caracter_de_fecha,
            ann);            
         printf("%02d:%02d:%02d\t",
            lc->tm_hour ,
            lc->tm_min  ,
            lc->tm_sec);            
	}
	printf("\t%8ld ",inidata.size);	
	printf("%s\r\n",pdirentry->name);
	total += inidata.size;
	return 0;
}

int sacadir(DBDirEntry *pdirentry)
{
	int file = -1,fd,n;	

	if (ntablas)
	{
		for (n = 0;n < ntablas;n++)
		{
			if (coincide_(pdirentry->name,tablas[n]))
				break;
		}
		if (n >= ntablas)
			return 0;
	}	

	if (!force)		
	{
		file = open(pdirentry->name,O_RDONLY|O_BINARY);
	}
	if (file < 0)
	{
		file = open(pdirentry->name,O_RDWR|O_BINARY|O_CREAT|O_TRUNC,S_IREAD|S_IWRITE);
		if (file < 0)
		{
			printf("El fichero no se puede crear [%s]\r\n",pdirentry->name);
			return 0;
		}
		strcpy(tmp0,basename);
		strcat(tmp0,"/");
		strcat(tmp0,pdirentry->name);
		fd = dsdb_u_abre(tmp0,O_RDWR|O_BINARY,SH_DENYNO);
		if (fd < 0)
		{
			printf("El fichero no se puede leer [%s]\r\n",pdirentry->name);
		}
		else
		{
			if (verbose)
			{
				printf("%s\r\n",pdirentry->name);
			}
			while((n = dsdb_u_lee(fd,tmp1,sizeof(tmp1))) > 0)
			{
				if (write(file,tmp1,n) != n)
				{
					printf("Error al grabar en %s\r\n",pdirentry->name);
					break;
				}
			}
			dsdb_u_cierra(fd);
			if (force)
			{
				if (dsdb_unlink_(tmp0) < 0)
				{
					printf("No se ha borrado la tabla [%s]\r\n",tmp0);
				}
			}
		}
	}	
	else
	{
		printf("El fichero ya existe [%s]\r\n",pdirentry->name);
	}
	close(file);
	return 0;
}

void expande_tablas()
{
	/* expandir ntablas */	
	int n,nn;
	char **dir;
	char tmp3[512];

	fichs = (char **)malloc(sizeof(char *)*110);
	fichs[0] = NULL;
	nfichs = 0;
	for (n = 0;n < ntablas;n++)
	{		
		if (strbusca(tablas[n],"*") > -1)
		{
			dir = lee_direct_interno(tablas[n],0);
			if (dir)
			{
				for (nn = 0;dir[nn];nn++)
				{
					strcpy(tmp3,tablas[n]);
					strcpy(quita_path(tmp3),dir[nn]);
					fichs[nfichs] = malloc(strlen(tmp3)+1);
					strcpy(fichs[nfichs],tmp3);
					nfichs++;
					fichs[nfichs] = NULL;
					if (nfichs && !(nfichs%100))
					{
						fichs = (char **)realloc((void *)fichs,sizeof(char *)*(nfichs+110));
					}
				}
				libera_dir(dir);
			}
		}
		else
		{
			fichs[nfichs] = malloc(strlen(tablas[n])+1);
			strcpy(fichs[nfichs],tablas[n]);
			nfichs++;
			fichs[nfichs] = NULL;
			if (nfichs && !(nfichs%100))
			{
				fichs = (char **)realloc((void *)fichs,sizeof(char *)*(nfichs+110));
			}
		}
	}
}

int mete_en_base()
{
	int file,n,nn,fd;	
	
	expande_tablas();

	for (n = 0;n < nfichs;n++)
	{
		if (coincide_(fichs[n],"*.dsb"))
		{
			continue;
		}
		file = open(fichs[n],O_RDONLY|O_BINARY);
		if (file >= 0)
		{
			strcpy(tmp0,basename);
			strcat(tmp0,"/");
			strcat(tmp0,quita_path(fichs[n]));
			fd = dsdb_u_abre(tmp0,O_RDWR|O_BINARY,SH_DENYNO);
			if (fd >= 0)
			{
				if (comando == 2 && force)
				{
					dsdb_base_vaciatabla(base_fd,NULL,NULL,fd);
				}				
				if (!force || comando == 1)
				{
					dsdb_u_cierra(fd);
					printf("El fichero ya existe [%s]\r\n",fichs[n]);					
					fd = -1;
					close(file);
					continue;
				}
			}
			else
			{
				fd = dsdb_u_abre(tmp0,O_RDWR|O_BINARY|O_CREAT,SH_DENYNO,S_IREAD|S_IWRITE);
			}
			if (fd >= 0)
			{
				if (verbose)
				{
					printf("%s\r\n",fichs[n]);
				}
				while((nn = read(file,tmp1,sizeof(tmp1))) > 0)
				{
					dsdb_u_graba(fd,tmp1,nn);
				}
				dsdb_u_cierra(fd);
			}
			else
			{
				printf("El fichero no se puede crear [%s]\r\n",fichs[n]);
			}			
			close(file);
		}
		else
		{
			printf("El fichero no existe [%s]\r\n",fichs[n]);
		}
	}
    libera_dir(fichs);
	fichs = NULL;
	nfichs = 0;
	return 0;
}

int duplica(DBDirEntry *pdirentry)
{
	int file = -1,fd,n;

	strcpy(tmp0,basename2);
	strcat(tmp0,"/");
	strcat(tmp0,pdirentry->name);
	if (!force)		
	{	
		file = dsdb_u_abre(tmp0,O_RDWR|O_BINARY,SH_DENYNO);
	}
	if (file < 0)
	{
		file = dsdb_u_abre(tmp0,O_RDWR|O_BINARY|O_CREAT|O_TRUNC,S_IREAD|S_IWRITE);
		if (file < 0)
		{
			printf("El fichero no se puede crear [%s]\r\n",pdirentry->name);
			return 0;
		}
		strcpy(tmp0,basename);
		strcat(tmp0,"/");
		strcat(tmp0,pdirentry->name);
		fd = dsdb_u_abre(tmp0,O_RDWR|O_BINARY,SH_DENYNO);
		if (fd < 0)
		{
			printf("El fichero no se puede leer [%s]\r\n",pdirentry->name);
		}
		else
		{
			if (verbose)
			{
				printf("%s\r\n",pdirentry->name);
			}
			while((n = dsdb_u_lee(fd,tmp1,sizeof(tmp1))) > 0)
			{
				if (dsdb_u_graba(file,tmp1,n) != n)
				{
					printf("Error al grabar en %s\r\n",tmp0);
					break;
				}
			}
			dsdb_u_cierra(fd);
		}
	}	
	else
	{
		printf("El fichero ya existe [%s]\r\n",pdirentry->name);
	}
	dsdb_u_cierra(file);
	return 0;
}


void quitab(s)
char *s;
{
	register char *r;
	
	for (r=s;(*r);r++);
	while (r > s) {
		r--;
		if (*r != ' ' && *r != 9) 
		{
			*(r+1) = 0;
			break;
		}
		if (r == s)
			*r = 0;
	}
	
}

void quitan(s)
char *s;
{
	register char *r;
	
	for (r=s;(*r);r++);
	while (r > s) {
		r--;
		if (*r != 10 && *r != 13) 
		{
			*(r+1) = 0;
			break;
		}
		if (r == s)
			*r = 0;
	}
	
}

static int carga_emprecopias(char *path)
{
	FILE *file;
	int i;	
	char tmp[512];	


	if (lista_emprecopias)
	{
		for (i = 0;lista_emprecopias[i];i++)
		{
			free(lista_emprecopias[i]);
		}		
		free(lista_emprecopias);
	}
	lista_emprecopias = NULL;
	
	file = fopen(path,"r");
	if (file)
	{
		if (fgets(tmp,510,file))
		{
			quitan(tmp);
			quitab(tmp);
			ip_2key = inet_addr(tmp);
			lista_emprecopias = malloc(sizeof(char *) * 502);

			i = 0;
			while(fgets(tmp,510,file) && i < 500)
			{
				quitan(tmp);
				quitab(tmp);
				lista_emprecopias[i] = malloc(strlen(tmp)+1);
				strcpy(lista_emprecopias[i],tmp);
				i++;
			}
			lista_emprecopias[i] = NULL;
			lista_emprecopias[i+1] = NULL;

		}
		fclose(file);
	}
	return 0;
}



static BOOL rwi_senddata(SOCKET sock,char  *sendbuf,int totalbytes)
{
		 int bytessent;
         int i,err;			
         unsigned char controla;
		 long tiempo;
		 int pj;

		 if (totalbytes == -1)
		 {
			 totalbytes = strlen(sendbuf);
		 }
         
         do 
         {
            bytessent = 0;			  
            while(bytessent < totalbytes)
            {
               i = send(sock,(char *)sendbuf + bytessent,totalbytes - bytessent,0);
               if (i == SOCKET_ERROR)
               {
#ifdef FGVDOS
                  if (WSAGetLastError() != WSAEWOULDBLOCK)
                  {
					 Sleep(50);
                     return FALSE;
                  }
				  Sleep(10);				  
#else
                  if (errno != EWOULDBLOCK)
                  {
					 usleep(50000);
                     return FALSE;
                  }
				  usleep(10000);
#endif
				  
                  continue;
               }
               else
                  bytessent += i;
            }
           
            return TRUE;
            
         } while(-1);
         return FALSE;
}


static BOOL rwi_recvdata(SOCKET sock,char  *pr,int totalbytes)
{
   int totalbytesread = 0,bytesread,readsize = totalbytes;   
   while(totalbytesread < totalbytes)
   {
      if((bytesread = recv(sock,(char *)pr + totalbytesread,readsize,0)) == SOCKET_ERROR)
      {
#ifdef FGVDOS
         if (WSAGetLastError() != WSAEWOULDBLOCK)
         {													  
		    return FALSE;
         }
		 Sleep(10);
#else
         if (errno != EWOULDBLOCK)
         {
            return FALSE;
         }
		 usleep(10000);
#endif		 
         continue;
      }
      totalbytesread += bytesread;
      readsize -= bytesread;
   }        
   return(TRUE);
} 

int strminus(s)
char *s;
{
   while (*s != '\0') 
   {
      if (*s > 64 && *s < 91)
         *s += 32;
	  else
	  {
		  if (*s == 128)
			  *s = 135;
		  else if (*s == 165)
			  *s = 164;
	  }
      s++;
   }

   return(0);
}


char fechas[75];
char *fecha()
{
			time_t a; 
         char *p;
         struct tm *lc;
         int ann;
         
         p = fechas;
         a = time(NULL);
         if (a == -1L) {
            memset(fechas,0,sizeof(fechas));
            return(p);
         }
         
         /* ------
         * seg.transcurridos  *b
         * segundos           localtime(b)->tm_sec
         * minutos            localtime(b)->tm_min 
         * hora               localtime(b)->tm_hour 
         * dia del mes        localtime(b)->tm_mday 
         * mes                (localtime(b)->tm_mon)+1 
         * ano -1900          localtime(b)->tm_year
         * dia de la semana   localtime(b)->tm_wday 
         * dia del ano        localtime(b)->tm_yday 
         * activado DST       localtime(b)->tm_isdst 
         */ 
         lc = localtime(&a);
         if (!lc) 
         {
            memset(fechas,0,sizeof(fechas));
            return(p);
         }
         
         ann = lc->tm_year;
         if (ann > 50)
            ann += 1900;
         else
            ann += 2000;
         
         sprintf(p,"%02d%c%02d%c%04d%c",
            lc->tm_mday ,caracter_de_fecha,
            (lc->tm_mon)+1 ,caracter_de_fecha,
            ann,
            '\0');
         sprintf(p+11,"%02d:%02d:%02d%c",
            lc->tm_hour ,
            lc->tm_min  ,
            lc->tm_sec  ,
            '\0');
         
         return(p);
}

static int negocia_dsdb(unsigned char *path)
{
    SOCKET sock;
    struct sockaddr_in addr;
#ifdef FGVDOS
       WORD      wVersionRequested;
       WSADATA   wsaData;
         
       wVersionRequested = MAKEWORD(1,1);
       if (WSAStartup(wVersionRequested, &wsaData) != 0)
       {	 
          return -1;
       }

#endif


    ip_2key = 0L;
    memset(copia_key,0,sizeof(copia_key));
    carga_emprecopias(path);
	if (ip_2key)
	{
	   int recibido;

	   recibido = 0;
	   sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	   if (sock != SOCKET_ERROR)
	   {
		   FastSocket(sock);
		   addr.sin_family = AF_INET;
		   addr.sin_port = htons(22276);
		   addr.sin_addr.s_addr = ip_2key;
		   if (connect(sock,(struct sockaddr *) &addr,sizeof (addr)) != SOCKET_ERROR)
		   {										   
			   if (rwi_senddata(sock,"cqea0abc.",10) != FALSE)
			   {
				   long cuanto;
				   int n;

				   n = 0;
				   while(n < 100)
				   {
					   cuanto = 0;
					   if (ioctlsocket(sock,FIONREAD,&cuanto) == SOCKET_ERROR)
					   {
						   n = 100;
						   break;
					   }
					   if (cuanto >= 248)
						   break;
#ifdef FGVDOS
					   Sleep(100);
#else
					   usleep(100000);
#endif
					   n++;
				   }         
				   if (n < 100)
				   {											   
					   if (rwi_recvdata(sock,copia_key,248) == FALSE)
					   {
						   memset(copia_key,0,sizeof(copia_key));
					   }
					   else
					   {
						   for (n = 0;n < 248;n++)
						   {
							   if (copia_key[n])
								   break;
						   }
						   if (n < 248)
								recibido = 1;
					   }
				   }
			   }
		   }
		   closesocket(sock);
	   }
	   if (!recibido)
	   {
#ifdef FGVDOS
			WSACleanup();
#endif

		   aviso(0,"Falta módulo de protección de datos (mochila).");
		   return 0;
	   }
	}
#ifdef FGVDOS
    WSACleanup();
#endif
   return 1;
}

int main(int argc,char *argv[])
{
	char *pars;
	char *file = NULL;	
	int i,ppi;
	unsigned char pp[256];

	printf("dsdb, administrador basico de base de datos, version 2.0 .\r\n\r\n");
	dsdb_sin_errores = 1;

	if (argc < 2)
	{
		uso();
		pars = "t";
	}
	else
	{
		/* c,a,x or t */
		pars = argv[1];
		if (argc > 2)
		{
			ntablas = argc - 2;
			tablas = &(argv[2]);
		}
	}
	while (*pars)
	{
		switch(*pars)
		{
		case 't':
			comando = 0;
			break;
		case 'c':
			comando = 1;
			break;
		case 'u':
			comando = 2;
			break;
		case 'x':
			comando = 3;
			break;
		case 'd':
			comando = 4;
			break;
		case 'R':
			comando = 5;
			break;
		case 'O':
			comando = 6;
			break;
		case 'D':
			comando = 7;
			break;
		case 'v':
			verbose = 1;
			break;
		case 'f':
			if (argc > 2)
			{
				file = argv[2];
				if (argc > 3)
				{
					ntablas = argc - 3;
					tablas = &(argv[3]);
				}
				else
				{
					ntablas = 0;
					tablas = NULL;
				}
				break;
			}
		case 'p':
			dsdb_usa_crypto = 1;
			pars++;
			if (*pars != '[')
			{
				uso();
				exit(2);
			}
			pars++;
			ppi = 0;
			while (*pars && *pars != ']' && ppi < 16)
			{
				pp[ppi] = *pars;
				ppi++;
				pars++;
			}
			if (*pars != ']')
			{
				uso();
				exit(2);
			}
			pp[ppi] = 0;

		    ppi = strlen(pp);
		    while(ppi < 16)
			{
			   pp[ppi] = 32+ppi;
			   ppi++;
			}
		    for (ppi = 0;ppi < 16;ppi++)
			{
			   pp[ppi] ^= 0xAA;
			}
			memcpy(dsdb_,pp,16);
			break;
		case 'm':			
			pars++;
			if (*pars != '[')
			{
				uso();
				exit(2);
			}
			pars++;
			ppi = 0;
			while (*pars && *pars != ']' && ppi < 200)
			{
				pp[ppi] = *pars;
				ppi++;
				pars++;
			}
			if (*pars != ']')
			{
				uso();
				exit(2);
			}
			pp[ppi] = 0;
			negocia_dsdb(pp);
			break;
		case 's':			
			pars++;
			if (*pars != '[')
			{
				uso();
				exit(2);
			}
			pars++;
			ppi = 0;
			while (*pars && *pars != ']' && ppi < 16)
			{
				pp[ppi] = *pars;
				ppi++;
				pars++;
			}
			if (*pars != ']')
			{
				uso();
				exit(2);
			}
			pp[ppi] = 0;

			strcpy(__dsdb_serverip,pp);
			dsdb_serverip = __dsdb_serverip;

			break;
		default:
			uso();
			exit(2);
		}
		pars++;
	}	
	if (comando < 0)
	{
		uso();
		exit(2);
	}

    if (!file || strbusca(file,"*") > -1)
	{
		static char _file[MAX_PATH];
		char **dir;
		char *filtro;

		if (!file)
		{
			filtro = "*.dsb";
		}
		else
		{
			filtro = file;
			file = NULL;
		}

		dir = lee_direct_interno(filtro,0);
		if (dir)
		{
			if (dir[0])
			{
				strcpy(_file,filtro);
				strcpy(quita_path(_file),dir[0]);
				file = _file;
			}
			libera_dir(dir);
		}
		if (!file)
		{
			printf("especifique el parametro f fichero.\r\n");
			exit(2);
		}
	}

	while(ntablas && *(tablas[0]) == '-')
	{
		pars = tablas[0] + 1;
		i = strlen(pars);
		if (!strcmp(pars,"force"))
		{
			force = 1;
		} 
		else if (i > 6 && !memcmp(pars,"block=",6))
		{
			rw_dsdb_tambloque = atoi(pars+6);
		}
		else if (i > 5 && !memcmp(pars,"size=",5))
		{
			rw_dsdb_bloquesdefecto = atoi(pars+5);
		}
		else if (i > 8 && !memcmp(pars,"factori=",8))
		{
			rw_dsdb_bloquesaumento = atoi(pars+8);
		}
		else
		{
			uso();
			exit(2);
		}
		tablas++;
		ntablas--;
	}

	basename = file;
	if (comando == 1)
	{
		if (force)
		{
			if (unlink(basename) < 0 && errno != ENOENT)
			{
				printf("La base de datos [%s] no se puede borrar\r\n.",basename);
				exit(2);
			}
		}
		else
		{
			FILE *tmp_file;
			tmp_file = fopen(basename,"r");
			if (tmp_file)
			{				
				fclose(tmp_file);
				printf("La base de datos [%s] existe, use -force si es necesario.\r\n",basename);
				exit(2);
			}
		}
	}
	if (comando != 5)
	{
		base_fd = dsdb_base_abre(file);
		if (base_fd < 0)
		{
			printf("No se puede abrir la base de datos %s Pruebe opcion 'R'\r\n",file);
			exit(2);
		}
	}

	total = (ds_off)0;

	switch(comando)
	{
	case 0:
		i = dsdb_lee_dir(base_fd,NULL,pintadir);
		printf("\t\t%6d contenedores %.0lf bytes",i,(double)total);
		break;
	case 1:
	case 2:
		mete_en_base();
		break;
	case 3:
		dsdb_lee_dir(base_fd,NULL,sacadir);
		break;
	case 4:
		if (ntablas < 1)
		{
			printf("Debe especificar una base de datos destino\r\n");
		}
		else
		{
			expande_tablas();
			for (i = 0;i < nfichs;i++)
			{
				base_fd2 = dsdb_base_abre(fichs[i]);
				if (base_fd2 < 0)
				{
					printf("No se puede abrir o crear la base de datos destino %s.\r\n",fichs[i]);
				}
				else
				{
					basename2 = fichs[i];
					dsdb_lee_dir(base_fd,NULL,duplica);
					dsdb_base_cierra(base_fd2);
				}
			}
		    libera_dir(fichs);
			fichs = NULL;
			nfichs = 0;
		}
		break;
	case 5:
		if (DSDB_ReconstruyeLimpiamente(file,1+verbose) < 0)
		{
			printf("No se ha completado la recuperacion de %s\r\n",file);
			if (!force)
			{
				printf("Use -force para intentar forzar la extraccion de cualquier dato.\r\n");
			}
			else
			{
				/* extraer bloques de datos a pelo */
				printf("No se han extraido bloques de datos desligados.\r\n");
			}
		}
		else
		{
			printf("Base de datos OK\r\n");
		}
		break;
	case 6:
		printf("En esta version de base de datos no es aplicable la desfragmentacion avanzada, use la duplicacion para desfragmentacion standard.\r\n");
		break;
	}

	if (base_fd > -1)
	{
		dsdb_base_cierra(base_fd);
	}

	return 0;
}

