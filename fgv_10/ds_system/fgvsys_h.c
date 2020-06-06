

/**********************************************************************/
/**********************************************************************/
/*                          DIAGRAM9                                  */
/**********************************************************************/
/**********************************************************************/


/********************************************************************
CON MODULO TRACER
********************************************************************/

#include <fgvsys.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef O_BINARY
#include <io.h>
#endif


#ifndef O_BINARY  /* compatibilidad xenix - dos */

#define O_BINARY 0
#define SEEK_SET 0
#define SEEK_CUR 1
#ifndef LARGESEEK
extern long rw_lseek_();
#endif
extern unsigned int rw_read_();
#endif


#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>


#include <rwmalloc.h>
#include <rtermin.h>
#define _NUEVODEF
#include <rentra.h>
#include <rdef.h>       /* ya incluye el isam.h */
#include <rfecha.h>
#include <rmante.h>
#include <rcalcul.h>
#include <teclas.h>

#ifdef FGVWIN
#include <fgvwin.h>
#endif


static char *letrasm = "MB";
static char *textom[] = 
{
   " Modificacion ",
   " Baja ",
};

extern char *suma_linea();
extern int apli_aborto; /* flag para abortar la aplicacion (QUIT) */

extern long trata_memo();
extern char *cambia_path_fich();

extern long da_registo_actual();

extern long que_registro();

extern void set_registro();

extern double ld_doble();

extern short _si_escrola;

extern int es_modo_test;

extern int std_size;

extern int rentra_atrib;
extern int rentra_sal_atrib;
extern int rentra_cc;
extern int rentra_cf;
extern int rentra_sal_cc;
extern int rentra_sal_cf;

extern int especial_tecla_linea;


extern int  que_color_inicio;
extern char *color_factor_inicio;
extern int  cambio_color_fijo;
extern int  campo_color;
extern int  file_color;
extern int  que_color;
extern int  atributo_1;
extern int  atributo_2;
extern char *color_factor;
extern int  len_color;
extern int p_color;
extern unsigned char l_separador;
extern int t_ini;
extern int t_fin;


/* de MOMENTO para pruebas */
extern int default_id;
extern int default_sid;
extern int default_wid;


static int IncrementoLineas = 100;
static int SumaIncremento   = 0;

extern int n_np;

extern int espuro10;

int _kaka()
{
}

int se_suma_l(ipfi,f)
int ipfi,f;
{
   int c,k,i,n;
   if (ipfi >= 0) return(1);
   c = campo_linea(f) + 1;
   k = da_cla_lin(f);
   n = v_mpfi(f)->f_clave[k].k_ncampos - 1;
   if (v_mpfi(f)->f_clave[k].k_campo[n] == c)
   {
      return(1);
   }
   return(0);
}


int borra_lineas(f)
int f;
{
   int fac = 100+IncrementoLineas;

   pinta1_linea(f,100,3,0);
   while (pinta1_linea(f,fac,1,0) == 0)
      fac += IncrementoLineas;
}

static int pon_el_color(c_t,flin)
int c_t;
int flin;
{
   if (!cambio_color_fijo && color_factor && campo_color > 0)
   {		 
      memcpy(color_factor,v_mpfi(flin)->f_buf+p_color,len_color);
      que_color = c_t;		    
   }
   return(0);
}

int dame_limite_lineas(int flin)
{
	if (-((v_mpfirel(flin)->lineas%160)%6) == 4)
	{
		return -((v_mpfirel(flin)->lineas/160)%100);
	}
    return -((v_mpfirel(flin)->lineas%160)/6);	
}

int dame_cuadro_lineas(int flin,int *pi,int *pf)
{
   int limitel = dame_limite_lineas(flin);
   int campini,campfin;
   int c;
   int p;
   int visual = 0,lineal;
   int i,j;
   int numcam;
   int limite;
   int ini,minlin,maxlin,colini;
   
   limite = 0;
   ini = 2480;
   minlin = 9999;
   maxlin = 0;
   colini = 9999;
   
   campini = v_mpfi(flin)->f_pantalla[0].p_campoini;
   campfin = v_mpfi(flin)->f_pantalla[0].p_campofin;
   for (c=campini; c<=campfin;c++) 
   {
      numcam = v_mpfi(flin)->f_taborder[c-1]+1;
      lineal = (v_mpfi(flin)->f_campos[numcam-1].c_mante.c_lineal& C_LINEAL);
      if (lineal && v_mpfi(flin)->f_campos[numcam-1].c_mante.c_visual == 0) 
      {
         p = v_mpfi(flin)->f_campos[numcam-1].c_mante.c_qp;
         i = v_mpfi(flin)->f_campos[numcam-1].c_mante.c_qpl;
         j = long_campo(v_mpfi(flin),(numcam - 1));
         if ((v_mpfi(flin)->f_campos[numcam-1].c_tipo == FECHATIPO &&  i != 6 && i != 8) ||
            (v_mpfi(flin)->f_campos[numcam-1].c_tipo != CHARTIPO  && v_mpfi(flin)->f_campos[numcam-1].c_tipo != FECHATIPO) ||
            i < 1 || i > j)
            i = j;
         if (ini > p)
            ini = p;
         if (minlin > (p/100))
             minlin = p/100;
         if (maxlin < (p/100))
             maxlin = p/100;
         if (colini > (p%100))
             colini = p%100;
         p += (i-1);
         if (limite < p)
            limite = p;
      }
   }
   
   if (v_mpfi(flin)->f_fi_modo_lineas)
   {
       *pi = minlin*100+colini;
       *pf = maxlin*100+colini+ limitel*v_mpfi(flin)->f_fi_modo_lineas - 1;
       return(1);
   }

   for (c=campini; c<=campfin;c++) 
   {
      numcam = v_mpfi(flin)->f_taborder[c-1]+1;
      lineal = (v_mpfi(flin)->f_campos[numcam-1].c_mante.c_lineal& C_LINEAL);
      if (!lineal && v_mpfi(flin)->f_campos[numcam-1].c_mante.c_visual == 0) 
      {
         p = v_mpfi(flin)->f_campos[numcam-1].c_mante.c_qp;
         i = v_mpfi(flin)->f_campos[numcam-1].c_mante.c_qpl;
         j = long_campo(v_mpfi(flin),(numcam - 1));
         if ((v_mpfi(flin)->f_campos[numcam-1].c_tipo == FECHATIPO &&  i != 6 && i != 8) ||
            (v_mpfi(flin)->f_campos[numcam-1].c_tipo != CHARTIPO  && v_mpfi(flin)->f_campos[numcam-1].c_tipo != FECHATIPO) ||
            i < 1 || i > j)
            i = j;
         if ( (p / 100) >= (ini / 100) && (p / 100) <= limitel)
         {
            p += (i-1);
            if (limite < p)
               limite = p;
         }
      }
   }   
   
   if (limite)
   {
      *pi = ini;
      *pf = (limitel*100)+(limite%100);
      return(1);
   }
   else
      return(0);
}

int pinta1_linea(flin,faclin,modo,r)
int flin,faclin,modo,r;
{
   register int numcam,i,p = 0,c,att = 0,j;
   int campini,campfin;
   int visual = 0,lineal,haylineal = 0;
   int limitel = dame_limite_lineas(flin);
   int nopinta = 0;
   int salvaa;
   char *tmp = malloc(MAXDEFECTO);
   int _rentra_sal_atrib = rentra_sal_atrib;
   int es_prime = 1;
   int _rentra_sal_cc = rentra_sal_cc;
   int _rentra_sal_cf = rentra_sal_cf;
   
   if (modo > 3) {modo -= 4;att = 1;}
   
   if (!modo)
   {
      if (!cambio_color_fijo && color_factor && campo_color > 0 && file_color == flin && faclin == 100 && color_factor_inicio)
      {
         que_color = que_color_inicio;		 
         if (!que_color)
            rentra_sal_atrib = atributo_1;
         else
            rentra_sal_atrib = atributo_2;         
         memcpy(color_factor_inicio,v_mpfi(flin)->f_buf+p_color,len_color);
         memcpy(color_factor,v_mpfi(flin)->f_buf+p_color,len_color);
         rentra_sal_cc = 0;
         rentra_sal_cf = 0;		 
      }
      else
      if (color_factor && campo_color > 0 && file_color == flin)
      {		 
            if (memcmp(color_factor,v_mpfi(flin)->f_buf+p_color,len_color))
            {
               if (!cambio_color_fijo)
               {
                    memcpy(color_factor,v_mpfi(flin)->f_buf+p_color,len_color);               
                    if (que_color)
                    {			   
                      que_color = 0;
                    }
                    else
                    {			   
                      que_color = 1;
                    }
               }
               else
               {
                   que_color = 1;
               }
            }
            else
            {
                if (cambio_color_fijo)
                {
                    que_color = 0;
                }
            }

            if (!que_color)
               rentra_sal_atrib = atributo_1;
            else
               rentra_sal_atrib = atributo_2;
            rentra_sal_cc = 0;
            rentra_sal_cf = 0;		 
      }
   }
   
   if (modo == 2) 
   {
      if (ejecuta_otro(flin,13,O_NOR,faclin,0) == -1) 
      {
         nopinta = 1;
      }
   }

   campini = v_mpfi(flin)->f_pantalla[0].p_campoini;
   campfin = v_mpfi(flin)->f_pantalla[0].p_campofin;
   
   
   for (c=campini; c<=campfin;c++) 
   {
      numcam = v_mpfi(flin)->f_taborder[c-1]+1;

      if (v_mpfi(flin)->f_campopos[ numcam-1 ] != -1)
      {
        v_mpfi(flin)->f_campos[ numcam-1 ].c_mante.c_qp = v_mpfi(flin)->f_campopos[ numcam-1];
      }


      if (v_mpfi(flin)->f_campos[numcam-1].c_qc == -2) {
         lineal = 0;
         if (!nopinta && !r && v_mpfi(flin)->f_campos[numcam-1].c_mante.c_visual == 0 && modo == 3) 
         {
            if (!(v_mpfi(flin)->f_campos[numcam-1].c_mante.c_lineal& C_LINEAL)) 
            {
               p=atoi(v_mpfi(flin)->f_campos[numcam-1].c_minimo);
               salvaa = v_mpfi(flin)->f_campos[numcam-1].c_fflag;
               if (actualiza_cam(flin,numcam,faclin,14) != -1) 
               { /* para velocidad control */
                  trata_memo("",
                     0L,
                     3,p,
                     p + atoi(v_mpfi(flin)->f_campos[numcam-1].c_maximo) - 101,
                     (v_mpfi(flin)->f_campos[numcam-1].c_mante.c_lineal& C_LINEAL),
                     v_mpfi(flin)->f_campos[numcam-1].c_mante.c_qp,
                     "",v_mpfi(flin)->f_campos[numcam-1].c_mante.c_qpl,NULL,v_mpfi(flin)->f_campos[numcam-1].c_fflag);
               }
               v_mpfi(flin)->f_campos[numcam-1].c_fflag = salvaa;
            }
         }
         continue;
      }
      else 
      {
         lineal = (v_mpfi(flin)->f_campos[numcam-1].c_mante.c_lineal& C_LINEAL);
         p = v_mpfi(flin)->f_campos[numcam-1].c_mante.c_qp;
						}
      if (v_mpfi(flin)->f_campos[numcam-1].c_mante.c_visual == 0)
         if (!nopinta && (lineal != 0 || modo > 1)) 
         {
            if (lineal != 0) 
            {
               haylineal = 1;
               if (v_mpfi(flin)->f_fi_modo_lineas)
               {
                   p = p + ((faclin - 100)/100)*v_mpfi(flin)->f_fi_modo_lineas;
                   visual = faclin;
                   if ((faclin/100) > limitel)  {modo = 0;break;}
               }
               else
               {
                   p = p + faclin - 100;
                   visual = p;
                   if ((p/100) > limitel)  {modo = 0;break;}
               }               
               
            }
            if (r != 0) continue;
            if (modo == 0 || modo == 2)
            {
               salvaa = v_mpfi(flin)->f_campos[numcam-1].c_fflag;          
               if (actualiza_cam(flin,numcam,faclin,14) == -1) /* para velocidad control */         
               {
                  v_mpfi(flin)->f_campos[numcam-1].c_fflag = salvaa;                
                  continue;
               }
            }
            if (!es_prime && lineal && l_separador)
            {
               cursor(p-1);
               tmp[0] = l_separador;
               tmp[1] = 0;
               /*v_print(tmp);*/
            }
            es_prime = 0;
            
            cursor(p);
            if (Patrib != ' ')
               atributo('0');
            if (modo == 2 && lineal != 0)
            {
               if (rentra_cc != rentra_cf)
                  Color(rentra_cc,rentra_cf);
               atributo(rentra_atrib);             
               a_atri_campo(v_mpfi(flin),numcam-1,rentra_atrib,0,rentra_cc,rentra_cf);
            }
            else
            {
               if (rentra_sal_cc != rentra_sal_cf)
                  Color(rentra_sal_cc,rentra_sal_cf);
               atributo(rentra_sal_atrib);
               a_atri_campo(v_mpfi(flin),numcam-1,rentra_sal_atrib,0,rentra_sal_cc,rentra_sal_cf);             
            }


            /*
            if (!modo)
            {
                
               if (DespachadorExterno)
               {       
                   char tmp[512]);
                   int *p1 = (int *)tmp;
                   p1[0] = sizeof(int) * 5;
                   p1[1] = default_id;
                   p1[2] = default_sid;
                   p1[3] = -1;
                   p1[5] = 2;
                   p1[4] = ((numcam-1) & 0xffff) | (Pcursor << 16);
                   (*DespachadorExterno)((int)DESPACHO_DESTRUYECONTROL,-1,tmp);
               }
               
            }
            else
            */
            {
               int vacio = 0;
               if (modo == 1 || modo == 3)
                   vacio = 1;
               if (modo == 2)
               {
                   entra_cedit(v_mpfi(flin),numcam-1,-1,0,flin,0,NULL,(lineal ? Pcursor : 0),-1,-1,0);
               }
               else
               {
                   entra_cedit(v_mpfi(flin),numcam-1,-1,0,flin,0,NULL,(lineal ? Pcursor : 0),-1,-1,vacio);
               }
            }

            if (modo == 0 || modo == 2)
            {               
               v_mpfi(flin)->f_campos[numcam-1].c_fflag = salvaa;      
            }
            /*
            else 
            {
               i = v_mpfi(flin)->f_campos[numcam-1].c_mante.c_qpl;
               j = long_campo(v_mpfi(flin),(numcam - 1));
               if ((v_mpfi(flin)->f_campos[numcam-1].c_tipo == FECHATIPO &&  i != 6 && i != 8) ||
                  (v_mpfi(flin)->f_campos[numcam-1].c_tipo != CHARTIPO  && v_mpfi(flin)->f_campos[numcam-1].c_tipo != FECHATIPO) ||
                  i < 1 || i > j)
                  i = j;			
               memset(tmp,' ',i);
               tmp[i]='\0';
               Patrib |= 128;
               v_print(tmp);
               Patrib &= (~128);
            }
            */

            if (modo == 2 && lineal != 0)
            {
               a_atri_campo(v_mpfi(flin),numcam-1,rentra_atrib,1,rentra_cc,rentra_cf);
               atributo(tolower(rentra_atrib));
            }
            else
            {
               a_atri_campo(v_mpfi(flin),numcam-1,rentra_sal_atrib,1,rentra_sal_cc,rentra_sal_cf);
               atributo(tolower(rentra_sal_atrib));
            }
         }
   }
   
   rentra_sal_atrib = _rentra_sal_atrib;
   rentra_sal_cc = _rentra_sal_cc;
   rentra_sal_cf = _rentra_sal_cf;
   
   fflush(stdout);
   free(tmp);
   if (modo < 2) 
   {
      if (!haylineal)
          visual = 2400;
      if (((visual)/100) >= limitel) 
      {
          if (att && ((visual)/100) > limitel)
             return(-2);
          return(-1);
      }
   }
   
   return(0);
}


