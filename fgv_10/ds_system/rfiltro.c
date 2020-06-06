

/* Autor: Francisco Gonzalez Valiente */
#include <fgvsys.h>

#ifdef FGVWIN
#include <windows.h>
#include <windowsx.h>
#endif

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef FGVDOS
#include <io.h>
#endif

#ifndef O_BINARY 
#define O_BINARY 0
#define SEEK_SET 0
#define SEEK_CUR 1
#ifndef LARGESEEK
extern long rw_lseek_();
#endif
extern unsigned int rw_read_();
#endif

#include <string.h>
#ifdef FGVDOS
#include <conio.h>
#include <ctype.h>
#include <direct.h>
#else
#ifdef ESXENIX
#include <sys/ndir.h>
#else
#include <dirent.h>
#endif
#endif
#include <errno.h>
#include <malloc.h>
#include <memory.h>
#include <signal.h>
#include <time.h>
#include <stdio.h>
#ifdef FGVWIN
#include <fgvwin.h>
#endif

#ifdef FGVDOS

/*extern BOOL fIsTNT;*/

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
      /*strlwr(dr->d_name);*/
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
            /*strlwr(dr->d_name);*/
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

static int filtro(mandato,c)
char *mandato;
char c;
{
   int i;
   for (i=0;mandato[i] != '\0' && mandato[i] != c;i++) {
      if ( mandato[i] == '/')
         mandato[i] = '\\';
   }
   return(0);
}
int filtroma(mandato)
char *mandato;
{
   return(filtro(mandato,' '));
}

int filtropar(mandato)
char *mandato;
{
   filtro(mandato,'\0');
   return(0);
}

#else

/* filtroma
* funcion que en MS-DOS cambia los slash por back-slash.
*/

int filtroma(mandato)
char *mandato;
{
   return(0);
}

/* filtropar
*  funcion que en MS-DOS sustituye a los systems
*/

int filtropar(mandato)
char *mandato;
{
   return(0);
}

#endif


int copiarfich(_o,_d,modo)
char *_o,*_d;
int modo;
{
   unsigned char *buf;
   int fo,fd;
   FILE *fileo,*filed;
   unsigned int nread;
   
   fo = rw_open_(_o,O_RDONLY | O_BINARY);
   if (fo < 0) return(-1);
   if (!modo) 
      fd = rw_open_(_d,O_RDWR | O_BINARY | O_CREAT | O_TRUNC,S_IWRITE | S_IREAD);
   else
      if (modo > 1)
         fd = rw_open_(_d,O_RDWR | O_BINARY | O_CREAT,S_IWRITE | S_IREAD);
      else
         fd = rw_open_(_d,O_RDWR | O_BINARY | O_CREAT | O_APPEND,S_IWRITE | S_IREAD);
      if (fd < 0) {rw_close_(fo);return(-1);}
      fileo = fdopen(fo,"rb");
      if (!fileo) {rw_close_(fo);rw_close_(fd);return(-1);}
      if (!modo)
         filed = fdopen(fd,"wb");
      else
         if (modo > 1)
												filed = fdopen(fd,"w+b");
         else
												filed = fdopen(fd,"ab");
         if (!filed) {fclose(fileo);rw_close_(fd);return(-1);}
         
         if (modo > 1)
         {
												modo--;  
                                    fseek( filed, (long)modo,SEEK_SET);
         }
         
         buf = malloc(30000);
         
         while( (nread = fread(buf,1,30000,fileo)) > 0 )
         {
              if (fwrite(buf,1,nread,filed) < nread)
                                    {
                                       fclose(fileo);
                                       fclose(filed);
                                       unlink(_d);
                                       free(buf);
                                       return(-1);
                                    }
         }
         
         fclose(fileo);
         fclose(filed);
         free(buf);
         return(0);
}

