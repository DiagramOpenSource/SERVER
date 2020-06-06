
#ifdef FGVLINUX
#define _FILE_OFFSET_BITS 64
#define _LARGEFILE_SOURCE 1
#define _LARGEFILE64_SOURCE 1
#include "unistd.h"
#endif


#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef WIN32
#include <sys/locking.h>
#include <share.h>
#include <io.h>
#include <windows.h>
#define sleep(a) Sleep((long)(((long)a)*1000L));

#ifndef LK_UNLCK
#define LK_UNLCK _LK_UNLCK
#endif

#ifndef LK_LOCK
#define LK_LOCK  _LK_LOCK
#endif

#ifndef LK_WAIT
#define LK_WAIT  _LK_LOCK
#endif

#ifndef LK_NBLCK
#define LK_NBLCK _LK_NBLCK
#endif

#else

#define dsdb_usleep usleep
#define dsdb_usleep usleep
#define SH_DENYNO 0
#define SH_DENYRW 2
#define MAX_PATH 128
#ifdef FGVLINUX
#define O_BINARY O_LARGEFILE
#else
#define O_BINARY 0
#endif
#define LK_WAIT LK_LOCK

int _doserrno = 0;

#include <stdarg.h>

#endif

#include <errno.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <sys/timeb.h>

#include "dsdbfs.h"

#include <rwmalloc.h>
/*

#ifdef WIN32

__int64 dsdb_lseek(int fh, __int64 distance, DWORD MoveMethod);
int dsdb_lee(int fh,void *buf,unsigned cnt);
int dsdb_cierra(int fh);
int dsdb_sabre(const char *path,int oflag,int shflag,...);
int dsdb_abre(const char *path,int oflag,...);        
int dsdb_locking(int fh,int lmode,long nbytes);
int dsdb_locking64(int fh,int lmode,ds_off nbytes);
int dsdb_graba(int fh,const void *buf,unsigned cnt);

#else

#define dsdb_sabre          _sopen_
#define dsdb_abre           open
#define dsdb_lee            read
#define dsdb_graba          write
#define dsdb_lseek(a,b,c)   ((ds_off)lseek(a,((off_t)b),c))
#define dsdb_cierra         close
#define dsdb_locking(a,b,c)        lockf(a,b,((off_t)c))
#define dsdb_locking64(a,b,c) 	   lockf(a,b,((off_t)c))

#endif

*/


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

#define OFFSET_PUNTEROS(a,b) (int)((char *)(a) - (char *)(b))

#define COMPARA_OFFSETS(a,b) ( (a > b) ? 1 : ( (a < b) ? -1 : 0 ) )

#define DSLODWORD(l)   ((ulong)(l & 0xFFFFFFFF)) 
#define DSHIDWORD(l)   ((ulong)(((ds_off) (l) >> 32) & 0xFFFFFFFF))
#define DSMAKEOFF(a, b) ((ds_off) (((ulong) (a)) | ((ds_off) ((ulong) (b))) << 32)) 

#define dsdb_pasahandle(a) (a & 0x40000000)

unsigned long ip_2key = 0L;
char **lista_emprecopias = NULL;
unsigned char copia_key[248];

extern char *quita_path();

extern int (*ixxdat_aviso)(int,long,char *);

ulong dsdb_blocksize = DEFDBBLOCKSIZE; /* tamaño del bloque (para el caso de crear el fichero) */

static DBFile dsdb_base_files[MAX_OPEN_FILES];
int ndsdb_base_files = 0;
int dsdb_base_free_file = -1;

unsigned char DSDB_RANDOM[18] = "\1\2\3\4\5\6\7\10\11\0\1\2\3\4\5\6\0";
unsigned char DSDB_[18] = "\1\2\3\4\5\6\7\10\11\0\1\2\3\4\5\6\0";

extern int coincide_(char *buf,char *filtro);
extern void cambia_barra(char *s);

extern int  rw_usa_dsdb; /* definido en rini */
extern int  rw_dsdb_tambloque;
extern int  rw_dsdb_bloquesdefecto;
extern int  rw_dsdb_bloquesaumento;
extern char *rw_dsdb_filtro;

extern char rw_user[11];
extern char nomapli[25];
extern char modulo[9];

unsigned char keycopia[18] = "\1\2\3\4\5\6\7\10\11\0\1\2\3\4\5\6\0";


unsigned long refrecos_cab = 0;
unsigned long info_abre = 0;
unsigned long info_cierra = 0;
unsigned long info_lseek = 0;
unsigned long info_graba = 0;
unsigned long info_lee = 0;
unsigned long info_locking = 0;
unsigned long info_tamtabla = 0;
unsigned long info_unlink= 0;
unsigned long info_rename= 0;
unsigned long info_arbol = 0;
unsigned long info_iarbol = 0;
unsigned long info_nounlink= 0;
unsigned long info_confirma = 0;
unsigned long info_rabre = 0;
unsigned long info_basea = 0;

#ifdef WIN32
void dsdb_usleep(int nm)
{
	Sleep(nm/1000 + ((nm % 1000) ? 1 : 0));
/*
	static int waiter = 0;
	if (nm > 999)
	{
		Sleep(nm/1000);
		waiter = 0;
	}
	else
	{
		waiter += nm;
		if (waiter >= 1000)
		{
			Sleep(1);
			waiter = 0;
		}
	}
*/
}
#endif

extern char *dsdb_serverip;

static char *dsdb_extensiones[] =
{
	".ixx",
	".dat",
	".dbf",
	".ddx",
	".diu",
	".tmx",
	".dbt",
	".tdy",
	NULL
};

#define DBMAXNIVELES 200
typedef struct
{
    int handle;
	ulong id;
	void *compara;
	int sizex;
	int size0;
	ulong blocksize;	
	void *resultado;
	int r;
	ulong rbloque;
	ulong roffset;
	ulong padres[DBMAXNIVELES+1];
	DBFile *pfile;
} DBArbolBusca;

static struct
{
   char base[MAX_PATH];
   int handle;
   int referencia; /* contador de accesos corrientes (ficheros abiertos) */   
   ulong nupdate;
   unsigned short openid;
   DBHeader header;
   int refbloqueoescritura;
   ulong flags; 
   /*ulong oupdate;*/
} BasesAbiertas[MAX_OPEN_BASES];
int nBasesAbiertas = 0;


ulong dsdb_nuevo_bloque(int base_fd);
int dsdb_u_graba(int handle, void *buffer, unsigned int count);
int dsdb_u_cierra(int handle);
ds_off dsdb_u_lseek(int handle,ds_off offset,int origin);

int dsdb_base_abre(char *base);
int dsdb_base_cierra(int base_fd);

int DSDB_ReconstruyeLimpiamente(char *base,int modo);

/* De momento no es thread safe ... */
/* Incluir bloqueos ... */

/* Definiciones para la gestion de errores */
static int lista_de_errores_fatales[] = 
{
	147,149,151,148,152,146,168,157,159,160,7,
	166,167,22,12,182,184,186,120,141,123,103,106,109,11,
	98,100,163,200,0
};
char *dsdb_ = DSDB_;

#define MAXERRORES 512

int dsdb_sin_errores = 0;
static int lista_de_errores_internos[MAXERRORES];
static int lista_de_errores_errno;
static int lista_nerrores = 0;
static int control_error = 0;
static int base_fd_error = -1;
static char base_tabla_error[MAX_PATH];

#define DSDB_ERROR_INICIO InicializaError()
#define DSDB_ERROR_FIN FinalizaError()
#define DSDB_ERROR_QUE_BASE(a) PonBaseError(a)
#define DSDB_ERROR_NOMBRE_BASE(a) PonBaseNombreError(a)


/* buffers de trabajo */
static char *bloque_tmp1 = NULL; /* para escritura datos,y escritura arbol */
static int len_bloque_tmp1 = 0;  /* es el doble del bloque */

static char *bloque_tmp2 = NULL; /* auxiliar */
static int len_bloque_tmp2 = 0;

static char *bloque_tmp3 = NULL; /* para lecturas de arbol */
static int len_bloque_tmp3 = 0;

static char *bloque_tmp4 = NULL; /* auxiliar arbol doble del bloque */
static int len_bloque_tmp4 = 0;


/* definiciones para bloqueos y flags */

#define DBPOS_IDTOUCH       (ds_off)400

#define DBPOS_TOUCH         (ds_off)504
static ulong ft_tocado = 0xffffffff;
static ulong ft_notocado = 0;
#define FLAG_TOUCH          ft_tocado
#define FLAG_NOTOUCH        ft_notocado
#define FLAG_TOUCHLEE       ulong
#define PFLAG_TOUCH         ((void *)&ft_tocado)
#define PFLAG_NOTOUCH       ((void *)&ft_notocado)
#define OFFSET_PADDING_TOUCH 1

#define DBLOCK_FLAG_REC     (ds_off)506

#define DBLOCK_FLAG_DIR     (ds_off)509


#define DBLOCK_ALL          (ds_off)65536
#define DBPOS_LOCK			(ds_off)2199023255552
#define DBPOS_LOCK_DIR		DBPOS_LOCK
#define DBPOS_LOCK_FREELIST (DBPOS_LOCK_DIR + DBLOCK_ALL)
#define DBPOS_LOCK_MAP      (DBPOS_LOCK_FREELIST+(ds_off)1)
#define LENMAPLOCK          DBLOCK_ALL

#define LENALLLOCKS        ((DBLOCK_ALL*(ds_off)DSBDBMAXTABLES)+DBPOS_LOCK+(ds_off)1)

#ifndef IXXFILEEND
#define IXXFILEEND			(ds_off)2147483645
#endif

#define LENIXXVLOCK			(ds_off)256131074 /* adaptado para los indices */
#define DBINIVIRLOCK        (IXXFILEEND-LENIXXVLOCK)

#define DBPOS_LOCK_FILE     (DBPOS_LOCK+LENALLLOCKS)

#define LENFILELOCK         (ds_off)(DBLOCK_ALL*(ds_off)MAX_OPEN_FILES)
#define LENUSERLOCK         (LENIXXVLOCK+LENFILELOCK)

#define DBOFFEXLOCK         0
#define DBOFFUSERLOCK       LENFILELOCK



/* CRYTO, TEA */
/*a key, k[0] - k[3].  Data is in v[0] and v[1]. */

static void dsdb_c_code(long* vo, long* v, long* k)  
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
#ifdef INNECESARIO
static void dsdb_c_decode( long* v, long* k )  
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
#endif

int dsdb_usa_crypto = 0;
static unsigned char *crypto_buffer = NULL;
static unsigned int  ncrypto_buffer = 0;
static unsigned char matriz[313][8];
static int matriz_creada = 0;



static int es_empresa_copia(char *base)
{
	int i;
	char _base[512];
	
	if (!lista_emprecopias)
		return 0;

	strcpy(_base,base);
	strminus(_base);
	cambia_barra(_base);

	for (i = 0;lista_emprecopias[i];i += 2)
	{
		if (lista_emprecopias[i])
		{
			if (!strcmp(_base,lista_emprecopias[i]))
				return 1;
		}
		else
			break;
	}
	return 0;
}

static char *dame_empresa_copia(char *base)
{
	int i;
	char _base[512];
	
	if (!lista_emprecopias)
		return base;

	strcpy(_base,base);
	strminus(_base);
	cambia_barra(_base);

	for (i = 0;lista_emprecopias[i];i += 2)
	{
		if (lista_emprecopias[i])
		{
			if (!strcmp(_base,lista_emprecopias[i]))
				return lista_emprecopias[i+1];
		}
		else
			break;
	}
	return base;
}

static void crea_tabla_codes(unsigned char *enigma,unsigned char *localkey)
{
	int i;	
	unsigned char comparador[9];	
	unsigned char key[16];
	unsigned char *p;

	if (!matriz_creada)
	{
		matriz_creada = 1;
		for (i = 0;i < 313;i++)
		{			
			matriz[i][0] = i%256;
			matriz[i][1] = 4 + i%12;
			matriz[i][2] = 5 + i/12;
			matriz[i][3] = 1 + i%4;
			matriz[i][4] = 7 + i/2;
			matriz[i][5] = 2 + i/17;
			matriz[i][6] = 13 + i%17;
			matriz[i][7] = i/256;
		}
	}
	for (i = 0;i < 16;i++)
	{
		key[i] = dsdb_[i] ^ localkey[i];
	}
	p = enigma;
	for (i = 0;i < 313;i++)
	{
		dsdb_c_code((long *)(matriz[i]),(long *)comparador,(long *)key);
		memcpy(p,comparador,8);
		p += 8;
	}
}

static void	encrypta(unsigned char *enigma,ds_off posicion,unsigned char *buffer,unsigned char *crypto_buffer,int count)
{	
	unsigned char c,c1;
	register unsigned char *p1,*p2,*p3,*p4,*p5,*p6,*p7,*p8;
	register int i1,i2,i3,i4,i5,i6,i7,i8;

	if (!count)
		return;

	if (posicion < (ds_off)313)
	{	
		i1 = (int)posicion;
		p1 = enigma + i1 * 8;
		i2 = i3 = i4 = i5 = i6 = i7 = i8 = 0;
		p2 = enigma + 1;
		p3 = p2 + 1;
		p4 = p3 + 1;
		p5 = p4 + 1;
		p6 = p5 + 1;
		p7 = p6 + 1;
		p8 = p7 + 1;
	}
	else
	{
		i1 = (int)(ds_off)(posicion % 313);
		p1 = enigma + i1 * 8;
		posicion /= 313;

		if (posicion)
		{	
			i2 = (int)(ds_off)(posicion % 313);
			p2 = enigma + i2 * 8 + 1;
			posicion /= 313;

			if (posicion)
			{	
				i3 = (int)(ds_off)(posicion % 313);
				p3 = enigma + i3 * 8 + 2;
				posicion /= 313;

				if (posicion)
				{
					i4 = (int)(ds_off)(posicion % 313);
					p4 = enigma + i4 * 8 + 3;
					posicion /= 313;

					if (posicion)
					{
						i5 = (int)(ds_off)(posicion % 313);
						p5 = enigma + i5 * 8 + 4;
						posicion /= 313;

						if (posicion)
						{
							i6 = (int)(ds_off)(posicion % 313);
							p6 = enigma + i6 * 8 + 5;
							posicion /= 313;

							i7 = (int)(ds_off)(posicion % 313);
							p7 = enigma + i7 * 8 + 6;
							posicion /= 313;

							i8 = (int)(ds_off)(posicion % 313);
							p8 = enigma + i8 * 8 + 7;
						}
						else
						{
							i6 = i7 = i8 = 0;
							p6 = enigma + 5;
							p7 = p6 + 1;
							p8 = p7 + 1;
						}
					}
					else
					{
						i5 = i6 = i7 = i8 = 0;						
						p5 = enigma + 4;
						p6 = p5 + 1;
						p7 = p6 + 1;
						p8 = p7 + 1;
					}
				}
				else
				{
					i4 = i5 = i6 = i7 = i8 = 0;
					p4 = enigma + 3;
					p5 = p4 + 1;
					p6 = p5 + 1;
					p7 = p6 + 1;
					p8 = p7 + 1;
				}
			}
			else
			{
				i3 = i4 = i5 = i6 = i7 = i8 = 0;			
				p3 = enigma + 2;
				p4 = p3 + 1;
				p5 = p4 + 1;
				p6 = p5 + 1;
				p7 = p6 + 1;
				p8 = p7 + 1;
			}
		}
		else
		{
			i2 = i3 = i4 = i5 = i6 = i7 = i8 = 0;
			p2 = enigma + 1;
			p3 = p2 + 1;
			p4 = p3 + 1;
			p5 = p4 + 1;
			p6 = p5 + 1;
			p7 = p6 + 1;
			p8 = p7 + 1;
		}
	}

	c1 = *p2 ^ *p3 ^ *p4 ^ *p5 ^ *p6 ^ *p7 ^ *p8;
	
	while(count > 0)
	{
		c = *p1 ^ c1;
		*crypto_buffer++ = *buffer++ ^ c;
		count--;
		i1++;
		p1 += 8;
		if (i1 == 313)
		{
			i1 = 0;
			p1 = enigma;
			i2++;
			p2 += 8;
			if (i2 == 313)
			{
				i2 =0;
				p2 = enigma + 1;
				i3++;
				p3 += 8;
				if (i3 == 313)
				{
					i3 = 0;
					p3 = enigma + 2;
					i4++;
					p4 += 8;
					if (i4 == 313)
					{
						i4 = 0;
						p4 = enigma + 3;
						i5++;
						p5 += 8;
						if (i5 == 313)
						{
							i5 = 0;
							p5 = enigma + 4;
							i6++;
							p6 += 8;
							if (i6 == 313)
							{
								i6 = 0;
								p6 = enigma + 5;
								i7++;
								p7 += 8;
								if (i7 == 313)
								{
									i7 = 0;
									p7 = enigma + 6;
									i8++;
									p8 += 8;
									if (i8 == 313)
									{
										i8 = 0;
										p8 = enigma + 7;
									}
								}
							}
						}
					}
				}
			}			
			c1 = *p2 ^ *p3 ^ *p4 ^ *p5 ^ *p6 ^ *p7 ^ *p8;
		}		
	}
}

static void	desencrypta(unsigned char *enigma,ds_off posicion,unsigned char *buffer,int count)
{
	unsigned char c,c1;
	register unsigned char *p1,*p2,*p3,*p4,*p5,*p6,*p7,*p8;
	register int i1,i2,i3,i4,i5,i6,i7,i8;

	if (!count)
		return;

	if (posicion < (ds_off)313)
	{	
		i1 = (int)posicion;
		p1 = enigma + i1 * 8;
		i2 = i3 = i4 = i5 = i6 = i7 = i8 = 0;
		p2 = enigma + 1;
		p3 = p2 + 1;
		p4 = p3 + 1;
		p5 = p4 + 1;
		p6 = p5 + 1;
		p7 = p6 + 1;
		p8 = p7 + 1;
	}
	else
	{
		i1 = (int)(ds_off)(posicion % 313);
		p1 = enigma + i1 * 8;
		posicion /= 313;

		if (posicion)
		{	
			i2 = (int)(ds_off)(posicion % 313);
			p2 = enigma + i2 * 8 + 1;
			posicion /= 313;

			if (posicion)
			{	
				i3 = (int)(ds_off)(posicion % 313);
				p3 = enigma + i3 * 8 + 2;
				posicion /= 313;

				if (posicion)
				{
					i4 = (int)(ds_off)(posicion % 313);
					p4 = enigma + i4 * 8 + 3;
					posicion /= 313;

					if (posicion)
					{
						i5 = (int)(ds_off)(posicion % 313);
						p5 = enigma + i5 * 8 + 4;
						posicion /= 313;

						if (posicion)
						{
							i6 = (int)(ds_off)(posicion % 313);
							p6 = enigma + i6 * 8 + 5;
							posicion /= 313;

							i7 = (int)(ds_off)(posicion % 313);
							p7 = enigma + i7 * 8 + 6;
							posicion /= 313;

							i8 = (int)(ds_off)(posicion % 313);
							p8 = enigma + i8 * 8 + 7;
						}
						else
						{
							i6 = i7 = i8 = 0;
							p6 = enigma + 5;
							p7 = p6 + 1;
							p8 = p7 + 1;
						}
					}
					else
					{
						i5 = i6 = i7 = i8 = 0;						
						p5 = enigma + 4;
						p6 = p5 + 1;
						p7 = p6 + 1;
						p8 = p7 + 1;
					}
				}
				else
				{
					i4 = i5 = i6 = i7 = i8 = 0;
					p4 = enigma + 3;
					p5 = p4 + 1;
					p6 = p5 + 1;
					p7 = p6 + 1;
					p8 = p7 + 1;
				}
			}
			else
			{
				i3 = i4 = i5 = i6 = i7 = i8 = 0;			
				p3 = enigma + 2;
				p4 = p3 + 1;
				p5 = p4 + 1;
				p6 = p5 + 1;
				p7 = p6 + 1;
				p8 = p7 + 1;
			}
		}
		else
		{
			i2 = i3 = i4 = i5 = i6 = i7 = i8 = 0;
			p2 = enigma + 1;
			p3 = p2 + 1;
			p4 = p3 + 1;
			p5 = p4 + 1;
			p6 = p5 + 1;
			p7 = p6 + 1;
			p8 = p7 + 1;
		}
	}

	c1 = *p2 ^ *p3 ^ *p4 ^ *p5 ^ *p6 ^ *p7 ^ *p8;

	while(count > 0)
	{
		c = *p1 ^ c1;
		*buffer++ ^= c;
		count--;
		i1++;
		p1 += 8;
		if (i1 == 313)
		{
			i1 = 0;
			p1 = enigma;
			i2++;
			p2 += 8;
			if (i2 == 313)
			{
				i2 =0;
				p2 = enigma + 1;
				i3++;
				p3 += 8;
				if (i3 == 313)
				{
					i3 = 0;
					p3 = enigma + 2;
					i4++;
					p4 += 8;
					if (i4 == 313)
					{
						i4 = 0;
						p4 = enigma + 3;
						i5++;
						p5 += 8;
						if (i5 == 313)
						{
							i5 = 0;
							p5 = enigma + 4;
							i6++;
							p6 += 8;
							if (i6 == 313)
							{
								i6 = 0;
								p6 = enigma + 5;
								i7++;
								p7 += 8;
								if (i7 == 313)
								{
									i7 = 0;
									p7 = enigma + 6;
									i8++;
									p8 += 8;
									if (i8 == 313)
									{
										i8 = 0;
										p8 = enigma + 7;
									}
								}
							}
						}
					}
				}
			}
			c1 = *p2 ^ *p3 ^ *p4 ^ *p5 ^ *p6 ^ *p7 ^ *p8;
		}		
	}
}


/* GESTION DE ERRORES */
static void InicializaError()
{
	lista_nerrores = 0;
	base_fd_error = -1;
	base_tabla_error[0] = 0;
	if (control_error == 1 && !dsdb_sin_errores)
	{
		char tmp[512];
		sprintf(tmp,"recursion no controlada");
		aviso(0,tmp);
	}
	control_error = 1;	
}

static void FinalizaError()
{
	if (!control_error && !dsdb_sin_errores)
	{
		char tmp[512];
		sprintf(tmp,"no control error ...");
		aviso(0,tmp);
	}
	base_fd_error = -1;
	control_error = 0;
}

static void PonBaseError(int base_fd)
{
	base_fd_error = base_fd;
}

static void PonBaseNombreError(char *base)
{
	strcpy(base_tabla_error,base);
}

static void guarda_error(char *tmp)
{
	guarda_incidencia(0,tmp);
	{
	   FILE *f;
	   char tmpc[512];
	   sprintf(tmpc,"%sprobunico.txt",rw_directorio("logs/"));
	   f = fopen(tmpc,"a");
	   if (f)
	   {
		   extern char *fecha();
		   fprintf(f,"%s %s %s %s %s %s\r\n",fecha(),fecha()+11,rw_user,nomapli,modulo,tmp);
		   fclose(f);
	   }
	}
}

static void GuardaErrores(int modo)
{	
	char tmp[2256];
	int i;
	char *p = base_tabla_error;
	if (lista_nerrores > 0)
	{
		if (base_fd_error != -1)
		{
			p = BasesAbiertas[base_fd_error].base;
		}
		if (lista_de_errores_internos[0] == 200)
		{
			sprintf(tmp,"ATENCION! Clave incorrecta de acceso a los datos! [%s]",p);
		}
		else
		{
			sprintf(tmp,"cod[%d]sys[%d] <%s> ",lista_de_errores_internos[0],lista_de_errores_errno,quita_path(p));
			for (i = 1;i < lista_nerrores;i++)
			{
				sprintf(tmp+strlen(tmp),", %d",lista_de_errores_internos[i]);
			}
		}	
		if (modo)
		{
			strcatd(tmp,"(traza) ");
		}
		guarda_error(tmp);
	}
	if (modo)
	{
		lista_nerrores = 0;
	}
}




static void PintaErrores()
{	
	char tmp[2256];
	int i;
	char *p = base_tabla_error;
	if (lista_nerrores > 0)
	{
		if (base_fd_error != -1)
		{
			p = BasesAbiertas[base_fd_error].base;
		}
		if (lista_de_errores_internos[0] == 200)
		{
			sprintf(tmp,"ATENCION! Clave incorrecta de acceso a los datos! [%s]",p);
		}
		else
		{
			sprintf(tmp,"[%s] cod[%d]sys[%d] ",p,lista_de_errores_internos[0],lista_de_errores_errno);
			for (i = 1;i < lista_nerrores;i++)
			{
				sprintf(tmp+strlen(tmp),", %d",lista_de_errores_internos[i]);
			}
		}
		tmp[250] = 0;
		aviso(0,tmp);	
	}
}

static void DSDB_ERROR(int error)
{
	if (lista_nerrores < MAXERRORES)
	{
		lista_de_errores_internos[lista_nerrores] = error;
		if (!lista_nerrores)
		{
			lista_de_errores_errno = errno;
		}		
		lista_nerrores++;
	}
}

