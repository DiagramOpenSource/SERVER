
#ifndef TEST
#include <fgvsys.h>
#endif

#ifdef FGVDOS

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef FGVDOS
#include <io.h>
#include <conio.h>
#endif

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
#include <Commctrl.h>
#include <rglobal.h>
#include <rwmalloc.h>


#endif
#ifdef FGVWIN
#include <fgvwin.h>
#else
#include "../rci/resource.h"
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

static char   nommenu[300];   /* nombre del menu principal */
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

static char **relacion_nombres = NULL;
static int tope_relacion_nombres = 0;

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

static FILE *crea_relacion_global(char *directorio,char *aplicacion,char *titulo,char *dira)
{
   char tmp[512];
   FILE *f;
   
   sprintf(tmp,"%s/%s.hpj",directorio,aplicacion);
   f = fopen(tmp,"w");
   if (!f) return(NULL);
   
   fprintf(f,"[OPTIONS]\n");
   fprintf(f,"TITLE=%s\n",titulo);
   fprintf(f,"COMPRESS=true\n");
   fprintf(f,"WARNING=0\n");
   sprintf(tmp,"%s/%s.hlp",dira,aplicacion);
   cambia_barra_dos(tmp);
   fprintf(f,"HLP=%s\n",tmp);
   sprintf(tmp,"%s/%s.cnt",dira,aplicacion);
   cambia_barra_dos(tmp);
   fprintf(f,"CNT=%s\n",tmp);
   sprintf(tmp,"%s/%s.log",dira,aplicacion);
   cambia_barra_dos(tmp);
   fprintf(f,"ERRORLOG=%s\n",tmp);
   fprintf(f,"\n");
   fprintf(f,"[MAP]\n");
   fprintf(f,"#include <%s.hm>\n",aplicacion);
   fprintf(f,"\n");
   fprintf(f,"[FILES]\n");
   
   return(f);
}


static void cierra_relacion_global(FILE *fglobal)
{
   if (fglobal)
   {
      fprintf(fglobal,"\n");
      fclose(fglobal);
   }
}



#ifdef RWXARXA
extern int en_conexion_terminal;
#endif
static int idx_global = -1;
static void pon_en_campo(char *tmpx,long pci,long pcf)
{
	if (idx_global > -1)
	{
		rw_write_(idx_global,tmpx,30);
		rw_write_(idx_global,&pci,sizeof(long));
		rw_write_(idx_global,&pcf,sizeof(long));
	}
}


int pon_ayuda_de_campo(void *puntero)
{
	char *bloque;
	char path[256];
	char modulo[128];
	char refer[128];	
	int abierto = 1;
	char tmpx[256];
	char tmpc[256];
	long tama = 38 * 3000; /* 38 es la longitud del 'registro' */
	int n,i;
	int ok = 0;
	char *buf,*p1,*p2;
	FILE *f;

	bloque = (char *)puntero;
	memcpy(refer,bloque+15,15);
	refer[15] = 0;
	quita_blancos(refer);
	bloque[15] = 0;
	quita_blancos(bloque);
	strcpy(modulo,bloque);
    

	if (en_conexion_terminal)
	{
		sprintf(path,"%s%s/",rw_directorio("remoto/"),modulo);
	}
	else
	{
		sprintf(path,"%s%s/",rw_directorio(""),modulo);
	}   

	*bloque = 0;
	if (idx_global < 0)
	{
		abierto = 0;
	    strcpy(tmpx,path);
		strcat(tmpx,"ayuda/campohlp.ref");
		idx_global = rw_open_(tmpx,O_RDONLY | O_BINARY);
	}
    if (idx_global < 0)
	{
		return -1;
	}	
	sprintf(tmpc,"%-15.15s",refer);
	buf = malloc(tama);
	rw_lseek_(idx_global,0L,SEEK_SET);
	while( (n = rw_read_(idx_global,buf,tama)) > 0)
	{
		p1 = buf;
		for (i = 0;i < (n/38);i++)
		{
			if (!memcmp(p1,tmpc,15))
			{
				memcpy(tmpc,p1+15,15);
				tmpc[15] = 0;
				quita_blancos(tmpc);
				sprintf(tmpx,"%sayuda/%s",path,tmpc);
				f = fopen(tmpx,"r");
				if (f)
				{
					if (*bloque)
					{
						strcat(bloque,"\r\n");
					}
					fseek(f,*((long *)(p1+30)),SEEK_SET);					
					for (i = 0;i < (*((long *)(p1+34)) - 1);i++)
					{
						if (i)
						{
							strcat(bloque," ");
						}
						if (!lee_linea(tmpx,250,f)) break;
						for (p2 = tmpx;*p2;p2++)
						{
							oem_a_ansi((unsigned char *)p2);
						}
						strcat(bloque,tmpx);
					}
					fclose(f);
					quitab(bloque);
					ok = 1;
					break;
				}
				else
				{
					memcpy(tmpc,p1,15);
				}
			}
			p1 += 38;
		}
		if (ok || n < tama)
			break;
	}
	free(buf);
	if (!abierto)
	{
		rw_close_(idx_global);
		idx_global = -1;
	}
	return 0;
}