int crea_dir(d) /* crea todos los directorios entendiendo el final como fichero */
char *d;
{
   char *o;
   char tmp[2048];
   int e = 0;
   strcpy(tmp,d);
   o = tmp;
#ifdef FGVDOS
   for(d = o;*d;d++)
   {
      if (*d == '/') *d = '\\';
   }
   d = o;
   while ( (d = strchr(d,'\\')) != NULL) {
      *d = '\0';
      e = mkdir(o);
      *d = '\\';
      d++;
   }
   
#else
   for(d = o;*d;d++)
   {
      if (*d == '\\') *d = '/';
   }      
   d = o;
   
   while ( (d = strchr(d,'/')) != NULL) {
      *d = '\0';
      e = mkdir(o,0xffff);
      *d = '/';
      d++;
   }
#endif
   return(e);
}


int crea_dir_total(d) /* crea todos los directorios entendiendo el final como otro directorio */
char *d;
{
   char *o;
   char tmp[2048];
   int e = 0;
   strcpy(tmp,d);
   o = tmp;
#ifdef FGVDOS
   for(d = o;*d;d++)
   {
      if (*d == '/') *d = '\\';
   }
   if (!(*d) && d > o)
   {
      d--;
      if (*d != '\\')
      {
         d++;
         *d = '\\';
         d++;
         *d = 0;
      }
   }
   d = o;
   while ( (d = strchr(d,'\\')) != NULL) {
      *d = '\0';
      e = mkdir(o);
      *d = '\\';
      d++;
   }
   
#else
   for(d = o;*d;d++)
   {
      if (*d == '\\') *d = '/';
   }   
   if (!(*d) && d > o)
   {
      d--;
      if (*d != '/')
      {
         d++;
         *d = '/';
         d++;
         *d = 0;
      }
   }
   
   d = o;
   
   while ( (d = strchr(d,'/')) != NULL) {
      *d = '\0';
      e = mkdir(o,0xffff);
      *d = '/';
      d++;
   }
#endif
   return(e);
}

void cambia_barra(s)
char *s;
{
   while(*s)
   {
      if (*s == '\\') *s = '/';
      s++;
   }
}


void cambia_barra_dos(s)
char *s;
{
   while(*s)
   {
      if (*s == '/') *s = '\\';
      s++;
   }
}

