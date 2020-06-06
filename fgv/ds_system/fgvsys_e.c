 
/**********************************************************************/
/**********************************************************************/
/*                    DIAGRAM9                                   */
/**********************************************************************/
/**********************************************************************/

#include <fgvsys.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef ESDOS
#include <io.h>
#include <dos.h>
#ifndef WATCOM
#ifndef _MSC_VER 
#include <dir.h>
#endif
#endif


#ifndef FGV32
#ifndef _MSC_VER
#include <dirent.h>
#endif
#endif

#else
#ifdef ESXENIX
#include <sys/ndir.h>
#else
#include <dirent.h>
#endif
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

#include <rwmalloc.h>
#include <rtermin.h>
#include <rentra.h>
#define _NUEVODEF
#include <rfecha.h>
#include <rdef.h>       /* ya incluye el isam.h */
#include <rmante.h>
#include <rcalcul.h>
#include <rwmenus.h>

#include <math.h>

#include "fgvixx.h"

#include <stdlib.h>
/*extern char *getenv();*/


extern char bufpan[PANBUFSIZE];
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
extern char rw_version[256];

extern char *lee_linea();

extern int fgv_ver_euro;

extern int repinta_texto;

extern int ultimo_ipfi;

extern int (*general_int)();

extern int agiavisa();
extern int agisies;
extern int agivez;
extern int magi;
extern int agitei;

extern int es_la_doble_;
extern int va_con_doble_;
extern char nomapli_doble[260];
extern char empresa_doble[260];

extern int dont_close_this_;

#ifdef ESDOS
extern int (*timer_especial_int)();
#endif

extern int data_fecha_conmutar;

extern int mimodo_debug;

char *sqlsrv_conexion = NULL;

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


#ifndef SEEK_END
#define SEEK_END 2
#endif	   

/********************************************************************
CON MODULO TRACER
********************************************************************/
#ifndef DSMINIBASICO

extern int tracer_int;
extern int ntr_br;
extern int tracer_break[];

extern int flag_leedefn;

int pasa_a_int(double);

extern int rw_noop();
extern int rw_subfun();
extern int rw_skip();
extern int rw_atira();
extern int rw_cpath();
extern int rw_operror();
extern int rw_oper();
extern int rw_kfecha();
extern int rw_tratafi();
extern int rw_pcontr();
extern int rw_mante();
extern int rw_consul();
extern int rw_menu();
extern int rw_abrtrel();
extern int rw_entcamp();
extern int rw_quitabt();
extern int rw_cietrel();
extern int rw_leerel();
extern int rw_grarel();
extern int rw_camind();
extern int rw_clarel();
extern int rw_entdat();
extern int rw_pinpan();
extern int rw_pindat();
extern int rw_bucind();
extern int rw_borrel();
extern int rw_abrerel();
extern int rw_cierel();
extern int rw_defecfi();
extern int rw_dcamrel();
extern int rw_drelrel();
extern int rw_creind();
extern int rw_borind();
extern int rw_locind();
extern int rw_ulocind();
extern int rw_libind();
extern int rw_buclin();
extern int rw_mensa();
extern int rw_blin();
extern int rw_atri();
extern int rw_cuadro();
extern int rw_zonabla();
extern int rw_cabeza();
extern int rw_quitab();
extern int rw_graf();
extern int rw_for();
extern int rw_cond();
extern int rw_inf();
extern int rw_goto();
extern int rw_pausa();
extern int rw_letras();
extern int rw_impre();
extern int rw_finimp();
extern int rw_pincam();
extern int rw_gosub();
extern int rw_wpon();
extern int rw_wqui();
extern int rw_goco();
extern int rw_gobu();
extern int rw_ptec();
extern int rw_abrbrel();
extern int rw_abrerte();
extern int rw_system();
extern int rw_pulsate();
extern int rw_sleep();
extern int rw_break();
extern int rw_nobreak();
extern int rw_corre();
extern int rw_aborta();
extern int rw_acaba();
extern int rw_sal();
extern int rw_edita();
extern int rw_memo();
extern int rw_hora();
extern int rw_scroll();
extern int rw_imprime();
extern int rw_lineas();
extern int rw_alarma();

extern int rw_dareg();
extern int rw_ponreg();
extern int rw_campo();
extern int rw_crea_buffer();
extern int rw_mete_buffer();
extern int rw_saca_buffer();
extern int rw_fin_buffer();
extern int rw_lee_texto();
extern int rw_graba_texto();
extern int rw_bmenug();
extern int rw_quesis();
extern int rw_color();
extern int rw_tecla();
extern int rw_protefun();
extern int rw_cua1();
extern int rw_subopc();
extern int rw_trataendef();
extern int rw_repinta();
extern int rw_colodef();
extern int rw_posedita();
extern int rw_campoatri();
extern int rw_sialarma();
extern int rw_debug();

extern int rw_iniciocom();
extern int rw_leecom();
extern int rw_grabacom();
extern int rw_fincom();
extern int rw_estadocom();
extern int rw_ontimer();
/*extern int rw_fintimer();*/
extern int fgvestadocontrol();
extern int rw_salvadatos();
extern int rw_repondatos();
extern int rw_mddirec();
extern int rw_entorno();
extern int rw_ex_enlaza_mpfi();
extern int rw_ex_desenlaza_mpfi();
extern int rw_agiver();
extern int agi_topedemo();
extern int ag_validachk();
extern int ag_validaapl();
extern int agi_modowin();
extern int agi_alt_fich();
extern int agicomprime();
extern int agidescomprime();
extern int agidetar();
extern int agiatar();
extern int agiponclave();

extern int inidde();
extern int execdde();
extern int pokedde();
extern int findde();
extern int peekdde();

extern int agibegint();
extern int agianulat();
extern int agibient();

extern int agi_copiafich();
extern int agi_afegirfich();
extern int agi_pdir();
extern int agi_sdir();
extern int agi_versionrtme();
extern int agi_externoexe();
extern int agi_renombra();
extern int agi_mkdir();
extern int agi_rmdir();

extern int agi_cdefecfi();
extern int agi_pdefecfi();

extern int agi_lineainforme();

extern int agi_deltree();


extern int agi_cargawmenu();
extern int agi_liberawmenu();
extern int agi_wmenu();
extern int agi_nuevawopcion();
extern int agi_borrawopcion();
extern int agi_datoswhoja();
extern int agi_grabawmenu();

extern int rw_instala();
extern int rw_datos_paquete();
extern int rw_bloquea_master();
extern int rw_desbloquea_master();
extern int rw_copiainstalacion();

extern int rw_graba_bmp_pantalla();

extern int rw_graba_defs_pantallas();
extern int rw_graba_menu_pantallas();

extern int rw_daclaves();

extern int rw_va_con_doble();
extern int rw_es_la_doble();
extern int rw_test_doble();
extern int rw_hiden_indicator();
extern int rw_cambia_doble();

extern int rw_dime_mezcla();

extern int rw_set_other_buf();
extern int rw_set_restore_buf();

extern int rw_posiciona();

extern int rw_setlincolor();

extern int rw_ocultalineas();

extern int fgv_formato_hacienda();
extern int fgv_imprime_hacienda();

extern int fgv_ejecuta_remoto();
extern int fgv_recibe_fichero();
extern int fgv_envia_fichero();

extern int fgv_cambia_de_def();

extern int fgv_control();

extern int fgv_parte_fichero();

extern int fgv_ipremoto();
extern int fgv_ipnombre();

extern int fgv_enlaza_proc();
extern int fgv_desenlaza_proc();
extern int fgv_carga_def();
extern int fgv_descarga_def();
extern int fgv_referencia_def();

extern int fgv_consul_clave();

extern int fgv_cargadll();
extern int fgv_descargadll();
extern int fgv_alfacalldll();

extern int fgv_wcontrol();
extern int fgv_finwcontrol();

extern int pon_salto_de_lineas();

extern int fgv_xabre();
extern int fgv_xcierra();
extern int fgv_xlee();
extern int fgv_xgraba();
extern int fgv_xleenumero();
extern int fgv_xgrabanumero();
extern int fgv_xposicion();
extern int fgv_xleededef();
extern int fgv_xgrabaadef();

extern int fgv_enclave();

extern int fgv_modo_bmp();

extern int fgv_modo_relacion();

extern int fgvsubejecutanp();

extern int fgv_enviafich();
extern int fgv_recibefich();
extern int fgv_abre_word();
extern int fgv_salva_word();
extern int fgv_prop_word();
extern int fgv_carga_txt_word();
extern int fgv_asigna_campo_word();
extern int fgv_asigna_texto_word();
extern int fgv_imprime_word();

extern int fgv_especifica();

extern int fgv_menuitem();

extern int fgv_valorini();

extern int ds_sql();

extern char *snap_shoot_menu;

extern int fgv_printpa();

#ifdef RWXARXA
#ifdef RWSERVER
extern int en_conexion_externa;
#endif
#endif

extern int data_hiden_indicator;

int *puntero_a_cual = NULL;

extern int requiere_reposicionar;
extern int qj_forzado;

extern int rw_consul_clave();

/*
extern int rw_abred();
extern int rw_leed();
extern int rw_grabad();
extern int rw_cierrad();
*/

extern int fgv_indice_def();

extern int fgvCreaPaqueteCiu();
extern int fgvProcesaPaqueteCiu();
extern int fgvLeePaqueteCiu();

extern int fgv_dscorreo_envia();
extern int fgv_dscorreo_recibe();
extern int fgv_cambio_empresa();
extern int fgv_browse();
extern int fgvsimplelineal();

extern int fgvfocoteclado();
extern int fgvdestruyecontrol();
extern int fgvrefrescacontrol();

extern int fgvsubpinpa();
extern int fgvborrahijospan();

extern int fgvcontrolsimple();

extern int fgvbotonsalida();

extern int fgvocxregistra();
extern int fgvocxactiva();
extern int fgvocxlibera();
extern int fgvocxinvoca();

extern int fgv_maincommand();
extern int fgv_oemansi();
extern int fgv_ansioem();
extern int rw_pito();

extern int modo_desatendido;

extern char BufferSerie[250];


extern int numero_build;
extern int fgv_internal_build;
extern char rw_version[256];


/*int (*siscal[256])(int,int,int) = {/* (!) == Cuidado con estos ASCII */
int (*siscal[256])() = 
{/* (!) == Cuidado con estos ASCII */
      rw_atira,  /* 0 #define TDFIB  '\0'*/
      rw_atira,  /* 1 #define TDPAN  '\1'*/
      rw_atira,  /* 2 #define TDDEF  '\2'*/
      rw_atira,  /* 3 #define TDBIN  '\3'*/
      rw_atira,  /* 4 #define TDBSS  '\4'*/
      rw_atira,  /* 5 #define TNAPL  '\5'*/
      rw_cpath,  /* 6 #define TPASI  '\6'*/
      rw_cpath,  /* 7 #define TNASI  '\7'*/
      rw_break,  /* 8 #define TBREAK '\10'*/
      rw_operror,/* 9 #define TERROD '\11'*/
      rw_noop,   /* 10 (!) */
      rw_kfecha, /* 11 #define TPROTE '\13'*/
      rw_tratafi,/* 12 #define TTEAB  '\14'*/
      rw_noop,   /* 13 (!) */
      rw_tratafi,/* 14 #define TTECI  '\16'*/
      rw_pcontr, /* 15 #define TFICO  '\17'*/
      rw_noop,   /* 16 (!) */
      rw_noop,   /* 17 (!) */
      rw_mante,  /* 18 #define TMANTE '\22'*/
      rw_menu,   /* 19 #define TMENU  '\23'*/
      rw_nobreak,/* 20 #define TNBREAK '\24'*/
      rw_corre,  /* 21 #define TRUN   '\25'*/
      rw_aborta, /* 22 #define TABORTA '\26' */
      rw_acaba,  /* 23 #define TACABA  '\27' */
      rw_sal,    /* 24 #define TSAL    '\30' */
      rw_cpath,  /* 25 #define TFNAME  '\31' */
      rw_noop,   /* 26 (!) */
      rw_edita,  /* 27 #define TEDITA   '\33'*/
      rw_memo,   /* 28 #define TMEMO    '\34'*/
      rw_scroll ,/* 29 #define TSCROL   '\35'*/
      rw_imprime,/* 30 #define TIMPRI   '\36'*/
      rw_lineas ,/* 31 #define TLINEA   '\37'*/
      rw_hora,   /* 32 #define THORA    '\40'*/
      rw_alarma, /* 33 #define TALARMA  '\41'*/
      rw_noop,   /* 34 #define TNOALA   '\42'*/
      rw_abrtrel,/* 35 #define TFABR1 '#'*/
      rw_campo,  /* 36 */
      rw_campo,  /* 37 */
      rw_entcamp,/* 38 #define TFENC  '&'*/
      rw_quitabt,/* 39 #define TQBLT  '\''*/
      rw_dareg,  /* 40 */
      rw_ponreg, /* 41 */
      rw_campo,  /* 42 */
      rw_campo,  /* 43 */
      rw_campo,  /* 44 */
      rw_menu,   /* 45 */
      rw_bmenug, /* 46 */
      rw_atira,  /* 47 #define TDBAS  '/'*/
      rw_crea_buffer,   /* 48 */
      rw_mete_buffer,   /* 49 */
      rw_saca_buffer,   /* 50 */
      rw_fin_buffer,   /* 51 */
      rw_lee_texto,   /* 52 */
      rw_graba_texto,   /* 53 */
      rw_quesis,   /* 54 */
      rw_color,   /* 55 */
      rw_cpath,  /* 56 #define TPASP  '8'*/
      rw_cpath,  /* 57 #define TNASP  '9'*/
      rw_tecla,   /* 58 */
      rw_protefun,   /* 59 */
      rw_cua1,   /* 60 */
      rw_cua1,   /* 61 */
      rw_subopc, /* 62 */
      rw_repinta,   /* 63 */
      rw_cietrel,/* 64 #define TFCIE1 '@'*/
      rw_grarel, /* 65 #define TFGRA  'A'*/
      rw_leerel, /* 66 #define TFLEE  'B'*/
      rw_camind, /* 67 #define TFTCL  'C'*/
      rw_clarel, /* 68 #define TFCLA  'D'*/
      rw_entdat, /* 69 #define TFENT  'E'*/
      rw_pinpan, /* 70 #define TFPIP  'F'*/
      rw_pindat, /* 71 #define TFPID  'G'*/
      rw_bucind, /* 72 #define TFBUC  'H'*/
      rw_borrel, /* 73 #define TFBOR  'I'*/
      rw_abrerel,/* 74 #define TFABR  'J'*/
      rw_cierel, /* 75 #define TFCIE  'K'*/
      rw_defecfi,/* 76 #define TFDEF  'L'*/
      rw_dcamrel,/* 77 #define TFDFN  'M'*/
      rw_drelrel,/* 78 #define TFDFS  'N'*/
      rw_creind, /* 79 #define TFCRE  'O'*/
      rw_borind, /* 80 #define TFDEL  'P'*/
      rw_locind, /* 81 #define TFBLQ  'Q'*/
      rw_ulocind,/* 82 #define TFLIB  'R'*/
      rw_libind, /* 83 #define TFLIR  'S'*/
      rw_buclin, /* 84 #define TFBCL  'T'*/
      rw_mensa,  /* 85 #define TMENS  'U'*/
      rw_mensa,  /* 86 #define TCONF  'V'*/
      rw_mensa,  /* 87 #define TFMEAV 'W'*/
      rw_blin,   /* 88 #define TFBLI  'X'*/
      rw_atri,   /* 89 #define TFATR  'Y'*/
      rw_cuadro, /* 90 #define TFCUA  'Z'*/
      rw_zonabla,/* 91 #define TFBLAN '['*/
      rw_atira,  /* 92 #define TDFIC  '\\'*/
      cls,       /* 93 #define TFCLS  ']'*/
      rw_cabeza, /* 94 #define TFCAB  '^'*/
      rw_quitab, /* 95 #define TQBLF  '_'*/
      rw_graf,   /* 96 #define TGRAF  '`'*/
      rw_tratafi,/* 97 #define TTELE  'a'*/
      rw_for,    /* 98 #define TBUCL  'b'*/
      rw_cond,   /* 99 #define TCOND  'c'*/
      rw_tratafi,/* 100 #define TFITE  'd'*/
      rw_noop,   /* 101 #define TEND   'e'(!!)*/
      rw_inf,    /* 102 #define TFLIF  'f'*/
      rw_gosub,  /* 103 #define TGOSUB  'g'*/
      rw_operror,/* 104 #define TERROR 'h'*/
      rw_pito,      /* 105 #define TAVISO 'i'*/
      rw_pausa,  /* 106 #define TPAUSA 'j'*/
      rw_letras, /* 107 #define TLETR  'k'*/
      rw_impre,  /* 108 #define TIMPR  'l'*/
      rw_finimp, /* 109 #define TFIMP  'm'*/
      rw_trataendef,/* 110 */
      rw_oper,   /* 111 #define TOPER  'o'(!!)*/
      rw_pincam, /* 112 #define TPINTA 'p'*/
      rw_inf,    /* 113 #define TINFO  'q'*/
      rw_inf,    /* 114 #define TPRINT 'r'*/
      rw_goto,  /* 115 #define TGOTO 's'*/
      rw_inf,    /* 116 #define TFINF  't'*/
      rw_wpon,   /* 117 #define TWPON  'u'*/
      rw_wqui,   /* 118 #define TWQUI  'v'*/
      rw_tratafi,/* 119 #define TTEMP  'w'*/
      rw_goco,   /* 120 #define TGOCO  'x'(!!)*/
      rw_tratafi,/* 121 #define TTEGR  'y'*/
      rw_gobu,   /* 122 #define TGOBU  'z'*/
      rw_colodef,/* 123 */
      rw_posedita,/* 124 */
      rw_campoatri,   /* 125 */
      rw_ptec,   /* 126 #define TPTEC  '~'*/
      rw_abrbrel,/* 127 #define TFABRB '\177'*/
      rw_system, /* 128 #define TSYSTEM '\200'*/
      rw_pulsate,/* 129 #define TPTECLA '\201'*/
      rw_sleep,  /* 130 #define TSLEEP  '\202'*/
      rw_atira,   /* 131 obtener modulo */
      rw_sialarma,   /* 132 */
      rw_debug,   /* 133 */
      rw_noop,   /* 134 */
      fgv_cargadll,   /* 135 */
      fgv_descargadll,   /* 136 */
      fgv_alfacalldll,   /* 137 */
      rw_operror,   /* 138 */
      rw_campo,   /* 139 */
      fgv_indice_def,/* 140 */
      rw_ontimer,   /* 141 */
      /*rw_fintimer,   /* 142 */
	  fgvestadocontrol,   /* 142 */
      rw_wqui,   /* 143 */
      rw_salvadatos,   /* 144 */
      rw_repondatos,   /* 145 */
      rw_mddirec,   /* 146 */
      rw_entorno,   /* 147 */
      rw_consul,   /* 148 */
      rw_abrerte,   /* 149 */
      rw_ex_enlaza_mpfi, /* 150 */
      rw_ex_desenlaza_mpfi, /* 151 */
      rw_agiver,   /* 152 */
      agi_topedemo,   /* 153 */
      execdde,   /* 154 */
      inidde,   /* 155 */
      agi_modowin,   /* 156 */
      agi_alt_fich,   /* 157 */
      agicomprime,   /* 158 */
      agidescomprime,   /* 159 */
      agidetar,   /* 160 */
      agiponclave,/* 161 */
      agibegint,   /* 162 */
      agianulat,   /* 163 */
      agibient,   /* 164 */
      rw_trataendef,   /* 165 */
      agi_copiafich,   /* 166 */
      agi_afegirfich,   /* 167 */
      agi_pdir,   /* 168 */
      agi_sdir,   /* 169 */
      agi_versionrtme,   /* 170 */
      agi_externoexe,   /* 171 */
      agi_renombra,   /* 172 */
      agi_mkdir,   /* 173 */
      agi_rmdir,   /* 174 */
      agi_pdefecfi,   /* 175 */
      agi_cdefecfi,   /* 176 */
      agi_lineainforme,   /* 177 */
      agi_deltree,   /* 178 */
      agi_cargawmenu,/* 179 */
      agi_liberawmenu, /* 180 */
      agi_wmenu,       /* 181 */ 
      agi_nuevawopcion,/* 182 */
      agi_borrawopcion,/* 183 */
      agi_datoswhoja,  /* 184 */
      agi_grabawmenu,  /* 185 */
      agiatar,   /* 186 */    
      rw_instala,   /* 187 */
      rw_datos_paquete,   /* 188 */
      rw_bloquea_master,   /* 189 */
      rw_desbloquea_master,   /* 190 */
      rw_copiainstalacion,   /* 191 */
      rw_graba_bmp_pantalla,   /* 192 */
      rw_graba_defs_pantallas,   /* 193 */
      rw_graba_menu_pantallas,   /* 194 */
      rw_daclaves,   /* 195 */
      rw_va_con_doble,   /* 196 */
      rw_es_la_doble,   /* 197 */
      rw_test_doble,   /* 198 */
      rw_bucind,   /* 199 */
      rw_bucind,   /* 200 */
      rw_hiden_indicator,   /* 201 */
      rw_cambia_doble,   /* 202 */
      rw_dime_mezcla,   /* 203 */
      rw_set_other_buf,   /* 204 */
      rw_set_restore_buf,   /* 205 */
      rw_posiciona,   /* 206 */
      rw_salvadatos,   /* 207 */
      rw_repondatos,   /* 208 */
      rw_setlincolor,   /* 209 */
      rw_consul_clave,  /* 210 */
      rw_ocultalineas,/* 211 */
      fgv_formato_hacienda,   /* 212 */
      fgv_imprime_hacienda,   /* 213 */
      fgv_ejecuta_remoto,   /* 214 */
      fgv_recibe_fichero,   /* 215 */
      fgv_formato_hacienda,   /* 216 */
      fgv_cambia_de_def,   /* 217 */
      pokedde,   /* 218 */
      findde,   /* 219 */
      peekdde,   /* 220 */
      fgv_control,   /* 221 */
      fgv_parte_fichero,   /* 222 */
      ds_sql,   /* 223 */
      fgv_envia_fichero,   /* 224 */
      rw_tratafi,   /* 225 */
      rw_tratafi,   /* 226 */
      fgv_ipremoto,
      fgv_ipnombre,
      fgv_enlaza_proc,   /* 229 */
      fgv_desenlaza_proc,   /* 230 */
      fgv_carga_def,   /* 231 */
      fgv_descarga_def,   /* 232 */
      fgv_referencia_def,   /* 233 */
      /*
      "ENLAZA_PROCESO", "9E7598"	     ,229,NULL,
      "DESENLAZA_PROCESO","9E7598"	 	 ,230,NULL,
      "CARGA_DEF",        "4995"         ,231,NULL,
      "DESCARGA_DEF",     "49"           ,232,NULL,
      "REFERENCIA_DEF",   "4995"         ,233,NULL,
      */
      fgv_consul_clave,   /* 234 */
      rw_campo,   /* 235 */
      rw_imprime,   /* 236 */ /* impresin realocatado */
      rw_system,    /* system remoto */
      fgv_modo_bmp, /* 238 *//* control de windows */
      fgv_enviafich,   /* 239 */
      fgv_recibefich,   /* 240 */
      fgv_abre_word,   /* 241 */
      fgv_salva_word,   /* 242 */
      fgv_prop_word,   /* 243 */
      fgv_carga_txt_word,   /* 244 */
      fgv_asigna_campo_word,   /* 245 */
      fgv_imprime_word,   /* 246 */
      fgv_printpa,   /* 247 */
      fgv_asigna_texto_word,   /* 248 */
      fgv_especifica,   /* 249 */
      rw_noop,   /* 250 */
      rw_noop,   /* 251 */
      rw_noop,   /* 252 */
      rw_noop,   /* 253 */
      rw_subfun, /* inst 254 puerta a subfunciones */
      rw_skip, /* parametro standar , no usada como instruccion */
      }; /* 255 instrucciones directas mas 256 indirectas a traves de la func 254 */
    
    
    /*int (*extra_fgv[])(int,int,int) = {*/
    int nextra_fgv = 63;
    int (*extra_fgv[])() = 
    {
          fgv_wcontrol,
          fgv_finwcontrol,
          rw_menu,
          rw_bmenug,
          agi_pdir,
          agi_sdir,   
          agi_copiafich,   /* 166 */
          agi_afegirfich,   /* 167 */
          agi_renombra,   /* 172 */
          agi_mkdir,   /* 173 */
          agi_rmdir,   /* 174 */
          agi_deltree,   /* 178 */
          agicomprime,   /* 158 */
          agidescomprime,   /* 159 */
          agidetar,   /* 160 */
          agiatar,
          rw_tratafi,
          fgv_xabre, /* "XABRE"          ,"959598"          ,254,18, /* 12  */
          fgv_xcierra, /* "XCIERRA"        ,"98"              ,254,19, /* 14  */
          fgv_xlee, /* "XLEE"           ,"989895"          ,254,20, /* 97  */
          fgv_xgraba, /* "XGRABA"         ,"989895"          ,254,21, /* 121 */
          rw_tratafi,
          fgv_xleenumero, /* "XLEE_NUMERO",   "989898"             ,254,23, /* 225 */
          fgv_xgrabanumero, /* "XGRABA_NUMERO", "989898"             ,254,24, /* 226 */
          pon_salto_de_lineas,
          fgv_ipremoto, /* aqui devuelve la ip local */
          fgvCreaPaqueteCiu,
          fgvProcesaPaqueteCiu,
          fgvLeePaqueteCiu,
          rw_lineas,
          fgv_xposicion,
          fgv_xleededef,
          fgv_xgrabaadef,
          fgv_dscorreo_envia,/* "DSCORREO_ENVIA",  "959595959595"   ,254,34, */
          fgv_modo_relacion,
          fgv_cambio_empresa,
          fgv_enclave,
          fgv_dscorreo_recibe,
          fgv_browse,
          fgvsubejecutanp,
          fgvsimplelineal,
          fgvfocoteclado,
          fgvsubpinpa,
          fgvborrahijospan,
          fgvdestruyecontrol,
          fgvrefrescacontrol,
          fgvcontrolsimple,
          fgvbotonsalida,
          fgvocxregistra,
          fgvocxactiva,
          fgvocxlibera,
          fgvocxinvoca,
          fgv_maincommand,
          rw_zonabla,
		  fgv_menuitem,
		  fgv_valorini,
		  rw_iniciocom,
		  rw_leecom,
		  rw_grabacom,
		  rw_fincom,
		  fgv_oemansi,
		  fgv_ansioem,
		  NULL
    };

   
    
    
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


	extern char rw_user[11];
    
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
    
    extern int rtme_version1;
    extern int rtme_version2;
    extern unsigned char rtme_versionletra;
    
    static int nbuc = -1;
    static int *bucle;
    
    static int valcond = 0;
    
    extern int n_np = 0;

	extern int excel_alt;
    
    
    /* principio de rcalcul.c */
    
    /* error_puntero() == rw_error(43)
    {
    printf("\n\r\007Runtime Error: Variable puntero ilegal.\n\r");
    getchar();
    }
    */

    
char *cambiar_el_fich(char *path,char *buffer)
{
	char tmp[512];
	int punto;

	strcpy(tmp,path);
	punto = strbusca(path,"/fich/");
	if (punto > -1)
	{		
		strcpy(tmp+punto+1,fichdir);
		strcat(tmp,path+punto+5);
	}

	if (buffer)
	{
		strcpy(buffer,tmp);
		return buffer;
	}
	strcpy(path,tmp);
	return path;
}

int oem_a_ansi(unsigned char *c)
{
   switch (*c)
   {
   case     130 :*c=233;break;
   case     129 :*c=252;break;
   case     160 :*c=225;break;
   case     161 :*c=237;break;
   case     162 :*c=243;break;
   case     163 :*c=250;break;
   case     164 :*c=241;break;
   case     165 :*c=209;break;
   case     166 :*c=170;break;
   case     167 :*c=186;break;
   case     168 :*c=191;break;
   case     239 :*c=146;break;
   case     241 :*c=177;break;
   case     246 :*c=247;break;
   case     128 :*c=199;break;
   case     131 :*c=226;break;
   case     132 :*c=228;break;
   case     133 :*c=224;break;
   case     134 :*c=229;break;
   case     135 :*c=231;break;
   case     136 :*c=234;break;
   case     137 :*c=235;break;
   case     138 :*c=232;break;
   case     139 :*c=239;break;
   case     140 :*c=238;break;
   case     141 :*c=236;break;
   case     142 :*c=196;break;
   case     143 :*c=197;break;
   case     144 :*c=201;break;
   case     145 :*c=230;break;
   case     146 :*c=198;break;
   case     147 :*c=244;break;
   case     148 :*c=245;break;
   case     149 :*c=242;break;
   case     150 :*c=251;break;
   case     151 :*c=249;break;
   case     152 :*c=255;break;
   case     153 :*c=214;break;
   case     154 :*c=220;break;
   case     155 :*c=248;break;
   case     156 :*c=163;break;
   case     157 :*c=216;break;
   case     158 :*c=215;break;
   case     159 :*c=131;break;
   case     169 :*c=174;break;
   case     170 :*c=172;break;
   case     171 :*c=189;break;
   case     172 :*c=188;break;
   case     173 :*c=161;break;
   case     174 :*c=171;break;
   case     175 :*c=187;break;
   case     176 :*c=128;break;
   case     177 :*c=20; break;
   case     178 :*c=21; break;
   case     179 :*c=124;break;
   case     180 :*c=22; break;
   case     181 :*c=193;break;
   case     182 :*c=194;break;
   case     183 :*c=192;break;
   case     184 :*c=169;break;
   case     185 :*c=23; break;
   case     186 :*c=24; break;
   case     187 :*c=25; break;
   case     188 :*c=26; break;
   case     189 :*c=162;break;
   case     190 :*c=165;break;
   case     198 :*c=227;break;
   case     199 :*c=195;break;
   case     196 :*c=151;break;
   /*case     196 :*c='-';break;*/
   case     207 :*c=156;break;
   case     210 :*c=202;break;
   case     211 :*c=203;break;
   case     212 :*c=200;break;
   case     214 :*c=205;break;
   case     224 :*c=211;break;
   case     225 :*c=223;break;
   case     227 :*c=210;break;
   case     230 :*c=181;break;
   case     233 :*c=218;break;
   case     235 :*c=217;break;  
   }
   return 0;
}



