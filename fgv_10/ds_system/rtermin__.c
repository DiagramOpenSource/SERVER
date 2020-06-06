/*---------------------------------------------------------------------------
* Programa ..: rtermin.c
* Fecha .....:
* Version ...: 4.00 UNIFICADA por  Francisco Gonzalez Valiente
* Notas .....: Rutinas de tratamiento de terminales y ventanas.
* Contenido .: VENT *w_ini();
*              void w_refresca(), w_fin(), w_salva(), w_pinta(), w_mueve(),
*                   w_putc(), w_cls(), w_ll(), cursor(), cls(), ll(), pito(),
*                   Ar(), Cr(), Ai(), Bi(), Ap(), Cp(), As(), Cs(), Gon(),
*                   Goff(), Graf(), atributo(), Terinici(), Terfinal(), Bl(),
*              int  w_saca(), w_mete(), w_lee(), w_print(), w_printd(),
*                   w_prints(), w_printc(), w_printl(), w_printF(), v_print(),
*                   v_printd(), v_prints(), v_printc(), v_printl(),
*                   v_printF(), pprint(), leech(), leecar(), curon(),
*                   pantalla(),  w_graba(),
*                   w_cambia(),w_marco(),v_scroll()
*---------------------------------------------------------------------------*/

#ifdef _DEBUG
#pragma check_pointer( off )
#endif


#define CONSOLANTINPUT

#include <fgvsys.h>

#include <stdio.h>
#ifdef FGVWIN
#include <windows.h>
#include <windowsx.h>
#endif

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef ESDOS
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

#include <string.h>
#ifdef ESDOS
#include <conio.h>
#include <ctype.h>
#define __MSC
#include <dos.h>
#ifdef _MSC_VER
#ifndef FGV32
#include <bios.h>
#endif
#endif
#endif
#include <errno.h>
#include <malloc.h>
#include <memory.h>
#include <signal.h>
#include <time.h>
#include <rwmalloc.h>
#define _R_TERMIN
#include <rtermin.h>
#include <teclas.h>


#ifdef FGVWIN
#include <FGVWIN.h>
extern char NombreClase[81];
int en_lee_teclado = 0;
#endif

int NO_PINTAR_AHORA = 0;

#ifdef FGV32

extern BOOL fIsTNT;

#ifndef FGVWIN
#ifndef WATCOMNLM
extern HANDLE myconsola;
extern HANDLE hStdIn;
#endif
#endif

#endif

#ifdef FGVDOS
unsigned long nt_version = 0L;
#endif

#ifdef RWXARXAIDX
int _en_conexion_server_ = 0;
int _en_conexion_index_ = 0;
#endif


void (*DespachadorExterno)() = NULL;

char other_server[128] = "";
char server_request[128] = "";
char segundo_dir[128] = "";
int  server_id = 0;

short limitacion_especial = 0;
int   mimodo_debug = 0;

#ifdef RWXARXA

int en_conexion_terminal = 0;
int leecar_terminal_puro = 0;

#ifdef RWSERVER
int leecar_server_puro = 0;
int en_conexion_externa = 0;
int en_conexion_externa_stop = 0;
int remoto_ver1 = -1;
int remoto_ver2 = -1;
unsigned char remoto_verletra  = 0;
int remoto_que_sistema_es = -1;
int remoto_es_sistema_grafico = -1;
#endif
#endif


int fgv_activa_impredll = 1;
int fgv_activa_dragdrop = 1;
int fgv_activa_progdll = 1;

int data_hiden_indicator = 0;
int identidad_cliente = 0;

int pendiente_de_instalar = 0;

static int no_bitblt = 0;


void w_refresca();

#ifndef ESDOS
extern char *ttyname();
extern void alarma_sig();
#else
#ifdef FGV_MAXDOS
#ifndef FGV32
unsigned char *direc_ppanta = NULL;
#endif
#endif
#endif

#ifdef FGVWIN
#define dosprint(a,b) w_winprint(a,b,0)
#endif


unsigned long NumeroDeSerie = 0L;


extern short _fgv_version = 6;
extern short i3 = 11;
extern short _salida2 = 0;

extern int os_subtipo;

extern int no_especial;

int modo_desatendido = 0;

int quien_soy1 = 0; /* nume_code    */
int quien_soy2 = 0; /* nume_mochila */


int *sitecla_ = NULL;

int modo_de_validacion = CRETURN;

long _programada_alarma = 0L;

extern short _si_escrola = 1;

int data_fecha_conmutar = 0;

int llama_fun_int();

int win_fondo_neutro = 0;

char R_midir[256] = "";

int que_sistema_es = QUE_SISTEMA_SOY;

#ifdef FGVWIN
int es_sistema_grafico = 1;
#else
int es_sistema_grafico = 0;
#endif

int resta_dias_log = 30;

/* ojo acordarse de hacerlo coincidir!!!*/
int rtme_version1 = 9;
int rtme_version2 = 10;
unsigned char rtme_versionletra = 66;
int numero_build = 1;
char rw_version[256] = "DIAGRAM 9.10.066-1                                                ";

void Gon();
void Goff();
char dirbin[51];
char dirdef[51];
char dirpan[51];
char dirfic[51];
char nomfdir[51];
char dirficb[51];
char dirbass[51];
char dirbase[51];
char nomapli[25];
char nmen0[51];
char par1[51];
char par2[51];
char modulo[9] = {0,};

char no_instala[30];
int no_instala_enbucla = 0;

char rw_user[11];
int user_who = -1;
int subnumero_de_usuario = 0;
int super_usuario = 0;

extern int rwagi_infoteclado;

int (*f_ayuda)() = NULL;
char clave_help[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0}; /* clave del help pos 31 linea */

char referencia_help[128] = {0,};

int (*f_util)() = NULL;

int (*avisar)() = NULL;

void (*f_disparo)() = NULL;

/* #define MASFICHS /* cierre de stdaux y stdprn para MS-DOS */
/* agi agi agi quitado
long nume_serie = 0L;  */
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

static int cuantas_tecla_timeout = 0;
static int que_tecla_timeout = 0;
static int time_tecla_timeout = 0;
static int duracion_tecla_timeout = 0;

extern int time_d_tiempo1 = 180;
extern int time_d_tiempo2 = 3600;


char _fispool[51];
char _ejspool[51];

extern char *dato_entra = NULL;
extern int dato_entra_len = 0;
/* desplegue de menus o no */

/* Funcion para ejecutar una interrrucion interna agisys */
int (*general_int)() = NULL;
int (*int_aviso)() = NULL;
#ifdef ESDOS
int (*timer_especial_int)() = NULL;
#endif


int f_noallowed = 0;

extern char *poncoma(); /* Se podria incluir aqui */

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
extern struct tit_tecla *titu_tecla = _titu_tecla;


extern int fich_tecla = 0;

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
int rentra_edicion = 0; /* funciones especiales de edicion de textos */
int rentra_atrib = 'R';
int rentra_sal_atrib = 'I';
int rentra_cc = 0;
int rentra_cf = 0;
int rentra_sal_cc = 0;
int rentra_sal_cf = 0;
int rentra_offqp = 0;  /* 0 = modo normal de entrada de un campo */
/* -1 = entrada normal pero salida dejando colro por defecto de entrada */
/* -2 o > 0 se supone pintado bien y se sale dejando
color por defecto de entrada */
/* ( !0 es para modos de edicion de texto ) */
int rentra_offini= 0;  /* para cuando la edicion es de lineas largas */
int rentra_insert = 0;

int especial_tecla_linea = 0;

#ifdef FGVWIN
int redita_atrib = ' ';
int redita_cc = 0;
int redita_cf = 7;
int default_redita_atrib = ' ';
int default_redita_cc    = 0;
int default_redita_cf    = 7;
#else
int redita_atrib = ' ';
int redita_cc = 7;
int redita_cf = 0;
int default_redita_atrib = ' ';
int default_redita_cc    = 7;
int default_redita_cf    = 0;
#endif
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
unsigned char Patrib = 0;
char Pgrafi = ' ';
VENT *stdpan = NULL;

extern short buf_tecla[200] = {-1,0,};

#ifdef ESDOS

#ifdef FGV_MAXDOS
static unsigned char C_conversion[128] = {
   /*CODIGO INTERNO*/ /*INDICE SECUENCIA DE CONTROL*/
   /* 128*/                  89,
   /* 129*/                 103,
   /* 130*/                  98,
   /* 131*/                   0,
   /* 132*/                   0,
   /* 133*/                 104,
   /* 134*/                  90,
   /* 135*/                   0,
   /* 136*/                   0,
   /* 137*/                   0,
   /* 138*/                 105,
   /* 139*/                 102,
   /* 140*/                   0,
   /* 141*/                   0,
   /* 142*/                   0,
   /* 143*/                   0,
   /* 144*/                   0,
   /* 145*/                   0,
   /* 146*/                   0,
   /* 147*/                   0,
   /* 148*/                   0,
   /* 149*/                 106,
   /* 150*/                   0,
   /* 151*/                   0,
   /* 152*/                   0,
   /* 153*/                   0,
   /* 154*/                   0,
   /* 155*/                   0,
   /* 156*/                   0,
   /* 157*/                   0,
   /* 158*/                   0,
   /* 159*/                   0,
   /* 160*/                  97,
   /* 161*/                  99,
   /* 162*/                 100,
   /* 163*/                 101,
   /* 164*/                  91,
   /* 165*/                  92,
   /* 166*/                  95,
   /* 167*/                  96,
   /* 168*/                  93,
   /* 169*/                   0,
   /* 170*/                   0,
   /* 171*/                   0,
   /* 172*/                   0,
   /* 173*/                  94,
   /* 174*/                   0,
   /* 175*/                   0,
   /* 176*/                  26,
   /* 177*/                  56,
   /* 178*/                  57,
   /* 179*/                  14,
   /* 180*/                  21,
   /* 181*/                  54,
   /* 182*/                  45,
   /* 183*/                  44,
   /* 184*/                  53,
   /* 185*/                  34,
   /* 186*/                  27,
   /* 187*/                  31,
   /* 188*/                  37,
   /* 189*/                  46,
   /* 190*/                  55,
   /* 191*/                  18,
   /* 192*/                  22,
   /* 193*/                  23,
   /* 194*/                  17,
   /* 195*/                  19,
   /* 196*/                  15,
   /* 197*/                  20,
   /* 198*/                  48,
   /* 199*/                  39,
   /* 200*/                  35,
   /* 201*/                  29,
   /* 202*/                  36,
   /* 203*/                  30,
   /* 204*/                  32,
   /* 205*/                  28,
   /* 206*/                  33,
   /* 207*/                  52,
   /* 208*/                  43,
   /* 209*/                  50,
   /* 210*/                  41,
   /* 211*/                  40,
   /* 212*/                  49,
   /* 213*/                  47,
   /* 214*/                  38,
   /* 215*/                  42,
   /* 216*/                  51,
   /* 217*/                  24,
   /* 218*/                  16,
   /* 219*/                  25,
   /* 220*/                  58,
   /* 221*/                  59,
   /* 222*/                  60,
   /* 223*/                  61,
   /* 224*/                   0,
   /* 225*/                   0,
   /* 226*/                   0,
/* 227*/                   0,
/* 228*/                   0,
/* 229*/                   0,
/* 230*/                   0,
/* 231*/                   0,
/* 232*/                   0,
/* 233*/                   0,
/* 234*/                   0,
/* 235*/                   0,
/* 236*/                   0,
/* 237*/                   0,
/* 238*/                   0,
/* 239*/                   0,
/* 240*/                   0,
/* 241*/                   0,
/* 242*/                   0,
/* 243*/                   0,
/* 244*/                   0,
/* 245*/                   0,
/* 246*/                   0,
/* 247*/                   0,
/* 248*/                   0,
/* 249*/                   0,
/* 250*/                   0,
/* 251*/                   0,
/* 252*/                   0,
/* 253*/                   0,
/* 254*/                   0,
/* 255*/                   0
         };
#endif
         
#ifdef FGVWIN
         
         unsigned char control[_NUMCONTROL][NUMCARCO+2]={
            "\0\0\0\0\0\0",
               "\0\0\0\0\0\0",
               "\10\0\0\0\0\0",
               "\0\0\0\0\0\0",
               "\0\0\0\0\0\0",
               "\0\0\0\0\0\0",
               "\0\0\0\0\0\0",
               "\0\0\0\0\0\0",
               "\0\0\0\0\0\0",
               "\0\0\0\0\0\0",
               "\0\0\0\0\0\0",
               "\0\0\0\0\0\0",
               "\0\0\0\0\0\0",
               "\0\0\0\0\0\0",
               "\263\0\0\0\0\0",
               "\304\0\0\0\0\0",
               "\332\0\0\0\0\0",
               "\302\0\0\0\0\0",
               "\277\0\0\0\0\0",
               "\303\0\0\0\0\0",
               "\305\0\0\0\0\0",
               "\264\0\0\0\0\0",
               "\300\0\0\0\0\0",
               "\301\0\0\0\0\0",
               "\331\0\0\0\0\0",
               "\333\0\0\0\0\0",
               "\260\0\0\0\0\0",
               "\272\0\0\0\0\0",
               "\315\0\0\0\0\0",
               "\311\0\0\0\0\0",
               "\313\0\0\0\0\0",
               "\273\0\0\0\0\0",
               "\314\0\0\0\0\0",
               "\316\0\0\0\0\0",
               "\271\0\0\0\0\0",
               "\310\0\0\0\0\0",
               "\312\0\0\0\0\0",
               "\274\0\0\0\0\0",
               "\326\0\0\0\0\0",
               "\307\0\0\0\0\0",
               "\323\0\0\0\0\0",
               "\322\0\0\0\0\0",
               "\327\0\0\0\0\0",
               "\320\0\0\0\0\0",
               "\267\0\0\0\0\0",
               "\266\0\0\0\0\0",
               "\275\0\0\0\0\0",
               "\325\0\0\0\0\0",
               "\306\0\0\0\0\0",
               "\324\0\0\0\0\0",
               "\321\0\0\0\0\0",
               "\330\0\0\0\0\0",
               "\317\0\0\0\0\0",
               "\270\0\0\0\0\0",
               "\265\0\0\0\0\0",
               "\276\0\0\0\0\0",
               "\261\0\0\0\0\0",
               "\262\0\0\0\0\0",
               "\334\0\0\0\0\0",
               "\335\0\0\0\0\0",
               "\336\0\0\0\0\0",
               "\337\0\0\0\0\0",
               "\0\0\0\0\0\0",
               "\0\0\0\0\0\0",
               "\0\0\0\0\0\0",
               "\1\0\0\0\0\0",
               "\0\0\0\0\0\0",
               "\17\0\0\0\0\0",
               "\1\0\0\0\0\0",
               "\14\0\0\0\0\0",
               "\0\0\0\0\0\0",
               "\4\0\0\0\0\0",
               "\0\0\0\0\0\0",
               "\144\0\0\0\0\0",
               "\144\0\0\0\0\0",
               "\144\0\0\0\0\0",
               "\144\0\0\0\0\0",
               "\144\0\0\0\0\0",
               "\144\0\0\0\0\0",
               "\144\0\0\0\0\0",
               "\144\0\0\0\0\0",
               "\144\0\0\0\0\0",
               "\144\0\0\0\0\0",
               "\144\0\0\0\0\0",
               "\144\0\0\0\0\0",
               "\144\0\0\0\0\0",
               "\144\0\0\0\0\0",
               "\144\0\0\0\0\0",
               "\144\0\0\0\0\0",
               "\307\0\0\0\0\0",
               "\347\0\0\0\0\0",
               "\361\0\0\0\0\0",
               "\321\0\0\0\0\0",
               "\277\0\0\0\0\0",
               "\241\0\0\0\0\0",
               "\252\0\0\0\0\0",
               "\272\0\0\0\0\0",
               "\341\0\0\0\0\0",
               "\351\0\0\0\0\0",
               "\355\0\0\0\0\0",
               "\363\0\0\0\0\0",
               "\372\0\0\0\0\0",
               "\357\0\0\0\0\0",
               "\374\0\0\0\0\0",
               "\340\0\0\0\0\0",
               "\350\0\0\0\0\0",
               "\362\0\0\0\0\0",
};


static short _tecla1[]={
   801,999,999,999,999,999,999,999,801,
};
static short _tecla2[]={
   802,999,999,999,999,999,999,999,802,
};
static short _tecla3[]={
   803,999,999,999,999,999,999,999,803,
};
static short _tecla4[]={
   804,999,999,999,999,999,999,999,804,
};
static short _tecla5[]={
   805,999,999,999,999,999,999,999,805,
};
static short _tecla6[]={
   806,999,999,999,999,999,999,999,806,
};
static short _tecla7[]={
   807,999,999,999,999,999,999,999,807,
};
static short _tecla8[]={
   808,999,999,999,999,999,999,999,808,
};
static short _tecla9[]={
   809,999,999,999,999,999,999,999,809,
};
static short _tecla10[]={
   810,999,999,999,999,999,999,999,810,
};
static short _tecla11[]={
   811,999,999,999,999,999,999,999,811,
};
static short _tecla12[]={
   812,999,999,999,999,999,999,999,812,
};
static short _tecla13[]={
   813,999,999,999,999,999,999,999,813,
};
static short _tecla14[]={
   814,999,999,999,999,999,999,999,814,
};
static short _tecla15[]={
   815,999,999,999,999,999,999,999,815,
};
static short _tecla16[]={
   816,999,999,999,999,999,999,999,816,
};
static short _tecla17[]={
   817,999,999,999,999,999,999,999,817,
};
static short _tecla18[]={
   818,999,999,999,999,999,999,999,818,
};
static short _tecla19[]={
   819,999,999,999,999,999,999,999,819,
};
static short _tecla20[]={
   820,999,999,999,999,999,999,999,820,
};
static short _tecla21[]={
   821,999,999,999,999,999,999,999,821,
};
static short _tecla22[]={
   822,999,999,999,999,999,999,999,822,
};
static short _tecla23[]={
   823,999,999,999,999,999,999,999,823,
};
static short _tecla24[]={
   824,999,999,999,999,999,999,999,824,
};
static short _tecla25[]={
   825,999,999,999,999,999,999,999,825,
};
static short _tecla26[]={
   826,999,999,999,999,999,999,999,826,
};
static short _tecla27[]={
   827,999,999,999,999,999,999,999,827,
};
static short _tecla28[]={
   828,999,999,999,999,999,999,999,828,
};
static short _tecla29[]={
   829,999,999,999,999,999,999,999,829,
};
static short _tecla30[]={
   830,999,999,999,999,999,999,999,830,
};
static short _tecla31[]={
   831,999,999,999,999,999,999,999,831,
};
static short _tecla32[]={
   832,999,999,999,999,999,999,999,832,
};
static short _tecla33[]={
   833,999,999,999,999,999,999,999,833,
};
static short _tecla34[]={
   834,999,999,999,999,999,999,999,834,
};
static short _tecla35[]={
   835,999,999,999,999,999,999,999,835,
};
static short _tecla36[]={
   836,999,999,999,999,999,999,999,836,
};
static short _tecla37[]={
   837,999,999,999,999,999,999,999,837,
};
static short _tecla38[]={
   838,999,999,999,999,999,999,999,838,
};
static short _tecla39[]={
   839,999,999,999,999,999,999,999,839,
};
static short _tecla40[]={
   840,999,999,999,999,999,999,999,840,
};
static short _tecla41[]={
   841,999,999,999,999,999,999,999,841,
};
static short _tecla42[]={
   842,999,999,999,999,999,999,999,842,
};
static short _tecla43[]={
   199,999,999,999,999,999,999,999,128,
};
static short _tecla44[]={
   231,999,999,999,999,999,999,999,135,
};
static short _tecla45[]={
   241,999,999,999,999,999,999,999,164,
};
static short _tecla46[]={
   209,999,999,999,999,999,999,999,165,
};
static short _tecla47[]={
   191,999,999,999,999,999,999,999,168,
};
static short _tecla48[]={
   161,999,999,999,999,999,999,999,173,
};
static short _tecla49[]={
   170,999,999,999,999,999,999,999,166,
};
static short _tecla50[]={
   186,999,999,999,999,999,999,999,167,
};
static short _tecla51[]={
   225,999,999,999,999,999,999,999,160,
};
static short _tecla52[]={
   233,999,999,999,999,999,999,999,130,
};
static short _tecla53[]={
   237,999,999,999,999,999,999,999,161,
};
static short _tecla54[]={
   243,999,999,999,999,999,999,999,162,
};
static short _tecla55[]={
   250,999,999,999,999,999,999,999,163,
};
static short _tecla56[]={
   239,999,999,999,999,999,999,999,139,
};
static short _tecla57[]={
   252,999,999,999,999,999,999,999,129,
};
static short _tecla58[]={
   224,999,999,999,999,999,999,999,133,
};
static short _tecla59[]={
   232,999,999,999,999,999,999,999,138,
};
static short _tecla60[]={
   242,999,999,999,999,999,999,999,149,
};


#else

