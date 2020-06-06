
/**********************************************************************/
/**********************************************************************/
/*                         DIAGRAM9                                   */
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
#ifndef LARGESEEK
extern long rw_lseek_();
#endif
extern unsigned int rw_read_();
#endif



#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <errno.h>
#ifdef ESDOS
#ifndef FGV32
#include <dos.h>
#endif
#include <process.h>
#endif

#ifdef FGVWIN
#include <FGVWIN.h>
#endif

#include <rwmalloc.h>
#include <rtermin.h>
#include <rentra.h>
#include <rwmenus.h>
#include <rsignal.h>
#include <teclas.h>
#define _NUEVODEF
#include <rfecha.h>
#include <rdef.h>       /* ya incluye el isam.h */
#include <rmante.h>
#include <rcalcul.h>


#ifdef ESDOS
extern int (*timer_especial_int)();
#endif


#ifdef FGV32
#ifndef FGVWIN
#ifndef WATCOMNLM
extern HANDLE myconsola;
extern HANDLE hStdIn;
#endif
#endif
#endif

#ifdef RWXARXA
extern int en_conexion_terminal;   
#endif

#ifdef RWSERVER
extern int en_conexion_externa;
#endif

extern char *rw_modulo;

extern char other_server[128];
extern char server_request[128];
extern char segundo_dir[128];
extern int server_id;

extern char *agi_aplicacion;
extern char *agi_empresa;
extern char *agi_codi_empre;

extern int is_alreves;

#include <stdlib.h>
/*extern long atol();*/

extern char empresa[MAXDEFECTO];
extern char proceso[81];
extern char titulo[81];

extern int ixx_absoluto;

extern char rw_version[256];

extern char *dirbase_ayuda;

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
extern char *alt_dirfic;

extern int seted_dir;

#ifdef FGV32

extern BOOL fIsTNT;

#endif


extern int rentra_atrib;
extern int rentra_sal_atrib;
extern int rentra_cc;
extern int rentra_cf;
extern int rentra_sal_cc;
extern int rentra_sal_cf;

extern int   mimodo_debug;

extern char *logger_salida;

extern int auto_empresa;

extern int es_la_doble_;
extern int va_con_doble_;
extern char nomapli_doble[260];
extern char empresa_doble[260];

extern int apli_aborto; /* flag para abortar la aplicacion (QUIT) */

extern char *rw_usuario;
extern int fecha_protegida;
extern char *numero_serie;
extern int issingleuser; /* flag mono usuario del disam */

extern char clave_help[32];
extern char referencia_help[128];

extern int modo_de_relacion;

extern int user_who;
extern char rw_user[11];
extern char *quita_path();

extern int apli_bloqueo;

extern int subnumero_de_usuario;

/* extern int agiavisa(); */
extern int agisies;
extern int agivez;
extern int magi;
extern int agitei;
extern int teiaviso;
extern int teinolohagas;
extern int vesylee;


extern int pasa_a_int(double);

extern int bucle_break;

extern int translevel;

extern char *aginfo;
extern int LEERTERMINAL;
extern char nmen0[551];

extern char prog_rw[128];
extern char par1[551];
extern char par2[551];

#ifdef FGVWIN
extern char **tabla_icon;
extern short *punto_tabla;
extern int ntabla_icon;
#endif

extern int e_contador; /* control de contadores */

extern long trata_memo();

extern long da_registo_actual();

extern double ld_doble();

extern int ultimo_ipfi;

extern int wdesplegamenu;

extern int es_el_modulo_erp;


int fuerza_empresa = 0;

int  cambio_color_fijo = 0;
int  que_color_inicio = 0;
char *color_factor_inicio = NULL;
int  campo_color = 0;
int  file_color = -1;
int  que_color = 0;
int  atributo_1 = 0;
int  atributo_2 = 0;
char *color_factor = NULL;
int  len_color = 0;
int p_color = 0;
unsigned char l_separador = 0;
int t_ini = -1;
int t_fin = -1;

extern struct limitacion_ /* alineada a 8 bytes */
{
   char fichero[12]; 
   char desde[260];
   char hasta[260];
   long campo;
   long registros;
   long modo;
} *limitaciones;
extern int numero_limitaciones;
extern int **relacion_limitacion;


static int control_exec = 0;

static int  punto = 0;
#define MAXMENUA 15
static char  menua[25][MAXMENUA+1];
static char *_n[10];

static int enlaza_nm_ex = 0;
M_EX *enlaza_m_ex = NULL;

static int enlaza_n_ndeci = 0;
M_EX *enlaza_ndeci = NULL;

extern double DsEuroConvierte; 
extern int DsDeciModoOtro;
extern int DsDeciModoNormal;
extern char DsEuroTexto[512];


extern char no_instala[30];

extern int recur_punto;
extern int recur_nfi[MAXRECUR];
extern void *recur_mpfi[MAXRECUR];
extern struct enlaza_prc *salva_procs[MAXRECUR];
extern short nsalva_procs[MAXRECUR];

extern int es_menu_acceso_directo;
int con_acceso_directo = 0;

static char fich_apli[15] = {0,};

extern long nume_serie;

/********************************************************************
MODULO TRACER
********************************************************************/
extern int tracer_int; /* esta en la entrada de datos */
extern int pinta_free;

/* mapa general */
static char *tabla_tcal = NULL;
/* mapa de un calculo (informacion simbolica) */
static char *tabla_tvar = NULL;
static char *tabla_tcam = NULL;
static char *tabla_tfic = NULL;
extern char bufpan[PANBUFSIZE];

int ntrace_point = 0;
char trace_point[50][100];
char trace_pointipo[50];
char trace_pointval[50][300];
char trace_pointfijo[50][300];

int tracer_break[50];
int ntr_br = 0;
int hay_traza = 0;

static char tfichero[10] = {0,0,0,0,0,0,0,0,0,0};
static char taplica[51];
static int  nftodo = 0;
static int limi_ftodo = 0;
static char **ftodo = NULL;
static int ftodo_nivel = 0;
static short dtf;
static short dtc;
static int ftd = 0;
static int traza_w = 0;

static struct _user_breakpoints 
{
  char _tfichero[10];
  int _tracer_break[50];
  int _ntr_br;
} user_breakpoints[50];
static int nuser_breakpoints = 0;


#ifndef DSMINIBASICO

int busca_externa(char *n)
{
  int i,j;
  for (i = memoplin+maxpila*2;i < nm_ex;i++)
   {
      if (!strcmp(m_ex[i].nombre,n))
	  {
         /*i -= (memoplin+maxpila*2);*/
         for (j = 0;en_pr[programa_activo].tabla_externa[j] != -2;j++)
         {
            if (en_pr[programa_activo].tabla_externa[j] == i)
            {               
               return(5000+memoplin+maxpila*2+j);
            }
         }      
         return(-1);
	  }
   }
  return -1;
}

static short busca_tvar(n)
char *n;
{
   short i,f,d,r = -1,j;
   char tmp[15];
   
   ftd = 0;
   
   if (strcmp(n,"Campo") == 0) r = 5000;
   if (strcmp(n,"Contenido") == 0) r = 5001;
   if (strcmp(n,"Control") == 0) r = 5002;
   if (strcmp(n,"FEntrada") == 0) r = 5003;
   if (strcmp(n,"FSalida") == 0) r = 5004;
   if (strcmp(n,"E_sup") == 0) r = 5005;
   if (strcmp(n,"E_inf") == 0) r = 5006;
   if (strcmp(n,"Pos") == 0) r = 5007;
   if (strcmp(n,"Usuario") == 0) r = 5008;
   if (strcmp(n,"Impresora") == 0) r = 5009;
   if (strcmp(n,"PARAMETRO") == 0) r = 5010;
   
   if (r > -1) 
   {
      ftd = 1;
      return(r);
   }
   
   /* mirar en externas */
   i = busca_externa(n);
   if (i > -1)
   {
	   ftd = 1;
	   return i;
   }
 
   
   if (tabla_tvar) 
   {
	   char ntmp[512];

	   strcpy(ntmp,n);
	   ntmp[13] = 0;
	   quita_blancos(ntmp);
      
#ifdef ESMOTOROLA
      /* ya convertido */
      memcpy(&f,tabla_tvar,2);
#else
      f = *((short *)tabla_tvar);
#endif
      
      
      d = sizeof(short);
      for (i = 0;i < f;i++) 
	  {
         memcpy(tmp,tabla_tvar+d,13);
         tmp[13] = '\0';
         quita_blancos(tmp);
         if (!strcmp(tmp,ntmp)) 
		 {
            ftd = 1;
#ifdef ESMOTOROLA
            memcpy(&i,tabla_tvar+d+13,2);
            moto_short(&i);
            return(i);
#else     
            return((short)*((short *)(tabla_tvar+d+13)));
#endif     
         }
         d += 17;
	  }
      
   }
   
   
   return(0);
}

static int busca_tfiche()
{
   short i,f,d;
#ifdef ESMOTOROLA
   short tmp_m;
#endif
   ftd = 0;
   
   if (tabla_tfic) {
						
#ifdef ESMOTOROLA
						memcpy(&f,tabla_tfic,2);
#else
                  f = *((short *)tabla_tfic);
#endif
                  d = sizeof(short);
                  for (i = 0;i < f;i++) {
#ifdef ESMOTOROLA
                     memcpy(&tmp_m,tabla_tfic+d,2);
                     moto_short(&tmp_m);
                     if (dtf == tmp_m) {
                        ftd = 1;
                        memcpy(&dtf,tabla_tfic+d+4,2);
                        moto_short(&dtf);
                        dtf = v_prog(dtf);
                        return;
                     }
#else
                     if (dtf == (short)*((short *)(tabla_tfic+d))  ) {
                        ftd = 1;
                        dtf = (short)*((short *)(tabla_tfic+d+4));
                        dtf = v_prog(dtf);
                        return;
                     }
#endif
                     d += 8;
                  }
                  
   }
   else {
						
						if ((dtf > -1 && dtf < nfi) || dtf == 48)
                     ftd = 1;
                  
   }
   
}

static int td_campo(char *tdato)
{
   short final,i,f,d;
   char tmp[82];
 
   ftd = 0;
   dtf = -1;
   
   if (tdato[0] != '#' && tabla_tcam)
   {
						
#ifdef ESMOTOROLA
						memcpy(&f,tabla_tcam,2);
#else
                  f = *((short *)tabla_tcam);
#endif
                  d = sizeof(short);
                  for (i = 0;i < f;i++) 
				  {
                     memcpy(tmp,tabla_tcam+d,13);
                     tmp[13] = '\0';
                     quita_blancos(tmp);
                     if (!strcmp(tmp,tdato)) 
					 {
#ifdef ESMOTOROLA
                        memcpy(&dtf,tabla_tcam+d+13,2);
                        memcpy(&dtc,tabla_tcam+d+17,2);
                        moto_short(&dtf);
                        moto_short(&dtc);
#else
                        dtf = *((short *)(tabla_tcam+d+13));
                        dtc = *((short *)(tabla_tcam+d+17));
#endif
                        busca_tfiche();	 
                        return(0);
                     }
                     d += 21;
                  }                  
   }
   
   if (tdato[0] == '#' && tdato[1]) 
   {
						final = busca_fin(tdato+1,'\0');
                  if (!final)
                  {
                     final = busca_fin(tdato+1,'#');
                     if (!final || tdato[final+1] != '#')
                        return(-1); /* ilegal */
                     tdato[final+1] = 0;
                     if (tdato[final] == '@')
                     {
                        dtf = busca_tvar(tdato+1);
						ftd = 0;
                        if (dtf > -1)
                        {
							int f,c,t;
							M_VAL val;

	    					f = -1;
							c = dtc;
							t = busca_valor(&f,&c,&val);
							if ( t == CHARTIPO || t == FECHATIPO )
							   dtf = -1;
							else 
							{		   		
								dtf = (int)val.vald;
								if (dtf < 0 || dtf >= nfi)
								{
								    dtf = -1;
								}
								else
								{
									dtf = v_prog(dtf);
								}
							}
                        }
                     }
                     else
                        dtf = ind_rel(tdato+1);
					 if (dtf > -1)
					 {
						 i = final;
						 final = busca_fin(tdato+i+2,'#');
						 if (tdato[i+2+final] == '#')
						 {
							tdato[i+2+final] = 0;
							dtc = -(busca_tvar(tdato + i + 2) + 1);
							ftd = 0;
						 }
						 else
						 {
							dtc = atoi(tdato+i+2);
							if (dtc < 0 || dtc >= v_mpfi(dtf)->f_ncampos)
							{
								dtf = -1;
							}
						 }						 
					 }
					 if (dtf > -1) 
					 {
						 ftd = 1;
					 }
                  }
                  else
                  {
                     if (!tdato[final+1])
                        return(-1);
                     dtf = (int)vals(tdato,1,final);
                     if (dtf > -1)
                        busca_tfiche();
                     if (tdato[final+1] == '#')
                        dtc = atoi(tdato+final+2);
                     else
                     {
                        dtc = -(busca_tvar(tdato + final + 1) + 1);
                     }
                  }
   }
   else 
   {
						dtc = busca_tvar(tdato);
   }
   
   return(0);
}


static unsigned char b_buf[514];

int decripto(buf)
unsigned char *buf;
{
   short k;  
   short i,n;
   unsigned short nn,mm,hh;
#ifdef ESMOTOROLA
   unsigned short t_t;
   unsigned short kk[256],ll[256];
#else
   unsigned short *kk,*ll;
#endif
   
   
#ifdef ESMOTOROLA
   memcpy(kk,buf,512);
#else   
   kk = (unsigned short *)buf;
#endif
   
   nn = nume_mochila(); /* decripto2 */
   
   for(i = 0;i < 256;i++)
   {
#ifdef ESMOTOROLA 
      for (n = 0;n < ((nn & 0xff) & 0xf);n++)
      {
         moto_short(&(kk[i]));
         hh = kk[i] & 0x8000;
         hh >>= 15;
         kk[i] <<= 1;
         kk[i] |= hh;
         moto_short(&(kk[i]));     
      }
#else
      for (n = 0;n < ((nn & 0xff) & 0xf);n++)
      {
         hh = kk[i] & 0x8000;
         hh >>= 15;
         kk[i] <<= 1;
         kk[i] |= hh;
      }
#endif
   }
#ifdef ESMOTOROLA
   memcpy(buf,kk,512);
   memcpy(ll,buf+1,508);
#else
   ll = (unsigned short *)( ((unsigned char *)buf+1) );
#endif
   
   for (n = 0;n < 254;)
   {
#ifdef ESMOTOROLA
      for(i = 0;i < (nn & 0xff) && n < 254;i++)
      {
         moto_short(&(ll[n]));
         ll[n] ^= ((((nn & 0xff00) >> 8) & 0xf) + i);
         moto_short(&(ll[n]));
         n++;
      }
#else
      for(i = 0;i < (nn & 0xff) && n < 254;i++)
      {
         ll[n] ^= (((nn & 0xff00) >> 8) & 0xf) + i;
         n++;
      }
#endif    
   }
   
#ifdef ESMOTOROLA
   memcpy(buf+1,ll,508);
   memcpy(kk,buf,512);
#endif
   
   for(i = 0;i < 256;i++)
   {
      moto_short(&(kk[i]));         
      kk[i] ^= nn;
      moto_short(&(kk[i]));    
   }
   
   /* decripto */
   nn = nume_code();
   /* moto_short(&nn); */
   
   if (!nn) {
      return(-1);
   }
   
   moto_short(&(kk[255]));
   mm = kk[255] ^ nn;
   moto_short(&(kk[255]));
   
   for(i = 0;i < 100;i++)
   {
#ifdef ESMOTOROLA       
      for (n = 0;n < ((mm & 0xff) & 0xf);n++)
      {
         moto_short(&(kk[i]));
         hh = kk[i] & 0x8000;
         hh >>= 15;
         kk[i] <<= 1;
         kk[i] |= hh;
         moto_short(&(kk[i]));
      }       
#else
      for (n = 0;n < ((mm & 0xff) & 0xf);n++)
      {
         hh = kk[i] & 0x8000;
         hh >>= 15;
         kk[i] <<= 1;
         kk[i] |= hh;
      }
#endif       
   }
   for(i = 100;i < 155;i++)
   {
#ifdef ESMOTOROLA
      for (n = 0;n < (((mm & 0xff00) >> 8) & 0xf);n++)
      {
         moto_short(&(kk[i]));
         hh = kk[i] & 0x8000;
         hh >>= 15;
         kk[i] <<= 1;
         kk[i] |= hh;
         moto_short(&(kk[i]));
      }
#else
      for (n = 0;n < (((mm & 0xff00) >> 8) & 0xf);n++)
      {
         hh = kk[i] & 0x8000;
         hh >>= 15;
         kk[i] <<= 1;
         kk[i] |= hh;
      }
#endif       
   }
   mm = 0;
   for(i = 0;i < 255;i++)
   {
      moto_short(&(kk[i]));
      mm ^= kk[i];
      moto_short(&(kk[i]));
   }
   /* printf("mm=%X\n",mm); */
   mm ^= nn;
   moto_short(&(kk[255]));
   if (mm != kk[255])
   {
      /*    printf("PARITY ERROR!!\n"); */
   }
   moto_short(&(kk[255]));
   for(i = 0;i < 255;i++)
   {
#ifdef ESMOTOROLA
      for (n = 0;n < ((nn & 0xff) & 0xf);n++)
      {
         moto_short(&(kk[i]));                              
         hh = kk[i] & 0x8000;
         hh >>= 15;
         kk[i] <<= 1;
         kk[i] |= hh;
         moto_short(&(kk[i]));                                   
      }
#else   
      for (n = 0;n < ((nn & 0xff) & 0xf);n++)
      {
         hh = kk[i] & 0x8000;
         hh >>= 15;
         kk[i] <<= 1;
         kk[i] |= hh;
      }
#endif       
   }
#ifdef ESMOTOROLA
   memcpy(buf,kk,512);
   memcpy(ll,buf+1,508);
#else  
   ll = (unsigned short *)( ((unsigned char *)buf+1) );
#endif
   for (n = 0;n < 254;)
   {
#ifdef ESMOTOROLA
      for(i = 0;i < (nn & 0xff) && n < 254;i++)
      {
         moto_short(&(ll[n]));
         ll[n] ^= (((nn & 0xff00) >> 8) & 0xf) + i;
         moto_short(&(ll[n]));    
         n++;
      }
#else   
      for(i = 0;i < (nn & 0xff) && n < 254;i++)
      {
         ll[n] ^= (((nn & 0xff00) >> 8) & 0xf) + i;
         n++;
      }
#endif    
   }
   
#ifdef ESMOTOROLA
   memcpy(buf+1,ll,508);
   memcpy(kk,buf,512);
#endif
   
   for(i = 0;i < 255;i++)
   {
      moto_short(&(kk[i]));         
      kk[i] ^= nn;
      moto_short(&(kk[i]));           
   }
   
#ifdef ESMOTOROLA
   memcpy(buf,kk,512);
#endif
   
   return(0);
}


