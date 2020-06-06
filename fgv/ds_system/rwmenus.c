
/*---------------------------------------------------------------------------
* Programa ..: rwmenus.c
* Fecha .....: 25-3-88
* Version ...: 1.00
* Notas .....: Rutinas de menus (simple, varias pantallas, con submenus) 
*              que salvan pantalla.
* Contenido .: int    leemenu(), varmenu(), w_menu(), w_menug(), w_menus(),
*                     nsmenu, def[MAXSUBMENU], pintamenu,
*              VENT  *pinmenu(), *mw[MAXSUBMENU]
*              char   nommenu[MAXNOMOP],
*              T_MENU **wmenu;
*---------------------------------------------------------------------------
*/

#include <fgvsys.h>

#ifndef FGVWIN

#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <malloc.h>

#include <rwmalloc.h>
#include <rtermin.h>
#include <rdef.h>
#include <teclas.h>     /* para leecar */
#include <rfecha.h>
#define _LOSMENUS
#include <rwmenus.h>

extern char *quita_path();
extern char *getenv();

#define TRUE          1
#define FALSE         0

#define min(a,b)  (((a) < (b)) ? (a) : (b))
#define max(a,b)  (((a) > (b)) ? (a) : (b))


extern int wmenu_en_alfa;

static int def[MAXSUBMENU];
static int lockm;
static int s_lock;
static int tipo;
static char nfm[15];
static int prime;
static char nom[MAXPATH];
static char pro[MAXNOMPRO];

extern char *nombre_tecla();

/* ===========================================================================
*  FUNCIONES
* ===========================================================================
*/

#ifdef FGV32
#ifndef FGVWIN
#ifndef WATCOMNLM
extern HANDLE myconsola;
extern HANDLE hStdIn;
#endif
#endif
#endif

#ifdef RWXARXA
extern int en_conexion_terminal;
extern char *lineamenu_en_server();
#ifdef RWSERVER
extern int en_conexion_externa;
extern int en_conexion_externa_stop;
#endif
#endif


#define coma_linea(a,b) comas_linea(a,b,1,&coma)


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
extern char clave_help[32];
extern int (*f_ayuda)();

extern int wdesplegamenu;

char *snap_shoot_menu = NULL;

