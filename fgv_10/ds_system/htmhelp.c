
#ifndef TEST
#include <fgvsys.h>
#endif

#ifdef FGVDOS

#include <stdio.h>
#include <string.h>
#ifndef FGVDOS
#include <dirent.h>
#endif

#ifdef TEST
#include <windows.h> 
#include <direct.h> 
#include <iostream.h>
#else
#include <rglobal.h>
#include <rwmalloc.h>


#endif

/* de momento incorporan rutinas de los menus en modo local para evitar conflictos con las standard */
/* se han de bajar al puesto local junto con el paquete (de todos modos si se accede se bajan) */
#define t_rwfopen   fopen
#define t_lee_linea lee_linea
#define t_rwfclose  fclose

#ifndef TRUE 
#define TRUE 1
#endif

#define MAXPATH 128

#define MAXMENUS  100
#define MAXSUBMENU 10       /* numero de submenus */
#define TAMANO     10       /* opciones en una pagina */
#define MAXOP     100       /* opciones en un menu */
#define MAXNOMOP   30       /* tamanyo del nombre de opcion */
#define MAXNOMPRO  81       /* tamanyo del programa de opcion */

#define coma_linea(a,b) comas_linea(a,b,1,&coma)

struct t_opcion {
   char *o_nombre;
   int  o_menuhijo;
   char *o_programa;
   };
#define T_OPCION struct t_opcion

struct t_menu {
   int      m_nop;       /* numero de opciones del menu */
   int      m_menupadre; /* indice del menu padre */
   int      m_fqp;       /* fila > 0 esquina arriba, < 0 esquina abajo */
   int      m_cqp;       /* col. > 0 esquina izquierda, < 0 esquina derecha */
   T_OPCION **m_op; /* opciones */
   };
#define T_MENU struct t_menu

static int **posiciones;

static char   nommenu[MAXNOMOP];   /* nombre del menu principal */
static T_MENU **wmenu = NULL;    /* estructuras de menus */
static int    nsmenu;              /* submenu actual 0,1..,MAXSUBMENU-1 */
static int    wdef;
static int    nmenus;
static int    pintamenu;           /* si debe o no pintar el menu */
static int    pintahmenu;          /* si debe o no pintar el menu hijo */
static char *dir_smenu = (char *)0;
static char *dir_sgmenu = (char *)0;
static char  fichemenu[128];
static int es_menu_acceso_directo = 0;
static int wdesplegamenu = 0;

static int wmenu_en_alfa = 0;

static int def[MAXSUBMENU];
static int lockm;
static int s_lock;
static int tipo;
static char nfm[15];
static int prime;
static char nom[MAXPATH];
static char pro[MAXNOMPRO];

#ifdef TEST

static char *quita_path(char *ff)
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

static int es_numero(char *s) /* devuleve -1 si el string es un numero o vacio */
{
while(*s)
  {
  if (*s < '0' || *s > '9')
     return(0);
  s++;
  }
return(-1);
}


static void quitab(char *s)
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
static void quitabp(char *s)
{
   register char *c;

   for (c=s;(*c == ' ' || *c == 9);c++);
   while ((*s++=*c++) != '\0');
}

/*
 * quitan
 *    quitar los \n de final de string
 */
static void quitan(char *s)
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
      
}



static int _separa(char *str,char **p,int m,int tope)
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
return(i);
}



static int general = 0;

static int comas_linea(char *texto,int t,int m,int *p)
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


static char *lee_linea(char *texto,int num,FILE *file)
{

   if (fgets(texto,num,file) == NULL)
      return(NULL);
        quitan(texto);    /* quita los '\n' de detras */
   quitab(texto);   /* quita blancos de detras  */
   if (texto[0] == 4 || texto[0] == 26) /* tanto el Ctrl-D como el Ctrl-Z acaban */ 
      return(NULL);
   return(texto);
}

#endif

static int convierte_char(unsigned char c,char *buf2,int j)
{	  


	  switch(c)
	     {
		 case ' ':
		 case 160:
		   strcpy(buf2+j,"&aacute");
		   j += 7;
		   break;
		 case '‚':
		 case 130:
		   strcpy(buf2+j,"&eacute");
		   j += 7;
		   break;
		 case 161:
		 case '¡':
		   strcpy(buf2+j,"&iacute");
		   j += 7;
		   break;
		 case 162:
		 case '¢':
		   strcpy(buf2+j,"&oacute");
		   j += 7;
		   break;
		 case '£':
		 case 163:
		   strcpy(buf2+j,"&uacute");
		   j += 7;
		   break;
		 case 144:
		   strcpy(buf2+j,"&Eacute");
		   j += 7;
		   break;
		 case 135:
		   strcpy(buf2+j,"&ccedil");
		   j += 7;
		   break;
		 case 128:
		   strcpy(buf2+j,"&Ccedil");
		   j += 7;
		   break;
		 case '¤':
		 case 164: /* eñe minuscula */
		   strcpy(buf2+j,"&ntilde");
		   j += 7;
		   break;
		 case '¥':
		 case 165: /* eñe mayuscula */
		   strcpy(buf2+j,"&Ntilde");
		   j += 7;		   
		   break;
		 case 34:
		   strcpy(buf2+j,"&quot");
		   j += 5;		   
		   break;
		 case 38:
		   strcpy(buf2+j,"&amp");
		   j += 4;		   
		   break;
		 case 37:
		   strcpy(buf2+j,"&#37");
		   j += 4;		   
		   break;
		 case '¦':
		 case 166:
		   strcpy(buf2+j,"&ordf");
		   j += 5;		   
		   break;
		 case '§':
		 case 167:
		   strcpy(buf2+j,"&ordm");
		   j += 5;		   
		   break;
		 case 174:
		   strcpy(buf2+j,"&#171");
		   j += 5;		   
		   break;
		 case 175:
		   strcpy(buf2+j,"&#187");
		   j += 5;		   
		   break;
		 case '<':
		   buf2[j] = '(';
		   j ++;		   
		   break;		   
		 case '>':
		   buf2[j] = ')';
		   j ++;		   
		   break;		   
		 case '¨':
		   strcpy(buf2+j,"&iquest");
           j += 7;
		   break;
		 default:
           if (c >= 179 && c <= 220)
               buf2[j] = '*';
           else
               buf2[j] = c;
           j++;
		   break;
		 }
return(j);
}


#ifdef TEST
static char **lee_direct(char *path)
{
char **dir = NULL;
char tmp[512];
int entries = 0;
int tama = 0,i;
WIN32_FIND_DATA __dir;
HANDLE _busqueda = INVALID_HANDLE_VALUE;

strcpy(tmp,path);
strcat(tmp,"/*.txt");
for (i = 0;tmp[i];i++)
   {
   if (tmp[i] == '/') tmp[i] = '\\';
   }
memset((void *)&__dir,0,sizeof(__dir));
if ((_busqueda = FindFirstFile(tmp,&__dir)) != INVALID_HANDLE_VALUE)
   {
   do {
      if (tama <= entries)
         {
         tama = entries + 100;
         if (dir)
            dir = (char **)realloc(dir,sizeof(char *) * tama);
         else
            dir = (char **)malloc(sizeof(char *) * tama);
         }
      dir[entries] = new char[strlen(__dir.cFileName)+1];
      strcpy(dir[entries],__dir.cFileName);
      entries++;
      dir[entries] = NULL;
      } while(FindNextFile(_busqueda,&__dir) == TRUE);
   FindClose(_busqueda);
   }

return dir;
}
#endif