int check_tracepoints()
{
	int i;
	char tmp[5000];
    extern int c_rw_error;
    extern int no_rw_error;

	for (i = 0;i < ntrace_point;i++)
	{
		if (!trace_point[i][0])
			continue;
		td_campo(trace_point[i]);
        if (ftd) 
		{
			int f,c,t;
			M_VAL val;

	    	f = dtf;
		    c = dtc;
  		    no_rw_error = 1;
			c_rw_error = 0;
		    t = busca_valor(&f,&c,&val);
			no_rw_error = 0;
			if (c_rw_error)
			   continue;

		    if ( t == CHARTIPO || t == FECHATIPO )
			   strcpy(tmp,val.valc);
		    else 
			{
		   	   sprintf(tmp,"%lf",val.vald);
			   quitadec(tmp);
		    }
			tmp[299] = 0;

			if (strcmp(tmp,trace_pointval[i]))
			{
				strcpy(trace_pointval[i],tmp);
				if (trace_pointipo[i] ==  0)
				{
					tracer_int = 1;					
				}
				else if (trace_pointipo[i] == 4 && strcmp(tmp,trace_pointfijo[i]) > 0)
				{
					tracer_int = 1;
				}
				else if (trace_pointipo[i] == 3 && strcmp(tmp,trace_pointfijo[i]) < 0)
				{
					tracer_int = 1;
				}
				else if (trace_pointipo[i] == 2 && strcmp(tmp,trace_pointfijo[i]))
				{
					tracer_int = 1;
				}
				else if (trace_pointipo[i] == 1 && !strcmp(tmp,trace_pointfijo[i]))
				{
					tracer_int = 1;
				}
			}
	    }
	}
	return 0;	
}

int ej_traza(p)
int p;
{
   char tmp[15000],b;
   char tmp0[512];
   short linea,i,n,kk,j;
   short s;
   int f = -1;
#ifdef ESMOTOROLA
   long tmpm1;
#endif
   static int nplin = 2;
   static int p2 = 380;
   extern int c_rw_error;
   extern int no_rw_error;

   
   if (!hay_traza) return;
   
   if (p < 0)   {
      if (ftodo)
      {
         for (i = 0;i < nftodo;i++)
            free(ftodo[i]);   
         free((char *)ftodo);
         ftodo = NULL;
      }
      nftodo = 0;
      tfichero[0] = '\0';
      if (traza_w) 
      {         
         if (DespachadorExterno)
			{
			char nada[2];
			nada[0] = 0;
            (*DespachadorExterno)((int)DESPACHO_FIN_TRAZA,(int)0,nada);   
			}                 
         traza_w = 0;
      }
      return(0);
   }
   
   if (!nprog) return(0);   
   
 
   
   if (ftodo_nivel != tabla_tcal[nprog*5 + p + 1000])
   {
      if (ftodo)
      {
         for (i = 0;i < nftodo;i++)
            free(ftodo[i]);
         free((char *)ftodo);
         ftodo = NULL;
      }
      nftodo = 0;
      ftodo_nivel = tabla_tcal[nprog*5 + p + 1000];
   }
   
   if (!nftodo)
   {
      if (!ftodo_nivel)
      {
         sprintf(tmp,"%scal/%s.cal",dirbase,quita_path(en_pr[programa_activo].prc));
         f = rw_open_(tmp,O_RDWR | O_BINARY);
         if (f < 0) {
            sprintf(tmp,"%sinc/%s.cal",dirbase,quita_path(en_pr[programa_activo].prc));
            f = rw_open_(tmp,O_RDWR | O_BINARY);
         }
         if (f < 0) {
            sprintf(tmp,__Texto[268],quita_path(en_pr[programa_activo].prc));
            tfichero[0] = '\0';
            tracer_int = 0;
            aviso(0,tmp);
            return;
         }
         if (strlen(quita_path(en_pr[programa_activo].prc)) < 10)
            strcpy(tfichero,quita_path(en_pr[programa_activo].prc));
      }
      else
      {
         sprintf(tmp,"%sinc/%s.cal",dirbase,tabla_tcal+nprog*5+(ftodo_nivel-1)*10);
         f = rw_open_(tmp,O_RDWR | O_BINARY);
         if (f < 0) 
         {
            sprintf(tmp,"%scal/%s.cal",dirbase,tabla_tcal+nprog*5+(ftodo_nivel-1)*10);
            f = rw_open_(tmp,O_RDWR | O_BINARY);
         }
         if (f < 0) 
         {
            sprintf(tmp,__Texto[269],tabla_tcal+nprog*5+(ftodo_nivel-1)*10);
            tfichero[0] = '\0';
            tracer_int = 0;
            aviso(0,tmp);
            return;
         }
         strcpy(tfichero,tabla_tcal+nprog*5+(ftodo_nivel-1)*10);
      }
      
      
      while (rw_read_(f,b_buf,512) == 512)
      {
         decripto(b_buf);
         kk = 0;
         while(kk < 510 && b_buf[kk] != 4)
         {
            strcpy(tmp0,b_buf+kk);
            kk += (strlen(tmp0) + 1);
            
            cambia_tabs(tmp0);
            /*tmp0[75] = '\0';*/
            i = strlen(tmp0)+10;
            
            if (!ftodo)
            {
               limi_ftodo = 1000;
               ftodo = (char **)malloc(sizeof(char *) * limi_ftodo);			  
            }
            else
               if ((limi_ftodo-10) < nftodo)
               {
                  limi_ftodo += 1000;
                  ftodo = (char **)realloc((char *)ftodo,sizeof(char *) * limi_ftodo);
               }
               
               ftodo[nftodo] = strmalloc(i);
			sprintf(ftodo[nftodo],"%5d %s",nftodo+1,tmp0);
            nftodo++;
         }
      }
      rw_close_(f);      
	  traza_w = 0; /* para que refresque el fichero!!*/
   }
   
   
   memcpy(tmp,tabla_tcal + (p * 5),5);
   tmp[5] = '\0';
   linea = atoi(tmp);
   
   if (linea < 1 || linea > nftodo)
   {
      char tmp_xx[256];
      sprintf(tmp_xx,__Texto[270],linea,nftodo);
      aviso(0,tmp_xx);
      return(0);
   }
   

   if (!traza_w) 
      {
      if (ftodo && DespachadorExterno)
		{
		char *pp;
		kk = 0;
		pp = malloc(20000);
		n = 0;
        for (i = 0;i < nftodo;i++)
		  {
		  if (n < 18500)
		     {
             strcpy(pp+n,ftodo[i]);
			 n += strlen(ftodo[i]);
			 pp[n] = 13;
			 n++;
			 pp[n] = 10;
			 n++;
			 pp[n] = 0;			 
			 }
		  else
		     {
			 (*DespachadorExterno)((int)DESPACHO_INICIO_TRAZA,(int)kk,pp);   
			 kk = 1;
			 n = 0;
			 i--;
			 }
		  }		
        pp[n++] = 1;
		strcpy(pp+n,tfichero);
        n += strlen(tfichero);
		pp[n++] = 13;
		pp[n++] = 10;
        pp[n] = 0;
        (*DespachadorExterno)((int)DESPACHO_INICIO_TRAZA,(int)kk,pp);
		free(pp);

	    pp = tmp;
        for (i = 0;i < nuser_breakpoints;i++)
            {
		    sprintf(pp,"%-10.10s%5d",user_breakpoints[i]._tfichero,user_breakpoints[i]._ntr_br);
		    pp += 15;
		    for (j = 0;j < user_breakpoints[i]._ntr_br;j++)
				{
                sprintf(pp,"%5d",user_breakpoints[i]._tracer_break[j]);
				pp += 5;
			    }				  
	        }
		*pp = 0;	
		(*DespachadorExterno)((int)DESPACHO_INICIO_TRAZA,(int)2,tmp);

        if (ftodo)
		   {
           for (i = 0;i < nftodo;i++)
             free(ftodo[i]);   
		   }
        free((char *)ftodo);
        ftodo = NULL;
		}
      traza_w = 1;
      }   

   if (DespachadorExterno)
	  {     
	   char tracepointrevisados[50];
	   int revision = 0;
	   memset(tracepointrevisados,0,50);
   	  tmp[1] = 0;
	  do {	  
	     tmp[0] = tracer_int+1;
	     /* bucle de acciones traza */
         (*DespachadorExterno)((int)DESPACHO_PUNTO_TRAZA,(int)linea,tmp);
         tracer_int = tmp[0] - 1;

		 if (revision && (tracer_int == 2 || tmp[1] != 1))
		 {
			 for (i = 0;i < 50;i++)
			 {
				 if (!tracepointrevisados[i])
				 {
					 trace_point[i][0] = 0;
				 }
			 }
		 }

		 if (tracer_int == 2)
		    {
			traza_w = 0;
			tracer_int = 0;
			fin_traza();
			return(0);
			}

		 if (!tmp[1]) break;

 		 if (tmp[1] == 3) 
		    {
			int kl;

			for (kl = 2;tmp[kl] && tmp[kl] != 1;kl++);
			if (tmp[kl] == 1)
			  {
              tmp[kl] = 0;
			  kl++;
			  }			
            td_campo(tmp+2);
	        if (ftd) 
		      {
			  int f,c,t;
			  M_VAL val;			  

	    	  f = dtf;
		      c = dtc;
			  no_rw_error = 1;
			  c_rw_error = 0;
		      t = busca_valor(&f,&c,&val);
			  no_rw_error = 0;
			  if (!c_rw_error)
			  {
				  if ( f < 0 ) 
				   {
				   if ( t == CHARTIPO || t == FECHATIPO ) 
					  {
					  if (no_memo_interna(c)) 
						  {
						  free(v_memo(c)->alfabetica);
						  f = strlen(tmp+kl)+1;
						  if (t == FECHATIPO && f < 11) f = 11;
						  v_memo(c)->alfabetica = strmalloc(f);
						  }
					   strcpy(v_memo(c)->alfabetica,tmp+kl);
					  }
				   else
			   		  v_memo(c)->numerica = rwatof(tmp+kl);
				   }
				  else 
				   {
				   st_campo(c,v_mpfi(f),tmp+kl);
				   }            
			  }
			  }
            tmp[1] = 0;
            continue;
			}

		 if (tmp[1] == 2) 
		    {
			char *pp;

			nuser_breakpoints = 0;
			ntr_br = 0;
	        pp = tmp + 2;
            while (*pp)
			   {
			   if (strlen(pp) < 15) break;

               memcpy(user_breakpoints[nuser_breakpoints]._tfichero,pp,9);
			   user_breakpoints[nuser_breakpoints]._tfichero[9] = 0;
			   quitab(user_breakpoints[nuser_breakpoints]._tfichero);
			   pp += 10;
			   user_breakpoints[nuser_breakpoints]._ntr_br = mid_atoi(pp,5);
			   pp += 5;
			   if (strlen(pp) < (user_breakpoints[nuser_breakpoints]._ntr_br*5)) break;

		       for (j = 0;j < user_breakpoints[nuser_breakpoints]._ntr_br;j++)
			  	  {
                  user_breakpoints[nuser_breakpoints]._tracer_break[j] = mid_atoi(pp,5);
				  pp += 5;
				  }

		       if (!strcmp(user_breakpoints[nuser_breakpoints]._tfichero,tfichero))
		          {
		          char *ppp;
				  int ll;

		          ntr_br = user_breakpoints[nuser_breakpoints]._ntr_br;
		          for (j = 0;j < ntr_br;j++)
			        {
			        ppp = tabla_tcal;
			        tracer_break[j] = nprog;					
			        for (n = 0;n < nprog;n++)
			          {
					  ll = mid_atoi(ppp,5);
			          if (ll == user_breakpoints[nuser_breakpoints]._tracer_break[j])
				        {
                        tracer_break[j] = n;
					    break;
					    }
					  else
					  if (ll > user_breakpoints[nuser_breakpoints]._tracer_break[j])
					    {
                        tracer_break[j] = n;
					    break;
						}
				      ppp += 5;
				      }
			        }
		          }           

			   nuser_breakpoints++;     
			   }
            tmp[1] = 0;
            break;
			}
					 
		 {
			 char *ptmp;
			 char *ptmp1;
			 int estracepoint = -1;
			 char tipo = 0;
			 
			 ptmp = tmp+2;
			 if (*ptmp == '-' && *(ptmp+1) == '>')
			 {
				 ptmp += 2;
				 ptmp1 = ptmp;
				 while(*ptmp1 && *ptmp1 != '=' && *ptmp1 != '!' && *ptmp1 != '<' && *ptmp1 != '>') ptmp1++;
				 if (*ptmp1)
				 {
					 switch(*ptmp1)
					 {
					 case '=':
						 tipo = 1;
						 break;
					 case '!':
						 tipo = 2;
						 break;
					 case '<':
						 tipo = 3;
						 break;
					 case '>':
						 tipo = 4;
						 break;
					 default:
						 tipo = 0;
					 }
					 *ptmp1 = 0;
					 ptmp1++;					 
				 }
				

				 for (i = 0;i < ntrace_point;i++)
				 {
					 if (!strcmp(ptmp,trace_point[i]))
					 {						 
						 trace_pointipo[i] = tipo;
						 if (tipo > 0)
						 {
							 strcpy(trace_pointfijo[i],ptmp1);
						 }
						 tracepointrevisados[i] = 1;
						 break;
					 }
				 }
				 if (i >= ntrace_point)
				 {
					 for (i = 0;i < ntrace_point;i++)
					 {
						 if (!trace_point[i][0])
						 {
							 break;
						 }
					 }
					 if (i < 50)
					 {
						 trace_pointipo[i] = tipo;
						 tracepointrevisados[i] = 1;
						 estracepoint = i;
						 strcpy(trace_point[i],ptmp);
						 if (tipo > 0)
						 {
							 strcpy(trace_pointfijo[i],ptmp1);
						 }
						 if (i >= ntrace_point)
							ntrace_point++;
					 }
				 }
			 }
			 revision = 1;
			 td_campo(ptmp);		 

			 if (ftd) 
			 {
				int f,c,t;
				M_VAL val;

	    		f = dtf;
				c = dtc;
			    no_rw_error = 1;
				c_rw_error = 0;
				t = busca_valor(&f,&c,&val);
				no_rw_error = 0;
				if (!c_rw_error)
				{				
					if ( t == CHARTIPO || t == FECHATIPO )
					   strcpy(tmp+2,val.valc);
					else 
					{
		   			   sprintf(tmp+2,"%lf",val.vald);
					   quitadec(tmp+2);
					}		 
					if (estracepoint > -1)
					{
						strcpy(trace_pointval[estracepoint],tmp+2);
					}
				}
				else
				{
					strcpy(tmp+2,"???????????????????????????????");
				}
			 }
			 else
			 {
				strcpy(tmp+2,"???????????????????????????????");
			 }
		 }
		 } while(-1);
	  }
   else
      tracer_int = 0;   

   return(0);
}

int ini_traza(p)
int p;
{
   int f,i,j,n;
   short s;

 
   sprintf(taplica,"%scal/%s.map",dirbase,quita_path(en_pr[p].prc));
   
   f = rw_open_(taplica,O_BINARY | O_RDONLY);
   if (f < 0) 
   {
      return(0);
   }
   tabla_tcal = texmalloc(nprog * 6 + 1001);
   if (!tabla_tcal)
   {
      return(0);
   }
   if (rw_read_(f,tabla_tcal,nprog * 5) != (nprog * 5)) 
   {
      free(tabla_tcal);
      tabla_tcal = NULL;
      rw_close_(f);
      return(0);
   }
   
   /*
   rw_read_(f,(char *)&s,sizeof(short)); 
   moto_short(&s);
   rw_lseek_(f,(long)(s*17),SEEK_CUR);
   rw_read_(f,(char *)&s,sizeof(short)); 
   moto_short(&s);
   rw_lseek_(f,(long)(s*21),SEEK_CUR);
   rw_read_(f,(char *)&s,sizeof(short)); 
   moto_short(&s);
   rw_lseek_(f,(long)(s*8),SEEK_CUR);
   */
   
   
   /* leer informacion simbolica del calculo */
   rw_lseek_(f,(long)(5 * nprog),SEEK_SET);
   
   rw_read_(f,(char *)&s,sizeof(short)); /* variables */
   moto_short(&s);
   if (s < 0 || s > 15000)
   {
      free(tabla_tcal);
      tabla_tcal = NULL;
      rw_close_(f);
      return(0);
   }
   tabla_tvar = strmalloc((int)(s * 17 + sizeof(short)) + 1);
#ifdef ESMOTOROLA
   memcpy(tabla_tvar,&s,2);
#else
   *((short *)tabla_tvar) = s;
#endif
   rw_read_(f,tabla_tvar+sizeof(short),s * 17);
   
   rw_read_(f,(char *)&s,sizeof(short)); /* campos (alias) */
   moto_short(&s);

   if (s < 0 || s > 1500)
   {
	  free(tabla_tvar);
      free(tabla_tcal);
      tabla_tcal = NULL;
      rw_close_(f);
      return(0);
   }
   tabla_tcam = strmalloc((int)(s * 21 + sizeof(short)) + 1);
#ifdef ESMOTOROLA
   memcpy(tabla_tcam,&s,2);
#else
   *((short *)tabla_tcam) = s;
#endif
   rw_read_(f,tabla_tcam+sizeof(short),s * 21);
   
   rw_read_(f,(char *)&s,sizeof(short)); /* ficheros (numero alias) */
   moto_short(&s);
   if (s < 0 || s > 1500)
   {
	  free(tabla_tcam);
	  free(tabla_tvar);
      free(tabla_tcal);
      tabla_tcal = NULL;
      rw_close_(f);
      return(0);
   }
   tabla_tfic = strmalloc((int)(s * 8 + sizeof(short)) + 1);
#ifdef ESMOTOROLA
   memcpy(tabla_tfic,&s,2);
#else
   *((short *)tabla_tfic) = s;
#endif
   rw_read_(f,tabla_tfic+sizeof(short),s * 8);    
   
   
   ftodo_nivel = 0;
   if (rw_read_(f,tabla_tcal+nprog * 5,nprog+1000) != (nprog+1000)) 
   {
      memset(tabla_tcal+nprog*5,0,nprog+1000);
   }
   
   rw_close_(f);
   hay_traza = 1;
   ftd = 0;


   /*return(0);*/

   ntr_br = 0;
   for (i = 0;i < nuser_breakpoints;i++)
      {
		 if (!strcmp(user_breakpoints[i]._tfichero,quita_path(en_pr[p].prc)))
		 {
		    char *ppp;
			int ll;
		    ntr_br = user_breakpoints[i]._ntr_br;
		    for (j = 0;j < ntr_br;j++)
			   {
			   ppp = tabla_tcal;
			   tracer_break[j] = nprog;
			   for (n = 0;n < nprog;n++)
			     {
				 ll = mid_atoi(ppp,5);
			     if (ll == user_breakpoints[i]._tracer_break[j])
				    {
                    tracer_break[j] = n;
					break;
				    }
			     else
				 if (ll > user_breakpoints[i]._tracer_break[j])
				    {
                    tracer_break[j] = n;
				    break;
					}

				 ppp += 5;
				 }
			   }			
			break;
		 }
	  }

   return(0);
}

