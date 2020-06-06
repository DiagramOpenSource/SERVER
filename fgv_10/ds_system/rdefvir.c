

/* Autor: Francisco Gonzalez Valiente */
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

extern void *rwmalloc();

#ifndef O_BINARY  /* compatibilidad xenix - dos */

#define O_BINARY 0
#define SEEK_SET 0
#define SEEK_CUR 1
#ifndef LARGESEEK
extern long rw_lseek_();
#endif
extern unsigned int rw_read_();

#endif

#include <rtermin.h>
#define _NUEVODEF
#include <rdef.h>
#include <rmante.h>
#include <rcalcul.h>

extern int rwdebug;

extern int flag_leedefn;
extern int dentro_de_leeaplic;

extern int (*libera_virtual)();

extern int no_prog_virtual = 0;

extern int recur_punto = -1;
extern int recur_nfi[MAXRECUR] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
extern void *recur_mpfi[MAXRECUR] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};

int ultimo_ipfi = -1;
static unsigned long cuando_cont = 0L;

struct salva_def
{
   char *nombre_fichero;
   char *enlaza_pfi;
   char *salvadatos;
   char *buf;
   short fd;
   unsigned long cuando;
   short quememo;
   int lock;
   int tam_buf;
} s_d[MAXDEF*MAXRECUR]; 

struct enlaza_prc *salva_procs[MAXRECUR];
short nsalva_procs[MAXRECUR];

static FI_DEF *cargame_el_def(int ipfi,int recur)
{
   int i;
   char *tmp2;
   flag_leedefn = 1;
   if (lee_defn(s_d[recur].nombre_fichero,&mpfi[ipfi]))
   {
       char tmp[512];
       int file_t;
       static FI_DEF nulidad;
       extern char dirbase[MAXPATH];


       strcpy(tmp,dirbase);
       strcat(tmp,"vent/");
       strcat(tmp,quita_path(s_d[recur].nombre_fichero));
       strcat(tmp,".adi");
       file_t = rw_open_(tmp,O_RDONLY | O_BINARY);
       if (file_t >= 0)
       {
          rw_close_(file_t);
          memset(&nulidad,0,sizeof(nulidad));
          return &nulidad;
       }
	   return NULL;
   }
   
   if (!s_d[recur].salvadatos) /* no inicializado */
   {
	  if (s_d[recur].buf && s_d[recur].tam_buf != mpfi[ipfi]->f_lfich)
	  {
		  free(s_d[recur].buf);
		  s_d[recur].buf = rwmalloc(mpfi[ipfi]->f_lfich+1);
		  s_d[recur].tam_buf = mpfi[ipfi]->f_lfich;
	  }
      if (!s_d[recur].buf)
      {
         mpfi[ipfi]->f_buf = rwmalloc(mpfi[ipfi]->f_lfich+1);		 
      }
      else
         mpfi[ipfi]->f_buf = s_d[recur].buf;
      inicializa_def(ipfi);
      da_memoria_salva_datos(mpfi[ipfi],recur);
      return(mpfi[ipfi]);
   }
   
   tmp2 = s_d[recur].salvadatos;
   for (i = 0;i < mpfi[ipfi]->f_ncampos;i++)
   {
      memcpy(&(mpfi[ipfi]->f_campos[i].c_mante.c_actual),tmp2,12);
      tmp2 += 12;
   }
   strcpy(mpfi[ipfi]->f_fichero,tmp2);
   tmp2 += (strlen(mpfi[ipfi]->f_fichero) + 1);
   for (i = 0;i < mpfi[ipfi]->f_npanta;i++)
   {
      strcpy(mpfi[ipfi]->f_pantalla[i].p_nombrepan,tmp2);
      tmp2 += (strlen(mpfi[ipfi]->f_pantalla[i].p_nombrepan) + 1);
   }
   mpfi[ipfi]->f_buf = s_d[recur].buf;
   mpfi[ipfi]->f_fd = s_d[recur].fd;

   return mpfi[ipfi];
}

int v_existe_def(int ipfi)
{
	int recur = (recur_punto < 1) ? 0 : recur_punto * MAXDEF;

	if (ipfi == 48) 
	{
		if (mpfi[ipfi])
			return 1;
		return 0;		
	}
	recur += ipfi;

	ultimo_ipfi = ipfi;
	if (mpfi[ipfi] || cargame_el_def(ipfi,recur))
		return 1;
	return 0;
}

