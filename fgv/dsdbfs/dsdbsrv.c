
#ifdef WIN32

#define FGVDOS
#define _WIN32_WINNT 0x0500 

#endif



#ifdef FGVDOS

typedef union _DS_LARGE_INTEGER { 
    struct {
        unsigned long LowPart; 
        long  HighPart; 
    } s;
    __int64 QuadPart;
} DS_LARGE_INTEGER; 

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/locking.h>
#include <share.h>
#include <io.h>
#include <windows.h>
#include <windowsx.h>
#include <winsock2.h>
#include <nspapi.h>

#define usleep(a) Sleep(a/1000)

#define pthread_t unsigned long
#define pthread_create(a,b,c,d) *(a) = _beginthread(c,(unsigned)0,d)
#define ERROR_THREAD(a) ((a) == -1)
#define pthread_kill(a,b) ResumeThread(a)
#define DSsigwait(a,b,c)  SuspendThread(a)
#define sigemptyset(a) /* */
#define sigaddset(a,b) /* */


#define sleep(a) Sleep((long)(((long)a)*1000L));

#else

typedef union _DS_LARGE_INTEGER { 
    struct {
        unsigned long LowPart; 
        long  HighPart; 
    } s;
    long long QuadPart;
} DS_LARGE_INTEGER; 

#include <pthread.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/mman.h>

#include <sys/stat.h>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/filio.h>
#include <netdb.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/resource.h>

#ifdef WATCOMNLM
#include <io.h>
#else
#include <unistd.h>
#endif

#include <time.h>
#include <sys/timeb.h>

#define Sleep(a)  usleep(a*1000)

#define DSsigwait(a,b,c)      sigwait(b,c)
#define ERRORWAIT(a) (a != 0)

#define SOCKET int
#define SOCKET_ERROR -1
#define INVALID_HANDLE_VALUE -1
#define ERROR_INVALID_HANDLE -1
#define INVALID_SOCKET -1

#define closesocket close
#define ioctlsocket ioctl


#define CRITICAL_SECTION pthread_rwlock_t
#define InitializeCriticalSection(a) pthread_rwlock_init(a,NULL)
#define EnterCriticalSection pthread_rwlock_wrlock
#define LeaveCriticalSection pthread_rwlock_unlock
#define DeleteCriticalSection pthread_rwlock_destroy

#ifndef MAP_NOSYNC
#define MAP_NOSYNC 0x0800
#endif

static pthread_attr_t t_attr;

#ifndef SEMMSL
#define SEMMSL 50
#endif


/*
#define CRITICAL_SECTION int
#define InitializeCriticalSection(a) InitBasura(a)
#define EnterCriticalSection EntraBasura
#define LeaveCriticalSection SalBasura
#define DeleteCriticalSection FinBasura
*/

#define ERROR_THREAD(a) ((a) != 0)


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

#ifndef O_BINARY
#define O_BINARY 0
#endif

#define __int64 long long


#endif

#ifndef LK_UNLCK
#define LK_UNLCK _LK_UNLCK
#endif

#ifndef LK_LOCK
#define LK_LOCK  _LK_LOCK
#endif

#ifndef LK_NBLCK
#define LK_NBLCK _LK_NBLCK
#endif

#include <errno.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>


#ifndef MSG_WAITALL
#define MSG_WAITALL 0
#endif

#ifndef IPTOS_THROUGHPUT
#define IPTOS_THROUGHPUT 0x08
#endif
#ifndef IPTOS_LOWDELAY
#define IPTOS_LOWDELAY 0x10
#endif

#ifndef DS_OFF_DEFINIDO
#define DS_OFF_DEFINIDO
#ifdef WIN32
typedef __int64 ds_off;
#else
typedef long long ds_off;
#endif
#endif

#include "dsdbsrv.h"

#ifdef WIN32
int aviso(char *mensaje)
{
	MessageBox(NULL,mensaje,"ATENCION",MB_ICONHAND | MB_OK);
	return 0;
}
#else

/*#define MUTEX_PROPIO*/
#define SEM_PROPIO

int *PMutex[NMUTEXPROPIO];

int yo_sem = 0;

int mipid = 0;

int aviso(char *mensaje)
{
	printf("%s\r\n",mensaje);	
	return 0;
}
#endif

HANDLE mapa = NULL; /* Mapa virtual memoria de cache y de trasferencia */

unsigned long TamanoCache = 256 * 1024 * 1024; // 256 megas 
unsigned char *PMemoria = NULL;


HANDLE semaforo = NULL; /* solo unix de momento */
HANDLE mutex_cache = NULL;
HANDLE mutex_ficheros = NULL;
HANDLE evento_cache = NULL;
HANDLE mutex_peticion = NULL; /* solo unix */
HANDLE evento_usuario = NULL; /* solo unix */

HANDLE eventos_usuario[64] = {NULL,NULL,}; /* solo windows */
int n_eventos_usuario = 0;

struct _controles_ *PControles = NULL;

CRITICAL_SECTION sem_user;
struct _handle_usuario_ *PUsuario = NULL;
int freeUsuario = -1;

int nusers = 0;

struct _handle_fichero_ *PFichero = NULL;
int freeFichero = -1;

int nCache = 0;
int ntbloque = 0;
int ultimobloque = 0;
struct _lista_cache_ *PLista = NULL;
struct _cache_ *PCache = NULL;

char *Version = "dsdbsrv 1.00";
char tmpcon[1024];

int SalirYa = 0;

int Limpieza = 0;

int nsigusr1 = 0;
int nsigusr2 = 0;

int debug = 1;

SOCKET consola = ERROR_INVALID_HANDLE;

unsigned long IncluyeBloque(int fh,unsigned long blofi);

static void activa_signal(int s)
{
#ifndef WIN32
   {
	   sigset_t ss;
	   sigemptyset (&ss);
	   sigaddset (&ss,s);	   
       sigprocmask(SIG_UNBLOCK, &ss,NULL);
   }
#endif
}

static void desactiva_signal(int s)
{
#ifndef WIN32
   {
	   sigset_t ss;
	   sigemptyset (&ss);
	   sigaddset (&ss,s);	   
       sigprocmask(SIG_BLOCK, &ss,NULL);
   }
#endif
}


static void logea(char *texto)
{
	FILE *f;

	f = fopen("/dsdbsrv.log","a");
	if (f)
	{
		fputs(texto,f);
		fclose(f);
	}
}

int CreaCache()
{    
	int i;
	
	InitializeCriticalSection(&sem_user);

#ifdef WIN32	
	evento_cache = CreateEvent(NULL,TRUE,FALSE,"DSDB_SRV_EVENTO_XXXXXXXXX");
	if (evento_cache == ERROR_INVALID_HANDLE || GetLastError() == ERROR_ALREADY_EXISTS)
	{
		evento_cache = NULL;
		return 4;
	}

	mutex_cache = CreateMutex( NULL, FALSE , NULL);

	mutex_ficheros = CreateMutex( NULL, FALSE , NULL);

    n_eventos_usuario = MAXIMUM_WAIT_OBJECTS - 1;

	if (n_eventos_usuario > 64) 
		n_eventos_usuario = 64;

	for (i = 0;i < n_eventos_usuario;i++)
	{
		eventos_usuario[i] = ERROR_INVALID_HANDLE;
		eventos_usuario[i] = CreateEvent(NULL,TRUE,FALSE,NULL);
		if (eventos_usuario[i] == ERROR_INVALID_HANDLE)
		{
			break;
		}
	}
	if (i < n_eventos_usuario)
	{
		return 4;
	}

	if (!(mapa = CreateFileMapping(
	  INVALID_HANDLE_VALUE,             
	  NULL,
	  PAGE_READWRITE, // protection for mapping object
	  0,              // high-order 32 bits of object size
	  TamanoCache,      // low-order 32 bits of object size
	  /*"DSDB_SRV_MAP"*/NULL)))
	{		
		return 1;
	}
	/*
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		CloseHandle(mapa);		
		return 2;
	}
	*/
	PMemoria = (unsigned char *)MapViewOfFile(mapa,FILE_MAP_WRITE,0,0,TamanoCache);
	if (!PMemoria)
	{
		CloseHandle(mapa);
		return 3;
	}
#else
	{
		int rc;
		unsigned short init_vals[NMUTEXPROPIO];
		int i;

		semaforo = semget( 1366 , NMUTEXPROPIO , 0666 | IPC_CREAT | IPC_EXCL );
		if ( semaforo == -1 )
		{
			printf("main: semget() failed %d\n",errno);
			return 4;
		}
		mutex_cache    = 0;
		mutex_ficheros = 1;
		evento_cache   = 2;
		mutex_peticion = 3;
		evento_usuario = 4;

		/* el evento propio por signal */

#ifdef MUTEX_PROPIO
		init_vals[0] = 0;
		init_vals[1] = 0;
		init_vals[2] = 0;
		init_vals[3] = 0;
		init_vals[4] = 0;
		for (i = 5;i < NMUTEXPROPIO;i++)
		{
			init_vals[i] = 0;
		}
#else
		init_vals[0] = 1;
		init_vals[1] = 1;
		init_vals[2] = 0;
		init_vals[3] = 1;
		init_vals[4] = 0;
		for (i = 5;i < NMUTEXPROPIO;i++)
		{
			init_vals[i] = 1;
		}
#endif

		for (i = 0;i < NMUTEXPROPIO;i++)
		{
			rc = semctl( semaforo, i , SETVAL, init_vals[i]);
			if(rc == -1)
			{
				printf("main: semctl() initialization failed\n");
			}
		}

		/*
		rc = semctl( semaforo, NMUTEXPROPIO , SETALL, init_vals);
		if(rc == -1)
		{
			printf("main: semctl() initialization failed\n");
			return ( -1 );
		}
		*/

		/*
        mapa = shmget(1365, TamanoCache , 0666 | IPC_CREAT);
		if (mapa == -1)
		{
			printf("main: shmget() failed %d\n",errno);
			return ( -1 );
		}       

		PMemoria = shmat(mapa, NULL, 0);
		if ( PMemoria==NULL )
		{
			printf("main: shmat() failed %d\n",errno);
			return ( -1 );
		}
		*/
		
		mapa = open("/usr/prueba.mapa",O_RDWR|O_CREAT,S_IREAD|S_IWRITE);
		if (mapa < 0)
		{
			printf("main: open mapa failed %d\n",errno);
			return ( -1 );
		}
		lseek(mapa,TamanoCache,SEEK_SET);
		write(mapa,"",1);		
		PMemoria = mmap(0,TamanoCache,PROT_READ|PROT_WRITE,MAP_SHARED|MAP_NOSYNC|MAP_HASSEMAPHORE,mapa,0);
	}


#endif
	memset(PMemoria,0,TamanoCache);

	nCache = TamanoCache - sizeof(struct _controles_);
	nCache -= sizeof(struct _handle_usuario_) * MAX_USUARIOS;
	nCache -= sizeof(struct _handle_fichero_) * MAX_FICHEROS;
	nCache -= 513 * sizeof(struct _lista_cache_);
	nCache /= (sizeof(struct _lista_cache_)*2 + sizeof(struct _cache_));

	PControles = (struct _controles_ *)PMemoria;
	PUsuario = (struct _handle_usuario_ *)(((char *)PControles)+sizeof(struct _controles_));
	PFichero = (struct _handle_fichero_ *)(((char *)PUsuario)+sizeof(struct _handle_usuario_)*MAX_USUARIOS);	
	PLista = (struct _lista_cache_ *)(((char *)PFichero)+sizeof(struct _handle_fichero_)*MAX_FICHEROS);
	PCache   = (struct _cache_ *)(((char *)PLista)+sizeof(struct _lista_cache_)*(513+nCache*2));

	ntbloque = ((nCache*2)/512);

	ultimobloque = 513 + (512 * ntbloque);

#ifdef WIN32
	PControles->procesocache = GetCurrentProcessId();
#else
	PControles->procesocache = getpid();
	mipid = getpid();
	{
		for (i = 0;i < NMUTEXPROPIO;i++)
		{			
			PMutex[i] = &(PControles->mutex[(i*(MAX_USUARIOS+3))]);
		}
	}
#endif

	return 0;
}


