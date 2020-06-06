
/**********************************************************************/
/**********************************************************************/
/*                    DIAGRAM9                                        */
/**********************************************************************/
/**********************************************************************/
/*
 *---------------------------------------------------------------------------
 * Programa ..: DIAGRAM Creado a partir de la concepcion del entorno MDRUN y posteriormente AGISYS
 * Fecha .....: 1990-1999
 * Version ...:
 * Notas .....: Se van transformado los sufijos MD,AGI.RW a FGV para evitar dependencias a circunstancias que ya no existen
 * Autor e idea de  FRANCISCO DE ASIS GONZALEZ VALIENTE (Autor e idea tambien de MDRUN-MDGEN, AGISYS-AGIGEN)
 *---------------------------------------------------------------------------
 */

#include <fgvsys.h>

#ifdef FGVWIN
char NombreClase[81] = "FGVWIN";
#endif
extern int win_fondo_neutro;

#ifdef FGVDOS
#ifndef FGVWIN

#include <dos.h>
#ifdef BORLANDWORLD
/* esto es muy singular de BORLANDC */
extern unsigned _stklen = 15000U;
#endif

#endif
#endif
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef FGVDOS
#include <io.h>
#include <conio.h>
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

#include <time.h>
#include <sys/timeb.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <errno.h>

#ifdef WATCOMNLM
#include   <nwcntask.h>
#include   <nwbindry.h>
#endif

#include <rwmalloc.h>
#include <rtermin.h>
#include <rentra.h>
#include <rfecha.h>
#include <rwmenus.h>
#include <rsignal.h>
#include <teclas.h>
#define _NUEVODEF
#include <rdef.h>       /* ya incluye el isam.h */
#define _MANTENIMIENTO
#include <rmante.h>
#define _CALCULOS
#include <rcalcul.h>
#ifdef FGV32
#include <process.h>
#endif
#ifdef FGVWIN
#include <fgvwin.h>
#endif


#include <stdlib.h>
/*extern char *getenv();*/

extern char rw_version[256];

extern int tracer_int;

int dont_close_this_ = -1;

#ifdef FGVDOS

#include <process.h>


#define TITLE_SIZE          64 
#define PROCESS_SIZE        MAX_PATH 



typedef struct _TASK_LIST { 
   DWORD       dwProcessId; 
   DWORD       dwInheritedFromProcessId; 
   BOOL        flags; 
   HANDLE      hwnd; 
   CHAR        ProcessName[PROCESS_SIZE]; 
   CHAR        WindowTitle[TITLE_SIZE]; 
} TASK_LIST, *PTASK_LIST; 

typedef struct _TASK_LIST_ENUM { 
   PTASK_LIST  tlist; 
   DWORD       numtasks; 
} TASK_LIST_ENUM, *PTASK_LIST_ENUM; 


// 
// Function pointer types for accessing platform-specific functions 
// 
typedef DWORD (*LPGetTaskList)(PTASK_LIST, DWORD); 
typedef BOOL  (*LPEnableDebugPriv)(VOID); 


// 
// Function prototypes 
// 
DWORD 
GetTaskList95( 
              PTASK_LIST  pTask, 
              DWORD       dwNumTasks 
              ); 

DWORD 
GetTaskListNT( 
              PTASK_LIST  pTask, 
              DWORD       dwNumTasks 
              ); 


BOOL 
EnableDebugPriv95( 
                  VOID 
                  ); 

BOOL 
EnableDebugPrivNT( 
                  VOID 
                  ); 

BOOL 
KillProcess( 
            PTASK_LIST tlist, 
            BOOL       fForce 
            ); 

BOOL KillPid( int pid );

VOID 
GetWindowTitles( 
                PTASK_LIST_ENUM te 
                ); 

BOOL 
MatchPattern( 
             PUCHAR String, 
             PUCHAR Pattern 
             ); 

#define MAX_TASKS 1024
TASK_LIST tlista[MAX_TASKS]; 


#endif



int sin_mensaje_error_a = 0;

#ifdef RWXARXA
extern int en_conexion_terminal;
#ifdef RWSERVER
extern int en_conexion_externa;
extern int en_conexion_externa_stop;
extern int remoto_ver1;
extern int remoto_ver2;
extern unsigned char remoto_verletra;
extern int remoto_que_sistema_es;
extern int remoto_es_sistema_grafico;
#endif
#endif

extern unsigned short local_term[52];
extern unsigned short local_term_[52];

extern char *rw_usuario;
extern int *rw_extranumuser;
extern int fecha_protegida;
extern char *numero_serie;
extern char rw_user[11];
extern int issingleuser; /* flag mono usuario del disam */
extern int agipro();
extern int agiavisa();

/*
#ifdef ESDOS
extern int (*timer_especial_int)();
#endif
*/

typedef struct __fgv_contexto__
{   
   char fichemenu[128];
   char menua[25][MAXMENUA+1];
   int  punto;
   char nommenu[MAXNOMOP];   /* nombre del menu principal */
   T_MENU **wmenu;    /* estructuras de menus */
   int    nsmenu;              /* submenu actual 0,1..,MAXSUBMENU-1 */
   int    wdef;
   int    nmenus;
   VENT   *mw[MAXSUBMENU];     /* ventanas del menu actual */
   /* no eliminar el menu mientras dure el 'contexto' */
   int    pintamenu;           /* si debe o no pintar el menu */
   int    pintahmenu;          /* si debe o no pintar el menu hijo */
   char   *dir_smenu;
   char   *dir_sgmenu;

   FI_DEF *empresa;
   RELAS  *mpfiemp;


   char modulo[9];
   char dirbin[MAXPATH];
   char dirdef[MAXPATH];
   char dirpan[MAXPATH];
   char dirfic[MAXPATH];
   char nomfdir[MAXPATH];
   char dirficb[MAXPATH];
   char dirbass[MAXPATH];
   char dirbase[MAXPATH];
   char nomapli[25];
   char fich_apli[15];

   int p;

} FgvContexto;

FgvContexto DsContexto[10];
int ActContexto = -1;

typedef struct __fgv_p_estado__
{
  short programa_activo;
  int p_punto;
  int nbuc;
  int bucle[20];
  int valcond;  
} FgvPestado;

typedef struct __fgv_aplicacion__
{
   int id;  /* identificador de la aplicacion */
   int wid; /* identificador de la ventana de la aplicacion */

   int padre_anidacion; /* tratar la anidacion generica o por pila??? */

   int Id_Contexto;  /* a que FgvContexto pertenece */

   /* punto de ejecucion */
   int p_anidacion;
   FgvPestado DsPestado[100]; /* habra que hacer dinamico el anidamiento */
   

   /* genericos datos de los procesos */
   unsigned long c_programa_en_uso;
   struct enlaza_prc en_pr;
   short nen_pr;
   short *p_tabla;
   char *proc_tabla;
   char *tipo_tabla;      
   short n_p_tabla;
   short maxpila;   
   short memoplin;
   int nm_ex;
   int np_ex;
   M_EX *m_ex;
   P_EX *p_ex;
   int bucle_break;
   int apli_aborto;
   int operaerror;

   short nfi;
   FI_DEF *mpfi[MAXDEF];
   FI_DEF *mpfie;   
   short  mflagfi[MAXDEF];
   RELAS  *mpfirel[MAXDEF];
   RELAS  *mpfierel;
   FI_DEF *dempresa;
   RELAS  *mpfiemp;

   char *n[6];
   
   int fecha_protegida;
   /* aunque ya sea mucho ...*/
   int mpfi_enlazado;
   char *mpfi_buf_enlazado;
   
   
   int win_fondo_neutro;
   int apli_bloqueo;
   struct limitacion_ *limitaciones;
   int numero_limitaciones;
   int **relacion_limitacion;

   char empresa[MAXDEFECTO];
   char proceso[81];
   char titulo[81];
   int  es_la_doble_;
   int  va_con_doble_;
   char nomapli_doble[260];
   char empresa_doble[260];   
   int  punto_abierto[MAXDEF+5];
   char referencia_help[128];

   char modulo[9];
   char dirbin[MAXPATH];
   char dirdef[MAXPATH];
   char dirpan[MAXPATH];
   char dirfic[MAXPATH];
   char nomfdir[MAXPATH];
   char dirficb[MAXPATH];
   char dirbass[MAXPATH];
   char dirbase[MAXPATH];
   char nomapli[25];
   char fich_apli[15];

} FgvAp;

FgvAp DsAp[50];
int CorrienteDsAp = -1;

int es_dssql = 0;
char *mensajesql = "Error de carga del servidor";

extern int es_un_caso_especial;

#define SALVA_IDS  int _default_id  = default_id;int _default_sid = default_sid;int _default_wid = default_wid;
#define REPON_IDS  default_id  = _default_id;default_sid = _default_sid;default_wid = _default_wid;


int demomento_espera_x(void)
{
	static unsigned long cuenta = 0;
	cuenta++;
	if ((cuenta%1000))
	{
#ifdef RWSERVER
		if (en_conexion_externa)
		    flush_mysock();
#endif
	}
#ifdef WATCOMNLM
   ThreadSwitch(); 
   sleep(1);
#else
#ifdef WIN32
   Sleep(1);
#else
   usleep(100);
#endif
#endif
   return(0);
}

extern int (*ixxdat_micro_espera)(void);


extern int ixx_absoluto;
int agisies = 0;
int agivez;
int magi = 0;
int agitei = 0;
int teiaviso = 0;
int teinolohagas = 0;
int vesylee = 0;

int es_la_doble_ = 0;
int va_con_doble_ = 0;
char nomapli_doble[260];
char empresa_doble[260];

char UsuarioPedido[260];
char PasswordPedido[260];


extern int modo_desatendido;

extern int no_instala_enbucla;

extern int pasa_a_int(double);

extern int con_acceso_directo;

char *nombre_tecla();


extern int (*memo_pregu_imp)();
extern int (*memo_printa_linea)();
extern char *dsdb_;
extern int (*memo_finimp)();
extern int mimodo_debug;
extern char *logger_salida;

extern int se_ha_requerido_usuario;

char empresa[MAXDEFECTO];
char proceso[81];
char titulo[81];
char bufpan[PANBUFSIZE];     /* buffer de pantalla se usa solo como cadena
de 80 blancos                              */
extern char dirbin[MAXPATH];
extern char dirdef[MAXPATH];
extern char dirpan[MAXPATH];
extern char dirfic[MAXPATH];
extern char nomfdir[MAXPATH];
extern char dirficb[MAXPATH];
extern char dirbass[MAXPATH];
extern char dirbase[MAXPATH];

int nomapliset = 0;

extern char *alt_dirfic = NULL;

extern char nomapli[25];

extern char *nomapli_ayuda;

extern char modulo[9];

extern int fgv_ver_euro;

extern int modo_otro_euro;

extern int (*asigna_decimales)();

extern int (*asigna_euro)();


#ifdef FGV32

extern BOOL fIsTNT;

#endif

char prog_rw[128];
extern char nmen0[551];
extern char par1[551];
extern char par2[551];

char apli_corre[25]; /* siguiente aplicacion a ejecutar */
int apli_aborto = 0; /* flag para abortar la aplicacion (QUIT) */

extern int user_who;
extern int subnumero_de_usuario;
int Acceso_directo = 0;

extern FILE *_imp; /* externo en rimpre */
extern atento_imp; /* control esclava */


extern short limitacion_especial;
/***** Enlace con helps y utilidades *****/
extern int agiayuda();

extern int ayuda_remota();

extern char *help_externo;

extern int (*f_ayuda)();
extern char clave_help[32];
extern char referencia_help[128];
extern int (*f_util)();

extern int (*_ejecuta_evento_fichero15)(int ipfi);
extern int (*_ejecuta_evento_fichero16)(int ipfi);
extern int (*_ejecuta_evento_fichero17)(int ipfi);

void (*RetroFuncion[4096])(char *bloque,char *par,int el_punto) = {NULL,NULL,};
char *RetroPar[4096] = {NULL,NULL,};
int RetroTipo[4096] = {0,0,};
int RetroPunto = 0;

extern int modo_de_relacion = 0;


/****************************/
/*agicont,I,X,6,0,A,0,fecha,Contabilidad Gorda,control1,control2,0,0,a,produccion.alcoi,agicont/fich/agicont.opc,0*/
struct info_paquete
{  
   int  version1;
   int  version2;
   int  autoversion;
   int  ver_interna;
   int  num_interno;
   int  rdepende1;
   int  rdepende2;
   int  dependesubsistema;
   int  rdependemodo;
   int  pdepende1;
   int  pdepende2;  
   int  pdependemodo;
   unsigned char versionletra;  
   char tipo;
   char sistema[16];
   char fecha[21];
   char descrip[31];
   unsigned char rdependeletra;
   unsigned char pdependeletra;
   char origen[21];
   long build_numero;
   char es_ayuda[9];
};

struct paquetes
{
   char nompaq[20];
   struct info_paquete i;
   long control_utime;    
   int flag;
};
extern struct paquetes Paq[100];
extern int nPaq;
extern int instalacion_silenciosa;

struct info_paquete *datos_paquete();
struct info_paquete *instala_paquete();
/****************************/
int campo_euro_localizado = 0;
/****************************/

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

extern int (*libera_virtual)();


extern int pregu_impre();
extern int rw_finimp();

extern char _fispool[MAXPATH];
extern char _ejspool[MAXPATH];

char *aginfo = "";

char user_batch[110] = "batch";
char user_batch_psw[110] = "";

extern char *quita_path(char *);

extern int LEERTERMINAL;

extern int bucle_break;

static int u_p_ipfi = -1;
static int u_p_npan = 0;

extern int translevel;
extern int trans_save_block;
extern int auto_trans;


extern int rentra_atrib;
extern int rentra_sal_atrib;
extern int rentra_cc;
extern int rentra_cf;
extern int rentra_sal_cc;
extern int rentra_sal_cf;

extern long trata_memo();
extern char *cambia_path_fich();

extern long da_registo_actual();

extern long que_registro();

extern double ld_doble();

extern int (*asigna_decimales)();

extern int (*asigna_euro)();

extern double DsEuroConvierte; 
extern int DsDeciModoOtro;
extern int DsDeciModoNormal;
extern char DsEuroTexto[512];


int inicializa_repinta_mificha()
{
	u_p_ipfi = -1;
	u_p_npan = 0;
}


int mis_decimales(char *nom)
{
   int i;
   char tmp[128];
   int decimales = 0;

   if (!es_euro(nom))
   {
	   campo_euro_localizado = 1;
	   i = strlen(nom);
	   if (i > 1 && nom[i-2] == 'O' && nom[i-1] == '*')
	   {
		   return 7;
	   }

	   if (fgv_ver_euro && modo_otro_euro)
	   {
		   strcpy(tmp,"EURODV");
	   }
	   else
	   {
		   strcpy(tmp,"EURODB");
	   }
	   for (i = memoplin+maxpila*2;i < nm_ex;i++)
	   {
		  if (!strcmp(tmp,m_ex[i].nombre) && m_ex[i].m_tipo != CHARTIPO && m_ex[i].m_tipo != FECHATIPO)
		  {
			 decimales = (int)m_ex[i].mv.vald;
			 if (fgv_ver_euro && modo_otro_euro)
			 {
				 DsDeciModoOtro = decimales;
			 }
			 else
			 {
				 DsDeciModoNormal = decimales;
			 }			 
			 if (strlen(nom) == 5 && nom[4] >= '0' && nom[4] <= '9')
				 decimales += (nom[4]-48);
			 return(decimales);
		  }
	   }
	   if (fgv_ver_euro && modo_otro_euro)
	   {
		   decimales = DsDeciModoOtro;		   
	   }
	   else
	   {
		   decimales = DsDeciModoNormal;
	   }
	   if (strlen(nom) == 5 && nom[4] >= '0' && nom[4] <= '9')
	 	  decimales += (nom[4]-48);	   
	   return decimales;
   }
   sprintf(tmp,"nDeci_%s",nom);
   for (i = memoplin+maxpila*2;i < nm_ex;i++)
   {
      if (!strcmp(tmp,m_ex[i].nombre) && m_ex[i].m_tipo != CHARTIPO && m_ex[i].m_tipo != FECHATIPO)
      {
         return((int)m_ex[i].mv.vald);
      }
   }
   return(7);
}


int mis_euro(char *nom,double *numd)
{
   int i;
   if (!es_euro(nom))
   {
	   for (i = memoplin+maxpila*2;i < nm_ex;i++)
	   {
		  if (!strcmp("EURO",m_ex[i].nombre) && m_ex[i].m_tipo != CHARTIPO && m_ex[i].m_tipo != FECHATIPO)
		  {							
			  if (m_ex[i].mv.vald == (double)0.00601012 || m_ex[i].mv.vald == (double)0.006010 || m_ex[i].mv.vald == (double)0.00601)
			  {
				  m_ex[i].mv.vald = (double)0.006010121043837822893753080187035;
			  }
			  *numd *= (DsEuroConvierte = m_ex[i].mv.vald);
			  campo_euro_localizado = 1;
			  return 0;
		  }
	   }
	   *numd *= DsEuroConvierte;
	   campo_euro_localizado = 1;
   }
   return 0;
}


/*============================================================================*/

/*============================================================================*/

int v_prog(f)
int f;
{
   int r,i;
   if (f < 0 || f >= 48) return(f);
   if (programa_activo < 0)
   {
      aviso(0,"No hay programa activo(A)");
   }
   r = en_pr[programa_activo].tabla_ficheros[f];
   if (r >= 1000)
   {
      r -= 1000;
      r = pasa_a_int(v_memo(r)->numerica);
      if (r < 0 || r >= nfi)
      {
         aviso(0,"Referencia a fichero erronea!");
         r = 48;
         tracer_int = 1;
      }
   }
   return(r);
}

int no_memo_interna(c)
int c;
{
   if (programa_activo < 0)
   {
      aviso(0,"No hay programa activo(C)");
   }   
   if (c < 5000) 
	   return(-1);
   c -= 5000;
   if (c > memoplin) 
	   return(-1);
   return(0);
}

int que_memo_tipo(n)
int n;
{
register i;
         
         if (programa_activo < 0)
         {
            aviso(0,"No hay programa activo(B)");
         }
         
         if (n >= 5000)
         {
            n -= 5000;
            if (n < 0 || n >= nm_ex)
                return -1;
            i = memoplin+maxpila*2;
            if (n < i)
               return(m_ex[n].m_tipo);
            return(m_ex[ en_pr[programa_activo].tabla_externa[n-i] ].m_tipo);
         }
         if (n < 0 || n >= nmemo)
             return -1;
         return(memo[n].m_tipo);
}


static char tempo_memo_buf[300];

static union f_val_memo default_memo;

int control_ojo_al_dado = 0;

