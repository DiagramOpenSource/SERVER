
#ifndef __YA_RWIXX__
#define __YA_RWIXX__

#ifndef CISAM_DAT
#define CISAM_DAT
#endif

#include "../dsdbfs/dsdbfs.h"

/* Nuevas estructuras DIAGRAM9 */

/* nuevos tipos standard */
#define AGIALFA     '0'
#define AGISHORT    '1'
#define AGILONG     '2'
#define AGIDOUBLE   '3'
#define AGIFLOAT    '4'
#define AGIFECHA    '5'

#include <fgvsttab.h>

#ifdef RWIXXMAIN

#ifdef FGVSCO
#ifdef FGVBSD
#error No puede definir FGVBSD y FGVSCO
#endif
#ifndef FGVUNIX
#define FGVUNIX
#endif
#endif


#ifdef FGVBSD
#ifndef FGVUNIX
#define FGVUNIX
#endif
#endif

#ifndef FGVUNIX
#ifdef FGVWIN
#ifndef FGVDOS
#define FGVDOS
#endif
#endif
#endif

#ifdef FGVDOS
#ifdef FGVUNIX
#error No se puede definir FGVUNIX(FGVSCO-FGVBSD) y FGVDOS(FGVWIN)
#endif
#endif

#ifdef FGVDOS

#ifndef DOSDIRECTO
#define DOSDIRECTO
#endif
#ifndef FGV_MAXDOS 
#define FGV_MAXDOS 
#endif
#ifndef AGI16
#ifndef FGV32
#define FGV32
#endif
#else
#error AGI16: el modelo de 16 bit ya no esta soportado en su conjunto
#endif



#ifndef BORLANDWORLD
#define asm _asm
#else
#define _asm asm
#endif

#ifdef FGV_MAXDOS
#ifndef WATCOMNLM
#include <windows.h>
#endif
#endif
#else

#ifndef FGVUNIX
#error Se debe definir al menos FGVDOS o FGVUNIX
#endif

#endif


#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifndef WATCOM
#ifndef _MSC_VER
#include <values.h>
#endif
#endif

#ifdef FGVDOS
#include <sys/locking.h>
#include <share.h>
#include <io.h>
#ifdef FGV32
#ifndef WATCOMNLM
#include <windows.h>
#endif
#endif
#ifdef FGV32
#ifndef WATCOMNLM
#define sleep(a) Sleep((long)(((long)a)*1000L));
#endif
#endif
#endif

#ifdef FGVBSD
#ifndef LARGESEEK
#define LARGESEEK
#endif
#endif

#include <errno.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#ifdef IXXTEST
#include <stdio.h>
#include <time.h>
#endif


#endif


#ifndef LK_UNLCK
#define LK_UNLCK 0
#endif

#ifndef LK_LOCK
#define LK_LOCK  1
#endif

#ifndef LK_NBLCK
#define LK_NBLCK 2
#endif



#ifndef EDEADLOCK
#define EDEADLOCK EACCES
#endif

#ifndef MAXDOUBLE
#define MAXDOUBLE   1.797693E+308
#endif
#ifndef MAXFLOAT
#define MAXFLOAT    3.37E+38F
#endif
#ifdef MINDOUBLE
#undef MINDOUBLE
#endif
#ifndef MINDOUBLE
#define MINDOUBLE   (-1.797693E+308)
#endif
#ifndef MINFLOAT
#define MINFLOAT    (-3.37E+38F)
#endif



#define FGV_ID_CIU     0x5DE861C7
#define FGV_ID_DIU     0x5DE861C8
#define FGV_ID_CIU_PAQ 0x5DE861C9
#define FGV_ID_CIU2_PAQ 0x5DE861CA

/* Definiciones para RW Index */

#ifdef ESMOTOROLA

#define IXXTIPO_PROCESADOR 'M'  /* Motorola PowerPC o similar */

#else

#define IXXTIPO_PROCESADOR 'I'  /* Intel x86 o similar */

#endif

/* limites basicos*/


#define IXXMAXFILES 500


