
/* Autor: Francisco Gonzalez Valiente */
#include <fgvsys.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef O_BINARY

#include <io.h>
#include <errno.h>

/* extern int errno; */

#endif

#ifndef O_BINARY  /* compatibilidad xenix - dos */

#ifdef FGVBSD
#include <termios.h>
#else
#include <termio.h>
#endif

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
#include <sys/locking.h>
#include <dos.h>
#include <conio.h>
#include <sys/utime.h>
#else
#include <utime.h>
#endif

#ifdef FGV32
#ifndef WATCOMNLM
#include <winioctl.h>
#endif
#endif


#include <rtermin.h>

char *numero_serie = "??????";

static int no_puedor = 0;

extern int os_subtipo;

extern unsigned long NumeroDeSerie;

extern int rtme_version2;


int lectura_fisica = 0;

#include <stdlib.h>
/*extern long atol();*/
extern char *fecha();
long nume_serie = 0L;
long nume_serie_real = 0L;
extern unsigned short local_term[52];
extern unsigned short local_term_[52];
extern unsigned short local_acce_[52];

unsigned short tabla_aplis[52];

static long mis_apli_bloq[52];
static int mis_apli_coun[52];
static int mis_apli_prime = 1;

static int ya_esta_leida = 0;

extern char other_server[128];

extern char *key_codigocapullo;
int familia_monster = 0;
int cabrones_estafadores = 0;

extern char *act_key;
extern char *act_red;
extern char *act_instalacion[52];
extern char *codigo_de_sistema1;
extern char *codigo_de_sistema2;
extern char *codigo_de_sistema3;
extern char *codigo_de_sistema4;
extern char *act_servidor;
extern char *key_generador;

extern int cuantos_se_permiten = 1;
extern int maximo_entran = 9999;

#ifdef RWXARXA
#ifdef RWSERVER
extern int en_conexion_externa;
extern int en_conexion_externa_stop;
extern int remoto_ver1;
extern int remoto_ver2;
extern unsigned char remoto_verletra;
extern int remoto_que_sistema_es;
extern int remoto_es_sistema_grafico;
extern int leecar_server_puro;
#endif
#endif

static char dirfichepro[256] = "/dev/";
static char fichepro[256] = "/dev/.MAKEDEV.old";

extern char BufferSerie[250];

extern int identidad_cliente;

static int AGIMKPausa = 1;

extern int quien_soy1;
extern int quien_soy2;

int cur_pos_dat = 0;
int *quo_vadis = &cur_pos_dat;

int de_new_code(unsigned char *,unsigned short,unsigned int);

long user_cuantos = 0L;

static short procesa_generar_(short numero)
{
   return(-1);
}
#ifdef COMENTARIO
if (numero == 2870)
{
   ya_esta_leida = 777;
   tabla_aplis[50] = 0;
   tabla_aplis[51] = 65000;
   nume_serie_real = 999001L;
   return(777);
}
ya_esta_leida = 778;
if ( (numero & 1) )
{
   tabla_aplis[0] = 5100;
}
/* resto aplis por determinar */

numero >>= 4;
numero &= 0x3f;
tabla_aplis[50] = 31476 + numero * 5;
tabla_aplis[51] = 31480 + numero * 5;
if (numero == 1) numero = 100;
nume_serie_real = 999000L + (long)numero;
return(778);
}
#endif


#ifdef ESDOS

#ifdef FGV32
#ifndef WATCOMNLM

short _LeeAgiMouseKey()
{
   return(-1);
}

#endif
#endif

#if !defined(FGV32) || defined(WATCOMNLM)

#define Sleep S_leep
S_leep(a)
int a;
{
   int i;
   for (i = 0;i < (a * 2000);i++) inp(0);
   return(0);
}

#define MK_status  6 /*; serial status register*/
#define MK_control 4 /*; serial control register */

static unsigned short AGILeeUnNumero(Base)
int Base;
{
   int salida;
   int entrada;
   int i,t,j;
   int numero = 0;
   int origen;
   
   origen = inp(Base+MK_control);
   salida = origen;
   salida &= ~0x3;
   outp(Base+MK_control,salida);
   
   for (i = 0;i < (AGIMKPausa*2);i++) Sleep(1);
   
   salida = inp(Base+MK_control);
   salida |= 0x1;
   outp(Base+MK_control,salida);
   
   for (i = 0;i < AGIMKPausa;i++) Sleep(1);
   entrada=(inp(Base+MK_status) & 0x40)>>6;
   numero = entrada;
   for (i = 1;i < 10;i++)
			{
      salida = inp(Base+MK_control);
      salida |= 0x2;
      outp(Base+MK_control,salida);
      
      for (j = 0;j < (AGIMKPausa/2);j++) Sleep(1);
      
      salida = inp(Base+MK_control);
      salida &= ~0x2;
      outp(Base+MK_control,salida);
      
      for (j = 0;j < (AGIMKPausa/2);j++) Sleep(1);
      
      entrada=(inp(Base+MK_status) & 0x40)>>6;
      numero <<= 1;
      numero |= entrada;
			}
   
   outp(Base+MK_control,origen);
   return(numero);
}

short _LeeAgiMouseKey()
{
			int numero1 = 0,numero2 = 0;
         int base = 0x3f8;
         while(-1)
         {
            numero1 = AGILeeUnNumero(base);
            if (numero1  > 0 &&  numero1 < 1023)
            {
               numero2 = AGILeeUnNumero(base);
               if (numero1 == numero2)
                  return(numero1);
               else
               {
                  if (AGIMKPausa == 1)
                     AGIMKPausa = 10;
                  numero1 = AGILeeUnNumero(base);
               }
               if (numero1  > 0 &&  numero1 < 1023 && numero1 == numero2)
                  return(numero1);
            }
            if (base == 0x2e8) break;
            switch(base)
            {
            case 0x3f8:
               base = 0x2f8;
               break;
            case 0x2f8:
               base = 0x3e8;
               break;
            case 0x3e8:
               base = 0x2e8;
               break;
            default:
               break;
            }
         }
         return(-1);
}
#endif

short LeeAgiMouseKey()
{
			unsigned char tmp[256];
         int r;
         short numero = 0;
         char *par1;
         int ll;
         int f;
#ifndef WATCOMNLM
#ifdef FGV32
         HANDLE  hndFile;
         ULONG   PortNumber;         /* Buffer sent to the driver (Port #). */
         union   {
            ULONG   LongData;
            USHORT  ShortData;
            UCHAR   CharData;
         }   DataBuffer;             /* Buffer received from driver (Data). */
         DWORD   ReturnedLength;     /* Number of bytes returned */
#endif
#endif
         
         f = rw_open_("AGI_KEY1",O_RDWR | O_BINARY);
         if (f < 0)
         {
#if !defined(FGV32) || defined(WATCOMNLM)
            /* aviso(0,"Modulo AGMK no Instalado!!"); */
            numero = _LeeAgiMouseKey();  
            return(numero);
         }
#else
         hndFile = CreateFile(
            "\\\\.\\AGI_KEY1",                    /* Open the Device "file" */
            GENERIC_READ,
            FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            0,
            NULL
            );
         if (hndFile == -1)
         {
            aviso(0,"Modulo AGMK no Instalado!!");
            return(-1);
         }
         PortNumber = 9999;
         par1 = getenv("AGMKPAUSA");
         if (par1)
         {
            PortNumber += atoi(par1);
         }
         DeviceIoControl(hndFile,CTL_CODE( 40000, 0x901, METHOD_BUFFERED, FILE_READ_ACCESS ),          /* IO Control code for Read */
            &PortNumber,
            sizeof(PortNumber),
            &DataBuffer,
            sizeof(short),
            &ReturnedLength,
            NULL
            );
         CloseHandle(hndFile);
         if (DataBuffer.ShortData != 0xffff)
         {
            DataBuffer.ShortData %= 5000;
            if (DataBuffer.ShortData > 1024)
            {
               DataBuffer.ShortData = procesa_generar_(DataBuffer.ShortData);
            }
         }
         return(DataBuffer.ShortData);
}
#endif

tmp[0] = 213;
par1 = getenv("AGMKPAUSA");
if (par1)
{
			AGIMKPausa = atoi(par1);
         if (rw_write_(f,tmp,AGIMKPausa) == AGIMKPausa)
         {
            ll = 1;
         }
         else
            ll = 0;
}
else
{
			ll = rw_write_(f,tmp,1);
}

if (ll == 1)
{
			rw_lseek_(f,0L,SEEK_SET);
         rw_read_(f,tmp+1,1);
         rw_lseek_(f,0L,SEEK_SET);
         rw_read_(f,tmp,1);
}
memcpy(&numero,tmp,2);
/* sprintf(tmp,"Leido [%d] %d",numero,r);
aviso(0,tmp); */
rw_close_(f);

if (numero != 0xffff)
{
			numero %= 5000;
         if (numero > 1024)
         {
            numero = procesa_generar_(numero);
         }
}
return(numero);
}

#else
short LeeAgiMouseKey()
{
#ifndef COMENTARIO
	return -1;
#else
   int ind,mientras;
   char tmp[51];
#ifdef FGVBSD
   struct termios buf;
#else
   struct termio buf;
#endif
   short numero;
   
   tmp[0] = 0;
   tmp[1] = 0;
   for (ind = -1,errno = EBUSY,mientras = 0;
   ind == -1 && errno == EBUSY && mientras < 60;mientras++) {
      errno = 0;
      if ((ind = open("/dev/agmk",O_RDWR)) == -1)
         sleep(1);
   }
   if (ind > -1) {
#ifdef FGVBSD
      if (ioctl(ind,TIOCGETA,&buf) != -1) 
      {
         mientras = 0;
         if (buf.c_cc[2] == (char)123)
         {                        
            if (ioctl(ind,TIOCSETA,&buf) != -1)
            {
               read(ind,tmp,2);
            }
         }
      }
#else
      if (ioctl(ind,TCGETA,&buf) != -1) 
      {
         mientras = 0;
         if (buf.c_cc[6] == (char)123)
         {
            buf.c_line = 1;
            if (ioctl(ind,TCSETA,&buf) != -1)
            {
               read(ind,tmp,2);
            }
         }
      }
#endif
      close(ind);
   }
   numero = *((short *)tmp);
   if (numero != 0xffff)
   {
      numero %= 5000;
      if (numero > 1024)
      {
         numero = procesa_generar_(numero);
      }
   }
   return(numero);
#endif
}
#endif

int agi_control_fildes =-1;


int ds_super_fildes  = -1;
/*static int fildes = -1; */
#define fildes agi_control_fildes

static int establece_red = 0;
static int oculto_red = 0;
static int mas_red = 0;

