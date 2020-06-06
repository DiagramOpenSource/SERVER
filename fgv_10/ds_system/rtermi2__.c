/*---------------------------------------------------------------------------
* Programa ..: rtermi2.c parte de rtermin que puede ir en overlays
* Fecha .....:
* Version ...: 4.00 UNIFICADA por  Francisco Gonzalez Valiente
* Notas .....: Rutinas de tratamiento de terminales y ventanas.
* Contenido .: VENT *w_ini();
*              void w_refresca(), w_fin(), w_salva(), w_pinta(), w_mueve(),
*                   w_putc(), w_cls(), w_ll(), cursor(), cls(), ll(), pito(),
*                   Ar(), Cr(), Ai(), Bi(), Ap(), Cp(), As(), Cs(), Gon(),
*                   Goff(), Graf(), atributo(), Terinici(), Terfinal(), Bl(),
*              int  w_saca(), w_mete(), w_lee(), w_print(), w_printd(),
*                   w_prints(), w_printc(), w_printl(), w_printF(), v_print(),
*                   v_printd(), v_prints(), v_printc(), v_printl(),
*                   v_printF(), pprint(), leech(), leecar(), curon(),
*                   pantalla(),  w_graba(),
*                   w_cambia(),w_marco(),v_scroll()
*---------------------------------------------------------------------------*/

#ifdef _DEBUG
#pragma check_pointer( off )
#endif


#include <fgvsys.h>

#include <stdio.h>
#ifdef FGVWIN
#include <windows.h>
#include <windowsx.h>
#endif

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef ESDOS
#include <io.h>
#endif

#ifndef O_BINARY  /* compatibilidad xenix - dos */
#define O_BINARY 0
#define SEEK_SET 0
#define SEEK_CUR 1
#ifndef LARGESEEK
extern long lseek();
#endif
extern unsigned int read();
#endif

#include <string.h>
#ifdef ESDOS
#include <conio.h>
#define __MSC
#include <dos.h>
#endif
#include <errno.h>
#include <malloc.h>
#include <memory.h>
#include <signal.h>
#include <sys/locking.h>
#include <rwmalloc.h>
#include <rtermin.h>
#include <teclas.h>

#ifdef FGVWIN
#include <FGVWIN.h>
extern HACCEL agi_acelerador;
extern HACCEL orig_acelerador;
#endif

#ifdef FGVDOS
extern unsigned long nt_version;
#endif


#ifndef ESDOS
extern char *ttyname();
extern void alarma_sig();
extern char *nombreter();
#endif


#ifdef FGV32
#ifndef FGVWIN
#ifndef WATCOMNLM
HANDLE myconsola;
DWORD dwOutputMode;
DWORD dwInputMode;
HANDLE hStdIn;
#endif
#endif
#endif

#ifdef RWXARXA
#ifdef RWSERVER
extern int en_conexion_externa;
extern int en_conexion_externa_stop;
extern int remoto_es_sistema_grafico;
extern int leecar_server_puro;
#endif
extern int en_conexion_terminal;
#endif


#ifdef _MSC_VER
#ifndef __Seg0000
#define __Seg0000 0x0000
#endif
#ifndef __SegB000
#define __SegB000 0xB000
#endif
#ifndef __SegB800
#define __SegB800 0xB800
#endif
#endif

extern long _programada_alarma;
extern int wdesplegamenu;
int orig_wdesplegamenu;

int llama_fun_int();

char *agi_aplicacion = NULL;
char *agi_empresa = NULL;
char *agi_codi_empre = NULL;

void Gon();
void Goff();

extern char other_server[128];

extern int (*f_ayuda)();
extern char clave_help[32];

extern short _si_escrola;

extern int (*f_util)();

extern int (*avisar)();

extern int tracer_int; /* tecla de interrupcion pulsada */
extern int logger_int; /* interrucion activacion - desactivacion del logger */

extern int cuantas_tecla_timeout;
extern int que_tecla_timeout;
extern int time_tecla_timeout;
extern int duracion_tecla_timeout;

extern int data_fecha_conmutar;

extern int mimodo_debug;

/* Funcion para ejecutar una interrrucion interna agisys */
extern int (*general_int)();
extern int (*int_aviso)();

extern int f_noallowed;

extern char *poncoma(); /* Se podria incluir aqui */

/* esto estara en fichero */
extern struct tit_tecla {
   char *r;
   char *t;
   char *d;
} _titu_tecla[_NUMTECLA-NUMESPEC];
extern struct tit_tecla *titu_tecla;

extern int fich_tecla;

extern int linea_25;
extern int rentra_autosal;
extern void (*rentra_pos)();
extern int rentra_edicion; /* funciones especiales de edicion de textos */
extern int rentra_atrib;
extern int rentra_sal_atrib;
extern int rentra_cc;
extern int rentra_cf;
extern int rentra_sal_cc;
extern int rentra_sal_cf;
extern int rentra_offqp;  /* 0 = modo normal de entrada de un campo */
/* -1 = entrada normal pero salida dejando colro por defecto de entrada */
/* -2 o > 0 se supone pintado bien y se sale dejando */
/*       color por defecto de entrada */
/* ( !0 es para modos de edicion de texto ) */
extern int rentra_offini;  /* para cuando la edicion es de lineas largas */
extern int rentra_insert;


