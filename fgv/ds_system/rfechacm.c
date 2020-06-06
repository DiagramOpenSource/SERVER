

/*---------------------------------------------------------------------------
* Programa ..: rfecomp.c
* Fecha .....: 15-02-88
* Version ...: 1.00
* Notas .....: Rutina para comparar fechas
* Contenido .: int  fechacmp(), 
*---------------------------------------------------------------------------*/

#include <fgvsys.h>

/*
* fechacmp
* compara las fechas f1 y f2 devolviendo un numero entero de la forma :
* menor, igual o mayor que cero si f1 es menor, igual o mayor que f2
*/
fechacmp(f1,f2)
char *f1;
char *f2;
{
   return(cmfecha(f1,f2));
   /*
   register int i;
   char t1[10],t2[10];
   
    for (i=0;i<4;i++)    t1[i]   = f1[6+i];
    for (i=0;i<2;i++)    t1[i+4] = f1[3+i];
    for (i=0;i<2;i++)    t1[i+6] = f1[i];
    t1[8]='\0';
    for (i=0;i<4;i++)    t2[i]   = f2[6+i];
    for (i=0;i<2;i++)    t2[i+4] = f2[3+i];
    for (i=0;i<2;i++)    t2[i+6] = f2[i];
    t2[8]='\0';
    return(strcmp(t1,t2));
   */
}



