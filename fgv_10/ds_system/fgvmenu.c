

#include <fgvsys.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <malloc.h>

#include <rwmalloc.h>
#include <rtermin.h>
#include <rdef.h>
#include <teclas.h> 
#include <rfecha.h>

#include <rwmenus.h>


#ifdef RWXARXA

extern int en_conexion_terminal;
/*
extern int server_ver1;
extern int server_ver2;
extern unsigned char server_verletra ;
*/

extern FILE *t_rwfopen();
extern char *t_lee_linea();
extern int   t_rwfclose();
#else
extern char *lee_linea();
#define t_rwfopen   fopen
#define t_lee_linea lee_linea
#define t_rwfclose  fclose
#endif

/**********
ATENCION: A PONER!!!!
Nueva especificacion de comando especial (acceso a aplicacion) en menus:

  ,:directorio aplicacion,empresa,directorio ficheros,nombre menu
  
    -Si no se da el directorio de los ficheros se asume 'fich'.
    -Si no se da el nombre del menu se asume el directorio aplicacion.
    
      Ejemplo> AGIPROF /Acceso     ,:facges[,facemp1][,fich][,facges]
      AGIPROF /Definir    ,:facges[,facemp][,fich][,facges]
**********/



static int **posiciones;


int es_menu_acceso_directo = 0;
extern int wdesplegamenu;

static char *menu_lee_linea(char *buf,int max,FILE *file)
{
	char *ret =	t_lee_linea(buf,max,file);
	if (ret)
	{
		int i;
		if (buf[0] == '\\')
		{
			for (i = 1;buf[i] && buf[i] != '\\';i++);
			if (buf[i] == '\\')
			{
				strcpy(buf,buf+i+1);
			}
		}
	}
	return ret;
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

static int pon_opcion(titulo,sm,posicion,auto_inc)
char *titulo;
int sm;
int posicion;
char *auto_inc;
{
char mi_opcion[512];
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
		 if (titulo[i] == '[')
		   {
		   strcpy(mi_opcion,titulo+i);
		   }
		 else
		   {
		   strcpy(mi_opcion,auto_inc);
		   strcat(mi_opcion,titulo+i);
		   }
		 
         wmenu[sm]->m_op[posicion]->o_programa = malloc(strlen(mi_opcion)+2);
         strcpy(wmenu[sm]->m_op[posicion]->o_programa,mi_opcion);		 
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


static char sustituto[512] = "";
static int pendiente = 0;
static int punsus = 0;
static char *siguiente_linea(char *texto,int max,FILE *file1,char **sustitucion)
{
	char *r;
	int p1,p2;
	if (pendiente)
	{
		r = sustituto;
		strcpy(texto,r);
		r = texto;
		pendiente = 0;
	}
	else
	{
		r = menu_lee_linea(texto,max,file1);
		if (r)
			quitabp(texto);
	}
	if (!r)
	{
		if (sustitucion && sustitucion[punsus])
		{
			r = sustitucion[punsus];
			punsus++;
			strcpy(texto,r);
			r = texto;
		}
		return r;
	}
	if (texto[0] != ';' && sustitucion && sustitucion[punsus])
	{
		p1 = strbusca(texto,":");
		if (p1 >= 0)
		{
			p2 = strbusca(sustitucion[punsus],":");
			if (p2 < 0)
				punsus++;
			else
			{
				if (p1 == p2 && !memcmp(texto,sustitucion[punsus],p1))
				{
					strcpy(texto,sustitucion[punsus]);
					punsus++;
				}
				else
				{
					if (p1 < p2)
					{
						p2 = p1;
					}
					if (memcmp(texto,sustitucion[punsus],p2) > 0)
					{
						pendiente = 1;
						strcpy(sustituto,texto);
						r = sustitucion[punsus];
						punsus++;
						strcpy(texto,r);
						r = texto;
					}
				}
			}
		}
	}
	return r;
}

int carga_fgvmenu(file1,texto,nomfile)
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
   char auto_inc[256];
   char **sustitucion = NULL;

   pendiente = 0;
   punsus = 0;


   i = 0;
/*
#ifdef RWXARXA
   if (en_conexion_terminal)
   {
	   i = server_ver1 * 100000 + server_ver2 * 1000 + server_verletra;
	   if (i >= 1009085)
	   {
		   i = 0;
	   }
   }
#endif
*/

   if (!i && nomfile)
   {
	   FILE *f;
	   int nn = 0;
	   strcpy(auto_inc,nomfile);
	   strcat(auto_inc,".mnm");
	   f = t_rwfopen(auto_inc,"r");
	   if (f)
	   {
		   sustitucion = (char **)malloc(sizeof(char *) * 100);
		   while(menu_lee_linea(auto_inc,200,f))
		   {
			   nn++;
			   if (nn > 100 && (nn%100) == 1)
			   {
				   sustitucion = (char **)realloc((char *)sustitucion,sizeof(char *) * (nn/100)*100+100);
			   }
			   quitabp(auto_inc);
			   sustitucion[nn-1] = malloc(strlen(auto_inc)+1);
			   strcpy(sustitucion[nn-1],auto_inc);
		   }
		   sustitucion[nn] = NULL;
		   t_rwfclose(f);
	   }
   }
   
   auto_inc[0] = 0;

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
	  if (texto[0] == '[')
	    {
	    strcpy(auto_inc,texto);
		continue;
		}
	  
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
                           pon_opcion(titulo,nmenus-1,0,auto_inc);
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
                              pon_opcion(titulo,nmenus-1,0,auto_inc);
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
                              pon_opcion(titulo,sm,v_posicion(sm,posicion[p]),auto_inc);
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
     } while(siguiente_linea(texto,200,file1,sustitucion));
     
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




static int procesa_nivel(FILE *f,int nivel,int *opcion,int m)
{
   int i,j;
   
   opcion[nivel] = 1;
   /*fprintf(f,"!%d,%d\n",wmenu[m]->m_fqp,wmenu[m]->m_cqp);*/
   for (i = 0;i < wmenu[m]->m_nop;i++)
   {
      for(j = 0;j <= nivel;j++)
      {
         if (j)
            fprintf(f,",%d",opcion[j]);
         else
            fprintf(f,"%d",opcion[j]);
      }
      fprintf(f,":%-30s",wmenu[m]->m_op[i]->o_nombre);
      if (wmenu[m]->m_op[i]->o_menuhijo > -1)
      {
         fprintf(f,"\n");
         procesa_nivel(f,nivel+1,opcion,wmenu[m]->m_op[i]->o_menuhijo);
      }
      else
      {
         fprintf(f,"{{%s}}\n",wmenu[m]->m_op[i]->o_programa);
      }
      opcion[nivel]++;
   }
   return(0);
}

int graba_fgvmenu(file)
char *file;
{
   char nombre[256];
   FILE *f;
   int opcion[50];
   int nivel = 0;
   
   strcpy(nombre,file);strcat(nombre,".men");
   
   f = fopen(nombre,"w");
   if (!f) return(-1);
   fprintf(f,"#:%s\n",nommenu);
   nivel = 0;
   procesa_nivel(f,nivel,opcion,0);
   fclose(f);
   
   return(0);
}