static int DSDB_ERROR_ANALIZA()
{
    static int _lista_de_errores_internos[MAXERRORES];
    static int _lista_de_errores_errno;
	static int _lista_nerrores;

	if (!control_error && !dsdb_sin_errores)
	{
		char tmp[512];
		sprintf(tmp,"no control error (analiza)...");
		aviso(0,tmp);
		return 0;
	}
	if (lista_nerrores > 0)
	{
		char *p;
		if ((unsigned)base_fd_error >= (unsigned)nBasesAbiertas || BasesAbiertas[base_fd_error].handle < 0)
		{			
			base_fd_error = -1;
		}

		GuardaErrores(0);

		if (dsdb_sin_errores)
		{
			PintaErrores();
			return 0;
		}

		if (control_error == 1)
		{
 		   int i;
		   for (i = 0;lista_de_errores_fatales[i];i++)
		   {
			   if (lista_de_errores_fatales[i] == lista_de_errores_internos[0])
			   {
				  PintaErrores();
				  return 0;
			   }
		   }
           memcpy((void *)&_lista_de_errores_internos,(void *)&lista_de_errores_internos,sizeof(lista_de_errores_internos));
		   _lista_de_errores_errno = lista_de_errores_errno;
	       _lista_nerrores = lista_nerrores;
		}
		if (control_error == 2)
		{
            memcpy((void *)&lista_de_errores_internos,(void *)&_lista_de_errores_internos,sizeof(lista_de_errores_internos));
		    lista_de_errores_errno = _lista_de_errores_errno;
	        lista_nerrores = _lista_nerrores;
			PintaErrores();
			return 0;
		}

		if (base_fd_error != -1)
		{
			p = BasesAbiertas[base_fd_error].base;
		}
		else
		{
			p = base_tabla_error;
		}
        if (DSDB_ReconstruyeLimpiamente(p,0) < 0)
		{
            memcpy((void *)&lista_de_errores_internos,(void *)&_lista_de_errores_internos,sizeof(lista_de_errores_internos));
		    lista_de_errores_errno = _lista_de_errores_errno;
	        lista_nerrores = _lista_nerrores;
			PintaErrores();
			return 0;
		}		

		control_error = 2;
		return 1;		
	}	
	return 0;
}

/* BUFFERS */
static void dsdb_ajusta_tmps(int tambloque)
{

	if (len_bloque_tmp3 < tambloque)
	{
		if (bloque_tmp3)
		  free(bloque_tmp3);
		bloque_tmp3 = malloc(tambloque);
		len_bloque_tmp3 = tambloque;
	}
	if (len_bloque_tmp2 < tambloque)
	{
		if (bloque_tmp2)
		  free(bloque_tmp2);
		bloque_tmp2 = malloc(tambloque);
		len_bloque_tmp2 = tambloque;
	}
	if (len_bloque_tmp1 < (tambloque*2))
	{
		if (bloque_tmp1)
		  free(bloque_tmp1);
		bloque_tmp1 = malloc(tambloque*2);
		len_bloque_tmp1 = tambloque*2;
	}
	if (len_bloque_tmp4 < (tambloque*2))
	{
		if (bloque_tmp4)
		  free(bloque_tmp4);
		bloque_tmp4 = malloc(tambloque*2);
		len_bloque_tmp4 = tambloque*2;
	}
}


/* CONTROL DE INTEGRIDAD */
static void dsdb_tocando_dir(int base_fd)
{
	extern char *fecha();
	char tmp[51];	

	dsdb_lseek(base_fd,DBPOS_TOUCH,SEEK_SET);
	dsdb_graba(base_fd,PFLAG_TOUCH,sizeof(FLAG_TOUCH));

	dsdb_lseek(base_fd,DBPOS_IDTOUCH,SEEK_SET);	
	dsdb_graba(base_fd,rw_user,sizeof(rw_user));
	dsdb_graba(base_fd,nomapli,sizeof(nomapli));
	dsdb_graba(base_fd,modulo,sizeof(modulo));	
	sprintf(tmp,"%s %s",fecha(),fecha()+11);
	dsdb_graba(base_fd,tmp,sizeof(tmp));	
}

static void dsdb_fin_tocando_dir(int base_fd)
{
	static char vacio[25] = {0,0,0,};
	dsdb_lseek(base_fd,DBPOS_TOUCH,SEEK_SET);
	dsdb_graba(base_fd,PFLAG_NOTOUCH,sizeof(FLAG_NOTOUCH));	

	dsdb_lseek(base_fd,DBPOS_IDTOUCH,SEEK_SET);	
	dsdb_graba(base_fd,vacio,sizeof(rw_user));
	dsdb_graba(base_fd,vacio,sizeof(nomapli));
	dsdb_graba(base_fd,vacio,sizeof(modulo));
}

static int dsdb_esta_tocado_dir(int base_fd)
{
	FLAG_TOUCHLEE flag = FLAG_TOUCH;
	dsdb_lseek(base_fd,DBPOS_TOUCH,SEEK_SET);
	dsdb_lee(base_fd,(void *)&flag,sizeof(FLAG_TOUCHLEE));
	if (flag)
	{
		/* grabar log */
		char tmp[51];	
		FILE *file;
		file = fopen("/dsdbtocado.log","a");
		if (file)
		{
			dsdb_lseek(base_fd,DBPOS_IDTOUCH,SEEK_SET);	
			dsdb_lee(base_fd,tmp,sizeof(rw_user));
			fprintf(file,"1:%s ",tmp);
			dsdb_lee(base_fd,tmp,sizeof(nomapli));
			fprintf(file,"%s ",tmp);
			dsdb_lee(base_fd,tmp,sizeof(modulo));
			fprintf(file,"%s ",tmp);
			dsdb_lee(base_fd,tmp,sizeof(tmp));
			fprintf(file,"%s\r\n",tmp);
			fclose(file);
		}
		return 1;
	}
	return 0;
}

static void dsdb_tocando_mapa(int fd,ds_off pos)
{ /* Puesto al bloquear de escritura */
	unsigned char flag = (unsigned char)0xff;
	extern char *fecha();
	char tmp[51];	

	pos += OFFSET_PADDING_TOUCH;
	dsdb_lseek(fd,pos,SEEK_SET);
	dsdb_graba(fd,(void *)&flag,sizeof(char));

	dsdb_lseek(fd,DBPOS_IDTOUCH,SEEK_SET);	
	dsdb_graba(fd,rw_user,sizeof(rw_user));
	dsdb_graba(fd,nomapli,sizeof(nomapli));
	dsdb_graba(fd,modulo,sizeof(modulo));	
	sprintf(tmp,"%s %s",fecha(),fecha()+11);
	dsdb_graba(fd,tmp,sizeof(tmp));	
}

static void dsdb_fin_tocando_mapa(DBFile *pfile)
{ /* Puesto a mano */
	unsigned char flag = (unsigned char)0x00;
	ds_off pos;
	static char vacio[25] = {0,0,0,};

	int fd = BasesAbiertas[pfile->base_fd].handle;
	ulong tambloque = BasesAbiertas[pfile->base_fd].header.blocksize;

	pos = (ds_off)(pfile->direntry.pinidata-1) * (ds_off)tambloque + (ds_off)OFFSET_PADDING;		
	
	pos += OFFSET_PADDING_TOUCH;
	dsdb_lseek(fd,pos,SEEK_SET);
	dsdb_graba(fd,(void *)&flag,sizeof(char));

	dsdb_lseek(fd,DBPOS_IDTOUCH,SEEK_SET);	
	dsdb_graba(fd,vacio,sizeof(rw_user));
	dsdb_graba(fd,vacio,sizeof(nomapli));
	dsdb_graba(fd,vacio,sizeof(modulo));

}

static int dsdb_esta_mapa_tocado(int fd,ds_off pos)
{
	unsigned char flag = (unsigned char)0xff;
	pos += OFFSET_PADDING_TOUCH;
	dsdb_lseek(fd,pos,SEEK_SET);
	dsdb_lee(fd,(void *)&flag,sizeof(char));
	if (flag)
	{
		/* grabar log */
		char tmp[51];	
		FILE *file;
		file = fopen("/dsdbtocado.log","a");
		if (file)
		{
			dsdb_lseek(fd,DBPOS_IDTOUCH,SEEK_SET);	
			dsdb_lee(fd,tmp,sizeof(rw_user));
			fprintf(file,"2:%s ",tmp);
			dsdb_lee(fd,tmp,sizeof(nomapli));
			fprintf(file,"%s ",tmp);
			dsdb_lee(fd,tmp,sizeof(modulo));
			fprintf(file,"%s ",tmp);
			dsdb_lee(fd,tmp,sizeof(tmp));
			fprintf(file,"%s\r\n",tmp);
			fclose(file);
		}
		return 1;
	}
	return 0;
}

/* BLOQUEOS */

static void dsdb_suma_bloqueo(DBFile *pfile,ds_off pos,ds_off nbytes,int modo)
{
	int i,n;
	if (!modo)
	{
		n = pfile->nbloqueos;
		if (n > 0)
		{
			for (i = 0;i < n;i++)
			{
				if (pfile->bloqueo[i] == pos && pfile->lenbloqueo[i] == nbytes)
				{
					pfile->nbloqueos--;
					n--;
					for (;i < n;i++)
					{
						pfile->bloqueo[i] = pfile->bloqueo[i+1];
						pfile->lenbloqueo[i] = pfile->lenbloqueo[i+1];
					}
					return;
				}
			}
		}
	}
	else
	{
		i = pfile->nbloqueos;
		if (i >= MAXDSDBBLOQUEOS)
		{
			return;
		}
		pfile->bloqueo[i] = pos;
		pfile->lenbloqueo[i] = nbytes;
		pfile->nbloqueos++;
	}
}


static int dsdb_bloq_escritura_directorio(int b_fd,int base_fd)
{	   
	char c;
    
	if (BasesAbiertas[b_fd].refbloqueoescritura > 0)
	{
		BasesAbiertas[b_fd].refbloqueoescritura++;
		return 0;
	}

	while(-1)
	{
		if (dsdb_lseek(base_fd,DBLOCK_FLAG_REC,SEEK_SET) == (ds_off)-1)
		{
			 DSDB_ERROR(201);
			 return -1;
		}
		if (dsdb_lee(base_fd,&c,1) == 1) break;
		/*if (!dsdb_locking(base_fd,9999,1L)) break;*/
		if (errno != EACCES && errno != EAGAIN && errno != 13 && errno != 35)
		{
			DSDB_ERROR(202);
			 return -1;
		}		
		dsdb_usleep(1);
	}   

	if (dsdb_lseek(base_fd,DBLOCK_FLAG_DIR,SEEK_SET) == (ds_off)-1)
	{
		DSDB_ERROR(203);
	    return -1;
	}
    while(-1) /* se espera (de momento asi) indefinidamente */
	{ 
        if (!dsdb_locking(base_fd,LK_WAIT,1L)) break;
        if (errno != EACCES && errno != EDEADLOCK && errno != EAGAIN  && errno != 13 && errno != 35)
		{
			DSDB_ERROR(204);
			return -1;
		}
		dsdb_usleep(1);
    }
 
	if (dsdb_lseek(base_fd,DBPOS_LOCK_DIR,SEEK_SET) == (ds_off)-1)
	{
		DSDB_ERROR(206);
		return -1;
	}
    while(-1) 
    {
		if (!dsdb_locking(base_fd,LK_WAIT,(long)DBLOCK_ALL)) break;
	    if (errno != EACCES && errno != EDEADLOCK && errno != EAGAIN  && errno != 13 && errno != 35)
		{ 
		  DSDB_ERROR(205);
          dsdb_lseek(base_fd,DBLOCK_FLAG_DIR,SEEK_SET);
          dsdb_locking(base_fd,LK_UNLCK,1L);		  
          return -1;
		}
		dsdb_usleep(1);
    }
	
    dsdb_lseek(base_fd,DBLOCK_FLAG_DIR,SEEK_SET);
    dsdb_locking(base_fd,LK_UNLCK,1L);

	BasesAbiertas[b_fd].refbloqueoescritura++;

	if (dsdb_esta_tocado_dir(base_fd))
	{
		int dsdb_desbloq_escritura_directorio(int b_fd,int base_fd);
		DSDB_ERROR(189);
		dsdb_desbloq_escritura_directorio(b_fd,base_fd);
		return -1;
	}

	return 0;
}


int dsdb_desbloq_escritura_directorio(int b_fd,int base_fd)
{
	if (BasesAbiertas[b_fd].refbloqueoescritura > 0)
	{
		BasesAbiertas[b_fd].refbloqueoescritura--;
		if (BasesAbiertas[b_fd].refbloqueoescritura > 0)
		{
			return 0;
		}
	}

	if (dsdb_lseek(base_fd,DBPOS_LOCK_DIR,SEEK_SET) == (ds_off)-1)
	{
		return -1;
	}
	if (dsdb_locking(base_fd,LK_UNLCK,(long)DBLOCK_ALL) != 0) 
	{
		return -1;
	}

	return 0;
}

static int dsdb_bloq_lectura_directorio(int b_fd,int base_fd,unsigned short openid)
{
    char c;
	int n;

	if (BasesAbiertas[b_fd].refbloqueoescritura > 0)
	{
		return 0;
	}

	while(-1)
	{
		if (dsdb_lseek(base_fd,DBLOCK_FLAG_REC,SEEK_SET) == (ds_off)-1)
		{
			 return -1;
		}
		if (dsdb_lee(base_fd,&c,1) == 1) break;
		/*if (!dsdb_locking(base_fd,9999,1L)) break;*/
		if (errno != EACCES && errno != EAGAIN && errno != 13 && errno != 35)
		{
			 return -1;
		}		
		dsdb_usleep(1);
	}   
	
	n = 0;
	while(-1)
	{
		if (dsdb_lseek(base_fd,DBLOCK_FLAG_DIR,SEEK_SET) == (ds_off)-1)
		{
			 return -1;
		}
		if (dsdb_lee(base_fd,&c,1) == 1) break;
		/*if (!dsdb_locking(base_fd,9999,1L)) break;*/
		if (errno != EACCES && errno != EAGAIN && errno != 13 && errno != 35)
		{
			 return -1;
		}		
		dsdb_usleep(1);
		if (n >= 50)
		{
			if (dsdb_lseek(base_fd,DBLOCK_FLAG_DIR,SEEK_SET) == (ds_off)-1)
			{
				 return -1;
			}
			if (!dsdb_locking(base_fd,LK_WAIT,1L))
			{
				break;
			}
		}
		else
			n++;
	}   
   
	if (dsdb_lseek(base_fd,DBPOS_LOCK_DIR+(ds_off)openid,SEEK_SET) == (ds_off)-1)
	{
		if (n >= 50)
		{
			dsdb_lseek(base_fd,DBLOCK_FLAG_DIR,SEEK_SET);
			dsdb_locking(base_fd,LK_UNLCK,1L);
		}
		return -1;
	}
	while(-1) 
	{
      if (!dsdb_locking(base_fd,LK_WAIT,1L)) break;
      if (errno != EACCES && errno != EDEADLOCK && errno != EAGAIN  && errno != 13 && errno != 35)
      {
			if (n >= 50)
			{
				dsdb_lseek(base_fd,DBLOCK_FLAG_DIR,SEEK_SET);
				dsdb_locking(base_fd,LK_UNLCK,1L);
			}
		  return -1;
      }
	  dsdb_usleep(1);
	}

	if (n >= 50)
	{
		dsdb_lseek(base_fd,DBLOCK_FLAG_DIR,SEEK_SET);
		dsdb_locking(base_fd,LK_UNLCK,1L);
	}

	if (dsdb_esta_tocado_dir(base_fd))
	{
		int dsdb_desbloq_lectura_directorio(int b_fd,int base_fd,unsigned short openid);
		DSDB_ERROR(190);
		dsdb_desbloq_lectura_directorio(b_fd,base_fd,openid);
		return -1;
	}

	return 0;
}

int dsdb_desbloq_lectura_directorio(int b_fd,int base_fd,unsigned short openid)
{
	if (BasesAbiertas[b_fd].refbloqueoescritura > 0)
	{
		return 0;
	}
	if (dsdb_lseek(base_fd,DBPOS_LOCK_DIR+(ds_off)openid,SEEK_SET) == (ds_off)-1)
	{
		return -1;
	}
	if (dsdb_locking(base_fd,LK_UNLCK,1L) != 0)
	{
		return -1;
	}

	return 0;
}

static int dsdb_bloq_escritura_freelist(int base_fd)
{
	if (dsdb_lseek(base_fd,DBPOS_LOCK_FREELIST,SEEK_SET) == (ds_off)-1)
	{
	    return -1;
	}
    while(-1)
	{ 
        if (!dsdb_locking(base_fd,LK_WAIT,1L)) break;
        if (errno != EACCES && errno != EDEADLOCK && errno != EAGAIN  && errno != 13 && errno != 35)
		{
			return -1;
		}
		dsdb_usleep(1);
    }	
	return 0;
}

static int dsdb_desbloq_escritura_freelist(int base_fd)
{
	if (dsdb_lseek(base_fd,DBPOS_LOCK_FREELIST,SEEK_SET) == (ds_off)-1)
	{
	    return -1;
	}
	if (dsdb_locking(base_fd,LK_UNLCK,1L) != 0)
	{
		return -1;
	}
	return 0;
}

static int dsdb_bloq_escritura_mapa(DBFile *pfile)
{
	ds_off pos1,pos2;
	ulong tambloque = BasesAbiertas[pfile->base_fd].header.blocksize;		
	int fd = BasesAbiertas[pfile->base_fd].handle;
	ulong identry;
	char c;


	while(-1)
	{
		if (dsdb_lseek(fd,DBLOCK_FLAG_REC,SEEK_SET) == (ds_off)-1)
		{
			 return -1;
		}
		if (dsdb_lee(fd,&c,1) == 1) break;
		/*if (!dsdb_locking(base_fd,9999,1L)) break;*/
		if (errno != EACCES && errno != EAGAIN && errno != 13 && errno != 35)
		{
			 return -1;
		}		
		dsdb_usleep(1);
	}   


	pos1 = (ds_off)(pfile->direntry.pinidata-1) * (ds_off)tambloque + (ds_off)OFFSET_PADDING;
	if (dsdb_lseek(fd,pos1,SEEK_SET) == (ds_off)-1)
	{
			DSDB_ERROR(147);
			return -1;
	}
    while(-1) 
	{ 
        if (!dsdb_locking(fd,LK_WAIT,1L)) break;
        if (errno != EACCES && errno != EDEADLOCK && errno != EAGAIN  && errno != 13 && errno != 35)
		{
			return -1;
		}
		dsdb_usleep(1);
    }

	if (pfile->identry == 0xffffffff)
	{
		pos2 = (ds_off)(pfile->direntry.pinidata-1) * (ds_off)tambloque + (ds_off)OFFSET_IDENTRY;
		if (dsdb_lseek(fd,pos2,SEEK_SET) == (ds_off)-1)
		{
				DSDB_ERROR(149);
				return -1;
		}
		if (dsdb_lee(fd,(void *)&identry,sizeof(ulong)) != sizeof(ulong))
		{
				dsdb_lseek(fd,pos1,SEEK_SET);
				dsdb_locking(fd,LK_UNLCK,1L);
				DSDB_ERROR(150);
				return -1;
		}
		pfile->identry = identry;
	}
	else
		identry = pfile->identry;

	pos2 = DBPOS_LOCK_MAP + (ds_off)identry * LENMAPLOCK;
	if (dsdb_lseek(fd,pos2,SEEK_SET) == (ds_off)-1)
	{
			DSDB_ERROR(151);
			return -1;
	}
    while(-1) 
	{ 
        if (!dsdb_locking(fd,LK_WAIT,(ulong)DBLOCK_ALL)) break;
        if (errno != EACCES && errno != EDEADLOCK && errno != EAGAIN  && errno != 13 && errno != 35)
		{
		    dsdb_lseek(fd,pos1,SEEK_SET);
			dsdb_locking(fd,LK_UNLCK,1L);
			return -1;
		}
		dsdb_usleep(1);
    }

	if (dsdb_lseek(fd,pos1,SEEK_SET) == (ds_off)-1)
	{
			DSDB_ERROR(148);
			return -1;
	}
	if (dsdb_locking(fd,LK_UNLCK,1L) != 0)
	{
			DSDB_ERROR(149);
			return -1;
	}

	if (dsdb_esta_mapa_tocado(fd,pos1))
	{
		int dsdb_desbloq_escritura_mapa(DBFile *pfile);
		DSDB_ERROR(192);
		dsdb_desbloq_escritura_mapa(pfile);
		return -1;
	}

	dsdb_tocando_mapa(fd,pos1);

	return 0;
}

int dsdb_desbloq_escritura_mapa(DBFile *pfile)
{
	ds_off pos2;	
	int fd = BasesAbiertas[pfile->base_fd].handle;

	if (pfile->identry == 0xffffffff)
	{
		return -1;
	}

	pos2 = DBPOS_LOCK_MAP + (ds_off)pfile->identry * LENMAPLOCK;
	if (dsdb_lseek(fd,pos2,SEEK_SET) == (ds_off)-1)
	{
			DSDB_ERROR(152);
			return -1;
	}
	if (dsdb_locking(fd,LK_UNLCK,(ulong)DBLOCK_ALL) != 0)
	{
			DSDB_ERROR(153);
			return -1;
	}
	return 0;
}

static int dsdb_bloq_lectura_mapa(DBFile *pfile)
{
	ds_off pos1,pos2;
	ulong tambloque = BasesAbiertas[pfile->base_fd].header.blocksize;
	ds_off openid = (ds_off)BasesAbiertas[pfile->base_fd].openid;
	ulong padding;
	int fd = BasesAbiertas[pfile->base_fd].handle;
	ulong identry;
	int n;
	char c;


	while(-1)
	{
		if (dsdb_lseek(fd,DBLOCK_FLAG_REC,SEEK_SET) == (ds_off)-1)
		{
			 return -1;
		}
		if (dsdb_lee(fd,&c,1) == 1) break;
		/*if (!dsdb_locking(base_fd,9999,1L)) break;*/
		if (errno != EACCES && errno != EAGAIN && errno != 13 && errno != 35)
		{
			 return -1;
		}		
		dsdb_usleep(1);
	}   

	pos1 = (ds_off)(pfile->direntry.pinidata-1) * (ds_off)tambloque + (ds_off)OFFSET_PADDING;

	n = 0;
	while(-1)
	{
		if (dsdb_lseek(fd,pos1,SEEK_SET) == (ds_off)-1)
		{
			DSDB_ERROR(146);
			return -1;
		}
		/*if (!dsdb_locking(fd,9999,1L)) break;*/
		if (dsdb_lee(fd,(void *)&padding,1) == 1) break;
		if (errno != EACCES && errno != EAGAIN && errno != 13 && errno != 35)
		{
			 return -1;
		}
		dsdb_usleep(1);
		if (n >= 50)
		{
			if (dsdb_lseek(fd,pos1,SEEK_SET) == (ds_off)-1)
			{				
				return -1;
			}
			if (!dsdb_locking(fd,LK_WAIT,1L)) 
			{
				break;
			}
		}
		else
			n++;
	}

    if (pfile->identry == 0xffffffff)
	{
		pos2 = (ds_off)(pfile->direntry.pinidata-1) * (ds_off)tambloque + (ds_off)OFFSET_IDENTRY;
		if (dsdb_lseek(fd,pos2,SEEK_SET) == (ds_off)-1)
		{
				if (n >= 50)
				{
					dsdb_lseek(fd,pos1,SEEK_SET);
					dsdb_locking(fd,LK_UNLCK,1L);
				}
				DSDB_ERROR(149);
				return -1;
		}
		if (dsdb_lee(fd,(void *)&identry,sizeof(ulong)) != sizeof(ulong))
		{
				if (n >= 50)
				{
					dsdb_lseek(fd,pos1,SEEK_SET);
					dsdb_locking(fd,LK_UNLCK,1L);
				}
				DSDB_ERROR(150);
				return -1;
		}
		pfile->identry = identry;
	}
	else
		identry = pfile->identry;

	pos2 = DBPOS_LOCK_MAP + (ds_off)identry * LENMAPLOCK + (ds_off)openid;
	if (dsdb_lseek(fd,pos2,SEEK_SET) == (ds_off)-1)
	{
			if (n >= 50)
			{
				dsdb_lseek(fd,pos1,SEEK_SET);
				dsdb_locking(fd,LK_UNLCK,1L);
			}
			DSDB_ERROR(151);
			return -1;
	}
    while(-1) 
	{ 
        if (!dsdb_locking(fd,LK_WAIT,1L)) break;
        if (errno != EACCES && errno != EDEADLOCK && errno != EAGAIN  && errno != 13 && errno != 35)
		{
			if (n >= 50)
			{
				dsdb_lseek(fd,pos1,SEEK_SET);
				dsdb_locking(fd,LK_UNLCK,1L);
			}
			return -1;
		}
		dsdb_usleep(1);
    }

	if (n >= 50)
	{
		dsdb_lseek(fd,pos1,SEEK_SET);
		dsdb_locking(fd,LK_UNLCK,1L);
	}
	if (dsdb_esta_mapa_tocado(fd,pos1))
	{
		int dsdb_desbloq_lectura_mapa(DBFile *pfile);
		DSDB_ERROR(191);
		dsdb_desbloq_lectura_mapa(pfile);
		return -1;
	}

	return 0;
}

/* ACCESOS A BAJO NIVEL */
int dsdb_desbloq_lectura_mapa(DBFile *pfile)
{
	ds_off pos2;
	ds_off openid = (ds_off)BasesAbiertas[pfile->base_fd].openid;
	int fd = BasesAbiertas[pfile->base_fd].handle;

	if (pfile->identry == 0xffffffff)
	{
		return -1;
	}

	pos2 = DBPOS_LOCK_MAP + (ds_off)pfile->identry * LENMAPLOCK + (ds_off)openid;

	if (dsdb_lseek(fd,pos2,SEEK_SET) == (ds_off)-1)
	{
			DSDB_ERROR(168);
			return -1;
	}
    if (dsdb_locking(fd,LK_UNLCK,1L) != 0)
	{
			DSDB_ERROR(154);
			return -1;
    }
	return 0;
}

static int dsdb_bloqueo_exclusivo(int fd,DBDirEntry *pdirentry,ulong *pidentry)
{
	ds_off pos2;
	ulong tambloque = BasesAbiertas[fd].header.blocksize;		
	ulong identry;
	int handle = BasesAbiertas[fd].handle;

	if (!pdirentry->pinidata)
	{
		return 0;
	}

	if (*pidentry == 0xffffffff)
	{
		pos2 = (ds_off)(pdirentry->pinidata-1) * (ds_off)tambloque + (ds_off)OFFSET_IDENTRY;
		if (dsdb_lseek(handle,pos2,SEEK_SET) == (ds_off)-1)
		{
				DSDB_ERROR(157);
				return -1;
		}
		if (dsdb_lee(handle,(void *)pidentry,sizeof(ulong)) != sizeof(ulong))
		{
				DSDB_ERROR(158);
				return -1;
		}		
	}	
	identry = *pidentry;

	pos2 = DBPOS_LOCK_FILE + (ds_off)identry * LENUSERLOCK + DBOFFEXLOCK;
	if (dsdb_lseek(handle,pos2,SEEK_SET) == (ds_off)-1)
	{
			DSDB_ERROR(159);
			return -1;
	}
    if (dsdb_locking(handle,LK_NBLCK,LENFILELOCK) != 0)
	{
		{
			FILE *f;
		    f = fopen("/tmp/prob207.txt","a");
			if (f)
			{
				fprintf(f,"control de 207 file:%s\r\n",pdirentry->name);
				fclose(f);
			}
		}
		DSDB_ERROR(207);
		return -1;
	}
	return 0;
}