int fin_traza()
{
   if (!hay_traza) return;
   
   ej_traza(-1);
   
   if (tabla_tcal)
      free(tabla_tcal);
   tabla_tcal = NULL;
   
   if (tabla_tvar)
      free(tabla_tvar);
   tabla_tvar = NULL;
   
   if (tabla_tcam)
      free(tabla_tcam);
   tabla_tcam = NULL;
   
   if (tabla_tfic)
      free(tabla_tfic);
   tabla_tfic = NULL;
   
   hay_traza = 0;
}



/*******************************************************************/


/* =========================================================================

*/

int inicio_dir()
{
   strcpy(dirbase,dirbass);
   strcpy(dirbin,dirbass);
   strcpy(dirdef,dirbass);
   strcpy(dirpan,dirbass);
   if (alt_dirfic)
			{
      strcpy(dirfic,alt_dirfic);
      strcpy(dirficb,alt_dirfic);
			}
   else
			{
      strcpy(dirfic,dirbass);
      strcpy(dirficb,dirbass);
			}
   strcpy(fich_apli,fichdir);
   strcpy(nomapli,"diagram9");
   lee_permisos(nomapli);
}


int entra_int(char *e,int pestanya)
{
   VENT *w;
   int i;
   int guarda_empre;
   char _fichemenu[128];
   char _menua[25][MAXMENUA+1];
   int  _punto;
   char   _nommenu[MAXNOMOP];   /* nombre del menu principal */
   T_MENU **_wmenu;    /* estructuras de menus */
   int    _nsmenu;              /* submenu actual 0,1..,MAXSUBMENU-1 */
   int    _wdef;
   int    _nmenus;
   VENT   *_mw[MAXSUBMENU];     /* ventanas del menu actual */
   int    _pintamenu;           /* si debe o no pintar el menu */
   int    _pintahmenu;          /* si debe o no pintar el menu hijo */
   char *_dir_smenu;
   char *_dir_sgmenu;
   FI_DEF *_empresa;
   RELAS  *_mpfiemp;
   char _nomapli[25];
   int _campo_color = campo_color,_que_color = que_color;
   int _file_color = file_color;
   char *_color_factor_inicio = color_factor_inicio;
   int _que_color_inicio   = que_color_inicio;
   int _cambio_color_fijo = cambio_color_fijo;
   int  _atributo_1 = atributo_1;
   int  _atributo_2 = atributo_2;
   char *_color_factor = color_factor;
   int  _len_color = len_color;
   int _p_color = p_color;
   unsigned char _l_separador = l_separador;
   int _t_ini = t_ini;
   int _t_fin = t_fin;
#ifdef FGVWIN
   char **_tabla_icon = tabla_icon;
   short *_punto_tabla = punto_tabla;
   int _ntabla_icon = ntabla_icon;
   char ***_wwmenus = wwmenus; /* opciones */
   int *_nwwmenus = nwwmenus;  /* numero de opciones para liberacion */
   int _en_menu = en_menu;
   int _n_wmenus = n_wmenus;
   HMENU _principal_menu = principal_menu;
   HMENU tmp_menu;
   
   
   en_menu = 0;

   if (DespachadorExterno)
   {
      (*DespachadorExterno)((int)DESPACHO_EMENU,(int)en_menu,(void *)NULL);
   }

   /*
   n_wmenus = 0;
   wwmenus = NULL;
   nwwmenus = NULL;
   */
   
   duplica_wwmenus();
   
   
   
   tabla_icon = NULL;
   punto_tabla = NULL;
   ntabla_icon = 0;
   
   /*
   if (principal_menu)
   {
   principal_menu = NULL;
   tmp_menu = LoadMenu(hInst, "AGIXMENU");
   SetMenu(principal_w, tmp_menu);		
   RedrawWindow(principal_w,NULL,NULL,RDW_FRAME | RDW_INVALIDATE);
   }
   */
#endif
   
   if (*e >= '0' && *e <= '9')
			{
      _empresa = mpfi[48];
      _mpfiemp = mpfirel[48];
      guarda_empre = 1;
      mpfi[48]    = NULL;
      mpfirel[48] = NULL;
      strcpy(_nomapli,nomapli);  
			}
   else
      guarda_empre = 0;
   
   
   memcpy(_fichemenu,fichemenu,128);
   memcpy(_nommenu,nommenu,MAXNOMOP);   /* nombre del menu principal */
   _wmenu = wmenu;    /* estructuras de menus */
   _nsmenu = nsmenu;              /* submenu actual 0,1..,MAXSUBMENU-1 */
   _wdef = wdef;
   _nmenus = nmenus;
   for (i = 0;i < MAXSUBMENU;i++)
			{
      _mw[i] = mw[i];
      mw[i] = NULL;
			}
   _pintamenu = pintamenu;           /* si debe o no pintar el menu */
   _pintahmenu = pintahmenu;          /* si debe o no pintar el menu hijo */
   _dir_smenu = dir_smenu;
   _dir_sgmenu = dir_sgmenu;
   
   wmenu = NULL;
   nsmenu = 0;
   wdef = 0;
   
   file_color = -1;
   campo_color = 0;
   que_color = 0;
   atributo_1 = 0;
   atributo_2 = 0;
   cambio_color_fijo = 0;
   color_factor = NULL;
   color_factor_inicio = NULL;
   que_color_inicio   = 0;
   len_color = 0;
   p_color = 0;
   l_separador = 0;
   t_ini = -1;
   t_fin = -1;
   
   
   _punto = punto;
   for (i=0;i<25;i++)
			{
      memcpy(_menua[i],menua[i],MAXMENUA+1);
      menua[i][0]=0;
			}
   punto = 0;
   
   w = w_ini(101,2580);
   w_salva(w);
   
   if (pestanya && DespachadorExterno)
			{
      (*DespachadorExterno)((int)DESPACHO_SOBRE_PANTALLA,(int)0,NULL);   
			}                 
   
   guarda_externas(1); /* enlaza externas!!! */
   
   
   control_exec++;
   opcion_ej(e,"");
   control_exec--;
   
   
   repon_externas(1);  /* y las repone !!! */
   
   if (pestanya && DespachadorExterno)
			{
      (*DespachadorExterno)((int)DESPACHO_REPON_PANTALLA,(int)0,NULL);   
			}                 
   
   w_pinta(w);
   w_fin(w);
   
   for (i=0;i<25;i++)
			{
      memcpy(menua[i],_menua[i],MAXMENUA+1);  
			}
   punto = _punto;
   memcpy(fichemenu,_fichemenu,128);
   memcpy(nommenu,_nommenu,MAXNOMOP);   /* nombre del menu principal */
   wmenu = _wmenu;    /* estructuras de menus */
   nsmenu = _nsmenu;              /* submenu actual 0,1..,MAXSUBMENU-1 */
   wdef = _wdef;
   nmenus = _nmenus;
   for (i = 0;i < MAXSUBMENU;i++)
			{
      mw[i] = _mw[i];
			}
   pintamenu = _pintamenu;           /* si debe o no pintar el menu */
   pintahmenu = _pintahmenu;          /* si debe o no pintar el menu hijo */
   dir_smenu = _dir_smenu;
   dir_sgmenu = _dir_sgmenu;
   
   
   if (guarda_empre)
			{  
      mpfi[48]    = _empresa;
      mpfirel[48] = _mpfiemp;
      libera_permisos();
      lee_permisos(_nomapli);
			}
   
   
   if (color_factor)
      free(color_factor);
   if (color_factor_inicio)
      free(color_factor_inicio);
   color_factor_inicio = _color_factor_inicio;
   que_color_inicio   = _que_color_inicio;
   campo_color = _campo_color;
   que_color   = _que_color;
   cambio_color_fijo = _cambio_color_fijo;
   atributo_1 = _atributo_1;
   atributo_2 = _atributo_2;
   color_factor = _color_factor;
   len_color = _len_color;
   p_color = _p_color;
   l_separador = _l_separador;
   t_ini = _t_ini;
   t_fin = _t_fin;
   file_color = _file_color;
   
   
#ifdef FGVWIN
   tabla_icon = _tabla_icon;
   punto_tabla = _punto_tabla;
   ntabla_icon = _ntabla_icon;
   
   wwlibera();/* ya chequea si hay menu antes de liberar */
   wwmenus = _wwmenus; /* opciones */
   nwwmenus = _nwwmenus;  /* numero de opciones para liberacion */
   n_wmenus = _n_wmenus;
   
   en_menu = _en_menu;
   if (DespachadorExterno)
   {
      (*DespachadorExterno)((int)DESPACHO_EMENU,(int)en_menu,(void *)NULL);
   }

   /*
   principal_menu = _principal_menu;
   if (principal_menu)
   {
   SetMenu(principal_w, principal_menu);		
   RedrawWindow(principal_w,NULL,NULL,RDW_FRAME | RDW_INVALIDATE);
   }
   */
#endif
 
   if (!punto)
   {
		cabecera_v(rw_version,proceso,titulo,empresa);
   }
   else
   {
		cabecera(proceso,titulo,empresa);
   }
   
   return(0);
}

static int nivel = 0;

static void strcpy_4(char *d,char *o)
{
	while(*o && *o != 4)	
		*d++ = *o++;	
	*d = 0;
}

int entra_util(op)
char *op;
{
   char tmp[250];
   char *v2,*v3;
   double campo,vcf;
   int r = 0,i,j;
   int o_modo_rel = modo_de_relacion;
   char _proceso[81];   
   int entraempresa;
   entraempresa = 0;
   strcpy(_proceso,proceso);
   proceso[0] = 0;
   
   modo_de_relacion = 0;
   
   switch (*op) {
   case '&': /* calculo declarado externo */
      for (i = 0;i < np_ex;i++)
         if (!strcmp(op+1,p_ex[i].nombre)) {
            v2 = malloc(250);
            v3 = malloc(250);
            campo = v_memo(5000)->numerica;
            vcf = v_memo(5011)->numerica;
            memcpy(tmp,v_memo(5002)->alfabetica,250);
            memcpy(v2,v_memo(5003)->alfabetica,250);
            memcpy(v3,v_memo(5004)->alfabetica,250);
            p_ejecuta(p_ex[i].indice,O_NOR,0,8);
            memcpy(v_memo(5002)->alfabetica,tmp,250);
            memcpy(v_memo(5003)->alfabetica,v2,250);
            memcpy(v_memo(5004)->alfabetica,v3,250);
            v_memo(5000)->numerica = campo;
            v_memo(5011)->numerica = vcf;
            free(v2);
            free(v3);
            break;
         }
         if (i == np_ex)
            pito(118);
         break;
   case '*':
   case '|': /* ejecuta un programa */	  
	   {

		   char *_nomapli,*_dirfic,*_fich_apli;
		   char *_agi_empresa = agi_empresa;
		   char *_agi_codi_empre = agi_codi_empre;
		   char *_agi_aplicacion = agi_aplicacion;
		   FI_DEF *defempre = NULL;
		   char _empresa[MAXDEFECTO+10];
		   char _proceso[260];
		   char _titulo[260];
		   int salva_entorno = 0;
		   RELAS  *defemprerel = NULL;

	  
		  if (nivel >= (MAXRECUR-5)) 
		  { /* no se permiten mas por razones de pila */
			 aviso(0,__Texto[32]);       
			 break;
		  }

			  salva_entorno = 1;
			  strcpy(_empresa,empresa);
			  strcpy(_proceso,proceso);
			  strcpy(_titulo,titulo);  

			  _nomapli = malloc(25);
			  _dirfic = malloc(51);
			  _fich_apli = malloc(15);
			  memcpy(_nomapli,nomapli,25);
			  memcpy(_dirfic,dirfic,51);
			  memcpy(_fich_apli,fich_apli,15);
			  defempre = mpfi[48];
			  defemprerel = mpfirel[48];


		  strcpy(tmp,op+1);
		  for (i = 0;tmp[i];i++)
		  {
			  if (tmp[i] == 5)
			  {
				  fuerza_empresa = 1;
				  tmp[i] = 0;
				  continue;
			  }
			  if (tmp[i] == 4)
			  {
				  tmp[i] = 0;
				  for (j = i+1;tmp[j];j++)
				  {
					  if (tmp[j] == 4)
					  {
						  tmp[j] = 0;
						  strcpy_4(proceso,tmp+j+1);
						  j++;
						  break;
					  }
				  }              
				  for (;tmp[j];j++)
				  {
					  if (tmp[j] == 4)
					  {
						  tmp[j] = 0;
						  strcpy_4(dirbass,tmp+j+1);
						  j++;
						  break;
					  }
				  }
				  for (;tmp[j];j++)
				  {
					  if (tmp[j] == 4)
					  {
						  tmp[j] = 0;
						  strcpy_4(nomapli,tmp+j+1);
						  j++;
						  break;
					  }
				  }              
				  for (;tmp[j];j++)
				  {
					  if (tmp[j] == 4)
					  {
						  tmp[j] = 0;
						  strcpy_4(fich_apli,tmp+j+1);
						  j++;
						  break;
					  }
				  }
				  if (strcmp(nomapli,"diagram9") && strcmp(nomapli,"diagramx"))
				  {
					  compon_dirbase(dirbase,dirbass,nomapli);
					  cambia_paths();
					  strcpy(dirfic,dirficb);
					  libera_permisos();
					  lee_permisos(nomapli);
				  } 
				  for (;tmp[j];j++)
				  {
					  if (tmp[j] == 4)
					  {
						  tmp[j] = 0;
						  strcpy_4(dirficb,tmp+j+1);
						  j++;
						  break;
					  }
				  }
				  for (;tmp[j];j++)
				  {
					  if (tmp[j] == 4)
					  {
						  tmp[j] = 0;
						  strcpy_4(dirfic,tmp+j+1);
						  j++;
						  break;
					  }
				  }
				  /*****/
				  {
					  for (;tmp[j];j++)
					  {
						  if (tmp[j] == 4)
						  {
							  tmp[j] = 0;
							  if (tmp[j+1] && agi_empresa) {
									strcpy_4(agi_empresa,tmp+j+1);
									entraempresa++;
							  }
							  j++;
							  break;
						  }
					  }
					  for (;tmp[j];j++)
					  {
						  if (tmp[j] == 4)
						  {
							  tmp[j] = 0;
							  if (tmp[j+1] && agi_codi_empre) {
									strcpy_4(agi_codi_empre,tmp+j+1);
									entraempresa++;
							  }
							  j++;
							  break;
						  }
					  }
				  }
				  strcpy_4(nommenu,tmp+i+1);
				  strcpy_4(titulo,tmp+i+1);
				  
				  break;
			  }
		  }
		  if (entraempresa > 1) {
			  entra_empresa(agi_empresa,agi_codi_empre);
		  }

		  /*
		  strcpy(tmp,op+1);
		  for (i = 0;tmp[i];i++)
		  {
			  if (tmp[i] == 5)
			  {
				  fuerza_empresa = 1;
				  tmp[i] = 0;
				  continue;
			  }
			  if (tmp[i] == 4)
			  {
				  tmp[i] = 0;
				  for (j = i+1;tmp[j];j++)
				  {
					  if (tmp[j] == 4)
					  {
						  tmp[j] = 0;
						  strcpy(proceso,tmp+j+1);
					  }
				  }
				  strcpy(nommenu,tmp+i+1);
				  strcpy(titulo,tmp+i+1);
				  break;
			  }
		  }
		  */
		  r = entra_int(tmp,1);
		     agi_empresa = _agi_empresa;
		     agi_codi_empre = _agi_codi_empre;
		     agi_aplicacion = _agi_aplicacion;

			 if (salva_entorno)
			 {
				   memcpy(nomapli,_nomapli,25);
				   compon_dirbase(dirbase,dirbass,nomapli);
				   /*
				   strcpy(dirbase,dirbass);
				   strcat(dirbase,nomapli);
				   strcat(dirbase,"/");
				   */

				   memcpy(fich_apli,_fich_apli,15);
				   mpfi[48] = defempre;
				   mpfirel[48] = defemprerel;
				   cambia_paths();
				   memcpy(dirfic,_dirfic,51);
				   free(_nomapli);
				   free(_dirfic);
				   free(_fich_apli);
				   libera_permisos();
				   lee_permisos(nomapli);
			 }
     
			 strcpy(empresa,_empresa);
			 strcpy(proceso,_proceso);
			 strcpy(titulo,_titulo);
			 if (va_con_doble_)   
			 {
				if (es_la_doble_ && empresa[0] == '*')
				   empresa[0] = '/';
				else
				   if (!es_la_doble_ && empresa[0] == '/')
					  empresa[0] = '*';
			 }
	   }

      break;
   default:
      pito(119);
   }
   modo_de_relacion = o_modo_rel;
   strcpy(proceso,_proceso);
   return(r);
}


