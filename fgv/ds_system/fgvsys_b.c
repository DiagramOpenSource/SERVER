
/**********************************************************************/
/**********************************************************************/
/*                    DIAGRAM9                                  */
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

extern char rw_version[256];

extern int fecha_protegida;
extern char *numero_serie;

extern int rentra_atrib;
extern int rentra_sal_atrib;
extern int rentra_cc;
extern int rentra_cf;
extern int rentra_sal_cc;
extern int rentra_sal_cf;
extern int rentra_offqp;

extern int id_g_control;
extern int id_g_campo;


extern int especial_tecla_linea;

extern int fgv_ver_euro;

extern long trata_memo();
extern char *cambia_path_fich();

extern long da_registo_actual();

extern long que_registro();

extern double ld_doble();

extern int   mimodo_debug;

extern char *logger_salida;

extern int modo_de_relacion;

extern char empresa[MAXDEFECTO];
extern char proceso[81];
extern char titulo[81];
extern char dirbin[51];
extern char dirdef[51];
extern char dirpan[51];
extern char dirfic[51];
extern char nomfdir[51];
extern char dirbass[51];
extern char dirbase[51];
extern char dirficb[51];
extern char nomapli[25];
extern char modulo[9];
extern char apli_corre[25];
extern int apli_aborto; /* flag para abortar la aplicacion (QUIT) */

extern int nds_entrada_automatica;


extern FILE *_imp;
extern char *aginfo;
extern int LEERTERMINAL;
extern char nmen0[551];

extern char *rw_usuario;

extern int user_who;
extern long posi_user;
extern char rw_user[11];
extern char *quita_path();

extern void pinta_en_logger();

static int clave_de_consulta = 0;

extern int pasa_a_int(double);

extern int agisies;
extern int agivez;
extern int agitei;
extern int teiaviso;
extern int magi;
extern int vesylee;

extern int data_fecha_conmutar;

extern int es_la_doble_;
extern int va_con_doble_;

int requiere_reposicionar = 0;
int qj_forzado = -1;
extern int modo_desatendido;
extern char bufpan[PANBUFSIZE];

static int campo_entrado = 0;

/* ********* Clave de HELP ********** */

extern char clave_help[];
extern char referencia_help[128];

/**************************************/

int e_contador = 0; /* control de contadores */

                    /* =========================================================================
                    
*/

extern int defectoesc;

static int nnivel_p = -1;

static char *nivel_p[200] = {NULL,NULL,};
static int nivel_np[200] = {0,0,};
static int nivel_ep[200] = {0,0,};

extern char def_erp_actual[25];
extern int campo_erp_actual;
extern int ipfi_erp_actual;


#ifndef DSMINIBASICO
int recurre_pantalla(int modo)
{
	if (!modo)
	{
		nnivel_p++;
		nivel_p[nnivel_p] = NULL;
		nivel_np[nnivel_p] = 0;
		nivel_ep[nnivel_p] = 0;
		return nnivel_p;
	}
	if (nnivel_p >= 0)
	{
		if (nivel_p[nnivel_p])
		{
			free(nivel_p[nnivel_p]);		    
		    nivel_p[nnivel_p] = NULL;		   
		}
		nivel_np[nnivel_p] = 0;
		nivel_ep[nnivel_p] = 0;
		nnivel_p--;
		if (nnivel_p >= 0 && nivel_p[nnivel_p])
		{
		   int parametro;
		   char *puntero;
		   parametro = nivel_np[nnivel_p];
		   puntero = nivel_p[nnivel_p];
           if (parametro > 100 && puntero)
           {
              char *arrai[100];
              int ppi;
              for (ppi = 0;ppi  < (parametro - 100);ppi++)
              {
                 arrai[ppi] = puntero;
                 puntero += (strlen(puntero)+1);
              }
			  if (DespachadorExterno)
			  {
				(*DespachadorExterno)(DESPACHO_PANTALLAS,parametro,(char *)arrai);
			  }              
           }
		   else if (DespachadorExterno && (puntero || parametro))
		   {
				(*DespachadorExterno)(DESPACHO_PANTALLAS,parametro,puntero);   
		   }
		   if (DespachadorExterno)
		   {
			    (*DespachadorExterno)((int)DESPACHO_QUEPANTALLA,(int)nivel_ep[nnivel_p],"");
		   }
		}
	}
	return nnivel_p;
}

int DespachoPantalla(int despacho,int parametro,char *puntero)
{
   if (nnivel_p >= 0 && despacho == DESPACHO_PANTALLAS)
   {
	   char *pp = NULL;	   
	   if (nivel_p[nnivel_p])
	   {
		  free(nivel_p[nnivel_p]);
	   }
	   if (parametro > 100 && puntero)
	   {		  
		  int i,ni = parametro - 100;
		  char **arrai = (char **)puntero;
		  char tmp[5000];     
		  int len;
		  len = 0;      
		  for (i = 0;i < ni;i++)
		  {
			 strcpy(tmp+len,arrai[i]);
			 len += (strlen(arrai[i])+1);
		  }
		  pp = malloc(len);
		  memcpy(pp,tmp,len);
	   }
	   else if (puntero)
	   {
		  pp = malloc(strlen(puntero)+1);
		  strcpy(pp,puntero);
	   }	   
	   nivel_p[nnivel_p] = pp;
	   nivel_np[nnivel_p] = parametro;
   }

   if (DespachadorExterno)
   {
		(*DespachadorExterno)(despacho,parametro,puntero);   
   }                 
   return nnivel_p;
}

int DespachoQuePantalla(int despacho,int parametro,char *puntero)
{
   if (nnivel_p >= 0)
   {
		nivel_ep[nnivel_p] = parametro;
   }
   if (DespachadorExterno)
   {
		(*DespachadorExterno)(despacho,parametro,puntero);   
   }                 
   return nnivel_p;
}


/* ============================================================================
*/

/* ---------------------------------------------------------------------------
*  clave_rel
*    descripcion: rellena la clave primaria del fichero relacionado en
*       la relacion numero 'r' del fichero 'ipfi'
*    devuelve: -2 si la clave anterior era la misma ,
*              -1 si error al definir la relacion ,
*              sino el indice de 'mpfi' donde esta el fichero relacionado.
* ---------------------------------------------------------------------------
*/
int clave_rel(r,fr,que,faclin,ipfi,npan,alta,encl)
int r;
int fr;
int que;
int faclin,ipfi,npan,alta,encl;
{
   register int i,j,k,p;
   int m,mirada = 0;
   int salta = 0;
   char tmp[81];
   
   if (fr >= nfi || v_mpfi(fr)->f_nclaves <= 0)
   {		  
      return(-1);		
   }
   
   if (v_mpfi(ipfi)->f_nrelaci <= r)
   {
      sprintf(tmp,__Texto[256],quita_path(v_mpfi(ipfi)->f_PathDef),quita_path(v_mpfi(fr)->f_PathDef));
      aviso(0,tmp);
      return(-1);
   }
   
   for (j=0;j<v_mpfi(fr)->f_clave[0].k_ncampos;j++) {
      k = v_mpfi(fr)->f_clave[0].k_campo[j];  /* campo en clave 1.ia */
      if (!salta)
      {
         i = v_mpfi(ipfi)->f_relaci[r].r_campo[j]; /* campo en relacion */
         if (!i)
            salta = 1;
      }
      
      if (que <= 0) {
         if (que == -2) p = (-i); else p = (i - 1); /* no hace tras entra */
         if (que == 0 && memoplin > 2) ld_campo(k-1,v_mpfi(fr),v_memo(5001)->alfabetica);
         
         if (salta)
         {
            defecto_csficha(v_mpfi(fr),k-1,k);
            m = 0;
         }
         else
            if ((m = _copia_cam(fr,k-1,ipfi,p)) < 0)
               return(-1); /* error al definir  la relacion */
            
            if (que == 0) {
               m = (m == 0) ? 1 : 0;
               if (tras_entra(-1,k,ALTA,1,m,"",fr,faclin) < 0)
                  return(-1);
               m = (m == 0) ? 1 : 0;
            }
      }
      else {
         if (!salta)
         {
            if (memoplin > 2) ld_campo(i-1,v_mpfi(ipfi),v_memo(5001)->alfabetica);
            if ((m = _copia_cam(ipfi,i-1,fr,k-1)) < 0)
               return(-1); /* error al definir  la relacion */
            
            if (!v_mpfi(ipfi)->f_campos[i-1].c_mante.c_visual) /* bug fixed */
               pinta_campo(ipfi,i,faclin,0);
            if (que == 2) {
               f_n(fr,RLEIDA); /* permitir un tras_entra correcto */
               m = (m == 0) ? 1 : 0;
               if (tras_entra(npan,i,alta,encl,m,"",ipfi,faclin) < 0)
                  return(-1); /* abandonar */
               m = (m == 0) ? 1 : 0;
               f_s(fr,RLEIDA);
            }
         }
         else
            m = 0;
      }
      mirada+= m; /* cuenta cuantos campos de la clave son = en el fichero */
   }
   
   if (mirada == j)
      return(-2);
   return(fr);
}

int recoge_codigo_empresa(pfi,qo)
FI_DEF *pfi;
char *qo;
{
   ld_campo(0,pfi,qo);
   if (va_con_doble_ && es_la_doble_)
			{
      strcat(qo,"!");
			}
   return(0);
}

int graba_codigo_empresa(pfi,parametros)
FI_DEF *pfi;
char *parametros;
{
		 char tmp[260];
         int l;
         strcpy(tmp,parametros);
         if (tmp[0] && va_con_doble_ && es_la_doble_)
         {
            l = strlen(tmp) - 1;
            if (tmp[l] == '!')
               tmp[l] = 0;
         }
         st_campo(0,pfi,tmp);
         return(0);
}

/*
*  lee1_rel
*
*
*
*
*
*
*
*/

static int lee1_rel(j,fr,modo,que,faclin,ipfi,npan,alta,encl,campo)
int j;         /* relacion del fichero 0,.. */
int fr;
int modo;
int que;
int faclin,ipfi,   /* fichero que contiene la relacion */
npan,alta,encl,campo;
{
   register int r = 0;
   
   iserrno = 0;
   if (fr < 0 || v_mpfi(fr)->f_nclaves <= 0) {          /* -1 error al definir relaciones */
      iserrno = 1;
      return(-1);
   }
   else {
      if (que == 0 || que == 5) {
         if (clave_rel(j,fr,0,faclin,ipfi,npan,alta,encl) == -1) {
            /* rellena clave de relacion */
            iserrno = 1;
            return(-1);
         }
         r = leer_reg(fr,ISEQUAL,modo,1);
						}
      else
         if (que == 1 || que == -1) { /* defecto ultima clave del fichero relacionado */
            r = leer_reg(fr,((que == 1) ? ISFIRST : ISLAST),modo,1);
            if (r == 0)
            {
               clave_rel(j,fr,1,faclin,ipfi,npan,alta,encl);
            }
            r = 0;
         }
         else
            if (que == 2) {
               /* se ha pulsado ayuda y se hace una consulta */
               clave_rel(j,fr,-1,faclin,ipfi,npan,alta,encl);
               leer_reg(fr,ISEQUAL,0,40);
			   if (eqj == 1 || eqj == 2)
			   {
				   r = consul_clave(fr,1,-1,0,0,0,ipfi,campo,NULL);
				   /** 9.10 **/
			   }
			   else
			   {
				   if (v_mpfi(ipfi)->f_relaci[j].r_leer == 0) r = 1;else r = 2;
				   r = alta_rel(fr,r);
			   }
               if (r == 0) {
               /* si la consulta ha sido validada pone la clave del relacionado
                  en la ficha principal */
                  /* en clave_rel se hace hace el entra_rel a 0 por medio
                  del tras_entra si todo va bien */
                  f_s(fr,RELECION);
                  f_s(fr,RENBUF);
                  if (clave_rel(j,fr,2,faclin,ipfi,npan,alta,encl) == -1)
                     r = -1;
                  f_n(fr,RELECION);
                  f_n(fr,RENBUF);
               }
               cambia_ind(v_mpfi(fr),1);
            }
            if (r != 0)
               return(-1);
   }
   return(0);
}

/*
* alta_rel
*    ejectuta la alta de fichas relacionadas
*    devuelve: -1 en caso de salida con esc o ctrl-c
*             o 0 si va bien
*   ESTA RUTINA ES POR DONDE SE CREAN LAS VENTANAS
*/
int alta_rel(ipfi,cont)
int ipfi;
int cont;
{
   int acabar = 0,r,e_cnt = 0,pini,pfin,leida;
   VENT *w;
   int cla,i = 0;
   char gcontrol[MAXDEFECTO];
   char ncontrol[MAXDEFECTO];
   int o_modo_rel = modo_de_relacion;
   
   if (v_mpfi(ipfi)->f_nclaves <= 0)
						return(-1);
   
   if (!hay_dat_permiso(v_mpfi(ipfi),-1))
						return(-1);
   
   
   /* modo_de_relacion = 1;*/
   modo_de_relacion = 0;
   
   pini = v_mpfi(ipfi)->f_pantalla[0].p_posref;
   pfin = v_mpfi(ipfi)->f_pantalla[0].p_posfin + pini - 101;
   
   w = w_ini(pini,pfin);
   w_salva(w);
   
   
   recurre_pantalla(0);
   if (DespachadorExterno)
   {
		(*DespachadorExterno)((int)DESPACHO_SOBRE_PANTALLA,(int)0,NULL);   
   }                 
   
   
#ifdef FGVWIN
   /* crear ventana independiente */
   
#endif
   
   if (memoplin > 2) memcpy(gcontrol,v_memo(5002)->alfabetica,MAXDEFECTO); /* Control */
   if (memoplin > 3) memcpy(ncontrol,v_memo(5003)->alfabetica,MAXDEFECTO); /* FEntrada */
   

   if (pinppan(ipfi,0) == -777)
   {	   
       return consul_clave(ipfi,1,0,0,0,0,ipfi,cont-1,NULL);
   }

   prin_pan(ipfi,0);   /* pinta datos de la pantalla 0 */
   
   if (cont == 0) {
						/* esto es para coger las relaciones a contadores o similares */
						/* en caso de que el campo relacinado este vacio .... no se   */
						/* tienen en cuenta casos de relacion a contador con varios campos */
						for (i = 0;i < v_mpfi(ipfi)->f_clave[0].k_ncampos;i++) {
                     cla = v_mpfi(ipfi)->f_clave[0].k_campo[i];
                     if (!mira_sivacio(v_mpfi(ipfi),cla-1)) {
                        if ((r = antes_entra(0,cla,ALTA,1,ipfi,0,0)) == 2) {
                           acabar = -1; /* exclusivo de contadores */
                           break;
                        }
                        if (e_contador) { /* antencion contador !! */
                           if (!mira_sivacio(v_mpfi(ipfi),cla-1) || !r) {
                              acabar = -1;
                              break;
                           }
                           e_cnt = 1;
                        }
                     }
                  }
                  
                  
                  
                  if (!(hay_dat_permiso(v_mpfi(ipfi),-1) & 2))
                  {
                     aviso(0,__Texto[11]);
                     acabar = -1;
                  }
                  
                  if (!acabar && !e_cnt && confirmar(0,__Texto[12],(defectoesc ? __Texto[447] : __Texto[448])) < 0)
                     acabar = -1;
                  
                  if (acabar || grabar_reg(ipfi,3,0) < 0)
                     acabar = -1;
                  else {
                     if (e_cnt)
                        acabar = 10;
                     if (entra_datos(ALTA,ipfi,0) < 0) {
                        acabar = -1;
                        borrar_reg(ipfi,0,0);
                     }
                     else {
                        grabar_reg(ipfi,0,0);
                        ejecuta_otro(ipfi,30,O_NOR,0,0);
                        if (va_con_doble_ && !es_la_doble_)
                        {
                           graba_el_doble(ipfi);
                        }
                     }
                     libera_ind(v_mpfi(ipfi));
                  }
   }
   else {
						if (v_mpfirel(ipfi)->clave < -1) {
                     if ((leida = f_v(ipfi,RLEIDA)) != -1)
                        f_n(ipfi,RLEIDA);
                     if ( cont > 1 )
                        sprintf(v_memo(5004)->alfabetica,"%d",5); /* a pelo modo de entrada de seleccion */
                     else
                        sprintf(v_memo(5004)->alfabetica,"%d",2); /* a pelo modo de modificacion */
                     p_ejecuta(v_mpfirel(ipfi)->clave + 1,O_NOR,0,5);
                     if (leida != -1)
                        f_s(ipfi,RLEIDA);
                     acabar = atoi(v_memo(5004)->alfabetica);
                  }
                  else
                     acabar = consul_ficha(ipfi,1,cont - 1);
   }
   
   if (acabar < 0) {
						defecto_ficha(v_mpfi(ipfi));
                  f_n(ipfi,RENBUF);
   }
   
   if (DespachadorExterno)
   {
						(*DespachadorExterno)((int)DESPACHO_REPON_PANTALLA,(int)0,NULL);   
   }
   recurre_pantalla(1);
   
   w_pinta(w);
   w_fin(w);
   des_activa(ipfi);
   if (memoplin > 2) memcpy(v_memo(5002)->alfabetica,gcontrol,MAXDEFECTO);
   if (memoplin > 3) memcpy(v_memo(5003)->alfabetica,ncontrol,MAXDEFECTO);
   modo_de_relacion = o_modo_rel;
   
   return(acabar);
}


