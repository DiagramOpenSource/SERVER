
#include <fgvsys.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef O_BINARY

#include <io.h>

#else

#define ES_UNIX

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

#include <signal.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <time.h>
#include <sys/timeb.h>

/*#include <rwmalloc.h>*/
#include <rtermin.h>
#include <rdef.h>
#include <teclas.h>
#include <rfecha.h>
#include <rentra.h>
#define _GENERADOR
#include <rmante.h>
#include <rcalcul.h>
#include <rcompi.h>


#define strmalloc malloc

extern char c_nombrecal[300];

extern int indice_def_local[48];
extern int  nindice_def_local;
extern int en_ini_par;
extern int compilando_un_prc;
extern int minimun;
extern short fini_par[MAXDEF];
extern int c_linea;
extern FILE *c_error;
extern int e_nivel;
extern int c_nerror;
extern int error_break;
extern char *tabla_defbucle[MAX_BUCLE];

extern int cont_defs;


static char t_incluyes[1000];
static char t_niveles[5000];
static  int t_niv_punto = 0;
static  int t_niv_actual = 0;


static int t_f = -1;

static int calerrno = 0;

static int gen_modo = 0;
static int prev_modo = 0;

int _nivel = 0;
int linea_nivel1  = 0;


extern int oculto;
extern int primero;

static int c_ftv = -1;
static int c_fto = -1;


extern char *nombre_defs[MAXDEF*2];
extern char ventana_defs[MAXDEF*2];

extern char notacion_dir_aplicacion[256];

extern char dirbase[51];
extern char dirbass[51];
extern char dirbase_[51];

static int maxpilame;
static int pilame;
static int maxpilamen;
static int pilamen;
static int maxpilamea;
static int pilamea;
static int pare_modif = 0;

extern unsigned short tam_gene_def[MAXDEF];
extern unsigned short def_varmemo;

extern int ya_esta_avisado;

extern long numero_generacion;
extern int modo_gen_antiguo;

extern unsigned short tipo_de_apli;

extern FI_DEF *mpfi_maximo;

extern long nume_serie;

extern unsigned short local_term[52];
extern unsigned short local_term_[52];

extern short _fgv_version;
extern short i3;


static int b_punto = 0;
static int modob = 0;
static int ncondici = 0;
static int condici[30];


extern char **llamada_interna;
extern short nllamada_interna;

#define mensaje(a,b) v_mensaje(a,b,0)
#define Bl(a)        v_mensaje(a,(char *)0,1)
#define aviso(a,b)   v_aviso(a,b)


struct instruccion 
{
	unsigned char *nombre;
	unsigned char *parametros;
	unsigned char in; /* 255 ES PARA PARAMETROS, AQUI INDICA CASO ESPECIAL
					  LA INSTRUCCION SE PONE DEPENDIENDO DE
					  LOS PARAMETROS
	*/
	unsigned short extra_in;
};





/*
descripcion de parametros:
grupos de dos digitos
primero:                                    segundo:
0 = INSTRUCCIONES BUCLE PARA              INSTRUCCION EN CONCRETO
PARA-HACIENDO-SIGUE-SAL

		1 = INSTRUCCIONES DE CONDICIONES                    "
		SI-Y-O-SINO-FINSI
		
		  2 = INSTRUCCIONES CON NUMERO DE FICHERO   INSTRUCCION EN CONCRETO
		  Y OTRO (pon_ccf)
		  
			3 = INSTRUCCIONES CON PARAMETROS          INSTRUCCION EN CONCRETO
			EN OPERACIONES (pon_dcf)
			
			  4 = INSTRUCCIONES CON NUMERO FICHERO                "
			  (pon_acf)
			  
				5 = PARAMETRO ESPECIAL FICHEROS INDEXADOS INSTRUCCION EN CONCRETO
				(pon_bcf)
				
				  6 = NOMBRE DE ETIQUETA                    0 = declara
				  1 = referencia
				  
					7 = PARAMETRO NOMBRE DE PROCESO           modo de guardar
					0 como gosub
					1 como buclelin (delante va el numero de fichero)
					
					  9 = PARAMETRO STANDAR                     variables o campos
					  0 alfanumerico
					  1 alfa-fecha
					  2 fecha
					  3 numerico
					  4 todos tipos (incluido punteros)
					  5-9 igual pero solo para
					  variables
					  A-E igual pero solo para
					  campos
					  F SOLO PUNTERO
					  G Valor numerico tal cual
					  H Letra tal cual
					  I-M fichero+variable
					  
						A = CASO ESPECIAL PINTA
						
						  B = CASO DEFBUCLE
						  
							C = CASO FICHEROS
							
							  D = CASO CAMPOS
							  
								E = CASO VARIABLES
								
								  F = INSTRUCCION BUCLE
								  
									Zn  Caso parametros variables: n indica los bloque de parametros.

*/


#define INST struct instruccion
extern INST std_inst[] = {
		  /* SIMBOLO          PARAMETROS        INSTRUCCION CASOS */
	
			 "HACIENDO"       ,"01"              ,255 ,NULL,
				 "SIPARA"         ,""                ,8   ,NULL,
				 "NOPARA"         ,""                ,20  ,NULL,
				 "ABORTA"         ,""                ,22  ,NULL,
				 "FINSI"          ,"14"              ,255 ,NULL,
				 "SIGUE"          ,"02"              ,255 ,NULL,
				 "SINO"           ,"13"              ,255 ,NULL,
				 "PARA"           ,"00"              ,98 ,NULL,
				 "SI"             ,"10"              ,99 ,NULL,
				 "Y"              ,"11"              ,255 ,NULL,
				 "O"              ,"12"              ,255 ,NULL,
				 "SAL"            ,"03"              ,24  ,NULL,
				 "PATH_DAT"       ,"9I"              ,6   ,NULL,
				 "PATH_PAN"       ,"9I"              ,56  ,NULL,
				 "NOME_DAT"       ,"9I"              ,7   ,NULL,
				 "FNOM"           ,"9I"              ,25  ,NULL,
				 "ENDATOS"        ,"24"              ,69  ,NULL,
				 "ENCAMPO"        ,"25"              ,38  ,NULL,
				 "ENTLINEAL"      ,"20989898987070"  ,31  ,NULL,
				 "GRABAVE"        ,"30"              ,111 ,NULL,
				 "DESBLOQ"        ,"46"              ,82  ,NULL,
				 "DEFECTO"        ,"23"              ,78  ,NULL,
				 "CIERRAT"        ,"4A"              ,64  ,NULL,
				 "CIERRA"         ,"41"              ,75  ,NULL,
				 "INDEXA"         ,"43"              ,79  ,NULL,
				 "LIBERA"         ,"42"              ,83  ,NULL,
				 "SELECT"         ,"20"              ,67  ,NULL,
				 "FINIMP"         ,""                ,109 ,NULL,
				 "FININF"         ,""                ,102 ,NULL,
				 "PIEINF"         ,""                ,116 ,NULL,
				 "PULSATECLA"     ,""                ,129 ,NULL,
				 "MENU"           ,"9F"              ,19  ,NULL,
				 "DELINDEX"       ,"44"              ,80  ,NULL,
				 "DDEFECTO"       ,"47"              ,76  ,NULL,
				 "RDEFECTO"       ,"48"              ,77  ,NULL,
				 "BUCLELIN"       ,"71"              ,84  ,NULL,
				 "IMPRIME"        ,"95"              ,30  ,NULL,
				 "PINTA"          ,"A0"              ,112 ,NULL,
				 "MANTE"          ,"4B"              ,18  ,NULL,
				 "BUCLE"          ,"F0"              ,72  ,NULL,
				 "CIMPR"          ,"9G"              ,15  ,NULL,
				 "PINPA"          ,"21"              ,70  ,NULL,
				 "IMPRE"          ,"9G"              ,108 ,NULL,
				 "PINDA"          ,"22"              ,71  ,NULL,
				 "PINVE"          ,"31"              ,111 ,NULL,
				 "ERROR10"        ,""                ,9   ,NULL,
				 "ERROR"          ,""                ,104 ,NULL,
				 "AVISO"          ,""                ,105 ,NULL,
				 "PAUSA"          ,""                ,106 ,NULL,
				 "ACABA"          ,""                ,23  ,NULL,
				 "GRABA"          ,"51"              ,65  ,NULL,
				 "BORRA"          ,"52"              ,73  ,NULL,
				 "MENSA"          ,"95"              ,85  ,NULL,
				 "SYSTEM"         ,"95"              ,128 ,NULL,
				 "PROTE"          ,"9C"              ,11  ,NULL,
				 "DBASS"          ,"95"              ,4   ,NULL,
				 "CABEZA"         ,"95"              ,94  ,NULL,
				 "DBASE"          ,"95"              ,47  ,NULL,
				 "DFICE"          ,"95"              ,92  ,NULL,
				 "DFICB"          ,"95"              ,0   ,NULL,
				 "NOMAP"          ,"95"              ,5   ,NULL,
				 "ABRET"          ,"49"              ,35  ,NULL,
				 "ABREB"          ,"4C"              ,127 ,NULL,
				 "EDITA"          ,"98989898989895989898",27  ,NULL,
				 "F_TEXTO"        ,"9895989898989898989895",28  ,NULL,
				 "HORA"           ,"95"              ,32  ,NULL,
				 "SCROLL"         ,"989898"          ,29  ,NULL,
				 "ALARMA"         ,"969696"          ,33  ,NULL,
				 "LETRA"          ,"99"              ,107 ,NULL,
				 "MENAV"          ,"95"              ,87  ,NULL,
				 "CONFI"          ,"95"              ,86  ,NULL,
				 "PRINT"          ,""                ,114 ,NULL,
				 "NOALARMA"       ,""                ,34  ,NULL,
				 "INFOR"          ,"95"              ,113 ,NULL,
				 "PUSHV"          ,"98"              ,117 ,NULL,
				 "ATRI"           ,"9H"              ,89  ,NULL,
				 "GRAF"           ,"98"              ,96  ,NULL,
				 "PTEC"           ,"98"              ,126 ,NULL,
				 "CLS"            ,""                ,93  ,NULL,
				 "CUADRO"         ,"98"              ,90  ,NULL,
				 "BLANCO"         ,"98"              ,91  ,NULL,
				 "DBIN"           ,"95"              ,3   ,NULL,
				 "DDEF"           ,"95"              ,2   ,NULL,
				 "DPAN"           ,"95"              ,1   ,NULL,
				 "BLIN"           ,"98"              ,88  ,NULL,
				 "SLEEP"          ,"98"              ,130 ,NULL,
				 "ABRE"           ,"40"              ,74  ,NULL,
				 "POPV"           ,""                ,118 ,NULL,
				 "BLOQ"           ,"45"              ,81  ,NULL,
				 "VETE"           ,"61"              ,115 ,NULL,
				 "HAZ"            ,"70"              ,103 ,NULL,
				 "LEE"            ,"50"              ,66  ,NULL,
				 "ET"             ,"60"              ,255 ,NULL,
				 "TEMPO"          ,"95"              ,119 ,NULL,
				 "FABRE"          ,"95"              ,12  ,NULL,
				 "FCIERRA"        ,"95"              ,14  ,NULL,
				 "FLEE"           ,"95"              ,97  ,NULL,
				 "FGRABA"         ,"95"              ,121 ,NULL,
				 "FBORRA"         ,"95"              ,100 ,NULL,
				 "QBF"            ,"95"              ,95  ,NULL,
				 "QBT"            ,"95"              ,39  ,NULL,
				 "CORRE"          ,"95"              ,21  ,NULL,
				 "PONREGN"        ,"4093"            ,41  ,NULL,
				 "DAREGN"         ,"4093"            ,40  ,NULL,
				 "CAMPO_MODIFICA" ,"9E9G95"          ,36  ,NULL,
				 "C_M"            ,"9E9G95"          ,36  ,NULL,
				 "CAMPO_VISUAL"   ,"9E9G95"          ,37  ,NULL,
				 "C_V"            ,"9E9G95"          ,37  ,NULL,
				 "CAMPO_LINEAL"   ,"9E9G95"          ,42  ,NULL,
				 "C_L"            ,"9E9G95"          ,42  ,NULL,
				 "CAMPO_POSICION" ,"9E9G98"          ,43  ,NULL,
				 "C_P"            ,"9E9G98"          ,43  ,NULL,
				 "CAMPO_ANCHO"    ,"9E9G98"          ,44  ,NULL,
				 "C_A"            ,"9E9G98"          ,44  ,NULL,
				 "MENUG"          ,"9F"              ,45  ,NULL,
				 "DIMENSIONA"     ,"989898"          ,48  ,NULL,
				 "AL_BUF"         ,"989899"          ,49  ,NULL,
				 "DEL_BUF"        ,"989899"          ,50  ,NULL,
				 "FINDIM"         ,"98"              ,51  ,NULL,
				 "LEESECUENCIAL"  ,"9598989898"      ,52  ,NULL,
				 "GRABASECUENCIAL","95989898"        ,53  ,NULL,
				 "BMENUG"         ,"989898989898"    ,46  ,NULL,
				 "QUE_SISTEMA"    ,"95"              ,54  ,NULL,
				 "COLOR"          ,"9898"            ,55  ,NULL,
				 "LEETECLA"       ,"95"              ,58  ,NULL,
				 "TECLA_FUNCION"  ,"989895"          ,59  ,NULL,
				 "S_CUADRO"       ,"9898"            ,60  ,NULL,
				 "D_CUADRO"       ,"9898"            ,61  ,NULL,
				 "SUB_EJECUTA"    ,"95"              ,62  ,NULL,
				 "GRABAENDEF"     ,"9595Z29894"      ,110 ,NULL,
				 "REPINTA_TEXTO"  ,"9898"            ,63,NULL,
				 "ATRIBUTOS_DEFEC","9H98989H98989H9898",123,NULL,
				 "POSICION_EDITA" ,"9898"            ,124,NULL,
				 "CAMPO_ATRIBUTO" ,"9E9H9898"        ,125,NULL,
				 "MODULO"         ,"95"              ,131,NULL,
				 "SIALARMA"       ,""                ,132  ,NULL,
				 "DEBUG"          ,""                ,133  ,NULL,
				 "DEFBUCLE"       ,"B0"              ,255, NULL,
				 "FICHEROS"       ,"C0"              ,255, NULL,
				 "CAMPOS"         ,"D0"              ,255, NULL,
				 "VARIABLES"      ,"E0"              ,255, NULL,			 
				 /****/
				 "CARGADLL"       ,"95"              ,135, NULL,
				 "DESCARGADLL"    ,"95"              ,136, NULL,
				 "ALFACALLDLL"    ,"959595"          ,137, NULL,
				 /****/
				 "CERROR"          ,""               ,138 ,NULL,			 
				 /****/
				 "CAMPO_MINIMO"   ,"9E9G95"          ,139, NULL,
				 /****/
				 "INDICE_DEF"      ,"9895"          ,140, NULL,
				 /****/
				 "ONTIMER"        ,"9895"            ,141, NULL,
				 "FINTIMER"       ,""                ,142, NULL,
				 /****/
				 "POPVNULO"       ,""                ,143, NULL,
				 "SALVA_DATOS"    ,"4E"              ,144,NULL,
				 "REPON_DATOS"    ,"4F"              ,145,NULL,
				 "DIRECTORIO"     ,"95"              ,146,NULL,
				 "ENTORNO"        ,"95"              ,147,NULL,
				 "CONSULTA"       ,"20"              ,148,NULL,
				 "ABRETEMP"       ,"4D"              ,149,NULL,
				 "ENLAZA_DEF_A_EXTERNO","4E"         ,150,NULL, /*OJO!!!*/
				 "DESENLAZA_DEF_A_EXTERNO",""        ,151,NULL, /*OJO!!!*/
				 "CONTROL_DEMO","95"                  ,152,NULL,
				 "REGISTROS_DEMO","98"               ,153,NULL,
				 "EXECDDE"    ,"9595"            ,154,NULL, /* Comando, Topic (reenlaza) */
				 "INIDDE"     ,"95959598"        ,155,NULL, /* programa, Item Topic Modo*/
				 "USERDIRFIC"    ,"95"               ,157,NULL,
				 "COMPRIME"      ,"95"               ,158,NULL,
				 "DESCOMPRIME"   ,"95"               ,159,NULL,
				 "DETAR"         ,"9595"             ,160,NULL,
				 "PONSERIAL"     ,"959898"           ,161,NULL,
				 "INICIOTRANS"   ,""                 ,162,NULL,
				 "ANULATRANS"   ,""                  ,163,NULL,
				 "FINTRANS"    ,""                   ,164,NULL,
				 "LEEDEDEF"       ,"9595Z29894"      ,165,NULL,
				 "COPIA_FICHERO"  ,"9595"            ,166,NULL,
				 "AFEGIR_FICHERO"  ,"9595"           ,167,NULL,
				 "SUMA_FICHERO"    ,"9595"           ,167,NULL,
				 "_PDIR"            ,"95"            ,168,NULL,
				 "_SDIR"            ,"95"            ,169,NULL,
				 "VERSIONRTME"      ,"95"            ,170,NULL,
				 "EXTERNOEXE"       ,"95"            ,171,NULL,
				 "RENOMBRA_FICHERO" ,"9595"          ,172,NULL,
				 "MKDIR"            ,"95"            ,173,NULL,
				 "RMDIR"            ,"95"            ,174,NULL,
				 "PDEFECTO"        ,"479898"         ,175,NULL,
				 "CDEFECTO"        ,"4798"           ,176,NULL,
				 "LINEAINFORME"    ,"9898"           ,177,NULL,
				 "DELTREE"         ,"95"             ,178,NULL,
				 "CARGAWMENU"	   ,"959898"		 ,179,NULL, /* nombre menu , total hojas devuelta y menu real (path completo) leido, opcion por defecto */
				 "LIBERAWMENU"	   ,""				 ,180,NULL,
				 "WMENU"		   ,"989595"		 ,181,NULL, /* codigo de salida o numero de hoja * 100 numero de opcion dato de la opcion , programa */
				 "NUEVAWOPCION"	   ,"9898989595"	 ,182,NULL, /* flag cuelga de hoja,opcion ... datos */
				 "BORRAWOPCION"	   ,"9898"			 ,183,NULL, /* hoja,opcion*/ 
				 "DATOSWHOJA"	   ,"9898989595"     ,184,NULL, /* flag hoja opcion datos */
				 "GRABAWMENU"	   ,"95"			 ,185,NULL, /* nombre */
				 "ATAR"            ,"9595"           ,186,NULL,
				 "INSTALA"		   ,"95"			 ,187,NULL,
				 "DATOS_PAQUETE"   ,"9895959598"	 ,188,NULL,
				 "BLOQUEA_MASTER"  ,""				 ,189,NULL,
				 "DESBLOQUEA_MASTER",""				 ,190,NULL,
				 "COPIAINSTALACION","95"			 ,191,NULL,
				 "GRABABMPPANTALLA","95"			 ,192,NULL,
				 "GRABADEFSPANTALLAS","959595"		 ,193,NULL,
				 "GRABAMENUPANTALLAS","959595"		 ,194,NULL,
				 "MODULO_EXTERNO" ,""                ,195,NULL,
				 "VA_CON_DOBLE","9595"				 ,196,NULL,
				 "ES_LA_DOBLE" ,"98"				 ,197,NULL,
				 "TEST_DOBLE" ,"9898"				 ,198,NULL,
				 "JOIN_BUCLE"  ,"F0"				 ,199,NULL,
				 "MIXED_BUCLE","F0"					 ,200,NULL,
				 "GET_HIDEN_INDICATOR" ,"98"	     ,201,NULL,
				 "CAMBIA_A_DOBLE","98"         	     ,202,NULL,
				 "DIME_LA_MEZCLA","98"				 ,203,NULL,
				 "MIXED_BUFFERS","4040"				 ,204,NULL,
				 "END_MIXED_BUFFERS","4040"			 ,205,NULL,
				 "POSICIONATE","9E"					 ,206,NULL,
				 "SALVA_FICHA"    ,"4E"              ,207,NULL,
				 "REPON_FICHA"    ,"4F"              ,208,NULL,
				 "LINEAS_BI_ATRI","9E9H9H9G"		 ,209,NULL,
				 "CONSULTA_CLAVE","20"				 ,210,NULL,
				 "OCULTA_LINEAS","4E98"              ,211,NULL,
				 "FORMATO_HACIENDA","4E95959595"     ,212,NULL,
				 /* fichero fuente datos,dir,fichero formato hacienda(txt),tabla(txt) de relacion de campos,fichero de salida(txt) una linea por hoja(crlf)*/
				 "IMPRIME_HACIENDA" ,"959595"        ,213,NULL,
				 /* servicio, remoto,ejecucion */
				 "EJECUCION_REMOTA","959595"         ,214,NULL,
				 /* servicio, remoto ,fichero origen,fichero destino */
				 "RECIBE_FICHERO"   ,"95959595"		 ,215,NULL,
				 "PARTEFORMATO_HACIENDA","4E95959595",216,NULL,
				 "CAMBIA_DE_DEF","4E95"              ,217,NULL,
				 "POKEDDE", "959595"             ,218,NULL, /* texto itemid Topic */
				 "FINDDE",  ""                   ,219,NULL,
				 "PEEKDDE", "959595"             ,220,NULL, /* texto itemid Topic */			 
				 "POKEDDE", "959595"             ,218,NULL, /* texto itemid Topic */
				 "FINDDE",  ""                   ,219,NULL,
				 "PEEKDDE", "959595"             ,220,NULL, /* texto itemid Topic */			 
				 "PONCONTROL",  "9895"				 ,221,NULL,
				 "PARTE_FICHERO","959895"            ,222,NULL,/* fichero tam destino */
				 "SQLSELECT",""						 ,223,NULL,
				 /* servicio, remoto ,fichero destino,fichero origen */
				 "ENVIA_FICHERO"   ,"95959595"		 ,224,NULL,
				 "FLEE_NUMERO",  "9598"              ,225,NULL,
				 "FGRABA_NUMERO","9598"              ,226,NULL,
				 "IP_REMOTO",    "95"				 ,227,NULL,
				 "NOMBRE_IP",    "9595"              ,228,NULL,			 
				 "ENLAZA_PROCESO", "9E7598"		     ,229,NULL,
				 "DESENLAZA_PROCESO","9E7598"	 	 ,230,NULL,
				 "CARGA_DEF",        "9598"          ,231,NULL,
				 "DESCARGA_DEF",     "49"            ,232,NULL,
				 "REFERENCIA_DEF",   "9849"          ,233,NULL,
				 "CONSULTA_F_CLAVE", "207070"        ,234,NULL,			 
				 "CAMPO_MAXIMO"     ,"9E9G95"        ,235,NULL,
				 "IMPRIMESIN"       ,"95"            ,236,NULL,
				 "RSYSTEM"          ,"95"            ,237,NULL,
                 "MODO_BMP"         ,"9898"          ,238,NULL,
                 /* Reservamos algunas directas para casos de necesidad */
				 "CONTROL_WINDOWS"  ,"957070989595"  ,254,1, /* control,rutina,pos,defecto,valor(!) */
				 "FIN_CONTROL_WINDOWS","98"          ,254,2,
				 "MENUG_LIMITE","9F98"               ,254,3,
				 "BMENUG_LIMITE"  ,"98989898989898"  ,254,4,
				 "R_PDIR"            ,"95"           ,254,5,
				 "R_SDIR"            ,"95"           ,254,6,
				 "RCOPIA_FICHERO"  ,"9595"           ,254,7,			 
				 "RSUMA_FICHERO"    ,"9595"          ,254,8,
				 "RRENOMBRA_FICHERO" ,"9595"         ,254,9,
				 "RMKDIR"            ,"95"           ,254,10,
				 "RRMDIR"            ,"95"           ,254,11,
				 "RDELTREE"         ,"95"            ,254,12,
				 "RCOMPRIME"      ,"95"              ,254,13,
				 "RDESCOMPRIME"   ,"95"              ,254,14,
				 "RDETAR"         ,"9595"            ,254,15,
				 "RATAR"          ,"9595"            ,254,16,
				 "RTEMPO"          ,"95"             ,254,17, /* 119 */
				 "XABRE"          ,"959598"          ,254,18, /* 12  */
				 "XCIERRA"        ,"98"              ,254,19, /* 14  */
				 "XLEE"           ,"989895"          ,254,20, /* 97  */
				 "XGRABA"         ,"9895"            ,254,21, /* 121 */
                 "RFBORRA"         ,"95"             ,254,22, /* 100 */
				 "XLEE_NUMERO",   "989898"           ,254,23, /* 225 */
				 "XGRABA_NUMERO", "989898"           ,254,24, /* 226 */				 
				 "SALTO_DE_LINEAS","98"              ,254,25,
				 "IP_LOCAL"       ,"95"			     ,254,26,
                 "CREA_PAQUETE_CIU","95"             ,254,27,
                 "PROCESA_PAQUETE_CIU","95"          ,254,28,
                 "LEE_PAQUETE_CIU"   ,"95"           ,254,29,
                 "ENTCOLUMNA"    ,"20989898987070"   ,254,30,
                 "XPOSICION"     ,  "989898"         ,254,31,
                 "XLEEDEDEF"     ,  "9849"           ,254,32,
                 "XGRABAADEF"    ,  "9849"           ,254,33,
                 "DSCORREO_ENVIA",  "95959595959595" ,254,34,
                 "MODO_RELACION" ,  "4998959595"     ,254,35,
                 "CAMBIO_EMPRESA",  "95"             ,254,36,
                 "ENCLAVE"       ,  "499898"         ,254,37,
                 "DSCORREO_RECIBE", "959595959595"   ,254,38,
                 "BROWSE"         , "95"             ,254,39,
                 "SUB_EJECUTA_NP" , "95"             ,254,40,
                 "LINEAL_SIMPLE"  , "20989898707070" ,254,41,
				 /* definicion de def ? */
				 NULL,NULL,255,NULL,
			};
			

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