union f_val_memo *v_memo(n)
int n;
{
   register i;
   if (n >= 5000)
   {
		  if (n >= 10000)
		  {
			  n -= 10000;
			  if (programa_activo >= 0)
			  {
				  n = en_pr[programa_activo].tabla_gosubs[n];
				  p_ejecuta(n,O_NOR,0,1); /* faclin se pierde!! */

			  }
		  }
		  else
		  if (!nm_ex || (!memoplin  && n < 5011))
		  {
			 n -= 5000;
			 if (n == 0 || n == 7)
			 {
				default_memo.numerica = 0.0;
			 }
			 else
			 {
				memset(tempo_memo_buf,0,300);
				default_memo.alfabetica = tempo_memo_buf;
			 }
			 return(&default_memo);
		  }
		n -= 5000;
		i = memoplin+maxpila*2;
		if (n < i) {
			 return((union f_val_memo *)&(m_ex[n].mv));
		}
		if (programa_activo < 0 || programa_activo  >= nen_pr) {
			aviso(0,"No hay programa activo(D)");
		}

		if (!en_pr[programa_activo].tabla_externa) {
			return(&default_memo);
		}

		if (en_pr[programa_activo].tabla_externa[n-i] < 0 || en_pr[programa_activo].tabla_externa[n-i] > nm_ex) {
			char tmp5[512];
			sprintf(tmp5,"error en tabla externas [%s] [%s]",modulo,en_pr[programa_activo].prc);
			tracer_int = 1;
			aviso(0,tmp5);
			return(&default_memo);
		}

		/*		
		if (control_ojo_al_dado && strstr(m_ex[ en_pr[programa_activo].tabla_externa[n-i] ].nombre,"Divisa")) {
			char tmp5[512];
			if (strcmp(m_ex[ en_pr[programa_activo].tabla_externa[n-i] ].mv.valc,"EUR")) {
				if (strcmp(m_ex[ en_pr[programa_activo].tabla_externa[n-i] ].mv.valc,"   ")) {
					if (strcmp(m_ex[ en_pr[programa_activo].tabla_externa[n-i] ].mv.valc,"")) {
						
						sprintf(tmp5,"[%s] [%s] var %s conteniendo %s",modulo,en_pr[programa_activo].prc,
							                                           m_ex[ en_pr[programa_activo].tabla_externa[n-i] ].nombre,
					                                                   m_ex[ en_pr[programa_activo].tabla_externa[n-i] ].mv.valc);
						aviso(0,tmp5);
						tracer_int = 1;						
					}
				}
			}			
		}	
		
		if (control_ojo_al_dado && strstr(m_ex[ en_pr[programa_activo].tabla_externa[n-i] ].nombre,"Divisa")) {
			control_ojo_al_dado = 2;
		}
		*/

	   return((union f_val_memo *)&(m_ex[ en_pr[programa_activo].tabla_externa[n-i] ].mv));
   }
   
   
   if (programa_activo < 0)
   {
      aviso(0,"No hay programa activo(E)");
      return &default_memo;
   }
   
   if (n < 0 || n >= nmemo)
   {
      aviso(0,"Error memoria ilegal(P.A.)");
      return &default_memo;
      /* [%d] n */
   }
   return((union f_val_memo *)&(memo[n].mv));
}

int fd_mpfi(i)
int i;
{
   int recur = (recur_punto < 1) ? 0 : recur_punto * MAXDEF;
   if (i < 0 || (i >= nfi && i != 48))
						return(-1);
   if (mpfi[i])
						return(mpfi[i]->f_fd);
   return(s_d[recur+i].fd);
}


int set_fd_mpfi(i,fd)
int i,fd;
{
   int recur = (recur_punto < 1) ? 0 : recur_punto * MAXDEF;
   if (i < 0 || (i >= nfi && i != 48))
      return(-1);
   if (mpfi[i])
      return((mpfi[i]->f_fd = fd));
   return((s_d[recur+i].fd = fd));
}



int ejecuta_evento_fichero15(int ipfi)
{
   if (ejecuta_otro(ipfi,15,O_NOR,0,0) == -1)
   {
	   if (memoplin > 4)
	   {
		   return atoi(v_memo(5004)->alfabetica);
	   }
   }
   return 0;
}

int ejecuta_evento_fichero16(int ipfi)
{
   if (ejecuta_otro(ipfi,16,O_NOR,0,0) == -1)
   {
	   if (memoplin > 4)
	   {
		   return atoi(v_memo(5004)->alfabetica);
	   }
   }
   return 0;
}

int ejecuta_evento_fichero17(int ipfi)
{
   if (ejecuta_otro(ipfi,17,O_NOR,0,0) == -1)
   {
	   if (memoplin > 4)
	   {
		   return atoi(v_memo(5004)->alfabetica);
	   }
   }
   return 0;
}

/* ---------------------------------------------------------------------------
*  cierra_rel
*    descripcion: cierra los ficheros a leer (en mpfi) que esten abiertos.
*    devuelve: < 0  si hay error al cerrar alguno (- el indice del fichero),
*              = 0  sino.
* ---------------------------------------------------------------------------
*/
int cierra_rel(prime)
int prime;
{
   register int i,r = 0,s = 0;
   
   i = prime;
   for (;i<nfi;i++) 
   {
/*
#ifdef ESDOS
      if (timer_especial_int)
									(*timer_especial_int)();
#endif
*/
      if (fd_mpfi(i) > -1) 
	  {
		 r = _cierra_ind(fd_mpfi(i));
		 if (r == -1)
		   s = -i-1;
		 else
		   set_fd_mpfi(i,-1);
      }
   }
   
   return(s);
}


int cierra1_rel(i)
int i;
{
   register int r = 0;
/*
#ifdef ESDOS
   if (timer_especial_int)
      (*timer_especial_int)();
#endif
*/   
   if (dont_close_this_ == i) return(0);
   
   if (fd_mpfi(i) > -1)
   {
      r = _cierra_ind(fd_mpfi(i));
      if (r == -1) {
         return(-1);
      }
      else
         set_fd_mpfi(i,-1);
      f_s(i,RCERRADO);
      f_n(i,RABIERTO);
   }
   return(0);
}

int abre1_rel(i)
int i;
{
   register int r;

/*
#ifdef ESDOS
   if (timer_especial_int)
      (*timer_especial_int)();
#endif
*/ 
   
   if (v_mpfi(i)->f_fd == -1) 
   {
      if (v_mpfi(i)->f_nclaves > 0) 
      {
         r = abre_ind(v_mpfi(i),ISINOUT + ISMANULOCK);
         if (r == -1) {
								if (iserrno == ENOENT) 
                                    {
                                        char tmp[256];

                                        strcpy(tmp,v_mpfi(i)->f_fichero);
                                        strcat(tmp,".ixx");
                                        unlink(tmp);
                                       
                                       crea_ind(v_mpfi(i));
                                       cierra_ind(v_mpfi(i));
                                       v_mpfi(i)->f_fd = -1;
                                       agichmod(v_mpfi(i)->f_fichero,".*","","");
                                       r = abre_ind(v_mpfi(i),ISINOUT + ISMANULOCK);
                                    }
                                    if (r == -1) 
                                    {
                                       if (iserrno == EMFILE)
                                       {
                                          Ar();
                                          aviso(0,"Faltan 'handles' de fichero en el sistema");
                                          Cr();
                                       }
									   else if (iserrno == ETOOMANY) 
									   {
                                          Ar();
                                          aviso(0,__Texto[7]);
                                          Cr();
									   }
									   {
										   FILE *f;
										   char tmpc[512];
										   sprintf(tmpc,"%seabierto.txt",rw_directorio("logs/"));
										   f = fopen(tmpc,"a");
										   if (f)
										   {
											   fprintf(f,"%s %s %s %s %s %d\r\n",fecha(),fechas+11,rw_user,nomapli,modulo,iserrno);
											   fclose(f);
										   }
									   }
                                       cierra1_rel(i);
                                       return(-1);
                                    }
         }
         f_s(i,RABIERTO);
         f_n(i,RCERRADO);
      }
      else
         v_mpfi(i)->f_fd = -1;
   }
   else
   { 
      /* se supone que si llama al abre requiere la clave 1 y no tiene registro corriente */
      cambia_ind(v_mpfi(i),1); 
   }
   return(0);
}


ld_camcd(tipo,c,f,val)
int tipo;
int    c;
int    f;
M_VAL *val;
{
   int i;
   if (tipo == CHARTIPO || tipo == FECHATIPO) {
      ld_campo(c,v_mpfi(f),val->valc);
      if (c == v_mpfirel(f)->clave)
         encrip(val->valc,val->valc);
   }
   else
      val->vald = ld_doble(c,v_mpfi(f));
}

st_camcd(tipo,c,f,val)
int tipo;
int    c;
int    f;
M_VAL *val;
{
   if (tipo == CHARTIPO || tipo == FECHATIPO) {
      if (c == v_mpfirel(f)->clave)
         encrip(val->valc,val->valc);
      st_campo(c,v_mpfi(f),val->valc);
      if (c == v_mpfirel(f)->clave)
         encrip(val->valc,val->valc);
   }
   else
      st_doble(c,v_mpfi(f),val->vald);
}

q_tip(t)
int t;
{
   if (t == CHARTIPO)
      return(LETRA);
   
   if (t == FECHATIPO)
      return(ESFECHA);
   
   return(NUMERO);
}

conv_bool(s)
char *s;
{
   if (strlen(s) == 1) {
      if (*s == 'S' || *s == 's')
         strcpy(s,"1");
      else
         if (*s == 'N' || *s == 'n')
            strcpy(s,"0");
   }
   return(0);
}


int c_reverse(o)
char *o;
{
   switch (*o) {
   case OSUMAR :
      *o = ORESTA;
      return(0);
   case ORESTAR :
      *o = OSUMA;
      return(0);
   case OMULTIPLICAR :
      *o = ODIVIDE;
      return(0);
   case ODIVIDER :
      *o = OMULTIPLICA;
      return(0);
   case OIGUALR :
      *o = ORESET;
      return(0);
   default :
      return(0);
   }
}

cal_mensa(i,m)
int i,m;
{
   int o;
   char b[2];
   int z;
   
   if (strlen(v_memo(i)->alfabetica) < 5)
      return(0);
   b[0] = v_memo(i)->alfabetica[4];
   v_memo(i)->alfabetica[4] = '\0';
   o = atoi(v_memo(i)->alfabetica);
   v_memo(i)->alfabetica[4] = b[0];
   if (m == 0)
      mensaje(o,v_memo(i)->alfabetica+4);
   else
      if (m == 1)
         sprintf(v_memo(5004)->alfabetica,"%d",confirmar(o,v_memo(i)->alfabetica+5,b));
      else
         aviso(o,v_memo(i)->alfabetica+4);
      return(0);
}

/*
=========================================================================
*/

int f_s(f,m)
int f;
int m;
{
   mflagfi[f] |= m;
   return(0);
}

int f_n(f,m)
int f;
int m;
{
   mflagfi[f] &= (~m);
   return(0);
}

int f_v(f,m)
int f;
int m;
{
   if ( (mflagfi[f] & m) == 0)
      return(-1);
   else
      return(0);
}



int reset_pagactiva()
{
   if (memoplin > 2) memset(v_memo(5002)->alfabetica,'\0',11);
   return(0);
}


des_activa(f)
int f;
{
   if (memoplin > 2) memset(v_memo(5002)->alfabetica,'\0',11);
}

activa_pag(ipfi,n)
int ipfi,n;
{
   if (memoplin > 2) 
   {
      memcpy(v_memo(5002)->alfabetica,quita_path(v_mpfi(ipfi)->f_fichero),9);
      v_memo(5002)->alfabetica[10] = '0'+ n;
      v_memo(5002)->alfabetica[11] = '\0';
   }
   return(0);
}


/* pinppan
* pinta la pantalla o ventana correspondiente al def
*/
char *crea_control_edicion(FI_DEF *pfi,int campo,int wid,int id,int sid,int lineal,int posicional,int vacio,char *punt);
char **carga_mascara_pantalla(char **elem,char *pantalla,int ltam,int lpos,int partepor,int ajuste9);
char **realoca_multiplo(int n,int mul,char **p);
char *w_copia_stdid(char *pt);
int reduce_pantalla(char *pantalla,int *pi,int *pf);
int w_tam_stdid(void);

char *carga_pantalla(char *nomp);

/* de MOMENTO para pruebas */
extern int default_id;
extern int default_sid;
extern int default_wid;

extern int default_sid_padre;


static int check_ajuste_9(char *nompan) {
	FILE *f;
	char tmp[512];
	int ajuste9 = 0;

	sprintf(tmp,"%s.cfg",nompan);
	f = fopen(tmp,"r");
	if (!f)
		return 0;
	while(lee_linea(tmp,500,f)) {
		quitab(tmp);
		quitabp(tmp);
		if (!memcmp(tmp,"ajuste9:",8)) {
			ajuste9 = atoi(tmp+8);
		}
	}
	fclose(f);
	return (ajuste9);
}

int _pinppan(int ipfi,int n,int sidpadre,int reduccion)
{    
  char **elem;      
  char **p1;
  char *pt;
  int memoria;
  int partepor = 0;

  int numcam;
  int campini,campfin;
  int visual,campo;
  int salvaa;   

  int nelem = 0;
  int mas,l,k,ml,i,ajuste9[20];

  char *pantalla[20]; /* suficiente */
  char *pn;
  PAN_DEF *pans;



  activa_pag(ipfi,n);

  if (v_mpfi(ipfi)->f_pantaids[n] != -1)
  {
	  /*if (v_mpfi(ipfi)->f_id == default_id)*/
	  {
		  default_wid = v_mpfi(ipfi)->f_pantaids[n];
		  default_sid = v_mpfi(ipfi)->f_pantasids[n];
		  return default_sid;
	  }
	  /*
	  else
	  {
		  v_mpfi(ipfi)->f_pantaids[n] = -1;
	  }
	  */
  }  

  pantalla[0] = carga_pantalla(v_mpfi(ipfi)->f_pantalla[n].p_nombrepan);
  ajuste9[0] = check_ajuste_9(v_mpfi(ipfi)->f_pantalla[n].p_nombrepan);
  
  if (!pantalla[0])
  {
      return -1;
  }
 
  i = 1;
  if (v_mpfi(ipfi)->f_pantamix && (pans = v_mpfi(ipfi)->f_pantamix[n]))
  {     
     pantalla[1] = carga_pantalla(pans[0].p_nombrepan);
	 ajuste9[1] = check_ajuste_9(pans[0].p_nombrepan);
     i++;
  }  
  pantalla[i] = NULL;
  ajuste9[i] = 0;


  if (!n)
  {
      if (v_mpfirel(ipfi)->lineas == 0)
      {
         trata_lineas(ipfi,DIFERIDO,4,0,0);
      }
  }

  pn = v_mpfi(ipfi)->f_pantalla[n].p_nombrepan;
  pn += strlen(pn);
  if (pn[1] == 1 && pn[2] == 2 && pn[3] == ')' && pn[4] == ')') {
	partepor = atoi(pn+5);	
  }

  /*
  if (!strcmp(quita_path(v_mpfi(ipfi)->f_PathDef),"agifa024")) 
	  reduccion = 2;
	*/

  /*
  if (v_mpfi(ipfi)->f_npanta > 1 || (pn[1] == 1 && pn[2] == 2))
  */
  {  
        char tmpx[512];
        char *px = tmpx;        
        int tmpi = 0;       
        int pi,pf,opf;

		memset(tmpx,0,sizeof(tmpx));

        pi = v_mpfi(ipfi)->f_pantalla[n].p_posref;
        pf = pi + (v_mpfi(ipfi)->f_pantalla[n].p_posfin-101-(partepor*100));
        if (v_mpfi(ipfi)->f_pantamix && (pans = v_mpfi(ipfi)->f_pantamix[n]))
        {     
            opf = pans[0].p_posref + (pans[0].p_posfin-101);
            if ( (pans[0].p_posref/100) < (pi/100) )
            {
                pi = pans[0].p_posref/100*100 + pi%100;
            }
            if ( (pans[0].p_posref%100) < (pi%100) )
            {
                pi = pi/100*100 + pans[0].p_posref%100;
            }

            if ( (opf/100) > (pf/100) )
            {
                pf = opf/100*100 + pf%100;
            }
            if ( (opf%100) > (pf%100) )
            {
                pf = pf/100*100 + opf%100;
            }
        }
        else if (reduccion && v_mpfi(ipfi)->f_npanta == 1)
        {
           int qp,maxqc,qc;

           reduce_pantalla(pantalla[0],&pi,&pf);
        }

        
        v_mpfi(ipfi)->f_id = default_id;
        default_sid = (ipfi+1) + n * 1000;
        v_mpfi(ipfi)->f_pantasids[n] = default_sid;

        memcpy(px,&default_id,sizeof(int));
        px += sizeof(int);
        memcpy(px,&default_sid,sizeof(int));
        px += sizeof(int);
        memcpy(px,&sidpadre,sizeof(int));
        px += sizeof(int);        
        memcpy(px,&pi,sizeof(int));
        px += sizeof(int);        
        memcpy(px,&pf,sizeof(int));
        px += sizeof(int);

        if (pn[1] == 1 && pn[2] == 2) {
			if (pn[3] == ')' && pn[4] == ')')
			{
				if (!pn[7])
					sprintf(px,"%s  Pg.%d",v_mpfi(ipfi)->f_titulo,n+1);
				else
					strcpy(px,pn+7);
			}
		    else
				strcpy(px,pn+3);
        }
        else {
            if (v_mpfi(ipfi)->f_npanta > 1)
                sprintf(px,"%s  Pg.%d",v_mpfi(ipfi)->f_titulo,n+1);
            else
                px[0] = 0;
        }
		px += (strlen(px)+1);
		memset(px,0,sizeof(int));
		px += sizeof(int);
		memset(px,0xff,sizeof(int));
		px += sizeof(int);

        if (DespachadorExterno)
        {
            (*DespachadorExterno)((int)DESPACHO_PANTALLAS,(int)2,tmpx);        
            memcpy(&default_wid,tmpx,sizeof(int));        
        }
  }

  v_mpfi(ipfi)->f_pantaids[n] = default_wid;

  elem = realoca_multiplo(nelem,100,NULL);
  elem[nelem] = NULL;  

  if (hay_dat_permiso(v_mpfi(ipfi),-1))
  {
       campini = v_mpfi(ipfi)->f_pantalla[n].p_campoini;
       campfin = v_mpfi(ipfi)->f_pantalla[n].p_campofin;   
       for (numcam=campini; numcam<=campfin;numcam++) 
       {
          campo  = v_mpfi(ipfi)->f_taborder[numcam-1];
          visual = v_mpfi(ipfi)->f_campos[campo].c_mante.c_visual;
          if (visual == 0 && v_mpfi(ipfi)->f_campos[campo].c_tipo != NULOTIPO) 
          {
              if (v_mpfi(ipfi)->f_campopos[ campo ] != -1)
              {
                v_mpfi(ipfi)->f_campos[ campo ].c_mante.c_qp = v_mpfi(ipfi)->f_campopos[ campo ];
              }
              elem = realoca_multiplo(nelem,100,elem);
			  /*v_mpfi(ipfi)->f_campowids[campo] = -1;*/
			  if (partepor)
				 v_mpfi(ipfi)->f_campos[ campo ].c_mante.c_qp += (partepor*100);
              elem[nelem] = crea_control_edicion(v_mpfi(ipfi),campo,default_wid,default_id,default_sid,0,0,0,NULL);
			  if (partepor)
				 v_mpfi(ipfi)->f_campos[ campo ].c_mante.c_qp -= (partepor*100);
              nelem++;
              elem[nelem] = NULL;
          }
       }
  }

  for (i = 0;pantalla[i];i++)
  {
    if (i == 1 && v_mpfi(ipfi)->f_pantamix && (pans = v_mpfi(ipfi)->f_pantamix[n]))
    { 
        elem = carga_mascara_pantalla(elem,pantalla[i],((v_mpfi(ipfi)->f_pantalla[n].p_posref/100)-5)*100+80,pans[0].p_posref,0,ajuste9[i]);
        /*pantalla[i],pans[0].p_posfin*/
    }
    else
    {
        elem = carga_mascara_pantalla(elem,pantalla[i],v_mpfi(ipfi)->f_pantalla[n].p_posfin-(partepor*100),v_mpfi(ipfi)->f_pantalla[n].p_posref+(partepor*100),partepor,ajuste9[i]);
    }
    free(pantalla[i]);
  }

  if (elem)
  {
      char *p0;
      int *p3;  
      p1 = elem;
      l = 0;
      memoria = 0;
      while(*p1)
      {
          p3 = (int *)*p1;
          if (p3[4] != 2 && p3[4] != 7)
          {
              memoria += *((int *)(*p1));
              l++;
          }
          p1++;
      }
      pt = malloc(memoria);
      p0 = pt;
      p1 = elem;
      while(*p1)
      {
          p3 = (int *)*p1;
          if (p3[4] != 2 && p3[4] != 7)
          {
              memcpy(p0,*p1,*((int *)(*p1)));
              p0 += *((int *)(*p1));
          }
          free(*p1);
          p1++;
      }

      if (DespachadorExterno)
      {
          int *p1;  
          char *pp = pt;

          l--;

          (*DespachadorExterno)((int)DESPACHO_DIF_CCONTROL,l,pt);
          
          do
          {  
              p1 = (int *)pp;

              if (p1[4] == 2 || p1[4] == 7)
              {
                  v_mpfi(ipfi)->f_campowids[ p1[11] ] = p1[10];
              }
              
              l--;
              pp = (void *)(((char *)pp) + p1[0]);
          } while (l >= 0);          
      }      

      free(pt);
      free(elem);
  }

  /*
  if (v_mpfi(ipfi)->f_npanta > 1 || (pn[1] == 1 && pn[2] == 2)) 
  */
  if (DespachadorExterno)
  {
    char tmpx[512];
    char *px = tmpx; 

	memset(tmpx,0,sizeof(tmpx));
    

    memcpy(px,&default_wid,sizeof(int));
    px += sizeof(int);
    memcpy(px,&default_id,sizeof(int));
    px += sizeof(int);
    memcpy(px,&default_sid,sizeof(int));
    px += sizeof(int);
	*((int *)px) = 0;
	px += sizeof(int);

    (*DespachadorExterno)((int)DESPACHO_PANTALLAS,(int)4,tmpx);
    
  }

  /* CambiaElCampo(v_mpfi(ipfi),-1); /* Que cree la referencia de comparacion */

  agi_cola_timer();

  return default_sid;
}


