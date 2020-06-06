/*---------------------------------------------------------------------------
* Programa ..: rsignal.c                 Autor: Francisco Gonzalez Valiente
* Fecha .....:
* Version ...:
* Notas .....: Rutinas de tratamiento de signals.
* Contenido .: int   ver_sig(),
*              void  ini_sig(), fin_sig(),
*---------------------------------------------------------------------------*/

#include <fgvsys.h>

#ifdef ESDOS
#include <dos.h>
#endif

#include <signal.h>
#include <stdio.h>

#include <rtermin.h>

#ifdef FGV32
#define MANUAL_INT
extern BOOL fIsTNT;
#endif

#ifndef SIGCLD
#define SIGCLD 0
#endif

#ifndef SIGBREAK
#define SIGBREAK 0
#endif

#ifndef SIGHUP
#define SIGHUP 0
#endif

#ifndef SIGKILL
#define SIGKILL 0
#endif

#ifndef SIGSEGV
#define SIGSEGV 0
#endif

#ifndef SIGUSR1
#define SIGUSR1     16
#endif
#ifndef SIGUSR2
#define SIGUSR2     17  /* User defined signal 2 */
#endif
#ifndef SIGUSR3
#define SIGUSR3     20  /* User defined signal 3 */
#endif

#ifndef SIGINFO
#define SIGINFO     29
#endif

extern int (*general_int)(); /* Funci¢n de interrupcion */
extern long _programada_alarma;

extern int (*_pon_incidencia)() = NULL;

#ifdef ESDOS
static int timer_activado = 0;
#endif

extern int hay_mensaje_signal;

int nsigusr1 = 0;
int nsigusr2 = 0;

int sin_signals = 0;

static int en_recuperacion = 0;
static int presenta(ss)
int *ss;
{
   int i,s = *ss;
   char buf[80];
   
   
   if (en_recuperacion < 100)
   {
      en_recuperacion++;
      return(1);
   }
   
   
   if (_pon_incidencia)
   {
      sprintf(buf,"signal trap %d",s);
      (*_pon_incidencia)(0,buf);
   }
   
   pito(59);pito(60);
   pushv(808,1676);
   zonablan(808,1676);
   Ai();
   cuadro(808,1676);
   Bi();
#ifndef FGVWIN
   Ar();
#endif
   pprint(1009,"El Sistema Operativo ha causado un error de proteccion general");
   if (1 /*s != SIGHUP && s != SIGILL && s != SIGKILL && s != SIGSEGV && s != SIGTERM*/)
   {
      v_printd(" %2d",s);
      pprint(1228,"DESEA CONTINUAR? [ ]");
      pprint(1420,"**NO CONTINUAR SI NO SE ESTA MUY SEGURO**");
      entra(1246,1,"SN","","N",0,buf);
   }
   else
   {
      buf[0] = 'N';
      pito(61);pito(62);pito(63);pito(64);pito(65);pito(66);pito(67);pito(68);
      sleep(10);
   }
#ifndef FGVWIN
   Cr();
#endif
   if (buf[0] == 'S') { /* RECUPERAR EL ERROR */
      if (_pon_incidencia)
      {
         sprintf(buf,"RECUPERADO SIGNAL %d",s);
         (*_pon_incidencia)(0,buf);
      }
      popv(0);
      return(1);
   }
   return(0);
}


void recu_sig(int s)
{
   signal(s,recu_sig); /* restaurar recuperacion */
   if (s == SIGINFO)
   {
	   hay_mensaje_signal = 1;
   }
   if (s == SIGUSR1)
   {
	   nsigusr1++;
   }
   if (s == SIGUSR2)
   {
	   nsigusr2++;
   }   
   return;
}


void ver_sig(int s)
{
   static int activa = 0;
#ifndef ESDOS
   if (s == 20)
   {
	   signal(s,ver_sig); /* restaurar recuperacion */
	   return;
   }
#endif

   
   if (!activa) {
      activa = 1;  
      if (s == SIGTERM || s == SIGHUP || s == SIGILL || s == SIGKILL) 
      {
         en_recuperacion = 100;
      }
      if (llama_fun_int(presenta,(char *)&s)) {
         activa = 0;
         signal(s,ver_sig); /* restaurar recuperacion */
         return;
      }
			}
   cierra_todo(1);
   fflush(stdout);
   Terfinal();
   exit(2);
}

#ifdef CONTROLARCTRLC

BOOL RW_HandlerRoutine(DWORD);

BOOL RW_HandlerRoutine(
                       DWORD dwCtrlType 	//  control signal type
                       )
{
   Beep(1000,1000);
   return(TRUE);
}

#endif



void ini_sig()
{
   int i;
   extern void ver_sig( int );
   extern void recu_sig( int );
   
   if (sin_signals)
	   return;
#ifdef ESDOS
   
   
   for (i=1;i<NSIG;i++) 
   {
	   if (i != SIGABRT && i != SIGFPE && i!= SIGILL && i != SIGINT && i != SIGSEGV && i != SIGTERM)
		   continue;

      if (i == SIGBREAK || i == SIGINT || i == SIGUSR1 ||
         i == SIGUSR2 || i == SIGUSR3)
         signal(i,recu_sig); /* recuperar directamente */
      else
         signal(i,ver_sig); /* problemas en general no recuperados */
			}
#ifndef FGV32
   if (timer_activado == 1) 
			{
      activa_timer();
			}
#else
   SetConsoleCtrlHandler(/*(PHANDLER_ROUTINE)RW_HandlerRoutine*/ NULL,TRUE);
#endif
#else
   for (i=1;i<NSIG;i++) {
      if (i != SIGALRM && i != SIGUSR1 && i != SIGUSR2) 
      {
         if (i != SIGCLD) {/* unix */
            signal(i,ver_sig);
         }
      }
      else
         signal(i,recu_sig);
			}
#endif
#ifdef FGV32
#ifndef FGVWIN
   if (fIsTNT)
			{
      _asm
      {
         pushad
            mov ah,33h
            mov al,1h
            mov dl,0h
            int 21h
            mov ah,33h
            mov al,2h
            mov dl,0h
            int 21h
            popad
      }
			}
#endif
#endif
#ifndef WIN32
   {
	   sigset_t ss;
	   sigemptyset (&ss);
	   sigaddset (&ss,SIGUSR1);
	   sigaddset (&ss,SIGUSR2);
       sigprocmask(SIG_BLOCK, &ss,NULL);
   }
#endif
}



