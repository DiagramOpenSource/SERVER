

/* substituye a la rtermin , rtermi2 y wio */
int fgv_internal_build = 221; // 12,14,29,30 no validos no deben existir por ahi 

#include <fgvsys.h>

#include <stdio.h>
#ifdef FGVWIN
#include <windows.h>
#include <windowsx.h>
#endif

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>


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


#include <errno.h>
#include <malloc.h>
#include <memory.h>
#include <signal.h>
#include <time.h>
#include <rwmalloc.h>
#define _R_TERMIN
#include <rtermin.h>
#include <teclas.h>
#define _NUEVODEF
#include <rdef.h>
#include <rmante.h>

#include <rwmenus.h>

#include <fgvmnexp.h>


#ifndef IDOK
#define IDOK                1
#endif
#ifndef IDCANCEL
#define IDCANCEL            2
#endif 
#ifndef IDABORT
#define IDABORT             3
#endif 
#ifndef IDRETRY
#define IDRETRY             4
#endif 
#ifndef IDIGNORE
#define IDIGNORE            5
#endif 
#ifndef IDYES
#define IDYES               6
#endif 
#ifndef IDNO
#define IDNO                7
#endif 
#ifndef IDCLOSE
#define IDCLOSE         8
#endif 
#ifndef IDHELP
#define IDHELP          9
#endif 

extern int sinescape;

void aviso(int p,char *text);

void (*DespachadorExterno)() = NULL;

char *crea_control_texto(char *xxtmp,int tamxxtmp,int _qpi,int qpf,int area,int ref,int topelin,int cwid,int maxqc,char **texto,int flags,int maximo,int wid,int id,int sid);
char *crea_control_estatico(int lpi,int lpf,char *p,int n);

extern char *i_nombre();
extern int i_anchocarro;


#define SALVA_IDS  int _default_id  = default_id;int _default_sid = default_sid;int _default_wid = default_wid;
#define REPON_IDS  default_id  = _default_id;default_sid = _default_sid;default_wid = _default_wid;


char other_server[128] = "";
char server_request[128] = "";
char segundo_dir[128] = "";
int  server_id = 0;

int (*Eliminador_Diagram)(char *) = NULL;

short limitacion_especial = 0;
int   mimodo_debug = 1;

char BufferSerie[250] = "";

int en_conexion_terminal = 0;
int leecar_terminal_puro = 0;

int remoto_es_sistema_grafico = 0;
#ifdef RWSERVER
int leecar_server_puro = 0;
int en_conexion_externa = 0;
int en_conexion_externa_stop = 0;
int remoto_ver1 = -1;
int remoto_ver2 = -1;
unsigned char remoto_verletra  = 0;
int remoto_que_sistema_es = -1;
char remoto_dirbass[MAXPATH];
#endif

extern unsigned char nombredev[81];
extern char numerofax[300];

int oem_a_ansi(unsigned char *c);
int ansi_a_oem(unsigned char *c);

char *rw_modulo = NULL;

int fgv_activa_impredll = 1;
int fgv_activa_dragdrop = 1;
int fgv_activa_progdll = 1;

void pito(int codigo);

int data_hiden_indicator = 0;
int identidad_cliente = 0;

int pendiente_de_instalar = 0;

extern int sin_replicante;

extern int auto_completar;

static int no_cola_timer = 0;

char R_midir[256] = "";


extern int (*memo_pregu_imp)() = NULL;
extern int (*memo_printa_linea)() = NULL;
extern int (*memo_finimp)() =    NULL;

short _fgv_version = 6;
short i3 = 11;
short _salida2 = 0;

int hay_mensaje_signal = 0;

int modo_desatendido = 0;

int quien_soy1 = 0; /* nume_code    */
int quien_soy2 = 0; /* nume_mochila */

int se_ha_requerido_usuario = -1;

extern int noboton_usuarios;

int *sitecla_ = NULL;

int modo_de_validacion = CRETURN;

long _programada_alarma = 0L;

short _si_escrola = 0;

int data_fecha_conmutar = 0;

int std_size = 2480;

int win_fondo_neutro = 0;

int que_sistema_es = QUE_SISTEMA_SOY;

int espuro10 = 0; 

#ifdef FGVJAVA
int es_sistema_grafico = 129;
#else
#ifdef FGVWIN
int es_sistema_grafico = 1;
#else
int es_sistema_grafico = 0;
#endif
#endif

int resta_dias_log = 30;

int terminal_internal_build = 0;
int server_internal_build = 0;
int server_fgvid = 0;


/* ojo acordarse de hacerlo coincidir!!!*/
int rtme_version1 = 10;
int rtme_version2 = 9;
unsigned char rtme_versionletra = 86;
int numero_build = 54;
char rw_version[256] = "RUNTIME 10.09.086-54                                                           ";

char fichdir[MAXPATH] = "fich";
char fichdirbarra[MAXPATH] = "fich/";

char dirbin[MAXPATH];
char dirdef[MAXPATH];
char dirpan[MAXPATH];
char dirfic[MAXPATH];
char nomfdir[MAXPATH];
char dirficb[MAXPATH];
char dirbass[MAXPATH];
char dirbase[MAXPATH];
char nomapli[25];
char nmen0[551];
char par1[551];
char par2[551];
char modulo[9] = {0,};
char psx[11] = {125,126,125,126,125,126,125,126,125,126,0};
char original_rw_dirbase[MAXPATH] = "";

char no_instala[30];
int no_instala_enbucla = 0;

char rw_user[11];
int user_who = -1;
int subnumero_de_usuario = 0;
int super_usuario = 0;

int modo_otro_euro = 0;
int fgv_ver_euro = 0;
int (*asigna_euro)(char *nom,double *numd) = NULL;

char buffer_elcodigo[256];

extern int rwagi_infoteclado;

int (*f_ayuda)() = NULL;
char clave_help[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0}; /* clave del help pos 31 linea */

char referencia_help[128] = {0,};

int (*f_util)() = NULL;

int (*avisar)() = NULL;

void (*f_disparo)() = NULL;


static int _nleeteclaids = 0;
static int _leeteclaids[128];


int tracer_int = 0; /* tecla de interrupcion pulsada */
int logger_int = 0; /* interrucion activacion - desactivacion del logger */
/* modulo utilizables */


unsigned short local_term[52] = {0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,
0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,
0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff};
/* versiones de los modulos */
unsigned short local_term_[52] = {0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,
0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,
0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff};
unsigned short local_acce_[52] = {0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,
0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,
0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff};

int time_d_tiempo1 = 180;
int time_d_tiempo2 = 3600;


char _fispool[MAXPATH];
char _ejspool[MAXPATH];

char *dato_entra = NULL;
int dato_entra_len = 0;

/* Funcion para ejecutar una interrrucion interna simulada */
int (*general_int)() = NULL;
int (*int_aviso)() = NULL;
int (*timer_especial_int)() = NULL;

int f_noallowed = 0;

int fich_tecla = 0;

int PCcolor = -1;
int PFcolor = -1;

int _PCcolor = -1;
int _PFcolor = -1;

int wdesplegamenu = 1;
int nombre_de_listado = 0;
int duplicarusuario = 1;


int wmenu_en_alfa = 0;
int linea_25 = 0;
int rentra_autosal = 0;
void (*rentra_pos)() = NULL;

/*
int rentra_atrib = 'R';
int rentra_sal_atrib = 'I';
int rentra_cc = 0;
int rentra_cf = 0;
*/

int rentra_atrib = ' ';
int rentra_sal_atrib = ' ';
int rentra_cc = 0;
int rentra_cf = 0;


int rentra_edicion = 0; /* funciones especiales de edicion de textos */
int rentra_sal_cc = 0;
int rentra_sal_cf = 0;


int rentra_offqp = 0;

  /* 0 = modo normal de entrada de un campo */
/* -1 = entrada normal pero salida dejando colro por defecto de entrada */
/* -2 o > 0 se supone pintado bien y se sale dejando
color por defecto de entrada */
/* ( !0 es para modos de edicion de texto ) */
int rentra_offini= 0;  /* para cuando la edicion es de lineas largas */
int rentra_insert = 0;

int especial_tecla_linea = 0;

int redita_atrib = ' ';
int redita_cc = 7;
int redita_cf = 0;
int default_redita_atrib = ' ';
int default_redita_cc    = 7;
int default_redita_cf    = 0;

long redita_linea = 0L;
int redita_col = 0;
int repinta_texto = 0;


int atento_imp = 0; /* flag que debe ser activado al abrir _imp si la impre
es esclava */


FILE *_imp = NULL;

int LEERTERMINAL = 0;
int DESPROTEGIDO = 0;

int Pcursor = 0;
int _M_Cursor = 101;
int _ID_Cursor = -1;
int _M_EXTRA_AVANCE = 0;

int TestigoRaton = 0;

int eqj = 0;

int hay_raton = 0;
int Prefresco = CIERTO;
unsigned int Patrib = ' ';
char Pgrafi = ' ';
VENT *stdpan = NULL;

int buf_tecla[500] = {-1,0,};

int os_subtipo = 1;

long NumeroDeSerie = 0L;

int pulsa_en_espacio = -1;

char *rw_usuario = NULL;
char *cabeza_especial = (char *)0;
int  *rw_extranumuser = NULL;

/************************************* WINDOWS **************************************************/
#ifdef FGVWIN
int SinColaMensajes = 0;
HACCEL agi_acelerador = NULL;
int un_fondo_especial = 0;
HBITMAP icono_opcion[100] = {NULL,};
DWORD icono_pos[100] = {0L,};
#endif
/************************************************************************************************/
char *agi_aplicacion = NULL;
char *agi_empresa = NULL;
char *agi_codi_empre = NULL;
int orig_wdesplegamenu = 0;
#ifdef FGVDOS
BOOL fIsTNT = FALSE;
HANDLE myconsola; /*!!!*/
#endif
/************************************ OJO! ******************************************************/
/* defecto de titulos de teclas */
extern struct tit_tecla {
   char *r;
   char *t;
   char *d;
} _titu_tecla[_NUMTECLA-NUMESPEC] = {
   NULL        ,"Nueva linea"       ,"Validacion del campo",
      NULL        ,"Return"            ,"Validacion del campo",
      NULL        ,"Espacio atras"     ,"Borra hacia atras",
      NULL        ,"Tab adelante"      ,"Salta 5 espacios adelante",
      NULL        ,"Tab atras"         ,"Salta 5 espacios atras",
      NULL        ,"Sale y valida(Esc)","Sale en cualquier punto validando los datos",
      NULL        ,"Abandonar (^C)"    ,"Sale anulando lo entrado",
      NULL        ,"Arriba"            ,"Desplazamiento vertical arriba o campo anterior",
      NULL        ,"Abajo"             ,"Desplazamiento vertical abajo o campo siguiente",
      NULL        ,"Izquierda"         ,"Desplazamiento del cursor a la izquierda",
      NULL        ,"Derecha"           ,"Desplazamiento del cursor a la derecha",
      NULL        ,"Posic. Inicio"     ,"Desplazamiento del cursor a la primera posicion",
      NULL        ,"Valor inicial"     ,"Presentar de nuevo el valor inicial del campo",
      NULL        ,"Pagina arriba"     ,"Cambiar a la anterior pantalla",
      NULL        ,"Pagina abajo "     ,"Cambiar a la siguiente pantalla",
      NULL        ,"Posic. final"      ,"Desplazamiento del cursor al final del dato",
      NULL        ,"Borrar hasta"      ,"Borrar desde la posicion del cursor al final",
      NULL        ,"Modo insert"       ,"Activar-Desactivar el modo de insercion",
      NULL        ,"Borrar carac"      ,"Borra el caracter de la posicion del cursor",
      NULL        ,"Consulta"          ,"Permite acceder a las fichas de otro fichero",
      NULL        ,"Reponer anterior"  ,"Presenta el valor anterior del dato",
      NULL        ,"Ayuda"             ,"Invoca el sistema de ayudas",
      NULL        ,"Funcion  1"        ,NULL,
      NULL        ,"Funcion  2"        ,NULL,
      NULL        ,"Funcion  3"        ,NULL,
      NULL        ,"Funcion  4"        ,NULL,
      NULL        ,"Funcion  5"        ,NULL,
      NULL        ,"Funcion  6"        ,NULL,
      NULL        ,"Funcion  7"        ,NULL,
      NULL        ,"Funcion  8"        ,NULL,
      NULL        ,"Funcion  9"        ,NULL,
      NULL        ,"Funcion 10"        ,NULL,
      NULL        ,"Funcion 11"        ,NULL,
      NULL        ,"Funcion 12"        ,NULL,
      NULL        ,"Funcion 13"        ,NULL,
      NULL        ,"Funcion 14"        ,NULL,
      NULL        ,"Funcion 15"        ,NULL,
      NULL        ,"Funcion 16"        ,NULL,
      NULL        ,"Funcion 17"        ,NULL,
      NULL        ,"Funcion 18"        ,NULL,
      NULL        ,"Funcion 19"        ,NULL,
      NULL        ,"Funcion 20"        ,NULL,
};
struct tit_tecla *titu_tecla = _titu_tecla;
/************************************************************************************************/


static char C_graficolinea[128] = 
{
   /*CODIGO INTERNO*/ /* Gráfico */
   /* 128*/                  0,
   /* 129*/                  0,
   /* 130*/                  0,
   /* 131*/                  0,
   /* 132*/                  0,
   /* 133*/                  0,
   /* 134*/                  0,
   /* 135*/                  0,
   /* 136*/                  0,
   /* 137*/                  0,
   /* 138*/                  0,
   /* 139*/                  0,
   /* 140*/                  0,
   /* 141*/                  0,
   /* 142*/                  0,
   /* 143*/                  0,
   /* 144*/                  0,
   /* 145*/                  0,
   /* 146*/                  0,
   /* 147*/                  0,
   /* 148*/                  0,
   /* 149*/                  0,
   /* 150*/                  0,
   /* 151*/                  0,
   /* 152*/                  0,
   /* 153*/                  0,
   /* 154*/                  0,
   /* 155*/                  0,
   /* 156*/                  0,
   /* 157*/                  0,
   /* 158*/                  0,
   /* 159*/                  0,
   /* 160*/                  0,
   /* 161*/                  0,
   /* 162*/                  0,
   /* 163*/                  0,
   /* 164*/                  0,
   /* 165*/                  0,
   /* 166*/                  0,
   /* 167*/                  0,
   /* 168*/                  0,
   /* 169*/                  0,
   /* 170*/                  0,
   /* 171*/                  0,
   /* 172*/                  0,
   /* 173*/                  0,
   /* 174*/                  0,
   /* 175*/                  0,
   /* 176*/                  1,
   /* 177*/                  1,
   /* 178*/                  1,
   /* 179*/                  3,
   /* 180*/                  2,
   /* 181*/                  2,
   /* 182*/                  2,
   /* 183*/                  2,
   /* 184*/                  2,
   /* 185*/                  2,
   /* 186*/                  3,
   /* 187*/                  2,
   /* 188*/                  2,
   /* 189*/                  2,
   /* 190*/                  2,
   /* 191*/                  2,
   /* 192*/                  2,
   /* 193*/                  2,
   /* 194*/                  2,
   /* 195*/                  2,
   /* 196*/                  3,
   /* 197*/                  1,
   /* 198*/                  2,
   /* 199*/                  2,
   /* 200*/                  2,
   /* 201*/                  2,
   /* 202*/                  2,
   /* 203*/                  2,
   /* 204*/                  2,
   /* 205*/                  3,
   /* 206*/                  1,
   /* 207*/                  2,
   /* 208*/                  2,
   /* 209*/                  2,
   /* 210*/                  2,
   /* 211*/                  2,
   /* 212*/                  2,
   /* 213*/                  2,
   /* 214*/                  2,
   /* 215*/                  1,
   /* 216*/                  1,
   /* 217*/                  2,
   /* 218*/                  2,
   /* 219*/                  1,
   /* 220*/                  1,
   /* 221*/                  1,
   /* 222*/                  1,
   /* 223*/                  1,
   /* 224*/                  0,
   /* 225*/                  0,
   /* 226*/                  0,
/* 227*/                  0,
/* 228*/                  0,
/* 229*/                  0,
/* 230*/                  0,
/* 231*/                  0,
/* 232*/                  0,
/* 233*/                  0,
/* 234*/                  0,
/* 235*/                  0,
/* 236*/                  0,
/* 237*/                  0,
/* 238*/                  0,
/* 239*/                  0,
/* 240*/                  0,
/* 241*/                  0,
/* 242*/                  0,
/* 243*/                  0,
/* 244*/                  0,
/* 245*/                  0,
/* 246*/                  0,
/* 247*/                  0,
/* 248*/                  0,
/* 249*/                  0,
/* 250*/                  0,
/* 251*/                  0,
/* 252*/                  0,
/* 253*/                  0,
/* 254*/                  0,
/* 255*/                  0
    };



/** NUEVO !!! **/
int default_id  = -1;
int default_sid = -1;
int default_wid = -1;

int default_wcid = -1;
int default_ctipo = -1;
int default_cid  = -1;
int default_refcid  = -1;


int default_sid_padre = 0;
/***************/

static char _rw_dirbase[MAXPATH] = DEFAULTDIRBASE;

int rw_setdirectorio(char *nom);

char *rw_directorio(char *nom)
{
  char *pro;
  static short flag = 0;
  static char mdbase[81];
  if (!nom)
  {
     flag = 1;
     return(NULL);
  }
  if (!flag)
  {   
     pro=getenv("DIRBASE");
     if (pro)
     {
        rw_setdirectorio(pro);
     }
     else
     {
        rw_setdirectorio(DEFAULTDIRBASE);         
     }  
	 strcpy(original_rw_dirbase,_rw_dirbase);
  }
  sprintf(mdbase,"%s/%s",_rw_dirbase,nom);
  cambia_barra(mdbase);
  return(mdbase);
}

int rw_setdirectorio(char *nom)
{
  int i;
  strcpy(_rw_dirbase,nom);
  i = strlen(_rw_dirbase);
  if (i)
  {
     i--;
     if (_rw_dirbase[i] == '/' || _rw_dirbase[i] == '\\')
        _rw_dirbase[i] = 0;
  }
  cambia_barra(_rw_dirbase);
  strminus(_rw_dirbase);
  rw_directorio(NULL);
  return(0);
}

/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/
/******************************************************************************************/

void Terinici()
{
inicio_fgv();
if (getenv("AUTH_DIAGRAM"))
   strcpy(other_server,getenv("AUTH_DIAGRAM"));
}

void Terfinal()
{
fin_fgv();
}

int PonLogo()
{
    return 0;
}

void init_bmp_base(BMP_BASE_PAR *pPar)
{
}

void fin_bmp_base()
{
}


int agi_cola_timer()
{    
    int tecla = 807;

    if (no_cola_timer) 
        return tecla;

    if (DespachadorExterno)
    {
		do
		{
			 (*DespachadorExterno)((int)DESPACHO_PROCESAMENSAJES,2,&tecla);			 
			 if (tecla == TECLA_PING)
			 {
				 /* determinar si hay un mensaje */
				 char tmp[256];
				 int fdm;
				 if (!R_midir[0])
				 {
					strcpy(R_midir,rw_directorio("dev/reg/"));
				 }

				 sprintf(tmp,"%s%ld.msg",R_midir,getpid());
				 fdm = rw_open_(tmp,O_RDONLY | O_BINARY);
				 if (fdm > -1)
				 {
					 rw_close_(fdm);
					 hay_mensaje_signal = 1;
				 }

				 if (hay_mensaje_signal)
				 {
					 pito(40);
					 hay_mensaje_signal = 0;
		#ifdef RWXARXA
					if (en_conexion_terminal)
					{
					   envia_control_a_server("",0,63);
					   modo_terminal();
					   continue;
					}
					else
		#endif
					{
					   control_de_usuarios();
					   continue;
					}
				 }
				 continue;
			 }
			 else
				 break;
		} while(-1);
    }
    return tecla;
}

