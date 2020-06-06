
/**********************************************************************/
/**********************************************************************/
/*                     DIAGRAM9                                   */
/**********************************************************************/
/**********************************************************************/

#include <fgvsys.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef O_BINARY
#include <io.h>
#endif

#ifndef O_BINARY  /* compatibilidad xenix - dos */

#define O_BINARY 0
#define SEEK_SET 0
#define SEEK_CUR 1
/*
#ifndef LARGESEEK
extern long rw_base_lseek_();
#endif
extern unsigned int rw_base_read_();
*/
#endif


#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <errno.h>

#ifdef ESDOS
#ifndef FGV32
#include <dos.h>
#include <conio.h>
static union REGS inregs,outregs;
static struct SREGS segregs;
#endif
#endif


#include <rwmalloc.h>
#include <rtermin.h>
#include <rentra.h>
#define _NUEVODEF
#include <rfecha.h>
#include <rdef.h>       /* ya incluye el isam.h */
#include <rmante.h>
#include <rcalcul.h>
#include <rwmenus.h>

extern int modo_de_relacion;

extern int rentra_atrib;
extern int rentra_sal_atrib;
extern int rentra_cc;
extern int rentra_cf;
extern int rentra_sal_cc;
extern int rentra_sal_cf;
extern int redita_atrib;
extern int redita_cc;
extern int redita_cf;
extern long redita_linea;
extern int redita_col;

extern char *alt_dirfic;

extern char bufpan[PANBUFSIZE];

extern char rw_version[256];

extern char proceso[81];

extern char *lee_linea();

extern int repinta_texto;

extern int (*general_int)();

extern char *puntero_especial_rentra;

extern int   mimodo_debug;

extern int es_la_doble_;
extern int va_con_doble_;

/* Buffers para calculos ojo esto esta repetido en otros fuentes */
#define MAXBUFFERS 30
extern char **buff_ers[MAXBUFFERS] = {
   NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
      NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
      NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL
};
extern unsigned char tip_buf[MAXBUFFERS] = {
   255,255,255,255,255,255,255,255,255,255,
      255,255,255,255,255,255,255,255,255,255,
      255,255,255,255,255,255,255,255,255,255
};

/* =========================================================================*/


extern int fecha_protegida;
extern long max_prote_reg;


/********************************************************************
CON MODULO TRACER
********************************************************************/
extern int tracer_int;
extern int ntr_br;
extern int tracer_break[];

int es_el_modulo_erp = 0;

int pasa_a_int(double);

extern char no_imprime[3];

extern char dirbin[51];
extern char dirdef[51];
extern char dirpan[51];
extern char dirfic[51];
extern char nomfdir[51];
extern char dirbass[51];
extern char dirbase[51];
extern char dirficb[51];
extern char nomapli[25];
extern char modulo[9];
extern char apli_corre[25];
extern int apli_aborto;

extern char empresa[81];
extern char titulo[81];

extern FILE *_imp;
extern int atento_imp; /* control esclava */
extern char _fispool[51];
extern char _ejspool[51];

extern char *aginfo;
extern int LEERTERMINAL;
extern char nmen0[551];

extern char *quita_path();
extern double ld_doble();
extern long da_registo_actual();

extern int buf_tecla[200];

extern int bucle_break;

#ifdef RWXARXA
#ifdef RWSERVER
extern int en_conexion_externa;
#endif
#endif


/* para el gzip !!! */
extern void (*write_interno)();
extern unsigned (*read_interno)();
/********************/

extern int n_np;

#ifdef AGIEXTEND
DWORD   WINAPI GlobalDOSAlloc(UINT);
BOOL    WINAPI GlobalDOSFree(UINT);
#endif


/* Buffers para calculos ojo esto esta repetido en otros fuentes */
#define MAXBUFFERS 30
extern char **buff_ers[MAXBUFFERS];
extern unsigned char tip_buf[MAXBUFFERS];

#ifndef DSMINIBASICO

FILE *condefile(m,f,n)
int m;
FILE *f;
int *n;
{
   int i = 0;
   static FILE *ft[6] = {
      (FILE *)0,(FILE *)0,(FILE *)0,(FILE *)0,(FILE *)0,(FILE *)0};
      static char modos[6];
      
      if (m == 0) {
         while( ft[i] != (FILE *)0 && i < 6) i++;
         if (i == 6) {
            i = -1;
            f = NULL;
         }
         else {
            ft[i] = f;
            modos[i] = (char)(*n);
         }
         *n = i;
      }
      else
         if (m == 3) {
            for (;i < 6;i++)
               if (ft[i])
               {
#ifdef ESDOS
                  if ((unsigned long)ft[i] > 0x00000004L)
#endif
                     fclose(ft[i]);
                  ft[i] = (FILE *)0;
               }
         }
         else {
            i = *n;
            if (i < 0 && i > 5) return(NULL);
            f = ft[i];
            if (m == 2) {
               
               if (ft[i])
               {
#ifdef ESDOS
                  if ((unsigned long)ft[i] > 0x00000004L)
#endif
                     fclose(ft[i]);
               }
               ft[i] = (FILE *)0;
            }
            else
               *n = (int)modos[i];
         }
         return(f);
}

int rw_tratafi(p)
int p;
{
   int i,j,h,l,kk;
   unsigned char tmp[350];
   char tmp2[10];
   int sal = 0;
   FILE *f;
   long pos = -1L;
   unsigned short s1;
   unsigned long l2;
   float f3;
   double d4;
   int opcion;
   int externo = 0;
   
   
   if (prog[p].i == 255)
			{
      externo = 1;
      switch(prog[p-1].indice)
      {
      case 17:
         opcion = 119;
         break;
      case 18:
         opcion = 12;
         break;
      case 19:
         opcion = 14;
         break;
      case 20:
         opcion = 97;
         break;
      case 21:
         opcion = 121;
         break;
      case 22:
         opcion = 100;
         break;
      case 23:
         opcion = 225;
         break;
      case 24:
         opcion = 226;
         break;
      default:
         return (-1);
      }
			}
   else
      opcion = prog[p].i;
   
   if ( (i = memoria_alfa(p)) >= 0) {
      pos = (long)v_memo(5007)->numerica;
      switch (opcion) {
      case TTEMP: /* nombre temporal */
#ifdef RWSERVER
         if (externo && en_conexion_externa)
         {
            strcpy(tmp,v_memo(i)->alfabetica);
            funcion_windows_cliente(34,tmp,strlen(tmp)+1);
            if (no_memo_interna(i)) 
            {
              free(v_memo(i)->alfabetica);
              v_memo(i)->alfabetica = strmalloc(strlen(tmp) + 1);
            }
            strcpy(v_memo(i)->alfabetica,tmp);          
            break;
         }	     
#endif
         
         strcpy(tmp,v_memo(i)->alfabetica);
         temporal_(tmp);
         if (no_memo_interna(i)) {
            free(v_memo(i)->alfabetica);
            v_memo(i)->alfabetica = strmalloc(strlen(tmp) + 1);
         }
         strcpy(v_memo(i)->alfabetica,tmp);
         break;
      case TTEAB: /* abrir fichero */
#ifdef RWSERVER
         if (externo && en_conexion_externa)
         {
             /* no aplicable se usan funcioens X*/
            pos = -1L;
            sal = -1;
            break;
         }	     
#endif
         
         if (strlen(v_memo(i)->alfabetica) > 4) {
            v_memo(i)->alfabetica[3] = '\0';
            sal = 1;/* por defecto texto */
            for (j = 0;j < 3;j++) {
               if (v_memo(i)->alfabetica[j] == 'b') {sal = 0;break;} /* binario */
            }
            quita_blancos(v_memo(i)->alfabetica);
#ifdef ESDOS
            if (!strcmp(v_memo(i)->alfabetica+4,"LPT1") && v_memo(i)->alfabetica[0] == 'w')
            {
               f = (FILE *)(unsigned long)0x00000001L;
               no_imprime[0] = 0;
            }
            else
               if (!strcmp(v_memo(i)->alfabetica+4,"LPT2") && v_memo(i)->alfabetica[0] == 'w')
               {
                  f = (FILE *)(unsigned long)0x00000002L;
                  no_imprime[1] = 0;
               }
               else
                  if (!strcmp(v_memo(i)->alfabetica+4,"LPT3") && v_memo(i)->alfabetica[0] == 'w')
                  {
                     f = (FILE *)(unsigned long)0x00000003L;
                     no_imprime[2] = 0;
                  }
                  else
#endif	 
                     strcpy(tmp2,v_memo(i)->alfabetica);
                  strcpy(tmp,v_memo(i)->alfabetica+4);
                  f = fopen(tmp,tmp2);
                  for (j = 0;j < 4;j++) {
                     if (v_memo(i)->alfabetica[j] == 0)
                        v_memo(i)->alfabetica[j] = ' ';
                  }
                  if (f != NULL) {
                     if (condefile(0,f,&sal) != NULL) 
                     {
                        if (pos != -1L || !sal)
                           pos = ftell(f);
                        break;
                     }
                     fclose(f);
                  }
         }
         sal = -1;
         break;
      case TTELE:
#ifdef RWSERVER
         if (externo && en_conexion_externa)
         {
             /* no aplicable se usan funcioens X*/
            pos = -1L;
            sal = -1;
            break;
         }	     
#endif
         
         v_memo(i)->alfabetica[1] = '\0';
         sal = atoi(v_memo(i)->alfabetica);
         if (( f = condefile(1,(FILE *)0,&sal)) == (FILE *)0 || ((unsigned long)f) < (unsigned long)0x00000004L) {sal = -1;break;}
         
         if (pos >= 0L) fseek(f,pos,0);
         
         j = atoi(v_memo(i)->alfabetica+2);
         
         if (j >= MAXDEFECTO) {sal = -2;break;}
         
         if (sal == 0) {/* binario */
            sal = j = fread(tmp,1,j,f);
            if (j < 0) j = 0;
            tmp[j] = '\0';
         }
         else  { /* texto */
            if (fgets(tmp,j,f) == NULL) {
               sal = -1;
               j = 0;
               tmp[j] = 0;
            }
            else {
               quitan(tmp);    /* quita los '\n' de detras */
               sal = j = strlen(tmp);
            }
         }
         
         for (h = 0;h < j;h++) { /* codigos de escape */
            if (!tmp[h]) { /* solo para el caso de caracter 0 */
               j += 3;
               for(l=j;l > (h+3);l--)
                  tmp[l] = tmp[l-3];
               l = (int)tmp[h+4];
               sprintf(tmp+h+1,"%03d",(int)tmp[h]);
               tmp[h] = '\033';
               tmp[h+4] = (char)l;
            }
         }
         
         if (j >= MAXDEFECTO)
         {
            fseek(f,(long)(MAXDEFECTO - j - 1),1);
            j = MAXDEFECTO - 1;
            tmp[MAXDEFECTO - 1] = '\0';
         }
         
         if (no_memo_interna(i)) {
            free(v_memo(i)->alfabetica);
            v_memo(i)->alfabetica = strmalloc(j + 1);
         }
         strcpy(v_memo(i)->alfabetica,tmp);
         if (pos != -1L)
            pos = ftell(f);
         break;
      case TTEGR:
#ifdef RWSERVER
         if (externo && en_conexion_externa)
         {
             /* no aplicable se usan funcioens X*/
            pos = -1L;
            sal = -1;
            break;
         }	     
#endif
         
         if (strlen(v_memo(i)->alfabetica) < 2) break;
         v_memo(i)->alfabetica[1] = '\0';
         sal = atoi(v_memo(i)->alfabetica);
         if ( (f = condefile(1,(FILE *)0,&sal)) == (FILE *)0) {sal = -1;break;}
         if (pos != -1L) fseek(f,pos,0);
         j = strlen(v_memo(i)->alfabetica+2);
         for (h = 0,l = 0;h < j;h++,l++) {
            if (*(v_memo(i)->alfabetica+2+h) == '\033') { /* codigos de escape */
               memcpy(tmp+340,v_memo(i)->alfabetica+3+h,3);
               tmp[343] = '\0';
               tmp[l] = (char)atoi(tmp+340);
               h += 3;
            }
            else
               tmp[l] = *(v_memo(i)->alfabetica+2+h);
         }
         tmp[l] = '\0';
         if (sal == 0) /* binario */
         {
            j = l;
            if ( ((unsigned long)f) < (unsigned long)0x00000004L )
            {
               tmp[j+1] = j;
               sal = _graba_linea(tmp,f,j+2);
            }
            else
               sal = fwrite(tmp,1,j,f);
         }
         else { /* texto */
            j = strlen(tmp);
            sal = graba_linea(tmp,f);
         }
         if (pos != -1L)
            pos = ftell(f);
         break;
      case TTECI: /* cerrar fichero */
#ifdef RWSERVER
         if (externo && en_conexion_externa)
         {
             /* no aplicable se usan funcioens X*/
            pos = -1L;
            sal = -1;
            break;
         }	     
#endif
         
         sal = atoi(v_memo(i)->alfabetica);
         if (condefile(2,(FILE *)0,&sal) == (FILE *)0) 
         {
            sal = -1;
         }
         else
         {
            pos = -1L;
            v_memo(5007)->numerica = (double)pos;
         }
         break;
      case TFITE: /* borrar fichero */
         
#ifdef RWSERVER
         if (externo && en_conexion_externa)
         {
            strcpy(tmp,v_memo(i)->alfabetica);
            funcion_windows_cliente(16,tmp,strlen(tmp)+1);
            memcpy_int(&sal,tmp,sizeof(int));
            pos = -1L;
            break;
         }	     
#endif	   
         sal = rw_base_unlink(v_memo(i)->alfabetica);
         break;
      case 225:
#ifdef RWSERVER
         if (externo && en_conexion_externa)
         {
             /* no aplicable se usan funcioens X*/
            pos = -1L;
            sal = -1;
            break;
         }	     
#endif
         
         v_memo(i)->alfabetica[1] = '\0';
         sal = atoi(v_memo(i)->alfabetica);
         if (( f = condefile(1,(FILE *)0,&sal)) == (FILE *)0 || ((unsigned long)f) < (unsigned long)0x00000004L) {sal = -1;n_np = 2;break;}
         if (pos >= 0L) fseek(f,pos,0);
         kk = prog[p+1].indice;
         j = atoi(v_memo(i)->alfabetica+2);
         switch(j)
         {
         case 2:
            sal = fread(tmp,1,4,f);
            if (sal == 4)
            {
               memcpy(&l2,tmp,4);
               v_memo(kk)->numerica = (double)l2;
            }
            break;
         case 3:
            sal = fread(tmp,1,4,f);
            if (sal == 4)
            {
               memcpy(&f3,tmp,4);
               v_memo(kk)->numerica = (double)f3;
            }
            break;
         case 4:
            sal = fread(tmp,1,8,f);
            if (sal == 8)
            {
               memcpy(&d4,tmp,8);
               v_memo(kk)->numerica = (double)d4;
            }
            break;
         default:
            sal = fread(tmp,1,2,f);
            if (sal == 2)
            {
               memcpy(&s1,tmp,2);
               v_memo(kk)->numerica = (double)s1;
            }
            break;
         }
         n_np = 2;
         if (pos != -1L)
            pos = ftell(f);
         break;
         case 226:
#ifdef RWSERVER
            if (externo && en_conexion_externa)
            {
                /* no aplicable se usan funcioens X*/
               pos = -1L;
               sal = -1;
               break;
            }	     
#endif
            
            v_memo(i)->alfabetica[1] = '\0';
            sal = atoi(v_memo(i)->alfabetica);
            if (( f = condefile(1,(FILE *)0,&sal)) == (FILE *)0 || ((unsigned long)f) < (unsigned long)0x00000004L) {sal = -1;n_np = 2;break;}
            if (pos >= 0L) fseek(f,pos,0);
            kk = prog[p+1].indice;
            j = atoi(v_memo(i)->alfabetica+2);
            switch(j)
            {
            case 2:		   
               l2 = (long)v_memo(kk)->numerica;
               memcpy(tmp,&l2,4);		     
               sal = fwrite(tmp,1,4,f);
               break;
            case 3:
               f3 = (float)v_memo(kk)->numerica;
               memcpy(tmp,&f3,4);		     
               sal = fwrite(tmp,1,4,f);
               break;
            case 4:
               d4 = (double)v_memo(kk)->numerica;
               memcpy(tmp,&d4,8);		     
               sal = fwrite(tmp,1,8,f);
               break;
            default:
               s1 = (unsigned short)v_memo(kk)->numerica;
               memcpy(tmp,&s1,2);		     
               sal = fwrite(tmp,1,2,f);
               break;
            }
            n_np = 2;
            if (pos != -1L)
               pos = ftell(f);
            break;
            default:
               break;
    }
    sprintf(v_memo(5004)->alfabetica,"%d",sal);
    if (pos != -1L)
       v_memo(5007)->numerica = (double)pos;
    return;
  }
  
  strcpy(v_memo(5004)->alfabetica,"-1");
  return;
}

