
/**********************************************************************/
/**********************************************************************/
/*                    DIAGRAM9                                    */
/**********************************************************************/
/**********************************************************************/

#include <fgvsys.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <errno.h>

#include <rwmalloc.h>
#include <rtermin.h>
#include <rentra.h>
#include <rfecha.h>
#define _NUEVODEF
#include <rdef.h>       /* ya incluye el isam.h */
#include <rmante.h>
#include <rcalcul.h>



#ifdef ESUNIX
extern char *ttyname();
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
#ifdef RWSERVER
extern char remoto_dirbass[51];
extern int en_conexion_externa;
extern int remoto_es_sistema_grafico;
#endif
extern int en_conexion_terminal;
#endif

extern int Pseudo10;

extern char rw_version[256];

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
extern int apli_aborto; /* flag para abortar la aplicacion (QUIT) */

extern int user_who;
extern char rw_user[11];

extern int subnumero_de_usuario;

char nom_inf[256];
char numerofax[300];

extern char bufpan[PANBUFSIZE];   

extern char *w_plantilla_principal;

extern int n_np;

extern int fgv_crear_plantilla_doc;

extern int (*asigna_decimales)();

extern int fgv_ver_euro;

extern int campo_euro_localizado;

static char ult_fispool[51]  = ""; 
static char ult_ejspool[51] = "";
static char ult_fispo[51]    = "";

static char _ds_absoluta_impresora[512];
static char _ds_default_spool[128] = "";
extern char *ds_absoluta_impresora;

extern char _fispool[51];
extern char _ejspool[51];
extern int atento_imp;
extern int es_esclava;

extern FILE *_imp;
extern char *aginfo;
extern int LEERTERMINAL;
extern char nmen0[551];

extern int pasa_a_int(double);

extern char *quita_path();

extern int (*externo_dll)();
extern char *el_dll;

extern long trata_memo();
extern char *cambia_path_fich();

extern char *lee_linea();

extern char no_imprime[3];

extern int _tipopermiso;

static char linea_vacia[2] = "";

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
extern int ultimo_ipfi;

struct linea_inf {
   int l_ncamp ;        /* numero de campos o variables a imprimir   */
   int l_repe ;         /* numero d eveces que se repetira la linea  */
   int *l_camp ;        /* n.de los campos     (<0 indica variable)  */
   int *l_fich ;        /* n. de fichero de cada campo               */
   char **l_usi ;       /* usings tipo 'printf' de los campos        */
   int *l_p ;           /* flags que indican formatos numericos      */
   int *l_s ;           /* separacion con el campo anterior          */
   int *l_cond;         /* condicion para imprimir el campo          */
   char **l_form ;      /* formato tipo printf asociado              */
   int *l_cal ;         /* calculo asociado a la linea               */
   char *l__lin ;       /* si no hay campos, linea impresa           */
   char *l__usi ;       /* usi de la linea impresa.                  */
   int l_lcond; 
   long w_ri;
   long w_rf;
   int *w_indice;
   char **w_datos;   
   int *l_pfi;
};
#define LINEA_INF struct linea_inf
#define MAXBLOQ 20

struct bloque_inf 
{
   int totalbloque;               /* tama¤o del bloque en lineas     */
   int b_nlin ;                   /* numero de lineas del bloque     */
   int b_interlin;                /* interlineado                    */
   LINEA_INF **b_lin;             /* matriz de punteros a lineas     */
   int i_word;
   int i_exacto;
   long i_ri;
   long i_rf;
   FI_DEF *pfi;
};

#define BLOQUE_INF struct bloque_inf

struct  subbloque_inf 
{
   BLOQUE_INF *s_bloq;
   int s_cond;                  /* condicion de impresion de subbloque */
   int cuando;                  /* 0 == antes  -1 == despues           */
};
#define SUBBLOQUE_INF struct subbloque_inf

struct cuerpo_inf 
{
   BLOQUE_INF *c_bloq;             /* bloque basico de lineas              */
   int c_nsub ;                    /* numero de subbloques a imprimir      */
   SUBBLOQUE_INF *c_sub[MAXBLOQ];  /* matriz de punteros a subbloques      */
   int totalcuerpo;                /* numero lineas del cuerpo             */
};
#define CUERPO_INF struct cuerpo_inf

struct informe 
{
   char i_fich[MAXPATH];       /* nombre del fichero de informe        */
   char i_nom[161];            /* nombre del informe                   */
   int i_nlin;                 /* numero de lineas por pagina          */
   BLOQUE_INF *i_cabecera;     /* cabecera del informe                 */
   CUERPO_INF *i_cuerpo;       /* cuerpo del informe                   */
   BLOQUE_INF *i_pie;          /* pie del informe                      */
   BLOQUE_INF *i_salto;        /* bloque de salto de pagina            */
   char si_juntopie;           /* S seguido al cuerpo F al final       */
   int i_especial;
   int i_rayado;
   int i_inirayado;
   int i_tipo_informe; /* 0 = std , 1 = word , 2 = auto-rpt */
   int w_handle1;   
   char *i_buffer_word;
   int i_w_para_salto;
   FI_DEF *pfi_cabecera;
   FI_DEF *pfi_cuerpo;
   int ipfifax;
   int campofax; 
   int ipfimail;
   int campomail;
};
#define INFORME struct informe


struct c_ficheros {
   int numero;
   int indice;
};
#define FF struct c_ficheros

struct c_memorias {
   char nombre[50];
   int numero;
   int externo;
};
#define MM struct c_memorias

FF *fichs;
static int totalfichs = 0;
MM **memos;
static int totalmemos = 0;
static int tope_memos = 200;

T_MEMORIA *_memo; /* indices alternativos para datos informes */
static int _nmemo = 0;
static int tope_memo = 200;
OPERA *_oper;
static int _noper = 0;
static int tope_oper = 200;
CONDIC *_cond;
static int _ncond = 0;
static int tope_cond = 50;


char *mi_remoto_dirbass();
/* ATENCION -> coma_linea sin quitab */
#define coma_linea(a,b) comas_linea(a,b,0,(int *)0)

extern unsigned char nombredev[81];
/*************************************/

extern int i_nlineas;
extern int i_anchocarro;

/* informes */

static int vez ;

extern int (*asigna_euro)();

#ifdef FGV32

extern BOOL fIsTNT;

#endif



static double pfi_cabeceras = 0.0;
static int n_pseudo_lineas = 0;
static char **pseudo_lineas = NULL;

struct linea_word 
{	
	long ri;
	long rf;
	int *cc;
};


void pasa_inf_a_doc(char *inf);

static int n_lineas_word = 0;
static struct linea_word *lineas_word = NULL;


#ifndef DSMINIBASICO

static char *lee_linea_i(char *text,int nt,FILE *pfich)
{	
	if (pfich)
	{
		return lee_linea(text,nt,pfich);
	}
	if (!n_pseudo_lineas) return NULL;
    n_pseudo_lineas--;
	strcpy(text,pseudo_lineas[n_pseudo_lineas]);
	free(pseudo_lineas[n_pseudo_lineas]);
	if (!n_pseudo_lineas)
	{
		free((char *)pseudo_lineas);
		pseudo_lineas = NULL;
	}

	return text;
}

/*  =========================================================================
busca_fin(s,c)
========================================================================*/


int busca_fin(s,c)
char *s;
char c;
{
   int i = 0,tip = 0;
   char m,n;
   
   if (c == '\0') {
      m = '9';n = '0';tip = 1;
   }
   else
      if (c == '\1') {
         m = '9';n = '0';
      }
      else {
         m = c;n = c;
      }
      
      if (tip == 0)
         while(*(s + i) != '\0' && (*(s + i) > m || *(s + i) < n) ) i++;
         else
            while(*(s + i) != '\0' && *(s + i) <= m && *(s + i) >= n ) i++;
            return(i);
}

/*  =========================================================================
busca_con(s)
========================================================================*/

int busca_con(s)
char *s;
{
   int i = 0;
   
   while(*(s + i) != '\0' && *(s + i) != '!' && *(s + i) != '>' &&
      *(s + i) != '<' && *(s + i) != '=' && *(s + i) != ']' &&
      *(s + i) != '[' ) i++;
   if (*(s + i) == '\0') i = 0;
   return(i);
}

/*  =========================================================================
busca_ope(s)
========================================================================*/

int busca_ope(s)
char *s;
{
			int i = 0;
         
         while(*(s + i) != '\0' && *(s + i) != '!' && *(s + i) != '+' &&
            *(s + i) != '-' && *(s + i) != '*' && *(s + i) != '>' &&
            *(s + i) != '<' && *(s + i) != '/' && *(s + i) != '%' &&
            *(s + i) != '$' && *(s + i) != '~') i++;
         if (*(s + i) == '\0') i = 0;
         return(i);
}



int cambia_fichero(f)
int *f;
{
   int i;
   
   for (i = 0;i < totalfichs;i++) {
      if (*f == fichs[i].numero) {
         *f = fichs[i].indice;
         return(0);
						}
   }
   return(-1);
}


static char *nombre_impresora()
{
   static char _nombre_impresora[257];

   _nombre_impresora[0] = 'A';
   if (_imp == 0x00000003L && nombredev[0] == 2 && nombredev[1] == 0)
   {
	   if (nombredev[2] == '!' && nombredev[3] == '!')
	   {
		   strcpy(_nombre_impresora+1,"Fax");
		   strcat(_nombre_impresora+1,";");
		   strcat(_nombre_impresora+1,nombredev+4);
	   }
	   else if (nombredev[2] == '$' && nombredev[3] == '$')
	   {
		   strcpy(_nombre_impresora+1,"PDF");
		   strcat(_nombre_impresora+1,";");
		   strcat(_nombre_impresora+1,nombredev+4);
	   }
	   else if (nombredev[2] == '|' && nombredev[3] == '|')
	   {
		   strcpy(_nombre_impresora+1,"Correo");
		   strcat(_nombre_impresora+1,";");
		   strcat(_nombre_impresora+1,nombredev+4);
	   }
	   else
	   {
		   strcpy(_nombre_impresora+1,"CrystalReport");		   
		   if (nombredev[2])
		   {
			   strcat(_nombre_impresora+1,";");
			   strcat(_nombre_impresora+1,nombredev+2);
		   }		   
	   }
   }
   else
	   strcpy(_nombre_impresora+1,nombredev);

   return _nombre_impresora;
}


/*  ========================================================================
ind_rel(fichero)
=======================================================================*/

int ind_rel(fichero)
char *fichero;
{
   register int j;
   char tmpx[512];
   FILE *file;
   
   for (j=0;j<nfi;j++) 
   {                  
 				  if (j == 48) 
					  j++;
  				  if (j >= nfi)
					  break;

                  if (!mpfi[j]) continue;
                  if (strcmp(quita_path(v_mpfi(j)->f_PathDef),quita_path(fichero)) == 0) return(j);
   }
   
   for (j=0;j<nfi;j++) 
   {                  
				  if (j == 48) j++;
				  if (j >= nfi)
					  break;
                  if (!mpfi[j]) continue;
                  if (strcmp(quita_path(v_mpfi(j)->f_fichero),quita_path(fichero)) == 0) return(j);
   }

   strcpy(tmpx,dirdef);
   strcat(tmpx,quita_path(fichero));
   strcat(tmpx,".ref");
   file = fopen(tmpx,"r");
   if (file)
   {
       fclose(file);
       return -1;
   }  
   

   for (j=0;j<nfi;j++) 
   {                  
 				  if (j == 48) 
					  j++;
				  if (j >= nfi)
					  break;
				  if (!mpfi[j]) 
				  {
					  if (!v_existe_def(j))
						  continue;
				  }
                  if (strcmp(quita_path(v_mpfi(j)->f_PathDef),quita_path(fichero)) == 0) 
					  return(j);
   }
   
   for (j=0;j<nfi;j++) 
   {                  
				  if (j == 48) 
					  j++;
				  if (j >= nfi)
					  break;
				  if (!mpfi[j]) 
				  {
					  if (!v_existe_def(j))
						  continue;
				  }
                  if (strcmp(quita_path(v_mpfi(j)->f_fichero),quita_path(fichero)) == 0) 
					  return(j);
   }
   return(-1);
}

/*  =========================================================================
pon_varia(nombre,valor)
========================================================================*/
int pon_varia(nombre,valor)
char *nombre;
char *valor;
{
   int numero;
   int in = totalmemos;
   int ll;
   int externo = 0;
   
   if (totalmemos  >= tope_memo) 
   {
      tope_memo += 200;
      _memo = (T_MEMORIA *)realloc(_memo,tope_memo *sizeof(T_MEMORIA));
   }
   
   if (nombre[0] == '&') {externo = -1;nombre++;}
   if (strlen(nombre) == 0) return(0);
   memos[totalmemos] = (MM *)malloc(sizeof(MM));
   
   memos[totalmemos]->externo = -1;
   if (externo == -1) {
      externo = 0;
      for (ll = memoplin+maxpila*2;ll < nm_ex;ll++)
         if (strcmp(nombre,m_ex[ll].nombre) == 0) {
            memos[totalmemos]->externo = ll; /* m_ex[ll].indice; */
            externo = -1;
            break;
									}
   }
   
   if (valor[0] == 'A' && externo == 0) {
      _memo[_nmemo].m_tipo = CHARTIPO;
      _memo[_nmemo].mv.valc = strmalloc(strlen(valor+1)+1);
      strcpy(_memo[_nmemo].mv.valc,valor+1);
   }
   else
      if ( valor[0] == '@' && externo == 0) 
	  {
         _memo[_nmemo].m_tipo = FECHATIPO;
         ll = strlen(valor+1)+1;if (ll < 11) {ll = 11;strcpy(valor+1,"00.00.0000");}
         _memo[_nmemo].mv.valc = strmalloc(ll);
         strcpy(_memo[_nmemo].mv.valc,valor+1);
      }
      else 
	  {
         _memo[_nmemo].m_tipo = DOUBLETIPO;
         _memo[_nmemo].mv.vald = rwatof(valor);
      }
      memos[totalmemos]->numero = _nmemo;
      strcpy(memos[totalmemos]->nombre,nombre);
      _nmemo++;
      totalmemos++;
      return(in);
}

/*=========================================================================
busca_var(nom)
========================================================================*/

int busca_var(nom)
char *nom;
{
   int i;
   
   for (i = 0;i < totalmemos;i++) {
      if (strcmp(nom,memos[i]->nombre) == 0)
									return(memos[i]->numero);
   }
   i = pon_varia(nom,nom);
   return(memos[i]->numero);
}

/*  =========================================================================
devuelve_campo(nom,f,c)
========================================================================*/

int devuelve_campo(nom,f,c)
char *nom;
int *f,*c;
{
   int final;
   if (nom[0] == '#') 
   {
      final = busca_fin(nom+1,'\0');
      *f = (int)vals(nom,1,final);
      if (nom[final+1] == '#')
         *c = atoi(nom+final+2);
      else {
         nom += (final + 1);
         *c = -(busca_var(nom) + 1);
      }
   }
   else 
   {
      *c = busca_var(nom);
   }
   if (*f != F_INVALIDO)
   {      
      if (cambia_fichero(f) && (*f != 48 || !mpfi[48])) 
	  {
         *f = F_INVALIDO;
      }
      else 
	  {
         if (*c >= 0) 
		 {
            if (*c >= v_mpfi(*f)->f_ncampos) 
			{
               *f = F_INVALIDO;
			}
         }
      }
      
      if (*f == F_INVALIDO) 
	  {
         rw_error(29);
         /* printf("\n\rRuntime error: Campo ilegal en Informe\n\r");
         getchar(); */
      }
   }
   else
      *f = -1;
   return(0);
}

/*  =========================================================================
pon_opera(nom1,nom2,op,nomr)
========================================================================*/

int pon_opera(nom1,nom2,op,nomr)
char *nom1,*nom2;
char op;
char *nomr;
{
   int fichero1 = F_INVALIDO,campo1 = -1;
   int fichero2 = F_INVALIDO,campo2 = -1;
   int ficheror = F_INVALIDO,campor = -1;
   int final,in;
   
   if (strlen(nom1) > 0) devuelve_campo(nom1,&fichero1,&campo1);
   if (strlen(nom2) > 0) devuelve_campo(nom2,&fichero2,&campo2);
   if (strlen(nomr) > 0) devuelve_campo(nomr,&ficheror,&campor);
   in = pon_oper(fichero1,campo1,fichero2,campo2,ficheror,campor,op);
   return(in);
}

/*  ========================================================================
pon_opera(f1,c1,f2,c2,fr,cr,op)
=======================================================================*/

int pon_oper(f1,c1,f2,c2,fr,cr,op)
int f1,c1,f2,c2,fr,cr,op;
{
   int in = _noper;
   
   if (_noper  >= tope_oper) 
   {
      tope_oper += 200;
      _oper = (OPERA *)realloc(_noper,tope_oper *sizeof(OPERA));
   }
   
   _oper[in].fichero1 = f1;
   _oper[in].campo1 = c1;
   _oper[in].fichero2 = f2;
   _oper[in].campo2 = c2;
   _oper[in].ficheror = fr;
   _oper[in].campor = cr;
   _oper[in].operacion = op;
   _noper++;
   return(in);
}

/*  =========================================================================
pon_condicion(f1,c1,f2,c2,c,ulcon,o)
=======================================================================*/

int pon_condicion(f1,c1,f2,c2,c,ulcon,o)
int f1,c1,f2,c2;
char c;
int *ulcon;
int o;
{
   int in = _ncond;
   
   if (_ncond  >= tope_cond) {
      tope_cond += 50;
      _cond = (CONDIC *)realloc(_cond,tope_cond *sizeof(CONDIC));
   }
   
   _cond[in].fichero1 = f1;
   _cond[in].campo1 = c1;
   _cond[in].fichero2 = f2;
   _cond[in].campo2 = c2;
   _cond[in].condicion = c;
   _cond[in].sigue = -1;
   _cond[in].tipo = -1;
   _ncond++;
   if (o == 1) 
   {
      _cond[*ulcon].sigue = in;
      _cond[*ulcon].tipo = 0;
   }
   if (o == 2) 
   {
      _cond[*ulcon].sigue = in;
      _cond[*ulcon].tipo = 1;
   }
   *ulcon = in;
   return(in);
}


/*  =========================================================================
com_condicion(pr)
========================================================================*/

int com_condicion(pr)
char *pr;
{
   int i,o = 0;
   char c;
   char *p;
   char nom1[81];
   char nom2[81];
   int final;
   int f1,c1,f2,c2,indice,pindice = -1;
   static int ulcon = -1;
   
   while (-1) {
      i = busca_con(pr);
      if (i == 0) {
         rw_error(32);
         /* printf("\n\rRuntime error: Condicion ilegal en Informe\n\r");
         getchar(); */
         return(-1);
      }
      c = *(pr + i);
      p = pr + i + 1;
      f1 = F_INVALIDO;c1 = -1;f2 = F_INVALIDO;c2 = -1;
      memcpy(nom1,pr,i);
      nom1[i] = '\0';
      final = busca_fin(p,'|');
      memcpy(nom2,p,final);
      nom2[final] = '\0';
      quita_blancos(nom1);
      quita_blancos(nom2);
      devuelve_campo(nom1,&f1,&c1);
      devuelve_campo(nom2,&f2,&c2);
      if (f1 == F_INVALIDO || f2 == F_INVALIDO)
         return(-1);
      indice = pon_condicion(f1,c1,f2,c2,c,&ulcon,o);
      p += final;
      if (pindice == -1) pindice = indice;
      if (*p == '\0') break;
      pr = p + 1;
      if (*pr == 'Y') {
         o = 1;
      }
      else {
         o = 2;
      }
      pr += 1;
   }
   return(pindice);
}

/*  =========================================================================
    com_opera(pr)
    ========================================================================*/

int com_opera(pr)
char *pr;
{
   int i;
   char *p;
   char nomr[81];
   char nom1[81];
   char nom2[81];
   int final,indice;
   char op;
   
   quita_blancos(pr);
   i = busca_fin(pr,'=');
   if (*(pr+i) != '=') {
      strcpy(nomr,pr);
      indice = -1;
      for (i = 0;i < np_ex;i++)
         if (strcmp(nomr,p_ex[i].nombre) == 0) {
            indice = -2 - p_ex[i].indice;
            break;
         }
         if (indice == -1) {
            rw_error(33);
            /* printf("\n\rRuntime error: Calculo ilegal en Informe\n\r");
            getchar(); */
         }
         return(indice);
   }
   p = pr + i + 1;
   memcpy(nomr,pr,i);
   nomr[i] = '\0';
   final = busca_ope(p);
   if (*(p+final) == '~') {
      op = ORESET;
      strcpy(nom1,"NULL");
      strcpy(nom2,"NULL");
   }
   else {
      if (final == 0) {
         op = '=';
         final = strlen(p);
         nom2[0] ='\0';
         strcpy(nom1,p);
      }
      else {
         op = *(p+final);
         memcpy(nom1,p,final);
         nom1[final] = '\0';
         p += (final+1);
         strcpy(nom2,p);
      }
   }
   indice = pon_opera(nom1,nom2,op,nomr);
   
   return(indice);
}

static pon_comas(s)
unsigned char *s;
{
   while(*s) {
      if (*s == 255) *s = ',';
      s++;
   }
}


enlaza_externas(m)
int m;
{
   int i;
   int l;
   int c;
   
   for (i = 0;i < totalmemos;i++)
      if (memos[i]->externo > -1) {
         l = memos[i]->externo;
         c = memos[i]->numero;
         if (m == 0) {
            memcpy(&(_memo[c].mv),&(m_ex[l].mv),sizeof(M_VAL1));
            _memo[c].m_tipo = m_ex[l].m_tipo;
         }
         if (m == 1) {
            memcpy(&(m_ex[l].mv),&(_memo[c].mv),sizeof(M_VAL1));
            _memo[c].m_tipo = DOUBLETIPO;
         }
      }
      return(0);
}

calcula_i(in,m)
int in,m;
{
   int i1,i2;
   char *p1,*p2;
   int r;
   int op;
   
   /* cambio tablas de memorias - operaciones */
   if (in < 0) {
      i1 = (-in) - 2;
      r = p_ejecuta(i1,O_NOR,0,10);
   }
   else 
   {
      enlaza_externas(0);
      i1 = nmemo;
      p1 = (char *)memo;
      i2 = noper;
      p2 = (char *)oper;
      nmemo = _nmemo;
      memo = _memo;
      noper = _noper;
      oper = _oper;
      
      op = programa_activo;
      programa_activo = nen_pr;
      
      r = calcula(in,m);
      
      programa_activo = op;
      
      
      nmemo = i1;
      memo = (T_MEMORIA *)p1;
      noper = i2;
      oper = (OPERA *)p2;
      enlaza_externas(1);
   }
   
   return(r);
}