static int dsdb_desbloqueo_exclusivo(int fd,ulong *pidentry)
{
	ds_off pos2;	
	int handle = BasesAbiertas[fd].handle;

	if (*pidentry == 0xffffffff)
	{
		return -1;
	}

	pos2 = DBPOS_LOCK_FILE + (ds_off)(*pidentry) * LENUSERLOCK + DBOFFEXLOCK;
	if (dsdb_lseek(handle,pos2,SEEK_SET) == (ds_off)-1)
	{
			DSDB_ERROR(160);
			return -1;
	}
    if (dsdb_locking(handle,LK_UNLCK,LENFILELOCK) != 0)
	{
		return -1;
	}
	return 0;
}

static int dsdb_bloqueo_abierto(DBFile *pfile,int extra)
{
	ds_off pos2;
	ulong tambloque = BasesAbiertas[pfile->base_fd].header.blocksize;	
	int fd = BasesAbiertas[pfile->base_fd].handle;
	ds_off openid = (ds_off)BasesAbiertas[pfile->base_fd].openid;	
	ulong identry;

	if (!pfile->direntry.pinidata)
	{
		return 0;
	}

	if (pfile->identry == 0xffffffff)
	{
		pos2 = (ds_off)(pfile->direntry.pinidata-1) * (ds_off)tambloque + (ds_off)OFFSET_IDENTRY;
		if (dsdb_lseek(fd,pos2,SEEK_SET) == (ds_off)-1)
		{
				DSDB_ERROR(157);
				return -1;
		}
		if (dsdb_lee(fd,(void *)&identry,sizeof(ulong)) != sizeof(ulong))
		{
				DSDB_ERROR(158);
				return -1;
		}
		pfile->identry = identry;
	}
	else
		identry = pfile->identry;

	pos2 = DBPOS_LOCK_FILE + (ds_off)identry * LENUSERLOCK + DBOFFEXLOCK + openid*DBLOCK_ALL + extra;
	if (dsdb_lseek(fd,pos2,SEEK_SET) == (ds_off)-1)
	{
			DSDB_ERROR(159);
			return -1;
	}
    if (dsdb_locking(fd,LK_NBLCK,1) != 0)
	{
		return -1;
	}
	return 0;
}


static int dsdb_desbloqueo_abierto(DBFile *pfile,int extra)
{
	ds_off pos2;
	int fd = BasesAbiertas[pfile->base_fd].handle;
	ds_off openid = (ds_off)BasesAbiertas[pfile->base_fd].openid;	

	if (!pfile->direntry.pinidata)
	{
		return 0;
	}
	if (pfile->identry == 0xffffffff)
	{
		return -1;
	}

	pos2 = DBPOS_LOCK_FILE + (ds_off)pfile->identry * LENUSERLOCK + DBOFFEXLOCK + openid*DBLOCK_ALL + extra;
	if (dsdb_lseek(fd,pos2,SEEK_SET) == (ds_off)-1)
	{
			DSDB_ERROR(160);
			return -1;
	}
    if (dsdb_locking(fd,LK_UNLCK,1) != 0)
	{
		return -1;
	}
	return 0;
}



static int dsdb_graba_bloque(int fd,long bloque,void *data,int len,int tambloque,int pskip,int nskip)
{
	ds_off pos;
	if (bloque < 1)
	{
		DSDB_ERROR(6);
		return -1;
	}
	if (data != bloque_tmp1)
	{
		memcpy(bloque_tmp1,data,len);
		memset(bloque_tmp1+len,0,tambloque-len);
	}
	bloque--;
	pos = (ds_off)bloque * (ds_off)tambloque;
	if (dsdb_lseek(fd,pos,SEEK_SET) == (ds_off)-1)
	{
		DSDB_ERROR(7);
		return -1;
	}
	if (data == bloque_tmp1)
	{
		tambloque = len;
	}

	if (pskip >= tambloque)
	{
		if (dsdb_graba(fd,bloque_tmp1,tambloque) != tambloque)
		{
			DSDB_ERROR(8);
			return -1;
		}
	}
	else
	{
		if (pskip > 0)
		{
			if (dsdb_graba(fd,bloque_tmp1,pskip) != pskip)
			{
				DSDB_ERROR(8);
				return -1;
			}
		}
		if (dsdb_lseek(fd,nskip,SEEK_CUR) == (ds_off)-1)
		{
			DSDB_ERROR(7);
			return -1;
		}
		tambloque -= (pskip+nskip);
		if (tambloque > 0)
		{
			if (dsdb_graba(fd,bloque_tmp1+pskip+nskip,tambloque) != tambloque)
			{
				DSDB_ERROR(8);
				return -1;
			}
		}
	}
	return 0;
}

static int dsdb_bloqueo_virtual(DBFile *pfile,ds_off posicion,int modo,ulong nbytes)
{
	ds_off pos2;
	ulong tambloque = BasesAbiertas[pfile->base_fd].header.blocksize;
	ds_off openid = (ds_off)BasesAbiertas[pfile->base_fd].openid;	
	int fd = BasesAbiertas[pfile->base_fd].handle;
	ulong identry;
	int r;

    if (pfile->identry == 0xffffffff)
	{
		pos2 = (ds_off)(pfile->direntry.pinidata-1) * (ds_off)tambloque + (ds_off)OFFSET_IDENTRY;
		if (dsdb_lseek(fd,pos2,SEEK_SET) == (ds_off)-1)
		{
				DSDB_ERROR(166);
				return -1;
		}
		if (dsdb_lee(fd,(void *)&identry,sizeof(ulong)) != sizeof(ulong))
		{
				DSDB_ERROR(165);
				return -1;
		}
		pfile->identry = identry;
	}
	else
		identry = pfile->identry;

	pos2 = DBPOS_LOCK_FILE + (ds_off)identry * LENUSERLOCK + DBOFFUSERLOCK + posicion;
	if (dsdb_lseek(fd,pos2,SEEK_SET) == (ds_off)-1)
	{
			DSDB_ERROR(167);
			return -1;
	}
	r = dsdb_locking(fd,modo,nbytes);
	
	if (r >= 0)
	{
		dsdb_suma_bloqueo(pfile,pos2,nbytes,modo);
	}
	
	
	return r;
}



static int dsdb_graba_dato(int fd,long bloque,void *data,int len,int offset_bloque,int tambloque)
{
	ds_off pos;
	if (bloque < 1)
	{
		DSDB_ERROR(21);
		return -1;
	}
	bloque--;
	pos = ((ds_off)bloque * (ds_off)tambloque) + (ds_off)offset_bloque;
	if (dsdb_lseek(fd,pos,SEEK_SET) == (ds_off)-1)
	{
		DSDB_ERROR(22);
		return -1;
	}
	if (dsdb_graba(fd,data,len) != len)
	{
		DSDB_ERROR(23);
		return -1;
	}
	return 0;
}


static int dsdb_lee_bloque(int fd,long bloque,void *_data,int tambloque,int tamdata,int pskip,int nskip)
{
	char *data = (char *)_data; /* por cosas del fedora */

	ds_off pos;
	if (bloque < 1)
	{
		DSDB_ERROR(11);
		return -1;
	}
	bloque--;
	pos = (ds_off)bloque * (ds_off)tambloque;
	if (dsdb_lseek(fd,pos,SEEK_SET) == (ds_off)-1)
	{
		DSDB_ERROR(12);
		return -1;
	}
	if (pskip >= tamdata)
	{
		if (dsdb_lee(fd,data,tamdata) != tamdata)
		{
			DSDB_ERROR(13);
			return -1;
		}
	}
	else
	{
		if (dsdb_lee(fd,data,pskip) != pskip)
		{
			DSDB_ERROR(13);
			return -1;
		}
		/*(char *)data += pskip;*/
		data += pskip;
		memset(data,0,nskip);
		/*(char *)data += nskip;*/
		data += nskip;
		if (dsdb_lseek(fd,nskip,SEEK_CUR) == (ds_off)-1)
		{
			DSDB_ERROR(12);
			return -1;
		}
		tamdata -= (pskip+nskip);
		if (dsdb_lee(fd,data,tamdata) != tamdata)
		{
			DSDB_ERROR(13);
			return -1;
		}
	}
	return 0;
}

/* ARBOLES */
static int dsdb_busca_en_bloque(char **_pdata,char *compara,int nitems,int size,int len,ulong tipo)
{
	int r = -1;
	char *pdata = *_pdata;
	nitems--;	
	if (tipo == IDBMPB)
	{
		if ((r = COMPARA_OFFSETS(*(ds_off *)compara,*(ds_off *)pdata)) > 0)
		{	
			/* de momento para simplificar un recorrido secuencial estupido, luego ya harems la particion */		
			while(nitems)
			{
				pdata += len;
				nitems--;
				if ((r = COMPARA_OFFSETS(*(ds_off *)compara,*(ds_off *)pdata)) <= 0)
					break;
			}
			if (r < 0)
			{
				pdata -= len;
				r = COMPARA_OFFSETS(*(ds_off *)compara,*(ds_off *)pdata);
			}
		}
	}
	else
	{
		if ((r = memcmp(compara,pdata,size)) > 0)
		{	
			/* de momento para simplificar un recorrido secuencial estupido, luego ya harems la particion */		
			while(nitems)
			{
				pdata += len;
				nitems--;
				if ((r = memcmp(compara,pdata,size)) <= 0)
					break;
			}
			if (r < 0)
			{
				pdata -= len;
				r = memcmp(compara,pdata,size);
			}
		}
	}
	*_pdata = pdata;
	if (r < 0)
		r = 0;
	else if (r > 0)
		r = 2;
	else
		r = 1;
	return r;
}


ulong dsdb_nuevo_bloque(int base_fd) /* la cabecera debe estar ya "refrescada" */
{	
	int handle = BasesAbiertas[base_fd].handle;
	ulong pfreeblock;
	ulong nblo;
	ulong blocksize = BasesAbiertas[base_fd].header.blocksize;
	DBLibre libre;


	if (BasesAbiertas[base_fd].refbloqueoescritura < 999999)
	{
		if (dsdb_bloq_escritura_freelist(handle) < 0)
		{
			DSDB_ERROR(38);
			return 0;
		}
	}

	if (dsdb_lseek(handle,OFFSET_NTOTALBLOCKS,SEEK_SET) == (ds_off)-1)
	{
		DSDB_ERROR(182);
		return -1;
	}
	if (dsdb_lee(handle,(void *)&(BasesAbiertas[base_fd].header.ntotalblocks),sizeof(ulong)) != sizeof(ulong))
	{
		DSDB_ERROR(183);
		return -1;
	}	
	if (dsdb_lseek(handle,OFFSET_PFREEBLOCK,SEEK_SET) == (ds_off)-1)
	{
		DSDB_ERROR(184);
		return -1;
	}	
	if (dsdb_lee(handle,(void *)&(BasesAbiertas[base_fd].header.pfreeblock),sizeof(ulong)) != sizeof(ulong))
	{
		DSDB_ERROR(185);
		return -1;
	}
	pfreeblock = BasesAbiertas[base_fd].header.pfreeblock;


	if (pfreeblock)
	{
		nblo = pfreeblock;
		if (dsdb_lee_bloque(handle,nblo,(void *)&libre,blocksize,sizeof(libre),0,0) < 0)
		{
			DSDB_ERROR(42);
			dsdb_desbloq_escritura_freelist(handle);			
			return 0;
		}
		if (libre.id != IDBFDB)
		{
			DSDB_ERROR(43);
			dsdb_desbloq_escritura_freelist(handle);			
			return 0;
		}
		pfreeblock = libre.pnext;
		if (dsdb_graba_dato(handle,1L,(void *)&(pfreeblock),sizeof(pfreeblock),OFFSET_PFREEBLOCK,sizeof(pfreeblock)/* irrelevante */) < 0)
		{
			DSDB_ERROR(39);
			dsdb_desbloq_escritura_freelist(handle);			
			return 0;
		}	
		BasesAbiertas[base_fd].header.pfreeblock = pfreeblock;
	}
	else
	{
		nblo = ++(BasesAbiertas[base_fd].header.ntotalblocks);
		if (dsdb_graba_dato(handle,1L,(void *)&(nblo),sizeof(nblo),OFFSET_NTOTALBLOCKS,sizeof(nblo)/* irrelevante */) < 0)
		{
			DSDB_ERROR(44);
			dsdb_desbloq_escritura_freelist(handle);			
			return 0;
		}	
		if (rw_dsdb_bloquesaumento > 1)
		{
			ds_off tama,total;
			tama = (ds_off)blocksize * (ds_off)nblo;
		    if (tama > ((total = dsdb_lseek(handle,0L,SEEK_END))  - (rw_dsdb_bloquesaumento*blocksize)))
			{
				total /= blocksize;
				total += rw_dsdb_bloquesaumento;
				dsdb_graba_bloque(handle,(ulong)total,"",1,blocksize,0,0);
			}
		}
	}

	if (BasesAbiertas[base_fd].refbloqueoescritura < 999999)
	{
		if (dsdb_desbloq_escritura_freelist(handle) < 0)
		{
			DSDB_ERROR(40);
			return 0;
		}
	}
	return nblo;	
}


static int dsdb_elimina_bloque_mapa(int handle,ulong nblo,ulong blocksize,ulong *previo /* extrablocks*/)
{
	ulong *bloques = NULL;
	int nbloques = 0,i;
	DBLibre libre;
	
    if (dsdb_lee_bloque(handle,nblo,(void *)bloque_tmp2,blocksize,blocksize,0,0) < 0)
    {
		DSDB_ERROR(84);		
		return -1;
    }
	libre.id = IDBFDB;
	libre.nextrablocks = 0L;
    libre.pnext = *previo;
    if (dsdb_graba_dato(handle,nblo,(void *)&libre,sizeof(libre),0L,blocksize) < 0)
    { 
		DSDB_ERROR(86);		
		return -1;
    }	
	*previo = nblo;
	
	if (((DBArbol *)bloque_tmp2)->level)
	{
		char *pdata;
		nbloques = ((DBArbol *)bloque_tmp2)->nitems;
		bloques = (ulong *)malloc(nbloques);
		pdata = bloque_tmp2 + sizeof(DBArbol) + sizeof(ds_off);
		for (i = 0;i < nbloques;i++)
		{
			bloques[i] = *((ulong *)pdata);
			pdata += (sizeof(ds_off)+sizeof(ulong));
		}
		for (i = 0;i < nbloques;i++)
		{
			if (dsdb_elimina_bloque_mapa(handle,bloques[i],blocksize,previo) < 0)
			{
				free(bloques);
				DSDB_ERROR(87);		
				return -1;
			}
		}
		free(bloques);
	}
	return 0;
}

static int dsdb_libera_bloque(int base_fd,ulong nblo,ulong blocksize,int modo) /* la cabecera debe estar ya "refrescada" */
{	
	int handle = BasesAbiertas[base_fd].handle;
	ulong pfreeblock;
	DBLibre libre;

	if (dsdb_bloq_escritura_freelist(handle) < 0)
	{
		DSDB_ERROR(67);
		return -1;
	}
	

	if (dsdb_lseek(handle,OFFSET_PFREEBLOCK,SEEK_SET) == (ds_off)-1)
	{
		DSDB_ERROR(186);
		return -1;
	}	
	if (dsdb_lee(handle,(void *)&(BasesAbiertas[base_fd].header.pfreeblock),sizeof(ulong)) != sizeof(ulong))
	{
		DSDB_ERROR(187);
		return -1;
	}

	pfreeblock = BasesAbiertas[base_fd].header.pfreeblock;	

	libre.id = IDBFDB;
	if (!modo) /* borramos este bloque */
	{				
		libre.pnext = pfreeblock;	

		if (dsdb_graba_dato(handle,nblo,(void *)&libre,sizeof(libre),0L,blocksize) < 0)
		{ /* los bloques libres los dejamos sucios para no hacer tan penoso el borrado */
			DSDB_ERROR(70);
			dsdb_desbloq_escritura_freelist(handle);
			return -1;
		}

		pfreeblock = nblo;		
	}
	else if (modo == 1) /* borramos una cadena de bloques */
	{		
	   DBLibre otro; /* todas las estructuras encadenadas tienen que empezar asi */	   
	   ulong ll = nblo;	   
	   while(ll)
	   {
		   if (dsdb_lee_bloque(handle,ll,(void *)&otro,blocksize,sizeof(otro),0,0) < 0)
		   {
				DSDB_ERROR(82);
				dsdb_desbloq_escritura_freelist(handle);
				return -1;
		   }		   
		   libre.pnext = otro.pnext;
		   if (!libre.pnext)
		   {
			   libre.pnext = pfreeblock;
		   }
		   if (dsdb_graba_dato(handle,ll,(void *)&libre,sizeof(libre),0L,blocksize) < 0)
		   { 
			    DSDB_ERROR(83);
			    dsdb_desbloq_escritura_freelist(handle);
			    return -1;
		   }
		   if (!otro.pnext)
		   {
			   break;
		   }
		   ll = otro.pnext;
	   }	   
	   pfreeblock = nblo;	 
	}
	else if (modo == 2) /* borramos un arbol de bloques de mapa */
	{ /* ojo: SOLO PARA ARBOL DE MAPA ... */		
		if (dsdb_elimina_bloque_mapa(handle,nblo,blocksize,&pfreeblock) < 0)
		{
			DSDB_ERROR(85);
			dsdb_desbloq_escritura_freelist(handle);
			return -1;
		}
	}
	
	if (dsdb_graba_dato(handle,1L,(void *)&(pfreeblock),sizeof(pfreeblock),OFFSET_PFREEBLOCK,sizeof(pfreeblock)/* irrelevante */) < 0)
	{
		DSDB_ERROR(68);
		dsdb_desbloq_escritura_freelist(handle);			
		return -1;
	}	
	BasesAbiertas[base_fd].header.pfreeblock = pfreeblock;

	if (dsdb_desbloq_escritura_freelist(handle) < 0)
	{
		DSDB_ERROR(69);
		return -1;
	}
	return 0;	
}


static int dsdb_busca_en_arbol(DBArbolBusca *busca,ulong bini,ulong level,int modo)
{ /* devuelve: -1 error, 0 < 1 = 2 > */
	DBArbol *parbol;
	char *pdata;
	char *pinidata;
	int len;
	int nitems;
	int r;	
	int size;
	char *compara;	
	int handle = busca->handle;

	info_arbol++;

	if (dsdb_lee_bloque(handle,bini,bloque_tmp3,busca->blocksize,busca->blocksize,0,0) < 0)
	{
		DSDB_ERROR(29);
		return -1;
	}
	parbol = (DBArbol *)bloque_tmp3;
	if (parbol->id != busca->id)
	{
		DSDB_ERROR(30);
		return -1;
	}
	if ((long)parbol->nitems <= 0)
	{
		if (!(long)parbol->level)
		{
			busca->padres[0] = 0;
			busca->rbloque = bini;
			busca->roffset = sizeof(DBArbol);
			busca->r = 2;
			return 2;
		}		
		DSDB_ERROR(31);
		return -1;
	}
	if ((long)parbol->level < 0)
	{
		DSDB_ERROR(32);
		return -1;
	}
	if ((long)level == -1)
	{
		level = parbol->level;
		busca->padres[level] = 0;
	}
	else if (level != parbol->level)
	{
		DSDB_ERROR(33);
		return -1;
	}
	if (level)
	{	
		len = busca->sizex + sizeof(ulong);
	}
	else
	{
		len = busca->size0;
	}

	compara = busca->compara;
	size    = busca->sizex;
	nitems = (int)parbol->nitems;
	pinidata = pdata = ((char *)parbol)+sizeof(DBArbol);
	r = dsdb_busca_en_bloque(&pdata,compara,nitems,size,len,busca->id);
	if (level)
	{
		if (modo == 2)
		{
			if ((nitems-1) > (int)(((int)(pdata - (char *)parbol) - sizeof(DBArbol))/len))
			{
				pdata += len;				
				modo = 3;
			}
			else
			{
				if (busca->padres[level])
				{
					bini = busca->padres[level];
					level++;					
					return dsdb_busca_en_arbol(busca,bini,level,modo);
				}
				modo = 4;
			}			
		}
		level--;
		busca->padres[level] = bini;
		pdata += busca->sizex;
		bini = *(ulong *)pdata;
		return dsdb_busca_en_arbol(busca,bini,level,modo);
	}

	if (modo == 1 || modo == 4)
	{		
		if (r == 2 || r == 1)
		{
			if ((nitems-1) > (int)(((int)(pdata - (char *)parbol) - sizeof(DBArbol))/len))
			{
				pdata += len;				
				if (modo == 4)
				{
					DSDB_ERROR(173);
					return -1;
				}
			}
			else
			{
				if (modo == 1 && busca->padres[level])
				{
					bini = busca->padres[level];
					level++;					
					return dsdb_busca_en_arbol(busca,bini,level,2);
				}
				/* fin de arbol */
				r = 3;
			}
		}
		else
		{
			if (modo == 4)
			{
				DSDB_ERROR(174);
				return -1;
			}
		}
	}
	memcpy(busca->resultado,pdata,busca->size0);
	busca->r = r;
	busca->rbloque = bini;
	busca->roffset = (int)(pdata - (char *)parbol) - sizeof(DBArbol);	
	if (busca->id == IDBMPB && busca->pfile)
	{
		/* incluir lo que tenemos en el mapa */
		ds_off pos;
		long x;
		ulong blocksize;
		int uno,dos;		
		DBFile *pfile;
		int i;

		pfile = busca->pfile;
		pos = *((ds_off *)pinidata);		

		blocksize =  busca->blocksize;
		uno = blocksize-sizeof(DBIniData);
		dos = blocksize-sizeof(DBMoreData);
		if (pos < uno)
		{
			x = 0;
		}
		else
		{
			x = (long)(ds_off)((ds_off)(pos-(ds_off)uno)/(ds_off)dos) + 1;			
		}
		pinidata += size;
		for (i = 0;i < nitems && x < pfile->maxlmapa;i++)		
		{
			pfile->lmapa[x] = *((unsigned long *)pinidata);
			pinidata += len;
			x++;
		}
	}
	return r;
}

        
static int dsdb_arbol_nuevo_bloque(int handle,ulong nblo,ulong id,void *data,int len,ulong blocksize,ulong level,ulong nitems)
{
	((DBArbol *)bloque_tmp1)->id = id;
	((DBArbol *)bloque_tmp1)->level = level;	
	((DBArbol *)bloque_tmp1)->nitems = nitems;
	memcpy((void *)(bloque_tmp1+sizeof(DBArbol)),data,len);
	memset(bloque_tmp1+sizeof(DBArbol)+len,0,blocksize-(sizeof(DBArbol)+len));
	if (dsdb_graba_bloque(handle,nblo,bloque_tmp1,blocksize,blocksize,0,0) < 0)
	{
		DSDB_ERROR(46);		
		return -1;
	}
	return 0;
}