int rw_bmenug(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int b,c,n,t,pos,defe,r,m,i,ops;
   char *opcis[400];
   char **tmp;
   int sumap = 0;
   
   if (prog[p].i == 255)
      sumap = 1;
   
   b   = pasa_a_int(v_memo(prog[p++].indice)->numerica); /* buffer */
   c   = pasa_a_int(v_memo( prog[p++].indice )->numerica); /* comienzo */
   n   = pasa_a_int(v_memo( prog[p++].indice )->numerica); /* salto */
   t   = pasa_a_int(v_memo( prog[p++].indice )->numerica); /* fin */
   pos = pasa_a_int(v_memo( prog[p++].indice )->numerica); /* posicion */
   defe= pasa_a_int(v_memo( prog[p++].indice )->numerica); /* defecto */
   
   if (b < 0 || b >= MAXBUFFERS)
      r = -1;
   else
      if (!buff_ers[b] || tip_buf[b])
         r = -1;
      else {
         m = *((int *)buff_ers[b]);
         tmp = (char **)(((char *)(buff_ers[b]))+sizeof(int));
         if (m < t) t = m;
         ops = 0;
         for (i = c;i < t;i += n) {
            opcis[ops] = tmp[i];
            ops++;
         }
         if (sumap)
         {	  
            r = menug_tam(opcis,ops,pos,defe,pasa_a_int(v_memo( prog[p++].indice )->numerica));
         }
         else
         {
            r = menug(opcis,ops,pos,defe);
         }
      }
      
      n_np = 6 + sumap;
      sprintf(v_memo(5004)->alfabetica,"%d",r);
      return(0);
}


asigna_tira(prov,p)
char *prov;
int p;
{
   int i;
   if (( i = memoria_alfa(p)) >= 0) {
      if (no_memo_interna(i)) {
         free(v_memo(i)->alfabetica);
         v_memo(i)->alfabetica = strmalloc(strlen(prov)+1);
      }
      strcpy(v_memo(i)->alfabetica,prov);
			}
   return(0);
}


int rw_zonabla(p,tip,faclin)
int p;
int tip;
int faclin;
{
			zonablan(pasa_a_int((v_memo(prog[p].indice)->numerica / 10000)),
            pasa_a_int(((long)v_memo(prog[p].indice)->numerica % 10000)));
         return(0);
}


int rw_graf(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int i;
   
   i = pasa_a_int(v_memo(prog[p].indice)->numerica) - 2;
   if (i == -2)
      Gon();
   else
      if (i == -1) {
         Goff();
         fflush(stdout);
						}
      else
         Graf(i);
      
      return(0);
}



int rw_pausa(p,tip,faclin)
int p;
int tip;
int faclin;
{
   
   sprintf(v_memo(5004)->alfabetica,"%d",parada());
   
   return(0);
}

static VENT *ventas[7];
static int nventas = 0; /* ventanas en calculos -- limitadas a 7 -- */

int rw_wpon(p,tip,faclin)
int p;
int tip;
int faclin;
{
   if (nventas < 7)
   {
      rwdebug = 0;
      ventas[nventas] = w_iniesp(pasa_a_int((v_memo(prog[p].indice)->numerica / 10000)),
         pasa_a_int(((long)v_memo(prog[p].indice)->numerica % 10000)));
      rwdebug = 1;
      if (!ventas[nventas])
      {
         return(0);
      }
      w_salva(ventas[nventas]);
      nventas++;
   }
   return(0);
}

int rw_wqui(p,tip,faclin)
int p;
int tip;
int faclin;
{
   if (nventas)
   {
      nventas--;
      if (ventas[nventas] != NULL) {
									if (prog[p].i == 118)
                              w_pinta(ventas[nventas]);
                           w_finesp(ventas[nventas]);
                           ventas[nventas] = NULL;
      }
   }
   return(0);
}


int rw_cabeza(p,tip,faclin)
int p;
int tip;
int faclin;
{
   
   /*cabecera_v(rw_version,v_memo(prog[p].indice)->alfabetica,titulo,empresa);*/
   if (v_memo(prog[p].indice)->alfabetica[0])
	   cabecera(v_memo(prog[p].indice)->alfabetica,titulo,empresa);
   else
	   cabecera(proceso,titulo,empresa);
   return(0);
}


static int cambia_fpath(s,m,c)
char *s;
int m;
int c;
{
   char tmp[256];
   char tmp2[256];
   int punto;
   int i;
   
   if (c && alt_dirfic && strlen(v_memo(m)->alfabetica) >= strlen(dirbass) &&
      !memcmp(v_memo(m)->alfabetica,dirbass,strlen(dirbass)))
   {
      strcpy(tmp,alt_dirfic);
      strcat(tmp,v_memo(m)->alfabetica+strlen(dirbass));
   }
   else
   {
      i = strlen(v_memo(m)->alfabetica);
      if (va_con_doble_ && es_la_doble_ && i > 1 && i >= strlen(dirfic) &&
         !memcmp(v_memo(m)->alfabetica,dirfic,strlen(dirfic)) && 
         v_memo(m)->alfabetica[i-2] >= '0' && v_memo(m)->alfabetica[i-2] <= '9')
      {
         strcpy(tmp,v_memo(m)->alfabetica);
         tmp[i-1] = '!';
         tmp[i] = '/';
         tmp[i+1] = 0;
      }
      else
	  {
         strcpy(tmp,v_memo(m)->alfabetica);
		 if (c)
		 {
			 punto = strbusca(tmp,"/fich/");
			 if (punto > -1)
			 {
				 tmp[punto+1] = 0;
				 strcpy(tmp2,tmp);
				 strcat(tmp2,fichdir);
				 strcat(tmp2,tmp+punto+5);
				 strcpy(tmp,tmp2);
			 }
		 }
	  }
   }
   strcat(tmp,quita_path(s));
   strcpy(s,tmp);
   return(0);
}


/* formato de prueba compatible 100% con estructura mdr antigua */

int rw_atira(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int i;
   char tmp[60];
   switch(((unsigned char)prog[p].i)) {
   case TDBAS:
      asigna_tira(dirbase,p);
      break;
   case TDFIC:
      i = strlen(dirfic);	
      if (va_con_doble_ && es_la_doble_ && mpfi[48] && i > 2 && dirfic[i-2] == '!')
      {
         strcpy(tmp,dirfic);
         tmp[i-2] = '/';
         tmp[i-1] = 0;
         asigna_tira(tmp,p);
      }
      else
         asigna_tira(dirfic,p);
      break;
   case TDFIB:
      asigna_tira(dirficb,p);
      break;
   case TDPAN:
      asigna_tira(dirpan,p);
      break;
   case TDDEF:
      asigna_tira(dirdef,p);
      break;
   case TDBIN:
      asigna_tira(dirbin,p);
      break;
   case TDBSS:
      asigna_tira(dirbass,p);
      break;
   case 131:
      asigna_tira(modulo,p);
      break;
   default:
      asigna_tira(nomapli,p);
   }
   return(0);
}

int rw_cpath(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int f,
      i,
      m;
   
   f = v_prog(prog[p].indice);
   m = prog[p+1].indice;
   
   if (que_memo_tipo(m) == CHARTIPO) {
      if (prog[p].i == TPASI)
      {
         cambia_fpath(v_mpfi(f)->f_fichero,m,1);
      }
      else
         if (prog[p].i == TPASP) {
            for (i = 0;i < v_mpfi(f)->f_npanta;i++)
															cambia_fpath(v_mpfi(f)->f_pantalla[i].p_nombrepan,m,0);
         }
         else
            if (prog[p].i == TFNAME) 
			{
				if (no_memo_interna(m)) 
				{
                    free(v_memo(m)->alfabetica);
                    v_memo(m)->alfabetica = strmalloc(strlen(v_mpfi(f)->f_fichero)+1);
                }
                strcpy(v_memo(m)->alfabetica,v_mpfi(f)->f_fichero);
            }
            else
			{
				char *ext;
				strcpy(quita_path(v_mpfi(f)->f_fichero),v_memo(m)->alfabetica);
				ext = v_mpfi(f)->f_fichero + strlen(v_mpfi(f)->f_fichero);
                ext -= 4;
				if (ext >= v_mpfi(f)->f_fichero && *ext == '.')
				{
				   if (!strcmp(ext,".dat"))
				   {
					  v_mpfi(f)->tipo_datos = 0;
				   }
				   else  if (!strcmp(ext,".dbs"))
				   {
					  v_mpfi(f)->tipo_datos = 1;
				   }
				   else	 if (!strcmp(ext,".db3"))
				   {
				 	  v_mpfi(f)->tipo_datos = 2;
				   }
				   else  if (!strcmp(ext,".dbp"))
				   {
					  v_mpfi(f)->tipo_datos = 3;
				   }
				   else  if (!strcmp(ext,".odb"))
				   {
					  v_mpfi(f)->tipo_datos = 4;
				   }		
				   *ext = 0;
				}
			}
            /* isrename( from,to ); */
   }
   n_np = 2;
   return(0);
}


/* YA NO SE USA SALVO PARA ERROR */
int rw_acaba(p,tip,faclin)
int p;
int tip;
int faclin;
{
   
   n_np = 0;
   while(prog[p].i != TEND) {
      p++;n_np++;
   } /* a mejorar !!! */
   /* los posibles bucles ya se resetean con la salida del p_ejecuta */
   return(0);
}

int rw_operror(p,tip,faclin)
int p;
int tip;
int faclin;
{
   
   
   if (prog[p].i == TERROR)
   {
      operaerror = OE_AVISO;
   }
   else
      if (prog[p].i == 138)
         operaerror = 2;
      else
         operaerror = 10;
      return(0);
}

int rw_break()
{
   bucle_break = 1;
   return(0);
}

int rw_nobreak()
{
   bucle_break = 0;
   return(0);
}

int rw_aborta() /* abandonar el mdr corriente */
{
   apli_aborto = 1;
   return(0);
}

int rw_kfecha(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int f;
   char tmp[MAXDEFECTO];
   
   
   f = prog[p].indice;
   
   ld_campo(f / 100,v_mpfi(v_prog(f%100)),tmp); /* cuidado si no es fecha */
   /* check_kfecha(tmp); */
   
   return(0);
}


int rw_pcontr(p,tip,faclin)
int p;
int tip;
int faclin;
{
   sprintf(v_memo(5004)->alfabetica,"%d",printa_control(prog[p].indice));
   return(0);
}

int rw_mante(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int i;
   
   i = prog[p].indice;
   
   sprintf(v_memo(5004)->alfabetica,"%d",mante_ficha(v_prog((i%10000)),(i/10000),0,(char *)0));
   
   return(0);
}

int rw_consul(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int i;
   
   i = prog[p].indice;
   
   sprintf(v_memo(5004)->alfabetica,"%d",consul_ficha(v_prog(i%100),i/100,1));
   
   return(0);
}