static int pinta_lineasfin(flin,faclin,lim2,ultima,primera,linea,sl,lim1,inicio,color_trobato)
int flin;
int *faclin;
char *lim2;
char **ultima;
char **primera;
char **linea;
int sl;
char *lim1;
char *inicio;
int *color_trobato;
{
   int r,lleno;
   int fac = *faclin;
   int fl = 0;
   int tot;
   int factrobato = -1;
   char *lintrobato = NULL;
   long regitrobato = 0L;
   int ho_trobato = 0;
   

   *color_trobato = -1;


   if (*ultima)
   {
      free((char *)*ultima);
      *ultima = NULL;
   }
   
   if ((r = primeralinea(flin,lim2,0,lim1,inicio)) !=0 ) 
   {
      borra_lineas(flin);
      return(-1);
   }
   
   clave_lin_trata(flin,(char *)primera,4,"");
   
   pon_registro(flin,*primera,0);
   clave_lin_trata(flin,(char *)ultima,4,"");
   pon_registro(flin,*ultima,0);
   if (linea != NULL)  {
      if (que_registro(flin,*linea) == que_registro(flin,*ultima)) /*  !clave_lin_trata(flin,*linea,3,*ultima) /* comprobar registro */
      {
         fl = *faclin;
         ho_trobato = 1;
      }
      else
         if (clave_lin_trata(flin,*linea,3,*ultima) > 0)
         {
            fl = -1;
         }
         else
         {
            factrobato = *faclin;
            copia_linea(flin,&lintrobato,*ultima);
            regitrobato = que_registro(flin,*ultima);
            ho_trobato = 1;
         }
   }
   
   if (pinta1_linea(flin,*faclin,4,sl) == -2) /* fuera de limite de pantalla*/
   {   
      if (lintrobato)
         free(lintrobato);   
      return(-1);
   }
   tot = 1;
   if (ho_trobato)
      *color_trobato = que_color;
   while (-1)
   {
      r = siguientelinea(flin,lim2,0,0,lim1);
      if (r != 0) break;
      ho_trobato = 0;
      *faclin +=IncrementoLineas;
      clave_lin_trata(flin,(char *)ultima,4,"");
      pon_registro(flin,*ultima,0);
      if (fl <= 0 && linea != NULL) {
         if (que_registro(flin,*linea) == que_registro(flin,*ultima))      /* !clave_lin_trata(flin,*linea,3,*ultima)) */
         {
            fl = *faclin;
            ho_trobato = 1;
         }
         else
            if (factrobato <= 0)
            {
               if (clave_lin_trata(flin,*linea,3,*ultima) <= 0)
               {
                  factrobato = *faclin;
                  copia_linea(flin,&lintrobato,*ultima);
                  regitrobato = que_registro(flin,*ultima);
                  ho_trobato = 1;
               }
            }
      }
      if (pinta1_linea(flin,*faclin,0,sl) != 0) break;
      if (ho_trobato)
         *color_trobato = que_color;
      tot++;
   }
   *faclin += IncrementoLineas;
   if (*faclin != fac) 
   {    /* si hay lineas intento borrar el resto de pagina */
      fac = *faclin;

      /*while (pinta1_linea(flin,fac,1,0) == 0)*/
      while (pinta1_linea(flin,fac,5,sl) != -2)
         fac += IncrementoLineas;
   }

   if (fac == *faclin && *ultima != NULL)
      lleno = -1;
   else
      lleno = tot;

   *faclin -= IncrementoLineas; /* pone la pos en pan a la ultima linea */
   if (linea != NULL) 
   {
      if (fl > 0)
      {
         *faclin = fl;	 
      }
      else 
      {
         if (factrobato > 0)
         {
            *faclin = factrobato;
            copia_linea(flin,linea,lintrobato);
            set_registro(flin,*linea,regitrobato);
         }
         else
         {
            copia_linea(flin,linea,*ultima);
            *color_trobato = que_color;
         }
      }
   }
   if (lintrobato)
      free(lintrobato);
   return(lleno);
}