int trataop(i,modo)
int i,modo;
{
   int hijo,k,numfi,mb,k1,k2,kn,n_bloque,max_b1,kk;
   long c_fichas;
   char *r;
   MENU_DEF *dird[501];
   char *ficheros[501];
   int memoria;
   int j = 0; /* flag de entrada */
   
   if (modo) {
      if (lockm == i) {
         j = 1;
         if (!s_lock)
            mb = 2;/* si no esta bloqueado solo comprobar si ahora lo esta*/
         else
            mb = 0;
      }
   }
   else 
   {
      for (r=wmenu[i]->m_op[0]->o_nombre;((*r) && (*r) != '.');r++);
      if (*r == '.' && memcmp(r,".di",3) == 0) {
         *r ='\0';
         tipo = atoi(r+4);
         lockm = i;
         if (*(r+3) == 'r')
         {
            if (dir_smenu != (char *)0)
               strcpy(nom,dir_smenu);
            else
               nom[0] = '\0';
         }
         else
         {
            if (dir_sgmenu != (char *)0)
               strcpy(nom,dir_sgmenu);
            else
               nom[0] = '\0';
         }      
         strcat(nom,wmenu[i]->m_op[0]->o_nombre);
         strcpy(pro,wmenu[i]->m_op[0]->o_programa); /* aqui extension */
         j = 1;
         mb = 0;
      }
   }
   
   if (j) {
      hijo   =   wmenu[i]->m_op[0]->o_menuhijo;         
      if ( (numfi = lee_dir(nom,dird,ficheros,mb*100+tipo)) < 1 ) 
      {
         s_lock = 1; /* bloqueado */
         free_omenu(i);
         wmenu[i]->m_nop = 1;
         wmenu[i]->m_op =  (T_OPCION **)malloc(sizeof(T_OPCION *));
         wmenu[i]->m_op[0] = (T_OPCION *)malloc(sizeof(T_OPCION));
         wmenu[i]->m_op[0]->o_nombre = malloc(12);
         strcpy(wmenu[i]->m_op[0]->o_nombre,"BLOQUEADO");
         wmenu[i]->m_op[0]->o_programa = malloc(5);
         *(wmenu[i]->m_op[0]->o_programa) = '\0';
         wmenu[i]->m_op[0]->o_menuhijo = hijo;
         return(-2);
      }
      s_lock = 0; /* leido */
      
      if (mb) return(0);
      
      free_omenu(i);
      
      /* creacion de submenus */
      /* primero ordenar el dir por submenus */
      
      n_bloque = 0;
      wmenu[i]->m_nop = ordena_dir_(dird,numfi);
      kn = wmenu[i]->m_nop;
      if (kn == 1)
      {
         wmenu[i]->m_nop = numfi;
         wmenu[i]->m_op = (T_OPCION **)malloc(numfi*sizeof(T_OPCION *));
         for (k1 = 0;k1 < numfi;k1++)
         {
            wmenu[i]->m_op[k1] = (T_OPCION *)malloc(sizeof(T_OPCION));
            wmenu[i]->m_op[k1]->o_menuhijo = -1;
            
            memoria = strlen(dird[k1]->m_titulo)+2;
            wmenu[i]->m_op[k1]->o_nombre = malloc(memoria);
            strcpy(wmenu[i]->m_op[k1]->o_nombre,dird[k1]->m_titulo);
            if ((wmenu[i]->m_cqp + memoria + 12) > 80)
            {
               wmenu[i]->m_cqp = 80 - memoria - 12;
               if (wmenu[i]->m_cqp < 1)
                  wmenu[i]->m_cqp = 1;
            }
            memoria = (strlen(pro) + strlen(dird[k1]->m_nombre) + 2);
            wmenu[i]->m_op[k1]->o_programa = malloc(memoria);
            sprintf(wmenu[i]->m_op[k1]->o_programa,"%s%s",dird[k1]->m_nombre,pro);
            free((MENU_DEF *)dird[k1]);
         }
         if ( (wmenu[i]->m_fqp+14) > 23) wmenu[i]->m_fqp = 23 - 14;
      }
      else
      {
         wmenu[i]->m_op = (T_OPCION **)malloc(kn*sizeof(T_OPCION *));
         k = 0;
         if ( (wmenu[i]->m_fqp+14) > 23) wmenu[i]->m_fqp = 23 - 14;
         wmenu = (T_MENU **)realloc(wmenu,sizeof(T_MENU *) * (nmenus + 2 + kn));		 
         for (k1 = 0;k1 < kn;k1++)
         {
            wmenu[i]->m_op[k1] = (T_OPCION *)malloc(sizeof(T_OPCION));
            wmenu[i]->m_op[k1]->o_menuhijo = nmenus;
            
            k2 = strbusca(dird[k]->m_titulo,"#");
            if (k2 > -1)
            {
               wmenu[i]->m_op[k1]->o_nombre = malloc(strlen(dird[k]->m_titulo+k2));
               strcpy(wmenu[i]->m_op[k1]->o_nombre,dird[k]->m_titulo+k2+1);
            }
            else
            {
               wmenu[i]->m_op[k1]->o_nombre = malloc(40);
               sprintf(wmenu[i]->m_op[k1]->o_nombre,__Texto[379],++n_bloque);
            }
            
            wmenu[i]->m_op[k1]->o_programa = malloc(2);
            *(wmenu[i]->m_op[k1]->o_programa) = 0;		   
            wmenu[nmenus] = (T_MENU *)malloc(sizeof(T_MENU));
            wmenu[nmenus]->m_op = (T_OPCION **)malloc(20*sizeof(T_OPCION *));
            max_b1 = 20;
            wmenu[nmenus]->m_fqp = wmenu[i]->m_fqp;
            wmenu[nmenus]->m_cqp = wmenu[i]->m_cqp+20;
            wmenu[nmenus]->m_menupadre = i;
            c_fichas = dird[k]->m_fichas;
            for (k2 = 0;/*k2 < 20*/ k < numfi && dird[k]->m_fichas == c_fichas;k2++,k++) 
            {			
               if (k2 >= max_b1)
               {
                  max_b1 += 20;
                  wmenu[nmenus]->m_op = (T_OPCION **)realloc(wmenu[nmenus]->m_op,max_b1*sizeof(T_OPCION *));
               }
               wmenu[nmenus]->m_op[k2] = (T_OPCION *)malloc(sizeof(T_OPCION));
               wmenu[nmenus]->m_op[k2]->o_menuhijo = -1;
               
               kk = strbusca(dird[k]->m_titulo,"#");
               if (kk > -1)
               {
                  dird[k]->m_titulo[kk] = 0;
               }
               
               memoria = strlen(dird[k]->m_titulo)+2;
               wmenu[nmenus]->m_op[k2]->o_nombre = malloc(memoria);
               strcpy(wmenu[nmenus]->m_op[k2]->o_nombre,dird[k]->m_titulo);
               if ((wmenu[nmenus]->m_cqp + memoria + 12) > 80)
               {
                  wmenu[nmenus]->m_cqp = 80 - memoria - 12;
                  wmenu[i]->m_cqp = wmenu[nmenus]->m_cqp - 20;
                  if (wmenu[i]->m_cqp < 1)
                     wmenu[i]->m_cqp = 1;
               }
               memoria = (strlen(pro) + strlen(dird[k]->m_nombre) + 2);
               wmenu[nmenus]->m_op[k2]->o_programa = malloc(memoria);
               sprintf(wmenu[nmenus]->m_op[k2]->o_programa,"%s%s",dird[k]->m_nombre,pro);
               free((MENU_DEF *)dird[k]);
            }		   
            wmenu[nmenus]->m_nop = k2;
            nmenus++;
         }
      }
      
      
#ifdef COMENTARIO_EJEMPLO
      wmenu[i]->m_nop = numfi / 20;
      if ((numfi % 20)) wmenu[i]->m_nop++;
      kn = wmenu[i]->m_nop;
      wmenu[i]->m_op = (T_OPCION **)malloc(kn*sizeof(T_OPCION *));
      k = 0;
      if ( (wmenu[i]->m_fqp+14) > 23) wmenu[i]->m_fqp = 23 - 14;
      wmenu = (T_MENU **)realloc(wmenu,sizeof(T_MENU *) * (nmenus + 2 + kn));
      for (k1 = 0;k1 < kn;k1++)
      {
         wmenu[i]->m_op[k1] = (T_OPCION *)malloc(sizeof(T_OPCION));
         wmenu[i]->m_op[k1]->o_menuhijo = nmenus;
         wmenu[i]->m_op[k1]->o_nombre = malloc(40);
         sprintf(wmenu[i]->m_op[k1]->o_nombre,__Texto[380],k1+1);
         wmenu[i]->m_op[k1]->o_programa = malloc(2);
         *(wmenu[i]->m_op[k1]->o_programa) = 0;		   
         wmenu[nmenus] = (T_MENU *)malloc(sizeof(T_MENU));
         wmenu[nmenus]->m_op = (T_OPCION **)malloc(20*sizeof(T_OPCION *));
         wmenu[nmenus]->m_fqp = wmenu[i]->m_fqp;
         wmenu[nmenus]->m_cqp = wmenu[i]->m_cqp+20;
         wmenu[nmenus]->m_menupadre = i;
         for (k2 = 0;k2 < 20 && k < numfi;k2++,k++) 
         {
            wmenu[nmenus]->m_op[k2] = (T_OPCION *)malloc(sizeof(T_OPCION));
            wmenu[nmenus]->m_op[k2]->o_menuhijo = -1;
            memoria = strlen(dird[k]->m_titulo)+2;
            wmenu[nmenus]->m_op[k2]->o_nombre = malloc(memoria);
            strcpy(wmenu[nmenus]->m_op[k2]->o_nombre,dird[k]->m_titulo);
            if ((wmenu[nmenus]->m_cqp + memoria + 12) > 80)
            {
               wmenu[nmenus]->m_cqp = 80 - memoria - 12;
               wmenu[i]->m_cqp = wmenu[nmenus]->m_cqp - 20;
               if (wmenu[i]->m_cqp < 1)
                  wmenu[i]->m_cqp = 1;
            }
            memoria = (strlen(pro) + strlen(dird[k]->m_nombre) + 2);
            wmenu[nmenus]->m_op[k2]->o_programa = malloc(memoria);
            sprintf(wmenu[nmenus]->m_op[k2]->o_programa,"%s%s",dird[k]->m_nombre,pro);
            free((MENU_DEF *)dird[k]);
         }		   
         wmenu[nmenus]->m_nop = k2;
         nmenus++;
      }
#endif
      return(1);
         }
         return(0);
}


