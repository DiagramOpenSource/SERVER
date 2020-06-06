
#include <fgvsys.h>
#ifndef FGVWIN
#include <stdio.h>
#include <rtermin.h>

int rw_error(n)
int n;
{
   char tmp[81];
   sprintf(tmp,"Aviso de Control Interno de Sistema Número %d",n);
   if (stdpan)
   {
      aviso(0,tmp);
   }
   else
   {
      printf("\07AVISO N.%d\r\n",n);
      getchar();
   }
   return(0);
}
#endif