int ansi_a_oem(unsigned char *c)
{
   switch (*c)
   {
   /* sin conversion 'atras'*/
   case     193:*c ='A';break;
   case		201:*c ='E';break;
   case		205:*c ='I';break;
   case		211:*c ='O';break;
   case		218:*c ='U';break;
   case		192:*c ='A';break;
   case		200:*c ='E';break;
   case		204:*c ='I';break;
   case		210:*c ='O';break;
   case		217:*c ='U';break;   
   case	    180:*c = 39;break; /* apostrofo */
   /* reversibles (mostly) */
   case     233:*c =130;break;
   case     252:*c =129;break;
   case     225:*c =160;break;
   case     237:*c =161;break;
   case     243:*c =162;break;
   case     250:*c =163;break;
   case     241:*c =164;break;
   case     209:*c =165;break;
   case     170:*c =166;break;
   case     186:*c =167;break;
   case     191:*c =168;break;
   case     146:*c =239;break;
   case     177:*c =241;break;
   case     247:*c =246;break;
   case     199:*c =128;break;
   case     226:*c =131;break;
   case     228:*c =132;break;
   case     224:*c =133;break;
   case     229:*c =134;break;
   case     231:*c =135;break;
   case     234:*c =136;break;
   case     235:*c =137;break;
   case     232:*c =138;break;
   case     239:*c =139;break;
   case     238:*c =140;break;
   case     236:*c =141;break;
   case     196:*c =142;break;
   case     197:*c =143;break;
   /*case     201:*c =144;break;*/
   case     230:*c =145;break;
   case     198:*c =146;break;
   case     244:*c =147;break;
   case     245:*c =148;break;
   case     242:*c =149;break;
   case     251:*c =150;break;
   case     249:*c =151;break;
   case     255:*c =152;break;
   case     214:*c =153;break;
   case     220:*c =154;break;
   case     248:*c =155;break;
   case     163:*c =156;break;
   case     216:*c =157;break;
   case     215:*c =158;break;
   case     131:*c =159;break;
   case     174:*c =169;break;
   case     172:*c =170;break;
   case     189:*c =171;break;
   case     188:*c =172;break;
   case     161:*c =173;break;
   case     171:*c =174;break;
   case     187:*c =175;break;
   case     128:*c =176;break;
   case     20: *c =177;break;
   case     21: *c =178;break;
   /*case     124:*c =179;break;*/
   case     22: *c =180;break;
   /*case     193:*c =181;break;*/
   case     194:*c =182;break;
   /*case     192:*c =183;break;*/
   case     169:*c =184;break;
   case     23: *c =185;break;
   case     24: *c =186;break;
   case     25: *c =187;break;
   case     26: *c =188;break;
   case     162:*c =189;break;
   case     165:*c =190;break;
   case     227:*c =198;break;
   case     195:*c =199;break;
   
   case     156:*c =207;break;
   case     202:*c =210;break;
   case     203:*c =211;break;
   /*case     200:*c =212;break;*/
   /*case     205:*c =214;break;*/
   /*case     211:*c =224;break;*/
   case     223:*c =225;break;
   /*case     210:*c =227;break;*/
   case     181:*c =230;break;
   /*case     218:*c =233;break;*/
   /*case     217:*c =235;break;*/
   }
   return 0;
}

    
int memo_strcpy(mm,str)
int mm;
char *str;
{
	   int i = strlen(str);
	   if (i > 250)
	      str[250] = 0;
       if (no_memo_interna(mm)) 
       {
          free(v_memo(mm)->alfabetica);
          v_memo(mm)->alfabetica = strmalloc(i+1);
       }
       strcpy(v_memo(mm)->alfabetica,str);
       return(0);
}
    
int comoes_cam(p,tk0,tip0,lon0,f0,c0,ord)
short p,*tk0,*tip0,*lon0,*f0,*c0,*ord;
{
             int f,c,tk,tip,lon,or = 1;
             
             if (p < 0) 
             {
                c = (-p) - 1;
                if (c / 10000) {c %= 10000;or = -1;}
                f = -1;
                tk = 0;
                if ((tip = que_memo_tipo(c)) == CHARTIPO)
                   lon = MAXDEFECTO;
                else
                   if (tip == FECHATIPO)
                      lon = FECHATAMA;
                   else {
                      lon = DOUBLETAMA;
                      tk=1;
                   }
             }
             else 
             {
                f = p % 100;
                if (f >= 50) {f -= 50;or = -1;}
                f = v_prog(f);
                c = p / 100;
                lon = v_mpfi(f)->f_campos[c].c_long;
                tip = v_mpfi(f)->f_campos[c].c_tipo;
                tk = 0;
                if (tip == DOUBLETIPO)
                   tk = 1;
             }
             if (tk0 != (short *)0) *tk0 = tk;
             if (tip0 != (short *)0) *tip0 = tip;
             if (lon0 != (short *)0) *lon0 = lon;
             if (f0 != (short *)0) *f0 = f;
             if (c0 != (short *)0) *c0 = c;
             if (or == -1 && ord != (short *)0) *ord = or;
             return(f);
          }
          
          
          int pasa_a_int(d) /* pasa double a short comprobando limites */
             double d;
          {
             if (d > 32767.0 || d < -32767.0) {
                rw_error(48);
                return(0);
             }
             return((int)d);
}
          
double conv_alfa(p)
char *p;
{
             return(rwatof(p));
}
          
int saca_dato_del_def(char *dato,M_VAL *val,int *f0,int *c0)
{
   int tipo = CHARTIPO;
   int f = *f0,i;
   int numero;

   val->valc[0] = 0;

   /*
             memcpy(val->valc,v_memo(c)->alfabetica,MAXDEFECTO);
       }
    }
    else
    {
       dtmp = v_memo(c)->numerica; 
       val->vald = dtmp;
    */

   quita_blancos(dato);
   strmayus(dato);
   *c0 = 0;
   
   if (!strcmp(dato,"NC"))
   {
       val->vald = (double)v_mpfi(f)->f_ncampos;
       tipo = DOUBLETIPO;
   }
   else if (!strcmp(dato,"NP"))
   {
       val->vald = (double)v_mpfi(f)->f_npanta;
       tipo = DOUBLETIPO;
   }
   else if (!strcmp(dato,"NK"))
   {
       val->vald = (double)v_mpfi(f)->f_nclaves;
       tipo = DOUBLETIPO;
   }
   else if (!strcmp(dato,"NR"))
   {
       val->vald = (double)v_mpfi(f)->f_nrelaci;
       tipo = DOUBLETIPO;
   }
   else if (!strcmp(dato,"TITULO"))
   {
       strcpy(val->valc,v_mpfi(f)->f_titulo);
       tipo = CHARTIPO;
   }
   else if (!strcmp(dato,"FICHERO"))
   {
       strcpy(val->valc,v_mpfi(f)->f_fichero);
       tipo = CHARTIPO;
   }
   else if (!strcmp(dato,"LONGITUD"))
   {
       val->vald = (double)v_mpfi(f)->f_lfich;
       tipo = DOUBLETIPO;
   }
   else if (!strcmp(dato,"TIPODATOS"))
   {
       val->vald = (double)v_mpfi(f)->tipo_datos;
       tipo = DOUBLETIPO;
   }
   else if (!strcmp(dato,"DEF"))
   {
       strcpy(val->valc,v_mpfi(f)->f_PathDef);
       tipo = CHARTIPO;
   }
   else if (!strcmp(dato,"IPFI"))
   {
       val->vald = (double)-1.0;
       tipo = DOUBLETIPO;
   }
   else if (!strcmp(dato,"ID"))
   {
       val->vald = (double)-1.0;
       tipo = DOUBLETIPO;
   }
   else
   {
       numero = (dato[1]-48)*100 + (dato[2]-48)*10 + (dato[3]-48);
       if (dato[0] == 'C')
       {
           if (numero >= 0 && numero < v_mpfi(f)->f_ncampos)
           {
               *c0 = numero;
               dato += 4;
               if (!strcmp(dato,"TIPO"))
               {
                   val->valc[1] = 0;
                   switch(v_mpfi(f)->f_campos[numero].c_tipo)
                   {
                   case CHARTIPO:
                       val->valc[0] = 'A';
                       break;
                   case DOUBLETIPO:
                   case FLOATTIPO:
                   case LONGTIPO:
                   case INTTIPO:
                       val->valc[0] = 'N';
                       break;
                   case FECHATIPO:
                       val->valc[0] = 'F';
                       break;
                   default:
                       val->valc[0] = 'X';
                       break;
                   }
                   tipo = CHARTIPO;
               }
               else if (!strcmp(dato,"LONGITUD"))
               {
                   val->vald = (double)long_campo(v_mpfi(f),numero);
                   tipo = DOUBLETIPO;                   
               }
               else if (!strcmp(dato,"IOFFSET"))
               {
                   val->vald = (double)v_mpfi(f)->f_campos[numero].c_offset;
                   tipo = DOUBLETIPO;
               }
               else if (!strcmp(dato,"ILON"))
               {
                   val->vald = (double)v_mpfi(f)->f_campos[numero].c_long;
                   tipo = DOUBLETIPO;
               }
               else if (!strcmp(dato,"ITIPO"))
               {
                   val->vald = (double)v_mpfi(f)->f_campos[numero].c_tipo;
                   tipo = DOUBLETIPO;
               }
               else if (!strcmp(dato,"NOMBRE"))
               {
                   strcpy(val->valc,v_mpfi(f)->f_campos[numero].c_nombre);
                   tipo = CHARTIPO;
               }
               else if (!strcmp(dato,"IDCAMPO"))
               {
                   strcpy(val->valc,v_mpfi(f)->f_campos[numero].c_lista);
                   tipo = CHARTIPO;
               }
               else if (!strcmp(dato,"MAXIMO"))
               {
                   strcpy(val->valc,v_mpfi(f)->f_campos[numero].c_maximo);
                   tipo = CHARTIPO;
               }
               else if (!strcmp(dato,"MINIMO"))
               {
                   strcpy(val->valc,v_mpfi(f)->f_campos[numero].c_minimo);
                   tipo = CHARTIPO;
               }
               else if (!strcmp(dato,"MENSAJE"))
               {
                   strcpy(val->valc,v_mpfi(f)->f_campos[numero].c_mante.c_mensaje);
                   tipo = CHARTIPO;
               }
               else if (!strcmp(dato,"DEFECTO"))
               {
                   strcpy(val->valc,v_mpfi(f)->f_campos[numero].c_mante.c_defecto);
                   tipo = CHARTIPO;
               }

/*
struct mante_def {
   char *c_mensaje;
   char *c_defecto;
   char *c_relacion;
   short  c_actual;
   short  c_modif;
   short  c_visual;
   short  c_lineal;
   short  c_qp;
   short  c_qpl;
};
#define MANTE_DEF struct mante_def

struct campo_def {
   char *c_nombre;
   char *c_lista;
   char *c_minimo;
   char *c_maximo;
   char *c_usi;
   MANTE_DEF c_mante;
   short  c_qc;
   short  c_offset;
   short  c_long;
   short  c_tipo;
   short  c_key;
   short  c_fflag;
};
*/
           }
       }
       else if (dato[0] == 'P')
       {
           if (numero >= 0 && numero < v_mpfi(f)->f_npanta)
           {
           }
       }
       else if (dato[0] == 'K')
       {
           if (numero >= 0 && numero < v_mpfi(f)->f_nclaves)
           {
			   if (numero >= 0 && numero < v_mpfi(f)->f_nclaves)
			   {  
				  sprintf(val->valc,"%03d",v_mpfi(f)->f_clave[numero].k_ncampos);
				  for (i = 0;i < v_mpfi(f)->f_clave[numero].k_ncampos;i++)
				  {
					  sprintf(val->valc+strlen(val->valc),"%03d",v_mpfi(f)->f_clave[numero].k_campo[i]-1);
				  }              
				  tipo = CHARTIPO;
			   }
           }
       }
       else if (dato[0] == 'R')
       {
           if (numero >= 0 && numero < v_mpfi(f)->f_nrelaci)
           {
  			   int totc = 0;
			   sprintf(val->valc,"%s|",v_mpfi(f)->f_relaci[numero].r_fichero);
               for (i = 0;i < MAXCAMPOSPORCLAVE;i++)
               {
				   if (!v_mpfi(f)->f_relaci[numero].r_campo[i]) break;
				   totc++;
			   }
			   sprintf(val->valc+strlen(val->valc),"%03d",totc);
               for (i = 0;i < totc;i++)
               {
				   sprintf(val->valc+strlen(val->valc),"%03d",v_mpfi(f)->f_relaci[numero].r_campo[i]);
			   }			   
         }
       }
   }


/*
struct pan_def {
   char p_nombrepan[ MAXPATH ];
   short  p_campoini;
   short  p_campofin;
   short  p_posfin;
   short  p_posref;
};
#define PAN_DEF struct pan_def
struct key_def {
   short *k_campo;
   short *k_orden;
   short k_ncampos;
   short k_fflag;
};
#define KEY_DEF struct key_def
#define CAMPO_DEF struct campo_def
struct rel_def {
   char *r_fichero;
   char *r_mfichero;
   short  *r_campo;
   short  r_obliga;
   short  r_leer;
};
#define REL_DEF struct rel_def
*/

   return tipo;
}




int busca_valor(f0,c0,val)
int *f0,*c0;
M_VAL *val;
{
 int tipo;
 double dtmp;
 int f = *f0;
 int c = *c0;
 int i;
 char *x; 
 
 if (f == F_INVALIDO) {val->vald = 0.0;return(DOUBLETIPO);}
 if (f == F_CASIVALIDO) f = v_memo(5011)->numerica;
 
 if ( c < 0 ) 
 {
    tipo =  -(c) - 1; /* tipo aqui se usa temporalmente */
    if (que_memo_tipo(tipo) != DOUBLETIPO)
    {
       if ( f < 0 ) 
       {
          /* buscar variable por nombre !! */
       }
       else
       {
          /* buscar campo por id */
          x = v_memo(tipo)->alfabetica;
          if (x[0] == '|')
          {             
             return(saca_dato_del_def(x+1,val,f0,c0));
          }

          if (x[0] == '&') /* buscar por desc */
          {
             for (i = 0;i < v_mpfi(f)->f_ncampos;i++)
             {
                if (!strcmp(v_memo(tipo)->alfabetica,v_mpfi(f)->f_campos[i].c_nombre))
                {
                   c = i;
                   break;
                }
             }
          }
          else 
          {
             for (i = 0;i < v_mpfi(f)->f_ncampos;i++)
             {
                if (!strcmp(v_memo(tipo)->alfabetica,v_mpfi(f)->f_campos[i].c_lista))
                {
                   c = i;
                   break;
                }
             }
          }
       }
    }
    else
       c = pasa_a_int(v_memo(tipo)->numerica);
    if (c > 5000)
    {
       if (c > (5000 + memoplin + maxpila *2 + nm_ex))
       {
          rw_error(43);
          c = 0;
       }
    }
    else
    if (c < 0 || (f < 0 && c >= nmemo) || (f > -1 && c >= v_mpfi(f)->f_ncampos))
    {
          rw_error(43);
          c = 0;
    }
 }
 
 if ( f < 0 ) {
    tipo = que_memo_tipo(c);
    if ( tipo == CHARTIPO || tipo == FECHATIPO )
    {
       if (no_memo_interna(c))
       {
          memset(val->valc,0,MAXDEFECTO);
          strcpy(val->valc,v_memo(c)->alfabetica);
       }
       else
       {
          memcpy(val->valc,v_memo(c)->alfabetica,MAXDEFECTO);
       }
    }
    else
    {
       dtmp = v_memo(c)->numerica; /* esto es por el compilador de xenix */
       val->vald = dtmp;
    }
 }
 else {
	if (c >= v_mpfi(f)->f_ncampos)
	{
          rw_error(43);
          c = 0;
	}
    tipo  = v_mpfi(f)->f_campos[c].c_tipo;
    ld_camcd(tipo,c,f,val);
 }
 
 *f0 = f;
 *c0 = c;
 return(tipo);
}
          
          
          int conv_var(tipop,tipo,val)
             int tipop,tipo;
          M_VAL *val;
          {
             int i;
             int o;
             double dtmp;
             
             i = q_tip(tipop);
             if (tipo == -1) return(i);
             o = q_tip(tipo);
             
             if (i != o) {
                if (i == NUMERO) {
                   if (o == ESFECHA)
                      val->vald = diasfecha(val->valc,0,0,0);
                   else {
                      conv_bool(val->valc);
                      val->vald = conv_alfa(val->valc);
                   }
                }
                else {
                   if (o == NUMERO) {
                      dtmp = val->vald;
                      sprintf(val->valc,"%lf",dtmp);
                      quitadec(val->valc);
                   }
                }
             }
             return(i);
          }
          
          
          int operanumero(val1,op,val2,resu)
             M_VAL val1;
          char op;
          M_VAL val2;
          M_VAL *resu;
          {
             char *tmp;
             switch (op) {
             case OSUMA :
             case OSUMAR :
                resu->vald = (val1.vald + val2.vald);
                break;
             case ORESTA :
             case ORESTAR :
                resu->vald = (val1.vald - val2.vald);
                break;
             case OMULTIPLICA :
             case OMULTIPLICAR :
                resu->vald = (val1.vald * val2.vald);
                break;
             case ODIVIDE :
             case ODIVIDER :
                if (val2.vald == (double)0)
                   resu->vald = (double)0;
                else
                   resu->vald = (val1.vald / val2.vald);
                break;
             case OPORCENT :
                resu->vald = ( (val1.vald * val2.vald) / 100 );
                break;
             case OMASPORCENT :
                resu->vald = ( val1.vald + ((val1.vald * val2.vald) / 100) );
                break;
             case OMENOSPORCENT :
                resu->vald = ( val1.vald - ((val1.vald * val2.vald) / 100) );
                break;
             case OREDONDEO :
                resu->vald = redondea(val1.vald,pasa_a_int(val2.vald));
                break;
             case ORESTO :
                if ( (long)val2.vald != 0L )
                   resu->vald = (double)( (long)val1.vald % (long)val2.vald );
                else
                   resu->vald = (double)0;
                break;
             case OIGUAL :
             case OIGUALR :
                resu->vald = val1.vald;
                break;
             case OPINTA :
                tmp = malloc(MAXDEFECTO);
                sprintf(tmp,"%lf",val1.vald);
                quitadec(tmp);
                sprintf(tmp,"%%%d.%dlf",strlen(tmp),decimales(tmp));
                cursor(pasa_a_int(val2.vald));
                v_printF(tmp,val1.vald);
                fflush(stdout);
                free(tmp);
                return(-1);
             case OENTRA :
                llama_entra(pasa_a_int(val1.vald),pasa_a_int(val2.vald),resu,0);
                break;
             case ORESET :
                resu->vald = (double)0;
                break;
             case '&' :
                resu->vald = (double)(unsigned char)val2.valc[0];
                break;
             default :
                break;
             }
             return(0);
          }
          
          int operaletra(val1,op,val2,resu)
             M_VAL val1;
          char op;
          M_VAL val2;
          M_VAL *resu;
									 {
                               int i,l,a,n;
                               int is;
                               VENT *venta;
                               
                               switch (op) {
                               case OSUMA :
                               case OSUMAR :
                                  if ( (strlen(val1.valc) + strlen(val2.valc)) >= MAXDEFECTO  ) break;
                                  strcpy(resu->valc,val1.valc);
                                  strcat(resu->valc,val2.valc);
                                  break;
                               case ORESTA :
                               case ORESTAR :
                                  if ( (is = strbusca(val1.valc,val2.valc)) != -1 ) {
                                     val1.valc[is] = '\0';
                                     a = strlen(val2.valc);
                                     sprintf(v_memo(5004)->alfabetica,"%d",(is+1)*100+a);
                                     is += a;
                                     strcpy(resu->valc,val1.valc);
                                     strcat(resu->valc,val1.valc+is);
                                  }
                                  else
                                  {
                                     strcpy(resu->valc,val1.valc);
                                     strcpy(v_memo(5004)->alfabetica,"0");
                                  }
                                  break;
                               case OPORCENT :
                                  i =atoi(val2.valc);
                                  if (i == 0) {
                                     sprintf(resu->valc,"%d",strlen(val1.valc));
                                     break;
                                  }
                                  if (i < 0) {
                                     is = -1;
                                     i = (-i);
                                  }
                                  else
                                     is = 0;
                                  if ( i < 100 ) 
                                  {
                                     l = strlen(val1.valc);
                                     if (i > l) break;
                                     if (is == 0)
                                        strcpy(resu->valc,val1.valc + i - 1);
                                     else {
                                        val1.valc[l-i+1] = '\0';
                                        strcpy(resu->valc,val1.valc);
                                     }
                                     break;
                                  }
                                  a = (i / 100) - 1;
                                  l = (i % 100);
                                  if (a < 0 || a >= MAXDEFECTO ) break;
                                  if ( (a+l) >= MAXDEFECTO ) l = MAXDEFECTO - a - 1;
                                  if (is == 0) 
                                  {
                                     val1.valc[a+l] = '\0';
                                     strcpy(resu->valc,val1.valc + a);
                                  }
                                  else 
                                  {
                                     n = strlen(val1.valc);	   
                                     if (a >= n) {resu->valc[0] = 0;break;}
                                     if ((n-a-l) < 0) 
                                     {
                                        l = n-a;
                                     }
                                     val1.valc[n-a] = '\0';
                                     strcpy(resu->valc,(val1.valc+(n-a-l)));
                                  }
                                  break;
                               case OMULTIPLICA :
                               case OMULTIPLICAR :
                                  i = ABS( atoi(val2.valc) );
                                  resu->valc[0] = '\0';
                                  a = strlen(val1.valc);
                                  for (l=0;l < i;l++) {
                                     if ( (strlen(resu->valc) + a) >= MAXDEFECTO ) break;
                                     strcat(resu->valc,val1.valc);
                                  }
                                  break;
                               case ODIVIDE :
                               case ODIVIDER :
                                  i = ABS( atoi(val2.valc) );
                                  resu->valc[0] = '\0';
                                  a = strlen(val1.valc);
                                  if (i > 0 && a/i >= 0)
                                     val1.valc[a / i] = '\0';
                                  else
                                     val1.valc[0] = '\0';
                                  strcpy(resu->valc,val1.valc);
                                  break;
                               case OIGUAL :
                               case OIGUALR :
                                  strcpy(resu->valc,val1.valc);
                                  break;
                               case OMASPORCENT :
                                  strmayus(val1.valc);
                                  strcpy(resu->valc,val1.valc);
                                  break;
                               case OMENOSPORCENT :
                                  strminus(val1.valc);
                                  strcpy(resu->valc,val1.valc);
                                  break;
                               case OREDONDEO :
                                  sprintf(resu->valc,val2.valc,val1.valc);
                                  break;
                               case OPINTA :
                                  pprint(atoi(val2.valc),val1.valc);
                                  fflush(stdout);
                                  return(-1);
                               case OPINPA :
                                  venta = w_ini(atoi(val1.valc),atoi(val2.valc));
                                  w_lee(venta,resu->valc);
                                  w_pinta(venta);
                                  w_fin(venta);
                                  break;
                               case OGRAPA :
                                  venta = w_ini(atoi(val1.valc),atoi(val2.valc));
                                  w_saca(venta,stdpan);
                                  w_graba(venta,resu->valc);
                                  w_fin(venta);
                                  break;
                               case ORESET :
                                  resu->valc[0] = '\0';
                                  break;
                               case OENTRA :
                                  llama_entra(atoi(val1.valc),atoi(val2.valc),resu,1);
                                  break;
                               case '&' : /* chr$ */
                                  resu->valc[0] = (char)atoi(val2.valc);
                                  resu->valc[1] = '\0';
                                  break;
                               default :
                                  return(-1);
   }
   return(0);
}

int operafecha(val1,tipo1,op,val2,tipo2,resu)
M_VAL val1;
int tipo1;
char op;
M_VAL val2;
int tipo2;
M_VAL *resu;
{
   double dias;
   int meses;
   int anos;
   double tmp;
   
   if (op == OENTRA) {
      llama_entra(atoi(val1.valc),atoi(val2.valc),resu,2);
      return(0);
   }
   
   if (tipo1 != FECHATIPO && tipo1 != CHARTIPO) {
      dias = rwatof(val1.valc);
      strcpy(val1.valc,"00.00.0000");
      fechadias(val1.valc,dias);
   }
   
   switch (op) {
   case OIGUAL :
   case OIGUALR :
      strcpy(resu->valc,val1.valc);
      break;
   case OSUMA :
   case OSUMAR :
      if (tipo2 == FECHATIPO) {
         dias = valsd(val2.valc,0,2);
         meses = (int)vals(val2.valc,3,2);
         anos = (int)vals(val2.valc,6,4);
      }
      else {
         tmp = rwatof(val2.valc);
         dias = redondea(tmp,3); /*** poner parte entera **********/
         anos = pasa_a_int(redondea((tmp - dias) * (double)1000000.0,0));
         dias = redondea(tmp,0);
         meses = pasa_a_int(redondea((tmp - dias) * (double)1000,0));
      }
      diasfecha(val1.valc,pasa_a_int(dias),meses,anos);
      strcpy(resu->valc,val1.valc);
      break;
   case ORESTA :
   case ORESTAR :
      if (tipo2 == FECHATIPO)
         dias = diasfecha(val2.valc,0,0,0);
      else {
         dias = rwatof(val2.valc);
      }
      if (dias < diasfecha(val1.valc,0,0,0)) {
         strcpy(val2.valc,"00.00.0000");
         fechadias(val2.valc,dias);
         dias = diasfecha(val1.valc,0,0,0);
         strcpy(val1.valc,val2.valc);
      }
      fechadias(val1.valc,dias);
      strcpy(resu->valc,val1.valc);
      break;
   case ORESET :
      strcpy(resu->valc,fecha());
      break;
   case OPORCENT:
      anos = atoi(val2.valc);
      strcpy(resu->valc,val1.valc);
      fechafor(val1.valc);
      if (anos == 0)
         strcat(resu->valc,val1.valc+60);
      else
         if (anos == 1)
            strcat(resu->valc,val1.valc+20);
         else
            if (anos == 2)
               strcat(resu->valc,val1.valc+30);
            break;
   default :
      return(-1);
   }
   return(0);
}

int calcula(f,tt)
int f;
int tt;
{
   return(rw_oper(-(f+1),tt,0));
}

int rw_pito(p,tt,faclin)
int p;
int tt;
int faclin;
{
      pito(125);
	  return 0;
}

int rw_oper(p,tt,faclin)
int p;
int tt;
int faclin;
{
			int i,f = ( (p >= 0) ? prog[p].indice : (-p)-1 );
         M_VAL val1;
         M_VAL val2;
         M_VAL resu;
         int tipo1;
         int tipo2;
         int tipor;
         int f1;
         int c1;
         int f2;
         int c2;
         int fr;
         int cr;
         char op;
         int sal;
         
         do {
            
            f1 = v_prog(oper[f].fichero1);
            c1 = oper[f].campo1;
            f2 = v_prog(oper[f].fichero2);
            c2 = oper[f].campo2;
            fr = v_prog(oper[f].ficheror);
            cr = oper[f].campor;
            op = oper[f].operacion;
            
            if (fr == -2) {
               fr = -1; /* eliminar marca de operacion encadenada */
            }
            
            if (op == OADIR) {
               v_memo(cr)->numerica = (double)c1;
               return(0);
            }
            
			
            tipo1 = busca_valor(&f1,&c1,&val1);
			
            tipo2 = busca_valor(&f2,&c2,&val2);
			
            tipor = busca_valor(&fr,&cr,&resu); /* se puede poner condicional */
			
            if (op == OPINTAC) { /* OPINTAC sin campo es CURSOR */
               cursor(pasa_a_int(val2.vald));
               if (f1 != F_INVALIDO) {
                  printcam(v_mpfi(f1),(c1 + 1));
                  fflush(stdout);
               }
               return(0);
            }
            
            if (fr == F_INVALIDO) return(0); /* no chuta */
            
			
            conv_var(tipor,tipo1,&val1);
            /**** especial funcion ASCII ****/
			
            if (op == '&') {
               conv_var(CHARTIPO,tipo2,&val2);
               i = q_tip(tipor);
            }
            else
               i = conv_var(tipor,tipo2,&val2);			
            
            if (tt == O_REV) c_reverse(&op);
            if (i == NUMERO) {
               sal = operanumero(val1,op,val2,&resu);
            }
            else
               
               if (i == LETRA || op == OPINTA)
                  sal = operaletra(val1,op,val2,&resu);
               else
                  sal = operafecha(val1,tipo1,op,val2,tipo2,&resu);
               
               if ( fr < 0 ) {
                  if (sal == 0) {
                     if (i == LETRA || i == ESFECHA) {
                        if (no_memo_interna(cr)) {
                           free(v_memo(cr)->alfabetica);
                           f1 = strlen(resu.valc)+1;
                           if (i == ESFECHA && f1 < 11) f1 = 11;
                           v_memo(cr)->alfabetica = strmalloc(f1);
                        }
                        strcpy(v_memo(cr)->alfabetica,resu.valc);
                     }
                     else
                        v_memo(cr)->numerica = resu.vald;
                  }
               }
               else {
                  if (sal == 0) st_camcd(tipor,cr,fr,&resu);
               }
               
               
               f++;
         }  while(oper[f-1].ficheror == -2);
         /*operaciones encadenadas (las iniciales siempre usan variables de pila)*/
         
         if (faclin == -2) {
            if (oper[f-1].ficheror == -1)
               return(oper[f-1].campor + 1);
         }
         return(0);
}

int comparanumero(val1,con,val2)
M_VAL val1;
char con;
M_VAL val2;
{
   switch (con) {
   case CIGUAL :
						if (val1.vald == val2.vald)
                     return(0);
                  else
                     return(-1);
   case CMAYOR :
						if (val1.vald > val2.vald)
                     return(0);
                  else
                     return(-1);
   case CMENOR :
						if (val1.vald < val2.vald)
                     return(0);
                  else
                     return(-1);
   case CMAIGU :
						if (val1.vald >= val2.vald)
                     return(0);
                  else
                     return(-1);
   case CMEIGU :
						if (val1.vald <= val2.vald)
                     return(0);
                  else
                     return(-1);
   case CDIFER :
						if (val1.vald != val2.vald)
                     return(0);
                  else
                     return(-1);
   default :
						return(-1);
   }
}

int comparaletra(val1,con,val2)
M_VAL val1;
char con;
M_VAL val2;
{
   switch (con) {
   case CIGUAL :
      if (strcmp(val1.valc,val2.valc) == 0)
         return(0);
      else
         return(-1);
   case CMAYOR :
      if (strcmp(val1.valc,val2.valc) > 0)
         return(0);
      else
         return(-1);
   case CMENOR :
      if (strcmp(val1.valc,val2.valc) < 0)
         return(0);
      else
         return(-1);
   case CMAIGU :
      if (strcmp(val1.valc,val2.valc) >= 0)
         return(0);
      else
         return(-1);
   case CMEIGU :
      if (strcmp(val1.valc,val2.valc) <= 0)
         return(0);
      else
         return(-1);
   case CDIFER :
      if (strcmp(val1.valc,val2.valc) != 0)
         return(0);
      else
         return(-1);
   default :
      return(-1);
   }
}


int comparafecha(val1,con,val2)
M_VAL val1;
char con;
M_VAL val2;
{
   switch (con) {
   case CIGUAL :
      if (cmfecha(val1.valc,val2.valc) == 0)
									return(0);
      else
									return(-1);
   case CMAYOR :
      if (cmfecha(val1.valc,val2.valc) > 0)
									return(0);
      else
									return(-1);
   case CMENOR :
      if (cmfecha(val1.valc,val2.valc) < 0)
									return(0);
      else
									return(-1);
   case CMAIGU :
      if (cmfecha(val1.valc,val2.valc) >= 0)
									return(0);
      else
									return(-1);
   case CMEIGU :
      if (cmfecha(val1.valc,val2.valc) <= 0)
									return(0);
      else
									return(-1);
   case CDIFER :
      if (cmfecha(val1.valc,val2.valc) != 0)
									return(0);
      else
									return(-1);
   default :
      return(-1);
   }
}


