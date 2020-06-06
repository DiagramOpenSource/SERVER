


#ifdef WIN32

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/locking.h>
#include <share.h>
#include <io.h>
#include <windows.h>
#include <windowsx.h>
#include <winsock.h>
#include <nspapi.h>

typedef union _DS_LARGE_INTEGER { 
    struct {
        unsigned long LowPart; 
        long  HighPart; 
    } s;
    __int64 QuadPart;
} DS_LARGE_INTEGER; 


#define usleep(a) Sleep(a/1000)

#define pthread_t unsigned long
#define pthread_create(a,b,c,d) *(a) = _beginthread(c,(unsigned)0,d)
#define ERROR_THREAD(a) ((a) == -1)
#define pthread_kill(a,b) ResumeThread(a)
#define DSsigwait(a,b,c)  SuspendThread(a)
#define sigemptyset(a) /* */
#define sigaddset(a,b) /* */


#define sleep(a) Sleep((long)(((long)a)*1000L));

#ifndef LK_UNLCK
#define LK_UNLCK _LK_UNLCK
#endif

#ifndef LK_LOCK
#define LK_LOCK  _LK_LOCK
#endif

#ifndef LK_NBLCK
#define LK_NBLCK _LK_NBLCK
#endif


#else

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
#include <signal.h>
#ifdef WATCOMNLM
#include <io.h>
#else
#include <unistd.h>
#endif

#include <time.h>
#include <sys/timeb.h>

typedef union _DS_LARGE_INTEGER { 
    struct {
        unsigned long LowPart; 
        long  HighPart; 
    } s;
    long long QuadPart;
} DS_LARGE_INTEGER; 

#ifndef MAP_NOSYNC
#define MAP_NOSYNC 0x0800
#endif

#ifndef MAP_HASSEMAPHORE
#define MAP_HASSEMAPHORE 0
#endif

#define DSsigwait(a,b,c)      sigwait(b,c)
#define ERRORWAIT(a) (a != 0)

#define SOCKET int
#define SOCKET_ERROR -1
#define INVALID_HANDLE_VALUE -1
#define closesocket close
#define ioctlsocket ioctl

#ifndef O_BINARY
#define O_BINARY 0
#endif

#define CRITICAL_SECTION pthread_rwlock_t
#define InitializeCriticalSection(a) pthread_rwlock_init(a,NULL)
#define EnterCriticalSection pthread_rwlock_wrlock
#define LeaveCriticalSection pthread_rwlock_unlock
#define DeleteCriticalSection pthread_rwlock_destroy

#define Sleep(a) usleep(a*1000)

static pthread_attr_t t_attr;

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

#define __int64 long long

#ifndef FALSE
#define FALSE               0
#endif

#ifndef TRUE
#define TRUE                1
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

#ifndef _LK_UNLCK
#define _LK_UNLCK LK_UNLCK
#endif

#ifndef _LK_LOCK
#define _LK_LOCK  LK_LOCK
#endif

#ifndef _LK_NBLCK
#define _LK_NBLCK LK_NBLCK
#endif


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

/*#include "C:\\fgv_10\\proyectos\\dsdbsrv\\dsdbsrv.h"*/
#include "dsdbsrv.h"

#ifndef WIN32
#define SEM_PROPIO
/*#define MUTEX_PROPIO*/

int *PMutex[NMUTEXPROPIO];

int yo_sem = 0;
int mipid = 0;
#endif

static long max_bloqueos = 0;

extern char *dsdb_serverip;

extern int nsigusr1;
extern int nsigusr2;

SOCKET sock = INVALID_HANDLE_VALUE;

/*static unsigned long bloquecero[MAX_FICHEROS];*/

HANDLE semaforo = -1; /* solo unix */
HANDLE mapa = NULL; /* Mapa virtual memoria de cache y de trasferencia */
HANDLE evento_propio = NULL;
HANDLE evento_accion = NULL;
HANDLE evento_cache  = NULL;
HANDLE mutex_cache   = NULL;
HANDLE mutex_ficheros= NULL;

HANDLE mutex_peticion = NULL;
HANDLE evento_usuario = NULL;

static int yo = -1;

static HANDLE mutex_lock[MAX_FICHEROS];

unsigned long TamanoCache = 0; // 256 megas 
unsigned char *PMemoria = NULL;

struct _controles_ *PControles = NULL;

struct _handle_usuario_ *PUsuarios = NULL;

struct _handle_usuario_ *PUsuario = NULL;
/*
int nUsuarios = 0;
int freeUsuario = -1;
*/


struct _handle_fichero_ *PFichero = NULL;
/*
int nFicheros = 0;
int freeFichero = -1;
*/

int nCache = 0;

struct _lista_cache_ *PLista = NULL;
struct _cache_ *PCache = NULL;

#define OutputDebugString(a) /* */



extern char rw_user[11];
extern char nomapli[25];
extern char modulo[9];

#include <rwmalloc.h>

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

	usleep(1);

	return;

	m = PMutex[evento];
	m[2+yo_sem] = mipid;		
	activa_signal(SIGUSR2);
	usleep(10000);
	desactiva_signal(SIGUSR2);
	nsigusr2 = 0;
	m[2+yo_sem] = 0;
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