void FinCache()
{
	int i;

#ifdef WIN32	
	if (PMemoria)
		UnmapViewOfFile(PMemoria);
	if (mapa)
		CloseHandle(mapa);
	if (evento_cache)
		CloseHandle(evento_cache);
	for (i = 0;i < n_eventos_usuario;i++)
	{
		if (eventos_usuario[i])
			CloseHandle(eventos_usuario[i]);
		eventos_usuario[i] = NULL;
	}
#else
	{
		struct shmid_ds shmid_struct;
		unsigned short init_vals[NMUTEXPROPIO];

		memset(&shmid_struct,0,sizeof(shmid_struct));
		memset(init_vals,0,sizeof(init_vals));

		semctl(semaforo, NMUTEXPROPIO , IPC_RMID,init_vals);		
		/*
		shmdt(PMemoria);
		shmctl(mapa, IPC_RMID, &shmid_struct);
		*/
		munmap(PMemoria, TamanoCache); 
		PMemoria = NULL;
		close(mapa);
		mapa = -1;

	}
#endif

	evento_cache = NULL;
	PMemoria = NULL;
	mapa = NULL;
}

/******************/

/* Socket helpers */
SOCKET bsock = INVALID_HANDLE_VALUE;

SOCKET controlsock = INVALID_HANDLE_VALUE;

void FastSocket(SOCKET sock)
{
	int i;

#ifdef IP_TOS
	i = IPTOS_LOWDELAY;
	setsockopt(sock,IPPROTO_IP,IP_TOS,(const char *)&i,sizeof(i));
#endif

	i = 1;
	setsockopt(sock,SOL_SOCKET,SO_OOBINLINE,(const char *)&i,sizeof(i));

	i = -1;
	setsockopt(sock,IPPROTO_TCP,TCP_NODELAY,(const char *)&i,sizeof(i));
}

void ReutilizaDireccion(SOCKET sock)
{
	 int i = -1;
	 setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,(const char *)&i,sizeof(i));
#ifndef FGVDOS
#ifdef SO_REUSEPORT
	 i = -1;
	 setsockopt(sock,SOL_SOCKET,SO_REUSEPORT,(const char *)&i,sizeof(i));
#endif
#endif
}


static BOOL EnviaPPaquete(SOCKET sock,PPaquete *sendbuf)
{
    int bytessent,totalbytes;
    int i;

    totalbytes = LEN_CABECERA;

    bytessent = 0;			  
    while(bytessent < totalbytes)
    {
#ifndef WIN32
		errno = 0;
#endif
       i = send(sock,((char *)&(sendbuf->c)) + bytessent,totalbytes - bytessent,0);
       if (i == SOCKET_ERROR)
       {
#ifdef FGVDOS
          if (WSAGetLastError() != WSAEWOULDBLOCK)
          {
             
             return FALSE;
          }
#else
		  if (errno == EINTR)
		  {
			  usleep(1);
			  continue;
		  }
          if (errno != EWOULDBLOCK)
          {
             return FALSE;
          }
		  usleep(1);
#endif          		  
          continue;
       }
       else
	   {
		  if (!i)
		  {
			  return FALSE;
		  }
          bytessent += i;
	   }
    }

	bytessent  = 0;	
	totalbytes = LEN_DATA(sendbuf);
	
	while(bytessent < totalbytes)
	{
#ifndef WIN32
		errno = 0;
#endif
	   i = send(sock,((char *)(sendbuf->buffer)) + bytessent,totalbytes - bytessent,0);
	   if (i == SOCKET_ERROR)
	   {
#ifdef FGVDOS
		  if (WSAGetLastError() != WSAEWOULDBLOCK)
		  {
         
			 return FALSE;
		  }
#else
		  if (errno == EINTR)
		  {
			  usleep(1);
			  continue;
		  }
		  if (errno != EWOULDBLOCK)
		  {
			 return FALSE;
		  }
		  usleep(1);
#endif
      
		  continue;
	   }
	   else
	   {
		  if (!i)
		  {
			  return FALSE;
		  }
		  bytessent += i;
	   }
	}
	
    return TRUE;            
}

static BOOL EnviaPaquete(SOCKET sock,Paquete *paquete)
{
	PPaquete sendbuf;
	sendbuf.c = paquete->c;
	sendbuf.buffer = paquete->buffer;
	return EnviaPPaquete(sock,&sendbuf);
}

int RecibePaquete(SOCKET sock, Paquete *recvbuf,int tout)
{
	fd_set setread;
	fd_set seterror;
	struct timeval timeout;
	struct timeval *ptm = NULL;
	long cuanto;
	int readsize, totalbytesread, msgsize, bytesread,err;

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
		if ((err = select(sock+1,&setread,NULL,&seterror,ptm)) == SOCKET_ERROR)
		{
#ifndef WIN32
			if (errno == EINTR)
			{
				usleep(1);
				continue;
			}
#endif
			return(SOCKET_ERROR);
		}
		if (!err)  
		{
			return(0); /* timeout */
		}
        cuanto = 0L;
        ioctlsocket(sock,FIONREAD,&cuanto);
        if (!cuanto)
		{
            return(SOCKET_ERROR);
		}
		if (cuanto >= LEN_CABECERA)
		{
			break;
		}
	} while(-1);

	msgsize = LEN_CABECERA;	
	totalbytesread = 0;
	while(totalbytesread < msgsize)
	{		
#ifndef WIN32
		errno = 0;
#endif
		readsize = msgsize - totalbytesread;
		if((bytesread =  recv(sock, ((char *)&(recvbuf->c))+totalbytesread, readsize,MSG_WAITALL)) == SOCKET_ERROR)
		{		
#ifdef FGVDOS
		  if (WSAGetLastError() != WSAEWOULDBLOCK)
		  {
			  
			  return SOCKET_ERROR;
		  }										  
#else
		  if (errno == EINTR)
		  {
			  usleep(1);
			  continue;
		  }
		  if (errno != EWOULDBLOCK)
		  {
			  return SOCKET_ERROR;
		  }
		  usleep(1);
#endif
		  continue;
		}	
		if (!bytesread)
		{
		   return SOCKET_ERROR;
		}
		totalbytesread += bytesread;
	}
	
	if(recvbuf->c.id != DSDSDBID)
	{
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
#ifdef FGVDOS
		  if (WSAGetLastError() != WSAEWOULDBLOCK)
		  {
			  
			  return SOCKET_ERROR;
		  }										  
#else
		  if (errno == EINTR)
		  {
			  usleep(1);
			  continue;
		  }
		  if (errno != EWOULDBLOCK)
		  {
			  return SOCKET_ERROR;
		  }
		  usleep(1);
#endif
		  continue;
		}
		if (!bytesread)
		{
		   return SOCKET_ERROR;
		}
		totalbytesread += bytesread;
		readsize -= bytesread;
	}
	
	return 1;
}


static BOOL rwi_senddata(SOCKET sock,char  *sendbuf,int totalbytes)
{
			int bytessent;
         int i,err;			
         unsigned char controla;			

        
		 if (totalbytes == -1)
		 {
			 totalbytes = strlen(sendbuf);
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
                     
                     return FALSE;
                  }
#else
				  if (errno == EINTR)
				  {
					  usleep(1);
					  continue;
				  }
                  if (errno != EWOULDBLOCK)
                  {
                     return FALSE;
                  }
				  usleep(1);
#endif
                  
                  continue;
               }
               else
			   {
					if (!i)
					{
					   return FALSE;
					}
                  bytessent += i;
			   }
            }
            
            return TRUE;
            
         } while(-1);
         return FALSE;
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
#else
  	     if (errno == EINTR)
		 {
			  usleep(1);
			  continue;
		 }
         if (errno != EWOULDBLOCK)
         {
            return FALSE;
         }
		 usleep(1);
#endif
         continue;
      }
		if (!bytesread)
		{
		   return FALSE;
		}

      totalbytesread += bytesread;
      readsize -= bytesread;
   }        
   return(TRUE);
} 

int CreaSocket()
{
	struct sockaddr_in addr;
#ifdef WIN32
	WORD      wVersionRequested;
	WSADATA   wsaData;
#endif	

#ifdef WIN32
	wVersionRequested = MAKEWORD(2,0);
	if (WSAStartup(wVersionRequested, &wsaData) != 0)
	{		
		return -1;
	}
#endif

	bsock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (bsock == INVALID_HANDLE_VALUE)
	{
		return -2;
	}

	FastSocket(bsock);

	addr.sin_addr.s_addr = inet_addr("0.0.0.0"); 
	addr.sin_family = AF_INET;
	addr.sin_port = htons(DSDBPORT);
	if (bind(bsock,(struct sockaddr *) &addr,sizeof (addr)) == SOCKET_ERROR)
	{		
		closesocket(bsock);		
#ifdef WIN32
		WSACleanup();
#endif
		return(2); 
	}
	
	if (listen(bsock,100) == SOCKET_ERROR)
	{		
		closesocket(bsock);	
#ifdef WIN32
		WSACleanup();
#endif
		return 2;
    }	
	

	controlsock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if (bsock == INVALID_HANDLE_VALUE)
	{
		printf("Error al crear socket de control ...\r\n");
		return 0;
	}

	FastSocket(controlsock);

	addr.sin_addr.s_addr = inet_addr("0.0.0.0"); 
	addr.sin_family = AF_INET;
	addr.sin_port = htons(DSDBPORTC);
	if (bind(controlsock,(struct sockaddr *) &addr,sizeof (addr)) == SOCKET_ERROR)
	{		
		closesocket(controlsock);		
		controlsock = INVALID_HANDLE_VALUE;
		printf("Error al crear socket de control (bind)...\r\n");
		return(0); 
	}
	
	if (listen(controlsock,10) == SOCKET_ERROR)
	{		
		closesocket(controlsock);		
		controlsock = INVALID_HANDLE_VALUE;
		printf("Error al crear socket de control (listen)...\r\n");
		return(0); 
    }	

	return 0;
}


void FinSocket()
{
	closesocket(bsock);
#ifdef WIN32
	WSACleanup();
#endif
}


/*******************************************/
/* Sesion helpers */


#ifndef WIN32
int InitAttr()
{
	pthread_attr_init(&t_attr);
	pthread_attr_setdetachstate(&t_attr,PTHREAD_CREATE_DETACHED);
	pthread_attr_setinheritsched(&t_attr,PTHREAD_INHERIT_SCHED);	
}
#endif
/*******************************************/