int pulsatecla()
{
    int tecla = 807;
    char buffer[ sizeof(int) * 10 ];
    if (DespachadorExterno)
    {
		do
		{
			memset(buffer,0,sizeof(buffer));
			(*DespachadorExterno)((int)DESPACHO_PROCESAMENSAJES,1,buffer);
			memcpy(&tecla,buffer,sizeof(int));
			 if (tecla == TECLA_PING)
			 {
				 /* determinar si hay un mensaje */
				 char tmp[256];
				 int fdm;
				 if (!R_midir[0])
				 {
					strcpy(R_midir,rw_directorio("dev/reg/"));
				 }
				 sprintf(tmp,"%s%ld.msg",R_midir,getpid());
				 fdm = rw_open_(tmp,O_RDONLY | O_BINARY);
				 if (fdm > -1)
				 {
					 rw_close_(fdm);
					 hay_mensaje_signal = 1;
				 }

				 if (hay_mensaje_signal)
				 {
					 pito(41);
					 hay_mensaje_signal = 0;
		#ifdef RWXARXA
					if (en_conexion_terminal)
					{
					   envia_control_a_server("",0,63);
					   modo_terminal();
					   continue;
					}
					else
		#endif
					{
					   control_de_usuarios();
					   continue;
					}
				 }
				 continue;
			 }
			 else
				 break;
		} while(-1);
    }
    return tecla;
}