static FILE *crea_relacion_global(char *directorio)
{
char tmp[512];
FILE *f;

sprintf(tmp,"%s/~list.js",directorio);
f = fopen(tmp,"w");
fprintf(f,"profiles = new Array(\n");
return(f);
}


static void cierra_relacion_global(FILE *fglobal)
{
   if (fglobal)
      {
	  fprintf(fglobal,"\"||\"\n");
      fprintf(fglobal,");\n");
      fclose(fglobal);
      }
}


static char *rellena_espacios(char *texto)
{
char *p,*p2;
static char tmp[1024];


while(*texto && *texto == ' ') texto++;
p = tmp;
p2 = texto;
while (*p2)
   {
   while (*p2 == ' ' && *(p2+1) == ' ') p2++;
   *p = *p2;   
   p2++;
   p++;
   }
*p = *p2;
quitab(tmp);
strminus(tmp);
p = tmp;
while (*p)
   {
   switch (*p)
      {
		 case ' ':
		 case 160:
           *p = 'a';
		   break;
		 case '‚':
		 case 130:
           *p = 'e';
		   break;
		 case 161:
		 case '¡':
           *p = 'i';
		   break;
		 case 162:
		 case '¢':
           *p = 'o';
		   break;
		 case '£':
		 case 163:
           *p = 'u';
		   break;
		 case 144:
           *p = 'e';
 	       break;
         case ' ':
           *p = '_';
           break; 
         default:
           break;
      }
   p++;
   }
return(tmp);
}

static char *convierte_chars(char *_linea)
{
static char tmp[2048];
unsigned char *linea = (unsigned char *)_linea;
int j = 0,i = 0;
for (i = 0;linea[i];i++)
   {
   j = convierte_char(linea[i],tmp,j);   
   }
tmp[j] = 0;
return(tmp);
}

static char *busca_titulo(char **txt,char *aplicacion,char *fichero)
{
char **_txt;
char *p,*p2;
int l;
static char tmp[512];

   
   for (_txt = txt;*_txt;_txt++)
       {
       if ((p = strstr(*_txt,"((")))
          {
		  p += 2;
          p2 = strstr(p,"))");
          if (p2)
             {
             l = p2 - p;
             }
          else
             {
             l = strlen(p);
             }
          if (l < 512)
             {
             memcpy(tmp,p,l);
             tmp[l] = 0;
             return(tmp);
             }
          }
       }
   sprintf(tmp,"%s/%s",aplicacion,fichero);
   return(tmp);
}


static void analiza_linea(FILE *fo,char *linea,char *aplicacion,char *titulo,int *saltar,int *fin_p,char *arrastre,int *arrastrar,int *continua)
{
unsigned char *p,*p2;
char tmp[2048];
char tmp2[512];
char tmp3[512];
int j = 0;
int l,i,es;
int no_vale = 0;
int hay_ref = 0;
int hay_link = 0;
int para_arrastre = 0;
int sin_ref = 0;
int pon_salto = 0;

  
   p = (unsigned char *)linea;
   while(*p)
     {  
     /* saltar titulo */
     if (*p == '(' && *(p+1) == '(')   
        {
		*continua = 0;
        p2 = (unsigned char *)strstr((char *)p,"))");
        if (p2)
           p = p2 + 2;
        else
		   {
		   *continua = 1;
           return;
           }
        no_vale = 1;
        continue;
        }
     if (*p == '<' && *(p+1) == '<')   
        {
        /* html directo */        
        p2 = (unsigned char *)strstr((char *)p,">>");
        if (p2)
           {
           l = p2 - p;
           p++;
           memcpy(tmp+j,p,l);
           j += l;
           tmp[j] = 0;
           p = p2 + 2;
           continue;
           }        
        }
     /* vinculos */
     if (!*continua && *p == '{' && *(p+1) == '{')   
        {			
		no_vale = 0;		
		hay_link = 1;
	    p += 2;		   
        p2 = (unsigned char *)strstr((char *)p,"}}");
        if (p2)
           l = p2 - p;
        else
		   {
           *continua = 1;
		   return;
		   }
        memcpy(tmp2,p,l);
        tmp2[l] = 0;
		quitab(tmp2);
		p += l;
		p += 2;
		/* definicion del parametro */
        p2 = (unsigned char *)strstr((char *)p,"{{");
		if (p2)
		   {
		   p = p2 + 2;
		   p2 = (unsigned char *)strstr((char *)p,"}}");
           if (p2)
               l = p2 - p;
           else
		       {
               *continua = 1;
		       return;
		       }
		   memcpy(tmp3,p,l);
		   tmp3[l] = 0;
		   quitab(tmp3);
		   if (tmp3[0])
		     {			 
             for (i = 0;tmp3[i];i++)
			    {
				if (tmp3[i] == '#')
				   {				   
				   break;
				   }
				}			 
		     memcpy(tmp+j,"<A HREF=",8);
		     j += 8;
			 if (!tmp3[i])
			    {
				if (!strcmp(tmp3,aplicacion))
				   {
				   para_arrastre = 1;
				   }
		        sprintf(tmp+j,"%s.html>%s</A>",tmp3,convierte_chars(tmp2));
				}
			 else
			    {
				tmp3[i] = 0;
				quitab(tmp3);
				if (!strcmp(tmp3,aplicacion))
				   {
				   para_arrastre = 1;
				   }
				if (tmp3[0])
                   sprintf(tmp+j,"%s.html#%s>%s</A>",tmp3,tmp3+i+1,convierte_chars(tmp2));
				else
				   sprintf(tmp+j,"#%s>%s</A>",tmp3+i+1,convierte_chars(tmp2));
				}
			 }
		   else
		     {
			 sprintf(tmp+j,"<A HREF=~bmenu.html#%s%s>%s</A>",arrastre,rellena_espacios(tmp2),convierte_chars(tmp2));
			 }
		   if (*arrastrar == 1)
		      {
			  strcat(arrastre,rellena_espacios(tmp2));
			  strcat(arrastre,"\\");
			  }
		   p += l;
		   p += 2;
		   }
		else
		   {
		   /* Se puede hacer una segunda pasada para intentar ligarlos segun el titulo en el indice */
           sprintf(tmp+j,"<A HREF=~bmenu.html#%s%s>%s</A>",arrastre,rellena_espacios(tmp2),convierte_chars(tmp2));
		   }
		j = strlen(tmp);        
        continue;
        }

     /* Etiquetas */
     if (*p == '[' && *(p+1) == '[')   
        {
		no_vale = 0;
		hay_ref = 1;

        if (*saltar)
          {
		  pon_salto = 1;          
	      }
        *saltar = 1;		

		p += 2;
        p2 = (unsigned char *)strstr((char *)p,"]]");
        if (p2)
           l = p2 - p;
        else
		   {
           *continua = 1;
		   return;
		   }
        memcpy(tmp2,p,l);
        tmp2[l] = 0;
		p += l;
        p += 2;	
		strcpy(tmp+j,"<A NAME=");
		j = strlen(tmp);

		/* definicion del parametro */
        p2 = (unsigned char *)strstr((char *)p,"[[");
		if (p2)
		   {
		   p = p2 + 2;
		   p2 = (unsigned char *)strstr((char *)p,"]]");
           if (p2)
               l = p2 - p;
           else
		       {
               *continua = 1;
		       return;
		       }
		   memcpy(tmp3,p,l);
		   tmp3[l] = 0;
		   quitab(tmp3);
		   if (tmp3[0])
		     {		   
		     sprintf(tmp+j,"%s><B>%s</B></A>",tmp3,convierte_chars(tmp2));
			 }
		   else
		     {
			 /* avisar de que falta!!! */
			 sprintf(tmp+j,"%s><B>%s</B></A>",tmp2,convierte_chars(tmp2));
			 }
		   p += l;
		   p += 2;
		   }
		else
		   {
		   /* avisar de que falta!!! */
           sprintf(tmp+j,"%s><B>%s</B></A>",tmp2,convierte_chars(tmp2));
		   }
		j = strlen(tmp);
        continue;
        }

     j = convierte_char(*p,tmp,j);
	 if (*p != ' ' || *p != 9)
	    sin_ref++;
	 p++;
     }

   tmp[j] = 0;
   quitab(tmp);
   if (!tmp[0] && no_vale)
      {
      return;
      }
   if (tmp[0])
      j--;

   if (!hay_link)
      {
	  *arrastrar = 0;
	  }

   if (pon_salto)
      fprintf(fo,"<A HREF=#Inicio><IMG SRC=../../../ds_help/begin.gif ALT='Inicio' BORDER=0 ALIGN=RIGHT></A><BR>\n");

   if (hay_ref || !sin_ref || !tmp[j] || tmp[j] == '.' || tmp[j] == ':' || tmp[j] == '-' || tmp[j] == '>' || (tmp[j] == '>' && j && tmp[j-1] == ','))
      {
	  if ((!tmp[j] || hay_ref || !sin_ref) && !*fin_p)
	     {
		 fprintf(fo,"<BR>\n");
		 }
      fprintf(fo,"%s<BR>\n",tmp);
	  *fin_p = 1;
	  if (para_arrastre && tmp[j] == '>')
	     {
         *arrastrar = 1;
		 arrastre[0] = 0;
		 }
	  }
   else
      {
	  *arrastrar = 0;
	  fprintf(fo,"%s\n",tmp);
	  *fin_p = 0;
	  }
}



