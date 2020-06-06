

#include <fgvsys.h>

#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef FGVDOS
#include <io.h>
#include <process.h>
#include <windows.h>            
#include <windowsx.h>

#include <winsock.h>
#include <nspapi.h>
#include <sys/utime.h>

#else

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


#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef O_BINARY
#include <io.h>
#endif
#include <stdio.h>
#include <rtermin.h>
#include <stdlib.h>
#include <malloc.h>

#ifndef O_BINARY
#define O_BINARY 0
#endif


extern char *salvapar;
extern char *quita_path();

extern int rtme_version1;
extern int rtme_version2;
extern unsigned char rtme_versionletra;

extern int sin_iconos;

typedef struct _MSGHDR
{
			BYTE            signature;
			BYTE            command;
			WORD            length;    
} MSGHDR;


#define MAXRDATA 0x2000
typedef struct _DATAMSG            
{
         MSGHDR          hdr;
         BYTE            data[MAXRDATA];  
} DATAMSG;

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

static SOCKDATA MySock;
static SOCKET rwsock = INVALID_HANDLE_VALUE;
static struct sockaddr_in rwnetcon;

static int  rwmax_buffer = 0x6000;

#define RWNIDENTIFICACION    146/* identificacion para los mensajes entorno FGV */
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
#define SOCORRO			      99

#define FGV_SORT                  7
#define RW_INDEX                  8

unsigned short PORTRWNETCON = 21765;
unsigned short RWPORTIP = 56743;

#ifndef MSG_WAITALL
#define MSG_WAITALL 0
#endif

#ifndef IPTOS_THROUGHPUT
#define IPTOS_THROUGHPUT 0x08
#endif
#ifndef IPTOS_LOWDELAY
#define IPTOS_LOWDELAY 0x10
#endif


int en_conexion_terminal = 0;
extern int server_id;

extern char nmen0[551];


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
 
VOID 
GetWindowTitles( 
    PTASK_LIST_ENUM te 
    ); 
 
BOOL 
MatchPattern( 
    PUCHAR String, 
    PUCHAR Pattern 
    ); 

#define MAX_TASKS 256
TASK_LIST tlista[MAX_TASKS]; 
#endif

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

extern char server_request[128];
extern char segundo_dir[128];
extern char *lee_linea();

struct info_paquete *datos_paquete();
struct info_paquete *instala_paquete();



static DATAMSG xferbuf;
static DATAMSG xrcbbuf;

extern int ultima_instalacion;

static int _nvolume = 0;
static char *_volume[100];

static char lpServName[81];

void cierra_todo()
{

}

fd_set setread;
fd_set seterror;
struct timeval timeout;


static BOOL rwi_senddata(SOCKET sock,char  *sendbuf,int totalbytes)
{
			int bytessent;
			int i,err;			
			unsigned char controla;			


	  do 
	     {
		 bytessent = 0;			  
         while(bytessent < totalbytes)
			{
            i = send(sock,(char *)sendbuf + bytessent,totalbytes - bytessent,0);
            if (i == SOCKET_ERROR)
               {
#ifdef FGVDOS
										  if (WSAGetLastError() != WSAEWOULDBLOCK)
										  {
													  
													  return FALSE;
										  }
#else
										 if (errno != EWOULDBLOCK)
										 {
													  return FALSE;
										 }
#endif

			   continue;
			   }
			 else
               bytessent += i;
             }

         return TRUE;

		 } while(-1);
	return FALSE;
}