condicion_i(in)
int in;
{
   int i1,i2;
   char *p1,*p2;
   int r;
   int op;
   
   enlaza_externas(0);
   i1 = nmemo;
   p1 = (char *)memo;
   i2 = ncond;
   p2 = (char *)cond;
   nmemo = _nmemo;
   memo = _memo;
   ncond = _ncond;
   cond = _cond;
   
   op = programa_activo;
   programa_activo = nen_pr;
   r = condicion(in);
   programa_activo = op;
   
   nmemo = i1;
   memo = (T_MEMORIA *)p1;
   ncond = i2;
   cond = (CONDIC *)p2;
   enlaza_externas(1);
   
   
   return(r);
}

/*  =========================================================================
leo_bloque(pfich)   lee un bloque del fichero abierto pfich.
devuelve un puntero a BLOQUE_INF.
========================================================================*/

BLOQUE_INF *leo_bloque(pfich)
FILE *pfich;
{
   int i,n,k,f,t;
   char text2[256];
   BLOQUE_INF *pbl;
   char texto[256];
   int calcs[30];
   LINEA_INF *susti;
   int totalbloque;
   
   pbl = (BLOQUE_INF *)malloc(sizeof(BLOQUE_INF));
   memset((char *)pbl,0,sizeof(BLOQUE_INF));
   if (pfich)
   {
		pbl->i_word = -1;
   }
   else
   {
	    pbl->i_word = 0;
   }
   pbl->i_exacto = 0;
   lee_linea_i(texto,255,pfich);
   pbl->b_nlin = atoi(texto+coma_linea(texto,0));
   pbl->b_lin = (LINEA_INF **)malloc(sizeof(LINEA_INF *) * pbl->b_nlin);
   pbl->b_interlin = atoi(texto+coma_linea(texto,1));
   totalbloque = atoi(texto+coma_linea(texto,1));
   for (k=0;k<pbl->b_nlin;k++)
   {
      susti = (LINEA_INF *)malloc(sizeof(LINEA_INF));
	  memset((char *)susti,0,sizeof(LINEA_INF));

	  if (pbl->i_word >= 0)
	  {
		  susti->w_ri = lineas_word[k].ri;
		  susti->w_rf = lineas_word[k].rf;
	  }   

      pbl->b_lin[k] = susti;
      lee_linea_i(texto,255,pfich);
      i = atoi(texto + coma_linea(texto,0));
      susti->l_ncamp = i;
      susti->l_repe = atoi(texto + coma_linea(texto,1));
      susti->l_camp = (int *)malloc(i*sizeof(int));
	  if (pbl->i_word >= 0)
	  {
		  susti->w_indice = (int *)malloc(i*sizeof(int));
		  susti->w_datos = (char **)malloc((i+1)*sizeof(char *));
		  susti->w_datos[0] = NULL;
	  }
      susti->l_fich = (int *)malloc(i*sizeof(int));
      susti->l_p =    (int *)malloc(i*sizeof(int));
      susti->l_s =    (int *)malloc(i*sizeof(int));
      susti->l_cond = (int *)malloc(i*sizeof(int));
      susti->l_usi =  (char **)malloc(i*sizeof(char *));
      susti->l_form = (char **)malloc(i*sizeof(char *));
      for(i=0;i<susti->l_ncamp;i++)
      {
		 if (pbl->i_word >= 0)
		 {
			 susti->w_indice[i] = lineas_word[k].cc[i];
			 susti->w_datos[i] = malloc(500);
			 susti->w_datos[i][0] = 0;
			 susti->w_datos[i+1] = NULL;
		 }
         lee_linea_i(texto,255,pfich);      
         coma_linea(texto,0);
         quita_blancos(texto);
         n = 0;f = 0;
         if (texto[0] == 'A') {
            f = MAXDEF+1;
         }
         if (texto[0] == 'C') {
            f = MAXDEF+2;
         }
         if (f > 0 && (texto[1] == 'C' || texto[1] == 'N' || texto[1] == 'E')) {
            if (texto[1] == 'N') f += 2;
            if (texto[1] == 'E') f += 4;
         }
         else
            if (strcmp(texto,"PAG") == 0)
               f = MAXDEF;
            else
               if (strcmp(texto,"SP") == 0)
                  f = MAXDEF+7;
               else
                  if (strcmp(texto,"OC") == 0)
                     f = MAXDEF+8;
                  else
                     if (strcmp(texto,"RESET") == 0)
                        f = MAXDEF+9;
                     else {
                        f = F_INVALIDO;n = -1;
                        devuelve_campo(texto,&f,&n);
                     }
                     susti->l_camp[i] = n;
                     susti->l_fich[i] = f;
                     strcpy(text2,texto+coma_linea(texto,1));
                     quitab(text2);
                     susti->l_usi[i] = strmalloc(strlen(text2)+1);
                     strcpy(susti->l_usi[i],text2);
                     pon_comas(susti->l_usi[i]);
                     susti->l_p[i] = atoi(texto+coma_linea(texto,1));
                     susti->l_s[i] = atoi(texto+coma_linea(texto,1));
                     strcpy(text2,texto+coma_linea(texto,1));
                     susti->l_form[i] = strmalloc(strlen(text2)+1);
                     strcpy(susti->l_form[i],text2);
                     pon_comas(susti->l_form[i]);
                     susti->l_p[i] += (atoi(texto+coma_linea(texto,1))*10);
                     strcpy(text2,texto+coma_linea(texto,1));
                     quita_blancos(text2);
                     if (strlen(text2) > 0)
                        susti->l_cond[i] =  com_condicion(text2);
                     else
                        susti->l_cond[i] = -1;
      }
      if (susti->l_ncamp == 0) 
	  {
         lee_linea_i(texto,255,pfich);
         coma_linea(texto,0);
         susti->l__usi = strmalloc(strlen(texto)+1);
         strcpy(susti->l__usi,texto);
         pon_comas(susti->l__usi);
         n = coma_linea(texto,1);
         susti->l__lin = strmalloc(strlen(texto+n)+1);
         strcpy(susti->l__lin,texto+n);
         pon_comas(susti->l__lin);
      }
      lee_linea_i(texto,255,pfich);
      n = 0;                                          /* lee linea de calculo */
      coma_linea(texto,0);
      strcpy(text2,texto);
      while (text2[0] != '\0') {
         if (!memcmp(text2,"LEE",3)) {
            t = atoi(text2+3);
            if (!cambia_fichero(&t)) {
               calcs[n] = 32000 + t;
               n++;
            }
         }
         else
            if ((calcs[n] = com_opera(text2)) != -1) n++;
            strcpy(text2,texto+coma_linea(texto,1));
      }
      susti->l_cal = (int *)malloc((n+1) * sizeof(int));
      *(susti->l_cal+n) = -1;
      while(n > 0) {
         n--;
         *(susti->l_cal+n) = calcs[n];
      }
      lee_linea_i(texto,255,pfich);
      quita_blancos(texto);
      if (strlen(texto) > 0)
         susti->l_lcond = com_condicion(texto);
      else
         susti->l_lcond = -1;

	   if (pbl->i_word >= 0)
	   {
		   if (lineas_word[k].cc)
		   {
			   free((char *)lineas_word[k].cc);
		   }
	   }
      }
        
	   if (pbl->i_word >= 0)
	   {
		   free((char *)lineas_word);
		   n_lineas_word = 0;
	   }
	    
        if (totalbloque)
        {
           pbl->totalbloque = totalbloque;
        }
        else
        {
           pbl->totalbloque = 0;
           for (i=0;i<pbl->b_nlin;i++)           /* calculo de la longitud del bloque */
              pbl->totalbloque += pbl->b_lin[i]->l_repe;
           pbl->totalbloque *= pbl->b_interlin;
        }
        return(pbl);
}


/*  =========================================================================
leo_subbloque(pfich)
=======================================================================*/

SUBBLOQUE_INF *leo_subbloque(pfich)
FILE *pfich;
{
   SUBBLOQUE_INF *psb;
   char texto[256];
   int i;
   psb = (SUBBLOQUE_INF *)malloc(sizeof(SUBBLOQUE_INF));
   if (!(psb->s_bloq = leo_bloque(pfich))) 
   {
      free((char *)psb);
      return((SUBBLOQUE_INF *)0);
   }
   lee_linea_i(texto,255,pfich);
   psb->s_cond = com_condicion(texto);
   lee_linea_i(texto,255,pfich);
   psb->cuando = atoi(texto);
   return(psb);
}


/*  =========================================================================
leo_cuerpo(pfich)
========================================================================*/

CUERPO_INF *leo_cuerpo(pfich)
FILE *pfich;
{
   char texto[256];
   int i;
   CUERPO_INF *pc;
   pc = (CUERPO_INF *)malloc(sizeof(CUERPO_INF));
   lee_linea_i(texto,255,pfich);
   pc->totalcuerpo = atoi(texto);
   if (!(pc->c_bloq = leo_bloque(pfich))) {
      free((char *)pc);
      return((CUERPO_INF *)0);
			}
   lee_linea_i(texto,255,pfich);
   pc->c_nsub = atoi(texto);
   for (i=0;i<pc->c_nsub;i++)
			{
      if (!(pc->c_sub[i] = leo_subbloque(pfich)))
      {
         pc->c_nsub = i;
         break;
      }
			}
   return(pc);
}


/*  =========================================================================
lee_ifich(pfich)
========================================================================*/


lee_ifich(pfich)
FILE *pfich;
{
   	     int nfich,i,k,f,nu;
         char text[256];
		 char tmpx[256];
		 FILE *file;
         int recur = (recur_punto < 1) ? 0 : recur_punto * MAXDEF;
         
         lee_linea_i(text,255,pfich);
         nfich = atoi(text);
         if (nfich >= 50) {
            rw_error(34);
            /* printf("\n\rRuntime error: Informe ilegal\n\r");getchar(); */
            return(-1);
         }
         for (i = 0;i < nfich;i++) 
		 {
            if (lee_linea_i(text,255,pfich) == NULL) break;
            k = busca_fin(text,'#');
            if (text[k] == '#') {
               text[k] = '\0';
               nu = atoi(text + k + 1);
               quita_blancos(text);
               do
               {
                   f = ind_rel(text);
                   if (f < 0) 
                   {
                       strcpy(tmpx,dirdef);
                       strcat(tmpx,quita_path(text));
                       strcat(tmpx,".ref");
                       file = fopen(tmpx,"r");
                       if (!file)
                       {
                           break;
                       }
                       lee_linea(text,200,file);
                       fclose(file);
                   }
                   else
                   {
                       break;
                   }
               } while(-1);

               if (f < 0) 
			   {
                  if (nfi >= MAXDEF) {printf("demasiados def en informe");pito(120);continue;}
                  if (nfi == 48) 
					  nfi++;
                  
                  strcatd(text,dirdef);
                  /* def virtualizado */
                  s_d[recur+nfi].buf = NULL;
				  s_d[recur+nfi].tam_buf = 0;
                  s_d[recur+nfi].fd = -1;
                  s_d[recur+nfi].salvadatos = NULL;
                  s_d[recur+nfi].nombre_fichero = strmalloc(strlen(text)+1);
                  s_d[recur+nfi].enlaza_pfi = strmalloc(1);
				  s_d[recur+nfi].enlaza_pfi[0] = 0;
                  strcpy(s_d[recur+nfi].nombre_fichero,text);
                  s_d[recur+nfi].cuando = 0L;
                  mpfi[nfi] = NULL;
                  mflagfi[nfi] = 0;
                  f_s(nfi,RCERRADO);
                  mpfirel[nfi] = (RELAS *)malloc(sizeof(RELAS));
                  mpfirel[nfi]->nrel = 0;
                  mpfirel[nfi]->nrelact = 0;
                  mpfirel[nfi]->nrelpin = 0;
                  mpfirel[nfi]->lineas = -1;
                  mpfirel[nfi]->clave = -1;
                  f = nfi;
                  nfi++;
               }
               if (f > -1) {
                  fichs[totalfichs].numero = nu;
                  fichs[totalfichs].indice = f;
                  totalfichs++;
               }
            }
            else 
			{
               rw_error(35);
               /* printf("\n\rRuntime error: fichero ilegal en Informe\n\r");
               getchar(); */
               return(-1);
            }
         }
         return(0);
}

/*  =========================================================================
lee_ivar(pfich)
========================================================================*/

lee_ivar(pfich)
FILE *pfich;
{
   int nfich,i,k,f,nu;
   char text[256];
   
   lee_linea_i(text,255,pfich);
   nfich = atoi(text);
   if (nfich >= tope_memos) 
   {
      tope_memos += 200;
      memos = (MM **)realloc(memos,sizeof(MM *) * tope_memos);
   }
   for (i = 0;i < nfich;i++) {
      if (lee_linea_i(text,255,pfich) == NULL) break;
      k = busca_fin(text,'=');
      if (text[k] == '=') {
         text[k] = '\0';
         quitabp(text+k+1);
         quita_blancos(text);
         pon_varia(text,text+k+1);
						}
      else {
         rw_error(37);
         /* printf("\n\rRuntime error: variable ilegal en Informe\n\r");
         getchar(); */
         return(-1);
						}
   }
   return(0);
}

#ifdef FGVWIN
static int INICIO_LIS=501;
#else
static int INICIO_LIS=101;
#endif

/*  =======================================================================
leo_inf(nomfich)  lee un .inf
======================================================================*/

static INFORME *inicializa_infor(int tipo)
{
   INFORME *pin;


   pin = (INFORME *)malloc(sizeof(INFORME));

   memset((char *)pin,0,sizeof(INFORME));

   fichs = (FF *)malloc(sizeof(FF) * 50);
   memos = (MM **)malloc(sizeof(MM *) * tope_memos);
   _memo = (T_MEMORIA *)malloc(tope_memo *sizeof(T_MEMORIA));
   _oper = (OPERA *)malloc(tope_oper *sizeof(OPERA));
   _cond = (CONDIC *)malloc(tope_cond *sizeof(CONDIC));

   pin->i_fich[0] = 0;         /* nombre del fichero de informe        */
   pin->i_nom[0]  = 0;         /* nombre del informe                   */
   pin->i_nlin = -1;           /* numero de lineas por pagina          */
   pin->i_cabecera = NULL;     /* cabecera del informe                 */
   pin->i_cuerpo = NULL;       /* cuerpo del informe                   */
   pin->i_pie = NULL;          /* pie del informe                      */
   pin->i_salto = NULL;        /* bloque de salto de pagina            */
   pin->si_juntopie = 'S';     /* S seguido al cuerpo F al final       */
   pin->i_especial = 0;
   pin->i_rayado = 0;
   pin->i_inirayado = 0;
   pin->i_tipo_informe  = tipo; 
   pin->w_handle1 = -1;
   pin->i_buffer_word = NULL;
   pin->i_w_para_salto = 0;
   pin->ipfifax = -1;
   pin->campofax = -1;   
   pin->ipfimail = -1;
   pin->campomail = -1;

   return pin;
}

static char *busca_private(char *p,int *counter)
{
   do
   {
	   while (*p == ' ') p++;
	   if (!memcmp(p,"PRIVATE",7))
	   {
		   p += 7;
		   while(*p && *p != 1 && (*p != '{' || (*p == '{' && *(p+1) != '{'))) p++;
		   if (*p == '{')
		   {
			    p += 2;
				return p;
		   }
	   }
	   while(*p && *p != 1) p++;
	   if (*p == 1) 
	   {
		   (*counter)++;
		   p++;
	   }
   } while(*p);
   return NULL;
}

static char *busca_quote(char *p,int *counter,int *eslinea)
{
   do
   {
	   while (*p == ' ') p++;
	   if (!memcmp(p,"PRIVATE",7))
	   {
		   p += 7;
		   while(*p && *p != 1 && (*p != '{' || (*p == '{' && *(p+1) != '{'))) p++;
		   if (*p == '{')
		   {
			    p += 2;
				if (!memcmp(p,"LIN",3))
				{
					*eslinea = 1;
					return p;
				}
		   }
	   } else if (!memcmp(p,"QUOTE",5))
	   {
		   p += 5;
		   while(*p && *p != 1 && (*p != '{' || (*p == '{' && *(p+1) != '{'))) p++;
		   if (*p == '{')
		   {
			    p += 2;
				return p;
		   }
	   }
	   while(*p && *p != 1) p++;
	   if (*p == 1) 
	   {
		   (*counter)++;
		   p++;
	   }
   } while(*p);
   return NULL;
}



static void pon_pseudo_lineas(char *p)
{
	int n = 2;
	char *p1 = p;	
	while (*p1)
	{
		if (*p1 == '$' && *(p1+1) == '$')
		{
			n++;
			p1++;
		}
		p1++;
	}	

    n_pseudo_lineas = n;
    pseudo_lineas = (char **)malloc(sizeof(char *) * n);
	n--;
	pseudo_lineas[n] = malloc(100);
	sprintf(pseudo_lineas[n],"%d",n);

	p1 = p;	
	while (*p1)
	{
		if (*p1 == '$' && *(p1+1) == '$')
		{
			n--;
			pseudo_lineas[n] = malloc((int)(p1-p)+1);
			memcpy(pseudo_lineas[n],p,(int)(p1-p));
			pseudo_lineas[n][(int)(p1-p)] = 0;
			p1++;
			p = p1 + 1;
		}
		p1++;
	}	
	n--;
	pseudo_lineas[n] = malloc((int)(p1-p)+1);
	memcpy(pseudo_lineas[n],p,(int)(p1-p));
	pseudo_lineas[n][(int)(p1-p)] = 0;
}


static void saca_rango_campo(char *p,long *ri,long *rf)
{
	if (ri)
		*ri = 0L;
	if (rf)
		*rf = 999999999L;
	while(*p && *p != 1 && *p != 2) p++;
	if (*p == 2)
	{
		p++;
		if (ri)
		{
			*ri = atol(p);
		}
	}
	while(*p && *p != 1 && *p != 2) p++;
	if (*p == 2)
	{
		p++;
		if (rf)
		{
			*rf = atol(p);
		}
	}
}

static char *crea_pseudo_bloque(char *p,char *final,int *numf,long *b_ri,long *b_rf)
{
	int n,pn,eslinea;
	char *op = p;
	char *p1 = p;
	char *p2,*p3;
	int numc;
	int sinc;
	char tmp[10000];
	int nlineas = 0;
	long ri,rf;

	saca_rango_campo(p,NULL,&ri);
	ri++;
	if (b_ri)
		*b_ri = ri;

	while((p = busca_private(p,numf)))
	{		
		p2 = p;
		while(*p2 && *p2 != 1 && (*p2 != '}' || (*p2 == '}' && *(p2+1) != '}'))) p2++;
		memcpy(tmp,p,(int)(p2-p));
		tmp[(int)(p2-p)] = 0;
		quita_blancos(tmp);
		if (!strcmp(tmp,final))
		{
			break;
		}
	}

	saca_rango_campo(p,&rf,NULL);
	rf--;
	if (b_rf)
	   *b_rf = rf;
	
	numc = 0;
	n_pseudo_lineas = 1;
	p1 = op;
	while (p1 && p1 < p)
	{
		sinc = 0;
		pn = 3;
		eslinea = 0;		
		while((p1 = busca_quote(p1,&numc,&eslinea)) && p1 < p)
		{
			if (eslinea)
			{
				break;
			}
			pn++;
		}
		if (pn == 3) 
		{
			pn++;
			sinc = 1;
		}

		n_pseudo_lineas += pn;
		nlineas++;
	}

	n_lineas_word = nlineas;
	lineas_word = (struct linea_word *)malloc(sizeof(struct linea_word) * n_lineas_word);

	pseudo_lineas = (char **)malloc(sizeof(char *) * n_pseudo_lineas);		
	n = n_pseudo_lineas;
	n--;
	pseudo_lineas[n] = malloc(100);
	sprintf(pseudo_lineas[n],"%d,1",nlineas);


	nlineas = 0;
	numc = 0;
	p1 = op;
	while (p1 && p1 < p)
	{
		sinc = 0;
		pn = 3;
		eslinea = 0;
		op = p1;
		while((p1 = busca_quote(p1,&numc,&eslinea)) && p1 < p)
		{
			if (eslinea)
			{
				break;
			}
			pn++;
		}
		if (pn == 3) 
		{			
			sinc = 1;
		}

		lineas_word[nlineas].ri = ri;

		if ( sinc )
		{
			n--;
			pseudo_lineas[n] = malloc(100);
			sprintf(pseudo_lineas[n],"0,1");		
			n--;
			pseudo_lineas[n] = malloc(100);
			sprintf(pseudo_lineas[n],"%%s,");
			lineas_word[nlineas].cc = NULL;
		}
		else
		{
			n--;
			pseudo_lineas[n] = malloc(100);
			sprintf(pseudo_lineas[n],"%d,1",pn-3);

			lineas_word[nlineas].cc = (int *)malloc(sizeof(int *)*(pn-2));

			numc = 0;
			p1 = op;
			eslinea = 0;
			pn = 0;
			while((p1 = busca_quote(p1,&numc,&eslinea)) && p1 < p)
			{
				if (eslinea)
				{
					break;
				}
				p2 = p1;
				while(*p2 && *p2 != 1 && (*p2 != '}' || (*p2 == '}' && *(p2+1) != '}'))) p2++;
				memcpy(tmp,p1,(int)(p2-p1));
				tmp[(int)(p2-p1)] = 0;
				/*quita_blancos(tmp);*/
				n--;
				pseudo_lineas[n] = malloc(strlen(tmp)+1);
				strcpy(pseudo_lineas[n],tmp);
				lineas_word[nlineas].cc[pn] = numc;
				pn++;
			}
		}
		if (eslinea)
		{
			p2 = p1;
			while(*p2 && *p2 != -1 && (*p2 != '$' || (*p2 == '$' && *(p2+1) != '$')) && (*p2 != '}' || (*p2 == '}' && *(p2+1) != '}')) ) p2++;			
			n--;
			pseudo_lineas[n] = malloc((int)(p2-p1)+1);
			memcpy(pseudo_lineas[n],p1,(int)(p2-p1));
			pseudo_lineas[n][ (int)(p2-p1) ] = 0;

			if (*p2 == '$' && *(p2+1) == '$')
			{
				p2 += 2;
				p3 = p2;
				while(*p2 && *p2 != -1 && (*p2 != '$' || (*p2 == '$' && *(p2+1) != '$')) && (*p2 != '}' || (*p2 == '}' && *(p2+1) != '}')) ) p2++;
				n--;
				pseudo_lineas[n] = malloc((int)(p2-p3)+1);
				memcpy(pseudo_lineas[n],p3,(int)(p2-p3));
				pseudo_lineas[n][ (int)(p2-p3) ] = 0;
			}
			else
			{
				n--;
				pseudo_lineas[n] = malloc(100);
				sprintf(pseudo_lineas[n],"");
			}

			saca_rango_campo(p1,&ri,NULL);
			ri--;
			lineas_word[nlineas].rf = ri;
			saca_rango_campo(p1,NULL,&ri);
			ri++;
		}
		else
		{
			n--;
			pseudo_lineas[n] = malloc(100);
			sprintf(pseudo_lineas[n],"");
			n--;
			pseudo_lineas[n] = malloc(100);
			sprintf(pseudo_lineas[n],"");
			lineas_word[nlineas].rf = rf;
		}
		nlineas++;
	}

	return p;
}