static void analiza_txt(char *path,char *aplicacion,char *fichero,char *html,FILE *fglobal)
{
char path_txt[512];
char path_html[512];
char titulo[1024];
char buf[4000];
char *pp;
char arrastre[1024];
int arrastrar = 0;
char **txt = NULL;
char **_txt;
int entries = 0;
int tama = 0;
int salta = 0;
int fin_p = 1;
int continua = 0;

FILE *fi,*fo;
  sprintf(path_txt,"%s/%s.txt",path,fichero);
  sprintf(path_html,"%s/%s.html",html,fichero);  
  fi = fopen(path_txt,"r");
  if (fi)
     {
     fo = fopen(path_html,"w");
     if (fo)
        {
        /* cargar el txt en memoria para mejor analisis */
        while(fgets(buf,2000,fi))
            {
            quitan(buf);
            quitab(buf);
            if (tama <= (entries+1))
              {
              tama = entries + 100;
              if (txt)
                 txt = (char **)realloc(txt,sizeof(char *) * tama);
              else
                 txt = (char **)malloc(sizeof(char *) * tama);
              }
            txt[entries] = malloc(strlen(buf)+1);
            strcpy(txt[entries],buf);
            entries++;
            txt[entries] = NULL;            
            }
        if (entries)
		   {

		   strcpy(titulo,convierte_chars(busca_titulo(txt,aplicacion,fichero)));

           fprintf(fglobal,"\"%s||%s.html\",\n",titulo,fichero);		   

           fprintf(fo,"<HTML>\n");
           fprintf(fo,"<!%s/%s>\n",aplicacion,fichero);
           fprintf(fo,"<HEAD>\n");                
           fprintf(fo,"<TITLE>%s</TITLE>\n",titulo);
           /* fprintf(fo,"<P><B>%s</B></P><HR>\n",aplicacion); */
           fprintf(fo,"</HEAD>\n\n");
           fprintf(fo,"<BODY BGCOLOR=ADD8E6>\n");
           fprintf(fo,"<A NAME=Inicio></A>\n");
           fprintf(fo,"<FONT FACE='Arial, Helvetica' SIZE=2>\n");
		   fprintf(fo,"<A HREF=~bmenu.html>Menu Principal</A><BR>\n");
           fprintf(fo,"<BR>\n");		   
           for (_txt = txt;*_txt;_txt++)
             {             
			 if (continua)
			   {
			   strcat(buf,*_txt);
			   pp = buf;
			   continua = 0;
			   }
			 else
			   {
			   pp = *_txt;
			   buf[0] = 0;
			   }			   			 
			 analiza_linea(fo,pp,aplicacion,titulo,&salta,&fin_p,arrastre,&arrastrar,&continua);
			 if (continua)
			   {
			   if (buf[0])
			     continua = 0;
			   else
			     strcpy(buf,*_txt);
			   }
             free(*_txt);
             }
		   fprintf(fo,"<A HREF=#Inicio><IMG SRC=../../../ds_help/begin.gif ALT='Inicio' BORDER=0 ALIGN=RIGHT></A><BR>\n");

           fprintf(fo,"</FONT>\n");
           fprintf(fo,"</BODY>\n");
           fprintf(fo,"</HTML>\n");

           free(txt);

		   }

        fclose(fo);
        }
     fclose(fi);
     }
}


