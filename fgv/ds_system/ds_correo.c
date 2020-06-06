
#include <fgvsys.h>


#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>


#ifdef FGVDOS

#define CONSTDARG
#include <stdarg.h>
/*
#ifdef va_start
#undef va_start
#endif

#ifdef va_end
#undef va_end
#endif

#include <varargs.h>
*/

#include <mapi.h>

#include <io.h>
#include <process.h>
#include <windows.h>            
#include <windowsx.h>

#include <winsock.h>
#include <nspapi.h>
#include <sys/utime.h>

#define NTFACTOR 1000

#else

#define CONSTDARG
#include <stdarg.h>
/*
#include <varargs.h>
/*#include <stdarg.h>*/

#define NTFACTOR 1

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

#define IDMENSAJE 0x23169134

extern char *quita_path();

int total_fichero = 0;
int donde_fichero = 0;
char path_tmp[2048];
char fich_tmp[2048];

extern char el_sujeto[2048];

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

union fgv_in_addr
        {
                struct { u_char s_b1,s_b2,s_b3,s_b4; } S_un_b;
                struct { u_short s_w1,s_w2; } S_un_w;
                u_long S_addr;
        };


BOOL ReportStatusToSCMgr(DWORD dwCurrentState,
                         DWORD dwWin32ExitCode,
                         DWORD dwWaitHint);



typedef struct MAIL_MENSAJE_
{
    unsigned long id;
    char from[250];
    char to[250];
    char subject[250];
    char message[250];
    char files[252];
} MailMensaje;



#ifdef FGVDOS

ULONG (PASCAL *pfnSendMail)(ULONG, ULONG, MapiMessage*, FLAGS, ULONG);

ULONG (PASCAL *pfnLogon)(ULONG ulUIParam,LPTSTR lpszProfileName,LPTSTR lpszPassword,FLAGS flFlags,ULONG ulReserved,LPLHANDLE lplhSession);

ULONG (PASCAL *pfnFindNext)(LHANDLE lhSession,ULONG ulUIParam,LPTSTR lpszMessageType,LPTSTR lpszSeedMessageID,FLAGS flFlags,ULONG ulReserved,LPTSTR lpszMessageID);

ULONG (PASCAL *pfnReadMail)(LHANDLE lhSession,ULONG ulUIParam,LPTSTR lpszMessageID,FLAGS flFlags,ULONG ulReserved,lpMapiMessage FAR * lppMessage);
 
ULONG (PASCAL *pfnDeleteMail)(LHANDLE lhSession,ULONG ulUIParam,LPTSTR lpszMessageID,FLAGS flFlags, ULONG ulReserved);

ULONG (PASCAL *pfnLogoff)(LHANDLE lhSession,ULONG ulUIParam,FLAGS flFlags,ULONG ulReserved);
 
ULONG (PASCAL *pfnFreeBuffer)(LPVOID pv);

ULONG (PASCAL *pfnResolveName)(LHANDLE lhSession,ULONG ulUIParam,LPTSTR lpszName,                
                               FLAGS flFlags,                  
                               ULONG ulReserved,               
                               lpMapiRecipDesc FAR * lppRecip);
  

HMODULE l;

LHANDLE lhSession;


BOOL MapiRecibe(char *usuario,char *password,char *path,char *bufr)
{
    MapiMessage   *Msg;
    ULONG r;
    ULONG kk;
    int i,j,l,ll;
    int file;
    int rr;
    int borrarlo = 0;
    char *buf;
    char idmsg[1024];

    lhSession = NULL;
    r = pfnLogon(NULL,(LPTSTR)usuario,(LPTSTR)password,MAPI_FORCE_DOWNLOAD /*| MAPI_NEW_SESSION*/,NULL,&lhSession);
    if (r != SUCCESS_SUCCESS)
    {
        printf("error logon (%s)\n",usuario);
        return FALSE;
    }

    printf("logon ok (%s)\n",usuario);

    r = pfnFindNext(lhSession,NULL,NULL,NULL,MAPI_LONG_MSGID,NULL,idmsg);

    if (r != SUCCESS_SUCCESS) goto salida_r;

    printf("FindNext OK %s\n",idmsg);

    Msg = NULL;

    r = pfnReadMail(lhSession,NULL,idmsg,0,NULL,&Msg);

    if (r != SUCCESS_SUCCESS) goto salida_r;

    printf("Read OK (%d)\n",Msg->nFileCount);

	el_sujeto[0] = 0;
	if (Msg->lpszSubject)
	{
		strcpy(el_sujeto,Msg->lpszSubject);
	}

    rr = 0;
    bufr[0] = 0;
    for (kk= 0;kk < Msg->nFileCount /*&& kk < 1*/ && !rr;kk++)
    {        
        printf("fichero recibido ->%s\n",Msg->lpFiles[kk].lpszPathName);
        if (bufr[0])
        {
           strcat(bufr,";");
        }
        strcat(bufr,Msg->lpFiles[kk].lpszPathName);
    }

    if (!rr || borrarlo)
    {
        printf("Recibido OK\n");
        pfnDeleteMail(lhSession,NULL,idmsg,NULL,NULL);
    }
    else
    {
        printf("Recibido MAL\n");
        r = !SUCCESS_SUCCESS;
    }

    pfnFreeBuffer((LPVOID)Msg);  

    /**/

salida_r:
    pfnLogoff(lhSession,NULL,NULL,NULL);
    if (r != SUCCESS_SUCCESS)
        return FALSE;
    return TRUE;
}