unsigned char control[_NUMCONTROL][NUMCARCO+2]={
   "\0\0\0\0\0\0",
      "\0\0\0\0\0\0",
      "\10\0\0\0\0\0",
      "\0\0\0\0\0\0",
      "\0\0\0\0\0\0",
      "\0\0\0\0\0\0",
      "\200\0\0\0\0\0",
      "\0\0\0\0\0\0",
      "\0\0\0\0\0\0",
      "\0\0\0\0\0\0",
      "\0\0\0\0\0\0",
      "\0\0\0\0\0\0",
      "\0\0\0\0\0\0",
      "\0\0\0\0\0\0",
      "\263\0\0\0\0\0",
      "\304\0\0\0\0\0",
      "\332\0\0\0\0\0",
      "\302\0\0\0\0\0",
      "\277\0\0\0\0\0",
      "\303\0\0\0\0\0",
      "\305\0\0\0\0\0",
      "\264\0\0\0\0\0",
      "\300\0\0\0\0\0",
      "\301\0\0\0\0\0",
      "\331\0\0\0\0\0",
      "\333\0\0\0\0\0",
      "\260\0\0\0\0\0",
      "\272\0\0\0\0\0",
      "\315\0\0\0\0\0",
      "\311\0\0\0\0\0",
      "\313\0\0\0\0\0",
      "\273\0\0\0\0\0",
      "\314\0\0\0\0\0",
      "\316\0\0\0\0\0",
      "\271\0\0\0\0\0",
      "\310\0\0\0\0\0",
      "\312\0\0\0\0\0",
      "\274\0\0\0\0\0",
      "\326\0\0\0\0\0",
      "\307\0\0\0\0\0",
      "\323\0\0\0\0\0",
      "\322\0\0\0\0\0",
      "\327\0\0\0\0\0",
      "\320\0\0\0\0\0",
      "\267\0\0\0\0\0",
      "\266\0\0\0\0\0",
      "\275\0\0\0\0\0",
      "\325\0\0\0\0\0",
      "\306\0\0\0\0\0",
      "\324\0\0\0\0\0",
      "\321\0\0\0\0\0",
      "\330\0\0\0\0\0",
      "\317\0\0\0\0\0",
      "\270\0\0\0\0\0",
      "\265\0\0\0\0\0",
      "\276\0\0\0\0\0",
      "\261\0\0\0\0\0",
      "\262\0\0\0\0\0",
      "\334\0\0\0\0\0",
      "\335\0\0\0\0\0",
      "\336\0\0\0\0\0",
      "\337\0\0\0\0\0",
      "\0\0\0\0\0\0",
      "\17\0\0\0\0\0",
      "\1\0\0\0\0\0",
      "\6\0\0\0\0\0",
      "\1\0\0\0\0\0",
      "\1\0\0\0\0\0",
      "\7\0\0\0\0\0",
      "\17\0\0\0\0\0",
      "\1\0\0\0\0\0",
      "\14\0\0\0\0\0",
      "\1\0\0\0\0\0",
      "\144\0\0\0\0\0",
      "\144\0\0\0\0\0",
      "\144\0\0\0\0\0",
      "\144\0\0\0\0\0",
      "\144\0\0\0\0\0",
      "\144\0\0\0\0\0",
      "\144\0\0\0\0\0",
      "\144\0\0\0\0\0",
      "\144\0\0\0\0\0",
      "\144\0\0\0\0\0",
      "\144\0\0\0\0\0",
      "\144\0\0\0\0\0",
      "\144\0\0\0\0\0",
      "\144\0\0\0\0\0",
      "\144\0\0\0\0\0",
      "\144\0\0\0\0\0",
      "\200\0\0\0\0\0",
      "\207\0\0\0\0\0",
      "\244\0\0\0\0\0",
      "\245\0\0\0\0\0",
      "\250\0\0\0\0\0",
      "\255\0\0\0\0\0",
      "\246\0\0\0\0\0",
      "\247\0\0\0\0\0",
      "\240\0\0\0\0\0",
      "\202\0\0\0\0\0",
      "\241\0\0\0\0\0",
      "\242\0\0\0\0\0",
      "\243\0\0\0\0\0",
      "\213\0\0\0\0\0",
      "\201\0\0\0\0\0",
      "\205\0\0\0\0\0",
      "\212\0\0\0\0\0",
      "\225\0\0\0\0\0",
};


static short _tecla1[]={
   10,999,999,999,999,999,999,999,801,
};
static short _tecla2[]={
   13,999,999,999,999,999,999,999,802,
};
static short _tecla3[]={
   8,999,999,999,999,999,999,999,803,
};
static short _tecla4[]={
   9,999,999,999,999,999,999,999,804,
};
static short _tecla5[]={
   0,15,999,999,999,999,999,999,805,
};
static short _tecla6[]={
   27,999,999,999,999,999,999,999,806,
};
static short _tecla7[]={
   3,999,999,999,999,999,999,999,807,
};
static short _tecla8[]={
   0,72,999,999,999,999,999,999,808,
};
static short _tecla9[]={
   0,80,999,999,999,999,999,999,809,
};
static short _tecla10[]={
   0,75,999,999,999,999,999,999,810,
};
static short _tecla11[]={
   0,77,999,999,999,999,999,999,811,
};
static short _tecla12[]={
   0,71,999,999,999,999,999,999,812,
};
static short _tecla13[]={
   25,999,999,999,999,999,999,999,813,
};
static short _tecla14[]={
   0,73,999,999,999,999,999,999,814,
};
static short _tecla15[]={
   0,81,999,999,999,999,999,999,815,
};
static short _tecla16[]={
   0,79,999,999,999,999,999,999,816,
};
static short _tecla17[]={
   0,117,999,999,999,999,999,999,817,
};
static short _tecla18[]={
   0,82,999,999,999,999,999,999,818,
};
static short _tecla19[]={
   0,83,999,999,999,999,999,999,819,
};
static short _tecla20[]={
   0,133,999,999,999,999,999,999,820,
};
static short _tecla21[]={
   21,990,999,999,999,999,999,999,821,
};
static short _tecla22[]={
   0,134,999,999,999,999,999,999,822,
};
static short _tecla23[]={
   0,59,999,999,999,999,999,999,823,
};
static short _tecla24[]={
   0,60,999,999,999,999,999,999,824,
};
static short _tecla25[]={
   0,61,999,999,999,999,999,999,825,
};
static short _tecla26[]={
   0,62,999,999,999,999,999,999,826,
};
static short _tecla27[]={
   0,63,999,999,999,999,999,999,827,
};
static short _tecla28[]={
   0,64,999,999,999,999,999,999,828,
};
static short _tecla29[]={
   0,65,999,999,999,999,999,999,829,
};
static short _tecla30[]={
   0,66,999,999,999,999,999,999,830,
};
static short _tecla31[]={
   0,67,999,999,999,999,999,999,831,
};
static short _tecla32[]={
   0,68,999,999,999,999,999,999,832,
};
static short _tecla33[]={
   0,84,999,999,999,999,999,999,833,
};
static short _tecla34[]={
   0,85,999,999,999,999,999,999,834,
};
static short _tecla35[]={
   0,86,999,999,999,999,999,999,835,
};
static short _tecla36[]={
   0,87,999,999,999,999,999,999,836,
};
static short _tecla37[]={
   0,88,999,999,999,999,999,999,837,
};
static short _tecla38[]={
   0,89,999,999,999,999,999,999,838,
};
static short _tecla39[]={
   0,90,999,999,999,999,999,999,839,
};
static short _tecla40[]={
   0,91,999,999,999,999,999,999,840,
};
static short _tecla41[]={
   0,92,999,999,999,999,999,999,841,
};
static short _tecla42[]={
   0,93,999,999,999,999,999,999,842,
};
static short _tecla43[]={
   128,999,999,999,999,999,999,999,128,
};
static short _tecla44[]={
   135,999,999,999,999,999,999,999,135,
};
static short _tecla45[]={
   164,999,999,999,999,999,999,999,164,
};
static short _tecla46[]={
   165,999,999,999,999,999,999,999,165,
};
static short _tecla47[]={
   168,999,999,999,999,999,999,999,168,
};
static short _tecla48[]={
   173,999,999,999,999,999,999,999,173,
};
static short _tecla49[]={
   166,999,999,999,999,999,999,999,166,
};
static short _tecla50[]={
   167,999,999,999,999,999,999,999,167,
};
static short _tecla51[]={
   160,999,999,999,999,999,999,999,160,
};
static short _tecla52[]={
   130,999,999,999,999,999,999,999,130,
};
static short _tecla53[]={
   161,999,999,999,999,999,999,999,161,
};
static short _tecla54[]={
   162,999,999,999,999,999,999,999,162,
};
static short _tecla55[]={
   163,999,999,999,999,999,999,999,163,
};
static short _tecla56[]={
   139,999,999,999,999,999,999,999,139,
};
static short _tecla57[]={
   129,999,999,999,999,999,999,999,129,
};
static short _tecla58[]={
   133,999,999,999,999,999,999,999,133,
};
static short _tecla59[]={
   138,999,999,999,999,999,999,999,138,
};
static short _tecla60[]={
   149,999,999,999,999,999,999,999,149,
};

#endif

#else

unsigned char control[_NUMCONTROL][NUMCARCO+2]={
   "\33[\62\112\0\0",
      "\33[\113\0\0\0",
      "\33[\61m\0\0",
      "\33[\60m\0\0",
      "\0\0\0\0\0\0",
      "\33[\60m\0\0",
      "\33[\65m\0\0",
      "\33[\60m\0\0",
      "\33[\64m\0\0",
      "\33[\60m\0\0",
      "\0\0\0\0\0\0",
      "\0\0\0\0\0\0",
      "\0\0\0\0\0\0",
      "\0\0\0\0\0\0",
      "\263\0\0\0\0\0",
      "\304\0\0\0\0\0",
      "\332\0\0\0\0\0",
      "\302\0\0\0\0\0",
      "\277\0\0\0\0\0",
      "\303\0\0\0\0\0",
      "\305\0\0\0\0\0",
      "\264\0\0\0\0\0",
      "\300\0\0\0\0\0",
      "\301\0\0\0\0\0",
      "\331\0\0\0\0\0",
      "\262\0\0\0\0\0",
      "\260\0\0\0\0\0",
      "\272\0\0\0\0\0",
      "\315\0\0\0\0\0",
      "\311\0\0\0\0\0",
      "\313\0\0\0\0\0",
      "\273\0\0\0\0\0",
      "\314\0\0\0\0\0",
      "\316\0\0\0\0\0",
      "\271\0\0\0\0\0",
      "\310\0\0\0\0\0",
      "\312\0\0\0\0\0",
      "\274\0\0\0\0\0",
      "\326\0\0\0\0\0",
      "\307\0\0\0\0\0",
      "\323\0\0\0\0\0",
      "\322\0\0\0\0\0",
      "\327\0\0\0\0\0",
      "\320\0\0\0\0\0",
      "\267\0\0\0\0\0",
      "\266\0\0\0\0\0",
      "\275\0\0\0\0\0",
      "\325\0\0\0\0\0",
      "\306\0\0\0\0\0",
      "\324\0\0\0\0\0",
      "\321\0\0\0\0\0",
      "\330\0\0\0\0\0",
      "\317\0\0\0\0\0",
      "\270\0\0\0\0\0",
      "\265\0\0\0\0\0",
      "\276\0\0\0\0\0",
      "\261\0\0\0\0\0",
      "\262\0\0\0\0\0",
      "\334\0\0\0\0\0",
      "\335\0\0\0\0\0",
      "\336\0\0\0\0\0",
      "\337\0\0\0\0\0",
      "\33[\60m\0\0",
      "\17\0\0\0\0\0",
      "\1\0\0\0\0\0",
      "\6\0\0\0\0\0",
      "\1\0\0\0\0\0",
      "\1\0\0\0\0\0",
      "\7\0\0\0\0\0",
      "\16\0\0\0\0\0",
      "\1\0\0\0\0\0",
      "\14\0\0\0\0\0",
      "\1\0\0\0\0\0",
      "\33[\63\60m\0",
      "\33[\63\64m\0",
      "\33[\63\62m\0",
      "\33[\63\66m\0",
      "\33[\63\61m\0",
      "\33[\63\65m\0",
      "\33[\63\63m\0",
      "\33[\63\67m\0",
      "\33[\64\60m\0",
      "\33[\64\64m\0",
      "\33[\64\62m\0",
      "\33[\64\66m\0",
      "\33[\64\61m\0",
      "\33[\64\65m\0",
      "\33[\64\63m\0",
      "\33[\64\67m\0",
      "\200\0\0\0\0\0",
      "\207\0\0\0\0\0",
      "\244\0\0\0\0\0",
      "\245\0\0\0\0\0",
      "\250\0\0\0\0\0",
      "\255\0\0\0\0\0",
      "\246\0\0\0\0\0",
      "\247\0\0\0\0\0",
      "\240\0\0\0\0\0",
      "\202\0\0\0\0\0",
      "\241\0\0\0\0\0",
      "\242\0\0\0\0\0",
      "\243\0\0\0\0\0",
      "\213\0\0\0\0\0",
      "\201\0\0\0\0\0",
      "\205\0\0\0\0\0",
      "\212\0\0\0\0\0",
      "\225\0\0\0\0\0",
      
};


static short _tecla1[]={
   10,999,999,999,999,999,999,999,801,
};
static short _tecla2[]={
   13,999,999,999,999,999,999,999,802,
};
static short _tecla3[]={
   8,999,999,999,999,999,999,999,803,
};
static short _tecla4[]={
   9,999,999,999,999,999,999,999,804,
};
static short _tecla5[]={
   27,91,90,999,999,999,999,999,805,
};
static short _tecla6[]={
   27,27,999,999,999,999,999,999,806,
};
static short _tecla7[]={
   3,999,999,999,999,999,999,999,807,
};
static short _tecla8[]={
   27,91,65,999,999,999,999,999,808,
};
static short _tecla9[]={
   27,91,66,999,999,999,999,999,809,
};
static short _tecla10[]={
   27,91,68,999,999,999,999,999,810,
};
static short _tecla11[]={
   27,91,67,999,999,999,999,999,811,
};
static short _tecla12[]={
   27,91,72,999,999,999,999,999,812,
};
static short _tecla13[]={
   25,999,999,999,999,999,999,999,813,
};
static short _tecla14[]={
   27,91,73,999,999,999,999,999,814,
};
static short _tecla15[]={
   27,91,71,999,999,999,999,999,815,
};
static short _tecla16[]={
   27,91,70,999,999,999,999,999,816,
};
static short _tecla17[]={
   16,999,999,999,999,999,999,999,817,
};
static short _tecla18[]={
   27,91,76,999,999,999,999,999,818,
};
static short _tecla19[]={
   127,999,999,999,999,999,999,999,819,
};
static short _tecla20[]={
   27,91,87,999,999,999,999,999,820,
};
static short _tecla21[]={
   21,999,999,999,999,999,999,999,821,
};
static short _tecla22[]={
   27,91,88,999,999,999,999,999,822,
};
static short _tecla23[]={
   27,91,77,999,999,999,999,999,823,
};
static short _tecla24[]={
   27,91,78,999,999,999,999,999,824,
};
static short _tecla25[]={
   27,91,79,999,999,999,999,999,825,
};
static short _tecla26[]={
   27,91,80,999,999,999,999,999,826,
};
static short _tecla27[]={
   27,91,81,999,999,999,999,999,827,
};
static short _tecla28[]={
   27,91,82,999,999,999,999,999,828,
};
static short _tecla29[]={
   27,91,83,999,999,999,999,999,829,
};
static short _tecla30[]={
   27,91,84,999,999,999,999,999,830,
};
static short _tecla31[]={
   27,91,85,999,999,999,999,999,831,
};
static short _tecla32[]={
   27,91,86,999,999,999,999,999,832,
};
static short _tecla33[]={
   27,91,107,999,999,999,999,999,833,
};
static short _tecla34[]={
   27,91,108,999,999,999,999,999,834,
};
static short _tecla35[]={
   27,91,109,999,999,999,999,999,835,
};
static short _tecla36[]={
   27,91,110,999,999,999,999,999,836,
};
static short _tecla37[]={
   27,91,111,999,999,999,999,999,837,
};
static short _tecla38[]={
   27,91,112,999,999,999,999,999,838,
};
static short _tecla39[]={
   27,91,113,999,999,999,999,999,839,
};
static short _tecla40[]={
   27,91,114,999,999,999,999,999,840,
};
static short _tecla41[]={
   27,91,115,999,999,999,999,999,841,
};
static short _tecla42[]={
   27,91,116,999,999,999,999,999,842,
};
static short _tecla43[]={
   128,999,999,999,999,999,999,999,128,
};
static short _tecla44[]={
   135,999,999,999,999,999,999,999,135,
};
static short _tecla45[]={
   164,999,999,999,999,999,999,999,164,
};
static short _tecla46[]={
   165,999,999,999,999,999,999,999,165,
};
static short _tecla47[]={
   168,999,999,999,999,999,999,999,168,
};
static short _tecla48[]={
   173,999,999,999,999,999,999,999,173,
};
static short _tecla49[]={
   166,999,999,999,999,999,999,999,166,
};
static short _tecla50[]={
   167,999,999,999,999,999,999,999,167,
};
static short _tecla51[]={
   160,999,999,999,999,999,999,999,160,
};
static short _tecla52[]={
   130,999,999,999,999,999,999,999,130,
};
static short _tecla53[]={
   161,999,999,999,999,999,999,999,161,
};
static short _tecla54[]={
   162,999,999,999,999,999,999,999,162,
};
static short _tecla55[]={
   163,999,999,999,999,999,999,999,163,
};
static short _tecla56[]={
   139,999,999,999,999,999,999,999,139,
};
static short _tecla57[]={
   129,999,999,999,999,999,999,999,129,
};
static short _tecla58[]={
   133,999,999,999,999,999,999,999,133,
};
static short _tecla59[]={
   138,999,999,999,999,999,999,999,138,
};
static short _tecla60[]={
   149,999,999,999,999,999,999,999,149,
};

/* ATENCION ESTA INCLUIDA EN LAS RUTINAS DE ENSAMBLADOR DE MS-DOS A PELO */
static unsigned char C_conversion[128] = {
   /*CODIGO INTERNO*/ /*INDICE SECUENCIA DE CONTROL*/
   /* 128*/                  89,
   /* 129*/                 103,
   /* 130*/                  98,
   /* 131*/                   0,
   /* 132*/                   0,
   /* 133*/                 104,
   /* 134*/                  90,
   /* 135*/                   0,
   /* 136*/                   0,
   /* 137*/                   0,
   /* 138*/                 105,
   /* 139*/                 102,
   /* 140*/                   0,
   /* 141*/                   0,
   /* 142*/                   0,
   /* 143*/                   0,
   /* 144*/                   0,
   /* 145*/                   0,
   /* 146*/                   0,
   /* 147*/                   0,
   /* 148*/                   0,
   /* 149*/                 106,
   /* 150*/                   0,
   /* 151*/                   0,
   /* 152*/                   0,
   /* 153*/                   0,
   /* 154*/                   0,
   /* 155*/                   0,
   /* 156*/                   0,
   /* 157*/                   0,
   /* 158*/                   0,
   /* 159*/                   0,
   /* 160*/                  97,
   /* 161*/                  99,
   /* 162*/                 100,
   /* 163*/                 101,
   /* 164*/                  91,
   /* 165*/                  92,
   /* 166*/                  95,
   /* 167*/                  96,
   /* 168*/                  93,
   /* 169*/                   0,
   /* 170*/                   0,
   /* 171*/                   0,
   /* 172*/                   0,
   /* 173*/                  94,
   /* 174*/                   0,
   /* 175*/                   0,
   /* 176*/                  26,
   /* 177*/                  56,
   /* 178*/                  57,
   /* 179*/                  14,
   /* 180*/                  21,
   /* 181*/                  54,
   /* 182*/                  45,
   /* 183*/                  44,
   /* 184*/                  53,
   /* 185*/                  34,
   /* 186*/                  27,
   /* 187*/                  31,
   /* 188*/                  37,
   /* 189*/                  46,
   /* 190*/                  55,
   /* 191*/                  18,
   /* 192*/                  22,
   /* 193*/                  23,
   /* 194*/                  17,
   /* 195*/                  19,
   /* 196*/                  15,
   /* 197*/                  20,
   /* 198*/                  48,
   /* 199*/                  39,
   /* 200*/                  35,
   /* 201*/                  29,
   /* 202*/                  36,
   /* 203*/                  30,
   /* 204*/                  32,
   /* 205*/                  28,
   /* 206*/                  33,
   /* 207*/                  52,
   /* 208*/                  43,
   /* 209*/                  50,
   /* 210*/                  41,
   /* 211*/                  40,
   /* 212*/                  49,
   /* 213*/                  47,
   /* 214*/                  38,
   /* 215*/                  42,
   /* 216*/                  51,
   /* 217*/                  24,
   /* 218*/                  16,
   /* 219*/                  25,
   /* 220*/                  58,
   /* 221*/                  59,
   /* 222*/                  60,
   /* 223*/                  61,
   /* 224*/                   0,
   /* 225*/                   0,
   /* 226*/                   0,
/* 227*/                   0,
/* 228*/                   0,
/* 229*/                   0,
/* 230*/                   0,
/* 231*/                   0,
/* 232*/                   0,
/* 233*/                   0,
/* 234*/                   0,
/* 235*/                   0,
/* 236*/                   0,
/* 237*/                   0,
/* 238*/                   0,
/* 239*/                   0,
/* 240*/                   0,
/* 241*/                   0,
/* 242*/                   0,
/* 243*/                   0,
/* 244*/                   0,
/* 245*/                   0,
/* 246*/                   0,
/* 247*/                   0,
/* 248*/                   0,
/* 249*/                   0,
/* 250*/                   0,
/* 251*/                   0,
/* 252*/                   0,
/* 253*/                   0,
/* 254*/                   0,
/* 255*/                   0
                 };
                 
                 
#endif
                 
                 static unsigned char g_[5];
                 extern short *_tecla[_NUMTECLA] =  {
                    _tecla1,
                       _tecla2,
                       _tecla3,
                       _tecla4,
                       _tecla5,
                       _tecla6,
                       _tecla7,
                       _tecla8,
                       _tecla9,
                       _tecla10,
                       _tecla11,
                       _tecla12,
                       _tecla13,
                       _tecla14,
                       _tecla15,
                       _tecla16,
                       _tecla17,
                       _tecla18,
                       _tecla19,
                       _tecla20,
                       _tecla21,
                       _tecla22,
                       _tecla23,
                       _tecla24,
                       _tecla25,
                       _tecla26,
                       _tecla27,
                       _tecla28,
                       _tecla29,
                       _tecla30,
                       _tecla31,
                       _tecla32,
                       _tecla33,
                       _tecla34,
                       _tecla35,
                       _tecla36,
                       _tecla37,
                       _tecla38,
                       _tecla39,
                       _tecla40,
                       _tecla41,
                       _tecla42,
                       _tecla43,
                       _tecla44,
                       _tecla45,
                       _tecla46,
                       _tecla47,
                       _tecla48,
                       _tecla49,
                       _tecla50,
                       _tecla51,
                       _tecla52,
                       _tecla53,
                       _tecla54,
                       _tecla55,
                       _tecla56,
                       _tecla57,
                       _tecla58,
                       _tecla59,
                       _tecla60,
                 }; /* teclas como secuencia de int. */
                 
                 
                 short **tecla = _tecla;
                 short NUMTECLA = _NUMTECLA;
                 short NUMCONTROL = _NUMCONTROL;
                 short FUNCIONES = _FUNCIONES;
                 extern int fcursor = 0; /* tipo de funcion cursor 0-9 */
                 
                 static unsigned char *mochila = "\0\0\'\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
                 
                 
                 extern unsigned char cod_i_graf[48] = { /* codigos internos de los semi-graficos */
                    179,
                       196,
                       218,
                       194,
                       191,
                       195,
                       197,
                       180,
                       192,
                       193,
                       217,
                       219,
                       176,
                       186,
                       205,
                       201,
                       203,
                       187,
                       204,
                       206,
                       185,
                       200,
                       202,
                       188,
                       214,
                       199,
                       211,
                       210,
                       215,
                       208,
                       183,
                       182,
                       189,
                       213,
                       198,
                       212,
                       209,
                       216,
                       207,
                       184,
                       181,
                       190,
                       177,
                       178,
                       220,
                       221,
                       222,
                       223
                 };
                 