static int entra_linea(f,linea,faclin,ultima,alta,final,lleno,lim1,lim2,baja,saleret,noprim,ipfi,desorden,primera,ant_qj,sinbotones)
int f;
char **linea;
int *faclin;
char **ultima;
int alta,*final,*lleno;
char *lim1;
char *lim2;
char **baja;
int saleret;
int noprim;
int ipfi;
int *desorden;
char **primera;
int *ant_qj;
int sinbotones;
{
   int qj = 0,c,h,leida = 0,salida = 0,modoi,altai,nn,ll,c_t;
   int cam_lin = campo_linea(f) + 1;
   int k = da_cla_lin(f);
   /* int erafinal; */
   char *tmp = NULL;
   /* int antes; */
   int nopintar = 0,ya_puesto = 0;
   int old_modo_validacion = modo_de_validacion;
   VENT *w = NULL;
   long salva_alta;
   int posm = v_mpfi(f)->f_posmenu;

   if (!posm)
	   posm = 24;
   
principio:
   
   modo_de_validacion = CRETURN;
   
   
   if (!memcmp(v_memo(5002)->alfabetica+34,"INS",3))
   {
      memcpy(v_memo(5002)->alfabetica+34,"   ",3);
      if (!*final && saleret != 2)
      {
         if (_si_escrola)
         {
            scrola_lineas(f,2,*faclin - 100);
         }
         else
         {/* machaca sobre las anteriores */
         }
         *desorden = 2;
         *final = 1;
         /* situar el corriente en la anterior !! */
         anteriorlinea(f,lim2,0,0,lim1);
         
         defecto_ficha(v_mpfi(f));
         if (ipfi >= 0)
         {
            deferel_rel(f,ipfi,-1,0,ALTA,-1);			
         }
         clave_lin_trata(f,*linea,5,"");
         if (se_suma_l(ipfi,f))     
         {
            *linea = suma_linea(f,*linea,*linea);
         }
      }
   }
   
   if (*final == 0) modoi = 2; else modoi = 3;
   
   while(-1) 
   {      
      if (!memcmp(v_memo(5002)->alfabetica+34,"MAS",3))
      {
         memcpy(v_memo(5002)->alfabetica+34,"   ",3);
         if (*final)
         {
            if (!leer_reg(f,ISCURR,0,1))		 
            {
               clave_lin_trata(f,(char *)&tmp,4,"");
               primeralinea(f,lim2,0,lim1,tmp);
               if (siguientelinea(f,lim2,0,0,lim1))
               {
                  primeralinea(f,lim2,0,lim1,tmp);
                  /* falsa alarma!!!*/
                  defecto_ficha(v_mpfi(f));
                  if (ipfi >= 0)
                  {
                     deferel_rel(f,ipfi,-1,0,ALTA,-1);			
                  }
                  clave_lin_trata(f,*linea,5,"");
               }
               else
               {
                  while(!siguientelinea(f,lim2,0,0,lim1))
                  {
                     *faclin += IncrementoLineas; /* de momento asi pensado para pocas lineas */
				                 /* de todos modos convendria poder fijar un limite */
                  }
                  clave_lin_trata(f,(char *)linea,4,"");
                  nn = total_lineas(f);
                  ll = (*faclin+SumaIncremento) / IncrementoLineas;
                  if (ll >= nn)
                     *faclin = ((nn-1) * IncrementoLineas) - SumaIncremento;
                  ll = (*faclin+SumaIncremento) / IncrementoLineas;
                  /* establecer la nueva primera y repintar la pantalla */
                  while(ll > 1 && !anteriorlinea(f,lim2,0,0,lim1))
                  {
                     ll --;
                  }
                  clave_lin_trata(f,&tmp,4,"");
                  *faclin = 100;
                  *lleno = pinta_lineasfin(f,faclin,lim2,ultima,primera,linea,0,lim1,tmp,&c_t);			  
                  *faclin += IncrementoLineas;
                  *final = 1;			  
                  defecto_ficha(v_mpfi(f));
                  if (ipfi >= 0)
                  {
                     deferel_rel(f,ipfi,-1,0,ALTA,-1);			
                  }
                  clave_lin_trata(f,*linea,5,"");
                  if (se_suma_l(ipfi,f))     
                  {
                     *linea = suma_linea(f,*linea,*linea);
                  }
               }
            }
            
         }
      }
      
      
      if ( pinta1_linea(f,*faclin,modoi,0) != 0 ) 
      {
         qj = 5; /* pagina abajo */
         goto e_l_salida;
      }
      
      altai = alta;
      if (*final == 0 && alta == ALTA) altai = MODIF;
      if (*final != 0) altai = ALTA;
      do {  
         clave_lin_trata(f,*linea,5,"");
         if (k > 0 && *final && noprim)
         {
            h = 0;	 	 
         }
         else
            h = -1;
            /*
            antes = 0;  
            if (!h && noprim && qj != 0 && qj != 3 && qj != 5) 
            {
            if (memoplin > 4) sprintf(v_memo(5004)->alfabetica,"%d",qj);     
            antes = qj;
            }
         */
         
         if (h == -1 && *ant_qj >= 0)
         {
            h = -(*ant_qj + 2);	 
            ya_puesto = 1;
         }
         *ant_qj = -1;

         salva_alta = da_registo_actual(v_mpfi(f));

         qj = pregun_clave(&h,f,*faclin,altai,sinbotones);

		 
		 if (salva_alta > 0) {
			 pon_registro_actual(v_mpfi(f),salva_alta);
			 apunta_ind(v_mpfi(f),0,ISEQUAL,0);
			 apunta_ind(v_mpfi(f),k+1,ISCURR,0);
		 }
		 
         
         if (qj == SOLICITA_POSICION)
         {
            if (!TestigoRaton)
               TestigoRaton = 1;
            else
            {
               pito(28);
               TestigoRaton = 0;
               continue;
            }
            qj = 0;
            if (alta == CONSULTA)
            {
               TestigoRaton = 0;
               pito(29);
               continue;
            }
         }
         if (qj == SOLICITA_INICIO || qj == SOLICITA_FINAL)
         {
            goto e_l_salida;
         }
         /* 
         SOLICITA_SUPRESION;
         SOLICITA_INSERCION;
         */

         /*if (qj != SOLICITA_SCROLLDOWN && qj != SOLICITA_SCROLLUP && qj < 0) */
         if (qj == -1)
             qj = 7;
         /*
         if (antes && qj == 0)
         qj = antes;
         */
         
         clave_lin_trata(f,(char *)&tmp,4,"");
         if (saleret == 1 && (!qj || qj == 1 || qj == 7)) {
            if (!qj && *final) qj = 7;
            if (memoplin > 4) 
            {                
                sprintf(v_memo(5004)->alfabetica,"%d",((!qj) ? 0 : -1));
            }
            if (!qj)
               qj = 7;
         }
      } while (qj != 1 && qj != 7 && (clave_lin_trata(f,tmp,7,lim2) || clave_lin_trata(f,tmp,8,lim1) ||
          (*final == 1 && (( (qj == 3 || qj == SOLICITA_SCROLLDOWN) && *desorden != 2) || (qj == 5 && *desorden != 2)))));
      
      if (qj == 7) qj = -1;      
      
      if (memoplin > 3 && memcmp(v_memo(5002)->alfabetica+22,"si",2) == 0)
      {
         memset(v_memo(5002)->alfabetica+22,'\0',2);
         *final = 0;   
         qj = 777;
         goto e_l_salida;
      }
      
      if (memoplin > 3 && memcmp(v_memo(5002)->alfabetica+22,"SI",2) == 0)
      {
         memset(v_memo(5002)->alfabetica+22,'\0',2);
         *final = 0;   
         qj = 778;
         goto e_l_salida;
      }
      
      
      
      if (alta == CONSULTA || qj != 0) 
      {
         if (*final)
         {
            pinta1_linea(f,*faclin,3,0);
            nopintar = 1;
            if (qj != -1 && qj != 1 && *desorden)
            {  /* reposicionar corriente (ultimo entrado) */
               /* si no hay no pasa nada (!)... */
               if (!leer_reg(f,ISCURR,0,1) && *desorden == 2 && qj == 3)		 
               {
                  clave_lin_trata(f,(char *)&tmp,4,"");
                  if (siguientelinea(f,lim2,0,0,lim1))
                  {
                     primeralinea(f,lim2,0,lim1,tmp);
                     if (*faclin > 199)
                        *faclin -= IncrementoLineas;
                  }
               }
               if (qj == 2) /* debe situarse en la corriente (la anterior) */
               {			
                  if (*faclin > 199)
                     *faclin -= IncrementoLineas;
               }
               else		 
                  if (qj == 4)
                     *ant_qj = qj;
                  else
                     if (*desorden == 2 && qj == 5)
                        *ant_qj = qj;
                     qj = 778;
                     *desorden = 0;		
            }	  
         }
         else
         {
            clave_lin_trata(f,(char *)&tmp,4,""); 
            if (qj != -1 && qj != 1 && clave_lin_trata(f,*linea,3,tmp) != 0)
            {
               if (ya_puesto)
               {
                  aviso(0,"1");
               }
               else
               {
                  if (qj != 0)
                     *ant_qj  = qj;
                  qj = 778; /* reentrante */		 
               }
            }
         }
         goto e_l_salida;
      }
      
      clave_lin_trata(f,(char *)&tmp,4,"");
      /* erafinal = 0;*/
      if (*final)
      {
          if (!*desorden && clave_lin_trata(f,*ultima,3,tmp) > 0)
             {
                *desorden = 1;
             }
          clave_lin_trata(f,linea,4,"");
      }
      else
      {
          if (clave_lin_trata(f,*linea,3,tmp) != 0) 
          {
            /* el modo compatible seria comprobando si la clave entrada es una ficha o no */            
            if (ya_puesto)
            {
               aviso(0,"1");
            }
            else
            {    
               /* *ant_qj = 0; */ /* el return hay que confirmarlo */
               qj = 778; /* reentrante */
            }
            goto e_l_salida;
          }
      }
      
      
      if (*final == 0) 
      {
         
         leida = leer_reg(f,ISCURR,ISLOCK,0);
         
         if (leida == 0 && *lleno != -2)
         {
			/*if (!sinbotones)*/ {
				w = w_ini(posm*100+1,posm*100+80);
				w_salva(w);
				switch(menusg(textom,letrasm,posm*100,"",1)) 
				{
				case -1:
				case 0:
				   libera_ind(v_mpfi(f));
				   continue;
				case 2:
				   pon_registro(f,*linea,0); /* redundante ! */
				   /* asegurarse leyendo la siguiente */
				   if (!siguientelinea(f,lim2,0,0,lim1)) 
				   {
					  clave_lin_trata(f,(char *)baja,4,"");
					  pon_registro(f,*baja,0);
				   }
				   if (primeralinea(f,lim2,ISLOCK,lim1,*linea)) 
				   {
					  rw_error(55);
					  break;
				   }
				   if (baja_fichao(f)) { /* no se hizo la baja */
					  if (*baja) {
						 free(*baja);
						 *baja = NULL;
					  }
				   }
				   libera_ind(v_mpfi(f));
				   qj = 777;
				   goto e_l_salida;
				default:
				   break;
				}
				w_pinta(w);
				w_fin(w);
			}
            w = NULL;
         }
      }
      else
      {
         salva_alta = da_registo_actual(v_mpfi(f));
         if (ipfi >= 0)
         {
            if (leer_reg(f,ISEQUAL,0,0) < 0)
               c = -1;
            else
               c = 0;
            pon_registro_actual(v_mpfi(f),salva_alta);
            apunta_ind(v_mpfi(f),0,ISEQUAL,0);
            apunta_ind(v_mpfi(f),k+1,ISCURR,0);
            if (!c)
            {
               qj = 778; 
               goto e_l_salida;
            }	 
            c = 0;
         }
         else
         if (trata_reg(f,2,ISLOCK,0,1,"Grabar ficha") < 0)
         {
           if (ya_puesto)
           {
              aviso(0,"XX1");
           }
           qj = 778; 
           goto e_l_salida;
         }
      }
      
      if (leida == 0
         ) {
         c = 0;
         if (*final != 0) 
         {
            if (memcmp(v_memo(5002)->alfabetica+29,"NOR",3))
            {
               pinta1_linea(f,*faclin,2,0);
            }
         }
         if ((qj = __entra_datos(altai,f,*faclin,sinbotones)) < 0 || qj == 7)
         {
            if (*final != 0)
            {
               if (ipfi < 0)
               {
                  borrar_reg(f,0,0);
               }
               libera_ind(v_mpfi(f));   /* desbloquea el registro */
               pon_registro_actual(v_mpfi(f),salva_alta);
               apunta_ind(v_mpfi(f),0,ISEQUAL,0);
               apunta_ind(v_mpfi(f),k+1,ISCURR,0);
            }
         }
         else {
         /*
         if (!memcmp(v_memo(5002)->alfabetica+34,"NUE",3))
         {
         memcpy(v_memo(5002)->alfabetica+34,"   ",3);
         *final = 0;
         clave_lin_trata(f,(char *)linea,4,"");
         qj = 778;
         goto e_l_salida;
         }
            */
            if (qj == 1) qj = 0;
            if (*final == 0) 
            {
               c = grabar_reg(f,0,0);
            }
            else 
            {
               clave_lin_trata(f,(char *)&tmp,4,"");
               /*
               if (k > 0 && noprim) 
               {
               apunta_ind(v_mpfi(f),1,ISCURR,0);
               }
               */
               if (ipfi >= 0)
                  c = grabar_reg(f,2,0);
               else
                  c = grabar_reg(f,0,0);
                  /*
                  if (k > 0 && noprim)
                  {
                  apunta_ind(v_mpfi(f),k+1,ISCURR,0);
                  }
               */
            }
            /* revisar esto probando si se quita 
            if (*final && !noprim) {
            qj = 0; 
            }
            */
            if (memoplin > 3 && memcmp(v_memo(5002)->alfabetica+22,"si",2) == 0) 
            {
               memset(v_memo(5002)->alfabetica+22,'\0',2);
               *final = 0;
               clave_lin_trata(f,(char *)linea,4,"");
               qj = 777;
               libera_ind(v_mpfi(f));   /* desbloquea el registro */
               goto e_l_salida;
            }
         }
         libera_ind(v_mpfi(f));   /* desbloquea el registro */
         if (c != 0 || qj < 0 || qj == 7)
         {
            if (qj < -1) qj = (-qj) - 2;  	
            if (*final == 0) 
            {
               leer_reg(f,ISCURR,0,1);
               clave_lin_trata(f,(char *)linea,4,"");
               pon_registro(f,*linea,0);
            }
            else 
            {
               /* *final = 1;*/
               defecto_ficha(v_mpfi(f));
               if (ipfi >= 0)
                  deferel_rel(f,ipfi,-1,0,ALTA,-1);
               f_n(f,RENBUF);
               clave_lin_trata(f,*linea,5,"");
            }           
            continue;
         }
      }
      else
         continue;
      break;
}

pon_registro(f,*linea,0);

if (*final != 0) 
{
   if (!*desorden)
      copia_linea(f,ultima,*linea);    
   if (/* qj == 2 || */qj == 4 || (qj == 5 && *desorden == 2))
   {
      if (*desorden)
      {
         if (qj == 2)
            *ant_qj = -2;
         else
            *ant_qj = qj;
         qj = 778;
         *desorden = 0;
      }	  
   }
   if (qj == 5 || qj == 3) qj = 0;
}   
if (qj == 2) 
{
   qj = 0;
   *final = 0;
   goto principio;
}
e_l_salida:   

if (w)
{
   w_pinta(w);
   w_fin(w);
   w = NULL;
}

if (!nopintar) 
pinta1_linea(f,*faclin,0,0);

if (tmp) free(tmp);

modo_de_validacion = old_modo_validacion;

return(qj);
}


/* determina el minimo salto necesario para estas lineas */
static int determina_salto(int flin)
{
   int campini,campfin,numcam,c;
   int ini = 9999,p,salto = 1;

   if (v_mpfi(flin)->f_fi_modo_lineas)
   {
       return(100); /* de momento asi */
   }

   campini = v_mpfi(flin)->f_pantalla[0].p_campoini;
   campfin = v_mpfi(flin)->f_pantalla[0].p_campofin;
   for (c=campini; c<=campfin;c++)
   {
      numcam = v_mpfi(flin)->f_taborder[c-1];
      if (v_mpfi(flin)->f_campos[numcam].c_qc != -2 &&
         (v_mpfi(flin)->f_campos[numcam].c_mante.c_lineal& C_LINEAL) &&
         v_mpfi(flin)->f_campos[numcam].c_mante.c_visual == 0)
      {	
         p = (v_mpfi(flin)->f_campos[numcam].c_mante.c_qp)/100;
         if (ini > p)
         {
            ini = p;
         }	    
         if ((p-ini+1) > salto)
            salto = p-ini+1;
      }
   }
   if (salto > (IncrementoLineas/100))
   {
      IncrementoLineas = salto * 100;
      SumaIncremento = IncrementoLineas - 100;   
   }
   
   return(0);
}