#define IXXMAXLOCKS 100    /* aunque ponga bastantes no deberia ser practica habitual
el bloquear muchos registros a la vez de un mismo fichero
de todos modos este limite es solo a efectos de memoria
se puede ampliar o reducir como convenga */
/* SE AUTO INCREMENTA X IXXMAXLOCKS!!! */

#define IXXMAXLEVEL 200   /* un indexado de 200 niveles es imposible */
#define IXXMAXCONTENIDO 11 /* elementos para seleccion de "contenido" - 1 (busqueda por parte de la clave) */

/* posicones "estrategicas" */

#define OTOUCH      20L
#define OTAMREG     22L
#define OUPDATE     52L
#define OIDENTI     56L
#define OTRANSN     60L
#define OOPENID     1022L

/* areas de bloqueo siempre sobre fichero indice */

/* Max 32bits = 4294967295UL  usando este tope se duplica la capacidad pero
no estoy seguro de la portabilidad */
/* Max 32bits signed 7FFFFFFFL o 2147483647L */
/* bloqueo por opertura (modo exclusivo) de momento vamos a probar con SH_DENY */

#ifndef IXXFILEEND
#define IXXFILEEND  2147483645L
#endif

/* para bloquear el ultimo byte */
#define LCKFILEEND  2147483644L

/* 65536 maximos usuarios simultaneos */
#define LCKIZONAALL 65536L
/* 256 millones de registros maximos */
#define LCKDZONAALL 256000000L

/* zona control acceso al indice */
#define LCKIZONAINI (IXXFILEEND-LCKIZONAALL-1L)
/* zona bloqueos de datos */
#define LCKDZONAINI (LCKIZONAINI-LCKDZONAALL-1L)

/* bloqueo exclusivo "soft" */
#define LCKFIZONAALL 65536L
#define LCKFIZONAINI (LCKDZONAINI-LCKFIZONAALL-1L)

#define IXXBLOCKEND (LCKFIZONAINI-1L)

/* flag de bloqueo de escritura en indice */
#define LCKBEZONAINI OTOUCH
/* flag peticion de escritura */
#define LCKPEZONAINI 1021L
/* flag de reindexado */
#define LCKREZONAINI 1022L

/* estructuras */

typedef unsigned char Uchar;

struct ixx_tabla_de_filtro /* la tabla ya esta ordenada */
{
   Uchar *referencia;
   Uchar condicion; /* 0== 1=! 2=< 3=> 4=<= 5=>= +10 referencia apunta a filtro con resultado anterior +20 a rutina que devuelve la clave sino a clave*/
   Uchar modo; /* 0=Y 1=O 2=continua*/
   Uchar resultado; /* 0 false 1 true */
   struct ixx_tabla_de_filtro *siguiente;   
};

struct ixx_cabecera
{
   char file_id[6]; /*ixxfgv*/  /* IDENTIFICADOR */
   char file_ver[6]; /* 0002 */ /* VERSION */
   char nombre[8];
   unsigned char touch;  /* control de consistencia interna y de bloqueo de grabacion */
   unsigned char reserved;
   short tamreg;
   short tamblock;
   short nkeys;
   long nblocks;
   long nrecords;
   long nfreeblocks;
   long nfreerecords;
   /* las claves estan por orden en los bloques iniciales */
   long recordfreelist;
   long blockfreelist;
   unsigned long  updatecount;
   unsigned short autentificacion;
   unsigned short vacio;
   unsigned long numero_transaccion;
   unsigned long registros_reservados;
   
   /* OJO LA ESTRUCTURA NO PUEDE MEDIR MAS DE 1021 Bytes */
};
typedef struct ixx_cabecera IXXCABEZA;

struct ixx_campo
{
   short len;
   short pos_en_reg;
   char type;
   char modo;
   char name[25];
   char extra_type;
};
typedef struct ixx_campo IXXCAMPO;


struct ixx_clave
{
   short clave_len;
   short clave_num;
   short clave_tipo;
   short clave_ncampos;
   IXXCAMPO *clave_campos;
   struct ixx_tabla_de_filtro *filtro;
};
typedef struct ixx_clave IXXCLAVE;

