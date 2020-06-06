
/*---------------------------------------------------------------------------
* Programa ..: rfidoble.c
* Fecha .....: 04-03-88
* Version ...: 1.00
* Notas .....: Rutina para leer y grabar campos tipo double de un fichero
* Contenido .: int  st_doble(),
*              double  ld_doble(), 
*---------------------------------------------------------------------------*/

#include <fgvsys.h>
#include <string.h>

#define _NUEVODEF
#include <rdef.h>     /* ya incluye el isam.h */

#include <stdlib.h>
/*extern long atol();*/

extern double redondea();


/*
* st_doble
* pone el numero (origen) en el campo correspondiente al registro (destino)
* el fichero corresponde a una estructura tipo pfi
* destino es el buffer del registro correspondiente a pfi
*/
st_doble(campo,pfi,origen)
int campo;
FI_DEF *pfi;
double origen;
{
   char *destino,*usi,*min,*max;
   int tipo,offset,longit,i;
   
   destino = pfi->f_buf;
   tipo = pfi->f_campos[campo].c_tipo;
   offset = pfi->f_campos[campo].c_offset;
   min    = pfi->f_campos[campo].c_minimo;
   max    = pfi->f_campos[campo].c_maximo;
   
   switch (tipo) {
   case DOUBLETIPO :
      i = strlen(min);	    
      longit = decimales_redondeo_campo(min,max);
      origen = redondea(origen,longit);
      stdbl(origen,destino+offset);
      break;
   case INTTIPO :
      stint((int) origen,destino+offset);
      break;
   case LONGTIPO :
      stlong((long) origen,destino+offset);
      break;
   default:
      break;
   }
   return(0);
}

/*
* ld_doble
* lee un campo double del registro (origen) y lo pone en el numero (destino)
* el fichero corresponde a una estructura tipo pfi
* origen es el buffer del registro correspondiente a pfi
*/
double ld_doble(campo,pfi)
int campo;
register FI_DEF *pfi;
{
   double destino = 0.0;
   char *origen;
   int tipo,offset,longit,i;
   char *usi,*min,*max;
   
   origen = pfi->f_buf;
   tipo = pfi->f_campos[campo].c_tipo;
   offset = pfi->f_campos[campo].c_offset;
   min    = pfi->f_campos[campo].c_minimo;
   max    = pfi->f_campos[campo].c_maximo;
   
   switch (tipo) {
			case DOUBLETIPO :
            destino = lddbl(origen+offset);        
            longit = decimales_redondeo_campo(min,max);
            destino = redondea(destino,longit);       
            break;
         case INTTIPO :
            destino = (double)ldint(origen+offset);	 
            break;
         case LONGTIPO :
            destino = (double)ldlong(origen+offset);
            break;
         default:
            break;
			}
   return(destino);
}