int pintar_otrocampo(ipfi,i,npan,faclin)
int ipfi,i,npan,faclin;
{
   int visual;
   
   if ((v_mpfi(ipfi)->f_campos[i].c_mante.c_actual / 1000) == npan) 
   {
      /* controlar que la pantalla ipfi este pintada */
      if ( memoplin < 3 || ((!memcmp(
         v_memo(5002)->alfabetica,
         quita_path(v_mpfi(ipfi)->f_fichero),
         strlen(quita_path(v_mpfi(ipfi)->f_fichero))
         )
         && npan == (v_memo(5002)->alfabetica[10] - '0')))
         ) {
         visual = v_mpfi(ipfi)->f_campos[i].c_mante.c_visual;
         if (visual == 0)
            pinta_campo(ipfi,i+1,faclin,0);
      }
   }
   if (v_mpfi(ipfi)->f_campos[i].c_key == 1)
      return(CIERTO);
   else
      return(FALSO);
}

/*
* printp1rel
*    pintar campo a pintar
*    esta pantalla del fichero 'ipfi'.
*/
int printp1rel(ipfi,ipfr,npan,r)
int ipfi;   /* puntero al fichero */
int ipfr;         /* numero de la relacion 0,... */
int npan;      /* numero de  campo  0,... */
int r;
{
   register int k,i,l;
   
   for (k=0;k<v_mpfirel(ipfi)->nrelpin;k++) {
      if ( ( l = v_mpfirel(ipfi)->relpin[k].nfirel ) == ipfr &&
         v_mpfirel(ipfi)->relpin[k].qp  > 0 &&
         v_mpfirel(ipfi)->relpin[k].numrela == r) {
         i = v_mpfirel(ipfi)->relpin[k].campo;
         if ((v_mpfi(ipfi)->f_campos[i].c_mante.c_actual / 1000) == npan) 
         {
            cursor(v_mpfirel(ipfi)->relpin[k].qp);
            printusi_i(v_mpfi(l),v_mpfirel(ipfi)->relpin[k].nficampo+1);
         }
      }
			}
   
   return(0);
}



/* TODO LO QUE TIENE QUE VER CON LAS RELACIONES PASA POR AQUI:
* entra_rel
*   Cosas a hacer tras entrar el dato 'ipfi,numpan,numcam,alta,enclave,modif'
*   si tiene alguna relacion:
*      - leer relacion,
*      - si hay error de indexado o al definir la relacion: avisa,
*      - si no existe: alta,
*      - si se da de alta la relacion o se habia modificado su clave y su
*        ficha existia: - pinta campos relacionados,
*                       - pone campos con relacion de defecto.
*      - activa la consulta relacionada
*/

static int no_mensaje = 0;

int entra_rel(numpan,numcam,altaa,enclave,modif,ipfi,que,faclin)
/* fichero de mantenimiento */
int numpan;   /* pantalla en la que esta el campo 0,... */
int numcam;   /* campo entrado 1,... */
int altaa;     /* ALTA o MODIF */
int enclave;  /* se esta entrando una clave */
int modif;    /* 0 si no ha sido modificado */
int ipfi;
int que;
int faclin;
{
		 register int i,j,f,r,alta;
         int salida = 0,ok;
         char *p_tmp;
         int si;
         char tmp[81];
		 int abierta = -1;
         

         /*
         que == 0   trato normal de la relacion tras validar un campo
         que == 1   trato de la relacion antes de la entrada del campo
         -> en caso de que el campo este "vacio" le pone el
         ultimo codigo del relacionado
         que == 2   consulta relacionada
         que == 4   antes de entrar el campo pintar el mensaje de consulta-alta
         si se usa que == 1 no hace falta que == 4
         que == 5   comprobacion al acabar la ficha de si se han entrado las
         relaciones obligatorias
         que == 6   al entrar en modificacion da las relaciones como validadas
         que == 7   limpia las marcas de las relaciones obligatorias
         */
         
         /* un control */
         if (que == 6 && altaa != MODIF) {
            rw_error(27);
            /* printf("\n\r No ha lugar h000 \n\r");
            getchar(); */
            return(-1);
         }
         
         /* si es consulta recursiva no aceptar por defecto */
         if (que == 2) salida = -2;
         
         /* las relaciones se tratan de una en una no se puede en recursion acceder
         a la misma relacion ---> flag RLEIDA  */
         
         if (!no_mensaje)
            borra_mensaje_cabecera(1); /* borrar mensaje */
         
         
         if (que < 5 && numcam == 0) return(0);
         
         
         for (j=0;j<v_mpfirel(ipfi)->nrel;j++) {
			if (v_mpfirel(ipfi)->numrela[j] < 0) continue; 
            if (v_mpfirel(ipfi)->camminu[j] < 0) continue; /* las lineas no se */
            ok = 0;                                      /* tratan aqui */
            si = 0;
            i = v_mpfirel(ipfi)->numrela[j];
            f = v_mpfirel(ipfi)->relacis[j];

			if (f >= 0 && f < nfi && (!v_mpfi(f) || !v_mpfi(f)->f_titulo))
				continue;			
            
            if (que == 6) { /* marcar como listas */
               if (v_mpfi(ipfi)->f_relaci[i].r_obliga > 0) {
                  v_mpfi(ipfi)->f_relaci[i].r_obliga =
                     (-v_mpfi(ipfi)->f_relaci[i].r_obliga);
               }
               continue;
            }
            if (que == 7) { /* limpiar marcas */
               if (v_mpfi(ipfi)->f_relaci[i].r_obliga < 0) {
                  v_mpfi(ipfi)->f_relaci[i].r_obliga =
                     (-v_mpfi(ipfi)->f_relaci[i].r_obliga);
               }
               continue;
            }
            
            alta = altaa;
            /* campos relativos segun entrada por pantalla o absoluto
            si no se entra                                         */
            if (v_mpfirel(ipfi)->camminu[j] == (numcam - 1)) si = 1;
            if (v_mpfirel(ipfi)->camrela[j] == (numcam - 1)) si = 2;
            
            if (si > 0) {
               /* caso de que este abierto el fichero (claves de varios campos)*/
               if (que == 0) {
                  if (si == 1) {
                     if (modif) { /* ATENCION RELACION TOCADA */
                        if (v_mpfi(ipfi)->f_relaci[i].r_obliga < 0)
                           v_mpfi(ipfi)->f_relaci[i].r_obliga =
                           (-v_mpfi(ipfi)->f_relaci[i].r_obliga);
                     }
                     alta = CONSULTA; /* esto evita que se entre la relacion */
                  }
                  else
                     if (alta == MODIF) {
                        /* controlo si algun campo de la relacion ha sido modificado */
                        if (v_mpfi(ipfi)->f_relaci[i].r_obliga <= 0 && !modif)
                           alta = CONSULTA;
                        else
                           modif = 1;
                     }
               }
               if (que < 3) si = 1;
            }
            else
               if (!que && modif && f < 99 && (alta == MODIF || alta == ALTA) ) {
                  /* la relacion esta marcado como correcta pero el campo */
                  /* como ha sido tocado comprobar si pertenece a la relacion */
                  if (v_mpfi(f)->f_nclaves > 0)
                     for (r = 0;r < v_mpfi(f)->f_clave[0].k_ncampos;r++) {
                        if (!v_mpfi(ipfi)->f_relaci[i].r_campo[r])
                        {
                           break;
                        }
                        if (numcam == v_mpfi(ipfi)->f_relaci[i].r_campo[r]) 
                        {
                           /* PERTENECE A LA RELACION Y HA SIDO TOCADO */
                           if (f_v(f,RELECION)) /* anulacion */
                              f_n(f,RENBUF); /* el fichero ya no esta leido */
                           /* y la relacion obligatoria esta pendiente */
                           if (v_mpfi(ipfi)->f_relaci[i].r_obliga < 0) {
                              v_mpfi(ipfi)->f_relaci[i].r_obliga =
                                 (-v_mpfi(ipfi)->f_relaci[i].r_obliga);
                           }
                           break;
                        }
                     }
               }
               
               if ( (que == 4 || que == 1) && alta != DIFERIDO && f < 100 &&
                  modif != -3 && si > 0)
			   {
                  /* modif == -3 no pintar mensaje relacion solo para que == 4 */
                  if (!no_mensaje)
                  {
                     if (v_mpfi(ipfi)->f_relaci[i].r_leer == 0)
                        mensaje_en_cabecera(__Texto[190],v_mpfi(f)->f_titulo,1);
                     else
                        mensaje_en_cabecera(__Texto[191],v_mpfi(f)->f_titulo,1);
                  }
                  if (que == 4) si = 0;
               }
               
               if (si == 1 || que == 5) 
			   {
                  if (que == 5 && v_mpfi(ipfi)->f_relaci[i].r_obliga <= 0) 
				  {
                     v_mpfi(ipfi)->f_relaci[i].r_obliga =
                        (-v_mpfi(ipfi)->f_relaci[i].r_obliga);
                     continue;
                  }
                  
                  if (f > /*99*/9999) 
				  {    /*relacion a ficha unica*/
                     
                     if ((que == 1 && (alta != ALTA || f < 10000)) ||
                        que == 2 || que == 5 || (que == 0 &&  f > 9999))
                        continue;
                     
                     if ((f / 10000) == 1)
                        e_contador = 1; /* puesto a cero en el antes_entra */
                     if ((salida = trata_1ficha(f%100,
                        v_mpfi(ipfi)->f_relaci[i].r_fichero+21,f/100,faclin)) == 0 &&
                        f / 10000 == 1) 
					 {
                        pinta_campo(ipfi,numcam,faclin,0);salida = 1;
                        break;
                     }
                     if (salida == 2)
                        break;
                     else
                        salida = 0;
                     continue;
                  }
                  
                  if (f_v(f,RLEIDA) == 0 ) continue; /* impedir circulos viciosos */
                  
                  if (alta == DIFERIDO && v_mpfi(ipfi)->f_relaci[i].r_leer != 0) {
                     deferel_rel(ipfi,f,numpan,faclin,alta,i);
                     printp1rel(ipfi,f,numpan,i);
                     continue;
                  }
                  
                  if (que == 0 && alta == CONSULTA)
                  {
                     /* si es consulta solo cerrar ficheros                         */
                     /* atencion el modo consulta en entrada de datos normal        */
                     /* como no tiene sentido se usa para cerrar ficheros tras un   */
                     /* aborto , pero en entrada de clave el modo consulta es como  */
                     /* otro ya que si en la consulta ven que les falta algo lo     */
                     /* pueden dar de alta y ademas se deben controlar la existencia*/
                     /* obligatoria                                                 */
                     if (f_v(f,RNOCERRAR) == -1) cierra1_rel(f);
                     continue;
                  }
                  p_tmp = "@";
                  if (que == 1) 
				  {
                     p_tmp = v_mpfi(ipfi)->f_campos[numcam-1].c_mante.c_defecto;
                     if (
                        mira_sivacio(v_mpfi(ipfi),numcam-1) < 0 ||
                        mira_sirel(ipfi,numcam-1) < 0 ||
                        (*p_tmp != '@' && *p_tmp != '#')
                        )
                        continue;
                  }
                  if (v_mpfi(ipfi)->f_relaci[i].r_obliga > 0) {
                     if (f_v(f,RELECION)) /* anulacion */
                        f_n(f,RENBUF); /* al haber sido tocado y ser obligatoria hay que leerla por obligacion */
                  }
                  r = 0;
                  /* si ya esta leida no la vuelve a leer salvo consulta o defecto */
				  abierta = v_mpfi(f)->f_fd;
                  if ( (que == 0 && f_v(f,RENBUF) == 0 &&
                     (r = clave_rel(i,f,-2,0,ipfi,0,0,0)) == -2) ||
                     (abierta > -1 || abre1_rel(f) == 0)) {
                     f_s(f,RLEIDA);
                     if (r != -2) {
                        r = 0;
                        defecto_ficha(v_mpfi(f));
						hay_dat_permiso(v_mpfi(f),-3);
                        f_n(f,RENBUF);
                        
                        /* la r se usa para el bloqueo ahora siempre a 0    */
                        /* si fuese != 0 no se cerraria el fich relacionado */
                        
                        /* En lee1_rel se procesan los modos de consulta (2) y
                        defecto (1) */
                        
                        if ( (salida = lee1_rel(i,f,r,((*p_tmp == '@') ? que : -que),faclin,ipfi,
                           numpan,alta,enclave,numcam-1)) < 0 && (que == 0 || que == 5)) {
                           if (v_mpfi(ipfi)->f_relaci[i].r_leer == 0 &&
                              iserrno == ENOREC && alta != DIFERIDO) {
                              iserrno = 0;
                              salida = alta_rel(f,0);
                              if (salida == 10) {
                                 clave_rel(i,f,1,faclin,ipfi,numpan,alta,enclave);
                                 salida = 0;
                              }
                              if (!salida)
                                 modif = 1;
                              else
                                 ok = -1;
                           }
                           else 
						   {
                              if (alta != DIFERIDO &&
                                 v_mpfi(ipfi)->f_relaci[i].r_obliga != 0) 
							  {
                                 sprintf(tmp,__Texto[13]
                                    ,v_mpfi(f)->f_titulo,iserrno);
                                 Ar();
                                 aviso(0,tmp);
                                 Cr();
                              }
                              iserrno = 0;
                              ok = -1;
                           }
                        }
                        
                        /* para saber si modif al coger el campo de la consulta */
                        if (que == 2 && salida == 0) modif = 1;
                        
                        if (salida != 0 && que == 5) ok = -1;
                     }
                     else
                        r = 0;
                     
                     if (alta != CONSULTA && que != 1 &&
                        (modif != 0 || alta == ALTA)) 
					 {
                        if (salida == 0) {
                           f_s(f,RENBUF);  /* ya ha sido leida la ficha */
                        }
                        else {
                           f_n(f,RENBUF); /* la ficha no esta leida cuidado */
                        }
                        if (que == 0 || (salida == 0 && que != 2))
                        {
                           deferel_rel(ipfi,f,numpan,faclin,alta,i);
                           printp1rel(ipfi,f,numpan,i);
                        }
                     }
                     
                     if (que == 2 && salida) {
                        /* releer codigo inicial */
                        lee1_rel(i,f,0,0,faclin,ipfi,numpan,alta,enclave,numcam-1);
                        f_n(f,RENBUF);
                     }
                     
                     if ( abierta < 0 && f_v(f,RNOCERRAR) == -1 && r == 0 && (que == 0 ||
                        que == 5 || (que == 2 && salida == 0) ))
                        cierra1_rel(f); /* permiso de cierre */
                     
                     f_n(f,RLEIDA);
                     
                     /* solo se puede activar una relacion en estos casos */
                     
                     if (que == 2 || que == 1) { /* en validar consulta tambien control de relacion obligatoria */
                                                 /******
                                                 if (que == 2 && (alta == ALTA || alta == MODIF)) {
                                                 if (v_mpfi(ipfi)->f_relaci[i].r_obliga > 0) {
                                                 if (!salida)
                                                 v_mpfi(ipfi)->f_relaci[i].r_obliga =
                                                 (-v_mpfi(ipfi)->f_relaci[i].r_obliga);
                                                 }
                                                 }
                        ******/
                        /* en el caso de la consulta validada
                        la clave_rel que se llama desde lee1_rel
                        en modo consulta de relacion ya hace un
                        tras_entra de los campos de la clave con
                        el control de si han sido modificados,
                        es como hacer un que == 0 y un continue
                        ahora pero mejor ya que se recomprueba campo
                        a campo si ha sido cambiado durante la consulta */
                        break;
                     }
                     }
                     else
                        ok = -1;
                     if (que == 0 || que == 5) {
                        if (v_mpfi(ipfi)->f_relaci[i].r_obliga != 0 && ok == -1) {
                           salida = -1;
                           break;
                        }
                        else {
                           salida = 0;
                           if ((alta == ALTA || alta == MODIF) && que == 0
                              && v_mpfi(ipfi)->f_relaci[i].r_obliga > 0)
                              v_mpfi(ipfi)->f_relaci[i].r_obliga =
                              (-v_mpfi(ipfi)->f_relaci[i].r_obliga);
                        }
                     }
                }
        }
        return(salida);
}