static int dsdb_inserta_en_arbol(DBArbolBusca *busca,int base_fd,int equilibrado,char *bloque,ulong *pdirectory)
{ /* Inserta en el arbol despues de una busqueda o al menos con "busca" en iguales condiciones */
  /* el resultado tiene que haber sido cambiado por el elemento a insertar */
  /* en bloque_tmp3 debe de estar el bloque valido de la busqueda */
  int r = 0;
  int nitems = ((DBArbol *)bloque)->nitems;
  int maxitems;
  int offset;
  int resto;
  int size;
  ulong blocksize = busca->blocksize;
  ulong nblo = 0L;
  ulong nivel;
  int nuevopadre = 0;

  info_iarbol++;

  if (((DBArbol *)bloque)->id != busca->id)
  {
	  DSDB_ERROR(48);		
	  return -1;
  }

  nivel = ((DBArbol *)bloque)->level;
  if (nivel > 0)
  {
	  size = busca->sizex+sizeof(ulong);
  }
  else
  {
	  size = busca->size0;
  }  

  maxitems = (busca->blocksize-sizeof(DBArbol)) / size;


  offset = busca->roffset; 
  resto =  (nitems * size) -  offset;

  ((DBArbol *)bloque)->nitems++;
  nitems++;

  if (busca->r > 1 && nitems > 1)
  {
	  offset += size;
	  resto -= size;
  }
  offset += sizeof(DBArbol);
  memcpy(bloque_tmp4,bloque,offset);
  memcpy(bloque_tmp4+offset,busca->resultado,size);    
  if (resto > 0)
  {
	  memcpy(bloque_tmp4+offset+size,bloque+offset,resto);
  }
  offset += (size+resto);
  if (nitems > maxitems)
  {
	  ulong padre;
	  int sizex;	  

	  sizex = busca->sizex+sizeof(ulong);	  
	  padre = busca->padres[nivel];
	  if (!padre)
	  {		  
		  if (nivel >= DBMAXNIVELES)
		  {
			 DSDB_ERROR(127);
			 return -1;
		  }
		  nuevopadre = 1;
		  padre = dsdb_nuevo_bloque(base_fd);
		  busca->padres[nivel] = padre;
		  busca->padres[nivel+1] = 0;
		  if (!padre)
		  {
			 DSDB_ERROR(51);
			 return -1;
		  }
		  /* crear padre, se inaugura un nuevo nivel */
		  memcpy(bloque_tmp2,bloque_tmp4+sizeof(DBArbol),busca->sizex);
		  *((ulong *)(bloque_tmp2+busca->sizex)) = busca->rbloque;
		  if (dsdb_arbol_nuevo_bloque(busca->handle,padre,busca->id,bloque_tmp2,sizex,blocksize,((DBArbol *)bloque_tmp4)->level+1,1) < 0)
		  {
			 DSDB_ERROR(53);
			 return -1;
		  }
		  *pdirectory = padre;  /* nuevo padre */		  
	  }
	  nblo = dsdb_nuevo_bloque(base_fd);
	  if (!nblo)
	  {
		 DSDB_ERROR(52);
		 return -1;
	  }
	  if (equilibrado)
	  {
		  offset = nitems/2;
	  }
	  else
	  {
		  offset = maxitems;
	  }

	  ((DBArbol *)bloque_tmp4)->nitems = offset;
	  nitems -=offset;
	  ((DBArbol *)bloque_tmp2)->id = busca->id;
	  ((DBArbol *)bloque_tmp2)->level = ((DBArbol *)bloque_tmp4)->level;	  
	  ((DBArbol *)bloque_tmp2)->nitems = nitems;	  

	  offset *= size;	  
	  offset += sizeof(DBArbol);
	  memcpy(bloque_tmp2+sizeof(DBArbol),bloque_tmp4+offset,nitems*size);
	  
	  if (dsdb_graba_bloque(busca->handle,nblo,bloque_tmp2,nitems*size+sizeof(DBArbol),blocksize,0,0) < 0)
	  {
		 DSDB_ERROR(54);
		 return -1;
	  }	  
	  offset = ((DBArbol *)bloque_tmp4)->nitems * size + sizeof(DBArbol);
  }

  if (dsdb_graba_bloque(busca->handle,busca->rbloque,bloque_tmp4,offset,blocksize,0,0) < 0)
  {
	  DSDB_ERROR(50);
	  return -1;
  }

  if (nblo)
  {
	  /* leer padre y localizar punto de insercion */
	  char *pdata;
	  
	  if (dsdb_lee_bloque(busca->handle,busca->padres[nivel],bloque,blocksize,blocksize,0,0) < 0)
	  {
	  	 DSDB_ERROR(55);
         return -1;
	  }

	  if (((DBArbol *)bloque)->level != (((DBArbol *)bloque_tmp2)->level + 1))
	  {
	 	 DSDB_ERROR(131);
		 return -1;
	  }

	  if (!nuevopadre)
	  {		  
		  if (busca->r < 1)
		  {
			  if (busca->roffset)
			  {
	 			 DSDB_ERROR(128);
				 return -1;
			  }
			  /* Actualizar comienzo bloque padre */
			  memcpy(bloque+sizeof(DBArbol),bloque_tmp4+sizeof(DBArbol),busca->sizex);
		  }
		  /* en la posterior insercion se grabara */
	  }

	  memcpy(busca->resultado,bloque_tmp2+sizeof(DBArbol),busca->sizex);
	  *((ulong *)((char *)busca->resultado+busca->sizex)) = nblo;
	  pdata = bloque+sizeof(DBArbol);
	  busca->r = dsdb_busca_en_bloque(&pdata,bloque_tmp4+sizeof(DBArbol),((DBArbol *)bloque)->nitems,busca->sizex,busca->sizex+sizeof(ulong),busca->id);
	  if (busca->r < 0)
	  {
	 	 DSDB_ERROR(140);
		 return -1;
	  }
	  if (busca->r < 1)
	  {
		  /* actualizar bloque padre con valor al primer elemento */
		  memcpy(pdata,bloque_tmp4+sizeof(DBArbol),busca->sizex);
	  }
	  busca->r = 2; /* siempre detras a de ser */
	  if (*((ulong *)((char *)pdata+busca->sizex)) != busca->rbloque)
	  {
	 	 DSDB_ERROR(132);
		 return -1;
	  }	  
	  busca->roffset = (pdata - bloque) - sizeof(DBArbol);	  
	  busca->rbloque = busca->padres[nivel];	  
	  return dsdb_inserta_en_arbol(busca,base_fd,equilibrado,bloque,pdirectory);
  }
  
  return r;
}

static int dsdb_borra_en_arbol(DBArbolBusca *busca,int base_fd,char *bloque,ulong *pdirectory)
{
  int r = 0;
  int nitems = ((DBArbol *)bloque)->nitems;  
  int offset;
  int resto;
  int size;
  ulong blocksize = busca->blocksize;
  ulong nblo = 0L;
  ulong nivel;

  if (((DBArbol *)bloque)->id != busca->id)
  {
	DSDB_ERROR(64);
	return -1;
  }

  nivel = ((DBArbol *)bloque)->level;
  if (nivel > 0)
  {
	size = busca->sizex+sizeof(ulong);
  }
  else
  {
	size = busca->size0;
  }
  ((DBArbol *)bloque)->nitems--;
  nitems--;
  if (!nitems)
  {
	  char *pdata;
	  ulong padre;
	  int sizex;

	  /* ojo: usa bloque_tmp1 */
	  if (dsdb_libera_bloque(base_fd,busca->rbloque,blocksize,0) < 0)
	  {
		  DSDB_ERROR(66);
		  return -1;
	  }

	  sizex = busca->sizex+sizeof(ulong);	  
	  padre = busca->padres[nivel];
	  if (!padre)
	  {
		  *pdirectory = 0L;
		  return 0;
	  }
	  if (dsdb_lee_bloque(busca->handle,padre,bloque,blocksize,blocksize,0,0) < 0)
	  {
	  	 DSDB_ERROR(71);
         return -1;
	  }
	  pdata = bloque+sizeof(DBArbol);
	  busca->r = dsdb_busca_en_bloque(&pdata,busca->resultado,((DBArbol *)bloque)->nitems,busca->sizex,busca->sizex+sizeof(ulong),busca->id);
	  if (*((ulong *)(pdata+busca->sizex)) != busca->rbloque)
	  {
	  	 DSDB_ERROR(72);
         return -1;
	  }
	  busca->rbloque = padre;
	  busca->roffset = (pdata - bloque) - sizeof(DBArbol);	  
	  return dsdb_borra_en_arbol(busca,base_fd,bloque,pdirectory);
  }
  offset = busca->roffset;
  resto =  (nitems * size) -  offset;
  offset += sizeof(DBArbol);

  memcpy(bloque_tmp1,bloque,offset);  
  if (resto > 0)
  {
	  memcpy(bloque_tmp1+offset,bloque+offset+size,resto);
  }
  offset += resto;
  memset(bloque_tmp1+offset,0,size);
  offset += size;
  if (dsdb_graba_bloque(busca->handle,busca->rbloque,bloque_tmp1,offset,blocksize,0,0) < 0)
  {
	  DSDB_ERROR(65);
	  return -1;
  }
  return r;
}

/* PRINCIPALES */

/* el filtro de base de datos es por directorio no por nombre de fichero */
static char *_filtro[] = 
{
	"*/fich/*/",	
	NULL
};
int dsdb_coincide_filtro(char *name)
{
	static char **filtro = (char **)_filtro;
	int r = 0;
	int i,off;
	char tmp[512];
	char *p;
	static int cargado = 0;	

	if (rw_dsdb_filtro && !cargado)
	{
		FILE *file;
		file = fopen(rw_dsdb_filtro,"r");
		if (file)
		{
			i = 0;
			filtro = (char **)malloc(sizeof(char *)*101);
			while(fgets(tmp,500,file) && i < 100)
			{
				p = tmp + strlen(tmp);
				while(p > tmp)
				{
					p--;
					if (*p == ' ' || *p == '\r' || *p == '\n' || *p == 9)
					   *p = 0;
					else
						break;					
				}
				if (tmp[0])
				{
					filtro[i] = malloc(strlen(tmp)+1);
					strcpy(filtro[i],tmp);
					i++;
				}
			}
			filtro[i] = NULL;
			fclose(file);
		}
		cargado = 1;
	}
	if (!filtro)
		return 1;	

	strcpy(tmp,name);
	*quita_path(tmp) = 0;
	cambia_barra(tmp);
	for (i = 0;filtro[i];i++)
	{
		if (*(filtro[i]) == '-')
			continue;

		if (*(filtro[i]) == '+')
		  off = 1;
		else
		  off = 0;
		if (coincide_(tmp,filtro[i]+off))
		{
			r = 1;		
			break;
		}		
	}
	for (i = 0;filtro[i];i++)
	{
		if (*(filtro[i]) == '-')
		{
			if (coincide_(tmp,filtro[i]+1))
			{
				r = 0;
				break;
			}
		}
	}
	return r;
}

static int dsdb_pasafiltro(const char *filename)
{
	char patron[30];	
	int kk;	
	

	for (kk = 0;dsdb_extensiones[kk];kk++)
	{
		patron[0] = '*';
		strcpy(patron+1,dsdb_extensiones[kk]);
		if (coincide_((char *)filename,patron))
			break;
	}
	if (dsdb_extensiones[kk])
	{
		return dsdb_coincide_filtro((char *)filename);
	}
	return 0;
}


static int dsdb_refresca_cabecera(int handle,ulong *pnupdate,DBHeader *pheader)
{
	DBFullHeader fheader;

	/* no usamos el nupdate ... leemeos la cabecera siempre ... lento pero pensando en la
	   futuro servidor central */
	if (dsdb_lee_bloque(handle,1L,(void *)&fheader,sizeof(DBFullHeader),sizeof(DBFullHeader),DBLOCK_FLAG_DIR,1) < 0)
	{
		DSDB_ERROR(25);
		return -1;
	}

	*pnupdate = fheader.nupdate;
	*pheader = fheader.header;

	refrecos_cab++;
	
	return 0;
}

static int dsdb_crea_base(const char *base)
{
	int handle;
	DBFullHeader header;

	if (rw_dsdb_tambloque < 1 || rw_dsdb_tambloque > 128)
		rw_dsdb_tambloque = 1;
	dsdb_blocksize = DEFDBBLOCKSIZE * rw_dsdb_tambloque;	
	dsdb_ajusta_tmps(dsdb_blocksize);

	handle = dsdb_sabre(base,O_RDWR | O_BINARY | O_CREAT ,SH_DENYRW,S_IREAD|S_IWRITE);
	if (handle < 0)
	{		
		return -1;	
	}

	if (rw_dsdb_bloquesdefecto > 1 || rw_dsdb_bloquesaumento > 1)
	{		
		if (rw_dsdb_bloquesdefecto > (rw_dsdb_bloquesaumento*2))
		{
			dsdb_graba_bloque(handle,rw_dsdb_bloquesdefecto,"",1,dsdb_blocksize,0,0);
		}
		else
		{
			dsdb_graba_bloque(handle,rw_dsdb_bloquesaumento*2,"",1,dsdb_blocksize,0,0);
		}
		dsdb_lseek(handle,(ds_off)0,SEEK_SET);
	}
	
	memset((void *)&header,0,sizeof(header));
	header.id = IDDSDB;	
    if (!dsdb_usa_crypto)
	{
		header.ssize   = sizeof(DBFullHeader1);	
		header.version = DSDB_MIN_VERSION; /* DSDB_VERSION; */
	}
	else
	{
		unsigned char t1[8];
		unsigned char clave[18];
		/*int basura;*/
		memcpy(clave,dsdb_,18);
		header.ssize   = sizeof(header);	
		header.version = DSDB_VERSION;
		header.flags  |= DSDB_FLAG_CRYPTO;
	    *((ulong *)t1) = header.id;
		*((ulong *)( t1 + 4)) = header.ssize;
		if (es_empresa_copia(base))
		{
			int i;
			header.flags  |= DSDB_FLAG_COPIA;
			for (i = 0;i < 16;i++)
			{
				clave[i] ^= keycopia[i];
			}
		}
		dsdb_c_code((long *)t1,(long *)t1,(long *)clave);
		header.test1 = *((ulong *)t1);
		header.test2 = *((ulong *)( t1 + 4));
	}
	memset(bloque_tmp1,0,dsdb_blocksize);
	header.header.blocksize = dsdb_blocksize;
	header.header.ntotalblocks = 1L; /* este */
	memcpy(bloque_tmp1,(void *)(void *)&header,sizeof(header));
	{ /* rellenar con basura, numeros aleatorios de diferente momento */
		int basura;
		/*int i;		*/
		for (basura = sizeof(header);basura < (sizeof(header)+101);basura++)
		{	
			bloque_tmp1[basura] = DSDB_RANDON[basura&0xf] ^ rand();
		}
		for (;basura < (sizeof(header)+141);basura++)
		{			
			bloque_tmp1[basura] = DSDB_RANDON[basura&0xf] ^ (42+(basura&0xf));
		}
		for (;basura < DBPOS_IDTOUCH;basura++)
		{			
			bloque_tmp1[basura] = rand();
		}
	}
	if (dsdb_graba_bloque(handle,1L,(void *)bloque_tmp1,dsdb_blocksize,dsdb_blocksize,0,0) < 0)
	{
		DSDB_ERROR(2);
		return -2; /* en este caso no reintentar obertura */
	}

	dsdb_cierra(handle);
	return 0;
}

static int dsdb_localiza_tabla(int base_fd,const char *tabla,DBDirEntry *direntry)
{
	int handle;
	int r = 2;
	int i;
	DBHeader *pheader;
    ulong blocksize;
	char _tabla[MAXNAMEDB];
	/* de momento nos creemos base_fd  */

	if (!tabla)
	{
		memcpy(_tabla,direntry->name,MAXNAMEDB);
		tabla = _tabla;
	}
	handle = BasesAbiertas[base_fd].handle;
	blocksize = BasesAbiertas[base_fd].header.blocksize;


	if (dsdb_bloq_lectura_directorio(base_fd,handle,BasesAbiertas[base_fd].openid) < 0)
	{
		DSDB_ERROR(16);		
		return -1;
	}

	pheader = &(BasesAbiertas[base_fd].header);
	if (dsdb_refresca_cabecera(handle,&(BasesAbiertas[base_fd].nupdate),pheader) < 0)
	{
		DSDB_ERROR(20);
		return -1;
	}

	if (pheader->pdirectory)
	{
		DBArbolBusca busca;
		busca.handle = handle;
		busca.id = IDBDDB;
		busca.compara = (void *)tabla;
		busca.sizex = sizeof(direntry->name);
		busca.size0 = sizeof(DBDirEntry);
		busca.blocksize = blocksize;
		busca.resultado = (void *)direntry;
	    busca.r = -1;
		busca.rbloque = 0;
		busca.roffset = 0;
		busca.padres[0] = 0;
		busca.pfile = NULL;
		r = dsdb_busca_en_arbol(&busca,pheader->pdirectory,(ulong)(long)-1,0);
		if (r < 0)
		{
			DSDB_ERROR(34);
			dsdb_desbloq_lectura_directorio(base_fd,handle,BasesAbiertas[base_fd].openid);			
			return -1;
		}
	}

	if (dsdb_desbloq_lectura_directorio(base_fd,handle,BasesAbiertas[base_fd].openid) < 0)
	{
		DSDB_ERROR(24);
		return -1;
	}

	return r;
}


static int dsdb_crea_tabla(int base_fd,const char *tabla,DBDirEntry *direntry,DBDirEntry *inicial)
{ /* optimizacion por hacer ... si hemos leido el directorio y no ha cambiado la cabecera .. 
     no volver a leerlo */
	int handle;
	int r = 0;
	DBHeader *pheader;	
    ulong blocksize;
	ulong pdirectory;
	/* de momento nos creemos base_fd  */
	handle = BasesAbiertas[base_fd].handle;
	blocksize = BasesAbiertas[base_fd].header.blocksize;

	if (dsdb_bloq_escritura_directorio(base_fd,handle) < 0)
	{
		DSDB_ERROR(35);		
		return -1;
	}

	if (BasesAbiertas[base_fd].refbloqueoescritura < 999999)
	{
		dsdb_tocando_dir(handle);
	}

	pheader = &(BasesAbiertas[base_fd].header);
	if (dsdb_refresca_cabecera(handle,&(BasesAbiertas[base_fd].nupdate),pheader) < 0)
	{
		DSDB_ERROR(37);
		return -1;
	}	

	pdirectory = pheader->pdirectory;
	if (pdirectory)
	{		
		DBArbolBusca busca;
		busca.handle = handle;
		busca.id = IDBDDB;
		busca.compara = (void *)tabla;
		busca.sizex = sizeof(direntry->name);
		busca.size0 = sizeof(DBDirEntry);
		busca.blocksize = blocksize;
		busca.resultado = (void *)direntry;
	    busca.r = -1;
		busca.rbloque = 0;
		busca.roffset = 0;
		busca.pfile = NULL;
		r = dsdb_busca_en_arbol(&busca,pdirectory,(ulong)(long)-1,0);
		if (r < 0)
		{
			DSDB_ERROR(34);
			dsdb_desbloq_escritura_directorio(base_fd,handle);			
			return -1;
		}
		if (r == 1)
		{
			dsdb_fin_tocando_dir(handle);
			dsdb_desbloq_escritura_directorio(base_fd,handle);			
			return r;
		}
		/* bloque_tmp3 y busca tienen todos lo datos */
		if (inicial)
		{
			*direntry = *inicial;
		}
		else
		{
			memset((void *)direntry,0,sizeof(DBDirEntry));
			strcpy(direntry->name,tabla);
		}
		if (dsdb_inserta_en_arbol(&busca,base_fd,1,bloque_tmp3,&pdirectory) < 0)
		{
			DSDB_ERROR(45);
			dsdb_desbloq_escritura_directorio(base_fd,handle);			
			return -1;
		}

	}
	else
	{		
		pdirectory = dsdb_nuevo_bloque(base_fd);
		if (!pdirectory)
		{
			DSDB_ERROR(41);
			dsdb_desbloq_escritura_directorio(base_fd,handle);			
			return -1;
		}

		if (inicial)
		{
			*direntry = *inicial;
		}
		else
		{
			memset((void *)direntry,0,sizeof(DBDirEntry));
			strcpy(direntry->name,tabla);
		}

		if (dsdb_arbol_nuevo_bloque(handle,pdirectory,IDBDDB,direntry,sizeof(DBDirEntry),blocksize,0,1) < 0)
		{
			DSDB_ERROR(47);
			dsdb_desbloq_escritura_directorio(base_fd,handle);
			return -1;
		}
	}	

	if (pdirectory != pheader->pdirectory)
	{
		pheader->pdirectory = pdirectory;				
		if (dsdb_graba_dato(handle,1L,(void *)&(pdirectory),sizeof(pdirectory),OFFSET_PDIRECTORY,sizeof(pdirectory)/* irrelevante */) < 0)
		{
			DSDB_ERROR(37);
			dsdb_desbloq_escritura_directorio(base_fd,handle);			
			return -1;
		}	
	}

	/*
	BasesAbiertas[base_fd].nupdate++;
	BasesAbiertas[base_fd].oupdate = BasesAbiertas[base_fd].nupdate;
	if (dsdb_lseek(handle,(ds_off)(sizeof(ulong)*3),SEEK_SET) != (ds_off)-1)
	{	
		dsdb_graba(handle,(void *)&(BasesAbiertas[base_fd].nupdate),sizeof(BasesAbiertas[base_fd].nupdate));
	}
	*/

	if (BasesAbiertas[base_fd].refbloqueoescritura < 999999)
	{
		dsdb_fin_tocando_dir(handle);
	}

	if (dsdb_desbloq_escritura_directorio(base_fd,handle) < 0)
	{
		DSDB_ERROR(36);
		return -1;
	}
	return r;
}

static int dsdb_comprueba_handle(int handle)
{
	int base_fd;

	if ((handle & 0x40000000))
	{
		handle &= ~0x40000000;
	}
    if (handle < 0 || handle > ndsdb_base_files || dsdb_base_files[handle].base_fd < 0)
	{		
		return -1;
	}
	base_fd = dsdb_base_files[handle].base_fd;	
	DSDB_ERROR_QUE_BASE(base_fd);
	if ((unsigned)base_fd >= (unsigned)nBasesAbiertas || BasesAbiertas[base_fd].handle < 0)
	{		
		DSDB_ERROR(90);
		return -1;
	}
	return handle;
}


/* Si se especifica pbusca es para borrado total, sino solo reset, se conserva el bloque inicial */
int dsdb_base_vaciatabla(int base_fd,DBDirEntry *direntry,DBArbolBusca *pbusca,int filehandle)
{
	DBArbolBusca busca;
	int handle;
	int r = 2;
	DBHeader *pheader;
    ulong blocksize;
	DBIniData inidata;
	DBMapEntry mapentry;
	DBFile *pfile = NULL;
	
	if (!direntry)
	{
		filehandle = dsdb_comprueba_handle(filehandle);
		if (filehandle < 0)
			return -1;		
		direntry = &(dsdb_base_files[filehandle].direntry);
	}

    if (!direntry->pinidata)
	{
		return 0;
	}

	handle = BasesAbiertas[base_fd].handle;
	blocksize = BasesAbiertas[base_fd].header.blocksize;

	pheader = &(BasesAbiertas[base_fd].header);
	if (!pbusca)
	{
		if (dsdb_bloq_lectura_directorio(base_fd,handle,BasesAbiertas[base_fd].openid) < 0)
		{
			DSDB_ERROR(74);
			return -1;
		}	

		if (dsdb_refresca_cabecera(handle,&(BasesAbiertas[base_fd].nupdate),pheader) < 0)
		{
			DSDB_ERROR(75);
			return -1;
		}		

		if (pheader->pdirectory)
		{			
			char tabla[MAXNAMEDB];            

			memcpy(tabla,direntry->name,MAXNAMEDB);
			busca.handle = handle;
			busca.id = IDBDDB;
			busca.compara = (void *)tabla;
			busca.sizex = sizeof(direntry->name);
			busca.size0 = sizeof(DBDirEntry);
			busca.blocksize = blocksize;
			busca.resultado = (void *)direntry;
			busca.r = -1;
			busca.rbloque = 0;
			busca.roffset = 0;
			busca.pfile = NULL;
			r = dsdb_busca_en_arbol(&busca,pheader->pdirectory,(ulong)(long)-1,0);
			if (r != 1)
			{
				DSDB_ERROR(76);
				dsdb_desbloq_lectura_directorio(base_fd,handle,BasesAbiertas[base_fd].openid);
				return -1;
			}
			if (!direntry->pinidata)
			{
				dsdb_desbloq_lectura_directorio(base_fd,handle,BasesAbiertas[base_fd].openid);
				return 0;
			}

		}
		else
		{
			DSDB_ERROR(77);
			dsdb_desbloq_lectura_directorio(base_fd,handle,BasesAbiertas[base_fd].openid);
			return -1;
		}
		if (dsdb_desbloq_lectura_directorio(base_fd,handle,BasesAbiertas[base_fd].openid) < 0)
		{
			DSDB_ERROR(78);
			return -1;
		}
		pfile = &(dsdb_base_files[filehandle]);
		if (dsdb_bloq_escritura_mapa(pfile) < 0)
		{
			DSDB_ERROR(194);			
			return -1;
		}
	}	
	else
	{
		busca = *pbusca;
	}	
	if (dsdb_lee_bloque(handle,direntry->pinidata,(void *)&inidata,blocksize,sizeof(inidata),OFFSET_PADDING,sizeof(ulong)) < 0)
	{
		DSDB_ERROR(88);
		if (!pbusca)
		{
			dsdb_desbloq_escritura_mapa(pfile);
		}
		return -1;
	}
	if (!pbusca)
	{
		if (inidata.pnext)
		{
			if (dsdb_libera_bloque(base_fd,inidata.pnext,blocksize,1) < 0)
			{
				  DSDB_ERROR(196);
				  dsdb_desbloq_escritura_mapa(pfile);
				  return -1;
			}
			inidata.pnext = 0L;
			
			if (dsdb_libera_bloque(base_fd,inidata.pmapa,blocksize,2) < 0)
			{
				  DSDB_ERROR(197);
				  dsdb_desbloq_escritura_mapa(pfile);
				  return -1;
			}				

			inidata.pmapa = dsdb_nuevo_bloque(base_fd);

			((DBArbol *)bloque_tmp1)->id = IDBMPB;
			((DBArbol *)bloque_tmp1)->level  = 0;
			((DBArbol *)bloque_tmp1)->nitems = 1;				
			mapentry.pos = (ds_off)0;
			mapentry.pbloque = direntry->pinidata;
			memcpy(bloque_tmp1+sizeof(DBArbol),(void *)&mapentry,sizeof(mapentry));
			memset(bloque_tmp1+sizeof(DBArbol)+sizeof(mapentry),0,blocksize-(sizeof(DBArbol)+sizeof(mapentry)));
			pfile->pmapa = inidata.pmapa;
			
			if (dsdb_graba_bloque(handle,inidata.pmapa,(void *)bloque_tmp1,blocksize,blocksize,0,0) < 0)
			{
				DSDB_ERROR(198);
				dsdb_desbloq_escritura_mapa(pfile);
				return -1;
			}			
		}
		inidata.size = 0L;
		if (dsdb_graba_bloque(handle,direntry->pinidata,(void *)&inidata,sizeof(inidata),blocksize,OFFSET_PADDING,sizeof(ulong)) < 0)
		{
			DSDB_ERROR(193);			
			dsdb_desbloq_escritura_mapa(pfile);
			return -1;
		}
		pfile->size = 0L;
		pfile->posicion = 0;
		pfile->bloquecorriente = 0L;
		dsdb_fin_tocando_mapa(pfile);
		if (dsdb_desbloq_escritura_mapa(pfile) < 0)
		{
			DSDB_ERROR(195);
			return -1;
		}
	}
	else
	{
		if (dsdb_libera_bloque(base_fd,direntry->pinidata,blocksize,1) < 0)
		{
			  DSDB_ERROR(80);
			  return -1;
		}
		direntry->pinidata = 0L;			

		if (inidata.pmapa)
		{
			if (dsdb_libera_bloque(base_fd,inidata.pmapa,blocksize,2) < 0)
			{
				  DSDB_ERROR(81);
				  return -1;
			}		
		}

		if (dsdb_graba_dato(handle,busca.rbloque,(void *)direntry,sizeof(DBDirEntry),busca.roffset+sizeof(DBArbol),blocksize) < 0)
		{
			DSDB_ERROR(79);
			return -1;
		}	
	}

	return 0;
}

