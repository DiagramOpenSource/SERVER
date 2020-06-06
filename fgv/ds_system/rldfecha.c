
/*---------------------------------------------------------------------------
* Programa ..: rldfecha.c
* Fecha .....:
* Version ...:
* Notas .....: Rutina para leer fechas
* Contenido .: int  ldfecha(),
*---------------------------------------------------------------------------*/

#include <fgvsys.h>

#include <memory.h>


extern char caracter_de_fecha;
/*
* ldfecha
* obtiene en 'destino' un string en formato fecha (dd.mm.aaaa)
* procedente de un registro 'origen' de 8 caracteres de la forma aaaammdd
* destino ha de tener al menos 11 caracteres (ya incluido el \0)
*/
ldfecha(origen,destino)
char *origen;
char *destino;
{
   memcpy(destino  ,origen+6,2);
   *(destino+2) = caracter_de_fecha;
   memcpy(destino+3,origen+4,2);
   *(destino+5) = caracter_de_fecha;
   memcpy(destino+6,origen  ,4);
   *(destino+10) = '\0';
   return(0);
}


ldfecha6(origen,destino)
char *origen;
char *destino;
{
   memcpy(destino  ,origen+6,2);   
   memcpy(destino+2,origen+4,2);   
   memcpy(destino+4,origen+2,2);   
   *(destino+6) = '\0';
   return(0);
}


ldfecha8(origen,destino)
char *origen;
char *destino;
{
			memcpy(destino  ,origen+6,2);
         *(destino+2) = caracter_de_fecha;
         memcpy(destino+3,origen+4,2);
         *(destino+5) = caracter_de_fecha;
         memcpy(destino+6,origen+2,2);
         *(destino+8) = '\0';
         return(0);
}