static int wlee_un_submenu(padre,path,i,texto,file1,r)
int padre;
char *path;
int i;
char *texto;
FILE *file1;
int *r;
{
   register int j;
   char *elhijo;
   FILE *file2;
   int separa,separa2,separa3;
   int memoria,rrr;
   int coma;
   char tmp[258];
   
   t_lee_linea(texto,162,file1);
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
      t_lee_linea(texto,162,file1);
      separa = coma_linea(texto,0);
      memoria = strlen(texto+separa)+2;
      wmenu[i]->m_op[j]->o_nombre = malloc(memoria);
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
      wmenu[i]->m_op[j]->o_programa = malloc(strlen(tmp)+2);
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
      if (j == 0 && trataop(i,0) > 0) /* ha puesto menu ficheros o listados */
         break;
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


/*
* leemenu
*     lee 'nom'.men y lo pone en '*wmenu[]' y 'def[]'
*/
int leemenu(nnom)
char *nnom;
{
   FILE *file1;
   char nomfi[MAXPATH];
   char texto[300];
   register int i,final;
   int r;
   int coma;
   int convertir = 0;
   
   
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
   
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      wmenulee_en_terminal(nnom);
   }
#endif
#endif
   
   if ((file1 = t_rwfopen(nomfi,"r")) == (FILE *) 0)
   {
#ifdef RWXARXA
#ifdef RWSERVER
      if (en_conexion_externa)
      {
         wmenufinlee_en_terminal();
      }
#endif
#endif
      return(-1);
   }
   
   *(quita_path(nomfi)) = 0;
   
   t_lee_linea(texto,162,file1);
   quitabp(texto);
   if (texto[0] == '#' || texto[1] == ';')
   {
      carga_fgvmenu(file1,texto,nnom);
   }
   else
   {
      for (i=0;i<MAXSUBMENU;i++)
         def[i] = atoi(texto+coma_linea(texto,i));
      r = 0;
      wdef = def[r];
      t_lee_linea(texto,162,file1);
      strcpy(nommenu,texto+coma_linea(texto,0));
      nmenus = atoi(texto+coma_linea(texto,1));
      wmenu = (T_MENU **)malloc(sizeof(T_MENU *) * (nmenus + 1));
      final = nmenus;
      for (i=0;i<final;i++) 
      {
         wlee_un_submenu(-1,nomfi,i,texto,file1,&r);
      }
#ifdef RWXARXA
      if (!en_conexion_terminal)
#endif
         convertir = 1;
      
   }
   
   t_rwfclose(file1);
   
   
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      wmenufinlee_en_terminal();
   }
#endif
#endif
   
   if (convertir)
   {
      convierte_a_fgvmenu(nnom);
   }
   
   return(0);
}