static int dsdb_borra_renombra_tabla(int base_fd,char *tabla,char *tabla2)
{
	int handle;
	int r = -1,i;
	int algo = 0;
	DBHeader *pheader;	
    ulong blocksize;
	ulong pdirectory;
	DBDirEntry *direntry,de,nueva;	
	ulong identry = 0xffffffff;
	/* de momento nos creemos base_fd  */
	direntry = &de;
	handle = BasesAbiertas[base_fd].handle;
	blocksize = BasesAbiertas[base_fd].header.blocksize;

	if (dsdb_bloq_escritura_directorio(base_fd,handle) < 0)
	{
		DSDB_ERROR(56);		
		return -1;
	}

	pheader = &(BasesAbiertas[base_fd].header);
	if (dsdb_refresca_cabecera(handle,&(BasesAbiertas[base_fd].nupdate),pheader) < 0)
	{
		DSDB_ERROR(57);
		dsdb_desbloq_escritura_directorio(base_fd,handle);			
		return -1;
	}		

	pdirectory = pheader->pdirectory;
	if (pdirectory)
	{		
		DBArbolBusca busca;
		busca.handle = handle;
		busca.id = IDBDDB;
		busca.sizex = sizeof(direntry->name);
		busca.size0 = sizeof(DBDirEntry);
		busca.blocksize = blocksize;
		busca.resultado = (void *)direntry;
		busca.r = -1;
		busca.rbloque = 0;
		busca.roffset = 0;
		busca.pfile = NULL;
		if (tabla2)
		{
			busca.compara = (void *)tabla2;
			r = dsdb_busca_en_arbol(&busca,pdirectory,(ulong)(long)-1,0);
			if (r == 1)
			{				
				dsdb_desbloq_escritura_directorio(base_fd,handle);
				{
					char tmp[512];
					sprintf(tmp,"renombra %s a %s existe destino",tabla,tabla2);
				    guarda_error(tmp);
				}
				return -1;
			}
		}
		busca.r = -1;
		busca.rbloque = 0;
		busca.roffset = 0;
		busca.compara = (void *)tabla;
		r = dsdb_busca_en_arbol(&busca,pdirectory,(ulong)(long)-1,0);
		if (r < 0)
		{
			DSDB_ERROR(58);
			dsdb_desbloq_escritura_directorio(base_fd,handle);			
			return -1;
		}
		if (r == 1)
		{
			int bloqueado = 0;
			/* match */
			dsdb_tocando_dir(handle);
			if (direntry->pinidata)
			{
				if (dsdb_bloqueo_exclusivo(base_fd,direntry,&identry) < 0)
				{
					DSDB_ERROR(208);
					GuardaErrores(1);
					dsdb_fin_tocando_dir(handle);
					dsdb_desbloq_escritura_directorio(base_fd,handle);
					errno = EACCES;
					return -1;
				}
				bloqueado = 1;
			}
			if (!tabla2)
			{
				if (direntry->pinidata)
				{
					if (dsdb_base_vaciatabla(base_fd,direntry,&busca,-1) < 0)
					{
						DSDB_ERROR(73);
						dsdb_desbloq_escritura_directorio(base_fd,handle);
						return -1;
					}					
				}
			}
			else
			{
				nueva = de;
			}
			if (bloqueado)
			{
				dsdb_desbloqueo_exclusivo(base_fd,&identry);
			}

			if (dsdb_borra_en_arbol(&busca,base_fd,bloque_tmp3,&pdirectory) < 0)
			{
				DSDB_ERROR(63);
				dsdb_desbloq_escritura_directorio(base_fd,handle);
				return -1;
			}			
			if (tabla2)
			{
				busca.compara = (void *)tabla2;
				busca.r = -1;
				busca.rbloque = 0;
				busca.roffset = 0;
				r = dsdb_busca_en_arbol(&busca,pdirectory,(ulong)(long)-1,0);
				if (r == 1)
				{
					dsdb_fin_tocando_dir(handle);
					dsdb_desbloq_escritura_directorio(base_fd,handle);
					return -1;
				}
				busca.resultado = (void *)&nueva;
				memset((void *)nueva.name,0,sizeof(nueva.name));
				strcpy(nueva.name,tabla2);
				if (dsdb_inserta_en_arbol(&busca,base_fd,1,bloque_tmp3,&pdirectory) < 0)
				{
					DSDB_ERROR(178);
					dsdb_desbloq_escritura_directorio(base_fd,handle);			
					return -1;
				}
			}
			algo = 1;
			if (pdirectory != pheader->pdirectory)
			{
				pheader->pdirectory = pdirectory;				
				if (dsdb_graba_dato(handle,1L,(void *)&(pdirectory),sizeof(pdirectory),OFFSET_PDIRECTORY,sizeof(pdirectory)/* irrelevante */) < 0)
				{
					DSDB_ERROR(61);
					dsdb_desbloq_escritura_directorio(base_fd,handle);			
					return -1;
				}	
			}
			dsdb_fin_tocando_dir(handle);
		}
		else
			info_nounlink++;
	}

	/*
	if (algo)
	{
		BasesAbiertas[base_fd].nupdate++;
		BasesAbiertas[base_fd].oupdate = BasesAbiertas[base_fd].nupdate;
		if (dsdb_lseek(handle,(ds_off)(sizeof(ulong)*3),SEEK_SET) != (ds_off)-1)
		{	
			dsdb_graba(handle,(void *)&(BasesAbiertas[base_fd].nupdate),sizeof(BasesAbiertas[base_fd].nupdate));
		}	
	}
	*/

	if (dsdb_desbloq_escritura_directorio(base_fd,handle) < 0)
	{
		DSDB_ERROR(62);
		return -1;
	}
	return r;
}


static int dsdb_lee_inicial(DBFile *pfile,DBIniData *pinidata)
{	
	int base_fd = pfile->base_fd;

    if (dsdb_lee_bloque(BasesAbiertas[base_fd].handle,pfile->direntry.pinidata,(void *)pinidata,BasesAbiertas[base_fd].header.blocksize,sizeof(DBIniData),OFFSET_PADDING,sizeof(ulong)) < 0)
    {
		DSDB_ERROR(91);
		return -1;
    }	
	pfile->size = pinidata->size;
	pfile->pmapa = pinidata->pmapa;
	pfile->identry = pinidata->identry;
	return 0;
}


static int dsdb_confirma_corriente(DBFile *pfile,int grabacion,DBArbolBusca *pbusca)
{	
	int r = 2;
	long x;
	ulong blocksize;
	int uno,dos;
	ds_off de,hasta;
	
	blocksize =  BasesAbiertas[pfile->base_fd].header.blocksize;
	uno = blocksize-sizeof(DBIniData);
	dos = blocksize-sizeof(DBMoreData);
	if (pfile->posicion < uno)
	{
		x = 0;
		de = 0;
		hasta = uno-1;
	}
	else
	{
		x = (long)(ds_off)((ds_off)(pfile->posicion-(ds_off)uno)/(ds_off)dos);
		de = (ds_off)uno + (ds_off)x*(ds_off)dos;
		hasta = de + (dos - 1);
		x++;
	}

	if (!grabacion)
	{
		if (dsdb_serverip)
		{
			if (pfile->bloquecorriente && pfile->de <= pfile->posicion && pfile->hasta >= pfile->posicion)
			{
				return 1;
			}	
			if (x < pfile->maxlmapa && pfile->lmapa[x])
			{
				pfile->bloquecorriente = pfile->lmapa[x];				
				pfile->de = de;
				pfile->hasta = hasta;
				return 1;
			}		
		}
	}	
	info_confirma++;
	{
		DBArbolBusca busca;
		DBMapEntry mapentry;		
		DBIniData inidata;

		if (!grabacion)
		{
			if (dsdb_bloq_lectura_mapa(pfile) < 0)
			{
				DSDB_ERROR(142);
				return -1;
			}
		}		

		if (dsdb_lee_inicial(pfile,&inidata) < 0)
		{
			if (!grabacion)
				dsdb_desbloq_lectura_mapa(pfile);
			DSDB_ERROR(93);
			return -1;
		}

		pfile->size = inidata.size;

		if (!grabacion && inidata.size <= pfile->posicion)
		{			
			dsdb_desbloq_lectura_mapa(pfile);
			return 3; /* off limits */
		}

		pfile->pmapa = inidata.pmapa;

		memset((void *)&mapentry,0,sizeof(mapentry));		
		busca.handle = BasesAbiertas[pfile->base_fd].handle;
		busca.id = IDBMPB;
		busca.compara = (void *)&pfile->posicion;
		busca.sizex = sizeof(pfile->posicion);
		busca.size0 = sizeof(DBMapEntry);
		busca.blocksize = blocksize;
		busca.resultado = (void *)&mapentry;
	    busca.r = -1;
		busca.rbloque = 0;
		busca.roffset = 0;
		busca.pfile = pfile;
		
		r = dsdb_busca_en_arbol(&busca,inidata.pmapa,(ulong)(long)-1,0);
		if (r < 0)
		{
			if (!grabacion)
				dsdb_desbloq_lectura_mapa(pfile);
			DSDB_ERROR(95);
			return -1;
		}
		if (r < 1)
		{
			if (!grabacion)
				dsdb_desbloq_lectura_mapa(pfile);
			DSDB_ERROR(96);
			return -1;
		}
		if (pbusca)
		{
			*pbusca = busca;
		}
		pfile->bloquecorriente = mapentry.pbloque;
		pfile->de = mapentry.pos;
		if (pfile->bloquecorriente == pfile->direntry.pinidata)
		{
			pfile->hasta = pfile->de + (uno - 1);
		}
		else
		{
			pfile->hasta = pfile->de + (dos - 1);
		}
		
		if (x < pfile->maxlmapa && de == pfile->de && hasta == pfile->hasta)
		{
				pfile->lmapa[x] = pfile->bloquecorriente;
		}
		

		if (!grabacion)
		{
			if (dsdb_desbloq_lectura_mapa(pfile) < 0)
			{
				DSDB_ERROR(143);
				return -1;
			}
		}

		return r;
	}
	return 0;
}

static void saca_base_tabla(const char *filename,char *base,char *tabla)
{
	char *p;
	int  i,j;	
	char tmp[1024];

	j = 0;
	for (i = 0;filename[i];i++)
	{
		tmp[j] = filename[i];
		if (tmp[j] == '/' && filename[i+1] == '/') i++;
		j++;
	}
	tmp[j] = 0;
	/*strcpy(tmp,filename);*/

	p = quita_path(tmp);
	if ((int)(p-tmp) > 5)
	{
		if (!memcmp(p-5,".dsb",4) || !memcmp(p-5,".DSB",4))
		{			
			strcpy(base,tmp);
			p = quita_path(base);
			memset(tabla,0,MAXNAMEDB); 
			strcpy(tabla,p);
			p--;
			*p = 0;
			return;
		}
	}

	memset(tabla,0,MAXNAMEDB); /* muy importante */
	strcpy(tabla,p);	
	strcpy(base,tmp);	
	*(quita_path(base)) = 0;
	j = strlen(base);	
	for (i = 0;tmp[i] && (tmp+i) < p && tmp[i] != '/' && tmp[i] != '\\';i++);
	if ((tmp+i) >= p || (tmp[i] != '/' && tmp[i] != '\\'))
	{
		strcat(base,"dsbase");
		j = strlen(base);		
	}
	else
	{	
		for (;tmp[i] && (tmp+i) < p;i++)
		{
			if (tmp[i] != '/' && tmp[i] != '\\' && tmp[i] != ':')
			{
				base[j] = tmp[i];
				j++;				
			}
		}	
	}
	memcpy(base+j,".dsb",4);
	base[j+4] = 0;
}


int dsdb_u_abre(char *filename, int oflag, int shflag,...)
{
	va_list ap;
	char tabla[MAXNAMEDB];
	char base[MAX_PATH];	
	int base_fd;    
	int i,j;
    int pmode;
	int crear_tabla = 0;
	int resetear_tabla = 0;
	int fd = -1;
	int importacion = -1;
	DBDirEntry direntry;		
	int creado = 0;
	static int recursion = 0;
	/*DWORD test;*/

	/*test = GetTickCount();*/

	info_abre++;

    va_start(ap, shflag);
    pmode = va_arg(ap, int);
    va_end(ap);

	saca_base_tabla(filename,base,tabla);

    switch ( oflag & (O_CREAT | O_EXCL | O_TRUNC) ) 
	{
        case 0:
        case O_EXCL:                   // ignore EXCL w/o CREAT
            //filecreate = OPEN_EXISTING;
            break;		
        case O_CREAT:
            crear_tabla = 1;
            break;
        
        case O_CREAT | O_EXCL:
        case O_CREAT | O_TRUNC | O_EXCL:
            crear_tabla = 1;
			resetear_tabla = 1;
            break;

        case O_TRUNC:
        case O_TRUNC | O_EXCL:			            
			resetear_tabla = 1;
            break;

        case O_CREAT | O_TRUNC:
            crear_tabla = 1;
			resetear_tabla = 1;
            break;

        default:            
            errno = EINVAL;
            _doserrno = 0L;
            return -1;
    }


	if (dsdb_base_free_file > -1)
	{
		int b_fd;
		int ant_fd;
		ant_fd = -1;		
		if (!recursion && dsdb_serverip && !(oflag & (O_CREAT | O_TRUNC | O_EXCL)))
		{
			fd = dsdb_base_free_file;
			while(fd > -1)
			{
				b_fd = dsdb_base_files[fd].base_fd;
				if (b_fd > -1 && !strcmp(BasesAbiertas[b_fd].base,base) && !strcmp(tabla,dsdb_base_files[fd].direntry.name))
				{
					break;
				}
				ant_fd = fd;
				fd = dsdb_base_files[fd].free_file;
			}
			if (fd > -1)
			{
				DBIniData inidata;

				b_fd = dsdb_base_files[fd].base_fd;
				if (b_fd < 0 || strcmp(BasesAbiertas[b_fd].base,base))
				{
					aviso(0,base);
				}
				if (!dsdb_base_files[fd].direntry.pinidata)
				{
					aviso(0,"no pinidata");
				}
				if (dsdb_lee_bloque(BasesAbiertas[b_fd].handle,dsdb_base_files[fd].direntry.pinidata,(void *)&inidata,BasesAbiertas[b_fd].header.blocksize,sizeof(DBIniData),OFFSET_PADDING,sizeof(ulong)) < 0)
				{	
					DSDB_ERROR(214);				
					return -1;
				}
				if (inidata.id == IDBIDB && !strncmp(inidata.name,dsdb_base_files[fd].direntry.name,MAXNAMEDB) && inidata.identry == dsdb_base_files[fd].identry)
				{
					if (ant_fd > -1)
					{
						dsdb_base_files[ant_fd].free_file  = dsdb_base_files[fd].free_file;
					}
					else
					{
						dsdb_base_free_file = dsdb_base_files[fd].free_file;
					}
					dsdb_base_files[fd].free_file = -1;
					dsdb_base_files[fd].bloquecorriente = 0L;
					dsdb_base_files[fd].de = (ds_off)0;
					dsdb_base_files[fd].hasta = (ds_off)0;		
					dsdb_base_files[fd].posicion = (ds_off)0;
					if (dsdb_bloqueo_abierto(&(dsdb_base_files[fd]),fd) < 0)
					{	
						DSDB_ERROR(213);
						GuardaErrores(1);
						dsdb_u_cierra(fd);
						errno = EACCES;
						return -1;
					}
					return ( (fd | 0x40000000) );
				}
				if (dsdb_base_files[fd].base_fd > -1)
				{
					dsdb_base_cierra(dsdb_base_files[fd].base_fd); 
					dsdb_base_files[fd].base_fd = -1;
				}		
			}
		}
	}

	info_rabre++;

	base_fd = dsdb_base_abre(base);
	if (base_fd < 0)
	{
		_doserrno = 0L;
		errno = EACCES;
		return -1;
	}
	
	if ((i = dsdb_localiza_tabla(base_fd,tabla,&direntry)) != 1) /* devolver la entrada del directorio */
	{		
		if (i < 0)
		{
			DSDB_ERROR(17);
			dsdb_base_cierra(base_fd);
			return -1;
		}
		if (!crear_tabla && !resetear_tabla && !dsdb_sin_errores)
		{
			/* ver si existe como fichero externo para "absorberlo" automaticamente */
			/* si existe ... crear_tabla = 1;  y pasarlo */
			j = 0;
			do
			{
				importacion = dsdb_sabre(filename,O_RDWR | O_BINARY,SH_DENYRW);
				if (importacion > -1)
				{
					if (j)
					{
						dsdb_cierra(importacion);
						importacion = -1;						
					}
					else
					{
						if (dsdb_bloq_escritura_directorio(base_fd,BasesAbiertas[base_fd].handle) < 0)
						{							
							DSDB_ERROR(181);							
							dsdb_cierra(importacion);
							dsdb_base_cierra(base_fd);							
							return -1;
						}						
						crear_tabla = 1;
					}
					break;
				}
				else
				{
					if (errno != EACCES)
					{
						break;
					}
					j = 1;
					dsdb_usleep(10000);
				}
			} while(-1);
			if (!crear_tabla && j)
			{
				crear_tabla = 1;
			}
		}
		if (crear_tabla)
		{
			/* los ficheros vacios solo tiene la entrada del directorio */
			if ((i = dsdb_crea_tabla(base_fd,tabla,&direntry,NULL)) < 0)
			{				
				DSDB_ERROR(18);
				if (importacion > -1)
				{
					dsdb_desbloq_escritura_directorio(base_fd,BasesAbiertas[base_fd].handle);
					dsdb_cierra(importacion);
				}
				dsdb_base_cierra(base_fd);
				return -1;
			}
			if (i != 1)
			{
				i = dsdb_localiza_tabla(base_fd,tabla,&direntry);
			}
			else
			{
				if (importacion > -1)
				{
					dsdb_desbloq_escritura_directorio(base_fd,BasesAbiertas[base_fd].handle);
					dsdb_cierra(importacion);
					importacion = -1;
				}
			}
			if (i != 1 || strcmp(tabla,direntry.name))
			{
				DSDB_ERROR(129);
				if (importacion > -1)
				{
					dsdb_desbloq_escritura_directorio(base_fd,BasesAbiertas[base_fd].handle);
					dsdb_cierra(importacion);
				}
				dsdb_base_cierra(base_fd);
				return -1;
			}
			creado = 1;
		}		
		else if (resetear_tabla)
		{
			/* si resetear_tabla .... cargarse el posible externo */
			unlink(filename);
		}
	}	

	if (i == 1 && strcmp(tabla,direntry.name))
	{
		DSDB_ERROR(130);
		if (importacion > -1)
		{
			dsdb_desbloq_escritura_directorio(base_fd,BasesAbiertas[base_fd].handle);
			dsdb_cierra(importacion);
			importacion = -1;
		}
		dsdb_base_cierra(base_fd);
		return -1;
	}

	if (i != 1)
	{
		if (importacion > -1)
		{
			dsdb_desbloq_escritura_directorio(base_fd,BasesAbiertas[base_fd].handle);
			dsdb_cierra(importacion);
		}
		dsdb_base_cierra(base_fd);
		errno = ENOENT;
		return -1;
	}

	/* asignar handle y estructura de trabajo */
	if (dsdb_base_free_file > -1 && (recursion || ndsdb_base_files > 40 || !dsdb_serverip))
	{
		int ant_fd;
		ant_fd = -1;
		fd = dsdb_base_free_file; /* coger el ultimo o el que este sin base abierta ... tiempo gastado necesario para optimizacion abre */
		
		while(dsdb_base_files[fd].base_fd > -1 && dsdb_base_files[fd].free_file > -1)
		{
			ant_fd = fd;
			fd = dsdb_base_files[fd].free_file;
		}
		
		if (fd == dsdb_base_free_file)
		{
			dsdb_base_free_file = dsdb_base_files[fd].free_file;
		}
		else
		{
			dsdb_base_files[ant_fd].free_file = dsdb_base_files[fd].free_file;
		}
		if (dsdb_base_files[fd].base_fd > -1)
		{
			dsdb_base_cierra(dsdb_base_files[fd].base_fd); /* si es la misma se mantiene abierta */
			dsdb_base_files[fd].base_fd = -1;
		}		
	}
	else
	{
		if (ndsdb_base_files >= MAX_OPEN_FILES)
		{
			if (importacion > -1)
			{
				dsdb_desbloq_escritura_directorio(base_fd,BasesAbiertas[base_fd].handle);
				dsdb_cierra(importacion);
			}
			dsdb_base_cierra(base_fd);
			errno = EMFILE;
			return -1;
		}
		else
		{
			fd = ndsdb_base_files++;			
			dsdb_base_files[fd].enigma = NULL;
			/* inicializar valores estructura por primera vez */
			dsdb_base_files[fd].maxlmapa = 50000;
			dsdb_base_files[fd].lmapa = (unsigned long *)malloc(dsdb_base_files[fd].maxlmapa*sizeof(long) + 1);
		}
	}


	/*
	{
		char tmp[256];
		sprintf(tmp,"fd=%d,n=%d,M=%d",fd,ndsdb_base_files,MAX_OPEN_FILES);
		aviso(0,tmp);
	}
	*/

	dsdb_base_files[fd].free_file = -1;
	dsdb_base_files[fd].flags = oflag;
	dsdb_base_files[fd].base_fd = base_fd;
	dsdb_base_files[fd].bloquecorriente = 0L;
	dsdb_base_files[fd].de = (ds_off)0;
	dsdb_base_files[fd].hasta = (ds_off)0;
	dsdb_base_files[fd].direntry = direntry;
	dsdb_base_files[fd].posicion = (ds_off)0;
	dsdb_base_files[fd].identry  = 0xffffffff;
	dsdb_base_files[fd].acceso = 0;
	memset(dsdb_base_files[fd].lmapa,0,dsdb_base_files[fd].maxlmapa*sizeof(long));

	
	dsdb_base_files[fd].nbloqueos = 0;
	
	if ((BasesAbiertas[base_fd].flags & DSDB_FLAG_CRYPTO))
	{		
		unsigned char localkey[128];
		int i;		
		if (!dsdb_base_files[fd].enigma)
		{
			dsdb_base_files[fd].enigma = (unsigned char *)malloc(313*8);
		}
		memset(localkey,0,sizeof(localkey));
		/*strcpy(localkey,direntry.name);*/
		for (i = 0;i < 16;i++)
		{
			localkey[i] ^= i;
		}		
		if ((BasesAbiertas[base_fd].flags & DSDB_FLAG_COPIA))
		{
			for (i = 0;i < 16;i++)
			{
				localkey[i] ^= keycopia[i];
			}
		}
		crea_tabla_codes(dsdb_base_files[fd].enigma,localkey);
	}

	/*aviso(0,"1");*/

	if ((oflag & O_EXCL))
	{
		if (!dsdb_base_files[fd].direntry.pinidata)
		{
			if (dsdb_u_graba(fd,"",0) < 0)
			{
				DSDB_ERROR(210);
				GuardaErrores(1);
				if (importacion > -1)
				{
					dsdb_desbloq_escritura_directorio(base_fd,BasesAbiertas[base_fd].handle);
					dsdb_cierra(importacion);
				}				
				dsdb_u_cierra(fd);
				errno = EACCES;
				return -1;
			}			
		}
		dsdb_desbloqueo_abierto(&(dsdb_base_files[fd]),fd);
		if (dsdb_bloqueo_exclusivo(base_fd,&(dsdb_base_files[fd].direntry),&(dsdb_base_files[fd].identry)) < 0)
		{			
			DSDB_ERROR(209);
			GuardaErrores(1);
			if (importacion > -1)
			{
				dsdb_desbloq_escritura_directorio(base_fd,BasesAbiertas[base_fd].handle);
				dsdb_cierra(importacion);
			}
			dsdb_u_cierra(fd);
			errno = EACCES;
			return -1;
		}
	}	
	else
	{
		/* comprobar si esta abierto exclusivo */
		if (dsdb_base_files[fd].direntry.pinidata)
		{
			if (dsdb_bloqueo_abierto(&(dsdb_base_files[fd]),fd) < 0)
			{	
				DSDB_ERROR(211);
				GuardaErrores(1);
				dsdb_u_cierra(fd);
				errno = EACCES;
				return -1;
			}			
		}
	}

	/*aviso(0,"2");*/

	if (i == 1 && resetear_tabla)
	{
		if (dsdb_base_vaciatabla(base_fd,&direntry,NULL,fd) < 0)
		{
			DSDB_ERROR(19);
			dsdb_u_cierra(fd);
			return -1;
		}
		dsdb_base_files[fd].direntry = direntry;
		creado = 1;
	}

	/* Quitar en caso de querer ficheros de 0 bytes "sin coste"-> ojo con los borrados y renombrados
	   concurrentes */
	if (creado)
	{
		dsdb_u_graba(fd,"",0);
	}

	/*aviso(0,"3");*/

	if (importacion > -1)
	{
		char *lbuffer;
		int mlen = 100000;
		int len;
		int ok = 1;
		long reg;
		lbuffer = malloc(mlen);
		
        if (ixxdat_aviso)
        {
			reg = (long)((ds_off)dsdb_lseek(importacion,0L,SEEK_END) / (ds_off)mlen) + 1;
			dsdb_lseek(importacion,0L,SEEK_SET);
            (*ixxdat_aviso)(0,reg,tabla);
			reg = 0;
        }		
		while((len = dsdb_lee(importacion,lbuffer,mlen)) > 0)
		{			
			if (dsdb_u_graba(fd,lbuffer,len) != len)
			{
				/* ERROR !!! */
				ok = 0;
				break;
			}			
            if (ixxdat_aviso)
            {
				reg++;
                (*ixxdat_aviso)(1,reg,tabla);
            }
		}
		dsdb_u_lseek(fd,(ds_off)0,SEEK_SET);
        if (ixxdat_aviso)
        {
            (*ixxdat_aviso)(2,0L,tabla);
        }             

		dsdb_cierra(importacion);
		importacion = -1;
		if (ok)
		{
			for (j = 0;j < 1000;j++)
			{
				if (unlink(filename) >= 0)
					break;
				dsdb_usleep(1000);
			}
		}
		free(lbuffer);		
		/* entrar en recursion para todos los ficheros .dat,.ixx,.ddx,.dbf que encontremos */
		{
			char **_dir = NULL;
			char **dir;
			int ifd,kk;
			char patron[30];
			extern char **lee_direct(char *pdir,int flags);
			extern int coincide_(char *buf,char *filtro);
			extern void libera_dir(char **dir_);			
			if (!recursion)
			{
				recursion++;
				strcpy(quita_path(base),"*");
				if ((_dir = lee_direct(base,0)))
				{
					dir = _dir;
					while(*dir)
					{		
						for (kk = 0;dsdb_extensiones[kk];kk++)
						{
							patron[0] = '*';
							strcpy(patron+1,dsdb_extensiones[kk]);
							if (coincide_(*dir,patron))
								break;
						}						
						if (dsdb_extensiones[kk])
						{
							strcpy(quita_path(base),*dir);
							ifd = dsdb_u_abre(base,O_RDWR | O_BINARY,SH_DENYNO);
							if (ifd > -1)
							{
								dsdb_u_cierra(ifd);
							}
						}
						dir++;
					}
					libera_dir(_dir);
				}
				recursion--;
			}			
		}
		dsdb_desbloq_escritura_directorio(base_fd,BasesAbiertas[base_fd].handle);
	}

	/*aviso(0,"4");*/
	/*
	test = GetTickCount() - test;
	if (test > 100)
	{
		test = 0;
	}
	*/

	return ( (fd | 0x40000000) );
}

