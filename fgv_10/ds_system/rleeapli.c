
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

#include <rwmalloc.h>
#define _NUEVODEF
#include <rdef.h>
#include <rmante.h>
#include <rcalcul.h>


int apli_bloqueo = -1;

static short _el_fin = 0;

extern short _fgv_version;
extern short i3;
extern short _salida2;

extern unsigned short local_term[52];
extern unsigned short local_term_[52];

extern long nume_serie;

#ifndef O_BINARY  /* compatibilidad xenix - dos */

#define O_BINARY 0
#define SEEK_SET 0
#define SEEK_CUR 1
#ifndef LARGESEEK
extern long rw_lseek_();
#endif
extern unsigned int rw_read_();

#endif


extern char dirbin[MAXPATH];
extern char dirdef[MAXPATH];
extern char dirpan[MAXPATH];
extern char dirfic[MAXPATH];
extern char nomfdir[MAXPATH];
extern char dirficb[MAXPATH];
extern char dirbass[MAXPATH];
extern char dirbase[MAXPATH];
extern char nomapli[25];


extern double DsEuroConvierte; 
extern int DsDeciModoOtro;
extern int DsDeciModoNormal;
extern char DsEuroTexto[512];

extern int dentro_de_leeaplic = 0;

extern int fecha_protegida; /* control */

extern short limitacion_especial;
extern int   mimodo_debug;

extern char *quita_path();

extern struct salva_def
{
   char *nombre_fichero;
   char *enlaza_pfi;
   char *salvadatos;
   char *buf;
   int fd;
   unsigned long cuando;
   int quememo;
   int lock;
   int tam_buf;
} s_d[MAXDEF*MAXRECUR];

extern int recur_punto;
extern int recur_nfi[MAXRECUR];
extern void *recur_mpfi[MAXRECUR];


static void libera_prog();


struct limitacion_ /* alineada a 8 bytes */
{
   char fichero[12]; 
   char desde[260];
   char hasta[260];
   long campo;
   long registros;
   long modo;
} *limitaciones = NULL;
int numero_limitaciones = 0;
int **relacion_limitacion = NULL;


char *cambia_path_def(char *nom,int v,char *destino)
{
   char nombre[256];
   char tmp[256];
   char c;
   FILE *file_t;

      if (nom == quita_path(nom)) 
      {
         if (v) {
            strcpy(nombre,dirbase);
            strcat(nombre,"vent/");
            strcat(nombre,nom);
         }
         else {
            strcpy(nombre,dirbase);
            strcat(nombre,"def/");
            strcat(nombre,nom);
         }
      }
      else 
      {
         if (*nom == ':') 
         {
            if (*(nom+1) == '/') 
            {
               strcpy(nombre,dirbass);
               strcat(nombre,nom+2);
            }
            else 
            {
               c = *(quita_path(nom));
               *(quita_path(nom)) = 0;
               strcpy(nombre,dirbass);
               strcat(nombre,nom+1);
               *(quita_path(nom)) = c;
               if (v)
                  strcat(nombre,"vent/");
               else
                  strcat(nombre,"def/");
               strcat(nombre,quita_path(nom));
            }
         }
         else
            strcpy(nombre,nom);
         strcpy(tmp,nombre);
         strcat(tmp,".mas");
         file_t = rw_open_(tmp,O_RDONLY | O_BINARY);
         if (file_t < 0)
         {
			 if (file_t < 0)
			 {
				strcpy(tmp,dirbase);
				strcat(tmp,"vent/");
				strcat(tmp,quita_path(nombre));            
				strcat(tmp,".adi");
				file_t = rw_open_(tmp,O_RDONLY | O_BINARY);
				if (file_t < 0)
				{
					return NULL;
				}
				rw_close_(file_t);
			 }
         }
         else
         {
            rw_close_(file_t);
         }
      }
	  strcpy(destino,nombre);
	  return destino;
}


char *cambia_path_fich(p)
char *p;
{
   char tmp[256];
   int punto;
   
   if (p == quita_path(p))
      strcatd(p,dirfic);
   else 
   {
      if (*p == ':' && *(p+1) == '/') 
      {
         strcpy(tmp,dirbass);
         strcat(tmp,p+2);
		 punto = strbusca(tmp,"/fich/");
		 if (punto > -1)
		 {
			 tmp[punto+1] = 0;
			 strcpy(p,tmp);
			 strcat(p,fichdir);
			 strcat(p,tmp+punto+5);
		 }
		 else
			strcpy(p,tmp);
      }
   }
   return(p);
}


inicializa_def(ipfi)
int ipfi;
{
   FI_DEF *fi = v_mpfi(ipfi);
   int i,j;
   char *p;
   char tmp[256];
   char titulo[256];
   
   /* conservar el nombre original del def */   
   
   fi->f_ipfi = ipfi;
   p = fi->f_fichero;
   cambia_path_fich(p);
   for (i = 0; i < fi->f_npanta; i++) 
			{
      p = fi->f_pantalla[i].p_nombrepan;
      /* preservar titulo pantalla (!!!!) */
      p += strlen(p);
      if (*(p+1) == 1 && *(p+2) == 2)
      {
         strcpy(titulo,p+1);
         quitab(titulo);
      }
      else
	  {		 
         titulo[0] = 0;
	  }
      
      p = fi->f_pantalla[i].p_nombrepan;
	  quitab(p);
      
      if (p == quita_path(p))
         strcatd(p,dirpan);
      else 
      {
         if (*p == ':' && *(p+1) == '/') 
         {
            strcpy(tmp,dirbass);
            strcat(tmp,p+2);
            strcpy(p,tmp);
         }
      }	  
      if (titulo[0])
      {
         j = O_MAXPATH - strlen(p) - 1;
         if (j < strlen(titulo))
         {
            titulo[j] = 0;
         }
         p = fi->f_pantalla[i].p_nombrepan;
         p += strlen(p);
         strcpy(p+1,titulo);
      }
	}
   defecto_ficha(fi);

   llama_a_proceso_inicializacion(ipfi);
   
   if (limitacion_especial && limitaciones && relacion_limitacion)
   {
      for (i = 0;i < numero_limitaciones;i++)
      {
         if (!strcmp(quita_path(fi->f_fichero),limitaciones[i].fichero))
         {
            if (!relacion_limitacion[ipfi])
            {
               relacion_limitacion[ipfi] = (int *)malloc(sizeof(int) * 61);
               memset(relacion_limitacion[ipfi],0xff,sizeof(int) * 61);
            }
            for (j = 0; j < 60 && relacion_limitacion[ipfi][j] != -1;j++);		    
            relacion_limitacion[ipfi][j] = i;
         }
      }
   }
   return(0);
}