/* determina el total de lineas que caben en una pantalla */
int total_lineas(flin)
int flin;
{
   int campini,campfin,numcam,c;
   int limitel = dame_limite_lineas(flin);

   if (v_mpfi(flin)->f_fi_modo_lineas)
   {
       return(limitel);
   }
   
   campini = v_mpfi(flin)->f_pantalla[0].p_campoini;
   campfin = v_mpfi(flin)->f_pantalla[0].p_campofin;
   for (c=campini; c<=campfin;c++)
   {
      numcam = v_mpfi(flin)->f_taborder[c-1];
      if (v_mpfi(flin)->f_campos[numcam].c_qc != -2 &&
         (v_mpfi(flin)->f_campos[numcam].c_mante.c_lineal& C_LINEAL) &&
         v_mpfi(flin)->f_campos[numcam].c_mante.c_visual == 0)
      {	
         return( ((((limitel - (v_mpfi(flin)->f_campos[numcam].c_mante.c_qp/100) + 1)) * 100) / IncrementoLineas)  );
      }
   }
   return(0);
}


static int ancho_columna(flin,modo)
int flin;
int modo;
{
   int campini,campfin,numcam,c,i,j,col,mincol = 9999,maxcol = 0,pini = 9999;
   
   
   campini = v_mpfi(flin)->f_pantalla[0].p_campoini;
   campfin = v_mpfi(flin)->f_pantalla[0].p_campofin;
   for (c=campini; c<=campfin;c++)
   {
      numcam = v_mpfi(flin)->f_taborder[c-1];
      if (v_mpfi(flin)->f_campos[numcam].c_qc != -2 &&
         (v_mpfi(flin)->f_campos[numcam].c_mante.c_lineal& C_LINEAL) &&
         v_mpfi(flin)->f_campos[numcam].c_mante.c_visual == 0)
      {	
           col = v_mpfi(flin)->f_campos[numcam].c_mante.c_qp;
           if (pini > col) pini = col;
           col %= 100;
           i = v_mpfi(flin)->f_campos[numcam].c_mante.c_qpl;
           j = long_campo(v_mpfi(flin),(numcam));
           if ((v_mpfi(flin)->f_campos[numcam].c_tipo == FECHATIPO &&  i != 6 && i != 8) ||
               (v_mpfi(flin)->f_campos[numcam].c_tipo != CHARTIPO  && v_mpfi(flin)->f_campos[numcam].c_tipo != FECHATIPO) ||
               i < 1 || i > j)
               i = j;
           if (col < mincol)
               mincol = col;
           if ((col+i) > maxcol)
               maxcol = col+i;           
      }
   }

   if (modo)
   {
       col = 80 - (pini % 100) + 1;
       return(col / (maxcol-mincol+1));
   }
   else
       return(maxcol-mincol+1);
}

int scrola_lineas(flin,ll,factor)
int flin,ll;
int factor;
{
   int campini,campfin,numcam,c,ini,fin;
   int limitel,total_l;
   int j,i,poner = 0;
   int mincol = 9999,minlin = 9999,maxlin = 0;;
   

   if (!factor && (ll == -1 || ll == 1))
      poner = 1;

   limitel = dame_limite_lineas(flin);

   if (!l_separador || t_ini == -1 || !poner)
   {   
      if (poner || v_mpfi(flin)->f_fi_modo_lineas)
           t_ini = -1;      
      campini = v_mpfi(flin)->f_pantalla[0].p_campoini;
      campfin = v_mpfi(flin)->f_pantalla[0].p_campofin;
      for (c=campini; c<=campfin;c++)
      {
           numcam = v_mpfi(flin)->f_taborder[c-1];
           if (v_mpfi(flin)->f_campos[numcam].c_qc != -2 &&
               (v_mpfi(flin)->f_campos[numcam].c_mante.c_lineal& C_LINEAL) &&
               v_mpfi(flin)->f_campos[numcam].c_mante.c_visual == 0)
           {

               if (v_mpfi(flin)->f_fi_modo_lineas) 
               {
                   ini = v_mpfi(flin)->f_campos[numcam].c_mante.c_qp + (factor/100)*v_mpfi(flin)->f_fi_modo_lineas;
               }
               else
               {
                   ini = v_mpfi(flin)->f_campos[numcam].c_mante.c_qp + factor;
               }
               if (v_mpfi(flin)->f_fi_modo_lineas) 
               {
                   if (mincol > (ini%100))
                       mincol = ini%100;
                   if (minlin > (ini/100))
                       minlin = ini/100;
                   if (maxlin < (ini/100))
                       maxlin = ini/100;
               }
               else
               {
                   i = v_mpfi(flin)->f_campos[numcam].c_mante.c_qpl;
                   j = long_campo(v_mpfi(flin),(numcam));
                   if ((v_mpfi(flin)->f_campos[numcam].c_tipo == FECHATIPO &&  i != 6 && i != 8) ||
                       (v_mpfi(flin)->f_campos[numcam].c_tipo != CHARTIPO  && v_mpfi(flin)->f_campos[numcam].c_tipo != FECHATIPO) ||
                       i < 1 || i > j)	    
                       i = j;

                   total_l = ((((limitel - (ini/100)) + 1) / (IncrementoLineas/100) ) * (IncrementoLineas/100)) - 1;           
                   fin = (total_l * 100) + ini + i - 1;
                   if (!l_separador || !poner)
                   {
                       v_scroll(v_mpfi(flin)->f_id,v_mpfi(flin)->f_pantasids[0],ini,fin,ll*(IncrementoLineas/100));
                   }          
                   else
                   {
                       if (t_ini == -1)
                           t_ini = ini;
                       t_fin = fin;
                   }
               }
           }
      }
   }

   if (v_mpfi(flin)->f_fi_modo_lineas) 
   {
       if (t_ini < 0)
       {
           t_ini = minlin*100+mincol;
           t_fin = maxlin*100+mincol+( limitel*v_mpfi(flin)->f_fi_modo_lineas )-1;
       }
       v_scroll(v_mpfi(flin)->f_id,v_mpfi(flin)->f_pantasids[0],t_ini,t_fin,(ll*(IncrementoLineas/100)*v_mpfi(flin)->f_fi_modo_lineas)*100);
   }
   else
   {
       if (l_separador && poner)
       {
          v_scroll(v_mpfi(flin)->f_id,v_mpfi(flin)->f_pantasids[0],t_ini,t_fin,ll*(IncrementoLineas/100));
       }
   }

   return(0);
}

int pon_salto_de_lineas(p,tip,faclin)
int p;
int tip;
int faclin;
{
			int salto = pasa_a_int(v_memo(prog[p].indice)->numerica);
         
         if (salto > 0)
         {
            IncrementoLineas = salto * 100;
            SumaIncremento = IncrementoLineas - 100;
         }
         n_np = 1;
         return(0);
}