int rw_consul_clave(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int i;
   char *ffiltro = NULL;
   char tmp[257];
   i = prog[p].indice;

   {
	    char *p;
		p = v_memo(5004)->alfabetica;
		p = strstr(p,"{F:");
		if (p)
		{
			char *p1;
			int n,i;
			p += 3;
			p1 = p;
			while(*p1 && *p1 != '}') p1++;
			n = (int)(p1 - p);
			if (n >= sizeof(tmp))
			{
				n = sizeof(tmp) - 1;
			}
			memcpy(tmp,p,n);
			tmp[n] = 0;
			ffiltro = tmp;
		}		
   }   
   sprintf(v_memo(5004)->alfabetica,"%d",consul_clave(v_prog(i%100),i/100,-1,0,0,0,-1,-1,ffiltro));
   
   return(0);
}

int rw_pulsate(p,tip,faclin)
int p;
int tip;
int faclin;
{
   sprintf(v_memo(5004)->alfabetica,"%d",pulsatecla());
   return(0);
}


int rw_hora(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int i;
   if (( i = memoria_alfa(p)) >= 0) 
   {
      if (no_memo_interna(i)) 
	  {
         free(v_memo(i)->alfabetica);
         v_memo(i)->alfabetica = malloc(9);
      }
      strcpy(v_memo(i)->alfabetica,fecha()+11);
   }
   
   return(0);
}

rw_imprime(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int i;
   int modo = 0;
   
   if (prog[p].i != 30)
   {
      modo = 4;
   }
   
   if (( i = memoria_alfa(p)) >= 0)
   {
      printa_linea_(v_memo(i)->alfabetica,modo);
   }
   n_np = 1;
   return(0);
}

int crea_buffer(maxcomp,tipo)
int maxcomp;
int tipo;
{
   int i;
   
   if (maxcomp < 0 || maxcomp > 8000) return(-1);
   
   for (i = 0;i < MAXBUFFERS;i++) {
      if (!buff_ers[i]) break;
   }
   if (i >= MAXBUFFERS) return(-1);
   
   buff_ers[i] = (char **)malloc( maxcomp * sizeof(char *) + sizeof(int) + 1);
   
   memset( ((char *)(buff_ers[i])) + sizeof(int), 0 , maxcomp * sizeof(char *) );
   *((int *)(buff_ers[i])) = maxcomp;
   tip_buf[i] = tipo;
   return(i);
}


int trata_el_buffer(b,n,d,l,que,tipo)
int b;   /* Buffer */
int n;   /* numero de componente del buffer */
char *d; /* Dato */
int l;   /* longitud del dato */
int que; /* 0 mete 1 saca 2 saca string (cuidado con maximo)*/
int tipo;
{
   int m,i;
   char **tmp;
   int ojo = 0;
   
   if (b < 0 || b >= MAXBUFFERS) {
      return(-1);
   }
   if (!buff_ers[b]) {
      return(-1);
   }
   m = *((int *)buff_ers[b]);
   if (n < 0 || n >= m || (tipo < 255 && tipo != tip_buf[b])) {
      return(-1);
   }
   tmp = (char **)(((char *)(buff_ers[b]))+sizeof(int));
   if (tmp[n]) {
      if (!que) {
         if (tmp[n] != puntero_especial_rentra)
            free(tmp[n]);
         else
            ojo = 1;
      }
   }
   else {
      if (que) {
         if (que == 2)
            d[0] = 0;
         else
            memset(d,0,l);
         return(0);
      }
   }
   if (!que) {
      if (!ojo)
         tmp[n] = malloc(l);
      memcpy(tmp[n],d,l);
   }
   else {
      if (que == 2) {
         i = strlen(tmp[n]);
         if (i > l)
            l++;
         else
            l = i + 1;
      }
      memcpy(d,tmp[n],l);
   }
   
   return(0);
}


int fin_buffer(b)
int b;
{
   int m,i;
   char **tmp;
   
   if (b < 0 || b >= MAXBUFFERS) return(-1);
   
   if (buff_ers[b]) {
      m = *((int *)buff_ers[b]);
      tmp = (char **)(((char *)(buff_ers[b]))+sizeof(int));
      for (i = 0;i < m;i++) {
         if (*tmp) {
            free(*tmp);
         }
         tmp++;
      }
      free(buff_ers[b]);
      buff_ers[b] = NULL;
      tip_buf[b] = 255;
   }
   return(0);
}

int rw_crea_buffer(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int mt,mb,t,tipo,b;
   
   mt  = prog[p++].indice; /* tamano */
   mb  = prog[p++].indice; /* buffer */
   t = pasa_a_int(v_memo(mt)->numerica);
   tipo = pasa_a_int(v_memo(prog[p++].indice)->numerica);
   
   if (tipo < 0) tipo = CHARTIPO;
   b = crea_buffer(t,tipo);
   v_memo(mb)->numerica = (double)b;
   sprintf(v_memo(5004)->alfabetica,"%d",b);
   n_np = 3;
   return(0);
}


int rw_fin_buffer(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int mb  = prog[p].indice;
   int b = pasa_a_int(v_memo(mb)->numerica);
   b = fin_buffer(b);
   v_memo(mb)->numerica = (double)b;
   sprintf(v_memo(5004)->alfabetica,"%d",b);
   return(0);
}


int rw_quesis(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int d = prog[p].indice;
   
   if (no_memo_interna(d))
   {
      free(v_memo(d)->alfabetica);
      v_memo(d)->alfabetica = strmalloc(strlen(_SISTEMA)+1);
   }
   strcpy(v_memo(d)->alfabetica,_SISTEMA);
   
   return(0);
}


int rw_protefun(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int f = pasa_a_int(v_memo( prog[p++].indice )->numerica);
   int m = pasa_a_int(v_memo( prog[p++].indice )->numerica);
   int md = prog[p++].indice;
   char *d = v_memo(md)->alfabetica;
   extern char *tecla_funcion();
   
   d = tecla_funcion(f,d,m);
   if (d) {
      if (m) {
         if (no_memo_interna(md)) {
            free(v_memo(md)->alfabetica);
            v_memo(md)->alfabetica = strmalloc(strlen(d)+1);
         }
         strcpy(v_memo(md)->alfabetica,d);
      }
   }
   n_np = 3;
   return(0);
}


int rw_subopc(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int pp = prog[p++].indice;
   int o_modo_rel = modo_de_relacion;
   char tmp[512];
   memset(tmp,0,sizeof(tmp));
   modo_de_relacion = 1;
   strcpy(tmp,v_memo(pp)->alfabetica);
   sprintf(v_memo(5004)->alfabetica,"%d",entra_int(tmp,1));
   modo_de_relacion = o_modo_rel;
   return(0);
}


int fgvsubejecutanp(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int pp = prog[p++].indice;
   int o_modo_rel = modo_de_relacion;
   char tmp[512];
   memset(tmp,0,sizeof(tmp));
   modo_de_relacion = 1;
   strcpy(tmp,v_memo(pp)->alfabetica);
   sprintf(v_memo(5004)->alfabetica,"%d",entra_int(tmp,0));
   modo_de_relacion = o_modo_rel;
   return(0);
}


int rw_abrbrel(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int ipfi = v_prog(prog[p].indice);
   
   if (v_mpfi(ipfi)->f_nclaves <= 0 || v_mpfi(ipfi)->f_fd >= 0)
   {
      strcpy(v_memo(5004)->alfabetica,"-1");	
      return(0);
   }
   iserrno = 0;
   if (abre_ind(v_mpfi(ipfi),ISINOUT + ISEXCLLOCK) >= 0) 
   {
      f_s(ipfi,RABIERTO);
      f_n(ipfi,RCERRADO);
   }
   else
   {
      if (iserrno == ENOENT) 
      {
         crea_ind(v_mpfi(ipfi));
         cierra_ind(v_mpfi(ipfi));
         v_mpfi(ipfi)->f_fd = -1;
         agichmod(v_mpfi(ipfi)->f_fichero,".*","","");
         if (abre_ind(v_mpfi(ipfi),ISINOUT + ISEXCLLOCK) >= 0) 
         {
            f_s(ipfi,RABIERTO);
            f_n(ipfi,RCERRADO);
         }
      }
   }
   sprintf(v_memo(5004)->alfabetica,"%d",iserrno);
   if (iserrno == 0)
   {
      punto_abierto[ipfi] = 0;
      f_s(ipfi,RNOCERRAR);
   }
   return(0);
}


int rw_abrtrel(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int ipfi = v_prog(prog[p].indice);
   
   iserrno = 0;
   abre1_rel(ipfi);
   sprintf(v_memo(5004)->alfabetica,"%d",iserrno);
   if (iserrno == 0)
   {
      punto_abierto[ipfi] = 0;
      f_s(ipfi,RNOCERRAR);
      libera_ind(v_mpfi(ipfi));
   }
   
   return(0);
}

int rw_cietrel(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int ipfi = v_prog(prog[p].indice);
   
   iserrno = 0;
   libera_ind(v_mpfi(ipfi));
   cierra1_rel(ipfi);
   sprintf(v_memo(5004)->alfabetica,"%d",iserrno);
   if (iserrno == 0)
   {
      punto_abierto[ipfi] = 0; 
      f_n(ipfi,RNOCERRAR);
   }
   return(0);
}

int rw_abrerel(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int ipfi = v_prog(prog[p].indice);
   
   iserrno = 0;
   abre1_rel(ipfi);
   sprintf(v_memo(5004)->alfabetica,"%d",iserrno);
   if (v_mpfi(ipfi)->f_fd >= 0) {punto_abierto[ipfi]++;libera_ind(v_mpfi(ipfi));}
   
   return(0);
}

int rw_cierel(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int ipfi = v_prog(prog[p].indice);
   
   if (v_mpfi(ipfi)->f_fd >= 0 && punto_abierto[ipfi] > 0) 
   {
      punto_abierto[ipfi]--;
   }
   libera_ind(v_mpfi(ipfi));
   if (punto_abierto[ipfi] > 0)
   {
      iserrno = 0;
      cambia_ind(v_mpfi(ipfi),1);
      sprintf(v_memo(5004)->alfabetica,"ABIERTO %d",iserrno);
      return(0);
   }
   if (f_v(ipfi,RNOCERRAR) == -1) 
   {
      iserrno = 0;
      cierra1_rel(ipfi);
      sprintf(v_memo(5004)->alfabetica,"%d",iserrno);
   }
   else
   {
      sprintf(v_memo(5004)->alfabetica,"%d",-1);
   }
   return(0);
}


int rw_repinta(p,tip,faclin)
int p;
int tip;
int faclin;
{
   repinta_texto = 1;
   redita_linea = (long)v_memo( prog[p].indice )->numerica;
   p++;
   redita_col   = pasa_a_int(v_memo( prog[p].indice )->numerica);
   n_np = 2;
   return(0);
}

int rw_posedita(p,tip,faclin)
int p;
int tip;
int faclin;
{
   v_memo( prog[p].indice )->numerica = (double)redita_linea;
   p++;
   v_memo( prog[p].indice )->numerica = (double)redita_col;
   n_np = 2;
   return(0);
}

#ifdef ESDOS
#ifndef FGV32

static unsigned char com_int[2] =
{
   0xc,
      0xb,
};

static unsigned short com_data[2] =
{
   0x3f8,
      0x2f8,
};
static unsigned short com_ier[2] =
{
   0x3f9,
      0x2f9
};

static unsigned short com_lcr[2] =
{
   0x3fb,
      0x2fb
};

static unsigned short com_mcr[2] =
{
   0x3fc,
      0x2fc
};

static unsigned short com_sts[2]=
{
   0x3fd,
      0x2fd
};

static unsigned char int_mask[2]=
{
   0x10,
      0x08
};

static unsigned short pic_mask=0x21; /* 8259 interrupt mask port */
static unsigned short pic_eoi=0x20; /* 8259 EOI port */

static unsigned short o_seg[2];
static unsigned short o_off[2];

static unsigned short c_status[2] = {0,0};

#ifdef AGIEXTEND
extern unsigned char *Rbuffer1;
extern unsigned char *Rbuffer2;
extern unsigned int c_punto1;
extern unsigned int c_punto2;
extern unsigned int m_punto1;
extern unsigned int m_punto2;
extern unsigned int r_punto1;
extern unsigned int r_punto2;
static unsigned char *buffer1 = NULL;
static unsigned char *buffer2 = NULL;
extern unsigned int acodeSegPara;
extern unsigned int asc_int1;
extern unsigned int asc_int2;
#else
static unsigned char *buffer1 = NULL;
static unsigned char *buffer2 = NULL;
static unsigned int c_punto1 = 0;
static unsigned int c_punto2 = 0;
static unsigned int m_punto1 = 0;
static unsigned int m_punto2 = 0;
static unsigned int r_punto1 = 0;
static unsigned int r_punto2 = 0;
#endif

/* Rutinas para control del puerto serie */

static unsigned short c_tmp1;
static unsigned char c_tmp2;


#ifndef AGIEXTEND
void (interrupt far asc_int1)(void)
{
   asm {
      sti;
   }
   
   
   c_tmp1 = com_data[0];
   asm {
      mov dx,c_tmp1;
      in al,dx;
      mov c_tmp2,al
   }
   buffer1[c_punto1] = c_tmp2;
   
   c_punto1++;
   if (c_punto1 == m_punto1)
      c_punto1 = 0;
   asm {
      mov dx,pic_eoi;
      mov al,20h;
      out dx,al;
   }
}

void (interrupt far asc_int2)(void)
{
   
   asm {
      sti;
   }
   
   c_tmp1 = com_data[1];
   asm {
      mov dx,c_tmp1;
      in al,dx;
      mov c_tmp2,al
   }
   
   
   
   buffer2[c_punto2] = c_tmp2;
   c_punto2++;
   if (c_punto2 == m_punto2)
      c_punto2 = 0;
   asm {
      mov dx,pic_eoi;
      mov al,20h;
      out dx,al;
   }
}
#endif