static char tmpcon[256];

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
				/*
				if (PControles->_sema[mutex])
				{
					sprintf(tmpcon,"[D]EL SEMAFORO NO VA PARA NADA %d TomaMutex: %d\r\n",mutex,PControles->_sema[mutex]);
					aviso(0,tmpcon);
					continue;
				}
				*/
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
			operation[0].sem_flg =  SEM_UNDO| IPC_NOWAIT;

			errno = 0;
			e = semop( semaforo, operation, 1 );
			if (e >= 0)
				break;

		   if (semctl( semaforo, mutex , GETVAL) == 0)
		   {
#ifdef SEM_PROPIO
			activa_signal(SIGUSR2);
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
			desactiva_signal(SIGUSR2);
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
			aviso(0,tmpcon);
			continue;
		}
		/*
		if (e == 0)
		{
			if (pid != mipid)
			{
				sprintf(tmpcon,"[D]Error semaforo(2) %d TomaMutex:%d\r\n",mutex,errno);
				aviso(0,tmpcon);
				continue;
			}
		}
		*/
		if (e == 1)
			continue;
		if (e > 1)
		{
			sprintf(tmpcon,"[D]EL SEMAFORO NO VA BIEN %d TomaMutex:%d\r\n",mutex,e);
			aviso(0,tmpcon);
			DejaMutex(mutex);
			continue;
		}
		break;

		/*
		if (PControles->_sema[mutex])
		{
			sprintf(tmpcon,"[D]EL SEMAFORO NO VA PARA NADA %d TomaMutex: %d\r\n",mutex,PControles->_sema[mutex]);
			aviso(0,tmpcon);
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
		aviso(0,tmpcon);
	}

	PControles->_sema[mutex]--;

	m = PMutex[mutex];
	if (*m != mipid)
	{
		sprintf(tmpcon,"[D]Error semaforo %d DejaMutex(1):%d - %d\r\n",mutex,*m,mipid);
		aviso(0,tmpcon);		
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
		int n;
		int espera = 0;
		

		/*
		if (!(PControles->_sema[mutex]))
		{
			sprintf(tmpcon,"[D]Error semaforo %d DejaMutex(1):%d\r\n",mutex,errno);
			aviso(0,tmpcon);
		}
		*/

		PControles->_sema[mutex]--;
		/*
		if (PControles->_sema[mutex])
		{
			sprintf(tmpcon,"[D]EL SEMAFORO NO VA PARA NADA %d DejaMutex: %d\r\n",mutex,PControles->_sema[mutex]);
			aviso(0,tmpcon);
		}
		*/

		/*
		e = semctl( semaforo, mutex , GETVAL);
		if (e > 0)
		{
			sprintf(tmpcon,"[D]EL SEMAFORO NO VA PARA NADA(1) %d DejaMutex: %d\r\n",mutex,PControles->_sema[mutex]);
			aviso(0,tmpcon);
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
				aviso(0,tmpcon);
				continue;
			}			
			break;
		}		
		if (e == 0)
		{			
			if (semctl( semaforo, mutex , GETPID) == mipid)
			{
				sprintf(tmpcon,"[D]Error semaforo(2) %d DejaMutex:%d\r\n",mutex,errno);
				aviso(0,tmpcon);
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
				aviso(0,tmpcon);
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
			aviso(0,tmpcon);
		}		
	}
#endif
#endif
}

#ifndef WIN32

void PonPeticion()
{
	int i;
	int c;
			 /* esperar semaforo peticiones */
			 /* incluir la mia, si esta todo ocupado, reintentar de momento ningun
			    mecanismo anti dead lock, al ser x peticiones procesadas de golpe y misma
			    prioridad de procesos asumimos que no se dara */
			 /* activar semaforo de cache */			 
			 /* esperar un signal .. comprobar peticion */
	
	do
	{
		TomaMutex(mutex_peticion);
		for (i = 0;i < MAX_PETICIONES;i++)
		{
			if (!PControles->peticiones[i])
			{
				PControles->peticiones[i] = yo + 1;
				break;
			}
		}
		DejaMutex(mutex_peticion);
	} while(i >= MAX_PETICIONES);	

	activa_signal(SIGUSR1);

	kill(PControles->procesocache,SIGUSR1);					
	c = 0;
	do
	{
	  
	  i = PUsuario->peticion;
	  /*DejaMutex(mutex_cache);*/

	  if (!i)
	    break;	     

	  if (!nsigusr1)
	  {
		  usleep(10000);
	  }
	  nsigusr1 = 0;	  
	  /*TomaMutex(mutex_cache);*/
	  

	
	  /*usleep(1);*/
	  /*nsigusr1 = 0;*/
	} while(-1);
	desactiva_signal(SIGUSR1);
}
#endif

static void cache_debug_log(int cod)
{
	char tmp[512];
	FILE *f;

	strcpy(tmp,rw_directorio("logs/dsdbsrvapi.log"));
	f = fopen(tmp,"a");
	if (f)
	{
		extern char *fecha();			   
	    fprintf(f,"%s %s %s %s %s [%03d]\r\n",fecha(),fecha()+11,rw_user,nomapli,modulo,cod);
		fclose(f);
	}
}

static void info_cache_debug_log(int cod,char *mensa)
{
	char tmp[2048];
	FILE *f;

	strcpy(tmp,rw_directorio("logs/dsdbsrvapi.log"));
	f = fopen(tmp,"a");
	if (f)
	{
		extern char *fecha();			   
	    fprintf(f,"%s %s %s %s %s [%d] %s\r\n",fecha(),fecha()+11,rw_user,nomapli,modulo,cod,mensa);
		fclose(f);
	}
}


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
       i = send(sock,((char *)&(sendbuf->c)) + bytessent,totalbytes - bytessent,0);
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

	bytessent  = 0;	
	totalbytes = LEN_DATA(sendbuf);
	
	while(bytessent < totalbytes)
	{
	   i = send(sock,((char *)(sendbuf->buffer)) + bytessent,totalbytes - bytessent,0);
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

	readsize = LEN_CABECERA;	
	if((totalbytesread = recv(sock, (char *)&(recvbuf->c), readsize,MSG_WAITALL)) == SOCKET_ERROR)
	{		
		return SOCKET_ERROR;
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
		  if (errno != EWOULDBLOCK)
		  {
			  return SOCKET_ERROR;
		  }
#endif
		  continue;
		}
		totalbytesread += bytesread;
		readsize -= bytesread;
	}
	
	return 1;
}