BOOL recvdatamessage(SOCKET sock, DATAMSG *recvbuf)
{
         int readsize, totalbytesread, msgsize, bytesread,err;
		 unsigned char controla;
		 DATAMSG *pr;		 
		 char tmp[128];
		 char tmp2[128];
		 unsigned long tam;
         unsigned long rec;
		 int file = -1,pl;
		 VENT *w = NULL;

		 

do      {         

		 readsize = HDRSIZE;
		 pr = recvbuf;		   		   
		 if((bytesread = recv(sock, (char *)pr,readsize,MSG_WAITALL)) == SOCKET_ERROR)
           {					
           return FALSE;
           }
		 
		 totalbytesread = bytesread;        
         if(pr->hdr.signature != RWNIDENTIFICACION)
           {		   
           return FALSE;
           }

         msgsize = pr->hdr.length;
         readsize = msgsize - totalbytesread;
         while(totalbytesread < msgsize)
			 {
						if((bytesread = recv(sock,
													(char *)pr + totalbytesread,
													  readsize,
													  0)) == SOCKET_ERROR)
						{
#ifdef FGVDOS
										  if (WSAGetLastError() != WSAEWOULDBLOCK)
										  {													  
													  return FALSE;
										  }
#else
										 if (errno != EWOULDBLOCK)
										 {
													  return FALSE;
										 }
#endif
						continue;
						}
						totalbytesread += bytesread;
						readsize -= bytesread;
			}        


		 if (pr->hdr.command == FILE_TRANSFER_OPEN)
		   {
		   if (pr->hdr.length > (HDRSIZE+sizeof(long)))
		      {
			  memcpy(&tam,pr->data,sizeof(long));
			  rec = 0L;
			  strcpy(tmp,rw_directorio(pr->data+sizeof(long)));			  
			  file = rw_open_(tmp,O_RDWR | O_CREAT | O_TRUNC | O_BINARY,S_IREAD | S_IWRITE);
			  if (file >= 0)
			     {
                 xferbuf.hdr.signature = RWNIDENTIFICACION;
                 xferbuf.hdr.command   = FILE_TRANSFER_OK;                 
                 xferbuf.hdr.length = HDRSIZE;
                 if (rwi_senddata(sock,(char *)&xferbuf,HDRSIZE) == FALSE)
                   {                 
                   rw_close_(file);
                   return(FALSE);
                   }
				 if (en_conexion_terminal)
				   {
				   w = w_ini(2401,2480);
				   w_salva(w);
				   pprint(2401,"Recibiendo ");
				   v_print(tmp);				   
				   v_printl(" %ld",tam);
				   pl = (Pcursor/80)*100 + Pcursor%80 + 102;
				   v_print("/0.");
				   }
				 continue;
			     }			  
		      }
           xferbuf.hdr.signature = RWNIDENTIFICACION;
           xferbuf.hdr.command   = FILE_TRANSFER_NOK;
           xferbuf.hdr.length = HDRSIZE;
           rwi_senddata(sock,(char *)&xferbuf,HDRSIZE);
           return(FALSE);              
		   }
		 else
		 if (pr->hdr.command == FILE_TRANSFER_DATA)
		   {
		   if (file != -1)
		      {
			  rw_write_(file,pr->data,msgsize-HDRSIZE);
			  rec += (msgsize-HDRSIZE);
			  if (w)
			    {
                cursor(pl);
				v_printl("%ld.",rec);
			    }
			  continue;
		      }
		   if (w)
		      {
			  w_fin(w);
			  }
           return(FALSE);
		   }
		 else
		 if (pr->hdr.command == FILE_TRANSFER_CLOSE)
		   {
		   if (file != -1)
		      {
			  if (en_conexion_terminal && w)
			     {
				 pprint(pl,"COMPLETADO........");
			     } 
			  rw_close_(file);
			  file = -1;
			  if (pr->hdr.length > HDRSIZE)
			     {
				 strcpy(tmp2,rw_directorio(pr->data));
				 unlink(tmp2);
				 rename(tmp,tmp2); /* si no se puede paciencia !!! */
			     }
		      }
          if (w)
	          {
			  w_pinta(w);
	  	      w_fin(w);
			  }	   
		      
		   return(TRUE);
		   }
		 else
         if (pr->hdr.command == BLOQUE_DE_MENSAJES)
		    {			
		    if (file != -1)
		      {
			  rw_close_(file);
		      } 
	        if (w)
		      {
			  w_fin(w);
			  }
		    return FALSE;
			}                  
	      else
          if (pr->hdr.command == BLOQUE_COMPRIMIDO)
		    {
		    if (file != -1)
		      {
			  rw_close_(file);
		      } 
	        if (w)
		      {
			  w_fin(w);
			  }
			return FALSE;
		    }

		  if (file != -1)
		    {
			rw_close_(file);
		    }		  

          if (w)
  	        { 
			w_fin(w);
			}          

	      return TRUE;

     } while (-1);

   return FALSE;
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
 
  wVersionRequested = MAKEWORD(1,1);
  if (WSAStartup(wVersionRequested, &wsaData) != 0)
	 {	 
	 return -1;
	 }
#endif
if (server_request[0] == '@') 
   {
   for(i = 3;server_request[i] && server_request[i] != ':' && server_request[i] != ')';i++);
   if (ntmp <= (i-3))
     {
     memcpy(tmp,server_request+3,i-3);
	 tmp[i-3] = 0;
	 quita_blancos(tmp);
	 if (strlen(tmp))
	    {
        if (inet_addr(tmp) != INADDR_NONE)
		  {
#ifdef FGVDOS
	      WSACleanup();
#endif
		  return(0);
		  }
	    }
	 else
		gethostname(tmp,ntmp);
     }
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
WSACleanup();
#endif
return(0);
}


int dame_ser_por_defecto(tmp,ntmp)
char *tmp;
int ntmp;
{
int i;
struct hostent *mihost;
union agi_in_addr S_un;
#ifdef FGVDOS
WORD      wVersionRequested;
WSADATA   wsaData;
 
  wVersionRequested = MAKEWORD(1,1);
  if (WSAStartup(wVersionRequested, &wsaData) != 0)
	 {	 
	 return -1;
	 }
#endif
if (server_request[0] == '@') 
   {
   for(i = 3;server_request[i] && server_request[i] != ':' && server_request[i] != ')';i++);
   if (ntmp <= (i-3))
     {
     memcpy(tmp,server_request+3,i-3);
	 tmp[i-3] = 0;
	 quita_blancos(tmp);
	 if (strlen(tmp))
	    {
        if (inet_addr(tmp) != INADDR_NONE)
		  {
#ifdef FGVDOS
	      WSACleanup();
#endif
		  return(0);
		  }
	    }
	 else
		gethostname(tmp,ntmp);
     }
   }
else
   gethostname(tmp,ntmp);

if ( (mihost = gethostbyname(tmp)) && mihost->h_length == 4 && mihost->h_addr_list[0])
   {	    
   S_un.S_addr = *((unsigned long *)mihost->h_addr_list[0]);
   sprintf(tmp,"%d.%d.%d.1",S_un.S_un_b.s_b1,S_un.S_un_b.s_b2,S_un.S_un_b.s_b3);
   }
#ifdef FGVDOS
WSACleanup();
#endif
return(0);
}


int responde_sesion()
{
unsigned long cuanto;
int err;


  FD_ZERO(&setread);
  FD_SET(MySock.sock,&setread);
  FD_ZERO(&seterror);
  FD_SET(MySock.sock,&seterror);
  timeout.tv_sec = 60L; /* 60 segundos para obtener respuesta ... sino nada*/
  timeout.tv_usec = 0L;
  if ((err = select(MySock.sock+1,&setread,NULL,&seterror,&timeout)) == SOCKET_ERROR)
  {
        return(-1);
  }
  if (!err)  
  {
	   pprint(1001,"(TIMEOUT!)");	   
	return(-1);
  }

  if (FD_ISSET(MySock.sock,&seterror))
   {
	   pprint(1001,"(EXCEPCION!)");	   
     return(-1);   
   }
  cuanto = 0L;
  ioctlsocket(MySock.sock,FIONREAD,&cuanto);
  if (!cuanto)
		{
        return(-1);
        }  
if (recvdatamessage(MySock.sock,&xrcbbuf) == FALSE) 
{
   return(-1);
}
return(xrcbbuf.hdr.command);
}


int envia_control_a_server(pdata,len,comando)
char *pdata;
int len;
int comando;
{
  xferbuf.hdr.signature = RWNIDENTIFICACION;
  xferbuf.hdr.command = CONTROL_DE_TERMINAL;  
  xferbuf.hdr.length = HDRSIZE + 1 + len;
  xferbuf.data[0] = comando;
  memcpy(xferbuf.data+1,pdata,len);
  rwi_senddata(MySock.sock,(char *)&xferbuf,xferbuf.hdr.length);  
  return(len);
}


int establece_conexion(modo,npa,paqs)
int modo;
int npa;
char **paqs;
{
int j,k,i,kki,pid,tmerr;
char tmp[256];
unsigned char *pdata;
union agi_in_addr S_un;
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


if (modo) return(-1); /* no usar servidor de indices de momento */

if (!memcmp(server_request+1,"(C:",3))
  {
  if (server_request[4] == '?')
    {	
    for (i = 5;server_request[i] && server_request[i] != ')';i++);
	strcpy(tmp,server_request+i);
	server_request[i] = 0;
    strcpy(server_request+4,server_request+5);
	pprint(2401,"Entre la Direccion del Servidor:");
	entra(2433,16,"","",server_request+4,0,server_request+4);
	quita_blancos(server_request+4);
	strcat(server_request+4,tmp);
    }  
  sprintf(tmp,"ESTABLECIENDO CONEXION CON (%s",server_request+4);
  mensaje(0,tmp);    
  }
else
  {
  sprintf(tmp,"MALA OPCION CLIENTE-SERVIDOR: %s",server_request);
  aviso(0,tmp);
  return(-1);
  }

if (!time(NULL))
  {
  aviso(0,"LA FECHA DE SU SISTEMA ES INCORRECTA, SEGURAMENTE MAYOR DEL AÑO 2030.");
  return(-1);
  }

#ifdef FGVDOS
  wVersionRequested = MAKEWORD(1,1);
  if (WSAStartup(wVersionRequested, &wsaData) != 0)
	 {
	 aviso(0,"FALLO EN INICIALIZACION DE LOS SOCKETS DE WINDOWS");
	 return -1;
	 }
#endif


MySock.sock = INVALID_HANDLE_VALUE;
if (rwsock != INVALID_HANDLE_VALUE)
  {
  closesocket(rwsock);
  }
rwsock = INVALID_HANDLE_VALUE;

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
		if (lpServName[i] == ')')
		  {
		  lpServName[i] = 0;
		  break;
		  }
		i++;
	    }
	  PORTRWNETCON = (unsigned short)atoi(lpServName+j+1);
	  break;
	  }
	if (lpServName[j] == ')')
	  {
      lpServName[j] = 0;
	  break;
	  }
	j++;
    } 
  if((rwsock =  socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) == SOCKET_ERROR)
	{
    aviso(0,"NO HAY SOCKETS DISPONIBLES");
#ifdef FGVDOS
	 WSACleanup();
#endif
	return (-1);
	}

  i = 1;
  if (setsockopt(rwsock,IPPROTO_TCP,TCP_NODELAY,&i,sizeof(i)) < 0)
    {
	i = 1;
	memcpy(tmp,&i,sizeof(int));
	setsockopt(rwsock,IPPROTO_TCP,TCP_NODELAY,tmp,sizeof(tmp));
    }  

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

  rwnetcon.sin_family = AF_INET;
  rwnetcon.sin_port = PORTRWNETCON;
  if (connect(rwsock,(struct sockaddr *) &rwnetcon,sizeof (rwnetcon)) == SOCKET_ERROR)
    {   
    sprintf(tmp,"No hay servicio RWNETCON en (%s:%u)",lpServName,PORTRWNETCON);
	 aviso(0,tmp);     
	closesocket(rwsock);
    rwsock = INVALID_HANDLE_VALUE;
#ifdef FGVDOS
    WSACleanup();
#endif	
    return(-1);   
    }

    sprintf(tmp,"CONECTADO CON RWNETCON (%s",server_request+4);
    mensaje(0,tmp);  	
    

#ifdef WATCOMNLM
  pid = GetNLMID();  
#else
#ifdef FGVDOS
  pid = GetCurrentProcessId();
#else
  pid = getpid();