int carga_limitacion(aplic)
char *aplic;
{
			char tmp[128];
         int file,i,j,k;
         long ll,aa,bb,kk;
         long *ptt;
         
         strcpy(tmp,aplic);
         *quita_path(tmp) = 0;
         if (limitaciones)
         { 
            free((char *)limitaciones);
            limitaciones = NULL;
         }
         if (relacion_limitacion)
         {
            for (i = 0;i < 100;i++)
            {
               if (relacion_limitacion[i])
               {
                  free((char *)relacion_limitacion[i]);
               }
            }
            free((char *)relacion_limitacion);
            relacion_limitacion = NULL;
         }
         numero_limitaciones = 0;
         sprintf(tmp+strlen(tmp),"%s.l%d",nomapli,limitacion_especial);
         file = rw_open_(tmp,O_RDONLY | O_BINARY);
         if (file < 0)
         { 
            strcat(tmp," INEXSISTENTE!!");
            aviso(0,tmp);
            return(-1);
         }
         ll = rw_lseek_(file,0L,SEEK_END);
         ll -= (sizeof(long)*2);
         if (ll < 0L ||  (ll%(sizeof(struct limitacion_))) )
         {
            rw_close_(file);
            strcat(tmp," ALTERADO O INCORRECTO!!");  
            aviso(0,tmp);
            return(-1);
         }
         rw_lseek_(file,0L,SEEK_SET);
         rw_read_(file,&aa,sizeof(long));
         if ( ((aa) ^ ((long)limitacion_especial)) != (0x12439824L)) 
         {
            rw_close_(file);
            strcat(tmp," INCORRECTO!!");
            aviso(0,tmp);
            return(-1);  
         }
         rw_read_(file,&bb,sizeof(long));
         k = 0;
         for (i = 0;nomapli[i];i++)
            k ^= nomapli[i];
         bb ^= k;
         
         limitaciones = (struct limitacion_ *)malloc(ll);
         numero_limitaciones = ll / sizeof(struct limitacion_);
         /* rw_lseek_(file,(long)(sizeof(long)*2),SEEK_SET); */
         rw_read_(file,(char *)limitaciones,ll);
         rw_close_(file);
         
         
         
         i = sizeof(struct limitacion_) / sizeof(long);
         kk = 0L;
         for (k = 0;k < numero_limitaciones;k++)
         {
            ptt =  (long *)(&limitaciones[k]);
            for (j = 0;j < i;j++)
            {
               *ptt ^= aa;
               kk ^= *ptt;
               ptt++;	 
            }  
         }
         
         if (kk != bb) 
         {
            free((char *)limitaciones);
            limitaciones = NULL;
            rw_close_(file);
            strcat(tmp," ALTERADO!!");
            aviso(0,tmp);
            return(-1);  
         }
         
         relacion_limitacion = (int **)malloc(sizeof(int *) * 100);
         memset((char *)relacion_limitacion,0,sizeof(int *) * 100);
         
         return(0);
}

int libera_limitacion()
{
   int i;
   if (limitaciones)
   { 
      free((char *)limitaciones);
      limitaciones = NULL;
   }
   if (relacion_limitacion)
   {
      for (i = 0;i < 100;i++)
						{
         if (relacion_limitacion[i])
         {
            free((char *)relacion_limitacion[i]);
         }
						}
      free((char *)relacion_limitacion);
      relacion_limitacion = NULL;
   }
   limitacion_especial = 0;
   numero_limitaciones = 0;
   return(0);
}

extern double rwatof();

int esta_limitado(fichero,registro,valor,tipo)
int fichero;
long registro;
unsigned char *valor;
int tipo;
{
   int i,j;
   double d1,d2;
   if (!limitacion_especial) return(0);
   if (!relacion_limitacion || fichero > 99 || fichero < 0) return(1);
   if (!limitaciones) return(1); /* no hay fichero todo limitado */
   
   if (!relacion_limitacion[fichero]) return(0);
   
   for (j = 0;j < 61;j++)
   {   
      if ((i = relacion_limitacion[fichero][j]) == -1) return(0);
      
      if (mimodo_debug)
						{
         Bl(1);
         Bl(25);
						}
      
      if (!valor)
						{
         if (limitaciones[i].registros && registro && registro > limitaciones[i].registros)
         {
            return(1);
         }
         continue;
						}
      
      if (!(limitaciones[i].modo) || limitaciones[i].campo != registro) 
						{
         if (mimodo_debug)
         {
            pprint(101,"No limitado ");
            v_printd("%5d ",(int)limitaciones[i].modo);
            v_printd("[%5d ! ",(int)limitaciones[i].campo);
            v_printd("%5d]",(int)registro);
         }
         continue;
						}
      
      if (mimodo_debug)
						{
         pprint(2501,"Limitado [");
         v_print(valor);
         v_print("]desde[");
         v_print(limitaciones[i].desde);
         v_print("]hasta[");
         v_print(limitaciones[i].hasta);
         v_print("]");
						}
      switch(tipo)
						{
						case INTTIPO :
                  case DOUBLETIPO:
                     d1 = (double)rwatof(valor);
                     d2 = (double)rwatof(limitaciones[i].desde);
                     if (d1 < d2) return(1);
                     d1 = (double)rwatof(valor);
                     d2 = (double)rwatof(limitaciones[i].hasta);
                     if (d1 > d2) return(1);
                     break;
                  case FECHATIPO:
                     if (cmfecha(valor,limitaciones[i].desde) < 0) return(1);
                     if (cmfecha(valor,limitaciones[i].hasta) > 0) return(1);	   
                     break;
                  default:
                     if (strcmp(valor,limitaciones[i].desde) < 0) return(1);
                     if (strcmp(valor,limitaciones[i].hasta) > 0) return(1);
                     break;
						}
   }
   
   return(0);
}

int lees_relas(rel,file1)
int rel;
int file1;
{
   int i,e = 0;
#ifdef ESMOTOROLA
   int _m_i = 0;
#endif
   
   if (!(mpfirel[rel] = (RELAS *)malloc(sizeof(RELAS))))
						return(-1);
   rw_read_(file1,(char *)mpfirel[rel],sizeof(RELAS));
   
   moto_short(&(mpfirel[rel]->nrel));
   moto_short(&(mpfirel[rel]->nrelact));
   moto_short(&(mpfirel[rel]->nreldef));
   moto_short(&(mpfirel[rel]->nrelpin));
   moto_short(&(mpfirel[rel]->lineas));
   moto_short(&(mpfirel[rel]->clave));
   
   if ((i = mpfirel[rel]->nrel) > 0) {
						if (!(mpfirel[rel]->relacis = (short *)malloc(sizeof(short) * i)))
                     goto relas_error;
                  e++;
                  if (!(mpfirel[rel]->camminu = (short *)malloc(sizeof(short) * i)))
                     goto relas_error;
                  e++;
                  if (!(mpfirel[rel]->camrela = (short *)malloc(sizeof(short) * i)))
                     goto relas_error;
                  e++;
                  if (!(mpfirel[rel]->numrela = (short *)malloc(sizeof(short) * i)))
                     goto relas_error;
                  e++;
                  
                  rw_read_(file1,(char *)mpfirel[rel]->relacis,sizeof(short) * i);
                  rw_read_(file1,(char *)mpfirel[rel]->camrela,sizeof(short) * i);
                  rw_read_(file1,(char *)mpfirel[rel]->camminu,sizeof(short) * i);
                  rw_read_(file1,(char *)mpfirel[rel]->numrela,sizeof(short) * i);
                  
#ifdef ESMOTOROLA
                  for (_m_i = 0;_m_i < i;_m_i++)
                  {
                     moto_short(&(mpfirel[rel]->relacis[_m_i]));
                     moto_short(&(mpfirel[rel]->camrela[_m_i]));
                     moto_short(&(mpfirel[rel]->camminu[_m_i]));
                     moto_short(&(mpfirel[rel]->numrela[_m_i]));
                  }
#endif
   }
   
   if ((i = mpfirel[rel]->nrelact) > 0) {
						if (!(mpfirel[rel]->relact = (REL_ACT *)malloc(sizeof(REL_ACT) * i))) {
                     mpfirel[rel]->nrelact = 0;
                     mpfirel[rel]->nreldef = 0;
                     mpfirel[rel]->nrelpin = 0;
                     goto relas_error;
                  }
                  rw_read_(file1,(char *)mpfirel[rel]->relact,sizeof(REL_ACT) * i);
#ifdef ESMOTOROLA
                  for (_m_i = 0;_m_i < i;_m_i++)
                  {
                     moto_short(&(mpfirel[rel]->relact[_m_i].campo));
                     moto_short(&(mpfirel[rel]->relact[_m_i].ambito));
                     moto_short(&(mpfirel[rel]->relact[_m_i].tabla));
                     moto_short(&(mpfirel[rel]->relact[_m_i].fflag));
                  }
#endif
   }
   if ((i = mpfirel[rel]->nreldef) > 0) {
						if (!(mpfirel[rel]->reldef = (REL0_DEF *)malloc(sizeof(REL0_DEF) * i))) {
                     mpfirel[rel]->nreldef = 0;
                     mpfirel[rel]->nrelpin = 0;
                     goto relas_error;
                  }
                  rw_read_(file1,(char *)mpfirel[rel]->reldef,sizeof(REL0_DEF) * i);
#ifdef ESMOTOROLA
                  for (_m_i = 0;_m_i < i;_m_i++)
                  {
                     moto_short(&(mpfirel[rel]->reldef[_m_i].campo));
                     moto_short(&(mpfirel[rel]->reldef[_m_i].nfirel));
                     moto_short(&(mpfirel[rel]->reldef[_m_i].nficampo));
                     moto_short(&(mpfirel[rel]->reldef[_m_i].numrela));
                  }
#endif
   }
   if ((i = mpfirel[rel]->nrelpin) > 0) {
						if (!(mpfirel[rel]->relpin = (REL_PIN *)malloc(sizeof(REL_PIN) * i))) {
                     mpfirel[rel]->nrelpin = 0;
                     goto relas_error;
                  }
                  rw_read_(file1,(char *)mpfirel[rel]->relpin,sizeof(REL_PIN) * i);
#ifdef ESMOTOROLA
                  for (_m_i = 0;_m_i < i;_m_i++)
                  {
                     moto_short(&(mpfirel[rel]->relpin[_m_i].campo));
                     moto_short(&(mpfirel[rel]->relpin[_m_i].qp));
                     moto_short(&(mpfirel[rel]->relpin[_m_i].nfirel));
                     moto_short(&(mpfirel[rel]->relpin[_m_i].nficampo));
                     moto_short(&(mpfirel[rel]->relpin[_m_i].numrela));
                     moto_short(&(mpfirel[rel]->relpin[_m_i].fflag));
                  }
#endif
   }
   
   
   return(0);
   
relas_error:
   
   if (e < 4) {
						mpfirel[rel]->nrel = 0;
                  if (e > 0)
                     free((char *)mpfirel[rel]->relacis);
                  if (e > 1)
                     free((char *)mpfirel[rel]->camminu);
                  if (e > 2)
                     free((char *)mpfirel[rel]->camrela);
   }
   return(-1);
}