int leecar_foco(int modo_foco,int d_id,int d_sid,int d_ctipo,int d_cid,int d_ref,int cwid,char **_destino,char *codigocampo)
{

 {
    int tecla = saca_ptec();
    int wid,id,sid,wcid,ctipo,cid,refcid;
    int oldw = 0;
	int ant_oldw = 0;
    char _buffer[ sizeof(int) * 10 ];    
	char *buffer = NULL;
	int maxdata = 0;
	char *destino = NULL;
	int recogido = 0;

	if (_destino)
		destino = *_destino;	

	memset(_buffer,0,sizeof(_buffer));

	if (modo_desatendido && tecla == -1) {
		tecla = modo_desatendido;
	}

	if (tecla != -1) 
	{
		goto salida_leecarfoco;
	}

    if (DespachadorExterno)
    {
       char *p;

	   if (destino && cwid != -1)
	   {
		   maxdata = *((int *)destino);
		   buffer = malloc((sizeof(int)*10) + maxdata+256);
	   }
	   else
		   buffer = _buffer;


       do
       {

	   
       if (cwid != -1)
       {
          (*DespachadorExterno)((int)DESPACHO_DIF_FOCOCONTROL,cwid,NULL);
       }
	   

	   do
	   {
           tecla = saca_ptec();
		   if (tecla != -1)
		   {
			  goto salida_leecarfoco;
		   }		   

		   SetAutoCompletar(codigocampo,cwid);

		   se_ha_requerido_usuario = 2;
		   if (destino && cwid != -1)
		   {

			   memset(buffer,0,(sizeof(int)*10) + maxdata);
		       memcpy(buffer,destino,sizeof(int)*2);
		       memcpy(buffer+sizeof(int)*2,&cwid,sizeof(int));
		       memcpy(buffer+sizeof(int)*10,destino,maxdata);

			   if (DespachadorExterno)
					(*DespachadorExterno)((int)DESPACHO_DATA_PROCESAMENSAJES,0,buffer);
			   recogido = 1;
		   }
		   else
		   {			   
			   if (DespachadorExterno)
				   (*DespachadorExterno)((int)DESPACHO_PROCESAMENSAJES,0,buffer);
			   recogido = 0;
		   }
       

		   p = buffer;
		   memcpy(&tecla,p,sizeof(int));
		   p += sizeof(int);
		   memcpy(&wid,p,sizeof(int));
		   p += sizeof(int);
		   memcpy(&id,p,sizeof(int));
		   p += sizeof(int);
		   memcpy(&sid,p,sizeof(int));
		   p += sizeof(int);
		   memcpy(&wcid,p,sizeof(int));
		   p += sizeof(int);
		   memcpy(&ctipo,p,sizeof(int));
		   p += sizeof(int);
		   memcpy(&cid,p,sizeof(int));
		   p += sizeof(int);
		   memcpy(&refcid,p,sizeof(int));
		   p += sizeof(int);
		   memcpy(&oldw,p,sizeof(int));
		   p += sizeof(int);

           if (tecla == TECLA_HINT)
		   {
			   if (auto_completar && codigocampo)
			   {				   
				   char resultado[10240];
				   if (recogido)
				   {					   
					   if (ObtenAutoCompletar(codigocampo,buffer+sizeof(int)*12,resultado+5) >= 0)
					   {
						   memcpy(resultado,"DATA:",5);
						   quitab(resultado);
						   if (DespachadorExterno)
							(*DespachadorExterno)((int)DESPACHO_ESTADOCONTROL,cwid,(void *)resultado);
					   }
				   }				   
			   }
			   continue;
		   }


         if (tecla == TECLA_PING)
		 {
			 /* determinar si hay un mensaje */
			 char tmp[256];
			 int fdm;

			 if (!R_midir[0])
			 {
				strcpy(R_midir,rw_directorio("dev/reg/"));
			 }

			 sprintf(tmp,"%s%ld.msg",R_midir,getpid());
			 fdm = rw_open_(tmp,O_RDONLY | O_BINARY);
			 if (fdm > -1)
			 {
				 rw_close_(fdm);
				 hay_mensaje_signal = 1;
			 }

			 if (hay_mensaje_signal)
			 {
				 pito(42);
				 hay_mensaje_signal = 0;
	#ifdef RWXARXA
				if (en_conexion_terminal)
				{
				   envia_control_a_server("",0,63);
				   modo_terminal();
				   continue;
				}
				else
	#endif
				{
				   control_de_usuarios();
				   continue;
				}
			 }
			 continue;
         }

		 break;

	   } while(-1);

       
       /* ojo igual hace falta proceso de activacion */

         if (tecla == 840)
         {
             tracer_int = 1;
         }

         if (tecla == PIDO_EMPRESA)
		 {
             tecla = 831;
         }

         if (tecla == PIDO_USUARIOS)
		 {
#ifdef RWXARXA
			if (en_conexion_terminal)
			{
               envia_control_a_server("",0,63);
               modo_terminal();
               continue;
			}
		    else
#endif
			{
			   control_de_usuarios();
			   continue;
			}
         }

         if (tecla == PIDO_TRASMISION)
		 {
#ifdef RWXARXA
			if (en_conexion_terminal)
			{
               envia_control_a_server("",0,61);
               modo_terminal();
               continue;
			}
		    else
#endif
			{
			   continue;
			}
         }  
		 
#ifdef RWXARXA
 		 if (tecla == PIDO_SUPERBLOQUEO || tecla == PIDO_SUPERDESBLOQUEO ||
			 tecla == PIDO_SUPERCAMBIO  || tecla == PIDO_SUPERTODO || tecla == PIDO_SUPERDESTODO)
		 {
			if (!en_conexion_terminal)
			{
				gestiona_superusuario((tecla - PIDO_SUPERBLOQUEO) + 1);
				continue;
			}
			else
			{
			   char tmp[2];
			   tmp[1] = 0;
			   tmp[0] = (tecla - PIDO_SUPERBLOQUEO) + 1;
               envia_control_a_server(tmp,2,166);
               modo_terminal();
               continue;
			}
		 }
		 if (tecla == TECLA_SUPERUSUARIO)
		 {
			if (!en_conexion_terminal)
			{
				sw_superusuario();
				continue;
			}
			else
			{
               envia_control_a_server("",0,164);
               modo_terminal();
               continue;
			}
		 }
 		 if (tecla == PIDO_VEREURO)
		 {
			if (!en_conexion_terminal)
			{
				sw_euro();
				continue;
			}
			else
			{
               envia_control_a_server("",0,165);
               modo_terminal();
               continue;
			}
		 }
#else
 		 if (tecla == PIDO_SUPERBLOQUEO || tecla == PIDO_SUPERDESBLOQUEO  ||
			 tecla == PIDO_SUPERCAMBIO || tecla == PIDO_SUPERTODO || tecla == PIDO_SUPERDESTODO)
		 {
			gestiona_superusuario((tecla - PIDO_SUPERBLOQUEO) + 1);
			continue;
		 }
 		 if (tecla == TECLA_SUPERUSUARIO)
		 {
				sw_superusuario();
				continue;			
		 }
 		 if (tecla == PIDO_VEREURO)
		 {
				sw_euro();
				continue;
		 }
#endif



       if (tecla == AYUDA)
       {
            llama_fun_int(f_ayuda,clave_help);
            continue;
       }

       if (tecla == 999)
       {
           if (!modo_foco)
           {
               if (cwid == -1 && oldw) {
				   if (oldw != ant_oldw) {
					  ant_oldw = oldw;
					  if (DespachadorExterno)
						(*DespachadorExterno)((int)DESPACHO_FOCOWINDOW,oldw,NULL);
				   }
			   }
               continue;
           }
       }

 		 if (tecla == ID_BUSCON)
		 {
#ifdef RWXARXA
			if (!en_conexion_terminal)
			{
				super_buscon(NULL);
				continue;
			}
			else
			{
               envia_control_a_server("",0,167);
               modo_terminal();
               continue;
			}
#endif
		 }


 		 if (tecla == ID_ACTIVAR_ERP)
		 {
#ifdef RWXARXA
			if (!en_conexion_terminal)
#endif
			{
				llama_modulo_erp();
				continue;
			}
#ifdef RWXARXA
			else
			{
               envia_control_a_server("",0,168);
               modo_terminal();
               continue;
			}
#endif
		 }


 		 if (tecla == ID_ACTIVAR_EVT || tecla == ID_ACTIVAR_EVTPBX || tecla == ID_ACTIVAR_EVTCRM)
		 {
#ifdef RWXARXA
			if (!en_conexion_terminal)
#endif
			{
				extern int llama_evento_desc(void *nulo);
				llama_fun_int(llama_evento_desc,(void *)tecla);
				continue;
			}
#ifdef RWXARXA
			else
			{
               envia_control_a_server(&tecla,sizeof(int),169);
               modo_terminal();
               continue;
			}
#endif
		 }


      if ((tecla >= DISPARO_CONTROL && tecla < MAX_DISPARO_CONTROL) || (tecla >= PREDISPARO_CONTROL && tecla < PREMAX_DISPARO_CONTROL))
      {
#ifdef RWXARXA
         if (en_conexion_terminal)
         {
            extern void dispara_servidor();
            llama_fun_int(dispara_servidor,tecla);
            continue;
         }
#endif
         if (f_disparo)
         {	  
            llama_fun_int(f_disparo,tecla);
         }
         continue;
      }


       
       if (!wmenu && tecla >= MID_XPRIMERA && tecla < MID_XOPCION1)
       {
		  char tmp[512];
          tmp[0] = '|';
          tmp[1] = 0;
		  dame_opcion_de_menu(tecla - MID_XPRIMERA,tmp+1);
		  if (tmp[0])
		  {
             if (f_util)
             {
                  sin_replicante = default_sid;
                  llama_fun_int(f_util,tmp);                  
             }
		  }
          continue;
       }
       


       if (id == -1) break; /* bypass */
       /* de momento ignoramos */
       if (d_id != -1 && d_id != id)
       {
           if (cwid == -1 && oldw) {
			   if (DespachadorExterno)
				(*DespachadorExterno)((int)DESPACHO_FOCOWINDOW,oldw,NULL);
		   }
           continue;
       }
       if (d_sid != -1 && d_sid != sid)
       {
           if (cwid == -1 && oldw) {
			   if (DespachadorExterno)
				(*DespachadorExterno)((int)DESPACHO_FOCOWINDOW,oldw,NULL);
		   }
           continue;
       }
       if (d_ctipo != -1 && d_ctipo != ctipo)
       {
           if (cwid == -1 && oldw) {
			   if (DespachadorExterno)
				(*DespachadorExterno)((int)DESPACHO_FOCOWINDOW,oldw,NULL);
		   }
           continue;
       }
       if (d_cid != -1 && d_cid != cid)
       {
           if (cwid == -1 && oldw) {
			   if (DespachadorExterno)
				(*DespachadorExterno)((int)DESPACHO_FOCOWINDOW,oldw,NULL);
		   }
           continue;
       }
	   if (d_ref != -1 && d_ref != refcid)
	   {
		   if (refcid == 0 && (tecla == 801 || tecla == 806 || tecla == 807))
		   {
			   refcid = d_ref;
		   }
	   }
       if (d_ref != -1 && d_ref != refcid)
       {		   
           if (cwid == -1 && oldw) {
			   if (DespachadorExterno)
				(*DespachadorExterno)((int)DESPACHO_FOCOWINDOW,oldw,NULL);
		   }
           continue;
       }
       
       if (default_id == id)
       {
          if (sid != -1 && (sid > 0 || ctipo != 3))
          {
              default_sid = sid;
              default_wid = wid;
              if (cid != -1)
              {
                  default_wcid = wcid;
                  default_ctipo = ctipo;
                  default_cid  = cid;
                  default_refcid = refcid;
              }
          }
       }
       
       break;
       
       } while(-1); /*default_id != id);*/

    }

salida_leecarfoco:
   if (!recogido)
   {
	   destino = NULL;
	   if (_destino)
		   *_destino = NULL;
   }
   if (buffer != _buffer)
   {
	   if (destino && cwid != -1)
	   {
		   memcpy(destino,buffer+sizeof(int)*10,maxdata);
	   }
	   if (buffer)
		free(buffer);
	   buffer = NULL;
   }
    return tecla;
 }
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


int leecar_modal()
{
    return leecar_foco(0,default_id,-1,-1,-1,-1,-1,NULL,NULL);
}

/*int leecar_foco(int modo_foco,int d_id,int d_sid,int d_ctipo,int d_cid,int d_ref)*/
int leecar_campo(FI_DEF *pfi,int campo,int faclin)
{
	/* campo puede ser -1 */
    return leecar_foco(1,pfi->f_id,-1,-1,-1,pfi->f_ipfi,-1,NULL,NULL);
}

int leecar()
{
return leecar_modal();
}

#ifdef FGVDOS
void TxtARect(int pi,int pf,RECT *r)
{
}
#endif

int stdpan_es_invisible()
{
   /*stdpan->visible = 0;*/
   return 0;
}


void RePon_StdPan(void *puntero)
{

}

void Coge_StdPan(void *puntero)
{

}


#ifdef FGVDOS
void CuadroTab(int pos,RECT *r)
{
}
#endif

void w_fin(VENT *w)
{
}

void w_pinta(VENT *w)
{
}

unsigned char codifica_semigrafico(unsigned char c)
{
          switch(c)
          {
          case 179: /* raya vertical */
          case 186:
              return 1;              
          case 196: /* raya horizontal */
          case 205:
              return 2;
              
          case 218: /* esquina i-a */
          case 201:
          case 214:
          case 213:
              return 3;
              
          case 194: /* interseccion c-a */
          case 203:
          case 210:
          case 209:
              return 4;
              
          case 191: /* esquina d-a */
          case 187:
          case 183:
          case 184:
              return 5;
              
          case 195: /* interseccion i-c */
          case 204:
          case 199:
          case 198:
              return 6;
              
          case 197: /* interseccion central */
          case 206:
          case 215:
          case 216:
              return 7;
              
          case 180: /* interseccion c-d */
          case 185:
          case 182:
          case 181:
              return 8;
              
          case 192: /* esquina i-abajo */
          case 200:
          case 211:
          case 212:
              return 9;
              
          case 193: /* interseccion c-abajo */
          case 202:
          case 208:
          case 207:
              return 10;
              
          case 217: /* esquina d-abajo */
          case 188:
          case 189:
          case 190:
              return 11;
              
          default:
              break;
          }

    return 0;
}

int pprint(int pos,char *str)
{
  Pcursor = pos;
  if (DespachadorExterno)
  {      
     int i;
     unsigned char c;     
     char *p;
	 int npos = pos;
     int normal = 0;
     int semi = 0;
     char tmp[512];

	 if (espuro10) {
		 
		 for (i = 0;i < (strlen(str)+1);i++)
		 {			 
			 c = codifica_semigrafico((unsigned char)str[i]);
			 if (c || !str[i])
			 {
				 if (normal)
				 {                 
					 tmp[normal] = 0;                 
					 Pcursor = pos;					 
					 p = crea_control_estatico(npos,npos+strlen(tmp)-1,tmp,-1);
					 (*DespachadorExterno)((int)DESPACHO_DIF_CCONTROL,-1,p);
					 free(p);					 
				 }
				 normal = 0;
				 if (str[i])
				 {
				   if (!semi)
					   npos = pos+i;
				   tmp[semi] = c;
				   semi++;
				 }
			 }
         
			 if (!c || !str[i])
			 {
				 if (semi)
				 {                 
					 tmp[semi] = 0;
					 Pcursor = pos;
					 p = crea_control_estatico(npos,npos+strlen(tmp)-1,tmp,-1);
					 (*DespachadorExterno)((int)DESPACHO_CCONTROL,-1,p);
					 free(p);					 
				 }
				 if (str[i])
				 {
				     if (!normal)
						 npos = pos+i;
					 tmp[normal] = str[i];
					 normal++;             
				 }
				 semi = 0;
			 }
		 }
	 }
	 else {		 
		int wid = default_wid;
		int sid = default_sid;
		if (default_sid > 0) {
		   int ifi,ipa,pi,pf;
		   ifi = (default_sid%1000)-1;
		   ipa = default_sid/1000;
		   if (ifi >= 0 && ifi < nfi && ipa >= 0 && ipa < v_mpfi(ifi)->f_npanta && default_id == v_mpfi(ifi)->f_id) {
			   int pi,pf;
			   pi = v_mpfi(ifi)->f_pantalla[ipa].p_posref;
			   pf = pi + (v_mpfi(ifi)->f_pantalla[ipa].p_posfin-101);
			   if ((pos/100) > (pf/100)) {
				   wid = -1;
				   sid = 0;
			   }
		   }
		}    

		 BorraDescendencia(wid,default_id,sid,-(pos),pos+strlen(str));

		 for (i = 0;i < (strlen(str)+1);i++) {			 
			 c = codifica_semigrafico((unsigned char)str[i]);
			 if (str[i] == ' ' || str[i] == 255 || c || !str[i])
			 {
				 if (normal)
				 {                 
					 tmp[normal] = 0;                 
					 Pcursor = pos;
					 p = crea_control_estatico(npos,npos+strlen(tmp)-1,tmp,-1);
					 (*DespachadorExterno)((int)DESPACHO_DIF_CCONTROL,-1,p);
					 free(p);					 
				 }
				 normal = 0;             
				 if (c)
				 {
				   if (!semi)
					   npos = pos+i;
				   tmp[semi] = c;
				   semi++;
				 }
			 }
         
			 if (!c || !str[i])
			 {
				 if (semi)
				 {                 
					 tmp[semi] = 0;
					 Pcursor = pos;
					 p = crea_control_estatico(npos,npos+strlen(tmp)-1,tmp,-1);
					 (*DespachadorExterno)((int)DESPACHO_CCONTROL,-1,p);
					 free(p);					 
				 }
				 if (str[i])
				 {
					 if (str[i] != ' ' && str[i] != 255) {
						 if (!normal)
							 npos = pos+i;
						 tmp[normal] = str[i];
						 normal++;             
					 }
				 }
				 semi = 0;
			 }

		 }
	 }
  }
  Pcursor = pos+strlen(str);

  return Pcursor;
}

char **realoca_multiplo(int n,int mul,char **p)
{
    if (!p)
    {
        return (char **)malloc(sizeof(char *) * (mul+2));
    }

    if (n &&  !(n%mul))
    {

        return (char **)realloc((char *)p,sizeof(char *) * ( (((n/mul)+1)*mul) +2));
    }

    return p;
}


char *w_copia_stdid(char *pt)
{
    memcpy(pt,(char *)&default_wid,sizeof(int));
    pt += sizeof(int);
    memcpy(pt,(char *)&default_id,sizeof(int));
    pt += sizeof(int);
    memcpy(pt,(char *)&default_sid,sizeof(int));
    pt += sizeof(int);

    return pt;
}

int w_tam_stdid()
{
    return(sizeof(int)*3);
}



char *crea_control_estatico(int lpi,int lpf,char *p,int n)
{
    char *px,*pt;
    int mas,j;
	int sid = 0;
	int wid = -1;

    if (p && n == -1) n = strlen(p);
    if (n < 0) n = 0;

	/*
	if (p && strstr(p,"Contrap.")) {
		mas = 0;
	}
	*/

    mas = w_tam_stdid() + sizeof(int)*7 + n + 1;

    px = malloc(mas);
    pt = px;

    memcpy(pt,&mas,sizeof(int));
    pt += sizeof(int);
    

	wid = default_wid;
	sid = default_sid;
    if (!p)
    {
        mas = 9; /* 'caja' propia */
    }
    else
    {
        mas = 1; /* Texto extatico */
		if (!espuro10 && default_sid > 0) {
		   int ifi,ipa;
		   ifi = (default_sid%1000)-1;
		   ipa = default_sid/1000;
		   if (ifi >= 0 && ifi < nfi && ipa >= 0 && ipa < v_mpfi(ifi)->f_npanta && default_id == v_mpfi(ifi)->f_id) {
			   int pi,pf;
			   pi = v_mpfi(ifi)->f_pantalla[ipa].p_posref;
			   pf = pi + (v_mpfi(ifi)->f_pantalla[ipa].p_posfin-101);
			   if ((lpi/100) > (pf/100)) {
				   wid = -1;
				   sid = 0;
			   }
		   }
		}    
    }

	/*pt = w_copia_stdid(pt);*/
    memcpy(pt,(char *)&wid,sizeof(int));
    pt += sizeof(int);
    memcpy(pt,(char *)&default_id,sizeof(int));
    pt += sizeof(int);
    memcpy(pt,(char *)&sid,sizeof(int));
    pt += sizeof(int);


    memcpy(pt,&mas,sizeof(int));
    pt += sizeof(int);
    
    memcpy(pt,&lpi,sizeof(int));
    pt += sizeof(int);
    
    memcpy(pt,&lpf,sizeof(int));
    pt += sizeof(int);

    j = (int)Patrib;
    memcpy(pt,&j,sizeof(int));
    pt += sizeof(int);

    memcpy(pt,&PCcolor,sizeof(int));
    pt += sizeof(int);

    memcpy(pt,&PFcolor,sizeof(int));
    pt += sizeof(int);
    if (p)
    {
        memcpy(pt,p,n);
        pt += n;
    }
    *pt = 0;
    pt++;
        
    return px;
}

char *crea_entrada(int wid,int id,int sid,char *qd,int maxqc,int qc,char *qi,char *qs,int qp,int wcid,int lcid,int ref,int modif,char *mensaje,char *punt,int vacio,int lineal,int pass)
{
    int mas,i,j;
    char *p,*pt;
    int tipo = 0;
    int lqi,lqs,lmensa;
    int decimal = 0,decmin,decmax;
    char *dqd;


    if (maxqc > 0)
    {
        if (pass)
            tipo = 10;
        else
            tipo=1;                   /* string */
        quitab(qd);
    }
    else 
    if (maxqc==-1) 
    {   
        tipo=2;                   /* fecha dd.mm.aaaa */   
        if (qc == 6) 
        {
           maxqc = 6;
        }
        else
        if (qc == 8) 
        {
           maxqc = 8;
        }
        else
        {
           maxqc = 10;
        }
        if (!vacio && maxqc != 6 && maxqc != 8 && strlen(qd) != maxqc)
        {  
           dqd = qd;
           minimo_fecha(&dqd,maxqc); /* fecha del sistema segun formato */
           if (dqd != qd)
           {
               strcpy(qd,dqd);
           }
        }
    }
    else 
    if (maxqc == 0)
    {
        tipo=3;
        i = strlen(qi);
        if (i > 1 && qi[i-2] == '.' && qi[i-1] == '*') 
        {
            decimal = -1;
        }
        else 
        {
            decmin = decimales(qi);
            decmax = decimales(qs);
            if (decmin < decmax)
                decimal = decmax;
            else
                decimal = decmin;            
        }
        maxqc = len_numerico(qi,qs);
        quita_blancos(qd);
		/*
        if (!vacio)
        {            
            poncoma(qd,qd);
        }
		*/
    }
    else 
    {
        /* aunque no es valido crear el control de todos modos */
        tipo=0;
        DebugBreak_();
    }
      
      
    if (tipo != 1 || !qc || qc > maxqc) qc = maxqc;

    lqi = strlen(qi) + 1;
    lqs = strlen(qs) + 1;
    
    lmensa = strlen(mensaje) + 1;

    i = MAXDEFECTO + 1;

    mas = sizeof(int)*10 + sizeof(int)*4+ sizeof(int)*11 + lqi+ lqs + i + lmensa + sizeof(int);

    if (punt)
        p = punt;
    else
        p = malloc(mas);

    pt = p;

    memcpy(pt,&mas,sizeof(int));
    pt += sizeof(int);

    memcpy(pt,(char *)&wid,sizeof(int));
    pt += sizeof(int);
    memcpy(pt,(char *)&id,sizeof(int));
    pt += sizeof(int);
    memcpy(pt,(char *)&sid,sizeof(int));
    pt += sizeof(int);      

    if (lineal)
    {
        mas = 11; /* campo especial(efimero) */
    }
    else
    {
        mas = 2; /* standard(permanente) campo!! (input) */
    }
    memcpy(pt,&mas,sizeof(int));
    pt += sizeof(int);

    memcpy(pt,&qp,sizeof(int));
    pt += sizeof(int);

    qp += (qc-1);
    memcpy(pt,&qp,sizeof(int));
    pt += sizeof(int);

    j = (int)Patrib;
    memcpy(pt,&j,sizeof(int));
    pt += sizeof(int);

    memcpy(pt,&PCcolor,sizeof(int));
    pt += sizeof(int);

    memcpy(pt,&PFcolor,sizeof(int));
    pt += sizeof(int);


    memcpy(pt,&wcid,sizeof(int));
    pt += sizeof(int);

    memcpy(pt,&lcid,sizeof(int));
    pt += sizeof(int);

    memcpy(pt,&maxqc,sizeof(int));
    pt += sizeof(int);

    memcpy(pt,&qc,sizeof(int));
    pt += sizeof(int);

    memcpy(pt,&tipo,sizeof(int));
    pt += sizeof(int);    

    memcpy(pt,&modif,sizeof(int));
    pt += sizeof(int);
    
    memcpy(pt,&decimal,sizeof(int));
    pt += sizeof(int);
    
    memcpy(pt,&lqi,sizeof(int));
    pt += sizeof(int);
    
    memcpy(pt,&lqs,sizeof(int));
    pt += sizeof(int);

    memcpy(pt,&ref,sizeof(int));
    pt += sizeof(int);

    memcpy(pt,&i,sizeof(int));
    pt += sizeof(int);

    strcpy(pt,qi);
    pt += lqi;

    strcpy(pt,qs);
    pt += lqs;

    strcpy(pt,qd);
    pt += i;

    memcpy(pt,&lmensa,sizeof(int));
    pt += sizeof(int);

    strcpy(pt,mensaje);
    pt += lmensa;

    return p;
}

int vacia_campo_texto(FI_DEF *pfi,int campo,int cwid,int grabamemo,char *codigocampo)
{
	int k;
    int topelin = 0;
    char *texto[300];
	int maxi = 0;
	char *p,*p1;
	int maxqc = pfi->f_campos[campo].c_qc;
	for (k = campo;k < pfi->f_ncampos;k++)
	{
		if (pfi->f_campos[k].c_qc != maxqc || (k > campo && !pfi->f_campos[k].c_mante.c_visual))
		{
			break;
		}
		texto[topelin] = pfi->f_buf + pfi->f_campos[k].c_offset;
		maxi += pfi->f_campos[k].c_qc;
		topelin++;
	}

	p = malloc(maxi+(sizeof(int) * 2 + 3)+topelin*2 + 5000);
	*((int *)p) = maxi;
	p[sizeof(int)*2] = 0;
	if (DespachadorExterno)
		(*DespachadorExterno)((int)DESPACHO_GETDATACONTROL,cwid,p);

	p1 = p+sizeof(int)*2;	

	
    if (auto_completar && grabamemo && codigocampo)
	{
		GrabaAutoCompletar(codigocampo,p1,p1,1);
	}	

	k = strlen(p1);
	if (k)
	{
		k--;
		if (p1[k] != 10 || p1[k] != 13)
		{
			quitan(p1);
			k = strlen(p1);			
			p1[k] = 13;
			k++;
			/*
			p1[k] = 10;
			k++;
			*/
			p1[k] = 0;
		}
	}

	
    if (!strstr(pfi->f_campos[campo].c_maximo ,"B"))
	{
		char *p2 = p1;
		int j = 0;
		int ll;
		int clen;

		for (k = 0;p1[k] && j < topelin;k++)
		{
			if (p1[k] == 13)
			{
				p1[k] = 0;
				if (p1[k+1] == 13 && p1[k+2] == 10)
				{
					k++;
					p1[k] = 0;
				}
				ll = strlen(p2);
				clen = pfi->f_campos[campo+j].c_qc;
				if (ll > clen)
				{
					ll = clen;
				}
				memcpy(texto[j],p2,ll);
				for (;ll < clen;ll++)
				{
					texto[j][ll] = ' ';
				}
				j++;
				p2 = p1 + k + 1;
				if (*p2 == 10)
				{
					p2++;
				}
			}
		}
		
		for (;j < topelin;j++)
		{
			clen = pfi->f_campos[campo+j].c_qc;
			for (ll = 0;ll < clen;ll++)
			{
				texto[j][ll] = ' ';
			}
		}
		
	}
	else
	{
		for (k = strlen(p1);k <maxi;k++)
		{
			p1[k] = 0;
		}
		memcpy(texto[0],p1,maxi);
	}

	free(p);
	return 0;
}

char *crea_control_edicion(FI_DEF *pfi,int campo,int wid,int id,int sid,int lineal,int posicional,int vacio,char *punt)
{
    int lcid;
    int k,i,j;
    int wcid;
    int qc,maxqc;
    int modif;
    int posi;
    char qd[MAXDEFECTO+1];
    char qs[256];
	char qi[128];
	char *pp;
    int pass = 0;
	int rela = 0;


    /* entra(el_flag*10000+qp,qc,pqi,pqs,qd,1+v_mpfi(ipfi)->f_campos[numcam-1].c_mante.c_qpl*10,qo); */
    maxqc = pfi->f_campos[campo].c_qc;
    qc    = pfi->f_campos[campo].c_mante.c_qpl;
    strcpy(qi,pfi->f_campos[campo].c_minimo);
	/*strcat(qi,"\004");*/
    strcpy(qs,pfi->f_campos[campo].c_maximo);

    modif = pfi->f_campos[campo].c_mante.c_modif;


	if ((pp = strstr(qs,"[")))
	{
		int _qpi;
		int qpf;
		int area;
		int ref;
		int topelin;
		char *texto[300];
		int flags = 0;
		int maximo = 0;
		
		area = atoi(pp+1);
		topelin = 0;
		for (k = campo;k < pfi->f_ncampos;k++)
		{
			if (pfi->f_campos[k].c_qc != maxqc || (k > campo && !pfi->f_campos[k].c_mante.c_visual))
			{
				break;
			}
			texto[topelin] = pfi->f_buf + pfi->f_campos[k].c_offset;
			topelin++;
			maximo += maxqc;
		}		
		texto[topelin] = NULL;
		ref = pfi->f_ipfi;
		wcid = pfi->f_campowids[campo];
	    _qpi = pfi->f_campos[campo].c_mante.c_qp;
		qpf = _qpi + area - 101;

		if (strstr(qs,"H"))
			flags += 1;
		if (strstr(qs,"V"))
			flags += 2;
		if (strstr(qs,"B"))
			flags += 4;
		else
		    flags |= ((maxqc << 16) & 0xffff0000);

		if (punt)
		{
			return crea_control_texto(punt,10000,_qpi,qpf,campo,ref,topelin,wcid,maxqc,texto,flags,maximo,wid,id,sid);
		}
		else
		{
			return crea_control_texto(NULL,0,_qpi,qpf,campo,ref,topelin,wcid,maxqc,texto,flags,maximo,wid,id,sid);
		}
	}

    if (vacio)
        qd[0] = 0;
    else
    {
        if (modif)
            formato_campo(pfi,campo,qd);
        else
            ld_campo(campo,pfi,qd);  /* valor defecto guardado en el buffer */
    }

    /*
    //qd
    //qs
    //qi
    */

    if (posicional)
    {
        k = pfi->f_campos[campo].c_mante.c_qp;
        if (!(posicional%100) && posicional >= 100)
        {
            k += (posicional-100);
        }
        else
        {
            k = posicional;
        }
        posi = k;
    }
    else
    {
        k = pfi->f_campos[campo].c_mante.c_qp;
    }

	if ((k%100) <= 1 || k%100 >= 80) {
		rela = 0;
	}

    if (lineal)
    {       
        wcid = pfi->f_idgrafico;

        lcid = pfi->f_campogids[campo];
    }
    else
    {
        wcid = pfi->f_campowids[campo];


        if (posicional)
        {
            posi =   (campo & 0xffff) | (posi << 16);
            lcid = posi;
        }
        else
        {
            lcid = campo;            
        }        
    }

    /*
    // px1[0] == cwid (en este caso a poner *)
    // px1[1] == cid
    // px1[2] == maxqc
    // px1[3] == qc (minimo visualizable)
    // px1[4] == ctipo
    // px1[5] == modificable 
    // px1[6] == decimales
    // px1[7] == len qi
    // px1[8] == len qs
    // px1[9] == ref_cid;
    // px1[10] == len qd-qo?
    */	

	rela = 0;
	for (i = 0;i < pfi->f_nrelaci;i++)
	{
		for (j = 0;pfi->f_relaci[i].r_campo[j] > 0;j++);
		if (!j) continue;
		j--;
		if (pfi->f_relaci[i].r_campo[0] == (campo+1) || pfi->f_relaci[i].r_campo[j] == (campo+1))
		{
			rela = 1;
			break;
		}		
	}	
	if (!rela)
	{
		if (pfi->f_campos[campo].c_key == 1 && !posicional && pfi->f_idgrafico <= 0)
			rela = 1;
	}
	if (vacio || (posicional && modif))
		rela = 0;
	if (rela)
	{
		strcat(qs,"\010\010");
	}
    return crea_entrada(wid,id,sid,qd,maxqc,qc,qi,qs,k,wcid,lcid,pfi->f_ipfi,modif,pfi->f_campos[campo].c_mante.c_mensaje,punt,vacio,lineal,pass);
}

char *carga_pantalla(char *nomp)
{
  int alto,ancho,tam;
  char *wvv;  
  char *rrr;
  int *p1;
  int f,j,i,k,n,l;
  short ppp;
  unsigned char texto;
  int comprimido = 0;

  f = rw_open_(nomp,O_RDONLY | O_BINARY,S_IWRITE | S_IREAD);
  if (f < 0) 
  {
     return(NULL);
  }

  j = rw_read_(f,&texto,1);
  if (j != 1 || texto) 
  {     
	 if (texto == 0xf1)
	 {
		 unsigned char tt[3];
		 j = rw_read_(f,(char *)tt,3);
		 if (j == 3 && tt[0] == 0xff && tt[1] == 0xff && tt[2] == 0x1f)
		 {
			 rw_read_(f,(char *)&i,sizeof(int));  
			 rw_read_(f,(char *)&k,sizeof(int));
			 rrr = (char *)malloc(i);
			 *((int *)rrr) = 0;			 
			 i -= sizeof(int);
			 if (i == k)
			 {
				rw_read_(f,(char *)(rrr+sizeof(int)),i);
			 }
			 else
			 {
				if (!k)
				{
					 rrr +=sizeof(int);
					 k = i;
					 j = 0;
					 while (j < k)
					 {
						if (rw_read_(f,(char *)&ppp,sizeof(short)) != sizeof(short)) break;
						if (ppp < 0)
						{
						   if (rw_read_(f,rrr+j,1) != 1) break;
						   ppp = (-ppp);
						   for (i = 0;i < ppp;i++)
							  *(rrr+j+i+1) = *(rrr+j+i);				
						   j++;
						}
						else
						{
						   if (rw_read_(f,rrr+j,ppp) != ppp) break;
						}
						j += ppp;
					 }
					 rrr -=sizeof(int);
				}
				/* Formato nuevo ... No se que hacer ...
				else
				{
				}
				*/
			 }
			 rw_close_(f);
			 return rrr;
		 }		 
	 }
     rw_close_(f);
     /* ATENCION PROBAR FORMATO ANTIGUO!!! *//* FALTA */     
     return(NULL);
  }
  
  rw_read_(f,(char *)&ppp,sizeof(short));
  moto_short(&ppp);
  if (!ppp)
  {
     comprimido = 1;
     rw_read_(f,(char *)&ppp,sizeof(short));     
  }
  
  alto  = ppp / 100;
  ancho = ppp % 100;
  tam = alto*ancho;

  rrr = malloc(tam*3+sizeof(int)*3);
  p1 = (int *)rrr;
  p1[0] = alto;
  p1[1] = ancho;
  p1[2] = tam;

  wvv = rrr + sizeof(int)*3;
  
  if (comprimido)
  {
     k = tam*3;
     j = 0;
     while (j < k)
     {
        if (rw_read_(f,(char *)&ppp,sizeof(short)) != sizeof(short)) break;
        moto_short(&ppp);
        if (ppp < 0)
        {
           if (rw_read_(f,wvv+j,1) != 1) break;
           ppp = (-ppp);
           for (i = 0;i < ppp;i++)
              *(wvv+j+i+1) = *(wvv+j+i);				
           j++;
        }
        else
        {
           if (rw_read_(f,wvv+j,ppp) != ppp) break;
        }
        j += ppp;
     }
  }
  else
  {
     j = rw_read_(f,wvv,tam*3);
  }
  
  if (j > (tam*3)) j = -1; else j = 0;
  rw_close_(f);

  return rrr;
}

int reduce_pantalla(char *pantalla,int *pi,int *pf)
{
  int alto,ancho,tam,j,i;
  int *pn;
  unsigned char *wvv,*p;

  pn = (int *)pantalla;
  
  alto = pn[0];
  if (!alto)
  {
	  return 0;
  }
  ancho = pn[1];
  tam = pn[2];
  wvv = pantalla + sizeof(int)*3;

  i = 0;
  for (j = 0;j < alto;j++)
  {
      p = wvv + j*ancho;      
      for (i = 0;i < ancho;i++)
      {
          if (/*p[i] != ' ' && */p[i] != 178)
          {
              break;
          }
      }
      if (i < ancho)
      {
          break;
      }
  }
  *pi += ((j*100)+i);

  i = 0;
  for (j = 0;j < alto;j++)
  {
      p = wvv + ((alto-1)*ancho)-(j*ancho);      
      for (i = 0;i < ancho;i++)
      {
          if (/*p[ancho-1-i] != ' ' && */p[ancho-1-i] != 178)
          {
              break;
          }
      }
      if (i < ancho)
      {
          break;
      }
  }
  *pf -= ((j*100)+i);

  return 0;
}

static void copia_las_rayas(char **_elem,int *p1,int posref,int tam,int *pn,int partepor)
{
	int ancho;
	int alto;
	int oancho;
	int *p0;	
	int ox,oy,oox,ooy;
	int sx,sy;
	int i;
	char *p,*op;
	char *elem;

	ancho = tam%100;
	alto = tam/100;
	elem = *_elem;
	if (!elem)
	{
		elem = (char *)malloc(sizeof(int)*10+ancho*alto+1);		 
		p0 = (int *)elem;
		p0[0] = sizeof(int)*10+ancho*alto+1;
		p0[1] = default_wid;
		p0[2] = default_id;
		p0[3] = default_sid;
		p0[4] = p1[4];
		p0[5] = posref;
		p0[6] = posref+tam-101;
		/*
		p0[7] = p1[7];
		p0[8] = p1[8];
		p0[9] = p1[9];
		*/
		p0[7] = pn[3];
		p0[8] = pn[4];
		p0[9] = pn[5];
		memset((char *)(p0+10),0,ancho*alto+1);
	}

	oancho = (p1[6]%100 - p1[5]%100)+1;
	if (oancho > ancho )
	{
		sx = ancho;
	}
	else
	{
		sx = oancho;
	}
	sy = (p1[6]/100 - p1[5]/100)+1;
	if (sy > alto)
	{
		sy = alto;
	}

	if ((p1[5]/100) < (posref/100))
	{
		oy = 0;
		ooy = (posref/100)-(p1[5]/100);
	}
	else
	{
		oy = (p1[5]/100)-(posref/100);
		ooy = 0;		
	}

	if ((p1[5]%100) < (posref%100))
	{
		ox = 0;
		oox = (posref%100)-(p1[5]%100);
	}
	else
	{
		ox = (p1[5]%100)-(posref%100);
		oox = 0;
	}

	p = elem + sizeof(int)*10;
	op = (char *)(p1+10);
	for (i = 0;i < sy;i++)
	{
		memcpy(p+ancho*(oy+i)+ox,op+oancho*(ooy+i+partepor)+oox,sx);
	}	

	*_elem = elem;
}

char **carga_mascara_pantalla(char **elem,char *pantalla,int ltam,int lpos,int partepor,int ajuste9)
{ /* ojo solo se comprueba linea de ltam para exclusion!! */
  int f,j,i,k,n,l,mas;
  int alto,ancho,tam;
  char *wvv,*p,*p1,*pt;  
  char **pp;
  int nelem;  
  unsigned char *mr,*pm;
  int *pn;
  unsigned int __Patrib = Patrib;
  int maparayas = -1;


  pn = (int *)pantalla;



  alto = pn[0];
  ancho = pn[1];
  tam = pn[2];
  wvv = pantalla + sizeof(int)*3;

  if (!elem)
  {      
      elem = realoca_multiplo(0,100,NULL);
      nelem = 0;
  }
  else
  {      
      nelem = 0;
      pp = elem;
      while(*pp) {nelem++;pp++;}
  }

  if (!pn[0])
  {
	 int *p1;	 
	 char *pele;
	 int i;
	 pele = (char *)(pn+9);
	 for (i = 0;i < pn[8];i++)
	 {
		 p1 = (int *)pele;
		 p1[1] = default_wid;
		 p1[2] = default_id;
		 p1[3] = default_sid;
		 if (((p1[5]/100)-(lpos/100)) >= (ltam/100)) {
			 pele += p1[0];
			 continue;
		 }
		 elem = realoca_multiplo(nelem,100,elem);
		 if (!p1[4])
		 {
			 elem[nelem] = NULL;
			 if (maparayas < 0)
			 {
				 maparayas = nelem;
			 }			 
			 copia_las_rayas(&(elem[maparayas]),p1,lpos,ltam,pn,partepor);
			 if (nelem != maparayas)
			 {
				 pele += p1[0];
				 continue;
			 }
		 }		 
		 else
		 {
			 
			 elem[nelem] = (char *)malloc(p1[0]);
			 memcpy(elem[nelem],pele,p1[0]);			 
		 }
		 pele += p1[0];
		 nelem++;
	 }
	 elem[nelem] = NULL;
	 return elem;
  }

  /* mascara de rallas */

  elem = realoca_multiplo(nelem,100,elem);
  mas = w_tam_stdid() + sizeof(int)*10 + tam; /* memoria */

  elem[nelem] = malloc(mas);
  pt = elem[nelem];
  memset(pt,0,mas);  
  
  memcpy(pt,&mas,sizeof(int));
  pt += sizeof(int);

  pt = w_copia_stdid(pt);

  mas = 0; /* mascara de rallas */
  memcpy(pt,&mas,sizeof(int));
  pt += sizeof(int);

  memcpy(pt,&lpos,sizeof(int));
  pt += sizeof(int);

  j = lpos + ltam - 101;
  memcpy(pt,&j,sizeof(int));
  pt += sizeof(int);

  j = (int)Patrib;
  memcpy(pt,&j,sizeof(int));
  pt += sizeof(int);

  memcpy(pt,&PCcolor,sizeof(int));
  pt += sizeof(int);

  memcpy(pt,&PFcolor,sizeof(int));
  pt += sizeof(int);

  mr = pt;

  pt += tam; /* ya no tiene sentido */

  nelem++;

  for (i = 0;i < nelem;i++)
  { 
      pn = (int *)elem[i];
      if (pn[5] == 2)
      {
         if (pn[6] >= lpos && pn[6] <= (lpos + ltam - 101))
         {
            l = pn[6] - lpos;
            p = wvv + (l/100)*ancho + (l%100);
            *((unsigned char *)p) = 255;
         }
      }
  }                                  


  for (j = 0;j < alto;j++)
  {
      p = wvv + (j+partepor)*ancho;
      n = 0;
      pm = mr + j*ancho;
      for (i = 0;i < ancho;i++)
      {
          if (j < (ltam/100) && i < (ltam%100))
              pm[i] = codifica_semigrafico((unsigned char)p[i]);
          else
              pm[i] = 0;
      }


      i = 0;
      do
      {
          for (;i < ancho;i++)
          {              
              if (p[i] != ' ' && (((unsigned char)p[i]) < 128 || !C_graficolinea[((unsigned char)p[i])-128]))
              {                  
                  n = i;
                  break;
              }
              n = i + 1;
          }
          if (i < ancho)
          {
              for (;i < ancho;i++)
              {
                  if (((unsigned char)p[i]) == 255 || p[i] == ' ' || (((unsigned char)p[i]) >= 128 && C_graficolinea[((unsigned char)p[i])-128]))
                  {
                      if ( (i -n) > 0)
                      {
                          if (ajuste9) {
							  if (p[i] == ' ' && (i == 0 || p[i-1] != ':'))
							  { 								  
								  for (l = i+1;l < ancho;l++)
								  {
									  if (p[l-1] == ' ' && p[l] == ' ') 
									  {
										  i = l-1;
										  break;
									  }
									  if (((unsigned char)p[l]) == 255 || (((unsigned char)p[l]) >= 128 && C_graficolinea[((unsigned char)p[l])-128]))
									  {
										  if ( ((unsigned char)p[l]) == 255 && (p[l-1] == ' ' || p[l-1] == '.'))
										  {
											  p[l-1] = ':';
										  }
										  i = l;
										  break;
									  }
									  if (p[l] == ':')
									  {
										  i = l+1;
										  break;
									  }
								  }
							  }
						  }

                          if (j < (ltam/100))
                          {
                              Patrib = (unsigned int)*(p+n+tam*2);
                              elem = realoca_multiplo(nelem,100,elem);
                              elem[nelem] = crea_control_estatico(lpos + j * 100 + n,lpos + j * 100 + n + ((i - n) - 1),p+n,i - n);
                              nelem++;
                          }
                         
                      }
                      n = i;
                      break;
                  }
              }
          }
      } while(i < ancho);
      if ((i-n) > 0)
      {
          if (j < (ltam/100))
          {
              Patrib = (unsigned int)*(p+n+tam*2);
              elem = realoca_multiplo(nelem,100,elem);
              elem[nelem] = crea_control_estatico(lpos + j * 100 + n,lpos + j * 100 + n + ((i - n) - 1),p+n,i - n);
              nelem++;
          }
      }
  }  

  elem[nelem] = NULL;

  Patrib = __Patrib;
  
  return elem;
}

void w_salva(VENT *w)
{
}

VENT *w_ini(int pi,int pf)
{
  return NULL;
}


void Cp()
{
}

void Ap()
{
}

void Bi()
{
}

void Ai()
{
}

void Cr()
{
}

void Ar()
{
}

void Nor()
{
}

void As()
{
}

void Cs()
{
}

void Graf(int i)
{
}

void Gon()
{
}

void Goff()
{
}

int es_atri(char a)
{
   if (a == 'I' || a == 'R' || a == 'P' || a == 'S')
      return(1);
   return(0);
}

int Conversion(unsigned char c)
{
    return 0;
}

void w_refresca(VENT *w)
{
}

int w_mete(VENT *w,VENT *wo)
{
    return 0;
}

int w_metev(VENT *w,VENT *wo)
{
}

void w_mueve(VENT *w,int p)
{
}

int w_lee(VENT *w,char *tmp)
{
}

void Color(int c,int f)
{
    if (c != f)
    {
       PCcolor = c;
       PFcolor = f;
    }
}

void atributo(char c)
{   
    if (c < 'A')
        Patrib = ' ';
    else
        Patrib = c & ~128;   
    PCcolor = -1;
    PFcolor = -1;
}

void Bl(int ln)
{
	int wid = default_wid;
	int sid = default_sid;

    if (ln == 4) {
        borra_mensaje_std();
		return;
	}
	if (espuro10)
		return;

	if (default_sid > 0) {
	   int ifi,ipa;
	   ifi = (default_sid%1000)-1;
	   ipa = default_sid/1000;
	   if (ifi >= 0 && ifi < nfi && ipa >= 0 && ipa < v_mpfi(ifi)->f_npanta && default_id == v_mpfi(ifi)->f_id) {
		   int pi,pf;
		   pi = v_mpfi(ifi)->f_pantalla[ipa].p_posref;
		   pf = pi + (v_mpfi(ifi)->f_pantalla[ipa].p_posfin-101);
		   if (ln > (pf/100)) {
			   wid = -1;
			   sid = 0;
		   }
	   }
	}
    BorraDescendencia(wid,default_id,sid,-(ln*100+1),ln*100+80);
}


int w_saca(VENT *w,VENT *wo)
{
    return 0;
}

int v_printd(char *u,int s)
{
    return 0;
}

char *tecla_funcion(int t,char *p,int m)
{
    return NULL;
}

static int B_D_primero = 1;
void ClsLimpio(int modo)
{
	B_D_primero = modo;
}
int BorraDescendencia(int wid,int id,int sid,int pi,int pf)
{
    int tmpx[512];         
    int ipfi;
    int n;
    int i,npan,j;	

	memset(tmpx,0,sizeof(tmpx));

    if (DespachadorExterno)
    {
        tmpx[0] = -1;
        tmpx[1] = wid;        
        tmpx[2] = id;
        tmpx[3] = sid;
        tmpx[4] = pi;
        tmpx[5] = pf;

		if (!nfi || B_D_primero)
		{			
			
			(*DespachadorExterno)((int)DESPACHO_PANTALLAS,(int)10008,(void *)tmpx);			
			B_D_primero = 0;
		}
		else
		{
			(*DespachadorExterno)((int)DESPACHO_PANTALLAS,(int)8,(void *)tmpx);
			for (i = 0;tmpx[i] != -1;i++)
			{
				ipfi = (tmpx[i] % 1000) - 1;
				npan = tmpx[i] / 1000;
				if (ipfi >= 0 && ipfi < nfi && npan >= 0 && npan < v_mpfi(ipfi)->f_npanta)
				{
					v_mpfi(ipfi)->f_pantaids[npan] = -1;
					for (j = v_mpfi(ipfi)->f_pantalla[npan].p_campoini - 1;j < v_mpfi(ipfi)->f_pantalla[npan].p_campofin;j++)
					{
						n = v_mpfi(ipfi)->f_taborder[j];
						if (n >= 0 && n < v_mpfi(ipfi)->f_ncampos)
						{
							v_mpfi(ipfi)->f_campowids[n] = -1;
						}
					}
				}
			}
		}
    }

    agi_cola_timer();

    return 0;
}

void cls()
{
    BorraDescendencia(-1,default_id,0,-1,-1);
    default_sid = 0;
    default_wid = -1;
}

int curon(int i)
{
    return 0;
}

int anula_mapa_raton(int id)
{
    return 0;
}

int recurre_raton(int m)
{
    return 0;
}

int pon_mapa_raton(int pi,int pf,int id,int estado)
{
    return 0;
}

int no_pintar_ahora(int p)
{
    return 0;
}

int nume_code()
{
return( 00000 );
}

int nume_mochila()
{
return( 00000 );
}

int pon_code(short n)
{
    /* *((short *)(g_+2)) = n; */
    return 0;
}

int pon_mochila(short n)
{
   /*  *((short *)(g_)) = n; */
    return 0;
}

int cambia_s_req(char *tmp2,int modo)
{
   if (!modo)
   {
      while (*tmp2)
	  { 
         if (*tmp2 == '(' || *tmp2 == ')') *tmp2 = '-';
         tmp2++;
	  }
   }
   else
   {
      if (*tmp2 == '@' && *(tmp2+1) == '-')
	  {
         tmp2++;
         *tmp2 = '(';
         tmp2++;
         while (*tmp2)
         {
            tmp2++;
         }
         while (*tmp2 != '(')
         {
            if (*tmp2 == '-')
            {
               *tmp2 = ')';
               break;
            }
            tmp2--;
         }
	  }
   }
   return(0);
}


int printa_grafico(char *FileName)
{
#ifndef FGVJAVA
	if (DespachadorExterno)
    {
            int m = -1;	 
            (*DespachadorExterno)((int)DESPACHO_FUNCIONES_IMPRESION,4,(void *)FileName);
            agi_cola_timer();
            return(m);
    }    
#endif
    return(-1);
}

int ini_w_printer(int m)
{
			extern char inicializo[37];
         extern int i_nlineas;
         /* si m coger impresora por defecto */
         if (DespachadorExterno)
         {
            unsigned char tmp[1024];
            unsigned short LLength = (unsigned short)(unsigned char)inicializo[35];
            memcpy(tmp,&m,sizeof(int));
            memcpy(tmp+sizeof(int),&LLength,sizeof(short));
            memcpy(tmp+sizeof(int)+sizeof(short),inicializo,LLength);
            (*DespachadorExterno)((int)DESPACHO_FUNCIONES_IMPRESION,1,(void *)tmp);
            //agi_cola_timer(); 
            inicializo[0] = 0;
            inicializo[35] = 0;
            memcpy(&m,tmp,sizeof(int));
            if (m < 0)
               return -1;
            if (m)
            {
               i_nlineas = m;
               memcpy(&m,tmp+sizeof(int),sizeof(int));
               i_anchocarro = m;
            }
            return(0);
         }
         return(-1);
}

int printa_w_linea(unsigned char *szLine,unsigned short LLength)
{
		if (DespachadorExterno)
        {	 
            unsigned char tmp[2048];
			int *p;
			int l;
			
			if (nombredev[0] == 2 && nombredev[1] == 0 && nombredev[2])
			{
				memset(tmp,0,sizeof(tmp));
				p = (int *)tmp;
				*p = 2;
				p++;
				l = (LLength/4)+1;
				*p = l;
				p++;				
				memcpy((char *)p,szLine,LLength);
				p += l;
				if (nombredev[2] == '!' && nombredev[3] == '!')
				{
					l = ((strlen(numerofax)+1) / 4) + 2;
					*p = l;
					p++;
					*p = 1;				
					p++;
					strcpy((char *)p,numerofax);
				}
				else if (nombredev[2] == '$' && nombredev[3] == '$')
				{
					l = ((strlen(nombredev+4)+1) / 4) + 2;
					*p = l;
					p++;
					*p = 0;
					p++;
					strcpy((char *)p,nombredev+4);					
				}
				else if (nombredev[2] == '|' && nombredev[3] == '|')
				{
					l = ((strlen(numerofax)+1) / 4) + 2;
					*p = l;
					p++;
					*p = 2;
					p++;
					strcpy((char *)p,numerofax);
				}
				else
					return -1;
				
				(*DespachadorExterno)((int)DESPACHO_FUNCIONES_IMPRESION,6,(void *)tmp);				
			}
			else			
			{
				memcpy(tmp,&LLength,sizeof(short));
				memcpy(tmp+sizeof(short),szLine,LLength);
				(*DespachadorExterno)((int)DESPACHO_FUNCIONES_IMPRESION,2,(void *)tmp);
			}
            return(*((int *)tmp));
         }
         return(-1);
}

int fin_w_printer()
{
		if (DespachadorExterno)
         {            
			if (nombredev[0] == 2 && nombredev[1] == 0 && nombredev[2])
			{
				unsigned char tmp[2048];
				int *p;
				int l;
				memset(tmp,0,sizeof(tmp));
				p = (int *)tmp;
				*p = 1;
				p++;
				if (nombredev[2] == '!' && nombredev[3] == '!')
				{
					l = ((strlen(numerofax)+1) / 4) + 2;
					*p = l;
					p++;
					*p = 1;				
					p++;
					strcpy((char *)p,numerofax);
				}
				else if (nombredev[2] == '$' && nombredev[3] == '$')
				{
					l = ((strlen(nombredev+4)+1) / 4) + 2;
					*p = l;
					p++;
					*p = 0;
					p++;
					strcpy((char *)p,nombredev+4);
				}				
				else if (nombredev[2] == '|' && nombredev[3] == '|')
				{
					l = ((strlen(numerofax)+1) / 4) + 2;
					*p = l;
					p++;
					*p = 2;
					p++;
					strcpy((char *)p,numerofax);
				}
				else
					return -1;
				(*DespachadorExterno)((int)DESPACHO_FUNCIONES_IMPRESION,7,(void *)tmp);
				return(*((int *)tmp));
			}
			else
			{
				int m = 0;
				(*DespachadorExterno)((int)DESPACHO_FUNCIONES_IMPRESION,3,(void *)&m);
				return(m);
			}            
         }
         return(-1);
}



/*static int _especial_servidor = 0;*/

int pregunta_impresion(char *_fispo,char *__ejspool,char *_rw_user,int *especial_servidor,char *__fispool,char *otrospool)
{
   int r;
  extern char queimpresora[512];  
   
   // Preguntar si usar la impresora interna o las antiguas
   if (DespachadorExterno)
   {
	extern char inicializo[37];
	extern char finalizo[37];

			   {
					  /*char *_fispo,char *__ejspool,char *_rw_user,int *especial_servidor,char *__fispool;*/
					  /* impresora "antigua" */
					  char tmp[1024];

					  strcpy(tmp,_fispo);
					  quitab(tmp);
					  strcpy(tmp+256,_rw_user);
					  quitab(tmp+256);
					  (*DespachadorExterno)((int)DESPACHO_FUNCIONES_IMPRESION,5,(void *)tmp);
					  if (!tmp[0])
						 return -1;
					  if (!strcmp(tmp,"Estandard"))
					  {
						  strcpy(queimpresora,"Estandard");
						  r = 0;
					  }
					  else if (!strcmp(tmp,"Excel Estandard"))
					  {
						  strcpy(queimpresora,"Excel Estandard");
						  r = 2;
					  }
					  else if (!strcmp(tmp,"Pantalla Estandard"))
					  {
						  strcpy(queimpresora,"Pantalla Estandard");
						  r = 3;
					  }
					  else if (!strcmp(tmp,"Crystal Reports"))
					  {
						  strcpy(queimpresora,"Crystal Reports");
						  r = 4;
						  strcpy(numerofax,tmp+256);
					  }
					  else if (!strcmp(tmp,"Fax"))
					  {						  
						  strcpy(queimpresora,"Fax");
						  r = 5;
						  strcpy(numerofax,tmp+256);
					  }
					  else if (!strcmp(tmp,"PDF"))
					  {						  
						  strcpy(queimpresora,"PDF");
						  r = 6;
						  strcpy(numerofax,tmp+256);
					  }
					  else if (!strcmp(tmp,"Correo"))
					  {						  
						  strcpy(queimpresora,"Correo");
						  r = 7;
						  strcpy(numerofax,tmp+256);
					  }
					  else
					  {	
						  strcpy(_fispo,rw_directorio("dev/imp/"));
						  strcat(_fispo,tmp);
						  if ((r = i_leeimpre(_fispo)) < 0)
						  {
							  return -1;
						  }

						  r = 0;

						  strcpy(_fispo,i_nombre());

						  
						  if (_fispo[0] == '@')
						  {
							*especial_servidor = 1;							
						  }

						  if (_fispo[0] == '*' && strlen(tmp+256))
						  {
							 strcpy(__ejspool,"bin/");					 
							 quitab(_fispo);
							 if (_fispo[1] == 0)
								strcat(__ejspool,"agispool");
							 else
								strcat(__ejspool,_fispo+1);
							 strcpy(_fispo,tmp+256);
							 quitab(_fispo);
							 strcatd(_fispo,rw_directorio("spool/"));
							 strcat(_fispo,".spo");
							 if (__ejspool[0])
							 {
								strcat(__ejspool," ");
								strcat(__ejspool,_fispo);
							 }					 
							 strcpy(__fispool,_fispo);
						  }
						  else
						  {	
							  if (*especial_servidor)
							  {
								 strcpy(__fispool,tmp+256);
								 if (__fispool[0])
								 {
									 strcat(__fispool,".spo");
									 strcpy(__ejspool,"bin/");
									 if (_fispo[2] == 0)
										strcat(__ejspool,"agispool");
									 else
										strcat(__ejspool,_fispo+2);
								 }
								 else
								 {
									 __ejspool[0] = 0;
								 }
							  }
							  else
							  {
								 __fispool[0] = 0;
								 __ejspool[0] = 0;
							  }
							 strcpy(_fispo,i_nombre());
						  }      
						  return r;
					  }
			   }
				  
                  if (r == 3)
                  {
                      /* pantalla */
                      _fispo[0] = 1;
                      _fispo[1] = 0;
                      __fispool[0] = 0;
                      __ejspool[0] = 0;                      
                      return 0;
                  } else if (r == 2)
                  {
					 int yaini = 0;
					 i_leeimpre((char *)1);
					 if (_fispo[0] == '?')
					 {
						 yaini = 1;
						 strcpy(inicializo,_fispo+1);
						 inicializo[35] = strlen(inicializo);
						 strcpy(finalizo,"salva");
						 finalizo[35] = strlen(finalizo);
					 }                     
                     strcpy(_fispo,i_nombre());
                     __fispool[0] = 0;
                     __ejspool[0] = 0;
					 if (!yaini)
					 {
						(*DespachadorExterno)((int)DESPACHO_FUNCIONES_IMPRESION,4,(void *)inicializo);
                        inicializo[35] = strlen(inicializo);
					 }
#ifdef RWXARXA
                     if (en_conexion_terminal)
                        *especial_servidor = 0;
#endif
                     return(0);
                  }
                  else if (!r || r == 4 || r == 5 || r == 6 || r == 7)
                  { // usar la impresora windows interna
                        i_leeimpre(NULL);
                        strcpy(_fispo,i_nombre());						

						if ((r == 4 || r == 5 || r == 6 || r == 7) && !_fispo[0])
						{
							quitab(numerofax);
							_fispo[0] = 2;
							_fispo[1] = 0;
							{								
								nombredev[0] = 2;
								nombredev[1] = 0;
								if (r == 5)
								{
									strcpy(queimpresora,"Fax");
									strcpy(otrospool,"!!");
									strcat(otrospool,numerofax);
									strcpy(nombredev+2,otrospool);									
								}
								else if (r == 6)
								{
									strcpy(queimpresora,"PDF");
									strcpy(otrospool,"$$");
									if (quita_path(numerofax) == numerofax)
									{
										strcatd(numerofax+1,rw_directorio("spool/"));
										cambia_barra_dos(numerofax+1);
									}									
									strcat(otrospool,numerofax);
									strminus(numerofax);
									if (!strstr(numerofax,".pdf"))
									{
										strcat(otrospool,".pdf");
										strcat(numerofax,".pdf");
									}
									strcpy(nombredev+2,otrospool);									
								}
								else if (r == 7)
								{
									strcpy(queimpresora,"Correo");
									strcpy(otrospool,"||");
									strcat(otrospool,numerofax);
									strcpy(nombredev+2,otrospool);									
								}
								else
								{
									strcpy(otrospool,numerofax);
									strcpy(nombredev+2,otrospool);
								}
							}
						}
                        __fispool[0] = 0;
                        __ejspool[0] = 0;
#ifdef RWXARXA
                        if (en_conexion_terminal)
                           *especial_servidor = 0;
#endif
                        return(0);
                  }
   }

/*
#ifdef RWXARXA
   if (en_conexion_terminal)
	  *especial_servidor = _especial_servidor;
#endif
*/   return(r);
}

int v_printF(char *u,double s)
{
    return 0;
}

int w_graba(VENT *w,char *p)
{
    return 0;
}

void v_scroll(int id,int sid,int ini,int fin,int n)
{
/*
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      vscroll_en_terminal(ini,fin,n);
      return;
   }
#endif
#endif
*/
   if (id == -1)
   {
       id = default_id;
   }
   if (sid == -1)
   {
       sid = default_sid;
   }
   if (DespachadorExterno)
   {
       char tmp[512];
       int *p1 = (int *)tmp;

	   memset(tmp,0,sizeof(tmp));

       p1[0] = sizeof(int) * 6;
       p1[1] = id;
       p1[2] = sid;
       p1[3] = ini;
       p1[4] = fin;
       p1[5] = n;
       (*DespachadorExterno)((int)DESPACHO_PANTALLAS,9,(void *)tmp);
       /*agi_cola_timer();*/
   }
}

void cursor(int a)
{
    Pcursor = a;
}

void pito(int codigo)
{
	/*
	{
		char tmp[512];
		sprintf(tmp,"codigo pito:%d",codigo);
		aviso(0,tmp);
	}
	*/
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      pito_en_terminal();
      return;
   }
#endif
#endif
#ifdef FGVJAVA
   {
	   char tmp[512];
	   tmp[0] = 0;
	   (*DespachadorExterno)((int)DESPACHO_MULTIPLE,666,(void *)tmp);
   }
#else
#ifdef FGVDOS
   MessageBeep(MB_ICONEXCLAMATION);
#endif
#endif
}