int BorraBloqueo(int fh,int idx)
{
	int i,n;
	ds_off *pbloqueos;
	ds_off *phasta;
	int *pamo;
	
	if (PFichero[fh].nbloqueos <= 0)
	{		
		return 0;
	}
	(PFichero[fh].nbloqueos)--;

	n = PFichero[fh].nbloqueos;

	pbloqueos = &(PFichero[fh].bloqueos[0]);
	phasta    = &(PFichero[fh].hasta[0]);
	pamo      = &(PFichero[fh].amo[0]);
	
	for (i = idx;i < n;i++)
	{
		pbloqueos[i] = pbloqueos[i+1];
		phasta[i] = phasta[i+1];
		pamo[i] = pamo[i+1];
	}
	
	return 1;
}

void DejaMutex(HANDLE mutex);

void TomaMutex(HANDLE mutex)
{
#ifdef WIN32
 	if (WaitForSingleObject(mutex,0) != WAIT_OBJECT_0)
	{
		 while((WaitForSingleObject(mutex,10000) != WAIT_OBJECT_0));
	}	
#else
#ifdef MUTEX_PROPIO
	int *m,e;
	struct sembuf operation;

	m = PMutex[mutex];
	do
	{		
		nsigusr2 = 0;
		e = semctl( semaforo, mutex , GETVAL);
		if (!e)
		{
			while(semctl( semaforo, mutex , SETVAL, (mipid&0x7fff)) < 0);
		}
		else
		{
			if (e < 0)
				continue;
			if (e && *m && ((*m)&0x7fff) == e && kill(*m,0) < 0)
			{
				while(semctl( semaforo, mutex , SETVAL, (mipid&0x7fff)) < 0);
				e = 0;
			}
		}
		if (!e)
		{			
			usleep(0);
			/**m = mipid;*/
			/*
		    operation.sem_num =  mutex;
		    operation.sem_op =  0;
		    operation.sem_flg =  IPC_NOWAIT;
			semop( semaforo, &operation, 1 );
			*/
			while((e = semctl( semaforo, mutex , GETVAL)) < 0);
			while((e = semctl( semaforo, mutex , GETVAL)) < 0);
			while((e = semctl( semaforo, mutex , GETVAL)) < 0);
			while((e = semctl( semaforo, mutex , GETVAL)) < 0);
			while((e = semctl( semaforo, mutex , GETVAL)) < 0);
			while((e = semctl( semaforo, mutex , GETVAL)) < 0);
			while((e = semctl( semaforo, mutex , GETVAL)) < 0);
			while((e = semctl( semaforo, mutex , GETVAL)) < 0);
			while((e = semctl( semaforo, mutex , GETVAL)) < 0);
			while((e = semctl( semaforo, mutex , GETVAL)) < 0);

			if ((mipid&0x7fff) == e)
			{
				*m = mipid;
				m[2+yo_sem] = 0;
				if (PControles->_sema[mutex])
				{
					sprintf(tmpcon,"[D]EL SEMAFORO NO VA PARA NADA %d TomaMutex: %d\r\n",mutex,PControles->_sema[mutex]);
					printf(tmpcon);
					logea(tmpcon);
					if (consola != INVALID_HANDLE_VALUE)
					{
						rwi_senddata(consola,tmpcon,-1);
					}					
					continue;
				}
				PControles->_sema[mutex]++;
				return; /* ya es mio */
			}
		}
		if (m[2+yo_sem] != mipid)
		{
			m[2+yo_sem] = mipid;
		}
		if (!nsigusr2)
		{
			usleep(10000);
		}
	} while (-1);
#else
	{
		int e;
		int pid;
		struct sembuf operation[2];

		do
		{

			operation[0].sem_num =  mutex;
			operation[0].sem_op =  -1;
			operation[0].sem_flg =  SEM_UNDO | IPC_NOWAIT;
			errno = 0;
			e = semop( semaforo, operation, 1 );
			if (e >= 0)
			{
				break;
			}

		   if (semctl( semaforo, mutex , GETVAL) == 0)
		   {			
#ifdef SEM_PROPIO
			/*activa_signal(SIGUSR2);*/
			(*PMutex[mutex])++;
			*(PMutex[mutex]+2+yo_sem) = mipid;
#endif
			while(semctl( semaforo, mutex , GETVAL) == 0)
			{
				
#ifdef SEM_PROPIO				
				usleep(10000);
#else
				usleep(1);
#endif
				
			}			
#ifdef SEM_PROPIO
			/*desactiva_signal(SIGUSR2);*/
			*(PMutex[mutex]+2+yo_sem) = 0;
			(*PMutex[mutex])--;
#endif
		   }
			/*
		e = semctl( semaforo, mutex , GETNCNT);
		if (e > 0)
		{
			usleep(1);
			continue;
		}			

		e = semctl( semaforo, mutex , GETVAL);
		if (!e && !PControles->_sema[mutex])
		{
			for (e = 0;e < 100000 && !PControles->_sema[mutex] && semctl( semaforo, mutex , GETVAL) == 0;e++)
			{
				usleep(0);
			}
			if (e >= 100000)
			   continue;
		}
		*/


		operation[0].sem_num =  mutex;
		operation[0].sem_op =  -1;
		operation[0].sem_flg =  SEM_UNDO;
		/*
		operation[1].sem_num =  mutex;
		operation[1].sem_op =  1;
		operation[1].sem_flg =  SEM_UNDO | IPC_NOWAIT;
		*/
		errno = 0;
		e = semop( semaforo, operation, 1 );
		if (e == -1)
		{
			continue;
		}
		break;
		pid = semctl( semaforo, mutex , GETPID);
		/*
		if (e == -1)
		{
			sprintf(tmpcon,"[D]Error semaforo %d TomaMutex:%d\r\n",mutex,errno);
			printf(tmpcon);
			logea(tmpcon);
			if (consola != INVALID_HANDLE_VALUE)
			{
				rwi_senddata(consola,tmpcon,-1);
			}
		}
		*/
		e = semctl( semaforo, mutex , GETVAL);
		if(e == -1)
		{
			sprintf(tmpcon,"[D]Error semaforo(ctl) %d TomaMutex:%d\r\n",mutex,errno);
			printf(tmpcon);
			logea(tmpcon);
			if (consola != INVALID_HANDLE_VALUE)
			{
				rwi_senddata(consola,tmpcon,-1);
			}
			continue;
		}
		/*
		if (e == 0)
		{
			if (pid != getpid())
			{
				continue;
			}
		}
		*/
		if (e == 1)
			continue;
		if (e > 1)
		{
			sprintf(tmpcon,"[D]EL SEMAFORO NO VA BIEN %d TomaMutex:%d\r\n",mutex,e);
			printf(tmpcon);
			logea(tmpcon);
			if (consola != INVALID_HANDLE_VALUE)
			{
				rwi_senddata(consola,tmpcon,-1);
			}
			/*DejaMutex(mutex);*/
			continue;
		}
		break;
		/*
		if (PControles->_sema[mutex])
		{
			sprintf(tmpcon,"[D]EL SEMAFORO NO VA PARA NADA %d TomaMutex: %d\r\n",mutex,PControles->_sema[mutex]);
			printf(tmpcon);
			logea(tmpcon);
			if (consola != INVALID_HANDLE_VALUE)
			{
				rwi_senddata(consola,tmpcon,-1);
			}
			DejaMutex(mutex);
			continue;
		}
		*/
		}while(-1);
		PControles->_sema[mutex]++;
	}
#endif
#endif
}

void DejaMutex(HANDLE mutex)
{
#ifdef WIN32
	ReleaseMutex(mutex);
#else
#ifdef MUTEX_PROPIO
	int *m;
	int i,n,j;
	int espera;

	if (!(PControles->_sema[mutex]))
	{
		sprintf(tmpcon,"[D]Error semaforo %d DejaMutex(0):%d\r\n",mutex,errno);
		printf(tmpcon);
		logea(tmpcon);
		if (consola != INVALID_HANDLE_VALUE)
		{
			rwi_senddata(consola,tmpcon,-1);
		}
	}

	PControles->_sema[mutex]--;

	m = PMutex[mutex];
	if (*m != mipid)
	{
		sprintf(tmpcon,"[D]Error semaforo %d DejaMutex(1):%d - %d\r\n",mutex,*m,mipid);
		printf(tmpcon);
		logea(tmpcon);
		if (consola != INVALID_HANDLE_VALUE)
		{
			rwi_senddata(consola,tmpcon,-1);
		}		
	}
	n = m[1];		
	for(j = 0;j <= PControles->nUsuarios;j++)
	{
		if (n > PControles->nUsuarios)
			n = 0;
		if ((espera = m[2+n]))
		{				
			if (kill(espera,0) < 0)
			    m[2+n] = 0;
			else
			{
				m[1] = n+1;
				m[2+n] = 0;
				*m = 0;
				while(semctl( semaforo, mutex , SETVAL, 0) < 0);
				kill(espera,SIGUSR2);
				return;
			}
		}
		n++;
	}	
	*m = 0;
	while(semctl( semaforo, mutex , SETVAL, 0) < 0);
	return;
#else
	{
		struct sembuf operation;
		int e;
		int pid;
		int espera = 0;
		
		
		/*
		if (!(PControles->_sema[mutex]))
		{
			sprintf(tmpcon,"[D]Error semaforo %d DejaMutex(1):%d\r\n",mutex,errno);
			printf(tmpcon);
			logea(tmpcon);
			if (consola != INVALID_HANDLE_VALUE)
			{
				rwi_senddata(consola,tmpcon,-1);
			}
		}
		*/

		PControles->_sema[mutex]--;
		
		/*
		if (PControles->_sema[mutex])
		{
			sprintf(tmpcon,"[D]EL SEMAFORO NO VA PARA NADA %d DejaMutex: %d\r\n",mutex,PControles->_sema[mutex]);
			printf(tmpcon);
			logea(tmpcon);
			if (consola != INVALID_HANDLE_VALUE)
			{
				rwi_senddata(consola,tmpcon,-1);
			}
		}
		*/

		/*

		e = semctl( semaforo, mutex , GETVAL);
		if (e > 0)
		{
			sprintf(tmpcon,"[D]EL SEMAFORO NO VA PARA NADA(1) %d DejaMutex: %d\r\n",mutex,PControles->_sema[mutex]);
			printf(tmpcon);
			logea(tmpcon);
			if (consola != INVALID_HANDLE_VALUE)
			{
				rwi_senddata(consola,tmpcon,-1);
			}
		}
		*/
#ifdef SEM_PROPIO
		{
			int n,j;
			int *m;
			m = PMutex[mutex];
			if (*m)
			{
				n = m[1];		
				for(j = 0;j <= PControles->nUsuarios;j++)
				{
					if (n > PControles->nUsuarios)
						n = 0;
					if ((espera = m[2+n]))
					{				
						if (kill(espera,0) < 0)
							m[2+n] = 0;
						else
						{
							m[1] = n+1;
							m[2+n] = 0;
							break;
						}
					}
					n++;
				}	
			}
		}
#endif


		do
		{

		operation.sem_num =  mutex;
		operation.sem_op =   1;
		operation.sem_flg =  SEM_UNDO;

		e = semop( semaforo, &operation, 1 );
		/*e = semctl( semaforo, mutex , SETVAL,1);*/
		if (e == -1)
		{
			while (semctl( semaforo, mutex , GETVAL) < 0) usleep(1);
			if (semctl( semaforo, mutex , GETVAL) < 1)
				continue;
		}
		if (espera)
		{
			kill(espera,SIGUSR2);
		}
		break;

		pid = semctl( semaforo, mutex , GETPID);
		e = semctl( semaforo, mutex , GETVAL);

		if (pid != mipid) /* esperamos que otro lo a pillado inmediatamente */
		{
			if (e > 0 && PControles->_sema[mutex])
			{
				sprintf(tmpcon,"[D]Error semaforo(5) %d DejaMutex:%d\r\n",mutex,errno);
				printf(tmpcon);
				logea(tmpcon);
				if (consola != INVALID_HANDLE_VALUE)
				{
					rwi_senddata(consola,tmpcon,-1);
				}
				continue;
			}
			break;
		}		
		if (e == 0)
		{
			if (semctl( semaforo, mutex , GETPID) == mipid)
			{
				continue;
			}			
		}		
		if (e == 0)
		{			
			if (semctl( semaforo, mutex , GETPID) == mipid)
			{
				sprintf(tmpcon,"[D]Error semaforo(2) %d DejaMutex:%d\r\n",mutex,errno);
				printf(tmpcon);
				logea(tmpcon);
				if (consola != INVALID_HANDLE_VALUE)
				{
					rwi_senddata(consola,tmpcon,-1);
				}
				continue;
			}			
		}
		
		e = 0;
		while(semctl( semaforo, mutex , GETNCNT) > 0)
		{
			usleep(1);
			e++;
			if (semctl( semaforo, mutex , GETPID) == mipid)
			{
				usleep(1);
			}			
			else
				break;			
			if (e > 10000)
			{
				sprintf(tmpcon,"[D]Error semaforo(3) %d DejaMutex:%d\r\n",mutex,errno);
				printf(tmpcon);
				logea(tmpcon);
				if (consola != INVALID_HANDLE_VALUE)
				{
					rwi_senddata(consola,tmpcon,-1);
				}
				break;
			}			
		}
		
		if (e > 10000)
		{
			continue;
		}
		

		break;
		} while(-1);
		if (e == -1)
		{
			sprintf(tmpcon,"[D]Error semaforo %d DejaMutex:%d\r\n",mutex,errno);
			printf(tmpcon);
			logea(tmpcon);
			if (consola != INVALID_HANDLE_VALUE)
			{
				rwi_senddata(consola,tmpcon,-1);
			}
		}		
	}
#endif
#endif
}