#ifdef FGV32
int examina_condicion(f,ipfi,pdata)
int f;
int ipfi;
char *pdata;
{
   int n = 0;
   M_VAL val1;
   M_VAL val2;
   short tipo1;
   short tipo2;
   int f1,c1,f2,c2,sig,salida;
   short tip;
   char con;
   f1 = v_prog(cond[f].fichero1);
   c1 = cond[f].campo1;
   f2 = v_prog(cond[f].fichero2);
   c2 = cond[f].campo2;
   sig = cond[f].sigue;
   tip = cond[f].tipo;
   con = cond[f].condicion;
   tipo1 = busca_valor(&f1,&c1,&val1);
   tipo2 = busca_valor(&f2,&c2,&val2);
   
   if (f1 == ipfi)
   {
      n += 2;
      *pdata = '#';
      pdata++;
      *pdata = (unsigned char)c1;
      pdata++;
   }
   else
   {
      n += (1+sizeof(M_VAL));
      *pdata = 'V';
      pdata++;
      memcpy(pdata,(char *)&val1,sizeof(M_VAL));
      pdata += sizeof(M_VAL);
   }
   n++;
   *pdata = con;
   pdata++;
   if (f2 == ipfi)
   {
      n += 2;
      *pdata = '#';
      pdata++;
      *pdata = (unsigned char)c2;
      pdata++;
   }
   else
   {
      n += (1+sizeof(M_VAL));
      *pdata = 'V';
      pdata++;
      memcpy(pdata,(char *)&val2,sizeof(M_VAL));
      pdata += sizeof(M_VAL);
   }
   
   *((short *)pdata) = tipo1;
   pdata += sizeof(short);
   n += sizeof(short);
   
   *((short *)pdata) = tipo2;
   pdata += sizeof(short);
   n += sizeof(short);
   
   *((short *)pdata) = tip;
   pdata += sizeof(short);
   n += sizeof(short);
   if (sig < 0)
   {
      *pdata = 0;
      pdata++;
      n++;
      return(n);
   }
   *pdata = 1;
   pdata++;
   n++;
   return(n+examina_condicion(sig,ipfi,pdata));
}
#endif


int condicion(f)
int f;
{
   int i;
   M_VAL val1;
   M_VAL val2;
   int tipo1;
   int tipo2;
   int f1,c1,f2,c2,sig,tip,salida;
   char con;
   f1 = v_prog(cond[f].fichero1);
   c1 = cond[f].campo1;
   f2 = v_prog(cond[f].fichero2);
   c2 = cond[f].campo2;
   sig = cond[f].sigue;
   tip = cond[f].tipo;
   con = cond[f].condicion;
   tipo1 = busca_valor(&f1,&c1,&val1);
   tipo2 = busca_valor(&f2,&c2,&val2);
   i = conv_var(tipo1,tipo2,&val2);
   if (i == LETRA)
      salida = comparaletra(val1,con,val2);
   else
      if (i == ESFECHA) {
         if (tipo1 == tipo2 || (tipo2 == CHARTIPO && strlen(val2.valc) > 9))
            salida = comparafecha(val1,con,val2);
         else
            salida = -1;
      }
      else
         salida = comparanumero(val1,con,val2);
      if ( (sig < 0) || (tip == 0 && salida == -1) || (tip == 1 && salida == 0))
         return(salida);
      else
         return(condicion(sig));
}

int memoria_alfa(p)
int p;
{
   int i;
   
   i = prog[p].indice;
   
   if (i < 0) {i = (-i)-1;i = pasa_a_int(v_memo(i)->numerica);}
   
   if (que_memo_tipo(i) == CHARTIPO)
      return(i);
   pito(126);pito(127);
   return(-1);
}


int rw_quitabt(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int f;
   if ((f = memoria_alfa(p)) > -1)
      quita_blancos(v_memo(f)->alfabetica);
   return(0);
}


int rw_leerel(p,tip,faclin)
int p;
int tip;
int faclin;
{
   short o,b,t,f;
   if (agisies == 1){
      if (agivez == 1 || agivez == 3  || agivez == 6){
         agivez++;
         iserrno = 0;
         sprintf(v_memo(5004)->alfabetica,"%d",iserrno);
         return (0);
      }
      if (agivez == 10){
         agivez = 0;   
         iserrno = 0;
         sprintf(v_memo(5004)->alfabetica,"%d",iserrno);
         return (0);
      }
      agivez++;
   }
   f = v_prog(prog[p].indice);
   o = prog[p+1].indice;
   b = prog[p+2].indice;
   if (b)
      b = ISLOCK;
   t = prog[p+3].indice;
   iserrno = 0;
   leer_reg(f,o,b,t);
   sprintf(v_memo(5004)->alfabetica,"%d",iserrno);
   n_np = 4;
   return(0);
}

int rw_grarel(p,tip,faclin)
int p;
int tip;
int faclin;
{
   short o,t,f;
   f = v_prog(prog[p].indice);
   o = prog[p+1].indice;
   t = prog[p+3].indice;
   grabar_reg(f,o,t);
   sprintf(v_memo(5004)->alfabetica,"%d",iserrno);
   n_np = 4;
   return(0);
}

int rw_camind(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int ipfi = prog[p].indice%100;
   cambia_ind(v_mpfi(v_prog(ipfi)),prog[p].indice/100);
   return(0);
}

int rw_clarel(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int i,o,b,t;
   
   
   i = v_prog(prog[p].indice % 100);
   o = prog[p].indice / 100;
   b = v_mpfirel(i)->numrela[o];
   t = v_mpfirel(i)->relacis[o];
   clave_rel(b,t,-1,0,i,0,0,0);
   return(0);
}

int rw_pinpan(p,tip,faclin)
int p;
int tip;
int faclin;
{
   
   pinppan(v_prog(prog[p].indice%100),prog[p].indice/100);
   return(0);
}


int fgv_printpa(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int ipfi = v_prog(prog[p].indice%100);
   int n = prog[p].indice/100;
   VENT *w;
   VENT *w2;
   int aa;
   char tmpx[512];
   char linea[512];
   unsigned char *v;
   int nl = 0,i,j,k;
   int ancho,alto;
   int negrita = 0;   
   int valor = pasa_a_int(v_memo(prog[p+1].indice)->numerica);
   int inicio = 0;


   if (!valor)
   {
       sprintf(v_memo(5004)->alfabetica,"%d",v_mpfi(ipfi)->f_pantalla[n].p_posfin);
       n_np = 2;
       return(0);
   }

   if (valor < 0)
   {
       inicio = -valor;
       valor = 9999;
   }
   

   w = w_ini(101,v_mpfi(ipfi)->f_pantalla[n].p_posfin);
   w2 = w_ini(101,v_mpfi(ipfi)->f_pantalla[n].p_posfin);   

   w_mueve(w,v_mpfi(ipfi)->f_pantalla[n].p_posref);
   w_mueve(w2,v_mpfi(ipfi)->f_pantalla[n].p_posref);
   w_saca(w2,stdpan);

   aa = pag_activa[ipfi];
   if (memoplin > 2) 
   {
      memcpy(tmpx,v_memo(5002)->alfabetica,250);
   }


   pinppan(ipfi,n);
   prin_pan(ipfi,n);

   w_saca(w,stdpan);

   v = w->vv;
   alto  = (w->vfin)/100 - (w->vini)/100 + 1;
   ancho = (w->vfin)%100 - (w->vini)%100 + 1;   
   for (i = 0;i < alto;i++)
   {
      k = 0;
      negrita = 0;
      for (j = 0;j< ancho;j++)
      {
          if ( (v[j+(w->size*2)] & ~128) == 'I')
          {
              if (!negrita)
              {
                  negrita = 1;
                  memcpy(linea+k,"{{N}}",5);
                  k += 5;
              }
          }
          else
          {
              if (negrita)
              {
                  negrita = 0;
                  memcpy(linea+k,"{{n}}",5);
                  k += 5;
              }
          }
          linea[k] = v[j];
          k++;
      }      
      if (negrita)
      {
          negrita = 0;
          memcpy(linea+k,"{{n}}",5);
          k += 5;
      }
      linea[k] = 0;
      if (inicio <= i)
      {
          printa_linea(linea);
          nl++;
      }
      v += ancho;      
      if (nl >= valor)
         break;
   }
   

   w_fin(w);
   w_pinta(w2);
   w_fin(w2);

   pag_activa[ipfi] = aa;
   if (memoplin > 2) 
   {
      memcpy(v_memo(5002)->alfabetica,tmpx,250);
   }  

   sprintf(v_memo(5004)->alfabetica,"%d",nl);
   n_np = 2;

   return(0);   
}

int rw_borrel(p,tip,faclin)
int p;
int tip;
int faclin;
{
   short o,t,f;
   f = v_prog(prog[p].indice);
   o = prog[p+1].indice;
   t = prog[p+3].indice;
   borrar_reg(f,o,t);
   sprintf(v_memo(5004)->alfabetica,"%d",iserrno);
   n_np = 4;
   return(0);
}

int agibegint(p,tip,faclin)
int p;
int tip;
int faclin;
{
   empieza_trans();
   sprintf(v_memo(5004)->alfabetica,"%d",iserrno);
   n_np = 1;
   return(0);
}

int agianulat(p,tip,faclin)
int p;
int tip;
int faclin;
{
   anula_trans();
   sprintf(v_memo(5004)->alfabetica,"%d",iserrno);
   n_np = 1;
   return(0);
}
int agibient(p,tip,faclin)
int p;
int tip;
int faclin;
{
   acaba_trans();
   sprintf(v_memo(5004)->alfabetica,"%d",iserrno);
   n_np = 1;
   return(0);
}

int rw_defecfi(p,tip,faclin)
int p;
int tip;
int faclin;
{
   
   defecto_ficha(v_mpfi(v_prog(prog[p].indice)));
   if (v_mpfi(v_prog(prog[p].indice))->f_nclaves < 1)
   {
	   hay_dat_permiso(v_mpfi(v_prog(prog[p].indice)),-3);
   }
   return(0);
}


int agi_pdefecfi(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int ini,fin;
   ini = pasa_a_int(v_memo(prog[p+1].indice)->numerica);
   fin = pasa_a_int(v_memo(prog[p+2].indice)->numerica);
   defecto_csficha(v_mpfi(v_prog(prog[p].indice)),ini,fin);
   n_np = 3;
   return(0);
}


int agi_cdefecfi(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int clave;
   clave = pasa_a_int(v_memo(prog[p+1].indice)->numerica);
   defecto_sficha(v_mpfi(v_prog(prog[p].indice)),clave);
   n_np=2;
   return(0);
}


int rw_dcamrel(p,tip,faclin)
int p;
int tip;
int faclin;
{
   defecam_rel(v_prog(prog[p].indice),-1,-1,0);
   return(0);
}

int rw_drelrel(p,tip,faclin)
int p;
int tip;                                
int faclin;
{
   deferel_rel(v_prog(prog[p].indice/100),v_prog(prog[p].indice%100),-1,0,ALTA,-1);
   return(0);
}



int rw_libind(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int ipfi = v_prog(prog[p].indice);
   libera_ind(v_mpfi(ipfi));
   return(0);
}

int rw_buclin(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int o,k= 0,b,i;
   
   b = prog[p].indice/10000;
   i = (prog[p].indice - (b*10000))/100;
   
   if (!i && prog[p+1].i == 255) {
      k = 1;
      i = prog[p+1].indice;
   }
   if (i > 0)
      i = en_pr[programa_activo].tabla_gosubs[i];
   b += 2;
   o = trata_lineas(v_prog(prog[p].indice%100),b,1,i,faclin);
   sprintf(v_memo(5004)->alfabetica,"%d",o);
   if (k)
      n_np = 2;
   
   return(0);
}

int rw_mensa(p,tip,faclin)
int p;
int tip;
int faclin;
{
   
   switch(prog[p].i) {
   case TMENS:
      cal_mensa(prog[p].indice,0);
      break;
   case TCONF:
      cal_mensa(prog[p].indice,1);
      break;
   default:
      cal_mensa(prog[p].indice,2);
   }
   
   return(0);
}

int rw_blin(p,tip,faclin)
int p;
int tip;
int faclin;
{
   
   Bl(pasa_a_int(v_memo(prog[p].indice)->numerica));
   return(0);
}

int rw_atri(p,tip,faclin)
int p;
int tip;
int faclin;
{
   atributo((char)prog[p].indice);
   return(0);
}

int rw_cuadro(p,tip,faclin)
int p;
int tip;
int faclin;
{
   
   cuadro(pasa_a_int((v_memo(prog[p].indice)->numerica / 10000)),
      pasa_a_int(((long)v_memo(prog[p].indice)->numerica % 10000)));
   return(0);
}


int rw_quitab(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int f;
   if ((f = memoria_alfa(p)) > -1)
      quitab(v_memo(f)->alfabetica);
   return(0);
}


int rw_for(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int i;
   
   
   nbuc++;
   i = prog[p].indice;
   bucle[nbuc] = i;
   if (bucl[i].operacion1 > -1) calcula(bucl[i].operacion1,O_NOR);
   if (bucl[i].condicion > -1 && condicion(bucl[i].condicion) == -1) {
      i = bucl[i].salto;
      n_np = i - p;
      nbuc--;
   }
   return(0);
}

int rw_cond(p,tip,faclin)
int p;
int tip;
int faclin;
{
   
   valcond = condicion(prog[p].indice);
   
   return(0);
}

int rw_goto(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int i,cc,pp,ii,ff;
   
   i = prog[p].indice;
   
   for (cc = nbuc;cc > -1;cc--) {
      ff = bucle[cc];
      pp = (bucl[ff].salto - 1);
      
      
      
      ii = prog[pp].indice;
      if (i >= bucl[ff].salto || i < ii)  {
         nbuc--; /* desactivar bucles si hace falta */
      }
      else
         break;
   }
   
   n_np = i - p;
   
   return(0);
}

int rw_sal(p,tip,faclin)
int p;
int tip;
int faclin;
{
   if (nbuc > -1) {
      n_np = bucl[ bucle[nbuc] ].salto - p;
      nbuc--;
   }
   
   return (0);
}


int rw_pincam(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int ff,cc,ii;
   
   
   ff = v_prog(prog[p].indice);
   cc = prog[p+1].indice;
   if (cc < 0) 
   {
      ii =  ( -cc ) - 1;
      cc = pasa_a_int(v_memo(ii)->numerica);
      if ( cc >= v_mpfi(ff)->f_ncampos ) 
      {
         rw_error(43);
         cc = 0;
      }
   }
   
   if (ver_activa(ff,cc + 1) == 0) 
   {
      pinta_campo(ff,cc+1,faclin,0);
      fflush(stdout);
   }
   
   n_np = 2;
   return(0);
}

int rw_gosub(p,tip,faclin)
int p;
int tip;
int faclin;
{
   if (agitei == 1)
   {
      if (agivez == 5  || agivez == 7  || agivez == 8)
      {
         agivez++;
         return(0);
      }
      if (agivez == 10)
      {
         agivez = 0;
         return(0);
      }
      agivez++;
   }
   
   if ( (p = prog[p].indice) >= 0 )
      p = en_pr[programa_activo].tabla_gosubs[p];
   p_ejecuta(p,tip,faclin,11);
   return(0);
}


int rw_goco(p,tip,faclin)
int p;
int tip;
int faclin;
{
   
   if (valcond)
      return(rw_goto(p,tip,faclin));
   
   return(0);
}

int rw_gobu(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int i;
   
   i = bucle[nbuc];
   if (bucl[i].operacion2 > -1 ) calcula(bucl[i].operacion2,O_NOR);
   if (bucl[i].condicion < 0 || condicion(bucl[i].condicion) == 0) {
      i = prog[p].indice;
      n_np = i - p;
      return(0);
   }
   nbuc--;
   
   return(0);
}

int rw_abrerte(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int ipfi = v_prog(prog[p].indice);
   
   iserrno = 0;
   if (!crea_ind(v_mpfi(ipfi)))
   {
      cierra_ind(v_mpfi(ipfi));
      v_mpfi(ipfi)->f_fd = -1;
      agichmod(v_mpfi(ipfi)->f_fichero,".*","","");
   }
   if (abre_ind(v_mpfi(ipfi),-1) >= 0) {
      f_s(ipfi,RABIERTO);
      f_n(ipfi,RCERRADO);
   }
   sprintf(v_memo(5004)->alfabetica,"%d",iserrno);
   return(0);
}


int rw_scroll(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int i,j;
   int par[3];
   for (j = 0;j < 3;j++,p++) {
      i = prog[p].indice;
      if (i < 0) {i = (-i)-1;i = pasa_a_int(v_memo(i)->numerica);}
      if (que_memo_tipo(i) != DOUBLETIPO)
         par[j] = atoi(v_memo(i)->alfabetica);
      else
         par[j] = pasa_a_int(v_memo(i)->numerica);
   }
   v_scroll(par[0],par[1],par[2]);
   n_np = 3;
   return(0);
}


extern long IxxNumeroPosicion;
int rw_dareg(p,tip,faclin)
int p;
int tip;
int faclin;
{
   short f,c,f0,c0;
   
   f = v_prog(prog[p++].indice);
   c = prog[p++].indice;
   
   comoes_cam(c,NULL,NULL,NULL,&f0,&c0,NULL);
   if (f0 == -1) {
      v_memo(c0)->numerica = (double)da_registo_actual(v_mpfi(f));
   }
   else {
      st_doble(c0,v_mpfi(f0),(double)da_registo_actual(v_mpfi(f)));
   }
   sprintf(v_memo(5004)->alfabetica,"%ld",IxxNumeroPosicion);
   
   n_np = 2;
   return(0);
}


int fgv_oemansi(p,tip,faclin)
int p;
int tip;
int faclin;
{
   short f0,c0,f1,c1;   
   char tmp[512];
   unsigned char *pp;
   comoes_cam(prog[p++].indice,NULL,NULL,NULL,&f0,&c0,NULL);
   comoes_cam(prog[p++].indice,NULL,NULL,NULL,&f1,&c1,NULL);
   if (f1 == -1) 
   {
	  strcpy(tmp,v_memo(c1)->alfabetica);      
   }
   else 
   {
      ld_campo(c1,v_mpfi(f1),tmp);
   }
   for (pp = (unsigned char *)tmp;*pp;pp++)
	  ansi_a_oem(pp);
   if (f0 == -1) 
   {
	  memo_strcpy(c0,tmp);      
   }
   else 
   {
      st_campo(c0,v_mpfi(f0),tmp);
   }

   n_np = 2;
   return(0);
}

int fgv_ansioem(p,tip,faclin)
int p;
int tip;
int faclin;
{
   short f0,c0,f1,c1;   
   char tmp[512];
   unsigned char *pp;
   comoes_cam(prog[p++].indice,NULL,NULL,NULL,&f0,&c0,NULL);
   comoes_cam(prog[p++].indice,NULL,NULL,NULL,&f1,&c1,NULL);
   if (f1 == -1) 
   {
	  strcpy(tmp,v_memo(c1)->alfabetica);      
   }
   else 
   {
      ld_campo(c1,v_mpfi(f1),tmp);
   }
   for (pp = (unsigned char *)tmp;*pp;pp++)
	  oem_a_ansi(pp);
   if (f0 == -1) 
   {
	  memo_strcpy(c0,tmp);      
   }
   else 
   {
      st_campo(c0,v_mpfi(f0),tmp);
   }
   n_np = 2;
   return(0);
}

int rw_ponreg(p,tip,faclin)
int p;
int tip;
int faclin;
{
   short f,c,f0,c0;
   
   f = v_prog(prog[p++].indice);
   c = prog[p++].indice;
   
   
   comoes_cam(c,NULL,NULL,NULL,&f0,&c0,NULL);
   if (f0 == -1) {
      pon_registro_actual(v_mpfi(f),(long)v_memo(c0)->numerica);
   }
   else {
      pon_registro_actual(v_mpfi(f),(long)ld_doble(c0,v_mpfi(f0)));
   }
   
   n_np = 2;
   return(0);
}

int rw_posiciona(p,tip,faclin)
int p;
int tip;
int faclin;
{
   short i,f0,f,c,r,pp;
   
   f0 = prog[p++].indice;
   if (f0 < 0)
   {  
      n_np = 2;
      f0 =  -(f0) - 1;
      c = pasa_a_int(v_memo(f0)->numerica);
      f = v_prog(prog[p++].indice);
      if (c < 0 || c >= v_mpfi(f)->f_ncampos)
      {
         rw_error(43);
         return(0);
      }
   }
   else
   {
      comoes_cam(f0,NULL,NULL,NULL,&f,&c,NULL);
      n_np = 1;
   }
   
   pp = v_mpfi(f)->f_campos[c].c_mante.c_qp;
   i = esta_en_el_raton(pp,NULL);
   
   if (i >= 0)
   {
      if ( (i%100)  == f )
      {
         r = v_mpfi(f)->f_campos[(i/100)-1].c_mante.c_actual % 1000;
         if (r != c)
            i = -3;
      }
      else
         i = -2;
   }
   if (i < 0)
   {
      sprintf(v_memo(5004)->alfabetica,"%d",i);
      return(0);
   }
   requiere_reposicionar = 1;
   _ID_Cursor = i;
   _M_Cursor = p;
   strcpy(v_memo(5004)->alfabetica,"0");
   return(0);
}



int rw_campo(p,tip,faclin)
int p;
int tip;
int faclin;
{
   short f0,modo,c0,f,c,r;
   unsigned short i;
   char tmp[256];
   
   n_np = 3;
   i    = prog[p].i;
   f0   = prog[p++].indice;
   if (f0 < 0)
   {
      n_np = 4;
      f0 =  -(f0) - 1;
      c = pasa_a_int(v_memo(f0)->numerica);
      f = v_prog(prog[p++].indice);
      if (c < 0 || c >= v_mpfi(f)->f_ncampos)
      {
         rw_error(43);
         return(0);
      }
   }
   else
      comoes_cam(f0,NULL,NULL,NULL,&f,&c,NULL);
   modo = prog[p++].indice;
   c0   = prog[p++].indice;
   
   
   tmp[0] = 0;
   tmp[1] = 0;
   if (i >= 36 && i <= 42) 
   {
      tmp[0] = v_memo(c0)->alfabetica[0];
      if (tmp[0] == 'S' || tmp[0] == 's')
         r = 1;
      else
         r = 0;
      if (i == 36 || i == 37) {
         if (r) r = 0;else r = 1;
      }
   }
   else
      if (i != 139 && i != 235)
         r = pasa_a_int(v_memo(c0)->numerica);
      switch(i) {
      case 36:
         if (modo)
            v_mpfi(f)->f_campos[c].c_mante.c_modif = r;
         else
            r = v_mpfi(f)->f_campos[c].c_mante.c_modif;
         break;
      case 37:
         if (modo)
            v_mpfi(f)->f_campos[c].c_mante.c_visual = r;
         else
            r = v_mpfi(f)->f_campos[c].c_mante.c_visual;
         break;
      case 42:
         if (modo)
         {
             v_mpfi(f)->f_campos[c].c_mante.c_lineal = (r & C_LINEAL) | (v_mpfi(f)->f_campos[c].c_mante.c_lineal & ~C_LINEAL);
         }
         else
            r = (v_mpfi(f)->f_campos[c].c_mante.c_lineal & C_LINEAL);
         break;
      case 43:
         if (modo)
            v_mpfi(f)->f_campos[c].c_mante.c_qp = r;
         else
		 {
            r = v_mpfi(f)->f_campos[c].c_mante.c_qp;
			sprintf(v_memo(5004)->alfabetica,"%d",faclin);
		 }
         break;
      case 44:
         if (modo)
            v_mpfi(f)->f_campos[c].c_mante.c_qpl = r;
         else
            r = v_mpfi(f)->f_campos[c].c_mante.c_qpl;
         break;
      case 139:     
         if (modo)
         {
           free(v_mpfi(f)->f_campos[c].c_minimo);
           v_mpfi(f)->f_campos[c].c_minimo = malloc(strlen(v_memo(c0)->alfabetica)+1);
           strcpy(v_mpfi(f)->f_campos[c].c_minimo,v_memo(c0)->alfabetica);
            if (v_mpfi(f)->f_campos[c].c_tipo == DOUBLETIPO)
            {
               calcula_usi(v_mpfi(f),c);
            }
           strcpy(tmp,v_mpfi(f)->f_campos[c].c_minimo);  
            
         }
         else
            strcpy(tmp,v_mpfi(f)->f_campos[c].c_minimo);  
         break;
      case 235:
         if (modo)
         {
           free(v_mpfi(f)->f_campos[c].c_maximo);
           v_mpfi(f)->f_campos[c].c_maximo = malloc(strlen(v_memo(c0)->alfabetica)+1);
           strcpy(v_mpfi(f)->f_campos[c].c_maximo,v_memo(c0)->alfabetica);
            if (v_mpfi(f)->f_campos[c].c_tipo == DOUBLETIPO)
            {
               calcula_usi(v_mpfi(f),c);
            }
           strcpy(tmp,v_mpfi(f)->f_campos[c].c_maximo);  
         }
         else
            strcpy(tmp,v_mpfi(f)->f_campos[c].c_maximo);  
         break;
      default:
         break;
      }
      if ((i >= 36 && i <= 42) || i == 139 || i == 235) 
      {
         if (i == 36 || i == 37) 
         {
            if (r)
               tmp[0] = 'N';
            else
               tmp[0] = 'S';
         }
         else
            if (i != 139 && i != 235)
            {
               if (r)
                  tmp[0] = 'S';
               else
                  tmp[0] = 'N';
            }
            if (no_memo_interna(c0)) {
               free(v_memo(c0)->alfabetica);
               v_memo(c0)->alfabetica = strmalloc(strlen(tmp)+2);
            }
            strcpy(v_memo(c0)->alfabetica,tmp);
      }
      else
         v_memo(c0)->numerica = (double)r;
      
      return(0);
}



int rw_mete_buffer(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int mb,mn,md,b,n,r,l,tipo;
   char *d;
   
   mb  = prog[p++].indice; /* buffer */
   mn  = prog[p++].indice; /* indice */
   md  = prog[p++].indice; /* dato (var) */
   b   = pasa_a_int(v_memo(mb)->numerica);
   n   = pasa_a_int(v_memo(mn)->numerica);
   
   if (md < 0) {
      md = (-md) - 1;
      md = pasa_a_int(v_memo(md)->numerica);
   }
   tipo = que_memo_tipo(md);
   if (tipo == DOUBLETIPO) {
      d = (char *)&(v_memo(md)->numerica);
      l = sizeof(double);
   }
   else {
      d = v_memo(md)->alfabetica;
      l = strlen(d) + 1;
   }
   r = trata_el_buffer(b,n,d,l,0,tipo);
   
   sprintf(v_memo(5004)->alfabetica,"%d",r);
   n_np = 3;
   return(0);
}

int rw_saca_buffer(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int mb,mn,md,b,n,r,l,tipo;
   char *d,tmp[258];
   
   mb  = prog[p++].indice; /* buffer */
   mn  = prog[p++].indice; /* indice */
   md  = prog[p++].indice; /* dato (var) */
   b   = pasa_a_int(v_memo(mb)->numerica);
   n   = pasa_a_int(v_memo(mn)->numerica);
   
   if (md < 0) {
      md = (-md) - 1;
      md = pasa_a_int(v_memo(md)->numerica);
			}
   
   d = v_memo(md)->alfabetica;
   tmp[0] = 0;
   tipo = que_memo_tipo(md);
   switch(tipo) {
			case DOUBLETIPO:
            d = (char *)&(v_memo(md)->numerica);
            r = trata_el_buffer(b,n,d,sizeof(double),1,tipo);
            break;
         case FECHATIPO:
            r = trata_el_buffer(b,n,d,11,1,tipo);
            break;
         default:
            r = trata_el_buffer(b,n,tmp,256,2,tipo);
            l = strlen(tmp) + 1;
            if (no_memo_interna(md)) {
               free(v_memo(md)->alfabetica);
               v_memo(md)->alfabetica = d = strmalloc(l);
            }
            strcpy(d,tmp);
            break;
			}
   
   sprintf(v_memo(5004)->alfabetica,"%d",r);
   n_np = 3;
   return(0);
}


int rw_color(p,tip,faclin)
int p;
int tip;
int faclin;
{
			int c = pasa_a_int(v_memo( prog[p++].indice )->numerica); /* caracter */
         int f = pasa_a_int(v_memo( prog[p++].indice )->numerica); /* fondo */
         Color(c,f);
         n_np = 2;
         return(0);
}

int rw_tecla(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int c = prog[p++].indice;
   unsigned int i = (unsigned int)leecar();
   char tmp[81];
   
   sprintf(tmp,((i > 255) ? "\377%d" : "%c"),i);
   memo_strcpy(c,tmp);
   return(0);
}


int rw_cua1(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int m  = prog[p].i - 60;
   int p1 = pasa_a_int(v_memo( prog[p++].indice )->numerica);
   int p2 = pasa_a_int(v_memo( prog[p++].indice )->numerica);
   bi_cuadro(p1,p2,m);
   n_np = 2;
   return(0);
}


char *mi_remoto_dirbass()
{
#ifdef RWSERVER
	   extern char remoto_dirbass[51];
	   if (en_conexion_externa)
	   {
		 return remoto_dirbass;
	   }
#endif
	   return dirbass;
}

int externo_copiafich(int externo,char *or,char *des)
{
   char tmp1[512];
   char tmp2[512];
#ifdef RWSERVER
   if (externo && en_conexion_externa)
   {
      int sal = 0;
      char tmp[601];
      strcpy(tmp,or);
      strcpy(tmp+300,des);
      funcion_windows_cliente(19,tmp,600);
      memcpy_int(&sal,tmp,sizeof(int));     
      
      return(sal);
   }
#endif   
    return copiarfich(cambiar_el_fich(or,tmp1),cambiar_el_fich(des,tmp2),0);
}


int externo_enviafich(int externo,char *or,char *des)
{
#ifdef RWSERVER
   if (externo && en_conexion_externa)
   {
      char tmp[601];
	  int sal;

	  cambiar_el_fich(or,tmp);      
      sal = strlen(tmp);
      tmp[sal] = 1;
      strcpy(tmp+sal+1,des);
      sal = rw_envia_fichero(tmp,777);      

	  return sal;
   }
#endif   
   return externo_copiafich(0,or,des);
}


int agi_copiafich(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int o,d;
   int externo = 0;
   
   if (prog[p].i == 255)
   {
      externo = 1;
   }
   
   o = prog[p].indice;
   p++;
   d = prog[p].indice;

   
   sprintf(v_memo(5004)->alfabetica,"%d",externo_copiafich(externo,v_memo( o )->alfabetica,v_memo( d )->alfabetica));


   n_np = 2;
   return(0);
}


int agi_renombra(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int o,d;
   int externo = 0;
   char tmp1[512];
   char tmp2[512];
   
   if (prog[p].i == 255)
   {
      externo = 1;
   }
   
   o = prog[p].indice;
   p++;
   d = prog[p].indice;
#ifdef RWSERVER
   if (externo && en_conexion_externa)
   {
      int sal = 0;
      char tmp[601];
      strcpy(tmp,v_memo( o )->alfabetica);
      strcpy(tmp+300,v_memo( d )->alfabetica);
      funcion_windows_cliente(20,tmp,600);
      memcpy_int(&sal,tmp,sizeof(int));
      sprintf(v_memo(5004)->alfabetica,"%d",sal);
      n_np = 2;
      return(0);
   }
#endif
   sprintf(v_memo(5004)->alfabetica,"%d",
      rw_base_rename(cambiar_el_fich(v_memo( o )->alfabetica,tmp1),cambiar_el_fich(v_memo( d )->alfabetica,tmp2)));
   n_np = 2;
   return(0);
}

int agi_afegirfich(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int o,d;
   int externo = 0;
   char tmp1[512];
   char tmp2[512];
   
   if (prog[p].i == 255)
   {
      externo = 1;
   }
   
   o = prog[p].indice;
   p++;
   d = prog[p].indice;
#ifdef RWSERVER
   if (externo && en_conexion_externa)
   {
      int sal = 0;
      char tmp[601];
      strcpy(tmp,v_memo( o )->alfabetica);
      strcpy(tmp+300,v_memo( d )->alfabetica);
      funcion_windows_cliente(21,tmp,600);
      memcpy_int(&sal,tmp,sizeof(int));
      sprintf(v_memo(5004)->alfabetica,"%d",sal);
      n_np = 2;
      return(0);
   }
#endif
   sprintf(v_memo(5004)->alfabetica,"%d",
      copiarfich(cambiar_el_fich(v_memo( o )->alfabetica,tmp1),cambiar_el_fich(v_memo( d )->alfabetica,tmp2),1));
   n_np = 2;
   return(0);
}

