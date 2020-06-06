
/*---------------------------------------------------------------------------
* Programa ..: rdefecfi.c
* Fecha .....: 17-12-87
* Version ...: 1.00
* Notas .....: Rutina para poner a valores por defecto el registro 
* Contenido .: int  defecto_ficha(), 
*---------------------------------------------------------------------------*/


#include <fgvsys.h>

#define _NUEVODEF
#include <rdef.h>      /* ya incluye el isam.h */

#include <stdlib.h>
/*extern long atol();*/
extern char *fecha();

/*
* defecto_ficha
* pone a los valores de defecto la variable de buffer que contiene una ficha
* segun la estructura del fichero pfi
*/
defecto_csficha(pfi,k,k1)
register FI_DEF *pfi;
int k,k1;
{
   int i;
   int tipodato,
      offset,
      longit;
   char *defecto;
   char *buf;
   char *cmin,*cmax;
   int ini,fin;
   
   if (k1 < 0 || k < 0 || k1 > pfi->f_ncampos || k >= pfi->f_ncampos)
   {
      ini = 0;
      fin = pfi->f_ncampos;
   }
   else
   {
      ini = k;
      fin = k1;
   }
   
   buf = pfi->f_buf;
   for (i=ini;i<fin;i++) {
      tipodato = pfi->f_campos[i].c_tipo;
      offset = pfi->f_campos[i].c_offset;
      longit = pfi->f_campos[i].c_long;
      defecto = pfi->f_campos[i].c_mante.c_defecto;
	  cmin = pfi->f_campos[i].c_minimo;
	  cmax = pfi->f_campos[i].c_maximo;
      if (pfi->f_campos[i].c_key != k)
         switch (tipodato) {
    case CHARTIPO :
       if (*defecto == '#' || *defecto == '@')
          defecto = "";
       stchar(defecto,buf+offset,longit);
       break;
    case FECHATIPO :
       if (atoi(defecto) == 0) {
          stfecha(fecha(),buf+offset);
       }
       else
          stfecha(defecto,buf+offset);
       break;
    case LONGTIPO :
		{
			long v = atol(defecto);
#ifdef CONMINDEFECTO
			if (cmin && v < atol(cmin))
				v = atol(cmin);
			if (cmax && v > atol(cmax))
				v = atol(cmax);
#endif
			stlong(v,buf+offset);
		}
       break;
    case INTTIPO :
		{
			int v = atoi(defecto);
#ifdef CONMINDEFECTO
			if (cmin && v < atoi(cmin))
				v = atoi(cmin);
			if (cmax && v > atoi(cmax))
				v = atoi(cmax);			
#endif
			stint(v,buf+offset);
		}
       break;
    case DOUBLETIPO :
		{
			double v = rwatof(defecto);
#ifdef CONMINDEFECTO
			if (cmin && v < rwatof(cmin))
				v = rwatof(cmin);
			if (cmax && v > rwatof(cmax))
				v = rwatof(cmax);			
#endif
			stdbl(v,buf+offset);
		}
       break;
    default:
       break;
      }
   }
   return(0);
}

defecto_sficha(pfi,k)
register FI_DEF *pfi;
register int k;
{
   return(defecto_csficha(pfi,k,-1));
}

defecto_ficha(pfi)
register FI_DEF *pfi;
{
			return(defecto_csficha(pfi,-13,-1));
}