#ifdef ESDOS
#ifndef FGV32
                 union REGS inregs,outregs;
#endif
                 
#ifdef WATCOMNLM
                 int _textattr(int atri,char *data,int tama)
                 {
                 /*
                 struct cell {
                 
                   char   charValue;
                   char   attribute;
                   } buf[260];
                    */
                    unsigned char buf[512];
                    int i,j;
                    
                    if (tama == -1)
                       SetScreenRegionAttribute(0,25,atri);
                    else
                    {
                       for (i = 0,j = 0;i < tama;i++)
                       {
                          buf[j] = data[i];
                          j++;
                          buf[j] = atri;
                          j++;
                       }
                       CopyToScreenMemory(1,tama,buf,Pcursor%80,Pcursor/80);
                    }
                    return(0);
                 }
#endif
                 
                 int dos_atri() /* devuelve el control correspondiente al atributo */
                 {
                    switch((Patrib & (~128))) {
                    case 'R':
                       return(PCcolor + PFcolor * 16 + control[ABRIRREV][0]);
                    case 'I':
                    /*
                    #ifdef RWXARXA
                    #ifdef RWSERVER
                    if (en_conexion_externa && remoto_es_sistema_grafico && (Patrib & 128))
                    {
                    return(7*16+control[ALTAINTEN][0]);
                    }
                    else
                    #endif
                    #endif
                       */
#ifdef FGVWIN
                       if (Patrib & 128)
                          return(7*16+control[ALTAINTEN][0]);
                       else
#endif
                          return(PCcolor + PFcolor * 16 + control[ALTAINTEN][0]);
                    case 'P':
                       return(PCcolor + PFcolor * 16 + control[ABRIRPAR][0]);
                    case 'S':
                       return(PCcolor + PFcolor * 16 + control[ABRIRSUB][0]);
                    default:
                       return(PCcolor + PFcolor * 16 + control[NORMAL][0]);
                    }
                 }
                 
                 
#ifndef FGVWIN
#ifdef FGV_MAXDOS
#ifndef FGV32
                 int dosprint(s,atri)
                    unsigned char *s;
                 int atri;
                 {
                    register unsigned char c;
                    register unsigned char *pant = direc_ppanta + Pcursor * 2;
                    while ((c = *s))
                    {
                       if (c > 127 && C_conversion[c-128])
                       {
                          c = C_conversion[c-128];
                          c = control[c][0];
                       }
                       *pant = c;
                       pant++;
                       *pant = atri;
                       pant++;
                       s++;
                    }
                 }
#endif
#endif
                 
                 int dos_print(s)
                    char *s;
                 {
#ifdef FGV_MAXDOS
#ifdef FGV32
                    char tmp[256];
#ifndef WATCOMNLM
                    DWORD i;
                    COORD backgLoc;
                    DWORD dwCharsWritten;
#else
                    int i;
#endif
                    register unsigned char c;
                    register unsigned char *pant = tmp;
#ifdef RWXARXA
#ifdef RWSERVER
                    if (en_conexion_externa)
                    {
                       print_en_terminal(s);
                    }
#endif
#endif
                    i =0;
                    while ((c = *s))
                    {
                       if (c > 127 && C_conversion[c-128])
                       {
                          c = C_conversion[c-128];
                          c = control[c][0];
                       }
                       *pant = c;
                       pant++;
                       s++;
                       i++;
                    }
                    *pant = 0;
#ifndef WATCOMNLM
                    if (myconsola != INVALID_HANDLE_VALUE)
                    {
                       backgLoc.X = Pcursor%80;
                       backgLoc.Y = Pcursor/80;
                       WriteConsoleOutputCharacter(myconsola,tmp,i,backgLoc,&dwCharsWritten);
                       FillConsoleOutputAttribute(myconsola,dos_atri(),i,backgLoc,&dwCharsWritten);
                    }
#else
                    /*i = cputs(tmp);*/
                    _textattr(dos_atri(),tmp,strlen(tmp));
#endif
                    return(i);
#else
                    return(dosprint(s,dos_atri()));
#endif
#else
                    return(DOSPRINT(s,dos_atri(),sizeof(char *),control));
#endif
                 }
#endif
                 
#ifdef FGVWIN
                 int convierte_caracter(c)
                    unsigned char c;
                 {
                    unsigned int i = c;
                    if (i > 127) {
                       if (C_conversion[i-128])
                       {
                          i = C_conversion[i-128];
                          i = control[i][0];
                       }
                    }
                    return(i);
                 }
#endif
                 
                 
#ifdef FGV32
#ifdef _MSC_VER
#ifndef FGVWIN
                 
                 
                 clrscr()
                 {
                    COORD backgLoc;
                    DWORD dwCharsWritten;
                    
                    backgLoc.X = 0;
                    backgLoc.Y = 0;
                    FillConsoleOutputCharacter(myconsola,' ',80*25,backgLoc,&dwCharsWritten);
                    FillConsoleOutputAttribute(myconsola,dos_atri(),80*25,backgLoc,&dwCharsWritten);
                 }
                 
                 textattr()
                 {
                 }
                 
#endif
#endif
#else
#ifdef _MSC_VER
#ifndef FGVWIN
                 clrscr()
                 {
                    unsigned char linea[160];
                    int i;
                    unsigned char a = dos_atri();
                    unsigned char *p = direc_ppanta;
                    
                    
                    for (i = 0;i < 160;i += 2)
                    {
                       *p = linea[i] = ' ';
                       p++;
                       *p = linea[i+1] = a;
                       p++;
                    }
                    
                    for (i = 0;i < 24;i++)
                    {
                       memcpy(p,linea,160);
                       p += 160;
                    }
                    
                    /*
                    _settextrows( 25 );
                    _clearscreen( _GVIEWPORT );
                    */
                 }
                 textattr(int a)
                 {
                 /*
                 _settextcolor(a&0xff);
                 _setbkcolor((a >> 4)&0xff);
                    */
                 }
                 
                 gotoxy(a,b)
                    int a,b;
                 {
                    inregs.h.ah = 0x02;     /* cambios para MS DOS */
                    inregs.h.bh = 0;
                    inregs.h.dh = b-1;
                    inregs.h.dl = a-1;
                    int86(0x10,&inregs,&outregs);
                    
                    /*_settextposition( b, a ); */
                    
                 }
#endif
#endif
#endif
                 
                 /* no hace el v_putc pero se supone que lo hace la llamada */
                 int Conversion(c)
                    unsigned char c;
                 {
#ifdef FGV_MAXDOS
#ifndef FGVWIN
#ifdef FGV32
#ifndef WATCOMNLM
                    COORD backgLoc;
                    DWORD dwCharsWritten;
#endif
                    
#ifdef RWXARXA
#ifdef RWSERVER
                    if (en_conexion_externa)
                    {
                       conversion_en_terminal(c);
                    }
#endif
#endif
                    
                    if (c > 127) {
                       if (C_conversion[c-128])
                       {
                          c = C_conversion[c-128];
                          c = control[c][0];
                       }
                    }
#ifndef WATCOMNLM
                    if (myconsola != INVALID_HANDLE_VALUE)
                    {
                       backgLoc.X = Pcursor%80;
                       backgLoc.Y = Pcursor/80;
                       WriteConsoleOutputCharacter(myconsola,&c,sizeof(c),backgLoc,&dwCharsWritten);
                       FillConsoleOutputAttribute(myconsola,dos_atri(),sizeof(c),backgLoc,&dwCharsWritten);
                    }
#else
                    /* putch(c);*/
                    _textattr(dos_atri(),&c,1);
#endif
#else
                    register unsigned char *pant = direc_ppanta + Pcursor * 2;
                    if (c > 127) {
                       if (C_conversion[c-128])
                       {
                          c = C_conversion[c-128];
                          c = control[c][0];
                       }
                    }
                    *pant = c;
                    *(pant+1) = dos_atri();
                    
#endif
#else
                    w_winprint(&c,dos_atri(),1);
#endif
                    return(c);
#else
                    return(DOSPUTC((int)c,dos_atri(),sizeof(char *),control));
#endif
                 }
                 
#else
                 
                 /*
                 * pinta  pinta una secuencia de caracteres (solo para controles de
                 *        pantalla)
                 *
                 */
                 void pinta(s,i,m)
                    register char *s;
                 register int i;
                 register int m;
                 {
                    static char u_s[NUMCARCO] = {0,};
                    static int u_i = 0;
                    
                    if (atento_imp) return;
                    
#ifdef RWXARXA
#ifdef RWSERVER
                    if (en_conexion_externa) return;
#endif
#endif
                    
                    if (m) {
                    /* ESTO ES UNA PEQUE�A OPTIMIZACION QUE EVITA QUE SE REPITA SEGUIDO EL MISMO
                       STRING DE CONTROL SOLO SE APLICA A ATRIBUTOS */
                       if (i == u_i && !memcmp(s,u_s,i)) return;
                       if (i <= NUMCARCO) {
                          u_i = i;
                          memcpy(u_s,s,i);
                       } 
                    }   
                    
                    /* if (atento_imp) i_acabo(_imp); */
                    while (i--) {
                       if (*s)
                          putchar(*s);
                       *s++;
                    }
                    /* if (atento_imp) i_inicio(_imp);        */
                 }
                 /* string Convierte los codigos internos en su indice de secuencia de control */
                 /* Solo Semigraficos y Caracteres Especiales */
                 /* No aplicable a MS-DOS */
                 int Conversion(c)
                    unsigned char c;
                 {
                    int i;
                    int g = 0;
                    
                    if ((Pcursor/80) >= 24 && !linea_25) return(c);
                    
#ifdef RWXARXA
#ifdef RWSERVER
                    if (en_conexion_externa)
                    {
                       conversion_en_terminal(c);
                       return(c);
                    }
#endif
#endif
                    
                    
                    if (c > 127) {
                       if ((i = C_conversion[c-128])) {
                          if (i >= GRVE && i <= GRC8 && Pgrafi == ' ') {
                             g = 1;
                             Gon();
                          }
                          pinta(control[i],NUMCARCO,0);
                          if (g)
                             Goff();
                          return(c);
                       }
                    }
                    return(putchar(c));
                 }
                 
#endif
                 
                 /* -----------------------------------------------------------------*/
                 
#ifndef ESDOS
                 /*
                 * nombreter: 
                 * devuelve el nombre del terminal en la forma  "dev/ter/tty?.tnv"
                 */
                 
                 char *nombreter(s)
                    char *s;
                 {
                    char *t,*i;
                    
                    if (isatty(fileno(stdout)))
                       t = i = ttyname(fileno(stdout));
                    else
                    {
                       strcpy(s,"dev/ter/daemon.tnv");
                       return(s);
                    }
                    
                    if (!t)
                    {
                       strcpy(s,"dev/ter/console.tnv"); 
                       return(s);
                    }
                    
                    while (*t) t++;
                    t--;
                    while (t >= i && *t != '/') t--;
                    
                    strcpy(s,"dev/ter/");
                    strcat(s,t + 1);
                    strcat(s,".tnv");
                    
                    return(s);
                 }
                 
                 
#endif
                 
                 
#ifndef ESDOS
                 /*
                 * pulsatecla
                 * devuelve EOF (-1) si no se ha pulsado ninguna tecla
                 * en caso contrario devuelve el caracter pulsado
                 */
                 int pulsatecla()
                 {
                    int a,c;
                    
                    
                    if (modo_desatendido) return(-1);
                    
#ifdef RWXARXA
#ifdef RWSERVER
                    if (en_conexion_externa)
                    {
                       return(pulsatecla_en_terminal());
                    }
#endif
#endif
                    
                    
                    if (!isatty(fileno(stdin)))
                    { 	  
                       return(-1);
                    }
                    
                    a = fcntl(0,F_GETFL,0);
                    fcntl(0,F_SETFL,a | O_NDELAY);
                    do {
                       if (errno == EINTR)
                          errno = 0;
                       c=getchar();
                    } while(c == EOF && errno == EINTR);
                    fcntl(0,F_SETFL,a);
                    return(c);
                 }
                 
#endif
                 
                 /*
                 * compacon  compara un caracter c con el i-esimo de todas las secuencias
                 *           de control para ver si lo contienen.
                 *           En la matriz sitecla guarda 1 si lo contiene y 0 en caso contrario.
                 *           Devuelve: - -1 si se ha de seguir procesando alguna secuencia
                 *                     - el caracter c que le llega, indicando que no es, ni
                 *                       forma parte de una secuencia de control
                 *                     - el numero de secuencia que corresponda
                 */
                 
                 int compacon(i,c,sitecla)
                    int i,c;
                 int *sitecla;
                 {
                    register int j;
                    int seguir=0;
                    
                    for (j=0;j<NUMTECLA;j++) {
                       if (sitecla[j]) {
                          if (tecla[j][i] == c) {
                             if ((i == (NUMCARTE-1)) || (tecla[j][i+1] == 999) )
                                return (tecla[j][NUMCARTE]);
                             else {
                                seguir = 1;
                                sitecla[j] = 1;
                                break;
                             }
                          }
                          else
                             sitecla[j] = 0;
                       }
                    }
                    return ((seguir == 0) ? (c) : (-1));
                 }
                 
                 
                 /******************************************************************************
                 *
                 *
                 *
                 ******************************************************************************
                 */
                 
                 
                 /*
                 * w_putc  pone un caracter o grafico con su atributo en la ventana
                 */
                 void w_putc(w,c)
                    VENT *w;
                 unsigned char c;
															  {
                                                  int fil = (w->vfin)%100 - (w->vini)%100 + 1;  /* columnas de la ventana */
                                                  int h = ((Pcursor/80+1) - (w->vini)/100) * fil; /* despazamiento filas */
                                                  int g = (Pcursor%80+1) - ((w->vini)%100); /* desplazamiento columnas */
                                                  unsigned char *wvv = w->vv;
                                                  
#ifdef RWXARXA
#ifdef RWSERVER
                                                  if (en_conexion_externa && w != stdpan)
                                                  {
                                                     w_putc_en_terminal(w,c);
                                                  }
#endif
#endif
                                                  
#ifdef FGVWIN
                                                  if (w == stdpan && (c == 219 || c == 176 || c == 177 || c == 178))
                                                     c == 1;
#endif
                                                  
                                                  if ( ((h+g) <= (w->size)) &&  ( (h+g) >= 0 ) ) {
                                                     if (Pgrafi != ' ') {if (c < 97) { /* Grafico empezando de 0 */
                                                        if (c < 48)
                                                           c = cod_i_graf[c];
                                                     }
                                                     else
                                                        if (c < 128) {
                                                           c -= 97;
                                                           c = cod_i_graf[c];
                                                        }
                                                     }
                                                     *((wvv) + h + g) = c;
                                                     *((wvv) + (w->size) + h + g) = (unsigned char)(((PFcolor << 4) & 0xf0) | PCcolor); /* Color Especificado */
                                                     *((wvv) + 2 * (w->size) + h + g) = Patrib;  /* atributo del caracter */
                                                  }
                                               }
                                               /* -----------------------------------------------------------------*/
                                               
                                               /*
                                               * w_ini
                                               * pide memoria para la pantalla y la inicializa con blancos
                                               * devuelve -1 si falla al pedir memoria, 0 si va bien
                                               */
                                               
                                               extern void cursor();
                                               
                                               extern void w_cls();
                                               
#ifdef FGVWIN
                                               void crea_bmp_w(w,flag)
                                                  VENT *w;
                                               int flag;
                                               {
                                                  HDC hdc;
                                                  int pi =        w->vini;
                                                  int pf = w->vfin;
                                                  
                                                  if (w->borigen)
                                                     SelectObject(w->vhdc,w->borigen);
                                                  if (w->grafico)
                                                     DeleteObject(w->grafico);
                                                  if (w->Oldpalette)
                                                     SelectPalette(w->vhdc,w->Oldpalette,FALSE);
                                                  if (w->vhdc)
                                                     DeleteDC(w->vhdc);
                                                  
                                                  w->Oldpalette = NULL;
                                                  hdc = GetDC(principal_w);
                                                  w->vhdc = CreateCompatibleDC(hdc);
                                                  if (flag)
                                                     w->grafico= CreateCompatibleBitmap(hdc,principal_nx,principal_ny);
                                                  else
                                                     w->grafico= CreateCompatibleBitmap(hdc,(pf%100 - pi%100 + 1) * scaracter.cx,(pf/100 - pi/100 + 1) * scaracter.cy);
                                                  ReleaseDC(principal_w,hdc);
                                                  SetMapMode(w->vhdc,MM_TEXT);
                                                  SetBkMode(w->vhdc,TRANSPARENT);
                                                  SetTextAlign(w->vhdc,TA_NOUPDATECP | TA_LEFT );
                                                  w->borigen= SelectObject(w->vhdc,w->grafico);
                                               }
                                               
#endif
                                               
                                               VENT *w_ini(pi,pf)
                                                  int pi,pf;
                                               {
                                                  VENT *w;
                                                  int n;
                                                  int lastdpan = 0;
                                                  if (pi == -1 && pf == -1)
                                                     lastdpan = 1;
                                                  if (pi == -1) pi = 101;
                                                  if (pf == -1) pf = 2580;
                                                  if ((w = (VENT *)malloc(sizeof(VENT))) == NULW)
                                                     return(NULW);
#ifdef FGVWIN
                                                  /* trucos traperos!!*/
                                                  if ((pi/100) == 4)
                                                     pi -= 100;
                                                  if ((pf/100) == 24)
                                                     pf += 100;
#endif
                                                  w->vini = pi;
                                                  w->vfin = pf;
                                                  n = (pf/100 - pi/100 + 1) * (pf%100 - pi%100 + 1);
                                                  w->size = n;
                                                  if ((w->vv = malloc(3*n)) == NULL) return(NULW);
                                                  memset(w->vv,' ',n);
                                                  memset(w->vv+n,128,n);/* 128 = 1 0000 000 esto es color por defecto */
                                                  memset(w->vv+2*n,' ',n);
#ifdef FGVWIN
                                                  w->mapagrafico = malloc(n);
                                                  memset(w->mapagrafico,0,n);
                                                  w->vhdc = NULL;
                                                  w->grafico= NULL;
                                                  w->borigen= NULL;
                                                  w->sacada = 0;
                                                  w->Oldpalette = NULL;
                                                  w->visible = 1;
                                                  if (!no_bitblt)
                                                     crea_bmp_w(w,lastdpan);
#endif
#ifdef RWXARXA
#ifdef RWSERVER
                                                  if (en_conexion_externa)
                                                  {
                                                     w_ini_en_terminal(w);
                                                  }
#endif
#endif
                                                  return(w);
                                               }
                                               
#ifdef ESMOTOROLA
                                               int moto_short(ppp)
                                                  unsigned short *ppp;
                                               {
                                                  union _moto_short
                                                  {
                                                     unsigned short k;
                                                     unsigned char p[2];
                                                  } _m;
                                                  _m.p[0] = *(((unsigned char *)ppp)+1);
                                                  _m.p[1] = *((unsigned char *)ppp);
                                                  *ppp = _m.k;
                                                  return(0);
                                               }
                                               
                                               int moto_long(ppp)
                                                  unsigned long *ppp;
                                               {
                                                  union _moto_long
                                                  {
                                                     unsigned long k;
                                                     unsigned char p[4]; 
                                                  } _m;
                                                  _m.p[0] = *(((unsigned char *)ppp)+3);
                                                  _m.p[1] = *(((unsigned char *)ppp)+2);
                                                  _m.p[2] = *(((unsigned char *)ppp)+1);
                                                  _m.p[3] = *((unsigned char *)ppp);
                                                  *ppp = _m.k;
                                                  return(0);
                                               }
                                               
                                               int moto_float(ppp)
                                                  float *ppp;
                                               {
                                                  union _moto_float
                                                  {
                                                     float k;
                                                     unsigned char p[4];
                                                  } _m;
                                                  _m.p[0] = *(((unsigned char *)ppp)+3);
                                                  _m.p[1] = *(((unsigned char *)ppp)+2);
                                                  _m.p[2] = *(((unsigned char *)ppp)+1);
                                                  _m.p[3] = *((unsigned char *)ppp);
                                                  *ppp = _m.k;
                                                  return(0);
                                               }
                                               
                                               int moto_double(ppp)
                                                  double *ppp;
                                               {
                                                  union _moto_double
                                                  {
                                                     double k;
                                                     unsigned char p[8]; 
                                                  } _m;
                                                  _m.p[0] = *(((unsigned char *)ppp)+7);
                                                  _m.p[1] = *(((unsigned char *)ppp)+6);
                                                  _m.p[2] = *(((unsigned char *)ppp)+5);
                                                  _m.p[3] = *(((unsigned char *)ppp)+4);
                                                  _m.p[4] = *(((unsigned char *)ppp)+3);
                                                  _m.p[5] = *(((unsigned char *)ppp)+2);
                                                  _m.p[6] = *(((unsigned char *)ppp)+1);
                                                  _m.p[7] = *((unsigned char *)ppp);
                                                  *ppp = _m.k;
                                                  return(0);
                                               }
                                               
#endif
                                               
                                               /*
                                               * w_fin
                                               * libera la memoria reservada a una pantalla
                                               */
                                               void w_fin(w)
                                                  VENT *w;
                                               {
                                                  int i;
                                                  
#ifdef RWXARXA
#ifdef RWSERVER
                                                  if (en_conexion_externa)
                                                  {
                                                     w_fin_en_terminal(w);
                                                  }
#endif
#endif
                                                  
                                                  free(w->vv);
#ifdef FGVWIN
                                                  if (w->vhdc)
                                                  {
                                                     if (w->borigen)
                                                        SelectObject(w->vhdc,w->borigen);
                                                     if (w->grafico)
                                                        DeleteObject(w->grafico);
                                                     if (w->Oldpalette)
                                                        SelectPalette(w->vhdc,w->Oldpalette,FALSE);
                                                     DeleteDC(w->vhdc);
                                                  }
                                                  free(w->mapagrafico);
                                                  w->mapagrafico = NULL;
                                                  
#endif
                                                  
                                                  free((char *)w);
                                               }
                                               
                                               static char _rw_dirbase[51] = DEFAULTDIRBASE;
                                               
