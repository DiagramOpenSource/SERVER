/*---------------------------------------------------------------------------
* Programa ..: rcuabla.c
* Fecha .....: 15-12-87
* Version ...: 1.00
* Notas .....: Rutina de cuadros
* Contenido .: int  cuabla(),
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


#ifdef COMENTARIO

#ifdef ESDOS

static void qursor(a)
register int a;
{
   Pcursor = (a/100-1)*80 + a%100 - 1;
}

static void qgraf(i)
register int i;
{
   v_putc(i+'a');
   Pcursor++;
}

/* cuadro pinta un cuadro con vertices opuestos es pi,pf
* cuadro(101,2380); recuadra la pantalla
*/
int cuabla(pi,pf)
int pi,pf;
{
			char fi,ff,ci,cf,controlinea;
         extern int DOSMABLA(char,char,char,char,char); /* cambios para MS DOS */
         extern int DOSLIBLA(char,char,char,char,char); /* cambios para MS DOS */
         int i;
         
         fi=pi/100;   /* fila inicio */
         ci=pi%100;   /* columna inicio */
         ff=pf/100;   /* fila final */
         cf=pf%100;   /* columna final */
         
         if(ff<fi) {
            controlinea = fi;
            fi = ff;
            ff = controlinea;
         }
         
         if(cf<ci) {
            controlinea = ci;
            ci = cf;
            cf = controlinea;
         }
         
         if ((fi*ff*ci*cf) == 0)
            return(-1);
         
         if(fi == ff) {
            ff = cf - ci + 1;
            controlinea = 'H';
            DOSLIBLA(fi,ci,ff,controlinea,dos_atri());
            return(0);
         }
         
         if(ci == cf) {
            cf = ff - fi + 1;
            controlinea = 'V';
            DOSLIBLA(fi,ci,cf,controlinea,dos_atri());
            return(0);
         }
         
         ff--;
         cf--;
         
         DOSMABLA(fi,ci,ff,cf,dos_atri());
         
         ff++;
         cf++;
         
         Pgrafi ='a';
         qursor(fi*100+ci);
         for (i=ci;i<=cf;i++)
            qgraf(1);
         if (fi == ff) {
            Pgrafi =' ';
            return(0);
         }
         qursor(ff*100+ci);
         for (i=ci;i<=cf;i++)
            qgraf(1);
         for (i=fi;i<=ff;i++) {
            qursor(i*100+ci);
            qgraf(0);
         }
         if (ci == cf) {
            Pgrafi =' ';
            return(0);
         }
         for (i=fi;i<=ff;i++) {
            qursor(i*100+cf);
            qgraf(0);
         }
         qursor(fi*100+ci);
         qgraf(2);
         qursor(fi*100+cf);
         qgraf(4);
         qursor(ff*100+ci);
         qgraf(8);
         qursor(ff*100+cf);
         qgraf(10);
         Pgrafi =' ';
         return(0);
         
}

#else

/* cuabla pinta un cuadro con vertices opuestos es pi,pf
* cuabla(101,2380); recuadra la pantalla
*/
int cuabla(pi,pf)
int pi,pf;
{
   int p,i;
   int fi,ff,ci,cf;
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
   cursor(fi*100+ci);
   for (i=ci;i<=cf;i++,Pcursor++) {
      v_putc(' ');
      putchar(' ');
   }
   if (fi == ff) {
      fflush(stdout);
      return(0);
   }
   cursor(ff*100+ci);
   for (i=ci;i<=cf;i++,Pcursor++) {
      v_putc(' ');
      putchar(' ');
   }
   for (i=fi;i<=ff;i++) {
      cursor(i*100+ci);
      v_putc(' ');
      putchar(' ');
   }
   if (ci == cf) {
      fflush(stdout);
      return(0);
   }
   for (i=fi;i<=ff;i++) {
      cursor(i*100+cf);
      v_putc(' ');
      putchar(' ');
   }
   fflush(stdout);
   return(0);
}

#endif

#else

/* cuabla pinta un cuadro con vertices opuestos es pi,pf
* cuabla(101,2380); recuadra la pantalla
*/
int cuabla(pi,pf)
int pi,pf;
{
   int p,i;
   int fi,ff,ci,cf;
   fi=pi/100;   /* fila inicio */
   ci=pi%100;   /* columna inicio */
   ff=pf/100;   /* fila final */
   cf=pf%100;   /* columna final */
   
   
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
						return(bi_cuadro_en_terminal(pi,pf,-1));
   }
#endif
#endif
   
   
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
   cursor(fi*100+ci);
   for (i=ci;i<=cf;i++,Pcursor++) {
						v_putc(' ');
                  putchar(' ');
   }
   if (fi == ff) {
						fflush(stdout);
                  return(0);
   }
   cursor(ff*100+ci);
   for (i=ci;i<=cf;i++,Pcursor++) {
						v_putc(' ');
                  putchar(' ');
   }
   for (i=fi;i<=ff;i++) {
						cursor(i*100+ci);
                  v_putc(' ');
                  putchar(' ');
   }
   if (ci == cf) {
						fflush(stdout);
                  return(0);
   }
   for (i=fi;i<=ff;i++) {
						cursor(i*100+cf);
                  v_putc(' ');
                  putchar(' ');
   }
   fflush(stdout);
   return(0);
}

#endif
