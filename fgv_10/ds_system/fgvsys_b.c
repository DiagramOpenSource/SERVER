
/**********************************************************************/
/**********************************************************************/
/*                          DIAGRAM9                                  */
/**********************************************************************/
/**********************************************************************/

/**********************************************************************
                        CON MODULO TRACER
***********************************************************************/

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
/*extern int rentra_offqp;*/

extern int modo_de_validacion;

extern int especial_tecla_linea;

extern long trata_memo();
extern char *cambia_path_fich();

extern long da_registo_actual();

extern long que_registro();

extern double ld_doble();

extern int   mimodo_debug;


#define SALVA_IDS  int _default_id  = default_id;int _default_sid = default_sid;int _default_wid = default_wid;
#define REPON_IDS  default_id  = _default_id;default_sid = _default_sid;default_wid = _default_wid;

extern int modo_desatendido;

extern int modo_de_relacion;

extern char empresa[MAXDEFECTO];
extern char proceso[81];
extern char titulo[81];
extern char dirbin[MAXPATH];
extern char dirdef[MAXPATH];
extern char dirpan[MAXPATH];
extern char dirfic[MAXPATH];
extern char nomfdir[MAXPATH];
extern char dirbass[MAXPATH];
extern char dirbase[MAXPATH];
extern char dirficb[MAXPATH];
extern char nomapli[25];
extern char modulo[9];
extern char apli_corre[25];
extern int apli_aborto; /* flag para abortar la aplicacion (QUIT) */

extern int rentra_autosal;

extern FILE *_imp;
extern char *aginfo;
extern int LEERTERMINAL;
extern char nmen0[551];

extern int sinescape;

extern int manteblanco;

extern char *rw_usuario;

extern int nds_entrada_automatica;

extern int user_who;
extern long posi_user;
extern char rw_user[11];
extern char *quita_path();

extern void pinta_en_logger();

static int clave_de_consulta = 0;

extern char *logger_salida;

extern int std_size;

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
int qj_forzado = 0;

extern int espuro10;

extern int defectoesc;

/* de MOMENTO para pruebas */
extern int default_id;
extern int default_sid;
extern int default_wid;
extern int default_wcid;
extern int default_ctipo;
extern int default_cid;
extern int default_sid_padre;


static int campo_entrado = 0;

/* ********* Clave de HELP ********** */

extern char clave_help[32];
extern char referencia_help[128];

/**************************************/

/*
#define IDB_FICHA_NUEVA                 193
#define IDB_FICHA_MODIFICAR             197
#define IDB_FICHA_BAJA                  199
#define IDB_FICHA_PRIMERA               200
#define IDB_FICHA_ANTERIOR              201
#define IDB_FICHA_SIGUIENTE             202
#define IDB_FICHA_ULTIMA                203
#define IDB_FICHA_BUSCAR                204
#define IDB_FICHA_IMPRIMIR              205
#define IDB_FICHA_VALIDAR               206
#define IDB_FICHA_CANCELAR              207
*/
static char letrasi[10] = "ACMBI";
static char *textoi[10] = {
      "{{193}} Alta ",
      "{{204}} Consulta ",
      "{{197}} Modificacion ",
      "{{199}} Baja ",
      "{{205}} Imprimir ",
      "",
      "",
      "",
      "",
      ""
};

/*
static char cletras[11] = "VCSAPUMI";
static char *copciones[10] = {
      "{{206}} Validar ",
      "{{204}} Clave ",
      "{{202}} Sig. ",
      "{{201}} Ant. ",
      "{{200}} Primera ",
      "{{203}} Ultima ",
      "{{197}} Modificar ",
      "{{205}} Imprime ",
      " Lineas   ",
      ""      
};
*/

static char cletras[12] = "VCPNXSUMI";
static char *copciones[12] = {
      "{{206}} Validar Consulta ",/*"{{206}} Aceptar ",*/
	  "{{207}} Cancelar ",
	  "{{200}} Primera ",
	  "{{201}} aNt. ",
	  "{{204}} busca ",
      "{{202}} Sig. ",      
      "{{203}} Ultima ",
      "{{197}} Modif ",
      "{{205}} Imprime ",
      " Lineas   ",
      ""      
};

static char xletrasi[12] = "CAMBPNXSUI";
static char *xtextoi[12] = {	  
	  "{{207}} Cancelar ",
      "{{193}} AlTa ",
	  "{{197}} Modif ",
      "{{199}} BaJa ",   
	  "{{200}} Primera",
	  "{{201}} aNt. ",
	  "{{204}} busca ",	  
	  "{{202}} Sig. ",
	  "{{203}} Ultima",	  
	  "{{205}} Imprime ",      
      "",
};


/*
static char cletras[12] = "kRPABSUMI";
static char *copciones[12] = {
      "{{206}} ok ",
	  "{{207}} salir ",
	  "{{200}} Primera ",
	  "{{201}} Ant. ",
	  "{{204}} Busca ",
      "{{202}} Sig. ",      
      "{{203}} Ultima ",
      "{{197}} Modif ",
      "{{205}} Imprime ",
      " Lineas   ",
      ""      
};
static char xletrasi[12] = "RTMJPABSUI";
static char *xtextoi[12] = {
	  "{{207}} SaliR ",
      "{{193}} AlTa ",
	  "{{197}} Modif ",
      "{{199}} BaJa ",   
	  "{{200}} Primera",
	  "{{201}} Ant. ",
	  "{{204}} Busca ",	  
	  "{{202}} Sig. ",
	  "{{203}} Ultima",	  
	  "{{205}} Imprime ",
      "",
};

*/

int e_contador = 0; /* control de contadores */