static int como_maestra = 0;
static unsigned short _en_alared = 0;

unsigned short _Posicion();

#ifdef RWXARXA
#ifdef RWSERVER
static int mi_cliente = -1;
static long que_cliente = 0L;
static int mi_posicion = -1;
static int mis_clientes[5] = {0,0,0,0,0};

int desbloquea_cliente()
{
   int i;
   if (mi_cliente == -1) return(-1);
   for (i = 0;i < 10;i++)
   {
      rw_lseek_(fildes,12999999L,SEEK_SET);
      if (!locking(fildes,1,1L)) break;
   }
   if (i >= 10) return(-1);
   
   rw_lseek_(fildes,10000000L+que_cliente,SEEK_SET);
   locking(fildes,0,1L);
   
   rw_lseek_(fildes,12999999L,SEEK_SET);
   locking(fildes,0,1L);
   
   mi_cliente = -1;
   que_cliente = 0L;
   return(0);
}

int bloquea_cliente()
{
   int i,j,r = -1,cuenta = 0;
   int cliente = -1,limite,cuantos;
   
   if (!en_conexion_externa || !identidad_cliente)
      return(-1);
   
   if (mi_cliente != -1)
   {
      desbloquea_cliente();
   }
   
   if (mi_posicion != _Posicion())
   {
      return(-2);
   }
   
#ifdef DISTINGUECLIENTESPORSISTEMA
   if (remoto_que_sistema_es == 13)
      cliente = 0; /* ntdos */
   else
      if (remoto_que_sistema_es == 11)
         cliente = 1; /* ntgui */
      else
         if (remoto_que_sistema_es == 23)
            cliente = 2; /* sco */
         else
            if (remoto_que_sistema_es == 7)
               cliente = 3; /* bsd */
            else
               cliente = 4; /* cualquier otro minoritario */
#else
	  cliente = 1;
#endif


            
            if (cliente < 0)
               return(-1);
            
            limite = mis_clientes[cliente];
            if (limite <= 0)
            {
               return(-1);
            }
			if (limite >= 49)
				limite = 512;

            
            for (i = 0;i < 10;i++)
            {
               rw_lseek_(fildes,12999999L,SEEK_SET);
               if (!locking(fildes,1,1L)) break;
            }
            if (i >= 10) return(-1);
            
            /* cliente = cliente * 512 + identidad_cliente; */
            
            cuantos = 0;
            for (i = 0;i < 512;i++) 
            {
               if (i == identidad_cliente) continue;
               
               rw_lseek_(fildes,10000000L+(long)(cliente)*512000L+(long)i*1000L,SEEK_SET);
               if (locking(fildes,2,1000L))
               {
                  cuantos++;
               }
               rw_lseek_(fildes,10000000L+(long)(cliente)*512000L+(long)i*1000L,SEEK_SET);
               locking(fildes,0,1000L);
               if (cuantos >= limite)
                  break;
            }
            if (cuantos < limite)
            {
               /* registrar mi propia entrada */
               for (i = 0;i < 1000;i++)
               {
                  rw_lseek_(fildes,10000000L+(long)(cliente)*512000L+(long)identidad_cliente*1000L+(long)i,SEEK_SET);
                  if (!locking(fildes,2,1L))
                  {
                     mi_cliente = cliente;
                     que_cliente = (long)(cliente)*512000L+(long)identidad_cliente*1000L+(long)i;        
                     break;
                  }
               }
               r = 0;
            }
            
            rw_lseek_(fildes,12999999L,SEEK_SET);
            locking(fildes,0,1L);
            return(r);
}
#endif
#endif


int gana_super_usuario()
{
	if (fildes >= 0)
	{
		rw_lseek_(fildes,49999L,SEEK_SET);
		if (!locking(fildes,2,1L))
		{
           return 0;
		}		
	}
	return -1;
}

int deja_super_usuario()
{	
	if (fildes >= 0)
	{
		rw_lseek_(fildes,49999L,SEEK_SET);
		if (!locking(fildes,0,1L))
		{
           return 0;
		}		
	}
	return -1;
}

int super_usuario_password()
{
   char buffer[256];

   if (ds_super_fildes < 0) 
   {
	   aviso(0,"Falta configuracion de superusuario");
	   return -1;
   }
   rw_lseek_(ds_super_fildes,(long)sizeof(long),SEEK_SET);
   rw_read_(ds_super_fildes,buffer,20);
   if (decodifica_pas(buffer) < 0)
   {
	   aviso(0,"Fichero de passwords alterado!");
	   return -1;
   }

   if (DespachadorExterno)
   {
	   (*DespachadorExterno)((int)DESPACHO_SUPERUSUARIO,(int)2,buffer);
   }

   if (!strcmp(buffer,"\1\1\1OK!\1\1\1"))
   {
	   return 1;
   }

   return 0;
}


int super_permiso_opcion_(char *opcion)
{
	long pos;
	char *p,*p1;
	char pass[100];
    if (ds_super_fildes < 0) 
    {
		return -1;
	}
	pos = rw_lseek_(ds_super_fildes,(long)0,SEEK_END) - sizeof(long) - 20;
	if (!pos || (pos %100))
		return -1;

	rw_lseek_(ds_super_fildes,(long)sizeof(long)+20,SEEK_SET);
    p = malloc(pos);
	if (rw_read_(ds_super_fildes,p,pos) != pos)
	{
		free(p);
		return -1;
	}

	if (!p[0])
	{	   
	   memcpy(pass,p+80,20);
	   decodifica_pas(pass);
	   free(p);
		/* bloqueo total */
	   /*
	   if (p1)
	   {
		   return 0;
	   }
	   */
	   if (DespachadorExterno)
	   {
		   (*DespachadorExterno)((int)DESPACHO_SUPERUSUARIO,(int)2,pass);
	   }

	   if (!strcmp(pass,"\1\1\1OK!\1\1\1"))
	   {
		   return 0;
	   }	   
	   return -1;
	}


	/* localizar opcion */
	for (p1 = p+100;p1 < (p+pos);p1 += 100)
	{
		if (!strcmp(p1,opcion))
		{
			break;
		}
	}
	if (p1 >= (p+pos))
	{
		p1 = NULL;
	}

	if (p1)
	{
	   memcpy(pass,p1+80,20);
	   decodifica_pas(pass);
	}
	free(p);

	if (!p1)
	{
		return 0;
	}

    if (DespachadorExterno)
    {
	   (*DespachadorExterno)((int)DESPACHO_SUPERUSUARIO,(int)2,pass);
    }

    if (!strcmp(pass,"\1\1\1OK!\1\1\1"))
    {
	   return 0;
    }	   

	return -1;
}

int gestiona_superusuario(int opcion)
{
	char buffer[512];
	char pp[100];	
	long pos;

	if (opcion == 1 || opcion == 4)
		return 0;

	if (opcion != 5)
	{
		if (opcion == 4)
		{
			buffer[0] = 3;
		}
		else
		{
			buffer[0] = opcion;
		}
		buffer[1] = 0;

		if (DespachadorExterno)
		{
		   (*DespachadorExterno)((int)DESPACHO_SUPERUSUARIO,(int)3,buffer);
		}
	}
	else
		buffer[0] = 1;
	if (buffer[0])
	{	
		if (opcion == 4 || opcion == 5)
		{
		   rw_lseek_(ds_super_fildes,(long)sizeof(long)+20,SEEK_SET);
		   if (opcion == 5)
		   {			   
			   memset(pp,0,100);
			   pp[0] = 1;
			   rw_write_(ds_super_fildes,pp,100);
		   }
		   else
		   {
			   memset(pp,0,100);			   
			   strcpy(pp+81,buffer);
               codifica_pas(pp+80);
			   rw_write_(ds_super_fildes,pp,100);			   
		   }
			
		} else if (opcion == 3)
		{
		   strcpy(pp+1,buffer);
		   codifica_pas(pp);
           rw_lseek_(ds_super_fildes,(long)sizeof(long),SEEK_SET);
		   rw_write_(ds_super_fildes,pp,20);		   
		}
		else
		{
		   int punto = atoi(buffer);
		   if (punto)
		   {
			   char *p;
			   buffer[0] = 0;
			   dame_opcion_de_menu(punto-1,buffer);
			   pos = rw_lseek_(ds_super_fildes,(long)0,SEEK_END) - sizeof(long) - 20;
			   if (!pos || (pos %100))
			   {
				   rw_lseek_(ds_super_fildes,(long)sizeof(long)+20,SEEK_SET);
				   memset(pp,0,100);
				   pp[0] = 1;
				   rw_write_(ds_super_fildes,pp,100);
				   pos = rw_lseek_(ds_super_fildes,(long)0,SEEK_END) - sizeof(long) - 20;
			   }
			   p = malloc(pos);
			   rw_lseek_(ds_super_fildes,(long)sizeof(long)+20,SEEK_SET);
			   if (rw_read_(ds_super_fildes,p,pos) == pos)
			   {
					char *p1;
					for (p1 = p+100;p1 < (p+pos);p1 += 100)
					{
						if (!strcmp(p1,buffer))
						{
							break;
						}
					}
					if (p1 >= (p+pos))
					{
						for (p1 = p+100;p1 < (p+pos);p1 += 100)
						{
							if (!p1[0])
							{
								break;
							}
						}
					}
					pos = sizeof(long) + 20 + p1 - p;
					rw_lseek_(ds_super_fildes,pos,SEEK_SET);
					if (opcion == 2)
					{
						memset(buffer,0,100);
						rw_write_(ds_super_fildes,buffer,100);
					}
					else
					{
					   buffer[81] = 3;
					   buffer[82] = 0;
					   (*DespachadorExterno)((int)DESPACHO_SUPERUSUARIO,(int)3,buffer+81);
					   if (buffer[81])
					   {
						   codifica_pas(buffer+80);
						   rw_write_(ds_super_fildes,buffer,100);
					   }
					}
			   }				   
			   free(p);			   
		   }
		   else
		   {
			   aviso(0,"Debe seleccionar una opcion, no un submenu");
		   }
		}
	}

	return 0;
}

