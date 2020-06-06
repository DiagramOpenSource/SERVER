
/*---------------------------------------------------------------------------
* Programa ..: rimpre.c
* Fecha .....: 24-02-88
* Version ...: 1.00
* Notas .....: Rutinas para el tratamiento de impresoras
* Contenido .: int  i_leeimpre(), i_oncomprime(), i_offcomprime(),
*                   i_onnegrita(), i_offnegrita(),
*                   i_onexpande(), i_offexpande(), 
*---------------------------------------------------------------------------*/

#include <fgvsys.h>

#include <stdio.h>
#include <string.h>

unsigned char nombredev[81],           /* nombre */
oncompri[17],             /* secuencia para comprimir */
offcompri[17],            /* secuencia para descomprimir */
onnegrita[17],            /*   "            negrita */
offnegrita[17],           /*   "            quitar negrita */
onexpande[17],            /*   "            expandido */
offexpande[17],           /*   "            quitar expandido */
inicializo[37],
finalizo[37],
saltopag[17],
otrocar[17];

char *impre_ccont = oncompri;
int impre_nccont = 0;

int i_nlineas;
int i_anchocarro;

extern FILE *_imp; /* canal de la ultima impresora abierta */

int es_esclava = 0;

extern int atento_imp;

unsigned char no_reset = 0;

extern char *lee_linea();

char queimpresora[512] = {0,};