extern int default_redita_atrib;
extern int default_redita_cc;
extern int default_redita_cf;


extern int redita_atrib;
extern int redita_cc;
extern int redita_cf;
extern long redita_linea;
extern int redita_col;
extern int repinta_texto;

extern int atento_imp;

extern FILE *_imp;

extern int LEERTERMINAL;
extern int DESPROTEGIDO;

extern short buf_tecla[200];

extern  unsigned char control[_NUMCONTROL][NUMCARCO+2];
extern short *_tecla[_NUMTECLA];
extern short **tecla;

extern int fcursor; /* tipo de funcion cursor 0-9 */

extern int PCcolor;
extern int PFcolor;


extern int *sitecla_;

extern int pulsa_en_espacio = -1;

extern unsigned char cod_i_graf[48];

static short extra_terinici = 0;


#define MAX_MAPARATON 500
static struct mapa_de_raton
{
   int pi;
   int pf;
   int id;
   int estado;
} **mapa_raton = NULL;


#ifndef ESDOS
static char *def_ini_fich = "dev/default.ini";
static char *def_ini_str  = "stty raw -echo -opost ixon ixany";
static char *def_fin_str  = "stty -raw echo opost -ixon";
static char ini_str[81];
static char fin_str[81];
extern void pinta();
#else
#ifdef FGV_MAXDOS
#ifndef FGV32
extern unsigned char *direc_ppanta;
#endif
#endif
#endif

#ifdef FGVDOS
BOOL fIsTNT = FALSE;
#endif

int os_subtipo = 1;

int no_especial = 0;

/*
* pasan  pasa una parte de un string a numero entero
* p.e.  pasan("1234567",4,2) da 56
*/

static int pasan(s,a,b)
unsigned char *s;
int a,b;
{
   unsigned char *r;
   unsigned char t[82];
   int i,valor;
   
   if (strlen(s) <= a)
      return(0);   /* se sale del string */
   r=t;
   for (i=0;i<b && *(s+a+i);i++)
      *r++ = *(s+a+i);
   *r='\0';
   valor = atoi(t);
   return(valor);
}

#ifndef FGVWIN
#ifdef FGV32
#ifndef WATCOMNLM
ini_my_consola()
{
   DWORD mdwOutputMode;
   DWORD mdwInputMode;
   
   myconsola = GetStdHandle(STD_OUTPUT_HANDLE);
   /* SetFocus */
   if (myconsola == INVALID_HANDLE_VALUE)
			{
      hStdIn = INVALID_HANDLE_VALUE;
      return(-1);
			}
   
   hStdIn = GetStdHandle(STD_INPUT_HANDLE);
   
   GetConsoleMode(myconsola, &dwOutputMode);
   mdwOutputMode = dwOutputMode;
   GetConsoleMode(hStdIn, &dwInputMode);
   mdwInputMode = dwInputMode;
   
   mdwOutputMode &= ~ENABLE_PROCESSED_OUTPUT;
   mdwOutputMode &= ~ENABLE_WRAP_AT_EOL_OUTPUT;
   SetConsoleMode(myconsola, mdwOutputMode);
   
   mdwInputMode &= ~ENABLE_PROCESSED_INPUT;
   mdwInputMode &= ~ENABLE_MOUSE_INPUT;
   mdwInputMode &= ~ENABLE_WINDOW_INPUT;
   mdwInputMode &= ~ENABLE_LINE_INPUT;
   mdwInputMode &= ~ENABLE_ECHO_INPUT;
   SetConsoleMode(hStdIn, mdwInputMode);
   return(0);
}

fin_my_consola()
{
   if (myconsola == INVALID_HANDLE_VALUE) return(-1);
   SetConsoleMode(myconsola, dwOutputMode);
   SetConsoleMode(hStdIn, dwInputMode);
   myconsola = INVALID_HANDLE_VALUE;
   hStdIn = INVALID_HANDLE_VALUE;
   return(0);
}
#else
ini_my_consola()
{
   SetCtrlCharCheckMode(FALSE);
   return(0);
}
fin_my_consola()
{
   SetCtrlCharCheckMode(TRUE);
   return(0);
}
#endif
#endif
#else
ini_my_consola()
{
   
   return(0);
}
fin_my_consola()
{
   return(0);
}
#endif

#include <stdlib.h>
/*extern char *getenv();*/