static int entra_lineas(ipfi,f,modo,lim1,lim2,mensa_,posicion,sin_barra)
int ipfi,f,modo;
char *lim1;
char *lim2;
int mensa_;
char *posicion;
int sin_barra;
{
   int i,hacer,lleno,h,k,kk,noprim = 0;
   int saleret = 0;
   int faclin = 100;
   int final = 0;
   int r = 0,rr = -1,kkk,ant_qj = -1;
   int total_l = 0;
   long salvar;
   char *buf2 = NULL;
   char *linea = NULL;
   char *ultima  = NULL,
      *primera = NULL;
   char *baja = NULL;
   char gcontrol[MAXDEFECTO];
   char ncontrol[MAXDEFECTO];
   int desorden = 0,menor_es = 0,nn;
   int _campo_color = campo_color,_que_color = que_color,_cambio_color_fijo = cambio_color_fijo;
   int _file_color = file_color;
   char *_color_factor_inicio = color_factor_inicio;
   int _que_color_inicio   = que_color_inicio;
   int  _atributo_1 = atributo_1;
   int  _atributo_2 = atributo_2;
   char *_color_factor = color_factor;
   int  _len_color = len_color;
   int _p_color = p_color;
   unsigned char _l_separador = l_separador;
   int _t_ini = t_ini;
   int _t_fin = t_fin;
   int c_t;
   char tmpn[3];
   int _IncrementoLineas = IncrementoLineas;
   int _SumaIncremento   = SumaIncremento;
   int plin;
   int sinbotones = 0;
   
   if (memoplin > 2) memcpy(gcontrol,v_memo(5002)->alfabetica,MAXDEFECTO); /* Control */
   if (memoplin > 3) memcpy(ncontrol,v_memo(5003)->alfabetica,MAXDEFECTO); /* FEntrada */
   
   
   if (memoplin > 2)
   {
      memcpy(v_memo(5002)->alfabetica+29,"   ",3);
      memcpy(v_memo(5002)->alfabetica+32,"  ",2);
      memcpy(v_memo(5002)->alfabetica+34,"   ",3);
   }
   
   if (modo == ELECCION)  {modo = CONSULTA;saleret = 1;}
   if (modo == SOLOMODIFICA)  {modo = MODIF;saleret = 2;}
   

   if (!v_mpfi(f)->f_posmenu) {
   	  v_mpfi(f)->f_posmenu = std_size/100;
	   if (!espuro10 && v_mpfi(f)->f_posmenu < 24)
		   v_mpfi(f)->f_posmenu = 24;
   }


   if (ipfi == -2) 
   {
      /*int xxx;*/
      if (modo == DIFERIDO) 
        {
         rr = 0;
         goto SalirLimpio;
        }
      /*
      pinppan(f,0);
      */
        if (v_mpfi(f)->f_pantaids[0] == -1)
        {
              pinppan(f,0);
        }
        default_wid = v_mpfi(f)->f_pantaids[0];
        default_sid = v_mpfi(f)->f_pantasids[0];
   }
   else
   {
     if (ipfi >= 0)
     {  
		 if (!v_mpfi(ipfi)->f_posmenu) {
   		   v_mpfi(ipfi)->f_posmenu = std_size/100;   
		   if (!espuro10 && v_mpfi(ipfi)->f_posmenu < 24)
			   v_mpfi(ipfi)->f_posmenu = 24;
		 }

        plin = AjustaPanLin(ipfi,f);
        
        pinppan(ipfi,plin);
        v_mpfi(f)->f_pantaids[0] = v_mpfi(ipfi)->f_pantaids[plin];
        v_mpfi(f)->f_pantasids[0] = v_mpfi(ipfi)->f_pantasids[plin];
        v_mpfi(f)->f_id = v_mpfi(ipfi)->f_id;
		printpan(ipfi,plin);
		printprel(ipfi,plin);      

        if (modo != DIFERIDO && modo != VISUALIZA) 
        {
              r = -1;          /* las lineas estan pintadas ya */
        }        
     }     
     else
     {
        v_mpfi(f)->f_pantaids[0] = default_wid;
        v_mpfi(f)->f_pantasids[0] = default_sid;
        v_mpfi(f)->f_id = default_id;
     }
     
   }

      
      
      IncrementoLineas = 100; /* resetear valor para evitar 'molestias' */
      SumaIncremento = 0;
      
      
      h = da_cla_lin(f) + 1;
      if (h > 1) 
      {
         noprim = 0;
         for (i = v_mpfi(f)->f_clave[0].k_ncampos - 1;i > -1;i--) 
         { /* ver si la clave primaria 'aparece' y algo se puede entrar */
            k = v_mpfi(f)->f_clave[0].k_campo[i] - 1;
            if (!(v_mpfi(f)->f_campos[k].c_mante.c_modif) && !(v_mpfi(f)->f_campos[k].c_mante.c_visual)) 
            {
               noprim = 1;			
               break;
            }		 
         }
         cambia_ind(v_mpfi(f),h);
						}
      
      buf2 = malloc( clave_lin_trata(f,NULL,0,"") + 1 );
      
      file_color = -1;
      campo_color = 0;
      que_color = 0;
      cambio_color_fijo = 0;
      atributo_1 = 0;
      atributo_2 = 0;
      color_factor = NULL;
      color_factor_inicio = NULL;
      que_color_inicio   = 0;
      len_color = 0;
      p_color = 0;
      l_separador = 0;
      t_ini = -1;
      t_fin = -1;
      
      ejecuta_otro(f,5,O_NOR,0,0);
      if (!strstr(v_memo(5004)->alfabetica,"SINBARRA"))
      {
          sin_barra = 1;
      }
      else if (!strstr(v_memo(5004)->alfabetica,"CONBARRA"))
      {
          sin_barra = 0;
      }
      if (!strstr(v_memo(5004)->alfabetica,"::ENDATOS::SINBOTONES::")) {		   
		  sinbotones = 1;
	  }
      
      determina_salto(f);
      
      total_l = total_lineas(f);
      
      /* antes que nada comprobar el modo, si es alta posicionarse al final */
      if (modo == ALTA && !posicion)
      {
         if (!primeralinea(f,lim2,0,lim1,lim1))
         { /* ojo hay lineas */
            clave_lin_trata(f,lim2,5,"");
            if ( leer_reg(f,ISGREAT,0,1) )
               leer_reg(f,ISLAST,0,1);
            else
               leer_reg(f,ISPREV,0,1);
            clave_lin_trata(f,(char *)&primera,4,"");	  
            clave_lin_trata(f,(char *)&linea,4,"");	  
            if (primeralinea(f,lim2,0,lim1,primera))
            {
               anteriorlinea(f,lim2,0,0,lim1);
            }	  
            kkk = total_l / 2;
            while(kkk > 0 && !anteriorlinea(f,lim2,0,0,lim1)) 
            {
               clave_lin_trata(f,(char *)&primera,4,"");
               kkk--;
            }
            primeralinea(f,lim2,0,lim1,primera);
            posicion = primera;
            r = 0;
         }
      }
      if (!posicion)
         posicion = lim1;
      lleno = pinta_lineasfin(f,&faclin,lim2,&ultima,&primera,((linea) ? &linea : NULL),r,lim1,posicion,&c_t);      
      
      if (modo == VISUALIZA)
      {
         rr = 0;
         goto e_ls_salida;
      }

      if (!ultima) lleno = 0;
      if (modo && modo != DIFERIDO) {
         if (!ultima) {
            copia_linea(f,&linea,posicion); /* el caso posicion == lim2 por aqui no deberia pasar */
            copia_linea(f,&primera,posicion);
            r = leer_reg(f,ISGTEQ,0,1);
            if (r)
               r = leer_reg(f,ISLAST,0,1);
            else
            {
               r = anteriorlinea(f,lim2,0,0,lim1); /* poner el corriente anterior */
            }
            final = 1;
            desorden = 0;
         }
         else {
            if (!final) 
            {
               /*if (modo == ALTA && mensa_) pito(30);*/
               if (modo == ALTA && lleno > 0)
               {
                  primeralinea(f,lim2,0,lim1,ultima);
                  copia_linea(f,&linea,ultima);           
                  final = 1;
                  linea = suma_linea(f,linea,linea);
                  desorden = 0;
                  faclin += IncrementoLineas;
               }
               else
               {
                  primeralinea(f,lim2,0,lim1,primera);
                  clave_lin_trata(f,(char *)&linea,4,"");
                  pon_registro(f,linea,0);
                  faclin = 100;
                  final = 0;
                  pon_el_color(que_color_inicio,f);
               }
            }
         }
         
         
         
         if (DespachadorExterno)
         {
            int pi,pf;
            long pn;
            if (!sin_barra && dame_cuadro_lineas(f,&pi,&pf))
            {
                if (DespachadorExterno)
                {
                   char tmp[512];
                   int *p1 = (int *)tmp;
                   p1[0] = sizeof(int) * 6;
                   p1[1] = v_mpfi(f)->f_id;
                   p1[2] = v_mpfi(f)->f_pantasids[0];
                   p1[3] = pi+1;
                   p1[4] = pf+1;
                   p1[5] = -1;
                   if (v_mpfi(f)->f_fi_modo_lineas)
                   {
                       (*DespachadorExterno)((int)DESPACHO_EN_LINEAS,100,(void *)tmp);
                   }
                   else
                   {
                       (*DespachadorExterno)((int)DESPACHO_EN_LINEAS,0,(void *)tmp);
                   }
                   v_mpfi(f)->f_fi_scroll_lineas = p1[5];
                }
            }
         }
         
         
         do {                           /* donde esta a clave ultima de lin*/
            
            if (final == 1) 
            {
               defecto_ficha(v_mpfi(f));
               if (ipfi >= 0)
               {
                  deferel_rel(f,ipfi,-1,0,ALTA,-1);			
               }
               clave_lin_trata(f,linea,5,"");
            }
            
            if (!final && ipfi >= 0 && (!ultima || que_registro(f,primera) == que_registro(f,ultima))) 
            {
               if (anteriorlinea(f,lim2,0,0,lim1))
                  lleno = -2;
               primeralinea(f,lim2,0,lim1,primera);
            }
            
            if (baja) 
            {
               free(baja);
               baja = NULL;
            }	 
            
            hacer = entra_linea(f,&linea,&faclin,&ultima,modo,&final,&lleno,lim1,lim2,&baja,saleret,noprim,ipfi,&desorden,&primera,&ant_qj,sinbotones);
            if (lleno == -2) lleno = 1;
            if (hacer != 777 && hacer != 1 && !que_registro(f,primera)) 
            {/* falta comprobar que al borrar todos reajuste esto tambien */
               pon_registro(f,primera,0);
            }
            if (memcmp(v_memo(5002)->alfabetica+32,"  ",2))
            {
               tmpn[0] = v_memo(5002)->alfabetica[32];
               tmpn[1] = v_memo(5002)->alfabetica[33];
               tmpn[2] = 0;
               if (tmpn[0] >= 48 && tmpn[0] <= 57 && tmpn[1] >= 48 && tmpn[1] <= 57)
               {
                  /* A CRITERIO DEL PROGRAMADOR EL DESCUADRAR LAS LINEAS O NO OJO!!!*/
                  nn = atoi(tmpn) + (faclin+SumaIncremento) / IncrementoLineas;
                  if (nn > total_l)
                  {
                     nn -= total_l;
                     faclin -= (nn * IncrementoLineas);
                     nn = -nn;
                     scrola_lineas(f,nn,0);
                  }
               }		 
               memcpy(v_memo(5002)->alfabetica+32,"  ",2);
            }
            
            
            switch (hacer) 
            {
            case 0 :
            case 3 :		
            case SOLICITA_SCROLLDOWN:
               salvar = que_registro(f,linea);
               if (!final)
               {         
                  if (siguientelinea(f,lim2,0,0,lim1) != 0) 
                  { /* si envio final no prevee */
                     if (modo == CONSULTA) break;    /* que aparezcan nuevas lineas */
                     final = 1;        
                     desorden = 0;
                     primeralinea(f,lim2,0,lim1,linea);
                     if (se_suma_l(ipfi,f))
                        linea = suma_linea(f,linea,linea);
                     if (ultimalin(linea,f,lim2) == -1)
                     {
                        
                     /* 
                     final = 0;
                     clave_lin_trata(f,(char *)&linea,4,"");
                     pon_registro(f,linea,0);	          
                     continue;
                        */
                        copia_linea(f,&linea,lim1);
                     }
                  }
                  else 
                  {
                     clave_lin_trata(f,(char *)&linea,4,"");
                     pon_registro(f,linea,0);
                  }
               }
               else
                  if (se_suma_l(ipfi,f))
                     linea = suma_linea(f,linea,linea);
                  
                  if (_si_escrola)
                  {
                     if (((faclin+SumaIncremento)/IncrementoLineas) < total_l)
                     {
                        if (desorden == 2 && ((faclin+SumaIncremento)/IncrementoLineas) < (total_l-1))
                           scrola_lineas(f,1,faclin); 
                        faclin += IncrementoLineas;
                     }
                     else
                     {
                        if (!desorden)
                        {
                           /* escrolar */
                           r = primeralinea(f,lim2,0,lim1,primera);
                           if (!r) r = siguientelinea(f,lim2,0,0,lim1);
                           if (r)
                              faclin += IncrementoLineas; /* anulado scroll */
                           else
                           {					 	                
                              if (!cambio_color_fijo && color_factor && campo_color > 0 && color_factor_inicio)
                              {
                                 if (memcmp(color_factor_inicio,v_mpfi(f)->f_buf+p_color,len_color))
                                 {
                                    if (que_color_inicio)
                                       que_color_inicio = 0;
                                    else
                                       que_color_inicio = 1;
                                    memcpy(color_factor_inicio,v_mpfi(f)->f_buf+p_color,len_color);
                                 }
                              } 
                              clave_lin_trata(f,(char *)&primera,4,"");
                              pon_registro(f,primera,0);                 				 
                              /* scrolar ventana */
                              scrola_lineas(f,-1,0);
                           }				
                           if (final)
                           {
                              pon_registro_actual(v_mpfi(f),salvar);
                              pon_registro(f,linea,0);
                           }				 
                           primeralinea(f,lim2,0,lim1,linea);			     
                           if (final)
                           {
                              clave_lin_trata(f,(char *)&linea,4,"");
                              pon_registro(f,linea,1);
                              if (se_suma_l(ipfi,f))
                                 linea = suma_linea(f,linea,linea);
                           }
                           else
                              copia_linea(f,&ultima,linea);
                        }
                        else
                        {
                           scrola_lineas(f,-1,0);
                           copia_linea(f,&primera,linea);
                        }
                     }
                  }
                  else
                     faclin += IncrementoLineas;
                  break;
            case 5:
               if (final == 1) 
               { /* esto solo ocurre cuando es la ultima */ /*!!!!*/
                  borra_lineas(f); /* y esta al final de pagina */
                  copia_linea(f,&primera,linea);
               }
               else {
                  if (primeralinea(f,ultima,0,ultima,ultima)) { /* !!!! */
                     rw_error(49);
                  }
                  if (siguientelinea(f,lim2,0,0,lim1) != 0) 
                  {
                     clave_lin_trata(f,linea,5,"");
                     if (modo == CONSULTA) 
                     {
                        primeralinea(f,lim2,0,lim1,linea);
                        break;
                     }
                     final = 1;
                     desorden = 0;
                     if (ultimalin(suma_linea(f,ultima,buf2),f,lim2) == -1)
                     {
                        copia_linea(f,&linea,lim1);
                     }
                     else
                        copia_linea(f,&linea,suma_linea(f,ultima,buf2));
                     if (lleno < 0)
                     {
                        if (primeralinea(f,primera,0,primera,primera)) 
                        { /* !!!! */
                           rw_error(49);
                        }
                        if (!siguientelinea(f,lim2,0,0,lim1)) 
                        {
                           clave_lin_trata(f,(char *)&primera,4,"");
                        }
                     }
                     /*
                     if (lleno >= 0) 
                     {			
                     */
                     faclin = 100; /* ya que se pinta desde la primera */
                     lleno = pinta_lineasfin(f,&faclin,lim2,&ultima,&primera,NULL,0,lim1,primera,&c_t);			   
                     if (!ultima) lleno = 0;
                     faclin += IncrementoLineas;
                     break;			
                     /*
                     }			 
                     borra_lineas(f);
                     copia_linea(f,&primera,linea);
                     */
                  }
                  else 
                  {
                     clave_lin_trata(f,(char *)&linea,4,"");
                     pon_registro(f,linea,0);
                     faclin = 100;
                     lleno = pinta_lineasfin(f,&faclin,lim2,&ultima,&primera,NULL,0,lim1,linea,&c_t);		   
                     if (!ultima) 
                        lleno = 0;		      
                     primeralinea(f,lim2,0,lim1,linea);
                     pon_el_color(que_color_inicio,f);
                  }
               }
               faclin = 100;
               break;
            case 2:
            case SOLICITA_SCROLLUP:
               /* clave_lin_trata(f,linea,3,primera) > 0 */
               /* si final y faclin == 100 se esta en la primera linea */
               /* y no hay registro */
               if (desorden)
               {
                  /* ordenar !! */
                  /*aviso(0,"DESORDEN1!"); */
               }
               if (!final && faclin == 100 && que_registro(f,linea) != que_registro(f,primera))
               {
                  /* caso de alta especial (la ficha se graba desde calculo) */
                  /* caso de borrar la primera linea */
                  clave_lin_trata(f,(char *)&linea,4,"");
                  pon_registro(f,linea,0);
                  copia_linea(f,&primera,linea);
                  pon_registro(f,primera,0);
                  /* ajustado (!) */
               }
               r = 0;
               if ((!final || faclin > 100) && que_registro(f,linea) != que_registro(f,primera))
               {
                  r = 1;
               }
               else
                  if (_si_escrola)
                  {
                     r = 1;
                  }
                  if (r) {
                     if (final == 0) {
                        if (anteriorlinea(f,lim2,0,0,lim1))
                           break;
                     }
                     else {
                        r = anteriorlinea(f,lim2,1,0,lim1);
                        if (r != 0) break;
                        clave_lin_trata(f,(char *)&ultima,4,"");
                        pon_registro(f,ultima,0);
                     }
                     clave_lin_trata(f,(char *)&linea,4,"");
                     pon_registro(f,linea,0);
                     r = 0;
                     if (faclin > 100)
                     {
                        faclin -= IncrementoLineas;
                     }
                     else /* escrolar (en teoria solo pasa por aqui en caso de scroll */
                     {
                        if (ultima)
                        {
                           if (!final && lleno == -1)
                           {
                              r = primeralinea(f,lim2,0,lim1,ultima);
                              if (!r) r = anteriorlinea(f,lim2,0,0,lim1);
                              if (!r) /* en caso contrario esta anulado el scroll */
                              {
                                 clave_lin_trata(f,(char *)&ultima,4,"");
                                 pon_registro(f,ultima,0);
                              }
                           }   
                        }
                        else
                        {
                           copia_linea(f,&ultima,linea);
                        }
                        if (lleno > -1)
                           lleno++;
                        if (lleno == total_l)
                           lleno = -1;       
                        primeralinea(f,lim2,0,lim1,linea);
                        if (!r)
                        {
                           copia_linea(f,&primera,linea);
                           pon_registro(f,primera,0);
                           if (!cambio_color_fijo && color_factor && campo_color > 0 && color_factor_inicio)
                           {
                              if (memcmp(color_factor_inicio,v_mpfi(f)->f_buf+p_color,len_color))
                              {
                                 if (que_color_inicio)
                                    que_color_inicio = 0;
                                 else
                                    que_color_inicio = 1;
                                 memcpy(color_factor_inicio,v_mpfi(f)->f_buf+p_color,len_color);
                              }
                           }
                           /* scrolar ventana */
                           scrola_lineas(f,1,0);
                        }
                     }
                     final = 0;
                     if (!r)
                        break;
                  }
            case 4 :
               /* determinar pagina anterior */
               if (desorden)
               {
                  /* ordenar !! */
                  /* aviso(0,"DESORDEN1!"); */
               }
               if (!final || faclin > 100)
               {
                  if (primeralinea(f,lim2,0,lim1,primera)) {pito(31);break;}
                  if (anteriorlinea(f,lim2,0,0,lim1))
                  {
                     /* es la primera pagina */
                     faclin = 100;
                     copia_linea(f,&linea,primera);
                     final = 0;
                     break;
                  }
               }
               else
               {
                  if (anteriorlinea(f,lim2,final,0,lim1)) {pito(32);break;}
               }
               i = total_l - 1;
               r = 1;
               while(-1)
               {
                  clave_lin_trata(f,&primera,4,"");
                  pon_registro(f,primera,0);
                  if (!i) break;
                  if (anteriorlinea(f,lim2,0,0,lim1)) break;
                  i--;
                  r++;
               }
               r += (faclin/IncrementoLineas);
               faclin = 100;
               if (!final && r <= total_l)
               {
                  primeralinea(f,lim2,0,lim1,linea);
                  anteriorlinea(f,lim2,0,0,lim1);
                  clave_lin_trata(f,&linea,4,"");
                  pon_registro(f,linea,0);
                  lleno = pinta_lineasfin(f,&faclin,lim2,&ultima,&primera,&linea,0,lim1,primera,&c_t);			
                  if (!ultima) lleno = 0;
               }
               else
               {
                  lleno = pinta_lineasfin(f,&faclin,lim2,&ultima,&primera,NULL,0,lim1,primera,&c_t);			
                  if (!ultima) lleno = 0;
                  copia_linea(f,&linea,ultima);
                  final = 0;
               }
               primeralinea(f,lim2,0,lim1,linea);
               break;
            case 778:
               clave_lin_trata(f,(char *)&buf2,4,"");
               menor_es = 0;
               if ((r = primeralinea(f,lim2,0,lim1,buf2)))
               {
                  /* no hay linea posterior ... buscar anterior */
                  clave_lin_trata(f,buf2,5,""); /* por si acaso */
                  r = leer_reg(f,ISGTEQ,0,1);
                  if (r)
                     r = leer_reg(f,ISLAST,0,1);
                  else
                     r = leer_reg(f,ISPREV,0,1);
                  if (!r)
                  {
                     clave_lin_trata(f,(char *)&buf2,4,"");
                     r = primeralinea(f,lim2,0,lim1,buf2);
                     menor_es = 1;
                  }
                  if (r)
                  {
                     /* no hay lineas !!! (normalmente solo puede ser por interferencia de otro usuario)*/
                     final = 1;
                     desorden = 0;
                     ant_qj = -1;
                     borra_lineas(f);
                     faclin = 100;
                     break;
                  }		    
               }
               clave_lin_trata(f,(char *)&linea,4,"");			 
               /* buscar primera !! */
               pon_registro(f,linea,0);
               nn = faclin / 100;
               while(nn > 1 && !anteriorlinea(f,lim2,0,0,lim1))
               {
                  nn -= (IncrementoLineas/100);
               }		    
               clave_lin_trata(f,&primera,4,"");
               pon_registro(f,primera,0);
               /* nn = faclin; */
               faclin = 100;
               lleno = pinta_lineasfin(f,&faclin,lim2,&ultima,&primera,&linea,0,lim1,primera,&c_t);		
               r = primeralinea(f,lim2,0,lim1,linea);
               if (c_t != -1) pon_el_color(c_t,f);
               if (ant_qj == -2 && final)
               {
                  r = anteriorlinea(f,lim2,0,0,lim1); 
                  if (!r)
                  {			
                     if (faclin > 199) faclin -= IncrementoLineas;			
                  }
                  else
                     r = primeralinea(f,lim2,0,lim1,linea);			 
               }
               if (!ultima || r) 
               { 
                  lleno = 0;                    
                  final = 1; /* se hizo lo que se pudo! */
                  desorden = 0;
               }
               else
                  clave_lin_trata(f,(char *)&linea,4,"");
               final = 0; /* implicacion de un 778 */
               break;
            case 777: /* anadir modificar otra linea o borrar */		 
               desorden = 0;
               clave_lin_trata(f,(char *)&buf2,4,"");	   
               /* pon_registro(f,buf2,0); */
               if (clave_lin_trata(f,buf2,3,ultima) > 0 ||
                  clave_lin_trata(f,buf2,3,primera) < 0)
               {
                  if (!baja)
                  {
                     copia_linea(f,&primera,buf2);
                     copia_linea(f,&linea,buf2);               
                  }
                  else 
                  {
                     copia_linea(f,&primera,baja);
                     copia_linea(f,&linea,baja);               	
                  }
               }
               else   
                  if (baja)
                  {
                     if (que_registro(f,linea) == que_registro(f,primera))
                        copia_linea(f,&primera,baja);
                     copia_linea(f,&linea,baja);
                  }
                  faclin = 100;
                  lleno = pinta_lineasfin(f,&faclin,lim2,&ultima,&primera,&linea,0,lim1,primera,&c_t);	 
                  if (!ultima) lleno = 0;
                  if (!ultima) 
                  {
                     clave_lin_trata(f,primera,5,"");
                     r = leer_reg(f,ISGTEQ,0,1);
                     if (r)
                        r = leer_reg(f,ISLAST,0,1);
                     if (r)
                     {
                        copia_linea(f,&linea,lim1);
                        final = 1; /* en todos los casos */
                        desorden = 0;
                     }
                     else
                     {
                        r = anteriorlinea(f,lim2,0,0,lim1);
                        if (r)
                        {
                           copia_linea(f,&linea,lim1);
                           final = 1; /* en todos los casos */                 
                           desorden = 0;
                        }
                        else
                        {
                           if (final)
                              copia_linea(f,&linea,primera);
                           else
                           {
                              clave_lin_trata(f,&primera,4,"");
                              lleno = pinta_lineasfin(f,&faclin,lim2,&ultima,&primera,&linea,0,lim1,primera,&c_t);				
                              if (!ultima) { 
                                 lleno = 0;                    
                                 final = 1; /* se hizo lo que se pudo! */
                                 desorden = 0;
                              }
                           }
                        }
                     }
                  }
                  else
                     final = 0;
                  primeralinea(f,lim2,0,lim1,linea);
                  if (c_t != -1) pon_el_color(c_t,f);
                  break;
            case SOLICITA_INICIO:
               faclin = 100;
               lleno = pinta_lineasfin(f,&faclin,lim2,&ultima,&primera,NULL,0,lim1,lim1,&c_t);
               if (!ultima || primeralinea(f,lim2,0,lim1,primera)) 
               {
                  faclin = 100;
                  copia_linea(f,&linea,lim1);
                  copia_linea(f,&primera,lim1);
                  r = leer_reg(f,ISGTEQ,0,1);
                  if (r)
                     r = leer_reg(f,ISLAST,0,1);
                  else
                  {
                     r = anteriorlinea(f,lim2,0,0,lim1); /* poner el corriente anterior */
                  }
                  final = 1;
                  desorden = 0;		   
                  break;
               }
               copia_linea(f,&linea,primera);
               faclin = 100;
               final = 0;
               desorden = 0;
               pon_el_color(que_color_inicio,f);
               break;
            case SOLICITA_FINAL:
               if (final == 1) break;
               clave_lin_trata(f,lim2,5,"");
               if ( leer_reg(f,ISGREAT,0,1) )
                  leer_reg(f,ISLAST,0,1);
               else
                  leer_reg(f,ISPREV,0,1);
               clave_lin_trata(f,(char *)&primera,4,"");
               if (primeralinea(f,lim2,0,lim1,primera))
               {		   
                  if (anteriorlinea(f,lim2,0,0,lim1))
                  {			  
                     /* !!!! */			  
                     defecto_ficha(v_mpfi(f));
                     if (ipfi >= 0)
                     {
                        deferel_rel(f,ipfi,-1,0,ALTA,-1);			
                     }
                     clave_lin_trata(f,linea,5,"");			  
                     final = 1;
                     break;
                  }
               }
               kkk = total_l;
               while(kkk > 1 && !anteriorlinea(f,lim2,0,0,lim1)) kkk--;
               clave_lin_trata(f,(char *)&primera,4,"");
               pon_registro(f,primera,0);
               faclin = 100;
               lleno = pinta_lineasfin(f,&faclin,lim2,&ultima,&primera,NULL,0,lim1,primera,&c_t);
               if (!ultima || primeralinea(f,lim2,0,lim1,ultima))
               {
                  faclin = 100;
                  copia_linea(f,&linea,lim1);
                  copia_linea(f,&primera,lim1);
                  r = leer_reg(f,ISGTEQ,0,1);
                  if (r)
                     r = leer_reg(f,ISLAST,0,1);
                  else
                  {
                     r = anteriorlinea(f,lim2,0,0,lim1); /* poner el corriente anterior */
                  }
                  final = 1;
                  desorden = 0;		   
                  break;
               }
               copia_linea(f,&linea,ultima);		
               final = 0;
               desorden = 0;		
               break;
            default :
               break;
    }
    if (baja) 
    {
       free(baja);
       baja = NULL;
    }
    if (hacer == 1) break;
   } while(hacer != -1);
   
   
   if (!sin_barra && DespachadorExterno)
   {
      char tmp[512];
      int *p1 = (int *)tmp;
      p1[0] = sizeof(int)*2;
      p1[1] = v_mpfi(f)->f_fi_scroll_lineas;
      (*DespachadorExterno)((int)DESPACHO_EN_LINEAS,1,(void *)tmp);
      v_mpfi(f)->f_fi_scroll_lineas = -1;
   }   
   
   

   if (!ultima && modo != DIFERIDO && modo != CONSULTA) 
   {
      if (mensa_) 
      {
         Ar();
         aviso(0,"Se Anulan los datos Entrados");
         Cr();
      }
      rr = -1;
      goto e_ls_salida;
   }
   } /* hasta aqui si no es diferido o modo 0 */
   rr = 0;
e_ls_salida:
   /* liberar todo */
   if (linea)
      free(linea);
   if (ultima)
      free(ultima);
   if (primera)
      free(primera);
   if (buf2)
      free(buf2);
   if (baja)
      free(baja);
   
   if (h > 1) {
      cambia_ind(v_mpfi(f),1);
   }
   
   if (memoplin > 2) memcpy(v_memo(5002)->alfabetica,gcontrol,MAXDEFECTO);
   if (memoplin > 3) memcpy(v_memo(5003)->alfabetica,ncontrol,MAXDEFECTO);
   
   
   if (color_factor)
      free(color_factor);
   if (color_factor_inicio)
      free(color_factor_inicio);
   color_factor_inicio = _color_factor_inicio;
   file_color = _file_color;
   que_color_inicio   = _que_color_inicio;
   cambio_color_fijo = _cambio_color_fijo;
   campo_color = _campo_color;
   que_color   = _que_color;
   atributo_1 = _atributo_1;
   atributo_2 = _atributo_2;
   color_factor = _color_factor;
   len_color = _len_color;
   p_color = _p_color;
   l_separador = _l_separador;
   t_ini = _t_ini;
   t_fin = _t_fin;
   
SalirLimpio:
   IncrementoLineas = _IncrementoLineas;
   SumaIncremento   = _SumaIncremento;
   
   return(rr);
}