#include <stdlib.h>
                                               /*extern char *getenv();*/
                                               
                                               
                                               
                                               char *rw_directorio(nom)
                                                  char *nom;
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
                                                  }
                                                  sprintf(mdbase,"%s/%s",_rw_dirbase,nom);
                                                  cambia_barra(mdbase);
                                                  return(mdbase);
                                               }
                                               
                                               int rw_setdirectorio(nom)
                                                  char *nom;
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
                                               
                                               
                                               /*
                                               * w_cambia
                                               * cambia las posiciones de inicio y final (y el tamano) de una ventana
                                               * esto solo se puede hacer cuendo la ventana esta inicializada a un
                                               * tamano (w_size) mayor del que se le quiere reasignar
                                               * en caso contrario puede dar problemas de memoria,
                                               * de todas formas se hace el cambio de parametros
                                               * devuelve 0 si va bien, -1 en caso contrario
                                               */
                                               int w_cambia(w,pi,pf)
                                                  VENT *w;
                                               int pi,pf;
                                               {
                                                  int n;
                                                  
#ifdef RWXARXA
#ifdef RWSERVER
                                                  if (en_conexion_externa && w != stdpan)
                                                  {
                                                     wcambia_en_terminal(w,pi,pf);
                                                  }
#endif
#endif
                                                  n = w->size;  /* tamano original */
                                                  w->size = (pf/100 - pi/100 + 1) * (pf%100 - pi%100 + 1);
                                                  w->vini = pi;
                                                  w->vfin = pf;
                                                  if (w->size > n) return(-1);
                                                  return(0);
                                               }
                                               
                                               /*
                                               * w_marco -> pone un marco a la ventana
                                               */
                                               
                                               int w_marco_(w,pi,pf,tipo)
                                                  VENT *w;
                                               int pi,pf;
                                               int tipo;
                                               {
                                                  int alto  = (w->vfin)/100 - (w->vini)/100 + 1;
                                                  int ancho = (w->vfin)%100 - (w->vini)%100 + 1;
                                                  int size = w->size;
                                                  unsigned char *wvv = w->vv;
                                                  int i;
                                                  unsigned char c = (((PFcolor << 4) & 0xf0) | PCcolor);
                                                  int fi = pi/100 - (w->vini)/100;
                                                  int ff = pf/100 - (w->vini)/100;
                                                  int ci = pi%100 - (w->vini)%100;
                                                  int cf = pf%100 - (w->vini)%100;
                                                  int calto  = ff - fi + 1;
                                                  int cancho = cf - ci + 1;
                                                  unsigned char hori = (tipo) ? 205 : 196;
                                                  unsigned char vert = (tipo) ? 186 : 179;
                                                  unsigned char e1 = (tipo) ? 201 : 218;
                                                  unsigned char e2 = (tipo) ? 187 : 191;
                                                  unsigned char e3 = (tipo) ? 200 : 192;
                                                  unsigned char e4 = (tipo) ? 188 : 217;
                                                  
                                                  
#ifdef RWXARXA
#ifdef RWSERVER
                                                  if (en_conexion_externa)
                                                  {
                                                     wmarco_en_terminal(w,pi,pf,tipo);
                                                  }
#endif
#endif
                                                  
                                                  if (cancho > 2) {
                                                     memset(wvv+fi*ancho+ci+1,hori,cancho-2);
                                                     memset(wvv+ff*ancho+ci+1,hori,cancho-2);
                                                     memset(wvv+size+fi*ancho+ci+1,c,cancho-2);
                                                     memset(wvv+size+ff*ancho+ci+1,c,cancho-2);
                                                     memset(wvv+size+size+fi*ancho+ci+1,Patrib,cancho-2);
                                                     memset(wvv+size+size+ff*ancho+ci+1,Patrib,cancho-2);
                                                  }
                                                  
                                                  if (calto > 2) {
                                                     for (i = fi+1;i < ff;i++) {
                                                        *(wvv+i*ancho+ci) = vert;
                                                        *(wvv+i*ancho+ci+cancho-1) = vert;
                                                        *(wvv+size+i*ancho+ci) = c;
                                                        *(wvv+size+i*ancho+ci+cancho-1) = c;
                                                        *(wvv+size*2+i*ancho+ci) = Patrib;
                                                        *(wvv+size*2+i*ancho+ci+cancho-1) = Patrib;
                                                     }
                                                  }
                                                  if (calto < 2) {
                                                     *(wvv+fi*ancho+ci) = hori;
                                                     *(wvv+ff*ancho+ci+cancho-1) = hori;
                                                     *(wvv+size+fi*ancho+ci) = c;
                                                     *(wvv+size+ff*ancho+ci+cancho-1) = c;
                                                     *(wvv+size*2+fi*ancho+ci) = Patrib;
                                                     *(wvv+size*2+ff*ancho+ci+cancho-1) = Patrib;
                                                  }
                                                  else
                                                     if (cancho < 2) {
                                                        *(wvv+fi*ancho+ci) = vert;
                                                        *(wvv+ff*ancho+ci+cancho-1) = vert;
                                                        *(wvv+size+fi*ancho+ci) = c;
                                                        *(wvv+size+ff*ancho+ci+cancho-1) = c;
                                                        *(wvv+size*2+fi*ancho+ci) = Patrib;
                                                        *(wvv+size*2+ff*ancho+ci+cancho-1) = Patrib;
                                                     }
                                                     else {
                                                        *(wvv+fi*ancho+ci) = e1;
                                                        *(wvv+fi*ancho+ci+ancho-1) = e2;
                                                        *(wvv+ff*ancho+ci) = e3;
                                                        *(wvv+ff*ancho+ci+ancho-1) = e4;
                                                        *(wvv+size+fi*ancho+ci) = c;
                                                        *(wvv+size+fi*ancho+ci+ancho-1) = c;
                                                        *(wvv+size+ff*ancho+ci) = c;
                                                        *(wvv+size+ff*ancho+ci+ancho-1) = c;
                                                        *(wvv+size*2+fi*ancho+ci) = Patrib;
                                                        *(wvv+size*2+fi*ancho+ci+ancho-1) = Patrib;
                                                        *(wvv+size*2+ff*ancho+ci) = Patrib;
                                                        *(wvv+size*2+ff*ancho+ci+ancho-1) = Patrib;
                                                     }
                                               }
                                               
                                               int w_marco(w,pi,pf)
                                                  VENT *w;
                                               int pi,pf;
                                               {
                                                  return(w_marco_(w,pi,pf,0));
                                               }
                                               
                                               /*
                                               * w_saca
                                               * saca el contenido de una ventana sobre otra
                                               * como condicion la de destino ha de estar incluida en la de origen
                                               * devuelve -1 en caso contrario
                                               */
                                               int w_saca(w,wo)
                                                  VENT *w;            /* ventana de destino */
                                               VENT *wo;           /* ventana de origen  */
                                               {
                                                  int i,j=0,k;
                                                  int ancho = (w->vfin)%100 - (w->vini)%100 + 1;   /* ancho destino */
                                                  int alto = (w->vfin)/100 - (w->vini)/100 + 1; /* alto destino  */
                                                  int oncho = (wo->vfin)%100 - (wo->vini)%100 + 1; /* ancho origen  */
                                                  int cini = (w->vini)%100 - (wo->vini)%100;
                                                  int fini = (w->vini)/100 - (wo->vini)/100;
                                                  int ffin = (w->vfin)/100 - (wo->vini)/100;
                                                  unsigned char *wvv = w->vv;
                                                  
                                                  
                                                  if (general_int) (*general_int)();
                                                  
#ifdef ESDOS
#ifndef FGVWIN
                                                  if (timer_especial_int)
                                                     (*timer_especial_int)();
#endif
#endif
                                                  
                                                  if ((w->vini) < (wo->vini) || (w->vfin) > (wo->vfin)) return(-1);
                                                  for (i = fini;i <= ffin;i++) {
                                                     memcpy((wvv)+j,(wo->vv)+i*oncho+cini,ancho);
#ifdef FGVWIN
                                                     memcpy((w->mapagrafico)+j,(wo->mapagrafico)+i*oncho+cini,ancho);
#endif
                                                     memcpy((wvv)+(w->size)+j,(wo->vv)+(wo->size)+i*oncho+cini,ancho);
                                                     memcpy((wvv)+2*(w->size)+j,(wo->vv)+2*(wo->size)+i*oncho+cini,ancho);
                                                     j += ancho;
                                                  }
#ifdef FGVWIN
                                                  if (!no_bitblt)
                                                     BitBlt(w->vhdc,0,0,ancho*scaracter.cx,alto*scaracter.cy,wo->vhdc,cini*scaracter.cx,((wo == stdpan) ? espacio_arriba : 0) +fini*scaracter.cy,SRCCOPY);
                                                  if (wo == stdpan)
                                                  {
                                                     w->sacada = 1;
                                                  } 
#endif
#ifdef RWXARXA
#ifdef RWSERVER
                                                  if (en_conexion_externa)
                                                  {
                                                     w_saca_en_terminal(w,wo);
                                                  }
#endif
#endif
                                                  return(0);
                                               }
                                               
                                               /*
                                               * w_mete
                                               * mete el contenido de una ventana sobre otra
                                               * como condicion la de origen ha de estar incluida en la de destino
                                               * devuelve -1 en caso contrario
                                               */
                                               int _w_mete(w,wo,modo)
                                                  VENT *w;            /* ventana de destino */
                                               VENT *wo;           /* ventana de origen  */
                                               int modo; /* 0 opaco 1 trasparente  */
                                               {
                                                  int i,j=0,k,l,k1,j1,k2,j2;
                                                  int ancho = (w->vfin)%100 - (w->vini)%100 + 1;   /* ancho destino */
                                                  int oncho = (wo->vfin)%100 - (wo->vini)%100 + 1; /* ancho origen  */
                                                  int olto = (wo->vfin)/100 - (wo->vini)/100 + 1; /* alto origen  */
                                                  int cini = (wo->vini)%100 - (w->vini)%100;
                                                  int fini = (wo->vini)/100 - (w->vini)/100;
                                                  int ffin = (wo->vfin)/100 - (w->vini)/100;
                                                  unsigned char *wvv = w->vv;
                                                  unsigned char *woo = wo->vv;
                                                  
                                                  
                                                  if (general_int) (*general_int)();
#ifdef ESDOS
#ifndef FGVWIN
                                                  if (timer_especial_int)
                                                     (*timer_especial_int)();
#endif
#endif
                                                  
                                                  
                                                  if ((wo->vini) < (w->vini) || (wo->vfin) > (w->vfin)) return(-1);
                                                  for (i = fini;i <= ffin;i++) {
                                                     k = i*ancho+cini;
                                                     k1 = k + w->size;
                                                     k2 = k1 + w->size;
                                                     j1 = j + wo->size;
                                                     j2 = j1 + wo->size;
                                                     for (l = 0;l < oncho;l++,j++,k++,k1++,k2++,j1++,j2++)
                                                     {
                                                        if (!modo || woo[j] != 176 || woo[j1] != 112 || woo[j2] != 'R')
                                                        {
#ifdef FGVWIN
                                                           if (woo[j] == 219 || woo[j] == 176 || woo[j] == 177 || woo[j] == 178)
                                                           {
                                                              if (modo)
                                                              {
                                                                 /*woo[j1]=wvv[k1]; /* no se pone se deja lo que habia */
                                                                 /*woo[j2]=wvv[k2];
                                                                 //woo[j] =wvv[k];
                                                                 //wo->mapagrafico[j] = w->mapagrafico[k];*/
                                                              }
                                                              else
                                                              {
                                                                 wvv[k] = 1;
                                                                 wvv[k1]=woo[j1];
                                                                 wvv[k2]=woo[j2];
                                                                 w->mapagrafico[k] = 0;
                                                              }
                                                           }
                                                           else
                                                           {
#endif
                                                              wvv[k]=woo[j];
                                                              wvv[k1]=woo[j1];
                                                              wvv[k2]=woo[j2];
#ifdef FGVWIN
                                                              w->mapagrafico[k] = wo->mapagrafico[j];
                                                              
                                                           }
                                                           if (!wo->sacada && !wvv[k])
                                                              if (!no_bitblt)
                                                                 BitBlt(w->vhdc,(cini+l)*scaracter.cx,((w == stdpan) ? espacio_arriba : 0) +i*scaracter.cy,scaracter.cx,scaracter.cy,wo->vhdc, l*scaracter.cx,((wo == stdpan) ? espacio_arriba : 0) +(i-fini)*scaracter.cy ,SRCCOPY);
#endif
                                                        }
                                                        else
                                                        {
                                                           woo[j]=wvv[k];
                                                           woo[j1]=wvv[k1];
                                                           woo[j2]=wvv[k2];
#ifdef FGVWIN
                                                           wo->mapagrafico[j] = w->mapagrafico[k];
                                                           if (!woo[j])
                                                           {
                                                              if (!no_bitblt)
                                                                 BitBlt(wo->vhdc,l*scaracter.cx,((wo == stdpan) ? espacio_arriba : 0) +(i-fini)*scaracter.cy,scaracter.cx,scaracter.cy,w->vhdc, (cini+l)*scaracter.cx,((w == stdpan) ? espacio_arriba : 0) +i*scaracter.cy ,SRCCOPY);
                                                           }
#endif
                                                        }
                                                     }
                                                  }
                                                  
#ifdef FGVWIN
                                                  if (wo->sacada)
                                                  {
                                                     if (!no_bitblt)
                                                        BitBlt(w->vhdc,cini*scaracter.cx,((w == stdpan) ? espacio_arriba : 0) +fini*scaracter.cy,oncho*scaracter.cx,olto*scaracter.cy,wo->vhdc,0,((wo == stdpan) ? espacio_arriba : 0),SRCCOPY);
                                                     wo->sacada = 0;
                                                  }
#endif
                                                  
#ifdef RWXARXA
#ifdef RWSERVER
                                                  if (en_conexion_externa)
                                                  {
                                                     w_mete_en_terminal(w,wo,modo);
                                                  }
#endif
#endif
                                                  return(0);
                                               }
                                               
                                               int w_mete(w,wo)
                                                  VENT *w;            /* ventana de destino */
                                               VENT *wo;           /* ventana de origen  */
                                               {
                                                  return(_w_mete(w,wo,0));
                                               }
                                               
                                               int w_metev(w,wo)
                                                  VENT *w;            /* ventana de destino */
                                               VENT *wo;           /* ventana de origen  */
                                               {
                                                  return(_w_mete(w,wo,1));
                                               }
                                               /*
                                               * w_salva
                                               * guarda el contenido de la stdpan en la pantalla deseada
                                               */
                                               void w_salva(w)
                                                  VENT *w;
                                               {
                                                  w_saca(w,stdpan);
                                               }
                                               
                                               /*
                                               * w_lee  lee una ventana de disco (referida al vertice superior izquierdo)
                                               *
                                               * devuelve -1 si no puede leer el fichero, 0 si va bien;
                                               */
                                               int w_lee(w,tmp)
                                                  VENT *w;
                                               char *tmp;                   /* nombre del fichero de pantalla */
                                               {
                                                  char texto;
                                                  char p[81];
                                                  int i,j,k=0;
                                                  int f,punto;
                                                  int alto  = (w->vfin)/100 - (w->vini)/100 + 1;
                                                  int ancho = (w->vfin)%100 - (w->vini)%100 + 1;
                                                  int tam = alto*ancho;
                                                  short ppp;
                                                  unsigned char *wvv = w->vv;
                                                  int comprimido = 0;
                                                  char *otmp = tmp;
                                                  
                                                  if (general_int) (*general_int)();
#ifdef ESDOS
#ifndef FGVWIN
                                                  if (timer_especial_int)
                                                     (*timer_especial_int)();
#endif
#endif
                                                  
                                                  i = 0;
                                                  punto = -1;
                                                  while(*tmp)
                                                  {
                                                     p[i] = *tmp;
                                                     if (p[i] == '.')
                                                        punto = i;
                                                     tmp++;
                                                     i++;
                                                  }
                                                  p[i] = *tmp;
                                                  
                                                  if (punto == -1)
                                                  {
                                                     punto = i;
                                                     strcat(p,".pan");
                                                  }
                                                  
                                                  f = rw_open_(p,O_RDONLY | O_BINARY,S_IWRITE | S_IREAD);
                                                  if (f < 0) {
#ifdef FGVWIN
                                                     p[punto] = 0;
                                                     if (!carga_grafico(p,w)) {p[punto] = '.';return(0);}
                                                     p[punto] = '.';
#endif
                                                     w_cls(w);
#ifdef RWXARXA
#ifdef RWSERVER
                                                     if (en_conexion_externa)
                                                     {
                                                        w_lee_terminal(w,otmp);
                                                     }
#endif
#endif
                                                     return(f);
                                                  }
                                                  j = rw_read_(f,&texto,1);
                                                  if (j != 1 || texto) { /* ATENCION PROBAR FORMATO ANTIGUO!!! */
                                                     rw_close_(f);
#ifdef FGVWIN
                                                     p[punto] = 0;
                                                     if (!carga_grafico(p,w)) {p[punto] = '.';return(0);}
                                                     p[punto] = '.';
                                                     w_cls(w);
#endif
                                                     f = __w__lee_(w,p);
#ifdef RWXARXA
#ifdef RWSERVER
                                                     if (en_conexion_externa)
                                                     {
                                                        w_lee_terminal(w,otmp);
                                                     }
#endif
#endif
                                                     return(f);
                                                  }
                                                  
                                                  rw_read_(f,(char *)&ppp,sizeof(short));
                                                  moto_short(&ppp);
                                                  if (!ppp)
                                                  {
                                                     comprimido = 1;
                                                     rw_read_(f,(char *)&ppp,sizeof(short));
                                                     moto_short(&ppp);
                                                  }
                                                  
                                                  alto  = ppp / 100;
                                                  ancho = ppp % 100;
                                                  
                                                  if (tam != (alto * ancho)) {
                                                     j = tam;
                                                     tam = alto * ancho;
                                                     if (tam > j)
                                                     {
                                                        if (w->size > 0)
                                                        {
#ifdef FGVWIN
                                                           if (w->mapagrafico)
                                                              free(w->mapagrafico);
                                                           w->mapagrafico = malloc(tam);
                                                           memset(w->mapagrafico,0,tam);
#endif
                                                           free(wvv);
                                                           w->vv = wvv = malloc(tam*3);				  
#ifdef FGVWIN
                                                           crea_bmp_w(w,0);
#endif
                                                        }
                                                     }
                                                     w->size = tam;
                                                     alto  += (w->vini/100 - 1);
                                                     ancho += (w->vini%100 - 1);
                                                     if (alto > 24 || ancho > 80) {
                                                        if (alto > 24) {
                                                           alto -= 24;
                                                           alto = w->vini/100 - alto;
                                                        }
                                                        else
                                                           alto = w->vini/100;
                                                        if (ancho > 80) {
                                                           ancho -= 80;
                                                           ancho = w->vini%100 - ancho;
                                                        }
                                                        else
                                                           ancho = w->vini%100;
                                                        w->vini = alto * 100 + ancho;
                                                     }
                                                     w->vfin = w->vini + ppp - 101;
                                                  }
                                                  
#ifdef FGVWIN
                                                  p[punto] = 0;
                                                  if (!carga_grafico(p,w)) {rw_close_(f);p[punto] = '.';return(0);}
                                                  p[punto] = '.';
                                                  w_cls(w);
#endif
                                                  
                                                  
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
#ifdef RWXARXA
#ifdef RWSERVER
                                                  if (en_conexion_externa)
                                                  {
                                                     w_lee_terminal(w,otmp);
                                                  }
#endif
#endif
                                                  return(j);
}

/*
* w_graba  graba una ventana en disco (referida al vertice superior izquierdo)
* w_graba(w,"/hijoputas/pedro.pan");
* devuelve -1 si no puede grabar el fichero, 0 si va bien;
*/
int w_graba(w,p)
VENT *w;
char *p;                   /* nombre del fichero de pantalla */
{
   int j,f,i,k,compresion = 0;
   int alto  = (w->vfin)/100 - (w->vini)/100 + 1;
   int ancho = (w->vfin)%100 - (w->vini)%100 + 1;
   short ppp,yyy,iii;
   int tam = alto*ancho;
   unsigned char cosa;
   unsigned char *wvv = w->vv;
   
   
   if (general_int) (*general_int)();
#ifdef ESDOS
#ifndef FGVWIN
   if (timer_especial_int)
      (*timer_especial_int)();
#endif
#endif
   
#ifdef FGVWIN
   /* graba_grafico(p,w); */
#endif
   
   f = rw_open_(p,O_RDWR | O_BINARY | O_TRUNC | O_CREAT,S_IREAD | S_IWRITE);
   if (f < 0) return(f);
   j = 0;
   rw_write_(f,(char *)&j,1);
   /* estudiar compresion */
   k = tam*3;
   cosa = *wvv;
   for (i = 1;i < k;i++)
   {
      if (cosa == *(wvv+i))
      {
         j++;
         if (j > 6)
         {
            compresion = 1;
            break;
         }
      }
      else
      {
         j = 0;
         cosa = *(wvv+i);
      }
   }
   if (compresion)
   {
      ppp = 0;
      moto_short(&ppp);
      rw_write_(f,(char *)&ppp,sizeof(short));
   }
   ppp = alto*100+ancho;
   moto_short(&ppp);
   rw_write_(f,(char *)&ppp,sizeof(short));
   if (compresion)
   {
      cosa = *wvv;
      j = 0;
      yyy = 0;
      iii = 0;
      for (i = 1;i < k;i++)
      {
         if (cosa == *(wvv+i))
         {
            yyy++;
         }
         else
         {
            if (yyy > 6)
            {
               if (iii)
               {
                  ppp = iii;
                  moto_short(&ppp);
                  rw_write_(f,(char *)&ppp,sizeof(short));
                  rw_write_(f,wvv+j,iii);
                  j += iii;
                  iii = 0;
               }
               ppp = (-yyy);
               moto_short(&ppp);
               rw_write_(f,(char *)&ppp,sizeof(short));
               rw_write_(f,wvv+j,1);
               j++;
               j += yyy;
               yyy = 0;
            }
            else
            {
               iii += yyy;
               yyy = 0;
               iii++;
            }
            cosa = *(wvv+i);
         }
      }  
      if (yyy < 5)
      {
         yyy++;
         iii += yyy;
         yyy = 0;
      }
      if (iii)
      {
         ppp = iii;
         moto_short(&ppp);
         rw_write_(f,(char *)&ppp,sizeof(short));
         rw_write_(f,wvv+j,iii);
         j += iii;
         iii = 0;       
      }
      if (yyy)
      {
         ppp = (-yyy);
         moto_short(&ppp);
         rw_write_(f,(char *)&ppp,sizeof(short));
         rw_write_(f,wvv+j,1);    
         j += yyy;
         yyy = 0;
      }
   }
   else
   {
      j = rw_write_(f,wvv,tam*3);
   }
   if (j != tam*3) j = -1; else j = 0;
   rw_close_(f);
   return(j);
}