int _ejecutas_(mandato_,modo)
char *mandato_;
int modo;
{
   char **funs;
   char mandato[512];
   int i,j = strlen(mandato_),n = 0;
   VENT *w = NULL;
   
   mandato[0] = 0;
   for (i = 0;i < j;i++)
			{
      if ((!i || mandato_[i-1] == ' ') && mandato_[i] == ':' && mandato_[i+1] == '/')
      {
         strcpy(mandato+n,rw_directorio(""));
         i += 2;
         n = strlen(mandato);
      }
      mandato[n] = mandato_[i];
      n++;
			}
   mandato[n] = 0;
   n = 0;
   j=strlen(mandato);
   
#ifndef FGVWIN
#ifdef RWSERVER
   if (!en_conexion_externa)
			{
#endif
      w = w_ini(101,2580);
      w_salva(w);
      cls();
      fflush(stdout);
      fin_sig();
      funs = salva_funciones(0,-1);
      Terfinal();
#ifdef RWSERVER
			}
#endif
#endif
   if (modo)
      filtropar(mandato);
   else
      filtroma(mandato);
   
   
#ifdef FGV32
   if (fIsTNT)
			{
      FreeConsole();
      AllocConsole();
			}
#endif
   
   agi_system(mandato);
   
#ifdef FGV32
   if (fIsTNT)
			{
      FreeConsole();
      AllocConsole();
			}		
#endif
   
   
#ifndef FGVWIN
#ifdef RWSERVER
   if (!en_conexion_externa)
			{
#endif
      Terinici();
      repon_funciones(funs,0,-1);
      ini_sig();
      if (w)
      {
         w_pinta(w);
         w_fin(w);
         w = NULL;
      }
#ifdef RWSERVER
			}
#endif
#endif
   return(0);
}


int ejecutas(mandato_)
char *mandato_;
{
			return(_ejecutas_(mandato_,0));
}


static void pon_ndeci(M_EX *m)
{
	int i;
	for (i = 0;i < enlaza_n_ndeci;i++)
	{
		if (!strcmp(enlaza_ndeci[i].nombre,m->nombre))
		{
			if (m->m_tipo == enlaza_ndeci[i].m_tipo)
			{				
				if (m->m_tipo != DOUBLETIPO)
				{
				   if (enlaza_ndeci[i].mv.valc)
				  	  free(enlaza_ndeci[i].mv.valc);
				   enlaza_ndeci[i].mv.valc = strmalloc(strlen(m->mv.valc)+1);
				   strcpy(enlaza_ndeci[i].mv.valc,m->mv.valc);
				}
				else
				{
					memcpy(&(enlaza_ndeci[i].mv.vald),&(m->mv.vald),sizeof(double));
				}
			}			
			return;
		}
	}
	if (!enlaza_n_ndeci)
	{
		enlaza_ndeci = (M_EX *)malloc(sizeof(M_EX) * 100);
	}
	else
	{
		if (!(enlaza_n_ndeci%100))
		{
			enlaza_ndeci = (M_EX *)realloc(enlaza_ndeci,sizeof(M_EX) * (enlaza_n_ndeci+100));
		}
	}
	enlaza_ndeci[enlaza_n_ndeci] = *m;
    enlaza_ndeci[enlaza_n_ndeci].nombre = strmalloc(strlen(m->nombre)+1);
    strcpy(enlaza_ndeci[enlaza_n_ndeci].nombre,m->nombre);
	if (m->m_tipo != DOUBLETIPO)
	{
	    enlaza_ndeci[enlaza_n_ndeci].mv.valc = strmalloc(strlen(m->mv.valc)+1);
	    strcpy(enlaza_ndeci[enlaza_n_ndeci].mv.valc,m->mv.valc);
	}
	enlaza_n_ndeci++;
}

static int saca_ndeci(M_EX *m)
{
	int i;
	for (i = 0;i < enlaza_n_ndeci;i++)
	{
		if (!strcmp(enlaza_ndeci[i].nombre,m->nombre))
		{
			if (m->m_tipo == enlaza_ndeci[i].m_tipo)
			{				
				if (m->m_tipo != DOUBLETIPO)
				{
					if (m->mv.valc)
						free(m->mv.valc);
					m->mv.valc = strmalloc(strlen(enlaza_ndeci[i].mv.valc)+1);
					strcpy(m->mv.valc,enlaza_ndeci[i].mv.valc);
				}
				else
				{
					memcpy(&(m->mv.vald),&(enlaza_ndeci[i].mv.vald),sizeof(double));
				}
				return 1;
			}
		}
	}
	return 0;
}

int guarda_externas(int flag)
{
   int i,j;
   int k = memoplin + maxpila * 2;   
   if (flag)
   {
	   borra_externas();
	   enlaza_nm_ex = nm_ex - k;
	   if (enlaza_nm_ex > 0)
	   {
		  enlaza_m_ex = (M_EX *)malloc(sizeof(M_EX) * enlaza_nm_ex);
		  memcpy(enlaza_m_ex,m_ex+k,sizeof(M_EX) * enlaza_nm_ex);
	   }
	   for (i = 0;i < enlaza_nm_ex;i++) 
	   {
		  j = strlen(m_ex[i+k].nombre);
		  if (j > 6 && (!memcmp(m_ex[i+k].nombre,"nDeci_",6) || !memcmp(m_ex[i+k].nombre,"PRMNT_",6)))
		  {
			  pon_ndeci(&(m_ex[i+k]));
		  }		  
		  enlaza_m_ex[i].nombre = strmalloc(strlen(m_ex[i+k].nombre)+1);
		  strcpy(enlaza_m_ex[i].nombre,m_ex[i+k].nombre);
		  if (enlaza_m_ex[i].m_tipo != DOUBLETIPO) 
		  {
			  if (enlaza_m_ex[i].m_tipo != CHARTIPO && enlaza_m_ex[i].m_tipo != FECHATIPO)
			  {
				  rw_error(999);
			  }
			  enlaza_m_ex[i].mv.valc = strmalloc(strlen(m_ex[i+k].mv.valc)+1);
			  strcpy(enlaza_m_ex[i].mv.valc,m_ex[i+k].mv.valc);
		  }
	   }
	   if (enlaza_nm_ex < 0)
		  enlaza_nm_ex = 0;
   }
   else
   {
	   for (i = k;i < nm_ex;i++) 
	   {
		  j = strlen(m_ex[i].nombre);
		  if (j > 6 && (!memcmp(m_ex[i].nombre,"nDeci_",6) || !memcmp(m_ex[i].nombre,"PRMNT_",6)))
		  {
			  pon_ndeci(&(m_ex[i]));
		  }
		  for (j = 0;j < enlaza_nm_ex;j++) 
		  {
			 if (!strcmp(enlaza_m_ex[j].nombre,m_ex[i].nombre))
			 {
				  if (enlaza_m_ex[j].m_tipo != DOUBLETIPO) 
				  {
					  if (enlaza_m_ex[j].m_tipo != CHARTIPO && enlaza_m_ex[j].m_tipo != FECHATIPO)
					  {
						  rw_error(999);
					  }
					  free(enlaza_m_ex[j].mv.valc);
					  enlaza_m_ex[j].mv.valc = strmalloc(strlen(m_ex[i].mv.valc)+1);
					  strcpy(enlaza_m_ex[j].mv.valc,m_ex[i].mv.valc);
				  }
				  else
				  {
					  memcpy(&(enlaza_m_ex[j].mv.vald),&(m_ex[i].mv.vald),sizeof(double));
				  }
			 }
		  }
	   }
   }
}

int repon_externas(int flag)
{
   int i,j,ti,tt;
   int k = memoplin + maxpila * 2;
   
   for (i = k;i < nm_ex;i++) 
   {
	  j = strlen(m_ex[i].nombre);
	  if (j > 6 && (!memcmp(m_ex[i].nombre,"nDeci_",6) || !memcmp(m_ex[i].nombre,"PRMNT_",6)))
	  {
		  if (saca_ndeci(&(m_ex[i])))
			  continue;
	  }
      for (j = 0;j < enlaza_nm_ex;j++) 
	  {
         if (!strcmp(enlaza_m_ex[j].nombre,m_ex[i].nombre))
         {
            tt = m_ex[i].m_tipo;
            ti = enlaza_m_ex[j].m_tipo;
            if (ti == tt) 
			{
               if (ti != DOUBLETIPO)
			   {
				  if (tt != CHARTIPO && tt != FECHATIPO)
				  {
					  rw_error(998);
				  }
                  free(m_ex[i].mv.valc);
			   }
               
		   	   if (!flag)
			   {
				   if (ti == DOUBLETIPO)
				   {
				 	   memcpy(&(m_ex[i].mv.vald),&(enlaza_m_ex[j].mv.vald),sizeof(double));
				   }
				   else
				   {
					   m_ex[i].mv.valc = malloc(strlen(enlaza_m_ex[j].mv.valc)+1);
					   strcpy(m_ex[i].mv.valc,enlaza_m_ex[j].mv.valc);
				   }
			   }
			   else
			   {
				   memcpy(&(m_ex[i].mv.vald),&(enlaza_m_ex[j].mv.vald),sizeof(double));
				   if (ti != DOUBLETIPO)
					  enlaza_m_ex[j].m_tipo = DOUBLETIPO;
				   enlaza_m_ex[j].nombre[0] = 0;
			   }
               break;
            }
         }
		}
   }   
   if (flag)
   {
	   borra_externas();
   }
}

int borra_externas()
{
   int j;
   
   if (enlaza_m_ex > 0) 
   {
      for (j = 0;j < enlaza_nm_ex;j++) 
	  {
         if (enlaza_m_ex[j].m_tipo != DOUBLETIPO)
            free(enlaza_m_ex[j].mv.valc);
         free(enlaza_m_ex[j].nombre);
	  }
      free((char *)enlaza_m_ex);
   }
   enlaza_m_ex = NULL;
   enlaza_nm_ex = 0;
}

int cambia_paths()
{
   strcpy(dirbin,dirbase);
   strcat(dirbin,"bin/");
   strcpy(dirdef,dirbase);
   strcat(dirdef,"def/");
   strcpy(dirpan,dirbase);
   strcat(dirpan,"pan/");
   if (alt_dirfic)
   {
      strcpy(dirficb,alt_dirfic);
      strcat(dirficb,nomapli);
      strcat(dirficb,"/");
   }
   else
   {
      strcpy(dirficb,dirbase);
   }
   strcat(dirficb,fich_apli);
   strcat(dirficb,"/");
   strcpy(nomfdir,dirficb);
   strcat(nomfdir,nomapli);
}

/*  modif_def
Modifica los paths del fichero y de las pantallas
*/

static modif_defs()
{
   int i;
   /*if (nfi == 49) i = 48; else i = 0;*/
   i = 0;
   for (;i<nfi;i++) {
      f_n(i,RENBUF);
   }
}

/* esto es para el def de enlace con programa externo */
static int mpfi_enlazado = -1;
static char *mpfi_buf_enlazado = NULL;

#ifdef ESDOS
#ifndef MK_FP
#define MK_FP( seg,ofs )( (void _seg * )( seg ) +( void near * )( ofs ))
#endif
#endif

extern void set_entra_timer();

int rw_ex_enlaza_mpfi(p,tip,faclin)
int p;
int tip;
int faclin;
{
   unsigned int segmento,despla;
   unsigned int *maximo;
   unsigned char *tmp;
   unsigned long **tmp1;
   int ipfi = v_prog(prog[p].indice);
   if (mpfi_enlazado != -1) {
      strcpy(v_memo(5004)->alfabetica,"-1");
      return(0);
   }
   mpfi_buf_enlazado = v_mpfi(ipfi)->f_buf;
   mpfi_enlazado = ipfi;
#ifdef ESDOS
#ifndef FGVWIN
#ifndef FGV32
   asm {
      push ax
         push es
         push bx
         mov ax,35E2h
         int 21h
         mov segmento,es
         mov despla,bx
         pop bx
         pop es
         pop ax
   }
   tmp = (unsigned char *)MK_FP(segmento,despla);
   if (tmp[9] != 'A' || tmp[10] != 'U'  || tmp[11] != 'T' || tmp[12] != 'O' ||
      tmp[13] != 'R' || tmp[14] != ':' || tmp[15] != 'F' || tmp[16] != 'r' ||
      tmp[17] != 'a' || tmp[18] != 'n' || tmp[19] != 'c' || tmp[20] != 'i' ||
      tmp[21] != 's' || tmp[22] != 'c' || tmp[23] != 'o' || tmp[24] != ' ' ||
      tmp[33] != 'G' || tmp[34] != 'o' || tmp[35] != 'n' || tmp[36] != 'z' ||
      tmp[37] != 'a' || tmp[38] != 'l' || tmp[39] != 'e' || tmp[40] != 'z' ||
      tmp[41] != ' ' || tmp[42] != 'V' || tmp[43] != 'a' || tmp[44] != 'l' ||
      tmp[45] != 'i' || tmp[46] != 'e' || tmp[47] != 'n' || tmp[48] != 't' ||
      tmp[49] != 'e')
   {
      /* programa residente no esta */
      mpfi_enlazado = -1;
      strcpy(v_memo(5004)->alfabetica,"-1");
      return(0);
   }
   tmp += 55;
   /* recoger el puntero */
   /* realmente solo el offset nos es necesario */
   despla = *((unsigned int *)tmp);
   tmp = (unsigned char *)MK_FP(segmento,despla);
   tmp1 = (unsigned long **)tmp;
   set_entra_timer(tmp1);
   timer_especial_int = *(tmp1+3);
   tmp += 16;
   maximo = (unsigned int *)tmp;
   if ( ((*maximo)-despla-2) < v_mpfi(ipfi)->f_lfich )
   {
      mpfi_enlazado = -1;
      strcpy(v_memo(5004)->alfabetica,"-1");
      return(0);
   }
   v_mpfi(ipfi)->f_buf = tmp + 2;
#endif
#endif
#endif
   strcpy(v_memo(5004)->alfabetica,"0");
   return(0);
}

int rw_ex_desenlaza_mpfi(p,tip,faclin)
int p;
int tip;
int faclin;
{
   if (mpfi_enlazado == -1) 
   {
   /*
   if (memoplin > 3)
   strcpy(v_memo(5004)->alfabetica,"-1");
						*/
	  return(0);
   }
   v_mpfi(mpfi_enlazado)->f_buf = mpfi_buf_enlazado;
   mpfi_enlazado = -1;
#ifdef ESDOS
#ifndef FGVWIN
   timer_especial_int = NULL;
#endif
#endif
   /*
   if (memoplin > 3)
   strcpy(v_memo(5004)->alfabetica,"0");
   */
   return(0);
}


int dame_campo_empresa(FI_DEF *pfi)
{
int i;

for (i = 0;i < pfi->f_ncampos;i++)
{
	if (strstr(pfi->f_campos[i].c_maximo,"^"))
	{
		return i;
	}
}
return 1;
}

extern int win_fondo_neutro;
extern int _especial_control_;

extern int sin_mensaje_error_a;

static int con_euro = 1;
extern int fgv_ver_euro;

static struct _ds_entrada_automatica_
{
	int ipfi;
	int campo;
	char *valor;
}   **ds_entrada_automatica = NULL;
int nds_entrada_automatica = 0;

char *ds_absoluta_impresora = NULL;

extern int modo_desatendido;

int rellena_entrada_automatica(char *nomfich)
{
	FILE *f;
	char tmp_nom[514];

	if (ds_entrada_automatica)
	{
		while(nds_entrada_automatica > 0)
		{
			nds_entrada_automatica--;
			if (ds_entrada_automatica[nds_entrada_automatica])
			{
				if (ds_entrada_automatica[nds_entrada_automatica]->valor)
					free(ds_entrada_automatica[nds_entrada_automatica]->valor);
				free((char *)ds_entrada_automatica[nds_entrada_automatica]);			
			}
		}
		free((char *)ds_entrada_automatica);
	}
	ds_entrada_automatica = NULL;
	nds_entrada_automatica = 0;
	if (ds_absoluta_impresora)
	{
		free(ds_absoluta_impresora);
	}
	ds_absoluta_impresora = NULL;

	if (!nomfich)
		return 0;

	f = fopen(nomfich,"r");
	if (f)
	{	
		int i,j,n;
		char *p;
		char fichero[512];
		char campo[512];
		char valor[512];
		while(fgets(tmp_nom,512,f))
		{
			nds_entrada_automatica++;
		}
		rewind(f);	
		ds_entrada_automatica = (struct _ds_entrada_automatica_ **)malloc(sizeof(void *) * (nds_entrada_automatica+1));
		for (i = 0;i < nds_entrada_automatica;i++)
		{
			if (!fgets(tmp_nom,512,f)) break;
			ds_entrada_automatica[i] = malloc(sizeof(struct _ds_entrada_automatica_));
			memset((char *)ds_entrada_automatica[i],0,sizeof(struct _ds_entrada_automatica_));
			ds_entrada_automatica[i]->campo = -1;
			ds_entrada_automatica[i]->ipfi = -1;
			ds_entrada_automatica[i]->valor = NULL;
			quitab(tmp_nom);
			quitan(tmp_nom);
			quitab(tmp_nom);
			quitabp(tmp_nom);
			if (!memcmp(tmp_nom,"IMPRE=",6))
			{
				quitabp(tmp_nom+6);
				if (!ds_absoluta_impresora)
				{
					ds_absoluta_impresora = malloc(strlen(tmp_nom+6)+1);
					strcpy(ds_absoluta_impresora,tmp_nom+6);
				}
			}
			else if (tmp_nom[0] == '#')
			{
				j = 1;
				p = tmp_nom+j;
				while(tmp_nom[j] && tmp_nom[j] != '#') j++;
				if (tmp_nom[j] == '#')				
				{
					n = (int)((tmp_nom+j) - p);
					if (n > 0)
					{
						memcpy(fichero,p,n);
						fichero[n] = 0;
						quitab(fichero);
						quitabp(fichero);
						j++;
						p = tmp_nom + j;
						while(tmp_nom[j] && tmp_nom[j] != '=') j++;
						if (tmp_nom[j] == '=')
						{
							n = (int)((tmp_nom+j) - p);
							if (n > 0)
							{
								memcpy(campo,p,n);
								campo[n] = 0;
								quitab(campo);
								quitabp(campo);
								j++;
								if (strlen(tmp_nom+j) > 0)
								{
									strcpy(valor,tmp_nom+j);
									quitab(valor);
									quitabp(valor);

									ds_entrada_automatica[i]->valor = malloc(strlen(valor)+1);
									strcpy(ds_entrada_automatica[i]->valor,valor);

									ds_entrada_automatica[i]->ipfi = ind_rel(fichero);
									ds_entrada_automatica[i]->campo = atoi(campo);

								}
							}
						}
					}					
				}
			}
		}
		for(;i <= nds_entrada_automatica;i++)
		{
			ds_entrada_automatica[i] = NULL;
		}
		return 1;
	}
	return 0;
}

