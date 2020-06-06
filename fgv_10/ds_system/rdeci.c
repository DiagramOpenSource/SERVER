/* Autor: Francisco Gonzalez Valiente */
#include <fgvsys.h>
#include <string.h>
#include <stdio.h>



int (*asigna_decimales)(char *nom) = NULL;

int decimales_redondeo_campo(min,max)
char *min,*max;
{
   char *p;
   int t;
   
   if (!min) return(7);
   p = min;
   while(*p && *p != '.' && *p != '*') p++;   
   if (!*p && max)
   {
      p = max;
      while(*p && *p != '.' && *p != '*') p++;
   }   
   if (!*p)       return(0);
   if (*p == '*') return(7);
   
   t = 0;
   p++;
   while(*p && *p != '*' && *p != '?' && *p != '!') {p++;t++;}
   if (*p == '*' || *p == '!') return(7);
   if (*p == '?') 
   {
      if (asigna_decimales)
      {
         while(*p == '?') p++;
         return((*asigna_decimales)(p));
      }
      return(0);   
   }
   return(t);
}


/* decimales  devuelve el numero de decimales de un string
*            o 0 si no hay
*/
int decimales(s)
char *s;
{
   char *i = s;
   int n;
   
   while(*i && *i != '.') i++;
   if (!*i) return(0);
   n = 0;
   i++;
   if (*i == '*') return(1);
   if (*i == '?' || *i == '!')
			{
      if (asigna_decimales)
      {
         while(*i == '?' || *i == '!') i++;
         return((*asigna_decimales)(i));
      }
      return(0);
			}
   while(*i && *i != '*') {i++;n++;}
   return(n);
}


int quitadec(tmp)
char *tmp;
{
			int i;
         for (i=strlen(tmp)-1;;i--) {
            if (tmp[i] == '.') {
               tmp[i]='\0';
               return(0);
            }
            if (tmp[i] == '0')
               tmp[i]='\0';
            else
               return(0);
         }
}


