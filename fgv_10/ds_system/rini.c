
/* Autor: Francisco Gonzalez Valiente */

#include <fgvsys.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef O_BINARY
#include <io.h>
#endif

#include <errno.h>
#include <malloc.h>
#include <rwmalloc.h>

#include <rglobal.h>


/* extern int errno; */
char *logger_salida = NULL;
char *logger_salida__ = NULL;
long loggersize = 0;

int bbdd_tiempo_rotate = 0;

char *bbdd_file_path = NULL;

int no_clave_vacia = 0;

int tempo_logger = 0;

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
#include <stdio.h>
#include <string.h>
#include <errno.h>
#ifdef ESDOS
#include <dos.h>
#endif

#include <rtermin.h>

#include <teclas.h>

#include <stdlib.h>
/*extern char *getenv();*/


#ifdef RWXARXA
#ifdef RWSERVER
extern int en_conexion_externa;
extern int en_conexion_externa_stop;
#endif
extern int en_conexion_terminal;
#endif

/*
extern int IXXMAXBUF = 10;
int ixx_absoluto = 1;
*/

int sinescape  = 0;
int gridextras = 1;

int manteblanco = 0;
int avancelinea = 0;

int modosinbarras = -1;

int port_fijo_de_server = 0;

int excel_alt = 0;


AGIREGISTRO *AgiRegistro = NULL;
int UpdateRegistro = 0;

extern int IXXMAXBUF = 0;
int ixx_absoluto = 0;

int pass_cambio_autorizado = 0;

long _maximo_dsk_ = 2000000000;

int laszetassonfinal = 0;
extern int dsdb_usa_crypto;

int noboton_usuarios = 0;

unsigned long other3_server = 0;

char *log_trafico_tcp = NULL;

char *dsdb_serverip = NULL;

char *tablamacdir = NULL;
char *tablauserdir = NULL;
char *tablauserfich = NULL;
char *tablauserapli = NULL;

extern char *act_key = NULL;
extern char *act_red = NULL;
extern char *act_instalacion[52] = {NULL,NULL,NULL,NULL,NULL,NULL,};
extern char *codigo_de_sistema1 = NULL;
extern char *codigo_de_sistema2 = NULL;
extern char *codigo_de_sistema3 = NULL;
extern char *codigo_de_sistema4 = NULL;
extern char *act_servidor = NULL;
extern char *help_externo = NULL;
char *key_generador = NULL;
char *key_codigocapullo = NULL;

int usar_grid = 0;
int tuned_blck = 0;

char *definicion_tipo_datos = NULL;

int modo_respaldo = -1; /* modo dbf de respaldo */
char *dir_respaldo = NULL;
char *lista_respaldo = NULL;

char *telefonos_claves = NULL;

double DsEuroConvierte = (double)166.386;
int DsDeciModoOtro = 0;
int DsDeciModoNormal = 2;
char DsEuroTexto[512] = "Cambio Moneda";

int minimizado_10 = 0;

int _tipopermiso = 1;

int fgv_crear_plantilla_doc = 0;

char *w_plantilla_principal = NULL;

char *PreEjecucion = NULL;


char *filtro_consulta_remota = NULL;

char *hay_password_asesino = NULL;
int maximo_timeouts = 9999999;

int TeclaScanner = 0;
int ScannerNoBorra = 0;

int simple_tarea_bruta = 0;

char *PortScanner = NULL;

int entorno_antivirus = 0;

int isslowconection = 0;
int consulta_vieja = 0;

int es_modo_test = 0;


int fichdeusuario = 0;


char *ixx_ciudiu = NULL;
char *lista_diu  = NULL;

int default_dbf_id = 0;
int ixx_datos_dbf = 0;
int ixx_control_diu = 0;

extern struct tit_tecla {
   char *r;
   char *t;
   char *d;
} _titu_tecla[_NUMTECLA-NUMESPEC];

static char *salva_titu[_NUMTECLA-NUMESPEC];
extern struct tit_tecla *titu_tecla;

int rwagi_infoteclado = 0;

int que_version_ciu = 2;

int con_btrieve = 0;
int siempre_anula = 0;
extern int time_d_tiempo1;
extern int time_d_tiempo2;
extern int rentra_autosal;
extern int wmenu_en_alfa;
extern int wdesplegamenu;
extern int orig_wdesplegamenu;
extern int nombre_de_listado;
extern int duplicarusuario;
extern int resta_dias_log;


char *lista_incluidos_logger = NULL;

int resta_dias_logger = 0;

int proporcion_especial_bmp = 0;

/*int auto_empresa = 1;*/
int auto_empresa = 0;

int defecto_modo_datos = 0;

int recoge_todo_caracter = 0;

int ModoProfundo = 1;

char *sqlsrv_con_s = NULL;
char *sqlsrv_con_sql = NULL;
char *sql_externo_filtro = NULL;

char *escapa_sql = NULL;

char *log_debug = NULL;

char *rw_dsdb_filtro = NULL;

int rw_usa_dsdb = 0; /* fichero "unico" para datos */
int rw_dsdb_tambloque = 8;
int rw_dsdb_bloquesdefecto = 0;
int rw_dsdb_bloquesaumento = 0;

int ini_minmegaslibres = 300;
int modo_minmegaslibres = 0;


char *p_erp_tab = NULL;


int xautocentrado = 0;
int xtabreturn = 0;

int auto_completar = 0;

int siempre_password = 0;

int instalaservidor = 0;

int con_respaldo_externo = 0;

char *desc_funs[20] = {NULL,};
static int posi_funs[20] = {0,};

char *url_ayuda_f1 = NULL;

char *haylog_mensajes_cs = NULL;

char *log_enviafichero = NULL;

char *dsproxy = NULL;

int pausa_transferencia = 0;

int extra_de_relleno_enviafichero = 0;

int _modo_portal = 0;

int defectoesc = 0;

extern char rw_user[11];

char *ds_mysql = NULL;
char *ds_mysql_exec = NULL;
char *ds_mysql_pass = NULL;

#ifdef RWSERVER
extern int en_conexion_externa;
#endif

