
/* Autor: Francisco Gonzalez Valiente */
#include <fgvsys.h>


#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef O_BINARY
#include <io.h>
#endif

#ifndef O_BINARY  /* compatibilidad xenix - dos */

#define O_BINARY 0
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#ifndef LARGESEEK
extern long rw_lseek_();
#endif
extern unsigned int rw_read_();
#endif

#include <string.h>
#include <time.h>
#include <sys/timeb.h>
#include <sys/locking.h>

#define _NUEVODEF
#include <rdef.h>
#include <rfecha.h>

extern char clave_help[32];

#ifdef FGV32
#ifndef FGVWIN
#ifndef WATCOMNLM
extern HANDLE myconsola;
extern HANDLE hStdIn;
#endif
#endif
#endif
#ifdef RWXARXA
#ifdef RWSERVER
extern int en_conexion_externa;
extern int en_conexion_externa_stop;
#endif
extern int en_conexion_terminal;
#endif


extern int resta_dias_log;

extern char nomapli[25];
extern char modulo[9];

static int flog = -1;


extern char rw_user[11];
extern int user_who;
extern int subnumero_de_usuario;


extern long _e_log_open;
extern long _e_log_close;
extern long _e_log_read;
extern long _e_log_update;
extern long _e_log_write;
extern long _e_log_delete;


extern long _g_log_open = 0L;
extern long _g_log_close = 0L;
extern long _g_log_read = 0L;
extern long _g_log_update = 0L;
extern long _g_log_write = 0L;
extern long _g_log_delete = 0L;


/* 8 registro 8 fecha 6 hora 10 user 2 id user 2 subuser 2 operacion
8 aplicacion 8 modulo 30 descripcion 8 aperturas 8 cierres 8 lecturas 8 actualizaciones
8 nuevas 8 borrados */
static short ficha_log[] = {
   8,
      8,
      6,
      10,
      2,
      2,
      2,
      8,
      8,
      30,
      8,
      8,
      8,
      8,
      8,
      8,
      0
};

static int recu_esta = 0;
#define MAXRECUESTA 15
static long gua_esta[MAXRECUESTA][6];


int reset_g_estadistica()
{
   _g_log_open = 0L;
   _g_log_close = 0L;
   _g_log_read = 0L;
   _g_log_update = 0L;
   _g_log_write = 0L;
   _g_log_delete = 0L;
   return(0);
}


int reset_e_estadistica()
{
   _e_log_open = 0L;
   _e_log_close = 0L;
   _e_log_read = 0L;
   _e_log_update = 0L;
   _e_log_write = 0L;
   _e_log_delete = 0L;
   return(0);
}

int guarda_estadistica()
{
   if (recu_esta < MAXRECUESTA)
   {
      gua_esta[recu_esta][0] = _e_log_open;
      gua_esta[recu_esta][1] = _e_log_close;
      gua_esta[recu_esta][2] = _e_log_read;
      gua_esta[recu_esta][3] = _e_log_update;
      gua_esta[recu_esta][4] = _e_log_write;
      gua_esta[recu_esta][5] = _e_log_delete;
   }
   reset_e_estadistica();
   recu_esta++;
   return(0);
}

int repon_estadistica()
{
   if (!recu_esta) return(0);
   recu_esta--;
   
   _g_log_open   += _e_log_open;
   _g_log_close  += _e_log_close;
   _g_log_read   += _e_log_read;
   _g_log_update += _e_log_update;
   _g_log_write  += _e_log_write;
   _g_log_delete += _e_log_delete;
   
   if (recu_esta < MAXRECUESTA)
   {
      _e_log_open = gua_esta[recu_esta][0];
      _e_log_close= gua_esta[recu_esta][1];
      _e_log_read = gua_esta[recu_esta][2];
      _e_log_update = gua_esta[recu_esta][3];
      _e_log_write = gua_esta[recu_esta][4];
      _e_log_delete = gua_esta[recu_esta][5];
   }
   else
   {
      reset_e_estadistica();
   }
   
   return(0);
}