pinppan(ipfi,n)
{
   return(_pinppan(ipfi,n,default_sid_padre,0));
}


int cambia_ind_curr(_pfi,nclave)
FI_DEF *_pfi;
int nclave;
{
   if (apunta_ind(_pfi,++nclave,ISCURR,0) < 0)
      apunta_ind(_pfi,nclave,ISFIRST,0);
}

/* ============================================================================
*/

int pinta_campo(ipfi,c,faclin,a)
int ipfi;
int c,faclin;
int a;/* atributo *//* se asume que 0 es el por defecto */

{
   int pp,l = 0;
   int salvaa = v_mpfi(ipfi)->f_campos[c-1].c_fflag;
   int npan = -1;
   int orden = -1;
   
   if (v_mpfi(ipfi)->f_campos[c-1].c_mante.c_visual != 0) return(0);
   if (v_mpfi(ipfi)->f_campos[c-1].c_tipo == NULOTIPO)
   {
	   if (v_mpfi(ipfi)->f_camporden)
		   orden = v_mpfi(ipfi)->f_camporden[c-1];
	   if (orden >= 0 && orden < v_mpfi(ipfi)->f_ncampos && DespachadorExterno && v_mpfi(ipfi)->f_campowids[c-1] > -1 /*&&
		   (v_mpfi(ipfi)->f_orderflags[orden]&ORDER_FLAG_EVENTO)*/)
	   {
		   if (DespachadorExterno)
				(*DespachadorExterno)((int)DESPACHO_REFRESCACONTROL,v_mpfi(ipfi)->f_campowids[c-1],NULL);
	   }
	   return 0;
   }

   if ((v_mpfi(ipfi)->f_campos[c-1].c_mante.c_lineal & FGV_C_GRID))
   {
	   printcam(v_mpfi(ipfi),c);
	   return 0;
   }
   npan = pantalla_segun_campo(v_mpfi(ipfi),c-1);          
   if (npan < 0 || v_mpfi(ipfi)->f_pantaids[npan] == -1)
   {
       return 0;
   }
   
   if (actualiza_cam(ipfi,c,faclin,14) == -1) 
   {
      v_mpfi(ipfi)->f_campos[c-1].c_fflag = salvaa;
      return(0);
   }
   
   if ((v_mpfi(ipfi)->f_campos[c-1].c_mante.c_lineal & C_LINEAL) != 0 && faclin > 0)
   {
      if (v_mpfi(ipfi)->f_fi_modo_lineas)
      {
         pp = v_mpfi(ipfi)->f_campos[c-1].c_mante.c_qp + ((faclin - 100)/100) * v_mpfi(ipfi)->f_fi_modo_lineas;
      }
      else
         pp = v_mpfi(ipfi)->f_campos[c-1].c_mante.c_qp + faclin - 100;
      l = 1;
   } 
   else {
      pp = v_mpfi(ipfi)->f_campos[c-1].c_mante.c_qp;
	  faclin = 0;
   }
   cursor(pp);
   if (!a)
   {
      if (Patrib != ' ')
         atributo('0');
      
      if (!l)
         a_atri_campo(v_mpfi(ipfi),c-1,' ',0,0,0);
      else
      {
         if (rentra_cc != rentra_cf)
            Color(rentra_cc,rentra_cf);
         atributo(rentra_atrib);              
         a_atri_campo(v_mpfi(ipfi),c-1,rentra_atrib,0,rentra_cc,rentra_cf);
      }            
   }
   else {
      if (rentra_cc != rentra_cf)
         Color(rentra_cc,rentra_cf);
      atributo(rentra_atrib);
   }
   
   if (faclin)
   {
     printcam_x(v_mpfi(ipfi),c,3);
   }
   else
   {
     printcam(v_mpfi(ipfi),c);
   }
   
   if (!a)
   {
      if (!l)
         a_atri_campo(v_mpfi(ipfi),c-1,' ',1,0,0);
      else
      {
         a_atri_campo(v_mpfi(ipfi),c-1,rentra_atrib,1,rentra_cc,rentra_cf);
         atributo(tolower(rentra_atrib));                                     
      } 
   }
   else
      atributo(tolower(rentra_atrib));
   v_mpfi(ipfi)->f_campos[c-1].c_fflag = salvaa;
   cursor(pp);
   return(0);
}

/*
 * pinta_datos
 * pinta los datos de los campos de un fichero, pantalla a pantalla
 * devuelve 0
 */
_pinta_datos(ipfi,modo,ppp,pflag)
int ipfi,modo;
int *ppp;
int pflag;
{
   int qj;
   int panini,panfin;
   register int numpan;
   int pintar;
   int pintada1;
   int estoyen;
   int salida;
   int panlin = -1;
   char tmp[81];
   int ipan;
   
   panfin = v_mpfi(ipfi)->f_npanta;
   if (v_mpfirel(ipfi)->lineas == 0) {panlin = panfin;panfin++;}
   
   if (ppp)
   {
      ipan = *ppp;
      pintada1 = pflag;
   }
   else
   {
      ipan = 0;
      pintada1 = 1;
   }
   if (ipan < 0)
      ipan =0;
   if (ipan >= panfin)
      ipan = panfin -1;
   for (panini=0,numpan=ipan;numpan<panfin;numpan++) {
      
      
      if ((numpan < panini || pintada1 == 1))
      {
         pintar = 0;           /* no pintar la pantalla otra vez */
         if (!pintada1)
            numpan = panini;
         pintada1 = 0;
      }
      else
         pintar = -1;
      estoyen = numpan;
      
      
      if (DespachadorExterno)
      {
          /*
         if (!_M_EXTRA_AVANCE)
            (*DespachadorExterno)((int)DESPACHO_QUEPANTALLA,(int)estoyen,v_mpfi(ipfi)->f_titulo);
            */
      }
      
      
      
      if (numpan == panlin) 
	  {
         trata_lineas(ipfi,CONSULTA,0,0,0);
         numpan -=1;
         estoyen = numpan;
         if (trata_lineas(ipfi,CONSULTA,3,0,0) != 0) 
		 {
            numpan -=1;
            continue;
         }
      }
      else
	  {
         if (pintar) 
		 {
            pinppan(ipfi,numpan);                  /* pintar pantalla */
            cabecera_v(NULL,proceso,titulo,empresa);
         }
         salida = 0;
         prin_pan(ipfi,numpan);
         if (panfin == 1)
            break;
      }
      tmp[0] = 0;
      if (estoyen <= panfin)
      {
         if (panlin == -1 || estoyen <= panlin)
         {
            strcat(tmp,"Pulse <Av.Pag.> para Siguiente Página");
         }
         else
         {
            strcat(tmp,"Pulse <Av.Pag.> para Lineas");
         }
      }
      if (estoyen > panini)
      {
         if ((estoyen+1) < panfin)
         {
            strcat(tmp,",<Re.Pag.> para anterior Página");
         }
         else
         {
            strcat(tmp,"Pulse <Re.Pag.> para anterior Página");
         }
      }
      Ar();
      mensaje(0,tmp);
      Cr();
      if (ppp && !estoyen) 
      {
         *ppp = estoyen;
         break;
      }
      
      fin_botones_();
      
      qj = parada();
      if (qj == SOLICITA_AVANCE)
      {	   
         numpan += _M_EXTRA_AVANCE;
         _M_EXTRA_AVANCE = 0;	   
         continue;
      }
      else
         if (qj == SOLICITA_RETROCESO)
         {
            numpan -= 2;
            numpan -= _M_EXTRA_AVANCE;
            _M_EXTRA_AVANCE = 0;	   
            continue;
         }
         
         
         if (qj == 3 || qj == 5)         /* abajo, pgdn */
            continue;
         else if (qj == 2 || qj == 4) {  /* arriba, pgup */
            numpan -= 2;
         }
         else if (qj == 7) {             /* abort  */
            salida = -1;
            break;
         }
         else {                          /* fin    */
            break;
         }
   }
   
   
   if (estoyen != 0) {  /* estoy en una pantalla != inicial (=0) */
      pinppan(ipfi,0);
      if (modo == 0) prin_pan(ipfi,0); /* pintar pantalla */
      if (ppp) {
         *ppp = 0;
         tmp[0] = 0;
         if (panfin > 1)
         {
            if (panlin == -1 || panlin > 1)
            {
               strcat(tmp,"Pulse <Av.Pag.> para Siguiente Página");
            }
            else
            {
               strcat(tmp,"Pulse <Av.Pag.> para Lineas");
            }
         }
         Ar();
         mensaje(0,tmp);
         Cr();
      }
   }
   if (!ppp)
   {
      Ar();Bl(4);Cr();   /* borrar mensaje */
   }
   return(salida);
}

pinta_datos(ipfi,modo)
{
    int xi;
    int oid = default_wid;
    int osid = default_sid;

    if (ejecuta_otro(ipfi,13,O_NOR,0,0) != -1) 
    {
        for (xi = 0;xi < v_mpfi(ipfi)->f_npanta;xi++)
        {
            printpan__(ipfi,xi,0);
            printprel(ipfi,xi);
            /*prin_pan(ipfi,xi);*/
        }
        trata_lineas(ipfi,DIFERIDO,0,0,0);
    }

    default_wid = oid;
    default_sid = osid;

    agi_cola_timer();

/*   return(_pinta_datos(ipfi,modo,NULL,0));*/
}

int pin_all_pan(int ipfi,int modo)
{
int i;

    for (i = 0;i < v_mpfi(ipfi)->f_npanta;i++)
    {
       if (v_mpfi(ipfi)->f_pantaids[i] == -1)
       {
          pinppan(ipfi,i);		  
          printpan__(ipfi,i,0);		  
          if (modo)
             printprel(ipfi,i);          
       }
    }
    if (modo)
    {
       trata_lineas(ipfi,DIFERIDO,0,0,0);
    }

    default_wid = v_mpfi(ipfi)->f_pantaids[0];
    default_sid = v_mpfi(ipfi)->f_pantasids[0];


    agi_cola_timer();
    /*trata_lineas(ipfi,DIFERIDO,0,0,0);*/
    return 0;
}



int AjustaPanLin(int ipfi,int f)
{
    int plin,i;
    PAN_DEF *ptmp;

    plin = -((v_mpfirel(f)->lineas%160)%6);
	if (plin < 0)
		plin = 0;
	else if (plin == 4)
	{
		plin = -((v_mpfirel(f)->lineas%160)/6);			
		if (plin > 0)
		{
			plin--;
		}
		else
		{
			plin = v_mpfi(ipfi)->f_npanta;
		}
	}

    if (plin > v_mpfi(ipfi)->f_npanta)
    {
        plin = v_mpfi(ipfi)->f_npanta;
		i = -v_mpfirel(f)->lineas;
		v_mpfirel(f)->lineas  = -((i/160)*160+((i%160)/6)*6+plin);
    }

    if (plin == v_mpfi(ipfi)->f_npanta)
    {
        int pini,pfin,c;
        ptmp = (PAN_DEF *)malloc(sizeof(PAN_DEF)*(v_mpfi(ipfi)->f_npanta+1));
        memcpy(ptmp,v_mpfi(ipfi)->f_pantalla,sizeof(PAN_DEF)*v_mpfi(ipfi)->f_npanta);

        ptmp[plin] = v_mpfi(f)->f_pantalla[0];

        v_mpfi(ipfi)->f_taborder  = (int *)realloc(v_mpfi(ipfi)->f_taborder,sizeof(int) * (v_mpfi(ipfi)->f_ncampos+ptmp[0].p_campofin+3));
        v_mpfi(ipfi)->f_orderflags  = (int *)realloc(v_mpfi(ipfi)->f_orderflags,sizeof(int) * (v_mpfi(ipfi)->f_ncampos+ptmp[0].p_campofin+3));

        pini = ptmp[v_mpfi(ipfi)->f_npanta-1].p_campofin+1;
        pfin = pini-1;        
        for (i = ptmp[0].p_campoini-1;i < ptmp[0].p_campofin;i++)
        {
            c = v_mpfi(ipfi)->f_taborder[i];
            if ( (v_mpfi(ipfi)->f_campos[c].c_mante.c_qp/100) < (ptmp[plin].p_posref/100) )
            {
                pfin++;
                v_mpfi(ipfi)->f_taborder[pfin-1] = c;
                v_mpfi(ipfi)->f_orderflags[pfin-1] |= ORDER_FLAG_COPIA;
            }
        }
        v_mpfi(ipfi)->f_taborder[pfin] = -1;
        v_mpfi(ipfi)->f_taborder[pfin+1] = -1;
        ptmp[plin].p_campoini = pini;
        ptmp[plin].p_campofin = pfin;
        
        /* ojo , de momento solo una pantalla .... */
        v_mpfi(ipfi)->f_pantamix[plin] = (PAN_DEF *)malloc(sizeof(PAN_DEF));
        *(v_mpfi(ipfi)->f_pantamix[plin]) = v_mpfi(ipfi)->f_pantalla[0];

        free((char *)v_mpfi(ipfi)->f_pantalla);

        v_mpfi(ipfi)->f_pantalla = ptmp;
        v_mpfi(ipfi)->f_npanta++;

        v_mpfi(ipfi)->f_pantaids = (int *)realloc((void *)v_mpfi(ipfi)->f_pantaids,sizeof(int) * (v_mpfi(ipfi)->f_npanta+1));
        v_mpfi(ipfi)->f_pantaids[v_mpfi(ipfi)->f_npanta-1] = -1;
        v_mpfi(ipfi)->f_pantaids[v_mpfi(ipfi)->f_npanta] = -1;
        

        v_mpfi(ipfi)->f_pantasids = (int *)realloc((void *)v_mpfi(ipfi)->f_pantasids,sizeof(int) * (v_mpfi(ipfi)->f_npanta+1));
        v_mpfi(ipfi)->f_pantasids[v_mpfi(ipfi)->f_npanta-1] = -1;
        v_mpfi(ipfi)->f_pantasids[v_mpfi(ipfi)->f_npanta] = -1;
    }

    return plin;
}

static int borrar_las_memos(ipfi)
int ipfi;
{
   char tmp[MAXPATH];
   int i,f = v_mpfi(ipfi)->f_ncampos;
   
   for (i = 0;i < f;i++) 
   {
      if (v_mpfi(ipfi)->f_campos[i].c_qc == -2) {
         strcpy(tmp,v_mpfi(ipfi)->f_campos[i].c_mante.c_defecto);
         trata_memo(cambia_path_fich(tmp),
            (long)ld_doble(i,v_mpfi(ipfi)),
            6, /* BORRAR !! */
            505,2075,0,9999,"",0,NULL,0);
      }
   }
}