int dsdb_u_cierra(int handle)
{	
	int i,n;
	int fd;
	info_cierra++;

	if ((handle = dsdb_comprueba_handle(handle)) < 0)
	{
		errno = EBADF;
		return -1;
	}	
	
	
	fd = BasesAbiertas[dsdb_base_files[handle].base_fd].handle;
	n = dsdb_base_files[handle].nbloqueos;
	for (i = 0;i < n;i++)
	{
		dsdb_lseek(fd,dsdb_base_files[handle].bloqueo[i],SEEK_SET);
		dsdb_locking(fd,0,dsdb_base_files[handle].lenbloqueo[i]);
	}
	dsdb_base_files[handle].nbloqueos = 0;	
	
	
	if ((dsdb_base_files[handle].flags & O_EXCL))
	{
		dsdb_desbloqueo_exclusivo(dsdb_base_files[handle].base_fd,&(dsdb_base_files[handle].identry));
	}
	else
	{
        dsdb_desbloqueo_abierto(&(dsdb_base_files[handle]),handle);
	}
	/* optimizacion abre */
	if ((dsdb_base_files[handle].flags & (O_CREAT | O_TRUNC | O_EXCL)))
	{
		dsdb_base_cierra(dsdb_base_files[handle].base_fd);
		dsdb_base_files[handle].base_fd = -1;
	}	
	dsdb_base_files[handle].free_file = dsdb_base_free_file;
	dsdb_base_free_file = handle;

	return 0;
}


static ds_off dsdb_u_tamtabla(int handle)
{		
	DBDirEntry *pdirentry;	
	ds_off pos,size;
	int base_fd;
	int fd;

	/*
	if (dsdb_base_files[handle].acceso)
	{
		return dsdb_base_files[handle].size;
	}
	*/
	info_tamtabla++;

	pdirentry = &(dsdb_base_files[handle].direntry);
	base_fd = dsdb_base_files[handle].base_fd;
	fd = BasesAbiertas[base_fd].handle;
	if (!pdirentry->pinidata)
	{
		/* verificar que sigue vacio */
		if (dsdb_localiza_tabla(dsdb_base_files[handle].base_fd,NULL,pdirentry) < 0)
		{
			errno = EBADF;
			return -1;
		}
		if (!pdirentry->pinidata)
		{
			return (ds_off)0;
		}
	}

	pos = (ds_off)(pdirentry->pinidata-1) * (ds_off)BasesAbiertas[base_fd].header.blocksize;
	pos += OFFSET_INIDATASIZE;


	if (dsdb_lseek(fd,pos,SEEK_SET) == (ds_off)-1)
	{
		DSDB_ERROR(92);
		return (ds_off)0;
	}
	if (dsdb_lee(fd,(void *)&size,sizeof(size)) != sizeof(size))
	{
		DSDB_ERROR(92);
		return (ds_off)0;
	}

	dsdb_base_files[handle].size = size;	

	/* dsdb_base_files[handle].acceso = 1; /* usar un control dependiente de la cache */

	return size;
}

ds_off dsdb_u_lseek(int handle,ds_off offset,int origin)
{	
	info_lseek++;

	if ((handle = dsdb_comprueba_handle(handle)) < 0)
	{
		errno = EBADF;
		return (ds_off)-1;
	}
	switch(origin)
	{
	case SEEK_SET:
		dsdb_base_files[handle].posicion = offset;
		break;
	case SEEK_CUR:
		dsdb_base_files[handle].posicion += offset;
		break;
	case SEEK_END:
		dsdb_base_files[handle].posicion = dsdb_u_tamtabla(handle) + offset;
		break;
	}	
	return dsdb_base_files[handle].posicion;
}

int dsdb_u_graba(int handle, void *_buffer, unsigned int count)
{
	char *buffer = (char *)_buffer; /* fedora again */
	DBFile *pfile;
	int r;
	int dbhandle;
	ulong blocksize;
	int grabados = 0;		
	ds_off inisize;
	ds_off tmppos,real,hasta;
	int base_fd;
	int falta;	
	DBMapEntry mapentry;		
	DBIniData inidata;	
	DBMoreData more;
	ulong viejopmapa;
	DBArbolBusca busca;
	int len,ilen;
	int conampliacion = 1;
	long x;
	int uno,dos;
	ds_off xde,xhasta;
	int xok = 0;

	info_graba++;

	if ((handle = dsdb_comprueba_handle(handle)) < 0)
	{
		errno = EBADF;
		return -1;
	}
	pfile = &(dsdb_base_files[handle]);
	base_fd = pfile->base_fd;
	dbhandle  = BasesAbiertas[base_fd].handle;
	blocksize = BasesAbiertas[base_fd].header.blocksize;	
	if (!pfile->direntry.pinidata)
	{   /* grabamos bloque inicial del fichero */
		ulong pdirectory;
		DBDirEntry *direntry,de;
		DBHeader *pheader;
		
		conampliacion = 2;

		direntry = &de;

		if (dsdb_bloq_escritura_directorio(base_fd,dbhandle) < 0)
		{
			DSDB_ERROR(115);
			return -1;
		}

		pheader = &(BasesAbiertas[base_fd].header);
		if (dsdb_refresca_cabecera(dbhandle,&(BasesAbiertas[base_fd].nupdate),pheader) < 0)
		{
			dsdb_desbloq_escritura_directorio(base_fd,dbhandle);
			DSDB_ERROR(116);
			return -1;
		}			

		pdirectory = pheader->pdirectory;
		if (!pdirectory)
		{		
			dsdb_desbloq_escritura_directorio(base_fd,dbhandle);
			DSDB_ERROR(117);
			return -1;
		}
		{			
			busca.handle = dbhandle;
			busca.id = IDBDDB;
			busca.compara = (void *)pfile->direntry.name;
			busca.sizex = sizeof(direntry->name);
			busca.size0 = sizeof(DBDirEntry);
			busca.blocksize = blocksize;
			busca.resultado = (void *)direntry;
			busca.r = -1;
			busca.rbloque = 0;
			busca.roffset = 0;
			busca.pfile = NULL;
			r = dsdb_busca_en_arbol(&busca,pdirectory,(ulong)(long)-1,0);
			if (r != 1)
			{
				DSDB_ERROR(118);
				dsdb_desbloq_escritura_directorio(base_fd,dbhandle);			
				return -1;
			}						
			if (!direntry->pinidata)
			{			

				dsdb_tocando_dir(dbhandle);

				memset((void *)&inidata,0,sizeof(inidata));
				inidata.id = IDBIDB;
				inidata.reservado = 0;
				strcpy(inidata.name,pfile->direntry.name);				

				inidata.identry =    ++(BasesAbiertas[base_fd].header.newfileid);
				if (inidata.identry >= DSBDBMAXTABLES)
				{
					BasesAbiertas[base_fd].header.newfileid = 1;
					inidata.identry =   BasesAbiertas[base_fd].header.newfileid;
				}

				inidata.pmapa   =    dsdb_nuevo_bloque(base_fd);
				inidata.ssize   =    sizeof(inidata);
				direntry->pinidata = dsdb_nuevo_bloque(base_fd);
				inidata.ctime   = time(NULL);
				inidata.mtime   = inidata.ctime;
				inidata.atime   = inidata.ctime;
				((DBArbol *)bloque_tmp1)->id = IDBMPB;
				((DBArbol *)bloque_tmp1)->level  = 0;
				((DBArbol *)bloque_tmp1)->nitems = 1;
				mapentry.pos = (ds_off)0;
				mapentry.pbloque = direntry->pinidata;
				memcpy(bloque_tmp1+sizeof(DBArbol),(void *)&mapentry,sizeof(mapentry));
				memset(bloque_tmp1+sizeof(DBArbol)+sizeof(mapentry),0,blocksize-(sizeof(DBArbol)+sizeof(mapentry)));
				pfile->pmapa = inidata.pmapa;
			    
				if (dsdb_graba_bloque(dbhandle,inidata.pmapa,(void *)bloque_tmp1,blocksize,blocksize,0,0) < 0)
				{
					DSDB_ERROR(122);
					dsdb_desbloq_escritura_directorio(base_fd,dbhandle);
					return -1;
				}
				if (dsdb_graba_bloque(dbhandle,direntry->pinidata,(void *)&inidata,sizeof(inidata),blocksize,0,0) < 0)
				{
					DSDB_ERROR(121);
					dsdb_desbloq_escritura_directorio(base_fd,dbhandle);
					return -1;
				}
				real = (ds_off)((ds_off)(busca.rbloque-1)*(ds_off)blocksize) + (sizeof(DBArbol)+busca.roffset);
				if (dsdb_lseek(dbhandle,real,SEEK_SET) == (ds_off)-1)
				{
					DSDB_ERROR(120);
					dsdb_desbloq_escritura_directorio(base_fd,dbhandle);
					return -1;
				}
				if (dsdb_graba(dbhandle,(void *)direntry,sizeof(DBDirEntry)) != sizeof(DBDirEntry))
				{
					DSDB_ERROR(119);
					dsdb_desbloq_escritura_directorio(base_fd,dbhandle);
					return -1;
				}

				if (dsdb_lseek(dbhandle,OFFSET_PFILEID,SEEK_SET) == (ds_off)-1)
				{
					DSDB_ERROR(141);
					dsdb_desbloq_escritura_directorio(base_fd,dbhandle);
					return -1;
				}

				if (dsdb_graba(dbhandle,(void *)&(BasesAbiertas[base_fd].header.newfileid),sizeof(BasesAbiertas[base_fd].header.newfileid)) != sizeof(BasesAbiertas[base_fd].header.newfileid))
				{
					DSDB_ERROR(155);
					dsdb_desbloq_escritura_directorio(base_fd,dbhandle);
					return -1;
				}

				pfile->direntry = *direntry;
				if (dsdb_bloqueo_abierto(pfile,handle) < 0)
				{
					/* esto es raro ... */
					DSDB_ERROR(199);
					dsdb_desbloq_escritura_directorio(base_fd,dbhandle);
					return -1;
				}

				dsdb_fin_tocando_dir(dbhandle);
			}
			pfile->direntry = *direntry;
			pfile->bloquecorriente = 0L;
		}        

		if (dsdb_desbloq_escritura_directorio(base_fd,dbhandle) < 0)
		{
			DSDB_ERROR(156);
			return -1;
		}
	}

	if (count && (BasesAbiertas[base_fd].flags & DSDB_FLAG_CRYPTO))
	{		
		if (count > ncrypto_buffer)
		{
			if (crypto_buffer)
				crypto_buffer = realloc((void *)crypto_buffer,count);
			else
				crypto_buffer = malloc(count);
			ncrypto_buffer = count;
		}
		if (!crypto_buffer)
		{
			char tmp[512];
			sprintf(tmp,"fallo malloc crypto_buffer, %d %d",count,ncrypto_buffer);
			aviso(0,tmp);
		}		
		encrypta(pfile->enigma,pfile->posicion,buffer,crypto_buffer,count);
		buffer = crypto_buffer;
	}
	
	uno = blocksize-sizeof(DBIniData);
	dos = blocksize-sizeof(DBMoreData);
	if (pfile->posicion < uno)
	{
		x = 0;
		xde = 0;
		xhasta = uno-1;
	}
	else
	{
		x = (long)(ds_off)((ds_off)(pfile->posicion-(ds_off)uno)/(ds_off)dos);
		xde = (ds_off)uno + (ds_off)x*(ds_off)dos;
		xhasta = xde + (dos - 1);
		x++;
	}
	if (dsdb_serverip && x < pfile->maxlmapa && pfile->lmapa[x])
	{
		pfile->bloquecorriente = pfile->lmapa[x];				
		pfile->de = xde;
		pfile->hasta = xhasta;				
		xok = 1;
	}

	if (pfile->bloquecorriente)
	{
		if (pfile->de <= pfile->posicion)
		{
			if (pfile->hasta >= (pfile->posicion+(ds_off)count))
			{
				if (pfile->size >= (pfile->posicion+(ds_off)count))
				{
					/* optimizar rapido .... */
					real = (ds_off)(pfile->bloquecorriente-1)*(ds_off)blocksize + (pfile->posicion - pfile->de);
					if (pfile->direntry.pinidata == pfile->bloquecorriente)
					{
						real += sizeof(DBIniData);
					}
					else
					{
						real += sizeof(DBMoreData);
					}
					if (dsdb_lseek(dbhandle,real,SEEK_SET) == (ds_off)-1)
					{
						DSDB_ERROR(123);
						return -1;
					}
					grabados = dsdb_graba(dbhandle,buffer,count);
					if (grabados > 0)
					{
						pfile->posicion += grabados;
						if (pfile->posicion > pfile->size)
						{							
							DSDB_ERROR(180);
							return -1;
						}
					}
					return grabados;
				}
			}
		}
	}

	if (conampliacion == 1 && dsdb_serverip && (pfile->posicion+count) < pfile->size)
	{
		conampliacion = 0;
	}

	if (conampliacion)
	{
		if (dsdb_bloq_escritura_mapa(pfile) < 0)
		{
			DSDB_ERROR(144);
			return -1;
		}
		if ((r = dsdb_confirma_corriente(pfile,1,&busca)) < 0) 
		{
			dsdb_desbloq_escritura_mapa(pfile);
			DSDB_ERROR(102);
			return -1;
		}
		if (r < 0 || pfile->posicion < pfile->de)
		{
			dsdb_desbloq_escritura_mapa(pfile);
			DSDB_ERROR(135);
			return -1;				
		}
	}
	else
	{
		if (xok)
			r = 1;
		else
		{
			if ((r = dsdb_confirma_corriente(pfile,0,NULL)) < 0)
			{
				DSDB_ERROR(102);
				return -1;
			}
		}
	}

	viejopmapa = pfile->pmapa;
	inisize = pfile->size;
	while(count)
	{
		if (pfile->posicion < pfile->de)
		{
			if (conampliacion)
				dsdb_desbloq_escritura_mapa(pfile);
			DSDB_ERROR(134);
			return -1;				
		}
		if (pfile->posicion > (pfile->hasta+(ds_off)1))
		{
			len = -1; /* esto provoca la ampliacion */
		}
		else
		{
			if (pfile->hasta >= pfile->size)
			{
				hasta = pfile->size - 1;
			}
			else
			{
				hasta = pfile->hasta;
			}
			if (pfile->posicion > (hasta+(ds_off)1))
			{
				len = 0;
			}
			else
			{
				len = (int)(hasta - pfile->posicion) + 1;		
				if (len > (int)count)
					len = count;
				else if (len < 0)
				{
					if (conampliacion)
						dsdb_desbloq_escritura_mapa(pfile);
					DSDB_ERROR(105);
					return -1;				
				}			
			}
		}

		if (len > 0)
		{
			real = (ds_off)(pfile->bloquecorriente-1)*(ds_off)blocksize + (pfile->posicion - pfile->de);
			if (pfile->direntry.pinidata == pfile->bloquecorriente)
			{
				real += sizeof(DBIniData);
			}
			else
			{
				real += sizeof(DBMoreData);
			}

			if (dsdb_lseek(dbhandle,real,SEEK_SET) == (ds_off)-1)
			{
				if (conampliacion)
					dsdb_desbloq_escritura_mapa(pfile);
				DSDB_ERROR(103);
				return -1;
			}

			if ((ilen = dsdb_graba(dbhandle,buffer,len)) != (int)len)
			{
				if (conampliacion)
					dsdb_desbloq_escritura_mapa(pfile);
				/*if (ilen < 0)*/
				{ /* Se descuadraria lo grabado en el bloque del fichero, de todos modos no puede pasar */
					DSDB_ERROR(104);
					return -1;
				}
				/*return grabados;*/
			}	

			grabados += len;
			/*(char *)buffer += len;*/
			buffer += len;
			count -= len;
			pfile->posicion += len;
			if (pfile->posicion > pfile->size)
			{
				if (conampliacion)
					dsdb_desbloq_escritura_mapa(pfile);
				DSDB_ERROR(179);
				return -1;
			}
		}

		if (count)
		{
			if (!conampliacion)
			{
				if (pfile->size > (pfile->hasta+dos))
				{									
					x++;
					if (dsdb_serverip && x < pfile->maxlmapa && pfile->lmapa[x])
					{
						xde = xhasta + 1;
						xhasta = xde + (dos - 1);
						pfile->bloquecorriente = pfile->lmapa[x];				
						pfile->de = xde;
						pfile->hasta = xhasta;
						continue;
					}
					x--;
				}
			}

			real = (ds_off)(pfile->bloquecorriente-1)*(ds_off)blocksize;
			if (dsdb_lseek(dbhandle,real,SEEK_SET) == (ds_off)-1)
			{
				if (conampliacion)
					dsdb_desbloq_escritura_mapa(pfile);
				DSDB_ERROR(106);
				return -1;
			}
			if (dsdb_lee(dbhandle,&more,sizeof(more)) != sizeof(more))
			{
				if (conampliacion)
					dsdb_desbloq_escritura_mapa(pfile);
				DSDB_ERROR(107);
				return -1;
			}
			if (more.id == IDBIDB)
			{
				if (pfile->size < more.grabadoyorder)
				{
					pfile->size = more.grabadoyorder;
				}
				if (pfile->size <= pfile->hasta)
				{
					pfile->size = pfile->hasta+1;
					if (pfile->size > (pfile->posicion+count))
					{
						pfile->size = pfile->posicion+count;
					}
					if (len >= 0)
					{
						continue;
					}
				}
			}
			else if (more.id == IDBSDB)
			{
				falta = (blocksize-sizeof(DBMoreData)) - DSLODWORD(more.grabadoyorder);
				if (pfile->hasta >= pfile->size)
				{
					if (falta != ((pfile->hasta-pfile->size)+1))
					{
						falta = (blocksize-sizeof(DBMoreData)) - ((int)(ds_off)(pfile->hasta-pfile->size)+1);
						more.grabadoyorder = DSMAKEOFF(falta,DSHIDWORD(more.grabadoyorder));
						falta = (blocksize-sizeof(DBMoreData)) - DSLODWORD(more.grabadoyorder);
					}
				}
				if ( falta > 0 )
				{					
					if (len < 0)
					{
						falta = (blocksize-sizeof(DBMoreData));
					}
					else
					{
						if (pfile->posicion > pfile->size)
						{
							falta = (int)((ds_off)(pfile->posicion - pfile->size)) + count + DSLODWORD(more.grabadoyorder);
						}
						else
						{
							falta = count + DSLODWORD(more.grabadoyorder);
						}
						if (falta > (int)(blocksize-sizeof(DBMoreData)))
						{
							falta = (blocksize-sizeof(DBMoreData));
						}						
					}
					pfile->size += (ds_off)(falta - DSLODWORD(more.grabadoyorder));
					if (pfile->size > (pfile->posicion+count))
					{
						falta -= (int)(ds_off)((pfile->posicion+count) - pfile->size);
						pfile->size = pfile->posicion+count;						
					}
					more.grabadoyorder = DSMAKEOFF(falta,DSHIDWORD(more.grabadoyorder));

					if (dsdb_lseek(dbhandle,real,SEEK_SET) == (ds_off)-1)
					{
						if (conampliacion)
							dsdb_desbloq_escritura_mapa(pfile);
						DSDB_ERROR(109);
						return -1;
					}
					if (dsdb_graba(dbhandle,&more,sizeof(more)) != sizeof(more))
					{
						if (conampliacion)
							dsdb_desbloq_escritura_mapa(pfile);
						DSDB_ERROR(110);
						return -1;
					}
					if (len >= 0)
					{
						continue;
					}
				}
			}
			else
			{
				if (conampliacion)
					dsdb_desbloq_escritura_mapa(pfile);
				DSDB_ERROR(108);
				return -1;
			}			

			if (!more.pnext)
			{
				/* necesitamos un nuevo bloque ! */
				if (!conampliacion)
				{
					DSDB_ERROR(215);
					GuardaErrores(1);

					/* esto no deberia pasar!! */
					conampliacion = 1;
					if (dsdb_bloq_escritura_mapa(pfile) < 0)
					{
						DSDB_ERROR(144);
						return -1;
					}
					if ((r = dsdb_confirma_corriente(pfile,1,&busca)) < 0) 
					{
						dsdb_desbloq_escritura_mapa(pfile);
						DSDB_ERROR(102);
						return -1;
					}
					if (r < 0 || pfile->posicion < pfile->de)
					{
						dsdb_desbloq_escritura_mapa(pfile);
						DSDB_ERROR(135);
						return -1;				
					}
				}

				/*

				if (!conampliacion)
				{
					aviso(0,"ERROR_A");
				}
				*/

				if (len < 0)
				{
					if (pfile->posicion > (pfile->hasta+(blocksize-sizeof(DBMoreData))))
					{
						falta = (blocksize-sizeof(DBMoreData));
					}
					else
					{
						falta = (int)(ds_off)((ds_off)(pfile->hasta+(blocksize-sizeof(DBMoreData))) - pfile->posicion) + count;
						if (falta > (int)(blocksize-sizeof(DBMoreData)))
						{
							falta = (blocksize-sizeof(DBMoreData));
						}
					}
				}
				else if (count > (blocksize-sizeof(DBMoreData)))
				{
					falta = (blocksize-sizeof(DBMoreData));					
				}
				else
				{
					falta = count;
				}

				pfile->size += falta;

				more.pnext = dsdb_nuevo_bloque(base_fd);
				if (more.id == IDBIDB)
				{
					more.grabadoyorder = pfile->size;
				}

				if (dsdb_lseek(dbhandle,real,SEEK_SET) == (ds_off)-1)
				{
					if (conampliacion)
						dsdb_desbloq_escritura_mapa(pfile);
					DSDB_ERROR(111);
					return -1;
				}
				if (dsdb_graba(dbhandle,&more,sizeof(more)) != sizeof(more))
				{
					if (conampliacion)
						dsdb_desbloq_escritura_mapa(pfile);
					DSDB_ERROR(112);
					return -1;
				}

				if (more.id == IDBIDB)
				{
					more.id = IDBSDB;
					more.grabadoyorder = DSMAKEOFF(falta,1);
				}
				else
				{				
					if (more.id != IDBSDB)
					{
						if (conampliacion)
							dsdb_desbloq_escritura_mapa(pfile);
						DSDB_ERROR(133);
						return -1;
					}
					more.grabadoyorder = DSMAKEOFF(falta,DSHIDWORD(more.grabadoyorder)+1); /* importante el incremento */
				}
				pfile->bloquecorriente = more.pnext;
				more.reservado = 0;
				more.pnext = 0;
				if (dsdb_graba_bloque(dbhandle,pfile->bloquecorriente,(void *)&more,sizeof(more),blocksize,0,0) < 0)
				{
					DSDB_ERROR(113);
					return -1;
				}

				tmppos = pfile->hasta+1;
				busca.compara = (void *)&tmppos;
				busca.resultado = (void *)&mapentry;
				busca.r = -1;
				busca.rbloque = 0;
				busca.roffset = 0;				
				busca.padres[0] = 0;
				busca.pfile = pfile;
				r = dsdb_busca_en_arbol(&busca,pfile->pmapa,(ulong)(long)-1,0);
				if (r != 2)
				{
					if (conampliacion)
						dsdb_desbloq_escritura_mapa(pfile);
					DSDB_ERROR(126);
					return -1;
				}
				/* busca.r = 2; /* en este caso apunta al ultimo siempre, y siempre añadimos */
				mapentry.pos = pfile->hasta+1;
				mapentry.pbloque = pfile->bloquecorriente;
				busca.resultado = (void *)&mapentry;
				if (dsdb_inserta_en_arbol(&busca,base_fd,0,bloque_tmp3,&(pfile->pmapa)) < 0)
				{
					if (conampliacion)
						dsdb_desbloq_escritura_mapa(pfile);
					DSDB_ERROR(114);
					return -1;
				}
			}
			else
			{
				pfile->bloquecorriente = more.pnext;
			}
			if (len >= 0)
			{
				if (pfile->posicion != (pfile->hasta+1))
				{
					if (conampliacion)
						dsdb_desbloq_escritura_mapa(pfile);
					DSDB_ERROR(139);
					return -1;
				}
			}
			pfile->de = pfile->hasta+1;
			pfile->hasta = pfile->de + ((blocksize-sizeof(DBMoreData)) - 1);

			
			if (pfile->de < uno)
			{
				x = 0;
				xde = 0;
				xhasta = uno-1;
			}
			else
			{
				x = (long)(ds_off)((ds_off)(pfile->de-(ds_off)uno)/(ds_off)dos) + 1;
				xde = pfile->de;
				xhasta = pfile->hasta;				
			}
			if (x < pfile->maxlmapa)
			{
				pfile->lmapa[x] = pfile->bloquecorriente;
			}
			
		}
	}
	
	if (inisize != pfile->size || viejopmapa != pfile->pmapa) /* actulizar size */
	{
		if (!conampliacion)
		{
			aviso(0,"ERROR_B");
		}
		if (dsdb_lee_bloque(dbhandle,pfile->direntry.pinidata,(void *)bloque_tmp1,blocksize,sizeof(DBIniData),0,0) < 0)
		{
			if (conampliacion)
				dsdb_desbloq_escritura_mapa(pfile);
			DSDB_ERROR(124);
			return -1;
		}
		((DBIniData *)bloque_tmp1)->size = pfile->size;
		((DBIniData *)bloque_tmp1)->pmapa = pfile->pmapa;

		if (dsdb_graba_bloque(dbhandle,pfile->direntry.pinidata,(void *)bloque_tmp1,sizeof(DBIniData),blocksize,OFFSET_PADDING,sizeof(ulong)) < 0)
		{
			if (conampliacion)
				dsdb_desbloq_escritura_mapa(pfile);
			DSDB_ERROR(125);
			return -1;
		}
	}
	

	if (conampliacion)
	{
		dsdb_fin_tocando_mapa(pfile);
		if (dsdb_desbloq_escritura_mapa(pfile) < 0)
		{
			DSDB_ERROR(145);
			return -1;
		}	
	}

	return grabados;
}