/* i_leeimpre  lee los datos de la impresora del fichero de nombre 'f'
* poniendolos en las variables estaticas siguientes
* nombreimp , anchocarro , oncompri , offcompri , onnegrita , offnegrita
* devuelve el ancho de la impresora (80,132) o -1 si da error
*/
int i_leeimpre(f)
char *f;
{
   FILE *file1;
   char nomfi[80];
   char texto[82];
   char *tmp;
   int i;
   
   if (f == (char *)1)
   {
      /* Inicializar con valores por defecto para windows */
      strcpy(nombredev,"{agixl97.dll}");
      i_anchocarro  = 250;
      oncompri[0]   = 0;
      oncompri[15]  = 0;
      offcompri[0]  = 0;
      offcompri[15] = 0;
      onnegrita[0]  = 0;
      onnegrita[15] = 0;
      offnegrita[0] = 0;
      offnegrita[15]= 0;
      onexpande[0]  = 0;
      onexpande[15] = 0;
      offexpande[0] = 0;
      offexpande[15]= 0;
      inicializo[0] = 0;
      inicializo[35] = 0;
      finalizo[0] = 0;             
      finalizo[35] = 0;	  
      saltopag[0] = 0;
      saltopag[15] = 0;
      otrocar[0] = 0;
      otrocar[15] = 0;
      i_nlineas  = 99;
      no_reset   = 1;
      es_esclava = 0;
      return(i_anchocarro);
   }
   
   
   if (!f || !f[0])
   {
      /* Inicializar con valores por defecto para windows */
      nombredev[0]  = 0;
      i_anchocarro  = 80; /* variable segun formato de papel */
      oncompri[0]   = 139;
      oncompri[15]  = 1;
      offcompri[0]  = 140;
      offcompri[15] = 1;
      onnegrita[0]  = 141;
      onnegrita[15] = 1;
      offnegrita[0] = 142;
      offnegrita[15]= 1;
      onexpande[0]  = 143;
      onexpande[15] = 1;
      offexpande[0] = 144;
      offexpande[15]= 1;
      inicializo[0] = 0;
      inicializo[35] = 0;
      finalizo[0] = 0;             
      finalizo[35] = 0;	  
      strcpy(saltopag,"#FIN#\n");
      saltopag[15] = strlen(saltopag);
      otrocar[0] = 0;
      otrocar[15] = 0;
      i_nlineas  = -1;
      no_reset   = 1;
      es_esclava = 0;
      return(i_anchocarro);
   }

   strcpy(queimpresora,quita_path(f));
   strcpy(nomfi,f);
   strcat(nomfi,".prn");
   if (( file1 = fopen(nomfi,"r")) == (FILE *) 0)
      return(-1);
   lee_linea(texto,82,file1);
   lee_linea(texto,82,file1);
   strcpy(nombredev,texto);
   lee_linea(texto,82,file1);
   i_anchocarro = atoi(texto);
   for (i=0;i<15;i++) {
      lee_linea(texto,82,file1);
      oncompri[i] = (char) atoi(texto);
   }
   oncompri[i] = '\0';
   for (i=0;i<15;i++) {
      lee_linea(texto,82,file1);
      offcompri[i] = (char) atoi(texto);
   }
   offcompri[i] = '\0';
   for (i=0;i<15;i++) {
      lee_linea(texto,82,file1);
      onnegrita[i] = (char) atoi(texto);
   }
   onnegrita[i] = '\0';
   for (i=0;i<15;i++) {
      lee_linea(texto,82,file1);
      offnegrita[i] = (char) atoi(texto);
   }
   offnegrita[i] = '\0';
   for (i=0;i<15;i++) {
      lee_linea(texto,82,file1);
      onexpande[i] = (char) atoi(texto);
   }
   onexpande[i] = '\0';
   for (i=0;i<15;i++) {
      lee_linea(texto,82,file1);
      offexpande[i] = (char) atoi(texto);
   }
   offexpande[i] = '\0';
   for (i=0;i<15;i++) {
      lee_linea(texto,82,file1);
      inicializo[i] = (char) atoi(texto);
   }
   inicializo[i] = '\0';
   for (i=0;i<15;i++) {
      lee_linea(texto,82,file1);
      finalizo[i] = (char) atoi(texto);
   }
   finalizo[i] = '\0';
   lee_linea(texto,82,file1);
   i_nlineas = atoi(texto);
   
   for (i=15;i<35;i++) {
      if (!lee_linea(texto,82,file1))
         inicializo[i] = 0;
      else
         inicializo[i] = (char) atoi(texto);
   }
   inicializo[34] = 0;
   for (i=15;i<35;i++) {
      if (!lee_linea(texto,82,file1))
         finalizo[i] = 0;
      else
         finalizo[i] = (char) atoi(texto);
   }
   finalizo[34] = 0;
   for (i=0;i<15;i++) {
      if (!lee_linea(texto,82,file1))
         saltopag[i] = 0;
      else
         saltopag[i] = (char) atoi(texto);
   }
   saltopag[i] = 0;
   for (i=0;i<15;i++) {
      if (!lee_linea(texto,82,file1))
         otrocar[i] = 0;
      else
         otrocar[i] = (char) atoi(texto);
   }
   otrocar[i] = 0;
   
   
   for (i = 0;i < 10;i++)
   {
      if (!lee_linea(texto,82,file1)) break;
      switch(i)
      {
      case 0:
         tmp = oncompri;
         break;
      case 1:
         tmp = offcompri;
         break;
      case 2:
         tmp = onnegrita;
         break;
      case 3:
         tmp = offnegrita;
         break;
      case 4:
         tmp = onexpande;
         break;
      case 5:
         tmp = offexpande;
         break;
      case 6:
         tmp = inicializo;
         break;
      case 7:
         tmp = finalizo;
         break;
      case 8:
         tmp = saltopag;
         break;
      case 9:
         tmp = otrocar;
         break;
      default:
         break;
      }
      if (tmp == inicializo || tmp == finalizo)
         tmp[35] = atoi(texto);
      else
         tmp[15] = atoi(texto);
   }
   if (i < 10)
   {
      for (i = 0;i < 10;i++)
      {
         switch(i)
         {
         case 0:
            tmp = oncompri;
            break;
         case 1:
            tmp = offcompri;
            break;
         case 2:
            tmp = onnegrita;
            break;
         case 3:
            tmp = offnegrita;
            break;
         case 4:
            tmp = onexpande;
            break;
         case 5:
            tmp = offexpande;
            break;
         case 6:
            tmp = inicializo;
            break;
         case 7:
            tmp = finalizo;
            break;
         case 8:
            tmp = saltopag;
            break;
         case 9:
            tmp = otrocar;
            break;
         default:
            break;
         }
         if (tmp == inicializo || tmp == finalizo)
            tmp[35] = strlen(tmp);
         else
            tmp[15] = strlen(tmp);
      }
   }
   if (!lee_linea(texto,82,file1))
      no_reset = 0;
   else
      no_reset = atoi(texto);
   if (!lee_linea(texto,82,file1))
      es_esclava = 0;
   else
      es_esclava = atoi(texto);
   fclose(file1);
   return(i_anchocarro);
}