/*
* Terinici   inicializa el terminal para evitar el eco e ignora las
*            senales de interrupcion
*/
void Terinici()
{
   int CURSOR=3; /* funcion cursor es el tercer campo */
   int DESDE=4;  /* primer campo que es control */
   FILE *file1;
   char texto[81];
   char linea[90];
   char *pro;
   int i,j,k,ii;
   int numero,cc;
#ifdef ESDOS
#ifdef FGV32
   HANDLE h;
   FARPROC p;
#endif
#ifndef WATCOMNLM
   DWORD dwVersion,dwWindowsMajorVersion,dwWindowsMinorVersion;
   
   dwVersion = GetVersion();
   dwWindowsMajorVersion =  (DWORD)(LOBYTE(LOWORD(dwVersion)));
   dwWindowsMinorVersion =  (DWORD)(HIBYTE(LOWORD(dwVersion)));
   
#ifndef FGVWIN
   /*
   printf("%ld  %ld  %ld\r\n",dwVersion & (~0x80000000),dwWindowsMajorVersion,dwWindowsMinorVersion);
   getchar();
   */
#endif
   
   
   // Get build numbers for Windows NT or Win32s
   if (dwVersion < 0x80000000)                // Windows NT
			{
      /* dwBuild = (DWORD)(HIWORD(dwVersion)); */
      os_subtipo = 1;
			}
   else if (dwWindowsMajorVersion < 4)        // Win32s
			{
      /* dwBuild = (DWORD)(HIWORD(dwVersion) & ~0x8000); */
      os_subtipo = 3;	
			}
   else
			{         // Windows 95 -- No build numbers provided
      /* dwBuild =  0; */
      os_subtipo = 2;
      if (dwWindowsMajorVersion > 4 ||  (dwWindowsMajorVersion == 4 && dwWindowsMinorVersion > 0))
         no_especial = 2;
			}
   
#ifndef FGVWIN
   ini_my_consola();
   hay_raton = 1;
   h = GetModuleHandle("kernel32.dll");
   if (h == NULL)
			{    	
      fIsTNT = FALSE;
			}
   else
			{
      p = GetProcAddress(h, "IsTNT");
      if (p == NULL)
      {
         fIsTNT = FALSE;
      }
      else
      {
         fIsTNT = (*p)();
         if (fIsTNT == TRUE)
            os_subtipo = 2;
      }
			}
#else
   fIsTNT = FALSE;
#endif
#endif
   
#ifndef FGVWIN
#ifndef FGV32
   union REGS inregs,outregs;
#ifdef FGV_MAXDOS
   direc_ppanta = MK_FP(__Seg0000,0x410);
   if ( !((*direc_ppanta) & 0x30) )
			{
      direc_ppanta = MK_FP(__SegB000,0);
			}
   else
			{
      direc_ppanta = MK_FP(__SegB800,0);
			}
#endif
   
   inregs.x.ax = 0x0;
   int86(0x33,&inregs,&inregs);
   if (inregs.x.ax == 0xffff)
			{
      hay_raton = 1;
			}
#endif
#else
   hay_raton = 1;
#endif
   
#else
   int FINICI= 2; /* fichero de inicializacion */
   char l_ini[81];
   char ini_fich[51];
#endif
   
#ifdef FGVDOS
   nt_version = GetVersion();
#endif
   
   if (getenv("NOTECLADOW95"))
      no_especial = 1;
   
   inicio_fgv();
   
   if ((stdpan = w_ini(-1,-1)) == NULW) exit(1);
   w_cls(stdpan);
   buf_tecla[0] = -1;
   Pcursor = 0;
   Pgrafi = ' ';
   Patrib = ' ';       /* cambios para MS DOS */
   
   /*******/
   
   /* leer fichero y asignar valores por defecto */
   
   mimodo_debug = 0;
   pro = getenv("AGIDEBUG");
   if (pro) mimodo_debug = atoi(pro);
   
   linea_25 = 1;
   
#ifdef ESDOS
#ifdef FGVWIN
   strcpy(texto,"dev/ter/");
   pro = getenv("AGITERW");
   if (pro && strlen(pro))
      strcat(texto,pro);
   else
      strcat(texto,"pc.tnw"); /* cambios para MS DOS */
#else
   strcpy(texto,"dev/ter/");
   pro = getenv("AGITER");
   if (pro && strlen(pro))
      strcat(texto,pro);
   else
      strcat(texto,"pc.tnv"); /* cambios para MS DOS */
#endif
#else
   pro = getenv("LINEAS24");
   if (pro)
      linea_25 = 0;
   nombreter(texto);
#endif
   
#ifndef ESDOS
   strcpy(ini_fich,rw_directorio(def_ini_fich));
#endif
   
   if ((file1=fopen(rw_directorio(texto),"r")) == (FILE *)0) {
#ifndef ESDOS
      file1=fopen(ini_fich,"r");
#endif
			}
   
   
   if (file1 != (FILE *)0) {
      i=0;
      while (fgets(linea,82,file1) != NULL) {
         quitan(linea);  /* quitar el linefeed del final */
         quitab(linea);
         
         if (!i)
         {
            j = strlen(linea);
            if (j > 5)
            {
               linea[5] = 0;
               linea[7] = 0;
               _si_escrola = atoi(linea+6);
            }
            if (j > 7)
            {
               linea[9] = 0;
               linea_25 = atoi(linea+8);
            }
            extra_terinici = atoi(linea);
            if (extra_terinici > 0)
            {
               NUMTECLA = _NUMTECLA + extra_terinici;
               FUNCIONES = _FUNCIONES + extra_terinici;
               titu_tecla = (struct tit_tecla *)malloc(sizeof(struct tit_tecla) * (NUMTECLA-NUMESPEC));
               memcpy((char *)titu_tecla,(char *)_titu_tecla,sizeof(struct tit_tecla) * (_NUMTECLA-NUMESPEC));
               for (j = 0;j < extra_terinici;j++)
               {
                  titu_tecla[_NUMTECLA-NUMESPEC+j].r = NULL;
                  titu_tecla[_NUMTECLA-NUMESPEC+j].t = "Extra funcion";
                  titu_tecla[_NUMTECLA-NUMESPEC+j].d = NULL;
               }
               tecla = (short **)malloc(sizeof(short *) * NUMTECLA);
               memcpy((char *)tecla,(char *)_tecla,sizeof(short *) * _NUMTECLA);
               
               for (j = 0;j < extra_terinici;j++)
               {
                  tecla[_NUMTECLA+j] = (short *)malloc(sizeof(short) * (NUMCARTE+1));
               }
               for (j = NUMESPEC-1;j >= 0;j--)
               {
                  memcpy((char *)tecla[NUMTECLA-NUMESPEC+j],tecla[_NUMTECLA-NUMESPEC+j],sizeof(short)*(NUMCARTE+1));
               }
               for (j = 0;j < extra_terinici;j++)
               {
                  tecla[_NUMTECLA-NUMESPEC+j][0] = 999;
                  tecla[_NUMTECLA-NUMESPEC+j][NUMCARTE] = FUNCI_20 + j + 1;
               }
            }
            else
               extra_terinici = 0;
         }
         
#ifndef ESDOS
         if (i == FINICI)
            strcpy(ini_fich,rw_directorio(linea));
         else
#endif
            if (i == CURSOR)
               fcursor = atoi(linea);  /* tipo de funcion cursor */
            else  
               if (i >= DESDE && i < (DESDE+_NUMCONTROL))
               {
                  ii = i - DESDE;
                  for (j=0;j<NUMCARCO;j++) /* secuencias de pantalla */
                  {
                     control[ii][j] = (unsigned char) pasan(linea,j*3,3);
                  }
                  control[ii][NUMCARCO] = '\0';
               }
               else
                  if (i >= (DESDE+_NUMCONTROL) && i < (DESDE+_NUMCONTROL+_NUMTECLA)) {
                     /* las teclas especiales van al final */
                     if ( i < (DESDE+_NUMCONTROL+_NUMTECLA-NUMESPEC) )
                     {
                        k = i-DESDE-_NUMCONTROL;
                        tecla[k][NUMCARTE] = 801+i-DESDE-_NUMCONTROL;				   
                     }
                     else
                     {
                        k = NUMTECLA + i -DESDE-_NUMCONTROL-_NUMTECLA;
                     }
                     for (j=0;j<NUMCARTE;j++) /* secuencias de teclado */
                        tecla[k][j] = pasan(linea,j*3,3);
                  }
                  else
                     if (i >= (DESDE+_NUMCONTROL+_NUMTECLA) && i < (DESDE+_NUMCONTROL+_NUMTECLA*2-NUMESPEC)) {
                        /* descripcion de las teclas segun teclado usado no estan las especiales */
                        titu_tecla[i-DESDE-_NUMCONTROL-_NUMTECLA].r = malloc(strlen(linea)+1);
                        strcpy(titu_tecla[i-DESDE-_NUMCONTROL-_NUMTECLA].r,linea);
                     }
                     else
                        if (i >= (DESDE+_NUMCONTROL+_NUMTECLA*2-NUMESPEC) && i < (DESDE+_NUMCONTROL+_NUMTECLA*2+_FUNCIONES-NUMESPEC)) {
                           /* programacion de las teclas programables */
                           tecla_funcion(i-DESDE-_NUMCONTROL-_NUMTECLA*2+NUMESPEC,linea,2);
                        }
                        else
                           if (i >= (DESDE+_NUMCONTROL+_NUMTECLA*2+_FUNCIONES-NUMESPEC) &&
                              i < ((DESDE+_NUMCONTROL+_NUMTECLA*2+_FUNCIONES-NUMESPEC) + extra_terinici*3))
                           {
                              j = i - (DESDE+_NUMCONTROL+_NUMTECLA*2+_FUNCIONES-NUMESPEC);
                              k = _NUMTECLA - NUMESPEC + j/3;
                              switch( (j%3) )
                              {
                              case 0:
                                 for (j=0;j<NUMCARTE;j++) /* secuencias de teclado */
                                    tecla[k][j] = pasan(linea,j*3,3);
                                 break;
                              case 1:
                                 titu_tecla[k].r = malloc(strlen(linea)+1);
                                 strcpy(titu_tecla[k].r,linea);
                                 break;
                              case 2:
                                 tecla_funcion(_FUNCIONES+j/3,linea,2);
                                 break;
                              default:
                                 break;
                              }
                           }
                           i++;
      }
      fclose(file1);
   }
   
   
   
   /* INTERCAMBIO DE F1 POR F12 !!!!!!*/
   
   tecla[21][NUMCARTE] = 823;
   tecla[22][NUMCARTE] = 822;
   /***********************************/
   
   
   /****************/
   /****************/
#ifndef ESDOS
   if (!LEERTERMINAL) {
      l_ini[0] = 0;
      strcpy(ini_str,def_ini_str);
      strcpy(fin_str,def_fin_str);
      if ((file1=fopen(ini_fich,"r")) != (FILE *)0) {
         if (fgets(linea,80,file1) != NULL) {
            quitan(linea);  /* quitar el linefeed del final */
            quitab(linea);
            strcpy(ini_str,linea);
            if (fgets(linea,80,file1) != NULL) {
               quitan(linea);  /* quitar el linefeed del final */
               quitab(linea);
               strcpy(fin_str,linea);
               if (fgets(linea,80,file1) != NULL) {
                  quitan(linea);  /* quitar el linefeed del final */
                  quitab(linea);
                  strcpy(l_ini,linea);
               }
            }
         }
         fclose(file1);
      }
      if (isatty(fileno(stdout)))
         agi_system(ini_str);
   }
#endif
   
   curon(0);
#ifndef ESDOS
   if (!LEERTERMINAL) {
      pinta(control[INICIOPAN],NUMCARCO,0); /* inicializar la pantalla */
      if (atento_imp) i_acabo(_imp);
      for (j=0;j<strlen(l_ini);j += 3)
         putchar((char) pasan(l_ini,j,3));
      if (atento_imp) i_inicio(_imp);
   }
#endif
   
   Prefresco = CIERTO;
   
   if (DESPROTEGIDO == 10) { /* sistema de control por fichero */
      if ( (fich_tecla = rw_open_("teclas",O_BINARY | O_RDWR)) == -1)
         DESPROTEGIDO = 0;
   }
   if (DESPROTEGIDO == 20) {
      if ( (fich_tecla = rw_open_("teclas",O_CREAT | O_TRUNC | O_BINARY | O_RDWR,
         S_IREAD | S_IWRITE)) == -1)
         DESPROTEGIDO = 0;
   }
   Nor();
#ifdef ESDOS
#ifndef FGVWIN
#ifndef FGV32
   inregs.x.ax = 0x0b00;
   inregs.x.bx = ((dos_atri() & 0x70) >> 4);
   int86(0x10,&inregs,&outregs); /* set border color */
#endif
#endif
#endif
   
   sitecla_ = (int *)malloc(NUMTECLA*sizeof(int));
   
#ifdef SACALOBIEN
   file1 = fopen("default.agi","wt");
   if (file1)
   {
      fprintf(file1,"unsigned char control[_NUMCONTROL][NUMCARCO+2]={\n"); 
      for (i = 0;i < _NUMCONTROL;i++)
      {	 
         fprintf(file1,"\"");
         for (j = 0;j < (NUMCARCO+1);j++)
         {
            fprintf(file1,"\\%o",control[i][j]);       
         }
         fprintf(file1,"\",\n");
      }
      fprintf(file1,"\n};\n\n\n");
      for (i = 0;i < _NUMTECLA;i++)
      {
         fprintf(file1,"static short _tecla%d[]={\n%d,%d,%d,%d,%d,%d,%d,%d,%d,\n};\n",i+1,
            _tecla[i][0],_tecla[i][1],_tecla[i][2],_tecla[i][3],
            _tecla[i][4],_tecla[i][5],_tecla[i][6],_tecla[i][7],
            _tecla[i][8]);
      }
      fclose(file1);
   }
#endif
   
   if (getenv("AUTH_DIAGRAM"))
      strcpy(other_server,getenv("AUTH_DIAGRAM"));
}