static char *crea_pseudo_cuerpo(char *p,char *final,int *numf,long *b_ri,long *b_rf,int *rayado,int *inirayado)
{
	int i;
	int ncuerpo = atoi(p+3);
	char *p1 = p + 3;
	while (*p1 && *p1 != ';' && *p1 != '}') p1++;
	if (*p1 == ';')
	{
		p1++;
		if (rayado)
			*rayado = atoi(p1);
		while (*p1 && *p1 != ';' && *p1 != '}') p1++;
		if (*p1 == ';')
		{
			p1++;
			if (inirayado)
				*inirayado = atoi(p1);
		}
	}


	p = crea_pseudo_bloque(p,final,numf,b_ri,b_rf);

    n_pseudo_lineas += 2;
    pseudo_lineas = (char **)realloc(pseudo_lineas,sizeof(char *) * n_pseudo_lineas);
	for (i = n_pseudo_lineas-2;i > 0;i--)
	{
		pseudo_lineas[i] = pseudo_lineas[i-1];
	}
	pseudo_lineas[0] = malloc(100);
	strcpy(pseudo_lineas[0],"0");
	pseudo_lineas[n_pseudo_lineas-1] = malloc(100);
	sprintf(pseudo_lineas[n_pseudo_lineas-1],"%d",ncuerpo);

	return p;
}

static char *crea_pseudo_subbloque(char *p,char *final,int *numf)
{
	char tmp[512];
	char *p2;
	int cuando = 0,i;
	if (!memcmp(p,"SUT",3))
	{
		cuando = -1;
	}
	p2 = p+3;
	while(*p2 && *p2 != 1 && (*p2 != '}' || (*p2 == '}' && *(p2+1) != '}'))) p2++;
	memcpy(tmp,p+3,(int)(p2-p-3));
	tmp[(int)(p2-p-3)] = 0;
	
	p = crea_pseudo_bloque(p,final,numf,NULL,NULL);
	n_pseudo_lineas += 2;
	pseudo_lineas = (char **)realloc(pseudo_lineas,sizeof(char *) * n_pseudo_lineas);
	for (i = n_pseudo_lineas-1;i > 0;i--)
	{
		pseudo_lineas[i] = pseudo_lineas[i-2];
	}
	pseudo_lineas[1] = malloc(strlen(tmp)+1);
	strcpy(pseudo_lineas[1],tmp);
	pseudo_lineas[0] = malloc(100);
	sprintf(pseudo_lineas[0],"%d",cuando);
    
	return p;
}

INFORME *lee_informe_word(char *nom)
{
	INFORME *pin;
	char tmp[10000];
	int handle;	
	long nt;
	char *p1;	
	SUBBLOQUE_INF *c_sub[MAXBLOQ];
	int nsubbloque = 0;
	long b_ri,b_rf;

#ifdef RWSERVER
	if (en_conexion_externa)
	{
      int sal = 0;      
      strcpy(tmp,nom);
      sal = strlen(tmp);
      tmp[sal] = 1;
      strcpy(tmp+sal+1,tmp);
      sal = rw_envia_fichero(tmp,777);      
	  if (sal < 0)
		  return NULL;
	}
#endif

	sprintf(tmp,"#%s",nom);    
    (*DespachadorExterno)((int)DESPACHO_ENLACEWORD,0,tmp);
    handle = atoi(tmp);

	if (handle < 0)
	{
		return NULL;
	}


	pin = inicializa_infor(1);

	strcpy(pin->i_fich,nom);
    pin->w_handle1 = handle;	

	
    /* creamos rango en el nuevo doc para insercion */
	sprintf(tmp,"#%d\001&1\0010",handle);
    (*DespachadorExterno)((int)DESPACHO_ENLACEWORD,6,tmp);

	/* seleccionamos toda la 'historia' principal de la plantilla y tenemos el buffer maximo */
	sprintf(tmp,"%d\001&1\0010",handle);
    (*DespachadorExterno)((int)DESPACHO_ENLACEWORD,6,tmp);

	nt = atol(tmp);
	if (nt >= 0)
	{
		p1 = tmp;
		while(*p1 && *p1 != ',') p1++;
		if (*p1) p1++;
		while(*p1 && *p1 != ',') p1++;
		if (*p1) p1++;
		nt = atol(p1);

		/* alocamos memoria para todo el texto posible */
		p1 = malloc(nt + 512);

		pin->i_buffer_word = p1;

		/*
		sprintf(p1,"%d\001|A\001%ld",handle,nt+1);
		(*DespachadorExterno)((int)DESPACHO_ENLACEWORD,6,p1);
		*/

		/* recogemos los campos que contiene el documento */
		sprintf(p1,"%d\001@\0010",handle);
		(*DespachadorExterno)((int)DESPACHO_ENLACEWORD,6,p1);

        if ( strbusca(p1,"{{DSINFORM}}") >= 0 )
		{			
			/* es formato Diagram */
			/* modo rapido: crear un .inf compatible y luego enlazarlo al doc */
			int nb = 0;
			int numf;
			char *p = p1;
			char *p2;			
			
			numf = 0;
			while((p = busca_private(p,&numf)))
			{		
				p2 = p;
				while(*p2 && *p2 != 1 && (*p2 != '}' || (*p2 == '}' && *(p2+1) != '}'))) p2++;
				memcpy(tmp,p,(int)(p2-p));
				tmp[(int)(p2-p)] = 0;
				quita_blancos(tmp);
				if (!memcmp(tmp,"FIC",3))
				{
					pon_pseudo_lineas(tmp+3);
					lee_ifich(NULL);
				} else if (!memcmp(tmp,"VAR",3))
				{
					pon_pseudo_lineas(tmp+3);
					lee_ivar(NULL);
				} else if (!memcmp(tmp,"CAB",3))
				{					
					p = crea_pseudo_bloque(p,"/CAB",&numf,&b_ri,&b_rf);
					pin->i_cabecera = leo_bloque(NULL);
					pin->i_cabecera->i_word = handle;
					pin->i_cabecera->i_exacto = 3;
					pin->i_cabecera->i_ri = b_ri;
					pin->i_cabecera->i_rf = b_rf;
				} else if (!memcmp(tmp,"CUE",3))
				{
					p = crea_pseudo_cuerpo(p,"/CUE",&numf,&b_ri,&b_rf,&(pin->i_rayado),&(pin->i_inirayado));
					pin->i_cuerpo = leo_cuerpo(NULL);
					pin->i_cuerpo->c_bloq->i_word = handle;
					pin->i_cuerpo->c_bloq->i_exacto = 2;
					pin->i_cuerpo->c_bloq->i_ri = b_ri;
					pin->i_cuerpo->c_bloq->i_rf = b_rf;
				} else if (!memcmp(tmp,"PIE",3))
				{				
					if (tmp[3] == 'S' || tmp[3] == 'F')
					{
						pin->si_juntopie = tmp[3];
					}
					p = crea_pseudo_bloque(p,"/PIE",&numf,&b_ri,&b_rf);
					pin->i_pie = leo_bloque(NULL);
					pin->i_pie->i_word = handle;
					pin->i_pie->i_exacto = 1;
					pin->i_pie->i_ri = b_ri;
					pin->i_pie->i_rf = b_rf;
					if (pin->i_salto)
					{
						pin->i_salto->i_ri = b_ri;
						pin->i_salto->i_rf = b_rf;
					}
				} else if (!memcmp(tmp,"SAL",3))
				{					
					p = crea_pseudo_bloque(p,"/SAL",&numf,&b_ri,&b_rf);
					pin->i_salto = leo_bloque(NULL);
					pin->i_salto->i_word = handle;
					pin->i_salto->i_exacto = 1;
					if (pin->i_pie)
					{
						pin->i_salto->i_ri = pin->i_pie->i_ri;
						pin->i_salto->i_rf = pin->i_pie->i_rf;
					}
					else
					{
						pin->i_salto->i_ri = b_ri;
						pin->i_salto->i_rf = b_rf;
					}
				} else if (!memcmp(tmp,"SUC",3))
				{									
					p = crea_pseudo_subbloque(p,"/SUC",&numf);
					c_sub[nsubbloque] = leo_subbloque(NULL);
					c_sub[nsubbloque]->s_bloq->i_word = handle;
				    nsubbloque++;
				} else if (!memcmp(tmp,"SUT",3))
				{					
					p = crea_pseudo_subbloque(p,"/SUT",&numf);				
					c_sub[nsubbloque] = leo_subbloque(NULL);
					c_sub[nsubbloque]->s_bloq->i_word = handle;
				    nsubbloque++;
				}
			}
		}
	}

	if (!pin->i_cuerpo)
	{
		libera_inf(pin);
		pin = NULL;
	}
	if (pin)
	{
		int i;
		pin->i_cuerpo->c_nsub = nsubbloque;
		for (i = 0;i < nsubbloque;i++)
		{
			pin->i_cuerpo->c_sub[i] = c_sub[i];
		}
	}

	return pin;
}

char *empaqueta_temp_def(FI_DEF *fi)
{
char *da_memoria_respaldo(int memoria);
int memoria = sizeof(FI_DEF_LEE) + sizeof(CAMPO_DEF) * fi->f_ncampos + sizeof(KEY_DEF) * fi->f_nclaves;
int i;
char *p,*buf;


  for (i=0;i<fi->f_nclaves;i++) 
  {
      memoria += (fi->f_clave[i].k_ncampos * 2 * sizeof(short));          
  }
  for (i = 0;i < fi->f_ncampos;i++)
  { 
      memoria += (strlen(fi->f_campos[i].c_lista)+1);	  
	  memoria += (strlen(fi->f_campos[i].c_minimo)+1);
	  memoria += (strlen(fi->f_campos[i].c_maximo)+1);
  }
  
  memoria += (sizeof(short) + sizeof(int)*2);
  
  buf = da_memoria_respaldo(memoria);

  p = buf;
  *((int *)p) = memoria;
  p += sizeof(int);
  *((short *)p) = FGVGEN_ID_MAS;
  p += sizeof(short);

  *((FI_DEF_LEE *)p) = *((FI_DEF_LEE *)fi);
  p += sizeof(FI_DEF_LEE);

  memcpy(p,fi->f_campos,sizeof(CAMPO_DEF) * fi->f_ncampos);
  p += (sizeof(CAMPO_DEF) * fi->f_ncampos);

  memcpy(p,fi->f_clave,sizeof(KEY_DEF) * fi->f_nclaves);
  p += (sizeof(KEY_DEF) * fi->f_nclaves);

  for (i=0;i<fi->f_nclaves;i++) 
  {
      memcpy(p,fi->f_clave[i].k_campo,sizeof(short) * fi->f_clave[i].k_ncampos);
      p += (sizeof(short) * fi->f_clave[i].k_ncampos);
      memcpy(p,fi->f_clave[i].k_orden,sizeof(short) * fi->f_clave[i].k_ncampos);
      p += (sizeof(short) * fi->f_clave[i].k_ncampos);
  }

  for (i = 0;i < fi->f_ncampos;i++)
  {
      strcpy(p,fi->f_campos[i].c_lista);
      p += (strlen(fi->f_campos[i].c_lista)+1);
  }

  for (i = 0;i < fi->f_ncampos;i++)
  {
      strcpy(p,fi->f_campos[i].c_minimo);
      p += (strlen(fi->f_campos[i].c_minimo)+1);
  }

  for (i = 0;i < fi->f_ncampos;i++)
  {
      strcpy(p,fi->f_campos[i].c_maximo);
      p += (strlen(fi->f_campos[i].c_maximo)+1);
  }

  return buf;
}

FI_DEF *ajusta_inline_temp_pfi(char *p,int memoria)
{	
    FI_DEF *fi;
    int i;
	int tam = memoria+(sizeof(FI_DEF)-sizeof(FI_DEF_LEE))+((FI_DEF_LEE *)p)->f_lfich + 1;

	fi = (FI_DEF *)malloc(tam);
	memset((char *)fi,0,tam);
	memcpy((char *)fi,p,sizeof(FI_DEF_LEE));
	memcpy(((char *)fi)+sizeof(FI_DEF),p+sizeof(FI_DEF_LEE),memoria-sizeof(FI_DEF_LEE));

	p = ((char *)fi)+sizeof(FI_DEF);    

    fi->f_campos = (CAMPO_DEF *)p;
    
    p += (sizeof(CAMPO_DEF) * fi->f_ncampos);

    fi->f_clave = (KEY_DEF *)p;
  
    p += (sizeof(KEY_DEF) * fi->f_nclaves);

    for (i=0;i<fi->f_nclaves;i++) 
    {
       fi->f_clave[i].k_campo = (short *)p;       
       p += (sizeof(short) * fi->f_clave[i].k_ncampos);
       fi->f_clave[i].k_orden = (short *)p;
       p += (sizeof(short) * fi->f_clave[i].k_ncampos);
    }
    for (i = 0;i < fi->f_ncampos;i++)
    {
       fi->f_campos[i].c_lista = p;       
       p += (strlen(p)+1);
    } 

    for (i = 0;i < fi->f_ncampos;i++)
    {
       fi->f_campos[i].c_minimo = p;       
       p += (strlen(p)+1);
    } 

    for (i = 0;i < fi->f_ncampos;i++)
    {
       fi->f_campos[i].c_maximo = p;       
       p += (strlen(p)+1);
    } 

    for (i = 0;i < fi->f_ncampos;i++)
    {
	   fi->f_campos[i].c_mante.c_defecto = "";
   	   fi->f_campos[i].c_usi = strmalloc(50);
	   fi->f_campos[i].c_usi[0] = 0;
	   calcula_campo(&fi->f_campos[i]);
	}

	fi->f_buf = ((char *)fi) + memoria + (sizeof(FI_DEF)-sizeof(FI_DEF_LEE));
    return fi;
}

int remoto_maneja_def_temporal(char *buf)
{
	char *p = buf + sizeof(int);
	short id = *(short *)p;
	int memoria,fd;
	static FI_DEF *temp_pfis[100] = {NULL,NULL,NULL,};
	FI_DEF *pfi;

	memoria = *((int *)buf);
	memoria -= (sizeof(int)+sizeof(short));
	p += sizeof(short);

	switch(id)
	{
	case FGVGEN_ID_MAS:
		pfi = ajusta_inline_temp_pfi(p,memoria);
		if (pfi)
		{
			 if (pfi->f_fd >= 0 && pfi->f_fd < 100)
			 {
				 temp_pfis[pfi->f_fd] = pfi;
				 pfi->f_fd = -1;
				 abre_def_temporal(pfi);
			 }
			 else
				 free((char *)pfi);
		}
		break;
	case 1:
		fd = *((int *)p);
		if (fd >= 0 && fd < 100 && temp_pfis[fd] && temp_pfis[fd]->f_lfich == (memoria-sizeof(int)))
		{
			p += sizeof(int);
			memcpy(temp_pfis[fd]->f_buf,p,temp_pfis[fd]->f_lfich);
			graba_datos_def_temporal(temp_pfis[fd]);
		}
		break;
	case 2:
		fd = *((int *)p);
		if (fd >= 0 && fd < 100 && temp_pfis[fd])
		{
			cierra_def_temporal(temp_pfis[fd]);
		}
		break;
	case 3:
		fd = *((int *)p);
		if (fd >= 0 && fd < 100 && temp_pfis[fd])
		{
			int i;
			borra_def_temporal(temp_pfis[fd]);
			for (i = 0;i < temp_pfis[fd]->f_ncampos;i++)
			{
				free(temp_pfis[fd]->f_campos[i].c_usi);
			}
			free((char *)temp_pfis[fd]);
			temp_pfis[fd] = NULL;
		}
		break;
	}
	return 0;
}

#ifdef WIN32
static HANDLE mutex = NULL;
#endif

int abre_def_temporal(FI_DEF *pfi)
{	
#ifdef RWSERVER
	if (en_conexion_externa)
	{
	   char *empaqueta_temp_def(FI_DEF *fi);
	   char *p;
	   static int contador_abre = 0;
	   
	   if (contador_abre >= 100)
		   contador_abre = 0;
	   pfi->f_fd = contador_abre;
	   contador_abre++;

	   p = empaqueta_temp_def(pfi);
	   funcion_windows_cliente(1041,p,*((int *)p));
	   return pfi->f_fd;
	}
#endif
#ifdef WIN32		
		if (!mutex) {
			mutex = CreateMutex(NULL,FALSE,"DSCRISTAL_DATOS");
			if (mutex != NULL) {
				while(WaitForSingleObject( 
					mutex,    // handle to mutex
					2000) == WAIT_TIMEOUT) {
					aviso(0,"Por favor, cierre del proceso de impresión en Crystal Reports anterior o espere a que termine.");
					//pulsatecla();
				}
			}
		}
#endif
	/*MessageBox(NULL,pfi->f_fichero,pfi->f_fichero,MB_OK);*/
	crea_dir(pfi->f_fichero);
	pfi->tipo_datos = 3;
	pfi->f_ipfi = -1;
	borra_ind(pfi);
	pfi->f_fd = abre_directo(pfi,pfi->f_fichero);
	return pfi->f_fd;
}

int graba_datos_def_temporal(FI_DEF *pfi)
{
#ifdef RWSERVER
	if (en_conexion_externa)
	{
		char *da_memoria_respaldo(int memoria);
		char *p;
		char *p1;
		p = da_memoria_respaldo(sizeof(short)+sizeof(int)*2+pfi->f_lfich);
		*((int *)p) = sizeof(short)+sizeof(int)*2+pfi->f_lfich;
		p1 = p + sizeof(int);
		*((short *)p1) = 1;
		p1 += sizeof(short);
		*((int *)p1) = pfi->f_fd;
		p1 += sizeof(int);
		memcpy(p1,pfi->f_buf,pfi->f_lfich);
		funcion_windows_cliente(1041,p,*((int *)p));
		return 0;
	}
#endif
	if (pfi->f_fd > -1)
	{
		long reg = -(pfi->f_ipfi);
		pfi->f_ipfi--;
		return graba_directo(pfi->f_fd,reg,pfi,pfi->f_buf);
	}
	return -1;
}



int cierra_def_temporal(FI_DEF *pfi)
{
#ifdef RWSERVER
	if (en_conexion_externa)
	{
		char *da_memoria_respaldo(int memoria);
		char *p;
		char *p1;
		p = da_memoria_respaldo(sizeof(short)+sizeof(int)*2);
		*((int *)p) = sizeof(short)+sizeof(int)*2;
		p1 = p + sizeof(int);
		*((short *)p1) = 2;
		p1 += sizeof(short);
		*((int *)p1) = pfi->f_fd;
		funcion_windows_cliente(1041,p,*((int *)p));
		return 0;
	}
#endif
	if (pfi->f_fd > -1)
	{
		if (!strcmp(quita_path(pfi->f_fichero),"cabecera") || !strcmp(quita_path(pfi->f_fichero),"detalle")) {
			char tmp[512];
			char tmp2[512];			
			FILE *fout2;
			int i;
			unsigned char *pp;
			long pos = 1L;

			strcpy(tmp,pfi->f_fichero);			
			strcat(tmp,".csv");
			fout2 = fopen(tmp,"w");
			if (fout2) {
				for (i = 0;i < pfi->f_ncampos;i++) {
					if (pfi->f_campos[i].c_lista[0] == 13)	{
					   strcpy(tmp,pfi->f_campos[i].c_lista+1);
					}
					else if (pfi->f_campos[i].c_lista[0]) {
					   strcpy(tmp,pfi->f_campos[i].c_lista);
					}
					else if (pfi->f_campos[i].c_nombre){
					   strcpy(tmp,pfi->f_campos[i].c_nombre);
					}
					tmp[10] = 0;
                                        tmp2[0] = 0;
                                        switch(pfi->f_campos[i].c_tipo) {
                                            case FECHATIPO:
                                                 strcpy(tmp2,"@Date;");
                                                 break;
                                            case INTTIPO:
                                                 strcpy(tmp2,"@Int;");
                                                 break;
                                            case FLOATTIPO:
                                            case DOUBLETIPO:
                                                 strcpy(tmp2,"@Double;");
                                                 break;
					    default:
                                                 strcpy(tmp2,"@String;");
                                                 break;
					}
                                        strcat(tmp2,tmp);
					if (i)
						fprintf(fout2,"%c%s",8,tmp2);
					else
						fprintf(fout2,"%s",tmp2);
				}
				fprintf(fout2,"\n");
				while(lee_directo(pfi->f_fd,pos,pfi,pfi->f_buf) >= 0) {

                                       //printf("pos=%d\r\n",pos);fflush(stdout);

					for (i = 0;i < pfi->f_ncampos;i++) {
						//printf("campo=%d\r\n",i);fflush(stdout);
                                                if (pfi->f_campos[i].c_tipo == FECHATIPO) {
                                                       int idx = pfi->f_campos[i].c_offset;
                                                       memcpy(tmp2,pfi->f_buf+idx,4);
						       tmp2[4] = '-';
                                                       memcpy(tmp2+5,pfi->f_buf+idx+4,2);
						       tmp2[7] = '-';
                                                       memcpy(tmp2+8,pfi->f_buf+idx+6,2);
						       tmp2[10] = 0;
                                                }
							else
								ld_campo(i,pfi,tmp2);
							//printf("tmp2=%s\r\n",tmp2);fflush(stdout);
		                                        tmp2[257] = 0;
							for (pp = (unsigned char *)tmp2;*pp;pp++)
							   oem_a_ansi(pp);
							for (pp = (unsigned char *)tmp2;*pp;pp++) {
							   if (*pp < 14)
								*pp = ' ';
							}						
						if (i)
							fprintf(fout2,"%c%s",8,tmp2);
						else
							fprintf(fout2,"%s",tmp2);
					}
					fprintf(fout2,"\n");
					pos++;
				}
				fclose(fout2);
			}			
		}
		cierra_directo(pfi->f_fd);
	}	
	return 0;
}

int borra_def_temporal(FI_DEF *pfi)
{
#ifdef RWSERVER
	if (en_conexion_externa)
	{
		char *da_memoria_respaldo(int memoria);
		char *p;
		char *p1;
		p = da_memoria_respaldo(sizeof(short)+sizeof(int)*2);
		*((int *)p) = sizeof(short)+sizeof(int)*2;
		p1 = p + sizeof(int);
		*((short *)p1) = 3;
		p1 += sizeof(short);
		*((int *)p1) = pfi->f_fd;
		funcion_windows_cliente(1041,p,*((int *)p));
		return 0;
	}
#endif
	borra_ind(pfi);
#ifdef WIN32
		if (mutex != NULL) {
			ReleaseMutex(mutex);
			CloseHandle(mutex);
			mutex = NULL;
		}
#endif
}

