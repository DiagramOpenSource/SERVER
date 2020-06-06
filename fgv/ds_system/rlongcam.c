
/* Autor: Francisco Gonzalez Valiente */
#include <fgvsys.h>

#include <string.h>

#define _NUEVODEF
#include <rdef.h>

#define min(a,b)  (((a) < (b)) ? (a) : (b))
#define max(a,b)  (((a) > (b)) ? (a) : (b))


int long_campo(pfi,i)
FI_DEF *pfi;
int i;
{
   int largo;
   
   switch(pfi->f_campos[i].c_tipo) {
   case CHARTIPO :
      largo = pfi->f_campos[i].c_qc;
      break;
   case FECHATIPO :
      largo = 10;
      break;
   case LONGTIPO:
      if (pfi->f_campos[i].c_qc == -2) { /* caso memo */
         largo = (atoi(pfi->f_campos[i].c_maximo) % 100);
         break;
      }
   default :
      largo = len_numerico(pfi->f_campos[i].c_minimo,pfi->f_campos[i].c_maximo);
      break;
   }
   return(largo);
}


int long_campo_nofecha(pfi,i)
FI_DEF *pfi;
int i;
{
   int largo;
   
   switch(pfi->f_campos[i].c_tipo) {
   case CHARTIPO :
      largo = pfi->f_campos[i].c_qc;
      break;
   case FECHATIPO :
      largo = 8;
      break;
   case LONGTIPO:
      if (pfi->f_campos[i].c_qc == -2) { /* caso memo */
         largo = (atoi(pfi->f_campos[i].c_maximo) % 100);
         break;
      }
   default :
      largo = len_numerico(pfi->f_campos[i].c_minimo,pfi->f_campos[i].c_maximo);
      break;
   }
   return(largo);
}


int deci_campo(pfi,i)
FI_DEF *pfi;
int i;
{
   int largo;
   int l;
   char *min;
   
   switch(pfi->f_campos[i].c_tipo) {
			case CHARTIPO :
            largo = 0;
            break;
         case FECHATIPO :
            largo = 0;
            break;
         case LONGTIPO:
            largo = 0;
            break;     
         default :
            min = pfi->f_campos[i].c_minimo;
            largo = max(decimales(pfi->f_campos[i].c_maximo),decimales(min));
            l = strlen(min);
            if (l > 1 && min[l-2] == '.' && min[l-1] == '*') 
            {
               largo = 7;
            }
            break;
			}
   return(largo);
}