int bloquea_aplicacion(apli)
int apli;
{
   int i,mio,r = 0;
   long pos;
   unsigned short cuentas;
   
   if (mis_apli_prime)
   {
      mis_apli_prime = 0;
      for (i = 0;i < 52;i++)  
      {
         mis_apli_bloq[i] = -1L;
         mis_apli_coun[i] = 0;
      }
   }
   
   if (local_acce_[apli] == 65535) /* ilimitado */
   {
      return(0);
   }
   
   if (mis_apli_bloq[apli] != -1L)
   {
      mis_apli_coun[apli]++;
      return(0);
   }
   
   for (i = 0;i < 10;i++)
   {
      rw_lseek_(fildes,15000000L,SEEK_SET);
      if (!locking(fildes,1,1L)) break;
   }
   if (i >= 10) return(-1);
   
   /* primero contar total puestos */
   cuentas = 0;
   pos = ((long)apli * 512L * 256L);
   for (i = 0;i < 512;i++)
   {
      if (i != identidad_cliente)
      {
         rw_lseek_(fildes,15000001L+pos+i*256,SEEK_SET);
         if (locking(fildes,2,256L))
         {
            cuentas++;
         }
         rw_lseek_(fildes,15000001L+pos+i*256,SEEK_SET);
         locking(fildes,0,256L);
      }
   }
   
   if ((local_acce_[apli]/256) < cuentas)
   {
      /* exceso de puestos */
      rw_lseek_(fildes,15000000L,SEEK_SET);
      locking(fildes,0,1L);
      return(-2);
   }
   
   
   /* luego contar tareas en mi mismo puesto */
   pos = ((long)apli * 512L * 256L) + ((long)identidad_cliente * 256L);
   mio = -1;
   cuentas = 0;
   for (i = 0;i < 256;i++)
   {
      rw_lseek_(fildes,15000001L+pos+i,SEEK_SET);
      if (!locking(fildes,2,1L))
      {
         if (mio == -1) mio = i;
         rw_lseek_(fildes,15000001L+pos+i,SEEK_SET);
         locking(fildes,0,1L);
      }
      else
      {
         cuentas++;
      }
   }
   if ((local_acce_[apli]%256) >= cuentas)
   {
      mis_apli_bloq[apli] = 15000001L+pos+mio;
      rw_lseek_(fildes,15000001L+pos+mio,SEEK_SET);
      locking(fildes,2,1L);
      mis_apli_coun[apli]++;
   }
   else
      r = -3;
   rw_lseek_(fildes,15000000L,SEEK_SET);
   locking(fildes,0,1L);
   return(r);
}

int desbloquea_aplicacion(apli)
int apli;
{
   if (fildes < 0) return(-1);
   if (mis_apli_bloq[apli] != -1L)
			{ 
      mis_apli_coun[apli]--;
      if (mis_apli_coun[apli] <= 0)
      {
         rw_lseek_(fildes,mis_apli_bloq[apli],SEEK_SET);
         locking(fildes,0,1L);
         mis_apli_coun[apli] = 0;
         mis_apli_bloq[apli] = -1L;
      }
			}
   return(0);
}

int bloquea_usuario(int usuario,int modo,short *l_u)
{
			int subnumero = 0;
         int i,j,limi;
         
         
         if (act_key && !strcmp(act_key,"DEMO")) return(0);
         
#ifdef RWXARXA
#ifdef RWSERVER
         if (en_conexion_externa && subnumero >= 0)
         {  
            i = bloquea_cliente();
            if (i < 0)
            {
               i -= 2;
               return(i);
            }
         }
#endif
#endif
         
         
         for (i = 0;i < 10;i++)
         {
            rw_lseek_(fildes,999999L,SEEK_SET);
            if (!locking(fildes,1,1L)) break;
         }
         if (i >= 10) 
         {
#ifdef RWXARXA
#ifdef RWSERVER
            desbloquea_cliente();
#endif
#endif
            return(-1);
         } 
         
         
         user_cuantos = 0L;
         if (l_u)
         {
            for (i = 0;(*l_u) != (short)-1;i++,l_u++)
            {
               rw_lseek_(fildes,1000000L+(long)(*l_u)*1000L,SEEK_SET);
               if (locking(fildes,2,1000L))
               {
                  rw_lseek_(fildes,1000000L+(long)(*l_u)*1000L+20L,SEEK_SET);
                  if (locking(fildes,2,980L))
                  {
                     limi = 1000;
                  }
                  else
                  {
                     rw_lseek_(fildes,1000000L+(long)(*l_u)*1000L+20L,SEEK_SET);
                     locking(fildes,0,980L);
                     limi = 20;
                  }
                  for (j = 0;j < limi;j++)
                  {
                     rw_lseek_(fildes,1000000L+(long)(*l_u)*1000L+j,SEEK_SET);
                     if (locking(fildes,2,1L))
                     {
                        user_cuantos++;
                     }
                     else
                     {
                        rw_lseek_(fildes,1000000L+(long)(*l_u)*1000L+j,SEEK_SET);
                        locking(fildes,0,1L);
                     }
                  }
               }
               else
               {
                  rw_lseek_(fildes,1000000L+(long)(*l_u)*1000L,SEEK_SET);
                  locking(fildes,0,1000L);
               }
            }
         }
         
         do {
            rw_lseek_(fildes,1000000L+(1000L * usuario)+subnumero,SEEK_SET);
            if (locking(fildes,2,1L))
            {
               if (!modo)
               {
                  rw_lseek_(fildes,999999L,SEEK_SET);
                  locking(fildes,0,1L);
#ifdef RWXARXA
#ifdef RWSERVER
                  desbloquea_cliente();
#endif
#endif
                  return(-1);
               }
               subnumero++;
            }
            else
               break;
         } while(-1);
         
         rw_lseek_(fildes,999999L,SEEK_SET);
         locking(fildes,0,1L);
         
         if (cuantos_se_permiten < 51 && cuantos_se_permiten <= ((int)user_cuantos))
         {
#ifdef RWXARXA
#ifdef RWSERVER
            desbloquea_cliente();
#endif
#endif
            return(-2);
         }
         
#ifdef RWXARXA
#ifdef RWSERVER
         if (subnumero < 0)
            desbloquea_cliente();
#endif
#endif
         return(subnumero);
}


int dame_mi_fildes()
{
   return(fildes);
}

int desbloquea_usuario(usuario,subnumero)
int usuario;
int subnumero;
{
   int i;
   
   if (act_key && !strcmp(act_key,"DEMO")) return(0);
   
   if (fildes < 0) return(-1);
#ifdef RWXARXA
#ifdef RWSERVER
   desbloquea_cliente();
#endif
#endif
   
   for (i = 0;i < 10;i++)
   {
      rw_lseek_(fildes,999999L,SEEK_SET);
      if (!locking(fildes,1,1L)) break;
   }
   if (i >= 10) return(-1);
   
   rw_lseek_(fildes,1000000L+(1000L * usuario)+subnumero,SEEK_SET);
   locking(fildes,0,1L);
   
   rw_lseek_(fildes,999999L,SEEK_SET);
   locking(fildes,0,1L);
   return(0);
}

int codifica_pas(char *buf)
{
	int i;
    buf[0] = strlen(buf+1);
	for (i = 0;i < (19/buf[0]+1);i++)
	{
		memcpy(buf+1+buf[0]*i,buf+1,buf[0]);
	}

    for(i = 1;i < 20;i += 2)
    {
	   buf[i] ^= 55;
    }
    for(i = 0;i < 20;i += 2)
    {
	   buf[i] ^= buf[i+1];
    }
	return 0;
}

int decodifica_pas(char *buffer)
{
   int i,l;
   for(i = 0;i < 20;i += 2)
   {
	   buffer[i] ^= buffer[i+1];
   }
   for(i = 1;i < 20;i += 2)
   {
	   buffer[i] ^= 55;
   }
   l = buffer[0];
   if (l < 0 || l > 19) return -1;
   if (l < 19 &&  buffer[1] != buffer[1+l]) return -1;
   buffer[1+l] = 0;
   strcpy(buffer,buffer+1);
   return 0;
}

static long mi_numero_de_ejecucion = 0L;
static int  ya_tiene_numero = 1;

void pon_numero_agisys(long numero_de_ejecucion)
{
	mi_numero_de_ejecucion = numero_de_ejecucion;
	ya_tiene_numero = 1;
}

int inicio_agisys()
{
   char buf[81];
   long n1,n2;
   long i;
   int fd;
   char buf1[1024];
#ifdef FGVDOS  
   char buf2[1024];
   DWORD nada1,nada2;
   BOOL r;  
#else
   struct stat buffer;
#endif
   
   if (act_key && !strcmp(act_key,"DEMO")) return(0);
   
#ifdef FGVDOS
   r =  GetVolumeInformation("C:\\",buf1,sizeof(buf1),(DWORD *)&NumeroDeSerie,&nada1,&nada2,buf2,sizeof(buf2));
#else
   buffer.st_ino = rand();
   if (stat( fichepro, &buffer ) < 0)
			{
      crea_inodo();
      stat( fichepro, &buffer );	 
			}
   NumeroDeSerie = buffer.st_ino * 153243L;
#endif

   /*NumeroDeSerie |= ((!((time(NULL) >> 16) ^ 0x3929)) * 0x3929);*/
   
   memset(buf1,0,sizeof(buf1));
   sprintf(buf1,"%lu",NumeroDeSerie);
   memcpy(buf,buf1+6,6);
   buf[6] = 0;
   buf1[6] = 0;
   n1 = atol(buf1);
   n2 = atol(buf);
   NumeroDeSerie = n1 ^ n2;
   
   strcpy(buf,rw_directorio("informa.pan"));
   if (fildes != -1) rw_close_(fildes);
   fildes = rw_open_(buf,O_RDWR | O_BINARY);
   if (fildes < 0)
			{
      char tmpx[256];
      sprintf(tmpx,"No hay entorno correcto en %s. Indique la ruta al llamar al programa",rw_directorio(""));
      aviso(0,tmpx);
      fin_sig();
      Terfinal();
      exit(2);
			}
   
   if (!ya_tiene_numero)
   {
	   for (mi_numero_de_ejecucion = 50052L;mi_numero_de_ejecucion < 51000L;mi_numero_de_ejecucion++)
	   {
		  rw_lseek_(fildes,mi_numero_de_ejecucion,SEEK_SET);
		  if (!locking(fildes,2,1L))
		  {
			 break;
		  }
	   }
   }
   

   if (ds_super_fildes != -1) rw_close_(ds_super_fildes);
   strcpy(buf,rw_directorio("dev/usr/super_usr.psw"));
   ds_super_fildes = rw_open_(buf,O_RDWR | O_BINARY);
   if (ds_super_fildes >= 0)
   {
	   long id = 0L;
	   rw_read_(ds_super_fildes,&id,sizeof(long));
	   if (id != ID_SUPERFILE)
	   {
		   rw_close_(ds_super_fildes);
		   unlink(buf);
		   ds_super_fildes = -1;
	   }
   }

   if (ds_super_fildes < 0)
   {
	   long id = ID_SUPERFILE;
	   ds_super_fildes = rw_open_(buf,O_CREAT | O_TRUNC | O_RDWR | O_BINARY,S_IREAD|S_IWRITE);
	   rw_write_(ds_super_fildes,&id,sizeof(long));
	   strcpy(buf+1,"diagram");
	   codifica_pas(buf);
	   rw_write_(ds_super_fildes,buf,20);
   }
   
   return(0);
}