extern int tracer_int;
FI_DEF *v_mpfi(ipfi)
int ipfi;
{   
   int recur = (recur_punto < 1) ? 0 : recur_punto * MAXDEF;   
   FI_DEF *pfir = NULL;
   
   if (ipfi < 0 || (ipfi >= nfi && ipfi < 48))
   {
      printf(__Texto[293],ipfi,nfi);getchar();
   }
   
   if (ipfi == 48) {
	   if (mpfi[ipfi] == NULL) {
		   return mpfi[0];
	   }
	   return(mpfi[ipfi]);
   }
  
   
   if (ipfi > (MAXDEF+5))
   {
      printf(__Texto[294],ipfi);getchar();
   }
   
   recur += ipfi;
   if (ipfi < MAXDEF) {cuando_cont++;s_d[recur].cuando = cuando_cont;}
   ultimo_ipfi = ipfi;
   if (mpfi[ipfi]) {
	   /*
	   if (strstr(mpfi[ipfi]->f_fichero,"pcegm004")) {
		   char *buf;
		   buf = mpfi[ipfi]->f_buf + mpfi[ipfi]->f_campos[35].c_offset;
		   if (memcmp(buf,"   ",3) && memcmp(buf,"EUR",3)) {
			   tracer_int = 1;
		   }
	   }
	   */
	   return(mpfi[ipfi]);
   }
   
   if (ipfi >= nfi)
   {
      printf(__Texto[295],ipfi);getchar();
   }

   if (!(pfir = cargame_el_def(ipfi,recur)))
   {
      if (stdpan)
      {
         aviso(0,s_d[recur].nombre_fichero);
         Terfinal();
         exit(2);
      }
      else
         printf(__Texto[296],s_d[recur].nombre_fichero);getchar();
      exit(2);
   }   
   if (!mpfi[ipfi]) {
       tracer_int = 1;
	   return pfir;
   }
   
   /*
   if (strstr(mpfi[ipfi]->f_fichero,"pcegm004")) {
	   char *buf;
	   buf = mpfi[ipfi]->f_buf + mpfi[ipfi]->f_campos[35].c_offset;
	   if (memcmp(buf,"   ",3) && memcmp(buf,"EUR",3)) {
		   tracer_int = 1;
	   }
   }
   */
   return(mpfi[ipfi]);
}

int _salva_datos_lock(int ipfi)
{
    int recur = (recur_punto < 1) ? 0 : recur_punto * MAXDEF;
    recur += ipfi;
    s_d[recur].lock = 1;
    return(recur);
}

int _salva_datos_unlock(int ipfi)
{
    int recur = (recur_punto < 1) ? 0 : recur_punto * MAXDEF;
    recur += ipfi;
    s_d[recur].lock = 0;
    return(recur);
}

int da_memoria_salva_datos(fi,ipfi)
FI_DEF *fi;
int ipfi;
{
   int quememo,i;
   quememo = 12 * fi->f_ncampos + strlen( fi->f_fichero ) + 1;
   for (i = 0;i < fi->f_npanta;i++)
      quememo += (strlen(fi->f_pantalla[i].p_nombrepan) + 1);
   s_d[ipfi].quememo = quememo;
   s_d[ipfi].salvadatos = rwmalloc(quememo);   
}

static int virtualiza_def(fi,ipfi,tam)
FI_DEF *fi;
int ipfi;
unsigned int tam;
{
			int quememo,i;
         char *tmp,*tmp2;
#ifndef ESDOS
         char memo_stack[8000]; /* solucion para unix */
#endif
         
         if (s_d[ipfi].lock) return -1;

         quememo = 12 * fi->f_ncampos + strlen( fi->f_fichero ) + 1;
         for (i = 0;i < fi->f_npanta;i++)
            quememo += (strlen(fi->f_pantalla[i].p_nombrepan) + 1);
         
         if (s_d[ipfi].quememo < quememo)
         {
            rwfree(s_d[ipfi].salvadatos);
            s_d[ipfi].salvadatos = NULL;
            printf(__Texto[297],s_d[ipfi].quememo,quememo);getchar();
         }
         
         if (s_d[ipfi].salvadatos)
         {
            tmp = s_d[ipfi].salvadatos;
         }
         else
         {
#ifndef ESDOS
            tmp = memo_stack;      
#else      	
            tmp = alloca(quememo); /* el stack es el que puede petar */
            if (!tmp) {
               printf(__Texto[298],quememo);
               return(-1);
            }
#endif      
         }
         
         tmp2 = tmp;
         for (i = 0;i < fi->f_ncampos;i++)
         {
            memcpy(tmp2,&(fi->f_campos[i].c_mante.c_actual),12);
            tmp2 += 12;
         }
         strcpy(tmp2,fi->f_fichero);
         tmp2 += (strlen(fi->f_fichero) + 1);
         for (i = 0;i < fi->f_npanta;i++)
         {
            strcpy(tmp2,fi->f_pantalla[i].p_nombrepan);
            tmp2 += (strlen(fi->f_pantalla[i].p_nombrepan) + 1);
         }
         s_d[ipfi].buf = fi->f_buf;
         s_d[ipfi].fd = fi->f_fd;
		 s_d[ipfi].tam_buf = fi->f_lfich;
         fi->f_buf = NULL;
         free_defpri(fi);
         if (!s_d[ipfi].salvadatos)
         {
            s_d[ipfi].salvadatos = rwmalloc(quememo);
            s_d[ipfi].quememo = quememo;
            memcpy(s_d[ipfi].salvadatos,tmp,quememo);
         }
         return(0);
}