static int crea_def_de_bloque(FI_DEF **_fi,BLOQUE_INF *pb,char *nombre)
{
	FI_DEF *pfi = *_fi;
	int k,i,n,nc,nvar,memo_id,campo,nrep,hayrep;
	LINEA_INF *susti;
	char tmpx[512];
	char tmp1[512];
	char tmp2[512];
	char *p;
	int lenapa;
	int nletras = 0;
	double doble;

	if (!pfi)	
	{		
		pfi = (FI_DEF *)malloc(sizeof(FI_DEF));
		memset((char *)pfi,0,sizeof(FI_DEF));
		pfi->f_titulo = strmalloc(strlen(nombre)+1);
	    strcpy(pfi->f_titulo,nombre);
		strcpy(pfi->f_fichero,mi_remoto_dirbass());
		strcat(pfi->f_fichero,"crystal/");
		strcpy(pfi->f_PathDef,pfi->f_fichero);
		strcat(pfi->f_fichero,nombre);
		strcat(pfi->f_PathDef,nombre);

	    inserta_campo(pfi,0,"_key_dbf",
							"_key_dbf",
					0,
					"-999999999",
					"999999999",
					"");
	}
	nc = pfi->f_ncampos;
	pb->pfi = pfi;


    for (k=0;k<pb->b_nlin;k++) 
    {
	   susti = pb->b_lin[k];	   

	   if (!susti->l_pfi)
	   {
		   susti->l_pfi = (int *)malloc(sizeof(int)*susti->l_ncamp);
		   memset((char *)susti->l_pfi,0xff,sizeof(int)*susti->l_ncamp);
	   }
       for(i=0;i<susti->l_ncamp;i++) 
	   {
		   p = susti->l_form[i];
		   lenapa = 0;
		   do
		   {
			   while (*p && *p != '%') p++;
			   if (*p == '%' && *(p+1) == '%')
			   {
				   p += 2;
			   }
			   else
				   break;
		   } while(-1);
		   if (*p == '%')
		   {
			   for (n = 0;p[n] && p[n] != 's';n++)
				   tmp1[n] = p[n];
			   if (p[n] == 's')
			   {
				   tmp1[n] = p[n];
				   n++;
				   tmp1[n] = 0;
				   sprintf(tmpx,tmp1,"");
				   lenapa = strlen(tmpx);
			   }
		   }

		   if ((susti->l_p[i] % 10) > 1)
		   {
			   if (lenapa < 1)
				   lenapa = 80;

			   nletras++;
			   sprintf(tmpx,"LETRAS_%d",nletras);
			   inserta_campo(pfi,nc,tmpx,
									tmpx,
							lenapa,
							"",
							"",
							"");
			   susti->l_pfi[i] = nc;
			   nc++;
		   }
           else if (susti->l_fich[i] >= 0) 
		   {		   
               nvar = susti->l_fich[i] - MAXDEF;
               if (nvar < 0) 
			   {
                   memo_id = susti->l_fich[i];
                   campo = susti->l_camp[i];
                   if (campo < 0) 
                   {
                       campo = pasa_a_int(_memo[((-campo)-1)].mv.vald);
                   }				   
				   sprintf(tmpx,"%s%02x",quita_path(v_mpfi(memo_id)->f_PathDef),campo);

				   for (n = 0;n < pfi->f_ncampos;n++)
				   {
					   if (!strcmp(pfi->f_campos[n].c_nombre,tmpx))
					   {
						   break;
					   }		
				   }
				   if (n < pfi->f_ncampos)
					   continue;

				   inserta_campo(pfi,nc,tmpx,
										tmpx,
								v_mpfi(memo_id)->f_campos[campo].c_qc,
								v_mpfi(memo_id)->f_campos[campo].c_minimo,
								v_mpfi(memo_id)->f_campos[campo].c_maximo,
								v_mpfi(memo_id)->f_campos[campo].c_mante.c_defecto);
				   susti->l_pfi[i] = nc;
				   nc++;
			   }
		   }
           else
		   {			   
               nvar = susti->l_camp[i];
			   for (n = 0;n < totalmemos;n++)
			   {
				   if (memos[n]->numero == nvar)
				   {
					   strcpy(tmpx,memos[n]->nombre);
					   tmpx[10] = 0;
					   break;
				   }
			   }
			   if (n >= totalmemos)
			   {
				   sprintf(tmpx,"MI_%02x",nvar);
			   }
			   /*
			   for (n = 0;n < pfi->f_ncampos;n++)
			   {
				   if (!strcmp(pfi->f_campos[n].c_nombre,tmpx))
				   {
					   break;
				   }		
			   }
			   if (n < pfi->f_ncampos)
				   continue;		   
			   else
			   */
			   {
				   nrep = 0;					   
				   do
				   {
					   if (nrep > 0)
						   sprintf(tmp1,"%s%d",tmpx,nrep);
					   else
						   strcpy(tmp1,tmpx);
					   hayrep = 0;					   
					   for (n = 0;n < pfi->f_ncampos;n++)
					   {
						   strcpy(tmp2,pfi->f_campos[n].c_nombre);						   
						   if (!strcmp(tmp2,tmp1))
						   {
							   nrep++;
							   hayrep = 1;
						   }
					   }
				   } while(hayrep);
				   if (nrep > 0)
					   strcpy(tmpx,tmp1);
			   }
			   
			   if (_memo[nvar].m_tipo == CHARTIPO || _memo[nvar].m_tipo == FECHATIPO)
			   {
				   if (lenapa < 1)
				   {
					   inserta_campo(pfi,nc,tmpx,
											tmpx,
									60,
									"",
									"",
									"");
				   }
				   else
				   {
					   inserta_campo(pfi,nc,tmpx,
											tmpx,
									lenapa,
									"",
									"",
									"");
				   }
			   }
			   else
			   {
				   doble = (double)999999999999.999999;
				   sprintf(tmp1,susti->l_usi[i],doble);
				   for (p = tmp1;*p;p++)
				   {
					   if (*p != '.')
					   {
						   *p = '9';
					   }
				   }
				   tmp2[0] = '-';
				   strcpy(tmp2+1,tmp1);

				   inserta_campo(pfi,nc,tmpx,
										tmpx,
								0,
								tmp2,
								tmp1,
								"");
			   }
			   susti->l_pfi[i] = nc;
			   nc++;			   
		   }

	   }
    }

	*_fi = pfi;
	return 0;
}

static int crea_defs_informe(INFORME *pin)
{
	char *p;
	int i;


	pfi_cabeceras = 0.0;

	if (pin->i_cabecera)
	{
		crea_def_de_bloque(&pin->pfi_cabecera,pin->i_cabecera,"cabecera");
	}
	if (pin->i_pie)
	{
		crea_def_de_bloque(&pin->pfi_cabecera,pin->i_pie,"cabecera");
	}

	if (pin->i_salto)
	{
		pin->i_salto->pfi = (FI_DEF *)1;
	}

	if (pin->i_cuerpo)
	{
		if (pin->i_cuerpo->c_bloq)
		{
			crea_def_de_bloque(&pin->pfi_cuerpo,pin->i_cuerpo->c_bloq,"detalle");
		}
        for(i=0;i<pin->i_cuerpo->c_nsub;i++) 
		{
			crea_def_de_bloque(&pin->pfi_cuerpo,pin->i_cuerpo->c_sub[i]->s_bloq,"detalle");
		}
	}

	
	if (pin->pfi_cabecera)
	{
		abre_def_temporal(pin->pfi_cabecera);		
	}
	
	if (pin->pfi_cuerpo)
	{
		abre_def_temporal(pin->pfi_cuerpo);		
	}
	

	return 0;
}

INFORME *leo_inf(nomfich)
char *nomfich;
{
   char text[256];
   char nom[MAXPATH] ;
   FILE *pfich ;
   INFORME *pin;
   int i;
   int rptauto = 0;
   
   
#ifdef FGVWIN
   INICIO_LIS=501;
#else
   INICIO_LIS=101;
#endif
   
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
			if (remoto_es_sistema_grafico)
                  {
                     INICIO_LIS=501;
                  }
                  else
                  {
                     INICIO_LIS=101;
                  }
   }
#endif
#endif

   
   
   
   if (nomfich == quita_path(nomfich)) {
						strcpy(nom,dirdef);
        strcat(nom,nomfich);
   }
   else
	    strcpy(nom,nomfich);
   
   strcpy(nom_inf,nom);

   if (strbusca(nom,".doc") > -1)
   {
	   /* informe word */	   
	   return lee_informe_word(nom);
   } else if (strbusca(nom,".xls") > -1)
   {
	   /* informe excel */
	   return NULL;
   } else if (strbusca(nom,".rpt") > -1)
   {
	   int imprime = 0;
	   /* informe crystal */
	   {
			  int var;
			  var = busca_externa("XVAR_DC_IMPRIME");
			  if (var > 0)
			  {
				  if ((int)v_memo(var)->numerica == 1)
					  imprime = 1;
			  }
	   }

	   CargaVInterface("dscristal.dll",(unsigned long)(0x80000000 | imprime));
	   return NULL;
   } else if (strbusca(nom,".inf") < 0)
   {
	   strcat(nom,".inf");
   }   
   
   if (fgv_crear_plantilla_doc)
   {
	   struct stat st1,st2;
	   char tmp2[300];
	   int pasar = 0;
	   if (stat(nom,&st1) < 0)
		   pasar = 1;
	   strcpy(tmp2,nom);
       strcat(tmp2,".doc");
	   if (!pasar && stat(tmp2,&st2) < 0)
		   pasar = 1;
	   if (!pasar)
	   {
		   if (st1.st_mtime >= st2.st_mtime)
		   {
			   pasar = 1;
		   }
	   }
	   if (pasar)
	   {
		   pasa_inf_a_doc(nom);
	   }	   
	   strcat(nom,".doc");
	   return lee_informe_word(nom);
   }
   
   strcpy(text,nom_inf);
   if (_imp == 0x00000003L && nombredev[0] == 2 && nombredev[1] == 0)
   {
	   char tmp2[128];
	   if ((i = strbusca(text,".inf")) >= 0)
	   {
		   text[i] = 0;
	   }
	   strcat(text,".rpt");
	   pfich = fopen(text,"r");
	   if (pfich)
	   {
		   /* Informe rpt-auto */

		   fclose(pfich);
		   rptauto = 1;
	   }
	   if (!rptauto)
	   {
		   if (nombredev[0] == 2 && nombredev[1] == 0 && nombredev[2] == '!' && nombredev[3] == '!')
		   {
			   /* es un fax .... grabar a pdf */
		   }
		   else if (nombredev[0] == 2 && nombredev[1] == 0 && nombredev[2] == '$' && nombredev[3] == '$')
		   {
			   /* salida normal a pdf */
		   }
		   else if (nombredev[0] == 2 && nombredev[1] == 0 && nombredev[2] == '|' && nombredev[3] == '|')
		   {
			   /* salida normal a correo-pdf */
		   }
		   else
		   {
			   sprintf(tmp2,"No existe el informe de Crystal Reports %s",text);
			   aviso(0,tmp2);
			   return NULL;
		   }
	   }
   }
   if ((pfich = fopen(nom,"r")) == NULL) 
   {
		return(NULL);
   }

   pin = inicializa_infor(0);
   if (rptauto)
	   pin->i_tipo_informe = 2;
   
   lee_linea(text,255,pfich);
   if (strlen(text) >= MAXPATH) 
   {
		  rw_error(38);
          /* printf("\n\rRuntime error: Informe ilegal\n\r");getchar(); */
          return(NULL);
   }
   strcpy(pin->i_fich,text);
   lee_linea(text,255,pfich);
   if (strlen(text) >= 161) 
   {
		  rw_error(39);
          /*printf("\n\rRuntime error: Informe ilegal\n\r");getchar();*/
          return(NULL);
   }
   strcpy(pin->i_nom,text);

   lee_linea(text,255,pfich);
   /* printf("Informe [%s] Linea leida[%s]\n",nom,text); */
   coma_linea(text,0);
   pin->i_nlin = atoi(text);
   pin->i_especial = atoi(text+coma_linea(text,1));
   pin->i_rayado = atoi(text+coma_linea(text,1));
   pin->i_inirayado = atoi(text+coma_linea(text,1));
   /* printf("ESpecial[%d]",pin->i_especial);getchar(); */
   
   lee_linea(text,255,pfich);
   quita_blancos(text);
   pin->si_juntopie = text[0];
   
   pin->i_cabecera = (BLOQUE_INF *)0;
   pin->i_cuerpo = (CUERPO_INF *)0;
   pin->i_pie = (BLOQUE_INF *)0;
   pin->i_salto = (BLOQUE_INF *)0;
   
   while (lee_linea(text,255,pfich) != NULL) 
   {
			quita_blancos(text);
                  if (memcmp(text,"FIC",3) == 0) 
				  {
                     lee_ifich(pfich);
                  }
                  else
                     if (memcmp(text,"VAR",3) == 0) 
					 {
                        lee_ivar(pfich);
						  pon_varia("&EURO","0.00601012");
						  pon_varia("&EURODB","0");
						  pon_varia("&EURODV","2");
						  pon_varia("&EUROTEXTO","AVer Euros");
  						  pon_varia("IMPRE",nombre_impresora());						  
						  sprintf(text,"%d",rptauto);
						  pon_varia("CRYSTAL",text);
                     }
                     else
                        if (memcmp(text,"CAB",3) == 0) 
						{
                           pin->i_cabecera = leo_bloque(pfich);
                        }
                        else
                           if (memcmp(text,"CUE",3) == 0) 
						   {
                              pin->i_cuerpo = leo_cuerpo(pfich);
                           }
                           else
                              if (memcmp(text,"PIE",3) == 0) 
							  {
                                 pin->i_pie = leo_bloque(pfich);
                              }
                              else
                                 if (memcmp(text,"SAL",3) == 0) 
								 {
                                    pin->i_salto = leo_bloque(pfich);
                                 }
   }
   {
	   char *pp;
	   char tmp1[512];
	   int ii;
	   strcpy(tmp1,pin->i_nom);
	   if ((pp = strstr(tmp1,"$F$")))
	   {		  
		   pp += 3;
		   for (ii = 0;pp[ii];ii++)
		   {
			   if (pp[ii] == '$' || pp[ii] == ',')
			   {
				   pp[ii] = 0;
				   break;
			   }
		   }
		   devuelve_campo(pp,&pin->ipfifax,&pin->campofax);		   
	   }
	   strcpy(tmp1,pin->i_nom);
	   if ((pp = strstr(tmp1,"$M$")))
	   {		  
		   pp += 3;
		   for (ii = 0;pp[ii];ii++)
		   {
			   if (pp[ii] == '$' || pp[ii] == ',')
			   {
				   pp[ii] = 0;
				   break;
			   }
		   }
		   devuelve_campo(pp,&pin->ipfimail,&pin->campomail);
	   }
   }
   totalfichs = 0;
   free((char *)fichs);
   fclose(pfich);


   if (pin->i_nlin == 0) {
				  if (_imp != (FILE *)0)
                     pin->i_nlin = i_nlineas;
                  else
                  {
					 if (Pseudo10)
					 {
						 pin->i_nlin = -1;
					 }
					 else
					 {
#ifdef FGVWIN
						pin->i_nlin = 19;
#else
						pin->i_nlin = 23;
#endif
					 }
#ifdef RWXARXA
#ifdef RWSERVER
                     if (en_conexion_externa)
                     {
						 if (Pseudo10)
						 {
							 pin->i_nlin = -1;
						 }
						 else
						 {
							if (remoto_es_sistema_grafico)
							{
							   pin->i_nlin = 19;
							}
							else
							{
							   pin->i_nlin = 23;
							}
						 }
                     }
#endif
#endif
                  }
                  if (pin->i_cuerpo)
                  {
                     if (pin->i_pie && pin->i_cabecera)
                     {
                        pin->i_cuerpo->totalcuerpo = pin->i_nlin - pin->i_cabecera->totalbloque -
                           pin->i_pie->totalbloque - 2;
                     }
                     else
                     {
                        if (pin->i_cabecera)
                        {
                           pin->i_cuerpo->totalcuerpo = pin->i_nlin - pin->i_cabecera->totalbloque -2;					 ;
                        }
                        else
                           if (pin->i_pie)
                           {
                              pin->i_cuerpo->totalcuerpo = pin->i_nlin - pin->i_pie->totalbloque -2;					 ;
                           }
                           else
                              pin->i_cuerpo->totalcuerpo = pin->i_nlin  -2;					 ;
                     }
                  }
   }

   if (pin && pin->i_tipo_informe == 2)
   {
	   crea_defs_informe(pin);
   }

   return(pin);
}


static int marca_campo_para_export(char *destino,char *formato,char *campo)
{
   int i,p1,j;
   char usi[128];
   int espuroalfa = 0;
   int esfecha = 0;
   char *xp = campo;

   while(*xp) 
   {
	   if (*xp == 31)
	   {
		   espuroalfa = 1;		   
	   }
	   if (*xp == 30)
	   {
		   esfecha = 1;		
	   }
	   xp++;
   }
   
   j = 0;
   for (i = 0;formato[i];i++)
   {
      if (formato[i] == '%')
      {
         p1 = i + 1;
         while(formato[p1] && ((formato[p1] >= '0' && formato[p1] <= '9') || formato[p1] == '.' || formato[p1] == '-')) p1++;
         if (formato[p1] == 's')
         {
            usi[j] = 9;
            j++;
            while(i <= p1)
            {
               usi[j] = formato[i];
               j++;
               i++;
            }
			if (espuroalfa)
			{
				usi[j] = 31;
				j++;
			}
            if (esfecha)
			{
				usi[j] = 30;
				j++;
			}
            usi[j] = 9;
            j++;
            i = p1;
            continue;
         }
      }
      usi[j] = formato[i];
      j++;
   }
   usi[j] = 0;
   sprintf(destino,usi,campo);   
   return(0);
}

/*  ========================================================================
prin_bloque(pb,pfich) imprime un bloque, en el fichero pfich abierto.
devuelve el numero de lineas impreso.
=======================================================================*/
/* NO SE PUEDE ACCEDER DIRECTAMENTE A FICHEROS TEMPORALES */

static int saca_lin_texto(char *dest,int ft,int campo,int nlintexto,int modolintexto)
{
	int topelin = 0;
	int k;
	int i;
	char *texto;
	int maxqc = v_mpfi(ft)->f_campos[campo].c_qc;
	
	for (k = campo;k < v_mpfi(ft)->f_ncampos;k++)
	{
		if (v_mpfi(ft)->f_campos[k].c_qc != maxqc || (k > campo && !v_mpfi(ft)->f_campos[k].c_mante.c_visual))
		{
			break;
		}
		texto = v_mpfi(ft)->f_buf + v_mpfi(ft)->f_campos[k].c_offset;		
		if (modolintexto)
		{
			for (i = 0;i < maxqc;i++)
			{
				if (nlintexto == topelin)
				{
					if (i >= maxqc)
					{
						dest[0] = 0;
					}
					else
					{
						memcpy(dest,texto+i,maxqc-i);
						dest[maxqc-i] = 0;
						for (i = 0;dest[i] && dest[i] != 13 && dest[i] != 10;i++);
						dest[i] = 0;
						quitab(dest);
					}
					return nlintexto;
				}
				if (texto[i] == 10)
				{
					topelin++;
				}
			}
		}
		else
		{
			if (nlintexto == topelin)
			{
				ld_campo(k,v_mpfi(ft),dest);
				return nlintexto;
			}
			topelin++;
		}
	}

	return -1;
}


extern char *impre_ccont;
extern int impre_nccont;

static int es_inf_cuerpo = 0;

extern int modo_otro_euro;

