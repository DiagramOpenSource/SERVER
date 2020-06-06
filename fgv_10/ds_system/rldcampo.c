
/*---------------------------------------------------------------------------
* Programa ..: rldcampo.c
* Fecha .....:
* Version ...:
* Notas .....: Rutina para leer campos de un fichero
* Contenido .: int  ld_campo(),
*---------------------------------------------------------------------------*/

#include <fgvsys.h>

#include <memory.h>

#define _NUEVODEF
#include <rdef.h>      /* ya incluye el isam.h */

/*
* ld_campo
* lee un campo del registro (origen) y lo pone en el string (destino)
* el fichero corresponde a una estructura tipo pfi
* origen es el buffer del registro correspondiente a pfi
*/

extern int fgv_ver_euro;
extern int modo_otro_euro;
extern int (*asigna_euro)();


ld_campo(campo,pfi,destino)
int campo;
register FI_DEF *pfi;
char *destino;
{
   char *origen;
   int tipo,offset,longit,i;
   char *usi,*min,*max;
   char t[MAXDEFECTO],usi_2[20];
   double numd;
   
   origen = pfi->f_buf;
   tipo = pfi->f_campos[campo].c_tipo;
   offset = pfi->f_campos[campo].c_offset;
   longit = pfi->f_campos[campo].c_long;
   usi    = recalcula_usi(pfi,campo);
   min    = pfi->f_campos[campo].c_minimo;
   max    = pfi->f_campos[campo].c_maximo;
   switch (tipo) {
   case CHARTIPO :
      ldchar(origen+offset,longit,t);
      t[longit] = '\0';
      /******************************
      sprintf(destino,usi,t);
      *******************************/
      memset(destino,' ',longit);
      destino[longit] = '\0';
      memcpy(destino,t,strlen(t));
      break;
   case FECHATIPO :
      ldfecha(origen+offset,destino);
      break;
   case INTTIPO :
      sprintf(destino,usi,ldint(origen+offset));
      break;
   case LONGTIPO :
      sprintf(destino,usi,ldlong(origen+offset));
      break;
   case DOUBLETIPO :
      numd = lddbl(origen+offset);

		if (modo_otro_euro && fgv_ver_euro)
		{
			int l = 0;
			for (i = 0;min[i];i++)
			{
			   if (min[i] == '.') l++;
			   if (l && (min[i] == '?' || min[i] == '!')) {break;}
			}
			if (min[i]) 
			{
				i++;
				while(min[i] == '?') i++;
				if (asigna_euro)
				{							 
					 (*asigna_euro)(min+i,&numd);
				}
			}
		}


      /* longit = decimales_redondeo_campo(min,max); */
      sprintf(destino,usi,numd);
      /*i = strlen(min);*/
      /* SISTEMA DE DECIMALES VARIABLES */
      if (decimales_variables(min)) 
      {
         i = strlen(destino);
         sprintf(t,"%lf",numd);
         quitadec(t); /* siempre hay decimales (.0000) */
         sprintf(usi_2,"%%%d.%dlf",i,decimales(t));
         sprintf(destino,usi_2,numd);
      }
      break;
   default:
      break;
   }
   return(0);
}



/*
* ld_campo_nofecha
* lee un campo del registro (origen) y lo pone en el string (destino)
* el fichero corresponde a una estructura tipo pfi
* origen es el buffer del registro correspondiente a pfi
* Las fechas se tratan como alfanumericos de 8 (orden de fecha interno)
*/

ld_campo_nofecha(campo,pfi,destino)
int campo;
register FI_DEF *pfi;
char *destino;
{
   char *origen;
   int tipo,offset,longit,i;
   char *usi,*min,*max;
   char t[MAXDEFECTO],usi_2[20];
   double numd;
   
   origen = pfi->f_buf;
   tipo = pfi->f_campos[campo].c_tipo;
   offset = pfi->f_campos[campo].c_offset;
   longit = pfi->f_campos[campo].c_long;
   usi    = recalcula_usi(pfi,campo);
   min    = pfi->f_campos[campo].c_minimo;
   max    = pfi->f_campos[campo].c_maximo;
   switch (tipo) {
   case CHARTIPO :
      ldchar(origen+offset,longit,t);
      t[longit] = '\0';
      /******************************
      sprintf(destino,usi,t);
      *******************************/
      memset(destino,' ',longit);
      destino[longit] = '\0';
      memcpy(destino,t,strlen(t));
      break;
   case FECHATIPO :
	  ldchar(origen+offset,8,destino);      
      break;
   case INTTIPO :
      sprintf(destino,usi,ldint(origen+offset));
      break;
   case LONGTIPO :
      sprintf(destino,usi,ldlong(origen+offset));
      break;
   case DOUBLETIPO :
      numd = lddbl(origen+offset);
      /* longit = decimales_redondeo_campo(min,max); */
		if (modo_otro_euro && fgv_ver_euro)
		{
			int l = 0;
			for (i = 0;min[i];i++)
			{
			   if (min[i] == '.') l++;
			   if (l && (min[i] == '?' || min[i] == '!')) {break;}
			}
			if (min[i]) 
			{
				i++;
				while(min[i] == '?') i++;
				if (asigna_euro)
				{							 
					 (*asigna_euro)(min+i,&numd);
				}
			}
		}
      sprintf(destino,usi,numd);
      /*i = strlen(min);*/
      /* SISTEMA DE DECIMALES VARIABLES */
      if (decimales_variables(min)) 
      {
         i = strlen(destino);
         sprintf(t,"%lf",numd);
         quitadec(t); /* siempre hay decimales (.0000) */
         sprintf(usi_2,"%%%d.%dlf",i,decimales(t));
         sprintf(destino,usi_2,numd);
      }
      break;
   default:
      break;
   }
   return(0);
}
