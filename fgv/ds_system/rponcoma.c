

/* #define _MAIN /* para test locales */
/*---------------------------------------------------------------------------
* Programa ..: rponcoma.c
* Fecha .....:
* Version ...:
* Notas .....: Rutina para usar formatos de print en castellano
* Contenido .: char *poncoma(), 
*---------------------------------------------------------------------------*/

#include <fgvsys.h>

#include <string.h>
#include <malloc.h>

extern char *reverstr();

#define DECIMAL ','
#define MILLAR  '.'

/*
* poncoma
* pone en el string de destino 's' el valor del string de origen 'r'
* pero con el formato de print en castellano
* el string 's' debe tener el suficiente espacio antes de llamar a la funcion
* P.ej. poncoma(s,"1234567.12") da s="1.234.567,12"
*/
char *poncoma(s,r)
char *s;      /* string de destino */
char *r;      /* string de origen (con valor numerico) */
{
   char *t;
   int i,j,k,l,punto,maximo,malo = 0,flag;
   
   if (!r) 
   {
      r = s;
      malo = 1; /* solo poner la coma !!! */
   }
   
   maximo = 0;
   punto = 0; /* punto donde comenzar a contar */
   l = strlen(r);
   t = malloc(l + 1);
   k = 0;
   j = 0; /* numero de comas a poner */
   for (i = l - 1 ;i > -1;i--) 
   { /* copiar string en t al reves contando incidencias */
      if (r[i] == ' ')
         maximo++; /* ignorar espacios pero contarlos como sitio para los puntos */
      else 
	  {
         if (r[i] == '.')
		 {
            if (r == s)
               r[i] = DECIMAL;
            t[k++] = DECIMAL;
            punto = k;
            j = 0;
         }
         else 
		 {
            if (r[i] >= '0' && r[i] <= '9') 
			{               
               if ((k - punto) && !((k - punto) % 3) ) j++;
            }
            else {
               if (r[i] != '+' && r[i] != '-')
                  malo = 1; /* hay alfanumericos !!! */
            }
            t[k++] = r[i];
         }
      }
   }
   if (malo || (r == s && !maximo)) {
      free(t);
      if (r != s)
         strcpy(s,r);
      return(r);
   }
   if (r == s && j > maximo)
   {
      flag = j - maximo;
   }
   else
      flag = 0;
   if (punto)
      memcpy(s,t,punto);
   j = punto;   
   for(i = punto;i < k;i++) 
   {
      if ((i - punto) && !((i - punto) % 3) && t[i] >= '0' && t[i] <= '9')
      {
         if (flag)
            flag--;
         else
            s[j++] = MILLAR;
      }
      s[j++] = t[i];
   }
   if (r == s)
      for(;j < (k+maximo);j++)
         s[j] = ' ';
      s[j] = 0;
      
      reverstr(s);
      free(t);
      return(s);
}


#ifdef _MAIN

main()
{
   char qo[82];
   
   
   poncoma(qo,"12345.678");
   printf("%s\n\r",qo);
   strcpy(qo,"    12345.678");
   poncoma(qo,qo);
   printf("%s\n\r",qo);
}

#endif

