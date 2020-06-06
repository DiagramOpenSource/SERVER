
#include <fgvsys.h>

int abandonarelbarco = 0;

#ifdef RWXARXA

#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define CONSTDARG
#include <stdarg.h>

#ifdef FGVDOS
#include <io.h>
#include <process.h>
#include <windows.h>            
#include <windowsx.h>

#include <winsock.h>
#include <nspapi.h>
#include <sys/utime.h>

#define NTFACTOR 1000

#else

#include <sys/time.h>
struct timeval tmval;
/* NTFACTOR NO DEFINIDO */

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/filio.h>
#include <netdb.h>
#include <sys/time.h>
#ifdef WATCOMNLM
#include <io.h>
#else
#include <unistd.h>
#endif
#include <utime.h>

#define SOCKET int
#define SOCKET_ERROR -1
#define INVALID_HANDLE_VALUE -1
#define closesocket close
#define ioctlsocket ioctl

#define FAR                 far
#define NEAR                near
#ifndef CONST
#define CONST               const
#endif

typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef float               FLOAT;
typedef int                 INT;
typedef unsigned int        UINT;

#ifndef FALSE
#define FALSE               0
#endif

#ifndef TRUE
#define TRUE                1
#endif

#endif


#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>

#ifndef O_BINARY
#define O_BINARY 0
#endif


#ifdef FGVWIN
#include <fgvwin.h>
#else
#define MID_XPRIMERA  8000
#define MID_XOPCION1 18001
#define MID_DISENO 1100
#endif

#include <rtermin.h>
#include <rwmenus.h>
#include <teclas.h>

extern int (*_pon_incidencia)();

typedef struct _MSGHDR
{
		 BYTE            signature;
         BYTE            command;
         WORD            length;    
} MSGHDR;


#define MAXRDATA 0x6500
typedef struct _DATAMSG            
{
   MSGHDR          hdr;
   BYTE            data[MAXRDATA*2];  
} DATAMSG;

typedef struct _DATAMSG2
{
   MSGHDR          hdr;
   BYTE            *data;
} DATAMSG2;

typedef struct _DATAMSGACK            
{
   MSGHDR          hdr;
   BYTE            data[1];  
} DATAMSGACK;


#define HDRSIZE      sizeof(MSGHDR)


typedef struct _SOCKDATA
{
			SOCKET              sock;			
         struct sockaddr_in  addr;			
         int                 addrlen;		
         struct sockaddr_in  raddr;			
         int                 raddrlen;					
         SOCKET              peer;			
         int type;
         int protocol;
         char name[16];
} SOCKDATA;

union agi_in_addr
{
   struct { u_char s_b1,s_b2,s_b3,s_b4; } S_un_b;
   struct { u_short s_w1,s_w2; } S_un_w;
   u_long S_addr;
};

#define RWNIDENTIFICACION_PING 147
#define RWNIDENTIFICACION    146/* identificacion para los mensajes entorno AGI-DIAGRAM */
#define COMANDO_TERMINAL       1  /* mensajes con subprotocolo entre cliente-servidor, complementa la identificacion*/
#define SESION_ACEPTADA        2  /* el servidor responde que acepta el cliente */
#define SERVIDOR_REQUERIDO     3  /* un cliente pide a rwnetcon una direccion de servidor */
#define SERVIDOR_DISPONIBLE    4  /* un servidor pide a rwnetcon una direccion para esperar a un cliente */
#define DENEGADO_SERVICIO      5  /* rwnetcon deniega la dierccion solicitada */
#define ACEPTADO_SERVICIO      6  /* rwnetcon entrega la direccion solicitada */
#define INICIANDO_SERVIDOR     9
#define CONTROL_DE_TERMINAL   10
#define CONTROL_DE_SERVIDOR   11
#define RECONECTANDO_SERVIDOR 12
#define RECONECTANDO_CLIENTE  13
#define BLOQUE_DE_MENSAJES    14
#define BLOQUE_COMPRIMIDO     15
#define SERVIDOR_INTERRUMPIDO 16
#define FILE_TRANSFER_OPEN    17
#define FILE_TRANSFER_DATA    18
#define FILE_TRANSFER_CLOSE   19
#define FILE_TRANSFER_OK      20
#define FILE_TRANSFER_NOK     21
#define RWNETCON_REQUERIDO    22
#define RWNETCON_CERTIFICADO  23
#define FILE_TRANSFER_END     24
#define FILE_TRANSFER_ACK     25
#define FILE_TRANSFER_NACK	  26
#define SOCORRO			      99

#define AGI_SORT                  7
#define RW_INDEX                  8

unsigned short PORTRWNETCON = 21765;
unsigned short PORTRWNETCON2 = 21765;
unsigned short RWPORTIP = 56743;
int maximoports = 512;


static unsigned char last_cabecera[1024];
static int total_last_cabecera = 0;

int (*ext_carga_dll)() = NULL;

extern int maximo_timeouts;

extern int fuerza_empresa;

extern char *dirbase_ayuda;
extern char *nomapli_ayuda;
extern char referencia_help[128];
extern char *help_externo;

extern int pulsa_en_espacio;

extern int noboton_usuarios;
extern int modosinbarras;
extern int defectoesc;
extern int xtabreturn;
extern int auto_completar;

extern int _modo_portal;

extern int entorno_antivirus;


extern int pausa_transferencia;

extern int busqueda_general;

extern unsigned long ip_2key;
extern char **lista_emprecopias;
extern unsigned char copia_key[248];


/*
static int MAXRDATA=0x2000;
static DATAMSG xferbuf = {'\0','\0',0,NULL};
static DATAMSG xrcbbuf = {'\0','\0',0,NULL};
*/
static DATAMSG xferbuf;
static DATAMSG xrcbbuf;
static DATAMSG xrcbbuf2;

static SOCKDATA MySock;
static SOCKET rwsock = INVALID_HANDLE_VALUE;
static struct sockaddr_in rwnetcon;
unsigned long rwsockaddr = 0L;
unsigned char rwsockhost[1024] = ""; /* ruta alternativa */ /* ping adicional */
unsigned char _rwsockc[MAXRDATA*2];
unsigned char *rwsockc = _rwsockc;


static struct sockaddr_in *ds_proxy = NULL;
static struct sockaddr_in _ds_proxy;

extern char *dsproxy;


/*Header	00 02 01 00 00 01 00 00 00 00 00 00
  Message	09 .. .. .. .. .. .. .. .. .. 03 .. .. .. 00 00 01 00 01 */

extern int no_instala_enbucla;

extern int data_hiden_indicator;
extern int data_fecha_conmutar;

extern char numerofax[300];
extern unsigned char nombredev[81],           /* nombre */
oncompri[17],             /* secuencia para comprimir */
offcompri[17],            /* secuencia para descomprimir */
onnegrita[17],            /*   "            negrita */
offnegrita[17],           /*   "            quitar negrita */
onexpande[17],            /*   "            expandido */
offexpande[17],           /*   "            quitar expandido */
inicializo[37],
finalizo[37],
saltopag[17],
otrocar[17];

extern int i_nlineas;
extern int i_anchocarro;
extern int es_esclava;
extern unsigned char no_reset;

extern void RetroDespachador(void *puntero);

#ifdef FGVDOS
#ifndef FGVWIN
#ifndef WATCOMNLM
extern HANDLE myconsola;
extern HANDLE hStdIn;
#endif
#endif
#endif


#ifndef IPTOS_THROUGHPUT
#define IPTOS_THROUGHPUT 0x08
#endif
#ifndef IPTOS_LOWDELAY
#define IPTOS_LOWDELAY 0x10
#endif


#ifdef RWXARXAIDX
extern int _en_conexion_server_;
extern int _en_conexion_index_;
#endif


static char lpServName[81];

extern int en_conexion_terminal;
extern int leecar_terminal_puro;

extern char *quita_path();

extern char server_request[128];
extern int  server_id;
static char dir_solicitado[51];

char server_dirbase1[81];
static char server_dirbase2[81];
int server_ver1 = 0;
int server_ver2 = 0;
unsigned char server_verletra = 0;
int server_build = 0;
static int server_que_sistema_es;
static int server_es_sistema_grafico;

extern char segundo_dir[128];

extern char dirbass[51];
extern char dirbase[51];
extern char nomapli[25];
extern char nmen0[551];
extern char par1[551];
extern char par2[551];

extern unsigned char la_ethernet_[6];

extern int pendiente_de_instalar;

extern char *lee_linea();

#define MAXFILESINSERVER 20
FILE *files_in_server[MAXFILESINSERVER];
FILE *files_in_client[MAXFILESINSERVER];
char *name_in_server[MAXFILESINSERVER];
char *name_in_client[MAXFILESINSERVER];
unsigned long time_in_server[MAXFILESINSERVER];
int nfiles_in_server = 0;
extern int pass_cambio_autorizado;

#ifdef RWSERVER
extern int en_conexion_externa;
extern int en_conexion_externa_stop;
extern int remoto_ver1;
extern int remoto_ver2;
extern unsigned char remoto_verletra;
extern int remoto_build;
extern int remoto_que_sistema_es;
extern int remoto_es_sistema_grafico;
extern int leecar_server_puro;
extern char remoto_dirbass[51];
#endif
extern int identidad_cliente;

#ifdef FGVWIN
extern char **tabla_icon;
extern short *punto_tabla;
extern int ntabla_icon;
extern HWND ventana_avisos;
#endif


#ifdef FGVSYS
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
#endif

static long trafico_salida[60];
static long trafico_entrada[60];
static long bytes_entrada[60];
static long bytes_salida[60];
static long trafico_time = 0L;
static FILE *file_trafico = NULL;
extern char *log_trafico_tcp;

static int remoto = 0;

static int finalizando_conexion = 0;

static DWORD remote_vent[1000];
static VENT *local_vent[1000];
static int   nextventfree = 0;

extern int user_who;
extern int subnumero_de_usuario;
extern char rw_user[11];

extern int fecha_protegida;
extern int linea_25;
extern int duplicarusuario;
extern char *cabeza_especial;
extern int *rw_extranumuser;
extern char *rw_usuario;
extern int  menusg_virtual;
extern int win_fondo_neutro;

extern FILE *_imp;

extern int Pseudo10;

struct datos_comunes
{
	long *nume_serie;
	long *server_nume_serie;
	long *nume_serie_real;
	unsigned short *local_term;
	unsigned short *local_term_;
	unsigned short *local_acce_;
	int *en_conexion_terminal;
	char reservado[1024];
} *datoscomunes = NULL;

extern long nume_serie;
extern long server_nume_serie;
extern long nume_serie_real;
extern unsigned short local_term[52];
extern unsigned short local_term_[52];
extern unsigned short local_acce_[52];
static unsigned short nume_code;
static unsigned short posicion,nume_mochila;


extern int siempre_password;

extern int isslowconection;
extern int consulta_vieja;

extern int _PCcolor;
extern int _PFcolor;
extern int PCcolor;
extern int PFcolor;

extern char _fispool[51];
extern char _ejspool[51];

extern int que_sistema_es;
extern int es_sistema_grafico;
extern int dsdb_usa_crypto;
extern char *dsdb_;

int solouno = 0;

extern int default_redita_atrib;
extern int default_redita_cc;
extern int default_redita_cf;

extern int redita_atrib;
extern int redita_cc;
extern int redita_cf;
extern int rtme_version1;
extern int rtme_version2;
extern unsigned char rtme_versionletra;
extern int numero_build;
extern int min_puesto_build;

extern int terminal_internal_build;
extern int server_internal_build;
extern int server_fgvid;
extern int fgv_internal_build;

extern int excel_alt;

extern int sos_activado;
extern int conexion_lenta;

/* rentra */
extern char clave_help[32];
extern char *dato_entra;
extern int dato_entra_len;
extern void (*rentra_pos)();
extern int rentra_autosal;
extern int rentra_edicion; 
extern int rentra_atrib;
extern int rentra_sal_atrib;
extern int rentra_cc;
extern int rentra_cf;
extern int rentra_sal_cc;
extern int rentra_sal_cf;
extern int rentra_offqp; 
extern int rentra_offini;
extern int rentra_insert;

extern int redita_col;
extern int qpi_rt;
/*********/


int extra_de_relleno_enviafichero = 0;

static int  rwmax_buffer = 0x6500; //0x6500
static unsigned char *gzip_buffer = NULL;
static unsigned char *rwsend_buffer = NULL;
static unsigned char *rwrcv_buffer = NULL;
static int  rwp_send_buffer = 0;
static int  rwp_rcv_buffer = 0;
static char *punt_rcvb = NULL;

static int envio_situacion = 0;
static int no_entres_en_terminal = 0;

extern void pinta_col();

extern int modo_de_validacion;

extern short buf_tecla[200];

extern short FUNCIONES;
extern int (*f_util)();


extern int (*f_disparo)();

extern char *agi_aplicacion;
extern char *agi_empresa;
extern char *agi_codi_empre;

char agi_aplicacion_[81];
char agi_empresa_[81];
char agi_codi_empre_[81];

static int fichero_local_remoto = 0;


static int server_espera_control = 0;

static int _usando_compresion = -1;
static int usando_compresion = 0;
#ifndef MSG_WAITALL
#define MSG_WAITALL 0
#endif

static int en_progreso = 0;
static long timeout_send = 0L;

/***** ATENCION *******/
static int soporta_ver1=9;
static int soporta_ver2=0;
static unsigned char  soporta_letra=0;


static int soportas_ver1=9;
static int soportas_ver2=0;
static unsigned char  soportas_letra=0;
/***********************/


static char x_dir_smenu[81];
static char x_dir_sgmenu[81];


/* LOG ESPECIAL */

extern char *haylog_mensajes_cs;
static int file_log_especial = -1;
void log_mensajes_cs(int tipo,void *_buffer,int totalbytes)
{
   DATAMSG *pr;
   char tmp[128];
   long pos;
   if (!haylog_mensajes_cs)
		return;
   if (!tipo || tipo == 1)
   {
	   pr = (DATAMSG *)_buffer;
	   totalbytes = pr->hdr.length;	   
   }
   if (file_log_especial == -1)
   {
	   sprintf(tmp,"%s.%d.log",haylog_mensajes_cs,getpid());
	   file_log_especial = rw_open_(tmp,O_RDWR | O_BINARY | O_CREAT,S_IREAD|S_IWRITE);
	   if (file_log_especial == -1)
	   {
		   return;
	   }
	   pos = rw_lseek_(file_log_especial,0,SEEK_END);
	   if (!pos)
	   {
		   pos = 0x71341231;
		   rw_write_(file_log_especial,&pos,sizeof(pos));
	   }
   }
   rw_write_(file_log_especial,&tipo,sizeof(tipo));
   rw_write_(file_log_especial,&totalbytes,sizeof(totalbytes));
   rw_write_(file_log_especial,_buffer,totalbytes);

   /* forzar grabacion */
   rw_close_(file_log_especial);
   file_log_especial = -1;
}


/****************/

/*************************************************************************************/
/*           FUNCIONES GENERALES      */
/*************************************************************************************/


unsigned char BtoH(char ch)
{
			if (ch >= '0' && ch <= '9') return (ch - '0');
         if (ch >= 'A' && ch <= 'F') return (ch - 'A' + 0xA);
         if (ch >= 'a' && ch <= 'f') return (ch - 'a' + 0xA);
         return(255);
}



void AtoH(char * src, char * dest, int destlen)
{
   char * srcptr;
   
   srcptr = src;
   
   while(destlen--)
   {
      *dest = BtoH(*srcptr++) << 4;
      *dest++ += BtoH(*srcptr++);      
   }
}


static void pinta_log_trafico()
{
	struct tm *lc;	
	union agi_in_addr S_un;
	if (log_trafico_tcp)
	{
		if (!file_trafico)
		{
			file_trafico = fopen(log_trafico_tcp,"a+");
		}
		if (file_trafico)
		{
			long totale,totals,bytese,bytess;
			int i;
			totale = totals = bytese = bytess = 0;
			for (i = 0;i < 60;i++)
			{
				totale += trafico_entrada[i];
				totals += trafico_salida[i];				
				bytese += bytes_entrada[i];
				bytess += bytes_salida[i];
			}

   
            S_un.S_addr = MySock.addr.sin_addr.s_addr;  
            fprintf(file_trafico,"%03d.%03d.%03d.%03d <--> ",
						S_un.S_un_b.s_b1,S_un.S_un_b.s_b2,
                        S_un.S_un_b.s_b3,S_un.S_un_b.s_b4
                        );	  
            S_un.S_addr = MySock.raddr.sin_addr.s_addr;  
            fprintf(file_trafico,"%03d.%03d.%03d.%03d, ",
						S_un.S_un_b.s_b1,S_un.S_un_b.s_b2,
                        S_un.S_un_b.s_b3,S_un.S_un_b.s_b4
                        );	  
			lc = localtime(&trafico_time);
			if (!lc)
			{
				fprintf(file_trafico,"FECHA ERRONEA ,");
			}
			else
			{
				fprintf(file_trafico,"%02d/%02d/%02d %02d:%02d,",
					lc->tm_mday ,
					(lc->tm_mon)+1,
					lc->tm_year,
					lc->tm_hour ,
					lc->tm_min);
			}
			fprintf(file_trafico," IN[%10ld,%10ld] OUT[%10ld,%10ld] USER[%s] MAC[%s]\r\n",
				totale,bytese,totals,bytess,rw_user,la_ethernet_);
			fflush(file_trafico);
		}
	}
}

static void FastSocket(SOCKET sock)
{
	int i;

	
	i = IPTOS_LOWDELAY;
	setsockopt(sock,IPPROTO_IP,IP_TOS,(const char *)&i,sizeof(i));
    

	i = 1;
	setsockopt(sock,SOL_SOCKET,SO_OOBINLINE,(const char *)&i,sizeof(i));

	i = -1;
	setsockopt(sock,IPPROTO_TCP,TCP_NODELAY,(const char *)&i,sizeof(i));
}

BOOL rwi_senddata(SOCKET sock,char  *sendbuf,int totalbytes)
{
		 int bytessent;
         int i,err;			
         unsigned char controla;
		 long tiempo;
		 int pj;

		 if (totalbytes == -1)
		 {
			 totalbytes = strlen(sendbuf);
		 }
         
		 if (haylog_mensajes_cs)
		 {
			 log_mensajes_cs(3,(void *)sendbuf,totalbytes);
		 }

         do 
         {
            bytessent = 0;			  
            while(bytessent < totalbytes)
            {
#ifndef WIN32
			   errno = 0;
#endif
               i = send(sock,(char *)sendbuf + bytessent,totalbytes - bytessent,0);
               if (i == SOCKET_ERROR)
               {
#ifdef FGVDOS
                  if (WSAGetLastError() != WSAEWOULDBLOCK)
                  {
					 Sleep(50);
                     return FALSE;
                  }
				  Sleep(10);				  
#else
				  if (errno == EINTR)
					  continue;
                  if (errno != EWOULDBLOCK)
                  {
					 usleep(50000);
                     return FALSE;
                  }
				  usleep(10000);
#endif
				  
                  continue;
               }
               else
                  bytessent += i;
            }

			if (log_trafico_tcp)
			{
				 tiempo = time(NULL);

				 if ((tiempo / 60) != (trafico_time/60))
				 {
					 if (trafico_time)
					 {
						 pinta_log_trafico();
					 }
					 memset((char *)trafico_entrada,0,sizeof(trafico_entrada));
					 memset((char *)trafico_salida,0,sizeof(trafico_salida));
					 memset((char *)bytes_entrada,0,sizeof(bytes_entrada));
					 memset((char *)bytes_salida,0,sizeof(bytes_salida));
					 trafico_time = tiempo;
				 }					 
				 pj = tiempo%60;					 
				 trafico_salida[pj]++;
				 bytes_salida[pj] += bytessent;                     
			}
            
            return TRUE;
            
         } while(-1);
         return FALSE;
}

extern void (*write_interno)();
extern unsigned (*read_interno)();
extern int gzip_asignada_memoria;

static int punto_gzip_read = 0;
static int punto_gzip_write = 0;

static BOOL rwi_recvdata(SOCKET sock,char  *pr,int totalbytes);

static void write_gzip(fd,buf,tam)
int fd;
unsigned char *buf;
unsigned tam;
{
   int i;
   unsigned char mascara;
   if (punto_gzip_write == -1) return;
   if ((punto_gzip_write + tam) > rwmax_buffer)
   {
      punto_gzip_write = -1;
      return;
   }
   if (punto_gzip_write == HDRSIZE)
   {
      gzip_buffer[punto_gzip_write] = rand();
      gzip_buffer[punto_gzip_write+1] = rand();
      gzip_buffer[punto_gzip_write+2] = rand();
      mascara = gzip_buffer[punto_gzip_write] ^ gzip_buffer[punto_gzip_write+2]; 
      for (i = 3;i < tam;i++)
      {
         gzip_buffer[punto_gzip_write+i] = buf[i] ^ mascara;
      }
   }
   else
   {
      memcpy(gzip_buffer+punto_gzip_write,buf,tam);
   }
   punto_gzip_write += tam;
   return;
}

static unsigned read_gzip(pfd,buf,tam)
int *pfd;
unsigned char *buf;
unsigned tam;
{
   unsigned leido;
   int resto;
   if (punto_gzip_read >= rwp_send_buffer) return(0);
   
   resto = rwp_send_buffer - punto_gzip_read;
   if (resto < tam)
			{
      leido = resto;
			}
   else
			{
      leido = tam;
			}  
   memcpy(buf,rwsend_buffer+HDRSIZE+punto_gzip_read,leido);
   punto_gzip_read += leido;
   return(leido);
}

int ds_connect_(SOCKET s,struct sockaddr *addr,int addrlen) {	
	if (ds_proxy)
    {
		char buff[128];
		int len;

		if (connect(s,(struct sockaddr *)ds_proxy,addrlen) == SOCKET_ERROR) {
			return SOCKET_ERROR;
		}

        memset(buff,0,sizeof(buff));
        buff[0]=4; // socks version
  		buff[1]=1; // connect command
		memcpy(&buff[2],&(((struct sockaddr_in *)addr)->sin_port),2); // dest port
		memcpy(&buff[4],&(((struct sockaddr_in *)addr)->sin_addr.s_addr),4); // dest host
		/*
		len=strlen(user)+1; // username
    		if(len>1)	
         		strcpy(&buff[8],user);
		*/
		len = 1;
		if(rwi_senddata(s,buff,(8+len)) == FALSE)
			return SOCKET_ERROR;

		if(rwi_recvdata(s,buff,8) == FALSE) {
			return SOCKET_ERROR;
		}
    
		if (buff[0]!=0||buff[1]!=90) {
			return SOCKET_ERROR;
		}
     	
        return 0;
    }
	return connect(s,addr,addrlen);
}

static BOOL flush_mysock()
{
	     int r;
         DATAMSG *p;
         void (*_write_interno)();
         unsigned (*_read_interno)();
         
         if (!rwp_send_buffer) return(TRUE);
         timeout_send = time(NULL) + 2;
         
         if (!usando_compresion || rwp_send_buffer < 32)
         {
            punto_gzip_write = -1;
         }
         else
         {	
            _write_interno = write_interno;
            _read_interno = read_interno;
            write_interno = write_gzip;
            read_interno = read_gzip;
            punto_gzip_read = 0;
            punto_gzip_write = HDRSIZE;
            ds___gzip(0,0,0,usando_compresion);
            write_interno = _write_interno;
            read_interno = _read_interno;
         }  
         if (punto_gzip_write == -1 || punto_gzip_write >= punto_gzip_read)
         {
            p = rwsend_buffer;
            p->hdr.signature = RWNIDENTIFICACION;
            p->hdr.command   = BLOQUE_DE_MENSAJES;
            rwp_send_buffer += HDRSIZE;
            p->hdr.length = rwp_send_buffer;	
         }
         else
         {
            p = gzip_buffer;
            p->hdr.signature = RWNIDENTIFICACION;
            p->hdr.command   = BLOQUE_COMPRIMIDO;	
            p->hdr.length = punto_gzip_write;
         }
       
		 r = p->hdr.length;
     	 if (haylog_mensajes_cs)
		 {
			log_mensajes_cs(2,(void *)p,r);
		 }
		 if (r > 0)
		 {
			 moto_short(&(p->hdr.length));
			 r = rwi_senddata(MySock.sock,(char *)p,r);  
			 moto_short(&(p->hdr.length));
			 /* piggy backing la actualizacion */

		 }
         rwp_send_buffer = 0;    
         
         return(r);
}

static BOOL senddatamessage (SOCKET sock, DATAMSG *sendbuf)
{
	 	 int totalbytes = sendbuf->hdr.length;
         
         if (sock != MySock.sock)
         {
			moto_short(&(sendbuf->hdr.length));
            totalbytes = rwi_senddata(sock,(char *)sendbuf,totalbytes);
			moto_short(&(sendbuf->hdr.length));
			return totalbytes;
         }
         
         
         if (totalbytes > ((MAXRDATA*2)-HDRSIZE))
         {
            aviso(0,__Texto[105]);
            return(FALSE);
         }
         
		 if (haylog_mensajes_cs)
		 {
			log_mensajes_cs(1,(void *)sendbuf,0);
		 }

         if ( (rwp_send_buffer + totalbytes) >= (rwmax_buffer-2048)) /* 512*/
         {
            DATAMSG guarda_sendbuf;
            memcpy(&guarda_sendbuf,sendbuf,sizeof(guarda_sendbuf));
            flush_mysock(); 
            memcpy(sendbuf,&guarda_sendbuf,sizeof(guarda_sendbuf));
         }
         
		 moto_short(&(sendbuf->hdr.length));
         memcpy( rwsend_buffer + HDRSIZE + rwp_send_buffer , (char *)sendbuf , totalbytes);
		 moto_short(&(sendbuf->hdr.length));
         rwp_send_buffer += totalbytes;		 
         
         if (!timeout_send)
         {
            timeout_send = time(NULL) + 2;
         }
         if (timeout_send < time(NULL))
         {
            flush_mysock();
         }
         return(TRUE);
}


static int punto_gunzip_read = 0;
static int total_gunzip_read = 0;
static int punto_gunzip_write = 0;

static void write_gunzip(fd,buf,tam)
int fd;
unsigned char *buf;
unsigned tam;
{
		 if (punto_gunzip_write == -1) return;
         if ((punto_gunzip_write + tam) > rwmax_buffer)
         {			
            punto_gunzip_write = -1;
            return;
         }
         memcpy(rwrcv_buffer+punto_gunzip_write,buf,tam);
         punto_gunzip_write += tam;
         return;
}

static unsigned read_gunzip(pfd,buf,tam)
int *pfd;
unsigned char *buf;
unsigned tam;
{
   int i;
   unsigned leido;
   unsigned char mascara;
   int resto;
   char *p;
   
   if (punto_gunzip_read >= total_gunzip_read) return(0);
   
   resto = total_gunzip_read - punto_gunzip_read;
   if (resto < tam)
   {
      leido = resto;
   }
   else
   {
      leido = tam;
   }
   
   if (punto_gunzip_read == HDRSIZE)
   {
      mascara = gzip_buffer[punto_gunzip_read] ^ gzip_buffer[punto_gunzip_read+2];
      buf[0] = '\037';
      buf[1] = '\213';
      buf[2] = 8;
      for (i = 3;i < leido;i++)
						{
         buf[i] = gzip_buffer[punto_gunzip_read+i] ^ mascara;
						}
   }
   else
   {
      memcpy(buf,gzip_buffer+punto_gunzip_read,leido);
   }
   punto_gunzip_read += leido;
   return(leido);
}


BOOL recvdatamessage(SOCKET sock, DATAMSG *recvbuf)
{
   static unsigned long totfic = 0;
   int readsize, totalbytesread, msgsize, bytesread,err;
   unsigned char controla;
   DATAMSG *pr;		 
   char tmp[512];
   char tmp2[512];
   void (*_write_interno)();
   unsigned (*_read_interno)();
   unsigned long tam;
   unsigned long rec;     
   unsigned long rectmp;     
   unsigned long tick;
   unsigned long tick2;
   int pl;
   /*VENT *w = NULL;*/
   char *p;
   long tiempo;
   int pj,i;
   static char mala_cabeza[4];
   static int nmala_cabeza = 0;
   static int file = -1;
   
   
   /*pausa_transferencia = 0;*/
   
   flush_mysock();
   
   do      {         
						
						readsize = HDRSIZE;
                  if (sock == MySock.sock)
                  {
                     if (rwp_rcv_buffer)
                     {
                        memcpy((char *)recvbuf,punt_rcvb,readsize);		   
                        punt_rcvb += readsize;			 
                        rwp_rcv_buffer -= readsize;

						moto_short(&(recvbuf->hdr.length));

                        msgsize = recvbuf->hdr.length - readsize;						

                        memcpy((char *)recvbuf+readsize,punt_rcvb,msgsize);
                        rwp_rcv_buffer -= msgsize;
                        punt_rcvb += msgsize;
						if (haylog_mensajes_cs)
						{
							log_mensajes_cs(0,(void *)recvbuf,0);
						}
                        return TRUE;
                     }
                     punt_rcvb = rwrcv_buffer;
                     pr = (DATAMSG *)punt_rcvb;
                  }
                  else
                  {
                     pr = recvbuf;		   
                  }
				  do
				  {
#ifndef WIN32
			   errno = 0;
#endif
                  if((bytesread = recv(sock, (char *)pr,readsize,MSG_WAITALL)) == SOCKET_ERROR)
                  {			
#ifndef WIN32
					 if (errno == EINTR)
					 {						 
						 continue;
					 }
#endif
					  
#ifdef WIN32
					  {
						  int er;
					      er = WSAGetLastError();
						  if (er == WSAEWOULDBLOCK)
						  {
							  Sleep(10);
							  continue;
						  }
						  /*er = er;*/
					  }
#endif
                     return FALSE;
                  }
				  if (haylog_mensajes_cs)
				  {
				     log_mensajes_cs(5,(void *)pr,bytesread);
				  }
				  if (bytesread < readsize)
				  {
					  if (nmala_cabeza > 0 && (nmala_cabeza+bytesread) == readsize && 
						  (mala_cabeza[0] == RWNIDENTIFICACION || mala_cabeza[0] == RWNIDENTIFICACION_PING)
						  )
					  {
						  memcpy(mala_cabeza+nmala_cabeza,(char *)pr,bytesread);
						  memcpy((char *)pr,mala_cabeza,readsize);
						  bytesread = readsize;
						  nmala_cabeza = 0;
					  }
					  else
					  {
						  memcpy(mala_cabeza,(char *)pr,bytesread);
						  nmala_cabeza = bytesread;
						  continue;
					  }
				  }
				  else
				  {
  					  if (bytesread > readsize)
					  {
						  nmala_cabeza = 0;
						  aviso(0,"Posible problema en tcpip stack, reinicie el sistema por favor");
						  continue;
					  }
					  if (nmala_cabeza > 0)
					  {
						  if (!memcmp((char *)pr,mala_cabeza,nmala_cabeza))
						  {
							  nmala_cabeza = 0;
						  }
						  else
						  {
							  i = (bytesread+nmala_cabeza)-1;
							  while (i >= bytesread)
							  {
								  *(((char *)pr)+i) = *(((char *)pr)+i-nmala_cabeza);
								  i--;
							  }
							  memcpy((char *)pr,mala_cabeza,nmala_cabeza);
							  bytesread += nmala_cabeza;
							  nmala_cabeza = 0;
						  }
					  }
				  }

				  if(pr->hdr.signature == RWNIDENTIFICACION_PING)
				  {
					  continue;
				  }
				  break;
				  } while(-1);
                  
                  totalbytesread = bytesread;        
                  if(pr->hdr.signature != RWNIDENTIFICACION)
                  {		   
                     return FALSE;
                  }
                  if (sock != MySock.sock && (pr->hdr.command == BLOQUE_DE_MENSAJES || pr->hdr.command == BLOQUE_COMPRIMIDO))
                  {
                     continue; /* ojo ack */
                  }
                  
                  if (pr->hdr.command == BLOQUE_COMPRIMIDO)
                  {
                     memcpy(gzip_buffer,pr,readsize);
                     pr = (DATAMSG *)gzip_buffer;
                  }
                  else if (pr->hdr.command != BLOQUE_DE_MENSAJES && sock == MySock.sock)
                  {
                     memcpy((char *)recvbuf,(char *)pr,readsize);
                     pr = (DATAMSG *)recvbuf;		   
                  }		 
                     
					 moto_short(&(pr->hdr.length));
                     msgsize = pr->hdr.length;
					 
                     readsize = msgsize - totalbytesread;
                     while(totalbytesread < msgsize)
                     {
#ifndef WIN32
			   errno = 0;
#endif

                        if((bytesread = recv(sock,
                           (char *)pr + totalbytesread,
                           readsize,
                           0)) == SOCKET_ERROR)
                        {
#ifdef FGVDOS
                           if (WSAGetLastError() != WSAEWOULDBLOCK)
                           {		
							  Sleep(50);
                              return FALSE;
                           }
						   Sleep(10);
#else
						   if (errno == EINTR)
							   continue;
                           if (errno != EWOULDBLOCK)
                           {
							  usleep(50000);
                              return FALSE;
                           }
						   usleep(10000);
#endif
                           continue;
                        }
                        totalbytesread += bytesread;
                        readsize -= bytesread;
                     }        

				     if (haylog_mensajes_cs)
					 {
				         log_mensajes_cs(6,(void *)pr,totalbytesread);
					 }

					 if (log_trafico_tcp)
					 {
						 tiempo = time(NULL);

						 if ((tiempo / 60) != (trafico_time/60))
						 {
							 if (trafico_time)
							 {
								 pinta_log_trafico();
							 }
							 memset((char *)trafico_entrada,0,sizeof(trafico_entrada));
							 memset((char *)trafico_salida,0,sizeof(trafico_salida));
							 memset((char *)bytes_entrada,0,sizeof(bytes_entrada));
							 memset((char *)bytes_salida,0,sizeof(bytes_salida));
							 trafico_time = tiempo;
						 }					 
						 pj = tiempo%60;					 
						 trafico_entrada[pj]++;
						 bytes_entrada[pj] += msgsize;
					 }

					 
					 if (pr->hdr.command == FILE_TRANSFER_ACK || pr->hdr.command == FILE_TRANSFER_NACK) {
						 continue;
					 }                     
                     else if (pr->hdr.command == FILE_TRANSFER_OPEN)
                     {
						
                        if (pr->hdr.length > (HDRSIZE+sizeof(long)))
                        {	
						   char *pmd5;
						   pmd5 = NULL;
                           memcpy(&tam,pr->data,sizeof(long));
						   totfic = tam;
                           rec = 0L;
						   rectmp = 100000L;
#ifdef FGVDOS
						   tick = GetTickCount();
#endif                     
						   p = pr->data+sizeof(long);
						   if (*p == 2)
						   {
							   p++;
							   pmd5 = p;
							   p += 32;							   
						   }
                           if (*p == 1)
                           {			     
                              if (other3_server != 56443696 && getenv("DS_TELEBLOQUEO"))
                                 p = NULL;
                              else {
								  if (p[1] == ':') {
									  tmp[0] = 'C';
									  strcpy(tmp+1,p+1);
								  }
								  else
									  strcpy(tmp,p+1);
							  }
                           }
                           else {
                              strcpy(tmp,rw_directorio(p));
						   }
                           if (p)
						   {
							  char tmpmd5[34];							  
							  if (pmd5 && calcula_md5(tmp,tmpmd5) == 0 && !memcmp(pmd5,tmpmd5,32))
							  {
								  
								  xferbuf.hdr.signature = RWNIDENTIFICACION;
								  xferbuf.hdr.command   = FILE_TRANSFER_END;
								  xferbuf.hdr.length = HDRSIZE;
								  if (rwi_senddata(sock,(char *)&xferbuf,HDRSIZE) == FALSE)
								  {                 
									 rw_base_close_(file);
									 file = -1;
									 aviso(0,"control fichero 6");
									 return(FALSE);
								  }
								  continue;
							  }
							  if (file != -1)
							  {
                                 rw_base_close_(file);
								 file = -1;
								 aviso(0,"control fichero 1");
							  }
                              file = rw_base_open_(tmp,O_RDWR | O_CREAT | O_TRUNC | O_BINARY,S_IREAD | S_IWRITE);							  
						   }
                           else
						   {							  
                              file = -1;
						   }
                           if (file >= 0)
                           {
                              xferbuf.hdr.signature = RWNIDENTIFICACION;
                              xferbuf.hdr.command   = FILE_TRANSFER_OK;                 
                              xferbuf.hdr.length = HDRSIZE;
							  
                              if (rwi_senddata(sock,(char *)&xferbuf,HDRSIZE) == FALSE)
                              {                 
                                 rw_base_close_(file);
								 file = -1;
								 aviso(0,"control fichero 2");
                                 return(FALSE);
                              }
                              if (en_conexion_terminal)
                              {
                                 if (DespachadorExterno)
                                 {
                                    (*DespachadorExterno)((int)DESPACHO_PROGRESO,0,(void *)&tam);
									en_progreso = 1;
                                    /*
                                    w = w_ini(2401,2480);
                                    w_salva(w);
                                    pprint(2401,"Recibiendo ");
                                    v_print(tmp);				   
                                    v_printl(" %ld",tam);
                                    pl = (Pcursor/80)*100 + Pcursor%80 + 102;
                                    v_print("/0.");
                                    */
                                 }
                              }			
#ifdef FGVDOS
							  if (en_conexion_terminal)
							  {								 
								 DWORD tt;
								 Sleep(10);
								 tt = GetTickCount()+100;
								 do
								 {
									 pulsatecla();									 
								 }  while(tt > GetTickCount());
								 Sleep(10);								
							  }
#endif
                              continue;
                           }
                        }
                        xferbuf.hdr.signature = RWNIDENTIFICACION;
                        xferbuf.hdr.command   = FILE_TRANSFER_NOK;
                        xferbuf.hdr.length = HDRSIZE;
                        rwi_senddata(sock,(char *)&xferbuf,HDRSIZE);
						
                        continue;
                     }
                     else
                        if (pr->hdr.command == FILE_TRANSFER_DATA)
                        {					       
						   /*mensaje(0,"Tracepoint 0xB005");*/
                           //if (file != -1)
                           //{
							int antiguo = 0;
#ifdef RWSERVER
						   if (en_conexion_externa && terminal_internal_build < 158) 
							   antiguo = 1;
#else
						   if (en_conexion_terminal && server_internal_build < 158)
							   antiguo = 1;
#endif
						   if (antiguo) {
                              rw_base_write_(file,pr->data,msgsize-HDRSIZE);
                              rec += (msgsize-HDRSIZE);
                              if (en_conexion_terminal && DespachadorExterno)
                              {
                                 (*DespachadorExterno)((int)DESPACHO_PROGRESO,1,(void *)&rec);
								 /*en_progreso = 1;*/
                              }
#ifdef FGVDOS
							  if (pausa_transferencia)
								  Sleep(pausa_transferencia);
#endif
                              continue;
						   }						   
						   else {
							   int ultpos;
							   ultpos = 0;
							   if (file != -1)
							   {
								  do {
									  long cuanto;
									  int pos,tam;
									  int errr;
									  long tiempo;
									  int nack;								  

									  tiempo = time(NULL);
									  nack = 0;
									  errr = 0;
									  memcpy(&pos,pr->data,sizeof(int));
									  memcpy(&tam,pr->data+sizeof(int),sizeof(int));
									  
									  if (tam != (pr->hdr.length-HDRSIZE-(sizeof(int)*2)) || ultpos < pos)
										  errr = 1;
									  else {
										  rw_base_lseek_(file,pos,SEEK_SET);
										  rw_base_write_(file,pr->data+sizeof(int)*2,tam);
										  rec = pos + tam;
										  if (rec > ultpos)
											  ultpos = rec;
										  else
											  errr = 0;
									  }

									  if (errr) {
										  xrcbbuf2.hdr.signature = RWNIDENTIFICACION;
										  xrcbbuf2.hdr.command   = FILE_TRANSFER_NACK;
										  xrcbbuf2.hdr.length = HDRSIZE+sizeof(long);
										  memcpy(xrcbbuf2.data,&rec,sizeof(long));
										  moto_short(&(xrcbbuf2.hdr.length));
										  rwi_senddata(sock,(char *)&xrcbbuf2,xrcbbuf2.hdr.length);
										  moto_short(&(xrcbbuf2.hdr.length));
									  }
									  else {
										  /*
										  if (ultpos >= totfic) {
											  xrcbbuf2.hdr.signature = RWNIDENTIFICACION;
											  xrcbbuf2.hdr.command   = FILE_TRANSFER_ACK;
											  xrcbbuf2.hdr.length = HDRSIZE+sizeof(long)+extra_de_relleno_enviafichero;
											  memcpy(xrcbbuf2.data,&rec,sizeof(long));
											  moto_short(&(xrcbbuf2.hdr.length));
											  rwi_senddata(sock,(char *)&xrcbbuf2,xrcbbuf2.hdr.length);
											  moto_short(&(xrcbbuf2.hdr.length));

											  xrcbbuf2.hdr.signature = RWNIDENTIFICACION;
											  xrcbbuf2.hdr.command   = FILE_TRANSFER_CLOSE;
											  xrcbbuf2.hdr.length = HDRSIZE;											  
											  moto_short(&(xrcbbuf2.hdr.length));
											  rwi_senddata(sock,(char *)&xrcbbuf2,xrcbbuf2.hdr.length);
											  moto_short(&(xrcbbuf2.hdr.length));
											  goto salta_aqui_close;
										  }
										  */
										  xrcbbuf2.hdr.signature = RWNIDENTIFICACION;
										  xrcbbuf2.hdr.command   = FILE_TRANSFER_ACK;
										  xrcbbuf2.hdr.length = HDRSIZE+sizeof(long)+extra_de_relleno_enviafichero;
										  memcpy(xrcbbuf2.data,&rec,sizeof(long));
										  moto_short(&(xrcbbuf2.hdr.length));
										  rwi_senddata(sock,(char *)&xrcbbuf2,xrcbbuf2.hdr.length);
										  moto_short(&(xrcbbuf2.hdr.length));
									  }
									  

									  if (en_conexion_terminal && DespachadorExterno)
									  {
										 (*DespachadorExterno)((int)DESPACHO_PROGRESO,1,(void *)&rec);
									  }
								  /*mensaje(0,"Tracepoint 0xB006");							  */
	#ifdef FGVDOS
								  /*
								  tick2 = GetTickCount() - tick;
								  if (tick2 > 3) {
									  Sleep(1);
									  tick = GetTickCount();
								  }	
								  */
									  if (pausa_transferencia)
										  Sleep(pausa_transferencia);
								  
	#endif
	salta_aqui_io:
									  if ( (time(NULL)-tiempo) > (60+(nack*30)) ) {
										  if (ultpos >= totfic) {
											  goto salta_aqui_close;
										  }
										  else {
											  xrcbbuf2.hdr.signature = RWNIDENTIFICACION;
											  xrcbbuf2.hdr.command   = FILE_TRANSFER_NACK;
											  xrcbbuf2.hdr.length = HDRSIZE+sizeof(long);
											  memcpy(xrcbbuf2.data,&rec,sizeof(long));
											  moto_short(&(xrcbbuf2.hdr.length));
											  rwi_senddata(sock,(char *)&xrcbbuf2,HDRSIZE);
											  moto_short(&(xrcbbuf2.hdr.length));
											  nack++;
										  }
									  }
									  if ( (time(NULL)-tiempo) > 600 ) {
										  rw_base_close_(file);
										  if (en_conexion_terminal && DespachadorExterno)
										  {
											 (*DespachadorExterno)((int)DESPACHO_PROGRESO,2,(void *)NULL);
										  }
										  return FALSE;
									  }
									  cuanto = 0L;
									  if (ioctlsocket(MySock.sock,FIONREAD,&cuanto) == SOCKET_ERROR) {
										  rw_base_close_(file);
										  if (en_conexion_terminal && DespachadorExterno)
										  {
											 (*DespachadorExterno)((int)DESPACHO_PROGRESO,2,(void *)NULL);
										  }
										  return FALSE;
									  }
									  if (cuanto >= HDRSIZE) {
										  if (rwi_recvdata(MySock.sock,(char *)&xrcbbuf2,HDRSIZE) == FALSE) {
											  rw_base_close_(file);
											  if (en_conexion_terminal && DespachadorExterno)
											  {
												 (*DespachadorExterno)((int)DESPACHO_PROGRESO,2,(void *)NULL);
											  }
											  return FALSE;
										  }
										  if (xrcbbuf2.hdr.length > HDRSIZE) {
											   if (rwi_recvdata(MySock.sock,((char *)&xrcbbuf2)+HDRSIZE,xrcbbuf2.hdr.length-HDRSIZE) == FALSE) {
												  rw_base_close_(file);
												  if (en_conexion_terminal && DespachadorExterno)
												  {
													 (*DespachadorExterno)((int)DESPACHO_PROGRESO,2,(void *)NULL);
												  }
												  return FALSE;
											   }
										  }
										  if (xrcbbuf2.hdr.command != FILE_TRANSFER_DATA) {
											  if (xrcbbuf2.hdr.command == FILE_TRANSFER_CLOSE) {
												 pr = &xrcbbuf2;
												 break;
											  }
											  
											  if (xrcbbuf2.hdr.command != FILE_TRANSFER_ACK && xrcbbuf2.hdr.command != FILE_TRANSFER_NACK) {
												  /*if (en_conexion_terminal) {*/
															/*pump_mensaje(&xrcbbuf2);*/
												  /*}*/
											  }

											  goto salta_aqui_io;
										  }
										  pr = &xrcbbuf2;
									  }
									  else {
	#ifdef FGVDOS
										  Sleep(10);
	#else
										  usleep(10000);
	#endif
										  goto salta_aqui_io;
									  }
								  } while(-1);
								  if (pr->hdr.command == FILE_TRANSFER_CLOSE)
									  goto salta_aqui_close;
								  continue;
							   }
						   }
                           if (en_conexion_terminal && DespachadorExterno && en_progreso)
                           {
                              (*DespachadorExterno)((int)DESPACHO_PROGRESO,2,(void *)NULL);
							  en_progreso = 0;
                           }
						   if (pr->hdr.command == FILE_TRANSFER_DATA) {							  
							  xrcbbuf2.hdr.signature = RWNIDENTIFICACION;
							  xrcbbuf2.hdr.command   = FILE_TRANSFER_ACK;
							  xrcbbuf2.hdr.length = HDRSIZE+sizeof(long);
							  memcpy(xrcbbuf2.data,&totfic,sizeof(long));
							  moto_short(&(xrcbbuf2.hdr.length));
							  rwi_senddata(sock,(char *)&xrcbbuf2,xrcbbuf2.hdr.length);
							  moto_short(&(xrcbbuf2.hdr.length));
							  xrcbbuf2.hdr.signature = RWNIDENTIFICACION;
							  xrcbbuf2.hdr.command   = FILE_TRANSFER_CLOSE;
							  xrcbbuf2.hdr.length = HDRSIZE;							  
							  moto_short(&(xrcbbuf2.hdr.length));
							  rwi_senddata(sock,(char *)&xrcbbuf2,xrcbbuf2.hdr.length);
							  moto_short(&(xrcbbuf2.hdr.length));
							  continue;
						   }
                           return(FALSE);
                        }
                        else if (pr->hdr.command == FILE_TRANSFER_CLOSE)
                           {
salta_aqui_close:
                              if (file != -1)
                              {			  
                                 if (en_conexion_terminal && DespachadorExterno && en_progreso)
                                 {
                                    (*DespachadorExterno)((int)DESPACHO_PROGRESO,2,(void *)NULL);
									en_progreso = 0;
                                 } 
                                 rw_base_close_(file);
                                 file = -1;
                                 if (pr->hdr.length > HDRSIZE)
                                 {
                                    p = pr->data;
                                    if (*p == 1)
                                       strcpy(tmp2,p+1);
                                    else
                                       strcpy(tmp2,rw_directorio(p));
                                    rw_base_unlink(tmp2);
                                    rw_base_rename(tmp,tmp2); /* si no se puede paciencia !!! */
                                 }
                              }
                              if (en_conexion_terminal && DespachadorExterno && en_progreso)
                              {
                                 (*DespachadorExterno)((int)DESPACHO_PROGRESO,2,(void *)NULL);
								 en_progreso = 0;
                              }
							  /*
							  if ((recvbuf->hdr.command & (unsigned char)128))
							  {
								 recvbuf->hdr.command &= ~(unsigned char)128;
								 recvbuf->hdr.length -= 2;
#ifdef RWSERVER
								local_nivel_mensaje = *((unsigned short *)(recvbuf->data+recvbuf->hdr.length-HDRSIZE));
#else
								remoto_nivel_mensaje = *((unsigned short *)(recvbuf->data+recvbuf->hdr.length-HDRSIZE));
#endif
							  }                     
							  else
								  remoto_nivel_mensaje = 0;
								*/
								 if (haylog_mensajes_cs)
								 {
									log_mensajes_cs(0,(void *)recvbuf,0);
								 }
                              return(TRUE);
                           } else if (pr->hdr.command == BLOQUE_DE_MENSAJES)
                           {			
                                 pr = (DATAMSG *)((char *)pr + HDRSIZE);
                                 memcpy((char *)recvbuf,(char *)pr,HDRSIZE);
                                 pr = (DATAMSG *)((char *)pr + HDRSIZE);			
                                 memcpy((char *)recvbuf + HDRSIZE,(char *)pr,recvbuf->hdr.length-HDRSIZE);
                                 err = msgsize - HDRSIZE - recvbuf->hdr.length;
                                 if (err > 0)
                                 {
                                    punt_rcvb += (HDRSIZE+recvbuf->hdr.length);
                                    rwp_rcv_buffer = err;
                                 }            
                                 if (file != -1)
                                 {
									 /*
                                    rw_base_close_(file);
									file = -1;
									*/
									aviso(0,"control fichero 3");
                                 } 
                                 if (en_conexion_terminal && DespachadorExterno && en_progreso)
                                 {
                                    (*DespachadorExterno)((int)DESPACHO_PROGRESO,2,(void *)NULL);
									en_progreso = 0;
                                 }

								 if (haylog_mensajes_cs)
								 {
									log_mensajes_cs(0,(void *)recvbuf,0);
								 }
                                 return TRUE;                                             
                                     
                           }  else
                                 if (pr->hdr.command == BLOQUE_COMPRIMIDO)
                                 {
                                    _write_interno = write_interno;
                                    _read_interno = read_interno;
                                    
                                    write_interno = write_gunzip;
                                    read_interno = read_gunzip;
                                    total_gunzip_read = pr->hdr.length;
                                    punto_gunzip_read = HDRSIZE;
                                    punto_gunzip_write = 0;
                                    ds___gzip(0,0,1,6);
                                    pr = (DATAMSG *)rwrcv_buffer;
									moto_short(&(pr->hdr.length));
                                    memcpy((char *)recvbuf,(char *)pr,pr->hdr.length);
                                    punto_gunzip_write -= pr->hdr.length;
                                    rwp_rcv_buffer = punto_gunzip_write;
                                    punt_rcvb = (char *)rwrcv_buffer + pr->hdr.length;
                                    
                                    write_interno = _write_interno;
                                    read_interno = _read_interno;
                                    if (file != -1)
                                    {
									   /*aviso(0,"Tracepoint 0xB00D");*/
                                       rw_base_close_(file);
									   file = -1;
                                    } 
                                    if (en_conexion_terminal && DespachadorExterno)
                                    {
                                       (*DespachadorExterno)((int)DESPACHO_PROGRESO,2,(void *)NULL);
                                    }

									 if (haylog_mensajes_cs)
									 {
										log_mensajes_cs(0,(void *)recvbuf,0);
									 }
                                    return TRUE;
                                 }
                                 
                                 if (file != -1)
                                 {
									/*aviso(0,"Tracepoint 0xB00E");*/
                                    rw_base_close_(file);
									file = -1;
                                 }		  
                                 

                                 

                                 if (en_conexion_terminal && DespachadorExterno && en_progreso)
                                 {
                                    (*DespachadorExterno)((int)DESPACHO_PROGRESO,2,(void *)NULL);
									en_progreso = 0;
                                 }          
								 if (haylog_mensajes_cs)
								 {
									log_mensajes_cs(0,(void *)recvbuf,0);
								 }
                                 
                                 return TRUE;
                                 
     } while (-1);
     
     return FALSE;
}


static int convierte_a_cache(nom)
char *nom;
{
   char tmp[512];
   int i;
   /* evitar recursiones */
   strcpy(tmp,rw_directorio(""));
   strcat(tmp,"remoto/");
   if (tmp[0] && !memcmp(nom,tmp,strlen(tmp)))
      return(0);
   strcpy(tmp,dirbass);
   strcat(tmp,"remoto/");
   if (tmp[0] && !memcmp(nom,tmp,strlen(tmp)))
      return(0);
   
   if (!memcmp(server_dirbase1,nom,strlen(server_dirbase1)))
   {
      strcpy(tmp,rw_directorio(""));
      strcat(tmp,"remoto/");
      strcat(tmp,nom+strlen(server_dirbase1));
      strcpy(nom,tmp);     
   }
   else
      if (!memcmp(server_dirbase2,nom,strlen(server_dirbase2)))
      {
         strcpy(tmp,dirbass);
         strcat(tmp,"remoto/");
         strcat(tmp,nom+strlen(server_dirbase2));
         strcpy(nom,tmp);     
      }
      else
         if (nom[0] == '/' || nom[0] == '\\')
         {
            strcpy(tmp,rw_directorio(""));
            strcat(tmp,"remoto/");
            strcat(tmp,nom+1);	 
            strcpy(nom,tmp);	 
         }
         else
            if (nom[1] == ':' && (nom[2] == '/' || nom[2] == '\\'))
            {
               strcpy(tmp,rw_directorio(""));
               strcat(tmp,"remoto/");
               strcat(tmp,nom+3);	 
               strcpy(nom,tmp);	 
            } 
            else
            {
               strcpy(tmp,rw_directorio(""));
               strcat(tmp,"remoto/");
               strcat(tmp,nom);	 
               strcpy(nom,tmp);
            }
            for (i = 0;nom[i];i++)
            {
               if (nom[i] == '\\')
                  nom[i] == '/';
            }
            return(0);
}

int rw_natoi(s,i,n)
unsigned char *s;
int i,n;
{
   char tmp[15];
   if (n > 14) return(0);
   memcpy(tmp,s+i,n);
   tmp[n] = 0;
   return(atoi(tmp));
}

/*************************************************************************************/
/*************************************************************************************/

void libera_x_buffers()
{
			
/*
if (xferbuf.data)
{
free(xferbuf.data);
xferbuf.data = NULL;
}
if (xrcbbuf.data)
{
free(xrcbbuf.data);
xrcbbuf.data = NULL;
}
			*/
			
			if (gzip_buffer)
         {
            free(gzip_buffer);
            gzip_buffer = NULL;
         }
         if (rwsend_buffer)
         {
            free(rwsend_buffer);
            rwsend_buffer = NULL;
         }
         if (rwrcv_buffer)
         {
            free(rwrcv_buffer);
            rwrcv_buffer = NULL;
         }
}


int dame_ip_por_defecto(tmp,ntmp)
char *tmp;
int ntmp;
{
			int i;
         struct hostent *mihost;
         union agi_in_addr S_un;
#ifdef FGVDOS
         WORD      wVersionRequested;
         WSADATA   wsaData;
         
#ifdef RWSERVER
         if (!en_conexion_externa && !en_conexion_terminal)
#else
            if (!en_conexion_terminal)
#endif
            { 
               wVersionRequested = MAKEWORD(1,1);
               if (WSAStartup(wVersionRequested, &wsaData) != 0)
               {	 
                  return -1;
               }
            }
#endif
            if (server_request[0] == '@') 
            {
               for(i = 3;server_request[i] && server_request[i] != ':' && server_request[i] != ')';i++);
               if (ntmp >= (i-3))
               {
                  memcpy(tmp,server_request+3,i-3);
                  tmp[i-3] = 0;
                  quita_blancos(tmp);
                  if (strlen(tmp))
                  {
                     if (inet_addr(tmp) != INADDR_NONE)
                     {
#ifdef FGVDOS
#ifdef RWSERVER
                        if (!en_conexion_externa && !en_conexion_terminal)
#else
                           if (!en_conexion_terminal)
#endif
                           { 
                              WSACleanup();
                           }
#endif
                           return(0);
                     }
                  }
                  else
                     gethostname(tmp,ntmp);
               }
               else
                  gethostname(tmp,ntmp);
            }
            else
               gethostname(tmp,ntmp);
            if ( (mihost = gethostbyname(tmp)) && mihost->h_length == 4 && mihost->h_addr_list[0])
            {	    
               S_un.S_addr = *((unsigned long *)mihost->h_addr_list[0]);
               sprintf(tmp,"%d.%d.%d.%d",S_un.S_un_b.s_b1,S_un.S_un_b.s_b2,S_un.S_un_b.s_b3,
                  S_un.S_un_b.s_b4);
            }
#ifdef FGVDOS
#ifdef RWSERVER
            if (!en_conexion_externa && !en_conexion_terminal)
#else
               if (!en_conexion_terminal)
#endif
               { 
                  WSACleanup();
               }
#endif
               return(0);
}



int dame_nombre_por_ip(tmp,buf)
char *tmp;
char *buf;
{
   int i;
   struct hostent *host;
   unsigned long addr;
#ifdef FGVDOS
   WORD      wVersionRequested;
   WSADATA   wsaData;
   
#ifdef RWSERVER
   if (!en_conexion_externa && !en_conexion_terminal)
#else
      if (!en_conexion_terminal)
#endif
						{ 
         wVersionRequested = MAKEWORD(1,1);
         if (WSAStartup(wVersionRequested, &wsaData) != 0)
         {	 
            return -1;
         }
						}
#endif
      
      
      addr = inet_addr(tmp);
      host = gethostbyaddr (  (char *)&addr,sizeof(addr),AF_INET);
      if (host)
						{
         strcpy(buf,host->h_name);
						}
      
#ifdef FGVDOS
#ifdef RWSERVER
      if (!en_conexion_externa && !en_conexion_terminal)
#else
         if (!en_conexion_terminal)
#endif
         { 
            WSACleanup();
         }
#endif
         return(0);
}


fd_set setread;
fd_set seterror;
struct timeval timeout;

int es_la_ip_remota(char *buf)
{
	if (inet_addr(buf) == MySock.raddr.sin_addr.s_addr)
	{
		return 1;
	}
	return 0;
}

int recoge_ip_remota(char *buf)
{
   union agi_in_addr S_un;
   S_un.S_addr = MySock.raddr.sin_addr.s_addr;  
   sprintf(buf,"%d.%d.%d.%d",
						S_un.S_un_b.s_b1,S_un.S_un_b.s_b2,
                  S_un.S_un_b.s_b3,S_un.S_un_b.s_b4
                  );	  
   return(0);
}


int comprueba_ethernet(unsigned char *la_ethernet_)
{
	char tmp[512];
	char tmpe[512];
	FILE *file;

	sprintf(tmpe,"%02X:%02X:%02X:%02X:%02X:%02X",la_ethernet_[0],la_ethernet_[1],la_ethernet_[2],la_ethernet_[3],la_ethernet_[4],la_ethernet_[5]);

	strcpy(tmp,rw_directorio("bin/"));	
	strcat(tmp,"eth.ds");
	file = fopen(tmp,"r");
	if (file)
	{
		while(fgets(tmp,128,file))
		{				
			quitan(tmp);
			quitab(tmp);
			quitabp(tmp);
			tmp[17] = 0;
			if (!strcmp(tmp,tmpe))
			{
				fclose(file);
				return 1;
			}
		}
		fclose(file);
		return 0;
	}	
	return 1;
}

#ifdef FGVDOS
#define MAX_INTERFACE_NAME_LEN  256
#define MAXLEN_IFDESCR 256
#define MAXLEN_PHYSADDR 8

#define MIB_IF_TYPE_ETHERNET 6

typedef struct _MIB_IFROW
{
    WCHAR   wszName[MAX_INTERFACE_NAME_LEN];
    DWORD	dwIndex;
    DWORD	dwType;
    DWORD	dwMtu;
    DWORD	dwSpeed;
    DWORD	dwPhysAddrLen;
    BYTE	bPhysAddr[MAXLEN_PHYSADDR];
    DWORD	dwAdminStatus;
    DWORD	dwOperStatus;
    DWORD	dwLastChange;
    DWORD	dwInOctets;
    DWORD	dwInUcastPkts;
    DWORD	dwInNUcastPkts;
    DWORD	dwInDiscards;
    DWORD	dwInErrors;
    DWORD	dwInUnknownProtos;
    DWORD	dwOutOctets;
    DWORD	dwOutUcastPkts;
    DWORD	dwOutNUcastPkts;
    DWORD	dwOutDiscards;
    DWORD	dwOutErrors;
    DWORD	dwOutQLen;
    DWORD	dwDescrLen;
    BYTE	bDescr[MAXLEN_IFDESCR];
} MIB_IFROW,*PMIB_IFROW;


typedef struct _MIB_IFTABLE {
  DWORD     dwNumEntries;       // number of entries in table
  MIB_IFROW table[100];    // array of interface entries
} MIB_IFTABLE, *PMIB_IFTABLE;


typedef struct _MIB_IFNUMBER {
  DWORD   dwValue;    // number of interfaces
} MIB_IFNUMBER, *PMIB_IFNUMBER;


  
typedef DWORD (WINAPI *__GetIfTable)(PMIB_IFTABLE pIfTable,  // buffer for interface table 
						 PULONG pdwSize,         // size of buffer
					  	  BOOL bOrder             // sort the table by index?
						 );






// Map from the most-significant 6 bits of the first byte to the total
// number of bytes in a
// UTF-8 character.
static char UTF8_2_ISO_8859_1_len[] =
{
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* erroneous */
  2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 5, 6
};

static char UTF8_2_ISO_8859_1_mask[] = {0x3F, 0x7F, 0x1F, 0x0F, 0x07, 0x03, 0x01};


/*------------------------------------------------------------
----------
-------
   Convert a UTF-8 string to a ISO-8859-1 MultiByte string.
   No more than 'count' bytes will be written to the output buffer.
   Return the size of the converted string in bytes, excl null
terminator.
*/
int
ldap_x_utf8s_to_iso_8859_1s( char *mbstr, const char *utf8str, size_t
count )
{
  int res = 0;

  while (*utf8str != '\0')
  {
    int           len = UTF8_2_ISO_8859_1_len[(*utf8str >> 2) & 0x3F];
    unsigned long u   = *utf8str & UTF8_2_ISO_8859_1_mask[len];

    // erroneous
    if (len == 0)
      len = 5;

    for (++utf8str; --len > 0 && (*utf8str != '\0'); ++utf8str)
    {
      // be sure this is not an unexpected start of a new character
      if ((*utf8str & 0xC0) != 0x80)
        break;

      u = (u << 6) | (*utf8str & 0x3F);
    }

    if (mbstr != 0 && count != 0)
    {
      // be sure there is enough space left in the destination buffer
      if (res >= count)
        return res;

      // add the mapped character to the destination string or '?'
      // (0x1A, SUB) if character
      // can't be represented in ISO-8859-1
      *mbstr++ = (u <= 0xFF ? (char)u : '?');
    }
    ++res;
  }

  // add the terminating null character
  if (mbstr != 0 && count != 0)
  {
    // be sure there is enough space left in the destination buffer
    if (res >= count)
      return res;
    *mbstr = 0;
  }

  return res;
} // ldap_x_utf8s_to_iso_8859_1s


/*------------------------------------------------------------
----------
-------
   Convert a ISO-8859-1 MultiByte string to a UTF-8 string.
   No more than 'count' bytes will be written to the output buffer.
   Return the size of the converted string in bytes, excl null
terminator.
*/   
int
ldap_x_iso_8859_1s_to_utf8s(char *utf8str, const char *mbstr, size_t count)
{
  int res = 0;

  // loop until we reach the end of the mb string
  for (; *mbstr != '\0'; ++mbstr)
  {
    // the character needs no mapping if the highest bit is not set
    if ((*mbstr & 0x80) == 0) 
    {
      if (utf8str != 0 && count != 0)
      {
        // be sure there is enough space left in the 
        //destination buffer
        if (res >= count)
          return res;

        *utf8str++ = *mbstr;
      }
      ++res;
    }

    // otherwise mapping is necessary
    else
    {
      if (utf8str != 0 && count != 0)
      {
        // be sure there is enough space left in the destination buffer
        if (res+1 >= count)
          return res;

        *utf8str++ = (0xC0 | (0x03 & (*mbstr >> 6)));
        *utf8str++ = (0x80 | (0x3F & *mbstr));
      }
      res += 2;
    }
  }

  // add the terminating null character
  if (utf8str != 0 && count != 0)
  {
    // be sure there is enough space left in the destination buffer
    if (res >= count)
      return res;
    *utf8str = 0;
  }

  return res;
} // ldap_x_iso_8859_1s_to_utf8s




static void dame_ethernet_address_win(unsigned char *buf)
{
	MIB_IFTABLE itable;		
	ULONG ss = sizeof(itable);
	__GetIfTable GetIfTable;
	HINSTANCE ll;

	memset(buf,0xff,6);	

	itable.dwNumEntries = 0;
	ll = LoadLibrary("iphlpapi.dll");

	if (ll)
	{
		GetIfTable = (__GetIfTable)GetProcAddress(ll,"GetIfTable");
		if (GetIfTable)
		{
			DWORD ex = GetIfTable(&itable,&ss,FALSE);
			if (ex != NO_ERROR)
			{
				LPVOID lpMsgBuf;
				FormatMessage( 
						FORMAT_MESSAGE_ALLOCATE_BUFFER | 
						FORMAT_MESSAGE_FROM_SYSTEM | 
						FORMAT_MESSAGE_IGNORE_INSERTS,
						NULL,
						/*GetLastError()*/ex,
						MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
						(LPTSTR) &lpMsgBuf,
						0,
						NULL 
					);
				MessageBox( NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );
				// Free the buffer.
				LocalFree( lpMsgBuf );
			}
			else
			{
				DWORD i;
				int primero = 1;
				for(i = 0;i < itable.dwNumEntries;i++)
				{					
					/*MIB_IF_TYPE_ETHERNET*/
					if ((itable.table[i].dwType == 6 || itable.table[i].dwType == 16) && itable.table[i].dwOperStatus > 3 && itable.table[i].dwOutOctets > 0 && itable.table[i].dwInOctets > 0)
					{
						/*
						char tmpx[512];
						char tmpy[512];
						char tmpz[512];
						*/

						if (primero)
							memcpy(buf,itable.table[i].bPhysAddr,6);						
						return;

						/*
						
						 sprintf(tmpx,"%02X-%02X-%02X-%02X-%02X-%02X\n",itable.table[i].bPhysAddr[0],
							itable.table[i].bPhysAddr[1],
							itable.table[i].bPhysAddr[2],
							itable.table[i].bPhysAddr[3],
							itable.table[i].bPhysAddr[4],
							itable.table[i].bPhysAddr[5]
							);

						  WideCharToMultiByte(CP_ACP,WC_DEFAULTCHAR,itable.table[i].wszName,-1,tmpy,511,NULL,NULL);

						  ldap_x_iso_8859_1s_to_utf8s(tmpz,tmpy,500);

						  //wctomb( tmpy, itable.table[i].wszName);

						 MessageBox(NULL,tmpx,tmpz,MB_OK);
						 */
					}
				}
			}
		}
		FreeLibrary(ll);
	}
}
#else

static int numero_hex(char *p)
{
	int r = 0;
	if (*p >= '0' && *p <= '9')
	{
		r = *p - 48;
	}
	else if (*p >= 'a' && *p <= 'f')
	{
		r = *p - 87;
	}
	else if (*p >= 'A' && *p <= 'F')
	{
		r = *p - 55;
	}
	else 
		return 0;
	r *= 16;
	p++;
	if (*p >= '0' && *p <= '9')
	{
		r += (*p - 48);
	}
	else if (*p >= 'a' && *p <= 'f')
	{
		r += (*p - 87);
	}
	else if (*p >= 'A' && *p <= 'F')
	{
		r += (*p - 55);
	}
	else 
		return 0;

	return r;
}


void dame_ethernet_address_unix(unsigned char *buf)
{
	char tmp[524];
	FILE *f;
	int i,j;

	memset(buf,0,6);
	strcpy(tmp,"ifconfig -a > /tmp/eth");
	system(tmp);
	f = fopen("/tmp/eth","r");
	if (f)
	{
		memset(tmp,0,sizeof(tmp));
		fgets(tmp,500,f);
		fclose(f);
		for (i = 0;tmp[i];i++)
		{
			if (tmp[i+2] == ':' && tmp[i+5] == ':' && tmp[i+8] == ':' && tmp[i+11] == ':' && tmp[i+14] == ':')
			{			
				j = i;
				for (i = 0;i < 6;i++)
				{
					buf[i] = numero_hex(tmp+j);
					j += 3;
				}
				break;
			}
		}
	}
}

#endif

void dame_ethernet_address(unsigned char *buf)
{
#ifdef FGVDOS
	dame_ethernet_address_win(buf);
#else
	dame_ethernet_address_unix(buf);
#endif
}

int solouno_()
{
	return (0);
}

int envia_serie()
{
      xferbuf.hdr.signature = RWNIDENTIFICACION;                 
      xferbuf.hdr.command = COMANDO_TERMINAL;
      xferbuf.hdr.length = HDRSIZE + 1 + sizeof(long)*2;
      xferbuf.data[0] = 59; /* activar (por defecto) sistema de mensajes directos */
	  memcpy(xferbuf.data+1,&nume_serie,sizeof(long)*2);
	  memcpy(xferbuf.data+1+sizeof(long),&pass_cambio_autorizado,sizeof(long));

      senddatamessage(MySock.sock, &xferbuf);
}

#ifdef FGVWIN

/* solo el 9 enviara la informacion de medicion de velocidad */
#define ID_TICSCON                      32896

struct dsinfolinea
{
	unsigned short id;
	unsigned short size;
	DWORD tick;
};

static char mensaje_globo_netcon[1024] = "";
static int mensajes_globo_enviados = 0;
static char buffglobo[512];
static char buffglobotarea[512];
static long tiempo_globo = 0L;



/*static int pruba = 0;*/
void EstoyVivo(void *_s)
{
	int err;
	fd_set _setread;
	fd_set _seterror;
	long cuanto;
	SOCKET s = (SOCKET)_s;
	struct dsinfolinea infos;
	struct dsinfolinea infor;
	struct timeval _tout;
	char tmp[1024];
	char tmp2[512];
	int reenvio;

	infos.id = 0x171;
    infos.size = sizeof(infos);
	infos.tick = GetTickCount();

	/*
	if (!pruba)
	{
		      sprintf(mensaje_globo_netcon,"0\0015\001Activada ventana de mantenimiento UUUU.\001 flash!! \001");
			  PostMessage(ventana_avisos,WM_COMMAND,ID_AVISOGLOBO,(LPARAM)mensaje_globo_netcon);
				 pruba++;

	}
	*/
	if (send(s,&infos,sizeof(infos),0) != SOCKET_ERROR)
	do
	{
		 FD_ZERO(&_setread);
		 FD_SET(s,&_setread);
		 FD_ZERO(&_seterror);
		 FD_SET(s,&_seterror);
		 err = 0;
         _tout.tv_sec = 10L;
         _tout.tv_usec = 0L;
#ifndef WIN32
			   errno = 0;
#endif
		 if ((err = select(s+1,&_setread,NULL,&_seterror,&_tout)) == SOCKET_ERROR || FD_ISSET(s,&_seterror))
		 {
#ifndef WIN32
			 if (err == SOCKET_ERROR && errno == EINTR)
			 {
				 continue;
			 }
#endif

			 break;
		 }
		 if (!err)
		 {
			 
			 if (ventana_avisos)
			 {
				PostMessage(ventana_avisos,WM_COMMAND,ID_TICSCON|0x20000,99999999);
			 }
			 
			 infos.tick = GetTickCount();
			 if (send(s,&infos,sizeof(infos),0) == SOCKET_ERROR)
			 {
				 break;
			 }
			 Sleep(100);
			 continue;
		 }
         cuanto = 0L;
         ioctlsocket(s,FIONREAD,&cuanto);
         if (cuanto <= 0)
         {
			 break;
		 }
		 if (cuanto < sizeof(infor))
		 {
			 Sleep(100);
			 continue;
		 }
#ifndef WIN32
			   errno = 0;
#endif
		 if(recv(s,&infor,sizeof(infor),MSG_WAITALL) != sizeof(infor))
		 {
#ifndef WIN32
			 if (errno == EINTR)
			 {
				 continue;
			 }
#endif
			 /* mal royo .. */
			 Sleep(100);
			 continue;
		 }
#ifndef WIN32
			   errno = 0;
#endif
		 if (infor.id == 0x172)
		 {
			 if (ventana_avisos)
			 {
				 PostMessage(ventana_avisos,WM_COMMAND,ID_TICSCON|0x20000,GetTickCount()-infor.tick);
			 }
			 if(recv(s,tmp2,infor.size,MSG_WAITALL) != infor.size)
			 {
#ifndef WIN32
				 if (errno == EINTR)
				 {
					 continue;
				 }
#endif
				 /* mal royo .. */
				 Sleep(100);
				 continue;			 
			 }

			 reenvio = 0;
			 if (mensajes_globo_enviados && tiempo_globo > time(NULL))
			 {
				 if (strcmp(buffglobo,tmp2))
					 reenvio = 1;
				 else
				 {
					 GetWindowText(ventana_avisos,tmp,sizeof(tmp)-1);
					 if (strcmp(buffglobotarea,tmp))
						 reenvio = 1;
				 }				 
			 }

			 if (!mensajes_globo_enviados || tiempo_globo < time(NULL) || reenvio)
			 {
				 strcpy(buffglobo,tmp2);
				 if (!(mensajes_globo_enviados-reenvio))
				 {
 					 sprintf(mensaje_globo_netcon,"0\0017\001Activada ventana de mantenimiento %s.\001",buffglobo);
					 GetWindowText(ventana_avisos,buffglobotarea,sizeof(buffglobotarea)-1);
					 sprintf(mensaje_globo_netcon+strlen(mensaje_globo_netcon),"Es necesario que cierre la tarea: %s\001",buffglobotarea);
				 }
				 else if ((mensajes_globo_enviados-reenvio) == 1)
				 {
 					 sprintf(mensaje_globo_netcon,"0\0018\001Activada ventana de mantenimiento %s.\001",buffglobo);
					 GetWindowText(ventana_avisos,buffglobotarea,sizeof(buffglobotarea)-1);
					 sprintf(mensaje_globo_netcon+strlen(mensaje_globo_netcon),"En un minuto el sistema cerrará la tarea: %s\001",buffglobotarea);
				 }
				 else if ((mensajes_globo_enviados-reenvio) >= 2)
				 {
 					 sprintf(mensaje_globo_netcon,"0\0019\001Activada ventana de mantenimiento %s.\001",buffglobo);
					 GetWindowText(ventana_avisos,buffglobotarea,sizeof(buffglobotarea)-1);
					 sprintf(mensaje_globo_netcon+strlen(mensaje_globo_netcon),"Cerrando tarea: %s\001",buffglobotarea);
				 }				 
				 PostMessage(ventana_avisos,WM_COMMAND,ID_AVISOGLOBO,(LPARAM)mensaje_globo_netcon);
				 if (!reenvio)
				 {
					 mensajes_globo_enviados++;
					 if (mensajes_globo_enviados == 1)
						tiempo_globo = time(NULL) + 180;
					 else if (mensajes_globo_enviados == 2)
						tiempo_globo = time(NULL) + 60;
					 else if (mensajes_globo_enviados > 2)
					 {
						tiempo_globo = time(NULL) + 60;						
					 }
				 }
			 }		 
		 }
		 else if (infor.id == 0x171)
		 {
			 
			 if (ventana_avisos)
			 {
				 PostMessage(ventana_avisos,WM_COMMAND,ID_TICSCON|0x20000,GetTickCount()-infor.tick);
			 }			 
			 if (mensajes_globo_enviados)
			 {
				 mensajes_globo_enviados = 0;
				 mensaje_globo_netcon[0] = 0;
				 PostMessage(ventana_avisos,WM_COMMAND,ID_AVISOGLOBO,(LPARAM)mensaje_globo_netcon);
			 }
		 }


		 Sleep(1000);
		 infos.tick = GetTickCount();
		 if (send(s,&infos,sizeof(infos),0) == SOCKET_ERROR)
		 {
#ifndef WIN32
			 if (errno == EINTR)
			 {
				 continue;
			 }
#endif

			 break;
		 }		 
	} while(-1);

	closesocket(s);
	Sleep(100);
}

#endif


#ifdef FGVDOS
extern char UsuarioPedido[260];
unsigned long __direccion_nwtcon = 0L;

static char *ms_sitios[] =
{"Cookies",
 "AppData/Roaming/Microsoft/Windows/Cookies",
 "Configuración local/Archivos temporales de Internet",  
 "AppData/Roaming/Microsoft/Windows/Cookies/Low",
 "Configuración local/Datos de programa/Google/Chrome/User Data/Default",
 "AppData/Local/Google/Chrome/User Data/Default", 
 "Datos de programa/Apple Computer/Safari/Cookies",
 "AppData/Roaming/Apple Computer/Safari/Cookies",
 "Datos de programa/Mozilla/Firefox/Profiles/*.default",
 "AppData/Roaming/Mozilla/Firefox/Profiles/*.default"
 "Datos de programa/Opera/Opera/profile",
 "AppData/Roaming/Opera/Opera/profile",
 "AppData/Local/Opera/Opera/profile",
 NULL
};
static char *ms_mascaras[] = 
{"*geconet*",
 "*ds*",
 "cookies*",
 "*",
  NULL
};

static int hay_una_geco_cookie(unsigned long direccion_nwtcon,char *usuario,FILE *flog)
{
	char tmp[1024];
	char clave[512];
	char diru[512];
	char **_dir;
	char *_diru = getenv("USERPROFILE");
	char *_test = getenv("TESTDSDS");
	int i,xi,xj;
	struct in_addr in;

	if (flog)
		fprintf(flog,"->*1\r\n");
	if (_test)
		return 1;
	if (!_diru)
	{
		if (flog)
			fprintf(flog,"->**1\r\n");
		aviso(0,"No puedo determinar directorio de configuracion");
		if (flog)
			fprintf(flog,"->***\r\n");
		return 1; /* dejamos pasar ... evitamos males mayores */
	}
	if (flog)
		fprintf(flog,"->*2\r\n");
	strcpy(diru,_diru);

	if (flog)
		fprintf(flog,"->*3\r\n");
	if (!usuario)
		usuario = UsuarioPedido;
	if (flog)
		fprintf(flog,"->*4\r\n");
	in.S_un.S_addr = direccion_nwtcon;
	sprintf(clave,"%s_%s_%s",inet_ntoa(in),dir_solicitado,usuario);
	if (flog)
		fprintf(flog,"->*5\r\n");

	for (xi = 0;ms_sitios[xi];xi++)
	{
		if (strstr(ms_sitios[xi],"*")) {
			sprintf(tmp,"%s/%s",diru,ms_sitios[xi]);
			_dir = lee_direct(tmp,2);
			if (_dir) {
				if (_dir[0]) {
					strcpy(tmp,ms_sitios[xi]);
					strcpy(quita_path(tmp),_dir[0]);
					ms_sitios[xi] = malloc(strlen(tmp)+1);
					strcpy(ms_sitios[xi],tmp);
				}
				libera_dir(_dir);
			}
		}
		for (xj = 0;ms_mascaras[xj];xj++)
		{
			if (flog)
				fprintf(flog,"->*!1\r\n");

			sprintf(tmp,"%s/%s/%s",diru,ms_sitios[xi],ms_mascaras[xj]);
			if (flog)
				fprintf(flog,"->*!2\r\n");
			_dir = lee_direct(tmp,0);
			if (_dir)
			{	
				if (flog)
					fprintf(flog,"->*!3\r\n");
				for (i = 0;_dir[i];i++)
				{
					if (flog)
						fprintf(flog,"->*!*1\r\n");
					sprintf(tmp,"%s/%s/%s",diru,ms_sitios[xi],_dir[i]);
					if (flog)
						fprintf(flog,"->*!*2\r\n");
					if (localizaenfich(tmp,clave) >= 0)
					{
						if (flog)
							fprintf(flog,"->*!**\r\n");
						libera_dir(_dir);
						return 1;
					}
					if (flog)
						fprintf(flog,"->*!*3\r\n");
				}
				libera_dir(_dir);
				if (flog)
					fprintf(flog,"->*!4\r\n");
			}			
			if (flog)
				fprintf(flog,"->*!!\r\n");
		}
	}
	if (flog)
		fprintf(flog,"->**\r\n");
	return 0;
}
#endif


/************************************************************************/
/*** COLA DE MENSAJES DE CONTROL FUERA DE SINCRONIA DESDE EL SERVIDOR ***/
/************************************************************************/

/* esto para futura ampliacion con id unica de transaccion 
typedef struct _COLAMSG
{
   MSGHDR          hdr;
   BYTE            *data;
   void			   *anterior;
   void			   *siguiente;
} COLAMSG;
static COLAMSG *colamsg_primero = NULL;
static COLAMSG *colamsg_segundo = NULL;
*/


static DATAMSG2 *acumulador_colaserver[256];


int busca_en_cola_server(char *pdata,int len,int waitfor)
{
	DATAMSG2 *uno;
	int datalen;
	if (waitfor < 0 || waitfor > 255)
	{
		aviso(0,"error waitfor");
		return -1;
	}
	if (!acumulador_colaserver[waitfor])
	{
		if (!waitfor || waitfor == 1)
		{
			if (acumulador_colaserver[99])
				waitfor = 99;
			else
				return -1;
		}
		else
			return -1;
	}

	uno = acumulador_colaserver[waitfor];
	acumulador_colaserver[waitfor] = NULL;

	datalen = uno->hdr.length-HDRSIZE-1;
	if (datalen < len)
	{
		aviso(0,"error datalen");
	}
	if (len > 0 && pdata)
	{
		memcpy(pdata,uno->data+1,len);
	}
	memcpy(xrcbbuf.data,uno->data,datalen+1);
	memcpy(&(xrcbbuf.hdr),&(uno->hdr),sizeof(MSGHDR));

	free((char *)(uno->data));
	free((char *)uno);

	return datalen;
}

int pon_en_cola_server()
{
	DATAMSG2 *uno;
	unsigned int indice;


	indice = (unsigned int)(unsigned char)xrcbbuf.data[0];
	uno = malloc(sizeof(DATAMSG2));
	memcpy(&(uno->hdr), &(xrcbbuf.hdr),sizeof(MSGHDR));
	uno->data = malloc(xrcbbuf.hdr.length-HDRSIZE);
	memcpy(uno->data,xrcbbuf.data,xrcbbuf.hdr.length-HDRSIZE);	

	if (acumulador_colaserver[indice])
	{
		free((void *)acumulador_colaserver[indice]);
		aviso(0,"Mensaje duplicado");
	}
	acumulador_colaserver[indice] = uno;
	
	return indice;
}


int pon_en_cola_server2()
{
	DATAMSG2 *uno;
	unsigned int indice;


	indice = (unsigned int)(unsigned char)xrcbbuf2.data[0];
	uno = malloc(sizeof(DATAMSG2));
	memcpy(&(uno->hdr), &(xrcbbuf2.hdr),sizeof(MSGHDR));
	uno->data = malloc(xrcbbuf2.hdr.length-HDRSIZE);
	memcpy(uno->data,xrcbbuf2.data,xrcbbuf.hdr.length-HDRSIZE);	

	if (acumulador_colaserver[indice])
	{
		free((void *)acumulador_colaserver[indice]);
		aviso(0,"Mensaje duplicado");
	}
	acumulador_colaserver[indice] = uno;
	
	return indice;
}

/************************************************************************/


int establece_conexion(modo)
int modo;
{
   int saltos_netcon = 0;
   int j,k,i,kki,pid,tmerr;
   int debe_actualizar;
   int modo_reconexion = 0;
   int modo_recuperacion = 0;
   char tmp[256];
   char rwn_sol[128];
   char linea_de_reenvio[256];
   unsigned long direccion_nwtcon = 0L;
   unsigned char *pdata;
   union agi_in_addr S_un;
   char certificado[512];
   int certificado_enviado;
   struct linger {  
						int    l_onoff; 
                  int    l_linger; 
   } lingo;
   struct hostent *mihost;
#ifdef FGVDOS
   STARTUPINFO stinfo;
   PROCESS_INFORMATION proi;
#ifndef WATCOMNLM
   WORD      wVersionRequested;
   WSADATA   wsaData;
#endif
#endif   
   
   if (server_request[0] != '@') return(-1); /* no hay que conectar */
   
   if (dsproxy) {
	   char *salvap = NULL;
	   unsigned short port = htons(1080);

	   memset((char *)&_ds_proxy,0,sizeof(_ds_proxy));
       j = 0;  
       while(dsproxy[j])
       {
          if (dsproxy[j] == ':')
          {
			  port = htons(atoi(dsproxy+j+1));
			  salvap = dsproxy + j;
			  *salvap = 0;
          }
		  j++;
	   }
	   _ds_proxy.sin_addr.s_addr = inet_addr(dsproxy); 
	   if (_ds_proxy.sin_addr.s_addr == INADDR_NONE)
	   {
		  if ( (mihost = gethostbyname(dsproxy)) && mihost->h_length == 4 && mihost->h_addr_list[0])
		  {	    
			 _ds_proxy.sin_addr.s_addr = *((unsigned long *)mihost->h_addr_list[0]);
		  }
	   }
	   if (salvap)
		   *salvap = ':';
   
	   _ds_proxy.sin_family = AF_INET;
	   _ds_proxy.sin_port = port;
        
	   ds_proxy = &_ds_proxy;
   }


   if (modo < 0)
   {
						modo_recuperacion = 1;
                  modo = 0;
   }
   
   dir_solicitado[0] = 0;
#ifdef RWSERVER
   if (en_conexion_externa) return(-1); /* ya conectado como servidor */
#endif
   
   if (modo) return(-1); /* no usar servidor de indices de momento */
   
   
   cambia_s_req(server_request,1); 
   
   if (!memcmp(server_request+1,"(C:",3))
   {
#ifndef DSMINIBASICO   
#ifdef FGVSYS
						controla_paqueteria(0); /* instalar paquetes locales antes de conectar!!! */
#endif
#endif
                  if (server_request[4] == '?')
                  {	
                     for (i = 5;server_request[i] && server_request[i] != ')';i++);
                     strcpy(tmp,server_request+i);
                     server_request[i] = 0;
                     strcpy(server_request+4,server_request+5);
                     pprint(2401,__Texto[107]);
                     entra(2433,16,"","",server_request+4,0,server_request+4);
                     quita_blancos(server_request+4);
                     strcat(server_request+4,tmp);
                  }
                  remoto = 1;  
                  sprintf(tmp,__Texto[108],server_request+4);
                  mensaje(0,tmp);  
                  if (_pon_incidencia)
                  {
                     sprintf(tmp,__Texto[109],server_request+4);
                     (*_pon_incidencia)(0,tmp);
                  }
                  modo_reconexion = 0;
   }
   else	if (!memcmp(server_request+1,"(/:",3))
   {  
		 remoto = 1;  
		 sprintf(tmp,__Texto[110],server_request+4);
		 mensaje(0,tmp);  
		 if (_pon_incidencia)
		 {
			sprintf(tmp,__Texto[111],server_request+4);
			(*_pon_incidencia)(0,tmp);
		 }
		 modo_reconexion = 1;
		 server_request[2] = 'C';
   }
   else if (!memcmp(server_request+1,"(S:",3))
   {  
         remoto = 0;  
         modo_reconexion = 0;
   }
   else if (!memcmp(server_request+1,"(-:",3))
   {  
         remoto = 0;  
         modo_reconexion = 1;
         server_request[2] = 'S';
   }
   else
   {
       sprintf(tmp,__Texto[112],server_request);
       aviso(0,tmp);
       return(-1);
   }

   if (!modo_recuperacion && remoto)
   {
	   dame_ethernet_address(la_ethernet_);
   }

    if (!time(NULL))
    {
       aviso(0,__Texto[106]);
       return(2);
    }
                        
#ifdef FGVDOS
	
    wVersionRequested = MAKEWORD(1,1);
    if (WSAStartup(wVersionRequested, &wsaData) != 0)
    {
       aviso(0,__Texto[113]);
       return -1;
    }
	
#endif
                        
        
    /*
    if (!xferbuf.data)
    {  
    xferbuf.data = malloc(MAXRDATA);
    }
    if (!xrcbbuf.data)
    {
    xrcbbuf.data = malloc(MAXRDATA);
    }
    */
    if (!gzip_buffer)
    {
       gzip_buffer = malloc(rwmax_buffer*2);
    }
    if (!rwsend_buffer)
    {
       rwsend_buffer = malloc(rwmax_buffer*2);
    }
    if (!rwrcv_buffer)
    {
       rwrcv_buffer = malloc(rwmax_buffer*2);
       punt_rcvb = rwrcv_buffer;
    }
    
    MySock.sock = INVALID_HANDLE_VALUE;
    if (rwsock != INVALID_HANDLE_VALUE)
    {
       closesocket(rwsock);
    }
    rwsock = INVALID_HANDLE_VALUE;
    
    
    linea_de_reenvio[0] = 0;
                        if (server_request[4] != '-')
                        {
                           /* primero buscar a rwnetcon */
                           strcpy(lpServName,server_request+4);
                           j = 0;  
                           while(lpServName[j])
                           {
                              if (lpServName[j] == ':')
                              {
                                 lpServName[j] = 0;
                                 i = j + 1;
                                 while(lpServName[i])
                                 {
                                    if (lpServName[i] == ',')
                                    {
                                       strcpy(linea_de_reenvio,lpServName+i);
                                       lpServName[i] = ')';		  
                                    }
                                    if (lpServName[i] == ')')
                                    {		  
                                       break;
                                    }
                                    i++;
                                 }
                                 if (lpServName[i] == ')')
                                 {
                                    lpServName[i] = 0;									
                                    PORTRWNETCON2 = (unsigned short)atoi(lpServName+j+1);
                                    lpServName[i] = ')';
                                 }
                                 else
								 {
                                    PORTRWNETCON2 = (unsigned short)atoi(lpServName+j+1);
								 }
								 if (!remoto)
								 {
									 sprintf(server_request+4+j,":%d%s",PORTRWNETCON,lpServName+i);
								 }
                                 j = i;	  
                              }
                              if (lpServName[j] == ',')
                              {
                                 strcpy(linea_de_reenvio,lpServName+j);
                                 lpServName[j] = 0;
                                 i = j + 1;
                                 while(lpServName[i])
                                 {
                                    if (lpServName[i] == ')')
                                       break;
                                    i++;
                                 }
                                 j = i;
                              }
                              
                              if (lpServName[j] == ')')
                              {
                                 lpServName[j] = 0;
                                 if (lpServName[j+1])
                                 {
                                    if (lpServName[j+1] == 'H')
                                    {
                                       _usando_compresion = 9;
                                    }
                                    else
                                       if (lpServName[j+1] == 'M')
                                       {
                                          _usando_compresion = 6;
                                       }
                                       else
                                          if (lpServName[j+1] == 'L')
                                          {
                                             _usando_compresion = 1;
                                          }	 
                                          else
                                          {
                                             _usando_compresion = 0;
                                          }
                                          strcpy(dir_solicitado,lpServName+j+2);
                                 }
                                 break;
                              }
                              j++;
                           }
                           
                           quita_blancos(linea_de_reenvio);
                           for (i = 0;linea_de_reenvio[i];i++)
                           {
                              if (linea_de_reenvio[i] == ')') {linea_de_reenvio[i] = 0;break;}
                           }

						   certificado[0] = 0;
						   certificado_enviado = 0;

						   do
						   {                           
							   if((rwsock =  socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) == SOCKET_ERROR)
							   {
								  if (remoto)
								  { 
									 aviso(0,__Texto[114]);
								  }
								  else
									 printf(__Texto[115],errno);
#ifdef FGVDOS
								WSACleanup();
#endif
								  libera_x_buffers();
								  return (2);
							   }                           
							   
							   
							   i = 1;
							   if (setsockopt(rwsock,IPPROTO_TCP,TCP_NODELAY,&i,sizeof(i)) < 0)
							   {
								  i = 1;
								  memcpy(tmp,&i,sizeof(int));
								  setsockopt(rwsock,IPPROTO_TCP,TCP_NODELAY,tmp,sizeof(tmp));
							   }
							   
/*
#ifdef NTFACTOR
  i = 600 * NTFACTOR;
  setsockopt(rwsock,SOL_SOCKET,SO_SNDTIMEO,&i,sizeof(i));
  i = 600 * NTFACTOR;
  setsockopt(rwsock,SOL_SOCKET,SO_RCVTIMEO,&i,sizeof(i));
#else
  tmval.tv_sec = 600;
  tmval.tv_usec = 0;
  setsockopt(rwsock,SOL_SOCKET,SO_SNDTIMEO,&tmval,sizeof(tmval));
  setsockopt(rwsock,SOL_SOCKET,SO_RCVTIMEO,&tmval,sizeof(tmval));
#endif
*/
                           
							   if (!lpServName[0])
							   {
								  gethostname(lpServName,sizeof(lpServName));
							   }						   
							   rwnetcon.sin_addr.s_addr = inet_addr(lpServName);
							   if (rwnetcon.sin_addr.s_addr == INADDR_NONE)
							   {
								  if ( (mihost = gethostbyname(lpServName)) && mihost->h_length == 4 && mihost->h_addr_list[0])
								  {	    
									 rwnetcon.sin_addr.s_addr = *((unsigned long *)mihost->h_addr_list[0]);
								  }
							   }
							   direccion_nwtcon = rwnetcon.sin_addr.s_addr;
#ifdef FGVDOS
							   __direccion_nwtcon = direccion_nwtcon;
#endif
                           
							   rwnetcon.sin_family = AF_INET;
							   rwnetcon.sin_port = PORTRWNETCON2;
							   moto_short(&(rwnetcon.sin_port));

#ifdef FGVDOS
							   if (_modo_portal && remoto)
							   {
								   /* antes de conectar vamos a comprobar si se autentifico en el portal */
								   if (!hay_una_geco_cookie(direccion_nwtcon,NULL,NULL))
								   {										  
										  aviso(0,"No dispone de acceso registrado para este servidor");
										  closesocket(rwsock);
										  rwsock = INVALID_HANDLE_VALUE;
			#ifdef FGVDOS
										  WSACleanup();
			#endif
										  libera_x_buffers();
										  return(2);
								   }
							   }
#endif

							   if (ds_connect_(rwsock,(struct sockaddr *) &rwnetcon,sizeof (rwnetcon)) == SOCKET_ERROR)
							   {   
								  sprintf(tmp,__Texto[385],lpServName,PORTRWNETCON2);
								  if (remoto)
								  {	 
									 aviso(0,tmp);     
								  }
								  else
								  {								  
									 printf("%s\r\n",tmp);								 
								  }
								  closesocket(rwsock);
								  rwsock = INVALID_HANDLE_VALUE;
	#ifdef FGVDOS
								  WSACleanup();
	#endif
								  libera_x_buffers();
								  return(2);
							   }
                           
							   if (remoto)
							   {
								 struct hostent *dhost;
								 if (rwsockhost[0] && (dhost = gethostbyname(rwsockhost)) && dhost->h_length == 4 && dhost->h_addr_list[0])
								 {	    
									rwsockaddr = *((unsigned long *)dhost->h_addr_list[0]);
								 }
								 sprintf(tmp,__Texto[212],server_request+4);
								 mensaje(0,tmp);  	
							   }
                           
	#ifdef WATCOMNLM
							   pid = GetNLMID();  
	#else
	#ifdef FGVDOS
							   pid = GetCurrentProcessId();
	#else
							   pid = getpid();
	#endif
	#endif
							   if (!remoto && !server_id)
							   {
								  server_id = pid;
								  if (!server_id) server_id++;
							   }
                           
                           
							   xferbuf.hdr.signature = RWNIDENTIFICACION;                 
							   rwn_sol[0] = 0;
							   if (remoto && !modo_recuperacion && dir_solicitado[0] == '[')
							   {
								   int kki;
								   xferbuf.hdr.command = RWNETCON_REQUERIDO;
								   for (kki = 1;dir_solicitado[kki] && dir_solicitado[kki] != ']';kki++);
								   dir_solicitado[0] = 0;
								   strcpy(rwn_sol,dir_solicitado+1);								   
								   if (dir_solicitado[kki])
								   {
									   strcpy(dir_solicitado,dir_solicitado+kki+1);
									   rwn_sol[kki-1] = 0;
								   }
							   } else if (!remoto)
							   {
								  if (modo_recuperacion)
								  {
									 xferbuf.hdr.command = SERVIDOR_INTERRUMPIDO;
								  }
								  else
									 if (modo_reconexion)
									 {
										xferbuf.hdr.command = RECONECTANDO_SERVIDOR;
									 }
									 else
									 {
										xferbuf.hdr.command = INICIANDO_SERVIDOR;
									 }
							   }
							   else
							   {
								  if (modo_reconexion)
								  {
									 xferbuf.hdr.command = RECONECTANDO_CLIENTE;
								  }
								  else
								  {
									 xferbuf.hdr.command = SERVIDOR_REQUERIDO;       
								  }
							   }
                           
                           
							   if (xferbuf.hdr.command != SERVIDOR_INTERRUMPIDO)
							   {
								  MySock.addrlen = sizeof(MySock.raddr);
								  getsockname(rwsock,&(MySock.raddr),&(MySock.addrlen));
							   }
                           
							   if (xferbuf.hdr.command == RWNETCON_REQUERIDO)
							   {
								    xferbuf.hdr.length = HDRSIZE + strlen(rwn_sol)+1;
									strcpy(xferbuf.data,rwn_sol);
							   }
							   else
							   {
									xferbuf.hdr.length = HDRSIZE + sizeof(int)*3 + sizeof(MySock.raddr.sin_addr.s_addr) + strlen(linea_de_reenvio)+1;
								    i = 0;
								    memcpy_int(xferbuf.data,&i,sizeof(int));
								    i = sizeof(int);
								    memcpy_int(xferbuf.data+i,&server_id,sizeof(int));
								    i += sizeof(int);
								    memcpy(xferbuf.data+i,&(MySock.raddr.sin_addr.s_addr),sizeof(MySock.raddr.sin_addr.s_addr));
								    i += sizeof(MySock.raddr.sin_addr.s_addr);
								    strcpy(xferbuf.data+i,linea_de_reenvio);
								    i += (strlen(xferbuf.data+i)+1);
								    memcpy_int(xferbuf.data+i,&pid,sizeof(int));
									/*if (xferbuf.hdr.command == SERVIDOR_REQUERIDO)*/
									{
										i += sizeof(int);
										memcpy(xferbuf.data+i,la_ethernet_,6);
										xferbuf.hdr.length += 6;
										i += 6;
										memcpy(xferbuf.data+i,&(rwnetcon.sin_addr.s_addr),sizeof(rwnetcon.sin_addr.s_addr));
										xferbuf.hdr.length += sizeof(rwnetcon.sin_addr.s_addr);
										i += sizeof(rwnetcon.sin_addr.s_addr);
										xferbuf.data[i] = rtme_version1;
										i++;
										xferbuf.hdr.length++;
										xferbuf.data[i] = rtme_version2;
										i++;
										xferbuf.hdr.length++;
										xferbuf.data[i] = rtme_versionletra;
										i++;
										xferbuf.hdr.length++;
										xferbuf.data[i] = que_sistema_es;
										i++;
										xferbuf.hdr.length++;
										xferbuf.data[i] = certificado_enviado;
										i++;
										xferbuf.hdr.length++;
										if (certificado[0])
										{
											xferbuf.data[i] = strlen(certificado)+1;
											i++;
											xferbuf.hdr.length++;
											strcpy(xferbuf.data+i,certificado);
											i += (strlen(certificado)+1);
											xferbuf.hdr.length += (strlen(certificado)+1);
											certificado[0] = 0;
										}
										else
										{
											xferbuf.data[i] = 0;
											i++;
											xferbuf.hdr.length++;
										}
										/* seccion parametros opcionales del terminal */
										xferbuf.data[i] = strlen(dir_solicitado)+1;
										i++;
										xferbuf.hdr.length++;
										strcpy(xferbuf.data+i,dir_solicitado);
										i += (strlen(dir_solicitado)+1);
										xferbuf.hdr.length += (strlen(dir_solicitado)+1);

										xferbuf.data[i] = 0;
										i++;
										xferbuf.hdr.length++;
										*((int *)rwsockc) = i;
										memcpy(rwsockc+sizeof(int),xferbuf.data,i);
									}									
							   }
                           
							   /*
							   {
								   char tmp7[512];

								   memset(tmp7,0,sizeof(tmp7));
								   tmp7[0] = RWNIDENTIFICACION;                 
								   tmp7[1] = SERVIDOR_REQUERIDO;
								   *((short *)(tmp7+2)) = 15;//HDRSIZE
							       send(rwsock,(char *)tmp7,32,0);
							   }
							   */							   
							   
							   if (senddatamessage (rwsock,&xferbuf) == FALSE)
							   {
								  if (remoto)
									 aviso(0,__Texto[116]);
								  else
									 printf(__Texto[386]);
								  closesocket(rwsock);
								  rwsock = INVALID_HANDLE_VALUE;
#ifdef FGVDOS
								  WSACleanup();
#endif
								  libera_x_buffers();
								  return(2);
							   } 
							   
                           
							   
							   if (recvdatamessage(rwsock,&xferbuf) == FALSE || (xferbuf.hdr.command != DENEGADO_SERVICIO && 
								  xferbuf.hdr.command != ACEPTADO_SERVICIO && xferbuf.hdr.command != RWNETCON_REQUERIDO	&&
								  xferbuf.hdr.command != RWNETCON_CERTIFICADO) )
							   {
								  if (remoto)
									 aviso(0,__Texto[117]);
								  else
									 printf(__Texto[118]);
								  closesocket(rwsock);
								  rwsock = INVALID_HANDLE_VALUE;
#ifdef FGVDOS
								  WSACleanup();
#endif
								  libera_x_buffers();
								  return(2);
							   }
							   if (xferbuf.hdr.command == RWNETCON_CERTIFICADO)
							   {								   
								   /* buscar certificado */
#ifdef WIN32
								   int ii;

								   ii =  xferbuf.data[0] + 1;								   
								   ds_busca_certificado(xferbuf.data+ii,certificado);
								   if (!certificado[0] && ii > 1)
								   {
									   strcpy(lpServName,xferbuf.data+1);
								   }
#endif
								   certificado_enviado = 1;
								   closesocket(rwsock);
								   rwsock = INVALID_HANDLE_VALUE;								   
								   continue;
							   }

							   if (xferbuf.hdr.command == RWNETCON_REQUERIDO)
							   {
								   if (saltos_netcon > 1)
								   {
									  aviso(0,"Sin permiso en la tabla de accesos geconetmac.srv.");
									  closesocket(rwsock);
									  rwsock = INVALID_HANDLE_VALUE;
#ifdef FGVDOS
									  WSACleanup();
#endif
									  libera_x_buffers();
									  return(2);
								   }
								   saltos_netcon++;
								   closesocket(rwsock);
								   rwsock = INVALID_HANDLE_VALUE;
								   strcpy(lpServName,xferbuf.data);
								   continue;								   						   
							   }                           
                           
							   if (xferbuf.hdr.command == DENEGADO_SERVICIO)
							   {
								  if (remoto)
									 aviso(0,__Texto[119]);
								  else
									 printf(__Texto[120]);
								  closesocket(rwsock);
								  rwsock = INVALID_HANDLE_VALUE;
#ifdef FGVDOS
								  WSACleanup();
#endif
								  libera_x_buffers();
								  return(2);
							   }
                           
							   break;
						   } while(-1);
                           
                           if (remoto)
                           {
                              memcpy_int(&server_id,xferbuf.data,sizeof(int));
                              strcpy(lpServName,xferbuf.data+sizeof(int));
                           }
                           else
                           {
                              strcpy(lpServName,xferbuf.data);
                           }
                           
                           if (remoto)
                           {
                              /*i = sizeof(rwudpa);
                              getsockname(rwsock,&(rwudpa),&i);*/							  
#ifdef FGVWIN 
							   /* Transferir socket a tarea de control */
							   /* solo windows! */
							  _beginthread(EstoyVivo,NULL,(void *)rwsock);	  
#else
                              closesocket(rwsock);                                
#endif
							  rwsock = INVALID_HANDLE_VALUE;
                           }  
  }
  else
  {
     strcpy(lpServName,server_request+5);
  }
  
  /* CONEXION CLIENTE-SERVIDOR DIRECTA */
  /* no switch de directorio ni compresion */
  j = 0;
  while(lpServName[j])
  {
     if (lpServName[j] == ':')
     {
        lpServName[j] = 0;
        i = j + 1;
        while(lpServName[i])
        {
           if (lpServName[i] == ')')
           {
              lpServName[i] = 0;
              break;
           }
           i++;
        }
        RWPORTIP = (unsigned short)atoi(lpServName+j+1);
        break;
     }
     if (lpServName[j] == ')')
     {
        lpServName[j] = 0;
        break;
     }
     j++;
  }

  
  MySock.type = SOCK_STREAM;
  MySock.protocol = IPPROTO_TCP;
  if((MySock.sock =  socket(AF_INET,MySock.type,MySock.protocol)) == SOCKET_ERROR)
  {
     if (remoto)
     { 
        aviso(0,__Texto[121]);
     }
     else
        printf(__Texto[122],errno);
#ifdef FGVDOS
     WSACleanup();
#endif
     libera_x_buffers();
     return(-1);
  }
  MySock.peer = INVALID_HANDLE_VALUE;
  j = 0;
  if (!strcmp(lpServName,"default"))
  {
	  MySock.addr.sin_addr.s_addr = direccion_nwtcon;
  }
  else
  {
	  MySock.addr.sin_addr.s_addr = inet_addr(lpServName);
	  if (MySock.addr.sin_addr.s_addr == INADDR_NONE)
	  {
		  MySock.addr.sin_addr.s_addr = direccion_nwtcon;
	  }
  }
  MySock.addr.sin_family = AF_INET;
  if (!remoto)
  {
	  if (!RWPORTIP)
	  {
	     RWPORTIP = htons(14535 + (time(NULL)%512));
	  }
	  else
	  {
		 maximoports = RWPORTIP;
		 RWPORTIP = htons(14535 + (time(NULL)%maximoports));
	  }
  }
  MySock.addr.sin_port = RWPORTIP;
  moto_short(&(MySock.addr.sin_port)); 

  if (remoto)
  {
	  RWPORTIP = ntohs(RWPORTIP);
	  strcpy(lpServName,inet_ntoa(MySock.addr.sin_addr));
      sprintf(tmp,__Texto[213],lpServName);
      mensaje(0,tmp);
  }

  
  if (!modo)
  {
#ifdef WATCOMNLM
     sprintf(MySock.name,"%ld",GetNLMID());
#else
#ifdef FGVDOS
     sprintf(MySock.name,"%ld",GetCurrentProcessId());
#else
     sprintf(MySock.name,"%ld",getpid());
#endif
#endif
  }
  else
  {
     sprintf(MySock.name,"%03d%03d(%s)",user_who,subnumero_de_usuario,rw_user);
  }
  
  if (!remoto)
  {
	 i = -1;
	 setsockopt(MySock.sock,SOL_SOCKET,SO_REUSEADDR,&i,sizeof(i));
#ifndef ESDOS
#ifdef SO_REUSEPORT
	 i = -1;
	 setsockopt(MySock.sock,SOL_SOCKET,SO_REUSEPORT,&i,sizeof(i));
#endif
#endif
  }
  
  i = 1;
  if (setsockopt(MySock.sock,IPPROTO_TCP,TCP_NODELAY,&i,sizeof(i)) < 0)
  {
     i = 1;
     memcpy(tmp,&i,sizeof(int));
     setsockopt(MySock.sock,IPPROTO_TCP,TCP_NODELAY,tmp,sizeof(tmp));
  }
  
  
  i = IPTOS_LOWDELAY;
  setsockopt(MySock.sock,IPPROTO_IP,IP_TOS,&i,sizeof(i));
  
  
  i = rwmax_buffer;
  setsockopt(MySock.sock,SOL_SOCKET,SO_RCVBUF,&i,sizeof(int));
  i = rwmax_buffer;
  setsockopt(MySock.sock,SOL_SOCKET,SO_SNDBUF,&i,sizeof(int));
  
  i = 1;
  setsockopt(MySock.sock,SOL_SOCKET,SO_OOBINLINE,&i,sizeof(i));
  
  
#ifdef NTFACTOR
  i = 600 * NTFACTOR;
  setsockopt(MySock.sock,SOL_SOCKET,SO_SNDTIMEO,&i,sizeof(i));
  i = 600 * NTFACTOR;
  setsockopt(MySock.sock,SOL_SOCKET,SO_RCVTIMEO,&i,sizeof(i));
#else
  tmval.tv_sec = 600;
  tmval.tv_usec = 0;
  setsockopt(MySock.sock,SOL_SOCKET,SO_SNDTIMEO,&tmval,sizeof(tmval));
  setsockopt(MySock.sock,SOL_SOCKET,SO_RCVTIMEO,&tmval,sizeof(tmval));
#endif
  
  
  lingo.l_onoff = 1;
  lingo.l_linger = 2;
  setsockopt(MySock.sock,SOL_SOCKET,SO_LINGER,&lingo,sizeof(lingo));

  
  if (remoto)
  {
     for (i = 0;i < 3;i++)
     {
        tmerr = ds_connect_(MySock.sock,(struct sockaddr *) &(MySock.addr),sizeof (MySock.addr));
        if (tmerr != SOCKET_ERROR) break;
     }  
     if (tmerr == SOCKET_ERROR)
     {
        if (remoto)
        {
           sprintf(tmp,__Texto[387],lpServName,ntohs(RWPORTIP));
           aviso(0,tmp);
        }
        closesocket(MySock.sock);
#ifdef FGVDOS
        WSACleanup();
#endif
        libera_x_buffers();
        return(2);
     }
     if (remoto)
     {
        sprintf(tmp,__Texto[214],lpServName);
        mensaje(0,tmp);

		MySock.raddr = MySock.addr;
        MySock.addrlen = sizeof(MySock.addr);
        getsockname(MySock.sock,&(MySock.addr),&(MySock.addrlen));
     }
  }
#ifdef RWSERVER
  else
  {
	 if (PORTRWNETCON2 != PORTRWNETCON)
	 {
	  	  PORTRWNETCON2 = PORTRWNETCON;
	 }
     MySock.addrlen = sizeof (MySock.addr);
     i = 1000;
     while(i)
     {
        i--;
        if (bind(MySock.sock,(struct sockaddr *) &(MySock.addr),sizeof (MySock.addr)) == SOCKET_ERROR)
        {
           if (rwsock != INVALID_HANDLE_VALUE)
           {
              if (!i)
              {  
                 xferbuf.hdr.signature = RWNIDENTIFICACION;                 
                 xferbuf.hdr.command = DENEGADO_SERVICIO;
                 xferbuf.hdr.length = HDRSIZE;
                 senddatamessage(rwsock,&xferbuf);
                 closesocket(rwsock);  
                 rwsock = INVALID_HANDLE_VALUE;
              }
              else
              {
				 MySock.addr.sin_port = htons(14535 + (rand()%maximoports));				 
				 if (MySock.addr.sin_port == RWPORTIP)
				 {
					 MySock.addr.sin_port = ntohs(RWPORTIP);
					 if (MySock.addr.sin_port <= 14535)
					 {
						 MySock.addr.sin_port++;
					 }
					 else
					 {
						 MySock.addr.sin_port--;
					 }
					 MySock.addr.sin_port = htons(MySock.addr.sin_port);
				 }
				 /*
                 if (!(MySock.addr.sin_port))
                 {
                    MySock.addr.sin_port = 51001 + rand() % 1024;					
					moto_short(&(MySock.addr.sin_port));
                 }
                 else
                 {
                    MySock.addr.sin_port--;
                 }
				 */
				 
                 RWPORTIP = MySock.addr.sin_port;				 
				 moto_short(&RWPORTIP);
                 continue;
              }
           }
           printf(__Texto[388],errno);
           closesocket(MySock.sock);
#ifdef FGVDOS
           WSACleanup();
#endif
           libera_x_buffers();
           return(2); /* exit !! */
        }
        break;
     }
     
     if (listen(MySock.sock, 1) == SOCKET_ERROR)
     {
        if (rwsock != INVALID_HANDLE_VALUE)
        {
           xferbuf.hdr.signature = RWNIDENTIFICACION;                 
           xferbuf.hdr.command = DENEGADO_SERVICIO;
           xferbuf.hdr.length = HDRSIZE;
           senddatamessage(rwsock,&xferbuf);
           closesocket(rwsock);  
           rwsock = INVALID_HANDLE_VALUE;
        }
        printf(__Texto[389]);
        closesocket(MySock.sock);
#ifdef FGVDOS
        WSACleanup();
#endif
        libera_x_buffers();
        return -1;
     }     
	 
     if (getsockname(MySock.sock,
        &(MySock.addr),
        &(MySock.addrlen)) == SOCKET_ERROR)
     {
        if (rwsock != INVALID_HANDLE_VALUE)
        {
           xferbuf.hdr.signature = RWNIDENTIFICACION;                 
           xferbuf.hdr.command = DENEGADO_SERVICIO;
           xferbuf.hdr.length = HDRSIZE;
           senddatamessage(rwsock,&xferbuf);
           closesocket(rwsock);  
           rwsock = INVALID_HANDLE_VALUE;
        }
        printf("Unable to getsockname:%d.\r\n",errno);
        closesocket(MySock.sock);
#ifdef FGVDOS
        WSACleanup();
#endif
        libera_x_buffers();
        return(2); /* exit !! */
     }
     
     RWPORTIP = MySock.addr.sin_port;
	 moto_short(&(RWPORTIP));
     S_un.S_addr = MySock.addr.sin_addr.s_addr;
     if (rwsock != INVALID_HANDLE_VALUE)
     {	
        sprintf(tmp,"%d.%d.%d.%d:%u",S_un.S_un_b.s_b1,S_un.S_un_b.s_b2,S_un.S_un_b.s_b3,
           S_un.S_un_b.s_b4,RWPORTIP);
        xferbuf.hdr.signature = RWNIDENTIFICACION;                 
        xferbuf.hdr.command = SERVIDOR_DISPONIBLE;
        xferbuf.hdr.length = HDRSIZE + strlen(tmp) + 1;
        strcpy(xferbuf.data,tmp);
        senddatamessage(rwsock,&xferbuf);
        closesocket(rwsock);
        rwsock = INVALID_HANDLE_VALUE;
     }  
     /*
     printf("Esperando Conexion en [%03d.%03d.%03d.%03d:%05u]\r\n",
     S_un.S_un_b.s_b1,S_un.S_un_b.s_b2,
     S_un.S_un_b.s_b3,S_un.S_un_b.s_b4,
     MySock.addr.sin_port);  
     */
#ifdef FGVUNIX
     fflush(stdout);
#endif
     MySock.peer = MySock.sock;
     MySock.raddrlen = sizeof(MySock.raddr);
     wait_agisys();
     MySock.sock = accept(MySock.peer,&(MySock.raddr),&(MySock.raddrlen));
     restore_agisys();
     if (MySock.peer != MySock.sock)
     {
        closesocket(MySock.peer);
        MySock.peer = INVALID_HANDLE_VALUE;
     }  
     if (MySock.sock == INVALID_HANDLE_VALUE)
     {	 
        printf(__Texto[390],errno);
        closesocket(MySock.sock);
#ifdef FGVDOS
        WSACleanup();
#endif
        libera_x_buffers();
        return(2); /* exit !! */
     }
     S_un.S_addr = MySock.raddr.sin_addr.s_addr;  
     /*
     printf("Conectado con [%03d.%03d.%03d.%03d:%05u]\r\n",
     S_un.S_un_b.s_b1,S_un.S_un_b.s_b2,
     S_un.S_un_b.s_b3,S_un.S_un_b.s_b4,
     MySock.raddr.sin_port);
     */
     i = 1;
     if (setsockopt(MySock.sock,IPPROTO_TCP,TCP_NODELAY,&i,sizeof(i)) < 0)
     {  
        i = 1;
        memcpy(tmp,&i,sizeof(int));
        setsockopt(MySock.sock,IPPROTO_TCP,TCP_NODELAY,tmp,sizeof(tmp));
     }    
     
	 
     i = IPTOS_LOWDELAY;
     setsockopt(MySock.sock,IPPROTO_IP,IP_TOS,&i,sizeof(i));
	 
     
     i = 1;
     setsockopt(MySock.sock,SOL_SOCKET,SO_OOBINLINE,&i,sizeof(i));
     

#ifdef NTFACTOR
	  i = 600 * NTFACTOR;
	  setsockopt(MySock.sock,SOL_SOCKET,SO_SNDTIMEO,&i,sizeof(i));
	  i = 600 * NTFACTOR;
	  setsockopt(MySock.sock,SOL_SOCKET,SO_RCVTIMEO,&i,sizeof(i));
#else
	  tmval.tv_sec = 600;
	  tmval.tv_usec = 0;
	  setsockopt(MySock.sock,SOL_SOCKET,SO_SNDTIMEO,&tmval,sizeof(tmval));
	  setsockopt(MySock.sock,SOL_SOCKET,SO_RCVTIMEO,&tmval,sizeof(tmval));
#endif

	  
	  i = -1;
	  i = setsockopt(MySock.sock,SOL_SOCKET,SO_KEEPALIVE,&i,sizeof(i));

     
#ifdef FGVUNIX
     fflush(stdout);
#endif
  }
#else
  else
  {
     aviso(0,__Texto[123]);
     closesocket(MySock.sock);
#ifdef FGVDOS
     WSACleanup();
#endif
     libera_x_buffers();
     return(-1);
  }
#endif
  
  
  if (!modo)
  {
     if (!remoto)
     {
#ifdef RWSERVER
        en_conexion_externa = 1;
        if (!modo_recuperacion && !ver_si_es_servidor())
        {
           xferbuf.hdr.signature = RWNIDENTIFICACION;                 
           xferbuf.hdr.command = DENEGADO_SERVICIO;
           xferbuf.hdr.length = HDRSIZE;
		   moto_short(&xferbuf.hdr.length);
           rwi_senddata(MySock.sock,(char *)&xferbuf,HDRSIZE);
		   moto_short(&xferbuf.hdr.length);
           flush_mysock();
           sleep(1);
           establece_desconexion(0);
           if (_pon_incidencia)
           {
              sprintf(tmp,__Texto[391],S_un.S_un_b.s_b1,S_un.S_un_b.s_b2,S_un.S_un_b.s_b3,S_un.S_un_b.s_b4);
              (*_pon_incidencia)(0,tmp);
           }
           /*printf(__Texto[391]);*/
           return(2);
        } 
        else
        {					  /* aceptar sesion */
           xferbuf.hdr.signature = RWNIDENTIFICACION;                 
           xferbuf.hdr.command = SESION_ACEPTADA;
           xferbuf.hdr.length = HDRSIZE + sizeof(int);
           if (_usando_compresion != -1)
           {				       
              usando_compresion = _usando_compresion;
           }
           memcpy_int(xferbuf.data,&usando_compresion,sizeof(int));		   
		   moto_short(&xferbuf.hdr.length);
           rwi_senddata(MySock.sock,(char *)&xferbuf,HDRSIZE + sizeof(int));
		   moto_short(&xferbuf.hdr.length);
           flush_mysock();
           identidad_cliente = S_un.S_un_b.s_b4 + S_un.S_un_b.s_b2/2 +
              (S_un.S_un_b.s_b1 + S_un.S_un_b.s_b2 + S_un.S_un_b.s_b3) / 6;
           if (_pon_incidencia)
           {
              sprintf(tmp,__Texto[392],S_un.S_un_b.s_b1,S_un.S_un_b.s_b2,S_un.S_un_b.s_b3,S_un.S_un_b.s_b4);
              (*_pon_incidencia)(0,tmp);
           }
        }        


        
        /* solicitar datos del tipo de terminal e informar del tipo de servidor */
        xferbuf.hdr.signature = RWNIDENTIFICACION;                 
        xferbuf.hdr.command = COMANDO_TERMINAL;                 
			  xferbuf.data[0] = 38;
              pdata = xferbuf.data + 1;
              *pdata = rtme_version1;
              pdata++;
              *pdata = rtme_version2;
              pdata++;
              *pdata = rtme_versionletra;
              pdata++;
              *pdata = que_sistema_es;
              pdata++;
			  if (Pseudo10)
			  {
				 *pdata = 2;
			  }
			  else
			  {
				 *pdata = es_sistema_grafico;
			  }
			  if (noboton_usuarios)
			  {
				  *pdata |= 4;
			  }
			  if (modosinbarras == 1)
			  {
				  *pdata |= 8;
			  }
			  if (modosinbarras == 2)
			  {
				  *pdata |= 16;
			  }
              if (xtabreturn)
			  {
				  *pdata |= 64;
			  }
              if (!busqueda_general)
			  {
				  *pdata |= 128;
			  }			  
			  *pdata |= 32; /*informacion extendida */
              pdata++;
              strcpy(pdata,rw_directorio(""));
              pdata += strlen(pdata);
              pdata++;
              strcpy(pdata,dirbass);
              pdata += strlen(pdata);
              pdata++;
              *pdata = numero_build;
              pdata++;
	          *pdata = sos_activado;
			  pdata++;
	          *pdata = conexion_lenta;
			  pdata++;
			  *pdata = sizeof(int)*2;
			  pdata++;
			  *((int *)pdata) = fgv_internal_build;
			  pdata += sizeof(int);
			  *((int *)pdata) = FGVID;
			  pdata += sizeof(int);
			  *pdata = 1;
			  pdata++;
              *((int *)pdata) = auto_completar;
			  pdata += sizeof(int);
			  *pdata = 1;
			  pdata++;
			  *((int *)pdata) = siempre_password;
			  pdata += sizeof(int);
			  *pdata = 1;
			  pdata++;
			  *((int *)pdata) = modosinbarras;
			  pdata += sizeof(int);
			  *pdata = 1;
			  pdata++;
			  *((int *)pdata) = defectoesc;
			  pdata += sizeof(int);
			  *pdata = 0;
			  pdata++;


              xferbuf.hdr.length = HDRSIZE + (int)(pdata - xferbuf.data);
			  {
				  int xxl = xferbuf.hdr.length;
				  moto_short(&xferbuf.hdr.length);
                  rwi_senddata(MySock.sock,(char *)&xferbuf,xxl);
				  moto_short(&xferbuf.hdr.length);
			  }
              if (responde_sesion() != CONTROL_DE_TERMINAL || (xrcbbuf.data[0] != 7 && xrcbbuf.data[0] != 20 && xrcbbuf.data[0] != 21))
              {
                 establece_desconexion(0);
                 printf(__Texto[393]);					
                 return(2);
              }
              if (xrcbbuf.data[0] != 7)
              {
                 while (xrcbbuf.data[0] != 21)
                 {
                    flush_mysock();
                    sprintf(tmp,"master/%s.tgz",xrcbbuf.data+1);
                    if (rw_envia_fichero(tmp,4))
                    {
                       break;
                    }
                    if (responde_sesion() != CONTROL_DE_TERMINAL) break;
                 }
                 sleep(10);
                 establece_desconexion(0);                      
                 return(2);
              }
              
              pdata = xrcbbuf.data +1;
              remoto_ver1 = *pdata;
              pdata++;
              remoto_ver2 = *pdata;
              pdata++;
              remoto_verletra  = *pdata;
              pdata++;
              remoto_que_sistema_es = *pdata;
              pdata++;
              remoto_es_sistema_grafico = *pdata;
			  if ((remoto_es_sistema_grafico & 32))
			  {
				  consulta_vieja = 1;
				  remoto_es_sistema_grafico &= ~32;
			  }
			  if ((remoto_es_sistema_grafico & 64))
			  {
				  isslowconection = 1;
				  remoto_es_sistema_grafico &= ~64;
			  }
			  if ((remoto_es_sistema_grafico & 16))
			  {
				  if (!entorno_antivirus)
				  {
					  entorno_antivirus = 1;
					  pon_en_ini("ENTORNO_ANTIVIRUS=1");            
					  graba_fgv(1);
				  }
				  remoto_es_sistema_grafico &= ~16;
			  }
			  if (remoto_es_sistema_grafico == 2)
			  {
				 remoto_es_sistema_grafico = 1;
			     Pseudo10 = 1;
			  }
              pdata++;
              memcpy_int(&redita_atrib,pdata,sizeof(int));
              pdata += sizeof(int);
              memcpy_int(&redita_cc,pdata,sizeof(int));
              pdata += sizeof(int);
              memcpy_int(&redita_cf,pdata,sizeof(int));
              pdata += sizeof(int);
              /* tiene que ser mayor que este mensaje */
              memcpy_int(&usando_compresion,pdata,sizeof(int));
              pdata += sizeof(int);
              if (!modo_recuperacion)
              {
                 if (*pdata)
                 {
                    strcpy(dir_solicitado,pdata);
                    pdata += strlen(pdata);
                 }
                 pdata++;                 
                 if (*pdata)
                 {
                    strcpy(par1,pdata);
                    if (quita_path(par1) == par1)
                       strcpy(nmen0,rw_directorio(par1));
                    else
                       strcpy(nmen0,par1);				   
                    pdata += strlen(pdata);
                    solouno = 0;
                 }
                 pdata++;
                 if (*pdata)
                 {
                    strcpy(dirbass,pdata);
                    strcpy(par2,pdata);
                    pdata += strlen(pdata);
                    solouno = 0;
                 }
                 pdata++;
				 if (*pdata == 6 && *(pdata+1) == 7 && *(pdata+2) == 8)
				 {
					 pdata += 3;
					 memcpy(la_ethernet_,pdata,6);
					 pdata += 7;/* + trailing 0 */
				 }				 
				 if (*pdata == 8 && *(pdata+1) == 6 && *(pdata+2) == 7)
				 {
					 pdata += 3;
					 strcpy(remoto_dirbass,pdata);					 
					 pdata += (strlen(pdata)+1);/* + trailing 0 */
				 }
				 remoto_build = *pdata;
				 pdata++;
				 if (*pdata == 9 && *(pdata+1) == 6 && *(pdata+2) == 7)
				 {
					 pdata += 3;
					 terminal_internal_build = *((int *)pdata);					 
					 pdata += sizeof(int);
				 }				 
				 if (*pdata == 7 && *(pdata+1) == 9 && *(pdata+2) == 7)
				 {
					 pdata += 3;
					 excel_alt = *((int *)pdata);					 
					 pdata += sizeof(int);
				 }				 
				 
				 if (dir_solicitado[0])
				 {
					sprintf(tmp,"%s/",dir_solicitado);
					if (strcmp(dir_solicitado,rw_directorio("")))
					{
					   libera_permisos();
					   final_agisys();
					   fin_sig();
					   Terfinal();					   
					   rw_setdirectorio(dir_solicitado);					   
					   Terinici();
					   ini_sig();					   
					   if (par1[0])
					   {
						  if (quita_path(par1) == par1)
							 strcpy(nmen0,rw_directorio(par1));
						  else
							 strcpy(nmen0,par1);
					   }
					   else
						  strcpy(nmen0,rw_directorio("ds"));
					   if (!par2[0])				 
						  strcpy(dirbass,rw_directorio(""));
					   inicio_agisys();
					   /*inicio_dir(); /* carga permisos generales */
					   /*
					   if (lee_prot() < 0)
					   {
						  xferbuf.hdr.signature = RWNIDENTIFICACION;
						  xferbuf.hdr.command = COMANDO_TERMINAL;
						  xferbuf.hdr.length = HDRSIZE + 1;
						  xferbuf.data[0] = 100;
						  senddatamessage(MySock.sock,&xferbuf);
						  flush_mysock();
						  sleep(2);
						  establece_desconexion(0);                          
						  return 2;
					   }
					   */

					  xferbuf.hdr.signature = RWNIDENTIFICACION;                 
					  xferbuf.hdr.command = COMANDO_TERMINAL;                 
					  xferbuf.data[0] = 38;
					  pdata = xferbuf.data + 1;
					  *pdata = rtme_version1;
					  pdata++;
					  *pdata = rtme_version2;
					  pdata++;
					  *pdata = rtme_versionletra;
					  pdata++;
					  *pdata = que_sistema_es;
					  pdata++;
					  if (Pseudo10)
					  {
						 *pdata = 2;
					  }
					  else
					  {
						 *pdata = es_sistema_grafico;
					  }
					  if (noboton_usuarios)
					  {
						  *pdata |= 4;
					  }
					  if (modosinbarras == 1)
					  {
						  *pdata |= 8;
					  }
					  if (modosinbarras == 2)
					  {
						  *pdata |= 16;
					  }
					  if (xtabreturn)
					  {
						  *pdata |= 64;
					  }
					  if (!busqueda_general)
					  {
						  *pdata |= 128;
					  }			  
					  *pdata |= 32; /*informacion extendida */
					  pdata++;
					  strcpy(pdata,rw_directorio(""));
					  pdata += strlen(pdata);
					  pdata++;
					  strcpy(pdata,dirbass);
					  pdata += strlen(pdata);
					  pdata++;
					  *pdata = numero_build;
					  pdata++;
					  *pdata = sos_activado;
					  pdata++;
					  *pdata = conexion_lenta;
					  pdata++;
					  *pdata = sizeof(int)*2;
					  pdata++;
					  *((int *)pdata) = fgv_internal_build;
					  pdata += sizeof(int);
					  *((int *)pdata) = FGVID;
					  pdata += sizeof(int);
					  *pdata = 1;
					  pdata++;
					  *((int *)pdata) = auto_completar;
					  pdata += sizeof(int);
					  *pdata = 1;
					  pdata++;
					  *((int *)pdata) = siempre_password;
					  pdata += sizeof(int);

					  *pdata = 1;
					  pdata++;
					  *((int *)pdata) = modosinbarras;
					  pdata += sizeof(int);

					  *pdata = 1;
					  pdata++;
					  *((int *)pdata) = defectoesc;
					  pdata += sizeof(int);

					  *pdata = 0;
					  pdata++;


					  xferbuf.hdr.length = HDRSIZE + (int)(pdata - xferbuf.data);
					  {
						  int xxl = xferbuf.hdr.length;
						  moto_short(&xferbuf.hdr.length);
						  rwi_senddata(MySock.sock,(char *)&xferbuf,xxl);
						  moto_short(&xferbuf.hdr.length);
					  }
					  if (responde_sesion() != CONTROL_DE_TERMINAL || (xrcbbuf.data[0] != 7 && xrcbbuf.data[0] != 20 && xrcbbuf.data[0] != 21))
					  {
						 establece_desconexion(0);
						 printf(__Texto[393]);					
						 return(2);
					  }
					  if (xrcbbuf.data[0] != 7)
					  {
						 while (xrcbbuf.data[0] != 21)
						 {
							flush_mysock();
							sprintf(tmp,"master/%s.tgz",xrcbbuf.data+1);
							if (rw_envia_fichero(tmp,4))
							{
							   break;
							}
							if (responde_sesion() != CONTROL_DE_TERMINAL) break;
						 }
						 sleep(10);
						 establece_desconexion(0);                      
						 return(2);
					  }
              
					  pdata = xrcbbuf.data +1;
					  remoto_ver1 = *pdata;
					  pdata++;
					  remoto_ver2 = *pdata;
					  pdata++;
					  remoto_verletra  = *pdata;
					  pdata++;
					  remoto_que_sistema_es = *pdata;
					  pdata++;
					  remoto_es_sistema_grafico = *pdata;
					  if ((remoto_es_sistema_grafico & 32))
					  {
						  consulta_vieja = 1;
						  remoto_es_sistema_grafico &= ~32;
					  }
					  if ((remoto_es_sistema_grafico & 64))
					  {
						  isslowconection = 1;
						  remoto_es_sistema_grafico &= ~64;
					  }
					  if ((remoto_es_sistema_grafico & 16))
					  {
						  if (!entorno_antivirus)
						  {
							  entorno_antivirus = 1;
							  pon_en_ini("ENTORNO_ANTIVIRUS=1");            
							  graba_fgv(1);
						  }
						  remoto_es_sistema_grafico &= ~16;
					  }
					  if (remoto_es_sistema_grafico == 2)
					  {
						 remoto_es_sistema_grafico = 1;
						 Pseudo10 = 1;
					  }
					  pdata++;
					  memcpy_int(&redita_atrib,pdata,sizeof(int));
					  pdata += sizeof(int);
					  memcpy_int(&redita_cc,pdata,sizeof(int));
					  pdata += sizeof(int);
					  memcpy_int(&redita_cf,pdata,sizeof(int));
					  pdata += sizeof(int);
					  /* tiene que ser mayor que este mensaje */
					  memcpy_int(&usando_compresion,pdata,sizeof(int));
					  pdata += sizeof(int);
					  if (!modo_recuperacion)
					  {
						 if (*pdata)
						 {
							strcpy(dir_solicitado,pdata);
							pdata += strlen(pdata);
						 }
						 pdata++;                 
						 if (*pdata)
						 {
							strcpy(par1,pdata);
							if (quita_path(par1) == par1)
							   strcpy(nmen0,rw_directorio(par1));
							else
							   strcpy(nmen0,par1);				   
							pdata += strlen(pdata);
							solouno = 0;
						 }
						 pdata++;
						 if (*pdata)
						 {
							strcpy(dirbass,pdata);
							strcpy(par2,pdata);
							pdata += strlen(pdata);
							solouno = 0;
						 }
						 pdata++;
						 if (*pdata == 6 && *(pdata+1) == 7 && *(pdata+2) == 8)
						 {
							 pdata += 3;
							 memcpy(la_ethernet_,pdata,6);
							 pdata += 7;/* + trailing 0 */
						 }				 
						 if (*pdata == 8 && *(pdata+1) == 6 && *(pdata+2) == 7)
						 {
							 pdata += 3;
							 strcpy(remoto_dirbass,pdata);					 
							 pdata += (strlen(pdata)+1);/* + trailing 0 */
						 }
						 remoto_build = *pdata;
						 pdata++;
						 if (*pdata == 9 && *(pdata+1) == 6 && *(pdata+2) == 7)
						 {
							 pdata += 3;
							 terminal_internal_build = *((int *)pdata);					 
							 pdata += sizeof(int);
						 }				 
						 
					  }
					}
				 }


				 if (con_tabla_macdir(la_ethernet_))
				 {
					 dir_solicitado[0] = 0;
				 }

				 if (!comprueba_ethernet(la_ethernet_))
				 {
					 aviso(0,"Terminal no incluido en el acceso a este servidor");
					 xferbuf.hdr.signature = RWNIDENTIFICACION;
					 xferbuf.hdr.command = COMANDO_TERMINAL;
					 xferbuf.hdr.length = HDRSIZE + 1;
					 xferbuf.data[0] = 101;
					 senddatamessage(MySock.sock,&xferbuf);
					 flush_mysock();
					 sleep(2);
					 establece_desconexion(0);
					 return 2;
				 }
              }

              default_redita_atrib = redita_atrib;
              default_redita_cc    = redita_cc;
              default_redita_cf    = redita_cf;
              
              if (pendiente_de_instalar)
              {					
                 xferbuf.hdr.signature = RWNIDENTIFICACION;
                 xferbuf.hdr.command = COMANDO_TERMINAL;
                 xferbuf.hdr.length = HDRSIZE + 1;
                 xferbuf.data[0] = 103;
                 senddatamessage(MySock.sock,&xferbuf);
                 flush_mysock();
                 sleep(2);
                 establece_desconexion(0);
                 if (ejecuta_instalacion() == -10)
					 exit(0);
              }
              
              debe_actualizar = 0;
              
              if (getenv("OBLIGADO_DS"))
              {
                 if ( soporta_ver1 > remoto_ver1 || (soporta_ver1 == remoto_ver1 && soporta_ver2 > remoto_ver2) ||
                    (soporta_letra && soporta_ver1 == remoto_ver1 && soporta_ver2 == remoto_ver2 && soporta_letra > remoto_verletra )
                    )
                    debe_actualizar = 1;
              }
              else
              {
                 if ( rtme_version1 != remoto_ver1 || (rtme_version1 == remoto_ver1 && rtme_version2 != remoto_ver2) ||	
                    (rtme_versionletra && rtme_version1 == remoto_ver1 && rtme_version2 == remoto_ver2 && rtme_versionletra != remoto_verletra ) ||
                    (rtme_versionletra && rtme_version1 == remoto_ver1 && rtme_version2 == remoto_ver2 && rtme_versionletra == remoto_verletra && min_puesto_build != remoto_build))
                    debe_actualizar = 1;
              }
              
              if (debe_actualizar)
              {
                 /* ver si podemos actualizarle y que vuelva a conectar!!! */				  
#ifdef FGVSYS
                 
                 /* buscar runtime adecuado */
                 tmp[0] = 0;
                 kki = 0;
                 if (remoto_que_sistema_es == 13)
                 {
                    /* no deberia pasar!!! */
                    strcpy(tmp,"NT");
                    kki = 'C';
                 }
                 else
                    if (remoto_que_sistema_es == 11)
                    {
                       strcpy(tmp,"NT");
                       kki = 'C';
                    }
                    else
                       if (remoto_que_sistema_es == 7)
                       {
                          strcpy(tmp,"BSD");
                          kki = 'C';
                       }
                       else
                          if (remoto_que_sistema_es == 23)
                          {
                             strcpy(tmp,"SCO");
                             kki = 'C';
                          }
                          if (remoto_ver1 >= 9)
                          {                 
                             for (i = 0;i < nPaq;i++)
                             {
                                if (Paq[i].i.tipo == kki && !strcmp(Paq[i].i.sistema,tmp))
                                   break;
                             }
                          }
                          else
                          {
                             i = nPaq;
                          }
                          if (no_instala_enbucla)
                          {
                             i = nPaq;
                          }
                          if (i < nPaq)
                          {
                             /* saldra en el terminal o eso espero */
                             /*mensaje(0,"LA VERSION DE SU CLIENTE ES INCORRECTA SE PROCEDE A LA ACTUALIZACION");*/
                             Bl(24);
                             if (confirmar(2400,__Texto[215],__Texto[448]) != 0)
                             {
                                Bl(24);
                                pprint(2401,__Texto[394]);
                                flush_mysock();
                                sleep(5);
                                establece_desconexion(0);
                                return(2);
                             }
                             flush_mysock();
                             sprintf(tmp,"master/instalar.exe");
                             rw_envia_fichero(tmp,4);
							 if (entorno_antivirus)
							 {
								 sprintf(tmp,"master/agitool.rwi");
								 rw_envia_fichero(tmp,4);
							 }
							 sprintf(tmp,"master/agitool.tgz");
							 rw_envia_fichero(tmp,4);
							 if (rtme_version2 == 10)
							 {
								 if (entorno_antivirus)
								 {
									 sprintf(tmp,"master/%s.rwi","diagramx");
									 rw_envia_fichero(tmp,4);
								 }
								 sprintf(tmp,"master/%s.tgz","diagramx");
								 rw_envia_fichero(tmp,4);
								 if (entorno_antivirus)
								 {
									 sprintf(tmp,"master/%s.rwi","diagram9");
									 rw_envia_fichero(tmp,4);
								 }
								 sprintf(tmp,"master/%s.tgz","diagram9");								 								 
							 }
							 else
							 {
								 if (entorno_antivirus)
								 {
									 sprintf(tmp,"master/%s.rwi",Paq[i].nompaq);
									 rw_envia_fichero(tmp,4);
								 }
							 	 sprintf(tmp,"master/%s.tgz",Paq[i].nompaq);
							 }
                             if (!rw_envia_fichero(tmp,4))
                             {
							    aviso(0,__Texto[124]);
                                xferbuf.hdr.signature = RWNIDENTIFICACION;
                                xferbuf.hdr.command = COMANDO_TERMINAL;
                                xferbuf.hdr.length = HDRSIZE + 1;
                                xferbuf.data[0] = 101; /* solicitar reinicio del terminal */
								{
									int xxl = xferbuf.hdr.length;
									moto_short(&xferbuf.hdr.length);
                                    rwi_senddata(MySock.sock,(char *)&xferbuf,xxl);
									moto_short(&xferbuf.hdr.length);
								}
                                responde_sesion();
                                sleep(5);
                                establece_desconexion(0);                      
                                return(2);
                             }
                          }
#endif				   
                          if (!time(NULL))
                          {
                             aviso(0,__Texto[125]);
                          }
                          else
                          {
                             aviso(0,__Texto[126]);
                          }
                          /***********************************/
                          xferbuf.hdr.signature = RWNIDENTIFICACION;
                          xferbuf.hdr.command = COMANDO_TERMINAL;
                          xferbuf.hdr.length = HDRSIZE + 1;
                          xferbuf.data[0] = 100;
                          senddatamessage(MySock.sock,&xferbuf);
                          flush_mysock();
                          sleep(2);
                          establece_desconexion(0);
                          printf(__Texto[395]);
                          return(2);
              }
              if (usando_compresion)
              {
                 gzip_asignada_memoria = 0;
              }
              
              
              xferbuf.hdr.signature = RWNIDENTIFICACION;                 
              xferbuf.hdr.command = COMANDO_TERMINAL;
              xferbuf.hdr.length = HDRSIZE + 1 + sizeof(long);
              xferbuf.data[0] = 59; /* activar (por defecto) sistema de mensajes directos */
			  memcpy(xferbuf.data+1,&nume_serie,sizeof(long));

              senddatamessage(MySock.sock, &xferbuf);


#ifdef FGVDOS
#ifndef FGVWIN
#ifndef WATCOMNLM
              
              if (myconsola != INVALID_HANDLE_VALUE)
              {  
                 fin_my_consola();
                 FreeConsole();
                 myconsola = INVALID_HANDLE_VALUE;
              }				 
              /*
              j = GetPriorityClass(GetCurrentProcess());
              i = GetThreadPriority(GetCurrentThread());
              if (j < NORMAL_PRIORITY_CLASS)
              SetPriorityClass(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);
              if (j <= NORMAL_PRIORITY_CLASS)
              SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_ABOVE_NORMAL);
              */
              SetPriorityClass(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);
              SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_BELOW_NORMAL);
#endif
#endif
#endif
#endif				  
              return(0);
              }
              else
              {               /* solicitar session */
                 en_conexion_terminal = 1;
                 if (responde_sesion() != SESION_ACEPTADA)
                 {
					sprintf(tmp,"CONEXION %s DENEGADA! 9(%d)",lpServName,xrcbbuf.hdr.command); 
                    /*sprintf(tmp,__Texto[127],lpServName);*/
                    aviso(0,tmp);
                    establece_desconexion(0);
                    return(2);
                 }
                 memcpy_int(&usando_compresion,xrcbbuf.data,sizeof(int));
                 sprintf(tmp,__Texto[128],lpServName);
                 mensaje(0,tmp);
                 
                 /* esperar mensaje de inicio de sesion */
#ifdef FGVDOS
#ifndef FGVWIN
                 SetPriorityClass(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);
                 SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_HIGHEST);
#endif
#endif
                 if (par1[0]) /* en el terminal se ignora el efecto de los parametros!! */
                 {          /* pues se pasan directos al servidor */
                    strcpy(nmen0,rw_directorio("ds"));
                 }
                 if (par2[0])
                 {
                    strcpy(dirbass,rw_directorio(""));
                 }
                 if (no_entres_en_terminal)
                 {
                    no_entres_en_terminal = 0;
                    return(0);
                 }
				 GrabaRegistroEntrada(2);
                 if ((i = modo_terminal()) == 1)
                 {
                    return(0); /* NO HAY MODO TERMINAL DISPONIBLE ... INTENTA COMO CLIENTE */
                 }
                 if (i == 3) return(3);
                 if (i == 4) return(4);
                 return(2);
              }
         }
#ifdef RWXARXAIDX
         else
         {
            xferbuf.hdr.signature = RWNIDENTIFICACION;
            xferbuf.hdr.command = REGISTER_NAME;                 
            xferbuf.hdr.length = strlen(MySock.name) + HDRSIZE + 1;
            strcpy(xferbuf.data, MySock.name);                  
            _en_conexion_server_ = 1;
            _en_conexion_index_ = 1;
            senddatamessage(MySock.sock, &xferbuf);
         }
#endif
         server_espera_control = 0;
         return(0);
}




int establece_desconexion(modo)
int modo;
{

  if (log_trafico_tcp)
  {
	 if (trafico_time)
	 {
		 pinta_log_trafico();
	 }
	 memset((char *)trafico_entrada,0,sizeof(trafico_entrada));
	 memset((char *)trafico_salida,0,sizeof(trafico_salida));
	 memset((char *)bytes_entrada,0,sizeof(bytes_entrada));
	 memset((char *)bytes_salida,0,sizeof(bytes_salida));
	 trafico_time = 0;
	 if (file_trafico)
	 {
		 fclose(file_trafico);
		 file_trafico = NULL;
	 }
  }

   
   if (usando_compresion && gzip_asignada_memoria != -1)
   {
      if (gzip_asignada_memoria > 0)
      {
         gzip_asignada_memoria = -1;
         ds___gzip(-1,-1,1,-1);
      }
      else
         gzip_asignada_memoria = -1;
   }
   
   if (!modo)
   {
#ifdef RWSERVER
      if (!en_conexion_externa && !en_conexion_terminal)
#else
         if (!en_conexion_terminal)
#endif
         {
            /* libera_x_buffers(); */
            return(-1);
         } 
   }
#ifdef RWXARXAIDX
   else
   {
      if (!_en_conexion_server_)
      {
         /* libera_x_buffers(); */
         return(-1);
      }
   }
   _en_conexion_server_ = 0;
   _en_conexion_index_ = 0;
#else
   else
   {
      /* libera_x_buffers(); */
      return(-1);
   }
#endif
   closesocket(MySock.sock);
#ifdef RWSERVER
   /*
   if (en_conexion_externa)
   {
   unlink(MySock.addr);
   }
   */
   if (en_conexion_externa && _pon_incidencia)
   {  
      (*_pon_incidencia)(0,"SERVIDOR OFF");
   }
   
   en_conexion_externa = 0;
#endif
   if (en_conexion_terminal && _pon_incidencia)
   {  
      (*_pon_incidencia)(0,"CLIENTE OFF");
   }
   en_conexion_terminal = 0;
   if (!remoto)
   {
      _imp = NULL;
   }
#ifdef FGVDOS
   WSACleanup();
#endif
   /* libera_x_buffers(); */
   return(0);
}



#ifdef RWXARXAIDX
int responde_xarxa(int id,DATAMSG *xfer)
{
   unsigned long cuanto;
   int err;
   if (!xfer) xfer = &xrcbbuf;
   
   while(-1)
   {      
      flush_mysock();
      if (!rwp_rcv_buffer)
      {
         FD_ZERO(&setread);
         FD_SET(MySock.sock,&setread);
         FD_ZERO(&seterror);
         FD_SET(MySock.sock,&seterror);
         err = 0;
#ifndef WIN32
			   errno = 0;
#endif
         if ((err = select(MySock.sock+1,&setread,NULL,&seterror,NULL)) == SOCKET_ERROR || FD_ISSET(MySock.sock,&seterror))
         {
#ifndef WIN32
			 if (err == SOCKET_ERROR && errno == EINTR)
			 {
				 continue;
			 }
#endif

            memset(xfer->data,0xff,MAXRDATA);
            establece_desconexion(1);
#ifdef FGVDOS
            MessageBox(NULL,"LA CONEXION CON EL SERVIDOR SE HA ROTO!!!!","ATENCION!",MB_OK);
#endif
            return(-1);
         }
         
         if (!err)
         {
#ifdef FGVDOS
            err = MessageBox(NULL,"Timeout en conexion con el Server","ATENCION!",MB_RETRYCANCEL);
            if (err == IDRETRY)
               continue;
#endif
            memset(xfer->data,0xff,MAXRDATA);
            establece_desconexion(1);
#ifdef FGVDOS
            MessageBox(NULL,"LA CONEXION CON EL SERVIDOR SE HA ROTO!!!!","ATENCION!",MB_OK);
#endif
            return(-1);
         }
      }
      
      if (rwp_rcv_buffer || FD_ISSET(MySock.sock,&setread))
      {
         if (!rwp_rcv_buffer)
         {
            cuanto = 0L;
            ioctlsocket(MySock.sock,FIONREAD,&cuanto);
            if (!cuanto)
            {
               /* desconexion !!! */
               recv(MySock.sock,&cuanto,cuanto,0);
               memset(xfer->data,0xff,MAXRDATA);
               establece_desconexion(1);
#ifdef FGVDOS
               MessageBox(NULL,"LA CONEXION CON EL SERVIDOR SE HA ROTO!!!!","ATENCION!",MB_OK);
#endif
               return(-1);
            }
            if (cuanto < HDRSIZE)
               continue;
         }
         if (recvdatamessage(MySock.sock,xfer) == FALSE)
         {
#ifdef FGVDOS
            MessageBox(NULL,__Texto[216],"ERROR",MB_OK);
#endif
            continue;
         }
         if (id == AGI_SORT)
         {
            return(xfer->data[0]);
         }
         return(0);
      }
   } /* while */   
}


int envia_xarxa(DATAMSG *sendbuf)
{
   sendbuf->hdr.signature = RWNIDENTIFICACION;
   if (senddatamessage (MySock.sock,sendbuf ) == TRUE) return(0);
   return(-1);
}
#endif

int responde_sesion()
{
   unsigned long cuanto;
   int err;
   
   flush_mysock();
   
   if (!rwp_rcv_buffer)
			{
      FD_ZERO(&setread);
      FD_SET(MySock.sock,&setread);
      FD_ZERO(&seterror);
      FD_SET(MySock.sock,&seterror);
      timeout.tv_sec = 60L; /* 60 segundos para obtener respuesta ... sino nada*/
      timeout.tv_usec = 0L;
	  do
	  {
#ifndef WIN32
			   errno = 0;
#endif
      if ((err = select(MySock.sock+1,&setread,NULL,&seterror,&timeout)) == SOCKET_ERROR)
      {
#ifndef WIN32
			 if (err == SOCKET_ERROR && errno == EINTR)
			 {
				 continue;
			 }
#endif

         return(-1);
      }
	  break;
	  } while(-1);
      if (!err)  
      {
         if (remoto)
         {
            pprint(1001,"(TIMEOUT!)");
         }
         return(-1);
      }
      
      if (FD_ISSET(MySock.sock,&seterror))
      {
         if (remoto)
         {
            pprint(1001,"(EXCEPCION!)");
         }
         return(-1);   
      }
      cuanto = 0L;
      ioctlsocket(MySock.sock,FIONREAD,&cuanto);
      if (!cuanto)
      {
         return(-1);
      }
			}
   if (recvdatamessage(MySock.sock,&xrcbbuf) == FALSE) 
			{
      return(-1);
			}
   return(xrcbbuf.hdr.command);
}


void envia_retro_al_server(void *puntero)
{
int funcion;
long tam,modo,envio,recibidos;
char tmp[11000];
char *p = (char *)puntero;
int len;
int codigo;
int waitfor;
char *pp;
char parcial;
DATAMSG2 __xrcbbuf;


   memcpy(&__xrcbbuf,&xrcbbuf,sizeof(__xrcbbuf));


   memcpy(tmp,p,sizeof(int));
   p += sizeof(int);
   len = sizeof(int);
   memcpy(&tam,p,sizeof(long));
   memcpy(tmp+len,&tam,sizeof(long));
   p += sizeof(long);
   len += sizeof(long);
   tam -= sizeof(long);
   memcpy(tmp+len,p,sizeof(long));
   p += sizeof(long);
   len += sizeof(long);
   tam -= sizeof(long);
   memcpy(&modo,p,sizeof(long));
   memcpy(tmp+len,&modo,sizeof(long));
   p += sizeof(long);
   len += sizeof(long);
   tam -= sizeof(long);
   if (tam > 10000)
      {
	  tmp[len] = 1;
	  len++;
	  envio = 10000;
      memcpy(tmp+len,&envio,sizeof(long));
	  len += sizeof(long);
	  }
   else
      {
	  tmp[len] = 0;
	  len++;
	  envio = tam;
	  }
   codigo = 67;
   while(tam)
      {
	  memcpy(tmp+len,p,envio);
	  p += envio;
	  tam -= envio;
	  len += envio;
      envia_control_a_server(tmp,len,codigo);
      len = 0;
	  codigo = 68;
      if (tam > 10000)
	     {
		 envio = 10000;
	     }
	  else
	     {
		 envio = tam;
		 }
	  }
   if ((modo & 1))
      {
	  waitfor = 67;
      if ((recibidos = _leecontrol_en_server(NULL,0,&waitfor)) < (sizeof(long)+sizeof(int)+1))
	    {
		  memcpy(&xrcbbuf,&__xrcbbuf,sizeof(__xrcbbuf));
		return;
		}			  
	  p = xrcbbuf.data+1;
	  pp =(char *)puntero;
	  
      memcpy(pp,p,sizeof(int));
      p += sizeof(int);	  
      len = sizeof(int);

      memcpy(&tam,p,sizeof(long));
      memcpy(pp+len,&tam,sizeof(long));
      p += sizeof(long);
      len += sizeof(long);
      tam -= sizeof(long);
	  parcial =*p;
	  p++;
	  if (parcial)
	     {
		 memcpy(&recibidos,p,sizeof(long));
		 p += sizeof(long);
		 }
	  else
	     recibidos = tam;
	  memcpy(pp+len,p,recibidos);
	  len += recibidos;	  
	  tam -= recibidos;
	  while(tam && parcial)
	     {
		 waitfor = 68;
         if ((recibidos = _leecontrol_en_server(NULL,0,&waitfor)) < 0)
	       {
			 memcpy(&xrcbbuf,&__xrcbbuf,sizeof(__xrcbbuf));
		   return;
	  	   }
		 p = xrcbbuf.data+1;
		 memcpy(pp+len,p,recibidos);
		 len += recibidos;
		 tam -= recibidos;
		 }
	  }
   memcpy(&xrcbbuf,&__xrcbbuf,sizeof(__xrcbbuf));
}


/*************************************************************************************/
/*           MODO SERVIDOR COMPLETO      */
/*************************************************************************************/


#ifdef RWSERVER


int print_en_terminal(s)
char *s;
{
   int i = strlen(s);
   if (en_conexion_externa_stop) return(0);
   xferbuf.hdr.signature = RWNIDENTIFICACION;
   xferbuf.hdr.command = COMANDO_TERMINAL;
   xferbuf.data[0] = 0; /* print */
   xferbuf.hdr.length = i + HDRSIZE + 4 + (sizeof(int)*3);
   xferbuf.data[1] = Patrib;
   memcpy_int(xferbuf.data+2,&Pcursor,sizeof(int));
   memcpy_int(xferbuf.data+2+sizeof(int),&PCcolor,sizeof(int));
   memcpy_int(xferbuf.data+2+(sizeof(int)*2),&PFcolor,sizeof(int));
   strcpy(xferbuf.data+2+(sizeof(int)*3),s);
   senddatamessage(MySock.sock, &xferbuf);
   return(i);
}

int conversion_en_terminal(c)
char c;
{
			if (en_conexion_externa_stop) return(0);
         xferbuf.hdr.signature = RWNIDENTIFICACION;
         xferbuf.hdr.command = COMANDO_TERMINAL;
         xferbuf.data[0] = 1; /* putc */
         xferbuf.hdr.length = HDRSIZE + 3 + sizeof(int);
         memcpy_int(xferbuf.data+1,&Pcursor,sizeof(int));
         xferbuf.data[1+sizeof(int)] = c;
         xferbuf.data[2+sizeof(int)] = Patrib;
         senddatamessage(MySock.sock, &xferbuf);
         return(c);
}

int cursor_en_terminal(a)
int a;
{
   if (en_conexion_externa_stop) return(0);
   xferbuf.hdr.signature = RWNIDENTIFICACION;
   xferbuf.hdr.command = COMANDO_TERMINAL;
   xferbuf.data[0] = 2; /* cursor */
   xferbuf.hdr.length = HDRSIZE + 1 + sizeof(int);
   memcpy_int(xferbuf.data+1,&a,sizeof(int));
   senddatamessage(MySock.sock, &xferbuf);
   return(a);
}

int cls_en_terminal()
{
   if (en_conexion_externa_stop) return(0);
   xferbuf.hdr.signature = RWNIDENTIFICACION;
   xferbuf.hdr.command = COMANDO_TERMINAL;
   xferbuf.data[0] = 3; /* cls */
   xferbuf.hdr.length = HDRSIZE + 3 + (sizeof(int)*3);
   xferbuf.data[1] = Patrib;
   xferbuf.data[2] = Pgrafi;
   memcpy_int(xferbuf.data+3,&PCcolor,sizeof(int));
   memcpy_int(xferbuf.data+3+sizeof(int),&PFcolor,sizeof(int));
   memcpy_int(xferbuf.data+3+(sizeof(int)*2),&win_fondo_neutro,sizeof(int));
   senddatamessage(MySock.sock, &xferbuf);
   return(0);
}

int graf_en_terminal(i)
int i;
{
   if (en_conexion_externa_stop) return(0);
   xferbuf.hdr.signature = RWNIDENTIFICACION;
   xferbuf.hdr.command = COMANDO_TERMINAL;
   xferbuf.data[0] = 4; /* graf */
   xferbuf.hdr.length = HDRSIZE + 4 + sizeof(int);
   memcpy_int(xferbuf.data+1,&Pcursor,sizeof(int));
   xferbuf.data[1+sizeof(int)] = i;
   xferbuf.data[2+sizeof(int)] = Patrib;
   senddatamessage(MySock.sock, &xferbuf);
   return(i);
}


/* conexion rota espera un nuevo terminal para reponer la situacion */
static int repon_situacion()
{
   int i;
   establece_desconexion(0);	 
   /*
   if (!establece_conexion(-1)) 
   {
      cls();
      w_lee_terminal(NULL,"");		 
      if (total_last_cabecera)
      {
         memcpy((char *)&xferbuf,last_cabecera,total_last_cabecera);
         senddatamessage(MySock.sock, &xferbuf);
      }
      return(0);
   }
   */
   /* eliminacion de la tarea a lo bestia de momento */
   if (stdpan)
   {
      cierra_todo();
      
      for (i = 0;i < 50;i++)
         desbloquea_aplicacion(i);
      
#ifdef RWSERVER
      if (en_conexion_externa)
      {
         salida_usuario_terminal();
      }
#endif
      desbloquea_usuario(user_who,subnumero_de_usuario);
      final_agisys();
      fin_sig();
      Terfinal();
   }
   exit(2);
   
   return(-1);
}


int leecontrol_en_terminal(pdata,len,waitfor)
char *pdata;
int len;
int *waitfor;
{
   unsigned long cuanto;
   int err,i;
   int prio;
   char *dat;
   char tmp[1024];
   char dati[255];
   char *_dato_entra;
   int _dato_entra_len;
   struct timeval timeout;
   int _timeouts;
   
   while(-1)
   {
      
      flush_mysock();
      if (!rwp_rcv_buffer)
      {  
		  _timeouts = 0;
		  do
		  {
			 FD_ZERO(&setread);
			 FD_SET(MySock.sock,&setread);
			 FD_ZERO(&seterror);
			 FD_SET(MySock.sock,&seterror);
			 timeout.tv_sec = 30L; /* ... para obtener respuesta ... sino comprobar conexion */
			 timeout.tv_usec = 0L;
#ifndef WIN32
			   errno = 0;
#endif
			 if ((err = select(MySock.sock+1,&setread,NULL,&seterror,&timeout)) == SOCKET_ERROR || FD_ISSET(MySock.sock,&seterror))
			 {
#ifndef WIN32
			 if (err == SOCKET_ERROR && errno == EINTR)
			 {
				 continue;
			 }
#endif
				 
				if (!repon_situacion()) 
				{		 
				   return(-1);
				}
	#ifdef FGVDOS
	#ifndef FGVWIN
	#ifndef WATCOMNLM
				if (myconsola == INVALID_HANDLE_VALUE)
				{
				   AllocConsole();
				   ini_my_consola();
				   cursor(Pcursor/80*100+Pcursor%80+101);
				}
	#endif
	#endif
	#endif
				v_refresca();
				aviso(0,__Texto[129]);
				return(-1);
			}
			if (!err)
			{
				_timeouts++;
				cuanto = 0L;
			    if (ioctlsocket(MySock.sock,FIONREAD,&cuanto) == SOCKET_ERROR)
				{
					if (!repon_situacion()) 
					{		 
					   return(-1);
					}
				}
				if (_timeouts >= maximo_timeouts)
				{
					if (!repon_situacion()) 
					{		 
					   return(-1);
					}
					_timeouts = 0;
				}
			}
		  } while(!err);
         
         cuanto = 0L;
         ioctlsocket(MySock.sock,FIONREAD,&cuanto);
         if (!cuanto)
		 {
            /* desconexion !!! */
            recv(MySock.sock,&cuanto,cuanto,0);        
            if (!repon_situacion()) 
            {		 		 
               return(-1);
            }
#ifdef FGVDOS
#ifndef FGVWIN
#ifndef WATCOMNLM
            if (myconsola == INVALID_HANDLE_VALUE)
            {
               AllocConsole();
               ini_my_consola();
               cursor(Pcursor/80*100+Pcursor%80+101);
            }
#endif
#endif
#endif
            v_refresca();
            aviso(0,__Texto[130]);
            return(-1);
		}
      }
      if (recvdatamessage(MySock.sock,&xrcbbuf) == FALSE)
      {   
         return(-1);
      }
      
      if (xrcbbuf.hdr.command == FILE_TRANSFER_CLOSE)
         continue;

      if (xrcbbuf.hdr.command == FILE_TRANSFER_ACK)
         continue;
      if (xrcbbuf.hdr.command == FILE_TRANSFER_NACK)
         continue;
      
      if (xrcbbuf.hdr.command == FILE_TRANSFER_DATA)
         continue;

      if (xrcbbuf.hdr.command == SOCORRO)
      {   
         return(-1); /* repite la comanda */
      }
      
      if (xrcbbuf.hdr.command != CONTROL_DE_TERMINAL)
      {
         return(-1);
      }
      
      if (waitfor)
      {
         *waitfor = xrcbbuf.data[0];
      }
      
      if (xrcbbuf.data[0] == 67) /* control trasmision binaria inversa */
      {  
	    int waitfor = 0;
		char *puntero;
		char *buffer;
		int funcion;
		char parcial;
		long tam,respuesta,modo,recibidos;
		int m_len = xrcbbuf.hdr.length-HDRSIZE-1;
		int len;
		int codigo;

		if ( m_len < (sizeof(int)+sizeof(long)*3+1) ) continue;
		
		puntero = xrcbbuf.data+1;

		memcpy_int(&funcion,puntero,sizeof(int));
		puntero += sizeof(int);
		memcpy_long(&tam,puntero,sizeof(long));
		puntero += sizeof(long);
		memcpy_long(&respuesta,puntero,sizeof(long));
		puntero += sizeof(long);
		memcpy_long(&modo,puntero,sizeof(long));
		puntero += sizeof(long);
		parcial = *puntero;
		puntero++;
				   
		if (tam > respuesta)
		   buffer = malloc(tam+sizeof(int));
		else
		   buffer = malloc(respuesta+sizeof(int));
		memcpy(buffer,&funcion,sizeof(int));
		len = sizeof(int);
		memcpy_long(buffer+len,&tam,sizeof(long));
		len += sizeof(long);
		tam -= sizeof(long);
		memcpy_long(buffer+len,&respuesta,sizeof(long));
		len += sizeof(long);
		tam -= sizeof(long);
		memcpy_long(buffer+len,&modo,sizeof(long));
		len += sizeof(long);
		tam -= sizeof(long);

		if (parcial)
		   {
		   memcpy_long(&recibidos,puntero,sizeof(long));
		   puntero += sizeof(long);
		   }
		else
		   {
		   recibidos = tam;
		   }

		memcpy(buffer+len,puntero,recibidos);
		tam -= recibidos;
		len += recibidos;

        while(tam)
		   {
		   waitfor = 0;
		   do
		     {
             if ((recibidos = leecontrol_en_terminal(NULL,0,&waitfor)) < 0) {free(buffer);return -1;}
		     } while(waitfor != 68);
		   memcpy(buffer+len,puntero,recibidos);
		   tam -= recibidos;
		   len += recibidos;
		   }

        RetroDespachador((void*)buffer);

		if ((modo & 1))
		   {
		   char tmp2[11000];
		   long envio;
		   long tam1; /* tam original */

		   puntero = buffer;		   

		   memcpy_int(tmp2,puntero,sizeof(int));
		   puntero += sizeof(int);
		   len = sizeof(int);
		   memcpy_long(&tam1,puntero,sizeof(long));
		   memcpy(&tam,puntero+sizeof(long),sizeof(long));
		   memcpy_long(tmp2+len,&tam,sizeof(long));
		   len     += sizeof(long);
		   puntero += sizeof(long);   
		   tam     -= sizeof(long);

		   if (tam > 10000)
		      {
			  envio = 10000;
			  tmp2[len] = 1;
			  len++;
			  memcpy_long(tmp2+len,&envio,sizeof(long));
			  len += sizeof(long);
			  }
		   else
		      {
			  envio = tam;
			  tmp2[len] = 0;
			  len++;
			  }		   
		   codigo = 67;
		   while(tam)
		      {
		      memcpy(tmp2+len,puntero,envio);
			  len += envio;
		      tam -= envio;
		      puntero += envio;
		      envia_control_a_terminal(tmp2,len,codigo);
			  codigo = 68;
			  len = 0;
		      if (tam > 10000)
		         {
				 envio = 10000;
				 }
			  else
			     {
				 envio = tam;
				 }
			  }
		   }
        continue;
      }
	  else      
      if (xrcbbuf.data[0] == 61) /* control trasmision */
      {
         trasmision_de_ficheros(); /* exclusivo C/S */
         envia_comando_a_terminal("",0,200);
         continue;
      }
	  else
      if (xrcbbuf.data[0] == 63) /* control usuarios */
      {
         control_de_usuarios(0,NULL);
         envia_comando_a_terminal("",0,200);
         continue;
      }
	  else
      if (xrcbbuf.data[0] == 164) /* control superusuario */
      {
         sw_superusuario();
         envia_comando_a_terminal("",0,200);
         continue;
      }	  
	  else
      if (xrcbbuf.data[0] == 165) /* control euro */
      {
         sw_euro();
         envia_comando_a_terminal("",0,200);
         continue;
      }	  
	  else
      if (xrcbbuf.data[0] == 166) /* funciones superusuario */
      {
		 gestiona_superusuario(xrcbbuf.data[1]);         
         envia_comando_a_terminal("",0,200);
         continue;
      }
	  else
      if (xrcbbuf.data[0] == 167) /* BUSCON */
      {
		 super_buscon(NULL);
         envia_comando_a_terminal("",0,200);
         continue;
      }
	  else
      if (xrcbbuf.data[0] == 168) /* ERP */
      {
		 llama_modulo_erp();
         envia_comando_a_terminal("",0,200);
         continue;
      }
      else if (xrcbbuf.data[0] == 169) /* EVT */
      {
		 int d;
		 extern int llama_evento_desc(void *nulo);
		 memcpy(&d,xrcbbuf.data+1,sizeof(int));
		 llama_fun_int(llama_evento_desc,(void *)d);
         envia_comando_a_terminal("",0,200);
         continue;
      } 
      else if (xrcbbuf.data[0] == 170) /* enviar fichero */
      {
		 rw_envia_fichero((char *)xrcbbuf.data+1,777);         
		 envia_comando_a_terminal("",0,200);
         continue;
      } 
      else
         if (xrcbbuf.data[0] == 42) /* control */
									{
            envia_control_a_terminal("",0,42);
            flush_mysock();
            if (f_disparo)
            {
               int d;
               memcpy_int(&d,xrcbbuf.data+1,sizeof(int));
               llama_fun_int(f_disparo,d);
            }
            envia_comando_a_terminal("",0,200);
            continue;
									}
         else
            if (xrcbbuf.data[0] == 15)
            {
               /* caso particular de subejecuta */
               dat = xrcbbuf.data+1;
               memcpy_int(&redita_col,dat,sizeof(int));    
               dat += sizeof(int);
               strcpy(tmp,dat);
               dat += (strlen(tmp)+1);
               _dato_entra = dato_entra;
               _dato_entra_len = dato_entra_len;
               dato_entra = NULL;
               if (*dat)
               {
                  dat++;
                  dato_entra = dati;
                  memcpy_int(&dato_entra_len,dat,sizeof(int));
                  dat += sizeof(int);
                  strcpy(dato_entra,dat);
               }
               if (f_util)
               {	 
                  llama_fun_int(f_util,tmp);
               }
               dat = tmp;
               memcpy_int(dat,&redita_col,sizeof(int));
               dat += sizeof(int);
               if (dato_entra)
               {
                  strcpy(dat,dato_entra);
               }
               else
                  *dat = 0;
               dato_entra = _dato_entra;
               dato_entra_len = _dato_entra_len;     
               envia_control_a_terminal(tmp,300,4);     
               continue;
            }
            else
               if (xrcbbuf.data[0] == 10)
               {
                  /* caso especialisimo control teclado dentro de leecontrol */
                  dat = xrcbbuf.data+1;
                  memcpy_int(&err,dat,sizeof(int));
                  dat += sizeof(int);
                  memcpy_int(&_M_Cursor,dat,sizeof(int));
                  dat += sizeof(int);
                  memcpy_int(&_ID_Cursor,dat,sizeof(int)); 
                  dat += sizeof(int);
                  memcpy_int(&_M_EXTRA_AVANCE,dat,sizeof(int)); 
                  dat += sizeof(int);
                  memcpy_int(&redita_col,dat,sizeof(int));    
                  dat += sizeof(int);
                  _dato_entra = dato_entra;
                  _dato_entra_len = dato_entra_len;
                  dato_entra = NULL;
                  if (*dat)
                  {
                     dat++;
                     dato_entra = dati;
                     memcpy_int(&dato_entra_len,dat,sizeof(int));
                     dat += sizeof(int);
                     strcpy(dato_entra,dat);
                  }	 
                  memcpy_short(tmp,buf_tecla,sizeof(short)*200);
                  buf_tecla[0] = err; /* perentorio */
                  buf_tecla[1] = -1;
                  leecar_server_puro = 1;
                  i = leecar();
                  memcpy_short(buf_tecla,tmp,sizeof(short)*200);   
                  
                  dat = tmp;
                  memcpy_int(dat,&redita_col,sizeof(int));
                  dat += sizeof(int);
                  *dat = leecar_server_puro;
                  dat++;
                  memcpy_int(dat,&i,sizeof(int));
                  dat += sizeof(int);
                  leecar_server_puro = 0;
                  if (dato_entra)
                  {
                     strcpy(dat,dato_entra);
                  }
                  else
                     *dat = 0;
                  dato_entra = _dato_entra;
                  dato_entra_len = _dato_entra_len;
                  envia_control_a_terminal(tmp,300,2);
                  continue;
               }
               break;
}


if (pdata)
{
   if (len == -1)
   {
      len = xrcbbuf.hdr.length - HDRSIZE - 1;
   }
   memcpy(pdata,xrcbbuf.data+1,len);
   return len;
}
return(xrcbbuf.hdr.length-HDRSIZE-1);
}


int ayuda_remota(clave)
char *clave;
{
   unsigned char *pdata;
   int bien;
   
   do {
      envio_situacion = 72;
      xferbuf.hdr.signature = RWNIDENTIFICACION;
      xferbuf.hdr.command = COMANDO_TERMINAL;
      xferbuf.data[0] = 72;  
      pdata = xferbuf.data+1;
      strcpy(pdata,clave);
      pdata += (strlen(clave)+1);
      if (dirbase_ayuda)
      {
         *pdata = 1;
         pdata++;
         strcpy(pdata,dirbase_ayuda);
         pdata += (strlen(dirbase_ayuda)+1);
      }
      else
      {
         *pdata = 0;
         pdata++;
      }
      
      if (nomapli_ayuda)
      {
         *pdata = 1;
         pdata++;
         strcpy(pdata,nomapli_ayuda);
         pdata += (strlen(nomapli_ayuda)+1);
      }
      else
      {
         *pdata = 0;
         pdata++;
      }
      
      strcpy(pdata,referencia_help);
      pdata += (strlen(referencia_help)+1);
      
      xferbuf.hdr.length = HDRSIZE + (int)(pdata - xferbuf.data);
      senddatamessage(MySock.sock, &xferbuf);
      
      if (leecontrol_en_terminal(&bien,sizeof(int),NULL) < 0)
      {
         continue;
      }
	  moto_short(&bien);
      return(bien);
      
   } while(-1);
   
   return(0);
}


int pulsatecla_en_terminal()
{
   int err;
   
   /* chequear pulsacion !! */
   do {
      envio_situacion = 53;
      xferbuf.hdr.signature = RWNIDENTIFICACION;
      xferbuf.hdr.command = COMANDO_TERMINAL;
      xferbuf.data[0] = 53;
      xferbuf.hdr.length = HDRSIZE + 1;
      senddatamessage(MySock.sock, &xferbuf);
      /*printf("Pedida pulsatecla en terminal\n"); */  
      if (leecontrol_en_terminal(NULL,0,NULL) < 0)
      {
         continue;
      }
      memcpy_int(&err,xrcbbuf.data+1,sizeof(int)); 
      /* printf("Leida pulsatecla en terminal[%d]\n",err);*/
      return(err);
			} while(-1);
}

int leecar_en_terminal()
{
   unsigned long cuanto;
   int err;
   
   /* pedir tecla !! */
   
   do {
      envio_situacion = 5;
      xferbuf.hdr.signature = RWNIDENTIFICACION;
      xferbuf.hdr.command = COMANDO_TERMINAL;
      xferbuf.data[0] = 5;
      xferbuf.hdr.length = HDRSIZE + 1 + sizeof(int);
      memcpy_int(xferbuf.data+1,&modo_de_validacion,sizeof(int));
      senddatamessage(MySock.sock, &xferbuf);
      
      
      if (leecontrol_en_terminal(NULL,0,NULL) < 0)
      {
         continue;
      }
      
      memcpy_int(&err,xrcbbuf.data+1,sizeof(int));
      memcpy_int(&_M_Cursor,xrcbbuf.data+1+sizeof(int),sizeof(int));
      memcpy_int(&_ID_Cursor,xrcbbuf.data+1+(sizeof(int)*2),sizeof(int));
      memcpy_int(&_M_EXTRA_AVANCE,xrcbbuf.data+1+(sizeof(int)*3),sizeof(int));
      /* printf("Leida tecla en terminal[%d]\n",err); */
      
      return(err);
			} while(-1);
}


int curon_en_terminal(i)
int i;
{
   if (en_conexion_externa_stop) return(0);
   xferbuf.hdr.signature = RWNIDENTIFICACION;
   xferbuf.hdr.command = COMANDO_TERMINAL;
   xferbuf.data[0] = 6; /* curon */
   xferbuf.hdr.length = HDRSIZE + 1 + (sizeof(int)*2);
   memcpy_int(xferbuf.data+1,&i,sizeof(int));
   memcpy_int(xferbuf.data+1+sizeof(int),&Pcursor,sizeof(int));
   senddatamessage(MySock.sock, &xferbuf);
   return(i);
}


int entra_en_terminal(qp,maxqc,qi,qs,qd,qh,qo)
int qp,
maxqc,
qh;
char *qi,*qs,*qd,*qo;
{
			unsigned long cuanto;
         int err;
         unsigned char *pdata;
         
         
         /* entras de dato */
         do {
            
            envio_situacion = 55;
            xferbuf.hdr.signature = RWNIDENTIFICACION;
            xferbuf.hdr.command = COMANDO_TERMINAL;
            xferbuf.data[0] = 55;  
            pdata = xferbuf.data + 1;
            memcpy_int(pdata,&modo_de_validacion,sizeof(int));
            pdata += sizeof(int);
            memcpy(pdata,clave_help,32);
            pdata += 32;
            /*memcpy(pdata,&rentra_autosal,sizeof(int));
            pdata += sizeof(int);*/
            memcpy_int(pdata,&rentra_edicion,sizeof(int));
            pdata += sizeof(int);
            memcpy_int(pdata,&rentra_sal_atrib,sizeof(int));
            pdata += sizeof(int);
            /*memcpy(pdata,&rentra_autosal,sizeof(int));
            pdata += sizeof(int);*/
            memcpy_int(pdata,&rentra_cc,sizeof(int));
            pdata += sizeof(int);
            memcpy_int(pdata,&rentra_cf,sizeof(int));
            pdata += sizeof(int);
            memcpy_int(pdata,&rentra_sal_cc,sizeof(int));
            pdata += sizeof(int);
            memcpy_int(pdata,&rentra_sal_cf,sizeof(int));
            pdata += sizeof(int);
            memcpy_int(pdata,&rentra_offqp,sizeof(int));
            pdata += sizeof(int);
            memcpy_int(pdata,&rentra_offini,sizeof(int));
            pdata += sizeof(int);
            memcpy_int(pdata,&rentra_insert,sizeof(int));
            pdata += sizeof(int);
            memcpy_int(pdata,&redita_col,sizeof(int));
            pdata += sizeof(int);
            memcpy_int(pdata,&qpi_rt,sizeof(int));
            pdata += sizeof(int);
            
            memcpy_int(pdata,&qp,sizeof(int));
            pdata += sizeof(int);
            memcpy_int(pdata,&maxqc,sizeof(int));
            pdata += sizeof(int);
            memcpy_int(pdata,&qh,sizeof(int));
            pdata += sizeof(int);
            
            memcpy_int(pdata,&data_fecha_conmutar,sizeof(int));
            pdata += sizeof(int);  
            memcpy_int(pdata,&data_hiden_indicator,sizeof(int));
            pdata += sizeof(int);  
            *pdata = caracter_de_fecha;
            pdata++;
            
            if (rentra_pos)
            {
               if (rentra_pos == pinta_col)
                  *pdata = 1;
               else
                  *pdata = 2;
            }
            else
               *pdata = 0;
            pdata++;
            strcpy(pdata,qi);
            pdata += strlen(qi)+1;
            strcpy(pdata,qs);
            pdata += strlen(qs)+1;
            strcpy(pdata,qd);
            pdata += strlen(qd)+1;
            
            if (rentra_edicion)
            {
               strcpy(pdata,qo);
               pdata += strlen(qo)+1;
            }
            else
            {
               if (qo != qd)
                  *pdata = 0;
               else
                  *pdata = 1;
               pdata++;
               /*
               memcpy(pdata,qo,maxqc+1);  
               pdata += (maxqc+1);
               */
            }
            
            
            xferbuf.hdr.length = HDRSIZE + (int)(pdata - xferbuf.data) + 5;
            senddatamessage(MySock.sock, &xferbuf);
            
            if (leecontrol_en_terminal(NULL,0,NULL) < 0)
            {
               continue;
            }
            
            pdata = xrcbbuf.data+1;
            memcpy_int(&err,pdata,sizeof(int));
            pdata += sizeof(int);
            memcpy_int(&_M_Cursor,pdata,sizeof(int));
            pdata += sizeof(int);
            memcpy_int(&_ID_Cursor,pdata,sizeof(int));
            pdata += sizeof(int);
            memcpy_int(&_M_EXTRA_AVANCE,pdata,sizeof(int));
            pdata += sizeof(int);  
            memcpy(clave_help,pdata,32);
            pdata += 32;
            /*memcpy(&rentra_autosal,pdata,sizeof(int));
            pdata += sizeof(int);*/
            memcpy_int(&rentra_edicion,pdata,sizeof(int));
            pdata += sizeof(int);
            memcpy_int(&rentra_sal_atrib,pdata,sizeof(int));
            pdata += sizeof(int);
            /*memcpy(&rentra_autosal,pdata,sizeof(int));
            pdata += sizeof(int);*/
            memcpy_int(&rentra_cc,pdata,sizeof(int));
            pdata += sizeof(int);
            memcpy_int(&rentra_cf,pdata,sizeof(int));
            pdata += sizeof(int);
            memcpy_int(&rentra_sal_cc,pdata,sizeof(int));
            pdata += sizeof(int);
            memcpy_int(&rentra_sal_cf,pdata,sizeof(int));
            pdata += sizeof(int);
            memcpy_int(&rentra_offqp,pdata,sizeof(int));
            pdata += sizeof(int);
            memcpy_int(&rentra_offini,pdata,sizeof(int));
            pdata += sizeof(int);
            memcpy_int(&rentra_insert,pdata,sizeof(int));
            pdata += sizeof(int);
            memcpy_int(&redita_col,pdata,sizeof(int));
            pdata += sizeof(int);
            memcpy_int(&eqj,pdata,sizeof(int));
            pdata += sizeof(int);
            memcpy_int(&data_fecha_conmutar,pdata,sizeof(int));
            pdata += sizeof(int);  
            memcpy_int(&data_hiden_indicator,pdata,sizeof(int));
            pdata += sizeof(int);  
            caracter_de_fecha = *pdata;
            pdata++;
            strcpy(qo,pdata);
            return(err);
            
 } while(-1);
 
}


int refresca_en_terminal(w)
VENT *w;
{
   char *pdata = xferbuf.data;
   
   if (en_conexion_externa_stop) return(0);
   
   xferbuf.hdr.signature = RWNIDENTIFICACION;
   xferbuf.hdr.command = COMANDO_TERMINAL;
   *pdata = 7; /* refresca */
   pdata++;
   if (w == stdpan)
   {
      w = NULL;
   }
   memcpy_int(pdata,&Pcursor,sizeof(int));
   pdata += sizeof(int);
   memcpy_VENT(pdata,&w,sizeof(w));
   pdata += sizeof(int);
   memcpy_int(pdata,&win_fondo_neutro,sizeof(int));
   pdata += sizeof(int);
   *pdata = Patrib;
   xferbuf.hdr.length = HDRSIZE + sizeof(w) + 3 + (sizeof(int)*2);
   senddatamessage(MySock.sock, &xferbuf);
   return(0);
}


int atributo_en_terminal(a,c1,c2)
char a;
int c1;
int c2;
{
   if (en_conexion_externa_stop) return(0);
   
   xferbuf.hdr.signature = RWNIDENTIFICACION;
   xferbuf.hdr.command = COMANDO_TERMINAL;
   xferbuf.data[0] = 8; /* atributo */
   xferbuf.hdr.length = HDRSIZE + 3 + (sizeof(int)*2);
   xferbuf.data[1] = a;
   xferbuf.data[2] = Pgrafi;
   memcpy_int(xferbuf.data+3,&c1,sizeof(int));
   memcpy_int(xferbuf.data+3+sizeof(int),&c2,sizeof(int));
   senddatamessage(MySock.sock, &xferbuf);
   return(0);
}


int w_ini_en_terminal(w)
VENT *w;
{
			
			if (en_conexion_externa_stop) return(0);
         
         xferbuf.hdr.signature = RWNIDENTIFICACION;
         xferbuf.hdr.command = COMANDO_TERMINAL;
         xferbuf.data[0] = 9; /* w_ini */
         xferbuf.hdr.length = HDRSIZE + 1 + sizeof(w) + (sizeof(int)*2);
         memcpy_VENT(xferbuf.data+1,&w,sizeof(w));
         memcpy_int(xferbuf.data+1+sizeof(w),&(w->vini),sizeof(int));
         memcpy_int(xferbuf.data+1+sizeof(w)+sizeof(int),&(w->vfin),sizeof(int));
         senddatamessage(MySock.sock, &xferbuf);
         return(0);
}

int w_fin_en_terminal(w)
VENT *w;
{
   if (en_conexion_externa_stop) return(0);
   
   xferbuf.hdr.signature = RWNIDENTIFICACION;
   xferbuf.hdr.command = COMANDO_TERMINAL;
   xferbuf.data[0] = 10; /* w_fin */
   xferbuf.hdr.length = HDRSIZE + 1 + sizeof(w);
   memcpy_VENT(xferbuf.data+1,&w,sizeof(w));
   senddatamessage(MySock.sock, &xferbuf);
   return(0);
}


int w_iniesp_en_terminal(w)
VENT *w;
{
			
		 if (en_conexion_externa_stop) return(0);
         
         xferbuf.hdr.signature = RWNIDENTIFICACION;
         xferbuf.hdr.command = COMANDO_TERMINAL;
         xferbuf.data[0] = 209; /* w_ini */
         xferbuf.hdr.length = HDRSIZE + 1 + sizeof(w) + (sizeof(int)*2);
         memcpy_VENT(xferbuf.data+1,&w,sizeof(w));
         memcpy_int(xferbuf.data+1+sizeof(w),&(w->vini),sizeof(int));
         memcpy_int(xferbuf.data+1+sizeof(w)+sizeof(int),&(w->vfin),sizeof(int));
         senddatamessage(MySock.sock, &xferbuf);
         return(0);
}

int w_finesp_en_terminal(w)
VENT *w;
{
   if (en_conexion_externa_stop) return(0);
   
   xferbuf.hdr.signature = RWNIDENTIFICACION;
   xferbuf.hdr.command = COMANDO_TERMINAL;
   xferbuf.data[0] = 210; /* w_fin */
   xferbuf.hdr.length = HDRSIZE + 1 + sizeof(w);
   memcpy_VENT(xferbuf.data+1,&w,sizeof(w));
   senddatamessage(MySock.sock, &xferbuf);
   return(0);
}



int w_putc_en_terminal(w,c)
VENT *w;
char c;
{
   if (en_conexion_externa_stop) return(0);
   
   xferbuf.hdr.signature = RWNIDENTIFICACION;
   xferbuf.hdr.command = COMANDO_TERMINAL;
   xferbuf.data[0] = 11; /* w_putc */
   xferbuf.data[1] = c;
   xferbuf.hdr.length = HDRSIZE + 2 + sizeof(w) + sizeof(int);
   memcpy_VENT(xferbuf.data+2,&w,sizeof(w));
   memcpy_int(xferbuf.data+2+sizeof(w),&Pcursor,sizeof(int));
   senddatamessage(MySock.sock, &xferbuf);
   return(0);
}


int wcambia_en_terminal(w,pi,pf)
VENT *w;
int pi,pf;
{
   
   if (en_conexion_externa_stop) return(0);
   
   xferbuf.hdr.signature = RWNIDENTIFICACION;
   xferbuf.hdr.command = COMANDO_TERMINAL;
   xferbuf.data[0] = 12; /* w_cambia */
   xferbuf.hdr.length = HDRSIZE + 1 + sizeof(w) + (sizeof(int)*2);
   memcpy_VENT(xferbuf.data+1,&w,sizeof(w));
   memcpy_int(xferbuf.data+1+sizeof(w),&pi,sizeof(int));
   memcpy_int(xferbuf.data+1+sizeof(w)+sizeof(int),&pf,sizeof(int));
   senddatamessage(MySock.sock, &xferbuf);
   return(0);
}

int wmarco_en_terminal(w,pi,pf,tipo)
VENT *w;
int pi,pf,tipo;
{
   
   if (en_conexion_externa_stop) return(0);
   
   xferbuf.hdr.signature = RWNIDENTIFICACION;
   xferbuf.hdr.command = COMANDO_TERMINAL;
   xferbuf.data[0] = 13; /* w_marco_ */
   xferbuf.hdr.length = HDRSIZE + 1 + sizeof(w) + (sizeof(int)*3);
   memcpy_VENT(xferbuf.data+1,&w,sizeof(w));
   memcpy_int(xferbuf.data+1+sizeof(w),&pi,sizeof(int));
   memcpy_int(xferbuf.data+1+sizeof(w)+sizeof(int),&pf,sizeof(int));
   memcpy_int(xferbuf.data+1+sizeof(w)+(sizeof(int)*2),&tipo,sizeof(int));
   senddatamessage(MySock.sock, &xferbuf);
   return(0);
}


int w_saca_en_terminal(w,wo)
VENT *w,*wo;
{
   if (en_conexion_externa_stop) return(0);
   
   if (w == stdpan) w = NULL;
   if (wo == stdpan) wo = NULL;
   
   xferbuf.hdr.signature = RWNIDENTIFICACION;
   xferbuf.hdr.command = COMANDO_TERMINAL;
   xferbuf.data[0] = 14; /* w_saca */
   xferbuf.hdr.length = HDRSIZE + 1 + (sizeof(w)*2);
   memcpy_VENT(xferbuf.data+1,&w,sizeof(w));
   memcpy_VENT(xferbuf.data+1+sizeof(w),&wo,sizeof(wo));
   senddatamessage(MySock.sock, &xferbuf);
   return(0);
}

int w_mete_en_terminal(w,wo,modo)
VENT *w,*wo;
int modo;
{
   if (en_conexion_externa_stop) return(0);
   
   if (w == stdpan) w = NULL;
   if (wo == stdpan) wo = NULL;
   
   xferbuf.hdr.signature = RWNIDENTIFICACION;
   xferbuf.hdr.command = COMANDO_TERMINAL;
   xferbuf.data[0] = 15; /* _w_mete */
   xferbuf.hdr.length = HDRSIZE + 1 + (sizeof(w)*2) + sizeof(int);
   memcpy_VENT(xferbuf.data+1,&w,sizeof(w));
   memcpy_VENT(xferbuf.data+1+sizeof(w),&wo,sizeof(wo));
   memcpy_int(xferbuf.data+1+(sizeof(w)*2),&modo,sizeof(int));
   senddatamessage(MySock.sock, &xferbuf);
   return(0);
}

int w_lee_terminal(w,tmp)
VENT *w;
char *tmp;
{
   unsigned char *pdata;
   int i,j,k;
   char tt[81];
   short ppp,yyy,iii;
   unsigned char cosa;
   unsigned char *wvv;
   
   if (en_conexion_externa_stop) return(0);
   
   
   if (!w) 
   {  
      xferbuf.data[0] = 58; /* stdpan */
      w = stdpan;	
   }
   else
      xferbuf.data[0] = 16; /* w_lee */
   xferbuf.hdr.signature = RWNIDENTIFICACION;
   xferbuf.hdr.command = COMANDO_TERMINAL;
   
   pdata = xferbuf.data + 1;
   memcpy_VENT(pdata,&w,sizeof(w));
   pdata += sizeof(w);  
   
   memcpy_int(pdata,&(w->vini),sizeof(int));
   pdata += sizeof(int);  
   
   memcpy_int(pdata,&(w->vfin),sizeof(int));
   pdata += sizeof(int);
   
   
   memcpy_int(pdata,&(w->size),sizeof(int));
   pdata += sizeof(int);  
   
   *pdata = 1; /* compresion fija */
   pdata++;
   
   wvv = w->vv;
   
   k= w->size*3;
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
               memcpy_short(pdata,(char *)&ppp,sizeof(short));
               pdata += sizeof(short);
               memcpy(pdata,wvv+j,iii);
               pdata += iii;
               j += iii;
               iii = 0;
            }
            ppp = (-yyy);
            memcpy_short(pdata,(char *)&ppp,sizeof(short));
            pdata += sizeof(short);
            *pdata = wvv[j];
            pdata++;          
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
      memcpy_short(pdata,(char *)&ppp,sizeof(short));
      pdata += sizeof(short);
      memcpy(pdata,wvv+j,iii);
      pdata += iii;
      j += iii;
      iii = 0;       
   }
   if (yyy)
   {
      ppp = (-yyy);      
      memcpy_short(pdata,(char *)&ppp,sizeof(short));
      pdata += sizeof(short);
      *pdata = wvv[j];
      pdata++;    
      j += yyy;
      yyy = 0;
   }
   
   strcpy(pdata,tmp);
   pdata += (strlen(tmp)+1);
   
   xferbuf.hdr.length = HDRSIZE + (int)(pdata -xferbuf.data);
   senddatamessage(MySock.sock, &xferbuf);
   
   return(0);
}

int w_cls_en_terminal(w)
VENT *w;
{
   if (en_conexion_externa_stop) return(0);
   xferbuf.hdr.signature = RWNIDENTIFICACION;
   xferbuf.hdr.command = COMANDO_TERMINAL;
   xferbuf.data[0] = 17; /* w_cls */
   xferbuf.hdr.length = HDRSIZE + 1 + sizeof(w);
   memcpy_VENT(xferbuf.data+1,&w,sizeof(w));
   senddatamessage(MySock.sock, &xferbuf);
   return(0);
}

int w_wll_en_terminal(w,l)
VENT *w;
int l;
{
   
   if (en_conexion_externa_stop) return(0);
   xferbuf.hdr.signature = RWNIDENTIFICACION;
   xferbuf.hdr.command = COMANDO_TERMINAL;
   xferbuf.data[0] = 18; /* w_wll */
   xferbuf.hdr.length = HDRSIZE + 1 + sizeof(w) + sizeof(int);
   memcpy_VENT(xferbuf.data+1,&w,sizeof(w));
   memcpy_int(xferbuf.data+1+sizeof(w),&l,sizeof(int));
   senddatamessage(MySock.sock, &xferbuf);
   return(0);
}

int Bl_en_terminal(lin)
int lin;
{
			if (en_conexion_externa_stop) return(0);
         xferbuf.hdr.signature = RWNIDENTIFICACION;
         xferbuf.hdr.command = COMANDO_TERMINAL;
         xferbuf.data[0] = 19; /* Bl */
         xferbuf.hdr.length = HDRSIZE + 1 + sizeof(int);
         memcpy_int(xferbuf.data+1,&lin,sizeof(int));
         senddatamessage(MySock.sock, &xferbuf);
         return(0);
}

int pito_en_terminal()
{
   
   if (en_conexion_externa_stop) return(0);
   xferbuf.hdr.signature = RWNIDENTIFICACION;
   xferbuf.hdr.command = COMANDO_TERMINAL;
   xferbuf.data[0] = 20; /* pito */
   xferbuf.hdr.length = HDRSIZE + 1;
   senddatamessage(MySock.sock, &xferbuf);
   return(0);
}


int vscroll_en_terminal(ini,fin,n)
int ini,fin,n;
{
   if (en_conexion_externa_stop) return(0);
   xferbuf.hdr.signature = RWNIDENTIFICACION;
   xferbuf.hdr.command = COMANDO_TERMINAL;
   xferbuf.data[0] = 21; /* vscroll */
   xferbuf.hdr.length = HDRSIZE + 1 + (sizeof(int)*3);
   memcpy_int(xferbuf.data+1,&ini,sizeof(int));
   memcpy_int(xferbuf.data+1+sizeof(int),&fin,sizeof(int));
   memcpy_int(xferbuf.data+1+(sizeof(int)*2),&n,sizeof(int));
   senddatamessage(MySock.sock, &xferbuf);
   return(0);
}


int mensaje_terminal(pos,str)
int pos;
char *str;
{
   if (en_conexion_externa_stop) return(0);
   xferbuf.hdr.signature = RWNIDENTIFICACION;
   xferbuf.hdr.command = COMANDO_TERMINAL;
   xferbuf.data[0] = 22; /* mensaje */
   xferbuf.hdr.length = HDRSIZE + 2 + sizeof(int) + strlen(str);
   memcpy_int(xferbuf.data+1,&pos,sizeof(int));
   strcpy(xferbuf.data+1+sizeof(int),str);
   senddatamessage(MySock.sock, &xferbuf);
   return(0);
}

int parada_en_terminal()
{
   int bien;
   char *pdata;
   if (en_conexion_externa_stop) return(0);
   do {
      envio_situacion = 23;
      xferbuf.hdr.signature = RWNIDENTIFICACION;
      xferbuf.hdr.command = COMANDO_TERMINAL;
      xferbuf.data[0] = 23; /* parada */
      xferbuf.hdr.length = HDRSIZE + 1;
      senddatamessage(MySock.sock, &xferbuf);
      if (leecontrol_en_terminal(NULL,0,NULL) < 0)
      {
         continue;
      }  
      pdata = xrcbbuf.data+1;
      memcpy_int(&bien,pdata,sizeof(int));
      pdata += sizeof(int);
      memcpy_int(&eqj,pdata,sizeof(int));
      return(bien);
   } while(-1);
}

int aviso_terminal(pos,str)
int pos;
char *str;
{
   int bien;
   char *pdata;
   if (en_conexion_externa_stop) return(0); 
   
   do {
      envio_situacion = 24;
      xferbuf.hdr.signature = RWNIDENTIFICACION;
      xferbuf.hdr.command = COMANDO_TERMINAL;
      xferbuf.data[0] = 24; /* aviso */
      xferbuf.hdr.length = HDRSIZE + 2 + sizeof(int) + strlen(str);
      memcpy_int(xferbuf.data+1,&pos,sizeof(int));
      strcpy(xferbuf.data+1+sizeof(int),str);
      senddatamessage(MySock.sock, &xferbuf);
      if (leecontrol_en_terminal(NULL,0,NULL) < 0)
      {
         continue;
      }
      pdata = xrcbbuf.data+1;
      memcpy_int(&bien,pdata,sizeof(int));
      pdata += sizeof(int);
      memcpy_int(&eqj,pdata,sizeof(int));  
      return(bien);
   } while(-1);
}

int correcto_en_terminal()
{
   int bien;
   char *pdata;
   if (en_conexion_externa_stop) return(0);
   do {
      envio_situacion = 25;
      xferbuf.hdr.signature = RWNIDENTIFICACION;
      xferbuf.hdr.command = COMANDO_TERMINAL;
      xferbuf.data[0] = 25; /* correcto */
      xferbuf.hdr.length = HDRSIZE + 1 + sizeof(int);
      memcpy_int(xferbuf.data+1,&modo_de_validacion,sizeof(int));
      senddatamessage(MySock.sock, &xferbuf);
      if (leecontrol_en_terminal(NULL,0,NULL) < 0)
      {
									continue;
      }
      pdata = xrcbbuf.data+1;
      memcpy_int(&bien,pdata,sizeof(int));
      pdata += sizeof(int);
      memcpy_int(&eqj,pdata,sizeof(int));
      return(bien);  
   } while(-1);
}

int confirmar_en_terminal(pos,mensa,defec)
int pos;
char *mensa;
char *defec;
{
   int bien;
   char *pdata;
   if (en_conexion_externa_stop) return(0);
   do {
      envio_situacion = 26;
      xferbuf.hdr.signature = RWNIDENTIFICACION;
      xferbuf.hdr.command = COMANDO_TERMINAL;
      xferbuf.data[0] = 26; /* confirmar */
      xferbuf.hdr.length = HDRSIZE + 3 + sizeof(int)*2 + strlen(mensa) + strlen(defec);
      memcpy_int(xferbuf.data+1,&modo_de_validacion,sizeof(int));
      memcpy_int(xferbuf.data+1+sizeof(int),&pos,sizeof(int));
      pdata = xferbuf.data+1+sizeof(int)*2;
      strcpy(pdata,mensa);
      pdata += (strlen(mensa)+1);
      strcpy(pdata,defec);
      senddatamessage(MySock.sock, &xferbuf);
      if (leecontrol_en_terminal(NULL,0,NULL) < 0)
      {
									continue;
      }
      pdata = xrcbbuf.data+1;
      memcpy_int(&bien,pdata,sizeof(int));
      pdata += sizeof(int);
      memcpy_int(&eqj,pdata,sizeof(int));
      return(bien);
   } while(-1);
}


int recurre_mensaje_cab_terminal(m)
int m;
{
   if (en_conexion_externa_stop) return(0);
   xferbuf.hdr.signature = RWNIDENTIFICACION;
   xferbuf.hdr.command = COMANDO_TERMINAL;
   xferbuf.data[0] = 27; /*  */
   xferbuf.hdr.length = HDRSIZE + 1 + sizeof(int);
   memcpy_int(xferbuf.data+1,&m,sizeof(int));
   senddatamessage(MySock.sock, &xferbuf);
   return(0);
}


int borra_mensaje_cab_terminal(l)
int l;
{
   if (en_conexion_externa_stop) return(0);
   xferbuf.hdr.signature = RWNIDENTIFICACION;
   xferbuf.hdr.command = COMANDO_TERMINAL;
   xferbuf.data[0] = 28; /*  */
   xferbuf.hdr.length = HDRSIZE + 1 + sizeof(int);
   memcpy_int(xferbuf.data+1,&l,sizeof(int));
   senddatamessage(MySock.sock, &xferbuf);
   return(0);
}

int mensaje_en_cab_terminal(usi,men,lado)
char *usi,*men;
int lado;
{
   char *pdata;
   if (en_conexion_externa_stop) return(0);
   xferbuf.hdr.signature = RWNIDENTIFICACION;
   xferbuf.hdr.command = COMANDO_TERMINAL;
   xferbuf.data[0] = 29; /*  */
   xferbuf.hdr.length = HDRSIZE + 3 + sizeof(int) + strlen(usi) + strlen(men);
   
   memcpy_int(xferbuf.data+1,&lado,sizeof(int));
   
   pdata = xferbuf.data+1+sizeof(int);
   strcpy(pdata,usi);
   pdata += (strlen(usi)+1);
   strcpy(pdata,men);
   senddatamessage(MySock.sock, &xferbuf);
   return(0);
}



int cabecera_v_terminal(version,proceso,titulo,empresa)
char *version,*proceso,*titulo,*empresa;
{
   char *pdata;
   if (en_conexion_externa_stop) return(0);
   xferbuf.hdr.signature = RWNIDENTIFICACION;
   xferbuf.hdr.command = COMANDO_TERMINAL;
   xferbuf.data[0] = 30; /* cabecera */
   xferbuf.hdr.length = HDRSIZE + 8 + (sizeof(int)*3) + strlen(proceso) + strlen(titulo) + strlen(empresa);
   pdata = xferbuf.data+1;
   memcpy_int(pdata,&fecha_protegida,sizeof(int));
   pdata += sizeof(int);
   memcpy_int(pdata,&linea_25,sizeof(int));
   pdata += sizeof(int);
   memcpy_int(pdata,&duplicarusuario,sizeof(int));
   pdata += sizeof(int);
   
   strcpy(pdata,proceso);
   pdata += (strlen(proceso)+1);
   strcpy(pdata,titulo);
   pdata += (strlen(titulo)+1);
   strcpy(pdata,empresa);
   pdata += (strlen(empresa)+1);
   if (version)
   {
      *pdata = 1;
      pdata++;
      xferbuf.hdr.length += (strlen(version)+1);
      strcpy(pdata,version);
      pdata += (strlen(version)+1);
   }
   else
   {
      *pdata = 0;
      pdata++;
   }
   
   if (cabeza_especial)
   {
      *pdata = 1;
      pdata++;
      xferbuf.hdr.length += (strlen(cabeza_especial)+1);
      strcpy(pdata,cabeza_especial);
      pdata += (strlen(cabeza_especial)+1);
   }
   else
   {
      *pdata = 0;
      pdata++;
   }
   if (rw_extranumuser)
   {
      *pdata = 1;
      pdata++;
      xferbuf.hdr.length += sizeof(int);
      memcpy_int(pdata,rw_extranumuser,sizeof(int));
      pdata += sizeof(int);
   }
   else
   {
      *pdata = 0;
      pdata++;
   }
   if (rw_usuario)
   {
      *pdata = 1;
      pdata++;
      xferbuf.hdr.length += (strlen(rw_usuario)+1);
      strcpy(pdata,rw_usuario);
      pdata += (strlen(rw_usuario)+1);
   }
   else
   {
      *pdata = 0;
      pdata++;
   }
   total_last_cabecera = xferbuf.hdr.length;
   if (total_last_cabecera < 1024)
   {
      memcpy(last_cabecera,(char *)&xferbuf,total_last_cabecera);
   }
   else
      total_last_cabecera = 0;
   senddatamessage(MySock.sock, &xferbuf);
   return(0);
}


int menusg_en_terminal(opcion,letras,pos,texto,defecto,lineas)
char *opcion[];
char *letras;
int pos;
char *texto;
int defecto;
int lineas;
{
   char *pdata;
   int n,i;
   char col[256];
   if (en_conexion_externa_stop) return(0);
   do {
      envio_situacion = 31;
      xferbuf.hdr.signature = RWNIDENTIFICACION;
      xferbuf.hdr.command = COMANDO_TERMINAL;
      xferbuf.data[0] = 31; /* menusg */
      
      pdata = xferbuf.data + 1;
      memcpy_int(pdata,&modo_de_validacion,sizeof(int));
      pdata += sizeof(int);
      n = strlen(letras);
      memcpy_int(pdata,&n,sizeof(int));
      pdata += sizeof(int);
      memcpy_int(pdata,&pos,sizeof(int));
      pdata += sizeof(int);
      memcpy_int(pdata,&defecto,sizeof(int));
      pdata += sizeof(int);
      memcpy_int(pdata,&lineas,sizeof(int));
      pdata += sizeof(int);
      memcpy_int(pdata,&menusg_virtual,sizeof(int));
      pdata += sizeof(int);
      
      n = 1 + (sizeof(int)*6);
      
      strcpy(pdata,letras);
      pdata += (strlen(letras)+1);
      n += (strlen(letras) + 1);
      
      strcpy(pdata,texto);
      pdata += (strlen(texto)+1);
      n += (strlen(texto)+1);
      
      for (i = 0;i < strlen(letras);i++)
      {
         strcpy(pdata,opcion[i]);
         pdata += (strlen(opcion[i])+1);
         n += (strlen(opcion[i])+1);
      }
      xferbuf.hdr.length = HDRSIZE + n;
      senddatamessage(MySock.sock, &xferbuf);
      if (leecontrol_en_terminal(col,sizeof(int)*4,NULL) < 0)
      {
         continue;
      }
      memcpy_int(&i,col,sizeof(int));
      memcpy_int(&_M_Cursor,col+sizeof(int),sizeof(int));
      memcpy_int(&_M_EXTRA_AVANCE,col+sizeof(int)*2,sizeof(int));
      memcpy_int(&menusg_virtual,col+(sizeof(int)*3),sizeof(int));
      return(i);
   } while(-1);
}

int wmenulee_en_terminal(nnom)
char *nnom;
{
   char *pdata;
   
   xferbuf.hdr.signature = RWNIDENTIFICACION;
   xferbuf.hdr.command = COMANDO_TERMINAL;
   xferbuf.data[0] = 32; /* wmenulee */
   xferbuf.hdr.length = HDRSIZE + 1 + strlen(nnom)+1;
   pdata = xferbuf.data+1;
   strcpy(pdata,nnom);
   pdata += (strlen(nnom)+1);
   if (dir_smenu)
   {
      strcpy(pdata,dir_smenu);
      pdata += (strlen(dir_smenu)+1);
      xferbuf.hdr.length += (strlen(dir_smenu)+1);
   }
   else
   {
      *pdata = 0;
      pdata++;
      xferbuf.hdr.length++;
   }
   if (dir_sgmenu)
   {
      strcpy(pdata,dir_sgmenu);
      pdata += (strlen(dir_sgmenu)+1);
      xferbuf.hdr.length += (strlen(dir_sgmenu)+1);
   }
   else
   {
      *pdata = 0;
      pdata++;
      xferbuf.hdr.length++;
   }
   senddatamessage(MySock.sock, &xferbuf);
   fichero_local_remoto = 1;
   return(0);
}

int wmenufinlee_en_terminal()
{
   xferbuf.hdr.signature = RWNIDENTIFICACION;
   xferbuf.hdr.command = COMANDO_TERMINAL;
   xferbuf.data[0] = 54;
   xferbuf.hdr.length = HDRSIZE + 1;
   senddatamessage(MySock.sock, &xferbuf);
   fichero_local_remoto = 0;
   return(0);
}

int wmenu_en_terminal(pim)
int pim;
{
   char *pdata;
   char tmp[50];

   int i,fe;
   do {
      envio_situacion = 33;
      xferbuf.hdr.signature = RWNIDENTIFICACION;
      xferbuf.hdr.command = COMANDO_TERMINAL;
      xferbuf.data[0] = 33; /* wmenu */
      xferbuf.hdr.length = HDRSIZE + 1 + (sizeof(int)*3);
      
      memcpy_int(xferbuf.data+1,&modo_de_validacion,sizeof(int));
      memcpy_int(xferbuf.data+1+sizeof(int),&pim,sizeof(int));
      memcpy_int(xferbuf.data+1+sizeof(int)*2,&wdef,sizeof(int));
      pdata = xferbuf.data+1+(sizeof(int)*3);
      if (dir_smenu)
      {
         strcpy(pdata,dir_smenu);
         pdata += (strlen(dir_smenu)+1);
         xferbuf.hdr.length += (strlen(dir_smenu)+1);
      }
      else
      {
         *pdata = 0;
         pdata++;
         xferbuf.hdr.length++;
      }
      if (dir_sgmenu)
      {
         strcpy(pdata,dir_sgmenu);
         pdata += (strlen(dir_sgmenu)+1);
         xferbuf.hdr.length += (strlen(dir_sgmenu)+1);
      }
      else
      {
         *pdata = 0;
         pdata++;
         xferbuf.hdr.length++;
      }
      
      if (agi_aplicacion)
      {
         strcpy(pdata,agi_aplicacion);
         pdata += (strlen(agi_aplicacion)+1);
         xferbuf.hdr.length += (strlen(agi_aplicacion)+1);
      }
      else
      {
         *pdata = 0;
         pdata++;
         xferbuf.hdr.length++;
      }
      
      if (agi_empresa)
      {
         strcpy(pdata,agi_empresa);
         pdata += (strlen(agi_empresa)+1);
         xferbuf.hdr.length += (strlen(agi_empresa)+1);
      }
      else
      {
         *pdata = 0;
         pdata++;
         xferbuf.hdr.length++;
      }
      
      if (agi_codi_empre)
      {
         strcpy(pdata,agi_codi_empre);
         pdata += (strlen(agi_codi_empre)+1);
         xferbuf.hdr.length += (strlen(agi_codi_empre)+1);
      }
      else
      {
         *pdata = 0;
         pdata++;
         xferbuf.hdr.length++;
      }
      
      senddatamessage(MySock.sock, &xferbuf);
      if (leecontrol_en_terminal(tmp,sizeof(int)*3,NULL) < 0)
      {
         continue;
      }
      memcpy_int(&i,tmp,sizeof(int));
      memcpy_int(&wdef,tmp+sizeof(int),sizeof(int));
	  memcpy_int(&fe,tmp+sizeof(int)*2,sizeof(int));	  
	  if (fe)
		  fuerza_empresa = 1;
      return(i);
   } while(-1);
}

int menulibera_en_terminal()
{
   
   xferbuf.hdr.signature = RWNIDENTIFICACION;
   xferbuf.hdr.command = COMANDO_TERMINAL;
   xferbuf.data[0] = 34; /* wmenu libera*/
   xferbuf.hdr.length = HDRSIZE + 1;
   senddatamessage(MySock.sock, &xferbuf);
   return(0);
}


int w_print_en_terminal(w,s)
VENT *w;
char *s;
{
   if (en_conexion_externa_stop) return(0);
   
   xferbuf.hdr.signature = RWNIDENTIFICACION;
   xferbuf.hdr.command = COMANDO_TERMINAL;
   xferbuf.data[0] = 35; /* w_print */
   xferbuf.hdr.length = HDRSIZE + 2 + sizeof(w) + sizeof(int) + strlen(s);
   memcpy_VENT(xferbuf.data+1,&w,sizeof(w));
   memcpy_int(xferbuf.data+1+sizeof(w),&Pcursor,sizeof(int));
   strcpy(xferbuf.data+1+sizeof(w)+sizeof(int),s);
   senddatamessage(MySock.sock, &xferbuf);
   return(0);
}


int w_mueve_terminal(w,p)
VENT *w;
int p;
{
   if (en_conexion_externa_stop) return(0);
   
   xferbuf.hdr.signature = RWNIDENTIFICACION;
   xferbuf.hdr.command = COMANDO_TERMINAL;
   xferbuf.data[0] = 36; /* w_mueve */
   xferbuf.hdr.length = HDRSIZE + 1 + sizeof(w) + sizeof(int);
   memcpy_VENT(xferbuf.data+1,&w,sizeof(w));
   memcpy_int(xferbuf.data+1+sizeof(w),&p,sizeof(int));
   senddatamessage(MySock.sock, &xferbuf);
   return(0);
}

int bi_cuadro_en_terminal(pi,pf,m)
int pi,pf,m;
{
   if (en_conexion_externa_stop) return(0);
   
   xferbuf.hdr.signature = RWNIDENTIFICACION;
   xferbuf.hdr.command = COMANDO_TERMINAL;
   xferbuf.data[0] = 37; /* bi_cuadro */
   xferbuf.hdr.length = HDRSIZE + 1 + sizeof(int) * 3;
   memcpy_int(xferbuf.data+1,&pi,sizeof(int));
   memcpy_int(xferbuf.data+1+sizeof(int),&pf,sizeof(int));
   memcpy_int(xferbuf.data+1+sizeof(int)*2,&m,sizeof(int));
   senddatamessage(MySock.sock, &xferbuf);
   return(0);
}

int printa_grafico_en_terminal(nom_inf)
char *nom_inf;
{
   if (en_conexion_externa_stop) return(0);
   
   xferbuf.hdr.signature = RWNIDENTIFICACION;
   xferbuf.hdr.command = COMANDO_TERMINAL;
   xferbuf.data[0] = 39; /* grafico impresora */
   xferbuf.hdr.length = HDRSIZE + 2 + strlen(nom_inf);
   strcpy(xferbuf.data+1,nom_inf);
   senddatamessage(MySock.sock, &xferbuf);
   return(0);
}


int pregu_impre_terminal(t,alfabetica,fispo)
int t;
char *alfabetica;
char *fispo;
{
   char *pdata;
   int r;
   char tmp[256];
   
   strcpy(tmp,fispo);  
   do 
   {
      
      envio_situacion = 40;
      xferbuf.hdr.signature = RWNIDENTIFICACION;
      xferbuf.hdr.command = COMANDO_TERMINAL;
      xferbuf.data[0] = 40; /* pregunta impresora */
      xferbuf.hdr.length = HDRSIZE + 3 + strlen(fispo) + sizeof(int)*2;
      pdata = xferbuf.data+1;
      memcpy_int(pdata,&modo_de_validacion,sizeof(int));
      pdata += sizeof(int);
      memcpy_int(pdata,&t,sizeof(int));
      pdata += sizeof(int);
      strcpy(pdata,fispo);
      pdata += (strlen(fispo)+1);
      if (!alfabetica)
         *pdata = 0;
      else
      {
         *pdata = 1;
         pdata++;
         strcpy(pdata,alfabetica);
         xferbuf.hdr.length += (strlen(alfabetica)+1);
      }
      senddatamessage(MySock.sock, &xferbuf);
      
      if (leecontrol_en_terminal(NULL,0,NULL) < 0)
      {
         strcpy(fispo,tmp);  
         continue;
      }
      pdata = xrcbbuf.data+1;
      memcpy_int(&r,pdata,sizeof(int));
      pdata += sizeof(int);
      if (alfabetica)
      {
         strcpy(alfabetica,pdata);
      }
      
      if (r >= 0)
      {
         pdata += (strlen(pdata)+1);
         memcpy(nombredev,pdata,81);
         pdata += 81;
         memcpy_int(&i_anchocarro,pdata,sizeof(int));
         pdata += sizeof(int);
         memcpy(oncompri,pdata,17);
         pdata += 17;
         memcpy(offcompri,pdata,17);
         pdata += 17;
         memcpy(onnegrita,pdata,17);
         pdata += 17;
         memcpy(offnegrita,pdata,17);
         pdata += 17;
         memcpy(onexpande,pdata,17);
         pdata += 17;
         memcpy(offexpande,pdata,17);
         pdata += 17;
         memcpy(inicializo,pdata,37);
         pdata += 37;
         memcpy(finalizo,pdata,37);
         pdata += 37;
         memcpy_int(&i_nlineas,pdata,sizeof(int));
         pdata += sizeof(int);
         memcpy(saltopag,pdata,17);
         pdata += 17;
         memcpy(otrocar,pdata,17);
         pdata += 17;
         memcpy_int(&es_esclava,pdata,sizeof(int));
         pdata += sizeof(int);
         memcpy_int(&no_reset,pdata,sizeof(int));
         pdata += sizeof(int);	
         if (r == 2)
         {			  
            strcpy(fispo,pdata);			  
            if (fispo[0] == '{')
            {
               if (ext_carga_dll)
               {
                  _imp = (FILE *)(unsigned long)0x00000004L;
                  tmp[0] = 0;   				
                  (*ext_carga_dll)(tmp);
                  /*carga_por_dll(tmp);*/
               }
            }
            else
            {
               pdata += (strlen(pdata)+1);
               if (strlen(pdata))
               {			    
                  strcpy(_ejspool,pdata);
               }
               else
                  _ejspool[0] = 0;
               pdata += (strlen(pdata)+1);
               if (strlen(pdata))
                  sprintf(_fispool,"%s%s",rw_directorio("spool/"),pdata);
               else
                  _fispool[0] = 0;
               if (_fispool[0])
               {
                  strcpy(fispo,_fispool);
                  if ((_imp = fopen(fispo,"r")) != NULL) 
                  {
                     fclose(_imp);
                     _imp = NULL;                    
                     r = -1;
                     aviso(0,__Texto[131]);
                     strcpy(fispo,tmp);
                     continue;
                  }
                  strcat(_ejspool," ");
                  strcat(_ejspool,_fispool);
               }
               if ((_imp = fopen(fispo,"w")) == NULL) 
               {
                  if (alfabetica) alfabetica[0] = 0;
                  _imp = (FILE *)0;
                  r = -1;
                  aviso(0,__Texto[132]);
                  strcpy(fispo,tmp);
                  continue;
               }
            }
			if (_imp != (FILE *)(unsigned long)0x00000004L) {
				i_inicio(_imp);
				i_reset(_imp);
			}
            r = 0;
         }
         else
         {
            /* redireccionada */
            _imp = (FILE *)(unsigned long)0x00000003L;
         }
      }   
      break;
   } while(-1);
   return(r);
}

int fin_printer_terminal(im)
FILE *im;
{
   if (im == _imp)
   {
      /* estos no se paran!!*/
      xferbuf.hdr.signature = RWNIDENTIFICACION;
      xferbuf.hdr.command = COMANDO_TERMINAL;
      xferbuf.data[0] = 41; /* cierra impresora */
      xferbuf.hdr.length = HDRSIZE + 1 + 300;
	  memcpy(xferbuf.data+1,numerofax,300);
      senddatamessage(MySock.sock, &xferbuf);
      _fispool[0] = 0;
      _ejspool[0] = 0;  
   }
   if ((unsigned long)im > (unsigned long)0x00000004L)
      fclose(im);
   return(0);
}

int printa_en_terminal(texto,len)
char *texto;
int len;
{
#if defined(FGVUNIX) || defined(WATCOMNLM)
   int i;
#endif
   /* estos no se paran!!*/
   xferbuf.hdr.signature = RWNIDENTIFICACION;
   xferbuf.hdr.command = COMANDO_TERMINAL;
   xferbuf.data[0] = 42; /* imprime esto */
   xferbuf.hdr.length = HDRSIZE + 2 + sizeof(int);
   memcpy_int(xferbuf.data+1,&len,sizeof(int));
   if (len < 2)
			{
      memcpy(xferbuf.data+1+sizeof(int),texto,strlen(texto)+1);
      xferbuf.hdr.length += (strlen(texto)+1);
			}
   else
			{
      memcpy(xferbuf.data+1+sizeof(int),texto,len);
      xferbuf.hdr.length += len;
			}
   senddatamessage(MySock.sock, &xferbuf);
#if defined(FGVUNIX) || defined(WATCOMNLM)
#ifdef FGVBSD
   usleep(1000);
#endif
#else
   Sleep(1);
#endif  
   return(len);
}

int ejecutae_terminal()
{
			char *pdata;
         int n;
         char c;
         /* OJO NETCON EN LA MISMA IP QUE EL SERVIDOR */
         do {
            envio_situacion = 43;
            xferbuf.hdr.signature = RWNIDENTIFICACION;
            xferbuf.hdr.command = COMANDO_TERMINAL;
            xferbuf.data[0] = 43; /* aviso de cambio de servidor */
            xferbuf.hdr.length = HDRSIZE + 1 + sizeof(int);  
            memcpy_int(xferbuf.data+1,&server_id,sizeof(int));
            senddatamessage(MySock.sock, &xferbuf);
            if (leecontrol_en_terminal(NULL,0,NULL) < 0 || xrcbbuf.data[0] != 14)
            {
               continue;
            }
            pdata = xrcbbuf.data+1;
            memcpy_int(&n,pdata,sizeof(int));
            if (n == -1)
            {
               return(-1);
            }
            pdata += sizeof(int);  
            /* es lo ultimo que hace */			
            envia_control_a_terminal("",0,3);
            establece_desconexion(0);
            server_request[2] = '-';			
            return(0);
         } while(-1);
}

int entrada_usuario_terminal()
{
			char *pdata;
         xferbuf.hdr.signature = RWNIDENTIFICACION;
         xferbuf.hdr.command = COMANDO_TERMINAL;
         xferbuf.data[0] = 44; /* usuario */
         xferbuf.hdr.length = HDRSIZE + 2 + sizeof(int)*2 + strlen(rw_user);
         
         pdata = xferbuf.data+1;
         memcpy_int(pdata,&user_who,sizeof(int));
         pdata += sizeof(int);
         memcpy_int(pdata,&subnumero_de_usuario,sizeof(int));
         pdata += sizeof(int);
         strcpy(pdata,rw_user);
         
         senddatamessage(MySock.sock, &xferbuf);
         return(0);
}

int salida_usuario_terminal()
{
			char *pdata;
         xferbuf.hdr.signature = RWNIDENTIFICACION;
         xferbuf.hdr.command = COMANDO_TERMINAL;
         xferbuf.data[0] = 45; /* salida usuario */
         xferbuf.hdr.length = HDRSIZE + 1;
         senddatamessage(MySock.sock, &xferbuf);
}

int recurre_boton_terminal(m)
int m;
{
			xferbuf.hdr.signature = RWNIDENTIFICACION;
         xferbuf.hdr.command = COMANDO_TERMINAL;
         xferbuf.data[0] = 46; /* recurre boton */
         xferbuf.hdr.length = HDRSIZE + 1 + sizeof(int);
         memcpy_int(xferbuf.data+1,&m,sizeof(int));
         senddatamessage(MySock.sock, &xferbuf);
}


int fin_botones__terminal()
{
   xferbuf.hdr.signature = RWNIDENTIFICACION;
   xferbuf.hdr.command = COMANDO_TERMINAL;
   xferbuf.data[0] = 47; /*  */
   xferbuf.hdr.length = HDRSIZE + 1;
   senddatamessage(MySock.sock, &xferbuf);
}

int prepara_menusg_botones_terminal()
{
   xferbuf.hdr.signature = RWNIDENTIFICACION;
   xferbuf.hdr.command = COMANDO_TERMINAL;
   xferbuf.data[0] = 48; /*  */
   xferbuf.hdr.length = HDRSIZE + 1;
   senddatamessage(MySock.sock, &xferbuf);
}

int pon_mapa_raton_terminal(pi,pf,id,estado)
int pi,pf,id,estado;
{
   char *pdata;
   xferbuf.hdr.signature = RWNIDENTIFICACION;
   xferbuf.hdr.command = COMANDO_TERMINAL;
   xferbuf.data[0] = 49; /*  */
   xferbuf.hdr.length = HDRSIZE + 1 + sizeof(int) * 4;
   pdata = xferbuf.data + 1;
   memcpy_int(pdata,&pi,sizeof(int));
   pdata += sizeof(int);
   memcpy_int(pdata,&pf,sizeof(int));
   pdata += sizeof(int);
   memcpy_int(pdata,&id,sizeof(int));
   pdata += sizeof(int);
   memcpy_int(pdata,&estado,sizeof(int));
   pdata += sizeof(int);
   senddatamessage(MySock.sock, &xferbuf);
}

int anula_mapa_raton_terminal(id)
int id;
{
   char *pdata;
   xferbuf.hdr.signature = RWNIDENTIFICACION;
   xferbuf.hdr.command = COMANDO_TERMINAL;
   xferbuf.data[0] = 50; /*  */
   xferbuf.hdr.length = HDRSIZE + 1 + sizeof(int);
   pdata = xferbuf.data + 1;
   memcpy_int(pdata,&id,sizeof(int));
   pdata += sizeof(int);
   senddatamessage(MySock.sock, &xferbuf);
}

int recurre_raton_terminal(m)
int m;
{
   char *pdata;
   xferbuf.hdr.signature = RWNIDENTIFICACION;
   xferbuf.hdr.command = COMANDO_TERMINAL;
   xferbuf.data[0] = 51; /*  */
   xferbuf.hdr.length = HDRSIZE + 1 + sizeof(int);
   pdata = xferbuf.data + 1;
   memcpy_int(pdata,&m,sizeof(int));
   pdata += sizeof(int);
   senddatamessage(MySock.sock, &xferbuf);
}


int ptec_en_terminal(m)
int m;
{
   char *pdata;
   xferbuf.hdr.signature = RWNIDENTIFICACION;
   xferbuf.hdr.command = COMANDO_TERMINAL;
   xferbuf.data[0] = 52; /*  */
   xferbuf.hdr.length = HDRSIZE + 1 + sizeof(int);
   pdata = xferbuf.data + 1;
   memcpy_int(pdata,&m,sizeof(int));
   pdata += sizeof(int);
   senddatamessage(MySock.sock, &xferbuf);
}

int envia_control_a_terminal(pdata,len,comando)
char *pdata;
int len;
int comando;
{
   xferbuf.hdr.signature = RWNIDENTIFICACION;
   xferbuf.hdr.command = CONTROL_DE_SERVIDOR;  
   xferbuf.hdr.length = HDRSIZE + 1 + len;
   xferbuf.data[0] = comando;
   memcpy(xferbuf.data+1,pdata,len);
   senddatamessage(MySock.sock, &xferbuf);
   return(len);
}

int envia_comando_a_terminal(pdata,len,comando)
char *pdata;
int len;
int comando;
{
   xferbuf.hdr.signature = RWNIDENTIFICACION;
   xferbuf.hdr.command = COMANDO_TERMINAL;
   xferbuf.hdr.length = HDRSIZE + 1 + len;
   xferbuf.data[0] = comando;
   memcpy(xferbuf.data+1,pdata,len);
   senddatamessage(MySock.sock, &xferbuf);
   return(len);
}

void pon_tecla_timeout_terminal(a,b,c,d)
int a,b,c,d;
{
   char *pdata;
   xferbuf.hdr.signature = RWNIDENTIFICACION;
   xferbuf.hdr.command = COMANDO_TERMINAL;
   xferbuf.data[0] = 56;
   xferbuf.hdr.length = HDRSIZE + 1 + sizeof(int) * 4;
   pdata = xferbuf.data+1;
   memcpy_int(pdata,&a,sizeof(int));
   pdata += sizeof(int);
   memcpy_int(pdata,&b,sizeof(int));
   pdata += sizeof(int);
   memcpy_int(pdata,&c,sizeof(int));
   pdata += sizeof(int);
   memcpy_int(pdata,&d,sizeof(int));
   senddatamessage(MySock.sock, &xferbuf);
}


int opcion_ej_en_terminal(pro,codigo)
char *pro,*codigo;
{
   unsigned char *pdata;
   int bien;   
   
   do 
   {
      envio_situacion = 57;
      xferbuf.hdr.signature = RWNIDENTIFICACION;
      xferbuf.hdr.command = COMANDO_TERMINAL;
      xferbuf.data[0] = 57;  
      pdata = xferbuf.data+1;
      strcpy(pdata,pro);
      pdata += (strlen(pro)+1);
	  if (*pro == ':')
	  {
		  *pdata = 0;
	  }
	  else
	  {
		  strcpy(pdata,nomapli);	  
	  }
      pdata += (strlen(nomapli)+1);
      if (codigo)
      {
         *pdata = 1;
         pdata++;
         strcpy(pdata,codigo);
         pdata += (strlen(codigo)+1);
      }
      else
      {
         *pdata = 0;
         pdata++;
      }
	  /* informacion de proteccion */
	  *pdata = 2;
	  pdata++;  
			  

      memcpy(pdata,&nume_serie,sizeof(nume_serie));
	  pdata += sizeof(nume_serie);
      memcpy(pdata,&nume_serie_real,sizeof(nume_serie_real));
	  pdata += sizeof(nume_serie_real);
      memcpy(pdata,local_term,sizeof(local_term));
	  pdata += sizeof(local_term);
      memcpy(pdata,local_term_,sizeof(local_term_));
	  pdata += sizeof(local_term_);
      memcpy(pdata,local_acce_,sizeof(local_acce_));
	  pdata += sizeof(local_acce_);
      memcpy(pdata,&nume_code,sizeof(nume_code));
	  pdata += sizeof(nume_code);
      memcpy(pdata,&posicion,sizeof(posicion));
	  pdata += sizeof(posicion);
	  memcpy(pdata,&nume_mochila,sizeof(nume_mochila));
	  pdata += sizeof(nume_mochila);
	  *pdata = 0;
	  pdata++;	  

      xferbuf.hdr.length = HDRSIZE + (int)(pdata - xferbuf.data);
      senddatamessage(MySock.sock, &xferbuf);
      if (leecontrol_en_terminal(&bien,sizeof(int),NULL) < 0)
      {
         continue;
      }
	  moto_short(&bien);
      return(bien);
   } while(-1);
}


int funcion_windows_cliente(ff,p,len)
int ff;
char *p;
int len;
{
   char *pdata;
   int f,modeless;
   int nr;
   if (remoto_que_sistema_es != 24 && remoto_que_sistema_es != 11 && remoto_que_sistema_es != 13)
   {
      return(-1);
   }
   
   f = ff % 1000;
   modeless = ff / 1000;
   
   do {
	  nr = 0;
      envio_situacion = 61;
      xferbuf.hdr.signature = RWNIDENTIFICACION;
      xferbuf.hdr.command = COMANDO_TERMINAL;
      xferbuf.data[0] = 61;
      xferbuf.hdr.length = HDRSIZE + 1 + sizeof(int) + len;
      pdata = xferbuf.data+1;
      memcpy_int(pdata,&f,sizeof(int));
      pdata += sizeof(int);
      memcpy(pdata,p,len);
      senddatamessage(MySock.sock, &xferbuf);
      if (f >= 1/* && f <= 6*/ && !modeless)
      {
         if ((nr = leecontrol_en_terminal(p,-1,NULL)) < 0)
         {
            continue;
         } 
      }
      return(nr);
   } while(-1);
return(0);   
}

int trasmision_de_ficheros()
{
   char **_dir;
   char _buf_[20000];
   char eldir[256];
   int i;
   int nolee = 0,erro = 0;
   char **__dir;
   static int t_no_recursion_ = 0;
   
   if (t_no_recursion_) return 0;
   
   t_no_recursion_ = 1;
   
   _buf_[0] = 2;
   _buf_[1] = 2;
   _buf_[2] = 0;
   funcion_windows_cliente(33,_buf_,3);
   do
   {
      if (_buf_[0] == 1) {t_no_recursion_ = 0;return 0;}
      
      if (_buf_[0] == 0)
      {
         strcpy(eldir,rw_directorio(""));
      }
      else
      {
         if (_buf_[0] == 2)
         {
            erro = rw_envia_fichero(_buf_+1,777);
            nolee = 1;
         }
         else
            strcpy(eldir,_buf_);
      }
      if (!nolee)
      {
         _dir = lee_direct(eldir,3);
         _buf_[0] = 3;
         _buf_[1] = 3;
         strcpy(_buf_+2,eldir);
         i = strlen(_buf_);
         _buf_[i] = 1;
         i++;
         _buf_[i] = 0;
         if (_dir)
         {	
            for (__dir = _dir;*__dir;__dir++)
            {
               strcpy(_buf_+i,*__dir);
               i += strlen(*__dir);
               _buf_[i] = 1;
               i++;
               _buf_[i] = 0;
            }
            libera_dir(_dir);
         }
      }
      else
      {
         nolee = 0;
         _buf_[0] = 3;
         _buf_[1] = 3;
         _buf_[2] = 1;
         if (erro)
            _buf_[3] = 1;
         else
            _buf_[3] = 0;
         _buf_[4] = 0;
      }
      funcion_windows_cliente(33,_buf_,strlen(_buf_)+1);
   } while(-1);
   t_no_recursion_ = 0;
   return 0;
}


int llama_a_despacho_en_lineas(int pi,int pf,int modo)
{
   char tmp[100];
   char *pdata = tmp;
   int f,j;
   
   if (!en_conexion_externa) return(-1);
   
   f = DESPACHO_EN_LINEAS;
   memcpy_int(pdata,&f,sizeof(int));
   pdata += sizeof(int);
   j = modo;
   memcpy_int(pdata,&j,sizeof(int));
   pdata += sizeof(int);
   *pdata = 1;
   pdata++;
   memcpy_int(pdata,&pi,sizeof(int));
   pdata += sizeof(int);
   memcpy_int(pdata,&pf,sizeof(int));
   pdata += sizeof(int);
   funcion_windows_cliente(1010,tmp,sizeof(int)*4+1);
   return(0);
}

void llama_a_despachador(int f,int parametro,char *puntero)
{
   char tmp[101300];
   int len;
   
   if (!en_conexion_externa) return;
   
   if (f == DESPACHO_OMENU) return;
   
   if (f == DESPACHO_FUNCIONES_IMPRESION && (parametro == 998 || parametro == 8)) {
	   memcpy_int(tmp,&f,sizeof(int));
	   memcpy_int(tmp+sizeof(int),&parametro,sizeof(int));
	   len = sizeof(int)*2;
		tmp[len] = 1;
		len++;
		memcpy(tmp+len,puntero,300);
		len += 300;
		funcion_windows_cliente(10,tmp,len);
		memcpy(puntero,tmp,300);
		/*free(pilatmp);*/
        return;
   }


   memcpy_int(tmp,&f,sizeof(int));
   memcpy_int(tmp+sizeof(int),&parametro,sizeof(int));
   len = sizeof(int)*2;


   if (f == DESPACHO_BINARIO)
   {
   long tam,envio,porenviar = 0,modo = 0;
   char parcial = 0;   
   char *p = puntero;
     /* si el tamaño es demasiado grande recomponerlos automaticamente */
	 if (puntero)
	    {
	    tmp[len] = 1;
		}
	else
	    {
		tmp[len] = 0;
		}
	 len++;

	 if (puntero)
	    {       

        memcpy_long(&tam,p,sizeof(long));
        p += sizeof(long);		
        memcpy_long(&envio,p,sizeof(long)); /* longitud esperada de vuelta */
        p += sizeof(long);		
        memcpy_long(&modo,p,sizeof(long));  /* flags */
        p += sizeof(long);		

		memcpy_long(tmp+len,&tam,sizeof(long));
		len += sizeof(long);		

		tam -= sizeof(long);
        tam -= sizeof(long);
		tam -= sizeof(long);

        memcpy_long(tmp+len,&envio,sizeof(long));
		len += sizeof(long);		
        memcpy_long(tmp+len,&modo,sizeof(long));
		len += sizeof(long);		

	    if (tam > 10000)
	      {
	      envio = 10000;
		  parcial = 1;
		  }
	    else
	      {
	      envio = tam;
		  parcial = 0;
		  }

	    tmp[len] = parcial;
	    len++;
		if (parcial)
		   {
		   memcpy_long(tmp+len,&envio,sizeof(long));
		   len += sizeof(long);
		   }
		memcpy(tmp+len,p,envio);
        len += envio;
		p += envio;
		porenviar = tam - envio;
		}

	 funcion_windows_cliente(1010,tmp,len);     

	 if (!puntero || (!(modo&1) && !porenviar))
	    {
		return;
		}

    while (porenviar)
	    {
	    if (porenviar > 10000)
	      {
	      envio = 10000;		  
		  }
	    else
	      {
	      envio = porenviar;		  
		  }       
        envia_control_a_terminal(p,envio,66);
		p += envio;
		porenviar -= envio;
		}


     if ((modo & 1))
	    {
		int waitfor = 66;
		int recibidos;
		char *pp;

		p = puntero;

		do
		  {
          if ((recibidos = leecontrol_en_terminal(NULL,0,&waitfor)) < (sizeof(long)+1)) return;
		  } while(waitfor != 66);
		pp = xrcbbuf.data+1;
        memcpy_long(&tam,pp,sizeof(long));
		pp += sizeof(long);
		parcial = *pp;
		pp++;
		memcpy_long(p,&tam,sizeof(long));
        tam -= sizeof(long);
		p += sizeof(long);
		if (parcial)
		   {
		   memcpy_long(&envio,pp,sizeof(long));
		   pp += sizeof(long);
		   }
		else
		   envio = tam;
#ifdef FGVDOS
        /*ASSERT( envio == (recibidos-(pp-xrcbbuf.data+1)) );*/
#endif
        memcpy(p,pp,envio);
		p += envio;
		tam -= envio;
		while(tam && parcial)
		   {
		   do
		     {
             if ((recibidos = leecontrol_en_terminal(NULL,0,&waitfor)) < 0) return;
		     } while(waitfor != 66);
		   memcpy(p,xrcbbuf.data+1,recibidos);
		   p += recibidos;
		   tam -= recibidos;
		   }
		}

	 return;
   }

   
   if (f == DESPACHO_PANTALLAS && parametro > 100 && puntero)
   {
      int i,ni = parametro - 100;
      char **arrai = (char **)puntero;
      
      tmp[len] = 1;
      len++;
      
      for (i = 0;i < ni;i++)
      {
         strcpy(tmp+len,arrai[i]);
         len += (strlen(arrai[i])+1);
      }
   }
   else
      if (f == DESPACHO_MI_CONTROL)
      {
         tmp[len] = 1;
         len++;	 
         memcpy(tmp+len,puntero,1024);
         len += 1024;
         funcion_windows_cliente(10,tmp,len); /* requiere confirmacion */
         memcpy_int(puntero,tmp,sizeof(int));
         return;
      }
      else
         if (f == DESPACHO_EDITA)
         {
            int l;
            tmp[len] = 1;
            len++;
            memcpy_int(&l,puntero,sizeof(int));
            if (l <= 0)
            {
                l = sizeof(int)+strlen(puntero+sizeof(int))+1;
            }
            memcpy(tmp+len,puntero,l);
            len += l;
            funcion_windows_cliente(10,tmp,len); /* requiere confirmacion */
            memcpy_int(puntero,tmp,sizeof(int));
            strcpy(puntero+sizeof(int),tmp+sizeof(int));
            return;
         }
         else
         if (f == DESPACHO_USUARIOS)
         {
            tmp[len] = 1;
            len++;
            strcpy(tmp+len,puntero);
            len += (strlen(puntero)+1);
            funcion_windows_cliente(10,tmp,len); /* requiere confirmacion */
            strcpy(puntero,tmp);
            return;
         }
         else
         {
            if (!puntero)
            {
               tmp[len] = 0;
               len++;
            }
            else
            {   
               tmp[len] = 1;
               len++;
			   if (f == DESPACHO_PROGRESO)
			   {
				   memcpy(tmp+len,puntero,sizeof(long));
				   len += sizeof(long);
			   }
			   else
			   {
				   strcpy(tmp+len,puntero);
				   len += (strlen(puntero)+1);
			   }
            }
         }


	     if (f == DESPACHO_FOCOCONTROL || f == DESPACHO_MULTIPLE || f == DESPACHO_MENUITEM || f == DESPACHO_INICIO_TRAZA || f == DESPACHO_FIN_TRAZA || f == DESPACHO_PUNTO_TRAZA || f == DESPACHO_ENLACEWORD || f == DESPACHO_TRAEFICHERO || f == DESPACHO_BROWSE || f == DESPACHO_CONSULTA_REMOTA || (f == DESPACHO_SUPERUSUARIO && puntero))
		 {
		    int rr = funcion_windows_cliente(10,tmp,len);
			if (rr > 0 && f != DESPACHO_FOCOCONTROL)
			{
				memcpy(puntero,tmp,rr);
			}
		 }
		 else
		 {
            funcion_windows_cliente(1010,tmp,len);
		 }
}

FILE *remoto_fopen(tmp,modo,flag)
char *tmp;
char *modo;
int flag;
{
   FILE *ff;
   char *pdata;
   do {
      envio_situacion = 62;
      xferbuf.hdr.signature = RWNIDENTIFICACION;
      xferbuf.hdr.command = COMANDO_TERMINAL;
      xferbuf.data[0] = 62;
      xferbuf.hdr.length = HDRSIZE + 3 + strlen(tmp) + strlen(modo) + sizeof(int);
      pdata = xferbuf.data+1;
      memcpy_int(pdata,&flag,sizeof(int));
      pdata += sizeof(int);
      strcpy(pdata,tmp);
      pdata += (strlen(tmp)+1);
      strcpy(pdata,modo);
      senddatamessage(MySock.sock, &xferbuf);
      if (leecontrol_en_terminal(&ff,sizeof(ff),NULL) < 0)
      {
         continue;
      }      
      return(ff);
   } while(-1);
}

int remoto_fclose(ff)
FILE *ff;
{
   int r;
   char *pdata;
   do {
      envio_situacion = 63;
      xferbuf.hdr.signature = RWNIDENTIFICACION;
      xferbuf.hdr.command = COMANDO_TERMINAL;
      xferbuf.data[0] = 63;
      xferbuf.hdr.length = HDRSIZE + 1 + sizeof(FILE *);
      pdata = xferbuf.data+1;
      memcpy(pdata,&ff,sizeof(FILE *));
      senddatamessage(MySock.sock, &xferbuf);
      if (leecontrol_en_terminal(&r,sizeof(int),NULL) < 0)
      {
         continue;
      }      
	  moto_short(&r);
      return(r);
   } while(-1);
}

char *remoto_lee_linea(tmp,n,ff)
char *tmp;
int n;
FILE *ff;
{
   char *r;
   char *pdata;
   do {
      envio_situacion = 64;
      xferbuf.hdr.signature = RWNIDENTIFICACION;
      xferbuf.hdr.command = COMANDO_TERMINAL;
      xferbuf.data[0] = 64;
      xferbuf.hdr.length = HDRSIZE + 1 + sizeof(int) + sizeof(FILE *);
      pdata = xferbuf.data+1;
      memcpy_int(pdata,&n,sizeof(int));
      pdata += sizeof(int);
      memcpy(pdata,&ff,sizeof(FILE *));
      senddatamessage(MySock.sock, &xferbuf);
      if (leecontrol_en_terminal(&r,sizeof(char *),NULL) < 0)
      {
         continue;
      }     
      if (r)
      {
         strcpy(tmp,xrcbbuf.data+1+sizeof(char *));
         r = tmp;
      }  
      return(r);
   } while(-1);   
}


int remoto_graba_linea(tmp,ff)
char *tmp;
FILE *ff;
{
   int r;
   char *pdata;
   do {
      envio_situacion = 75;
      xferbuf.hdr.signature = RWNIDENTIFICACION;
      xferbuf.hdr.command = COMANDO_TERMINAL;
      xferbuf.data[0] = 75;
      xferbuf.hdr.length = HDRSIZE + 1 + sizeof(FILE *) + strlen(tmp) + 1;
      pdata = xferbuf.data+1;
      memcpy(pdata,&ff,sizeof(FILE *));
      pdata += sizeof(FILE *);
      strcpy(pdata,tmp);

      senddatamessage(MySock.sock, &xferbuf);
      if (leecontrol_en_terminal(&r,sizeof(int),NULL) < 0)
      {
         continue;
      }     
	  moto_short(&r);
      return(r);
   } while(-1);   
}


long remoto_fseek(ff,pos,modo)
FILE *ff;
long pos;
int modo;
{
   
   char *pdata;
   do {
      envio_situacion = 76;
      xferbuf.hdr.signature = RWNIDENTIFICACION;
      xferbuf.hdr.command = COMANDO_TERMINAL;
      xferbuf.data[0] = 76;
      xferbuf.hdr.length = HDRSIZE + 1 + sizeof(FILE *) + sizeof(long) + sizeof(int);
      pdata = xferbuf.data+1;
      memcpy(pdata,&ff,sizeof(FILE *));
      pdata += sizeof(FILE *);
      memcpy_int(pdata,&modo,sizeof(int));
      pdata += sizeof(int);
      memcpy_long(pdata,&pos,sizeof(long));

      senddatamessage(MySock.sock, &xferbuf);
      if (leecontrol_en_terminal(&pos,sizeof(long),NULL) < 0)
      {
         continue;
      }
	  moto_long(&pos);
      return(pos);
   } while(-1);   
}


int remoto_rw_open_(tmp,flag,flg)
char *tmp;
int flag;
int flg;
{
   int r;
   char *pdata;
   do {
      envio_situacion = 65;
      xferbuf.hdr.signature = RWNIDENTIFICACION;
      xferbuf.hdr.command = COMANDO_TERMINAL;
      xferbuf.data[0] = 65;
      xferbuf.hdr.length = HDRSIZE + 2 + sizeof(int)*2 + strlen(tmp);
      pdata = xferbuf.data+1;
      memcpy_int(pdata,&flg,sizeof(int));
      pdata += sizeof(int);
      memcpy_int(pdata,&flag,sizeof(int));
      pdata += sizeof(int);
      strcpy(pdata,tmp);
      senddatamessage(MySock.sock, &xferbuf);
      if (leecontrol_en_terminal(&r,sizeof(int),NULL) < 0)
      {
         continue;
      }     
	  moto_short(&r);
      return(r);
   } while(-1);   
}

int remoto_rw_write_(file,regi,tot)
int file;
char *regi;
int tot;
{
   int r;
   char *pdata;
   do {
      envio_situacion = 66;
      xferbuf.hdr.signature = RWNIDENTIFICACION;
      xferbuf.hdr.command = COMANDO_TERMINAL;
      xferbuf.data[0] = 66;
      xferbuf.hdr.length = HDRSIZE + 1 + sizeof(int)*2 + tot;
      pdata = xferbuf.data+1;
      memcpy_int(pdata,&tot,sizeof(int));
      pdata += sizeof(int);
      memcpy_int(pdata,&file,sizeof(int));
      pdata += sizeof(int);
      memcpy(pdata,regi,tot);
      senddatamessage(MySock.sock, &xferbuf);
      if (leecontrol_en_terminal(&r,sizeof(int),NULL) < 0)
      {
         continue;
      }     
	  moto_short(&r);
      return(r);
   } while(-1);   
}


int remoto_rw_read_(file,regi,tot)
int file;
char *regi;
int tot;
{
   int r;
   char *pdata;   
   do {
      envio_situacion = 73;
      xferbuf.hdr.signature = RWNIDENTIFICACION;
      xferbuf.hdr.command = COMANDO_TERMINAL;
      xferbuf.data[0] = 73;
      xferbuf.hdr.length = HDRSIZE + 1 + sizeof(int)*2;
      pdata = xferbuf.data+1;
      memcpy_int(pdata,&tot,sizeof(int));
      pdata += sizeof(int);
      memcpy_int(pdata,&file,sizeof(int));
      pdata += sizeof(int);      
      senddatamessage(MySock.sock, &xferbuf);
      if (leecontrol_en_terminal(&r,sizeof(int),NULL) < 0)
      {
         continue;
      }      
	  moto_short(&r);
      if (r > 0)
      {
          memcpy(regi,xrcbbuf.data+1+sizeof(int),r);
      }
      return(r);
   } while(-1);   
}



long remoto_rw_lseek_(file,regi,tot)
int file;
long regi;
int tot;
{
   int r;
   char *pdata;
   do {
      envio_situacion = 74;
      xferbuf.hdr.signature = RWNIDENTIFICACION;
      xferbuf.hdr.command = COMANDO_TERMINAL;
      xferbuf.data[0] = 74;
      xferbuf.hdr.length = HDRSIZE + 1 + sizeof(int)*2 + sizeof(long);
      pdata = xferbuf.data+1;
      memcpy_int(pdata,&tot,sizeof(int));
      pdata += sizeof(int);
      memcpy_int(pdata,&file,sizeof(int));
      pdata += sizeof(int);      
      memcpy_long(pdata,&regi,sizeof(long));
      pdata += sizeof(long);      
      senddatamessage(MySock.sock, &xferbuf);
      if (leecontrol_en_terminal(&regi,sizeof(long),NULL) < 0)
      {
         continue;
      }
	  moto_long(&regi);
      return(regi);
   } while(-1);   
}


int remoto_rw_close_(file)
int file;
{
   int r;
   char *pdata;
   do {
      envio_situacion = 67;
      xferbuf.hdr.signature = RWNIDENTIFICACION;
      xferbuf.hdr.command = COMANDO_TERMINAL;
      xferbuf.data[0] = 67;
      xferbuf.hdr.length = HDRSIZE + 1 + sizeof(int);
      pdata = xferbuf.data+1;
      memcpy_int(pdata,&file,sizeof(int));
      senddatamessage(MySock.sock, &xferbuf);
      if (leecontrol_en_terminal(&r,sizeof(int),NULL) < 0)
      {
         continue;
      }     
	  moto_short(&r);
      return(r);
   } while(-1);   
}

int remoto_ejecutas(tmp)
char *tmp;
{
   int r;
   char *pdata;
   do {
      envio_situacion = 68;
      xferbuf.hdr.signature = RWNIDENTIFICACION;
      xferbuf.hdr.command = COMANDO_TERMINAL;
      xferbuf.data[0] = 68;
      xferbuf.hdr.length = HDRSIZE + 2 + strlen(tmp);
      pdata = xferbuf.data+1;
      strcpy(pdata,tmp);
      senddatamessage(MySock.sock, &xferbuf);
      if (leecontrol_en_terminal(&r,sizeof(int),NULL) < 0)
      {
         continue;
      }     
	  moto_short(&r);
      return(r);
   } while(-1);   
}

int net_dame_compresion()
{
   return(usando_compresion);
}

int net_reasigna_compresion(int nivel)
{
   int r;
   char *pdata;
   
   envio_situacion = 69;
   xferbuf.hdr.signature = RWNIDENTIFICACION;
   xferbuf.hdr.command = COMANDO_TERMINAL;
   xferbuf.data[0] = 69;
   xferbuf.hdr.length = HDRSIZE + sizeof(int) + 1;
   pdata = xferbuf.data+1;
   usando_compresion = nivel;
   memcpy_int(pdata,&usando_compresion,sizeof(int));
   senddatamessage(MySock.sock, &xferbuf);
   flush_mysock();
   
   return(0);  
}

/* server */
#endif


static VENT *dame_vent(w_w)
DWORD w_w;
{
   int i;
   char tt[81];
   
   if (!w_w) return(stdpan);
   for (i = 0;i < nextventfree;i++)
   {
      if (w_w == remote_vent[i])
      {
         return(local_vent[i]);
      }
   }
   sprintf(tt,__Texto[396],w_w,nextventfree,xrcbbuf.data[0]);
   aviso(0,tt);
   return(stdpan);
}

static VENT *ini_vent(w,pi,pf)
DWORD w;
int pi;
int pf;
{
   int i;
   char tt[81];
   if (!w) return(stdpan);
   
   for (i = 0;i < nextventfree;i++)
   {
      if (!remote_vent[i])
      {
         break;
      }
   }
   if (i >= nextventfree) nextventfree++;
   remote_vent[i] = w;
   local_vent[i] = w_ini(pi,pf);
   return(local_vent[i]);
}

static int fin_vent(w)
DWORD w;
{
   int i;
   if (!w) return(0);
   for (i = 0;i < nextventfree;i++)
   {
      if (w == remote_vent[i])
      {
         w_fin(local_vent[i]);
         remote_vent[i] = NULL;
         local_vent[i] = NULL;
         while(nextventfree && !remote_vent[i] && (i+1) == nextventfree)
         {
            nextventfree--;
            i--;
         }
         return(0);
      }
   }
   return(-1);
}



int envia_control_a_server(pdata,len,comando)
char *pdata;
int len;
int comando;
{
   /* punto de recursion del terminal */

   xferbuf.hdr.signature = RWNIDENTIFICACION;
   xferbuf.hdr.command = CONTROL_DE_TERMINAL;  
   xferbuf.hdr.length = HDRSIZE + 1 + len;
   xferbuf.data[0] = comando;
   memcpy(xferbuf.data+1,pdata,len);
   senddatamessage(MySock.sock, &xferbuf);
   
   return(len);
}


int envia_pingping()
{
   MSGHDR hdr;

   /*ioctlsocket(MySock.sock,FIONREAD,&cuanto);*/
   
   hdr.signature = RWNIDENTIFICACION_PING;
   hdr.command = 0;  
   hdr.length = HDRSIZE;   

   rwi_senddata(MySock.sock,(char *)&hdr,hdr.length);
   
   return(0);
}


static int _w_lee_(w,tmp,wo)
VENT *w;
char *tmp;                   /* nombre del fichero de pantalla */
VENT *wo;
{
   char texto;
   char p[128];
   char nombre[128];
   int i,j,k=0;
   int f,punto;
   int alto  = (w->vfin)/100 - (w->vini)/100 + 1;
   int ancho = (w->vfin)%100 - (w->vini)%100 + 1;
   int tam = alto*ancho;
   short ppp;
   unsigned char *wvv = w->vv;
   int comprimido = 0;
   
   i = 0;
   punto = -1;
   strcpy(nombre,tmp);
   convierte_a_cache(nombre);
   tmp = nombre;
   while(*tmp)
   {
      p[i] = *tmp;
      if (p[i] == '.')
         punto = i;
      tmp++;
      if (i > 120) {aviso(0,__Texto[133]);return(-1);}
      i++;
   }
   p[i] = *tmp;
   
   if (punto == -1)
   {
      punto = i;
      strcat(p,".pan");
   }
   
   
   alto  = (wo->vfin)/100 - (wo->vini)/100 + 1;
   ancho = (wo->vfin)%100 - (wo->vini)%100 + 1;
   
   ppp = alto*100+ancho;
   
   
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
   w_cls(w);
   p[punto] = 0;		
   if (!carga_grafico(p,w)) 
   {
      p[punto] = '.';return(0);
   } 	
   p[punto] = '.';
#endif
   memcpy(wvv,wo->vv,tam*3);
   return(tam*3);
}


int mensaje_de_cabecera(pdata)
char *pdata;
{
   char *proceso;
   char *titulo;
   char *empresa;
   char *version;
   
   memcpy(&fecha_protegida,pdata,sizeof(int));
   pdata += sizeof(int);
   memcpy(&linea_25,pdata,sizeof(int));
   pdata += sizeof(int);
   memcpy(&duplicarusuario,pdata,sizeof(int));
   pdata += sizeof(int);
   
   proceso = pdata;
   pdata += (strlen(proceso)+1);
   titulo = pdata;
   pdata += (strlen(titulo)+1);
   empresa = pdata;
   pdata += (strlen(empresa)+1);
   
   
   if (*pdata)
   {
      *pdata = 1;
      pdata++;
      version = pdata;
      pdata += (strlen(version)+1);
   }
   else
   {
      pdata++;
      version = NULL;
   }
   
   if (*pdata)
   {
      *pdata = 1;
      pdata++;
      cabeza_especial=pdata;
      pdata += (strlen(cabeza_especial)+1);
   }
   else
   {
      cabeza_especial=NULL;
      pdata++;
   }
   
   if (*pdata)
   {
      *pdata = 1;
      pdata++;
      rw_extranumuser=pdata;
      pdata += sizeof(int);
   }
   else
   {
      rw_extranumuser=NULL;
      pdata++;
   }
   
   if (*pdata)
   {
      *pdata = 1;
      pdata++;
      rw_usuario=pdata;
      pdata += (strlen(rw_usuario)+1);
   }
   else
   {
      rw_usuario=NULL;
      pdata++;
   }
   cabecera_v(version,proceso,titulo,empresa);
   return(0);
}


static int menus_(pdata)
char *pdata;
{
   char *opcion[100];
   char letras[256];
   int pos;
   char texto[512];
   int defecto;
   int lineas;
   int i,n;
   char tmp[81];
   
   
   memcpy(&modo_de_validacion,pdata,sizeof(int));	
   pdata += sizeof(int);
   memcpy(&n,pdata,sizeof(int));
   pdata += sizeof(int);
   memcpy(&pos,pdata,sizeof(int));
   pdata += sizeof(int);
   memcpy(&defecto,pdata,sizeof(int));
   pdata += sizeof(int);
   memcpy(&lineas,pdata,sizeof(int));
   pdata += sizeof(int);
   memcpy(&menusg_virtual,pdata,sizeof(int));
   pdata += sizeof(int);
   strcpy(letras,pdata);
   pdata += (strlen(letras)+1);
   strcpy(texto,pdata);
   pdata += (strlen(texto)+1);
   
   for (i = 0;i < n;i++)
   {
      opcion[i] = pdata;
      pdata += (strlen(opcion[i])+1);
   }
   
   server_espera_control++;
   i = menus(opcion,letras,pos,texto,defecto,lineas);
   server_espera_control--;
   
   memcpy(tmp,&i,sizeof(int));
   memcpy(tmp+sizeof(int),&_M_Cursor,sizeof(int));
   memcpy(tmp+sizeof(int)*2,&_M_EXTRA_AVANCE,sizeof(int));
   memcpy(tmp+(sizeof(int)*3),&menusg_virtual,sizeof(int));
   envia_control_a_server(tmp,sizeof(int)*4,0);
   
   return(0);
}


char *leelinea_en_server(texto,n,file1)
char *texto;
int n;
FILE *file1;
{
   FILE *file2;
   int n2;
   if (!fichero_local_remoto) {aviso(0,__Texto[134]);texto[0] = 0;return(NULL);}
   
   do {
      leecontrol_en_server(NULL,0,0);
      if (xrcbbuf.data[0] == 99) /* anulacion */
      {
         return(NULL);
      }
/*
      else
     if (xrcbbuf.data[0] == 2)
     {
        
        pito(178);
        continue;
     }
	 */
	  /*
     if (xrcbbuf.data[0] == 1)
     {
     }
	 */
     break;
   } while(-1);
   
   memcpy(&n2,xrcbbuf.data+1,sizeof(int));
   if (!n2) return(NULL);
   if (n2 != n)
   {
      /*aviso(0,"Diferencia en lectura de fichero cliente-servidor");*/
       if (n > n2) n = n2;
   }
   if (strlen(xrcbbuf.data+1+sizeof(int)) > n)
   {
      xrcbbuf.data[n+1+sizeof(int)] = 0;
   }
   strcpy(texto,xrcbbuf.data+1+sizeof(int));
   return(texto);
}


FILE *t_rwfopen(nom,modo)
char *nom,*modo;
{
   FILE *file1,*file2;
   unsigned long tmpl;
   char *pdata;
   struct stat buf;
   char tmp[512];
   char nombre[300];
   int i;
   int flagi;
   
   if (!fichero_local_remoto)
   {
      return(fopen(nom,modo));
   }
   
   if (*modo != 'r')
   { 
      aviso(0,__Texto[135]);
      return(fopen(nom,modo));
   }
   
   if (en_conexion_terminal)
   {
      /* esperar mensaje del servidor */   
      do {
         if (leecontrol_en_server(NULL,0,1) < 0)
         {
            return(NULL);
         }
         if (xrcbbuf.data[0] == 99)
         {
            return(NULL);
         }
		 /*
         if (xrcbbuf.data[0] != 1)
         {            
            pito(179);
            continue;
         }
		 */
         pdata = xrcbbuf.data+1;
         memcpy(&file1,pdata,sizeof(FILE *));
         if (!file1)
         {
            return(NULL);
         }     
         pdata += sizeof(FILE *);
         strcpy(nombre,pdata);
         pdata += (strlen(nombre)+1);
         
         convierte_a_cache(nombre);
         /* comprobar fecha del fichero!! */

         flagi = 0;		 
         if (stat(nombre,&buf) == -1)
         {
            strcpy(tmp,nombre);
            *(quita_path(tmp)) = 0;
            crea_dir(tmp);
            /*if (stat(nombre,&buf) == -1)*/
            flagi = 1;
         }
         tmpl = buf.st_mtime;
         memcpy(&tmpl,pdata,sizeof(long));
		 /*
		 if (!strcmp(quita_path(nombre),"dsmenu01.men")) // in memoriam of expediente x de "marga"
		 {
			 aviso(0,"checkpoint 0x0111B");
			 file2 = fopen(nombre,modo);
             envia_control_a_server("",0,13); 
             return(file2);
		 }
		 */
         if (flagi || tmpl < (buf.st_mtime-1) || tmpl > (buf.st_mtime+1) || buf.st_size != *((long *)(pdata+sizeof(long))))
         {

			unlink(nombre);

            /* fopens para ficheros de texto!! */   	   
            files_in_server[nfiles_in_server] = file1;
            name_in_server[nfiles_in_server] = malloc(strlen(nombre)+1);
            strcpy(name_in_server[nfiles_in_server],nombre);
            /*strcpy(tmp,mktemp("XXXXXXXX"));*/
            strcpy(tmp,nombre);
            
            name_in_client[nfiles_in_server] = malloc(strlen(tmp)+1);
            strcpy(name_in_client[nfiles_in_server],tmp);
            time_in_server[nfiles_in_server] = tmpl;
            strcpy(tmp,modo);
            tmp[0] = 'w';    
            files_in_client[nfiles_in_server] = fopen(name_in_client[nfiles_in_server],tmp);	   
            nfiles_in_server++;	   
            envia_control_a_server("",0,12); /* traer fichero */
            return(file1);
         }
         else
         {
            file2 = fopen(nombre,modo);
            if (!file2)
            {
               files_in_server[nfiles_in_server] = file1;
               name_in_server[nfiles_in_server] = malloc(strlen(nombre)+1);
               strcpy(name_in_server[nfiles_in_server],nombre);
               /*strcpy(tmp,mktemp("XXXXXXXX"));*/
               strcpy(tmp,nombre);
               
               name_in_client[nfiles_in_server] = malloc(strlen(tmp)+1);
               strcpy(name_in_client[nfiles_in_server],tmp);
               time_in_server[nfiles_in_server] = tmpl;
               strcpy(tmp,modo);
               tmp[0] = 'w';    
               files_in_client[nfiles_in_server] = fopen(name_in_client[nfiles_in_server],tmp);	   
               nfiles_in_server++;	   
               envia_control_a_server("",0,12); /* traer fichero */
               return(file1);	     
            }
            else
            {
               envia_control_a_server("",0,13); /* fichero en cache */
               return(file2);
            }
         }
      } while(-1);   
   }
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      envio_situacion = 201;
      file1 = fopen(nom,modo);     
      pdata = tmp;     
      memcpy(pdata,&file1,sizeof(FILE *));
      pdata += sizeof(FILE *);
      strcpy(pdata,nom);
      pdata += (strlen(nom) + 1);
      if (file1)
      {
         fstat(fileno(file1),&buf); /*file1->_file*/
         tmpl = buf.st_mtime;
         memcpy_long(pdata,&tmpl,sizeof(long));
         pdata += sizeof(long);
		 tmpl = buf.st_size;
		 memcpy_long(pdata,&tmpl,sizeof(long));
		 pdata += sizeof(long);
      }   
      if (envia_control_a_terminal(tmp,(int)(pdata - tmp) + 1,1) < 0)
      {
         if (file1) fclose(file1);
         return(NULL);
      }
      if (file1)
      {
         if (leecontrol_en_terminal(NULL,0,NULL) < 0) /* anulado */
         {
            fclose(file1);
            envia_control_a_terminal("",0,99);
            return(NULL);
         }
         if (xrcbbuf.data[0] == 12)
         {
            /* el fichero se ha de enviar */
            files_in_client[nfiles_in_server] = file1;
            files_in_server[nfiles_in_server] = NULL;
            nfiles_in_server++;	   	   
         }	 
      }   
      return(file1);    
   }
#endif
   return(NULL);
}

char *t_lee_linea(texto,n,file1)
char *texto;
int n;
FILE *file1;
{
   char *p;
   int i;
   char tmp[512];
   
   
   if (!file1) return(NULL);
   
   if (!fichero_local_remoto) /* se supone que si no esta en conexion esta a cero */
   {  
      return(lee_linea(texto,n,file1));
   }
   
   if (en_conexion_terminal)
   {  
      for (i = 0;i < nfiles_in_server;i++)
      {
         if (file1 == files_in_server[i])
            break;
      }
      if (i < nfiles_in_server)
      {
         if ((p = leelinea_en_server(texto,n,file1)))
         {
            if (files_in_client[i])
               fprintf(files_in_client[i],"%s\r\n",texto);
         }
         return(p);
      }
      else
         return(lee_linea(texto,n,file1));
   }
   
#ifdef RWSERVER
   if (en_conexion_externa)
   {   
      p = lee_linea(texto,n,file1);
      for (i = 0;i < nfiles_in_server;i++)
      {
         if (file1 == files_in_client[i])
            break;
      }
      if (i < nfiles_in_server)
      {
         if (!p)
         {
            n = 0;
            memcpy_int(tmp,&n,sizeof(int));    
            envia_control_a_terminal(tmp,sizeof(int),0);
         }
         else
         {
            memcpy_int(tmp,&n,sizeof(int));
            strcpy(tmp+sizeof(int),texto);
            envia_control_a_terminal(tmp,sizeof(int)+strlen(texto)+1,0);
         }	 
         return(p);
      }
      else
      {	 
         return(p);
      } 
   }    
#endif
   return(NULL);
}

int t_rwfclose(file1) /* de momento el fclose sin mensajes ... deberia estar sincronizado por si solo */
FILE *file1;
{
   int i,j;
   struct utimbuf tb;
   
   if (!fichero_local_remoto) /* se supone que si no esta en conexion esta a cero */
			{
      return(fclose(file1));
			}
   if (en_conexion_terminal)
			{  
      for (i = 0;i < nfiles_in_server;i++)
      {
         if (file1 == files_in_server[i])
            break;
      }
      if (i < nfiles_in_server)
      {
         if (files_in_client[i])
         {
            fclose(files_in_client[i]); /* cache!! */
         }
         /* unlink(name_in_server[i]);
         rename(name_in_client[i],name_in_server[i]);*/
         tb.modtime = time_in_server[i];
         tb.actime = tb.modtime;
         utime(name_in_server[i],&tb);
         if (name_in_server[i])
            free(name_in_server[i]);
         if (name_in_client[i])
            free(name_in_client[i]);
         nfiles_in_server--;
         for (;i < nfiles_in_server;i++)
         {
            files_in_server[i] = files_in_server[i+1];
            files_in_client[i] = files_in_client[i+1];
            name_in_server[i] = name_in_server[i+1];
            name_in_client[i] = name_in_client[i+1];
            time_in_server[i] = time_in_server[i+1];
         }
         return(0);
      }
      else
         return(fclose(file1));
			}
#ifdef RWSERVER
   if (en_conexion_externa)
			{
      for (i = 0;i < nfiles_in_server;i++)
      {
         if (file1 == files_in_client[i])
            break;
      }
      if (i < nfiles_in_server)
      {
         nfiles_in_server--;
         for (;i < nfiles_in_server;i++)
         {
            files_in_server[i] = files_in_server[i+1];
            files_in_client[i] = files_in_client[i+1];
         }
      }
      return(fclose(file1));
			}
#endif
   return(0);
}

int leecontrol_fistro(fistro)
char *fistro;
{
			int rkk;
         int se = server_espera_control;
         
         if (fistro[sizeof(int)*5] == 1)
         {
            rkk = 2 + sizeof(int) + strlen(dato_entra);
         }
         else
            rkk = 1;
         if (se)
            server_espera_control--;
         envia_control_a_server(fistro,sizeof(int)*5+rkk,10); /* leecar en servicio */
         do {
            leecontrol_en_server(NULL,0,2);
			/*
            if (xrcbbuf.data[0] != 2)
            {               
               pito(180);
               continue; 
            }
			*/
            memcpy(fistro,xrcbbuf.data+1,300);
            if (se)
               server_espera_control++;
            return(0);
         } while(-1);
         if (se)
            server_espera_control++;
}


int conecta_a_otro_server(pdata)
char *pdata;
{
   int n;
   char c;
   
   memcpy_int(&server_id,pdata,sizeof(int)); /* en teoria deberia ya ser el mismo id */
   n = 0;
   envia_control_a_server(&n,sizeof(int),14);
   finalizando_conexion = 1;
   leecontrol_en_server("",0,3); /* ojo si hay problemas poner una pausa */
   finalizando_conexion = 0;
   establece_desconexion(0);
   server_request[2] = '/';
   ejecutae(":/bin/agisys");
   server_request[2] = '/';
   return(establece_conexion(0));
}


int _leecontrol_en_server(pdata,len,waitfor)
char *pdata;
int len;
int *waitfor;
{
   unsigned long cuanto;
   int err;
   int prio;

   if (!waitfor)
   {
	   aviso(0,"Control interno: depuracion cola de mensajes (1)");	   
   }


   if ((err = busca_en_cola_server(pdata,len,*waitfor)) >= 0)
	   return err;
   
   while(-1)
   {
						
						
						flush_mysock();
                  
                  if (!rwp_rcv_buffer)
                  {
                     FD_ZERO(&setread);
                     FD_SET(MySock.sock,&setread);
                     FD_ZERO(&seterror);
                     FD_SET(MySock.sock,&seterror);
#ifndef WIN32
			   errno = 0;
#endif
                     if ((err = select(MySock.sock+1,&setread,NULL,&seterror,NULL)) == SOCKET_ERROR || FD_ISSET(MySock.sock,&seterror))
                     {
#ifndef WIN32
						 if (err == SOCKET_ERROR && errno == EINTR)
						 {
							 continue;
						 }
#endif

                        if (finalizando_conexion) return(-1);
                        establece_desconexion(0);
#ifdef FGVDOS
#ifndef FGVWIN
#ifndef WATCOMNLM
                        if (myconsola == INVALID_HANDLE_VALUE)
                        {
                           AllocConsole();
                           ini_my_consola();
                           cursor(Pcursor/80*100+Pcursor%80+101);
                        }
#endif
#endif
#endif
                        v_refresca();
                        aviso(0,__Texto[136]);
                        return(-1);
                     }
                     
                     cuanto = 0L;
                     ioctlsocket(MySock.sock,FIONREAD,&cuanto);
                     if (!cuanto)
                     {
                        /* desconexion !!! */
                        recv(MySock.sock,&cuanto,cuanto,0);
                        if (finalizando_conexion) return(-1);
                        establece_desconexion(0);
#ifdef FGVDOS
#ifndef FGVWIN
#ifndef WATCOMNLM
                        if (myconsola == INVALID_HANDLE_VALUE)
                        {
                           AllocConsole();
                           ini_my_consola();
                           cursor(Pcursor/80*100+Pcursor%80+101);
                        }
#endif
#endif
#endif
                        v_refresca();
                        aviso(0,__Texto[137]);
                        return(-1);
                     }
                  }
                  if (recvdatamessage(MySock.sock,&xrcbbuf) == FALSE)
                  {
                     if (finalizando_conexion) return(-1); 
                     aviso(0,__Texto[138]);
                     return(-1);
                  }
                  if (xrcbbuf.hdr.command != CONTROL_DE_SERVIDOR)
                  {
					 /********/
                     if (xrcbbuf.hdr.command == FILE_TRANSFER_CLOSE)
                        continue;
                     
                     if (xrcbbuf.hdr.command == FILE_TRANSFER_ACK)
                        continue;

                     if (xrcbbuf.hdr.command == FILE_TRANSFER_NACK)
                        continue;

					 if (xrcbbuf.hdr.command == FILE_TRANSFER_DATA)
						continue;

                     if (xrcbbuf.hdr.command == COMANDO_TERMINAL)
                     {
                        procesa_terminal(&xrcbbuf); 
                        continue;
                     }
                     if (finalizando_conexion) return(-1);
                     aviso(0,__Texto[139]);
                     return(-1);
                  }
                  if (waitfor)
                  {
                     if (*waitfor != xrcbbuf.data[0])
                     {
						if (*waitfor == 0 || *waitfor == 1)
						{
							if (xrcbbuf.data[0] == 99)
								break;
						}
						 /* guardarlo */
						pon_en_cola_server();
                        continue; 
                     }
                  }
                  
                  break;
   }
   
   
   if (pdata)
   {
						memcpy(pdata,xrcbbuf.data+1,len);
   }
   return(xrcbbuf.hdr.length-HDRSIZE-1);
}

int leecontrol_en_server(pdata,len,queespero)
char *pdata;
int len;
int queespero;
{
   int waitfor = queespero;
   return(_leecontrol_en_server(pdata,len,&waitfor));
}


void en_server_rentra_pos(qp,contai,conta,insert)
int qp,contai,conta,insert;
{
   return;
}

static int llama_entra(pdata)
unsigned char *pdata;
{
   unsigned char tmp[1024];
   int qp,maxqc,qh,err;
   char qi[51],qs[51];
   char qd[300];
   char qo[300];
   char *qod;
   int _rentra_autosal = rentra_autosal;
   int _rentra_edicion = rentra_edicion; 
   int _rentra_atrib = rentra_atrib;
   int _rentra_sal_atrib = rentra_sal_atrib;
   int _rentra_cc = rentra_cc;
   int _rentra_cf = rentra_cf;
   int _rentra_sal_cc = rentra_sal_cc;
   int _rentra_sal_cf = rentra_sal_cf;
   int _rentra_offqp = rentra_offqp; 
   int _rentra_offini = rentra_offini;
   int _rentra_insert = rentra_insert;
   void (*_rentra_pos)() = rentra_pos;
   
   
   memcpy(&modo_de_validacion,pdata,sizeof(int));
   pdata += sizeof(int);
   memcpy(clave_help,pdata,32);
   pdata += 32;
   /*memcpy(&rentra_autosal,pdata,sizeof(int));
   pdata += sizeof(int);*/
   memcpy(&rentra_edicion,pdata,sizeof(int));
   pdata += sizeof(int);
   memcpy(&rentra_sal_atrib,pdata,sizeof(int));
   pdata += sizeof(int);
   /*memcpy(&rentra_autosal,pdata,sizeof(int));
   pdata += sizeof(int);*/
   memcpy(&rentra_cc,pdata,sizeof(int));
   pdata += sizeof(int);
   memcpy(&rentra_cf,pdata,sizeof(int));
   pdata += sizeof(int);
   memcpy(&rentra_sal_cc,pdata,sizeof(int));
   pdata += sizeof(int);
   memcpy(&rentra_sal_cf,pdata,sizeof(int));
   pdata += sizeof(int);
   memcpy(&rentra_offqp,pdata,sizeof(int));
   pdata += sizeof(int);
   memcpy(&rentra_offini,pdata,sizeof(int));
   pdata += sizeof(int);
   memcpy(&rentra_insert,pdata,sizeof(int));
   pdata += sizeof(int);
   memcpy(&redita_col,pdata,sizeof(int));
   pdata += sizeof(int);
   memcpy(&qpi_rt,pdata,sizeof(int));
   pdata += sizeof(int);
   memcpy(&qp,pdata,sizeof(int));
   pdata += sizeof(int);
   memcpy(&maxqc,pdata,sizeof(int));
   pdata += sizeof(int);
   memcpy(&qh,pdata,sizeof(int));
   pdata += sizeof(int);  
   memcpy(&data_fecha_conmutar,pdata,sizeof(int));
   pdata += sizeof(int);  
   memcpy(&data_hiden_indicator,pdata,sizeof(int));
   pdata += sizeof(int);  
   caracter_de_fecha = *pdata;
   pdata++;
   if (*pdata)
   {
      if (*pdata == 1)
         rentra_pos = pinta_col;
      else
         rentra_pos = en_server_rentra_pos;
   }
   else
      rentra_pos = NULL;   
   pdata++;
   strcpy(qi,pdata);
   pdata += strlen(qi)+1;
   strcpy(qs,pdata);
   pdata += strlen(qs)+1;
   strcpy(qd,pdata);
   pdata += strlen(qd)+1;
   if (rentra_edicion)
   {
      qod = qo;
      strcpy(qo,pdata);
      pdata += strlen(qo)+1;	  
   }
   else
   {
      if (*pdata)
      {
         qod = qd;
      }
      else
      {
         qod = qo;
         qo[0] = 0;
      }
      pdata++;
   }
   
   /*
   memcpy(qo,pdata,maxqc+1);
   pdata += (maxqc+1);
   */
   
   
   server_espera_control++;
   if (abandonarelbarco)
	   err = 7;
   else
	   err = entra(qp,maxqc,qi,qs,qd,qh,qod);
   server_espera_control--;
   
   
   pdata = tmp;
   
   memcpy(pdata,&err,sizeof(int));
   pdata += sizeof(int);
   memcpy(pdata,&_M_Cursor,sizeof(int));
   pdata += sizeof(int);
   memcpy(pdata,&_ID_Cursor,sizeof(int));
   pdata += sizeof(int);
   memcpy(pdata,&_M_EXTRA_AVANCE,sizeof(int));
   pdata += sizeof(int);
   
   memcpy(pdata,clave_help,32);
   pdata += 32;
   /*memcpy(pdata,&rentra_autosal,sizeof(int));
   pdata += sizeof(int);*/
   memcpy(pdata,&rentra_edicion,sizeof(int));
   pdata += sizeof(int);
   memcpy(pdata,&rentra_sal_atrib,sizeof(int));
   pdata += sizeof(int);
   /*memcpy(pdata,&rentra_autosal,sizeof(int));
   pdata += sizeof(int);*/
   memcpy(pdata,&rentra_cc,sizeof(int));
   pdata += sizeof(int);
   memcpy(pdata,&rentra_cf,sizeof(int));
   pdata += sizeof(int);
   memcpy(pdata,&rentra_sal_cc,sizeof(int));
   pdata += sizeof(int);
   memcpy(pdata,&rentra_sal_cf,sizeof(int));
   pdata += sizeof(int);
   memcpy(pdata,&rentra_offqp,sizeof(int));
   pdata += sizeof(int);
   memcpy(pdata,&rentra_offini,sizeof(int));
   pdata += sizeof(int);
   memcpy(pdata,&rentra_insert,sizeof(int));
   pdata += sizeof(int);
   memcpy(pdata,&redita_col,sizeof(int));
   pdata += sizeof(int);
   memcpy(pdata,&eqj,sizeof(int));
   pdata += sizeof(int);
   memcpy(pdata,&data_fecha_conmutar,sizeof(int));
   pdata += sizeof(int);  
   memcpy(pdata,&data_hiden_indicator,sizeof(int));
   pdata += sizeof(int);  
   *pdata = caracter_de_fecha;
   pdata++;
   
   
   strcpy(pdata,qod);  
   pdata += (strlen(qod)+1);
   
   
   envia_control_a_server(tmp,(int)(pdata-tmp)+5,11);
   
   
   rentra_autosal = _rentra_autosal;
   rentra_edicion = _rentra_edicion; 
   rentra_atrib = _rentra_atrib;
   rentra_sal_atrib = _rentra_sal_atrib;
   rentra_cc = _rentra_cc;
   rentra_cf = _rentra_cf;
   rentra_sal_cc = _rentra_sal_cc;
   rentra_sal_cf = _rentra_sal_cf;
   rentra_offqp = _rentra_offqp; 
   rentra_offini = _rentra_offini;
   rentra_insert = _rentra_insert;
   rentra_pos = _rentra_pos;
   return(0);
}


char *salta_un_path(char *destino)
{
	while (*destino && *destino != '/' && *destino != '\\')
	{
		destino++;
	}
	if (*destino == '/' || *destino == '\\')
		destino++;
	return destino;
}

int rw_envia_fichero(destino,temp)
char *destino;
int temp;
{
   int file;
   int r,err,l,i;
   int max;
   unsigned long tam;
   int borra = 0;
   int nodir = 0;
   char nom[512];
   char *p;
   char tmpmd5[34];
   int conmd5 = 0;
   char *pp;
   long cuanto;
   int veces;
   int pos,nack;
   long tiempo;
   int antiguo = 0;
   long conta1,conta2,conta3;
   FILE *ff = NULL;

   /*
   char _destino[1024];
   strcpy(_destino,destino);
   destino = _destino;
   */
   
   if (temp == 777)
   {
      temp = 0;
	  if (*destino == 3) /* bueno para el origen si queremos el dirbass */
	  {
		  destino++;
	  }
	  else
	  {
		  nodir = 1;   
		  if (other3_server != 56443696 && getenv("DS_TELEBLOQUEO"))
		  {	   
			 return(-1);
		  }
	  }
   }
   
   if (temp == 666)
   {
      temp = 4;
      borra = 1;
   }
   if (nodir)
   {   
      strcpy(nom,destino);
      for (i = 0;nom[i] && nom[i] != 1;i++);
      if (nom[i])
      {
         nom[i] = 0;
         destino += i;
      }
   }
   else
   {
	  if (temp == 5)
	  {
		  strcpy(nom,rw_directorio(salta_un_path(destino)));
	  }
	  else
	  {
		  strcpy(nom,rw_directorio(destino));
	  }
   }
   flush_mysock();

   if (temp != 4 && calcula_md5(nom,tmpmd5) == 0)
	   conmd5 = 1;

   file = rw_base_open_(nom,O_RDONLY | O_BINARY);
   if (file < 0)
      return(-1);
   
   tam = rw_base_lseek_(file,0L,SEEK_END);
   rw_base_lseek_(file,0L,SEEK_SET);
   /*max = MAXRDATA-1024;   */
   max = 2048;
   l = strlen(destino);
   if (temp == 4)
   { 
      strcpy(nom,destino+l-temp);
      strcpy(destino+l-temp,".tmp");
   }
   xferbuf.hdr.signature = RWNIDENTIFICACION;
   xferbuf.hdr.command   = FILE_TRANSFER_OPEN;
   r = l+1+sizeof(long);   
   memcpy(xferbuf.data,&tam,sizeof(long));

   pp = xferbuf.data+sizeof(long);
   if (conmd5)
   {
	   *pp = 2;
	   pp++;
	   memcpy(pp,tmpmd5,32);
	   pp += 32;
	   r += 33;
   }
   strcpy(pp,destino);
   xferbuf.hdr.length = r+HDRSIZE;
   moto_short(&xferbuf.hdr.length);
   if (rwi_senddata(MySock.sock,(char *)&xferbuf,r+HDRSIZE) == FALSE)
   { 
	  moto_short(&xferbuf.hdr.length);
      rw_base_close_(file);
      return(-1);
   }
   moto_short(&xferbuf.hdr.length);
   
   if (en_conexion_terminal)
   {
      if (en_conexion_terminal && DespachadorExterno)
      { 
         (*DespachadorExterno)((int)DESPACHO_PROGRESO,0,(void *)&tam);
      }
   }
   
   FD_ZERO(&setread);
   FD_SET(MySock.sock,&setread);
   FD_ZERO(&seterror);
   FD_SET(MySock.sock,&seterror);
   do
   {
#ifndef WIN32
			   errno = 0;
#endif
   if ((err = select(MySock.sock+1,&setread,NULL,&seterror,NULL)) == SOCKET_ERROR || FD_ISSET(MySock.sock,&seterror))
   {
#ifndef WIN32
			 if (err == SOCKET_ERROR && errno == EINTR)
			 {
				 continue;
			 }
#endif

      rw_base_close_(file);
      if (en_conexion_terminal && DespachadorExterno)
      {
         (*DespachadorExterno)((int)DESPACHO_PROGRESO,2,(void *)NULL);
      }
      return(-1);
   }
   break;
   } while(-1);
   if (recvdatamessage(MySock.sock,&xrcbbuf) == FALSE)
   {
      rw_base_close_(file);
      if (en_conexion_terminal && DespachadorExterno)
      {
         (*DespachadorExterno)((int)DESPACHO_PROGRESO,2,(void *)NULL);
      }
      return(-1);
   }
   if (xrcbbuf.hdr.command == FILE_TRANSFER_END)
   {
      rw_base_close_(file);
      if (en_conexion_terminal && DespachadorExterno)
      {
         (*DespachadorExterno)((int)DESPACHO_PROGRESO,2,(void *)NULL);
      }
	   if (borra)
	   {
		  char tmpx[256];
		  strcpy(tmpx,rw_directorio(destino));
		  rw_base_unlink(tmpx);
	   }
	   return 0;
   }

   if (xrcbbuf.hdr.command != FILE_TRANSFER_OK)
   {
      rw_base_close_(file);
      if (en_conexion_terminal && DespachadorExterno)
      {
         (*DespachadorExterno)((int)DESPACHO_PROGRESO,2,(void *)NULL);
      }
      return(-1);   
   }

   
   
   do {
	   cuanto = 0L;
	   ioctlsocket(MySock.sock,FIONREAD,&cuanto);
	   if (cuanto >= HDRSIZE) {
		   if (rwi_recvdata(MySock.sock,(char *)&xrcbbuf2,HDRSIZE) == FALSE) {
			  rw_base_close_(file);
			  if (en_conexion_terminal && DespachadorExterno)
			  {
				 (*DespachadorExterno)((int)DESPACHO_PROGRESO,2,(void *)NULL);
			  }
			  return -1;
		   }
		   if (xrcbbuf2.hdr.length > HDRSIZE) {
			   if (rwi_recvdata(MySock.sock,((char *)&xrcbbuf2)+HDRSIZE,xrcbbuf2.hdr.length-HDRSIZE) == FALSE) {
				  rw_base_close_(file);
				  if (en_conexion_terminal && DespachadorExterno)
				  {
					 (*DespachadorExterno)((int)DESPACHO_PROGRESO,2,(void *)NULL);
				  }
				  return -1;
			   }
		   }
		   pon_en_cola_server2();
	   }
	   else
		   break;

   } while(-1);
   
   

#ifdef RWSERVER
   if (en_conexion_externa && terminal_internal_build < 158) 
	   antiguo = 1;
#else
   if (en_conexion_terminal && server_internal_build < 158)
	   antiguo = 1;
#endif

   if (antiguo) {
	   err = 0;
	   xferbuf.hdr.signature = RWNIDENTIFICACION;
	   xferbuf.hdr.command   = FILE_TRANSFER_DATA;
	   while((r = rw_base_read_(file,xferbuf.data,max)) > 0)
	   {
		  xferbuf.hdr.length = r+HDRSIZE;
		  moto_short(&xferbuf.hdr.length);
		  if (rwi_senddata(MySock.sock,(char *)&xferbuf,r+HDRSIZE) == FALSE)
		  {
			 moto_short(&xferbuf.hdr.length);
			 err = -1;
			 break;
		  }
		  moto_short(&xferbuf.hdr.length);
		  if (antiguo) {
#ifdef FGVDOS
			  Sleep(50);
#else
			  usleep(50000);
#endif
		  }
	   }
   }
   else
   {
	   veces = 0;
	   conta1 = 0;
	   conta2 = 0;
	   err = 0;
	   while(-1)
	   {
		  nack = 0;
		  tiempo = time(NULL);
		  pos = rw_base_lseek_(file,0L,SEEK_CUR);
		  r = rw_base_read_(file,xferbuf.data+(sizeof(int)*2),max);
			   

	      if (ff)
			   fprintf(ff,"leidos:%d\r\n",r);fflush(ff);

		  if (r > 0) {
			  xferbuf.hdr.signature = RWNIDENTIFICACION;
			  xferbuf.hdr.command   = FILE_TRANSFER_DATA;
			  memcpy(xferbuf.data,&pos,sizeof(int));
			  memcpy(xferbuf.data+sizeof(int),&r,sizeof(int));
			  xferbuf.hdr.length = r+HDRSIZE+sizeof(int)*2;
			  moto_short(&xferbuf.hdr.length);
			  if (rwi_senddata(MySock.sock,(char *)&xferbuf,xferbuf.hdr.length) == FALSE)
			  {
				 moto_short(&xferbuf.hdr.length);
				 err = -1;
				 break;
			  }
			  conta1++;
			  moto_short(&xferbuf.hdr.length);

			  if (ff)
				   fprintf(ff,"enviados:%d pos:%ld conta1:%d\r\n",r,pos,conta1);fflush(ff);

			  if (en_conexion_terminal && DespachadorExterno)
			  { 
				  long ccc;
				  ccc = pos + r;
				 (*DespachadorExterno)((int)DESPACHO_PROGRESO,1,(void *)&ccc);
			  }
		  }
		  else {
			  /*if (conta2 >= conta1)*/
			    if (ff)
				   fprintf(ff,"salgo envio\r\n");fflush(ff);
			     break;
		  }

		  conta3 = 0;
		  do {
			  if ((time(NULL)-tiempo) > (60+(nack*20)) ) {
				  if (r > 0) {
					  if (rwi_senddata(MySock.sock,(char *)&xferbuf,xferbuf.hdr.length) == FALSE)
					  {
						 moto_short(&xferbuf.hdr.length);
						 err = -1;
						 break;
					  }
					  conta1++;
					  if (ff)
						   fprintf(ff,"reenviados:%d pos:%ld conta1:%d\r\n",r,pos,conta1);fflush(ff);
				  }
				  nack++;
				  if (ff)
				     fprintf(ff,"nack:%d\r\n",nack);fflush(ff);
			  }
			  if ((time(NULL)-tiempo) > 600) {
			    if (ff)
				     fprintf(ff,"timeout!\r\n");fflush(ff);
				 err = -1;
				 break;
			  }
			  cuanto = 0L;
			  if (ioctlsocket(MySock.sock,FIONREAD,&cuanto) == SOCKET_ERROR) {
				  //rw_base_close_(file);
				  
				  if (en_conexion_terminal && DespachadorExterno)
				  {
					 (*DespachadorExterno)((int)DESPACHO_PROGRESO,2,(void *)NULL);
				  }				  
				  err = -1;
			      if (ff)
				     fprintf(ff,"error ioctlsocket\r\n");fflush(ff);
				  break;
			  }
		      if (ff)
			     fprintf(ff,"cuanto:%ld\r\n",cuanto);fflush(ff);

			  if (cuanto >= HDRSIZE) {
				  if (rwi_recvdata(MySock.sock,(char *)&xrcbbuf2,HDRSIZE) == FALSE) {
					  //rw_base_close_(file);
					  
					  if (en_conexion_terminal && DespachadorExterno)
					  {
						 (*DespachadorExterno)((int)DESPACHO_PROGRESO,2,(void *)NULL);
					  }
				      err = -1;
				      break;
				  }
				  if (xrcbbuf2.hdr.length > HDRSIZE) {
					   if (rwi_recvdata(MySock.sock,((char *)&xrcbbuf2)+HDRSIZE,xrcbbuf2.hdr.length-HDRSIZE) == FALSE) {
						  rw_base_close_(file);
						  
						  if (en_conexion_terminal && DespachadorExterno)
						  {
							 (*DespachadorExterno)((int)DESPACHO_PROGRESO,2,(void *)NULL);
						  }
						  
						  if (ff)
							 fprintf(ff,"ERROR recepcion\r\n");fflush(ff);
						  return -1;
					   }
				  }
				  if (xrcbbuf2.hdr.command == FILE_TRANSFER_CLOSE) {
					  r = 0;
					  if (ff)
						 fprintf(ff,"CLOSE\r\n");fflush(ff);
					  break;					  
				  }
				  else if (xrcbbuf2.hdr.command == FILE_TRANSFER_NACK) {
					  memcpy(&pos,xrcbbuf2.data,sizeof(int));
					  rw_base_lseek_(file,pos,SEEK_SET);
					  veces = 0;
					  conta2++;					  
					  if (ff)
						 fprintf(ff,"NACK conta2:%ld\r\n",conta2);fflush(ff);
					  break;
				  }	
				  else if (xrcbbuf2.hdr.command == FILE_TRANSFER_ACK) {
					  veces = 0;
					  conta2++;
					  if (ff)
						 fprintf(ff,"ACK conta2:%ld\r\n",conta2);fflush(ff);
					  /*if (r > 0 || conta2 >= conta1)*/
						 break;
				  }
				  else {
				   if (ff)
					   fprintf(ff,"paquete (3) out of sync:%d\r\n",xrcbbuf2.hdr.command);fflush(ff);
					   /*if (en_conexion_terminal)*/
							/*pump_mensaje(&xrcbbuf2);*/
				  }				  
			  }
			  else {
				  if (veces > 3 || r < max) {
					  conta3++;
	#ifdef FGVDOS
					  Sleep(10);
	#else
					  usleep(10000);
	#endif
				      if (ff)
					      fprintf(ff,"bucle veces:%d r:%d CONTA3:%ld\r\n",veces,r,conta3);fflush(ff);

					  continue;
				  }
			  /*if (r > 0 || conta2 >= conta1)*/
				 break;
			  }
		  } while(-1);	  

		  if (r <= 0 || err == -1)
			  break;
		  veces++;
	      if (ff)
		      fprintf(ff,"veces:%d\r\n",veces);fflush(ff);

	   }
   }
   rw_base_close_(file);
   if (borra)
   {
      char tmpx[256];
      strcpy(tmpx,rw_directorio(destino));
      rw_base_unlink(tmpx);
   }
   
   xferbuf.hdr.command  = FILE_TRANSFER_CLOSE;
   xferbuf.hdr.length = HDRSIZE;
   if (temp == 4)
   {
      strcpy(destino+l-temp,nom);
      xferbuf.hdr.length += (l+1);
      strcpy(xferbuf.data,destino);
   }
   {
	   int xxl = xferbuf.hdr.length;
       moto_short(&xferbuf.hdr.length);
       rwi_senddata(MySock.sock,(char *)&xferbuf,xxl);
       moto_short(&xferbuf.hdr.length);
   }
   
   if (en_conexion_terminal && DespachadorExterno)
   {
      (*DespachadorExterno)((int)DESPACHO_PROGRESO,2,(void *)NULL);
   }
   
   return(err);
}



int procesa_terminal(xfer)
DATAMSG *xfer;
{
   int i,j,k,t,i2,i3;
   long ll;
   int flag;
   short ppp;
   char c;
   DWORD ww;
   VENT *wo;
   VENT wl;
   FILE *ff;
   char *pdata,*pdata2,*pdata3;
   int datalen;
   char tmp[18000];
   char tmp2[512];
#ifdef FGVUNIX
   int pcurso;
#endif
#ifdef FGVDOS
   HWND tmpw;
   static HWND dlg = NULL;
#endif
   
   flag = 0;
   pdata = xfer->data+1;
   datalen = xfer->hdr.length - HDRSIZE - 1;

   switch(xfer->data[0])
   {
   case 0:
      Patrib &= ~128;
      Patrib |= (xfer->data[1]&128);
#ifdef FGVUNIX
      memcpy(&pcurso,xfer->data+2,sizeof(int));
      if (pcurso != Pcursor)
      {
         cursor(pcurso/80+pcurso%80+101);
      }
      
#else
      memcpy(&Pcursor,xfer->data+2,sizeof(int));
#endif
      /*memcpy(&PCcolor,xfer->data+2+sizeof(int),sizeof(int));*/
      /*memcpy(&PFcolor,xfer->data+2+(sizeof(int)*2),sizeof(int));*/
      v_print(xfer->data+2+(sizeof(int)*3));
      break;
   case 1:
      Patrib &= ~128;
      Patrib |= (xfer->data[2+sizeof(int)]&128);                  
#ifdef FGVUNIX
      memcpy(&pcurso,xfer->data+1,sizeof(int));
      if (pcurso != Pcursor)
      {
         cursor(pcurso/80+pcurso%80+101);
      }
      
#else
      memcpy(&Pcursor,xfer->data+1,sizeof(int));
#endif
      
      Conversion(xfer->data[1+sizeof(int)]);
      v_putc(xfer->data[1+sizeof(int)]);
      break;
   case 2:
      Patrib &= ~128;
      memcpy(&i,xfer->data+1,sizeof(int));
      cursor(i);
      break;
   case 3:
      Patrib &= ~128;
      Patrib |= (xfer->data[1]&128);
      Pgrafi = xfer->data[2];
      /*memcpy(&PCcolor,xfer->data+3,sizeof(int));*/
      /*memcpy(&PFcolor,xfer->data+3+sizeof(int),sizeof(int));*/
      memcpy(&win_fondo_neutro,xfer->data+3+(sizeof(int)*2),sizeof(int));
      cls();
      break;
   case 4:
      Patrib &= ~128;
      Patrib |= (xfer->data[2+sizeof(int)]&128);                  
#ifdef FGVUNIX
      memcpy(&pcurso,xfer->data+1,sizeof(int));
      if (pcurso != Pcursor)
      {
         cursor(pcurso/80+pcurso%80+101);
      }
      
#else
      memcpy(&Pcursor,xfer->data+1,sizeof(int));
#endif
      
      Graf(xfer->data[1+sizeof(int)]);
      break;
   case 5:
      /* solicitud de lectura de teclado */
      memcpy(&modo_de_validacion,pdata,sizeof(int));
      leecar_terminal_puro = 1;
      server_espera_control++;
      k = leecar();
      server_espera_control--;
      leecar_terminal_puro = 0;
      pdata = tmp;
      memcpy(pdata,&k,sizeof(int));
      pdata += sizeof(int);
      memcpy(pdata,&_M_Cursor,sizeof(int));
      pdata += sizeof(int);
      memcpy(pdata,&_ID_Cursor,sizeof(int));
      pdata += sizeof(int);
      memcpy(pdata,&_M_EXTRA_AVANCE,sizeof(int));
      envia_control_a_server(tmp,sizeof(int)*4,1);
      break;
   case 6:
      Patrib &= ~128;
      memcpy(&i,xfer->data+1,sizeof(int));
      memcpy(&Pcursor,xfer->data+1+sizeof(int),sizeof(int));
      curon(i);
#if defined(FGVWIN) || defined(FGVUNIX)
      cursor(Pcursor/80*100+Pcursor%80+101);
#endif
      break;
   case 7:                  
#ifdef FGVUNIX
      memcpy(&pcurso,xfer->data+1,sizeof(int));
      if (pcurso != Pcursor)
      {
         cursor(pcurso/80+pcurso%80+101);
      }
      
#else
      memcpy(&Pcursor,xfer->data+1,sizeof(int));
#endif
      
      pdata = xfer->data+1+sizeof(int);
      memcpy(&ww,pdata,sizeof(ww));
      pdata += sizeof(int);
      memcpy(&win_fondo_neutro,pdata,sizeof(int));
      pdata += sizeof(int);
      Patrib &= ~128;
      Patrib |= (*pdata & 128);
      w_refresca(dame_vent(ww));
      break;
   case 8:
      /* atributo */
      k = xfer->data[1];
      Pgrafi = xfer->data[2];
      Patrib &= ~128;
      if (k == 'Z') break;
      memcpy(&_PCcolor,xfer->data+3,sizeof(int));
      memcpy(&_PFcolor,xfer->data+3+sizeof(int),sizeof(int));
      atributo(k);
      break;
   case 9:
   case 209:
      /* w_ini */
      Patrib &= ~128;
      memcpy(&ww,pdata,sizeof(ww));
      pdata += sizeof(ww);
      memcpy(&k,pdata,sizeof(int));
      pdata += sizeof(int);
      memcpy(&j,pdata,sizeof(int));
      pdata += sizeof(int);
      ini_vent(ww,k,j);
#ifdef FGVWIN
#ifdef DSPROFUIS9
	  if (xfer->data[0] == 209)
		  w_sumafondo();
#endif
#endif
      break;
   case 10:
   case 210:
#ifdef FGVWIN
#ifdef DSPROFUIS9
	  w_restafondo();
#endif
#endif

      /* w_fin */
      Patrib &= ~128;
      memcpy(&ww,pdata,sizeof(ww));
      fin_vent(ww);
      break;
   case 11:
      /* w_putc */
      Patrib &= ~128;
      c = *pdata;
      pdata++;
      memcpy(&ww,pdata,sizeof(ww));
      pdata += sizeof(ww);                  
#ifdef FGVUNIX
      memcpy(&pcurso,pdata,sizeof(int));
      if (pcurso != Pcursor)
      {
         cursor(pcurso/80+pcurso%80+101);
      }
      
#else
      memcpy(&Pcursor,pdata,sizeof(int));
#endif
      
      w_putc(dame_vent(ww),c);
      break;
   case 12:
      /* w_cambia */
      Patrib &= ~128;
      memcpy(&ww,pdata,sizeof(ww));
      pdata += sizeof(ww);
      memcpy(&k,pdata,sizeof(int));
      pdata += sizeof(int);
      memcpy(&j,pdata,sizeof(int));
      pdata += sizeof(int);
      w_cambia(dame_vent(ww),k,j);
      break;
   case 13:
      /* w_marco */
      Patrib &= ~128;
      memcpy(&ww,pdata,sizeof(ww));
      pdata += sizeof(ww);
      memcpy(&k,pdata,sizeof(int));
      pdata += sizeof(int);
      memcpy(&j,pdata,sizeof(int));
      pdata += sizeof(int);
      memcpy(&t,pdata,sizeof(int));
      pdata += sizeof(int);
      w_marco_(dame_vent(ww),k,j,t);
      break;
   case 14:
      /* w_saca */
      Patrib &= ~128;
      memcpy(&ww,pdata,sizeof(ww));
      pdata += sizeof(ww);
      memcpy(&wo,pdata,sizeof(wo));
      pdata += sizeof(wo);
      w_saca(dame_vent(ww),dame_vent((DWORD)wo));
      break;
   case 15:
      /* _w_mete */
      Patrib &= ~128;
      memcpy(&ww,pdata,sizeof(ww));
      pdata += sizeof(ww);
      memcpy(&wo,pdata,sizeof(wo));
      pdata += sizeof(wo);
      memcpy(&k,pdata,sizeof(int));
      pdata += sizeof(int);
      _w_mete(dame_vent(ww),dame_vent((DWORD)wo),k);
      break;
   case 58: /* recibe stdpan */                  /* w_lee */
      flag = 1;
   case 16:                
      Patrib &= ~128;
      memcpy(&ww,pdata,sizeof(ww));
      pdata += sizeof(ww);
      memset(&wl,0,sizeof(VENT));
      memcpy(&(wl.vini),pdata,sizeof(int));
      pdata += sizeof(int);
      memcpy(&(wl.vfin),pdata,sizeof(int));
      pdata += sizeof(int);
      memcpy(&(wl.size),pdata,sizeof(int));
      pdata += sizeof(int);
      k = wl.size*3;
      if (!*pdata)
      {
         pdata++;
         wl.vv = pdata;
         pdata += k;
      }
      else
      {
         pdata++;
         wl.vv = tmp;
         pdata2 = tmp;
         j = 0;
         while (j < k)
         {
            memcpy(&ppp,pdata,sizeof(short));               
            pdata += sizeof(short);
            if (ppp < 0)
            {
               *pdata2 = *pdata;
               pdata2++;					 
               ppp = (-ppp);
               for (i = 0;i < ppp;i++)
               {
                  *pdata2 = *pdata;
                  pdata2++;
               }
               pdata++;					 
               j++;
            }				   
            else
            {
               memcpy(pdata2,pdata,ppp);
               pdata += ppp;
               pdata2 += ppp;
            }
            j += ppp;		  
         }                  
      } /* j puede se 1 mas */
      if (flag)
      {					
         ww = (DWORD)w_ini(wl.vini,wl.vfin);
         if (((VENT *)ww)->size < wl.size)
            i = ((VENT *)ww)->size;
         else
            i = wl.size;
         memcpy(((VENT *)ww)->vv,wl.vv,i*3);
         w_pinta((VENT *)ww);
         w_fin((VENT *)ww);
      }
      else
         _w_lee_(dame_vent(ww),pdata,&wl);
      break;
   case 17:
      /* w_cls */
      Patrib &= ~128;
      memcpy(&ww,pdata,sizeof(ww));
      w_cls(dame_vent(ww));
      break;
   case 18:
      /* w_wll */
      Patrib &= ~128;
      memcpy(&ww,pdata,sizeof(ww));
      pdata += sizeof(ww);
      memcpy(&k,pdata,sizeof(int));
      pdata += sizeof(int);
      w_wll(dame_vent(ww),k);
      break;
   case 19:
      /* Bl() */
      Patrib &= ~128;
      memcpy(&k,pdata,sizeof(int));
      pdata += sizeof(int);
      Bl(k);
      break;
   case 20:
      /* pito */
      pito(181);
      break;
   case 21:
      /* vscroll */
      Patrib &= ~128;
      memcpy(&k,pdata,sizeof(int));
      pdata += sizeof(int);
      memcpy(&j,pdata,sizeof(int));
      pdata += sizeof(int);
      memcpy(&t,pdata,sizeof(int));
      pdata += sizeof(int);
      v_scroll(k,j,t);
      break;
   case 22:
      /* mensaje */
      Patrib &= ~128;
      memcpy(&k,pdata,sizeof(int));
      pdata += sizeof(int);
      mensaje(k,pdata);
      break;
   case 23:
      /* parada */
      Patrib &= ~128;
      server_espera_control++;
      k = parada();
      server_espera_control--;
      pdata2 = tmp;
      memcpy(pdata2,&k,sizeof(int));
      pdata2 += sizeof(int);
      memcpy(pdata2,&eqj,sizeof(int));
      envia_control_a_server(tmp,sizeof(int)*2,2);
      break;
   case 24:
      /* aviso */
      Patrib &= ~128;
      memcpy(&k,pdata,sizeof(int));
      pdata += sizeof(int);
      server_espera_control++;
      k = aviso(k,pdata);
      server_espera_control--;
      pdata2 = tmp;
      memcpy(pdata2,&k,sizeof(int));
      pdata2 += sizeof(int);
      memcpy(pdata2,&eqj,sizeof(int));
      envia_control_a_server(tmp,sizeof(int)*2,3);
      break;
   case 25:
      /* correcto */
      memcpy(&modo_de_validacion,pdata,sizeof(int));
      Patrib &= ~128;
      server_espera_control++;
      k = correcto();
      server_espera_control--;
      pdata2 = tmp;
      memcpy(pdata2,&k,sizeof(int));
      pdata2 += sizeof(int);
      memcpy(pdata2,&eqj,sizeof(int));
      envia_control_a_server(tmp,sizeof(int)*2,3);
      break;
   case 26:
      /* confirmar */
      memcpy(&modo_de_validacion,pdata,sizeof(int));	
      pdata += sizeof(int);
      Patrib &= ~128;
      memcpy(&k,pdata,sizeof(int));
      pdata += sizeof(int);
      pdata2 = pdata + (strlen(pdata)+1);
      server_espera_control++;
      k = confirmar(k,pdata,pdata2);
      server_espera_control--;
      pdata2 = tmp;
      memcpy(pdata2,&k,sizeof(int));
      pdata2 += sizeof(int);
      memcpy(pdata2,&eqj,sizeof(int));
      envia_control_a_server(tmp,sizeof(int)*2,3);
      break;
   case 27:
      /*  */
      Patrib &= ~128;
      memcpy(&k,pdata,sizeof(int));
      pdata += sizeof(int);
      recurre_mensaje_cabecera(k);
      break;
   case 28:
      memcpy(&k,pdata,sizeof(int));
      pdata += sizeof(int);
      borra_mensaje_cabecera(k);
      break;
   case 29:
      memcpy(&k,pdata,sizeof(int));
      pdata += sizeof(int);
      pdata2 = pdata + (strlen(pdata)+1);
      mensaje_en_cabecera(pdata,pdata2,k);
      break;
   case 30:
      /* cabecera */
      mensaje_de_cabecera(pdata);
      break;
   case 31:
      /* menusg */
      menus_(pdata);
      break;
   case 32:
      pdata2 = pdata;
      pdata += (strlen(pdata2)+1);
      
      dir_smenu = x_dir_smenu;
      strcpy(x_dir_smenu,pdata);
      
      pdata += (strlen(dir_smenu)+1);
      
      dir_sgmenu = x_dir_sgmenu;
      strcpy(x_dir_sgmenu,pdata);
      
      convierte_a_cache(pdata2);
      fichero_local_remoto = 1;				  
      leemenu(pdata2);
      fichero_local_remoto = 0;
      break;
   case 33:
      /* wmenu */
      memcpy(&modo_de_validacion,pdata,sizeof(int));
      pdata += sizeof(int);
      memcpy(&k,pdata,sizeof(int));
      pdata += sizeof(int);
      memcpy(&wdef,pdata,sizeof(int));
      pdata += sizeof(int);
      
      dir_smenu = x_dir_smenu;
      strcpy(x_dir_smenu,pdata);
      
      pdata += (strlen(dir_smenu)+1);
      
      dir_sgmenu = x_dir_sgmenu;
      strcpy(x_dir_sgmenu,pdata);
      
      pdata += (strlen(dir_sgmenu)+1);
      
      strcpy(agi_aplicacion_,pdata);
      pdata += (strlen(agi_aplicacion_)+1);
      
      strcpy(agi_empresa_,pdata);
      pdata += (strlen(agi_empresa_)+1);
      
      strcpy(agi_codi_empre_,pdata);
      pdata += (strlen(agi_codi_empre_)+1);
      
      agi_aplicacion = agi_aplicacion_;
      agi_empresa = agi_empresa_;
      agi_codi_empre = agi_codi_empre_;
      
      server_espera_control++;
	  fuerza_empresa = 0;
      j = w_menus(&k);
      server_espera_control--;

		if (abandonarelbarco)
		{
			establece_desconexion(0);
			exit(0);
		}

      if (j >= 0)
      {
         j = j + k * 100;
      }
      memcpy(tmp,&j,sizeof(int));
      memcpy(tmp+sizeof(int),&wdef,sizeof(int));
	  memcpy(tmp+sizeof(int)*2,&fuerza_empresa,sizeof(int));
      envia_control_a_server(tmp,sizeof(int)*3,6);

      break;
   case 34:
      libera();
      break;
   case 35:
      /* w_print */
      memcpy(&ww,pdata,sizeof(ww));
      pdata += sizeof(ww);                  
#ifdef FGVUNIX
      memcpy(&pcurso,pdata,sizeof(int));
      if (pcurso != Pcursor)
      {
         cursor(pcurso/80+pcurso%80+101);
      }
      
#else
      memcpy(&Pcursor,pdata,sizeof(int));
#endif
      
      pdata += sizeof(int);
      w_print(dame_vent(ww),pdata);
      break;
   case 36:
      /* w_mueve */
      memcpy(&ww,pdata,sizeof(ww));
      pdata += sizeof(ww);
      memcpy(&k,pdata,sizeof(int));
      pdata += sizeof(int);
      w_mueve(dame_vent(ww),k);
      break;
   case 37:
      /* bi_cuadro , cuabla , zonablan */
      memcpy(&j,pdata,sizeof(int));
      pdata += sizeof(int);
      memcpy(&k,pdata,sizeof(int));
      pdata += sizeof(int);
      memcpy(&t,pdata,sizeof(int));
      pdata += sizeof(int);
      if (t == -1)
         cuabla(j,k);
      else
         if (t == -2)
            zonablan(j,k);
         else
            bi_cuadro(j,k,t);				  
         break;
   case 38:
      /* revisar version de servidor */				  
	  {
		  int extendida;
		  int ponbarras = 0;
		  extendida = 0;
		  server_ver1 = *pdata;
		  pdata++;
		  server_ver2 = *pdata;
		  pdata++;
		  server_verletra = *pdata;
		  pdata++;
		  server_que_sistema_es = *pdata;
		  pdata++;
		  server_es_sistema_grafico = *pdata;
		  if ((server_es_sistema_grafico & 4))
		  {
			  noboton_usuarios = 1;
			  server_es_sistema_grafico &= ~4;
		  }
		  if (modosinbarras == -1) {
			  ponbarras = 1;
			  if ((server_es_sistema_grafico & 8))
			  {
				  modosinbarras = 1;
				  server_es_sistema_grafico &= ~8;
			  }
			  if ((server_es_sistema_grafico & 16))
			  {
				  modosinbarras = 2;
				  server_es_sistema_grafico &= ~16;
			  }
		  }
		  if ((server_es_sistema_grafico & 64)) 
		  {
			  xtabreturn = 1;
			  server_es_sistema_grafico &= ~64;
		  }
		  if ((server_es_sistema_grafico & 128)) 
		  {
			  busqueda_general = 0;
			  server_es_sistema_grafico &= ~128;
		  }

		  if ((server_es_sistema_grafico & 32)) /* informacion extendida */
		  {
			  extendida = 1;
			  server_es_sistema_grafico &= ~32;
		  }
		  pdata++;             
		  strcpy(server_dirbase1,pdata);
		  pdata += strlen(pdata);
		  pdata++;
		  strcpy(server_dirbase2,pdata);
		  pdata += strlen(pdata);
		  pdata++;	 
		  server_build = *pdata;
		  pdata++;
		  sos_activado = *pdata;
		  pdata++;
		  conexion_lenta = *pdata;
		  pdata++;
		  if (extendida)
		  {			 
			 if (*pdata)
			 {				 
				 pdata++;
				 server_internal_build = *((int *)pdata);
				 pdata += sizeof(int);
				 server_fgvid = *((int *)pdata);
				 pdata += sizeof(int);
			 }			 			 			 
			 if (*pdata)
			 {
				 pdata++;
 				 auto_completar = *((int *)pdata);
				 pdata += sizeof(int);				 
			 }			 
			 if (*pdata)
			 {
				 pdata++;
 				 siempre_password = *((int *)pdata);
				 pdata += sizeof(int);				 
			 }			 
			 if (*pdata)
			 {
				 pdata++;
				 if (ponbarras)
 					modosinbarras = *((int *)pdata);
				 pdata += sizeof(int);				 
			 }			 
			 if (*pdata)
			 {
				 pdata++;
 				 defectoesc = *((int *)pdata);
				 pdata += sizeof(int);				 
			 }			 
			 pdata++;
			 /* reservado ampliaciones ... */
		  }		
		  /*
#ifdef FGVDOS
		  if (server_internal_build != 60) {			  
			  MessageBox(NULL,"El servidor no es correcto!!!","El servidor no es correcto!!!",MB_OK);
			  exit(0);
		  }
#endif
		  */
		  
	  }

      if (soportas_ver1 > server_ver1 || (soportas_ver1 == server_ver1 && soportas_ver2 > server_ver2) || 
         (soportas_letra && soportas_ver1 == server_ver1 && soportas_ver2 == server_ver2 && soportas_letra > server_verletra)
         )
      {
         envia_control_a_server("",0,99);
         establece_desconexion(0);
         aviso(0,__Texto[140]);
         return(2);
      }
      /* peticion de sistema */
      pdata = tmp;
      *pdata = rtme_version1;
      pdata++;
      *pdata = rtme_version2;
      pdata++;
      *pdata = rtme_versionletra;
      pdata++;
      *pdata = que_sistema_es;
      pdata++;
	  if (Pseudo10)
	  {
		 *pdata = 2;
	  }
	  else
	  {
		 *pdata = es_sistema_grafico;
	  }      
	  if (consulta_vieja)
	  {
		 *pdata  |= 32;
	  }
	  if (isslowconection)
	  {
		 *pdata  |= 64;
	  }
	  if (entorno_antivirus)
	  {
		 *pdata  |= 16;
	  }
      pdata++;
      memcpy(pdata,&redita_atrib,sizeof(int));
      pdata += sizeof(int);
      memcpy(pdata,&redita_cc,sizeof(int));
      pdata += sizeof(int);
      memcpy(pdata,&redita_cf,sizeof(int));
      pdata += sizeof(int);
      if (_usando_compresion != -1)				    
      {				       
         usando_compresion = _usando_compresion;
      }
      memcpy(pdata,&usando_compresion,sizeof(int));
      pdata += sizeof(int);
      strcpy(pdata,dir_solicitado);
      if (dir_solicitado[0])
      {
         strcpy(server_dirbase1,dir_solicitado);
         strcpy(server_dirbase2,dir_solicitado);
         strcat(server_dirbase2,"/");
      }
      pdata += (strlen(pdata)+1);
      strcpy(pdata,par1);
      pdata += (strlen(pdata)+1);
      strcpy(pdata,par2);
      if (par2[0])
      {
         strcpy(server_dirbase2,par2);
      }
      pdata += (strlen(pdata)+1);
	  /* la_ethernet_ */
	  *pdata = 6;
	  pdata++;
	  *pdata = 7;
	  pdata++;
	  *pdata = 8;
	  pdata++;	  	  
	  memcpy(pdata,la_ethernet_,6);
	  pdata += 6;
	  *pdata = 0;
	  pdata++;
	  /* dirbass del puesto */
	  *pdata = 8;
	  pdata++;
	  *pdata = 6;
	  pdata++;
	  *pdata = 7;
	  pdata++;
	  strcpy(pdata,dirbass);	  
	  pdata += (strlen(dirbass)+1);
	  *pdata = numero_build;
	  pdata++;

	  /* control de version interna */
	  *pdata = 9;
	  pdata++;
	  *pdata = 6;
	  pdata++;
	  *pdata = 7;
	  pdata++;	  
	  memcpy(pdata,(char *)&fgv_internal_build,sizeof(int));
	  pdata += sizeof(int);

	  /* control excel interno */
	  *pdata = 7;
	  pdata++;
	  *pdata = 9;
	  pdata++;
	  *pdata = 7;
	  pdata++;	  
	  memcpy(pdata,(char *)&excel_alt,sizeof(int));
	  pdata += sizeof(int);


      if (usando_compresion)
      {
         gzip_asignada_memoria = 0;
      }
      envia_control_a_server(tmp,(int)(pdata-tmp),7);
      break;
   case 39:
      /* grafico impresora */
#ifdef FGVWIN				 
      if( _imp == (FILE *)3L)
      {
         convierte_a_cache(pdata);
         printa_grafico(pdata);
      }
#endif
      break;
   case 40:
      /* impresora */
      memcpy(&modo_de_validacion,pdata,sizeof(int));
      pdata += sizeof(int);
      memcpy(&k,pdata,sizeof(int));
      pdata += sizeof(int);
      strcpy(tmp,pdata);
      pdata += (strlen(pdata)+1);
      if (*pdata)
      {
         pdata++;
         pdata2 = pdata;
      }
      else
      {
         pdata2 = NULL;
      }
      server_espera_control++;
	  i3 = 0;
      i2 = _pregu_impre_(k,pdata2,tmp);
      server_espera_control--;
      if (i2 == 2)
      {					
         if (tmp[0] != '@')
         {
            strcpy(tmp2,tmp);
			i3 = 1;
         }					
         else
            strcpy(tmp2,tmp+1);
      }
      else
         tmp2[0] = 0;
      memcpy(tmp,&i2,sizeof(int));
      tmp[sizeof(int)] = 0;
      if (pdata2)
      {
         strcpy(tmp+sizeof(int),pdata2);
      }
      j = sizeof(int)+ strlen(tmp+sizeof(int))+1;
      pdata2 = tmp+j;
      memcpy(pdata2,nombredev,81);
      pdata2 += 81;
      memcpy(pdata2,&i_anchocarro,sizeof(int));
      pdata2 += sizeof(int);
      memcpy(pdata2,oncompri,17);
      pdata2 += 17;
      memcpy(pdata2,offcompri,17);
      pdata2 += 17;
      memcpy(pdata2,onnegrita,17);
      pdata2 += 17;
      memcpy(pdata2,offnegrita,17);
      pdata2 += 17;
      memcpy(pdata2,onexpande,17);
      pdata2 += 17;
      memcpy(pdata2,offexpande,17);
      pdata2 += 17;
      memcpy(pdata2,inicializo,37);
      pdata2 += 37;
      memcpy(pdata2,finalizo,37);
      pdata2 += 37;
      memcpy(pdata2,&i_nlineas,sizeof(int));
      pdata2 += sizeof(int);
      memcpy(pdata2,saltopag,17);
      pdata2 += 17;
      memcpy(pdata2,otrocar,17);
      pdata2 += 17;
      memcpy(pdata2,&es_esclava,sizeof(int));
      pdata2 += sizeof(int);
      memcpy(pdata2,&no_reset,sizeof(int));
      pdata2 += sizeof(int);
	  if (i3 == 1)
	  {
		  inicializo[0] = 0;
		  inicializo[35] = 0;
		  finalizo[0] = 0;
		  finalizo[35] = 0;
	  }
      if (i2 == 2)
	  {
         strcpy(pdata2,tmp2);
         pdata2 += (strlen(tmp2)+1);
         j += (strlen(tmp2)+1);
         /* si viene con el parametro quitarlo */
         i = 0;
         while(_ejspool[i] && _ejspool[i] != ' ') i++;
         _ejspool[i] = 0;
         strcpy(pdata2,_ejspool);
         pdata2 += (strlen(_ejspool)+1);
         j += (strlen(_ejspool)+1);		 
         strcpy(pdata2,quita_path(_fispool));
         pdata2 += (strlen(quita_path(_fispool))+1);
         j += (strlen(quita_path(_fispool))+1);
         _fispool[0] = 0;
         _ejspool[0] = 0;
         _imp = NULL;
	  }
      j += (136 + 74 + 81 + (4 *sizeof(int)));
      envia_control_a_server(tmp,j,8);
      break;
   case 41:
      /* fin_impre */
	  if (datalen >= 300)
		memcpy(numerofax,pdata,300);
      acaba_impresora(NULL);
      break;
   case 42:
      /* imprime esto */				  
      memcpy(&k,pdata,sizeof(int));
      pdata += sizeof(int);
      /*if (_imp)*/
         _graba_linea(pdata,_imp,k);
      break;
   case 43:
      /* el servidor va ejecutar otro programa de la familia RECONECTAR */				  
      no_entres_en_terminal = 1;
      {
         int g = conecta_a_otro_server(pdata);
         no_entres_en_terminal = 0;
         if (g == 2 || g == 3 || g == 4 || g == 5)
         {
            return(g);
         }
      }				  
      break;
   case 44:
      /* usuario */
      memcpy(&user_who,pdata,sizeof(int));
      pdata += sizeof(int);
      memcpy(&subnumero_de_usuario,pdata,sizeof(int));
      pdata += sizeof(int);
	  if (strlen(pdata) > 10)
	  {
		  pdata[10] = 0;
	  }
      strcpy(rw_user,pdata);
      break;
   case 45:
      user_who = -1;
      subnumero_de_usuario = 0;
      rw_user[0] = 0;
      break;
   case 46:
      memcpy(&k,pdata,sizeof(int));
      recurre_boton(k);
      break;
   case 47:
      fin_botones_();
      break;
   case 48:
      prepara_menusg_botones();
      break;
   case 49:
      memcpy(&k,pdata,sizeof(int));
      pdata += sizeof(int);
      memcpy(&j,pdata,sizeof(int));
      pdata += sizeof(int);
      memcpy(&i,pdata,sizeof(int));
      pdata += sizeof(int);
      memcpy(&t,pdata,sizeof(int));
      pdata += sizeof(int);
      pon_mapa_raton(k,j,i,t);
      break;
   case 50:
      memcpy(&k,pdata,sizeof(int));
      pdata += sizeof(int);
      anula_mapa_raton(k);
      break;
   case 51:
      memcpy(&k,pdata,sizeof(int));
      pdata += sizeof(int);
      recurre_raton(k);
      break;
   case 52:
      memcpy(&k,pdata,sizeof(int));
      ptec(k);
      break;
   case 53:
      server_espera_control++;
      k = pulsatecla();
      server_espera_control--;
      envia_control_a_server(&k,sizeof(k),9);
      break;
   case 54:
      fichero_local_remoto = 0;
      break;
   case 55:
      /* rentra */
      llama_entra(pdata);
      break;
   case 56:
      /* timeout de teclado */
      memcpy(&k,pdata,sizeof(int));
      pdata += sizeof(int);
      memcpy(&j,pdata,sizeof(int));
      pdata += sizeof(int);
      memcpy(&i,pdata,sizeof(int));
      pdata += sizeof(int);
      memcpy(&t,pdata,sizeof(int));
      pdata += sizeof(int);
      pon_tecla_timeout(k,j,i,t);
      break;				  
   case 57:
      /* opcion_ej */	  
	  pdata3 = pdata;
      strcpy(tmp,pdata);
      pdata += (strlen(pdata)+1);
      strcpy(nomapli,pdata);
      pdata += (strlen(pdata)+1);
      if (!strcmp(nomapli,"diagram9"))
      {
#ifndef DSMINIBASICO      
         inicio_dir();
#endif
      }
      else
      {
		 compon_dirbase(dirbase,dirbass,nomapli);
         /*sprintf(dirbase,"%s%s/",dirbass,nomapli);*/
         cambia_paths();
         lee_permisos(nomapli);
      }
      if (*pdata)
      {
         pdata2 = tmp2;
         pdata++;
         strcpy(tmp2,pdata);
		 pdata += (strlen(pdata)+1);
      }
      else
      {
		 pdata++;
         pdata2 = NULL;
      }
	  if ((int)(pdata-pdata3) < datalen)
	  {
		  if (*pdata == 2)
		  {
			  /* envia informacion de proteccion */			  
			  pdata++;

			  memcpy(&nume_serie,pdata,sizeof(nume_serie));
			  pdata += sizeof(nume_serie);
			  memcpy(&nume_serie_real,pdata,sizeof(nume_serie_real));
			  pdata += sizeof(nume_serie_real);
			  memcpy(local_term,pdata,sizeof(local_term));
			  pdata += sizeof(local_term);
			  memcpy(local_term_,pdata,sizeof(local_term_));
			  pdata += sizeof(local_term_);
			  memcpy(local_acce_,pdata,sizeof(local_acce_));
			  pdata += sizeof(local_acce_);
			  memcpy(&nume_code,pdata,sizeof(nume_code));
			  pdata += sizeof(nume_code);
			  memcpy(&posicion,pdata,sizeof(posicion));
			  pdata += sizeof(posicion);
			  memcpy(&nume_mochila,pdata,sizeof(nume_mochila));
			  pdata += sizeof(nume_mochila);			  
			  if (datoscomunes)
			  {
				  *(datoscomunes->nume_serie) = nume_serie;
				  *(datoscomunes->server_nume_serie) = server_nume_serie;
				  *(datoscomunes->nume_serie_real) = nume_serie_real;
				  memcpy(datoscomunes->local_term,local_term,sizeof(local_term));
				  memcpy(datoscomunes->local_term_,local_term_,sizeof(local_term_));
				  memcpy(datoscomunes->local_acce_,local_acce_,sizeof(local_acce_));				  
			  }			  
		  }
	  }

#ifndef DSMINIBASICO
      en_conexion_terminal = 0;
	  if (datoscomunes)
		*(datoscomunes->en_conexion_terminal) = 0;
      i = opcion_ej(tmp,pdata2);
	  if (datoscomunes)
		*(datoscomunes->en_conexion_terminal) = 1;
      en_conexion_terminal = 1;
#endif
      libera_permisos();
      envia_control_a_server(&i,sizeof(int),16);
      break;
      /* 58 == 16 */
   case 59: /* activar en el terminal sistema de mensajes */
      /****************/
	   memcpy(&server_nume_serie,pdata,sizeof(long));
	   if (xfer->hdr.length >= (HDRSIZE+1+sizeof(long)*2))
	   {
		   memcpy(&pass_cambio_autorizado,pdata+sizeof(long),sizeof(long));	   
	   }
      break;
   case 60:
      /* desactivar sistema de mensajes */
      /****************/
      break;
   case 61: /* funciones windows */
      memcpy(&k,pdata,sizeof(int));
      pdata += sizeof(int);				 
      switch(k)
      {
      case 0:
#ifdef FGVDOS
         graba_bmp_pantalla(pdata);
#endif
         break;
      case 1:
#ifdef FGVDOS
         en_daclaves(pdata);
#endif
         memcpy(&ll,pdata,sizeof(ll));					  
         envia_control_a_server(pdata,ll+(sizeof(ll)*2),17);
         break;
      case 2:					  
         memcpy(&i2,pdata,sizeof(int));
         pdata += sizeof(int);
         pdata2 = pdata + (strlen(pdata)+1);
         pdata3 = pdata2 + (strlen(pdata2)+1);
#ifdef FGVDOS
         i2 = _dde_init(pdata,pdata2,pdata3,i2);
#endif
         envia_control_a_server(&i2,sizeof(i2),23);
         break;
      case 3:
         pdata2 = pdata + (strlen(pdata)+1);
         pdata3 = pdata2 + (strlen(pdata2)+1);
#ifdef FGVDOS
         i2 = _dde_poke(pdata,pdata2,pdata3);
#endif
         envia_control_a_server(&i2,sizeof(i2),26);
         break;
      case 4:
         pdata2 = pdata + (strlen(pdata)+1);
#ifdef FGVDOS
         i2 = _dde_exec(pdata,pdata2);
#endif					  
         envia_control_a_server(&i2,sizeof(i2),27);
         break;
      case 5:
#ifdef FGVDOS
         i2 = _dde_fin();
#endif
         envia_control_a_server(&i2,sizeof(i2),24);
         break;
      case 6:
         pdata2 = pdata + (strlen(pdata)+1);					  
         tmp[sizeof(int)] = 0;
#ifdef FGVDOS                      
         pdata3 = NULL;
         i2 = _dde_peek(&pdata3,pdata,pdata2);
         if (i2 >= 0 && pdata3)
         {
            strcpy(tmp+sizeof(int),pdata3);
            free(pdata3);
         }					  
#endif
         memcpy(tmp,&i2,sizeof(int));
         envia_control_a_server(tmp,sizeof(int)+strlen(tmp+sizeof(int))+2,25);
         break;
      case 7:
         /* carga de dll */
#ifdef FGVDOS                      
         i2 = carga_libreria_usuario(pdata);
#endif
         memcpy(tmp,&i2,sizeof(int));
         envia_control_a_server(tmp,sizeof(int),34);
         break;  
      case 8:
         /* descarga de dll */
#ifdef FGVDOS                      
         i2 = descarga_libreria_usuario(pdata);
#endif
         memcpy(tmp,&i2,sizeof(int));
         envia_control_a_server(tmp,sizeof(int),35);
         break;
      case 9:
         /* alfacall a dll */
         pdata2 = pdata + (strlen(pdata)+1);
         pdata3 = pdata2 + (strlen(pdata2)+1);
         strcpy(tmp+sizeof(int),pdata3);
#ifdef FGVDOS                      
         i2 = alfacall_libreria_usuario(pdata,pdata2,tmp+sizeof(int));
#endif
         memcpy(tmp,&i2,sizeof(int));
         envia_control_a_server(tmp,sizeof(int)+strlen(tmp+sizeof(int))+1,36);
		 break;
      case 10:
         if (DespachadorExterno)
         {
            int funcion;
            int parametro;
            char *puntero;
            memcpy(&funcion,pdata,sizeof(int));
            pdata += sizeof(int);
            memcpy(&parametro,pdata,sizeof(int));
            pdata += sizeof(int);
            if (*pdata)
               puntero = pdata+1;
            else
               puntero = NULL;
            pdata++;
            switch(funcion)
            {
			case DESPACHO_BINARIO:
			   if (!puntero)
			      {
				  (*DespachadorExterno)(funcion,parametro,puntero);
                  break;
				  }	   
			   else
			      {
				  long tam,respuesta,recibidos,modo;
				  char parcial;
				  char *buffer;
				  int len = 0,waitfor;

		          memcpy(&tam,puntero,sizeof(long));
		          puntero += sizeof(long);
                  memcpy(&respuesta,puntero,sizeof(long));
		          puntero += sizeof(long);
                  memcpy(&modo,puntero,sizeof(long));
		          puntero += sizeof(long);
                  parcial = *puntero;
				  puntero++;

				  if (tam > respuesta)
				     buffer = malloc(tam);
				  else
				     buffer = malloc(respuesta);

		          memcpy(buffer,&tam,sizeof(long));
		          len = sizeof(long);
				  tam -= sizeof(long);
                  memcpy(buffer+len,&respuesta,sizeof(long));
		          len += sizeof(long);
				  tam -= sizeof(long);
                  memcpy(buffer+len,&modo,sizeof(long));
		          len += sizeof(long);
				  tam -= sizeof(long);			  

				  if (parcial)
				     {
                     memcpy(&recibidos,puntero,sizeof(long));
		             puntero += sizeof(long);		 
					 }
				  else
				     recibidos = tam;

				  memcpy(buffer+len,puntero,recibidos);
				  len += recibidos;
				  tam -= recibidos;
				  waitfor = 66;
                  while(tam && parcial)
				     {
			         if ((recibidos = _leecontrol_en_server(NULL,0,&waitfor)) < 0)
					    {
						free(buffer);
						break;
						}			         
					 memcpy(buffer+len,xrcbbuf.data+1,recibidos);
                     len += recibidos;
					 tam -= recibidos;
					 }
				  (*DespachadorExterno)(funcion,parametro,buffer);
				  if ((modo&1))
				     {					 					 
                     memcpy(&tam,buffer,sizeof(long));
					 memcpy(&tam,buffer+sizeof(long),sizeof(long)); /* tamaño de la respuesta!! */
					 i2 = sizeof(long);                    					 

					 memcpy(tmp,&tam,sizeof(long));
					 tam -= sizeof(long);
					 len = sizeof(long);
	                 if (tam > 10000)
	                    {
	                    recibidos = 10000;
		                tmp[len] = 1;
						len++;
						memcpy(tmp+len,&recibidos,sizeof(long));
						len += sizeof(long);
		                }
	                 else
	                    {
	                    recibidos = tam;
		                tmp[len] = 0;
						len++;
		                }
					 memcpy(tmp+len,buffer+i2,recibidos);
					 i2 += recibidos;
                     tam -= recibidos;
					 len += recibidos;
					 envia_control_a_server(tmp,len,66);

					 while(tam)
					   {
	                   if (tam > 10000)
	                      {
						  recibidos = 10000;
						  }
					   else
					      {
						  recibidos = tam;
						  }
                       envia_control_a_server(buffer+i2,recibidos,66);
					   i2 += recibidos;
					   tam -= recibidos;
					   }
					 }
				  free(buffer);
				  break;
				  }
				  			   
			   break;
            case DESPACHO_EN_LINEAS:
#ifdef FGVWIN
               if (!parametro || parametro == 100)
               {
                  int pi,pf;
                  RECT r;
                  memcpy(&pi,pdata,sizeof(int));
                  pdata += sizeof(int);
                  memcpy(&pf,pdata,sizeof(int));
                  pdata += sizeof(int);
                  ConvierteTxtARect(pi,pf,&r);
                  (*DespachadorExterno)((int)DESPACHO_EN_LINEAS,parametro,(void *)&r);
               }
               else
               {
                  (*DespachadorExterno)((int)DESPACHO_EN_LINEAS,parametro,puntero);
               }
#endif
               break;
            case DESPACHO_OMENU:
               /* no deberia venir por transporte de red!!! */
               break;
            case DESPACHO_MI_CONTROL:
               {
                  int retorno = 0;
                  (*DespachadorExterno)((int)DESPACHO_MI_CONTROL,parametro,puntero);
                  memcpy(&retorno,puntero,sizeof(int));
                  envia_control_a_server(&retorno,sizeof(int),46);
               }
               break;
            case DESPACHO_EDITA:
				{
                  (*DespachadorExterno)((int)DESPACHO_EDITA,parametro,puntero);
                  envia_control_a_server(puntero,sizeof(int)+strlen((char *)puntero+sizeof(int))+1,47);
				}
               break;
			case DESPACHO_USUARIOS:
               (*DespachadorExterno)((int)DESPACHO_USUARIOS,parametro,puntero);
               envia_control_a_server(puntero,strlen((char *)puntero)+1,64);
               break;
			case DESPACHO_TRAEFICHERO:                
               sprintf(tmp,"%d",rw_envia_fichero((char *)puntero,777));
               envia_control_a_server(tmp,strlen((char *)tmp)+1,65);
               break;            
			case DESPACHO_CONSULTA_REMOTA:
			   consulta_local(puntero);
			   envia_control_a_server(puntero,strlen((char *)puntero)+1,65);
			   break;
			case DESPACHO_FUNCIONES_IMPRESION:
               (*DespachadorExterno)((int)DESPACHO_FUNCIONES_IMPRESION,parametro,puntero);
			   if (parametro == 998 || parametro == 8) {
				   envia_control_a_server(puntero,300,65);
			   }
			   break;
            case DESPACHO_PANTALLAS:
               if (parametro > 100 && puntero)
               {
                  char *arrai[100];
                  int ppi;
                  for (ppi = 0;ppi  < (parametro - 100);ppi++)
                  {
                     arrai[ppi] = puntero;
                     puntero += (strlen(puntero)+1);
                  }
                  (*DespachadorExterno)(funcion,parametro,(char *)arrai);
               }
			   else
                  (*DespachadorExterno)(funcion,parametro,puntero);
			   break;
			case DESPACHO_PONINI:
			   if (parametro == 0)
				   pon_en_ini((char *)puntero);
			   break;
            default:
			   if (!abandonarelbarco)
					(*DespachadorExterno)(funcion,parametro,puntero);
			   if (funcion == DESPACHO_FOCOCONTROL)
			   {
				   envia_control_a_server("",1,65);
			   }
			   else if (funcion == DESPACHO_MULTIPLE || funcion == DESPACHO_MENUITEM || funcion == DESPACHO_INICIO_TRAZA || funcion == DESPACHO_FIN_TRAZA || funcion == DESPACHO_PUNTO_TRAZA || funcion == DESPACHO_ENLACEWORD || funcion == DESPACHO_BROWSE || funcion == DESPACHO_CONSULTA_REMOTA || (funcion == DESPACHO_SUPERUSUARIO && puntero))
			   {
				  envia_control_a_server(puntero,strlen((char *)puntero)+1,65);
			   }
               break;
            }					     
         }
         break;
      case 11:
		 {
			int *p1 = (int *)pdata;
            if (!p1[0])
				_M_EXTRA_AVANCE = 0;
			no_pintar_ahora(p1[0]);			
		 }
         break;
      case 12:
         regulariza_menua_(pdata);
         break;
      case 13:
		 if ((xfer->hdr.length-HDRSIZE-sizeof(int)) > 1)
		 {
			memcpy(&wdef,pdata,sizeof(int));
		 }
         w_menus(NULL);
         break;
      case 14:
         pdata2 = pdata + (strlen(pdata)+1);
         strcpy(tmp,pdata2 + (strlen(pdata2)+1));
#ifdef FGVWIN
         dialogo_activacion(pdata,pdata2,tmp);
#else
         cuadro_activacion(pdata,pdata2,tmp);
#endif				
         envia_control_a_server(tmp,strlen(tmp)+1,44);
         break;
      case 15:
         {
            int ei = system_proceso(pdata);
            envia_control_a_server("",1,45);
         }
         break;
      case 16:
         i2 = rw_base_unlink(pdata);
         envia_control_a_server(&i2,sizeof(int),46);
         break;
      case 17:					  
         i2 = fgv_i_pdir(pdata);
         memcpy(tmp,&i2,sizeof(int));
         strcpy(tmp+sizeof(int),pdata);
         envia_control_a_server(tmp,sizeof(int)+strlen(pdata)+1,47);
         break;
      case 18:
         i2 = fgv_i_sdir(pdata);
         memcpy(tmp,&i2,sizeof(int));
         strcpy(tmp+sizeof(int),pdata);
         envia_control_a_server(tmp,sizeof(int)+strlen(pdata)+1,48);
         break;
      case 19:
         i2 = copiarfich(pdata,pdata+300,0);
         envia_control_a_server(&i2,sizeof(int),49);
         break;
      case 20:
         i2 = rw_base_rename(pdata,pdata+300);
         envia_control_a_server(&i2,sizeof(int),49);
         break;
      case 21:
         i2 = copiarfich(pdata,pdata+300,1);
         envia_control_a_server(&i2,sizeof(int),49);
         break;
      case 22:
         i2 = crea_dir_total(pdata);
         envia_control_a_server(&i2,sizeof(int),50);
         break;
      case 23:
         i2 = rmdir(pdata);
         envia_control_a_server(&i2,sizeof(int),51);
         break;
      case 24:
         i2 = lee_y_borra(pdata);
         envia_control_a_server(&i2,sizeof(int),52);
         break;
      case 25:
         i2 = agi_compress(pdata,0);
         envia_control_a_server(&i2,sizeof(int),53);
         break;					  
      case 26:
         i2 = agi_compress(pdata,1);
         envia_control_a_server(&i2,sizeof(int),54);
         break;					  
      case 27:
         i2 = agi_detar(pdata,pdata+300);
         envia_control_a_server(&i2,sizeof(int),55);
         break;					  
      case 28:
         i2 = agi_atar(pdata,pdata+300);
         envia_control_a_server(&i2,sizeof(int),56);
         break;
      case 29:
         /* comprobar registro ayuda windows */
         {
            long registro = 0L;
            sprintf(tmp,"%s%s/ayuda/%s.hlp",rw_directorio("remoto/"),pdata,pdata);
            crea_dir(tmp);
            cambia_barra(tmp);
            {
               FILE *fx;
               fx = fopen(tmp,"r");
               if (fx)
               {
                  fclose(fx);
                  sprintf(tmp,"%s%s/ayuda/control.srv",rw_directorio("remoto/"),pdata);
                  cambia_barra(tmp);
                  fx = fopen(tmp,"r");
                  if (fx)
                  {
                     if (lee_linea(tmp,50,fx))
                     {
                        registro = atol(tmp);
                     }
                     fclose(fx);
                  }
               }
            }
            envia_control_a_server(&registro,sizeof(long),57);
         }
         break;
      case 30:
         /* crear ayuda por orden del servidor */
         {
            long registro;
            memcpy(&registro,pdata,sizeof(long));
            pdata2 = pdata + sizeof(long);
            sprintf(tmp,"%s%s/ayuda/control.srv",rw_directorio("remoto/"),pdata2);
            crea_dir(tmp);
            {
               FILE *fx;
               fx = fopen(tmp,"w");
               if (fx)
               {
                  fprintf(fx,"%ld\n",registro);
                  fclose(fx);
               }
            }
            sprintf(tmp,"%s%s/ayuda/",rw_directorio("remoto/"),pdata2);					   
            i2 = agi_detar(rw_directorio(pdata2+100),tmp);
            unlink(rw_directorio(pdata2+100));
#ifdef FGVDOS
            if (dlg) 
            {
               DestroyWindow(dlg);
               dlg = NULL;
            }
            sprintf(tmp,"%s%s/",rw_directorio("remoto/"),pdata2);                       
            ds_rtfhlp(tmp,"Actualizando la ayuda de %s desde el servidor...");
#endif
            envia_control_a_server(&i2,sizeof(int),58);
         }
         break;
      case 31:
         /* presentar ayuda en modo cliente (directorio remoto) */
#ifdef FGVDOS
         if (dlg) 
         {
            DestroyWindow(dlg);
            dlg = NULL;
         }
#endif
         {
            char _referencia_help[128];
            char *_dirbase_ayuda = dirbase_ayuda;
            char *_nomapli_ayuda = nomapli_ayuda;
            
            memcpy(_referencia_help,referencia_help,128);
            
            strcpy(referencia_help,pdata+200);
            
            if (strcmp(pdata,"diagram9"))
               sprintf(tmp,"%s%s/",rw_directorio("remoto/"),pdata);
            else
               sprintf(tmp,"%s",rw_directorio("ayuda/"));
            crea_dir(tmp);
            cambia_barra(tmp);
            
            dirbase_ayuda = tmp;
            nomapli_ayuda = pdata;
            i2 = agiayuda(pdata+100);
            
            memcpy(referencia_help,_referencia_help,128);
            dirbase_ayuda = _dirbase_ayuda;
            nomapli_ayuda = _nomapli_ayuda;
            envia_control_a_server(&i2,sizeof(int),59);
         }
         break;
      case 32:
         i2 = 0;
#ifdef FGVDOS
         {
#ifdef FGVWIN
            HINSTANCE  hInstancia = hInst;
#else
            HINSTANCE  hInstancia = GetModuleHandle (NULL);
#endif;                           
            MSG msg;
            extern BOOL CALLBACK _CREA_AYUDA(HWND hWnd, UINT message,WPARAM wParam, LPARAM lParam);
            dlg  = CreateDialogParam(  hInstancia,  // handle to application instance
               "CREA_AYUDA",  // identifies dialog box template
               NULL,      // handle to owner window
               _CREA_AYUDA, // pointer to dialog box procedure      
               (LPARAM)NULL);   // initialization value
            if (dlg)
            {	                      
               sprintf(tmp,"Transfiriendo la ayuda de %s desde el Servidor",pdata);	   
               SetWindowText(dlg,tmp);
               BringWindowToTop(dlg);
               SetForegroundWindow(dlg);           	                      
               while (PeekMessage((LPMSG)&msg,dlg,0,0,PM_NOREMOVE) && GetMessage((LPMSG)&msg,dlg,0,0))
               {
                  if (!IsDialogMessage(dlg,&msg))
                  {
                     TranslateMessage((LPMSG)&msg);
                     DispatchMessage((LPMSG)&msg);
                  }	     
               }
               i2 = 1;         
            }
         }
#endif
         envia_control_a_server(&i2,sizeof(int),60);
         break;
      case 33: /* que directorio? trasmision de ficheros */
         tmp[0] = 1;
         tmp[1] = 0;
         if (DespachadorExterno)
         {	
            strcpy(tmp,pdata);
            if (*pdata == 2 && *(pdata+1) == 2)
            {							  
               (*DespachadorExterno)((int)DESPACHO_TRASMISION,0,(void *)tmp);
            }
            else
               if (*pdata == 3 && *(pdata+1) == 3)
               {
                  (*DespachadorExterno)((int)DESPACHO_TRASMISION,1,(void *)tmp);
               }						  
         }	
         envia_control_a_server(tmp,strlen(tmp)+1,62);
         break;
      case 34:
         strcpy(tmp,pdata);
         temporal_(tmp);
         envia_control_a_server(tmp,strlen(tmp)+1,46);
         break;
      case 35:
         i2 = fgv_arranca(pdata);         
         envia_control_a_server(&i2,sizeof(int),77);
         break;
	  case 36:
#ifdef FGVWIN
		  {
		  HANDLE PortInitialize (LPTSTR lpszPortName);
          strcpy(tmp,pdata);
		  i2 = (int)PortInitialize(tmp);
		  }
#else
		  i2 = 0;
#endif
		 envia_control_a_server(&i2,sizeof(int),77);
		 break;
	  case 37:
#ifdef FGVWIN
		  {
		  int LeePort(HANDLE hPort,int max,char *buf);
		  i2 = *((int *)pdata);
		  i3 = *((int *)(pdata+sizeof(int)));		  
		  i2 = LeePort((HANDLE)i2,i3,tmp+sizeof(int));
		  *((int *)tmp) = i2;
		  }
#else
		  i2 = 0;
		  *((int *)tmp) = i2;
#endif
		 envia_control_a_server(tmp,sizeof(int)+i2,77);
		 break;
	  case 38:
#ifdef FGVWIN
		  {
		  void PortWrite(HANDLE hPort,char *buf);
		  i2 = *((int *)pdata);
		  strcpy(tmp,pdata+sizeof(int));
		  PortWrite((HANDLE)i2,tmp);
		  i2 = 0;
		  *((int *)tmp) = i2;
		  }
#else
		  i2 = 0;
		  *((int *)tmp) = i2;
#endif
		 envia_control_a_server(tmp,sizeof(int),77);
		 break;
	  case 39:
#ifdef FGVWIN
		  {
		  BOOL PortClose (HANDLE hPort);
		  i2 = *((int *)pdata);		  
		  i2 = (int)PortClose ((HANDLE)i2);
		  }
#else
		  i2 = 0;		  
#endif
		 envia_control_a_server(&i2,sizeof(int),77);
		 break;
	  case 40:
		  {
			  char resultado[33];
			  calcula_md5(pdata,resultado);
			  envia_control_a_server(resultado,33,78);
		  }
		  break;
	  case 41:
		  {
			  remoto_maneja_def_temporal(pdata);
		  }
		  break;
      default:
         break;
                }
                break;
            case 62:
               /* fopen en terminal */
               memcpy(&i,pdata,sizeof(int));
               pdata += sizeof(int);
               pdata2 = pdata;
               pdata2 += (strlen(pdata)+1);
               if (!i)
                  sprintf(tmp,"%s%s",dirbass,pdata);
               else
                  strcpy(tmp,pdata);
               ff = fopen(tmp,pdata2);
               envia_control_a_server(&ff,sizeof(FILE *),18);
               break;
            case 63:
               memcpy(&ff,pdata,sizeof(FILE *));
               i = fclose(ff);
               envia_control_a_server(&i,sizeof(int),19);
               break;
            case 64:
               memcpy(&i,pdata,sizeof(int));
               pdata += sizeof(int);
               memcpy(&ff,pdata,sizeof(FILE *));
               tmp[sizeof(char *)] = 0;
               pdata2 = lee_linea(tmp+sizeof(char *),i,ff);
               memcpy(tmp,&pdata2,sizeof(char *));
               if (!pdata2)
               {
                  tmp[sizeof(char *)] = 0;
                  envia_control_a_server(tmp,sizeof(char *)+1,20);
               }
               else
               {
                  pdata2 = tmp + sizeof(char *);				  
                  envia_control_a_server(tmp,strlen(pdata2)+1+sizeof(char *),20);
               }
               break;
            case 65:
               memcpy(&j,pdata,sizeof(int));
               pdata += sizeof(int);
               memcpy(&i,pdata,sizeof(int));
               pdata += sizeof(int);
               if (!i)
               {
                  i = O_BINARY | O_RDWR | O_CREAT | O_TRUNC;
               }
               else
                  if (i == 1)
                  {
                     i = O_BINARY | O_RDWR | O_CREAT | O_APPEND;
                  }
				  else if (i == 3)
				  {
					 i = O_BINARY | O_RDONLY;
				  }
                  else
                  {
                     i = O_BINARY | O_RDWR;
                  }
                  if (j)
                     strcpy(tmp,pdata);
                  else
                     sprintf(tmp,"%s%s",dirbass,pdata);
                  i = rw_base_open_(tmp,i,S_IREAD | S_IWRITE);
                  envia_control_a_server(&i,sizeof(int),21);
                  break;
            case 66:
               memcpy(&j,pdata,sizeof(int));
               pdata += sizeof(int);
               memcpy(&i,pdata,sizeof(int));
               pdata += sizeof(int);				  
               i = rw_base_write_(i,pdata,j);
               envia_control_a_server(&i,sizeof(int),22);
               break;
            case 67:
               memcpy(&i,pdata,sizeof(int));
               pdata += sizeof(int);				  
               i = rw_base_close_(i);
               envia_control_a_server(&i,sizeof(int),23);
               break;
            case 68:                  
               sprintf(tmp,"%s%s",dirbass,pdata);
               i = _ejecutas_(tmp,1); 
               envia_control_a_server(&i,sizeof(int),23);
               break;
            case 69:
               /* el servidor reasigna factor de compresion */				  
               memcpy(&usando_compresion,pdata,sizeof(int));
               break;
            case 70:
               /* libre */
               break;
            case 71:
               /* libre */
               break;
            case 72:
               /* ayuda_remota */
               pdata2 = pdata;
               pdata2 += (strlen(pdata)+1);
               if (*pdata2)
               {
                  pdata2++;
                  dirbase_ayuda = pdata2;
                  pdata2 += (strlen(dirbase_ayuda)+1);
               }
               else
               {
                  pdata2++;
                  dirbase_ayuda = NULL;
               }
               if (*pdata2)
               {
                  pdata2++;
                  nomapli_ayuda = pdata2;
                  pdata2 += (strlen(nomapli_ayuda)+1);
               }
               else
               {
                  pdata2++;
                  nomapli_ayuda = NULL;
               }
               strcpy(referencia_help,pdata2);
               i = 0;
               envia_control_a_server(&i,sizeof(int),33);
               if (!help_externo)
               {
                  help_externo = "default";
                  agiayuda(pdata);
                  help_externo = NULL;
               }
               else
                  agiayuda(pdata);
               break;				
            case 73:
               memcpy(&j,pdata,sizeof(int));
               pdata += sizeof(int);
               memcpy(&i,pdata,sizeof(int));
               pdata += sizeof(int);				  
               i = rw_base_read_(i,tmp+sizeof(int),j);
               memcpy(tmp,&i,sizeof(int));
               if (i < 0)
                   i = 0;
               envia_control_a_server(tmp,sizeof(int)+i,24);
               break;
            case 74:
               memcpy(&j,pdata,sizeof(int));
               pdata += sizeof(int);
               memcpy(&i,pdata,sizeof(int));
               pdata += sizeof(int);
               memcpy(&ll,pdata,sizeof(long));
               pdata += sizeof(long);
               ll = rw_base_lseek_(i,ll,j);
               envia_control_a_server(&ll,sizeof(long),25);
               break;
            case 75:
               memcpy(&ff,pdata,sizeof(FILE *));
               pdata += sizeof(FILE *);
               i = graba_linea(pdata,ff);
               envia_control_a_server(&i,sizeof(int),26);
               break;
            case 76:
               memcpy(&ff,pdata,sizeof(FILE *));
               pdata += sizeof(FILE *);
               memcpy(&j,pdata,sizeof(int));
               pdata += sizeof(int);
               memcpy(&ll,pdata,sizeof(long));
               pdata += sizeof(long);
               if (ll || j != SEEK_CUR)
               {
                  fseek( ff, ll, j );
               }
               ll = ftell(ff);
               envia_control_a_server(&ll,sizeof(long),27);
               break;
            case 100:
               /* el terminal ha sido rechazado */
               establece_desconexion(0);
               aviso(0,__Texto[141]);
               return(2);
            case 103:
               aviso(0,__Texto[142]);
            case 101:
               /* fin de la comunicacion normal*/
               establece_desconexion(0);
               return(2);
            case 102:
               /* reinicializa */
               flush_mysock();
               xferbuf.hdr.signature = RWNIDENTIFICACION;
               xferbuf.hdr.command = CONTROL_DE_TERMINAL;  
               xferbuf.hdr.length = HDRSIZE + 1;
               xferbuf.data[0] = 99; /* estoy listo */			   
               rwi_senddata(MySock.sock,(char *)&xferbuf,xferbuf.hdr.length);
               sleep(2);
               establece_desconexion(0);
               return(3);
			case 203:
				return 6;
			case 204:
				return 7;
			case 205:
				return 8;
			case 206:
				return 9;
            case 201:
               /* Ready!!!!: El servidor a completado la tarea asignada, resultado negativo */
               return(5);            
			case 202:
				
			   /* Ready si comprobar cookie es ok! */
#ifdef FGVDOS				
				{
					FILE *f;
					f = fopen(rw_directorio("logs/cookcheck.log"),"w");
					if (f)
						fprintf(f,"->1\r\n");
					
				   _modo_portal = 1;
				   if (!hay_una_geco_cookie(__direccion_nwtcon,NULL,f))
				   {			
						  if (f)
						 	 fprintf(f,"->2\r\n");
					      en_conexion_terminal = 0;
						  aviso(0,"No dispone de acceso registrado para este servidor");
						  en_conexion_terminal = 1;
						  if (f)
						 	 fprintf(f,"->3\r\n");
						  envia_control_a_server("",0,75);
						  if (f)
						 	 fprintf(f,"->!!\r\n");
						  if (f)
						    fclose(f);
						  return(5);
				   }
				   
				   if (f)
					 fprintf(f,"->!\r\n");
				   if (f)
					 fclose(f);
				}
#endif
		   case 200:
               /* Ready!!!!: El servidor a completado la tarea asignada, resultado positivo */
               return(4);
            default:
#ifdef FGVDOS
               /*MessageBox(NULL,"MAL MENSAJE DEL SERVIDOR (Orden desconocida)","ERROR",MB_OK);*/
               Beep(1000,50);
#endif
               break;
                }
                return(0);
}


#ifdef WIN32
typedef int (WINAPI *__fnDsaucert)(const char *name,char *holder);

int ds_busca_certificado(char *name,char *holder)
{
	HINSTANCE lib;
    __fnDsaucert llamada= NULL;

	holder[0] = 0;

	lib = LoadLibrary("dsaucert.dll");
	if (lib)
	{
		llamada = GetProcAddress(lib,"fnDsaucert");
		if (llamada)
		{
			if (llamada((const char *)name,holder) != 1)
			{
				holder[0] = 0;
			}
		}
		FreeLibrary(lib);
	}

	return 0;

}

#endif

int envia_subejecuta_a_server(par) /* subejecuta */
char *par;
{
   char tmp[2048];
   char *dat;
   int i;
   char _fichemenu[128];
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
   char *_dato_entra;
   int _dato_entra_len;
   int _pulsa_en_espacio = pulsa_en_espacio;
   int ojo = 0;
#ifdef FGVWIN
   char ***_wwmenus = wwmenus; /* opciones */
   int *_nwwmenus = nwwmenus;  /* numero de opciones para liberacion */
   int _en_menu = en_menu;
   int _n_wmenus = n_wmenus;
   HMENU _principal_menu = principal_menu;
   HMENU tmp_menu;
   char **_tabla_icon = tabla_icon;
   short *_punto_tabla = punto_tabla;
   int _ntabla_icon = ntabla_icon;
   
   
   
   
   pulsa_en_espacio = -1; /* solo para menusg!!! */
   n_wmenus = 0;
   wwmenus = NULL;
   nwwmenus = NULL;
   tabla_icon = NULL;
   punto_tabla = NULL;
   ntabla_icon = 0;
   
   en_menu = 0;
   if (DespachadorExterno)
   {
      (*DespachadorExterno)((int)DESPACHO_EMENU,(int)en_menu,(void *)NULL);
   }
   
   if (principal_menu)
   {
      principal_menu = NULL;
      tmp_menu = LoadMenu(hInst, "AGIXMENU");
      SetMenu(principal_w, tmp_menu);		
      RedrawWindow(principal_w,NULL,NULL,RDW_FRAME | RDW_INVALIDATE);
   }
#endif
   
   dat = tmp;
   
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
   
   
   
   if (!server_espera_control) 
   {   
	   ojo = 1;
      /*pito(182);
      return(-1);*/
   }
   
   memcpy(dat,&redita_col,sizeof(int));
   dat += sizeof(int);
   strcpy(dat,par);
   dat += (strlen(par)+1);
   if (dato_entra)
   {
      *dat = 1;
      dat++;
      memcpy(dat,&dato_entra_len,sizeof(int));
      dat += sizeof(int);
      strcpy(dat,dato_entra);   
   }
   else
   {
      *dat = 0;
      dat++;
   }
   _dato_entra = dato_entra;;
   _dato_entra_len = dato_entra_len;
   dato_entra_len = 0;
   dato_entra = NULL;
   
   if (!ojo)
	 server_espera_control--;
   envia_control_a_server(tmp,(int)(dat-tmp)+1,15);
   if (!ojo)
	 server_espera_control++;
   leecontrol_en_server(tmp,300,4);
   
   dato_entra = _dato_entra;;
   dato_entra_len = _dato_entra_len;
   
   dat = tmp;
   memcpy(&redita_col,dat,sizeof(int));
   dat += sizeof(int);
   if (dato_entra)
   {
      /*  strcpy(dato_entra,dat); */ /* atencion!!!*/
   }
   
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
   pulsa_en_espacio = _pulsa_en_espacio;
   
#ifdef FGVWIN
   tabla_icon = _tabla_icon;
   punto_tabla = _punto_tabla;
   ntabla_icon = _ntabla_icon;
   wwmenus = _wwmenus; /* opciones */
   nwwmenus = _nwwmenus;  /* numero de opciones para liberacion */
   en_menu = _en_menu;
   n_wmenus = _n_wmenus;
   principal_menu = _principal_menu;
   if (principal_menu)
   {
      SetMenu(principal_w, principal_menu);		
      RedrawWindow(principal_w,NULL,NULL,RDW_FRAME | RDW_INVALIDATE);
   }
   if (DespachadorExterno)
   {
      (*DespachadorExterno)((int)DESPACHO_EMENU,(int)en_menu,(void *)NULL);
   }

#endif
   
   return(0);
}

void dispara_servidor(int d)
{   
   envia_control_a_server(&d,sizeof(int),42);
   flush_mysock();
   leecontrol_en_server(NULL,0,42);
   modo_terminal();
}

int modo_terminal()
{
   unsigned long cuanto;
   int err,i;
   int tolerance;
   int recupera;
   DATAMSG *xfer = &xrcbbuf;
   int ojo;
   
   tolerance = 0;
   recupera = 0;
   while(-1)
   {
      flush_mysock();
      ojo = 0;
      if (!rwp_rcv_buffer)
      {
         if (recupera)
         {
            sleep(2); /* prudencia !!! */ 
            xferbuf.hdr.signature = RWNIDENTIFICACION;                 
            xferbuf.hdr.command = SOCORRO;
            xferbuf.hdr.length = HDRSIZE;
            senddatamessage(rwsock,&xferbuf);
            flush_mysock();
            recupera = 0;
         }
         
         do {
            
            
            FD_ZERO(&setread);
            FD_SET(MySock.sock,&setread);
            FD_ZERO(&seterror);
            FD_SET(MySock.sock,&seterror);
            timeout.tv_sec = 30L;
            timeout.tv_usec = 0L;
#ifndef WIN32
			   errno = 0;
#endif
            if ((err = select(MySock.sock+1,&setread,NULL,&seterror,&timeout)) == SOCKET_ERROR || (err && FD_ISSET(MySock.sock,&seterror)))
            {
#ifndef WIN32
			 if (err == SOCKET_ERROR && errno == EINTR)
			 {
				 continue;
			 }
#endif

               if (tolerance < 5)
               {
                  tolerance++;
                  ojo = 1;
                  sleep(1);
               }
               else
               {
                  if (err && FD_ISSET(MySock.sock,&seterror))
                  {
                     aviso(0,__Texto[515]);
                  }
                  else
                  {
                     aviso(0,__Texto[143]);
                  }
                  establece_desconexion(0);
				  /*
                  sleep(60);		  
                  if (!establece_conexion(-1)) 
                  {
                     continue;
                  }
				  */
                  return(-1);
               }
            }
#ifdef FGVWIN
            //agi_cola_win();
#endif
         } while(!err);
         
         if (ojo)
         {
            continue;
         }
      }
      
      if (rwp_rcv_buffer || FD_ISSET(MySock.sock,&setread))
      {
         if (!rwp_rcv_buffer)
         {
            cuanto = 0L;
            ioctlsocket(MySock.sock,FIONREAD,&cuanto);
            if (!cuanto)
            {
               /* desconexion !!! */
               recv(MySock.sock,&cuanto,cuanto,0);
               return(-1);
               /*MessageBox(NULL,"LA CONEXION CON EL SERVIDOR SE HA ROTO!!!!","ATENCION!",MB_OK);*/
            }
            if (cuanto < HDRSIZE)
               continue;
         }
         if (recvdatamessage(MySock.sock,xfer) == FALSE || xfer->hdr.command != COMANDO_TERMINAL)
         {
            if (xfer->hdr.command == CONTROL_DE_SERVIDOR || xfer->hdr.command == FILE_TRANSFER_CLOSE || xfer->hdr.command == FILE_TRANSFER_ACK || xfer->hdr.command == FILE_TRANSFER_NACK || xfer->hdr.command == FILE_TRANSFER_DATA)
            {
               /* aviso(0,"CONTROL_DE_SERVIDOR"); */
               /* puede ser por recuperacion de conexion !!! ignorar */
               continue;
            }
            recupera = 1;
            continue;
         }
         tolerance = 0;
         if ( (i = procesa_terminal(xfer))  >= 2)
         {
			 /*
			if (i != 4)
			{
				char tmp[256];
				sprintf(tmp,"%d",i);
				//aviso(0,tmp);
			}
			*/
            return(i);
         } 
      }
      
   } /* while */
   
   
}



static BOOL rwi_recvdata(SOCKET sock,char  *pr,int totalbytes)
{
   int totalbytesread = 0,bytesread,readsize = totalbytes;   
   while(totalbytesread < totalbytes)
   {
#ifndef WIN32
			   errno = 0;
#endif
      if((bytesread = recv(sock,(char *)pr + totalbytesread,readsize,0)) == SOCKET_ERROR)
      {
#ifdef FGVDOS
         if (WSAGetLastError() != WSAEWOULDBLOCK)
         {													  
		    return FALSE;
         }
		 Sleep(10);
#else
		 if (errno == EINTR)
			 continue;
         if (errno != EWOULDBLOCK)
         {
            return FALSE;
         }
		 usleep(10000);
#endif		 
         continue;
      }
      totalbytesread += bytesread;
      readsize -= bytesread;
   }        
   if (haylog_mensajes_cs)
   {
		 log_mensajes_cs(4,(void *)pr,totalbytesread);
   }
   return(TRUE);
} 

int peticion_a_transfer(destino,comando,len,tmp)
char *destino;
char comando;
int len;
char *tmp;
{
   SOCKET osock;
   struct sockaddr_in onetcon;
   struct hostent *mihost;
   int e = 0,i;
   unsigned short PORTRWT = 1112;
   char itmp[300];
#ifdef FGVDOS
#ifndef WATCOMNLM
   WORD      wVersionRequested;
   WSADATA   wsaData;
#endif
#endif
#ifdef FGVDOS
#ifdef RWSERVER
   if (!en_conexion_externa && !en_conexion_terminal)
#else
      if (!en_conexion_terminal)
#endif
      {
         wVersionRequested = MAKEWORD(1,1);
         if (WSAStartup(wVersionRequested, &wsaData) != 0)
         {	 
            return -1;
         }
      }
#endif
      
      
      if((osock =  socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) == SOCKET_ERROR)
      {   
         e = -1;
      }
      else
      {
         i = 1;
         setsockopt(osock,IPPROTO_TCP,TCP_NODELAY,&i,sizeof(i));
         

#ifdef NTFACTOR
         i = 600 * NTFACTOR;
         setsockopt(osock,SOL_SOCKET,SO_SNDTIMEO,&i,sizeof(i));
         i = 600 * NTFACTOR;
         setsockopt(osock,SOL_SOCKET,SO_RCVTIMEO,&i,sizeof(i));		 
#else
		 tmval.tv_sec = 600;
		 tmval.tv_usec = 0;
		 setsockopt(osock,SOL_SOCKET,SO_SNDTIMEO,&tmval,sizeof(tmval));
		 setsockopt(osock,SOL_SOCKET,SO_RCVTIMEO,&tmval,sizeof(tmval));
#endif

		 i = -1;
		 setsockopt(osock,SOL_SOCKET,SO_KEEPALIVE,&i,sizeof(i));         
         
         onetcon.sin_addr.s_addr = inet_addr(destino); 
         if (onetcon.sin_addr.s_addr == INADDR_NONE)
         {
            if ( (mihost = gethostbyname(destino)) && mihost->h_length == 4 && mihost->h_addr_list[0])
            {	    
               onetcon.sin_addr.s_addr = *((unsigned long *)mihost->h_addr_list[0]);
            }
         }
         onetcon.sin_family = AF_INET;
         onetcon.sin_port = PORTRWT;
		 moto_short(&(onetcon.sin_port));
         e = -1;
         if (ds_connect_(osock,(struct sockaddr *) &onetcon,sizeof (onetcon)) != SOCKET_ERROR)
         {	 
            itmp[0] = '0';
            itmp[1] = '7';
            itmp[2] = comando;
            if (len > 294)
               len = 294;
            memcpy(itmp+3,tmp,len);	 
            itmp[297] = 0;
            itmp[298] = '9';
            itmp[299] = '5';
            rwi_senddata(osock,itmp,300);
            
            /* select */
            
            if((rwi_recvdata(osock,itmp,2+sizeof(int))) != FALSE)
            {					
               if (itmp[0] == '0' && itmp[1] == '7')
                  memcpy(&e,itmp+2,sizeof(int));
               else
               {
                  aviso(0,__Texto[144]); 
               }
            }
            else
            {
               aviso(0,__Texto[145]);
            }
         }
         closesocket(osock);
      }
      
      
#ifdef FGVDOS
#ifdef RWSERVER
      if (!en_conexion_externa && !en_conexion_terminal)
#else
         if (!en_conexion_terminal)
#endif
         {
            WSACleanup();  
         }
#endif
         return(e);
}


#define IDMENSAJE 0x23169134

typedef struct MAIL_MENSAJE_
{
    unsigned long id;
    char from[250];
    char to[250];
    char subject[250];
    char message[250];
    char files[252];
} MailMensaje;


int dscorreo_envia(char *destino,char *_servicio,char *from,char *to,char *subject,char *message,char *files,char *version)
{
   SOCKET osock;
   struct sockaddr_in onetcon;
   struct hostent *mihost;
   int e = 0,i,j;
   unsigned short PORTRWT = 1122;
   MailMensaje Msg;
   int masmessage = 0;
   int file;
   char buf1[512];
   char servicio[260];
   char *buf2;
#ifdef FGVDOS
#ifndef WATCOMNLM
   WORD      wVersionRequested;
   WSADATA   wsaData;
#endif
#endif
   strcpy(servicio,_servicio);
   strcpy(version,"[0000000]");
#ifdef FGVDOS
#ifdef RWSERVER
   if (!en_conexion_externa && !en_conexion_terminal)
#else
      if (!en_conexion_terminal)
#endif
      {
         wVersionRequested = MAKEWORD(1,1);
         if (WSAStartup(wVersionRequested, &wsaData) != 0)
         {	 
            return -1;
         }
      }
#endif      
      
      if((osock =  socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) == SOCKET_ERROR)
      {   
         e = -1;
      }
      else
      {
         i = 1;
         setsockopt(osock,IPPROTO_TCP,TCP_NODELAY,&i,sizeof(i));


#ifdef NTFACTOR
         i = 600 * NTFACTOR;
         setsockopt(osock,SOL_SOCKET,SO_SNDTIMEO,&i,sizeof(i));
         i = 600 * NTFACTOR;
         setsockopt(osock,SOL_SOCKET,SO_RCVTIMEO,&i,sizeof(i));		 
#else
		 tmval.tv_sec = 600;
		 tmval.tv_usec = 0;
		 setsockopt(osock,SOL_SOCKET,SO_SNDTIMEO,&tmval,sizeof(tmval));
		 setsockopt(osock,SOL_SOCKET,SO_RCVTIMEO,&tmval,sizeof(tmval));
#endif

		 i = -1;
		 setsockopt(osock,SOL_SOCKET,SO_KEEPALIVE,&i,sizeof(i));
         
         
         onetcon.sin_addr.s_addr = inet_addr(destino); 
         if (onetcon.sin_addr.s_addr == INADDR_NONE)
         {
            if ( (mihost = gethostbyname(destino)) && mihost->h_length == 4 && mihost->h_addr_list[0])
            {	    
               onetcon.sin_addr.s_addr = *((unsigned long *)mihost->h_addr_list[0]);
            }
         }
         onetcon.sin_family = AF_INET;
         onetcon.sin_port = PORTRWT;
		 moto_short( &(onetcon.sin_port) );
         e = -1;
         if (ds_connect_(osock,(struct sockaddr *) &onetcon,sizeof (onetcon)) != SOCKET_ERROR)
         {	 

            /*dscorreo_envia(char *destino,char *from,char *to,char *subject,char *message,char *files);*/
            e = strlen(servicio) + 3;
			servicio[e-2] = 'V';
			servicio[e-1] = 'V';

			moto_int( &e );
            rwi_senddata(osock,&e,sizeof(e));
			moto_int( &e );
            rwi_senddata(osock,servicio,e);

            if (!strcmp(servicio,"LLAMA") || !strcmp(servicio,"CUELGA"))
            {
                rwi_recvdata(osock,&e,sizeof(e));
                rwi_senddata(osock,&e,sizeof(e));
				moto_int( &e );
                closesocket(osock);
                goto salir;
            }

            e = 8080;
			moto_int( &e );
            rwi_senddata(osock,&e,sizeof(e));
			moto_int( &e );

            Msg.id = IDMENSAJE;
            strcpy(Msg.from,from);
            strcpy(Msg.to,to);
            strcpy(Msg.subject,subject);
            if (strlen(message) > 249)
            {
                masmessage = 1;
                memcpy(Msg.message,message,240);
                memcpy(Msg.message+240,"...",3);
                Msg.message[244] = 0;
                Msg.message[247] = 1;
                Msg.message[248] = 2;
                Msg.message[249] = 1;
            }
            else
                strcpy(Msg.message,message);
            strcpy(Msg.files,files);
            if (rwi_senddata(osock,&Msg,sizeof(Msg)) != FALSE)
            {
                do
                {
                    e = 0;
                    if (rwi_recvdata(osock,&e,sizeof(e)) == FALSE) break;
					moto_int( &e );
                    if (e == 7997)
                    {
						if (rwi_recvdata(osock,&e,sizeof(e)) == FALSE) break;
						sprintf(version,"[%07d]",e);
						continue;
					}
                    if (e == 7557)
                    {
                        e = strlen(message);
						moto_int( &e );
                        if (rwi_senddata(osock,&e,sizeof(e)) == FALSE) break;
						moto_int( &e );
                        if (rwi_senddata(osock,message,e) == FALSE) break;
                        continue;
                    }
                    if (e == 1010)
                    {
                        e = 2020;
						moto_int( &e );
                        if (rwi_senddata(osock,&e,sizeof(e)) == FALSE) break;
                        e = 0;						
                        if (rwi_recvdata(osock,&e,sizeof(e)) == FALSE) break;
						moto_int( &e );
                        if (rwi_recvdata(osock,buf1,e) == FALSE) break;
                        e = 0;
                        file = rw_base_open_(buf1,O_RDONLY | O_BINARY);
                        if (file > -1)
                        {
                            e = rw_base_lseek_(file,0L,SEEK_END);
                        }
						moto_int( &e );
                        if (rwi_senddata(osock,&e,sizeof(e)) == FALSE) break;
						moto_int( &e );
                        if (file > -1)
                            rw_base_lseek_(file,0L,SEEK_SET);
                        i = 0;
                        buf2 = malloc(100000);
                        while(i < e)
                        {
                            j = e - i;
                            if (j > 100000) j = 100000;
                            if (file > -1)
                                rw_base_read_(file,buf2,j);
                            if (rwi_senddata(osock,buf2,j) == FALSE) break;
                            i += j;
                        }
                        if (buf2) free(buf2);
                        if (file > -1)
                            rw_base_close_(file);
                    }
                    else
                        break;
                } while(-1);                
            }
         }

		 moto_int( &e );
         rwi_senddata(osock,&e,sizeof(e));
		 moto_int( &e );
         closesocket(osock);
      }      
salir:      
#ifdef FGVDOS
#ifdef RWSERVER
      if (!en_conexion_externa && !en_conexion_terminal)
#else
         if (!en_conexion_terminal)
#endif
         {
            WSACleanup();  
         }
#endif
         return(e);
}


static char *recibe_fichero_correo(SOCKET tsock,char *p,char *r)
{
    int i,j;
    char *buf = NULL;    
    int file;
    char ff[256];    

    
        if (rwi_recvdata(tsock,&i,sizeof(i)) == FALSE) return NULL;
		moto_int( &i );
        if (i < 1)
        {
            return NULL;
        }
        strcpy(ff,p);
        if (ff[0] && ff[strlen(ff)-1] != '/')
        {
            strcat(ff,"/");
        }
        if (rwi_recvdata(tsock,ff+strlen(ff),i) == FALSE) return NULL;


        if (rwi_recvdata(tsock,&i,sizeof(i)) == FALSE) return NULL;
		moto_int( &i );
        if (i < 0)
        {
            return NULL;
        }

        /* en bloques de 100K */
        buf = malloc(100000);
        file = -1;
       
        file = rw_base_open_(ff,O_CREAT | O_BINARY | O_RDWR | O_TRUNC,S_IREAD | S_IWRITE);

        while(i > 0)
        {
            if (i > 100000)
                j = 100000;
            else
                j = i;
            if (rwi_recvdata(tsock,buf,j) == FALSE) break;
            if (file > -1) rw_base_write_(file,buf,j);
            i -= j;
        }
        if (file > -1)
            rw_base_close_(file);
        if (buf)
            free(buf);
    
    if (file < 0)
    {
        i = -1;
		moto_int( &i );
        rwi_senddata(tsock,&i,sizeof(i));
		moto_int( &i );
        return NULL;
    }
    else
    {
       i = 0;
	   moto_int( &i );
       rwi_senddata(tsock,&i,sizeof(i));
	   moto_int( &i );
       if (*r)
       {
		   if ((strlen(r) + quita_path(ff) + 4) > 250)
		   {
			   i = strlen(r);
			   i--;
			   if (r[i] != ';')
			   {
				   strcat(r,";;");
			   }
		   }
		   else
		   {
			   strcat(r,";");
			   strcat(r,quita_path(ff));
		   }
       }
	   else
		   strcat(r,ff);
       return r;
    }
}


int dscorreo_recibe(char *servidor,char *_servicio,char *usuario,char *password,char *path,char *bufr,char *sujeto,char *version)
{
   SOCKET osock;
   struct sockaddr_in onetcon;
   struct hostent *mihost;
   int e = 0,i,j;
   unsigned short PORTRWT = 1122;
   MailMensaje Msg;
   int file;
   char buf1[512];
   char servicio[260];
   char *buf2;
#ifdef FGVDOS
#ifndef WATCOMNLM
   WORD      wVersionRequested;
   WSADATA   wsaData;
#endif
#endif
   strcpy(servicio,_servicio);
   strcpy(version,"[0000000]");
#ifdef FGVDOS
#ifdef RWSERVER
   if (!en_conexion_externa && !en_conexion_terminal)
#else
      if (!en_conexion_terminal)
#endif
      {
         wVersionRequested = MAKEWORD(1,1);
         if (WSAStartup(wVersionRequested, &wsaData) != 0)
         {	 
            return -1;
         }
      }
#endif
      
      
      if((osock =  socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) == SOCKET_ERROR)
      {   
         e = -1;
      }
      else
      {
         i = 1;
         setsockopt(osock,IPPROTO_TCP,TCP_NODELAY,&i,sizeof(i));                     
         
         
         onetcon.sin_addr.s_addr = inet_addr(servidor);
         if (onetcon.sin_addr.s_addr == INADDR_NONE)
         {
            if ( (mihost = gethostbyname(servidor)) && mihost->h_length == 4 && mihost->h_addr_list[0])
            {	    
               onetcon.sin_addr.s_addr = *((unsigned long *)mihost->h_addr_list[0]);
            }
         }
         onetcon.sin_family = AF_INET;
         onetcon.sin_port = PORTRWT;

		 moto_short( &(onetcon.sin_port) );

         e = -1;
         if (ds_connect_(osock,(struct sockaddr *) &onetcon,sizeof (onetcon)) != SOCKET_ERROR)
         {	 

			
#ifdef NTFACTOR
            i = 1200 * NTFACTOR;
            setsockopt(osock,SOL_SOCKET,SO_SNDTIMEO,&i,sizeof(i));
            i = 1200 * NTFACTOR;
            setsockopt(osock,SOL_SOCKET,SO_RCVTIMEO,&i,sizeof(i));            
#else
		    tmval.tv_sec = 1200;
		    tmval.tv_usec = 0;
			setsockopt(MySock.sock,SOL_SOCKET,SO_SNDTIMEO,&tmval,sizeof(tmval));
			setsockopt(MySock.sock,SOL_SOCKET,SO_RCVTIMEO,&tmval,sizeof(tmval));
#endif
			 
			i = -1;
			setsockopt(osock,SOL_SOCKET,SO_KEEPALIVE,&i,sizeof(i));


            /*dscorreo_envia(char *destino,char *from,char *to,char *subject,char *message,char *files);*/
            e = strlen(servicio) + 3;
			servicio[e-2] = 'V';
			servicio[e-1] = 'V';

			moto_int( &e );
            rwi_senddata(osock,&e,sizeof(e));
			moto_int( &e );
            rwi_senddata(osock,servicio,e);

            if (!strcmp(servicio,"LLAMA") || !strcmp(servicio,"CUELGA"))
            {
                rwi_recvdata(osock,&e,sizeof(e));
                rwi_senddata(osock,&e,sizeof(e));
                closesocket(osock);
                goto salir;
            }
			

            e = 9090;
			moto_int( &e );
            rwi_senddata(osock,&e,sizeof(e));			

            e = strlen(usuario) + 1;
			moto_int( &e );
            rwi_senddata(osock,&e,sizeof(e));
			moto_int( &e );
            rwi_senddata(osock,usuario,e);

            e = strlen(password) + 1;
			moto_int( &e );
            rwi_senddata(osock,&e,sizeof(e));
			moto_int( &e );
            rwi_senddata(osock,password,e);

            bufr[0] = 0;
            cambia_barra(path);
            do
            {
              e = 0;
              if (rwi_recvdata(osock,&e,sizeof(e)) == FALSE) 
              {
                  e = -1;
                  break;
              }
			  moto_int( &e );
              if (e == 7997)
              {
				 if (rwi_recvdata(osock,&e,sizeof(e)) == FALSE) break;
				 sprintf(version,"[%07d]",e);			  	 
			  }
              else if (e == 1010)
              {                  
                  if (!recibe_fichero_correo(osock,path,bufr))
                  {
                      e = -1;
                      break;
                  }
              }
              else if (e == 1331)
			  {
				  if (rwi_recvdata(osock,&e,sizeof(e)) == FALSE) 
				  {
                      e = -1;
                      break;
				  }
				  if (rwi_recvdata(osock,sujeto,e) == FALSE) 
				  {
                      e = -1;
                      break;
				  }
				  e = 0;
			  }
			  else
                  break;
            } while(-1);

			moto_int( &e );
            rwi_senddata(osock,&e,sizeof(e));
			moto_int( &e );
            closesocket(osock);
         }      
      }

salir:
#ifdef FGVDOS
#ifdef RWSERVER
      if (!en_conexion_externa && !en_conexion_terminal)
#else
         if (!en_conexion_terminal)
#endif
         {
            WSACleanup();  
         }
#endif
         return(e);
}



SOCKET respaldo_remoto_sock = SOCKET_ERROR;

int activa_respaldo_remoto(char *destino)
{   
   struct sockaddr_in onetcon;
   struct hostent *mihost;
   int i;
#ifdef FGVDOS
#ifndef WATCOMNLM
   WORD      wVersionRequested;
   WSADATA   wsaData;
#endif
#endif
#ifdef FGVDOS
#ifdef RWSERVER
   if (!en_conexion_externa && !en_conexion_terminal)
#else
      if (!en_conexion_terminal)
#endif
      {
         wVersionRequested = MAKEWORD(1,1);
         if (WSAStartup(wVersionRequested, &wsaData) != 0)
         {	 
            return -1;
         }
      }
#endif

      if (respaldo_remoto_sock != SOCKET_ERROR)
      {
          closesocket(respaldo_remoto_sock);
          respaldo_remoto_sock = SOCKET_ERROR;
      }
      
      
      if((respaldo_remoto_sock =  socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) != SOCKET_ERROR)
      {
         i = 1;
         setsockopt(respaldo_remoto_sock,IPPROTO_TCP,TCP_NODELAY,&i,sizeof(i));


#ifdef NTFACTOR
         i = 600 * NTFACTOR;
         setsockopt(respaldo_remoto_sock,SOL_SOCKET,SO_SNDTIMEO,&i,sizeof(i));
         i = 600 * NTFACTOR;
         setsockopt(respaldo_remoto_sock,SOL_SOCKET,SO_RCVTIMEO,&i,sizeof(i));
#else
		 tmval.tv_sec = 600;
		 tmval.tv_usec = 0;
		 setsockopt(respaldo_remoto_sock,SOL_SOCKET,SO_SNDTIMEO,&tmval,sizeof(tmval));
		 setsockopt(respaldo_remoto_sock,SOL_SOCKET,SO_RCVTIMEO,&tmval,sizeof(tmval));
#endif		 

		  i = -1;
		  setsockopt(respaldo_remoto_sock,SOL_SOCKET,SO_KEEPALIVE,&i,sizeof(i));

         
         
         onetcon.sin_addr.s_addr = inet_addr(destino); 
         if (onetcon.sin_addr.s_addr == INADDR_NONE)
         {
            if ( (mihost = gethostbyname(destino)) && mihost->h_length == 4 && mihost->h_addr_list[0])
            {	    
               onetcon.sin_addr.s_addr = *((unsigned long *)mihost->h_addr_list[0]);
            }
         }
         onetcon.sin_family = AF_INET;
         onetcon.sin_port = 1133/*PORTRWT*/;
		 moto_short(&(onetcon.sin_port));
         if (ds_connect_(respaldo_remoto_sock,(struct sockaddr *) &onetcon,sizeof (onetcon)) != SOCKET_ERROR)
         {
             return 0;
         }         
      }      

      if (respaldo_remoto_sock != SOCKET_ERROR)
      {
          closesocket(respaldo_remoto_sock);
          respaldo_remoto_sock = SOCKET_ERROR;
      }

#ifdef FGVDOS
#ifdef RWSERVER
      if (!en_conexion_externa && !en_conexion_terminal)
#else
         if (!en_conexion_terminal)
#endif
         {
            WSACleanup();  
         }
#endif
         return(-1);
}

static FILE *flog = NULL;

#ifdef CONSTDARG
int ultra_log(char *format, ...)
#else
int ultra_log(format,va_alist)
char *format;
va_dcl
#endif
{

     va_list ap;
#ifdef CONSTDARG
 	va_start(ap,format);
#else
     va_start(ap);
#endif
    

	if (!flog)
	{
		flog = fopen("dsrespaldo.log","a");
	}

	fprintf(flog,format,ap);

	va_end(ap);

	return 0;
}

int envia_respaldo_remoto(char *buffer)
{   
    if (respaldo_remoto_sock != SOCKET_ERROR)
    {
        int id = 0x11111111;
        int memoria = *((int *)buffer);

		ultra_log("Envia %d %d bytes\r\n",(int)id,memoria);
		
        if (!rwi_senddata(respaldo_remoto_sock,&id,sizeof(int)))
        {
            return -1;
        }
        if (!rwi_senddata(respaldo_remoto_sock,&memoria,sizeof(int)))
        {
            return -1;
        }
        if (rwi_senddata(respaldo_remoto_sock,buffer,memoria))
        {
            return 0;
        }
    }
    return -1;
}

int desactiva_respaldo_remoto()
{
  if (respaldo_remoto_sock != SOCKET_ERROR)
  {
      closesocket(respaldo_remoto_sock);
      respaldo_remoto_sock = SOCKET_ERROR;
  }

#ifdef FGVDOS
#ifdef RWSERVER
  if (!en_conexion_externa && !en_conexion_terminal)
#else
     if (!en_conexion_terminal)
#endif
     {
        WSACleanup();  
     }
#endif

  return 0;
}

int esta_activo_el_respaldo_remoto()
{
  if (respaldo_remoto_sock != SOCKET_ERROR)
  {
	  return 1;
  }
  return 0;
}



static int RecibePaquete(SOCKET sock, Paquete *recvbuf,int tout)
{	
	
	fd_set setread;
	fd_set seterror;
	struct timeval timeout;
	struct timeval *ptm = NULL;	
	unsigned long cuanto;
	int readsize, totalbytesread, msgsize, bytesread,err;
	int maximo = 0;

	
	if (tout)
	{
		timeout.tv_sec  = tout/1000;
		timeout.tv_usec = (tout%1000)*1000;
		ptm = &timeout;
	}	
	do
	{
		FD_ZERO(&setread);
		FD_SET(sock,&setread);
		FD_ZERO(&seterror);
		FD_SET(sock,&seterror);
		maximo = sock+1;
#ifndef WIN32
			   errno = 0;
#endif
		if ((err = select(maximo,&setread,NULL,&seterror,ptm)) == SOCKET_ERROR)
		{
#ifndef WIN32
			 if (err == SOCKET_ERROR && errno == EINTR)
			 {
				 continue;
			 }
#endif

			return(SOCKET_ERROR);
		}
		if (!err)
		{
			return(0); // timeout 
		}

	
		cuanto = 0L;
		if (ioctlsocket(sock,FIONREAD,&cuanto) == SOCKET_ERROR)
		{
			return SOCKET_ERROR;
		}
		if (!cuanto)
		{
			return(0);
		}
		
		if (cuanto >= LEN_CABECERA)
		{
			break;
		}
	} while(-1);	
	
	if (cuanto < LEN_CABECERA)
		return 0;

	readsize = LEN_CABECERA;	
	do
	{
#ifndef WIN32
			   errno = 0;
#endif
	if((totalbytesread = recv(sock, (char *)&(recvbuf->c), readsize,0/*MSG_WAITALL*/)) == SOCKET_ERROR)
	{		
#ifndef WIN32
			 if (errno == EINTR)
			 {
				 continue;
			 }
#endif

		return SOCKET_ERROR;
	}
	break;
	} while(-1);

	if(recvbuf->c.id != DSWEBCALLID)
	{		
		if (recvbuf->c.id == 0x7117)
		{
			/* debug, no deberia recibir esto */
			readsize = recvbuf->c.command; 
			bytesread = recv(sock,((char *)recvbuf->buffer),readsize,0);
			return 0;
		}
		return SOCKET_ERROR;		
	}		
	
	

	totalbytesread = 0;
	msgsize = LEN_DATA(recvbuf);
	readsize = msgsize;
	
	while(totalbytesread < msgsize)
	{		
#ifndef WIN32
			   errno = 0;
#endif
		if((bytesread = recv(sock,
			((char *)(recvbuf->buffer)) + totalbytesread,
			readsize,
			0)) == SOCKET_ERROR)
		{
			int i;
#ifdef FGVDOS
		  if ((i = WSAGetLastError()) != WSAEWOULDBLOCK && i != WSAEINPROGRESS && i != WSAENETRESET && i != WSAEINTR && i != WSAENOBUFS)
		  {
			  
			  return SOCKET_ERROR;
		  }										  
		  Sleep(10);
#else
		  if (errno == EINTR)
			  continue;
		  if (errno != EWOULDBLOCK)
		  {
			  return SOCKET_ERROR;
		  }
		  usleep(10000);
#endif
		  
		  continue;
		}
		totalbytesread += bytesread;
		readsize -= bytesread;		
	}
	
	return 1;
}


#define C_HOLA    0x1000 // Inicio conexion al nodo
#define C_ADIOS   0x2000 // Fin conexion al nodo
#define C_CHAT    0x3000 // Mensaje de chat
#define C_DATA    0x4000 // Intercambio de datos entre cliente/operador
#define C_ESTADO  0x5000 // Cambio de estado del operador o cliente (ocupado etc)
#define C_PASAR   0x6000 // Pasar a otro operador
#define C_INFO    0x7000 // Pedir informacion al nodo
#define C_ACABADO 0x8000 // Acabado con el cliente/operador
#define C_RES9    0x9000
#define C_RESA    0xA000
#define C_RESB    0xB000
#define C_RESC    0xC000
#define C_RESD    0xD000
#define C_MULTI   0xE000
#define C_RESF    0xF000
/* Conecta, envia el paquete, espera confirmacion del envio , deconecta y vuelve */
int dstray_conexion(char *destino,Paquete *paquete)
{
   SOCKET osock = SOCKET_ERROR;
   struct sockaddr_in onetcon;
   struct hostent *mihost;
   int e = -1,i,err;
   unsigned short PORTDST = 2078;   
   Paquete paqueter;
#ifdef FGVDOS
#ifndef WATCOMNLM
   WORD      wVersionRequested;
   WSADATA   wsaData;
#endif
#endif
#ifdef FGVDOS
#ifdef RWSERVER
   if (!en_conexion_externa && !en_conexion_terminal)
#else
      if (!en_conexion_terminal)
#endif
      {
         wVersionRequested = MAKEWORD(1,1);
         if (WSAStartup(wVersionRequested, &wsaData) != 0)
         {	 
            return -1;
         }
      }
#endif      
      
      if((osock =  socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) == SOCKET_ERROR)
      {   
         e = -1;
      }
      else
      {
         i = 1;
         setsockopt(osock,IPPROTO_TCP,TCP_NODELAY,&i,sizeof(i));
		 i = -1;
		 setsockopt(osock,SOL_SOCKET,SO_KEEPALIVE,&i,sizeof(i));
         
         onetcon.sin_addr.s_addr = inet_addr(destino); 
         if (onetcon.sin_addr.s_addr == INADDR_NONE)
         {
            if ( (mihost = gethostbyname(destino)) && mihost->h_length == 4 && mihost->h_addr_list[0])
            {	    
               onetcon.sin_addr.s_addr = *((unsigned long *)mihost->h_addr_list[0]);
            }
         }
         onetcon.sin_family = AF_INET;
         onetcon.sin_port = htons(PORTDST);		 
         e = -1;
         if (ds_connect_(osock,(struct sockaddr *) &onetcon,sizeof (onetcon)) != SOCKET_ERROR)
         {
			 if (RecibePaquete(osock,&paqueter,30000) && COMANDO(&paqueter) == C_HOLA)
			 {
				 if (rwi_senddata(osock,(char *)paquete,LEN_PAQUETE(paquete)))
				 {
					 if (RecibePaquete(osock,&paqueter,60000) && COMANDO(&paqueter) == COMANDO(paquete))
					 {
						 e = 0;
					 }
				 }
			 }
		 }
	  }
salir:
	  if (osock != SOCKET_ERROR)
	  {
		  closesocket(osock);
	  }
#ifdef FGVDOS
#ifdef RWSERVER
      if (!en_conexion_externa && !en_conexion_terminal)
#else
         if (!en_conexion_terminal)
#endif
         {
            WSACleanup();  
         }
#endif
	  return e;
}

#define MAXLOCALSOCKETS 256
static SOCKET local_sockets[MAXLOCALSOCKETS];
static char  *local_clave[MAXLOCALSOCKETS];
static int nlocal_sockets = 0;

static void x_c_code(long* vo, long* v, long* k)  
{   unsigned long y = vo[0],
                  z = vo[1], 
                  sum = 0,                           /* set up */
                  delta = 0x9e3779b9, 
                  n = 32;           /* a key schedule constant */

    while(n-- > 0)                        /* basic cycle start */
    {   sum += delta;
        y += (z << 4) + k[0] ^ z + sum ^ (z >> 5) + k[1];
        z += (y << 4) + k[2] ^ y + sum ^ (y >> 5) + k[3];
    }                                             /* end cycle */

    v[0] = y;
    v[1] = z;
}
static void x_c_decode( long* v, long* k )  
{   unsigned long n = 32, 
                  sum, 
                  y = v[0], 
                  z=v[1],
                  delta = 0x9e3779b9;

    sum = delta << 5;
                                                /* start cycle */
    while(n-- > 0) 
    {   z -= (y << 4) + k[2] ^ y + sum ^ (y >> 5) + k[3]; 
        y -= (z << 4) + k[0] ^ z + sum ^ (z >> 5) + k[1];
        sum -= delta;
    }
                                                  /* end cycle */
    v[0] = y;
    v[1] = z;
}



int dstcp_asigna_socket(char *_type,char *_protocol)
{
	SOCKET sock;
	int type,protocol,i;

	if (!strcmp(_type,"SOCK_STREAM"))
	{
		type = SOCK_STREAM;
	}
	else if (!strcmp(_type,"SOCK_DGRAM"))
	{
		type = SOCK_DGRAM;
	}
	else
		return -11;

	if (!strcmp(_protocol,"IPPROTO_TCP"))
	{
		protocol = IPPROTO_TCP;
	}
	else if (!strcmp(_protocol,"IPPROTO_IP"))
	{
		protocol = IPPROTO_IP;
	}
	else
		return -12;


	for (i = 0;i < nlocal_sockets;i++)
	{
		if (local_sockets[i] == INVALID_HANDLE_VALUE)
		{
			break;
		}
	}
	if (i >= MAXLOCALSOCKETS)
	{
		return -2;
	}

	if ((sock = socket(AF_INET,type,protocol)) == SOCKET_ERROR)
	{
		return -1;
	}
	if (i == nlocal_sockets)
		nlocal_sockets++;

	local_sockets[i] = sock;
	local_clave[i] = NULL;

	FastSocket(sock);

	return i;
}

int dstcp_libera_socket(int i)
{
	if (i < 0 || i >= nlocal_sockets || local_sockets[i] == INVALID_HANDLE_VALUE)
		return -1;

	closesocket(local_sockets[i]);
	local_sockets[i] = INVALID_HANDLE_VALUE;
	if (local_clave[i])
	{
		free(local_clave[i]);
		local_clave[i] = NULL;
	}
	if (i == (nlocal_sockets-1))
	{
		nlocal_sockets--;
	}
	return i;
}

int dstcp_connect_socket(int i,char *direccion,int port)
{
	struct sockaddr_in addr;
	struct hostent *mihost;
	if (i < 0 || i >= nlocal_sockets || local_sockets[i] == INVALID_HANDLE_VALUE)
		return -1;

    addr.sin_family = AF_INET;
    addr.sin_port   = htons((unsigned short)port);
	addr.sin_addr.s_addr = inet_addr(direccion);
	if (addr.sin_addr.s_addr == INADDR_NONE)
	{
        if ( (mihost = gethostbyname(direccion)) && mihost->h_length == 4 && mihost->h_addr_list[0])
        {	    
            addr.sin_addr.s_addr = *((unsigned long *)mihost->h_addr_list[0]);
        }
	}
    if (ds_connect_(local_sockets[i],(struct sockaddr *) &addr,sizeof (addr)) == SOCKET_ERROR)
    {   
		return -2;
	}
	FastSocket(local_sockets[i]);
	return 0;
}

int dstcp_bind_socket(int i,char *direccion,int port)
{
	struct sockaddr_in addr;
	struct hostent *mihost;
	if (i < 0 || i >= nlocal_sockets || local_sockets[i] == INVALID_HANDLE_VALUE)
		return -1;

    addr.sin_family = AF_INET;
    addr.sin_port   = htons((unsigned short)port);
	addr.sin_addr.s_addr = inet_addr(direccion);
	if (addr.sin_addr.s_addr == INADDR_NONE)
	{
        if ( (mihost = gethostbyname(direccion)) && mihost->h_length == 4 && mihost->h_addr_list[0])
        {	    
            addr.sin_addr.s_addr = *((unsigned long *)mihost->h_addr_list[0]);
        }
	}
    if (bind(local_sockets[i],(struct sockaddr *) &(addr),sizeof (addr)) == SOCKET_ERROR)
    {
		return -2;
	}
	return 0;
}

int dstcp_listen_socket(int i,int cuantos)
{
	if (i < 0 || i >= nlocal_sockets || local_sockets[i] == INVALID_HANDLE_VALUE)
		return -1;
    if (listen(local_sockets[i], cuantos) == SOCKET_ERROR)
    {
		 return -2;
	}
	return 0;
}

int dstcp_select_socket(int timeout,int *n1,int *n2,int *s)
{
	int res[256];
	int nres1,nres2;
	fd_set _setread;
	fd_set _setwrite;
	/*fd_set _seterror;*/
	struct timeval _timeout;
	int i,j,k,err;
	int m = -1;
	FD_ZERO(&_setread);
	FD_ZERO(&_setwrite);
	/*FD_ZERO(&_seterror);*/
	for (j = 0;j < *n1;j++)
	{
		i = s[j];
		if (i < 0 || i >= nlocal_sockets || local_sockets[i] == INVALID_HANDLE_VALUE)
			return -1;		
		FD_SET(local_sockets[i],&_setread);
		/*FD_SET(local_sockets[i],&_seterror);*/
		if (local_sockets[i] > m)
			m = local_sockets[i];
	}
	for (k = 0;k < *n2;k++)
	{
		i = s[j];
		j++;
		if (i < 0 || i >= nlocal_sockets || local_sockets[i] == INVALID_HANDLE_VALUE)
			return -1;		
		FD_SET(local_sockets[i],&_setwrite);
		/*FD_SET(local_sockets[i],&_seterror);*/
		if (local_sockets[i] > m)
			m = local_sockets[i];		
	}
	_timeout.tv_sec  = timeout / 1000;
	_timeout.tv_usec = (timeout % 1000) * 1000;
	do
	{
#ifndef WIN32
			   errno = 0;
#endif
	if ((err = select(m+1,&_setread,&_setwrite,NULL/*&_seterror*/,&_timeout)) == SOCKET_ERROR)
	{
#ifndef WIN32
			 if (err == SOCKET_ERROR && errno == EINTR)
			 {
				 continue;
			 }
#endif

		return(-2);
	}
	break;
	} while(-1);
	if (!err)
	{
		*n1 = 0;
		*n2 = 0;
		return 0;
	}
	nres1 = 0;
	nres2 = 0;
	for (j = 0;j < *n1;j++)
	{
		i = s[j];
		if (FD_ISSET(local_sockets[i],&_setread))
		{
			res[nres1] = i;
			nres1++;
		}
	}
	for (k = 0;k < *n2;k++)
	{
		i = s[j];
		j++;
		if (FD_ISSET(local_sockets[i],&_setwrite))
		{
			res[nres1+nres2] = i;
			nres2++;
		}
	}
	*n1 = nres1;
	*n2 = nres2;
	for (j = 0;j < (nres1+nres2);j++)
	{
		s[j] = res[j];
	}
	return err;
}

int dstcp_accept_socket(int i,int timeout,char **_dest,char **_port)
{
	SOCKET sock;
	fd_set _setread;
	fd_set _seterror;
	struct timeval _timeout;
	struct sockaddr_in addr;
	int len,j,err;
	if (i < 0 || i >= nlocal_sockets || local_sockets[i] == INVALID_HANDLE_VALUE)
		return -1;

	if (nlocal_sockets < MAXLOCALSOCKETS)
	{
		j = nlocal_sockets;
	}
	else
	{
		for (j = 0;j < nlocal_sockets;j++)
		{
			if (local_sockets[j] == INVALID_HANDLE_VALUE)
			{
				break;
			}
		}
	}
	if (j >= MAXLOCALSOCKETS)
	{
		return -3;
	}

	if (timeout > 0)
	{
		FD_ZERO(&_setread);
		FD_SET(local_sockets[i],&_setread);
		FD_ZERO(&_seterror);
		FD_SET(local_sockets[i],&_seterror);
		_timeout.tv_sec  = timeout / 1000;
		_timeout.tv_usec = (timeout % 1000) * 1000;
		do
		{
#ifndef WIN32
			   errno = 0;
#endif
		if ((err = select(local_sockets[i]+1,&_setread,NULL,&_seterror,&_timeout)) == SOCKET_ERROR)
		{
#ifndef WIN32
			 if (err == SOCKET_ERROR && errno == EINTR)
			 {
				 continue;
			 }
#endif

			 return(-2);
		}
		break;
		} while(-1);
		if (!err)  
		{
			return(-4);
		}
      
		if (FD_ISSET(local_sockets[i],&_seterror))
		{
			return(-2);   
		}
	}

	len = sizeof(addr);
    sock = accept(local_sockets[i],&addr,&len);
	if (sock == INVALID_HANDLE_VALUE)
	{
		return -2;
	}
	if (j == nlocal_sockets)
		nlocal_sockets++;
	local_sockets[j] = sock;
	if (*_dest)
	{
		free(*_dest);
	}
	*_dest = malloc(128);
	strcpy(*_dest,inet_ntoa(addr.sin_addr));
	if (*_port)
	{
		free(*_port);
	}
	*_port = malloc(56);
	sprintf(*_port,"%d",(int)ntohs(addr.sin_port));
	FastSocket(sock);


	return j;
}

int dstcp_ioctl_socket(int i,char *opcion,char **_dest)
{
	long cuanto = 0;
	if (i < 0 || i >= nlocal_sockets || local_sockets[i] == INVALID_HANDLE_VALUE)
		return -1;
	if (strcmp(opcion,"FIONREAD"))
	{
		return -3;
	}
	if (ioctlsocket(local_sockets[i],FIONREAD,&cuanto) == SOCKET_ERROR)
	{
		return -2;
	}
	if (*_dest)
	{
		free(*_dest);
	}
	*_dest = malloc(56);
	sprintf(*_dest,"%ld",cuanto);
	return cuanto;
}

int dstcp_crypto_socket(int i,char *clave)
{
	int j,k;
	if (i < 0 || i >= nlocal_sockets || local_sockets[i] == INVALID_HANDLE_VALUE)
		return -1;

	if (local_clave[i])
	{
		free(local_clave[i]);
	}	
	local_clave[i] = malloc(16);
	memset(local_clave[i],0,16);
	k = 0;
	for (j = 0;j < 16;j++)
	{
		if (!clave[k])
			k = 0;
		*(local_clave[i]+j) += clave[k];
		k++;
	}
	return 0;
}

int dstcp_sendstring_socket(int i,char *string)
{
	char *enc = string;
	int l = strlen(string)+1;
	if (i < 0 || i >= nlocal_sockets || local_sockets[i] == INVALID_HANDLE_VALUE)
		return -1;

	if (local_clave[i])
	{		
		int j;
		enc = malloc(l);
		for (j = 0;j < l;j += 8)
		{
			x_c_code((long *)(string+j),(long *)(enc+j),(long *)local_clave[i]);
		}
	}

	if (rwi_senddata(local_sockets[i],enc,l) == FALSE)
	{
		if (enc != string)
			free(enc);
		return -2;
	}
	if (enc != string)
		free(enc);
	return 0;
}

int dstcp_recvstring_socket(int i,int timeout,char **_deststring)
{
	fd_set _setread;
	fd_set _seterror;
	int err;
	long cuanto;
	struct timeval _timeout;
	if (i < 0 || i >= nlocal_sockets || local_sockets[i] == INVALID_HANDLE_VALUE)
		return -1;

    FD_ZERO(&_setread);
    FD_SET(local_sockets[i],&_setread);
    FD_ZERO(&_seterror);
    FD_SET(local_sockets[i],&_seterror);
    _timeout.tv_sec  = timeout / 1000;
    _timeout.tv_usec = (timeout % 1000) * 1000;
	do
	{
#ifndef WIN32
			   errno = 0;
#endif
    if ((err = select(local_sockets[i]+1,&_setread,NULL,&_seterror,&_timeout)) == SOCKET_ERROR)
    {
#ifndef WIN32
			 if (err == SOCKET_ERROR && errno == EINTR)
			 {
				 continue;
			 }
#endif

         return(-2);
    }
	break;
	} while(-1);
    if (!err)  
    {
        return(0);
    }
      
    if (FD_ISSET(local_sockets[i],&_seterror))
	{
        return(-2);   
    }
    cuanto = 0L;
    ioctlsocket(local_sockets[i],FIONREAD,&cuanto);
    if (!cuanto)
    {
         return(-2);
    }
	if (*_deststring)
	{
		free(*_deststring);
	}

	if (cuanto > 249)
	{
		cuanto = 249;
	}
	if (local_clave[i])
	{
		if ((cuanto%8))
		{
			cuanto = (cuanto/8)*8;
		}
		if (cuanto < 8)
			return -3;
	}
	
	*_deststring = malloc(cuanto+1);
	memset(*_deststring,0,cuanto+1);
	if (rwi_recvdata(local_sockets[i],*_deststring,cuanto) == FALSE)
	{
		return -2;
	}
	if (local_clave[i])
	{		
		int j;
		for (j = 0;j < cuanto;j += 8)
		{
			x_c_decode((long *)((*_deststring)+j),(long *)local_clave[i]);
		}
	}

	return cuanto;
}

int dstcp_enviadatapaquete_socket(int i,int codigo,int n,char *dato)
{
	Paquete paquete;	
	int len;
	if (i < 0 || i >= nlocal_sockets || local_sockets[i] == INVALID_HANDLE_VALUE)
		return -1;

	paquete.c.id = DSWEBCALLID;		
	codigo &= 0xffff;
	codigo |= (n << 16);
	memcpy(paquete.buffer,&codigo,sizeof(int));
	memcpy(paquete.buffer+sizeof(int),dato,n);
	if (local_clave[i])
	{
		len = (n/8) * 8;
		if ((n%8))
		{
			len += 8;
		}		
		for (n = 0;n < len;n += 8)
		{
			x_c_code((long *)(paquete.buffer+sizeof(int)+n),(long *)(paquete.buffer+sizeof(int)+n),(long *)local_clave[i]);
		}
	}
	else
		len = n;

	len += sizeof(int);
	paquete.c.command = C_DATA | (len&0xfff);

    if (rwi_senddata(local_sockets[i],(char *)&paquete,LEN_PAQUETE(&paquete)))
	{
		return 0;
	}		
	return -2;
}

int dstcp_recibedatapaquete_socket(int i,int timeout,int codigo,int *_n,char **_dato)
{
	Paquete paquete;
	int r;
	int len;
	if (i < 0 || i >= nlocal_sockets || local_sockets[i] == INVALID_HANDLE_VALUE)
		return -1;
	r = RecibePaquete(local_sockets[i],&paquete,timeout);
	if (r < 0)
		r = -2;
	if (r == 1)
	{
		if (COMANDO(&paquete) != C_DATA)
		{
			r = -3;
		}
		else
		{
			*_n = LEN_DATA(&paquete);
			if (*_dato)
			{
				free(*_dato);
				*_dato = NULL;
			}
			if (*_n > 0)
			{
				if (*_n <= sizeof(int))
				{
					*_n = 0;
				}
				else
				{
					*_n -= sizeof(int);

					len = *((int *)paquete.buffer);
					if (codigo != (len&0xffff))
					{
						*_n = 0;
					}
					else
					{						
						len >>= 16;
						len &= 0xffff;

						if (local_clave[i])
						{
							int n;
							if (*_n >= 8)
							{
								for (n = 0;n < *_n;n += 8)
								{
									x_c_decode((long *)(paquete.buffer+sizeof(int)+n),(long *)local_clave[i]);
								}
							}
							*_n = len;
						}
						else
						{
							if (*_n != len)
							{
								*_n = 0;
							}
						}
						if (*_n > 0)
						{
							*_dato = malloc(*_n);
							memcpy(*_dato,paquete.buffer+sizeof(int),*_n);
						}
					}
				}
			}
			r = *_n;
		}
	}
	return r;
}

static int itcp_saca_linea_texto(char *buffer,int len,char *destino)
{
	int n,i;

	destino[0] = 0;
	for (n = 0;n < len;n++)
	{
		if (buffer[n] == 10 || (buffer[n] == 13 && (n+1) < len && buffer[n+1] == 10))
		{
			destino[0] = n + 1;
			if (n > 0)
			{
				memcpy(destino+1,buffer,n);
			}
			destino[n+1] = 0;
			
			if (buffer[n] == 10)
				n++;
			else
				n += 2;
			len -= n;
			for (i = 0;i < len;i++)
			{
				buffer[i] = buffer[n+i];
			}
			break;
		}
	}
	return len;
}

static int ndstcp = 0;
static int ndstcpsocks = 0;
static SOCKET dstcp_socks[100];

int ds_tcp_conecta(char *destino,int port)
{
   SOCKET sock = SOCKET_ERROR;
   struct sockaddr_in addr;
   int i;
#ifdef FGVDOS
#ifndef WATCOMNLM
   WORD      wVersionRequested;
   WSADATA   wsaData;
#endif
#endif
#ifdef FGVDOS
#ifdef RWSERVER
   if (!ndstcp && !en_conexion_externa && !en_conexion_terminal)
#else
      if (!en_conexion_terminal)
#endif
      {
         wVersionRequested = MAKEWORD(1,1);
         if (WSAStartup(wVersionRequested, &wsaData) != 0)
         {	 
            return -1;
         }
      }
#endif

   addr.sin_family = AF_INET;
   addr.sin_port   = htons((unsigned short)port);
   addr.sin_addr.s_addr = inet_addr(destino);
   sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
   if (sock != SOCKET_ERROR)
   {
	   FastSocket(sock);
	   if (ds_connect_(sock,(struct sockaddr *) &addr,sizeof (addr)) != SOCKET_ERROR)
	   {
		   for (i = 0;i < ndstcpsocks;i++)
		   {
			   if (dstcp_socks[i] == SOCKET_ERROR)
			   {
				   break;
			   }
		   }
		   if (i >= ndstcpsocks)
			   ndstcpsocks++;

		   dstcp_socks[i] = sock;
		   ndstcp++;
		   return i;
	   }
   }

   closesocket(sock);
#ifdef FGVDOS
#ifdef RWSERVER
      if (!ndstcp && !en_conexion_externa && !en_conexion_terminal)
#else
         if (!en_conexion_terminal)
#endif
         {
            WSACleanup();  
         }
#endif		 

   return -1;
}

int ds_tcp_envia_s(int i,char *s,int len)
{
	if (i < 0 || i >= 100 || dstcp_socks[i] == SOCKET_ERROR)
		return -1;	
	if (rwi_senddata(dstcp_socks[i],s,len) != FALSE)
		return 0;
	return -1;
}

int ds_tcp_cuanto(int i)
{
	long cuanto;
	if (i < 0 || i >= 100 || dstcp_socks[i] == SOCKET_ERROR)
		return -1;	
	cuanto = 0L;
	if (ioctlsocket(dstcp_socks[i],FIONREAD,&cuanto) != SOCKET_ERROR)
	{
		return cuanto;
	}
	return -1;
}

int ds_tcp_recibe_s(int i,char *s,int len)
{
	if (i < 0 || i >= 100 || dstcp_socks[i] == SOCKET_ERROR)
		return -1;	
	if (rwi_recvdata(dstcp_socks[i],s,len) != FALSE)
		return len;
	return -1;
}

int ds_tcp_desconecta(int i)
{	
	if (i < 0 || i >= 100 || dstcp_socks[i] == SOCKET_ERROR)
		return -1;	

	closesocket(dstcp_socks[i]);
	dstcp_socks[i] = SOCKET_ERROR;
	ndstcp--;

#ifdef FGVDOS
#ifdef RWSERVER
      if (!ndstcp && !en_conexion_externa && !en_conexion_terminal)
#else
         if (!en_conexion_terminal)
#endif
         {
            WSACleanup();  
         }
#endif
	return 0;
}


int DSTCP_VInterface(int port,int param)
{
	SOCKET sock = SOCKET_ERROR,client;
	struct sockaddr_in addr;
	struct hostent *mihost;
	int len;
	int punto = -1;
	int nosalir = -1;
	long tiempo;
#ifdef FGVDOS
#ifndef WATCOMNLM
   WORD      wVersionRequested;
   WSADATA   wsaData;
#endif
#endif
#ifdef FGVDOS
#ifdef RWSERVER
   if (!en_conexion_externa && !en_conexion_terminal)
#else
      if (!en_conexion_terminal)
#endif
      {
         wVersionRequested = MAKEWORD(1,1);
         if (WSAStartup(wVersionRequested, &wsaData) != 0)
         {	 
            return -1;
         }
      }
#endif

	do
	{
		if ((sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) == SOCKET_ERROR)
		{
			break;
		}
		FastSocket(sock);
		addr.sin_family = AF_INET;
		addr.sin_port   = htons((unsigned short)port);
		addr.sin_addr.s_addr = inet_addr("0.0.0.0");
		if (bind(sock,(struct sockaddr *) &(addr),sizeof (addr)) == SOCKET_ERROR)
		{
			break;
		}
		if (listen(sock, 1 ) == SOCKET_ERROR)
		{
			 break;
		}
		do
		{			
			len = sizeof(addr);
			client = accept(sock,&addr,&len);
			if (client == INVALID_HANDLE_VALUE)
			{
				break;
			}

			tiempo = time(NULL);

			FastSocket(client);

			punto = 0;

			if (rwi_senddata(client,"DSITCP 1\r\n",-1) == FALSE)
			{
				break;
			}
			{
				fd_set setread;
				fd_set seterror;
				struct timeval timeout;
				struct timeval *ptm = NULL;	
				unsigned long cuanto;
				int readsize, totalbytesread, msgsize, bytesread,err;
				int maximo = 0;
				char nomdef[256];
				char tmp[10240];
				char linea[4096];
				int lectura = 0;				
				int verclien = 0;
				void *data = NULL;

				do
				{
					timeout.tv_sec  = 60;
					timeout.tv_usec = 0;
					ptm = &timeout;

					FD_ZERO(&setread);
					FD_SET(client,&setread);
					FD_ZERO(&seterror);
					FD_SET(client,&seterror);
					maximo = client+1;
#ifndef WIN32
			   errno = 0;
#endif
					if ((err = select(maximo,&setread,NULL,&seterror,ptm)) == SOCKET_ERROR)
					{
#ifndef WIN32
						 if (err == SOCKET_ERROR && errno == EINTR)
						 {
							 continue;
						 }
#endif

						break;
					}
					if (!err)
					{
						if (!punto)
						{
							break;
						}
						continue;
						//return(0); // timeout 
					}
				
					cuanto = 0L;
					if (ioctlsocket(client,FIONREAD,&cuanto) == SOCKET_ERROR)
					{
						break;
					}

					if (!cuanto)
					{
						break;
					}

					if ((cuanto+lectura) > sizeof(tmp))
					{
						cuanto = sizeof(tmp) - lectura;
					}

					if (cuanto > 0)
					{
						if (rwi_recvdata(client,tmp+lectura,cuanto) == FALSE)
						{
							break;
						}
						lectura += cuanto;
					}
					
					lectura = itcp_saca_linea_texto(tmp,lectura,linea);

					if (!linea[0])
					{
						if (cuanto <= 0)
						{
							/* buffer lleno y no hay linea de texto a asacar */
							if (rwi_senddata(client,":ERROR: Linea demasiado larga.\r\n",-1) == FALSE)
							{
								break;
							}
							break;
						}
						continue;
					}	
					
					if (punto > 1)
					{
						punto = itcp_lista_def(client,linea+1,punto,&data);
						if (punto <= 0)
						{
							break;
						}
						continue;
					}

					if (!punto)
					{
						if (strncmp(linea+1,"ITCPDS ",7))
						{
							break;
						}
						verclien = atoi(linea+8);
						punto = 1;
						if (verclien != 1)
						{
							rwi_senddata(client,":ERROR: Version no soportada.\r\n",-1);
							break;
						}
						if (rwi_senddata(client,":OK: Version de cliente aceptada.\r\n",-1) == FALSE)
						{
							break;
						}
						continue;
					}

					if (!strcmp(linea+1,"HELP"))
					{						
						rwi_senddata(client,":OK: Lista de comandos disponibles.\r\n",-1);
						rwi_senddata(client,"LISTA:def\r\n",-1);
						rwi_senddata(client,"DEF:def\r\n",-1);
						rwi_senddata(client,"GRABA:def<lf>numlins<lf>registro<lf>registro<lf> (...) .<lf>\r\n",-1);
						rwi_senddata(client,"BORRA:def,claveprimaria\r\n",-1);
						rwi_senddata(client,"BORRATODO:def\r\n",-1);
						rwi_senddata(client,"LEE:def,numclave,= o > o ] o p o u o X,datos clave o posicion relativa segun orden clave para X\r\n",-1);
						rwi_senddata(client,"LISTAFILTRO:def,numclave<lf>datos clave desde<lf>datos clave hasta<lf>\r\n",-1);
						rwi_senddata(client,"SQL:sentencia sql'interno'\r\n",-1);
						rwi_senddata(client,"LEEVAR:variable\r\n",-1);
						rwi_senddata(client,"GRABAVAR:variable,valor\r\n",-1);
						rwi_senddata(client,"PROCESO:proceso\r\n",-1);
						rwi_senddata(client,"HELP\r\n",-1);
						rwi_senddata(client,"EXIT\r\n",-1);
						rwi_senddata(client,".\r\n",-1);
						continue;
					}

					if (!strncmp(linea+1,"LISTA:",6))
					{

						if (itcp_lista_def(client,linea+7,0,NULL) <= 0)
						{
							break;
						}
						continue;
					}


					if (!strncmp(linea+1,"DEF:",4))
					{
						if (itcp_lista_def(client,linea+5,1,NULL) <= 0)
						{
							break;
						}
						continue;
					}

					if (!strncmp(linea+1,"GRABA:",6))
					{
						if ((punto = itcp_lista_def(client,linea+7,2,&data)) <= 0)
						{
							break;
						}						
						continue;
					}

					if (!strncmp(linea+1,"BORRA:",6))
					{
						if (itcp_lista_def(client,linea+7,3,NULL) <= 0)
						{
							break;
						}
						continue;
					}

					if (!strncmp(linea+1,"BORRATODO:",10))
					{
						if (itcp_lista_def(client,linea+11,4,NULL) <= 0)
						{
							break;
						}
						continue;
					}

					if (!strncmp(linea+1,"LEE:",4))
					{
						if (itcp_lista_def(client,linea+5,5,NULL) <= 0)
						{
							break;
						}
						continue;
					}

					if (!strncmp(linea+1,"LISTAFILTRO:",12))
					{
						if ((punto = itcp_lista_def(client,linea+13,6,&data)) <= 0)
						{
							break;
						}
						continue;
					}

					if (!strncmp(linea+1,"SQL:",4))
					{
						if (itcp_varios(client,3,linea+5) <= 0)
							break;
						continue;
					}

					if (!strncmp(linea+1,"LEEVAR:",7))
					{
						if (itcp_varios(client,1,linea+8) <= 0)
							break;						
						continue;
					}

					if (!strncmp(linea+1,"GRABAVAR:",9))
					{
						if (itcp_varios(client,2,linea+10) <= 0)
							break;						
						continue;
					}

					if (!strncmp(linea+1,"PROCESO:",8))
					{
						if (itcp_varios(client,0,linea+9) <= 0)
							break;						
						continue;
					}

					if (!strcmp(linea+1,"EXIT"))
					{
						sprintf(tmp,":OK: %ld segundos de conexion.\r\n",time(NULL)-tiempo);
						rwi_senddata(client,tmp,-1);
						nosalir = 0;
						break;
					}

					{
						char tmpx[10000];
						int g;
						sprintf(tmpx,":ERROR: Comando no soportado [%s] <",linea+1);						
						for (g = 1;linea[g];g++)
						{
							if (g > 1)
								strcat(tmpx,",");
							sprintf(tmpx+strlen(tmpx),"%d",linea[g]);
						}
						strcat(tmpx,">.\r\n");
						if (rwi_senddata(client,tmpx,-1) == FALSE)
						{
							break;
						}
					}
					
				} while(nosalir);
				if (data)
				{
					free((char *)data);
				}
			}
			closesocket(client);
		} while(nosalir);
	} while(0);

	if (sock != SOCKET_ERROR)
		closesocket(sock);

#ifdef FGVDOS
#ifdef RWSERVER
      if (!en_conexion_externa && !en_conexion_terminal)
#else
         if (!en_conexion_terminal)
#endif
         {
            WSACleanup();  
         }
#endif

	return 0;
}

#define MAXWATCHMANS 100
int dsdb_lista_watchmans(long ips[])
{
	FILE *file;
	int i;
	int n;
	char tmp[512];

	ips[0] = 0;
	file = fopen("/dswm/dslista.wm","r");
	if (file)
	{
		if (fgets(tmp,510,file))
		{
			quitan(tmp);
			quitab(tmp);
			if (!strcmp(tmp,"dswm"))
			{
				n = ips[0] + 1;
				while(fgets(tmp,510,file) && n <= MAXWATCHMANS)
				{
					quitan(tmp);
					quitab(tmp);
					ips[n] = inet_addr(tmp);
					n++;
				}
				ips[0] = n - 1;
			}
		}
		fclose(file);
	}
}

/*
extern unsigned long ip_2key;
extern char **lista_emprecopias;
*/

int carga_emprecopias()
{
	FILE *file;
	int i;	
	char tmp[512];	

	if (lista_emprecopias)
	{
		for (i = 0;lista_emprecopias[i];i++)
		{
			free(lista_emprecopias[i]);
		}		
		free(lista_emprecopias);
	}
	lista_emprecopias = NULL;
	
	file = fopen(rw_directorio("copiasconsulta.txt"),"r");
	if (file)
	{
		if (fgets(tmp,510,file))
		{
			quitan(tmp);
			quitab(tmp);
			ip_2key = inet_addr(tmp);
			lista_emprecopias = malloc(sizeof(char *) * 502);

			i = 0;
			while(fgets(tmp,510,file) && i < 500)
			{
				quitan(tmp);
				quitab(tmp);
				lista_emprecopias[i] = malloc(strlen(tmp)+1);
				strcpy(lista_emprecopias[i],tmp);
				i++;
			}
			lista_emprecopias[i] = NULL;
			lista_emprecopias[i+1] = NULL;

		}
		fclose(file);
	}
}


int negocia_dsdb(unsigned char *clave)
{
   SOCKET sock;
   struct sockaddr_in addr;
   long ips[MAXWATCHMANS+1];
   Paquete paquete;
   char tmp[128];
   char identidad[128];
   int i;

   ip_2key = 0L;
   memset(copia_key,0,sizeof(copia_key));
   if (dsdb_usa_crypto)
   {	   
	   strcpy(tmp,rw_directorio(""));

	   i = strlen(tmp) - 1;
	   if (i > 0)
	   {
		   if (tmp[i] == '/')
		   {
			   tmp[i] = 0;
		   }
		   strcpy(identidad,quita_path(tmp));

		   ips[0] = 0;
		   dsdb_lista_watchmans(ips);
		   carga_emprecopias();
		   if (ips[0] < 1)
		   {
			   aviso(0,"No se disponen de servidores de claves, configuración necesaria.");
			   return 0;
		   }
		   /* negociar la clave */	   		   
		   for (i = 1;i <= ips[0];i++)
		   {
			   addr.sin_family = AF_INET;
			   addr.sin_port = htons(5007);
			   addr.sin_addr.s_addr = ips[i];
			   sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
			   if (sock == SOCKET_ERROR)
				   break;
			   FastSocket(sock);
			   if (ds_connect_(sock,(struct sockaddr *) &addr,sizeof (addr)) != SOCKET_ERROR)
			   {			   
				   paquete.c.id = DSWEBCALLID;
				   paquete.c.command = C_DATA | ((sizeof(int) + strlen(identidad) + 1) & 0xfff);
				   *((int *)paquete.buffer) = 1;
				   strcpy(paquete.buffer+sizeof(int),identidad);
				   if (rwi_senddata(sock,(char *)&paquete,LEN_PAQUETE(&paquete)))
				   {				   
					   if (RecibePaquete(sock, &paquete,15000) == 1 && COMANDO(&paquete) == C_DATA)
					   {
						   if (LEN_DATA(&paquete) > (256+sizeof(int)) && *((int *)paquete.buffer) == 2 && !strcmp(paquete.buffer+sizeof(int)+256,identidad))
						   {
							   clave[0] = paquete.buffer[7]  ^ paquete.buffer[11];
							   clave[1] = paquete.buffer[16] ^ paquete.buffer[128];
							   clave[2] = paquete.buffer[5]  ^ paquete.buffer[243];
							   clave[3] = paquete.buffer[33] ^ paquete.buffer[99];
							   clave[4] = paquete.buffer[75] ^ paquete.buffer[83];
							   clave[5] = paquete.buffer[49] ^ paquete.buffer[54];
							   clave[6] = paquete.buffer[61] ^ paquete.buffer[148];
							   clave[7] = paquete.buffer[10] ^ paquete.buffer[111];
							   clave[8] = paquete.buffer[13] ^ paquete.buffer[201];
							   clave[9] = paquete.buffer[25] ^ paquete.buffer[66];
							   clave[10]= paquete.buffer[27] ^ paquete.buffer[156];
							   clave[11]= paquete.buffer[68] ^ paquete.buffer[168];
							   clave[12]= paquete.buffer[19] ^ paquete.buffer[45];
							   clave[13]= paquete.buffer[24] ^ paquete.buffer[124];
							   clave[14]= paquete.buffer[37] ^ paquete.buffer[137];
							   clave[15]= paquete.buffer[38] ^ paquete.buffer[238];
							   closesocket(sock);
							   if (ip_2key)
							   {
								   int recibido;

								   recibido = 0;
								   sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
								   if (sock != SOCKET_ERROR)
								   {
									   FastSocket(sock);
									   addr.sin_family = AF_INET;
									   addr.sin_port = htons(22276);
									   addr.sin_addr.s_addr = ip_2key;
									   if (ds_connect_(sock,(struct sockaddr *) &addr,sizeof (addr)) != SOCKET_ERROR)
									   {										   
										   if (rwi_senddata(sock,"cqea0abc.",10) != FALSE)
										   {
											   long cuanto;
											   int n;

											   n = 0;
											   while(n < 100)
											   {
												   cuanto = 0;
												   if (ioctlsocket(sock,FIONREAD,&cuanto) == SOCKET_ERROR)
												   {
													   n = 100;
													   break;
												   }
												   if (cuanto >= 248)
													   break;
#ifdef FGVDOS
												   Sleep(100);
#else
												   usleep(100000);
#endif
												   n++;
											   }         
											   if (n < 100)
											   {											   
												   if (rwi_recvdata(sock,copia_key,248) == FALSE)
												   {
													   memset(copia_key,0,sizeof(copia_key));
												   }
												   else
												   {
													   for (n = 0;n < 248;n++)
													   {
														   if (copia_key[n])
															   break;
													   }
													   if (n < 248)
															recibido = 1;
												   }
											   }
										   }
									   }
									   closesocket(sock);
								   }
								   if (!recibido)
								   {
				   					   aviso(0,"Falta módulo de protección de datos (mochila).");
									   return 0;
								   }
							   }
							   return 1;
						   }
					   }
				   }
			   }
			   closesocket(sock);
		   }
	   }
	   /*                   */
	   aviso(0,"No se dispone de clave de cifrado de datos, el administrador debe proporcionarla.");
	   return 0;
   }   
   return 1;
}

#else
#include <stdio.h>

int envia_subejecuta_a_server()
{
}

int leecontrol_fistro()
{
}

int envia_control_a_server()
{
}

int t_rwfclose(FILE *f)
{
   return(fclose(f));
}
char *t_lee_linea(texto,n,file1)
char *texto;
int n;
FILE *file1;
{  
   return(lee_linea(texto,n,file1));
}
FILE *t_rwfopen(f,m)
char *f,*m;
{
			return(fopen(f,m));
}
#endif


int memcpy_short(char *dest,void *src,int len)
{
   int i;
   memcpy(dest,src,len);
   for (i = 0;i < (len/sizeof(short));i++)
   {
	  moto_short(dest);
	  dest += sizeof(short);
   }
   return len;
}

int memcpy_int(char *dest,void *src,int len)
{
int i;
   memcpy(dest,src,len);
   for (i = 0;i < (len/sizeof(int));i++)
   {
	  moto_int(dest);
	  dest += sizeof(int);
   }
   return len;
}

int memcpy_long(char *dest,void *src,int len)
{
int i;
   memcpy(dest,src,len);
   for (i = 0;i < (len/sizeof(long));i++)
   {
	  moto_long(dest);
	  dest += sizeof(long);
   }
   return len;
}

int memcpy_VENT(char *dest,void *src,int len)
{
 int i;
   memcpy(dest,src,len);
   for (i = 0;i < (len/sizeof(VENT));i++)
   {
	  moto_VENT(dest);
	  dest += sizeof(VENT);
   }
   return len;
}