int abre_la_doble(ipfi,clave)
int ipfi,clave;
{
   char tmp[50];
   char guarda[60];
   int i;
   int fd;
   int ff = mflagfi[ipfi];
   int ret = -1;
   long _isrecnum = isrecnum;
   
   strcpy(guarda,v_mpfi(ipfi)->f_fichero);
   strcpy(tmp,quita_path(v_mpfi(ipfi)->f_fichero));
   *quita_path(v_mpfi(ipfi)->f_fichero) = 0;
   i = strlen(v_mpfi(ipfi)->f_fichero);
   fd = v_mpfi(ipfi)->f_fd;
   if (i)
			{
      v_mpfi(ipfi)->f_fd = -1;
      if (es_la_doble_)
      {
         i-=2;
         if (v_mpfi(ipfi)->f_fichero[i] != '!')
         {
            aviso(0,"CONTROL DOBLE INCORRECTO EN FASE A1");
         }
         v_mpfi(ipfi)->f_fichero[i] = '/';
         v_mpfi(ipfi)->f_fichero[i+1] = 0;     
      }
      else
      {
         i--;
         v_mpfi(ipfi)->f_fichero[i] = '!';
         strcat(v_mpfi(ipfi)->f_fichero,"/");
      }
      strcat(v_mpfi(ipfi)->f_fichero,tmp);
      if (abre1_rel(ipfi) >= 0)
      {
         ret = v_mpfi(ipfi)->f_fd;
         cambia_ind(v_mpfi(ipfi),clave);
      }
			}
   v_mpfi(ipfi)->f_fd = fd;
   strcpy(v_mpfi(ipfi)->f_fichero,guarda);
   mflagfi[ipfi] = ff;
   isrecnum = _isrecnum;
   return(ret);
}

int alta_del_doble(ipfi)
int ipfi;
{
			char tmp[50];
         char guarda[60];
         int i;
         int fd;
         int ff = mflagfi[ipfi];
         long _isrecnum = isrecnum;
         
         if (!v_mpfi(ipfi)->f_titulo) return(0);
         i = strlen(v_mpfi(ipfi)->f_titulo);
         if (!i) return(0);
         i--;
         while(i && v_mpfi(ipfi)->f_titulo[i] == ' ') i--;
         if (v_mpfi(ipfi)->f_titulo[i] != ',') return(0);
         
         strcpy(guarda,v_mpfi(ipfi)->f_fichero);
         strcpy(tmp,quita_path(v_mpfi(ipfi)->f_fichero));
         *quita_path(v_mpfi(ipfi)->f_fichero) = 0;
         i = strlen(v_mpfi(ipfi)->f_fichero);
         fd = v_mpfi(ipfi)->f_fd;
         if (i)
         {
            v_mpfi(ipfi)->f_fd = -1;
            i--;
            v_mpfi(ipfi)->f_fichero[i] = '!';
            strcat(v_mpfi(ipfi)->f_fichero,"/");
            strcat(v_mpfi(ipfi)->f_fichero,tmp);
            if (abre1_rel(ipfi) >= 0)
            {
               graba_nuevo(v_mpfi(ipfi));
               cierra1_rel(ipfi);
            }
         }
         v_mpfi(ipfi)->f_fd = fd;
         strcpy(v_mpfi(ipfi)->f_fichero,guarda);
         mflagfi[ipfi] = ff;
         isrecnum = _isrecnum;
         return(0);
}

int graba_el_doble(ipfi)
int ipfi;
{
   char tmp[50];
   char guarda[60];
   int i;
   int fd;
   int ff = mflagfi[ipfi];
   long _isrecnum = isrecnum;
   char *buf;
   
   if (!v_mpfi(ipfi)->f_titulo) return(0);
   i = strlen(v_mpfi(ipfi)->f_titulo);
   if (!i) return(0);
   i--;
   while(i && v_mpfi(ipfi)->f_titulo[i] == ' ') i--;
   if (v_mpfi(ipfi)->f_titulo[i] != ',') return(0);
   
   
   strcpy(guarda,v_mpfi(ipfi)->f_fichero);
   strcpy(tmp,quita_path(v_mpfi(ipfi)->f_fichero));
   *quita_path(v_mpfi(ipfi)->f_fichero) = 0;
   i = strlen(v_mpfi(ipfi)->f_fichero);
   fd = v_mpfi(ipfi)->f_fd;
   if (i)
   {
      v_mpfi(ipfi)->f_fd = -1;
      i--;
      v_mpfi(ipfi)->f_fichero[i] = '!';
      strcat(v_mpfi(ipfi)->f_fichero,"/");
      strcat(v_mpfi(ipfi)->f_fichero,tmp);
      if (abre1_rel(ipfi) >= 0)
	  {
         buf = malloc(v_mpfi(ipfi)->f_lfich+1);
         memcpy(buf,v_mpfi(ipfi)->f_buf,v_mpfi(ipfi)->f_lfich);	  
         if (lee_reg(v_mpfi(ipfi),ISEQUAL+ISLOCK) >= 0)
         {
            memcpy(v_mpfi(ipfi)->f_buf,buf,v_mpfi(ipfi)->f_lfich);
            graba_actual(v_mpfi(ipfi));
            libera_ind(v_mpfi(ipfi));
         }
         memcpy(v_mpfi(ipfi)->f_buf,buf,v_mpfi(ipfi)->f_lfich);
         cierra1_rel(ipfi);
	  }
   }
   v_mpfi(ipfi)->f_fd = fd;
   strcpy(v_mpfi(ipfi)->f_fichero,guarda);
   mflagfi[ipfi] = ff;
   isrecnum = _isrecnum;
   return(0);
}

int trata_reg(f,o,b,t,h,d)
int f,o,b,t,h;
char *d;
{
   int e = 0;
   int r = t%10;
   int m = (t%100)/10;
   int a = (t%1000)/100;
   int n = (t%10000)/1000;      
   int repite = 0;
   int epitando = 0;
   int libera = 0;
   
   do {
/*
#ifdef ESDOS
						if (timer_especial_int)
                     (*timer_especial_int)();
#endif
*/
                  
                  if (h == 0) 
				  {
                     e = lee_reg(v_mpfi(f),o+b);
                  }
                  else
                     if (h == 1) {
                        if (o == 3) {o = 2;b = ISLOCK;} /* reposicionar bloqueando */
                        if (o == 0) {
                           if (mimodo_debug || (logger_salida && *logger_salida))
                              e = t_graba_actual(v_mpfi(f));
                           else
                              e = graba_actual(v_mpfi(f));
                        }
                        else {
                           if (mimodo_debug || (logger_salida && *logger_salida))
                              e = t_graba_nuevo(v_mpfi(f));
                           else
                              e = graba_nuevo(v_mpfi(f));
#ifdef INNECESARIO
                           /* Parche para aplicaciones que graban la linea nueva 'actual' */
                           if (e < 0 && iserrno == EKEXISTS)
                           { 
                              if (ixx_es_el_curr(v_mpfi(f)->f_fd,v_mpfi(f)->f_buf) == 1)
                              {
                                 e = graba_actual(v_mpfi(f));
                              }
                              else
                              {
                                 e = -1;
                                 iserrno = EKEXISTS;
                              }
                           }
                           /***************************************************************/		
#endif
                           if (limitacion_especial) 
                           {
                              if (esta_limitado(f,isrecnum,NULL,0))
                              {
                                 borra_reg(v_mpfi(f));
                                 e = -1;
                                 iserrno = 999;
                              }
                           }
                           if (!e && va_con_doble_ && !es_la_doble_)
                           {
                              alta_del_doble(f);
                           }
                        }
                        if (o == 2 && e == 0) {
                           e = lee_reg(v_mpfi(f),ISEQUAL+b); /* reponer el courrent */
                        }
                     }
                     else
                        if (h == 2) 
                        {
                           if (mimodo_debug || (logger_salida && *logger_salida))
						   {
							   if (o == 0)
								  e = t_borra_actual(v_mpfi(f));
							   else
								  e = t_borra_reg(v_mpfi(f));
						   }
						   else
						   {
							   if (o == 0)
								  e = borra_actual(v_mpfi(f));
							   else
								  e = borra_reg(v_mpfi(f));
						   }
                           if (!e)
                              borrar_las_memos(f);
                        }
                        if (e != 0) 
                        {                           
                           if (m != 4 && repite == 0) 
						   {
                              if (iserrno == ELOCKED || m == 2 || (m == 1 && lista_error() == 2)) 
							  {								 
								 libera = -1;
                                 if (!(iserrno && (r == 2 || (r == 1 && iserrno == ELOCKED))) ) 
                                 {
                                     if (!epitando)
                                        epitando = pinta_error(f,d,1);
                                 }
                                 else
                                 {
									 libera = 0;
                                     if (!epitando)
									 {
                                        epitando = pinta_error(f,d,0);										
									 }
                                 }                                 
                              }
                           }
                           if (libera != -1)
                           {
                                if ((repite = captura_salida(r,libera)) == 0) 
                                {
                                    break;
                                }
                           }
                           else
                           {
                               repite = 0;
                               break;
                           }
                        }
   } while(e != 0);
   /*
   if (libera == -1) 
   {
   */
        if (epitando && DespachadorExterno)
        {       
             char tmp[512];
             int *p1 = (int *)tmp;
             tmp[sizeof(int)*2] = 0;
             p1[0] = sizeof(int)*2 + 2;
             p1[1] = -1;
			 if (DespachadorExterno)
				(*DespachadorExterno)((int)DESPACHO_MAINCOMMAND,ID_ERRORFICHERO,(void *)tmp);
             agi_cola_timer();
        }    
   /*}*/
   if (e) 
   {
						f_n(f,RENBUF); /* ante cualquier error se quita la marga de que esta leido */
                  /* para las relaciones y tiene que volverlo a leer */
   }
   return(e);
}

leer_reg(f,o,b,t)
int f,o,b,t;
{
   return(trata_reg(f,o,b,t,0,"Leer ficha"));
}
grabar_reg(f,o,t)
int f,o,t;
{
   return(trata_reg(f,o,0,t,1,"Grabar ficha"));
}
borrar_reg(f,o,t)
int f,o,t;
{
   return(trata_reg(f,o,0,t,2,"Borrar ficha"));
}


/*
* printprel
*    pintar campos a pintar en la pantalla 'npan' de las relaciones de
*    esta pantalla del fichero 'ipfi'.
*/
int printprel(ipfi,npan)
int ipfi;   /* puntero al fichero */
int npan;      /* numero de la pantalla a pintar 0,... */
{
   register int k,i,l,a,h;   
   int di = default_id;
   int ds = (ipfi+1) + npan * 1000;
   SALVA_IDS
   
   default_wid = v_mpfi(ipfi)->f_pantaids[npan];
   default_id  = v_mpfi(ipfi)->f_id;
   default_sid = v_mpfi(ipfi)->f_pantasids[npan];
   di = v_mpfi(ipfi)->f_id;
   ds = v_mpfi(ipfi)->f_pantasids[npan];
   
   for (k=0;k<v_mpfirel(ipfi)->nrelpin;k++) 
   {
      if ( v_mpfirel(ipfi)->relpin[k].qp  > 0 ) 
	  {
         i = v_mpfirel(ipfi)->relpin[k].campo;
         if ((v_mpfi(ipfi)->f_campos[i].c_mante.c_actual / 1000) == npan) 
         {
			l = v_mpfirel(ipfi)->relpin[k].nfirel;
            a = f_v(l,RABIERTO);
            if (abre1_rel(l) == 0) 
			{
               h = clave_rel(v_mpfirel(ipfi)->relpin[k].numrela,l,-1,0,ipfi,-1,DIFERIDO,FALSO);
               if (h >= 0)
                   h = leer_reg(l,ISEQUAL,0,1);
               if (h == -1 || (h == -2 && f_v(l,RENBUF) != 0)) 
			   {
                   defecto_ficha(v_mpfi(l));
                   f_n(l,RENBUF);
               }
               else
                   f_s(l,RENBUF);
			   
               entra_cedit(v_mpfi(l),v_mpfirel(ipfi)->relpin[k].nficampo,-1,0,l,0,NULL,v_mpfirel(ipfi)->relpin[k].qp,di,ds,0);
			   
               /*
               cursor(v_mpfirel(ipfi)->relpin[k].qp);
               printusi_i(v_mpfi(l),v_mpfirel(ipfi)->relpin[k].nficampo+1);
			   */
               
			}
            if (a == -1) 
			   cierra1_rel(l);
         }
      }
   }

   REPON_IDS
   return(0);
}


/*
* printpan
* pinta el contenido de los campos de una pantalla
*/
printpan__(ipfi,numpan,cola)
int ipfi;
int numpan;
int cola;
{
   int numcam;
   int campini,campfin;
   int visual,campo;
   int salvaa;
   int id;
   int sid;   
   int o_m;
   char buff[10000];
   int *p1 = (int *)buff;  
   char **elem;
   int nelem = 0;   
   int memoria = 0;

   if (v_mpfi(ipfi)->f_pantaids[numpan] == -1)
   {
       return -1;
       /*return pinppan(ipfi,numpan);*/
   }
   if (se_ha_requerido_usuario < 1)
	   se_ha_requerido_usuario = 1;
   default_wid = v_mpfi(ipfi)->f_pantaids[numpan];
   default_id  = v_mpfi(ipfi)->f_id;
   default_sid = v_mpfi(ipfi)->f_pantasids[numpan];

   id  = v_mpfi(ipfi)->f_id;
   sid = v_mpfi(ipfi)->f_pantasids[numpan];   

   if (!hay_dat_permiso(v_mpfi(ipfi),-1)) return(-1);
   
   campini = v_mpfi(ipfi)->f_pantalla[numpan].p_campoini;
   campfin = v_mpfi(ipfi)->f_pantalla[numpan].p_campofin;
   
   
   if (rentra_sal_cc != rentra_sal_cf)
      Color(rentra_sal_cc,rentra_sal_cf);
   atributo(rentra_sal_atrib);
   
   
   
   elem = realoca_multiplo(nelem,100,NULL);
   elem[nelem] = NULL;
   

   for (numcam=campini; numcam<=campfin;numcam++) 
   {
      campo = v_mpfi(ipfi)->f_taborder[numcam-1];
	  if (campo < 0 || campo >= v_mpfi(ipfi)->f_ncampos)
	  {
		  continue;
	  }
      visual = v_mpfi(ipfi)->f_campos[campo].c_mante.c_visual;

      if (v_mpfi(ipfi)->f_campopos[ campo ] != -1)
      {
         v_mpfi(ipfi)->f_campos[ campo ].c_mante.c_qp = v_mpfi(ipfi)->f_campopos[ campo ];
      }


      if (visual == 0 /*&& v_mpfi(ipfi)->f_campos[campo].c_tipo != NULOTIPO*/) 
	  {
         salvaa = v_mpfi(ipfi)->f_campos[campo].c_fflag;
		 if (v_mpfi(ipfi)->f_campos[campo].c_tipo == NULOTIPO)
		 {
			   if (DespachadorExterno && v_mpfi(ipfi)->f_campowids[campo] > -1 /*&& (v_mpfi(ipfi)->f_orderflags[numcam-1]&ORDER_FLAG_EVENTO)*/)
			   {				   
				   (*DespachadorExterno)((int)DESPACHO_REFRESCACONTROL,v_mpfi(ipfi)->f_campowids[campo],NULL);
			   }			   
		 } else if ( (v_mpfi(ipfi)->f_orderflags[numcam-1]&ORDER_FLAG_COPIA) )
         {			

            a_atri_campo(v_mpfi(ipfi),campo,rentra_sal_atrib,0,rentra_sal_cc,rentra_sal_cf);
            o_m = v_mpfi(ipfi)->f_campos[campo].c_mante.c_modif;
            v_mpfi(ipfi)->f_campos[campo].c_mante.c_modif = 1;
            cursor(v_mpfi(ipfi)->f_campos[campo].c_mante.c_qp);
            printcam_x(v_mpfi(ipfi),campo+1,2);
            v_mpfi(ipfi)->f_campos[campo].c_mante.c_modif = o_m;
            a_atri_campo(v_mpfi(ipfi),campo,rentra_sal_atrib,1,rentra_sal_cc,rentra_sal_cf);
         }
         else if (actualiza_cam(ipfi,campo+1,0,14) != -1) 
         {     
            a_atri_campo(v_mpfi(ipfi),campo,rentra_sal_atrib,0,rentra_sal_cc,rentra_sal_cf);
          
            /*
            elem = realoca_multiplo(nelem,100,elem);
            elem[nelem] = crea_control_edicion(v_mpfi(ipfi),campo,-1,id,sid,0,0,0,NULL);
            nelem++;
            elem[nelem] = NULL;
            */
            
            if (DespachadorExterno && CambiaElCampo(v_mpfi(ipfi),campo,0))
            {               
			    if ((v_mpfi(ipfi)->f_campos[campo].c_mante.c_lineal & FGV_C_GRID))
				{
	                printcam(v_mpfi(ipfi),campo+1);	                
				}
				else
				{
					visual = v_mpfi(ipfi)->f_campos[campo].c_mante.c_visual;
					if (visual)
					{
						if (v_mpfi(ipfi)->f_campowids[ campo ] > -1)
						{
							if (DespachadorExterno)
								(*DespachadorExterno)((int)DESPACHO_DESTRUYECONTROL,v_mpfi(ipfi)->f_campowids[ campo ],NULL);
						   v_mpfi(ipfi)->f_campowids[ campo ] = -1;
						}
					}
					else
					{
						crea_control_edicion(v_mpfi(ipfi),campo,-1,id,sid,0,0,0,buff);
						elem[nelem] = malloc(p1[0]);
						memcpy(elem[nelem],buff,p1[0]);
						memoria += p1[0];
						nelem++;
						elem = realoca_multiplo(nelem,100,elem);
						elem[nelem] = NULL;
					/*
					   crea_control_edicion(v_mpfi(ipfi),campo,-1,id,sid,0,0,0,buff);
					   (*DespachadorExterno)((int)DESPACHO_CEDIT,0,buff);
					   v_mpfi(ipfi)->f_campowids[ campo ]  = p1[10];
					 */
					}
				}
            }             
            

            a_atri_campo(v_mpfi(ipfi),campo,rentra_sal_atrib,1,rentra_sal_cc,rentra_sal_cf);
         }
         v_mpfi(ipfi)->f_campos[campo].c_fflag = salvaa;
      }
   }
   
   atributo(tolower(rentra_sal_atrib));

   
  if (elem)
  {
      char *p0;
      char *pt;            
      char **p1;
	  int flag;
      p1 = elem;

	  flag = (nelem << 16);

      pt = malloc(memoria);
      p0 = pt;
      p1 = elem;
      while(*p1)
      {
          memcpy(p0,*p1,*((int *)(*p1)));
          p0 += *((int *)(*p1));
          free(*p1);
          p1++;
      }

      if (nelem && DespachadorExterno)
      {
          int *p1;
          char *pp = pt;         		  

          (*DespachadorExterno)((int)DESPACHO_DIF_CEDIT,flag,pt);
          
          do 
		  {  
              p1 = (int *)pp;

              v_mpfi(ipfi)->f_campowids[ p1[11] ] = p1[10];
              
              nelem--;
              pp = (void *)(((char *)pp) + p1[0]);

          } while (nelem > 0);
      }     
      free(pt);
      free(elem);
  }     
   
   if (cola)
      agi_cola_timer();

   return(default_sid);
}