int coincide_(buf,filtro)
char *buf;
char *filtro;
{
   int i;
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
            if (strbusca(p2,p1) < 0)
            {
               return(0);
            }
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


int mid_atoi(char *pp,int lon)
{
char tmp[256];
if (lon > 255) return 0;
memcpy(tmp,pp,lon);
tmp[lon] = 0;
return(atoi(tmp));
}

long lee_direct_tot_tama = 0L;
char **lee_direct(char *pdir,int flags)
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

   lee_direct_tot_tama = 0L;
   
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
#ifdef FGVDOS
            if ((b_stat.st_mode & _S_IFDIR)) continue; /* es directorio */
#else
            if ((b_stat.st_mode & 0x4000)) continue; /* es directorio */
#endif
         }
         else
            if (flags == 2)
            {
#ifdef FGVDOS
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
#ifdef FGVDOS
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
			lee_direct_tot_tama += b_stat.st_size;
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


#ifndef _S_IFDIR
#define _S_IFDIR 0x4000
#endif

int _lee_y_borra(char *_dir,char *hasta,char *desde)
{
   char dir[256];
   int op;
#ifdef ESXENIX
   struct direct *buf;
#else
   struct dirent *buf;
#endif
   DIR *dd;
   char path[256];
   struct stat b_stat;
   char filtro[81];
   
   
   strcpy(dir,_dir);
   op = strlen(dir);
   if (op) {
      op--;
      if (dir[op] == '/' || dir[op] == '\\')
         strcat(dir,"*");
   }
   strcpy(filtro,quita_path(dir));
   *(quita_path(dir)) = 0;
   op = strlen(dir);
   if (op) {
      op--;
      if (dir[op] == '/' || dir[op] == '\\')
         dir[op] = 0;
   }
   
   if ((dd = opendir(dir)))
   {
      while(-1) {
         buf = readdir(dd);
         if (!buf) break;
         if (coincide_(buf->d_name,filtro))
         {
            if (!hasta || (strcmp(hasta,buf->d_name) >= 0 && strcmp(desde,buf->d_name) <= 0))
            {
               strcpy(path,dir);
               strcat(path,"/");
               strcat(path,buf->d_name);
               stat(path,&b_stat); /* obtengo todos los datos refentes al fichero */
			   if (!hasta && desde)
			   {
				   char tmpdate[30];
				   sprintf(tmpdate,"%04d%02d%02d",localtime(&b_stat.st_mtime)->tm_year+1900,localtime(&b_stat.st_mtime)->tm_mon+1,localtime(&b_stat.st_mtime)->tm_mday);
				   if (strcmp(desde,tmpdate) < 0)
				   {
					   continue;
				   }
			   }
               if ((b_stat.st_mode & _S_IFDIR))
               {
                  strcat(path,"/*");
                  _lee_y_borra(path,hasta,desde);
                  strcpy(path,dir);
                  strcat(path,"/");
                  strcat(path,buf->d_name);
                  rmdir(path);
               }
               else
               {
                  unlink(path);
               }
            }
         }
      }
      closedir(dd);
   }
   
   return(0);
}


int lee_y_borra(char *_dir)
{
   return(_lee_y_borra(_dir,NULL,NULL));
}


/* ----------------------------------------------------------------------------
*   coma_linea
*      en llamadas sucesivas a esta funcion se dan los elementos que estan
*      en un string separados por comas:
*      si 't' == 0 : busca lo que hay antes de la primera coma
*             != 0 : busca lo que hay entre dos comas
*      devuelve el indice a partir de donde esta lo que busca en texto
*      y al final de lo que busca en texto pone '\0'.
*      si no hay mas 'texto'+'jj' == '\0'
* ----------------------------------------------------------------------------
*/



#ifdef FGVDOS
int share_open(path,acceso,mode)
char *path;
int acceso;
unsigned int mode;
{
			return(sopen(path,acceso,0x40,mode));
}
#endif


#ifdef FGVBSD

static struct flock bsd_flock;
static int ini_flock()
{
   bsd_flock.l_start = (off_t)0;
   bsd_flock.l_pid = 0;
   bsd_flock.l_whence = SEEK_CUR;
   return(0);
}

int fgv_lockf(int f,int modo,long len)
{
   int cmd;
   static int primero_bsd = 1;
   if (primero_bsd)
   {
      ini_flock();
      primero_bsd = 0;
   }
   
   bsd_flock.l_len = (off_t)len;
   if (modo == 0)
   {
      cmd = F_SETLK;
      bsd_flock.l_type = F_UNLCK;  
   }
   else
      if (modo == 1)
						{
         cmd = F_SETLKW;
         bsd_flock.l_type = F_WRLCK;  
						}
      else
         if (modo == 2 || modo == 20)
         {
            cmd = F_SETLK;
            bsd_flock.l_type = F_WRLCK;  
         }
         return(fcntl(f,cmd,(int)&bsd_flock));
}

#endif

#ifdef SIN_CIERRA_TODO
int cierra_todo(a)
int a;
{
   return(0);
}
#endif


#ifdef FGVDOS
#ifndef FGVWIN
HWND GetConsoleHwnd(void)
{
#define MY_BUFSIZE 1024 /* buffer size for console window titles    */
   char pszNewWindowTitle[MY_BUFSIZE]; // contains fabricated WindowTitle
   char pszOldWindowTitle[MY_BUFSIZE]; // contains original WindowTitle
   static HWND hwndFound = NULL;         // this is what is returned to the caller
   // fetch current window title
   
   /*
   if (fIsTNT == TRUE)
   {
   return(NULL);
   }
   */
   
   
   
   if (hwndFound) return(hwndFound);
   
   GetConsoleTitle(pszOldWindowTitle, MY_BUFSIZE);
   
   // format a "unique" NewWindowTitle
   
   wsprintf(pszNewWindowTitle,"%d/%d",
						GetTickCount(),
                  GetCurrentProcessId());
   
   // change current window title
   
   SetConsoleTitle(pszNewWindowTitle);
   
   
   Sleep(1);
   
   
   {
						int espera;
                  for (espera = 0;espera < 21;espera++)
                  {
                     hwndFound=FindWindow(NULL, pszNewWindowTitle);
                     if (hwndFound) break;
                     Sleep(10);
                  }
   }
   
   // restore original window title
   
   SetConsoleTitle(pszOldWindowTitle);
   
   return(hwndFound);
}
#endif

int copiafich_win_(_o,_d,modo)
char *_o,*_d;
int modo;
{
   SHFILEOPSTRUCT shellop;
   char tmp_o[512];
   char tmp_d[512];
   char *p;
   
   
   p = tmp_o;
   if (*_o != 0 && *(_o+1) != ':' && (*_o != '/' || *_o != '\\') && (*(_o+1) != '/' || *(_o+1) != '\\'))
   {
						GetCurrentDirectory(sizeof(tmp_o),tmp_o);
                  if (*_o != '/' && *_o != '\\')
                  {
                     strcat(tmp_o,"\\");
                     p = tmp_o + strlen(tmp_o);
                  }
                  else
                     if (tmp_o[0] == '\\')
                     {
                        p = tmp_o + 1;
                     }
                     else  
                     {	    
                        p = tmp_o + 2;
                     }
   }
   while(*_o)
   {
						if (*_o == '/')
                     *p = '\\';
                  else
                     *p = *_o;
                  p++;
                  _o++;
   }
   *p = 0;
   *++p = 0;
   p = tmp_d;
   if (*_d != 0 && *(_d+1) != ':' && (*_d != '/' || *_d != '\\') && (*(_d+1) != '/' || *(_d+1) != '\\'))
   {
						GetCurrentDirectory(sizeof(tmp_d),tmp_d);
                  if (*_d != '/' && *_d != '\\')
                  {
                     strcat(tmp_d,"\\");
                     p = tmp_d + strlen(tmp_d);
                  }
                  else
                     if (tmp_d[0] == '\\')
                     {
                        p = tmp_d + 1;
                     }
                     else  
                     {	    
                        p = tmp_d + 2;
                     }
   }
   while(*_d)
   {
						if (*_d == '/')
                     *p = '\\';
                  else
                     *p = *_d;
                  p++;
                  _d++;
   }
   *p = 0;
   *++p = 0;

	if (modo)
	{
		FILE *ff;
		ff = fopen(tmp_d,"r");
		if (ff)
		{
			fclose(ff);
			return -1;
		}		
	}


   memset(&shellop,0,sizeof(shellop));
#ifdef FGVWIN
   shellop.hwnd = principal_w;
#else
   shellop.hwnd = GetConsoleHwnd();
#endif
   shellop.wFunc = FO_COPY;
   shellop.pFrom = tmp_o;
   shellop.pTo = tmp_d;
   shellop.fFlags = FOF_FILESONLY | FOF_NOCONFIRMATION | FOF_NOERRORUI/* | FOF_NOCOPYSECURITYATTRIBS*/;
   shellop.lpszProgressTitle = "Copiando";
   do {
						shellop.fAnyOperationsAborted = FALSE;
                  if (!SHFileOperation(    &shellop	 )) return(0);   
   } while(shellop.fAnyOperationsAborted == TRUE);
   /*if (CopyFile(_o,_d,FALSE)) return(0);*/
   return(-1);
}

int copiafich_win(_o,_d)
char *_o,*_d;
{
	return copiafich_win_(_o,_d,0);
}


#endif