/* varmenu
*    rutina que calcula variables del menu
*    devuelve: qpm -> posicion a pintar primera opcion
*/
static int varmenu(m,pag,nom,pnumero,ppaginas,pqpwi,pqpwf,usi)
int m;                     /* indice en 'menu' 0,1... */
int pag;                   /* pagina del menu 0,1,... */
char *nom;                 /* nombre del menu */
int *pnumero;              /* --> numero de opciones en esa pagina */
int *ppaginas;             /* --> numero de paginas del menu 0,1.. */
int *pqpwi;                /* --> posicion inicial ventana de menu */
int *pqpwf;                /* --> posicion  final  ventana de menu */
char *usi;                 /* --> usi con el que pintar opciones */
{
   int i,j,fqp,cqp,mlon,mlin;
   int numero,paginas,qpwi,qpwf,qpm;
   
   paginas = (wmenu[m]->m_nop - 1) / TAMANO;      /* paginas del menu 0,1,.. */
   if (paginas == 0)
      pag = 0;
   if (pag < paginas)          /* numero de opciones de la pagina 1..TAMANO */
      numero = TAMANO;
   else
      numero = (wmenu[m]->m_nop - (pag * TAMANO));
   mlin = numero + 5;             /* mlin , calcula lineas de la ventana */
   for (i=0,mlon=0;i<numero;i++)  /* mlon , calcula columnas de la ventana */
      if ((j = strlen(wmenu[m]->m_op[i+pag*TAMANO]->o_nombre)) > mlon)
         mlon = j;
      i = mlon+10;
      if ((strlen(nom) + 3) > i)
         mlon += strlen(nom) - i + 3;
      fqp = wmenu[m]->m_fqp;
      cqp = wmenu[m]->m_cqp;
      qpwi = max(fqp,-fqp)*100+max(cqp,-cqp); /* qpwi , 1.a esquina de ventana */
      if (fqp < 0)
         qpwi -= 100 * (mlin -1);
      if (cqp < 0)
         qpwi -= mlon + 13;
      if (fqp == 0)                /* centra fila */
         qpwi += 700;
      if (cqp == 0)                /* centra columna */
         qpwi += (80-mlon-14)/2+1;
      qpwf = qpwi + 100 * (mlin - 1) + mlon + 13;   /* ultima esquina */
      qpm  = qpwi + 308;             /* posicion del primer texto del menu */
      sprintf(usi," %%-%ds ",mlon);  /* using para el print */
      *pnumero  = numero;
      *ppaginas = paginas;
      *pqpwi    = qpwi;
      *pqpwf    = qpwf;
      return(qpm);
}

/* pinmenu
*    rutina que pinta una pagina del menu
*/
static VENT *pinmenu(m,pag,menunom,vw)
int m;                     /* indice en 'menu' 0,1... */
int pag;                   /* pagina del menu 0,1,... */
char *menunom;             /* nombre del menu */
int vw;
{
			VENT *wr,*w2;
         char usi[30];
         char usinom[30];
         int i;
         int sombra1 = 0;
         int sombra2 = 0;
         int numero,paginas,qpwi,qpwf,qpm;
         char aux[81];
         char *pros;
         qpm = varmenu(m,pag,menunom,&numero,&paginas,&qpwi,&qpwf,usi);
         
         if ((qpwi%100) > 1)  sombra1 = 1;
         if ((qpwf/100) < 23) sombra2 = 100;
         
         /*   wr = w_ini(qpwi-sombra1,qpwf+sombra2);*/
         wr = w_ini(qpwi,qpwf+sombra2+1);
         w_salva(wr);                     /* salva lo que habia en la ventana */
         
         
         
         Color(1,3);
         
         
         if (vw) 
         {
            Patrib = 'R';
            pon_color(REVCOLOR,REVFONDO,0,1);
            w2 = w_ini(qpwi,qpwf);
            w_cls(w2);
            w_marco_(w2,w2->vini,w2->vfin,0);
            w_mete(stdpan,w2);
            w_fin(w2);
            Patrib = ' ';
            pon_color(NORCOLOR,NORFONDO,0,1);
         }
         else 
         {
            Ar();/*****/
            zonablan(qpwi,qpwf);             /* pone en blanco la zona del menu */
            bi_cuadro(qpwi,qpwf,0);               /* recuadra el menu */
            Cr();/*****/
         }
         
         /*   i = (qpwf-qpwi-6)%100;           /* pinta nombre del menu */
         /*   sprintf(usinom,"[%%02d] %%-%ds",i); */
         /*   sprintf(aux,usinom,m,menunom);*/
         i = (qpwf-qpwi-3)%100;           /* pinta nombre del menu */
         sprintf(usinom," %%-%ds ",i);
         sprintf(aux,usinom,menunom);
         
         Color(7,1);   
         
         if (vw) 
         {
            Patrib = 'I';
            pon_color(INTCOLOR,INTFONDO,0,1);
            qursor(qpwi + 101);
            w_print(stdpan,aux);
            Patrib = ' ';
            pon_color(NORCOLOR,NORFONDO,0,1);
         }
         else 
         {
            Ai();/*****/
            cursor(qpwi + 101);
            v_print(aux);
            Bi();/*****/
         }
         
         for (i=0;i<numero;i++) {         /* pinta opciones */
            if (vw) {
               qursor(qpm + i * 100 - 5);
               Color(7,1);
               Patrib = 'I';
               pon_color(INTCOLOR,INTFONDO,0,1);
               if (wmenu_en_alfa)
                  w_printc(stdpan," %c ",'A'+i);
               else
                  w_printc(stdpan," %c ",'0'+i);
               /* Color(15,3); */
               Color(0,3);
               Patrib = 'R';
               pon_color(REVCOLOR,REVFONDO,0,1);
               w_print(stdpan,"ÄÄ");
               w_prints(stdpan,usi,wmenu[m]->m_op[i+pag*TAMANO]->o_nombre);
               /*
               if (wmenu[m]->m_op[i+pag*TAMANO]->o_menuhijo >= 0)
               w_print(stdpan,"Ä>");
               */  
               Patrib = ' ';
               pon_color(NORCOLOR,NORFONDO,0,1);
            }
            else {
               cursor(qpm + i * 100 - 5);
               Color(7,1);
               Ai();
               if (wmenu_en_alfa)
                  v_printc(" %c ",'A'+i);
               else
                  v_printc(" %c ",'0'+i);
               Bi();
               /* Color(15,3); */
               Color(0,3);
               Ar();/*****/
               v_print("ÄÄ");
               v_prints(usi,wmenu[m]->m_op[i+pag*TAMANO]->o_nombre);
               /*
               if (wmenu[m]->m_op[i+pag*TAMANO]->o_menuhijo >= 0)
               v_print("Ä>");
               */
               Cr();/*****/
            }
         }
         
         if (paginas) {                   /* pinta senyal de mas paginas */
            if (vw) {
               /* Color(15,3); */
               Color(0,3);
               Patrib = 'R';
               pon_color(REVCOLOR,REVFONDO,0,1);
               qursor(qpm + numero * 100 - 5);
               w_print(stdpan," ..Ä>");
               Patrib = ' ';
               pon_color(NORCOLOR,NORFONDO,0,1);
            }
            else {
               /* Color(15,3); */
               Color(0,3);
               Ar();/*****/
               cursor(qpm + numero * 100 - 5);
               v_print(" ..Ä>");
               Cr();/*****/
            }
         }
         
         
         Color(7,0);
         Nor();
         if (sombra1) {
            Gon();
            /*      for (i = qpwi + 99;i < (qpwf+100) && i < 2400;i += 100) {*/
            i = (qpwi / 100) * 100 + (qpwf%100) + 101;
            for (;i < (qpwf+102) && i < 2400;i += 100) {
               if (vw) {
                  qursor(i);
                  w_putc(stdpan,C_GRC1);
               }
               else {
                  cursor(i);
                  Graf(12);
               }
            }
            Goff();
         }
         if (sombra2) {
            i = (qpwf/100 + 1)*100 + qpwi%100;
            Gon();
            if (vw)
               qursor(i+1);
            else
               cursor(i+1);
            for(;i < (qpwf + 100);i++) {
               if (vw) {
                  w_putc(stdpan,C_GRC1);
                  Pcursor++;
               }
               else
                  Graf(12);
            }
            Goff();
         }
         
         return(wr);
}

