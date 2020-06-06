
/******************************************************************************/

/* wwmenu.c :  Rutinas para el tratamiento del menu standar de Windows         */

/******************************************************************************/


#include <fgvsys.h>

#ifdef FGVWIN

#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
#include <winuser.h>

#include <string.h>
#include <memory.h>
#include <malloc.h>

#include <rwmalloc.h>
#include <rtermin.h>
#include <rdef.h>
#include <teclas.h>
#include <rfecha.h>
#define _LOSMENUS
#include <rwmenus.h>

#include <FGVWIN.h>

static int libera_todo = 0;

extern char *quita_path();

#define TRUE          1
#define FALSE         0

#define min(a,b)  (((a) < (b)) ? (a) : (b))
#define max(a,b)  (((a) > (b)) ? (a) : (b))


static int def[MAXSUBMENU];
static int lockm;
static int s_lock;
static int tipo;
static char nom[MAXPATH];
static char pro[MAXNOMPRO];
static char nfm[15];
static int prime;

extern char *nombre_tecla();

extern int fuerza_empresa;


/* ===========================================================================
*  FUNCIONES
* ===========================================================================
*/
char *snap_shoot_menu = NULL;

extern int wmenu_en_alfa;

#define coma_linea(a,b) comas_linea(a,b,1,&coma)
extern char *lee_linea();

#ifdef RWXARXA
extern FILE *t_rwfopen();
extern char *t_lee_linea();
extern int   t_rwfclose();
#else
#define t_rwfopen   fopen
#define t_lee_linea lee_linea
#define t_rwfclose  fclose
#endif

extern int (*f_ayuda)();
extern char clave_help[32];

extern short wdesplegamenu;

int que_menu = 0;

#ifdef RWXARXA
extern int en_conexion_terminal;
extern char *lineamenu_en_server();
#ifdef RWSERVER
extern int en_conexion_externa;
extern int en_conexion_externa_stop;
#endif
#endif

char **tabla_icon = NULL;
short *punto_tabla = NULL;
int ntabla_icon = 0;


/***************************************************************************************************************/
#include <fgvmnexp.h>

static MenuStruct *Crea_Menu_Struct(int indice,char *nombre)
{
   MenuStruct *el_menu = NULL;
   int i;
   long id;
   
   el_menu = (MenuStruct *)malloc(sizeof(MenuStruct));
   el_menu->Ident[0] = 1;
   el_menu->Ident[1] = 2;
   el_menu->Ident[2] = 3;
   el_menu->Ident[3] = 0;
   el_menu->Items = (ItemStruct *)malloc(sizeof(ItemStruct) * (wmenu[indice]->m_nop+1));
   el_menu->NomMenu = malloc(strlen(nombre)+1);
   strcpy(el_menu->NomMenu,nombre);
   for (i = 0;i < wmenu[indice]->m_nop;i++)
   {
      el_menu->Items[i].NomItem = malloc(strlen(wmenu[indice]->m_op[i]->o_nombre)+1);
      strcpy(el_menu->Items[i].NomItem,wmenu[indice]->m_op[i]->o_nombre);
      if (wmenu[indice]->m_op[i]->o_menuhijo > -1)
      {
         el_menu->Items[i].OpItem = (char *)Crea_Menu_Struct(wmenu[indice]->m_op[i]->o_menuhijo,wmenu[indice]->m_op[i]->o_nombre);
      }
      else
      {
         el_menu->Items[i].OpItem = malloc(strlen(wmenu[indice]->m_op[i]->o_programa)+2+sizeof(long));
         id = indice * 100 + i;
         el_menu->Items[i].OpItem[0] = 10;
         memcpy(el_menu->Items[i].OpItem+1,&id,sizeof(long));
         strcpy(el_menu->Items[i].OpItem+1+sizeof(long),wmenu[indice]->m_op[i]->o_programa);
      }
      //wmenu[indice]->m_op[i];
   }
   el_menu->Items[i].NomItem = NULL;
   el_menu->Items[i].OpItem = NULL;
   
   return el_menu;
}