#ifdef ESMOTOROLA
struct memo_temp {
   double d;
   short t;
   short f;
} *m_t;
#endif


lee_prog(numero)
int numero;
{
   short i,j,n,ntabex,nproex,nficin,indio;
   int file1,leememo;
   unsigned short tamano = 0;
   char tmp[81];
   char c;
   char nombre[81];
   int recur = (recur_punto < 1) ? 0 : recur_punto * MAXDEF;
#ifdef ESMOTOROLA
   int _m_i = 0;
   double tmpd;
#endif
   
   strcpy(nombre,en_pr[numero].prc);
   /*
   if (!strcmp(quita_path(nombre),"pcegz001")) {
	   c = 0;
   }
   */
   strcat(nombre,".prc");
   
   if (( file1 = rw_open_(nombre,O_RDWR | O_BINARY)) < 0)
      return(-1);
   
   rw_lseek_(file1,0L,SEEK_SET);
   rw_read_(file1,(char *)&i,sizeof(short));
   moto_short(&i);
   if (i != 201)
   {
      /*printf("mal prc[%d]",i);getchar();*/
   }
   rw_lseek_(file1,24L,SEEK_CUR);
   
   rw_read_(file1,(char *)&nprog,sizeof(short));
   moto_short(&nprog);
   rw_read_(file1,(char *)&nmemo,sizeof(short));
   moto_short(&nmemo);
   rw_read_(file1,(char *)&ncond,sizeof(short));
   moto_short(&ncond);
   rw_read_(file1,(char *)&nbucl,sizeof(short));
   moto_short(&nbucl);
   rw_read_(file1,(char *)&noper,sizeof(short));
   moto_short(&noper);
   rw_read_(file1,(char *)&nf_B,sizeof(short));
   moto_short(&nf_B);
   rw_read_(file1,(char *)&ntabex,sizeof(short));
   moto_short(&ntabex);
   rw_read_(file1,(char *)&nproex,sizeof(short));
   moto_short(&nproex);
   rw_read_(file1,(char *)&nficin,sizeof(short));
   moto_short(&nficin);
   
   
   en_pr[numero].e_nprog = nprog;
   en_pr[numero].e_ncond = ncond;
   en_pr[numero].e_nbucl = nbucl;
   en_pr[numero].e_noper = noper;
   en_pr[numero].e_nf_B = nf_B;
   en_pr[numero].e_prog = prog = (PROGRAM *)reserva_malloc(sizeof(PROGRAM) * nprog,6);
   en_pr[numero].e_cond = cond = (CONDIC *)reserva_malloc(sizeof(CONDIC) * ncond,7);
   en_pr[numero].e_bucl = bucl = (BUCLE *)reserva_malloc(sizeof(BUCLE) * nbucl,8);
   en_pr[numero].e_oper = oper = (OPERA *)reserva_malloc(sizeof(OPERA) * noper,9);
   if (!en_pr[numero].e_memo && nmemo)
   {
      leememo = 1;
      en_pr[numero].e_memo = memo = (T_MEMORIA *)malloc(sizeof(T_MEMORIA) * nmemo);
      en_pr[numero].e_nmemo = nmemo;
   }
   else
   {
      if (!en_pr[numero].e_nmemo)
      {
         en_pr[numero].e_nmemo = nmemo;
         leememo = 1;
      }
      else
         leememo = 0;
      memo = en_pr[numero].e_memo;
   }
   en_pr[numero].e_f_B = f_B = (struct _filebucle *)reserva_malloc(sizeof(struct _filebucle) * nf_B,10);
   en_pr[numero].tabla_externa = (short *)reserva_malloc(sizeof(short) * (ntabex+1),11);
   en_pr[numero].tabla_ficheros = (short *)reserva_malloc(sizeof(short) * 48,12);
   en_pr[numero].activo = 1;
   
   for (j = 0;j < 48;j++)
      en_pr[numero].tabla_ficheros[j] = j;
   
   /* enlazar codigos de fichero empleados */
   for (i = 0;i < nficin;i++)
   {
      rw_read_(file1,(char *)&n,sizeof(short));
      moto_short(&n);
      rw_read_(file1,(char *)&indio,sizeof(short));
      moto_short(&indio);
      rw_read_(file1,&c,1);
      rw_read_(file1,nombre,c);    
      if (indio >= 2000)
      {
         indio /= 2;
         en_pr[numero].tabla_ficheros[n] = indio;
      }
      else
      {
         for (j = 0;j < nfi;j++)
         {
            if (j == 48) continue;
            if (s_d[recur+j].nombre_fichero && !strcmp(quita_path(s_d[recur+j].nombre_fichero),nombre))
            {
               en_pr[numero].tabla_ficheros[n] = j;
               break;
            }
         }  
         if (j == nfi)
         {
			 for (j = 0;j < nfi;j++)
			 {
				if (j == 48) continue;
				if (s_d[recur+j].nombre_fichero && !strcmp(quita_path(s_d[recur+j].nombre_fichero),quita_path(nombre)))
				{
				   en_pr[numero].tabla_ficheros[n] = j;
				   break;
				}
			 }  
			 if (j == nfi)
			 {

				for (j = 0;j < nfi;j++)
				{
				   if (j == 48) continue;
				   if (s_d[recur+j].enlaza_pfi && !strcmp(s_d[recur+j].enlaza_pfi,nombre))
				   {
					  en_pr[numero].tabla_ficheros[n] = j;
					  break;
				   }
				}
				if (j == nfi)
				{       
				   printf("fichero no enlazado[%s]",nombre);getchar();
				}        
			 }
		 }
      }
   }
   
   /* enlazar memorias externas */
   /* esto se puede crear al momento de generar y grabarlo en el .tab */
   rw_lseek_(file1,(long)sizeof(M_EX) * ntabex,SEEK_CUR);
   for (i = 0;i < ntabex;i++)
   {
      rw_read_(file1,&c,1);
      rw_read_(file1,nombre,c);
      for (j = memoplin + maxpila * 2;j < nm_ex;j++)
      {
         if (!strcmp(m_ex[j].nombre,nombre))
         {
            en_pr[numero].tabla_externa[i] = j;
            break;
         }
         else
            en_pr[numero].tabla_externa[i] = -1;
      }
      if (j == nm_ex)
      {
         printf("memoria externa no enlazada[%s]",nombre);getchar();
      }
      else
      {
         if (m_ex[j].m_tipo == CHARTIPO || m_ex[j].m_tipo == FECHATIPO)
         {
            rw_read_(file1,&c,1);
            rw_lseek_(file1,(long)c,SEEK_CUR);
         }
      }
   }
   en_pr[numero].tabla_externa[i] = -2;
   
   rw_read_(file1,(char *)&tamano,sizeof(short));
   moto_short(&tamano);
   /* saltar informacion no necesaria para ejecucion */
   rw_lseek_(file1,(long)tamano,SEEK_CUR);
   
   
   if (nprog > 0)
   {
      rw_read_(file1,(char *)prog,sizeof(PROGRAM) * nprog);
#ifdef ESMOTOROLA
      for (_m_i = 0; _m_i < nprog; _m_i++)
      {
         moto_short(&(prog[_m_i].indice));
      }
#endif
   }
   
   if (nmemo > 0)
   {
      if (leememo)
      {
         rw_read_(file1,(char *)memo,nmemo *sizeof(T_MEMORIA));
#ifdef ESMOTOROLA
         for (_m_i = 0; _m_i < nmemo; _m_i++)
         {
            moto_short(&(memo[_m_i].m_tipo));
            if (memo[_m_i].m_tipo == DOUBLETIPO)
               moto_double(&(memo[_m_i].mv.vald));
         }
#endif
      }
      else
      {
         rw_lseek_(file1,(long)nmemo *sizeof(T_MEMORIA),SEEK_CUR);
      }
   }
   
   if (ncond > 0)
   {
      rw_read_(file1,(char *)cond,sizeof(CONDIC) * ncond);
#ifdef ESMOTOROLA
      for (_m_i = 0; _m_i < ncond; _m_i++)
      {
         moto_short(&(cond[_m_i].fichero1));
         moto_short(&(cond[_m_i].campo1));
         moto_short(&(cond[_m_i].fichero2));
         moto_short(&(cond[_m_i].campo2));
         moto_short(&(cond[_m_i].sigue));
         moto_short(&(cond[_m_i].tipo));
      }
#endif
   }
   
   if (nbucl > 0)
   {
      rw_read_(file1,(char *)bucl,sizeof(BUCLE) * nbucl);
#ifdef ESMOTOROLA
      for (_m_i = 0; _m_i < nbucl; _m_i++)
      {
         moto_short(&(bucl[_m_i].operacion1));
         moto_short(&(bucl[_m_i].condicion));
         moto_short(&(bucl[_m_i].operacion2));
         moto_short(&(bucl[_m_i].salto));
      }
#endif
   }
   
   if (noper > 0)
   {
      rw_read_(file1,(char *)oper,sizeof(OPERA) * noper);
#ifdef ESMOTOROLA
      for (_m_i = 0; _m_i < noper; _m_i++)
      {
         moto_short(&(oper[_m_i].fichero1));
         moto_short(&(oper[_m_i].campo1));
         moto_short(&(oper[_m_i].fichero2));
         moto_short(&(oper[_m_i].campo2));
         moto_short(&(oper[_m_i].ficheror));
         moto_short(&(oper[_m_i].campor));
      }
#endif
   }
   
   if (nf_B > 0)
   {
      rw_read_(file1, (char *)f_B,sizeof(struct _filebucle) * nf_B);
      
#ifdef ESMOTOROLA
      for (_m_i = 0; _m_i < nf_B; _m_i++)
      {
         moto_short(&(f_B[_m_i].nfi));
         moto_short(&(f_B[_m_i].nord1));
         moto_short(&(f_B[_m_i].nord2));
         moto_short(&(f_B[_m_i].ncomp));
         moto_short(&(f_B[_m_i].flag));
      }
#endif
      
      for (i = 0;i < nf_B;i++)
      {
         rw_read_(file1,&j,sizeof(short));
         moto_short(&j);
         f_B[i].ord1 = (short *)malloc(j*sizeof(short));
         rw_read_(file1,f_B[i].ord1,j*sizeof(short));
#ifdef ESMOTOROLA
         for (_m_i = 0; _m_i < j;  _m_i++)
         {
            moto_short(&(f_B[i].ord1[_m_i]));
         }
#endif
         f_B[i].ord2 = (short *)malloc((f_B[i].nord2/100)*sizeof(short));
         rw_read_(file1,f_B[i].ord2,(f_B[i].nord2/100)*sizeof(short));
#ifdef ESMOTOROLA
         for (_m_i = 0; _m_i < (f_B[i].nord2/100);  _m_i++)
         {
            moto_short(&(f_B[i].ord2[_m_i]));
         }
#endif
         f_B[i].comp = (short *)malloc(f_B[i].ncomp * sizeof(short));
         rw_read_(file1,f_B[i].comp,f_B[i].ncomp*sizeof(short));
#ifdef ESMOTOROLA
         for (_m_i = 0; _m_i < f_B[i].ncomp;  _m_i++)
         {
            moto_short(&(f_B[i].comp[_m_i]));
         }
#endif
         rw_read_(file1,&j,sizeof(short));
         moto_short(&j);
         f_B[i].calc = (short *)malloc(j*sizeof(short));
         rw_read_(file1,f_B[i].calc,j*sizeof(short));
#ifdef ESMOTOROLA
         for (_m_i = 0; _m_i < j;  _m_i++)
         {
            moto_short(&(f_B[i].calc[_m_i]));
         }
#endif
      }
   }
   
   if (leememo)
   {
      for (i = 0;i < nmemo;i++)
      {
         if (memo[i].m_tipo == CHARTIPO || memo[i].m_tipo == FECHATIPO)
         {
            rw_read_(file1,&c,1);
            memo[i].mv.valc = strmalloc(c);
            rw_read_(file1,memo[i].mv.valc,c);
         }
      }
   }
   
   rw_close_(file1);
   
   programa_activo = numero;
   
   return(0);
}

