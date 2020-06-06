/*---------------------------------------------------------------------------
* Programa ..: rtecla.c                  Autor: Francisco Gonzalez Valiente
*
* Fecha .....:
* Version ...:
* Notas .....: Rutina para controlar si se ha pulsado una tecla y
*              rutinas para pausa e interrupcion desde el teclado
* Contenido .: int  pulsatecla(), pausa_fin()
*---------------------------------------------------------------------------*/

#include <fgvsys.h>

#include <time.h>
#include <sys/timeb.h>
#include <stdio.h>
#include <rtermin.h>

extern FILE *_imp;
extern int atento_imp;
int s_salida = 0;
#ifdef RWXARXA
#ifdef RWSERVER
extern int en_conexion_externa;
static long milastime = 0L;
#endif
#endif

/*
* pausa_fin
* permite parar el proceso con el caracter blanco ' '
* una vez parado puede proseguirse pulsando return 'CR'
* o abortarse pulsando ctrl-c '^C'
* la rutina devuelve -1 en caso de abortar, 0 para continuar
*/
int pausa_fin()
{
   int c;
   VENT *w;
   int oe = 0;
   
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      /* al menos 2 segundos */
      if (milastime >= time(NULL))
      {
         return(0);
      }
      milastime = time(NULL) + 2;
   }
#endif
#endif
   
   if (pulsatecla() != 32) return(0);
   
   if (_imp && atento_imp) {
      i_acabo(_imp);
      oe = 1;
      atento_imp = 0;
   }
   
   w = w_ini(1229,1650);
   w_salva(w);
   zonablan(1229,1650);
   cuadro(1229,1650);
   Ap();
   pprint(1330,__Texto[345]);
   Cp();
   Ar();
   pprint(1430,__Texto[346]);
   pprint(1530,__Texto[347]);
   Cr();
   pito(165);
   do {
      c = leecar();
   }
   while (c != 807 && c != 802);
   w_pinta(w);
   w_fin(w);
   
   if (_imp && oe) {
      i_inicio(_imp);
      atento_imp = 1;
   }
   
   if (c == 807)
      return((s_salida = -1));
   else
      return((s_salida = 0));
}