static void analiza_txt(char *path,char *aplicacion,char *fichero,char *rtf,FILE *fglobal)
{
   char path_txt[512];
   char path_rtf[512];
   char titulo[1024];
   char titulo2[1024];
   char buf0[4000];
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
   int encampo = 0;
   char tmpx[300];
   char tmpc[300];
   long pci = 0,pcf = 0;   
   FILE *fi,*fo;

   tmpx[0] = 0;
   sprintf(path_txt,"%s/%s.txt",path,fichero);
   sprintf(path_rtf,"%s/%s.rtf",rtf,fichero);  
   fi = fopen(path_txt,"r");
   if (fi)
   {
      fo = fopen(path_rtf,"w");
      if (fo)
      {
         titulo2[0] = 0;
         /* cargar el txt en memoria para mejor analisis */
         while(fgets(buf,2000,fi))
         {
            quitan(buf);
            quitab(buf);
            quitan(buf);
            quitab(buf);
            {
               strcpy(buf0,buf);
               quita_blancos(buf0);
               if (txt && buf0[0] == '#' && strlen(buf0) < 20)
               {
                  if (!titulo2[0] && entries < 10)
                  {
                     int jj;
                     for (jj = 0;jj < entries;jj++)
                     {
                        strcpy(buf0,txt[jj]);
                        quitabp(buf0);
                        if (buf0[0])
                        {
                           strcat(titulo2,buf0);
                           if (jj < (entries-1))
                           {
                              if (jj < (entries-2))
                              {
                                 strcat(titulo2,",");
                              }
                              else
                              {
                                 strcat(titulo2,":");
                              }
                           }
                        }
                        free(txt[jj]);
                     }
                     entries = 0;
                     txt[entries] = NULL;
                  }
                  continue;
               }
            }
            if (tama <= (entries+1))
            {
               tama = entries + 100;
               if (txt)
                  txt = (char **)realloc(txt,sizeof(char *) * tama);
               else
                  txt = (char **)malloc(sizeof(char *) * tama);
            }
            txt[entries] = malloc(strlen(buf)+1);
            {
			   char *pc;
               char *p1 = buf;
               char *p2 = txt[entries];
               while (*p1)
               {
				   if (*p1 == '[' && *(p1+1) == '[')
				   {
					   p1 += 2;
					   pc = p1;
					   while (*p1 && (*p1 != ']' || *(p1+1) != ']'))
					   {
						   p1++;
					   }
					   if (!*p1)
						   break;
					   memcpy(tmpc,pc,(int)(p1-pc));
					   tmpc[(int)(p1-pc)] = 0;
					   quita_blancos(tmpc);
					   if (tmpc[0] == '/')
					   {
						   if (encampo)
						   {
							   pon_en_campo(tmpx,pci,pcf);
							   encampo = 0;
							   pci = 0;
							   pcf = 0;
						   }
					   }
					   else
					   {
						   encampo = 1;
						   sprintf(tmpx,"%-15.15s%-15.15s",tmpc,quita_path(path_txt));						   
						   pci = ftell(fi);
						   pcf = 0;
					   }
					   p1 += 2;
					   continue;
				   }
                   *p2 = *p1;
                   oem_a_ansi(p2);
                   p1++;p2++;
               }
               *p2 = 0;
            }
            /*strcpy(txt[entries],buf);*/
			if (encampo)
			{
				pcf++;
			}

            entries++;
            txt[entries] = NULL;
         }
	     if (encampo)
		 {
			   pon_en_campo(tmpx,pci,pcf);
			   encampo = 0;
			   pci = 0;
			   pcf = 0;
		 }

         if (entries)
         {
            fprintf(fglobal,"%s.rtf\n",fichero);		   
            
            fprintf(fo,"{\\rtf1 \\ansi\n\\deff0\n");
            fprintf(fo,"{\\fonttbl\n");
            fprintf(fo,"{\\f0\\froman Times Roman;}\n");
            fprintf(fo,"{\\f1\\fmodern Courier;}\n");
            fprintf(fo,"{\\f2\\froman Symbol;}\n");
            fprintf(fo,"}\n");   
            
            fprintf(fo,"{\\colortbl;\n");
            fprintf(fo,"\\red0\\green0\\blue0;\n");
            fprintf(fo,"\\red0\\green0\\blue255;\n");
            fprintf(fo,"\\red0\\green255\\blue255;\n");
            fprintf(fo,"\\red0\\green255\\blue0;\n");
            fprintf(fo,"\\red255\\green0\\blue255;\n");
            fprintf(fo,"\\red255\\green0\\blue0;\n");
            fprintf(fo,"\\red255\\green255\\blue0;\n");
            fprintf(fo,"\\red255\\green255\\blue255;\n");
            fprintf(fo,"\\red0\\green0\\blue127;\n");
            fprintf(fo,"\\red0\\green127\\blue127;\n");
            fprintf(fo,"\\red0\\green127\\blue0;\n");
            fprintf(fo,"\\red127\\green0\\blue127;\n");
            fprintf(fo,"\\red127\\green0\\blue0;\n");
            fprintf(fo,"\\red127\\green127\\blue0;\n");
            fprintf(fo,"\\red127\\green127\\blue127;\n");
            fprintf(fo,"\\red192\\green192\\blue192;\n");
            fprintf(fo,"}\\cb7\\cf1\n");
            
            fprintf(fo,"{#{\\footnote {#}{%s_ref}}}\n",fichero);
            
            strcpy(titulo,fichero);
            if (relacion_nombres)
            {
               int i;
               for (i = 0;relacion_nombres[i];i++)
               {
                  if (!strcmp(relacion_nombres[i],fichero))
                  {
                     strcpy(titulo,relacion_nombres[i]+20);
                     break;
                  }
               }
               if (!relacion_nombres[i])
               {
                  if (!strcmp(fichero,aplicacion) && nommenu[0])
                  {
                     strcpy(titulo,nommenu);
                  }
                  else
                     if (titulo2[0])
                     {
                        strcpy(titulo,titulo2);
                     }
               }
            }
            
            
            {
               FILE *ff;
               char tmp[1024];      	  	  
               sprintf(tmp,"%s/%s.top",path,fichero);
               ff = fopen(tmp,"w");
               if (ff)
               {
                  fprintf(ff,"%s\n",titulo);
                  fclose(ff);
               }
            }
            
            fprintf(fo,"{${\\footnote {$}{%s}}}\n",titulo);		   		   
            if (titulo[0] != 'Z' || titulo[1] != 'Z' || titulo[2] != '_')
            {
               fprintf(fo,"{K{\\footnote {K}{%s}}}\n",titulo);
               fprintf(fo,"{\\b\\ul %s \\ulnone\\par\\par\n",titulo);
            }
            for (_txt = txt;*_txt;_txt++)
            {
               int i,j;
               char *pt = *_txt;
               j = 0;
               for (i = 0;pt[i];i++)
               {
                  if (pt[i] == '{' || pt[i] == '}' || pt[i] == '\\')
                  {
                     buf[j] = '\\';
                     j++;
                  }
                  buf[j] = pt[i];
                  j++;
               }
               buf[j] = 0;
               fprintf(fo,"%s\\par\n",buf);
               free(*_txt);
            }           
            fprintf(fo,"}}\n");
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

static int carga_fgvmenu(file1,texto,nomfile)
FILE *file1;
char *texto;
char *nomfile;
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
                              /*trataop(nm,0);*/
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
   char texto[512];
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
      carga_fgvmenu(file1,texto,NULL);
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

static void procesa_hoja_menu(FILE *f,int m,int *item,char *arrastre,int nivel)
{
   int i,n,j,conpar = 0,otraapli = 0,acceso = 0;
   char *seps[250];
   char tmp1[512];
   char tmp2[1024];
   
   nivel++;
   for (i = 0;i < wmenu[m]->m_nop;i++)
   {
      if (wmenu[m]->m_op[i]->o_menuhijo > -1)
      {
         if (wmenu[m]->m_op[i]->o_menuhijo < nmenus)
         {		   
            fprintf(f,"%d %s\n",nivel,wmenu[m]->m_op[i]->o_nombre);
            strcpy(tmp2,arrastre);
            if (arrastre[0])
            {
               strcat(tmp2,",");
            }
            strcat(tmp2,wmenu[m]->m_op[i]->o_nombre);
            procesa_hoja_menu(f,wmenu[m]->m_op[i]->o_menuhijo,item,tmp2,nivel);
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
            strcpy(tmp2,arrastre);
            if (arrastre[0])
            {
               strcat(tmp2,": ");
            }		   
            strcat(tmp2,wmenu[m]->m_op[i]->o_nombre);		   
            relacion_nombres[*item] = malloc(strlen(tmp2)+30);
            strcpy(relacion_nombres[*item],tmp1);		   
            strcpy(relacion_nombres[*item]+20,tmp2);
            (*item)++;
            if ((*item) >= tope_relacion_nombres)
            {
               tope_relacion_nombres += 1000;
               relacion_nombres = realloc(relacion_nombres,tope_relacion_nombres * sizeof(char **));
            }
            relacion_nombres[*item] = NULL;
            fprintf(f,"%d %s=%s_ref\n",nivel,wmenu[m]->m_op[i]->o_nombre,tmp1);
         }        
      }
   }
}



static void crea_rtf_standars(char *directorio,char *aplicacion,char **dir)
{
   char tmp[512];
   FILE *f;
   char **_dir = dir;
   int numero;
   
   sprintf(tmp,"%s/%s.hm",directorio,aplicacion);
   f = fopen(tmp,"w");
   if (!f) return;
   
   numero = 20000;
   for (_dir = dir;*_dir;_dir++)
   {
      fprintf(f,"%s_ref 0x%d\n",*_dir,numero);
      free(*_dir);
      numero++;
   }
   
   fclose(f);
}

static void procesa_menu(char *dir_hlp,char *aplicacion)
{
   char tmp[512];
   FILE *f;
   int nitem = 0;
   
   sprintf(tmp,"%s/%s.cnt",dir_hlp,aplicacion);
   f = fopen(tmp,"w");
   if (f)
   {
      fprintf(f,":Base %s.hlp\n",aplicacion);  
      fprintf(f,"1 %s\n",nommenu);
      nitem = 0;
      procesa_hoja_menu(f,0,&nitem,"",1);
      fclose(f);
   }
}

BOOL CALLBACK _CREA_AYUDA(HWND hWnd, UINT message,WPARAM wParam, LPARAM lParam)
{
   
   
   
   switch (message)
   {
   case WM_SYSCOMMAND:      
      break;
   case WM_CHAR:
      break;
   case WM_COMMAND:
      switch(wParam)
      {
      case IDOK:
         //empieza_dlg = 1;
         return FALSE;
      case IDCANCEL:
         //empieza_dlg = 2;
         return(FALSE);
      }	 
      break;
      case WM_INITDIALOG:
         {
            HWND mw;
            char tmp[512];
            mw = GetDlgItem(hWnd,IDC_ANIMADO);
            if (mw)
            {
               strcpy(tmp,rw_directorio("dev/ayuda.avi"));
               cambia_barra_dos(tmp);
               Animate_Open(mw,tmp);
            }
         }
         return(FALSE);
         break;
      case WM_DESTROY:
      /*
      if (wPadre)
      {
      SetActiveWindow(wPadre);
      SetFocus(wPadre);
      }
      dlg = NULL;
         */
         break;
      default:
         break;
   }
   
   return FALSE; 
}


#ifdef TEST
int main(int argc,char *argv[])
{
   char aplicacion[128];
   char directorio[256];
   char dir_rtf[256];
   char path_men[256];
   char **dir,**_dir;
   FILE *fglobal;
   /* FILE *fmenu;*/
   /* falta crear el tree en control y en rtf*/
   
   /* primer parametro la aplicacion*/
   /* con directorio a on*/
   if (argc < 2 || !argv[1][0]) {cout << "Debe indicar el directorio de la aplicacio\n";exit(1);}
   strcpy(directorio,argv[1]);
#else
int ds_rtfhlp(char *direct,char *usi)
   {
      char aplicacion[128];
      char directorio[256];
      char dir_rtf[256];
      char path_men[256];
      char **dir,**_dir;
      FILE *fglobal;
#endif
      int i;	  
      
      
      strcpy(directorio,direct);
      if (directorio[ strlen(directorio) - 1 ] == '/' || directorio[ strlen(directorio) - 1 ] == '\\')
      {
         directorio[ strlen(directorio) - 1 ] = 0;
      }
      
      strcpy(aplicacion,quita_path(directorio));
      sprintf(path_men,"%s/%s",directorio,aplicacion);
      strcat(directorio,"/ayuda");

      strcpy(dir_rtf,directorio);
      strcat(dir_rtf,"/campohlp.ref");
	  idx_global = rw_open_(dir_rtf,O_RDWR | O_BINARY | O_CREAT | O_TRUNC,S_IWRITE | S_IREAD);
	  if (idx_global > -1)
	  {
		  rw_lseek_(idx_global,0L,SEEK_SET);
	  }

      strcpy(dir_rtf,directorio);
      strcat(dir_rtf,"/*.txt");

	  
      if ((dir = lee_direct(dir_rtf,0)))
      {
#ifdef FGVWIN
         HINSTANCE  hInstancia = hInst;
#else
         HINSTANCE  hInstancia = GetModuleHandle (NULL);
#endif;    
         HWND dlg;
         MSG msg;
         
         if (dir[0])
         {
            
            
            dlg  = CreateDialogParam(  hInstancia,  // handle to application instance
               "CREA_AYUDA",  // identifies dialog box template
               NULL,      // handle to owner window
               _CREA_AYUDA, // pointer to dialog box procedure      
               (LPARAM)NULL);   // initialization value
            if (dlg)
            {
               char tmp[1024];
               sprintf(tmp,usi,aplicacion);	   
               SetWindowText(dlg,tmp);
               BringWindowToTop(dlg);
               SetForegroundWindow(dlg);           	   
               
               while (PeekMessage((LPMSG)&msg,dlg,0,0,PM_NOREMOVE) && GetMessage((LPMSG)&msg,dlg,0,0))
               {
                  if (!IsDialogMessage(dlg,&msg))
                  {
                     TranslateMessage((LPMSG)&msg);
                     DispatchMessage((LPMSG)&msg);
                  }	     
               }         
            }
            
            
            nommenu[0] = 0;
            strcpy(dir_rtf,directorio);
            strcat(dir_rtf,"/rtf");
            mkdir(dir_rtf);
            lee_menu(path_men);
            if (wmenu)
            {
               relacion_nombres = (char **)malloc(sizeof(char **) * 1000);
               tope_relacion_nombres = 1000;
               relacion_nombres[0] = NULL;
               procesa_menu(directorio,aplicacion);
               libera_menu();
            }
            
            fglobal = crea_relacion_global(dir_rtf,aplicacion,nommenu,directorio);
            for (_dir = dir;*_dir;_dir++)
            {
               (*_dir)[ strlen(*_dir) - 4 ] = 0; /* la extension .txt se da por supuesta */
               analiza_txt(directorio,aplicacion,*_dir,dir_rtf,fglobal);      
            }
            cierra_relacion_global(fglobal); 
            
            crea_rtf_standars(dir_rtf,aplicacion,dir);
            
            if (relacion_nombres)
            {
               for (i = 0;relacion_nombres[i];i++)
               {
                  free(relacion_nombres[i]);
               }
               free(relacion_nombres);
               relacion_nombres = NULL;
            }
            
            
            
            {
               char tmp[1024];
               
               sprintf(tmp,"%s -xtn %s/%s.hpj",rw_directorio("bin/hcrtf"),dir_rtf,aplicacion);
               cambia_barra_dos(tmp);
               fgv_system_oculto(tmp);	
               lee_y_borra(dir_rtf);
               
            }
            if (dlg)
               DestroyWindow(dlg);
            
         }
         
         free(dir);
         
      }

	  if (idx_global > -1)
	  {
		  rw_close_(idx_global);
		  idx_global = -1;
	  }
      
      return 0;
}

#endif