int agi_mkdir(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int o = prog[p].indice;
   int externo = 0;
   char tmp1[512];
   
   if (prog[p].i == 255)
   {
      externo = 1;
   }
#ifdef RWSERVER
   if (externo && en_conexion_externa)
   {
      int sal = 0;
      char tmp[301];
      strcpy(tmp,v_memo( o )->alfabetica);     
      funcion_windows_cliente(22,tmp,strlen(tmp)+1);
      memcpy_int(&sal,tmp,sizeof(int));
      sprintf(v_memo(5004)->alfabetica,"%d",sal);
      n_np = 1;
      return(0);
   }
#endif
   
   sprintf(v_memo(5004)->alfabetica,"%d",crea_dir_total(cambiar_el_fich(v_memo( o )->alfabetica,tmp1)));
   
   n_np = 1;
   return(0);
}



#ifdef ESXENIX
static struct direct *_buf;
#else
static struct dirent *_buf;
#endif
static DIR *dd = NULL;
static char filtro[81];
static char **_dir_de_base = NULL;
static char **pdir_de_base = NULL;

int fgv_i_pdir(char *tmp)
{
   int sal = 0;
   char *name = NULL;
   char tmp2[256];

   libera_dir(_dir_de_base);
   _dir_de_base = NULL;
   pdir_de_base = NULL;

   if (!strcmp(tmp,"-:1"))
   {
       strcpy(tmp,rw_directorio(""));
       return(sal);
   }
   else
   if (strlen(tmp) > 4 && !memcmp(tmp,"-:X:",4))
   {
       char *pp = NULL;
       if ((pp = getenv(tmp+4)))
       {
           strcpy(tmp,pp);
       }
       else
       {
           tmp[0] = 0;
       }
       return(sal);
   }
   
   strcpy(filtro,quita_path(tmp));
   if (quita_path(tmp) == tmp)
   {
      tmp[0] = '.';
      tmp[1] = 0;
   }
   else
   {
      *(quita_path(tmp)-1) = 0;
   }
   
   name = NULL;
   dd = opendir(tmp);
   strcat(tmp,"/");
   if (dd)
   {	  
      do 
	  {		 
		 if (pdir_de_base && *pdir_de_base)
		 {
            if (coincide_(*pdir_de_base,filtro))
			{
			   name = *pdir_de_base;               
			}
			pdir_de_base++;
			if (name)
				break;
		 }
		 else if (!name)
		 {
			_buf = readdir(dd);		 
			 if (_buf)
			 {		
#ifdef DSDB_ACTIVA				 
				if (coincide_(_buf->d_name,"*.dsb"))
				{
					strcpy(tmp2,tmp);
					strcpy(quita_path(tmp2),_buf->d_name);					
					if (dsdb_coincide_filtro(tmp2))
					{
						libera_dir(_dir_de_base);						
						_dir_de_base = dsdb_lee_direct(tmp2,0);
						pdir_de_base = _dir_de_base;
					}
				}
#endif
				if (coincide_(_buf->d_name,filtro))
				{
				   name = _buf->d_name;
				   break;
				}
			 }
			 else
				break;
		 }
		 else
			 break;
      } while(-1);
      if (name)
      {
         strcpy(quita_path(tmp),name);
         sal = 0;
      }
      else
      {
         sal = -1;      
		 libera_dir(_dir_de_base);
		 _dir_de_base = NULL;
		 pdir_de_base = NULL;
         closedir(dd);
         dd = NULL;
      }
   }
   else
      sal = -1;
   
   return(sal);
}

int agi_pdir(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int o = prog[p].indice;
   char tmp[512];
   int sal;
   int externo = 0;
   
   strcpy(tmp,v_memo( o )->alfabetica);
   
#ifdef RWSERVER
   if (prog[p].i == 255 && en_conexion_externa)
   {
      funcion_windows_cliente(17,tmp,strlen(tmp)+1);
      memcpy_int(&sal,tmp,sizeof(int));
      strcpy(tmp,tmp+sizeof(int));
      externo = 1;
   }
#endif
   
   if (!externo)
      sal = fgv_i_pdir(cambiar_el_fich(tmp,NULL));
   
   if (sal >= 0)
   {
      if (no_memo_interna(o))
      {
         free(v_memo( o )->alfabetica);
         v_memo( o )->alfabetica = strmalloc(strlen(tmp)+1);
      }
      strcpy(v_memo( o )->alfabetica,tmp);   
   }
   
   sprintf(v_memo(5004)->alfabetica,"%d",sal);
   n_np = 1;
   return(0);
}

int fgv_i_sdir(char *tmp)
{
   int sal = 0;
   char *name = NULL;
   char tmp2[256];
   
   if (dd)
   {
      do {
		 if (pdir_de_base && *pdir_de_base)
		 {
            if (coincide_(*pdir_de_base,filtro))
			{
			   name = *pdir_de_base;               
			}
			pdir_de_base++;
			if (name)
				break;
		 }
		 else if (!name)
		 {
			 _buf = readdir(dd);
			 if (_buf)
			 {
#ifdef DSDB_ACTIVA
				if (coincide_(_buf->d_name,"*.dsb"))
				{
					strcpy(tmp2,tmp);
					strcpy(quita_path(tmp2),_buf->d_name);					
					if (dsdb_coincide_filtro(tmp2))
					{
						libera_dir(_dir_de_base);						
						_dir_de_base = dsdb_lee_direct(tmp2,0);
						pdir_de_base = _dir_de_base;
					}
				}
#endif
				if (coincide_(_buf->d_name,filtro))
				{
				   name = _buf->d_name;
				   break;
				}
			 }
			 else
				break;
		 }
		 else
			 break;
      } while(-1);
      if (name)
      {	  
         strcpy(quita_path(tmp),name);
         sal = 0;      
      }
      else
      {
         sal = -1;      
		 libera_dir(_dir_de_base);
		 _dir_de_base = NULL;
		 pdir_de_base = NULL;
         closedir(dd);
         dd = NULL;
      }
   }
   else
      sal = -1;
   return(sal);
}

int agi_sdir(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int o = prog[p].indice;
   int sal;
   char tmp[500];
   int externo = 0;
   
   strcpy(tmp,v_memo( o )->alfabetica);
   
#ifdef RWSERVER
   if (prog[p].i == 255 && en_conexion_externa)
   {
      funcion_windows_cliente(18,tmp,strlen(tmp)+1);
      memcpy_int(&sal,tmp,sizeof(int));
      strcpy(tmp,tmp+sizeof(int));
      externo = 1;
   }
#endif
   
   if (!externo)
      sal = fgv_i_sdir(cambiar_el_fich(tmp,NULL));
   
   if (sal >= 0)
   {
      if (no_memo_interna(o))
      {
         free(v_memo( o )->alfabetica);
         v_memo( o )->alfabetica = strmalloc(strlen(tmp)+1);
      }
      strcpy(v_memo( o )->alfabetica,tmp);   
   }
   
   
   sprintf(v_memo(5004)->alfabetica,"%d",sal);
   n_np = 1;
   return(0);
}

int agi_rmdir(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int o = prog[p].indice;
   int externo = 0;
   char tmp1[512];
   
   if (prog[p].i == 255)
   {
      externo = 1;
   }
#ifdef RWSERVER
   if (externo && en_conexion_externa)
   {
      int sal = 0;
      char tmp[301];
      strcpy(tmp,v_memo( o )->alfabetica);     
      funcion_windows_cliente(23,tmp,strlen(tmp)+1);
      memcpy(&sal,tmp,sizeof(int));
      sprintf(v_memo(5004)->alfabetica,"%d",sal);
      n_np = 1;
      return(0);
   }
#endif
   
   sprintf(v_memo(5004)->alfabetica,"%d",rmdir(cambiar_el_fich(v_memo( o )->alfabetica,tmp1)));
   n_np = 1;
   return(0);
}



int agi_deltree(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int o = prog[p].indice;
   int externo = 0;
   char tmp1[512];
   
   if (prog[p].i == 255)
   {
      externo = 1;
   }
#ifdef RWSERVER
   if (externo && en_conexion_externa)
   {
      int sal = 0;
      char tmp[301];
      strcpy(tmp,v_memo( o )->alfabetica);     
      funcion_windows_cliente(24,tmp,strlen(tmp)+1);
      memcpy_int(&sal,tmp,sizeof(int));
      sprintf(v_memo(5004)->alfabetica,"%d",sal);
      n_np = 1;
      return(0);
   }
#endif
   sprintf(v_memo(5004)->alfabetica,"%d",lee_y_borra(cambiar_el_fich(v_memo( o )->alfabetica,tmp1)));
   n_np = 1;
   return(0);
}

int agi_versionrtme(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int o = prog[p].indice;
   char tmp[81];
   sprintf(tmp,"%d.%02d%c-%d",rtme_version1,rtme_version2,rtme_versionletra,numero_build);
   if (no_memo_interna(o))
   {
      free(v_memo(o)->alfabetica);
      v_memo(o)->alfabetica = strmalloc(strlen(tmp)+1);
   }
   strcpy(v_memo(o)->alfabetica,tmp);
   sprintf(v_memo(5004)->alfabetica,"%d",fgv_internal_build);
   n_np = 1;
   return(0);
}

int agi_externoexe(p,tip,faclin)
int p;
int tip;
int faclin;
{
   strcpy(v_memo(5004)->alfabetica,"-1");
   n_np = 1;
   return(0);
}

int rw_campoatri(p,tip,faclin)
int p;
int tip;
int faclin;
{
   short f0;
   short a;
   short cc;
   short cf;
   short f,c;
   short ff;
   
   n_np = 4;
   f0   = prog[p++].indice;
   if (f0 < 0)
   {
      n_np = 5;
      f0 =  -(f0) - 1;
      c = pasa_a_int(v_memo(f0)->numerica);
      f = v_prog(prog[p++].indice);
      if (c < 0 || c >= v_mpfi(f)->f_ncampos)
      {
         rw_error(43);
         return(0);
      }
   }
   else
      comoes_cam(f0,NULL,NULL,NULL,&f,&c,NULL);
   
   a    = prog[p++].indice;
   cc   = pasa_a_int(v_memo( prog[p++].indice )->numerica);
   cf   = pasa_a_int(v_memo( prog[p++].indice )->numerica);
   
   *(((char *)&(ff)) + 1) = a;
   *(((char *)&(ff))) = (unsigned char)(((cf << 4) & 0xf0) | cc);
   v_mpfi(f)->f_campos[c].c_fflag = ff;
   return(0);
}

int agi_modowin(p,tip,faclin)
int p;
int tip;
int faclin;
{
   extern int win_fondo_neutro;
   /*win_fondo_neutro = 2;*/
   return(0);
}

int rw_graba_bmp_pantalla(p,tip,faclin)
int p;
int tip;
int faclin;
{
   sprintf(v_memo(5004)->alfabetica,"%d",graba_bmp_pantalla(v_memo( prog[p].indice )->alfabetica));
   n_np = 1;
   return(0);
}


/*************************************************************/

static kaka_printpan(pfi,numpan)
FI_DEF *pfi;
int numpan;
{
   int numcam;
   int campini,campfin;
   int visual,campo;
   int salvaa;
   
   
   campini = pfi->f_pantalla[numpan].p_campoini;
   campfin = pfi->f_pantalla[numpan].p_campofin;
   
   
   if (rentra_sal_cc != rentra_sal_cf)
      Color(rentra_sal_cc,rentra_sal_cf);
   atributo(rentra_sal_atrib);
   
   
   for (numcam=campini; numcam<=campfin;numcam++) 
   {
      campo = pfi->f_campos[numcam-1].c_mante.c_actual % 1000;
      visual = pfi->f_campos[campo].c_mante.c_visual;
      if (visual == 0) 
      {
         salvaa = pfi->f_campos[campo].c_fflag;
         a_atri_campo(pfi,campo,rentra_sal_atrib,0,rentra_sal_cc,rentra_sal_cf);
         printusi(pfi,campo+1);
         a_atri_campo(pfi,campo,rentra_sal_atrib,1,rentra_sal_cc,rentra_sal_cf);                 
         pfi->f_campos[campo].c_fflag = salvaa;
      }
   }
   atributo(tolower(rentra_sal_atrib));
   fflush(stdout);
   return(0);
}



static int captura_pantallas_defs(fichero,empresa,destino,version)
char *fichero;
char *empresa;
char *destino;
char *version;
{
   VENT *w;
   int err = 0,i,ii,r,n,m,o;
   FILE *file;
   char tmp[270];
   char tmp2[270];
   char menu[256];
   char sele[256];
   FI_DEF *_pfi;
   
   file = fopen(fichero,"rt");
   if (!file)
   {
      aviso(0,__Texto[42]);
      return(-1);
   }
   
   
   while(lee_linea(tmp,255,file))
   {   
      menu[0] = 0;
      sele[0] = 0;
      quita_blancos(tmp);
      quitan(tmp);
      for (i = 0;tmp[i] && tmp[i] != ',';i++);
      if (tmp[i] == ',')
      {
         tmp[i] = 0;
         strcpy(menu,tmp+i+1);
         for (i = 0;menu[i] && menu[i] != ',';i++);
         if (menu[i] == ',')
         {
            menu[i] = 0;
            strcpy(sele,menu+i+1);
         }
      }
      strminus(quita_path(tmp));
      i = strlen(tmp);
      if (i > 4)
      {
         i -= 4;
         if (!strcmp(tmp+i,".mas"))
         {
            tmp[i] = 0;
         }
      }
      r = lee_defn(tmp,&_pfi);
      if (!r)
      {
         defecto_ficha(_pfi);
         cls();
         /*cabecera_v(version,"",_pfi->f_titulo,empresa);	   */
		 cabecera("",_pfi->f_titulo,empresa);
         
         if (menu[0])
         {
			libera();
            if (leemenu(menu) >= 0)
            {
               m = 0;
               o = 0;
               for (ii = 0;sele[ii];ii++)
               {
                  if (m >= nmenus) {o = 0;break;}
                  if (sele[ii] < 'A' || sele[ii] > 'Z') {o = 0;break;}
                  o = sele[ii] -'A';
                  if (o < 0 || o >= wmenu[m]->m_nop)
                  {
                     o = 0;break;
                  }
                  if (!sele[ii])
                     o++;
                  else
                  {
                     n = wmenu[m]->m_op[o]->o_menuhijo;
                     if (n == -1)
                     {
                        o++;break;
                     }
                     else
                     {
                        m = wmenu[m]->m_op[o]->o_menuhijo;
                     }
                  }
               }
               if (!o) continue;   
               wdef	= m * 100 + o;
               snap_shoot_menu = "";
               o = w_menus(&m);			 
            }
         }
         
         
         for (i = 0;i < _pfi->f_npanta;i++)
         {
            strcpy(tmp2,tmp);
            *quita_path(tmp2) = 0;
            n = strlen(tmp2);
            if (n > 4)
            {
               if (!memcmp(tmp2+n-4,"def/",4))
               {
                  memcpy(tmp2+n-4,"pan/",4);
               }
            }
            strcat(tmp2,quita_path(_pfi->f_pantalla[i].p_nombrepan));	   
            
            w = w_ini(101,_pfi->f_pantalla[i].p_posfin);       
            w_lee(w,tmp2);
            w_mueve(w,_pfi->f_pantalla[i].p_posref);
            if (i)
               w_metev(stdpan,w);
            else
               w_mete(stdpan,w);
            w_refresca(w);
            w_fin(w);
            kaka_printpan(_pfi,i);
            strcpy(tmp2,destino);
            if (strlen(destino) && destino[strlen(destino)-1] != '/' && destino[strlen(destino)-1] != '\\')
            {
               strcat(tmp2,"/");
            }
            strcat(tmp2,quita_path(tmp));
            sprintf(tmp2+strlen(tmp2),".%d.bmp",i+1);
            graba_bmp_pantalla(tmp2);
         }
         free_defpri(_pfi);
		 libera();
      }
      else
      {
         pito(128);
         pprint(2401,tmp);
      }
   }
   
   return(err);
}


int rw_graba_defs_pantallas(p,tip,faclin)
int p;
int tip;
int faclin;
{
   char *p1,*p2,*p3;
   p1 = v_memo( prog[p].indice )->alfabetica;
   p++;
   p2 = v_memo( prog[p].indice )->alfabetica;
   p++;
   p3 = v_memo( prog[p].indice )->alfabetica;
   sprintf(v_memo(5004)->alfabetica,"%d",captura_pantallas_defs(p2,p1,p3,rw_version));
   n_np = 3;
   return(0);
}



static int menu_captura_pantallas(fichero,menu,destino,version)
char *fichero;
char *menu;
char *destino;
char *version;
{
   VENT *w;
   int err = 0,i,r,n,o,m;
   FILE *file;
   char tmp[270];
   char tmp2[270];
   
   file = fopen(fichero,"rt");
   if (!file)
			{
      aviso(0,__Texto[43]);
      return(-1);
			}
   if (leemenu(menu) < 0)
			{
      fclose(file);
      aviso(0,__Texto[44]);
      return(-1);
			}
   while(lee_linea(tmp,255,file))
			{   
      quita_blancos(tmp);
      quitan(tmp);
      m = 0;
      o = 0;
      cls();
      /*cabecera_v(version,"",nommenu,"");*/
	  cabecera("",nommenu,"");
      for (i = 0;tmp[i];i++)
      {
         if (m >= nmenus) {o = 0;break;}
         if (tmp[i] < 'A' || tmp[i] > 'Z') {o = 0;break;}
         o = tmp[i] -'A';
         if (o < 0 || o >= wmenu[m]->m_nop)
         {
            o = 0;break;
         }
         if (!tmp[i])
            o++;
         else
         {
            n = wmenu[m]->m_op[o]->o_menuhijo;
            if (n == -1)
            {
               o++;break;
            }
            else
            {
               m = wmenu[m]->m_op[o]->o_menuhijo;
            }
         }
      }
      if (!o) continue;   
      wdef	= m * 100 + o;
      strcpy(tmp2,destino);
      if (strlen(destino) && destino[strlen(destino)-1] != '/' && destino[strlen(destino)-1] != '\\')
      {
         strcat(tmp2,"/");
      }
      strcat(tmp2,quita_path(menu));
      sprintf(tmp2+strlen(tmp2),".%s.bmp",tmp);
      snap_shoot_menu = tmp2;
      o = w_menus(&m);
			}
   libera();
   return(0);
}

int rw_graba_menu_pantallas(p,tip,faclin)
int p;
int tip;
int faclin;
{
			char *p1,*p2,*p3;
         
         p1 = v_memo( prog[p].indice )->alfabetica;
         p++;
         p2 = v_memo( prog[p].indice )->alfabetica;
         p++;
         p3 = v_memo( prog[p].indice )->alfabetica;
         sprintf(v_memo(5004)->alfabetica,"%d",menu_captura_pantallas(p2,p1,p3,rw_version));
         n_np = 3;
         return(0);
         
}

/*************************************************************/

int activa_programa(p)
int p;
{
   int i;
   char tmp[256];
   if (p < 0 || p > nen_pr) {
	   sprintf(tmp,__Texto[277],p,nen_pr);aviso(0,tmp);;return(-1);
   }
   
   if (en_pr[p].activo)
   {
      nprog = en_pr[p].e_nprog;
      ncond = en_pr[p].e_ncond;
      nbucl = en_pr[p].e_nbucl;
      noper = en_pr[p].e_noper;
      nmemo = en_pr[p].e_nmemo;
      nf_B = en_pr[p].e_nf_B;
      prog = en_pr[p].e_prog;
      cond = en_pr[p].e_cond;
      bucl = en_pr[p].e_bucl;
      oper = en_pr[p].e_oper;
      memo = en_pr[p].e_memo;
      f_B  = en_pr[p].e_f_B;
   }
   else
   {
      programa_activo = -1;
      ultimo_ipfi = -1;
      if (lee_prog(p))
	  {		  
         sprintf(tmp,__Texto[278],en_pr[p].prc);
		 aviso(0,tmp);
         return(-1);
	  }
   }
   programa_activo = p;
   
   if (mimodo_debug)
   {
      fin_traza();
      ini_traza(p);
   }
   
   return(0);
}


/* "DAME_AGI_CLAVES"  ,"98",195,NULL, */

int rw_daclaves(p,tip,faclin)
int p;
int tip;
int faclin;
{
   char tmp[128];
   unsigned char *buffer = NULL;
   long ll;
   int file;
   int recibido = 0;
   unsigned long numero = (unsigned long)(v_memo( prog[p].indice )->numerica);
   
   buffer = malloc(8010);
   if (!buffer)
   {
						aviso(0,__Texto[45]);
                  n_np = 1;
                  return(0);
   }
   
   sprintf(tmp,"c:/francesc/registro/%06ld.ini",numero);
   file = rw_open_(tmp,O_RDONLY | O_BINARY);
   if (file > -1)
   {
						ll = rw_lseek_(file,0L,SEEK_END);
   }
   else
   {
						ll = 0L;
   }
   
   if (ll > 8000L)
   {
						sprintf(buffer,__Texto[46],tmp);
                  aviso(0,buffer);
                  free(buffer);
                  n_np = 1;
                  return(0);
   }
   
   memset(buffer,0,8000);
   memcpy(buffer,&ll,sizeof(ll));
   
   memcpy(buffer+sizeof(ll),&numero,sizeof(ll));
   if (file > -1)
   {   
						rw_lseek_(file,0L,SEEK_SET);
                  rw_read_(file,buffer+(sizeof(ll)*2),ll);
                  rw_close_(file);
                  file = -1;
   }
   
#ifdef RWSERVER
   if (en_conexion_externa)
   {
						funcion_windows_cliente(1,buffer,(int)(ll+sizeof(ll)*2));
                  recibido = 1;
   }
#endif
   
   if (!recibido)
   {
#ifndef FGVDOS
						aviso(0,__Texto[47]);
#else
                  en_daclaves(buffer);
                  recibido = 1;
#endif
   }
   if (recibido)
   {
						memcpy(&ll,buffer,sizeof(ll));
                  if (!ll)
                  {
                     aviso(0,__Texto[48]);
                  }
                  else
                  {
#ifdef XXXXXXX_NO
                     sprintf(tmp,"c:/francesc/registro/%06ld.ini",numero);
                     file = rw_open_(tmp,O_RDWR | O_BINARY | O_CREAT | O_TRUNC,S_IREAD | S_IWRITE);
                     if (file < 0)
                     {
                        sprintf(tmp,"No se puede crear /claves/agi/%06ld.txt",numero);
                        aviso(0,tmp);
                     }
                     else
                     {	 
                        if (rw_write_(file,buffer+(sizeof(ll)*2),ll) == ll)
                        {
                        /*sprintf(tmp,"/claves/agi/%06ld.txt GRABADO!",numero);
                        aviso(0,tmp);		
                           */
                        }
                        rw_close_(file);
                     }
#endif
                  }
   }
   free(buffer);
   n_np = 1;
   return(0);
}


int rw_va_con_doble(p,tip,faclin)
int p;
int tip;
int faclin;
{
   /* puede que sea un impresin del generador antiguo! */
   if (p == (nprog-1) || prog[p+1].i != 255)
   {
      return(rw_imprime(p,tip,faclin));
   }
   /****************************************************/   
   
   strcpy(nomapli_doble,v_memo( prog[p++].indice )->alfabetica);
   strcpy(empresa_doble,v_memo( prog[p++].indice )->alfabetica);
   if (!strcmp(nomapli_doble,nomapli))
   {
      va_con_doble_ = 1;
   }
   else
   {
      va_con_doble_ = 0;
   }
   n_np = 2;
   return(0);
}

int rw_es_la_doble(p,tip,faclin)
int p;
int tip;
int faclin;
{
   if (va_con_doble_)
   {
      es_la_doble_ = pasa_a_int(v_memo( prog[p++].indice )->numerica);
      if (es_la_doble_)
									caracter_de_fecha = '/';
      else
									caracter_de_fecha = '.'; 
   }
   n_np = 1;
   return(0);
}


int rw_test_doble(p,tip,faclin)
int p;
int tip;
int faclin;
{
   v_memo( prog[p++].indice )->numerica = (double)va_con_doble_;
   v_memo( prog[p].indice )->numerica = (double)es_la_doble_;  
   n_np = 2;
   return(0);
}


int rw_hiden_indicator(p,tip,faclin)
int p;
int tip;
int faclin;
{
   v_memo( prog[p].indice )->numerica = (double)data_hiden_indicator;
   n_np = 1;
   return(0);
}

int rw_cambia_doble(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int quiero;
   if (va_con_doble_)
   {
      quiero = pasa_a_int(v_memo( prog[p].indice )->numerica);
      if (quiero != es_la_doble_)
      {
         data_fecha_conmutar = quiero + 2;
         cambiame_paths_especial();
         data_fecha_conmutar = 0;
      }
   }
   n_np = 1;
   return(0);
}

int rw_dime_mezcla(p,tip,faclin)
int p;
int tip;
int faclin;
{
   if (puntero_a_cual)
   {
      v_memo( prog[p].indice )->numerica = (double)*puntero_a_cual;
   }
   else
   {
      v_memo( prog[p].indice )->numerica = (double)-1;
   }
   n_np = 1;
   return(0);
}

int rw_set_other_buf(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int f1 = v_prog(prog[p++].indice);
   int f2 = v_prog(prog[p].indice);
   free(v_mpfi(f1)->f_buf);
   if (v_mpfi(f1)->f_lfich != v_mpfi(f2)->f_lfich)
   {
      aviso(0,"WARNING!! DIFERENT FILES MIXED!!");
   }
   v_mpfi(f1)->f_buf = v_mpfi(f2)->f_buf;
   dont_close_this_ = f2;
   v_mpfi(f2)->f_fd = v_mpfi(f1)->f_fd;
   n_np = 2;
   return(0);
}

int rw_set_restore_buf(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int f1 = v_prog(prog[p++].indice);
   int f2 = v_prog(prog[p].indice);
   v_mpfi(f1)->f_buf = malloc(v_mpfi(f1)->f_lfich+1);
   dont_close_this_ = -1;
   v_mpfi(f2)->f_fd = -1;
   n_np = 2;
   return(0);
}

int rw_ocultalineas(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int ipfi   = v_prog(prog[p++].indice);
   int modo = pasa_a_int(v_memo(prog[p++].indice)->numerica);
   int j;
   
   for (j=0;j<v_mpfirel(ipfi)->nrel;j++) 
   {
      if (v_mpfirel(ipfi)->camminu[j] < 0)
      {
         if (modo)
         {
            if (v_mpfirel(ipfi)->camminu[j] > -15000)
            {
               v_mpfirel(ipfi)->camminu[j] -= 15000;
            }
         }
         else
         {
            if (v_mpfirel(ipfi)->camminu[j] < -15000) 
            {
               v_mpfirel(ipfi)->camminu[j] += 15000;
            }
         }
      }
   }
   
   n_np = 2;
   return(0);
}


extern FILE *remoto_fopen();

FILE *quiza_remoto_fopen(tmp,modo,remoto)
char *tmp;
char *modo;
int *remoto;
{
   char tmp2[256];
   int flg = 0;

#ifdef RWSERVER
   if (en_conexion_externa && (!remoto || *remoto))
   {
      if (remoto) flg = 1;
      return(remoto_fopen(tmp,modo,flg));
   }
#endif

   if (remoto)
   {
       *remoto = 0;
       flg = 1;
   }
   if (flg)
      strcpy(tmp2,tmp);
   else
      sprintf(tmp2,"%s%s",dirbass,tmp);
   return(fopen(tmp2,modo));
}

int quiza_remoto_fclose(ff,remoto)
FILE *ff;
int *remoto;
{
#ifdef RWSERVER
   if (en_conexion_externa && (!remoto || *remoto))
      return(remoto_fclose(ff));
#endif
   return(fclose(ff));
}

extern char *remoto_lee_linea();
char *quiza_remoto_lee_linea(tmp,n,ff,remoto)
char *tmp;
int n;
FILE *ff;
int *remoto;
{
#ifdef RWSERVER
   if (en_conexion_externa && (!remoto || *remoto))
      return(remoto_lee_linea(tmp,n,ff));
#endif
   return(lee_linea(tmp,n,ff));
}


int quiza_remoto_graba_linea(tmp,ff,remoto)
char *tmp;
FILE *ff;
int *remoto;
{
#ifdef RWSERVER
   if (en_conexion_externa && (!remoto || *remoto))
      return(remoto_graba_linea(tmp,ff));
#endif
   return(graba_linea(tmp,ff));
}

extern long remoto_fseek();
long quiza_remoto_fseek(ff,pos,modo,remoto)
FILE *ff;
long pos;
int modo;
int *remoto;
{
#ifdef RWSERVER
   if (en_conexion_externa && (!remoto || *remoto))
      return(remoto_fseek(ff,pos,modo));
#endif

   if (pos || modo != SEEK_CUR)
   {
      fseek( ff, pos, modo );
   }
   pos = ftell(ff);

   return(pos);
}


int quiza_remoto_rw_open_(tmp,flag,remoto)
char *tmp;
int flag;
int *remoto;
{
   int fl;
   int flg = 0;
   char tmp2[256];
#ifdef RWSERVER
   if (en_conexion_externa && (!remoto || *remoto))
   {
        if (remoto)
        {
            flg = 1;
        }
      return(remoto_rw_open_(tmp,flag,flg));
   }
#endif
   if (remoto)
   {
       flg = 1;
   }

   if (!flag)
   {
      fl = O_BINARY | O_RDWR | O_CREAT | O_TRUNC;
   }
   else
   if (flag == 1)
   {
      fl = O_BINARY | O_RDWR | O_CREAT | O_APPEND;
   }
   else
   if (flag == 3)
   {
      fl = O_BINARY | O_RDONLY;
   }
   else
   {
      fl = O_BINARY | O_RDWR;
   }
   
   if (flg)
   {
      strcpy(tmp2,tmp);
   }
   else
   {
      sprintf(tmp2,"%s%s",dirbass,tmp);
   }
   return(rw_base_open_(tmp2,fl,S_IREAD | S_IWRITE));
}



int quiza_remoto_rw_read_(file,regi,tot,remoto)
int file;
char *regi;
int tot;
int *remoto;
{
#ifdef RWSERVER
   if (en_conexion_externa && (!remoto || *remoto))
      return(remoto_rw_read_(file,regi,tot));
#endif
   return(rw_base_read_(file,regi,tot));
}


int quiza_remoto_rw_write_(file,regi,tot,remoto)
int file;
char *regi;
int tot;
int *remoto;
{
#ifdef RWSERVER
   if (en_conexion_externa && (!remoto || *remoto))
      return(remoto_rw_write_(file,regi,tot));
#endif
   return(rw_base_write_(file,regi,tot));
}


extern long remoto_rw_lseek_();
long quiza_remoto_rw_lseek_(file,regi,tot,remoto)
int file;
long regi;
int tot;
int *remoto;
{
#ifdef RWSERVER
   if (en_conexion_externa && (!remoto || *remoto))
      return(remoto_rw_lseek_(file,regi,tot));
#endif
   return(rw_base_lseek_(file,regi,tot));
}

int quiza_remoto_rw_close_(file,remoto)
int file;
int *remoto;
{
#ifdef RWSERVER
   if (en_conexion_externa && (!remoto || *remoto))
      return(remoto_rw_close_(file));
#endif
   return(rw_base_close_(file));
}