int prin_bloque(pb,npag)
BLOQUE_INF *pb;
int npag;
{
   int i,j,k,nvar,repe,jk,jk1;
   int *ptr;
   int numlin = 0;
   char pas[512];
   char dest[MAXDEFECTO];
   char *usi;
   char linea[2000];
   int campo;
   LINEA_INF *susti;
   char **memos[255];/* punteros a memo */
   int qmemo[255];   /* identificador de memos */
   int qmemo2[255];   /* identificador de memos */
   int cmemo[255];   /* numero de linea en la memo */
   int nmemos = 0;  /* total memos activas */
   int nmemo;       /* memo corriente */
   int no_memo_leer = 1; /* indica si se deben leer o no el resto de campos */
   int memo_id;
   int memo_repe = 0; /* flag de repeticion para memos sin bloque previsto */
   int memo_att; /* flag que indica que hay memos en la linea */
   int tiene_permiso = 1;
   char *tmpt1,*tmpt2;
   int sexoeuro = 0;
   int eseuro = 0;
   int ndecieuro = 0;
   int hayconper;
   int haysinper;
   int es_la_primera = 1;
   int fichero_texto = -1;
   int campo_texto  = -1;
   int nlintexto = 0;
   int modolintexto = 0;
   
   
   if (pb == (BLOQUE_INF *)0 ) return(0);


   if (pb->i_word >= 0)
   {	   
	   if (pb->i_exacto == 11)
	   {
		  sprintf(pas,"#%d\001>%d\001%d/",pb->i_word,pb->i_ri,pb->i_rf);
		  (*DespachadorExterno)((int)DESPACHO_ENLACEWORD,6,pas);
	   } else if (pb->i_exacto == 1)
	   {
		  sprintf(pas,"#%d\001>%d\001%d",pb->i_word,pb->i_ri,pb->i_rf);
		  (*DespachadorExterno)((int)DESPACHO_ENLACEWORD,6,pas);
	   }
   }

   if (asigna_decimales)
   {
	    modo_otro_euro++;
		ndecieuro = (*asigna_decimales)("EURO");
		modo_otro_euro--;
   }
   
   for (i = 0;i < 255;i++) 
   {
      memos[i] = NULL; /* memos[n] a NULL indica memo no activa */
      qmemo[i] = -1;
	  qmemo2[i] = -1;
      cmemo[i] = 0;
   }

   
   for (k=0;k<pb->b_nlin;k++) 
   {
      susti = pb->b_lin[k];
      memo_att = 0;
      if ((j = susti->l_lcond) > -1 && condicion_i(j) != 0) continue;
      
      ptr = susti->l_cal;                   /* hace el calculo de linea */
      while(*ptr != -1) 
	  {
         if (*ptr > 31999) 
		 { /* leer fichero */
            i = *ptr - 32000;
            if (f_v(i,RABIERTO) != 0) 
			{
               abre1_rel(i);
               j = 1;
            }
            else
               j = 0;
            lee_reg(v_mpfi(i),ISEQUAL);
            if (j)
               cierra1_rel(i);
         }
         else
            calcula_i(*ptr,O_NOR);
         ptr++;
      }
      
      for (repe=0;repe < susti->l_repe;repe++)  
      {       /* repeticion de linea  */
         do 
		 {
			hayconper = 1;
			haysinper = 0;
            
            linea[0] = '\0';
            nmemos = 0; /* total activas */
            if (susti->l_ncamp > 0) 
			{
			   hayconper = 0;
			   haysinper = 0;
               for(i=0;i<susti->l_ncamp;i++) 
			   {
				  sexoeuro = 0;
				  eseuro = 0;
                  tiene_permiso = 1;
                  dest[0] = 0;
                  if (susti->l_fich[i] >= 0) 
				  {    /* caso campos del def...   */
                     nvar = susti->l_fich[i] - MAXDEF;
                     if (nvar < 0) 
					 {
                        memo_id = susti->l_fich[i];
                        campo = susti->l_camp[i];
                        if (campo < 0) 
                        {
                           campo = pasa_a_int(_memo[((-campo)-1)].mv.vald);
                        }
                        /*memo_id += (campo * 100);*/
                        
                        /******** no_memo_leer solo se baja cuando las memos no
                        tienen bloque predefinido en cuyo caso todas
                        las memos de la linea deben ser de esta naturaleza */
						if (memo_id == fichero_texto && campo == campo_texto)
						{
							nlintexto++;
							saca_lin_texto(dest,fichero_texto,campo_texto,nlintexto,modolintexto);
						}
                        else if (no_memo_leer)
                        {
						   tmpt1 = v_mpfi(memo_id)->f_campos[campo].c_maximo;
						   if ((tmpt1 = strstr(tmpt1,"[")))
						   {
							   fichero_texto = memo_id;
							   campo_texto = campo;
							   nlintexto = 0;
							   if (strstr(tmpt1,"B"))			
								   modolintexto = 1;
							   else
								   modolintexto = 0;
							   tmpt1 = NULL;
							   saca_lin_texto(dest,fichero_texto,campo_texto,nlintexto,modolintexto);
						   }
						   else
						   {
							   tmpt1 = susti->l_usi[i];
							   if (strlen(tmpt1) > 0)
							   {
								  tmpt1 += (strlen(tmpt1) - 1);
								  if (*tmpt1 == 's' && !v_mpfi(memo_id)->f_campos[campo].c_qc)
								  {
									 tmpt1 = NULL;
								  }
								  else
								  {
									 tmpt2 = v_mpfi(memo_id)->f_campos[campo].c_minimo;
									 if (tmpt2 && strstr(tmpt2,"?EURO") /*&& fgv_ver_euro*/) //*OJO*
									 {
										 tmpt2 = NULL;
										 tmpt1 = NULL;
									 }
									 else
									 {
										 tmpt1 = v_mpfi(memo_id)->f_campos[campo].c_usi;
										 v_mpfi(memo_id)->f_campos[campo].c_minimo = "";
										 v_mpfi(memo_id)->f_campos[campo].c_usi = susti->l_usi[i];
									 }
								  }
							   }
							   else
								  tmpt1 = NULL;
							   modo_otro_euro++;
							   campo_euro_localizado = 0;
							   ld_campo(campo,v_mpfi(memo_id),dest);							   
							   if (_imp == 0x00000004L)
							   {
								   char *xp = dest;
								   if (v_mpfi(memo_id)->f_campos[campo].c_tipo == CHARTIPO)
								   {
									   while(*xp) xp++;
									   *xp++ = 31;
									   *xp = 0;
								   }
								   else if (v_mpfi(memo_id)->f_campos[campo].c_tipo == FECHATIPO)
								   {									   
									   while(*xp)
									   {
										   if (*xp == '.') *xp = '/';
										   xp++;
									   }
									   *xp++ = 30;
									   *xp = 0;
								   }
							   }
							   if (campo_euro_localizado)
							   {								     
									 if (fgv_ver_euro && asigna_euro)
									 {
										 sexoeuro = 1;
									 }
									 if (asigna_euro)
									 {
										 double conv_anal = 1.0;

										 (*asigna_euro)("EURO",&conv_anal);
										 conv_anal = redondea(conv_anal,3);									 
										 
										 
										 /*
										 {
											 char tmp[512];
											 sprintf(tmp,"conversion en %.4lf",conv_anal);
											 aviso(0,tmp);
										 }
										 */
										 
										 if (conv_anal == 166.386)
										 {
											if (fgv_ver_euro)
											{
												eseuro = 0;
												/*aviso(0,"1-eseuro=0");*/
											}
											else
												eseuro = 1;
										 }
										 else if (fgv_ver_euro)
										 {
											eseuro = 1;
										 }
										 else
										 {
											/*aviso(0,"2-eseuro=0");*/
											eseuro = 0;
										 }

									 }
							   }
							   modo_otro_euro--;
							   if (hay_dat_permiso(v_mpfi(memo_id),campo))
							   {
								  tiene_permiso = 1;
								  hayconper = 1;
							   }
							   else
							   {
								  memset(dest,' ',strlen(dest));
								  tiene_permiso = 0;
								  haysinper = 1;
							   }
							   if (tmpt1)
							   {
								  v_mpfi(memo_id)->f_campos[campo].c_usi = tmpt1;
								  v_mpfi(memo_id)->f_campos[campo].c_minimo = tmpt2;
							   }
						   }
                        }
                        if ((j = v_mpfi(memo_id)->f_campos[campo].c_tipo) == CHARTIPO || j == FECHATIPO) 
                        {
                           nvar = 0;
                           for (j = 0;dest[j] != '\0';j++)
                           {
                              if (dest[j] != ' ' && dest[j] != 31 && dest[j] != 30) {nvar = -1;break;}
                           }
                        }
                        else
                           if (v_mpfi(memo_id)->f_campos[campo].c_qc == -2) { /* caso MEMO */
                              memo_att = 1; /* hay memos en esta linea */
                              if ((susti->l_p[i] % 10) == 1)
                                 memo_repe = 1;
                              nvar = -1;
                              nmemo = -1;
                              for (j = 0; j < 255;j++) {
                                 if (nmemo == -1 && qmemo[j] == -1)
                                    nmemo = j;
                                 if (qmemo[j] == memo_id && qmemo2[j] == campo) {
                                    nmemo = j;
                                    break;
                                 }
                              }
                              if (nmemo == -1)
                                 continue; /* saltar este campo */
                              
                              if (qmemo[nmemo] == -1 && memos[nmemo]) {
                                 rw_error(40);
                                 /* printf("error interno inme01\n\r"); */
                              }
                              if (qmemo[nmemo] == -1) {
                                 strcpy(pas,v_mpfi(memo_id)->f_campos[campo].c_mante.c_defecto);
                                 trata_memo(cambia_path_fich(pas),
                                    atol(dest),104,0,0,0,9999,"",0,memos+nmemo,0);
                              }
                              qmemo[nmemo] = memo_id;
							  qmemo2[nmemo] = campo;
                              if (memos[nmemo]) {
                                 if (*(memos[nmemo] + cmemo[nmemo])) {
                                    strcpy(dest,*(memos[nmemo] + cmemo[nmemo]));
                                    if (!repe || memo_repe)
                                       cmemo[nmemo]++;
                                    nmemos++;
                                 }
                                 else { /* desactivar */
                                    libera_texto(memos[nmemo]);
                                    memos[nmemo] = NULL;
                                    strcpy(dest,"");
                                 }
                              }
                              else
                              {
                                 strcpy(dest,"");
                              }
                           }
                           else {
                           /* quitar blancos delante para evitar el problema
                              de los numeros en blanco cuando hay pocas mascaras */                              
							  quitabp(dest);
                              nvar = ( (rwatof(dest) == (double)0) ? 0 : -1 );
                              if (_imp != 0x00000004L && !pb->pfi)
							  {
								 if ((susti->l_p[i] % 10) != 1) 
                                     poncoma(dest,NULL); /* solo poner la coma!!! */
							  }
                           }
                        }
                        else {
                           if (nvar == 0) {
                              if (no_memo_leer) {
                                 sprintf(dest,"%d",npag);
                                 nvar = -1;
                              }
                           }
                           else {
                              impre_quecontrol(nvar);
                              for (jk = 0,jk1=0;jk < impre_nccont;jk++,jk1++)
                              {
                                 if (impre_ccont[jk] == 0 || impre_ccont[jk] == 27)
                                 {
                                    dest[jk1] = 27;
                                    jk1++;
                                    sprintf(dest+jk1,"%03d",impre_ccont[jk]);
                                    jk1 += 2;
                                 }
                                 else
                                    dest[jk1] = impre_ccont[jk];
                              }
                              if (nvar == 9) {
                                 for (nvar = 2;nvar < 7;nvar++) {
                                    impre_quecontrol(nvar);
                                    for (jk = 0;jk < impre_nccont;jk++,jk1++)
                                    {
                                       if (impre_ccont[jk] == 0 || impre_ccont[jk] == 27)
                                       {
                                          dest[jk1] = 27;
                                          jk1++;
                                          sprintf(dest+jk1,"%03d",impre_ccont[jk]);
                                          jk1 += 2;
                                       }
                                       else
                                          dest[jk1] = impre_ccont[jk];
                                    }
                                 }
                              }
                              dest[jk1] = 0;
                           }
                        }
                         }                                   /* del def.                 */
                         else 
						 {                                 /* caso variable de calculo */
                            nvar = susti->l_camp[i];
                            if (_memo[nvar].m_tipo == CHARTIPO ||
                               _memo[nvar].m_tipo == FECHATIPO) 
							{
                               if (no_memo_leer)
							   {
                                   sprintf(dest,susti->l_usi[i],_memo[nvar].mv.valc);
								   if (_imp == 0x00000004L)
								   {
									   char *xp = dest;

									   while(*xp) xp++;
									   if (_memo[nvar].m_tipo == FECHATIPO)
										   *xp++ = 30;
									   else
										   *xp++ = 31;
									   *xp = 0;
								   }
							   }
                               nvar = 0;
                               for (j = 0;dest[j] != '\0';j++)
                                  if (dest[j] != ' ' && dest[j] != 31 && dest[j] != 30) {nvar = -1;break;}
                            }
                            else {
                               if (no_memo_leer)
							   {
								  sprintf(dest,susti->l_usi[i],_memo[nvar].mv.vald);
							      if (decimales(dest) == 8 && asigna_decimales)
								  {										 
									 int nd;
									 char otrusi[100];
									 double tmpd = _memo[nvar].mv.vald;									 
									 if (fgv_ver_euro && asigna_euro)
									 {							 
							             (*asigna_euro)("EURO",&tmpd);
										 sexoeuro = 1;
									 }
									 if (asigna_euro)
									 {
										 double conv_anal = 1.0;
							             (*asigna_euro)("EURO",&conv_anal);
										 conv_anal = redondea(conv_anal,3);
										 
										 
										 /*
										 {
											 char tmp[512];
											 sprintf(tmp,"conversion en %.4lf",conv_anal);
											 aviso(0,tmp);
										 }
										 */
										 

										 if (conv_anal == 166.386)
										 {
											if (fgv_ver_euro)
											{
												eseuro = 0;
												/*aviso(0,"3-eseuro=0");*/
											}
											else
												eseuro = 1;
										 }
										 else if (fgv_ver_euro)
										 {
											eseuro = 1;
										 }
										 else
										 {
											eseuro = 0;
											/*aviso(0,"4-eseuro=0");*/
										 }
									 }

									 /* analizar conversion para ver si es euro */
									 modo_otro_euro++;
									 nd = (*asigna_decimales)("EURO");
									 modo_otro_euro--;
									 strcpy(otrusi,susti->l_usi[i]);
									 {
									     char *ppp = strstr(otrusi,".8");
										 if (ppp)
										 {
											 ppp[1] = '0';
										 }
									     sprintf(dest,otrusi/*susti->l_usi[i]*/,(double)0.0);
									 }
									 /*
									     free(susti->l_usi[i]);
									     susti->l_usi[i] = malloc(100);
									  */
									 sprintf(otrusi,"%%%d.%dlf",strlen(dest),nd);
									 tmpd = redondea(tmpd,nd);
									 sprintf(dest,otrusi,tmpd);
								  }
                                  if (_imp != 0x00000004L && !pb->pfi)
								  {
									 if ((susti->l_p[i] % 10) != 1) 
									 	poncoma(dest,NULL); /* solo poner la coma!!! */
								  }
                               }
                               nvar = ( (rwatof(dest) == (double)0) ? 0 : -1 );
                            }
                         }
                         
                         if (no_memo_leer) 
						 {
                            if ((susti->l_p[i] % 10) > 1) 
							{
								int fff = susti->l_p[i] % 10;
								/*
								if (sexoeuro && fff != 9 )
								{
									if ((fff%2))
									{
										fff--;
									}
									else
									{
										if (fff < 8)
											fff++;
									}									
								}
								*/								
                                letras(dest,(double)rwatof(dest),fff-2,eseuro,ndecieuro);
                            }
                            if ((susti->l_p[i] % 10) == 1) 
							{  /* el formato es con puntos */
                               j = strlen(dest);
                               if (_imp != 0x00000004L && !pb->pfi)
                                  poncoma(pas,dest);
                               else
                                  strcpy(pas,dest);
                               sprintf(pas+200,"%%%ds",j); /* restaurar con longitud original */
                               sprintf(dest,pas+200,pas);
                            }
                            
                            if (((susti->l_p[i] / 10)%10) == 2) 
							{
                               for (j=0;dest[j] == ' ';j++)
                                  dest[j] = '*';
                            }
                         }
                         
                         
                         for (j=0;j< (susti->l_s[i]);j++)   /* espaciado */
                            strcat(linea," ");
                         
                         
                         
						 if (pb->pfi)
						 {
							 strcpy(pas,dest);
						 }
						 else
						 {
							 if (_imp == 0x00000004L && es_inf_cuerpo)
							 {
								marca_campo_para_export(pas,susti->l_form[i],dest);
								/*
								if (dest[0] == ' ')
								dest[0] = 8;
								else
								strcatd(dest,"\008");
								if (dest[0] && dest[strlen(dest)-1] == ' ')
								{
								dest[strlen(dest)-1] = 8;
								}
								else	   
								strcat(dest,"\008");
								*/
							 }
							 else
								sprintf(pas,susti->l_form[i],dest);
						 }
                         
                         if (tiene_permiso)
                         {
							hayconper = 1;
                            if ( !((susti->l_p[i] / 10)%10) )
                               nvar = -1; /* si el cero es valido forzar impresion */
                            if ((j = susti->l_cond[i]) > -1 && condicion_i(j) != 0)
                               nvar = 0;
                         }
                         else
                            nvar = 0;

                         if (!tiene_permiso ||  (j > -1  && nvar == 0) || (((susti->l_p[i] / 10)%10) == 1 && nvar == 0) )
                         {
                            for (j = 0;pas[j];j++)
                            {
                               if (_imp != 0x00000004L || pas[j] != 9)
                                  pas[j] = ' ';
                            }
                         }
						 if (pb->pfi)
						 {
							 if (susti->l_pfi && susti->l_pfi[i] != -1)
							 {
								 st_campo(susti->l_pfi[i],pb->pfi,pas);
							 }
						 }
						 else
						 {
							 if (pb->i_word >= 0)
							 {
								 strcpy(susti->w_datos[i],pas);
							 }
							 else
							 {
								 strcat(linea,pas);
							 }
						 }
                  }
         }
         else
		 {
            sprintf(linea,susti->l__usi,susti->l__lin);
         }
        
		 if (!haysinper)
			 hayconper = 1;

		 if (hayconper || _tipopermiso != 1)
		 {	
			 if (!pb->pfi)
			 { 
				 if (pb->i_word < 0)
				 {			 
					printa_linea(linea);
				 }
				 else
				 {
					  /* seleccionar bloque, añadirlo al documento nuevo y reemplazar los campos */
					  sprintf(pas,"%d\001&%d\001%d",pb->i_word,susti->w_ri,susti->w_rf);
					  (*DespachadorExterno)((int)DESPACHO_ENLACEWORD,6,pas);				  
					  if (es_la_primera && pb->i_exacto == 3)
					  {
						  es_la_primera = 0;
						  sprintf(pas,"#%d\001=\001+>%d",pb->i_word,pb->i_ri);
					  }
					  else
					  {
						  sprintf(pas,"#%d\001=\001+",pb->i_word);
					  }
					  (*DespachadorExterno)((int)DESPACHO_ENLACEWORD,6,pas);
					  /* sustituir los campos */
					  for(i=0;i<susti->l_ncamp;i++) 
					  {
						  sprintf(pas,"#%d\001#%d\001%s",pb->i_word,susti->w_indice[i],susti->w_datos[i]);
						  (*DespachadorExterno)((int)DESPACHO_ENLACEWORD,4,pas);
					  }
				 }
			 }
			 numlin++ ;
		 }
         
         if (memo_repe) 
		 {
            no_memo_leer = 0;
         }
     } while(memo_repe && nmemos);
     
     if (memo_repe) 
	 {
        no_memo_leer = 1;
        memo_repe = 0;
     }

	 if (pb->pfi)
		 break;
     
     } /* del for de repe */
     
     if (!memo_att && nmemos) 
	 {
        for (j = 0;j < 255;j++) {
           if (memos[j]) {
              libera_texto(memos[j]);
              memos[j] = NULL;
              nmemos--;
           }
        }
        memo_att = 1;
     }
     if (memo_att && !nmemos) 
	 {
        for (j = 0;j < 255;j++) 
		{
           qmemo[j] = -1;
		   qmemo2[j] = -1;
           cmemo[j] = 0;
        }
     }
     if (nmemos < 0) {
        rw_error(41);
        /* printf("\n\r error interno inme02 \n\r");
        getchar();*/
     }
     
     for (j=0;j<pb->b_interlin - 1;j++)  
	 {		
		 if (!(pb->i_word))
		 {
			printa_linea(linea_vacia);                   /* control interlineado */
		 }
        numlin++ ;
     }
   } /* del for de lineas */

   if (pb->i_exacto == 11)
   {
	   /* falta incluir los valores del rayado */
	   sprintf(pas,"#%d\001>%d\001/%d/%d",pb->i_word,pb->i_ri,0,0);
	   (*DespachadorExterno)((int)DESPACHO_ENLACEWORD,6,pas);
	   pb->i_exacto = 1;
   }
   
   if (nmemos)
      for (j = 0;j < 255;j++) 
	  {
         if (memos[j])
            libera_texto(memos[j]);
      }
      
      return(numlin);
}

/*  ========================================================================
libera_bloq(pb)
=======================================================================*/

int libera_bloq(pb)
BLOQUE_INF *pb;
{
   int k,i,j;
   LINEA_INF *susti;
   
   if (pb == (BLOQUE_INF *)0 ) return(0);
   for (k=0;k<pb->b_nlin;k++) {
      susti = pb->b_lin[k];
      for (i=0;i<susti->l_ncamp;i++) {
         free((char *)susti->l_usi[i]);
         free((char *)susti->l_form[i]);
      }
	  if (pb->i_word >= 0)
	  {
		  free((char *)susti->w_indice);
		  if (susti->w_datos)
		  {
			  for (j = 0;susti->w_datos[j];j++)
			  {
				  free(susti->w_datos[j]);
			  }
			  free((char *)susti->w_datos);
		  }
	  }
      free ((char *)susti->l_camp);
      free ((char *)susti->l_fich);
      free ((char *)susti->l_p);
      free ((char *)susti->l_s);
      free ((char *)susti->l_cond);
      free ((char *)susti->l_form);
      free ((char *)susti->l_usi);
      free ((char *)susti->l_cal);

	  if (susti->l_pfi)
	  {
		  free ((char *)susti->l_pfi);
	  }

      
      if (pb->b_lin[k]->l_ncamp == 0) {
         free((char *)susti->l__usi);
         free((char *)susti->l__lin);
      }
      free((char *)susti);
   }
   free((char *)pb->b_lin);
   free((char *)pb);
   return(0);
}

/*  ========================================================================
libera_inf(pin)  libera la mamoria del informe
=======================================================================*/

libera_inf(pin)
INFORME *pin;
{
   int i;
   
   if (pin == NULL) return(-1);
   libera_bloq(pin->i_cabecera);
   if (pin->i_cuerpo != (CUERPO_INF *)0 ){
      libera_bloq(pin->i_cuerpo->c_bloq);
      for(i=0;i<pin->i_cuerpo->c_nsub;i++) 
	  {
         libera_bloq(pin->i_cuerpo->c_sub[i]->s_bloq);
         free((char*)pin->i_cuerpo->c_sub[i]);
      }
      free((char*)pin->i_cuerpo);
			}
   libera_bloq(pin->i_pie);
   libera_bloq(pin->i_salto);
   
   for (;totalmemos > 0;totalmemos--)
      free((char *)memos[totalmemos-1]);
   free((char *)memos);
   for (i = 0;i < _nmemo;i++) {
      if (_memo[i].m_tipo == CHARTIPO || _memo[i].m_tipo == FECHATIPO) {
         free(_memo[i].mv.valc);
      }
			}
   free((char *)_memo);
   _nmemo = 0;
   free((char *)_oper);
   _noper = 0;
   free((char *)_cond);
   _ncond = 0;  

   if (pin->i_tipo_informe == 1)
   {	   
	   if (!(pin->i_buffer_word))
	   {
		   pin->i_buffer_word = malloc(512);
	   }
		/* salir */

	   sprintf(pin->i_buffer_word,"#%d",pin->w_handle1);
	   (*DespachadorExterno)((int)DESPACHO_ENLACEWORD,1,pin->i_buffer_word);

	   /*if (!_imp)*/
	   {
		   sprintf(pin->i_buffer_word,"%d\001VISIBLE\001SI",pin->w_handle1);
		   (*DespachadorExterno)((int)DESPACHO_ENLACEWORD,2,pin->i_buffer_word);
		   
	       sprintf(pin->i_buffer_word,"%d\003",pin->w_handle1);
		   (*DespachadorExterno)((int)DESPACHO_ENLACEWORD,1,pin->i_buffer_word);
	   }
	   /*
	   else
	   {	   
		   sprintf(pin->i_buffer_word,"%d",pin->w_handle1);
		   (*DespachadorExterno)((int)DESPACHO_ENLACEWORD,1,pin->i_buffer_word);
	   }
	   */
	   free(pin->i_buffer_word);
   }

   if (pin->pfi_cabecera)
	   free_defpri(pin->pfi_cabecera);

   if (pin->pfi_cuerpo)
	   free_defpri(pin->pfi_cuerpo);

   free ((char*)pin);
   return(0);
}