/* i_oncomprime  pone en comprimido
* devuelve 0 si va bien , -1 en caso contrario
*/
int i_oncomprime(file)
FILE *file;
{
   _graba_linea(oncompri,file,16);
   return(0);
}

/* i_offcomprime  quita el comprimido
* devuelve 0 si va bien , -1 en caso contrario
*/
int i_offcomprime(file)
FILE *file;
{
   _graba_linea(offcompri,file,16);
   return(0);
}

/* i_onnegrita  pone en negrita
* devuelve 0 si va bien , -1 en caso contrario
*/
int i_onnegrita(file)
FILE *file;
{
			_graba_linea(onnegrita,file,16);
         return(0);
}

/* i_offnegrita  quita la negrita
* devuelve 0 si va bien , -1 en caso contrario
*/
int i_offnegrita(file)
FILE *file;
{
   _graba_linea(offnegrita,file,16);
   return(0);
}

/* i_onexpande  pone en expandido
* devuelve 0 si va bien , -1 en caso contrario
*/
int i_onexpande(file)
FILE *file;
{
   _graba_linea(onexpande,file,16);
   return(0);
}

/* i_offexpande  quita el expandido
* devuelve 0 si va bien , -1 en caso contrario
*/
int i_offexpande(file)
FILE *file;
{
   _graba_linea(offexpande,file,16);
   return(0);
}

int i_inicio(file)
FILE *file;
{
   _graba_linea(inicializo,file,36);
   
   if ((unsigned long)file > (unsigned long)0x00000004L)
      fflush(file);
}

int i_acabo(file)
FILE *file;
{
   _graba_linea(finalizo,file,36);
   if ((unsigned long)file > (unsigned long)0x00000004L)
      fflush(file);
}

int i_saltopag(file)
FILE *file;
{
   if (*saltopag == 0 || *saltopag == ' ')
      return(-1);
   _graba_linea(saltopag,file,16);
   
   if ((unsigned long)file > (unsigned long)0x00000004L)
      fflush(file);
   return(0);
}


int i_otrocar(file)
FILE *file;
{
   _graba_linea(otrocar,file,16);
   if ((unsigned long)file > (unsigned long)0x00000004L)
      fflush(file);
}

int i_reset(file)
FILE *file;
{
   if (no_reset) return(0);
   _graba_linea(offcompri,file,16);
   _graba_linea(offnegrita,file,16);
   _graba_linea(offexpande,file,16);
   return(0);
}

char *i_nombre()
{
   return(nombredev);
}

int impre_quecontrol(c)
int c;
{
   switch (c) {
   case 2:
      impre_ccont = offcompri;
      impre_nccont = offcompri[15];
      break;
   case 3:
      impre_ccont = onnegrita;
      impre_nccont = onnegrita[15];     
      break;
   case 4:
      impre_ccont = offnegrita;
      impre_nccont = offnegrita[15];     
      break;
   case 5:
      impre_ccont = onexpande;
      impre_nccont = onexpande[15];     
      break;
   case 6:
      impre_ccont = offexpande;
      impre_nccont = offexpande[15];     
      break;
   case 7:
      impre_ccont = saltopag;
      impre_nccont = saltopag[15];     
      break;
   case 8:
      impre_ccont = otrocar;
      impre_nccont = otrocar[15];     
      break;
   default:
      impre_ccont = oncompri;
      impre_nccont = oncompri[15];     
      break;
   }
   return(0);
}

