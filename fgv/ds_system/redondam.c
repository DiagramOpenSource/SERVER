/*---------------------------------------------------------------------------
* Programa ..: redondam.c
* Fecha .....: 29-01-88
* Version ...: 1.00
* Notas .....: Rutina para redondear numeros con funciones matematicas
*              Esta version es mas rapida que la redondeo pero ocupa 
*              mas espacio.
* Contenido .: double  redondea(), 
*---------------------------------------------------------------------------*/

#include <fgvsys.h>
#include <math.h>

#ifdef ES_AIX
extern double redondeo();
#endif

/*
extern double rwatof();

double redondea(num,dec)
double num;
int dec;
{
	char t[820],usi[100];
	
	sprintf(usi,"%%.%dlf",dec);
	sprintf(t,usi,num);
	return(rwatof(t));
}
*/

#ifndef CON_FLOOR

/*
* redondea
* devuelve un numero double 'num' redondeado al numero de decimales 'dec'
*/
double redondea(num,dec)
double num;
int dec;
{
   double a = 1.0;
   double d/*,c,f*/;
   int negativo = 0;
   
   while (dec-- > 0) a *= 10.0;    /* 10 elevado a dec */
   num *= a;
#ifdef ES_AIX   
   if (num > 999999999.0) /* PROBLEMA DEL AIX PS-2 (absoleto)*/
      return(redondeo(num,0) / a);
   else 
   {
#endif   	
	  if (num < 0)
	  {
		 negativo = 1;
		 num = -num;
	  }
	  
      num += .500001;
      d = floor(num);       /* para esta funcion necesitamos la libreria matem.*/	  

	  if (negativo)
	  {
		  d = -d;
	  }
	  
	  /*
	  if (d != num)
	  {
		  f = modf( num, &c);		  
		  if (num == c)
		  {
			  d = c;
		  }
	  }
	  */
#ifdef ES_AIX      
   }
#endif      
   return(d / a);
}

#endif