/*
* Terfinal   deja el terminal en su forma original
*/
void Terfinal()
{
   int i;
   FILE *f,tmp;
   
   
   if (stdpan == NULL) return;
   
   
   borra_mensaje_cabecera(-1); /* por si se ha quedado */
   
   free(sitecla_);
   sitecla_ = NULL;
   v_fin();
   curon(1);
   stdpan = NULL;
   
   if (DESPROTEGIDO > 9) rw_close_(fich_tecla);
#ifndef FGVWIN
#ifdef FGV32
   fin_my_consola();
#endif
#endif
   
#ifndef ESDOS
   if (!LEERTERMINAL) {
      pinta(control[FINALPAN],NUMCARCO,0);
      if (isatty(fileno(stdout)))
         agi_system(fin_str);
   }
#endif
   
   for (i = 0;i < (NUMTECLA-NUMESPEC);i++)
   {
      if (titu_tecla[i].r)
      {
         free(titu_tecla[i].r);
         titu_tecla[i].r = NULL;
      }
   }
   
   for (i = 0;i < FUNCIONES;i++) {
      if (titu_tecla[PRIMERAF+i].d) {
         free(titu_tecla[PRIMERAF+i].d);
         titu_tecla[PRIMERAF+i].d = NULL;
      }
   }
   
   if (extra_terinici)
   {
      memcpy((char *)_titu_tecla,(char *)titu_tecla,sizeof(struct tit_tecla) * (_NUMTECLA-NUMESPEC));
      for (i = 0;i < extra_terinici;i++)
      {
         free((char *)tecla[_NUMTECLA+i]);
      }
      free((char *)tecla);
      tecla = _tecla;
      extra_terinici = 0;
      free((char *)titu_tecla);
      titu_tecla = _titu_tecla;
      NUMTECLA = _NUMTECLA;
      FUNCIONES = _FUNCIONES;
   }
   fin_fgv();
#ifdef RWXARXA
#ifdef RWSERVER
   if (!en_conexion_externa && !en_conexion_terminal)
#else
      if (!en_conexion_terminal)
#endif
         libera_x_buffers();
#endif
}