static struct parametros_iniciales
{
   char *nombre;
   void **dato;
   int tipo; /* 0 = char * inicializado a NULL 1 = int 2 = long 3 = double  */
   int linea;
} agi_init[] =
{
   {"Activacion",&act_key,0,0},
   {"A_Red",&act_red,0,0},
   {"Sistema1",&codigo_de_sistema1,0,0},
   {"Sistema2",&codigo_de_sistema2,0,0},
   {"Sistema3",&codigo_de_sistema3,0,0},
   {"Sistema4",&codigo_de_sistema4,0,0},
   {"InsPaq01",&(act_instalacion[0]),0,0},
   {"InsPaq02",&(act_instalacion[1]),0,0},
   {"InsPaq03",&(act_instalacion[2]),0,0},
   {"InsPaq04",&(act_instalacion[3]),0,0},
   {"InsPaq05",&(act_instalacion[4]),0,0},
   {"InsPaq06",&(act_instalacion[5]),0,0},
   {"InsPaq07",&(act_instalacion[6]),0,0},
   {"InsPaq08",&(act_instalacion[7]),0,0},
   {"InsPaq09",&(act_instalacion[8]),0,0},
   {"InsPaq10",&(act_instalacion[9]),0,0},
   {"InsPaq11",&(act_instalacion[10]),0,0},
   {"InsPaq12",&(act_instalacion[11]),0,0},
   {"InsPaq13",&(act_instalacion[12]),0,0},
   {"InsPaq14",&(act_instalacion[13]),0,0},
   {"InsPaq15",&(act_instalacion[14]),0,0},
   {"InsPaq16",&(act_instalacion[15]),0,0},
   {"InsPaq17",&(act_instalacion[16]),0,0},
   {"InsPaq18",&(act_instalacion[17]),0,0},
   {"InsPaq19",&(act_instalacion[18]),0,0},
   {"InsPaq20",&(act_instalacion[19]),0,0},
   {"InsPaq21",&(act_instalacion[20]),0,0},
   {"InsPaq22",&(act_instalacion[21]),0,0},
   {"InsPaq23",&(act_instalacion[22]),0,0},
   {"InsPaq24",&(act_instalacion[23]),0,0},
   {"InsPaq25",&(act_instalacion[24]),0,0},
   {"InsPaq26",&(act_instalacion[25]),0,0},
   {"InsPaq27",&(act_instalacion[26]),0,0},
   {"InsPaq28",&(act_instalacion[27]),0,0},
   {"InsPaq29",&(act_instalacion[28]),0,0},
   {"InsPaq30",&(act_instalacion[29]),0,0},
   {"InsPaq31",&(act_instalacion[30]),0,0},
   {"InsPaq32",&(act_instalacion[31]),0,0},
   {"InsPaq33",&(act_instalacion[32]),0,0},
   {"InsPaq34",&(act_instalacion[33]),0,0},
   {"InsPaq35",&(act_instalacion[34]),0,0},
   {"InsPaq36",&(act_instalacion[35]),0,0},
   {"InsPaq37",&(act_instalacion[36]),0,0},
   {"InsPaq38",&(act_instalacion[37]),0,0},
   {"InsPaq39",&(act_instalacion[38]),0,0},
   {"InsPaq40",&(act_instalacion[39]),0,0},
   {"InsPaq41",&(act_instalacion[40]),0,0},
   {"InsPaq42",&(act_instalacion[41]),0,0},
   {"InsPaq43",&(act_instalacion[42]),0,0},
   {"InsPaq44",&(act_instalacion[43]),0,0},
   {"InsPaq45",&(act_instalacion[44]),0,0},
   {"InsPaq46",&(act_instalacion[45]),0,0},
   {"InsPaq47",&(act_instalacion[46]),0,0},
   {"InsPaq48",&(act_instalacion[47]),0,0},
   {"InsPaq49",&(act_instalacion[48]),0,0},
   {"InsPaq50",&(act_instalacion[49]),0,0},
   {"PaqDesde",&(act_instalacion[50]),0,0},
   {"PaqHasta",&(act_instalacion[51]),0,0},
   {"MenuAlfanumerico",&wmenu_en_alfa,1,0},
   {"AutoReturn",&rentra_autosal,1,0},
   {"DesplegaMenus",&wdesplegamenu,1,0},
   {"NombreDeListado",&nombre_de_listado,1,0},
   {"DuplicarUsuario",&duplicarusuario,1,0},
   {"TiempoUsuario",&time_d_tiempo1,1,0},
   {"TiempoAplicacion",&time_d_tiempo2,1,0},
   {"PFuncion01",&(_titu_tecla[PRIMERAF].d),0,0},
   {"PFuncion02",&(_titu_tecla[PRIMERAF+1].d),0,0},
   {"PFuncion03",&(_titu_tecla[PRIMERAF+2].d),0,0},
   {"PFuncion04",&(_titu_tecla[PRIMERAF+3].d),0,0},
   {"PFuncion05",&(_titu_tecla[PRIMERAF+4].d),0,0},
   {"PFuncion06",&(_titu_tecla[PRIMERAF+5].d),0,0},
   {"PFuncion07",&(_titu_tecla[PRIMERAF+6].d),0,0},
   {"PFuncion08",&(_titu_tecla[PRIMERAF+7].d),0,0},
   {"PFuncion09",&(_titu_tecla[PRIMERAF+8].d),0,0},
   {"PFuncion10",&(_titu_tecla[PRIMERAF+9].d),0,0},
   {"PFuncion11",&(_titu_tecla[PRIMERAF+10].d),0,0},
   {"PFuncion12",&(_titu_tecla[PRIMERAF+11].d),0,0},
   {"PFuncion13",&(_titu_tecla[PRIMERAF+12].d),0,0},
   {"PFuncion14",&(_titu_tecla[PRIMERAF+13].d),0,0},
   {"PFuncion15",&(_titu_tecla[PRIMERAF+14].d),0,0},
   {"PFuncion16",&(_titu_tecla[PRIMERAF+15].d),0,0},
   {"PFuncion17",&(_titu_tecla[PRIMERAF+16].d),0,0},
   {"PFuncion18",&(_titu_tecla[PRIMERAF+17].d),0,0},
   {"PFuncion19",&(_titu_tecla[PRIMERAF+18].d),0,0},
   {"PFuncion20",&(_titu_tecla[PRIMERAF+19].d),0,0},
   {"AnularColores",&siempre_anula,1,0},
   /*{"RWIndexBuffers",&IXXMAXBUF,1,0},*/
   {"ConBtrieve",&con_btrieve,1,0},
   /*{"RWFastOpen",&ixx_absoluto,1,0},*/
   {"InfoTeclado",&rwagi_infoteclado,1,0},
   {"RWBorrarLog",&resta_dias_log,1,0},	 
   {"Servidor",&act_servidor,0,0},
   {"Generador",&key_generador,0,0},	  
   {"CodigoInestable",&key_codigocapullo,0,0},
   {"WinTodoCaracter",&recoge_todo_caracter,1,0},	  
   {"HttpAyuda",&help_externo,0,0},
   {"SERVICIO_DB_EXTERNO",&sqlsrv_con_s,0,0},
   {"SERVICIO_DB_MYSQL",&sqlsrv_con_s,0,0},   
   {"SistemaDeFicheros",&defecto_modo_datos,1,0},
   {"EspecificacionDatos",&definicion_tipo_datos,0,0},
   {"ModoRespaldo",&modo_respaldo,1,0},
   {"DirRespaldo",&dir_respaldo,0,0},
   {"ListaRespaldo",&lista_respaldo,0,0},
   {"BackupControl",&ixx_ciudiu,0,0},
   {"MaximoBackupControl",&_maximo_dsk_,2,0},
   {"ListaBackupControl",&lista_diu,0,0},
   {"DS_GRID",&usar_grid,1,0},
   {"LOG_DIU",&ixx_control_diu,1,0},
   {"ModoBmp",&proporcion_especial_bmp,1,0},
   {"AutoEmpresa",&auto_empresa,1,0},
   {"3DLook",&ModoProfundo,1,0},
   {"ACTIVAR_RESPALDO_EXTERNO",&con_respaldo_externo,1,0},
   {"ACTIVAR_DB_MYSQL",&con_respaldo_externo,1,0},   
   {"DsEuroConvierte",&DsEuroConvierte,3,0},
   {"DsDeciModoOtro",&DsDeciModoOtro,1,0},
   {"DsDeciModoNormal",&DsDeciModoNormal,1,0},
   {"DsEuroTexto",&DsEuroTexto,4,0},
   {"TipoPermiso",&_tipopermiso,1,0},   
   {"ConvertirADoc",&fgv_crear_plantilla_doc,1,0},
   {"PlantillaPrincipal",&w_plantilla_principal,0,0},
   {"PreEjecucion",&PreEjecucion,0,0},
   {"TablasLocales",&filtro_consulta_remota,0,0},
   {"SQLSERVER_OLEDB",&sqlsrv_con_sql,0,0},      
   {"SERVIDOR_TIMEOUTS",&maximo_timeouts,1,0},
   {"FORZAR_UNA_TAREA",&simple_tarea_bruta,1,0},
   {"KILL_KEY",&hay_password_asesino,0,0},
   {"SCANNER_COM",&PortScanner,0,0},
   {"TECLA_SCANNER",&TeclaScanner,1,0},
   {"SLOW_CONNECTION",&isslowconection,1,0},
   {"CONSULTA_SIMPLE",&consulta_vieja,1,0},
   {"ENTORNO_ANTIVIRUS",&entorno_antivirus,1,0},
   {"SCANNER_NOBORRA",&ScannerNoBorra,1,0},  
   {"USERFICH",&fichdeusuario,1,0},
   {"MODOTEST",&es_modo_test,1,0},
   {"VERSIONCIU",&que_version_ciu,1,0},
   {"FILTRO_DB_EXTERNO",&sql_externo_filtro,0,0},   
   {"FILTRO_DB_MYSQL",&sql_externo_filtro,0,0},      
   {"MODOBASEDATOS",&rw_usa_dsdb,1,0},   
   {"BASEDATOSBLOQUE",&rw_dsdb_tambloque,1,0},   
   {"BASEDATOSESPACIO",&rw_dsdb_bloquesdefecto,1,0},   
   {"BASEDATOSAUMENTO",&rw_dsdb_bloquesaumento,1,0},
   {"BASEDATOSFILTRO" ,&rw_dsdb_filtro,0,0},
   {"ERPTAB",&p_erp_tab,0,0},
   {"TABLAUSERFICH",&tablauserfich,0,0},
   {"MODO_TAB",&dsdb_usa_crypto,1,0},
   {"TABLAUSERAPLI",&tablauserapli,0,0},   
   {"TABLAUSERDIR",&tablauserdir,0,0},
   {"NOBOTONUSUARIOS",&noboton_usuarios,1,0},   
   {"NoClaveVacia",&no_clave_vacia,1,0},   
   {"LOG_TRAFICO_TCP",&log_trafico_tcp ,0,0},   
   {"CLAVESUSUARIO",&pass_cambio_autorizado,1,0},   
   {"MINIMOSMEGAS",&ini_minmegaslibres,1,0},
   {"MODOMINIMOSMEGAS",&modo_minmegaslibres,1,0},
   {"TABLAUSERFICH",&tablauserfich,0,0},
   {"MODO_TAB",&dsdb_usa_crypto,1,0},
   {"TABLAUSERAPLI",&tablauserapli,0,0},   
   {"TABLAUSERDIR",&tablauserdir,0,0},     
   {"LASZETASSONFINAL",&laszetassonfinal,1,0},   
   {"TABLAMACDIR",&tablamacdir,0,0},
   {"LOGGER",&logger_salida__,0,0},
   {"LOGGERSIZE",&loggersize,2,0},
   {"DSDBSERVER",&dsdb_serverip,0,0},
   {"SINESCAPE",&sinescape,1,0},
   {"GRIDEXTRAS",&gridextras,1,0},
   {"MODO_SIN_BARRAS",&modosinbarras,1,0},
   {"MANTEBLANCO",&manteblanco,1,0},
   {"MENSAJE_LICENCIA_1",&telefonos_claves,0,0},
   {"AVANCELINEA",&avancelinea,1,0},
   {"BBDD_FILE_ROTATE",&bbdd_tiempo_rotate,1,0},
   {"BBDD_FILE_FILTRO" ,&sql_externo_filtro,0,0},
   {"BBDD_FILE_PATH" ,&bbdd_file_path,0,0},
   {"AUTOCENTRADO" ,&xautocentrado,1,0},
   {"TABRETURN" ,&xtabreturn,1,0},   
   {"AUTOCOMPLETAR" ,&auto_completar,1,0},
   {"PASSWORD" ,&siempre_password,1,0},
   {"AUTOINSTALASERVIDOR" ,&instalaservidor,1,0},   
   {"ESCAPASQL" ,&escapa_sql,0,0},
   {"LOGDEBUG", &log_debug,0,0},
   {"DIASLOGGER" ,&resta_dias_logger,1,0},
   {"MINIMIZADO" ,&minimizado_10,1,0},
   {"URL_AYUDA_F1" ,&url_ayuda_f1,0,0},
   {"XDUMPLOGCS" ,&haylog_mensajes_cs,0,0}, 
   {"PAUSATRANSFER" ,&pausa_transferencia,1,0}, 
   {"LISTALOGGER" ,&lista_incluidos_logger,0,0},    
   {"LOGENVIAFICHERO" ,&log_enviafichero,0,0},
   {"EXTRAENVIAFICHERO" ,&extra_de_relleno_enviafichero,1,0}, 
   {"PUERTOPREFERENTE" ,&port_fijo_de_server,1,0}, 
   {"PORTAL" ,&_modo_portal,1,0},
   {"DSPROXY" ,&dsproxy,0,0},
   {"TEMPO_LOGGER" ,&tempo_logger,1,0},   
   {"DEFECTOESC" ,&defectoesc,1,0},
   {"DSMYSQL" ,&ds_mysql,0,0},
   {"DSMYSQLEXEC" ,&ds_mysql_exec,0,0},
   {"DSMYSQLPASS" ,&ds_mysql_pass,0,0},
   {"EXCEL_ALT" ,&excel_alt,1,0},
   {NULL,NULL,0,0},
   /*{"BLK_TUNE",&tuned_blck,1,0},*/
    };

	
    static int *no_lineas = NULL;
    
    extern char *rw_directorio();
	#include <stdlib.h>
    /*extern long atol();*/
    
    int gran_control = 0;

	static char **_salva_ini = NULL;	
    
    
    static void pon_dato_en_ini(j,tmp)
       int j;
    char *tmp;
    {
       char *punt;
       switch(agi_init[j].tipo)
       {
       case 0:
          punt =(char *)*agi_init[j].dato;
          if (punt)
             free(punt);
          punt = malloc(strlen(tmp)+1);
          strcpy(punt,tmp);
          *agi_init[j].dato = (void *)punt;
          break;
       case 1:
          *((int *)agi_init[j].dato) = atoi(tmp);
          break;
       case 2:
          *((long *)agi_init[j].dato) = atol(tmp);
          break;
       case 3:
          *((double *)agi_init[j].dato) = rwatof(tmp);
          break;
       default:
          break;
       }
       if (agi_init[j].linea == -1)
       {
          agi_init[j].linea = -2;
       }
    }
    
    
    static void claves_por_defecto()
			 {
       if (act_key) free(act_key);
       if (act_instalacion[50]) free(act_instalacion[50]);
       if (act_instalacion[51]) free(act_instalacion[51]);
       
       act_key = malloc(30);
       act_instalacion[50] = malloc(30);
       act_instalacion[51] = malloc(30);
#ifdef FGVDOS
       strcpy(act_key,"QhWAB5lHXmA8e0");
       strcpy(act_instalacion[50],"RzaxGnipK84B");
       strcpy(act_instalacion[51],"nDtnGnipK8gB");
#else
       strcpy(act_key,"6b5PB5lHXmAee4");
       strcpy(act_instalacion[50],"RzaxGnipK84B");
       strcpy(act_instalacion[51],"nDtnGnipK8gB");
#endif
       
			 }
    
    int inicio_fgv()
			 {
       char nom[256];
       char comenta[256];
       FILE *file;
       int i,j,e,k;
       int linea;
       char *punt;
       
       
       if (gran_control)
       {
          rw_error(1000);
       }
       gran_control = 1;
       
       for (j = 0;agi_init[j].nombre;j++)
       {
          agi_init[j].linea = -1;
       }
       
       punt = getenv("DSINI");
       if (punt && strlen(punt))
          strcpy(nom,punt);
       else
       {
          strcpy(nom,rw_directorio("ds.ini"));
       }
       
       for (i = 0;i < 20;i++) {desc_funs[i] = NULL;posi_funs[i]=0;}
       
	   

       file = fopen(nom,"rt");
       
       if (!file)
       {
          rename(rw_directorio("agi.ini"),nom);
          file = fopen(nom,"rt");
       }
       
       if (!file) 
       {
          claves_por_defecto();
          lee_agiregistro();
          orig_wdesplegamenu = wdesplegamenu;
          return(0); /* como si hubiera ini */
       }
       
       
       for (j = 0;agi_init[j].nombre;j++)
       {   
          if (strlen(agi_init[j].nombre) == 10 &&
             !memcmp(agi_init[j].nombre,"PFuncion",8))
          {
             k = agi_init[j].nombre[8] - 48;
             k *= 10;
             k += (agi_init[j].nombre[9] - 48);
             k--;
             posi_funs[k] = j+1;
          }   
       }
       
       linea = 0;
       while(fgets(nom,250,file))
       {
          linea++;
          quitan(nom);
          quitab(nom);
          quitabp(nom);
          if (!nom[0] || nom[0] == ';')
             continue;
          for (i = 0;nom[i] && nom[i] != '=' && nom[i] != ';';i++);
          if (nom[i] != '=')
          {
             printf("bad line (%d) in initialization file",linea);
             exit(2);
          }
          nom[i] = 0;
          i++;
          comenta[0] = 0;
          for (j = i;nom[j];j++)
          {
             if (nom[j] == ';')
             {
                strcpy(comenta,nom+j+1);
                nom[j] = 0;
                break;
             }
          }
          quitabp(nom);
          quitab(nom);
          quitabp(nom+i);
          quitab(nom+i);
          e = 0;
          for (j = 0;agi_init[j].nombre;j++)
          {
             if (!strcmp(agi_init[j].nombre,nom))
             {
                /* caso especial !!!! */
                for (k = 0;k < 20;k++)
                {
                   if (posi_funs[k] == (j+1)) break;
                }
                if (k < 20)
                {
                   desc_funs[k] = malloc(strlen(comenta)+1);
                   strcpy(desc_funs[k],comenta);
                }
                e = 1;
                agi_init[j].linea = linea;
                pon_dato_en_ini(j,nom+i);
                break;
             }
          }
          if (!e)
          {
             printf("Indentificador Desconocido (%s)",nom);
          }
       }
       fclose(file);
       
       orig_wdesplegamenu = wdesplegamenu;
       
       lee_agiregistro();
       
       if (!act_key)
       {
          claves_por_defecto();
       }

	   if (logger_salida__) {		   
		   strcpy(nom,logger_salida__);		   
		   strcat(nom,"_");
		   crea_dir(nom);
		   if (logger_salida)
			   free(logger_salida);
		   logger_salida = malloc(strlen(nom)+1);
		   strcpy(logger_salida,nom);
	   }
       
       return(0);
}