int trata_lineas(ipfi,modo,m,c,faclin) /*buscar relaciones de lineas*/
int ipfi;
int modo;
int m;  /* 0 == entrada de lineas , 1 = bucle index de las mismas */
/* 2 = si todo esta en la misma pantalla borra las lineas */
/* 3 = chekeo para ver si todo esta en la misma pantalla */
int c,faclin;
{
   int j,f,cerrar = 0,sal = 0,leida,n,ol;
   char *lim1;
   char *lim2;
   int modolineas;
   
   
   if ((leida = f_v(ipfi,RLEIDA)) == -1) f_s(ipfi,RLEIDA);
   for (j=0;j<v_mpfirel(ipfi)->nrel;j++) 
   {
      if (v_mpfirel(ipfi)->camminu[j] < 0 && v_mpfirel(ipfi)->camminu[j] > -15000) 
      {
         f = v_mpfirel(ipfi)->relacis[j];
         if (m == 5) /* elimnar grids de lineas */
		 {		 
			 if (v_mpfi(f)->f_puntoretropar > 0)
			 {
				 BorraGrid(v_mpfi(f)->f_puntoretropar);
				 v_mpfi(f)->f_puntoretropar = 0;
				 v_mpfi(f)->f_idgrafico = -1;
			 }
			 /*
			 if (v_mpfi(f)->f_idgrafico > -1)
			 {
				 v_mpfi(f)->f_idgrafico = -1;				 
			 }
			 */
			 continue;
		 }
         if (f_v(f,RLEIDA) == 0 || apli_aborto) continue;
         if (m == 4)
         {
             AjustaPanLin(ipfi,f);
         }
         else
         if (m == 3) 
         {
             /*
            if ( (-((v_mpfirel(f)->lineas % 160) % 6)) != pag_activa[ipfi]) {
               sal = -1;
               break;
            }
            */
         }
         else
            if (m == 2) 
            {
               n = AjustaPanLin(ipfi,f);
               pinppan(ipfi,n);
               borra_lineas(f);
            }
            else
            {
               if ((cerrar = f_v(f,RNOCERRAR)) == -1)
               {
                  if (v_mpfi(f)->f_fd >= 0)
                     cerrar = 1;
                  f_s(f,RNOCERRAR);
               }
               if (abre1_rel(f) == 0) 
               {
                  f_s(f,RLEIDA);
				  modolineas = 0;
                  /* interface modo viejo -> modo nuevo */
                  lim1 = NULL;
                  lim2 = NULL;
                  /* establecer limite inferior y superior ... formato lineas mante */
                  ol = v_mpfi(f)->f_fi_clave_lineas;
				  if ((-((v_mpfirel(f)->lineas % 160) % 6)) == 4)
				  {
					  modolineas = 1;
					  /* X */
					  n = (-v_mpfirel(ipfi)->camminu[j]) - 2;
					  if (n < 0 || n >=  v_mpfi(f)->f_ncampos)
						  modolineas = 9999;
				  }
				  else
				  {
					  n = ( (-(v_mpfirel(f)->lineas))/160 - 2 ); /* campo linea!! */
				  }
				  if (modolineas != 9999)
				  {
					  if (!v_mpfi(f)->f_campos[n].c_key) 
					  {
						 rw_error(79);
					  }
					  v_mpfi(f)->f_fi_clave_lineas = n * 100 + v_mpfi(f)->f_campos[n].c_key - 1;
                  
					  defecto_ficha(v_mpfi(f));
					  deferel_rel(f,ipfi,-1,0,ALTA,-1);
					  minimo_campo(v_mpfi(f),n);
					  clave_lin_trata(f,&lim1,4,"");
					  maximo_campo(v_mpfi(f),n);
					  clave_lin_trata(f,&lim2,4,"");                  
					  
					  if (m == 0)
					  {
						 if (modolineas)
						 {
							 sal = lineasgrid(ipfi,f,modo,lim1,lim2,1);
							 cerrar = 0;
						 }
						 else
							 sal = entra_lineas(ipfi,f,modo,lim1,lim2,1,NULL,0);
					  }
					  else if (m == 1) 
					  {
						 sal = bucle_lineas(f,modo,c,faclin,lim1,lim2);
					  }
                     
					  if (lim1)
						free(lim1);
					  if (lim2)
						free(lim2);
				  }

                  v_mpfi(f)->f_fi_clave_lineas = ol;
                 
                  if (cerrar) 
                  {
                    f_n(f,RNOCERRAR);
                    if (cerrar == -1)
                       cierra1_rel(f);
                  }
                  f_n(f,RLEIDA);
                  if (sal != 0) break;
               }
            }
      }
   }
   if (leida == -1) f_n(ipfi,RLEIDA);
   
   return(sal);
}