lee_aplic(aplic,prime)
char *aplic;
int prime;
{
   FI_DEF *pfi;
   int file1;
   short rel,i,_i3,m,jj,j;
   unsigned short nn;
   short t;
   char nom[MAXPATH];
   char nombre[MAXPATH];
   char tmp[256];
   long pp,xx;
   int o_rwdebug;
   int salida = prime;
   char c,v,cc;
   char *tmp0;
   int recur = (recur_punto < 1) ? 0 : recur_punto * MAXDEF;
   short max_panta,max_campos,max_clave,max_relaci,max_gene,max_prog,max_cond;
   short max_bucl,max_oper,max_f_B,max_ex;
   unsigned short dato_e;
   unsigned short dato;
   unsigned long n_s;
   unsigned short version_de_def;
   int control_s = 0;
   int es_una_con = 0;
   int demos;
   short i_nume_code;
   short i_nume_mochi,limi1;
   int como_bloqueo = 0;
   int file_t;
   long ll1,ll2;
   int ant_formato = 0;
   int nfimalo = -1;
   
   nfi = 0;
   nprog = 0;
   ncond = 0;
   nbucl = 0;
   noper = 0;
   nmemo = 0;
   nf_B = 0;
   nm_ex = 0;
   np_ex = 0;
   prog = NULL;
   cond = NULL;
   bucl = NULL;
   oper = NULL;
   memo = NULL;
   f_B  = NULL;
   m_ex = NULL;
   p_ex = NULL;
   apli_bloqueo = -1;
   
   _salida2 = 0;
   fecha_protegida = 0;
   
   if (( file1 = rw_open_(aplic,O_RDWR | O_BINARY)) < 0)
      return(-1);
   
   rw_lseek_(file1,0L,SEEK_SET);
   rw_read_(file1,(char *)&rel,sizeof(short));
   moto_short(&rel);
   if (rel == ID_ANT_TAB) 
   {
      ant_formato = 1; 
      aviso(0,"Formato incompatible con Diagram9");
      rw_close_(file1);
      return(-1);
   }
   
   if (rel == ID_TAB)
   {	
      rw_read_(file1,(char *)&rel,sizeof(short));
      moto_short(&rel);
   }
   if (rel < 0) /* es aplicacion estandar */
   {
      if ( (rel+32000) > _fgv_version)
      {
         /* atencion es una aplicacion para version superior de runtime */
         salida = -4;
         rw_close_(file1);
         goto super_salida;
      }
      rw_read_(file1,(char *)&_i3,sizeof(short));
      moto_short(&_i3);
      rw_read_(file1,(char *)&_i3,sizeof(short));
      moto_short(&_i3);
      rw_read_(file1,(char *)&_i3,sizeof(short));
      moto_short(&_i3);
      
      if (_i3 != i3)
      {
         /* no corresponde a este runtime */
         salida = -5;
         rw_close_(file1);
         goto super_salida;
      }
      xx = rw_lseek_(file1,(long)0,SEEK_CUR);
      rw_lseek_(file1,(long)103*sizeof(short),SEEK_CUR);
      
      
      
      rw_read_(file1,(char *)&dato,sizeof(short));
	  moto_short(&dato);
      rw_read_(file1,(char *)&dato_e,sizeof(short));
	  moto_short(&dato_e);
      i_nume_code  = (short)(dato ^ dato_e);
#ifdef AGIMAXPROTECT
      if (nume_code() != (short)(dato ^ dato_e))
      {
         control_s++;
         /*
         salida = -5;
         rw_close_(file1);
         goto super_salida;
         */
      }
#endif
      
      rw_read_(file1,(char *)&dato,sizeof(short));
	  moto_short(&dato);
      n_s = (long)(dato ^ dato_e) * 0xffffL;
      rw_read_(file1,(char *)&dato,sizeof(short));
	  moto_short(&dato);
      n_s += (dato ^ dato_e);
#ifdef AGIMAXPROTECT
      if (n_s != nume_serie)
      {
         control_s++;
         /*
         salida = -5;
         rw_close_(file1);
         goto super_salida;
         */
      }
#endif
      
      rw_read_(file1,(char *)&dato,sizeof(short));
	  moto_short(&dato);
      dato = dato ^ dato_e;
      i_nume_mochi = dato;
#ifdef AGIMAXPROTECT
      if (dato != _Posicion())
      {
         control_s++;
         /*
         salida = -5;
         rw_close_(file1);
         goto super_salida;
         */
      }
#endif
      
      rw_read_(file1,(char *)&dato,sizeof(short));
	  moto_short(&dato);
      version_de_def = dato ^ dato_e;
      
      
      rw_read_(file1,(char *)&rel,sizeof(short));
      moto_short(&rel);
      pp = rw_lseek_(file1,(long)0,SEEK_CUR);
      rw_lseek_(file1,xx,SEEK_SET);
      demos = 0;
      for (i = 0;i < /*rel*/103;i++)
      {         
         rw_read_(file1,(char *)&nn,sizeof(short));
         moto_short(&nn);

         if (i && nn == 0xffff) continue;

#ifdef AGIMAXPROTECT
         nn ^= nume_code();
#else
         nn ^= i_nume_code;
#endif
         if (nn == -27273 || nn == -1) {
            es_una_con = 1;
            nn = -1;
         }
         if (nn == 0xffff && version_de_def == 0xffff) control_s = 0;
         /*
         if (nn != -1 && version_de_def != -1 && control_s)
         {
         salida = -5;
         rw_close_(file1);
         goto super_salida;
         }
         */
		 if (version_de_def == 850 || nn == 850)
		 {
			 jj = 0;
		 }
		 else if (nfimalo == -1)
		 {
			 for (jj = 0;jj < 50;jj++)
			 {
				if (!control_s && nn == local_term[jj]) {
				   if (i || local_term_[jj] == version_de_def)
					  break;
				}
			 }

			 if (jj >= 50)
			 {
				if (local_term[50] > nn || local_term[51] < nn)
				{
				   char tmpxx[256];
				   demos++;
				   sprintf(tmpxx,"%d-%d",nn,version_de_def);
				   /*cursor(2501);
				   v_printd("%05d ",nn);
				   v_printd("%05d",version_de_def);*/
				   mensaje_en_cabecera("%s",tmpxx,1);
				   nfimalo = i;
				}
			 }
		 }
         if (!i && !demos)
         {
            apli_bloqueo = jj;
         }
      }
      if (demos)
      {
         fecha_protegida = 1;         
      }	  
      rw_lseek_(file1,pp,SEEK_SET);
   }
   else
   { /* es una version anterior */
      salida = -999;
      goto lee_salida;
   }
   rel += prime;
   if (rel > (MAXDEF - 1)) {rw_close_(file1);return(-1);}
   
   o_rwdebug = rwdebug;
   rwdebug = 0;
   
   dentro_de_leeaplic = 1;
   
   for (nfi = prime;nfi < rel;nfi ++) {
      if (prime != 48 && nfi == 48) 
      {
         continue;
      }
      mpfi[nfi] = NULL;
      mpfirel[nfi] = NULL;
      
      rw_read_(file1,&v,1);
      rw_read_(file1,&c,1);
      rw_read_(file1,nom,c);
      
	  if (!cambia_path_def(nom,v,nombre))
	  {
         salida = -1;
         goto lee_salida;
	  }

      if (demos && ((nfimalo+prime) == nfi || nfimalo == -1))
      {   
		  char tmpx[512];
		  sprintf(tmpx,"%s (%s)",__Texto[85],quita_path(nombre));
          aviso(0,tmpx);
		  demos = 0;
      }

      rw_read_(file1,&c,1); /* datos de enlace */
      rw_read_(file1,nom,c);
      
      /* inicialmente todos los def virtuales */      
     
      if (nfi == 48 && prime == 48) /* caso especial de empresa en modo acceso */
      {
         if (lee_defn(nombre,&(mpfi[nfi])))
         {
            salida = -2;
            goto lee_salida;
         }
         inicializa_def(nfi);
      }
      
      if (nfi > 48 && prime == 48) {
         salida = -2;
         goto lee_salida;
      }
      
      
      if (nfi != 48)
      {
         s_d[recur+nfi].salvadatos = NULL;
         s_d[recur+nfi].buf = NULL;
		 s_d[recur+nfi].tam_buf = 0;
         s_d[recur+nfi].fd = -1;
         s_d[recur+nfi].lock = 0;
         if ( !(s_d[recur+nfi].nombre_fichero = strmalloc(strlen(nombre)+1)) )
         {
            salida = -2;
            goto lee_salida;
         }
         strcpy(s_d[recur+nfi].nombre_fichero,nombre);
         if ( !(s_d[recur+nfi].enlaza_pfi = strmalloc(strlen(nom)+1)) )
         {
            salida = -2;
            nfi++;
            goto lee_salida;
         }
         strcpy(s_d[recur+nfi].enlaza_pfi,nom);
         s_d[recur+nfi].cuando = 0L;
      }
      
      rw_read_(file1,(char *)&mflagfi[nfi],sizeof(short));
      moto_short(&(mflagfi[nfi]));
      
      if (nfi == prime) {
         rw_read_(file1,nom,6);
         pp = rw_lseek_(file1,0L,SEEK_CUR);
         rw_read_(file1,(char *)&i,sizeof(short));
         moto_short(&i);
         rw_read_(file1,(char *)&m,sizeof(short));
         moto_short(&m);
         rw_read_(file1,&t,1);
         m ^= (*((char *)&t) & 0xff);
         i ^= m;
         if (memcmp(nom,"\376\332\'\370\211\342",6)) /* tiene que ir serializada */
         {
            salida = -3;
            nfi++;
            goto lee_salida;
         }
#ifdef AGIMAXPROTECT
         if (!es_una_con && !fecha_protegida && (i || m) && (i != nume_code() || m != nume_mochila()))
         {
            salida = -3;
            nfi++;
            goto lee_salida;
         }
#endif
         rw_lseek_(file1,1L,SEEK_CUR);
      }
  }
  
  for (rel = prime;rel < nfi;rel++)
  {
     if (prime != 48 && rel == 48) 
     {
        s_d[recur+rel].buf = NULL;
        continue;
     }
     if (lees_relas(rel,file1)) {
        salida = -2;
        goto lee_salida;
     }
     rw_read_(file1,(char *)&jj,sizeof(short));
     moto_short(&jj);
     if (rel != 48)
     {
        s_d[recur+rel].buf = malloc(jj+1);
		s_d[recur+rel].tam_buf = jj;
        if (!s_d[recur+rel].buf)
        {
           salida = -2;
           goto lee_salida;
        }		
     }
     else
        s_d[recur+rel].buf = NULL;
  }
  
  /* cargar programa hasta que se use */
  rw_read_(file1,(char *)&n_p_tabla,sizeof(short));
  moto_short(&n_p_tabla);
  if (n_p_tabla)
  {
     p_tabla = malloc(sizeof(short)*n_p_tabla);
     if (!p_tabla)
     {
        salida = -2;
        goto lee_salida;
     }
     proc_tabla = malloc(n_p_tabla);
     if (!proc_tabla)
     {
        salida = -2;
        goto lee_salida;
     }
     tipo_tabla = malloc(n_p_tabla);
     if (!tipo_tabla)
     {
        salida = -2;
        goto lee_salida;
     }
     rw_read_(file1,(char *)p_tabla,sizeof(short)*n_p_tabla);
     rw_read_(file1,proc_tabla,n_p_tabla);
     rw_read_(file1,tipo_tabla,n_p_tabla);
  }
  else
  {
     p_tabla = NULL;
     proc_tabla = NULL;
     tipo_tabla = NULL;
  }
  
#ifdef ESMOTOROLA
  for (_m_i = 0;_m_i < n_p_tabla;_m_i++)
  {
     moto_short(p_tabla+_m_i);
  }
#endif
  rw_read_(file1,(char *)&memoplin,sizeof(short));
  moto_short(&memoplin);
  rw_read_(file1,(char *)&maxpila,sizeof(short));
  moto_short(&maxpila);
  
  /* leer programas externos */
  rw_read_(file1,(char *)&np_ex,sizeof(short));
  moto_short(&np_ex);
  
  /* luego en todo caso optimizar memoria a bloques */
  
  p_ex = NULL;
  if (np_ex > 0) {
     p_ex = (P_EX *)malloc(sizeof(P_EX) * np_ex);
     if (!p_ex)
     {
        salida = -2;
        goto lee_salida;
     }
     rw_read_(file1,(char *)p_ex,sizeof(P_EX) * np_ex);
#ifdef ESMOTOROLA
     for (_m_i = 0; _m_i < np_ex;_m_i++)
     {
        moto_short(&(p_ex[_m_i].indice));
        moto_short(&(p_ex[_m_i].fflag));
     }
#endif
     for (i = 0;i < np_ex;i++) {
        rw_read_(file1,&c,1);
        if (!(p_ex[i].nombre = strmalloc(c)))
        {
           salida = -2;
           goto lee_salida;
        }
        rw_read_(file1,p_ex[i].nombre,c);
     }
  }
  
  /* leer memorias globales (y crear variables sistema y pila) */
  rw_read_(file1,(char *)&nm_ex,sizeof(short));
  moto_short(&nm_ex);
  i = memoplin + maxpila * 2;
  nm_ex += i;
  m_ex = (M_EX *)malloc(sizeof(M_EX) * nm_ex);
  if (!m_ex)
  {
     salida = -2;
     goto lee_salida;
  }
  
  /* inicializar variables del sistema */
  for (jj = 0;jj < i;jj++)
  {
     if (!(m_ex[jj].nombre = strmalloc(1)))
     {
        salida = -2;
        goto lee_salida;
     }
     m_ex[jj].nombre[0] = 0;
     if ( !jj || jj == 7 || jj == 11 || (jj >= memoplin && jj < (memoplin+maxpila)) )
     {
        m_ex[jj].m_tipo = DOUBLETIPO;
        if (jj == 7)
           m_ex[jj].mv.vald = -1.0;
        else
           m_ex[jj].mv.vald = 0.0;
     }
     else
     {
        m_ex[jj].m_tipo = CHARTIPO;
        if (jj < memoplin)
           m_ex[jj].mv.valc = malloc(MAXDEFECTO);
        else
           m_ex[jj].mv.valc = strmalloc(1);
        if (!m_ex[jj].mv.valc)
        {
           salida = -2;
           goto lee_salida;
        }
        if (jj == 5)
           strcpy(m_ex[jj].mv.valc,"10");
        else
           m_ex[jj].mv.valc[0] = 0;
     }
  }
  
  
  if (nm_ex > i) {
     rw_read_(file1,(char *)(m_ex+i),sizeof(M_EX) * (nm_ex-i));
#ifdef ESMOTOROLA
     for (_m_i = i; _m_i < nm_ex;_m_i++)
     {
        moto_short(&(m_ex[_m_i].indice));
        moto_short(&(m_ex[_m_i].fflag));
     }
#endif
     for (;i < nm_ex;i++)
     {
        rw_read_(file1,&c,1);
        if (!(m_ex[i].nombre = strmalloc(c)))
        {
           salida = -2;
           goto lee_salida;
        }
        
        rw_read_(file1,m_ex[i].nombre,c);
		/*
		if (strstr(m_ex[i].nombre,"Divisa")) {
			i = i;
		}
		*/

        if (m_ex[i].m_tipo == CHARTIPO || m_ex[i].m_tipo == FECHATIPO)
        {
           rw_read_(file1,&c,1);
           if (!(m_ex[i].mv.valc = strmalloc(c)))
           {
              salida = -2;
              goto lee_salida;
           }
           rw_read_(file1,m_ex[i].mv.valc,c);
		   if (!strcmp("EUROTEXTO",m_ex[i].nombre))
		   {
			   free(m_ex[i].mv.valc);
			   if (!(m_ex[i].mv.valc = strmalloc(strlen(DsEuroTexto)+1)))
			   {
				  salida = -2;
				  goto lee_salida;
			   }
			   strcpy(m_ex[i].mv.valc,DsEuroTexto);
		   }
        }
		else
		{
		    if (!strcmp("EURO",m_ex[i].nombre))
			{ 							
			    m_ex[i].mv.vald = DsEuroConvierte;
			} else if (!strcmp("EURODV",m_ex[i].nombre))
			{
				m_ex[i].mv.vald = (double)DsDeciModoOtro;
			} else if (!strcmp("EURODB",m_ex[i].nombre))
			{
				m_ex[i].mv.vald = (double)DsDeciModoNormal;
			}			
		}
     }
  }
  
  /* crear tabla de procesos */
  
  rw_read_(file1,(char *)&nen_pr,sizeof(short));
  moto_short(&nen_pr);
  
  if (nen_pr)
  {
     en_pr = (struct enlaza_prc *)malloc(sizeof(struct enlaza_prc) * (nen_pr+2));
     if (!en_pr)
     {
        salida = -2;
        goto lee_salida;
     }
     memset(en_pr,0,sizeof(struct enlaza_prc) * (nen_pr+1));
     
     tmp0 = quita_path(aplic);
     if (tmp0 == aplic)
     {
        tmp0 = "";
        j = 0;
     }
     else
     {
        cc = *tmp0;
        *tmp0 = 0;
        j = strlen(aplic);
     }
     for (i = 0;i < nen_pr;i++)
     {
        en_pr[i].tabla_gosubs = NULL;
        en_pr[i].e_nprog = 0;
        en_pr[i].e_ncond = 0;
        en_pr[i].e_nbucl = 0;
        en_pr[i].e_noper = 0;
        en_pr[i].e_nmemo = 0;
        en_pr[i].e_nf_B = 0;
        en_pr[i].e_prog = NULL;
        en_pr[i].e_cond = NULL;
        en_pr[i].e_bucl = NULL;
        en_pr[i].e_oper = NULL;
        en_pr[i].e_memo = NULL;
        en_pr[i].e_f_B = NULL;
        en_pr[i].prc = NULL;
        en_pr[i].tabla_externa = NULL;
        en_pr[i].tabla_ficheros = NULL;
        en_pr[i].e_usado = 0L;
        en_pr[i].activo = 0;
        rw_read_(file1,&c,1);
        en_pr[i].prc = strmalloc(c+j);
        if (!en_pr[i].prc)
        {
           salida = -2;
           goto lee_salida;
        }
        if (j)
           strcpy(en_pr[i].prc,aplic);
        rw_read_(file1,en_pr[i].prc+j,c);
        rw_read_(file1,&jj,sizeof(short));
        moto_short(&jj);
        /* total memos */
        en_pr[i].e_nmemo = 0;
        en_pr[i].e_memo = (T_MEMORIA *)malloc(sizeof(T_MEMORIA) * jj);
        if (jj && !en_pr[i].e_memo)
        {
           salida = -2;
           goto lee_salida;
        }
        rw_read_(file1,&jj,sizeof(short));
        moto_short(&jj);
        en_pr[i].tabla_gosubs = (short *)malloc(sizeof(short) * jj);
        if (jj && !en_pr[i].tabla_gosubs)
        {
           salida = -2;
           goto lee_salida;
        }
        rw_read_(file1,en_pr[i].tabla_gosubs,sizeof(short) * jj);

#ifdef ESMOTOROLA
		 for (_m_i = 0; _m_i < jj;_m_i++)
		 {
			moto_short(&(en_pr[i].tabla_gosubs[_m_i]));
		 }
#endif


     }
     if (*tmp0)
     {
        *tmp0 = cc;
     }
  }
  else
     en_pr = NULL;
  programa_activo = -1;
  c_programa_en_uso = 0L;
  
  if (nen_pr)
  {
     en_pr[nen_pr].prc = "informes";
     en_pr[nen_pr].tabla_ficheros = (short *)reserva_malloc(sizeof(short) * 48,12);
     for (j = 0;j < 48;j++)
     {
        en_pr[nen_pr].tabla_ficheros[j] = j;
     }
  }
  
  if (prime == 48)
     goto lee_salida;
  
  if (ini_reserva(sizeof(FI_DEF),0))
  {
     salida = -2;
     goto lee_salida;
  }
  rw_read_(file1,&(max_panta),sizeof(short));
  moto_short(&max_panta);
  if (ini_reserva(max_panta*sizeof(PAN_DEF),1))
  {
     salida = -2;
     goto lee_salida;
  }
  rw_read_(file1,&(max_campos),sizeof(short));
  moto_short(&max_campos);
  if (ini_reserva(max_campos*sizeof(CAMPO_DEF),2))
  {
     salida = -2;
     goto lee_salida;
  }
  rw_read_(file1,&(max_clave),sizeof(short));
  moto_short(&max_clave);
  if (ini_reserva(max_clave*sizeof(KEY_DEF),3))
  {
     salida = -2;
     goto lee_salida;
  }
  rw_read_(file1,&(max_relaci),sizeof(short));
  moto_short(&max_relaci);
  if (ini_reserva(max_relaci*sizeof(REL_DEF),4))
  {
     salida = -2;
     goto lee_salida;
  }
  rw_read_(file1,&(max_gene),sizeof(short));
  moto_short(&max_gene);
  if (ini_reserva(max_gene,5))
  {
     salida = -2;
     goto lee_salida;
  }
  rw_read_(file1,&(max_prog),sizeof(short));
  moto_short(&max_prog);
  if (ini_reserva(max_prog*sizeof(PROGRAM),6))
  {
     salida = -2;
     goto lee_salida;
  }
  rw_read_(file1,&(max_cond),sizeof(short));
  moto_short(&max_cond);
  if (ini_reserva(max_cond*sizeof(CONDIC),7))
  {
     salida = -2;
     goto lee_salida;
  }
  rw_read_(file1,&(max_bucl),sizeof(short));
  moto_short(&max_bucl);
  if (ini_reserva(max_bucl*sizeof(BUCLE),8))
  {
     salida = -2;
     goto lee_salida;
  }
  rw_read_(file1,&(max_oper),sizeof(short));
  moto_short(&max_oper);
  if (ini_reserva(max_oper*sizeof(OPERA),9))
  {
     salida = -2;
     goto lee_salida;
  }
  rw_read_(file1,&(max_f_B),sizeof(short));
  moto_short(&max_f_B);
  if (ini_reserva(max_f_B*sizeof(struct _filebucle),10))
  {
     salida = -2;
     goto lee_salida;
  }
  rw_read_(file1,&(max_ex),sizeof(short));
  moto_short(&max_ex);
  if (ini_reserva(max_ex*sizeof(short),11))
  {
     salida = -2;
     goto lee_salida;
  }
  if (ini_reserva(48*sizeof(short),12))
  {
     salida = -2;
     goto lee_salida;
  }
  
  limi1 = 0;
  ll1 = rw_lseek_(file1,0L,SEEK_CUR);
  ll2 = rw_lseek_(file1,0L,SEEK_END);
  if (ll1 != ll2)
  {  
     rw_lseek_(file1,ll2-sizeof(short),SEEK_SET);
     if (rw_read_(file1,&(limi1),sizeof(short)) != sizeof(short))
     {
        limi1 = 0;
     }
     else
     {
        if (limi1 == 1234)
        {
           rw_lseek_(file1,ll2-(sizeof(short)*2),SEEK_SET);
           if (rw_read_(file1,&(limi1),sizeof(short)) != sizeof(short))
           {
              limi1 = 0;
           }
        }
        else
        {
           limi1 = 0;
        }
     }
  }
  if (limitacion_especial != limi1)
  {
     if (limitacion_especial)
        libera_limitacion();
     limitacion_especial = limi1;
     if (limitacion_especial)
        carga_limitacion(aplic);
  }
  
lee_salida:
  rw_close_(file1);
  
  if (salida < 0) {
     nfi--;
     while(nfi >= prime) {
        if (nfi != 48)
        {
           if (s_d[recur+nfi].nombre_fichero)
              free(s_d[recur+nfi].nombre_fichero);
           if (s_d[recur+nfi].enlaza_pfi)
              free(s_d[recur+nfi].enlaza_pfi);
        }
        if (mpfi[nfi])
           free_defpri(mpfi[nfi]);
        else
        {
           if (s_d[nfi+recur].buf)
              free(s_d[nfi+recur].buf);
        }
        if (nfi != 48)
        {
           if (s_d[recur+nfi].nombre_fichero)
              free(s_d[recur+nfi].nombre_fichero);
           if (s_d[recur+nfi].enlaza_pfi)
              free(s_d[recur+nfi].enlaza_pfi);
           if (s_d[nfi+recur].salvadatos)
           {
              free(s_d[nfi+recur].salvadatos);
              s_d[nfi+recur].salvadatos = NULL;
           }
        }
        libera_rela(nfi);
        nfi--;
			  }
     nfi = 0;
     libera_prog();
  }
  rwdebug = o_rwdebug;
super_salida:
  dentro_de_leeaplic = 0;
  
  if (apli_bloqueo != -1 && salida >= 0)
  {	  
     if ((como_bloqueo = bloquea_aplicacion(apli_bloqueo)) < 0)
			  {
        if (como_bloqueo == -2)
        {
           aviso(0,"HA EXCEDIDO EL NUMERO DE PUESTOS CONTRATADOS");
        }
        if (como_bloqueo == -3)
        {
           aviso(0,"HA EXCEDIDO EL NUMERO DE ACCESOS CONTRATADOS");
        }
        apli_bloqueo = -1;
        libera_aplic(prime);
        salida = -1;
			  }
  }
  else
     apli_bloqueo = -1;
	
  return(salida);
}