int fin_fgv()
{
   int j,i;
   char *punt;
   
   if (!gran_control)
   {
      /*rw_error(1001);*/
   }
   gran_control = 0;
   
   for (j = 0;agi_init[j].nombre;j++)
   {
      agi_init[j].linea = 0;
      if (!agi_init[j].tipo)
      {
         punt =(char *)*agi_init[j].dato;
         if (punt)
         {
            free(punt);
            *agi_init[j].dato = NULL;
         }
      }
   }
   for (i =0;i < 20;i++)
   {
      if (desc_funs[i])
      {
         free(desc_funs[i]);
         desc_funs[i] = NULL;
      }
   }
   
   fin_agiregistro();
   
   return(0);
}


static int resuelve_ini(nom,j,comenta)
char *nom;
int j;
char *comenta;
{
   int i;
   char *punt;
   nom[0] = 0;
   switch(agi_init[j].tipo)
   {
   case 0:
      punt =(char *)*agi_init[j].dato;
      if (punt) quitab(punt);
      if (punt && strlen(punt))
      {
         sprintf(nom,"%s=%s",agi_init[j].nombre,punt);
         for (i =0;i<20;i++)
         {
            if (posi_funs[i] == (j+1) && desc_funs[i])
            {
               comenta[0] = ';';
               strcpy(comenta+1,desc_funs[i]);
            }
         }
      }
      break;
   case 1:
      sprintf(nom,"%s=%d",agi_init[j].nombre,*((int *)agi_init[j].dato));
      break;
   case 2:
      sprintf(nom,"%s=%ld",agi_init[j].nombre,*((long *)agi_init[j].dato));
      break;
   case 3:
      sprintf(nom,"%s=%lf",agi_init[j].nombre,*((double *)agi_init[j].dato));
      break;
   default:
      break;
   }
   return(0);
}