/*
* tras_entra
*   Cosas a hacer tras entrar el dato 'ipfi,numpan,numcam,alta,enclave,modif':
*      - si tiene relacion: leerla, avisa si hay error, si no existe la da
*           de alta, y si la relacion cambia (se da de alta o se habia
*           modificado su clave y la ficha existia):
*                . pinta campos relacionados,
*                . pone campos con relacion de defecto.
*      - calculos
*      - ...
*/
int tras_entra(numpan,numcam,alta,enclave,modif,qd,ipfi,faclin)
/* fichero de mantenimiento */
int numpan;   /* pantalla en la que esta el campo 0,... */
int numcam;   /* campo entrado 1,... */
int alta;     /* ALTA o MODIF */
int enclave;  /* se esta entrando la clave en ALTA */
int modif;    /* 0 si no ha sido modificado */
char *qd;     /* valor por defecto si se entra, sino "" y modificado = 0 */
int ipfi;
int faclin;
{
   
   int salida = 0; /* ,altaa = alta; */
   
                   /*
                   if (modif == 0 && alta != ALTA) {
                   altaa = CONSULTA;
                   }
   */
   
   if (alta != CONSULTA)
      salida = actualiza_cam(ipfi,numcam,faclin,6);
   if (salida == 0)
      salida = entra_rel(numpan,numcam,alta,enclave,modif,ipfi,0,faclin);
   if (salida == 0 && alta != CONSULTA)
      salida = actualiza_cam(ipfi,numcam,faclin,0);
   
   return(salida);
}

/*
 *
 * antes_entra
 *     antes de entrar un dato en 'ipfi,numpan,numcam,alta' cosas a hacer :
 *     - mira y trata si tiene algun defecto de un campo del mismo fichero.
 *       ej.:  nombre y nombre comercial en clientes.
 *     - mira y trata si el campo no es visual o modificable.
 *     - ...
 *     devuelve: -1 si el campo debe saltarse
 *                0 sino.
 *
 */
int antes_entra(numpan,numcam,alta,enclave,ipfi,faclin,cr)
/* fichero de mantenimiento */
int numpan;   /* pantalla en la que esta el campo 0,... */
int numcam;   /* campo a entrar 1,... */
int alta;     /* ALTA o MODIF */
int enclave;  /* se esta entrando la clave en ALTA */
int ipfi;
int faclin;
int cr;
{
   int visual,modif = 0,modificado = 0,como = 0,qj;
   
   e_contador = 0; /* solo se debe controlar para casos de hacer un antes_entra */
   
   if (enclave == FALSO && v_mpfi(ipfi)->f_campos[numcam-1].c_key == 1)
      return(-1);
   /* mira y trata si tiene algun defecto de un campo del mismo fichero */
   if (memoplin > 2) ld_campo(numcam-1,v_mpfi(ipfi),v_memo(5001)->alfabetica);
   if (memoplin > 3) {
      visual = atoi(v_memo(5003)->alfabetica);
      visual = (visual / 100) * 100 + alta;
      sprintf(v_memo(5003)->alfabetica,"%d",visual);
      if (memoplin > 6) 
      {
         sprintf(v_memo(5006)->alfabetica,"%d",enclave); 
      }
			}
   if (alta == CONSULTA || actualiza_cam(ipfi,numcam,faclin,7) > -1) 
   {      
      /* mira y trata si el campo no es visual o modificable */
      visual = v_mpfi(ipfi)->f_campos[numcam-1].c_mante.c_visual; /* visualiz. */
      modif = v_mpfi(ipfi)->f_campos[numcam-1].c_mante.c_modif;
      /* modificable */
      if (memoplin > 3) {
         if (memcmp(v_memo(5002)->alfabetica+20,"si",2) == 0) {visual = 0;modif = 0;}
         if (memcmp(v_memo(5002)->alfabetica+20,"no",2) == 0) {modif = -1;}
         memset(v_memo(5002)->alfabetica+20,'\0',2);
      }
      
      if (visual != 0 || modif != 0) como = -3; /* no pintar mensaje relacion */
      
      if (alta == ALTA || enclave) {
         defecam_rel(ipfi,numcam-1,numpan,faclin);
         if ((modificado = entra_rel(numpan,numcam,alta,enclave,como,
            ipfi,1,faclin)) == 2) 
		 {
            return(2); /* abortado por contador o semejante */
         }
         if (modificado != 0) {
            modif = 1;
            modificado = 1; /* contador validado */
         }
         /* que == 1 /* ya queda el fichero abierto */
      }
      else /* pintar aviso de consulta relacionada */
         entra_rel(numpan,numcam,alta,enclave,como,ipfi,4,faclin);
      
      if (visual != 0 || modif != 0) 
	  {
         qj = -1;
         if (cr && !visual && modif)
         {
a_repeticion:
         /* modo igual a la entrada del campo */
         curon(1);
         pinta_campo(ipfi,numcam,faclin,1);
         qj = -1;
         do {
            qj = leecar();
            if (qj == FTAB || qj == DERECHA)
               qj = 3;
            else
               if (qj == BTAB || qj == IZQUIERDA)
                  qj = 2;
               else
               {
                  /* solo ocurre en lineas */
                  qj = que_qjes(qj,1+especial_tecla_linea);
               }
         } while(qj == -1);
         curon(0);
         pinta_campo(ipfi,numcam,faclin,0);
         if (memoplin > 4) sprintf(v_memo(5004)->alfabetica,"%d",qj);
         qj = -(qj + 2);
         }
         if (qj == -9 || qj == -3)
         {
            tras_entra(numpan,numcam,CONSULTA,enclave,0,"",ipfi,faclin);
            if (actualiza_cam(ipfi,numcam,faclin,11))
               goto a_repeticion;
         }
         else
         {
            if (tras_entra(numpan,numcam,alta,enclave,modificado,"",ipfi,faclin)< 0 && cr && modif && !visual)
               goto a_repeticion;
         }
         return(qj);
      }
      return(0);
			}
   return(-1);
}

int reabre_los_ficheros(char *dirant,int i)
{
int f,flag;
char tmp[256];

            
            for (f = 0;f < nfi;f++)
            {   
               if (!memcmp(v_mpfi(f)->f_fichero,dirant,i))
               {
                  flag = 0;
                  if (v_mpfi(f)->f_fd != -1)
                  {
                     cierra1_rel(f);
                     flag = 1;
                  }
                  strcpy(tmp,quita_path(v_mpfi(f)->f_fichero));
                  strcpy(v_mpfi(f)->f_fichero,dirfic);
                  strcat(v_mpfi(f)->f_fichero,tmp);
                  if (flag)
                  { 
                     if (abre1_rel(f) < 0)
                     {
                        aviso(0,__Texto[14]);
                     }
                  }
               }
            }
}


int cambiame_paths_especial()
{
		 int i,flag = 0;
         
         
         i = strlen(dirfic);
         if (i < 2) return(0);
         if (dirfic[i-2] == '!')
         {   
            dirfic[i-2] = '/';
            dirfic[i-1] = 0;
            i -= 3;
            flag = 2;
         }
         else
         {
            if (dirfic[i-2] >= '0' && dirfic[i-2] <= '9')
            {
               dirfic[i-1] = '!';
               dirfic[i] = '/';
               dirfic[i+1] = 0;
               i -= 2;
               flag = 3;
            }
            else
            {
               return(0);
            }
         }
         
         if (data_fecha_conmutar && data_fecha_conmutar != flag)
         {
            aviso(0,__Texto[15]);
         }
         
         if (flag == 2)
         {
            es_la_doble_ = 0;
            caracter_de_fecha = '.';
            if (empresa[0] == '/')
            {
               empresa[0] = '*' ;
               /*cabecera_v(rw_version,proceso,titulo,empresa);*/
			   cabecera(proceso,titulo,empresa);
            }
         }
         else
            if (flag == 3)
            {
               es_la_doble_ = 1;
               caracter_de_fecha = '/';
               if (empresa[0] == '*')
               {
                  empresa[0] = '/' ;
                  /*cabecera_v(rw_version,proceso,titulo,empresa);*/
				  cabecera(proceso,titulo,empresa);
               }
            }


            reabre_los_ficheros(dirfic,i);
            
            
            return(0);
}

/* ============================================================================
*/