int ds_recoge_entrada_automatica(int ipfi,int campo,char *qd,char *qo,int *qj,int antqj)
{
	int i;

	if (ds_entrada_automatica)
	{
		for (i = 0;i < nds_entrada_automatica;i++)
		{
			if (ds_entrada_automatica[i])
			{
				if (ds_entrada_automatica[i]->ipfi == ipfi &&
					ds_entrada_automatica[i]->campo == campo)
				{
					if (ds_entrada_automatica[i]->valor)
					{
						strcpy(qo,ds_entrada_automatica[i]->valor);
					}
					else
					{						
						strcpy(qo,qd);
					}
					*qj = antqj;
					return 1;
				}
			}
		}
	}

	if (modo_desatendido > 0)
	{
		*qj = antqj;
		strcpy(qo,qd);
		return 1;
	}
	return 0;
}

int ejecuta_ap(_param,prime,_par)
char *_param;          /* nombre del fichero de parametros e instrucciones */
/*  de mantenimiento                                */
int prime;        /* si no es cero se trata del n de mpfi del de empresas */
char *_par;
{
   char *param = _param;   
   int ipfi,i,j,rei;
   int ficha = 0;
   char tmpj[MAXMENUA+1];
   char parametros[81];
   char qo[81];
   char _nomapli[25];
   char _dirfic[51];
   char _dirbase[51];
   char _modulo[9];
   char _fich_apli[15];
   /* nuevo */
   struct enlaza_prc *_en_pr = en_pr;
   short _nen_pr = nen_pr;
   short *_p_tabla = p_tabla;
   char *_proc_tabla = proc_tabla;
   char *_tipo_tabla = tipo_tabla;
   short _programa_activo = programa_activo;
   unsigned long _c_programa_en_uso = c_programa_en_uso;
   short _n_p_tabla = n_p_tabla;
   short _maxpila = maxpila;
   /* ***** */
   short _memoplin = memoplin;
   int nm_ex_ = nm_ex;
   int np_ex_ = np_ex;
   M_EX              *m_ex_ = m_ex;
   P_EX              *p_ex_ = p_ex;
   int bucle_break_ = bucle_break;
   int apli_aborto_ = apli_aborto;
   int _operaerror = operaerror;
   short  _nfi = nfi;
   FI_DEF *_mpfi[MAXDEF];
   FI_DEF *_mpfie ;
   short    _pag_activa[MAXDEF];
   short    _mflagfi[MAXDEF];
   RELAS *_mpfirel[MAXDEF];
   RELAS *_mpfierel;
   FI_DEF *_empresa = NULL;
   RELAS  *_mpfiemp = NULL;
   int si_empresa = 0;
   int si_haymas = 0;
   short _fi_clave_lineas[MAXDEF];
   short _fi_modo_lineas[MAXDEF];   
   char *_n[6];
   char *c_help = NULL;   
   int _fecha_protegida = fecha_protegida;
   /* aunque ya sea mucho ...*/
   int _mpfi_enlazado = mpfi_enlazado;
   char *_mpfi_buf_enlazado = mpfi_buf_enlazado;
   int mpfi_enlazado = -1;
   char *mpfi_buf_enlazado = NULL;
   int w_f_n = win_fondo_neutro;
   int _apli_bloqueo = apli_bloqueo;
   struct limitacion_ *_limitaciones = limitaciones;
   int _numero_limitaciones = numero_limitaciones;
   int **_relacion_limitacion = relacion_limitacion;
   char log_aplica[20];
   char log_tmp[81];
   /*int _tracer_break[50];
   int _ntr_br = ntr_br;
   int _hay_traza = hay_traza;*/
   char __empresa[MAXDEFECTO];
   char _proceso[81];
   char _titulo[81];
   int _es_la_doble_;
   int _va_con_doble_;
   char _nomapli_doble[260];
   char _empresa_doble[260];
   char entrada_automatica[255];
   int __va_con_doble_ = va_con_doble_;
   int _punto_abierto[MAXDEF+5];
   char _referencia_help[128];
   int _con_euro = con_euro;   
   int es_el_erp = es_el_modulo_erp;
   char **_ds_entrada_automatica = ds_entrada_automatica;
   int _nds_entrada_automatica = nds_entrada_automatica;
   char *_ds_absoluta_impresora = ds_absoluta_impresora;
   es_el_modulo_erp = 0;   

   nds_entrada_automatica = 0;
   ds_entrada_automatica = NULL;
   ds_absoluta_impresora = NULL;
   

   /*
   if (mimodo_debug)
   { 
      if (nivel)
      {
         memcpy(_tracer_break,tracer_break,sizeof(int) * 50);
      }
   }
   */
   
   if (prime == 48) va_con_doble_ = 0;
   

   if (recur_punto > (MAXRECUR-1))
   {
      aviso(0,__Texto[33]);
      return(-1);
   }
   
   ultimo_ipfi = -1;

   inicializa_repinta_mificha();
   
   operaerror = 0;
   
   strcpy(__empresa,empresa);
   strcpy(_proceso,proceso);
   strcpy(_titulo,titulo);
   memcpy(_referencia_help,referencia_help,128);
   memset(referencia_help,0,128);
   
   win_fondo_neutro = 0;
   
   memcpy(_mpfi,mpfi,MAXDEF * sizeof(FI_DEF *)); /* ATENCION A ESTO */
   
   memcpy(_punto_abierto,punto_abierto,(MAXDEF+5)*sizeof(int));
   memset(punto_abierto,0,(MAXDEF+5)*sizeof(int));
   
   /* EN EL CASO DE DEF VIRTUAL */
   if (recur_punto > -1)
   {
      recur_mpfi[recur_punto] = (void *)&(_mpfi[0]);
      recur_nfi[recur_punto] = _nfi;
      salva_procs[recur_punto] = _en_pr;
      nsalva_procs[recur_punto] = _nen_pr;
   }
   recur_punto++;

   
   apli_bloqueo = -1;
   /*****************************/
   
   memcpy(_pag_activa,pag_activa,MAXDEF * sizeof(short));
   memcpy(_mflagfi,mflagfi,MAXDEF * sizeof(short));
   memcpy(_mpfirel,mpfirel,MAXDEF * sizeof(RELAS *));
   memcpy(_fi_clave_lineas,fi_clave_lineas,MAXDEF * sizeof(short));
   memcpy(_fi_modo_lineas,fi_modo_lineas,MAXDEF * sizeof(short));
   c_help = malloc(32);
   memcpy(c_help,clave_help,32);
   
   if (nivel)
      translevel++;
   nivel++;
   
   en_pr = NULL;
   nen_pr = 0;
   p_tabla = NULL;
   proc_tabla = NULL;
   tipo_tabla = NULL;
   programa_activo = -1;
   c_programa_en_uso = 0L;
   n_p_tabla = 0;
   maxpila = 23;
   memoplin = 11;
   
   do  
   {	  

	  i = strlen(param) - 1;	  
	  entrada_automatica[0] = 0;
	  if (param[i] == '}')
	  {
		  param[i] = 0;
		  i--;
		  while(i >= 0)
		  {
			  if (param[i] == '{')
			  {
				  param[i] = 0;
				  strcpy(entrada_automatica,param+i+1);
				  break;
			  }
			  i--;
		  }
	  }

	  
      _nomapli[0] = 0;
      
      bucle_break = 1;
      apli_aborto = 0; /* flag para abortar la aplicacion (QUIT) */
      strcpy(_modulo,modulo);
      si_haymas = _separa(param,_n,1,5);
      
      if (memcmp(param+1,"D:",2) == 0) 
	  {
         if (param[0] == 'F')
            strcpy(parametros,dirfic);
         else if (param[0] == 'D')
            strcpy(parametros,dirdef);
         else if (param[0] == 'G')
            strcpy(parametros,dirficb);
         param += 3;
      }
      else 
	  {
         if (quita_path(param) == param)
		 {		 
              strcpy(parametros,dirbin);
		 }
         else 
		 {
            strcpy(qo,param);
            strcpy(param,quita_path(qo));
            *(quita_path(qo)) = 0;
            if (qo[0] == ':') 
			{			   
               strcpy(parametros,dirbass);
               if (qo[1] == '/') {
                  strcat(parametros,qo+2);
               }
               else {
                  strcpy(_nomapli,nomapli);
                  strcpy(nomapli,qo+1);
                  nomapli[ strlen(nomapli) - 1 ] = 0;
                  strcpy(_dirfic,dirfic);
                  strcpy(_dirbase,dirbase);
                  strcpy(_fich_apli,fich_apli);
				  compon_dirbase(dirbase,dirbass,nomapli);
				  /*
                  strcpy(dirbase,dirbass);
                  strcat(dirbase,nomapli);
                  strcat(dirbase,"/");
				  */
                  strcpy(fich_apli,fichdir);
                  cambia_paths();
                  strcpy(dirfic,dirficb);
                  strcat(parametros,qo+1);
                  strcat(parametros,"bin/");
                  libera_permisos();
                  lee_permisos(nomapli);				  				  
               }
            }
            else
               strcpy(parametros,qo);
		}
      }
	  
      
      if (si_haymas == 1 || si_haymas == 2)
	  { /* hay que cargar una nueva empresa! */
         _empresa = mpfi[48];
         _mpfiemp = mpfirel[48];
         
         _es_la_doble_ = es_la_doble_;
         _va_con_doble_ = va_con_doble_;
         strcpy(_nomapli_doble,nomapli_doble);
         strcpy(_empresa_doble,empresa_doble);
         es_la_doble_ = 0;
         va_con_doble_ = 0;
         nomapli_doble[0] = 0;
         empresa_doble[0] = 0;
         
         i = punto;
         punto = 0;
         /* j = menua[0]; */
         strcpy(tmpj,menua[0]);

         if (si_haymas == 1)
         {
            strcpy(menua[0],"");
         }
         else
            strncpy(menua[0],_n[1],MAXMENUA);
         /* menua[0] = atoi(_n[1]); */
         strcpy(qo,_n[0]);
         strcat(qo,".tab");
         strcpy(dirfic,dirficb);				
         if (ejecuta_ap(qo,48,NULL) < 0) 
         {
            Ar();
            aviso(0,__Texto[34]);
            Cr();
            if (mpfi[48]) 
            {
               recoge_codigo_empresa(mpfi[48],qo);
               quitab(qo);
               strcat(dirfic,qo);
               strcat(dirfic,"/");					
            }
            _empresa = NULL;
            _mpfiemp = NULL;
			punto = i;
            ficha = -1;
            goto eje_salida;
         } 
         /* menua[0] = j; */
         strcpy(menua[0],tmpj);
         punto = i;
         ld_campo(dame_campo_empresa(mpfi[48]),mpfi[48],empresa);             
         if (va_con_doble_ && es_la_doble_ && empresa[0] == '*')
									{
            empresa[0] = '/';
									}
         quitab(empresa);
         si_empresa = 1;
         recoge_codigo_empresa(mpfi[48],qo);
         quitab(qo);
         strcat(dirfic,qo);
         strcat(dirfic,"/");			 
      }
      
      if (memcmp(param,"FF:",3) == 0) {
         if (strlen(param) < 9) 
		 {
            ficha = 0;
            goto eje_salida;
		 }
         ficha = -1;
         param += 3;
         *(param+8) = '\0';
         strcpy(qo,param+9);
         strcat(param,".tab");
      }
      
      strcat(parametros,param);   /* nombre del fichero */
      quitab(parametros);

      if (strbusca(quita_path(parametros),".") < 0)
      {
         strcat(parametros,".tab");
      }
      else
      {
         i = strlen(parametros) - 4;
         if (i >= 0)
		 {
            if (!memcmp(parametros+i,".run",4))
               memcpy(parametros+i+1,"tab",3);
		 }
      }
      
      
      strcpy(log_aplica,quita_path(parametros));	  
      
	  _especial_control_ = 1;
      if ((ipfi = lee_aplic(parametros,prime)) < 0) 
	  {
		 _especial_control_ = 2;
		 if (sin_mensaje_error_a > 0)
		 {
			 sin_mensaje_error_a--;
		 }
		 else
		 {
			 if (ipfi == -3) 
			 {
				Ar();
				aviso(0,__Texto[35]);
				Cr();
			 }
			 else if (ipfi == -2) 
			 {
				Ar();
				aviso(0,__Texto[36]);
				Cr();
			 }
			 else 
			 {
				Ar();
				if (es_el_erp)
				{
					aviso(0,__Texto[525]);
				}
				else
				{
					aviso(0,__Texto[37]);
				}
				Cr();
			 }
		 }
         ficha = -1;
         goto eje_salida;
      }

	   if (prime != 48)
	   {
		   int en_menu = 2;
		   if (recur_punto <= 0 && DespachadorExterno)
		   {
			  (*DespachadorExterno)((int)DESPACHO_EMENU,(int)en_menu,(void *)NULL);
		   }
	   }

	   if (se_esta_instalando()) {
		  sleep(1);
		  while (se_esta_instalando())
		  {
			  aviso(0,__Texto[526]);
		  }	  
	   }

	  _especial_control_ = 3;

	  if (entrada_automatica[0])
		rellena_entrada_automatica(entrada_automatica);

	  con_euro = 1;
	  /*
	  for (i = memoplin+maxpila*2;i < nm_ex;i++)
	  {
		  if (!strcmp("EURO",m_ex[i].nombre) && m_ex[i].m_tipo != CHARTIPO && m_ex[i].m_tipo != FECHATIPO)
		  {							
			  con_euro = 1;
			  break;
		  }
	  }
	  */

	  if (con_euro)
	  {		  
	      for (i = memoplin+maxpila*2;i < nm_ex;i++)
		  {
			  if (!strcmp("EUROTEXTO",m_ex[i].nombre) && m_ex[i].m_tipo == CHARTIPO)
			  {							
				  strcpy(DsEuroTexto,m_ex[i].mv.valc);
				  break;
			  }
		  }
	  }
	  _especial_control_ = 4;
     
	  if (DespachadorExterno)
		(*DespachadorExterno)((int)DESPACHO_EURO,(int)0,DsEuroTexto);
	  _especial_control_ = 5;
	  if (fgv_ver_euro)
	  {
		  _especial_control_ = 6;
	 	 if (DespachadorExterno)
			(*DespachadorExterno)((int)DESPACHO_EURO,(int)2,NULL);
	  }
	  _especial_control_ = 7;
      
      sprintf(clave_help,"%-30.30s",param);
	  _especial_control_ = 8;
      if ((i = strbusca(param,".")) == -1) 
	  {
		 _especial_control_ = 9;
         i = strlen(param);
      }
	  _especial_control_ = 10;
      if (memoplin > 10 && _par)
	  {
		 _especial_control_ = 11;
         strcpy(v_memo(5010)->alfabetica,_par);
	  }
	  _especial_control_ = 12;
      if (i > 8) i = 8;
      j = param[i];
	  _especial_control_ = 13;
      param[i] = 0;
	  _especial_control_ = 14;
      strcpy(modulo,param);
	  _especial_control_ = 15;
      param[i] = j;
	  _especial_control_ = 16;
      modif_defs();
	  _especial_control_ = 17;

	  (*DespachadorExterno)((int)DESPACHO_DATA_INFO,(int)1,(void *)modulo);

	  GrabaRegistroModulo(modulo);
      
      /********************** CONTROL DE DOBLEZ ************************/
      if (prime != 48)
      {
		 _especial_control_ = 18;
         va_con_doble_ = 0;
         if (nomapli_doble[0] && !strcmp(nomapli_doble,nomapli))
									{
			 _especial_control_ = 19;
            if (!mpfi[48]) /* caso excepcional desde mantenimiento de empresas */
            {
				_especial_control_ = 20;
               va_con_doble_ = 1;
            }
            else
            {
				_especial_control_ = 21;
               recoge_codigo_empresa(mpfi[48],qo);
			   _especial_control_ = 22;
               if (!memcmp(qo,empresa_doble,strlen(empresa_doble)))
               {
                  va_con_doble_ = 1;
               }
			   _especial_control_ = 23;
            }
            if (va_con_doble_ && es_la_doble_ && mpfi[48])
            {
				_especial_control_ = 24;
               rei = strlen(dirfic);
               if (rei > 1) 
               {
				   _especial_control_ = 25;
                  if (dirfic[rei-2] != '!')
                  {
					  _especial_control_ = 26;
                     cambiame_paths_especial(); /* conmuta */
                  }
               }
            }			
		}
      }
	  _especial_control_ = 27;
      /****************************************************************/
      
      if (memoplin > 8)
      {
		 _especial_control_ = 28;
         sprintf(v_memo(5008)->alfabetica,"%03d%03d",user_who,subnumero_de_usuario);
		 rw_user[10] = 0;
		 _especial_control_ = 29;
         strcpy(v_memo(5008)->alfabetica+7,rw_user);
      }
      
	  _especial_control_ = 30;
      if (!fuerza_empresa && auto_empresa && prime > 0 && !strlen(menua[punto]))
      {
          char tmpx[256];
          int fdx;
          
		  _especial_control_ = 31;
          /* verificar empresa por defecto */
          sprintf(tmpx,"%s%s.dfe",dirficb,rw_user);
		  _especial_control_ = 32;
          fdx = rw_open_(tmpx,O_RDONLY | O_BINARY);
          if (fdx >= 0)
          {
			  _especial_control_ = 33;
              rw_read_(fdx,menua[punto],MAXMENUA);
			  _especial_control_ = 34;
              rw_close_(fdx);
          }
      }

      fuerza_empresa = 0;

	  _especial_control_ = 35;
      if (prime > 0 && strlen(menua[punto]))
      { 
		  _especial_control_ = 36;
                                            /* control para el caso de que */
         abre1_rel(prime);                   /* sea acceso a empresa y se deba*/
		 _especial_control_ = 37;
         strcpy(parametros,menua[punto]);   /* saltar por ir a opcion mas baja*/
		 _especial_control_ = 38;
         cambia_ind(v_mpfi(prime),v_mpfi(prime)->f_campos[0].c_key);
		 _especial_control_ = 39;
         graba_codigo_empresa(v_mpfi(prime),parametros);                
		 _especial_control_ = 40;
         if (leer_reg(prime,ISEQUAL,0,0) < 0)
         {
			 _especial_control_ = 41;
             strcpy(menua[punto],"");			 
         }
		 _especial_control_ = 42;
         cambia_ind(v_mpfi(prime),1);
		 _especial_control_ = 43;
         cierra1_rel(prime);
      }
	  _especial_control_ = 44;

      if (1 /*prime <= 0 || !strlen(menua[punto])*/)
      {
		 _especial_control_ = 45;
         reset_pagactiva();                
		 _especial_control_ = 46;
         for (i=ipfi;i<nfi;i++) 
		 {
			_especial_control_ = 47;
            if (ipfi != 48 && i == 48) continue;
            if (f_v(i,RNOCERRAR) == 0) {
				_especial_control_ = 48;
               if (abre1_rel(i) >= 0)
			   {
				   _especial_control_ = 49;
                  f_s(i,RNOCERRAR);
			   }
            }
		}
		 _especial_control_ = 50;
         
         repon_externas((int)(prime <= 0 || !strlen(menua[punto])));
      
		 if (prime <= 0 || !strlen(menua[punto]))
		 {
			 if (mimodo_debug || logger_salida)
			 {
				 _especial_control_ = 51;
				sprintf(log_tmp,__Texto[271],log_aplica,v_mpfi(ipfi)->f_titulo);
				_especial_control_ = 52;
				/*pinta_en_logger(log_tmp);*/
				registro_logger("T",1,log_aplica,0,"",log_tmp,"","","");
			 }
			 _especial_control_ = 53;
         
			 guarda_estadistica();
			 _especial_control_ = 54;
			 guarda_incidencia(3,"");
		 }


    	 if (DespachadorExterno)
			(*DespachadorExterno)((int)DESPACHO_PANTALLAS,-2,NULL);

		 _especial_control_ = 55;

		 if (prime == 48)
		 {
			 ejecuta_otro(prime,80,O_NOR,0,0);
		 }
		 else
		 {
		     ejecuta_otro(-1,80,O_NOR,0,0);
		 }

		 _especial_control_ = 56;		 

		 if (prime <= 0 || !strlen(menua[punto]))
		 {         
			 if (p_tabla[0] != -1) 
			 {
				_especial_control_ = 57;
				p_ejecuta(0,O_NOR,0,9);			
			 }
			 else
			 {
				_especial_control_ = 58;
				ficha =  mante_ficha(ipfi,ficha,prime,qo);
			 }
		 }

		 _especial_control_ = 59;        		 
		 
		 if (prime == 48)
		 {
			 ejecuta_otro(prime,90,O_NOR,0,0);
		 }
		 else
		 {
			 ejecuta_otro(-1,90,O_NOR,0,0);
		 }


    	 if (DespachadorExterno && nivel < 2)
			(*DespachadorExterno)((int)DESPACHO_PANTALLAS,-3,NULL);

		 _especial_control_ = 60;

         guarda_incidencia(4,"");
		 _especial_control_ = 61;
         repon_estadistica();
		 
		 (*DespachadorExterno)((int)DESPACHO_DATA_INFO,(int)1,(void *)_modulo);
         
		 if (prime <= 0 || !strlen(menua[punto]))
		 {         
			 if (mimodo_debug || logger_salida)
										{
            
				sprintf(log_tmp,__Texto[272],log_aplica,v_mpfi(ipfi)->f_titulo);
				/*pinta_en_logger(log_tmp);*/
				registro_logger("T",2,log_aplica,0,"",log_tmp,"","","");
										}
		 }
         
         
		 guarda_externas((int)(prime <= 0 && (strlen(apli_corre) || nivel > 1)));


         if (prime > 0 && ficha >= 0)
         {
          char tmpx[256];
          int fdx;
          
          /* verificar empresa por defecto */
          sprintf(tmpx,"%s%s.dfe",dirficb,rw_user);
          fdx = rw_open_(tmpx,O_RDWR | O_BINARY | O_CREAT | O_TRUNC,S_IREAD | S_IWRITE);
          if (fdx >= 0)
          {
              recoge_codigo_empresa(mpfi[48],tmpx);         
              quitab(tmpx);
              rw_write_(fdx,tmpx,MAXMENUA);
              rw_close_(fdx);
          }

         }
      }      

      cierra_rel(prime);
	  
      if (memoplin > 10 && _par)
         strcpy(_par,v_memo(5010)->alfabetica);

      if (ixx_absoluto < 2 && nivel < 2)
      {
         ixx_limpia_pseudos(1);
      }
      rw_ex_desenlaza_mpfi(0,0,0); /* por si acaso */

	  if (con_euro)
	  {		  
	      for (i = memoplin+maxpila*2;i < nm_ex;i++)
		  {
			  if (!strcmp("EUROTEXTO",m_ex[i].nombre) && m_ex[i].m_tipo == CHARTIPO)
			  {							
				  strcpy(DsEuroTexto,m_ex[i].mv.valc);				  
			  }
			  if (!strcmp("EURODV",m_ex[i].nombre) && m_ex[i].m_tipo != CHARTIPO && m_ex[i].m_tipo != FECHATIPO)
			  {
				 DsDeciModoOtro = (int)m_ex[i].mv.vald;
			  }
			  if (!strcmp("EURO",m_ex[i].nombre) && m_ex[i].m_tipo != CHARTIPO && m_ex[i].m_tipo != FECHATIPO)
			  {
				 DsEuroConvierte = m_ex[i].mv.vald;
			  }
		  }
	  }


	  rellena_entrada_automatica(NULL);
      libera_aplic(prime);	  

      if (mimodo_debug)
      {
         /* por si se ha quedado habierta !! */
         fin_traza();
      }      
      

      if (strlen(apli_corre)) 
	  {
         param = _param;
         strcpy(param,apli_corre);
         apli_corre[0] = '\0';
         strcpy(modulo,_modulo);		 
         if (_nomapli[0]) 
			{
            strcpy(nomapli,_nomapli);
            strcpy(dirfic,_dirfic);
            strcpy(dirbase,_dirbase);
            strcpy(fich_apli,_fich_apli);
            cambia_paths();
            libera_permisos();
            lee_permisos(nomapli);
			if (si_empresa) 
            {
               strcpy(dirfic,dirficb);
               free_defpri(mpfi[48]);
               libera_rela(48);
               es_la_doble_ = _es_la_doble_;
               va_con_doble_ = _va_con_doble_;
               if (es_la_doble_ && va_con_doble_)
                  caracter_de_fecha = '/';
               else
                  caracter_de_fecha = '.'; 
               strcpy(nomapli_doble,_nomapli_doble);
               strcpy(empresa_doble,_empresa_doble);
               
               mpfi[48] = _empresa;
               mpfirel[48] = _mpfiemp;
               if (mpfi[48]) {
                  ld_campo(dame_campo_empresa(mpfi[48]),mpfi[48],empresa);
                  quitab(empresa);
                  if (va_con_doble_ && es_la_doble_ && empresa[0] == '*')
                  {
                     empresa[0] = '/';
                  }
                  recoge_codigo_empresa(mpfi[48],qo);
                  quitab(qo);
                  strcat(dirfic,qo);
                  strcat(dirfic,"/");
               }
               else
                  strcpy(empresa,aginfo);
               
               _mpfiemp = NULL;
               _empresa = NULL;
               si_empresa = 0;			   
            }
		}	    
        continue;
      }
      
	  
      if (nivel < 2)
         borra_externas();	   
      
      
eje_salida:

	  _especial_control_ = 0;

      fuerza_empresa = 0;
      strcpy(modulo,_modulo);
      if (_nomapli[0]) {
         strcpy(nomapli,_nomapli);
         strcpy(dirfic,_dirfic);
         strcpy(dirbase,_dirbase);
         strcpy(fich_apli,_fich_apli);
         cambia_paths();
         libera_permisos();
         lee_permisos(nomapli);
      }
      if (si_empresa) {
         strcpy(dirfic,dirficb);
         free_defpri(mpfi[48]);
         libera_rela(48);
         mpfi[48] = _empresa;
         mpfirel[48] = _mpfiemp;
         es_la_doble_ = _es_la_doble_;
         va_con_doble_ = _va_con_doble_;
         if (es_la_doble_ && va_con_doble_)
            caracter_de_fecha = '/';
         else
            caracter_de_fecha = '.'; 
         strcpy(nomapli_doble,_nomapli_doble);
         strcpy(empresa_doble,_empresa_doble);
         
         if (mpfi[48]) 
		 {
            ld_campo(dame_campo_empresa(mpfi[48]),mpfi[48],empresa);
            if (va_con_doble_ && es_la_doble_ && empresa[0] == '*')
            {
               empresa[0] = '/';
            }
            quitab(empresa);
            recoge_codigo_empresa(mpfi[48],qo);
            quitab(qo);
            strcat(dirfic,qo);
            strcat(dirfic,"/");
									}
         else
            strcpy(empresa,aginfo);
         _mpfiemp = NULL;
         _empresa = NULL;
         si_empresa = 0;		  
      }
      
      break;
  } while (-1);
  

  con_euro = _con_euro;  
  
  if (!con_euro && DespachadorExterno)
  {
	 (*DespachadorExterno)((int)DESPACHO_EURO,(int)1,NULL);
  }
  

  
  memcpy(punto_abierto,_punto_abierto,(MAXDEF+5)*sizeof(int));
  _mpfie = mpfi[48];
  memcpy(mpfi,_mpfi,MAXDEF * sizeof(FI_DEF *));
  mpfi[48] = _mpfie;
  memcpy(pag_activa,_pag_activa,MAXDEF * sizeof(short));
  memcpy(mflagfi,_mflagfi,MAXDEF * sizeof(short));
  _mpfierel = mpfirel[48];
  memcpy(mpfirel,_mpfirel,MAXDEF * sizeof(RELAS *));
  mpfirel[48] = _mpfierel;
  memcpy(fi_clave_lineas,_fi_clave_lineas,MAXDEF * sizeof(short));
  memcpy(fi_modo_lineas,_fi_modo_lineas,MAXDEF * sizeof(short));
  if (c_help) 
  {
     memcpy(clave_help,c_help,32);
     free(c_help);
     c_help = NULL;
  }
  
  
  en_pr = _en_pr;
  nen_pr = _nen_pr;
  p_tabla = _p_tabla;
  proc_tabla = _proc_tabla;
  tipo_tabla = _tipo_tabla;
  c_programa_en_uso = _c_programa_en_uso;
  n_p_tabla = _n_p_tabla;
  maxpila = _maxpila;
  fecha_protegida = _fecha_protegida;
  mpfi_enlazado = _mpfi_enlazado;
  mpfi_buf_enlazado = _mpfi_buf_enlazado;
  nfi = _nfi;
  m_ex = m_ex_;
  p_ex = p_ex_;
  nm_ex = nm_ex_;
  np_ex = np_ex_;
  bucle_break = bucle_break_;
  apli_aborto = apli_aborto_;
  ds_entrada_automatica = _ds_entrada_automatica;
  nds_entrada_automatica = _nds_entrada_automatica;
  ds_absoluta_impresora = _ds_absoluta_impresora;
  memoplin = _memoplin;
  operaerror = _operaerror;
  nivel--;
  recur_punto--;
  ultimo_ipfi = -1;
  if (translevel)
     translevel--;
  programa_activo = _programa_activo;
  if (programa_activo != -1 && nen_pr)
  {
     activa_programa(programa_activo);
  }
  if (!nivel)
  {
     /* esto es al volver a menu */

     reset_reserva();
     rw_inf(-1);
     rw_finimp();
  }
  
  /*
  if (mimodo_debug)
  {
     ntr_br = _ntr_br;
     if (nivel)
     {
        memcpy(tracer_break,_tracer_break,sizeof(int) * 50);
        if (_hay_traza && programa_activo != -1 && nen_pr)
        {
           fin_traza();
           ini_traza(programa_activo);
        }
     }
  }
  */
  
  limitaciones = _limitaciones;
  numero_limitaciones = _numero_limitaciones;
  relacion_limitacion = _relacion_limitacion;
  
  win_fondo_neutro = w_f_n;
  apli_bloqueo = _apli_bloqueo;
  
  strcpy(empresa,__empresa);
  strcpy(proceso,_proceso);
  strcpy(titulo,_titulo);

 
  if (prime != 48) va_con_doble_ = __va_con_doble_;
  
  if (va_con_doble_)   
  {
     if (es_la_doble_ && empresa[0] == '*')
        empresa[0] = '/';
     else
        if (!es_la_doble_ && empresa[0] == '/')
           empresa[0] = '*';
  }
  
  memcpy(referencia_help,_referencia_help,128);



  GrabaRegistroModulo(modulo);
  
 
  return(ficha);
}

