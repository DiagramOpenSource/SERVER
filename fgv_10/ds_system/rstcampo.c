/*---------------------------------------------------------------------------
* Programa ..: rstcampo.c
* Fecha .....:
* Version ...:
* Notas .....: Rutina para grabar campos de un fichero
* Contenido .: int  st_campo(),
*---------------------------------------------------------------------------*/

#include <fgvsys.h>

#define _NUEVODEF
#include <rdef.h>      /* ya incluye el isam.h */

#include <stdlib.h>
/*extern long atol();*/

/*
* st_campo
* pone el string (origen) en el campo correspondiente al registro (destino)
* el fichero corresponde a una estructura tipo pfi
* destino es el buffer del registro correspondiente a pfi
*/
st_campo(campo,pfi,origen)
int campo;
FI_DEF *pfi;
char *origen;
{
   char *destino;
   int tipo,offset,longit;
   
   destino = pfi->f_buf;
   tipo = pfi->f_campos[campo].c_tipo;
   offset = pfi->f_campos[campo].c_offset;
   longit = pfi->f_campos[campo].c_long;
   switch (tipo) {
   case CHARTIPO :
      stchar(origen,destino+offset,longit);
      break;
   case FECHATIPO :
      stfecha(origen,destino+offset);
      break;
   case INTTIPO :
      stint(atoi(origen),destino+offset);
      break;
   case LONGTIPO :
      stlong(atol(origen),destino+offset);
      break;
   case DOUBLETIPO :
      stdbl(rwatof(origen),destino+offset);
      break;
   default:
      break;
   }
   return(0);
}


st_campo_no_fecha(campo,pfi,origen)
int campo;
FI_DEF *pfi;
char *origen;
{
   char *destino;
   int tipo,offset,longit;
   
   destino = pfi->f_buf;
   tipo = pfi->f_campos[campo].c_tipo;
   offset = pfi->f_campos[campo].c_offset;
   longit = pfi->f_campos[campo].c_long;
   switch (tipo) {
   case CHARTIPO :
      stchar(origen,destino+offset,longit);
      break;
   case FECHATIPO :
      stchar(origen,destino+offset,8);
      break;
   case INTTIPO :
      stint(atoi(origen),destino+offset);
      break;
   case LONGTIPO :
      stlong(atol(origen),destino+offset);
      break;
   case DOUBLETIPO :
      stdbl(rwatof(origen),destino+offset);
      break;
   default:
      break;
   }
   return(0);
}