printa_linea_10(linea,modo)
char *linea;
int modo;
{
int i,j,k,h,numcon;
char tmp[2000];
int sin_lf = 0;
static int lin = -1;
         
         if (modo == 4)
         {
            sin_lf = 1;
            modo = 0;
         }
         
         if (lin == -1)
         {
            lin = INICIO_LIS;
         }
         
         if (modo && linea == (char *)0) {lin = INICIO_LIS;return(0);}
         
         if (modo && vez == -1) return(0);
         
            if (_imp != 0x00000004L) 
               quitab(linea);
            k = strlen(linea);
            for (i = 0,j =0;i < k;i++)
            {
               if ((k-i) > 4 && linea[i] == '{' && linea[i+1] == '{' &&
                  linea[i+3] == '}' && linea[i+4] == '}')
               {
                  switch(linea[i+2])
                  {
                  case 'C':
                     numcon = 1;
                     break;
                  case 'c':
                     numcon = 2;
                     break;
                  case 'N':
                     numcon = 3;
                     break;
                  case 'n':
                     numcon = 4;
                     break;
                  case 'E':
                     numcon = 5;
                     break;
                  case 'e':
                     numcon = 6;
                     break;
                  case 'P':
                     numcon = 7;
                     break;
                  case 'O':
                     numcon = 8;
                     break;
                  default:
                     numcon = -1;
                     break;
                  }
                  if (numcon != -1)
                  {
                     impre_quecontrol(numcon);
                     memcpy(tmp+j,impre_ccont,impre_nccont);
                     j += (impre_nccont-1);
                  }
                  else
                     j--;
                  i += 4;
               }
               else
                  if (linea[i] == 27)
                  {
                     i++;
                     h = linea[i+3];
                     linea[i+3] = 0;
                     tmp[j] = atoi(linea+i);
                     linea[i+3] = h;
                     i += 2;
                  }
                  else
                     tmp[j] = linea[i];
                  j++;
            }
            if (!sin_lf)
            {
#ifndef ESDOS
               tmp[j] = '\r';
               j++;
#endif
               tmp[j] = '\n';
               j++;
            }
            tmp[j] = 0;
            tmp[j+1] = j;
            _graba_linea(tmp,_imp,j+2);
         return(0);
}



printa_linea_(linea,modo)
char *linea;
int modo;
{
int i,j,k,h,numcon;
char tmp[2000];
int sin_lf = 0;
static int lin = -1;

         if (Pseudo10)
		 {
			return printa_linea_10(linea,modo);
		 }
         
         if (modo == 4)
         {
            sin_lf = 1;
            modo = 0;
         }
         
         if (lin == -1)
         {
            lin = INICIO_LIS;
         }
         
         if (modo && linea == (char *)0) {lin = INICIO_LIS;return(0);}
         
         if (modo && vez == -1) return(0);
         
         if (_imp != (FILE *)0)
         {
            if (_imp != 0x00000004L) 
               quitab(linea);
            k = strlen(linea);
            for (i = 0,j =0;i < k;i++)
            {
               if ((k-i) > 4 && linea[i] == '{' && linea[i+1] == '{' &&
                  linea[i+3] == '}' && linea[i+4] == '}')
               {
                  switch(linea[i+2])
                  {
                  case 'C':
                     numcon = 1;
                     break;
                  case 'c':
                     numcon = 2;
                     break;
                  case 'N':
                     numcon = 3;
                     break;
                  case 'n':
                     numcon = 4;
                     break;
                  case 'E':
                     numcon = 5;
                     break;
                  case 'e':
                     numcon = 6;
                     break;
                  case 'P':
                     numcon = 7;
                     break;
                  case 'O':
                     numcon = 8;
                     break;
                  default:
                     numcon = -1;
                     break;
                  }
                  if (numcon != -1)
                  {
                     impre_quecontrol(numcon);
                     memcpy(tmp+j,impre_ccont,impre_nccont);
                     j += (impre_nccont-1);
                  }
                  else
                     j--;
                  i += 4;
               }
               else
                  if (linea[i] == 27)
                  {
                     i++;
                     h = linea[i+3];
                     linea[i+3] = 0;
                     tmp[j] = atoi(linea+i);
                     linea[i+3] = h;
                     i += 2;
                  }
                  else
                     tmp[j] = linea[i];
                  j++;
            }
            if (!sin_lf)
            {
#ifndef ESDOS
               tmp[j] = '\r';
               j++;
#endif
               tmp[j] = '\n';
               j++;
            }
            tmp[j] = 0;
            tmp[j+1] = j;
            _graba_linea(tmp,_imp,j+2);
         }
         else {
            if (!modo) return(0);
            
            if (lin == INICIO_LIS) {
               cls();
            }
            if (strlen(linea) > 80) linea[79] = '\0';
            if (es_inf_cuerpo)
            {
               Ai();
               Patrib |= 128;
            }
            pprint(lin,linea);
            if (es_inf_cuerpo)
            {
               Patrib &= ~128;
               Bi();
            }
            lin += 100;
            if (lin > 2301) {
               i = parada();
               if (i == 1 || i == 7)
                  vez = -1;
               lin = INICIO_LIS;
            }
         }
         return(0);
}

printa_linea(linea)
char *linea;
{
   return(printa_linea_(linea,1));
}

static int _pinta_rayado(in,quedan)
INFORME *in;
int quedan;
{
   int anchura = in->i_rayado;
   int inicio = in->i_inirayado;
   int i,j;
   char buf[512];

   if (in->i_tipo_informe == 1)
   {	   
	   /* solo se puede hacer una vez completado el pie */
	   if (in->i_pie->i_exacto == 1)
			in->i_pie->i_exacto = 11;
   }
   else
   {
	   for (i = 0;i < quedan;i++)
	   {
		  memset(buf,' ',anchura+inicio);
		  buf[anchura+inicio] = 0;
		  if (!i)
		  {
			 for (j = 0;j < ((anchura-quedan-2)/2);j++)
			 {
				buf[inicio+j] = '-';
			 }
		  }
		  j = ( (anchura-quedan-2)/2 ) + i;
		  buf[inicio+j] = '\\';
		  j++;
		  if (i == (quedan-1))
		  {
			 while(j < anchura)
			 {
				buf[inicio+j] = '-';
				j++;
			 }
		  }
		  printa_linea(buf);
	   }
   }
   return(quedan);
}


int quizasalto_pag(in,linblo,npag,nlinea)
INFORME *in;
int linblo,*npag,nlinea;
{
			int quedan;
         
         if (_imp == 0x00000004L || !_imp) return(nlinea);
         
         /* quedan son las lineas disponibles para el cuerpo */
         quedan = in->i_cabecera->totalbloque + in->i_cuerpo->totalcuerpo - nlinea;
         if (in->i_nlin > -1 && quedan < linblo) 
		 {
			if (in->i_tipo_informe == 0)
			{
				if (!(in->i_especial))
				{
				   for (;quedan > 0;quedan--) {
					  printa_linea(linea_vacia);
					  nlinea++;
				   }
				}
			}
            nlinea += prin_bloque(in->i_salto,*npag);

			if (in->i_tipo_informe == 0)			
			{
				if (!_imp || i_saltopag(_imp) == -1)
				{
				   for (;nlinea < in->i_nlin;nlinea++)
					  printa_linea(linea_vacia);
				}
			}
			else
			{
				  char pas[300];
				  sprintf(pas,"#%d\001=\001+=%c",in->w_handle1,12);
				  (*DespachadorExterno)((int)DESPACHO_ENLACEWORD,6,pas);
			}
            (*npag)++ ;
#ifdef RWXARXA
#ifdef RWSERVER
            if (en_conexion_externa && in->i_tipo_informe == 0)
            {
               if (remoto_es_sistema_grafico)
               {
                  printa_grafico_en_terminal(nom_inf);
               }
            }
#endif
#endif
#ifdef FGVWIN
#ifdef RWXARXA
#ifdef RWSERVER
            if (!en_conexion_externa)
#endif				
#endif
               if(_imp == (FILE *)3L && in->i_tipo_informe == 0)
                  printa_grafico(nom_inf);
#endif
               
               nlinea = prin_bloque(in->i_cabecera,*npag);
				if (in->i_tipo_informe == 1)			
				{
				   char pas[300];
   		           sprintf(pas,"#%d\001>%d\001%d",in->i_cuerpo->c_bloq->i_word,in->i_cuerpo->c_bloq->i_ri,in->i_cuerpo->c_bloq->i_rf);
		           (*DespachadorExterno)((int)DESPACHO_ENLACEWORD,6,pas);
				}

         }

         return(nlinea);
}

static libera_extradef(onfi)
int onfi;
{
   int recur = (recur_punto < 1) ? 0 : recur_punto * MAXDEF;
   
   while(nfi > onfi) {
      nfi--;
      free(s_d[recur+nfi].nombre_fichero);
      free(s_d[recur+nfi].enlaza_pfi);
      if (mpfi[nfi])
						{ 
         free_defpri(mpfi[nfi]);
         mpfi[nfi] = NULL;
						}
      else
						{
         if (s_d[nfi+recur].buf)  /* puede no estar inicializado */
         {
            free(s_d[nfi+recur].buf);
            s_d[nfi+recur].buf = NULL;
			s_d[nfi+recur].tam_buf = 0;
         }
						}
      if (s_d[nfi+recur].salvadatos)
						{
         free(s_d[nfi+recur].salvadatos);
         s_d[nfi+recur].salvadatos = NULL;
						}
      free((char *)mpfirel[nfi]);
      mpfirel[nfi] = NULL;
      if (nfi == 49) nfi--;
   }
   
   return(0);
}



/*  ========================================================================
rw_inf(p) rutina basica de impresion. En cada llamada imprimira
lo que toque (cabecera, linea o bloque basico ...)
llevando control del numero de linea, condiciones, etc
TINFO  para leer el .inf.
TPRINT para imprimir.
TFINF  para finalizar y liberar.
TFLIF  para liberar el informe
========================================================================*/

extern void *el_informe = NULL;

printa_control(nvar)
int nvar;
{
   INFORME *in;
   if (_imp == (FILE *)0) return(0);
   switch(nvar) {
   case 1:
      i_oncomprime(_imp);
      break;
   case 2:
      i_offcomprime(_imp);
      break;
   case 3:
      i_onnegrita(_imp);
      break;
   case 4:
      i_offnegrita(_imp);
      break;
   case 5:
      i_onexpande(_imp);
      break;
   case 6:
      i_offexpande(_imp);
      break;
   case 7:
      i_saltopag(_imp);
      break;
   case 8:
      i_otrocar(_imp);
      break;
   case 10:
      return(i_anchocarro);
   case 11:
      if (el_informe)
      {
         in = (INFORME *)el_informe;
         return(in->i_nlin);
      }
   case 12:
      return(i_nlineas);
   default:
      i_reset(_imp);
      break;
   }
   return(0);
}

static int nlinea;

agi_lineainforme(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int d,o = prog[p].indice;
   p++;
   d = prog[p].indice;
   if (!pasa_a_int(v_memo(o)->numerica))
   {
      v_memo(d)->numerica = (double)nlinea;
   }
   else
   {
      nlinea = pasa_a_int(v_memo(d)->numerica);
   }
   n_np = 2;
   return(0);
}


int rw_inf(p)
int p;
{
   INFORME *in = (INFORME *)el_informe;
   int quedan,linblo;
   int i,j,k,r = 0;
   int condi[MAXBLOQ];
   int usubpie,usubcab;
   char *nom;
   char tmp1[300];
   char tmp2[300];		  
   static int npag ;
   static int onfi = 0;

   
   ultimo_ipfi = -1;
   
   if (p == -1) {
      if (in) {
									if (onfi)
                              onfi = libera_extradef(onfi);
                           libera_inf(in);
                           in = NULL;
                           el_informe = NULL;
      }
      return(0);
   }
   
   if (programa_activo == -1)
   {
      printf(__Texto[274]);
      getchar();
      return(0);
   }
   
   nom = ((prog[p].i == TINFO) ? v_memo(prog[p].indice)->alfabetica : v_memo(5004)->alfabetica);
   
   switch (prog[p].i)  {
      
   case TINFO:
      
      if (in != NULL) {r = -2;break;}
      
      if (onfi)
         onfi = libera_extradef(onfi);
      onfi = nfi;

      if ( (in = leo_inf(nom)) == NULL ) 
	  {
         r = -1;break;
      }
      el_informe = (void *)in;
      npag = 1;
      break;
      
   case TPRINT:
      if (!in) {r = -2;break;}
      
      if (pausa_fin()) {vez = 0;nlinea = 0;printa_linea((char *)0);r = -1;break;}
      if (nlinea == 0)            
	  {
		  if (in->i_w_para_salto)
		  {		  
			  char pas[300];
			  sprintf(pas,"#%d\001=\001+=%c",in->w_handle1,12);
			  (*DespachadorExterno)((int)DESPACHO_ENLACEWORD,6,pas);
			  in->i_w_para_salto = 0;
		  }

         nlinea = prin_bloque(in->i_cabecera,npag);
		 
         if (nombredev[0] == 2 && nombredev[1] == 0 && ((nombredev[2] == '!' && nombredev[3] == '!') || (nombredev[2] == '|' && nombredev[3] == '|')))
		 {
			 char *pp;
			 pp = nombredev+5;
			 while(*pp && *pp != '*') pp++;
			 if (!*pp)
			 {
					*pp = '*';
					*(pp+1) = 0;
			 }
			 strcpy(numerofax,nombredev+4);
			 /*aviso(0,numerofax);*/
			 if (nombredev[2] == '!' && nombredev[3] == '!')
			 {
				 if (in->ipfifax > -1 && in->campofax > -1)
				 {
					tmp1[0] = nombredev[4];
					tmp1[1] = 0;
					ld_campo(in->campofax,v_mpfi(in->ipfifax),tmp1+1);
					quitab(tmp1);
					if (tmp1[1])
					{
						if (nombredev[4] == '!' || nombredev[5] == '*')
						{
							strcat(tmp1,pp);
							strcpy(numerofax,tmp1);
							/*aviso(0,numerofax);*/
						}
					}
				 }
			 }
			 tmp2[0] = 0;
			 pp = numerofax+1;
			 while(*pp && *pp != '&') pp++;
			 if (*pp == '&')
			 {				 
				 strcpy(tmp2,pp);
				 *pp = 0;
			 }
			 pp = numerofax+1;
			 while(*pp && *pp != '*') pp++;
			 if (!*pp)
			 {
					*pp = '*';
					*(pp+1) = 0;
			 }
			 if (in->ipfimail > -1 && in->campomail > -1)
			 {
				tmp1[0] = '*';
				ld_campo(in->campomail,v_mpfi(in->ipfimail),tmp1+1);
				quitab(tmp1);
				if (tmp1[1])
				{					
					if (nombredev[4] == '!' || !*(pp+1))
					{
						*pp = 0;
						strcat(numerofax,tmp1);
						*pp = '*';
						/*aviso(0,numerofax);*/
					}
				}
			 }
			 strcat(numerofax,tmp2);
		 }
		 if (in->i_tipo_informe == 1)
		 {
		   char pas[300];
   		   sprintf(pas,"#%d\001>%d\001%d",in->i_cuerpo->c_bloq->i_word,in->i_cuerpo->c_bloq->i_ri,in->i_cuerpo->c_bloq->i_rf);
		   (*DespachadorExterno)((int)DESPACHO_ENLACEWORD,6,pas);
		 }
	  }
      
      if (vez == 0) 
	  {
#ifdef RWXARXA
#ifdef RWSERVER
         if (en_conexion_externa && in->i_tipo_informe == 0)
         {
            if (remoto_es_sistema_grafico)
			{
               printa_grafico_en_terminal(nom_inf);
			}
         }                 
#endif
#endif
#ifdef FGVWIN
#ifdef RWXARXA
#ifdef RWSERVER
         else
#endif
#endif
            if( _imp == (FILE *)3L && in->i_tipo_informe == 0)
               printa_grafico(nom_inf);
#endif

            if (in->i_cuerpo)
			{
				for (j=0;j<in->i_cuerpo->c_nsub;j++) 
				{
				   if( in->i_cuerpo->c_sub[j]->cuando == 0 )      /* subcabeceras */
					  nlinea += prin_bloque(in->i_cuerpo->c_sub[j]->s_bloq,npag);
													}
			}
      }
      else 
	  {
         i=0;
		 usubpie = -1;
		 usubcab = -1;
         if (in->i_cuerpo)
		 {
			 for (k=0;k<in->i_cuerpo->c_nsub;k++)
			 {   /* control condiciones */
				if (condicion_i(in->i_cuerpo->c_sub[k]->s_cond) == 0 ) 
				{
					if (!memcmp(in->i_fich,"[lis]",5))
					{					
						if (in->i_cuerpo->c_sub[k]->cuando == 0)
						{
							if (usubcab == -1)
							{
								usubcab = k;
							}
						}
						if (in->i_cuerpo->c_sub[k]->cuando == -1)
						{
							usubpie = k;							
						}
					}
					else
					{
					   condi[i] = k;
					   i++ ;
					}				   
				} /* condi contiene los indices de los subloques a imprimir  */
			 }
			 if (!i)
			 {
				 for (k=0;k<in->i_cuerpo->c_nsub;k++)
				 {
					if (in->i_cuerpo->c_sub[k]->cuando == 0)
					{
						if (usubcab > -1 && k >= usubcab)
						{
							condi[i] = k;
							i++;
						}
					}
					if (in->i_cuerpo->c_sub[k]->cuando == -1)
					{
						if (k <= usubpie)
						{
							condi[i] = k;
							i++;
						}
					}
				 }
			 }
		 }
         
         if (!in->i_cuerpo || (nom && strcmp(nom,"PAGINA") == 0)) {/* solo contar los subtotales */
            j = 0;
            linblo = 0;
         }
         else 
		 {
            j = 1; /* contar todo */
            linblo = in->i_cuerpo->c_bloq->totalbloque;
         }
         if (in->i_cuerpo)
		 {
			 for (k=0;k<i;k++) {
				if (j > in->i_cuerpo->c_sub[condi[k]]->cuando)
				   linblo += in->i_cuerpo->c_sub[condi[k]]->s_bloq->totalbloque;
			 }
		 }
         
         
         nlinea = quizasalto_pag(in,linblo,&npag,nlinea);
         
         
         if (in->i_cuerpo)
		 {
			 for (j=0;j<i;j++) 
			 {                /* se imprimen los subpies      */
				if (in->i_cuerpo->c_sub[condi[j]]->cuando == -1 )
				   nlinea += prin_bloque(in->i_cuerpo->c_sub[condi[j]]->s_bloq,npag);
			 }
		 }
         
         
         if (nom && strcmp(nom,"PAGINA") == 0) /* PROVOCAR SALTO DE PAGINA antes de imprimir*/
            nlinea = quizasalto_pag(in,32000,&npag,nlinea);
         
       
         if (in->i_cuerpo)
		 {
			 for (j=0;j<i;j++) 
			 {                /* se imprimen las subcabeceras */
				if (in->i_cuerpo->c_sub[condi[j]]->cuando == 0 )
				   nlinea += prin_bloque(in->i_cuerpo->c_sub[condi[j]]->s_bloq,npag);
			 }
		 }	
	  }
      if (in->i_cuerpo)
	  {
		  es_inf_cuerpo = 1;
		  nlinea += prin_bloque(in->i_cuerpo->c_bloq,npag);
		  es_inf_cuerpo = 0;
	  }
      if (vez == -1) {vez = 0;nlinea = 0;r = -1;break;}
      vez = 1;
	  if (in->i_tipo_informe == 2 && in->pfi_cuerpo)
	  {
		  st_doble(0,in->pfi_cuerpo,pfi_cabeceras);
          graba_datos_def_temporal(in->pfi_cuerpo);
	  }
      break;
   case TFINF :
      if (!in) {r = -2;break;}
      linblo = 0;
      if (in->i_cuerpo)
	  {
		  for (j=0;j<in->i_cuerpo->c_nsub;j++) 
		  {
			 if( in->i_cuerpo->c_sub[j]->cuando == -1 )
				linblo += in->i_cuerpo->c_sub[j]->s_bloq->totalbloque;
		  }
	  }
      nlinea = quizasalto_pag(in,linblo,&npag,nlinea);
	  if (in->i_cuerpo)
	  {
		  for (j=0;j<in->i_cuerpo->c_nsub;j++) {
			 if( in->i_cuerpo->c_sub[j]->cuando == -1 )
				nlinea += prin_bloque(in->i_cuerpo->c_sub[j]->s_bloq,npag);
		  }
	  }
      quedan = in->i_cabecera->totalbloque+in->i_cuerpo->totalcuerpo - nlinea;
      if (_imp != 0x00000004L && _imp && in->si_juntopie != 'S') 
	  {
         if ( (in->i_rayado) )
         {
            nlinea += _pinta_rayado(in,quedan);
         }
         else if (in->i_tipo_informe == 0)			
		 {
            for (;quedan > 0;quedan--)
			{
               printa_linea(linea_vacia);
               nlinea++;
			}
		 }
      }
      quedan = in->i_nlin - nlinea;
      if (_imp != 0x00000004L && in->i_pie && in->i_nlin > -1 && quedan < in->i_pie->totalbloque) 
      {
		 if (in->i_tipo_informe == 0)
		 {
			if (!_imp || i_saltopag(_imp) == -1)
			{
				 for (j=0;j<quedan;j++) 
				 {
					printa_linea(linea_vacia);
				 }
			}
		 }
		 else
		 {
			  char pas[300];
			  sprintf(pas,"#%d\001=\001+=%c",in->w_handle1,12);
			  (*DespachadorExterno)((int)DESPACHO_ENLACEWORD,6,pas);
		 }
         npag ++ ;
         nlinea = prin_bloque(in->i_cabecera,npag);
      }
      if (in->i_pie)
         nlinea += prin_bloque(in->i_pie,npag);
      if (_imp != 0x00000004L) {
		  if (in->i_tipo_informe == 0)
		  {
			  if (!_imp || i_saltopag(_imp) == -1)
			  {
				 for (;nlinea < in->i_nlin;nlinea++)
					printa_linea(linea_vacia);
			  }
		  }
		  else
		  {
			  in->i_w_para_salto = 1;
			  /*
			  char pas[300];
			  sprintf(pas,"#%d\001=\001+=%c",in->w_handle1,12);
			  (*DespachadorExterno)((int)DESPACHO_ENLACEWORD,6,pas);
			  */
		  }
	  }

	  if (in->i_tipo_informe == 2 && in->pfi_cabecera)
	  {
		  st_doble(0,in->pfi_cabecera,pfi_cabeceras);
          graba_datos_def_temporal(in->pfi_cabecera);
		  pfi_cabeceras += 1.0;
	  }
      nlinea = 0;
      if (vez == -1)
         r = -1;
      vez = 0;
      npag = 1;
      printa_linea((char *)0);
      break;
   case TFLIF:
	   if (!in) {r = -2;break;}
		   
	  if (in->i_tipo_informe == 2)
	  {
		  int imprime = 0;

		  if (in->pfi_cuerpo)
			 cierra_def_temporal(in->pfi_cuerpo);		
		  if (in->pfi_cabecera)
			 cierra_def_temporal(in->pfi_cabecera);		
		  /* enviar rpt */
		  strcpy(tmp1,nom_inf);
		  strcat(tmp1,".rpt");

		  strcpy(tmp2,mi_remoto_dirbass());
		  strcat(tmp2,"crystal/");
		  strcat(tmp2,quita_path(nom_inf));
		  strcat(tmp2,".rpt");

		  externo_enviafich(1,tmp1,tmp2);
		  
		  /* presentarlo */

		  {
			  int var;
		      var = busca_externa("XVAR_DC_IMPRIME");
			  if (var > 0)
			  {
				  if ((int)v_memo(var)->numerica == 1)
					  imprime = 1;
			  }
		  }
		  
		  
		  CargaVInterface("dscristal.dll",(unsigned long)(0x80000000 | (1+imprime)));
		  
	  }
	  if (in->pfi_cuerpo)
		 borra_def_temporal(in->pfi_cuerpo);		
	  if (in->pfi_cabecera)
		 borra_def_temporal(in->pfi_cabecera);

	  if (in->i_tipo_informe == 2)
	  {
		  strcpy(tmp1,mi_remoto_dirbass());
		  strcat(tmp1,"crystal/");
		  strcat(tmp1,quita_path(nom_inf));
		  strcat(tmp1,".rpt");
#ifdef RWSERVER
          if (en_conexion_externa)
          {            
			  /* no se borran ... */
             /*funcion_windows_cliente(16,tmp1,strlen(tmp1)+1);            */
          }	     
		  else
#endif
			 unlink(tmp1);
	  }

      if (onfi)
         onfi = libera_extradef(onfi);
      if (libera_inf(in) == -1) {r = -1;break;}
      in = NULL;
      el_informe = NULL;
   default :
      break;
        }
        
        sprintf(v_memo(5004)->alfabetica,"%d",r);
        return(0);
}