/*
* w_mueve
* desplaza una pantalla a la posicion inicial indicada
*/
void w_mueve(w,p)
VENT *w;
int p;
{
   int n = w->vfin - w->vini;
   
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      w_mueve_terminal(w,p);
   }
#endif
#endif
   
   
   w->vini = p;
   w->vfin = p + n;
   
}

/*
* w_cls
* pone en blanco una ventana
*/
void w_cls(w)
VENT *w;
{
   int size = w->size;
#ifdef FGVWIN
   int alto  = ((w->vfin)/100 - (w->vini)/100 + 1) * scaracter.cy;
   int ancho = ((w->vfin)%100 - (w->vini)%100 + 1) * scaracter.cx;
   RECT espacio;
   
   if (w == stdpan)
			{
      if (!no_bitblt)
      {
         hOldBitmap = SelectObject(hMemoryDC,agitapiz);
         BitBlt(w->vhdc,0,0,principal_nx,principal_ny,hMemoryDC,0,0,SRCCOPY);
         SelectObject(hMemoryDC,hOldBitmap);
         hOldBitmap = NULL;
         if (win_fondo_neutro)
         {
            SetRect(&espacio,0,espacio_arriba+scaracter.cy*4-3,scaracter.cx*80,espacio_arriba+scaracter.cy*23);
            FillRect(w->vhdc,&espacio,agi_brush_gris);
         }
      }
      memset(w->vv,1,size); /* codigo de tapiz */
      if (w->mapagrafico)
         memset(w->mapagrafico,0,size);
			}
   else
      memset(w->vv,' ',size);
#else
   memset(w->vv,' ',size);
#endif
   memset(w->vv+size,(((PFcolor << 4) & 0xf0) | PCcolor),size);
   memset(w->vv+2*size,Patrib,size);
   
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa && w != stdpan)
			{
      w_cls_en_terminal(w);
			}
#endif
#endif
}

/*
* w_wll
* pone en blanco una linea de la ventana
*/
void w_wll(w,l)
VENT *w;
int l;       /* numero de linea a borrar tomando como origen 1 la */
/* primera linea de la ventana independientemente    */
/* de su posicion en la pantalla  (w->vini)          */
{
			int j;
         int alto  = (w->vfin)/100 - (w->vini)/100 + 1;
         int ancho = (w->vfin)%100 - (w->vini)%100 + 1;
         char i;
         
         if (general_int) (*general_int)();
#ifdef ESDOS
#ifndef FGVWIN
         if (timer_especial_int)
            (*timer_especial_int)();
#endif
#endif
         
#ifdef RWXARXA
#ifdef RWSERVER
         if (en_conexion_externa && w != stdpan)
         {
            w_wll_en_terminal(w,l);
         }
#endif
#endif
         
#ifdef FGVWIN
#ifdef COMENTARIO
         if (w == stdpan && (j = w->vini/100+l-2) != 24)
         {
            if (j < 4 || j > 22)
            {
               if (!no_bitblt)
                  hOldBitmap = SelectObject(hMemoryDC,agitapiz);
               if (j < 3) /* borrar en la cabecera es borrarla toda*/
               {
                  if (!no_bitblt)
                     BitBlt(stdpan->vhdc,0,0,640,70,hMemoryDC,0,0,SRCCOPY);
               }
               else
                  if (j == 3)
                  {
                     if (!no_bitblt)
                        BitBlt(stdpan->vhdc,0,espacio_arriba+scaracter.cy*4-24,640,24,hMemoryDC,0,espacio_arriba+scaracter.cy*4-24,SRCCOPY);
                  }
                  else
                     if (j == 23)
                     {
                        if (!no_bitblt)
                           BitBlt(stdpan->vhdc,0,espacio_arriba+scaracter.cy*23,640,24,hMemoryDC,0,espacio_arriba+scaracter.cy*23,SRCCOPY);
                     }
                     else
                     {
                        if (!no_bitblt)
                           BitBlt(stdpan->vhdc,0,espacio_arriba+scaracter.cy*j,scaracter.cx*80,scaracter.cy,hMemoryDC,0,espacio_arriba+scaracter.cy*j,SRCCOPY);
                     }
                     
                     
                     if (!no_bitblt)
                     {
                        SelectObject(hMemoryDC,hOldBitmap);
                        hOldBitmap = NULL;
                     }
                     memset(stdpan->vv+j*80,0,80);
                     return;
            }
         }
#endif
#endif
         
         
         if (l>alto || l<1) return;
         for (j=0;j<3;j++) {
            if (j == 2 && (Patrib & (~128)) > 64 && (Patrib & (~128)) < 91)
               i = Patrib;
            else {
               if (j == 1)
                  i = (((PFcolor << 4) & 0xf0) | PCcolor);
               else
                  i = ' ';
            }
            memset((w->vv)+j*(w->size)+(l-1)*ancho,i,ancho);
         }
}

/*
* w_ll
* pone en blanco una linea de la ventana respecto de su posicion en pantalla
*/
void w_ll(w,l)
VENT *w;
int l;
{
   w_wll(w,l - (w->vini)/100 + 1);
}


/*
* w_print
* pinta un string en la ventana
* devuelve el numero de caracteres pintados
*/
int w_print(v,s)
VENT *v;
register char *s;
{
   register int i=0;
   unsigned char c;
   int fil = (v->vfin)%100 - (v->vini)%100 + 1;  /* columnas de la ventana */
   int h = ((Pcursor/80+1) - (v->vini)/100) * fil; /* despazamiento filas */
   int g = (Pcursor%80+1) - ((v->vini)%100); /* desplazamiento columnas */
   
   if (general_int) (*general_int)();
   
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa && v != stdpan)
   {
						w_print_en_terminal(v,s);
   }
#endif
#endif
   
#ifdef ESDOS
#ifndef FGVWIN
   if (timer_especial_int)
						(*timer_especial_int)();
#endif
#endif
   
   while (*s && (h+g+i) < (v->size) ) {
						c = *s;
#ifdef FGVWIN
                  if (v == stdpan && (c == 219 || c == 176 || c == 177 || c == 178))
                     c == 1;
#endif
                  
                  *((v->vv) + h + g + i) = c;
                  *((v->vv) + (v->size) + h + g + i) = (unsigned char)(((PFcolor << 4) & 0xf0) | PCcolor); /* Color Especificado */
                  *((v->vv) + 2*(v->size) + h + g + i) = Patrib;
                  Pcursor++;
                  i++;
                  *s++;
   }
   return(i);
}

/*
* v_print
* printa un string en la stdpan y lo refresca
* devuelve el numero de caracteres pintados
*/
int v_print(s)
unsigned char *s;
{
   unsigned char *os = s;
#ifndef ESDOS
   int i = 0,h;
   char Grf = ' ';
   unsigned char c;
   if (atento_imp) return(0);
   
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      print_en_terminal(s);
   }
   else
   {
#endif
#endif
      
      /* if (atento_imp) i_acabo(_imp);   */
      if ((Pcursor/80) < 24 || linea_25)
         while(*s) {
            if ((c = *s) > 127 && (h = C_conversion[c-128]))  {
               if (h >= GRVE && h <= GRC8) {/* es semi GRAFICO */
                  if (Grf == ' ') {
                     Gon();
                     Grf = 'G';
                  }
               }
               else {
                  if (Grf != ' ') {
                     Goff();
                     Grf = ' ';
                  }
               }
               pinta(control[h],NUMCARCO,0);
            }
            else {
               if (Grf != ' ') {
                  Goff();
                  Grf = ' ';
               }
               putchar(c);
            }
            s++;
									}
         if (Grf != ' ')
            Goff();
         /* if (atento_imp) i_inicio(_imp); */
#ifdef RWXARXA
#ifdef RWSERVER
   }
#endif
#endif
#endif
#ifdef ESDOS
#ifdef FGV_MAXDOS
#ifdef FGV32
#ifndef FGVWIN
   dos_print(s);
#else
   dosprint(s,dos_atri());
#endif
#else
   dosprint(s,dos_atri());
#endif
#else
   DOSPRINT(s,dos_atri(),sizeof(char *),control);
#endif
#endif
   return(w_print(stdpan,os));
}

/*
* w_printd
* pinta un numero entero en la ventana
*/
int w_printd(v,u,s)
VENT *v;
char *u;
int s;
{
   char tmp[S_BUF];
   
   sprintf(tmp,u,s);
   return(w_print(v,poncoma(tmp,tmp)));
}

/*
* w_prints
* pinta un string en la ventana
*/
int w_prints(v,u,s)
VENT *v;
char *u;
char *s;
{
   char tmp[S_BUF];
   
   sprintf(tmp,u,s);
   return(w_print(v,tmp));
}

/*
* w_printc
* pinta un caracter en la ventana
*/
int w_printc(v,u,s)
VENT *v;
char *u;
char s;
{
   char tmp[S_BUF];
   
   sprintf(tmp,u,s);
   return(w_print(v,tmp));
}

/*
* w_printl
* pinta un long en la ventana
*/
int w_printl(v,u,s)
VENT *v;
char *u;
long s;
{
   char tmp[S_BUF];
   
   sprintf(tmp,u,s);
   return(w_print(v,tmp));
}

/*
* w_printF
* pinta un numero float en la ventana
*/
int w_printF(v,u,s)
VENT *v;
char *u;
double s;
{
   char tmp[S_BUF];
   
   sprintf(tmp,u,s);
   return(w_print(v,poncoma(tmp,tmp)));
}

/*
* v_printd
* pinta un numero entero en la stdpan
*/
int v_printd(u,s)
char *u;
int s;
{
   char tmp[S_BUF];
   
   sprintf(tmp,u,s);
   return(v_print(poncoma(tmp,tmp)));
}

/*
* v_prints
* pinta un string en la stdpan
*/
int v_prints(u,s)
char *u;
char *s;
{
   char tmp[S_BUF];
   
   sprintf(tmp,u,s);
   return(v_print(tmp));
}

/*
* v_printc
* pinta un caracter en la stdpan
*/
int v_printc(u,s)
char *u;
char s;
{
   char tmp[S_BUF];
   
   sprintf(tmp,u,s);
   return(v_print(tmp));
}

/*
* v_printl
* pinta un long en la stdpan
*/
int v_printl(u,s)
char *u;
long s;
{
   char tmp[S_BUF];
   
   sprintf(tmp,u,s);
   return(v_print(tmp));
}

/*
* v_printF
* pinta un numero float en la stdpan
*/
int v_printF(u,s)
char *u;
double s;
{
   char tmp[S_BUF];
   
   sprintf(tmp,u,s);
   return(v_print(poncoma(tmp,tmp)));
}

void qursor(a)
register int a;
{
   Pcursor = (a/100-1)*80 + a%100 - 1;
}

#ifndef ESDOS
/*
* cursor posiciona el cursor en a/100,a%100
* 0-ansi,vt100,teletype,apple lisa :\E[%i%d,%dH:
* 1-ibm3161,vt52,adds viewpoint,regent25a,visual 200,teleray,
*   cromenco :\EY%+ %+ :
* 2-h1500 :~^Q%r%.%+ :
* 3-Esprit 6110 emulando hazeltine
* 4-altosII,c.itoh 80,zilog,seiko,macintosh,onyx :\E[%i%2;%2H:
* 5-tvi925,ampex,wyse,adm3a :\E=%+ %+ :
* 6-Perkin elmer :\EX%+ \EY%+ :
*/
void cursor(a)
register int a;
{
   
   if (atento_imp) return;
   
   
   
   if (Prefresco)
      qursor(a);
   
   
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      if (curon(-1))
      {
         cursor_en_terminal(a);
      }
      return;
   }
#endif
#endif
   
   
   /* if (atento_imp) i_acabo(_imp);   */
   switch (fcursor) {
   case 0:
      printf("\033[%d;%dH",a/100,a%100);
      break;
   case 1:
      printf("\033Y%c%c",a/100+31,a%100+31);
      break;
   case 2:
      printf("\176\021%c%c",a%100+31,a/100+31);
      break;
   case 3:
      printf("\176\021%c%c",(((a%100)<31) ? (a%100 +95) : (a%100 -1)),
         (a/100 +95));
      break;
   case 4:
      printf("\033[%2d;%2dH",a/100,a%100);
      break;
   case 5:
      printf("\033=%c%c",a/100+31,a%100+31);
      break;
   case 6:
      printf("\033X%c\033Y%c",a/100+31,a%100+31);
      break;
   default :
      break;
   }
   /* if (atento_imp) i_inicio(_imp);   */
}
#endif

#ifdef ESDOS

/*
* cursor posiciona el cursor en a/100,a%100
*/
void cursor(a)
register int a;
{
#if !defined(FGVWIN) && defined(FGV32)
#ifndef WATCOMNLM
   COORD  coordCursor;
#else
   char f;
   char c;
#endif
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      if (curon(-1))
      {
         cursor_en_terminal(a);
      }
   }
#endif
#endif
   
#ifndef WATCOMNLM
   if (myconsola != INVALID_HANDLE_VALUE)
   {
      if (curon(-1))
      {
         coordCursor.X = (a%100)-1;
         coordCursor.Y = (a/100)-1;
         SetConsoleCursorPosition(myconsola,coordCursor);
      }
   }
#else
   f = a/100;
   c = a % 100;
   gotoxy((int)c-1,(int)f-1);
#endif
   
#else
   char f;
   char c;
   
#ifndef FGV_MAXDOS
   extern int DOSCURSOR(char,char);
#endif
   
   f = a/100;
   c = a % 100;  /* cambios para MS DOS */
   
#ifdef FGV_MAXDOS
   
#ifdef FGVWIN
   w_wcursor(c,f);
#else
   gotoxy((int)c,(int)f);
#endif
#else
   DOSCURSOR(f,c);
#endif
#endif
   qursor(a);
}
#endif


/*
* cls  borra la pantalla
*/
void cls()
{
   
   if (Prefresco) {
      v_cls();
      cursor(101);
      Pgrafi = ' ';
      Patrib = ' ';    /* cambios para MS DOS */
   }
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      cls_en_terminal();
   }
#endif
#endif
   
#ifdef ESDOS
#ifdef FGV_MAXDOS
#ifdef FGVWIN
   
   v_refresca();
#else
#ifdef FGV32
#ifdef WATCOMNLM
   clrscr();
   _textattr(dos_atri(),NULL,-1);
#else
   textattr(dos_atri());
   clrscr();
#endif
#else
   textattr(dos_atri());
   clrscr();
#endif
#endif
#else
   /* DOSCLS(control[NORCOLOR][0] + control[NORFONDO][0] * 16 + control[NORMAL][0]); */
   DOSCLS(dos_atri());
#endif
#else
   pinta(control[BORRARPAN],NUMCARCO,0);
#endif
}

/*
* ll   borra la linea donde esta posicionado el cursor
*/
void ll()
{
#ifdef ESDOS
   char buf[81];
   
   memset(buf,' ',80);
   buf[80] = 0;
#ifdef FGV_MAXDOS
#ifdef FGV32
#ifndef FGVWIN
   dos_print(buf);
#else
   dosprint(buf,dos_atri());
#endif
#else
   dosprint(buf,dos_atri());
#endif
#else
   DOSPRINT(buf,dos_atri(),sizeof(char *),control);
#endif
   if (Prefresco)
      v_ll(Pcursor/80 + 1);   /* cambios para MS DOS */
#else
   if (atento_imp) return;
   
   
   if (Prefresco)
      v_ll(Pcursor/80 + 1);
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      Bl_en_terminal(Pcursor/80 + 1);
      return;
   }
#endif
#endif
   if (strlen(control[BORRARLIN]) == 0) {
      /*      if (atento_imp) i_acabo(_imp); */
      if ((Pcursor/80) < 24 || linea_25)
         printf("%80s"," ");
      /*      if (atento_imp) i_inicio(_imp); */
   }
   else
      pinta(control[BORRARLIN],NUMCARCO,0);
#endif
}

/*
* Bl
* borra la linea que se indica
*/
void Bl(lin)
int lin;
{
#ifdef FGVWIN
#ifdef COMENTARIO
   HDC hdc;
   if (lin < 5 || lin > 23)
   {
      w_wll(stdpan,lin);
      if (lin == 25)
      {	     
         SetWindowText(principal_w,NombreClase);
         return;
      }
      lin--;
      
      if (!IsIconic(principal_w) && !NO_PINTAR_AHORA)
      {
         if (!no_bitblt)
            hdc = GetDC(principal_w);
         
         if (lin < 3) /* borrar en la cabecera es borrarla toda*/
         {
            if (!no_bitblt)
               BitBlt(hdc,principal_rect.left,principal_rect.top,640,70,stdpan->vhdc,0,0,SRCCOPY);
         }
         else
            if (lin == 3)
            {
               if (!no_bitblt)
                  BitBlt(hdc,principal_rect.left,principal_rect.top+espacio_arriba+scaracter.cy*4-24,640,24,stdpan->vhdc,0,espacio_arriba+scaracter.cy*4-24,SRCCOPY);
            }
            else
               if (lin == 23)
               {
                  if (!no_bitblt)
                     BitBlt(hdc,principal_rect.left,principal_rect.top+espacio_arriba+scaracter.cy*23,640,24,stdpan->vhdc,0,espacio_arriba+scaracter.cy*23,SRCCOPY);
               }
               else
               {
                  if (!no_bitblt) 
                     BitBlt(hdc,principal_rect.left,principal_rect.top+espacio_arriba+scaracter.cy*lin,scaracter.cx*80,scaracter.cy,stdpan->vhdc,0,espacio_arriba+scaracter.cy*lin,SRCCOPY);
               }
               if (!no_bitblt)
                  ReleaseDC(principal_w,hdc);
      }
      return;
   }
#endif
#endif
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      Bl_en_terminal(lin);
   }
   en_conexion_externa_stop++;
#endif
#endif
   cursor(lin*100+1);
   ll();
   cursor(lin*100+1);
   fflush(stdout);
#ifdef RWXARXA
#ifdef RWSERVER
   en_conexion_externa_stop--;
#endif
#endif
}

/*
* pito  hace sonar el pito
*/
#ifndef FGVWIN
void pito()
{
#ifndef ESDOS
   if (atento_imp) return;
#endif
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      pito_en_terminal();
   }
#endif
#endif
#if defined(FGV32) && !defined(WATCOMNLM)
   if (myconsola != INVALID_HANDLE_VALUE)
      MessageBeep(MB_ICONEXCLAMATION);
#else
   /* if (atento_imp) i_acabo(_imp); */
#ifdef WATCOMLM
   RingTheBell();
#else
   putc('\007',stdout);
   fflush(stdout);
#endif
   /* if (atento_imp) i_inicio(_imp); */
#endif
}
#endif

/*
* Color funcion que disdpone el color de la impresion
*
*
*
*
*/
void Color(c,f)
int c, /* color del caracter */
f; /* color del fondo */
{
   if (c > 15 || c < 0 || f > 7 || f < 0) return;
   _PCcolor = c;
   _PFcolor = f;
}

void pon_color(a,b,c,m)
int a,b,c,m;
{
   int i;
   
#ifndef ESDOS
   if (PCcolor > 7 && !m) /* ESTO NO SE DEBE EJECUTAR NORMALMENTE */
      pinta(control[BAJAINTEN],NUMCARCO,1);
#endif
   
   if (_PCcolor < -1 || _PCcolor > 15) _PCcolor = -1;
   if (_PFcolor < -1 || _PFcolor > 7)  _PFcolor = -1;
   if (!c && _PCcolor > -1) {
      if (_PCcolor > 7) i = 8;else i = 0;
      if (control[CNEGRO+_PCcolor-i][0]) {/*El resultado Practico en w_refresca*/
         PCcolor = _PCcolor;            /*es el mismo */
         if (a == INTCOLOR && i)
            PCcolor -= i;
      }
      else {
         PCcolor = control[a][0];
         if (i && PCcolor < 8 && PCcolor > 0 && a != INTCOLOR && a != REVCOLOR)
            PCcolor += 8;
      }
      _PCcolor = -1;
   }
   else
      PCcolor = control[a][0];
   
   if (!c && _PFcolor > -1) {
      if (control[FNEGRO+_PFcolor][0])
         PFcolor = _PFcolor;
      else
         PFcolor = control[b][0];
      _PFcolor = -1;
   }
   else
      PFcolor = control[b][0];
   
#ifndef ESDOS
   if (PCcolor > 7 && !m) {
      i = 8;
      pinta(control[ALTAINTEN],NUMCARCO,1);
   }
   else
      i = 0;
   if (!m) {
      pinta(control[CNEGRO+PCcolor-i],NUMCARCO,1);
      pinta(control[FNEGRO+PFcolor],NUMCARCO,1);
   }
#endif
}


#ifdef ESDOS
void pon_color_a(c)
#else
void pon_color_a(c)
#endif
char c;
{
   switch(c) {
   case '0' :
   case ' ' :
   case 'N' :
   case 'n' :
      pon_color(NORCOLOR,NORFONDO,0,0);
      break;
   case 'I' :
      pon_color(INTCOLOR,INTFONDO,0,0);
      break;
   case 'P' :
      pon_color(PARCOLOR,PARFONDO,0,0);
      break;
   case 'R' :
      pon_color(REVCOLOR,REVFONDO,0,0);
      break;
   case 'S' :
      pon_color(SUBCOLOR,SUBFONDO,0,0);   
      break;
   default  :
      break;
   }
}



#ifdef ESDOS
void Nor()
{
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      atributo_en_terminal('N',_PCcolor,_PFcolor);
   }
#endif
#endif
   Patrib = ' ';
   pon_color(NORCOLOR,NORFONDO,0,0);
}

/*
* Ar   abrir reverse
*/
void Ar()
{
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      atributo_en_terminal('R',_PCcolor,_PFcolor);
   }
#endif
#endif
   Patrib = 'R';    /* cambios para MS DOS */
   pon_color(REVCOLOR,REVFONDO,0,0);
}

/*
* Cr   cerrar reverse
*/
void Cr()
{
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      atributo_en_terminal('r',_PCcolor,_PFcolor);
   }
#endif
#endif
   Patrib = ' ';    /* cambios para MS DOS */
   pon_color(NORCOLOR,NORFONDO,1,0);
}

/*
* Ai   alta intensidad
*/
void Ai()
{
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      atributo_en_terminal('I',_PCcolor,_PFcolor);
   }
#endif
#endif
   Patrib = 'I';    /* cambios para MS DOS */
   pon_color(INTCOLOR,INTFONDO,0,0);
}