void SubeSem(HANDLE evento)
{
#ifdef WIN32
	SetEvent(evento);
#else
#ifdef SEM_PROPIO
	int *m;
	int j,n;
	int espera;

	m = PMutex[evento];
	n = m[1];
	for(j = 0;j <= PControles->nUsuarios;j++)
	{
		if (n > PControles->nUsuarios)
			n = 0;
		if ((espera = m[2+n]))
		{		
			m[1] = n + 1;
			m[2+n] = 0;
			kill(espera,SIGUSR2);
		}
		n++;
	}
	return;

#else
	{
		struct sembuf operation;
		
		operation.sem_num =  evento;
		operation.sem_op =  -1;
		operation.sem_flg =  IPC_NOWAIT;

		while(semop( semaforo, &operation, 1 ) == -1 && errno == EINTR);
	}
#endif
#endif
}


void BajaSem(HANDLE evento)
{
#ifdef WIN32
	ResetEvent(evento);
#else
	{
		struct sembuf operation;
		
		operation.sem_num =  evento;
		operation.sem_op =  1;
		operation.sem_flg =  IPC_NOWAIT;

		while(semop( semaforo, &operation, 1 ) == -1 && errno == EINTR);
	}
#endif
}

void EsperaSem(HANDLE evento)
{
#ifdef WIN32
	 int desb_i;
     desb_i = WaitForSingleObject(evento_cache,10);
	 if (desb_i == WAIT_OBJECT_0)
	 {
		 ResetEvent(evento_cache);
	 }
#else
#ifdef SEM_PROPIO
	int *m;
	int j;
	int espera;

	m = PMutex[evento];
	m[2+yo_sem] = mipid;	
	usleep(10000);
	m[2+yo_sem] = 0;
	nsigusr2 = 0;
	return;
#else
	{
		struct sembuf operation;
		
		operation.sem_num =  evento;
		operation.sem_op =  0;
		operation.sem_flg =  0;

		if (!semctl(semaforo,evento,GETVAL))
			BajaSem(evento);
		semop( semaforo, &operation, 1 );		
	}	 
#endif
#endif
}


void HebraControl(void *par)
{
	SOCKET sock = (SOCKET)par;
	char tmp[10240];
	int i,n = 0;
	char in[2];
	int idx,err;
	int permiso = 0;
	fd_set _setread;
	fd_set _seterror;
	struct timeval _timeout;


	if (SalirYa)
	{
		rwi_senddata(sock,"\r\nEl servidor esta finalizando\r\n",-1);
		Sleep(1000);
		closesocket(sock);
		return;
	}

	for (i = 0;i < nCache;i++)
	{
		if ((PCache[i].flags & DSDB_FLAGDIRTY))
			n++;
	}

	sprintf(tmp,"%s memoria:%ld debug %d Usuarios: %d Ficheros: %d Bloques por actualizar: %d\r\n",Version,TamanoCache,debug,nusers,PControles->nFicheros,n);
    if (rwi_senddata(sock,tmp,-1) == FALSE)
	{
		closesocket(sock);
		return;
	}

	do
	{
		idx = 0;	
		
		while(-1)
		{
			FD_ZERO(&_setread);
			FD_SET(sock,&_setread);			
			FD_ZERO(&_seterror);
			FD_SET(sock,&_seterror);			
			_timeout.tv_sec  = 0;
			_timeout.tv_usec = 10000;

            if ((err = select(sock+1,&_setread,NULL,&_seterror,&_timeout)) == SOCKET_ERROR)
			{
#ifndef WIN32
				if (errno == EINTR)
				{
					continue;
				}
#endif
			    closesocket(sock);
			    return;
			}
			if (SalirYa)
			{
				break;
			}
			if (FD_ISSET(sock,&_seterror))
			{
			    closesocket(sock);
			    return;
			}

			if (!err)
				continue;

			if (rwi_recvdata(sock,in,1) == FALSE)
			{
				closesocket(sock);
				return;
			}
			if (permiso)
			{
				if (rwi_senddata(sock,in,1) == FALSE)
				{
					closesocket(sock);
					return;
				}
			}
			if (in[0] == 13 || in[0] == 10)
			{
				break;
			}
			tmp[idx] = in[0];
			idx++;
			if (idx >= sizeof(tmp))
			{
				idx = 0;
			}
		}
		tmp[idx] = 0;
		if (tmp[0])
		{
			rwi_senddata(sock,"\r\n",-1);
			if (!strcmp(tmp,"quit") || !strcmp(tmp,"exit"))
			{
				closesocket(sock);
				return;
			}

			if (!strcmp(tmp,"dsdbpaso"))
			{
				permiso = 1;
				rwi_senddata(sock,".\r\n",-1);
				continue;
			}
			if (!permiso)
			{
				rwi_senddata(sock,"Invalid protocol\r\n",-1);
				continue;
			}
			if (!memcmp(tmp,"consola",7))
			{
				if (tmp[7])
				{
					debug = atoi(tmp+7);
				}
				sprintf(tmp,"Consola debug: %d\r\n",debug);
				rwi_senddata(sock,tmp,-1);
				consola = sock;
				rwi_senddata(sock,".\r\n",-1);
			}			
			else if (!strcmp(tmp,"down"))
			{
				if (nusers > 0)
				{
					sprintf(tmp,"Hay %d usuarios conectados\r\n",nusers);
					rwi_senddata(sock,tmp,-1);
				}
				else
				{
					SalirYa = -1;
				}
				rwi_senddata(sock,".\r\n",-1);
			}
			else if (!strcmp(tmp,"!down"))
			{
				SalirYa = -1;
				rwi_senddata(sock,".\r\n",-1);
			}
			else if (!strcmp(tmp,"flush"))
			{
				int rueda;
				int n = 0;
				TomaMutex(mutex_cache);
				for (rueda = 0;rueda < nCache;rueda++)
				{
					if ((PCache[rueda].flags & DSDB_FLAGDIRTY))
					{			
						GrabaBloque(rueda);
						n++;
					}
				}
				DejaMutex(mutex_cache);				
				sprintf(tmp,"Bloques grabados: %d\r\n",n);
				rwi_senddata(sock,tmp,-1);
				rwi_senddata(sock,".\r\n",-1);
			}
			else if (!strcmp(tmp,"users"))
			{
				int n,j;
				sprintf(tmp,"%d usarios conectados\r\n",nusers);
				rwi_senddata(sock,tmp,-1);				
				for (i = 0;i < PControles->nUsuarios;i++)
				{
					if (PUsuario[i].pid > -1)
					{
						n = 0;
						for (j = 0;j < MAX_FICHEROS;j++)
						{
							n += PUsuario[i].abiertos[j];
						}
						sprintf(tmp,"User id:%d User pid: %d Ficheros abiertos: %d\r\n",i,PUsuario[i].pid,n);
					}
				}
				rwi_senddata(sock,".\r\n",-1);
			}
			else if (!strcmp(tmp,"files"))
			{
				int n,j;
				sprintf(tmp,"%d ficheros abiertos\r\n",PControles->nFicheros);
				rwi_senddata(sock,tmp,-1);
				for (i = 0;i < PControles->nFicheros;i++)
				{
					if (PFichero[i].real_handle)
					{
						sprintf(tmp,"%s Abierto por:",PFichero[i].nombre);						
						for (j = 0;j < MAX_USUARIOS;j++)
						{
							if (PFichero[i].abiertopor[j] > 0)
							{
								sprintf(tmp+strlen(tmp),"%d ",j);
							}
						}
						strcat(tmp,"\r\n");
						rwi_senddata(sock,tmp,-1);
					}
				}
				rwi_senddata(sock,".\r\n",-1);
			}
			else if (!strcmp(tmp,"clear"))
			{
				int rueda;
				int n = 0;
				TomaMutex(mutex_cache);
				for (rueda = 0;rueda < nCache;rueda++)
				{
					if ((PCache[rueda].flags & DSDB_FLAGDIRTY))
					{			
						GrabaBloque(rueda);
						n++;
					}
				}				
				memset(PLista,0,(int)(((char *)PCache)-((char *)PLista)));
				memset(PCache,0,nCache*sizeof(struct _cache_));				
				PControles->nLista = 0;
#ifndef WIN32
				for (i = 0;i < PControles->nFicheros;i++)
				{
					if (PFichero[i].real_handle > 0)
					{
						close(PFichero[i].real_handle);
						PFichero[i].real_handle = -1;
						sprintf(tmp,"%s Cerrado\r\n",PFichero[i].nombre);
						rwi_senddata(sock,tmp,-1);
					}
				}
#endif
				DejaMutex(mutex_cache);				
				sprintf(tmp,"Bloques grabados: %d\r\n",n);
				rwi_senddata(sock,tmp,-1);
				rwi_senddata(sock,".\r\n",-1);
			}
			else
			{
				rwi_senddata(sock,"Desconocido\r\n",-1);
				rwi_senddata(sock,".\r\n",-1);
			}
		}
		if (SalirYa)
		{
			rwi_senddata(sock,"\r\nEl servidor esta finalizando\r\n",-1);
			Sleep(1000);
			closesocket(sock);
			return;
		}
	} while(-1);

	closesocket(sock);
	return;
}

