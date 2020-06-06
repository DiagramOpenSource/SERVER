

/* Autor: Francisco Gonzalez Valiente */
#include <fgvsys.h>

#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef O_BINARY
#include <io.h>
#endif
#include <string.h>
#include <malloc.h>

#include <rwmalloc.h>
#include <rdef.h>

#include <stdlib.h>
/*long atol();*/

#ifndef O_BINARY
#define O_BINARY 0
#define SEEK_SET 0
#define SEEK_CUR 1
#ifndef LARGESEEK
extern long rw_lseek_();
#endif
extern unsigned int rw_read_();
#endif


#ifdef RWXARXA
extern FILE *t_rwfopen();
extern char *t_lee_linea();
extern int   t_rwfclose();
#else
extern char *lee_linea();
#define t_rwfopen   fopen
#define t_lee_linea lee_linea
#define t_rwfclose  fclose
#endif


/*  --------------------------------------------------------------
*  lee_dir
*  Lee todo el fichero de directorio 'nme'+".dir" donde se encuentra
*  la lista de ficheros .def y los guarda en las estructuras a las
*  cuales apuntan los elementos de 'pme[]'
*  y devuelve el numero de ficheros def existentes.
*  En caso de error devuelve
*  -1 si el fichero .dir no es accesible o no existe.
*  Si tipo = 0 guarda todos,
*            -1 los de listados
*            -2 indexados
*            >0 tipo de mantenimiento
*  --------------------------------------------------------------
*/
int lee_dir(nme,pme,ptitulo,tipo)
char *nme;
MENU_DEF *pme[];
char *ptitulo[];
int tipo;
{
   FILE *file1;
   char nomfi[MAXPATH];
   char texto[128];
   int mientras = -1;
   int contador = 0;
   int eslista,esmante;
   int lock;
   /*   char c;*/
   
   strcpy(nomfi,nme);
   strcat(nomfi,".dir");
   
   lock = tipo / 100;
   tipo %= 100;
   
   
   if (lock == 2) return(1);
   
   
   
   if (( file1 = t_rwfopen(nomfi,"r")) == NULL)
   {
      if (lock)
      {
         file1 = fopen(nomfi,"w");
         if (file1)
         {
            fclose(file1);
            return(0);
         }
      }
      return(-1);                   /* fichero .dir no accesible */
   }
   
   while (mientras) {
      if (t_lee_linea(texto,82,file1) == NULL) {
         t_rwfclose(file1);
         mientras = 0;
         return(contador);
      }
      else 
      {
         
         if (memcmp(texto,"LOCK",4) == 0) continue;
         
         pme[contador] = (MENU_DEF *)malloc(sizeof(MENU_DEF));
         strcpy(pme[contador]->m_nombre,texto);
         t_lee_linea(texto,82,file1);
         texto[30] = 0;
         strcpy(pme[contador]->m_titulo,texto);
         if (ptitulo)
            ptitulo[contador] = pme[contador]->m_titulo;
         t_lee_linea(texto,82,file1);
         texto[10] = 0;
         strcpy(pme[contador]->m_passwd,texto);
         t_lee_linea(texto,82,file1);
         esmante = pme[contador]->m_manten =(int) (texto[0]-'0');
         eslista = pme[contador]->m_lista  =(int) (texto[1]-'0');
         t_lee_linea(texto,82,file1);
         pme[contador]->m_fichas = atol(texto);
         if ( (tipo==0)
            || (tipo == -2 && (esmante == 1 || esmante == 2))
            || (tipo>0 && tipo==esmante)
            || (tipo==-1 && eslista==0) )
            contador++;
         else
            free(pme[contador]);
      }
   }
   t_rwfclose(file1);
   return(contador);
}


int ordena_dir_(MENU_DEF **dird,int numfi)
{
   int bloques = 0;
   char *ordenado[501];
   int referencia[501];
   MENU_DEF *orden[501];
   int ordenados = 0;
   int i,k,j,l;
   char *p1;
   long mi_ficha;
   
   if (!numfi) return(0);
   
   for (i = 0;i < numfi;i++)
			{
      k = strbusca(dird[i]->m_titulo,"#");
      if (k > -1)	    		
      {	  
         k++;
         quitabp(dird[i]->m_titulo+k);
         quitab(dird[i]->m_titulo+k);
         for (j = 0;j < ordenados;j++)
         {
            if (!ordenado[j][0] || strcmp(dird[i]->m_titulo+k,ordenado[j]) < 0)
            {
               break;
            }
         }
         for (l = ordenados;l > j;l--)
         {
            ordenado[l] = ordenado[l-1];
            referencia[l] = referencia[l-1];
         }
         ordenados++;
         ordenado[j] = dird[i]->m_titulo+k;
         referencia[j] = i;
      }
      else
      {
         ordenado[ordenados] = "";
         referencia[ordenados] = i;
         ordenados++;
      }	  
			}
   bloques = 0;
   mi_ficha = 0;
   p1 = ordenado[0];
   if (p1[0])
			{
      bloques = 1;
      mi_ficha = 1;
			}
   for (i = 0;i < ordenados;i++)
			{
      if (!ordenado[i][0]) break;
      if (strcmp(p1,ordenado[i]))
      {
         mi_ficha++;
         p1 = ordenado[i];
         bloques++;
      }
      dird[ referencia[i] ]->m_fichas = mi_ficha;
			}
   if (i < ordenados)
			{
      mi_ficha++;
      bloques++;
      j = 0;
      for (;i < ordenados;i++)
      {
         j++;
         if (j > 20)
         {
            j = 1;
            mi_ficha++;
            bloques++;
         }
         dird[ referencia[i] ]->m_fichas = mi_ficha;	   
      }
			}
   
   /* ordenar !! */
   for (i = 0;i < numfi;i++)
			{
      orden[i] = dird[ referencia[i] ];
			}
   for (i = 0;i < numfi;i++)
			{
      dird[i] = orden[i];
			}
   
   return(bloques);
}