int fgv_consul_clave(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int cl1,cl2;
   int i = prog[p].indice;
   char *ffiltro = NULL;
   char tmp[257];
   p++;
   
   cl1 = prog[p].indice;
   p++;
   if (cl1 >= 0)
			{
      cl1 = en_pr[programa_activo].tabla_gosubs[cl1];
			}
   
   cl2 = prog[p].indice;
   p++;
   if (cl2 >= 0)
			{
      cl2 = en_pr[programa_activo].tabla_gosubs[cl2];
			}   
   
   {
	    char *p;
		p = v_memo(5004)->alfabetica;
		p = strstr(p,"{F:");
		if (p)
		{
			char *p1;
			int n,i;
			p += 3;
			p1 = p;
			while(*p1 && *p1 != '}') p1++;
			n = (int)(p1 - p);
			if (n >= sizeof(tmp))
			{
				n = sizeof(tmp) - 1;
			}
			memcpy(tmp,p,n);
			tmp[n] = 0;
			ffiltro = tmp;
		}		
   }
   sprintf(v_memo(5004)->alfabetica,"%d",consul_clave(v_prog(i%100),i/100,-1,cl1,cl2,0,-1,-1,ffiltro));
   n_np = 3;
   return(0);
}

int rw_lineas(p,tip,faclin)
int p;
int tip;
int faclin;
{
		 char *lim1 = NULL,*lim2 = NULL;
         char *posicion = NULL;
         int i,j,ii;
         int par[6];
         int abierto;
         int campo;
         int sin_barra = 0;
         int encolumnas = 0;
         int k = prog[p].indice/100 - 1,         
            f = v_prog(prog[p].indice%100),
            ol = v_mpfirel(f)->lineas,
            oll = v_mpfi(f)->f_fi_clave_lineas,
            olc = v_mpfi(f)->f_fi_modo_lineas;

         
         if (prog[p].i == 255)
         {      
             encolumnas = 1;
         }


         /* par[0] == campo en clave */
         /* par[1] == modo */
         /* par[2] == limite */
         /* par[3] == pintar pantalla */
         
         p++;
         for (j = 0;j < 4;j++,p++) {
            
            i = prog[p].indice;
            if (i < 0) {i = (-i)-1;i = pasa_a_int(v_memo(i)->numerica);}
            par[j] = pasa_a_int(v_memo(i)->numerica);
         }

         if (par[0] < 0)
         {
             sin_barra = 1;
             par[0] = -par[0];
         }
         
         i = par[0] - 1;
         if (i < 0 || i >= v_mpfi(f)->f_clave[k].k_ncampos) 
         {
            rw_error(47);
            i = 0;
         }
         campo = v_mpfi(f)->f_clave[k].k_campo[i] - 1;
         i = ((par[3]) ? -2 : -1);
         
         par[4] = prog[p].indice;
         p++;
         if (par[4] >= 0)
         {
            par[4] = en_pr[programa_activo].tabla_gosubs[par[4]];
         }
         
         par[5] = prog[p].indice;
         p++;
         if (par[5] >= 0)
         {
            par[5] = en_pr[programa_activo].tabla_gosubs[par[5]];
         }
         
         v_mpfi(f)->f_fi_clave_lineas = campo * 100 + k;
         if (encolumnas)
         {
             v_mpfi(f)->f_fi_modo_lineas = ancho_columna(f,0);
         }
         else
         {
             v_mpfi(f)->f_fi_modo_lineas = 0;
         }
         
         if (encolumnas)
         {
             ii = ancho_columna(f,1);
             if (ii < par[2])
             {
                 par[2] = ii;
             }
         }
         v_mpfirel(f)->lineas = -(par[2] * 6); /* limite */
         
         
         strcpy(v_memo(5004)->alfabetica,"POSICION");
         p_ejecuta(par[4],tip,faclin,19); /* posicion */
         
         clave_lin_trata(f,(char *)&posicion,4,"");
         strcpy(v_memo(5004)->alfabetica,"");
         p_ejecuta(par[4],tip,faclin,20); /* limite inferior */
         
         clave_lin_trata(f,(char *)&lim1,4,"");
         
         if (!clave_lin_trata(f,lim1,3,posicion))
         {
            if (posicion)
               free(posicion);
            posicion = NULL;
         }
         p_ejecuta(par[5],tip,faclin,21); /* limite superior */
         clave_lin_trata(f,(char *)&lim2,4,"");
         
         if (v_mpfi(f)->f_fd >= 0)
            abierto = 0;
         else
            abierto = -1;
         
         /* abierto = f_v(f,RABIERTO); */
         if (f_v(f,RLEIDA) == 0)
            strcpy(v_memo(5004)->alfabetica,"-100");
         else {
            if (!abre1_rel(f)) {
               f_s(f,RLEIDA);
               j = entra_lineas(i,f,par[1],lim1,lim2,0,posicion,sin_barra);
               if (par[1] != ELECCION || j < 0) /* cuidar caso de ELECCION */
                  sprintf(v_memo(5004)->alfabetica,"%d",j);
               f_n(f,RLEIDA);
               if (abierto == -1)
                  cierra1_rel(f);
            }
            else
               strcpy(v_memo(5004)->alfabetica,"-200");
         }
         
         v_mpfirel(f)->lineas = ol;
         v_mpfi(f)->f_fi_clave_lineas = oll;
         v_mpfi(f)->f_fi_modo_lineas = olc; 
         
         if (lim1)
            free(lim1);
         if (lim2)
            free(lim2);
         if (posicion)
            free(posicion);
         
         n_np = 7;
         return(0);
}