int libera_rela(qj)
int qj;
{
   if (mpfirel[qj]) {
      if (mpfirel[qj]->nrel > 0) {
         free((char *)mpfirel[qj]->relacis);
         free((char *)mpfirel[qj]->camrela);
         free((char *)mpfirel[qj]->camminu);
         free((char *)mpfirel[qj]->numrela);
      }
      if (mpfirel[qj]->nrelact > 0)
         free((char *)mpfirel[qj]->relact);
      if (mpfirel[qj]->nreldef > 0)
         free((char *)mpfirel[qj]->reldef);
      if (mpfirel[qj]->nrelpin > 0)
         free((char *)mpfirel[qj]->relpin);
      free((char *)mpfirel[qj]);
   }
}

static void libera_prog()
{
   int i,nn;
   
   if (en_pr)
      for (nn = 0;nn < nen_pr;nn++)
      {
         
         if (en_pr[nn].prc)
            free(en_pr[nn].prc);
         
         if (en_pr[nn].tabla_gosubs)
         {
            free(en_pr[nn].tabla_gosubs);
            en_pr[nn].tabla_gosubs = NULL;
         }
         
         nprog = en_pr[nn].e_nprog;
         ncond = en_pr[nn].e_ncond;
         nbucl = en_pr[nn].e_nbucl;
         noper = en_pr[nn].e_noper;
         nmemo = en_pr[nn].e_nmemo;
         nf_B = en_pr[nn].e_nf_B;
         prog = en_pr[nn].e_prog;
         cond = en_pr[nn].e_cond;
         bucl = en_pr[nn].e_bucl;
         oper = en_pr[nn].e_oper;
         memo = en_pr[nn].e_memo;
         f_B  = en_pr[nn].e_f_B;
         
         if (memo)
            for (i = 0;i < nmemo;i++)
            {
               if (memo[i].m_tipo == CHARTIPO || memo[i].m_tipo == FECHATIPO)
               {
                  if (memo[i].mv.valc)
                     free(memo[i].mv.valc);
               }
            }
            
            if (f_B)
               for (i = 0;i < nf_B;i++)
               {
                  if (f_B[i].ord1)
                     free(f_B[i].ord1);
                  if (f_B[i].ord2)
                     free(f_B[i].ord2);
                  if (f_B[i].comp)
                     free(f_B[i].comp);
                  if (f_B[i].calc)
                     free(f_B[i].calc);
               }
               if (en_pr[nn].tabla_externa)
                  reserva_free((char *)en_pr[nn].tabla_externa,11);
               if (en_pr[nn].tabla_ficheros)
                  reserva_free((char *)en_pr[nn].tabla_ficheros,12);
               if (prog)
                  reserva_free((char *)prog,6);
               if (oper)
                  reserva_free((char *)oper,9);
               if (cond)
                  reserva_free((char *)cond,7);
               if (memo)
                  free((char *)memo);
               if (bucl)
                  reserva_free((char *)bucl,8);
               if (f_B)
                  reserva_free((char *)f_B,10);
      }
      
      if (m_ex)
         for (i = 0;i < nm_ex;i++) {
            if (m_ex[i].nombre)
               free(m_ex[i].nombre);
            if (m_ex[i].m_tipo == CHARTIPO || m_ex[i].m_tipo == FECHATIPO) {
               if (m_ex[i].mv.valc)
                  free(m_ex[i].mv.valc);
            }
         }
         
         if (p_ex)
            for (i = 0;i < np_ex;i++)
            {
               if (p_ex[i].nombre)
                  free(p_ex[i].nombre);
            }
            
            if (m_ex)
               free((char *)m_ex);
            if (p_ex)
               free((char *)p_ex);
            m_ex = (M_EX *)0;
            p_ex = (P_EX *)0;
            
            nprog = 0;
            ncond = 0;
            nbucl = 0;
            noper = 0;
            nmemo = 0;
            nf_B = 0;
            nm_ex = 0;
            np_ex = 0;
            prog = (PROGRAM *)0;
            cond = (CONDIC *)0;
            bucl = (BUCLE *)0;
            oper = (OPERA *)0;
            memo = (T_MEMORIA *)0;
            f_B  = (struct _filebucle *)0;
            
            
            if (p_tabla)
               free(p_tabla);
            if (proc_tabla)
               free(proc_tabla);
            if (tipo_tabla)
               free(tipo_tabla);
            p_tabla = NULL;
            proc_tabla = NULL;
            tipo_tabla = NULL;
            n_p_tabla = 0;
            if (en_pr)
            {
               if (en_pr[nen_pr].tabla_ficheros)
                  reserva_free((char *)en_pr[nen_pr].tabla_ficheros,12);
               free(en_pr);
            }
            nen_pr = 0;
            en_pr = NULL;
}