struct ixx_fichero
{
   int fd;
   int fd_dat;
   short tamreg;
   short tamblock;
   short clave_activa;
   short trozo_activa;
   short nclaves;
   IXXCLAVE *claves;
   short openmode; /* 0 normal , -2 solo lectura , -4 modo exclusivo */
   unsigned short openid; /* numero identificador del usuario de este fichero */
   long reg_cur;
   long blo_cur;
   short pos_cur;
   Uchar **DatosCurr; /* claves compuestas del corriente para preveer borrado del mismo */
   short PosCurr[IXXMAXLEVEL];
   IXXCABEZA cabecera;
   long maximo_registros;
   long maximo_bloques;
   long *lista_bloqueos;
   long max_bloqueos;
   int  numero_bloqueos;
   long control_numblocks; /* saber si se ha ampliado y en cuanto se pueda cerrar y abrir */
   char *nombreixx;
   char *nombredat;
   short contenidos[IXXMAXCONTENIDO];
   Uchar *contenido;
   long ocupacion_total;
   short pseudo_cierra;
   void *pseudo_guarda;
   FgvStdTabla dtabla;  
   short tipo_datos;
   long *NumeroPosicion;
   int  flags_operacion;
   char *pathdef;
};


typedef struct ixx_fichero IXXFILE;

extern char  *ixx_ciudiu;
extern char  *lista_diu;


#ifdef FGVDOS
/*#define LK_WAIT              LK_NBLCK*/
#define LK_WAIT              LK_LOCK
#else
#define LK_WAIT              LK_LOCK
#endif

extern char *__Texto[];

#ifdef RWIXXMAIN

#ifndef NORWMALLOC
/* especial momentaneo */
extern void *rwmalloc(unsigned int);
extern void rwfree(void *);
extern void *rwrealloc(void *,unsigned int);
#define malloc rwmalloc
#define free rwfree
#define realloc rwrealloc

#endif

/* estructura general de los bloques */
/* 4 id */
/* 2 level */
/* 4 prev level */
/* 4 prev chain */
/* 4 next chain */
/* 2 n keys */
/* ... data ... */

/* identificadores */
#define IXXIDBCLAVE  0x1001
#define IXXIDBFRERE  0x1002
#define IXXIDBFREBL  0x1003

/* definicion funciones sistema */

#define eret(a,b,c) {IXXerror(a,b,c);return(-1);}
#define bret(a)  {iserrno=0;ixxstatus=0;ixxerrno=0;return(a);};
#define ret(a)   return(a)


/*#ifdef FGVDOS*/

/* para WINDOWS-DOS */

#ifndef FGVDOS
#ifndef SH_DENYNO
#define SH_DENYNO 0
#endif
#ifndef SH_DENYRW
#define SH_DENYRW 2
#endif
#ifndef O_BINARY
#define O_BINARY 0
#endif
#endif 

#define ixx_rename_ dsdb_rename

#define ixx_crea_nuevo(a)    dsdb_open_(a,O_CREAT|O_TRUNC|O_BINARY|O_RDWR,S_IREAD|S_IWRITE)
#define ixx_nuevo_cierra     dsdb_close_
#define ixx_abre_share(a)    dsdb_sopen_(a,O_RDWR | O_BINARY,SH_DENYNO)
#define ixx_abre_dat(a)      dsdb_sopen_(a,O_RDWR | O_BINARY,SH_DENYNO)
#define ixx_abre_exclu(a)    dsdb_sopen_(a,O_RDWR | O_BINARY,SH_DENYRW)
#define ixx_lee              dsdb_read_
#define ixx_graba            dsdb_write_
#define ixx_lseek(a,b,c)     ((long)dsdb_lseek_(a,((ds_off)b),c))
#define ixx_cierra           dsdb_close_
#define ixx_dat_cierra       dsdb_close_
#define ixx_lck_lseek(a,b,c) ((long)dsdb_lseek_(a,((ds_off)b),c))
#define ixx_lock             dsdb_lockf_
#define ixx_unlink_          dsdb_unlink_