/*************************************************************************/
/* compilador continuacion */

int busca_fina(s,c)
char *s;
char c;
{
	int i = 0,tip = 0;
	char m,n;
	
	if (c != '\"') return(busca_fin(s,c));
	
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


int busca_fin(s,c)
char *s;
char c;
{
	int i = 0,tip = 0;
	char m,n,salta = 0;
	
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
			while(*(s + i) != '\0') 
			{
				if (*(s + i) == '"' && (salta || !i || *(s + i-1) != 'A')) 
				{
					if (salta)
						salta = 0;
					else
						salta = 1;
					i++;
					continue;
				}
				if (salta || *(s + i) > m  || *(s + i) < n)
					i++;
				else
					break;
			}
			else
				while(*(s + i) != '\0') 
				{ /* en tip == 1 unas comillas suponen el fin */
					/* si no entran dentro del margen */
					if (!salta && (*(s + i) > m || *(s + i) < n))
						break;
					if (*(s + i) == '"' && (salta || !i || *(s + i-1) != 'A')) 
					{
						if (salta)
							salta = 0;
						else
							salta = 1;
					}
					i++;
				}
				
				if (salta) {
					un_error("Comillas desaparejadas","");
				}
				
				return(i);
}


int busca_con(s)
char *s;
{
	int i = 0,salta = 0;
	
	while(*(s + i) != '\0') {
		
		if (*(s + i) == '"' && (salta || !i || *(s + i-1) != 'A')) {
			if (salta)
				salta = 0;
			else
				salta = 1;
			i++;
			continue;
		}
		
		if (salta || (*(s + i) != '!' && *(s + i) != '>' &&
			*(s + i) != '<' && *(s + i) != '=' && *(s + i) != ']' &&
			*(s + i) != '[') )
			i++;
		else
			break;
	}
	
	return(i);
}

quita_blancosc(s)
char *s;
{
	int i = 0,modo = 0,at = 0;
	
	do {
		while (!modo && (*(s + i) == ' ' || *(s + i) == '\011')) i++;
		*s = *(s + i);
		if (*s) {
			if (*s == '"' && (modo || !at || *(s-1) != 'A'))
				if (modo)
					modo = 0;
				else
					modo = 1;
				s++;
				at = 1;
		}
	} while(*s);
	
}

/* esto esta montado asi por no haberme dado cuenta de la necesidad del mismo
hasta un poco tarde */
int numero_interno_prog(n)
int n;
{
	int i;
	char *nom = progr[n-1]->nombre;
	for (i = 0;i < nllamada_interna;i++)
	{
		if (!strcmp(llamada_interna[i],nom))
		{return(i+1);}
	}
	if (nllamada_interna >= 300)
	{
        un_error("!!exceso de llamadas internas!!","");		
	}
	llamada_interna[i] = malloc(strlen(nom)+1);
	strcpy(llamada_interna[i],nom);
	nllamada_interna++;
	return(nllamada_interna);
}



int pon_programa(programa,tipo)
char *programa;
int tipo;
{
	int externo = 0,i,total;
	if (programa[0] == '&') {
		if (tipo == -1)
			un_error("Declaracion de externo en una RUTINA ",programa);
		else
			externo = -1;
		programa++;
	}
	
	
	if (strlen(programa) == 0) {
		if (tipo == -1)
			un_error("Intento de definir una rutina sin nombre","");
		else
			un_error("Intento de definir un proceso sin nombre","");
		return(0);
	}
	
	nombre_bueno(programa,(tipo == -1) ? "RUTINA" : "PROCESO","");
	
	total = 0;
	for (i = 0;i < totalprogr;i++) {
		if (strcmp(progr[i]->nombre,quita_path(programa)) == 0) {
			un_error("nombre de proceso o rutina DUPLICADO",programa);
			break;
		}
		if (progr[i]->tipo != -1)
			total++;
	}
	
	if (totalprogr >= 1500) {
		un_error("EXCESO DE PROCESOS-RUTINAS","");
		return(0);
	}
	
	if (total >= 1469) {
		un_error("EXCESO DE PROCESOS","");
		return(0);
	}
	
	progr[totalprogr] = (PP *)malloc(sizeof(PP));
	progr[totalprogr]->nombre = strmalloc(strlen(programa)+1);
	strcpy(progr[totalprogr]->nombre,programa);
	progr[totalprogr]->indice = nprog;
	progr[totalprogr]->tipo = tipo;
	progr[totalprogr]->contador = 0;
	progr[totalprogr]->proceso = nen_pr;
	progr[totalprogr]->exportable = externo;
	totalprogr++;
	
	/* ojo que es diferente el uso en el generador que en runtime de la
	misma estructura */
	p_ex[np_ex] = (P_EX *)malloc(sizeof(P_EX));
	p_ex[np_ex]->nombre = strmalloc(strlen(programa)+1);
	strcpy(p_ex[np_ex]->nombre,programa);
	p_ex[np_ex]->indice = nprog;
	p_ex[np_ex]->tipo = tipo;
	p_ex[np_ex]->exportable = externo;
	np_ex++;
	
    /*
	strcpy(c_nombrecal+35,"\r");
	strcat(c_nombrecal+35,programa);
    */
	return(0);
}

int nombre_defbucle(nm)
char *nm;
{
	char nom[81];
	int i;
	int final = busca_fin(nm,';');
	
	if (final > 80) {
		un_error("Nombre de DEFBUCLE demasiado largo",nm);
	}
	memcpy(nom,nm,final);
	nom[final] = 0;
	quita_blancos(nom);
	
	if (!strlen(nom)) {
		sprintf(nom,"%d",nf_B);
		un_aviso("DEFBUCLE SIN NOMBRE SE ASUME",nom);
	}
	
	nombre_bueno(nom,"DEFBUCLE","0123456789");
	
	for (i = 0;i < nf_B;i++) {
		if (!tabla_defbucle[i]) {
			rw_error(62);
			continue;
		}
		if (!strcmp(nom,tabla_defbucle[i])) {
			un_error("NOMBRE DE DEFBUCLE REPETIDO",nom);
			break;
		}
	}
	if (tabla_defbucle[nf_B]) {
		free(tabla_defbucle[nf_B]);
	}
	tabla_defbucle[nf_B] = malloc(strlen(nom) + 1);
	strcpy(tabla_defbucle[nf_B],nom);
}

int busca_defbucle(nom)
char *nom;
{
	int i;
	
	quita_blancos(nom);
	for (i = 0;i < nf_B;i++) {
		if (!tabla_defbucle[i]) {
			rw_error(63);
			continue;
		}
		if (!strcmp(tabla_defbucle[i],nom))
			return(i);
	}
	return(-1);
}

int busca_comando(pr)
char **pr;
{
	int modo = 0,kk;
	int final,tipo;
	char programa[81];
	char tip[81];
	char *_pr;
	int ss;
	
	for (;**pr;(*pr)++) {
		if (**pr == '|' && (++(*pr))) {
			if (memcmp(*pr,"FICHEROS",8) == 0) {
				prev_modo = 0;
				modo = 1;
				break;
			}
			if (memcmp(*pr,"DEFBUCLE",8) == 0) {
				prev_modo = 0;
				modo = 5;
				nombre_defbucle((*pr) + 8);
				break;
			}
			if (memcmp(*pr,"CAMPOS",6) == 0) {
				prev_modo = 0;
				modo = 2;
				break;
			}
			if (memcmp(*pr,"VARIABLES",9) == 0) {
				prev_modo = 0;
				modo = 3;
				break;
			}
			
			if (memcmp(*pr,"LOCALIZA",9) == 0) {
				prev_modo = 0;
				modo = 10;
				break;
			}

			if (memcmp(*pr,"INCLUYE",7) == 0) {
				prev_modo = 0;
				final = busca_fin((*pr) + 7,';');
				if (final > 80) final = 80;
				memcpy(tip,(*pr)+7,final);
				tip[final] = '\0';
				quita_blancos(tip);
				if (tip == quita_path(tip)) 
                {
					_pr = notacion_dir_aplicacion;
					if (!_pr) _pr = "?";

					if (_pr != quita_path(_pr) && *_pr == ':' && *(_pr+1) != ':') 
                    {
						strcpy(programa,dirbase_);
						kk = *(quita_path(_pr));
						*(quita_path(_pr)) = 0;
						strcat(programa,_pr);
						*(quita_path(_pr)) = kk;
					}
					else 
                    {
						strcpy(programa,dirbase);
					}
					strcat(programa,"inc/");
					strcat(programa,tip);
				}
				else {
					if (tip[0] == ':') {
						strcpy(programa,dirbase_);
						if (tip[1] == '/') {
							strcat(programa,tip+2);
						}
						else {
							kk = *(quita_path(tip));
							*(quita_path(tip)) = 0;
							strcat(programa,tip+1);
							*(quita_path(tip)) = kk;
							strcat(programa,"inc/");
							strcat(programa,quita_path(tip));
						}
					}
					else {
						strcpy(programa,tip);
					}
				}
				/* recursion */
                
				strcpy(tip,c_nombrecal);
                /*
				sprintf(tip+40,"%s\n",programa);
				sprintf(c_nombrecal,"%-35.35s",tip+40);
                */


				final = c_linea;
				gen_modo = 0;
				prev_modo = 0;
				c_linea = 0;
				linea_nivel1 = final;
				if (t_niv_punto < 100)
				{
					strcpy(t_incluyes+t_niv_punto*10,quita_path(programa));
					t_niv_punto++;
				}
				ss = t_niv_actual;
				t_niv_actual = t_niv_punto;
				tipo = compila_prog(programa);
				t_niv_actual = ss;
                
				strcpy(c_nombrecal,tip);
                
				gen_modo = 0;
				prev_modo = 0;
				c_linea = final;
				if (tipo == -1) {
					un_error("FALLO EN INCLUYE",programa);
				}
				break;
			}
			
			if (  ((modo = 7) && !memcmp(*pr,"CALCULO",7))  ||
				((modo = 8) && !memcmp(*pr,"PROGRAMA",8)) ||
				((modo = 6) && !memcmp(*pr,"RUTINA",6))   ||
				((modo = 9) && !memcmp(*pr,"PROCESO",7))
				) {
				prev_modo = 0;
				if (!memoplin) 
				{
					pon_varsistem();
				}
				
				*pr += modo;
				if (modo == 9) *pr -= 2;/* !!! */
				
				final = busca_fin(*pr,';');
				if (final > 80) final = 80;
				
				memcpy(programa,*pr,final);
				programa[final] = '\0';
				quita_blancos(programa);
				if (modo == 8 || !strcmp(programa,"PROGRAMA")) {
					modo = 8;
					if (strlen(programa)) {
						error_sintaxis(*pr);
					}
					strcpy(programa,"PROGRAMA");
				}
				*pr += final;
				
				tipo = 0;
				if (**pr != '\0') {
					*pr += 1;
					final = busca_fin(*pr,';');
					memcpy(tip,*pr,final);tip[final] = '\0';
					quita_blancos(tip);
					if (memcmp(tip,"|TIPO",5) == 0) {
						if (modo == 8 || modo == 6)
							error_sintaxis(*pr);
						tipo = atoi(tip+5);
						*pr += final;
						if (tipo < 0 || tipo > 127) {
							mal_argu(*pr);
							tipo = 0;
						}
					}
				}
				if (modo == 8 || modo == 6) tipo = -1;
				
				pon_programa(programa,tipo);
				
				if (modo == 7)
					modo = 4;
				else
					if (modo == 8)
						modo = 7;
					else
						if (modo == 9)
							modo = 8;
						break;
			}
			else
				modo = 0;
    }
 }
 *pr += busca_fin(*pr,';');
 if (**pr == ';') (*pr)++;
 return(modo);
}



int pon_campos(fichero,campo,nombre)
int fichero;
int campo;
char *nombre;
{
	int i;
	
	if (strlen(nombre) == 0) {
		un_error("Intento de definir un campo sin nombre","");
		return(0);
	}
	
	nombre_bueno(nombre,"ALIAS CAMPO","");
	
	for (i=0;i < totalcamps;i++) {
		if (strcmp(camps[i]->nombre,nombre) == 0) {
			un_error("alias de campo DUPLICADO",nombre);
			break;
		}
	}
	
	if (totalcamps >= MAX_ALIAS) {
		un_error("EXCESO DE CAMPOS DEFINIDOS","");
		return(0);
	}
	
	camps[totalcamps] = (CC *)malloc(sizeof(CC));
	camps[totalcamps]->fichero = fichero;
	camps[totalcamps]->campo = campo;
	camps[totalcamps]->nombre = strmalloc(strlen(nombre)+1);
	strcpy(camps[totalcamps]->nombre,nombre);
	totalcamps++;
	return(0);
}


pon_linea(c,indice)
char c;
int indice;
{
	int in = nprog;
	char tmp[40];
	char tm1[50];
	
	if (nprog >= MAX_PROG) {
		un_error("EXCESO DE INSTRUCCIONES EN UN CAL","");
		return(nprog-1);
	}
	
	prog[in].i = c;
	prog[in].indice = indice;
	
	
	/***** TRACER *****/
	if (t_f > -1) {
		/*if (_nivel == 1)*/
		sprintf(tm1,"%5d",c_linea);
		/*else
		sprintf(tm1,"%5d",linea_nivel1);*/
		rw_write_(t_f,tm1,5);
		t_niveles[in] = t_niv_actual;
	}
	/******************/
	
	nprog++;
	return(in);
}


static char   _nombre2[51];

ini_tracer(nom)
char *nom;
{
	char nombre[51];
	
	t_niv_punto = 0;
	t_niv_actual = 0;
	strcpy(nombre,nom);
	strcat(nombre,".map");
	t_f = rw_open_(nombre,O_BINARY | O_TRUNC | O_RDWR | O_CREAT,S_IWRITE | S_IREAD);
}

graba_tracer()
{
	int i;
	char tmp[256];
	int f,d = sizeof(short);
	short t;
#ifdef ESMOTOROLA
	short tmp_m;
#endif
	
	
	if (t_f == -1)
		return(-1);
	
	t = totalmemos;
	moto_short(&t);
	rw_write_(t_f,(char *)&t,d);
	moto_short(&t);
	for (i = 0;i < totalmemos;i++) {
		strcpy(tmp,memos[i]->nombre);
		rw_write_(t_f,tmp,13);
#ifdef ESMOTOROLA
		tmp_m = memos[i]->numero;
		memcpy(tmp,&(tmp_m),2);
		moto_short(tmp);
#else
		*((short *)tmp) = memos[i]->numero;
#endif
		rw_write_(t_f,tmp,4);
	}
	
	t = totalcamps;
	moto_short(&t);
	rw_write_(t_f,(char *)&t,d);
	moto_short(&t);
	for (i = 0;i < totalcamps;i++) {
		strcpy(tmp,camps[i]->nombre);
		rw_write_(t_f,tmp,13);
#ifdef ESMOTOROLA
		tmp_m = camps[i]->fichero;
		memcpy(tmp,&(tmp_m),2);
		moto_short(tmp);
#else
		*((short *)tmp) = camps[i]->fichero;
#endif
		rw_write_(t_f,tmp,4);
#ifdef ESMOTOROLA
		tmp_m = camps[i]->campo;
		memcpy(tmp,&(tmp_m),2);
		moto_short(tmp);
#else
		*((short *)tmp) = camps[i]->campo;
#endif
		rw_write_(t_f,tmp,4);
	}
	
	t = totalfichs;
	moto_short(&t);
	rw_write_(t_f,(char *)&t,d);
	moto_short(&t);
	for (i = 0;i < totalfichs;i++) {
#ifdef ESMOTOROLA
		tmp_m = fichs[i]->numero;
		memcpy(tmp,&(tmp_m),2);
		moto_short(tmp);
#else
		*((short *)tmp) = fichs[i]->numero;
#endif
		rw_write_(t_f,tmp,4);
#ifdef ESMOTOROLA
		tmp_m = fichs[i]->indice;
		memcpy(tmp,&(tmp_m),2);
		moto_short(tmp);
#else
		*((short *)tmp) = fichs[i]->indice;
#endif
		rw_write_(t_f,tmp,4);
	}
	
	rw_write_(t_f,t_incluyes,1000);
	rw_write_(t_f,t_niveles,nprog);
	
	
	rw_close_(t_f);
	t_f = -1;
	return(0);
}


int busca_fich(nom,f,c)
char *nom;
int *f,*c;
{
	int i;
	
	if (nom[0] == '"') return(0);
	
	for (i = 0;i < totalcamps;i++) {
		if (strcmp(nom,camps[i]->nombre) == 0) {
			*f = camps[i]->fichero;
			*c = camps[i]->campo;
			return(-1);
		}
	}
	return(0);
}

int busca_var(nom)
char *nom;
{
	int i,final;
	char valor[81];
	
	if (nom[0] == '"') { /* literal */
		nom[0] = 'A';
		final = strlen(nom) - 1;
		if (final)
			nom[final] = '\0';
		sprintf(valor,"L%d",nmemo);
		i = pon_varia(valor,nom,0);
		nom[0] = '"';nom[final] = '"';
		return(memos[i]->numero);
	}
	
	if (strcmp(nom,"Campo") == 0) return(5000); /* variables del sistema cuando se eliminan las locales*/
	if (strcmp(nom,"Contenido") == 0) return(5001);
	if (strcmp(nom,"Control") == 0) return(5002);
	if (strcmp(nom,"FEntrada") == 0) return(5003);
	if (strcmp(nom,"FSalida") == 0) return(5004);
	if (strcmp(nom,"E_sup") == 0) return(5005);
	if (strcmp(nom,"E_inf") == 0) return(5006);
	if (strcmp(nom,"Pos") == 0) return(5007);
	if (strcmp(nom,"Usuario") == 0) return(5008);
	if (strcmp(nom,"Impresora") == 0) return(5009);
	if (strcmp(nom,"PARAMETRO") == 0) return(5010);
    if (strcmp(nom,"Fichero@") == 0) return(5011);
    if (strcmp(nom,"PARAMETRO_LOCAL") == 0) return(5012);

    /* ver si es funcion 
    if (strstr(nom"{"))
    {
        i = busca_prog();
        i = numero_interno_prog(i);
        return(10000+i);
    }
    */
	
	for (i = 0;i < totalmemos;i++) 
    {
		if (strcmp(nom,memos[i]->nombre) == 0)
			return(memos[i]->numero);
	}
	
	/*
	i = busca_ex(nom);
	if (i >= 0)
	{
	return(5000 + memoplin + maxpila*2 + i);
	}
	*/
	
	if (nom[0] == '-')
		i = 1;
	else
		i = 0; 
	for (;i < strlen(nom);i++) {  
		if (nom[i] == '.') continue;
		
		if (nom[i] < '0' || nom[i] > '9')
			break;
	}
	if (i == strlen(nom)) {
		/* constantes tipo 999 que se cambia al final de todo a double */
		for (i = 0;i < nmemo;i++) {
			if (memo[i]->m_tipo == 999 && memo[i]->mv.vald == rwatof(nom))
				return(i);
		}
		memo[nmemo] = (T_MEMORIA *)malloc(sizeof(T_MEMORIA));
		memo[nmemo]->m_tipo = 999;
		memo[nmemo]->mv.vald = rwatof(nom);
		nmemo++;
		if (!c_ftv)
			fprintf(c_error,"TRAZA Constante [%s] linea %d \n\r",nom,c_linea);
		return(i);
	}
	
	if (e_nivel < 2) {
		if (nom[0] != 'A' && nom[0] != '@')
			un_aviso("Variable no declarada ->",nom);
	}
	
	if (!c_ftv)
		fprintf(c_error,"TRAZA Variable [%s] linea %d \n\r",nom,c_linea);
	
	i = pon_varia(nom,nom,0);
	return(memos[i]->numero);
}


static int busca_prog(nom)
char *nom;
{
	int i;
	
	if (!strcmp(nom,"PROGRAMA"))  {
		un_error("PROGRAMA no es accesible!","");
		return(-1);
	}
	quita_blancos(nom); /* esto puede ser innecesario:  por optimizar */
	if (strcmp(nom,"NULL") == 0) return(-1);
	for (i = 0;i < totalprogr;i++) {
		if (strcmp(nom,progr[i]->nombre) == 0)
			return((i+1));
	}

	progr[totalprogr] = (PP *)malloc(sizeof(PP));
	progr[totalprogr]->nombre = strmalloc(strlen(nom)+1);
	strcpy(progr[totalprogr]->nombre,nom);
	progr[totalprogr]->indice = 0;
	progr[totalprogr]->tipo = 0;
	progr[totalprogr]->contador = 0;
	progr[totalprogr]->proceso = nen_pr;
	progr[totalprogr]->exportable = 0;
	totalprogr++;

	return(totalprogr);
}

int cambia_fichero(f)
int *f;
{
	int i;
	
	if (*f == 48) return(0);
	
	for (i = 0;i < totalfichs;i++) {
		if (*f == fichs[i]->numero) {
			*f = fichs[i]->indice;
			return(0);
		}
	}
	un_error("fichero inexistente","");
	return(-1);
}

static int es_numero(s,max_)
char *s;
int max_;
{
	int i;
	for (i = 0;i < max_ && *s;i++,s++) {
		if (*s != ' ' && (*s > '9' || *s < '0'))
			return(0);
	}
	return(i);
}

int busca_campo_nombre(f,nom)
int f;
char *nom;
{
	int i;
	char tmp[50];
	
	for (i = 0;i < vg_mpfi(f)->f_ncampos;i++) {
		strcpy(tmp,vg_mpfi(f)->f_campos[i]->c_nombre);
		quita_blancos(tmp);
		if (strlen(tmp) && tmp[0] != '~') {
			if (!strcmp(tmp,nom))
				return(i);
		}
	}
	return(-1);
}

int devuelve_campo(nom,f,c)
unsigned char *nom;
int *f,*c;
{
	int final,fin_1,flag = 0,ind;
	int j,j1,j2 = -1;
	
	if ((int)nom[0] == 255) {
		*f = -1;
		*c = ((int)nom[1] - 200) + 5000;
		return(0);
	}
	
	if (nom[0] == '#') {
		if (nom[1] == '@') {
			*f = F_CASIVALIDO;
			final = 1;
		}
		else {
			fin_1 = busca_fin(nom+1,'#');
			final = busca_fin(nom+1,'\0');
			if (nom[fin_1+1] == '#') { /* hay una segunda parrilla */
				if (!es_numero(nom+1,fin_1)) {
					final = fin_1;
					nom[final+1] = 0;
					flag = 1;
					if ((fin_1 = ind_def_rel_f(nom+1)) < 0) {
						un_error("fichero inexistente",nom+1);
						flag = -1;
					}
					else
						*f = fin_1;
					nom[final+1] = '#';
				}
				else
					*f = (int)valsd(nom,1,final);
			}
			else /* seguramente es indireccion o error */
				*f = (int)valsd(nom,1,final);
		}
		
		ind = 0;
		j2 = -1;
		
		if (nom[final+1] == '#') {
			fin_1 = busca_fin(nom+final+2,'#');
			if (nom[final+fin_1+2] == '#') { /* tercera parrilla = indireccion */
				j1 = nom[final+fin_1+2];
				j2 = final+fin_1+2;
				nom[final+fin_1+2] = 0;
				final++;
				ind = 1;
			}
			else {
				if (!es_numero(nom+final+2,fin_1)) { /* buscar por nombre de campo */
					if (!flag) {
						if (cambia_fichero(f) < 0)
							flag = -1;
						else
							flag = 1;
					}
					if (flag >= 0) {
						fin_1 = busca_campo_nombre(*f,nom+final+2);
						if (fin_1 < 0) {
							un_error("campo ilegal",nom);
							*c = 0;
							flag = -1;
						}
						else
							*c = fin_1;
					}
				}
				else
					*c = atoi(nom+final+2);
			}
		}
		else
			ind = 1;
		
		if (ind) { /* caso indireccion */
			j = busca_var(nom+final+1);
			if (j < 0) {
				un_error("Variable ilegal para apuntar a campo ",nom+final+1);
				j = 0;
			}
			*c = -(j + 1);
		}
	}
	else {
		if (busca_fich(nom,f,c) == 0) {
			*f = -1;
			*c = busca_var(nom);
		}
	}
	if (*f != F_INVALIDO && *f >= 0) {
		fin_1 = 0;
		if (!flag)
        {
			fin_1 = cambia_fichero(f);
            if (fin_1 == -1)
                flag = -1;
        }
		if (fin_1 != -1 && flag >= 0) 
        {
			if (*f < nfi && *c >= vg_mpfi((*f))->f_ncampos) 
            {
				un_error("campo ilegal",nom);
				*c = 0; /* para evitar que reviente */
			}
		}
    }
	
	if (j2 > -1)
		nom[j2] = j1;
    if (flag == -1)
        *c = 0;

	return(0);
}

int pon_acf(pr,m,instru)
char **pr;
int m;
unsigned char instru;
{
	int i;
	int final;
	char *p = *pr;
	int f;
	char ff[40];
	int in;
	unsigned char control;
	int aj,ai,rr;
	
	/*if (minimun && rw_user[5] != '1') return(0);*/
	
	ai = busca_fin(p,';');
	aj = busca_fin(p,',');
	final = ((ai <= aj) ? ai : aj);
	rr = ((final == ai) ? 0 : 1);
	
	i = busca_fin(p,'#');
	if (final > i) {
		memcpy(ff,p+i+1,final-i-1);
		ff[final-i-1] = '\0';
		quita_blancos(ff);
		if (m == 11 && ff[0] == 'P') {
			control = 1;
		}
		else
			control = 0;
		if (es_numero(ff+control,999)) {
			f = atoi(ff+control);
			cambia_fichero(&f);
		}
		else  {
			f = ind_def_rel_f(ff+control);
			if (f < 0) {
				un_error("fichero inexistente",ff+control);
				f = 0;
			}
		}
		if (m == 11 && control) {
			f += 10000;
		}
		pon_linea(instru,f);
	}
	else {
		mal_argu(ff);
	}
	p += final;
	if (*p == ',') p++;
	*pr = p;
	return(rr);
}


pon_bcf(pr,m,instru)
char **pr;
int m;
unsigned char instru;
{
	int i,fin_1;
	int final;
	char *p = *pr;
	int f,o,b,t;
	unsigned char ff[80];
	int in;
	int aj,ai,rr;
	
	ai = busca_fin(p,';');
	aj = busca_fin(p,',');
	final = ((ai <= aj) ? ai : aj);
	rr = ((final == ai) ? 0 : 1);
	
	if (final > 80) {
		error_sintaxis(*pr);
		final = 0;
	}
	else
		memcpy(ff,p,final);
	ff[final] = '\0';
	quita_blancos(ff);
	
	if (strlen(ff) < 6) {
		mal_argu(p);
		*pr = (p + final);
		return(rr);
	}
	
	if ( (int)(ff[0] - 48) < 0 ||
		(int)(ff[1] - 48) < 0 ||
		(int)(ff[2] - 48) < 0 ||
		(int)(ff[0] - 48) > 1 ||
		(int)(ff[1] - 48) > 4 ||
		(int)(ff[1] - 48) == 3 ||
		(int)(ff[2] - 48) > 2) {
		mal_argu(p);
	}
	
	b = (int)(ff[0] - '0');
	if (b != 1) b = 0;
	t = (int)(ff[1] - '0') * 10 + (int)(ff[2] - '0');
	if (t < 0) t = 0;
	in = busca_fin(ff,'#');
	if (ff[in] == '#') 
	{
		fin_1 = busca_fin(ff+in+1,'.');
		if (ff[in+1+fin_1] == '.') 
		{
			i = busca_fin(ff+in+1,'\0');
			if (i == fin_1)
			{
				o = (int)ff[in+2+fin_1];
				ff[in+1+fin_1] = 0;
				f = atoi(ff+in+1);
				cambia_fichero(&f);		
			}
			else
			{
				o = (int)ff[in+2+fin_1];
				ff[in+1+fin_1] = 0;
				f = ind_def_rel_f(ff+in+1);
			}
			if (f < 0) 
			{
				f = 0;
				un_error("fichero inexistente",ff+in+1);
			}
		}
		else 
		{
			i = busca_fin(ff+in+1,'\0');
			o = (int)ff[in+1+i];
			if (strlen(ff+in+2+i))
			{
				mal_argu(p);
			}
			ff[in+1+i] = '\0';
			f = atoi(ff+in+1);
			cambia_fichero(&f);
		}
		
		
		switch (m) {
		case 0: /* LEER */
			if (o == 61)
				o = ISEQUAL;
			else
				if (o == 62)
					o = ISGREAT;
				else
					if (o == 93)
						o = ISGTEQ;
					else
						if (o == 99)
							o = ISCURR;
						else
							if (o == 115)
								o = ISNEXT;
							else
								if (o == 97)
									o = ISPREV;
								else
									if (o == 112)
										o = ISFIRST;
									else
										if (o == 117)
											o = ISLAST;
										else {
											mal_argu(p);
										}
										break;
		case 1: /* GRABAR */
			if (o == 97)
				o = 0;
			else
				if (o == 110)
					o = 1;
				else
					if (o == 99)
						o = 2;
					else
						if (o == 98)
							o = 3;
						else {
							mal_argu(p);
						}
						break;
		case 2: /* BORRAR */
			if (o == 97)
				o = 0;
			else
				if (o == 99)
					o = 1;
				else {
					mal_argu(p);
				}
				break;
		default:
			mal_argu(p);
			break;
		}
		pon_linea(instru,f);
		pon_linea(255,o);
		pon_linea(255,b);
		pon_linea(255,t);
  }
  else {
	  mal_argu(p);
  }
  p += final;
  if (*p == ',') p++;
  *pr = p;
  return(rr);
}

int pon_ccf(pr,m,instru)
char **pr;
int m;
unsigned char instru;
{
	int i;
	int final;
	char *p = *pr;
	int f,p0;
	unsigned char ff[80];
	int in;
	int malo = 0;
	int aj,ai,rr;
	
	ai = busca_fin(p,';');
	aj = busca_fin(p,',');
	final = ((ai <= aj) ? ai : aj);
	rr = ((final == ai) ? 0 : 1);
	
	if (final > 80) {
		error_sintaxis(*pr);
		final = 0;
    }
	else
		memcpy(ff,p,final);
	ff[final] = '\0';
	quita_blancos(ff);
	in = busca_fin(ff,'#');
	if (ff[in] == '#') {
		i = busca_fin(ff+in+1,'#');
		if (ff[in+1+i] == '#') {
			ff[in+1+i] = '\0';
			if (es_numero(ff+in+2+i,999)) {
				f = atoi(ff+in+2+i);
				if (cambia_fichero(&f) < 0) goto sale_mal;
			}
			else {
				f = ind_def_rel_f(ff+in+2+i);
				if (f < 0) {
					un_error("fichero inexistente",ff+in+2+i);
					f =0;
				}
			}
			
			if (es_numero(ff+in+1,999)) {
				p0 = atoi(ff+in+1);
				if (m == 3) {
					if (cambia_fichero(&p0) < 0) goto sale_mal;
				}
			}
			else {
				if (m == 3) {
					p0 = ind_def_rel_f(ff+in+1);
					if (p0 < 0) {
						un_error("fichero inexistente",ff+in+1);
						p0 =0;
					}
				}
				else
					if (m == 5 || m == 6) {
						p0 = busca_campo_nombre(f,ff+in+1);
						if (p0 < 0)
							malo = 1;
					}
			}
			
			switch (m) {
			case 0:
				if (p0 < 0 || p0 > vg_mpfi(f)->f_nclaves)
					malo = 1;
				break;
			case 1:
				if (p0 < 0 || p0 >= vg_mpfi(f)->f_npanta)
					malo = 1;
				break;
			case 2:
				if (vg_mpfirel(f)->lineas < -1) {
					if (p0 < 0 || p0 > 3)
						malo = 1;
				}
				else
					if (p0 < 0 || p0 >= vg_mpfi(f)->f_npanta)
						malo = 1;
					break;
			case 3:
				/* cambia_fichero(&p0); *//* se hace arriba */
				break;
			case 4:
				if (p0 < 1 || p0 > 4)
					malo = 1;
				break;
			case 5:
				if (p0 < 0 || p0 >= vg_mpfi(f)->f_ncampos)
					malo = 1;
				break;
			case 6:
				if (p0 < 0 || p0 >= vg_mpfi(f)->f_ncampos)
					malo = 1;
				break;
			default:
				malo = 1;
				break;
			}
			if (malo)  {
				mal_argu(p);
			}
			pon_linea(instru,p0*100+f);
		}
		else
			mal_argu(p);
	}
	else {
		mal_argu(p);
	}

sale_mal:
	
	p += final;
	if (*p == ',') p++;
	*pr = p;
	return(rr);
}

pon_dcf(pr,m)
char **pr;
int m;
{
	int i;
	int final;
	char *p = *pr;
	unsigned char ff[80];
	int i1,i2,in;
	int aj,ai,rr;
	
	final = busca_fin(p,';');
	rr = 0;
	
	
	if (final > 80) {
		error_sintaxis(*pr);
		final = 0;
    }
	else
		memcpy(ff,p,final);
	ff[final] = '\0';
	quita_blancosc(ff);
	i1 = busca_fin(ff,',');
	if (ff[i1] == ',') {
		ff[i1] = '\0';
		i1++;
		i2 = busca_fin(ff+i1,',');
		i2 += i1;
		if (ff[i2] == ',') {
			ff[i2] = '\0';
			i2++;
			switch (m) {
			case 0:
				in = OGRAPA;
				break;
			case 1:
				in = OPINPA;
				break;
			default:
				break;
			}
			quita_blancosc(ff);
			quita_blancosc(ff+i1);
			quita_blancosc(ff+i2);
			i = pon_opera(ff,ff+i1,in,ff+i2);
			pon_linea(TOPER,i);
		}
		else {
			mal_argu(p);
		}
	}
	else {
		mal_argu(p);
	}
	p += final;
	if (*p == ',') p++;
	*pr = p;
	return(rr);
}

int busca_palabra(s1,s2,max_) /* SPACES NOT ALLOWED IN MATCH EXPRESION */
char *s1,*s2;
int max_;
{
	int i;
	int li = strlen(s2);
	int lo = strlen(s1);
	if (lo > max_) lo = max_;
	if (li > lo) return(0);
	lo -= li;
	for (i = 0;i <= lo;i++) {
		if (s1[i] == ' ') continue;
		if (s1[i] == s2[0]) {
			if (!memcmp(s1,s2,li))
				return(1);
		}
	}
	return(0);
}

int com_ficheros(pr)
char **pr;
{
	int i,j;
	int final,coma;
	char fichero[256];
	int numero;
	char *p = *pr;
	int modo = 1;
	
	for (i = 0;*(p + i);i++) {
		if (memcmp(p + i,"|FIN",4) == 0) {modo = prev_modo;i += 4;break;}
		
		if (memcmp(p+i,"||",2) == 0) {
			i += busca_fin(p+i,'\376');
			break;
		}
		
		coma = 0;
		if (*(p+i) == '#' || *(p+i) == ';' || *(p+i) == ',') {
			if (!i) continue;
			memcpy(fichero,p,i);fichero[i] = '\0';
			quita_blancos(fichero);
			if (!strlen(fichero)) continue;
			if (*(p+i) != ';') {
				final = busca_fin(p+i+1,';');
				if (*(p+i) != ',')
					coma = busca_fin(p+i+1,',');
				else
					coma = -1;
			}
			else {
				final = -1;
				coma = 0;
			}
			if (coma < final) {
				if (!busca_palabra(p+i+2+coma,"MANTE",final-coma+1)) {
					mal_argu(p);
					coma = 0;
				}
				else {
					j = final;
					if (coma == -1)
						final = -1;
					else
						final = coma;
					coma = j;
				}
			}
			else
				coma = 0;
			if (final != -1) {
				if (!es_numero(p+i+1,final))
					mal_argu(p);
				numero = (int)valsd(p+i,1,final);
			}
			else
				numero = -1;
			
			if (numero == 48 || (numero < 0 && final != -1) || numero > 32000) {
				mal_argu(p);
			}
			else {
				pon_fichero(fichero,numero,coma,NULL);
			}
			
			if (coma) {
				final = coma;
			}
			if (final == -1) {
				if (*(p+i) == ';')
					p += (i+1);
			}
			else
				if (*(p+i+final+1) == ';') {
					p += (i + final + 2);
					i = -1;
				}
		}
		if (*(p + i) == ';') {p += (i + 1);i = -1;}
		if (!*(p+i)) break;  
	}
	*pr = (p + i);
	return(modo);
}

int com_campos(pr)
char **pr;
{
	int i;
	int final;
	char nombre[256];
	int fichero;
	int campo;
	char *p = *pr;
	int modo = 2;
	
	for (i = 0;*(p + i);i++) {
		if (memcmp(p + i,"|FIN",4) == 0) {modo = prev_modo;i += 4;break;}
		if (memcmp(p+i,"||",2) == 0) {
			i += busca_fina(p+i,'\376');
			break;
		}
		if (*(p+i) == '#') {
			final = busca_fina(p+i+1,'#');
			if (*(p+i+final+1) != '#') {
				mal_argu(p);
				break;
			}
			fichero = (int)valsd(p+i,1,final);
			p += (i+final+2);i = 0;
			final = busca_fina(p,'\0');
			campo = valsd(p,0,final);p += final;
			final = busca_fin(p,';');
			memcpy(nombre,p,final);nombre[final] = '\0';
			quita_blancos(nombre);
			pon_campos(fichero,campo,nombre);
			p += (final-1);
		}
		if (*(p + i) == ';') {p += (i + 1);i = -1;}
		if (!*(p+i)) break;  
	}
	*pr = (p + i);
	return(modo);
}

int com_bucle(pr)
char **pr;
{
	int i,in,l,cc,xxx,xx;
	int final,f,c,h,j;
	short *prov;
	char *p = *pr;
	short modo = 5;
	char ff[80];
	char ff0[50];
	char ff1[MAXDEFECTO+1];
    int  j_orden = 0;
	static short   pila[MAX_BUCLE];
	static short _nfi;
	static short _c;
	static int laclave;
	static short como;
	
	while(*p) {
		if (*p == ' ') 
        {
			p++;
			continue;
		}
		if (memcmp(p,"|FIN",4) == 0) {
			b_punto = 0;modo = prev_modo;p += 4;
			break;
		}
		if (memcmp(p,"||",2) == 0) {
			p += busca_fina(p,'\376');
			break;
		}
		if (b_punto == 0) {
			final = busca_fina(p,';');
			memcpy(ff,p,final);
			ff[final] = '\0';
			quita_blancos(ff);
			p += final;
			if (ff[0] != '#') {
				mal_argu(p);
				*p = '\0';
				break;
			}
			c = 1;
			i = busca_fina(ff+1,'#');
			if (ff[i+1] == '#') {
				c = atoi(ff+i+2);
				ff[i+1] = '\0';
			}
			if (es_numero(ff+1,999)) {
				f = atoi(ff+1);
				cambia_fichero(&f);
			}
			else {
				f = ind_def_rel_f(ff+1);
				if (f < 0) {
					f = 0;
					un_error("fichero inexistente",ff+1);
				}
			}
			
			if (nf_B >= MAX_BUCLE) {
				un_error("EXCESO DE DEFBUCLE","");
				break;
			}
			if (c < 0 || c > vg_mpfi(f)->f_nclaves) 
			{
				if (c > 1000)
				{
					como = 0;
					l = c % 1000;
					c /= 1000;
				}
				else
				{
					un_error("Clave Ilegal en defbucle","");
					break;
				}
			}
			else
			{
				if (c == 0) 
				{
					como = 1;
					l = 0;	
				}
				else 
				{
					como = 0;
					l = vg_mpfi(f)->f_clave[c-1].k_ncampos;
				}
			}
			f_B[nf_B] = (struct _filebucle *)malloc(sizeof(struct _filebucle));
			f_B[nf_B]->nfi = _nfi = f;
			f_B[nf_B]->nord1 = -c;
			laclave = c - 1;
			f_B[nf_B]->nord2 = 0;
			f_B[nf_B]->flag = 0;
			f_B[nf_B]->ncomp = _c = l;
			f_B[nf_B]->ord1 = (short *)malloc((l+1)*sizeof(short));
			*(f_B[nf_B]->ord1 + l) = -32535;
			f_B[nf_B]->comp = NULL;
			if (l)
				f_B[nf_B]->comp = (short *)malloc(l*sizeof(short));
			f_B[nf_B]->calc = (short *)malloc(20 * sizeof(short));
			nf_B++;
			b_punto = 1;
		}
		else
			if (b_punto == 1) 
			{
				final = busca_fina(p,';');
				if (final > 0) memcpy(ff,p,final);
				ff[final] = '\0';
				quita_blancos(ff);
				p += final;
				in = -1;
				if (final != 0 && strlen(ff) > 0)
					do {
						in++;
						i = busca_fina(ff+in,',');
						l = f_B[nf_B-1]->ncomp;
						prov = (short *)malloc((l+1)*sizeof(short));
						if (l > 0) 
						{
							memcpy(prov,f_B[nf_B-1]->comp,l * sizeof(short));
							free((char *)f_B[nf_B-1]->comp);
						}
						*(prov+l) = (int)valsd(ff+in,0,i);
						f_B[nf_B-1]->comp = prov;
						if (como) 
						{
							prov = (short *)malloc((l+2)*sizeof(short));
							if (l) 
							{
								memcpy(prov,f_B[nf_B-1]->ord1,l * sizeof(short));
							}
							free((char *)f_B[nf_B-1]->ord1);
							*(prov + l) = (int)valsd(ff+in,0,i);
							*(prov + l + 1) = -32535;
							f_B[nf_B-1]->ord1 = prov;
						}
						(f_B[nf_B-1]->ncomp)++;
						in += i;
					} while(ff[in] != '\0');
					b_punto = 2;
			}
			else
				if (b_punto == 2) {
					final = busca_fina(p,';');
					if (final > 0) memcpy(ff,p,final);
					ff[final] = '\0';
					quita_blancosc(ff);
					p += final;
					in = -1;
					l = 0;
					
					if (final != 0 && strlen(ff) > 0)
					{
						if (!strcmp(ff,"INICIO"))
						{
							for (l = 0;l < f_B[nf_B-1]->ncomp;l++)
							{
                                j_orden = 1;
								sprintf(ff,"__I_B_%03d%03d",nf_B,l);
								j = 0;
								if (l < _c)
								{
									if (laclave >= 0)
									{
										j = vg_mpfi(_nfi)->f_clave[laclave].k_campo[l] - 1;
                                        j_orden = vg_mpfi(_nfi)->f_clave[laclave].k_orden[l] - 1;
									}
								}
								else
								{
									j = *(f_B[nf_B-1]->comp+l);
								}
                                if (j_orden < 0)
                                   maximo_campo_gen(vg_mpfi(_nfi),j,ff1);
                                else
								   minimo_campo_gen(vg_mpfi(_nfi),j,ff1);								
								j = pon_varia(ff,ff1,0);
								c = memos[j]->numero;
								c = -(c + 1);
								f = -1;
								if (l < _c) 
								{
									*(f_B[nf_B-1]->ord1+l) = c;
								} 
								else 
								{
									pila[(l-_c)] = c;
								}
							}
						}
						else
							do {
								in++;
								i = busca_fina(ff+in,',');
								memcpy(ff0,ff+in,i);
								ff0[i] = '\0';
								f = F_INVALIDO;c = -1;
								devuelve_campo(ff0,&f,&c);
								if (c < 0) 
								{
									un_error("Declaracion ilegal ",p);
									c = 0;
									break;
								}
								
								if (f > -1)
									xxx = vg_mpfi(f)->f_campos[c]->c_tipo;
								else
									xxx = que_memo_tipo(c);
								
								if (f >= 0)
									c = (c * 100 + f);
								else
									c = -(c + 1);
								if (f_B[nf_B-1]->ncomp <= l) 
								{
									un_error("DEMASIADOS LIMITES EN DEFBUCLE",p);
									break;
								}
								if (l < _c) 
								{
									xx = CHARTIPO;
									if (vg_mpfi(_nfi)->f_clave[0].k_ncampos && laclave >= 0)
									{
										xx = vg_mpfi(_nfi)->f_clave[laclave].k_campo[l] - 1;
										xx = vg_mpfi(_nfi)->f_campos[xx]->c_tipo;
									}		  
									*(f_B[nf_B-1]->ord1+l) = c;
								}
								else 
								{
									xx = *(f_B[nf_B-1]->comp+l);
							        xx = vg_mpfi(_nfi)->f_campos[xx]->c_tipo;
									pila[(l-_c)] = c;
								}
								
								if (vg_mpfi(_nfi)->f_clave[0].k_ncampos)
								{
									if (xxx == DOUBLETIPO || xxx == INTTIPO || xxx == 999)
									{
										if (xx != DOUBLETIPO && xx != INTTIPO)
										{
											un_error("AVISO: TIPOS INCOMPATIBLES EN DEFBUCLE",p);
										}
									}
									else
									{
										if (xx == DOUBLETIPO || xx == INTTIPO)
										{
											un_error("AVISO: TIPOS INCOMPATIBLES EN DEFBUCLE",p);
										}		  
									}
								}
								in += i;
								l++;
							} while(ff[in] != '\0');
					}
					if (f_B[nf_B-1]->ncomp > l) 
					{
						un_error("FALTAN LIMITES EN DEFBUCLE",p);
					}
					b_punto = 3;
     }
	 else
		 if (b_punto == 3) {
			 final = busca_fina(p,';');
			 if (final > 0) memcpy(ff,p,final);
			 ff[final] = '\0';
			 quita_blancosc(ff);
			 p += final;
			 in = -1;
			 l = 0;
			 if (final != 0 && strlen(ff) > 0)
			 {
				 if (!strcmp(ff,"FINAL"))
				 {
					 for (l = 0;l < f_B[nf_B-1]->ncomp;l++)
					 {
						 sprintf(ff,"__F_B_%03d%03d",nf_B,l);
						 j = 0;
                         j_orden = 1;
						 if (l < _c)
						 {
							 if (laclave >= 0)
							 {
								 j = vg_mpfi(_nfi)->f_clave[laclave].k_campo[l] - 1;
                                 j_orden = vg_mpfi(_nfi)->f_clave[laclave].k_orden[l] - 1;
							 }
						 }
						 else
						 {
							 j = *(f_B[nf_B-1]->comp+l);
						 
                         }
                         if (j_orden < 0)
                            minimo_campo_gen(vg_mpfi(_nfi),j,ff1);
                         else
						    maximo_campo_gen(vg_mpfi(_nfi),j,ff1);
						 j = pon_varia(ff,ff1,0);
						 c = memos[j]->numero;			
						 f = -1;
						 if (l < _c) 
						 {
							 c = -(c + 1);
							 *(f_B[nf_B-1]->comp+l) = c;
						 }
						 else 
						 {
							 j = *(f_B[nf_B-1]->comp+l);
							 *(f_B[nf_B-1]->comp+l) =  pon_condicion(_nfi,j,f,c,CMEIGU,&h);
							 Ylogico = 1;
							 c = pila[(l-_c)];
							 if ( c < 0) 
							 {
								 f = -1;
								 c = -(c) - 1;
							 }
							 else 
							 {
								 f = c % 100;
								 c /= 100;
							 }
							 pon_condicion(_nfi,j,f,c,CMAIGU,&h);
							 Ylogico = 0;
						 }
					 }
				 }
				 else
					 do {
						 in++;
						 i = busca_fina(ff+in,',');
						 memcpy(ff0,ff+in,i);
						 ff0[i] = '\0';
						 f = F_INVALIDO;c = -1;
						 devuelve_campo(ff0,&f,&c);
						 if (c < 0) 
						 {
							 un_error("Declaracion ilegal ",p);
							 c = 0;
						 }
						 if (f_B[nf_B-1]->ncomp <= l) 
						 {
							 un_error("DEMASIADOS LIMITES EN DEFBUCLE",p);
							 break;
						 }
						 
						 if (f > -1)
							 xxx = vg_mpfi(f)->f_campos[c]->c_tipo;
						 else
							 xxx = que_memo_tipo(c);
						 
						 if (l < _c) 
						 {
							 xx = CHARTIPO;
							 if (vg_mpfi(_nfi)->f_clave[0].k_ncampos && laclave >= 0)
							 {
								 xx = vg_mpfi(_nfi)->f_clave[laclave].k_campo[l] - 1;
								 xx = vg_mpfi(_nfi)->f_campos[xx]->c_tipo;
							 }		  
							 if (f >= 0)
								 c = (c * 100 + f);
							 else
								 c = -(c + 1);
							 *(f_B[nf_B-1]->comp+l) = c;
						 }
						 else 
						 {
							 xx = *(f_B[nf_B-1]->comp+l);
							 xx = vg_mpfi(_nfi)->f_campos[xx]->c_tipo;
							 
							 j = *(f_B[nf_B-1]->comp+l);
							 *(f_B[nf_B-1]->comp+l) =  pon_condicion(_nfi,j,f,c,CMEIGU,&h);
							 Ylogico = 1;
							 c = pila[(l-_c)];
							 if ( c < 0) 
							 {
								 f = -1;
								 c = -(c) - 1;
							 }
							 else 
							 {
								 f = c % 100;
								 c /= 100;
							 }
							 pon_condicion(_nfi,j,f,c,CMAIGU,&h);
							 Ylogico = 0;
						 }
						 if (vg_mpfi(_nfi)->f_clave[0].k_ncampos)
						 {
							 if (xxx == DOUBLETIPO || xxx == INTTIPO || xxx == 999)
							 {
								 if (xx != DOUBLETIPO && xx != INTTIPO)
								 {
									 un_error("AVISO: TIPOS INCOMPATIBLES EN DEFBUCLE",p);
								 }
							 }
							 else
							 {
								 if (xx == DOUBLETIPO || xx == INTTIPO)
								 {
									 un_error("AVISO: TIPOS INCOMPATIBLES EN DEFBUCLE",p);
								 }		  
							 }
						 }
						 in += i;
						 l++;
					 } while(ff[in] != '\0');
	  }
	  if (f_B[nf_B-1]->ncomp > l) 
	  {
		  un_error("FALTAN LIMITES EN DEFBUCLE",p);
	  }
	  b_punto = 4;
     }
	 else
		 if (b_punto == 4) {
			 final = busca_fina(p,';');
			 if (final > 0) memcpy(ff,p,final);
			 ff[final] = '\0';
			 quita_blancos(ff);
			 p += final;
			 f = 0;
			 j = strlen(ff);
			 if (j > 2) {
				 error_sintaxis(p);      
			 }
			 for (;j > 0;j--) {
				 if (ff[j-1] == 'e') {
					 if ( (f / 10) )
						 error_sintaxis(p);                     
					 else
						 f += 10;   
				 }
				 else   
					 if (ff[j-1] == 'b') {
						 if ( (f % 10) )
							 error_sintaxis(p);                     
						 else
							 f += 1;   
					 }
					 else
						 if (ff[j-1] == 'f') {
							 if ( (f % 10) )
								 error_sintaxis(p);                     
							 else
								 f += 2;   
						 }
						 else
							 error_sintaxis(p);                        
			 }
			 f_B[nf_B-1]->flag = (11200 + f);
			 b_punto = 6;
		 }
		 else
			 if (b_punto == 6) {
				 final = busca_fina(p,';');
				 if (final > 0) memcpy(ff,p,final);
				 ff[final] = '\0';
				 quita_blancos(ff);
				 p += final;
				 in = -1;
				 l = 0;
				 *(f_B[nf_B-1]->calc) = -32535;
				 j = -1;
				 cc = 0;
				 if (final != 0 && strlen(ff) > 0)
					 do {
						 in++;
						 i = busca_fin(ff+in,',');
						 if (i > 0) {
							 memcpy(ff0,ff+in,i);
							 ff0[i] = '\0';
							 f = busca_prog(ff0); /* esto tiene validez en cuanto hay que comprobar al generar la integridad del asunto */
							 if (f >= 0) {
								 if (progr[f-1]->tipo == -1) {
									 un_error("ilegal uso de una rutina",ff0);
								 }
								 f = numero_interno_prog(f); /* aqui va segun tabla_cal */
								 h = 0;
								 while(*(f_B[nf_B-1]->calc + h) != -32535) h++;
								 switch (l) {
								 case 0:
									 *(f_B[nf_B-1]->calc + h) = 1000 + f;
									 *(f_B[nf_B-1]->calc + h + 1) = -32535;
									 cc = 1;
									 break;
								 case 1:
									 *(f_B[nf_B-1]->calc + h) = 11000 + f;
									 *(f_B[nf_B-1]->calc + h + 1) = -32535;
									 cc = 1;
									 break;
								 case 2:
									 *(f_B[nf_B-1]->calc + h) = 11500 + f;
									 *(f_B[nf_B-1]->calc + h + 1) = -32535;
									 cc = 1;
									 break;
								 case 3:
									 *(f_B[nf_B-1]->calc + h) = 11000 + f;
									 *(f_B[nf_B-1]->calc + h + 1) = -32535;
									 cc = 1;
									 break;
								 case 4:
									 j = f;
									 *(f_B[nf_B-1]->calc + h) = 3000 + f;
									 *(f_B[nf_B-1]->calc + h + 1) = -32535;
									 cc = 1;
									 break;
								 case 5:
									 *(f_B[nf_B-1]->calc + h) = 21000 + f;
									 *(f_B[nf_B-1]->calc + h + 1) = -32535;
									 cc = 1;
									 break;
								 case 6:
									 *(f_B[nf_B-1]->calc + h) = 21500 + f;
									 *(f_B[nf_B-1]->calc + h + 1) = -32535;
									 cc = 1;
									 break;
								 case 7:
									 *(f_B[nf_B-1]->calc + h) = 21000 + f;
									 *(f_B[nf_B-1]->calc + h + 1) = -32535;
									 cc = 1;
									 break;
								 default : break;
								 }
							 }
							 else {
								 if (strcmp(ff0,"NULL")) {
									 un_error("DEFBUCLE: No encuentro el PROCESO",ff0);
								 }
							 }
						 }
						 in += i;
						 l++;
					 } while(ff[in] != '\0');
					 
					 if (j >= 0) {
						 h = 0;
						 while(*(f_B[nf_B-1]->calc + h) != -32535) h++;
						 *(f_B[nf_B-1]->calc + h) = 7000 + j;
						 *(f_B[nf_B-1]->calc + h + 1) = -32535;
					 }
					 
					 b_punto = 7;
			 }
			 else
				 if (b_punto == 7) {
					 f = f_B[nf_B-1]->nfi;
					 for (i=0;i<vg_mpfi(f)->f_clave[0].k_ncampos;i++) {
						 l = f_B[nf_B-1]->nord2;
						 prov = (short *)malloc((l+1)*sizeof(short));
						 if (l > 0) {
							 memcpy(prov,f_B[nf_B-1]->ord2,l * sizeof(short));
							 free(f_B[nf_B-1]->ord2);
						 }
						 *(prov+l) = ((vg_mpfi(f)->f_clave[0].k_campo[i] - 1)*100 + f);
						 f_B[nf_B-1]->ord2 = prov;
						 f_B[nf_B-1]->nord2++;
					 }
					 final = busca_fina(p,';');
					 if (final > 0) memcpy(ff,p,final);
					 ff[final] = '\0';
					 quita_blancosc(ff);
					 p += final;
					 in = -1;
					 if (final != 0 && strlen(ff) > 0)
						 do {
							 in++;
							 i = busca_fina(ff+in,',');
							 l = f_B[nf_B-1]->nord2;
							 prov = (short *)malloc((l+1)*sizeof(short));
							 if (l > 0) {
								 memcpy(prov,f_B[nf_B-1]->ord2,l * sizeof(short));
								 free(f_B[nf_B-1]->ord2);
							 }
							 memcpy(ff0,ff+in,i);
							 ff0[i] = '\0';
							 f = F_INVALIDO;c = -1;
							 devuelve_campo(ff0,&f,&c);
							 if (c < 0) {
								 un_error("Declaracion ilegal ",p);
								 c = 0;
							 }
							 if (f >= 0)
								 c = (c * 100 + f);
							 else
								 c = -(c + 1);
							 *(prov+l) = c;
							 f_B[nf_B-1]->ord2 = prov;
							 f_B[nf_B-1]->nord2++;
							 in += i;
						 } while(ff[in] != '\0');
						 f_B[nf_B-1]->nord2 *= 100;
						 f_B[nf_B-1]->nord2 += vg_mpfi((f_B[nf_B-1]->nfi))->f_clave[0].k_ncampos;
						 b_punto = 0;
				 }
				 if (*p == ';') p++;
				 if (!*p) break;  
  }
  *pr = p;
  return(modo);
}



int pon_opera(nom1,nom2,op,nomr)
unsigned char *nom1,*nom2;
unsigned char op;
unsigned char *nomr;
{
int fichero1 = F_INVALIDO,campo1 = -1;
int fichero2 = F_INVALIDO,campo2 = -1;
int ficheror = F_INVALIDO,campor = -1;
int final,in;

if (strlen(nom1) > 0 && strcmp(nom1,"NULL") != 0)
	devuelve_campo(nom1,&fichero1,&campo1);

if (strlen(nom2) > 0 && strcmp(nom2,"NULL") != 0)
	devuelve_campo(nom2,&fichero2,&campo2);

if (strlen(nomr) > 0 && strcmp(nomr,"NULL") != 0)
	devuelve_campo(nomr,&ficheror,&campor);

if (op == OADIR) {
	if (ficheror != -1 || fichero1 != -1 || campor < 0 || campo1 < 0) {
		return(-1);
	}
}

if ((int)nomr[0] == 255) {
	ficheror = -2; /* marca de encadenamiento */
}
in = pon_oper(fichero1,campo1,fichero2,campo2,ficheror,campor,op);
return(in);
}

int pon_oper(f1,c1,f2,c2,fr,cr,op)
int f1,c1,f2,c2,fr,cr,op;
{
int in = noper;

if (noper >= MAX_OPER) {
	un_error("EXCESO DE OPERACIONES FATAL!","");
	return(noper-1);
}

if (fr == -1 && cr > -1  && cr < 5000 && memo[cr]->m_tipo == 999) {
	un_error("Uso indebido de una Constante","");
	return(noper-1);
}

oper[in] = (OPERA *)malloc(sizeof(OPERA));

oper[in]->fichero1 = f1;
oper[in]->campo1 = c1;
oper[in]->fichero2 = f2;
oper[in]->campo2 = c2;
oper[in]->ficheror = fr;
oper[in]->campor = cr;
oper[in]->operacion = op;
noper++;


return(in);
}

int pon_condicion(f1,c1,f2,c2,c,ulcon)
int f1,c1,f2,c2;
char c;
int *ulcon;
{
int in = ncond;

if (ncond >= MAX_COND) 
{
	un_error("EXCESO DE CONDICIONES FATAL!","");
	return(ncond-1);
}


if ((Ylogico != 0 || Ologico != 0) && *ulcon == -1) 
{
	un_error("Y o O sin SI","");
	return(0);
}


cond[in] = (CONDIC *)malloc(sizeof(CONDIC));
cond[in]->fichero1 = f1;
cond[in]->campo1 = c1;
cond[in]->fichero2 = f2;
cond[in]->campo2 = c2;
cond[in]->condicion = c;
cond[in]->sigue = -1;
cond[in]->tipo = -1;
ncond++;

if (ulcon != (int *)0) {
	if (Ylogico != 0) {
		cond[*ulcon]->sigue = in;
		cond[*ulcon]->tipo = 0;
	}
	if (Ologico != 0) {
		cond[*ulcon]->sigue = in;
		cond[*ulcon]->tipo = 1;
	}
	*ulcon = in;
}
return(in);
}

int pon_cond(ind,modo)
int ind,modo;
{
int in;

if (modo == 0) {
	
	if (ncondici >= 20) {
		un_error("demasiados SI anidados","");
		return(nprog-1);
	}
	
	in = pon_linea(TCOND,ind);
	condici[ncondici] = pon_linea(TGOCO,nprog + 1);
	ncondici++;
}
else
	if (modo == 1) {
		if (!ncondici) {
			un_error("SINO sin SI","");
			return(0);
		}
		in = condici[ncondici - 1];
		condici[ncondici - 1] = pon_linea(TGOTO,nprog + 1);
		prog[in].indice = condici[ncondici - 1] + 1;
	}
	else
		if (modo == 2) {
			if (!ncondici) {
				un_error("FINSI sin SI","");
				return(0);
			}
			ncondici--;
			in = condici[ncondici];
			prog[in].indice = nprog;
		}
		else
			if (modo == 3) {
				return(ncondici);
			}
			else
				if (modo == 4) {
					ncondici = 0;
					return(ncondici);
				}
				
				return(in);
}

int pon_bucle(op1,con,op2,modo)
int op1;
int con;
int op2,modo;
{
static int nbucles = 0;
static int bucle[30];
int in,indice;

if (modo == 2) {
	return(nbucles);
}

if (modo == 0) {
	
	if (nbucl >= MAX_PARA) {
		un_error("EXCESO DE BUCLES PARA","");
		return(nbucl-1);
	}
	
	if (nbucles >= 20) {
		un_error("demasiados PARA anidados","");
		return(nbucl-1);
	}
	
	in = nbucl;
	bucl[in] = (BUCLE *)malloc(sizeof(BUCLE));
	bucl[in]->operacion1 = op1;
	bucl[in]->condicion = con;
	bucl[in]->operacion2 = op2;
	indice = pon_linea(TBUCL,in);
	bucl[in]->salto = indice + 1;
	bucle[nbucles] = in;
	nbucles++;nbucl++;
}
else
	if (modo == 1) {
		
		if (!nbucles) 
		{
			un_error("SIGUE sin PARA","");
			return(0);
		}
		
		nbucles--;
		in = bucle[nbucles];
		indice = bucl[in]->salto;
		bucl[in]->salto = pon_linea(TGOBU,indice) + 1;
		
	}
	
	return(in);
}

static int mas_gosub(m,nom)
int m;
char *nom;
{

if (totalgosub >= MAX_GOSUB) 
{
	un_error("EXCESO DE GOSUB-BUCLELIN","");
	return(0);
}

if (!strcmp(nom,"NULL"))
{
    pon_linea(m,-1);
    return 0;
}

gosub[totalgosub] = (GSS *)malloc(sizeof(GSS));
gosub[totalgosub]->nombre = strmalloc(strlen(nom)+1);
strcpy(gosub[totalgosub]->nombre,nom);
gosub[totalgosub]->indice = pon_linea(m,nprog + 1);
gosub[totalgosub]->proceso = nen_pr;
totalgosub++;
return 0;
}

int pon_gosub(pr,instru)
char **pr;
unsigned char instru;
{
char *p = *pr;
int final;
char nom[81];
int indice;
int aj,ai,rr;

ai = busca_fin(p,';');
aj = busca_fin(p,',');
final = ((ai <= aj) ? ai : aj);
rr = ((final == ai) ? 0 : 1);

memcpy(nom,p,final);nom[final] = '\0';
quita_blancos(nom);

if (!strlen(nom)) {
	un_error("Intento de hacer un gosub sin nombre","");
	return(rr);
}

mas_gosub(instru,nom);
*pr += final;
if (**pr == ',')
	(*pr)++;
return(rr);
}

int pon_buclelin(p,indice,instru)
char *p;
int indice;
unsigned char instru;
{
quita_blancos(p);

if (totalgosub >= MAX_GOSUB) {
	un_error("EXCESO DE GOSUB-BUCLELIN","");
	return(0);
}

gosub[totalgosub] = (GSS *)malloc(sizeof(GSS));
gosub[totalgosub]->nombre = strmalloc(strlen(p) + 1);
strcpy(gosub[totalgosub]->nombre,p);
pon_linea(instru,indice);
gosub[totalgosub]->indice = -(pon_linea(255,0) + 1);
gosub[totalgosub]->proceso = nen_pr;
totalgosub++;
return(0);
}

int pon_etiq(pr)
char **pr;
{
char *p = *pr;
int final,i;
char nom[81];
int indice;
int aj,ai,rr;

ai = busca_fin(p,';');
aj = busca_fin(p,',');
final = ((ai <= aj) ? ai : aj);
rr = ((final == ai) ? 0 : 1);

memcpy(nom,p,final);nom[final] = '\0';
quita_blancos(nom);

if (!strlen(nom)) {
	un_error("Intento de hacer una etiqueta sin nombre","");
	return(rr);
}

for (i = 0;i < totaletiqs;i++) {
	if (strcmp(etiqs[i]->nombre,nom) == 0) {
		un_error("nombre de etiqueta DUPLICADO",nom);
		return(rr);
	}
}

if (totaletiqs >= MAX_ETIQ) {
	un_error("EXCESO DE ETIQUETAS","");
	return(rr);
}

etiqs[totaletiqs] = (EEE *)malloc(sizeof(EEE));
etiqs[totaletiqs]->nombre = strmalloc(strlen(nom)+1);
strcpy(etiqs[totaletiqs]->nombre,nom);
etiqs[totaletiqs]->indice = nprog;
totaletiqs++;

*pr += final;
if (**pr == ',')
	(*pr)++;
return(rr);
}

int pon_goto(pr,instru)
char **pr;
unsigned char instru;
{
char *p = *pr;
int final;
char nom[81];
int indice;
int aj,ai,rr;

ai = busca_fin(p,';');
aj = busca_fin(p,',');
final = ((ai <= aj) ? ai : aj);
rr = ((final == ai) ? 0 : 1);


memcpy(nom,p,final);nom[final] = '\0';
quita_blancos(nom);

if (!strlen(nom)) {
	un_error("Intento de hacer un goto sin nombre","");
	return(rr);
}

if (totalgotos >= MAX_GOTO) {
	un_error("EXCESO DE GOTOS","");
	return(rr);
}

gotos[totalgotos] = (GGG *)malloc(sizeof(GGG));
gotos[totalgotos]->nombre = strmalloc(strlen(nom)+1);
strcpy(gotos[totalgotos]->nombre,nom);
gotos[totalgotos]->indice = nprog;
pon_linea(instru,nprog+1); /* por defecto salta inofensivamente */
totalgotos++;
*pr += final;
if (**pr == ',')
	(*pr)++;
return(rr);
}

int com_varia(pr)
char **pr;
{
int i,j,k;
char *p = *pr;
int modo = 3;
int final;
int fin,comillas,comillas2;
char nombre[256];
char valor[256];
int arrai;

if (!memoplin) {
	pon_varsistem();
}
for (i = 0;*(p + i);i++) {
	
	if (*(p + i) == ';') {p += (i + 1);i = 0;}
	
	if (strlen(p + i) > 3 && memcmp(p + i,"|FIN",4) == 0)
	{modo = prev_modo;i += 4;break;}
	
	fin = busca_fina(p + i,';');
	final = busca_fina(p + i,'=');
	if (final > fin) final = fin;
	memcpy(nombre,p+i,final);nombre[final] = '\0';
	quita_blancos(nombre);
	
	if (strlen(nombre) > 3 && memcmp(nombre,"|FIN",4) == 0)
	{modo = prev_modo;i += fin;break;}
	
	
	if (memcmp(nombre,"||",2) == 0) {
		i += busca_fina(p+i,'\376');
		break;
	}
	
	strcpy(valor,"0");
	arrai = 0;
	if (nombre[0] == '{') {
		j = busca_fina(nombre,'}');
		if (nombre[j] == '}') {
			nombre[j] = '\0';
			arrai = atoi(nombre+1);
			strcpy(nombre,nombre+j+1);
		}
	}
	if (fin != final) {
		if ((comillas = busca_fina(p+i+final+1,'\"')) >= (fin - final - i)) {
			if ((comillas = busca_fina(p+i+final+1,'\'')) < (fin - final - i)) {
				/* Falta asunto memorias Alfa-Buffer (m_longi) */
				
				valor[0] = 'A';valor[1] = '\0';
				if ((comillas2 = busca_fina(p+i+final+2+comillas,'\'')) < (fin - final - i)) {
					memcpy(valor+1,p+i+final+2+comillas,comillas2);
					valor[comillas2 + 1] = '\0';
				}
				
			}
			else {
				memcpy(valor,p+i+final+1,fin - final -1);
				valor[fin - final - 1] = '\0';
				quita_blancos(valor);
				if (valor[0] != '@')
					sprintf(valor,"%lf",valsd(p+i+final,1,fin - final -1));
			}
		}
		else {
			valor[0] = 'A';valor[1] = '\0';
			if ((comillas2 = busca_fina(p+i+final+2+comillas,'\"')) < (fin - final - i)) {
				memcpy(valor+1,p+i+final+2+comillas,comillas2);
				valor[comillas2 + 1] = '\0';
			}
		}
	}
	
	for (k = 0;i < strlen(nombre);k++) {
		if (nombre[k] == '+' || nombre[k] == '-' || nombre[k] == '*' ||
			nombre[k] == '/' || nombre[k] == '~' || nombre[k] == '?' ||
			nombre[k] == '%' || nombre[k] == '<' || nombre[k] == '>' ||
			nombre[k] == '!' || nombre[k] == '$') {
			k = strlen(nombre);
			break;
		}
		if (nombre[k] < '0' || nombre[k] > '9')
			break;
	}
	if (nombre[0])
    {
	    if (k == strlen(nombre) || nombre[0] == 'A' || nombre[0] == '"' ||
		    nombre[0] == '.' || (strlen(nombre) && nombre[strlen(nombre)-1] == '.')) {
		    un_error("nombre de variable ILEGAL",nombre);
	    }
	    else
		    pon_varia(nombre,valor,arrai);
    }
	
	p += (i+fin);i = 0;
	if (!*(p+i)) break;
}

*pr = (p + i);
return(modo);
}

/************************************************************************/

static suma_pilame()
{
/* si se necesitan mas temporales de las previstas ERROR */
if ( (++pilame) == (memoplin + maxpilame)) {
	un_error("Operacion demasiado compleja!!","");
	calerrno = 1;return(-1);
}
return(0);
}

static unsigned char   _operas[]= {OADIR, /* 166 de mas prioridad a menos */
OENTRA, /* 167 */
ORESET, /* 168 */
OMULTIPLICAR, /* 169 */
OMULTIPLICA,  /* 170 */
ODIVIDER,     /* 171 */
ODIVIDE, /* 172 */
ORESTO,  /* 173 */
OPORCENT,/* 174 */
OMENOSPORCENT,/* 175 */
OMASPORCENT,/* 176 */
OREDONDEO,/* 177 */
ORESTA,/* 178 */
ORESTAR,/* 179 */
OSUMA,/* 180 */
OSUMAR,/* 181 */
/* falta la operacion '&' de ascii o chr$ con DEFINE */
'&', /* 182 */
'\0'
};
/* ATENCION ESTA ELIMINADO ... SOLO ESTA A MODO DE INFORMACION
static unsigned char operas[] = {166,
167,
168,
169,
170,
171,
172,
173,
174,
175,
176,
177,
178,
179,
180,
181,
182,
0
};
*/

static int susti_oper(form,i,c,mem,hasaltado)
unsigned char *form;
int i;
unsigned char c;
int mem,hasaltado;
{
/* nucleo principal de decodificacion de las operaciones */
unsigned char o1[256];
unsigned char o2[256];
unsigned char or[5];
int p1,p2,ca,salta;
int p,menos = 0;

/* memoria resultado */
or[0] = (unsigned char)(255);
or[1] = (unsigned char)(mem + 200);
or[2] = '\0';

salta = 0;
/* obtener primero operando */
for (p1 = i - 1;p1 > -1;p1--) {
	if (*(form+p1) == '"' && hasaltado)
		if (salta)
			salta = 0;
		else
			salta = 1;
		if (!salta) {
			if ((ca = *(form+p1)) > 165 && ca < 183)
				break;
		}
}
p1++;
if (p1 != i) {  /* hay operando */
	memcpy(o1,form+p1,i - p1);
	o1[i - p1] = '\0';
	if (c == '&')
		un_error("No se que hacer con",o1);
}
else
	if (c == '&')
		o1[0] = '\0';
	else
		strcpy(o1,"0"); /* sino se usa la operacion con 0 de 1er operando */
	
	
	if (*(form+i+1) == 178) { /* caso de que el segundo operando este negado */
		/* solo para numericas (se sabe por la pila usada) */
		if (maxpilame == maxpilamen) {
			menos = 1;
			*(form+i+1) = ' ';
		}
		else
			*(form+i+1) = '-';
		
	}
	
	salta = 0;
	/* obtener segundo operando */
	for (p2 = i + 1;*(form +p2);p2++) {
		if (*(form+p2) == '"' && (salta || !p2 || *(form+p2-1) != 'A'))
			if (salta)
				salta = 0;
			else
				salta = 1;
			if (!salta) {
				if ((ca = *(form+p2)) > 165 && ca < 183)
					break;
			}
	}
	p2--;
	if (p2 != i) {  /* hay operando */
		memcpy(o2,form+i+1+menos,p2 - i - menos);
	}
	o2[p2 - i - menos] = '\0';
	
	if (maxpilame == maxpilamea && o2[0] == '-') {
		for (i = 1;i < strlen(o2);i++)
			if (o2[i] < '0' || o2[i] > '9') {
				i = 999;
				break;
			}
			if (i == 999) {
				un_error("operando ilegal",o2);
			}
	}
	
	if (o2[0] == '\0' && c != OADIR)  /*  ERROR en operaciones ej. *+  */
	{
		un_error("Falta Operando",form);
		calerrno = 7;return(0);
	}
	
	if (menos) {
		pon_opera("0",o2,ORESTA,or); /* se hace la negacion primero */
		strcpy(o2,or);
	}
	
	pon_opera(o1,o2,c,or);
	
	/* sustitucion en el string madre de la operacion por la variable or */
	if ((p2 - p1 + 1) < 2) /* incoherecia */
	{
		un_error("Mala Operacion",form);
		calerrno = 7;return(0);
	}
	
	
	*(form+p1) = or[0];
	*(form+p1+1) = or[1];
	strcpy(form+p1+2,form+p2+1);
	/* nuevo i */
	return(p1+1);
}

static int deco_oper(form,resul)
unsigned char *form;
unsigned char *resul;
{
	int o;
	int i,f,salta,hasaltado,j;
	unsigned char c;
	
	/* sustitucion de las operaciones de mas prioridad */
	
	salta = 0;hasaltado = 0;
	for(i = 0;*(form+i);i++) {
		if (*(form+i) == '"' && (salta || !i || *(form+i-1) != 'A'))
			if (salta)
				salta = 0;
			else {
				salta = 1;
				hasaltado = 1;
			}
			if (!salta) {
				
				if ( ((o = *(form+i)) > 165 && o < 169) || o == 182) {
					pare_modif = 0; /* solo vale en la ultima operacion */
					if (!(i = susti_oper(form,i, _operas[o - 166] ,pilame,hasaltado)) )
						return(-1);
					else
						if (suma_pilame())
							return(-1);
						hasaltado = 0;
				}
			}
	}
	
	/* sustitucion de las operaciones prioritarias */
	salta = 0;hasaltado = 0;
	for(i = 0;*(form+i);i++) {
		if (*(form+i) == '"' && (salta || !i || *(form+i-1) != 'A'))
			if (salta)
				salta = 0;
			else {
				salta = 1;
				hasaltado = 1;
			}
			if (!salta) {
				if ((o = *(form+i)) > 168 && o < 178) {
					pare_modif = 0; /* solo vale en la ultima operacion */
					if (!(i = susti_oper(form,i, _operas[o - 166] ,pilame,hasaltado)) )
						return(-1);
					else
						if (suma_pilame())
							return(-1);
						hasaltado = 0;
				}
			}
	}
	
	
	/* sustitucion de las secundarias */
	o = 0;
	salta = 0;hasaltado = 0;
	for(i = 0;*(form+i);i++) 
    {
		if (*(form+i) == '"' && (salta || !i || *(form+i-1) != 'A'))
			if (salta)
				salta = 0;
			else 
            {
				salta = 1;
				hasaltado = 1;
			}
	    if (!salta) 
        {
				if ((c = *(form+i)) > 177 && c < 182) { /* corregido bug es 166 en vez de 167 */
					pare_modif = 0; /* solo vale en la ultima operacion */
					if (!(i = susti_oper(form,i,_operas[(int)c - 166],pilame,hasaltado)) ) /* var acarreo resultado */
						return(-1);
					else
						o = 1;
					hasaltado = 0;
		}
	}
	}
	if (o && suma_pilame())
		return(-1);
	
	
	/* si form empieza por '\377'(decimal) ha habido sustitucion en algun sitio */
	if ((int)*form == 255) {
		/* optimizar resultado */
		if (oper[noper -1]->ficheror == -2 &&
			(j = oper[noper -1]->campor) == ( ((int)*(form+1) - 200)) + 5000 )
		{
			devuelve_campo(resul,&f,&i);
			/* si el resultado coincide con la pila usada se optimiza sino
			no pues interesa mas conservar el modo de la operacion */
			if (i >= 0) {
				if (f > -1)
					o = vg_mpfi(f)->f_campos[i]->c_tipo;
				else
					o = que_memo_tipo(i);
			}
			else
				o = i;
			
			j = que_memo_tipo(j); /* sera alfa o numerica segun de que pila sea */
			
			if (o < 0 && (int)resul[0] == 255)
				un_error("Error Interno","1"); /* !!!! error ???? */
			
			if (o < 0 && !pare_modif) /* salvo que se especifique por parentesis */
				j = 1;                /* en punteros prevalece el tipo del destino*/
			else
				if (o >= 0 && o != CHARTIPO && o != FECHATIPO && j == DOUBLETIPO)
					j = 1;
				else
					if (o >= 0 && (o == CHARTIPO || o == FECHATIPO) && j == CHARTIPO)
						j = 1;
					else
						j = 0;
					
					if (j) {
						oper[noper -1]->ficheror = f;
						oper[noper -1]->campor   = i;
					}
					else
						pon_opera(form,"",OIGUAL,resul);
					if ((int)resul[0] == 255) /* solo en caso de parentesis!! */
						oper[noper -1]->ficheror = -2;
		}
		else {
			/* !!!! error ???? */
			un_error("Error Interno","0");
			return(-1);
		}
	}
	else {/* sino es simple asignacion */
		pon_opera(form,"",OIGUAL,resul);
	}
	
	return(0);
}

static int deco_parent(form,modo)
unsigned char *form;
int modo;
{
	int i = 0,pp = -1,pa = -1,pc = -1,ap = 0,pn = 0;
	int me,modif,salta = 0;
	int nivelya = 0;
	unsigned char or[3];
	
	while(*(form+i)) {
		if (salta) {
			if (*(form+i) == '"')
				salta = 0;
			i++;
			continue;
		}
		if (*(form+i) == '"' && (salta || !i || *(form+i-1) != 'A')) {
			salta = 1;
			i++;
			continue;
		}
		if (*(form+i) == '(') {
			ap++;pp = i;
			if ((*(form+pp+1) == 'A' || *(form+pp+1) == 'N') && *(form+pp+2) =='\\') {
				modif = 2;
				pare_modif = 1;
				if (*(form+pp+1) == 'A')
					modo = 1;
				else
					modo = 0;
			}
			else {
				modif = 0;
				pare_modif = 0; /* solo vale en la ultima operacion */
			}
		}
		if (*(form+i) == ')') {
			if (!ap) {
				un_error("Falta ( Parentesis",form);
				calerrno = 8;return(-i-2);
			} /* error */
			if (pn < ap) {pn = ap;pa = pp;pc = i;}
			ap--;
			if (!ap)
				break;/* se hacen consecutivamente los de nivel 0 o al llegar al nivel 0*/
			/* esto evita la interferecia de los modificadores (A\ o (N\ . */
		}
		i++;
	}
	if (ap) {
		un_error("Falta ) Parentesis",form);
		calerrno = 8;return(-pp-2);
	} /* error */
	if (!pn) return(-1);     /* no hay parentesis */
	
	*(form+pa) = '\0';
	*(form+pc)  = '\0';
	
	if (modo) {
		pilame = pilamea;
		maxpilame = maxpilamea;
	}
	else {
		pilame = pilamen;
		maxpilame = maxpilamen;
	}
	
	me = pilame;
	if (suma_pilame())
		return(1);
	
	or[0] = (unsigned char)(255);
	or[1] = (unsigned char)(me + 200);
	or[2] = '\0';
	
	i = pare_modif;
	if (deco_oper(form+pa+1+modif,or)) return(1); /* error de calculo */
	pare_modif = i;
	
	if (modo)
		pilamea = pilame;
	else
		pilamen = pilame;
	
	*(form+pa) = (unsigned char)(255);
	*(form+pa+1) = (unsigned char)(me+200); /* sustituir por memoria temporal */
	strcpy(form+pa+2,form+pc+1);
	
	return(0);
}

static int deco_formula(form,resul)
char *form;
char *resul;
{
	int op;
	int i,j,b,modo;
	
	
	pare_modif = 0;
	devuelve_campo(resul,&op,&i);
	
	if (op > -2 && op < nfi) {
		if (i >= 0) {
			if (op == -1)  {
				i = que_memo_tipo(i);
			}
			else {
				i = vg_mpfi(op)->f_campos[i]->c_tipo;
			}
		}
		else
			i = -1;
	}
	
	if (!c_fto) {
		if (op > -2 && op < nfi) {
			if (i < 0)
				fprintf(c_error,"TRAZA Operacion arrai [%s=%s] linea %d \n\r",resul,form,c_linea);
			else
				if (i == CHARTIPO)
					fprintf(c_error,"TRAZA Operacion alfan [%s=%s] linea %d \n\r",resul,form,c_linea);
				else
					if (i == FECHATIPO)
						fprintf(c_error,"Operacion fecha [%s=%s] linea %d \n\r",resul,form,c_linea);
					
		}
	}
	
	/* set  pila variables temporales       */
	
	maxpilamen = maxpila;     /* pila numerica */
	pilamen = memoplin;
	maxpilamea = maxpila * 2; /* pila alfanumerica */
	pilamea = memoplin + maxpila;
	
	/* establecer de que indole es el resultado para sel. pila */
	if (i != CHARTIPO && i != FECHATIPO)
		modo = 0;
	else
		modo = 1;
	
	op = noper; /* Operacion con la que se empieza */
	
	if (strlen(form)) {
		
		/****************************************/
		quita_blancosc(form);
		while((i = deco_parent(form,modo)) == 0);                /* parentesis */
		
		if (modo)
		{
			pilame = pilamea; /* pila alfanumerica */
			maxpilame = maxpilamea;
		}
		else 
		{
			pilame = pilamen;  /* pila numerica */
			maxpilame = maxpilamen;
		}
		
		if (i != -1 || deco_oper(form,resul) || op == noper) { /* resto      */
			/*error*/
			/* eliminar operaciones hechas */
			for (i = op;i < noper;i++)
				free((char *)oper[i]);
			noper = op;
			return(-1);
		}
		
		/* no hace falta ya ajustar pila */
		
		/* en la formula no hay ningun error */
	}
	else {
		quita_blancosc(form+1);
		if (strlen(form+1)) {
			un_error("el RESET es una operacion singular",form+1);
		}
		pon_opera("","",ORESET,resul);
	}
	/* optimizacion : buscar una formula igual */
	
	
	for (b = -1,j = 0;j < op;j++) {
		if (oper[j]->fichero1  == oper[op]->fichero1  &&
			oper[j]->campo1    == oper[op]->campo1    &&
			oper[j]->fichero2  == oper[op]->fichero2  &&
			oper[j]->campo2    == oper[op]->campo2    &&
			oper[j]->ficheror  == oper[op]->ficheror  &&
			oper[j]->campor    == oper[op]->campor    &&
			oper[j]->operacion == oper[op]->operacion)
		{
			b = j;
			for (i = op + 1,j++;i < noper;i++,j++) {
				if (oper[i]->fichero1  != oper[j]->fichero1  ||
					oper[i]->campo1    != oper[j]->campo1    ||
					oper[i]->fichero2  != oper[j]->fichero2  ||
					oper[i]->campo2    != oper[j]->campo2    ||
					oper[i]->ficheror  != oper[j]->ficheror  ||
					oper[i]->campor    != oper[j]->campor    ||
					oper[i]->operacion != oper[j]->operacion)
				{
					b = -1;
					break;
				}
			}
			if (b > -1) break;
		}
    }
	
	if (b > -1) { /* encontrado ! */
		for (i = op;i < noper;i++) /* lo hecho se puede borrar */
			free((char *)oper[i]);
		noper = op;
		op = b;
	}
	
	return(op);
}


/************************************************************************/

int com_opera(pr,j)
char **pr;
int *j;
{
	int i = *j;
	unsigned char *p = *pr + i + 1;
	unsigned char nomr[81],formu[256],uc = 0;
	int n,h,salta = 0;
	
	
	memcpy(nomr,*pr,i);
	nomr[i] = '\0';
	quita_blancos(nomr);
	
	/* primero componer la formula con las operaciones reales */
	for (h = 0,n = 0;(*(p + h) != '\0' && (*(p + h) != ';' || salta));h++,n++) {
		if (salta) {
			if (*(p + h) == '"') {
				salta = 0;
				uc = 0;
			}
			formu[n] = *(p + h);
			continue;
		}
		if (memcmp(p+h,"<-",2) == 0) {
			formu[n] = 166;
			h++;
            uc = 0;
		}
		else {
			
			switch (*(p + h)) {
			case '+':
				if (*(p + h+1) == '.') {
					formu[n] = 181; /* los codigos de los define no son usables !!*/
					h++;
				}
				else
					formu[n] = 180;
                uc = 0;
				break;
			case '-':
				if (uc == 'A') 
                {
					formu[n] = *(p + h);
                    uc = 0;
					break;
				}
				if (*(p + h+1) == '.') 
                {
					formu[n] = 179;
					h++;
				}
				else
					formu[n] = 178;
                uc = 0;
				break;
			case '*':
				if (*(p + h+1) == '.') {
					formu[n] = 169;
					h++;
				}
				else
					formu[n] = 170;
                uc = 0;
				break;
			case '/':
				if (*(p + h+1) == '.') {
					formu[n] = 171;
					h++;
				}
				else
					formu[n] = 172;
                uc = 0;
				break;
			case '%':
				formu[n] = 174;
                uc = 0;
				break;
			case '!':
				formu[n] = 177;
                uc = 0;
				break;
			case '<':
				formu[n] = 175;
                uc = 0;
				break;
			case '>':
				formu[n] = 176;
                uc = 0;
				break;
			case '$':
				formu[n] = 173;
				break;
			case '~': /* caso RESET */
				formu[n] = ' '; /* 168 */
				formu[0] = '\0';
                uc = 0;
				break;
			case '?':
				formu[n] = 167;
                uc = 0;
				break;
			case '&':
				formu[n] = 182;
                uc = 0;
				break;
			case '"':
				if (uc != 'A') {
					salta = 1;
				}
				formu[n] = *(p + h);
                uc = 0;
				break;
			default:
				if ((int)*(p + h) > 165) {
					un_error("Caracter ilegal",p+h);
					n--;
				}
				else
					formu[n] = *(p + h);
                if (formu[n] =='(' || formu[n] == ')')
                    uc = 0;
				break;
			}
		}
		if (formu[n] != ' ' && !uc)
        {
			uc = formu[n];
        }
        else
            uc = 1;
	}
	formu[n] = '\0';
	h = busca_fin(p,';');
	*pr += (i+h+1);
	*j = 0;
	
	return(deco_formula(formu,nomr));
}

int com_condicion(pr,j)
char **pr;
int *j;
{
	int i = *j;
	char c = *((*pr) + i);
	char *p = (*pr) + i + 1;
	char nom1[81];
	char nom2[81];
	int final,tipo;
	char con;
	int f1 = F_INVALIDO,c1 = -1,f2 = F_INVALIDO,c2 = -1,indice;
	static int ulcon = -1;
	
	memcpy(nom1,*pr,i);
	nom1[i] = '\0';
	final = busca_fin(p,';');
	if (final > 80) {
		error_sintaxis(*pr);
		final = 80;
	}
	memcpy(nom2,p,final);
	nom2[final] = '\0';
	quita_blancosc(nom1);
	quita_blancosc(nom2);
	switch (c) {
	case '!':
		con = CDIFER;
		break;
	case '>':
		con = CMAYOR;
		break;
	case '<':
		con = CMENOR;
		break;
	case ']':
		con = CMAIGU;
		break;
	case '[':
		con = CMEIGU;
		break;
	case '=':
		con = CIGUAL;
		break;
	default:
		con = 0;
		break;
	}
	
	if (!con) {
		un_error("mala condicion",*pr);
	}
	
	if (strlen(nom1) > 0 && strcmp(nom1,"NULL") != 0)
		devuelve_campo(nom1,&f1,&c1);
	else {
		un_error("mala condicion",*pr);
	}
	if (strlen(nom2) > 0 && strcmp(nom2,"NULL") != 0)
		devuelve_campo(nom2,&f2,&c2);
	else {
		un_error("mala condicion",*pr);
	}
	
	if (f1 > -2 && f1 < nfi && c1 >= 0) {
		if (f1 == -1) {
			tipo = que_memo_tipo(c1);
		}
		else {
			tipo = vg_mpfi(f1)->f_campos[c1]->c_tipo;
		}
		if (tipo == CHARTIPO) {
			if (f2 > -2  && f2 < nfi && c2 >= 0) {
				if (f2 == -1) {
					tipo = que_memo_tipo(c2);
				}
				else {
					tipo = vg_mpfi(f2)->f_campos[c2]->c_tipo;
				}
				if (tipo == DOUBLETIPO || tipo == INTTIPO || tipo == 999) {
					tipo = f1;f1 = f2;f2 = tipo;
					tipo = c1;c1 = c2;c2 = tipo;
					if (con == CMAYOR)
						con = CMENOR;
					else
						if (con == CMENOR)
							con = CMAYOR;
						else
							if (con == CMEIGU)
								con = CMAIGU;
							else
								if (con == CMAIGU)
									con = CMEIGU;
				}
			}
		}
	}
	
	indice = pon_condicion(f1,c1,f2,c2,con,&ulcon);
	
	p += final;
	*j = (int)(p - *pr);
	return(indice);
}

int trata_para(pr,modo)
char **pr;
int modo;
{
	char *p = *pr;
	int final,fin;
	static int indice1 = -1;
	static int indice2 = -1;
	static int indice3 = -1;
	
	if (modo == 2 )
		final = busca_con(p);
	else
		final = busca_fin(p,'=');
	
	fin = busca_fin(p,';');
	
	if (final > 80) {
		error_sintaxis(*pr);
		final = fin;
	}
	
	switch (modo) {
	case 1:
		if (final >= fin || final == 0)
			indice1 = -1;
		else
			indice1 = com_opera(&p,&final);
		indice2 = -1;indice3 = -1;
		break;
	case 2:
		if (final >= fin || final == 0)
			indice2 = -1;
		else
			indice2 = com_condicion(&p,&final);
		break;
	case 3:
		if (final >= fin || final == 0);
		else
			indice3 = com_opera(&p,&final);
		pon_bucle(indice1,indice2,indice3,0);
		break;
	default:
		break;
	}
	*pr += fin;
	return(0);
}

int trata_si(pr)
char **pr;
{
	char *p = *pr;
	int final,fin,indice;
	
	final = busca_con(p);
	fin = busca_fin(p,';');
	
	if (final > 80) {
		error_sintaxis(*pr);
		final = fin;
	}
	
	if (final >= fin) return(-1);
	
	if (Ylogico != 0 || Ologico != 0) {
		com_condicion(&p,&final);
		*pr += fin;
		return(0);
	}
	indice = com_condicion(&p,&final);
	pon_cond(indice,0);
	*pr += fin;
	return(0);
}

int trata_pinta(pr,instru)
char **pr;
unsigned char instru;
{
	char *p = *pr;
	int final,fin,indice;
	int f1 = F_INVALIDO,c1 = -1,f2 = F_INVALIDO,c2 = -1;
	char nombre[81];
	
	final = busca_fin(p,',');
	fin = busca_fin(p,';');
	
	if (final >= fin) {
		final = fin;
	}
	else {
		if (final > 80) {
			error_sintaxis(*pr);
		}
		else {
			memcpy(nombre,p,final);
			nombre[final] = '\0';
			quita_blancos(nombre);
			devuelve_campo(nombre,&f2,&c2);
		}
		p += final+1;
		final = fin - final - 1;
	}
	if (final > 80) {
		error_sintaxis(*pr);
	}
	else {
		memcpy(nombre,p,final);
		nombre[final] = '\0';
		quita_blancosc(nombre);
		devuelve_campo(nombre,&f1,&c1);
	}
	
	if (f1 != F_INVALIDO) {
		if (f1 >= 0) {
			if (f2 == F_INVALIDO)
			{
				pon_linea(instru,f1);
				pon_linea(255,c1);
			}
			else {
				indice = pon_oper(f1,c1,f2,c2,f1,c1,OPINTAC);
				pon_linea(TOPER,indice);
			}
		}
		else {
			if (f2 == F_INVALIDO) {
				indice = pon_oper(f2,c2,f1,c1,f2,c2,OPINTAC);
				pon_linea(TOPER,indice);
			}
			else {
				indice = pon_oper(f1,c1,f2,c2,f1,c1,OPINTA);
				pon_linea(TOPER,indice);
			}
		}
	}
	*pr += fin;
	return(0);
}

int mal_argu(nom)
char *nom;
{
	return(un_error("mal argumento",nom));
}


static int coge_parametro(pr,nom)
char **pr;
char *nom;
{
	char *p = *pr;
	int i = busca_fin(p,';');
	int c = busca_fin(p,',');
	int j = ((i <= c) ? i : c);
	
	memcpy(nom,p,j);nom[j] = '\0';
	quita_blancosc(nom);
	p += j;
	if (*p == ',') p++;
	*pr = p;
	return((j == i) ? 0 : 1);
}

static int check_tipo(f,c,m)
int f,c,m;
{
	int t,e = 0;
	if (f < -1) return(1);
	
	if (c < 0) {
		t = 1000; /* puntero */
	}
	else
		if (f == -1) {
			t = que_memo_tipo(c);
			if (t == 999) t = DOUBLETIPO;
		}
		else {
			t = vg_mpfi(f)->f_campos[c]->c_tipo;
		}
		
		switch(m) {
		case 0:
			if (t != CHARTIPO)
				e = 1;
			break;
		case 1:
			if (t != CHARTIPO && t != FECHATIPO)
				e = 1;
			break;
		case 2:
			if (t != FECHATIPO)
				e = 1;
			break;
		case 3:
			if (t != DOUBLETIPO && t != INTTIPO && t != FLOATTIPO && t != LONGTIPO)
				e = 1;
			break;
		case 4:
			break;
		case 5:
			if (t != 1000)
				e = 1;
			break;
		default:
			e = 1;
			break;
		}
		return(e);
}



static std_parametro(nom,n,instru)
char *nom;
int n;
unsigned char instru;
{
	int j,r,  h = 0,
		f = F_INVALIDO,
		c = 0,
		e = 0,
		desdobla = 0,
		desdoble = 0;
	
	if (n == 16) {
		h = atoi(nom);
	}
	else
		if (n == 17) {
			h = (int)(*nom);
		}
		else {
			devuelve_campo(nom,&f,&c);
			if (n > 17 && n < 23) { /* Caso especial PATH_DAT etc #fnom_var */
				if (f < 0 || c >= 0)
					e = 1;
				else {
					c = -c;c--;
					if (!(e = check_tipo(-1,c,n-18)))
					{
						desdobla = 1;
						desdoble = c;
						h = f;
						/* h = (unsigned int)(((unsigned int)c * 50 ) + f); */
					}
				}
			}
			else {
#ifdef COMENTARIO
				if (f > -1 && c < 0) { /* De momento esto no es posible !! */
					e = 1;
				}
				else
#endif
					if (n >= 0 && n < 5) { /* caso variables - campos */
						if (f > -1 && c < 0)
							e = 1;
						else
							if (!(e = check_tipo(f,c,n))) {
								if (f != -1)
									h = (unsigned int)(((unsigned int)c * 100 ) + f);
								else
									h = -(c+1);
							}
					}
					else
						if (n > 4 && n < 10) { /* caso variables */
							if (f != -1)
								e = 1;
							else
								if (!(e = check_tipo(-1,c,n-5)))
									h = c;
						}
						else
							if (n > 9 && n < 15) { /* caso campos */
								if (f < 0)
									e = 1;
								else
									if (!(e = check_tipo(f,c,n-10)))
									{
										if (c < 0)
										{
											h = c;
											desdobla = 1;
											desdoble = f;
										}
										else
											h = (unsigned int)(((unsigned int)c * 100 ) + f);
									}
							}
							else
								if (n == 15) { /* caso exclusivo de punteros */
									if (f != -1 || c >= 0)
										e = 1;
									else {
										if (!(e = check_tipo(-1,c,5))) /* esto realmente es redundante */
											h = (-c)-1;
									}
								}
								else {
                                    un_error("Tabla de instrucciones incorrecta",nom);
								}
			}
		}
		
		if (e) {
			mal_argu(nom);
		}
		else
		{
			pon_linea(instru,h);
			if (desdobla)
			{
				pon_linea(255,desdoble);
			}
		}
}

int com_orden(pr,i)
char **pr;
int i;
{
	char *p = *pr + i + 1;
	char *t;
	int j,r,s,n,k,aj;
	unsigned char instru;
	char nom[256];
	int factor;
	int repeticion;
	int repetido;
	
	for(aj = 0;std_inst[aj].nombre;aj++) 
	{
		t = std_inst[aj].nombre;
		s = strlen(t);
		factor = 2;
		repeticion = 0;
		repetido = 0;
		if (!memcmp(p,t,s) && (*(p+s) == ' ' /*|| *(p+s) == 0 punto y coma OBLIGATORIO!! */ || *(p+s) == ';'))
		{
			p += s;
			/* analisis de los parametros */
			t = std_inst[aj].parametros;
			k = 0;
			for(i = 0;t[i] && t[i+1];i += factor) 
			{
				factor = 2;	    
				if (t[i] == 'Z') 
				{
					repeticion = t[i+1] - 48;
					repetido = 0;
					continue;
				}
				if (!k && i) 
				{
					if (!repeticion || (repeticion && repetido))
						un_error("Faltan parametros ",p);  
					break;
				}
				repetido++;
				if (repetido == repeticion) 
				{
					repetido = 0;
					factor = -(repeticion*2);
				}
				
				instru = ((!i) ? std_inst[aj].in : 255);
				if (instru == 254)
				{
					pon_linea(std_inst[aj].in,std_inst[aj].extra_in);
					instru = 255;
				}
				n = t[i+1]-48;
				if (n > 9) n -= 7;
				switch(t[i]) {
				case '0': /* bucles para */
					switch(n) {
					case 0:  /* PARA */
						if (modob) {
							un_error("falta algo en en un para antes de",p);
						}
						modob = 1;
						trata_para(&p,modob);
						break;
					case 1:  /* HACIENDO */
						if (modob == 2) {
							modob = 3;
							trata_para(&p,modob);
							modob = 0;
						}
						else
							un_error("falta la condicion en el para ",p);
						break;
					case 2:  /* SIGUE */
						if (modob) {
							un_error("falta algo en en un para antes de",p);
						}
						pon_bucle(-1,-1,-1,1);
						break;
					case 3:  /* SAL */
						if (!pon_bucle(-1,-1,-1,2))
							un_error("SAL ilegal",p-1);
						else
							pon_linea(TSAL,0);
						break;
					}
					break;
					case '1': /* condiciones */
						switch(n) {
						case 0:
							if (modob == 1) {
								modob = 2;
								trata_para(&p,modob);
							}
							else {
								trata_si(&p);
							}
							break;
						case 1:
							Ylogico = 1;
							trata_si(&p);			  
							Ylogico = 0;
							break;
						case 2:
							Ologico = 1;
							trata_si(&p);
							Ologico = 0;
							break;
						case 3:
							pon_cond(-1,1);
							break;
						case 4:
							pon_cond(-1,2);
							break;
						}
						break;
						case '2': /* pon_ccf */
							k = pon_ccf(&p,n,instru);
							break;
						case '3': /* pon_dcf */
							k = pon_dcf(&p,n); /* este caso va fijo con operaciones */
							break;
						case '4': /* pon_acf */
							k = pon_acf(&p,n,instru);
							break;
						case '5': /* pon_bcf */
							k = pon_bcf(&p,n,instru);
							break;
						case '6': /* ET-VETE */
							switch(n) {
							case 0:
								k = pon_etiq(&p); /* no tiene instruccion posible */
								break;
							case 1:
								k = pon_goto(&p,instru);
								break;
							}
							break;
							case '7': /* GOSUB-BUCLELIN-ENTLINEAL */
								switch(n) {
								case 0:
									k = pon_gosub(&p,instru);
									break;
								case 1:
									k = coge_parametro(&p,nom);
									r = (nom[0] - '0') * 10000;
									j = busca_fin(nom,'#');
									if (nom[j] == '#') {
										nom[j] = '\0';
										if (es_numero(nom+j+1,999)) {
											j = atoi(nom+j+1);
											cambia_fichero(&j);
										}
										else {
											j = ind_def_rel_f(nom+j+1);
											if (j < 0) {
												j = 0;
												un_error("fichero inexistente",nom+j+1);
											}
										}
										r += j;
										pon_buclelin(nom+1,r,instru);
									}
									else {
										mal_argu(nom);
									}
									break;
								case 2:
									k = coge_parametro(&p,nom);
									mas_gosub(instru,nom);
									break;
								}
								break;
								case '9': /* standar */
									k = coge_parametro(&p,nom);
									std_parametro(nom,n,instru);
									break;
								case 'A': /* pinta */
									k = trata_pinta(&p,instru); /* usa operaciones salvo en un caso */
									break;
								case 'B': /* defbucle dentro de proceso */
									prev_modo = gen_modo;
									gen_modo = 5; /* modo DEFBUCLE !! */
									nombre_defbucle(p);
									k = 0;
									p += busca_fin(p,';');
									break;
								case 'C': /* declaracion de fichero anidada */
									prev_modo = gen_modo;
									gen_modo = 1;
									k = 0;
									p += busca_fin(p,';');
									break;
								case 'D': /* declaracion de alias-campos anidada */
									prev_modo = gen_modo;
									gen_modo = 2;
									k = 0;
									p += busca_fin(p,';');
									break;
								case 'E': /* declaracion de variables anidada */
									prev_modo = gen_modo;
									gen_modo = 3;
									k = 0;
									p += busca_fin(p,';');
									break;
								case 'F': /* instruccion bucle */
									k = coge_parametro(&p,nom);
									j = busca_defbucle(nom);
									k = 0;
									if (j >= 0)
										pon_linea(instru,j);
									else
										un_error("DEFBUCLE NO ENCONTRADO (DEBE ESTAR DECLARADO ANTES)",nom);
									p += busca_fin(p,';');
									break;
								default:									
                                    un_error("Tabla de instrucciones incorrecta",p);									
									break;
		  }
	 }
	 if (!t[0]) {
		 pon_linea(std_inst[aj].in,0);
	 }
	 else
		 if (k) {
			 un_error("Sobran parametros ",p);
		 }
		 break;
      }
   }
   if (!std_inst[aj].nombre) {
	   error_sintaxis(p);
	   p += busca_fin(p,';');
   }
   *pr = p;
   return(gen_modo);
}


static int busca_error(pr,i)
char **pr;
int i;
{
	char *p = *pr + i;
	int j,k;
	/* char buf[128]; */
	
	/* strcpy(buf,p); */
	/* quita_blancos(buf); */
	j = busca_fin(p,';');
	k = busca_fin(p,'=');
	
	if (j <= k) {
		error_sintaxis(*pr);
		p += j;
		*pr = p;
		return(0);
	}
	
	return(i);
}


static reset_prog()
{
	int i,j;
	
	for (i = 0;i < nllamada_interna;i++)
		free(llamada_interna[i]);
	nllamada_interna = 0;
	
	for (j=0;j < totalfichs;j++) {
		free(fichs[j]->fichero);
		free((char *)fichs[j]);
	}
	totalfichs = 0;
	
	for (i=0;i<np_ex;i++) {
		free(p_ex[i]->nombre);
		free((char *)p_ex[i]);
	}
	np_ex = 0;
	for (i=0;i<nm_ex;i++) {
		free(m_ex[i]->nombre);
		free((char *)m_ex[i]);
	}
	nm_ex = 0;
	
	for (i=0;i<ncond;i++)
		free((char *)cond[i]);
	for (i=0;i<nbucl;i++)
		free((char *)bucl[i]);
	for (i=0;i<noper;i++)
		free((char *)oper[i]);
	for (i=0;i<nmemo;i++) {
		if (que_memo_tipo(i) == CHARTIPO || que_memo_tipo(i) == FECHATIPO)
			free(memo[i]->mv.valc);
		free((char *)memo[i]);
	}
	
	for (i=0;i<nf_B;i++) {
		free((char *)f_B[i]->ord1);
		free((char *)f_B[i]->calc);
		if (f_B[i]->ncomp > 0)
			free((char *)f_B[i]->comp);
		if (f_B[i]->nord2 > 0)
			free((char *)f_B[i]->ord2);
		free((char *)f_B[i]);
	}
	
	for (i = 0;i < nindice_def_local;i++)
	{
		indice_def_local[i] = -1;
	}
	nindice_def_local = 0;
	
	nprog = 0;
	ncond = 0;
	nbucl = 0;
	noper = 0;
	nmemo = 0;
	nf_B = 0;
	/*reset_tabla();*/
	return(0);
}


static int fin_compilacion()
{
	int j;
	
	for (j=0;j < totalgosub;j++) {
		free(gosub[j]->nombre);
		free((char *)gosub[j]);
	}
	totalgosub = 0;
	
	for (j=0;j < totalmemos;j++) {
		free(memos[j]->nombre);
		free((char *)memos[j]);
	}
	totalmemos = 0;
	for (j=0;j < totalcamps;j++) {
		free(camps[j]->nombre);
		free((char *)camps[j]);
	}
	totalcamps = 0;
	modob = 0;
	ncondici = 0;
	b_punto = 0;
	return(0);
}



int com_calculo(pr,modo)
char **pr;
int modo;
{
	char *p = *pr;
	int i,j,m;
	int fin,indice;
	
	for (i = 0;*(p + i);i++) {
		if (*(p + i) == ';') {p += (i+1);i = 0;}
		
		j = 0;
		switch(modo) {
		case 4:
			j = memcmp(p + i,"|FCAL",5);
			break;
		case 6:
			j = memcmp(p + i,"|FRUT",5);
			break;
		case 7:
			j = memcmp(p + i,"|FPRO",5);
			break;
		case 8:
			j = memcmp(p + i,"|FPRC",5);
			break;
		default:
			break;
		}
		if (!j) {
			
			modo = 0;i += 5;
			
			pon_linea(TEND,0);
			if ((j = pon_cond(0,3))) {
				un_error("hay SI dejados a su suerte","");/* j */
				pon_cond(0,4);
			}
			break;
		}
		
		switch (*(p+i)) {
		case '=' :
			if (i > 80) {
				error_sintaxis(*pr);
				while (*(p + i) != '\0') i++; /* !!! */
			}
			else
				if ((indice = com_opera(&p,&i)) >= 0)
					pon_linea(TOPER,indice);
				break;
		case '|' :
			if (*(p+i+1) == '|') {
				while (*(p + i) != '\0') i++; /* comentarios */
			}
			else {
				com_orden(&p,i);
				i = 0;
			}
			break;
		default :
			if (*(p + i) && *(p + i) != ' ' && *(p + i) != ';' && *(p + i) != '\011')
				i = busca_error(&p,i);
			break;
		}
		if (modo != gen_modo) { /* momentaneo paso a anidacion de declaracion */
			if (prev_modo != modo) {
				gen_modo = modo;
				rw_error(61);
			}
			else {
				modo = gen_modo;
				if (*(p + i) == ';') {p += (i+1);i = 0;}
				break;
			}
		}
		if (!*(p+i)) break;   
		if (*(p + i) == ';') {p += (i+1);i = -1;}
	}
	*pr = (p + i);
	return(modo);
}



int compila_prog(p)
char *p;
{
	char texto[514];
	unsigned char b_buf[514];
	int kk;
	int file;
	int i;
	char *pr,*pp,*pp2;
	int en_comentario = 0;
	
    compilando_un_prc++;
	_nivel++;
	
	if (_nivel == 1) 
    {
		ini_tracer(p);
	}
	
	strcpy(texto,p);
	strcat(texto,".cal");

	if (_nivel == 1) 
    {		
		/*v_prints("Compilando %s",texto);*/
    }
    

	if ((file = rw_open_(texto,O_RDWR | O_BINARY)) == -1) 
    {
			_nivel--;
            compilando_un_prc--;
			return(-1);
    }
	
	
    sprintf(c_nombrecal,"%s",texto);

	if (_nivel == 1) 
    {				
		gen_modo = 0;
		prev_modo = 0;
		c_linea = 0;
        strcpy(texto,"Fichero@");
        pon_fichero(texto,9999,1,NULL);
	}

	
	while (rw_read_(file,b_buf,512) == 512) {
		decripto(b_buf);
		kk = 0;
		while(kk < 510 && b_buf[kk] != 4)
		{
			if (strlen(b_buf+kk) > 250)
				texto[0] = 0;
			else
				strcpy(texto,b_buf+kk);
			kk += (strlen(texto) + 1);
			
			if (error_break) {
				rw_close_(file);
				if (_nivel == 1) {
					enlaza_gotos(); /* gotos trans-calculo */
					fin_compilacion();
				}
				_nivel--;
                compilando_un_prc--;
				return(-1); /* abandonar */
			}
			
			c_linea++;
			pr = texto;

            if (en_comentario)
            {
                pp = strstr(texto,"*/");
                if (pp)
                {
                    en_comentario = 0;
                    strcpy(texto,pp+2);
                    if (!texto[0]) continue;
                }
                else
                    continue;
            }
            
            pp = strstr(texto,"/*");
            if (pp)
            {
                *pp = 0;
                pp2 = strstr(pp+1,"*/");
                if (pp2)
                   strcpy(pp,pp2+2);
                else
                   en_comentario = 1;
            }           
			
			
			if (gen_modo == 1 || gen_modo == 2) {
				i = busca_fina(pr,'"');
				if (*(pr + i) == '"') {
					un_error("Comillas ILEGALES","");
					continue;
				}
			}
			do {
				switch(gen_modo) {
				case 1:
					gen_modo = com_ficheros(&pr);
					break;
				case 2:
					gen_modo = com_campos(&pr);
					break;
				case 3:
					gen_modo = com_varia(&pr);
					break;
				case 4:
				case 6:
				case 7:
				case 8:
					gen_modo = com_calculo(&pr,gen_modo);
					break;
				case 5:
					gen_modo = com_bucle(&pr);
					break;
				case 0:
					gen_modo = busca_comando(&pr); /* esta funcion contiene el incluye */
					break;
				default:
					un_error("Error interno gen_modo 001",pr);					
					break;
				}
			} while(*pr);
			
		}
	}
	
	
	switch(gen_modo) {
	case 1:
		un_error("falta FIN para FICHEROS","");
		break;
	case 2:
		un_error("falta FIN para CAMPOS","");
		break;
	case 3:
		un_error("falta FIN para VARIABLES","");
		break;
	case 5:
		un_error("falta FIN para DEFBUCLE","");
		break;
	case 4:
		un_error("falta FCAL para CALCULO","");
		break;
	case 6:
		un_error("falta FRUT para RUTINA","");
		break;
	case 7:
		un_error("falta FPRO para PROGRAMA","");
		break;
	case 8:
		un_error("falta FPRC para PROCESO","");
		break;
	default:
		break;
	}
	
	rw_close_(file);
	
	if (_nivel == 1) {
		enlaza_gosubs();
		
        /*
		sprintf(texto,    "Cal:%3d  C_Ex:%2d  Pro:%4d Con:%3d Paras:%3d Oper:%4d",
			totalprogr,np_ex,nprog,ncond,nbucl,noper);
		sprintf(texto+100,"Var:%4d V_Ex:%3d Cam:%3d  Fic:%2d  Gosub:%3d Goto:%3d  Etiq:%3d",
			nmemo,nm_ex,totalcamps,totalfichs,totalgosub,totalgotos,totaletiqs);
		*/

        /*
		if (!oculto) {
			Ar();
			pprint(1802,texto);
			pprint(1902,texto+100);
			Cr();
			fflush(stdout);
		}
		
        
		if (c_error != stdout) {
			fprintf(c_error,"%s\n\r",texto);
			fprintf(c_error,"%s\n\r",texto+100);
		}
        */
		
		enlaza_gotos(); /* gotos trans-calculo */
		
		graba_tracer();
		
		fin_compilacion();
	}
	
	_nivel--;
    compilando_un_prc--;
	return(0);
}


int enlaza_gotos()
{
	int i,j;
	
	for (i=0;i < totalgotos;i++) {
		for (j=0;j < totaletiqs;j++) {
			if (strcmp(gotos[i]->nombre,etiqs[j]->nombre) == 0) {
				prog[(gotos[i]->indice)].indice = etiqs[j]->indice;
				break;
			}
		}
		if (j == totaletiqs) {
			un_error("etiqueta no encontrada",gotos[i]->nombre);
		}
	}
	for (j=0;j < totaletiqs;j++) {
		free(etiqs[j]->nombre);
		free((char *)etiqs[j]);
	}
	for (j=0;j < totalgotos;j++) {
		free(gotos[j]->nombre);
		free((char *)gotos[j]);
	}
	totaletiqs = 0;totalgotos = 0;
	return(0);
}

int enlaza_gosubs()
{
	int i,indice,k;
	
	for (i = 0;i < totalgosub;i++) {
		if (gosub[i]->indice < 0 && !strcmp(gosub[i]->nombre,"NULL"))
			continue;
		if ((indice = busca_prog(gosub[i]->nombre)) >= 0) 
        {
			if (gosub[i]->indice < 0) 
            { /* este caso no usa tabla_cal */
				k = (-(gosub[i]->indice)) - 1;
				/* modificado para mas de 99 procesos y uso de rutinas */
				if (progr[indice-1]->tipo == -1) {
					if (progr[indice-1]->proceso == gosub[i]->proceso)
					{
						prog[k].indice = -(k + 1);
					}
					else
					{
						un_error("LAS RUTINAS DEBEN ESTAR EN EL MISMO CAL","");
					}
				}
				else 
                {
					prog[k].indice = numero_interno_prog(indice);
				}
			}
			else { /* este caso es directo si es necesario !! */
				if (progr[indice-1]->tipo == -1)
				{
					if (progr[indice-1]->proceso == gosub[i]->proceso)
						indice = -(progr[indice-1]->indice + 1);
					else
					{
						un_error("LAS RUTINAS DEBEN ESTAR EN EL MISMO CAL","");
					}
				}
				else
					indice = numero_interno_prog(indice);
				prog[gosub[i]->indice].indice = indice;
			}
		}
		else {
			un_error("gosub no encontrado",gosub[i]->nombre);
		}
	}
	return(0);
}


/*****************/

static int abortar = 0;


int aborta_compila()
{
return(abortar = 1);
}

int compilar(int argc,char *argv[])
{
int i,j;
int bien = 1;
char tmp[256];
char dbin[256];
char fichero[256];
char mibase[256];
char _notacion_dir_aplicacion[256];
char *p;
char **lista;

abortar = 0;

e_nivel = 0;

c_nombrecal[0] = 0;
_notacion_dir_aplicacion[0] = 0;
modo_gen_antiguo = 0;
j = 0;
mibase[0] = 0;
for (i = 1;i < argc;i++)
{
   if (argv[i][0] == '-')
   {
      if (!strcmp(argv[i]+1,"5"))
	     {
         modo_gen_antiguo = 1;
		 }
	  else
      if (!strcmp(argv[i]+1,"6"))
	     {
         modo_gen_antiguo = 0;
		 }
	  else
      if (argv[i][1] == "D")
	     {
         strcpy(mibase,argv[i]+2);
		 }
	  else
      if (argv[i][1] == "M")
	     {
         strcpy(_notacion_dir_aplicacion,argv[i]+2);
		 }
      else
	     {
		 bien = 0;
		 break;
		 }
   }
   else
   {
       if (quita_path(argv[i]) == argv[i])
       {
           j = 1;
       }
   }
}

if (!bien) 
   {
   pprint(101,"dscomp Version 8\n");
   pprint(101,"dscomp [-5 o -6 -Ddirbase -Mmodificadorpath] procesos\n");
   exit(2);
   }

cambia_barra(mibase);
cambia_barra(notacion_dir_aplicacion);

if (mibase[0])
{
    if (*(quita_path(mibase)))
    {
        strcat(mibase,"/");
    }
}

local_term[0]  = 0xffff;
local_term[50] = 0;
local_term[51] = 0xffff;

mpfi_maximo = (FI_DEF *)malloc(sizeof(FI_DEF));
memset(mpfi_maximo,0,sizeof(FI_DEF));
mpfi_maximo->f_npanta = MAXPANTA;
mpfi_maximo->f_nclaves = MAXCLAVES;
mpfi_maximo->f_ncampos = MAXCAM;
mpfi_maximo->f_nrelaci = MAXRELACI;
for (i  = 0;i < MAXCAM;i++)
   {
   mpfi_maximo->f_campos[i] = (CAMPO_DEF  *)malloc(sizeof(CAMPO_DEF));
   memset(mpfi_maximo->f_campos[i],0,sizeof(CAMPO_DEF));
   }


fichs = (FF **)malloc(sizeof(char *) * 150);
memos = (MM **)malloc(sizeof(char *) * MAX_MEMOS);
camps = (CC **)malloc(sizeof(char *) * MAX_ALIAS);
progr = (PP **)malloc(sizeof(char *) * 1500);
bucles = (BBB *)malloc(sizeof(BBB) * MAX_PARA);
etiqs = (EEE **)malloc(sizeof(char *) * MAX_ETIQ);
gotos = (GGG **)malloc(sizeof(char *) * MAX_GOTO);
gosub = (GSS **)malloc(sizeof(char *) * MAX_GOSUB);
llamada_interna = (char **)malloc(sizeof(char *) * 300);
nllamada_interna = 0;
en_pr = (struct enlaza_prc *)malloc(sizeof(struct enlaza_prc) * MAX_PROCES);
prog = (PROGRAM *)malloc(sizeof(PROGRAM) * MAX_PROG);
memo = (T_MEMORIA **)malloc(sizeof(T_MEMORIA *) * MAX_MEMOS);
cond = (CONDIC **)malloc(sizeof(CONDIC *) * MAX_COND);
bucl = (BUCLE **)malloc(sizeof(BUCLE *) * MAX_PARA);
oper = (OPERA **)malloc(sizeof(OPERA *) * MAX_OPER);
f_B = (struct _filebucle **)malloc(sizeof(struct _filebucle *) * MAX_BUCLE);
global_ex = (M_EX **)malloc(sizeof(M_EX *) * MAX_MEX);
m_ex = (M_EX **)malloc(sizeof(M_EX *) * MAX_MEX);
p_ex = (P_EX **)malloc(sizeof(P_EX *) * MAX_PEX);


ya_esta_avisado = 0;
numero_generacion = time(NULL);



for (j = 1;j < argc && !abortar;j++)
{
    if (argv[j][0] == '-') continue;    


    if (mibase[0])
        {           
        strcpy(dirbase,mibase);
        strcpy(dirbase_,dirbase);
        i = strlen(dirbase_) - 1;
        if (dirbase_[i] == '/')
           {
           dirbase_[i] = 0;
           }
        else
           {
           strcat(dirbase,"/");
           }
        strcpy(dirbass,quita_path(dirbase_));
        *(quita_path(dirbase_)) = 0;
        }

    strcpy(notacion_dir_aplicacion,_notacion_dir_aplicacion);


    if (quita_path(argv[j]) == argv[j])
    {
        
        if (!mibase[0])
        {
#ifdef FGVDOS
            tmp[0] = 0;
            GetCurrentDirectory(sizeof(tmp)-1,tmp);
            strlwr(tmp);
            cambia_barra(tmp);
            if (*(quita_path(tmp)))
            {
                strcat(tmp,"/");
            }
            p = strstr(tmp,"cal/");
            if (p)
            {
                *p = 0;
                strcpy(dirbase,tmp);                
                strcpy(dirbase,tmp);
                strcat(tmp,"cal/");
                strcat(tmp,argv[j]);
            }
            else
                continue;

#endif
        }
        else
        {
            strcpy(tmp,dirbase);
            strcat(tmp,"cal/");
            strcat(tmp,argv[j]);
        }
    }
    else
    {        
        if (argv[j][0] == ':' && argv[j][1] != '/')
        {
            strcpy(tmp,dirbase_);
            strcat(tmp,argv[j]+1);
            strcpy(quita_path(tmp),"cal/");
            strcat(tmp,quita_path(argv[j]));
            strcpy(notacion_dir_aplicacion,argv[j]);
        }
        else
        {
            strcpy(tmp,argv[j]);
            cambia_barra(tmp);
            p = strstr(tmp,"cal/");
            if (p)
            {
                *p = 0;
                strcpy(dirbase,tmp);
                *p = 'c';
            }            
        }
    }

    cambia_barra(tmp);

    strcpy(dirbase_,dirbase);
    i = strlen(dirbase_) - 1;
    if (dirbase_[i] == '/')
       {
       dirbase_[i] = 0;
       }
    else
       {
       strcat(dirbase,"/");
       }
    strcpy(dirbass,quita_path(dirbase_));
    *(quita_path(dirbase_)) = 0;



    if (strstr(quita_path(tmp),"*"))
    {
        lista = lee_direct(tmp,0);
    }
    else
    {
        lista = (char **)malloc(sizeof(char *) * 2);
        *lista = malloc(strlen(quita_path(tmp))+1);
        strcpy(*lista,quita_path(tmp));
        *(lista+1) = NULL;
    }

    *quita_path(tmp) = 0;

    for (j = 0;lista && lista[j] && !abortar;j++)
    {

        strcpy(fichero,tmp);
        strcat(fichero,lista[j]);

        ini_nombres();
        nfi = 0;
        nprog = 0;
        ncond = 0;
        nbucl = 0;
        noper = 0;
        nmemo = 0;
        nf_B = 0;
        memoplin = 0;


        p = strstr(fichero,".cal");
        if (p) *p = 0;

	    if (!memoplin) 
	    {
		    pon_varsistem();
	    }

        c_nombrecal[0] = 0;
	    compila_prog(fichero);
        c_nombrecal[0] = 0;
	    for (i = 0;i < nmemo;i++)
	    { 
		    if (que_memo_tipo(i) == 999)
			    memo[i]->m_tipo = DOUBLETIPO;
	    }

        sprintf(dbin,"%sbin/%s",dirbase,quita_path(fichero));

        graba_prog(dbin);

        fin_nombres();
        reset_prog();
        fin_aplic();
        fin_prog();
    } 

    if (lista)
    {
        libera_dir(lista);
    }

}

abortar = 0;

if (prog)
  free((char *)prog);
if (memo)
  free((char *)memo);
if (cond)
  free((char *)cond);
if (bucl)
  free((char *)bucl);
if (oper)
  free((char *)oper);
if (f_B)
  free((char *)f_B);
if (m_ex)
  free((char *)m_ex);
if (p_ex)
  free((char *)p_ex);
if (global_ex)
  free((char *)global_ex);
if (llamada_interna)
  free((char *)llamada_interna);
if (en_pr)
  free((char *)en_pr);

if (fichs)
  free((char *)fichs);
if (memos)
  free((char *)memos);
if (camps)
  free((char *)camps);
if (progr)
  free((char *)progr);
if (bucles)
  free((char *)bucles);
if (etiqs)
   free((char *)etiqs);
if (gotos)
   free((char *)gotos);
if (gosub)
   free((char *)gosub);

for (i  = 0;i < MAXCAM;i++)
   {
   free(mpfi_maximo->f_campos[i]);
   }
free(mpfi_maximo);

return(0);
}


