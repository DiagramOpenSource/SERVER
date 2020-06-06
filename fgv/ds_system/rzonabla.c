
/*---------------------------------------------------------------------------
* Programa ..: rzonabla.c
* Fecha .....:
* Version ...:
* Notas .....: Rutina de blanqueo de una zona de pantalla
* Contenido .: int  zonablan(), 
*---------------------------------------------------------------------------*/

#include <fgvsys.h>

#include <stdio.h>
#include <string.h>

#include <rtermin.h>

#ifdef RWXARXA
#ifdef RWSERVER
extern int en_conexion_externa;
extern int en_conexion_externa_stop;
#endif
#endif

/* zonablan blanquea el interior de un cuadro
* zonablan(101,2480); equivale a cls(); (lento)
*/
int zonablan(pi,pf)
int pi,pf;
{
   int p,i;
   int fi,ff,ci,cf;
   char usi[10];
   
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      return(bi_cuadro_en_terminal(pi,pf,-2));
   }
#endif
#endif
   
   
   fi=pi/100;   /* fila inicio */
   ci=pi%100;   /* columna inicio */
   ff=pf/100;   /* fila final */
   cf=pf%100;   /* columna final */
   if (fi>ff) {
      p=fi;
      fi=ff;
      ff=p;
   }
   if (ci>cf) {
      p=ci;
      ci=cf;
      cf=p;
   }
   if ((fi*ff*ci*cf) == 0)
      return(-1);
   sprintf(usi,"%%-%ds ",cf-ci);  /* using para el print */
   for (i=fi;i<=ff;i++) {
      cursor(i*100+ci);
      v_prints(usi," ");
   }
   fflush(stdout);
   return(0);
}