int wait_agisys()
{

   return 0;

   /*
   if (fildes < 0) return(-1);    
  
   if (mi_numero_de_ejecucion)
			{
      rw_lseek_(fildes,mi_numero_de_ejecucion,SEEK_SET);
      locking(fildes,0,1L);
			}
   
   for (mi_numero_de_ejecucion = 50002L;mi_numero_de_ejecucion < 50052L;mi_numero_de_ejecucion++)
			{
      rw_lseek_(fildes,mi_numero_de_ejecucion,SEEK_SET);
      if (!locking(fildes,2,1L))
      {
         break;
      }
			}
   */
   return(0);
}

int restore_agisys()
{
	return 0;
	/*
   if (fildes < 0) return(-1);    

   if (mi_numero_de_ejecucion)
			{
      rw_lseek_(fildes,mi_numero_de_ejecucion,SEEK_SET);
      locking(fildes,0,1L);
			}
   
   for (mi_numero_de_ejecucion = 50052L;mi_numero_de_ejecucion < 51000L;mi_numero_de_ejecucion++)
			{
      rw_lseek_(fildes,mi_numero_de_ejecucion,SEEK_SET);
      if (!locking(fildes,2,1L))
      {
         break;
      }
			}
   return(0);
   */
}

long dame_mi_numero()
{
   return(mi_numero_de_ejecucion);
}

int final_agisys()
{
   long il;
   int i;
   
   if (act_key && !strcmp(act_key,"DEMO")) return(0);
 
   if (ds_super_fildes != -1)
   {
	   rw_close_(ds_super_fildes);
   }
   ds_super_fildes = -1;

   if (fildes < 0) return(-1);
   
   if (como_maestra)
			{
      rw_lseek_(fildes,90000L,SEEK_SET);
      for (i = 0;locking(fildes,1,1L) && i < 10;i++);
      if (i < 10)
      {
         for (il = 100000L;il < 100016L;il++)
         {
            rw_lseek_(fildes,il,SEEK_SET);
            if ((_en_alared & 0x1))
            {
               locking(fildes,0,1L);
            }
            _en_alared >>= 1;
         }
         rw_lseek_(fildes,90001L,SEEK_SET);
         locking(fildes,0,1L);
         
         rw_lseek_(fildes,90000L,SEEK_SET);
         locking(fildes,0,1L);
      }
			}
   como_maestra = 0;
   
   if (!ya_tiene_numero)
   {
	   rw_lseek_(fildes,mi_numero_de_ejecucion,SEEK_SET);
	   locking(fildes,0,1L);
   }
   
   rw_close_(fildes);
   fildes = -1;

  
   return(0);
}

static int es_muy_nula  = 0;



unsigned short _Posicion()
{
   char nom[257];
   long il;
   int i,j;
   unsigned short potencia;
   int si = 0;
   static short leido = 0;
   static short mk = 0;
   static esta_red = 0;
   char tm;
   int especial = 0;
   
   if (ya_esta_leida)
			{
      return(ya_esta_leida);
			}
   
   if (es_muy_nula) return(0);
   
   
   mk = LeeAgiMouseKey();
   
   if (mk == 777) 
			{
      mas_red = 0;
      establece_red = 0;
      return(mk);
			}
   
   
#ifdef ESDOS
#ifndef FGVWIN
   if (hay_raton)
			{
#ifdef AGIEXTEND
         /*  memset(&REALREG,0,sizeof(REALREG));
         //  inregs.x.ax = 0x300;
         //  inregs.h.bl = 0x33;
         //  inregs.h.bh = 0;
         //  inregs.x.cx = 0;
         //  REALREG.AX = 0x16;
         //  segread( &b_sregs );
         //  b_sregs.es = FP_SEG((&REALREG));
         //  inregs.x.di = FP_OFF((&REALREG));
      //  int86x(0x31, &inregs, &inregs, &b_sregs );*/
#else
      /*  inregs.x.ax = 0x15;
      //  int86(0x33,&inregs,&inregs);
      //  salvacion = malloc(inregs.x.bx + 1);
      
       //  inregs.x.ax = 0x16;
       //  b_sregs.es = FP_SEG(salvacion);
       //  inregs.x.dx = FP_OFF(salvacion);
       //  int86(0x33, &inregs, &inregs);
       
        //  inregs.x.ax = 0x1F;
      //  int86(0x33, &inregs, &inregs);*/
#endif
			}
#endif
#endif
   
   if (establece_red != esta_red)
			{
      esta_red = establece_red;
      si = 1;
      mk = 0;
			}
   
   if (!(leido%500) || si)
			{
      if (!establece_red || lectura_fisica)
      {
         mk = LeeAgiMouseKey();
         mas_red = 0;
         establece_red = 0;
      }
      else
      {
         if (mas_red < 200 && mas_red != LeeAgiMouseKey())
         {						
            return(-1);
         }
         if (fildes < 0)
         {						
            return(-1);
         }
         mk = 0;
         rw_lseek_(fildes,90000L,SEEK_SET);
         for (i = 0;locking(fildes,1,1L) && i < 10;i++);
         if (i < 10)
         {
            rw_lseek_(fildes,90001L,SEEK_SET);
            if (!locking(fildes,2,1L))
            {
               rw_lseek_(fildes,90001L,SEEK_SET);
               locking(fildes,0,1L);
               especial = 1;
            }
            else
            {
               potencia = 1;
               for (il = 100000L;il < 100016L;il++)
               {
                  rw_lseek_(fildes,il,SEEK_SET);
                  if (locking(fildes,2,1L))
                  {
                     mk += potencia;
                  }
                  else
                  {
                     rw_lseek_(fildes,il,SEEK_SET);
                     locking(fildes,0,1L);
                  }
                  potencia *= 2;
               }
            }
            rw_lseek_(fildes,90000L,SEEK_SET);
            locking(fildes,0,1L);
         }
         if (especial)
         {
            mk = LeeAgiMouseKey();
            mas_red = 0;
            establece_red = 0;
         }
         else
         {
            if (!mk)
               mk = -1;
            else
               mk ^= oculto_red;
         }
      }
			}
   if (mk != -1)
      leido++;
   
#ifdef ESDOS
#ifndef FGVWIN
   if (hay_raton)
			{
#ifdef AGIEXTEND
         /*  memset(&REALREG,0,sizeof(REALREG));
         //  inregs.x.ax = 0x300;
         //  inregs.h.bl = 0x33;
         //  inregs.h.bh = 0;
         //  inregs.x.cx = 0;
         //  REALREG.AX = 0x17;
         //  segread( &b_sregs );
         //  b_sregs.es = FP_SEG((&REALREG));
         //  inregs.x.di = FP_OFF((&REALREG));
      //  int86x(0x31, &inregs, &inregs, &b_sregs );*/
#else
      /*  inregs.x.ax = 0x20;
      //  int86(0x33, &inregs, &inregs);
      
       //  inregs.x.ax = 0x17;
       //  b_sregs.es = FP_SEG(salvacion);
       //  inregs.x.dx = FP_OFF(salvacion);
       //  int86x(0x33, &inregs, &inregs, &b_sregs );
       //  inregs.x.ax = 0x0;
       //  int86(0x33, &inregs, &inregs);
       
      //  free(salvacion);*/
#endif
			}
#endif
#endif
   
   return(mk);
}




static int acabamiento(quo_vadis)
int *quo_vadis;
{
   int i = *quo_vadis * 345 / 3;
   if (i > 400)
      fin();
   if (i)
      fin();
   return(*quo_vadis^0xffff);
}