#endif
#endif


  xferbuf.hdr.signature = RWNIDENTIFICACION;                 
  xferbuf.hdr.command = SERVIDOR_REQUERIDO;       	   
	 
  xferbuf.hdr.length = HDRSIZE + sizeof(int)*2 + sizeof(MySock.raddr.sin_addr.s_addr);  

  memcpy(xferbuf.data,&pid,sizeof(int));
  memcpy(xferbuf.data+sizeof(int),&server_id,sizeof(int));
  memcpy(xferbuf.data+sizeof(int)*2,&(MySock.raddr.sin_addr.s_addr),sizeof(MySock.raddr.sin_addr.s_addr));  
  
  
  if (rwi_senddata(rwsock,(char *)&xferbuf,xferbuf.hdr.length) == FALSE)
      {
		aviso(0,"Sin servicio!");
	  closesocket(rwsock);
	  rwsock = INVALID_HANDLE_VALUE;
#ifdef FGVDOS
       WSACleanup();
#endif
	   
	   return(-1);
      }  
  

  if (recvdatamessage(rwsock,&xferbuf) == FALSE || (xferbuf.hdr.command != DENEGADO_SERVICIO && 
	                                                 xferbuf.hdr.command != ACEPTADO_SERVICIO) )
      {
		aviso(0,"Sin respuesta adecuada!");
	  closesocket(rwsock);
	  rwsock = INVALID_HANDLE_VALUE;
#ifdef FGVDOS
      WSACleanup();
#endif
	  
	  return(-1);
      }
  

  if (xferbuf.hdr.command == DENEGADO_SERVICIO)
	  {
		aviso(0,"DENEGADO EL SERVICIO POR RWNETCON!");
	  closesocket(rwsock);
	  rwsock = INVALID_HANDLE_VALUE;
#ifdef FGVDOS
      WSACleanup();
#endif
	  
	  return(-1);
      }
  

	memcpy(&server_id,xferbuf.data,sizeof(int));
	strcpy(lpServName,xferbuf.data+sizeof(int));

	 closesocket(rwsock);  
	 rwsock = INVALID_HANDLE_VALUE;
     sprintf(tmp,"ASIGNADA CONEXION POR RWNETCON:%s",lpServName);
     mensaje(0,tmp);
	 


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
	  aviso(0,"NO HAY SOCKETS DISPONIBLES PARA CONTINUAR");
#ifdef FGVDOS
	WSACleanup();
#endif
	
	return(-1);
	}
MySock.peer = INVALID_HANDLE_VALUE;
j = 0;
MySock.addr.sin_addr.s_addr = inet_addr(lpServName);
MySock.addr.sin_family = AF_INET;
MySock.addr.sin_port = RWPORTIP;

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

i = 3600;
setsockopt(MySock.sock,SOL_SOCKET,SO_SNDTIMEO,&i,sizeof(i));
i = 3600;
setsockopt(MySock.sock,SOL_SOCKET,SO_RCVTIMEO,&i,sizeof(i));

lingo.l_onoff = 1;
lingo.l_linger = 2;
setsockopt(MySock.sock,SOL_SOCKET,SO_LINGER,&lingo,sizeof(lingo));



  for (i = 0;i < 3;i++)
    {
    tmerr = connect(MySock.sock,(struct sockaddr *) &(MySock.addr),sizeof (MySock.addr));
	if (tmerr != SOCKET_ERROR) break;
    }  
  if (tmerr == SOCKET_ERROR)
    {
	  sprintf(tmp,"No existe conexion a servidor en (%s:%u)",lpServName,RWPORTIP);
      aviso(0,tmp);
	  
	closesocket(MySock.sock);
#ifdef FGVDOS
    WSACleanup();
#endif
	
    return(-1);
    }

    sprintf(tmp,"CONECTADO CON:%s",lpServName);
    mensaje(0,tmp);
    



  if (!modo)
			{
					  en_conexion_terminal = 1;
					  if (responde_sesion() != SESION_ACEPTADA)
								 {
						         sprintf(tmp,"CONEXION %s DENEGADA!",lpServName); 
								 aviso(0,tmp);
								 establece_desconexion(0);
								 return(-1);
								 }
					   /* memcpy(&usando_compresion,xrcbbuf.data,sizeof(int)); */
                       sprintf(tmp,"CONEXION %s ACEPTADA",lpServName);
                       mensaje(0,tmp);
                       if (responde_sesion() != COMANDO_TERMINAL)
					      {
						         sprintf(tmp,"CONEXION %s TIMEOUT!",lpServName); 
								 aviso(0,tmp);
								 establece_desconexion(0);
								 return(-1);
					      }
					   for (i =0;i < npa;i++)
					      {
					      envia_control_a_server(paqs[i],strlen(paqs[i])+1,20); /* pedir paquete */
						  if (responde_sesion() != FILE_TRANSFER_CLOSE)
						    {
							break;
						    }
					      }
					   envia_control_a_server("",0,21);
					   establece_desconexion(modo);
					   return(0);					  
			}  
  establece_desconexion(modo);
  return(-1);
}



int establece_desconexion(modo)
int modo;
{

if (!modo)
  {
  if (!en_conexion_terminal)
    {
    return(-1);
    } 
  }
else
   {
   return(-1);
   }
closesocket(MySock.sock);
en_conexion_terminal = 0;
#ifdef FGVDOS
	WSACleanup();
#endif

return(0);
}


int pon_la_marca(char *fich,long la_marca)
{
long posi;
int file;
char tmp[256];

file = rw_open_(fich,O_RDWR | O_BINARY);
if (file >= 0)
   {
   posi = rw_lseek_(file,0L,SEEK_END);
   rw_lseek_(file,posi-13,SEEK_SET);
   rw_read_(file,tmp,13);
   if (!memcmp(tmp,"011298",6) && !memcmp(tmp+10,"FGV",3))
      {
	  rw_lseek_(file,posi-13,SEEK_SET);
	  }
   else
      {
	  rw_lseek_(file,0L,SEEK_END);
	  }
   memcpy(tmp,"011298",6);
   memcpy(tmp+6,&la_marca,4);
   memcpy(tmp+10,"FGV",3);
   rw_write_(file,tmp,13);
   rw_close_(file);
   }

return(0);
}


static int tipo_clave = 0;

static int mal_numero = 0;
static int cupo_cubierto = 0;

static long dame_numero_diagram(char *act_key,unsigned short *posi,unsigned short *nc)
{
unsigned char buf[512];
unsigned short nume_code;
unsigned short posicion;
unsigned char mi_checo;
unsigned short ant_checo;
unsigned char checo;
long nume_serie = 0L;

strcpy(buf,act_key);
de_new_code(buf,99,12);
checo = (buf[3] ^ buf[1] ^ buf[2] ^ buf[9] ^ buf[7]) % 50;
posicion = buf[10] + buf[8] * 50;
nume_serie = buf[0] + buf[4] * 50L + buf[5] * 50L * 50L + buf[6] * 50L * 50L * 50L;

if (buf[11] == checo)
    {   
    nume_code = 00000;
    }
else
    {
    nume_code = buf[3] + buf[1] * 50 + buf[2] * 50 * 50;
    nume_code ^= posicion;
    }

tipo_clave = 0;

if (nume_serie < 0L || nume_serie > 999999L || posicion > 1022 || posicion < 0) 
  {  
  strcpy(buf,act_key);
  de_new_code(buf,99,12);

  mi_checo = 0;
  mi_checo ^= buf[3];
  mi_checo ^= buf[1];
  mi_checo ^= buf[2];
  mi_checo ^= buf[9];
  mi_checo ^= buf[7];
  checo = mi_checo % 50;
  ant_checo = (buf[3] ^ buf[1] ^ buf[2] ^ buf[9] ^ buf[7]) % 50;
  posicion = buf[8] + buf[10] * 50;
  nume_serie = buf[6] + buf[5] * 50L + buf[4] * 50L * 50L + buf[0] * 50L * 50L * 50L;

  if (buf[11] == checo || buf[11] == ant_checo)
     {   
     nume_code = 00000;   
     }
  else
     {
     nume_code = buf[3] + buf[1] * 50 + buf[2] * 50 * 50;
     nume_code ^= posicion;
     }
  tipo_clave = 1;
  }

*posi = posicion;
*nc = nume_code;
return(nume_serie);
}

static int analiza_esta_linea(char *tmp)
{
int i,j;
   if (!tmp[0] || tmp[0] == ';') return 0;
   for (i = 0;tmp[i] && tmp[i] != '=' && tmp[i] != ';';i++);
   if (tmp[i] != '=') return 0;

   tmp[i] = 0;
   i++;
   
   for (j = i;tmp[j];j++)
     {
     if (tmp[j] == ';')
		 {        
		 tmp[j] = 0;
         break;
         }
     }
   quitabp(tmp);
   quitab(tmp);
   quitabp(tmp+i);
   quitab(tmp+i);

   return(i);
}