int entra_campo(ipfi,numcam,numpan,altaa,faclin,qj,encl,cr,ccc,alta)
int ipfi;
int numcam,numpan,altaa,faclin,qj,encl,cr,ccc,alta;
{	 
	     char qi[51],qs[51],*qd = malloc(MAXDEFECTO);
         char *qo = malloc(MAXDEFECTO),*qini = malloc(MAXDEFECTO);
         char *pqi,*pqs,*pqd,*pqo;           /* usar al igualar punteros */
         int qp,qc,erroren = 0,modificado,fm,i;
         int tipoerror = 0,tmp_qj;
         int modif2 = 0,salvaa;
         VENT *ww;
         int esta_en_linea,sin_permiso;
         int o_modo_rel = modo_de_relacion;
         int repite_qj = -1;
         int flag_especial = 0;
		 int hflag = 0;
		 char _def_erp_actual[25];
		 int _campo_erp_actual = campo_erp_actual;
		 int _ipfi_erp_actual = ipfi_erp_actual;

		 memcpy(_def_erp_actual,def_erp_actual,sizeof(def_erp_actual));
		 strcpy(def_erp_actual,quita_path(v_mpfi(ipfi)->f_PathDef));
		 campo_erp_actual = numcam-1;
		 ipfi_erp_actual = ipfi;

         modo_de_relacion = 1;
         
         if (cr == -1)
         {
            cr = 0;
            flag_especial = 1;
         }
         else
            if (cr > 1)
            {
               repite_qj = cr - 2;
               cr = 0;
            }
            
            recurre_mensaje_cabecera(0);
            
            sin_permiso = 0;
            do {
               if (apli_aborto) {qj = 7;break;}
               esta_en_linea = 0;               
               
               sprintf(referencia_help,"%s.%03d",quita_path(v_mpfi(ipfi)->f_fichero),numcam-1);
               //if (mimodo_debug)
			   {
                  pprint(2540,referencia_help);
			   }
               
               if (!erroren)
               {
                  ld_campo(numcam-1,v_mpfi(ipfi),qini);
                  if ((qp = antes_entra(numpan,numcam,altaa,
                     encl,ipfi,faclin,cr)) < 0) 
				  {
                     if (cr && qp < -1) {
                        qj = (-qp) - 2;
                        break;
                     }
                     if (qj != 2)
					 {
						 qj = 0;
					 }
                     break;
                  }
               }
               else
               {
                  if (memoplin > 2) strcpy(v_memo(5001)->alfabetica,qini);
                  if (!v_mpfi(ipfi)->f_campos[numcam-1].c_mante.c_visual &&
                     !v_mpfi(ipfi)->f_campos[numcam-1].c_mante.c_modif)
                     entra_rel(numpan,numcam,altaa,encl,0,ipfi,4,faclin);
               }
               erroren = 0;
               if (qp != 2) 
               {  /* no se salta el campo */
                  qp = v_mpfi(ipfi)->f_campos[numcam-1].c_mante.c_qp;
                  if (faclin > 0) 
                  {
                     if (faclin%100 == 0) 
                     {
                        if ((v_mpfi(ipfi)->f_campos[numcam-1].c_mante.c_lineal & C_LINEAL) != 0) 
                        {
                           if (!fi_modo_lineas[ipfi])
                              qp += ( faclin - 100 ) ;
                           else
                              qp += ((( faclin - 100 )/100)*fi_modo_lineas[ipfi]);
                           esta_en_linea = 1;
                        }
                     }
                     else
                        qp = faclin;
                  }
                  
                  if (strlen(v_mpfi(ipfi)->f_campos[numcam-1].c_mante.c_mensaje) > 0) {
                     Ar();mensaje(0,v_mpfi(ipfi)->f_campos[numcam-1].c_mante.c_mensaje);Cr();
                     fm = 1;
                  }
                  else
                     fm = 0;
                  /* printf(clave_help,"%12.12s%8.8s%04d",modulo,quita_path(v_mpfi(ipfi)->f_fichero),numcam-1); */
                  /* clave_help se carga en ejecuta_ap */
                  if ((qc = v_mpfi(ipfi)->f_campos[numcam-1].c_qc) == -2)
                  { /* CASO CAMPO TEXTO */
                     i = atoi(v_mpfi(ipfi)->f_campos[numcam-1].c_minimo);
                     ww = w_ini(i,i);/* parece ridiculo */
                     w_salva(ww);
                     modificado = 0;
                     if (rentra_cc != rentra_cf)
                        Color(rentra_cc,rentra_cf);
                     atributo(rentra_atrib);
                     pprint(i,"T");
                     atributo(tolower(rentra_atrib));
                     cursor(i);
                     curon(2);
                     qj = que_qjes(leecar(),0);
                     curon(0);
                     if (qj == -1 || !qj) {
                        /* fic,pos,m,qpi,qpf,v,topelin,titulo,maxlon */
                        strcpy(qo,v_mpfi(ipfi)->f_campos[numcam-1].c_mante.c_defecto);
                        st_doble(numcam-1,v_mpfi(ipfi),
                           (double)trata_memo(cambia_path_fich(qo),
                           (long)ld_doble(numcam-1,v_mpfi(ipfi)),
                           0,i,
                           i + atoi(v_mpfi(ipfi)->f_campos[numcam-1].c_maximo) - 101,
                           (v_mpfi(ipfi)->f_campos[numcam-1].c_mante.c_lineal & C_LINEAL),
                           v_mpfi(ipfi)->f_campos[numcam-1].c_mante.c_qp,
                           "",v_mpfi(ipfi)->f_campos[numcam-1].c_mante.c_qpl,NULL,v_mpfi(ipfi)->f_campos[numcam-1].c_fflag));
                        modificado = 1;
                     }
                     /********
                     EN EL CASO DE LOS MEMO NO SE APLICA EL CALCULO TIPO 14
                     if (actualiza_cam(ipfi,numcam,faclin,14) != -1) {
                     }
                     ********/
                     if (!modificado)
                        w_pinta(ww);
                     w_fin(ww);
                  }
                  else {
                     pqi = v_mpfi(ipfi)->f_campos[numcam-1].c_minimo;
                     pqs = v_mpfi(ipfi)->f_campos[numcam-1].c_maximo;
                     ld_campo(numcam-1,v_mpfi(ipfi),qd);  /* valor defecto guardado en el buffer */
                     rentra_offqp = -3;
                     if (encl != FALSO && !faclin)
                        mensaje_en_cabecera("%s","",2);
                     
					 if (!encl)
					 {
						 sin_permiso = hay_dat_permiso(v_mpfi(ipfi),(numcam-1)+10000);
						 if (!sin_permiso || (sin_permiso & 0x400) || ((sin_permiso & 0x800) && alta != ALTA))
						 {
							 sin_permiso = 1;
						 }
						 else
						 {
							 sin_permiso = 0;
						 }
					 }

                     if ( sin_permiso )
                     {
                        strcpy(qo,qd);
                        if (encl == 1 || (encl && faclin >= 100))
                        {
                           qj = 7;
                        }
                        else
                        {
                           if (qj != 2) qj = 0;
                        }                     
                     }
                     else
                     {
                        do
                        {
                           if (va_con_doble_ && qc == -1 && (v_mpfi(ipfi)->f_campos[numcam-1].c_mante.c_qpl == 11 || (v_mpfi(ipfi)->f_campos[numcam-1].c_mante.c_qpl == 6 && encl != FALSO)) && mpfi[48])
                           {
                              data_fecha_conmutar = 1;
                           }
                           if (qc == -1)
                           {
                              desarregla_fecha(qd,v_mpfi(ipfi)->f_campos[numcam-1].c_mante.c_qpl);
                           }
                           /********/                           
                           /********/
                           if (repite_qj != -1)
                           {
                               qj = repite_qj;
                               repite_qj = -1;
                               strcpy(qo,qd);
                           }
                           else
                           {
                               int el_flag = (flag_especial&1);
                               if ( (v_mpfi(ipfi)->f_campos[numcam-1].c_mante.c_lineal & FGV_C_GRID) )
                               {
                                   el_flag |= 2;

                                   el_flag |= 1; /* como si estuviera siempre en campo linea!!!*/

                                   id_g_control = v_mpfi(ipfi)->f_idgrafico;
                                   if (v_mpfi(ipfi)->f_campogids && v_mpfi(ipfi)->f_campogids[numcam-1] != 0xffff)
                                   {
                                        id_g_campo   = v_mpfi(ipfi)->f_campogids[numcam-1];
                                   }
                                   else
                                        id_g_campo   = -1;

                               }
                               if (TestigoRaton)
			                   {
                                   TestigoRaton = 0;                                   
                                   qj = SOLICITA_POSICION;
                                   strcpy(qo,qd);
			                   }
                               else
                               {	
								   if (!nds_entrada_automatica || !ds_recoge_entrada_automatica(ipfi,numcam-1,qd,qo,&qj,qj))
								   {									   
									   qj = i_entra(el_flag*10000+qp,qc,pqi,pqs,qd,1+v_mpfi(ipfi)->f_campos[numcam-1].c_mante.c_qpl*10,qo,hflag,v_mpfi(ipfi)->f_campos[numcam-1].c_mante.c_mensaje);
								   }
                               }
                           }
                           if (qc == -1)
                           {
                               arregla_fecha(qo,v_mpfi(ipfi)->f_campos[numcam-1].c_mante.c_qpl);
                           }
                           if (va_con_doble_ && data_fecha_conmutar > 1)
                           {
                               cambiame_paths_especial();
                           }
                           data_fecha_conmutar = 0;
                           if (qj != 7)
                           {
                              if (esta_limitado(ipfi,(long)(numcam-1),qo,v_mpfi(ipfi)->f_campos[numcam-1].c_tipo)                                        )
                              {
                                 aviso(0,__Texto[16]);
                                 continue;
                              }
                           }
                           break;
                        } while(-1);
                        campo_entrado = 1;
                     }
                     
                     if (encl != FALSO && !faclin)
                        borra_mensaje_cabecera(2);
                     
                     st_campo(numcam-1,v_mpfi(ipfi),qo);  /* pone dato en el registro (buffer) */
                     if (!sin_permiso && !encl && !hay_dat_permiso(v_mpfi(ipfi), -((numcam-1)+10) ))
                     {
                        st_campo(numcam-1,v_mpfi(ipfi),qd);
						if (qj != 7 && (qj != 1 || !encl))
						{
							aviso(0,__Texto[17]);
							erroren = 1;
						}
                     }
                     
                     salvaa = v_mpfi(ipfi)->f_campos[numcam-1].c_fflag;
                     if (actualiza_cam(ipfi,numcam,faclin,14) != -1) 
                     {
                        cursor(qp);
                        if (esta_en_linea) 
                        {
                           if (rentra_cc != rentra_cf)
                              Color(rentra_cc,rentra_cf);
                           atributo(rentra_atrib);
                           a_atri_campo(v_mpfi(ipfi),numcam-1,rentra_atrib,0,rentra_cc,rentra_cf);
                           printcam(v_mpfi(ipfi),numcam);
                           a_atri_campo(v_mpfi(ipfi),numcam-1,rentra_atrib,1,rentra_cc,rentra_cf);
                           atributo(tolower(rentra_atrib));
                        }
                        else
                        {
                           printusi_i(v_mpfi(ipfi),numcam);
                        }
                     }
                     v_mpfi(ipfi)->f_campos[numcam-1].c_fflag = salvaa;
                     if (!modif2) {
                        modificado = ((strcmp(qini,qo)) ? 1 : 0); /* compara --> es 0 si son iguales */
                     }
                     else {
                        modificado = 1;
                        modif2 = 0;
                     }
              }
              if (fm)
              {Ar();Bl(4);Cr();}
              if (e_contador && qj != 1 && qj != 7 && encl && altaa == ALTA && !mira_sivacio(v_mpfi(ipfi),numcam-1)) {
                 Ar();
                 aviso(0,__Texto[18]);
                 Cr();
                 erroren = 1;
                 continue; /* ! */
              }
                }
                else
                   qj = 7;
                
                if (memoplin > 4) 
                {			
                   if (qj == SOLICITA_AVANCE)
                      tmp_qj = 5;
                   else
                      if (qj == SOLICITA_RETROCESO)
                         tmp_qj = 4;
                      else
                         if (qj < 0)
                         {
                            if (qj == SOLICITA_POSICION)
                            {
                                if ((_ID_Cursor%100) == ipfi)
                                {
                                   if (ccc > 0 && (_ID_Cursor/100) < ccc)
                                       tmp_qj = 2;
                                   else
                                       tmp_qj = 3;
                                }
                                else
                                    tmp_qj = 3; 
                            }
                            else
                            switch(qj)
                            {
                            case SOLICITA_INICIO:
                                tmp_qj = 2;
                                break;
                            default:
                                tmp_qj = 3; /* ojo en caso de subir !!! */
                                break;
                            }                            
                         }
                         else
                            tmp_qj = qj;
                         sprintf(v_memo(5004)->alfabetica,"%d",tmp_qj);
                }
                
                if ( qj == 7 || (encl && qj == 1) || (encl && faclin > 0 && qj != 0 && qj != 6) ) 
                {
                   tras_entra(numpan,numcam,CONSULTA,encl,0,qd,ipfi,faclin);
                   if (memoplin > 4) sprintf(v_memo(5004)->alfabetica,"%d",qj);
                   if ((tipoerror = actualiza_cam(ipfi,numcam,faclin,11)))
                   {
                      if (tipoerror == -2)
                         st_campo(numcam-1,v_mpfi(ipfi),qini);		    
                      modif2 = 1;
                      erroren = 1;
                   }
                }
                else
                   if (qj != 6 && (tipoerror = tras_entra(numpan,numcam,altaa,encl,modificado,
                      qd,ipfi,faclin)) < 0) {
                      if (tipoerror == -2)
                         st_campo(numcam-1,v_mpfi(ipfi),qini);		    
                      modif2 = 1;
                      erroren = 1;
                   }
                   else if (qj == 6) 
                      {              /* consulta */
                         if (!actualiza_cam_(ipfi,numcam,faclin,66,1))
                         {
							int ant_eqj = eqj;
							int s_ac;
							eqj = 0;
							s_ac = actualiza_cam_(ipfi,numcam,faclin,66,0);
							eqj = ant_eqj;
                            if (s_ac)
                            {
                               erroren = 1;
                               if (altaa == ALTA)
                                  modif2 = 1;
                            }
                            else
                            {				  
                               if ((tipoerror = tras_entra(numpan,numcam,altaa,encl,modificado,
                                  qd,ipfi,faclin)) < 0) 
                               {
                                  if (tipoerror == -2)
                                     st_campo(numcam-1,v_mpfi(ipfi),qini);		    
                                  modif2 = 1;
                                  erroren = 1;
                               }
                            }
                         }
                         else
							 /*
                            if (eqj == 1)
                            {
                               if (encl && !(faclin/100))
                                  i = consul_clave(ipfi,encl,numpan,0,0,0,NULL);
                               else
                                  i = -2;
                               if (i < 0)
                               {
                                  erroren = 1;
                                  if (altaa == ALTA)
                                     modif2 = 1;
                               }
                            }
							...9.10 :  */
                            if ((eqj == 1/* || eqj == 2*/) && encl && !(faclin/100))
                            {
                               /*if (encl && !(faclin/100))*/
                                  i = consul_clave(ipfi,encl,numpan,0,0,0,ipfi,numcam-1,NULL);
                               /*else
                                  i = -2;*/
                               if (i < 0)
                               {
                                  erroren = 1;
                                  if (altaa == ALTA)
                                     modif2 = 1;
                               }
							   else
							   {
								   if ((tipoerror = tras_entra(numpan,numcam,altaa,encl,modificado,
									  qd,ipfi,faclin)) < 0) 
								   {
									  if (tipoerror == -2)
										 st_campo(numcam-1,v_mpfi(ipfi),qini);		    
									  modif2 = 1;
									  erroren = 1;
								   }
							   }
                            }
                            else
                               if ((i = entra_rel(numpan,numcam,altaa,encl,0,ipfi,2,faclin)) < 0)
                               {
                                  if (i == -2 && encl && !(faclin/100))
                                  {
                                     i = consul_clave(ipfi,encl,numpan,0,0,0,ipfi,numcam-1,NULL);
									 if (i >= 0)
									 {
									   if ((tipoerror = tras_entra(numpan,numcam,altaa,encl,modificado,
										  qd,ipfi,faclin)) < 0) 
									   {
										   if (tipoerror == -2)
											 st_campo(numcam-1,v_mpfi(ipfi),qini);		    
										   modif2 = 1;
										   i = -1;
									   }
									 }
                                  }
                                  else
                                     qj = 0;
                                  if (i < 0)
                                  {
                                     erroren = 1;
                                     if (altaa == ALTA)
                                        modif2 = 1;
                                  }
                               }
                               else
                                  qj = 0;
                      }
                      if (apli_aborto) {qj = 7;break;}
                      
        } while (erroren && !sin_permiso);
        
        free(qd);free(qo);free(qini);
        
        recurre_mensaje_cabecera(1);
        modo_de_relacion = o_modo_rel;
        if (requiere_reposicionar)
        {
		   if (requiere_reposicionar == 2)
		   {
			   if (qj_forzado != -1)
			   {
				   qj = qj_forzado;
				   sprintf(v_memo(5004)->alfabetica,"%d",qj);
			   }		   
		   }
		   else
		   {			   
			   qj = SOLICITA_POSICION;
		   }
		   qj_forzado = -1;
		   requiere_reposicionar = 0;
        }

		memcpy(def_erp_actual,_def_erp_actual,sizeof(def_erp_actual));
		ipfi_erp_actual = _ipfi_erp_actual;
		campo_erp_actual = _campo_erp_actual;		

        return(qj);
}



/*
* pregun_clave
* pregunta una clave concreta 'nclave' del fichero opfi
* devuelve -1 si se ha elegido una clave (grabando su valor en pnclave)
* devuelve -1 si se ha decidido salir con esc o ctrl-c (grabando -1 en pnclave)
* o >=0 si se ha decidido cambiar de clave de acceso (devolviendo el numero
* de clave en el return y grabandolo en pnclave).
* en caso de elegir una clave, la variable de buffer del fichero (ipfi)
* contiene la clave solicitada.
*/
int pregun_clave(pnclave,ipfi,faclin,alta)
int *pnclave;
int ipfi;
int faclin;
int alta;
{
   int qj = 1,cr = 0; /* campo referencia */
   int nclave = *pnclave;
   int numclaves,altaa,
      claini,
      numcam,
      ncc,
      campo,
      camfin,ultimocampo = -1,flag;
   int campini,campfin,i,j;
   int repite_qj = -1;
   int sin_pintar = 0;
   static int pulsado_qj = 0;
   recurre_raton(0);
   if (faclin < 0)
   {
       sin_pintar = 1;
       faclin = 0;
   }
   
   
   campini = v_mpfi(ipfi)->f_pantalla[0].p_campoini;  /* campo inicial */
   campfin = v_mpfi(ipfi)->f_pantalla[0].p_campofin;  /* campo final */
   /* poner campos para raton */
   anula_mapa_raton(-1);
   
   if (!sin_pintar)
   {
       for (i = campini;i <= campfin;i++)
       {
          pon_campo_en_el_raton(ipfi,i,faclin);
       }
   }
   
   if (memoplin > 3)
   {
      altaa = atoi(v_memo(5003)->alfabetica);
      altaa = (altaa % 100) + alta * 100;
      sprintf(v_memo(5003)->alfabetica,"%d",altaa);
   }
   if (alta != ALTA) alta = MODIF;
   
   
   numcam = 0;
   if (nclave < 0)
   { /* control de usar la clave y referencia de lineas */
      if (nclave < -1)
      {
         repite_qj = (-nclave) - 2;
      }
      nclave =  da_cla_lin(ipfi);
      cr = campo_linea(ipfi) + 1;
      numcam = da_cam_cla(ipfi);
   }
   
   numclaves = v_mpfi(ipfi)->f_nclaves;
   if (nclave >= numclaves) nclave = numclaves - 1;
   camfin = v_mpfi(ipfi)->f_clave[nclave].k_ncampos;
   /* campos de la clave (cuantos) */
   claini=0;
   
   
   
   campo_entrado = 0;
   for (ncc=0;ncc<camfin;numcam++,ncc++)
   {
      if (numcam >= camfin)
         numcam = 0;
      if (ncc < 0)
      {
         if (faclin > 0) {anula_mapa_raton(-1);recurre_raton(1);return(qj);}
         nclave--;
         if (nclave < claini)
            nclave = claini;
         *pnclave = nclave;
         anula_mapa_raton(-1);
         recurre_raton(1);
         return(nclave);
      }
      campo = v_mpfi(ipfi)->f_clave[nclave].k_campo[numcam];
      /* numero del campo a procesar */
      if (ultimocampo >= ncc)
      {
         altaa = MODIF;
      }
      else {
         if (memoplin > 2)
         {
            v_memo(5000)->numerica = (double)(campo - 1);
            v_memo(5011)->numerica = (double)ipfi;
         }
         ultimocampo = ncc;
         altaa = alta;
      }
      if (campo == cr && !(v_mpfi(ipfi)->f_campos[campo-1].c_mante.c_visual) &&
         v_mpfi(ipfi)->f_campos[campo-1].c_mante.c_modif)
         flag = 1;
      else
      {
         if (repite_qj != -1)
         {
            flag = 2 + repite_qj;
            repite_qj = -1;						  
         }
         else
         {
            if (campo == cr)
               flag = -1;
            else
               flag = 0;
         }
      }
      qj = entra_campo(ipfi,campo,0,altaa,faclin,qj,nclave+1,flag,-1,alta);
      
      if (qj == SOLICITA_POSICION)
      {
         if ((_ID_Cursor%100) != ipfi)
         {
            ncc--;
            _ID_Cursor = -1;
            continue;
         }
         if (!sin_pintar)
         {
             for (i = 0;i < camfin;i++)
             {
                campo = (v_mpfi(ipfi)->f_campos[(_ID_Cursor/100)-1].c_mante.c_actual % 1000)+1;
                if (campo == v_mpfi(ipfi)->f_clave[nclave].k_campo[i])
                {
                   break;
                }
             }
             /* de momento sin pasar por todos los campos */
             if (i < camfin)
             {
                _ID_Cursor = -1;
                ncc = i - 1;
                numcam = i - 1;
                continue;
             }
         }
         else
         {
             qj = que_clave_tiene_el_raton(ipfi,&j);
             if (qj == (nclave+1))
             {
                _ID_Cursor = -1;
                ncc--;
                numcam--;
                continue;
             }
         }
         /* qj = que_clave_tiene_el_raton(ipfi,&j);
         if (qj && !cr)
         {
         *pnclave = qj - 1; */
         /*  if (j > 1) /* si no es el primero situar */
         /*    ptec(SOLICITA_POSICION);
         anula_mapa_raton(-1);
         recurre_raton(1);
         return(*pnclave);
      }*/
         anula_mapa_raton(-1);
         recurre_raton(1);
         return(SOLICITA_POSICION);
      }
      
      if (campo == cr && (qj == 3 || qj == 2 || qj == 4 || qj == 5 || qj == 1 || qj == 7 ||
         qj == SOLICITA_INICIO || qj == SOLICITA_FINAL ||
         qj == SOLICITA_INSERCION || qj == SOLICITA_SUPRESION))
      {
         anula_mapa_raton(-1);
         recurre_raton(1);
         return(qj);
      }
      else
         if (qj == 0 || qj == 3)         /* validar , abajo */
            ;
         else
            if (qj == 2)
            {              /* arriba */
               numcam -= 2;
               ncc -= 2;
            }
            else
               if (qj == 4)
               {             /* pgup   */
                  nclave--;
                  if (nclave < claini)
                     nclave = claini;
                  *pnclave = nclave;
                  if (faclin > 0)
                  {
                     anula_mapa_raton(-1);
                     recurre_raton(1);
                     return(qj);
                  }
                  else
                  {
                     break;
                  }
               }
               else
                  if (qj == 5)
                  {             /* pgdn   */
                     nclave++;
                     if (nclave >= numclaves)
                        nclave = claini;
                     *pnclave = nclave;
                     if (faclin > 0)
                     {
                        anula_mapa_raton(-1);
                        recurre_raton(1);
                        return(qj);
                     }
                     else
                     {
                        break;
                     }
                  }
                  else
                     if (qj == 1 || qj == 7)
                     {  /* esc , abort  */
                        *pnclave = -1;
                        break;
                     }
                     else
                        if (qj == 6)
                           break;
                }
                
                clave_de_consulta = 0;
                if (faclin > 0 || cr)
                {
                   anula_mapa_raton(-1);
                   recurre_raton(1);
                   return(qj);
                }
                
                if (!campo_entrado && qj != 1 && qj != 7)
                {
                   if (pulsado_qj == 4)
                   {
                      nclave--;
                      if (nclave < claini)
                         nclave = claini;
                      qj = 4;
                   }
                   else
                   {
                      nclave++;
                      if (nclave >= numclaves)
                         nclave = claini;
                      qj = 5;
                   }
                   *pnclave = nclave;
                }
                else
                   pulsado_qj = qj;
                
                if (qj == 1 || qj == 7)
                {
                   anula_mapa_raton(-1);
                   recurre_raton(1);
                   return(-1);
                }
                if (qj == 0 || qj == 6)
                {                     /* el ultimo campo de una clave */                                              /* lo valido con return         */
                   if (qj == 6)
                      clave_de_consulta = 1;
                   *pnclave = nclave;
                   anula_mapa_raton(-1);
                   recurre_raton(1);
                   return(-1);
                }
                else
                {                             /* en el ultimo campo pulso abajo */
                   if (qj == 3)
                   {
                      nclave++;
                      if (nclave >= numclaves)
                         nclave = claini;
                      *pnclave = nclave;
                   }
                   anula_mapa_raton(-1);
                   recurre_raton(1);
                   return(nclave);
                }
                
                anula_mapa_raton(-1);
                recurre_raton(1);
                /*
                * return         *pnclave        Eleccion
                *   -1              -1           Salir con esc o ctrl-c
                *   -1             nclave        Entramos la clave numero nclave
                * nclave           nclave        Pedimos otra clave (la nclave)
                */
}