int v_print(unsigned char *s)
{
    return pprint(Pcursor,s);
}

int esta_en_el_raton(int p,int *estado)
{
    return 0;
}

int agi_cola_win()
{
    agi_cola_timer();
    return 0;
}

void pon_tecla_timeout(int a,int b,int c,int d)
{
}

int pantalla(char *p)
{
    return 0;
}

#ifdef FGVDOS
void ConvierteTxtARect(int pi,int pf,RECT *r)
{ /* especial para lineas!!! */
}
#endif


int ptec(int t)
{
   int n;
   
   for (n = 0;n < 500 && buf_tecla[n] != -1;n++);
   if (n == 500) { /* reset por overflow */
      for (n = 0;n < 500;n++)
         buf_tecla[n] = -1;
      return(1);
   }
   if (n < 499)
      buf_tecla[n+1] = -1;
   for (;n > 0;n--)
      buf_tecla[n] = buf_tecla[n-1];
   buf_tecla[0] = t;
   return(0);
}

int que_ptec_hay()
{
   return(buf_tecla[0]);
}


int saca_ptec()
{
  int d = -1,c;


  if (buf_tecla[0] != -1) 
  {
     d = buf_tecla[0];
     for (c = 0;c < 499 && buf_tecla[c] != -1;c++)
       buf_tecla[c] = buf_tecla[c+1];
     buf_tecla[c] = -1;
  }
  return d;
}