int regulariza_menua_(char *dato)
{
   int i;
   memcpy(&punto,dato,sizeof(short));
   dato += sizeof(short);
   for (i = 0;i < 25;i++)
   {
      memcpy(menua[i],dato,MAXMENUA+1);
      dato += (MAXMENUA+1);
   }
   
   return(0);
}


int llama_menu(nmen,pro,con1)
char *nmen,*pro;
int con1;
{
   int m,o,salida = -1,prt;
   int nopinta;
   char tmp[512];   
   char tmpxx[1024];                    
   char alternativa[128];
   unsigned char c;
   int _wdesplegamenu = wdesplegamenu;
   
   dir_smenu = dirfic;
   dir_sgmenu = dirficb;
   

   /*
   for (m = 0;m < 1000000;m++)
   {
	  system_proceso("c:/pepe.bat");
   }
   */
   
   if (verifica_sistema(&prt)) return(-1);
   
   sprintf(alternativa,"%s%03d/%s",rw_directorio("dev/usr/"),user_who,quita_path(nmen));
   do {
      if ((m = leemenu(alternativa)) < 0)
         if ((m = leemenu(nmen)) < 0) 
         {
            sprintf(tmp,__Texto[273],nmen);
            Ar();
            aviso(0,tmp);
            Cr();
            es_menu_acceso_directo = 0;
            wdesplegamenu = _wdesplegamenu;
            return(-1);
         }
         if (!con_acceso_directo)
            es_menu_acceso_directo = 0;
         con_acceso_directo = 0;
        
		if (!strlen(menua[punto]) && strlen(menua[punto+1]))
		{
			int i;
			for (i = punto;i < 24;i++)
			{
				strcpy(menua[i],menua[i+1]);
			}
			menua[24][0] = 0;
		}
		 
         if (!strlen(menua[punto+1])) 
         {
            strcpy(titulo,nommenu);
            strcpy(proceso," Indice ");
            if (!prt &&  hay_permiso(pro,-1) )
            {/*Usuario de acceso directo a opcion*/
               proceso[0] = 0;
               if (con1)
               {
                  o = -1;salida = -1;
               } /* segunda entrada --> salir */
               else
               {
                  salida = 0;
                  libera();
                  break;
               }
            }
            else 
			{
               nopinta = 0;
               cls();
               Ar();Bl(4);Cr();
			   if (!punto)
			   {
					cabecera_v(rw_version,proceso,titulo,empresa);
			   }
			   else
			   {
					cabecera(proceso,titulo,empresa);
			   }
            }
            
            if (strlen(menua[punto]))
			{
               wdef = atoi(menua[punto]);
			   menua[punto][0] = 0;
			}
            
            if (nopinta == 0) 
			{
               sprintf(clave_help,"%-30.30s",quita_path(nmen));
               if ( wdef &&
                  (wdef%100) && es_menu_acceso_directo &&
                  wmenu[wdef/100]->m_op[(wdef%100)-1]->o_menuhijo == -1 /*&&
                  ((c = *wmenu[wdef/100]->m_op[(wdef%100)-1]->o_programa) >= 'a' &&
                  c <= 'z')*/
                  )
               {
                  o = wdef%100;
                  m = wdef/100;
               }
               else
               {
                  
                  if (DespachadorExterno)
                  {
                     (*DespachadorExterno)((int)DESPACHO_PANTALLAS,(int)1,nommenu);
                  }                 
#ifdef RWSERVER
                  if (en_conexion_externa)
                  {                    
                    funcion_windows_cliente(1012,tmpxx,compon_menua_(tmpxx));
                  }
#endif
				  GrabaRegistroModulo(quita_path(nmen));
                  do
                  {
                    o = w_menus(&m);
                    if (o == -3)
                    {
                      if (entra_empresa(NULL,NULL) == -2)
                      {
                          o = wdef%100;
                          m = wdef/100;
						  if (o < 1)
							  o = 1;
						  if (m < 0)
							  m = 0;
                          break;
                      }
                      continue;
                    } 
                    break;
                  } while(-1);
               }
            }
            
            sprintf(menua[punto],"%d",wdef);
            
#ifdef RWSERVER
            if (en_conexion_externa)
            {               
               funcion_windows_cliente(1012,tmpxx,compon_menua_(tmpxx));
            }
#endif
            
            
            if (o > 0)
            {
               es_menu_acceso_directo = 0;
               if (!prt && o <= wmenu[m]->m_nop && hay_permiso(wmenu[m]->m_op[o-1]->o_programa,0))
               {
                  strcpy(pro,wmenu[m]->m_op[o-1]->o_programa);
				  strcpy(tmpxx,wmenu[m]->m_op[o-1]->o_nombre);
				  tmpxx[80] = 0;
                  strcpy(proceso,tmpxx);
                  if (nopinta == 0)
                  {
                     /*
                     sprintf(tmp," Cargando la opcion  '%s'",proceso);
                     Ar();
                     mensaje(0,tmp);
                     Cr();
                     */
                  }
               }
               else
               {
                  pro[0] = '\0';
                  Ar();
                  if (prt)
                     aviso(0,__Texto[38]);
                  else
                     aviso(0,__Texto[39]);
                  Cr();
               }
            }
         }
         else 
		 {
            if (strlen(menua[punto]))
               wdef = atoi(menua[punto]);
            if (!strcmp(menua[punto+1],"XXX"))
            {
               menua[punto+1][0] = 0;
               w_menus(NULL);
            }
            o = atoi(menua[punto])%100;
            m = atoi(menua[punto])/100;
            strcpy(pro,wmenu[m]->m_op[o-1]->o_programa);
			strcpy(tmpxx,wmenu[m]->m_op[o-1]->o_nombre);
			tmpxx[80] = 0;
            strcpy(proceso,tmpxx);
#ifdef RWSERVER
            if (en_conexion_externa)
            {               
               funcion_windows_cliente(1012,tmpxx,compon_menua_(tmpxx));
            }
#endif
         }
         
         libera();
         if (o > 0) 
		 {
            if (pro[0] != '\0') 
			{
               salida = 0;
               break;
            }
         }
  } while (o > 0);
  if (salida && !punto)
  {
     menua[punto][0] = 0;
  }
  es_menu_acceso_directo = 0;
  wdesplegamenu = _wdesplegamenu;
  return(salida);
}

