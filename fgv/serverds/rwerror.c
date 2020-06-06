
#include <fgvsys.h>
#ifndef FGVWIN
#include <stdio.h>
#include <rtermin.h>

extern int tracer_int;
extern int modo_desatendido;

int c_rw_error = 0;
int no_rw_error = 0;

int rw_error(n)
int n;
{
   char tmp[81];

   c_rw_error++;
   if (no_rw_error)
	   return 0;

   sprintf(tmp,__Texto[189],n);
   if (stdpan)
   {
      aviso(0,tmp);
   }
   else
   {
      printf("\07WARNING N.%d\r\n",n);
	  if (modo_desatendido <= 0)
		getchar();
   }
   tracer_int = 1;
   return(0);
}
#endif

