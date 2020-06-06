
/* Autor: Francisco Gonzalez Valiente */

#include <fgvsys.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


double rwatof(s)
char *s;
{
   double d;
   char *t = s;
   
   while (*t && *t != ',') t++;
   if (*t == ',') *t = '.';
   d = atof(s);
   if (*t) *t = ',';
   
   return(d);
}