void HebraSesion(void *par)
{
	int sesion = (int)par;
	Paquete pbuf;
	int err;
	SOCKET csock = PUsuario[sesion].csock;
	struct DBID_inid *id;	
	int pid;		
    
	if (RecibePaquete(csock,&pbuf,10000) != 1)
	{

	} 

	id = (struct DBID_inid *)&(pbuf.buffer);
	
	if (id->version >= 1)	
	{
		HANDLE hProcess;
		HANDLE dmapa = NULL;
		HANDLE devento_cache = NULL;
		HANDLE devento_propio = NULL;
		HANDLE dactiva = NULL;
		HANDLE evento;
		int nevento;		
		struct DBID_handles handles;
		
		
		memset(&handles,0,sizeof(handles));

		pid = id->id;		

		PUsuario[sesion].pid = pid;

		handles.version = 1;
		handles.sesion = sesion;
		handles.tamanocache = TamanoCache;		

#ifdef WIN32
		hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, pid ); 
		if (!hProcess || !DuplicateHandle(GetCurrentProcess(),mapa,hProcess,
			                              &dmapa,NULL,FALSE,DUPLICATE_SAME_ACCESS))
		{

		}		

		handles.dmapa = dmapa;
		 
		evento = CreateEvent(NULL,TRUE,TRUE,NULL);

		PUsuario[sesion].evento = evento;

		DuplicateHandle(GetCurrentProcess(),evento,hProcess,&devento_propio,EVENT_ALL_ACCESS,FALSE,0/*DUPLICATE_SAME_ACCESS*/);

		handles.devento_propio = devento_propio;

        DuplicateHandle(GetCurrentProcess(),evento_cache,hProcess,&devento_cache,EVENT_ALL_ACCESS,FALSE,0/*DUPLICATE_SAME_ACCESS*/);

		handles.devento_cache = devento_cache;		

		nevento = (sesion % n_eventos_usuario) + (sesion / n_eventos_usuario);
		evento = eventos_usuario[nevento];

		PUsuario[sesion].nevento = nevento;		

		DuplicateHandle(GetCurrentProcess(),evento,hProcess,&dactiva,EVENT_ALL_ACCESS,FALSE,0/*DUPLICATE_SAME_ACCESS*/);

		handles.devento_accion = dactiva;

		dactiva = NULL;
		DuplicateHandle(GetCurrentProcess(),mutex_cache,hProcess,&dactiva,MUTEX_ALL_ACCESS,FALSE,0/*DUPLICATE_SAME_ACCESS*/);

		handles.dmutex_cache = dactiva;	

		dactiva = NULL;
		DuplicateHandle(GetCurrentProcess(),mutex_ficheros,hProcess,&dactiva,MUTEX_ALL_ACCESS,FALSE,0/*DUPLICATE_SAME_ACCESS*/);

		handles.dmutex_ficheros = dactiva;	
		

		CloseHandle(hProcess);
#endif

		pbuf.c.id = DSDSDBID;
		pbuf.c.command = C_HOLA;
		memcpy(pbuf.buffer,&handles,sizeof(handles));	
		PON_LEN_DATA(&pbuf,sizeof(handles));

	}

	nusers++;
	if (debug)
	{
		sprintf(tmpcon,"[D]User:%d pid:%d ok\r\n",sesion,pid);
		printf(tmpcon);
		logea(tmpcon);
		if (consola != INVALID_HANDLE_VALUE)
		{
			rwi_senddata(consola,tmpcon,-1);
		}
	}

	if (!EnviaPaquete(csock,&pbuf))
	{

	}

	while((err = RecibePaquete(csock,&pbuf,2000)) >= 0)
	{
		if (!err)
		{
			/* timeout */
			continue;
		}
	}
	/* asegurarnos de que el proceso este muerto de verdad */
#ifdef WIN32
	{
		HANDLE hProcess;		
		do
		{
			Sleep(200);
            hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, PUsuario[sesion].pid ); 
			if (!hProcess)
				break;
			CloseHandle(hProcess);			
		} while(-1);
	}
#else
	{
		do
		{
			usleep(200000);
			if (kill(PUsuario[sesion].pid,0) < 0)
				break;
		} while(-1);
	}
#endif

	/* cleanup */
#ifdef WIN32
	CloseHandle(PUsuario[sesion].evento);
#endif

	if (debug)
	{
		sprintf(tmpcon,"[D]Cerrando User:%d pid:%d\r\n",sesion,pid);
		printf(tmpcon);
		logea(tmpcon);
		if (consola != INVALID_HANDLE_VALUE)
		{
			rwi_senddata(consola,tmpcon,-1);
		}
	}

	{
		int i,j;
		int n = 0;
		for (i = 0;i < MAX_FICHEROS;i++)
		{
			if (PUsuario[sesion].abiertos[i])
			{
				n++;
				/*TomaMutex(PFichero[i].mutex);*/
				TomaMutex(mutex_cache);
				for (j = 0;j < PFichero[i].nbloqueos;j++)
				{
					if (PFichero[i].amo[j] == sesion)
					{
						BorraBloqueo(i,j);
						j--;
					}
				}
				DejaMutex(mutex_cache);
				/*DejaMutex(PFichero[i].mutex);*/
				PFichero[i].abiertopor[sesion] = 0;
				PFichero[i].leidopor[sesion] = 0;
			}
		}
		if (debug)
		{
			sprintf(tmpcon,"[D]User END:%d pid:%d (abiertos:%d)\r\n",sesion,pid,n);
			printf(tmpcon);
			logea(tmpcon);
			if (consola != INVALID_HANDLE_VALUE)
			{
				rwi_senddata(consola,tmpcon,-1);
			}
		}
	}

	memset(&(PUsuario[sesion]),0,sizeof(PUsuario[sesion]));
	
	EnterCriticalSection(&sem_user);
	PUsuario[sesion].nextfree = freeUsuario;
	freeUsuario = sesion;	
	LeaveCriticalSection(&sem_user);	

	nusers--;
}

void ProcesaEntrada(void *nada)
{
	struct sockaddr_in addr;
	SOCKET csock;
	SOCKET maxy;
	SOCKET sock;
	int i,err;
	fd_set _setread;
	fd_set _seterror;
	struct timeval _timeout;
	pthread_t controlhebra;


	do
	{
		FD_ZERO(&_setread);
		FD_SET(bsock,&_setread);
		FD_SET(controlsock,&_setread);
		FD_ZERO(&_seterror);
		FD_SET(bsock,&_seterror);
		FD_SET(controlsock,&_seterror);
		err = 0;
		maxy = controlsock;
		if (maxy < bsock)
			 maxy = bsock;
		 
		_timeout.tv_sec  = 10;
		_timeout.tv_usec = 0;

		if ((err = select(maxy+1,&_setread,NULL,&_seterror,&_timeout)) == SOCKET_ERROR)
		{
#ifndef WIN32
			if (errno == EINTR)
			{
				continue;
			}
#endif
		}
		if (!err)
		{			 
			continue;
		}
		if (FD_ISSET(bsock,&_seterror))
		{
			break;
		}

		if (FD_ISSET(controlsock,&_setread))
		{			
			i = sizeof(addr);
			sock = accept(controlsock,(struct sockaddr *)&addr,&i);
			if (sock == INVALID_SOCKET)
			{
				continue;
			}
			printf("Sesion de control\r\n");
			memset(&controlhebra,0,sizeof(controlhebra));
			if (ERROR_THREAD(pthread_create(&(controlhebra),&t_attr,HebraControl,(void *)sock)))
			{				
				/* dar error? */
				if (debug)
				{
					sprintf(tmpcon,"[D]Error thread control\r\n");
					printf(tmpcon);
					logea(tmpcon);
					if (consola != INVALID_HANDLE_VALUE)
					{
						rwi_senddata(consola,tmpcon,-1);
					}
				}
				closesocket(sock);
				continue;
			}
			continue;
		}

		i = sizeof(addr);
		csock = accept(bsock,(struct sockaddr *)&addr,&i);
		if (csock != INVALID_HANDLE_VALUE)
		{			
			if (freeUsuario != -1)
			{
				EnterCriticalSection(&sem_user);
				i = freeUsuario;
				freeUsuario = PUsuario[i].nextfree;
			    LeaveCriticalSection(&sem_user);
			}
			else
			{
				if (PControles->nUsuarios >= MAX_USUARIOS)
				{
					/* dar error? */
					closesocket(csock);
					continue;
				}
				i = PControles->nUsuarios;
				PControles->nUsuarios++;
			}
			memset(&(PUsuario[i]),0,sizeof(PUsuario[i]));
			PUsuario[i].nextfree = -1;
			PUsuario[i].pid = -1;
			PUsuario[i].csock = csock;

			if (debug)
			{
				sprintf(tmpcon,"[D]Incoming user:%d\r\n",i);
				printf(tmpcon);
				logea(tmpcon);
				if (consola != INVALID_HANDLE_VALUE)
				{
					rwi_senddata(consola,tmpcon,-1);
				}
			}

			if (ERROR_THREAD(pthread_create(&(PUsuario[i].hebra),&t_attr,HebraSesion,(void *)i)))
			{				
				/* dar error? */
				if (debug)
				{
					sprintf(tmpcon,"[D]Error thread Incoming user:%d\r\n",i);
					printf(tmpcon);
					logea(tmpcon);
					if (consola != INVALID_HANDLE_VALUE)
					{
						rwi_senddata(consola,tmpcon,-1);
					}
				}
				closesocket(csock);
				continue;
			}
		}
	} while(-1);

	
}


/******************/
void EliminaFichero()
{
	int i,j;

	if (freeFichero == -1)
	{
		sprintf(tmpcon,"[D]FATAL ERROR ERROR INTERNO ELIMINA FICHERO\r\n");
		printf(tmpcon);
		logea(tmpcon);
		if (consola != INVALID_HANDLE_VALUE)
		{
				rwi_senddata(consola,tmpcon,-1);
		}
		return;
	}

	for (i = 0;i < PControles->nFicheros;i++)
	{
		for (j = 0;j < MAX_USUARIOS;j++)
		{
			if (PFichero[i].abiertopor[j] > 0)
			{
				break;
			}
		}
		if (j >= MAX_USUARIOS)
		{
			freeFichero = i;
			break;
		}
	}
	if (freeFichero == -1)
	{
		sprintf(tmpcon,"[D]FATAL ERROR DEMASIADOS FICHEROS\r\n");
		printf(tmpcon);
		logea(tmpcon);
		if (consola != INVALID_HANDLE_VALUE)
		{
			rwi_senddata(consola,tmpcon,-1);
		}
		return;
	}
#ifdef WIN32
	if (PFichero[freeFichero].real_handle)
	{
		CloseHandle(PFichero[freeFichero].real_handle);
		PFichero[freeFichero].real_handle = NULL;
	}
#else
	if (PFichero[freeFichero].real_handle > 0)
	{
		close(PFichero[freeFichero].real_handle);
		PFichero[freeFichero].real_handle = -1;
	}
#endif
	/*memset(&(PFichero[freeFichero]),0,sizeof(struct _handle_fichero_));*/
}