int printpan(ipfi,numpan)
{
return printpan__(ipfi,numpan,1);
}
/*
* prin_pan
*    pintar datos de la pantalla del fichero 'ipfi' de numero 'npan' ,
*    datos = campos del fichero y campos a pintar de las relaciones
*/
prin_pan(ipfi,npan)
int ipfi; /* puntero al fichero */
int npan;    /* numero de la pantalla a pintar 0,... */
{
   int m;
   if (npan < 0) {npan = 0;m = 2;} else m = 0;   
   
   if (ejecuta_otro(ipfi,13,O_NOR,0,0) != -1) 
   {
      printpan(ipfi,npan);
      printprel(ipfi,npan);      
      trata_lineas(ipfi,DIFERIDO,m,0,0);
   }
   return(0);
}

limpia_ficha(ipfi)
int ipfi;
{
   defecto_ficha(v_mpfi(ipfi));
   /*prin_pan(ipfi,-1);*/
   pinta_datos(ipfi,2);
   f_n(ipfi,RENBUF);
}

int campo_linea(f)
int f;
{
   return(v_mpfi(f)->f_fi_clave_lineas / 100);
}

int da_cla_lin(f)
int f;
{
   return(v_mpfi(f)->f_fi_clave_lineas % 100);
}

int da_cam_cla(f)
int f;
{
   int c = campo_linea(f);
   int k = da_cla_lin(f);
   int i;
   for (i = v_mpfi(f)->f_clave[k].k_ncampos - 1;i > -1;i--)
      if ((v_mpfi(f)->f_clave[k].k_campo[i] - 1) == c)
         return(i);
      return(0);
}

int minimo_campo(_pfi,campo) /* pone el valor minimo de un campo */
FI_DEF *_pfi;
int campo;
{
   int i;
   char tmp[256];
   
   switch(_pfi->f_campos[campo].c_tipo) {
   case CHARTIPO:
      i = _pfi->f_campos[campo].c_qc;
      memset(tmp,' ',i);
      tmp[i]=0;
      break;
   case FECHATIPO:
      strcpy(tmp,"01.01.0001");
      break;
   case INTTIPO:
      sprintf(tmp,_pfi->f_campos[campo].c_usi,atoi(_pfi->f_campos[campo].c_minimo));
      break;
   case DOUBLETIPO:
      sprintf(tmp,recalcula_usi(_pfi,campo),rwatof(_pfi->f_campos[campo].c_minimo));
      break;
   case NULOTIPO:
	   tmp[0] = 0;
	  break;
   }
   st_campo(campo,_pfi,tmp);
}

int maximo_campo(_pfi,campo) /* pone el valor maximo de un campo */
FI_DEF *_pfi;
int campo;
{
   int i;
   char tmp[256];
   
   switch(_pfi->f_campos[campo].c_tipo) {
   case CHARTIPO:
      i = _pfi->f_campos[campo].c_qc;
      memset(tmp,255,i);
      tmp[i]=0;
      break;
   case FECHATIPO:
      strcpy(tmp,"31.12.9999");
      break;
   case INTTIPO:
      sprintf(tmp,_pfi->f_campos[campo].c_usi,atoi(_pfi->f_campos[campo].c_maximo));
      break;
   case DOUBLETIPO:
      sprintf(tmp,recalcula_usi(_pfi,campo),rwatof(_pfi->f_campos[campo].c_maximo));
      break;
   case NULOTIPO:
      tmp[0] = 0;
	  break;
   }
   st_campo(campo,_pfi,tmp);
}

#ifdef COMENTARIO
int suma_campo(_pfi,campo,valor)
FI_DEF *_pfi;
int campo;
int valor;
{
   int i = _pfi->f_campos[campo].c_tipo;
   char tmp[256];
   if (i == INTTIPO || i == DOUBLETIPO || i == LONGTIPO || i == NULOTIPO)
   {
      printf("Suma campo numerico %d \n\r",campo);
      st_doble(campo,_pfi,ld_doble(campo,_pfi)+valor);
   }
   else
   {
      ld_campo(campo,_pfi,tmp);
      tmp[_pfi->f_campos[campo].c_qc-1] += valor; /* esto sera suficiente */
      st_campo(campo,_pfi,tmp);
   }
   return(0);
}
#endif

/*
* compara dos claves (que pueden estar formadas por mas de un campo y
* meaclando orden ascendente y descendente)
* devuelve :   0 - iguales
*             >0 - clave 1 > clave 2
*             <0 - clave 1 < clave 2
*/

int clave_lin_trata(f,buf1,accion,bf)
int f;
char *buf1;
int accion; /* 0 da la longitud de la clave */
/* 1 compara la clave */
/* 2 copia la clave */
/* 3 compara la clave en buffer de clave     compara_linea*/
/* 4 copia la clave de buffer de mpfi a buffer de clave que_linea */
/* 5 copia la clave en buffer de mpfi  pon_linea */
/* 6 copia la clave en buffer de mpfi a partir de cierto campo */
/* 7 compara la clave segun lectura descendente */
/* 8 compara la clave segun lectura ascendente */
/* 9 igual que 6 pero el primer campo lo deja restandole 1 */
#ifdef COMENTARIO          
union bbb { /* asegurarse de que esto funciona !!! */
   char *b1;
   int b2;
} bf;
#endif
char *bf;   
{
#ifdef COMENTARIO          
   char *buf3 = bf.b1;
   int  pcla  = bf.b2;
#endif
   char *buf3 = bf;
   int pcla = *((int *)(&bf));
   char *buf2;
   int camfin,numcam,campo,nclave,orden,n;
   int compara = 0;
   char **tmp;
   double d;
   int smodo = 0;
#ifdef ESMOTOROLA
   unsigned long t_moto1;
#endif
   
   
#ifdef COMENTARIO
   if (accion == 9)
   {
      accion = 6;
      smodo = 1;
   }
#endif      
   
   if (accion > 6) 
   {
      smodo = accion - 6;
      accion = 3;
   }
   
   buf2 = ((accion != 3) ? v_mpfi(f)->f_buf : buf3);
   
   if (accion == 4) 
   {
      tmp = (char **)buf1;
      buf1 = *tmp;
   }
   
   if (!buf1) {
      if (accion == 3)
         return(-1);
      else
         if (accion == 4) 
		 {
            orden = clave_lin_trata(f,buf1,0,"");
            *tmp = malloc(orden+1);
            buf1 = *tmp;
         }
   }
   
   if (!buf2) {
      if (accion == 3)
         return(1);
   }
   
   nclave = da_cla_lin(f);
   
   if (nclave < 0 || nclave >= v_mpfi(f)->f_nclaves) 
   {
      cursor(2501);
      v_printd("f %d. accion ",f);
      v_printd("%d. nclave ",accion);
      v_printd("%d.",nclave);
      rw_error(46);
      return(0);
   }
   
   if (!accion)
      compara = sizeof(long);
   
   camfin = v_mpfi(f)->f_clave[nclave].k_ncampos;
   /* campos de la clave (cuantos) */
   
   for (numcam=0;numcam<camfin;numcam++) 
   {
      campo = v_mpfi(f)->f_clave[nclave].k_campo[numcam] - 1;
      orden = v_mpfi(f)->f_clave[nclave].k_orden[numcam];
      if (!accion) {
         compara += (long_campo(v_mpfi(f),campo) + 1);
      }
      else
         if (accion == 1) {
            compara = _campocmp(campo,orden,f,buf1,buf2,0);/* comparar el campo */
            if (compara != 0) return(compara);  /* si son diferentes no continuar */
         }
         else
            if (accion == 2) {
               _copias_cam(f,campo,f,campo,buf1,buf2);/* copiar */
            }
            else
               if (accion == 3) {
                  n = long_campo(v_mpfi(f),campo);
                  switch(v_mpfi(f)->f_campos[campo].c_tipo) {
                  case CHARTIPO:
                     compara = memcmp(buf1,buf2,n);
                     break;
                  case FECHATIPO:
                     compara = cmfecha(buf1,buf2);
                     break;
                  case INTTIPO:
                     compara = atoi(buf1) - atoi(buf2);
                     break;
                  case DOUBLETIPO:
                  case FLOATTIPO:
                     d = rwatof(buf1) - rwatof(buf2);
                     compara = 0;
                     if (d < 0.0)
                        compara = -1;
                     if (d > 0.0)
                        compara = 1;
                     break;
                  }
                  
                  if (orden < 0) compara = -compara;
                  if (
                     (!smodo && compara != 0)
                     ||
                     (smodo == 1 && compara > 0)
                     ||
                     (smodo == 2 && compara < 0)
                     ) {
                     if (compara < 0) 
					 {
                        compara = -(numcam+1);
                     }
                     else 
					 {
                        compara = numcam+1;
                     }
                     return(compara);  /* si son diferentes no continuar */
                  }
                  compara=0;
                  n++;
                  buf1 += n;
                  buf2 += n;
               }
               else
                  if (accion == 4) 
				  {
                     ld_campo(campo,v_mpfi(f),buf1);
                     buf1 += (long_campo(v_mpfi(f),campo) + 1);
                  }
                  else
                     if (accion == 5) 
					 {
                        st_campo(campo,v_mpfi(f),buf1);
                        buf1 += (long_campo(v_mpfi(f),campo) + 1);
                     }
                     else
                        if (accion == 6) 
						{
#ifdef COMETARIO        
																											if (numcam == pcla)
                                                                                 {
                                                                                    if (smodo)
                                                                                       suma_campo(v_mpfi(f),campo,-1);      
                                                                                    else
                                                                                       st_campo(campo,v_mpfi(f),buf1);            
                                                                                 }
                                                                                 else
#endif   
                                                                                    if (numcam >= pcla)
                                                                                       st_campo(campo,v_mpfi(f),buf1);
                                                                                    buf1 += (long_campo(v_mpfi(f),campo) + 1);
                        }
                        else {
																											rw_error(45);
                        }
   }
   
   if (accion == 4) { /* registro a 0 */
#ifdef ESMOTOROLA
      t_moto1 = 0L;
      memcpy(buf1,&t_moto1,4);
#else
      *((long *)buf1) = 0L;
#endif      
   }
   
   return(compara);
}


int bucle_lineas(f,modo,c,faclin,lim1,lim2)
int f,modo,c,faclin;
char *lim1,*lim2;
{
   int r,b,h,rr = 0;
   long ll,ss;
   int key = da_cla_lin(f) + 1;
   char *buf2 = NULL;
   int _operaerror = operaerror;
   
   if (modo == CONSULTA)
      b = 0;
   else
      b = ISLOCK;
   
   h = da_cla_lin(f) + 1;
   if (h > 1) {
      cambia_ind(v_mpfi(f),h);
   }
   
   r = primeralinea(f,lim2,b,lim1,lim1);
   
   while (!apli_aborto && r == 0) {
      ll = da_registo_actual(v_mpfi(f));
      r = siguientelinea(f,lim2,0,0,lim1); /* cojo la siguiente para evitar problemas */
      ss = da_registo_actual(v_mpfi(f));
      pon_registro_actual(v_mpfi(f),ll);
      if (apunta_ind(v_mpfi(f),0,ISEQUAL,0) < 0) {
         libera_ind(v_mpfi(f));
         rr = -1;
         goto b_ls_salida;
      }
      apunta_ind(v_mpfi(f),key,ISCURR,0);
      if (leer_reg(f,ISCURR,0,1) < 0) {
         libera_ind(v_mpfi(f));
         rr = -1;
         goto b_ls_salida;
      }
      
      if (c > 0) 
      {
         operaerror = 0;
         p_ejecuta(c,O_NOR,faclin,2);
         if (operaerror != 0) {
            if (operaerror == 10)
               rr = -10;
            else
               rr = -1;
            operaerror = 0;
            libera_ind(v_mpfi(f));       
            goto b_ls_salida;
         }
      }
      else
         if (modo == ALTA) /* especial acumulador */
         {
            if (actualiza_rel(f,modo,0,faclin,&buf2))
            {
               libera_ind(v_mpfi(f));
               rr = -1;
               goto b_ls_salida;
            }
         }
         
         if (modo == BAJA) {
            if (baja_fichao(f) < 0) {
               libera_ind(v_mpfi(f));
               rr = -1;
               goto b_ls_salida;
            }
         }
         libera_ind(v_mpfi(f));
         
         if (r == 0) {
            pon_registro_actual(v_mpfi(f),ss);
            if (apunta_ind(v_mpfi(f),0,ISEQUAL,0) < 0) {
               libera_ind(v_mpfi(f));
               rr = -1;
               goto b_ls_salida;
            }
            apunta_ind(v_mpfi(f),key,ISCURR,0);
            if (leer_reg(f,ISCURR,b,1) < 0) {
               libera_ind(v_mpfi(f));
               rr = -1;
               goto b_ls_salida;
            }
         }
   }
b_ls_salida:
   if (h > 1) {
      cambia_ind(v_mpfi(f),1);
			}
   operaerror = _operaerror;
   return(0);
}

int copia_linea(f,l1,l2)
int f;
char **l1,*l2;
{
   int i = clave_lin_trata(f,NULL,0,""); /* longitud */
   if (!l2) {
      if (*l1) {
         free(*l1);
         *l1 = NULL;
      }
      return(0);
			}
   if (!*l1) {
      *l1 = malloc(i+2);
			}
   memcpy(*l1,l2,i);
   *((*l1) + i) = 0;
   return(0);
}

int ultimalin(linea,f,lim2)
char *linea;
int f;
char *lim2;
{
			int c;
         
         c = clave_lin_trata(f,lim2,3,linea);
         if (c < 0)
            return(-1);
         else
            return(0);
}


/* actualiza_cam */

int actualiza_cam_(ipfi,campo,faclin,m,ver)
int ipfi;
int campo;
int faclin;
int m;
int ver;
{
   int j,i,r,n,c,rr = -1;
   double vca,vcf;
   int _operaerror = operaerror;
   campo--;
   operaerror = 0;

   if (campo > -1 && (v_mpfi(ipfi)->f_orderflags[campo]&ORDER_FLAG_EVENTO))
   {
	   char bloque[128];
	   int operacion;
	   int id = v_mpfi(ipfi)->f_campowids[campo];	   
	   if (m == 7)
		   operacion = 17;
	   else if (m == 6)
		   operacion = 18;
	   else
		   operacion = 0;
	   if (operacion && id > -1)
	   {
		   for (i = 0;i < RetroPunto;i++)
		   {   
			   if (RetroGridId(i) == id)
			   {		
				   memcpy((void *)(bloque+sizeof(long)*3),(void *)&operacion,sizeof(int));					
				   ( *RetroFuncion[ i ] )(bloque,RetroPar[i],i);
			   }
		   }
	   }
	   return 0;
   }
   /*
   if (memoplin > 2) 
   {
      vca = v_memo(5000)->numerica;
      vcf = v_memo(5011)->numerica;
      v_memo(5000)->numerica = (double)campo;
      v_memo(5011)->numerica = (double)ipfi;
   }
   if (memoplin > 12)
      sprintf(v_memo(5012)->alfabetica,"%d",v_mpfirel(ipfi)->relact[j].fflag);
   p_ejecuta(i,O_NOR,faclin,3);
   if (memoplin > 2) 
   {
      v_memo(5000)->numerica = vca;
      v_memo(5011)->numerica = vcf;
   }
   if (operaerror != 0)
      break;		   
   */


   for (j=0;j<v_mpfirel(ipfi)->nrelact;j++) 
   {
      if (v_mpfirel(ipfi)->relact[j].campo != -1) 
	  {
         n = v_mpfirel(ipfi)->relact[j].ambito;
         c = v_mpfirel(ipfi)->relact[j].campo;n += c;
         if ( campo < c || campo > n ) continue;
	  }
      i = v_mpfirel(ipfi)->relact[j].tabla;
      if (i > 0) {
         r = tipo_tabla[i];
         if (r == m) {
            if (ver)
            {
               rr = 0;
               break;
            }
            else
            {
               if (memoplin > 2) 
			   {
                  vca = v_memo(5000)->numerica;
                  vcf = v_memo(5011)->numerica;
                  v_memo(5000)->numerica = (double)campo;
                  v_memo(5011)->numerica = (double)ipfi;
               }
               if (memoplin > 12)
                  sprintf(v_memo(5012)->alfabetica,"%d",v_mpfirel(ipfi)->relact[j].fflag);
               p_ejecuta(i,O_NOR,faclin,4);
               if (memoplin > 2) 
			   {
                  v_memo(5000)->numerica = vca;
                  v_memo(5011)->numerica = vcf;
               }
               if (operaerror != 0)
                  break;		   
            }
         }
	  }
   }
   if (!ver)
   {
      r = operaerror;
   }
   else
   {
      r = rr;
   }
   operaerror = _operaerror;
   if (r != 0)
   {
      if (r == 2)
         return(-2);
      else
         return(-1);
   }
   else
      return(0);
}


int actualiza_cam(ipfi,campo,faclin,m)
int ipfi;
int campo;
int faclin;
int m;
{
   return(actualiza_cam_(ipfi,campo,faclin,m,0));
}

int ejecuta_otro(_ipfi,cual,modo,faclin,ver)
int _ipfi,cual,modo,faclin,ver;
{
   int j,i,r,ipfi;
   int _operaerror = operaerror;
   int desde,hasta;
   double vca;
   double vcf;     

   
   if (_ipfi == -1)
   {
       desde = 0;
       hasta = nfi-1;
   }
   else
   {
       desde = _ipfi;
       hasta = _ipfi;
   }

   for (ipfi = desde;ipfi <= hasta && ipfi < nfi;ipfi++)
   {
       if (!mpfirel[ipfi] || !tipo_tabla) continue;

       for (j=0;j<v_mpfirel(ipfi)->nrelact;j++) 
	   {
          i = v_mpfirel(ipfi)->relact[j].tabla;
          if (i > 0) 
		  {
             r = tipo_tabla[i];
             if (r == cual) 
			 {
                if (!ver) 
				{
                   operaerror = 0;
                   if (memoplin > 12)
                      sprintf(v_memo(5012)->alfabetica,"%d",v_mpfirel(ipfi)->relact[j].fflag);
				   vca = v_memo(5000)->numerica;
				   vcf = v_memo(5011)->numerica;  
				   v_memo(5011)->numerica = (double)ipfi;
				   /*
		   {
			   FILE *f;
			   char tmpc[512];
			   sprintf(tmpc,"%saviso64_X.txt",rw_directorio("logs/"));
			   f = fopen(tmpc,"a");
			   if (f)
			   {
				   fprintf(f,"Traza Llamada: %s %s %s %s %s (_ipfi,cual,modo,faclin,ver:%d-%d-%d-%d-%d)! recur:%d ipfi:%d j:%d r:%d\r\n",fecha(),fechas+11,rw_user,nomapli,modulo,_ipfi,cual,modo,faclin,ver,recur_punto,ipfi,j,r);
				   fclose(f);
			   }
		   }
		   */
                   p_ejecuta(i,modo,faclin,5);
				   v_memo(5000)->numerica = vca;
				   v_memo(5011)->numerica = vcf;
                   if (operaerror != 0) 
                   {
                      operaerror = _operaerror;
                      return(-1);
                   }
                }
                operaerror = _operaerror;
                if (desde == hasta)
                {
                    return(i);
                }
            }
          }
       }
   }
   operaerror = _operaerror;
   return(-2);
}