int graba_fgv(m)
int m;
{
   char nomf[81];
   char nom[256];
   char comenta[256];
   FILE *file;
   int i,j,e,k;
   int linea;
   char *punt;
   int nlineas;
   char **ini;
   int nfuns;
   int *funs;
   
   
#ifdef FGVDOS
   if (act_key && !strcmp(act_key,"QhWAB5lHXmA8e0"))
      return(-1);
#else
   if (act_key && !strcmp(act_key,"6b5PB5lHXmAee4"))
      return(-1);
#endif
   
   
   punt = getenv("DSINI");
   if (punt && strlen(punt))
      strcpy(nomf,punt);
   else
			{
      if (m)
         strcpy(nomf,rw_directorio("ds.ini"));
      else
         return(-1);
			}
   
   for (nfuns = 0;agi_init[nfuns].nombre;nfuns++);
   
   nlineas = 0;
   linea = 0;
   file = fopen(nomf,"rt");
   if (file) 
			{ 
      while(fgets(nom,250,file))
      {
         nlineas++;
      }
      fclose(file);  
			}
   if (nlineas)
			{
      ini = (char **)malloc(sizeof(char *)*nlineas);
      file = fopen(nomf,"rt");
      if (!file) {free(ini);return(-1);}  


	  for (i = 0;i < _NUMTECLA-NUMESPEC;i++)
	  {
		  salva_titu[i] = _titu_tecla[i].d;
		 _titu_tecla[i].d = titu_tecla[i].d;		 
	  }

      while(fgets(nom,250,file))
      {
         quitan(nom);
         for (i = 0;nom[i] && nom[i] != '=' && nom[i] != ';';i++);
         if (nom[i] == '=')
         {
            strcpy(comenta,nom);
            comenta[i] = 0;
            quita_blancos(comenta);	  
            for (j = 0;agi_init[j].nombre;j++)
            {
               if (!strcmp(agi_init[j].nombre,comenta))
               {
                  if (agi_init[j].linea != (linea+1))
                  {
                     comenta[0] = ';';
                     strcpy(comenta+1,nom);
                     strcpy(nom,comenta);
                     break;
                  }
               }
            }
         } 
         ini[linea] = malloc(strlen(nom)+1);
         strcpy(ini[linea],nom);
         linea++;
      }
      fclose(file);
			}
   
   while(linea)
			{
      linea--;
      strcpy(nom,ini[linea]);
      quitan(nom);
      quitab(nom);
      if (!nom[0])
      {
         free(ini[linea]);
         nlineas--;
      }
      else
         break;
			}
   
   funs = (int *)malloc(sizeof(int) * nfuns);
   memset(funs,0,sizeof(int) * nfuns);
   
   file = fopen(nomf,"wt");
   if (!file) goto sal;
   linea = 0;
   for (i = 0;i < nlineas;i++)
			{
      linea++;
      for (j =0;agi_init[j].nombre;j++)
      {
         if (agi_init[j].linea == linea)
         {
            funs[j] = linea;
            comenta[0] = 0;
            for (k = 0;ini[i][k];k++)
            {
               if (ini[i][k] == ';')
               {
                  strcpy(comenta,ini[i]+k);
                  break;
               }
            }
            
            resuelve_ini(nom,j,comenta);
            if (nom[0])
            {
               quitan(comenta);
               quitab(comenta);
               strcat(nom,comenta);
            }
#ifdef ESDOS
            strcat(nom,"\n");
#else
            strcat(nom,"\r\n");
#endif
            fputs(nom,file);
            break;
         }
      }
      
      
      if (!agi_init[j].nombre)
      {
#ifdef ESDOS
         fprintf(file,"%s\n",ini[i]);
#else
         fprintf(file,"%s\r\n",ini[i]);
#endif
      }
      
			}
   
   for (j = 0;j < nfuns;j++)
			{
      if (!funs[j] && agi_init[j].nombre && !agi_init[j].tipo && agi_init[j].linea == -1 && *agi_init[j].dato)
      {
         comenta[0] = 0;
         resuelve_ini(nom,j,comenta);
         quitan(nom);
         quitab(nom);
         if (nom[0])
         {
            if (agi_init[j].linea == -2)
               agi_init[j].linea = -1;
            quitan(comenta);
            strcat(nom,comenta);
#ifdef ESDOS
            strcat(nom,"\n");
#else
            strcat(nom,"\r\n");
#endif
            fputs(nom,file);
         }
      }
			}
   
   for (j = 0;agi_init[j].nombre;j++)
			{
      if (agi_init[j].linea == -2)
      {
         comenta[0] = 0;
         resuelve_ini(nom,j,comenta);
         quitan(nom);
         quitab(nom);	  
         if (nom[0])
         {
            quitan(comenta);
            strcat(nom,comenta);
#ifdef ESDOS
            strcat(nom,"\n");
#else
            strcat(nom,"\r\n");
#endif
            fputs(nom,file);
         }
         agi_init[j].linea = -1;
      }
			} 
   
   fclose(file);
   
sal:
   
   if (nlineas)
			{
	  for (i = 0;i < _NUMTECLA-NUMESPEC;i++)
	  {
		 titu_tecla[i].d = _titu_tecla[i].d;
		 _titu_tecla[i].d = salva_titu[i];
	  }

      for (i =0;i < nlineas;i++)
         free(ini[i]);
      free(ini);
			}
   free(funs);
   
   return(0);
}