MenuStruct *obtiene_menu()
{
#ifdef COMENTARIO
   char   _nommenu[MAXNOMOP];   /* nombre del menu principal */
   T_MENU **_wmenu;    /* estructuras de menus */
   int    _nsmenu;              /* submenu actual 0,1..,MAXSUBMENU-1 */
   int    _wdef;
   int    _nmenus;
   int    _pintamenu;           /* si debe o no pintar el menu */
   int    _pintahmenu;          /* si debe o no pintar el menu hijo */
   char *_dir_smenu;
   char *_dir_sgmenu;
   char  _fichemenu[128];
#endif
   MenuStruct *el_menu = NULL;
   char path_menu[256];
   
#ifdef COMENTARIO
   memcpy(_nommenu,nommenu,sizeof(nommenu));
   _wmenu = wmenu;
   _nsmenu = nsmenu;
   _wdef = wdef;
   _nmenus = nmenus;
   _pintamenu = pintamenu;
   _pintahmenu = pintahmenu;
   _dir_smenu = dir_smenu;
   _dir_sgmenu = dir_sgmenu;
   memcpy(_fichemenu,fichemenu,sizeof(fichemenu));
   
   
   nommenu[0] = 0;
   wmenu = NULL;    
   nsmenu = 0;          /* submenu actual 0,1..,MAXSUBMENU-1 */
   wdef = 0;
   nmenus = 0;
   pintamenu = 0;       /* si debe o no pintar el menu */
   pintahmenu = 0;      /* si debe o no pintar el menu hijo */
   _dir_smenu = (char *)0;
   _dir_sgmenu = (char *)0;
   fichemenu[0] = 0;
   
   
   sprintf(path_menu,"%sagi",rw_directorio(""));
   
   if (!leemenu(path_menu))
   {
      el_menu = Crea_Menu_Struct(0,nommenu);
      libera();
   }
#else
   el_menu = Crea_Menu_Struct(0,nommenu);
#endif
   
   
#ifdef COMENTARIO
   memcpy(nommenu,_nommenu,sizeof(nommenu));
   wmenu = _wmenu;
   nsmenu = _nsmenu;
   wdef = _wdef;
   nmenus = _nmenus;
   pintamenu = _pintamenu;
   pintahmenu = _pintahmenu;
   dir_smenu = _dir_smenu;
   dir_sgmenu = _dir_sgmenu;
   memcpy(fichemenu,_fichemenu,sizeof(fichemenu));
#endif
   
   return(el_menu);
}


void LiberaMenuStruct(MenuStruct *menu)
{
   int i;
   for (i = 0;menu->Items[i].NomItem;i++)
   {
      if (menu->Items[i].OpItem[0] == 1 && menu->Items[i].OpItem[1] == 2 && menu->Items[i].OpItem[2] == 3 && menu->Items[i].OpItem[3] == 0)
      {
         LiberaMenuStruct((MenuStruct *)menu->Items[i].OpItem);
      }
      else
      {
         free(menu->Items[i].OpItem);
      }
      free(menu->Items[i].NomItem);
   }
   free((char *)menu->NomMenu);
   free((char *)menu->Items);
   free((char *)menu);
}