static int wlee_un_submenu(int padre,char *path,int i,char *texto,FILE *file1,int *r)
{
register int j;
char *elhijo;
FILE *file2;
int separa,separa2,separa3;
int memoria;
int coma;
char tmp[81];

                t_lee_linea(texto,128,file1);
                wmenu[i] = (T_MENU *)malloc(sizeof(T_MENU));
                wmenu[i]->m_nop = atoi(texto+coma_linea(texto,0));
                wmenu[i]->m_menupadre = atoi(texto+coma_linea(texto,1));
                if (padre > -1)
                        wmenu[i]->m_menupadre = padre;
                wmenu[i]->m_fqp       = atoi(texto+coma_linea(texto,1));
                wmenu[i]->m_cqp       = atoi(texto+coma_linea(texto,1));
                wmenu[i]->m_op = (T_OPCION **)malloc(wmenu[i]->m_nop*sizeof(T_OPCION *));
                for (j=0;j<wmenu[i]->m_nop;j++)
                        {
                        wmenu[i]->m_op[j] = (T_OPCION *)malloc(sizeof(T_OPCION));
                        t_lee_linea(texto,128,file1);
                        separa = coma_linea(texto,0);
                        memoria = strlen(texto+separa)+2;
                        wmenu[i]->m_op[j]->o_nombre = (char *)malloc(memoria);
                        strcpy(wmenu[i]->m_op[j]->o_nombre,texto+separa);

                        elhijo = texto+coma_linea(texto,1);

                        separa = coma_linea(texto,1);
                        if (elhijo[0] == ':')
                                {
                                sprintf(tmp,"0 %s ",elhijo+1);
                                separa2 = coma_linea(texto,1);
                                separa3 = coma_linea(texto,1);
                                if (strlen(texto+separa2))
                                        {
                                        strcat(tmp,texto+separa2);
                                        }
                                else
                                        {
                                        strcat(tmp,fichdir);
                                        }

                                if (strlen(texto+separa))
                                        {
                                        strcat(tmp," ");
                                        strcat(tmp,texto+separa);
                                        strcat(tmp," ");
                                        }
                                else
                                        {
                                        strcat(tmp," NULL ");
                                        }

                                if (strlen(texto+separa3))
                                        {
                                        strcat(tmp,texto+separa3);
                                        }
                                else
                                        {
                                        strcat(tmp,elhijo+1);
                                        }
                                }
                        else
                                {
                                strcpy(tmp,texto+separa);
                                }
                        wmenu[i]->m_op[j]->o_programa = (char *)malloc(strlen(tmp)+2);
                        strcpy(wmenu[i]->m_op[j]->o_programa,tmp);

                        /* texto ya no se usa */
                        if (elhijo[0] != ':' && elhijo[0] != '-' && (elhijo[0] < '0' || elhijo[0] > '9'))
                          {
                          strcpy(texto,path);
                          strcat(texto,elhijo);
                          strcat(texto,".smn");
                          file2 = t_rwfopen(texto,"r");
                           if (file2)
                                  {
                                  wmenu[i]->m_op[j]->o_menuhijo = nmenus;
                                  nmenus++;
                                  wmenu = (T_MENU **)realloc(wmenu,sizeof(T_MENU *) * (nmenus + 1));
                                  wlee_un_submenu(i,path,nmenus-1,texto,file2,r);
                                  t_rwfclose(file2);
                                  }
                             else
							      {
                                  wmenu[i]->m_op[j]->o_menuhijo = -1;
							      }
                          }
                        else
                          {
                          if (elhijo[0] == '-' || elhijo[0] == ':')
                             wmenu[i]->m_op[j]->o_menuhijo = -1;
                          else
                             wmenu[i]->m_op[j]->o_menuhijo = atoi(elhijo);
                          }						
                        /*if (j == 0 && trataop(i,0) > 0) /* ha puesto menu ficheros o listados */
                        /*  break;*/
                        if (i == (wdef / 100) && ((wdef%100) - 1) == j) 
						  {
                          if (wmenu[i]->m_op[j]->o_menuhijo > -1)
						     {
                             (*r)++;
                             if ((*r) < MAXSUBMENU)
                                 wdef = wmenu[i]->m_op[j]->o_menuhijo * 100 + def[(*r)];
                             }
                          }
                        }
return(0);
}

static int carga_opcion(texto,opcion,titulo)
char *texto;
int *opcion;
char *titulo;
{
int i,j,coma = 0;
int r = 0;

for (i = 0;i < MAXSUBMENU;i++)
  {
  opcion[i] = -1; 
  } 

for (i = 0;texto[i] && texto[i] != ':';i++);

if (texto[i] == ':')
   {		   
   texto[i] = 0;
   strcpy(titulo,texto+i+1);
   for (i = 0;i < MAXSUBMENU;i++)
     {
	 j = comas_linea(texto,i,1,&coma);
	 if (!strlen(texto+j)) break;
	 if (texto[j] == '!')
	    {
		r = 1;
		break;
	    }
	 if (texto[j] < '0' || texto[j] > '9')
	    {
        if (texto[j] < 'A' || texto[j] > 'Z')
		   break;
		texto[j] -= 17;
	    }	 
	 opcion[i] = atoi(texto+j);
	 if (opcion[i] < 0)
	    {
		opcion[i] = -1;
		break;
	    }
	 }   
   for (;i < MAXSUBMENU;i++)
     {
	 opcion[i] = -1; 
     }
   }
return(r);
}


static int v_posicion(sm,posicion)
int sm;
int posicion;
{
int i;
if (sm >= nmenus) return(0);
if (!wmenu[sm]) return(0);
if (!(wmenu[sm]->m_nop)) return(0);
if (!posiciones[sm]) return(0);

for (i = 0;i < wmenu[sm]->m_nop;i++)
   {
   if (posiciones[sm][i] == posicion)
	  return(i);
   }
return(wmenu[sm]->m_nop);
}

static int pon_opcion(titulo,sm,posicion)
char *titulo;
int sm;
int posicion;
{
int i,j;
wmenu[sm]->m_op[posicion]->o_menuhijo = -1;

for (i = 0;titulo[i];i++)
  {
  if (titulo[i] == '{' && titulo[i+1] == '{')
     {
	 titulo[i] = 0;
	 i += 2;
     for (j = i;titulo[j];j++)
       {
	   if (titulo[j] == '}' && titulo[j+1] == '}') 
	      {
		  titulo[j] = 0;
	      }
	   }
	 break;
     }
  }

quitabp(titulo+i);
wmenu[sm]->m_op[posicion]->o_programa = malloc(strlen(titulo+i)+2);
strcpy(wmenu[sm]->m_op[posicion]->o_programa,titulo+i);
quitab(titulo);
wmenu[sm]->m_op[posicion]->o_nombre = malloc(strlen(titulo)+2);
strcpy(wmenu[sm]->m_op[posicion]->o_nombre,titulo);
return(0);
}

static int limite_pos_op(int ops)
{
if (ops < 10) return ops;
return 10;
}

static int ajusta_pos_(int pos)
{
return( (pos % 10) + 1);
}