void w_putc(VENT *w,unsigned char c)
{
}

int v_prints(char *u,char *s)
{
    return 0;
}

int v_printc(char *u,char s)
{
    return 0;
}

int recurre_boton(int m)
{
    return 0;
}

int dialogo_activacion(char *num1,char *num2,char *res)
{
    if (DespachadorExterno)
    {       
         char tmp[512];
         int *p1 = (int *)tmp;
         char *p = tmp+sizeof(int)*2;
         sprintf(p,"%-20.20s %-20.20s %-20.20s",num1,num2,res);
         p1[0] = sizeof(int)*2 + strlen(p) + 2;     
         p1[1] = 0;
         (*DespachadorExterno)((int)DESPACHO_MAINCOMMAND,ID_DLGKACTIVA,(void *)tmp);
         agi_cola_timer();
         quitab(p);
         strcpy(res,p);
         if (p1[1] != IDOK)
             return -1;
         return 0;
    }    
    return 0;
}

int llama_fun_int(int (*f_f)(),char *par_f)
{
    return (*f_f)(par_f);
}

char **salva_funciones(int ini,int fin)
{
    return NULL;
}

int repon_funciones(char **funs,int ini,int fin)
{
    return 0;
}

int w_print(VENT *v,char *s)
{
    return 0;
}

void w_ll(VENT *w,int l)
{
}

void w_wll(VENT *w,int l)
{
}

void w_cls(VENT *w)
{
}

int _w_mete(VENT *w,VENT *wo,int modo)
{
    return 0;
}

int w_marco_(VENT *w,int pi,int pf,int tipo)
{
    return 0;
}

int w_cambia(VENT *w,int pi,int pf)
{
    return 0;
}

int w_printc(VENT *v,char *u,char s)
{
    return 0;
}

int w_prints(VENT *v,char *u,char *s)
{
    return 0;
}
int carga_grafico(char *FileName,VENT *w)
{
    return 0;
}

void crea_bmp_w(VENT *w,int flag)
{
}

int convierte_caracter(unsigned char c)
{
    return 0;
}


/*****************************************!!!!*************************************************/

int control_de_usuarios()
{
   char **_dir;
   char **__dir;
   char *p;
   FILE *f;
   char _buf_[20000];
   char eldir[256];
   int i,j,n;
   int pid;
   int puede;
   static int t_no_recursion_ = 0;
   
   if (noboton_usuarios)
   {
	  return 0;
   }

   if (t_no_recursion_) return 0;
   
   t_no_recursion_ = 1;

   do
   {

	 if (!R_midir[0])
	 {
		strcpy(R_midir,rw_directorio("dev/reg/"));
	 }

   puede = 0;
   i = 0;
   strcpy(eldir,R_midir);
   strcat(eldir,"*.rlc");
   /*rw_directorio("dev/reg/*.rlc"));*/

   _dir = lee_direct(eldir,0);
   if (_dir)
   {	   
       for (__dir = _dir;*__dir;__dir++)
       {
		   p = *__dir;
		   for (j = 0;p[j] && p[j] != '.';j++);
		   if (p[j] == '.')
		   {
			 p[j]  = 0;
			 pid = atoi(p);			 
			 p[j]  = '.';
			 puede = 0;
#ifdef FGVDOS
			 {
             HANDLE  hProcess;  
             hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, pid );
			 if (hProcess)
			 {
				 puede = 1;
				 CloseHandle( hProcess );
			 }

			 }
#else
			 if (kill(pid,0) >= 0)
				 puede = 1;
#endif
			 if (!R_midir[0])
			 {
				strcpy(R_midir,rw_directorio("dev/reg/"));
			 }

			 sprintf(eldir,"%s%s",R_midir,*__dir);
			 if (puede)
			 {				
				f = fopen(eldir,"r");
				if (f)
				{
					fgets(eldir,250,f);
					quitan(eldir);
					quitab(eldir);
					fclose(f);
					sprintf(_buf_+i,"%ld %s",pid,eldir);
					i += strlen(_buf_+i);
					_buf_[i] = ';';
					i++;
					_buf_[i] = 0;
				}
			 }
			 else
			 {
				unlink(eldir);
			 }
		   }
	   }
       libera_dir(_dir);
   }

	 if (!R_midir[0])
	 {
		strcpy(R_midir,rw_directorio("dev/reg/"));
	 }

   sprintf(eldir,"%s%ld.msg",R_midir,getpid());
   j = rw_open_(eldir,O_RDONLY | O_BINARY);
   if (j > -1)
   {
	   n = rw_read_(j,_buf_+i,5000);
	   rw_close_(j);
	   if (n > 0)
		   i += n;
	   unlink(eldir);
   }
   _buf_[i] = 0;

   if (DespachadorExterno)
   { 
       (*DespachadorExterno)((int)DESPACHO_USUARIOS,0,(void *)_buf_);
       agi_cola_timer();
   }



   if (_buf_[0] == 2 && _buf_[1] == 2)
   {
	   char *mensaje;
	   char *p1;
	   p = _buf_+2;
	   while(*p && *p != '|')
	   {
		   p++;
	   }
	   if (*p == '|')
	   {
		   mensaje = p + 1;
		   n = strlen(mensaje);
		   p1 = _buf_+2;
		   for (p = _buf_+2;*p && *p != '|';p++)
		   {
			   if  (*p == ';')
			   {
					 if (!R_midir[0])
					 {
						strcpy(R_midir,rw_directorio("dev/reg/"));
					 }

				   sprintf(eldir,"%s%ld.msg",R_midir,atol(p1));
				   j = rw_open_(eldir,O_RDWR | O_BINARY | O_CREAT | O_APPEND,S_IWRITE | S_IREAD);
				   if (j > -1)
				   {
					   rw_write_(j,mensaje,n);
					   rw_close_(j);
				   }
				   p1 = p + 1;
			   }
		   }
	   }
	   continue;
   }
   if (_buf_[0] == 3 && _buf_[1] == 3)
   {
	   /* refrescar */
	   continue;
   }
   if (_buf_[0] == 4 && _buf_[1] == 4)
   {
	   /* ejecucion!!! */
	   pid = atoi(_buf_+2);
#ifdef FGVDOS
	   KillPid(pid);
#else
	   kill(pid,9);
#endif

	   continue;
   }

   break;

   } while(-1);
   
   t_no_recursion_ = 0;
   return 0;
}


int ini_my_consola()
{
    return 0;
}

int fin_my_consola()
{
    return 0;
}

char *nombre_tecla(int t)
{
    return NULL;
}

void pon_color(int a,int b,int c,int m)
{
}

void qursor(int a)
{
    Pcursor = a;
}


void aviso(int p,char *text)
{

if (saca_ptec() != -1) return p;

if (DespachadorExterno)
{       
     char tmp[512];
     int *p1 = (int *)tmp;     
     strcpy(tmp+sizeof(int)*2,text);

     /*oem_a_ansi(tmp+sizeof(int)*2);*/
	 /*ansi_a_oem(tmp+sizeof(int)*2);*/

     p1[0] = sizeof(int)*2 + strlen(text) + 2;     
     p1[1] = 0;
     (*DespachadorExterno)((int)DESPACHO_MAINCOMMAND,ID_MESSAGEBOX,(void *)tmp);
     agi_cola_timer();
     return p;
}    
return p;
}

int cabecera_v(char *version,char *proceso,char *titulo,char *empresa)
{
   char tmp[256];   
   
   if (version)
   {
      if (!proceso[0] && !titulo[0] && !empresa[0])
      {
         sprintf(tmp,"%s",version);
      }
      else
      {
		 if ((remoto_es_sistema_grafico & 128))	  
			sprintf(tmp,"%s ''%s'' %s.",proceso,empresa,version);
		 else
			sprintf(tmp,"%s %s %s",proceso,empresa,version);
      }
   }
   else
   {
	  if ((remoto_es_sistema_grafico & 128))	  
		 sprintf(tmp,"%s %s ''%s''.",titulo,proceso,empresa);
	  else
		 sprintf(tmp,"%s %s %s",titulo,proceso,empresa);
   }
   tmp[129] = 0;
   if (DespachadorExterno)
   {
      (*DespachadorExterno)((int)DESPACHO_TITULO,0,tmp);
      /*agi_cola_timer();*/
   }
   return 0;
}

static char *mensa_en_cabeza[100][3];
static int recur_mensa = 0;


int recurre_mensaje_cabecera(m)
int m;
{
   int lado;
   int hay;
   if (!m)
			{
      if (recur_mensa < 100)
      {
         hay = 0;
         for (lado = 0;lado < 3;lado++)
         {
            if (mensa_en_cabeza[recur_mensa][lado])
            {
               if (DespachadorExterno)
               {
                  (*DespachadorExterno)((int)DESPACHO_MENSAJE_CABECERA,lado,NULL);
                  /*agi_cola_timer();*/
               }
               hay = 1;
            }
         }
         if (hay || recur_mensa)
            recur_mensa++;
      }
			}
   else
			{
      if (recur_mensa)
      {
         recur_mensa--;
         for (lado = 0;lado < 3;lado++)
         {
            if (mensa_en_cabeza[recur_mensa][lado])
            {
               if (DespachadorExterno)
               {
                  (*DespachadorExterno)((int)DESPACHO_MENSAJE_CABECERA,lado,mensa_en_cabeza[recur_mensa][lado]);
                  /*agi_cola_timer();*/
               }
            }
         }
      }
			}
   return(0);
}


borra_mensaje_cabecera(l)
int l;
{
	     int lado;

         if (l == -1 || l == 3)
         {
               if (DespachadorExterno)
               {
                  (*DespachadorExterno)((int)DESPACHO_MENSAJE_CABECERA,3,NULL);                  
               }
               if (l == 3) return 0;
         }
         
         for (lado = 0;lado < 3;lado++)
         {
            if (l != -1 && l != lado) continue;
            if (mensa_en_cabeza[recur_mensa][lado])
            {
               free(mensa_en_cabeza[recur_mensa][lado]);
               mensa_en_cabeza[recur_mensa][lado] = NULL;
               if (DespachadorExterno)
               {
                  (*DespachadorExterno)((int)DESPACHO_MENSAJE_CABECERA,lado,NULL);
                  /*agi_cola_timer();*/
               }
            }
         }
return 0;
}

mensaje_en_cabecera(usi,men,lado)
char *usi,*men;
int lado;
{
   char mensa[255];


   sprintf(mensa,usi,men);
   
   /*ansi_a_oem(mensa);*/


   if (lado == 3)
   {
       if (DespachadorExterno)
        {
          (*DespachadorExterno)((int)DESPACHO_MENSAJE_CABECERA,lado,mensa);
          /*agi_cola_timer();*/
        }
       return 0;
   }

   
   if (lado >= 0 && lado < 3)
   {    
    borra_mensaje_cabecera(lado);
    mensa_en_cabeza[recur_mensa][lado] = malloc(strlen(mensa)+1);
    strcpy(mensa_en_cabeza[recur_mensa][lado],mensa);  
   
    if (DespachadorExterno)
    {
      (*DespachadorExterno)((int)DESPACHO_MENSAJE_CABECERA,lado,mensa_en_cabeza[recur_mensa][lado]);
      /*agi_cola_timer();*/
    }
  }
  return 0;
}


