
/* Autor: Francisco Gonzalez Valiente */
#include <fgvsys.h>

#include <stdio.h>
#include <malloc.h>
#include <rwmalloc.h>
#include <memory.h>

#include <rtermin.h>
#define _NUEVODEF
#include <rdef.h>      /* ya incluye el isam.h */

extern char dirdef[MAXPATH]; /* viene del agisys : esta rutina se usa con agisys o similares !! */

extern char *quita_path();
extern char *logger_salida;

#ifndef ESDOS
#ifdef ES_DISAM
extern int iSfd; /* handle of actual isam file in use */
#endif
#endif

extern int tracer_int;
extern int logger_int;

static int en_logger = 0;

static char *salida1 = "logger.spo";
#ifdef ESDOS
static char *salida2 = "PRN";
#else
static char *salida2 = "/dev/prn";
#endif
static char *salida = NULL;

extern char rw_user[11];
extern char *fecha();

static void logger_identifica(FILE *f)
{
	fprintf(f,"%-10.10s %s %s\r\n",rw_user,fecha(),fecha()+11);
}

extern void pinta_en_logger(s)
unsigned char *s;
{
   FILE *f;
   
   if (logger_salida)
   {
	   if (*logger_salida)
	   {
		   salida = logger_salida;
		   en_logger = 1;
	   }
	   else
	   {
		   en_logger = 0;
	   }
   }   
   else if (logger_int) 
   {
      if (logger_int == 1)
         salida = salida1;
      else
         if (logger_int == 2)
            salida = salida2;
         if (logger_int == en_logger)
            en_logger = 0;
         else
            en_logger = logger_int;
         logger_int = 0;
   }
   
   if (!en_logger) return;
   
   crea_dir(salida);
   f = fopen(salida,"a");
   if (!f) {pito(45);return;}
   logger_identifica(f);
   fprintf(f,"%s\n",s);
   fclose(f);
   return;
}

static void logger_actualiza(pfi,pfo)
FI_DEF *pfi;
FI_DEF *pfo;
{
   FI_DEF *pft;
   char buf[MAXDEFECTO];
   char buf2[MAXDEFECTO];
   int i,k,j;
   double rr;
   FILE *f = fopen(salida,"a");
   if (!f) {pito(46);return;}
   

   crea_dir(salida);

   logger_identifica(f);
   
   strcpy(buf,dirdef);
   strcat(buf,quita_path(pfi->f_fichero));
   pft = NULL;
   if (lee_defn(buf,&pft)) 
   {
      if (pft)
      {
         free((char *)pft);
      }
      pft = NULL;
   }
   
   fprintf(f,__Texto[311],pfi->f_fichero);
   fprintf(f,__Texto[312]);
   for (i = 0;i < pfi->f_clave[0].k_ncampos;i++) {
      ld_campo(pfi->f_clave[0].k_campo[i]-1,pfi,buf);
      fprintf(f,"%s ",buf);
			}
   fprintf(f,"\n");
   
   /* buscar diferencias en la actualizacion */
   k = 0;
   for(i = 0;i < pfi->f_ncampos;i++) {
      if (campocmp(i,1,pfi,pfi->f_buf,pfo->f_buf,0)) {
         k++;
         buf[0] = 0;
         if (pft) 
            strcpy(buf,pft->f_campos[i].c_nombre);
         fprintf(f,__Texto[313],i,buf);
         ld_campo(i,pfi,buf2);
         ld_campo(i,pfo,buf);
         fprintf(f,"[%s] -> [%s]",buf,buf2);
         j = pfi->f_campos[i].c_tipo;
         if (j != CHARTIPO && j != FECHATIPO) {
            rr = rwatof(buf2) - rwatof(buf);
            sprintf(buf,"%lf",rr);
            quitadec(buf);
            fprintf(f," : [%s]",buf);
         }
         fprintf(f,".\n");
      }
			}
   if (!k)
      fprintf(f,__Texto[314]);
   else
      fprintf(f,__Texto[315],k);
   fclose(f);
   if (pft)
      free_defpri(pft);
}