int pon_campo_en_el_raton(ipfi,ccc,faclin)
int ipfi;
int ccc;
int faclin;
{
   int i,kk,qp,j;
   int campo = v_mpfi(ipfi)->f_campos[ccc-1].c_mante.c_actual % 1000;
   qp = v_mpfi(ipfi)->f_campos[campo].c_mante.c_qp;
   if ((faclin/100) && (v_mpfi(ipfi)->f_campos[campo].c_mante.c_lineal & C_LINEAL))
   {
      qp += (faclin/100-1)*100;
   }
   i = v_mpfi(ipfi)->f_campos[campo].c_mante.c_qpl;
   j = long_campo(v_mpfi(ipfi),campo);
   if (v_mpfi(ipfi)->f_campos[campo].c_tipo != CHARTIPO || i < 1 || i > j)
      i = j;
   return(pon_mapa_raton(qp,qp+i-1,ccc*100+ipfi,0));
}

int que_clave_tiene_el_raton(ipfi,cc)
int ipfi;
int *cc;
{
   int campo,clave,i,j;
   if (_ID_Cursor == -1) return(-1);
   if ((_ID_Cursor%100) != ipfi)
			{
      return(-1);
			}
   campo = (v_mpfi(ipfi)->f_campos[(_ID_Cursor/100)-1].c_mante.c_actual % 1000) + 1;
   /* clave = v_mpfi(ipfi)->f_campos[campo].c_key;*/
   clave = 0;
   for (i = 0;i < v_mpfi(ipfi)->f_nclaves;i++)
			{
      for (j =0;j < v_mpfi(ipfi)->f_clave[i].k_ncampos;j++)
      {
         if (campo == v_mpfi(ipfi)->f_clave[i].k_campo[j])
         {
            clave = i + 1;
            i = 9999;
            if (cc)
               *cc = j + 1;
            break;
         }
      }
			}
   return(clave);
}

/*
* entra_datos
* entrada de datos de todos los campos de un fichero
* devuelve -1 si se sale con ctrl-c
* 0 en caso contrario
*/
int entra_datos(alta,ipfi,faclin)
int alta;
int ipfi;
int faclin;
{
		 int qj = 1;
         int campini,campfin;
         int panini,panfin;
         int numpan,numcam,ccc,pregucor = -2,i,j;
         int pintar;
         int pintada1;
         int salida=0,estoyen;
         int cerrar; /* si estaba ya en no cerrar no liberar el flag */
         int tipo;
         int altaa;
         int ultimocampo = -1;
         char *buf = NULL;
         double vca,vcf;
         int old_modo_validacion = modo_de_validacion;
         int o_modo_rel;
         int p_p_for_lin = 0;
         int resposicionar = 0;
         int estoy_en_lineas = -1;
         int tmp_qj;
         int avance_especial = 0;
         int sin_pintar = 0;
         
         if (faclin > 99)
         {
            estoy_en_lineas = campo_linea(ipfi) + 1;
         }

		 recurre_pantalla(0);
         
         
         _M_EXTRA_AVANCE = 0;
         
         recurre_raton(0);
         
         modo_de_validacion = ESC;
         
         if (memoplin > 2) 
         {
            vca = v_memo(5000)->numerica;
            vcf = v_memo(5011)->numerica;
         }
         
         if (faclin < 0) 
         {
             sin_pintar = 2;
             faclin = 0; 
         }
         
         if (memoplin > 3) {
            altaa = atoi(v_memo(5003)->alfabetica);
            altaa = (altaa % 100) + alta * 100;
            sprintf(v_memo(5003)->alfabetica,"%d",altaa);
         }
         if ((cerrar = f_v(ipfi,RNOCERRAR)) == -1)
            f_s(ipfi,RNOCERRAR);
         tipo = (-alta);
         o_modo_rel = modo_de_relacion;
         modo_de_relacion = 1;
         if (actualiza_rel(ipfi,tipo,0,faclin,&buf) < 0)
         {
            tipo = alta;
            actualiza_rel(ipfi,tipo,-1,faclin,&buf);
            Ar();Bl(4);Cr();   /* borrar mensaje */
            if (cerrar == -1)
               f_n(ipfi,RNOCERRAR);
            if (buf)
            {
               rw_error(77);
            }
			recurre_pantalla(1);
            modo_de_validacion = old_modo_validacion;
            modo_de_relacion = o_modo_rel;
            recurre_raton(1);
            if (faclin > 0)
			{
               return(7);
			}
            return(-1);
         }
         modo_de_relacion = o_modo_rel;   
         if (!sin_pintar)
            panfin = v_mpfi(ipfi)->f_npanta;   /* numero de pantallas */
         else
            panfin = 1;
         panini=0;
         
         if (faclin > 99)
         {
            panfin = panini + 1;
         }
         
         
         if (alta == MODIF) 
         { /* poner control de relaciones a ok */
            entra_rel(0,0,alta,FALSO,1,ipfi,6,0);
         }
         
         pintada1 = 1;
         numpan=panini;
         
         
         if (!sin_pintar && DespachadorExterno)
         {
            int total;
            char *arrai[100];
            char *puntero;
            total = 0;
            for (i = 0;i < panfin;i++)
            {
               puntero = v_mpfi(ipfi)->f_pantalla[i].p_nombrepan;
               puntero += strlen(puntero);		  
               if (*(puntero+1) == 1 && *(puntero+2) == 2)
               {
                  arrai[i] = puntero+3;
                  total++;
               }
            }
            if (total == panfin)
            {
				
               DespachoPantalla((int)DESPACHO_PANTALLAS,(int)total+100,(char *)arrai);
            }
            else
            {
               DespachoPantalla((int)DESPACHO_PANTALLAS,(int)panfin,v_mpfi(ipfi)->f_titulo);
            }
         }
         
         
         
         do {
            if (numpan > panfin)
            {
               estoyen = 0;
               pintada1 = 0;
               pintar = -1;
               numpan=panini;
            }
            while (numpan<panfin) 
            {
               if (_M_EXTRA_AVANCE && avance_especial)
               {
                  no_pintar_ahora(1);
               }
               
               if (numpan < panini || pintada1 == 1) {
                  pintar = 0;           /* no pintar la pantalla otra vez */
                  pintada1 = 0;         /* solo la primera vez no se pinta  */
                  numpan = panini;
                  estoyen = 0;
               }
               else
                  pintar = -1;
         
               if (!sin_pintar)
               {
                   campini = v_mpfi(ipfi)->f_pantalla[numpan].p_campoini;  /* campo inicial */
                   campfin = v_mpfi(ipfi)->f_pantalla[numpan].p_campofin;  /* campo final */
               }
               else
               {
                   campini = v_mpfi(ipfi)->f_pantalla[0].p_campoini;  /* campo inicial */
                   campfin = v_mpfi(ipfi)->f_pantalla[v_mpfi(ipfi)->f_npanta-1].p_campofin;  /* campo final */
               }               
               /* poner campos para raton */
               anula_mapa_raton(-1);
               if (!sin_pintar)
               {
                   for (i = campini;i <= campfin;i++)
                   {
                      pon_campo_en_el_raton(ipfi,i,faclin);
                   }
               }
               
               
               if (!sin_pintar && pintar) 
               {
                  pinppan(ipfi,numpan);                  /* pintar pantalla */
                  /*cabecera_v(rw_version,proceso,titulo,empresa);    /* pintar cabecera */
				  cabecera(proceso,titulo,empresa);
                  prin_pan(ipfi,numpan);     /* pintar datos pantalla numero numpan */
               }
               
               
               
               
               if (estoyen > numpan) {
                  ccc=campfin;
                  qj = 2;
               }
               else {
                  if (resposicionar)
                  {
                     ccc = p_p_for_lin;
                     resposicionar = 0;
                  }
                  else
                  {
                     ccc=campini;
                  }
                  qj = 0;
               }
               estoyen = numpan;            /* numero de pantalla en la que estamos */
               
               
               if (!_M_EXTRA_AVANCE || !avance_especial)
               {
				  _M_EXTRA_AVANCE = 0;
                  no_pintar_ahora(0);
               }
               if (!sin_pintar && DespachadorExterno)
               {
                  if (!_M_EXTRA_AVANCE)
                     DespachoQuePantalla((int)DESPACHO_QUEPANTALLA,(int)estoyen,v_mpfi(ipfi)->f_titulo);
               }
               
               while (ccc<=campfin) 
               {
                  if (ccc < campini) {
                     numpan -= 2;
                     break;
                  }
                  p_p_for_lin = ccc;
                  numcam = (v_mpfi(ipfi)->f_campos[ccc-1].c_mante.c_actual % 1000) + 1;
                  if (ultimocampo >= ccc) {
                     if (alta != CONSULTA) altaa = MODIF;
                  }
                  else  {
                     ultimocampo = ccc;
                     altaa = alta;
                     if (memoplin > 2) {
                        v_memo(5000)->numerica = (double)(numcam - 1);
                        v_memo(5011)->numerica = (double)ipfi;
                     }
                  }
                  if (numcam == estoy_en_lineas &&  alta == ALTA)
                  {
                     /* nada ! */
                  }
                  else
                  {
                     if (_M_EXTRA_AVANCE)
                     {
                        _M_EXTRA_AVANCE--;
                        if (avance_especial < 0)
                        {
                           ccc++;
                           qj = SOLICITA_RETROCESO;
                        }
                        else
                           if (avance_especial > 0)
                           {
                              qj = SOLICITA_AVANCE;
                              ccc--;
                           }
                     }
                     else
                     {
                        qj = entra_campo(ipfi,numcam,numpan,altaa,faclin,qj,FALSO,0,ccc,alta);
                        if (qj == SOLICITA_INICIO)
                        {
                            for (i = campini-1;i < ccc;i++)
                            {
                                qj = (v_mpfi(ipfi)->f_campos[i].c_mante.c_actual % 1000);
                                if (qj >= 0 && qj < v_mpfi(ipfi)->f_ncampos && !v_mpfi(ipfi)->f_campos[qj].c_mante.c_modif && !v_mpfi(ipfi)->f_campos[qj].c_mante.c_visual && v_mpfi(ipfi)->f_campos[qj].c_key != 1)
                                {
                                    break;
                                }
                            }
                            if (i < ccc)
                            {
                                qj = SOLICITA_POSICION;
                                _ID_Cursor = (i+1)*100+ipfi;
                            }
                            else
                            {
                                qj = 0;
                                continue;
                            }
                        }
                        else
                        if (qj == SOLICITA_FINAL)
                        {
                            for (i = campfin-1;i >= ccc;i++)
                            {
                                qj = (v_mpfi(ipfi)->f_campos[i].c_mante.c_actual % 1000);
                                if (qj >= 0 && qj < v_mpfi(ipfi)->f_ncampos && !v_mpfi(ipfi)->f_campos[qj].c_mante.c_modif && !v_mpfi(ipfi)->f_campos[qj].c_mante.c_visual && v_mpfi(ipfi)->f_campos[qj].c_key != 1)
                                {
                                    break;
                                }
                            }
                            if (i >= ccc)
                            {
                                qj = SOLICITA_POSICION;
                                _ID_Cursor = (i+1)*100+ipfi;
                            }
                            else
                            {
                                qj = 0;
                                continue;
                            }
                        }
                     }
                  }
                  avance_especial = 0;
                  if (qj == SOLICITA_POSICION || qj == 4 || qj == 5 || qj == SOLICITA_AVANCE || qj == SOLICITA_RETROCESO)
                  {
                     if (qj == SOLICITA_POSICION)
                     {
                        qj = que_clave_tiene_el_raton(ipfi,&j);
                        if (qj == 1)
                        {
                           pito(107);
                           qj = 0;
                           _ID_Cursor = -1;
                           continue;
                        }
                        if ((_ID_Cursor%100) != ipfi)
                        {
                           _ID_Cursor = -1;
                           continue;
                        }
                        i = (_ID_Cursor/100);
                        qj = (v_mpfi(ipfi)->f_campos[i-1].c_mante.c_actual % 1000);
                        if (v_mpfi(ipfi)->f_campos[qj].c_mante.c_modif)
                        {
                           pito(108);
                           qj = 0;
                           _ID_Cursor = -1;
                           continue;
                        }
                        _ID_Cursor = -1;
                        qj = 0;
                     }
                     else
                        if (qj == 4 || qj == SOLICITA_RETROCESO)
                        {
                           i = campini - 1;
                           if (qj == SOLICITA_RETROCESO)
                              avance_especial = -1;
                        }
                        else
                           if (qj == 5 || qj == SOLICITA_AVANCE)
                           {
                              i = campfin + 1;
                              if (qj == SOLICITA_AVANCE)
                                 avance_especial = 1;
                           }
                           no_mensaje = 1;
                           if (ccc < i)
                           {
                              /* ha de saltar el actual que ya ha pasado el filtro */
                              for(ccc++;ccc < i;ccc++)
                              {
                                 if (alta == ALTA && ccc > ultimocampo)
                                 {
                                    altaa = alta;
                                    ultimocampo = ccc;
                                 }
                                 j = (v_mpfi(ipfi)->f_campos[ccc-1].c_mante.c_actual % 1000)+1;
                                 
                                 if (numcam == estoy_en_lineas &&  alta == ALTA) continue;
                                 if (memoplin > 4) 
                                 {			
                                    if (qj == SOLICITA_AVANCE)
                                       tmp_qj = 5;
                                    else
                                       if (qj == SOLICITA_RETROCESO)
                                          tmp_qj = 4;
                                       else
                                          if (qj < 0)
                                             tmp_qj = 3;
                                          else
                                             tmp_qj = qj;
                                          sprintf(v_memo(5004)->alfabetica,"%d",tmp_qj);
                                 }
                                 
                                 if (!antes_entra(numpan,j,altaa,FALSO,ipfi,faclin,0))
                                 {
                                    if (tras_entra(numpan,j,altaa,FALSO,0,"",ipfi,faclin) < 0)
                                    {
                                       i = ccc;
                                       qj = 0;
                                       _M_EXTRA_AVANCE = 0;		              
                                       no_pintar_ahora(0);
                                       if (!sin_pintar && DespachadorExterno)
                                       {
                                          DespachoQuePantalla((int)DESPACHO_QUEPANTALLA,(int)estoyen,v_mpfi(ipfi)->f_titulo);
                                       }
                                       
                                       break;
                                    }
                                 }
                              }
                           }
                           
                           if (ccc > i)
                           {
                              altaa = MODIF;
                              for(ccc--;ccc > i;ccc--)
                              {
                                 j = (v_mpfi(ipfi)->f_campos[ccc-1].c_mante.c_actual % 1000)+1;
                                 if (numcam == estoy_en_lineas &&  alta == ALTA) continue;
                                 if (memoplin > 4) 
                                 {			
                                    if (qj == SOLICITA_AVANCE)
                                       tmp_qj = 5;
                                    else
                                       if (qj == SOLICITA_RETROCESO)
                                          tmp_qj = 4;
                                       else
                                          if (qj < 0)
                                             tmp_qj = 2;
                                          else
                                             tmp_qj = qj;
                                          sprintf(v_memo(5004)->alfabetica,"%d",tmp_qj);
                                 }
                                 if (!antes_entra(numpan,j,altaa,FALSO,ipfi,faclin,0))
                                 {
                                    if (tras_entra(numpan,j,altaa,FALSO,0,"",ipfi,faclin) < 0)
                                    {
                                       i = ccc;
                                       qj = 0;
                                       _M_EXTRA_AVANCE = 0;
                                       
                                       no_pintar_ahora(0);
                                       if (!sin_pintar && DespachadorExterno)
                                       {
                                          DespachoQuePantalla((int)DESPACHO_QUEPANTALLA,(int)estoyen,v_mpfi(ipfi)->f_titulo);
                                       }
                                       
                                       break;
                                    }
                                 }
                              }
                           }
                           no_mensaje = 0;
                           if (!qj) ccc--;
                          }
                          
                          if (qj == 0 || qj == 3)         /* validar , abajo */
                             ;
                          else if (qj == 2)               /* arriba */
                             ccc -= 2;
                          else if (qj == 4 || qj == SOLICITA_RETROCESO) {             /* pgup   */
                             numpan -= 2;
                             break;
                          }
                          else if (qj == 5 || qj == SOLICITA_AVANCE)               /* pgdn   */
                             break;
                          else if (qj == 1) {             /* esc    */
                             numpan = 9999;
                             ccc = 9999;
                             break;
                          }
                          else if (qj == 7) {             /* abort  */
                             numpan = 9999;
                             ccc = 9999;
                             salida = -1;
                             break;
                          }
                          ccc++;
                        }
                        numpan++;
                        if (!faclin && numpan == panfin)
                        {
                           numpan--; /* para que al dar error vuelva a donde estaba */
                           break;
                        }
                        if (faclin > 0 && numpan < panini) break;
                }
                
                if (memoplin > 3)
                   sprintf(v_memo(5004)->alfabetica,"%d",qj);
				if (nds_entrada_automatica > 0)
				{
					pregucor = 0;
				}
				else
				{
					pregucor = ejecuta_otro(ipfi,12,O_NOR,faclin,0);
				}
				if (pregucor > 0)
					pregucor = 0;
                if (pregucor == -2 && !salida) /* no hay proceso!! */
                {
                   /* preguntar si-no-anular */
                   
                }
                if (pregucor == -1)
                {
                   if (faclin > 100)
                   {
                      numpan=panini;
                      pintada1 = 1;
                      resposicionar = 1;
                   }
                   else
                   {
                      numpan = 9999;
                   }
                   salida = 0;
                   qj = 1;
                   continue;
                }
                
#ifdef COMENTARIO		
                if (memoplin > 3 && !memcmp(v_memo(5002)->alfabetica+22,"si",2))
                   salida = 1; /* abandono forzado por calculo */
#endif
                
                /* ojo continue ... no poner para salir sino para repetir */
                if (salida || entra_rel(numpan,0,alta,FALSO,1,ipfi,5,faclin) >= 0)
                   break;
        } while(-1);
        
        /* limpiar marcas */
        entra_rel(numpan,0,alta,FALSO,1,ipfi,7,faclin);
        
        if (!salida && v_mpfirel(ipfi)->clave > -1 && faclin == 0)
           /* solo fichas no lineales */
           salida = cheq_clave(ipfi,v_mpfirel(ipfi)->clave,1);
        
        /* conservo el antiguo sistema de correcto aunque nunca se ejecute */
        if (!salida && (v_mpfirel(ipfi)->lineas > -1 || faclin || pregucor != -2 || !(salida = correcto()))) 
        {
           
           if (alta == ALTA)
              defecam_rel(ipfi,-1,numpan,faclin);
           if (v_mpfirel(ipfi)->lineas > -1) {   /* lineas */
              salida = trata_lineas(ipfi,alta,0,0,faclin);
              if (trata_lineas(ipfi,alta,3,0,faclin) != 0)
                 estoyen = panini + 1;
              if (apli_aborto) {salida = -1;}
           }
        }
        
        
        
        if (salida != 1 || alta == MODIF)
        {		
           o_modo_rel = modo_de_relacion;
           modo_de_relacion = 1;
           tipo = actualiza_rel(ipfi,alta,((salida != 1) ? (salida) : (-1)),faclin,&buf);
           modo_de_relacion = o_modo_rel;
           if (!salida) salida = tipo;           		
        }
        
        if (memoplin > 2) {
           v_memo(5000)->numerica = vca;
           v_memo(5011)->numerica = vcf;
        }
        
        if (!sin_pintar && salida != 1 && estoyen != 0) { /* pinto la primera pantalla antes de salir */
           pinppan(ipfi,0);
           /*cabecera_v(rw_version,proceso,titulo,empresa);*/
		   cabecera(proceso,titulo,empresa);
           if (alta != ALTA || salida)
              prin_pan(ipfi,0);
        }
        
        anula_mapa_raton(-1);
        
        if (salida == 1) salida = 0;
        
        Ar();Bl(4);Cr();   /* borrar linea de mensajes */
        if (cerrar == -1)
           f_n(ipfi,RNOCERRAR);
        
        if (faclin > 0)
        {
           if (!salida)
              salida = qj;
           else
              salida = (-(qj+2));
        }
        
        if (buf) 
        {
           rw_error(78);
        }
		
        modo_de_validacion = old_modo_validacion;
        recurre_raton(1);
        
        
        if (!sin_pintar && DespachadorExterno)
        {
           DespachoPantalla((int)DESPACHO_PANTALLAS,(int)1,"");
        }

		recurre_pantalla(1);
        
        
        _M_EXTRA_AVANCE = 0;
        
        return(salida);
}