static int reemplaza_punto(char ***carga,int *lineas,int *maxi,char **_act_key,char *__act_key)
{
int i;
char **c = *carga;
int m = *maxi;
int l = *lineas;

if (*_act_key)
  {
  for (i = 0;i < l;i++)
    {
	if (c[i] == *_act_key)
	   {
	   free(c[i]);
       c[i] = malloc(strlen(__act_key)+1);
	   strcpy(c[i],__act_key);
	   *_act_key = c[i];
	   return(1);
	   }
	}
  }

if (l >= m)
  {
  m += 200;
  c = realloc(c,sizeof(char *)*m);
  }
c[l] = malloc(strlen(__act_key)+1);
strcpy(c[l],__act_key);
*_act_key = c[l];
l++;

*carga = c;
*maxi = m;
*lineas = l;
return(1);
}


static int es_buena_red(char *act_red,unsigned short posicion)
{
unsigned char _buf[512];
unsigned char *buf;
unsigned short oculto,comprueba;
int i;

strcpy(_buf,act_red);
i = analiza_esta_linea(_buf);
if (i < 1) return 0;
buf = _buf+i;

quita_blancos(buf);
        
de_new_code(buf,33,9);

if (!tipo_clave)
   {
   oculto = buf[2] + buf[4] * 50 + buf[0] * 50 * 50;
   comprueba = buf[6] + buf[7] * 50 + buf[8] * 50 * 50;
   comprueba ^= oculto;
   if (comprueba == posicion) return(1);
   }
else
   {
   oculto = buf[4] + buf[2] * 50 + buf[0] * 50 * 50;
   comprueba = buf[7] + buf[6] * 50 + buf[8] * 50 * 50;
   comprueba ^= oculto;
   if (comprueba == posicion) return(1);
   }


return(0);
}


int dame_paquete(char *__act_instalacion,unsigned short *pa,unsigned short *ve,char *clave,unsigned short nume_code,long nume_serie)
{
unsigned short paquete;
unsigned short version;
unsigned char _buf[512];
unsigned char *buf;
int i;

strcpy(_buf,__act_instalacion);
i = analiza_esta_linea(_buf);
if (i < 1) return 0;
buf = _buf+i;

quita_blancos(buf);
if (clave)
   strcpy(clave,buf);

if (!tipo_clave)
   {
   de_new_code(buf,99,9);
   paquete = buf[5] + buf[1] * 50 + buf[3] * 50 * 50;
   paquete = paquete ^ nume_code ^ (short)(nume_serie & 0xffff);
   version = buf[2] + buf[4] * 50 + buf[0] * 50 * 50;
   version = version ^ nume_code ^ (short)(nume_serie & 0xffff);
   }
else
   {        
   de_new_code(buf,99,9);
   paquete = buf[1] + buf[5] * 50 + buf[3] * 50 * 50;
   paquete = paquete ^ nume_code ^ (short)(nume_serie & 0xffff);
   version = buf[4] + buf[2] * 50 + buf[0] * 50 * 50;
   version = version ^ nume_code ^ (short)(nume_serie & 0xffff);
   }

*pa = paquete;
*ve = version;
return(0);
}

int integra_paquete(char ***carga,int *lineas,int *maxi,char *__act_instalacion,char **_act_instalacion,unsigned short nume_code,long nume_serie)
{
unsigned short paquete;
unsigned short version;
unsigned short otro_paquete;
unsigned short otra_version;
int i;
int maximo = 50;
char tmp[256];
char clave[256];

dame_paquete(__act_instalacion,&paquete,&version,clave,nume_code,nume_serie);

for (i = 0;i < 50;i++)
   {
   if (_act_instalacion[i])
      {
      dame_paquete(_act_instalacion[i],&otro_paquete,&otra_version,NULL,nume_code,nume_serie);
	  if (paquete == otro_paquete && version == otra_version)
	     {
		 sprintf(tmp,"InsPaq%02d=%s",i+1,clave);
         reemplaza_punto(carga,lineas,maxi,&(_act_instalacion[i]),tmp);
		 return(1);
		 }	  
	  if (maximo > i) maximo = i;
	  }
   else
      {
	  maximo = i;
	  }
   }

if (maximo >= 50)
   {
   cupo_cubierto = 1;
   return 0;
   }

sprintf(tmp,"InsPaq%02d=%s",maximo+1,clave);
reemplaza_punto(carga,lineas,maxi,&(_act_instalacion[maximo]),tmp);

return(1);
}