/** ATENCION SE LLAMA A LA RUTINA DE ENTRADA DE DATOS !!! **/
int ayuda_teclas()
{
   VENT *w;
   char *t;
   int i,p,c,j,cc,i1,i2,qj;
   static int activo = 0;
   
   if (activo || !stdpan) return(0);
   activo = 1;
   
   f_noallowed = 1;
   w = w_ini(501,2380);
   w_salva(w);
   Ai();cuadro(501,2380);Bi();
   Ar();
   zonablan(602,2279);
   pprint(611,"CONFIGURACION ACTUAL DE LAS TECLAS ESPECIALES");
   Cr();
   Ai();
   pprint(702,"FUNCION");
   pprint(721,"TECLA");
   pprint(732,"DESCRIPCION/PROGRAMACION");
   Bi();
   Ar();
   
   cc = 0;
   for (i = 0,p = 802;i < (NUMTECLA-NUMESPEC);i++) { /* no estan incluidas las teclas de conversion */
      pprint(p+4,"..............:");
      pprint(p,titu_tecla[i].t); /* quiza mejor hacerlo con una pantalla externa */
      cursor(p+19);
      t = titu_tecla[i].r;
      if (!t) {
         t = "";
      }
      v_prints("%-10.10s",t);
      
      cursor(p+30);
      t = titu_tecla[i].d;
      if (!t) {
         t = "";
      }
      v_prints("%-48.48s",t);
      
      p += 100;
      cc++;
      if (p > 2102 || i == (NUMTECLA-NUMESPEC-1)) { /* final de pantalla o final de teclas */
         for(;p < 2103;p += 100)
            zonablan(p,p+78);
         Cr();
         Ai();pprint(2202,"Pulse una tecla");Bi();
         p = 802;
         c = leecar();
         
         if (c == ESC || c == CTRLC || c == ABORTAR)
            break;
         else
            if (c == CRETURN || c == VALIDAR) {
               break;
               i1 = i - cc + 1;
               i2 = i;
               if (i2 >= PRIMERAF) {
                  if (i1 < PRIMERAF)
                     i1 = PRIMERAF;
                  j = i1;
                  t = malloc(258);
                  while(-1) {
                     if (titu_tecla[j].d)
                        strcpy(t,titu_tecla[j].d);
                     else
                        *t = 0;
                     rentra_sal_atrib = 'R';
                     rentra_atrib = 'I';
                     qj = entra(832 + (j - (i - cc + 1)) * 100,255,"","",t,481,t);
                     if (qj == 7) break;
                     quitab(t);
                     if (titu_tecla[j].d) {
                        free(titu_tecla[j].d);
                     }
                     titu_tecla[j].d = malloc(strlen(t)+1);
                     strcpy(titu_tecla[j].d,t);
                     if (qj == 2 && j > i1)
                        j--;
                     else
                        if ((qj == 0 || qj == 3) && j < i2)
                           j++;
                        else
                           if (qj == 4) {c = PGUP;break;}
                           else
                              if (qj == 5) {c = PGDN;break;}
                              else
                                 if (qj == 1) break;
                  }
                  free(t);
               }
            }
            Ar();
            
            if (c == ARRIBA || c == PGUP) {
               if (i > (cc*2))
                  i -= (cc*2);
               else
                  i = -1;
            }
            
            cc = 0;
            pprint(2202,"               ");
      }
   }
   Cr();
   w_pinta(w);
   w_fin(w);
   activo = 0;
   f_noallowed = 0;
   return(0);
}