int fgv_formato_hacienda(p,tip,faclin)
int p;
int tip;
int faclin;
{
   FILE *ff;
   int file;
   unsigned char mi_modo = prog[p].i;
   int ipfi = v_prog(prog[p++].indice);
   char *dir = v_memo(prog[p++].indice)->alfabetica;
   char *fformato = v_memo(prog[p++].indice)->alfabetica;
   char *ftabla = v_memo(prog[p++].indice)->alfabetica;
   char *fdestino = v_memo(prog[p++].indice)->alfabetica;
   unsigned char tmp[560];
   char usi[80];
   unsigned char *regi;
   char *_n[10];
   int separata,tipo,i,j,tot,n,nn;
   struct _h_formato
   {
      int tipo;
      int lon;
      int numero;
      int campo;
   } *h_formato = NULL;
   int nformato = 0;
   int errores = 0;
   
   
   
   sprintf(tmp,"%s/%s/%s",nomapli,dir,fformato);
   ff = quiza_remoto_fopen(tmp,"r",NULL);
   if (ff)
   {
      h_formato = (struct _h_formato *)malloc(sizeof(struct _h_formato) * 500);
      while(quiza_remoto_lee_linea(tmp,512,ff,NULL))
      {
         quitab(tmp);
         quitabp(tmp);
         if (strlen(tmp))
         {
            separata = _separa(tmp,_n,1,4);		   
            if (separata > 3 && atoi(tmp) > 0)
            {
               tipo = 0;
               if (atoi(_n[1]) > 0)
               {
                  quitabp(_n[2]);
                  quitab(_n[2]);
                  if (!strcmp(_n[2],"Num.") || !strcmp(_n[2],"Num"))
                  {
                     tipo = 1;
                  }
                  else
                     if (!strcmp(_n[2],"N"))
                     {
                        tipo = 2;
                     }
                     else
                        if (!strcmp(_n[2],"Alfan") || !strcmp(_n[2],"An"))
                        {
                           tipo = 3;
                        }
                        else
                           if (!strcmp(_n[2],"Alfab") || !strcmp(_n[2],"A"))
                           {
																														tipo = 4;
                           }			     
                           if (tipo > 0)
                           {
							h_formato[nformato].tipo = tipo;
                              h_formato[nformato].lon = atoi(_n[1]);
                              h_formato[nformato].numero = atoi(tmp);
                              if (h_formato[nformato].numero != (nformato+1))
                              {
                                 /* error */
                                 errores++;
                              }
                              h_formato[nformato].campo = -2;
                              nformato++;
                           }  
               }
            }
         }
      }
      quiza_remoto_fclose(ff,NULL);
      if (nformato)
      {
         sprintf(tmp,"%s/%s/%s",nomapli,dir,ftabla);
         ff = quiza_remoto_fopen(tmp,"r",NULL);
         if (ff)
         {
            while(quiza_remoto_lee_linea(tmp,512,ff,NULL))
            {
               quita_blancos(tmp);
               for (i = 0;tmp[i];i++)
               {
                  if (tmp[i] == ',')
                  {
                     tmp[i] = 0;
                     j = atoi(tmp+i+1);
                     if (j > 0 && j <= nformato)
                     {
                        if (h_formato[j-1].numero == j)
                        {
                           if (tmp[0])
                           {
																														h_formato[j-1].campo = atoi(tmp);
                           }
                           else
                           {
																														h_formato[j-1].campo = -1; 
                           }
                        }
                        else
                        {
                           /* error */
                           errores++;
                        }
                     }
                  }
               }
            }
            quiza_remoto_fclose(ff,NULL);
         }
         else
            errores++;
         sprintf(tmp,"%s/%s/%s",nomapli,dir,fdestino);
         file = quiza_remoto_rw_open_(tmp,1,NULL);
         if (file >= 0)
         {	
            tot = 0;
            for (i = 0;i < nformato;i++)
            {
               tot += h_formato[i].lon;
            }
            if (tot > 0)
            {
               regi = malloc(tot+1);
               j = 0;
               for (i = 0;i < nformato;i++)
               {
                  if (mi_modo != 216 && i == (nformato-1) && h_formato[i].lon == 2)
                  {
                     regi[j] = 13;
                     regi[j+1] = 10;
                     j += h_formato[i].lon;
                     continue;
                  }
                  if (h_formato[i].campo >= 0 && h_formato[i].campo < v_mpfi(ipfi)->f_ncampos)
                  {
                     ld_campo(h_formato[i].campo,v_mpfi(ipfi),tmp);
                     tipo = h_formato[i].tipo;
                     if (tipo == 3 || tipo == 4)
                     {
                        if (v_mpfi(ipfi)->f_campos[h_formato[i].campo].c_qc == -1) /* fecha */
                        {
                           memcpy(regi+j,tmp,2);
                           regi[j+2] = '-';
                           memcpy(regi+j+3,tmp+3,2);
                           regi[j+5] = '-';
                           memcpy(regi+j+6,tmp+8,2);
                           n = 8;
                        }
                        else
                        {
                           n =0;
                           for (n = 0;n < h_formato[i].lon && tmp[n];n++)
                           {
														switch(tmp[n])
                          {
                          case 129:
                             tmp[n] = 'U';
                             break;
                          case 130:
                             tmp[n] = 'E';
                             break;
                          case 132:
                             tmp[n] = 'A';
                             break;
                          case 133:
                             tmp[n] = 'A';
                             break;
                          case 137:
                             tmp[n] = 'E';
                             break;
                          case 138:
                             tmp[n] = 'E';
                             break;
                          case 139:
                             tmp[n] = 'I';
                             break;
                          case 141:
                             tmp[n] = 'I';
                             break;
                          case 148:
                             tmp[n] = 'O';
                             break;
                          case 149:
                             tmp[n] = 'O';
                             break;
                          case 151:
                             tmp[n] = 'U';
                             break;
                          case 160:
                             tmp[n] = 'A';
                             break;
                          case 161:
                             tmp[n] = 'I';
                             break;
                          case 162:
                             tmp[n] = 'O';
                             break;
                          case 163:
                             tmp[n] = 'U';
                             break;
                          default:
                             break;
                          }
                          if (tmp[n] >= 'a' && tmp[n] <= 'z')						     
                             tmp[n] -= 32;						     
                          else						     
                             if (tmp[n] == 164)
                                tmp[n] = 165;
                             else
                                if (tmp[n] == 135)
                                   tmp[n] = 128;
                                else
                                   if (tmp[n] != ',' && tmp[n] != '.' && tmp[n] != ';' && tmp[n] != ':' && (tmp[n] < 'A' || tmp[n] > 'Z'))
                                   {
                                      if (tipo == 4 || ((tmp[n] < '0' || tmp[n] > '9') && tmp[n] != '-')) 
                                         tmp[n] = ' ';						     
                                   }						  
                                   regi[j+n] = tmp[n];
                           }
                        }
                        for (;n < h_formato[i].lon;n++)
                        {
                           regi[j+n] = ' ';
                        }				      
                     }
                     else				   
                        if (tipo == 1)
                        {
                           sprintf(usi,"%%0%dld",h_formato[i].lon);
                           sprintf(regi+j,usi,atol(tmp));
                        }
                        else
                           if (tipo == 2)
                           {
														quita_blancos(tmp);				   
                          for (n = 0;tmp[n];n++)
                          {
                             if (!n && tmp[n] == '-') continue;
                             if (tmp[n] < '0' || tmp[n] > '9')
                             {
                                tmp[n] = 0;
                                break;
                             }
                          }
                          n = 0;
                          nn = h_formato[i].lon - strlen(tmp);
                          if (tmp[0] == '-')
                          {                     
                             regi[j+n] = 'N';
                             n = 1;
                             nn++;
                          }				   
                          for (;n < nn;n++)
                          {
                             regi[j+n] = '0';
                          }
                          if (tmp[0] == '-')
                          {
                             nn--;
                          }
                          for (;n < h_formato[i].lon && tmp[n-nn];n++)
                          {
                             regi[j+n] = tmp[n-nn];
                          }
                          for (;n < h_formato[i].lon;n++)
                          {
                             regi[j+n] = '0';
                          }
                           }
             }
             else
             {
                if (h_formato[i].campo > -2)
                {
                   tipo = h_formato[i].tipo;
                   if (tipo == 3 || tipo == 4)
                   {
                      memset(regi+j,' ',h_formato[i].lon);
                   }
                   else
                      if (tipo == 1 || tipo == 2)
                      {
                         memset(regi+j,'0',h_formato[i].lon);
                      }
                }
             }
             if (h_formato[i].campo > -2)
                j += h_formato[i].lon;
           }
           quiza_remoto_rw_write_(file,regi,j,NULL);
           free(regi);
          }
          else
             errores++;
          quiza_remoto_rw_close_(file,NULL);
        }
       }
       free((char *)h_formato);
     }
     else
        errores++;
     
     sprintf(v_memo(5004)->alfabetica,"%d",errores); 
     n_np = 5;
     return(0);
}

int fgv_imprime_hacienda(p,tip,faclin)
int p;
int tip;
int faclin;
{
   char *dir = v_memo(prog[p++].indice)->alfabetica;
   char *fexe = v_memo(prog[p++].indice)->alfabetica;
   char *ffile = v_memo(prog[p++].indice)->alfabetica;
   char tmp[560];
   
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      sprintf(tmp,"%s/%s/%s :/%s/%s %s",nomapli,dir,fexe,nomapli,dir,ffile);
      remoto_ejecutas(tmp);
      n_np = 3;
      return(0);
   }
#endif
   sprintf(tmp,"%s%s/%s :/%s/%s %s",dirbase,dir,fexe,nomapli,dir,ffile);
   _ejecutas_(tmp,1);
   n_np = 3;
   return(0);
}

int fgv_ejecuta_remoto(p,tip,faclin)
int p;
int tip;
int faclin;
{
#ifdef RWXARXA
   char tmp[300];
   int m1 = prog[p++].indice;
   int m2 = prog[p++].indice;
   int m3 = prog[p++].indice;
   
   strcpy(tmp,v_memo(m2)->alfabetica);
   tmp[19] = 0;
   strcpy(tmp+20,v_memo(m3)->alfabetica);
   sprintf(v_memo(5004)->alfabetica,"%d",peticion_a_transfer(v_memo(m1)->alfabetica,'5',294,tmp));
#else
   strcpy(v_memo(5004)->alfabetica,"-1");
#endif
   n_np = 3;
   return(0);
}

int fgv_recibe_fichero(p,tip,faclin)
int p;
int tip;
int faclin;
{
#ifdef RWXARXA
			char tmp[300];
         int m1 = prog[p++].indice;
         int m2 = prog[p++].indice;
         int m3 = prog[p++].indice;
         int m4 = prog[p++].indice;
         
         strcpy(tmp,v_memo(m2)->alfabetica);
         tmp[19] = 0;
         strcpy(tmp+20,v_memo(m3)->alfabetica);
         strcpy(tmp+120,v_memo(m4)->alfabetica);
         sprintf(v_memo(5004)->alfabetica,"%d",peticion_a_transfer(v_memo(m1)->alfabetica,'2',294,tmp));
#else
         strcpy(v_memo(5004)->alfabetica,"-1");
#endif
         n_np = 4;
         return(0);
}


int fgv_envia_fichero(p,tip,faclin)
int p;
int tip;
int faclin;
{
#ifdef RWXARXA
   char tmp[300];
   int m1 = prog[p++].indice;
   int m2 = prog[p++].indice;
   int m3 = prog[p++].indice;
   int m4 = prog[p++].indice;
   
   strcpy(tmp,v_memo(m2)->alfabetica);
   tmp[19] = 0;
   strcpy(tmp+20,v_memo(m3)->alfabetica);
   strcpy(tmp+120,v_memo(m4)->alfabetica);
   sprintf(v_memo(5004)->alfabetica,"%d",peticion_a_transfer(v_memo(m1)->alfabetica,'3',294,tmp));
#else
   strcpy(v_memo(5004)->alfabetica,"-1");
#endif
   n_np = 4;
   return(0);
}



int fgv_cambia_de_def(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int f1 = v_prog(prog[p++].indice);
   int c = prog[p++].indice;
   FI_DEF *_pfi;
   int r;
   int recur = (recur_punto < 1) ? 0 : recur_punto * MAXDEF;
   
   r = lee_defn(v_memo(c)->alfabetica,&_pfi);
   if (!r)
   {
				  free_defpri(v_mpfi(f1));
                  mpfi[f1] = _pfi;                  
                  if (s_d[recur+f1].nombre_fichero)
                  {
                     free(s_d[recur+f1].nombre_fichero);
                     s_d[recur+f1].nombre_fichero = strmalloc(strlen(v_memo(c)->alfabetica)+1);
                     strcpy(s_d[recur+f1].nombre_fichero,v_memo(c)->alfabetica);
                  }
                  if (s_d[recur+f1].enlaza_pfi)
                  {
                     free(s_d[recur+f1].enlaza_pfi);
                     s_d[recur+f1].enlaza_pfi = strmalloc(strlen(v_memo(c)->alfabetica)+1);
                     strcpy(s_d[recur+f1].enlaza_pfi,v_memo(c)->alfabetica);
                  }
                  if (s_d[recur+f1].salvadatos)
                  {
                     free(s_d[recur+f1].salvadatos);
                     s_d[recur+f1].salvadatos = NULL;
                  }
                  if (s_d[recur+f1].buf)
                  {
                     free(s_d[recur+f1].buf);
                     s_d[recur+f1].buf = NULL;
					 s_d[recur+f1].tam_buf = 0;
                  }
                  s_d[recur+f1].fd = -1;
                  s_d[recur+f1].cuando = 0L;
   }
   sprintf(v_memo(5004)->alfabetica,"%d",r);
   n_np = 2;
   return(0);
}


int inidde(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int m1 = prog[p++].indice; /* programa */
   int m2 = prog[p++].indice; /* Item */
   int m3 = prog[p++].indice; /* Topic */
   int m4 = prog[p++].indice; /* modo */
   int r = -1;
#ifdef RWSERVER
   char tmp[760];
   char *ptmp;
   
   if (en_conexion_externa)
   {
      r = pasa_a_int(v_memo(m4)->numerica);
      memcpy(tmp,&r,sizeof(int));
      ptmp = tmp + sizeof(int);
      strcpy(ptmp,v_memo(m1)->alfabetica);
      ptmp += (strlen(ptmp)+1);
      strcpy(ptmp,v_memo(m2)->alfabetica);
      ptmp += (strlen(ptmp)+1);
      strcpy(ptmp,v_memo(m3)->alfabetica);
      ptmp += (strlen(ptmp)+1);
      funcion_windows_cliente(2,tmp,(int)(ptmp-tmp));
      memcpy(&r,tmp,sizeof(int));
      sprintf(v_memo(5004)->alfabetica,"%d",r);
      n_np = 4;
      return(0);
   }
#endif
   
#ifdef FGVDOS
   r = _dde_init(v_memo(m1)->alfabetica,v_memo(m2)->alfabetica,v_memo(m3)->alfabetica,pasa_a_int(v_memo(m4)->numerica));
#endif
   
   sprintf(v_memo(5004)->alfabetica,"%d",r);
   n_np = 4;
   return(0);
}

int execdde(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int m1 = prog[p++].indice; /* par */
   int m2 = prog[p++].indice; /* Topic */
   int r = -1;
#ifdef RWSERVER
   char tmp[510];
   char *ptmp;
   
   if (en_conexion_externa)
   {
      ptmp = tmp;
      strcpy(ptmp,v_memo(m1)->alfabetica);
      ptmp += (strlen(ptmp)+1);
      strcpy(ptmp,v_memo(m2)->alfabetica);
      ptmp += (strlen(ptmp)+1);
      funcion_windows_cliente(4,tmp,(int)(ptmp-tmp));
      r = 0; /* !! */
      sprintf(v_memo(5004)->alfabetica,"%d",r);
      n_np = 2;
      return(0);
   }
#endif
   
#ifdef FGVDOS
   r = _dde_exec(v_memo(m1)->alfabetica,v_memo(m2)->alfabetica);
#endif
   
   sprintf(v_memo(5004)->alfabetica,"%d",r);
   n_np = 2;
   return(0);
}

int pokedde(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int m1 = prog[p++].indice; /* par */
   int m2 = prog[p++].indice; /* itemid */
   int m3 = prog[p++].indice; /* Topic */
   int r = -1;
#ifdef RWSERVER
   char tmp[760];
   char *ptmp;
   
   if (en_conexion_externa)
   { 
      ptmp = tmp;
      strcpy(ptmp,v_memo(m1)->alfabetica);
      ptmp += (strlen(ptmp)+1);
      strcpy(ptmp,v_memo(m2)->alfabetica);
      ptmp += (strlen(ptmp)+1);
      strcpy(ptmp,v_memo(m3)->alfabetica);
      ptmp += (strlen(ptmp)+1);
      funcion_windows_cliente(3,tmp,(int)(ptmp-tmp));
      r = 0; /* !! */
      sprintf(v_memo(5004)->alfabetica,"%d",r);
      n_np = 3;
      return(0);
   }
#endif
   
#ifdef FGVDOS
   r = _dde_poke(v_memo(m1)->alfabetica,v_memo(m2)->alfabetica,v_memo(m3)->alfabetica);
#endif
   
   sprintf(v_memo(5004)->alfabetica,"%d",r);
   
   n_np = 3;
   return(0);
}

int findde(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int r = -1;
#ifdef RWSERVER
   char tmp[1024];
   
   if (en_conexion_externa)
   {
      tmp[0] = 0;
      funcion_windows_cliente(5,tmp,1);
      memcpy(&r,tmp,sizeof(int));
      sprintf(v_memo(5004)->alfabetica,"%d",r);
      n_np = 1;
      return(0);
   }
#endif
   
#ifdef FGVDOS
   r = _dde_fin();
#endif
   
   sprintf(v_memo(5004)->alfabetica,"%d",r);
   
   n_np = 1;
   return(0);
}


int peekdde(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int m1 = prog[p++].indice; /* par */
   int m2 = prog[p++].indice; /* itemid */
   int m3 = prog[p++].indice; /* Topic */
   int r = -1;
   char *ptmp;
#ifdef RWSERVER
   char tmp[760];
   
   
   if (en_conexion_externa)
   { 
      ptmp = tmp;
      strcpy(ptmp,v_memo(m2)->alfabetica);
      ptmp += (strlen(ptmp)+1);
      strcpy(ptmp,v_memo(m3)->alfabetica);
      ptmp += (strlen(ptmp)+1);
      funcion_windows_cliente(6,tmp,(int)(ptmp-tmp));
      memcpy(&r,tmp,sizeof(int));
      if (r >= 0)
      {
         if (no_memo_interna(m1)) 
         {
            free(v_memo(m1)->alfabetica);
            v_memo(m1)->alfabetica = malloc(strlen(tmp+sizeof(int))+1);		 
         }
         strcpy(v_memo(m1)->alfabetica,tmp+sizeof(int));	   
      }	 	 
      sprintf(v_memo(5004)->alfabetica,"%d",r);
      n_np = 3;
      return(0);
   }
#endif
   
#ifdef FGVDOS
   if (no_memo_interna(m1)) 
   {
      ptmp = v_memo(m1)->alfabetica;
   }
   else
   {
      ptmp = NULL;
   }
   r = _dde_peek(&ptmp,v_memo(m2)->alfabetica,v_memo(m3)->alfabetica);
   if (r >= 0 && ptmp)
   {
      if (!no_memo_interna(m1)) 
      {
         strcpy(v_memo(m1)->alfabetica,ptmp);
         free(ptmp);
      }
      else
      {
         v_memo(m1)->alfabetica = ptmp;
      }
   }
#endif
   
   sprintf(v_memo(5004)->alfabetica,"%d",r);
   
   n_np = 3;
   return(0);
}


int fgv_control(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int m1 = prog[p++].indice; /* pos */
   int m2 = prog[p++].indice; /* dato */
   if (pasa_a_int(v_memo(m1)->numerica) > 0 && (strlen(v_memo(m2)->alfabetica) + pasa_a_int(v_memo(m1)->numerica)) < 250)
   {
      memcpy(v_memo(5002)->alfabetica + pasa_a_int(v_memo(m1)->numerica) - 1,v_memo(m2)->alfabetica,strlen(v_memo(m2)->alfabetica));
   }
   n_np = 2;
   return(0);
}


int fgv_parte_fichero(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int m1 = prog[p++].indice; /* fichero */
   int m2 = prog[p++].indice; /* tam */
   int m3 = prog[p++].indice; /* destino */
   int fdo,fdd;
   long parte,leido;
   unsigned char *buff;
   long tam = 0L;
   int err = 0,aleer,nr;
   char tmp[512];
   
   if (v_memo(m2)->numerica < 1)
   {
      err = -1;
   }
   else
   {
      fdo = rw_base_open_(v_memo(m1)->alfabetica,O_RDONLY | O_BINARY);
      if (fdo < 0)
      {
         err = -2;
      }
      else
      {
         parte = 0L;
         leido = 0L;
         sprintf(tmp,"%s%ld",v_memo(m3)->alfabetica,parte);
         fdd = rw_base_open_(tmp,O_RDWR | O_BINARY | O_CREAT | O_TRUNC,S_IREAD | S_IWRITE);
         if (fdd < 0)
         {
            err = -3;
         }
         else
         {
            buff = malloc(30000);
            aleer = 30000;		
            while(-1)
            {
               if ((leido + aleer) > (long)v_memo(m2)->numerica)
               {
                  aleer = (long)v_memo(m2)->numerica - leido;
               }
               if (aleer > 0)
               {
                  nr = rw_base_read_(fdo,buff,aleer);
                  if (nr <= 0) break;
               }
               else
                  nr = 0;
               if (nr > 0)
               {
                  if (rw_base_write_(fdd,buff,nr) != nr)
																		{
                     err = -4 - (int)parte;
                     break;
																		}
               }
               leido += nr;
               if (nr < aleer) break;
               if (aleer < 30000)
               {
                  rw_base_close_(fdd);
                  parte++;
                  sprintf(tmp,"%s%ld",v_memo(m3)->alfabetica,parte);
                  fdd = rw_base_open_(tmp,O_RDWR | O_BINARY | O_CREAT | O_TRUNC,S_IREAD | S_IWRITE);
                  if (fdd < 0)
				  {
                     err = -4 - (int)parte;
                     break;
				  }
                  aleer = 30000;
                  leido = 0L;
               }           
            }
            rw_base_close_(fdd);
            if (!err) err = (int)(parte+1); 
         }
         rw_base_close_(fdo);
      }
   }
   sprintf(v_memo(5004)->alfabetica,"%d",err);
   n_np = 3;
   return(0);
}

int fgv_ipremoto(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int m1 = prog[p].indice; 
   char buf[512];
   
   if (prog[p].i == 255)
   {
      buf[0] = 0;
      dame_ip_por_defecto(buf,511);
      memo_strcpy(m1,buf);
      n_np = 1;
      return(0);
   }
   
#ifdef RWSERVER 
   
   if (en_conexion_externa)
   {
      buf[0] = 0;
      recoge_ip_remota(buf);
      memo_strcpy(m1,buf);
   }
   
#endif
   n_np = 1;
   return(0);
}

int fgv_ipnombre(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int m1 = prog[p++].indice;
   int m2 = prog[p++].indice;
#ifdef RWXARXA
   char buf[1024];
   dame_nombre_por_ip(v_memo(m1)->alfabetica,buf);
   buf[250] = 0;
   memo_strcpy(m2,buf);
#endif
   n_np = 2;
   return(0);
}


int fgv_enlaza_proc(p,tip,faclin)
int p;
int tip;
int faclin;
{
   /*"ENLAZA_PROCESO", "9E7598",229,NULL,*/
   short f0,f,c;
   int pp,tipo,j;
   
   f0 = prog[p++].indice;
   comoes_cam(f0,NULL,NULL,NULL,&f,&c,NULL);  
   
   pp = prog[p++].indice;
   
   if (pp >= 0)
   {
       pp = en_pr[programa_activo].tabla_gosubs[pp];
   }

   tipo = (int)v_memo(prog[p++].indice)->numerica;

   
   for (j=0;j<v_mpfirel(f)->nrelact;j++) 
   {
       if (v_mpfirel(f)->relact[j].tabla == pp)
       {
           n_np = 3;
           return(0);
       }
   }
   
   j = v_mpfirel(f)->nrelact;
   v_mpfirel(f)->relact = (REL_ACT *)realloc((void *)v_mpfirel(f)->relact,(j+1)*sizeof(REL_ACT));
   v_mpfirel(f)->nrelact++;
   v_mpfirel(f)->relact[j].campo = c;
   v_mpfirel(f)->relact[j].ambito = 0;
   v_mpfirel(f)->relact[j].tabla = pp;
   v_mpfirel(f)->relact[j].fflag = 0;
   tipo_tabla[pp] = tipo;

   sprintf(v_memo(5004)->alfabetica,"%d",j);

   n_np = 3;
   return(0);
}

int fgv_desenlaza_proc(p,tip,faclin)
int p;
int tip;
int faclin;
{
   /*"DESENLAZA_PROCESO","9E7598"	 	 ,230,NULL,*/
   
   n_np = 3;
   return(0);
}

int inicializa_este_def(nom,nombre,ipfi,modo)
char *nom,*nombre;
int ipfi,modo;
{
   int recur = (recur_punto < 1) ? 0 : recur_punto * MAXDEF;
   
   mpfirel[ipfi] = (RELAS *)malloc(sizeof(RELAS));
   memset(mpfirel[ipfi],0,sizeof(RELAS));
   mpfirel[ipfi]->clave = -1;
   mpfirel[ipfi]->lineas = -1;
   mflagfi[ipfi] = 0;
   
   s_d[recur+ipfi].salvadatos = NULL;
   s_d[recur+ipfi].buf = NULL;
   s_d[recur+ipfi].tam_buf = 0;
   s_d[recur+ipfi].fd = -1;
   s_d[recur+ipfi].nombre_fichero = strmalloc(strlen(nombre)+1);
   strcpy(s_d[recur+ipfi].nombre_fichero,nombre);
   s_d[recur+ipfi].enlaza_pfi = strmalloc(strlen(nom)+1);
   strcpy(s_d[recur+ipfi].enlaza_pfi,nom);
   s_d[recur+ipfi].cuando = 0L;
   recur += ipfi;
   mpfi[ipfi]->f_buf = rwmalloc(mpfi[ipfi]->f_lfich+1);

   if (!modo)
   {
		inicializa_def(ipfi);
   }

   da_memoria_salva_datos(mpfi[ipfi],recur);   
   return(0);
}

int fgv_carga_def(p,tip,faclin) /*"CARGA_DEF",        "9598"          ,231,NULL,*/
int p;
int tip;
int faclin;
{
   int m1 = prog[p++].indice;
   int m2 = prog[p++].indice;
   FI_DEF *fi = NULL;
   int i;
   char nombre[256];
   char tmp[256];
   int tipo = 0;
   int noinicializar = 0;
   
   if (nfi < MAXDEF)
   {
      strcpy(nombre,v_memo(m1)->alfabetica);
      for (i = strlen(nombre)-1;i >= 0;i--)
      {
         if (nombre[i] == '.')
         {
            if (!strcmp(nombre+i,".mas") || !strcmp(nombre+i,".mmm"))
            {
			   if (!strcmp(nombre+i,".mmm"))
					noinicializar = 1;
               tipo = 0;
               nombre[i] = 0;
            }
            else
               if (!strcmp(nombre+i,".dbf"))
               {
                  tipo = 1;
                  nombre[i] = 0;
               }
               else
                  if (!strcmp(nombre+i,".odbc"))
                  {
                     tipo = 2;
                     nombre[i] = 0;
                  }
                  else
                  {
                     tipo = -1;
                  }
                  break;
         }
      }
      switch(tipo)
      {
      case 0:
         ajusta_path_def(nombre,tmp);
         flag_leedefn = 1;
         if (lee_defn(tmp,&fi))
         {
            strcpy(v_memo(5004)->alfabetica,"-1");
         }
         else
         {
            if (nfi == 48) nfi++;
            mpfi[nfi] = fi;
            nfi++;
			inicializa_este_def(nombre,tmp,nfi-1,noinicializar);			
            strcpy(v_memo(5004)->alfabetica,"0");
            v_memo(m2)->numerica = (double)(nfi-1);
         }
         break;
      default:
         strcpy(v_memo(5004)->alfabetica,"-2");
      }
   }
   else
   {
      strcpy(v_memo(5004)->alfabetica,"-3");
   }
   n_np = 2;
   return(0);
}

int fgv_descarga_def(p,tip,faclin)
int p;
int tip;
int faclin;
{
   /*              "DESCARGA_DEF",     "49"            ,232,NULL, */
   int f = (double)v_prog(prog[p].indice);
   int recur = (recur_punto < 1) ? 0 : recur_punto * MAXDEF;

   if (mpfi[f]->f_fd > -1)
   {
	   cierra_ind(v_mpfi(f));
	   /* controlado en busca_valor el numero de campo
	    para asignaciones normales */
   }
   
   if (f == (nfi-1) && !(mpfirel[f]->nrel) && !(mpfirel[f]->nrelact) && !(mpfirel[f]->nreldef) && !(mpfirel[f]->nrelpin))
   {
      free(s_d[recur+f].nombre_fichero);
      free(s_d[recur+f].enlaza_pfi);
      if (s_d[f+recur].salvadatos)
      {
         free(s_d[f+recur].salvadatos);
         s_d[f+recur].salvadatos = NULL;
      }
      free_defpri(mpfi[f]); 
      mpfi[f] = NULL;
      libera_rela(f);
      nfi--;
      if (nfi == 49)
         nfi--;   
      strcpy(v_memo(5004)->alfabetica,"0");  
   }
   else
   {
      strcpy(v_memo(5004)->alfabetica,"-1");
   }
   n_np = 1;
   return(0);
}

int fgv_referencia_def(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int m1 = prog[p++].indice;
   /*             "REFERENCIA_DEF",   "9849"          ,233,NULL,*/
   v_memo(m1)->numerica = (double)v_prog(prog[p].indice);
   
   n_np = 2;
   return(0);
}


int fgv_indice_def(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int m1 = prog[p++].indice;
   int m2 = prog[p++].indice;
   
   v_memo(m1)->numerica = (double)ind_rel(v_memo(m2)->alfabetica);
   
   n_np = 2;
   return(0);
}
/*
extern int fgv_cargadll();
extern int fgv_descargadll();
extern int fgv_alfacalldll();
*/

int fgv_cargadll(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int m1 = prog[p++].indice;
   char buffer[1024];
   static int fgv_activa_progdll_aviso = 0;
   
   if (!fgv_activa_progdll)
   {
      if (!fgv_activa_progdll_aviso)
      {
         fgv_activa_progdll_aviso = 1;
         aviso(0,__Texto[49]);
      }
      n_np = 1;
      return(0);
   }

   if (excel_alt) {
	     strminus(v_memo(m1)->alfabetica);
		 if (!strcmp(quita_path(v_memo(m1)->alfabetica),"agixl97.dll")) {
			 return 0;
		 }
   }
   
#ifdef RWSERVER
   
   if (en_conexion_externa)
   {
      strcpy(buffer,v_memo(m1)->alfabetica);
      funcion_windows_cliente(7,buffer,strlen(buffer)+1);
	  moto_int(buffer);
      sprintf(v_memo(5004)->alfabetica,"%d",*((int *)buffer));
      n_np = 1;
      return(0);
   }
#endif
   
#ifdef FGVDOS
   sprintf(v_memo(5004)->alfabetica,"%d",carga_libreria_usuario(v_memo(m1)->alfabetica));
#else
   strcpy(v_memo(5004)->alfabetica,"-1");
#endif
   
   n_np = 1;
   return(0);
}