static int actualiza_este_ini(char *nuevo)
{
char *punt;
char nom[128],tmp[512];
char **carga,**cargamas;
int i,j,lineas,maxi,lineasmas;
FILE *file,*filenew;
char *_act_key = NULL;
char *_act_red = NULL;
char *_act_instalacion[52] = {NULL,NULL,NULL,NULL,NULL,NULL,};
char *_codigo_de_sistema1 = NULL;
char *_codigo_de_sistema2 = NULL;
char *_codigo_de_sistema3 = NULL;
char *_codigo_de_sistema4 = NULL;
char *_act_servidor = NULL;
char *__act_key = NULL;
char *__act_red = NULL;
char *__act_instalacion[52] = {NULL,NULL,NULL,NULL,NULL,NULL,};
char *__codigo_de_sistema1 = NULL;
char *__codigo_de_sistema2 = NULL;
char *__codigo_de_sistema3 = NULL;
char *__codigo_de_sistema4 = NULL;
char *__act_servidor = NULL;
int buena_act = 0;
long numero_diagram = 0L;
long otro_numero;
int ttcc;
unsigned short posicion = 0;
unsigned short otra_posicion;
unsigned short nume_code;

for (i = 0;i < 52;i++)
  {
  _act_instalacion[i] = NULL;
  __act_instalacion[i] = NULL;
  }

punt = getenv("DSINI");
if (punt && strlen(punt))
  strcpy(nom,punt);
else
  {
  strcpy(nom,rw_directorio("ds.ini"));
  }

file = fopen(nom,"rt");
if (!file)
   {
   rename(rw_directorio("agi.ini"),nom);
   file = fopen(nom,"rt");
   }

if (!file) 
   {
   return(copiarfich(nuevo,nom,1));
   }     
filenew = fopen(nuevo,"rt");
if (!filenew)
   {
   fclose(file);
   return(0);
   /*return(copiarfich(nuevo,nom,1));*/
   }

maxi = 200;
cargamas = (char **)malloc(sizeof(char *) * maxi);
lineasmas = 0;
while(fgets(tmp,250,filenew))
   {
   if (lineasmas >= maxi)
      {
	  maxi += 200;
	  cargamas = realloc(cargamas,sizeof(char *) * maxi);
	  }   
   quitan(tmp);
   quitab(tmp);
   quitabp(tmp);
   cargamas[lineasmas] = malloc(strlen(tmp)+1);
   strcpy(cargamas[lineasmas],tmp);
   lineasmas++;

   if (analiza_esta_linea(tmp) < 1) continue;

   if (!strcmp(tmp,"Activacion"))
      {
	  __act_key = cargamas[lineasmas-1];
	  }
   else
   if (!strcmp(tmp,"A_Red"))
      {
	  __act_red = cargamas[lineasmas-1];
	  }
   else
   if (!strcmp(tmp,"Sistema1"))
      {
	  __codigo_de_sistema1 = cargamas[lineasmas-1];
	  }
   else
   if (!strcmp(tmp,"Sistema2"))
      {
	  __codigo_de_sistema2 = cargamas[lineasmas-1];
	  }
   else
   if (!strcmp(tmp,"Sistema3"))
      {
	  __codigo_de_sistema3 = cargamas[lineasmas-1];
	  }
   else
   if (!strcmp(tmp,"Sistema4"))
      {
	  __codigo_de_sistema4 = cargamas[lineasmas-1];
	  }
   else
   if (!strcmp(tmp,"Servidor"))
      {
	  __act_servidor = cargamas[lineasmas-1];
	  }
   else
   if (!memcmp(tmp,"InsPaq",6))
      {
	  i = atoi(tmp+6);
	  if (i > 0 && i < 51)
	     {
		 __act_instalacion[i-1] = cargamas[lineasmas-1];
		 }
	  }
   else
   if (!strcmp(tmp,"PaqDesde"))
      {
	  __act_instalacion[50] = cargamas[lineasmas-1];
	  }
   else
   if (!strcmp(tmp,"PaqHasta"))
      {
	  __act_instalacion[51] = cargamas[lineasmas-1];
	  }
   }
fclose(filenew);  


maxi = 200; /* se ha de quedar con el maximo de carga ya que puede ser realocatado */
carga = (char **)malloc(sizeof(char *) * maxi);
lineas = 0;
while(fgets(tmp,250,file))
   {
   if (lineas >= maxi)
      {
	  maxi += 200;
	  carga = realloc(carga,sizeof(char *) * maxi);
	  }   
   quitan(tmp);
   quitab(tmp);
   quitabp(tmp);
   carga[lineas] = malloc(strlen(tmp)+1);
   strcpy(carga[lineas],tmp);
   lineas++;

   
   if (analiza_esta_linea(tmp) < 1) continue;

   if (!strcmp(tmp,"Activacion"))
      {
	  _act_key = carga[lineas-1];
	  }
   else
   if (!strcmp(tmp,"A_Red"))
      {
	  _act_red = carga[lineas-1];
	  }
   else
   if (!strcmp(tmp,"Sistema1"))
      {
	  _codigo_de_sistema1 = carga[lineas-1];
	  }
   else
   if (!strcmp(tmp,"Sistema2"))
      {
	  _codigo_de_sistema2 = carga[lineas-1];
	  }
   else
   if (!strcmp(tmp,"Sistema3"))
      {
	  _codigo_de_sistema3 = carga[lineas-1];
	  }
   else
   if (!strcmp(tmp,"Sistema4"))
      {
	  _codigo_de_sistema4 = carga[lineas-1];
	  }
   else
   if (!strcmp(tmp,"Servidor"))
      {
	  _act_servidor = carga[lineas-1];
	  }
   else
   if (!memcmp(tmp,"InsPaq",6))
      {
	  i = atoi(tmp+6);
	  if (i > 0 && i < 51)
	     {
		 _act_instalacion[i-1] = carga[lineas-1];
		 }
	  }
   else
   if (!strcmp(tmp,"PaqDesde"))
      {
	  _act_instalacion[50] = carga[lineas-1];
	  }
   else
   if (!strcmp(tmp,"PaqHasta"))
      {
	  _act_instalacion[51] = carga[lineas-1];
	  }   
   }
fclose(file);

/*************************************************************************************************/
/* Una vez analizados los dos ini procedemos a la mezcla caso por caso */

/* Activacion: */

if (!_act_key && __act_key)
   {
   buena_act = 1;
   }
else
if (_act_key && __act_key)
   {
   strcpy(tmp,_act_key);
   i = analiza_esta_linea(tmp);
   quita_blancos(tmp+i);
   if (i < 1 || !strcmp(tmp+i,"DEMO"))
      {
	  buena_act = 1;
	  }
   else
      {
      /* comprobar numero de agisys */
      numero_diagram = dame_numero_diagram(tmp+i,&posicion,&nume_code);
	  if (numero_diagram < 1L || numero_diagram > 999999L || posicion < 1 || posicion > 1022 || (numero_diagram == 123456L && posicion == 768))
	     {
		 buena_act = 1;
		 }
	  else
	     {
		 ttcc = tipo_clave;
         strcpy(tmp,__act_key);
         i = analiza_esta_linea(tmp);
         quita_blancos(tmp+i);
		 if (i > 0 && strcmp(tmp+i,"DEMO"))
		    {
			otro_numero = dame_numero_diagram(tmp+i,&otra_posicion,&nume_code);
			if (numero_diagram == otro_numero && posicion == otra_posicion && ttcc == tipo_clave)
			   buena_act = 1;
			}
		 }
	  }
   }

if (buena_act)
   {
   strcpy(tmp,__act_key);
   i = analiza_esta_linea(tmp);
   quita_blancos(tmp+i);
   if (i < 1)
     {
	 buena_act = 0;
	 }
   else
     {
	 if (strcmp(tmp+i,"DEMO"))
	   { 
       numero_diagram = dame_numero_diagram(tmp+i,&posicion,&nume_code);
	   if (numero_diagram < 1L || numero_diagram > 999999L || posicion < 1 || posicion > 1022)
	      buena_act = 0;
	   }
	 else
	   {
	   if (_act_key)
	      buena_act = 0;
	   }
	 }
   }

if (!__act_key || buena_act) /* solo continuamos si la activacion es buena o solo van paquetes */
   {
   if (buena_act)
      {
	  reemplaza_punto(&carga,&lineas,&maxi,&_act_key,__act_key);
	  }

   /* clave de red */
   if (__act_red && __act_key && es_buena_red(__act_red,posicion))
      {
      reemplaza_punto(&carga,&lineas,&maxi,&_act_red,__act_red);
	  }

   /* codigos sistema y servidor ... no chequeados!!! */
   if (__codigo_de_sistema1 && __act_key)
      {
      reemplaza_punto(&carga,&lineas,&maxi,&_codigo_de_sistema1,__codigo_de_sistema1);
	  }
   if (__codigo_de_sistema2 && __act_key)
      {
      reemplaza_punto(&carga,&lineas,&maxi,&_codigo_de_sistema2,__codigo_de_sistema2);
	  }
   if (__codigo_de_sistema3 && __act_key)
      {
      reemplaza_punto(&carga,&lineas,&maxi,&_codigo_de_sistema3,__codigo_de_sistema3);
	  }
   if (__codigo_de_sistema4 && __act_key)
      {
      reemplaza_punto(&carga,&lineas,&maxi,&_codigo_de_sistema4,__codigo_de_sistema4);
	  }

   if (__act_servidor && __act_key)
      {
      reemplaza_punto(&carga,&lineas,&maxi,&_act_servidor,__act_servidor);
	  }
   for (j = 0;j < 50;j++)
      {
      if (__act_instalacion[j])
	     {
		 integra_paquete(&carga,&lineas,&maxi,__act_instalacion[j],_act_instalacion,nume_code,numero_diagram);
		 }
	  }
   if (__act_instalacion[50])
      {
	  reemplaza_punto(&carga,&lineas,&maxi,&(_act_instalacion[50]),__act_instalacion[50]);
	  }
   if (__act_instalacion[51])
      {
	  reemplaza_punto(&carga,&lineas,&maxi,&(_act_instalacion[51]),__act_instalacion[51]);
	  }
   }

/*************************************************************************************************/

file = fopen(nom,"wt");
if (!file)
   {
   return(copiarfich(nuevo,nom,1));
   }
for (i = 0;i < lineas;i++)
   {
   fprintf(file,"%s\n",carga[i]);
   free(carga[i]);
   }
fclose(file);
free(carga);
for (i = 0;i < lineasmas;i++)
   {
   free(cargamas[i]);
   }
free(cargamas);
return(1);
}

extern int (*Eliminador_Diagram)(char *);

#ifdef FGVDOS
LPGetTaskList     GetTaskList = NULL;

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


extern int instalacion_en_grafico;


int PASCAL WinMain(HANDLE hInstance,HANDLE hPrevInstance,
						 LPSTR lpCmdline,int nCmdShow)
{
  char mandato[1024];
  char *args[100];
  int argc = 0,i;
   
  strcpy(mandato,GetCommandLine());

  args[argc] = mandato;
  argc++;
  for (i = 0;mandato[i];i++)
     {
	 if (mandato[i] == ' ')
	    {
        mandato[i] = 0;
		i++;
		while(mandato[i] == ' ') i++;
		if (mandato[i])
		   {
		   args[argc] = mandato + i;
		   argc++;
		   }
		}
	 }
  return(main(argc,args));

}