int pon_mapa_raton(pi,pf,id,estado)
int pi,pf,id,estado;
{
   int i;
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      pon_mapa_raton_terminal(pi,pf,id,estado);
   }
#endif
#endif
   if (!mapa_raton)
   {
      mapa_raton = (struct mapa_de_raton **)malloc(sizeof(struct mapa_de_raton *)*MAX_MAPARATON);
      for (i = 0;i < MAX_MAPARATON;i++)
         mapa_raton[i] = NULL;
   }
   for (i = 0;i < MAX_MAPARATON;i++)
   {
      if (!mapa_raton[i]) break;
   }
   if (i >= MAX_MAPARATON)
   {
      aviso(0,"NO HAY MAS PARA RATON");
      return(-1);
   }
   mapa_raton[i] = (struct mapa_de_raton *)malloc(sizeof(struct mapa_de_raton ));
   mapa_raton[i]->pi = pi;
   mapa_raton[i]->pf = pf;
   mapa_raton[i]->id = id;
   mapa_raton[i]->estado = estado;
   return(i);
}

int anula_mapa_raton(id)
int id;
{
   int encontrados = 0;
   int i;
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
			{
      anula_mapa_raton_terminal(id);
			}
#endif
#endif
   
   if (mapa_raton)
			{
      for (i = 0;i < MAX_MAPARATON;i++)
      {
         if (mapa_raton[i])
         {
            if (id == -1 || mapa_raton[i]->id == id)
            {
               free(mapa_raton[i]);
               mapa_raton[i] = NULL;
            }
            else
               encontrados++;
         }
      }
      if (!encontrados)
      {
         free(mapa_raton);
         mapa_raton = NULL;
      }
			}
   return(0);
}