int rw_pindat(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int  si,i,o;
   
   
   i = v_prog(prog[p].indice%100);
   o = prog[p].indice/100;
   
   si = -2;
   if (pasa_a_int(v_memo(5007)->numerica) > 0 && pasa_a_int(v_memo(5007)->numerica)%100 == 0) {
      faclin = pasa_a_int(v_memo(5007)->numerica);
      si = v_mpfirel(i)->lineas;     /* linea forzada por Pos */
      if (si > -2) { /* solo en caso de que el referio no sea de lineas */
         v_mpfirel(i)->lineas = -463;
      }
   }
   if ((v_mpfirel(i)->lineas < -1 || si > -2) && faclin > 0)
      pinta1_linea(i,faclin,o,0);
   else
      prin_pan(i,o);
   if (si > -2) {
      v_mpfirel(i)->lineas = si;
   }
   
   return(0);
}

int rw_setlincolor(p,tip,faclin)
int p;
int tip;
int faclin;
{
   short i,f0,f,c,r,pp;
   short ii;
   
   f0 = prog[p++].indice;
   n_np = 4;
   if (f0 < 0)
			{  
      n_np++;
      f0 =  -(f0) - 1;
      c = pasa_a_int(v_memo(f0)->numerica);
      f = v_prog(prog[p++].indice);
      if (c < 0 || c >= v_mpfi(f)->f_ncampos)
      {
         rw_error(43);
         return(0);
      }
			}
   else
			{
      comoes_cam(f0,NULL,NULL,NULL,&f,&c,NULL);  
			}
   
   file_color = f;
   campo_color = c + 1;
   atributo_1  = prog[p++].indice;
   atributo_2  = prog[p++].indice;
   ii = prog[p++].indice;
   if (ii < 0)
   {
       ii++;
       ii = -ii;
       cambio_color_fijo = 1;
   }
   else
   {
       cambio_color_fijo = 0;
   }
   l_separador = ii;
   que_color = 0;
   que_color_inicio = 0;
   p_color = v_mpfi(f)->f_campos[c].c_offset;
   len_color = v_mpfi(f)->f_campos[c].c_long;
   if (color_factor)
      free(color_factor);
   if (color_factor_inicio)
      free(color_factor_inicio);
   color_factor = malloc(len_color+1);
   color_factor_inicio = malloc(len_color+1);
   memcpy(color_factor,v_mpfi(f)->f_buf+p_color,len_color);
   memcpy(color_factor_inicio,v_mpfi(f)->f_buf+p_color,len_color);
   return(0);
}

extern int con_respaldo_externo;

extern int bbdd_tiempo_rotate;

static FI_DEF *__pfi = NULL;
static int __fd = -1;

int respaldo_externo_ixx_prep(FI_DEF *pfi,int fd)
{
	__pfi = pfi;
	__fd = fd;

	return 0;
}

void *respaldo_externo_ixx_init(int fd, char *path)
{
	int i;
	FI_DEF *r_s_my_pfi = NULL;    

   if (!con_respaldo_externo && !bbdd_tiempo_rotate)
   {
       return NULL;
   }

   if (fd == -1 && path) {
		for (i = 0;i < nfi;i++)
		{
			if (mpfi[i])
			{
				if (!strcmp(mpfi[i]->f_fichero,path)) {
				   __fd = mpfi[i]->f_fd;
				   fd = mpfi[i]->f_fd;
				   __pfi = mpfi[i];
				   break;
				}
			}	
		}
   }

   if (__pfi &&	__fd == fd)
   {
	   r_s_my_pfi = __pfi;
	   if (bbdd_tiempo_rotate)
	   {	   
	      sql_txt_respaldo_drop(r_s_my_pfi);
		  sql_txt_respaldo_crea(r_s_my_pfi);
	   }
	   else
	   {
		respaldo_cierra_ind(fd);
		respaldo_borra_ind(r_s_my_pfi);
		respaldo_abre_ind(r_s_my_pfi);
	   }
	   return (void *)r_s_my_pfi;
   }

   
	for (i = 0;i < nfi;i++)
	{
		if (mpfi[i] && mpfi[i]->f_fd == fd)
		{
			r_s_my_pfi = mpfi[i];
		   if (bbdd_tiempo_rotate)
		   {	   
			  sql_txt_respaldo_drop(r_s_my_pfi);
			  sql_txt_respaldo_crea(r_s_my_pfi);
		   }
		   else
		   {
			respaldo_cierra_ind(fd);
			respaldo_borra_ind(r_s_my_pfi);
			respaldo_abre_ind(r_s_my_pfi);
		   }
		   return (void *)r_s_my_pfi;
		}
	}

	if (path) {
		for (i = 0;i < nfi;i++)
		{
			if (mpfi[i] && mpfi[i]->f_fd == -1)
			{
				if (!strcmp(mpfi[i]->f_fichero,path)) {
					r_s_my_pfi = mpfi[i];
				   if (bbdd_tiempo_rotate)
				   {	   
					  sql_txt_respaldo_drop(r_s_my_pfi);
					  sql_txt_respaldo_crea(r_s_my_pfi);
				   }
				   else
				   {
					respaldo_cierra_ind(fd);
					respaldo_borra_ind(r_s_my_pfi);
					respaldo_abre_ind(r_s_my_pfi);
				   }
				   return (void *)r_s_my_pfi;
				}
			}
		}
	}
	
	return r_s_my_pfi;
}

#ifdef ANTIGUO_CODIGO__
void *respaldo_externo_ixx_init(int fd, char *path)
{
	int i;
	FI_DEF *r_s_my_pfi = NULL;    

   if (!con_respaldo_externo && !bbdd_tiempo_rotate)
   {
       return NULL;
   }

   if (__pfi &&	__fd == fd)
   {
	   r_s_my_pfi = __pfi;
	   if (bbdd_tiempo_rotate)
	   {	   
	      sql_txt_respaldo_drop(r_s_my_pfi);
		  sql_txt_respaldo_crea(r_s_my_pfi);
	   }
	   else
	   {
		respaldo_cierra_ind(fd);
		respaldo_borra_ind(r_s_my_pfi);
		respaldo_abre_ind(r_s_my_pfi);
	   }
	   return (void *)r_s_my_pfi;
   }

   
	for (i = 0;i < nfi;i++)
	{
		if (mpfi[i] && mpfi[i]->f_fd == fd)
		{
			r_s_my_pfi = mpfi[i];
		   if (bbdd_tiempo_rotate)
		   {	   
			  sql_txt_respaldo_drop(r_s_my_pfi);
			  sql_txt_respaldo_crea(r_s_my_pfi);
		   }
		   else
		   {
			respaldo_cierra_ind(fd);
			respaldo_borra_ind(r_s_my_pfi);
			respaldo_abre_ind(r_s_my_pfi);
		   }
		   return (void *)r_s_my_pfi;
		}
	}
	
	return r_s_my_pfi;
}
#endif

int respaldo_externo_ixx_graba(void *mypfi,char *buffer)
{
	char *mybuffer;
	FI_DEF *pfi = (FI_DEF *)mypfi;

	mybuffer = pfi->f_buf;
	pfi->f_buf = buffer;

    if (bbdd_tiempo_rotate)
    {	   
		sql_txt_respaldo_nuevo(pfi);
	}
	else
	{
		respaldo_graba_nuevo(pfi);
	}

	pfi->f_buf = mybuffer;
	return 1;
}


#define C_HOLA    0x1000 // Inicio conexion al nodo
#define C_ADIOS   0x2000 // Fin conexion al nodo
#define C_CHAT    0x3000 // Mensaje de chat
#define C_DATA    0x4000 // Intercambio de datos entre cliente/operador
#define C_ESTADO  0x5000 // Cambio de estado del operador o cliente (ocupado etc)
#define C_PASAR   0x6000 // Pasar a otro operador
#define C_INFO    0x7000 // Pedir informacion al nodo
#define C_ACABADO 0x8000 // Acabado con el cliente/operador
#define C_RES9    0x9000
#define C_RESA    0xA000
#define C_RESB    0xB000
#define C_RESC    0xC000
#define C_RESD    0xD000
#define C_MULTI   0xE000
#define C_RESF    0xF000

int comunicacion_dstray(int idx)
{
	char tmp[512];
	char tmp2[128];
	char usr[128];
	char destino[20][128];
	int ndestinos = 0,i,n,j;
	int sal = 0;
	Paquete paquete;
	int modo = atoi(v_memo(idx++)->alfabetica);

	paquete.c.id = DSWEBCALLID;
	if (modo == 0)
	{
		paquete.c.command = C_CHAT;
	}
	else
		return -10;
	strcpy(usr,v_memo(idx++)->alfabetica);
	if (modo == 0)
	{
		n = atoi(v_memo(idx++)->alfabetica);
		paquete.buffer[0] = 0;
		for (i = 0;i < n;i++)
		{
			strcat(paquete.buffer,v_memo(idx++)->alfabetica);			
		}
		paquete.c.command |= (strlen(paquete.buffer)+1);
	}
	if (memcmp(usr,"IP:",3))
	{
		char buf[20000];
		int j;
		char *p,*p1,*user;
		buf[0] = 0;
		control_de_usuarios(1,buf);
		n = strlen(buf);
		j = 0;
		for (i = 0;i < n;i++)
		{
			if(buf[i] == ';')
			{			
				/*int subnumero = 0;*/
				user = NULL;


				/*cpu = -1;*/

				memcpy(tmp,buf+j,i-j);
				tmp[i-j] = 0;
				p = tmp;

				while(*p && *p != ' ') p++;
				if (*p == ' ') {*p = 0;p++;}			

				/* tmp = PID */


				while(*p && *p != '-') p++;
				if (*p == '-')
				{
				   p++;
				   p1 = p;
				   while(*p && *p != ' ') p++;
				   if (*p == ' ')
					 {
					 *p = 0;
					 p++;
					 /*subnumero = ::atoi(p1);*/
					 }
				}

				while(*p && *p != '\'') p++;
				if (*p == '\'') 
				{
					p++;
					p1 = p;
					while(*p && *p != '\'') p++;
					if (*p == '\'') {*p = 0;p++;}
					while(*p == ' ') p++;
				}

				/* p1 = usuario */
				if (!strcmp(p1,usr))
				{
					user = p1;
				}

				p1 = p;
				while(*p && *p != ' ') p++;
				if (*p == ' ') {*p = 0;p++;}

				/* p1 = localizacion */
				if (user)
				{
					if (!strcmp(p1,"LOCAL"))
					{
						strcpy(tmp2,"localhost");
					}
					else
					{
						strcpy(tmp2,p1);
					}
					for (j = 0;j < ndestinos;j++)
					{
						if (!strcmp(destino[j],tmp2))
							break;
					}
					if (j >= ndestinos)
					{
						strcpy(destino[ndestinos],tmp2);
						ndestinos++;
					}
				}

				p1 = p;
				while(*p && *p != ' ') p++;
				if (*p == ' ') {*p = 0;p++;}

				/* p1 = fecha */

				p1 = p;
  				while(*p && *p != ' ') p++;
				if (*p == ' ') {*p = 0;p++;}

				/* p1 = Hora */

				p1 = p;
  				while(*p && *p != ' ') p++;
				if (*p == ' ') {*p = 0;p++;}

				/* p1 = Rtme */

				p1 = p;
  				while(*p && *p != ' ') p++;
				if (*p == ' ') {*p = 0;p++;}

				/* p1 = ejecutable */

				/* saltar fecha hora opcion */
				p1 = p;			
  				while(*p && *p != ' ') p++;
				if (*p == ' ') {*p = 0;p++;}

				p1 = p;
				/*
  				while(*p && *p != ' ') p++;
				if (*p == ' ') {*p = 0;p++;}

				p1 = p;
				*/

				/* p1 = Opcion */

				j = i+1;
				/*iItem++;*/
			}
		}

	}
	else
	{
		strcpy(destino[0],usr+3);
		ndestinos = 1;
	}
	for (i = 0;i < ndestinos;i++)
	{
	  if (dstray_conexion(destino[i],&paquete) >= 0)
		  sal++;
	}
	return sal;
}
