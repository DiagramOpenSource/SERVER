/*---------------------------------------------------------------------------
* Programa ..: rcuadro.c
* Fecha .....: 15-12-87
* Version ...: 1.00
* Notas .....: Rutina de cuadros
* Contenido .: int  cuadro(), 
*---------------------------------------------------------------------------*/

#include <fgvsys.h>
#include <stdio.h>
#include <string.h>
#include <rtermin.h>

#ifdef FGVWIN

extern int un_fondo_especial;

#endif

#ifdef RWXARXA
#ifdef RWSERVER
extern int en_conexion_externa;
extern int en_conexion_externa_stop;
#endif
#endif

/* cuadro pinta un cuadro con vertices opuestos es pi,pf
* cuadro(101,2380); recuadra la pantalla
*/
int bi_cuadro(pi,pf,m)
int pi,pf,m; /* m == 0 simple m == 1 doble */
{
   int p,i;
   int fi,ff,ci,cf;
   
   
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      return(bi_cuadro_en_terminal(pi,pf,m));
   }
#endif
#endif
   
   
   if (m == 1) m += 12;
   
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
   if (fi == ff)
   {
      p = 1;
   }
   else
   {
#ifdef FGVWIN
      un_fondo_especial = 3;
#endif
      p = 0;
   }
   Gon();
   cursor(fi*100+ci+1-p);
   for (i = ci + 1 - p;i < (cf + p) ;i++)
      Graf(1+m);
   if (p) {
      Goff();
      fflush(stdout);
#ifdef FGVWIN
      un_fondo_especial = 0;
#endif
      return(0);
   }
   
#ifdef FGVWIN
   un_fondo_especial = 1;
#endif
   cursor(ff*100+ci+1);
   for (i = ci + 1;i < cf;i++)
      Graf(1+m);
   
   
#ifdef FGVWIN
   un_fondo_especial = 4;
#endif
   
   if (ci == cf)
   {
      p = 1;
#ifdef FGVWIN
      un_fondo_especial = 0;
#endif
   }
   else
      p = 0;
   
   for (i = fi + 1 - p;i < (ff + p);i++) {
      cursor(i*100+ci);
      Graf(0+m);
   }
   if (p) {
      Goff();
      fflush(stdout);
#ifdef FGVWIN
      un_fondo_especial = 0;
#endif
      return(0);
   }
   
#ifdef FGVWIN
   un_fondo_especial = 2;
#endif
   
   for (i = fi + 1;i < ff;i++) {
      cursor(i*100+cf);
      Graf(0+m);
   }
   
#ifdef FGVWIN
   un_fondo_especial = 4;
#endif
   
   cursor(fi*100+ci);
   Graf(2+m);
   
#ifdef FGVWIN
   un_fondo_especial = 2;
#endif
   
   cursor(fi*100+cf);
   Graf(4+m);
   
#ifdef FGVWIN
   un_fondo_especial = 4;
#endif
   
   cursor(ff*100+ci);
   Graf(8+m);
   
#ifdef FGVWIN
   un_fondo_especial = 2;
#endif
   
   cursor(ff*100+cf);
   Graf(10+m);
   Goff();
   fflush(stdout);
#ifdef FGVWIN
   un_fondo_especial = 0;
#endif
   return(0);
}

int cuadro(pi,pf)
int pi,pf;
{
   return(bi_cuadro(pi,pf,0));
}