int fgv_descargadll(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int m1 = prog[p++].indice;
   char buffer[1024];
   
   if (!fgv_activa_progdll)
   {
      n_np = 1;
      return(0);
   }
   
   if (excel_alt) {
	     strminus(v_memo(m1)->alfabetica);
		 if (!strcmp(quita_path(v_memo(m1)->alfabetica),"agixl97.dll")) {
			 return 0;
		 }
	}
   
#ifdef RWSERVER
   
   if (en_conexion_externa)
   {
      strcpy(buffer,v_memo(m1)->alfabetica);
      funcion_windows_cliente(8,buffer,strlen(buffer)+1);
      sprintf(v_memo(5004)->alfabetica,"%d",*((int *)buffer));
      n_np = 1;
      return(0);
   }
#endif
   
#ifdef FGVDOS
   sprintf(v_memo(5004)->alfabetica,"%d",descarga_libreria_usuario(v_memo(m1)->alfabetica));
#else
   strcpy(v_memo(5004)->alfabetica,"-1");
#endif
   
   n_np = 1;
   return(0);
}


int fgv_alfacalldll(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int m1 = prog[p++].indice;
   int m2 = prog[p++].indice;
   int m3 = prog[p++].indice;
   char buffer[1024];
   int i;
   
   if (!fgv_activa_progdll)
   {
      n_np = 3;
      return(0);
   }

   if (excel_alt) {
			 strminus(v_memo(m1)->alfabetica);
			 if (!strcmp(quita_path(v_memo(m1)->alfabetica),"agixl97.dll")) {
				 strcpy(buffer,v_memo(m2)->alfabetica);
				 strcpy(buffer+31,v_memo(m3)->alfabetica);
				 if (DespachadorExterno)
					(*DespachadorExterno)((int)DESPACHO_FUNCIONES_IMPRESION,998,(void *)buffer);
				 memo_strcpy(m3,buffer+31);	  
				 sprintf(v_memo(5004)->alfabetica,"%d",0);
				 n_np = 3;
				 return(0);
			 }
   }
   
#ifdef RWSERVER   
   /*
   _asm {
   push eax
   pop eax
   };
   */   
   if (en_conexion_externa)
   {
      i = strlen(v_memo(m1)->alfabetica) + 1;
      strcpy(buffer,v_memo(m1)->alfabetica);
      
      strcpy(buffer+i,v_memo(m2)->alfabetica);
      
      i += (strlen(v_memo(m2)->alfabetica)+1);
      
      strcpy(buffer+i,v_memo(m3)->alfabetica);
      i += (strlen(v_memo(m3)->alfabetica)+1);
      
      funcion_windows_cliente(9,buffer,i);
      i = *((int *)buffer);
	  moto_int(&i);
      sprintf(v_memo(5004)->alfabetica,"%d",i);
      memo_strcpy(m3,buffer+sizeof(int));	  
      n_np = 3;
      return(0);
   }
#endif

#ifdef FGVDOS
   strcpy(buffer,v_memo(m3)->alfabetica);
   sprintf(v_memo(5004)->alfabetica,"%d",alfacall_libreria_usuario(v_memo(m1)->alfabetica,v_memo(m2)->alfabetica,buffer));
   memo_strcpy(m3,buffer);
#else
   strcpy(v_memo(5004)->alfabetica,"-1");
#endif

   n_np = 3;
   return(0);
}

static int mis_controles[800] = {0,0,0,};

extern void (*f_disparo)();

void mi_disparo(int d)
{   
   if (d >= DISPARO_CONTROL && d < PREMAX_DISPARO_CONTROL)
   {   
      d -= DISPARO_CONTROL;
      if (mis_controles[d])
      {
         p_ejecuta(mis_controles[d],O_NOR,0,12);
      }
   }
}

/*
static int mis_controles[101] = {0,0,0,};
static int mis_precontrol[101] = {0,0,0,};
static int mis_controlprc[101] = {0,0,0,};
extern void (*f_disparo)();

void mi_disparo(int d)
{
   char salva_fsalida[MAXDEFECTO];
   
   if (d >= DISPARO_CONTROL && d < MAX_DISPARO_CONTROL)
   {   
      int c;   
      d -= DISPARO_CONTROL;
      c = d/100;   
      if (mis_controles[c])
      {
         int ant_pact = programa_activo;
         d += 100;
         memcpy(salva_fsalida,v_memo(5004)->alfabetica,MAXDEFECTO);
         sprintf(v_memo(5004)->alfabetica,"%d",d);
         if (mis_controles[c] < 0)
         {
            if (programa_activo != mis_controlprc[c])
            {
               activa_programa(mis_controlprc[c]);
            }
         }
         p_ejecuta(mis_controles[c],O_NOR,0,13);
         if (mis_controles[c] < 0)
         {
            if (ant_pact != -1 && ant_pact != mis_controlprc[c])
            {
               activa_programa(ant_pact);
            }
         }
         memcpy(v_memo(5004)->alfabetica,salva_fsalida,MAXDEFECTO);
      }
   }
   if (d >= PREDISPARO_CONTROL && d < PREMAX_DISPARO_CONTROL)
   {   
      int c;   
      d -= PREDISPARO_CONTROL;
      c = d/100;   
      if (mis_precontrol[c])
      {
         int ant_pact = programa_activo;
         d += 100;
         memcpy(salva_fsalida,v_memo(5004)->alfabetica,MAXDEFECTO);
         sprintf(v_memo(5004)->alfabetica,"%d",d);
         if (mis_precontrol[c] < 0)
         {
            if (programa_activo != mis_controlprc[c])
            {
               activa_programa(mis_controlprc[c]);
            }
         }
         p_ejecuta(mis_precontrol[c],O_NOR,0,14);
         if (mis_precontrol[c] < 0)
         {
            if (ant_pact != -1 && ant_pact != mis_controlprc[c])
            {
               activa_programa(ant_pact);
            }
         }	  
         if (DespachadorExterno)
         {
            int r = atoi(v_memo(5004)->alfabetica);
            (*DespachadorExterno)((int)DESPACHO_PRE_CONTROL,r,NULL);         
         }
         memcpy(v_memo(5004)->alfabetica,salva_fsalida,MAXDEFECTO);
      }
   }
}
*/

int fgv_wcontrol(p,tip,faclin)
int p;
int tip;
int faclin;
{
/*
   int prerutina = 0;
   int rutina = 0;
   int pos;
   char dato[1024];
   char *pdato;
   
   memset(dato,0,1024);
   pdato = dato;
   strcpy(pdato,v_memo(prog[p++].indice)->alfabetica);
   pdato += 257;
   prerutina = prog[p++].indice;
   if (prerutina >= 0)
   {
      prerutina = en_pr[programa_activo].tabla_gosubs[prerutina];
   }
   rutina = prog[p++].indice;
   if (rutina >= 0)
   {
      rutina = en_pr[programa_activo].tabla_gosubs[rutina];
   }
   pos = v_memo(prog[p++].indice)->numerica;
   memcpy_int(pdato,&pos,sizeof(int));
   pdato += sizeof(int);
   strcpy(pdato,v_memo(prog[p++].indice)->alfabetica);
   pdato += 257;
   strcpy(pdato,v_memo(prog[p++].indice)->alfabetica);
   pdato += 257;
   
   if (DespachadorExterno)
   {
      int retorno = 0;
      int i;
      for (i = 0;i < 100;i++)
      {
         if (!mis_controles[i])
         {
            break;
         }
      }
      (*DespachadorExterno)((int)DESPACHO_MI_CONTROL,i,dato);
      memcpy(&retorno,dato,sizeof(int));
      sprintf(v_memo(5004)->alfabetica,"%d",retorno);
      if (retorno)
      {
         mis_precontrol[i] = prerutina;
         mis_controles[i] = rutina;
         mis_controlprc[i] = programa_activo;
         f_disparo = mi_disparo;
      }
   }
   else
*/
   {
      strcpy(v_memo(5004)->alfabetica,"0");
   }
   n_np = 6;
   return(0);
}

int fgv_finwcontrol(p,tip,faclin)
int p;
int tip;
int faclin;
{
   if (DespachadorExterno)
   {
      int control = pasa_a_int(v_memo(prog[p].indice)->numerica);   	  
	  /*if (control < -1)*/
	  {
		  (*DespachadorExterno)((int)DESPACHO_FIN_CONTROL,control,NULL);
	  }
	  /*
	  else	  
	  {
		  control--;
		  if (control >= 0 && control < 100 && mis_controles[control])
		  {
			 mis_controles[ control] = 0;
			 mis_precontrol[control] = 0;
			 mis_controlprc[control] = 0;
			 (*DespachadorExterno)((int)DESPACHO_FIN_CONTROL,control,NULL);
		  }
	  }
	  */
   }
   n_np = 1;
   return(0);
}

int rw_subfun(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int otra = prog[p].indice;
   n_np= 1;
   
   if (otra > 0  && otra <= nextra_fgv)
   {
      p++;
      ( *extra_fgv[ otra-1 ] )(p,tip,faclin);
      n_np++;
   }
   return(0);
}

int fgv_modo_bmp(p,tip,faclin)
int p;
int tip;
int faclin;
{
    extern int proporcion_especial_bmp;
	int m1 = prog[p++].indice; /* coger o poner */
	int m2 = prog[p++].indice; /* dato */

	if (!pasa_a_int(v_memo(m1)->numerica))
    {
        v_memo(m2)->numerica = (double)proporcion_especial_bmp;
    }
    else
    {
        proporcion_especial_bmp = pasa_a_int(v_memo(m2)->numerica);
    }
    n_np = 2;
    return(0);
}


int fgv_modo_relacion(p,tip,faclin)
int p;
int tip;
int faclin;
{
int i,j;
int f = v_prog(prog[p++].indice);
int modo = pasa_a_int( v_memo(prog[p++].indice)->numerica );
char *fichero = v_memo(prog[p++].indice)->alfabetica;
int mleer   = prog[p++].indice;
int mobliga = prog[p++].indice;

    strcpy(v_memo(5004)->alfabetica,"-1");
    for (i = 0;i < v_mpfi(f)->f_nrelaci;i++)
    {
        if (!strcmp(fichero,quita_path(v_mpfi(f)->f_relaci[i].r_fichero)))
        {
            if (modo)
            {

                if (v_memo(mleer)->alfabetica[0] == 'X' && v_memo(mobliga)->alfabetica[0] == 'X')
                {
                    for (j = 0;j < v_mpfirel(f)->nrel;j++)
                    {
                        if (v_mpfirel(f)->numrela[j] == i)
                        {
                            v_mpfirel(f)->numrela[j] = -(v_mpfirel(f)->numrela[j]+1);
                        }
                    }
                    for (j = 0;j < v_mpfirel(f)->nreldef;j++)
                    {
                        if (v_mpfirel(f)->reldef[j].numrela == i)
                        {
                            v_mpfirel(f)->reldef[j].numrela = -(v_mpfirel(f)->reldef[j].numrela+1);
                        }
                    }                   
                }
                else
                {
                    if (v_memo(mleer)->alfabetica[0] == 's' || v_memo(mleer)->alfabetica[0] == 'S')
                        v_mpfi(f)->f_relaci[i].r_leer = 0;
                    else
                        v_mpfi(f)->f_relaci[i].r_leer = 1;

                    if (v_memo(mobliga)->alfabetica[0] == 's' || v_memo(mobliga)->alfabetica[0] == 'S')
                        v_mpfi(f)->f_relaci[i].r_obliga = 1;
                    else
                        v_mpfi(f)->f_relaci[i].r_obliga = 0;
                }
            }
            else
            {
                if (v_memo(mleer)->alfabetica[0] == 'X' && v_memo(mobliga)->alfabetica[0] == 'X')
                {
                    for (j = 0;j < v_mpfirel(f)->nrel;j++)
                    {
                        if (v_mpfirel(f)->numrela[j] == -(i+1))
                        {
                            v_mpfirel(f)->numrela[j] = (-v_mpfirel(f)->numrela[j])-1;
                        }
                    }
                    for (j = 0;j < v_mpfirel(f)->nreldef;j++)
                    {
                        if (v_mpfirel(f)->reldef[j].numrela == -(i+1))
                        {
                            v_mpfirel(f)->reldef[j].numrela = (-v_mpfirel(f)->reldef[j].numrela)-1;
                        }
                    }
                }
                else
                {
                    if (v_mpfi(f)->f_relaci[i].r_leer)
                    {
                        memo_strcpy(mleer,"N");
                    }
                    else
                    {
                        memo_strcpy(mleer,"S");
                    }
                    if (v_mpfi(f)->f_relaci[i].r_obliga)
                    {
                        memo_strcpy(mleer,"S");
                    }
                    else
                    {
                        memo_strcpy(mleer,"N");
                    }
                }
            }
            sprintf(v_memo(5004)->alfabetica,"%d",i+1);
        }
    }
    
    n_np = 5;
    return(0);
}

int fgv_cambio_empresa(p,tip,faclin)
int p;
int tip;
int faclin;
{
char antdir[256];
int i;
    strcpy(antdir,dirfic);
    i = strlen(dirfic);

    sprintf(v_memo(5004)->alfabetica,"%d",entra_empresa(NULL,v_memo(prog[p].indice)->alfabetica));

    reabre_los_ficheros(antdir,i);
    n_np = 1;
    return(0);
}

int fgv_enclave(p,tip,faclin)
int p;
int tip;
int faclin;
{
int f = v_prog(prog[p++].indice);
int pclave = prog[p++].indice;
int modo   = pasa_a_int( v_memo(prog[p++].indice)->numerica );
int clave = pasa_a_int( v_memo(pclave)->numerica );

    sprintf(v_memo(5004)->alfabetica,"%d",pregun_clave(&clave,f,faclin,modo));

    v_memo(pclave)->numerica = (double)clave;

    n_np = 3;
    return(0);
}

int fgv_enviafich(p,tip,faclin)
int p;
int tip;
int faclin;
{  
   int o,d;
   int sal = -1;
   
   o = prog[p].indice;
   p++;
   d = prog[p].indice;
#ifdef RWSERVER
   if (en_conexion_externa)
   {      
      char tmp[601];

	  cambiar_el_fich(v_memo( o )->alfabetica,tmp);
      /*strcpy(tmp,v_memo( o )->alfabetica);*/
      sal = strlen(tmp);
      tmp[sal] = 1;
      strcpy(tmp+sal+1,v_memo( d )->alfabetica);
      sal = rw_envia_fichero(tmp,777);      
   }
#endif
  sprintf(v_memo(5004)->alfabetica,"%d",sal);
  n_np = 2;
  return 0;
}

int fgv_recibefich(p,tip,faclin)
int p;
int tip;
int faclin;
{  
   int o,d;
   int sal = -1;
   
   o = prog[p].indice;
   p++;
   d = prog[p].indice;
#ifdef RWSERVER
         if (DespachadorExterno)
         {
            int r = atoi(v_memo(5004)->alfabetica);
            (*DespachadorExterno)((int)DESPACHO_PRE_CONTROL,r,NULL);                     
         }

   if (en_conexion_externa && DespachadorExterno)
   {      
      char tmp[601];
      strcpy(tmp,v_memo( o )->alfabetica);
      sal = strlen(tmp);
      tmp[sal] = 1;
	  cambiar_el_fich(v_memo( d )->alfabetica,tmp+sal+1);
      /*strcpy(tmp+sal+1,v_memo( d )->alfabetica);*/
      (*DespachadorExterno)((int)DESPACHO_TRAEFICHERO,0,tmp);
      sal = atoi(tmp);
   }
#endif
  sprintf(v_memo(5004)->alfabetica,"%d",sal);
  n_np = 2;
  return 0;
}

/*
                 "WORD_ABRE"        ,"9895"          ,241,NULL,
                 "WORD_SALVA"       ,"9895"          ,242,NULL,
                 "WORD_PROPIEDADES" ,"989595"        ,243,NULL,
                 "WORD_CARGA_TEXTO" ,"989595"        ,244,NULL,
                 "WORD_ASIGNA_CAMPO","989595"        ,245,NULL,
                 "WORD_IMPRIME"     ,"9895"          ,246,NULL,
*/

int fgv_abre_word(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int o,d;
   int sal = -1;
   char tmp[601];

   o = prog[p].indice;
   p++;
   d = prog[p].indice;   

   if (DespachadorExterno)
   {
       strcpy(tmp,v_memo( d )->alfabetica);
       (*DespachadorExterno)((int)DESPACHO_ENLACEWORD,0,tmp);
       sal = atoi(tmp);
   }
   v_memo( o )->numerica = (double)sal;
   
   sprintf(v_memo(5004)->alfabetica,"%d",sal);
   n_np = 2;
   return 0;
}

int fgv_salva_word(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int o,d;
   int sal = -1;
   char tmp[601];

   o = prog[p].indice;
   p++;
   d = prog[p].indice;   

   if (DespachadorExterno)
   {
       sprintf(tmp,"%.0lf",v_memo( o )->numerica);
       sal = strlen(tmp);
       tmp[sal] = 1;
       sal++;
       strcpy(tmp+sal,v_memo( d )->alfabetica);
       (*DespachadorExterno)((int)DESPACHO_ENLACEWORD,1,tmp);
       sal = atoi(tmp);
   }   
   
   sprintf(v_memo(5004)->alfabetica,"%d",sal);
   n_np = 2;
   return 0;
}

int fgv_prop_word(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int o,d,x;   
   char tmp[601];
   int sal = -1;

   o = prog[p].indice;
   p++;
   d = prog[p].indice;   
   p++;
   x = prog[p].indice;   

   tmp[0] = 0;
   if (DespachadorExterno)
   {
       sprintf(tmp,"%.0lf",v_memo( o )->numerica);
       sal = strlen(tmp);
       tmp[sal] = 1;
       sal++;
       strcpy(tmp+sal,v_memo( d )->alfabetica);
       sal = strlen(tmp);
       tmp[sal] = 1;
       sal++;
       strcpy(tmp+sal,v_memo( x )->alfabetica);
       (*DespachadorExterno)((int)DESPACHO_ENLACEWORD,2,tmp);       
   }   
   strcpy(v_memo(5004)->alfabetica,tmp);
   n_np = 3;
   return 0;
}


int fgv_carga_txt_word(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int o,d,x;
   int sal = -1;
   char tmp[601];

   o = prog[p].indice;
   p++;
   d = prog[p].indice;   
   p++;
   x = prog[p].indice;   

   if (DespachadorExterno)
   {
       sprintf(tmp,"%.0lf",v_memo( o )->numerica);
       sal = strlen(tmp);
       tmp[sal] = 1;
       sal++;
       strcpy(tmp+sal,v_memo( d )->alfabetica);
       sal = strlen(tmp);
       tmp[sal] = 1;
       sal++;
       strcpy(tmp+sal,v_memo( x )->alfabetica);
       (*DespachadorExterno)((int)DESPACHO_ENLACEWORD,3,tmp);
       sal = atoi(tmp);
   }   
   
   sprintf(v_memo(5004)->alfabetica,"%d",sal);
   n_np = 3;
   return 0;
}


int fgv_asigna_campo_word(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int o,d,x;
   int sal = -1;
   char tmp[601];

   o = prog[p].indice;
   p++;
   d = prog[p].indice;   
   p++;
   x = prog[p].indice;   

   if (DespachadorExterno)
   {
       sprintf(tmp,"%.0lf",v_memo( o )->numerica);
       sal = strlen(tmp);
       tmp[sal] = 1;
       sal++;
       strcpy(tmp+sal,v_memo( d )->alfabetica);
       sal = strlen(tmp);
       tmp[sal] = 1;
       sal++;
       strcpy(tmp+sal,v_memo( x )->alfabetica);
       (*DespachadorExterno)((int)DESPACHO_ENLACEWORD,4,tmp);
       sal = atoi(tmp);
   }   
   
   sprintf(v_memo(5004)->alfabetica,"%d",sal);
   n_np = 3;
   return 0;
}


int fgv_asigna_texto_word(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int o,d,x;
   int n,i;
   int sal = -1;
   char tmp[10000];

   o = prog[p].indice;
   p++;
   d = prog[p].indice;   
   p++;
   x = prog[p].indice;   

   if (DespachadorExterno)
   {
       sprintf(tmp,"%.0lf",v_memo( o )->numerica);
       sal = strlen(tmp);
       tmp[sal] = 1;
       sal++;
       strcpy(tmp+sal,v_memo( d )->alfabetica);
       sal = strlen(tmp);
       tmp[sal] = 1;
       sal++;
	   if (x < 0)
	   {
		   x = (-x) - 1;
		   x = pasa_a_int(v_memo( x )->numerica);
		   n = atoi(v_memo( d )->alfabetica);
		   for (i = 0;i < (n*2);i++)
		   {			   
			   if (que_memo_tipo(x+i) == DOUBLETIPO)
			   {
				   sprintf(tmp+sal,"%lf",v_memo( x+i )->numerica);
			   }
			   else
			   {
				   strcpy(tmp+sal,v_memo( x+i )->alfabetica);
			   }
			   sal += strlen(tmp+sal);
			   tmp[sal] = 2;
			   sal++;
			   i++;
			   if (que_memo_tipo(x+i) == DOUBLETIPO)
			   {
				   sprintf(tmp+sal,"%lf",v_memo( x+i )->numerica);
			   }
			   else
			   {
				   strcpy(tmp+sal,v_memo( x+i )->alfabetica);
			   }
			   sal += strlen(tmp+sal);
			   if (i < ((n*2)-1))
			   {
				   tmp[sal] = 2;
				   sal++;
			   }
		   }
	   }
	   else
	   {
		   if (que_memo_tipo(x) == DOUBLETIPO)
		   {
			   sprintf(tmp+sal,"%lf",v_memo( x )->numerica);
		   }
		   else
		   {
			   strcpy(tmp+sal,v_memo( x )->alfabetica);
		   }
	   }

       (*DespachadorExterno)((int)DESPACHO_ENLACEWORD,6,tmp);
       sal = atoi(tmp);
   }   
   
   sprintf(v_memo(5004)->alfabetica,"%d",sal);
   n_np = 3;
   return 0;
}

int fgv_imprime_word(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int o,d;
   int sal = -1;
   char tmp[601];

   o = prog[p].indice;
   p++;
   d = prog[p].indice;   

   if (DespachadorExterno)
   {
       sprintf(tmp,"%.0lf",v_memo( o )->numerica);
       sal = strlen(tmp);
       tmp[sal] = 1;
       sal++;
       strcpy(tmp+sal,v_memo( d )->alfabetica);
       (*DespachadorExterno)((int)DESPACHO_ENLACEWORD,5,tmp);
       sal = atoi(tmp);
   }   
   
   sprintf(v_memo(5004)->alfabetica,"%d",sal);
   n_np = 2;
   return 0;
}


int fgvfocoteclado(p,tip,faclin)
int p;
int tip;
int faclin;
{
    n_np = 1;
    return 0;
}


int fgvdestruyecontrol(p,tip,faclin)
int p;
int tip;
int faclin;
{
    n_np = 1;
    return 0;
}

int fgvrefrescacontrol(p,tip,faclin)
int p;
int tip;
int faclin;
{
    n_np = 1;
    return 0;
}

extern void (*RetroFuncion[512])(char *bloque,char *par,int el_punto);
extern int RetroTipo[512];
extern char *RetroPar[512];
extern int RetroPunto;

typedef struct _control_pars_
{
    int id;
    int evento;
    int Independiente;
} control_pars;

void FGVHookControl(char *bloque,control_pars *par,int el_punto)
{
    int operacion;

    if (!par || RetroPar[el_punto] != (char *)par) return;

    bloque += (sizeof(long)*3);

	memcpy((char *)&operacion,bloque,sizeof(int));
	bloque += sizeof(int);

    if (operacion == 999)
    {
        if (par->Independiente)
        {
            free((char *)par);
        }
        FinRetroPar(el_punto);
        return;
    }

    if (par->evento)
    {
        sprintf(v_memo(5003)->alfabetica,"%d",operacion);
        sprintf(v_memo(5004)->alfabetica,"%d",par->id);
        p_ejecuta(par->evento,O_NOR,0,15);
    }
}



int fgvcontrolsimple(p,tip,faclin)
int p;
int tip;
int faclin;
{

   int *tmpxx;
   char dato[1024];   
   char *nom;   
   int evento;
   int localizador;
   control_pars *par = NULL;

   /*"CONTROL_SIMPLE",  "9895989870"       ,254,47,*/

   if (DespachadorExterno)
   {
	  int pan,i;
	  int oipfi;
	  int n;

	  tmpxx = (int *)dato;
      tmpxx[0] = 0/*default_id*/;
      tmpxx[1] = (int)v_memo(prog[p++].indice)->numerica;
      nom = v_memo(prog[p++].indice)->alfabetica;
      tmpxx[2] = (int)v_memo(prog[p++].indice)->numerica;
      tmpxx[3] = (int)v_memo(prog[p++].indice)->numerica;
      evento = prog[p++].indice;
      if (evento >= 0)
         evento = en_pr[programa_activo].tabla_gosubs[evento];
      else
         evento = 0;
      if (evento)
      {
          tmpxx[4] = 0;
          localizador = DameRetroPar();
          RetroFuncion[localizador] = FGVHookControl;
		  RetroTipo[localizador] = 4;
          par = (control_pars *)malloc(sizeof(control_pars));
          memset((void *)par,0,sizeof(control_pars));
          par->evento = evento;
          par->Independiente = 1;
          RetroPar[localizador] = (char *)par;
          tmpxx[5] = localizador;
      }
      else
      {
          tmpxx[4] = 1;
          tmpxx[5] = atoi(v_memo(5004)->alfabetica);
      }     
      oipfi = (tmpxx[1] % 1000) - 1;
	  pan = tmpxx[1] / 1000;
	  if (oipfi >= 0 && oipfi < nfi)
	  {
	      n = v_mpfi(oipfi)->f_ncampos;	  
		  tmpxx[6] = -(n+1);
		  tmpxx[7] = oipfi;
	  }
	  else
	  {
		  tmpxx[6] = tmpxx[5];
		  tmpxx[7] = tmpxx[1];		  
	  }
      strcpy((char *)(tmpxx+8),nom);
      (*DespachadorExterno)((int)DESPACHO_MI_CONTROL,-1,(void *)dato);

	  /* ojo aqui vuelven en 0 los resultados no en 4 */
      if (par)
      {
          par->id = tmpxx[0];
      }
      sprintf(v_memo(5004)->alfabetica,"%d",tmpxx[0]);      
   }
   n_np = 5;
   return 0;
}

int fgvbotonsalida(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int tecla = 807;
   /*v_memo(prog[p++].indice)->numerica;*/
   sprintf(v_memo(5004)->alfabetica,"%d",tecla);
   n_np = 5;
   return 0;
}


#define MAXBUFFERS 30
extern char **buff_ers[MAXBUFFERS];
extern unsigned char tip_buf[MAXBUFFERS];

int fgvocxregistra(p,tip,faclin)
int p;
int tip;
int faclin;
{
    /*"OCX_REGISTRA"  ,  "95"             ,254,49,*/
    char tmp[512];
    long modo = 1;
    char *nome = v_memo(prog[p++].indice)->alfabetica;
    long tam = (sizeof(long)*10)+strlen(nome)+1;
    long id_ocx = 0; /* registro */
    long operacion = 0; /* registro */
    long did = 0/*default_id*/;
    long sid = 0L;
    long pi = -1;
    long pf = -1;

    
    memcpy(tmp,&tam,sizeof(long));
    memcpy(tmp+sizeof(long),&tam,sizeof(long));
    memcpy(tmp+(sizeof(long)*2),&modo,sizeof(long));
    memcpy(tmp+(sizeof(long)*3),&id_ocx,sizeof(long));
    memcpy(tmp+(sizeof(long)*4),&operacion,sizeof(long));
    memcpy(tmp+(sizeof(long)*5),&did,sizeof(long));
    memcpy(tmp+(sizeof(long)*6),&sid,sizeof(long));
    memcpy(tmp+(sizeof(long)*7),&pi,sizeof(long));
    memcpy(tmp+(sizeof(long)*8),&pf,sizeof(long));
    strcpy(tmp+(sizeof(long)*9),nome);
    (*DespachadorExterno)((int)DESPACHO_BINARIO,(int)10,tmp);
    memcpy(&id_ocx,tmp+(sizeof(long)*3),sizeof(long));
    sprintf(v_memo(5004)->alfabetica,"%d",id_ocx);
    n_np = 1;
    return 0;
}

int fgvocxactiva(p,tip,faclin)
int p;
int tip;
int faclin;
{
    /*"OCX_ACTIVA"    ,  "9598"           ,254,50,*/
    char tmp[512];
    long modo = 1;
    char *nome = v_memo(prog[p++].indice)->alfabetica;
    long tam = (sizeof(long)*10)+strlen(nome)+1;
    long id_ocx = 0; 
    long operacion = 1; /* activacion */
    long did = 0 /*default_id*/;
    long sid = 0L;
    long pi = -1;
    long pf = -1;
    
    memcpy(tmp,&tam,sizeof(long));
    memcpy(tmp+sizeof(long),&tam,sizeof(long));
    memcpy(tmp+(sizeof(long)*2),&modo,sizeof(long));
    memcpy(tmp+(sizeof(long)*3),&id_ocx,sizeof(long));
    memcpy(tmp+(sizeof(long)*4),&operacion,sizeof(long));
    memcpy(tmp+(sizeof(long)*5),&did,sizeof(long));
    memcpy(tmp+(sizeof(long)*6),&sid,sizeof(long));
    memcpy(tmp+(sizeof(long)*7),&pi,sizeof(long));
    memcpy(tmp+(sizeof(long)*8),&pf,sizeof(long));
    strcpy(tmp+(sizeof(long)*9),nome);
    (*DespachadorExterno)((int)DESPACHO_BINARIO,(int)10,tmp);
    memcpy(&id_ocx,tmp+(sizeof(long)*3),sizeof(long));
    v_memo(prog[p++].indice)->numerica = (double)id_ocx;
    sprintf(v_memo(5004)->alfabetica,"%d",id_ocx);
    n_np = 2;
    return 0;
}

int fgvocxlibera(p,tip,faclin)
int p;
int tip;
int faclin;
{
    /*"OCX_LIBERA"    ,  "98"             ,254,51,*/
    char tmp[512];
    long modo = 1;
    long tam = (sizeof(long)*10);
    long id_ocx = (long)v_memo(prog[p++].indice)->numerica;
    long operacion = 2; /* libera */
    long did = 0 /*default_id*/;
    long sid = 0L;
    long pi = -1;
    long pf = -1;
    
    memcpy(tmp,&tam,sizeof(long));
    memcpy(tmp+sizeof(long),&tam,sizeof(long));
    memcpy(tmp+(sizeof(long)*2),&modo,sizeof(long));
    memcpy(tmp+(sizeof(long)*3),&id_ocx,sizeof(long));
    memcpy(tmp+(sizeof(long)*4),&operacion,sizeof(long));    
    memcpy(tmp+(sizeof(long)*5),&did,sizeof(long));
    memcpy(tmp+(sizeof(long)*6),&sid,sizeof(long));
    memcpy(tmp+(sizeof(long)*7),&pi,sizeof(long));
    memcpy(tmp+(sizeof(long)*8),&pf,sizeof(long));

    (*DespachadorExterno)((int)DESPACHO_BINARIO,(int)10,tmp);
    memcpy(&id_ocx,tmp+(sizeof(long)*3),sizeof(long));
    sprintf(v_memo(5004)->alfabetica,"%d",id_ocx);
    n_np = 1;
    return 0;
}