static void logger_nueva(pfi)
FI_DEF *pfi;
{
			char buf[MAXDEFECTO];
         int i;
         FILE *f = fopen(salida,"a");
         if (!f) {pito(47);return;}

		 crea_dir(salida);
		 logger_identifica(f);        

         fprintf(f,__Texto[316],pfi->f_fichero);
         fprintf(f,__Texto[317]);
         for (i = 0;i < pfi->f_clave[0].k_ncampos;i++) {
            ld_campo(pfi->f_clave[0].k_campo[i]-1,pfi,buf);
            fprintf(f,"%s ",buf);
         }
         fprintf(f,"\n");
         fprintf(f,"********************************************************\n");
         
         fclose(f);
}


static void logger_baja(pfi)
FI_DEF *pfi;
{
			char buf[MAXDEFECTO];
         int i;
         FILE *f = fopen(salida,"a");
         if (!f) {pito(48);return;}

		 crea_dir(salida);
		 logger_identifica(f);
         
         fprintf(f,"Baja ficha %s.\n",pfi->f_fichero);
         fprintf(f,__Texto[317]);
         for (i = 0;i < pfi->f_clave[0].k_ncampos;i++) {
            ld_campo(pfi->f_clave[0].k_campo[i]-1,pfi,buf);
            fprintf(f,"%s ",buf);
         }
         fprintf(f,"\n");
         fprintf(f,"********************************************************\n");
         
         fclose(f);
}

static void pinta_clave_primaria(pfi)
FI_DEF *pfi;
{
   int i;
   char buf[MAXDEFECTO];
   
   for (i = 0;i < pfi->f_clave[0].k_ncampos;i++) {
      ld_campo(pfi->f_clave[0].k_campo[i]-1,pfi,buf);
      v_print(buf);
      v_print(" ");
   }
   
}

int t_graba_nuevo(pfi)
register FI_DEF *pfi;
{
   int e;
   
   
   if (logger_salida)
   {
	   if (*logger_salida)
	   {
		   salida = logger_salida;
		   en_logger = 1;
	   }
	   else
	   {
		   en_logger = 0;
	   }
   }
   else if (logger_int) 
   {
						if (logger_int == 1)
                     salida = salida1;
                  else
                     if (logger_int == 2)
                        salida = salida2;
                     if (logger_int == en_logger)
                        en_logger = 0;
                     else
                        en_logger = logger_int;
                     logger_int = 0;
   }
   
   
   e = graba_nuevo(pfi);
   
   if (en_logger) {
						if (!e) {
                     Bl(1);
                     pprint(110,__Texto[318]);
                     v_print(pfi->f_fichero);
                     Bl(2);
                     pprint(201,__Texto[319]);
                     pinta_clave_primaria(pfi);
                     logger_nueva(pfi);
                  }
                  else {
                     pito(49);
                     Bl(1);
                     pprint(110,__Texto[320]);
                     v_print(pfi->f_fichero);
                     Bl(2);
                     pprint(201,__Texto[321]);
                     pinta_clave_primaria(pfi);
                  }
   }
   
   return(e);
}

int t_graba_actual(pfi)
register FI_DEF *pfi;
{
   FI_DEF pfi2;
   int e;
   int i;
   int b = 0;
   int fd = pfi->f_fd;
   
   pfi2 = *pfi;
   
   if (logger_salida)
   {
	   if (*logger_salida)
	   {
		   salida = logger_salida;
		   en_logger = 1;
	   }
	   else
	   {
		   en_logger = 0;
	   }
   }
   else if (logger_int) 
   {
      if (logger_int == 1)
         salida = salida1;
      else
         if (logger_int == 2)
            salida = salida2;
         if (logger_int == en_logger)
            en_logger = 0;
         else
            en_logger = logger_int;
         logger_int = 0;
   }
   
   pfi2.f_buf = malloc(pfi2.f_lfich+1);
   if (!lee_reg(&pfi2,ISCURR)) {
      /* comprobar si el registro esta bloqueado */
#ifndef ESDOS
#ifdef ES_DISAM
      /* de momento en espera de solucionar con btrieve */
      iSfd = fd;
      b = iSLdcheck( isrecnum );
      if (!b) {
         Bl(24);
         pito(50);
         pprint(2401,__Texto[322]);
         v_print(pfi->f_fichero);
         /*  tracer_int = 1; */
      }
#endif
#endif
   }
   
   
   e = graba_actual(pfi);
   
   if (en_logger) {
      if (!e) {
         Bl(1);
         pprint(110,__Texto[323]);
         v_print(pfi->f_fichero);
         Bl(2);
         pprint(201,__Texto[324]);
         pinta_clave_primaria(pfi);
         logger_actualiza(pfi,&pfi2);
      }
      else {
         pito(51);
         Bl(1);
         pprint(110,__Texto[325]);
         v_print(pfi->f_fichero);
         Bl(2);
         pprint(201,__Texto[326]);
         pinta_clave_primaria(pfi);
      }
   }
   free(pfi2.f_buf);
   return(e);
}