static int _pon_en_ini(char *item,int salva)
{
   int j,d,varnum = 0,n,nn,trobato,nuevo,posible;
   char tmp[512];
   char *punt;
   
   strcpy(tmp,item);
   d = strbusca(tmp,"=");
   if (d == -1) return(-1);
   tmp[d] = 0;
   d++;
   quita_blancos(tmp);
   j = strlen(tmp);
   if (!j) return(-1);
   j--;
   if (tmp[j] == '?')
   {
      varnum = 1;
      if (j)
      {
         j--;
         if (tmp[j] == '?')
         {
            varnum = 2;
         }
      }
      if (!memcmp(tmp,"Sistema",7) || !memcmp(tmp,"InsPaq",6))
         quita_blancos(tmp+d);
      n = strlen(tmp) - varnum;
      trobato = -1;
      posible = -1;
      nuevo = -1;
   }
   
   for (j = 0;agi_init[j].nombre;j++)
   {
      if (varnum)
      {
         nn = strlen(agi_init[j].nombre) - varnum;
         if (n == nn && !agi_init[j].tipo)
         {
            if (!memcmp(tmp,agi_init[j].nombre,n))
            {
               /* solo datos alfanumericos ... coincide ... buscar dato igual ... */
               punt =(char *)*agi_init[j].dato;
               if (punt)
               {
                  if (!strcmp(punt,tmp+d))
                  {
                     trobato = j;
                     break;
                  }
               }
               else
               {
                  if (nuevo == -1)
                     nuevo = j;
               }
               posible = j;
            }
         }
      }
      else
      {
         if (!strcmp(agi_init[j].nombre,tmp))
         {
            pon_dato_en_ini(j,tmp+d,salva);
            break;
         }
      }
   }
   if (varnum && trobato == -1)
   {
      if (nuevo == -1)
      {
         pon_dato_en_ini(posible,tmp+d,salva);
      }
      else
      {
         pon_dato_en_ini(nuevo,tmp+d,salva);
      }
   }
   return(0);
}


int dame_valor_ini(char *_clave,char *valor)
{
int j;
char *punt;
char clave[512];

    strcpy(clave,_clave);

	j = strbusca(clave,"=");
	if (j > -1)
	{
		clave[j] = 0;
		quitab(clave);
	}    

	for (j = 0;agi_init[j].nombre;j++)
	{
	   if (!strcmp(clave,agi_init[j].nombre))
	   {
		   switch(agi_init[j].tipo)
		   {
		   case 0:		   
			  *valor = 0;
			  punt =(char *)*agi_init[j].dato;
			  if (punt)
			  {
				  strcpy(valor,punt);
			  }
			  break;
		   case 1:
			  sprintf(valor,"%d",*((int *)agi_init[j].dato));
			  break;
		   case 2:
			  sprintf(valor,"%ld",*((long *)agi_init[j].dato));
			  break;
		   case 3:
			  sprintf(valor,"%lf",*((double *)agi_init[j].dato));
			  break;
		   case 4:
			  punt = (char *)agi_init[j].dato;
			  strcpy(valor,*((char **)punt));
			  break;
		   default:
			  break;
		   }
		   strcpy(clave,_clave);
		   j = strbusca(clave,"=");
		   if (j > -1)
		   {
				pon_en_ini(clave);
		   }
		   return 0;
	   }
	}
	return -1;
}

int pon_en_ini(char *item)
{
	return _pon_en_ini(item,0);
}

int sobrepone_ini(char *elini)
{
	FILE *f;
	char nom[3000];
	int i,j;
	char *punt;
	/* reponer valores originales si hay */

	if (_salva_ini)
	{
	   for (j = 0;agi_init[j].nombre;j++)
	   {
		   if (!_salva_ini[j])
			  continue;
		   switch(agi_init[j].tipo)
		   {
		   case 0:
			  punt =(char *)*agi_init[j].dato;
			  if (punt)
				 free(punt);		  
			  *agi_init[j].dato = (void *)_salva_ini[j];
			  _salva_ini[j] = NULL;
			  break;
		   case 1:
			  *((int *)agi_init[j].dato) = *((int *)_salva_ini[j]);
			  free(_salva_ini[j]);
			  _salva_ini[j] = NULL;
			  break;
		   case 2:
			  *((long *)agi_init[j].dato) = *((long *)_salva_ini[j]);
			  free(_salva_ini[j]);
			  _salva_ini[j] = NULL;
			  break;
		   case 3:
			  *((double *)agi_init[j].dato) = *((double *)_salva_ini[j]);
			  free(_salva_ini[j]);
			  _salva_ini[j] = NULL;
			  break;
		   case 4:
			  strcpy(*agi_init[j].dato,_salva_ini[j]);
			  free(_salva_ini[j]);
			  _salva_ini[j] = NULL;
			  break;
		   default:
			  break;
		   }
	   }
	}

	f = fopen(elini,"r");
	if (f)
	{
	   while(fgets(nom,2990,f))
	   {          
		  quitan(nom);
		  quitab(nom);
		  quitabp(nom);
		  if (!nom[0] || nom[0] == ';')
			 continue;
		  for (i = 0;nom[i] && nom[i] != '=' && nom[i] != ';';i++);
		  if (nom[i] != '=')
		  {
			  break;
		  }
		  i++;	
		  for (j = i;nom[j];j++)
		  {
			 if (nom[j] == '"')
			 {
				 j++;
				 while(nom[j] && nom[j] != '"') j++;
				 if (!nom[j]) break;
			 }
			 if (nom[j] == ';')
			 {                
				nom[j] = 0;
				break;
			 }
		  }
		  quitab(nom+i);
		  quitabp(nom+i);		  
		  quitabp(nom);
		  quitab(nom);
		  _pon_en_ini(nom,1);
	   }
	   fclose(f);
	}
	return 0;
}