static int carga_fgvmenu(file1,texto)
FILE *file1;
char *texto;
{
int coma = 0;
int i,n,sm,p,pf,pc,l1,l2,nm,i1;
int a_defecto[MAXSUBMENU];
int posicion[MAXSUBMENU];
char titulo[512];
int directo = 0;

   posiciones = NULL;
   wdef = 0;
   pf = -1;
   pc = -1;
   nmenus = 0;
   wmenu  = NULL;
   for (i = 0;i < MAXSUBMENU;i++)
     {
	 a_defecto[i] = -1; 
     }
   nommenu[0] = 0;
   do
     {
	 quitabp(texto);
	 if (texto[0] && texto[0] != ';')	    
	    {
		if (texto[0] == '!')
		   {
		   pf = atoi(texto+1+comas_linea(texto+1,0,1,&coma));
		   pc = atoi(texto+1+comas_linea(texto+1,1,1,&coma));
		   if (pf < 5 || pf > 22) pf = 5;
		   if (pc < 1 || pc > 75) pc = 1;
		   }
		else
		if (texto[0] == '#')
		   {
		   l2 = 1;
		   if (texto[1] == '#')
		      {
			  wdesplegamenu = 0;
			  l2++;
		      }
		   directo = carga_opcion(texto+l2,a_defecto,nommenu);
		   }
		else
		   {
		   carga_opcion(texto,posicion,titulo);
		   sm = 0;
           for (p = 0;p < MAXSUBMENU;p++)
		      {			  
			  if (posicion[p] == -1) break;			  
 		      }
		   if (p)
		      {
			  p--;
			  sm = 0;
			  n = -1;
			  for (i = 0;i < p;i++)
			     {
				 if (sm == -1)
				    {
					break;
				    }
				 if (nmenus <= sm)
				    {
					break;
				    }
				 if (v_posicion(sm,posicion[i]) < 0 || v_posicion(sm,posicion[i]) >= wmenu[sm]->m_nop)
				    {
					break;
				    }
				 n = sm;
				 sm = wmenu[sm]->m_op[ v_posicion(sm,posicion[i]) ]->o_menuhijo;
			     }
			  if (i == p)
			     {
				 if (!wmenu)
				    {
					if (!p)
					   {
					   nmenus = 1;
					   wmenu = (T_MENU **)malloc(sizeof(T_MENU *) * (nmenus + 1));
					   posiciones = (int **)malloc(sizeof(int *) * (nmenus + 1));

					   wmenu[nmenus-1] = (T_MENU *)malloc(sizeof(T_MENU));					   
					   wmenu[nmenus-1]->m_menupadre = -1;
					   wmenu[nmenus-1]->m_nop = 1;					   
					   wmenu[nmenus-1]->m_op = (T_OPCION **)malloc(wmenu[nmenus-1]->m_nop*sizeof(T_OPCION *));
					   posiciones[nmenus-1] = (int *)malloc(wmenu[nmenus-1]->m_nop*sizeof(int));

					   posiciones[nmenus-1][0] = posicion[p];
					   wmenu[nmenus-1]->m_op[0] = (T_OPCION *)malloc(sizeof(T_OPCION));
					   pon_opcion(titulo,nmenus-1,0);
					   if (pf != -1)
					     {
                         wmenu[nmenus-1]->m_fqp = pf;
                         wmenu[nmenus-1]->m_cqp = pc;
					     }
					   else
					     {
                         wmenu[nmenus-1]->m_fqp = 6;
                         wmenu[nmenus-1]->m_cqp = 3;
					     }
                       pf = -1;
                       pc = -1;
					   }
				    }
				 else
				 if (sm == -1)
				    {
					if (p && n > -1)
					   {
					   nmenus++;
                       wmenu = (T_MENU **)realloc(wmenu,sizeof(T_MENU *) * (nmenus + 1));					   
					   posiciones = (int **)realloc(posiciones,sizeof(int *) * (nmenus + 1));

					   wmenu[nmenus-1] = (T_MENU *)malloc(sizeof(T_MENU));
					   wmenu[nmenus-1]->m_menupadre = n;
					   wmenu[nmenus-1]->m_nop = 1;
					   wmenu[nmenus-1]->m_op = (T_OPCION **)malloc(wmenu[nmenus-1]->m_nop*sizeof(T_OPCION *));
					   posiciones[nmenus-1] = (int *)malloc(wmenu[nmenus-1]->m_nop*sizeof(int));

					   posiciones[nmenus-1][0] = posicion[p];

					   wmenu[nmenus-1]->m_op[0] = (T_OPCION *)malloc(sizeof(T_OPCION));
                       wmenu[n]->m_op[ v_posicion(n,posicion[p-1]) ]->o_menuhijo = nmenus-1;
					   pon_opcion(titulo,nmenus-1,0);
					   nm = nmenus-1;
					   if (pf != -1)
					      {
                          wmenu[nm]->m_fqp = pf;
                          wmenu[nm]->m_cqp = pc;
					      }
					   else
					      {						  
                          wmenu[nm]->m_fqp = wmenu[n]->m_fqp + 4 + ajusta_pos_(v_posicion(n,posicion[p-1]));
						  if (!wdesplegamenu)
						    {							
							wmenu[nm]->m_cqp = wmenu[n]->m_cqp + 15;
						    }
						  else
						    {
						    l1 = 0;
						    for (l2 = 0;l2 <  wmenu[n]->m_nop;l2++)
						      {
							  if (l1 < strlen(wmenu[n]->m_op[ l2 ]->o_nombre))
                                l1 = strlen(wmenu[n]->m_op[ l2 ]->o_nombre);
						      }
						    l2 = 1;
						    wmenu[nm]->m_cqp = wmenu[n]->m_cqp + l1 + 15;
						    }
					      l1 = strlen(wmenu[n]->m_op[ v_posicion(n,posicion[p-1]) ]->o_nombre) + 7;
					      if (l1 < (strlen(wmenu[nm]->m_op[ 0 ]->o_nombre)+14))
					          {
						      l1 = strlen(wmenu[nm]->m_op[ 0 ]->o_nombre)+14;
					          }
					      if ( (wmenu[nm]->m_cqp + l1) > 80)
					          {
                              if (wdesplegamenu && (wmenu[n]->m_cqp - l1) > 1)
							   {
                               wmenu[nm]->m_cqp = wmenu[n]->m_cqp - l1;
							   }
							  else
							   {
						       wmenu[nm]->m_cqp = 80 - l1;
							   l2 = 0; 
							   }
							  }
					        
                          if (l2 &&  (wmenu[nm]->m_fqp + limite_pos_op(wmenu[nm]->m_nop) + 4) > 23 )
						     {
							 /* si no cabe debajo intentarlo encima */
							 l1 =  wmenu[n]->m_fqp +  2 + ajusta_pos_(v_posicion(n,posicion[p-1]));
							 l1 -= (limite_pos_op(wmenu[nm]->m_nop)+4);
							 if (l1 > 4)
							    {
								wmenu[nm]->m_fqp = l1;
							    }								 							 
							 /* sino cabe dejar el ajuste normal */
						     }
			              }					   
					   trataop(nm,0);
					   if ((nm+1) < nmenus)
					     {
					     posiciones = (int **)realloc(posiciones,sizeof(int *) * (nmenus + 1));
					     for (l1 = nm+1;l1 < nmenus;l1++)
					       {
						   posiciones[l1] = NULL;
					       }
					     }
					   
					   if ( (wmenu[nm]->m_fqp+5) > 23)
					      {
						  wmenu[nm]->m_fqp = 23 - 5;/* ? */
					      }
                       pf = -1;
                       pc = -1;
					   }
				    }
				 else
				 if (v_posicion(sm,posicion[p]) == wmenu[sm]->m_nop)
				    {
					wmenu[sm]->m_nop++;
					wmenu[sm]->m_op = (T_OPCION **)realloc(wmenu[sm]->m_op,wmenu[sm]->m_nop*sizeof(T_OPCION *));
					posiciones[sm] = (int *)realloc(posiciones[sm],wmenu[sm]->m_nop*sizeof(int));
					posiciones[sm][wmenu[sm]->m_nop-1] = posicion[p];

					wmenu[sm]->m_op[v_posicion(sm,posicion[p])] = (T_OPCION *)malloc(sizeof(T_OPCION));
					pon_opcion(titulo,sm,v_posicion(sm,posicion[p]));
					if (p && n != -1)
					   {					   
					   l1 = strlen(wmenu[sm]->m_op[ v_posicion(sm,posicion[p]) ]->o_nombre)+14;
					   l2 = 1;
					   if ( (wmenu[sm]->m_cqp + l1) > 80)
					      {
						  wmenu[sm]->m_cqp = 80 - l1;
						  l2 = 0;
					      }
					   if ( (wmenu[sm]->m_fqp+4+wmenu[sm]->m_nop) > 23)
					      {
						  /* si no cabe debajo intentarlo encima */
						  l1 =  wmenu[n]->m_fqp +  2 + ajusta_pos_(v_posicion(n,posicion[p-1]));
						  l1 -= (limite_pos_op(wmenu[sm]->m_nop)+4);
						  if (wdesplegamenu && l2 && l1 > 4)
						    {
							wmenu[sm]->m_fqp = l1;
						    }
						  else
						    {
						    /* sino cabe dejar el ajuste normal */
						    wmenu[sm]->m_fqp = 23 - 4 - limite_pos_op(wmenu[sm]->m_nop);
						    }
					      }					      
					   }
				    }
			     }
		      }
		   }
	    }
     } while(t_lee_linea(texto,82,file1));

   for (p = 0;p < MAXSUBMENU;p++)
     {			  
     if (a_defecto[p] == -1) break;			  
     }
   if (p)
     {
     p--;
	 sm = 0;	 
     for (i = 0;i < p;i++)
	    {
	    if (sm == -1)
		    {
			break;
		    }
		if (nmenus <= sm)
		    {
			break;
		    }
		if (v_posicion(sm,a_defecto[i]) < 0 || v_posicion(sm,a_defecto[i]) >= wmenu[sm]->m_nop)
		    {
			break;
		    }		 
		 sm = wmenu[sm]->m_op[ v_posicion(sm,a_defecto[i]) ]->o_menuhijo;
		 }
	 if (i == p && sm != -1)
	     {
		 wdef = (sm*100) + (v_posicion(sm,a_defecto[p]) + 1);
		 es_menu_acceso_directo = directo;
	     }
     }   

   if (posiciones)
     {
	 for (i1 = 0;i1 < nmenus;i1++)
	   { 
	   if (posiciones[i1])
	      free(posiciones[i1]);
	   }
	 free(posiciones);
	 posiciones = NULL;
     }

   return(0);
}