int esta_en_el_raton(p,estado)
int p;
int *estado;
{
			int i;
         if (mapa_raton)
         {
            for (i = 0;i < MAX_MAPARATON;i++)
            {
               if (mapa_raton[i] && mapa_raton[i]->pi <= p && mapa_raton[i]->pf >= p)
               {
                  if (estado)
                     *estado = mapa_raton[i]->estado;
                  return(mapa_raton[i]->id);
               }
            }
         }
         return(-1);
}


static int raton_recur = 0;
static struct mapa_de_raton **recur_raton[100];
int recurre_raton(m)
int m;
{
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      recurre_raton_terminal(m);
   }
#endif
#endif
   if (!m)
   {
      if (raton_recur < 100)
						{
         recur_raton[raton_recur] = mapa_raton;
         raton_recur++;
         mapa_raton = NULL;
						}
   }
   else
   {
      if (raton_recur)
						{
         raton_recur--;
         mapa_raton = recur_raton[raton_recur];
						}
   }
   return(0);
}


/* llama_fun_int : llama a una funcion f_f con el parametro par_f
salvando las posiciones de cursor atributo etc */
int llama_fun_int(f_f,par_f)
int (*f_f)();
char *par_f;
{
   int i,j,d,c,cu,n,p,pc,pf;
   void (*atp)() = rentra_pos;
   int ata = rentra_edicion;
   int atb = rentra_offqp;
   int atc = rentra_insert;
   int atd = rentra_offini;
   int att = rentra_atrib;
   int acc = rentra_cc;
   int acf = rentra_cf;
   int atts = rentra_sal_atrib;
   int accs = rentra_sal_cc;
   int acfs = rentra_sal_cf;
   int aeta = redita_atrib;
   int aecc = redita_cc;
   int aecf = redita_cf;
   int amod_val = modo_de_validacion;
   int _data_fecha_conmutar = data_fecha_conmutar;
   struct mapa_de_raton **omaparat = mapa_raton;
   char *_agi_aplicacion = agi_aplicacion;
   char *_agi_empresa = agi_empresa;
   char *_agi_codi_empre = agi_codi_empre;
   int _pulsa_en_espacio = pulsa_en_espacio;
   int _wdesplegamenu = wdesplegamenu;
#ifdef RWXARXA
#ifdef RWSERVER
   int _ant_server_puro = leecar_server_puro;
#endif
#endif
#ifdef FGVWIN
   
   char w_tmp[81];
   int o_en_menu = en_menu;
   HACCEL _agi_acelerador = agi_acelerador;

   if (DespachadorExterno)
   {
        (*DespachadorExterno)((int)DESPACHO_SOBRE_PANTALLA,(int)1,NULL);   
   }
   
   agi_acelerador = orig_acelerador;
   GetWindowText(principal_w,w_tmp,80);
   /* SetWindowText(principal_w,""); */

   en_menu = 0;
#endif
#ifdef RWXARXA
#ifdef RWSERVER
   leecar_server_puro = 0;
#endif
#endif
   
   data_fecha_conmutar = 0;
   
   wdesplegamenu = orig_wdesplegamenu;
   
   pulsa_en_espacio = -1;
   
   recurre_boton(0);
   
   
   recurre_raton(0);
   
   agi_aplicacion = NULL;
   agi_empresa = NULL;
   agi_codi_empre = NULL;
   
   
   /*
   long aeli = redita_linea;
   int aecl = redita_col;
   int aert = repinta_texto;
   redita_linea = 0L;
   redita_col = 0;
   repinta_texto = 0;
   redita_linea = aeli;
   redita_col = aecl;
   repinta_texto = aert;
   */
   
   mapa_raton = NULL;
   modo_de_validacion = CRETURN;
   rentra_pos = NULL;
   rentra_edicion = 0; /* funciones especiales de edicion de textos */
   rentra_atrib = 'R';
   rentra_sal_atrib = 'I';
   rentra_cc = 0;
   rentra_cf = 0;
   rentra_sal_cc = 0;
   rentra_sal_cf = 0;
   rentra_offqp = 0;  /* 0 = modo normal de entrada de un campo */
   /* -1 = entrada normal pero salida dejando colro por defecto de entrada */
   /* -2 o > 0 se supone pintado bien y se sale dejando
   color por defecto de entrada */
   /* ( !0 es para modos de edicion de texto ) */
   rentra_offini= 0;  /* para cuando la edicion es de lineas largas */
   rentra_insert = 0;
   
   redita_atrib = default_redita_atrib;
   redita_cc    = default_redita_cc;
   redita_cf    = default_redita_cf;
   
   
   j = (int)Patrib;
   pc = PCcolor;
   pf = PFcolor;
   c = (Pcursor / 80) * 100 + Pcursor % 80 + 101;
   cu= Pcursor;
   i = curon(-1);
   if ((p = Pgrafi) != ' ')
						Goff();
   curon(0);
   if ((int)(Patrib & (~128)) > 64 && (int)(Patrib & (~128)) < 91) {
						atributo((char)((int)(Patrib & (~128)) + 32)); /* Cerrar anterior atributo */
   }
   n = (*f_f)(par_f);
   if (p != ' ')
						Gon();
   if (j != Patrib || pc != PCcolor || pf != PFcolor) {
						Color(pc,pf);
                  atributo((char)j);
                  Patrib = j;
   }
   if (c != cu)
						cursor(c);
   if (curon(-1) != i)
						curon(i);
   
   
   pulsa_en_espacio = _pulsa_en_espacio;
   data_fecha_conmutar = _data_fecha_conmutar;
   rentra_pos = atp;
   rentra_edicion = ata;
   rentra_offqp = atb;
   rentra_insert = atc;
   rentra_offini = atd;
   rentra_atrib = att;
   rentra_cc = acc;
   rentra_cf = acf;
   rentra_sal_atrib = atts;
   rentra_sal_cc = accs;
   rentra_sal_cf = acfs;
   redita_atrib = aeta;
   redita_cc = aecc;
   redita_cf = aecf;
   modo_de_validacion = amod_val;
   mapa_raton = omaparat;
   agi_aplicacion = _agi_aplicacion;
   agi_empresa = _agi_empresa;
   agi_codi_empre = _agi_codi_empre;
   wdesplegamenu = _wdesplegamenu;
   recurre_raton(1);
#ifdef FGVWIN
   agi_acelerador = _agi_acelerador;
   SetWindowText(principal_w,w_tmp);
   en_menu = o_en_menu;

   if (DespachadorExterno)
   {
		(*DespachadorExterno)((int)DESPACHO_REPON_PANTALLA,(int)1,NULL);   
   }

#endif
   recurre_boton(1);
   
#ifdef RWXARXA
#ifdef RWSERVER
   leecar_server_puro = _ant_server_puro;
#endif
#endif
   
   return(n);
}