#define coma_linea(a,b) comas_linea(a,b,1,&coma)

#define MAXMENUS  100
#define MAXSUBMENU 10       /* numero de submenus */
#define TAMANO     10       /* opciones en una pagina */
#define MAXOP     100       /* opciones en un menu */
#define MAXNOMOP   30       /* tamanyo del nombre de opcion */
#define MAXNOMPRO  81       /* tamanyo del programa de opcion */

struct t_opcion {
   char *o_nombre;
   int  o_menuhijo;
   char *o_programa;
};
#define T_OPCION struct t_opcion

struct t_menu {
   int      m_nop;       /* numero de opciones del menu */
   int      m_menupadre; /* indice del menu padre */
   int      m_fqp;       /* fila > 0 esquina arriba, < 0 esquina abajo */
   int      m_cqp;       /* col. > 0 esquina izquierda, < 0 esquina derecha */
   T_OPCION **m_op; /* opciones */
};
#define T_MENU struct t_menu


static int wlee_un_submenu(_wmenu,nmenus,def,wdef,padre,path,i,texto,file1,r)
T_MENU ***_wmenu;
int *nmenus;
int *def;
int *wdef;
int padre;
char *path;
int i;
char *texto;
FILE *file1;
int *r;
{
   register int j;
   char *elhijo;
   FILE *file2;
   int separa,separa2,separa3;
   int memoria,rrr;
   int coma;
   char tmp[81];
   T_MENU **wmenu = *_wmenu;
   
   lee_linea(texto,128,file1);
   wmenu[i] = (T_MENU *)malloc(sizeof(T_MENU));
   wmenu[i]->m_nop = atoi(texto+coma_linea(texto,0));
   wmenu[i]->m_menupadre = atoi(texto+coma_linea(texto,1));
   if (padre > -1)
      wmenu[i]->m_menupadre = padre;
   wmenu[i]->m_fqp       = atoi(texto+coma_linea(texto,1));
   wmenu[i]->m_cqp       = atoi(texto+coma_linea(texto,1));
   wmenu[i]->m_op = (T_OPCION **)malloc(wmenu[i]->m_nop*sizeof(T_OPCION *));
   for (j=0;j<wmenu[i]->m_nop;j++)
   {
      wmenu[i]->m_op[j] = (T_OPCION *)malloc(sizeof(T_OPCION));
      lee_linea(texto,128,file1);
      separa = coma_linea(texto,0);
      memoria = strlen(texto+separa)+2;
      wmenu[i]->m_op[j]->o_nombre = malloc(memoria);
      strcpy(wmenu[i]->m_op[j]->o_nombre,texto+separa);
      
      elhijo = texto+coma_linea(texto,1);
      
      separa = coma_linea(texto,1);
      if (elhijo[0] == ':')
      {
         sprintf(tmp,"0 %s ",elhijo+1);
         separa2 = coma_linea(texto,1);
         separa3 = coma_linea(texto,1);
         if (strlen(texto+separa2))
         {
            strcat(tmp,texto+separa2);
         }
         else
         {
            strcat(tmp,fichdir);
         }
         
         if (strlen(texto+separa))
         {
            strcat(tmp," ");
            strcat(tmp,texto+separa);
            strcat(tmp," ");
         }
         else
         {
            strcat(tmp," NULL ");
         }
         
         if (strlen(texto+separa3))
         {
            strcat(tmp,texto+separa3);
         }
         else
         {
            strcat(tmp,elhijo+1);
         }
      }
      else
      {
         strcpy(tmp,texto+separa);
      }
      wmenu[i]->m_op[j]->o_programa = malloc(strlen(tmp)+2);
      strcpy(wmenu[i]->m_op[j]->o_programa,tmp);
      
      /* texto ya no se usa */
      if (elhijo[0] != ':' && elhijo[0] != '-' && (elhijo[0] < '0' || elhijo[0] > '9'))
      {
         strcpy(texto,path);
         strcat(texto,elhijo);
         strcat(texto,".smn");
         file2 = fopen(texto,"r");
         if (file2)
         {
            wmenu[i]->m_op[j]->o_menuhijo = *nmenus;
            (*nmenus)++;
            wmenu = (T_MENU **)realloc(wmenu,sizeof(T_MENU *) * ((*nmenus) + 1));
            *_wmenu = wmenu;
            wlee_un_submenu(_wmenu,nmenus,def,wdef,i,path,(*nmenus)-1,texto,file2,r);
            fclose(file2);
         }
         else
         {
            wmenu[i]->m_op[j]->o_menuhijo = -1;
         }
      }
      else
      {
         if (elhijo[0] == '-' || elhijo[0] == ':')
            wmenu[i]->m_op[j]->o_menuhijo = -1;
         else
            wmenu[i]->m_op[j]->o_menuhijo = atoi(elhijo);
      }
      /*if (j == 0)
      break;*/
      if (i == ((*wdef) / 100) && (((*wdef)%100) - 1) == j) 
      {
         if (wmenu[i]->m_op[j]->o_menuhijo > -1)
         {
            (*r)++;
            if ((*r) < MAXSUBMENU)
               *wdef = wmenu[i]->m_op[j]->o_menuhijo * 100 + def[(*r)];
         }
      }
   }
   return(0);
}


static free_omenu(wmenu,i)
T_MENU **wmenu;
int i;
{
   int j;
   
   for (j=0;j<wmenu[i]->m_nop;j++) {
      free(wmenu[i]->m_op[j]->o_nombre);
      free(wmenu[i]->m_op[j]->o_programa);
      free((char *)wmenu[i]->m_op[j]);
   }
   free((char *)wmenu[i]->m_op);
}

static free_menu(wmenu,i)
T_MENU **wmenu;
int i;
{
   free_omenu(wmenu,i);
   free((char *)wmenu[i]);
}

static int procesa_nivel(T_MENU **wmenu,FILE *f,int nivel,int *opcion,int m)
{
			int i,j;
         
         opcion[nivel] = 1;
         /*fprintf(f,"!%d,%d\n",wmenu[m]->m_fqp,wmenu[m]->m_cqp);*/
         for (i = 0;i < wmenu[m]->m_nop;i++)
         {
            for(j = 0;j <= nivel;j++)
            {
               if (j)
                  fprintf(f,",%d",opcion[j]);
               else
                  fprintf(f,"%d",opcion[j]);
            }
            fprintf(f,":%-30s",wmenu[m]->m_op[i]->o_nombre);
            if (wmenu[m]->m_op[i]->o_menuhijo > -1)
            {
               fprintf(f,"\n");
               procesa_nivel(wmenu,f,nivel+1,opcion,wmenu[m]->m_op[i]->o_menuhijo);
            }
            else
            {
               fprintf(f,"{{%s}}\n",wmenu[m]->m_op[i]->o_programa);
            }
            opcion[nivel]++;
         }
         return(0);
}