#ifdef NOHACEFALTA
int empresa_directa(char *fiempre,char *codigo)
{        
	     char tmpj[256];
		 char qo[256];
         int i = punto;
         punto = 0;
         
         strcpy(tmpj,menua[0]);

         strncpy(menua[0],codigo,MAXMENUA);
         
         strcpy(qo,fiempre);
         strcat(qo,".tab");
         strcpy(dirfic,dirficb);				
         if (ejecuta_ap(qo,48,NULL) < 0) 
         {
            Ar();
            aviso(0,"Empresa inaccesible!");
            Cr();
            if (mpfi[48]) 
            {
               recoge_codigo_empresa(mpfi[48],qo);
               quitab(qo);
               strcat(dirfic,qo);
               strcat(dirfic,"/");					
            }
			strcpy(menua[0],tmpj);
			punto = i;
			return -1;
         } 
         /* menua[0] = j; */
         strcpy(menua[0],tmpj);
         punto = i;
         ld_campo(dame_campo_empresa(mpfi[48]),mpfi[48],empresa);/*KEMP*/
         if (va_con_doble_ && es_la_doble_ && empresa[0] == '*')
		 {
            empresa[0] = '/';
		 }
         quitab(empresa);
         recoge_codigo_empresa(mpfi[48],qo);
         quitab(qo);
         strcat(dirfic,qo);
         strcat(dirfic,"/");
		 if (agi_codi_empre)
		 {
			 strcpy(agi_codi_empre,qo);
		 }
		 return 0;
}
#endif


opcion_ej(pro,codigo)
char *pro,*codigo;
{
   char *nmen;
   char *fiempre;
   char *_nomapli,*_dirfic,*_fich_apli;
   char *_agi_empresa = agi_empresa;
   char *_agi_codi_empre = agi_codi_empre;
   char *_agi_aplicacion = agi_aplicacion;
   FI_DEF *defempre = NULL;
   char tmp[350];
   char tmp2[350];
   char xpro[350];
   char par[350];
   char codi1[250];
   char codi2[250];
   int i;
   char _empresa[MAXDEFECTO+10];
   char _proceso[260];
   char _titulo[260];
   int salva_entorno = 0;
   RELAS  *defemprerel = NULL;

   
   if (*pro == '@')
   {
      pro++;
	  /*
	  if (fuerza_empresa)
	  {
		  fuerza_empresa = 0;
		  //entra_empresa
	  }
	  */
#ifdef RWXARXA
#ifdef RWSERVER
      if (en_conexion_externa)
      {
         return(opcion_ej_en_terminal(pro,codigo));
      }
#endif
#endif
   }
   

   strcpy(_empresa,empresa);
   strcpy(_proceso,proceso);
   strcpy(_titulo,titulo);  

   if (*pro =='(')
   {      
	   char *apro;
	   int conempresa = 0;
	   /* de momento no guardamos el estado anterior ...
	      si se llama de otro sitio que un acceso directo es un problema */	  
	  pro++;
	  apro = pro;
	  while(*apro && *apro != ' ' && *apro != ')') 
		  apro++;
	  if (*apro == ' ' || *apro == ')')
	  {
		  salva_entorno = 1;
          _nomapli = malloc(25);
          _dirfic = malloc(51);
          _fich_apli = malloc(15);
          memcpy(_nomapli,nomapli,25);
          memcpy(_dirfic,dirfic,51);
          memcpy(_fich_apli,fich_apli,15);
		  defempre = mpfi[48];
		  defemprerel = mpfirel[48];
		  

		  if (*apro == ' ')
		  {
			  conempresa = 1;
			  mpfi[48] = NULL;
			  mpfirel[48] = NULL;
		  }
		  *apro = 0;
		  apro++;
          strcpy(nomapli,pro);		  

		  compon_dirbase(dirbase,dirbass,nomapli);
		  /*
          strcpy(dirbase,dirbass);
          strcat(dirbase,nomapli);
          strcat(dirbase,"/");
		  */

          strcpy(fich_apli,fichdir);
          cambia_paths();
          strcpy(dirfic,dirficb);
          libera_permisos();
          lee_permisos(nomapli);
		  agi_aplicacion = nomapli;
		  if (conempresa)
		  {
			  pro = apro;
			  while(*apro && *apro != ' ' && *apro != ')') apro++;
			  if (*apro == ' ' || *apro == ')')
			  {
				  char *ppro;
				  ppro = apro;
				  if (*apro == ' ')
				  {					  
					  *ppro = 0;
					  ppro++;
					  apro++;					  
					  while(*ppro && *ppro != ')') ppro++;
					  *ppro = 0;
				  }	
				  else
					  *apro = 0;
				  ppro++;
				  

				  strcpy(codi1,pro);                  
                  agi_empresa = codi1;
				  agi_codi_empre = codi2;
				  codi2[0] = 0;

				  /*
				  if (empresa_directa(pro,apro)< 0)
					  return 0;
					*/
				  if (entra_empresa(pro,apro)< 0)
					  return 0;
				  apro = ppro;
			  }
		  }
		  pro = apro;
	  }
   }

   if (*pro == '|')
   {
	   int hayespacio;

	   strcatd(pro,"|");
	   hayespacio = 0;
	   for (i = 0;pro[i];i++)
	   {
		   if (pro[i] == ' ')
		   {
			   hayespacio++;
			   while(pro[i] == ' ') i++;
			   if (!pro[i])
			   {
				   hayespacio--;
				   break;			  
			   }
			   if (pro[i] == ';')
			   {
				   hayespacio--;
			   }
		   }
		   if (pro[i] == 4)
		   {
			   break;
		   }
	   }
	   if (!pro[i]);
	   {
		   /*
		   if (agi_codi_empre && hayespacio == 1)
		   {
			   pro[i] = ' ';
			   i++;
			   strcpy(pro+i,agi_codi_empre);
			   i = strlen(pro);
		   }
		   */
		   if (fuerza_empresa)
		   {				
				pro[i] = 5;
				i++;				
		   }
		   pro[i] = 4;
		   strcpy(pro+i+1,nommenu);
		   i = strlen(pro);
		   pro[i] = 4;
		   strcpy(pro+i+1,proceso);		   
		   i = strlen(pro);
		   pro[i] = 4;
		   strcpy(pro+i+1,dirbass);
		   i = strlen(pro);
		   pro[i] = 4;
		   strcpy(pro+i+1,nomapli);
		   i = strlen(pro);
		   pro[i] = 4;
		   strcpy(pro+i+1,fich_apli);
		   i = strlen(pro);
		   pro[i] = 4;
		   strcpy(pro+i+1,dirficb);
		   i = strlen(pro);
		   pro[i] = 4;
		   strcpy(pro+i+1,dirfic);
		   /*****/
	       i = strlen(pro);
		   pro[i] = 4;
		   if (agi_empresa) 
			   strcpy(pro+i+1,agi_empresa);		   
		   else
			   strcpy(pro+i+1,"");
	       i = strlen(pro);
		   pro[i] = 4;
		   if (agi_codi_empre) 
			   strcpy(pro+i+1,agi_codi_empre);		   
		   else
			   strcpy(pro+i+1,"");

	   }
	   if (DespachadorExterno)
	   {
           cabecera_v(NULL,proceso,titulo,empresa);
		   (*DespachadorExterno)((int)DESPACHO_MULTIPLE,7,pro);
		   if (pro[1] == '|' && pro[2] == 4 && pro[3]) {
			   for (i = 3;pro[i] && pro[i] != 4;i++);
			   if (pro[i] == 4) {
				   pro[i] = 0;
				   entra_empresa(pro+3,pro+i+1);
			   }
		   }		   
	   }
	   /*return(0);*/	   
	   goto salida_opcion_ej;
   }

   if (*pro == '*')
      pro++;


   if (*pro =='[')
     {
	 for(i = 1;pro[i] && pro[i] != ']';i++);
	 if (i == 1 && pro[i] == ']')
	   {
	   strcpy(xpro,pro+i+1);
	   quitabp(xpro);
       pro = xpro;
	   }
	 else
	 if (pro[i] == ']')
	   {
       memcpy(tmp,pro+1,i-1);  
	   tmp[i-1] = 0;
	   strcpy(xpro,pro+i+1);
	   quitabp(xpro);
       for(i = 0;tmp[i] && tmp[i] != ';';i++);
       if (tmp[i] == ';' || tmp[i] == 127)
         {
         tmp[i] = 0;
		 quitab(tmp);
         strcat(xpro,";");
		 strcat(xpro,tmp+i+1);
		 }
	   ejecuta_ap(tmp,0,xpro);
	   pro = xpro;
	   }
	 if (!pro[0])
	   return(0);
	 }


   strcpy(tmp,pro);
   i = _separa(pro,_n,0,10);
   if (i == -1)
      return(0);   
   
  
   if (!es_numero(pro))
   {
      if (strlen(pro) > 4 && !strcmp(".men",pro+strlen(pro)-4))
      {
         int con1;
         punto++;
         con1 = 0;
         con_acceso_directo = 1;
		 if (pro[0] == ':')
		 {
			 strcpy(tmp,dirbass);
			 if (pro[1] == '/')
			 {
				 strcat(tmp,pro+2);				 
			 }
			 else
			 {
				 strcat(tmp,pro+1);
			 }
		 }
		 else
		 {
			 strcpy(tmp,dirbase);
			 strcat(tmp,pro);
		 }
         tmp[strlen(tmp)-4] = 0;
         tmp2[0] = 0;
         fuerza_empresa = 0;
         while (llama_menu(tmp,tmp2,con1) == 0) 
         {
            opcion_ej(tmp2,codigo);
            con1 = 1;
            fuerza_empresa = 0;
         }
         menua[punto][0]= 0;
         punto--;
      }
      else
      {
         for (i = 0;tmp[i] && tmp[i] != ';' && tmp[i] != 127;i++);
         if (tmp[i] == ';' || tmp[i] == 127)
         {
            tmp[i] = 0;
			strcpy(par,tmp+i+1);
            ejecuta_ap(tmp,0,par);
         }
         else
            ejecuta_ap(tmp,0,NULL);
      }
   }
   else
      if (atoi(pro) == 1) 
	  {
		 GrabaRegistroModulo(_n[0]);
         ejecutas(_n[0]);
      }
      else
         if (atoi(pro) == 4)
		 {
			GrabaRegistroModulo(_n[0]);
            ejecutae(_n[0]);
		 }
         else {
            *(_n[0] - 1) = ' ';
            i = _separa(pro,_n,1,10);
            switch(atoi(pro)) 
			{
            case 0:			  
               agi_empresa = NULL;
               agi_codi_empre = NULL;
               agi_aplicacion = NULL;
               nmen = malloc(51);
               fiempre = malloc(50);
			   if (!salva_entorno)
			   {
				   _nomapli = malloc(25);
				   _dirfic = malloc(51);
				   _fich_apli = malloc(15);
				   memcpy(_nomapli,nomapli,25);
				   memcpy(_dirfic,dirfic,51);
				   memcpy(_fich_apli,fich_apli,15);
				   defempre = mpfi[48];
			   }
               strcpy(nomapli,_n[0]);
               strcpy(dirficb,_n[1]);
               dirficb[14] = 0;
			   if (!strcmp(dirficb,"fich"))
			   {
				   strcpy(dirficb,fich_apli);
			   }
               strcpy(fich_apli,dirficb);
               strcpy(fiempre,_n[2]);
               strcpy(nmen,_n[3]);

			   compon_dirbase(dirbase,dirbass,nomapli);
			   /*
			   strcpy(dirbase,dirbass);
			   strcat(dirbase,nomapli);
			   strcat(dirbase,"/");
			   */
               agi_aplicacion = nomapli;
               
               cambia_paths();
               strcpy(dirfic,dirficb);
               
               if (strcmp(fiempre,"NULL") != 0)
               {
                  strcpy(tmp,fiempre);
                  agi_empresa = tmp;
                  strcat(fiempre,".tab");
               }
               
               libera_permisos();
               lee_permisos(nomapli);
               entra_aplicacion(nmen,fiempre,pro);
               agi_empresa = _agi_empresa;
               agi_codi_empre = _agi_codi_empre;
               agi_aplicacion = _agi_aplicacion;
               free(nmen);
               free(fiempre);
               memcpy(nomapli,_nomapli,25);
			   compon_dirbase(dirbase,dirbass,nomapli);
			   /*
			   strcpy(dirbase,dirbass);
			   strcat(dirbase,nomapli);
			   strcat(dirbase,"/");
			   */

               memcpy(fich_apli,_fich_apli,15);
               mpfi[48] = defempre;
               cambia_paths();
               memcpy(dirfic,_dirfic,51);
   			   if (!salva_entorno)
			   {
				   free(_nomapli);
				   free(_dirfic);
				   free(_fich_apli);
			   }
               libera_permisos();
               lee_permisos(nomapli);
               break;
            case 2:
               strcpy(tmp2,_n[0]);
               if (_n[0] == quita_path(_n[0]))
                  strcatd(tmp2,dirbass);
               sprintf(tmp,"%s %s %s %s %s %s %s %s %s",tmp2,dirbase,_n[1],_n[2],
                  codigo,_n[3],_n[4],_n[5],_n[6]);
			   GrabaRegistroModulo(tmp);
               ejecutae(tmp);
               break;
            case 3:
               strcpy(tmp2,_n[0]);
               if (_n[0] == quita_path(_n[0]))
                  strcatd(tmp2,dirbass);
               sprintf(tmp,"%s %s %s",tmp2,dirbase,dirfic);
			   if (_n[1][0])
			   {
				   strcat(tmp," ");
				   strcat(tmp,_n[1]);
				   strcat(tmp," ");
				   strcat(tmp,codigo);
			   }
			   GrabaRegistroModulo(tmp);
               fgv_arranca(tmp);
               break;
            default:
               break;
            }
         }

salida_opcion_ej:
		 if (salva_entorno)
		 {
               memcpy(nomapli,_nomapli,25);
			   compon_dirbase(dirbase,dirbass,nomapli);
			   /*
			   strcpy(dirbase,dirbass);
			   strcat(dirbase,nomapli);
			   strcat(dirbase,"/");
			   */

               memcpy(fich_apli,_fich_apli,15);
               mpfi[48] = defempre;
			   mpfirel[48] = defemprerel;
               cambia_paths();
               memcpy(dirfic,_dirfic,51);
               free(_nomapli);
               free(_dirfic);
               free(_fich_apli);
               libera_permisos();
               lee_permisos(nomapli);
		 }
         
         strcpy(empresa,_empresa);
         strcpy(proceso,_proceso);
         strcpy(titulo,_titulo);
         if (va_con_doble_)   
         {
            if (es_la_doble_ && empresa[0] == '*')
               empresa[0] = '/';
            else
               if (!es_la_doble_ && empresa[0] == '/')
                  empresa[0] = '*';
         }
         
         return(0);
}

int entra_empresa(char *fiempre,char *aesta)
{
    char tmpx[256];
    char tmpy[MAXMENUA+1];
    FI_DEF *ant_pfi = NULL;
    RELAS *ant_rel = NULL;   
    int pp = punto;
    char _dirfic[300];
    char _empresa[300];
    char _titulo[300];
    int modo = 0;    

    tmpy[0] = 0;
    if (!fiempre)
    {
        if (!agi_empresa)
        {
            if (!aesta)
                fuerza_empresa = 1;
            return -2;
        }

        sprintf(tmpx,"%s%s.dfe",dirficb,rw_user);
        unlink(tmpx);

        sprintf(tmpx,"%s.tab",agi_empresa);
        fiempre = tmpx;
        ant_pfi = mpfi[48];
        ant_rel = mpfirel[48];
        mpfi[48] = NULL;
        mpfirel[48] = NULL;
        pp--;
        strncpy(tmpy,menua[pp],MAXMENUA);        
        modo = 1;
    }

    if (!agi_codi_empre) return(-1);

        if (aesta)
        {
            strncpy(menua[pp],aesta,MAXMENUA);
        }

         strcpy(_dirfic,dirfic);
         strcpy(_empresa,empresa);
         strcpy(_titulo,titulo);

         strcpy(dirfic,dirficb);
         strcpy(empresa,aginfo);
         /*strcpy(titulo," Acceso ");*/
		 strcpy(titulo,nommenu);

         if (ejecuta_ap(fiempre,48,NULL) < 0) {
            if (mpfi[48] != NULL)
            {
               free_defpri(mpfi[48]);
               libera_rela(48);
            }
            mpfi[48]    = ant_pfi;
            mpfirel[48] = ant_rel;
            strncpy(menua[pp],tmpy,MAXMENUA);            

            strcpy(dirfic,_dirfic);
            strcpy(empresa,_empresa);
            strcpy(titulo,_titulo);
            if (modo)
            {
               cls();
               Ar();Bl(4);Cr();
               /*cabecera_v(rw_version,proceso,titulo,empresa);*/
			   cabecera(proceso,titulo,empresa);
            }
            return(-1);
         }
         recoge_codigo_empresa(mpfi[48],agi_codi_empre);
         ld_campo(dame_campo_empresa(mpfi[48]),mpfi[48],empresa);
         if (va_con_doble_ && es_la_doble_ && empresa[0] == '*')
         {
            empresa[0] = '/';
         }
         quitab(agi_codi_empre);
         
         quitab(empresa);
         strncpy(menua[pp],agi_codi_empre,MAXMENUA);
         strcat(dirfic,agi_codi_empre);
         strcat(dirfic,"/");
         if (modo)
         {
            cls();
            Ar();Bl(4);Cr();
            /*cabecera_v(rw_version,proceso,titulo,empresa);*/
			cabecera(proceso,titulo,empresa);
         }
         return(0);
}