/*
* entra_clave
* pregunta la(s) clave(s) del fichero cuya estructura es ipfi
* devuelve el numero de la clave elegida para el acceso (>=0)
* o <0 si se ha salido con esc o ctrl-c.
* en caso de elegir una clave, la variable de buffer (ipfi)
* contiene la clave solicitada.
*/
entra_clave(ipfi,nclave,alta)
int ipfi,*nclave,alta;
{
   int j;
   while(-1)
   {
      j = pregun_clave(nclave,ipfi,0,alta);
      if (j == SOLICITA_POSICION)
         return(j);
      if (j < 0)
         break;
   }
   return(*nclave);
}

/* actualiza_rel() */

int actualiza_rel(ipfi,tipo,incorrecto,faclin,buf)
int ipfi;
int tipo;
int incorrecto;
int faclin;
char **buf;
{
   int f,j,i,r,rr = 0,actu,nclave = 0;
   int _operaerror = operaerror;
   
   if ( tipo == CONSULTA || tipo == -ALTA ||
      ( (tipo == ALTA || tipo == BAJA) && incorrecto != 0 ) )
      return(0);
   
   if (tipo == -MODIF || tipo == BAJA)
      actu = O_REV;
   else
      actu = O_NOR;
   
   if (tipo == -MODIF && buf) 
			{
      if (*buf) {
         rw_error(76);
      }
      *buf = malloc(v_mpfi(ipfi)->f_lfich);
      memcpy(*buf,v_mpfi(ipfi)->f_buf,v_mpfi(ipfi)->f_lfich);
			}
   
   if (tipo == MODIF && incorrecto && buf && *buf) 
			{
      memcpy(v_mpfi(ipfi)->f_buf,*buf,v_mpfi(ipfi)->f_lfich);
			}
   
   if (tipo == MODIF && buf && *buf) 
			{
      free(*buf);
      *buf = NULL;
			}
   
   for (j=0;j<v_mpfirel(ipfi)->nrelact;j++) 
			{
      i = v_mpfirel(ipfi)->relact[j].tabla;
      if (i > 0) {
         r = tipo_tabla[i];
         if (r > 0 && r < 4) {
            operaerror = 0;
            f = actu;
            if (r == T_d) if (actu == O_NOR) continue; else f = O_NOR;
            if (r == T_a) if (actu == O_REV) continue;
            if (memoplin > 12)
               sprintf(v_memo(5012)->alfabetica,"%d",v_mpfirel(ipfi)->relact[j].fflag);
            p_ejecuta(i,f,faclin,6);
            if (operaerror != 0) {
               rr = -1;
               goto salir;
            }
         }
      }
			}
   
salir:
         operaerror = _operaerror;
         return(rr);
}

int baja_fichao(ipfi)
int ipfi;
{
			int sal;
         int k;
         int r = -1;
         
         if (memoplin > 3) {
            k = atoi(v_memo(5003)->alfabetica);
            strcpy(v_memo(5003)->alfabetica,"303");
         }
         
         if ((sal = actualiza_rel(ipfi,BAJA,0,0,NULL)) == 0 && !apli_aborto) {
            if (v_mpfirel(ipfi)->lineas > -1) {/* flag que indica que hay lineas */
               sal = trata_lineas(ipfi,BAJA,1,0,0);
            }
            if (apli_aborto || sal != 0 || borrar_reg(ipfi,0,0) < 0) {
               actualiza_rel(ipfi,ALTA,0,0,NULL);
            }
            else {
               r = 0;
            }
         }
         
         if (memoplin > 3) sprintf(v_memo(5003)->alfabetica,"%d",k);
         return(r);
}

int consulta_lee_prev(int ipfi)
{
	int r;
	int saltos = 0;
	int err = 0;
	do
	{
       r = leer_reg(ipfi,ISPREV,0,1);
	   if (r < 0)
		   break;
	   if (hay_dat_permiso(v_mpfi(ipfi),-2))
		   break;
	   saltos++;
	} while(-1);
	if (r < 0 && saltos)
	{
		err = iserrno;
		while(saltos)
		{
		   leer_reg(ipfi,ISNEXT,0,1);
		   saltos--;
		}
		iserrno = err;
	}
	return r;
}

int consulta_lee_next(int ipfi)
{
	int r;
	int saltos = 0;
	int err = 0;
	do
	{
       r = leer_reg(ipfi,ISNEXT,0,1);
	   if (r < 0)
		   break;
	   if (hay_dat_permiso(v_mpfi(ipfi),-2))
		   break;
	   saltos++;
	} while(-1);
	if (r < 0 && saltos)
	{
		err = iserrno;
		while(saltos)
		{
		   leer_reg(ipfi,ISPREV,0,1);
		   saltos--;
		}
		iserrno = err;
	}
	return r;
}

int consulta_lee_last(int ipfi)
{
	int r = leer_reg(ipfi,ISLAST,0,1);
	if (r < 0)
		return r;
   if (hay_dat_permiso(v_mpfi(ipfi),-2))
	   return r;
	do
	{
       r = leer_reg(ipfi,ISPREV,0,1);
	   if (r < 0)
		   break;
	   if (hay_dat_permiso(v_mpfi(ipfi),-2))
		   break;
	} while(-1);
	return r;
}

int consulta_lee_first(int ipfi)
{
	int r = leer_reg(ipfi,ISFIRST,0,1);
	if (r < 0)
		return r;
   if (hay_dat_permiso(v_mpfi(ipfi),-2))
	   return r;
	do
	{
       r = leer_reg(ipfi,ISNEXT,0,1);
	   if (r < 0)
		   break;
	   if (hay_dat_permiso(v_mpfi(ipfi),-2))
		   break;
	} while(-1);
	return r;
}


/*
* consul_ficha
* ejecuta la consulta de fichas segun el tipo
*
*/