/*
* Bi   baja intensidad
7*/
void Bi()
{
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      atributo_en_terminal('i',_PCcolor,_PFcolor);
   }
#endif
#endif
   Patrib = ' ';    /* cambios para MS DOS */
   pon_color(NORCOLOR,NORFONDO,1,0);
}

/*
* Ap   abrir parpadeo
*/
void Ap()
{
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      atributo_en_terminal('P',_PCcolor,_PFcolor);
   }
#endif
#endif
   Patrib = 'P';    /* cambios para MS DOS */
   pon_color(PARCOLOR,PARFONDO,0,0);
}

/*
* Cp   cerrar parpadeo
*/
void Cp()
{
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      atributo_en_terminal('p',_PCcolor,_PFcolor);
   }
#endif
#endif
   Patrib = ' ';    /* cambios para MS DOS */
   pon_color(NORCOLOR,NORFONDO,1,0);
}

/*
* As   abrir subrayado
*/
void As()
{
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      atributo_en_terminal('S',_PCcolor,_PFcolor);
   }
#endif
#endif
   Patrib = 'S';    /* cambios para MS DOS */
   pon_color(SUBCOLOR,SUBFONDO,0,0);
}

/*
* Cs   cerrar subrayado
*/
void Cs()
{
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      atributo_en_terminal('s',_PCcolor,_PFcolor);
   }
#endif
#endif
   Patrib = ' ';    /* cambios para MS DOS */
   pon_color(NORCOLOR,NORFONDO,1,0);
}

/*
* Gon  activar graficos
*/
void Gon()
{
   if (Prefresco)
      Pgrafi = 'a';
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      atributo_en_terminal('Z',-1,-1);
   }
#endif
#endif
}

/*
* Goff  desactivar graficos
*/
void Goff()
{
   if (Prefresco)
      Pgrafi = ' ';
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      atributo_en_terminal('Z',-1,-1);
   }
#endif
#endif
}				   

/*
 * Grc1  cuadro grafico 1
 */
void Grc1()
{
   if (Prefresco)
      Pgrafi = 'l';
}

/*
 * Grc2  cuadro grafico 2
 */
void Grc2()
{
   if (Prefresco)
      Pgrafi = 'm';
}



/*
 * Gra   imprimir graficos en pantalla
 * 0-vertical  1-horizontal  2-s.i.  3-s.c.  4-s.d.  5-c.i.  6-c.c.
 * 7-c.d.  8-i.i.  9-i.c.  10-i.d. 11- cg1 12- cg2
 */
void Graf(i)
register int i;
{
#ifdef FGV_MAXDOS
   unsigned char c;
#ifndef FGVWIN
#ifndef FGV32
   register unsigned char *pant = direc_ppanta + Pcursor * 2;
#else
#ifndef WATCOMNLM
   COORD backgLoc;
   DWORD dwCharsWritten;
#endif
   
#endif
#endif
#endif
   if (i > 47) return;
   
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      graf_en_terminal(i);
   }
#endif
#endif
   
#ifdef FGV_MAXDOS
#ifndef FGVWIN
   c = cod_i_graf[i];
   if (c > 127) {
      if ((c = C_conversion[c-128]))
      {
         c = control[c][0];
      }
   }
#ifdef FGV32
#ifndef WATCOMNLM
   if (myconsola != INVALID_HANDLE_VALUE)
   {
      backgLoc.X = Pcursor%80;
      backgLoc.Y = Pcursor/80;
      WriteConsoleOutputCharacter(myconsola,&c,1,backgLoc,&dwCharsWritten);
      FillConsoleOutputAttribute(myconsola,dos_atri(),1,backgLoc,&dwCharsWritten);
   }
#else
   /*putch(c);*/
   _textattr(dos_atri(),&c,1);
#endif
#else
   
   *pant = c;
   *(pant+1) = dos_atri();
   
#endif
#else
   w_winprint(cod_i_graf+i,dos_atri(),1);
#endif
#endif
   if (Prefresco) {
      if (Pgrafi == ' ')
         v_putc(cod_i_graf[i]);
      else
         v_putc(i);
      Pcursor++;
   }
#ifndef FGV_MAXDOS
   DOSPUTC((int)cod_i_graf[i],dos_atri(),sizeof(char *),control);
#endif
}

#else

void a_nor()
{
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      atributo_en_terminal('N',_PCcolor,_PFcolor);
   }
#endif
#endif
   if (PCcolor > 7) {
      pinta(control[BAJAINTEN],NUMCARCO,1);
      PCcolor -= 8;
   }
   pinta(control[NORMAL],NUMCARCO,1);
   pon_color(NORCOLOR,NORFONDO,1,0);
}

void Nor()
{
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      atributo_en_terminal('N',_PCcolor,_PFcolor);
   }
#endif
#endif
   
   if (Prefresco)
      Patrib = ' ';
   if (PCcolor > 7) {
      pinta(control[BAJAINTEN],NUMCARCO,1);
      PCcolor -= 8;
   }
   pinta(control[NORMAL],NUMCARCO,1);
   pon_color(NORCOLOR,NORFONDO,0,0);
   
   
}

/*
* Ar   abrir reverse
*/
void Ar()
{
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      atributo_en_terminal('R',_PCcolor,_PFcolor);
   }
#endif
#endif
   if (Prefresco)
      Patrib = 'R';
   if (PCcolor > 7) {
      pinta(control[BAJAINTEN],NUMCARCO,1);
      PCcolor -= 8;
   }   
   pinta(control[ABRIRREV],NUMCARCO,1);
   pon_color(REVCOLOR,REVFONDO,0,0);
}

/*
* Cr   cerrar reverse
*/
void Cr()
{
   if (Prefresco)
      Patrib = ' ';
   pinta(control[CERRARREV],NUMCARCO,1);
   a_nor();      
}

/*
* Ai   alta intensidad
*/
void Ai()
{
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      atributo_en_terminal('I',_PCcolor,_PFcolor);
   }
#endif
#endif
   if (Prefresco)
      Patrib = 'I';
   if (PCcolor > 7) {
      pinta(control[BAJAINTEN],NUMCARCO,1);
      PCcolor -= 8;
   }
   pinta(control[ALTAINTEN],NUMCARCO,1);
   pon_color(INTCOLOR,INTFONDO,0,0);
   
}

/*
* Bi   baja intensidad
*/
void Bi()
{
   if (Prefresco)
      Patrib = ' ';
   pinta(control[BAJAINTEN],NUMCARCO,1);
   a_nor();
}

/*
* Ap   abrir parpadeo
*/
void Ap()
{
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      atributo_en_terminal('P',_PCcolor,_PFcolor);
   }
#endif
#endif
   if (Prefresco)
      Patrib = 'P';
   if (PCcolor > 7) {
      pinta(control[BAJAINTEN],NUMCARCO,1);
      PCcolor -= 8;
   }
   pinta(control[ABRIRPAR],NUMCARCO,1);
   pon_color(PARCOLOR,PARFONDO,0,0);
}

/*
* Cp   cerrar parpadeo
*/
void Cp()
{
   if (Prefresco)
      Patrib = ' ';
   pinta(control[CERRARPAR],NUMCARCO,1);
   a_nor();
}

/*
* As   abrir subrayado
*/
void As()
{
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      atributo_en_terminal('S',_PCcolor,_PFcolor);
   }
#endif
#endif
   if (Prefresco)
      Patrib = 'S';
   if (PCcolor > 7) {
      pinta(control[BAJAINTEN],NUMCARCO,1);
      PCcolor -= 8;
   }
   pinta(control[ABRIRSUB],NUMCARCO,1);
   pon_color(SUBCOLOR,SUBFONDO,0,0);
}

/*
* Cs   cerrar subrayado
*/
void Cs()
{
   if (Prefresco)
      Patrib = ' ';
   pinta(control[CERRARSUB],NUMCARCO,1);
   a_nor();   
}

/*
* Gon  activar graficos
*/
void Gon()
{
   if (Prefresco)
      Pgrafi = 'a';
   pinta(control[GRAFON],NUMCARCO,0);
}

/*
* Goff  desactivar graficos
*/
void Goff()
{
   if (Prefresco)
      Pgrafi = ' ';
   pinta(control[GRAFOFF],NUMCARCO,0);
}



/*
* Gra   imprimir graficos en pantalla
* 0-vertical  1-horizontal  2-s.i.  3-s.c.  4-s.d.  5-c.i.  6-c.c.
* 7-c.d.  8-i.i.  9-i.c.  10-i.d.  11-c.g.1.   12-c.g.2.
*/
void Graf(i)
register int i;
{
   int kk = 0;
   if (i > 47) return;
   if (atento_imp) return;
   
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      graf_en_terminal(i);		
   }
#endif
#endif
   
   if (Prefresco) {
      if ((Pcursor/80) >= 24 && !linea_25) kk = 1;
      v_putc(i);
      Pcursor++;
      if (kk) return;
   }
   
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      return;
   }
#endif
#endif
   /* if (atento_imp) i_acabo(_imp);    */
   putchar(*control[GRVE+i]);
   /* if (atento_imp) i_inicio(_imp);     */
}
#endif


/*
* atributo  activa los atributos
*           mayusculas los activa, minusculas los desactiva
*           0 los desactiva todos
* R-r  reverse
* I-i  intensidad
* S-s  subrayado
* P-p  parpadeo
*/

void atributo(c)
char c;
{
   switch((c & (~128))) {
   case '0' :
      Cr();
      Cp();
      Cs();
      Bi();
   case ' ' :
      if ((Patrib & (~128)) != ' ' && (Patrib & (~128)) > 'A' && (Patrib & (~128)) < 'Z')
         atributo(tolower((Patrib & (~128))));
   case 'N' :
   case 'n' :
      Nor();
      break;
   case 'I' :
      Ai();
      break;
   case 'i' :
      Bi();
      break;
   case 'P' :
      Ap();
      break;
   case 'p' :
      Cp();
      break;
   case 'R' :
      Ar();
      break;
   case 'r' :
      Cr();
      break;
   case 'S' :
      As();
      break;
   case 's' :
      Cs();
      break;
   default  :
      break;
   }
}


#ifndef ESDOS
static void _time_out(s)
int s;
{
   extern void recu_sig();
   signal(s,recu_sig); /* cualquier otra alarma se recupera !! */
   /* normalmente se debe interrunpir la ejecucion del getc */
}
#endif

#ifdef ESDOS
extern long tiempo = 0L;
/* sacar y poner en rsignal */
extern int comprueba_hora()
{
   if (tiempo <= time((long *)0) && avisar) {
      llama_fun_int(avisar,NULL);
   }
}

#ifndef FGVWIN
#ifdef FGV_MAXDOS

int pulsatecla_(espera)
int espera;
{
#ifdef _MSC_VER
   int c;
   unsigned char car;
   
   if (modo_desatendido && !espera) return(-1);
   
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      if (espera)
      {
         aviso(0,"Espera colada");
      }
      return(pulsatecla_en_terminal());
   }
#endif
#endif
   
   if (hStdIn == INVALID_HANDLE_VALUE) return(807);
   
   if (espera)  
   {
#ifdef FGV32
      if (fIsTNT)
      {
         _asm 
         {
            pushad
               mov ah,07h
               int 21h
               mov car,al
               popad
         }
         c = car;
         return(c);
      }
#endif
#ifdef CONSOLANTINPUT
      if (os_subtipo == 2 && no_especial != 1)
      {
         return(consola_input(espera));
      }
#endif
      c = _getch();
      if (c == 0xE0) c = 0;
      return(c);
   } 
#ifdef FGV32
   if (fIsTNT)
   {
      c = 0;
      _asm 
      {
         pushad
            mov ah,06h	   
            mov dl,0ffh
            int 21h	   
            jz nohay__
            mov car,al
            jmp sihay__
nohay__:
         mov c,-1
sihay__:
         popad
      }
      if (c != -1)
      {
         c = car;
      } 
      return(c);
   }
#endif
#ifdef CONSOLANTINPUT
   if (os_subtipo == 2 && no_especial != 1)
   {
      return(consola_input(espera));
   }
#endif
   if (_kbhit())
   {
      c = _getch();
      if (c == 0xE0) c = 0;
      return(c);
   }
   return(EOF);
#else
   inregs.h.ah = 0x06;
   inregs.h.dl = 0xff;
   intdos( &inregs,&outregs );
   if (outregs.x.flags & 64)
   {
      return(-1);
   }
   else
   {
      return(outregs.h.al);
   }
#endif
}
int pulsatecla()
{
   return(pulsatecla_(0));
}


#ifdef CONSOLANTINPUT

int es_un_caso_cronico(t)
int t;
{
   
   if (t >= 48 && t <= 57)
      return(1);
   switch(t)
   {
   case 219:
   case 221:
   case 226:
   case 188:
   case 190:
   case 189:
   case 222:
   case 186:
   case 187:
      return(1);
   default:
      break;
   }
   return(0);
}

int consola_input(espera)
int espera;
{
   INPUT_RECORD inputBuffer;
   DWORD dwInputEvents;
   int i;
   int cc; 
   static int primero_segundo = 0;
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      if (espera)
      {
         aviso(0,"Control de espera fuera de orden");
      }
      return(pulsatecla_en_terminal());
   }
#endif
#endif
   
   
   
   if (hStdIn == INVALID_HANDLE_VALUE) return(807);
   
   if (primero_segundo)
   {
      cc = primero_segundo;
      primero_segundo = 0;
      return(cc);
   }
   
   do {
      if (!espera)
      {
         GetNumberOfConsoleInputEvents(hStdIn,&dwInputEvents);
         if (!dwInputEvents)
         {
            return(-1);
         }
      }
      if (ReadConsoleInput(hStdIn, &inputBuffer, 1, &dwInputEvents) == FALSE) return(-1);
      if (!dwInputEvents) return(-1);
      
      
      switch (inputBuffer.EventType)
      {
      case KEY_EVENT:
         if (inputBuffer.Event.KeyEvent.bKeyDown == TRUE)
         {
            if (no_especial == 2 && os_subtipo == 2) 
            {
               switch(inputBuffer.Event.KeyEvent.wVirtualKeyCode)
               {
               case VK_RETURN:
                  return(802);
               case VK_BACK:
                  return(803);
               case VK_TAB:
                  if (inputBuffer.Event.KeyEvent.dwControlKeyState & SHIFT_PRESSED)
                     return(805);
                  return(804);
               case VK_ESCAPE:
                  return(806);
               case VK_UP:
                  return(808);
               case VK_DOWN:
                  return(809);
               case VK_LEFT:
                  return(810);
               case VK_RIGHT:
                  return(811);
               case VK_HOME:
                  return(812);
               case VK_PRIOR:
                  return(814);
               case VK_NEXT:
                  return(815);
               case VK_END:
                  if ((inputBuffer.Event.KeyEvent.dwControlKeyState & RIGHT_CTRL_PRESSED) ||
                     (inputBuffer.Event.KeyEvent.dwControlKeyState & LEFT_CTRL_PRESSED))
                     return(817);
                  return(816);
               case VK_INSERT:
                  return(818);
               case VK_DELETE:
                  return(819);
               case VK_F11:
                  return(820);
               case VK_F12:
                  return(822);
               case VK_F1:
                  if (inputBuffer.Event.KeyEvent.dwControlKeyState & (RIGHT_CTRL_PRESSED || LEFT_CTRL_PRESSED))
                     return(833);
                  return(823);
               case VK_F2:
                  if (inputBuffer.Event.KeyEvent.dwControlKeyState & (RIGHT_CTRL_PRESSED || LEFT_CTRL_PRESSED))
                     return(834);
                  return(824);
               case VK_F3:
                  if (inputBuffer.Event.KeyEvent.dwControlKeyState & (RIGHT_CTRL_PRESSED || LEFT_CTRL_PRESSED))
                     return(835);
                  return(825);
               case VK_F4:
                  if (inputBuffer.Event.KeyEvent.dwControlKeyState & (RIGHT_CTRL_PRESSED || LEFT_CTRL_PRESSED))
                     return(836);
                  return(826);
               case VK_F5:
                  if (inputBuffer.Event.KeyEvent.dwControlKeyState & (RIGHT_CTRL_PRESSED || LEFT_CTRL_PRESSED))
                     return(837);
                  return(827);
               case VK_F6:
                  if (inputBuffer.Event.KeyEvent.dwControlKeyState & (RIGHT_CTRL_PRESSED || LEFT_CTRL_PRESSED))
                     return(838);
                  return(828);
               case VK_F7:
                  if (inputBuffer.Event.KeyEvent.dwControlKeyState & (RIGHT_CTRL_PRESSED || LEFT_CTRL_PRESSED))
                     return(839);
                  return(829);
               case VK_F8:
                  if (inputBuffer.Event.KeyEvent.dwControlKeyState & (RIGHT_CTRL_PRESSED || LEFT_CTRL_PRESSED))
                     return(840);
                  return(830);
               case VK_F9:
                  if (inputBuffer.Event.KeyEvent.dwControlKeyState & (RIGHT_CTRL_PRESSED || LEFT_CTRL_PRESSED))
                     return(841);
                  return(831);
               case VK_F10:
                  if (inputBuffer.Event.KeyEvent.dwControlKeyState & (RIGHT_CTRL_PRESSED || LEFT_CTRL_PRESSED))
                     return(842);
                  return(832);
               default:
               /*
               if (stdpan)
               {
               i = Pcursor;
               cursor(101);
               
                 v_printd("%d-",(int)inputBuffer.Event.KeyEvent.wVirtualKeyCode);
                 v_printd("%d-",(int)inputBuffer.Event.KeyEvent.uChar.UnicodeChar);
                 v_printd("%d-",(int)inputBuffer.Event.KeyEvent.uChar.AsciiChar);
                 v_printd("%d       ",(int)(inputBuffer.Event.KeyEvent.dwControlKeyState & ENHANCED_KEY));
                 
                   
                     cursor(i/80*100+i%80+101);
                     }
                  */
                  if (inputBuffer.Event.KeyEvent.uChar.AsciiChar)
                  {
                     i = 0;
                     memcpy(&i,&(inputBuffer.Event.KeyEvent.uChar.AsciiChar),1); /* solo intel :-) */
                     return(i);
                  }
                  break;
             }
             return(-1);
         }
         
         if (inputBuffer.Event.KeyEvent.uChar.UnicodeChar)
         {
            cc = inputBuffer.Event.KeyEvent.uChar.UnicodeChar;
            if (os_subtipo == 2 )
            {
            /*
            i = Pcursor;
            cursor(101);
            v_printd("[%5d] ",inputBuffer.Event.KeyEvent.wVirtualKeyCode);
            v_printd("[%5d] ",inputBuffer.Event.KeyEvent.uChar.UnicodeChar);
            v_printd("[%5d]",inputBuffer.Event.KeyEvent.uChar.AsciiChar);
            v_printd("[%5d]",(inputBuffer.Event.KeyEvent.dwControlKeyState & ENHANCED_KEY));
            cursor(i/80*100+i%80+101);
               */
               
               if (  es_un_caso_cronico(inputBuffer.Event.KeyEvent.wVirtualKeyCode) &&
                  (inputBuffer.Event.KeyEvent.dwControlKeyState & CAPSLOCK_ON) &&
                  !(inputBuffer.Event.KeyEvent.dwControlKeyState & LEFT_ALT_PRESSED) &&
                  !(inputBuffer.Event.KeyEvent.dwControlKeyState & LEFT_CTRL_PRESSED) &&
                  !(inputBuffer.Event.KeyEvent.dwControlKeyState & RIGHT_ALT_PRESSED) &&
                  !(inputBuffer.Event.KeyEvent.dwControlKeyState & RIGHT_CTRL_PRESSED)
                  )
               {
                  if ((inputBuffer.Event.KeyEvent.dwControlKeyState & SHIFT_PRESSED))
                  {  
                     switch(inputBuffer.Event.KeyEvent.wVirtualKeyCode)
                     {
                     case 48:
                        cc = '=';
                        break;
                     case 49:
                        cc = '!';
                        break;
                     case 50:
                        cc = '"';
                        break;				   
                     case	51:
                        cc = '�';
                        break;
                     case 52:
                        cc = '$';
                        break;
                     case 53:
                        cc = '%';
                        break;
                     case 54:
                        cc = '&';
                        break;
                     case 55:
                        cc = '/';
                        break;
                     case 56:
                        cc = '(';
                        break;
                     case 57:
                        cc = ')';
                        break;
                     case 219:
                        cc = '?';
                        break;
                     case 221:
                        cc = (unsigned char)'�';
                        break;
                     case 226:
                        cc = '>';
                        break;
                     case 188:
                        cc = ';';
                        break;
                     case 190:
                        cc = ':';
                        break;
                     case 189:
                        cc = '_';
                        break;
                     case 222:
                        cc = (unsigned char)'�';
                        break;
                     case 186:
                        cc = (unsigned char)'^';
                        break;
                     case 187:
                        cc = '*';
                        break;
                     default:
                        break;
                     }
                  }
                  else
                  {
                     switch(inputBuffer.Event.KeyEvent.wVirtualKeyCode)
                     {
                     case 219:
                        cc = '\'';
                        break;
                     case 221:
                        cc = (unsigned char)'�';
                        break;
                     case 226:
                        cc = '<';
                        break;
                     case 188:
                        cc = (unsigned char)',';
                        break;
                     case 190:
                        cc = (unsigned char)'.';
                        break;
                     case 189:
                        cc = '-';
                        break;
                     case 222:
                        cc = (unsigned char)'�';
                        break;
                     case 186:
                        cc = (unsigned char)'`';
                        break;
                     case 187:
                        cc = '+';
                        break;
                     default:
                        cc = inputBuffer.Event.KeyEvent.wVirtualKeyCode;
                        break;
                     }				    
                  }
              }
           }
           return(cc);
           /*wVirtualKeyCode*/
            }
            switch(inputBuffer.Event.KeyEvent.wVirtualKeyCode)
            {
            case VK_RETURN:
               return(802);
            case VK_BACK:
               return(803);
            case VK_TAB:
               if (inputBuffer.Event.KeyEvent.dwControlKeyState & SHIFT_PRESSED)
                  return(805);
               return(804);
            case VK_ESCAPE:
               return(806);
            case VK_UP:
               return(808);
            case VK_DOWN:
               return(809);
            case VK_LEFT:
               return(810);
            case VK_RIGHT:
               return(811);
            case VK_HOME:
               return(812);
            case VK_PRIOR:
               return(814);
            case VK_NEXT:
               return(815);
            case VK_END:
               if ((inputBuffer.Event.KeyEvent.dwControlKeyState & RIGHT_CTRL_PRESSED) ||
                  (inputBuffer.Event.KeyEvent.dwControlKeyState & LEFT_CTRL_PRESSED))
                  return(817);
               return(816);
            case VK_INSERT:
               return(818);
            case VK_DELETE:
               return(819);
            case VK_F11:
               return(820);
            case VK_F12:
               return(822);
            case VK_F1:
               if (inputBuffer.Event.KeyEvent.dwControlKeyState & (RIGHT_CTRL_PRESSED || LEFT_CTRL_PRESSED))
                  return(833);
               return(823);
            case VK_F2:
               if (inputBuffer.Event.KeyEvent.dwControlKeyState & (RIGHT_CTRL_PRESSED || LEFT_CTRL_PRESSED))
                  return(834);
               return(824);
            case VK_F3:
               if (inputBuffer.Event.KeyEvent.dwControlKeyState & (RIGHT_CTRL_PRESSED || LEFT_CTRL_PRESSED))
                  return(835);
               return(825);
            case VK_F4:
               if (inputBuffer.Event.KeyEvent.dwControlKeyState & (RIGHT_CTRL_PRESSED || LEFT_CTRL_PRESSED))
                  return(836);
               return(826);
            case VK_F5:
               if (inputBuffer.Event.KeyEvent.dwControlKeyState & (RIGHT_CTRL_PRESSED || LEFT_CTRL_PRESSED))
                  return(837);
               return(827);
            case VK_F6:
               if (inputBuffer.Event.KeyEvent.dwControlKeyState & (RIGHT_CTRL_PRESSED || LEFT_CTRL_PRESSED))
                  return(838);
               return(828);
            case VK_F7:
               if (inputBuffer.Event.KeyEvent.dwControlKeyState & (RIGHT_CTRL_PRESSED || LEFT_CTRL_PRESSED))
                  return(839);
               return(829);
            case VK_F8:
               if (inputBuffer.Event.KeyEvent.dwControlKeyState & (RIGHT_CTRL_PRESSED || LEFT_CTRL_PRESSED))
                  return(840);
               return(830);
            case VK_F9:
               if (inputBuffer.Event.KeyEvent.dwControlKeyState & (RIGHT_CTRL_PRESSED || LEFT_CTRL_PRESSED))
                  return(841);
               return(831);
            case VK_F10:
               if (inputBuffer.Event.KeyEvent.dwControlKeyState & (RIGHT_CTRL_PRESSED || LEFT_CTRL_PRESSED))
                  return(842);
               return(832);
            }
       }
       break;
       case MOUSE_EVENT:
          if (inputBuffer.Event.MouseEvent.dwEventFlags != MOUSE_MOVED &&
             inputBuffer.Event.MouseEvent.dwButtonState)
          {
             i = inputBuffer.Event.MouseEvent.dwMousePosition.X+1;
             i += (inputBuffer.Event.MouseEvent.dwMousePosition.Y*100 + 100);
             if ((_ID_Cursor = esta_en_el_raton(i,NULL)) != -1)
             {
                _M_Cursor = i;
                return(SOLICITA_POSICION);
             }
          }
          break;
       default:
          primero_segundo = inputBuffer.Event.KeyEvent.wVirtualKeyCode;
          return(0);
          break;
                         }
                                                } while(-1);
                                                return(-1);
}
#endif