int guarda_incidencia(numero,descripcion)
int numero;
char *descripcion;
{
   int oflog = flog;
   char tmp[256];
   char buf[256];
   int i;
   short f_l_p[30];
   int nc;
   short tamreg;
   long posi;
   
#ifdef RWXARXA
   if (en_conexion_terminal)
      return(0);
#endif

   if (other3_server == 56443696)
	   return 0;   
   
   tamreg = 0;
   for (nc = 0;ficha_log[nc] && nc < 30;nc++)
   {
      f_l_p[nc] = tamreg;
      tamreg += ficha_log[nc];
   }
   
   if (tamreg >= 255) {aviso(0,"LenLog");return(-1);} /***/
   
   if (flog == -1)
   {
      stfecha(fecha(),tmp);
      tmp[8] = 0;
      strcpy(buf,rw_directorio("logs/"));
      strcat(buf,tmp);
      strcat(buf,".dat");
      flog = rw_open_(buf,O_RDWR | O_BINARY | O_CREAT,S_IWRITE | S_IREAD);
   }
   if (flog < 0)
   {
      return(-1);
   }
   
   for (i = 0;i < 1000;i++)
   {
      rw_lseek_(flog,RWFILEEND,SEEK_SET);
      if (!locking(flog,2,1L)) break;
      pulsatecla();
   }
   if (i >= 1000)
   {
      for (i = 990;i < 1000;i++)
      {
         rw_lseek_(flog,RWFILEEND,SEEK_SET);
         if (!locking(flog,1,1L)) break;
      }
   }
   
   if (i < 1000)
   {
      posi = rw_lseek_(flog,0L,SEEK_END);
      posi /= (tamreg+1);
      memset(buf,0,tamreg);
      buf[tamreg] = 10;
      /* registro */
      stdbl( ((double)posi) ,buf+f_l_p[0]);
      /* fecha */
      stfecha(fecha(),buf+f_l_p[1]);
      /* hora */
      memcpy(buf+f_l_p[2],fechas+11,2);
      memcpy(buf+f_l_p[2]+2,fechas+14,2);
      memcpy(buf+f_l_p[2]+4,fechas+17,2);
      /*  usuario */
      stchar(rw_user,buf+f_l_p[3],10);
      /* codigo usuario */
      stint(user_who,buf+f_l_p[4]);
      /* subcodigo de usuario */
      stint(subnumero_de_usuario,buf+f_l_p[5]);
      /* codigo de operacion */
      /* 0 = incidencia generica */
      /* 1 = entrada usuario */
      /* 2 = salida usuario */
      /* 3 = entrada opcion */
      /* 4 = salida opcion */
      stint(numero,buf+f_l_p[6]);
      /* aplicacion */
      stchar(nomapli,buf+f_l_p[7],8);
      /* modulo */
      stchar(modulo,buf+f_l_p[8],8);
      /* descripcion */
      if (descripcion)
         strcpy(tmp,descripcion);
      else
         tmp[0] = 0;
      stchar(tmp,buf+f_l_p[9],30);
      /* opens */
      stdbl( ((double)_e_log_open) ,buf+f_l_p[10]);
      /* closes */
      stdbl( ((double)_e_log_close) ,buf+f_l_p[11]);
      /* reads */
      stdbl( ((double)_e_log_read) ,buf+f_l_p[12]);
      /* updates */
      stdbl( ((double)_e_log_update) ,buf+f_l_p[13]);
      /* writes */
      stdbl( ((double)_e_log_write) ,buf+f_l_p[14]);
      /* deletes */
      stdbl( ((double)_e_log_delete) ,buf+f_l_p[15]);
      
      rw_write_(flog,buf,tamreg+1);
      
      rw_lseek_(flog,RWFILEEND,SEEK_SET);
      locking(flog,0,1L);
   }
   
   if (oflog < 0)
   {
      rw_close_(flog);
      flog = -1;
   }
   return(0);
}


int borra_incidencias()
{
   char tmp[20];  
   char buf[81];
   char fech[40];
   int resta;
   
   if (!resta_dias_log) return(0);
   resta = 0 - resta_dias_log;
   strcpy(fech,fecha());
   diasfecha(fech,resta,0,0);
   stfecha(fech,tmp);
   tmp[8] = 0;
   strcpy(buf,rw_directorio("logs/"));
   strcat(buf,"*.*");
   strcat(tmp,".dat");
   _lee_y_borra(buf,tmp,"00000000.dat");
   return(0);
}