consul_ficha(ipfi,nclave,m)
int ipfi,nclave,m;
{
   int leida,r,acabar = 0,opcion = 1,i,campini,campfin,j,kk = 0;
   int ppp = 0,pflag = 1;
   int oc = ind_clave_activa(v_mpfi(ipfi)) - 1;
   int salta_la_pregunta  = 0;
   char opci[31];
   char log_tmp[128];
   int otra_opcion = 99;
   int ant_opcion;
   char *l_copciones[10];
   char l_cletras[10];
   int la_primera_vez = 1;
	 char _def_erp_actual[25];
	 int _ipfi_erp_actual = ipfi_erp_actual;

	 memcpy(_def_erp_actual,def_erp_actual,sizeof(def_erp_actual));
	 strcpy(def_erp_actual,quita_path(v_mpfi(ipfi)->f_PathDef));

	 ipfi_erp_actual = ipfi;


/*
static char *copciones[10] = {
		__Texto[487],
		__Texto[488],
		__Texto[489],
		__Texto[490],
		__Texto[491],
		__Texto[492],
		__Texto[493],
        __Texto[494],      
		"",
		""      
};
static char cletras[10] = __Texto[495];
*/

   for (i = 0;i < 8;i++)
   {
       l_copciones[i] = __Texto[487+i];
       l_cletras[i] = __Texto[495][i];
   }
   for (;i < 10;i++)
   {
       l_copciones[i] = "";
       l_cletras[i] = 0;
   }  
   
  
   
   if (!hay_dat_permiso(v_mpfi(ipfi),-1))
   {
	   memcpy(def_erp_actual,_def_erp_actual,sizeof(def_erp_actual));ipfi_erp_actual = _ipfi_erp_actual;
      return(-1);
	  }
	
   recurre_raton(0);

   recurre_pantalla(0);
   


   if (nclave == -1) 
   {
      nclave = 0;
      opcion = 100;
      r = leer_reg(ipfi,ISFIRST,0,1);
      if (!r)
		{
         r = leer_reg(ipfi,ISNEXT,0,1);
         if (r)
         {
            leer_reg(ipfi,ISFIRST,0,1);
            salta_la_pregunta = 1;
         }
		}
      
   } /* acceso */
   
   
   while (acabar == 0)
   {
      clave_de_consulta = 0;
      if (salta_la_pregunta)
      {
         j = 0;
         salta_la_pregunta = 0;
      }
      else
      {
         if (DespachadorExterno)
         {
            DespachoPantalla((int)DESPACHO_PANTALLAS,(int)1,v_mpfi(ipfi)->f_titulo);
         }
         /*
         if (la_primera_vez)
            {
                ptec(820);
                la_primera_vez = 0;            
            }
          */
         j = entra_clave(ipfi,&nclave,CONSULTA);
      }
      if (j == SOLICITA_POSICION)
						{
         if (!TestigoRaton)
            TestigoRaton = 1;
         else
         {
            pito(109);
            TestigoRaton = 0;
         }
						}
      else
         if (j < 0)
         {
			recurre_pantalla(1);
            anula_mapa_raton(-1);
            recurre_raton(1);
            fin_botones_();			
			memcpy(def_erp_actual,_def_erp_actual,sizeof(def_erp_actual));ipfi_erp_actual = _ipfi_erp_actual;
            return(-1);
         }
         r = 0;
         if (oc != nclave)
         {
            if (!clave_de_consulta)
               r = cambia_ind(v_mpfi(ipfi),nclave+1);
            else
               r = apunta_ind(v_mpfi(ipfi),nclave+1,ISCURR,0);
         }
         if (r) {
            Ar();
            aviso(0,__Texto[19]);
            Cr();
            TestigoRaton = 0;
			recurre_pantalla(1);
            anula_mapa_raton(-1);
            recurre_raton(1);
            fin_botones_();			
			memcpy(def_erp_actual,_def_erp_actual,sizeof(def_erp_actual));ipfi_erp_actual = _ipfi_erp_actual;
            return(-1);
         }
         else {
            oc = nclave;
            if (clave_de_consulta == 0)
               r = leer_reg(ipfi,ISGTEQ,0,1);
            else
               r = leer_reg(ipfi,ISCURR,0,1);
			if (!r && !hay_dat_permiso(v_mpfi(ipfi),-2))
			{
				if (clave_de_consulta == 0)
				{
					r = consulta_lee_next(ipfi);
				}
				else
				{
					r = -1;
				}
			}
            clave_de_consulta = 0;
            if (r != 0)
            {
               TestigoRaton = 0;
               Ar();
               aviso(0,__Texto[20]);
               Cr();
               if (opcion == 100)
               {
				   recurre_pantalla(1);
                  anula_mapa_raton(-1);
                  recurre_raton(1);
                  fin_botones_();				  
				  memcpy(def_erp_actual,_def_erp_actual,sizeof(def_erp_actual));ipfi_erp_actual = _ipfi_erp_actual;
                  return(-1);
               }
            }
            else {
               clave_de_consulta = 0;
               while (-1) {
                  if (opcion < 7 || opcion > 99) _pinta_datos(ipfi,0,&ppp,pflag);
                  pflag = 1;
                  if (opcion == 100)
                  {
                     if (!hay_dat_permiso(v_mpfi(ipfi),-2))
                     {
                        TestigoRaton = 0;
                        aviso(0,__Texto[21]);
                        break;
                     }
                     
                     if (v_mpfirel(ipfi)->clave > -1)
                     {
                        if (cheq_clave(ipfi,v_mpfirel(ipfi)->clave,0) == 0)
                        {
							recurre_pantalla(1);
                           TestigoRaton = 0;
                           anula_mapa_raton(-1);
                           recurre_raton(1);
                           fin_botones_();						   
						   memcpy(def_erp_actual,_def_erp_actual,sizeof(def_erp_actual));ipfi_erp_actual = _ipfi_erp_actual;
                           return(0);
                        }
                        else
                        {
                           break;
                        }
                     }
					 recurre_pantalla(1);
                     TestigoRaton = 0;
                     anula_mapa_raton(-1);
                     recurre_raton(1);
                     fin_botones_();					 
					 memcpy(def_erp_actual,_def_erp_actual,sizeof(def_erp_actual));ipfi_erp_actual = _ipfi_erp_actual;
                     return(0);
                  }
                  
                  *(l_cletras+6) = 'M';
                  *(l_cletras+7) = 'I';                  
                  kk = 6;
                  otra_opcion = 99;
                  if (m)
                     *(l_cletras+6) = 0;
                  else
                  {
                     if (ejecuta_otro(ipfi,4,O_NOR,0,1) < 0)
                     {
                        kk = 7;
                        *(l_cletras+7) = 0;
                     }
                     else
                     {
                        kk = 8;
                     }
                  }

                  *(l_cletras+kk) = 0;

                  if (memoplin > 4)
                  {
                     if (ejecuta_otro(ipfi,20,O_NOR,0,1) >= 0)
                     {
                        strcpy(v_memo(5004)->alfabetica,"OPCION");
                        if (ejecuta_otro(ipfi,20,O_NOR,0,0) >= 0)
                        {
                           *(l_cletras + kk) = v_memo(5004)->alfabetica[0];
                           *(l_cletras + kk + 1) = 0;
                           strcpy(opci,v_memo(5004)->alfabetica);
                           l_copciones[kk] = opci;
                           otra_opcion = kk + 1;
                        }
                     }
                  }                  
                  
                  
                  
                  campini = v_mpfi(ipfi)->f_pantalla[0].p_campoini;  /* campo inicial */
                  campfin = v_mpfi(ipfi)->f_pantalla[0].p_campofin;  /* campo final */
                  /* poner campos para raton */
                  anula_mapa_raton(-1);
                  for (i = campini;i <= campfin;i++)
                  {
                         pon_campo_en_el_raton(ipfi,i,0);
                  }
               
                  
                  
                  TestigoRaton = 0;
                  if (apli_aborto)
                  {
                     acabar  = -1;
                     break;
                  }
                  prepara_menusg_botones();
                  
                  if (DespachadorExterno)
                  {	   
                     int total;
                     char *arrai[100];
                     char *puntero;
                     total = 0;
                     for (i = 0;i < v_mpfi(ipfi)->f_npanta;i++)
                     {
                        puntero = v_mpfi(ipfi)->f_pantalla[i].p_nombrepan;
                        puntero += strlen(puntero);		  
                        if (*(puntero+1) == 1 && *(puntero+2) == 2)
                        {
                           arrai[i] = puntero+3;
                           total++;
                        }
                     }
                     if (total == v_mpfi(ipfi)->f_npanta)
                     {
                        DespachoPantalla((int)DESPACHO_PANTALLAS,(int)total+100,(char *)arrai);
                     }
                     else
                     {
                        DespachoPantalla((int)DESPACHO_PANTALLAS,(int)v_mpfi(ipfi)->f_npanta,v_mpfi(ipfi)->f_titulo);
                     }
                  }
                  
                  ant_opcion = opcion;
                  opcion = menusg(l_copciones,l_cletras,0,"",opcion);
                  if (opcion != 3 && opcion != 4 && opcion != 5 && opcion != 6)
                  {		 
                     fin_botones_();
                  }
                  if (opcion < 1 && opcion > -3)
                  {
                     acabar  = -1;
                     break;
                  }
                  if (opcion == SOLICITA_POSICION)
                  {
                     i = que_clave_tiene_el_raton(ipfi,&j);
                     if (i > 0 && nclave == (i-1))
                     {
                        nclave = i - 1;
                        opcion = 100;
                        if (j > 1)
                        {
                           if (!TestigoRaton)
                              TestigoRaton = 1;
                           else
                           {
                              pito(110);
                              TestigoRaton = 0;
                           }
                        }
                        break;
                     }
                     if (m)
                     {
                       TestigoRaton = 0;
                        pito(111);
                        opcion = 1;
                        continue;
                     }
                     opcion = 7;
                     if (!TestigoRaton)
                        TestigoRaton = 1;
                     else
                     {
                        pito(112);
                        TestigoRaton = 0;
                     }
                  }
                  if (opcion < 0)
                  {
                     if (opcion == SOLICITA_AVANCE)
                     {
                        ppp++;
                        ppp += _M_EXTRA_AVANCE;
                        _M_EXTRA_AVANCE = 0;
                        opcion = ant_opcion;
                     }
                     else
                        if (opcion == SOLICITA_RETROCESO)
                        {
                           ppp--;
                           ppp -= _M_EXTRA_AVANCE;
                           _M_EXTRA_AVANCE = 0;
                           opcion = ant_opcion;
                        }
                        else
                        {
                           opcion = -opcion;
                           if ((opcion%10) == 3)
                              ppp--;
                           if ((opcion%10) == 4)
                              ppp++;       
                           opcion /= 10;
                        }
                        pflag = 0;
                        continue;
                  }
                  Ar();Bl(4);Cr();
                  if (opcion == otra_opcion)
                  {
                     strcpy(v_memo(5004)->alfabetica,"");
                     ejecuta_otro(ipfi,20,O_NOR,0,0);
                     if (strcmp(v_memo(5004)->alfabetica,"FINCONSULTA"))
                     {
                        continue;
                     }
                  }
                  else
                     if (opcion ==  7) 
					 {
                        if (mimodo_debug || logger_salida)
                        {
						   sprintf(log_tmp,__Texto[257],quita_path(v_mpfi(ipfi)->f_fichero),v_mpfi(ipfi)->f_titulo);
						   /*pinta_en_logger(log_tmp);*/
						   registro_logger("D",13,quita_path(v_mpfi(ipfi)->f_fichero),0,"",log_tmp,"","",quita_path(v_mpfi(ipfi)->f_oldfichero));
                        }
                        
                        modifica1_ficha(ipfi,nclave);
                        if (mimodo_debug || logger_salida)
                        {
                           sprintf(log_tmp,__Texto[258],quita_path(v_mpfi(ipfi)->f_fichero),v_mpfi(ipfi)->f_titulo);
                           /*pinta_en_logger(log_tmp);^*/
						   registro_logger("D",23,quita_path(v_mpfi(ipfi)->f_fichero),0,"",log_tmp,"","",quita_path(v_mpfi(ipfi)->f_oldfichero));
                        }
                        
                        continue;
                     }
                     else
                        if (opcion ==  8) {
                           ejecuta_otro(ipfi,4,O_NOR,0,0);                    
                           continue;
                        }
                        else
                           if (opcion ==  4) {
                              if (consulta_lee_prev(ipfi) < 0) 
							  {
                                 if (iserrno == EENDFILE) {
                                    Ar();
                                    aviso(0,__Texto[22]);
                                    Cr();
                                    leer_reg(ipfi,ISEQUAL,0,1);
                                 }
                              }
                              continue;
                           }
                           if (opcion ==  3) {
                              if (consulta_lee_next(ipfi) < 0) 
							  {
                                 if (iserrno == EENDFILE) {
                                    Ar();
                                    aviso(0,__Texto[23]);
                                    Cr();
                                    leer_reg(ipfi,ISEQUAL,0,1);
                                 }
                              }
                              continue;
                           }
                           if (opcion ==  6) 
						   {
                              consulta_lee_last(ipfi);
                              continue;
                           }
                           if (opcion ==  5) 
						   {
							  consulta_lee_first(ipfi);                              
                              continue;
                           }
                           break;
         }
         if (opcion == 1) {
            break;
         }
       }
     }
    }
    
    if (DespachadorExterno)
    {
       DespachoPantalla((int)DESPACHO_PANTALLAS,(int)1,v_mpfi(ipfi)->f_titulo);
    }
    
    
    Ar();Bl(4);Cr();
	recurre_pantalla(1);
    anula_mapa_raton(-1);
    recurre_raton(1);
    fin_botones_();	
	memcpy(def_erp_actual,_def_erp_actual,sizeof(def_erp_actual));ipfi_erp_actual = _ipfi_erp_actual;
    return(acabar);
}



modifica1_ficha(ipfi,nclave)
int ipfi,nclave;
{
   int salida = 0;
   int k;
   
   if (memoplin > 3) {
      k = atoi(v_memo(5003)->alfabetica);
      strcpy(v_memo(5003)->alfabetica,"303");
   }
   
   
   /* cambiar a clave primaria para leer la ficha */
   if (nclave) {
      cambia_ind_curr(v_mpfi(ipfi),0);
   }
   
   if (leer_reg(ipfi,ISEQUAL,ISLOCK,0) == 0) {
      if ((hay_dat_permiso(v_mpfi(ipfi),-1) & 4))
      {
         prin_pan(ipfi,0);
         if (v_mpfirel(ipfi)->clave > -1 &&
            cheq_clave(ipfi,v_mpfirel(ipfi)->clave,0) < 0)
         {
            salida = 0;
            TestigoRaton = 0;
         }
         else
            if (entra_datos(MODIF,ipfi,0) < 0)
            {
               salida = -1;
            }
            else {
               grabar_reg(ipfi,0,20);
               ejecuta_otro(ipfi,30,O_NOR,0,0);
            }
      }
      else
      {
         f_n(ipfi,RENBUF);
         libera_ind(v_mpfi(ipfi));   /* desbloquea el registro */
         aviso(0,__Texto[24]);
         salida = -1;
      }
   }
   else
   {
      TestigoRaton = 0;
      if (iserrno != ELOCKED) {Ar();aviso(0,__Texto[25]);Cr();}
   }
   
   f_n(ipfi,RENBUF);
   libera_ind(v_mpfi(ipfi));   /* desbloquea el registro */
   /* volver a la clave anterior */
   if (nclave) {
      cambia_ind_curr(v_mpfi(ipfi),nclave);
   }
   
   if (memoplin > 3) sprintf(v_memo(5003)->alfabetica,"%d",k);
   
   return(salida);
}

/*
* alta_ficha
* ejectuta la alta de fichas
* devuelve -1 en caso de salida con esc o ctrl-c
* o 0 si va bien
*/
alta_ficha(ipfi)
int ipfi;
{
   int leida,grabada,salida,clave = 0,j;
   
   limpia_ficha(ipfi);   
   
   j = pregun_clave(&clave,ipfi,0,ALTA);
   
   if (clave < 0 )
      return(-1);

   hay_dat_permiso(v_mpfi(ipfi),-3);
   if (!(hay_dat_permiso(v_mpfi(ipfi),-1) & 2))
			{
      aviso(0,__Texto[26]); 
      return(-1);
			}
   
   if (cambia_ind(v_mpfi(ipfi),1) < 0) {
      Ar();
      aviso(0,__Texto[27]);
      Cr();
      return(-1);
			}
   Bl(24);
   salida = 0;
   leida = leer_reg(ipfi,ISEQUAL,0,40);
   if (leida < 0) {      /* no existe, damos de alta */
      if (iserrno != ENOREC) {
         return(0);
      }
      if (grabar_reg(ipfi,3,20) < 0) {pito(113);return(0);}
      if (j == SOLICITA_POSICION)
      {
         if (!TestigoRaton)
            TestigoRaton = 1;
         else
         {
            pito(114);
            TestigoRaton = 0;
         }
      }
      if (entra_datos(ALTA,ipfi,0) < 0 || apli_aborto) {
         salida = -1;
         borrar_reg(ipfi,0,20);
      }
      else {
         grabar_reg(ipfi,0,20);
         ejecuta_otro(ipfi,30,O_NOR,0,0);
         if (va_con_doble_ && !es_la_doble_)
         {
            graba_el_doble(ipfi);
         }
      }
      libera_ind(v_mpfi(ipfi));
			}
   else {
      pinta_datos(ipfi,0);
      salida = -1;
			}
   return(salida);
}

/*
* modif_ficha
* ejecuta la modificacion de una ficha
* devuelve -1 si se sale con abort
* 0 en caso contrario
*/
modif_ficha(ipfi)
int ipfi;
{
			int nclave = 0,salida,j;
         
         j = entra_clave(ipfi,&nclave,MODIF);
         if (j < 0 && j != SOLICITA_POSICION) {
            return(-1);
         }
         Bl(24);
         
         if (cambia_ind(v_mpfi(ipfi),nclave+1) < 0) {
            Ar();
            aviso(0,__Texto[27]);
            Cr();
            return(-1);
         }
         if (j == SOLICITA_POSICION)
         {
            if (!TestigoRaton)
               TestigoRaton = 1;
            else
            {
               pito(115);
               TestigoRaton = 0;
            }
         }
         salida = modifica1_ficha(ipfi,0); /* en este caso no se debe cambiar */
         /* la clave de acceso a primaria! */
         return(salida);
}