static int lee_menu(char *nnom)
{
        FILE *file1;
        char nomfi[MAXPATH];
        char texto[300];
        register int i,final;
        int r;
        int coma;


        lockm = -1;
        s_lock = 0;

        pintamenu = TRUE;
        pintahmenu = TRUE;
		strcpy(fichemenu,nnom);
        strcpy(nomfi,nnom);
        strcat(nomfi,".men");

        strcpy(nfm,quita_path(nomfi));

        if (wmenu)
            return(-1);


        if ((file1 = t_rwfopen(nomfi,"r")) == (FILE *) 0)
		    {
			return(-1);
		    }

        *(quita_path(nomfi)) = 0;

        t_lee_linea(texto,82,file1);
        quitabp(texto);
		if (texto[0] == '#' || texto[1] == ';')
		   {
           carga_fgvmenu(file1,texto);
		   }
		else
		   {
           for (i=0;i<MAXSUBMENU;i++)
                def[i] = atoi(texto+coma_linea(texto,i));
           r = 0;
           wdef = def[r];
           t_lee_linea(texto,82,file1);
           strcpy(nommenu,texto+coma_linea(texto,0));
           nmenus = atoi(texto+coma_linea(texto,1));
           wmenu = (T_MENU **)malloc(sizeof(T_MENU *) * (nmenus + 1));
           final = nmenus;
           for (i=0;i<final;i++) {
                wlee_un_submenu(-1,nomfi,i,texto,file1,&r);
                }
		   }

        t_rwfclose(file1);


        return(0);
}


static void free_omenu(int i)
{
int j;

      for (j=0;j<wmenu[i]->m_nop;j++) {
         free(wmenu[i]->m_op[j]->o_nombre);
         free(wmenu[i]->m_op[j]->o_programa);
         free((char *)wmenu[i]->m_op[j]);
         }
      free((char *)wmenu[i]->m_op);
}

static void free_menu(int i)
{
free_omenu(i);
free((char *)wmenu[i]);
}


static int libera_menu()
{
int i;


if (!wmenu) return(0);

for (i = 0; i < nmenus ;i++)
  free_menu(i);
nmenus = 0;
free((char *)wmenu);
wmenu = NULL;
return(0);
}

static void procesa_hoja_menu(FILE *f,int m,char *anterior)
{
int i,n,j,conpar = 0,otraapli = 0,acceso = 0;
char *seps[250];
char tmp1[512];
char tmp2[1024];

  for (i = 0;i < wmenu[m]->m_nop;i++)
     {
     if (wmenu[m]->m_op[i]->o_menuhijo > -1)
	    {
		if (wmenu[m]->m_op[i]->o_menuhijo < nmenus)
		   {		   
		   fprintf(f,"<A NAME=%s></A>\n",rellena_espacios(wmenu[m]->m_op[i]->o_nombre));
		   fprintf(f,"<A NAME=%s%s></A>\n",anterior,rellena_espacios(wmenu[m]->m_op[i]->o_nombre));
           fprintf(f,"<LI>%s<MENU>\n",convierte_chars(wmenu[m]->m_op[i]->o_nombre));
		   sprintf(tmp2,"%s%s\\",anterior,rellena_espacios(wmenu[m]->m_op[i]->o_nombre));
           procesa_hoja_menu(f,wmenu[m]->m_op[i]->o_menuhijo,tmp2);
		   fprintf(f,"</MENU>\n</LI>\n");
		   }
		}
	 else
	    {
		strcpy(tmp1,wmenu[m]->m_op[i]->o_programa);
		/* analizar el tipo de opcion		*/
		n = _separa(tmp1,seps,0,250);
        if (!es_numero(tmp1))
           {
           strcpy(tmp1,quita_path(wmenu[m]->m_op[i]->o_programa));
           for (j = 0;tmp1[j] && tmp1[j] != ';';j++);
           if (tmp1[j] == ';')
              {
              tmp1[j] = 0;              
              }
           for (j = 0;tmp1[j] && tmp1[j] != '.';j++);
           if (tmp1[j] == '.')
              {
              tmp1[j] = 0;              
              }
		   fprintf(f,"<A NAME=%s.html></A>\n",tmp1);
		   fprintf(f,"<A NAME=%s></A>\n",rellena_espacios(wmenu[m]->m_op[i]->o_nombre));
		   fprintf(f,"<A NAME=%s%s></A>\n",anterior,rellena_espacios(wmenu[m]->m_op[i]->o_nombre));
		   fprintf(f,"<LI><A HREF=%s.html>%s</A></LI>\n",tmp1,convierte_chars(wmenu[m]->m_op[i]->o_nombre));
           }        
		}
	 }
}