int llama_a_proceso_inicializacion(int ipfi)
{
    double vca = v_memo(5000)->numerica;
    double vcf = v_memo(5011)->numerica;  
	char tmp[MAXDEFECTO+1];
    
	memcpy(tmp,v_memo(5004)->alfabetica,MAXDEFECTO);
    v_memo(5011)->numerica = (double)ipfi;
    strcpy(v_memo(5004)->alfabetica,v_mpfi(ipfi)->f_PathDef);
    ejecuta_otro(ipfi,88,O_NOR,0,0);

    v_memo(5011)->numerica = (double)ipfi;
    strcpy(v_memo(5004)->alfabetica,v_mpfi(ipfi)->f_PathDef);    
    ejecuta_otro(-1,81,O_NOR,0,0);

    v_memo(5000)->numerica = vca;
    v_memo(5011)->numerica = vcf;
	memcpy(v_memo(5004)->alfabetica,tmp,MAXDEFECTO);

    return 0;
}

/* da registro de la linea */

long que_registro(f,linea)
int f;
char *linea;
{
   int l = clave_lin_trata(f,NULL,0,"") - sizeof(long);
#ifdef ESMOTOROLA
   long t_moto1;
#endif
   if (!linea) {
      rw_error(50);
   }
   linea += l;
#ifdef ESMOTOROLA
   memcpy(&t_moto1,linea,4);
   return(t_moto1);
#else
   return(*((long *)linea));
#endif
}

long pon_registro(f,linea,m)
int f;
char *linea;
int m;
{
   int l = clave_lin_trata(f,NULL,0,"") - sizeof(long);
#ifdef ESMOTOROLA
   long t_moto1;
#endif
   
   if (!linea) {
      rw_error(51);
      return(0L);
   }
   
   linea += l;
#ifdef ESMOTOROLA
   if (m)
      t_moto1 = 0L;
   else
      t_moto1 = da_registo_actual(v_mpfi(f));
   memcpy(linea,&t_moto1,4);
   return(t_moto1);
#else
   if (m)
      *((long *)linea) = 0L;
   else
      *((long *)linea) = da_registo_actual(v_mpfi(f));
   return(*((long *)linea));
#endif
}

void set_registro(f,linea,regi)
int f;
char *linea;
long regi;
{
   int l = clave_lin_trata(f,NULL,0,"") - sizeof(long);
   if (!linea) {
      rw_error(51);
      return;
   }
   linea += l;
#ifdef ESMOTOROLA
   memcpy(linea,&regi,4);
#else
   *((long *)linea) = regi;
#endif
   return;
}


/* establece la busqueda por clave */
int salta_clave(f,lim1,lim2,modo,orden)
int f;
char *lim1;
char *lim2;
int modo,orden;
{
   int r = 0,c,prafa;
   int m1,
      m2,
      m3,
      m4;
   char *l1,
      *l2;
   char *tmp = NULL;
   char buff[258]; /* debug */
   
   if (!orden) { /* descender por la clave */
      m1 = ISGTEQ;
      m2 = ISGREAT;
      m3 = 7;
      m4 = 8;
      l1 = lim1;
      l2 = lim2;
   }
   else {        /* ascender por la clave */
      /*   m1 = ISLOWEQ; */
      /*   m2 = ISLOW; */
      m1 = ISGTEQ;
      m2 = ISGREAT;
      m3 = 8;
      m4 = 7;
      l1 = lim2;
      l2 = lim1;
   }
   
   while(!r) 
   { /* en sentido descendente ! */
      clave_lin_trata(f,&tmp,4,"");
      if ((c = clave_lin_trata(f,tmp,m3,l2))) 
      {
         if (orden)
         {
            c = -c;
         }
         if (c < 2) {
            r = -1;
            libera_ind(v_mpfi(f));
         }
         else 
         {
            libera_ind(v_mpfi(f));
            c--; /* aqui c es positiva */
            prafa = apunta_ind(v_mpfi(f),da_cla_lin(f)+1,(!orden) ? m2 : m1,c);		 
            if (prafa)
            {
               r = -1;
            }
            if (orden) { /* en caso descendente retroceder */
               if (r == -1) { /* si no hay una mayor coger la ultima */
                  r = 0;
                  if (apunta_ind(v_mpfi(f),da_cla_lin(f)+1,ISLAST,c)) 
				  {
                     apunta_ind(v_mpfi(f),da_cla_lin(f)+1,ISCURR,0);
                     r = -1;
                  }
															}
               else {
                  /* si hay una mayor o igual coger la anterior */
                  if (apunta_ind(v_mpfi(f),da_cla_lin(f)+1,ISPREV,c)) {
                     apunta_ind(v_mpfi(f),da_cla_lin(f)+1,ISCURR,0);
                     r = -1;
                  }						 
															}
            }
            if (r != -1) {
               if (apunta_ind(v_mpfi(f),da_cla_lin(f)+1,ISCURR,0)) {
                  libera_ind(v_mpfi(f));
                  r = -1;
															}
               else {
                  if (!(r = leer_reg(f,ISCURR,modo,1))) {
                     clave_lin_trata(f,&tmp,4,""); /* debug */
                     clave_lin_trata(f,l1,6,(char *)c);
                     r = leer_reg(f,m1,modo,1); /* definitvamente siguiente clave */						
                     if (orden) { /* caso descendente */
                        libera_ind(v_mpfi(f));
                        if (r)
                           break; /* tiene que existir */
                        else
                           r = leer_reg(f,ISPREV,modo,1);
                        clave_lin_trata(f,&tmp,4,""); /* debug */
                     }
                  }
															}
            }
         }
      }
      else { /* comprobacion del otro limite */
         if ((c = clave_lin_trata(f,tmp,m4,l1))) {
            libera_ind(v_mpfi(f));
            if (!orden)
               c = -c; /* viene en negativo !!! */
            c--;
            clave_lin_trata(f,l1,6,(char *)c);
            r = leer_reg(f,m1,modo,1); /* definitvamente siguiente clave */
            if (orden) { /* caso descendente */
               libera_ind(v_mpfi(f));
               if (r)               
                  r = leer_reg(f,ISLAST,modo,1); 
               else
                  r = leer_reg(f,ISPREV,modo,1);
            }
         }
         else
            break;
      }
   }
   
   apunta_ind(v_mpfi(f),da_cla_lin(f)+1,ISCURR,0);
   
   if (tmp)
      free(tmp);
   return(r);
}

int primeralinea(f,lim2,m,lim1,inicio)
int f;
char *lim2;
int m;
char *lim1;
char *inicio;
{
   int r,h;
   if (que_registro(f,inicio)) {
      h = da_cla_lin(f) + 1;
      apunta_ind(v_mpfi(f),0,ISCURR,0);
      pon_registro_actual(v_mpfi(f),que_registro(f,inicio));
      r = leer_reg(f,ISEQUAL,m,1);
      apunta_ind(v_mpfi(f),h,ISCURR,0);
   }
   else {
      clave_lin_trata(f,inicio,5,"");
      if (!(r = leer_reg(f,ISGTEQ,m,1)))   
         r = salta_clave(f,lim1,lim2,m,0);
   }
   
   return(r);
}

int anteriorlinea(f,lim2,final,m,lim1)
int f;
char *lim2;
int final;
int m;
char *lim1;
{
   int r;
   char *tmp = NULL;
   
   if (final == 1) {
      if (!(r=leer_reg(f,ISCURR,m,1)))
      {
         r = salta_clave(f,lim1,lim2,m,1); /* autentificar corriente */
      }
      return(r);
			}
   
   clave_lin_trata(f,&tmp,4,"");
   pon_registro(f,tmp,0);
   if (!(r = leer_reg(f,ISPREV,m,1))) 
   {
      r = salta_clave(f,lim1,lim2,m,1);
   }
   if (r)
      primeralinea(f,lim2,m,lim1,tmp); /* lee siguiente por registro */
   
   if (tmp)
      free(tmp);
   
   return(r);
}

int siguientelinea(f,lim2,final,m,lim1)
int f;
char *lim2;
int final;
int m;
char *lim1;
{
			int r;
         char *tmp = NULL;
         
         if (final == 1) return(-1);
         
         clave_lin_trata(f,&tmp,4,"");
         pon_registro(f,tmp,0);
         if (!(r = leer_reg(f,ISNEXT,m,1))) 
		 {
            r = salta_clave(f,lim1,lim2,m,0);
         }
         if (r)
            primeralinea(f,lim2,m,lim1,tmp); /* lee anterior por registro */
         
         if (tmp)
            free(tmp);
         
         return(r);
}


char *suma_linea(f,buf1,buf2)
int f;
char *buf1;
char *buf2;
{
   unsigned char tmp[256]; /* por si acaso v == buf2 */
   int campo = campo_linea(f);
   int i,c,n;
   double d,k;
   char *guarda = NULL;
   
   clave_lin_trata(f,(char *)&guarda,4,"");
   clave_lin_trata(f,buf1,5,"");
   ld_campo(campo,v_mpfi(f),tmp);
   
   switch(v_mpfi(f)->f_campos[campo].c_tipo) {
   case CHARTIPO:
						c = 0;
                  for (i = strlen(tmp);i > -1;i--) {
                     if (!c && tmp[i]) {
                        if ((tmp[i] < 122 && tmp[i] > 96) || (tmp[i] < 90 && tmp[i] > 64)) {
                           tmp[i]++;
                           c = 1;
                           break;
                        }
                     }
                  }
                  break;
   case FECHATIPO:
						diasfecha(tmp,1,0,0);
                  break;
   case INTTIPO:
						c = atoi(tmp);      
                  if (v_mpfi(f)->f_campos[campo].c_mante.c_defecto)
                  {
                     n = atoi(v_mpfi(f)->f_campos[campo].c_mante.c_defecto);		 
                     if (n == 0) 
                        n = 1;
                     else
                        if (v_mpfi(f)->f_campos[campo].c_maximo && n == atoi(v_mpfi(f)->f_campos[campo].c_maximo))
                           n = 0;
                  }
                  else
                     n = 1;
                  c += n;
                  sprintf(tmp,v_mpfi(f)->f_campos[campo].c_usi,c);
                  break;
   case DOUBLETIPO:
						d = rwatof(tmp);	  
                  if (v_mpfi(f)->f_campos[campo].c_mante.c_defecto)
                  {
                     k = rwatof(v_mpfi(f)->f_campos[campo].c_mante.c_defecto);		 
                     if (k == 0.0) 
                        k = 1.0;
                     else
                        if (v_mpfi(f)->f_campos[campo].c_maximo && k == rwatof(v_mpfi(f)->f_campos[campo].c_maximo))
                           k = 0.0;
                  }
                  else
                     k = 1.0;
                  d += k;
                  sprintf(tmp,recalcula_usi(v_mpfi(f),campo),d);
                  break;
   }
   
   st_campo(campo,v_mpfi(f),tmp);
   clave_lin_trata(f,(char *)&buf2,4,"");
   
   clave_lin_trata(f,guarda,5,"");
   if (guarda)
      free(guarda);
   
   return(buf2);
}


int defecam_rel(ipfi,i,npan,faclin)
int ipfi,i,npan,faclin;
{
   register int k;
   int ipfr,r,enclave,d,leida;
   
   if ((leida = f_v(ipfi,RLEIDA)) == -1) f_s(ipfi,RLEIDA);
   /* se asume que el principal esta leido y por si el flag no se ha puesto lo
   pongo para evitar recursiones sin fin */
   
   for (k=0;k<v_mpfirel(ipfi)->nreldef;k++) {
			if ( (i == -1 || v_mpfirel(ipfi)->reldef[k].campo  == i) &&
                     v_mpfirel(ipfi)->reldef[k].numrela == -1 ) {
                     d = v_mpfirel(ipfi)->reldef[k].campo;
                     if (mira_sidefecto(ipfi,d) == -1) 
                     {
                        if (i == -1) continue; else return(-1);
                     }
                     ipfr = v_mpfirel(ipfi)->reldef[k].nfirel;
                     if (memoplin > 2) ld_campo(d,v_mpfi(ipfi),v_memo(5001)->alfabetica);
                     r = _copia_cam(ipfi,d,ipfr,
                        v_mpfirel(ipfi)->reldef[k].nficampo);
                     if (i != -1) return(r);
                     if (r >= 0) {
                        enclave = pintar_otrocampo(ipfi,d,npan,faclin);
                        r = (r == 0) ? 1 : 0;
                        tras_entra(npan,d+1,DIFERIDO,
                           enclave,r,ipfi,faclin);
                        r = (r == 0) ? 1 : 0;
                     }
                  }
   }
   if (leida == -1) f_n(ipfi,RLEIDA);
   return(0);
}

/*
*  deferel_rel
*    Busca los campos de 'ipfi' con defecto los campos del fichero 'pfr' :
*       segun 'mpfi->f_campos[i].c_mante.c_relacion == DXXXffffffff',
*    y los copia :  del buffer de 'pfr' al buffer de 'ipfi'.
*    Si toca campos de la pantalla 'npan' los pinta.
*/
int deferel_rel(ipfi,ipfr,npan,faclin,alta,nrel)
int ipfi;
int ipfr;
int npan,faclin,alta,nrel;
{
   register int k,i,enclave,mo;
   
   for (k=0;k<v_mpfirel(ipfi)->nreldef;k++) 
   {
      if ( v_mpfirel(ipfi)->reldef[k].nfirel == ipfr &&
         (nrel == -1 || nrel == v_mpfirel(ipfi)->reldef[k].numrela)) 
      {
         i = v_mpfirel(ipfi)->reldef[k].campo;
         if ((mo = _copia_cam(ipfi,i,ipfr,v_mpfirel(ipfi)->reldef[k].nficampo)) >= 0)
									{
            enclave = pintar_otrocampo(ipfi,i,npan,faclin);
            mo = (mo == 0) ? 1 : 0;
            tras_entra(npan,i+1,DIFERIDO,enclave,mo,ipfi,faclin);
									}
      }
   }
   fflush(stdout);
   return(0);
}

int mira_sirel(ipfi,campo)
int ipfi;
int campo;
{
   register int k;
   for (k=0;k<v_mpfirel(ipfi)->nreldef;k++) {
      if (v_mpfirel(ipfi)->reldef[k].campo  == campo) {
									return(-1);
      }
   }
   return(0);
}

int mira_sivacio(_pfi,campo)
FI_DEF *_pfi;
int campo;
{
   char buf[MAXDEFECTO];
   int i;
   
   ld_campo(campo,_pfi,buf);
   for (i = strlen(buf) - 1 ;i > -1;i--) {
      if (*(buf + i) != ' ') return(-1);
   }
   return(0);
}

int mira_sidefecto(ipfi,campo)
int ipfi,campo;
{
   char buf[MAXDEFECTO];
   char *tmp;
   int i = v_mpfi(ipfi)->f_campos[campo].c_tipo;
   
   tmp = v_mpfi(ipfi)->f_campos[campo].c_mante.c_defecto;
   
   /* esta rutina no es perfecta */
   
   ld_campo(campo,v_mpfi(ipfi),buf);
   quitab(buf);
   
   if (i == CHARTIPO || i == FECHATIPO) {
      if (strcmp(buf,tmp) == 0) {
         return(0);
      }
   }
   else
      if (rwatof(buf) == rwatof(tmp)) {
         return(0);
      }
      
      return(-1);
}



/************************* M A I N *******************************/

extern int entra_int();
extern int entra_util();
extern int printa_linea_();

extern int (*_pon_incidencia)();

extern int guarda_incidencia(int,char *);

static char *tttk = "1AQUI PONER 1";

extern int saca_virtual();


#ifdef ESDOS
#ifndef FGVWIN
#ifdef FGVIXX
extern int (*agi_especial_system_antes)();
extern int (*agi_especial_system_despues)();
extern int ixx_anula_un_dat();
extern int ixx_repon_un_dat();
#endif
#endif
#endif

#ifdef FGVIXX
extern int (*ixx_incidencia)(int,char *);
extern char *(*ixxquita_path)(char *);
#endif

extern int usar_grid;

#ifdef WATCOMNLM
int multitarea()
{
   ThreadSwitch();
   return(0);
}
#endif


static int saca_opcion(char *linea,char *opcion)
{
	char *p1,*p = linea;
	int n;
	while(*p)
	{
		if (*p == '{' && *(p+1) == '{')
		{
			p += 2;
			p1 = p;
			while(*p)
			{
				if (*p == '}' && *(p+1) == '}')
				{
					break;
				}
				p++;
			}
			n = (int)(p-p1);
			memcpy(opcion,p1,n);
			opcion[n] = 0;
			quitab(opcion);
			return n;
		}
		p++;
	}
	return -1;
}

static int saca_nombre(char *linea,char *nombre)
{
	char *p;
	int n;

	while(*linea)
	{
		if (*linea == ':')
		{
			linea++;
			break;
		}
		linea++;
	}

	p = linea;
	while(*p)
	{
		if (*p == '{' && *(p+1) == '{')
		{
			break;
		}
		p++;
	}

	strcpy(nombre,nommenu);
	strcat(nombre," ");
	nombre += strlen(nombre);
	n = (int)(p-linea);
	memcpy(nombre,linea,n);
	nombre[n] = 0;
	quitab(nombre);
	strcat(nombre," ");
	strcat(nombre,empresa);
	return n;
}

extern char *agi_aplicacion;
extern char *agi_empresa;
extern char *agi_codi_empre;

static void pon_en_la_lista(char *linea,char ***lista,int *nlista)
{
	char tmp[512];
	char tmp2[300];
	tmp[0] = 0;
	saca_nombre(linea,tmp);
	saca_opcion(linea,tmp2);

	strcat(tmp,"\001|(");
    if (agi_aplicacion)
    {      
       strcat(tmp,agi_aplicacion);       
    }
    if (agi_empresa)
    {
          strcat(tmp," ");
          strcat(tmp,agi_empresa);
          if (agi_codi_empre)
          {
             strcat(tmp," ");
             strcat(tmp,agi_codi_empre);
          }
    }
	quitab(tmp);
	strcat(tmp,")");
	strcat(tmp,tmp2);
	/*
    if ((tmp2[0] < '0' || tmp2[0] > '9' || tmp2[1] != ' ') && tmp2[0] != ':')
    {
       if (agi_aplicacion)
       {
          strcat(tmp,":");
          strcat(tmp,agi_aplicacion);
          strcat(tmp,"/");
       }
       strcat(tmp,tmp2);
       if (agi_empresa)
       {
          strcat(tmp," ");
          strcat(tmp,agi_empresa);
          if (agi_codi_empre)
          {
             strcat(tmp," ");
             strcat(tmp,agi_codi_empre);
          }
       }
    }
    else
       strcat(tmp,tmp2);
	*/
	
	if (!*nlista || !*lista)
	{
		*lista = malloc(sizeof(char *)*100);
	}
	else
	{
		*lista = realloc(*lista,*nlista * sizeof(char *) + sizeof(char *));
	}
	(*lista)[*nlista] = malloc(strlen(tmp)+1);
	strcpy((*lista)[*nlista],tmp);
	(*nlista)++;
}


