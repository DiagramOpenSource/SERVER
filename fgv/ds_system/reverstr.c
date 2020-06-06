
/*---------------------------------------------------------------------------
* Programa ..: reverstr.c
* Fecha .....: 03-02-88
* Version ...: 1.00
* Notas .....: Rutina para invertir strings
* Contenido .: char *reverstr(),
*---------------------------------------------------------------------------*/


#include <fgvsys.h>
#include <string.h>

/*
* reverst
* rutina para invertir strings
* devuelve un puntero al string invertido (que se pone sobre 's')
*/
char * reverstr(s)
char *s;
{
#ifdef ESDOS
   return(strrev(s));
#else
   int i,l;
   char c;
   
   if ((l = strlen(s) - 1 ) > 0) {
      i = 0;
      while (i < l) {
         c = s[i];
         s[i++] = s[l];
         s[l--] = c;
      }
   }
   return(s);
#endif
}


