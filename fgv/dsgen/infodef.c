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
#include <rdef.h>


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

file = rw_open_(eldef,O_BINARY | O_RDWR);
if (file >= 0)
  {  
  rw_lseek_(file,20L,SEEK_SET);
  xx = 0;
  rw_read_(file,&xx,sizeof(short));
  if (xx)
     {
	 char tmpxx[26];
	 memset(tmpxx,0,26);
	 rw_lseek_(file,16L,SEEK_SET);
     rw_write_(file,tmpxx,26); 
	 }
  rw_lseek_(file,16L,SEEK_SET);
  xx = 0;
  rw_read_(file,&xx,sizeof(short));
  if (!xx)
     {
	 rw_read_(file,&xx,sizeof(short));
	 }
  if (!xx)
     {
     char *pi;
     long tam1;
     char *total;
     unsigned short checksum = 0;
     unsigned short suma = 0;
     /* grabar el check-sum */
     tam1 = rw_lseek_(file,0L,SEEK_END) - 40L;
     total = malloc(tam1);
     memset(total,0,tam1);
     rw_lseek_(file,42L,SEEK_SET);
     rw_read_(file,total,tam1-2);
     for (pi = total;pi < (total+tam1-1);pi += sizeof(short))
       {
 	   checksum ^= *((unsigned short *)pi);
	   suma += *((unsigned short *)pi);
	   }
     free(total);
     rw_lseek_(file,16L,SEEK_SET);
     rw_write_(file,&suma,sizeof(short));
     rw_write_(file,&checksum,sizeof(short));
	 }
  
  rw_lseek_(file,33L,SEEK_SET);
  rw_read_(file,mi_modulo,9);
  rw_lseek_(file,42L,SEEK_SET);
  xx = 0;
  rw_read_(file,&xx,sizeof(short));
  if (xx == FGVGEN_ID_MAS)
     {
	 xx = 0;
  	 rw_read_(file,(char *)&xx,sizeof(short));
	 if (xx)
	   {
	   datos |= 0x1;
	   }
	 xx = 0;
	 rw_read_(file,(char *)&xx,sizeof(short));
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
  rw_lseek_(file,49L + MAXPATH + (sizeof(char *) * 6) + (sizeof(short) * 2),SEEK_SET);
  rw_read_(file,(char *)&pantallas,sizeof(short));
  rw_read_(file,(char *)&claves,sizeof(short));
  rw_read_(file,(char *)&campos,sizeof(short));
  rw_read_(file,(char *)&relaciones,sizeof(short));
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

    offset = 49L + MAXPATH + (sizeof(char *) * 6) + (sizeof(short) * 7) + (sizeof(PAN_DEF) * pantallas) + (tamcampo * campos);    
	off2 = 0;
    for (j = 0;j < claves;j++)
	   {
       rw_lseek_(file,offset+(sizeof(char *)*2),SEEK_SET);
	   ncam = 0;
	   rw_read_(file,(char *)&ncam,sizeof(short));
       offset += tamclave;
	   off2 += (ncam * 2 * sizeof(short));
	   }
	offset += (off2 + (relaciones * tamrelaci));
	rw_lseek_(file,offset,SEEK_SET);
	rw_read_(file,titulo,129);
  }

  rw_close_(file);	 
  }
else
  datos |= 0x8;
return(datos);
}
