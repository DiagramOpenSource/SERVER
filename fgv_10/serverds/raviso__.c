/*---------------------------------------------------------------------------
* Programa ..: raviso.c
* Fecha .....:
* Version ...:
* Notas .....: Rutina de avisos
* Contenido .: int  aviso(),
*---------------------------------------------------------------------------*/

#include <fgvsys.h>

#ifndef FGVWIN
#include <rtermin.h>

extern int PCcolor;
extern int PFcolor;

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
* aviso
* pinta un texto en una posicion de pantalla y espera pulsar una tecla
* si la posicion es cero lo centra en la linea 24
*/
int aviso(pos,str)
int pos;
char *str;
{
#ifdef COMENTARIO
   int l = strlen(str);
   VENT *w;
   VENT *w1;
   int f1,c1,p1,p2;
   
   f1 = pos/100;
   c1 = pos%100;
   if (l > 78) l = 78;
   if (f1 <= 0)
      f1 = 4;                    /* fila en pantalla del mensaje */
   if (c1 <= 0)
      c1 = (80-l)/2+1;   /* columna         "            */
   
   if (c1 <= 1)     c1 = 2;
   if ((c1+l) > 80) c1 = 80 - l;
   if (f1 <= 1)     f1 = 2;
   if (f1 >= 24)    f1 = 23;
   
   p1 = f1*100+1;
   p2 = f1*100+80;
   
   w = w_ini(p1,p2);
   w_salva(w);
   w1 = w_ini(2401,2480);
   w_salva(w1);   
   Bl(f1);
   cuadro(p1,p2);
   str[l] = 0;
   pprint(f1*100+c1,str);
   pprint(p2-12,"Pulse tecla");
   pito();
   leecar();
   w_pinta(w);
   w_fin(w);
   w_pinta(w1);
   w_fin(w1);
   
   return(0);
#else
   int att = Patrib;
   int atc = PCcolor;
   int atf = PFcolor;
   int l;
   VENT *w1;
   
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      l = aviso_terminal(pos,str);        
      return(l);
   }
   en_conexion_externa_stop++;
#endif
#endif
   
   
   w1 = w_ini(2401,2480);
   w_salva(w1);
   l = mensaje(pos,str);
   atributo(tolower(Patrib));
   pito();
   parada();
   if (att == '0' || att == 'N' || att == ' ')
      Ar();
   Bl(l);
   w_pinta(w1);
   w_fin(w1);
   Color(atc,atf);
   atributo(att);
   
#ifdef RWXARXA
#ifdef RWSERVER
   en_conexion_externa_stop--;
#endif
#endif
   
   return(0);
#endif
}
#endif
