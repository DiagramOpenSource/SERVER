
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

#ifndef O_BINARY  /* compatibilidad xenix - dos */
#define O_BINARY 0
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#ifndef LARGESEEK
extern long rw_lseek_();
#endif
extern unsigned int rw_read_();
#endif

#include <string.h>
#ifdef FGVDOS
#include <conio.h>
#include <ctype.h>
#endif
#include <errno.h>
#include <malloc.h>
#include <memory.h>
#include <signal.h>
#include <time.h>
#include <stdio.h>



void *asigna_memoria(void *p1,int tam)
{
if (!p1)
   p1 = malloc(tam);
else
   p1 = realloc(p1,tam);
return(p1);
}

char *CargaFichero(int file,int tipo)
{
long pini;
long pfin;
long tama;
char *buf;
/* el tipo prc luego */
/* comparamos todo menos informacion aleatoria y checksums (nos fiamos mas de comparar a pelo) */

pini = 0L;
pfin = rw_lseek_(file,0L,SEEK_END);
tama = pfin - pini;
rw_lseek_(file,pini,SEEK_SET);

if (tipo == FGVGEN_ID_MAS)
   {
   tama -= 20L;
   tama += sizeof(short);   
   }
else
if (tipo == FGVGEN_ID_PRC)
   {
   tama -= 26L;
   tama += sizeof(short);   
   }

buf = malloc(tama+sizeof(long));

pini += sizeof(long);

if (tipo == FGVGEN_ID_MAS)
   {
   if (read(file,buf+pini,sizeof(short)) != sizeof(short))   
      {
      fprintf(stderr,"Error de lectura\n");
      free(buf);
      return(NULL);
	  }   
   pini += sizeof(short);
   rw_lseek_(file,20L,SEEK_SET);
   }
else
if (tipo == FGVGEN_ID_PRC)
   {
   if (read(file,buf+pini,sizeof(short)) != sizeof(short))   
      {
      fprintf(stderr,"Error de lectura\n");
      free(buf);
      return(NULL);
	  }   
   pini += sizeof(short);
   rw_lseek_(file,26L,SEEK_SET);
   }

if (read(file,buf+pini,tama-pini+sizeof(long)) != (int)(tama-pini+sizeof(long)))
   {
   fprintf(stderr,"Error de lectura\n");
   free(buf);
   return(NULL);
   }
memcpy(buf,&tama,sizeof(long));
return(buf);
}

int ComparaMiFichero(char *dirmaster,char *dirtrabajo,char *fichero,int tipo)
{
int f1,f2;
long s1,s2;
char tmp1[1024];
char *mf1,*mf2;
int r = 0;

sprintf(tmp1,"%s%s",dirmaster,fichero);
f1 = rw_open_(tmp1,O_BINARY | O_RDONLY);
if (f1 < 0)
   {   
   return(1); /* si no se puede con el master es que hay que actualizar (para caso de comparacion con el update) */
   }

sprintf(tmp1,"%s%s",dirtrabajo,fichero);
f2 = rw_open_(tmp1,O_BINARY | O_RDONLY);
if (f2 < 0)
   {
   rw_close_(f1);   
   /* si no esta en el trabajo (caso de comparacion con el update) no se incluye */
   return(0);
   }

mf1 = CargaFichero(f1,tipo);
mf2 = CargaFichero(f2,tipo);
rw_close_(f1);
rw_close_(f2);

if (!mf1 || !mf2)
   {
   fprintf(stderr,"Imposible comparar, problema de memoria!\n");   
   if (mf1) free(mf1);
   if (mf2) free(mf2);
   return(0);
   }
memcpy(&s1,mf1,sizeof(long));
memcpy(&s2,mf2,sizeof(long));
if (s1 != s2)
   r = 1;
else
if (memcmp(mf1+sizeof(long),mf2+sizeof(long),s1))
   {
#ifdef _DEBUG
   for (r = sizeof(long);r < (sizeof(long)+s1);r++)
      {
	  if (mf1[r] != mf2[r])
	     {
		 r = 1;
		 break;
		 }
	  }
#endif
   r = 1;
   }

if (mf1) free(mf1);
if (mf2) free(mf2);
return(r);
}