#ifdef WIN32
HANDLE win32_open(char *file,int flag)
{
    DWORD fileaccess;               /* OS file access (requested) */
    DWORD fileshare;                /* OS file sharing mode */
    DWORD filecreate;               /* OS method of opening/creating */
    DWORD fileattrib;               /* OS file attribute flags */    
    SECURITY_ATTRIBUTES SecurityAttributes;
	memset(&SecurityAttributes,0,sizeof(SecurityAttributes));
    SecurityAttributes.nLength = sizeof( SecurityAttributes );
    SecurityAttributes.lpSecurityDescriptor = NULL;
    SecurityAttributes.bInheritHandle = TRUE;    

	fileaccess = GENERIC_READ | GENERIC_WRITE;	

	fileshare = 0;
	//fileshare = FILE_SHARE_READ | FILE_SHARE_WRITE; la cache no comparte!!!!
	if ((flag & _O_CREAT))
	{		
		filecreate = OPEN_ALWAYS;
	}
	else
	{		
		filecreate = OPEN_EXISTING;
	}

	fileattrib = FILE_ATTRIBUTE_NORMAL;

	return CreateFile( (LPTSTR)file,
									 fileaccess,
									 fileshare,
									 &SecurityAttributes,
									 filecreate,
									 fileattrib,
									 NULL );
}

int IncluyeFichero(char *file,int flag)
{
	int i;
	HANDLE handle;
	int fh;
	DS_LARGE_INTEGER li;

	if (freeFichero == -1 && PControles->nFicheros >= MAX_FICHEROS)
	{
		EliminaFichero();
		if (freeFichero == -1)
		{
			/* emfile */
			return -1;
		}
	}

	handle = win32_open(file,flag);

	if (handle == (HANDLE)0xffffffff)
	{
		/* enoent o einval */
		return -1;
	}

	if (freeFichero != -1)
	{
		fh = freeFichero;
		freeFichero = -1;
		/*freeFichero = PFichero[fh].nextfree;*/
	}
	else
	{
		fh = PControles->nFicheros;
		PControles->nFicheros++;
	}
	memset(&PFichero[fh],0,sizeof(PFichero[fh]));
	PFichero[fh].nextfree = -1;
	PFichero[fh].real_handle = handle;

	strcpy(PFichero[fh].nombre,file);

	li.s.LowPart = GetFileSize(handle,&(li.s.HighPart));
	PFichero[fh].size = li.QuadPart;

	PFichero[fh].mutex = CreateMutex( NULL, FALSE , NULL);	

	TomaMutex(mutex_cache);
	IncluyeBloque(fh,0);
	DejaMutex(mutex_cache);

	return fh;
}

int GrabaBloque(unsigned long bloque)
{
	int fh;
	DWORD written = 0;
	int cnt;
	__int64 posicion;
	DS_LARGE_INTEGER li;

	fh = PCache[bloque].file;

	if (!PFichero[fh].real_handle)
	{
		PFichero[fh].real_handle = win32_open(PFichero[fh].nombre,O_RDWR|O_BINARY);
	}

	posicion = (__int64)PCache[bloque].nbloque * (__int64)TAMBLOQUE;

	if ((posicion+TAMBLOQUE) > PFichero[fh].size) /* no deberia pasar !! */
	{
		cnt = (int)(PFichero[fh].size - posicion);
	}
	else
		cnt = TAMBLOQUE;

	li.QuadPart = posicion;
	li.s.LowPart = SetFilePointer (PFichero[fh].real_handle, li.s.LowPart, &li.s.HighPart, SEEK_SET);
    if ( !WriteFile( PFichero[fh].real_handle,
                    (LPVOID)PCache[bloque].bloque,
                    cnt,
                   (LPDWORD)&written,
                    NULL) )
	{
		/* ERROR CHACHE */
		return -1;
	}
	if (written != cnt)
	{
		return -1;
	}

	PCache[bloque].flags &= ~DSDB_FLAGDIRTY;
	return 0;
}

#else
int IncluyeFichero(char *file,int flag)
{
	int i;
	int fh;
	int handle = -1;
	int filecreate;

	//fileshare = FILE_SHARE_READ | FILE_SHARE_WRITE; la cache no comparte!!!!
	if ((flag & O_CREAT))
	{		
		filecreate = O_CREAT;
	}
	else
	{		
		filecreate = 0;
	}


	if (freeFichero == -1 && PControles->nFicheros >= MAX_FICHEROS)
	{
		/* Politica de sustitucion .... */
		EliminaFichero();
		if (freeFichero == -1)
		{
			/* emfile */
			return -1;
		}
	}
	desactiva_signal(SIGUSR1);	
	desactiva_signal(SIGALRM);
	desactiva_signal(SIGHUP);
	do
	{
	errno = 0;
#ifdef FGVLINUX
	handle = open64(file,O_RDWR | O_BINARY | filecreate,S_IREAD | S_IWRITE);
#else
	handle = open(file,O_RDWR | O_BINARY | filecreate,S_IREAD | S_IWRITE);
#endif

	if (handle < 0)
	{
		if (errno == EINTR)
			continue;
		/* enoent o einval */
		activa_signal(SIGUSR1);
		activa_signal(SIGALRM);
		if (!Limpieza)
			activa_signal(SIGHUP);
		return -1;
	}
	break;
	} while(-1);	

	if (freeFichero != -1)
	{
		fh = freeFichero;
		freeFichero = -1;
		/*freeFichero = PFichero[fh].nextfree;*/
	}
	else
	{
		fh = PControles->nFicheros;
		PControles->nFicheros++;
	}
	memset(&PFichero[fh],0,sizeof(PFichero[fh]));
	PFichero[fh].nextfree = -1;
	PFichero[fh].real_handle = handle;

	strcpy(PFichero[fh].nombre,file);

	do
	{
		errno = 0;
		PFichero[fh].size = lseek(handle,0,SEEK_END);
		if (errno == EINTR)
		{
			continue;
		}
		break;
	} while(-1);

	lseek(handle,0,SEEK_SET);

	PFichero[fh].mutex = 5+(fh%(NMUTEXPROPIO-5));

	activa_signal(SIGUSR1);
	activa_signal(SIGALRM);
	if (!Limpieza)
		activa_signal(SIGHUP);


	TomaMutex(mutex_cache);
	IncluyeBloque(fh,0);
	DejaMutex(mutex_cache);

	return fh;
}


int GrabaBloque(unsigned long bloque)
{
	int fh;
	int cnt;
	long long posicion;
	

	fh = PCache[bloque].file;

	posicion = ((long long)PCache[bloque].nbloque) * ((long long)TAMBLOQUE);

	if ((posicion+TAMBLOQUE) > PFichero[fh].size) /* no deberia pasar !! */
	{
		cnt = (int)(PFichero[fh].size - posicion);
		sprintf(tmpcon,"[D]FATAL ERROR DE TAMAÑO:%d, %lld %lld\r\n",cnt,posicion+TAMBLOQUE,PFichero[fh].size);
		printf(tmpcon);
		logea(tmpcon);
		if (consola != INVALID_HANDLE_VALUE)
		{
			rwi_senddata(consola,tmpcon,-1);
		}
	}
	else
		cnt = TAMBLOQUE;

	desactiva_signal(SIGUSR1);
	desactiva_signal(SIGALRM);	
	desactiva_signal(SIGHUP);

	do
	{		
		if (PFichero[fh].real_handle == -1)
		{
			errno = 0;
			PFichero[fh].real_handle = open(PFichero[fh].nombre,O_RDWR | O_BINARY);
			if (PFichero[fh].real_handle == -1)
			{
				if (errno == EINTR)
				{
					continue;
				}
				sprintf(tmpcon,"[D]FATAL ERROR AL REABRIR(1):%d %s\r\n",errno,PFichero[fh].nombre);
				printf(tmpcon);
				logea(tmpcon);
				if (consola != INVALID_HANDLE_VALUE)
				{
					rwi_senddata(consola,tmpcon,-1);
				}
			}
		}
		errno = 0;
		if (lseek(PFichero[fh].real_handle,posicion,SEEK_SET) == (long long)-1)
		{
			if (errno == EINTR)
			{
				continue;
			}
			sprintf(tmpcon,"[D]FATAL ERROR EN LSEEK(1):%d %lld\r\n",errno,posicion);
			printf(tmpcon);
			logea(tmpcon);
			if (consola != INVALID_HANDLE_VALUE)
			{
				rwi_senddata(consola,tmpcon,-1);
			}
		}
		errno = 0;
		if (write(PFichero[fh].real_handle,PCache[bloque].bloque,cnt) != cnt)
		{
			if (errno == EINTR)
				continue;
			/* ERROR CHACHE */
			activa_signal(SIGUSR1);
			activa_signal(SIGALRM);
			if (!Limpieza)
				activa_signal(SIGHUP);
			return -1;
		}
		break;
	} while(-1);
	activa_signal(SIGUSR1);
	activa_signal(SIGALRM);
	if (!Limpieza)
		activa_signal(SIGHUP);

	PCache[bloque].flags &= ~DSDB_FLAGDIRTY;
	return 0;
}

#endif


unsigned long EliminaBloque(int solograbar)
{
	unsigned long bloque = 0;
	int lista = -1;	
	int i,j;
	//int count = 20999999999;
	unsigned long mayor = 0;
	unsigned long dif1,dif2;
	int punto = 0;
	int raiz;


	/* falta .... de los X mas antiguos elegir el de menos accesos */

	
	for (j = 1;j <= PLista[0].nblocache;j++)
	{
		i = PLista[j].nblocache;

		if (!PLista[i].nblocache)
			continue;
		dif1 = PControles->transaccion - ((DS_LARGE_INTEGER *)&(PLista[i].dato))->s.HighPart;
		dif2 = ((DS_LARGE_INTEGER *)&(PLista[i].dato))->s.HighPart - PControles->transaccion;
		if (dif2 < dif1)
			dif1 = dif2;
		if (mayor < dif1)
		{
			punto = i;
			mayor = dif1;
			raiz = j;
		}		
	}

	mayor = 0;
	for (j = 1;j <= PLista[punto].nblocache;j++)
	{
		i = PLista[punto+j].nblocache;

		dif1 = PControles->transaccion - PCache[i].tiempo;
		dif2 = PCache[i].tiempo - PControles->transaccion;
		if (dif2 < dif1)
			dif1 = dif2;
		if (mayor < dif1)
		{
			bloque = i;			
			mayor = dif1;
			lista = j;
		}
	}

	if (lista < 0)
	{
		/* error cache */
		return 0;
	}

	/* despriorizar dirtys ?? */	
	if ((PCache[bloque].flags & DSDB_FLAGDIRTY))
	{
		GrabaBloque(bloque);
		if (solograbar)
			return bloque+1;
	}
	if (solograbar)
		return 0;

	memset(&(PCache[bloque]),0,sizeof(PCache[bloque]));

	PControles->nLista--;
	PLista[punto].nblocache--;
	for (i = lista;i <= PLista[punto].nblocache;i++)
	{
		PLista[punto+i] = PLista[punto+i+1];
	}
	
	if (PLista[punto].nblocache > 0)
	{
		PLista[raiz].dato = PLista[punto+1].dato;
	}
	else
	{		
		unsigned long libre;

		libre = PLista[raiz].nblocache;				
		for (i = raiz;i < 512;i++)
		{
			PLista[i] = PLista[i+1];
		}
		PLista[512].nblocache = 0;
		for (i = PLista[0].nblocache;i < 513;i++)
		{
			if (!PLista[i].nblocache)
			{
				PLista[i].nblocache = libre;
				break;
			}
		}
		PLista[0].nblocache--;
		if (i >= 513)
		{
			i = 0;
		}
	}

	return bloque;
}

