/*---------------------------------------------------------------------------
* Programa ..: rcorrect.c
* Fecha .....: 15-12-87
* Version ...: 1.00
* Notas .....: Rutina de confirmacion
* Contenido .: int  confirma(), sino(), correcto(),
*---------------------------------------------------------------------------*/

#include <fgvsys.h>

#ifndef FGVWIN

#include <stdio.h>
#include <string.h>

#include <rtermin.h>
#include <rentra.h>


#ifdef FGV32
#ifndef WATCOMNLM
extern HANDLE myconsola;
extern HANDLE hStdIn;
#endif
#endif

#ifdef RWXARXA
#ifdef RWSERVER
extern int en_conexion_externa;
extern int en_conexion_externa_stop;
#endif
#endif


/* confirmar pregunta si es correcto o no
*           se le pasa la posicion, el mensaje y el valor por defecto
*           si pos es 0 lo centra en la linea 24
*           devuelve  0 si responde S s
*                    -1 si responde N n
*/
int confirmar(pos,mensa,defec)
int pos;
char *mensa;
char *defec;
{
   int bien=1;
   char sino[2];
   int qj;
   int f1,c1,posi;
   
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      qj = confirmar_en_terminal(pos,mensa,defec);        
      return(qj);
   }
   en_conexion_externa_stop++;
#endif
#endif
   
   
   f1 = pos/100;
   c1 = pos%100;
   if (f1 <= 0)
      f1 = 24;                   /* fila en pantalla del mensaje */
   if (c1 <= 0)
      c1 = (80-strlen(mensa))/2;   /* columna         "            */
   pos = f1*100+c1;
   posi = pos/100;
   
   *(defec+1) = '\0';
   pprint(pos,mensa);
   pos += strlen(mensa);
   return(sinod(pos,defec));
}

/* sino      pregunta si o no
*           devuelve  0 si responde S s
*                    -1 si responde N n
*/
int sinod(pos,defec)
int pos;
char *defec;
{
   int bien=1;
   char sino[2];
   int qj;
   do {
      qj = entras(pos,1,defec,1,sino);
      if (qj == 7) {
         Bl(pos/100);
         return(-1);
      }
      if (*sino == __Texto[448][0] || *sino == __Texto[452][0])
	  {      
         bien = 0;
	  } else if (*sino == __Texto[447][0] || *sino == __Texto[451][0])
	  {
         bien = -1;
	  }
	  else
	  {
         pito(190);
         bien = 1;
	  }      
   } while (bien == 1);
   Bl(pos/100);
   return(bien);
}

int sino(pos)
int pos;
{
   return(sinod(pos,"S"));
}

/* correcto  pregunta si es correcto o no
*           devuelve  0 si responde S s
*                    -1 si responde N n
*/
int correcto()
{
#ifdef RWXARXA
#ifdef RWSERVER
   int qj;
   if (en_conexion_externa)
   {
      qj = correcto_en_terminal();        
      return(qj);
   }
#endif
#endif
   pprint(2455,__Texto[431]);
   return(sino(2478));
}

#endif