char **compara_lista(char *aplicacion,char *dirmaster,char *dirtrabajo,char *dirupdate,char **efectivo,int tipo)
{
char **resultado = NULL;
char **lista_master = NULL;
char **lista_trabajo = NULL;
char **p0;
int total = 0,punto = 0;
char dir1[1024];
char dir2[1024];
char dir3[1024];
char tmp1[1024];
char subdir[20];
char extension[20];
int total_efectivo = 0;
int _efectivos = 0;

if (efectivo)
  {
  char **p = efectivo;
  while(*p) 
  {
      _efectivos++;
      p++;
  }
  total_efectivo = ( (_efectivos / 100) * 100) + 100;
  }

switch(tipo)
  {
  case FGVGEN_ID_MAS:
     strcpy(subdir,"def/");  
	 strcpy(extension,".mas");
	 break;
  case 1:
     strcpy(subdir,"vent/");  
	 strcpy(extension,".mas");
	 tipo = FGVGEN_ID_MAS;
     break;
  case FGVGEN_ID_PRC:
     strcpy(subdir,"bin/");  
	 strcpy(extension,".prc");
     break;
  case 0:
     strcpy(subdir,"pan/");
	 strcpy(extension,".p*");
	 break;
  case 2:
     strcpy(subdir,"def/");
	 strcpy(extension,".inf");
	 break;
  default:
     fprintf(stderr,"Formato desconocido\n");
	 return efectivo;
  }
  
sprintf(dir1,"%s%s/%s",dirmaster,aplicacion,subdir);
sprintf(tmp1,"%s*%s",dir1,extension);
lista_master = lee_direct(tmp1,0);

sprintf(dir2,"%s%s/%s",dirtrabajo,aplicacion,subdir);
sprintf(tmp1,"%s*%s",dir2,extension);
lista_trabajo = lee_direct(tmp1,0);

sprintf(dir3,"%s%s/%s",dirupdate,aplicacion,subdir);

if (!lista_trabajo)
   {   
   libera_dir(lista_master);
   return(efectivo);
   }


for (p0 = lista_trabajo;*p0;p0++)
   {
   char **p1 = NULL;
   if (lista_master)
      {
      for (p1 = lista_master;*p1;p1++)
        {
        if (!strcmp(*p0,*p1))
	      {
		  break;
		  }
	    }
	  }
   if (!p1 || !*p1 || ComparaMiFichero(dir1,dir2,*p0,tipo) || ComparaMiFichero(dir1,dir3,*p0,tipo))
      { /* si el fichero del master y el update son diferentes se incluyen en el resultado */
      if (punto >= (total-1))
	     {
		 total += 100;
		 resultado = asigna_memoria(resultado,sizeof(char *)*total);
		 }
	  resultado[punto] = malloc(strlen(*p0)+1);
	  strcpy(resultado[punto],*p0);
	  punto++;
	  resultado[punto] = NULL;
	  }
   }
libera_dir(lista_trabajo);
libera_dir(lista_master);


if (resultado)
   {
   char tmpx[1024];
   char tmpy[1024];
   char **p = resultado;   
   while(*p)
      {	  
      sprintf(tmpx,"%s%s/%s",dirupdate,aplicacion,subdir);
	  sprintf(tmpy,"%s%s/%s",dirtrabajo,aplicacion,subdir);
      if (ComparaMiFichero(tmpx,tmpy,*p,tipo))
	     {         
	     sprintf(tmpy,"%s%s/%s%s",dirtrabajo,aplicacion,subdir,*p);
		 sprintf(tmpx,"%s%s/%s%s",dirupdate,aplicacion,subdir,*p);
         copiarfich(tmpy,tmpx,0);
		 if (total_efectivo < ( ((_efectivos / 100) * 100) + 100) )
		 {			 
			total_efectivo = ( (_efectivos / 100) * 100) + 100;
		    efectivo = (char **)asigna_memoria((void *)efectivo,sizeof(char *)*total_efectivo);
		 }
		 efectivo[_efectivos] = malloc(strlen(tmpx)+1);
		 strcpy(efectivo[_efectivos],tmpx);
		 _efectivos++;
		 efectivo[_efectivos] = NULL;
		 }
	  p++;
	  }     
   }

libera_dir(resultado);

return(efectivo);
}