int mensaje(int pos,char *str)
{
 char tmp[128];
 int pp,pp2;
 
 pp2 = 0;
 for (pp = 0;str[pp];pp++)
 {
    tmp[pp2] = str[pp];
    if (str[pp] == 'F' && str[pp+1] == '1' && (str[pp+2] < '0' || str[pp+2] > '9'))
    {
       pp++;
       pp2++;
       tmp[pp2] = '1';
       pp2++;
       tmp[pp2] = '2';
    }
    pp2++;
 }
 if (pp2 > 80) pp2 = 80;
 tmp[pp2] = 0;
 str = tmp;

 if ((pos/100) > 4) {
	 if ((pos%100) == 0) {
		 pos += 1+(80-strlen(str))/2;
	 }
	 pprint(pos,str);
	 return pos;
 }

    mensaje_en_cabecera("%s",str,0);
    return pos;
}

int borra_mensaje_std()
{
    return borra_mensaje_cabecera(0);
}

int c_rw_error = 0;
int no_rw_error = 0;

int rw_error(n)
int n;
{
   char tmp[128];

   c_rw_error++;
   if (no_rw_error)
	   return 0;
    sprintf(tmp,"Aviso de control INTERNO: %d",n);
    aviso(0,tmp);
    tracer_int = 1;
    return n;
}

int que_qjes(c,flag)
int c;
int flag;
{
   int qj;
   eqj = 0;

   if (!sinescape && c == TECLAOK)
   {
        c = modo_de_validacion;
   }

   if (c >= MID_MENUSG && c < MID_DISENO) /* opcion de menu pulsada */
   {
      qj = 1;
      eqj = c;
      return(qj);
   }

   if (flag)
   {
      if (c == HOME)
         c = SOLICITA_INICIO;
      else
      if (c == END)
         c = SOLICITA_FINAL;
      else
      if (flag == 2)
      {
         if (c == DEL)
            c = SOLICITA_SUPRESION;
         else
         if (c == INS)
            c = SOLICITA_INSERCION;
      }
   }
   switch (c) {
   case SOLICITA_FINAL:
   case SOLICITA_INICIO:
   case SOLICITA_INSERCION:
   case SOLICITA_SUPRESION:
   case SOLICITA_SCROLLUP:
   case SOLICITA_SCROLLDOWN:
   case SOLICITA_POSICION:
   case SOLICITA_VALIDACION:
   case SOLICITA_ANULACION:
   case SOLICITA_AVANCE:
   case SOLICITA_RETROCESO:
   case TECLA_CHECK:
   case TECLA_DOBLECLICK:
   case 999:
      qj = c;
      break;
   case BTAB:
   case ARRIBA :
      qj = 2;
      break;
   case ABAJO :
   case FTAB:
      qj = 3;
      break;
   case TECLAOK :
	  if (sinescape)
	  {
		  qj = TECLAOK;
		  break;
	  }
   case VALIDAR :
   case CRETURN :
      qj = 0;
      break;
   case ACABAR :
   case ESC :
      qj = 1;
      break;
   case ABORTAR :
   case CTRLC :
      qj = 7;
      break;
   case PGUP :
      qj = 4;
      break;
   case PGDN :
      qj = 5;
      break;
   case CONSULTAR:
      qj = 6;
      break;
   case LISTADO:   
      qj = 6;
      eqj = 1;
      break;
   case TECLA_COMBO:   
      qj = 6;
      eqj = 2;
      break;
   case AYUDA:
   case _AYUDA:
      qj = 8;
      break;
   default :
      if (c >= EXTRATECLA)
	  {
		  if (((unsigned int)c) > 62000) {
			if ((c/100000) != 1 || (c%10000) >= 999)
				qj = c;
			else
				qj = -1;
		  }
		 else
		 {
			qj = 9 + (c - EXTRATECLA);
		 }
	  }
      else
         qj = -1;
   }
   return(qj);
}


int confirmar(int pos,char *mensa,char *defec)
{
    int d;
    do
    {
      d = saca_ptec();
      if (d == -1) break;
      if (d == 802)
      {
          if (*defec == 'S' || *defec == 's')
              return 0;
          return -1;
      }
      if (d == 806 || d == 807)
          return -1;
      if (d == 'S' || d == 's')
          return 0;
      if (d == 'N' || d == 'n')
          return -1;
    } while(-1);

    if (DespachadorExterno)
    {       
         char tmp[512];
         int *p1 = (int *)tmp;
         tmp[sizeof(int)*2] = *defec;
         strcpy(tmp+sizeof(int)*2+1,mensa);
         p1[0] = sizeof(int)*2 + strlen(mensa) + 2;
         p1[1] = 0;
         (*DespachadorExterno)((int)DESPACHO_MAINCOMMAND,ID_SIONO,(void *)tmp);
		 /*agi_cola_timer();*/
         if (p1[1] != IDOK && p1[1] != IDYES)
		 {
			 /*
			 char tmp2[512];
			 sprintf(tmp2,"%d",p1[1]);
			 aviso(0,tmp2);
			 */
             return -1;         
		 }
         return 0;
    }    
    return 0;
}

int correcto()
{
    return(confirmar(2400,"¿ES CORRECTO?","N"));    
}


/*
 * sinocancelar
 * devuelve -1 = cancelar
 *           0 = no
 *           1 = si
 */
int sinocancelar(char *mensa,char defec)
{
    int d;
    do
    {
      d = saca_ptec();
      if (d == -1) break;
      if (d == 802)
      {
          if (defec == 'S' || defec == 's')
              return 1;
          if (defec == 'C' || defec == 'c')
              return 0;
          return -1;
      }
      if (d == 806 || d == 807)
          return -1;
      if (d == 'S' || d == 's')
          return 1;
      if (d == 'N' || d == 'n')
          return -1;
      if (d == 'N' || d == 'n')
          return 0;
    } while(-1);

    if (DespachadorExterno)
    {       
         char tmp[512];
         int *p1 = (int *)tmp;
         tmp[sizeof(int)*2] = defec;
         strcpy(tmp+sizeof(int)*2+1,mensa);
         p1[0] = sizeof(int)*2 + strlen(mensa) + 2;
         p1[1] = 0;
         (*DespachadorExterno)((int)DESPACHO_MAINCOMMAND,ID_SIONOCANCELAR,(void *)tmp);
         agi_cola_timer();
         if (p1[1] == IDOK || p1[1] == IDYES)
             return 1;         
         if (p1[1] == IDNO || p1[1] == IDIGNORE)
             return -1;
         return 0;
    }    
    return 0; /* No por defecto, ojo es diferente que el es correcto!! */
}

int boton_tecla(int tecla,int id,int pi,int pf,char *control)
{
	char dato[1024];   
	int *tmpxx;

    tmpxx = (int *)dato;
	memset(dato,0,sizeof(dato));
    tmpxx[0] = id;
    tmpxx[1] = 0;      
    tmpxx[2] = pi;
    tmpxx[3] = pf;
    tmpxx[4] = 1;
    tmpxx[5] = tecla;
	tmpxx[6] = tmpxx[5];
	tmpxx[7] = tmpxx[1];

	strcpy((char *)(tmpxx+8),"BOTON,");
	if (control)
	{
		strcat((char *)(tmpxx+8),control);
	}
    (*DespachadorExterno)((int)DESPACHO_MI_CONTROL,-1,(void *)dato);
	agi_cola_timer();

	return tmpxx[4];
}

int parada_(char *control,int foco)
{	
    int tecla;
    int *tmpxx;
    char dato[1024];   
	int sinboton = 0;
	int extra = 0;
    SALVA_IDS

	/*
    if (!espuro10)
		extra = 100;
	*/

    memset(dato,0,1024);
    tmpxx = (int *)dato;
    tmpxx[0] = default_id;
    tmpxx[1] = 0;      
    tmpxx[2] = extra+std_size-14;
    tmpxx[3] = extra+std_size-4;
    tmpxx[4] = 1;
    tmpxx[5] = 802;
	tmpxx[6] = tmpxx[5];
	tmpxx[7] = tmpxx[1];

	strcpy((char *)(tmpxx+8),"BOTON,");
	if (control)
	{
		if (*control == 0 && foco != -1)
		{
			sinboton = 1;
		}
		else
		{
			strcat((char *)(tmpxx+8),control);
		}
	}
	else
	{
		strcat((char *)(tmpxx+8),"Continuar");
	}    
	if (!sinboton)
	{
		(*DespachadorExterno)((int)DESPACHO_MI_CONTROL,-1,(void *)dato);	
		_leeteclaids[_nleeteclaids] = tmpxx[4];
		_nleeteclaids++;
	}

	if (foco != -1)
	{
		if (DespachadorExterno)
			(*DespachadorExterno)((int)DESPACHO_DIF_FOCOCONTROL,foco,NULL);    
	}
	else
	{
		if (DespachadorExterno)
			(*DespachadorExterno)((int)DESPACHO_DIF_FOCOCONTROL,tmpxx[4],NULL);    
	}

    tecla = leecar_modal();

	if (!sinboton)
	{
		if (DespachadorExterno)
			(*DespachadorExterno)((int)DESPACHO_DESTRUYECONTROL,tmpxx[4],NULL);
		_nleeteclaids--;
		_leeteclaids[_nleeteclaids] = -1;	
	}
    agi_cola_timer();


    REPON_IDS
    return tecla;
}

int dame_id_botontecla(int nivel)
{
	if ((_nleeteclaids-nivel) <= 0 || nivel >= 128)
		return -1;
	return _leeteclaids[_nleeteclaids-nivel-1];
}

int parada()
{
    return que_qjes(parada_(NULL,-1));
}

/*
int menus(char *opcion[],char *letras,int pos,char *texto,int defecto,int lineas)
{

    leecar();
    return 0;
}
*/

int menug_tam(char **opcion,int numero,int pos,int defecto, int tamano)
{
    char *px,*pt;
    int *px1;
    int mas;
    int lpi,lpf;
    int f1,c1;
    int n,i,j,lado1,lado2;
    SALVA_IDS
    

    lpi = pos;   

    n = 0;
    j = 0;
    lpf = 0;
    for (i = 0;i < numero;i++)
    {
        quitab(opcion[i]);
        j = strlen(opcion[i]);
        if (lpf < j)
            lpf = j;
        n += (j+1);
    }



    f1 = pos/100;
    c1 = pos%100;
    if (f1 <= 0)
      f1 = (28-numero)/2;
    if (c1 <= 0)
      c1 = ((std_size%100)-lpf)/2+2;  

    if (numero < tamano)
        j = numero;
    else
        j = tamano;         


    /*    
    lado1 = -8;
    lado2 = 5;
    */
    lado1 = -8;
    lado2 = 2;


    lpi = (f1-2)*100+c1+lado1;
    /*((numero <= tamano) ? 0 : 1)*/
    lpf = (f1+j+1)*100+c1+lpf+lado2;
    

    no_cola_timer = 0;
    pushv(lpi,lpf);
    zonablan(lpi,lpf);
    
    mas = w_tam_stdid() + sizeof(int)*12 + n + 1;

    px = malloc(mas);

    px1 = (int *)px;
    pt = px;

    memcpy(pt,&mas,sizeof(int));
    pt += sizeof(int);

    pt = w_copia_stdid(pt);

    mas = 6; /* LISTBOX */
    memcpy(pt,&mas,sizeof(int));
    pt += sizeof(int);
    
    memcpy(pt,&lpi,sizeof(int));
    pt += sizeof(int);
    
    memcpy(pt,&lpf,sizeof(int));
    pt += sizeof(int);

    i = (int)Patrib;
    memcpy(pt,&i,sizeof(int));
    pt += sizeof(int);

    memcpy(pt,&PCcolor,sizeof(int));
    pt += sizeof(int);

    memcpy(pt,&PFcolor,sizeof(int));
    pt += sizeof(int);


    i = MID_MENUSG;
    memcpy(pt,&i,sizeof(int));
    pt += sizeof(int);

    memcpy(pt,&defecto,sizeof(int));
    pt += sizeof(int);

    memcpy(pt,&numero,sizeof(int));
    pt += sizeof(int);

    for (i = 0;i < numero;i++)
    {
        strcpy(pt,opcion[i]);
        pt += (strlen(opcion[i])+1);
    }

    *pt = 0;
    pt++;

	if (DespachadorExterno)
		(*DespachadorExterno)((int)DESPACHO_DIF_CCONTROL,0,px);

        
    if (DespachadorExterno)
		(*DespachadorExterno)((int)DESPACHO_DIF_FOCOCONTROL,px1[10],NULL);    

    no_cola_timer = 0;
    mas = leecar_modal();

	if (DespachadorExterno)
		(*DespachadorExterno)((int)DESPACHO_DESTRUYECONTROL,px1[10],NULL);

    free(px);

    popv(0);

    REPON_IDS

    if (mas >= MID_MENUSG && mas < (MID_MENUSG+numero))
    {
        mas -= MID_MENUSG;
        mas++;
    }
    else
    if (mas == 807)
        mas = -1;
    else
    if (mas == 806)
        mas = 0;

    return mas;
}

int menug(char **opcion,int numero,int pos,int defecto)
{
return menug_tam(opcion,numero,pos,defecto,10);
}

int prepara_menusg_botones()
{
    return 0;
}

int fin_botones_()
{
    return 0;
}


int _fin_botones_(int **fijo,FI_DEF *pfi)
{
	if (fijo && *fijo)
	{
		int *b_wids = *fijo;
		int i;
		for (i = 0;b_wids[i] != -1;i++)
		{
			if (DespachadorExterno)
			{       
			   (*DespachadorExterno)((int)DESPACHO_DESTRUYECONTROL,b_wids[i],NULL);         
			}
		}
		free(b_wids);
		*fijo = NULL;
	}
	if (pfi)
	{
		int i;
		for (i = 2;i < 31;i++)
		{
			pfi->f_controles[i] = 0;
		}
	}
    return 0;
}

int strlen_boton(char *p)
{
	int n = strlen(p);
	char *p1 = strstr(p,"{{");
	if (p1)
	{
		n -= 2;
		p1 += 2;
		while(*p1 && *p1 != '}' && *(p1+1) != '}') 
		{
			n--;
			p1++;
		}
		if (*p1)
			n--;
	}
	return n;
}

int _menusg(opcion,letras,pos,texto,defecto,fijo,pfi)
char *opcion[];    /* texto de las opciones */
char *letras;      /* caracteres de acceso a las opciones */
int pos;           /* posicion para pintar las opciones */
                   /* si es 0 se calcula automaticamente */
                   /* centrado en la linea 24 */
char *texto;       /* texto indicativo */
int defecto;       /* opcion por defecto >0 y <=numero */
                   /* en caso contrario toma 1 */
int **fijo;
FI_DEF *pfi;
{
	static int nivel = 0;
    int mas,i,k,tmpi,tl,n,x,j,l,ya;
    char *p,*pt;
    int *b_wids;
    int op,tot,mmax,partido = -1,pospartido;
    SALVA_IDS



	nivel++;

    if (pos <= 0) pos = 2400;


	if (std_size == -1)
		mmax = 80;
	else
		mmax = std_size%100;
	if (!espuro10 && mmax < 80)
		mmax = 80;

    tl = 0;
    for (i = 0;letras[i];i++)
    {
        pt = opcion[i];
        while(*pt && *pt == ' ') pt++;
		x = strlen_boton(pt);
		if ((tl+x) > mmax) {
			partido = i;
			break;
		}
        tl += x;
        while(*pt) pt++;
        pt--;
        while(pt > opcion[i] && pt == ' ') 
        {
            tl--;
            pt--;
        }
    }
    tot = strlen(letras);
	
	



    if (!(pos%100)) 
    {
        i = (((std_size%100)-tl)/2);
        if (i < 1) i = 1;
        pos += i;
    }


	if (partido > -1) {
		pospartido = pos+tl+100;
		for (i = partido;i < tot;i++) {
			pospartido -= strlen_boton(opcion[i]);
		}
	}


	if (!fijo || !*fijo)
	{
		b_wids = (int *)malloc(sizeof(int)*(strlen(letras)+2));
		memset((void *)b_wids,0xff,sizeof(int)*(strlen(letras)+2));

		for (i = 0;letras[i];i++)
		{
		  mas = w_tam_stdid() + sizeof(int)*13 + strlen(opcion[i])+2;

		  p = malloc(mas);  
		  pt = p;

		  memcpy(pt,&mas,sizeof(int));
		  pt += sizeof(int);

		  /*pt = w_copia_stdid(pt);*/
      
		  tmpi = -1;
			memcpy(pt,(char *)&tmpi,sizeof(int));
			pt += sizeof(int);
			if (pfi)
			{
				default_id = pfi->f_id;				
                default_wid = -1;              
                default_sid = 0;
			}
			memcpy(pt,(char *)&default_id,sizeof(int));			
			pt += sizeof(int);
		    tmpi = 0;
			memcpy(pt,(char *)&tmpi,sizeof(int));
			pt += sizeof(int);      


		  mas = 3; /* boton!! */
		  memcpy(pt,&mas,sizeof(int));
		  pt += sizeof(int);


		  if (!i)
		  {
			  k = pos;
		  }
		  else {
			  if (i == partido)
				  k = pospartido;
			  else
			      k++;
		  }


		  memcpy(pt,&k,sizeof(int));
		  pt += sizeof(int);

		  n = strlen_boton(opcion[i])-1;
		  if (tl > 99)
		  {
			  n = (n * 99) / tl;
		  }
		  k += n;

		  memcpy(pt,&k,sizeof(int));
		  pt += sizeof(int);


			j = (int)Patrib;
			memcpy(pt,&j,sizeof(int));
			pt += sizeof(int);

			memcpy(pt,&PCcolor,sizeof(int));
			pt += sizeof(int);

			memcpy(pt,&PFcolor,sizeof(int));
			pt += sizeof(int);


		  /* tecla respuesta */
		  x = MID_MENUSG + i;

		  memcpy(pt,&x,sizeof(int));
		  pt += sizeof(int);

		  /* cid */
		  memcpy(pt,&x,sizeof(int));
		  pt += sizeof(int);

		  /* ref */
		  tmpi = ((nivel << 16) & 0xffff0000);
		  memcpy(pt,&tmpi,sizeof(int));
		  pt += sizeof(int);

		  l = 0;
		  ya = 0;
		  for (j = 0;opcion[i][j];j++)
		  {
			  if (!ya && opcion[i][j] == letras[i])
			  {
				  pt[l] = '&';
				  l++;
				  ya = 1;
			  }
			  pt[l] = opcion[i][j];
			  l++;
		  }
		  pt[l] = 0;
		  quitab(pt);
		  quitabp(pt);

		  pt -= (sizeof(int)*3);

		  if (DespachadorExterno)
		  {       
			 (*DespachadorExterno)((int)DESPACHO_DIF_CCONTROL,0,p);
			 b_wids[i] = *((int *)pt);
			 if (pfi)
			 {
				 pfi->f_controles[i+2] = b_wids[i];
			 }
		  }

		  free(p);

		}

		if (fijo)
		{
			*fijo = b_wids; 
		}
	}
	else
	{
	     b_wids = *fijo; 
		 i = tot;
	}

    defecto--;
    if (defecto < 0 || defecto >= i)
        defecto = 0;

    op = defecto;
    defecto = -1;
    do
    {
        if (DespachadorExterno)
        {       
			(*DespachadorExterno)((int)DESPACHO_DIF_FOCOCONTROL,-1,NULL);
		}
        /*if (op != defecto)*/
        {
            if (op < 0) op = 0;
            if (op >= tot) op = tot-1;
            defecto = op;            
            if (DespachadorExterno)
            {       
               (*DespachadorExterno)((int)DESPACHO_DIF_FOCOCONTROL,b_wids[defecto],NULL);
            }
        }
		if (pfi)
		{
			n = leecar_foco(1,pfi->f_id,-1,-1,-1,-1,b_wids[defecto],NULL,NULL);
		}
		else
		{
			n = leecar_modal();
		}
		if (n == 999 && pfi)
		{
             int otro_campo = default_cid & 0xffff;
			 if (default_refcid == pfi->f_ipfi)
			 {
				 if (pfi->f_camporden[otro_campo] != -1)
				 {
					_ID_Cursor =  pfi->f_ipfi + (pfi->f_camporden[otro_campo]+1)*1000;
					n = SOLICITA_POSICION;
					break;
				 }			
			 }
		}
        if (n >= MID_MENUSG && n < MID_DISENO || n == 807 || n == 806) break;
        if (n == 802)
        {
            n = MID_MENUSG + defecto;
            break;
        }
        if (n >= 'a' && n <= 'z')
			n -= 32;
        if (n >= 'A' && n <= 'Z')
        {
            for (i = 0;letras[i];i++)
            {
                if (letras[i] == n)
                {
                    defecto = i;
                    break;
                }
            }
            if (defecto == i) 
            {
                n = MID_MENUSG + defecto;
                break;
            }
        }
        switch(n)
        {
        case 808:        
        case 810:
        case 805:
            op--;
            if (op < 0)
            {
                op = tot-1;
            }
            break;
        case 804:
        case 809:
        case 811:
            op++;
            if (op >= tot)
            {
                op = 0;
            }
            break;
        }

    } while(-1);

	if (!fijo)
	{
		for (i = 0;letras[i];i++)
		{
			if (DespachadorExterno)
			{       
			   (*DespachadorExterno)((int)DESPACHO_DESTRUYECONTROL,b_wids[i],NULL);         
			   if (pfi)
			   {
					 pfi->f_controles[i+2] = 0;
			   }
			}
		}
		free(b_wids);
	}

    agi_cola_timer();

	nivel--;

    REPON_IDS

    if (n == SOLICITA_POSICION)
		return n;

    if (n == 807) return -1;
    if (n == 806) return 0;

    if (n >= MID_MENUSG && n < MID_DISENO) /* opcion de menu pulsada */
    {
        return ((n - MID_MENUSG) + 1);
    }

    return 0;
}