int t_borra_actual(pfi)
register FI_DEF *pfi;
{
   FI_DEF pfi2;
   int e;
   int i;
   int b = 0;
   int fd = pfi->f_fd;
   
   pfi2 = *pfi;
   
   if (logger_salida)
   {
	   if (*logger_salida)
	   {
		   salida = logger_salida;
		   en_logger = 1;
	   }
	   else
	   {
		   en_logger = 0;
	   }
   }
   else if (logger_int) 
   {
      if (logger_int == 1)
         salida = salida1;
      else
         if (logger_int == 2)
            salida = salida2;
         if (logger_int == en_logger)
            en_logger = 0;
         else
            en_logger = logger_int;
         logger_int = 0;
   }
   
   pfi2.f_buf = malloc(pfi2.f_lfich+1);
   if (!lee_reg(&pfi2,ISCURR)) {
      /* comprobar si el registro esta bloqueado */
#ifndef ESDOS
#ifdef ES_DISAM
      /* de momento en espera de solucionar con btrieve */
      iSfd = fd;
      b = iSLdcheck( isrecnum );
      if (!b) {
         Bl(24);
         pito(52);
         pprint(2401,__Texto[322]);
         v_print(pfi->f_fichero);
         /*  tracer_int = 1; */
      }
#endif
#endif
   }
   
   
   e = borra_actual(pfi);
   
   if (en_logger) {
      if (!e) {
         Bl(1);
         pprint(110,__Texto[323]);
         v_print(pfi->f_fichero);
         Bl(2);
         pprint(201,__Texto[324]);
         pinta_clave_primaria(&pfi2);
         logger_baja(&pfi2);
      }
      else {
         pito(53);
         Bl(1);
         pprint(110,__Texto[325]);
         v_print(pfi->f_fichero);
         Bl(2);
         pprint(201,__Texto[326]);
         pinta_clave_primaria(&pfi);
      }
   }
   free(pfi2.f_buf);
   return(e);
}

int t_borra_reg(pfi)
register FI_DEF *pfi;
{
   int e;
   
   
   if (logger_salida)
   {
	   if (*logger_salida)
	   {
		   salida = logger_salida;
		   en_logger = 1;
	   }
	   else
	   {
		   en_logger = 0;
	   }
   }
   else if (logger_int) 
   {
						if (logger_int == 1)
                     salida = salida1;
                  else
                     if (logger_int == 2)
                        salida = salida2;
                     if (logger_int == en_logger)
                        en_logger = 0;
                     else
                        en_logger = logger_int;
                     logger_int = 0;
   }
   
   
   e = borra_reg(pfi);
   
   if (en_logger) {
						if (!e) {
                     Bl(1);
                     pprint(110,__Texto[318]);
                     v_print(pfi->f_fichero);
                     Bl(2);
                     pprint(201,__Texto[319]);
                     pinta_clave_primaria(pfi);
                     logger_baja(pfi);
                  }
                  else {
                     pito(54);
                     Bl(1);
                     pprint(110,__Texto[320]);
                     v_print(pfi->f_fichero);
                     Bl(2);
                     pprint(201,__Texto[321]);
                     pinta_clave_primaria(pfi);
                  }
   }
   
   return(e);
}