BOOL MapiCorreo(char *to,char *from,char *subject,char *message,char *files)
{
    char sender[512];    
    ULONG rr;
    MapiMessage   Msg;
    MapiRecipDesc Sender;
    MapiRecipDesc To[20];
    MapiFileDesc  File[20];
    MapiRecipDesc *pds[20];
    MapiRecipDesc *ps;
    /*MapiFileTagExt ftipo;*/
    int i,ok;
    char *p;
    int nFile = 0;
    int nTo   = 0;

    memset(&Msg,0,sizeof(Msg));
    memset(&Sender,0,sizeof(Sender));
    memset(&(To[0]),0,sizeof(To));
    memset(&(File[0]),0,sizeof(File));

    Sender.ulRecipClass = MAPI_ORIG;
    if (!from || !strlen(from))
    {
        from = NULL;
    }
    Sender.lpszName = (LPTSTR)from;
    if (!from)
    {
        Sender.lpszAddress = (LPTSTR)from;
    }
    else
    {
        sprintf(sender,"SMTP:%s",from);
        Sender.lpszAddress = (LPTSTR)sender;
    }
    Sender.ulEIDSize = 0; // !!!!****
    Sender.lpEntryID = NULL;

    printf("Enviando de : %s a: ",sender);

    p = to;
    nTo = 0;
    i = 0;
    ok = 1;
    do
    {        
        if (to[i] == ';' || !to[i])
        {
            if (!to[i] || nTo == 19) ok = 0;
            to[i] = 0;

            pds[nTo] = NULL;

            if (pfnResolveName(NULL,NULL,p,0/*MAPI_NEW_SESSION*/,NULL,&ps) == SUCCESS_SUCCESS)
                pds[nTo] = ps;

            if (pds[nTo])
            {
                To[nTo] = *(pds[nTo]);            
                printf("[%s][%d]\r\n ",To[nTo].lpszAddress,To[nTo].ulEIDSize);

                To[nTo].lpszName = pds[nTo]->lpszName;            
                To[nTo].lpszAddress = (LPTSTR)malloc(strlen(pds[nTo]->lpszAddress)+7);
                sprintf(To[nTo].lpszAddress,"%s",pds[nTo]->lpszAddress);
                printf(" %s ",To[nTo].lpszAddress);
            }
            else
            {
                printf("[No se resuelve la direccion]");
                To[nTo].ulEIDSize = 0; // !!!!****
                To[nTo].lpEntryID = NULL;

                To[nTo].lpszName = (LPTSTR)p;            
                To[nTo].lpszAddress = (LPTSTR)malloc(strlen(p)+8);
                if (*p == '=')
                   sprintf(To[nTo].lpszAddress,"SMTP:%s",p+1);
                else
                   sprintf(To[nTo].lpszAddress,"SMTP:%s",p);
                printf(" %s ",To[nTo].lpszAddress);
            }

            if (!nTo)
                To[nTo].ulRecipClass = MAPI_TO;
            else
                To[nTo].ulRecipClass = MAPI_CC;
            
            nTo++;
            p = to + i + 1;
        }

        i++;
    } while(ok);

    printf(".\r\n");
    

    /*memset(&ftipo,0,sizeof(ftipo));*/

    /*
    ftipo.lpTag = "String (Octet)";
    ftipo.cbTag = strlen(ftipo.lpTag);
    ftipo.lpEncoding = "base64";
    ftipo.cbEncoding = strlen(ftipo.lpEncoding);
    */

    p = files;
    nFile = 0;
    i = 0;
    ok = 1;
    do
    {        
        if (files[i] == ';' || !files[i])
        {
            if (!files[i] || nFile == 19) ok = 0;

            files[i] = 0;

            cambia_barra_dos(p);

            File[nFile].flFlags = 0;
            File[nFile].nPosition = -1;
            File[nFile].lpszPathName = p;
            File[nFile].lpszFileName = quita_path(p);
            File[nFile].lpFileType = NULL /*&ftipo*/;
            printf("Fichero: %s \r\n",p);
            nFile++;
            p = files + i + 1;
        }

        i++;
    } while(ok);


    Msg.lpszSubject = (LPTSTR)subject;
    Msg.lpszNoteText = (LPTSTR)message;
    Msg.lpszMessageType = NULL;
    Msg.lpszDateReceived = NULL; 
    //sprintf(miid,"%ld",time(NULL));
    //Msg.lpszConversationID = (LPTSTR)miid;
    Msg.lpszConversationID = NULL;
    Msg.flFlags = 0;//MAPI_RECEIPT_REQUESTED;     
    Msg.lpOriginator = &Sender;

    Msg.nRecipCount = nTo;
    Msg.lpRecips = &(To[0]);

    Msg.nFileCount = nFile;
    Msg.lpFiles = &(File[0]);

    //ulUIParam

    printf("Subject: %s \r\n",subject);
    
    rr = pfnSendMail(NULL,NULL,&Msg,/*MAPI_NEW_SESSION*/0/*MAPI_SEND_NO_RICH_INFO*/,NULL);

    for (i = 0;i < nTo;i++)
    {
        if (pds[i]) pfnFreeBuffer((LPVOID)pds[i]);
        free(To[i].lpszAddress);
    }

    if (rr != SUCCESS_SUCCESS)
    {
        /*MessageBox(NULL,"No enviado","El mensaje",MB_OK);*/
        printf("Fallo al enviar!\r\n");
        return FALSE;
    }

    printf("Envio OK!\r\n");
    return TRUE;

}