static void saca_control(FILE *f,char *DirMaster,char *DirUpdate,char *Aplicacion,char *subdir,int tipo)
{
char tmp[1024];
char **mdir,**udir;
char **p1,**p2;
char extension[10];
long posicion;

switch(tipo)
  {
  case FGVGEN_ID_MAS:     
	 strcpy(extension,".mas");
	 posicion= 16L;
	 break;
  case FGVGEN_ID_PRC:     
	 strcpy(extension,".prc");
	 posicion= 22L;
     break;
  default:
     fprintf(stderr,"Formato desconocido\n");
	 return;
  }

   sprintf(tmp,"%s%s/%s*%s",DirMaster,Aplicacion,subdir,extension);
   mdir= lee_direct(tmp,0);
   sprintf(tmp,"%s%s/%s*%s",DirUpdate,Aplicacion,subdir,extension);
   udir= lee_direct(tmp,0);


   for (p1 = mdir;p1 && *p1;p1++)
      {
	  for (p2 = udir;p2 && *p2;p2++)
	     {
		 if (!strcmp(*p1,*p2)) break;
		 }
	  if (!p2 || !*p2)
	     {
		 int file;
         unsigned short n1 = 0;
		 unsigned short n2 = 0;

         sprintf(tmp,"%s%s/%s%s",DirMaster,Aplicacion,subdir,*p1);
		 file = rw_open_(tmp,O_BINARY | O_RDONLY);
		 if (file >= 0)
		    {
			rw_lseek_(file,posicion,SEEK_SET);
            rw_read_(file,&n1,sizeof(short));
			rw_read_(file,&n2,sizeof(short));
			rw_close_(file);
			fprintf(f,"%04X-%04X %s/%s%s\n",n1,n2,Aplicacion,subdir,*p1);
			}
		 }
	  }

   libera_dir(mdir);
   libera_dir(udir);
return;
}  

/* dsupdate agicont /u/maestros/ /u/trabajo/ /u/updates/ */

main(argc,argv)
int argc;
char *argv[];
{
char **efectivo = NULL;
char Aplicacion[100];
char DirMaster[512];
char DirTrabajo[512];
char DirUpdate[512];

if (argc < 5)
{
   fprintf(stderr,"dsupdate aplicacion dirmaestros dirtrabajo dirupdate\n");
   exit(2);
}

strcpy(Aplicacion,argv[1]);
strcpy(DirMaster,argv[2]);
strcpy(DirTrabajo,argv[3]);
strcpy(DirUpdate,argv[4]);

  {
  char tmpx[1024];

  sprintf(tmpx,"%s%s/",DirUpdate,Aplicacion);
  crea_dir(tmpx);
  sprintf(tmpx,"%s%s/def/",DirUpdate,Aplicacion);
  crea_dir(tmpx);
  sprintf(tmpx,"%s%s/vent/",DirUpdate,Aplicacion);
  crea_dir(tmpx);
  sprintf(tmpx,"%s%s/pan/",DirUpdate,Aplicacion);
  crea_dir(tmpx);
  sprintf(tmpx,"%s%s/bin/",DirUpdate,Aplicacion);
  crea_dir(tmpx);
  }

/* def */
efectivo = compara_lista(Aplicacion,DirMaster,DirTrabajo,DirUpdate,efectivo,FGVGEN_ID_MAS);

/* vent */
efectivo = compara_lista(Aplicacion,DirMaster,DirTrabajo,DirUpdate,efectivo,1);

/* pan */
efectivo = compara_lista(Aplicacion,DirMaster,DirTrabajo,DirUpdate,efectivo,0);

/* inf */
efectivo = compara_lista(Aplicacion,DirMaster,DirTrabajo,DirUpdate,efectivo,2);

/* bin */
efectivo = compara_lista(Aplicacion,DirMaster,DirTrabajo,DirUpdate,efectivo,FGVGEN_ID_PRC);

if (efectivo)
   {
   FILE *f;

     {
     char tmp[1024];
     sprintf(tmp,"%s%s.lcu",DirUpdate,Aplicacion);
     f = fopen(tmp,"w");
     }

   if (f)
     {
     /* crear lista control */
     saca_control(f,DirMaster,DirUpdate,Aplicacion,"def/",FGVGEN_ID_MAS);
     saca_control(f,DirMaster,DirUpdate,Aplicacion,"vent/",FGVGEN_ID_MAS);
     saca_control(f,DirMaster,DirUpdate,Aplicacion,"bin/",FGVGEN_ID_PRC);
	 fclose(f);
	 }


   /*printf("Nuevo Update!\n");*/
   {
     char **p = efectivo;
     /*printf("Lista de nuevas inclusiones al update:\n");*/
     while(*p)
       {
	   printf("%s\n",*p);
	   p++;
	   }
   }

   }

libera_dir(efectivo);

return(0);
}