void fin_sig()
{
   int i;
   extern void ver_sig();   
 
#ifndef WIN32
   
   {
	   sigset_t ss;
	   sigemptyset (&ss);
	   sigaddset (&ss,SIGUSR1);
	   sigaddset (&ss,SIGUSR2);
       sigprocmask(SIG_UNBLOCK, &ss,NULL);
   }   
#endif
   
#ifdef FGV32
   SetConsoleCtrlHandler( /*(PHANDLER_ROUTINE)RW_HandlerRoutine */ NULL,FALSE);
#endif
   for (i=1;i<NSIG;i++) 
   {
	   if (i != SIGABRT && i != SIGFPE && i!= SIGILL && i != SIGINT && i != SIGSEGV && i != SIGTERM)
		   continue;
      signal(i,SIG_DFL);
	}
#ifdef ESDOS
#ifndef FGV32
   if (timer_activado == 2) {
      desactiva_timer();
      timer_activado = 1;
			}
#endif
#endif
}




#ifndef MANUAL_INT
extern int (*avisar)();
#ifdef ESDOS
/* Asignaci¢n de la rutina de tratamiento */
extern void (interrupt far rutina())
{
   /* MUY IMPORTANTE: NO HACER NADA MAS AQUI DENTRO */
   general_int = avisar;
}
#else
extern void alarma_sig(s)
int s;
{
   signal(s,recu_sig); /* cualquier otra alarma se recupera !! */
   _programada_alarma = 0L;
   llama_fun_int(avisar,NULL);
}
#endif
#endif


static short t_t_contador = -1;
static short t_t_delay = 0;
static char par_timer[256];
extern int (*f_util)();
static int _oldes;
static int _oldbx;
static int _oldes1;
static int _oldbx1;

int pon_par_timer(s)
char *s;
{
			strcpy(par_timer,s);
         return(0);
}

int pon_delay_timer(d)
int d;
{
   t_t_delay = d;
   return(0);
}

int entra_timer_tick()
{
   general_int = NULL;
   t_t_contador = -1;
   if (f_util)
						llama_fun_int(f_util,par_timer);
   t_t_contador = 0;
   return(0);
}

#ifdef ESDOS
#ifndef FGV32

int long_jon = 0;
static void desconocida(void)
{
   asm {
						int 0E0h;
                  sti;
                  iret;
   }
}
extern void (interrupt far ENTRA_TIMER())
{
   asm {
						int 0E0h;
                  sti;
   }
   if (t_t_contador == -1) return;
   
   if (t_t_contador >= t_t_delay)
   {
						t_t_contador = 0;
                  general_int = entra_timer_tick;
   }
   t_t_contador++;
}

extern void set_entra_timer(tmp1)
unsigned long **tmp1;
{
   *tmp1 = &t_t_contador;
   tmp1++;
   *tmp1 = &general_int;
   tmp1++;
   *tmp1 = entra_timer_tick;
}

int activa_timer()
{
   int oldes = 0,oldbx = 0;
   int oldes1= 0,oldbx1 = 0;
   int newes,newbx;
   if (timer_activado) return(-1);
   
   if (t_t_delay != 32535)
   {
      
      newes = FP_SEG(ENTRA_TIMER);
      newbx = FP_OFF(ENTRA_TIMER);
      asm {
         push es
            push bx
            mov ax,35E0h
            int 21h
            mov oldes1,es
            mov oldbx1,bx
            mov ax,351ch
            int 21h
            mov oldes,es
            mov oldbx,bx
            push dx
            push ds
            mov dx,oldbx
            mov ax,oldes
            mov ds,ax
            mov ax,25E0h
            int 21h
            pop ds
            push ds
            mov dx,newbx
            mov ax,newes
            mov ds,ax
            mov ax,251ch
            int 21h
            pop ds
            pop dx
            pop bx
            pop es
      }
      
      _oldes = oldes;
      _oldbx = oldbx;
      _oldes1 = oldes1;
      _oldbx1 = oldbx1;
      
   }
   timer_activado = 2;
   t_t_contador = 0;
   return(0);
}

int desactiva_timer()
{
   int oldes = _oldes,oldbx = _oldbx;
   int oldes1 = _oldes1,oldbx1 = _oldbx1;
   t_t_contador = -1;
   if (timer_activado != 2) {timer_activado = 0;return(-1);}
   
   if (t_t_delay != 32535)
   {
      asm {
         push dx
            push ds
            mov dx,oldbx
            mov ax,oldes
            mov ds,ax
            mov ax,251ch
            int 21h
            pop ds
            push ds
            mov dx,oldbx1
            mov ax,oldes1
            mov ds,ax
            mov ax,25E0h
            int 21h
            pop ds
            pop dx
      }
   }
   
   timer_activado = 0;
   return(0);
}

#endif
#endif
