
/*---------------------------------------------------------------------------
* Programa ..: rcmfecha.c
* Fecha .....: 11-01-88
* Version ...: 1.00
* Notas .....: Rutina para comparar fechas
* Contenido .: int  cmfecha(), 
*---------------------------------------------------------------------------*/

#include <fgvsys.h>

#include <memory.h>

/*
* cmfecha
* compara dos fechas f1 y f2 devolviendo un entero que sera < == o > que 0
* si f1 < f2  , f1 == f2  o  f1 > f2  respectivamente.
*/
int cmfecha(f1,f2)
char *f1;
char *f2;
{
   char ff1[10],ff2[10];
   
   stfecha(f1,ff1);
   stfecha(f2,ff2);
   return(memcmp(ff1,ff2,8));
}