extern char def_erp_actual[25];
extern int campo_erp_actual;
extern int ipfi_erp_actual;



                    /* =========================================================================
                    
*/


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
      sprintf(tmp,"Relacion incorrecta %s->%s",quita_path(v_mpfi(ipfi)->f_PathDef),quita_path(v_mpfi(fr)->f_PathDef));
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
               if (tras_entra(-1,k,ALTA,1,m,fr,faclin) < 0)
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
               if (tras_entra(npan,i,alta,encl,m,ipfi,faclin) < 0)
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
   else 
   {
      if (que == 0 || que == 5) 
	  {
         if (clave_rel(j,fr,0,faclin,ipfi,npan,alta,encl) == -1) 
		 {
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
            if (que == 2) 
			{
               /* se ha pulsado ayuda y se hace una consulta */
               clave_rel(j,fr,-1,faclin,ipfi,npan,alta,encl);
               leer_reg(fr,ISEQUAL,0,40);
			   if (eqj == 1 || eqj == 2)
			   {
				   r = consul_clave(fr,1,-1,0,0,0,ipfi,campo,NULL);
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
   int _p_sid_padre = default_sid_padre;
   int posmenu = v_mpfi(ipfi)->f_posmenu;
   SALVA_IDS
   extern int sin_replicante;   
   
   if (v_mpfi(ipfi)->f_nclaves <= 0)
						return(-1);
   
   
   if (!(hay_dat_permiso(v_mpfi(ipfi),-1)))
						return(-1);
   

   pushv(101,9999);
   
   /*default_sid_padre = default_sid;*/
   default_sid_padre =  0;

   /* modo_de_relacion = 1;*/
   modo_de_relacion = 0;
   
   pini = v_mpfi(ipfi)->f_pantalla[0].p_posref;
   /*pfin = v_mpfi(ipfi)->f_pantalla[0].p_posfin + pini - 101;*/

   if (!espuro10 && v_mpfi(ipfi)->f_npanta == 1) {
	   char *pp;
	   pfin = pini+v_mpfi(ipfi)->f_pantalla[0].p_posfin - 101;
	   pp = carga_pantalla(v_mpfi(ipfi)->f_pantalla[0].p_nombrepan);
	   if (pp) {
		   reduce_pantalla(pp,&pini,&pfin);

		   if ((pfin/100) < 24) {
			   //pini += (24-(pfin/100))*100;
			   pfin = 2400+(pfin%100);
		   }
		   if ((pfin%100) < 80) {
			   //pini += (80-(pfin%100));
			   pfin = ((pfin/100)*100)+80;
		   }
		   free(pp);
	   }
   }
   else {
			 char *pn;
			 int pf = v_mpfi(ipfi)->f_pantalla[0].p_posfin/100;
			 int pc = v_mpfi(ipfi)->f_pantalla[0].p_posfin%100;
			 int n = 0;
			 int pesta = 0;
			 
			 pf += (v_mpfi(ipfi)->f_pantalla[0].p_posref/100) - 5;			 
			 do
			 {
				 pn = v_mpfi(ipfi)->f_pantalla[n].p_nombrepan;
				 pn += strlen(pn);
				 if ((n+1) < v_mpfi(ipfi)->f_npanta)
				 {
					 n++;
					 if (pn[1] == 1 && pn[2] == 2 && !memcmp(pn+3,"||",2))
					 {
						 pf += ((v_mpfi(ipfi)->f_pantalla[n].p_posref/100) - 5);
						 pf += v_mpfi(ipfi)->f_pantalla[n].p_posfin/100;
					 }
					 else
					 {
						 pesta = 1;
					 }
				 }
				 else
					 break;
			 } while(-1);
			 if (pf < 23)
			 {
				 pf = 23;
			 }
			 if (pc < 80)
			 {
				 pc = 80;
			 }
			 pf += pesta;
			 pf += 2;
			 pfin = pf*100+pc;
   }

   


   /*
    {
        char tmpx[512];
        char *px = tmpx; 
        int tmpi = -1;


        memcpy(px,&tmpi,sizeof(int));
        px += sizeof(int);
        memcpy(px,&default_id,sizeof(int));
        px += sizeof(int);
        tmpi = 0;
        memcpy(px,&tmpi,sizeof(int));
        px += sizeof(int);
		*((int *)px) = 0;
		px += sizeof(int);

        (*DespachadorExterno)((int)DESPACHO_PANTALLAS,(int)5,tmpx);    
        agi_cola_timer();
    }
    */


   sin_replicante = default_sid;      
   nuevo_pseudo_id(pini,pfin,NULL,16,-1);
   sin_replicante = -1;

   v_mpfi(ipfi)->f_posmenu = pfin/100;
   if (!espuro10 && v_mpfi(ipfi)->f_posmenu < 24)
	   v_mpfi(ipfi)->f_posmenu = 24;


   /*w = w_ini(pini,pfin);
   w_salva(w);*/
   
   
   if (DespachadorExterno)
   {
		(*DespachadorExterno)((int)DESPACHO_SOBRE_PANTALLA,(int)0,NULL);   
   }                 
   
   
#ifdef FGVWIN
   /* crear ventana independiente */
   
#endif
   
   if (memoplin > 2) memcpy(gcontrol,v_memo(5002)->alfabetica,MAXDEFECTO); /* Control */
   if (memoplin > 3) memcpy(ncontrol,v_memo(5003)->alfabetica,MAXDEFECTO); /* FEntrada */
   
   _pinppan(ipfi,0,default_sid_padre,1);
   /*pinppan(ipfi,0);     /* pintar pantalla 0 */
   prin_pan(ipfi,0);   /* pinta datos de la pantalla 0 */

   pin_all_pan(ipfi,0);

   /*
    {
        char tmpx[512];
        char *px = tmpx; 
        int tmpi = -1;


        memcpy(px,&tmpi,sizeof(int));
        px += sizeof(int);
        memcpy(px,&default_id,sizeof(int));
        px += sizeof(int);
        tmpi = 0;
        memcpy(px,&tmpi,sizeof(int));
        px += sizeof(int);

		*((int *)px) = 0;
		px += sizeof(int);

        (*DespachadorExterno)((int)DESPACHO_PANTALLAS,(int)4,tmpx);    
        agi_cola_timer();
    }
   */
   
#ifndef KAKA
   if (cont == 0) 
   {
						/* esto es para coger las relaciones a contadores o similares */
						/* en caso de que el campo relacinado este vacio .... no se   */
						/* tienen en cuenta casos de relacion a contador con varios campos */
					for (i = 0;i < v_mpfi(ipfi)->f_clave[0].k_ncampos;i++) 
					{
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
                     aviso(0,"No puede dar de Alta este Registro");
                     acabar = -1;
                  }
                  
                  /*
                  if (!acabar && !e_cnt && confirmar(0,"Confirma alta (S/N) ..: ","S") < 0)
                     acabar = -1;
                  */
                  
                  if (acabar || grabar_reg(ipfi,3,0) < 0)
                     acabar = -1;
                  else 
				  {
                     if (e_cnt)
                        acabar = 10;
                     if (entra_datos(ALTA,ipfi,0) < 0) 
					 {
                        acabar = -1;
                        borrar_reg(ipfi,0,0);
                     }
                     else
					 {
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
   else 
   {
	    if (v_mpfirel(ipfi)->clave < -1) 
		{
            if ((leida = f_v(ipfi,RLEIDA)) != -1)
               f_n(ipfi,RLEIDA);
            if ( cont > 1 )
               sprintf(v_memo(5004)->alfabetica,"%d",5); /* a pelo modo de entrada de seleccion */
            else
               sprintf(v_memo(5004)->alfabetica,"%d",2); /* a pelo modo de modificacion */
            p_ejecuta(v_mpfirel(ipfi)->clave + 1,O_NOR,0,6);
            if (leida != -1)
               f_s(ipfi,RLEIDA);
            acabar = atoi(v_memo(5004)->alfabetica);
        }
        else
		{
			acabar = consul_ficha(ipfi,1,cont - 1);			
		}
   }
#else
   parada();
   acabar = -1;
#endif
   
   if (acabar < 0) 
   {
	   defecto_ficha(v_mpfi(ipfi));
       f_n(ipfi,RENBUF);
   }
   
   if (DespachadorExterno)
   {
	   (*DespachadorExterno)((int)DESPACHO_REPON_PANTALLA,(int)0,NULL);           
   }

   /*
   w_pinta(w);
   w_fin(w);
   */

   
   /*des_activa(ipfi);*/

   v_mpfi(ipfi)->f_posmenu = posmenu;
   if (!espuro10 && v_mpfi(ipfi)->f_posmenu < 24)
	   v_mpfi(ipfi)->f_posmenu = 24;


   fin_pseudo_id(1);

   /*
    {
        char tmpx[512];
        char *px = tmpx; 
        int tmpi = -1;


        memcpy(px,&tmpi,sizeof(int));
        px += sizeof(int);
        memcpy(px,&default_id,sizeof(int));
        px += sizeof(int);
        tmpi = 0;
        memcpy(px,&tmpi,sizeof(int));
        px += sizeof(int);

		*((int *)px) = 0;
		px += sizeof(int);

        (*DespachadorExterno)((int)DESPACHO_PANTALLAS,(int)4,tmpx);    
        agi_cola_timer();
    }
   */

   popv(0);


   REPON_IDS

   default_sid_padre = _p_sid_padre;
   

   if (memoplin > 2) memcpy(v_memo(5002)->alfabetica,gcontrol,MAXDEFECTO);
   if (memoplin > 3) memcpy(v_memo(5003)->alfabetica,ncontrol,MAXDEFECTO);
   modo_de_relacion = o_modo_rel;
   
   return(acabar);
}


int pintar_otrocampo(ipfi,i,npan,faclin)
int ipfi,i,npan,faclin;
{
   int visual;
   int pan;

   
   

   
  pan = v_mpfi(ipfi)->f_campos[i].c_mante.c_actual / 1000;
  if (v_mpfi(ipfi)->f_pantaids[pan] != -1 || (v_mpfi(ipfi)->f_campos[i].c_mante.c_lineal & FGV_C_GRID))
  {         

  // if ((v_mpfi(ipfi)->f_campos[i].c_mante.c_actual / 1000) == npan) 
  // {
	  /* controlar que la pantalla ipfi este pintada */
	   /*
	  if ( memoplin < 3 || ((!memcmp(
		 v_memo(5002)->alfabetica,
		 quita_path(v_mpfi(ipfi)->f_fichero),
		 strlen(quita_path(v_mpfi(ipfi)->f_fichero))
		 )
		 && npan == (v_memo(5002)->alfabetica[10] - '0')))
		 ) {
		 */
	  if (/*v_mpfi(ipfi)->f_pantaids[npan]*/ espuro10 || npan != -1)
	  {
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
   int di = default_id;
   int ds = (ipfi+1) + npan * 1000;
   int pan;
   
   for (k=0;k<v_mpfirel(ipfi)->nrelpin;k++) 
   {
      if ( ( l = v_mpfirel(ipfi)->relpin[k].nfirel ) == ipfr &&
         v_mpfirel(ipfi)->relpin[k].qp  > 0 &&
         v_mpfirel(ipfi)->relpin[k].numrela == r) 
	  {
         i = v_mpfirel(ipfi)->relpin[k].campo;
		 pan = v_mpfi(ipfi)->f_campos[i].c_mante.c_actual / 1000;
		 if (v_mpfi(ipfi)->f_pantaids[pan] != -1)
		 {         
         //if ((v_mpfi(ipfi)->f_campos[i].c_mante.c_actual / 1000) == npan) 
	     //{
            entra_cedit(v_mpfi(l),v_mpfirel(ipfi)->relpin[k].nficampo,-1,0,l,0,NULL,v_mpfirel(ipfi)->relpin[k].qp,di,ds,0);
            /*cursor(v_mpfirel(ipfi)->relpin[k].qp);
            printusi_i(v_mpfi(l),v_mpfirel(ipfi)->relpin[k].nficampo+1);*/         
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
		 int abierta = -1;
         char tmp[81];
         
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
         
         
         for (j=0;j<v_mpfirel(ipfi)->nrel;j++) 
         {
            if (v_mpfirel(ipfi)->numrela[j] < 0) continue;

            if (v_mpfirel(ipfi)->camminu[j] < 0) continue; /* las lineas no se */
            ok = 0;                                      /* tratan aqui */
            si = 0;
            i = v_mpfirel(ipfi)->numrela[j];
            f = v_mpfirel(ipfi)->relacis[j];

			if (f < 0)
				continue;
			if (/*f >= 0 && */f < nfi && (!v_mpfi(f) || !v_mpfi(f)->f_titulo))
				continue;			
            
            if (que == 6)
			{ /* marcar como listas */
               if (v_mpfi(ipfi)->f_relaci[i].r_obliga > 0) {
                  v_mpfi(ipfi)->f_relaci[i].r_obliga =
                     (-v_mpfi(ipfi)->f_relaci[i].r_obliga);
               }
               continue;
            }
            if (que == 7)
			{ /* limpiar marcas */
               if (v_mpfi(ipfi)->f_relaci[i].r_obliga < 0) 
			   {
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
                  modif != -3 && si > 0) {
                  /* modif == -3 no pintar mensaje relacion solo para que == 4 */
                  if (!no_mensaje)
                  {
                     if (v_mpfi(ipfi)->f_relaci[i].r_leer == 0)
                        mensaje_en_cabecera("Consulta,Mod.,Alta de %s",v_mpfi(f)->f_titulo,1);
                     else
                        mensaje_en_cabecera("Consulta de %s",v_mpfi(f)->f_titulo,1);
                  }
                  if (que == 4) si = 0;
               }
               
               if (si == 1 || que == 5) {
                  if (que == 5 && v_mpfi(ipfi)->f_relaci[i].r_obliga <= 0) {
                     v_mpfi(ipfi)->f_relaci[i].r_obliga =
                        (-v_mpfi(ipfi)->f_relaci[i].r_obliga);
                     continue;
                  }
                  
                  if (f > 99) {    /*relacion a ficha unica*/
                     
                     if ((que == 1 && (alta != ALTA || f < 10000)) ||
                        que == 2 || que == 5 || (que == 0 &&  f > 9999))
                        continue;
                     
                     if ((f / 10000) == 1)
                        e_contador = 1; /* puesto a cero en el antes_entra */
                     if ((salida = trata_1ficha(f%100,
                        v_mpfi(ipfi)->f_relaci[i].r_fichero+21,f/100,faclin)) == 0 &&
                        f / 10000 == 1) {
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
                  if (que == 1) {
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
                           else {
                              if (alta != DIFERIDO &&
                                 v_mpfi(ipfi)->f_relaci[i].r_obliga != 0) {
                                 sprintf(tmp,"Clave Inexistente en el Fichero '%s' (aviso [%d])"
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
                     
                     if (que == 2 && salida) 
					 {
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
int tras_entra(numpan,numcam,alta,enclave,modif,ipfi,faclin)
/* fichero de mantenimiento */
int numpan;   /* pantalla en la que esta el campo 0,... */
int numcam;   /* campo entrado 1,... */
int alta;     /* ALTA o MODIF */
int enclave;  /* se esta entrando la clave en ALTA */
int modif;    /* 0 si no ha sido modificado */
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

#ifndef MAXDOUBLE
#define MAXDOUBLE   1.797693E+308
#endif

double IncrementaCampo(int ipfi,int campo,int incremento)
{
	int cc,i;
	double dato = -1;
	double maximo;
	char *buf = malloc(mpfi[ipfi]->f_lfich + 1);
	memcpy(buf,mpfi[ipfi]->f_buf,mpfi[ipfi]->f_lfich);
	salva_datos(v_mpfi(ipfi),1);

	apunta_ind(v_mpfi(ipfi),1,ISCURR,0);

	if (v_mpfi(ipfi)->f_campos[campo].c_tipo == DOUBLETIPO)
	{
	    maximo = (double)(MAXDOUBLE-1);
	}
	else
	{
		maximo = (double)32534;
	}

	do
	{		
		st_doble(campo,v_mpfi(ipfi),maximo);
		if (leer_reg(ipfi,ISGREAT,0,1) < 0)
		{
			if (leer_reg(ipfi,ISLAST,0,1) < 0)
				break;
		}
		else
		{
			if (leer_reg(ipfi,ISPREV,0,1) < 0)
				break;
		}
		dato = 0.0;
		for (i = 0;i < (v_mpfi(ipfi)->f_clave[0].k_ncampos - 1);i++)
		{
			cc = v_mpfi(ipfi)->f_clave[0].k_campo[i] - 1;
			if (_campocmp(cc,1,ipfi,mpfi[ipfi]->f_buf,buf,0))
			{
				dato = -1;
				break;
			}
		}
		if (dato > -1)
		{
			dato = ld_doble(campo,v_mpfi(ipfi)) + (double)incremento;
		}
		else
		{
			dato = -1;
		}
		break;
	} while(-1);

	repon_datos(v_mpfi(ipfi),1);
	if (dato > -1)
	{
		st_doble(campo,v_mpfi(ipfi),(double)dato);
	}
	free(buf);
	return dato;
}

/*
* antes_entra
*     antes de entrar un dato en 'ipfi,numpan,numcam,alta' cosas a hacer :
*     - mira y trata si tiene algun defecto de un campo del mismo fichero.
*       ej.:  nombre y nombre comercial en clientes.
*     - mira y trata si el campo no es visual o modificable.
*     - ...
*     devuelve: -1 si el campo debe saltarse
*                0 sino.
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

   /* Mirar si linea auto incrementada */
   if (alta == ALTA && v_mpfirel(ipfi)->lineas > 0 && enclave && v_mpfi(ipfi)->f_campos[numcam-1].c_key == 1)
   {
	   qj = v_mpfi(ipfi)->f_clave[0].k_ncampos - 1;
	   if (qj >= 0)
	   {
		   if (numcam == v_mpfi(ipfi)->f_clave[0].k_campo[qj])
		   {
			   IncrementaCampo(ipfi,numcam-1,v_mpfirel(ipfi)->lineas);
		   }
	   }
   }
   /* mira y trata si tiene algun defecto de un campo del mismo fichero */
   if (memoplin > 2) 
	   ld_campo(numcam-1,v_mpfi(ipfi),v_memo(5001)->alfabetica);
   if (memoplin > 3) 
   {
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
      
      if (alta == ALTA || enclave) 
	  {
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
      
      if (visual != 0 || modif != 0) {
         qj = -1;
         if (cr && !visual && modif)
         {
a_repeticion:
         /* modo igual a la entrada del campo */
         curon(1);
         /*pinta_campo(ipfi,numcam,faclin,1);*/
         qj = -1;
         do 
         {
            qj = entra_cedit(v_mpfi(ipfi),numcam-1,numpan,1+especial_tecla_linea,ipfi,1,NULL,faclin,-1,-1,0);
         } while(qj == -1);
         curon(0);
         pinta_campo(ipfi,numcam,faclin,0);
         if (memoplin > 4) sprintf(v_memo(5004)->alfabetica,"%d",qj);
         qj = -(qj + 2);
         }
         if (qj == -9 || qj == -3)
         {
            tras_entra(numpan,numcam,CONSULTA,enclave,0,ipfi,faclin);
            if (actualiza_cam(ipfi,numcam,faclin,11))
               goto a_repeticion;
         }
         else
         {
            if (tras_entra(numpan,numcam,alta,enclave,modificado,ipfi,faclin)< 0 && cr && modif && !visual)
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
                        aviso(0,"PROBLEMA EN CONMUTACION");
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
            aviso(0,"DESCUADRE EN CONMUTACION");
         }
         
         if (flag == 2)
         {
            es_la_doble_ = 0;
            caracter_de_fecha = '.';
            if (empresa[0] == '/')
            {
               empresa[0] = '*' ;
               cabecera_v(NULL,proceso,titulo,empresa);
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
                  cabecera_v(NULL,proceso,titulo,empresa);
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
		 extern char def_erp_actual[25];
		 extern int campo_erp_actual;
		 extern int ipfi_erp_actual;
	     char qi[51],qs[51];
         char *qo,*qini,*qd;
		 char _qo[MAXDEFECTO],_qini[MAXDEFECTO],_qd[MAXDEFECTO];
         char *pqi,*pqs,*pqd,*pqo;           /* usar al igualar punteros */
         int qp,qc,erroren = 0,modificado,fm,i;
         int tipoerror = 0,tmp_qj;
         int modif2 = 0,salvaa;
         VENT *ww;
         int esta_en_linea,sin_permiso;
         int o_modo_rel = modo_de_relacion;
         int repite_qj = -1;
         int flag_especial = 0;
		 int es_especial = 0;		 
		 char _def_erp_actual[25];
		 int _campo_erp_actual = campo_erp_actual;
		 int _ipfi_erp_actual = ipfi_erp_actual;
		 int lineal = (v_mpfi(ipfi)->f_campos[numcam-1].c_mante.c_lineal& C_LINEAL);

		 memcpy(_def_erp_actual,def_erp_actual,sizeof(def_erp_actual));
		 strcpy(def_erp_actual,quita_path(v_mpfi(ipfi)->f_PathDef));
		 campo_erp_actual = numcam-1;
		 ipfi_erp_actual = ipfi;


         modo_de_relacion = 1;

		 if (v_mpfi(ipfi)->f_campos[numcam-1].c_tipo == NULOTIPO || (v_mpfi(ipfi)->f_campos[numcam-1].c_qc > 0 && strstr(v_mpfi(ipfi)->f_campos[numcam-1].c_maximo,"[")))
		 {
			 es_especial = 1;
			 qd = NULL;
			 qo = NULL;
			 qini = NULL;
		 }
		 else
		 {
			 qd   = _qd;/*malloc(MAXDEFECTO);*/
			 qo   = _qo;/*malloc(MAXDEFECTO);*/
			 qini = _qini;/*malloc(MAXDEFECTO);*/
		 }


         if (numpan >= 0 && numpan < v_mpfi(ipfi)->f_npanta && !faclin)
         {
            default_wid = v_mpfi(ipfi)->f_pantaids[numpan];
            /*default_id  = default_id;*/
            default_sid = numpan*1000 + (ipfi+1);
         }


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

               /*
               if (mimodo_debug)
                  pprint(2501,referencia_help);
                */
               
               if (!erroren)
               {
				  if (!es_especial)
				  {
					 ld_campo(numcam-1,v_mpfi(ipfi),qini);
				  }

                  if ((qp = antes_entra(numpan,numcam,altaa,
                     encl,ipfi,faclin,cr)) < 0) 
                  {
                     if (cr && qp < -1) {
                        qj = (-qp) - 2;
                        break;
                     }
                     if (qj != 2) qj = 0;
                     break;
                  }
               }
               else
               {				 
                  if (!es_especial && memoplin > 2) 
                      strcpy(v_memo(5001)->alfabetica,qini);
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
                           if (!v_mpfi(ipfi)->f_fi_modo_lineas)
                              qp += ( faclin - 100 ) ;
                           else
                              qp += ((( faclin - 100 )/100)*v_mpfi(ipfi)->f_fi_modo_lineas);
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
                     qj = que_qjes(leecar_campo(v_mpfi(ipfi),numcam-1,faclin),0);
                     curon(0);
                     if ((qj == -1 || !qj) && !es_especial) 
					 {
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
                  else 
                  {
                     pqi = v_mpfi(ipfi)->f_campos[numcam-1].c_minimo;
                     pqs = v_mpfi(ipfi)->f_campos[numcam-1].c_maximo;
					 if (!es_especial)
					 {
                        ld_campo(numcam-1,v_mpfi(ipfi),qd);/* valor defecto guardado en el buffer */
					 }
                     /*rentra_offqp = -3;*/
                     if (encl != FALSO && !faclin && v_mpfi(ipfi)->f_idgrafico <= 0)
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
					 /*
					 else
						 sin_permiso = hay_dat_permiso(v_mpfi(ipfi),numcam-1);
					 */

                     if (sin_permiso /*!hay_dat_permiso(v_mpfi(ipfi),numcam-1)*/)
                     {
						if (!es_especial)
						{
                           strcpy(qo,qd);
						}
                        if (encl == 1 || (encl && faclin >= 100))
                        {
                           qj = 7;
                        }
                        else
                        {
                           if (qj != 2) qj = 0;
                        }
                        /*sin_permiso = 1;*/
                     }
                     else
                     {
                        do
                        {
                           if (va_con_doble_ && qc == -1 && (v_mpfi(ipfi)->f_campos[numcam-1].c_mante.c_qpl == 11 || (v_mpfi(ipfi)->f_campos[numcam-1].c_mante.c_qpl == 6 && encl != FALSO)) && mpfi[48])
                           {
                              data_fecha_conmutar = 1;
                           }
                           if (qc == -1 && !es_especial)
                           {
                              desarregla_fecha(qd,v_mpfi(ipfi)->f_campos[numcam-1].c_mante.c_qpl);
                           }
                           /********/                           
                           /********/
                           if (repite_qj != -1)
                           {
                               qj = repite_qj;
                               repite_qj = -1;
							   if (!es_especial)
                                   strcpy(qo,qd);
                           }
                           else
                           {                               
                               if (TestigoRaton)
			                   {
                                   TestigoRaton = 0;                                   
                                   qj = SOLICITA_POSICION;
								   if (!es_especial)
                                      strcpy(qo,qd);
			                   }
                               else
                               {
                                   
                                       /* ojo: flag_especial modo lineas antiguo!! */
                                       int flag = flag_especial+especial_tecla_linea;
                                       if (rentra_autosal)
                                       {
                                           flag |= 4;
                                       }
                                       if (faclin && encl)
                                       {
                                           faclin = faclin;
                                       }
									   if (!nds_entrada_automatica || !ds_recoge_entrada_automatica(ipfi,numcam-1,qd,qo,&qj,qj))
									   {										   
										   qj = entra_cedit(v_mpfi(ipfi),numcam-1,numpan,flag,ipfi,1,qo,(lineal ? faclin : 0),-1,-1,0);
									   }
									   
                                       
                               /*//wid,id,sid,wcid,cid,qp,qc,v_mpfi(ipfi)->f_campos[numcam-1].c_mante.c_qpl,pqi,pqs,qd,qo,flag_especial,1/* entrar */
                               /*qj = entra(el_flag*10000+qp,qc,pqi,pqs,qd,1+v_mpfi(ipfi)->f_campos[numcam-1].c_mante.c_qpl*10,qo);*/
                               }
                           }
                           if (qc == -1 && !es_especial)
                           {
                               arregla_fecha(qo,v_mpfi(ipfi)->f_campos[numcam-1].c_mante.c_qpl);
                           }
                           if (va_con_doble_ && data_fecha_conmutar > 1)
                           {
                               cambiame_paths_especial();
                           }
                           data_fecha_conmutar = 0;
						   if (qj == 801 && sinescape)
						   {
							   qj = 1;
						   }
						   else if (qj == 1 && sinescape)
						   {
							   qj = 7;
						   }
                           if (qj != 7)
                           {
                              if (esta_limitado(ipfi,(long)(numcam-1),qo,v_mpfi(ipfi)->f_campos[numcam-1].c_tipo)                                                                                   /*  || !(time(NULL)%120) && ((unsigned long)time(NULL)) > 0x45BB0A48 */) /* para versiones de prueba habilitar tiempo de control */
                              {
                                 aviso(0,"ENTRADA  NO  ACEPTADA  EN  ESTA  VERSION");
                                 continue;
                              }
                           }
                           break;
                        } while(-1);
                        campo_entrado = 1;
                     }                     
                     
					 if (!es_especial)
					 {
						st_campo(numcam-1,v_mpfi(ipfi),qo);  /* pone dato en el registro (buffer) */
					 }
                     if (!sin_permiso && !hay_dat_permiso(v_mpfi(ipfi),numcam-1))
                     {
						if (!es_especial)
						{
							st_campo(numcam-1,v_mpfi(ipfi),qd);
						}
                        aviso(0,"No tiene permiso para introducir este valor");
                        erroren = 1;
                     }
                     
                     salvaa = v_mpfi(ipfi)->f_campos[numcam-1].c_fflag;
                     if (actualiza_cam(ipfi,numcam,faclin,14) != -1) 
                     {
                        cursor(qp);
                        if (lineal && (esta_en_linea || faclin)) 
                        {
                           if (rentra_cc != rentra_cf)
                              Color(rentra_cc,rentra_cf);
                           atributo(rentra_atrib);
                           a_atri_campo(v_mpfi(ipfi),numcam-1,rentra_atrib,0,rentra_cc,rentra_cf);
                           /*printcam(v_mpfi(ipfi),numcam);*/
                           printcam_x(v_mpfi(ipfi),numcam,3);
                           a_atri_campo(v_mpfi(ipfi),numcam-1,rentra_atrib,1,rentra_cc,rentra_cf);
                           atributo(tolower(rentra_atrib));
                        }
                        else
                        {
						   if (!es_especial)
								printusi_i(v_mpfi(ipfi),numcam);
                        }
                     }
                     v_mpfi(ipfi)->f_campos[numcam-1].c_fflag = salvaa;
                     if (modif2 && !es_especial) 
					 {
                        modificado = ((strcmp(qini,qo)) ? 1 : 0); /* compara --> es 0 si son iguales */
                     }
                     else {
                        modificado = 1;
                        modif2 = 0;
                     }
					 /*if (modificado)
						 aviso(0,"modificado");*/
              }
              if (fm)
              {
                  Ar();Bl(4);Cr();
              }
              if (e_contador && qj != 1 && qj != 7 && encl && altaa == ALTA && !mira_sivacio(v_mpfi(ipfi),numcam-1)) {
                 Ar();
                 aviso(0,"No se permiten codigos en blanco en esta ficha!");
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
                                if ((_ID_Cursor%1000) == ipfi)
                                {
                                   if (ccc > 0 && (_ID_Cursor/1000) < ccc)
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
                   tras_entra(numpan,numcam,CONSULTA,encl,0,ipfi,faclin);
                   if (memoplin > 4) sprintf(v_memo(5004)->alfabetica,"%d",qj);
                   if ((tipoerror = actualiza_cam(ipfi,numcam,faclin,11)))
                   {
                      if (tipoerror == -2 && !es_especial)
					  {
                         st_campo(numcam-1,v_mpfi(ipfi),qini);		    
					  }
                      modif2 = 1;
                      erroren = 1;
                   }
                }
                else
                   if (qj != 6 && (tipoerror = tras_entra(numpan,numcam,altaa,encl,modificado,
                      ipfi,faclin)) < 0) 
				   {
                      if (tipoerror == -2 && !es_especial)
                         st_campo(numcam-1,v_mpfi(ipfi),qini);		    
                      modif2 = 1;
                      erroren = 1;
                   }
                   else 
                      if (qj == 6) 
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
                                  ipfi,faclin)) < 0) 
                               {
                                  if (tipoerror == -2 && !es_especial)
                                     st_campo(numcam-1,v_mpfi(ipfi),qini);		    
                                  modif2 = 1;
                                  erroren = 1;
                               }
                            }
                         }
                         else
                            if ((eqj == 1/* || eqj == 2*/) && encl && !(faclin/100) && v_mpfi(ipfi)->f_idgrafico <= 0)
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
									  ipfi,faclin)) < 0) 
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
                                  if (i == -2 && encl && !(faclin/100) && v_mpfi(ipfi)->f_idgrafico <= 0)
                                  {
                                     i = consul_clave(ipfi,encl,numpan,0,0,0,ipfi,numcam-1,NULL);
									 if (i >= 0)
									 {
									   if ((tipoerror = tras_entra(numpan,numcam,altaa,encl,modificado,
										  ipfi,faclin)) < 0) 
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

        /*
		if (!es_especial)
		{
			free(qo);
			free(qd);
			free(qini);
		}
		*/
        
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
		ipfi_erp_actual = _ipfi_erp_actual;

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

int pregun_clave(pnclave,ipfi,faclin,alta,sinbotones)
int *pnclave;
int ipfi;
int faclin;
int alta;
int sinbotones;
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
   int amodif;
   int npan;
   int posm;
   static int pulsado_qj = 0;
   recurre_raton(0);
   if (faclin < 0)
   {
	   if (alta == MODIF)
		   sin_pintar = 2;
	   else
           sin_pintar = 1;
       faclin = 0;
   }
   
   
   if (!v_mpfi(ipfi)->f_posmenu) {
   	  v_mpfi(ipfi)->f_posmenu = std_size/100;
	   if (!espuro10 && v_mpfi(ipfi)->f_posmenu < 24)
		   v_mpfi(ipfi)->f_posmenu = 24;
   }

	posm = v_mpfi(ipfi)->f_posmenu;
    if (!posm)
	   posm = 24;


   /*campini = v_mpfi(ipfi)->f_pantalla[0].p_campoini;  /* campo inicial */
   /*campfin = v_mpfi(ipfi)->f_pantalla[0].p_campofin;  /* campo final */
   /* poner campos para raton */
   anula_mapa_raton(-1);
   
   
   if (0 && !sin_pintar)
   {
       for (i = campini;i <= campfin;i++)
       {
          /*pon_campo_en_el_raton(ipfi,i,faclin);*/
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
   

    if (!sinbotones && v_mpfi(ipfi)->f_idgrafico <= 0)
	{
		int px;

		px = std_size%100 - 25;				
		if ((altaa/100)  == ALTA)
		{
			px -= 3;
			v_mpfi(ipfi)->f_controles[0] = boton_tecla(802,v_mpfi(ipfi)->f_id,posm*100+px,posm*100+px+13,"{{206}} Validar Alta");
			px += 3;
		}
		else if ((altaa/100)  == MODIF)
		{
			px -= 3;
			v_mpfi(ipfi)->f_controles[0] = boton_tecla(802,v_mpfi(ipfi)->f_id,posm*100+px,posm*100+px+13,"{{206}} Validar Modif");
			px += 3;
		}
        else if ((altaa/100) == BAJA)
		{
			px -= 3;
			v_mpfi(ipfi)->f_controles[0] = boton_tecla(802,v_mpfi(ipfi)->f_id,posm*100+px,posm*100+px+13,"{{206}} Validar Baja");
			px += 3;
		}
		else
		{
		   if (!manteblanco)
		   {
			  px -= 4;
			  v_mpfi(ipfi)->f_controles[0] = boton_tecla(802,v_mpfi(ipfi)->f_id,posm*100+px,posm*100+px+14,"{{206}} Validar Consulta");
			  px += 4;
		   }
		}
		px += 12;
		v_mpfi(ipfi)->f_controles[1] = boton_tecla(807,v_mpfi(ipfi)->f_id,posm*100+px,posm*100+px+10,"{{207}} Cancelar");
	}
  
   
   campo_entrado = 0;
   for (ncc=0;ncc<camfin;numcam++,ncc++)
   {
      if (numcam >= camfin)
         numcam = 0;
      if (ncc < 0)
      {
		if (v_mpfi(ipfi)->f_controles[0])
		{
			if (DespachadorExterno)
				(*DespachadorExterno)((int)DESPACHO_DESTRUYECONTROL,v_mpfi(ipfi)->f_controles[0],NULL);			
			v_mpfi(ipfi)->f_controles[0] = 0;
		}
		if (v_mpfi(ipfi)->f_controles[1])
		{
			if (DespachadorExterno)
				(*DespachadorExterno)((int)DESPACHO_DESTRUYECONTROL,v_mpfi(ipfi)->f_controles[1],NULL);			
			v_mpfi(ipfi)->f_controles[1] = 0;
		}

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
      else
	  {
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

      if (requiere_reposicionar == 1)
      {
	      qj = SOLICITA_POSICION;
	      requiere_reposicionar = 0;
      }
	  else do
	  {
		if (sin_pintar == 2)
		{
			amodif = v_mpfi(ipfi)->f_campos[campo-1].c_mante.c_modif;
			v_mpfi(ipfi)->f_campos[campo-1].c_mante.c_modif = 1;
		}
		/*npan = v_mpfi(ipfi)->f_campos[campo-1].c_mante.c_actual / 1000;*/
		npan = pantalla_segun_campo(v_mpfi(ipfi),campo-1);
		qj = entra_campo(ipfi,campo,npan,altaa,faclin,qj,nclave+1,flag,-1,campo,alta);
		if (sin_pintar == 2)
		{
			v_mpfi(ipfi)->f_campos[campo-1].c_mante.c_modif = amodif;
		}
		if (qj == TECLA_CHECK || qj == TECLA_DOBLECLICK)
			continue;
		break;
	  } while(-1);
      
      if (qj == SOLICITA_POSICION)
      {
         if ((_ID_Cursor%1000) != ipfi)
         {
            ncc--;
            _ID_Cursor = -1;
            continue;
         }
         if (!sin_pintar)
         {
			 if ((_ID_Cursor/1000) > 0 && (_ID_Cursor/1000) <= v_mpfi(ipfi)->f_ncampos)
			 {
				 for (i = 0;i < camfin;i++)
				 {
					campo = v_mpfi(ipfi)->f_taborder[(_ID_Cursor/1000)-1]+1;
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
		if (v_mpfi(ipfi)->f_controles[0])
		{
			if (DespachadorExterno)
				(*DespachadorExterno)((int)DESPACHO_DESTRUYECONTROL,v_mpfi(ipfi)->f_controles[0],NULL);			
			v_mpfi(ipfi)->f_controles[0] = 0;
		}
		if (v_mpfi(ipfi)->f_controles[1])
		{
			if (DespachadorExterno)
				(*DespachadorExterno)((int)DESPACHO_DESTRUYECONTROL,v_mpfi(ipfi)->f_controles[1],NULL);			
			v_mpfi(ipfi)->f_controles[1] = 0;
		}
         anula_mapa_raton(-1);
         recurre_raton(1);

         return(SOLICITA_POSICION);
      }
      
      if (campo == cr && (qj == 3 || qj == 2 || qj == 4 || qj == 5 || qj == 1 || qj == 7 ||
         qj == SOLICITA_INICIO || qj == SOLICITA_FINAL ||
         qj == SOLICITA_SCROLLDOWN || qj == SOLICITA_SCROLLUP ||
         qj == SOLICITA_INSERCION || qj == SOLICITA_SUPRESION))
      {
		if (v_mpfi(ipfi)->f_controles[0])
		{
			if (DespachadorExterno)
				(*DespachadorExterno)((int)DESPACHO_DESTRUYECONTROL,v_mpfi(ipfi)->f_controles[0],NULL);			
			v_mpfi(ipfi)->f_controles[0] = 0;
		}
		if (v_mpfi(ipfi)->f_controles[1])
		{
			if (DespachadorExterno)
				(*DespachadorExterno)((int)DESPACHO_DESTRUYECONTROL,v_mpfi(ipfi)->f_controles[1],NULL);			
			v_mpfi(ipfi)->f_controles[1] = 0;
		}
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
					if (v_mpfi(ipfi)->f_controles[0])
					{
						if (DespachadorExterno)
							(*DespachadorExterno)((int)DESPACHO_DESTRUYECONTROL,v_mpfi(ipfi)->f_controles[0],NULL);						
						v_mpfi(ipfi)->f_controles[0] = 0;
					}
					if (v_mpfi(ipfi)->f_controles[1])
					{
						if (DespachadorExterno)
							(*DespachadorExterno)((int)DESPACHO_DESTRUYECONTROL,v_mpfi(ipfi)->f_controles[1],NULL);						
						v_mpfi(ipfi)->f_controles[1] = 0;
					}
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
						if (v_mpfi(ipfi)->f_controles[0])
						{
							if (DespachadorExterno)
								(*DespachadorExterno)((int)DESPACHO_DESTRUYECONTROL,v_mpfi(ipfi)->f_controles[0],NULL);
							v_mpfi(ipfi)->f_controles[0] = 0;
						}
						if (v_mpfi(ipfi)->f_controles[1])
						{
							if (DespachadorExterno)
								(*DespachadorExterno)((int)DESPACHO_DESTRUYECONTROL,v_mpfi(ipfi)->f_controles[1],NULL);							
							v_mpfi(ipfi)->f_controles[1] = 0;
						}
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
					if (v_mpfi(ipfi)->f_controles[0])
					{
						if (DespachadorExterno)
							(*DespachadorExterno)((int)DESPACHO_DESTRUYECONTROL,v_mpfi(ipfi)->f_controles[0],NULL);						
						v_mpfi(ipfi)->f_controles[0] = 0;
					}
					if (v_mpfi(ipfi)->f_controles[1])
					{
						if (DespachadorExterno)
							(*DespachadorExterno)((int)DESPACHO_DESTRUYECONTROL,v_mpfi(ipfi)->f_controles[1],NULL);						
						v_mpfi(ipfi)->f_controles[1] = 0;
					}
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
					if (v_mpfi(ipfi)->f_controles[0])
					{
						if (DespachadorExterno)
							(*DespachadorExterno)((int)DESPACHO_DESTRUYECONTROL,v_mpfi(ipfi)->f_controles[0],NULL);						
						v_mpfi(ipfi)->f_controles[0] = 0;
					}
					if (v_mpfi(ipfi)->f_controles[1])
					{
						if (DespachadorExterno)
							(*DespachadorExterno)((int)DESPACHO_DESTRUYECONTROL,v_mpfi(ipfi)->f_controles[1],NULL);
						v_mpfi(ipfi)->f_controles[1] = 0;
					}
                   anula_mapa_raton(-1);
                   recurre_raton(1);

                   return(-1);
                }
                if (qj == 0 || qj == 6)
                {                     /* el ultimo campo de una clave */                                              /* lo valido con return         */
					if (v_mpfi(ipfi)->f_controles[0])
					{
						if (DespachadorExterno)
							(*DespachadorExterno)((int)DESPACHO_DESTRUYECONTROL,v_mpfi(ipfi)->f_controles[0],NULL);						
						v_mpfi(ipfi)->f_controles[0] = 0;
					}
					if (v_mpfi(ipfi)->f_controles[1])
					{
						if (DespachadorExterno)
							(*DespachadorExterno)((int)DESPACHO_DESTRUYECONTROL,v_mpfi(ipfi)->f_controles[1],NULL);
						v_mpfi(ipfi)->f_controles[1] = 0;
					}
                   if (qj == 6)
                      clave_de_consulta = 1;
                   *pnclave = nclave;
                   anula_mapa_raton(-1);
                   recurre_raton(1);

                   return(-1);
                }
                else
                {                             /* en el ultimo campo pulso abajo */
					if (v_mpfi(ipfi)->f_controles[0])
					{
						if (DespachadorExterno)
							(*DespachadorExterno)((int)DESPACHO_DESTRUYECONTROL,v_mpfi(ipfi)->f_controles[0],NULL);						
						v_mpfi(ipfi)->f_controles[0] = 0;
					}
					if (v_mpfi(ipfi)->f_controles[1])
					{
						if (DespachadorExterno)
							(*DespachadorExterno)((int)DESPACHO_DESTRUYECONTROL,v_mpfi(ipfi)->f_controles[1],NULL);
						v_mpfi(ipfi)->f_controles[1] = 0;
					}
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
                
				if (v_mpfi(ipfi)->f_controles[0])
				{
					if (DespachadorExterno)
						(*DespachadorExterno)((int)DESPACHO_DESTRUYECONTROL,v_mpfi(ipfi)->f_controles[0],NULL);
					v_mpfi(ipfi)->f_controles[0] = 0;
				}
				if (v_mpfi(ipfi)->f_controles[1])
				{
					if (DespachadorExterno)
						(*DespachadorExterno)((int)DESPACHO_DESTRUYECONTROL,v_mpfi(ipfi)->f_controles[1],NULL);
					v_mpfi(ipfi)->f_controles[1] = 0;
				}
                anula_mapa_raton(-1);
                recurre_raton(1);
				return(nclave);

                /*
                * return         *pnclave        Eleccion
                *   -1              -1           Salir con esc o ctrl-c
                *   -1             nclave        Entramos la clave numero nclave
                * nclave           nclave        Pedimos otra clave (la nclave)
                */
}



int que_clave_tiene_el_raton(ipfi,cc)
int ipfi;
int *cc;
{
   int campo,clave,i,j;
   if (_ID_Cursor == -1) return(-1);
   if ((_ID_Cursor%1000) != ipfi)
			{
      return(-1);
			}
   campo = v_mpfi(ipfi)->f_taborder[(_ID_Cursor/1000)-1]+1;
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



int pantalla_segun_orden(int ipfi,int ccc)
{
    int numpan = 0;
    int i;

    for (i = 0;i < v_mpfi(ipfi)->f_npanta;i++)
    {
        if ( (ccc+1) >= v_mpfi(ipfi)->f_pantalla[i].p_campoini && 
             (ccc+1) <= v_mpfi(ipfi)->f_pantalla[i].p_campofin    )
        {
            numpan = i;
            break;
        }                    
    }

    return numpan;
}

int busca_un_campo_en_orden(int ipfi,int desde,int factor,int modo)
{
    while(desde >= 0 && desde < v_mpfi(ipfi)->f_ncampos && v_mpfi(ipfi)->f_taborder[desde] == -1)
    {
        desde += factor;
    }

    if (desde < 0)
    {
        desde = 0;        
    }
    else
    if (desde >= v_mpfi(ipfi)->f_ncampos)
    {
        desde = v_mpfi(ipfi)->f_ncampos - 1;
    }
    else
    {
        if (modo) /* buscar uno realmente 'entrable' */
        {
            int numcam = v_mpfi(ipfi)->f_taborder[desde];
            if (numcam == -1 || (v_mpfi(ipfi)->f_orderflags[desde]&ORDER_FLAG_COPIA) ) return desde;

            while((v_mpfi(ipfi)->f_orderflags[desde]&ORDER_FLAG_COPIA) || numcam == -1 /*|| v_mpfi(ipfi)->f_campos[numcam].c_mante.c_modif*/ || v_mpfi(ipfi)->f_campos[numcam].c_mante.c_visual || es_clave_primaria(v_mpfi(ipfi),numcam))
            {
                desde += factor;
                if (desde < 0 || desde >= v_mpfi(ipfi)->f_ncampos) break;
                numcam = v_mpfi(ipfi)->f_taborder[desde];
            }
            if (desde < 0)
            {
                desde = 0;        
            }
            else
            if (desde >= v_mpfi(ipfi)->f_ncampos)
            {
                desde = v_mpfi(ipfi)->f_ncampos - 1;
            }
            else
                return desde;
        }
        else
            return desde;
    }

    factor = factor * -1;

    while(desde >= 0 && desde < v_mpfi(ipfi)->f_ncampos && (v_mpfi(ipfi)->f_taborder[desde] == -1 || (v_mpfi(ipfi)->f_orderflags[desde]&ORDER_FLAG_COPIA)))
    {
        desde += factor;
    }

    if (desde >= 0 && desde < v_mpfi(ipfi)->f_ncampos)
    {
        if (modo) /* buscar uno realmente 'entrable' */
        {
            int numcam = v_mpfi(ipfi)->f_taborder[desde];
            if (numcam == -1) return desde;

            while((v_mpfi(ipfi)->f_orderflags[desde]&ORDER_FLAG_COPIA) || numcam == -1 /*|| v_mpfi(ipfi)->f_campos[numcam].c_mante.c_modif*/ || v_mpfi(ipfi)->f_campos[numcam].c_mante.c_visual || es_clave_primaria(v_mpfi(ipfi),numcam))
            {
                desde += factor;
                if (desde < 0 || desde >= v_mpfi(ipfi)->f_ncampos) break;
                numcam = v_mpfi(ipfi)->f_taborder[desde];
            }
            if (desde < 0)
            {
                desde = 0;        
            }
            else
            if (desde >= v_mpfi(ipfi)->f_ncampos)
            {
                desde = v_mpfi(ipfi)->f_ncampos - 1;
            }
            else
                return desde;
        }
        else
            return desde;        
    }

    return 0;
}

int pseudo_entra_campos(int ipfi,int alta,int faclin,int desde,int hasta,int *qj) /* desde y hasta no inclusive! */
{    
    int tmp_qj;
    int numcam;
    int numpan;


    if (desde < 0)
        desde = 0;
    if (desde >= v_mpfi(ipfi)->f_ncampos)
        desde = v_mpfi(ipfi)->f_ncampos-1;

    if (hasta < 0)
        hasta = 0;
    if (hasta >= v_mpfi(ipfi)->f_ncampos)
        hasta = v_mpfi(ipfi)->f_ncampos-1;

    numcam = v_mpfi(ipfi)->f_taborder[hasta];
    if ((v_mpfi(ipfi)->f_orderflags[hasta]&ORDER_FLAG_COPIA) || numcam == -1 /*|| v_mpfi(ipfi)->f_campos[numcam].c_mante.c_modif*/ || v_mpfi(ipfi)->f_campos[numcam].c_mante.c_visual || es_clave_primaria(v_mpfi(ipfi),numcam))
    {
        int factor;

        if (desde > hasta)
           factor = -1;
        else
        if (desde < hasta)
           factor = 1;
        else
        if (*qj == 2 || *qj == 4 || *qj == SOLICITA_INICIO || *qj == SOLICITA_RETROCESO)
           factor = -1;
        else
           factor = 1;

        hasta = busca_un_campo_en_orden(ipfi,hasta,factor,1);
    }
    
    numcam = v_mpfi(ipfi)->f_taborder[desde];
    if (numcam == -1 || (v_mpfi(ipfi)->f_orderflags[desde]&ORDER_FLAG_COPIA))
    {
        int factor;

        if (desde > hasta)
           factor = -1;
        else
        if (desde < hasta)
           factor = 1;
        else
        if (*qj == 2 || *qj == 4 || *qj == SOLICITA_INICIO || *qj == SOLICITA_RETROCESO)
           factor = -1;
        else
           factor = 1;
        
        desde = busca_un_campo_en_orden(ipfi,desde,factor,0);
        if (factor == -1 && desde < hasta)
            *qj = 3;
        else
        if (factor == 1 && desde > hasta)
            *qj = 2;        
    }
    
    if (desde < hasta)
    {
        desde++;
        tmp_qj = 3;
        while(desde < hasta)
        {
            numcam = v_mpfi(ipfi)->f_taborder[desde];
            if (numcam == -1 || (v_mpfi(ipfi)->f_orderflags[desde]&ORDER_FLAG_COPIA))
            {
                desde = busca_un_campo_en_orden(ipfi,desde,-1,0);
                if (desde > hasta)
                    *qj = 2;
                break;
            }
            if (memoplin > 4) 
            {			
                sprintf(v_memo(5004)->alfabetica,"%d",tmp_qj);
            }
             
            numpan = pantalla_segun_orden(ipfi,desde);

            if (!antes_entra(numpan,numcam+1,alta,FALSO,ipfi,faclin,0))
            {
                if (tras_entra(numpan,numcam+1,alta,FALSO,0,ipfi,faclin) < 0)
                {                   
                   break;
                }
            }
            desde++;            
        }
    }
    else
    if (desde > hasta)
    {
        desde--;
        tmp_qj = 2;
        while(desde > hasta)
        {
            numcam = v_mpfi(ipfi)->f_taborder[desde];            
            if (numcam == -1 || (v_mpfi(ipfi)->f_orderflags[desde]&ORDER_FLAG_COPIA))
            {
                desde = busca_un_campo_en_orden(ipfi,desde,1,0);
                if (desde < hasta)
                    *qj = 3;
                break;
            }
            if (memoplin > 4) 
            {			
                sprintf(v_memo(5004)->alfabetica,"%d",tmp_qj);
            }

            numpan = pantalla_segun_orden(ipfi,desde);
            
            if (!antes_entra(numpan,numcam+1,alta,FALSO,ipfi,faclin,0))
            {
                if (tras_entra(numpan,numcam+1,alta,FALSO,0,ipfi,faclin) < 0)
                {                   
                   break;
                }
            }
            desde--;
        }
    }
    return desde;
}


int entrada_de_campos(int ipfi,int alta,int faclin,int qj,int *u_c,int *cc,int *no)
{
    /* ccc y norden se debem suplir en el caso de orientacion amensajes */
    int ccc = *cc;      /* campo corriente segun orden a entrar */
    int ant_ccc;
    int norden = *no;   /* campo deseado*/
    int ultimo_campo = *u_c; /* ultimo campo 'nuevo' entrado*/    

    int numcam;       /* campo 'real' */
    int numpan;       /* pantalla 'logica' o dialogo al que pertenece el campo corriente */    

    int altaa;        /* modo 'parcial' */

    int tmpi;

    ant_ccc = -1;

    do /* bucle de control de entrada de los campos */
    {       
        
        if (memoplin > 3) 
        {
            altaa = atoi(v_memo(5003)->alfabetica);
            altaa = (altaa % 100) + alta * 100;
            sprintf(v_memo(5003)->alfabetica,"%d",altaa);
        }

        /* posicionar al campo deseado */
        if (ultimo_campo > norden)
            altaa = MODIF;
        else
            altaa = alta;
        ccc = pseudo_entra_campos(ipfi,altaa,faclin,ccc,norden,&qj);

        if (ccc == ant_ccc)
        {
            if (qj == 2)
            {
                qj = 0;
            }
            else
            {
                break;
            }
        }
        ant_ccc = ccc;

        if (ccc > ultimo_campo)
            ultimo_campo = ccc;

        numcam = v_mpfi(ipfi)->f_taborder[ccc];
        numpan = pantalla_segun_orden(ipfi,ccc);        

        norden = ccc;
        if ( (v_mpfi(ipfi)->f_orderflags[ccc]&ORDER_FLAG_COPIA) || numcam < 0)
        {

        }
        else
        {
			/*debug if (v_mpfi(ipfi)->f_campos[numcam].c_tipo == NULOTIPO)
			{
				qj = qj; 
			}*/
		    if (requiere_reposicionar == 1)
			{
			    qj = SOLICITA_POSICION;
			    requiere_reposicionar = 0;
			}
			else do
			{
				qj = entra_campo(ipfi,numcam+1,numpan,altaa,faclin,qj,FALSO,0,ccc,alta);
				if (qj == TECLA_CHECK || qj == TECLA_DOBLECLICK)
					continue;
				break;
			} while(-1);
        }

        if (qj == 1 || qj == 7) break;

        switch(qj)
        {
        case 2: /* uno menos */
            norden = ccc - 1;
            break;
        case 4: /* un bloque menos */
            norden = v_mpfi(ipfi)->f_pantalla[numpan].p_campoini - 2;
            break;
        case 5: /* un bloque mas */
            norden = v_mpfi(ipfi)->f_pantalla[numpan].p_campofin;
            break;
        case SOLICITA_POSICION:
            if ((_ID_Cursor%1000) != ipfi)
            {
               _ID_Cursor = -1; /* !!! */
               break;
            }
            norden = (_ID_Cursor/1000) - 1;            
            if (norden < ccc)
                qj = 2;
            else
                qj = 3;
			ant_ccc = -1;
            break;
        default:
            norden = ccc + 1;
        }


        if (norden > ccc)
        {
            tmpi = busca_un_campo_en_orden(ipfi,norden,1,1);
            if (tmpi == ccc || (!qj && tmpi < ccc))
            {
                break;
            }
        }

    } while(-1);

    *u_c = ultimo_campo;
    *cc = ccc;
    *no = norden;

    return qj;
}

/* 
 * entra_datos
 * entrada y/o proceso de campos de un def, todos o algunos
 * devuelve 7 para abandono
 * es facilmente convertible a funcion de entrada,proceso y salida para orientarlo a mensajes
 */


int __entra_datos(int alta,int ipfi,int faclin,int sinbotones)
{
    double vca,vcf;   /* salvar campo-fichero para memorias de referencia */
    int altaa;        /* flag de alta local */
    int cerrar;       /* flag para indicar si se ha de cerrar el fichero o no al salir */
    int tipo;         /* flag alta segun entrada (-) o salida (+) */
    char *buf = NULL; /* buffer de datos original por si se invalida la entrada */
    int pregucor;     /* control de salida por programa o 'tipo 12'*/
    int qj = 0;       /* operacion de movimiento de ficha a realizar */    	
	int noconfirmar  = 0;
    int ultimo_campo = 0; /* ultimo campo 'nuevo' entrado */
    int campo_actual = 0; /* campo actual */
    int campo_nuevo  = 0; /* campo a posicionarse */
    int i;
    int posm;
	int ant_modovalidacion = modo_de_validacion;


	modo_de_validacion = ESC;
	if (faclin < 0)
	{
		noconfirmar = 1;
		faclin = 0;
	}


	if (!v_mpfi(ipfi)->f_posmenu)
		v_mpfi(ipfi)->f_posmenu = std_size/100;
    if (!espuro10 && v_mpfi(ipfi)->f_posmenu < 24)
	    v_mpfi(ipfi)->f_posmenu = 24;


	posm = v_mpfi(ipfi)->f_posmenu;
    if (!posm)
	   posm = 24;

    if (v_mpfi(ipfi)->f_idgrafico < 0 || !v_mpfi(ipfi)->f_campogids)
    {
        pin_all_pan(ipfi,0);
    }

    if (memoplin > 2) 
    {
        vca = v_memo(5000)->numerica;
    }
    if (memoplin > 11) 
    {
        vcf = v_memo(5011)->numerica;
    }
    if (memoplin > 3) 
    {
        altaa = atoi(v_memo(5003)->alfabetica);
        altaa = (altaa % 100) + alta * 100;
        sprintf(v_memo(5003)->alfabetica,"%d",altaa);
    }

    if ((cerrar = f_v(ipfi,RNOCERRAR)) == -1)
        f_s(ipfi,RNOCERRAR);

    tipo = (-alta); 

    if (actualiza_rel(ipfi,tipo,0,faclin,&buf) < 0)
    {
        tipo = alta;
        actualiza_rel(ipfi,tipo,-1,faclin,&buf);        
        if (cerrar == -1)
           f_n(ipfi,RNOCERRAR);
        if (buf)
        {
           rw_error(77);
        }        
        if (!faclin && !noconfirmar)
        {
            qj = -1;
        }
        else
        {
            qj = 7;
        }
        if (memoplin > 3)
           sprintf(v_memo(5004)->alfabetica,"%d",qj);
		modo_de_validacion = ant_modovalidacion;
        return qj;        
    }

    for (i = 0;i < v_mpfi(ipfi)->f_ncampos;i++)
	{
		if (v_mpfi(ipfi)->f_campos[i].c_tipo == NULOTIPO)
		{
			  if (DespachadorExterno)
			  {       
				 char tmp[512];
				 int id_grid = v_mpfi(ipfi)->f_campowids[i];
				 strcpy(tmp,"CUSTOM1100"); /* 1100 Reponer mantenimento */
				 (*DespachadorExterno)((int)DESPACHO_ESTADOCONTROL,id_grid,(void *)tmp);
			  }
		}
	}

    if (alta == MODIF)
    { 
        entra_rel(0,0,alta,FALSO,1,ipfi,6,0); /* visto bueno a las relaciones obligatorias en caso de modificacion */
    }
    else
    {
        entra_rel(0,0,alta,FALSO,1,ipfi,7,faclin); /* limpiar marcas de relacion */
    }


    if (v_mpfi(ipfi)->f_idgrafico <= 0 && !sinbotones)	
	{
		int px;

		px = std_size%100 - 25;

		if (alta == ALTA)
		{
		   px -= 3;
		   v_mpfi(ipfi)->f_controles[0] = boton_tecla(801,v_mpfi(ipfi)->f_id,posm*100+px,posm*100+px+13,"{{206}} Validar Alta");
		   px += 3;
		}
		else if (alta == MODIF)
		{
		   px -= 3;
		   v_mpfi(ipfi)->f_controles[0] = boton_tecla(801,v_mpfi(ipfi)->f_id,posm*100+px,posm*100+px+13,"{{206}} Validar Modif");
		   px += 3;
		}
		else
		{
		   if (!manteblanco)
		   {
			   px -= 4;
			   v_mpfi(ipfi)->f_controles[0] = boton_tecla(801,v_mpfi(ipfi)->f_id,posm*100+px,posm*100+px+14,"{{206}} Validar Consulta");
			   px += 4;
		   }
		}
		px += 12;
		v_mpfi(ipfi)->f_controles[1] = boton_tecla(807,v_mpfi(ipfi)->f_id,posm*100+px,posm*100+px+10,"{{207}} Cancelar");
	}


    do /* bucle de control de salida */
    {           
        qj = entrada_de_campos(ipfi,alta,faclin,qj,&ultimo_campo,&campo_actual,&campo_nuevo);

        if (memoplin > 3)
           sprintf(v_memo(5004)->alfabetica,"%d",qj);

        pregucor = ejecuta_otro(ipfi,12,O_NOR,faclin,0); /* proceso de salida */
		if (pregucor >= 0)
			pregucor = 0;

        if (pregucor == -2) /* no hay proceso de control de salida */
        {
           /* preguntar si-no-anular o salir */
            if (!noconfirmar && !faclin && (v_mpfirel(ipfi)->lineas == -1 || v_mpfirel(ipfi)->lineas > 0) && (qj != 7 && (!buf || memcmp(v_mpfi(ipfi)->f_buf,buf,v_mpfi(ipfi)->f_lfich))))
            {
                int rx;
                
                if (!v_mpfi(ipfi)->f_nclaves)
                {
                    rx = sinocancelar("¿Desea Continuar?",(defectoesc ? 'N' : 'S'));
                }
                else
                {
                    rx = sinocancelar("¿Confirma los cambios?",(defectoesc ? 'N' : 'S'));
                }
                if (!rx)
                {
                    continue;
                }
                if (rx == -1)
                {
                    qj = 7;
                }
            }
        }
        else
        if (pregucor == -1)
        {
		   qj = 2;
           continue;           
        }

        if (qj == 7) /* ABANDONAR! */
        {
            entra_rel(0,0,alta,FALSO,1,ipfi,7,faclin);
            actualiza_rel(ipfi,alta,-1,faclin,&buf);
            if (!faclin)
                pinta_datos(ipfi,0);
            break;
        }

        if (v_mpfirel(ipfi)->clave > -1)       
        {
           if (cheq_clave(ipfi,v_mpfirel(ipfi)->clave,1) < 0)
           {
               continue;
           }
        }       

        if (entra_rel(0,0,alta,FALSO,1,ipfi,5,faclin) < 0) /* comprobar relaciones obligatorias */
           continue;          

        entra_rel(0,0,alta,FALSO,1,ipfi,7,faclin); /* limpiar marcas de relacion */    

        /*
        if (v_mpfirel(ipfi)->lineas == 0 || faclin) 
        { 
        */
           if (alta == ALTA)
              defecam_rel(ipfi,-1,0,faclin);
           if (v_mpfirel(ipfi)->lineas == 0) 
           {   /* lineas */
              if (trata_lineas(ipfi,alta,0,0,faclin) < 0)
              {
                  actualiza_rel(ipfi,alta,-1,faclin,&buf);
                  qj = 7;
                  break;
              }
           }
        /*
        }
        */

        if (actualiza_rel(ipfi,alta,0,faclin,&buf) < 0)
        {
            qj = 7;
        }
        break;

    } while (-1);

	if (v_mpfi(ipfi)->f_controles[0])
	{
		if (DespachadorExterno)
			(*DespachadorExterno)((int)DESPACHO_DESTRUYECONTROL,v_mpfi(ipfi)->f_controles[0],NULL);
		v_mpfi(ipfi)->f_controles[0] = 0;
		agi_cola_timer();
	}
	if (v_mpfi(ipfi)->f_controles[1])
	{
		if (DespachadorExterno)
			(*DespachadorExterno)((int)DESPACHO_DESTRUYECONTROL,v_mpfi(ipfi)->f_controles[1],NULL);
		v_mpfi(ipfi)->f_controles[1] = 0;
		agi_cola_timer();
	}

    for (i = 0;i < v_mpfi(ipfi)->f_ncampos;i++)
	{
		if (v_mpfi(ipfi)->f_campos[i].c_tipo == NULOTIPO)
		{
			  if (DespachadorExterno)
			  {       
				 char tmp[512];
				 int id_grid = v_mpfi(ipfi)->f_campowids[i];
				 strcpy(tmp,"CUSTOM1001"); /* 1001 sin mantenimento */
				 (*DespachadorExterno)((int)DESPACHO_ESTADOCONTROL,id_grid,(void *)tmp);
			  }
		}
	}


    if (memoplin > 2) 
    {
       v_memo(5000)->numerica = vca;
    }
    if (memoplin > 11) 
    {
       v_memo(5011)->numerica = vcf;
    }

    if (!faclin)
    {
        if (qj == 7)
            qj = -1;
        else
            qj = 0;
    }
    if (memoplin > 3)
       sprintf(v_memo(5004)->alfabetica,"%d",qj);
	modo_de_validacion = ant_modovalidacion;
    return qj;
}


int entra_datos(int alta,int ipfi,int faclin) {
	return __entra_datos(alta,ipfi,faclin,0); 
}

/*
 * entra_clave
 * pregunta la(s) clave(s) del fichero cuya estructura es ipfi
 * devuelve el numero de la clave elegida para el acceso (>=0)
 * o <0 si se ha salido con esc o ctrl-c.
 * en caso de elegir una clave, la variable de buffer (ipfi)
 * contiene la clave solicitada.
 */
entra_clave(ipfi,nclave,alta,sinbotones)
int ipfi,*nclave,alta,sinbotones;
{
   int j;
   while(-1)
   {
      j = pregun_clave(nclave,ipfi,0,alta,sinbotones);
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
            p_ejecuta(i,f,faclin,7);
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
            if (v_mpfirel(ipfi)->lineas == 0) {/* flag que indica que hay lineas */
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


int _consulta_lee_first(FI_DEF *pfi)
{
	int r = lee_reg(pfi,ISFIRST);
	if (r < 0)
		return r;
   if (hay_dat_permiso(pfi,-2))
	   return r;
	do
	{
       r = lee_reg(pfi,ISNEXT);
	   if (r < 0)
		   break;
	   if (hay_dat_permiso(pfi,-2))
		   break;
	} while(-1);
	return r;
}

int _consulta_lee_next(FI_DEF *pfi)
{
	int r;
	int saltos = 0;
	int err = 0;
	do
	{
       r = lee_reg(pfi,ISNEXT);
	   if (r < 0)
		   break;
	   if (hay_dat_permiso(pfi,-2))
		   break;
	   saltos++;
	} while(-1);
	if (r < 0 && saltos)
	{
		err = iserrno;
		while(saltos)
		{
		   lee_reg(pfi,ISPREV);
		   saltos--;
		}
		iserrno = err;
	}
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


int consulta_lee_gteq(int ipfi)
{
	int r = leer_reg(ipfi,ISGTEQ,0,1);
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
   char opci[300];
   char log_tmp[300];
   int otra_opcion = 99;
   int ant_opcion;
   char *l_copciones[30];
   char l_cletras[30];
   char *ml_copciones[30];   
   int mi;
   int la_primera_vez = 1;
   int opl = -63;
   int *b_wids = NULL;
   int posm = v_mpfi(ipfi)->f_posmenu;
	 char _def_erp_actual[200];
	 int _ipfi_erp_actual = ipfi_erp_actual;

	 memcpy(_def_erp_actual,def_erp_actual,sizeof(def_erp_actual));
	 strcpy(def_erp_actual,quita_path(v_mpfi(ipfi)->f_PathDef));

	 ipfi_erp_actual = ipfi;

   if (!posm)
	   posm = 24;

   for (i = 0;i < 11;i++)
   {
       l_copciones[i] = copciones[i];
       l_cletras[i] = cletras[i];
   }   
   
   
   if (!hay_dat_permiso(v_mpfi(ipfi),-1)) {
	   memcpy(def_erp_actual,_def_erp_actual,sizeof(def_erp_actual));ipfi_erp_actual = _ipfi_erp_actual;
      return(-1);
	  }
   recurre_raton(0);
   
   
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
         /*
         if (la_primera_vez)
            {
                ptec(820);
                la_primera_vez = 0;            
            }
          */
         j = entra_clave(ipfi,&nclave,CONSULTA,0);
      }
      if (j == SOLICITA_POSICION)
      {
		 if (manteblanco)
		 {
			 j = 0;
			 continue;
		 }
         if (!TestigoRaton)
            TestigoRaton = 1;
         else
         {
            pito(7);
            TestigoRaton = 0;
         }
      }
      else
         if (j < 0)
         {
            anula_mapa_raton(-1);
            recurre_raton(1);
            _fin_botones_(&b_wids,v_mpfi(ipfi));
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
         if (r) 
		 {
            Ar();
            aviso(0,"Estructura de claves incorrecta!");
            Cr();
            TestigoRaton = 0;
            anula_mapa_raton(-1);
            recurre_raton(1);
            _fin_botones_(&b_wids,v_mpfi(ipfi));
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
               aviso(0,"Clave inexistente");
               Cr();
               if (opcion == 100)
               {
                  anula_mapa_raton(-1);
                  recurre_raton(1);
                  _fin_botones_(&b_wids,v_mpfi(ipfi));
				  memcpy(def_erp_actual,_def_erp_actual,sizeof(def_erp_actual));ipfi_erp_actual = _ipfi_erp_actual;
                  return(-1);
               }
            }
            else {
               clave_de_consulta = 0;
               while (-1) {
                  if (opcion < 8 || opcion > 99) 
                  {
                      pinta_datos(ipfi,0);
                        /*_pinta_datos(ipfi,0,&ppp,pflag);*/
                  }
                  pflag = 1;
                  if (opcion == 100)
                  {
                     if (!hay_dat_permiso(v_mpfi(ipfi),-2))
                     {
                        TestigoRaton = 0;
                        aviso(0,"No dispone de permiso para Acceder a este Registro");
                        break;
                     }
                     
                     if (v_mpfirel(ipfi)->clave > -1)
                     {
                        if (cheq_clave(ipfi,v_mpfirel(ipfi)->clave,0) == 0)
                        {
                           TestigoRaton = 0;
                           anula_mapa_raton(-1);
                           recurre_raton(1);
                           _fin_botones_(&b_wids,v_mpfi(ipfi));
						   memcpy(def_erp_actual,_def_erp_actual,sizeof(def_erp_actual));ipfi_erp_actual = _ipfi_erp_actual;
                           return(0);
                        }
                        else
                        {
                           break;
                        }
                     }
                     TestigoRaton = 0;
                     anula_mapa_raton(-1);
                     recurre_raton(1);
                     _fin_botones_(&b_wids,v_mpfi(ipfi));
					 memcpy(def_erp_actual,_def_erp_actual,sizeof(def_erp_actual));ipfi_erp_actual = _ipfi_erp_actual;
                     return(0);
                  }
                  
                  *(l_cletras+7) = 'M';
                  *(l_cletras+8) = 'I';                  
                  kk = 6;
                  otra_opcion = 99;
                  if (m)
                     *(l_cletras+7) = 0;
                  else
                  {
                     if (ejecuta_otro(ipfi,4,O_NOR,0,1) < 0)
                     {
                        kk = 8;
                        *(l_cletras+8) = 0;
                     }
                     else
                     {
                        kk = 9;
                     }
                  }

				  if (v_mpfirel(ipfi)->lineas == 0)
				  {
					  l_copciones[kk] = copciones[9];
					  *(l_cletras+kk) = 'L';
					  kk++;
					  opl = kk;
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
                           otra_opcion = kk+1;
                        }
                     }
                  }                  
                  
                  
                  
                  campini = v_mpfi(ipfi)->f_pantalla[0].p_campoini;  /* campo inicial */
                  campfin = v_mpfi(ipfi)->f_pantalla[0].p_campofin;  /* campo final */
                  /* poner campos para raton */
                  anula_mapa_raton(-1);
                  /*
                  for (i = campini;i <= campfin;i++)
                  {
                         pon_campo_en_el_raton(ipfi,i,0);
                  }
                  */
               
                  
                  
                  TestigoRaton = 0;
                  if (apli_aborto)
                  {
                     acabar  = -1;
                     break;
                  }
                  prepara_menusg_botones();                  
                  
                  ant_opcion = opcion;

					/* intercambio de opciones */
					{
						int i;
						for (mi = 0;l_cletras[mi];mi++);							  
						mi -= 2;
						for (i = 0;i < mi;i++)
						{
							ml_copciones[i] = l_copciones[i+2];
						}
						mi += 2;
						ml_copciones[i] = l_copciones[0];
						ml_copciones[i+1] = l_copciones[1];

						if (opcion == 1)
							opcion = mi-1;
						else if (opcion == 2)
							opcion = mi;
						else
							opcion--;
					}

                  opcion = _menusg(ml_copciones,l_cletras,posm*100,"",opcion,&b_wids,v_mpfi(ipfi));
				  
				  {
					  if (opcion > 0)
					  {
						  if (opcion == mi)
							  opcion = 2;
						  else if (opcion == (mi-1))
							  opcion = 1;
						  else
							  opcion += 2;
					  }
				  }

                  
                  if (opcion != 3 && opcion != 4 && opcion != 6 && opcion != 7)
                  {		 
                     _fin_botones_(&b_wids,v_mpfi(ipfi));
                  }

				  if (opcion == opl)
				  {
				      trata_lineas(ipfi,CONSULTA,0,0,0);
					 /*numpan -=1;
					 estoyen = numpan;*/
					 if (trata_lineas(ipfi,CONSULTA,3,0,0) != 0) 
					 {
						/*numpan -=1;
						continue;*/
					 }
					 continue;
				  }

                  if (opcion < 1 && opcion > -3)
                  {
                     acabar  = -1;
                     break;
                  }
                  if (opcion == SOLICITA_POSICION)
                  {
					 if (manteblanco)
					 {
						 opcion = 1;
						 continue;
					 }
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
                              pito(8);
                              TestigoRaton = 0;
                           }
                        }
                        break;
                     }
                     if (m)
                     {
                       TestigoRaton = 0;
                        pito(9);
                        opcion = 1;
                        continue;
                     }
                     opcion = 8;
                     if (!TestigoRaton)
                        TestigoRaton = 1;
                     else
                     {
                        pito(10);
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
                           if ((opcion%10) == 4)
                              ppp--;
                           if ((opcion%10) == 6)
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
                     if (opcion ==  8) {
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
                        if (opcion ==  9) {
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
                           if (opcion ==  7) 
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
         if (opcion == 2) 
		 {
			 acabar = -1;
            break;
         }
         if (opcion == 1) {
            break;
         }
       }
     }
    }    
    
    
    Ar();Bl(4);Cr();
    anula_mapa_raton(-1);
    recurre_raton(1);
	_fin_botones_(&b_wids,v_mpfi(ipfi));    
	memcpy(def_erp_actual,_def_erp_actual,sizeof(def_erp_actual));ipfi_erp_actual = _ipfi_erp_actual;
    return(acabar);
}


modifica1_ficha(ipfi,nclave,sinbotones)
int ipfi,nclave,sinbotones;
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
		 {
			pinta_datos(ipfi,0);
            if (__entra_datos(MODIF,ipfi,0,sinbotones) < 0)
            {
               salida = -1;
            }
            else {
               grabar_reg(ipfi,0,20);
               ejecuta_otro(ipfi,30,O_NOR,0,0);
            }
		 }
      }
      else
      {
         f_n(ipfi,RENBUF);
         libera_ind(v_mpfi(ipfi));   /* desbloquea el registro */
         aviso(0,"No tiene permiso para Modificar este Registro");
         salida = -1;
      }
   }
   else
   {
      TestigoRaton = 0;
      if (iserrno != ELOCKED) {Ar();aviso(0,"Ficha inexistente");Cr();}
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
alta_ficha(ipfi,sinbotones)
int ipfi,sinbotones;
{
   int leida,grabada,salida,clave = 0,j;
   int posm = v_mpfi(ipfi)->f_posmenu;
   if (!posm)
	   posm = 24;
   
   /* se pierde optimizacion pero arregla de un modo directo problemas de comparacion  */
   if (v_mpfi(ipfi)->f_bufcompara)
	  memset(v_mpfi(ipfi)->f_bufcompara,0xff,v_mpfi(ipfi)->f_lfich);
   /**********/


   limpia_ficha(ipfi);
   
   j = pregun_clave(&clave,ipfi,0,ALTA,sinbotones);
   
   if (clave < 0 )
      return(-1);
   if (!(hay_dat_permiso(v_mpfi(ipfi),-1) & 2))
			{
      aviso(0,"No puede dar de Alta este Registro"); 
      return(-1);
			}
   
   if (cambia_ind(v_mpfi(ipfi),1) < 0) {
      Ar();
      aviso(0,"Fichero CORRUPTO");
      Cr();
      return(-1);
			}
   Bl(posm);
   salida = 0;
   leida = leer_reg(ipfi,ISEQUAL,0,40);
   if (leida < 0) {      /* no existe, damos de alta */
      if (iserrno != ENOREC) {
         return(0);
      }
      if (grabar_reg(ipfi,3,20) < 0) {pito(11);return(0);}
      if (j == SOLICITA_POSICION)
      {
         if (!TestigoRaton)
            TestigoRaton = 1;
         else
         {
            pito(12);
            TestigoRaton = 0;
         }
      }
      if (__entra_datos(ALTA,ipfi,0,sinbotones) < 0 || apli_aborto) 
      {
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

	    if (manteblanco)
		{			
			limpia_ficha(ipfi);
			pinta_datos(ipfi,0);			  			
		}	
  	    

			}
   else 
   {
	    if (manteblanco)
		{
			aviso(0,"Registro Existente");
			limpia_ficha(ipfi);
		}		
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
modif_ficha(ipfi,sinbotones)
int ipfi,sinbotones;
{
			int nclave = 0,salida,j;
   int posm = v_mpfi(ipfi)->f_posmenu;
   if (!posm)
	   posm = 24;

         
         j = entra_clave(ipfi,&nclave,MODIF,sinbotones);
         if (j < 0 && j != SOLICITA_POSICION) {
            return(-1);
         }
         Bl(posm);
         
         if (cambia_ind(v_mpfi(ipfi),nclave+1) < 0) {
            Ar();
            aviso(0,"Fichero CORRUPTO");
            Cr();
            return(-1);
         }
         if (j == SOLICITA_POSICION)
         {
            if (!TestigoRaton)
               TestigoRaton = 1;
            else
            {
               pito(13);
               TestigoRaton = 0;
            }
         }		 
         salida = modifica1_ficha(ipfi,0,sinbotones); /* en este caso no se debe cambiar */
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
   int posm = v_mpfi(ipfi)->f_posmenu;
   if (!posm)
	   posm = 24;

   
   while(-1)
   {
      j = entra_clave(ipfi,&nclave,BAJA,0);		  
      if (j == SOLICITA_POSICION)
         pito(14);
      else
         break;
   }
   if (j < 0 )
      return(-1);
   if (!(hay_dat_permiso(v_mpfi(ipfi),-1) & 8))
   {
      aviso(0,"No puede dar de Baja este Registro");
      return(-1);
   }
   if (cambia_ind(v_mpfi(ipfi),1) < 0) {
      Ar();
      aviso(0,"Aviso: no hay cambio de la clave de acceso");
      Cr();
      return(-1);
   }
   Bl(posm);
   salida = 0;
   if (leer_reg(ipfi,ISEQUAL,ISLOCK,0) == 0) { /* lee relaciones bloquea las que act. */
      pinta_datos(ipfi,0);				  
      if (confirmar(0,"Confirmacion baja (S/N) ","N") < 0)
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
                  aviso(0,"No se puede dar de baja la ficha");
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
      if (iserrno != ELOCKED) {Ar();aviso(0,"Ficha inexistente o bloqueada");Cr();}
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
int mante_ficha_old(ipfi,ficha,prime,qo,new_size)
int ipfi,ficha,prime;
char *qo;
int new_size;
{
   int opcion,bb;
   int opdef = 2;
   int leida,cerrar,i,j = 0;
   int campini,campfin;
   char opci[31];
   char log_tmp[128];
   char *l_textoi[10];
   char l_letrasi[10];   
   int otra_opcion = 99;
   extern int balance;
   int posm;
   if (new_size == -1)
	   new_size = 2480;
   
   for (i = 0;i < 10;i++)
   {
       l_textoi[i] = textoi[i];
       l_letrasi[i] = letrasi[i];
   }
   
   
   if ( !hay_dat_permiso(v_mpfi(ipfi),-1) )
   {
		if (qo)
        {
           sprintf(qo,"No dispone de Permiso para acceder a los datos:%s",quita_path(v_mpfi(ipfi)->f_fichero));
           aviso(0,qo);
        }
        return(-1);
   }
   
   recurre_raton(0);
   
   if ((cerrar = f_v(ipfi,RNOCERRAR)) == -1) 
   {
		f_s(ipfi,RNOCERRAR);     
        if (abre1_rel(ipfi) < 0) {recurre_raton(1);return(-1);}
   }
   
   
   
   if ((leida = f_v(ipfi,RLEIDA)) == -1) f_s(ipfi,RLEIDA);
   /* empieza aqui mantenimiento normal o anormal */
   
   ejecuta_otro(ipfi,8,O_NOR,0,0);
   

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
			 /*
			 if (total == v_mpfi(ipfi)->f_npanta)
			 {
				DespachoPantalla((int)DESPACHO_PANTALLAS,(int)total+100,(char *)arrai);
			 }
			 else
			 {
				DespachoPantalla((int)DESPACHO_PANTALLAS,(int)v_mpfi(ipfi)->f_npanta,v_mpfi(ipfi)->f_titulo);
			 }
			 */
		  }
		  _pinta_datos(ipfi,100,NULL,0);
   }
   else
   {   
	   if (ficha != 1) 
	   {
		  cls();
		  Ar();Bl(4);Cr();
		  /* de pruebas ... */
		  
		  pin_all_pan(ipfi,1);	  

		  if (v_mpfi(ipfi)->f_npanta > 1)
			default_sid = v_mpfi(ipfi)->f_pantasids[0]; /* por defecto primera pantalla */
      
	   }
	   else
		  ficha = 0;
	   if (ficha != -1 && !proceso[0]) 
		   strcpy(proceso,v_mpfi(ipfi)->f_titulo);
   
	   cabecera_v(NULL,proceso,titulo,empresa);      
   

	   limpia_ficha(ipfi);


   
	   /*if (new_size == 2480)*/
	   {
			char tmpx[512];
			char *px = tmpx; 
			int tmpi = -1;
			int pi,pf;

			memset(tmpx,0,sizeof(tmpx));


			memcpy(px,&tmpi,sizeof(int));
			px += sizeof(int);
			memcpy(px,&default_id,sizeof(int));
			px += sizeof(int);
			tmpi = 0;
			memcpy(px,&tmpi,sizeof(int));
			px += sizeof(int);
			if (v_mpfi(ipfi)->f_npanta > 0)
			{
				pi = v_mpfi(ipfi)->f_pantalla[0].p_posref;
				pf = pi + (v_mpfi(ipfi)->f_pantalla[0].p_posfin-101);
				if (pf > new_size)
				{
					memcpy(px,&pf,sizeof(int));
				}
				else
				{
					memcpy(px,&new_size,sizeof(int));	
				}
			}
			else
			{
				memcpy(px,&new_size,sizeof(int));
			}
			px += sizeof(int);

			(*DespachadorExterno)((int)DESPACHO_PANTALLAS,(int)4,tmpx);    
		}   

	   v_mpfi(ipfi)->f_posmenu = new_size/100;
	   if (!espuro10 && v_mpfi(ipfi)->f_posmenu < 24)
		   v_mpfi(ipfi)->f_posmenu = 24;

	   posm = v_mpfi(ipfi)->f_posmenu;
	   if (!posm)
		   posm = 24;



	   opcion = 1;
	   if (ficha == -1) {
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
							   if (!proceso[0]) 
							   {
								   strcpy(proceso,v_mpfi(ipfi)->f_titulo);
								   cabecera_v(NULL,proceso,titulo,empresa);
							   }
                           
                           
							   if (v_mpfirel(ipfi)->clave < -1) 
							   {
								  /* ATENCION MANTENIMIENTO POR RUTINA (lineal) */
								  if (leida == -1) f_n(ipfi,RLEIDA);
								  sprintf(v_memo(5004)->alfabetica,"%d",2); /* a pelo modo de entrada de modificacion */
								  p_ejecuta(v_mpfirel(ipfi)->clave + 1,O_NOR,0,8);
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
										otra_opcion = j+1;
									 }
								  }
							   }
                           
							   campini = v_mpfi(ipfi)->f_pantalla[0].p_campoini;  /* campo inicial */
							   campfin = v_mpfi(ipfi)->f_pantalla[0].p_campofin;  /* campo final */
							   /* poner campos para raton */
							   anula_mapa_raton(-1);
							   /*
							   for (i = campini;i <= campfin;i++)
							   {
								  //pon_campo_en_el_raton(ipfi,i,0);
							   }
							   */
							   TestigoRaton = 0;
                           
							   /*pulsatecla();*/

							   opcion = menusg(l_textoi,l_letrasi,posm*100," SELECCIONE --> ",opdef);


							   if (opcion == SOLICITA_POSICION)
							   {
								  if (!TestigoRaton)
									 TestigoRaton = 1;
								  else
								  {
									 pito(15);
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
								  cabecera_v(NULL,proceso,titulo,empresa);
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
							   default :
								  if (opcion == otra_opcion && memoplin > 4)
								  {
									 strcpy(v_memo(5004)->alfabetica,"");
									 ejecuta_otro(ipfi,20,O_NOR,0,0);
									 break;
								  }
								   
								  break;
							   }
                           
                           
					}
			  }
		  }
          
          ejecuta_otro(ipfi,9,O_NOR,0,0);
          
          
          if (leida == -1) f_n(ipfi,RLEIDA);
          if (cerrar == -1) {
             f_n(ipfi,RNOCERRAR);
             cierra1_rel(ipfi);
          }
          anula_mapa_raton(-1);
          recurre_raton(1);
          return(ficha);
}




int mante_ficha(ipfi,ficha,prime,qo,new_size)
int ipfi,ficha,prime;
char *qo;
int new_size;
{
   int opcion,bb;
   int opdef = 7;
   int leida,cerrar,i,j = 0,mi;
   int campini,campfin;
   char opci[300];
   char log_tmp[300];
   char *l_textoi[30];
   char l_letrasi[30];
   char *ml_textoi[30];   
   char ml_letrasi[30];
   int *b_wids = NULL;
   int ant_opcion;
   int nclave = 0;
   int oc = 0;
   int r = 0,opl = -1;
   extern int balance;   
   int posm;
   int otra_opcion = 99;
	 char _def_erp_actual[250];
	 int _ipfi_erp_actual = ipfi_erp_actual;
	 int sinbotones = 0;

	 memcpy(_def_erp_actual,def_erp_actual,sizeof(def_erp_actual));
	 strcpy(def_erp_actual,quita_path(v_mpfi(ipfi)->f_PathDef));
	 ipfi_erp_actual = ipfi;

   if (new_size == -1)
	   new_size = 2480;
   
   for (i = 0;i < 11;i++)
   {
       l_textoi[i] = xtextoi[i];
       l_letrasi[i] = xletrasi[i];
   }
   
   
   if ( !hay_dat_permiso(v_mpfi(ipfi),-1) )
   {
		if (qo)
        {
           sprintf(qo,"No dispone de Permiso para acceder a los datos:%s",quita_path(v_mpfi(ipfi)->f_fichero));
           aviso(0,qo);
        }
		memcpy(def_erp_actual,_def_erp_actual,sizeof(def_erp_actual));ipfi_erp_actual = _ipfi_erp_actual;
        return(-1);
   }
   
   recurre_raton(0);
   
   if ((cerrar = f_v(ipfi,RNOCERRAR)) == -1) 
   {
		f_s(ipfi,RNOCERRAR);     
        if (abre1_rel(ipfi) < 0) {recurre_raton(1);
		memcpy(def_erp_actual,_def_erp_actual,sizeof(def_erp_actual));ipfi_erp_actual = _ipfi_erp_actual;
		return(-1);}
   }
   
   
   
   if ((leida = f_v(ipfi,RLEIDA)) == -1) f_s(ipfi,RLEIDA);
   /* empieza aqui mantenimiento normal o anormal */
   
   ejecuta_otro(ipfi,8,O_NOR,0,0);
   if (!strcmp(v_memo(5004)->alfabetica,"::ENDATOS::SINBOTONES::")) {
	   strcpy(v_memo(5004)->alfabetica,"");
	   sinbotones = 1;
   }   
   
   if (ficha != 1) 
   {
      cls();
      Ar();Bl(4);Cr();
      /* de pruebas ... */
	  
      pin_all_pan(ipfi,1);	  

      if (v_mpfi(ipfi)->f_npanta > 1)
        default_sid = v_mpfi(ipfi)->f_pantasids[0]; /* por defecto primera pantalla */
      
   }
   else
	  ficha = 0;
   if (ficha != -1 && !proceso[0]) 
	   strcpy(proceso,v_mpfi(ipfi)->f_titulo);
   
   cabecera_v(NULL,proceso,titulo,empresa);   
   
   
   limpia_ficha(ipfi);


   
   /*if (new_size == 2480)*/
   {
        char tmpx[512];
        char *px = tmpx; 
        int tmpi = -1;
		int pi,pf;

		memset(tmpx,0,sizeof(tmpx));

        memcpy(px,&tmpi,sizeof(int));
        px += sizeof(int);
        memcpy(px,&default_id,sizeof(int));
        px += sizeof(int);
        tmpi = 0;
        memcpy(px,&tmpi,sizeof(int));
        px += sizeof(int);
		if (v_mpfi(ipfi)->f_npanta > 0)
		{
			pi = v_mpfi(ipfi)->f_pantalla[0].p_posref;
			pf = pi + (v_mpfi(ipfi)->f_pantalla[0].p_posfin-101);
			if (pf > new_size)
			{
				memcpy(px,&pf,sizeof(int));
			}
			else
			{
				memcpy(px,&new_size,sizeof(int));	
			}
		}
		else
		{
			memcpy(px,&new_size,sizeof(int));
		}
		px += sizeof(int);

        (*DespachadorExterno)((int)DESPACHO_PANTALLAS,(int)4,tmpx);    
    }   

   v_mpfi(ipfi)->f_posmenu = new_size/100;
   if (!espuro10 && v_mpfi(ipfi)->f_posmenu < 24)
	   v_mpfi(ipfi)->f_posmenu = 24;
   posm = v_mpfi(ipfi)->f_posmenu;
   if (!posm)
	   posm = 24;

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
                  else  if (v_mpfi(ipfi)->f_nclaves == 0) {
                        if (ejecuta_otro(ipfi,10,O_NOR,0,0) != -1)
                           __entra_datos(ALTA,ipfi,0,sinbotones);
                     }
                     else 
					 {						

		  			    cambia_ind(v_mpfi(ipfi),nclave+1);
						if (!manteblanco)
						{
							consulta_lee_gteq(ipfi);
							//consulta_lee_first(ipfi);
							//leer_reg(ipfi,ISFIRST,0,1);
						}
						pinta_datos(ipfi,0);							

                        while (opcion > 0 && !apli_aborto) 
						{
						   if (memoplin > 3) 
						   {							  
							  sprintf(v_memo(5003)->alfabetica,"%d",CONSULTA+CONSULTA*100);
							  if (memoplin > 6) 
							  {
								 sprintf(v_memo(5006)->alfabetica,"%d",nclave+1);
							  }
						   }
						   if (!proceso[0]) 
						   {
							   strcpy(proceso,v_mpfi(ipfi)->f_titulo);
							   cabecera_v(NULL,proceso,titulo,empresa);
						   }
                           
                           
                           if (v_mpfirel(ipfi)->clave < -1) 
						   {
                              /* ATENCION MANTENIMIENTO POR RUTINA (lineal) */
                              if (leida == -1) f_n(ipfi,RLEIDA);
                              sprintf(v_memo(5004)->alfabetica,"%d",2); /* a pelo modo de entrada de modificacion */
                              p_ejecuta(v_mpfirel(ipfi)->clave + 1,O_NOR,0,9);
                              if (leida == -1) f_s(ipfi,RLEIDA);
                              break;
                           }                          
						   

                           if (ejecuta_otro(ipfi,4,O_NOR,0,1) < 0)
                           {
                              *(l_letrasi + 9) = 0;
                              j = 9;
                           }
                           else
                           {
                              *(l_letrasi + 9) = 'I';
                              j = 10;
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
									j++;
									otra_opcion = j;
                                 }
                              }
                           }						   

						  if (v_mpfirel(ipfi)->lineas == 0)
						  {

							  *(l_letrasi + j) = 'L';
							  *(l_letrasi + j + 1) = 0;
							  //aviso(0,v_memo(5004)->alfabetica);
							  //strcpy(opci,v_memo(5004)->alfabetica);
							  l_textoi[j] = copciones[9];							  
							  j++;
							  opl = j;
						  }	
                           
                           campini = v_mpfi(ipfi)->f_pantalla[0].p_campoini;  /* campo inicial */
                           campfin = v_mpfi(ipfi)->f_pantalla[0].p_campofin;  /* campo final */
                           /* poner campos para raton */
                           anula_mapa_raton(-1);
                           /*
                           for (i = campini;i <= campfin;i++)
                           {
                              //pon_campo_en_el_raton(ipfi,i,0);
                           }
                           */
                           TestigoRaton = 0;
                           
						   /*pulsatecla();*/


						  ant_opcion = opcion;

						  memset(ml_letrasi,0,sizeof(ml_letrasi));
						  /* intercambio de opciones */
						  {
							  int i;
							  for (mi = 0;l_letrasi[mi];mi++);							  
							  mi--;
							  for (i = 0;i < mi;i++)
							  {
								  ml_textoi[i] = l_textoi[i+1];
								  ml_letrasi[i] = l_letrasi[i+1];
							  }
							  mi++;
							  ml_textoi[i] = l_textoi[0];
							  ml_letrasi[i] = l_letrasi[0];

							  if (opdef == 1)
								  opdef = mi;
							  else
								  opdef--;
						  }	
						  

						  opcion = _menusg(ml_textoi,ml_letrasi,posm*100,"",opdef,&b_wids,v_mpfi(ipfi));

						  {
							  if (opcion > 0)
							  {
								  if (opcion == mi)
									  opcion = 1;
								  else
									  opcion++;
							  }
						  }

						  if (opcion == 1)
							  opcion = -1;

						  if (opcion != 5 && opcion != 6 && opcion != 8 && opcion != 9)
						  {		 
							 _fin_botones_(&b_wids,v_mpfi(ipfi));
						  }


                           if (opcion == SOLICITA_POSICION)
                           {
							  if (manteblanco)
							  {
								  opcion = opdef;
								  continue;
							  }
                              if (!TestigoRaton)
                                 TestigoRaton = 1;
                              else
                              {
                                 pito(16);
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
                              cabecera_v(NULL,proceso,titulo,empresa);
                           }
                           switch (opcion) {
                           case 2 :          /* alta */
									  if (mimodo_debug || logger_salida)
									  {
										 sprintf(log_tmp,__Texto[260],quita_path(v_mpfi(ipfi)->f_fichero),v_mpfi(ipfi)->f_titulo);
										 /*pinta_en_logger(log_tmp);*/
										 registro_logger("D",11,quita_path(v_mpfi(ipfi)->f_fichero),0,"",log_tmp,"","",quita_path(v_mpfi(ipfi)->f_oldfichero));
									  }
                              bb = balance;
                              while (alta_ficha(ipfi,sinbotones) == 0);
                              /*****
                              if (bb != balance) {
                              printf("\n\r no cuadra el balance de mallocs %d -> %d. \n\r",bb,balance);
                              getchar();
                              }
                              ******/
							  if (!manteblanco)
							  {
								  if (leer_reg(ipfi,ISCURR,0,1) < 0)
								  {
									  leer_reg(ipfi,ISFIRST,0,1);
								  }
							  }
							  else
							  {
								  limpia_ficha(ipfi);
							  }
							  pinta_datos(ipfi,0);
                              
									  if (mimodo_debug || logger_salida)
									  {
                                 
										 sprintf(log_tmp,__Texto[261],quita_path(v_mpfi(ipfi)->f_fichero),v_mpfi(ipfi)->f_titulo);
										 /*pinta_en_logger(log_tmp);*/
										 registro_logger("D",21,quita_path(v_mpfi(ipfi)->f_fichero),0,"",log_tmp,"","",quita_path(v_mpfi(ipfi)->f_oldfichero));
									  }
                              
                              break;
                           case 3 :          /* modificacion */
									  if (mimodo_debug || logger_salida)
									  {
                                 
										 sprintf(log_tmp,__Texto[262],quita_path(v_mpfi(ipfi)->f_fichero),v_mpfi(ipfi)->f_titulo);
										 /*pinta_en_logger(log_tmp);*/
										 registro_logger("D",13,quita_path(v_mpfi(ipfi)->f_fichero),0,"",log_tmp,"","",quita_path(v_mpfi(ipfi)->f_oldfichero));
									  }

							  modifica1_ficha(ipfi,nclave,sinbotones);
							  TestigoRaton = 0;

							  if (leer_reg(ipfi,ISCURR,0,1) < 0)
							  {
								  leer_reg(ipfi,ISFIRST,0,1);
							  }
							  pinta_datos(ipfi,0);
                              
                              /*while (modif_ficha(ipfi) == 0);*/

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

									  if (!manteblanco)
									  {										  
										  if (leer_reg(ipfi,ISCURR,0,1) < 0)
										  {
											  leer_reg(ipfi,ISFIRST,0,1);
										  }
										  pinta_datos(ipfi,0);
									  }
                              
									  if (mimodo_debug || logger_salida)
									  {
										 sprintf(log_tmp,__Texto[265],quita_path(v_mpfi(ipfi)->f_fichero),v_mpfi(ipfi)->f_titulo);
										 /*pinta_en_logger(log_tmp);*/
										 registro_logger("D",22,quita_path(v_mpfi(ipfi)->f_fichero),0,"",log_tmp,"","",quita_path(v_mpfi(ipfi)->f_oldfichero));
									  }
                              break;
						   case 5:
							   consulta_lee_first(ipfi);
							   pinta_datos(ipfi,0);
							   break;
						   case 6:
                              if (consulta_lee_prev(ipfi) < 0) 
							  {
                                 if (iserrno == EENDFILE) 
								 {
                                    Ar();
                                    aviso(0,"Primera Ficha");
                                    Cr();
                                    leer_reg(ipfi,ISEQUAL,0,1);
                                 }
                              }
							  pinta_datos(ipfi,0);
							  break;
						   case 7:
							   j = entra_clave(ipfi,&nclave,CONSULTA,sinbotones);
							   if (j < 0)
							   {
								   nclave = oc;
								   break;
							   }
							   if (j == SOLICITA_POSICION)
							   {
								 if (!TestigoRaton)
									TestigoRaton = 1;
								 else
								 {
									pito(17);
									TestigoRaton = 0;
								 }
								 nclave = oc;
								 break;
							   }
							   if (nclave < 0)
							   {
								  nclave = oc;
							   }
							   r = 0;
							   if (oc != nclave)
							   {
									if (!clave_de_consulta)
									   r = cambia_ind(v_mpfi(ipfi),nclave+1);
									else
									   r = apunta_ind(v_mpfi(ipfi),nclave+1,ISCURR,0);
							   }
							   if (r == 0)
							   {
								   oc = nclave;
								   if (clave_de_consulta == 0)
									   r = leer_reg(ipfi,ISGTEQ,0,1);
								   else
									   r = leer_reg(ipfi,ISCURR,0,1);
							   }
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
								   aviso(0,"Clave inexistente");
								   Cr();
								   break;
							   }		
							   pinta_datos(ipfi,0);
							   break;
						   case 8:
                              if (consulta_lee_next(ipfi) < 0) 
							  {
                                 if (iserrno == EENDFILE) 
								 {
                                    Ar();
                                    aviso(0,"Ultima Ficha");
                                    Cr();
                                    leer_reg(ipfi,ISEQUAL,0,1);
                                 }
                              }
							  pinta_datos(ipfi,0);
							  break;
						   case 9:
							   consulta_lee_last(ipfi);
							   pinta_datos(ipfi,0);
							   break;
                           default :
                              if (opcion == otra_opcion && memoplin > 4)
                              {
                                 strcpy(v_memo(5004)->alfabetica,"");
                                 ejecuta_otro(ipfi,20,O_NOR,0,0);
                                 break;
                              }
							  else if (opcion == opl) {
								  trata_lineas(ipfi,CONSULTA,0,0,0);
								 /*numpan -=1;
								 estoyen = numpan;*/
								 if (trata_lineas(ipfi,CONSULTA,3,0,0) != 0) 
								 {
									/*numpan -=1;
									continue;*/
								 }
								 break;
							   }
							  else if (opcion > 9) {

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
							  }
                              
                              break;
                           }
                           
                           
                }
          }

		  _fin_botones_(&b_wids,v_mpfi(ipfi));
          
          ejecuta_otro(ipfi,9,O_NOR,0,0);
          
          
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