int IniciaSesion()
{	
	Paquete paquete;	
	struct sockaddr_in addr;
	struct hostent *mihost;
	struct DBID_inid id;
	struct DBID_handles handles;

	memset((char *)&id,0,sizeof(id));
	id.version = 1;
#ifdef WIN32
	id.id = GetCurrentProcessId();
#else
    id.id = getpid();
#endif

    addr.sin_addr.s_addr = inet_addr(dsdb_serverip);
    if (addr.sin_addr.s_addr == INADDR_NONE)
    {
	   if ( (mihost = gethostbyname(dsdb_serverip)) && mihost->h_length == 4 && mihost->h_addr_list[0])
	   {	    
		   addr.sin_addr.s_addr = *((unsigned long *)mihost->h_addr_list[0]);
	   }
    }
	
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(DSDBPORT);

	sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

	FastSocket(sock);

    if (connect(sock,(struct sockaddr *) &addr,sizeof (addr)) == SOCKET_ERROR)
    {
		closesocket(sock);
		sock = INVALID_HANDLE_VALUE;
		cache_debug_log(1);
		return -1;
	}

	paquete.c.id = DSDSDBID;
	paquete.c.command = C_HOLA;
	memcpy(paquete.buffer,&id,sizeof(id));	
	PON_LEN_DATA(&paquete,sizeof(id));

	if (!EnviaPaquete(sock,&paquete))
	{
		closesocket(sock);
		sock = INVALID_HANDLE_VALUE;
		cache_debug_log(2);
		return -1;
	}

	if (RecibePaquete(sock,&paquete,10000) != 1)
	{
		closesocket(sock);
		sock = INVALID_HANDLE_VALUE;
		cache_debug_log(3);
		return -1;
	}

	/* Handles del mapa de memoria y de los eventos */
	memcpy(&handles,paquete.buffer,sizeof(handles));	
	TamanoCache   = handles.tamanocache;

#ifdef WIN32
	mapa = handles.dmapa;
	evento_propio = handles.devento_propio;
	evento_accion = handles.devento_accion;
	evento_cache  = handles.devento_cache;

	mutex_cache   = handles.dmutex_cache;

	mutex_ficheros= handles.dmutex_ficheros;	

	PMemoria = (unsigned char *)MapViewOfFile(mapa,FILE_MAP_WRITE,0,0,TamanoCache);
	if (!PMemoria)
	{
		closesocket(sock);
		sock = INVALID_HANDLE_VALUE;
		aviso(0,"No se puede mapear la memoria de la cache");
		cache_debug_log(4);
		return -1;
	}	
#else
	semaforo = semget( 1366, 5, 0666);
	if (semaforo < 0)
	{
		closesocket(sock);
		sock = INVALID_HANDLE_VALUE;
		aviso(0,"No se pueden mapear los semaforos");
		cache_debug_log(4);
		return -1;
	}

	/*
	mapa = shmget(1365, TamanoCache , 0666);
	if (mapa < 0)
	{
		closesocket(sock);
		sock = INVALID_HANDLE_VALUE;
		aviso(0,"No se recoger el mapa de la memoria de la cache");
		cache_debug_log(4);
		return -1;
	}	

	PMemoria = shmat(mapa, NULL, 0);
	*/
	mapa = open("/usr/prueba.mapa",O_RDWR);
	if (mapa < 0)
	{
		closesocket(sock);
		sock = INVALID_HANDLE_VALUE;
		aviso(0,"No se recoger el mapa de la memoria de la cache");
		cache_debug_log(4);
		return -1;
	}	
	PMemoria = mmap(0,TamanoCache,PROT_READ|PROT_WRITE,MAP_SHARED|MAP_NOSYNC|MAP_HASSEMAPHORE,mapa,0);
	if (!PMemoria)
	{
		closesocket(sock);
		sock = INVALID_HANDLE_VALUE;
		aviso(0,"No se puede mapear la memoria de la cache");
		cache_debug_log(4);
		return -1;
	}	

	mutex_cache    = 0;
	mutex_ficheros = 1;
	evento_cache   = 2;
	mutex_peticion = 3;
	evento_usuario = 4;

#endif

	nCache = TamanoCache - sizeof(struct _controles_);
	nCache -= sizeof(struct _handle_usuario_) * MAX_USUARIOS;
	nCache -= sizeof(struct _handle_fichero_) * MAX_FICHEROS;
	nCache -= 513 * sizeof(struct _lista_cache_);
	nCache /= (sizeof(struct _lista_cache_)*2 + sizeof(struct _cache_));

	PControles = (struct _controles_ *)PMemoria;
	PUsuarios = (struct _handle_usuario_ *)(((char *)PControles)+sizeof(struct _controles_));	
	PFichero = (struct _handle_fichero_ *)(((char *)PUsuarios)+sizeof(struct _handle_usuario_)*MAX_USUARIOS);
	PLista = (struct _lista_cache_ *)(((char *)PFichero)+sizeof(struct _handle_fichero_)*MAX_FICHEROS);

	PCache   = (struct _cache_ *)(((char *)PLista)+sizeof(struct _lista_cache_)*(513+nCache*2));

	if (&(PCache[nCache-1]) >= (PMemoria+TamanoCache))
	{
		cache_debug_log(5);
		return -1;
	}


	yo = handles.sesion;

	PUsuario = PUsuarios + yo;
#ifndef WIN32
	{
		int i;
		yo_sem = yo + 1;
		mipid = getpid();
		for (i = 0;i < NMUTEXPROPIO;i++)
		{
			PMutex[i] = &(PControles->mutex[(i*(MAX_USUARIOS+3))]);
		}
	}
#endif

	return 0;
}