int fgvocxinvoca(p,tip,faclin)
int p;
int tip;
int faclin;
{
    /*"OCX_INVOCA"    ,  "989598989898"       ,254,52,*/
    char tmp[12048];
    long modo = 1;
    long id_ocx = (long)v_memo(prog[p++].indice)->numerica; 
    char *nome = v_memo(prog[p++].indice)->alfabetica;
    long tam = (sizeof(long)*10)+strlen(nome)+2;
    long operacion = 3; /* invocacion */
    char *px;
    int *p1;
    long did = 0 /*default_id*/;
    long sid = 0L;
    long pi = (int)v_memo(prog[p++].indice)->numerica;
    long pf = (int)v_memo(prog[p++].indice)->numerica;
    int pars = (int)v_memo(prog[p++].indice)->numerica;
    int puntero = (int)v_memo(prog[p++].indice)->numerica;
    int i,j,ibuf,t;
    int hay;
    char **txt;
    
    memcpy(tmp+(sizeof(long)*2),&modo,sizeof(long));
    memcpy(tmp+(sizeof(long)*3),&id_ocx,sizeof(long));
    memcpy(tmp+(sizeof(long)*4),&operacion,sizeof(long));
    memcpy(tmp+(sizeof(long)*5),&did,sizeof(long));
    memcpy(tmp+(sizeof(long)*6),&sid,sizeof(long));
    memcpy(tmp+(sizeof(long)*7),&pi,sizeof(long));
    memcpy(tmp+(sizeof(long)*8),&pf,sizeof(long));
    tmp[ sizeof(long)*9 ] = strlen(nome)+1;
    strcpy(tmp+(sizeof(long)*9)+1,nome);
    px = tmp+(sizeof(long)*9)+1+tmp[sizeof(long)*9];
    p1 = (int *)px;
    *p1 = pars;
    p1++;
    tam += sizeof(int);
    for (i = 0;i < pars;i++)
    {
        if (que_memo_tipo(puntero+i*2) == DOUBLETIPO)
        {
            p1[i] = (int)v_memo(puntero+i*2)->numerica;
        }
        else
        {
            p1[i] = 0;
        }
        tam += sizeof(int);
    }
    px = (char *)(p1+pars);
    /* (2)numerica = double , (1)alfa = 250 char (3)texto_buf = 250 * n (0)nada*/
    for (i = 0;i < pars;i++)
    {
        switch(p1[i])
        {
        case 2:
           if (que_memo_tipo(puntero+i*2+1) == DOUBLETIPO)
           {
               *((double *)px) = v_memo(puntero+i*2+1)->numerica;
               px += sizeof(double);
               tam += sizeof(double);
           }
           else
           {
               p1[i] = 0;
           }
           break;
        case 1:
           if (que_memo_tipo(puntero+i*2+1) == CHARTIPO || que_memo_tipo(puntero+i*2+1) == FECHATIPO)
           {
               strcpy(px,v_memo(puntero+i*2+1)->alfabetica);
               px += 251;
               tam += 251;
           }
           else
           {
               p1[i] = 0;
           }               
           break;
        case 3:
           p1[i] = 0;
           if (que_memo_tipo(puntero+i*2+1) == DOUBLETIPO)
           {
               ibuf = (int)v_memo(puntero+i*2+1)->numerica;
               if (ibuf >= 0 && ibuf < MAXBUFFERS || buff_ers[ibuf] || tip_buf[ibuf] == CHARTIPO) 
               {
                   p1[i] = 3;
                   hay = *((int *)(buff_ers[ibuf]));
                   *((int *)px) = hay * 251;
                   px += sizeof(int);
                   tam += sizeof(int) + hay * 251;

                   txt = (char **)(((char *)(buff_ers[ibuf])) + sizeof(int));
                   for (j = 0;j < hay;j++)
                   {                       
                       if (txt[j])
                       {
                           t = strlen(txt[j]);
                           strcpy(px,txt[j]);
                           px[t] = 13;
                           px[t+1] = 10;
                           px[t+2] = 0;
                           px += (t+2);
                       }
                       else
                       {
                           *px = 13;
                           *(px+1) = 10;
                           *(px+2) = 0;
                           px += 2;
                       }
                   }
               }
           }
           break;
        default:
           break;
        }
    }

    memcpy(tmp,&tam,sizeof(long));
    memcpy(tmp+sizeof(long),&tam,sizeof(long));
    (*DespachadorExterno)((int)DESPACHO_BINARIO,(int)10,tmp);
    memcpy(&id_ocx,tmp+(sizeof(long)*3),sizeof(long));    
    sprintf(v_memo(5004)->alfabetica,"%d",id_ocx);

    n_np = 6;
    return 0;
}

int fgv_maincommand(p,tip,faclin)
int p;
int tip;
int faclin;
{
   n_np = 6;
   return 0;
}

int fgvsubpinpa(p,tip,faclin)
int p;
int tip;
int faclin;
{   
   n_np = 2;
   return(0);
}

int fgvborrahijospan(p,tip,faclin)
int p;
int tip;
int faclin;
{      
    
    return(0);
}



int fgvestadocontrol(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int a,n;
   if (DespachadorExterno)
   {
	   n = (int)v_memo(prog[p++].indice)->numerica;
	   if (n >= 0)
	   {
		  n--;
	   }
	   a = prog[p++].indice;
	   (*DespachadorExterno)((int)DESPACHO_ESTADOCONTROL,n,v_memo(a)->alfabetica);
   }
   n_np = 2;
   return 0;
}

int fi_calcula_md5(int i,unsigned char digest[16])
{
char *buffer;
MD5_CTX context;
long pos;
int r;

   pos = rw_base_lseek_(i,0L,SEEK_CUR);
   memset(digest,0,16);
   buffer = malloc(102400);
   rw_base_lseek_(i,0L,SEEK_SET);
   MD5Init(&context);
   while((r = rw_base_read_(i,buffer,102400)) > 0)
   {
	   MD5Update (&context, buffer, r);
	   if (r < 102400)
		   break;
   }
   free(buffer);	   
   MD5Final (digest,&context);
   rw_base_lseek_(i,pos,SEEK_SET);

   return 0;
}


int calcula_md5(char *file,char *resultado)
{
	   unsigned char digest[16];
	   int i,r;
	   
	   memset(resultado,0,33);
	   i = rw_base_open_(file,O_RDONLY | O_BINARY);
	   if (i >= 0)
	   {
		   if (!fi_calcula_md5(i,digest))
		   {
			   for (r = 0;r < 16;r++)
			   {
				   sprintf(resultado+r*2,"%02x",digest[r]);
			   }
		   }
		   rw_base_close_(i);
		   return 0;
	   }
	   return -1;
}

int rw_skip(p,tip,faclin)
int p;
int tip;
int faclin;
{
   /* pasar */
   n_np = 1;
   return(0);
}


static void InstalaPython()
{
	char tmp[512];
	char tmp2[512];
	FILE *f;
	unsigned long tam;


	strcpy(tmp,dirbass);
    strcat(tmp,"python/python.ver");
	
	f = fopen(tmp,"r");
	if (f)
	{
		fclose(f);
		return;
	}


	tam = 3;
	(*DespachadorExterno)((int)DESPACHO_PROGRESO,0,(void *)&tam);
#ifdef RWSERVER
	if (en_conexion_externa)
	{
		pulsatecla();
	}
#endif


	crea_dir(tmp);

	tam = 1;
	(*DespachadorExterno)((int)DESPACHO_PROGRESO,1,(void *)&tam);

#ifdef RWSERVER
	if (en_conexion_externa)
	{
		pulsatecla();
	}
#endif


#ifdef FGVLINUX
	sprintf(tmp2,"%sagitool/dswget.fedora -O %spython/python.ver http://www.diagram.es/basico/python/python.ver",dirbass,dirbass);
#else
#ifdef ESDOS
	sprintf(tmp2,"%sagitool/dswget.exe -O %spython/python.ver http://www.diagram.es/basico/python/python.ver",dirbass,dirbass);
#else
	sprintf(tmp2,"%sagitool/dswget -O %spython/python.ver http://www.diagram.es/basico/python/python.ver",dirbass,dirbass);
#endif
#endif
	agi_system(tmp2);
	f = fopen(tmp,"r");
	if (!f)
	{
		/* error de descarga */
		return;
	}
	fclose(f);

	tam = 2;
	(*DespachadorExterno)((int)DESPACHO_PROGRESO,1,(void *)&tam);

#ifdef RWSERVER
	if (en_conexion_externa)
	{
		pulsatecla();
	}
#endif

	strcpy(tmp,dirbass);
    strcat(tmp,"agitool/python.tgz");

	unlink(tmp);

#ifdef FGVLINUX
	sprintf(tmp2,"%sagitool/dswget.fedora -O %sagitool/python.tgz http://www.diagram.es/basico/python/python.tgz",dirbass,dirbass);
#else
#ifdef ESDOS
	sprintf(tmp2,"%sagitool/dswget.exe -O %sagitool/python.tgz http://www.diagram.es/basico/python/python.tgz",dirbass,dirbass);
#else
	sprintf(tmp2,"%sagitool/dswget -O %sagitool/python.tgz http://www.diagram.es/basico/python/python.tgz",dirbass,dirbass);
#endif
#endif
	agi_system(tmp2);

	f = fopen(tmp,"r");
	if (!f)
	{
		/* error de descarga */
		strcpy(tmp,dirbass);
		strcat(tmp,"python/python.ver");
		unlink(tmp);
		return;
	}
	fclose(f);
	(*DespachadorExterno)((int)DESPACHO_PROGRESO,2,NULL);
#ifdef RWSERVER
	if (en_conexion_externa)
	{
		pulsatecla();
	}
#endif
	/* todo ok */
}


int graba_def_a_ttx(FI_DEF *pfi) {
	int i;
	char tmp[512];
	char tmp2[512];	
	FILE *fout;
	strcpy(tmp,pfi->f_fichero);			
	strcat(tmp,".ttx");
	fout = fopen(tmp,"w");
	if (fout) {
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
			switch(pfi->f_campos[i].c_tipo)
			{
			case 3:
				strcpy(tmp2,"Number");
				break;
			case 1:			
			case 2:
				strcpy(tmp2,"Number");
				break;
			
			case 5:
				strcpy(tmp2,"Date");
				break;
			default:
				sprintf(tmp2,"String%c%d",9,pfi->f_campos[i].c_qc);
				break;
			}		
			fprintf(fout,"%s%c%s\r\n",tmp,9,tmp2);
		}		
		fclose(fout);
	}
	return 0;
}


int graba_def_a_xml(FI_DEF *pfi) {
	char tmp[512];
	char tmp2[512];
	unsigned char *pp;
	long pos = 1L;
	int i;
	FILE *fout;

	pfi->f_fd = abre_directo(pfi,pfi->f_fichero);
	if (pfi->f_fd < 0)
		return -1;

	strcpy(tmp,pfi->f_fichero);
	strcat(tmp,".xml");
	fout = fopen(tmp,"w");
	if (fout) {
		fprintf(fout,"<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>\r\n");
		fprintf(fout,"<%s>\r\n",quita_path(pfi->f_fichero));
		while(lee_directo(pfi->f_fd,pos,pfi,pfi->f_buf) >= 0) {
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
				/*
				if (!strcmp(tmp,"_key_dbf"))
					continue;
				*/
				ld_campo(i,pfi,tmp2);
				for (pp = (unsigned char *)tmp2;*pp;pp++)
				   oem_a_ansi(pp);
				fprintf(fout,"<%s>%s</%s>\r\n",tmp,tmp2,tmp);
			}			
			pos++;
		}
		fprintf(fout,"</%s>\r\n",quita_path(pfi->f_fichero));
		fclose(fout);

		strcpy(tmp,pfi->f_fichero);
		strcat(tmp,".xsd");
		fout = fopen(tmp,"w");
		if (fout) {
			fprintf(fout,"<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>\r\n");
			fprintf(fout,"<xsd:schema xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">\r\n");
			fprintf(fout,"<xsd:annotation>\r\n");
			fprintf(fout,"<xsd:documentation xml:lang=\"es\">XML Schema - %s .</xsd:documentation>\r\n",quita_path(pfi->f_fichero));
			fprintf(fout,"</xsd:annotation>\r\n");	


			fprintf(fout,"<xsd:element name=\"%s\" minOccurs=\"0\" maxOccurs=\"unbounded\">\r\n",quita_path(pfi->f_fichero));
			fprintf(fout,"<xsd:complexType>\r\n");
			fprintf(fout,"<xsd:sequence>\r\n");

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
				fprintf(fout,"<xsd:element name=\"%s\">\r\n",tmp);
				fprintf(fout,"<xsd:annotation>\r\n");
				fprintf(fout,"<xsd:documentation xml:lang=\"es\">%s</xsd:documentation>\r\n",tmp2);
				fprintf(fout,"</xsd:annotation>\r\n");
				fprintf(fout,"<xsd:simpleType>\r\n");
				

				switch(pfi->f_campos[i].c_tipo)
				{
				case 3:
					fprintf(fout,"<xsd:restriction base=\"xsd:double\" >");
					fprintf(fout,"</xsd:restriction>\r\n");
					break;
				case 1:			
				case 2:
					fprintf(fout,"<xsd:restriction base=\"xsd:integer\" >");
					fprintf(fout,"</xsd:restriction>\r\n");
					break;
				
				case 5:
					fprintf(fout,"<xsd:restriction base=\"xsd:date\" >");
					fprintf(fout,"</xsd:restriction>\r\n");
					break;
				
				default:
					fprintf(fout,"<xsd:restriction base=\"xsd:string\" >\r\n");
					fprintf(fout,"<xsd:minLength value=\"0\"/>\r\n");								
					fprintf(fout,"</xsd:restriction>\r\n");
					break;
				}		

				fprintf(fout,"</xsd:simpleType>\r\n</xsd:element>\r\n");
			}

			fprintf(fout,"</xsd:sequence>\r\n");
			fprintf(fout,"</xsd:complexType>\r\n");
			fprintf(fout,"</xsd:element>\r\n");
			fprintf(fout,"</xsd:schema>\r\n");
			fclose(fout);
		}
	}
	cierra_directo(pfi->f_fd);
	return 0;
}