char **salva_funciones(ini,fin)
int ini,fin;
{
   int i = fin - ini + 1,j;
   char **funs;
   if (fin == -1) /* todas */
   {
      fin = FUNCIONES - 1;
      i = FUNCIONES - ini;
   }
   funs = (char **)malloc(sizeof(char *) * i);
   for (j = ini,i = 0;j <= fin;j++,i++) {
      funs[i] = titu_tecla[j+PRIMERAF].d;
      titu_tecla[j+PRIMERAF].d = NULL;
   }
   return(funs);
}

int repon_funciones(funs,ini,fin)
char **funs;
int ini,fin;
{
   int i = fin - ini + 1,j;
   if (fin == -1) /* todas */
			{
      fin = FUNCIONES - 1;
      i = FUNCIONES - ini;
			}
   for (j = ini,i = 0;j <= fin;j++,i++) {
      titu_tecla[j+PRIMERAF].d = funs[i];
			}
   free((char *)funs);
}

int __w__lee_(w,p)
VENT *w;
char *p;                   /* nombre del fichero de pantalla */
{
			unsigned char texto[82];
         unsigned char *linea;
         FILE *file1;
         int i,j,k=0;
         int f;
         int alto  = (w->vfin)/100 - (w->vini)/100 + 1;
         int ancho = (w->vfin)%100 - (w->vini)%100 + 1;
         int tam = alto*ancho;
         short ppp;
         unsigned char *wvv = w->vv;
         
         
         /* ------ leer el fichero ------------------------------*/
         if ((file1=fopen(p,"r")) == (FILE *) 0)
            return(-1);
         /* REALIZAR CONVERSION A FORMATO NUEVO */
         for (i=0;i<3;i++) {
            for (j=0;j<23;j++) {
               if ((linea=fgets(texto,82,file1)) == NULL) continue;
               quitan(linea);
               if (j >= alto) continue;
               if (i != 1)
                  memcpy((wvv)+k,linea,ancho);
               else {
                  for(f = 0;linea[f];f++) {
                     if (linea[f] >= 'a') {
                        *(wvv + k - tam + f) = cod_i_graf[linea[f]-'a'];
                     }
                  }
               }
               k += ancho;
            }
            if (i != 1 && alto == 24) {
               memset((wvv)+k,' ',ancho);
               k += ancho;
            }
         }
         memset(wvv+tam,128,tam);
         fclose(file1);
         return(0);
}



int cambia_s_req(char *tmp2,int modo)
{
   if (!modo)
   {
      while (*tmp2)
						{ 
         if (*tmp2 == '(' || *tmp2 == ')') *tmp2 = '-';
         tmp2++;
						}
   }
   else
   {
      if (*tmp2 == '@' && *(tmp2+1) == '-')
						{
         tmp2++;
         *tmp2 = '(';
         tmp2++;
         while (*tmp2)
         {
            tmp2++;
         }
         while (*tmp2 != '(')
         {
            if (*tmp2 == '-')
            {
               *tmp2 = ')';
               break;
            }
            tmp2--;
         }
						}
   }
   return(0);
}