/* w_menu
*    rutina de menus de una pagina
*    devuelve el numero de opcion elegida
*    o bien 0=esc  -1=ctrl-c  (si hay varias paginas -2=pgup -3=pgdn)
*/
static int w_menu(pwm,pws,m,pag,defecto)
VENT **pwm;        /* ventana donde debe guardar lo de abajo del menu */
VENT **pws;        /* ventana donde debe guardar lo de abajo del submenu */
int m;             /* indice em 'wmenu[]' del menu */
int pag;           /* pagina del menu a preguntar 0,1,... */
int defecto;       /* opcion por defecto, si es 0 se toma 1 */
{
   int mientras = -1;
   int elegida = ((defecto <= 0) ? 1 : defecto);
   int anterior = 0;
   char nom[81];
   char usi[30];
   int c,i;
   int numero,paginas,qpwi,qpwf,qpm;
   int salir = 0;
   char *pros;
   
   if ((i = wmenu[m]->m_menupadre) < 0)
      strcpy(nom,nommenu);
   else
      strcpy(nom,wmenu[i]->m_op[def[nsmenu-1]-1]->o_nombre);
   
   if (pintamenu)
      *pwm = pinmenu(m,pag,nom,prime);
   
   qpm = varmenu(m,pag,nom,&numero,&paginas,&qpwi,&qpwf,usi);
   if (elegida > numero)
      elegida = 1;
   while (mientras) {
      if (anterior != elegida)  {
         if (pintahmenu) {
            Color(7,1);
            if (prime) {
               qursor(qpm+(elegida-1)*100);
               Patrib = 'I';
               pon_color(INTCOLOR,INTFONDO,0,1);
               w_prints(stdpan,usi,wmenu[m]->m_op[(elegida-1+pag*TAMANO)]->o_nombre);
               Patrib = ' ';
               pon_color(NORCOLOR,NORFONDO,0,1);
            }
            else {
               cursor(qpm+(elegida-1)*100);
               Ai();
               v_prints(usi,wmenu[m]->m_op[(elegida-1+pag*TAMANO)]->o_nombre);
               Bi();
            }
            i = wmenu[m]->m_op[(elegida-1+pag*TAMANO)]->o_menuhijo;
            if (i >= 0) { /* cambia opcion y elegida tiene subm */
               if (wdesplegamenu)
                  *pws = pinmenu(i,0,wmenu[m]->m_op[(elegida-1+pag*TAMANO)]->o_nombre,prime);
            }
         }
      }
      if (prime) {
         v_refresca();
         prime = 0;
      }
      clave_help[31] = (qpm+(elegida-1)*100)/100;
      pintamenu = TRUE;
      pintahmenu = TRUE;
      cursor(2378);
      if (snap_shoot_menu)
      {
         graba_bmp_pantalla(snap_shoot_menu);
         snap_shoot_menu = NULL;
         c = 807;
      }
      else
      {
         if (salir)
            c = CRETURN;
         else
            c = leecar();
      }
      anterior = elegida;
      
      if (c == SOLICITA_POSICION)
      {
         i = _M_Cursor % 100;
         if (i >= (qpwi % 100) && i <= (qpwf % 100))
         {
            i = _M_Cursor / 100;
            if (i >= (qpm / 100))
            {
               i -= (qpm / 100);
               if (i < wmenu[m]->m_nop)
               {
                  elegida = i + 1;
                  if (elegida == anterior)
                  {
                     c = CRETURN;
                  }
               }
            }
         }
      }
      
      switch (c) {

          /*
      case 831:
          entra_empresa(NULL,NULL);
          break;
          */

      case HOME :
         elegida = 1;
         break;
      case END  :
         elegida = numero;
         break;
      case PGUP :
         if (paginas) {    /* hay varias paginas */
            elegida = -2;
            mientras = 0;
         }
         break;
      case PGDN :
         if (paginas) {    /* hay varias paginas */
            elegida = -3;
            mientras = 0;
         }
         break;
      case ARRIBA :
      case RETROCEDER :
      case BACKSPACE :
      case '-' :
         elegida--;
         if (elegida < 1) {             /* sale por arriba */
            if (paginas) {     /* hay varias pantallas */
               elegida = -2;
               mientras = 0;
            }
            else
               elegida = numero;
         }
         break;
      case ABAJO :
      case ' ' :
      case '+' :
         elegida++;
         if (elegida > numero) {   /* sale por abajo */
            if (paginas) {     /* hay varias pantallas */
               elegida = -3;
               mientras = 0;
            }
            else
               elegida = 1;
         }
         break;
      case DERECHA :
         i = wmenu[m]->m_op[(elegida-1+pag*TAMANO)]->o_menuhijo;
         if (i < 0)
            break; /* no hay submenu */
      case VALIDAR :
      case CRETURN :
         mientras = 0;
         break;
      case IZQUIERDA :
         i = wmenu[m]->m_menupadre;
         if (i < 0)  /* no tiene menu padre */
            break;
      case ACABAR :
      case ESC :
         elegida = 0;
         mientras = 0;
         break;
      case ABORTAR :
      case CTRLC :
         elegida = -1;
         mientras = 0;
         break;
      case FUNCI_1:			     
         qpm = varmenu(m,pag,nom,&numero,&paginas,&qpwi,&qpwf,usi);
         if ((qpwi%100) > 1)
         {
            wmenu[m]->m_cqp--;
            w_pinta(*pwm);
            w_fin(*pwm);
            *pwm = pinmenu(m,pag,nom,prime);
            qpm = varmenu(m,pag,nom,&numero,&paginas,&qpwi,&qpwf,usi);
         }
         break;
      case FUNCI_2:
         qpm = varmenu(m,pag,nom,&numero,&paginas,&qpwi,&qpwf,usi);
         if ((qpwf%100) < 80)
         {
            wmenu[m]->m_cqp++;
            w_pinta(*pwm);
            w_fin(*pwm);
            *pwm = pinmenu(m,pag,nom,prime);
            qpm = varmenu(m,pag,nom,&numero,&paginas,&qpwi,&qpwf,usi);
         }
         break;
      case FUNCI_3:
         qpm = varmenu(m,pag,nom,&numero,&paginas,&qpwi,&qpwf,usi);
         if ((qpwi/100) > 1)
         {
            wmenu[m]->m_fqp--;
            w_pinta(*pwm);
            w_fin(*pwm);
            *pwm = pinmenu(m,pag,nom,prime);
            qpm = varmenu(m,pag,nom,&numero,&paginas,&qpwi,&qpwf,usi);
         }
         break;
      case FUNCI_4:
         qpm = varmenu(m,pag,nom,&numero,&paginas,&qpwi,&qpwf,usi);
         if ((qpwf/100) < 80)
         {
            qpm = varmenu(m,pag,nom,&numero,&paginas,&qpwi,&qpwf,usi);
            wmenu[m]->m_fqp++;
            w_pinta(*pwm);
            w_fin(*pwm);
            *pwm = pinmenu(m,pag,nom,prime);
            qpm = varmenu(m,pag,nom,&numero,&paginas,&qpwi,&qpwf,usi);
         }
         break;
      default :
         c = toupper(c);
         if (wmenu_en_alfa)
            i = c - 'A' + 1;
         else
            i = c - '0' + 1;
         if (i > 0 && i <= numero) {
            elegida = i;
            salir = 1;
         }
         break;
         }
         
         if (anterior != elegida)  {        /* ha cambiado de opcion */
            i = wmenu[m]->m_op[(anterior-1+pag*TAMANO)]->o_menuhijo;
            if (i >= 0) {           /* y antes habia submenu */
               if (wdesplegamenu)
               {
                  w_pinta(*pws);
                  w_fin(*pws);
               }
            }
            cursor(qpm+(anterior-1)*100);   /* quitar reverse */
            /* Color(15,3); */
            Color(0,3);
            Ar();/*****/
            v_prints(usi,wmenu[m]->m_op[(anterior-1+pag*TAMANO)]->o_nombre);
            Cr();/*****/
         }
      }
      
      i = wmenu[m]->m_menupadre;
      
      if (elegida == 0 && i >= 0 && (pag != 0 || !wdesplegamenu)) 
      { /* <ESC>, es hijo y en otra pag */
         w_pinta(*pwm);
         w_fin(*pwm);
         if (wdesplegamenu)
            *pwm = pinmenu(m,0,wmenu[i]->m_op[def[nsmenu-1]-1]->o_nombre,0);
      }
      else
         if (elegida < 0 || (elegida == 0 && i < 0)) 
         { /* <^C>,<PGDoU> o es padre */
            w_pinta(*pwm);
            w_fin(*pwm);
         }
         
         return(elegida);
}