#else

int SystemCorreo(char *to,char *from,char *subject,char *message,char *files)
{
char parametro[2028];
char *p;
    
    sprintf(parametro,"./dsmpack -s \"%s\" -d \"%s\" -f \"%s\" ",subject,message,from);
    for (p = files;*p;p++)
    {
        if (*p == ';') *p = ',';
    }
    strcat(parametro,files);
    strcat(parametro," ");
    for (p = to;*p;p++)
    {
        if (*p == ';') *p = ' ';
    }
    strcat(parametro,to);

    system(parametro);

    return(1);
}

#endif


static BOOL rwi_senddata(SOCKET sock,char  *sendbuf,int totalbytes)
{
			int bytessent;
			int i;			
			


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


int
LeeSLinea(SOCKET csock,char *buf,int maximo)
{
int totalbytesread = 0,bytesread;
       while(totalbytesread < maximo)
		   {
		   if((bytesread = recv(csock,buf,1,0)) == SOCKET_ERROR)
			  {
#ifdef FGVDOS
										  if (WSAGetLastError() != WSAEWOULDBLOCK)
										  {
											  return -1;
										  }
#else
										 if (errno != EWOULDBLOCK)
										 {
													  return -1;
										 }
#endif
			   continue;
		 	   }
            if (bytesread != 1) return -1;
            if (*buf == '\n')
               break;
            if (*buf != '\r')
               buf += bytesread;
			totalbytesread += bytesread;			
			}
*buf = 0;
return(0);
}


int
Pop3_Ok(SOCKET csock)
{
char buf[2048];
int maximo = 1024;
if (LeeSLinea(csock,buf,maximo) < 0) return 0;
if (buf[0] != '+' && buf[1] != 'O' && buf[2] != 'K')
{
    printf("%s\r\n",buf);
    return 0;
}

return 1;
}

int
Pop3_LeeLista(SOCKET csock)
{
char buf[2048];
int maximo = 1024;
int i;
int cual_es = 0;
int total_cual = 0;
int cuanto_es = 0;

    total_fichero = 0;
    do
    {
      if (LeeSLinea(csock,buf,maximo) < 0) return 0;
      i = strlen(buf);
      while(i)
      {
          i--;
          if (buf[i] == ' ')
              buf[i] = 0;
          else
              break;
      }
      if (buf[0] == '.') return total_cual;

      for (i = 0;buf[i] && buf[i] == ' ';i++);
      cual_es = atoi(buf+i);
      for (;buf[i] && buf[i] != ' ';i++);
      for (;buf[i] && buf[i] == ' ';i++);
      cuanto_es = atoi(buf+i);

      if (cual_es == 1 || !total_fichero)
      {
          total_fichero = cuanto_es;
          total_cual = cual_es;
      }      

    } while(-1);

    return total_cual;
}

int
Pop3_TotOk(SOCKET csock)
{
char buf[2048];
int maximo = 1024;

if (LeeSLinea(csock,buf,maximo) < 0) return 0;
if (buf[0] != '+' && buf[1] != 'O' && buf[2] != 'K')
    return -1;
/*
total_fichero = atoi(buf+3);
*/
if (total_fichero) total_fichero += 3;

donde_fichero = 0;
return total_fichero;
}


int
Pop3_Quit(SOCKET csock)
{
if (rwi_senddata(csock,"QUIT\r\n",6) == FALSE) return -1;
if (!Pop3_Ok(csock)) return -1;
return 0;
}

int
Pop3_Auth(SOCKET csock,char *usuario,char *password)
{
char buf[1024];

sprintf(buf,"USER %s\r\n",usuario);
if (rwi_senddata(csock,buf,strlen(buf)) == FALSE) return 0;
/*
#ifdef FGVDOS
  Sleep(1000);
#else
  sleep(1);
#endif
*/
if (!Pop3_Ok(csock)) return 0;
sprintf(buf,"PASS %s\r\n",password);
if (rwi_senddata(csock,buf,strlen(buf)) == FALSE) return 0;
/*
#ifdef FGVDOS
  Sleep(1000);
#else
  sleep(1);
#endif
*/
if (!Pop3_Ok(csock)) return 0;
return 1;
}

int 
Haz_Pop3(char *elservidor,char *usuario,char *password,char *path,char *bufr)
{
SOCKET csock;
struct hostent *mihost;
struct sockaddr_in caddr;
int i;
int rr = -1,borrarlo = 0;
int modo = 0,okk;
/* bajar mensaje a mensaje */
  if (*elservidor == ':')
  {
	  modo = 1;
	  elservidor++;
  }

  if((csock =  socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) == SOCKET_ERROR)
     {    
     return -1;  
     }
  i = 1;
  setsockopt(csock,IPPROTO_TCP,TCP_NODELAY,(char *)&i,sizeof(i));
  i = 120*NTFACTOR;
  setsockopt(csock,SOL_SOCKET,SO_SNDTIMEO,&i,sizeof(i));
  i = 120*NTFACTOR;
  setsockopt(csock,SOL_SOCKET,SO_RCVTIMEO,&i,sizeof(i));


  caddr.sin_addr.s_addr = inet_addr(elservidor);
  if (caddr.sin_addr.s_addr == INADDR_NONE)
  {
    if ( (mihost = gethostbyname(elservidor)) && mihost->h_length == 4 && mihost->h_addr_list[0])
    {	    
       caddr.sin_addr.s_addr = *((unsigned long *)mihost->h_addr_list[0]);
    }
  }
  caddr.sin_family = AF_INET;
  caddr.sin_port = htons(110); /*28160;*/

  i = connect(csock,(struct sockaddr *) &caddr,sizeof (caddr));
  if (i == SOCKET_ERROR) 
  {
      closesocket(csock);
      return -1;
  }
  if (!Pop3_Ok(csock))
  {
      printf("No es servidor pop3\n");
      Pop3_Quit(csock);
      closesocket(csock);
      return -1;
  }

  if (!Pop3_Auth(csock,usuario,password))
  {
      printf("Usuario incorrecto\n");
      Pop3_Quit(csock);
      closesocket(csock);
      return -1;
  }
  printf("Usuario ok\n");

  if (rwi_senddata(csock,"LIST\r\n",6) == FALSE)
  {      
      Pop3_Quit(csock);
      closesocket(csock);
      return -1;
  }
  if (!Pop3_Ok(csock))
  {
      printf("Error al obtener la lista de mensajes\n");
      Pop3_Quit(csock);
      closesocket(csock);
      return -1;
  }

  {
      int el;
      char tmpd[128];

      if (!(el = Pop3_LeeLista(csock)))
      {
          printf("No hay mensajes\n");
          Pop3_Quit(csock);
          closesocket(csock);
          return 1; /* no hay mensajes */
      }

      sprintf(tmpd,"RETR %d\r\n",el);
      if (rwi_senddata(csock,tmpd,strlen(tmpd)) == FALSE)
      {      
          Pop3_Quit(csock);
          closesocket(csock);
          return -1;
      }
  }

  if (Pop3_TotOk(csock) <0)
  {
      printf("No hay mensajes\n");
      Pop3_Quit(csock);
      closesocket(csock);
      return 1; /* no hay mensajes */
  }

  printf("hay mensaje\n");


  strcpy(path_tmp,path);
  fich_tmp[0] = 0;
  okk = 1;
  if (modo)
  {
	  char buf[2048];
      int maximo = 1024;
	  FILE *ff;
	  char *r;

	  
	  strcpy(buf,path_tmp);
	  strcat(buf,"/dsXXXXXX.eml");
#ifndef ESDOS
		{
			r = mkstemp(buf);
			if (r >= 0)
				close(r);
		}
#else
        r = mktemp(buf);
#endif 	  
	  strcpy(fich_tmp,r);
	  ff = fopen(fich_tmp,"w");
	  if (ff)
	  {
		  while (LeeSLinea(csock,buf,maximo) >= 0)
		  {
			  if (buf[0] == '.' && !buf[1])
			  {
				  break;
			  }
			  fprintf(ff,"%s\n",buf);
		  }
		  fclose(ff);
	  }
	  else
		  okk = 0;
  }
  else if (handleMessage(part_init(csock), "text/plain", 0 , /*extractText*/0) || (total_fichero && total_fichero != donde_fichero))
  {
	  okk = 0;
  }
  if (!okk)
  {
      printf("Fallo al bajar el mensaje\n");
      rr = -1;
  }
  else
  {
      printf("correo recibido\n");
      strcpy(bufr,fich_tmp);
      if (borrarlo)
      {
        rwi_senddata(csock,"DELE 1\r\n",8);
        Pop3_Ok(csock);          
      }
  }


  Pop3_Quit(csock);
  closesocket(csock);
  
  return rr;
}

int
CompruebaSMTP(SOCKET csock,char *buf,int maximo,int codigo)
{
char tmp[10];

if (LeeSLinea(csock,buf,maximo) < 0) 
{
    return -1;
}
while (strlen(buf) > 3 && buf[3] == '-')
{
	if (LeeSLinea(csock,buf,maximo) < 0) 
	{
		  return -1;
	}
}
memcpy(tmp,buf,3);
tmp[3] = 0;
if (atoi(tmp) != codigo)
{
   printf("%s\r\n",buf);
   return -1;
}
return 0;
}

static int en_error = 0;

static char m_buffer[1024];
static int nm_buffer = 0;

int
m_putc(char c,SOCKET csock)
{
if (en_error) return -1;
if (c == 10)
{
    m_buffer[nm_buffer++] = 13;    
    m_buffer[nm_buffer++] = 10;    
}
else
    m_buffer[nm_buffer++] = c;    

if (nm_buffer >= 1000)
{
    if (rwi_senddata(csock,m_buffer,nm_buffer) == FALSE)  
    {
        en_error = 1;
        return -1;
    }
    nm_buffer = 0;
}
return c;
}

/*
#ifdef FGVDOS
#else
int m_fprintf(csock,format)
SOCKET csock;
char *format;
#endif
*/

#ifdef CONSTDARG
int m_fprintf(SOCKET csock,char *format, ...)
#else
int m_fprintf(csock,format,va_alist)
SOCKET csock;
char *format;
va_dcl
#endif
{

    va_list ap;
    char buf[8192];
    int n;   
#ifdef CONSTDARG
	va_start(ap,format);
#else
    va_start(ap);
#endif
    vsprintf(buf, format, ap);
    va_end(ap);


    if (en_error) return -1;

    if (nm_buffer)
    {
        if (rwi_senddata(csock,m_buffer,nm_buffer) == FALSE)  
        {
            en_error = 1;
            return -1;
        }
        nm_buffer = 0;
    }



    n = strlen(buf) - 1;
    if (n >= 0 && buf[n] == 10)
    {
        if (n == 0 || buf[n-1] != 13)
        {
            buf[n] = 13;
            buf[n+1] = 10;
            buf[n+2] = 0;
        }
    }

    if (rwi_senddata(csock,buf,strlen(buf)) == FALSE)
    {
        en_error = 1;
        return -1;
    }

    return 0;
}

static unsigned char basis_64[] =
   "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static unsigned char *output64chunk(int c1, int c2, int c3, int pads, unsigned char *dest)
{
	int pos = 0;
	int tmp;
	tmp = basis_64[c1>>2];
	dest[pos++] = tmp;
	tmp = basis_64[((c1 & 0x3)<< 4) | ((c2 & 0xF0) >> 4)];    
	dest[pos++] = tmp;
    if (pads == 2) 
	{
		dest[pos++] = '=';
		dest[pos++] = '=';
    } else if (pads) 
	{
		tmp = basis_64[((c2 & 0xF) << 2) | ((c3 & 0xC0) >>6)];
        dest[pos++] = tmp;	    
        dest[pos++] = '=';
    } else {
		tmp = basis_64[((c2 & 0xF) << 2) | ((c3 & 0xC0) >>6)];
        dest[pos++]  = tmp;

		tmp = basis_64[c3 & 0x3F];
        dest[pos++] = tmp;
    }
	dest[pos] = 0;

	return dest+pos;
}

static int convierte_a_base64(unsigned char *orig,int len,unsigned char *dest)
{
	int c1, c2, c3;

	*dest = 0;

    while (len > 0)
	{
		c1 = *(orig++);len--;
        c2 = *(orig++);len--;
        if (len < 0) 
		{
            dest = output64chunk(c1, 0, 0, 2, dest);
			break;
        } else 
		{
            c3 = *(orig++);len--;
            if (len < 0) 
			{
                dest = output64chunk(c1, c2, 0, 1, dest);
				break;
            } else 
			{
                dest = output64chunk(c1, c2, c3, 0, dest);
            }
        }
	}

	return 0;
}

int
SMTP_mensaje(char *elservidor,char *from,char *to,char *subject,char *messaje,char *files)
{
struct hostent *mihost;
struct sockaddr_in caddr;
int i,ok,j;
char *p;
int rr = -1;
char buf[2048];
int maximo = 1024;
long maxsize;
FILE *infile;
char *headers = NULL;
SOCKET csock;
int usarauth = 0;
char password[128];
char usuario[128];
char buf2[512];
int con_fichero = 0;

  if((csock =  socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) == SOCKET_ERROR)
     {    
     return rr;
     }
  i = 1;
  setsockopt(csock,IPPROTO_TCP,TCP_NODELAY,(char *)&i,sizeof(i));
  i = 60*NTFACTOR;
  setsockopt(csock,SOL_SOCKET,SO_SNDTIMEO,&i,sizeof(i));
  i = 60*NTFACTOR;
  setsockopt(csock,SOL_SOCKET,SO_RCVTIMEO,&i,sizeof(i));


  caddr.sin_addr.s_addr = inet_addr(elservidor);
  if (caddr.sin_addr.s_addr == INADDR_NONE)
  {
    if ( (mihost = gethostbyname(elservidor)) && mihost->h_length == 4 && mihost->h_addr_list[0])
    {	    
       caddr.sin_addr.s_addr = *((unsigned long *)mihost->h_addr_list[0]);
    }
  }
  caddr.sin_family = AF_INET;
  caddr.sin_port = htons(25);

  i = connect(csock,(struct sockaddr *) &caddr,sizeof (caddr));
  if (i == SOCKET_ERROR) 
  {
      goto salir;
  }

  if (CompruebaSMTP(csock,buf,maximo,220) < 0)
  {
      goto salir;
  }

  for (i = 0;from[i] && from[i] != '@';i++);
  if (from[i] == '@') i++;

  for (j = 0;j < i && from[j] != ':';j++);
  if (from[j] == ':')
  {
	 i -= j;
	 memcpy(password,from,j);
	 password[j] = 0;
	 from += j;
	 from++;
	 i--;
	 memcpy(usuario,from,i-1);
	 usuario[i-1] = 0;
     usarauth = 1;
  }
  
  if (usarauth)
  {
      sprintf(buf,"EHLO %s\r\n",from+i);
  }
  else  
  {
      sprintf(buf,"HELO %s\r\n",from+i);
  }

  if (rwi_senddata(csock,buf,strlen(buf)) == FALSE)
  {
      goto salir;
  }

  if (CompruebaSMTP(csock,buf,maximo,250) < 0)
  {
      goto salir;
  }

  if (usarauth)
  {	  
	  int trasera = 0;
	  do
	  {
		  sprintf(buf,"%s\001%s\001%s",usuario,usuario,password);
		  j = strlen(buf);
		  for (i = 0;i < j;i++)
		  {
			  if (buf[i] == 1)
				  buf[i] = 0;
		  }		  
		  convierte_a_base64(buf,j,buf2);
		  printf("AUTH PLAIN\r\n");
		  printf("%s\r\n",buf2);
		  sprintf(buf,"AUTH PLAIN %s\r\n",buf2);
		  if (rwi_senddata(csock,buf,strlen(buf)) == FALSE)
		  {
			  goto salir;
		  }		  
		  /*
		  if (CompruebaSMTP(csock,buf,maximo,334) < 0)
		  {
			  trasera = 1;
			  break;
		  }
		  if (rwi_senddata(csock,buf2,strlen(buf2)) == FALSE)
		  {
			  goto salir;
		  }	
		  */
		  if (CompruebaSMTP(csock,buf,maximo,235) < 0)
		  {
			  trasera = 1;
			  break;
		  }
		  printf("ok\r\n");
		  break;
	  } while(-1);
	  
	  if (trasera)
	  { 		
		  sprintf(buf,"AUTH LOGIN\r\n");
		  if (rwi_senddata(csock,buf,strlen(buf)) == FALSE)
		  {
			  goto salir;
		  }
		  if (CompruebaSMTP(csock,buf,maximo,334) < 0)
		  {
			  goto salir;			  
		  }
		  sprintf(buf,"\r\n");
		  if (rwi_senddata(csock,buf,strlen(buf)) == FALSE)
		  {
			  goto salir;
		  }
		  if (CompruebaSMTP(csock,buf,maximo,334) < 0)
		  {
			  goto salir;			  
		  }
		  sprintf(buf,"\r\n");
		  if (rwi_senddata(csock,buf,strlen(buf)) == FALSE)
		  {
			  goto salir;
		  }
		  if (CompruebaSMTP(csock,buf,maximo,235) < 0)
		  {
			  goto salir;			  
		  }
	  }
	  
  }


  sprintf (buf,"MAIL From:<%s>\r\n",from);
  if (rwi_senddata(csock,buf,strlen(buf)) == FALSE)
  {
      goto salir;
  }

  if (CompruebaSMTP(csock,buf,maximo,250) < 0)
  {
      goto salir;
  }


  headers = malloc(strlen(to)+50);
  sprintf(headers,"To: <%s>\r\nFrom: <%s>\r\n",to,from);

  i = 0;
  ok = 1;
  p = to;
  do
  {        
        if (to[i] == ';' || !to[i])
        {
            if (!to[i]) ok = 0;
            to[i] = 0;

            sprintf (buf,"RCPT TO:<%s>\r\n",p);
            if (rwi_senddata(csock,buf,strlen(buf)) == FALSE)
            {
                goto salir;
            }
            if (CompruebaSMTP(csock,buf,maximo,250) < 0)
            {
                goto salir;
            }

            p = to + i + 1;
        }

        i++;
  } while(ok);


  sprintf (buf, "DATA\r\n");
  if (rwi_senddata(csock,buf,strlen(buf)) == FALSE)
  {
      goto salir;
  }
  if (CompruebaSMTP(csock,buf,maximo,354) < 0)
  {
      goto salir;
  }

  /*******/

    p = files;    
    i = 0;
    ok = 1;
    maxsize = 0L;
    do
    {        
        if (files[i] == ';' || !files[i])
        {
            if (!files[i]) ok = 0;

            files[i] = 0;

    		if (p == files)
    		   maxsize = 0L;
    		else
    		   maxsize = -1L;

            infile = fopen(p, "rb");
            if (!infile)
			{
				infile = fopen(p, "wb");
				if (infile)				
				    fclose(infile);
				infile = fopen(p, "rb");
			}
            if (!infile)
			{
			   sprintf (buf, ".\r\n");
			   rwi_senddata(csock,buf,strlen(buf));
               CompruebaSMTP(csock,buf,maximo,250);
			   rr = 0;
               goto salir;
            }

            printf("%s [%s]\r\n",p,subject);

            en_error = 0;
            nm_buffer = 0;
			con_fichero = 1;
            if (encode(csock,infile, (FILE *)0, p ,messaje, subject, headers , maxsize, NULL/*typeoverride*/,NULL /*outfname*/) < 0 || en_error)
            {
               fclose(infile);
               goto salir;
            }

            fclose(infile);

            if (headers)
            {
                free(headers);
                headers = NULL;
            }

            p = files + i + 1;
        }

        i++;
    } while(ok);

  if (con_fichero)
  {
		m_fprintf(csock,"\r\n--===============1489874325==--\r\n"); /* por si queda buffer ... tambien */
  }
  else
  {
	    m_fprintf(csock,"\r\n%s\r\n",messaje); 
  } 
  /*******/


  sprintf (buf, ".\r\n");
  if (rwi_senddata(csock,buf,strlen(buf)) == FALSE)
  {
      goto salir;
  }
  if (CompruebaSMTP(csock,buf,maximo,250) < 0)
  {
      goto salir;
  }

  rr = 0;

salir:
  if (headers)
  {
    free(headers);
    headers = NULL;
  }
  sprintf (buf,"QUIT \r\n");
  rwi_senddata(csock,buf,strlen(buf));
  CompruebaSMTP(csock,buf,maximo,221);
  closesocket(csock);
return rr;
}


#ifdef FGVDOS
static BOOL carga_mapi()
{
    l = LoadLibrary("MAPI32.DLL");
    if (!l) return FALSE;    
    
    pfnSendMail  = GetProcAddress(l, "MAPISendMail");

    pfnLogon     = GetProcAddress(l, "MAPILogon");

    pfnFindNext  = GetProcAddress(l, "MAPIFindNext");

    pfnReadMail  = GetProcAddress(l, "MAPIReadMail");
 
    pfnDeleteMail= GetProcAddress(l, "MAPIDeleteMail");

    pfnLogoff    = GetProcAddress(l, "MAPILogoff");

    pfnFreeBuffer    = GetProcAddress(l, "MAPIFreeBuffer");

    pfnResolveName   = GetProcAddress(l, "MAPIResolveName");

  	if (pfnSendMail == NULL)
    {
        FreeLibrary(l);        
        l = NULL;
        return FALSE;
    }
    return TRUE;
}

void descarga_mapi()
{
  if (l)
  {
     FreeLibrary(l);        
     l = NULL;
  }
  pfnSendMail  = NULL;

  pfnLogon     = NULL;

  pfnFindNext  = NULL;

  pfnReadMail  = NULL;
 
  pfnDeleteMail= NULL;

  pfnLogoff    = NULL;

  pfnFreeBuffer= NULL;

  pfnResolveName = NULL;

}
#endif



int dscorreo_local_envia(char *servicio,char *from,char *to,char *subject,char *message,char *files)
{
   int i = -1;   
   char *llama;
   char tmp_l[512];
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

      if (!strcmp(servicio,"LLAMA"))
      {
          i = -1;
          llama = getenv("DS_LLAMA");
          if (llama)
          {      
              strcpy(tmp_l,llama);
              system(tmp_l);
              i = 0;
          }          
      } else if (!strcmp(servicio,"CUELGA"))
      {
          i = -1;
          llama = getenv("DS_CUELGA");
          if (llama)
          {
              strcpy(tmp_l,llama);
              system(tmp_l);
              i = 0;
          }  
      } else 
      {
          if (!servicio[0] || !strcmp(servicio,"systema") || !strcmp(servicio,"MAPI"))
          {

#ifdef FGVDOS
          carga_mapi();
          if (pfnSendMail)
          {
              if (MapiCorreo(to,from,subject,message,files) == TRUE)
                  i = 0;
              else
                  i = -1;
              descarga_mapi();
          }
          else
              i = -1;
#else
          i = SystemCorreo(to,from,subject,message,files);
#endif
          }
          else
          {
              i = SMTP_mensaje(servicio,from,to,subject,message,files);
          }
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
         return(i);
}


int dscorreo_local_recibe(char *servicio,char *usuario,char *password,char *path,char *bufr,char *sujeto)
{
   char *llama;
   int i = -1;
   MailMensaje Msg;
   char tmp_l[512];
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
      if (!strcmp(servicio,"LLAMA"))
      {
          i = -1;
          llama = getenv("DS_LLAMA");
          if (llama)
          {      
              strcpy(tmp_l,llama);
              system(tmp_l);
              i = 0;
          }          
      } else if (!strcmp(servicio,"CUELGA"))
      {
          i = -1;
          llama = getenv("DS_CUELGA");
          if (llama)
          {
              strcpy(tmp_l,llama);
              system(tmp_l);
              i = 0;
          }  
      } else 
      {
          i = 0;
          if (!servicio[0] || !strcmp(servicio,"systema") || !strcmp(servicio,"MAPI"))
          {
#ifdef FGVDOS
              carga_mapi();
              if (pfnSendMail)
              {
                  if (MapiRecibe(usuario,password,path,bufr) == TRUE)
                      i = 0;
                  else
                      i = -1;
                  descarga_mapi();
              }
              else
#endif
                  i = -1;
          }
          else
          {
              if (Haz_Pop3(servicio,usuario,password,path,bufr) < 0)
              {
	              i = -1;	      
              }    
          }
        /*
       if (*r)
       {
           strcat(r,";");
       }
       strcat(r,ff);
       */
		  if (i != -1)
		  {
			  strcpy(sujeto,el_sujeto);
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
         return(i);
}


