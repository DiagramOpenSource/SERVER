/*---------------------------------------------------------------------------
* Programa ..: rparada.c
* Fecha .....:
* Version ...:
* Notas .....: Rutina de parada
* Contenido .: int  parada(),
*---------------------------------------------------------------------------*/

#include <fgvsys.h>

#include <stdio.h>

#include <teclas.h>
#include <rtermin.h>
#ifdef FGVWIN
#include <FGVWIN.h>
#endif

/* parada  espera a que se pulse una tecla
*         devuelve  1 - esc
*                   2 - arriba
*                   3 - abajo
*                   4 - pgup
*                   5 - pgdn
*                   6 - consultar
*                   7 - abort
*                   0 - para el resto
*/

int que_qjes(c,flag)
int c;
int flag;
{
   int qj;
   eqj = 0;
#ifdef FGVWIN
   if (c >= MID_MENUSG && c < MID_DISENO) /* opcion de menu pulsada */
   {
      qj = 1;
      eqj = c;
      return(qj);
   }
#endif
   if (flag)
   {
      if (c == HOME)
         c = SOLICITA_INICIO;
      else
      if (c == END)
         c = SOLICITA_FINAL;
      else
      if (flag == 2)
      {
         if (c == DEL)
            c = SOLICITA_SUPRESION;
         else
         if (c == INS)
            c = SOLICITA_INSERCION;
      }
   }
   switch (c) {
   case SOLICITA_FINAL:
   case SOLICITA_INICIO:
   case SOLICITA_INSERCION:
   case SOLICITA_SUPRESION:
   case SOLICITA_SCROLLUP:
   case SOLICITA_SCROLLDOWN:
   case SOLICITA_POSICION:
   case SOLICITA_VALIDACION:
   case SOLICITA_ANULACION:
   case SOLICITA_AVANCE:
   case SOLICITA_RETROCESO:
   case 999:
      qj = c;
      break;
   case ARRIBA :
      qj = 2;
      break;
   case ABAJO :
      qj = 3;
      break;
   case VALIDAR :
   case CRETURN :
      qj = 0;
      break;
   case ACABAR :
   case ESC :
      qj = 1;
      break;
   case ABORTAR :
   case CTRLC :
      qj = 7;
      break;
   case PGUP :
      qj = 4;
      break;
   case PGDN :
      qj = 5;
      break;
   case CONSULTAR:
      qj = 6;
      break;
   case LISTADO:
      qj = 6;
      eqj = 1;
      break;
   case AYUDA:
   case _AYUDA:
      qj = 8;
      break;
   default :
      if (c >= EXTRATECLA)
         qj = 9 + (c - EXTRATECLA);
      else
         qj = -1;
   }
   return(qj);
}