int menusg(opcion,letras,pos,texto,defecto)
char *opcion[];    /* texto de las opciones */
char *letras;      /* caracteres de acceso a las opciones */
int pos;           /* posicion para pintar las opciones */
                   /* si es 0 se calcula automaticamente */
                   /* centrado en la linea 24 */
char *texto;       /* texto indicativo */
int defecto;       /* opcion por defecto >0 y <=numero */
                   /* en caso contrario toma 1 */
{
	return _menusg(opcion,letras,pos,texto,defecto,NULL,NULL);
}

/*****************************************!!!!*************************************************/



/*********** RENTRA ******************/
static int numblan(s)
char *s;
{
   int i=0;
   
   while (*s++ == ' ')
      i++;
   return(i);
}

static char *llenastr(i,r,s)
int i;
char *r,*s;
{
   int j=0;
   while ((*s++ = *r++) != '\0' && j<i)  /* poner en s el string r */
      j++;
   s--;
   while (j++ < i)
      *s++ = '\040';    /* rellenar con blancos */
   *s++='\0';
   return(s-j);
}

static int bienstring(qo,qi)
char *qo,*qi;
{
   int lo=strlen(qo);
   int li=strlen(qi);
   int i;
   
   if (li == 0)
      return(0);        /* no hay restriccion a qo */
   if (lo == 0) {
	   if (strstr(qi," "))
		   return 0;
	   return -1;
   }
   for (i=0;i<li;i += lo) {
      if (memcmp(qo,qi+i,lo) == 0)
         return(0);
   }
   return(-1);
}

static int biennumero(qo,qi,qs,decimal)
char *qo,*qi,*qs;          /* numero,minimo,maximo (como strings) */
int decimal;               /* numero de decimales */
{
   double tmin,tmax,tnum,vmin,vmax;
   int l = -1;
   int qc;
   int yacoma = 0;
   
   qc = 0; /* strlen mas posicion de la coma */
   while(qo[qc]) {
						if (qo[qc] == ',') {
                     l = qc;
                     qo[qc] = '.';
                  }
                  if (qo[qc] == '.') {
                     if (!yacoma)
                        yacoma = 1;
                     else {
                        if (l > -1) qo[l] = ',';
                        return(-1);
                     }
                  }
                  qc++;
   }
   
   tmin = rwatof(qi);
   tmax = rwatof(qs);
   
   tnum = rwatof(qo);
   
   vmin = min(tmin,tmax);    /* valor minimo */
   vmax = max(tmin,tmax);    /* valor maximo */
   if ((tnum<vmin) || (tnum>vmax)) {/* no esta entre los limites */
						if (l > -1) qo[l] = ',';
                  return(-1);
   }
   return(0);
}

static int chequea(tipo,qo,qi,qs,decimal,maxqc)
int tipo,decimal;
char *qo,*qi,*qs;
int maxqc;
{
   if (tipo == 1)          /* string */
      return(bienstring(qo,qi));
   else if (tipo == 2)     /* fecha */
      return(bienfecha_x(qo,maxqc));
   else if (tipo == 3)     /* numero */
      return(biennumero(qo,qi,qs,decimal));
   else
      return(0);
}

int es_clave_primaria(FI_DEF *pfi,int campo)
{
    if (campo < 0 || campo > pfi->f_ncampos || !pfi->f_nclaves) return 0;
    if (pfi->f_campos[campo].c_key == 1) return 1;
    return 0;
}

int pantalla_segun_campo(FI_DEF *pfi,int campo)
{
    int numpan = 0;
    int i,ccc = pfi->f_camporden[campo];

    for (i = 0;i < pfi->f_npanta;i++)
    {
        if ( (ccc+1) >= pfi->f_pantalla[i].p_campoini && 
             (ccc+1) <= pfi->f_pantalla[i].p_campofin    )
        {
            numpan = i;
            break;
        }                    
    }

    return numpan;
}


/*int campo_entra(int wid)*/
int entra_cedit(FI_DEF *pfi,int campo,int npan,int flag,int ipfi,int situar,char *qo,int faclin,int di,int ds,int vacio)
{
  int wid;
  int id;
  int sid;
  int tecla = 807;
  int qj,maxqc,i;
  /*char *p;*/
  int lineal = 0;
  int cwid = -1;
  char p[10000];
  char tmp[512];
  int entrado = 0;
  char *pentrado = NULL;
  int *p1 = NULL;
  int control_creado = 0;  
  int grabamemo = 0;  
  char *codigocampo = NULL;
 
  
  


cedit_repite_todo:

  if ( (pfi->f_campos[campo].c_mante.c_lineal & FGV_C_GRID)  &&
       pfi->f_idgrafico > -1 &&
       pfi->f_campogids && pfi->f_campogids[campo] != 0xffffffff)
  {       
       id  = -1;
       wid = -1;
       sid = -1;
       lineal = 1;
  }
  else
  {
	      
		  if (faclin && !(faclin%100) && !(pfi->f_campos[campo].c_mante.c_lineal& C_LINEAL))
		  {
			  /*faclin = 100;*/
			  faclin = 0;
		  }		  


		  if (di != -1)
		  {
			  wid = -1;
			  id = di;
			  sid = ds;
		  }
		  else
		  {

			  if (!faclin && npan < 0 || npan >= pfi->f_npanta)
			  {
				  npan = pantalla_segun_campo(pfi,campo);
			  }

			  if (npan < 0 || pfi->f_pantaids[npan] == -1)
			  {
				  wid = default_wid;
				  id  = default_id;
				  sid = default_sid;
			  }
			  else
			  {
				  wid = pfi->f_pantaids[npan];
				  id  = pfi->f_id;
				  sid = pfi->f_pantasids[npan];
				  if (situar && wid != -1) {
					  if (DespachadorExterno)
					  {     
						  char tmpxx[512];
						  memcpy(tmpxx,&wid,sizeof(wid));
						  (*DespachadorExterno)((int)DESPACHO_PANTALLAS,10999,(void *)tmpxx);
					  }
				  }
			  }
		  }
  }

  p1 = NULL;
  if (pfi->f_campos[campo].c_tipo == NULOTIPO)
  {
	  cwid = pfi->f_campowids[ campo ];
	  if (DespachadorExterno)
	  {     
		(*DespachadorExterno)((int)DESPACHO_DIF_FOCOCONTROL,cwid,NULL);
	  }
	  control_creado = 1;
  }
  else
  {	  

	  {
			  int modif;
			  if (faclin)
			  {
				  modif = pfi->f_campos[campo].c_mante.c_modif;
				  if (vacio == -1 || situar)
					 vacio = 0;
				  else
					 pfi->f_campos[campo].c_mante.c_modif = 1;
				  cwid = pfi->f_campowids[campo];
				  pfi->f_campowids[campo] = -1;
			  }

			  crea_control_edicion(pfi,campo,wid,id,sid,lineal,faclin,vacio,p);
			  if (/*situar || */pfi->f_campowids[campo] == -1 || CambiaElCampo(pfi,campo,vacio))
			  {			  
				  control_creado = 1;
			  }		  

			  if (faclin)
			  {          
				  pfi->f_campos[campo].c_mante.c_modif = modif;
				  pfi->f_campowids[campo] = cwid;
			  }
	  }

	  flag <<= 1;
	  flag |= situar;

	  cwid = pfi->f_campowids[ campo ];

	  p1 = (int *)p;
	  if (DespachadorExterno && control_creado)
	  {     
		 if ((!lineal && !faclin) || !(pfi->f_campos[campo].c_mante.c_modif))
		 {
			 if (!situar && cwid != -1)
			 {
				 flag |= 256;
			 }
			 else
			 {
				 flag &= ~256;
			 }
		 }
		 else
		 {
			 if (situar)
			 {
				 flag &= ~256;
			 }
			 else
			 {
				 flag |= 256;
			 }
		 }
		 /*if (situar)*/
		 {
			(*DespachadorExterno)((int)DESPACHO_DIF_CEDIT,flag,p);
		 }
		 /*
		 else
		 {
			(*DespachadorExterno)((int)DESPACHO_CEDIT,flag,p);
		 }
		 */
		 if (!(flag & 256))
		 {
			 cwid = p1[10];
			 if ((!lineal && !faclin) || !(pfi->f_campos[campo].c_mante.c_modif))
			 {				
				pfi->f_campowids[ campo ]  = cwid;
			 }
		 }
	  }

  }

  /*free(p);*/
cedit_repite:
  if (cwid != -1 /*control_creado*/)
  {

	  while(situar)
	  {
		entrado = 0;

		default_sid = sid;
		tecla = saca_ptec();
		if (tecla != -1)
		{
			qj = que_qjes(tecla,0);
			if (qj < 0 && qj != tecla)
			{
				char tmpxx[512];
				int *p01 = (int *)tmpxx;
				memset(tmpxx,0,sizeof(tmpxx));
				p01[0] = sizeof(int)*3;
				p01[1] = ID_PTECS;
				p01[2] = tecla;
				if (DespachadorExterno)
					(*DespachadorExterno)((int)DESPACHO_COMMANDCONTROL,cwid,(void *)tmpxx);
				continue;
			}
			else
			{
				tecla = qj;
			}
		}
		else
		{	
			codigocampo = NULL;			
			pentrado = NULL;			
			
			if (cwid != -1 && pfi->f_campos[campo].c_tipo != NULOTIPO /*&& p1[4] != 7 && qo */&& situar && !lineal)
			{				
				if (p1 && p1[4] == 7)
				{					
					*((int *)tmp) = 40;
					tmp[sizeof(int)*2] = 0;
					pentrado = tmp;
					entrado = 1;														
				}
				else if (qo)
				{					
					*((int *)tmp) = MAXDEFECTO + sizeof(int) * 2 + 1;
					tmp[sizeof(int)*2] = 0;
					pentrado = tmp;
					entrado = 1;												
				}				
			}			

			if (/*pfi->f_campos[campo].c_lista[0] != 'C' || strlen(pfi->f_campos[campo].c_lista) != 4 &&*/ pfi->f_campos[campo].c_tipo == CHARTIPO && pfi->f_campos[campo].c_long > 1)
			{
				
				if (auto_completar == 1)
				{
					if (pfi->f_campos[campo].c_lista[0] != 'C')
						codigocampo = pfi->f_campos[campo].c_lista;
				}
				else if (auto_completar == 2)
				{
					sprintf(buffer_elcodigo,"%-8.8s%03d",quita_path(pfi->f_fichero),campo);
					codigocampo = buffer_elcodigo;
				}								
			}


			grabamemo = 0;
 			tecla = que_qjes(leecar_foco(1,pfi->f_id,-1,-1,-1,pfi->f_ipfi,cwid,&pentrado,codigocampo),0);
			if (!pentrado)
			{
				entrado = 0;
			}
			if (auto_completar && codigocampo)
			{
				if (tecla == 0 || tecla == 1)
				{
					if (entrado)
					{						
						if (p1 && p1[4] == 7)
						{
							grabamemo = 1;			
						}
						else					
						{
							GrabaAutoCompletar(codigocampo,tmp+sizeof(int)*2,tmp+sizeof(int)*2,0);
						}
					}
				}
			}				
			if (p1 && p1[4] == 7)
			{				
				entrado = 0;
			}			
		}
        
		if (tecla == 999)
		{
			if (default_id != id  || default_refcid != pfi->f_ipfi)
			{
				/* no vale!!*/
				if (DespachadorExterno)
					(*DespachadorExterno)((int)DESPACHO_DIF_FOCOCONTROL,cwid,NULL);
				continue; 
			}
			if ((default_sid == sid || default_sid == (sid%1000))/*&& (default_ctipo == 2 || default_ctipo == 7 || default_ctipo == 4)*/ && (default_cid & 0xffff) == campo)
			{
				continue; /* es la confirmacion de foco */
			}
		}
		break;
	  } 

  
	  if (pfi->f_campos[campo].c_tipo != NULOTIPO)
	  {
		  if (p1 && p1[4] == 7)
		  {
			  if (situar)
			  {
				vacia_campo_texto(pfi,campo,cwid,grabamemo,codigocampo);
			  }
		  }
		  else if (DespachadorExterno && qo && situar)
		  {			  

			  if (lineal)
			  {
				  long tam;
				  long modo = 1;
				  int id_grid = pfi->f_idgrafico;          
				  char *p;

				  memset(tmp,0,sizeof(tmp));
				  tam = (sizeof(long)*3)+sizeof(int)+300;
				  memcpy(tmp,&tam,sizeof(long));
				  memcpy(tmp+sizeof(long),&tam,sizeof(long));
				  memcpy(tmp+(sizeof(long)*2),&modo,sizeof(long));
				  memcpy(tmp+(sizeof(long)*3),&id_grid,sizeof(int));
				  p = tmp+(sizeof(long)*3)+sizeof(int)*2;
				  (*DespachadorExterno)((int)DESPACHO_BINARIO,(int)6,tmp);
				  strcpy(qo,p);
			  }
			  else      
			  {				  

				  if (!entrado)
				  {
					  *((int *)tmp) = MAXDEFECTO + sizeof(int) * 2 + 1;
					  tmp[sizeof(int)*2] = 0;				  
					  (*DespachadorExterno)((int)DESPACHO_GETDATACONTROL,cwid,tmp);
				  }
				  strcpy(qo,tmp+sizeof(int)*2);
			  }

			  {
				  char *qs = pfi->f_campos[campo].c_maximo;
				  char *qi = pfi->f_campos[campo].c_minimo;
				  char *qd = pfi->f_campos[campo].c_mante.c_defecto;
				  int tipo = 1;
				  int decimal = 0,decmin,decmax,c;

				  maxqc = pfi->f_campos[campo].c_qc;
				  if (maxqc > 0)
				  {
					  tipo=1;                   /* string */
				  }
			      else if (maxqc==-1) 
				  {   
					 tipo=2;                   /* fecha dd.mm.aaaa */   
				  }
				  else if (maxqc == 0) 
				  {
					 tipo=3;                   /* numero */
					 i = strlen(qi);
					 if (i > 1 && qi[i-2] == '.' && qi[i-1] == '*') 
					 {
						decimal = -1;
					 }
					 else 
					 {
						decmin = decimales(qi);   /* numero de decimales de qi */
						decmax = decimales(qs);
						decimal = max(decmin,decmax); /* numero de decimales posibles */
					 }
					 maxqc = len_numerico(qi,qs);
					 c = numblan(qd);
					 qd += c;
				  }      


				  if (tecla != 7)
				  {
					if (chequea(tipo,qo,qi,qs,decimal,maxqc) == -1) 
					{
					   pito(43);               
					   if (tipo == 2)
					   {
						   if (atoi(qd) == 0) 
						   {
							  strcpy(qo,fecha());
						   }
						   else
							  strcpy(qo,qd);						   
						   if (lineal)
						   {
							   if (DespachadorExterno && control_creado)
							   {
								 (*DespachadorExterno)((int)DESPACHO_DESTRUYECONTROL,cwid,NULL);
								 pfi->f_campowids[ campo ]  = -1;
							   }	   
							   control_creado = 0;
							   st_campo(campo,pfi,qo);						   
							   goto cedit_repite_todo;
						   }
					   }
					   else
					      llenastr(maxqc,qd,qo);  /*  qo=qd+blancos  */
					   strcpy(tmp+5,qo);
					   memcpy(tmp,"DATA:",5);
					   quitab(tmp);
					   if (DespachadorExterno)
							(*DespachadorExterno)((int)DESPACHO_ESTADOCONTROL,cwid,(void *)tmp);
					   goto cedit_repite;
					}            		    					
				  }
				  if (maxqc > 0)
				  {
					 for (i = strlen(qo);i < maxqc;i++)
					 {
						 qo[i] = ' ';
					 }
					 qo[i] = 0;

					  if (maxqc > 0 && *qs)
					  {
						  unsigned char relleno = ' ';      

						  i = strbusca(qs,"\\");
						  if (i >= 0)
						  {
							  i = (int)((unsigned char)qs[i+1]);
							  if (i)
							  {
								  relleno = (unsigned char)i;
							  }
						  }
  
						  if (strbusca(qs,"+") >= 0)
						  {
							  int dd,c;          
      
							  quitab(qo);
							  quitabp(qo);
							  i = strlen(qo);

							  dd = maxqc-i;
							  for (c = i;c >= 0;c--)
							  {
								  qo[dd + c] = qo[c];
							  }
							  for (c = 0;c < dd;c++)
								  qo[c] = relleno;
						  }
						  else
						  {
							  if (relleno != ' ')
							  {
								  quitab(qo);
								  i = strlen(qo);
								  for (;i < maxqc;i++)
									  qo[i] = relleno;
								  qo[i] = 0;
							  }
						  }

						  if (strbusca(qs,">") >= 0)
						  {
							  strmayus(qo);
						  }
						  else
						  if (strbusca(qs,"<") >= 0)
						  {
							  strminus(qo);
						  }     
						}				  
				  }
			  }
		  }
		  else
		  {
			  if (qo)
				ld_campo(campo,pfi,qo);
		  }
	  }


	  if (lineal)
	  {      
		   if (DespachadorExterno && control_creado)
		   {
			 (*DespachadorExterno)((int)DESPACHO_DESTRUYECONTROL,cwid,NULL);
			 pfi->f_campowids[ campo ]  = -1;
		   }	   
	  }
  }


  /* ((default_sid%1000) - 1) == ipfi && */
  if (situar && tecla == 999 && /*(default_ctipo == 2 || default_ctipo == 7 || default_ctipo == 4) &&*/ default_cid != -1)
  {    
    int otro_campo = default_cid & 0xffff;
    if (default_refcid == pfi->f_ipfi)
    {
        if (pfi->f_camporden[otro_campo] != -1)
        {
            _ID_Cursor =  ipfi + (pfi->f_camporden[otro_campo]+1)*1000;
            tecla = SOLICITA_POSICION;
        }
        else
            DebugBreak_();
    }
    else
      DebugBreak_();
  }
  /*agi_cola_timer();*/

  SetAutoCompletar(NULL,-1);

  return tecla;
}