/*#else


#define ixx_unlink_          unlink

extern int unix_eopen(char *,int);
extern int unix_sopen(char *,int);
extern int unix_close(int);

#define ixx_rename_ rename

#define ixx_crea_nuevo(a)    open(a,O_CREAT|O_TRUNC|O_RDWR,S_IREAD|S_IWRITE)
#define ixx_nuevo_cierra     close

#define ixx_abre_share(a)    unix_sopen(a,O_RDWR)
#define ixx_abre_dat(a)      open(a,O_RDWR)
#define ixx_abre_exclu(a)    unix_eopen(a,O_RDWR)
#define ixx_lee              read
#define ixx_graba            write
#ifdef LARGESEEK
#define ixx_lseek(a,b,c)     ((long)lseek(a,((off_t)b),c))
#else
#define ixx_lseek            lseek
#endif
#define ixx_cierra           unix_close
#define ixx_dat_cierra       close

#ifdef LARGESEEK
#define ixx_lck_lseek(a,b,c)     ((long)lseek(a,((off_t)b),c))
#else
#define ixx_lck_lseek        lseek
#endif
#ifdef FGVSCO
#define ixx_lock             lockf
#define fgv_lockf            lockf
#else
#define ixx_lock             fgv_lockf
#endif

#endif
*/

#endif



#define IXXDBFID        0x77

/* errores */
#define IXXEMAXCAMCLAVE 5001
#define IXXENOCREA      5002
#define IXXEEXISTE      5003
#define IXXENOEN        5004
#define IXXEMUCHOS      5005
#define IXXENOMEM       5006
#define IXXEMALFICH     5007
#define IXXELSEEK       5008
#define IXXEREAD        5009
#define IXXENOSUPORT    5010
#define IXXENOOPEN      5011
#define IXXEMALARGU     5012
#define IXXECORRUP      5013
#define IXXEKEXIST      5014
#define IXXEWRITE       5015
#define IXXEABREDAT     5016
#define IXXECIERRADAT   5017
#define IXXENOCURR      5018
#define IXXENOKEY       5019
#define IXXETOCADO      5020
#define IXXENOREC       5021
#define IXXEPRIME       5022
#define IXXEFINAL       5023
#define IXXEFLOCK       5024
#define IXXENOMASRECORD 5025
#define IXXERECURS      5026
#define IXXENOLOCK      5027
#define IXXELOCKED      5028
#define IXXETOPELOCK    5029
#define IXXECAMBIADO    5030
#define IXXEMULTIPLE    5031

/* tipos de clave */
#define IXXCLAVENODUP 0
#define IXXCLAVEDUP 1

/* tipos de campo*/
#define IXXCALFA 0
#define IXXCSHORT 1
#define IXXCLONG 2
#define IXXCFLOAT 3
#define IXXCDOUBLE 4

/* tipos "extra" de campo */
/* este tipo usar en potencias de 2 hasta el numero 128 */
#define IXXCNONE   0
#define IXXCNOCASE 1


/* modos de grabacion */
#define IXXGNUEVO 0
#define IXXGACTUA 1
#define IXXGSOLOI 2 /* especial para reconstrucciones ... solo graba el indice
y el registro viene en isrecnum */
#define IXXGPOSIC 3 /* especial para replica ... graba en la posicion especificada
y reyena el resto */

/* modeo de borrado */
#define IXXBCLAVE 2
#define IXXBACTUA 3

/* modos de apertura */
#define IXXCOMPARTE 0
#define IXXSOLOLEE -2
#define IXXEXCLUYE -4

/* modos lectura compatibles c-isam */
/* #define IXXEXCLLOCK     0x800           /* full file lock */
#define IXXLOCK         0x100           /* lock on read request */
#define IXXWAIT         0x400           /* lock on read, wait til can */

