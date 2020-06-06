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
extern long fgv_serial_seek_();
#endif
extern unsigned int fgv_serial_read_();
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
#include <rdef.h>


extern int (*fgv_serial_open_)( const char *filename, int oflag , int pmode );
extern long (*fgv_serial_seek_)( int handle, long offset, int origin );
extern int (*fgv_serial_read_)(int handle, void *buffer, unsigned int count );
extern int (*fgv_serial_write_)(int handle, const void *buffer, unsigned int count);
extern int (*fgv_serial_close_)( int handle );
extern int (*fgv_serial_lock_)( int handle, int mode, long nbytes );


int saca_infodir_deldef(char *eldef,char *titulo,char *mi_modulo)
{
int file;
int datos = 0;
short xx;
short claves;
short pantallas;
short campos;
short relaciones;
titulo[0] = 0;
mi_modulo[0] = 0;

file = fgv_serial_open_(eldef,O_BINARY | O_RDWR,0);
if (file >= 0)
  {  
  fgv_serial_seek_(file,20L,SEEK_SET);
  xx = 0;
  fgv_serial_read_(file,&xx,sizeof(short));
  if (xx)
     {
	 char tmpxx[26];
	 memset(tmpxx,0,26);
	 fgv_serial_seek_(file,16L,SEEK_SET);
     fgv_serial_write_(file,tmpxx,26); 
	 }
  fgv_serial_seek_(file,16L,SEEK_SET);
  xx = 0;
  fgv_serial_read_(file,&xx,sizeof(short));
  if (!xx)
     {
	 fgv_serial_read_(file,&xx,sizeof(short));
	 }
  if (!xx)
     {
     char *pi;
     long tam1;
     char *total;
     unsigned short checksum = 0;
     unsigned short suma = 0;
     /* grabar el check-sum */
     tam1 = fgv_serial_seek_(file,0L,SEEK_END) - 40L;
     total = malloc(tam1);
     memset(total,0,tam1);
     fgv_serial_seek_(file,42L,SEEK_SET);
     fgv_serial_read_(file,total,tam1-2);
     for (pi = total;pi < (total+tam1-1);pi += sizeof(short))
       {
 	   checksum ^= *((unsigned short *)pi);
	   suma += *((unsigned short *)pi);
	   }
     free(total);
     fgv_serial_seek_(file,16L,SEEK_SET);
     fgv_serial_write_(file,&suma,sizeof(short));
     fgv_serial_write_(file,&checksum,sizeof(short));
	 }
  
  fgv_serial_seek_(file,33L,SEEK_SET);
  fgv_serial_read_(file,mi_modulo,9);
  fgv_serial_seek_(file,42L,SEEK_SET);
  xx = 0;
  fgv_serial_read_(file,&xx,sizeof(short));
  if (xx == FGVGEN_ID_MAS)
     {
	 xx = 0;
  	 fgv_serial_read_(file,(char *)&xx,sizeof(short));
	 if (xx)
	   {
	   datos |= 0x1;
	   }
	 xx = 0;
	 fgv_serial_read_(file,(char *)&xx,sizeof(short));
	 if (xx)
	   {
	   datos |= 0x2;
	   }
	 }
  else
     {
	 datos |= 0x8;
	 }
  pantallas = 0;
  claves = 0;
  campos = 0;
  relaciones = 0;
  fgv_serial_seek_(file,49L + O_MAXPATH + (sizeof(char *) * 6) + (sizeof(short) * 2),SEEK_SET);
  fgv_serial_read_(file,(char *)&pantallas,sizeof(short));
  fgv_serial_read_(file,(char *)&claves,sizeof(short));
  fgv_serial_read_(file,(char *)&campos,sizeof(short));
  fgv_serial_read_(file,(char *)&relaciones,sizeof(short));
  if (claves)
     {
     datos |= 0x4;
	 }
  {
	int tamcampo = (sizeof(char *) * 8) + (sizeof(short) * 12);
	int tamclave = (sizeof(char *) * 2) + (sizeof(short) * 2);
	int tamrelaci = (sizeof(char *) * 3) + (sizeof(short) * 2);
	short ncam;
	int j;
	long offset,off2;

    offset = 49L + O_MAXPATH + (sizeof(char *) * 6) + (sizeof(short) * 7) + (sizeof(PAN_DEF) * pantallas) + (tamcampo * campos);    
	off2 = 0;
    for (j = 0;j < claves;j++)
	   {
       fgv_serial_seek_(file,offset+(sizeof(char *)*2),SEEK_SET);
	   ncam = 0;
	   fgv_serial_read_(file,(char *)&ncam,sizeof(short));
       offset += tamclave;
	   off2 += (ncam * 2 * sizeof(short));
	   }
	offset += (off2 + (relaciones * tamrelaci));
	fgv_serial_seek_(file,offset,SEEK_SET);
	fgv_serial_read_(file,titulo,129);
  }

  fgv_serial_close_(file);	 
  }
else
  datos |= 0x8;

return(datos);
}