int inicio_com(c,t,bauds,par,bits,sbits)
int c;
int t;
int bauds;
char *par;
int bits;
int sbits;
{
   unsigned char _pari = 1;
   unsigned char _stat = 0;
   unsigned char _stat0 = 0;
   unsigned char l1 = 0;
   unsigned char l2 = 0;
   unsigned long ba = 0;
#ifdef AGIEXTEND
   char i1;
   int i2,i3,i4,i5;
   DWORD realreg;
#endif
   
   
   if (c < 0 || c > 1) return(-1);
   if (c_status[c]) return(-1);
   if (bauds < 300) return(-1);
   ba = 384 / (bauds / 300);
   l1 = *((unsigned char *)&ba);
   l2 = *(((unsigned char *)&ba)+1);
   if (bits == 8)
      _stat |= 0x3;
   else
      _stat |= 0x2;
   if (sbits != 1)
      _stat |= 0x4;
   if (*par == 'E')
   {
      _stat |= (_pari << 3);
      _stat |= (_pari << 4);
   }
   else
      if (*par == 'O')
      {
         _stat |= (_pari << 3);
      }
      _stat0 = _stat;
      _stat |= 0x80;
      
      outp(com_lcr[c],_stat);
      
      outp(com_data[c],l1);
      outp(com_ier[c],l2);
      outp(com_lcr[c],_stat0);
      
#ifdef AGIEXTEND
      
      i1 = com_int[c];
      asm {
         push cx
            push ax
            push bx
            push dx
            
            mov ax,0200h
            mov bl,i1;
         push ds
            int 31h;
         pop ds
            mov i2,cx
            mov i3,dx
            
            pop dx
            pop bx
            pop ax
            pop cx
      }
      o_seg[c] = i2;
      o_off[c] = i3;
      if (!c)
      {
         i2 = acodeSegPara;
         i3 = asc_int1;
         realreg = GlobalDOSAlloc(t);
         buffer1 = MK_FP( LOWORD(realreg), 0 );
         Rbuffer1 = MK_FP( HIWORD(realreg), 0 );
         if (!buffer1) return(-1);
         c_punto1 = 0;
         r_punto1 = 0;
         m_punto1 = t;
      }
      else
      {
         i2 = acodeSegPara;
         i3 = asc_int2;
         realreg = GlobalDOSAlloc(t);
         buffer2 = MK_FP( LOWORD(realreg), 0 );
         Rbuffer2 = MK_FP( HIWORD(realreg), 0 );
         if (!buffer2) return(-1);
         c_punto2 = 0;
         r_punto2 = 0;
         m_punto2 = t;
      }
      c_status[c] = 1;
      i1 = com_int[c];
      /* DPMI */
      asm {
         push dx
            push cx
            push ax
            push bx
            mov cx,i2
            mov dx,i3
            mov ax,0201h
            mov bl,i1;
         push ds
            int 31h;
         pop ds
            pop bx
            pop ax
            pop cx
            pop dx
      }
#else
      
      
      inregs.h.ah = 0x35;
      inregs.h.al = com_int[c];
      intdosx(&inregs,&outregs,&segregs);
      o_seg[c] = segregs.es;
      o_off[c] = outregs.x.bx;
      if (!c)
      {
         segregs.ds = FP_SEG(asc_int1);
         inregs.x.dx = FP_OFF(asc_int1);
         buffer1 = malloc(t);
         if (!buffer1) return(-1);
         c_punto1 = 0;
         r_punto1 = 0;
         m_punto1 = t;
      }
      else
      {
         segregs.ds = FP_SEG(asc_int2);
         inregs.x.dx = FP_OFF(asc_int2);
         buffer2 = malloc(t);
         if (!buffer2) return(-1);
         c_punto2 = 0;
         r_punto2 = 0;
         m_punto2 = t;
      }
      c_status[c] = 1;
      inregs.h.ah = 0x25;
      inregs.h.al = com_int[c];
      intdosx(&inregs,&outregs,&segregs);
#endif
      
      outp(com_mcr[c],0x0b | 0x03); /* modem control register */
      outp(com_ier[c],0x1); /* set interrupt enable */
                            /*
                            in al,pic_mask ; read current 8259 mask
                            and al, not int_mask ; set mask for COM port
                            out pic_mask,al ; write new 8259 mask
      */
      l1 = inp(pic_mask);
      l1 = l1 & (~int_mask[c]);
      outp(pic_mask,(unsigned int)l1);
      return(0);
}


int lee_com(c,buf,max_,modo)
int c;
char *buf;
int max_;
int modo;
{
   int i;
   int n = 0;
   if (c < 0 || c > 1) return(-1);
   if (!c_status[c]) return(-1);
   if (!c)
   {
      i = r_punto1;
      while(i != c_punto1 && n < max_)
      {
         buf[n] = buffer1[i];
         i++;
         if (i == m_punto1)
            i = 0;
         n++;
      }
      if (!modo)
         r_punto1 = i;
   }
   else
   {
      i = r_punto2;
      while(i != c_punto2 && n < max_)
      {
         buf[n] = buffer2[i];
         i++;
         if (i == m_punto2)
            i = 0;
         n++;
      }
      if (!modo)
         r_punto2 = i;
   }
   buf[n] = 0;
   return(n);
}

int graba_com(c,buf)
int c;
char *buf;
{
   int n = 0;
   long timeout;
   if (c < 0 || c > 1) return(-1);
   if (!c_status[c]) return(-1);
   while(buf[n])
			{
      timeout = 0L;
      /*si TBE bit is not set hay que esperar*/
      while(!(inp(com_sts[c]) & 0x20) && timeout < 300000L) timeout++;
      if (timeout >= 300000L) return(-1);
      outp(com_data[c],buf[n]);
      n++;
			}
   return(n);
}

int com_status(c,buf)
int c;
char *buf;
{
			int n = 0,i;
         if (c < 0 || c > 1) return(-1);
         if (!c_status[c]) return(-1);
         if (!c)
         {
            i = r_punto1;
            while(i != c_punto1)
            {
               i++;
               if (i == m_punto1)
                  i = 0;
               n++;
            }
         }
         else
         {
            i = r_punto2;
            while(i != c_punto2)
            {
               i++;
               if (i == m_punto2)
                  i = 0;
               n++;
            }
         }
         return(n);
}

int fin_com(c)
int c;
{
#ifdef AGIEXTEND
   char i1;
   int i2,i3,i4,i5;
#endif
   
   if (c < 0 || c > 1) return(-1);
   if (!c_status[c]) return(-1);
   
   outp(pic_mask,(inp(pic_mask) | int_mask[c]));
   
#ifdef AGIEXTEND
   
   i1 = com_int[c];
   i2 = o_seg[c];
   i3 = o_off[c];
   asm {
      push dx
         push cx
         push ax
         push bx
         mov cx,i2
         mov dx,i3
         mov ax,0201h
         mov bl,i1;
      int 31h;
      pop bx
         pop ax
         pop cx
         pop dx
   }
   
   if (!c)
   {
      GlobalDOSFree(FP_SEG(buffer1));
      buffer1 = NULL;
      Rbuffer1 = NULL;
   }
   else
   {
      GlobalDOSFree(FP_SEG(buffer2));
      buffer2 = NULL;
      Rbuffer2 = NULL;
   }
   
#else
   segregs.ds = o_seg[c];
   inregs.x.dx = o_off[c];
   inregs.h.ah = 0x25;
   inregs.h.al = com_int[c];
   intdosx(&inregs,&outregs,&segregs);
   if (!c)
   {
      free(buffer1);
      buffer1 = NULL;
   }
   else
   {
      free(buffer2);
      buffer2 = NULL;
   }
#endif
   c_status[c] = 0;
   return(0);
}

int fin_todo_com()
{
   int c;
   for (c = 0;c < 1;c++)
   {
						if (!c_status[c]) continue;
                  fin_com(c);
   }
   
}

#endif
#endif
/*
parametros; puerto,Bauds,Paridad,bits,stopbits,BUFFER
"INICIOCOM"      ,"959395939393"    ,135, NULL,
*/
int rw_iniciocom(p,tip,faclin)
int p;
int tip;
int faclin;
{
   char *com = v_memo( prog[p++].indice )->alfabetica;
   int baud = pasa_a_int( v_memo(prog[p++].indice)->numerica );
   char *par = v_memo( prog[p++].indice )->alfabetica;
   int bits = pasa_a_int(v_memo( prog[p++].indice )->numerica);
   int sbits = pasa_a_int(v_memo( prog[p++].indice )->numerica);
   int nbuf = pasa_a_int(v_memo( prog[p++].indice )->numerica);
   char tmp[128];
   int r = 0;
   sprintf(tmp,"%s:%ld,%c,%d,%d,%d",com,baud,*par,bits,sbits,nbuf);
   

#ifdef RWSERVER
   if (en_conexion_externa)
   {
       funcion_windows_cliente(36,tmp,strlen(tmp)+1);
       r = (*((int *)tmp));
   }
   else
   {   
#endif
#ifdef FGVWIN
	   {
		  HANDLE PortInitialize (LPTSTR lpszPortName);          
		  r = (int)PortInitialize(tmp);
	   }
#endif
#ifdef RWSERVER
   }
#endif
   sprintf(v_memo(5004)->alfabetica,"%ld",r);

#ifdef ESDOS
#ifndef FGV32
   int c = -1;
   if (!strcmp(com,"COM1"))
						c = 0;
   else
						if (!strcmp(com,"COM2"))
                     c = 1;
                  if (c > -1)
                     c = inicio_com(c,nbuf,baud,par,bits,sbits);
                  sprintf(v_memo(5004)->alfabetica,"%d",c);
#endif
#endif
                  n_np = 6;
                  return(0);
}
/*
Parametros: puerto,bytes a leer,variable alfa(destino)
"LEECOM"         ,"959395"          ,137, NULL,
*/
int rw_leecom(p,tip,faclin)
int p;
int tip;
int faclin;
{
   char *com = v_memo( prog[p++].indice )->alfabetica;
   int max_ = pasa_a_int(v_memo( prog[p++].indice )->numerica);
   int i_memo = prog[p++].indice;   
   char tmp[2048];
   int r;
   
#ifdef RWSERVER
   if (en_conexion_externa)
   {	   
	   int p;
	   p = atol(com);
	   memcpy(tmp,&p,sizeof(int));
	   memcpy(tmp+sizeof(int),&max_,sizeof(int));
       funcion_windows_cliente(37,tmp,sizeof(int)*2);
	   r = (*((int *)tmp));
   }
   else
   {   
#endif
#ifdef FGVWIN
	   {
		  int LeePort(HANDLE hPort,int max,char *buf);
		  r = LeePort((HANDLE)atol(com),max_,tmp+sizeof(int));
	   }
#endif
#ifdef RWSERVER
   }
#endif
   if (r > 0)
   {
	   memo_strcpy(i_memo,tmp+sizeof(int));
   }
   sprintf(v_memo(5004)->alfabetica,"%ld",r);

#ifdef ESDOS
#ifndef FGV32
   int c = -1;
   char *buf = v_memo( i_memo )->alfabetica;  
   if (!strcmp(com,"COM1"))
      c = 0;
   else
      if (!strcmp(com,"COM2"))
         c = 1;
      if (c > -1)
      {
         free(buf);
         buf = malloc(max_+1);
         v_memo( i_memo )->alfabetica = buf;
         c = lee_com(c,buf,max_,0);
      }
      sprintf(v_memo(5004)->alfabetica,"%d",c);
#endif
#endif
      n_np = 3;
      return(0);
}
/*
Parametros: puerto,variable alfa(origen)
"GRABACOM"       ,"9595"            ,138, NULL,
*/
int rw_grabacom(p,tip,faclin)
int p;
int tip;
int faclin;
{
   char *com = v_memo( prog[p++].indice )->alfabetica;
   char *buf = v_memo( prog[p++].indice )->alfabetica;
   char tmp[2048];
   int r;

#ifdef RWSERVER
   if (en_conexion_externa)
   {	   
	   int p;
	   p = atol(com);
	   memcpy(tmp,&p,sizeof(int));	   
	   strcpy(tmp+sizeof(int),buf);
       funcion_windows_cliente(38,tmp,sizeof(int)+strlen(tmp+sizeof(int))+1);
	   r = (*((int *)tmp));
   }
   else
   {   
#endif
#ifdef FGVWIN
	   {
		  void PortWrite (HANDLE hPort,BYTE Byte);
		  PortWrite((HANDLE)atol(com),buf);
		  r = 0;
	   }
#endif
#ifdef RWSERVER
   }
#endif
   sprintf(v_memo(5004)->alfabetica,"%ld",r);


#ifdef ESDOS
#ifndef FGV32
   int c = -1;
   if (!strcmp(com,"COM1"))
      c = 0;
   else
      if (!strcmp(com,"COM2"))
									c = 1;
      if (c > -1)
      {
									c = graba_com(c,buf);
      }
      sprintf(v_memo(5004)->alfabetica,"%d",c);
#endif
#endif
      n_np = 2;
      return(0);
}

/*
Parametros: puerto
"FINCOM"         ,"95"              ,136, NULL,
*/

int rw_fincom(p,tip,faclin)
int p;
int tip;
int faclin;
{
   char *com = v_memo( prog[p++].indice )->alfabetica;
   int r;

#ifdef RWSERVER
   if (en_conexion_externa)
   {   
	   r = atol(com);
       funcion_windows_cliente(39,&r,sizeof(int));	   
   }
   else
   {   
#endif
#ifdef FGVWIN
	   {
		  BOOL PortClose (HANDLE hPort);		  
		  r = (int)PortClose ((HANDLE)atol(com));
	   }
#endif
#ifdef RWSERVER
   }
#endif
   sprintf(v_memo(5004)->alfabetica,"%ld",r);

#ifdef ESDOS
#ifndef FGV32
   int c = -1;
   if (!strcmp(com,"COM1"))
      c = 0;
   else
      if (!strcmp(com,"COM2"))
         c = 1;
      if (c > -1)
      {
         c = fin_com(c);
      }
      sprintf(v_memo(5004)->alfabetica,"%d",c);
#endif
#endif
      n_np = 1;
      return(0);
}