static int nivel_busca = 0;
static int buscalo_dentro(char *menu,char *elque,char ***lista,int *nlista)
{
	char tmp[300];
	char opcion[300];
	char nombre[300];
	int pp,pp1;
	FILE *f;
	int i;
	int vacio;

	if (nivel_busca > 50) return 0;
	nivel_busca++;	

	sprintf(tmp,"%s",menu);
	if (strbusca(tmp,".men") < 0)
	{
		strcat(tmp,".men");
	}
	f = fopen(tmp,"r");
	if (f)
	{
		while(lee_linea(tmp,295,f))
		{
			vacio = 1;
			for (i = 0;tmp[i];i++)
			{
				if (tmp[i] == ';')
					break;
				if (tmp[i] != ' ' && tmp[i] != 9)
				{
					vacio = 0;
					break;
				}

			}
			if (vacio)
				continue;

			pp1 = strbusca(tmp,"{{");
			if (pp1 > -1)
			{
				opcion[0] = 0;
				strcpy(opcion,menu);
				*(quita_path(opcion)) = 0;
				saca_opcion(tmp,nombre);
				if (nombre[0])
				{					
					if (strbusca(nombre,".men") > -1)
					{
						i = 0;
						if (nombre[i] == '[')
						{
							while (nombre[i] && nombre[i] != ']') i++;
							if (nombre[i]) i++;
						}
						strcat(opcion,nombre+i);
						buscalo_dentro(opcion,elque,lista,nlista);
					}
					else
					{
						strcpy(nombre,tmp);
						strminus(nombre);
						if (strbusca(nombre,elque) > -1)
						{
							pon_en_la_lista(tmp,lista,nlista);
						}
					}
				}
			}
		}
		fclose(f);
	}

	nivel_busca--;

	return 0;
}


void RetroGenerico(char *bloque,char *par,int el_punto)
{   /* esto se ejecuta en el servidor!!!  */
	int operacion;		
	int memoria,i;
	int *pb;
	char *orig_bloque = bloque;
	int tam,respuesta;
	char tmp[512];		
	char tmp2[512];
	char *pp;
	char *p;
    int r = -1;	
	int n = 0;
	int total = 0;
	FI_DEF *pfi = NULL;
	long pos = 1L;

	static char **lista = NULL;
	static int nlista = 0;
	static int ndirectos = 0;
	static FI_DEF *directos[100];

	memcpy((char *)&tam,bloque,sizeof(long));
	bloque += sizeof(long);
	memcpy((char *)&respuesta,bloque,sizeof(long));
    bloque += (sizeof(long)*2);

	memcpy((char *)&operacion,bloque,sizeof(int));
	pb = (int *)bloque;

	bloque += sizeof(int);	
	if (!operacion) /* buscar en menu ... cargar memoria */
	/* operacion 0 = viene texto y se carga la memoria (estatica) */
	{
		memoria = 0;
		if (fichemenu[0])
		{
			strminus(bloque);
			buscalo_dentro(fichemenu,bloque,&lista,&nlista);
			for (i = 0;i < nlista;i++)
			{
				memoria += (strlen(lista[i])+1);
			}
		}
		memoria++;
	    memcpy(bloque,(char *)&memoria,sizeof(int));
	}
	else if (operacion == 1) /* buscar en menu ... rellenar y liberar */
	/* operacion 1 = se envia la memoria y se libera */
	{
		for (i = 0;i < nlista;i++)
		{
			strcpy(bloque,lista[i]);
			bloque += (strlen(lista[i])+1);			
			free(lista[i]);
		}
		*bloque = 0;
		if (lista)
		{
			free(lista);
			lista = NULL;			
		}
		nlista = 0;
	}	
	else if (operacion == 2) /* determinar ayuda de campo */
	{
		int x,y,maxi;
		memcpy(&x,bloque,sizeof(int));
		bloque += sizeof(int);
		memcpy(&y,bloque,sizeof(int));
		bloque += sizeof(int);
		memcpy(&maxi,bloque,sizeof(int));
		bloque += sizeof(int);		
		if (u_p_ipfi >= 0 && u_p_ipfi < nfi && v_mpfi(u_p_ipfi)->f_npanta > u_p_npan)
		{
			int i,px,py;
			for (i = 0;i < v_mpfi(u_p_ipfi)->f_ncampos;i++)
			{
				px = v_mpfi(u_p_ipfi)->f_campos[i].c_mante.c_qp;
				py = px / 100;
				if (py == y)
				{
					px %= 100;
					if (x >= px && x < (px+long_campo(v_mpfi(u_p_ipfi),i)))
					{	
						sprintf(bloque,"%-15.15s%s.%03d",nomapli,quita_path(v_mpfi(u_p_ipfi)->f_fichero),i);
						break;
					}
				}
			}			
		}		
	}
	else if (operacion == 3)
	{
		/* Obtener info generica del basico */		
		*pb = 0;
		DespachaVInterface(orig_bloque,par,el_punto);
	}
	else if (operacion == 4)
	{
		/* lee_dir */
		char **dir;
		char *pr;
		int *p;
		int n;
		int mem;

		dir = lee_direct(bloque,3);
		p = (int *)(bloque);
		mem = 0;
		n = 0;
		if (dir)
		{
			for (n = 0;dir[n];n++)
				mem += (strlen(dir[n])+1);
			if (mem >= (respuesta-50))
			{
				n = 0;
			}
			else
			{
				pr = (char *)(p+2);
				for (n = 0;dir[n];n++)
				{				
					strcpy(pr,dir[n]);
					pr += (strlen(pr) + 1);				
				}
			}
			libera_dir(dir);
		}
		p[0] = n;
		p[1] = mem;
	}
	else if (operacion == 5)
	{
		/* consulta autocompletar */

	}
	else if (operacion == 11) /* abrir directo */
	{		
		pos = 1L;
		strminus(bloque);
		i = strbusca(bloque,":");
		if (i >= 0) {
			memcpy(tmp,bloque,i);
			bloque[i] = '/';
			tmp[i] = 0;
			strcat(tmp,"/def/");
			strcat(tmp,quita_path(bloque));
			i = strbusca(tmp,":");
			if (i >= 0)
				tmp[i] = 0;
			for (i = 0;i < ndirectos;i++) {
				if (!directos[i])
					break;
			}
			if (i < 100) {
				if (lee_defn(tmp,&pfi) >= 0) {					
					int j;					
					j = strbusca(bloque,":");
					if (j >= 0)
						bloque[j] = 0;
					strcpy(pfi->f_fichero,bloque);
					if (j >= 0) {
						strcpy(quita_path(pfi->f_fichero),bloque+j+1);
					}

					total = 0;
					if (abre_ind(pfi,ISINOUT + ISMANULOCK) >= 0) {
						total = tam_btrv(pfi);
						cierra_ind(pfi);
					}

					pfi->f_fd = abre_directo(pfi,pfi->f_fichero);
					if (pfi->f_fd < 0) {
						free_defpri(pfi);
					}
					else {																		
						int c;

						p = bloque + sizeof(int)*4;

						directos[i] = pfi;
						r = i;
						respuesta -= 270;

						for (i = 0;i < pfi->f_ncampos;i++) {
							tmp[0] = 0;
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
								sprintf(p,"%c%s",8,tmp2);
							else
								sprintf(p,"%s",tmp2);							
							respuesta -= strlen(p);
							p += strlen(p);
							if (respuesta < 0)
								break;
						}
						strcpy(p,"\n");
						respuesta -= strlen(p);
						p += strlen(p);
						if (respuesta < 0) {
							free_defpri(pfi);
							directos[r] = NULL;
							r = -1;
						}
						else { 
							while(respuesta > 0 && (c = lee_directo(pfi->f_fd,pos,pfi,pfi->f_buf)) >= 0) {
								if (c) {
									pos++;
									continue;
								}
								for (i = 0;i < pfi->f_ncampos;i++) {								
															if (pfi->f_campos[i].c_tipo == FECHATIPO) {
																   int idx = pfi->f_campos[i].c_offset;
																   memcpy(tmp2,pfi->f_buf+idx,4);
										   tmp2[4] = '-';
																   memcpy(tmp2+5,pfi->f_buf+idx+4,2);
										   tmp2[7] = '-';
																   memcpy(tmp2+8,pfi->f_buf+idx+6,2);
										   tmp2[10] = 0;
															}
															else {
										ld_campo(i,pfi,tmp2);									
															tmp2[257] = 0;
										for (pp = (unsigned char *)tmp2;*pp;pp++)
										   oem_a_ansi(pp);
										for (pp = (unsigned char *)tmp2;*pp;pp++) {
										   if (*pp < 14)
											*pp = ' ';
										}
									}
									if (i)
										sprintf(p,"%c%s",8,tmp2);
									else
										sprintf(p,"%s",tmp2);
									respuesta -= strlen(p);
									p += strlen(p);
									if (respuesta < 0)
										break;								
								}
								strcpy(p,"\n");
								respuesta -= strlen(p);
								p += strlen(p);								
								if (respuesta < 0)
									break;
								pos++;
								n++;
							}							
							if (r >= ndirectos)
								ndirectos = r + 1;
						}
					}
				}
			}
		}
		memcpy(bloque,&r,sizeof(int));
		memcpy(bloque+sizeof(int),&n,sizeof(int));
		memcpy(bloque+sizeof(int)*2,&pos,sizeof(int));
		memcpy(bloque+sizeof(int)*3,&total,sizeof(int));
	}
	else if (operacion == 12) /* leer mas del directo */
	{		
		int c;
		p = bloque + sizeof(int)*4;

		memcpy(&i,bloque,sizeof(int));
		memcpy(&pos,bloque+sizeof(int),sizeof(int));

		if (i >= 0 && i < ndirectos && directos[i]) {
			pfi = directos[i];

			respuesta -= 270;

			while(respuesta > 0 && (c = lee_directo(pfi->f_fd,pos,pfi,pfi->f_buf)) >= 0) {
				if (c) {
					pos++;
					continue;
				}
				for (i = 0;i < pfi->f_ncampos;i++) {								
											if (pfi->f_campos[i].c_tipo == FECHATIPO) {
												   int idx = pfi->f_campos[i].c_offset;
												   memcpy(tmp2,pfi->f_buf+idx,4);
						   tmp2[4] = '-';
												   memcpy(tmp2+5,pfi->f_buf+idx+4,2);
						   tmp2[7] = '-';
												   memcpy(tmp2+8,pfi->f_buf+idx+6,2);
						   tmp2[10] = 0;
											}
											else {
						ld_campo(i,pfi,tmp2);									
											tmp2[257] = 0;
						for (pp = (unsigned char *)tmp2;*pp;pp++)
						   oem_a_ansi(pp);
						for (pp = (unsigned char *)tmp2;*pp;pp++) {
						   if (*pp < 14)
							*pp = ' ';
						}
					}
					if (i)
						sprintf(p,"%c%s",8,tmp2);
					else
						sprintf(p,"%s",tmp2);
					respuesta -= strlen(p);
					p += strlen(p);
					if (respuesta < 0)
						break;								
				}
				strcpy(p,"\n");
				respuesta -= strlen(p);
				p += strlen(p);				
				if (respuesta < 0)
					break;
				pos++;
				n++;
			}
			r = 0;
		}		
		memcpy(bloque,&r,sizeof(int));
		memcpy(bloque+sizeof(int),&n,sizeof(int));		
		memcpy(bloque+sizeof(int)*2,&pos,sizeof(int));
		c = 0;
		memcpy(bloque+sizeof(int)*3,&c,sizeof(int));
	}
	else if (operacion == 13) /* cerrar el directo */
	{
		memcpy(&i,bloque,sizeof(int));
		if (i >= 0 && i < ndirectos && directos[i]) {
			cierra_directo(directos[i]->f_fd);
			free_defpri(directos[i]);
			directos[i] = NULL;
			while (ndirectos > 0 && i == ndirectos-1 && directos[i] == NULL) {
				i--;
				ndirectos--;
			}
		}
		//memcpy(bloque,&i,sizeof(int));
	}
}


#ifdef FGVDOS
LPGetTaskList     GetTaskList = NULL;

extern int (*Eliminador_Diagram)(char *);

int ListaTareas(void *puntero)
{
	DWORD numTasks = 0;
	TASK_LIST_ENUM te;

    numTasks = GetTaskList( tlista, MAX_TASKS );
	te.numtasks = numTasks;
	te.tlist = &tlista;
    GetWindowTitles( &te );

	*((TASK_LIST **)puntero) = &(tlista[0]);

	return (int)numTasks;
}

int MiKill(char *proceso)
{
   DWORD numTasks;
   char tmp[128];
   int i;
   int muertes = 0;
   
   strcpy(tmp,quita_path(proceso));
   strminus(tmp);
   if ((i = strbusca(tmp,".exe")) < 0) return 0;
   tmp[i] = 0;
   
   
   numTasks = GetTaskList( tlista, MAX_TASKS );
   
   for (i = 0; i < numTasks;i++)
   {
      strminus(tlista[i].ProcessName);
      if (strbusca(tlista[i].ProcessName,tmp) >= 0)
      {
         if (KillProcess( &(tlista[i]), TRUE ))
            muertes++;
      }
   }
   
   return(muertes);
}
#endif


extern int no_especial;

extern int solouno;
static char pro[300];


int primer_contexto(int p)
{
    memset(&DsContexto[0],0,sizeof(DsContexto[0]));

    DsContexto[0].fichemenu[128];
    DsContexto[0].menua[25][MAXMENUA+1];
    DsContexto[0].punto;
    DsContexto[0].nommenu[MAXNOMOP];   /* nombre del menu principal */
    DsContexto[0].wmenu;    /* estructuras de menus */
    DsContexto[0].nsmenu;              /* submenu actual 0,1..,MAXSUBMENU-1 */
    DsContexto[0].wdef;
    DsContexto[0].nmenus;
    DsContexto[0].mw[MAXSUBMENU];     /* ventanas del menu actual */
   
    DsContexto[0].pintamenu;           /* si debe o no pintar el menu */
    DsContexto[0].pintahmenu;          /* si debe o no pintar el menu hijo */
    DsContexto[0].dir_smenu;
    DsContexto[0].dir_sgmenu;

    DsContexto[0].empresa;
    DsContexto[0].mpfiemp;

    DsContexto[0].modulo[9];
    DsContexto[0].dirbin[MAXPATH];
    DsContexto[0].dirdef[MAXPATH];
    DsContexto[0].dirpan[MAXPATH];
    DsContexto[0].dirfic[MAXPATH];
    DsContexto[0].nomfdir[MAXPATH];
    DsContexto[0].dirficb[MAXPATH];
    DsContexto[0].dirbass[MAXPATH];
    DsContexto[0].dirbase[MAXPATH];
    DsContexto[0].nomapli[25];
    DsContexto[0].fich_apli[15];    

    DsContexto[0].p = p;

    memcpy(&DsContexto[1],&DsContexto[0],sizeof(DsContexto[1]));
    ActContexto = 1;

    /*return(ActContexto);*/
	return p;
}

static int preinicializado = 0;
int pre_inicializa_systema(int *_argc,char *argv[])
{
   int i;   
   int argc = *_argc;   

#ifdef FGVDOS
   {
      OSVERSIONINFO     verInfo = {0};
      
      verInfo.dwOSVersionInfoSize = sizeof (verInfo);     
      GetVersionEx(&verInfo); 
      switch (verInfo.dwPlatformId)     
      { 
      case VER_PLATFORM_WIN32_NT: 
         GetTaskList     = GetTaskListNT; 
         break;  
      case VER_PLATFORM_WIN32_WINDOWS:        
         GetTaskList = GetTaskList95; 
         break;      
      }
      
      Eliminador_Diagram = MiKill;
   }

#endif   

   ixxdat_micro_espera = demomento_espera_x;

      /* FIJO y CONOCIDO */
   RetroFuncion[0] = RetroGenerico;
   RetroPar[0]     = NULL;
   RetroTipo[0]    = 1;
   RetroPunto = 1;
   
   
   libera_virtual = saca_virtual;
#ifdef ESDOS
#ifndef FGVWIN
#ifdef FGVIXX
   agi_especial_system_antes = ixx_anula_un_dat;
   agi_especial_system_despues = ixx_repon_un_dat;
#endif
#endif
#endif
   
   solouno = 0;
   
   _pon_incidencia = guarda_incidencia;
   ixx_incidencia = guarda_incidencia;
   ixxquita_path = quita_path;
   
   es_un_caso_especial = 0;
   auto_trans = 0;
   trans_save_block = tttk[0] - 48;
   nomapli_ayuda = nomapli;
   strcpy(nomapli,"diagram9");
   nomapli_doble[0]  = 0;
   empresa_doble[0] = 0;
   _fispool[0] = '\0';
   apli_corre[0] = '\0';
   apli_aborto = 0;
   
   
   if (argc == 1)
   {
      strcpy(bufpan,argv[0]);
      *(quita_path(bufpan)) = 0;
      rw_setdirectorio(bufpan);
   }
   
   
   memset(bufpan,' ',80);
   bufpan[80] = '\0';
   rw_user[0] = '\0';
   
   memset(referencia_help,0,128);
   
   f_ayuda = agiayuda; /* asignar puntero a funcion con la direccion adecuada */
   memo_pregu_imp = pregu_impre;
   memo_printa_linea = printa_linea_;
   memo_finimp    = rw_finimp;

   
   UsuarioPedido[0] = 0;
   PasswordPedido[0] = 0;
   {
	   char *lpszParam;
	   char tmp[512];
	   char *p,*p1;

	   for (i = 1;i < argc;i++)
	   {
		 lpszParam = argv[i];   
		 if (lpszParam[0] == '$' && lpszParam[1] == '$' && lpszParam[2] == '$')
		 {		  
			strcpy(tmp,lpszParam+3);
			p = tmp;
			while(*p && *p != ',' && *p != '#') p++;
			if (*p == ',')
			{
			   *p = 0;
			   p++;
			   p1 = p;		 
			   while(*p && *p != '#') p++;
			   if (*p) *p = 0;
			   strcpy(PasswordPedido,p1);
			}
			else		  
			   if (*p) *p = 0;
		 strcpy(UsuarioPedido,tmp);		 
		 }
	   }
   }


   strcpy(prog_rw,argv[0]);
   menusep(&argc,argv);
   
   /* el primero es el menu o la opcion de arranque */
   if (argc > 1) 
   {
      strcpy(par1,argv[1]);
      if (quita_path(par1) == par1)
         strcpy(nmen0,rw_directorio(par1));
      else
         strcpy(nmen0,par1);
   }
   else {
      par1[0] = 0;
      strcpy(nmen0,rw_directorio("ds"));	 
      solouno = -1;
   }   

   if (!solouno && nmen0[0] == '/' && (nmen0[1] == '!' || solouno_()))
   {
      modo_desatendido = 802;
   }

   if (argc > 2)
   {
      strcpy(par2,argv[2]);
      /* el segundo es el directorio base especial aplicacion */
      strcpy(dirbass,argv[2]);
   }
   else {
      par2[0] = 0;
      strcpy(dirbass,rw_directorio(""));
   }
   
   i = strlen(dirbass) - 1;
   if (dirbass[i] != '/' && dirbass[i] != '\\') 
   {
      if (par2[0])
      {
         strcat(par2,"/");
      }
      strcat(dirbass,"/");
   }

   *_argc = argc;
   preinicializado = 1;
   GrabaRegistroEntrada(1);

   if (nomapliset)
   {
	   nomapliset = 0;
       compon_dirbase(dirbase,dirbass,nomapli);
       cambia_paths();
	   strcpy(dirfic,dirficb);
       libera_permisos();
       lee_permisos(nomapli);
   }
  

   return 0;   
}