#define IXXFIRST        0               /* find logical first record */
#define IXXLAST         1               /* find logical last record */
#define IXXNEXT         2               /* find logical next record */
#define IXXPREV         3               /* find logical previous record */
#define IXXCURR         4               /* find "current" record */
#define IXXEQUAL        5               /* find exact key match */
#define IXXGREAT        6               /* find nearest greater than key */
#define IXXGTEQ         7               /* find exact or nearest key */
#define IXXLOW          8               /* find nearest lower than key */
#define IXXLOWEQ        9               /* find exact or nearest lower than key */



/* deben ser suplidas !! */
extern int (*ixxmen_aviso)(char *);
extern int (*ixxind_aviso)(int,long,long);
extern int (*ixxind_recespera)(int);
extern int (*ixxdat_aviso)(int,long,char *);


#ifdef RWIXXEXTRAMAIN
#undef RWIXXMAIN
#endif

/* funciones indice */
/* internas */
#ifdef RWIXXMAIN


extern long _e_log_open = 0L;
extern long _e_log_close = 0L;
extern long _e_log_read = 0L;
extern long _e_log_update = 0L;
extern long _e_log_write = 0L;
extern long _e_log_delete = 0L;


int   IXXerror(int,int,int);
int   ixxcompara_clave(Uchar *,Uchar *,IXXCLAVE *,int);
int   ixxbusca_en_bloque(short,short,short,Uchar *,long *,short *,int,short,int);
int   ixx_busca_clave(short,Uchar *,long *,long *,short *,int);
int   ixxinicializa_bloclave(int,short,short,long,Uchar *);
long  ixxnuevo_registro_datos(void);
long  ixxnuevo_bloque(void);
int   ixxinserta_clave(int,short,Uchar *,long,short *,long *,int);
int   ixxtouch(int);
int   ixxuntouch(int);
int   ixxtocado(int);
int   ixxlibera_bloque(long);
int   ixxlibera_registro(long);
int   ixxborra_clave(short,long *);
int   ixx_bloquea_escritura(int,int);
int   ixx_bloquea_lectura(int,unsigned short,int);
int   ixx_desbloquea_escritura(int,int);
int   ixx_desbloquea_lectura(int,unsigned short,int);
int   ixx_reg_esta_bloqueado(long);
int   ixx_reg_bloquea(long,int);
int   ixx_reg_desbloquea(long);
int   ixx_reconstruye(int);
int   ixx_limpia_pseudos(int);
int   ixx_fuera_cache(char *);

int   ixx_borra_respaldo(char *);
void  ixx_libera_respaldo(void);
void  ixx_libera_diu(void);
void  ixx_borra_diu(char *fichero);

/* funciones de portabilidad de datos ... usando como modelo el C-Isam */
int stchar( Uchar *,Uchar *,int);
int ldchar( Uchar *,int,Uchar *);
int ldint( Uchar * );
int stint(short,Uchar *);
long ldlong(Uchar *);
int stlong(long,Uchar *);
int stfloat(float,Uchar *);
float ldfloat(Uchar *);
double lddbl(Uchar *);
int stdbl(double,Uchar *);
/***********************************************************************/


#endif

/* externas */
int   ixx_nuevo_indice(char *,short,IXXCLAVE *,short,char **,char *,FgvStdTabla *,char *pathdef);
int   ixx_abre_indice(char *,int,char **,char *,int,FgvStdTabla *,char *pathdef);
int   ixx_cierra_indice(int);
int   ixx_graba_indice(int,Uchar *,int);
int   ixx_borra_indice(int,Uchar *,int);
int   ixx_lee_indice(int,Uchar *,int,int,int,int,int);
int   ixx_libera(int,long);
int   ixx_reconstruye(int);
int   ixx_pon_contenido(int,Uchar *,int);
int   ixx_fin_contenido(int);
int   ixx_anula_un_dat(void);
int   ixx_repon_un_dat(int);
int   ixx_ocupacion_total(int);
int   ixx_limpia_pseudos(int);
int   ixx_fuera_cache(char *);
int   ixx_tam_fichero(int,long *);
void  AjustaPunterosSegunDatos(int modo);

#ifdef RWIXXMAIN

