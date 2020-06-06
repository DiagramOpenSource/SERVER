
/*---------------------------------------------------------------------------
* Programa ..: rmensaje.c
* Fecha .....:
* Version ...:
* Notas .....: Rutina de mensajes
* Contenido .: int  mensaje(),
*---------------------------------------------------------------------------*/

#include <fgvsys.h>

#include <stdio.h>
#include <string.h>

#include <rtermin.h>


#ifndef FGVWIN

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


/*
* mensaje
* pinta un texto en una posicion de pantalla
* si la posicion es cero lo centra en la linea 4
* devuelve el numero de linea del mensaje
*/
int mensaje(pos,str)
int pos;
char *str;
{
   int f1,c1;
   int aatri = 0;
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      mensaje_terminal(pos,str);
   }
   en_conexion_externa_stop++;
#endif
#endif
   
   
   
   f1 = pos/100;
   c1 = pos%100;
   if (f1 <= 0)
   {
      f1 = 4;                    /* fila en pantalla del mensaje */
      if (Patrib == 'N' || Patrib == '0' || Patrib == ' ')
      {
         aatri = 1;
         Ar();
      }
   }
   if (c1 <= 0)
      c1 = (80-strlen(str))/2;   /* columna         "            */
   Bl(f1);
   if (*str != '\0') {
      pprint(f1*100+c1,str);
      fflush(stdout);
   }
   if (aatri)
   {
      Cr();
   }
   
#ifdef RWXARXA
#ifdef RWSERVER
   en_conexion_externa_stop--;
#endif
#endif
   return(f1);
}

#endif

