

/*---------------------------------------------------------------------------
* Programa ..: rquita1.c
* Fecha .....:
* Version ...:
* Notas .....: Rutina de eliminacion de blancos y newlines
*              del final de strings
* Contenido .: void quitab(),quitabp(),quitan()
*---------------------------------------------------------------------------*/

#include <fgvsys.h>
#include <teclas.h>

/*
* quitab
*    quitar los blancos de final de string
*/
void quitab(s)
char *s;
{
   register char *r;
   
   for (r=s;(*r);r++);
   while (r > s) {
      r--;
      if (*r != ' ' && *r != 9) 
      {
         *(r+1) = 0;
         break;
      }
      if (r == s)
         *r = 0;
   }
   
}

/*
* quitabp
*    quitar los blancos de principio del string
*/
void quitabp(s)
char *s;
{
   register char *c;
   
   for (c=s;(*c == ' ' || *c == 9);c++);
   while ((*s++=*c++) != '\0');
}

/*
* quitan
*    quitar los \n de final de string
*/
void quitan(s)
char *s;
{
 		 register char *r;
         
         for (r=s;(*r);r++);
         while (r > s) {
            r--;
            if (*r != '\n') {
               *(r+1) = 0;
               break;
            }
            if (r == s)
               *r = 0;
         }      
         
         for (r=s;(*r);r++);
         while (r > s) {
            r--;
            if (*r != '\r') {
               *(r+1) = 0;
               break;
            }
            if (r == s)
               *r = 0;
         }     
         
/*
         for (r=s;(*r);r++);
         while (r > s) {
            r--;
            if (*r != '\n') {
               *(r+1) = 0;
               break;
            }
            if (r == s)
               *r = 0;
         }
*/       
}

/*  =========================================================================
quita_blancos(p)
========================================================================*/

void quita_blancos(s)
char *s;
{
   int i = 0;
   
   do {
      while (*(s + i) == ' ' || *(s + i) == '\011') i++;
      *s = *(s + i);
      if (*s) s++;
   } while(*s);
   
}


void quita_char(s,c)
char *s;
char c;
{
   int i = 0;
   
   do {
      while (*(s + i) == c) i++;
      *s = *(s + i);
      if (*s) s++;
   } while(*s);
   
}


void reemplaza_char(s,o,d)
char *s;
char o,d;
{      
   while(*s)
   {
	   if (*s == o)
		   *s = d;
	   s++;
   }   
}


void cambia_tabs(char *s)
{
   int i,n;
   
   for (i = strlen(s)-1;i > -1;i--) {
						if (s[i] == '\011') {
                     for (n = strlen(s);n > i;n--)
                        s[n+ETAB-1] = s[n];
                     memset(s+i,' ',ETAB);
                  }
   }
}

static int general = 0;

int comas_linea(texto,t,m,p)
char *texto;
int t,m,*p;
{
   int ii;
   register int jj;
   
   if (p == (int *)0)
						ii = general;
   else
						ii = *p;
   
   if (t == 0) {                     /* primer elemento */
						ii = 0;
   }
   for (jj=ii;(texto[ii] != ',' && texto[ii] != '\0');ii++);
   
   if (texto[ii] != '\0') {  /* hay mas */
						texto[ii] = '\0';
                  ii++;
   }
   
   if (m) quitab(texto+jj);    /* quita blancos de detras  */
   
   
   if (p == (int *)0)
						general = ii;
   else
						*p = ii;
   
   return(jj);
}

