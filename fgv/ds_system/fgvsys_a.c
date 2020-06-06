
/**********************************************************************/
/**********************************************************************/
/*                        DIAGRAM9                                    */
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
#else
#include <sys/param.h>
#include <sys/mount.h>
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
#ifdef FGVLINUX
#include <sys/vfs.h>
#endif
#ifdef FGV32
#include <process.h>
#endif
#ifdef FGVWIN
#include <fgvwin.h>
extern HANDLE hPortScanner;
#endif


#include <stdlib.h>
/*extern char *getenv();*/


int sin_mensaje_error_a = 0;

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

extern int ini_minmegaslibres;
extern int modo_minmegaslibres;

/*
#ifdef ESDOS
extern int (*timer_especial_int)();
#endif
*/

#ifdef RWSERVER
extern void llama_a_despachador(int f,int parametro,char *puntero);
#endif


void despachador_vacio(int f,int parametro,char *puntero)
{
}

extern int es_un_caso_especial;

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


extern char *rw_modulo;

extern int modo_desatendido;

extern int no_instala_enbucla;

extern int pasa_a_int(double);

extern int con_acceso_directo;

extern int siempre_password;

char *nombre_tecla();

extern char *PreEjecucion;

char empresa[MAXDEFECTO];
char proceso[81];
char titulo[81];
char bufpan[PANBUFSIZE];     /* buffer de pantalla se usa solo como cadena
de 80 blancos                              */
extern char dirbin[51];
extern char dirdef[51];
extern char dirpan[51];
extern char dirfic[51];
extern char nomfdir[51];
extern char dirficb[51];
extern char dirbass[51];
extern char dirbase[51];

extern char *alt_dirfic = NULL;

extern char nomapli[25];

extern char *nomapli_ayuda;

extern char modulo[9];

extern int fgv_ver_euro;
extern int modo_otro_euro;

#ifdef FGV32

extern BOOL fIsTNT;

#endif

char prog_rw[128];
extern char nmen0[551];
extern char par1[551];
extern char par2[551];

extern char *PortScanner;

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

void (*RetroFuncion[512])(char *bloque,char *par,int el_punto) = {NULL,NULL,};
char *RetroPar[512] = {NULL,NULL,};
int RetroTipo[512] = {0,0,};
int RetroPunto = 0;
extern int modo_de_relacion = 0;


extern int (*memo_pregu_imp)();
extern int (*memo_printa_linea)();
extern char *dsdb_;
extern int (*memo_finimp)();


extern int mimodo_debug;
extern char *logger_salida;

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

extern char _fispool[51];
extern char _ejspool[51];

char *aginfo = "";

char user_batch[11] = "batch";
char user_batch_psw[11] = "";

extern char *quita_path(char *);

extern int LEERTERMINAL;

extern int bucle_break;


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

extern int (*repinta_ultimaficha)();

extern double DsEuroConvierte; 
extern int DsDeciModoOtro;
extern int DsDeciModoNormal;
extern char DsEuroTexto[512];


extern int _modo_portal;

#ifndef DSMINIBASICO
int mis_decimales(char *nom)
{
   int i;
   char tmp[128];
   int defecto = 7;
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
	  if (!strcmp("nDeci_DDEFECTO",m_ex[i].nombre) && m_ex[i].m_tipo != CHARTIPO && m_ex[i].m_tipo != FECHATIPO)
	  {
		  defecto = (int)m_ex[i].mv.vald;
	  }
   }
   return(defecto);
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

static int u_p_ipfi = -1;
static int u_p_npan = 0;

int inicializa_repinta_mificha()
{
	u_p_ipfi = -1;
	u_p_npan = 0;
}

int repinta_mificha()
{
	if (u_p_ipfi >= 0 && u_p_ipfi < nfi && v_mpfi(u_p_ipfi)->f_npanta > u_p_npan)
	{		
		prin_pan(u_p_ipfi,u_p_npan);
	}
	return 0;
}