static int lib_prg(tmp_pr,nj,no)
struct enlaza_prc *tmp_pr;
int nj,no;
{
   int j = -1;
   unsigned long uso = 0xffffffffL;
   int i;
   
   for (i = 0;i < nj;i++)
   {
      if (i != no && tmp_pr[i].activo)
						{
         if (uso > tmp_pr[i].e_usado)
         {
            uso = tmp_pr[i].e_usado;
            j = i;
         }
						}
   }
   
   if (j == -1)
      return(-1);
   
   if (tmp_pr[j].e_prog)
      reserva_free((char *)tmp_pr[j].e_prog,6);
   if (tmp_pr[j].e_oper)
      reserva_free((char *)tmp_pr[j].e_oper,9);
   if (tmp_pr[j].e_cond)
      reserva_free((char *)tmp_pr[j].e_cond,7);
   if (tmp_pr[j].e_bucl)
      reserva_free((char *)tmp_pr[j].e_bucl,8);
   if (tmp_pr[j].e_f_B)
      reserva_free((char *)tmp_pr[j].e_f_B,10);
   if (tmp_pr[j].tabla_externa)
      reserva_free((char *)tmp_pr[j].tabla_externa,11);
   if (tmp_pr[j].tabla_ficheros)
      reserva_free((char *)tmp_pr[j].tabla_ficheros,12);
   
   tmp_pr[j].e_prog = NULL;
   tmp_pr[j].e_oper = NULL;
   tmp_pr[j].e_cond = NULL;
   tmp_pr[j].e_bucl = NULL;
   tmp_pr[j].e_f_B = NULL;
   tmp_pr[j].tabla_externa = NULL;
   tmp_pr[j].tabla_ficheros = NULL;
   tmp_pr[j].activo = 0;
}

static int recur_control = 0;

int saca_virtual(tam)
unsigned int tam;
{
   int i,j,ipfidiff = -1;
   unsigned long diff,maxdiff = 0L;
   int recur = (recur_punto < 1) ? 0 : recur_punto * MAXDEF;
   FI_DEF **fi;
   struct enlaza_prc *tmp_pr;
   
   if (recur_control) {printf(__Texto[299]);getchar();return(-1);}
   recur_control = 1;
   
   /* primero recurridos */
   
   /* !! sistema a saco !! */
   for (i = 0;i < recur_punto;i++)
   {
						tmp_pr = salva_procs[i];
                  while(!lib_prg(tmp_pr,nsalva_procs[i],-1)) ipfidiff = 0; /** a saco **/
                  fi = (FI_DEF **)(recur_mpfi[i]);
                  for (j = recur_nfi[i] - 1;j >= 0;j--)
                  {
                     if ( fi[j] && j < 48)
                     {
                        if ( !virtualiza_def(fi[j],i*MAXDEF+j,tam) )
                        {
                           fi[j] = NULL;
                           ipfidiff = 0; /** a saco **/
                        }
                     }
                  }
   }
   
   for (i = 0;(i < nfi && i < 48);i++)
   {
						if (mpfi[i] && i != ultimo_ipfi)
                  {
                     if (!virtualiza_def(mpfi[i],i+recur,tam)) /** a saco **/
                     {
                        mpfi[i] = NULL;
                        ipfidiff = i;
                     }
                  }
   }
   
   /** a saco **/
   while(!lib_prg(en_pr,nen_pr,programa_activo)) ipfidiff = 0;
   /** a saco **/
   if (ipfidiff != -1)
   {      /** a saco **/
						recur_control = 0;
                  return(0);
   }
   if (rwdebug)
   {
						printf(__Texto[300]);
                  printf(__Texto[301],tam);getchar();
   }
   recur_control = 0;
   return(-1);
}