int rw_ontimer(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int max_ = pasa_a_int(v_memo( prog[p++].indice )->numerica);
   char *com = v_memo( prog[p++].indice )->alfabetica;
#ifdef ESDOS
#ifndef FGV32
   pon_delay_timer(max_);
   pon_par_timer(com);
   sprintf(v_memo(5004)->alfabetica,"%d",activa_timer());
#endif
#endif
   n_np = 2;
   return(0);
}

/*
int rw_fintimer(p,tip,faclin)
int p;
int tip;
int faclin;
{
#ifdef ESDOS
#ifndef FGV32
   sprintf(v_memo(5004)->alfabetica,"%d",desactiva_timer());
#endif
#endif
   n_np = 1;
   return(0);
}
*/


int rw_entorno(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int i;
   char *tmp;
   if (( i = memoria_alfa(p)) >= 0) {
      tmp = getenv(v_memo(i)->alfabetica);
      if (!tmp) tmp = "";
      if (no_memo_interna(i)) {
         free(v_memo(i)->alfabetica);
         v_memo(i)->alfabetica = strmalloc(strlen(tmp)+1);
      }
      strcpy(v_memo(i)->alfabetica,tmp);
   }
   n_np = 1;
   return(0);
}


int rw_ptec(p,tip,faclin)
int p;
int tip;
int faclin;
{
   ptec(pasa_a_int(v_memo(prog[p].indice)->numerica));
   return(0);
}


int rw_colodef(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int a1 = prog[p++].indice;
   int c1 = pasa_a_int(v_memo( prog[p++].indice )->numerica);
   int f1 = pasa_a_int(v_memo( prog[p++].indice )->numerica);
   int a2 = prog[p++].indice;
   int c2 = pasa_a_int(v_memo( prog[p++].indice )->numerica);
   int f2 = pasa_a_int(v_memo( prog[p++].indice )->numerica);
   int a3 = prog[p++].indice;
   int c3 = pasa_a_int(v_memo( prog[p++].indice )->numerica);
   int f3 = pasa_a_int(v_memo( prog[p++].indice )->numerica);
   
   if (a1 == ' ' || a1 == 'N' || es_atri(a1)) {
      if (a1 == 'N') a1 = ' ';
      rentra_sal_atrib = a1;
      rentra_sal_cc = c1;
      rentra_sal_cf = f1;
   }
   if (a2 == ' ' || a2 == 'N' || es_atri(a2)) {
      if (a2 == 'N') a2 = ' ';
      rentra_atrib = a2;
      rentra_cc = c2;
      rentra_cf = f2;
   }
   if (a3 == ' ' || a3 == 'N' || es_atri(a3)) {
      if (a3 == 'N') a3 = ' ';
      redita_atrib = a3;
      redita_cc = c3;
      redita_cf = f3;
   }
   n_np = 9;
   return(0);
}


static char *_bufsalvadatos[150];
static short _lensalvadatos[150];
static int _nsalvadatos = 0;
static long corriente_salvado[150];
static int clave_salvado[150];


int salva_datos(FI_DEF *pfi,int ficha)
{
   
   if (_nsalvadatos < 150)
   {
      _bufsalvadatos[_nsalvadatos] = malloc(pfi->f_lfich+1);
      memcpy(_bufsalvadatos[_nsalvadatos],pfi->f_buf,pfi->f_lfich);
      _lensalvadatos[_nsalvadatos]=pfi->f_lfich;
      if (ficha)
      {
         corriente_salvado[_nsalvadatos] = da_registo_actual(pfi);
         clave_salvado[_nsalvadatos] = ind_clave_activa(pfi);
      }
      else
         corriente_salvado[_nsalvadatos] = 0L;
      _nsalvadatos++;
	  return _nsalvadatos;      
   }
   return -1;
}

int rw_salvadatos(p,tip,faclin)
int p;
int tip;
int faclin;
{
   FI_DEF *pfi = v_mpfi(v_prog(prog[p].indice));
   int ficha = 0;
   
   if (prog[p].i == 207)
      ficha = 1;

   sprintf(v_memo(5004)->alfabetica,"%d",salva_datos(pfi,ficha));

   n_np = 1;
   return(0);
}


int repon_datos(FI_DEF *pfi,int ficha)
{
   if (_nsalvadatos && _lensalvadatos[_nsalvadatos-1] == pfi->f_lfich)
   {      
      _nsalvadatos--;
      if (ficha && pfi->f_fd > -1)
      {	 
		 if (corriente_salvado[_nsalvadatos] <= 0)
		 {
			 cambia_ind(pfi,clave_salvado[_nsalvadatos]);
		 }
		 else
		 {
			 pon_registro_actual(pfi,corriente_salvado[_nsalvadatos]);
			 apunta_ind(pfi,0,ISEQUAL,0);
			 apunta_ind(pfi,clave_salvado[_nsalvadatos],ISCURR,0);
		 }
      }
      memcpy(pfi->f_buf,_bufsalvadatos[_nsalvadatos],pfi->f_lfich);
      free(_bufsalvadatos[_nsalvadatos]);  

	  return _nsalvadatos+1;
   }
   return -1;
}

int rw_repondatos(p,tip,faclin)
int p;
int tip;
int faclin;
{
   FI_DEF *pfi = v_mpfi(v_prog(prog[p].indice));
   int ficha = 0;
   
   if (prog[p].i == 208)
      ficha = 1;

   sprintf(v_memo(5004)->alfabetica,"%d",repon_datos(pfi,ficha));
   
   n_np = 1;
   return(0);
}

int rw_mddirec(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int i;
   char tmp[MAXPATH];
   if (( i = memoria_alfa(p)) >= 0) {
      strcpy(tmp,rw_directorio(v_memo(i)->alfabetica));
      memo_strcpy(i,tmp);
   }
   n_np = 1;
   return(0);
}


int rw_agiver(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int f;
   if ((f = memoria_alfa(p)) > -1)
   {
      memo_strcpy(f,((fecha_protegida) ? "DEMO" : ""));
   }
   return(0);
}

int agi_topedemo(p,tip,faclin)
int p;
int tip;
int faclin;
{
   
   max_prote_reg = v_memo( prog[p].indice )->numerica;
   
   return(0);
}

int agi_alt_fich(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int f;
   if ((f = memoria_alfa(p)) > -1)
   {
      if (alt_dirfic)
         strcpy(v_memo(f)->alfabetica,alt_dirfic);
      else
         v_memo(f)->alfabetica[0] = 0;
   }
   n_np=1;
   return(0);
}


static int read_multi = 0;
static int real_close = 0;
static unsigned char multi_fichero[128];
extern int  ifd;

static unsigned read_multiple(pfd,buf,tam)
int *pfd;
unsigned char *buf;
unsigned tam;
{
   int fd = *pfd;
   int i;
   unsigned r;
   unsigned leido = 0;
   
   if (fd < 0) return(0);
   
   do
   {
      
      r = rw_base_read_(fd,buf+leido,tam-leido);
      if ((int)r != -1)
      {
         leido += r;
      }
      else
      {
         leido = r;
      }
      if((int)leido != -1 && leido < tam && read_multi)
      {
         i = strlen(multi_fichero);
         if (i)
         {
            i--;
            (multi_fichero[i])++;
            rw_base_close_(fd);
            real_close = -1;
            ifd = -1;
            fd = rw_base_open_(multi_fichero,O_RDONLY | O_BINARY);
            if (fd >= 0)
            {
               *pfd = fd;
               real_close = fd;
               ifd = fd;
               continue;
            }
            else
            {
               *pfd = -1;	  
            }
         }
      }
      break;
   } while(-1);
   
   return(leido);
}

#ifdef FGVUNIX
extern rw_base_write_();
#endif

int agi_compress(fichero,modo)
char *fichero;
int modo;
{
   void (*_write_interno)() = write_interno;
   unsigned (*_read_interno)() = read_interno;
   int r,fin,fout,i,pp;
   char tmp1[255];
   
   
   read_multi = 0;
   write_interno = rw_base_write_; /* ojo falta control de multiple */
   read_interno = read_multiple;
   
   fin = -1;
   fout = -1;
   real_close = -1;
   if (modo)
   {   
      strcpy(tmp1,fichero);
      pp = -1;
      for (i = 0;tmp1[i];i++)
      {
         if (tmp1[i] == '.')
         {
            pp = i;		  
         }   
      }
      /* probar gz */
      if (pp == -1)
      {
         pp = strlen(tmp1);
         strcat(tmp1,".tgz");
         fin = rw_base_open_(tmp1,O_RDONLY | O_BINARY);
         if (fin < 0)
         {
            strcpy(tmp1+pp,".tg0");
            fin = rw_base_open_(tmp1,O_RDONLY | O_BINARY);
            if (fin < 0)
            {
               strcpy(tmp1+pp,".gz");
               fin = rw_base_open_(tmp1,O_RDONLY | O_BINARY);
               if (fin < 0)
               {
                  strcpy(tmp1+pp,".Z01");
                  fin = rw_base_open_(tmp1,O_RDONLY | O_BINARY);
                  if (fin < 0)
                  {
                     strcpy(tmp1+pp,".Z");
                     fin = rw_base_open_(tmp1,O_RDONLY | O_BINARY);
                  }
                  else
                  {
                     strcpy(multi_fichero,tmp1);
                     read_multi = 1;
                  }
               }
            }
            else
            {
               strcpy(multi_fichero,tmp1);
               read_multi = 1;
            }
         }
      }
      else
      {
         if (!strcmp(tmp1+pp,".tar"))		  
            strcpy(tmp1+pp,".tgz");		  
         else
         {
            if (!strcmp(tmp1+pp,".tg0"))
               read_multi = 1;
            else
               if (!strcmp(tmp1+pp,".Z01"))
                  read_multi = 1;
               else
                  if (strcmp(tmp1+pp,".tgz") && strcmp(tmp1+pp,".gz") && strcmp(tmp1+pp,".Z"))
                     strcat(tmp1,".gz");
         }
         fin = rw_base_open_(tmp1,O_RDONLY | O_BINARY);
      }
      
      if (fin >= 0)
      {
         if (!strcmp(tmp1+pp,".tgz") || !strcmp(tmp1+pp,".tg0"))
         {
            strcpy(tmp1+pp,".tar");
         }
         else
         {
            tmp1[pp] = 0;
         }
         fout = rw_base_open_(tmp1,O_RDWR | O_BINARY | O_CREAT | O_TRUNC,S_IREAD | S_IWRITE);
         if (fout < 0)
         {
            rw_base_close_(fin);
         }
      }
   }
   else
   {
      fin = rw_base_open_(fichero,O_RDONLY | O_BINARY);
      if (fin >= 0)
      {
         strcpy(tmp1,fichero);
         pp = -1;
         for (i = 0;tmp1[i];i++)
         {
            if (tmp1[i] == '.')
            {
               pp = i;		  
            }   
         }
         if (pp != -1)
         { 
            if (!strcmp(tmp1+pp,".tar"))
            {
               strcpy(tmp1+pp,".tgz");
            }
            else
            {
               tmp1[pp] = 0;
               strcat(tmp1,".gz");
            }
         }
         else
            strcat(tmp1,".gz");
         fout = rw_base_open_(tmp1,O_RDWR | O_BINARY | O_CREAT | O_TRUNC,S_IREAD | S_IWRITE);
         if (fout < 0)
            rw_base_close_(fin);	 
      }
   }
   
   if (fin >= 0 && fout >= 0)
   {
      real_close = fin;
      r = ds___gzip(fin,fout,modo,6);
      
      if (real_close >= 0)
         rw_base_close_(real_close);
      rw_base_close_(fout);
   }
   else
      r = -1;
   
   write_interno = _write_interno;
   read_interno = _read_interno;
   return(r);
}

int agicomprime(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int f;
   if ((f = memoria_alfa(p)) > -1)
   {
      int externo = 0;
      
      if (prog[p].i == 255)
      {
         externo = 1;
      }
#ifdef RWSERVER
      if (externo && en_conexion_externa)
      {
         int sal = 0;
         char tmp[301];
         strcpy(tmp,v_memo( f )->alfabetica);     
         funcion_windows_cliente(25,tmp,strlen(tmp)+1);
         memcpy_int(&sal,tmp,sizeof(int));
         sprintf(v_memo(5004)->alfabetica,"%d",sal);
         n_np = 1;
         return(0);
      }
#endif
      sprintf(v_memo(5004)->alfabetica,"%d",agi_compress(v_memo(f)->alfabetica,0));
   }
   n_np = 1;
   return(0);
}

int agidescomprime(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int f;
   int modo = 1;
   if ((f = memoria_alfa(p)) > -1)
			{
      int externo = 0;
      
      if (prog[p].i == 255)
      {
         externo = 1;
      }
#ifdef RWSERVER
      if (externo && en_conexion_externa)
      {
         int sal = 0;
         char tmp[301];
         strcpy(tmp,v_memo( f )->alfabetica);     
         funcion_windows_cliente(26,tmp,strlen(tmp)+1);
         memcpy_int(&sal,tmp,sizeof(int));
         sprintf(v_memo(5004)->alfabetica,"%d",sal);
         n_np = 1;
         return(0);
      }
#endif
	  {
		  char *pp;
		  pp = v_memo(f)->alfabetica;
		  if (*pp && *(pp+1))
		  {
			 pp += strlen(pp);
			 pp--;
			 if (*pp == '?')
			 {
				 pp--;
				 if (*pp == ' ')
				 {
					 *pp = 0;
					 modo = 10;
				 }
			 }
		  }
	  }
      sprintf(v_memo(5004)->alfabetica,"%d",agi_compress(v_memo(f)->alfabetica,modo));
			}
   n_np = 1;
   return(0);
}