int ixx_haydiu = 0;

/* globales */
/*******/
extern int iserrno = 0;
extern long isrecnum = 0L;
extern int no_corrup_error = 0;
/*******/

extern int default_dbf_id;
extern int ixx_datos_dbf;

/*
int default_dbf_id = 0;
#ifdef CISAM_DAT
int ixx_datos_dbf = 0;
#else
int ixx_datos_dbf = 1;
#endif
*/

unsigned char _any_update = 98;
unsigned char _mes_update = 1;
unsigned char _dia_update = 1;

int ixx_inicial = 1;

int ixx_ojo_recursion = 0;

extern int ixxstatus = 0;
extern int ixxerrno = 0;

extern int ixx_absoluto;

extern IXXFILE *ixxfich = NULL;
extern IXXFILE *ixxfichs[IXXMAXFILES] = {NULL,};
/* buffers temporales de bloque para: */
extern Uchar *ixxbloque = NULL;  /* lectura general de clave */
extern Uchar *ixxbloque2 = NULL; /* grabacion/comprobacion de clave */
extern Uchar *ixxbloque3 = NULL; /* grabacion/borrado/comprobacion de clave */
extern Uchar *ixxbloque4 = NULL; /* record free list */
extern Uchar *ixxbloque5 = NULL; /* block free list */
extern short ixxtamblock = 0;
/* Bufers de datos compuestos de las claves */
extern Uchar **Datos = NULL;     /* para lectura en general */
extern Uchar **Datos2 = NULL;    /* para borrado en general */
extern short *datos_dim = NULL;
extern short datos_dim1 = 0;

extern Uchar *Dat_Cur = NULL;    /* buffer de registro par operaciones internas */
extern short len_dat_cur = 0;
/* mapa temporal de posicionamiento de una clave a traves de los
niveles */
extern short _Posblock[IXXMAXLEVEL] = {0,}; /* 100 niveles es inalcanzable */

/*  para accesos directos */
extern int ixx_directo_abre_datos(char *,char **,char *,short *,IXXFILE *,int *,FgvStdTabla *,char *pathdef);
extern int ixx_directo_cierra_datos(int);
extern int ixx_directo_tamfichero_datos(int,int,long,long *);
extern int ixx_directo_lee_datos(int,long,Uchar *,int);
extern int ixx_directo_graba_datos(int,long,Uchar *,int);
extern int ixx_directo_borra_datos(int,long,int);


extern int dat_abre(char *,char **,char *,short *,IXXFILE *,int *,FgvStdTabla *,char *pathdef);
extern int dat_cierra(int);
extern int dat_lee(int,long,Uchar *,int);
extern int dat_graba(int,long,Uchar *,int,int);
extern int dat_borra(int,long,int);
extern int dat_tamfichero(int,long,int,long *);

extern int dbf_abre(char *,char **,char *,short *,IXXFILE *,int *,FgvStdTabla *,char *pathdef);
extern int dbf_cierra(int);
extern int dbf_lee(int,long,Uchar *,int);
extern int dbf_graba(int,long,Uchar *,int,int);
extern int dbf_borra(int,long,int);
extern int dbf_tamfichero(int,long,int,long *);


extern int maxi_regi(IXXFILE *);
extern int dat_clave(IXXFILE *,Uchar *,Uchar **,short,long,int,int,int);


#ifdef CISAM_DAT
extern int (*ixxdat_abre)(char *,char **,char *,short *,IXXFILE *,int *,FgvStdTabla *,char *pathdef) = dat_abre;
extern int (*ixxdat_cierra)(int) = dat_cierra;
extern int (*ixxdat_lee)(int,long,Uchar *,int) = dat_lee;
extern int (*ixxdat_graba)(int,long,Uchar *,int,int) = dat_graba;
extern int (*ixxdat_borra)(int,long,int) = dat_borra;
extern int (*ixxdat_tamfichero)(int,long,int,long *) = dat_tamfichero;