int fgv_especifica(p,tip,faclin)
int p;
int tip;
int faclin;
{
   char tmp[1048];
   int i;
   int r = -1;
   char *ins = v_memo(prog[p].indice)->alfabetica;
   p++;   
   i = pasa_a_int(v_memo(prog[p].indice)->numerica);


   if (!strcmp(ins,"VERSIONBASICO"))
   {
	   memo_strcpy(i,rw_version);
	   r = fgv_internal_build;
   }
   else if (!strcmp(ins,"INTERFACE_VISUAL_PYTHON"))
   {
	     int modo = atoi(v_memo(i++)->alfabetica);		 
		 int j;
#ifdef RWSERVER
		 extern char remoto_dirbass[51];
	     strcpy(tmp,remoto_dirbass);
#else
	     strcpy(tmp,dirbass);
#endif
         strcat(tmp,"python/dspython.dll");
		 j = strlen(tmp);
		 tmp[j++] = 1;
		 strcpy(tmp+j,v_memo(i++)->alfabetica);
		 j += strlen(tmp+j);
		 tmp[j++] = 1;
		 strcpy(tmp+j,v_memo(i++)->alfabetica);
		 j += strlen(tmp+j);
		 tmp[j++] = 1;
		 tmp[j++] = 0;

		 /*
		 memcpy(tmp+j,default_id);
		 j += sizeof(int);
		 memcpy(tmp+j,default_sid);
		 j += sizeof(int);
		 memcpy(tmp+j,default_sid_padre);
		 j += sizeof(int);
		 memcpy(tmp+j,default_wid);
		 j += sizeof(int);
		 */
		 InstalaPython();

	     r = CargaVInterface(tmp,(unsigned long)( (modo & 0x7fffffff) | 0x40000000 ));
   }   
   else if (!strcmp(ins,"POW"))
   {
	   double res,par1;
	   if (que_memo_tipo(i+1) == DOUBLETIPO)
	   {
		   par1 = (double)v_memo(i+1)->numerica;
	   }
	   else
	   {
		   par1 = (double)atof(v_memo(i+1)->alfabetica);
	   }
	   if (que_memo_tipo(i) == DOUBLETIPO)
	   {
		   res = pow((double)v_memo(i)->numerica,par1);
	   }
	   else
	   {
		   res = pow((double)atof(v_memo(i)->alfabetica),par1);
	   }
	   i++;
	   if (que_memo_tipo(i) == DOUBLETIPO)
	   {
		   v_memo(i)->numerica = res;
	   }
	   else
	   {
		   sprintf(tmp,"%lf",res);
		   memo_strcpy(i,tmp);
	   }	   
	   sprintf(v_memo(5004)->alfabetica,"0");
	   n_np = 2;
	   return 0;
   }
   else if (!strcmp(ins,"SQRT"))
   {
	   double res;
	   if (que_memo_tipo(i) == DOUBLETIPO)
	   {
		   res = sqrt((double)v_memo(i)->numerica);
	   }
	   else
	   {
		   res = sqrt((double)atof(v_memo(i)->alfabetica));
	   }
	   i++;
	   if (que_memo_tipo(i) == DOUBLETIPO)
	   {
		   v_memo(i)->numerica = res;
	   }
	   else
	   {
		   sprintf(tmp,"%lf",res);
		   memo_strcpy(i,tmp);
	   }	   
	   sprintf(v_memo(5004)->alfabetica,"0");
	   n_np = 2;
	   return 0;
   }   
   else if (!strcmp(ins,"LOG"))
   {
	   double res;
	   if (que_memo_tipo(i) == DOUBLETIPO)
	   {
		   res = log10((double)v_memo(i)->numerica);
	   }
	   else
	   {
		   res = log10((double)atof(v_memo(i)->alfabetica));
	   }
	   i++;
	   if (que_memo_tipo(i) == DOUBLETIPO)
	   {
		   v_memo(i)->numerica = res;
	   }
	   else
	   {
		   sprintf(tmp,"%lf",res);
		   memo_strcpy(i,tmp);
	   }	   
	   sprintf(v_memo(5004)->alfabetica,"0");
	   n_np = 2;
	   return 0;
   }
   else if (!strcmp(ins,"LN"))
   {
	   double res;
	   if (que_memo_tipo(i) == DOUBLETIPO)
	   {
		   res = log((double)v_memo(i)->numerica);
	   }
	   else
	   {
		   res = log((double)atof(v_memo(i)->alfabetica));
	   }
	   i++;
	   if (que_memo_tipo(i) == DOUBLETIPO)
	   {
		   v_memo(i)->numerica = res;
	   }
	   else
	   {
		   sprintf(tmp,"%lf",res);
		   memo_strcpy(i,tmp);
	   }	   
	   sprintf(v_memo(5004)->alfabetica,"0");
	   n_np = 2;
	   return 0;
   }
   else if (!strcmp(ins,"XOR"))
   {
	   unsigned long res;
	   if (que_memo_tipo(i) == DOUBLETIPO)
	   {
		   res = (unsigned long)v_memo(i)->numerica;
	   }
	   else
	   {
		   res = (unsigned long)atol(v_memo(i)->alfabetica);
	   }
	   i++;
	   if (que_memo_tipo(i) == DOUBLETIPO)
	   {
		   res ^= (unsigned long)v_memo(i)->numerica;
	   }
	   else
	   {
		   res ^= (unsigned long)atol(v_memo(i)->alfabetica);
	   }	   
	   sprintf(v_memo(5004)->alfabetica,"%lu",res);
	   n_np = 2;
	   return 0;
   }
   else if (!strcmp(ins,"TCP"))
   {
	   if (!strcmp(v_memo(i)->alfabetica,"CONECTA"))
	   {		   
		   r = ds_tcp_conecta(v_memo(i+1)->alfabetica,atoi(v_memo(i+2)->alfabetica));
	   }
	   else
	   if (!strcmp(v_memo(i)->alfabetica,"ENVIA_S"))
	   {		   
		   r = ds_tcp_envia_s(atoi(v_memo(i+1)->alfabetica),v_memo(i+2)->alfabetica,atoi(v_memo(i+3)->alfabetica));
	   }
	   else
	   if (!strcmp(v_memo(i)->alfabetica,"CUANTO"))
	   {
		   r = ds_tcp_cuanto(atoi(v_memo(i+1)->alfabetica));
	   }
	   else
	   if (!strcmp(v_memo(i)->alfabetica,"RECIBE_S"))
	   {		   
		   r = ds_tcp_recibe_s(atoi(v_memo(i+1)->alfabetica),tmp,atoi(v_memo(i+3)->alfabetica));
		   if (r >= 0)
		   {
			   tmp[r] = 0;
			   memo_strcpy(i+2,tmp);
		   }
	   }
	   else
	   if (!strcmp(v_memo(i)->alfabetica,"CIERRA"))
	   {		   
		   r = ds_tcp_envia_s(atoi(v_memo(i+1)->alfabetica));
	   }
	   sprintf(v_memo(5004)->alfabetica,"%d",r);
	   n_np = 2;
	   return 0;
   }
   else if (!strcmp(ins,"MACADDRESS"))
   {
	   unsigned char la_ethernet_[6];
#ifdef RWXARXA
#ifdef RWSERVER
	   if (!en_conexion_externa)
		   dame_ethernet_address(la_ethernet_);
#else
	   dame_ethernet_address(la_ethernet_);
#endif
#endif
	   /*
	   unsigned char eth[6];
	   int j;
	   memset(eth,0,6);
	   dame_ethernet_address(eth);
	   */	   
	   sprintf(tmp,"%02X:%02X:%02X:%02X:%02X:%02X",la_ethernet_[0],la_ethernet_[1],la_ethernet_[2],la_ethernet_[3],la_ethernet_[4],la_ethernet_[5]);
	   memo_strcpy(i,tmp);
	   r = 0;
   }
   else if (!strcmp(ins,"ODB_DSN"))
   {
#ifdef FGVDOS
	   sqlsrv_cierra_todo(0);
	   if (sqlsrv_conexion)
		   free(sqlsrv_conexion);
	   sqlsrv_conexion = malloc(strlen(v_memo(i)->alfabetica)+1);
	   strcpy(sqlsrv_conexion,v_memo(i)->alfabetica);
	   r = 0;
#endif
   }
   else if (!strcmp(ins,"TCP"))
   {
	   char *fun = v_memo(i)->alfabetica;
	   if (!strcmp(fun,"socket"))
	   {
		   r = dstcp_asigna_socket(v_memo(i+1)->alfabetica,v_memo(i+2)->alfabetica);
	   }
	   else if (!strcmp(fun,"closesocket"))
	   {
		   r = dstcp_libera_socket(atoi(v_memo(i+1)->alfabetica));
	   }
	   else if (!strcmp(fun,"connect"))
	   {
		   r = dstcp_connect_socket(atoi(v_memo(i+1)->alfabetica),v_memo(i+2)->alfabetica,atoi(v_memo(i+3)->alfabetica));
	   }
	   else if (!strcmp(fun,"bind"))
	   {
		   r = dstcp_bind_socket(atoi(v_memo(i+1)->alfabetica),v_memo(i+2)->alfabetica,atoi(v_memo(i+3)->alfabetica));
	   }
	   else if (!strcmp(fun,"listen"))
	   {
		   r = dstcp_listen_socket(atoi(v_memo(i+1)->alfabetica),atoi(v_memo(i+2)->alfabetica));
	   }
	   else if (!strcmp(fun,"select"))
	   {
		   int n1,n2,j,k;
		   int s[256];
		   char tmp[32];
		   n1 = atoi(v_memo(i+2)->alfabetica);
		   n2 = atoi(v_memo(i+3)->alfabetica);
		   for (j = 0;j < n1;j++)
		   {
			   s[j] = atoi(v_memo(i+4+j)->alfabetica);
		   }
		   for (k = 0;k < n2;k++)
		   {
			   s[j] = atoi(v_memo(i+4+j)->alfabetica);
			   j++;
		   }
		   r = dstcp_select_socket(atoi(v_memo(i+1)->alfabetica),&n1,&n2,s);
		   sprintf(tmp,"%d",n1);
		   memo_strcpy(i+2,tmp);
		   sprintf(tmp,"%d",n2);
		   memo_strcpy(i+3,tmp);
		   for (j = 0;j < n1;j++)
		   {
			   sprintf(tmp,"%d",s[j]);
			   memo_strcpy(i+4+j,tmp);
		   }
		   for (k = 0;k < n2;k++)
		   {
			   sprintf(tmp,"%d",s[j]);
			   memo_strcpy(i+4+j,tmp);
			   j++;
		   }		   
	   }
	   else if (!strcmp(fun,"accept"))
	   {
		   char *dest = NULL;
		   char *port = NULL;
		   r = dstcp_accept_socket(atoi(v_memo(i+1)->alfabetica),atoi(v_memo(i+2)->alfabetica),&dest,&port);
		   if (dest)
		   {
			   memo_strcpy(i+3,dest);
			   free(dest);
		   }
		   if (port)
		   {
			   memo_strcpy(i+4,port);
			   free(port);
		   }
	   }
	   else if (!strcmp(fun,"send_string"))
	   {
		   r = dstcp_sendstring_socket(atoi(v_memo(i+1)->alfabetica),v_memo(i+2)->alfabetica);
	   }
	   else if (!strcmp(fun,"recv_string"))
	   {
		   char *dest = NULL;
		   r = dstcp_recvstring_socket(atoi(v_memo(i+1)->alfabetica),atoi(v_memo(i+2)->alfabetica),&dest);
		   if (dest)
		   {
			   memo_strcpy(i+3,dest);
			   free(dest);
		   }
	   }
	   else if (!strcmp(fun,"ioctlsocket"))
	   {
		   char *dest = NULL;
		   r = dstcp_ioctl_socket(atoi(v_memo(i+1)->alfabetica),v_memo(i+2)->alfabetica,&dest);
		   if (dest)
		   {
			   memo_strcpy(i+3,dest);
			   free(dest);
		   }
	   }
	   else if (!strcmp(fun,"crypto"))
	   {
		   r = dstcp_crypto_socket(atoi(v_memo(i+1)->alfabetica),v_memo(i+2)->alfabetica);
	   }
	   else if (!strcmp(fun,"envia_dat"))
	   {
		   int fo = ind_rel(v_memo(i+2)->alfabetica);
		   if (fo < 0)
			   r = -10;
		   else
		   {
			   r = dstcp_enviadatapaquete_socket(atoi(v_memo(i+1)->alfabetica),3344,v_mpfi(fo)->f_lfich,v_mpfi(fo)->f_buf);
		   }
	   }
	   else if (!strcmp(fun,"recibe_dat"))
	   {
		   int fo = ind_rel(v_memo(i+3)->alfabetica);
		   if (fo < 0)
			   r = -10;
		   else
		   {
			   int n = 0;
			   char *dest = NULL;
			   r = dstcp_recibedatapaquete_socket(atoi(v_memo(i+1)->alfabetica),atoi(v_memo(i+2)->alfabetica),3344,&n,&dest);
			   if (dest)
			   {
				   if (n == v_mpfi(fo)->f_lfich)
				   {
					   memcpy(v_mpfi(fo)->f_buf,dest,n);
				   }
				   else
				   {
					   r = -11;
				   }
				   free(dest);
			   }
		   }
		   
	   }
   }
   else if (!strcmp(ins,"DSTRAY"))
   {
	   r = comunicacion_dstray(i);
   }
   else if (!strcmp(ins,"EXPORTAPLB"))
   {
	    int fo = ind_rel(v_memo(i++)->alfabetica);
	    r = crea_pdb(v_mpfi(fo),v_memo(i++)->alfabetica);
   }
   else if (!strcmp(ins,"EXPORTAXML"))
   {
	    int fo = ind_rel(v_memo(i++)->alfabetica);
	    r = graba_def_a_xml(v_mpfi(fo));
   }
   else if (!strcmp(ins,"CREATTX"))
   {
	    int fo = ind_rel(v_memo(i++)->alfabetica);
	    r = graba_def_a_ttx(v_mpfi(fo));
   }
   else if (!strcmp(ins,"CONSULTAFICHAERP"))
   {
	    r = consulta_ficha_erp(v_memo(i)->alfabetica);
   }
   else if (!strcmp(ins,"TRACKPOPUP"))
   {
	   char *punt,*pp;
	   int n,j;
	   int mem = 0;
	   int pos;
	   pos = atoi(v_memo(i)->alfabetica);
	   mem = strlen(v_memo(i++)->alfabetica) + 1;
	   n = atoi(v_memo(i)->alfabetica);
	   mem += strlen(v_memo(i++)->alfabetica) + 1;
	   for (j = 0;j < n;j++)
	   {
	      mem += strlen(v_memo(i+j)->alfabetica) + 1;
	   }
	   punt = malloc(mem+1);
	   pp = punt;
	   sprintf(pp,"%d%c%d%c",pos,(char)1,n,(char)1);
	   pp += strlen(pp);
	   for (j = 0;j < n;j++)
	   {
		   strcpy(pp,v_memo(i+j)->alfabetica);		   
		   pp += strlen(pp);
		   *pp++ = 1;
	   }
	   *pp = 0;
	   if (DespachadorExterno)
	   {		    
		   (*DespachadorExterno)((int)DESPACHO_MULTIPLE,11,punt);
	   }
	   r = atoi(punt);
   }
   else if (!strcmp(ins,"CAMPOGRID"))
   {
	   int ipfi = (int)v_memo(i++)->numerica;
	   int campodef = (int)v_memo(i++)->numerica;
	   if (ipfi >= 0 && ipfi < nfi)
	   {
		   r = dame_campo_del_grid(ipfi,campodef);
		   v_memo(i)->numerica = (double)r;
	   }
	   else
		   r = -1;
   }
   else if (!strcmp(ins,"X_POP_ERP"))
   {	   
	   extern FI_DEF *pfi_llamada_erp;
	   int ipfi = (int)v_memo(i++)->numerica;
	   r = -1;
	   if (ipfi == -1)
	   {
		   if (pfi_llamada_erp)
		   {
			   memo_strcpy(i,quita_path(pfi_llamada_erp->f_PathDef));
			   r = 0;
		   }
		   else
		   {
			   memo_strcpy(i,"NO HAY FICHA ERP");			  
		   }
	   }
	   else if (ipfi >= 0 && ipfi < nfi && mpfi[ipfi])
	   {		   
		   if (pfi_llamada_erp && !strcmp(quita_path(pfi_llamada_erp->f_PathDef),quita_path(v_mpfi(ipfi)->f_PathDef)))
		   {
			   memcpy(v_mpfi(ipfi)->f_buf,pfi_llamada_erp->f_buf,v_mpfi(ipfi)->f_lfich);		  
			   memo_strcpy(i,quita_path(pfi_llamada_erp->f_PathDef));
			   r = 0;
		   }
		   else
		   {
			   memo_strcpy(i,quita_path(pfi_llamada_erp->f_PathDef));
		   }
	   }
	   else
	   {
		   memo_strcpy(i,"REFERENCIA ERRONEA");
	   }
   }
   else if (!strcmp(ins,"X_DATOS_ERP"))
   {
	   int nivel,k;
	   nivel = atoi(v_memo(i++)->alfabetica);
	   if (nivel >= 0 && nivel < recur_punto)
	   {
		   int fo = ind_rel(v_memo(i++)->alfabetica);
		   if (fo >= 0)
		   {
			   int nf = recur_nfi[nivel];
			   FI_DEF **ppfi = (FI_DEF **)recur_mpfi[nivel];
			   int j;
			   char *p = quita_path(v_memo(i++)->alfabetica);
			   for (j = 0;j < nf;j++)
			   {
				   if (!ppfi[j])
					   continue;
				   if (!strcmp(quita_path(ppfi[j]->f_PathDef),p) && ppfi[j]->f_buf)
				   {
					   if (ppfi[j]->f_lfich == v_mpfi(fo)->f_lfich)
					   {
						   memcpy(v_mpfi(fo)->f_buf,ppfi[j]->f_buf,v_mpfi(fo)->f_lfich);
						   memcpy(v_mpfi(fo)->f_PathDef,ppfi[j]->f_PathDef,sizeof(ppfi[j]->f_PathDef));
						   memcpy(v_mpfi(fo)->f_fichero,ppfi[j]->f_fichero,sizeof(ppfi[j]->f_fichero));
						   for (k = 0;k < v_mpfi(fo)->f_npanta;k++)
						   {
							   memcpy(v_mpfi(fo)->f_pantalla[k].p_nombrepan,ppfi[j]->f_pantalla[k].p_nombrepan,sizeof(ppfi[j]->f_pantalla[k].p_nombrepan));
						   }
						   r = 0;
					   }
					   memo_strcpy(i,ppfi[j]->f_fichero);
					   break;
				   }
			   }
		   }
	   }
   }
   else if (!strcmp(ins,"X_INFO_ERP"))
   {
	   FI_DEF *pfi = NULL;
	   sprintf(tmp,"%d",recur_punto+1);
	   memo_strcpy(i,tmp);	   
	   i++;
	   if (recur_punto > 0)
	   {
		   if (recur_nfi[0])
		   {
			  pfi = *((FI_DEF **)recur_mpfi[0]);
		   }
	   }
	   else
	   {
		   if (nfi)
			  pfi = v_mpfi(0);
	   }
	   if (pfi)
	   {
		   memo_strcpy(i,quita_path(pfi->f_PathDef));		   
		   i++;
		   memo_strcpy(i,pfi->f_PathDef);		   
		   r = 0;
	   }
   }
   else if (!strcmp(ins,"REPARA_FICHERO"))
   {
	   sprintf(v_memo(5004)->alfabetica,"%d",repara_fichero(v_memo(i)->alfabetica,v_memo(i+1)->alfabetica,v_memo(i+2)->alfabetica));
	   n_np = 2;
	   return 0;
   }
   else if (!strcmp(ins,"VERSION_WINDOWS"))
   {
	   tmp[0] = 0;
	   if (DespachadorExterno)
	   {		    
		   tmp[0] = 11;
		   tmp[1] = 0;
		   (*DespachadorExterno)((int)DESPACHO_MULTIPLE,8,tmp);		   
	   }
	   memo_strcpy(i,tmp);
	   strcpy(v_memo(5004)->alfabetica,tmp);
	   n_np = 2;
	   return 0;
   }   
   else if (!strcmp(ins,"VERSION_PROGRAMA"))
   {
	   tmp[0] = 0;
	   if (DespachadorExterno)
	   {		    
		   tmp[0] = 10;
		   tmp[1] = 0;
		   strcpy(tmp+1,v_memo(i)->alfabetica);
		   (*DespachadorExterno)((int)DESPACHO_MULTIPLE,8,tmp);		   
	   }
	   tmp[250] = 0;
	   memo_strcpy(i,tmp);
	   strcpy(v_memo(5004)->alfabetica,tmp);
	   n_np = 2;
	   return 0;
   }
   else if (!strcmp(ins,"VERSION_REMOTA"))
   {
#ifdef RWSERVER
	   extern int remoto_ver1;
	   extern int remoto_ver2;
  	   extern char remoto_verletra;
	   sprintf(tmp,"%02d.%02d%c",remoto_ver1,remoto_ver2,remoto_verletra);
#else
	   strcpy(tmp,"LOCAL");	   
#endif
	   memo_strcpy(i,tmp);
	   strcpy(v_memo(5004)->alfabetica,tmp);
	   n_np = 2;
	   return 0;
   }
   else if (!strcmp(ins,"TECLACAMPO"))
   {
       qj_forzado = pasa_a_int(v_memo(i)->numerica);
	   requiere_reposicionar = 2;
	   strcpy(v_memo(5004)->alfabetica,"0");
	   n_np = 2;
	   return 0;
   }
   else if (!strcmp(ins,"SERIALIZACION"))
   {	   
	   memo_strcpy(i,BufferSerie);
	   strcpy(v_memo(5004)->alfabetica,BufferSerie);
	   n_np = 2;
	   return 0;
   }
   else if (!strcmp(ins,"REMOTOMD5") || !strcmp(ins,"MD5"))
   {
	   char resultado[128];
#ifdef RWSERVER
	   if (!strcmp(ins,"REMOTOMD5") && en_conexion_externa)
	   {
		   strcpy(resultado,v_memo(i)->alfabetica);
		   funcion_windows_cliente(40,resultado,strlen(resultado)+1);
		   strcpy(v_memo(5004)->alfabetica,resultado);
		   n_np = 2;
		   return 0;
	   }
#endif
	   calcula_md5(v_memo(i)->alfabetica,resultado);
	   strcpy(v_memo(5004)->alfabetica,resultado);
	   n_np = 2;
	   return 0;
   }
   else if (!strcmp(ins,"MSLEEP"))
   {
#ifdef ESDOS
	   Sleep(i);
#else
	   usleep(i*1000);
#endif
   }
   else if (!strcmp(ins,"MODO_VER_EURO"))
   {	   
	   if (strcmp(v_memo(i++)->alfabetica,"CAMBIAR"))
	   {
		   r = fgv_ver_euro;
		   sprintf(tmp,"%d",fgv_ver_euro);
		   memo_strcpy(i,tmp);
	   }
   }
   else if (!strcmp(ins,"GRABA_DEF"))
   {
	   int fo = ind_rel(v_memo(i++)->alfabetica);
	   if (fo >= 0)
	   {
		   FI_DEF *pfi = v_mpfi(fo);
		   r = graba_defn(pfi->f_PathDef,pfi);
	   }
   }
   else if (!strcmp(ins,"POPUPMODE"))
   {
	   char tmpp[256];	   
	   int pi = atoi(v_memo(i++)->alfabetica);
	   int pf = atoi(v_memo(i++)->alfabetica);
	   int flags = atoi(v_memo(i++)->alfabetica);
	   /* pipfflagnombre si pi = -1 y pf = -2 = to max,flag = 0000 hide 0001 = show */
	   sprintf(tmpp,"%05d,%04d%04d%4d%s",modo_desatendido,pi,pf,flags,v_memo(i++)->alfabetica);
	   /*  */
	   if (DespachadorExterno)
	   {		    
		    (*DespachadorExterno)((int)DESPACHO_MULTIPLE,5,tmpp);
			modo_desatendido = atoi(tmpp);
	   }
   }
   else if (!strcmp(ins,"FIN_POPUPMODE"))
   {
	   char tmpp[256];	   	   
	   sprintf(tmp,"%d",modo_desatendido);
	   /*  */
	   if (DespachadorExterno)
	   {			
			(*DespachadorExterno)((int)DESPACHO_MULTIPLE,6,tmpp);
			modo_desatendido = atoi(tmpp);
	   }
   }
   else if (!strcmp(ins,"MEMORYFCOPY"))
   {
	   int fo = ind_rel(v_memo(i++)->alfabetica);
	   int campo1 = atoi(v_memo(i++)->alfabetica);
	   int campo2 = atoi(v_memo(i++)->alfabetica);	   
	   if (fo > -1 && v_mpfi(fo)->tipo_datos == 5 && v_mpfi(fo)->f_data && campo1 >= 0 && campo1 < v_mpfi(fo)->f_ncampos && campo2 >= 0 && campo2 < v_mpfi(fo)->f_ncampos)
	   {
		   FI_DEF *pfi;
		   char *pdata;
		   char *pdata2;
		   long *plong;
		   int ll;
		   int lfich;
		   long nregs;

		   pfi = v_mpfi(fo);
		   lfich = pfi->f_lfich;
		   plong = (long *)pfi->f_data;
		   pdata = pfi->f_data + plong[0] + pfi->f_campos[campo1].c_offset;
		   pdata2 = pfi->f_data + plong[0] + pfi->f_campos[campo2].c_offset;
		   ll = pfi->f_campos[campo1].c_long;
		   nregs = plong[2];
		   for (r = 0;r < nregs;r++)
		   {
			   memcpy(pdata,pdata2,ll);
			   pdata += lfich;
			   pdata2 += lfich;
		   }
	   }
   } else if (!strcmp(ins,"MEMORYFILE"))
   {
	   long nregs = 0,reg,extra;
	   long tama;
	   long *plong;
	   char *pdata;
	   int memokeys;
	   int nkeys;	   
	   int j,k,cc;
	   int dfd,tamr,tamdr;
	   char *pbuf;
	   char *pdbuf;
	   char *pxbuf;
	   char tmpp[512];
	   int fo = ind_rel(v_memo(i++)->alfabetica);
	   int fd = ind_rel(v_memo(i++)->alfabetica);
	   int sale,tipo;
	   int saltox = 737;
	   if (fd > -1)
	   {		   
		   if (fo != fd)
		   {
			   cierra_ind(v_mpfi(fd));
		   }
		   if (v_mpfi(fd)->f_data)
		   {
			   free(v_mpfi(fd)->f_data);
			   v_mpfi(fd)->f_data = NULL;
		   }
		   extra = atol(v_memo(i++)->alfabetica);
		   if (fo > 0)
		   {			    
				nregs = tam_btrv(v_mpfi(fo));
		   }
		   else
		   {
			    nregs = 0;
		   }
		   tama = v_mpfi(fd)->f_lfich * (nregs+extra);
		   tama += ((sizeof(long)*10)+512);
		   nkeys = v_mpfi(fd)->f_nclaves;
		   memokeys = sizeof(long)*(nregs+extra)*nkeys;
		   tama += memokeys;		   
		   
		   v_mpfi(fd)->f_data = malloc(tama);
		   if (!v_mpfi(fd)->f_data)
		   {
			   aviso(0,"No hay memoria!");
			   return 0;
		   }
		   memset(v_mpfi(fd)->f_data,0,tama);
		   plong = (long *)v_mpfi(fd)->f_data;
		   plong[0] = memokeys + ((sizeof(long)*10)+512); /* offset a data */
		   plong[1] = ((sizeof(long)*10)+512); /* offset a keys */
		   plong[2] = 0; /* registros existentes */
		   plong[3] = nregs+extra; /* registros maximos (tam memoria)*/
		   plong[4] = 0; /* current key */
		   plong[5] = 0; /* current pointer relativo key*/
		   plong[6] = 0; /* current reg */		   
		   plong[7] = 0; /* "trozo" */
		   plong[8] = 512; /* max len contenido (pos-keys)*/
		   plong[9] = 0; /* len contenido actual*/

#ifdef RWSERVER
			if (en_conexion_externa)
			{
				saltox = (nregs+extra)/100;
				if (saltox < 1)
					saltox = 50;
			}
#endif

		   if (fo > 0)
		   {
			   int modo = 0;
			   long pos = 0;
			   FI_DEF *pfi;
			   tamr = v_mpfi(fo)->f_lfich;
			   tamdr = v_mpfi(fd)->f_lfich;
			   pbuf = v_mpfi(fo)->f_buf;
			   pdbuf = v_mpfi(fd)->f_buf;			   

			   
			   if (DespachadorExterno)
			   {					
					sprintf(tmpp,"%ld",nregs);
					(*DespachadorExterno)((int)DESPACHO_MULTIPLE,2,tmpp);
			   }			   

			   tipo = v_mpfi(fo)->tipo_datos;
			   if (tipo == 0)
			   {  
				   pfi = v_mpfi(fo);
				   dfd  = ixxfichs[pfi->f_fd]->fd_dat;
                   sale = lee_secuencia(pfi,dfd,&pos,&modo,tamr,pbuf);
			   }
			   else
			   {
				   sale = lee_reg(v_mpfi(fo),ISFIRST);
			   }
			   if (sale >= 0)
			   {
				   /*pdata = (char *)plong + plong[0];*/
				   reg = 0;
				   nkeys = v_mpfi(fd)->f_ncampos;
				   do
				   {
					   if (fd != fo)
					   {
						   pxbuf = pdbuf;
						   
						   for (j = 0;j < nkeys;j++)
						   {
							   cc = atoi(v_memo(i+j)->alfabetica);
							   k = v_mpfi(fd)->f_campos[j].c_long;
							   memcpy(pxbuf,pbuf+v_mpfi(fo)->f_campos[cc].c_offset,k);
							   pxbuf += k;
						   }
						   
					   }
					   memory_graba_nuevo(v_mpfi(fd));
					   /*pdata += tamdr;*/
					   reg++;
					   if (reg >= nregs)
						   break;

					   
					   if (!(reg % saltox) && DespachadorExterno)
					   {					
							sprintf(tmpp,"%ld",reg);
							(*DespachadorExterno)((int)DESPACHO_MULTIPLE,3,tmpp);
#ifdef RWSERVER
							if (en_conexion_externa)
							{
								pulsatecla();
							}
#endif
					   }
					   

					   if (tipo == 0)
					   {  
						   sale = lee_secuencia(pfi,dfd,&pos,&modo,tamr,pbuf);
					   }
					   else
					   {
						   sale = lee_reg(v_mpfi(fo),ISNEXT);
					   }
				   } while(sale >= 0);				
			   }
			   
			   if (DespachadorExterno)
			   {					
					sprintf(tmpp,"%ld",reg);
					(*DespachadorExterno)((int)DESPACHO_MULTIPLE,4,tmpp);
					pulsatecla();
			   }
			   
		   }
		   if (fo == fd)
		   {
			   cierra_ind(v_mpfi(fo));
		   }
		   v_mpfi(fd)->tipo_datos = 5;
		   abre_ind(v_mpfi(fd),ISINOUT + ISMANULOCK);		   
		   r = plong[2];
	   }
   } else if (!strcmp(ins,"PIDEWIMPRE"))
   {
		if (DespachadorExterno)
		{        
			char tmp[512];
			strcpy(tmp,v_memo(i)->alfabetica);
			(*DespachadorExterno)((int)DESPACHO_MULTIPLE,0,tmp);
			memo_strcpy(i,tmp);
		}
   } else if (!strcmp(ins,"INTERFACE_VISUAL"))
   {	  
	   CargaVInterface(v_memo(i)->alfabetica,atoi(v_memo(i+1)->alfabetica));
	   r = 0;
   }
   else if (!strcmp(ins,"XEVENTO"))
   {
	    char *p;
		int j,k;
		unsigned int codigo;
		p = v_memo(i)->alfabetica;
		codigo = (unsigned int)atoi(v_memo(i+1)->alfabetica);
		if (codigo >= DISPARO_CONTROL && codigo < PREMAX_DISPARO_CONTROL)
		{
			k = (int)(codigo - DISPARO_CONTROL);
			if (!*p)
			{
				mis_controles[k] = 0;
				sprintf(v_memo(5004)->alfabetica,"%d borrado",codigo);
				n_np = 2;
				return 0;
			}
		}
		else
		{
			for (k = 0;k < 800;k++)
				if (!mis_controles[k])
					break;
			if (k >= 800)
			{
				strcpy(v_memo(5004)->alfabetica,"-1 demasiados eventos");
				n_np = 2;
				return 0;
			}
		}
   		for (j = 0;j < np_ex;j++)
		{
		   if (strcmp(p,p_ex[j].nombre) == 0) 
		   {
			  
			  break;
		   }
		}
		if (j < np_ex)
		{
			f_disparo = mi_disparo;
			if (k < 800)
			{
				mis_controles[k] = p_ex[j].indice;			
				sprintf(v_memo(5004)->alfabetica,"%d programado",DISPARO_CONTROL+k);
			}
		}
		else
		{
			mis_controles[k] = 0;
			strcpy(v_memo(5004)->alfabetica,"-1 no se encuentra proceso externo");
		}
	    n_np = 2;
	    return 0;
   }
   else if (!strcmp(ins,"DISKUSAGE"))
   {
	   extern double haz_du();
	/*
	   char resultado[128];
#ifdef RWSERVER
	   if (!strcmp(ins,"REMOTOMD5") && en_conexion_externa)
	   {
		   strcpy(resultado,v_memo(i)->alfabetica);
		   funcion_windows_cliente(40,resultado,strlen(resultado)+1);
		   strcpy(v_memo(5004)->alfabetica,resultado);
		   n_np = 2;
		   return 0;
	   }
#endif
	  */
	   sprintf(v_memo(5004)->alfabetica,"%.0lf",haz_du(v_memo(i)->alfabetica));
	   n_np = 2;
	   return 0;
   } else if (!strcmp(ins,"CREATETABLE"))
   {
	   int fo = ind_rel(v_memo(i++)->alfabetica);
	   if (fo >= 0)
	   {
           sql_txt_respaldo_crea(v_mpfi(fo));
		   r = 0;
	   }
   } else if (!strcmp(ins,"DROPTABLE"))
   {
	   int fo = ind_rel(v_memo(i++)->alfabetica);
	   if (fo >= 0)
	   {
           sql_txt_respaldo_drop(v_mpfi(fo));
		   r = 0;
	   }
   } else if (!strcmp(ins,"INSERT ALL"))
   {
	   int fo = ind_rel(v_memo(i++)->alfabetica);
	   if (fo >= 0)
	   {
		   int fd;
		   fd = abre_ind(v_mpfi(fo));
		   if (fd >= 0)
		   {
			   if (lee_reg(v_mpfi(fo),ISFIRST) >= 0)
			   {
				   do
				   {
					   sql_txt_respaldo_nuevo(v_mpfi(fo)); 
				   } while(lee_reg(v_mpfi(fo),ISNEXT) >= 0);
			   }
			   cierra_ind(v_mpfi(fo));
		   }           
		   r = 0;
	   }
   } else if (!strcmp(ins,"SENTENCIASQL"))
   {
       sql_txt_respaldo_sentencia(v_memo(i++)->alfabetica);
	   r = 0;
   } else if (!strcmp(ins,"ACELERAPERMISOS"))
   {
       r = crea_data_codigo(v_memo(i)->alfabetica);
   } else if (!strcmp(ins,"MENSAJETRAY"))
   {
	   strcpy(tmp,v_memo(i++)->alfabetica);
	   if (tmp[0] != 0)
	   {
		   strcat(tmp,"\001");
		   strcat(tmp,v_memo(i++)->alfabetica);
		   strcat(tmp,"\001");
		   strcat(tmp,v_memo(i++)->alfabetica);
		   strcat(tmp,"\001");
		   strcat(tmp,v_memo(i++)->alfabetica);
		   strcat(tmp,"\001");
	   }
   	   if (DespachadorExterno)
	   {		    
		   (*DespachadorExterno)((int)DESPACHO_MULTIPLE,18,tmp);
	   }
	   tmp[255] = 0;
	   strcpy(v_memo(5004)->alfabetica,tmp);		
	   n_np = 2;
	   return 0;
   } else if (!strcmp(ins,"FIEBDC"))
   {
	  if (!strcmp(v_memo(i)->alfabetica,"CARGA"))
	  {
		  r = carga_el_bc3(v_memo(i+1)->alfabetica);
	  }
	  else if (!strcmp(v_memo(i)->alfabetica,"LIBERA"))
	  {
		  fin_bc3();
		  r = 0;
	  }
	  else if (!strcmp(v_memo(i)->alfabetica,"PARAMETROS"))
	  {
		  char **parametros[4];
		  int total = atoi(v_memo(i+1)->alfabetica);
		  char tmp[257];
		  int npar = 0;
		  int j,np = 0,n,k;
		  npar = lista_de_parametros_bc3(v_memo(i+2)->alfabetica,parametros);
		  if (npar > 0)
		  {
		      sprintf(tmp,"%d",npar);
			  memo_strcpy(i+3,tmp);
			  j = 4;
			  while(j < total && np < npar)
			  {
				  for (n = 0;parametros[np][n];n++);
				  sprintf(tmp,"%d",n);
				  memo_strcpy(i+j,tmp);
				  j++;
				  for (k = 0;k < n && j < total;k++)
				  {
					  memo_strcpy(i+j,parametros[np][k]);
					  free(parametros[np][k]);
					  j++;
				  }
				  free(parametros[np]);
				  np++;
			  }
		  }
		  r = npar;
	  }
	  else if (!strcmp(v_memo(i)->alfabetica,"PROVINCIA"))
	  {
		  r = pon_provincia_bc3(atoi(v_memo(i+1)->alfabetica));
	  }
	  else if (!strcmp(v_memo(i)->alfabetica,"PARAMETRICO"))
	  {
		   int j;
		   char tmp[257];
		   char *texto = NULL;
		   char *resumen = NULL;
		   char *_error = NULL;
		   char *desglose[512];
		   double cantidad[512];
		   double precios[512];
		   double precio;
		   int ndesglose;
		   int total = atoi(v_memo(i+1)->alfabetica);
		   i++;
		   if (total > 8)
		   {		   
			   ndesglose = resuelve_parametrico_bc3(v_memo(i+1)->alfabetica,NULL,&texto,&resumen,desglose,cantidad,precios,&precio,&_error);
			   r = ndesglose;
			   sprintf(tmp,"%d",ndesglose);
			   memo_strcpy(i+2,tmp);
			   memo_strcpy(i+3,"");
			   if (_error)
			   {
				   memo_strcpy(i+3,_error);
				   free(_error);
			   }
			   for (j = 0;j < 21;j++)
			   {
				   memo_strcpy(i+4+j,"");
			   }
			   if (texto)
			   {
				   char *px1,*px2;
				   int lx = strlen(texto);
				   int ll = 4;
				   px1 = texto;
				   while(lx > 60 && ll < 24)
				   {
					   lx -= 60;
					   px2 = px1 + 60;
					   while (px2 >= px1 && *px2 && *px2 != ' ')
					   {
						   px2--;
						   lx++;
					   }
					   if (px1 == px2)
					   {
						   break;
					   }
					   *px2 = 0;
					   memo_strcpy(i+ll,px1);
					   px1 = px2 + 1;
					   ll++;
				   }
				   memo_strcpy(i+ll,px1);
				   free(texto);
			   }
			   memo_strcpy(i+25,"");
			   if (resumen)
			   {
				   memo_strcpy(i+25,resumen);
				   free(resumen);
			   }		   
			   sprintf(tmp,"%lf",precio);
			   memo_strcpy(i+26,tmp);

			   for (j = 0;j < ndesglose && (j*3) < (total-29);j++)
			   {
				   memo_strcpy(i+27+j*3,"");
				   if (desglose[j])
				   {
					   memo_strcpy(i+27+j*3,desglose[j]);
					   free(desglose[j]);
				   }
			       sprintf(tmp,"%lf",cantidad[j]);
			       memo_strcpy(i+28+j*3,tmp);
			       sprintf(tmp,"%lf",precios[j]);
			       memo_strcpy(i+29+j*3,tmp);
			   }
		   }
	  }	  
   } else if (!strcmp(ins,"CORTEBARRAS"))
   {
	   int *corte_barra(int lbarra,int ncortes,int *cortes,int lminimo);
	   int memores = (int)v_memo(i)->numerica;
	   int lbarra = (int)v_memo(i+1)->numerica;	   
	   int lminimo = (int)v_memo(i+2)->numerica;
	   int ncortes = pasa_a_int(v_memo(i+3)->numerica);
	   int *cortes,j,n,k;
	   int *res;
	   int imem = memores + 2;
	   int tope,tot = 0;
	   int ll;
	   int *tmpcortes;
	   int tmpncortes;
	   int cortespasados = 0;
	   int esultima;
	   int nbarras;
	   int nrestos;
	   int sepideal = 10;
	   int homo = 0;	   
	   static int __tbarra[500];

	   r = 0;
	   cortes = malloc(sizeof(int)*(ncortes+1));
	   for (j = 0;j < ncortes;j++)
	   {
		   cortes[j] =  (int)v_memo(i+4+j)->numerica;
	   }
	   cortes[j] = -1;
	   ncortes = ordena_cortes(ncortes,cortes,lbarra,lminimo,&sepideal,&homo);
	   tope = (int)v_memo(memores++)->numerica;
	   v_memo(memores)->numerica = 0.0;	   
	   nrestos = 0;
	   do
	   {
		   if (cortespasados < ncortes)
		   {
			   if ((ncortes-cortespasados) > sepideal)
			   {
				   tmpncortes = sepideal;
				   tmpcortes  = cortes + cortespasados;
			   }
			   else
			   {
				   tmpncortes = ncortes-cortespasados;
				   tmpcortes  = cortes + cortespasados;
			   }
		   }
		   nbarras = 0;
		   do
		   {
			   if (tot >= tope)
			   {
				   r = 1;
				   break;
			   }	
			   if (homo == 1 || sepideal > 12)
			   {
				   res = __tbarra;
				   ll = 0;
				   n = 0;
				   res[n] = -1;
				   for (j = 0;j < tmpncortes;j++)
				   {
					   if (tmpcortes[j] > 0)
					   {
						   if ( (ll + tmpcortes[j]) > (lbarra-lminimo) && (ll + tmpcortes[j]) != lbarra)
						   {
							   break;
						   }
						   ll  += tmpcortes[j];
						   res[n] = j;
						   n++;
						   res[n] = -1;						   
					   }
				   }
			   }
			   else
			   {
				   res = corte_barra(lbarra,tmpncortes,tmpcortes,lminimo);
			   }

			   if (res[0] == -1)
			   {
				   break;
			   }
			   n = 0;
			   j = 0;			   

			   while(res[n] != -1)
			   {				   
				   res[n] += cortespasados;
				   j += cortes[ res[n] ];
				   n++;
			   }			   
			   if ( (tot+1+n) > tope)
			   {
				   r = 1;
				   break;
			   }
			   
			   if (ncortes > sepideal)
			   {/* mirar si es la ultima barra */
				   esultima = 1;
				   for (j = 0;j < tmpncortes;j++)
				   {
					   if (tmpcortes[j] > 0)
					   {
						   for (k = 0;k < n;k++)
						   {
							   if (res[k] == (j+cortespasados))
							   {
								   break;
							   }
						   }
						   if (k >= n)
						   {
							   esultima = 0;
							   break;
						   }
					   }
				   }
				   if (esultima)
				   {
					   if (nbarras > 0 || tmpncortes == (ncortes-cortespasados))
					   {
						   nrestos += n;
					       break;
					   }
				   }
			   }

			   v_memo(memores)->numerica = v_memo(memores)->numerica + 1;

			   v_memo(imem++)->numerica = (double)n;
			   tot++;
			   for (j = 0;j < n && tot < tope;j++)
			   {
				   v_memo(imem++)->numerica = (double)cortes[ res[j] ];
				   cortes[ res[j] ] = -cortes[ res[j] ];
				   tot++;
			   }
			   nbarras++;
		   } while(-1);
		   if (r == 1)
		   {
			   break;
		   }
		   if (ncortes <= sepideal)
		   {
			   break;
		   }
		   if (tmpncortes == (ncortes-cortespasados))
		   {
			   if (!nrestos)
				   break;
			   ncortes = ordena_cortes(ncortes,cortes,lbarra,lminimo,&sepideal,&homo);
			   if (ncortes != nrestos)
			   {
				   /* error! */				   				   
			   }
			   nrestos = 0;
			   cortespasados = 0;
		   }
		   else
		   {
			   cortespasados += tmpncortes;
		   }		   		   
	   } while(-1);

	   if (cortes)
	   {
		   free((char *)cortes);
	   }
   } else if (!strcmp(ins,"RBASS"))
   {
#ifdef RWSERVER
	   extern char remoto_dirbass[51];
	   memo_strcpy(i,remoto_dirbass);
#else
	   memo_strcpy(i,dirbass);
#endif
	   r = 0;
   }

   sprintf(v_memo(5004)->alfabetica,"%d",r);
   n_np = 2;
   return 0;
}

int fgv_menuitem(p,tip,faclin)
int p;
int tip;
int faclin;
{   
	char tmp[512];
	int operacion = (int)v_memo(prog[p].indice)->numerica;
	p++;
	strcpy(v_memo(5004)->alfabetica,"");
	strcpy(tmp,v_memo(prog[p].indice)->alfabetica);
    if (DespachadorExterno)
    {        
        (*DespachadorExterno)((int)DESPACHO_MENUITEM,operacion,tmp);
		strcpy(v_memo(5004)->alfabetica,tmp);
    }	
	n_np = 2;
	return 0;
}

int fgv_valorini(p,tip,faclin)
int p;
int tip;
int faclin;
{   
	char tmp[5000];
	tmp[0] = 0;	
	dame_valor_ini(v_memo(prog[p++].indice)->alfabetica,tmp);
	memo_strcpy(prog[p].indice,tmp);
	n_np = 2;
	return 0;
}


static short recursion = 0;

p_ejecuta(int t,int tip,int faclin,int control)
{
   int p,i;
   int l_nbuc = nbuc;
   int *l_bucle = bucle;
   int buf_bucle[20];
   int old_valcond = valcond;
   int ant_t = programa_activo;  
   

   c_programa_en_uso++;
   
   
   if (t < 0) 
   {
      p = (-t) - 1;
   }
   else 
   {
      if (t >= (n_p_tabla-2))
	  {
		  /*printf("[%d]>[%d]",t,n_p_tabla);*/
		   {
			   FILE *f;
			   char tmpc[512];
			   sprintf(tmpc,"%saviso64_9.txt",rw_directorio("logs/"));
			   f = fopen(tmpc,"a");
			   if (f)
			   {
				   fprintf(f,"64: %s %s %s %s %s (t:%d-%d-%d)![n_t:%d] recur:%d control:%d p_a:%d\r\n",fecha(),fechas+11,rw_user,nomapli,modulo,t,tip,faclin,n_p_tabla,recur_punto,control,programa_activo);
				   fclose(f);
			   }
		   }
		  rw_error(64);
		  return(-1);
	  }
      p = p_tabla[t];
      if (proc_tabla[t] != programa_activo)
      {
         if (activa_programa(proc_tabla[t]))
         {
            /*aviso(0,__Texto[50]);*/
		   {
			   FILE *f;
			   char tmpc[512];
			   sprintf(tmpc,"%saviso64_9.txt",rw_directorio("logs/"));
			   f = fopen(tmpc,"a");
			   if (f)
			   {
				   fprintf(f,"!!: %s %s %s %s %s (t:%d-%d-%d)![n_t:%d] recur:%d control:%d p_a:%d p_t:%d\r\n",fecha(),fechas+11,rw_user,nomapli,modulo,t,tip,faclin,n_p_tabla,recur_punto,control,programa_activo,proc_tabla[t]);
				   fclose(f);
			   }
		   }
            return(-1);
         }
      }
   }
   if (p < 0 || p >= nprog) 
   {
	   rw_error(104);
	   return(-1);
   }
   
   en_pr[programa_activo].e_usado = c_programa_en_uso;
   
   recursion++;
   
   bucle = buf_bucle;
   nbuc = -1;
   
   while ( -1 ) 
   {      
      if (general_int) (*general_int)();
#ifdef ESDOS
#ifndef FGVWIN
      if (timer_especial_int)
         (*timer_especial_int)();
#endif
#endif
      
      if (mimodo_debug)
      {
         for (i = 0;i < ntr_br;i++) 
		 {
            if (tracer_break[i] == p)
            {
               tracer_int = 1;
               break;
            }
         }

		 check_tracepoints();

         if (tracer_int)  
		 {
            ej_traza(p);
         }
      }
      
      n_np = 1;
      
      if (programa_activo == -1)
      {
         printf(__Texto[280]);getchar();
      }
      
      if (prog[p].i == TEND || prog[p].i == 23 /* ACABA */ || apli_aborto) 
      {
#ifdef ESDOS
         if (timer_especial_int)
            (*timer_especial_int)();
#endif
         break;
      }
      
      ( *siscal[ (int)(unsigned char)prog[p].i ] )(p,tip,faclin);
      if (apli_aborto && t) apli_aborto = 0;
      p += n_np;
   }
   
   
   nbuc = l_nbuc;
   bucle = l_bucle;
   valcond = old_valcond;
   recursion--;
   
   if (!recursion)
      programa_activo = -1;
   else
      if (ant_t != -1 && ant_t != programa_activo)
      {
         if (!activa_programa(ant_t))
         {
            c_programa_en_uso++;
            en_pr[programa_activo].e_usado = c_programa_en_uso;
         }
      }
      return(0);
}

#endif