/* fin informes */
/* ============================================================================
*/

/* #ifndef FGVWIN */

extern char *i_nombre();
extern int i_anchocarro;

int fin_printer(im)
FILE *im;
{
   char qo[81];
   extern char finalizo[37];
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa && im == (unsigned long)0x00000003L)
   {
      fin_printer_terminal(im);
      return(0);
   }
#endif
#endif
   if ((unsigned long)im == (unsigned long)0x00000004L)
   {     
      if (el_dll)
      {
         strcpy(qo,finalizo);
         a_alfacalldll(el_dll,"finalizacion",qo);
         a_descargadll(el_dll);
		 
		 finalizo[0] = 0;
		 finalizo[35] = 0;
		 
      }
      externo_dll = NULL;
      el_dll = NULL;
   }
   else
   {
#ifdef ESDOS
      if ((unsigned long)im > (unsigned long)0x00000004L)
         fclose(im);
#ifdef FGVWIN
      else
      {
         fin_w_printer();
      }
#endif
#else
      if ((unsigned long)im != (unsigned long)0x00000003L)
         fclose(im);
#endif
   }
   return(0);
}

int carga_por_dll(qo)
char *qo;
{
   char *p;
   int i;
   extern int a_alfacalldll();
   static char mi_dll[250];
   extern char inicializo[37];		 
   extern unsigned char no_reset;
   extern int i_nlineas;
   extern int i_anchocarro;
   
   externo_dll = a_alfacalldll;
   el_dll = mi_dll;
   strcpy(qo,i_nombre());
   p = qo + 1;   
   for (i = 0;p[i] && p[i] != '}';i++);
   p[i] = 0;
   strcpy(mi_dll,qo+1);
   a_cargadll(mi_dll);   
   sprintf(qo,"%d,%d,%d",i_nlineas,i_anchocarro,no_reset);
   a_alfacalldll(mi_dll,"parametros",qo);
   strcpy(qo,inicializo);
   a_alfacalldll(mi_dll,"inicializacion",qo);
   
   inicializo[0] = 0;
   inicializo[35] = 0;
   
}

/* pregu_impre
* pregunta fichero , titulo , impresora y pagina
* devuelve -1 si el listado no cabe, 0 si va bien
*/

#ifdef FGVWIN
int x_system(const char *command)
{   
        int _catch;
        char *argv[4];

        argv[0] = getenv("COMSPEC");

        argv[1] = "/c";
        argv[2] = (char *) command;
        argv[3] = NULL;

        /* If there is a COMSPEC defined, try spawning the shell */

        if (argv[0])
		{
                if ((_catch = _spawnve(_P_NOWAIT ,argv[0],argv,NULL)) != -1
                || (errno != ENOENT && errno != EACCES))
                        return(_catch);
		}

        /* No COMSPEC so set argv[0] to what COMSPEC should be. */
        argv[0] = ( _osver & 0x8000 ) ? "command.com" : "cmd.exe";

        /* Let the _spawnvpe routine do the path search and spawn. */

        return(_spawnvpe(_P_NOWAIT ,argv[0],argv,NULL));
}
#endif

int _pregu_impre_(t,memo5009,fispo)
int t;
char *memo5009;
char *fispo;
{
		 int qj,i,m = 0;
         char qo[MAXDEFECTO];
         VENT *salva = NULL;
         FILE *strm;
         int especial_servidor = 0;
         int rr = 0,r;
		 char otrospool[128];
#ifdef ESDOS
		 int escom = 0;
		 DCB dcb; 
#endif
         
         
#ifdef RWXARXA
#ifdef RWSERVER
         extern int (*ext_carga_dll)();
#endif
#endif

		 otrospool[0] = 0;

		 linea_vacia[0] = 0;
		 nombredev[0] = 0;

		 if (t == -77)
		 {
			 if (!memo5009) return -1;
			 i_leeimpre((char *)1);
			 i_anchocarro = 132;
			 strcpy(i_nombre(),"*");
			 strcpy(fispo,memo5009);
			 /*
			 strcpy(fispo,rw_directorio("spool/"));
             strcat(fispo,memo5009);
             strcat(fispo,".spo");            
			 */
			 strcpy(_ejspool,"bin/agispool"); 
             /*sprintf(fispo,"%s%03d%03d",rw_directorio("spool/"),user_who,subnumero_de_usuario);
             strcat(fispo,".spo");*/
             unlink(fispo);
			 _fispool[0] = 0;
			 /* strcpy(_fispool,fispo); */
             if ((_imp = fopen(fispo,"w")) == NULL) 
			 {                
                _imp = (FILE *)0;
                pito(121);
                return(-1);
             }
             i_inicio(_imp);
             i_reset(_imp);
             strcpy(ult_fispo,fispo);
             strcpy(ult_fispool,_fispool);
             strcpy(ult_ejspool,_ejspool);

		     return 0;
		 }
#ifdef RWXARXA
#ifdef RWSERVER
         /* esto es trasparente al servidor */
         if (en_conexion_externa)
         {
            ext_carga_dll = carga_por_dll;
            return(pregu_impre_terminal(t,memo5009,fispo));
         }
#endif
#endif
         
         /*#ifdef FGVWIN
         int pedir_w = 0;
         
           if (t >= 0)
           {
           pedir_w = 1;
           t = -(t+1);
           }
#endif */

		 quitab(fispo);
		 if (fispo[0] == 0)
		 {
			 lee_impresora_defecto(fispo);
			 if (fispo[0] == 0)
			 {
				 strcpy(fispo,"Crystal Reports");
			 }
		 }

         if (t < 0) {m = 1;t = ((-t) - 1);}
#ifndef FGVWIN
         if (m == 0) 
         {
            if ((salva = w_ini(1605,2175)) == NULL) return(-1);
            w_saca(salva,stdpan);
            zonablan(1605,2175);
            Ai();
            cuadro(1605,2175);
            Bi();
            cuadro(1706,2074);
            As();
            pprint(1810,__Texto[275]);
            Cs();
         }
#endif
         
         if (m == 0) 
         {
#ifdef FGVWIN			
            if (pregunta_impresion(fispo,_ejspool,rw_user,&especial_servidor,_fispool,otrospool))
            {			   
               if (memo5009) memo5009[0] = 0;
               return(-1);
            }
			{
				extern char queimpresora[512];
				if (memo5009) strcpy(memo5009,queimpresora);
				graba_impresora_defecto(queimpresora);
			}

#else
            do 
            {
               qj = entras(1827,30,fispo,1,qo);
               if (qj == 1 || qj == 7) 
               {
                  w_pinta(salva);
                  w_fin(salva);
                  if (memo5009) memo5009[0] = 0;
                  return(-1);
               }
               quitab(qo);
               if (memo5009) strcpy(memo5009,qo);
               strcatd(qo,rw_directorio("dev/imp/"));
               if ((qj = i_leeimpre(qo)) <= 0) 
               {
                  Ar();
                  aviso(0,__Texto[40]);
                  Cr();
               }
			   graba_impresora_defecto(qo);
            } while (qj <= 0);
#endif
			
         }
         else 
         {
            r = 1;
		/*
#ifdef FGVWIN
            if (DespachadorExterno)
            {				
               (*DespachadorExterno)((int)DESPACHO_FUNCIONES_IMPRESION,0,(void *)&r);
               if (!r)
               { // usar la impresora windows interna
                  i_leeimpre(NULL);
               }
            }
#endif
	 	 */				

            if (r && r != 3)
            {
				if (!memcmp(fispo,"CrystalReport",13))
				{					    
					    if (fispo[13] == ';')
						{
							strcpy(otrospool,fispo+14);
							fispo[13] = 0;							
						}
                        i_leeimpre(NULL);
						fispo[0] = 2;
						fispo[1] = 0;
						{
							extern unsigned char nombredev[81];
							nombredev[0] = 2;
							nombredev[1] = 0;
							strcpy(nombredev+2,otrospool);
						}												
						_fispool[0] = 0;
						_ejspool[0] = 0;
						m = 0;
				}
				else if (!memcmp(fispo,"ExcelEstandard",14))
				{
					    int yaini = 0;
					    i_leeimpre((char *)1);
						if (fispo[14] == '?')
						{
							 extern char inicializo[37];
							 extern char finalizo[37];
							 yaini = 1;
							 strcpy(inicializo,fispo+15);
							 inicializo[35] = strlen(inicializo);
							 strcpy(finalizo,"salva");
							 finalizo[35] = strlen(finalizo);
						}
						strcpy(fispo,i_nombre());
						if (!yaini)
						 {
							extern char inicializo[37];
							(*DespachadorExterno)((int)DESPACHO_FUNCIONES_IMPRESION,4,(void *)inicializo);
							inicializo[35] = strlen(inicializo);
						}
#ifdef RWXARXA
                        if (en_conexion_terminal)
                            especial_servidor = 0;
#endif                        
						_fispool[0] = 0;
						_ejspool[0] = 0;
						m = 0;
				}
				else if (!memcmp(fispo,"PDF",3))
				{
					    if (fispo[3] == ';')
						{
							strcpy(otrospool,"$$");
							if (fispo[4] != '!' && fispo[4] != '-')
								strcat(otrospool,"-");
							strcat(otrospool,fispo+4);
							fispo[3] = 0;
						}
						else
						{
							strcpy(otrospool,"$$-default");
						}
						if (quita_path(otrospool) == otrospool)
						{
#ifdef RWSERVER
							if (en_conexion_externa)
							{
							    strcatd(otrospool+3,"spool/");
								strcatd(otrospool+3,remoto_dirbass);
							}
							else
#endif
								strcatd(otrospool+3,rw_directorio("spool/"));							
							cambia_barra_dos(otrospool+3);
						}									
						strminus(otrospool);
						if (!strstr(otrospool,".pdf"))
						{
							strcat(otrospool,".pdf");							
						}	
                        i_leeimpre(NULL);
						fispo[0] = 2;
						fispo[1] = 0;
						{
							extern unsigned char nombredev[81];
							nombredev[0] = 2;
							nombredev[1] = 0;
							strcpy(nombredev+2,otrospool);
						}												
						_fispool[0] = 0;
						_ejspool[0] = 0;
						m = 0;
				}
				else if (!memcmp(fispo,"Fax",3))
				{
					    if (fispo[3] == ';')
						{
							strcpy(otrospool,"!!");
							strcat(otrospool,fispo+4);
							fispo[3] = 0;
						}
						else
						{
							strcpy(otrospool,"!!!*");
						}
                        i_leeimpre(NULL);
						fispo[0] = 2;
						fispo[1] = 0;
						{
							extern unsigned char nombredev[81];
							nombredev[0] = 2;
							nombredev[1] = 0;
							strcpy(nombredev+2,otrospool);
						}												
						_fispool[0] = 0;
						_ejspool[0] = 0;
						m = 0;
				}
				else if (!memcmp(fispo,"Correo",6))
				{
					    if (fispo[6] == ';')
						{
							strcpy(otrospool,"||");
							strcat(otrospool,fispo+7);
							fispo[6] = 0;
						}
						else
						{
							strcpy(otrospool,"||!*");
						}
                        i_leeimpre(NULL);
						fispo[0] = 2;
						fispo[1] = 0;
						{
							extern unsigned char nombredev[81];
							nombredev[0] = 2;
							nombredev[1] = 0;
							strcpy(nombredev+2,otrospool);
						}												
						_fispool[0] = 0;
						_ejspool[0] = 0;
						m = 0;
				}
				else
				{
				   strcatd(fispo,rw_directorio("dev/imp/"));
				   if ((qj = i_leeimpre(fispo)) <= 0) 
				   {
					  if (memo5009) memo5009[0] = 0;
					  pito(122);
					  return(-1);
				   }
				}
            }
         }	  
         acaba_impresora(memo5009);	 
		 

         if (fispo[0] == 1 && fispo[1] == 0)
         {
             _imp = NULL;
             return(0);
         }

#ifdef FGVWIN
         if (m)
#endif
            strcpy(fispo,i_nombre());
         
         
         if (fispo[0] == '{')
         {
#ifdef RWXARXA
            if (en_conexion_terminal)
            {     
               especial_servidor = 1;	 
            }
#endif  
         }
         
         
         if (fispo[0] == '@')
         {
#ifdef RWXARXA
            if (en_conexion_terminal)
               especial_servidor = 1;
#endif
            fispo++;
         }
         
         if (fispo[0] == '*' && m)
         {
            strcpy(_ejspool,"bin/");
            if (fispo[1] == 0)
               strcat(_ejspool,"agispool");
            else
               strcat(_ejspool,fispo+1);
			if (_ds_default_spool[0])
			{
				sprintf(fispo,"%s%s",rw_directorio("spool/"),_ds_default_spool);
			}
			else
			{
				sprintf(fispo,"%s%03d%03d",rw_directorio("spool/"),user_who,subnumero_de_usuario);            
			}
            strcat(fispo,".spo");
            unlink(fispo);
            strcpy(_fispool,fispo);
            qj = 0;
         }         
#ifndef FGVWIN
         else
         if (fispo[0] == '*') 
         {
            strcpy(_ejspool,"bin/");
            if (fispo[1] == 0)
               strcat(_ejspool,"agispool");
            else
               strcat(_ejspool,fispo+1);
            As();
            pprint(1910,__Texto[276]);
            Cs();
            do {
			   if (_ds_default_spool[0])
			   {
				   qj = entras(1927,30,_ds_default_spool,1,fispo);
			   }
			   else
			   {
				   qj = entras(1927,30,rw_user,1,fispo);
			   }
               if (qj == 1 || qj == 7) 
               {
                  w_pinta(salva);
                  w_fin(salva);
                  if (memo5009) memo5009[0] = 0;
                  return(-1);
               }
               quitab(fispo);
               strcatd(fispo,rw_directorio("spool/"));
               strcat(fispo,".spo");
               if (!especial_servidor && (strm = fopen(fispo,"r")) != NULL) 
               {
                  fclose(strm);
                  Ar();
                  aviso(0,__Texto[41]);
                  Cr();
                  qj = -1;
               }
               else 
               {
                  qj = 0;
                  strcpy(_fispool,fispo);
               }
            } while(qj == -1);
         }
         
         
         if (m == 0 && salva) {
            w_pinta(salva);
            w_fin(salva);
         }
#endif
         
         atento_imp = 0; /* control esclava */
                         /*#ifdef FGVWIN
                         _imp = (FILE *)(unsigned long)0x00000003L;
                         i_inicio(_imp);
                         i_reset(_imp);
         /* falta ajustar tema fichero spool */
         /* return(0);
#endif*/
         if (especial_servidor)
         {
            return(2);
         }
         
         
#ifdef ESDOS
#ifndef FGVWIN
#ifndef FGV32
         
         if (!strcmp(fispo,"LPT1"))
         {
            _imp = (FILE *)(unsigned long)0x00000001L;
            no_imprime[0] = 0;
         }
         else
            if (!strcmp(fispo,"LPT2"))
            {
               _imp = (FILE *)(unsigned long)0x00000002L;
               no_imprime[1] = 0;
            }
            else
               if (!strcmp(fispo,"LPT3"))
               {
                  _imp = (FILE *)(unsigned long)0x00000003L;
                  no_imprime[2] = 0;
               }
               else
#endif
#endif
#ifdef FGVWIN
                  fin_w_printer();
               if (!fispo[0] || (fispo[0] == 2 && fispo[1] == 0))
               {				   
				  if (fispo[0] == 2)
				  {					  
					  nombredev[0] = 2;
					  nombredev[1] = 0;
					  strcpy(nombredev+2,otrospool);
				   	  i_nlineas = -1;					 
					  i_anchocarro = 80;
				  }
				  else if (ini_w_printer(m))
				  {				
					 aviso(0,"No hay impresora windows predeterminada");
                     return(-1);
				  }
                  _imp = (FILE *)(unsigned long)0x00000003L;
                  return(0);
               }
#endif
#ifdef ESDOS
			   if (!memcmp(fispo,"COM",3) && fispo[4] == ':')
			   {				   
				   int i;
				   fispo[4] = 0;/* 9600,8,N,1 */
				   escom = 1;
			       dcb.BaudRate = atoi(fispo+5);				   
				   i = 5;
			       dcb.ByteSize = 8;             // data size, xmit, and rcv
				   dcb.Parity = NOPARITY;        // no parity bit
				   dcb.StopBits = ONESTOPBIT;    // one stop bit
				   while(fispo[i] && fispo[i] != ',') i++;
				   if (fispo[i] == ',')
				   {
					   i++;
					   dcb.ByteSize = atoi(fispo+i);
					   while(fispo[i] && fispo[i] != ',') i++;
					   if (fispo[i] == ',')
					   {
						   i++;
						   if (fispo[i] == 'N')
						   {
							   dcb.Parity = NOPARITY;
						   }
						   else
						   {
							   dcb.Parity = fispo[i];
						   }
						   while(fispo[i] && fispo[i] != ',') i++;
						   if (fispo[i] == ',')
						   {
							   i++;
							   if (fispo[i] == '1')
							   {
								   dcb.StopBits = ONESTOPBIT;
							   }
							   else
							   {
								   dcb.StopBits = TWOSTOPBITS;
							   }
						   }
					   }
				   }
			   }
#endif

               
               if (fispo[0] == '{')
               {   
                  _imp = (FILE *)(unsigned long)0x00000004L;
                  qo[0] = 0;   
                  carga_por_dll(qo);
               }
               else
                  if ((_imp = fopen(fispo,"w")) == NULL) {
                     if (memo5009) memo5009[0] = 0;
                     _imp = (FILE *)0;
                     pito(123);
                     return(-1);
                  }
#else
                  if (fispo[0] == '{')
                  {   
                     _imp = (FILE *)(unsigned long)0x00000004L;
                     qo[0] = 0;   
                     carga_por_dll(qo);
                  }
                  else
                     if ((_imp = fopen(fispo,"w")) == NULL) {
                        if (memo5009) memo5009[0] = 0;
                        _imp = (FILE *)0;
                        pito(124);
                        return(-1);
                     }
#endif
                     
                     if ( _imp )
                     {
#ifdef ESDOS
				 if (escom)
				 {
					 DCB dc;
			         HANDLE oo = _get_osfhandle(fileno(_imp));					 
					 GetCommState(oo, &dc);
				     dc.BaudRate = dcb.BaudRate;     
			         dc.ByteSize = dcb.ByteSize;     
				     dc.Parity = dcb.Parity;
				     dc.StopBits = dcb.StopBits;
					 SetCommState(oo, &dc);
				 }
#endif

                        if (es_esclava)
                        {
                           atento_imp = 1; /* ES UNA ESCLAVA */
                        }
                     }
                     
                     if (_imp != (FILE *)(unsigned long)0x00000004L)
                     {
                        i_inicio(_imp);
                        i_reset(_imp);
                     }
                     
                     if (rr >= 0)
                     {
                         strcpy(ult_fispo,fispo);
                         strcpy(ult_fispool,_fispool);
                         strcpy(ult_ejspool,_ejspool);
                     }

                     return(rr);
}