int dsdb_u_lee(int handle, void *_buffer, unsigned int count)
{	
	char *buffer = (char *)_buffer; /* fedora again */
	DBFile *pfile;
	int r;
	int leidos = 0;
	ds_off real,hasta;
	int dbhandle;
	int len,ilen;
	ulong blocksize;
	DBMoreData more;
	int encriptado = 0;
	ds_off c_posicion = 0;
	unsigned char *c_buff;
	long x;
	int uno,dos;
	ds_off xde,xhasta;
	

	info_lee++;

	if ((handle = dsdb_comprueba_handle(handle)) < 0)
	{
		errno = EBADF;
		return -1;
	}
	pfile = &(dsdb_base_files[handle]);
	if (!pfile->direntry.pinidata)
	{
		/* verificar que sigue vacio */
		if (dsdb_localiza_tabla(pfile->base_fd,NULL,&(pfile->direntry)) < 0)
		{
			errno = EBADF;
			return -1;
		}
		if (!pfile->direntry.pinidata)
		{
			return leidos;
		}
	}		

	blocksize =  BasesAbiertas[pfile->base_fd].header.blocksize;
	uno = blocksize-sizeof(DBIniData);
	dos = blocksize-sizeof(DBMoreData);
	if (pfile->posicion < uno)
	{
		x = 0;
		xde = 0;
		xhasta = uno-1;
	}
	else
	{
		x = (long)(ds_off)((ds_off)(pfile->posicion-(ds_off)uno)/(ds_off)dos);
		xde = (ds_off)uno + (ds_off)x*(ds_off)dos;
		xhasta = xde + (dos - 1);
		x++;
	}
	if (dsdb_serverip && x < pfile->maxlmapa && pfile->lmapa[x])
	{
		pfile->bloquecorriente = pfile->lmapa[x];				
		pfile->de = xde;
		pfile->hasta = xhasta;		
		r = 1;
	}
	else if ((r = dsdb_confirma_corriente(pfile,0,NULL)) < 0)
	{
		DSDB_ERROR(94);
		return -1;
	}
	if (r == 3)
	{   /* por encima del fichero */
		return leidos;
	}

	if ((BasesAbiertas[pfile->base_fd].flags & DSDB_FLAG_CRYPTO))
	{		
		c_posicion = pfile->posicion;		
		encriptado = 1;
		c_buff = (unsigned char *)buffer;
	}
	
	dbhandle  = BasesAbiertas[pfile->base_fd].handle;	
	while(count)
	{
		if (pfile->size <= pfile->hasta)
			hasta = pfile->size - 1;
		else
			hasta = pfile->hasta;
		len = (int)(hasta - pfile->posicion) + 1;
		if (len > (int)count)
			len = count;
		else if (len < 0)
			len = 0; /* se asegurara que el final de fichero es correcto */

		if (len > 0)
		{
			real = (ds_off)(pfile->bloquecorriente-1)*(ds_off)blocksize + (pfile->posicion - pfile->de);
			if (pfile->direntry.pinidata == pfile->bloquecorriente)
			{
				real += sizeof(DBIniData);
			}
			else
			{
				real += sizeof(DBMoreData);
			}

			if (dsdb_lseek(dbhandle,real,SEEK_SET) == (ds_off)-1)
			{
				DSDB_ERROR(98);
				return -1;
			}

			if ((ilen = dsdb_lee(dbhandle,buffer,len)) != (int)len)
			{
				if (errno != EACCES && errno != EAGAIN && errno != 13 && errno != 35)
				{
					DSDB_ERROR(99);
				}
				if (ilen >= 0)
				{
					leidos += ilen;
					/*(char *)buffer += ilen;*/
					buffer += ilen;
					count -= ilen;
					pfile->posicion += ilen;
					if (encriptado)
					{
						desencrypta(pfile->enigma,c_posicion,c_buff,leidos);
					}
					return leidos;
				}
				return -1;
			}	

			leidos += len;
			/*(char *)buffer += len;*/
			buffer += len;
			count -= len;
			pfile->posicion += len;
		}

		if (count)
		{			
			x++;
			if (dsdb_serverip && x < pfile->maxlmapa && pfile->lmapa[x])
			{
				xde = xhasta + 1;
				xhasta = xde + (dos - 1);
				pfile->bloquecorriente = pfile->lmapa[x];				
				pfile->de = xde;
				pfile->hasta = xhasta;
				continue;
			}
			x--;

			real = (ds_off)(pfile->bloquecorriente-1)*(ds_off)blocksize;
			if (dsdb_lseek(dbhandle,real,SEEK_SET) == (ds_off)-1)
			{
				DSDB_ERROR(100);
				return -1;
			}
			if (dsdb_lee(dbhandle,&more,sizeof(more)) != sizeof(more))
			{
				DSDB_ERROR(101);
				return -1;
			}
			if (more.id == IDBIDB)
			{
				if (pfile->size < more.grabadoyorder)
				{
					if (pfile->hasta >= pfile->size)
					{
						pfile->size = more.grabadoyorder;
						continue;
					}
					pfile->size = more.grabadoyorder;
				}				
			}
			else if (more.id == IDBSDB)
			{				
				if ((int)(ds_off)(pfile->size - pfile->de) < (int)DSLODWORD(more.grabadoyorder))
				{
					pfile->size += (ds_off)((ds_off)DSLODWORD(more.grabadoyorder) - (ds_off)(pfile->size - pfile->de));
					continue;
				}
			}
			else
			{
				DSDB_ERROR(97);
				return -1;
			}
			if (!more.pnext)
				break;
			pfile->bloquecorriente = more.pnext;
			if (pfile->posicion != (pfile->hasta+1))
			{
				DSDB_ERROR(136);
				return -1;				
			}
			pfile->de = pfile->posicion;
			pfile->hasta = pfile->de + ((blocksize-sizeof(DBMoreData)) - 1);
			x++;
			xde = xhasta + 1;
			xhasta = xde + (dos - 1);
			if (x < pfile->maxlmapa)
			{
				pfile->lmapa[x] = pfile->bloquecorriente;
			}
		}
	}

	if (encriptado)
	{
		desencrypta(pfile->enigma,c_posicion,c_buff,leidos);
	}
	return leidos;
}

static int dsdb_u_locking(int handle, int mode, long nbytes)
{
	ds_off posicion;
	ds_off real;
	DBFile *pfile;
	int dbhandle;
	int r;
	ulong blocksize;

	info_locking++;

	if ((handle = dsdb_comprueba_handle(handle)) < 0)
	{
		errno = EBADF;
		return -1;
	}
	pfile = &(dsdb_base_files[handle]);
	dbhandle  = BasesAbiertas[pfile->base_fd].handle;
	blocksize = BasesAbiertas[pfile->base_fd].header.blocksize;	
	if (!pfile->direntry.pinidata)
	{
		dsdb_u_graba(handle,"",0); /* necesitamos el bloque inicial */		
		if (!dsdb_base_files[handle].direntry.pinidata)
		{
			errno = EBADF;
			return -1;
		}
	}
	posicion = pfile->posicion;
	if (posicion >= pfile->size && posicion < DBINIVIRLOCK)
	{
		DBIniData inidata;
		dsdb_lee_inicial(pfile,&inidata);
	}
	if (posicion < pfile->size || posicion < (blocksize-sizeof(DBIniData)))
	{
		if (nbytes > 1)
		{
			DSDB_ERROR(162);
			return -1;
		}
		/* Real Locking */
		/* OJO SOLO SIRVE PARA BLOQUEAR UN BYTE :., SEGUN LO QUE NECESITA EL 
		  INDEXADO, YA QUE NO SEGUIMOS (DE MOMENTO) EL FICHERO COMO EN EL LEE */
		if ((r = dsdb_confirma_corriente(pfile,0,NULL)) < 0)
		{
			DSDB_ERROR(161);
			return -1;
		}
		real = (ds_off)(pfile->bloquecorriente-1)*(ds_off)blocksize + (pfile->posicion - pfile->de);
		if (pfile->direntry.pinidata == pfile->bloquecorriente)
		{
			real += sizeof(DBIniData);
		}
		else
		{
			real += sizeof(DBMoreData);
		}
		if (dsdb_lseek(dbhandle,real,SEEK_SET) == (ds_off)-1)
		{
			DSDB_ERROR(163);
			return -1;
		}

		r = dsdb_locking(dbhandle,mode,1L);
		
		
		if (r >= 0)
		{
			dsdb_suma_bloqueo(pfile,real,1,mode);
		}
		
		
		return r;
	}
	else
	{
		if (posicion < DBINIVIRLOCK || (posicion+nbytes) > (DBINIVIRLOCK+LENIXXVLOCK))
		{
			DSDB_ERROR(164);
			return -1;
		}
		/* Virtual Locking */
		posicion -= DBINIVIRLOCK;		
		r = dsdb_bloqueo_virtual(pfile,posicion,mode,nbytes);
		return r;
	}
	return -1;
}

/* REPARADOR */

static void SalRecons(int base_fd)
{
	int file_fd = BasesAbiertas[base_fd].handle;

	dsdb_lseek(file_fd,DBPOS_LOCK,SEEK_SET);
	dsdb_locking64(file_fd,LK_UNLCK,LENALLLOCKS);
	dsdb_lseek(file_fd,(ds_off)DBLOCK_FLAG_REC,SEEK_SET);
	dsdb_locking(file_fd,LK_UNLCK,1);
	dsdb_cierra(file_fd);

	BasesAbiertas[base_fd].handle = -1;
	BasesAbiertas[base_fd].referencia = 0;	
	BasesAbiertas[base_fd].refbloqueoescritura = 0;

    if (ixxdat_aviso)
    {
        (*ixxdat_aviso)(2,0L,"");
    }             

}

int DSDB_ReconstruyeLimpiamente(char *base,int modo)
{ /* sobre todo ... PRUDENCIA!! */
	int file_fd;
	DBFullHeader header;
	ulong blocksize;
	ds_off tamfile,pos,real,size,hasta,tmppos;
	ulong nblo,bloa,antini;
	int i;
	ulong blkid;
	ulong totalextras;
	ulong extras;
	DBLibre libre;
	DBDirEntry direntry,d2;
	DBIniData inidata;
	DBMoreData more,morea;
	DBMapEntry mapentry;
	DBArbolBusca busca;
	char tabla[MAXNAMEDB];
	int base_fd = -1;
	ulong secuencia;
	int r;
	long cuenta = 0L;
	long tiempo = time(NULL);


	/* anular todos los mapas */
	for (i = 0;i < ndsdb_base_files;i++)
	{
		if (dsdb_base_files[i].lmapa)
		{
			memset(dsdb_base_files[i].lmapa,0,dsdb_base_files[i].maxlmapa*sizeof(long));
		}
	}

	file_fd = dsdb_sabre(base,O_RDWR | O_BINARY,SH_DENYNO);
	if (file_fd < 0)
		return -1;
	/* antes que nada bloqueo especial */
	if (dsdb_lseek(file_fd,(ds_off)DBLOCK_FLAG_REC,SEEK_SET) == (ds_off)-1)
	{
		dsdb_cierra(file_fd);
		return -1;
	}
	if (dsdb_locking(file_fd,LK_NBLCK,1))
	{
		unsigned char c;
		while(-1)
		{
			if (dsdb_lseek(file_fd,DBLOCK_FLAG_REC,SEEK_SET) == (ds_off)-1)
			{
				 return -1;
			}
			if (dsdb_lee(file_fd,&c,1) == 1) break;
			/*if (!dsdb_locking(base_fd,9999,1L)) break;*/
			if (errno != EACCES && errno != EAGAIN && errno != 13 && errno != 35)
			{
				 return -1;
			}		
			dsdb_usleep(1);
		}
		dsdb_cierra(file_fd);
		return 0;
	}

	/* fase 1: Identificar fichero y ante cualquier duda descartar la operacion */
	if (modo == 2)
	{
		aviso(0,"Fase 1: comprobacion inicial y ganar control de acceso");
	}	

	tamfile = dsdb_lseek(file_fd,(ds_off)0L,SEEK_END);
	if (tamfile == (ds_off)-1 || tamfile < (ds_off)512)
	{
	    dsdb_lseek(file_fd,(ds_off)DBLOCK_FLAG_REC,SEEK_SET);
	    dsdb_locking(file_fd,LK_UNLCK,1);
		dsdb_cierra(file_fd);
		return -1;
	}

	if (dsdb_lseek(file_fd,(ds_off)0L,SEEK_SET) == (ds_off)-1)
	{		
	    dsdb_lseek(file_fd,(ds_off)DBLOCK_FLAG_REC,SEEK_SET);
	    dsdb_locking(file_fd,LK_UNLCK,1);
		dsdb_cierra(file_fd);
		return -1;
	}
	/* cualquier problema en lecturas ... descartar la reparacion */
	if (dsdb_lee(file_fd,(void *)&header,sizeof(header)) != sizeof(header))
	{
		/* si no se puede leer la cabecera o al menos el equivalente ...  */
	    dsdb_lseek(file_fd,(ds_off)DBLOCK_FLAG_REC,SEEK_SET);
	    dsdb_locking(file_fd,LK_UNLCK,1);
		dsdb_cierra(file_fd);
		return -1;
	}

	/* SI LA CABECERA ESTA DAÑADA NO ARREGLAMOS */
	/* PARA RECONSTRUCCIONES EXTREMAS EL DSDB HA DE CREAR UNA CABECERA VALIDA */
	if (header.id != IDDSDB || (header.version != DSDB_VERSION && header.version != DSDB_MIN_VERSION))
	{
		/* de momento no hacemos la reconstruccion "dramatica" */
	    dsdb_lseek(file_fd,(ds_off)DBLOCK_FLAG_REC,SEEK_SET);
	    dsdb_locking(file_fd,LK_UNLCK,1);
		dsdb_cierra(file_fd);
		return -1;
	}
	/* comprobar el tamaño del bloque */
	blocksize = header.header.blocksize;
	if (!blocksize || blocksize > 65536 || (blocksize%512))
	{
	    dsdb_lseek(file_fd,(ds_off)DBLOCK_FLAG_REC,SEEK_SET);
	    dsdb_locking(file_fd,LK_UNLCK,1);
		dsdb_cierra(file_fd);
		return -1;
	}
	if (tamfile < (ds_off)(blocksize*2))
	{
	    dsdb_lseek(file_fd,(ds_off)DBLOCK_FLAG_REC,SEEK_SET);
	    dsdb_locking(file_fd,LK_UNLCK,1);
		dsdb_cierra(file_fd);
		return -1;
	}
	/* asegurarnos que hay bloques .... al menos uno con la marca correxcta en los 20 siguientes */
	pos = (ds_off)blocksize;
	i = 0;
	while(pos < tamfile && i < 20)
	{
		if (dsdb_lseek(file_fd,pos,SEEK_SET) == (ds_off)-1)
		{		
			dsdb_lseek(file_fd,(ds_off)DBLOCK_FLAG_REC,SEEK_SET);
			dsdb_locking(file_fd,LK_UNLCK,1);
			dsdb_cierra(file_fd);
			return -1;
		}
		if (dsdb_lee(file_fd,(void *)&blkid,sizeof(ulong)) != sizeof(ulong))
		{
			dsdb_lseek(file_fd,(ds_off)DBLOCK_FLAG_REC,SEEK_SET);
			dsdb_locking(file_fd,LK_UNLCK,1);
			dsdb_cierra(file_fd);
			return -1;
		}
		if (blkid == IDDSDB || blkid == IDBIDB || blkid == IDBSDB || blkid == IDBFDB ||
			blkid == IDBDDB || blkid == IDBMPB)
		{
			break;
		}
		i++;
		pos += (ds_off)blocksize;
	}
	if (i >= 20)
	{
		dsdb_lseek(file_fd,(ds_off)DBLOCK_FLAG_REC,SEEK_SET);
		dsdb_locking(file_fd,LK_UNLCK,1);
		dsdb_cierra(file_fd);
		return -1;
	}

	dsdb_ajusta_tmps(blocksize);

	/* bloquear de escritura ... global a pelo  ... */
	if (dsdb_lseek(file_fd,DBPOS_LOCK,SEEK_SET) == (ds_off)-1)
	{
		dsdb_lseek(file_fd,(ds_off)DBLOCK_FLAG_REC,SEEK_SET);
		dsdb_locking(file_fd,LK_UNLCK,1);
		dsdb_cierra(file_fd);
		return -1;
	}

	i = 0;
	while(dsdb_locking64(file_fd,LK_WAIT,LENALLLOCKS) != 0 && i < 120)
	{
		sleep(1); /* no hay prisa, la marca de recuperacion los pondra a esperar */
		i++;
	}	
	if (i >= 120)
	{
		/* timeout intendado ganar el control total de la base de datos, algo pasa ... */
		dsdb_lseek(file_fd,(ds_off)DBLOCK_FLAG_REC,SEEK_SET);
		dsdb_locking(file_fd,LK_UNLCK,1);
		dsdb_cierra(file_fd);
		return -1;
	}

	tamfile = dsdb_lseek(file_fd,(ds_off)0L,SEEK_END); /* refrescamos el tamaño */

	/* marca de tocado ... comprobar que podemos grabar */
	if (dsdb_lseek(file_fd,DBPOS_TOUCH,SEEK_SET) == (ds_off)-1)
	{
		dsdb_lseek(file_fd,DBPOS_LOCK,SEEK_SET);
		dsdb_locking64(file_fd,LK_UNLCK,LENALLLOCKS);
		dsdb_lseek(file_fd,(ds_off)DBLOCK_FLAG_REC,SEEK_SET);
		dsdb_locking(file_fd,LK_UNLCK,1);
		dsdb_cierra(file_fd);
		return -1;
	}
	/* grabar otra cosa */
	if (dsdb_graba(file_fd,PFLAG_TOUCH,sizeof(FLAG_TOUCH)) != sizeof(FLAG_TOUCH))
	{
		dsdb_lseek(file_fd,DBPOS_LOCK,SEEK_SET);
		dsdb_locking64(file_fd,LK_UNLCK,LENALLLOCKS);
		dsdb_lseek(file_fd,(ds_off)DBLOCK_FLAG_REC,SEEK_SET);
		dsdb_locking(file_fd,LK_UNLCK,1);
		dsdb_cierra(file_fd);
		return -1;
	}
	if (dsdb_lseek(file_fd,DBPOS_TOUCH,SEEK_SET) == (ds_off)-1)
	{
		dsdb_lseek(file_fd,DBPOS_LOCK,SEEK_SET);
		dsdb_locking64(file_fd,LK_UNLCK,LENALLLOCKS);
		dsdb_lseek(file_fd,(ds_off)DBLOCK_FLAG_REC,SEEK_SET);
		dsdb_locking(file_fd,LK_UNLCK,1);
		dsdb_cierra(file_fd);
		return -1;
	}


	for (i = 0;i < nBasesAbiertas;i++)
	{
		if (BasesAbiertas[i].handle < 0)
		{
			base_fd = i;
			break;
		}			
	}
	if (base_fd < 0)
	{
		base_fd = nBasesAbiertas;
		nBasesAbiertas++;
	}

	
	memset(&BasesAbiertas[base_fd],0,sizeof(BasesAbiertas[base_fd]));
	strcpy(BasesAbiertas[base_fd].base,base);
	BasesAbiertas[base_fd].handle = file_fd;
	BasesAbiertas[base_fd].referencia = 1;	
	BasesAbiertas[base_fd].refbloqueoescritura = 999999; /* importante, es un flag para los "tocando" */

	/* Fase 2: Limpiar cabecera y todos los bloques que no son de datos a la pila de libres */
	if (modo == 2)
	{
		aviso(0,"Fase 2: preparar cabecera y limpiar mapas");
	}

	/* no lo grabaremos hasta el final ... */	
	header.header.pdirectory = 0L;
	header.header.pfreeblock = 0L;

	/* de atras adelante para favorecer la espacialidad, (los primeros bloques libres a asignar
	   sean los mas cercanos al principio del fichero) */

	totalextras = 0;
	nblo = (ulong)((ds_off)tamfile/(ds_off)blocksize);
	pos = (ds_off)blocksize * (ds_off)(nblo-1);	
	header.header.ntotalblocks = nblo;

    if (ixxdat_aviso)
    {
        (*ixxdat_aviso)(0,(long)nblo*2,base);
    }		

	while(nblo > 1)
	{
		if (dsdb_lseek(file_fd,pos,SEEK_SET) == (ds_off)-1)
		{
			SalRecons(base_fd);
			return -1;
		}
		if (dsdb_lee(file_fd,(void *)&blkid,sizeof(ulong)) != sizeof(ulong))
		{
			SalRecons(base_fd);
			return -1;
		}
		if (blkid != IDBIDB && blkid != IDBSDB)
		{
			if (dsdb_lseek(file_fd,pos,SEEK_SET) == (ds_off)-1)
			{
				SalRecons(base_fd);
				return -1;
			}
			libre.id = IDBFDB;
			libre.pnext = header.header.pfreeblock;
			libre.nextrablocks = 0;			
			if (dsdb_graba(file_fd,(void *)&libre,sizeof(libre)) != sizeof(libre))
			{
				SalRecons(base_fd);
				return -1;
			}
			header.header.pfreeblock = nblo;
		}
		else
		{
			if (blkid == IDBSDB)
			{
				totalextras++;
			}
		}
		pos -= (ds_off)blocksize;
		nblo--;
		cuenta++;
        if (ixxdat_aviso && !(cuenta%33))
        {			
            (*ixxdat_aviso)(1,cuenta,base);
			{
				long ahora = time(NULL);
				if (ahora != tiempo)
				{
					tiempo = ahora;
					pulsatecla();
				}
			}
        }
	}
	/* grabar cabecera ... */
	header.nupdate++;
	if (dsdb_lseek(file_fd,(ds_off)0L,SEEK_SET) == (ds_off)-1)
	{	
		SalRecons(base_fd);
		return -1;
	}	
	if (dsdb_graba(file_fd,(void *)&header,sizeof(header)) != sizeof(header))
	{
		SalRecons(base_fd);
		return -1;
	}

    BasesAbiertas[base_fd].header = header.header;
    BasesAbiertas[base_fd].nupdate = header.nupdate;
    /* el openid puede ser 0 */

	/* Fase 3: Reconstruir directorio y mapas de ficheros a medida que los encontramos */
	if (modo == 2)
	{
		aviso(0,"Fase 3: reconstruir directorio y mapas");
	}

	pos = (ds_off)blocksize;
	nblo = 2;
	extras = 0;

	busca.handle = file_fd;
	busca.id = IDBMPB;	
	busca.sizex = sizeof(size);
	busca.size0 = sizeof(DBMapEntry);
	busca.blocksize = blocksize;


	while((pos+(ds_off)blocksize) <= tamfile)
	{
		if (dsdb_lseek(file_fd,pos,SEEK_SET) == (ds_off)-1)
		{
			SalRecons(base_fd);
			return -1;
		}
		if (dsdb_lee(file_fd,(void *)&blkid,sizeof(ulong)) != sizeof(ulong))
		{
			SalRecons(base_fd);
			return -1;
		}
		if (blkid == IDBIDB)			
		{
			if (dsdb_lee_bloque(file_fd,nblo,(void *)&inidata,blocksize,sizeof(DBIniData),OFFSET_PADDING,sizeof(ulong)) < 0)
			{				
				SalRecons(base_fd);
				return -1;
			}			
			inidata.padding = 0; /* importante, limpia el flag de tocado */
			memcpy(tabla,inidata.name,MAXNAMEDB);
			memcpy(direntry.name,inidata.name,MAXNAMEDB);
			direntry.filetype = inidata.filetype;
			direntry.pinidata = nblo;			
			d2 = direntry;
			if (dsdb_crea_tabla(base_fd,tabla,&d2,&direntry) < 0)
			{
				SalRecons(base_fd);
				return -1;
			}

			if (inidata.pnext > header.header.ntotalblocks)
			{
				inidata.pnext = 0L; /* esta estropeado ... seguramente habra restos de datos sueltos */
			}
			/* crear mapa */
			inidata.pmapa   =    dsdb_nuevo_bloque(base_fd);
			inidata.ssize   =    sizeof(inidata);
			inidata.reservado = 0;
			((DBArbol *)bloque_tmp1)->id = IDBMPB;
			((DBArbol *)bloque_tmp1)->level  = 0;
			((DBArbol *)bloque_tmp1)->nitems = 1;				
			mapentry.pos = (ds_off)0;
			mapentry.pbloque = nblo;
			memcpy(bloque_tmp1+sizeof(DBArbol),(void *)&mapentry,sizeof(mapentry));
			memset(bloque_tmp1+sizeof(DBArbol)+sizeof(mapentry),0,blocksize-(sizeof(DBArbol)+sizeof(mapentry)));						    
			if (dsdb_graba_bloque(file_fd,inidata.pmapa,(void *)bloque_tmp1,blocksize,blocksize,0,0) < 0)
			{				
				SalRecons(base_fd);
				return -1;
			}
			memcpy(bloque_tmp1,(void *)&inidata,sizeof(inidata));
			if (dsdb_graba_bloque(file_fd,nblo,(void *)bloque_tmp1,sizeof(inidata),blocksize,0,0) < 0)
			{
				SalRecons(base_fd);
				return -1;
			}			

			
			size = inidata.size;
			hasta = (blocksize-sizeof(DBIniData));
			if (size > hasta || inidata.pnext)
			{
				size = hasta;
			}
			hasta = (blocksize-sizeof(DBMoreData));
			/* seguir fichero y grabar mapa */			
			more.pnext = inidata.pnext;			
			more.reservado = 0;
			bloa = nblo;
			memcpy((void *)&morea,(void *)&inidata,sizeof(morea));
			secuencia = 1;
			antini = inidata.pmapa;
			while(more.pnext)
			{				
				real = (ds_off)(more.pnext-1)*(ds_off)blocksize;
				if (dsdb_lseek(file_fd,real,SEEK_SET) == (ds_off)-1)
				{
					SalRecons(base_fd);
					return -1;
				}
				if (dsdb_lee(file_fd,&more,sizeof(more)) != sizeof(more))
				{
					SalRecons(base_fd);
					return -1;
				}
				if (more.id != IDBSDB || DSHIDWORD(more.grabadoyorder) != secuencia || inidata.identry != more.identry)
				{
					morea.pnext = 0L;
					real = (ds_off)(bloa-1)*(ds_off)blocksize;
					if (bloa == nblo)
					{
						inidata.pnext = 0L;
					}
					if (dsdb_lseek(file_fd,real,SEEK_SET) == (ds_off)-1)
					{
						SalRecons(base_fd);
						return -1;
					}
					if (dsdb_graba(file_fd,&morea,sizeof(morea)) != sizeof(morea))
					{
						SalRecons(base_fd);
						return -1;
					}					
					break;
				}

				/* bloque bueno ... Grabar mapa del bloque */
				extras++;
			
				tmppos = size;				
				busca.compara = (void *)&tmppos;
				busca.resultado = (void *)&mapentry;
				busca.r = -1;
				busca.rbloque = 0;
				busca.roffset = 0;				
				busca.padres[0] = 0;
				busca.pfile = NULL;
				r = dsdb_busca_en_arbol(&busca,inidata.pmapa,(ulong)(long)-1,0);
				if (r != 2)
				{
					SalRecons(base_fd);
					return -1;
				}
				mapentry.pos = size;
				mapentry.pbloque = morea.pnext;
				busca.resultado = (void *)&mapentry;
				if (dsdb_inserta_en_arbol(&busca,base_fd,0,bloque_tmp3,&(inidata.pmapa)) < 0)
				{
					SalRecons(base_fd);
					return -1;
				}				

				/* busca.r = 2; /* en este caso apunta al ultimo siempre, y siempre añadimos */
				if (more.pnext)
				{
					size += hasta;
				}
				else
				{
					size += DSLODWORD(more.grabadoyorder);
				}
				secuencia++;

				if (more.pnext > header.header.ntotalblocks)
				{
					more.pnext = 0L; /* esta estropeado ... seguramente habra restos de datos sueltos */
					if (dsdb_lseek(file_fd,real,SEEK_SET) == (ds_off)-1)
					{
						SalRecons(base_fd);
						return -1;
					}
					if (dsdb_graba(file_fd,&more,sizeof(more)) != sizeof(more))
					{
						SalRecons(base_fd);
						return -1;
					}
					break;
				}
				bloa = morea.pnext;
				morea = more;
			}
			if (antini != inidata.pmapa || size != inidata.size)
			{
				memcpy(bloque_tmp1,(void *)&inidata,sizeof(inidata));
				if (dsdb_graba_bloque(file_fd,nblo,(void *)bloque_tmp1,sizeof(inidata),blocksize,0,0) < 0)
				{
					SalRecons(base_fd);
					return -1;
				}
			}
		}
		pos += (ds_off)blocksize;
		nblo++;
		cuenta++;
        if (ixxdat_aviso && !(cuenta%33))
        {			
            (*ixxdat_aviso)(1,cuenta,base);
			{
				long ahora = time(NULL);
				if (ahora != tiempo)
				{
					tiempo = ahora;
					pulsatecla();
				}
			}
        }

	}
    if (ixxdat_aviso)
    {
        (*ixxdat_aviso)(2,0L,base);
    }             

	if (extras < totalextras)
	{
		/* parece que se ha perdido algo */
		char tmp[512];
		if (!modo)
		{			
			sprintf(tmp,"Hay %d bloques de datos desligados, usar 'dsdb Rf %s'.",totalextras-extras,base);
			guarda_error(tmp);
			aviso(0,tmp);
		}
		else
		{
			sprintf(tmp,"Hay %d bloques de datos desligados.",totalextras-extras);
			guarda_error(tmp);
			if (modo == 2)
			{
				aviso(0,"Fase 4: extraer bloques desligados a ficheros y borralos de la base de datos.");				
			}

			/****/
			if (extras < totalextras)
			{
				guarda_error("No se han podido extraer todos los bloques de datos desligados.");
				aviso(0,"No se han podido extraer todos los bloques de datos desligados.\r\n");
			}
		}
	}
	

	dsdb_lseek(file_fd,DBPOS_TOUCH,SEEK_SET);
	dsdb_graba(file_fd,PFLAG_NOTOUCH,sizeof(FLAG_NOTOUCH));

	SalRecons(base_fd);
	return 0;
}