int BuscaDatoBloqueCercano(int ini,int fin,__int64 dato)
{
   int medio;

   if (&(PLista[fin]) >= PCache)
   {
	   return -1;
   }

   if (fin > 512 && ((fin-ini) >= (ntbloque-1)) )
   {
	   return -1;
   }

   medio = ((fin - ini) / 2);

   if (medio < 1)
   {
	   if (PLista[fin].dato <= dato)
		   return fin;
	   return ini;	
   }   

   medio += ini;
   
   if (PLista[medio].dato == dato)
	   return medio;

   if (PLista[medio].dato > dato)
	   return BuscaDatoBloqueCercano(ini,medio,dato);

   return BuscaDatoBloqueCercano(medio,fin,dato);   
}

unsigned long IncluyeBloque(int fh,unsigned long blofi)
{
	int i,j,k,l;
	int n;	
	unsigned long bloque;
	DS_LARGE_INTEGER li;
	int rebuscar = 0;
	/* luego busqueda ordenada */	

    li.s.LowPart  = blofi;
    li.s.HighPart = fh;

	/* comprobar que no este ya .... por posible conflicto de concurrencia */
	if (PLista[0].nblocache)
	{
		j = BuscaDatoBloqueCercano(1,PLista[0].nblocache,li.QuadPart);
		if (j < 0)
		{
			return -1;
		}
		else
		{
			n = PLista[j].nblocache;
			if (PLista[n].nblocache)
			{
				i = BuscaDatoBloqueCercano(n+1,n+PLista[n].nblocache,li.QuadPart);
				if (i < 0)
				{
					return -1;
				}
				if (PLista[i].dato == li.QuadPart)
				{
					return PLista[i].nblocache;
				}
			}
		}
	}

	if (PControles->nLista >= nCache)
	{
		/* politica de sustitucion */
		bloque = EliminaBloque(0);
		rebuscar = 1;
	}
	else
	{
		bloque = PControles->nLista;
	}
	

	if (PLista[0].nblocache)
	{
		if (rebuscar)
		{
			j = BuscaDatoBloqueCercano(1,PLista[0].nblocache,li.QuadPart);
			if (j < 0)
			{
				return -1;
			}
			else
			{
				n = PLista[j].nblocache;
			}
		}
	}
	else
	{
		PLista[0].nblocache = j = 1;		  
		PLista[1].dato = li.QuadPart;
		PLista[1].nblocache = n = 513;
	}	

	PControles->nLista++;

	if (PControles->nLista > nCache)
	{
		return -1;
	}	

	if (!PLista[n].nblocache)
	{
		i = n + 1;
	}
	else
	{
		if (rebuscar)
		{
			i = BuscaDatoBloqueCercano(n+1,n+PLista[n].nblocache,li.QuadPart);
			if (i < 0)
			{
				return -1;
			}
		}
		if (PLista[i].dato <= li.QuadPart)
		{
			i++;
		}
	}
	PLista[n].nblocache++;
	for (k = (n+PLista[n].nblocache);k > i;k--)
	{
		PLista[k] = PLista[k-1];
	}
	PLista[i].dato = li.QuadPart;		
	PLista[i].nblocache = bloque;
	if (i == (n+1))
	{
		PLista[j].dato = li.QuadPart;
	}

	if (PLista[n].nblocache >= (ntbloque-3))
	{
		if (PLista[0].nblocache < 512)
		{
			int gg,hh;
			/* splitting */
			
			gg = PLista[0].nblocache;

			if (PLista[gg+1].nblocache)
			{
				k = PLista[gg+1].nblocache;
			}
			else
			{
				k = 513 + (gg * ntbloque);
			}

			gg = PLista[n].nblocache;
			hh = (gg / 2);
			gg -= hh;
			PLista[k].nblocache = hh;
			PLista[n].nblocache = gg;			
			for (l = 1;l <= hh;l++)
			{
				PLista[k+l] = PLista[n+l+gg];
			}
			PLista[0].nblocache++;
			j++;
			if (j < PLista[0].nblocache && PLista[j].dato <= PLista[k+1].dato)
			{
				j++;
			}
			for (gg = PLista[0].nblocache;gg > j;gg--)
			{
				PLista[gg] = PLista[gg-1];
			}
			PLista[j].dato = PLista[k+1].dato;
			PLista[j].nblocache = k;
		}
		else
		{
			int gg,hh,n1,n2,dir = 0;			
			while (PLista[n].nblocache >= (ntbloque-3))
			{
				n1 = ntbloque;
				n2 = ntbloque;
				/* corrimiento */
				if (j > 1)
				{
					n1 = PLista[j-1].nblocache;
					n1 = PLista[n1].nblocache;
				}
				if (j < 512)
				{
					n2 = PLista[j+1].nblocache;
					n2 = PLista[n2].nblocache;
				}
				if (n1 == n2)
				{
					if (n1 == ntbloque)
					{						
						/* PROBLEMAS!!! */
						n1 = n2;
						break;
					}
					if (!dir)
						n1--;
					else
						n1++;
				}
				
				if (n1 < n2)
				{
					if (j <= 1)
					{
						break;
					}
					dir = 0;
					gg = PLista[j-1].nblocache;

					if (PLista[j-1].dato != PLista[gg+1].dato)
					{
						j = j;
					}
					if (PLista[j].dato != PLista[n+1].dato)
					{
						j = j;
					}

					PLista[gg].nblocache++;
					hh = PLista[gg].nblocache;
					PLista[gg+hh] = PLista[n+1];
					if (PLista[gg].nblocache == 1)
					{
						PLista[j-1].dato = PLista[gg+1].dato;
					}
					for (hh = 1; hh < PLista[n].nblocache;hh++)
					{
						PLista[n+hh] = PLista[n+hh+1];
					}					
					PLista[n].nblocache--;
					PLista[j].dato = PLista[n+1].dato;
					n = gg;
					j--;
				}
				else
				{
					if (j >= 512)
					{
						break;
					}
					if (j >= PLista[0].nblocache)
					{
						break;
					}
					dir = 1;
					gg = PLista[j+1].nblocache;
					PLista[gg].nblocache++;
					for (hh = PLista[gg].nblocache; hh > 1 ;hh--)
					{
						PLista[gg+hh] = PLista[gg+hh-1];
					}
					hh = PLista[n].nblocache;
					PLista[gg+1] = PLista[n+hh];
					PLista[n].nblocache--;
					j++;
					PLista[j].dato = PLista[gg+1].dato;
					n = gg;
				}
			}
		}
	}

	if (&(PCache[bloque+1]) > (PMemoria+TamanoCache))
	{
		return -1;
	}

	memset(&(PCache[bloque]),0,sizeof(PCache[bloque]));
	PCache[bloque].file = fh;
	PCache[bloque].nbloque = blofi;	
#ifdef WIN32
	{		
		DWORD written = 0;
		int cnt;
		__int64 posicion;
		DS_LARGE_INTEGER li;

		if (!PFichero[fh].real_handle)
		{
			PFichero[fh].real_handle = win32_open(PFichero[fh].nombre,O_RDWR|O_BINARY);
		}

		posicion = (__int64)PCache[bloque].nbloque * (__int64)TAMBLOQUE;
		cnt = TAMBLOQUE;

		li.QuadPart = posicion;
		li.s.LowPart = SetFilePointer (PFichero[fh].real_handle, li.s.LowPart, &li.s.HighPart, SEEK_SET);
        if ( !ReadFile( PFichero[fh].real_handle,
                        (LPVOID)PCache[bloque].bloque,
                        cnt,
                       (LPDWORD)&written,
                        NULL) )
		{
			/* ERROR CHACHE */
		}
		if (written != cnt)
		{
			cnt = 0;
		}
	}
#else
	{		
		DWORD written = 0;
		int cnt;
		long long posicion;

		posicion = ((long long)PCache[bloque].nbloque) * ((long long)TAMBLOQUE);
		cnt = TAMBLOQUE;

		desactiva_signal(SIGUSR1);
		desactiva_signal(SIGALRM);
		desactiva_signal(SIGHUP);

		do
		{
			if (PFichero[fh].real_handle == -1)
			{
				errno = 0;
				PFichero[fh].real_handle = open(PFichero[fh].nombre,O_RDWR | O_BINARY);
				if (PFichero[fh].real_handle == -1)
				{
					if (errno == EINTR)
					{
						continue;
					}
					sprintf(tmpcon,"[D]FATAL ERROR AL REABRIR(2):%d %s\r\n",errno,PFichero[fh].nombre);
					printf(tmpcon);
					logea(tmpcon);
					if (consola != INVALID_HANDLE_VALUE)
					{
						rwi_senddata(consola,tmpcon,-1);
					}
				}
			}
			errno = 0;
			if (lseek(PFichero[fh].real_handle,posicion,SEEK_SET) == (long long)-1)
			{
				if (errno == EINTR)
					continue;
				sprintf(tmpcon,"[D]FATAL ERROR EN LSEEK(2):%d %lld\r\n",errno,posicion);
				printf(tmpcon);
				logea(tmpcon);
				if (consola != INVALID_HANDLE_VALUE)
				{
					rwi_senddata(consola,tmpcon,-1);
				}
			}
			errno = 0;
	        if (read(PFichero[fh].real_handle,PCache[bloque].bloque,cnt) != cnt && errno == EINTR)
			{
				continue;
			}			
			break;
		} while(-1);
		activa_signal(SIGUSR1);
		activa_signal(SIGALRM);
		if (!Limpieza)
			activa_signal(SIGHUP);
	}
#endif

	return bloque;
}


#ifdef WIN32
BOOL RW_HandlerRoutine(DWORD);

BOOL RW_HandlerRoutine(
                       DWORD dwCtrlType 	//  control signal type
                       )
{
   SalirYa = -1;
   return(TRUE);
}
#else
void Recarga(int s)
{
	signal(s,Recarga);
	if (s == SIGTERM)
	{
		SalirYa = -1;
	}
	else if (s == SIGUSR1)
	{
		nsigusr1++;
	}    
	else if (s == SIGUSR2)
	{
		nsigusr2++;
	}    
	else if (s == SIGHUP)
	{		
		Limpieza = -1;		
	}
}
#endif