entra_aplicacion(nmen,fiempre,pro)
char *nmen,*fiempre,*pro;
{
   char codigo[MAXMENUA+1];
   int verdad = -1,i,con1;
   int _es_la_doble_;
   int _va_con_doble_;
   char _nomapli_doble[260];
   char _empresa_doble[260];
   
   dirbase_ayuda = dirbase;
   
   if (strcmp(nmen,"NULL") == 0) 
   {
      _es_la_doble_ = es_la_doble_;
      _va_con_doble_ = va_con_doble_;
      strcpy(_nomapli_doble,nomapli_doble);
      strcpy(_empresa_doble,empresa_doble);
      es_la_doble_ = 0;
      caracter_de_fecha = '.'; 
      va_con_doble_ = 0;
      nomapli_doble[0] = 0;
      empresa_doble[0] = 0;
      verdad = 0;
      strcpy(empresa,aginfo);
      /*strcpy(titulo," Mantenimiento ");*/
	  strcpy(titulo,nommenu);
      ejecuta_ap(fiempre,0,NULL);
      es_la_doble_ = _es_la_doble_;
      va_con_doble_ = _va_con_doble_;
      if (es_la_doble_ && va_con_doble_)
         caracter_de_fecha = '/';
      else
         caracter_de_fecha = '.'; 
      strcpy(nomapli_doble,_nomapli_doble);
      strcpy(empresa_doble,_empresa_doble);
   }
   else
      strcatd(nmen,dirbase);
   
   while (verdad) {
      
      
      _es_la_doble_ = es_la_doble_;
      _va_con_doble_ = va_con_doble_;
      strcpy(_nomapli_doble,nomapli_doble);
      strcpy(_empresa_doble,empresa_doble);
      
      es_la_doble_ = 0;
      caracter_de_fecha = '.'; 
      
      va_con_doble_ = 0;
      nomapli_doble[0] = 0;
      empresa_doble[0] = 0;
      
      mpfi[48] = NULL;
      mpfirel[48] = NULL;
	  

      if (strcmp(fiempre,"NULL") != 0) 
      {
         punto++;
		 codigo[0] = 0;
         agi_codi_empre = codigo;		
         if (entra_empresa(fiempre,NULL) < 0)
         {
            punto--;
            agi_codi_empre = NULL;
            break;
         }		 
      }
      else
         strcpy(codigo,"NULL");
      
      punto++;
      
      con1 = 0;
      
      fuerza_empresa = 0;
      con_acceso_directo = 1;	  

      while (llama_menu(nmen,pro,con1) == 0) 
	  {
         opcion_ej(pro,codigo);
         con1 = 1;
         fuerza_empresa = 0;
      }
	  
      menua[punto][0]= 0;
      punto--;
      
      
      es_la_doble_ = _es_la_doble_;
      va_con_doble_ = _va_con_doble_;
      if (es_la_doble_ && va_con_doble_)
         caracter_de_fecha = '/';
      else
         caracter_de_fecha = '.'; 
      strcpy(nomapli_doble,_nomapli_doble);
      strcpy(empresa_doble,_empresa_doble);
      
      
      if (strcmp(fiempre,"NULL") == 0)
         break;
      else {
         if (mpfi[48] != NULL)
         {
            free_defpri(mpfi[48]);			
            libera_rela(48);
         }
         mpfi[48] = NULL;
         menua[punto][0]= 0;
         punto--;
      }
      
      break;
   } /* while verdad */
   
   if (mpfi[48] != NULL)
   {
      free_defpri(mpfi[48]);
      libera_rela(48);
   }
   mpfi[48] = NULL;
   
   return(0);
}


#endif

int compon_menua_(char *px)
{
   int kki;
   char *or = px;
   memcpy(px,&punto,sizeof(short));
   px += sizeof(short);
   for (kki = 0;kki < 25;kki++)
   {
      memcpy(px,menua[kki],MAXMENUA+1);
      px += (MAXMENUA+1);
   }
   return((int)(px-or));
}


montapar(tmp)
char *tmp;
{
   int i;
   char m[6];
   sprintf(tmp,"$$%03d%s#",user_who,rw_user);
   for (i=0;i<=punto;i++) 
   {
      strcat(tmp,menua[i]);
      strcat(tmp,"%");
   }
   strcat(tmp,"$");
   strcat(tmp,prog_rw);
   strcat(tmp,"$");
   strcat(tmp,par1);
   strcat(tmp,"$");
   strcat(tmp,par2);
}

void montapar_x(char *exe,char *tmp,char *user,char *pass,int opcion)
{
   int i,j;
   char rw_dir[51];
   char rw_id[25];
   /*char tmpx[512];*/
   
   strcpy(exe,prog_rw);

#ifdef FGVWIN
   {
	  char tmp1[512],tmp2[512];	  
	  char *pp;
	  int k,j;
	  k = opcion;	  

	  tmp1[0] = '(';
	  tmp1[1] = 0;
	  if (k && wwmenus)
	  {
		  j = k / 100;
		  k %= 100;
		  k--;

		  strcpy(tmp2,wwmenus[j][k]);
	  }
	  else
	  {
		  strcpy(tmp2,quita_path(fichemenu));
		  strcat(tmp2,".men");		  
	  }
	  quitab(tmp2);
	  quitabp(tmp2);	  

	  if (agi_aplicacion)
	  {
		  strcat(tmp1,agi_aplicacion);
	  }
	  
	  if (agi_empresa)
	  {
		  strcat(tmp1," ");
		  strcat(tmp1,agi_empresa);
		  if (agi_codi_empre)
		  {
			 strcat(tmp1," ");
			 strcat(tmp1,agi_codi_empre);
		  }
	  }
	  quitab(tmp1);
	  strcat(tmp1,")");
      strcat(tmp1,tmp2);

	  pp = exe;
	  pp += (strlen(exe)+1);
	  *pp++ = 7;
	  *pp++ = 7;
	  strcpy(pp,tmp1);
   }   
#endif

   tmp[0] = 0;
   if (par1[0] && par1[0] != '/')
      strcat(tmp,par1);
   if (par2[0])
			{
      if (tmp[0]) strcat(tmp," ");
      strcat(tmp,par2);
			}
   
   if (seted_dir)
			{  
      strcpy(rw_dir+1,rw_directorio(""));
      i = strlen(rw_dir+1);
      if (i)
      {     
         rw_dir[i] = 0;
         if (rw_dir[1])
         {
            rw_dir[0] = '@';
            if (tmp[0]) strcat(tmp," ");
            strcat(tmp,rw_dir);	   
         }
      }
			}
   if (strlen(server_request))
			{
      if (server_request[2] == '-') server_request[2] = 'S';
      if (server_request[2] == '/') server_request[2] = 'C';
      cambia_s_req(server_request,0);
      if (tmp[0]) strcat(tmp," ");
      strcat(tmp,server_request);	     
			}
   
   if (strlen(other_server))
			{    
      if (tmp[0]) strcat(tmp," ");
      strcat(tmp,other_server);	     
			}
   
         /*
         if (server_id)
         {
         sprintf(rw_id,"@#%d",server_id);
         if (tmp[0]) strcat(tmp," ");
         strcat(tmp,rw_dir);	   
         }
   */
   
   if (segundo_dir[0])
			{  
      if (tmp[0]) strcat(tmp," ");
      strcat(tmp,segundo_dir);	   
			}
   
   if (no_instala[0])
			{  
      if (tmp[0]) strcat(tmp," ");
      strcat(tmp,no_instala);	   
			}
   
   if (tmp[0]) strcat(tmp," ");
   sprintf(tmp+strlen(tmp),"$$$%s,%s#",user,pass);
   for (i=0;i<punto;i++) 
			{
      strcat(tmp,menua[i]);
      strcat(tmp,"%");
			}
   if (opcion)
   {
	   sprintf(tmp+strlen(tmp),"%d",opcion);
	   strcat(tmp,"%XXX%");   
   }
   else
   {
	   strcat(tmp,"XXX%");
   }
   /*&& wmenu && (punto/100) < (punto%100) <= nmenus && wmenu[punto/100]->m_nop*/
   if (empresa[0])
   {
	   i = strlen(tmp);
	   tmp[i] = 3;
	   /*
	   wmenu[punto/100]->m_op[(punto%100)-1]->o_nombre
	   sprintf(tmpx,"%s %c %s",nommenu,1,agi_empresa);
	   strcpy(tmp+i+1,tmpx);
	   */
	   strcpy(tmp+i+1,empresa);
   }
}


ejecutae_xxx(char *mandato_,int modo)
{
   char mandato[256];
   char tmp[256];
   char rw_dir[51];
   char rw_id[25];
   char termin[2];
   char *termi;
   int i,ii,n=0,j = strlen(mandato_),k,haypars = 0;
   static char *args[20];
   char exte[256];
   char otropar[50];
#ifdef RWSERVER
   int es_externa = 0;
#endif
#ifdef ESDOS
#ifdef RWSERVER
   STARTUPINFO stinfo;
   PROCESS_INFORMATION proi;
   char tmp_x[1024];
#endif
#endif
   
   
   /*
   YA SE PUEDE!!!!!!! SIEMPRE SE HACEN SYSTEMS!!!!!!
   
     if (control_exec)
     {
     aviso(0,"Opcion no ejecutable desde Proceso o Funcion");
     return(-1);
     }
   */
   
   
   if (!modo)
	{
#ifdef FGVBSD
      sprintf(otropar," '@$$%03d$%s'",user_who,rw_user);
#else
      sprintf(otropar," @$$%03d$%s",user_who,rw_user);
#endif

#ifdef RWXARXA
      if (!en_conexion_terminal)
      {
#endif
         sal_user(0); 
#ifdef RWXARXA
      }
#ifdef RWSERVER
      es_externa = en_conexion_externa;
      if (en_conexion_externa)
      {
         if (ejecutae_terminal())
         {
            return(-1);
         }
      }
#endif
#endif
			}
   else
			{
      sprintf(otropar,"$$%03d%s#",user_who,rw_user);
			}
   
   mandato[0] = 0;
   for (i = 0;i < j;i++)
			{
      if ((!i || mandato_[i-1] == ' ') && mandato_[i] == ':' && mandato_[i+1] == '/')
      {
         strcpy(mandato+n,rw_directorio(""));
         i++;
         n = strlen(mandato);
         continue;
      }
      if (mandato_[i] == ' ' && !haypars)
      {
         haypars = 1;
         ii = n;
         if (ii)
         {
            mandato[n] = 0;
            if (strcmp(quita_path(mandato),"instalar"))
            {
               k = strlen(quita_path(mandato));
#ifdef FGVDOS
               if (k > 6)
               {
                  ii -= (k-6);
                  
               }
               strcpy(mandato+ii,"nt");
               ii += 2;
#else
#ifdef FGVBSD
               if (k > 6)
               {
                  ii -= (k-6);
               }
               strcpy(mandato+ii,"bx");
               ii += 2;
#else
               if (k > 6)
               {
                  ii -= (k-6);
               }
               strcpy(mandato+ii,"sx");
               ii += 2;
#endif
#endif
               n = ii;
            }
         }
         
      }
      mandato[n] = mandato_[i];
      n++;
			}
   mandato[n] = 0;
   if (!haypars)
			{
      ii = n;
      if (ii)
      {
         if (strcmp(quita_path(mandato),"instalar"))
         {
            k = strlen(quita_path(mandato));
#ifdef FGVDOS
            if (k > 6)
            {
               ii -= (k-6);
            }
            strcpy(mandato+ii,"nt");
#else
#ifdef FGVBSD
            if (k > 6)
            {
               ii -= (k-6);
            }
            strcpy(mandato+ii,"bx");
#else
            if (k > 6)
            {
               ii -= (k-6);
            }
            strcpy(mandato+ii,"sx");
#endif
#endif
         }
      }
			}
   
   n = 0;
   
   
   j=strlen(mandato);
   
   /*montapar(tmp);*/
   
   for (i=0;i<j;i++) 
			{
      if (*(mandato+i)==' ') 
      {
         *(mandato+i) = '\0';
         i++;
         while (*(mandato+i)==' ' && i<j) i++;
         if (i>=j) break;
         n++;
         args[n]=(mandato+i);
      }
			}
   
   termin[0] =  '~';
   termin[1] = '\0';
   
   if (LEERTERMINAL != 0)
      termi = termin;
   else
      termi = NULL;
   
   args[0]=mandato;
   n++;
   if (seted_dir)
			{
      strcpy(rw_dir+1,rw_directorio(""));
      i = strlen(rw_dir+1);
      if (i)
      {     
         rw_dir[i] = 0;
         if (rw_dir[1])
         {
            rw_dir[0] = '@';
            args[n] = rw_dir;
            n++;
         }
      }
			}
   if (strlen(server_request))
			{
      cambia_s_req(server_request,0);  
      args[n]=server_request;
      n++;
			}
   
   if (strlen(other_server))
			{  
      args[n]=other_server;
      n++;
			}
   
   if (server_id)
			{
      sprintf(rw_id,"@#%d",server_id);
      args[n]=rw_id;
      n++;
			}
   
   if (segundo_dir[0])
			{
      args[n]=segundo_dir;
      n++;
			}
   
   if (no_instala[0])
			{
      args[n]=no_instala;
      n++;
			}
   
   
   if (modo)
	{
      args[n]=otropar;
      n++;
      args[n]=NULL;
      final_agisys();
      fin_sig();
      Terfinal();
#ifdef FGVDOS
      {
         STARTUPINFO stinfo;
         PROCESS_INFORMATION proi;
         char tmpx[1024];
         
         strcpy(tmpx,mandato);
		 if (!strstr(tmpx,".exe") || !strstr(tmpx,".EXE"))
		 {
			 strcat(tmpx,".exe");
		 }
         for (i = 1;args[i];i++)
         {
            strcat(tmpx," ");
            strcat(tmpx,args[i]);
         }
         memset(&stinfo,0,sizeof(stinfo));
         stinfo.cb = sizeof(stinfo);
         stinfo.wShowWindow = SW_SHOWMINIMIZED;

         if (CreateProcess(NULL,tmpx,NULL,NULL,FALSE,NORMAL_PRIORITY_CLASS | CREATE_NEW_PROCESS_GROUP | DETACHED_PROCESS,NULL,NULL,&stinfo,&proi) != FALSE)
         {			 
			 return -10;
            /*exit(0);*/
         }
		 strcatd(tmpx,"start ");
		 system(tmpx);
         exit(2);
      }
#endif
      execv(mandato,args);
      exit(2);
	}
   
   args[n]=NULL;
   
   filtroma(mandato);
			{
            char tmp[5000];
            int tti;
            strcpy(tmp,mandato);
            for (tti = 1;args[tti];tti++)
            {
               strcat(tmp," ");
               strcat(tmp,args[tti]);
            }
#ifdef RWSERVER
            en_conexion_externa = es_externa; 
#endif 
            strcat(tmp,otropar); 
            final_agisys();
#ifdef WIN32
			Sleep(500);
#else
			usleep(500000);
#endif			
            agi_system(tmp);
            inicio_agisys();
            
#ifdef RWSERVER

			/*MessageBox(NULL,"VOY","VOY",MB_OK);*/
#ifdef WIN32
			Sleep(500);
#else
			usleep(500000);
#endif
            en_conexion_externa = 0;
            if (establece_conexion(0) == 2)
            {
               fin_sig();
               Terfinal();
               exit(2);
            }
			
#endif 
#ifdef RWXARXA
            if (!en_conexion_terminal)
            {
#endif 
               entra_user(0,NULL,"",NULL); 
#ifdef RWXARXA
            }
#endif
            if (server_request[2] == '-') server_request[2] = 'S';
            if (server_request[2] == '/') server_request[2] = 'C';
         }
         
         return(0);
}


ejecutae(mandato_)
{
   return(ejecutae_xxx(mandato_,0));
}

llenamenua()
{
   int i,j=0,m,k,n;
   
   for (i=0;i<25;i++)
      menua[i][0]=0;
   if (salvapar == (char *)0)
   {   
      return(-1);
   }
   if (salvapar[2] == '$')
   {
      m = 3;
      while(salvapar[m] && salvapar[m] != '#') m++;
      m -= 5;
   }
   else
   {
      user_who = (int)vals(salvapar,2,3);
      m = 0;
      while(*(salvapar+5+m) != '#' && *(salvapar+5+m)) {
         rw_user[m] = *(salvapar+5+m);
         m++;
      }
	  if (m > 10)
		  rw_user[10] = '\0';
	  else
		  rw_user[m] = '\0';
      rw_usuario = rw_user;
	  rw_modulo = modulo;
   }
   
   
   if (*(salvapar+5+m) == '#') m++;
   
   k = 0;
   for (i=0;*(salvapar+m+5+i)!='$'
      && *(salvapar+i+m+5)!='\0';i++) 
   {
      if (*(salvapar+m+5+i) == '%') 
      {
         memcpy(menua[j],salvapar+m+5+i-k,(k < MAXMENUA) ? k : MAXMENUA);
         menua[j][(k < MAXMENUA) ? k : MAXMENUA] = 0;
         j++;
         k = 0;
      }
      else   
         k++;
   }
   return(0);
}