int agidetar(p,tip,faclin)
int p;
int tip;
int faclin;
{
			int f1,f2;
         if ((f1 = memoria_alfa(p)) > -1 && (f2 = memoria_alfa(p+1)) > -1)
         {
            int externo = 0;
            
            if (prog[p].i == 255)
            {
               externo = 1;
            }
#ifdef RWSERVER
            if (externo && en_conexion_externa)
            {
               int sal = 0;
               char tmp[601];
               strcpy(tmp,v_memo( f1 )->alfabetica);
               strcpy(tmp+300,v_memo( f2 )->alfabetica);
               funcion_windows_cliente(27,tmp,600);
               memcpy_int(&sal,tmp,sizeof(int));
               sprintf(v_memo(5004)->alfabetica,"%d",sal);
               n_np = 2;
               return(0);
            }
#endif
            sprintf(v_memo(5004)->alfabetica,"%d",agi_detar(v_memo(f1)->alfabetica,v_memo(f2)->alfabetica));
         }
         n_np = 2;
         return(0);
}


int agiatar(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int f1,f2;
   if ((f1 = memoria_alfa(p)) > -1 && (f2 = memoria_alfa(p+1)) > -1)
   {
      int externo = 0;
      
      if (prog[p].i == 255)
						{
         externo = 1;
						}
#ifdef RWSERVER
      if (externo && en_conexion_externa)
						{
         int sal = 0;
         char tmp[601];
         strcpy(tmp,v_memo( f1 )->alfabetica);
         strcpy(tmp+300,v_memo( f2 )->alfabetica);
         funcion_windows_cliente(28,tmp,600);
         memcpy_int(&sal,tmp,sizeof(int));
         sprintf(v_memo(5004)->alfabetica,"%d",sal);
         n_np = 1;
         return(0);
						}
#endif
      sprintf(v_memo(5004)->alfabetica,"%d",agi_atar(v_memo(f1)->alfabetica,v_memo(f2)->alfabetica));
   }
   n_np = 2;
   return(0);
}


int agiponclave(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int f1,f2,f3;
   f1 = prog[p++].indice;
   f2 = prog[p++].indice;
   f3 = prog[p++].indice;
   sprintf(v_memo(5004)->alfabetica,"%d",pon_clave(v_memo(f1)->alfabetica,pasa_a_int(v_memo(f2)->numerica),pasa_a_int(v_memo(f3)->numerica)));
   n_np = 3;
   return(0);
}


static int control_menu_cargado = 0;
int agi_cargawmenu(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int r = -1;
   int f1,f2,f3;
   char tmp[260];
   f1 = prog[p++].indice;
   f2 = prog[p++].indice;
   f3 = prog[p++].indice;
   strcpy(tmp,v_memo(f1)->alfabetica);
   if (wmenu) 
   {
      if (!strlen(tmp))
      {
         memo_strcpy(f1,fichemenu);
         v_memo(f2)->numerica = (double)nmenus;
         strcpy(v_memo(5004)->alfabetica,nommenu);
      }
   }
   else
   {  
      if (strlen(tmp))
      {
         if (tmp == quita_path(tmp))
									{
            strcpy(tmp,dirbase);
            strcat(tmp,v_memo(f1)->alfabetica);
									}
         else
									{
            if (tmp[0] == ':' && tmp[1] == '/')
            {
               strcpy(tmp,dirbass);
               strcat(tmp,v_memo(f1)->alfabetica+2);
            }
									}
         r = leemenu(tmp);    
      }
      if (r < 0)
      {
         fichemenu[0] = 0;
         nommenu[0] = 0;
         v_memo(f2)->numerica = (double)0;
      }
      else
      {
         v_memo(f2)->numerica = (double)nmenus;
         v_memo(f3)->numerica = (double)wdef;
         control_menu_cargado = 1;
      }
      memo_strcpy(f1,fichemenu);
      strcpy(v_memo(5004)->alfabetica,nommenu);
   }
   n_np = 3;
   return(0);  
}

int agi_liberawmenu(p,tip,faclin)
int p;
int tip;
int faclin;
{
   if (control_menu_cargado)
   {  
      libera();
      control_menu_cargado = 0;
      strcpy(v_memo(5004)->alfabetica,"0");
      n_np = 1;
      return(0);
   }
   strcpy(v_memo(5004)->alfabetica,"-1");
   n_np = 1;
   return(0);
}

int agi_wmenu(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int r;
   int m;
   int f1,f2,f3;
   char tmp[260];
   f1 = prog[p++].indice;
   f2 = prog[p++].indice;
   f3 = prog[p++].indice;
   
   if (control_menu_cargado && wmenu)
   {
      wdef = pasa_a_int(v_memo(f1)->numerica);
      r = w_menus(&m);
      if (r > 0)
      {
         wdef = m * 100 + r;
         v_memo(f1)->numerica = (double)wdef;
         memo_strcpy(f2,wmenu[wdef/100]->m_op[(wdef%100)-1]->o_nombre);
         memo_strcpy(f3,wmenu[wdef/100]->m_op[(wdef%100)-1]->o_programa);
      }
      else
         v_memo(f1)->numerica = (double)-1;
      strcpy(v_memo(5004)->alfabetica,"0");
   }
   else
   {
      strcpy(v_memo(5004)->alfabetica,"-1");
   }
   
   n_np = 3;
   return(0);
}

int agi_nuevawopcion(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int f1,f2,f3,f4,f5,flag,m,o,n,i;
   T_OPCION **tmp1;
   char *nom,*pro;
   
   f1 = prog[p++].indice;
   f2 = prog[p++].indice;
   f3 = prog[p++].indice;
   f4 = prog[p++].indice;
   f5 = prog[p++].indice;
   
   strcpy(v_memo(5004)->alfabetica,"-1");
   if (control_menu_cargado || !wmenu)
   {  
      flag = pasa_a_int(v_memo(f1)->numerica);
      m = pasa_a_int(v_memo(f2)->numerica);
      o = pasa_a_int(v_memo(f3)->numerica);  
      if (!wmenu)
      {
         nmenus = 0;
         if (m == 0 && o == 0 && !flag)
         {
												wmenu = (T_MENU **)malloc(sizeof(T_MENU *) * (nmenus + 2));
                                    wmenu[0] = (T_MENU *)malloc(sizeof(T_MENU));
                                    wmenu[0]->m_nop = 0;
                                    wmenu[0]->m_op = (T_OPCION **)malloc(sizeof(T_OPCION *));
                                    wmenu[0]->m_menupadre = -1;
                                    strcpy(nommenu,v_memo(f4)->alfabetica);
                                    strcpy(fichemenu,v_memo(f5)->alfabetica);
                                    control_menu_cargado = 1;      
                                    nmenus++;
                                    sprintf(v_memo(5004)->alfabetica,"%d",nmenus);
         }
      }
      else
         if (m < nmenus)
         {
												n = wmenu[m]->m_nop;
                                    if (o <= n)
                                    {
                                       nom = v_memo(f4)->alfabetica;
                                       pro = v_memo(f5)->alfabetica;
                                       n++;
                                       tmp1 = wmenu[i]->m_op;
                                       wmenu[m]->m_op = (T_OPCION **)malloc(n*sizeof(T_OPCION *));
                                       for (i = (n-1);i > o;i--)
                                       {
                                          wmenu[m]->m_op[i] = tmp1[i-1];
                                       }
                                       wmenu[m]->m_op[o] = (T_OPCION *)malloc(sizeof(T_OPCION));
                                       for (i = o;i > 0;i--)
                                       {
                                          wmenu[m]->m_op[i-1] = tmp1[i-1];
                                       }
                                       free(tmp1);
                                       wmenu[m]->m_nop = n;
                                       
                                       wmenu[m]->m_op[o]->o_nombre = malloc(strlen(nom)+1);
                                       strcpy(wmenu[m]->m_op[o]->o_nombre,nom);
                                       
                                       if (flag)
                                       {
                                          wmenu[m]->m_op[o]->o_programa = malloc(5);
                                          *(wmenu[m]->m_op[o]->o_programa) = '\0';
                                          wmenu[m]->m_op[o]->o_menuhijo = nmenus;		
                                          wmenu = (T_MENU **)realloc(wmenu,sizeof(T_MENU *) * (nmenus + 2));
                                          wmenu[nmenus] = (T_MENU *)malloc(sizeof(T_MENU));      
                                          wmenu[nmenus]->m_nop = 1;
                                          wmenu[nmenus]->m_menupadre = m;
                                          wmenu[nmenus]->m_fqp       = 7;
                                          wmenu[nmenus]->m_cqp       = 5;
                                          wmenu[nmenus]->m_op = (T_OPCION **)malloc(sizeof(T_OPCION *));
                                          wmenu[nmenus]->m_op[0] = (T_OPCION *)malloc(sizeof(T_OPCION));
                                          wmenu[nmenus]->m_op[0]->o_nombre = malloc(strlen(pro)+1);
                                          strcpy(wmenu[nmenus]->m_op[0]->o_nombre,pro);
                                          wmenu[nmenus]->m_op[0]->o_programa = malloc(5);
                                          *(wmenu[nmenus]->m_op[0]->o_programa) = '\0';
                                          wmenu[nmenus]->m_op[0]->o_menuhijo = -1;
                                          nmenus++;
                                       }
                                       else
                                       {
                                          wmenu[m]->m_op[o]->o_programa = malloc(strlen(pro)+1);
                                          strcpy(wmenu[m]->m_op[o]->o_programa,pro);
                                          wmenu[m]->m_op[o]->o_menuhijo = -1;
                                       }
                                       sprintf(v_memo(5004)->alfabetica,"%d",nmenus); 
                                    }
         }  
   }  
   n_np = 5;
   return(0);
}


static int borra_w_hoja(hoja)
int hoja;
{
   int i,hijo;
   if (hoja >= nmenus || !wmenu[hoja]) return(-1);
   for (i = 0;i < wmenu[hoja]->m_nop;i++)
   {
      if (!wmenu[hoja]->m_op[i]) continue;
      hijo = wmenu[hoja]->m_op[i]->o_menuhijo;
      if (hijo != -1)
      {
         borra_w_hoja(hijo);
      }
      if (wmenu[hoja]->m_op[i]->o_nombre)
         free(wmenu[hoja]->m_op[i]->o_nombre);
      if (wmenu[hoja]->m_op[i]->o_programa)
         free(wmenu[hoja]->m_op[i]->o_programa);
      free(wmenu[hoja]->m_op[i]);	 
      wmenu[hoja]->m_op[i] = NULL;
   }
   free(wmenu[hoja]->m_op);   
   free(wmenu[hoja]);
   wmenu[hoja] = NULL;
   return(0);
}


static int cuenta_w_nuevo(hijo)
int hijo;
{
   int i;
   int n = 0;
   
   if (!wmenu[hijo]) return(-1);
   
   for (i = 0;i < hijo;i++)
   {
      if (wmenu[i]) n++;
   }
   
   return(n);
}

static int reorganiza_w_hojas()
{
   int i,m,hijo;
   
   if (!wmenu[0]) 
   {
      nmenus = 0;
      free(wmenu);
      wmenu = NULL;
      control_menu_cargado = 0;
   }
   
   for (m = 0;m < nmenus;m++)
   { 
      if (!wmenu[m]) continue;
      hijo = wmenu[m]->m_menupadre;
      if (hijo > 0)
         wmenu[m]->m_menupadre = cuenta_w_nuevo(hijo);
      for (i = 0;i < wmenu[m]->m_nop;i++)
      {
         hijo = wmenu[m]->m_op[i]->o_menuhijo;
         if (hijo != -1)
         {
            wmenu[m]->m_op[i]->o_menuhijo = cuenta_w_nuevo(hijo);
         }
      }
   }
   
   for (m = 0;m < nmenus;m++)
   { 
      while(m < nmenus && !wmenu[m])
      {
         nmenus--; 
         for (i = m;i < nmenus;i++)
            wmenu[i] = wmenu[i+1];    
      }
   }
   
   if (!wmenu[0]) 
   {
      nmenus = 0;
      free(wmenu);
      wmenu = NULL;
      control_menu_cargado = 0;
   }
   
   return(0);
}

int agi_borrawopcion(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int f1,f2,m,o,i,hijo;
   
   strcpy(v_memo(5004)->alfabetica,"-1");
   if (control_menu_cargado && wmenu)
   {
      f1 = prog[p++].indice;
      f2 = prog[p++].indice;
      m = pasa_a_int(v_memo(f1)->numerica);
      o = pasa_a_int(v_memo(f2)->numerica);  
      if (m < nmenus && o > 0 && o <= wmenu[m]->m_nop)
      {
         o--;
         hijo = wmenu[m]->m_op[o]->o_menuhijo;
         if (wmenu[m]->m_op[o]->o_nombre)
            free(wmenu[m]->m_op[o]->o_nombre);
         if (wmenu[m]->m_op[o]->o_programa)
            free(wmenu[m]->m_op[o]->o_programa);
         free(wmenu[m]->m_op[o]);
         wmenu[m]->m_nop--;
         for (i = o;i < wmenu[m]->m_nop;i++)
         {
            wmenu[m]->m_op[i] = wmenu[m]->m_op[i+1];
         }
         if (hijo != -1)
         {
            borra_w_hoja(hijo);     
         }
         if (wmenu[m] && !wmenu[m]->m_nop)
         {
            hijo = m;
            borra_w_hoja(hijo);
         }
         if (hijo != -1)
         {
            /* a partir de aqui las actuales referencias (m) no sirven */
            reorganiza_w_hojas();
         }
      }
      sprintf(v_memo(5004)->alfabetica,"%d",nmenus); 
   }
   n_np = 2;
   return(0);
}