main(argc,argv)
int argc;
char **argv;
{
char tmp[256];
char tmp2[256];
char tmp3[1024];
char tmp4[256];
char tmp5[256];
char tmp6[256];
int copiar_elini = 0;
char origen[256];
char origeni[256];
int i,j,ext,secu,err,mi_volume,preverr,puede;
int hay_que_copiar = 0;
struct info_paquete *info;
struct stat buf,buf2;
char _continua[81];
char *continua = NULL;
FILE *file;
char *argg[100];
int noes_volume = 0;
int reinicio = 0;
int abandonar = 0;


#ifdef FGVDOS
{
OSVERSIONINFO     verInfo = {0};

/*
if (argc > 1)
  {
  if (argv[1][0] == '@' && argv[1][1] == '!')
     FreeConsole();
  }
*/

verInfo.dwOSVersionInfoSize = sizeof (verInfo);     
GetVersionEx(&verInfo); 
switch (verInfo.dwPlatformId)     
  { 
  case VER_PLATFORM_WIN32_NT: 
              GetTaskList = GetTaskListNT;
              break;  
  case VER_PLATFORM_WIN32_WINDOWS:        
		      GetTaskList = GetTaskList95; 
              break;      
  }

Eliminador_Diagram = MiKill;
}

instalacion_en_grafico = 1;

AllocConsole();
#endif

menusep(&argc,argv);

Terinici();
ini_sig();
cls();
sprintf(tmp,"INSTALACIONES %d.%02d.%03d",rtme_version1,rtme_version2,(int)rtme_versionletra);
mensaje(0,tmp);
_nvolume = 0;

if (argc < 2)
  {  
	if (salvapar != (char *)0)
	  {
	  aviso(0,"PARAMETROS INTERNOS INCORRECTOS!!");
	  abandonar = 1;
	  goto insagifinal;
	  }
	/* lectura del disk id o del directorio para seleccion */	
  }

if (salvapar == (char *)0) 
   {
   instalacion_silenciosa = 0;
   continua = _continua;
   strcpy(origen,argv[0]);
#ifdef FGVDOS
#ifdef _DEBUG
   strcpy(origen,"instalar.exe");
#endif
#endif
   if (origen == quita_path(origen))
     {
	 strcpy(origen,".");
     }
   else
     {
	 *quita_path(origen) = 0;
     i = strlen(origen);
	 if (i)
	   {
	   i--;
	   if (origen[i] == '/' || origen[i] == '\\')
		   origen[i] = 0;
	   }
     }
   strcat(origen,"/");
   strminus(origen);
   cambia_barra(origen);
   if (!strcmp(origen,"./"))
      {
	  /* coger el directorio por defecto */
	  getcwd(origen,sizeof(origen));	  
      i = strlen(origen);
	  if (i)
	    {
	    i--;
	    if (origen[i] != '/' && origen[i] != '\\')
		   strcat(origen,"/");
	    }
	  cambia_barra(origen);
      }
#ifdef FGVDOS
   strcpy(tmp,origen);
   _fullpath(origen,tmp,sizeof(origen));
   i = strlen(origen);
   if (i)
	  {
	  i--;
	  if (origen[i] != '/' && origen[i] != '\\')
	    strcat(origen,"/");
	  }
	cambia_barra(origen);
	strminus(origen);
#endif
	strcpy(origeni,origen);

   _nvolume = 0;
   if (argc == 1)
     {
     if (!strcmp(server_request,"@(C:?)M"))
	    {
		do {
	       pprint(2401,"Entre direccion de red del servidor:");
		   server_request[4] = 0;
           i = entra(2438,32,"","",server_request+4,1,server_request+4);
           if (i == 1 || i == 7)
           {
           if (confirmar(2440," Desea Efectuar la Instalacion (S/N)? ","S"))
		      {
			  abandonar = 1;
		      goto insagifinal;
		      }
           }
		   quitab(server_request+4);
		   if (!strlen(server_request+4))
		      {
			  continue;
			  }
           strcat(server_request+4,")M");
		   if (!i) break;
		   } while(-1);
		Bl(24);
        }
     sprintf(tmp3,"%svolume.ds",origen);
     file = fopen(tmp3,"rt");
	 if (!file)
	    {		
        sprintf(tmp3,"%svolume.agi",origen);
        file = fopen(tmp3,"rt");
		}
	 if (!file)
	    {		
		noes_volume = 1;
		 /* o presentar el directorio o salir */
		if (server_request[0] == '@' && (server_request[2] == 'C' || server_request[2] == '/'))
		  { 
		  strcpy(origen,server_request);
		  }
		else
		  {
		  origen[0] = 0;
		  mensaje(0,"ESPECIFIQUE UNIDAD o ENTRADA EN BLANCO,CON @direccion OTRO SERVIDOR");
          do {
		   while(_nvolume)
             {
             _nvolume--;
             free(_volume[_nvolume]);
             }

	       pprint(2401,"Entre Origen de la Instalacion:");
           i = entra(2433,32,"","",origen,1,origen);
           if (i == 1 || i == 7)
           {
           if (confirmar(2440," Desea Efectuar la Instalacion (S/N)? ","S"))
		     {
			 abandonar = 1;
		     goto insagifinal;
		     }
            }

	       if (i || correcto()) continue;

           quita_blancos(origen);
		   strminus(origen);

		   j = 0;
           for (i = 0;origen[i];i++)
			  {
			  if (origen[i] == ',')
			    {
				origen[i] = 0;
				if (j)
				  {
				  _volume[_nvolume] = malloc(strlen(origen+j)+1);
				  strcpy(_volume[_nvolume],origen+j);
				  _nvolume++;
				  }				
				j = i + 1;
			    }
			  }

		   if (j)
			  {
			  _volume[_nvolume] = malloc(strlen(origen+j)+1);
			  strcpy(_volume[_nvolume],origen+j);
			  _nvolume++;
			  }

		   if (!origen[0])
		    {
		    /* coger del servidor */
			strcpy(origen,"@(C:");
		    dame_ser_por_defecto(origen+strlen(origen),sizeof(origen)-strlen(origen));
		    strcat(origen,")L");
			strcpy(server_request,origen);
			break;
		    }
		   else
		    {
		    if (!strcmp(origen,"ds"))
		     {
			 strcpy(origen,"@(C:www.diagram.es)M");
			 strcpy(server_request,origen);
			 break;
		     }
		    else
		    if (origen[0] == '@')
		     {			 
			 if (origen[1] != '(')
			   {			   
			   strcpy(tmp3,origen+1);
			   strcpy(origen,"@(C:");
			   strcat(origen,tmp3);
			   strcat(origen,")M");			   
			   }
			 strcpy(server_request,origen);
			 break;
		     }
		    else
		     {
			 i = strlen(origen);
			 i--;
			 if (origen[i] != ':' && origen[i] != '/' && origen[i] != '\\')
			   {
               strcat(origen,"/");
			   }  
			 cambia_barra(origen);
			 if (!_nvolume)
			   {
			   sprintf(tmp3,"%svolume.ds",origen);
			   file = fopen(tmp3,"rt");
			   if (!file)
			      {
			      sprintf(tmp3,"%svolume.agi",origen);
			      file = fopen(tmp3,"rt");
				  }
			   if (file)
			     {
		         while(_nvolume)
                   {
                   _nvolume--;
                   free(_volume[_nvolume]);
                   }
				 noes_volume = 0;
				 break;
			     }
			   }
			 else
			   {
			   break;
			   }
		     }
		    }		   
		   } while(-1);
		  }
	    }
	   if (file)
	    {
	    tmp3[0] = 0;	 
	    while(lee_linea(tmp3,250,file))
	     {
		 for (i = 0;tmp3[i] && tmp3[i] != ';';i++); /* de los comentarios hacer el menu de seleccion */
		 tmp3[i] = 0;
         quita_blancos(tmp3);
		 if (strlen(tmp3))
		   {
		   for (i = 0;tmp3[i] && tmp3[i] != '.';i++);
		   if (!strcmp(tmp3+i,".ini"))
		      {
			  sprintf(tmp5,"%s%s",origen,tmp3);
			  copiar_elini = 1;
			  continue;
		      }
		   _volume[_nvolume] = malloc(strlen(tmp3)+1);
		   strcpy(_volume[_nvolume],tmp3);
		   _nvolume++;
		   }
	     }
	    fclose(file);
	    if (!_nvolume)
	     {
         aviso(0,"Fichero de secuencia 'volume' incorrecto!!");
		 abandonar = 1;
	     goto insagifinal;			 
	     }
	    }

	   if (origen[0] == '@')
	     {
		 /* conectar !!! */           		 
		 if (!_nvolume)
		   {		   		   
		   if (QUE_SISTEMA_SOY == 11 || QUE_SISTEMA_SOY == 13)
		      {
			  _nvolume = 2;
			  _volume[0] = malloc(15);
			  strcpy(_volume[0],"agisysnt");
			  _volume[1] = malloc(15);
			  strcpy(_volume[1],"agiwinnt");
		      }
		   else
		   if (QUE_SISTEMA_SOY == 7)
		      {
			  _nvolume = 1;
			  _volume[0] = malloc(15);
			  strcpy(_volume[0],"agisysbx");
		      }
		   else
		   if (QUE_SISTEMA_SOY == 23)
		      {
			  _nvolume = 1;
			  _volume[0] = malloc(15);
			  strcpy(_volume[0],"agisyssx");
		      }
		   _volume[_nvolume] = malloc(15);
		   strcpy(_volume[_nvolume],"agitool");
		   _nvolume++;
		   if (QUE_SISTEMA_SOY == 11 || QUE_SISTEMA_SOY == 13)
		      {
		      _volume[_nvolume] = malloc(15);
		      strcpy(_volume[_nvolume],"dsimpre");
		      _nvolume++;
			  }
           instalacion_silenciosa = 1;		   
		   }
		 hay_que_copiar = 0;
	     }
	 }
   else
	 _nvolume = 0;


   /* presentar menu de seleccion y guardar claves .... en caso de varios paquetes */

   if (segundo_dir[0])
     {
	 if (segundo_dir[2] == '!')
	   {
       sin_iconos = 1;
	   strcpy(tmp,segundo_dir+3);
	   }
	 else
	   strcpy(tmp,segundo_dir+2);
	 instalacion_silenciosa = 1;
     }
   else
     {
     strcpy(tmp,rw_directorio(""));   
     i = strlen(tmp);
     if (i)
       {
	   i--;
	   if (tmp[i] == '/' || tmp[i] == '\\')
	     {
	     tmp[i] = 0;
	     }
	   }
    Bl(24);
    do {
	  pprint(2401,"Entre Directorio Destino:");
      i = entra(2427,32,"","",tmp,1,tmp);
      if (i == 1 || i == 7)
        {
        if (confirmar(2440," Desea Efectuar la Instalacion (S/N)? ","S"))
		  {
		  abandonar = 1;
		  goto insagifinal;
		  }
        }
	  if (!i && !correcto()) break;
	  } while(-1);
    }

   quita_blancos(tmp);
   i = strlen(tmp);
   if (i)
     {
	 i--;
	 if (tmp[i] == '/' || tmp[i] == '\\')
	   {
	   tmp[i] = 0;
	   }
	 }   

   fin_sig();
   Terfinal();

   rw_setdirectorio(tmp);
   strcpy(tmp,rw_directorio("master/"));
   crea_dir(tmp);   

   if (copiar_elini)
      {
	  actualiza_este_ini(tmp5);
	  copiar_elini = 0;
      }  

   strcpy(tmp5,rw_directorio("dev/ds.reg"));	  
   rename(rw_directorio("dev/agi.reg"),tmp5);  

   strcpy(tmp5,rw_directorio("dev/usr/ds_sys.psw"));	  
   rename(rw_directorio("dev/usr/agisys.psw"),tmp);

   Terinici();
   ini_sig();
#ifdef FGVDOS
   strcpy(tmp2,tmp);
   _fullpath(tmp,tmp2,sizeof(tmp));
   i = strlen(tmp);
   if (i)
	  {
	  i--;
	  if (tmp[i] != '/' && tmp[i] != '\\')
	    strcat(tmp,"/");
	  }
#endif
   strminus(tmp);
   cambia_barra(tmp);
   if (origen[0] != '@' && strcmp(tmp,origen))
     {
	 hay_que_copiar = 1;
     }

   if (strcmp(tmp,origeni))
     {
     sprintf(tmp2,"%sinstalar",tmp);
	 /*sprintf(tmp2,"%s%s",tmp,quita_path(argv[0]));*/

 	 strcpy(tmp,argv[0]);
	 strminus(tmp);
	 strminus(tmp2);
#ifdef FGVDOS
     if (strbusca(tmp,".exe") == -1) 
	 	   strcat(tmp,".exe");
	 if (strbusca(tmp2,".exe") == -1) 
	 	strcat(tmp2,".exe");
#endif
	 /* copiarme a mi mismo si el destino es de diferente tamaño o en caso de duda */
	 /*if (stat(tmp,&buf) == -1 || stat(tmp2,&buf2) == -1 || buf.st_size != buf2.st_size)
	    { copiar siempre */
		if (instalacion_silenciosa < 2)
           mensaje(0,"Preparando Instalacion ...");
        copiarfich(tmp,tmp2,0);
		if (stat(tmp,&buf2) != -1)
		   chmod(tmp2,buf2.st_mode);
	 /*   }*/
     }

   if (_nvolume)
     {
     argc = 1 + _nvolume;
	 argg[0] = argv[0];
     for (i = 1;i < argc;i++)
	   {
	   argg[i] = _volume[i-1];
	   }
	 argv = argg;
     }
   }
else
   {
   strcpy(origen,rw_directorio("master/"));
   if (server_request[0] == '@' && (server_request[2] == 'S' || server_request[2] == '-'))
      instalacion_silenciosa = 2;
   else
      instalacion_silenciosa = 1;
   }
mi_volume = 1;
preverr = 0;

strcpy(nmen0,rw_directorio("ds"));

puede = carga_master(0);
if (Eliminador_Diagram && puede < 1)
   {
   if (puede >= 0)
        descarga_master();   
   if (instalacion_silenciosa < 2)
      {
	  mensaje(0,"HAY PROGRAMAS EJECUTANDOSE!!,CON 'S' SE ANULARAN, MEJOR LO HAGA MANUALMENTE");
	  if (!instalacion_silenciosa)
	     {
         if (confirmar(2400," Desea Continuar la Instalacion (S/N)? ","N"))
		   {
           puede = 1;
		   }	  
		 else
		   {
		   pito(90);pito(91);pito(92);pito(93);
           if (confirmar(2400," Es mejor contestar N y reiniciar el sistema, Continuar (S/N)? ","N"))
		     {
             puede = 1;
		     }	  
		   }
		 }
	  else
      if (confirmar(2440," Desea Continuar la Instalacion (S/N)? ","S"))
		 {
         puede = 1;
		 }	  
	  }
   if (puede < 1)
      {
      (*Eliminador_Diagram)("agisysnt.exe");
	  (*Eliminador_Diagram)("agilisnt.exe");
	  (*Eliminador_Diagram)("agiwinnt.exe");
	  (*Eliminador_Diagram)("agilisnw.exe");
	  (*Eliminador_Diagram)("diagram9.exe");
	  (*Eliminador_Diagram)("serverds.exe");
      }
   sleep(1);
   puede = carga_master(0);
   if (puede < 1)
      {
      if (instalacion_silenciosa < 2)
	     {
		 aviso(0,"No se pueden anular los otros programas!");
		 }
	  }
   }

if (puede < 0 || (!puede && !hay_que_copiar && origen[0] != '@'))
   {
   aviso(0,"ERROR: No se puede instalar si alguien esta ejecutando un programa.");
   abandonar = 1;
   goto insagifinal;
   }
if (!puede)
   {
   aviso(0,"ATENCION: Solo se copiaran los instalables! (Hay programas ejecutandose!)");
   /*
   if (confirmar(2440," Desea Continuar la Instalacion (S/N)? ","S"))
      {
	  abandonar = 1;
      goto insagifinal;
      }
   */
   }

if (origen[0] == '@')
  {
  strcpy(origen,rw_directorio("master/"));
  if (establece_conexion(0,argc-1,argv+1) < 0)
     {
	 aviso(0,"NO SE PUEDEN OBTENER LOS PAQUETES NECESARIOS"); 
	 abandonar = 1;
	 goto insagifinal;
     }
  }

for (i = 1;i < argc;i++)
   {
   /* copiar paquetes e instalar */
   strminus(argv[i]);   
   ext = strbusca(quita_path(argv[i]),".");
   secu = 1;
   if (hay_que_copiar || ext != -1)
      {
	  if (instalacion_silenciosa < 2)
	    {
		Ap();
	    mensaje(0,"Preparando Instalacion ... Espere!");
		Cp();
	    }
	  do 
	    {
  	    sprintf(tmp,"%s%s",origen,quita_path(argv[i]));
	    quita_blancos(tmp);
	    if (ext != -1)
	     {
		 if (atoi(quita_path(argv[i])+ext+1) != secu)
		   {
		   sprintf(tmp,"Fuera de orden %s (%d),posible error en 'volume'!",quita_path(argv[i]),secu);
		   aviso(0,tmp);
		   continue;
		   }
	     }
	    else
	     {
	     strcat(tmp,".tgz");
	     }	  
		if (ext != -1)
		  {
		  *(quita_path(argv[i])+ext) = 0;
		  }
        sprintf(tmp2,"%s%s",rw_directorio("master/"),quita_path(argv[i]));
	    quita_blancos(tmp2);
		if (ext != -1)
		  {
		  *(quita_path(argv[i])+ext) = '.';
		  }
	    strcat(tmp2,".tgz");
	    if (secu == 1)
	     {
#ifdef FGVDOS
         err = copiafich_win(tmp,tmp2);
#else
         err = copiarfich(tmp,tmp2,0);
#endif
	     }
	    else
	     {
	 	 err = copiarfich(tmp,tmp2,1);
	     }
	    if (err)
	     {
		 if (!_nvolume)
		   {
	       sprintf(tmp3,"No se puede copiar el fichero %s ...",quita_path(argv[i]));
		   if (instalacion_silenciosa < 2)
	         mensaje(0,tmp);
		   mi_volume = 0;
		   break;
		   }

		 if (!preverr)
		   {
		   mi_volume++;		   
           sprintf(tmp3,"Inserte DISCO o SOPORTE Numero %d u otro que contenga %s",mi_volume,tmp);
		   tmp3[80] = 0;
		   mensaje(0,tmp3);pito(94);
		   }
		   /* posibilidad de cambio */
		  strcpy(tmp3,origen);
           do {
	        pprint(2401,"Pulse INTRO o entre nuevo origen:");
            j = entra(2435,32,"","",tmp3,1,tmp3);
            if (j == 1 || j == 7)
             {
             if (confirmar(2440," Desea Continuar la Instalacion (S/N)? ","S"))
		       {
			   abandonar = 1;
		       goto insagifinal;
		       }
             }
			quitab(tmp3);
	        if (strlen(tmp3) && !j && !correcto()) break;			
			if (!tmp3[0])
			   {
			   strcpy(tmp3,origen);
			   }
	        } while(-1);
		   cambia_barra(tmp3);
		   j = strlen(tmp3);
		   j--;		   
		   if (tmp3[j] != ':' && tmp3[j] != '/')
		      {
			  strcat(tmp3,"/");
		      }
		   strcpy(origen,tmp3);
	       if (instalacion_silenciosa < 2)
		     {
			 Ap();
	         mensaje(0,"Preparando Instalacion ... Espere!");
			 Cp();
		     }		   
		 preverr = err;
		 continue;
	     }
        preverr = 0;
		if (ext != -1)
		   {		   
		   if (i == (argc-1))
		     {
			 break;
		     }		   
		   secu++;
		   ext = strbusca(quita_path(argv[i+1]),".");
		   if (ext == -1 || atoi(quita_path(argv[i+1])+ext+1) == 1)
			 {
			 break;
		     }
		   i++;
		   }
		} while(ext != -1 || err);
      }

Sin_COPIAR:

  Bl(4);
  ext = strbusca(quita_path(argv[i]),".");
  if (ext != -1)
	  {
	  *(quita_path(argv[i])+ext) = 0;
	  }
   strcpy(tmp,argv[i]);
  if (ext != -1)
	  {
	  *(quita_path(argv[i])+ext) = '.';
	  }
   quita_blancos(tmp);
   if (continua)
      continua[0] = 0;
   if (puede && (strlen(tmp) < 8 || memcmp(tmp,"instalar",8)))
	  {
      info = instala_paquete(tmp,continua);
	  if (info && ultima_instalacion == 2)
	     {
		 if ((info->tipo == 'R' || info->tipo == 'W' || info->tipo == 'C' || info->tipo == 'S') && !strcmp(NAME_SYSTEM,info->sistema))
		   {
		   aviso(0,"SOLO SE COPIO EL INSTALABLE! (Debe Reinicilizar el Servidor)");
		   } 
		 else
		   {
		   if (!strcmp(NAME_SYSTEM,info->sistema) || !strcmp(info->sistema,"X"))
		      aviso(0,"NO SE INSTALA, SOLO COPIADO!");
		   }
	     }
	  }
   else
      {
	  info = datos_paquete(tmp,continua);
	  }
   if (info)
     {
     for (j = 0;j < nPaq;j++)
       {
	   if (!strcmp(Paq[j].nompaq,tmp))
	      {
		  break;
	      } 
       }
	 if (j < nPaq)
	   {
	   if (Paq[j].control_utime && Paq[j].i.version1 == info->version1 && Paq[j].i.version2 == info->version2 && 
				Paq[j].i.versionletra == info->versionletra)
		  {
		  info->autoversion = Paq[j].i.autoversion+1;
		  }
	   }
	 else
	   {
	   strcpy(Paq[j].nompaq,tmp);
	   nPaq++;
	   }
     memcpy(&(Paq[j].i),info,sizeof(Paq[j].i));
	 strcpy(tmp,rw_directorio("master/"));
	 strcat(tmp,Paq[j].nompaq);
	 strcat(tmp,".tgz");	
     if (ultima_instalacion == 1 && (stat(tmp,&buf) != -1 || (info->build_numero) ))
	   {
	   if (!(info->build_numero))
          Paq[j].control_utime = buf.st_mtime;
	   else
	      {
	      Paq[j].control_utime = info->build_numero;
		  pon_la_marca(tmp,info->build_numero);
		  }
	   Paq[j].flag = 3;
	   if (Paq[j].i.tipo == 'R' || Paq[j].i.tipo == 'W' || Paq[j].i.tipo == 'S' || Paq[j].i.tipo == 'C')
	     {
	     reinicio = 1;
	     }
	   }
	 else
	   {
	   Paq[j].control_utime = 0L;
	   Paq[j].flag = 2;	
	   }	 
     }
   else
     {
	 if (instalacion_silenciosa < 2)
	   {       
	   sprintf(tmp,"No se Instalo %s, No instale desde 'master'",quita_path(argv[i]));
	   aviso(0,tmp);
	   break;
	   }
	  /* quitar el paquete del master por si acaso !!! */
     sprintf(tmp2,"%s%s",rw_directorio("master/"),quita_path(argv[i]));
	 quita_blancos(tmp2);
	 strcat(tmp2,".tgz");
     unlink(tmp2);	  
     }
   if (continua && strlen(continua))
     {	 
	 if (i >= (argc-1) || strcmp(argv[i+1],continua))
	  {
	  if (!noes_volume && _nvolume)
	   {
	   if (i >= (argc-1) || memcmp(continua,quita_path(argv[i+1]),strlen(continua)))
	     {
		 struct stat bufxx;
         sprintf(tmp4,"%s%s",rw_directorio("master/"),quita_path(continua));  
	     quita_blancos(tmp4);
	     strcat(tmp4,".tgz");
         if (stat(tmp4,&bufxx) >= 0)
		   {		   
	       strcpy(tmp4,continua);
	       argv[i] = tmp4;
		   secu = 1;
		   strminus(argv[i]);
		   err = 0;
		   preverr = 0;
           goto Sin_COPIAR;
		   }
		 else
		   {
           int jjkk;
		   for (jjkk = 0;jjkk < i;jjkk++) 
		     {
             if (!strcmp(continua,quita_path(argv[jjkk])))
			    {
				break;
				}
			 }
		   if (jjkk < i)
		     {
	         strcpy(tmp4,continua);
	         argv[i] = tmp4;
		     secu = 1;
		     strminus(argv[i]);			 
		     err = 0;
             preverr = 0;
			 i--;
             continue;
			 }
		   }
		 aviso(0,"'VOLUME' FUERA DE SECUENCIA");
	     }
	   }
	  else
	   {
	   strcpy(tmp4,continua);
	   argv[i] = tmp4;
	   i--;
	   }
	  }
     }
   }
descarga_master();


insagifinal:

while(_nvolume)
  {
  _nvolume--;
  free(_volume[_nvolume]);
  }

cls();
fin_sig();
Terfinal();

if (!abandonar)
  {
  /*rw_exec(0);*/
  }

if (reinicio)
  {
  printf("POR FAVOR, REINICIALICE EL SISTEMA\r\n");
  }

return(0);
}