static void procesa_hoja_menu2(FILE *f,int m,int *nitem,char *anterior)
{
int i,n,j,conpar = 0,otraapli = 0,acceso = 0;
char *seps[250];
char tmp1[1024];

  for (i = 0;i < wmenu[m]->m_nop;i++)
     {
     if (wmenu[m]->m_op[i]->o_menuhijo > -1)
	    {
		if (wmenu[m]->m_op[i]->o_menuhijo < nmenus)
		   {
		   (*nitem)++;
		   fprintf(f,"<PARAM NAME=item%d VALUE=\"%s%s;agibook.gif,o_agibook.gif;;\">\n",*nitem,anterior,convierte_chars(wmenu[m]->m_op[i]->o_nombre));
		   sprintf(tmp1,"%s%s\\",anterior,convierte_chars(wmenu[m]->m_op[i]->o_nombre));
           procesa_hoja_menu2(f,wmenu[m]->m_op[i]->o_menuhijo,nitem,tmp1);		   
		   }
		}
	 else
	    {
		strcpy(tmp1,wmenu[m]->m_op[i]->o_programa);
 		/* analizar el tipo de opcion		*/
		n = _separa(tmp1,seps,0,250);
        if (!es_numero(tmp1))
           {
           strcpy(tmp1,quita_path(wmenu[m]->m_op[i]->o_programa));
           for (j = 0;tmp1[j] && tmp1[j] != ';';j++);
           if (tmp1[j] == ';')
              {
              tmp1[j] = 0;              
              }
           for (j = 0;tmp1[j] && tmp1[j] != '.';j++);
           if (tmp1[j] == '.')
              {
              tmp1[j] = 0;              
              }
		   (*nitem)++;
		   fprintf(f,"<PARAM NAME=item%d VALUE=\"%s%s;note.gif;%s.html,Frame3;\">\n",*nitem,anterior,convierte_chars(wmenu[m]->m_op[i]->o_nombre),tmp1);
           }        
		}
	 }
}

static void crea_html_standars(char *directorio)
{
char tmp[512];
FILE *f;

sprintf(tmp,"%s/~main.html",directorio);
f = fopen(tmp,"w");
if (f)
   {
   fprintf(f,"<HEAD>\n");
   fprintf(f,"<TITLE>Help</TITLE>\n");
   fprintf(f,"</HEAD>\n");
   fprintf(f,"<BODY BACKGROUND=\"../../../ds_help/paisaje.jpg\" BGPROPERTIES=\"fixed\">\n");
   fprintf(f,"<A NAME=\"#Inicio\"></A>\n");
   fprintf(f,"</BODY>\n");
   fprintf(f,"</HTML>\n");
   fclose(f);
   }

sprintf(tmp,"%s/~home.html",directorio);
f = fopen(tmp,"w");
if (f)
   {
   fprintf(f,"<HEAD>\n");
   fprintf(f,"<TITLE>Home Page</TITLE>\n");
   fprintf(f,"<SCRIPT>\n");
   fprintf(f,"function mainMenu(padre)\n");
   fprintf(f,"{\n");
   fprintf(f,"	padre.frames[3].location.replace(\"~main.html\");\n");
   fprintf(f,"	padre.frames[2].location.replace(\"~menu.html\");\n");
   fprintf(f,"}\n");
   fprintf(f,"</SCRIPT>\n");
   fprintf(f,"</HEAD>\n");
   fprintf(f,"<BODY BGCOLOR=\"#FFFFFF\">\n");
   fprintf(f,"<TABLE WIDTH=\"100%%\" HEIGHT=\"100%%\" BORDER=\"0\">\n");
   fprintf(f,"<TR>\n");
   fprintf(f,"<TD BGCOLOR=\"#FFFFFF\" WIDTH=50>\n");
   fprintf(f,"<A HREF=\"http://www.diagram.es\" TARGET=_parent>\n");
   fprintf(f,"<IMG SRC=\"../../../ds_help/logo.gif\" ALT=\"AGI Home Page\" BORDER=\"0\" ALIGN=\"BASELINE\" HSPACE=-3>\n");
   fprintf(f,"</A>\n");
   fprintf(f,"</TD>\n");   
   fprintf(f,"<TD BGCOLOR=\"#FFFFFF\" WIDTH=20>\n");   
   fprintf(f,"<A HREF=\"Javascript: mainMenu(parent);\"  TITLE=\"Menú inicial\"><FONT FACE=Arial SIZE=6>HELP</FONT></A>\n");
   fprintf(f,"</TD>\n");   
   fprintf(f,"</TR>\n");   
   fprintf(f,"</TABLE>\n");   
   fprintf(f,"</BODY>\n");   
   fprintf(f,"</HTML>\n");
   fclose(f);
   }

sprintf(tmp,"%s/~search.html",directorio);
f = fopen(tmp,"w");
if (f)
   {
   fprintf(f,"<HTML>\n");
   fprintf(f,"<HEAD>\n");
   fprintf(f,"<TITLE>Search Engine</TITLE>\n");
   fprintf(f,"<SCRIPT LANGUAGE=\"JavaScript\" SRC=\"~list.js\"></SCRIPT>\n");
   fprintf(f,"<SCRIPT LANGUAGE=\"JavaScript\" SRC=\"../../../ds_help/~search.js\"></SCRIPT>\n");
   fprintf(f,"</HEAD>\n");
   fprintf(f,"<BODY BACKGROUND=\"../../../ds_help/sbanner.jpg\" TEXT=\"BLACK\" BGCOLOR=\"#0066AA\" onLoad=\"document.forms[0].query.focus();\">\n");
   fprintf(f,"<A NAME=Inicio></A>\n");
   fprintf(f,"<TABLE WIDTH=\"100%%\" HEIGHT=\"50%%\" BORDER=\"0\">\n");
   fprintf(f,"<TR>\n");
   fprintf(f,"<TD ALIGN=\"TOP\" WIDTH=\"30\"><! NOWRAP >\n");
   fprintf(f,"<PRE><A HREF=\"javascript: if (parent.frames[3].document.title != 'Help') parent.frames[3].history.go(-1);\" TITLE='Página anterior'><IMG SRC=\"../../../ds_help/grayarrowl.gif\" ALIGN=BASELINE ALT=\"Página anterior\" BORDER=\"0\" HSPACE=5></A><A HREF='javascript: parent.frames[3].history.go(1)' TITLE='Página adelante'><IMG SRC=\"../../../ds_help/grayarrowr.gif\" ALIGN=BASELINE ALT=\"Página adelante\" BORDER=\"0\"></A></PRE>\n");
   fprintf(f,"</TD>\n");
   fprintf(f,"<TD ALIGN=\"TOP\"  WIDTH=\"300\" NOWRAP>\n");
   fprintf(f,"<PRE><FORM  NAME=\"search\" onSubmit=\"validate(document.forms[0].query.value); return false;\"> <INPUT TYPE=TEXT     NAME=\"query\"    SIZE=\"20\" 	     ALIGN=TOP><INPUT TYPE=HIDDEN NAME=\"standin\" VALUE=\"test.txt\" ALIGN=TOP> <INPUT TYPE=SUBMIT NAME=\"search\"  VALUE=\"Buscar\"  ALIGN=MIDDLE SIZE=height MAXLENGTH=20> <INPUT TYPE=BUTTON NAME=\"hints\" VALUE=\"Cómo?\"  ALIGN=BOTTOM onClick=\"showHints()\"></FORM></PRE>\n");
   fprintf(f,"</TD>\n");
   fprintf(f,"</TR>\n");
   fprintf(f,"</TABLE>\n");
   fprintf(f,"<TABLE WIDTH=\"100%%\" HEIGHT=\"70%%\" BORDER=\"0\" ALIGN=\"CENTER\">\n");
   fprintf(f,"<TR>\n");
   fprintf(f,"<TD ALIGN=\"JUSTIFY\">\n");
   fprintf(f,"<BR><BR>\n");
   fprintf(f,"</TD>\n");
   fprintf(f,"</TR>\n");
   fprintf(f,"</TABLE>\n");
   fprintf(f,"</BODY>\n");
   fprintf(f,"</HTML>\n");
   fclose(f);
   }

}