int inicializa_systema(int argc,char *argv[])
{
   int i,con1,p;
   long fec;   
   int nopaquete = 0;   

   if (!preinicializado)
   {
       pre_inicializa_systema(&argc,argv);
   }
   
   if (mimodo_debug)
      rwdebug = 1;
   else
      rwdebug = 2;
   
#ifdef MEMODEBUG
   RWdebug_();
#endif
   
   /* control de llamada --> si se debe volver a un punto de menu en concreto */
   con1 = llenamenua();
   
   alt_dirfic = getenv("DIRFICH");   
   
   Terinici();
   ini_sig();

   i_leeimpre(rw_directorio("dev/imp/pantalla"));
   /*
   if (!usar_grid)
   {
       ixx_a_version_3_();
   } 
   */
   
   fec = time(NULL);
   if (fec && !salvapar)
   {
      borra_incidencias();
   }
   
   if (mimodo_debug)
   {
      /* las funciones 18 y 19 estan reservadas: 18 interrupcion debugger */
      /*                                         19 logger */
      aginfo = ".";/*"D1.2";*/
      tecla_funcion(17,NULL,0);
      tecla_funcion(18,NULL,0);
   }
   
   no_instala_enbucla = 0;
punto_de_reinicio:
   
   strcpy(empresa,aginfo);
   titulo[0] = 0;
   proceso[0] = 0;
   
   rw_usuario = rw_user;
   rw_extranumuser = &subnumero_de_usuario;
   
   
   inicio_agisys();

   if (!solouno && nmen0[0] == '/' && (nmen0[1] == '!' || solouno_())) {
	   nopaquete = 1;
   }
   
   if (fec && !nopaquete)
   {
      if (controla_paqueteria(1) == -10)
      {
         final_agisys();	 
         fin_sig();
         Terfinal();
         /*execv(mandato,args);*/
         return(-10);     
      }
   }
   
   
#ifdef RWXARXA
   if ((i = establece_conexion(0)) == 2 || i == 3 || i == 4 || i == 5)
   {
      if (i == 4)
      {
         /* conectado !!! */
         char tmpxx[1024];
         int n;
         
         fgv_activa_impredll = 1;
         fgv_activa_progdll  = 1;
         fgv_activa_dragdrop = 1;
         asigna_decimales = mis_decimales;
		 asigna_euro = mis_euro;
         
         n = compon_menua_(tmpxx);
         if (n > sizeof(short)) 
            envia_control_a_server(tmpxx,n,74);
         return(4);
      }
	  /* DE MOMENTO SE DEPENDE DEL 9 PARA ACTUALIZAR */
	  /* reinicializa */	  
      if (i == 3) 
      {
         no_instala_enbucla = 1; 
		 /*
         goto punto_de_reinicio; 
		 */
		  final_agisys();	 
		  fin_sig();
		  Terfinal(); 
		  return -1;
      }
	  
	  
      final_agisys();	 
      fin_sig();
      Terfinal(); 
      return(-1);     
   }
   else
   {
#endif
      
      if (!fec)
      {       
#ifdef RWXARXA
#ifdef RWSERVER
         if (en_conexion_externa)
         {
            aviso(0,"LA FECHA DEL SERVIDOR ES INCORRECTA, SEGURAMENTE MAYOR DEL AÑO 2030."); 
         }
         else
         { 
            aviso(0,"LA FECHA DEL SISTEMA ES INCORRECTA, SEGURAMENTE MAYOR DEL AÑO 2030.");
         }
#else
         aviso(0,"LA FECHA DEL SISTEMA ES INCORRECTA, SEGURAMENTE MAYOR DEL AÑO 2030.");
#endif
         establece_desconexion(0);
#else
         aviso(0,"LA FECHA DEL SISTEMA ES INCORRECTA, SEGURAMENTE MAYOR DEL AÑO 2030.");
#endif
         
         fin_sig();
         Terfinal();
         return(-1);
      }
      
#ifdef RWSERVER
      if (en_conexion_externa && help_externo)
      {
         f_ayuda = ayuda_remota;
      }
#endif
#ifndef FGVDOS
      help_externo  = NULL;
#endif
	  
	  GrabaRegistroEntrada(0);

      if (fec && !nopaquete)
         controla_paqueteria(0);
      
      inicio_dir(); /* carga permisos generales */      

      p = 0;
	  if (!nopaquete) {
		  for (i = 0;i < nPaq;i++)
		  {
			 if (Paq[i].i.tipo == 'P' && Paq[i].flag == 1)
			 {
				p++;
			 } 
		  }
	  }
      
      if (p)
      {
         sprintf(pro,"Hay %d Paquetes Por Actualizar, Entre en la Opcion de Instalaciones",p);
         aviso(0,pro);
      }
      
#ifdef RWXARXA
   }
#endif  
   
   
   if (!solouno && nmen0[0] == '/' && (nmen0[1] == '/' || nmen0[1] == '!'))
   {
      if (nmen0[1] == '!')
         modo_desatendido = 802;
      for (i = 0;nmen0[i];i++)
      {
         if (nmen0[i] == '&')
              nmen0[i] = ' ';
      }
      i = strbusca(nmen0,"=");
      if (i < 0)
      {
         strcpy(pro,nmen0+2);
         strcpy(nmen0,rw_directorio("ds")); 
      }
      else
      {
         nmen0[i] = 0;         
         if (nmen0[i+1] == '-')
         {
             int j;

             i += 2;

             j = i;
             while(nmen0[i] && nmen0[i] != '-')
             {
                 i++;
             }
             if (!nmen0[i])
             {
                 i--;
             }
             else
             {
                int xx;
                nmen0[i] = 0;
                for (xx = j;nmen0[xx] && nmen0[xx] != ',';xx++);
                if (nmen0[xx] == ',')
                {
                    nmen0[xx] = 0;
                    if (strlen(nmen0+xx+1) > 10)
                       nmen0[xx+11] = 0;
                    strcpy(user_batch_psw,nmen0+xx+1);
                }
                if (strlen(nmen0+j) > 10)
                    nmen0[j+10] = 0;
                strcpy(user_batch,nmen0+j);
             }
         }

         strcpy(pro,nmen0+2);

         if (!strlen(nmen0+i+1))
         {         
            strcpy(nmen0,rw_directorio("ds")); 
         }
         else
         {             
             if (nmen0+i+1 == quita_path(nmen0+i+1))
             {
                strcpy(nmen0,rw_directorio(nmen0+i+1));
             }
             else
                strcpy(nmen0,nmen0+i+1);	   
         }
      }
      solouno = 1;
	  con1 = 0;
      for (i = 0;pro[i];i++) {
          if (pro[i] == '(') {
			  if (con1 == 1)
				  break;
			  con1 = 1;
		  }
          else if (con1 == 1 && pro[i] == ')') {
			  con1 = 2;
		  }
		  else if (con1 == 2) {
			  if (!memcmp(pro+i,"DS.SQL:",7)) {
					char *p1;
					int portc = -1;
					p1 = pro+i+7;
					while(*p1 && *p1 != '.') p1++;
					if (*p1 == '.') {
						*p1 = 0;
						portc = atoi(p1+1);
						*p1 = '.';
					}
					es_dssql = portc;
			  }
		  }
	  }
   }
   else
      solouno = 0;   

   p = lee_prot();  
   if (!negocia_dsdb(dsdb_))
   {
	  p = -1;
	  fin_sig();
      Terfinal();
      exit(2);
   }

#ifdef RWSERVER
   if (en_conexion_externa)
   {
      envia_serie();
   }
#endif 
   
   if (_Posicion() == 777)
   {
      /*aviso(0,"VERSION INTERNA DIAGRAM SOFTWARE");*/
   }
   else
  if (_Posicion() == 778)
  {
     /*aviso(0,"VERSION LIMITADA");*/
  }
  else
 if (!_Posicion())
 {
    aviso(0,"VERSION DE EVALUACION!");
 }
 
 fgv_activa_impredll = 1;
 fgv_activa_progdll  = 1;
 fgv_activa_dragdrop = 1;
 
 asigna_decimales = mis_decimales;
 asigna_euro = mis_euro;

 InicializaAutoCompletar();
 
 if (p < 0)
     return p;
  
 return(primer_contexto(p));
}


int login_usuario(int p,char *usuario,char *password,char *cambio)
{
   int i;
   /*
   if (contexto < 0)
   */
       /*p = contexto;*/
	/*
   else
       p = DsContexto[contexto].p;
	*/

#ifdef RWXARXA
   if (en_conexion_terminal)
   {
      char tmp[250];
      strcpy(tmp,usuario);
      memcpy(tmp+21,password,11);
	  if (cambio && *cambio)
	  {
		  memcpy(tmp+32,cambio,11);
		  envia_control_a_server(tmp,43,70);
	  }
	  else
	  {
		  envia_control_a_server(tmp,32,70);
	  }
      if ((i = modo_terminal()) != 4) {
		 if (i == 5)
			 return 0;
         return i-4;
	  }
      _ejecuta_evento_fichero15 = ejecuta_evento_fichero15;
      _ejecuta_evento_fichero16 = ejecuta_evento_fichero16;
      _ejecuta_evento_fichero17 = ejecuta_evento_fichero17;
      return 1;
   }
#endif
   
   user_who = -1; /* siempre!! */

   /*
   if (usuario && !strcmp(usuario,"batch")) {
	   return 0;
   }
   */

   if ((i = entra_user(p,usuario,password,cambio)))
   {
	  if (i > 1)
		  return i;
      _ejecuta_evento_fichero15 = ejecuta_evento_fichero15;
      _ejecuta_evento_fichero16 = ejecuta_evento_fichero16;
      _ejecuta_evento_fichero17 = ejecuta_evento_fichero17;	  
#ifdef RWSERVER
	  carga_ini_de_usuario();
#endif
      return 1;
   }   

   return 0;
}
#ifdef PROFALLIDOX
int login_usuario(int contexto,char *usuario,char *password, char *cambio)
{
   int i;
   int p;
   /*
   if (contexto < 0)
   */
       p = contexto;
	/*
   else
       p = DsContexto[contexto].p;
	*/
#ifdef RWXARXA
   if (en_conexion_terminal)
   {
      char tmp[22];
      memcpy(tmp,usuario,11);
      memcpy(tmp+11,password,11);
      envia_control_a_server(tmp,22,70);
      if (modo_terminal() != 4)
         return 0;

	  _ejecuta_evento_fichero15 = ejecuta_evento_fichero15;
	  _ejecuta_evento_fichero16 = ejecuta_evento_fichero16;
	  _ejecuta_evento_fichero17 = ejecuta_evento_fichero17;

      return 1;
   }
#endif
   
   user_who = -1; /* siempre!! */
   if (entra_user(p,usuario,password,cambio))
   {
     _ejecuta_evento_fichero15 = ejecuta_evento_fichero15;
     _ejecuta_evento_fichero16 = ejecuta_evento_fichero16;
     _ejecuta_evento_fichero17 = ejecuta_evento_fichero17;

      return 1;
   }
   
   return 0;
}
#endif

   
int opcion_usuario(int contexto)
{   
   int con1;
   int p;
   /*
   if (contexto < 0)
   */
       p = contexto;
	/*
   else
       p = DsContexto[contexto].p;
	*/

#ifdef RWXARXA
   if (en_conexion_terminal)
   {
      envia_control_a_server("",0,71);
      if (modo_terminal() != 4)
         return 0;
      return 1;
   }
#endif

   f_util   = entra_util;              

   cls();

   con1 = 0;
   mpfi[48] = NULL;
   if (p)
   {		     
      strcpy(pro,":agitool/ds_confi.tab");             
      solouno = 1;
   }
   else
   {
#ifdef RWSERVER
	   if ((remoto_es_sistema_grafico & 128)) {
		   char tmp[512];
		   sin_mensaje_error_a = 1;
		   strcpy(tmp,":dstool/autorun.tab");
		   if (ejecuta_ap(tmp,0,NULL) < 0)
		   {
			   sin_mensaje_error_a = 1;
			   strcpy(tmp,":agitool/autorun.tab");
			   ejecuta_ap(tmp,0,NULL);
		   }
		   sin_mensaje_error_a = 0;	   
	   }
#endif
   }
   
   if (solouno)
   {		   
      opcion_ej(pro,"");
   }
   else
   {
      con_acceso_directo = 1;	  
      while (!llama_menu(nmen0,pro,con1))
      {			 
         opcion_ej(pro,"");
         con1 = 1;
         inicio_dir(); 
      }
   }
      
      if (solouno || p || Acceso_directo)
         return(0);                
      return(1);
}

int logout_usuario()
{
#ifdef RWXARXA
   if (en_conexion_terminal)
   {
      envia_control_a_server("",0,72);
      if (modo_terminal() != 4)
         return 0;
      return 1;
   }
#endif
   libera_permisos();
   sal_user(0); /* salida normal */
   cierra_todo(0);
   if (memoplin > 9)
      acaba_impresora(v_memo(5009)->alfabetica);
   else
      acaba_impresora(NULL);
   
#ifdef ESDOS
   fcloseall();
#endif
   f_util   = NULL;        
   _ejecuta_evento_fichero15 = NULL;
   _ejecuta_evento_fichero16 = NULL;
   _ejecuta_evento_fichero17 = NULL;

   return 0;
}


int finaliza_systema()
{
   FinalizaAutoCompletar();

   /* operaciones finales */
#ifdef RWXARXA
   if (en_conexion_terminal)
   {
      envia_control_a_server("",0,73);
      modo_terminal();
   }
#endif
   
#ifdef RWXARXA
   establece_desconexion(0);
#endif
   
   libera_permisos();
   
   ixx_limpia_pseudos(0);
   
   final_agisys();
   
#ifdef FGV32
   if (fIsTNT)
   {
      cls();
   }
#endif
   
   fin_sig();
   cursor(2401);
   Terfinal();
   
   
#ifdef MEMODEBUG
   if (rwdebug == 1)
      RWfindebug_();
#endif

   fincache();
   
   /* final programa */
   return(0);
}

int RetroDespachador(void *puntero)
{
int opcion;
if (en_conexion_terminal)
   {
   if (envia_retro_al_server(puntero) < 0)
	   return -1;
   return 0;
   }
memcpy(&opcion,puntero,sizeof(int));
if (opcion >= 0 && opcion < 512 && RetroFuncion[opcion])
   {
   char *p = (char *)puntero;
   p += sizeof(int);
   ( *RetroFuncion[ opcion ] )(p,RetroPar[opcion],opcion);
   }
   return 0;
}

#ifdef RWSERVER

void antes_de_salir(void)
{
  if (es_dssql > 0) {
	  DSSQL_atexit(es_dssql,mensajesql);
  }
}

extern int _modo_portal;

main(argc,argv)
int argc;
char *argv[];
{
   int p,r,waitfor,len;
   char tmp[1024];   

   /*
   strcpy(fichdir,"fich");
   strcpy(fichdirbarra,"fich/");
   */
   atexit( antes_de_salir );

   p = inicializa_systema(argc,argv);
   if (p == -1) return 2;

   f_util   = entra_util;
   mpfi[48] = NULL;
   
   if (en_conexion_externa)
   {
	   extern int pass_cambio_autorizado;

	   if ((remoto_es_sistema_grafico & 128))	  
		   pass_cambio_autorizado = 1;
      /* enviar señal al cliente de que estamos listos para la siguiente fase */      
	   if (_modo_portal && (remoto_es_sistema_grafico & 128))	  
		   envia_comando_a_terminal("",0,202);
	   else      
		   envia_comando_a_terminal("",0,200);
      do {
         /* procesador de directivas primarias del cliente */
         while(en_conexion_externa && (len=leecontrol_en_terminal(tmp,-1,&waitfor)) < 0);
         if (!en_conexion_externa)
            break;
         /*while(en_conexion_externa && leecontrol_en_terminal(tmp,1024,&waitfor) < 0);
           if (!en_conexion_externa)
            break;*/
         switch(waitfor)
         {
         case 70:
			 {
				 char *cambio;
				 char *pass = NULL;
			     if (len == 43)
					 cambio = tmp+32;
				 else
					 cambio = NULL;
				 
				 /*if (!siempre_password)*/
					 pass = tmp+21;
				
				 if ((r = login_usuario(p,tmp,pass,cambio)) == 1)
				    envia_comando_a_terminal("",0,200);
				 else {					
					envia_comando_a_terminal("",0,201+r);
				 }
			}
            break;
         case 71:            
            if (opcion_usuario(p))
               envia_comando_a_terminal("",0,200);
            else
               envia_comando_a_terminal("",0,201);                         
            break;
         case 72:
            if (logout_usuario())
               envia_comando_a_terminal("",0,200);
            else
               envia_comando_a_terminal("",0,201);            
            break;
         case 73:            
            finaliza_systema();
            return(0);
         case 74:
            regulariza_menua_(tmp);           
            break;
         default:
            break;	     
         }    
      } while(-1);      
   }
   else
   {
       /* modo background */
       if (solouno || p)
       {
            if (login_usuario(p,user_batch,user_batch_psw,NULL) == 1)
                opcion_usuario(p);
			else
				mensajesql = "Error de credenciales";
       }
   }
   
   finaliza_systema();
   return(0);
}

#endif
