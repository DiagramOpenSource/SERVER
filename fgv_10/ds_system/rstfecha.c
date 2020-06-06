
/*---------------------------------------------------------------------------
* Programa ..: rstfecha.c
* Fecha .....:
* Version ...:
* Notas .....: Rutina para grabar fechas
* Contenido .: int  stfecha(), 
*---------------------------------------------------------------------------*/

#include <fgvsys.h>

#include <memory.h>

/*
* stfecha
* almacena en 'destino' un string en formato fecha 'origen' dd.mm.aaaa\0 
* en 8 caracteres aaaammdd
* para grabar en un registro (eliminando el \0)
*/
stfecha(origen,destino)
char *origen;
char *destino;
{
   memcpy(destino  ,origen+6,4);
   memcpy(destino+4,origen+3,2);
   memcpy(destino+6,origen  ,2);
   return(0);
}


