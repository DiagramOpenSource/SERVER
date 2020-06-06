

/* Autor: Francisco Gonzalez Valiente */
#include <fgvsys.h>

#include <stdio.h>
#include <string.h>


char caracter_de_fecha = '.';

/* funciones de string que no son del todo habituales en las librerias
del sistema
*/

int strmayus(s)
char *s;
{
#ifdef ESDOS
   strupr(s);
#else
   while (*s != '\0') {
      if (*s > 96 && *s < 123)
         *s -= 32;
      s++;
   }
#endif
   return(0);
}

int strminus(s)
char *s;
{
#ifdef ESDOS
   strlwr(s);
#else
   while (*s != '\0') {
      if (*s > 64 && *s < 91)
         *s += 32;
      s++;
			}
#endif
   return(0);
}

int strbusca(s1,s2)
char *s1;
char *s2;
{
/*
#ifdef ESDOS
			char *r;
         if ((r = strstr(s1,s2)) != NULL) 
		 {
			 return((int)(r - s1));
		 }
#else
*/         
         int i = 0,j;
         while (*s1 != '\0') {
            j = 0;
            while (*(s1+j) != '\0') {
               if (*(s1+j) != *(s2+j)) break;
               j++;
               if (*(s2+j) == '\0') return(i);
            }
            i++;
            s1++;
         }
         
/*endif*/
         return(-1);
}

/* Anade el string c delante del string resultado
* Ej. resultado[10]="mama" , c[5]="hola" Resultado = "holamama"
*/

int strcatd(resultado,c)
char *resultado;
char *c;
{
   int c0,ps;
   c0=strlen(c);
   for ( ps=strlen(resultado);ps>=0;ps-- )
      *(resultado+ps+c0)=*(resultado+ps);
   for ( ps=0;ps<c0;ps++ )
      *(resultado+ps)=*(c+ps);
   return(0);
}

/*---------------------------------------------------------------------------*/


/* vals devuelve el valor (val) numerico de una porcion de una tira de
* caracteres a la que apunta "tira" desde la "posicion" (tira+posicion)
* hasta contar tantos caracteres com "desplazamiento" se le indique.
* Ej. a[9]="123456789", vals(a,4,2) devuelve un long con 56 .
*/

double valsd(tira,posicion,desplazamiento)
char *tira;
int posicion;
int desplazamiento;
{
   char t[11];
   int ps;
   for (ps=0;ps<desplazamiento;ps++)
						t[ps]=*(tira+posicion+ps);
   t[ps]='\0';
   return(rwatof(t));
}

long vals(tira,posicion,desplazamiento)
char *tira;
int posicion;
int desplazamiento;
{
   return((long)valsd(tira,posicion,desplazamiento));
}

int strlen_limpio(char *str) 
{
    int n = 0;
	while(*str)
    {
		if (*str > 32)
			n++;
		str++;
	}
	return n;
}

int len_numerico(min,max) /* da la longitud de un campo numerico definido por */
char *min,*max;           /* su valor maximo y minimo */
{
   int i,j,x;
   while ((*min) == ' ') min++;
   while ((*max) == ' ') max++;
   i = strlen_limpio(max);
   j = strlen_limpio(min);
   if (j > 1 && min[j-2] == '.' && min[j-1] == '*')
   {
      j -= 2;
   }
   else
   {
      if (j)
      {
		   x = j - 1;
           while(x)
           {
              if ((min[x] == '?' || min[x] == '!') && min[x-1] == '.')
              {
                 j = x;
                 break;
              }
              x--;
           }
      }
   }
   if (i < j)
      return(j);
   return(i);
}

int es_numero(s) /* devuleve -1 si el string es un numero o vacio */
char *s;
{
   while(*s)
   {
      if (*s < '0' || *s > '9')
         return(0);
      s++;
   }
   return(-1);
}


char *quita_path(ff)
char *ff;
{
   char *f0 = ff;
   ff += strlen(ff);
   while (ff >= f0 && *ff != '\\' && *ff != '/') ff--;
   ff++;
#ifdef FGVDOS
   if (ff == f0 && *f0 && *(f0+1) == ':')
   {
      ff += 2;
   }
#endif
   return(ff);
}

char *cambia_espacios(char *str, int modo) {
	char *_str = str;
	if (modo == 1)
		while (*str) {		
			if (*str == 23)
				*str = ' ';
			str++;
		}
	else if (modo == 0)
		while (*str) {		
			if (*str == ' ')
				*str = 23;
			str++;
		}
	return _str;
}

/* tabs == espacios !! */
int _separa(str,p,m,tope)
char *str;
char **p;
int m;
int tope;
{
   int n, fin, i = 0;
   
   fin = strlen(str);
   if (fin == 0) return(-1);
   for (n = 0; n < fin; n++) {
      if (*(str + n) == ' ' || *(str + n) == 9) {
         *(str + n) = '\0';
         while(n < fin && (*(str + n + 1) == ' ' || *(str + n + 1) == 9)) n++;
         *(p + i) = (str + n + 1);
         ++i;
         if (i == tope || m == 0) break;
      }
   }
   if (i > 0) {
      fin = strlen(*(p+i-1));
      for (n = i;n < tope;n++) {
         *(p + n) = *(p+i-1) + fin;
      }
   }
   /*
   for (n = 0;n < i;n++) {
	   cambia_espacios(*(p + n),1);
   }
   */
   return(i);
}


int agistrcmp(q1,q2)
unsigned char *q1;
unsigned char *q2;
{
   int i;
   for (i = 0;q1[i];i++)
   {
      if (!q2[i]) return(1);
      if (q1[i] < q2[i])
         return(-1);
      else
         if (q1[i] > q2[i])
            return(1);
   }
   if (q2[i]) return(-1);
   return(0);
}

#ifdef FGVSCO
char *strlwr(s)
char *s;
{
char *t;
for (t = s; *t; t++) *t = tolower(*t);
return s;
}
#endif


char *strlwr(s)
char *s;
{
char *t;
for (t = s; *t; t++) *t = tolower(*t);
return s;
}