#else
#ifdef WATCOMNLM
int pulsatecla_(espera)
int espera;
{  
   
   if (espera) return(getch());
   
   if (kbhit() != FALSE)
   {
      return(getch());
   }
   if (timer_especial_int)
      (*timer_especial_int)();
   return(-1);
}
int pulsatecla()
{
   return(pulsatecla_(0));
}

#endif
#endif
#endif
#endif

int mouse_conversion(x,y,modo)
int x,y,modo;
{
   
   return(y/8*100+x/8+101);
}

#ifdef FGV32
#ifndef FGVWIN

static unsigned long rw_other_time()
{
			unsigned short l1,l2;
         unsigned char l0;
         unsigned long a;
         if (fIsTNT)
         {
            _asm
            {
               pushad
                  mov ah,0h
                  int 1ah
                  mov l0,al
                  mov l1,cx
                  mov l2,dx
                  popad
            }
            a =  l1;
            a <<= 16;
            a |= l2;
            a /= 18;
            if (l0)
            {
               a |= 0x80000000L;
            }
            return(a);
         }
         else
         {		 
            return(time(NULL));
         }
}
#endif
#endif

/*
*     lee un caracter desde la consola sin pintarlo ni chequear CTRL-C
*/
/*static */ int leech()
{
			int c,pp;
         long delay = 0L;
         long a;
#ifdef ESDOS
#ifdef FGVWIN
         HCURSOR oldCurs;
         
         if (NO_PINTAR_AHORA) {NO_PINTAR_AHORA = 0;v_refresca();}
#endif
#endif
         if (general_int) (*general_int)();
         
         if (modo_desatendido)
         {
            return(modo_desatendido);
         }
         
#ifdef ESDOS
         
inicial:
         if (time_tecla_timeout) {
#if defined(FGV32) && !defined(FGVWIN)
            a = rw_other_time();
#else
            a = time(NULL);
#endif
            delay = a + time_tecla_timeout;
         }
         
         if (hay_raton)
         {
            /* mostrarlo o cambiarle la forma */
#ifdef FGVWIN
            /* oldCurs = SetCursor(LoadCursor(NULL,IDC_IBEAM));*/
#else
#ifndef FGV32
            inregs.x.ax = 0x01;
            int86(0x33,&inregs,&inregs);
#endif
#endif
         }
         
         do {		
            if (general_int)
               (*general_int)();
            if (time_tecla_timeout)
            {
#if defined(FGV32) && !defined(FGVWIN)
               a = rw_other_time();
#else
               a = time(NULL);
#endif
               
               if (a >= delay) {
                  c = -1;
                  break;
               }
            }
            /** call **/
#ifndef FGVWIN
            if (timer_especial_int)
               (*timer_especial_int)();
#endif
            
            /****/		
            
            if (buf_tecla[0] != -1) {c=-2;break;}		
            
#ifndef FGVWIN
#ifndef FGV32
            if (hay_raton)
            {
               /* despues de los ptec viene el raton */
               inregs.x.ax = 0x05;
               inregs.x.bx = 0x0;
               int86(0x33,&inregs,&inregs);
               if (inregs.x.bx)
               {
                  if (_ID_Cursor == -1)
                  {
                     pp = mouse_conversion(inregs.x.cx,inregs.x.dx,0);
                     if ((_ID_Cursor = esta_en_el_raton(pp,NULL)) != -1)
                     {											      
                        _M_Cursor = pp;
                        c = SOLICITA_POSICION;
                        break;
                     }
                  }
               }
               else
               {
                  inregs.x.ax = 0x05;
                  inregs.x.bx = 0x1;
                  int86(0x33,&inregs,&inregs);
                  if (inregs.x.bx)
                  {					  
                     buf_tecla[0] = 806;
                     buf_tecla[1] = -1;
                     c=-2;
                     break;
                  }
               }
            }       /**************************************/
#endif
#endif
#ifdef FGVWIN	
         } while((c = win_leech(time_tecla_timeout)) == EOF);
#else
#ifdef FGV32
#ifdef WATCOMNLM
         if (0)
#else
            if (time_tecla_timeout /* || general_int */ || timer_especial_int)
#endif
               c = pulsatecla_(0);
            else
               c = pulsatecla_(1);
            if (c != -1) break;
            if (time_tecla_timeout /* || general_int */ || timer_especial_int)
            {
#ifndef FGVWIN
               if (!fIsTNT)
#endif
                  Sleep(2); /* algo de vidilla al sistema */
            }
        } while(-1);
#else
        } while((c = pulsatecla_(1)) == EOF);
#endif
#endif
        
        if (hay_raton)
        {
#ifdef FGVWIN
           /* SetCursor(oldCurs);*/
#else
           /* acultarlo o cambiarle la forma */
#ifndef FGV32
           inregs.x.ax = 0x02;
           int86(0x33,&inregs,&inregs);
#endif
#endif
        }
        
        if (time_tecla_timeout) {
           if (duracion_tecla_timeout) {
              duracion_tecla_timeout--;
              if (!duracion_tecla_timeout) {
                 time_tecla_timeout = 0;
              }
           }
        }
        if (general_int) (*general_int)();
        return (c);
#else
        if (time_tecla_timeout) {
           /* esto previene el timeout por si hay alguna alarma a programar */
           /* si la alarma es antes del timeout el timeout no se programa */
           a = time(NULL);
           if (!_programada_alarma || (a+time_tecla_timeout) < (_programada_alarma-2)) {
              alarm(0);
              signal(SIGALRM,_time_out);
              alarm(time_tecla_timeout);
              delay = 1L;
           }
        }
        
        
        do {
           if (errno == EINTR)
              errno = 0;				
           if (!isatty(fileno(stdin)))
           { 
              /* procurar salir lo mas gentilmente posible! */
              c = 807;
           }
           else
              c=getchar();
           if (errno == EINTR && (delay != 0L || buf_tecla[0] != -1)) {
              c = -2;
              break;
           }
        } while(c == EOF && errno == EINTR);
        if (c < 0) {
           if (c == EOF)
              c = 0;
           else
           {
              if (buf_tecla[0] != -1)
                 c = -2;
              else
                 c = -1;
           }
        }
        
        if (time_tecla_timeout && _programada_alarma) {
           alarm(0);
           a = time(NULL);
           signal(SIGALRM,alarma_sig);
           alarm((unsigned int)(_programada_alarma-a));
        }
        
        if (time_tecla_timeout) {
           if (duracion_tecla_timeout) {
              duracion_tecla_timeout--;
              if (!duracion_tecla_timeout) {
                 time_tecla_timeout = 0;
              }
           }
        }
        if (general_int) (*general_int)();
        return(c);
#endif
}

/* mete una pulsacion en el buffer de teclado */

int ptec(t)
int t;
{
   int n;
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      return(ptec_en_terminal(t));
   }
#endif
#endif
   
   for (n = 0;n < 200 && buf_tecla[n] != -1;n++);
   if (n == 200) { /* reset por overflow */
      for (n = 0;n < 200;n++)
         buf_tecla[n] = -1;
      return(1);
   }
   if (n < 199)
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

/*****************************/
int control_de_usuarios()
{
   char **_dir;
   char **__dir;
   char *p;
   FILE *f;
   char _buf_[20000];
   char eldir[256];
   int i,j;
   int pid;
   int puede = 0;
   static int t_no_recursion_ = 0;
   
   if (t_no_recursion_) return 0;
   
   t_no_recursion_ = 1;

   i = 0;
   if (!R_midir[0])
   {
 	  strcpy(R_midir,rw_directorio("dev/reg/"));
   }

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


   _buf_[i] = 0;
   if (DespachadorExterno)
   { 
       (*DespachadorExterno)((int)DESPACHO_USUARIOS,0,(void *)_buf_);
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

   }
   
   t_no_recursion_ = 0;
   return 0;
}
/*****************************/

/*
* leecar  lee caracter a caracter devolviendo un solo caracter o una secuencia
*         de control.
*/
#ifdef RWXARXA
extern int leecontrol_fistro();
#endif
#ifdef RWXARXA
extern int envia_subejecuta_a_server();
#endif
int leecar()
{
   int i=0,j,d,c,n,k,recur = 0,rkk;
   short p;
   int (*f_f)();
   char *par_f;
   static long uposrep;
   static int nrep = 0;
   int ayuda_teclas();
   char tmp[10];
   int ocursor;
#ifdef RWXARXA
   char fistro_tmp[512];
#endif
   
   if (TestigoRaton)
			{
      TestigoRaton = 0;
      return(SOLICITA_POSICION);
			}
   
   if (DESPROTEGIDO == 10) {
      if (( j = rw_read_(fich_tecla,(char *)&p,sizeof(short)) ) != 0 && j != -1) {
         d = p;
         if (d == (EXTRATECLA + 18)) {
            rw_read_(fich_tecla,(char *)&p,sizeof(short));
            nrep = p;
            uposrep = rw_lseek_(fich_tecla,0L,SEEK_CUR);
            rw_read_(fich_tecla,(char *)&p,sizeof(short));
            d = p;
         }
         if (d == (EXTRATECLA + 19)) {
            if (nrep) {
               nrep--;
               rw_lseek_(fich_tecla,uposrep,SEEK_SET);
            }
            rw_read_(fich_tecla,(char *)&p,sizeof(short));
            d = p;
         }       
         return(d);
      }
			}
   
inicio:
         
#ifdef RWXARXA
#ifdef RWSERVER
         if (en_conexion_externa && leecar_server_puro)
         {
            if (leecar_server_puro > 1) 
            {
               leecar_server_puro++;
               return(0);
            }
            leecar_server_puro++;
         }
#endif
#endif
         
         
         if (buf_tecla[0] != -1) {
            d = buf_tecla[0];
            for (c = 0;c < 199 && buf_tecla[c] != -1;c++)
               buf_tecla[c] = buf_tecla[c+1];
            buf_tecla[c] = -1;
         }
         else {
            i = 0;
            if (sitecla_)
               for (j=0;j<NUMTECLA;j++)
                  sitecla_[j] = 1;
               
#ifdef RWXARXA
#ifdef RWSERVER
               if (en_conexion_externa)
               {
                  c = leecar_en_terminal();
                  d = c;
               }
               else
#endif
#endif
                  do {
#ifdef FGVWIN
                     en_lee_teclado = 1;
#endif
                     c =  leech();
#ifdef FGVWIN
                     en_lee_teclado = 0;
#endif
                     if (rwagi_infoteclado)
                     {
                        sprintf(tmp,"[%3d]",c);
                        ocursor = Pcursor;
                        pprint(2501,tmp);
                        cursor( (ocursor/80) * 100 + ocursor%80 + 101 );
                     }
                     
                     if (c <= SOLICITA_POSICION) {d = c;break;}				
                     if (c >= MID_XNOTECLAS) {d = c;break;} /* funcion especial */
                     
                     if (c == -2) goto inicio;
                     if (c == -1 && time_tecla_timeout) { /* normalmente timeout */
                        d = cuantas_tecla_timeout - 1;
                        while(d > 0) {
                           ptec(que_tecla_timeout);
                           d--;
                        }
                        d = que_tecla_timeout;
                        break;
                     }
                     if (!sitecla_)
                        d = c;
                     else
                        d = compacon(i,c,sitecla_);
                     i++;
                  } while (d == -1);
         }
         
         
         if (d == BUSCA_PTECS)
         {   
            goto inicio;
         }
         
#ifdef RWSERVER
         if (d == PIDO_TRASMISION || d == PIDO_USUARIOS)
         {   
            goto inicio;
         }
#else
         
         if (d == PIDO_TRASMISION)
		 {
#ifdef RWXARXA
			if (en_conexion_terminal)
			{
               envia_control_a_server("",0,61);
               modo_terminal();
               goto inicio;
			}
		    else
#endif
			{
			   goto inicio;
			}
         }         

         if (d == PIDO_USUARIOS)
		 {
#ifdef RWXARXA
			if (en_conexion_terminal)
			{
               envia_control_a_server("",0,63);
               modo_terminal();
               goto inicio;
			}
		    else
#endif
			{
			   control_de_usuarios();
			   goto inicio;
			}
         }

#endif
         
         
         if (d == TECLAOK)
         {
            d = modo_de_validacion;
         }
         
         if (d == REFRESCO || d == R_EFRESCO) 
         {
            v_refresca();
            goto inicio;
         }
         
         
#ifdef RWXARXA
         if (en_conexion_terminal)
         {
            if (d == 20) 
            { /* CONTROL-T ---> AYUDA DE TECLADO */
               llama_fun_int(ayuda_teclas,NULL);
               goto inicio;
            }
            if (d == 1)
            {
               w_graba(stdpan,"stdpan");
               goto inicio;
            }
            if (d >= EXTRATECLA && d < (EXTRATECLA+FUNCIONES) && 
               titu_tecla[d-EXTRATECLA+PRIMERAF].d && 
               strlen(titu_tecla[d-EXTRATECLA+PRIMERAF].d)) 
            {
               par_f = titu_tecla[d-EXTRATECLA+PRIMERAF].d;
               if (par_f[0] == '@') 
               { /* string */
                  i = strlen(par_f+1);
                  par_f += i;
                  while(i) 
                  {
                     c = *par_f;
                     if (c == '&') 
                     {
                        i--;
                        par_f--;
                        if (i && *par_f != '&') 
                        {
                           c = 0;
                           k = 1;
                           while(k <= 100 && i) 
                           {
                              c += (*par_f - 48) * k;
                              par_f--;
                              i--;
                              k *= 10;
                           }
                        }
                        i++;
                        par_f++;
                     }
                     if (ptec(c))
                        break;
                     par_f--;
                     i--;
                  }
                  goto inicio;
               }
               else if (par_f[0] == '#') 
               {
                     quitab(par_f+1);
                     if (strlen(par_f+1))
                        w_graba(stdpan,par_f+1);
                     goto inicio;
               }
               /* envia_subejecuta_a_server(par_f); */
               llama_fun_int(envia_subejecuta_a_server,par_f);
               goto inicio;
            }
            
            if (leecar_terminal_puro)
            {
               return(d);
            }
            else
            {
               if (d == AYUDA || d == _AYUDA || (d >= EXTRATECLA && d < (EXTRATECLA+FUNCIONES)))
               {
                  memcpy(fistro_tmp,&d,sizeof(int));
                  memcpy(fistro_tmp+sizeof(int),&_M_Cursor,sizeof(int));
                  memcpy(fistro_tmp+(sizeof(int)*2),&_ID_Cursor,sizeof(int));
                  memcpy(fistro_tmp+(sizeof(int)*3),&_M_EXTRA_AVANCE,sizeof(int));
                  memcpy(fistro_tmp+(sizeof(int)*4),&redita_col,sizeof(int));
                  if (dato_entra)
                  {
                     fistro_tmp[sizeof(int)*5] = 1;
                     memcpy(fistro_tmp+1+sizeof(int)*5,&dato_entra_len,sizeof(int));
                     strcpy(fistro_tmp+1+sizeof(int)*6,dato_entra);			  
                  }
                  else
                  {
                     fistro_tmp[sizeof(int)*5] = 0;			  
                  }		   
                  /* recibir los ptec */
                  llama_fun_int(leecontrol_fistro,fistro_tmp);           		   
                  memcpy(&redita_col,fistro_tmp,sizeof(int));
                  rkk = 0;
                  if (fistro_tmp[sizeof(int)] < 3)
                  {
                     memcpy(&d,fistro_tmp+1+sizeof(int),sizeof(int));
                     rkk = 1;
                  }
                  if (dato_entra)
                  {
                     strcpy(dato_entra,fistro_tmp+sizeof(int)*2+1);
                  }
                  if (!rkk)
                     goto inicio;
                  return(d);
               }
            }
      }
#endif
      
      
      if (!f_noallowed && d < PFUNCIONENTRA) {
         f_f = NULL;
         if (d == 20)
         { /* CONTROL-T ---> AYUDA DE TECLADO */
            f_f = ayuda_teclas;
            par_f = NULL;
         }
         else
            if (d == 1)
            {
               w_graba(stdpan,"stdpan");
               goto inicio;
            }
            else
               if ((d == AYUDA || d == _AYUDA) && f_ayuda) 
               {
                  f_f = f_ayuda;
                  par_f = clave_help;
               }
               else
                  if (d >= EXTRATECLA && d < (EXTRATECLA+FUNCIONES) && 
                     titu_tecla[d-EXTRATECLA+PRIMERAF].d && strlen(titu_tecla[d-EXTRATECLA+PRIMERAF].d)) {
                     par_f = titu_tecla[d-EXTRATECLA+PRIMERAF].d;
                     if (par_f[0] == '@') { /* string */
                        i = strlen(par_f+1);
                        par_f += i;
                        while(i) {
                           c = *par_f;
                           if (c == '&') {
                              i--;
                              par_f--;
                              if (i && *par_f != '&') {
                                 c = 0;
                                 k = 1;
                                 while(k <= 100 && i) {
                                    c += (*par_f - 48) * k;
                                    par_f--;
                                    i--;
                                    k *= 10;
                                 }
                              }
                              i++;
                              par_f++;
                           }
                           if (ptec(c))
                              break;
                           par_f--;
                           i--;
                        }
                        goto inicio;
                     }
                     else
                        if (par_f[0] == '#') {
                           quitab(par_f+1);
                           if (strlen(par_f+1))
                              w_graba(stdpan,par_f+1);
                           goto inicio;
                        }
                        if (f_util) {
                           f_f = f_util;
                        }
                  }
                  
                  if (f_f) { /* Se llama a la funcion on-line salvando las definiciones actuales */
                     /* de atributo,posicion y forma del cursor,graficos*/
                     if (!(i = llama_fun_int(f_f,par_f)))
                        goto inicio;
                     if (i > 899) /* controles especiales */
                        d = i;
                  }
      }
      
      if (d == (EXTRATECLA + 17)) {
         /*      pito(); */
         tracer_int = 1; /* tecla de interrupcion pulsada */
         /*      goto inicio; */
      }
      
      if (d == (EXTRATECLA + 18)) {
         /*      pito(); */
         logger_int = 1; /* tecla de interrupcion pulsada */
         /*      goto inicio; */
      }
      
      if (d == (EXTRATECLA + 19)) {
         /*      pito(); */
         logger_int = 2; /* tecla de interrupcion pulsada */
         /*      goto inicio; */
      }
      
      
      if (DESPROTEGIDO == 20) {
         p = d;
         rw_write_(fich_tecla,(char *)&p,sizeof(short));
         if (d == (EXTRATECLA + 18)) {
            printf("\007Veces:");
            scanf("%d",(char *)&d);
            p = d;
            rw_write_(fich_tecla,(char *)&p,sizeof(short));
            goto inicio;
         }
         if (d == (EXTRATECLA + 19) )
            goto inicio;
      }
      
      
      if ((d >= DISPARO_CONTROL && d < MAX_DISPARO_CONTROL) || (d >= PREDISPARO_CONTROL && d < PREMAX_DISPARO_CONTROL))
      {
#ifdef RWXARXA
         if (en_conexion_terminal)
         {
            extern void dispara_servidor();
            llama_fun_int(dispara_servidor,d);
            goto inicio;
         }
#endif
         if (f_disparo)
         {	  
            llama_fun_int(f_disparo,d);
         }
         goto inicio;
      }
      
      return (d);
}


#ifndef FGVWIN
/*
*  curon    activa el cursor en el modo deseado
*           0-desactivado   1-activado    2-insercion
*/