#ifndef FGVDOS
int crea_inodo()
{
   int f1;
   char tmp[512];
   char buf1[512];
   int i,fd;
   long tiempo = time(NULL);
   long signature = 0x666,t1,midata,od;
   struct stat buffer;
   struct utimbuf tb;
   struct tm *lc;
   int ann;   
   
   tiempo %= 100;
   

   if (!strcmp(fichepro,"/dev/.MAKEDEV.old"))
   {
	   strcpy(tmp,rw_directorio("dev/.ds"));
	   if (stat( tmp, &buffer ) >= 0)
	   {
		   strcpy(fichepro,tmp);
		   strcpy(dirfichepro,rw_directorio("dev/"));
		   return 0;
	   }	   
	   if (creat( fichepro , S_IREAD | S_IWRITE ) < 0)
	   {
		   strcpy(fichepro,tmp);
		   strcpy(dirfichepro,rw_directorio("dev/"));
	   }
   }

   unlink(fichepro);
   for (i = 0;i < tiempo;i++)
   {
      sprintf(tmp,"%s.wfa.%d",dirfichepro,i);
      fd = creat( tmp , S_IREAD | S_IWRITE );
      if (fd >= 0) close(fd);   
   }
   fd = creat( fichepro , S_IREAD | S_IWRITE );
   if (fd >= 0) close(fd);   
   for (i = 0;i < tiempo;i++)
   {
      sprintf(tmp,"%s.wfa.%d",dirfichepro,i);
      unlink(tmp);
   }
   f1 = rw_open_(fichepro,O_RDWR | O_BINARY);
   if (f1 < 0)
   {
	   return -1;
   }
   stat( fichepro, &buffer );
   midata = buffer.st_mtime;
   od = midata;
   lc = localtime(&midata);
   if (lc) 
			{
      ann = lc->tm_year;
      if (ann > 50)
         ann += 1900;
      else
         ann += 2000;
      sprintf(buf1,"%02d%02d%04d",lc->tm_mday,(lc->tm_mon)+1 ,ann);  
      midata = atol(buf1);
			}
   else
			{
      rw_close_(f1);
      return(0);  
			}
   
   rw_lseek_(f1,0L,SEEK_SET);
   moto_long(&signature);
   rw_write_(f1,&signature,sizeof(long));
   moto_long(&signature);
   t1 = midata ^signature;
   moto_long(&t1);
   rw_write_(f1,&t1,sizeof(long));
   rw_write_(f1,&t1,sizeof(long));
   rw_write_(f1,&t1,sizeof(long));
   rw_write_(f1,&t1,sizeof(long));
   rw_close_(f1);
   moto_long(&t1);
   
   tb.modtime = od;
   tb.actime = od;
   utime( fichepro, &tb );
   chmod( fichepro,S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
   chown( fichepro,0,0);

   return(0);
}

int recoge_activacion(char *res)
{
   int f1;
   long signature = 0x666,t1,midata,od,activa;
   char buf1[512];
   char fiche1[512];
   struct stat buffer;
   struct utimbuf tb;
   struct tm *lc;
   int ann;
   
   res[0] = 0;
   
   strcpy(fiche1,fichepro);
   f1 = rw_open_(fiche1,O_RDWR | O_BINARY);
   if (f1 < 0)
			{   
       no_puedor = 1;
      /*crea_inodo();*/
      return(-1);   
			}
   memset(buf1,0,sizeof(buf1));
   rw_read_(f1,&signature,sizeof(long));
   if (signature != 0x666)
			{
      rw_close_(f1);
      return(0);
			}
   stat( fiche1, &buffer );
   midata = buffer.st_mtime;
   od = midata;
   lc = localtime(&midata);
   if (lc) 
			{
      ann = lc->tm_year;
      if (ann > 50)
         ann += 1900;
      else
         ann += 2000;
      sprintf(buf1,"%02d%02d%04d",lc->tm_mday,(lc->tm_mon)+1 ,ann);  
      midata = atol(buf1);
			}
   else
			{
      rw_close_(f1);
      return(0);  
			}
   
   rw_read_(f1,&activa,sizeof(long));
   if (midata == (activa ^ signature))
			{
      rw_close_(f1);
      tb.modtime = od;
      tb.actime = od;
      utime( fichepro, &tb );
      return(0);  
			}
   rw_close_(f1);
   activa ^= midata;
   sprintf(res,"%lu",activa);
   tb.modtime = od;
   tb.actime = od;
   utime( fichepro, &tb );
   return(1);
}

int pon_activacion(char *res)
{
   int f1;
   long signature = 0x666,t1,midata,od,activa;
   char buf1[512];
   char fiche1[512];
   struct stat buffer;
   struct utimbuf tb;
   struct tm *lc;
   int ann;
   
   
   strcpy(fiche1,fichepro);
   f1 = rw_open_(fiche1,O_RDWR | O_BINARY);
   if (f1 < 0)
			{   
      return(-1);   
			}
   memset(buf1,0,sizeof(buf1));
   rw_read_(f1,&signature,sizeof(long));
   if (signature != 0x666)
			{
      rw_close_(f1);
      return(0);
			}
   stat( fiche1, &buffer );
   midata = buffer.st_mtime;
   od = midata;
   lc = localtime(&midata);
   if (lc) 
			{
      ann = lc->tm_year;
      if (ann > 50)
         ann += 1900;
      else
         ann += 2000;
      sprintf(buf1,"%02d%02d%04d",lc->tm_mday,(lc->tm_mon)+1 ,ann);  
      midata = atol(buf1);
			}
   else
			{
      rw_close_(f1);
      return(0);  
			}
   
   activa = atol(res) ^ midata;
   rw_write_(f1,&activa,sizeof(long));
   rw_close_(f1);
   tb.modtime = od;
   tb.actime = od;
   utime( fichepro, &tb );
   return(1);
}



#endif

static int buen_oro_(long nume_serie,int posicion,char *res)
{
   unsigned long filtro;
   unsigned long activacion;
   unsigned char buf[100];
   
   sprintf(buf,"%06lu",NumeroDeSerie);
   buf[6] = 0;
   sprintf(BufferSerie,"%06lu-%03u %s %s",nume_serie,posicion,buf,res);   
   if (!res[0]) return(0);
   
   activacion = atol(res);
   
   sprintf(buf,"%06lu",NumeroDeSerie);
   buf[6] = 0;
   buf[0] ^= posicion;
   buf[1] ^= nume_serie;
   buf[2] ^= posicion;
   buf[3] ^= nume_serie;
   buf[4] ^= posicion;
   buf[5] ^= nume_serie;
   filtro = buf[0]*1000 + buf[1]*100 + buf[2]*10 + buf[3]*6 + buf[4]*2 + buf[5];
   filtro ^= nume_serie;
   
   if ((nume_serie >= 510000 && nume_serie <= 599999) || filtro == (unsigned long)atol(res))
			{
      maximo_entran = 9999;
      return(1);
			}
         /*sprintf(buf,"%lu",filtro);
   aviso(0,buf);*/
   return(0);
}

static int buen_gene(unsigned long activa,unsigned long nume_serie)
{
   unsigned long filtro;
   int numero;
   unsigned char buf[100];
   if (!key_generador) return(0);
   
   sprintf(buf,"%07lu",activa);
   buf[7] = 0;
   buf[0] ^= activa;
   buf[1] ^= nume_serie;
   buf[2] ^= activa/12;
   buf[3] ^= nume_serie;
   buf[4] ^= activa;
   buf[5] ^= nume_serie;
   buf[6] ^= nume_serie;
   filtro = buf[0]*1000 + buf[1]*100 + buf[2]*10 + buf[3]*6 + buf[4]*2 + buf[5];
   filtro ^= nume_serie;
   
   if (filtro = (unsigned long)atol(key_generador))
			{
      if (nume_serie == 999001L)
         return(2870);
      else
      {
         if (nume_serie > 999001L)
         {
            numero = (int)(999001L - 999000L);
            numero <<= 4;
            numero |= 1;
            return(numero);
         }
      }
			}
   return(0);
}

static int procesa_codigocapullo(unsigned long activa,unsigned long nume_serie)
{
   unsigned long filtro;
   int numero;
   unsigned char buf[100];
   if (!key_codigocapullo) return(0);
   
   sprintf(buf,"%07lu",activa);
   buf[7] = 0;
   buf[6] ^= activa;
   buf[5] ^= nume_serie;
   buf[4] ^= activa/12;
   buf[3] ^= nume_serie;
   buf[2] ^= activa;
   buf[1] ^= nume_serie;
   buf[0] ^= nume_serie;
   filtro = buf[0]*1000 + buf[1]*100 + buf[2]*10 + buf[3]*6 + buf[4]*2 + buf[5];
   filtro ^= nume_serie;
   
   if (filtro = (unsigned long)atol(key_codigocapullo))
			{
      return((familia_monster = 1));
			}
   return(0);
}


static int pon_respaldo(unsigned long numero,unsigned long data2,unsigned long data)
{
   int r = 0;
#ifdef FGVDOS
   HKEY hKey = NULL;
   DWORD dispo;
   LONG retv;
   DWORD Disposition;
   DWORD Type;
   DWORD Activa;
   DWORD Size;
   
   if ((retv = RegCreateKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Diagram8",0,"Diagram8",REG_OPTION_NON_VOLATILE,/*KEY_QUERY_VALUE | KEY_SET_VALUE*/ KEY_ALL_ACCESS,NULL,&hKey,&Disposition)) == ERROR_SUCCESS)
			{
      Size = sizeof(Activa);
      Type = REG_DWORD;
      Activa = numero ^ data2 ^ data;
      RegSetValueEx(hKey,"Indicando",0,REG_DWORD,&Activa,sizeof(DWORD));
      RegCloseKey(hKey);
			}
#else
   int f1;
   long signature = 0x666,t1,midata,od,activa;
   char buf1[512];
   char fiche1[512];
   struct stat buffer;
   struct utimbuf tb;
   struct tm *lc;
   int ann;
   
   strcpy(fiche1,fichepro);
   f1 = rw_open_(fiche1,O_RDWR | O_BINARY);
   if (f1 < 0)
			{      
      return(0);   
			}
   memset(buf1,0,sizeof(buf1));
   rw_read_(f1,&signature,sizeof(long));
   if (signature != 0x666)
			{
      rw_close_(f1);
      return(0);
			}
   stat( fiche1, &buffer );
   midata = buffer.st_mtime;
   od = midata;
   lc = localtime(&midata);
   if (lc) 
			{
      ann = lc->tm_year;
      if (ann > 50)
         ann += 1900;
      else
         ann += 2000;
      sprintf(buf1,"%02d%02d%04d",lc->tm_mday,(lc->tm_mon)+1 ,ann);  
      midata = atol(buf1);
			}
   else
			{
      rw_close_(f1);
      return(0);  
			}
   
   activa = midata;
   rw_write_(f1,&activa,sizeof(long));
   t1 = numero ^ midata;
   rw_write_(f1,&t1,sizeof(long));
   t1 = data2 ^ midata;
   rw_write_(f1,&t1,sizeof(long));
   t1 = data ^ midata;
   rw_write_(f1,&t1,sizeof(long));
   rw_close_(f1);
   
   tb.modtime = od;
   tb.actime = od;
   utime( fichepro, &tb );
   
   r = 1;
#endif
   
   return(r);
}


static int ver_respaldo(unsigned long numero,unsigned long data2,unsigned long data)
{
   int r = 0;
#ifdef FGVDOS
   HKEY hKey = NULL;
   DWORD dispo;
   LONG retv;
   DWORD Disposition;
   DWORD Type;
   DWORD Activa;
   DWORD Size;
   
   if ((retv = RegCreateKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Diagram8",0,"Diagram8",REG_OPTION_NON_VOLATILE,/*KEY_QUERY_VALUE | KEY_SET_VALUE*/ KEY_ALL_ACCESS,NULL,&hKey,&Disposition)) == ERROR_SUCCESS)
			{
      Size = sizeof(Activa);
      Activa = 0L;
      Type = REG_DWORD;
      retv = RegQueryValueEx(  hKey,"Indicando",NULL,&Type,&Activa,&Size);
      if (Activa == 125782L)
      {
         r = 0;
      }
      else
      {
         if (Activa != (numero ^ data2 ^ data)) r = 1;
      }
      RegCloseKey(hKey);  
			}
#else
   int f1;
   long signature = 0x666,t1,midata,od,activa;
   char buf1[512];
   char fiche1[512];
   struct stat buffer;
   struct utimbuf tb;
   struct tm *lc;
   int ann;
   
   
   strcpy(fiche1,fichepro);
   f1 = rw_open_(fiche1,O_RDWR | O_BINARY);
   if (f1 < 0)
			{      
      return(1);   
			}
   memset(buf1,0,sizeof(buf1));
   rw_read_(f1,&signature,sizeof(long));
   if (signature != 0x666)
			{
      rw_close_(f1);
      return(1);
			}
   stat( fiche1, &buffer );
   midata = buffer.st_mtime;
   od = midata;
   lc = localtime(&midata);
   if (lc) 
			{
      ann = lc->tm_year;
      if (ann > 50)
         ann += 1900;
      else
         ann += 2000;
      sprintf(buf1,"%02d%02d%04d",lc->tm_mday,(lc->tm_mon)+1 ,ann);  
      midata = atol(buf1);
			}
   else
			{
      rw_close_(f1);
      return(1);  
			}
   
   rw_read_(f1,&activa,sizeof(long));
   if (midata == (activa ^ signature)) /* punto debil */
			{
      rw_close_(f1);
      return(0);  
			}
   rw_read_(f1,&t1,sizeof(long));
   t1 ^= midata;
   if (t1 != numero)
			{
      rw_close_(f1);
      return(1);  
			}
   rw_read_(f1,&t1,sizeof(long));
   t1 ^= midata;
   if (t1 != data2)
			{
      rw_close_(f1);
      return(1);  
			}
   rw_read_(f1,&t1,sizeof(long));
   t1 ^= midata;
   if (t1 != data)
			{
      rw_close_(f1);
      return(1);  
			}
   rw_close_(f1);
   r = 0;
#endif
   
   return(r);
}

static int comprueba_tiempo(int aponer)
{
   char fiche1[256];
   unsigned long signature;
   unsigned long numero;
   unsigned long otro;
   unsigned long midata;
   unsigned long data,data2,a,b,t1,od;
   unsigned char buf1[256];
   char *p;
   int f1;
   int ann;
   struct stat buffer;
   struct utimbuf tb;
   struct tm *lc;
   
   strcpy(fiche1,rw_directorio("dev/imp/ibm80.bak"));
   f1 = rw_open_(fiche1,O_RDWR | O_BINARY);
   if (f1 < 0)
   {   
      return(0);   
   }
   memset(buf1,0,sizeof(buf1));
   rw_read_(f1,&signature,sizeof(long));
   if (signature != 0x777)
   {
      rw_close_(f1);
      return(0);
   }
   stat( fiche1, &buffer );
   midata = buffer.st_mtime;
   od = midata;
   lc = localtime(&midata);
   if (lc) 
   {
      ann = lc->tm_year;
      if (ann > 50)
         ann += 1900;
      else
         ann += 2000;
      sprintf(buf1,"%02d%02d%04d",lc->tm_mday,(lc->tm_mon)+1 ,ann);  
      midata = atol(buf1);
   }
   else
   {
      rw_close_(f1);
      return(0);  
   }
   
   rw_read_(f1,&numero,sizeof(long));
   numero ^= midata;
   rw_read_(f1,&data2,sizeof(long));
   data2 ^= midata;
   rw_read_(f1,&data,sizeof(long));
   data ^= midata;
   rw_read_(f1,&otro,sizeof(long));
   
   
   if (otro != (numero ^ data))
   {
      rw_close_(f1);
      return(0);  
   }
   
   
   a = time(NULL);
   lc = localtime(&a);
   if (lc) 
   {
      ann = lc->tm_year;
      if (ann > 50)
         ann += 1900;
      else
         ann += 2000;
      sprintf(buf1,"%02d%02d%04d",lc->tm_mday,(lc->tm_mon)+1 ,ann);  
      a = atol(buf1);
   }
   else
   {
      rw_close_(f1);
      return(0);  
   }
   
#ifdef FGVDOS
   p = getenv("windir");
   if (p)
   {
      if (os_subtipo == 1)
         sprintf(buf1,"%s\\system32\\config\\security.log",p);
      else
         sprintf(buf1,"%s\\system.ini",p);
   }
   else
   {
      rw_close_(f1);
      return(0);
   }
#else
   strcpy(buf1,"/var/log/messages");
#endif
   if (stat(buf1,&buffer) < 0)
   {
      rw_close_(f1);
      return(0);
   }
   b = buffer.st_mtime;
   lc = localtime(&b);
   if (lc) 
   {
      ann = lc->tm_year;
      if (ann > 50)
         ann += 1900;
      else
         ann += 2000;
      sprintf(buf1,"%02d%02d%04d",lc->tm_mday,(lc->tm_mon)+1 ,ann);  
      b = atol(buf1);
   }
   else
   {
      rw_close_(f1);
      return(0);  
   }
   
   /* comprobar validez del contador */
   if (!aponer && ver_respaldo(numero,data2,data)) 
   {
      numero  = 100;
      data = 0;
      data2 = 0;
   }
   
   if (aponer || a != data || b != data2)
   {
      if (aponer)
         numero = 1;
      else
         numero++;
      data2 = b;
      data = a; /* siempre la fecha del sistema */
      rw_lseek_(f1,0L,SEEK_SET);
      rw_write_(f1,&signature,sizeof(long));
      t1 = numero ^ midata;
      rw_write_(f1,&t1,sizeof(long));
      t1 = data2 ^ midata;
      rw_write_(f1,&t1,sizeof(long));
      t1 = data ^ midata;
      rw_write_(f1,&t1,sizeof(long));
      t1 = numero ^ data;
      rw_write_(f1,&t1,sizeof(long));
      pon_respaldo(numero,data2,data);
   }
   
   rw_close_(f1);
   
   tb.modtime = od;
   tb.actime = od;
   utime( fiche1, &tb );
   
   if (numero > 15) 
      numero = 0;
   else
      numero = 16 - numero;
   
   return(numero);
}

/*
int cuadro_activacion(char *num1,char *num2,char *res)
{
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      char tmpxx[1024];
      int i;
      strcpy(tmpxx,num1);
      i = strlen(num1) + 1;
      strcpy(tmpxx+i,num2);
      i += (strlen(num2) + 1);
      strcpy(tmpxx+i,res);
      i += (strlen(res) + 1);	 
      funcion_windows_cliente(14,tmpxx,i);
      strcpy(res,tmpxx);
      return(0);
   }
#endif
   
   {
      pushv(1020,2063);
      zonablan(1020,2063);
      cuadro(1020,2063);
      Ar();
      pprint(1021," Entre su N�mero de Activaci�n ");
      Cr();
      pprint(1222,"IDENTIFICACION:");
      pprint(1422,"N�mero de ORO.:");
      pprint(1622,"Activaci�n....:");
      pprint(1822,"TELEFONOS PARA SOLICITAR CLAVE BASICA");
      Ai();
      pprint(1238,num1);
      pprint(1438,num2);
      pprint(1931,"902100177 / 965337743");
      Bi();
      res[0] = 0;
      entra(1638,23,"","",res,0,res);
      quitabp(res);
      quitab(res);
      popv(0);
   }
   
   
   return(0);
}
*/

int dame_oro(char *res)
{
#ifdef FGVDOS
   HKEY hKey = NULL;
   DWORD dispo;
   LONG retv;
   DWORD data;
   DWORD Disposition;
   DWORD Type;
   DWORD Activa;
   DWORD Size;   
   
   if ((retv = RegCreateKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Diagram8",0,"Diagram8",REG_OPTION_NON_VOLATILE,/*KEY_QUERY_VALUE | KEY_SET_VALUE*/ KEY_ALL_ACCESS,NULL,&hKey,&Disposition)) == ERROR_SUCCESS)
   {
      Size = sizeof(Activa);
      Activa = 0L;
      Type = REG_DWORD;
      retv = RegQueryValueEx(  hKey,"Activacion",NULL,&Type,&Activa,&Size);  
      if (retv != ERROR_SUCCESS)
      {
         retv = RegQueryValueEx(  hKey,"Indicando",NULL,&Type,&Activa,&Size);  
         if (!Activa)
         {
            Activa = 125782L;
            RegSetValueEx(hKey,"Indicando",0,REG_DWORD,&Activa,sizeof(DWORD));
         }
         Activa = 0L;
      }
      RegCloseKey(hKey);
      sprintf(res,"%lu",(unsigned long)Activa);
   }   
#else   
   recoge_activacion(res);   
#endif
   return 0;
}

/* Activacion = Clave Basica */
static int llama_activacion(long nume_serie,int posicion)
{
   unsigned char num1[257];
   unsigned char num2[257];
   unsigned char res[257];
   int tiempo,numero;
#ifdef FGVDOS
   HKEY hKey = NULL;
   DWORD dispo;
   LONG retv;
   DWORD data;
   DWORD Disposition;
   DWORD Type;
   DWORD Activa;
   DWORD Size;   
   
   if ((retv = RegCreateKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Diagram8",0,"Diagram8",REG_OPTION_NON_VOLATILE,/*KEY_QUERY_VALUE | KEY_SET_VALUE*/ KEY_ALL_ACCESS,NULL,&hKey,&Disposition)) == ERROR_SUCCESS)
   {
      Size = sizeof(Activa);
      Activa = 0L;
      Type = REG_DWORD;
      retv = RegQueryValueEx(  hKey,"Activacion",NULL,&Type,&Activa,&Size);  
      if (retv != ERROR_SUCCESS)
      {
         retv = RegQueryValueEx(  hKey,"Indicando",NULL,&Type,&Activa,&Size);  
         if (!Activa)
         {
            Activa = 125782L;
            RegSetValueEx(hKey,"Indicando",0,REG_DWORD,&Activa,sizeof(DWORD));
         }
         Activa = 0L;
      }
      RegCloseKey(hKey);
      sprintf(res,"%lu",(unsigned long)Activa);
   }   
#else   
   recoge_activacion(res);   
#endif   
   
   if (nume_serie == 123456L && posicion == 789)
   {
      char tmpxx[81];  
      
      
      NumeroDeSerie ^= time(NULL);
      sprintf(tmpxx,"%06lu",NumeroDeSerie);
      
      
      tmpxx[6] = 0;
      NumeroDeSerie = atol(tmpxx);
      
      
      tiempo = comprueba_tiempo(0);
      if (tiempo > 1)
      {
         if (!getenv("NOESDSPRUEBA"))
         {
            sprintf(num1,"Versi�n de pruebas, dispone de %d dias aproximadamente!",tiempo);
            aviso(0,num1);
         }
         ya_esta_leida = posicion;
         return 0;     
      }
   }
   else if (buen_oro_(nume_serie,posicion,res))
   {
      /*
      else
      if (numero == 777)
      {
      ya_esta_leida = 777;
      tabla_aplis[50] = 0;
      tabla_aplis[51] = 65000;
      nume_serie_real = 999001L;
      }
      else
      if (numero == 777)
      {
      ya_esta_leida = 777;
      tabla_aplis[50] = 0;
      tabla_aplis[51] = 65000;
      nume_serie_real = 999001L;
      }
      else
      if (DataBuffer.ShortData == 777)
      {
      ya_esta_leida = 777;
      tabla_aplis[50] = 0;
      tabla_aplis[51] = 65000;
      nume_serie_real = 999001L;
      }
      ya_esta_leida = posicion;
      if (nume_serie == 999001 && posicion == 777)
      {
      tabla_aplis[50] = 0;
      tabla_aplis[51] = 65000;
      nume_serie_real = 999001L;
      }	
         */  
         ya_esta_leida = posicion;
         if (procesa_codigocapullo(atol(res),nume_serie))
         {
            cabrones_estafadores = 1;
         }
         
         /*
         if ((numero = buen_gene(atol(res),nume_serie)))
         {
         procesa_generar_(numero);
         }
         */
         return(0);
      }
      
      
      sprintf(num1,"%06ld-%03d",nume_serie,posicion);
      sprintf(num2,"%06ld",NumeroDeSerie);
      res[0] = 0;
      
      if (no_puedor)
      {
          aviso(0,"No tiene permiso o por algun motivo no puede activar el numero de oro");
      }
      else
      {
          dialogo_activacion(num1,num2,res);
      }
/*
#ifdef FGVWIN
      dialogo_activacion(num1,num2,res);
#else
      cuadro_activacion(num1,num2,res);
#endif
*/    
      if (!buen_oro_(nume_serie,posicion,res))
      {
         if ((tiempo = comprueba_tiempo(0)))
         {
            if (tiempo > 1)
               sprintf(num1,"Dispone de %d dias aproximadamente!, Recuerde pedir la clave b�sica!!",tiempo);
            else
               sprintf(num1,"Ultimo dia!!, VUELVA A ACCEDER Y PIDA LA CLAVE BASICA HOY!!");
            aviso(0,num1);
            ya_esta_leida = posicion;
            return(0);
         }
         
         if (nume_serie == 123456L && posicion == 789)
         {
            aviso(0,"PERIODO DE PRUEBAS FINALIZADO, DEBE SERIALIZAR CORRECTAMENTE LA INSTALACION!");
         }
         
         fin();
      }
      
      ya_esta_leida = posicion;
      
      if (nume_serie == 123456L && posicion == 789)
      {
         tiempo = comprueba_tiempo(1);
         if (tiempo != 15)
         {
            aviso(0,"PERIODO DE PRUEBAS NO APLICABLE, DEBE SERIALIZAR CORRECTAMENTE LA INSTALACION!");
            fin();
         }
         return 0;
      }
      
#ifdef FGVDOS
      if ((retv = RegCreateKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Diagram8",0,"Diagram8",REG_OPTION_NON_VOLATILE,/*KEY_QUERY_VALUE | KEY_SET_VALUE*/ KEY_ALL_ACCESS,NULL,&hKey,&Disposition)) == ERROR_SUCCESS)
      {
         data = (DWORD)atol(res);
         RegSetValueEx(hKey,"Activacion",0,REG_DWORD,&data,sizeof(DWORD));
         RegCloseKey(hKey);
      }
#else
      pon_activacion(res);
#endif
      
      return(0);
}

static unsigned short orip_i;

lee_prot()
{
   long il;
   unsigned short nume_code;
   unsigned short posicion,nume_mochila;
   unsigned char buf[257];
   unsigned short mascara;
   unsigned short paquete,version,numeropa;
   unsigned short red = 0,oculto = 0;
   unsigned short alared,checo;
   unsigned short comprueba,otra_red = 0;
   
   int i;
   char tmp[81];
   
   if (!act_key) return(-1);
   /* printf("Decodificando %s = ",act_key);*/
   quita_blancos(act_key);
   
   if (act_key && !strcmp(act_key,"DEMO")) return(0);
   
   
   if (act_red && !establece_red)
   {
      quita_blancos(act_red);
      strcpy(buf,act_red);
      de_new_code(buf,33,9);
      red = buf[5] + buf[1] * 50 + buf[3] * 50 * 50;
      oculto = buf[2] + buf[4] * 50 + buf[0] * 50 * 50;
      oculto_red = oculto;
      red ^= oculto;
      comprueba = buf[6] + buf[7] * 50 + buf[8] * 50 * 50;
      comprueba ^= oculto;
      if (red >= 200 || red == _Posicion())
      {
         if (red <= 200)
            cuantos_se_permiten = 9999;
         else
            cuantos_se_permiten = red - 199;
         mas_red = red;
         establece_red = 1;
      }
   }
   
   if (cuantos_se_permiten > maximo_entran)
      cuantos_se_permiten = maximo_entran;
   
   strcpy(buf,act_key);
   de_new_code(buf,99,12);
   checo = (buf[3] ^ buf[1] ^ buf[2] ^ buf[9] ^ buf[7]) % 50;
   posicion = buf[10] + buf[8] * 50;
   nume_serie = buf[0] + buf[4] * 50L + buf[5] * 50L * 50L + buf[6] * 50L * 50L * 50L;
   
   if (nume_serie < 0L || nume_serie > 999999L || posicion > 1022 || posicion < 0) return(-1);
   /*
   if ((nume_serie >= 999000L && posicion != 777) || (nume_serie < 999000L && posicion == 777))
   {
   aviso(0,"NO DISPONE DE UN CODIGO DE ACCESO LEGAL");
   nume_serie = 0L;
   posicion = -1;
   checo = 0;
   }
   */
   if (buf[11] != checo &&  !codigo_de_sistema1 && 
      !codigo_de_sistema2 &&
      !codigo_de_sistema3 &&
      !codigo_de_sistema4
      )
      return(-1);
   
   
   ya_esta_leida = 0;
   nume_serie_real = 0L;
   
#ifdef RWXARXA
   if (strlen(other_server) && other_server[1] == ':')
   {   
      pastilla_remota(&ya_esta_leida,&quien_soy1,&quien_soy2,&nume_serie_real,tabla_aplis);
   }
#endif
   
   if (!ya_esta_leida || ya_esta_leida != posicion || nume_serie != nume_serie_real)
   {  
      nume_serie_real = 0L;
      ya_esta_leida = 0;
      quien_soy1 = 0;
      quien_soy2 = 0;
      llama_activacion(nume_serie,posicion);  
   }
   else
      maximo_entran = 9999;
   
   
   /* ver si es combinada */
   if (buf[11] == checo)
   { 
      if (!posicion)
      {
         red = 0;
         nume_code = buf[1] + buf[2] * 50;
         nume_code += 10000;
         nume_mochila = (unsigned short)nume_serie;   
         pon_code(nume_code);
         pon_mochila(nume_mochila);
         es_muy_nula = 1;
         cuantos_se_permiten = 3;
         local_term[50] = 27999;
         local_term_[50] = 19999;
         local_term[51] = 27999;
         local_term_[51] = 19999;
         if (verifica_sistema(NULL))
         {
            aviso(0,"Falta Codigo de Sistema Adecuado");
            return(-1);
         }
         return(0);
      }
      
      nume_code = 00000;
      nume_mochila = 00000;   
      if (!act_red)
      { /* maximo 2499 tareas */
         red = buf[1] + buf[2] * 50;
         if (red)
         {
            oculto_red = checo;
            otra_red = 1;
            comprueba = posicion;
            if (red >= 200 || red == _Posicion())
            {
               if (red <= 200)
                  cuantos_se_permiten = 9999;
               else
                  cuantos_se_permiten = red - 199;
               mas_red = red;
               establece_red = 1;		   
            }
         }
      }
   }
   
   if (cuantos_se_permiten > maximo_entran)
      cuantos_se_permiten = maximo_entran;
   
   
   mascara = _Posicion();
   orip_i = mascara;
   mascara = ((mascara%50) ^ (mascara/50)) % 50;
   strcpy(buf,act_key);
   de_new_code(buf,mascara%50,12);
   checo = (buf[3] ^ buf[1] ^ buf[2] ^ buf[9] ^ buf[7]) % 50;
   
   switch(buf[3])
   {
   case 11:
   case 24:
   case 7:
   case 23:
	  break;
   case 9:
      buf[3] = 11;
      break;
   case 10:
      buf[3] = 24;
      break;
   case 3:
      buf[3] = 7;
      break;
   case 6:
      buf[3] = 23;
      break;
   default:
      aviso(0,"No corresponde la clave con Diagram9");
      return(-1);
   }    
   
   posicion = buf[10] + buf[8] * 50;
   nume_serie = buf[0] + buf[4] * 50L + buf[5] * 50L * 50L + buf[6] * 50L * 50L * 50L;
   /* ver si es combinada */
   if (buf[11] == checo)
   {
      /* !!!!!!!!!!! */
      nume_code = 00000;
      nume_mochila = 00000;   
   }
   else
   {
      nume_code = buf[3] + buf[1] * 50 + buf[2] * 50 * 50;
      nume_mochila = buf[9] + buf[7] * 50 + buf[11] * 50 * 50;
      nume_mochila ^= nume_code;
      nume_code ^= posicion;
   }
   
   /*
   if ((nume_serie >= 999000L && posicion != 777) || (nume_serie < 999000L && posicion == 777))
   {
   aviso(0,"NO DISPONE DE UN CODIGO DE ACCESO LEGAL");
   nume_serie = 0L;
   posicion = -1;
   checo = 0;
   nume_code = -1;
   nume_mochila = -1;
   }
   */
   
   pon_code(nume_code);
   pon_mochila(nume_mochila);
   cur_pos_dat = _Posicion() - posicion;
   alared = posicion ^ oculto_red;
   if (!mascara || mascara == 49)
   {
      if (_Posicion() == 0xffff) fin();
   }
   if (acabamiento(quo_vadis) != 0xffff)
      exit(3);
   
   for (i = 0;i < 52;i++)
   {
      if (act_instalacion[i])
      {
         quita_blancos(act_instalacion[i]);
         strcpy(buf,act_instalacion[i]);
         de_new_code(buf,mascara%50,9);
         paquete = buf[5] + buf[1] * 50 + buf[3] * 50 * 50;
         paquete = paquete ^ nume_code ^ (short)(nume_serie & 0xffff);
         version = buf[2] + buf[4] * 50 + buf[0] * 50 * 50;
         version = version ^ nume_code ^ (short)(nume_serie & 0xffff);
         numeropa = buf[8] + buf[7] * 50 + buf[6] * 50 * 50;
         if (i < 50 || version == 19999)
         {
            local_term[i]  = paquete;
            local_term_[i] = version;
            local_acce_[i] = numeropa;
         }
      }
   }
   
   for (i = 0;i < 50;i++)
   {
      if (local_term[i] == -1)
      {
         local_term[i] = 27999 + posicion;
         local_term_[i] = 101;
         break;
      }
   }
   
   if (act_red && !establece_red && posicion != comprueba)
   {
      cuantos_se_permiten = 0;
   }
   
   if ((act_red || otra_red) && !establece_red && posicion == comprueba)
   {
      if (fildes < 0)
      {
         aviso(0,"Falta modulo de instalacion");
         fin();
      }
      rw_lseek_(fildes,90000L,SEEK_SET);
      for (i = 0;locking(fildes,1,1L) && i < 10;i++);
      if (i < 10)
      {
         rw_lseek_(fildes,90001L,SEEK_SET);
         if (!locking(fildes,2,1L))
         {
            _en_alared = alared;
            for (il = 100000L;il < 100016L;il++)
            {
               rw_lseek_(fildes,il,SEEK_SET);
               if ((alared & 0x1))
               {
                  locking(fildes,2,1L);
               }
               alared >>= 1;
            }
            como_maestra = 1;
         }
         rw_lseek_(fildes,90000L,SEEK_SET);
         locking(fildes,0,1L);
      }
   }
   
   if (verifica_sistema(NULL))
   {
      aviso(0,"Falta Codigo de Sistema Adecuado");
      return(-1);
   }
   
   return(0);
}



int fin()
{
   char tmp[81];
   cierra_todo(1);
#ifdef ESDOS
   fcloseall();
#endif
   fin_sig();
   sprintf(tmp,"Instalacion Incorrecta o Sin Modulo de Proteccion Adecuado [%d]",orip_i);
   aviso(0,tmp);
   Terfinal();
   exit(0);
}

p_serie()
{
   if (nume_serie)
      printf(" NUMERO DE LICENCIA:%06lu ",nume_serie);
}


static char *codigo_de_sistema = NULL;





int pon_clave(clave,np,nv)
char *clave;
int np,nv;
{
   int i;
   unsigned char buf[257];
   unsigned short paquete,version,numeropa,mascara = ((_Posicion() % 50) ^ (_Posicion() / 50)) % 50;
   
   for (i = 0;i < 50;i++)
   {
      if (local_term[i] == np &&  local_term_[i] == nv)
      {
         return(0);
      }
   }
   
   quita_blancos(clave);
   if (!strlen(clave)) return(-1);
   
   strcpy(buf,clave);
   de_new_code(buf,mascara%50,9);
   paquete = buf[5] + buf[1] * 50 + buf[3] * 50 * 50;
   paquete = paquete ^ nume_code() ^ (short)(nume_serie & 0xffff);
   version = buf[2] + buf[4] * 50 + buf[0] * 50 * 50;
   version = version ^ nume_code() ^ (short)(nume_serie & 0xffff);
   numeropa = buf[8] + buf[7] * 50 + buf[6] * 50 * 50;
   if (paquete != np || version != nv)
   {
      return(-1);
   }
   
   for (i = 0;i < 50;i++)
   {
      if (local_term[i] == -1 &&  local_term_[i] == -1)
      {
         local_term[i] = paquete;
         local_term_[i] = version;
         local_acce_[i] = numeropa;
         if (act_instalacion[i])
            free(act_instalacion[i]);
         act_instalacion[i] = malloc(strlen(clave)+1);
         strcpy(act_instalacion[i],clave);
         graba_fgv(1);
         return(0);
      }
   }
   return(-2);
}

char *dame_codigo_de_sistema(prefe)
int prefe;
{
   unsigned char *p1[5];
   unsigned char buf[81];
   int i;
   p1[0] = codigo_de_sistema1;
   p1[1] = codigo_de_sistema2;
   p1[2] = codigo_de_sistema3;
   p1[3] = codigo_de_sistema4;
   
   for (i = 0;i < 4;i++)
			{
      if (p1[i])
      {
         strcpy(buf,p1[i]);
         de_new_code(buf,99,5);
         if (!prefe && (buf[3] == QUE_SISTEMA_SOY || (buf[3] == 24 && (QUE_SISTEMA_SOY == 11 || QUE_SISTEMA_SOY == 13))))
         {
            return(p1[i]);
         }
         else
            if (prefe == 1 && (buf[3] == 11 || (buf[3] == 24 && (QUE_SISTEMA_SOY == 11 || QUE_SISTEMA_SOY == 13))))
            {
               return(p1[i]);
            }
            else
               if (prefe == 2 && (buf[3] == 13 || (buf[3] == 24 && (QUE_SISTEMA_SOY == 11 || QUE_SISTEMA_SOY == 13))))
               {
                  return(p1[i]);
               }
      }
			}
   return(NULL);
}


char *dame_codigo_de_paquete(num,ver)
int num,ver;
{
			int i;
         unsigned char buf[81];
         unsigned short paquete,version;
         unsigned short nume_code,checo;
         unsigned short posicion,nume_mochila;
         long _nume_serie;
         unsigned short n_c = 00000;
         unsigned short n_m = 00000;
         
         if (act_key)
         {  
            strcpy(buf,act_key);
            de_new_code(buf,99,12);  
            _nume_serie = buf[0] + buf[4] * 50L + buf[5] * 50L * 50L + buf[6] * 50L * 50L * 50L;
            posicion = buf[10] + buf[8] * 50;
            checo = (buf[3] ^ buf[1] ^ buf[2] ^ buf[9] ^ buf[7]) % 50;
            /* ver si es combinada */
            if (buf[11] == checo)
            {
               nume_code = 00000;
               nume_mochila = 00000;
            }
            else
            {
               nume_code = buf[3] + buf[1] * 50 + buf[2] * 50 * 50;        
               nume_mochila = buf[9] + buf[7] * 50 + buf[11] * 50 * 50;
               nume_mochila ^= nume_code;
               nume_code ^= posicion;
            }
         }
         else
            return(NULL);
         
         for (i = 0;i < 50;i++)
         {
            if (act_instalacion[i])
            {
               strcpy(buf,act_instalacion[i]);
               de_new_code(buf,99,9);
               paquete = buf[5] + buf[1] * 50 + buf[3] * 50 * 50;
               paquete = paquete ^ nume_code ^ (short)(_nume_serie & 0xffff);
               version = buf[2] + buf[4] * 50 + buf[0] * 50 * 50;
               version = version ^ nume_code ^ (short)(_nume_serie & 0xffff);
               if (paquete == num && version == ver)
               {
                  return(act_instalacion[i]);
               }
            }
         }
         
         return(NULL);
}



int verifica_sistema(prt)
int *prt;
{
   unsigned short checo;
   int o,pp;
   unsigned char tmp[81];
   unsigned char buf[81];
   
   if (!prt) prt = &pp;
   
   if (act_key)  
   {
      *prt = 0;
      if (!strcmp(act_key,"DEMO"))
						{     
         return(0);
						}  
      strcpy(buf,act_key);
      de_new_code(buf,99,12);
      checo = (buf[3] ^ buf[1] ^ buf[2] ^ buf[9] ^ buf[7]) % 50;
      
      switch(buf[3])
						{
   case 11:
   case 24:
   case 7:
   case 23:
	  break;

						case 9:
                     buf[3] = 11;
                     break;
                  case 10:
                     buf[3] = 24;
                     break;
                  case 3:
                     buf[3] = 7;
                     break;
                  case 6:
                     buf[3] = 23;
                     break;
                  default:
                     buf[3] = 0;
                     break;
						}    
      
      /* ver si es combinada */
      if (buf[11] == checo)
						{
         if (buf[3] == 24 && (QUE_SISTEMA_SOY == 11 || QUE_SISTEMA_SOY == 13))
         {
            return(0);
         }
         if (buf[3] == QUE_SISTEMA_SOY)
         {
            return(0);
         }
						}   
   }
   
   if (!codigo_de_sistema1)
   {
      *prt = 1;
      return(-1);
   }
   
   codigo_de_sistema = codigo_de_sistema1;
   o = 0;
   do {
      *prt = 0;
      strcpy(tmp,codigo_de_sistema);
      de_new_code(tmp,(_Posicion() % 50),5);
      if (nume_serie != ((long)tmp[0] + (long)tmp[4] * 50L + (long)tmp[1] * 50L * 50L + (long)tmp[2] * 50L * 50L * 50L))
						{
         (*prt)++;
						}
      if (tmp[3] == 24 && (QUE_SISTEMA_SOY == 11 || QUE_SISTEMA_SOY == 13) && !(*prt))
         return(0);
      if (tmp[3] != QUE_SISTEMA_SOY) /* hardwired */
						{
         (*prt)++;
						}
      if(!*prt)
         return(0);
      switch(o)
						{
						case 0:
                     if (codigo_de_sistema2)
                        codigo_de_sistema = codigo_de_sistema2;
                     else
                        o = 3;
                     break;
                  case 1:
                     if (codigo_de_sistema3)
                        codigo_de_sistema = codigo_de_sistema3;
                     else
                        o = 3;
                     break;
                  case 2:
                     if (codigo_de_sistema4)
                        codigo_de_sistema = codigo_de_sistema4;
                     else
                        o = 3;
                     break;
                  default:
                     break;
						}
      o++;
   } while(o < 4);
   
   return(-1);
}



#ifdef RWXARXA
#ifdef RWSERVER

int ver_si_es_servidor()
{
   unsigned char tmp[81];
   unsigned char buf[81];
   int a,b,c,d,e,s;
   unsigned short checo;
   unsigned short __posicion;
   unsigned long __nume_serie;
   if (!en_conexion_externa) return(0);
   
   if (act_key && strcmp(act_key,"DEMO"))
   {
      strcpy(buf,act_key);
      de_new_code(buf,99,12);
      __posicion = buf[10] + buf[8] * 50;
      __nume_serie = buf[0] + buf[4] * 50L + buf[5] * 50L * 50L + buf[6] * 50L * 50L * 50L;
      if (__nume_serie < 0L || __nume_serie > 999999L || __posicion > 1022 || __posicion < 0) return(0);
   }
   
   
   if (!act_servidor && act_key)  
   {  
      if (!strcmp(act_key,"DEMO"))
      {     
         return(0);
      }  
      strcpy(buf,act_key);
      de_new_code(buf,99,12);
      checo = (buf[3] ^ buf[1] ^ buf[2] ^ buf[9] ^ buf[7]) % 50;
      /* ver si es combinada */
      if (buf[11] == checo)
      {
         if (!buf[9] && !buf[7])
            return(0);
         mi_posicion = buf[10] + buf[8] * 50;
         mis_clientes[0] = buf[9];
         mis_clientes[1] = buf[7];
         mis_clientes[2] = 0;
         mis_clientes[3] = 0;
         mis_clientes[4] = 0;
         return(buf[9] + buf[7]);
      }   
   }
   
   
   if (!act_servidor)
   {
      return(0);
   }
   strcpy(tmp,act_servidor);
   de_new_code(tmp,13,12);
   a = tmp[0] + tmp[1] * 50;
   b = tmp[3] + tmp[4] * 50;
   c = tmp[5] + tmp[6] * 50;
   d = tmp[8] + tmp[9] * 50; 
   e = tmp[10] + tmp[11] * 50;
   s = tmp[12] + tmp[13] * 50;
   mi_posicion = tmp[2] * 50 + tmp[7];
   if (s != (a+b+c+d))
   {
      return(0);
   }
   if (!s)
   {
      return(0);
   }
   mis_clientes[0] = a;
   mis_clientes[1] = b;
   mis_clientes[2] = c;
   mis_clientes[3] = d;
   mis_clientes[4] = e;
   return(s);
}
#endif
#endif
