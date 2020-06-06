
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


int es_euro(char *nom) /* invertido */
{
	if (strlen(nom) < 4) return -1;
    return memcmp(nom,"EURO",4);
}

int decimales_variables(char *min)
{
	int i = strlen(min);
	if (i > 1 && min[i-2] == '.' && min[i-1] == '*')
		return 1;
	if (min[i-2] == 'O' && min[i-1] == '*')
	{
		while(*min && *min != '?')
			min++;
		if (*min == '?')
			min++;
		if (!es_euro(min))
		{
			return 1;
		}
	}
	return 0;
}


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

	  /* defectos segun permisos !!! */
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


esdefecto_csficha(pfi,k,k1)
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
   char tmp[512];
   
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
   for (i=ini;i<fin;i++) 
   {
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
       ldchar(buf+offset,longit,tmp);
	   if (strcmp(tmp,defecto))
	   {
		   return -1;
	   }
       break;
    case FECHATIPO :
       if (atoi(defecto) == 0) 
	   {
          defecto = fecha();
       }
       ldfecha(buf+offset,tmp);	   
	   if (strcmp(tmp,defecto))
	   {
		   return -1;
	   }
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
		   if (v != ldlong(buf+offset))
		   {
			   return -1;
		   }
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
		   if (v != ldint(buf+offset))
		   {
			   return -1;
		   }
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
		   if (v != lddbl(buf+offset))
		   {
			   return -1;
		   }
		}
       break;
    default:
       break;
      }
   }
   return(0);
}
