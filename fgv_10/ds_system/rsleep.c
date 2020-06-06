/*---------------------------------------------------------------------------
* Programa ..:
* Fecha .....:
* Version ...: 2.00
* Notas .....:
* Contenido .:
*---------------------------------------------------------------------------*/

#include <fgvsys.h>

#ifdef ESDOS

#ifndef __MSC
#define __MSC
#endif
#include <stdio.h>
#include <dos.h>


#ifdef COMENTARIO

#ifdef FGVDOS

static int el_pid = 0;

int getpid()
{
   if (!el_pid) el_pid = rand() + 1;
   return(el_pid);
}

#endif

#endif


#ifndef FGVDOS
int sleep(x)        /* esta rutina sustituye a la funcion sleep de xenix */
unsigned int x;     /* y procura hacer una pausa de seg segundos de modo */
{                   /* que en caso de una red local la CPU pueda dedicar */
   
   return(sleep10th( x*10 ));
}
#endif


#ifndef FGV32
#ifdef _MSC_VER
int sleep(x)        /* esta rutina sustituye a la funcion sleep de xenix */
unsigned int x;     /* y procura hacer una pausa de seg segundos de modo */
{                   /* que en caso de una red local la CPU pueda dedicar */
   
   return(sleep10th( x*10 ));
}
#endif

sleep10th( x )   /* NOTE: THIS FACILITY HAS AN UPPER LIMIT OF 59 SECONDS */
int x;           /* # of tenths of seconds to sleep */
{
			int dest_sec,new_sec;
         int flipsec;
         int dest_10th,new_10th;
         union REGS inregs,outregs;
         
         /** establish destination time */
         inregs.h.ah = 0x2c;
         intdos( &inregs,&outregs );
         dest_sec = outregs.h.dh;
         flipsec = dest_sec;
         dest_10th = outregs.h.dl;
         dest_10th += (x % 10) * 10;
         if( dest_10th > 100 )
         { dest_10th -= 100; dest_sec++; }
         dest_sec += x / 10;
         
         /** loop til there */
         while( 1 )
         { intdos( &inregs,&outregs );
         new_sec = outregs.h.dh;
         if( new_sec < flipsec ) new_sec += 60;
         new_10th = outregs.h.dl;
         if( new_sec > dest_sec ) return 0;
         if( new_sec < dest_sec ) continue;
         if( new_10th > dest_10th ) return 0;
         }
}
#else
#ifdef _MSC_VER
int sleep(int i)
{
   
   Sleep(i*1000);
   return(0);
}
#endif
#endif


#endif
