int (*ixxdiu_abre)(char *,char **,char *,short *,IXXFILE *,int *,FgvStdTabla *,char *pathdef) = dat_abre;
int (*ixxdiu_cierra)(int) = dat_cierra;
int (*ixxdiu_graba)(int,long,Uchar *,int) = dat_graba;
int (*ixxdiu_borra)(int,long,int) = dat_borra;
#else
extern int (*ixxdat_abre)(char *,char **,char *,short *,IXXFILE *,int *,FgvStdTabla *,char *pathdef) = dbf_abre;
extern int (*ixxdat_cierra)(int) = dbf_cierra;
extern int (*ixxdat_lee)(int,long,Uchar *,int) = dbf_lee;
extern int (*ixxdat_graba)(int,long,Uchar *,int,int) = dbf_graba;
extern int (*ixxdat_borra)(int,long,int) = dbf_borra;
extern int (*ixxdat_tamfichero)(int,long,int,long *) = dbf_tamfichero;


int (*ixxdiu_abre)(char *,char **,char *,short *,IXXFILE *,int *,FgvStdTabla *,char *pathdef) = dbf_abre;
int (*ixxdiu_cierra)(int) = dbf_cierra;
int (*ixxdiu_graba)(int,long,Uchar *,int) = dbf_graba;
int (*ixxdiu_borra)(int,long,int) = dbf_borra;
#endif

extern int (*ixxdat_clave)(IXXFILE *,Uchar *,Uchar **,short,long,int,int,int) = dat_clave;
extern int (*ixxdat_max_reg)(IXXFILE *) = maxi_regi;


extern int diu_abre(char *,char **,char *,short *,IXXFILE *,int *,FgvStdTabla *,char *pathdef);
extern int diu_cierra(int);
extern int diu_graba(int,long,Uchar *,int,int);
extern int diu_borra(int,long,int);


/* de momento !!! */
static int demomento_espera(void)
{
	static unsigned long cuenta = 0;
	cuenta++;
	if ((cuenta%1000))
	{
		pulsatecla();
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

extern int (*ixxdat_micro_espera)(void) = demomento_espera;
/******************/

extern int (*ixx_incidencia)(int,char *) = NULL;
extern char *(*ixxquita_path)(char *) = NULL;

#else

#ifdef RWIXXEXTRAMAIN
extern IXXFILE *ixxfich;
#endif

extern int ixx_haydiu;

/*******/
extern int iserrno;
extern long isrecnum;
extern int no_corrup_error;
/*******/

extern unsigned char _any_update;
extern unsigned char _mes_update;
extern unsigned char _dia_update;


extern int default_dbf_id;
extern int ixx_datos_dbf;

extern int ixxstatus;
extern int ixxerrno;

extern int ixx_absoluto;

extern IXXFILE *ixxfichs[IXXMAXFILES];

extern int (*ixxdat_abre)(char *,char **,char *,short *,IXXFILE *,int *,FgvStdTabla *,char *pathdef);
extern int (*ixxdat_cierra)(int);
extern int (*ixxdat_lee)(int,long,Uchar *,int);
extern int (*ixxdat_graba)(int,long,Uchar *,int,int);
extern int (*ixxdat_borra)(int,long,int);
extern int (*ixxdat_clave)(IXXFILE *,Uchar *,Uchar **,short,long,int,int,int);
extern int (*ixxdat_max_reg)(IXXFILE *);
extern int (*ixxdat_tamfichero)(int,long,int,long *);
extern int (*ixxdat_micro_espera)(void);

extern int (*ixx_incidencia)(int,char *);
extern char *(*ixxquita_path)(char *);


extern int (*ixxdiu_abre)(char *,char **,char *,short *,IXXFILE *,int *,FgvStdTabla *,char *pathdef);
extern int (*ixxdiu_cierra)(int);
extern int (*ixxdiu_graba)(int,long,Uchar *,int);
extern int (*ixxdiu_borra)(int,long,int);


#endif


#ifndef min
#define min(a, b)  (((a) < (b)) ? (a) : (b)) 
#endif

#ifndef max
#define max(a, b)  (((a) > (b)) ? (a) : (b)) 
#endif

#endif