int agi_datoswhoja(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int f1,f2,f3,f4,f5;
   int flag,m,n,o,i;
   char tmp[81];
   char *pro,*nom;
   
   f1 = prog[p++].indice;
   f2 = prog[p++].indice;
   f3 = prog[p++].indice;
   f4 = prog[p++].indice;
   f5 = prog[p++].indice;
   strcpy(v_memo(5004)->alfabetica,"-1");
   if (control_menu_cargado && wmenu)
   {
      flag = pasa_a_int(v_memo(f1)->numerica);
      m = pasa_a_int(v_memo(f2)->numerica);
      o = pasa_a_int(v_memo(f3)->numerica);  
      if (flag == 2)
      {
         if (m < nmenus)
         {
            v_memo(f3)->numerica = (double)wmenu[m]->m_nop;
            n = wmenu[m]->m_menupadre;
            if (n != -1)
            {
               for (i = 0;i < wmenu[n]->m_nop;i++)
               {
																		if (wmenu[n]->m_op[i]->o_menuhijo == m)
                                                         break;
               }
               if (i < wmenu[n]->m_nop)
               {          
																		memo_strcpy(f4,wmenu[n]->m_op[i]->o_nombre);
                                                      sprintf(tmp,"%d",n*100+i+1);
                                                      memo_strcpy(f5,tmp);
                                                      strcpy(v_memo(5004)->alfabetica,"0");
               }
            }
            else
            {
               memo_strcpy(f4,nommenu);
               memo_strcpy(f5,"-1");
               strcpy(v_memo(5004)->alfabetica,"0");
            }
         }
      }
      else
         if (flag == 3)
         {
            v_memo(f2)->numerica = (double)nmenus;
            v_memo(f3)->numerica = (double)wdef;
            memo_strcpy(f4,nommenu);
            memo_strcpy(f5,fichemenu);
            strcpy(v_memo(5004)->alfabetica,"0");
         }
         else
            if (flag == 4)
            {
               strcpy(nommenu,v_memo(f4)->alfabetica);
               strcpy(fichemenu,v_memo(f5)->alfabetica);
            }
            else
               if (m < nmenus && o > 0 && o <= wmenu[m]->m_nop)
               {
																		o--;
                                                      if (flag == 1)
                                                      {
                                                         nom = v_memo(f4)->alfabetica;
                                                         pro = v_memo(f5)->alfabetica;
                                                         if (wmenu[m]->m_op[o]->o_nombre)
                                                            free(wmenu[m]->m_op[o]->o_nombre);
                                                         if (wmenu[m]->m_op[o]->o_programa)
                                                            free(wmenu[m]->m_op[o]->o_programa);
                                                         wmenu[m]->m_op[o]->o_nombre = malloc(strlen(nom)+1);
                                                         strcpy(wmenu[m]->m_op[o]->o_nombre,nom);
                                                         wmenu[m]->m_op[o]->o_programa = malloc(strlen(pro)+1);
                                                         strcpy(wmenu[m]->m_op[o]->o_programa,pro);
                                                         strcpy(v_memo(5004)->alfabetica,"0");
                                                      }
                                                      else
                                                      {
                                                         memo_strcpy(f4,wmenu[m]->m_op[o]->o_nombre);
                                                         memo_strcpy(f5,wmenu[m]->m_op[o]->o_programa);
                                                         strcpy(v_memo(5004)->alfabetica,"0");
                                                      }	
               }  
   }
   n_np = 5;
   return(0);
}


int agi_grabawmenu(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int f1,m,i;
   FILE *ff;
   char tmp[81];
   
   strcpy(v_memo(5004)->alfabetica,"-1");
   if (control_menu_cargado && wmenu)
   {
      f1 = prog[p++].indice;
      strcpy(tmp,v_memo(f1)->alfabetica);
      sprintf(v_memo(5004)->alfabetica,"%d",graba_fgvmenu(tmp));
      /* FORMATO ANTIGUO */
      /*
      strcat(tmp,".men");
      ff = fopen(tmp,"wt");
      if (ff)
      {         
         fprintf(ff,"0\n%s,%d\n",nommenu,nmenus);
         for (m = 0;m < nmenus;m++)
         {
            fprintf(ff,"%d,%d,%d,%d\n",wmenu[m]->m_nop,wmenu[m]->m_menupadre,wmenu[m]->m_fqp,wmenu[m]->m_cqp);
            for (i = 0;i < wmenu[m]->m_nop;i++)
            {	 
               fprintf(ff,"%-29s,%d,%s\n",wmenu[m]->m_op[i]->o_nombre,
                  wmenu[m]->m_op[i]->o_menuhijo,
                  wmenu[m]->m_op[i]->o_programa);
            }
         }
         
         fclose(ff);
         strcpy(v_memo(5004)->alfabetica,"0");
      }  
      */
   }
   n_np = 1;
   return(0);
}


int ag_validachk(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int t = pasa_a_int(v_memo( prog[p++].indice )->numerica);
   unsigned char *cla1 = v_memo( prog[p++].indice )->alfabetica;
   unsigned char *cla2 = v_memo( prog[p++].indice )->alfabetica;
   int i_memo = prog[p++].indice;
   unsigned char *pp = v_memo(i_memo)->alfabetica;
   int i;
   int n;
   int k;
   
   n_np=4;  
   return(0);
}

int ag_validaapl(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int t = pasa_a_int(v_memo( prog[p++].indice )->numerica);
   int cl1 = pasa_a_int(v_memo( prog[p++].indice )->numerica);
   int cl2 = pasa_a_int(v_memo( prog[p++].indice )->numerica);
   int n;
   
   
   n_np=3;  
   return(0);
}


static unsigned char *nocase_memchr(unsigned char *pbusca,unsigned char busca,int size)
{
	int factor = 0;
	if (busca >= 'A' && busca <= 'Z')
	{
		factor = 32;
	} else if (busca >= 'a' && busca <= 'z')
	{
		factor = -32;
	}

	while(size > 0)
	{
		if (*pbusca == busca || *pbusca == (busca+factor))
		{
			return pbusca;
		}
		pbusca++;
		size--;
	}
	return NULL;
}

static int nocase_memcmp(unsigned char *pbusca,unsigned char *busca,int ll)
{	
	int factor;
	while(ll > 0)
	{
		factor = 0;
		if (*busca >= 'A' && *busca <= 'Z')
		{
			factor = 32;
		} else if (*busca >= 'a' && *busca <= 'z')
		{
			factor = -32;
		}
		if (*pbusca != *busca && *pbusca != ((*busca)+factor))
		{
			return *pbusca - *busca;
		}
		pbusca++;
		busca++;
		ll--;
	}
	return 0;
}


char def_erp_actual[25] = "";
int ipfi_erp_actual = -1;
int campo_erp_actual = -1;
int llama_modulo_erp()
{
    extern char *p_erp_tab;
	extern int entra_util();   	
	static int recursion_erp = 0;
	char llamada[512];
	if (!p_erp_tab)
	{
		aviso(0,"ERP no instalado");
		return 0;
	}
	if (recursion_erp)
	{
		aviso(0,"ERP en ejecución");
		return 0;
	}
	recursion_erp++;
	llamada[0] = '|';
	strcpy(llamada+1,p_erp_tab);
	if (def_erp_actual[0] && campo_erp_actual > -1)
	{
		sprintf(llamada+strlen(llamada),";%s,%03d",def_erp_actual,campo_erp_actual);
	}
	es_el_modulo_erp = 1;
	llama_fun_int(entra_util,(char *)llamada);
	recursion_erp--;

	return 0;
}

int llama_evento_desc(void *nulo)
{
	  char tmp[128];
	  double vcf,vca;
	  int i;
	  int evento = (int)nulo;
	  
	  if (evento == ID_ACTIVAR_EVTPBX)
		  strcpy(tmp,"EVENTOPBX");
	  else if (evento == ID_ACTIVAR_EVTCRM)
		  strcpy(tmp,"EVENTOCRM");
	  else
		  strcpy(tmp,"EVENTOGRID");

	  for (i = 0;i < np_ex;i++)
	  {
		 if (strcmp(tmp,p_ex[i].nombre) == 0) 
		 {
               if (memoplin > 2) 
			   {
				  sprintf(v_memo(5004)->alfabetica,"%d %s",1771,def_erp_actual);
                  vca = v_memo(5000)->numerica;
                  vcf = v_memo(5011)->numerica;
                  v_memo(5000)->numerica = (double)campo_erp_actual;
                  v_memo(5011)->numerica = (double)ipfi_erp_actual;
               }
			   p_ejecuta(p_ex[i].indice,O_NOR,0,34);
               if (memoplin > 2) 
			   {
                  v_memo(5000)->numerica = vca;
                  v_memo(5011)->numerica = vcf;
               }			
			   return 1;
		 }
	  }		  
	  aviso(0,"Funcion no implementada.");
	  return 0;
}

int super_buscon(char *busca)
{
	char tmp[512];
	char tmx[512];
    char tmy[512];
	char tmt[256];
	char **dir;
	char **d;
	FI_DEF *pfi;
	int f,i;
	int n = 0;
	char *buffer;	
	int maxbuffer = 100000;
	int len;
	char *pbusca;
	int ll;
	int tamr;
	long off;
	long reg;
	long pos,p2,p3;
	FI_DEF *buscon;
	int salir;
	long puntobu = 0L;
	long espaciado,espaciador;
	long tot = 0L,progreso = 0L;
	extern long lee_direct_tot_tama;

	if (!busca)
	{
		/* despacho */
		tmy[0] = 0;	
		if (DespachadorExterno)
		{
			(*DespachadorExterno)((int)DESPACHO_MULTIPLE,(int)1,tmy);		
		}
		busca = tmy;
	}
	if (!*busca)
		return 0;

	ll = strlen(busca);

	if (ll < 4)
	{
		aviso(0,"MINIMO 4 CARACTERES");
		return 0;
	}

	strcpy(tmp,rw_directorio("agitool/def/dsbuscon"));
	buscon = NULL;
	if (lee_defn(tmp,&buscon) >= 0)
	{
        strcpy(buscon->f_fichero,"__XXXXXX");
        cambia_path_fich(buscon->f_fichero);
#ifndef ESDOS
		{
			int ff = mkstemp(buscon->f_fichero);
			if (ff >= 0)
				close(ff);
		}
#else
        mktemp(buscon->f_fichero);
#endif        
        {
            FILE *ff = fopen(buscon->f_fichero,"w");
            if (ff)
            {
                fclose(ff);
            }
        }
		borra_ind(buscon);
		crea_ind(buscon);


		strcpy(tmp,dirfic);
		strcat(tmp,"*.dat");
		dir = lee_direct(tmp,0);
		tot = lee_direct_tot_tama;
		libera_dir(dir);
		
		strcpy(tmp,dirdef);
		strcat(tmp,"*.mas");

		dir = lee_direct(tmp,0);
		if (dir)
		{
			espaciador = tot / 100;
			espaciado = espaciador;
			if (DespachadorExterno)
			{
				sprintf(tmt,"%ld",100);
				(*DespachadorExterno)((int)DESPACHO_MULTIPLE,(int)2,tmt);
			}

			buffer = malloc(maxbuffer+512);
			memset(buffer,0,maxbuffer+512);
			d = dir;
			while(*d)
			{			
				pfi = NULL;
				strcpy(tmp,dirdef);
				strcat(tmp,*d);
				if (lee_defn(tmp,&pfi) >= 0)
				{
					tamr = pfi->f_lfich + 1;
					defecto_ficha(buscon);
					st_campo(4,buscon,pfi->f_PathDef);
					cambia_path_fich(pfi->f_fichero);
					st_campo(5,buscon,pfi->f_fichero);
					f = abre_directo(pfi,pfi->f_fichero);
					if (f >= 0)
					{
						off = 0L;
						while ((len = rw_base_read_(f,buffer,maxbuffer)) > 0)
						{
							puntobu += len;
							if (puntobu >= espaciado && DespachadorExterno)
							{
								espaciado = puntobu + espaciador;
								progreso++;
								sprintf(tmt,"%ld",progreso);
								(*DespachadorExterno)((int)DESPACHO_MULTIPLE,(int)3,tmt);
							}							

							p3 = rw_base_lseek_(f,0L,SEEK_CUR) - ll;
							pbusca = buffer;
							while( (pbusca = nocase_memchr(pbusca,*busca,len-(int)(pbusca-buffer))) )
							{
								if (!nocase_memcmp(pbusca,busca,ll))
								{
									pos = off + (int)(pbusca-buffer);									
									reg = (pos / tamr);
									p2 = reg * tamr;
									reg++;
									n++;
									
									lee_directo(f,reg,pfi,pfi->f_buf);

									sprintf(tmp,"%d",n);
									st_campo(0,buscon,tmp);

									sprintf(tmp,"%ld",reg);
									st_campo(1,buscon,tmp);

									for (i = 0;i < pfi->f_ncampos;i++)
									{										
										p2 += pfi->f_campos[i].c_long;
										if (p2 > pos)
											break;
									}
									if (i >= pfi->f_ncampos)
									{
										i = 0;
									}

									ld_campo(i,pfi,tmp);
									st_campo(2,buscon,tmp);

									tmx[0] = 0;
									if (pfi->f_nclaves > 0)
									{
										for (i = 0;i < pfi->f_clave[0].k_ncampos;i++)
										{
											ld_campo(pfi->f_clave[0].k_campo[i]-1,pfi,tmx+strlen(tmx));
										}
									}
									st_campo(3,buscon,tmx);

									graba_nuevo(buscon);

									pbusca += ll;
								}
								else
								{
									pbusca++;
								}
								if (pbusca >= (buffer+len))
									break;
							}
							if (len < maxbuffer)
								break;
							rw_base_lseek_(f,p3,SEEK_SET);
							off += (len-ll);
						}
						cierra_directo(f);
					}
					free_defpri(pfi);
				}
				d++;
			}
			if (DespachadorExterno)
			{
				sprintf(tmt,"%ld",100);
				(*DespachadorExterno)((int)DESPACHO_MULTIPLE,(int)4,tmt);
			}
			libera_dir(dir);
			free(buffer);
		}

		
		do
		{
			salir = 1;
			if (!consul_clave_grid(buscon,0,1,0,0,0,0,-1,-1,NULL))
			{
				ld_campo(4,buscon,tmx);
				quitab(tmx);
				pfi = NULL;
				if (lee_defn(tmx,&pfi) >= 0)
				{
					ld_campo(5,buscon,tmx);
					quitab(tmx);
					strcpy(pfi->f_fichero,tmx);
					if (abre_ind(pfi) >= 0)
					{
						ld_campo(1,buscon,tmx);
						pon_registro_actual(pfi,atol(tmx));
						apunta_ind(pfi,0,ISEQUAL,0);
						apunta_ind(pfi,1,ISCURR,0);
						lee_reg(pfi,ISCURR);
						if (consul_clave_grid(pfi,0,1,0,0,0,0,-1,-1,NULL))
						{
							salir = 0;
						}
						cierra_ind(pfi);
					}
					free_defpri(pfi);
				}
			}
		} while(!salir);

		cierra_ind(buscon);
		borra_ind(buscon);
		rw_base_unlink(buscon->f_fichero);
		free_defpri(buscon);
	}
	return n;
}