/* w_menug
*    rutina de menus para muchas opciones
*    devuelve el numero de opcion elegida
*    o bien 0=esc  -1=ctrl-c
*/
static int w_menug(pwm,pws,m,defecto)
VENT **pwm;        /* ventana donde debe guardar lo de abajo del menu */
VENT **pws;        /* ventana donde debe guardar lo de abajo del submenu */
int m;             /* indice em 'wmenu[]' del menu */
int defecto;       /* opcion por defecto, si es 0 se toma 1 */
{
   int mientras = -1;
   int elegida = ((defecto <= 0) ? 1 : ((defecto-1)/TAMANO+1));
   int defec   = ((defecto <= 0) ? 1 : ((defecto-1)%TAMANO+1));
   int anterior = 0;
   int c,bloques,resto;
   
   bloques = (wmenu[m]->m_nop-1)/TAMANO+1;   /* numero de pantallas */
   resto   = (wmenu[m]->m_nop-1)%TAMANO+1;   /* opciones en la ultima */
   if (elegida > bloques) {
      elegida = 1;
      defec = 1;
   }
   while (mientras) {
      c = w_menu(pwm,pws,m,elegida-1,defec);
      defec = 0;        /* solo toma valor la primera vez */
      anterior = elegida;
      switch (c) {
      case 0 :                 /* escape */
         elegida = 0;
         mientras = 0;
         break;
      case -1 :                /* ctrl-c */
         elegida = -1;
         mientras = 0;
         break;
      case -2 :                /* pgup */
         elegida--;
         if (elegida < 1)
            elegida = bloques;
         if (elegida == bloques - 1)
            defec = TAMANO;
         else
            defec = resto;
         break;
      case -3 :                /* pgdn */
         elegida++;
         if (elegida > bloques)
            elegida = 1;
         break;
      default :
         mientras = 0;
         break;
      }
   }
   if (elegida > 0)
      return(c+(elegida-1)*TAMANO);
   return(elegida);
}