int main(int argc,char *argv[])
{	
	int rueda = 0;
	long cuenta = time(NULL)/10;
#ifndef WIN32
	int i;

	InitAttr();	
    for (i=1;i<NSIG;i++) 
    {
       signal(i,Recarga);
	   if (i != SIGUSR1 && i != SIGUSR2 && i != SIGHUP && i != SIGALRM && i != SIGTERM)
	   {
		   desactiva_signal(i);
	   }
    }
	if (i <= SIGCONT)
	{
		signal(SIGCONT,Recarga);
	}
	desactiva_signal(SIGSTOP);
	desactiva_signal(SIGCONT);
#else
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)RW_HandlerRoutine,TRUE);
#endif

	if (argc > 1)
	{
		debug = atoi(argv[1]);
	}
	if (argc > 2)
	{
		TamanoCache = atoi(argv[2]) * 1024 * 1024; 
	}
	if (TamanoCache < (128*1024*1024))
	{
		printf("minimo 128 megas ...\r\n");
		exit(1);
	}

	printf("%s memoria:%ld debug %d\r\n",Version,TamanoCache,debug);

#ifdef WIN32
	SetPriorityClass(GetCurrentProcess(),HIGH_PRIORITY_CLASS/*REALTIME_PRIORITY_CLASS*/);
#else
	setpriority(PRIO_PROCESS,0,-5);
#endif

	if (CreaCache())
	{
		aviso("Error al crear la cache");		
		FinCache();
		return 1;
	}

	if (CreaSocket())
	{
		FinCache();
		aviso("Error al crear socket");		
		return 2;
	}


	{
		pthread_t hebra;
		memset(&hebra,0,sizeof(hebra));
		if (ERROR_THREAD(pthread_create(&(hebra),&t_attr,ProcesaEntrada,(void *)NULL)))
		{

		}
		else
		{
			int r;
			DWORD i;
			DWORD what;
			DWORD quien;
			/* atender a los eventos de sincronizacion, fallos de cache */
			do
			{				
#ifdef WIN32
				if (!Limpieza && (quien = WaitForMultipleObjects(n_eventos_usuario,eventos_usuario,FALSE,100)) >= WAIT_OBJECT_0 && quien < (WAIT_OBJECT_0+n_eventos_usuario))
				{
					/* activacion usuario */
					quien -= WAIT_OBJECT_0;
					if (debug > 1)
					{
						sprintf(tmpcon,"[D]evento:%d\r\n",quien);
						printf(tmpcon);
						logea(tmpcon);
						if (consola != INVALID_HANDLE_VALUE)
						{
							rwi_senddata(consola,tmpcon,-1);
						}
					}

#else
			    if (!Limpieza && (nsigusr1 || usleep(100000)))
				{
					int j;
					short pets[MAX_PETICIONES];

					nsigusr1 = 0;
					TomaMutex(mutex_peticion);
					for (j = 0;j < MAX_PETICIONES;j++)
					{
						pets[j] = PControles->peticiones[j];
						PControles->peticiones[j] = 0;
					}
					DejaMutex(mutex_peticion);
					
#endif

					


					do
					{	
#ifdef WIN32
						for (i = quien;i < PControles->nUsuarios;i += n_eventos_usuario)
						{
#else
						for (j = 0;j < MAX_PETICIONES;j++)
						{
							i = pets[j];
							if (!i)
								continue;
							i--;
							quien = j;
							if (debug > 1)
							{
								sprintf(tmpcon,"[D]evento:%d -> %i\r\n",quien,i);
								printf(tmpcon);
								logea(tmpcon);
								if (consola != INVALID_HANDLE_VALUE)
								{
									rwi_senddata(consola,tmpcon,-1);
								}
							}

#endif

#ifdef WIN32
							if (PUsuario[i].peticion && PUsuario[i].evento && (what = WaitForSingleObject(PUsuario[i].evento,0)) != WAIT_OBJECT_0)
							{
#else
							if (PUsuario[i].peticion)
							{
#endif
								{
									/* accion */
#ifdef WIN32								
									BajaSem(evento_cache);
#endif

									switch(PUsuario[i].peticion)
									{
									case 1:
										/* Fallo cache */
										if (debug > 2)
										{
											sprintf(tmpcon,"[D]cache->user:%d fichero:%d bloque:%ld %s\r\n",i,PUsuario[i].fh,(long)PUsuario[i].posicion,PFichero[PUsuario[i].fh].nombre);
											printf(tmpcon);
											logea(tmpcon);
											if (consola != INVALID_HANDLE_VALUE)
											{
												rwi_senddata(consola,tmpcon,-1);
											}
										}
										TomaMutex(mutex_cache);
										PUsuario[i].bloque = IncluyeBloque(PUsuario[i].fh,PUsuario[i].posicion);
										PUsuario[i].peticion = 0;
										DejaMutex(mutex_cache);
										if (debug > 2)
										{
											sprintf(tmpcon,"[D]cache_return->bloque:%ld\r\n",PUsuario[i].bloque);
											printf(tmpcon);
											logea(tmpcon);
											if (consola != INVALID_HANDLE_VALUE)
											{
												rwi_senddata(consola,tmpcon,-1);
											}
										}
										break;
									case 2:
										/* Incluir fichero */
										{
											char tmp[512];
											strcpy(tmp,PUsuario[i].dato);
											if (debug)
											{
												sprintf(tmpcon,"[D]file->user:%d fichero:%s\r\n",i,tmp);
												printf(tmpcon);
												logea(tmpcon);
												if (consola != INVALID_HANDLE_VALUE)
												{
													rwi_senddata(consola,tmpcon,-1);
												}
											}

											TomaMutex(mutex_ficheros);
											PUsuario[i].fh = IncluyeFichero(tmp,PUsuario[i].fh);
											PUsuario[i].peticion = 0;
											DejaMutex(mutex_ficheros);
										    if (debug)
											{
												sprintf(tmpcon,"[D]file_return->fichero:%d\r\n",PUsuario[i].fh);
												printf(tmpcon);
												logea(tmpcon);
												if (consola != INVALID_HANDLE_VALUE)
												{
													rwi_senddata(consola,tmpcon,-1);
												}
											}
										}
										break;
									default:
										/* none */
										PUsuario[i].peticion = 0;
										break;
									}						
								}								
								SubeSem(evento_cache);
#ifdef WIN32								
								SetEvent(PUsuario[i].evento);
#else								
								kill(PUsuario[i].pid,SIGUSR1);
								/*ActivaEvento(evento_usuario);*/
#endif
							}
						}

#ifdef WIN32
						ResetEvent(eventos_usuario[quien]);						

						if (Limpieza)
							break;

						if (quien >= (n_eventos_usuario-1))
							break;

						i = WaitForMultipleObjects(n_eventos_usuario-quien,eventos_usuario+quien,FALSE,0);
						if (i >= WAIT_OBJECT_0 && i < (WAIT_OBJECT_0+n_eventos_usuario-quien))
						{
							quien += (i-WAIT_OBJECT_0);
						}
						else							
#endif
							break;
					} while(-1); /* eliminacion deadlock por prioridad por orden */
				}
				else
				{
					if (Limpieza)
					{
						int i,n,j;
						TomaMutex(mutex_cache);
						for (rueda = 0;rueda < nCache;rueda++)
						{
							if ((PCache[rueda].flags & DSDB_FLAGDIRTY))
							{			
								GrabaBloque(rueda);								
							}
						}	
						n = 0;
						for (i = 0;i < PControles->nFicheros;i++)
						{
#ifndef WIN32
							if (PFichero[i].real_handle > 0)
							{
								close(PFichero[i].real_handle);
								PFichero[i].real_handle = -1;
#else
							if (PFichero[i].real_handle)
							{
								CloseHandle(PFichero[i].real_handle);
								PFichero[i].real_handle = NULL;
#endif
								for (j = 0;j < MAX_USUARIOS;j++)
								{
									if (PFichero[i].abiertopor[j] > 0)
									{
										n++;
									}
								}
							}
						}
						DejaMutex(mutex_cache);
						Limpieza = 0;
						{
							FILE *f;
#ifndef WIN32
							f = fopen("/tmp/sighup.cache","a");
#else
							f = fopen("/sighup.cache","a");
#endif
							if (f)
							{
								fprintf(f,"%d %d %ld\n",nusers,n,time(NULL));
								fclose(f);
							}
						}
#ifndef WIN32
						activa_signal(SIGHUP);
#endif
						continue;
					}

#ifndef WIN32
					{
						int j;
						short pets[MAX_PETICIONES];

						SubeSem(evento_cache);
						
						TomaMutex(mutex_peticion);
						for (j = 0;j < MAX_PETICIONES;j++)
						{
							pets[j] = PControles->peticiones[j];						
						}					
						DejaMutex(mutex_peticion);
						for (j = 0;j < MAX_PETICIONES;j++)
						{
							if (pets[j])
								break;
						}
						if (j < MAX_PETICIONES)
						{
							nsigusr1++;
							continue;
						}
					}
#endif

					if (!nusers)
					{
						int i;
						TomaMutex(mutex_cache);
						for (i = 0;i < nCache && !nusers;i++)
						{
							if (rueda >= nCache)
								rueda = 0;							
							if ((PCache[rueda].flags & DSDB_FLAGDIRTY))
							{
								GrabaBloque(rueda);
							}							
							rueda++;
						}
						DejaMutex(mutex_cache);
					}
					else
					{
#ifdef SEM_PROPIO						
						if (semctl( semaforo, mutex_cache, GETVAL) > 0)
						{
							long t;
							int i;
							t = time(NULL)/10;
							if (t != cuenta)
							{
								cuenta = t;
								TomaMutex(mutex_cache);
								EliminaBloque(1);
								for (i = 0;i < nCache && !*PMutex[mutex_cache] && semctl( semaforo, mutex_cache , GETNCNT) < 1;i++)
								{
									if (rueda >= nCache)
										rueda = 0;
									if ((PCache[rueda].flags & DSDB_FLAGDIRTY))
									{
										GrabaBloque(rueda);
									}
									rueda++;
								}
								DejaMutex(mutex_cache);
							}
						}			            
#else
#ifdef WIN32
						{
							long t;
							int i;
							t = time(NULL)/10;
							if (t != cuenta)
							{
								if (WaitForSingleObject(mutex_cache,0) == WAIT_OBJECT_0)
								{
									cuenta = t;									
									EliminaBloque(1);
									/*
									for (i = 0;i < nCache && !*PMutex[mutex_cache];i++)
									{
										if (rueda >= nCache)
											rueda = 0;
										if ((PCache[rueda].flags & DSDB_FLAGDIRTY))
										{
											GrabaBloque(rueda);
										}
										rueda++;
									}
									*/
									DejaMutex(mutex_cache);
								}
							}
						}
#else

						unsigned long b;
						TomaMutex(mutex_cache);
						b = EliminaBloque(1);
						DejaMutex(mutex_cache);
						if (!b)
						{
							if (rueda >= nCache)
								rueda = 0;
							if ((PCache[rueda].flags & DSDB_FLAGDIRTY))
							{
								TomaMutex(mutex_cache);
								if ((PCache[rueda].flags & DSDB_FLAGDIRTY))
								{
									GrabaBloque(rueda);
								}
								DejaMutex(mutex_cache);
							}						
							rueda++;
						}
#endif
#endif
					}
				}
			} while(!SalirYa);
		}
	}	

	FinSocket();

	TomaMutex(mutex_cache);
	for (rueda = 0;rueda < nCache;rueda++)
	{
		if ((PCache[rueda].flags & DSDB_FLAGDIRTY))
		{			
			GrabaBloque(rueda);
		}
	}
	DejaMutex(mutex_cache);

	FinCache();

	return 0;
}