libera_aplic(prime)
int prime;
{
   int qj;
   int i;
   int recur = (recur_punto < 1) ? 0 : recur_punto * MAXDEF;
   int o_rwdebug = rwdebug;
   rwdebug = 0;
   

   
   if (limitacion_especial)
   {
      libera_limitacion();
   }
   
   if (prime == 0) {
      for (qj=0;qj<nfi;qj++) {
         if (qj == 48) continue;		 
         if (mpfi[qj])
		 {
            free_defpri(mpfi[qj]);			
		 }
         else
         {
            if (s_d[qj+recur].buf)  
               free(s_d[qj+recur].buf);
         }
         if (qj != 48)
         {
            free(s_d[recur+qj].nombre_fichero);
            free(s_d[recur+qj].enlaza_pfi);
            if (s_d[qj+recur].salvadatos)
            {
               free(s_d[qj+recur].salvadatos);
               s_d[qj+recur].salvadatos = NULL;
            }
         }
         mpfi[qj] = NULL;
         libera_rela(qj);
      }
   }
   nfi = 0;
   
   /*
   
            m_ex = (M_EX *)0;
            p_ex = (P_EX *)0;
            
            nprog = 0;
            ncond = 0;
            nbucl = 0;
            noper = 0;
            nmemo = 0;
            nf_B = 0;
            nm_ex = 0;
            np_ex = 0;
            prog = (PROGRAM *)0;
            cond = (CONDIC *)0;
            bucl = (BUCLE *)0;
            oper = (OPERA *)0;
            memo = (T_MEMORIA *)0;
            f_B  = (struct _filebucle *)0;            
            
            p_tabla = NULL;
            proc_tabla = NULL;
            tipo_tabla = NULL;
            n_p_tabla = 0;
            nen_pr = 0;
            en_pr = NULL;
*/
   libera_prog();

      
   
   rwdebug = o_rwdebug;
   if (apli_bloqueo != -1)
   {
	   
      desbloquea_aplicacion(apli_bloqueo);
	  
      apli_bloqueo = -1;
   }
   
   return(0);
}