/***************************************************************************************************************/

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
      if (lockm == i) 
	  {
         j = 1;
         if (!s_lock)
            mb = 2;/* si no esta bloqueado solo comprobar si ahora lo esta*/
         else
            mb = 0;
      }
   }
   else {
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
         strcpy(wmenu[i]->m_op[0]->o_nombre,__Texto[454]);
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
               sprintf(wmenu[i]->m_op[k1]->o_nombre,__Texto[414],++n_bloque);
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
         sprintf(wmenu[i]->m_op[k1]->o_nombre,__Texto[415],k1+1);
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


/*** Windows ***/

static void w_crea_submenu(i,men)
int i;
HMENU men;
{
   int j;
   HMENU tmppopup;
   for (j = 0;j < wmenu[i]->m_nop;j++)
   {
      if (wmenu[i]->m_op[j]->o_menuhijo > -1)
      {
         AppendMenu(men, MF_ENABLED | MF_POPUP | MF_STRING,(UINT)(tmppopup = CreatePopupMenu()),wmenu[i]->m_op[j]->o_nombre);
         w_crea_submenu(wmenu[i]->m_op[j]->o_menuhijo,tmppopup);
      }
      else
      {
         AppendMenu(men, MF_ENABLED | MF_STRING,MID_XPRIMERA+i*MAXOP+j,wmenu[i]->m_op[j]->o_nombre);
      }
   }
}

void duplica_wwmenus()
{
   if (wwmenus && nwwmenus)
			{
      char ***_wwmenus = wwmenus;
      int *_nwwmenus = nwwmenus;    
      int i,j;
      
      wwmenus = (char ***)malloc(sizeof(char *) * n_wmenus);
      nwwmenus = (int *)malloc(sizeof(char *) * n_wmenus);
      for (i = 0;i < n_wmenus;i++)
      {
         wwmenus[i] = (char **)malloc(sizeof(char *) * _nwwmenus[i]);
         nwwmenus[i] = _nwwmenus[i];
         for (j = 0;j < _nwwmenus[i];j++)
         {
            wwmenus[i][j] = malloc(strlen(_wwmenus[i][j])+1);
            strcpy(wwmenus[i][j],_wwmenus[i][j]);
         }
      }
			}
}

void wwlibera()
{
   int i,j;
   HMENU tmp = NULL;
   if (wwmenus && nwwmenus)
			{
         /*
         if (principal_menu)
         {
         tmp = principal_menu;
         principal_menu = LoadMenu(hInst, "AGIXMENU");
         SetMenu(principal_w, principal_menu);
         DestroyMenu(tmp);
         RedrawWindow(principal_w,NULL,NULL,RDW_FRAME | RDW_INVALIDATE);
         }
      */
      for (i = 0;i < n_wmenus;i++)
      {
         for (j = 0;j < nwwmenus[i];j++)
         {
            free(wwmenus[i][j]);
         }
         free((char *)wwmenus[i]);
      }
      free((char *)wwmenus);
      free((int *)nwwmenus);
      wwmenus = NULL;
      nwwmenus = NULL;
      n_wmenus = 0;
			}
}


/****************/


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
   int memoria;
   int coma,rrr;
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
      if (i == (wdef / 100) && ((wdef%100) - 1) == j) {
         if (wmenu[i]->m_op[j]->o_menuhijo > -1) {
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
   char nomfi[300];
   char texto[300];
   char tmp[300];	
   int i,j,r,final;
   int coma;
   int convertir = 0;
   /*** Windows ***/
   HMENU tmppopup;
   HMENU oldmenu;
   /***************/
   
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
      return(-1);
   
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
      t_lee_linea(texto,82,file1);
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
   if (convertir)
   {
      convierte_a_fgvmenu(nnom);       
   }
   
   while(ntabla_icon)
   {
      ntabla_icon--;
      free(tabla_icon[ntabla_icon]);
   }
   if (tabla_icon)
   {
      free(tabla_icon);	  
      free(punto_tabla);      
      tabla_icon = NULL;
      punto_tabla = NULL;
   }
   
   strcpy(texto,fichemenu);
   strcat(texto,".icn");
   file1 = fopen(texto,"rt");
   if (file1)
   {
      while(lee_linea(texto,82,file1))
      {
         coma_linea(texto,0);	   
         quita_blancos(texto);
         r = coma_linea(texto,1);
         for (i = 0;i < nmenus;i++)
         {
            if (ntabla_icon >= 1000) break; 
            for (j = 0;j < wmenu[i]->m_nop;j++)
            {
               strcpy(tmp,wmenu[i]->m_op[j]->o_nombre);
               quita_blancos(tmp);
               if (!strcmp(tmp,texto))
               {
                  if (!tabla_icon)
                  {
                     tabla_icon = (char **)malloc(sizeof(char *) * 1000);
                     punto_tabla = (short *)malloc(sizeof(short) * 1000);
                  }
                  punto_tabla[ntabla_icon] = i * 100 + j;
                  tabla_icon[ntabla_icon] = malloc(strlen(texto+r)+1);
                  strcpy(tabla_icon[ntabla_icon],texto+r);
                  ntabla_icon++;
                  break;
               }
            }
            if (j < wmenu[i]->m_nop)
               break;
         } 
      }
      fclose(file1);
   }
   
   /*** Windows ***/
   /* añadir recurso en windows */
   
   if (wwmenus)
   {
      wwlibera();
   }
   
   /* cargar wwmenus para llamadas desde aplicacion */
   n_wmenus = nmenus;
   wwmenus = (char ***)malloc(sizeof(char *) * nmenus);
   nwwmenus = (int *)malloc(sizeof(char *) * nmenus);
   for (i = 0;i < nmenus;i++)
   {
      wwmenus[i] = (char **)malloc(sizeof(char *) * wmenu[i]->m_nop);
      nwwmenus[i] = wmenu[i]->m_nop;
      for (j = 0;j < wmenu[i]->m_nop;j++)
      {
         wwmenus[i][j] = malloc(strlen(wmenu[i]->m_op[j]->o_programa)+1);
         strcpy(wwmenus[i][j],wmenu[i]->m_op[j]->o_programa);
      }
   }
   
   oldmenu = NULL;
   if (!principal_menu)
   {
      oldmenu = GetMenu(principal_w);
      principal_menu = LoadMenu(hInst, "AGIXMENU");
   }
   /*
   if (principal_menu)
   {
   // componer principal_menu 
   tmppopup = CreatePopupMenu();
   InsertMenu(principal_menu, 0,MF_ENABLED | MF_BYPOSITION | MF_POPUP | MF_STRING, (UINT)tmppopup , nommenu);
   w_crea_submenu(0,tmppopup);
   SetMenu(principal_w, principal_menu);
   }	
   if (oldmenu) DestroyMenu(oldmenu);
   RedrawWindow(principal_w,NULL,NULL,RDW_FRAME | RDW_INVALIDATE);
   */
   /***************/
   
   return(0);
}


static free_omenu(i)
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
         
         
         while(ntabla_icon)
         {
            ntabla_icon--;
            free(tabla_icon[ntabla_icon]);
         }
         if (tabla_icon)
         {
            free(tabla_icon);
            tabla_icon = NULL;
            free(punto_tabla);
            punto_tabla = NULL;
         }
         
         for (i = 0; i < nmenus ;i++)
            free_menu(i);
         nmenus = 0;
         free((char *)wmenu);
         wmenu = NULL;
         if (libera_todo)
         {
            wwlibera();
            libera_todo = 0;
         }
         return(0);
}

extern HBITMAP icono_opcion[100];
extern DWORD icono_pos[100];

static char *nombre_anterior(m)
int m;
{
			int hijo,i;
         if (wmenu[m]->m_menupadre > -1)
         {
            hijo = m;
            m = wmenu[hijo]->m_menupadre;
            for (i = 0;i < wmenu[m]->m_nop;i++)
            {
               if (hijo == wmenu[m]->m_op[i]->o_menuhijo)
               {
                  return(wmenu[m]->m_op[i]->o_nombre);
                  break;
               }
            }
            return("Menu Anterior");
         }
         return("Salida de Menu");
}



static int pinta_icono(x,y,ic,ind,texto,d,nm)
int x,y,ic,ind;
char *texto;
int d;
int nm;
{
   HICON icono;
   HDC hdc;
   RECT r;
   HFONT oldfont;
   HBRUSH bru = CreateSolidBrush(RGB(255,255,255));
   char tex[300];
   char nombre2[250];
   int i,j;
   short k;
   
   if (d == ind) d = 1;else d = 0;
   
   hdc = GetDC(principal_w);
   
   if (icono_opcion[ind])
   {
      hOldBitmap = SelectObject(hMemoryDC,icono_opcion[ind]);
      BitBlt(stdpan->vhdc,x,y,70,70,hMemoryDC,0,0,SRCCOPY);
      BitBlt(hdc,x,y,70,70,hMemoryDC,0,0,SRCCOPY);
      SelectObject(hMemoryDC,hOldBitmap);
      hOldBitmap = NULL;
      DeleteObject(icono_opcion[ind]);
   }
   icono_opcion[ind] = CreateCompatibleBitmap(hdc,70,70);
   hOldBitmap = SelectObject(hMemoryDC,icono_opcion[ind]);
   BitBlt(hMemoryDC,0,0,70,70,stdpan->vhdc,x,y,SRCCOPY);
   SelectObject(hMemoryDC,hOldBitmap);
   hOldBitmap = NULL;
   if (!ic)
      icono = LoadIcon(hInst,"ATRAS");
   else
   {
      icono = NULL;
      k = nm * 100 + (ind - 1);
      for (j = 0;j < ntabla_icon;j++)
						{
         if (punto_tabla[j] == k)
         {
            strcpy(nombre2,fichemenu);
            *quita_path(nombre2) = 0;
            strcat(nombre2,"iconos/");
            strcat(nombre2,tabla_icon[j]);
            icono = LoadImage(NULL,nombre2,IMAGE_ICON,32,32,LR_LOADFROMFILE);
            break;
         }
						}  
      if (!icono)
						{
         if (ic == 1)
            icono = LoadIcon(hInst,"OPCION");
         else
            icono = LoadIcon(hInst,"MASOP");
						}
   }
   if (d)
   {
      r.top = y;
      r.left = x+19;
      r.bottom = y+32;
      r.right = x+51;
      FillRect(stdpan->vhdc,&r,bru);
   }
   DrawIcon(stdpan->vhdc,x+19,y,icono);
   r.top = y+32;
   r.left = x;
   r.bottom = y+70;
   r.right = x+70;
   oldfont = SelectObject(stdpan->vhdc,agi_fboton);
   if (d)
   {
      SetTextColor(stdpan->vhdc,RGB(255,255,255));
      SetBkColor(stdpan->vhdc,RGB(0,0,0));
   }
   else
   {
      SetTextColor(stdpan->vhdc,RGB(0,0,0));
      SetBkColor(stdpan->vhdc,RGB(255,255,255));
   }
   SetBkMode(stdpan->vhdc,OPAQUE);
   
   for (i = 0;texto[i];i++)
      tex[i] = convierte_caracter(texto[i]);
   tex[i] = 0;
   
   DrawText(stdpan->vhdc,tex,i,&r,DT_WORDBREAK | DT_CENTER | DT_TOP);
   SetBkMode(stdpan->vhdc,TRANSPARENT);
   SelectObject(stdpan->vhdc,oldfont);
   BitBlt(hdc,x,y,70,70,stdpan->vhdc,x,y,SRCCOPY);
   ReleaseDC(principal_w,hdc);
   DestroyIcon(icono);
   icono_pos[ind] = x;
   icono_pos[ind] <<= 16;
   icono_pos[ind] |= y;
   DeleteObject(bru);
   return(0);
}

static void limpia_iconos()
{
   int i,x,y;
   HDC hdc;
   for (i = 0;i < 100;i++)
   {
						if (icono_opcion[i])
                  {
                     x = HIWORD(icono_pos[i]);
                     y = LOWORD(icono_pos[i]);
                     hdc = GetDC(principal_w);
                     hOldBitmap = SelectObject(hMemoryDC,icono_opcion[i]);
                     BitBlt(stdpan->vhdc,x,y,70,70,hMemoryDC,0,0,SRCCOPY);
                     BitBlt(hdc,x,y,70,70,hMemoryDC,0,0,SRCCOPY);
                     SelectObject(hMemoryDC,hOldBitmap);
                     hOldBitmap = NULL;
                     DeleteObject(icono_opcion[i]);
                     icono_opcion[i]=NULL;
                  }
   }
}


static int desplega_hoja(m,d)
int m,d;
{
   int i;
   int x,y;
   if (m >= nmenus) return(-1);
   
   limpia_iconos();
   x = 0;y = 94;
   
   pinta_icono(x,y,0,0,nombre_anterior(m),d,m);
   x += 75;
   for (i = 0;i < wmenu[m]->m_nop;i++)
   {
						if (x > (scaracter.cx * 80 - 70))
                  {
                     x = 0;
                     y += 75;
                  }
                  if (wmenu[m]->m_op[i]->o_menuhijo > -1)
                  {
                     pinta_icono(x,y,2,i+1,wmenu[m]->m_op[i]->o_nombre,d,m);
                  }
                  else
                  {
                     pinta_icono(x,y,1,i+1,wmenu[m]->m_op[i]->o_nombre,d,m);
                  }
                  x += 75;
   }
}

extern int abandonarelbarco;

int w_menus(pim)
int *pim;
{
   MSG msg;
   int tecla,i,hijo;
   char *s;
   POINT pt1;
   RECT rtmp;
   HMENU pop;
   int pintar = 0;
   int elegida = 0,anterior = 0;
   int men,numero,x,y;
   int ppp;   
   int ojo_fe;
   extern char nomapli[25];
   extern char modulo[9];

   
   
   if (!wmenu) return(-1);
   
	   {
		   FILE *f;
		   char tmpc[512];
		   extern char rw_user[11];
		   sprintf(tmpc,"%scontrolmenu.txt",rw_directorio("logs/"));
		   f = fopen(tmpc,"a");
		   if (f)
		   {
			   fprintf(f,"dll9: %s %s %s %s %s [1]\r\n",fecha(),fechas+11,rw_user,nomapli,modulo);
			   fclose(f);
		   }
	   }

   
   if (DespachadorExterno)
   {
      MenuStruct *otro_menu = obtiene_menu();
      if (otro_menu)
      {
         if (!pim)
            (*DespachadorExterno)((int)DESPACHO_OMENU,(int)(-wdef),(MenuStruct *)otro_menu);
         else
            (*DespachadorExterno)((int)DESPACHO_OMENU,(int)wdef,(MenuStruct *)otro_menu);
         LiberaMenuStruct(otro_menu);
      }	 
   }

	   {
		   FILE *f;
		   char tmpc[512];
		   extern char rw_user[11];
		   sprintf(tmpc,"%scontrolmenu.txt",rw_directorio("logs/"));
		   f = fopen(tmpc,"a");
		   if (f)
		   {
			   fprintf(f,"dll9: %s %s %s %s %s [2]\r\n",fecha(),fechas+11,rw_user,nomapli,modulo);
			   fclose(f);
		   }
	   }
   
   if (!pim) return(-1);

	   {
		   FILE *f;
		   char tmpc[512];
		   extern char rw_user[11];
		   sprintf(tmpc,"%scontrolmenu.txt",rw_directorio("logs/"));
		   f = fopen(tmpc,"a");
		   if (f)
		   {
			   fprintf(f,"dll9: %s %s %s %s %s [3]\r\n",fecha(),fechas+11,rw_user,nomapli,modulo);
			   fclose(f);
		   }
	   }
   
   libera_todo = 0;
   
   en_menu = 1;
   if (DespachadorExterno)
   {
      (*DespachadorExterno)((int)DESPACHO_EMENU,(int)en_menu,(void *)NULL);
   }   

	   {
		   FILE *f;
		   char tmpc[512];
		   extern char rw_user[11];
		   sprintf(tmpc,"%scontrolmenu.txt",rw_directorio("logs/"));
		   f = fopen(tmpc,"a");
		   if (f)
		   {
			   fprintf(f,"dll9: %s %s %s %s %s [4]\r\n",fecha(),fechas+11,rw_user,nomapli,modulo);
			   fclose(f);
		   }
	   }

   if (carga_grafico(fichemenu,stdpan) < 0)
      PonLogo();
   else
      v_refresca();


	   {
		   FILE *f;
		   char tmpc[512];
		   extern char rw_user[11];
		   sprintf(tmpc,"%scontrolmenu.txt",rw_directorio("logs/"));
		   f = fopen(tmpc,"a");
		   if (f)
		   {
			   fprintf(f,"dll9: %s %s %s %s %s [5]\r\n",fecha(),fechas+11,rw_user,nomapli,modulo);
			   fclose(f);
		   }
	   }

   /** intercepcion **/
   men = wdef/100;
   if (men < 0 || men >=  nmenus)
      men = 0;
   que_menu = men;
   do 
   {
      if (snap_shoot_menu)
      {
         if (*snap_shoot_menu)
            graba_bmp_pantalla(snap_shoot_menu);
         snap_shoot_menu = NULL;
         tecla = 807;
      }
      else
         tecla = leecar();

	   {
		   FILE *f;
		   char tmpc[512];
		   extern char rw_user[11];
		   sprintf(tmpc,"%scontrolmenu.txt",rw_directorio("logs/"));
		   f = fopen(tmpc,"a");
		   if (f)
		   {
			   fprintf(f,"dll9: %s %s %s %s %s [6]\r\n",fecha(),fechas+11,rw_user,nomapli,modulo);
			   fclose(f);
		   }
	   }

      ppp = 0;
      
      switch(tecla)
      {
      case ACABAR :
      case ESC :
									tecla = 806;
                           break;
      case ABORTAR :
      case CTRLC :
									tecla = 807;
                           break;
      default:
									break;
      }

	   {
		   FILE *f;
		   char tmpc[512];
		   extern char rw_user[11];
		   sprintf(tmpc,"%scontrolmenu.txt",rw_directorio("logs/"));
		   f = fopen(tmpc,"a");
		   if (f)
		   {
			   fprintf(f,"dll9: %s %s %s %s %s [7]\r\n",fecha(),fechas+11,rw_user,nomapli,modulo);
			   fclose(f);
		   }
	   }

	  ojo_fe = 0;
	  if (tecla == 831)
	  {
		   if (DespachadorExterno)
		   {
			   char tmp[512];
			   tmp[0] = 0;
			   (*DespachadorExterno)((int)DESPACHO_MULTIPLE,14,tmp);			   
			   tecla = atoi(tmp);			   
			   if (tecla < 1)
				   tecla = 831;
			   else
			   {
				   fuerza_empresa = 1;
				   ojo_fe = 1;
			   }
		   }
	  }
	   {
		   FILE *f;
		   char tmpc[512];
		   extern char rw_user[11];
		   sprintf(tmpc,"%scontrolmenu.txt",rw_directorio("logs/"));
		   f = fopen(tmpc,"a");
		   if (f)
		   {
			   fprintf(f,"dll9: %s %s %s %s %s [8]\r\n",fecha(),fechas+11,rw_user,nomapli,modulo);
			   fclose(f);
		   }
	   }

      if (tecla < 0)
      {
          ppp = 1;
          tecla = -tecla;
      }
      if (tecla >= MID_XPRIMERA && tecla < MID_XOPCION1)
      {
         tecla -= MID_XPRIMERA;
         *pim = tecla / 100;
         tecla %= 100;
		 s = (char *)0;
         if ((hijo = wmenu[*pim]->m_op[tecla]->o_menuhijo) == -1)
         {
            s = wmenu[*pim]->m_op[tecla]->o_programa;			
         }
         tecla++;
         wdef = (*pim)*100+tecla;
         if (hijo > -1 || ppp)
			{
            continue;
			}
		 /*
		 if (ojo_fe && s)
		 {
			 if (*s
		 }
		 else
		 */
			break;
      }

	   {
		   FILE *f;
		   char tmpc[512];
		   extern char rw_user[11];
		   sprintf(tmpc,"%scontrolmenu.txt",rw_directorio("logs/"));
		   f = fopen(tmpc,"a");
		   if (f)
		   {
			   fprintf(f,"dll9: %s %s %s %s %s [9]\r\n",fecha(),fechas+11,rw_user,nomapli,modulo);
			   fclose(f);
		   }
	   }

      if (tecla == 806 || tecla == 807 || tecla == 831)
      {
         libera_todo = 1;		 
         if (tecla == 831)
         {
             tecla = -3;
             SetFocus(principal_w);
         }
         else		 
             tecla = -1;
         break;
      }
	   {
		   FILE *f;
		   char tmpc[512];
		   extern char rw_user[11];
		   sprintf(tmpc,"%scontrolmenu.txt",rw_directorio("logs/"));
		   f = fopen(tmpc,"a");
		   if (f)
		   {
			   fprintf(f,"dll9: %s %s %s %s %s [10]\r\n",fecha(),fechas+11,rw_user,nomapli,modulo);
			   fclose(f);
		   }
	   }

   } while(!abandonarelbarco);
   
	   {
		   FILE *f;
		   char tmpc[512];
		   extern char rw_user[11];
		   sprintf(tmpc,"%scontrolmenu.txt",rw_directorio("logs/"));
		   f = fopen(tmpc,"a");
		   if (f)
		   {
			   fprintf(f,"dll9: %s %s %s %s %s [11]\r\n",fecha(),fechas+11,rw_user,nomapli,modulo);
			   fclose(f);
		   }
	   }

   if (abandonarelbarco)
   {
	   tecla = -1;
   }

   en_menu = 0;
   if (DespachadorExterno)
   {
      (*DespachadorExterno)((int)DESPACHO_EMENU,(int)en_menu,(void *)NULL);
   }

	   {
		   FILE *f;
		   char tmpc[512];
		   extern char rw_user[11];
		   sprintf(tmpc,"%scontrolmenu.txt",rw_directorio("logs/"));
		   f = fopen(tmpc,"a");
		   if (f)
		   {
			   fprintf(f,"dll9: %s %s %s %s %s [12]\r\n",fecha(),fechas+11,rw_user,nomapli,modulo);
			   fclose(f);
		   }
	   }

   return (tecla);                // Returns the value from PostQuitMessage
   
   /** anterior **/
   
   
   
   for (i = 0;i < 100;i++)
   {
      icono_opcion[i] = NULL;
   }
   
   memset(stdpan->vv+320,0,1520); /* modo grafico */
   pintar = 1;
   elegida = wdef%100;
   men = wdef/100;
   numero = wmenu[men]->m_nop;
   if (elegida > numero) elegida = 1;
   do {
      
      if (pintar)
      {
									numero = wmenu[men]->m_nop;
                           que_menu = men;
                           desplega_hoja(men,elegida);
                           pintar = 0;
                           anterior = elegida;
      }
      if (snap_shoot_menu)
      {
									graba_bmp_pantalla(snap_shoot_menu);
                           snap_shoot_menu = NULL;
                           tecla = 807;
      }
      else
      {
									tecla = leecar();			 
      }
      switch (tecla) {
      case HOME :
         elegida = 1;
         tecla = 0;
         break;
      case END  :
         elegida = numero;
         tecla = 0;
         break;
      case IZQUIERDA :
      case RETROCEDER :
      case BACKSPACE :
      case '-' :
         elegida--;
         if (elegida < 0)
            elegida = numero;
         tecla = 0;
         break;
      case DERECHA :
      case ' ' :
      case '+' :
         elegida++;
         if (elegida > numero)    /* sale por abajo */
            elegida = 0;
         tecla = 0;
         break;
      case ABAJO :
         if ( (elegida + 8) <= numero )
            elegida += 8;
         tecla = 0;
         break;
      case ARRIBA:
         if ( (elegida - 8) >= 0)
            elegida -= 8;
         tecla = 0;
         break;
      case VALIDAR :
      case CRETURN :
         if (!elegida)
            tecla = 806;
         else
            tecla = MID_XPRIMERA + men * 100 + elegida - 1;
         break;
      case ACABAR :
      case ESC :
         elegida = 0;
         tecla = 806;
         break;
      case ABORTAR :
      case CTRLC :
         elegida = 0;
         tecla = 807;
         break;
      default :
         if (tecla < 255)
         {
            tecla = toupper(tecla);
            if (tecla >= '0' && tecla <= '9')
               i = tecla - '0' + 1;
            else
               i = tecla - 'A' + 1;
            if (i > 0 && i <= numero)
												{
               elegida = i;
               tecla = MID_XPRIMERA + men * 100 + elegida - 1;
												}
            else
               tecla = 0;
         }
         break;
      }
      if (anterior != elegida)
      {        /* ha cambiado de opcion */
									x = 75 * (anterior%8);
                           y = 94 + 75 * (anterior/8);
                           if (!anterior)
                              pinta_icono(x,y,0,0,nombre_anterior(men),-1,men);
                           else
                              if (wmenu[men]->m_op[anterior-1]->o_menuhijo > -1)
                                 pinta_icono(x,y,2,anterior,wmenu[men]->m_op[anterior-1]->o_nombre,-1,men);
                              else
                                 pinta_icono(x,y,1,anterior,wmenu[men]->m_op[anterior-1]->o_nombre,-1,men);
                              anterior = elegida;
                              x = 75 * (elegida%8);
                              y = 94 + 75 * (elegida/8);
                              if (!elegida)
                                 pinta_icono(x,y,0,0,nombre_anterior(men),0,men);
                              else
                                 if (wmenu[men]->m_op[elegida-1]->o_menuhijo > -1)
                                    pinta_icono(x,y,2,elegida,wmenu[men]->m_op[elegida-1]->o_nombre,elegida,men);
                                 else
                                    pinta_icono(x,y,1,elegida,wmenu[men]->m_op[elegida-1]->o_nombre,elegida,men);
      }
      
      if (tecla == MID_ACTUALIZAMENU)
      {
									libera();
                           leemenu(fichemenu);
                           continue;
      }
      
      if (tecla >= MID_XPRIMERA && tecla < MID_XOPCION1)
      {
									tecla -= MID_XPRIMERA;
                           *pim = tecla / 100;
                           tecla %= 100;
                           if ((hijo = wmenu[*pim]->m_op[tecla]->o_menuhijo) == -1)
                           {
                              s = wmenu[*pim]->m_op[tecla]->o_programa;
                           }
                           tecla++;
                           wdef = (*pim)*100+tecla;
                           if (hijo > -1)
                           {
                              men = hijo;
                              elegida = 1;
                              pintar = 1;
                              continue;
                           }
                           if (*s >= '0' && *s <= '9' && *(s+1) == ' ')
                              libera_todo = 1;
                           break;
      }
      
      if (tecla == 806)
      {
									if (wmenu[men]->m_menupadre > -1)
                           {
                              hijo = men;
                              men = wmenu[hijo]->m_menupadre;
                              elegida = 0;
                              for (i = 0;i < wmenu[men]->m_nop;i++)
                              {
                                 if (hijo == wmenu[men]->m_op[i]->o_menuhijo)
                                 {
                                    elegida = i+1;
                                    break;
                                 }
                              }
                              pintar = 1;
                              continue;
                           }
                           tecla = 807;
      }
      if (tecla == 3 || tecla == 807)
      {
									tecla = -1;
                           libera_todo = 1;
                           break;
      }
     } while(-1);
     
     en_menu = 0;
     limpia_iconos();
     
     return (tecla);                // Returns the value from PostQuitMessage
}



/***************************************************************************************************/


#endif
