#ifndef DSDBSRV_H 
#define DSDBSRV_H
#ifndef WIN32
#define DWORD unsigned long
#define HANDLE int
#endif
/* Mapa memoria helpers */

#define TAMBLOQUE 4096

#define MAX_USUARIOS 1024
#define MAX_FICHEROS 1024

#define MAXBLOQUEOS 5120

#define DSDB_FLAGDIRTY 0x1

#define MAX_PETICIONES 20

#define NMUTEXPROPIO 50

struct _controles_
{
	unsigned long leyendo;	
	int nUsuarios;
	int nFicheros;
	int nLista;
	DWORD procesocache;
	unsigned long transaccion;	
	int mutex[(MAX_USUARIOS+3)*NMUTEXPROPIO];
	short _sema[NMUTEXPROPIO];
	short peticiones[MAX_PETICIONES];
	char reservado[128];
};

struct _handle_usuario_
{
	int fichero;
	int peticion;
	unsigned long bloque;
	int fh;
	ds_off posicion;
	ds_off tama;
	ds_off resultado;
	char dato[512];	
	HANDLE evento;
	int pid;
	pthread_t hebra;
	SOCKET csock;
	int nevento;
	unsigned char abiertos[MAX_FICHEROS];
	int nextfree;
};

struct _handle_fichero_
{
	unsigned long control;
	char nombre[512];
	__int64 size;	
	HANDLE real_handle;
	HANDLE mutex;
	unsigned long bleyendo;
	unsigned char bgrabando;	
	int nbloqueos;
	ds_off bloqueos[MAXBLOQUEOS];
	ds_off hasta[MAXBLOQUEOS];	
	int    amo[MAXBLOQUEOS];
	int    pide;
	unsigned char abiertopor[MAX_USUARIOS]; /* flag abierto */
	unsigned char leidopor[MAX_USUARIOS]; /* flag lectura */
	unsigned char pidedesbloqueo[MAX_USUARIOS]; /* flag lectura */
	int nextfree;
};

struct _lista_cache_
{
	unsigned __int64 dato; /* hi fichero, lo bloque */
	unsigned long nblocache; /* bloque de cache donde esta */
};

struct _cache_
{
	unsigned short flags;	
	unsigned short file;
	unsigned long nbloque;
	unsigned long tiempo;	
	unsigned long count;	
	unsigned char bloque[TAMBLOQUE];
};

struct DBID_inid
{
	int version;
	DWORD id;
	char reservado[256];
};

struct DBID_handles
{
	int version;
	int sesion;
	HANDLE dmapa;		
	HANDLE devento_propio;
	HANDLE devento_accion;	
	HANDLE devento_cache;	
	HANDLE dmutex_cache;	
	HANDLE dmutex_ficheros;	
	unsigned long tamanocache;
	char reservado[256];
};

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

#define TIPO_OPERADOR  0x1
#define TIPO_CLIENTE   0x2

#define ESTADO_VACIO        0x0
#define ESTADO_LIBRE        0x1
#define ESTADO_OCUPADO      0x2
#define ESTADO_NODISPONIBLE 0x3

#define LEN_CABECERA   sizeof(Cabecera)
#define LEN_DATA(a)    ((a)->c.command & 0xfff)
#define LEN_PAQUETE(a) (LEN_CABECERA + LEN_DATA(a))
#define COMANDO(a)     ((a)->c.command & 0xf000)
#define DATO_INT(a)    (*((int *)(a)->buffer))

#define PON_LEN_DATA(a,b) (a)->c.command = (((a)->c.command & 0xf000) | ((b) & 0xfff))

#define MAXBUFFER	  4096

typedef unsigned char  UChar;
typedef unsigned short UShort;
typedef unsigned long  ULONG;
typedef unsigned long  IPID;

#define DSDSDBID 0x6776

#define DSDBPORT  3991
#define DSDBPORTC 3992

typedef struct 
{
	UShort id;
	UShort command;
} Cabecera;

typedef struct 
{
    Cabecera  c;
	UChar buffer[MAXBUFFER*2];
} Paquete;


typedef struct 
{
    Cabecera  c;
	UChar *buffer;
} PPaquete;

#endif