static void procesa_menu(char *dir_html,char *aplicacion)
{
char tmp[512];
FILE *f;
int nitem = 0;

sprintf(tmp,"%s/~bmenu.html",dir_html);
f = fopen(tmp,"w");
if (f)
  {
  fprintf(f,"<HTML>\n");  
  fprintf(f,"<NOFRAMES>\n");
  fprintf(f,"<HEAD>\n");  
  fprintf(f,"<TITLE>%s</TITLE>\n",nommenu);  
  fprintf(f,"<CENTER><P><B><H2>MANUAL EN LINEA DE</h2><H1>%s</H1></B></P></CENTER>\n",nommenu);
  fprintf(f,"</HEAD>\n\n");
  fprintf(f,"</NOFRAMES>\n");
  fprintf(f,"<BODY BGCOLOR=ADD8E6>\n<MENU>\n");
  procesa_hoja_menu(f,0,"");
  fprintf(f,"</MENU>\n</BODY>\n");
  fprintf(f,"</HTML>\n");
  fclose(f);
  }

sprintf(tmp,"%s/~menu.html",dir_html);
f = fopen(tmp,"w");
if (f)
  {
  fprintf(f,"<HTML>\n");  
  fprintf(f,"<HEAD>\n");
  fprintf(f,"<TITLE>%s</TITLE>\n",nommenu);  
  fprintf(f,"</HEAD>\n");
  fprintf(f,"<BODY>\n");
  fprintf(f,"<BODY BGCOLOR=#0066AA>\n");
  fprintf(f,"<FONT FACE='Arial, Helvetica' SIZE=2>\n");
  fprintf(f,"<A NAME=Inicio></A>\n");
  fprintf(f,"<TABLE WIDTH=100%% HEIGHT=100%% BORDER=0 ALIGN=LEFT>\n");
  fprintf(f,"<TR VALIGN=top>\n");
  fprintf(f,"<TD BGCOLOR=#FFFFFF><BR>\n");
  fprintf(f,"<APPLET CODE=TreeApp.class CODEBASE=../../../ds_help NAME=Tree WIDTH=200 HEIGHT=250>\n");
  fprintf(f,"<PARAM NAME=rootTitle VALUE=\"%s;agibook.gif,o_agibook.gif;;DIAGRAM Software\">",nommenu);
  fprintf(f,"<PARAM NAME=expanded VALUE=true>\n");
  fprintf(f,"<PARAM NAME=baseURL VALUE=\"\">\n");
  fprintf(f,"<PARAM NAME=item1 VALUE=\"%s;note.gif;%s.html,Frame3;\">\n",nommenu,aplicacion);
  nitem = 1;
  procesa_hoja_menu2(f,0,&nitem,"");

  fprintf(f,"</APPLET>\n");

  fprintf(f,"<PRE>\n");
  fprintf(f,"<font size=\"4\" face=\"Tahoma\" COLOR=#0066AA>Oficinas:\n");
  fprintf(f,"<A HREF=\"../../../ds_help/~donde.html\" TARGET=\"Frame3\"><IMG SRC=\"../../../ds_help/oficinas.jpg\" border=0 alt=\"Oficinas\"></A>\n");
  fprintf(f,"</PRE>\n");

  fprintf(f,"</TD>\n");
  fprintf(f,"</TR>\n");
  fprintf(f,"</TABLE>\n");
  fprintf(f,"</FONT>\n");
  fprintf(f,"</BODY>\n");
  fprintf(f,"</HTML>\n");
  fclose(f);
  }

}

#ifdef TEST
int main(int argc,char *argv[])
{
char aplicacion[128];
char directorio[256];
char dir_html[256];
char path_men[256];
char **dir,**_dir;
FILE *fglobal;
/* FILE *fmenu;*/
/* falta crear el tree en control y en html*/

/* primer parametro la aplicacion*/
/* con directorio a on*/
if (argc < 2 || !argv[1][0]) {cout << "Debe indicar el directorio de la aplicacio\n";exit(1);}
strcpy(directorio,argv[1]);
#else
int ds_help(char *direct)
{
char aplicacion[128];
char directorio[256];
char dir_html[256];
char path_men[256];
char **dir,**_dir;
FILE *fglobal;
#endif

strcpy(directorio,direct);
if (directorio[ strlen(directorio) - 1 ] == '/' || directorio[ strlen(directorio) - 1 ] == '\\')
   {
   directorio[ strlen(directorio) - 1 ] = 0;
   }
strcpy(aplicacion,quita_path(directorio));
sprintf(path_men,"%s/%s",directorio,aplicacion);
strcat(directorio,"/ayuda");
strcpy(dir_html,directorio);
strcat(dir_html,"/*.txt");
if ((dir = lee_direct(dir_html,0)))
   {
   strcpy(dir_html,directorio);
   strcat(dir_html,"/html");
   mkdir(dir_html);
   lee_menu(path_men);
   fglobal = crea_relacion_global(dir_html);
   for (_dir = dir;*_dir;_dir++)
      {
      (*_dir)[ strlen(*_dir) - 4 ] = 0; /* la extension .txt se da por supuesta */
      analiza_txt(directorio,aplicacion,*_dir,dir_html,fglobal);
      free(*_dir);
      }
   cierra_relacion_global(fglobal);   

   crea_html_standars(dir_html);

   if (wmenu)
      {
	  procesa_menu(dir_html,aplicacion);
      libera_menu();
	  }
   free(dir);
   }

return 0;
}

#endif