int si_sleep()
{
	Sleep(1);
	return 1;
}

int BuscaFichero(const char *path)
{
	int i;
	int n = PControles->nFicheros;

	OutputDebugString("BuscaFichero\n");
	for (i = 0;i < n;i++)
	{
		if (!strcmp(PFichero[i].nombre,path))
		{
			return i;
		}
	}	
	OutputDebugString("FinBuscaFichero\n");
	
	return -1;
}

int BuscaDatoBloque(int ini,int fin,__int64 dato)
{
   int medio;

   if (&(PLista[fin]) >= PCache)
   {
	   cache_debug_log(6);
	   return -1;
   }

   medio = ((fin - ini) / 2);

   if (medio < 1)
   {
	   if (PLista[ini].dato == dato)
		   return ini;
	   if (PLista[fin].dato == dato)
		   return fin;
	   return -1;
   }   

   medio += ini;
   
   if (PLista[medio].dato == dato)
	   return medio;

   if (PLista[medio].dato < dato)
	   return BuscaDatoBloque(medio+1,fin,dato);

   return BuscaDatoBloque(ini,medio-1,dato);
}


int BuscaDatoBloqueCercano(int ini,int fin,__int64 dato)
{
   int medio;

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



unsigned long BuscaBloque(int fh,__int64 posicion,int *offset)
{
	int i,j,k;	
	DS_LARGE_INTEGER li;
	unsigned long bloque;
	__int64 posbloque;	
	/* luego busqueda ordenada */
	OutputDebugString("BuscaBloque\n");

	if (!PLista[0].nblocache)
		return 0;

	bloque = (unsigned long)(posicion / TAMBLOQUE);

	posbloque = ((__int64)bloque) * ((__int64)TAMBLOQUE);
	
    li.s.LowPart  = bloque;
    li.s.HighPart = fh;

	/*
	if (!bloque && bloquecero[fh])
	{
	    bloque = bloquecero[fh] - 1;
	    if (PCache[bloque].nbloque == li.s.LowPart && PCache[bloque].file == fh)
		{
			*offset = (int)(posicion - posbloque);
			PCache[bloque].tiempo = PControles->transaccion++;		
			PCache[bloque].count++;
			if (!PCache[bloque].count)
				PCache[bloque].count--;
			return (bloque+1);
		}		
		bloquecero[fh] = 0;
	}
	*/

	j = BuscaDatoBloqueCercano(1,PLista[0].nblocache,li.QuadPart);

	k = j;

	j = PLista[j].nblocache;


	i = BuscaDatoBloque(j+1,j+PLista[j].nblocache,li.QuadPart);	

	if (i >= 0)
	{
		*offset = (int)(posicion - posbloque);
		bloque = PLista[i].nblocache;
		if (bloque < 0 || bloque >= nCache || &(PCache[bloque]) >= (PMemoria+TamanoCache))
		{
			/*DebugBreak();*/
			cache_debug_log(7);
			return 0;
		}
		if (PCache[bloque].nbloque != li.s.LowPart)
		{
			/*DebugBreak();*/
			cache_debug_log(8);
			return 0;
		}
		/*
		if (!li.s.LowPart)
		{
			bloquecero[fh] = bloque+1;
		}
		*/
		PCache[bloque].tiempo = PControles->transaccion++;		
		li.QuadPart = PLista[j].dato;
		li.s.HighPart = PCache[bloque].tiempo;
		PCache[bloque].count++;
		if (!PCache[bloque].count)
			PCache[bloque].count--;
		if (li.s.LowPart < PCache[bloque].count)
			li.s.LowPart  = PCache[bloque].count;
		PLista[j].dato = li.QuadPart;
		OutputDebugString("FinOKBuscaBloque\n");
		return (bloque+1);
	}

	/*
	for (i = 0;i < n;i++)
	{
		if (li.QuadPart == PLista[i].dato)
		{
			*offset = (int)(posicion - posbloque);
			bloque = PLista[i].nblocache;
			PCache[bloque].tiempo = tiempo;
			PCache[bloque].count++;
			OutputDebugString("FinOKBuscaBloque\n");
			return (bloque+1);
		}
	}	
	*/
	OutputDebugString("FinNOOKBuscaBloque\n");
	return 0;
}

int BuscaBloqueo(int fh,__int64 posicion,__int64 nbytes,int *menor,int modo)
{
	int i,n;
	ds_off *pbloqueos;
	ds_off *phasta;
	int *pamo;
	ds_off fin = posicion + nbytes;

	OutputDebugString("BuscaBloqueo\n");

	n = PFichero[fh].nbloqueos;

	if (menor)
	{
		*menor = -1;
	}

	pbloqueos = &(PFichero[fh].bloqueos[0]);
	phasta    = &(PFichero[fh].hasta[0]);	
	pamo      = &(PFichero[fh].amo[0]);

	/* otra vez, falta buscar ordenadamente  */
	for (i = 0;i < n;i++)
	{
		if (modo == 2)
		{
			if (pamo[i] == yo && pbloqueos[i] == posicion && phasta[i] == fin)
			{
				return i;
			}
		}
		else
		{
			if (pbloqueos[i] <= posicion && phasta[i] > posicion)
			{
				OutputDebugString("FInokBuscaBloqueo\n");
				if (modo == 1 && pamo[i] == yo)
					return -2;
				if (pamo[i] == yo)
				{
					if (menor && pbloqueos[i] < posicion)
					{
						*menor = i;
					}
					return -1;
				}
				return i;
			}
			if (pbloqueos[i] >= posicion && pbloqueos[i] < fin)
			{
				OutputDebugString("FInokBuscaBloqueo\n");
				if (modo == 1 && pamo[i] == yo)
					return -2;
				if (pamo[i] == yo)
				{
					return -1;
				}
				return i;
			}
		}

		if (menor && pbloqueos[i] < posicion)
			*menor = i;
	}

	OutputDebugString("FInNOokBuscaBloqueo\n");
    return -1;
}

int BorraBloqueo(int fh,int idx,int amo)
{
	int i,n;
	ds_off *pbloqueos;
	ds_off *phasta;
	int *pamo;

	OutputDebugString("BorraBloqueo\n");
	if (PFichero[fh].nbloqueos <= 0)
	{
		if (amo < 0)
		{
			OutputDebugString("MALBorraBloqueo\n");
			cache_debug_log(9);
		}
		return 0;
	}

	if (amo < 0)
		(PFichero[fh].nbloqueos)--;

	n = PFichero[fh].nbloqueos;

	pbloqueos = &(PFichero[fh].bloqueos[0]);
	phasta    = &(PFichero[fh].hasta[0]);
	pamo      = &(PFichero[fh].amo[0]);
	
	if (amo > -1)
	{		
		for (idx = 0;idx < n;idx++)
		{
			if (pamo[idx] == amo)
			{
				for (i = idx;i < n;i++)
				{
					pbloqueos[i] = pbloqueos[i+1];
					phasta[i] = phasta[i+1];
					pamo[i] = pamo[i+1];
				}
				idx--;
				if (n)
				{
					(PFichero[fh].nbloqueos)--;
					n = PFichero[fh].nbloqueos;
				}
				if (!n)
					break;
			}
		}
	}
	else
	{
		for (i = idx;i < n;i++)
		{
			pbloqueos[i] = pbloqueos[i+1];
			phasta[i] = phasta[i+1];
			pamo[i] = pamo[i+1];
		}
	}
	OutputDebugString("FinBorraBloqueo\n");
	return 1;
}

int PonBloqueo(int fh,int menor,__int64 posicion,__int64 nbytes)
{
	int i,n;
	ds_off *pbloqueos;
	ds_off *phasta;
	int *pamo;
	ds_off fin = posicion + nbytes;

	OutputDebugString("PonBloqueo\n");

	n = PFichero[fh].nbloqueos;
	if (n >= MAXBLOQUEOS)
	{
		/* max lock */
		OutputDebugString("FinBloqueoMAXBLOQUEOS\n");
		cache_debug_log(10);
		return -1;
	}
	PFichero[fh].nbloqueos++;

	if (max_bloqueos < PFichero[fh].nbloqueos)
	{
		max_bloqueos = PFichero[fh].nbloqueos;
	}

	pbloqueos = &(PFichero[fh].bloqueos[0]);
	phasta    = &(PFichero[fh].hasta[0]);
	pamo      = &(PFichero[fh].amo[0]);

	menor++;
	for (i = n;i > menor;i--)
	{
		pbloqueos[i] = pbloqueos[i-1];
		phasta[i] = phasta[i-1];
		pamo[i] = pamo[i-1];
	}

	pbloqueos[menor] = posicion;
	phasta[menor] = fin;
	pamo[menor] = yo;

	OutputDebugString("FinBloqueo\n");
	return 0;
}

__int64 dsdbcache_size(int fh)
{
	 __int64 size;
	 OutputDebugString("dsdbcache_size\n");

	 if (fh < 0 || fh >= MAX_FICHEROS)
	 {
		  cache_debug_log(11);
		  return (__int64)-1;
	 }

	 if (!PMemoria)
	 {
		/* CACHE_ERROR */
		 aviso(0,"No pmemoria");
		cache_debug_log(12);
		return (__int64)-1;
	 }

	 if (!PFichero[fh].abiertopor[yo])
	 {
		 /* CACHE_ERROR */
		 cache_debug_log(13);
		return (__int64)-1;
	 }

	 /*TomaMutex(mutex_lock[fh]);*/
	 TomaMutex(mutex_cache);
	 size = PFichero[fh].size;
	 DejaMutex(mutex_cache);
	 /*DejaMutex(mutex_lock[fh]);*/

	 return size;
}

int dsdbcache_lee(int fh,void *buf,__int64 *posicion,unsigned int cnt)
{
	unsigned long bloque;
	int offset = 0;
	int i;
	int control = 0;
	int primero = 1;
	OutputDebugString("lee\n");
	 if (fh < 0 || fh >= MAX_FICHEROS)
	 {
		 cache_debug_log(14);
		  return -1;
	 }
	 if (!PMemoria)
	 {
		 aviso(0,"No pmemoria");
		/* CACHE_ERROR */
		 cache_debug_log(15);
		return -1;
	 }

	 if (!PFichero[fh].abiertopor[yo])
	 {
		 /* CACHE_ERROR */
		 cache_debug_log(16);
		return -1;
	 }
	 
	 if ((*posicion) > PFichero[fh].size || ((*posicion)+cnt) > PFichero[fh].size)
	 {
		 TomaMutex(mutex_cache);
		 /*TomaMutex(mutex_lock[fh]);*/
		 if ((*posicion) > PFichero[fh].size)
		 {
			 DejaMutex(mutex_cache);
			 /*DejaMutex(mutex_lock[fh]);*/
			 cache_debug_log(36);
			 return 0;
		 }
		 if (((*posicion)+cnt) > PFichero[fh].size)
		 {
		    cnt = (int)(PFichero[fh].size - (*posicion));
		 }
		 DejaMutex(mutex_cache);
		 /*DejaMutex(mutex_lock[fh]);*/
	 }
	 

	 if (cnt < 0)
	 {
		 cache_debug_log(37);
	    return 0;
	 }

	 if (cnt == 0)
	 {
		 cache_debug_log(40);
	    return 0;
	 }

	 if (cnt > TAMBLOQUE)
	 {
		 /* CACHE ERROR */
		 cache_debug_log(17);
		 return -1;
	 }

	 if (cnt != TAMBLOQUE)
	 {
		 /*cache_debug_log(38);*/
	 }

	 /* la busqueda de bloqueos se podria parametrizar 
	    de modo que solo se haga en las lecturas que lo esperan */
	 
	 /*
	 TomaMutex(mutex_lock[fh]);
	 i = BuscaBloqueo(fh,*posicion,(__int64)cnt,NULL,1);
	 DejaMutex(mutex_lock[fh]);

	 if (i > -1)
	 {		 
		 errno = EACCES;		 
		 return -1;
	 }
	 */
	 
	 

	 do
	 {
		 /* Esperar si el servidor esta actualizando la cache */
		 TomaMutex(mutex_cache);

		 
		 if (primero)
		 {			 
			 i = BuscaBloqueo(fh,*posicion,(__int64)cnt,NULL,1);
			 if (i > -1)
			 {				 
				 DejaMutex(mutex_cache);
				 errno = EACCES;		 
				 return -1;
			 }			 
			 primero = 0;
		 }
		 

		 PFichero[fh].leidopor[yo]++;
		 
		 /* a ver si lo que busco esta ya en la cache */
		 bloque = BuscaBloque(fh,*posicion,&offset);
		 if (bloque)
		 {	 
			 /*
			 if (offset != 0)
			 {
				 cache_debug_log(37);
			 }
			 */
			 memcpy(buf,PCache[bloque-1].bloque+offset,cnt);
		 }
		 
		 PFichero[fh].leidopor[yo]--;

		 DejaMutex(mutex_cache);

		 /* si no fallo de cache */
		 if (!bloque)
		 {
			 /* por afinar la concurrencia */		 

OutputDebugString("ini-0\n");
			 
			 PUsuario->peticion = 0;
			 PUsuario->fh = fh;
			 PUsuario->posicion = (*posicion)/TAMBLOQUE;			 
			 PUsuario->peticion = 1;
#ifdef WIN32

			 ResetEvent(evento_propio);
			 {
				 int atasco = 0;
			 do
			 {
			   SetEvent(evento_accion);		 
			   atasco++;
			   if (atasco > 5000)
			   {
				   aviso(0,"ATASCO WIN 1");
				   atasco = 0;
			   }
			 } while(WaitForSingleObject(evento_propio,5) != WAIT_OBJECT_0 && PUsuario->peticion && si_sleep());
			 }
#else
			 PonPeticion();
#endif

OutputDebugString("fin-0\n");
			 continue;
			 /*
			 PUsuario->bloque;
			 */
		 }
		 break;
	 } while(-1);

	 *posicion += cnt;

	 return cnt;
}

int dsdbcache_cierra(int fh)
{
	OutputDebugString("cierra\n");
	 if (fh < 0 || fh >= MAX_FICHEROS)
	 {
		 cache_debug_log(18);
		  return -1;
	 }
	 if (!PMemoria)
	 {
		 aviso(0,"No pmemoria");
		/* CACHE_ERROR */
		 cache_debug_log(19);
		return -1;
	 }

	 if (!PFichero[fh].abiertopor[yo])
	 {
		 /* CACHE_ERROR */

		 cache_debug_log(20);
		return -1;
	 }

	 /*TomaMutex(mutex_lock[fh]);*/
	 TomaMutex(mutex_cache);
	 BorraBloqueo(fh,0,yo);
	 DejaMutex(mutex_cache);
	 /*DejaMutex(mutex_lock[fh]);*/
	 

#ifdef WIN32
	 CloseHandle(mutex_lock[fh]);
	 mutex_lock[fh] = NULL;
#endif

	 PFichero[fh].abiertopor[yo]--;
	 PUsuario->abiertos[fh]--;

	 return 0;
}


DWORD dsdbcache_abre(const char * path,int oflag,int shflag,int pmode)					 
{
	int fh;

	OutputDebugString("abre\n");


	if (sock == INVALID_HANDLE_VALUE)
	{	   
       IniciaSesion();
	   if (sock == INVALID_HANDLE_VALUE)
	   {
		   cache_debug_log(21);
		   return (HANDLE)0xffffffff;
	   }
	   
	}
	
	TomaMutex(mutex_ficheros);
	fh = BuscaFichero(path); /* mecanismo de control de acceso incluido */
	DejaMutex(mutex_ficheros);

	if (fh < 0)
	{		
		
OutputDebugString("ini-1\n");
		PUsuario->peticion = 0;
		strcpy(PUsuario->dato,path);
		PUsuario->fh = oflag;
		PUsuario->peticion = 2;

#ifdef WIN32
		ResetEvent(evento_propio);
			 {
				 int atasco = 0;
			 do
			 {
			   SetEvent(evento_accion);		 
			   atasco++;
			   if (atasco > 5000)
			   {
				   aviso(0,"ATASCO WIN 2");
				   atasco = 0;
			   }
			 } while(WaitForSingleObject(evento_propio,5) != WAIT_OBJECT_0 && PUsuario->peticion && si_sleep());
			 }
#else
			 PonPeticion();
#endif
		
		fh = PUsuario->fh;
OutputDebugString("fin-1\n");
		if (fh < 0)
		{
			 /* ERRORCACHE ... fichero inexistente */
			 errno = ENOENT;			 
			 //Sleep(1);
			 cache_debug_log(22);
			 return (HANDLE)0xffffffff;			 
		}
	}

#ifdef WIN32
	{
		HANDLE hProcess;
		hProcess = OpenProcess( PROCESS_ALL_ACCESS, FALSE, PControles->procesocache ); 
		if (!hProcess)
		{
			cache_debug_log(23);
			 return (HANDLE)0xffffffff;			 
		}

		mutex_lock[fh] = NULL;
		DuplicateHandle(hProcess,PFichero[fh].mutex,GetCurrentProcess(),&(mutex_lock[fh]),MUTEX_ALL_ACCESS,FALSE,0/*DUPLICATE_SAME_ACCESS*/);		
		CloseHandle(hProcess);

		if (!mutex_lock[fh])
		{
			cache_debug_log(24);
			return (HANDLE)0xffffffff;
		}
	}
#else
    mutex_lock[fh] = PFichero[fh].mutex;
#endif

	PFichero[fh].abiertopor[yo]++;
	PUsuario->abiertos[fh]++;
	/*bloquecero[fh] = 0;*/
	
	return fh;
}

int dsdbcache_locking64(int fh, int lmode, ds_off posicion, ds_off nbytes)
{
	 int i;	 
	 int menor = -1;
	 int modo = 0;
	 int cuenta = 0;
	 int quien = -1;

	 OutputDebugString("locking64\n");

	 if (fh < 0 || fh >= MAX_FICHEROS)
	 {	
		 cache_debug_log(25);
		  return -1;
	 }

	 if (!PMemoria)
	 {
		 aviso(0,"No pmemoria");
		/* CACHE_ERROR */
		 cache_debug_log(26);
		return -1;
	 }

	 if (!PFichero[fh].abiertopor[yo])
	 {
		 /* CACHE_ERROR */
		 cache_debug_log(27);
		return -1;
	 }
	 if (lmode == _LK_UNLCK)
		 modo = 2;
	 

	 do
	 {

		 /*TomaMutex(mutex_lock[fh]);*/
		 TomaMutex(mutex_cache);
		 i = BuscaBloqueo(fh,posicion,nbytes,&menor,modo);
		 

		 if (lmode != _LK_UNLCK && lmode != _LK_NBLCK && i >= 0)
		 {
			 if (!PFichero[fh].pidedesbloqueo[yo])
			 {
				 PFichero[fh].pide++;
				 PFichero[fh].pidedesbloqueo[yo]++;
			 }
		 }

		 if (i >= 0 && lmode == _LK_UNLCK)
		 {
			 if (PFichero[fh].pide > 0)
			 {
				SubeSem(evento_cache);
			 }
			 BorraBloqueo(fh,i,-1);
		 }

		 if (lmode != _LK_UNLCK)
		 {
			if (i < 0)
			{
				if (PFichero[fh].pidedesbloqueo[yo])
				{
					 PFichero[fh].pide--;
					 PFichero[fh].pidedesbloqueo[yo] = 0;
				}
				i = PonBloqueo(fh,menor,posicion,nbytes);
			}
			else
			{
				quien = i;
				i = -1;
			}
		 }
		 DejaMutex(mutex_cache);
		 /*DejaMutex(mutex_lock[fh]);*/
		 

		 if (lmode == _LK_UNLCK)
		 {
			 if (i < 0)
			 {
				 /* no lock */
				 errno = EINVAL;				 
				 return -1;
			 }
			 return 0;
		 }

		 if (i < 0)
		 {
			 if (lmode == _LK_NBLCK)
			 {
				 /*  elocked */
				 errno = EACCES;
				 return -1;
			 }

			 /* retry */			 
			 EsperaSem(evento_cache);

			 cuenta++;
			 if (cuenta > /*6000*/ 6000 )
			 {				 
				 char tmp[2048];
				 int user,pid;
				 if (quien > -1)
				 {
					 FILE *f;
					 char tmp2[256];

					 user = PFichero[fh].amo[quien];
					 if (user >= 0)
					 {
						pid = PUsuarios[user].pid;
					 }
					 else 
						 pid = -1;
					 sprintf(tmp,"%d %d %lld %lld %ld ->",yo,user,PFichero[fh].bloqueos[quien],PFichero[fh].hasta[quien],pid);
					 sprintf(tmp2,"%s/%d.rlc",rw_directorio("dev/reg"),pid);
					 f = fopen(tmp2,"r");
					 if (f)
					 {
						 fgets(tmp+strlen(tmp),256,f);
						 fclose(f);
						 quitan(tmp);						 
					 }
					 sprintf(tmp2,"%s/%d.rlw",rw_directorio("dev/reg"),pid);
					 f = fopen(tmp2,"r");
					 if (f)
					 {
						 fgets(tmp+strlen(tmp),256,f);
						 fclose(f);
						 quitan(tmp);						 
					 }
				 }
				 else
				 {
					 strcpy(tmp,"quien=-1");
				 }				 
				 info_cache_debug_log(39,tmp);
				 /*aviso(0,"POSIBLE DEADLOCK");*/
				 cuenta = 0;
			 }

			 continue;
		 }
		 break;
	 } while(-1);

	 return 0;
}


int dsdbcache_graba(int fh,void *buf,__int64 *posicion,unsigned int cnt)
{
	unsigned long bloque;
	int offset = 0;
	int control = 0;
	OutputDebugString("graba\n");
	 if (fh < 0 || fh >= MAX_FICHEROS)
	 {
		 cache_debug_log(28);
		  return -1;
	 }
	 if (!PMemoria)
	 {
		 aviso(0,"No pmemoria");
		/* CACHE_ERROR */
		 cache_debug_log(29);
		return -1;
	 }

	 if (!PFichero[fh].abiertopor[yo])
	 {
		 /* CACHE_ERROR */
		 cache_debug_log(30);
		return -1;
	 }
	 
	 if ((*posicion) > PFichero[fh].size)
	 {
		 /*TomaMutex(mutex_lock[fh]);*/
		 TomaMutex(mutex_cache);
		 if ((*posicion) > PFichero[fh].size)
		 {
			PFichero[fh].size = *posicion;
		 }
		 DejaMutex(mutex_cache);
		 /*DejaMutex(mutex_lock[fh]);*/
	 }	 	 

	 if (cnt <= 0)
	 {
		cache_debug_log(33);
	    return 0;
	 }

	 if (cnt > TAMBLOQUE)
	 {
		 /* CACHE ERROR */
		 cache_debug_log(31);
		 return -1;
	 }

	 if (cnt != TAMBLOQUE)
	 {
		 /*cache_debug_log(34);*/
	 }

	 /* No es necesario buscar bloqueos ... dado el uso que se da en el dsdbfs */

	 do
	 {
		 /* Esperar si el servidor esta actualizando la cache */

		 TomaMutex(mutex_cache);

		 PFichero[fh].leidopor[yo]++;		 
		 /* a ver si lo que busco esta ya en la cache */

		 bloque = BuscaBloque(fh,*posicion,&offset);
		 if (bloque)
		 {	 
			 /*
			 if (offset != 0)
			 {
				 cache_debug_log(35);
			 }
			 */
			 memcpy(PCache[bloque-1].bloque+offset,buf,cnt);
			 PCache[bloque-1].flags |= DSDB_FLAGDIRTY;
			 /*TomaMutex(mutex_lock[fh]);*/
			 if (((*posicion)+cnt) > PFichero[fh].size)
			 {
				 /* afinar concurrencia ...en teoria deberia estar protegido por la gestion del fichero unico */
				 PFichero[fh].size = ((*posicion)+cnt);
			 }
			 /*DejaMutex(mutex_lock[fh]);*/
		 }
		 
		 PFichero[fh].leidopor[yo]--;

		 DejaMutex(mutex_cache);

		 /* si no fallo de cache */
		 if (!bloque)
		 {
			 /* por afinar la concurrencia */

OutputDebugString("ini-2\n");
			 
			 PUsuario->peticion = 0;
			 PUsuario->fh = fh;
			 PUsuario->posicion = (*posicion)/TAMBLOQUE;
			 PUsuario->peticion = 1;

#ifdef WIN32
			 ResetEvent(evento_propio);
			 {
				 int atasco = 0;
			 do
			 {
			   SetEvent(evento_accion);		 
			   atasco++;
			   if (atasco > 5000)
			   {
				   aviso(0,"ATASCO WIN 3");
				   atasco = 0;
			   }
			 } while(WaitForSingleObject(evento_propio,5) != WAIT_OBJECT_0 && PUsuario->peticion && si_sleep());
			 }
#else
			 PonPeticion();
#endif
OutputDebugString("fin-2\n");
			 continue;
			 /*
			 PUsuario->bloque;
			 */
		 }
		 break;
	 } while(-1);

	 *posicion += cnt;

	 return cnt;
}

void fincache()
{
extern unsigned long refrecos_cab;
extern unsigned long info_abre;
extern unsigned long info_cierra;
extern unsigned long info_lseek;
extern unsigned long info_graba;
extern unsigned long info_lee;
extern unsigned long info_locking;
extern unsigned long info_tamtabla;
extern unsigned long info_unlink;
extern unsigned long info_rename;
extern unsigned long info_arbol;
extern unsigned long info_iarbol;
extern unsigned long info_nounlink;
extern unsigned long info_confirma;
extern unsigned long info_rabre;
extern unsigned long info_basea;

#ifndef WIN32
	if (PMemoria)
	{
		munmap(PMemoria, TamanoCache); 
		PMemoria = NULL;
		close(mapa);
		mapa = -1;
	}
#else
	if (PMemoria)
	{
		UnmapViewOfFile(PMemoria);
		CloseHandle(mapa);
		CloseHandle(evento_propio);
		CloseHandle(evento_accion);
		CloseHandle(evento_cache);
		CloseHandle(mutex_cache);
		CloseHandle(mutex_ficheros);
	}	
#endif
	{
		char tmp[512];
		sprintf(tmp,"maximo bloqueos necesitados (r_cab:%ld,i_a:%ld,i_c:%ld,i_s:%ld,i_g:%ld,i_l:%ld,i_k:%ld,i_t:%ld,i_u:%ld,i_r:%ld,i_arbol:%ld,i_iarbol:%ld,nounlink:%ld,confirma:%ld,r_abre:%ld,b_a:%ld)",
		refrecos_cab,
info_abre,
info_cierra,
info_lseek,
info_graba,
info_lee,
info_locking,
info_tamtabla,
info_unlink,
info_rename,
info_arbol,
info_iarbol,
info_nounlink,
info_confirma,
info_rabre,
info_basea
		);
	    info_cache_debug_log(max_bloqueos,tmp);
	}
}