/* BASIC APIS, funciones de punto de entrada */

int dsdb_unlink_(char *filename)
{
	char tabla[MAXNAMEDB];
	char base[MAX_PATH];	
	int  base_fd;	
	int  r;	

	if (!rw_usa_dsdb || !dsdb_pasafiltro(filename))
		return unlink(filename);

	info_unlink++;

    DSDB_ERROR_INICIO;

	saca_base_tabla(filename,base,tabla);

	base_fd = dsdb_base_abre(base);
	if (base_fd < 0)
	{
		if (DSDB_ERROR_ANALIZA())
		{
			return dsdb_unlink_(filename);
		}
		_doserrno = 0L;
		errno = EACCES;
		return -1;
	}
	r =  dsdb_borra_renombra_tabla(base_fd,tabla,NULL);

	dsdb_base_cierra(base_fd);

	if (r < 0 && DSDB_ERROR_ANALIZA())
	{
		return dsdb_unlink_(filename);
	}

	DSDB_ERROR_FIN;

	if (r >= 0)
		r = 0;	
	return r;
}

int dsdb_rename(char *oldname, char *newname)
{
	char tabla[MAXNAMEDB];
	char tabla2[MAXNAMEDB];
	char base[MAX_PATH];
	int  base_fd;	
	int  r;

	info_rename++;

	r = rename(oldname,newname);
	if (!rw_usa_dsdb || !dsdb_pasafiltro(oldname) || !dsdb_pasafiltro(newname))
		return r;

	DSDB_ERROR_INICIO;

	saca_base_tabla(oldname,base,tabla);
	memset(tabla2,0,MAXNAMEDB); /* muy importante */
	strcpy(tabla2,quita_path(newname));	

	base_fd = dsdb_base_abre(base);
	if (base_fd < 0)
	{
		if (DSDB_ERROR_ANALIZA())
		{
			return dsdb_rename(oldname,newname);
		}
		_doserrno = 0L;
		errno = EACCES;
		return -1;
	}
	
	r =  dsdb_borra_renombra_tabla(base_fd,tabla,tabla2);

	dsdb_base_cierra(base_fd);

	if (r < 0 && DSDB_ERROR_ANALIZA())
	{
		return dsdb_rename(oldname,newname);
	}

	DSDB_ERROR_FIN;

	if (r >= 0)
		r = 0;
	return r;
}


int dsdb_share_open_(char *filename, int oflag, ... )
{
	va_list ap;
	int pmode = 0;
    if ( oflag & O_CREAT ) 
	{	
		va_start(ap, oflag);
		pmode = va_arg(ap, int);
		va_end(ap);
	}
	if (rw_usa_dsdb && dsdb_pasafiltro(filename))
	{
		int r;
		DSDB_ERROR_INICIO;
		r = dsdb_u_abre(filename,oflag,SH_DENYNO,pmode);
		if (r < 0 && DSDB_ERROR_ANALIZA())
		{
			return dsdb_share_open_(filename, oflag, pmode);
		}
		DSDB_ERROR_FIN;
		return r;
	}
	else
	    return dsdb_sabre(filename,oflag,SH_DENYNO,pmode);
}


int dsdb_sopen_(char *filename, int oflag, int shflag)
{
	if (rw_usa_dsdb && dsdb_pasafiltro(filename))
	{
		int r;
		DSDB_ERROR_INICIO;
		r = dsdb_u_abre(filename,oflag,shflag);
		if (r < 0 && DSDB_ERROR_ANALIZA())
		{
			return dsdb_sopen_(filename, oflag, shflag);
		}
		DSDB_ERROR_FIN;
		return r;
	}
	else
	    return dsdb_sabre(filename,oflag,shflag);
}

int dsdb_open_(char *filename, int oflag , int pmode)
{
	if (rw_usa_dsdb && dsdb_pasafiltro(filename))
	{
		int r;
		DSDB_ERROR_INICIO;
		r = dsdb_u_abre(filename,oflag,SH_DENYRW,pmode);
		if (r < 0 && DSDB_ERROR_ANALIZA())
		{
			return dsdb_open_(filename, oflag , pmode);
		}
		DSDB_ERROR_FIN;
		return r;
	}
	else
	    return dsdb_abre(filename,oflag,pmode);
}

ds_off dsdb_lseek_(int handle,ds_off offset,int origin)
{
	if (rw_usa_dsdb && dsdb_pasahandle(handle))
	{
		ds_off r;
		DSDB_ERROR_INICIO;		
		r = dsdb_u_lseek(handle,offset,origin);
		if (r == (ds_off)-1 && DSDB_ERROR_ANALIZA())
		{
			return dsdb_lseek_(handle,offset,origin);
		}
		DSDB_ERROR_FIN;
		return r;
	}
	else
		return dsdb_lseek(handle,offset,origin);
}

int dsdb_lockf_(int handle, int mode, long nbytes)
{
	if (rw_usa_dsdb && dsdb_pasahandle(handle))
	{
		int r;
		DSDB_ERROR_INICIO;		
		r = dsdb_u_locking(handle,mode,nbytes);
		if (r < 0 && DSDB_ERROR_ANALIZA())
		{
			return dsdb_lockf_(handle, mode, nbytes);
		}
		DSDB_ERROR_FIN;
		return r;
	}
	else
		return dsdb_locking(handle,mode,nbytes);
}

int dsdb_read_(int handle, void *buffer, unsigned int count)
{
	if (rw_usa_dsdb && dsdb_pasahandle(handle))
	{
		int r;
		DSDB_ERROR_INICIO;
		r = dsdb_u_lee(handle,buffer,count);
		if (r < 0 && DSDB_ERROR_ANALIZA())
		{
			return dsdb_read_(handle,buffer,count);
		}
		DSDB_ERROR_FIN;
		return r;
	}
	else
		return dsdb_lee(handle,buffer,count);
}

int dsdb_write_(int handle, void *buffer, unsigned int count)
{
	if (rw_usa_dsdb && dsdb_pasahandle(handle))
	{
		int r;
		DSDB_ERROR_INICIO;
		r = dsdb_u_graba(handle,buffer,count);
		if (r < 0 && DSDB_ERROR_ANALIZA())
		{
			return dsdb_write_(handle,buffer,count);
		}
		DSDB_ERROR_FIN;
		return r;
	}
	else
		return dsdb_graba(handle,buffer,count);
}

int dsdb_close_(int handle)
{
	if (rw_usa_dsdb && dsdb_pasahandle(handle))
	{
		int r;
		DSDB_ERROR_INICIO;
		r = dsdb_u_cierra(handle);
		if (r < 0 && DSDB_ERROR_ANALIZA())
		{
			return dsdb_close_(handle);
		}
		DSDB_ERROR_FIN;
		return r;
	}
	else
		return dsdb_cierra(handle);
}

/* APIS ESPECIALES .... */

int dsdb_base_cierra(int base_fd)
{	
	DSDB_ERROR_QUE_BASE(base_fd);
	if ((unsigned)base_fd >= (unsigned)nBasesAbiertas || BasesAbiertas[base_fd].handle < 0)
	{
		DSDB_ERROR(3);
		return -1;
	}	
	BasesAbiertas[base_fd].referencia--;
	if (BasesAbiertas[base_fd].referencia <= 0)
	{
		int i;
		DSDB_ERROR_NOMBRE_BASE(BasesAbiertas[base_fd].base);
		/* proceso de finalizacion ? */
		BasesAbiertas[base_fd].referencia = 0;
		BasesAbiertas[base_fd].refbloqueoescritura = 0;

		dsdb_cierra(BasesAbiertas[base_fd].handle);
		BasesAbiertas[base_fd].handle = -1;
		while (nBasesAbiertas && base_fd == (nBasesAbiertas-1) && BasesAbiertas[base_fd].handle < 0)
		{
			nBasesAbiertas--;
			base_fd--;
		}
	}
	return 0;
}

int dsdb_base_abre(char *_base)
{
	int i;
	int base_fd = -1;
	int handle;
	DBFullHeader header;
	char base[512];
	static int primera = 1;
	
	if (primera)
	{
		int n1,n2;
		for (i = 0;i < sizeof(DSDB_RANDOM);i++)
		{
			DSDB_RANDOM[i] = rand();
		}
		primera = 0;
		memset(keycopia,0,sizeof(keycopia));
		for (i = 0;i < 16;i++)
		{
			n1 = copia_key[0]*i + copia_key[2];
			n2 = i*copia_key[1] + copia_key[3];
			if (n1 < 0 || n1 > 247 || n2 < 0 || n2 > 247)
			{
				memset(keycopia,0,sizeof(keycopia));
				break;
			}
		    keycopia[i] = copia_key[n1] ^ copia_key[n2];
		}
	}

	if (es_empresa_copia(_base))
	{
		for (i = 0;i < 16;i++)
		{
			if (keycopia[i])
				break;
		}
		if (i < 16)
		{
			strcpy(base,_base);
		}
		else
		{
			strcpy(base,dame_empresa_copia(_base));
		}			
	}
	else
		strcpy(base,_base);


do
{
	base_fd = -1;

	DSDB_ERROR_NOMBRE_BASE(base);

	for (i = 0;i < nBasesAbiertas;i++)
	{
		if (BasesAbiertas[i].handle > -1)
		{
			if (!strcmp(BasesAbiertas[i].base,base))
			{
				BasesAbiertas[i].referencia++;
				DSDB_ERROR_QUE_BASE(i);
				return i;
			}
		}
		else
			base_fd = i;
	}

	if (base_fd < 0)
	{
		base_fd = nBasesAbiertas;
		nBasesAbiertas++;
	}	

	info_basea++;

	dsdb_ajusta_tmps(dsdb_blocksize);
	
	BasesAbiertas[base_fd].referencia = 0;
	BasesAbiertas[base_fd].refbloqueoescritura = 0;
	strcpy(BasesAbiertas[base_fd].base,base);
	BasesAbiertas[base_fd].handle = dsdb_sabre(base,O_RDWR | O_BINARY,SH_DENYNO);
#ifndef WIN32
	if (BasesAbiertas[base_fd].handle < 0)
	{
		usleep(10000);
		BasesAbiertas[base_fd].handle = dsdb_sabre(base,O_RDWR | O_BINARY,SH_DENYNO);
		if (BasesAbiertas[base_fd].handle < 0)
		{
			int ff;
			ff = open(base,O_RDWR | O_BINARY);
			if (ff >= 0)
			{
				close(ff);
				DSDB_ERROR(212);
				return -1;
			}
		}
	}
#endif
	if (BasesAbiertas[base_fd].handle < 0)
	{		
		if (dsdb_crea_base(base) < -1)
		{
			DSDB_ERROR(1);
			return -1;
		}
		BasesAbiertas[base_fd].handle = dsdb_sabre(base,O_RDWR | O_BINARY,SH_DENYNO);
		if (BasesAbiertas[base_fd].handle < 0)
		{			
			return -1;
		}		
	}

	handle = BasesAbiertas[base_fd].handle;
	BasesAbiertas[base_fd].referencia++;

	DSDB_ERROR_QUE_BASE(base_fd);

	if (dsdb_bloq_escritura_directorio(base_fd,handle) < 0)
	{
		DSDB_ERROR(9);
		dsdb_base_cierra(base_fd);	    
		return -1;
	}

	dsdb_tocando_dir(handle);

	if (dsdb_lee_bloque(handle,1L,(void *)&header,sizeof(header),sizeof(header),DBLOCK_FLAG_DIR,1) < 0)
	{
		DSDB_ERROR(14);
		dsdb_desbloq_escritura_directorio(base_fd,handle);
		dsdb_base_cierra(base_fd);	    
		return -1;
	}	

	/* control de version e integridad */

	if (header.id != IDDSDB)
	{
		DSDB_ERROR(26);
		dsdb_base_cierra(base_fd);	    
		return -1;
	}

	if (header.ssize != sizeof(header) && header.version == DSDB_VERSION)
	{
		DSDB_ERROR(27);
		dsdb_base_cierra(base_fd);	    
		return -1;
	}

	if (header.version < DSDB_MIN_VERSION || header.version > DSDB_VERSION)
	{
		DSDB_ERROR(28);
		dsdb_base_cierra(base_fd);	    
		return -1;
	}	
	

	if (header.version == DSDB_MIN_VERSION)
	{
		header.flags = 0;	
	}

	if ((header.flags & DSDB_FLAG_CRYPTO))
	{
		unsigned char t1[8];
		unsigned char clave[18];

	    *((ulong *)t1) = header.id;
		*((ulong *)( t1 + 4)) = header.ssize;
		memcpy(clave,dsdb_,16);
		if ((header.flags & DSDB_FLAG_COPIA))
		{
			int i;
			for (i = 0;i < 16;i++)
			{
				clave[i] ^= keycopia[i];
			}
		}
		dsdb_c_code((long *)t1,(long *)t1,(long *)clave);
		if (*((ulong *)t1) != header.test1 || *((ulong *)( t1 + 4)) != header.test2)
		{
			dsdb_fin_tocando_dir(handle);			
			dsdb_desbloq_escritura_directorio(base_fd,handle);
			dsdb_base_cierra(base_fd);
			if (es_empresa_copia(base))
			{
				strcpy(base,dame_empresa_copia(base));
				continue;
			}
		    DSDB_ERROR(200);
			return -1;
		}
	}
	break;
} while(-1);

	header.openid++;	

	if (dsdb_graba_dato(handle,1L,(void *)&(header.openid),sizeof(header.openid),OFFSET_OPENID,sizeof(header)/* irrelevante */) < 0)
	{
		DSDB_ERROR(15);
		dsdb_desbloq_escritura_directorio(base_fd,handle);
		dsdb_base_cierra(base_fd);	    
		return -1;
	}

	dsdb_fin_tocando_dir(handle);

	dsdb_ajusta_tmps(header.header.blocksize);

	if (rw_dsdb_bloquesdefecto > 1 || rw_dsdb_bloquesaumento > 1)
	{
		ds_off tama;
		ulong minimo;

		if (rw_dsdb_bloquesdefecto > (rw_dsdb_bloquesaumento*2))
		{
			minimo = rw_dsdb_bloquesdefecto;
		}
		else
		{
			minimo = rw_dsdb_bloquesaumento*2;
		}		
		tama = (ds_off)minimo * (ds_off)header.header.blocksize;
	    if (tama > dsdb_lseek(handle,0L,SEEK_END))
		{
			dsdb_graba_bloque(handle,minimo,"",1,header.header.blocksize,0,0);
		}
	}

	if (dsdb_desbloq_escritura_directorio(base_fd,handle) < 0)
	{
		DSDB_ERROR(10);
		dsdb_base_cierra(base_fd);	    
		return -1;
	}
	
	BasesAbiertas[base_fd].header  = header.header;
	BasesAbiertas[base_fd].openid  = header.openid;
	BasesAbiertas[base_fd].nupdate = header.nupdate;
	BasesAbiertas[base_fd].flags   = header.flags;
	/*BasesAbiertas[base_fd].oupdate = header.nupdate;*/
	
	return base_fd;
}

int dsdb_lee_dir(int base_fd,DBDirEntry **_direntry,int (*callback)(DBDirEntry *))
{
	int handle;
	int r;
	int ndir = 0;
	char tabla[MAXNAMEDB];
	DBHeader *pheader;
    ulong blocksize;
	DBDirEntry direntry;
	DBDirEntry *pdirentry = NULL;
	int nmem = 0;

	if (base_fd < 0 || base_fd >= nBasesAbiertas || BasesAbiertas[base_fd].handle < 0)
	{
		DSDB_ERROR(171);
		return -1;
	}
	
	handle = BasesAbiertas[base_fd].handle;
	blocksize = BasesAbiertas[base_fd].header.blocksize;

	if (dsdb_bloq_lectura_directorio(base_fd,handle,BasesAbiertas[base_fd].openid) < 0)
	{
		DSDB_ERROR(169);		
		return -1;
	}

	pheader = &(BasesAbiertas[base_fd].header);
	if (dsdb_refresca_cabecera(handle,&(BasesAbiertas[base_fd].nupdate),pheader) < 0)
	{
		DSDB_ERROR(170);
		return -1;
	}

	if (pheader->pdirectory)
	{
		DBArbolBusca busca;

		memset(tabla,0,sizeof(tabla));
		busca.handle = handle;
		busca.id = IDBDDB;
		busca.compara = (void *)tabla;
		busca.sizex = sizeof(direntry.name);
		busca.size0 = sizeof(DBDirEntry);
		busca.blocksize = blocksize;
		busca.resultado = (void *)&direntry;
		busca.pfile = NULL;
		do
		{
			busca.r = -1;
			busca.rbloque = 0;
			busca.roffset = 0;
			busca.padres[0] = 0;			
		    r = dsdb_busca_en_arbol(&busca,pheader->pdirectory,(ulong)(long)-1,1);
			if (r < 0)
			{
				DSDB_ERROR(171);
				dsdb_desbloq_lectura_directorio(base_fd,handle,BasesAbiertas[base_fd].openid);			
				return -1;
			}
			if (dsdb_desbloq_lectura_directorio(base_fd,handle,BasesAbiertas[base_fd].openid) < 0)
			{
				DSDB_ERROR(175);
				return -1;
			}
			if (r > 2)
				break;
			memcpy(tabla,direntry.name,sizeof(tabla));
			if (_direntry)
			{
				if (!pdirentry || ndir >= nmem)
				{
					nmem += 100;
					if (pdirentry)
					   pdirentry = (DBDirEntry *)realloc((void *)pdirentry,nmem * sizeof(DBDirEntry));
					else
					   pdirentry = (DBDirEntry *)malloc(nmem * sizeof(DBDirEntry));				
				}
				pdirentry[ndir] = direntry;
			}
			if (callback)
			{
				if ((*callback)(&direntry) == -1)
				{
					break;
				}
			}
			ndir++;			
			if (dsdb_bloq_lectura_directorio(base_fd,handle,BasesAbiertas[base_fd].openid) < 0)
			{
				DSDB_ERROR(176);		
				return -1;
			}			
			if (dsdb_refresca_cabecera(handle,&(BasesAbiertas[base_fd].nupdate),pheader) < 0)
			{
				DSDB_ERROR(188);
				return -1;
			}
		} while(-1);
	}

	dsdb_desbloq_lectura_directorio(base_fd,handle,BasesAbiertas[base_fd].openid);
	/*
	{
		DSDB_ERROR(172);
		return -1;
	}
	*/

	if (_direntry)
	{
		*_direntry = pdirentry;
	}

	return ndir;
}

int dsdb_u_stat_entry(int base_fd,DBDirEntry *pdirentry,DBIniData *pinidata)
{	
	if (base_fd < 0 || base_fd >= nBasesAbiertas || BasesAbiertas[base_fd].handle < 0)
	{
		DSDB_ERROR(177);
		return -1;
	}
	if (!pdirentry->pinidata)
	{
		return -1;
	}
    if (dsdb_lee_bloque(BasesAbiertas[base_fd].handle,pdirentry->pinidata,(void *)pinidata,BasesAbiertas[base_fd].header.blocksize,sizeof(DBIniData),OFFSET_PADDING,sizeof(ulong)) < 0)
    {		
		return -1;
    }
	return 0;
}


char **dsdb_lee_direct(char *pdir,int flags)
{
	int base_fd;
	DBDirEntry *dir = NULL;
	int ndir = 0,i;
	char **ddir = NULL;

	base_fd = dsdb_base_abre(pdir);
	if (base_fd < 0)
	{
		return NULL;
	}

	ndir = dsdb_lee_dir(base_fd,&dir,NULL);

	dsdb_base_cierra(base_fd);

	if (ndir > 0)
	{
		ddir = (char **)malloc(sizeof(char *) * (ndir + 1));
		for (i = 0;i < ndir;i++)
		{
			ddir[i] = malloc(strlen(dir[i].name)+1);
			strcpy(ddir[i],dir[i].name);
		}
		ddir[i] = NULL;
		return ddir;
	}
	return NULL;
}


#ifndef WIN32
/* especial Unix */

int _sopen_(const char *path,int oflag,int shflag,...)
{
	va_list ap;                     
	int pmode = 0;

	if ((oflag & O_CREAT))
	{
		va_start(ap, shflag);
		pmode = va_arg(ap, int);
		va_end(ap);
	}
	if (shflag == SH_DENYRW)
	{
		oflag |= O_EXCL;
	}
	return dsdb_abre(path,oflag,pmode);
}

#endif