/*
* baja_ficha
* ejecuta la baja de una ficha
* devuelve -1 si se sale con abort
* 0 en caso contrario
*/
int baja_ficha(ipfi)
int ipfi;
{
   int salida,j,nclave = 0;
   
   while(-1)
   {
      j = entra_clave(ipfi,&nclave,BAJA);		  
      if (j == SOLICITA_POSICION)
         pito(116);
      else
         break;
   }
   if (j < 0 )
      return(-1);
   if (!(hay_dat_permiso(v_mpfi(ipfi),-1) & 8))
   {
      aviso(0,__Texto[28]);
      return(-1);
   }
   if (cambia_ind(v_mpfi(ipfi),1) < 0) {
      Ar();
      aviso(0,__Texto[29]);
      Cr();
      return(-1);
   }
   Bl(24);
   salida = 0;
   if (leer_reg(ipfi,ISEQUAL,ISLOCK,0) == 0) { /* lee relaciones bloquea las que act. */
      pinta_datos(ipfi,0);				  
      if (confirmar(0,__Texto[433],__Texto[447]) < 0)
         salida = -1;
      else
         if (v_mpfirel(ipfi)->clave > -1 &&
            cheq_clave(ipfi,v_mpfirel(ipfi)->clave,0) < 0)
            salida = -1;
         else
         {
            if (baja_fichao(ipfi) < 0) {
               if (!apli_aborto) {
                  Ar();
                  aviso(0,__Texto[30]);
                  Cr();
               }
            }
            else {
               defecto_ficha(v_mpfi(ipfi));
               prin_pan(ipfi,0);
            }
         }
         
   }
   else
      if (iserrno != ELOCKED) {Ar();aviso(0,__Texto[31]);Cr();}
      libera_ind(v_mpfi(ipfi));   /* desbloquea el registro */
                                  /*
                                  if (vesylee)
                                  {
                                  magi = teipro();
                                  if (agitei == 1 )
                                  teiavisa();
                                  }
      */
      return(salida);
}

extern FI_DEF *pfi_llamada_erp;
int mante_ficha(ipfi,ficha,prime,qo)
int ipfi,ficha,prime;
char *qo;
{
   int opcion,bb;
   int opdef = 2;
   int leida,cerrar,i,j = 0;
   int campini,campfin;
   char opci[31];
   char log_tmp[128];
   char *l_textoi[10];
   char l_letrasi[10];
   /*FI_DEF *pfi_erp = pfi_llamada_erp;*/
   extern int balance;
	 char _def_erp_actual[25];
	 int _ipfi_erp_actual = ipfi_erp_actual;

	 memcpy(_def_erp_actual,def_erp_actual,sizeof(def_erp_actual));
	 strcpy(def_erp_actual,quita_path(v_mpfi(ipfi)->f_PathDef));
	 ipfi_erp_actual = ipfi;

   /*pfi_llamada_erp = NULL;*/

/*
static char letrasi[10] = __Texto[501];
static char *textoi[10] = {
      __Texto[496],
      __Texto[497],
      __Texto[498],      
      __Texto[499],
	  __Texto[500],
      "",
      "",
      "",
      "",
      ""
};
*/   

   for (i = 0;i < 5;i++)
   {
       l_textoi[i] = __Texto[496+i];
       l_letrasi[i] = __Texto[501][i];
   }
   for (;i < 10;i++)
   {
       l_textoi[i] = "";
       l_letrasi[i] = 0;
   }
   
   if ( !hay_dat_permiso(v_mpfi(ipfi),-1) )
   {
						if (qo)
                  {
                     sprintf(qo,__Texto[259],quita_path(v_mpfi(ipfi)->f_fichero));
                     aviso(0,qo);
                  }
						
		          memcpy(def_erp_actual,_def_erp_actual,sizeof(def_erp_actual));ipfi_erp_actual = _ipfi_erp_actual;

                  return(-1);
   }
   
   recurre_raton(0);
   
   if ((cerrar = f_v(ipfi,RNOCERRAR)) == -1) {
						f_s(ipfi,RNOCERRAR);     
                  if (abre1_rel(ipfi) < 0) {recurre_raton(1);memcpy(def_erp_actual,_def_erp_actual,sizeof(def_erp_actual));ipfi_erp_actual = _ipfi_erp_actual;return(-1);}
   }
   
   
   
   if ((leida = f_v(ipfi,RLEIDA)) == -1) f_s(ipfi,RLEIDA);
   

   
   /* empieza aqui mantenimiento normal o anormal */
   ejecuta_otro(ipfi,8,O_NOR,0,0);

   
   
   if (ficha != 1) 
   {
						cls();
                  Ar();Bl(4);Cr();
                  pinppan(ipfi,0);                  /* pintar pantalla */
   }
   else
						ficha = 0;
   if (ficha != -1 || !proceso[0]) 
   {
	   strcpy(proceso,v_mpfi(ipfi)->f_titulo);
   }
   /*cabecera_v(rw_version,proceso,titulo,empresa);*/
   cabecera(proceso,titulo,empresa);
   
   
   if (DespachadorExterno)
   {
		DespachoPantalla((int)DESPACHO_PANTALLAS,(int)1,v_mpfi(ipfi)->f_titulo);
   }
   
   
   if (pfi_llamada_erp && !strcmp(quita_path(pfi_llamada_erp->f_PathDef),quita_path(v_mpfi(ipfi)->f_PathDef)))
   {
	      memcpy(v_mpfi(ipfi)->f_buf,pfi_llamada_erp->f_buf,v_mpfi(ipfi)->f_lfich);		  
		  if (DespachadorExterno)
		  {	   
			 int total;
			 char *arrai[100];
			 char *puntero;
			 total = 0;
			 for (i = 0;i < v_mpfi(ipfi)->f_npanta;i++)
			 {
				puntero = v_mpfi(ipfi)->f_pantalla[i].p_nombrepan;
				puntero += strlen(puntero);		  
				if (*(puntero+1) == 1 && *(puntero+2) == 2)
				{
				   arrai[i] = puntero+3;
				   total++;
				}
			 }
			 if (total == v_mpfi(ipfi)->f_npanta)
			 {
				DespachoPantalla((int)DESPACHO_PANTALLAS,(int)total+100,(char *)arrai);
			 }
			 else
			 {
				DespachoPantalla((int)DESPACHO_PANTALLAS,(int)v_mpfi(ipfi)->f_npanta,v_mpfi(ipfi)->f_titulo);
			 }
		  }
		  _pinta_datos(ipfi,100,NULL,0);
   }
   else
   {
	   limpia_ficha(ipfi);
	   opcion = 1;
	   if (ficha == -1) 
	   {
				ficha = trata_1ficha(ipfi,qo,0,0);
	   }
	   else
					  if (prime > 0) 
					  {
						 ficha = consul_ficha(ipfi,-1,0);
					  }
					  else
						 if (v_mpfi(ipfi)->f_nclaves == 0) {
							if (ejecuta_otro(ipfi,10,O_NOR,0,0) != -1)
							   entra_datos(ALTA,ipfi,0);
						 }
						 else {
							while (opcion > 0 && !apli_aborto) 
							{
							   /*cabecera_v(rw_version,v_mpfi(ipfi)->f_titulo,titulo,empresa);*/
							   if (!proceso[0]) 
							   {
								   strcpy(proceso,v_mpfi(ipfi)->f_titulo);
								   cabecera(proceso,titulo,empresa);
							   }
                           
							   if (DespachadorExterno)
							   {
								  DespachoPantalla((int)DESPACHO_PANTALLAS,(int)1,v_mpfi(ipfi)->f_titulo);
							   }
                           
                           
							   if (v_mpfirel(ipfi)->clave < -1) {
								  /* ATENCION MANTENIMIENTO POR RUTINA (lineal) */
								  if (leida == -1) f_n(ipfi,RLEIDA);
								  sprintf(v_memo(5004)->alfabetica,"%d",2); /* a pelo modo de entrada de modificacion */
								  p_ejecuta(v_mpfirel(ipfi)->clave + 1,O_NOR,0,7);
								  if (leida == -1) f_s(ipfi,RLEIDA);
								  break;
							   }
                           
							   if (ejecuta_otro(ipfi,4,O_NOR,0,1) < 0)
							   {
								  *(l_letrasi + 4) = 0;
								  j = 4;
							   }
							   else
							   {
								  *(l_letrasi + 4) = 'I';                              
								  j = 5;
							   }                           

							   *(l_letrasi + j) = 0;

							   if (memoplin > 4)
							   {
								  if (ejecuta_otro(ipfi,20,O_NOR,0,1) >= 0)
								  {
									 strcpy(v_memo(5004)->alfabetica,"OPCION");
									 if (ejecuta_otro(ipfi,20,O_NOR,0,0) >= 0)
									 {
										*(l_letrasi + j) = v_memo(5004)->alfabetica[0];
										*(l_letrasi + j + 1) = 0;
										strcpy(opci,v_memo(5004)->alfabetica);
										l_textoi[j] = opci;
									 }
								  }
							   }

							   if (v_mpfi(ipfi)->f_npanta < 1)
							   {
								   BrowseData(ipfi,0,1+4+8+16,401,2581,0,0,0);
								   opcion = -1;
							   }
							   else
							   {                           
								   campini = v_mpfi(ipfi)->f_pantalla[0].p_campoini;  /* campo inicial */
								   campfin = v_mpfi(ipfi)->f_pantalla[0].p_campofin;  /* campo final */
								   /* poner campos para raton */
								   anula_mapa_raton(-1);
								   for (i = campini;i <= campfin;i++)
								   {
									  pon_campo_en_el_raton(ipfi,i,0);
								   }
								   TestigoRaton = 0;
                           
								   opcion = menusg(l_textoi,l_letrasi,0," SELECCIONE --> ",opdef);
								   if (opcion == SOLICITA_POSICION)
								   {
									  if (!TestigoRaton)
										 TestigoRaton = 1;
									  else
									  {
										 pito(117);
										 TestigoRaton = 0;
									  }
									  opcion = 3;
								   }
								   opdef = opcion;
								   if (opcion > 0) 
								   {
									  /*sprintf(proceso,"%s-%s",l_textoi[opcion-1],v_mpfi(ipfi)->f_titulo);*/
									  if (!proceso[0]) 
									  {
										  sprintf(proceso,"%s",v_mpfi(ipfi)->f_titulo);
									  }								  
									  /*cabecera_v(rw_version,proceso,titulo,empresa);*/
									  cabecera(proceso,titulo,empresa);
								   }
								   switch (opcion) {
								   case 1 :          /* alta */
									  if (mimodo_debug || logger_salida)
									  {
										 sprintf(log_tmp,__Texto[260],quita_path(v_mpfi(ipfi)->f_fichero),v_mpfi(ipfi)->f_titulo);
										 /*pinta_en_logger(log_tmp);*/
										 registro_logger("D",11,quita_path(v_mpfi(ipfi)->f_fichero),0,"",log_tmp,"","",quita_path(v_mpfi(ipfi)->f_oldfichero));
									  }
									  bb = balance;
									  while (alta_ficha(ipfi) == 0);
									  /*****
									  if (bb != balance) {
									  printf("\n\r no cuadra el balance de mallocs %d -> %d. \n\r",bb,balance);
									  getchar();
									  }
									  ******/
                              
									  if (mimodo_debug || logger_salida)
									  {
                                 
										 sprintf(log_tmp,__Texto[261],quita_path(v_mpfi(ipfi)->f_fichero),v_mpfi(ipfi)->f_titulo);
										 /*pinta_en_logger(log_tmp);*/
										 registro_logger("D",21,quita_path(v_mpfi(ipfi)->f_fichero),0,"",log_tmp,"","",quita_path(v_mpfi(ipfi)->f_oldfichero));
									  }
                              
									  break;
								   case 2 :          /* consulta */
									  consul_ficha(ipfi,0,0);
									  break;
								   case 3 :          /* modificacion */
									  if (mimodo_debug || logger_salida)
									  {
                                 
										 sprintf(log_tmp,__Texto[262],quita_path(v_mpfi(ipfi)->f_fichero),v_mpfi(ipfi)->f_titulo);
										 /*pinta_en_logger(log_tmp);*/
										 registro_logger("D",13,quita_path(v_mpfi(ipfi)->f_fichero),0,"",log_tmp,"","",quita_path(v_mpfi(ipfi)->f_oldfichero));
									  }
                              
									  while (modif_ficha(ipfi) == 0);
									  if (mimodo_debug || logger_salida)
									  {
                                 
										 sprintf(log_tmp,__Texto[263],quita_path(v_mpfi(ipfi)->f_fichero),v_mpfi(ipfi)->f_titulo);
										 /*pinta_en_logger(log_tmp);*/
										 registro_logger("D",23,quita_path(v_mpfi(ipfi)->f_fichero),0,"",log_tmp,"","",quita_path(v_mpfi(ipfi)->f_oldfichero));
									  }
                              
									  break;
								   case 4 :          /* baja */
									  if (mimodo_debug || logger_salida)
									  {
                                 
										 sprintf(log_tmp,__Texto[264],quita_path(v_mpfi(ipfi)->f_fichero),v_mpfi(ipfi)->f_titulo);
										 /*pinta_en_logger(log_tmp);*/
										 registro_logger("D",12,quita_path(v_mpfi(ipfi)->f_fichero),0,"",log_tmp,"","",quita_path(v_mpfi(ipfi)->f_oldfichero));
									  }
                              
									  while (baja_ficha(ipfi) == 0);
                              
									  if (mimodo_debug || logger_salida)
									  {
										 sprintf(log_tmp,__Texto[265],quita_path(v_mpfi(ipfi)->f_fichero),v_mpfi(ipfi)->f_titulo);
										 /*pinta_en_logger(log_tmp);*/
										 registro_logger("D",22,quita_path(v_mpfi(ipfi)->f_fichero),0,"",log_tmp,"","",quita_path(v_mpfi(ipfi)->f_oldfichero));
									  }
                              
									  break;
								   case 5:
									  if (j == 4 && memoplin > 4)
									  {
										 strcpy(v_memo(5004)->alfabetica,"");
										 ejecuta_otro(ipfi,20,O_NOR,0,0);
										 break;
									  }
									  if (mimodo_debug || logger_salida)
									  {
										 sprintf(log_tmp,__Texto[266],quita_path(v_mpfi(ipfi)->f_fichero),v_mpfi(ipfi)->f_titulo);
										 /*pinta_en_logger(log_tmp);*/
										 registro_logger("D",14,quita_path(v_mpfi(ipfi)->f_fichero),0,"",log_tmp,"","",quita_path(v_mpfi(ipfi)->f_oldfichero));
									  }
                              
									  ejecuta_otro(ipfi,4,O_NOR,0,0);
                              
									  if (mimodo_debug || logger_salida)
									  {
										 sprintf(log_tmp,__Texto[267],quita_path(v_mpfi(ipfi)->f_fichero),v_mpfi(ipfi)->f_titulo);
										 /*pinta_en_logger(log_tmp);*/
										 registro_logger("D",24,quita_path(v_mpfi(ipfi)->f_fichero),0,"",log_tmp,"","",quita_path(v_mpfi(ipfi)->f_oldfichero));
									  }
                              
									  break;
								   case 6:
									  if (j == 5 && memoplin > 4)
									  {
										 strcpy(v_memo(5004)->alfabetica,"");
										 ejecuta_otro(ipfi,20,O_NOR,0,0);
										 break;
									  }
								   default :
									  break;
								   }
							   }
                           
							   if (DespachadorExterno)
							   {
								  DespachoPantalla((int)DESPACHO_PANTALLAS,(int)1,v_mpfi(ipfi)->f_titulo);
							   }                           
					}
			  }
		  }
          
          ejecuta_otro(ipfi,9,O_NOR,0,0);
          
          
          if (DespachadorExterno)
          {
             DespachoPantalla((int)DESPACHO_PANTALLAS,(int)1,"");
          }
          
          
          if (leida == -1) f_n(ipfi,RLEIDA);
          if (cerrar == -1) {
             f_n(ipfi,RNOCERRAR);
             cierra1_rel(ipfi);
          }
		  
          anula_mapa_raton(-1);
          recurre_raton(1);
		  memcpy(def_erp_actual,_def_erp_actual,sizeof(def_erp_actual));ipfi_erp_actual = _ipfi_erp_actual;
          return(ficha);
}

#endif