/*
* w_menus
*    si se desea volver al menu la pantalla ha de quedar igual
*    en el 'pim' devuelve el menu elejido
*    y como retorno la opcion del menu elejida , 0 <ESC> , -1 <^C>
*/
int w_menus(pim)
int *pim;            /* --> puntero al indice del menu */
{
   char nom[81];
   int i,j,k,pag,n;
   int numero,paginas,qpwi,qpwf,qpm;
   char usi[20];
   
   
   if (!wmenu)
      return(-1);
   
   
   if (!pim) 
			{
#ifdef RWSERVER
      funcion_windows_cliente(1013,&wdef,sizeof(int));
#endif
      return(-1);
			}
   
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
			{
      do  {
         i = wmenu_en_terminal(*pim);
         if (i > (nmenus*100+99))
         {                
            return(-1);
         }
         if (i < 0)
         {
            return(i);
         }
         else
         {
            *pim = i/100;
            return(i%100);
         }
      } while(-1);
			}
#endif
#endif
   
   
   
   Patrib = 'R';
   pon_color(REVCOLOR,REVFONDO,0,1);
   w_ll(stdpan,4);
   sprintf(nom,__Texto[381],nombre_tecla(802),nombre_tecla(807));
   prime = 401 + ((80 - strlen(nom)) / 2);
   if (prime < 401) prime = 401;
   qursor(prime);
   prime = 1;
   w_print(stdpan,nom),
      fecha();
   sprintf(nom,"%s %s",fechas+20,fechas+30);
   w_ll(stdpan,24);
   qursor(2401 + (80 - strlen(nom))/2);
   w_print(stdpan,nom);
   Patrib = ' ';
   pon_color(NORCOLOR,NORFONDO,0,1);
   memset(stdpan->vv+320,C_GRC1,1520);
   memset(stdpan->vv+2320,(unsigned char)(((7/*7 fondo */ << 4) & 0xf0) | 0 /*15 caracter */),1600);
   memset(stdpan->vv+4320,'R',1600);
   n = wdef/100;k = wdef%100;
   j = 0;
   def[j] = k;
   while(wmenu[n]->m_menupadre > -1)
			{
      for (i = 0;i <  wmenu[ (wmenu[n]->m_menupadre) ]->m_nop;i++)
      {
         if ( (wmenu[ (wmenu[n]->m_menupadre) ]->m_op[i]->o_menuhijo) == n)
         {
            j++;
            def[j] = i + 1;
            break;
         }
      }
      n = wmenu[n]->m_menupadre;
			}
   
   for (i = 0;i < ((j+1)/2);i++)
			{
      n = def[i];
      def[i] = def[j-i];
      def[j-i] = n;
			}
   for (i = j+1;i < MAXSUBMENU;i++)
      def[i] = 0;
   
   for (nsmenu=0,k=0;nsmenu<MAXSUBMENU-1;nsmenu++) {
      if (def[nsmenu] > wmenu[k]->m_nop) { /* coje k,nsmenu segun defectos */
         def[nsmenu] = 1;
         break;
      }
      if (def[nsmenu+1] <= 0 || wmenu[k]->m_op[def[nsmenu]-1]->o_menuhijo < 0) {
         def[nsmenu+1] = 0;
         break;
      }
      if (pintamenu) {
         /* trataop(k,1); */
         pag = (def[nsmenu]-1)/TAMANO;
         if ((i = wmenu[k]->m_menupadre) < 0)
            strcpy(nom,nommenu);
         else
            strcpy(nom,wmenu[i]->m_op[def[nsmenu-1]-1]->o_nombre);
         mw[nsmenu] = pinmenu(k,pag,nom,1);
         qpm = varmenu(k,pag,nom,&numero,&paginas,&qpwi,&qpwf,usi);
         qursor(qpm+100*((def[nsmenu]-1)%TAMANO));
         Color(7,1);
         Patrib = 'I';
         pon_color(INTCOLOR,INTFONDO,0,1);
         w_prints(stdpan,usi,wmenu[k]->m_op[def[nsmenu]-1]->o_nombre);
         Patrib = ' ';
         pon_color(NORCOLOR,NORFONDO,0,1);
      }
      k = wmenu[k]->m_op[def[nsmenu]-1]->o_menuhijo;
			}
   
   do {
      i = k;                                       /* menu */
      j = w_menug(&mw[nsmenu],&mw[nsmenu+1],i,def[nsmenu]);  /* opcion */
      pintamenu = FALSE;
      if (j < 1) {                    /* sale con <ESC> o <^C> */
         pintahmenu = FALSE;
         def[nsmenu] = 0;
         nsmenu--;
         if (j == 0)   /* <ESC> */
            k = wmenu[i]->m_menupadre;
         else          /* <^C> */
            k = -1;
      }
      else {                          /* elije opcion */
         pintahmenu = TRUE;
         def[nsmenu] = j;
         k = wmenu[i]->m_op[j-1]->o_menuhijo;
         if (k >= 0) {                /* elije opcion con submenu */
            j = 0; /*trataop(k,1);*/
            if (!wdesplegamenu)
               mw[nsmenu+1] = pinmenu(k,0,wmenu[i]->m_op[def[nsmenu]-1]->o_nombre,0);              
            if (j != 0) {
               if (wdesplegamenu)
               {
                  w_pinta(mw[nsmenu+1]);
                  w_fin(mw[nsmenu+1]);
                  mw[nsmenu+1] = pinmenu(k,0,wmenu[i]->m_op[def[nsmenu]-1]->o_nombre,0);                  
               }
            }
            if (j < 0) {
               pito(171);
               pintahmenu = FALSE;
               k = i;
            }
            else {
               nsmenu++;
               def[nsmenu] = 1;
            }
         }
         else {
            /*comprobar si el menu de la opcion ejecuatble no esta bloqueado*/
            /* esta bloqueado crear una especie de escape */
            /* se supone que aqui no hay submenus */
            /*
            if (trataop(i,1) < 0) {
            w_pinta(mw[nsmenu]);
            w_fin(mw[nsmenu]);
            def[nsmenu] = 0;
            nsmenu--;
            k = wmenu[i]->m_menupadre;
            pintahmenu = TRUE;
            pito(172);
            }
            */
         }
      }
			} while (k >= 0);
   if (j < 1) {      /*  sale de menu con menupadre < 0  o ^C */
      for (n=nsmenu;n>=0;n--) {
         w_pinta(mw[n]);
      }
      pintamenu = TRUE;
      pintahmenu = TRUE;
			}
   for (n=nsmenu;n>=0;n--) {
      w_fin(mw[n]);
			}
   *pim = i;
   if (j >= 0)
      wdef = i * 100 + j;
   return(j);
}

free_omenu(i)
int i;
{
   int j;
   
   for (j=0;j<wmenu[i]->m_nop;j++) {
      free(wmenu[i]->m_op[j]->o_nombre);
      free(wmenu[i]->m_op[j]->o_programa);
      free((char *)wmenu[i]->m_op[j]);
   }
   free((char *)wmenu[i]->m_op);
}

free_menu(i)
int i;
{
   free_omenu(i);
   free((char *)wmenu[i]);
}

/* libera
*  Libera la memoria del menu
*/
libera()
{
			int i;
         
         
         if (!wmenu) return(0);
         
#ifdef RWXARXA
#ifdef RWSERVER
         if (en_conexion_externa)
         {
            menulibera_en_terminal();
         }
#endif
#endif
         
         for (i = 0; i < nmenus ;i++)
            free_menu(i);
         nmenus = 0;
         free((char *)wmenu);
         wmenu = NULL;
         return(0);
}


#endif