int convierte_a_fgvmenu(file)
char *file;
{
			char path[256];
         char texto[256];
         int def[MAXSUBMENU];
         int wdef;
         char nommenu[128];
         int i,final,r,nivel;
         int opcion[50];
         int nmenus;
         int coma = 0;
         T_MENU **wmenu;
         FILE *f;
         /* cargar */
         
         strcpy(path,file);
         strcat(path,".men");
         
         f = fopen(path,"r");
         if (!f) return(-1);
         
         lee_linea(texto,82,f);
         quitabp(texto);
         if (texto[0] == '#' || texto[0] == ';')
         {
            fclose(f);
            return(0);
         }
         
         for (i=0;i<MAXSUBMENU;i++)
            def[i] = atoi(texto+coma_linea(texto,i));
         
         r = 0;
         wdef = def[r];
         
         lee_linea(texto,82,f);
         strcpy(nommenu,texto+coma_linea(texto,0));
         nmenus = atoi(texto+coma_linea(texto,1));
         
         wmenu = (T_MENU **)malloc(sizeof(T_MENU *) * (nmenus + 1));
         final = nmenus;
         for (i=0;i<final;i++) 
         {
            wlee_un_submenu(&wmenu,&nmenus,def,&wdef,-1,path,i,texto,f,&r);
         }
         fclose(f);
         
         r = 1;
         strcpy(path,file);
         strcat(path,".men");
         f = fopen(path,"w");
         if (!f)
         {
            r = -1;
            goto salida_menu;
         }
         
         fprintf(f,"#:%s\n",nommenu);
         nivel = 0;
         procesa_nivel(wmenu,f,nivel,opcion,0);
         
         fclose(f);
         
         
salida_menu:
         for (i = 0; i < nmenus ;i++)
            free_menu(wmenu,i);
         free((char *)wmenu);
         
         return(r);
}


int compon_agi_menu()
{
   int i,j;
   char tmp[256];
   char nommenu[128];
   FILE *f,*f2;
   
   if (!AgiRegistro) return(-1);
   
   if (!UpdateRegistro) return(-1);
   
   UpdateRegistro = 0;
   
   /* ver si es necesario crearlo */
   for (i = 0;AgiRegistro[i].menu[0];i++)
   {
      if ( (AgiRegistro[i].flags & 1) ) break;
   }
   if (AgiRegistro[i].menu[0])
   {
      f = fopen(rw_directorio("ds.men"),"w");
      if (f)
						{
         if (AgiRegistro[0].menu[0] == 1 && AgiRegistro[0].orden == 0)
         {
            fprintf(f,"%s\n",AgiRegistro[0].descripcion);
         }
         else
         {
            fprintf(f,"#0:MENU GENERAL\n");
         }
         for (i = 0;AgiRegistro[i].menu[0];i++)
         {
            if (AgiRegistro[i].menu[0] == 1)
            {
               if (AgiRegistro[i].orden > 0)
               {
                  fprintf(f,"%s\n",AgiRegistro[i].descripcion);
               }
            }
            else
               if ( (AgiRegistro[i].flags & 1) )
               {
                  strcpy(nommenu,AgiRegistro[i].descripcion);
                  sprintf(tmp,"%s/%s.men",rw_directorio(AgiRegistro[i].directorio),AgiRegistro[i].menu);
                  f2 = fopen(tmp,"r");
                  if (f2)
                  {
                     while (fgets(tmp,250,f2))
                     {
                        quitan(tmp);
                        quitab(tmp);
                        quitan(tmp);
                        quitabp(tmp);
                        if (tmp[0] == '#')
                        {
                           for (j = 0;tmp[j] && tmp[j] != ':';j++);
                           if (tmp[j] == ':')
                           {
                              strcpy(nommenu,tmp+j+1);
                           }
                           break;
                        }
                     }
                     fclose(f2);
                  }
                  fprintf(f,"%d:%-30s",AgiRegistro[i].orden,nommenu);
                  if (AgiRegistro[i].acceso[0])
                  {
                     fprintf(f,"\n%d,1:Acceso a Empresa {{0 %s fich %s %s}}\n",AgiRegistro[i].orden,AgiRegistro[i].directorio,AgiRegistro[i].acceso,AgiRegistro[i].menu);
                     fprintf(f,"%d,2:Definir Empresas {{0 %s fich %s NULL}}\n",AgiRegistro[i].orden,AgiRegistro[i].directorio,AgiRegistro[i].empresa);
                  }
                  else
                  {				
                     fprintf(f,"{{0 %s fich NULL %s}}\n",AgiRegistro[i].directorio,AgiRegistro[i].menu);
                  }			 
               }
         }
         fclose(f);
						}
   }
   return(0);
}

static void elimina_comillas(char *s)
{
   int i = 0;
   
   do {
      if (*(s + i) == '"') i++;
      *s = *(s + i);
      if (*s) s++;
   } while(*s);
}


int parser_item_registro(char *tmp,AGIREGISTRO *item)
{
   char tmp2[256];
   int posi,i,salir,que,xx;

   posi = 0;
   que = 0;
   memset(item,0,sizeof(AGIREGISTRO));
   if (tmp[0] == '#')
   {
      item->menu[0] = 1;
      /* inclusion de menu */
      if (tmp[1] == '#')
	  {
         item->orden = 0;
	  }
      else
	  {
         item->orden = atoi(tmp+1);
         if (!item->orden)
         {
            item->orden = 999;
         }
	  }
      strcpy(item->descripcion,tmp+1);
   }
   else
   {
      do						
	  {
         i = posi;
         salir = 0;
         do
         {
            while(tmp[i] && tmp[i] != ',' && tmp[i] != '"') {i++;}
            if (!tmp[i] || tmp[i] == ',') break;
            i++;
            if (tmp[i] == '"') {i++;continue;}
            while(tmp[i] && tmp[i] != '"') {i++;}
            if (!tmp[i]) break;
            i++;
         } while(-1);
                     
         if (!tmp[i]) salir = 1;
         tmp[i] = 0;
         quitab(tmp+posi);
         quitabp(tmp+posi);
         elimina_comillas(tmp+posi);
         switch (que)
         {		
         case 0:
			if (tmp[posi] == '[')
			{
				xx = posi;
				for (;tmp[xx] && tmp[xx] != ']';xx++);
				if (tmp[xx] == ']')
				{
					xx++;
					memcpy(item->corchetes,tmp+posi,xx-posi);
					item->corchetes[xx-posi] = 0;
					posi = xx;
				}
			}
            strcpy(item->menu,tmp+posi);
            break;
         case 1:
            strcpy(item->directorio,tmp+posi);
            break;
         case 2:
            strcpy(item->instalable,tmp+posi);
            break;
         case 3:
            if (tmp[posi] > 96 && tmp[posi] < 123)
               tmp[posi] -= 32;
            if (tmp[posi] == 'S')
               item->flags |= 1;
            else
               if (tmp[posi] == 'X')
               {
                  sprintf(tmp2,"%s/%s",rw_directorio(item->directorio),item->menu);
                  if (convierte_a_fgvmenu(tmp2) >= 0)
                  {                 
                     item->flags |= 1;
                  }
               }
               if (tmp[posi] == 'S') 
                  item->flags |= 2;
               else
                  if (tmp[posi] == 'X') 
                     item->flags |= 4;
                  break;
         case 4:
            item->orden = atoi(tmp+posi);
            if (!item->orden)
            {
               item->orden = 1;
            }
            break;
         case 5:
            strcpy(item->descripcion,tmp+posi);
            break;
         case 6:
            strcpy(item->acceso,tmp+posi);
            break;
         case 7:
            strcpy(item->empresa,tmp+posi);
            break;
         default:
            break;
         }
         posi = i + 1;
         que++;
      } while(!salir);
   }

}