static int nlbarra = 0;
static int tbarra[500];
static int lbarra,ntcortes,*pcorte,lminimo;
static int barra[500];

static int cambia_por_la_mejor_barra(int lonbarra)
{
	int i;
	int ok = 0;
	if (lonbarra == lbarra) /* si coincide la cogemos sin pensar */
	{
		nlbarra = lonbarra;
		i = 0;
		while(barra[i] != -1)
		{
			tbarra[i] = barra[i];
			i++;
		}
		tbarra[i] = barra[i];
		return 1; /* nada mas que buscar */
	}

	if (lonbarra > nlbarra)
	{
		if (nlbarra > lminimo)
		{
			ok = 1;
		}
		else
		{
			if (lonbarra <= lminimo)
			{
				ok = 1;
			}
			else
			{
				if (lonbarra < lbarra && !nlbarra)
				{
					ok = 1;
				}
			}
		}
	}
	else
	{
		if (nlbarra > lminimo && lonbarra <= lminimo)
		{
			ok = 1;
		}
	}

	if (ok)
	{
		nlbarra = lonbarra;
		i = 0;
		while(barra[i] != -1)
		{
			tbarra[i] = barra[i];
			i++;
		}
		tbarra[i] = barra[i];
	}
	return 0;
}

static int busca_barra(int ibarra,int lonbarra)
{
	int i,tc,k;
	int ok;

	for (i = 0;i < ntcortes;i++)
	{
		tc = pcorte[i];
		if (tc > 0 && (lonbarra+tc) <= lbarra)
		{
			ok = 1;
			for (k = 0;k < ibarra;k++)
			{
				if (barra[k] == i)
				{
					ok = 0;
					break;
				}
			}
			if (ok)
			{					
				barra[ibarra] = i;
				barra[ibarra+1] = -1;
				if (cambia_por_la_mejor_barra(lonbarra+tc) == 1)
				{
					return 1;
				}
				if (busca_barra(ibarra+1,lonbarra+tc) == 1)
				{
					return 1;
				}
			}
		}
	}
	
	return 0;
}

int *corte_barra(int _lbarra,int _ntcortes,int *_plcortes,int _lminimo)
{
    nlbarra = 0;
    tbarra[0] = -1;
	barra[0] = -1;
	lbarra = _lbarra;
	ntcortes = _ntcortes;
	pcorte = _plcortes,
	lminimo = _lbarra - _lminimo;

	busca_barra(0,0);	

	return tbarra;
}

int ordena_cortes(int ncortes,int *_pcortes,int _lbarra,int _lminimo,int *_sepideal,int *homo)
{
	int i,j;
	int size = -1;
	int antsize = -1;
	int swap;
	int labarra = _lbarra - lminimo;
	int ll = 0;
	int nc = 0;
	int difsizes[500];
	int ndif = 0;
	int maxsegs = 0;
	int segs = 0;

	for (i = 0;i < ncortes;i++)
	{
		if (size < 0)
		{
			ll = 0;
			segs = 0;
			for (j = i;j < ncortes;j++)
			{
				if (_pcortes[j] > 0)
				{
					if (antsize > 0 && antsize != _pcortes[j])
					{
						size = _pcortes[j];
						break;
					}
				}
			}
			if (j >= ncortes)
			{
				antsize = -1;
				for (j = i;j < ncortes;j++)
				{
					if (_pcortes[j] > 0)
					{
						size = _pcortes[j];
						break;						
					}
				}
				if (j >= ncortes)
				    break;
			}
				
			if (j != i)
			{
				swap = _pcortes[j];
				_pcortes[j] = _pcortes[i];
				_pcortes[i] = swap;
			}
			for (j = 0;j < ndif;j++)
			{
				if (difsizes[j] == size)
				{
					break;
				}
			}
			if (j >= ndif)
			{
				difsizes[ndif] = size;
				ndif++;
			}
		}
		if (_pcortes[i] != size)
		{
			for (j = i + 1;j < ncortes;j++)
			{
				if (_pcortes[j] == size)
				{
					swap = _pcortes[j];
					_pcortes[j] = _pcortes[i];
					_pcortes[i] = swap;
					break;
				}
			}
			if (j >= ncortes)
			{
				size = _pcortes[i];
			}
		}
		if (size > 0)
		{
			ll += size;
			segs++;
			if (segs > maxsegs)
			{
				maxsegs = segs;
			}			
			if ((ll+size) > labarra && (ll+size) != _lbarra)
			{
				/* es momento de cambiar de tamaño */
				antsize = size;
				size = -1;
			}
		}
		else
		{
			antsize = -1;
			i--;
		}
	}

	for (i = 0;i < ncortes;i++)
	{
		if (_pcortes[i] < 0)
			break;
	}
	ncortes = i;

	if ((maxsegs*2) < 13)
	{
		*_sepideal = maxsegs*2;
	}
	else	
	{
		*_sepideal = maxsegs;
	}
	if (ndif > 1 && *_sepideal < 10)
	{
		*_sepideal = 10;
	}
	while (*_sepideal < 10)
	{
		*_sepideal += maxsegs;
	}

	*homo = ndif;
	return ncortes;
}

struct pdb_header
{
	char name[32];
	unsigned short attributes;
	unsigned short version;
	unsigned long creationdate;
	unsigned long modificationdate;
	unsigned long lastbackupdate;
	unsigned long modificationnumber;
	unsigned long appinfoarea;
	unsigned long sortinfoarea;
	char databasetype[4];
	char creatorid[4];
	unsigned long uniqueidseed;
	unsigned long nextrecordlistid;
	unsigned short nrecords;
};

struct pdb_record
{
	unsigned long recorddataoffset;	
	unsigned char uniqueid[4];
};


struct pdb_appcampo
{
	unsigned short offsetdesc;
	unsigned short tipocampo;
	unsigned short nulo;
};

struct pdb_appinfo
{
	char relleno[276];
	char id[12];
	unsigned short size;
	unsigned short offsetdesc;
	unsigned short ncampos;
	unsigned short minlen;
	char unknow[4];
	/*struct pdb_appcampo campos[n];*/
	/*char *descripciones[n];*/
	char relleno2[128]; /* probar a poner 9 nulos por campo */
};

int crea_pdb(FI_DEF *pfi,char *nombre)
{
	struct pdb_header hdr;
	struct pdb_record rec;
	struct pdb_appinfo app;
	struct pdb_appcampo cam;
	long offsetdata;
	int hdrsize    = 78;
	int recordsize = 8;
	int i,r,ml;
	unsigned short nrecords;
	int file;
	short offset,size,vacio = 0;
	char tmp[512];
	unsigned char idouble[8],ic;

	sprintf(tmp,"%s.pdb",nombre);
	file = rw_open_(tmp,O_RDWR|O_CREAT|O_TRUNC|O_BINARY,S_IREAD|S_IWRITE);
	if (file < 0)
		return -1;

	memset(&hdr,0,sizeof(hdr));
	strcpy(hdr.name,quita_path(nombre));
	stint((short)8,&(hdr.attributes));
	stint((short)1,&(hdr.version));
	stlong(time(NULL),&(hdr.creationdate));
	stlong(time(NULL),&(hdr.modificationdate));
	
	nrecords = (unsigned short)tam_btrv(pfi);

	stlong((long)hdrsize+nrecords*recordsize,&(hdr.appinfoarea));

	memcpy(hdr.databasetype,"DATA",4);	
	memcpy(hdr.creatorid,"1111",4);

	stint((short)nrecords,&(hdr.nrecords));

	rw_write_(file,&hdr,sizeof(hdr));
	rw_lseek_(file,(long)hdrsize+nrecords*recordsize,SEEK_SET);

	memset(&app,0,sizeof(app));	
	strcpy(app.id,"APPFORG2");
	offset = 24+pfi->f_ncampos*6;
	stint((short)offset,&(app.offsetdesc));
	offset += (strlen(hdr.name)+1);
	size = offset;
	ml = 0;
	for (i = 0;i < pfi->f_ncampos;i++)
	{
		size += (strlen(pfi->f_campos[i].c_nombre)+10); /* incluye los nulos */
		switch(pfi->f_campos[i].c_tipo)
		{
		case DOUBLETIPO:
			ml += 8;
			break;
		case INTTIPO:			
		case LONGTIPO:
			ml += 4;
			break;
		default:
			ml += 1;
			break;
		}
	}	
	stint((short)size,&(app.size));	
	stint((short)pfi->f_ncampos,&(app.ncampos));	
	stint((short)ml,&(app.minlen));	

	/*app.unknow[1] = 16;*/

	rw_write_(file,app.relleno,sizeof(app.relleno));
	rw_write_(file,&(app.id),sizeof(app.id));
	rw_write_(file,&(app.size),sizeof(app.size));
	rw_write_(file,&(app.offsetdesc),sizeof(app.offsetdesc));
	rw_write_(file,&(app.ncampos),sizeof(app.ncampos));
    rw_write_(file,&(app.minlen),sizeof(app.minlen));
	rw_write_(file,&(app.unknow),sizeof(app.unknow));	
	for (i = 0;i < pfi->f_ncampos;i++)
	{
		memset(&cam,0,sizeof(cam));
		stint((short)offset,&(cam.offsetdesc));
		offset += (strlen(pfi->f_campos[i].c_nombre)+1);		
		switch(pfi->f_campos[i].c_tipo)
		{
		case DOUBLETIPO:
			stint((short)8,&(cam.tipocampo));
			break;
		case INTTIPO:			
		case LONGTIPO:
			stint((short)3,&(cam.tipocampo));
			break;
		/*
		case FECHATIPO:
			stint((short)7,&(cam.tipocampo));
			break;
		*/
		default:
			stint((short)5,&(cam.tipocampo));
			break;
		}		
		rw_write_(file,&(cam.offsetdesc),sizeof(cam.offsetdesc));
		rw_write_(file,&(cam.tipocampo),sizeof(cam.tipocampo));
		rw_write_(file,&(cam.nulo),sizeof(cam.nulo));
	}
	rw_write_(file,hdr.name,strlen(hdr.name)+1);	
	for (i = 0;i < pfi->f_ncampos;i++)
	{
		rw_write_(file,pfi->f_campos[i].c_nombre,strlen(pfi->f_campos[i].c_nombre)+1);	
	}
	for (i = 0;i < pfi->f_ncampos;i++)
	{
		rw_write_(file,app.relleno2,9);
	}
	offsetdata = rw_lseek_(file,0L,SEEK_CUR);
	lee_reg(pfi,ISFIRST);
	for (r = 0;r < nrecords;r++)
	{
		memset(&rec,0,sizeof(rec));	
		rw_lseek_(file,hdrsize+r*recordsize,SEEK_SET);
		stlong(offsetdata,&(rec.recorddataoffset));
		stlong(r+1,rec.uniqueid);
		rec.uniqueid[0] = 0x40;/* atributes = dirty */
		rw_write_(file,&(rec.recorddataoffset),sizeof(rec.recorddataoffset));
		rw_write_(file,rec.uniqueid,sizeof(rec.uniqueid));
		rw_lseek_(file,offsetdata,SEEK_SET);
		for (i = 0;i < pfi->f_ncampos;i++)
		{
			switch(pfi->f_campos[i].c_tipo)
			{
			case DOUBLETIPO:
				memcpy(idouble,pfi->f_buf+pfi->f_campos[i].c_offset,8);
				ic = idouble[0];
				idouble[0] = idouble[7];
				idouble[7] = ic;
				ic = idouble[1];
				idouble[1] = idouble[6];
				idouble[6] = ic;
				ic = idouble[2];
				idouble[2] = idouble[5];
				idouble[5] = ic;
				ic = idouble[3];
				idouble[3] = idouble[4];
				idouble[4] = ic;
				/*offsetdata += rw_write_(file,pfi->f_buf+pfi->f_campos[i].c_offset,pfi->f_campos[i].c_long);*/
				offsetdata += rw_write_(file,idouble,8);
				break;
			case INTTIPO: /* los guardamos como longs */
				offsetdata += rw_write_(file,&vacio,sizeof(short));
				offsetdata += rw_write_(file,pfi->f_buf+pfi->f_campos[i].c_offset,pfi->f_campos[i].c_long);
				break;
			case LONGTIPO:
				offsetdata += rw_write_(file,pfi->f_buf+pfi->f_campos[i].c_offset,pfi->f_campos[i].c_long);
				break;
			/*
			case FECHATIPO:
				stint((short)7,&(cam.tipocampo));
				break;
			*/
			default:
				ld_campo(i,pfi,tmp);
				offsetdata += rw_write_(file,tmp,strlen(tmp)+1);
				break;
			}		
		}
		lee_reg(pfi,ISNEXT);
	}
	rw_close_(file);
	return 0;
}


#endif