int curon(i)
int i;
{
#ifndef FGVWIN
#ifdef FGV32
#ifndef WATCOMNLM
   CONSOLE_CURSOR_INFO cci;
   COORD  coordCursor;
#endif
#endif
#endif
   unsigned char a,b;
   static int cur = 1;
   
   if (i == -1)
      return(cur);
   
   if (i==0) {
      a=1;
      b=0;
      cur = 0;
   }
   else if (i==1) {
      a=6;
      b=7;
      cur = 1;
   }
   else if (i==2) {
      a=2;
      b=6;
      cur = 2;
   }
   else
      return(-1);
   
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      curon_en_terminal(i);
   }
#endif
#endif
   
#ifdef ESDOS
#ifndef FGVWIN
#ifndef FGV32
   inregs.h.ah = 0x01;     /* cambios para MS DOS */
   inregs.h.ch = a;
   inregs.h.cl = b;
   int86(0x10,&inregs,&outregs);
#else
#ifndef WATCOMNLM
   if (myconsola != INVALID_HANDLE_VALUE)
   {
      GetConsoleCursorInfo(myconsola,&cci);
      if (cur)
      {
         cci.bVisible = TRUE;
         cci.dwSize = b;
      }
      else
      {
         cci.bVisible = FALSE;
      }
      SetConsoleCursorInfo(myconsola,&cci);
      if (cur)
      {
         coordCursor.X = Pcursor%80;
         coordCursor.Y = Pcursor/80;
         SetConsoleCursorPosition(myconsola,coordCursor);
      }
   }
#else
   if (cur)
   {
      DisplayInputCursor();
      /*GetCursorShape(&a,&b);*/
      SetCursorShape(1,b);
   }
   else
      HideInputCursor();
#endif
#endif
#endif
   
#endif
   return(0);
}

#endif

int es_atri(a)
char a;
{
   if (a == 'I' || a == 'R' || a == 'P' || a == 'S')
      return(1);
   return(0);
}

/*
* w_refresca
* pinta la pantalla
*/
void w_refresca(w)
VENT *w;
{
			int occ = _PCcolor;
         int off = _PFcolor;
         char _att = Patrib;
#ifdef ESDOS
#ifdef FGV_MAXDOS
#ifdef FGVWIN
         POINT p;
         int alto  = (w->vfin)/100 - (w->vini)/100 + 1;
         int ancho = (w->vfin)%100 - (w->vini)%100 + 1;
         HDC hdc;
         if (w != stdpan)
         {
            if (alto == 25 && ancho == 80)
            {
               GetCursorPos(&p);
               p.x++;
               SetCursorPos(p.x,p.y);
            }
            w_win_texto(stdpan,w->vini,w->vfin,w);
         }
         
         if (!IsIconic(principal_w) && !NO_PINTAR_AHORA)
         {
            if (!no_bitblt)
               hdc = GetDC(principal_w);
            if (w != stdpan)
            {
               if (!no_bitblt)
                  BitBlt(hdc,principal_rect.left+((w->vini)%100-1)*scaracter.cx,
                  principal_rect.top+espacio_arriba+((w->vini)/100-1)*scaracter.cy,
                  ancho*scaracter.cx,alto*scaracter.cy,stdpan->vhdc,(w->vini%100-stdpan->vini%100)*scaracter.cx,
                  espacio_arriba+(w->vini/100-stdpan->vini/100)*scaracter.cy,SRCCOPY);
            }
            else
            {
               if (!no_bitblt)
                  BitBlt(hdc,principal_rect.left,principal_rect.top,principal_nx,
                  principal_ny,stdpan->vhdc,0,0,SRCCOPY);
            }
            if (!no_bitblt)
               ReleaseDC(principal_w,hdc);
         }
#else
         register unsigned char *s=w->vv;
         int i,j;
         int curs;
         int alto  = (w->vfin)/100 - (w->vini)/100 + 1;
         int ancho = (w->vfin)%100 - (w->vini)%100 + 1;
         int fini = (w->vini)/100;
         int cini = (w->vini)%100;
         register int k,kk,kkk;
         int r,h;
         int b=alto*ancho;
         unsigned char c,a,g,Atr=' ',Grf=' ',Col=128;
         int _occ = PCcolor;
         int _off = PFcolor;
         int _ocu = Pcursor/80*100 + Pcursor%80 + 101;
#ifndef FGV32
         register unsigned char *enpanta;
#else
#ifndef WATCOMNLM
         COORD backgLoc;
         DWORD dwCharsWritten;
#endif
         char cc;
         char buff[81];
         int nb;
#endif
         
         if (general_int) (*general_int)();
         
#ifndef FGVWIN
         if (timer_especial_int)
            (*timer_especial_int)();
#endif
         
         if (atento_imp) return;
         
         
#ifdef RWXARXA
#ifdef RWSERVER
         if (en_conexion_externa)
         {
            refresca_en_terminal(w);
            return;
         }
#endif
#endif
         
#ifndef FGVWIN
#ifdef FGV32
#ifndef WATCOMNLM
         if (myconsola == INVALID_HANDLE_VALUE)
         {
            return;
         }
#endif
#endif
#endif
         
         _PCcolor = -1;
         _PFcolor = -1;
         
         /* este parametro a 0 indica a las funciones de cursor,  */
         /* atributos y print que no deben alterar la stdpan      */
         if ((Patrib & (~128)) != ' ')
            atributo(tolower((Patrib & (~128))));
         atributo(' ');
         curs=dos_atri();
#ifdef FGV32
         nb = 0;
#endif
         
         
         Prefresco = FALSO; /* modifica stdpan desactivado */
         /* Patrib no sufre alteracion por Prefresco !! */
         if (Pgrafi != ' ')
            Goff();
         
         k = 0; /* i*ancho+j; /* desplazamiento corriente */
         kk = b;
         kkk = 2*b;
         for (i=0;i<alto;i++) {
            r = (fini-1+i) * 80 + cini - 1;
            if ( (fini+i) >= 25 && !linea_25) continue;
#ifndef WATCOMNLM
#ifdef FGV32
            backgLoc.X = cini-1;
            backgLoc.Y = fini+i-1;
#else
            
            enpanta = direc_ppanta + r*2;
            
#endif
#else
            cursor((fini+i)*100+cini);
#endif
            for (j=0;j<ancho;j++,r++,k++,kk++,kkk++) {
													  g = *(s+kk); /* Atencion COLOR */
                                         a = *(s+kkk) & (~128);
                                         if (a != Atr || g != Col) {
#ifdef FGV32
#ifdef WATCOMNLM
                                            if (nb)
                                            {
                                               buff[nb] = 0;
                                               /* cputs(buff);*/
                                               _textattr(curs,buff,nb);
                                               Pcursor += nb;
                                               nb = 0;
                                               
                                            }
#else
                                            if (nb)
                                            {
                                               WriteConsoleOutputCharacter(myconsola,buff,nb,backgLoc,&dwCharsWritten);
                                               FillConsoleOutputAttribute(myconsola,curs,nb,backgLoc,&dwCharsWritten);
                                               backgLoc.X+=nb;
                                               nb = 0;
                                            }
#endif
#endif
                                            
                                            if (g != 128) {
                                               _PFcolor = ((g & 0x70) >> 4);
                                               _PCcolor = (g & 0x0f);
                                            }
                                            
                                            if (g != Col) {
                                               if (!es_atri(a) && Atr == ' ')
                                               {
                                                  Nor();
                                                  curs=dos_atri();
                                               }
                                               Col = g;
                                            }
                                            if (es_atri(a) || Atr != ' ') {
                                               if (!es_atri(a))
                                                  a = ' ';
                                               
                                               if (Atr != ' ') {
                                                  if (Atr != a) {
                                                     atributo(tolower(Atr)); /*desactivar el anterior atributo */
                                                     atributo(a); /* pintar el atributo */
                                                     curs=dos_atri();
                                                  }
                                                  else
                                                  {
                                                     pon_color_a(a);
                                                     curs=dos_atri();
                                                  }
                                               }
                                               else {
                                                  atributo(a); /* pintar el atributo */
                                                  curs=dos_atri();
                                               }
                                               Atr = a;
                                            }
                                         }
                                         
                                         if (r != (24 * 80 - 1) && r != (25 * 80 - 1)) {    /* el ultimo caracter 2480 y 2580 no se pinta */
                                            g = *(s+k);
                                            if (g > 127 && (h = C_conversion[g-128])) {
#ifdef FGV32
                                               buff[nb] = control[h][0];
                                               nb++;
#else
                                               *enpanta = control[h][0];
                                               enpanta++;
                                               *enpanta = curs;
                                               enpanta++;
#endif
                                            }
                                            else {
#ifdef FGV32
                                               buff[nb] = g;
                                               nb++;
#else
                                               *enpanta = g;
                                               enpanta++;
                                               *enpanta = curs;
                                               enpanta++;
                                               
#endif
                                            }
                                         }
            }
#ifdef FGV32
#ifdef WATCOMNLM
            if (nb)
            {
               buff[nb] = 0;
               /* cputs(buff);*/
               _textattr(curs,buff,nb);
               Pcursor += nb;
               nb = 0;
            }
#else
            if (nb)
            {
               WriteConsoleOutputCharacter(myconsola,buff,nb,backgLoc,&dwCharsWritten);
               FillConsoleOutputAttribute(myconsola,curs,nb,backgLoc,&dwCharsWritten);
               backgLoc.X+=nb;
               nb = 0;
            }
#endif
#endif
                  }
                  
                  
                  if (Grf != ' ')
                     Goff();
                  if (Atr != ' ') {
                     atributo(tolower(Atr)); /*resetear atributos */
                     Nor();
                  }
                  _PCcolor = _occ;
                  _PFcolor = _off;
                  Patrib  = _att;
                  Prefresco = CIERTO; /* modifica stdpan activado */
                  atributo(Patrib);
                  cursor(_ocu);
                  fflush(stdout);
                  
#endif
#else
                  
                  char *s=w->vv;
                  register int i,j;
                  int alto  = (w->vfin)/100 - (w->vini)/100 + 1;
                  int ancho = (w->vfin)%100 - (w->vini)%100 + 1;
                  int fini = (w->vini)/100;
                  int cini = (w->vini)%100;
                  int k;
                  int r;
                  int b=alto*ancho;
                  char c,a,g,Atr=' ',Grf=' ';
                  extern int DOSVENT(); /* cambios para MS DOS */
                  
                  _PCcolor = -1;
                  _PFcolor = -1;
                  
                  
                  if (general_int) (*general_int)();
#ifndef FGVWIN
                  if (timer_especial_int)
                     (*timer_especial_int)();
#endif
                  
                  Prefresco = FALSO; /* modifica stdpan desactivado */
                  /* este parametro a 0 indica a las funciones de cursor,  */
                  /* atributos y print que no deben alterar la stdpan      */
                  
                  /* atencion al paso de control[] -> no es un arrai de punteros sino
                  un arrai de char de dos dimensiones !!!! */
                  
                  DOSVENT(fini,cini,alto,ancho,s,control+ALTAINTEN,sizeof(char *),control);  /* cambios para MS DOS */
                  
                  Prefresco = CIERTO; /* modifica stdpan activado */
                  fflush(stdout);
#endif
#else
                  unsigned char *s=w->vv;
                  register int i,j;
                  int curs;
                  int alto  = (w->vfin)/100 - (w->vini)/100 + 1;
                  int ancho = (w->vfin)%100 - (w->vini)%100 + 1;
                  int fini = (w->vini)/100;
                  int cini = (w->vini)%100;
                  int k,kk,kkk;
                  int r,h;
                  int b=alto*ancho;
                  unsigned char c,a,g,Atr=' ',Grf=' ',Col=128;
                  int _occ = PCcolor;
                  int _off = PFcolor;
                  int _ocu = Pcursor/80*100 + Pcursor%80 + 101;
                  
                  if (general_int) (*general_int)();
                  
                  if (atento_imp) return;
                  
#ifdef RWXARXA
#ifdef RWSERVER
                  if (en_conexion_externa)
                  {
                     refresca_en_terminal(w);
                     return;
                  }
#endif
#endif
                  
                  
                  
                  _PCcolor = -1;
                  _PFcolor = -1;
                  
                  
                  /* este parametro a 0 indica a las funciones de cursor,  */
                  /* atributos y print que no deben alterar la stdpan      */
                  if (Patrib != ' ')
                     atributo(tolower(Patrib));
                  atributo(' ');
                  Prefresco = FALSO; /* modifica stdpan desactivado */
                  /* Patrib no sufre alteracion por Prefresco !! */
                  if (Pgrafi != ' ')
                     Goff();
                  k = 0;
                  kk = b;
                  kkk = 2*b;
                  for (i=0;i<alto;i++) {
                     r = (fini-1+i) * 80 + cini - 1;
                     curs = (fini+i)*100+cini;
                     if ( (curs/100) >= 25 && !linea_25) continue;
                     cursor(curs);
                     for (j=0;j<ancho;j++,r++,k++,kk++,kkk++) {
                        g = *(s+kk); /* Atencion COLOR */
                        a = *(s+kkk) & (~128);
                        if (a != Atr || g != Col) {
                           if (g != 128) {
                              _PFcolor = ((g & 0x70) >> 4);
                              _PCcolor = (g & 0x0f);
                           }
                           if (g != Col) {
                              if (!es_atri(a) && Atr == ' ')
                                 Nor();
                              Col = g;
                           }
                           if (es_atri(a) || Atr != ' ') {
                              if (!es_atri(a))
                                 a = ' ';
                              if (Atr != ' ') {
                                 if (Atr != a) {
                                    atributo(tolower(Atr)); /*desactivar el anterior atributo */
                                    atributo(a); /* pintar el atributo */
                                 }
                                 else
                                    pon_color_a(a);
                              }
                              else {
                                 atributo(a); /* pintar el atributo */
                              }
                              Atr = a;
                           }
                        }
                        
                        if (r != (24 * 80 - 1) && r != (25 * 80 - 1)) {    /* el ultimo caracter 2480 y 2580 no se pinta */
                           /*       if (atento_imp) i_acabo(_imp); */
                           g = *(s+k);
                           if (g > 127 && (h = C_conversion[g-128])) {
                              if (h >= GRVE && h <= GRC8) {/* es semi GRAFICO */
                                 if (Grf == ' ') {
                                    Gon();
                                    Grf = 'G';
                                 }
                              }
                              else {
                                 if (Grf != ' ') {
                                    Goff();
                                    Grf = ' ';
                                 }
                              }
                              pinta(control[h],NUMCARCO,0);
                           }
                           else {
                              if (Grf != ' ') {
                                 Goff();
                                 Grf = ' ';
                              }
                              putchar(g);
                           }
                           /*       if (atento_imp) i_inicio(_imp); */
                        }
                     }
                  }
                  if (Grf != ' ')
                     Goff();
                  if (Atr != ' ') {
                     atributo(tolower(Atr)); /*resetear atributos */
                     Nor();
                  }
                  _PCcolor = _occ;
                  _PFcolor = _off;
                  Patrib  = _att;
                  Prefresco = CIERTO; /* modifica stdpan activado */
                  atributo(Patrib);
                  cursor(_ocu);
                  fflush(stdout);
#endif
                  _PCcolor = occ;
                  _PFcolor = off;
}

/*
* w_pinta
* copia el contenido de la ventana sobre la stdpan y la refresca (pintandola)
*/
void w_pinta(w)
VENT *w;
{
   w_mete(stdpan,w);
   w_refresca(w);
}

/* scrola el contenido de la stdpan entra ini,fin n lineas */
void v_scroll(ini,fin,n)
int ini,fin,n;
{
   VENT *w;
   int wini;
   int wfin;
   int nl = n % 100;
   int nc = n / 100;
#ifdef FGVWIN
   RECT Rect,ClipRect;
   int xn,yn;
   HDC hdc;
#endif
   
   if (!n) return;   


   if (nl)
   {
       wini = ((nl < 0) ? ini - nl*100 : ini);
       wfin = ((nl < 0) ? fin : fin - nl*100);
   }
   else
   {
       wini = ((nc < 0) ? ini - nc : ini);
       wfin = ((nc < 0) ? fin : fin - nc);
   }
       

#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      vscroll_en_terminal(ini,fin,n);
   }
   en_conexion_externa_stop++;
#endif
#endif
   
#ifdef FGVWIN
   Rect.top = espacio_arriba + ((ini/100)-1) * scaracter.cy;
   Rect.bottom = espacio_arriba + ((fin/100)) * scaracter.cy;
   Rect.left = ((ini%100)-1) * scaracter.cx;
   Rect.right = ((fin%100)) * scaracter.cx;
   ClipRect = Rect;
   /*
   ClipRect.top = espacio_arriba + ((ini/100)-1) * scaracter.cy;
   ClipRect.bottom = espacio_arriba + ((fin/100)) * scaracter.cy;
   ClipRect.left = ((ini%100)-1) * scaracter.cx;
   ClipRect.right = ((fin%100)) * scaracter.cx;
   */
   xn = nc*scaracter.cx;
   yn = nl*scaracter.cy;
   ScrollWindow(principal_w,xn,yn,&Rect,&ClipRect);
   ScrollDC(stdpan->vhdc,xn,yn,&Rect,&ClipRect,NULL,NULL);
     
   no_bitblt = 1;
   //BitBlt(w->vhdc,0,0,ancho*scaracter.cx,alto*scaracter.cy,wo->vhdc,cini*scaracter.cx,((wo == stdpan) ? espacio_arriba : 0) +fini*scaracter.cy,SRCCOPY);
#endif
   
   if (wini <= wfin) 
   {
      w = w_ini(wini,wfin);
      w_saca(w,stdpan);
      wini += ((nl*100)+nc);
      w_mueve(w,wini);
      w_pinta(w);
      w_fin(w);
      if (nl)
      {
          if (nl < 0) 
          {
             wini = (fin/100+nl+1)*100 + ini%100; /* ojo n es negativo */
             wfin = fin;
          }
          else 
          {
             wini = ini;
             wfin = (ini/100+nl-1)*100 + fin%100;
          }
          w = w_ini(wini,wfin);
          w_cls(w);
          w_pinta(w);
          w_fin(w);
      }
      if (nc)
      {
          if (nc < 0) 
          {
             wini = (ini/100)*100 + fin%100 + nc + 1; /* ojo n es negativo */
             wfin = fin;
          }
          else 
          {
             wini = ini;
             wfin = (fin/100)*100 + ini%100 + nc - 1;
          }
          w = w_ini(wini,wfin);
          w_cls(w);
          w_pinta(w);
          w_fin(w);
      }      
   }
   else 
   {
      wini = ini;
      wfin = fin;
       w = w_ini(wini,wfin);
       w_cls(w);
       w_pinta(w);
       w_fin(w);
   }
   
   
#ifdef FGVWIN
   SetRect(&Rect,scaracter.cx*((wini%100)-1),espacio_arriba+scaracter.cy*((wini/100)-1),scaracter.cx*((wfin%100)),espacio_arriba+scaracter.cy*((wfin/100)));
   FillRect(stdpan->vhdc,&Rect,agi_brush_gris);
   if (!NO_PINTAR_AHORA)
			{
      hdc = GetDC(principal_w);
      FillRect(hdc,&Rect,agi_brush_gris);
      ReleaseDC(principal_w,hdc);
			}
   no_bitblt = 0;
#endif
   
#ifdef RWXARXA
#ifdef RWSERVER
   en_conexion_externa_stop--;
#endif
#endif
}

/* pantalla lee y pinta pantallas
* pantalla("...../c/pepe.pan");
* devuelve -1 si no puede leer el fichero, 0 si va bien;
*/
int pantalla(p)
char *p;                   /* nombre del fichero de pantalla */
{
   VENT *w = w_ini(101,2380);
   if (!w) return(-1);
   w_lee(w,p);
   w_pinta(w);
   w_fin(w);
   return(0);
}

/* pprint pinta un string en la posicion indicada
*        si la posicion es 0 se pinta donde este el cursor
*        devuelve el numero de caracteres pintados
*/

int pprint(pos,str)
int pos;
char *str;
{
   if (pos != 0)
      cursor(pos);
   return(v_print(str));
}

int nume_code()
{
			return( (int)*((short *)(g_+2)) );
}

int pon_code(n)
short n;
{
			*((short *)(g_+2)) = n;
}

int nume_mochila()
{
   return((int)*((short *)(g_)));
}

int pon_mochila(n)
short n;
{
   *((short *)(g_)) = n;
}

#ifndef FGVWIN
fin_botones_()
{
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
						fin_botones__terminal();
   }
#endif
#endif
}
prepara_menusg_botones()
{
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
						prepara_menusg_botones_terminal();
   }
#endif
#endif
}

int recurre_boton(m)
int m;
{
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
						recurre_boton_terminal(m);
   }
#endif
#endif
   return(m);
}
#else
extern int *menusg_botones;
fin_botones_()
{
   int j;
   
   
   
   
   if (menusg_botones && menusg_botones != 0x00000001)
   { 
      for (j = 0;menusg_botones[j] != -1;j++)
      {					
         DestruyeBoton(menusg_botones[j]+1);
      }
      free(menusg_botones);
   }
   menusg_botones = NULL;
}
prepara_menusg_botones()
{
   if (!menusg_botones)
      menusg_botones = 0x00000001;
}
#endif

char *nombre_tecla(t)
int t;
{
   char *vacio = "";
   char *p;
   if (t < 801 || t > 842) return(vacio);
   t -= 801;
   p = titu_tecla[t].r;
   if (!p) {
      p = vacio;
   }
   return(p);
}


char *tecla_funcion(t,p,m)
int t;
char *p;
int m;
{
   
   if (t < 0 || t >= FUNCIONES) return(NULL);
   
   
   t += PRIMERAF;
   if (m == 1)
      p = titu_tecla[t].d;
   else {
      if (titu_tecla[t].d) {
									if (m == 2) return(p);
                           free(titu_tecla[t].d);
      }
      if (p) {
									titu_tecla[t].d = malloc(strlen(p)+1);
                           strcpy(titu_tecla[t].d,p);
      }
      else
									titu_tecla[t].d = p;
   }
   return(p);
}


void pon_tecla_timeout(a,b,c,d)
int a,b,c,d;
{
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      pon_tecla_timeout_terminal(a,b,c,d);
      return;
   }
#endif
#endif
   cuantas_tecla_timeout = b;
   que_tecla_timeout = a;
   time_tecla_timeout = c;
   duracion_tecla_timeout = d;
}

int no_pintar_ahora(int p)
{
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      funcion_windows_cliente(1011,&p,sizeof(int));
      return(p);
   }
#endif
   if (NO_PINTAR_AHORA && !p) {NO_PINTAR_AHORA = 0;v_refresca();}
   NO_PINTAR_AHORA = p;
   return(p);
}