int entra(qp,maxqc,qi,qs,qd,qh,qo)
int qp,
maxqc, /* longitud real del dato */
qh;
char *qi,*qs,*qd,*qo;
{
   int cwid;   
   /* longitud en pantalla */
   int qc = qh / 10; 
   int qj,i;
   int flag_extrafuncion = qp / 10000;
   char p[1024];
   int pass = 0;
   int sid = 0;
   int wid = -1;

   if (qh == -1)
   {
       pass = 1;
   }

   qp %= 10000;
   qh %= 10;

   flag_extrafuncion <<= 1;
   flag_extrafuncion |= 1;

   strcpy(qo,qd);   

   wid = default_wid;
   sid = default_sid;

   if (!espuro10 && default_sid > 0) {
	   int ifi,ipa;
	   ifi = (default_sid%1000)-1;
	   ipa = default_sid/1000;
	   if (ifi >= 0 && ifi < nfi && ipa >= 0 && ipa < v_mpfi(ifi)->f_npanta && default_id == v_mpfi(ifi)->f_id) {
		   int pi,pf;
		   pi = v_mpfi(ifi)->f_pantalla[ipa].p_posref;
		   pf = pi + (v_mpfi(ifi)->f_pantalla[ipa].p_posfin-101);
		   if ((qp/100) > (pf/100)) {
			   wid = -1;
			   sid = 0;
		   }
	   }
   }   

   crea_entrada(wid,default_id,sid,qo,maxqc,qc,qi,qs,qp,-1,qp,qp,0,"",p,0,0,pass);

entra_repite:
   if (DespachadorExterno)
   {
     int *p1 = (int *)p;
     (*DespachadorExterno)((int)DESPACHO_CEDIT,flag_extrafuncion,p);
     cwid = p1[10];
   }

   do
   {
     qj = que_qjes(leecar_foco(1,default_id,sid,2,qp,qp,cwid,NULL,NULL),0);
     if (qj == 999 || (qj && !qh)) continue;
     break;
   } while(-1);

   if (DespachadorExterno)
   {
	  int tipo = 1;
	  int decimal = 0,decmin,decmax,c;

     *((int *)p) = MAXDEFECTO + sizeof(int) * 2 + 1;
     p[sizeof(int)*2] = 0;
     (*DespachadorExterno)((int)DESPACHO_GETDATACONTROL,cwid,p);
     strcpy(qo,p+sizeof(int)*2);


	  if (maxqc > 0)
	  {
		  tipo=1;                   /* string */
	  }
	  else if (maxqc==-1) 
	  {   
		 tipo=2;                   /* fecha dd.mm.aaaa */   
	  }
	  else if (maxqc == 0) 
	  {
		 tipo=3;                   /* numero */
		 i = strlen(qi);
		 if (i > 1 && qi[i-2] == '.' && qi[i-1] == '*') 
		 {
			decimal = -1;
		 }
		 else 
		 {
			decmin = decimales(qi);   /* numero de decimales de qi */
			decmax = decimales(qs);
			decimal = max(decmin,decmax); /* numero de decimales posibles */
		 }
		 maxqc = len_numerico(qi,qs);
		 //c = numblan(qd);
		 //qd += c;
	  }      

	if (chequea(tipo,qo,qi,qs,decimal,maxqc) == -1) 
	{
	   pito(43);               
	   if (tipo == 2)
	   {
		   if (atoi(qd) == 0) 
		   {
			  strcpy(qo,fecha());
		   }
		   else
			  strcpy(qo,qd);						   
	   }
	   else
		  llenastr(maxqc,qd,qo);  /*  qo=qd+blancos  */
       (*DespachadorExterno)((int)DESPACHO_DESTRUYECONTROL,cwid,NULL);
	   goto entra_repite;
	}            		    					

     if (maxqc > 0)
     {
         for (i = strlen(qo);i < maxqc;i++)
         {
             qo[i] = ' ';
         }
         qo[i] = 0;

	  if (maxqc > 0 && *qs)
	  {
		  unsigned char relleno = ' ';      

		  i = strbusca(qs,"\\");
		  if (i >= 0)
		  {
			  i = (int)((unsigned char)qs[i+1]);
			  if (i)
			  {
				  relleno = (unsigned char)i;
			  }
		  }
      
		  if (strbusca(qs,"+") >= 0)
		  {
			  int dd,c;          
          
			  quitab(qo);
			  quitabp(qo);
			  i = strlen(qo);

			  dd = maxqc-i;
			  for (c = i;c >= 0;c--)
			  {
				  qo[dd + c] = qo[c];
			  }
			  for (c = 0;c < dd;c++)
				  qo[c] = relleno;
		  }
		  else
		  {
			  if (relleno != ' ')
			  {
				  quitab(qo);
				  i = strlen(qo);
				  for (;i < maxqc;i++)
					  qo[i] = relleno;
				  qo[i] = 0;
			  }
		  }

		  if (strbusca(qs,">") >= 0)
		  {
			  strmayus(qo);
		  }
		  else
		  if (strbusca(qs,"<") >= 0)
		  {
			  strminus(qo);
		  }     
		}


     }

     (*DespachadorExterno)((int)DESPACHO_DESTRUYECONTROL,cwid,NULL);
	 if (!pass)
		pprint(qp,qo);

   }   

   /*free(p);*/
   
   return qj;
}



char *crea_control_texto(char *xxtmp,int tamxxtmp,int _qpi,int qpf,int area,int ref,int topelin,int cwid,int maxqc,char **texto,int flags,int maximo,int wid,int id,int sid)
{
	
	char *ppp;
	int i,j;
	int r;
	char *opp;
		

	if (!xxtmp)
	{
		tamxxtmp = sizeof(int)*13 + w_tam_stdid();
	    for (i = 0;texto[i];i++)
		{
		   if (maxqc > -1)
		   {
			   tamxxtmp += (maxqc+2);
		   }
		   else
		   {
			   tamxxtmp += (strlen(texto[i])+2);			   
		   }
		}
		tamxxtmp++;
		xxtmp = malloc(tamxxtmp);
	}

    ppp = xxtmp + sizeof(int);

	if (wid == -1 && id == -1 && sid == -1)
	{
		wid = default_wid;
		id  = default_id;
		sid = default_sid;
	}	

    memcpy(ppp,(char *)&wid,sizeof(int));
    ppp += sizeof(int);
    memcpy(ppp,(char *)&id,sizeof(int));
    ppp += sizeof(int);
    memcpy(ppp,(char *)&sid,sizeof(int));
    ppp += sizeof(int);    

    r = 7; /* EDITBOX */
    memcpy(ppp,&r,sizeof(int));
    ppp += sizeof(int);
      
    memcpy(ppp,&_qpi,sizeof(int));
    ppp += sizeof(int);
    
    memcpy(ppp,&qpf,sizeof(int));
    ppp += sizeof(int);


    j = (int)Patrib;
    memcpy(ppp,&j,sizeof(int));
    ppp += sizeof(int);

    memcpy(ppp,&PCcolor,sizeof(int));
    ppp += sizeof(int);

    memcpy(ppp,&PFcolor,sizeof(int));
    ppp += sizeof(int);


    
    memcpy(ppp,&cwid,sizeof(int));
    ppp += sizeof(int);

    memcpy(ppp,&area,sizeof(int));
    ppp += sizeof(int);

    memcpy(ppp,&ref,sizeof(int));
    ppp += sizeof(int);

    memcpy(ppp,&topelin,sizeof(int));
    ppp += sizeof(int);


    memcpy(ppp,&flags,sizeof(int));
    ppp += sizeof(int);
	  
    memcpy(ppp,&maximo,sizeof(int));
    ppp += sizeof(int);

      r = sizeof(int) * 13 + w_tam_stdid();

      ppp[0] = 0;

	  opp = ppp;	  

      for (i = 0;texto[i];i++)
      {
		 if (i >= topelin && topelin > 0)
			 break;
         if (r < (tamxxtmp -300))
         {
			if (maxqc > -1)
			{
				memcpy(ppp,texto[i],maxqc);
				ppp[maxqc] = 0;
			}
			else
			{
				strcpy(ppp,texto[i]);
			}
			
		    if (!(flags & 4))
			{
				for (j = 0;ppp[j];j++)
				{
					if (ppp[j] == 10 || ppp[j] == 13)
						ppp[j] = ' ';
				}
			}
			
			if (!(flags & 4))
			{
				quitan(ppp);
				quitab(ppp);
				quitan(ppp);					
			}
			r += strlen(ppp);
			ppp += strlen(ppp);			
			if (!(flags & 4))
			{
				*ppp = 13;
				ppp++;
				r++;				
				*ppp = 10;
				ppp++;			
				r++;				
				*ppp = 0;
			}
         }
      }
	  
	  do
	  {
		  quitan(opp);
		  quitab(opp);
		  j = strlen(opp);
	  } while(j && (opp[j-1] == 10 || opp[j-1] == 13));

      r++;      
      memcpy(xxtmp,&r,sizeof(int));

	  return xxtmp;
}



int edita_texto(tx,_qpi,qpf,topelin,titulo,c_v,tcol,ol,oc)
char ***tx;
int _qpi,qpf,topelin;
char *titulo;
int *c_v;
int tcol;
long *ol;
int *oc;
{
   int r = 0;
   int tecla = 7;
   char **fns = salva_funciones(0,8 + ((/*memo_pregu_imp*/0) ? 1 : 0));
   int modo_externo = (*c_v) / 100;
   int v;   
   char **edi_text,**texto;
   int tex_a_tope = 0,i,j;
   char xxtmp[100300];   
   char *ppp = xxtmp + sizeof(int);
   int cuenta_l;

   *c_v %= 100;

   if (tcol < 0)
   {
      tcol = (-tcol) - 1;
   }

   edi_text  = *tx;
   texto = edi_text;
   
   v = ((*c_v) ? 1 : 0);   
   
   if (topelin < 0) {tex_a_tope = 1;topelin = -topelin;}
   if (topelin > 8000) topelin = 8000;
   
   if (DespachadorExterno)
   {     
      int *px1= (int *)xxtmp;
      int cwid;
      int area = _qpi * 10000 + qpf;
      int ref  = _qpi * 10000 + qpf;
	  int flags = 1; // siempre el horizontal!!


	  if (topelin > (area%100))
		  flags += 2;

	  flags |= ((tcol << 16) & 0xffff0000);

	  crea_control_texto(xxtmp,sizeof(xxtmp),_qpi,qpf,area,ref,topelin,-1,-1,texto,flags,0,-1,-1,-1);

      for (i = 0;texto[i];i++)
      {
         free(texto[i]);
         texto[i] = NULL;
	  }
      if (!tex_a_tope)
         free(texto);

      (*DespachadorExterno)((int)DESPACHO_DIF_CCONTROL,0,xxtmp);
      cwid = px1[10];

      (*DespachadorExterno)((int)DESPACHO_DIF_FOCOCONTROL,cwid,NULL);

      tecla = que_qjes(leecar_foco(0,default_id,default_sid,7,area,ref,cwid,NULL,NULL),0);

      *((int *)xxtmp) = sizeof(xxtmp)- (sizeof(int) * 2 - 1);
      xxtmp[sizeof(int)*2] = 0;
      (*DespachadorExterno)((int)DESPACHO_GETDATACONTROL,cwid,xxtmp);

      ppp = xxtmp+sizeof(int)*2;
      
      cuenta_l = 0;      
      for (i = sizeof(int)*2;xxtmp[i];i++)
      {
         if (xxtmp[i] == 13 && (xxtmp[i+1] == 10 || xxtmp[i+1] == 13))
         {
            cuenta_l++;
         }
      }
      if (!tex_a_tope)
         texto = (char **)malloc(sizeof(char *) * (cuenta_l+1));
      
      edi_text = texto;
      cuenta_l = 0;
      j = sizeof(int)*2;
      for (i = sizeof(int)*2;xxtmp[i] && cuenta_l < topelin;i++)
      {
         if (tex_a_tope && cuenta_l >= topelin) break;

         if (xxtmp[i] == 13 && (xxtmp[i+1] == 10 || xxtmp[i+1] == 13))
         {
            xxtmp[i] = 0;
            quitab(xxtmp+j);
            texto[cuenta_l] = malloc(strlen(xxtmp+j)+1);
            strcpy(texto[cuenta_l],xxtmp+j);
            if (xxtmp[i+1] == 10)
            {
               j = i + 2;
               i++;
            }
            else
            {
               j = i + 3;
               i += 2;
            }
            cuenta_l++;
            if (!xxtmp[i]) 
                break;
         }
      }
      if (tex_a_tope)
      {
          for (;cuenta_l < topelin;cuenta_l++)
          {
              texto[cuenta_l] = malloc(1);
              texto[cuenta_l][0] = 0;
          }
      }
      texto[cuenta_l] = NULL;
      *tx = texto;      

      (*DespachadorExterno)((int)DESPACHO_DESTRUYECONTROL,cwid,NULL);
  }
      
  repon_funciones(fns,0,8 + ((/*memo_pregu_imp*/0) ? 1 : 0));      
  return(tecla);
}


int pinta_hoja_texto(texto,qpi_rt,qpf,titulo,lin,ntlin,v,tpan,tini,b1,b2,i_margen,d_margen)
char **texto;
int qpi_rt,qpf;
char *titulo;
int lin,ntlin,v,tpan,tini;
char *b1,*b2;
int i_margen,d_margen;
{
    return -1;
}

int zonablan(pi,pf)
int pi,pf;
{
    BorraDescendencia(default_wid,default_id,default_sid,pi,pf);
	if (espuro10) {
		default_sid = pi*10000+pf;
		default_wid = -1;
	}
    return 0;
}


int bi_cuadro(int pi,int pf,int modo)
{
    char *p = crea_control_estatico(pi,pf,NULL,0); /* el tipo 8 es la group box standar pero aqui usamos uno propio */

    if (DespachadorExterno)
    {       
         (*DespachadorExterno)((int)DESPACHO_DIF_CCONTROL,-1,p);         
    }

    free(p);
   
   return 0;
}

int cuadro(pi,pf)
int pi,pf;
{
   return(bi_cuadro(pi,pf,0));
}



int DebugBreak_()
{
/*    aviso(0,"DEBUG_BREAK!!!"); */
	guarda_incidencia(0,"Control de depuracion, posible anomalía interna");
 
    return -1;
}




int sw_superusuario()
{
	if (super_usuario < 2)
	{
		if (super_usuario_password() == 1)
		{
			if (!gana_super_usuario())
			{
				super_usuario = 2;
				if (DespachadorExterno)
					(*DespachadorExterno)((int)DESPACHO_SUPERUSUARIO,(int)0,NULL);
			}
			else
			{
				aviso(0,"Ya hay un Super-Usuario en el sistema");
			}
		}
	}
	else
	{					
		deja_super_usuario();
		super_usuario = 0;
		if (DespachadorExterno)
			(*DespachadorExterno)((int)DESPACHO_SUPERUSUARIO,(int)1,NULL);
	}				
	return 0;
}

int sw_euro()
{
	if (!fgv_ver_euro)
	{
		fgv_ver_euro = 1;
		if (DespachadorExterno)
			(*DespachadorExterno)((int)DESPACHO_EURO,(int)2,NULL);
	}
	else
	{					
		fgv_ver_euro = 0;
		if (DespachadorExterno)
			(*DespachadorExterno)((int)DESPACHO_EURO,(int)3,NULL);
	}
	return 0;
}

int CambiaElCampo(FI_DEF *pfi,int campo,int vacio)
{
	int offset;
	int lon;
	int check;
	char *pp;
	int r = 0;	
	char buffer[300];

	if (!pfi->f_bufcompara || campo == -1)
	{
		if (!pfi->f_bufcompara)
		{
			pfi->f_bufcompara = malloc(pfi->f_lfich+1);
			pfi->f_crccampos  = (int *)malloc((pfi->f_ncampos + 1) * sizeof(int));
			memcmp(pfi->f_bufcompara,pfi->f_buf,pfi->f_lfich);
			r = 1;
		}
		if (campo == -1)
		{
			memcpy(pfi->f_bufcompara,pfi->f_buf,pfi->f_lfich);		
			for (offset = 0;offset < pfi->f_ncampos;offset++)
			{
				pfi->f_crccampos[offset] = pfi->f_campos[offset].c_mante.c_visual;
				pfi->f_crccampos[offset] ^= pfi->f_campos[offset].c_mante.c_modif;
				pfi->f_crccampos[offset] ^= pfi->f_campos[offset].c_mante.c_lineal;
			}
			return 1;
		}
		else
		{
			memset(pfi->f_bufcompara,0xff,pfi->f_lfich);
			for (offset = 0;offset < pfi->f_ncampos;offset++)
			{
				pfi->f_crccampos[offset] = pfi->f_campos[offset].c_mante.c_visual;
				pfi->f_crccampos[offset] ^= pfi->f_campos[offset].c_mante.c_modif;
				pfi->f_crccampos[offset] ^= pfi->f_campos[offset].c_mante.c_lineal;
				pfi->f_crccampos[offset] = !pfi->f_crccampos[offset];
			}
		}		
	}
	offset = pfi->f_campos[campo].c_offset;
	lon    = pfi->f_campos[campo].c_long;

	check = pfi->f_campos[campo].c_mante.c_visual;
	check ^= pfi->f_campos[campo].c_mante.c_modif;
	check ^= pfi->f_campos[campo].c_mante.c_lineal;	
	if (pfi->f_campowids[ campo ] == -1 || pfi->f_crccampos[campo] == -1 || check != pfi->f_crccampos[campo])
	{
		pfi->f_crccampos[campo] = check;
		memcpy(pfi->f_bufcompara+offset,pfi->f_buf+offset,lon);
		return 1;
	}
	if (vacio) {
		memset(buffer,0,lon);
		if (memcmp(pfi->f_bufcompara+offset,buffer,lon)) {
			memcpy(pfi->f_bufcompara+offset,buffer,lon);
			return 1;
		}
	}
	else if (memcmp(pfi->f_bufcompara+offset,pfi->f_buf+offset,lon))
	{
		memcpy(pfi->f_bufcompara+offset,pfi->f_buf+offset,lon);
		return 1;
	}
	if ((pp = strstr(pfi->f_campos[campo].c_maximo,"[")))
	{
		int k;
		int maxqc;
		maxqc = pfi->f_campos[campo].c_qc;
		for (k = campo + 1;k < pfi->f_ncampos;k++)
		{
			if (pfi->f_campos[k].c_qc != maxqc || !pfi->f_campos[k].c_mante.c_visual)
			{
				break;
			}
			offset = pfi->f_campos[k].c_offset;
			lon    = pfi->f_campos[k].c_long;
			if (memcmp(pfi->f_bufcompara+offset,pfi->f_buf+offset,lon))
			{
				memcpy(pfi->f_bufcompara+offset,pfi->f_buf+offset,lon);
				r = 1;
			}
		}		
	}
	return r;
}

#ifndef FGVDOS

#include <sys/times.h>


long GetTickCount()
{
    /*tms tm;*/
	char tmp[1024];
    return times(tmp/*&tm*/);
}

#endif