static int recursion = 0;
int item_registro(char *tmp)
{
   int max_items = 100;
   int actual_items = 0;
   int n,i;
   AGIREGISTRO item;
   
   
   recursion++;
   
   UpdateRegistro = 1;
   
   if (!AgiRegistro)
   {
      if (recursion < 2)
						{
         lee_agiregistro();
         if (!AgiRegistro)
         {
            recursion--;
            return(-1);
         }
	  }
   }
   
   if (!AgiRegistro)
   {
      AgiRegistro = (AGIREGISTRO *)malloc(sizeof(AGIREGISTRO) * max_items);	  
      memset(AgiRegistro,0,sizeof(AGIREGISTRO) * max_items);
   }
   else
   {
      for (actual_items = 0;AgiRegistro[actual_items].menu[0];actual_items++);
      max_items = (actual_items / 100) * 100 + 100;
   }


   parser_item_registro(tmp,&item);
   


  for (n = 0;n < actual_items;n++)
  {
     
     if (AgiRegistro[n].orden == item.orden &&
        !strcmp(AgiRegistro[n].directorio,item.directorio) &&
        !strcmp(AgiRegistro[n].menu,item.menu)
        )
     {
        AgiRegistro[n] = item; /* sustitucion!!! */
        n = -1;
        break;
     }
     
     if (AgiRegistro[n].orden > item.orden)
     {
        for (i = actual_items;i > n;i--)
        {
           AgiRegistro[i] = AgiRegistro[i-1];
        }
        break;
     }
  }
  if (n >= 0)
  {
     AgiRegistro[n] = item;
     actual_items++;   
     if (actual_items >= max_items)
     {
        max_items += 100;
        AgiRegistro = (AGIREGISTRO *)realloc((char *)AgiRegistro,sizeof(AGIREGISTRO) * max_items);
     }   
     memset(&item,0,sizeof(item));
     AgiRegistro[actual_items] = item;
  }
  
  recursion--;
  
  return(0);
}


int lee_agiregistro()
{
   FILE *f;
   char tmp[256];
   
   /*
   ; buscar por menu!!!! 
   ;Activo,menu,directorio,instalable,orden menu,"descripcion generica",empresadef,empresaac
   agicomer,agicomer,agiges,X,30,"Gestion Comercial",agifa041,agif0041
   agicont,agicont,agicontp,X,40,"Contabilidad",canempr1,canempre
   */
   if (AgiRegistro) 
   {
      free((char *)AgiRegistro);
      AgiRegistro = NULL;
   }
   
   recursion++;
   
   f = fopen(rw_directorio("dev/ds.reg"),"r");
   if (!f)
   {
      strcpy(tmp,rw_directorio("dev/agi.reg"));
      rename(tmp,rw_directorio("dev/ds.reg"));
      f = fopen(rw_directorio("dev/ds.reg"),"r");
   }
   
   if (!f) 
   {
      recursion--;
      return(-1);
   }
   
   while(fgets(tmp,250,f))
   {   
      quitan(tmp);
      quitab(tmp);
      quitabp(tmp);
      if (tmp[0] == ';' || !tmp[0]) continue;
      item_registro(tmp);
   }
   
   fclose(f);
   
   recursion--;
   
   return(0);
}

int graba_agiregistro()
{
   FILE *f;
   int i;
   char c;
   char tmp[256];

   return -1; /* hasta que se acabe ... */
   
   if (!AgiRegistro) return(-1);
   
   f = fopen(rw_directorio("dev/ds.reg"),"w");
   if (!f) return(-1);
   for (i = 0;AgiRegistro[i].menu[0];i++)
   {
      if (AgiRegistro[i].menu[0] == 1)
      {
         fprintf(f,"#%s\n",AgiRegistro[i].descripcion);
      }
      else
      {
         c = 'N';
         if ((AgiRegistro[i].flags & 2))
            c = 'S';
         else
            if ((AgiRegistro[i].flags & 4))
               c = 'X';
            fprintf(f,"%s,%s,%s,%c,%d,\"%s\"",AgiRegistro[i].menu,AgiRegistro[i].directorio,AgiRegistro[i].instalable,c,AgiRegistro[i].orden,AgiRegistro[i].descripcion);
            if (AgiRegistro[i].acceso[0])
            {
               fprintf(f,",%s,%s\n",AgiRegistro[i].acceso,AgiRegistro[i].empresa);
            }
            else
            {
               fprintf(f,"\n");
            }
      }
   }
   fclose(f);
   
   return(0);
}

int fin_agiregistro()
{
   if (AgiRegistro) 
   {
      free((char *)AgiRegistro);
      AgiRegistro = NULL;
   }
   return(0);
}

int carga_filtro_x(char *filtro,char ***_lista_filtro)
{
	char **lista_filtro = *_lista_filtro;
    FILE *f = fopen(filtro,"r");
    if (f)
    {
        char tmp_nom[128];
        int lineas = 0,kk;
        while(fgets(tmp_nom,120,f))
        {
            quitan(tmp_nom);
            quitab(tmp_nom);
			if (tmp_nom[0])
			{
				lineas++;
			}
        }
        rewind(f);
        lista_filtro = (char **)malloc(sizeof(char *)*(lineas+1));
        kk = 0;
        while(kk < lineas && fgets(tmp_nom,120,f))
        {
            quitan(tmp_nom);
            quitab(tmp_nom);
			if (tmp_nom[0])
			{
				lista_filtro[kk] = malloc(strlen(tmp_nom)+1);
				strcpy(lista_filtro[kk],tmp_nom);
				kk++;
			}
        }
        lista_filtro[kk] = NULL;
        fclose(f);
		*_lista_filtro = lista_filtro;
		return 1;
    }
	return 0;
}

#ifdef RWSERVER
int carga_ini_de_usuario() {
	char nom[512];
       FILE *file;
       int i,j;
       int linea;

    strcpy(nom,rw_directorio("dev/usr/"));
	strcat(nom,rw_user);
	strcat(nom,".ini");


       

       file = fopen(nom,"rt");
       
       if (!file) 
       {
          return(0); 
       }
       
       
       linea = 0;
       while(fgets(nom,250,file))
       {
          linea++;
          quitan(nom);
          quitab(nom);
          quitabp(nom);
          if (!nom[0] || nom[0] == ';')
             continue;
          for (i = 0;nom[i] && nom[i] != '=' && nom[i] != ';';i++);
          if (nom[i] != '=')
          {
			  continue;
          }
          i++;
          for (j = i;nom[j];j++)
          {
             if (nom[j] == ';')
             {
                nom[j] = 0;
                break;
             }
          }
          quitabp(nom);
          quitab(nom);

		  pon_en_ini(nom);
		  if (en_conexion_externa) {
			  llama_a_despachador((int)DESPACHO_PONINI,0,nom);      
		  }

       }
       fclose(file);
	
	return 0;
}
#endif