int dame_opcion_de_menu(int tecla,char *tmp)
{          
  int k = tecla;
  int j = k / 100;
  int hijo;
  int liberar = 0;
  k %= 100;
  
  if (!wmenu)
  {              
      leemenu(fichemenu);
      if (!wmenu) return -1;
      liberar = 1;
  }

  if ((hijo = wmenu[j]->m_op[k]->o_menuhijo) == -1)
  {
     strcpy(tmp,wmenu[j]->m_op[k]->o_programa);
  }

  if (liberar)
  {
      libera();
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
      aviso(0,__Texto[0]);
   }
   r = en_pr[programa_activo].tabla_ficheros[f];
   if (r >= 1000)
   {
      r -= 1000;
      r = pasa_a_int(v_memo(r)->numerica);
      if (r < 0 || r >= nfi)
      {
         aviso(0,__Texto[1]);
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
      aviso(0,__Texto[2]);
			}
   
   if (c < 5000) return(-1);
   c -= 5000;
   if (c > memoplin) return(-1);
   return(0);
}

int que_memo_tipo(n)
int n;
{
			register i;
         
         if (programa_activo < 0)
         {
            aviso(0,__Texto[3]);
         }
         
         if (n >= 5000)
         {
            n -= 5000;
            i = memoplin+maxpila*2;
            if (n < i)
               return(m_ex[n].m_tipo);
            return(m_ex[ en_pr[programa_activo].tabla_externa[n-i] ].m_tipo);
         }
         return(memo[n].m_tipo);
}


static char tempo_memo_buf[300];

static union f_val_memo default_memo;

union f_val_memo *v_memo(n)
int n;
{
   register i;
   /*
   char tmp[512];
   sprintf(tmp,"%d",n);
   aviso(0,tmp);
   */
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
    if (n < i)
         return((union f_val_memo *)&(m_ex[n].mv));
    if (programa_activo < 0)
		{
        aviso(0,__Texto[4]);
		}
   return((union f_val_memo *)&(m_ex[ en_pr[programa_activo].tabla_externa[n-i] ].mv));
   }
   
   
   if (programa_activo < 0)
   {
      aviso(0,__Texto[5]);
   }
   
   if (n < 0 || n >= nmemo)
   {
      aviso(0,__Texto[6]);
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
   for (;i<nfi;i++) {        
/*
#ifdef ESDOS
      if (timer_especial_int)
									(*timer_especial_int)();
#endif
*/
      if (fd_mpfi(i) > -1) {
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


										crea_dir(tmp);
                                       
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
   if (tipo == CHARTIPO || tipo == FECHATIPO) 
   {
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
   int i;
   
   /*if (nfi == 49) i = 48; else i = 0;*/
   i = 0;
   for (;i<nfi;i++)
      pag_activa[i] = -1;
   if (memoplin > 2) memset(v_memo(5002)->alfabetica,'\0',11);
   return(0);
}

int ver_activa(f,c)
int f,c;
{
   int i;
   
   if ((i = pag_activa[f]) < 0) return(-1);
   if ((v_mpfi(f)->f_campos[c-1].c_mante.c_actual / 1000) != i) return(-1);
   return(0);
}

des_activa(f)
int f;
{
   if (memoplin > 2) memset(v_memo(5002)->alfabetica,'\0',11);
   pag_activa[f] = -1;
}

activa_pag(ipfi,n)
int ipfi,n;
{
   pag_activa[ipfi] = n;
   if (memoplin > 2) 
   {
      memcpy(v_memo(5002)->alfabetica,quita_path(v_mpfi(ipfi)->f_fichero),9);
      v_memo(5002)->alfabetica[10] = '0'+ n;
      v_memo(5002)->alfabetica[11] = '\0';
   }
   return(0);
}
#endif

#ifndef DSMINIBASICO


#ifdef FGVWIN
/* pinppan
* pinta la pantalla o ventana correspondiente al def
*/
static _pinppan(ipfi,n,modo)
int ipfi;
int n;
int modo;
{
   VENT *w = w_ini(101,v_mpfi(ipfi)->f_pantalla[n].p_posfin);
   char *pp;
   int ln,l;
   
   activa_pag(ipfi,n);
   
   if (w_lee(w,v_mpfi(ipfi)->f_pantalla[n].p_nombrepan) == -777)
   {
	   w_fin(w);
	   return -777;
   }
   w_mueve(w,v_mpfi(ipfi)->f_pantalla[n].p_posref);
   if (modo)
      w_metev(stdpan,w);
   else
      w_mete(stdpan,w);
   w_refresca(w);
   w_fin(w);
   return(0);
}
#else
#if defined(RWXARXA) || defined(RWSERVER)
static _pinppan(ipfi,n,modo)
int ipfi;
int n;
int modo;
{
   VENT *w = w_ini(101,v_mpfi(ipfi)->f_pantalla[n].p_posfin);
   char *pp;
   int ln,l;
   
   activa_pag(ipfi,n);
   
   w_lee(w,v_mpfi(ipfi)->f_pantalla[n].p_nombrepan);
   w_mueve(w,v_mpfi(ipfi)->f_pantalla[n].p_posref);
   if (modo)
      w_metev(stdpan,w);
   else
      w_mete(stdpan,w);
   w_refresca(w);
   w_fin(w);
   return(0);
}
#else
/* pinppan
* pinta la pantalla o ventana correspondiente al def
*/
static _pinppan(ipfi,n,modo)
int ipfi;
int n;
int modo;
{
   VENT *w;
   char *pp;
   char venta[sizeof(VENT)];
   int ln,l;
#ifndef ESDOS
   char tmp[6000];
#else
   char *tmp = NULL;
#endif
   
   activa_pag(ipfi,n);
   
   w = (VENT *)&venta;
   w->vini = 101;
   w->vfin = v_mpfi(ipfi)->f_pantalla[n].p_posfin;
   ln = (w->vfin/100) * (w->vfin%100);
   w->size = (-ln);
#ifdef ESDOS
   /*#ifndef AGIEXTEND
   tmp = alloca(ln*3);
   w->vv = tmp;
#else*/
   tmp = NULL;
   w->size = ln;
   w->vv = malloc(ln*3);
   /* #endif*/
#else
   w->vv = tmp;
#endif
   w_lee(w,v_mpfi(ipfi)->f_pantalla[n].p_nombrepan);
   l = w->size;
   if (l < 0)
      w->size = ln;
   w_mueve(w,v_mpfi(ipfi)->f_pantalla[n].p_posref);
   if (modo)
      w_metev(stdpan,w);
   else
      w_mete(stdpan,w);
   w_refresca(w);
   if (l > 0 && w->vv != tmp)
   {
      free(w->vv);
   }
   return(0);
}
#endif
#endif

pinppan(ipfi,n)
{
   if (v_mpfi(ipfi)->f_npanta > n)
   {
	  return(_pinppan(ipfi,n,modo_de_relacion));
   }
   else
	   return -1;
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
   
   if (v_mpfi(ipfi)->f_campos[c-1].c_mante.c_visual != 0) return(0);
   
   if (actualiza_cam(ipfi,c,faclin,14) == -1) 
   {
      v_mpfi(ipfi)->f_campos[c-1].c_fflag = salvaa;
      return(0);
   }
   
   if ((v_mpfi(ipfi)->f_campos[c-1].c_mante.c_lineal & C_LINEAL) != 0 && faclin > 0)
   {
      if (fi_modo_lineas[ipfi])
      {
         pp = v_mpfi(ipfi)->f_campos[c-1].c_mante.c_qp + ((faclin - 100)/100) * fi_modo_lineas[ipfi];
      }
      else
         pp = v_mpfi(ipfi)->f_campos[c-1].c_mante.c_qp + faclin - 100;
      l = 1;
   } 
   else
      pp = v_mpfi(ipfi)->f_campos[c-1].c_mante.c_qp;
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
   
   printcam(v_mpfi(ipfi),c);
   
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
   int pausasiempre = 0;

   if ((modo/100))
   {
	   pausasiempre = 1;
	   modo %= 100;
   }
   
   panfin = v_mpfi(ipfi)->f_npanta;
   if (v_mpfirel(ipfi)->lineas > -1) {panlin = panfin;panfin++;}
   
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
   for (panini=0,numpan=ipan;numpan<panfin;numpan++) 
   {
      
      
      if ((numpan < panini || pintada1 == 1))
      {
         pintar = 0;           /* no pintar la pantalla otra vez */
         if (!pintada1)
            numpan = panini;
         pintada1 = 0;
      }
      else
         pintar = -1;

	  if (pausasiempre)
		  pintar = -1;

      estoyen = numpan;
      
      
      if (DespachadorExterno)
      {
         if (!_M_EXTRA_AVANCE)
		 {
			if (estoyen == panlin && pausasiempre)
			{
				DespachoQuePantalla((int)DESPACHO_QUEPANTALLA,(int)panlin-1,v_mpfi(ipfi)->f_titulo);
			}
			else
			{
				DespachoQuePantalla((int)DESPACHO_QUEPANTALLA,(int)estoyen,v_mpfi(ipfi)->f_titulo);
			}
		 }
      }
      
      
      
      if (numpan == panlin) {
         trata_lineas(ipfi,CONSULTA,0,0,0);
         numpan -=1;
         estoyen = numpan;
         if (trata_lineas(ipfi,CONSULTA,3,0,0) != 0) 
		 {
            numpan -=1;
            continue;
         }
      }
      else {
         if (pintar) 
		 {
            pinppan(ipfi,numpan);                  /* pintar pantalla */
            /*cabecera_v(rw_version,proceso,titulo,empresa);*/
			cabecera(proceso,titulo,empresa);
         }
         salida = 0;
         prin_pan(ipfi,numpan);
         if (panfin == 1 && !pausasiempre)
            break;
      }
      tmp[0] = 0;
      if (estoyen <= panfin)
      {
         if (panlin == -1 || estoyen <= panlin)
         {
            strcat(tmp,__Texto[506]);
         }
         else
         {
            strcat(tmp,__Texto[364]);
         }
      }
      if (estoyen > panini)
      {
         if ((estoyen+1) < panfin)
         {
            strcat(tmp,__Texto[505]);
         }
         else
         {
            strcat(tmp,__Texto[504]);
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
      
	  if (!pausasiempre)
	  {
		  fin_botones_();
	  }

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
   
   
   if (estoyen != 0) 
   {  /* estoy en una pantalla != inicial (=0) */
      pinppan(ipfi,0);
      if (modo == 0) prin_pan(ipfi,0); /* pintar pantalla */
      if (ppp) {
         *ppp = 0;
         tmp[0] = 0;
         if (panfin > 1)
         {
            if (panlin == -1 || panlin > 1)
            {
               strcat(tmp,__Texto[503]);
            }
            else
            {
               strcat(tmp,__Texto[502]);
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
   return(_pinta_datos(ipfi,modo,NULL,0));
}



static int borrar_las_memos(ipfi)
int ipfi;
{
   char tmp[51];
   int i,f = v_mpfi(ipfi)->f_ncampos;
   
   for (i = 0;i < f;i++) {
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
            aviso(0,__Texto[8]);
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
   VENT *salva;
   int libera = 0;
   int repite = 0;
   
   do {
/*
#ifdef ESDOS
						if (timer_especial_int)
                     (*timer_especial_int)();
#endif
*/
                  
                  if (h == 0) {
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
                        if (h == 2) {
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
                        if (e != 0) {
                           if (m != 4 && repite == 0) {
                              if (iserrno == ELOCKED || m == 2 || (m == 1 && lista_error() == 2)) {
                                 salva = w_ini(818,1662);
                                 libera = -1;
                                 w_saca(salva,stdpan);
                                 pinta_error(f,d);
                              }
                           }
                           
                           if ((repite = captura_salida(r,libera)) == 0) break;
                        }
   } while(e != 0);
   if (libera == -1) {
						w_pinta(salva);
                  w_fin(salva);
   }
   if (e) {
						f_n(f,RENBUF); /* ante cualquier error se quita la marga de que esta leido */
                  /* para las relaciones y tiene que volverlo a leer */
   }
   if (e == -1 && iserrno == 0)
   {
	   iserrno = ENOTOPEN;
   }
   return(e);
}

leer_reg(f,o,b,t)
int f,o,b,t;
{
   return(trata_reg(f,o,b,t,0,__Texto[507]));
}
grabar_reg(f,o,t)
int f,o,t;
{
   return(trata_reg(f,o,0,t,1,__Texto[508]));
}
borrar_reg(f,o,t)
int f,o,t;
{
   return(trata_reg(f,o,0,t,2,__Texto[509]));
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
   register int k,i,l,a,h,kk,rr,cc;
   
   for (k=0;k<v_mpfirel(ipfi)->nrelpin;k++) {
      if ( v_mpfirel(ipfi)->relpin[k].qp  > 0 ) {
         i = v_mpfirel(ipfi)->relpin[k].campo;
         if ((v_mpfi(ipfi)->f_campos[i].c_mante.c_actual / 1000) == npan) 
         {
					 l = v_mpfirel(ipfi)->relpin[k].nfirel;
					 rr = v_mpfirel(ipfi)->relpin[k].numrela;
					 for (kk = 0;(cc = v_mpfi(ipfi)->f_relaci[rr].r_campo[kk]);kk++)
					 {						    
							if (!hay_dat_permiso(v_mpfi(ipfi),cc-1))
							{
								break;
							}
					 }
					 if (v_mpfi(ipfi)->f_relaci[rr].r_campo[kk])
					 {
						    defecto_ficha(v_mpfi(l));
							cursor(v_mpfirel(ipfi)->relpin[k].qp);
							printusi_i(v_mpfi(l),v_mpfirel(ipfi)->relpin[k].nficampo+1);
					 }
					 else
					 {
							a = f_v(l,RABIERTO);
							if (abre1_rel(l) == 0) 
							{						
								   h = clave_rel(v_mpfirel(ipfi)->relpin[k].numrela,
									  l,-1,0,ipfi,-1,DIFERIDO,FALSO);
								   if (h >= 0)
									  h = leer_reg(l,ISEQUAL,0,1);
								   if (h == -1 || (h == -2 && f_v(l,RENBUF) != 0)) {
									  defecto_ficha(v_mpfi(l));
									  f_n(l,RENBUF);
								   }
								   else
									  f_s(l,RENBUF);
								   cursor(v_mpfirel(ipfi)->relpin[k].qp);
								   printusi_i(v_mpfi(l),v_mpfirel(ipfi)->relpin[k].nficampo+1);
							}
							if (a == -1) cierra1_rel(l);
					 }
		 }
      }
   }
   return(0);
}


/*
* printpan
* pinta el contenido de los campos de una pantalla
*/
printpan(ipfi,numpan)
int ipfi;
int numpan;
{
   int numcam;
   int campini,campfin;
   int visual,campo;
   int salvaa;

   if (v_mpfi(ipfi)->f_npanta <= numpan) return -1;
   
   
   if (!hay_dat_permiso(v_mpfi(ipfi),-1)) 
	   return(-1);
   
   campini = v_mpfi(ipfi)->f_pantalla[numpan].p_campoini;
   campfin = v_mpfi(ipfi)->f_pantalla[numpan].p_campofin;
   
   
   if (rentra_sal_cc != rentra_sal_cf)
      Color(rentra_sal_cc,rentra_sal_cf);
   atributo(rentra_sal_atrib);
   
   
   for (numcam=campini; numcam<=campfin;numcam++) 
   {
      campo = v_mpfi(ipfi)->f_campos[numcam-1].c_mante.c_actual % 1000;
      visual = v_mpfi(ipfi)->f_campos[campo].c_mante.c_visual;
      if (visual == 0) {
         salvaa = v_mpfi(ipfi)->f_campos[campo].c_fflag;
         if (actualiza_cam(ipfi,campo+1,0,14) != -1) 
		 {
            a_atri_campo(v_mpfi(ipfi),campo,rentra_sal_atrib,0,rentra_sal_cc,rentra_sal_cf);
            printusi(v_mpfi(ipfi),campo+1);
            a_atri_campo(v_mpfi(ipfi),campo,rentra_sal_atrib,1,rentra_sal_cc,rentra_sal_cf);
         }
         v_mpfi(ipfi)->f_campos[campo].c_fflag = salvaa;
      }
   }
   
   atributo(tolower(rentra_sal_atrib));
   fflush(stdout);
   return(0);
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

   if (v_mpfi(ipfi)->f_npanta <= npan)
	   return -1;
   if (npan < 0) {npan = 0;m = 2;} else m = 0; 

   u_p_ipfi = ipfi;
   u_p_npan = npan;
   
   if (ejecuta_otro(ipfi,13,O_NOR,0,0) != -1) 
   {
      printpan(ipfi,npan);
      printprel(ipfi,npan);
      fflush(stdout);
      trata_lineas(ipfi,DIFERIDO,m,0,0);
   }
   return(0);
}

limpia_ficha(ipfi)
int ipfi;
{
   defecto_ficha(v_mpfi(ipfi));
   hay_dat_permiso(v_mpfi(ipfi),-3);
   prin_pan(ipfi,-1);
   f_n(ipfi,RENBUF);
}

int campo_linea(f)
int f;
{
   return(fi_clave_lineas[f] / 100);
}

int da_cla_lin(f)
int f;
{
   return(fi_clave_lineas[f] % 100);
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
   if (i == INTTIPO || i == DOUBLETIPO || i == LONGTIPO)
   {
      printf(__Texto[253],campo);
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
   
   if (accion == 4) {
      tmp = (char **)buf1;
      buf1 = *tmp;
   }
   
   if (!buf1) 
   {
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
   
   if (nclave < 0 || nclave >= v_mpfi(f)->f_nclaves) {
      cursor(2501);
      v_printd(__Texto[254],f);
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
                     if (compara < 0) {
                        compara = -(numcam+1);
                     }
                     else {
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
                  if (accion == 4) {
                     ld_campo(campo,v_mpfi(f),buf1);
                     buf1 += (long_campo(v_mpfi(f),campo) + 1);
                  }
                  else
                     if (accion == 5) {
                        st_campo(campo,v_mpfi(f),buf1);
                        buf1 += (long_campo(v_mpfi(f),campo) + 1);
                     }
                     else
                        if (accion == 6) {
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
   for (j=0;j<v_mpfirel(ipfi)->nrelact;j++) {
      if (v_mpfirel(ipfi)->relact[j].campo != -1) {
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
               if (memoplin > 2) {
                  vca = v_memo(5000)->numerica;
                  vcf = v_memo(5011)->numerica;
                  v_memo(5000)->numerica = (double)campo;
                  v_memo(5011)->numerica = (double)ipfi;
               }
               if (memoplin > 12)
                  sprintf(v_memo(5012)->alfabetica,"%d",v_mpfirel(ipfi)->relact[j].fflag);
               p_ejecuta(i,O_NOR,faclin,3);
               if (memoplin > 2) {
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

   for (ipfi = desde;ipfi <= hasta;ipfi++)
   {
	   if (!mpfirel[ipfi] || !tipo_tabla) continue;

       for (j=0;j<v_mpfirel(ipfi)->nrelact;j++) {
          i = v_mpfirel(ipfi)->relact[j].tabla;
          if (i > 0) {
             r = tipo_tabla[i];
             if (r == cual) {
                if (!ver) {
                   operaerror = 0;
                   if (memoplin > 12)
                      sprintf(v_memo(5012)->alfabetica,"%d",v_mpfirel(ipfi)->relact[j].fflag);
				   vca = v_memo(5000)->numerica;
				   vcf = v_memo(5011)->numerica;  
				   v_memo(5011)->numerica = (double)ipfi;
                   p_ejecuta(i,modo,faclin,4);
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
    
    v_memo(5011)->numerica = (double)ipfi;
	memcpy(tmp,v_memo(5004)->alfabetica,MAXDEFECTO);
    strcpy(v_memo(5004)->alfabetica,v_mpfi(ipfi)->f_PathDef);
    ejecuta_otro(ipfi,88,O_NOR,0,0);

    v_memo(5011)->numerica = (double)ipfi;
    strcpy(v_memo(5004)->alfabetica,v_mpfi(ipfi)->f_PathDef);    
	if (ipfi == 48)
	{
		ejecuta_otro(ipfi,81,O_NOR,0,0);
	}
	else
	{
		ejecuta_otro(-1,81,O_NOR,0,0);
	}

	memcpy(v_memo(5004)->alfabetica,tmp,MAXDEFECTO);
    v_memo(5000)->numerica = vca;
    v_memo(5011)->numerica = vcf;

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
   
   while(!r) { /* en sentido descendente ! */
      clave_lin_trata(f,&tmp,4,"");
      if ((c = clave_lin_trata(f,tmp,m3,l2))) {
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
                  if (apunta_ind(v_mpfi(f),da_cla_lin(f)+1,ISLAST,c)) {
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
   if (que_registro(f,inicio)) 
   {
      h = da_cla_lin(f) + 1;
      apunta_ind(v_mpfi(f),0,ISCURR,0);
      pon_registro_actual(v_mpfi(f),que_registro(f,inicio));
      r = leer_reg(f,ISEQUAL,m,1);
      apunta_ind(v_mpfi(f),h,ISCURR,0);
   }
   else 
   {
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
   if (!(r = leer_reg(f,ISPREV,m,1))) {
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
                     if (mira_sidefecto(ipfi,d) == -1) {
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
                           enclave,r,"",ipfi,faclin);
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
   
   for (k=0;k<v_mpfirel(ipfi)->nreldef;k++) {
      if ( v_mpfirel(ipfi)->reldef[k].nfirel == ipfr &&
         (nrel == -1 || nrel == v_mpfirel(ipfi)->reldef[k].numrela)) {
         i = v_mpfirel(ipfi)->reldef[k].campo;
         if ((mo = _copia_cam(ipfi,i,ipfr,v_mpfirel(ipfi)->reldef[k].nficampo)) >= 0)
									{
            enclave = pintar_otrocampo(ipfi,i,npan,faclin);
            mo = (mo == 0) ? 1 : 0;
            tras_entra(npan,i+1,DIFERIDO,enclave,mo,"",ipfi,faclin);
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
   
   if (i == CHARTIPO || i == FECHATIPO) 
   {
      if (strcmp(buf,tmp) == 0) 
	  {
         return(0);
      }
   }
   else
      if (rwatof(buf) == rwatof(tmp)) 
	  {
         return(0);
      }
      
      return(-1);
}

/*****************************************************************************/


/************************* M A I N *******************************/

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
	int tam,respuesta;
	char *orig_bloque = bloque;
	static char **lista = NULL;
	static int nlista = 0;

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
}
#endif

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



#ifdef FGVDOS
LPGetTaskList     GetTaskList = NULL;

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

extern int (*Eliminador_Diagram)(char *);

int YaEstoy()
{
   DWORD numTasks;
   char tmp[1024];
   char tmp2[256];
   int i;
   int muertes = 0;   
  
   strcpy(tmp,GetCommandLine());
   i = strbusca(tmp,".exe");
   if (i < 0)
	   return 0;
   tmp[i] = 0;
   i--;
   while (i >= 0)
   {
	   if (tmp[i] == '\\' || tmp[i] == '/')
	   {		   
		   break;
	   }
	   i--;
   }
   i++;
   strcpy(tmp2,tmp+i);
   strminus(tmp2);


   numTasks = GetTaskList( tlista, MAX_TASKS );
   /*GetWindowTitles( tlista );*/

   for (i = 0; i < numTasks;i++)
   {
	  if (tlista[i].dwProcessId != getpid())
	  {
		  strminus(tlista[i].ProcessName);
		  if (strbusca(tlista[i].ProcessName,tmp2) >= 0)
		  {
			 muertes++;
		  }
	  }
   }
   
   return muertes;
}

int MiKill(char *proceso)
{
   DWORD numTasks;
   char tmp[128];
   int i;
   int muertes = 0;
   DWORD pid = 0;
   
   strcpy(tmp,quita_path(proceso));
   strminus(tmp);
   if ((i = strbusca(tmp,".exe")) < 0)
   {
	   if ((i = strbusca(tmp,".pid")) >= 0)
	   {
		   pid = (DWORD)atol(tmp+i+4);
		   if (!pid)
		   {
			   return 0;
		   }
	   }
	   else
		   return 0;	   
   }
   tmp[i] = 0;
   
   
   numTasks = GetTaskList( tlista, MAX_TASKS );
   
   for (i = 0; i < numTasks;i++)
   {
      strminus(tlista[i].ProcessName);
      if (strbusca(tlista[i].ProcessName,tmp) >= 0)
      {
		 if (pid)
		 {
			 if (tlista[i].dwProcessId == pid && tlista[i].dwProcessId != getpid())
			 {
				 if (KillProcess( &(tlista[i]), TRUE ))
					muertes++;				 
			 }
		 }
		 else
		 {
			 if (tlista[i].dwProcessId != getpid())
			 {
				 if (KillProcess( &(tlista[i]), TRUE ))
					muertes++;
			 }
		 }
      }
   }
   
   return(muertes);
}



#endif


extern int no_especial;

extern int solouno;
static char pro[128];


int inicializa_systema(int argc,char *argv[])
{
   int i,con1,p;
   long fec;


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

   /*
   if (getenv("COMSPEC"))
	 MessageBox(NULL,"si hola","si hola",MB_OK);
   else
     MessageBox(NULL,"no hola","no hola",MB_OK);
   if (agi_system("/bin/bash") < 0)
       MessageBox(NULL,"no adios","no adios",MB_OK);
   else
	   MessageBox(NULL,"si adios","si adios",MB_OK);
	*/
   

   /* FIJO y CONOCIDO */
#ifndef DSMINIBASICO
   RetroFuncion[0] = RetroGenerico;
   RetroPar[0]     = NULL;
   RetroTipo[0]    = 1;
   RetroPunto = 1;
#endif

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
   else 
   {
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
   else 
   {
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
   
   
   if (mimodo_debug)
      rwdebug = 1;
   else
      rwdebug = 2;
   
#ifdef MEMODEBUG
   RWdebug_();
#endif
   

   GrabaRegistroEntrada(1);
   
   /* control de llamada --> si se debe volver a un punto de menu en concreto */
   con1 = llenamenua();
   
   alt_dirfic = getenv("DIRFICH");   


   Terinici();
   ini_sig();


   i_leeimpre(rw_directorio("dev/imp/pantalla"));

   if (usar_grid != 777)
   {
       ixx_a_version_3_();
   }
   
   
   fec = time(NULL);
   if (fec && !salvapar)
   {
      borra_incidencias();
   }

  
   if (mimodo_debug)
   {
      /* las funciones 18 y 19 estan reservadas: 18 interrupcion debugger */
      /*                                         19 logger */
      aginfo = ".";/*D1.2";*/
      tecla_funcion(17,NULL,0);
      tecla_funcion(18,NULL,0);
   }
   
   no_instala_enbucla = 0;
punto_de_reinicio:
   
   strcpy(empresa,aginfo);
   titulo[0] = 0;
   proceso[0] = 0;   
   

   rw_usuario = rw_user;
   rw_modulo = modulo;
   rw_extranumuser = &subnumero_de_usuario;   
   
   inicio_agisys();      
   
#ifndef DSMINIBASICO   
   if (fec)
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
#endif
   
#ifdef FGVWIN
   PonLogo();
/*
#else
   if (!modo_desatendido)
        pantalla(rw_directorio("inicio.pan"));
*/
   if (PortScanner)
   {
	   hPortScanner = PortInitialize(PortScanner);
   }
#endif

#ifndef DSMINIBASICO
   if (PreEjecucion)
   {
	   char pro[300];
	   strcpy(pro,PreEjecucion);
	   opcion_ej(pro,"");
   }
#endif
  
#ifdef RWXARXA
#ifdef FGVDOS
   if (getenv("DSSOLOUNO"))
   {
	   if (YaEstoy())
	   {
		  final_agisys();	 
		  fin_sig();
		  Terfinal();	
		  exit(1);
	   }
   }
#endif

   i = sizeof(FI_DEF);
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
#ifndef DSMINIBASICO
         asigna_decimales = mis_decimales;
		 asigna_euro = mis_euro;
		 repinta_ultimaficha = repinta_mificha;
#endif
         
         n = compon_menua_(tmpxx);
         if (n > sizeof(short)) 
            envia_control_a_server(tmpxx,n,74);
         return(4);
      }
      if (i == 3) /* reinicializa */
      {
         no_instala_enbucla = 1; 
         goto punto_de_reinicio; 
      }
      final_agisys();	 
      fin_sig();
      Terfinal();	
	  /*exit(1);*/
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
            aviso(0,__Texto[9]); 
         }
         else
         { 
            aviso(0,__Texto[9]);
         }
#else
         aviso(0,__Texto[9]);
#endif
         establece_desconexion(0);
#else
         aviso(0,__Texto[9]);
#endif
         
		 final_agisys();
         fin_sig();
         Terfinal();		 
		 exit(1);
         /*return(-1);*/
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
      
#ifndef DSMINIBASICO   
      if (fec)
         controla_paqueteria(0);
#endif    
	  
      
#ifndef DSMINIBASICO      
      inicio_dir(); /* carga permisos generales */
#endif
      
      p = 0;
      for (i = 0;i < nPaq;i++)
      {
         if (Paq[i].i.tipo == 'P' && Paq[i].flag == 1)
         {
            p++;
         } 
      }
      
      if (p)
      {
         sprintf(pro,__Texto[255],p);
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
            aviso(0,__Texto[10]);
         }
         
         /*
         if (_Posicion() == 777 || _Posicion() == 778)
         {
         */
         fgv_activa_impredll = 1;
         fgv_activa_progdll  = 1;
         fgv_activa_dragdrop = 1;
         /*
         }
         else
         for (i = 0;i< 50;i++)
         {
         if (local_term[i] == 25631)
         {
         if ((local_term_[i] & 1))
         {
         fgv_activa_impredll = 1;
         }		
         if ((local_term_[i] & 2))
         {
         fgv_activa_progdll = 1;
         }		
         if ((local_term_[i] & 4))
         {
         fgv_activa_dragdrop = 1;
         }		
         break;
         }
         }
         */
         /************************************/
         /************************************/		 

#ifndef DSMINIBASICO         
         asigna_decimales = mis_decimales;
		 asigna_euro = mis_euro;         
		 repinta_ultimaficha = repinta_mificha;
#endif


		 {
			 int megaslibres = -1;
			 char tmp[512];
#ifdef FGVDOS
			 {
				ULARGE_INTEGER libre,total;				
				strcpy(tmp,rw_directorio("."));
				cambia_barra_dos(tmp);			
				if (GetDiskFreeSpaceEx(tmp,&libre,&total,NULL))
				{
					megaslibres = (libre.LowPart / 1048576);
					if (libre.HighPart > 524287)
					{
						megaslibres = 2147483647;
					}
					else
					{				
						libre.HighPart *= 4096;
						megaslibres += libre.HighPart;
					}
				}
			 }
#else
			 {
				 struct statfs statfsbuf;			 
				 if (
					 statfs(rw_directorio("."),&statfsbuf) != -1)
				 {
					 if (!(statfsbuf.f_bsize))
					 {
						 statfsbuf.f_bsize = 4096;
					 }
					 megaslibres = (/*statfsbuf.f_bavail */ statfsbuf.f_bfree / (1048576/statfsbuf.f_bsize));
					 /*
					 statfsbuf.f_bsize
					 statfsbuf.f_blocks
					 statfsbuf.f_bavail
					 */
				 }
			 }
#endif		 
			 
			 if (megaslibres > -1 && megaslibres < ini_minmegaslibres)
			 {
				 if ((modo_minmegaslibres%100) == 1)
					 sprintf(tmp,"Solo hay %d MegaBytes libres. No se puede continuar!",megaslibres);
				 else
					 sprintf(tmp,"Atencion: solo se disponen de %d MegaBytes libres!",megaslibres);
				 aviso(0,tmp);
				 if ((modo_minmegaslibres%100) == 1)
				 {
					 p = -1;
			         fin_sig();
                     Terfinal();
                     exit(2);
				 }
			 }
			 else
			 {
				 sprintf(tmp,"Espacio libre: %d MegaBytes",megaslibres);
			 }
			 guarda_incidencia(0,tmp);
			 
		 }

         return(p);
}

int login_usuario(int p,char *usuario,char *password,char *cambio)
{
   int i;
#ifdef RWXARXA
   if (en_conexion_terminal)
   {
      char tmp[50];	  	  
      memcpy(tmp,usuario,11);
      memcpy(tmp+11,password,11);
	  if (cambio && *cambio)
	  {
		  memcpy(tmp+22,cambio,11);
		  envia_control_a_server(tmp,33,70);
	  }
	  else
	  {
		  envia_control_a_server(tmp,22,70);
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

int opcion_usuario(int p)
{
   static char static_user_msg[50] = "";
   int con1;
#ifdef RWXARXA
   if (en_conexion_terminal)
   {
      envia_control_a_server("",0,71);
      if (modo_terminal() != 4)
         return 0;
      return 1;
   }
#endif
#ifndef DSMINIBASICO

   f_util   = entra_util;

   cls();

   if (DespachadorExterno)
   {
	  sprintf(static_user_msg,"%s(%d)",rw_user,subnumero_de_usuario+1);
      (*DespachadorExterno)((int)DESPACHO_MENSAJE_CABECERA,3,static_user_msg);
   }

   con1 = 0;
   mpfi[48] = NULL;
   if (p)
   {		     
      strcpy(pro,":agitool/ds_confi.tab");             
      solouno = 1;
   }
   else
   {
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
   
   if (solouno)
   {		  
	  nommenu[0] = 0;
	  leemenu(nmen0);
	  strcpy(titulo,nommenu);
	  libera();
      opcion_ej(pro,"");
   }
   else
   {	   
      con_acceso_directo = 1;
	  pulsatecla();
      while (!llama_menu(nmen0,pro,con1)) 
      {	
         opcion_ej(pro,"");
         con1 = 1;
         inicio_dir();		 
      }	   
   }
      
   if (solouno || p || Acceso_directo)
       return(0);                
#endif
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
#ifndef DSMINIBASICO
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

#endif
   return 0;
}


int finaliza_systema()
{
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

#ifdef FGVWIN
   if (PortScanner)
   {
	   PortClose(hPortScanner);
	   hPortScanner = INVALID_HANDLE_VALUE;
   }
#endif
   
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

void RetroDespachador(void *puntero)
{
int opcion;
if (en_conexion_terminal)
   {
   envia_retro_al_server(puntero);   
   return;
   }
#ifndef DSMINIBASICO
memcpy(&opcion,puntero,sizeof(int));
if (opcion >= 0 && opcion < 512 && RetroFuncion[opcion])
   {
   char *p = (char *)puntero;
   p += sizeof(int);
   ( *RetroFuncion[ opcion ] )(p,RetroPar[opcion],opcion);
   }
#endif
}

#ifdef RWSERVER


main(argc,argv)
int argc;
char *argv[];
{
   int p,r,waitfor,len;
   char tmp[1024];


   p = inicializa_systema(argc,argv);
  
   if (en_conexion_externa)
   {
	  /**** modo_portal -> pedir comprobacion de cookie ****/
	   /* enviar señal al cliente de que estamos listos para la siguiente fase */
	   if (_modo_portal)	  
		   envia_comando_a_terminal("",0,202);
	   else      
		   envia_comando_a_terminal("",0,200);
      do {
         /* procesador de directivas primarias del cliente */
         while(en_conexion_externa && (len=leecontrol_en_terminal(tmp,-1,&waitfor)) < 0);
         if (!en_conexion_externa)
            break;
         switch(waitfor)
         {
         case 70:
			 {
				 char *cambio;
				 char *pass = NULL;
			     if (len == 33)
					 cambio = tmp+22;
				 else
					 cambio = NULL;
				 
				 /*if (!siempre_password)*/
					 pass = tmp+11;
				
				 if ((r = login_usuario(p,tmp,pass,cambio)) == 1)
				    envia_comando_a_terminal("",0,200);
				 else {					
					envia_comando_a_terminal("",0,201+r);
				 }
			}
            break;
         case 71:
            DespachadorExterno = llama_a_despachador;       
            if (opcion_usuario(p))
               envia_comando_a_terminal("",0,200);
            else
               envia_comando_a_terminal("",0,201);             
            DespachadorExterno = NULL;
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
		 case 75:
            finaliza_systema();
            return(0);			
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
		    DespachadorExterno = despachador_vacio;			
            if (login_usuario(p,"batch","batch",NULL) == 1)
			{				
                opcion_usuario(p);
			}
       }
   }
   
   finaliza_systema();
   return(0);
}

#endif
