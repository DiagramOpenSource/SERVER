/*---------------------------------------------------------------------------
* Programa ..: rencrip.c                     Autor: Francisco Gonzalez Valiente
* Fecha .....: 01-12-87
* Version ...: 1.00
* Notas .....: Rutina de encriptacion de strings.
* Contenido .: int asc_hex(),hex_asc,encrip(),dencrip()
*---------------------------------------------------------------------------*/


#include <fgvsys.h>

#include <stdio.h>
#include <string.h>


/*
* encrip
* encripta y desencripta un string
*/
encrip(ostr,hstr)
char *ostr;
char *hstr;
{
   unsigned char dstr[81];
   int i, j, l = 0;
   int p;
   
   strcpy(dstr,ostr);
   l = strlen(dstr);
   for (j = 1, i = l; i > 0; i--, j++) {
      if (*(dstr + i - 1)  > 157)
         hstr[j-1] = 158 + (255 - *(dstr + i - 1));
      else
         hstr[j-1] = 126 - *(dstr + i - 1) + 32;
   }
   *(hstr + l) = '\0';
}