int graba_impresora_defecto(char *laimpre)
{
#ifdef FGVDOS
   HKEY hKey = NULL;
   DWORD dispo;
   LONG retv;
   DWORD Disposition;
   DWORD Type;
   DWORD Activa;
   DWORD Size;
   
   if ((retv = RegCreateKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Diagramx",0,"Diagramx",REG_OPTION_NON_VOLATILE,/*KEY_QUERY_VALUE | KEY_SET_VALUE*/ KEY_ALL_ACCESS,NULL,&hKey,&Disposition)) == ERROR_SUCCESS)
   {
      Size = strlen(laimpre)+1;
      Type = REG_SZ;      
      RegSetValueEx(hKey,"Impresora",0,Type,laimpre,Size);
      RegCloseKey(hKey);
   }
#endif
   return 0;
}

int lee_impresora_defecto(char *laimpre)
{
#ifdef FGVDOS
   HKEY hKey = NULL;
   DWORD dispo;
   LONG retv;
   DWORD Disposition;
   DWORD Type;
   DWORD Activa;
   DWORD Size;
   
   if ((retv = RegCreateKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Diagramx",0,"Diagramx",REG_OPTION_NON_VOLATILE,/*KEY_QUERY_VALUE | KEY_SET_VALUE*/ KEY_ALL_ACCESS,NULL,&hKey,&Disposition)) == ERROR_SUCCESS)
   {
      Size = 81;
      Type = REG_SZ;      
	  retv = RegQueryValueEx(  hKey,"Impresora",NULL,&Type,laimpre,&Size);
      RegCloseKey(hKey);
   }
#endif
   return 0;
}

int pregu_impre(t)
int t;
{
   char *memo5009;
   int i,qj;
   char fispo[256];

   _ds_default_spool[0] = 0;
   if (t != -1 && ds_absoluta_impresora)
   {
	   strcpy(_ds_absoluta_impresora,ds_absoluta_impresora);	   
	   for (i = 0;_ds_absoluta_impresora[i];i++)
	   {
		   if (_ds_absoluta_impresora[i] == ',')
		   {
			   _ds_absoluta_impresora[i] = 0;
			   strcpy(_ds_default_spool,_ds_absoluta_impresora+i+1);
			   quitab(_ds_absoluta_impresora);
			   quitab(_ds_default_spool);
			   quitabp(_ds_default_spool);
			   break;
		   }
	   }
	   memo5009 = _ds_absoluta_impresora;	   
	   t = -1;
   }
   else
   {
	   if (memoplin > 9)
		  memo5009 = v_memo(5009)->alfabetica;
	   else
		  memo5009 = NULL;  
   }
   
   if (memo5009 && strlen(memo5009))
      strcpy(fispo,memo5009);
   else
   {
	  strcpy(fispo,"");
	  if (mpfirel[48] != (RELAS *)0 && mpfi[48] != NULL ) {
		 i = 0;
		 for (qj = 0;qj < mpfirel[48]->nrelpin;qj++) {
			if (mpfirel[48]->relpin[qj].qp == -1) {
			   if (i == t && mpfirel[48]->relpin[qj].numrela > -1) {
				  ld_campo(mpfirel[48]->relpin[qj].numrela,mpfi[48],fispo);
				  break;
			   }
			   i++;
			}
		 }
	  }	  
   }
   quitab(fispo);
   if (!strcmp(fispo,"ibm80"))
	   fispo[0] = 0;
   i = _pregu_impre_(t,memo5009,fispo);
   if (i >= 0)
   {
	   if (_imp == 0x00000003L && nombredev[0] == 2 && nombredev[1] == 0)
	   {
		   if (memo5009)
		   {
			   if (nombredev[2] == '!' && nombredev[3] == '!')
			   {
				   strcpy(memo5009,"Fax");
				   strcat(memo5009,";");
				   strcat(memo5009,nombredev+4);
			   }
			   else if (nombredev[2] == '$' && nombredev[3] == '$')
			   {
				   strcpy(memo5009,"PDF");
				   strcat(memo5009,";");
				   strcat(memo5009,nombredev+4);
			   }
			   else if (nombredev[2] == '|' && nombredev[3] == '|')
			   {
				   strcpy(memo5009,"Correo");
				   strcat(memo5009,";");
				   strcat(memo5009,nombredev+4);
			   }
			   else
			   {
				   strcpy(memo5009,"CrystalReport");
				   if (nombredev[2])
				   {
					   strcat(memo5009,";");
					   strcat(memo5009,nombredev+2);
				   }
			   }
		   }
	   }
   }
   return i;
}



int acaba_impresora(memo5009)
char *memo5009;
{
   char pro[1024];
   VENT *w = NULL;
   char **funs;
   int noacabo =0;
   
   if (nombredev[0] == 2 && nombredev[1] == 0)
   {
	   /*nombredev[0] = 0;*/
	   noacabo =1;
   }   

   if (Pseudo10 && _imp == (FILE *)0)
   {
        if (DespachadorExterno)
        {	 
           (*DespachadorExterno)((int)DESPACHO_LISTADO,2,"");
        }
   }


   if (_imp != (FILE *)0) {
#ifdef RWSERVER
      if (!en_conexion_externa || ((unsigned long)_imp) != ((unsigned long)0x00000003L))
#endif
         if (_imp != (FILE *)(unsigned long)0x00000004L)
         {
			if (!noacabo)
				i_acabo(_imp);
         }
         
         atento_imp = 0; /* control esclava */
         if (_imp)
         {
            fin_printer(_imp);
         }
         _imp = NULL;
         
         if (memo5009) memo5009[0] = 0;
         if (_fispool[0] && _ejspool[0]) 
         {
            sprintf(pro,rw_directorio("%s %s"),_ejspool,_fispool);
            filtropar(pro);		   
#ifndef FGVWIN
#ifndef FGV32
            cursor(301);fflush(stdout);
            fin_sig(); /* no hace falta el terfinal */
#else
            
            
            if (-1 /*fIsTNT*/)
            {
#ifdef RWSERVER
               if (!en_conexion_externa)
               {	 
#endif
                  w = w_ini(101,2580);
                  w_salva(w);
#ifdef RWSERVER
               } 
#endif
               
               fin_sig();
               funs = salva_funciones(0,-1);
               Terfinal();  
               if (fIsTNT)
               {
                  FreeConsole();
                  AllocConsole();
               }
            }
            
#endif
#else
			//fin_sig(); /* no hace falta el terfinal */
#endif
            
			/*aviso(0,"Voy!!");*/
#ifdef FGVDOS
			//_flushall();		   
#endif
#ifdef FGVWIN
		   if (getenv("DSWINEX"))
		   {
			   agi_system_wine(pro);
		   }
		   else
		   {
				/*
				extern HWND principal_w;
			MessageBox(NULL,"anTEs1","ojo",MB_OK);
			*/
			//SendMessage(principal_w,WM_COMMAND,44422,(LPARAM)pro);						
			x_system(pro);
			//MessageBox(NULL,"Despues1","ojo",MB_OK);
		   }
#else			
            system(pro);
#endif
#ifdef FGVWIN
			{
				//int i;

				/*aviso(0,"Vengo!!");*/
                //for (i = 0;i < 100;i++) /* aligerar sistema */
                  //agi_cola_win();
			}
#endif
			
            /*agi_system(pro);*/
#ifndef FGVWIN
#ifndef FGV32
            ini_sig();
#else
            if (-1 /*fIsTNT*/)
            {
               if (fIsTNT)
               {
                  FreeConsole();
                  AllocConsole();
               }
               Terinici();
               repon_funciones(funs,0,-1);
               ini_sig();	     
               if (w)
               {
                  w_mete(stdpan,w);
                  w_fin(w);
               }
#ifdef RWSERVER
               if (!en_conexion_externa)
#endif
                  v_refresca();
            }
            
#endif
#else
			//ini_sig();
#endif
         }
         _fispool[0] = '\0';
			}
   return(0);
}

static char *Font = "Courier New";
static int  sFont = 10;
static char bFont = 'N';
static char *FontN = "Courier New";
static int  sFontN = 10;
static char bFontN = 'S';
static char *FontE = "Courier New";
static int  sFontE = 12;
static char bFontE = 'S';
static char *FontC = "Courier New";
static int  sFontC = 8;
static char bFontC = 'N';

static int saca_bloque(pfich,texto,handle)
FILE *pfich;
char *texto;
int handle;
{
	int i,n,k,f,l,j;
	char text2[256];
	char elcampo[512];
	char tmp[1024];
	char usi[81];
	char usi2[300];
	int nlin,interlin,ncamp,repe,ndeci,xn;
	char *p1,*p2;
	
	lee_linea_i(texto,255,pfich);
	nlin = atoi(texto+coma_linea(texto,0));	
	
	for (k=0;k<nlin;k++)
	{
		lee_linea_i(texto,255,pfich);
		ncamp = atoi(texto + coma_linea(texto,0));
		/*
		repe = atoi(texto + coma_linea(texto,1));
		if (repe < 1) {
			aviso(0,"flag repeticion incorrecto");
		}
		*/		
		text2[0] = '\0';
		for(i = 0;i < ncamp;i++) 
		{
			lee_linea_i(texto,255,pfich);
			strcpy(elcampo,texto);

			coma_linea(texto,0);
			quita_blancos(texto);
			n = 0;f = 0;
			if (texto[0] == 'A') 
			{
				f = 51;
			}
			if (texto[0] == 'C') 
			{
				f = 52;
			}
			if (f > 0 && (texto[1] == 'C' || texto[1] == 'N' || texto[1] == 'E')) 
			{
				if (texto[1] == 'N') f += 2;
				if (texto[1] == 'E') f += 4;
			}
			else if (strcmp(texto,"PAG") == 0)
				f = 50;
			else if (strcmp(texto,"RESET") == 0)
			{
				f = 59;
			}
			else if (strcmp(texto,"OC") == 0)
				f = 58;
			else if (strcmp(texto,"SP") == 0)
				f = 57;
			else 
			{
				f = 0;
			}
			if (1)
			{
				switch(f)
				{
				case 52:
				case 54:
				case 56:
				case 59:
					sprintf(tmp,"%d\001%%%c%s\001%d",handle,bFont,Font,sFont);
					(*DespachadorExterno)((int)DESPACHO_ENLACEWORD,6,tmp);
					break;
				case 51:
					sprintf(tmp,"%d\001%%%c%s\001%d",handle,bFontC,FontC,sFontC);
					(*DespachadorExterno)((int)DESPACHO_ENLACEWORD,6,tmp);
					break;
				case 53:
					sprintf(tmp,"%d\001%%%c%s\001%d",handle,bFontN,FontN,sFontN);
					(*DespachadorExterno)((int)DESPACHO_ENLACEWORD,6,tmp);
					break;
				case 55:
					sprintf(tmp,"%d\001%%%c%s\001%d",handle,bFontE,FontE,sFontE);
					(*DespachadorExterno)((int)DESPACHO_ENLACEWORD,6,tmp);
					break;
				default:
					break;
				}
				coma_linea(texto,1);
				coma_linea(texto,1);				;
				n = atoi(texto+coma_linea(texto,1));
				if (n > 0)
				{
					memset(usi,' ',n);
					usi[n] ='\0';				
					sprintf(tmp,"%d\001=+\001%s",handle,usi);
					(*DespachadorExterno)((int)DESPACHO_ENLACEWORD,6,tmp);
				}

				if (f > 50 && f != 57 && f != 58)
				{
					usi[0] = 0;
				}
				else
				{
					l = 80;
					memset(usi,'.',l);
					usi[l] ='\0';
				}				
				sprintf(usi2,texto+coma_linea(texto,1),usi);
				sprintf(tmp,"%d\001$+QUOTE %s\001{{%s}}",handle,usi2,elcampo);
				(*DespachadorExterno)((int)DESPACHO_ENLACEWORD,6,tmp);
			}
		}
	    if (!ncamp) 
		{
		  lee_linea_i(texto,255,pfich);
		  coma_linea(texto,0);
		  n = coma_linea(texto,1);
		  sprintf(text2,texto,texto+n);

		  p1 = text2;
		  p2 = text2;
		  while(*p2)
		  {
			  if (*p2 == '{' && *(p2+1) == '{' && *(p2+3) == '}' && *(p2+4) == '}')
			  {
				  if (p2 > p1)
				  {
					  xn = (int)(p2-p1);
					  memcpy(texto,p1,xn);
					  texto[xn] = 0;
					  sprintf(tmp,"%d\001=+\001%s",handle,texto);
					  (*DespachadorExterno)((int)DESPACHO_ENLACEWORD,6,tmp);
				  }
				  p1 = p2 + 5;
				  switch(*(p2+2))
				  {
                  case 'C':
					sprintf(tmp,"%d\001%%%c%s\001%d",handle,bFontC,FontC,sFontC);
					(*DespachadorExterno)((int)DESPACHO_ENLACEWORD,6,tmp);
					break;
                  case 'c':
				  case 'n':
				  case 'e':
					 sprintf(tmp,"%d\001%%%c%s\001%d",handle,bFont,Font,sFont);
					 (*DespachadorExterno)((int)DESPACHO_ENLACEWORD,6,tmp);
                     break;
                  case 'N':
					sprintf(tmp,"%d\001%%%c%s\001%d",handle,bFontN,FontN,sFontN);
					(*DespachadorExterno)((int)DESPACHO_ENLACEWORD,6,tmp);
					break;
                  case 'E':
					sprintf(tmp,"%d\001%%%c%s\001%d",handle,bFontE,FontE,sFontE);
					(*DespachadorExterno)((int)DESPACHO_ENLACEWORD,6,tmp);
					break;
                  case 'P':
                     *(p2+4) = 12;
					 p2--;
                     break;
                  case 'O':
                     break;
                  default:
                     break;
				  }
				  p2 += 4;
			  }
			  p2++;
		  }
		  if (p2 > p1)
		  {
			  xn = (int)(p2-p1);
			  memcpy(texto,p1,xn);
			  texto[xn] = 0;
			  sprintf(tmp,"%d\001=+\001%s",handle,texto);
			  (*DespachadorExterno)((int)DESPACHO_ENLACEWORD,6,tmp);
		  }
		}
	    sprintf(tmp,"%d\001=+\001\n",handle);
		(*DespachadorExterno)((int)DESPACHO_ENLACEWORD,6,tmp);


		lee_linea_i(texto,255,pfich); /* calculos */			
		lee_linea_i(text2,255,pfich); /* condiciones */		
		if (texto[0] || text2[0])
		{
			sprintf(tmp,"%d\001$+PRIVATE\001{{LIN %s$$%s}}",handle,texto,text2);
			(*DespachadorExterno)((int)DESPACHO_ENLACEWORD,6,tmp);
		}
   }
   
   return(nlin);
}

static void mete_pseudo(char *texto)
{
	int i;
	n_pseudo_lineas++;
	pseudo_lineas = (char **)realloc(pseudo_lineas,sizeof(char *) * n_pseudo_lineas);
    for (i = n_pseudo_lineas-1;i > 0;i--)
	{
		pseudo_lineas[i] = pseudo_lineas[i-1];
	}
	pseudo_lineas[0] = malloc(strlen(texto)+1);
	strcpy(pseudo_lineas[0],texto);
}

static int saca_subbloque(pfich,texto,handle)
FILE *pfich;
char *texto;
int handle;
{
	int i,li,k,nc;
	char tmp[1024];
	char text2[300];

	lee_linea_i(texto,255,pfich);
	li = atoi(texto+coma_linea(texto,0));
	n_pseudo_lineas = 1;
	pseudo_lineas = (char **)malloc(sizeof(char *));
	pseudo_lineas[0] = malloc(100);
	sprintf(pseudo_lineas[0],"%d",li);

	for (k=0;k<li;k++)
	{
		lee_linea(texto,255,pfich);
		mete_pseudo(texto);
		nc = atoi(texto + coma_linea(texto,0));
		for (i = 0;i < nc;i++)
		{
			lee_linea(texto,255,pfich);
			mete_pseudo(texto);
		}
		if (!nc)
		{
			lee_linea(texto,255,pfich);
			mete_pseudo(texto);
		}
		lee_linea(texto,255,pfich);
		mete_pseudo(texto);
		lee_linea(texto,255,pfich);
		mete_pseudo(texto);
	}	

	lee_linea(texto,255,pfich);	
	lee_linea(text2,255,pfich);
	if (!atoi(text2))
	{
		sprintf(tmp,"%d\001$+PRIVATE\001{{SUC %s}}",handle,texto);
		(*DespachadorExterno)((int)DESPACHO_ENLACEWORD,6,tmp);
		saca_bloque(NULL,texto,handle);
		sprintf(tmp,"%d\001$+PRIVATE\001{{/SUC}}",handle);
		(*DespachadorExterno)((int)DESPACHO_ENLACEWORD,6,tmp);
	}
	else
	{
		sprintf(tmp,"%d\001$+PRIVATE\001{{SUT %s}}",handle,texto);
		(*DespachadorExterno)((int)DESPACHO_ENLACEWORD,6,tmp);
		saca_bloque(NULL,texto,handle);
		sprintf(tmp,"%d\001$+PRIVATE\001{{/SUT}}",handle);
		(*DespachadorExterno)((int)DESPACHO_ENLACEWORD,6,tmp);
	}

	return(0);
}



static int saca_cuerpo(pfich,texto,handle,rayado,inirayado)
FILE *pfich;
char *texto;
int handle,rayado,inirayado;
{
	int i,nsub,nc,li,nlin,tt;
	char tmp[1024];
	
	lee_linea(texto,255,pfich);
	nlin = atoi(texto);	

	sprintf(tmp,"%d\001$+PRIVATE\001{{CUE %d;%d;%d}}",handle,nlin,rayado,inirayado);
	(*DespachadorExterno)((int)DESPACHO_ENLACEWORD,6,tmp);
	tt = saca_bloque(pfich,texto,handle);
	sprintf(tmp,"%d\001$+PRIVATE\001{{/CUE}}",handle);
	(*DespachadorExterno)((int)DESPACHO_ENLACEWORD,6,tmp);	
	
	lee_linea(texto,255,pfich);
	nsub = atoi(texto);
	for (i = 0;i < nsub;i++)
	{
		saca_subbloque(pfich,texto,handle);
	}
	if (nlin > tt)
	{
		return (nlin-tt);
	}
	else
	   return(0);
}


void pasa_inf_a_doc(char *inf_)
{
	FILE *pfich;
	char inf[300];	
	char tmp[2048];
	char tmp2[2048];
	char text[1024];
	int handle;
	int i;
	char juntopie = 'S';
	int lineas = 0;
	int rayado = 0;
	int inirayado = 0;

	if (strbusca(inf,".doc") >= 0) return;

    if (inf_ == quita_path(inf_)) 
	{
		strcpy(inf,dirdef);
        strcat(inf,inf_);
	} 
    else
	{
	    strcpy(inf,inf_);
	}

	if (strbusca(inf,".inf") < 0)
	{
		strcat(inf,".inf");
	}
	
	pfich = fopen(inf,"r");
	if (!pfich) return;

	strcat(inf,".doc");

	unlink(inf);

	
	if (w_plantilla_principal)
	{
#ifdef RWSERVER
		if (en_conexion_externa)
		{
		  int sal = 0;      
		  strcpy(tmp,w_plantilla_principal);
		  sal = strlen(tmp);
		  tmp[sal] = 1;
		  strcpy(tmp+sal+1,w_plantilla_principal);
		  sal = rw_envia_fichero(tmp,777);      
		}
#endif
		strcpy(tmp,w_plantilla_principal);
	}
	else
		tmp[0] = 0;
	(*DespachadorExterno)((int)DESPACHO_ENLACEWORD,0,tmp);
	handle = atoi(tmp);

	if (handle < 0)
	{
		fclose(pfich);
		return;
	}

	/*
    sprintf(tmp,"%d\001/0\0010",handle); // margenes horizontales
    (*DespachadorExterno)((int)DESPACHO_ENLACEWORD,6,tmp);
    sprintf(tmp,"%d\001\\0\0010",handle); // margenes verticales
    (*DespachadorExterno)((int)DESPACHO_ENLACEWORD,6,tmp);
	*/

    sprintf(tmp,"%d\001&1\0010",handle);
    (*DespachadorExterno)((int)DESPACHO_ENLACEWORD,6,tmp);

	sprintf(tmp,"%d\001%%%c%s\001%d",handle,bFont,Font,sFont);
	(*DespachadorExterno)((int)DESPACHO_ENLACEWORD,6,tmp);

	sprintf(tmp,"%d\001$+PRIVATE\001{{DSINFORM}}",handle);
	(*DespachadorExterno)((int)DESPACHO_ENLACEWORD,6,tmp);


	lee_linea(text,255,pfich);
	/* .inf  */
	lee_linea(text,255,pfich);
	/* nombre */
	lee_linea(text,255,pfich);
	
	
	coma_linea(text,0);
	/*pInf->linpapel = atoi(text);*/	
	coma_linea(text,1);/*pInf->i_especial = atoi(text+coma_linea(text,1));*/
	rayado = atoi(text+coma_linea(text,1));
	inirayado = atoi(text+coma_linea(text,1));
	
	
	lee_linea(text,255,pfich);
	quita_blancos(text);	
	juntopie = text[0];
	
	
	while (lee_linea(text,255,pfich) != NULL) 
	{
		quita_blancos(text);
		if (memcmp(text,"FIC",3) == 0) 
		{
			tmp2[0] = 0;
			lee_linea(text,255,pfich);
		    i = atoi(text);
			for (;i > 0;i--)
			{
				lee_linea(text,255,pfich);
				strcat(tmp2,text);
				if (i > 1)
					strcat(tmp2,"$$");
			}
			sprintf(tmp,"%d\001$+PRIVATE\001{{FIC %s}}",handle,tmp2);
			(*DespachadorExterno)((int)DESPACHO_ENLACEWORD,6,tmp);
		}
		else if (memcmp(text,"VAR",3) == 0) 
		{
			tmp2[0] = 0;
			lee_linea(text,255,pfich);
		    i = atoi(text);
			for (;i > 0;i--)
			{
				lee_linea(text,255,pfich);
				strcat(tmp2,text);
				if (i > 1)
					strcat(tmp2,"$$");
			}
			sprintf(tmp,"%d\001$+PRIVATE\001{{VAR %s}}",handle,tmp2);
			(*DespachadorExterno)((int)DESPACHO_ENLACEWORD,6,tmp);
		}
		else if (memcmp(text,"CAB",3) == 0)
		{
			sprintf(tmp,"%d\001$+PRIVATE\001{{CAB}}",handle);
			(*DespachadorExterno)((int)DESPACHO_ENLACEWORD,6,tmp);
			saca_bloque(pfich,text,handle);
			sprintf(tmp,"%d\001$+PRIVATE\001{{/CAB}}",handle);
			(*DespachadorExterno)((int)DESPACHO_ENLACEWORD,6,tmp);
		}
		else if (memcmp(text,"CUE",3) == 0)
		{
			lineas = saca_cuerpo(pfich,text,handle,rayado,inirayado);
		}
		else if (memcmp(text,"PIE",3) == 0)
		{
			if (lineas > 0 && juntopie == 'F')
			{
				while(lineas)
				{
					sprintf(tmp,"%d\001=+\001\n",handle);
					(*DespachadorExterno)((int)DESPACHO_ENLACEWORD,6,tmp);
					lineas--;
				}
			}
			sprintf(tmp,"%d\001$+PRIVATE\001{{PIE%c}}",handle,juntopie);
			(*DespachadorExterno)((int)DESPACHO_ENLACEWORD,6,tmp);
			saca_bloque(pfich,text,handle);
			sprintf(tmp,"%d\001$+PRIVATE\001{{/PIE}}",handle);
			(*DespachadorExterno)((int)DESPACHO_ENLACEWORD,6,tmp);
		}
		else if (memcmp(text,"SAL",3) == 0)
		{
			sprintf(tmp,"%d\001$+PRIVATE\001{{SAL}}",handle);
			(*DespachadorExterno)((int)DESPACHO_ENLACEWORD,6,tmp);
			saca_bloque(pfich,text,handle);
			sprintf(tmp,"%d\001$+PRIVATE\001{{/SAL}}",handle);
			(*DespachadorExterno)((int)DESPACHO_ENLACEWORD,6,tmp);
		}
	}

	fclose(pfich);
	sprintf(tmp,"%d\001%s",handle,inf);
	(*DespachadorExterno)((int)DESPACHO_ENLACEWORD,1,tmp);	

#ifdef RWSERVER
    if (en_conexion_externa)
    {    
      int sal;
      strcpy(tmp,inf);
      sal = strlen(tmp);
      tmp[sal] = 1;
      strcpy(tmp+sal+1,inf);
      (*DespachadorExterno)((int)DESPACHO_TRAEFICHERO,0,tmp);      
    }
#endif
}


#endif