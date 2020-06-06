
/**********************************************************************/
/**********************************************************************/
/*                    DIAGRAM9                                  */
/**********************************************************************/
/**********************************************************************/


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
#define SEEK_END 2
#ifndef LARGESEEK
extern long rw_lseek_();
#endif
extern unsigned int rw_read_();
#endif

#ifdef ESDOS
#include <dos.h>
#else
#define SOCKET int
#define SOCKET_ERROR -1
#define INVALID_HANDLE_VALUE -1
#define closesocket close
#define ioctlsocket ioctl
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
#include <fgvsttab.h>

#include "fgvixx.h"

extern char rw_version[256];

extern int fecha_protegida;
extern char *numero_serie;

extern int rentra_atrib;
extern int rentra_sal_atrib;
extern int rentra_cc;
extern int rentra_cf;
extern int rentra_sal_cc;
extern int rentra_sal_cf;

extern int modo_de_relacion;

extern long trata_memo();
extern char *cambia_path_fich();

extern long da_registo_actual();

extern long que_registro();

extern double ld_doble();

extern int es_la_doble_;
extern int va_con_doble_;


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

extern int usar_grid;

extern FILE *_imp;
extern char *aginfo;
extern int LEERTERMINAL;
extern char nmen0[551];

extern char *rw_usuario;

extern char *lee_linea();

extern int user_who;
extern int super_usuario;
extern char rw_user[11];
extern char *quita_path();


extern int default_id;
extern int default_sid;
extern int default_wid;

extern int default_wcid;
extern int default_ctipo;
extern int default_cid;
extern int default_refcid;

char nom_inf[256];
extern char numerofax[300];

extern int modo_desatendido;

extern int recur_punto;

extern int ultima_instalacion;

extern char *filtro_consulta_remota;

extern int gridextras;

extern int sinescape;

extern int tracer_int;

extern int ignorar_aviso_64;

extern int no_report_error;

/****************************/
/*agicont,I,X,6,0,A,0,fecha,Contabilidad Gorda,control1,control2,0,0,a,i+d.alcoy,agicont/fich/agicont.opc,0*/
struct info_paquete
{  
   int  version1;
   int  version2;
   int  autoversion;
   int  ver_interna;
   int  num_interno;
   int  rdepende1;
   int  rdepende2;
   int  dependesubsistema;
   int  rdependemodo;
   int  pdepende1;
   int  pdepende2;  
   int  pdependemodo;
   unsigned char versionletra;  
   char tipo;
   char sistema[16];
   char fecha[21];
   char descrip[31];
   unsigned char rdependeletra;
   unsigned char pdependeletra;
   char origen[21];
   long build_numero;
   char es_ayuda[9];
};

struct paquetes
{
   char nompaq[20];
   struct info_paquete i;
   long control_utime;    
   int flag;
};
extern struct paquetes Paq[100];
extern int nPaq;
extern int instalacion_silenciosa;

struct info_paquete *datos_paquete();
struct info_paquete *instala_paquete();
/****************************/

extern int n_np;

#ifdef RWXARXA
extern int en_conexion_terminal;
#ifdef RWSERVER
extern int en_conexion_externa;
extern int remoto_es_sistema_grafico;
#endif
#endif

extern void pinta_en_logger();


#ifdef FGV32

extern BOOL fIsTNT;

#endif

extern int pasa_a_int(double);

extern void (*RetroFuncion[4096])(char *bloque,char *par,int el_punto);
extern int RetroTipo[4096];
extern char *RetroPar[4096];
extern int RetroPunto;


/* ********* Clave de HELP ********** */

extern char clave_help[32];

/**************************************/

static char *letrascon = "AM";
static char *textocon[] = {" Automatico ",
" Manual ",
};

static char tmp_s[MAXDEFECTO];


/*#ifdef CONGRID*/

struct mini_cache
{
    long id_reg;
    long isrec;
    char *buf;
};



struct _sql_campo
{
    char id[100];
	char alias[100];
};

struct _sql_campos
{
	char modif[100];
	int ncampos;
	struct _sql_campo campos[350];
};

struct _sql_origen
{
    int ipadre;
	int icampo;
};

struct _sql_relacion
{
	int ofd;
	int nrel;
	int frel[50];
	int ncrel[50];
	int rel[50][100];
	int relh[50][100]; /* campos correspondientes en el padre = 0 para directa a clave primaria (compatibilidad) */
	int hrel[50]; /* clave a usar en el padre 0 = sin clave busqueda a pelo */
	long *mapa_ordenacion[50];
	long nmapa_ordenacion[50];	
};

struct _sql_orden_lectura
{
	int file;
	int rela;
};

struct _sql_from
{
	int nfrom;
	char from[50][100];

};

struct _sql_into
{
	int tipo;
	char into[100];
};

struct _sql_where /* prueba version solo con ands y sin parentesis */
{	
	int inverso;	
	char campo[100];
	int cond;
	char valor[350];
	char valor1[350];
	int esor;
};

struct _sql_wheres
{
	int ncond;
	struct _sql_where where[100];
};

struct _sql_orderby
{
	int ncampos;
	char campos[50][100];
};

struct _sql_orderbys
{	
	int norders;
	int agrupado;
	struct _sql_orderby orderby[100];
};


struct _sql_values 
{	
	int nvalues;	
	char valor[350][350];
};


struct _sql_toscreen
{
	FI_DEF *pfi;
	FI_DEF *padrepfis[100];
	int padrecargado[100];
	int nfiltros[100];
	int nf;
	struct grid_filtro *filtros[100];
    struct _sql_origen *origen;
    struct _sql_relacion *relacion;
	int campototal;
	int totales;
	int camptot[300];
};

struct _sql_output_
{
	FI_DEF *pfi;
	int (*out_fun)();
	double resultados;
	int hay;
	int nt;
	void *param;	
	struct _sql_origen *origen;
	struct gridpars *par;
    long contad/* = time(NULL)*/;
    long tactual;
	long tpulsa;
	int el_punto;
	char *buf;	 /* buffer temporal para acumulacion etc */
	int totales; /* campo (+1) clave de subtotal */
	int opcampo[300]; /* operacion en campos  */
	int agrupacion; /* flag agrupado */
	long registropadre;
	int distinct;
};


struct gridpars
{
   int structid;
   char *titulo;
   struct Info_Entra_Campos *InfoCampos;
   int ncampos;
   int *colcampos;
   FI_DEF *pfi;
   long reg;   
   long regsel;
   long regcur;
   long registros;
   int encl;   
   long offset;
   int id_grid;
   int limi1;
   int limi2;
   int hook;   
   int gmodo;
   int pi;
   int pf;
   int curcol;
   char *for_free;
   struct mini_cache *cache;
   struct gridpars *padre;
   int *hijos;
   int atencion;
   int nfiltros;
   struct grid_filtro *filtros;
   int procesando;
   int _wid;
   int _id;
   int _sid;
   int ipfi;
   int Independiente;
   int pact;
   char *b_lim1;
   char *b_lim2;
   int  control_ipfi;
   int  control_campo;
   /*
   int  nmapa_de_asignacion;
   long *mapa_de_asignacion;   
   */
   int  nmapa_de_ordenacion;
   long *mapa_de_ordenacion;   
   int  *campowids;
   int  *claves_validas;
   int  *totales;
   double *t_totales;
   int totales_sucios;
   char autofiltro[MAXPATH+1];   
   struct _sql_toscreen *sqltoscreen;
   int lineas;
   int lineas_ipfi;
   int campo_de_ordenacion;
   int rgbcolor;
   char **multiples_ficheros;
   int indice_multiples_ficheros;
   int temporal;
   int sesion_id;
};


extern int ap_sesion_id;

void dsdb_sql(char *origen,char *salida,FI_DEF **fires,int (*out_fun)(),void *param);
static int sql_select_ejecuta(FI_DEF *pfi,FI_DEF *padrepfis[],int nfiltros[],struct grid_filtro *filtros[],struct _sql_origen *origen,struct _sql_relacion *relacion,char *buffers[],struct gridpars *par,int el_punto,int (*out_fun)(),void *param,int nf,int agrupacion, int distinct);

extern long IxxNumeroPosicion;
#ifndef IDOK
#define IDOK                1
#endif
#ifndef IDCANCEL
#define IDCANCEL            2
#endif 
#ifndef IDABORT
#define IDABORT             3
#endif 
#ifndef IDRETRY
#define IDRETRY             4
#endif 
#ifndef IDIGNORE
#define IDIGNORE            5
#endif 
#ifndef IDYES
#define IDYES               6
#endif 
#ifndef IDNO
#define IDNO                7
#endif 
#ifndef IDCLOSE
#define IDCLOSE         8
#endif 
#ifndef IDHELP
#define IDHELP          9
#endif 


extern int ixx_nocase;

#define SALVA_IDS  int _default_id  = default_id;int _default_sid = default_sid;int _default_wid = default_wid;
#define REPON_IDS  default_id  = _default_id;default_sid = _default_sid;default_wid = _default_wid;


static int rellena_mapa_ordenacion(struct gridpars *par,int campo);

int EntraLineaGrid(char *bloque,struct gridpars *par,int el_punto);

static struct Info_Entra_Campo *dInfoCampo(struct Info_Entra_Campos *info,int cam,int dato)
{
    struct Info_Entra_Campo *campo;
    char *p;
    int d;


    d = *((int *)((char *) ((char *)info) + info->d_Campos + (cam*sizeof(int)) ));
    campo = (struct Info_Entra_Campo *)((char *) ((char *)info) + d );

    p = (char *)campo;

    switch(dato)
    {
    case 1:
      p += campo->d_nombre;
      campo = (struct Info_Entra_Campo *)p;
      break;
    case 2:
      p += campo->d_minimo;
      campo = (struct Info_Entra_Campo *)p;
      break;
    case 3:
      p += campo->d_maximo;
      campo = (struct Info_Entra_Campo *)p;
      break;
    case 4:
      p += campo->d_usi;
      campo = (struct Info_Entra_Campo *)p;
      break;
    default:
      break;
    }

    return campo;
}


static int InicializaLimite(struct gridpars *par)
{
int limi1 = par->limi1;
int limi2 = par->limi2;
FI_DEF *pfi = par->pfi;
char *buf;
char *p;

  buf = malloc(pfi->f_lfich+1);
  memcpy(buf,pfi->f_buf,pfi->f_lfich);

    if (limi1)
    {
        defecto_ficha(pfi);
        if (par->b_lim1) 
           free(par->b_lim1);
        par->b_lim1 = malloc(pfi->f_lfich+1);
        p_ejecuta(limi1,O_NOR,0,22);      
        memcpy(par->b_lim1,pfi->f_buf,pfi->f_lfich);
		if (!par->claves_validas)
		{
			p = v_memo(5004)->alfabetica;
			p = strstr(p,"{K:");
			if (p)
			{
				char *p1;
				int n,i;
				p += 3;
				p1 = p;
				while(*p1 && *p1 != '}') p1++;
				n = (int)(p1 - p) / 3;
				n++;
				par->claves_validas = (int *)malloc((n+1)*sizeof(int));
				memset(par->claves_validas,0,(n+1)* sizeof(int));
				par->claves_validas[0] = n;
				par->claves_validas[1] = -1; /* aponer el defecto */
				for (i = 1;i < n;i++)
				{
					par->claves_validas[i+1] = (int)((*p)-48) * 100;
					par->claves_validas[i+1]+= (int)((*(p+1))-48) * 10;
					par->claves_validas[i+1]+= (int)((*(p+2))-48) - 1;
					p += 3;
				}
			}
		}
		if (!par->totales)
		{
			p = v_memo(5004)->alfabetica;
			p = strstr(p,"{T:");
			if (p)
			{
				char *p1;
				int n,i;
				p += 3;
				p1 = p;
				while(*p1 && *p1 != '}') p1++;
				n = (int)(p1 - p) / 3;			
				par->totales = (int *)malloc((n+1)*sizeof(int));
				memset(par->totales,0,(n+1)* sizeof(int));
				par->t_totales = (double *)malloc(n*sizeof(double));
				par->totales[0] = n;			
				for (i = 0;i < n;i++)
				{
					par->t_totales[i] = 0.0;
					par->totales[i+1] = (int)((*p)-48) * 100;
					par->totales[i+1]+= (int)((*(p+1))-48) * 10;
					par->totales[i+1]+= (int)((*(p+2))-48);
					p += 3;
				}			
			}		
		}
    }

    if (limi2)
    {
        defecto_ficha(pfi);
        if (par->b_lim2) 
           free(par->b_lim2);
        par->b_lim2 = malloc(pfi->f_lfich+1);
        p_ejecuta(limi2,O_NOR,0,23);
        memcpy(par->b_lim2,pfi->f_buf,pfi->f_lfich);
    }

   memcpy(pfi->f_buf,buf,pfi->f_lfich);
   free(buf);

return 0;
}

static int BorraLosHijos(struct gridpars *par)
{
  int *hijos;    
  if (par->hijos)
  {
      hijos = par->hijos;
      while(*hijos > -2)
      {
          BorraHijoGrid(*hijos,0);/****/
          hijos++;
      }
      free((char *)par->hijos);
      par->hijos = NULL;
  }
  return 0;
}

static int libera_sqltoscreen(struct _sql_toscreen *sqltoscreen)
{
	int i;
	if (!sqltoscreen)
		return -1;

	free((char *)sqltoscreen->filtros[0]);

	for (i = 0;i < sqltoscreen->nf;i++)
	{		
		if (sqltoscreen->padrecargado[i])
		{
			free_defpri(sqltoscreen->padrepfis[i]);
		}
	}
	if (sqltoscreen->origen)
		free((void *)sqltoscreen->origen);
	if (sqltoscreen->relacion)
		free((void *)sqltoscreen->relacion);

	free(sqltoscreen);
	
	return 0;
}

static int LanZaHiJo(struct gridpars *par,char *titulo,int nfiltros,struct grid_filtro *filtros,int ncolucam,int *colucam,int noborra,int eventohijo,int sql)
{
    struct gridpars *mihijo;
    FI_DEF *pfi;
    int *hijos;    
    int i,j,k;
    int np;
	int laclave;
	char nomdef[128];
	int temporal = 0;
	extern int flag_leedefn;

    
    if (par->procesando)
    {        
        aviso(0,"No se puede activar otra busqueda mientras no haya acabado la anterior");
		if (sql == 1)
		{
			libera_sqltoscreen((struct _sql_toscreen *)filtros);
		}
        return -1;
    }

	if (!noborra)
	{
		BorraLosHijos(par);
	}

	if (sql == 1)
	{
		if (!filtros)
		{
			return -1;
		}
		pfi = ((struct _sql_toscreen *)filtros)->pfi;
		laclave = pfi->f_nclaves - 1;
	}
	else
	{
		pfi = NULL;
		if (copia_defn(&pfi,par->pfi) < 0) return -1;
		/*
		flag_leedefn = 0;
		nomdef[0] = 5;
		strcpy(nomdef+1,par->pfi->f_PathDef);
		if (lee_defn(nomdef,&pfi) < 0) return -1;
		*/

		laclave = par->encl;
	}

    if (filtros)
    {		
		if (sql == 1)
		{
			/* nada ... */
		}
		else
		{
			int hay_orden = 0;
			/* modificar claves .... segun filtros */
			for (i = 0;i < nfiltros;i++)
			{
				if ((filtros[i].tipo & 0x10))
				{
					hay_orden++;
				}
			}
			if (hay_orden)
			{
				if (hay_orden >= MAXCLAVES)
				{
					hay_orden = MAXCLAVES-1;
				}
				for (i = 1;i < pfi->f_nclaves;i++) 
				{
					free((char *)pfi->f_clave[i].k_campo);
					free((char *)pfi->f_clave[i].k_orden);
				}
				if (pfi->f_nclaves < (hay_orden+1))
				{
					char *tmpx;
					tmpx = reserva_malloc(sizeof(KEY_DEF) * (hay_orden+1),3);
					memcpy(tmpx,(char *)pfi->f_clave,sizeof(KEY_DEF));
					reserva_free((char *)pfi->f_clave,3);
					pfi->f_clave = (KEY_DEF *)tmpx;
				}
				pfi->f_nclaves = hay_orden+1;
				for (i = 1;i < pfi->f_nclaves;i++)
				{
					pfi->f_clave[i].k_ncampos = 1;
					pfi->f_clave[i].k_fflag = 0;
					pfi->f_clave[i].k_campo = (short *)malloc(sizeof(short) * 2);
					pfi->f_clave[i].k_orden = (short *)malloc(sizeof(short) * 2);
				}
				hay_orden = 1;
				for (i = 0;i < nfiltros && hay_orden < pfi->f_nclaves;i++)
				{
					if ((filtros[i].tipo & 0x10))
					{
						pfi->f_clave[hay_orden].k_campo[0] = filtros[i].campo+1;
						pfi->f_clave[hay_orden].k_campo[1] = 0;
						if ((filtros[i].tipo & 0x20))
						{
							pfi->f_clave[hay_orden].k_orden[0] = -1;
							pfi->f_clave[hay_orden].k_orden[1] = 0;
						}
						else
						{
							pfi->f_clave[hay_orden].k_orden[0] = 1;
							pfi->f_clave[hay_orden].k_orden[1] = 0;
						}
						hay_orden++;					
					}
				}
				for (i=0;i<pfi->f_ncampos;i++)
				{
					pfi->f_campos[i].c_key = 0;
				}
				for (i=pfi->f_nclaves;i>0;i--) 
				{
					for (j=pfi->f_clave[i-1].k_ncampos;j>0;j--) 
					{
						if((k = pfi->f_clave[i-1].k_campo[j-1]) <= pfi->f_ncampos && k)
						{
							pfi->f_campos[k-1].c_key = i; 
						}
					}
				}
				laclave = 1;
			}
		}

        strcpy(pfi->f_fichero,"__XXXXXX");
        cambia_path_fich(pfi->f_fichero);
#ifndef ESDOS
		{
			int ff = mkstemp(pfi->f_fichero);
			if (ff >= 0)
				close(ff);
		}
#else
        mktemp(pfi->f_fichero);
#endif

        {
            FILE *ff = fopen(pfi->f_fichero,"w");
            if (ff)
            {
                fclose(ff);
            }
        }

        defecto_ficha(pfi);
        pfi->f_fd = -1;
        if (crea_ind(pfi) < 0)
        {
            borra_ind(pfi);			
            rw_base_unlink(pfi->f_fichero);
            free_defpri(pfi);        
			if (sql == 1)
			{
				libera_sqltoscreen((struct _sql_toscreen *)filtros);
			}
            return -1;
        }
        cierra_ind(pfi);
		temporal = 1;
    }
    else
    {
        strcpy(pfi->f_fichero,par->pfi->f_fichero);
    }

    if (abre_ind(pfi,ISINOUT + ISMANULOCK) < 0)
    {
        if (filtros)
        {
            borra_ind(pfi);
            rw_base_unlink(pfi->f_fichero);
        }
		if (sql == 1)
		{
			libera_sqltoscreen((struct _sql_toscreen *)filtros);
		}
        free_defpri(pfi);
        return -1;
    }

	if (!filtros)
    {
		pon_registro_actual(pfi,par->regsel );
		apunta_ind(pfi,0,ISEQUAL,0);
		apunta_ind(pfi,par->encl+1,ISCURR,0);
	}
	else
	{
		apunta_ind(pfi,par->encl+1,ISFIRST,0);
	}

    mihijo = (struct gridpars *)malloc(sizeof(struct gridpars));
    memset((void *)mihijo,0,sizeof(struct gridpars));
    mihijo->structid = 0x3212;
	mihijo->sesion_id = ap_sesion_id;
	mihijo->rgbcolor = -1;

    mihijo->titulo = malloc(strlen(titulo)+1);
    strcpy(mihijo->titulo,titulo); 
    
    mihijo->pfi    = pfi;
    mihijo->ipfi   = -1;
    mihijo->encl   = laclave;
	mihijo->temporal = temporal;

	if (!filtros)
    {
		memcpy(pfi->f_buf,par->pfi->f_buf,pfi->f_lfich);
		mihijo->limi1  = par->limi1;
		mihijo->limi2  = par->limi2;
		if (!par->b_lim1 || !par->b_lim2)
		{
			InicializaLimite(par);
		}
		mihijo->b_lim1 = NULL;
		if (par->b_lim1)
		{
			mihijo->b_lim1 = malloc(pfi->f_lfich+1);
			memcpy(mihijo->b_lim1,par->b_lim1,pfi->f_lfich);
		}
		mihijo->b_lim2 = NULL;
		if (par->b_lim2)
		{
			mihijo->b_lim2 = malloc(pfi->f_lfich+1);
			memcpy(mihijo->b_lim2,par->b_lim2,pfi->f_lfich);
		}
	}
    mihijo->hook   = 0;
    mihijo->pact   = 0;
	if (par->id_grid > -1)
	{
		mihijo->gmodo  = 0x10000;
	}
	else
	{
		mihijo->gmodo  = 0;
	}
    mihijo->_id    = par->_id;
    mihijo->_sid   = par->_sid;
    mihijo->_wid   = par->_wid;
    mihijo->Independiente = 0; /* los hijo no son independientes del todo, deben ser destruidos con la 
                                 funcion borrahijogrid */

	if (eventohijo || (par->gmodo & 0x1) || (par->gmodo & 0x20000) || (par->gmodo & 0x8))
	{
		mihijo->gmodo |= 0x20000;
	}

    mihijo->pi = 0;
    mihijo->pf = 0;
    mihijo->padre = par;
    if (filtros)
    {
		if (sql == 1)
		{
			mihijo->sqltoscreen = (struct _sql_toscreen *)filtros;
			mihijo->filtros = NULL;
			if (mihijo->sqltoscreen->totales > 0)
			{
				int n = mihijo->sqltoscreen->totales;
				int i;

				mihijo->totales = (int *)malloc((n+1)*sizeof(int));
				memset(mihijo->totales,0,(n+1)* sizeof(int));
				mihijo->t_totales = (double *)malloc(n*sizeof(double));
				mihijo->totales[0] = n;			
				for (i = 0;i < n;i++)
				{					
					mihijo->totales[i+1] = mihijo->sqltoscreen->camptot[i];
					if ((mihijo->totales[i+1]&0xf0000) == 0x10000)
						mihijo->t_totales[i] = MINDOUBLE;
					else if ((mihijo->totales[i+1]&0xf0000) == 0x20000)
						mihijo->t_totales[i] = MAXDOUBLE;
					else
						mihijo->t_totales[i] = 0.0;
				}
			}
		}
		else
		{
			mihijo->sqltoscreen = NULL;
			mihijo->filtros = (struct grid_filtro *)malloc(sizeof(struct grid_filtro) * nfiltros);
			memcpy(mihijo->filtros,filtros,sizeof(struct grid_filtro) * nfiltros);
		}
        mihijo->gmodo |= 0x40000;
    }
    else
	{
        mihijo->filtros = NULL;
		mihijo->sqltoscreen = NULL;
	}
    mihijo->nfiltros = nfiltros;

	mihijo->hook = eventohijo;

    if (!par->hijos)
    {
        par->hijos = (int*)malloc(sizeof(int)*100);
        par->hijos[0] = -2;
    }
    
    hijos = par->hijos;

    while(*hijos > -2) hijos++;

    np = LanzaGrid(mihijo,ncolucam,colucam,NULL,NULL,NULL,NULL,NULL);

    *hijos = np;
    hijos++;
    *hijos = -2;
    
    return np;
}


static int ComparaClave(FI_DEF *pfi,int key,char *buf1)
{
   int i,campo,orden,compara;   
   
   for (i = 0;i < pfi->f_clave[key].k_ncampos;i++) 
   {
      campo = pfi->f_clave[key].k_campo[i] - 1;
      orden = pfi->f_clave[key].k_orden[i];
      
      compara = x_campocmp(campo,orden,pfi,pfi->f_buf,buf1,0);
      if (compara)
	  {
          return compara;
	  }
   }
   return(0);
}

static int ComparaClaveLimites(FI_DEF *pfi,int key,char *buf1,int *camp,int sentido)
{
   int i,campo,orden,compara;   
   
   for (i = 0;i < pfi->f_clave[key].k_ncampos;i++) 
   {
      campo = pfi->f_clave[key].k_campo[i] - 1;
      orden = pfi->f_clave[key].k_orden[i];
      
      compara = x_campocmp(campo,orden,pfi,pfi->f_buf,buf1,0);
	  if (!sentido && compara > 0)
	  {
		  *camp = i;
          return compara;
	  } 
	  else if (sentido == 1 && compara < 0)
	  {
		  *camp = i;
          return compara;
	  }
   }
   return(0);
}


static long TotalRegistros(struct gridpars *par)
{
long posini,posfin;
int limi1 = par->limi1;
int limi2 = par->limi2;
FI_DEF *pfi = par->pfi;
int key = par->encl;
long registros;
char *buf;
long rega;
long posa = IxxNumeroPosicion;
long _nmapa_de_ordenacion = par->nmapa_de_ordenacion;
long *_mapa_de_ordenacion = par->mapa_de_ordenacion;


/*
  if (par->mapa_de_ordenacion)
  {
	 aviso(0,"control de ordenacion de servidor 1");
  }
*/

  registros = tam_btrv(pfi);
  if (!registros)
  {
	  if ((limi1 || limi2) && (!par->b_lim1 || !par->b_lim2))
	  {
		 InicializaLimite(par);
	  }
      return 0L;
  }

  par->mapa_de_ordenacion = NULL;
  par->nmapa_de_ordenacion = 0;

  /*
  if (par->mapa_de_ordenacion)
  {
	  free((char *)par->mapa_de_ordenacion);
	  par->mapa_de_ordenacion = NULL;
  }
  */

  /*
  if (par->mapa_de_asignacion)
  {
	  free((char *)par->mapa_de_asignacion);
	  par->mapa_de_asignacion = NULL;
  }
  */

  buf = malloc(pfi->f_lfich+1);
  memcpy(buf,pfi->f_buf,pfi->f_lfich);

  rega = da_registo_actual(pfi);

  if (limi2 || par->b_lim2)
  {      
      if (!par->b_lim2)
      {
          InicializaLimite(par);
      }
      memcpy(pfi->f_buf,par->b_lim2,pfi->f_lfich);
      if (lee_reg(pfi,ISGREAT) < 0)
	  {
          if (lee_reg(pfi,ISLAST) < 0)
			  IxxNumeroPosicion = -1;
	  }
      else
	  {
          if (lee_reg(pfi,ISPREV) < 0)
			  IxxNumeroPosicion = -1;
	  }

      posfin = IxxNumeroPosicion;
	  /*
      if (!(par->gmodo & 128) && ComparaClave(pfi,key,par->b_lim2) > 0)
      {
          posfin = -1;
      } 
	  */
  }
  else
      posfin = registros - 1;
   
   
   if (limi1 || par->b_lim1)
   {      
      if (!par->b_lim1)
      {
          InicializaLimite(par);
      }
      memcpy(pfi->f_buf,par->b_lim1,pfi->f_lfich);
      if (lee_reg(pfi,ISGTEQ) < 0)
	  {
          //if (lee_reg(pfi,ISFIRST) < 0)
		  // IxxNumeroPosicion = -1;
		  posini = -1;
	  }
	  else
	  {
		  posini = IxxNumeroPosicion;
		  /*
		  if (!(par->gmodo & 128) && ComparaClave(pfi,key,par->b_lim1) < 0)
		  {
			  posini = -1;
		  } 
		  */
	  }
   }      
   else
      posini = 0L;

   registros = posfin - posini + 1;
   if (registros < 0 || posini == -1L || posfin == -1L)
   {
       registros = 0L;
	   posini = 0;
   }

   par->registros = registros;  
   par->offset = posini;  


   pon_registro_actual(par->pfi,rega);
   apunta_ind(par->pfi,0,ISEQUAL,0);
   apunta_ind(par->pfi,par->encl+1,ISCURR,0);
   memcpy(pfi->f_buf,buf,pfi->f_lfich);
   free(buf);

   IxxNumeroPosicion = posa;

   par->mapa_de_ordenacion = _mapa_de_ordenacion;
   par->nmapa_de_ordenacion = _nmapa_de_ordenacion;

   if (par->mapa_de_ordenacion && par->nmapa_de_ordenacion != registros)
   {
	   if (par->mapa_de_ordenacion)
	   {
			free(par->mapa_de_ordenacion);
			par->mapa_de_ordenacion = NULL;
			par->nmapa_de_ordenacion = 0;
			rellena_mapa_ordenacion(par,par->campo_de_ordenacion);
	   }
   }

   return(registros);
}

static int evaluador_simple_de_filtro(FI_DEF *pfi,int nfiltros,struct grid_filtro *filtros,FI_DEF *pfitest)
{
   int resultado = 0;
   int parcial;
   int i;
   int campo;
   char buffer[512];
   char valor[512];
   char *p,*p1;
   

   for (i = 0;i < nfiltros;i++)
   {
	   if (resultado && !(filtros[i].tipo & 0x40)) break;

	   if (i && !resultado && (filtros[i].tipo & 0x40))
		   return 0;

       parcial = 0;
       campo = filtros[i].campo;	   
       switch((filtros[i].tipo & 0xf))
       {
       case 1:
           st_campo(campo,pfitest,filtros[i].valor);		   
           if (x_campocmp(campo,1,pfi,pfi->f_buf,pfitest->f_buf,0))
              parcial = 1;
           break;
       case 2:
           ld_campo(campo,pfi,buffer);
		   strcpy(valor,filtros[i].valor);
		   if (filtros[i].mayus)
		   {
			   strmayus(buffer);
			   strmayus(valor);
		   }
           if ( strlen(valor) > strlen(buffer) )
               parcial = 1;
           else
           {
               if (memcmp(valor,buffer,strlen(valor)))
                   parcial = 1;
           }
           break;
        case 3:
           ld_campo(campo,pfi,buffer);
		   strcpy(valor,filtros[i].valor);
		   if (filtros[i].mayus)
		   {
			   strmayus(buffer);
			   strmayus(valor);
		   }
		   p1 = valor;
		   for (p = valor;!parcial;p++)
		   {
			   if (!*p || (*p == '\\' && *(p+1) == '\\'))
			   {
				   if (*p)
				   {
					   *p = 0;
					   p++;					   
				   }
				   if (strlen(p1) > strlen(buffer))
					   parcial = 1;
				   else
				   {
					   if (!strstr(buffer,p1))
						   parcial = 1;
				   }       				  
				   p1 = p+1;
			   }
			   if (!*p)
				   break;
		   }

           break;
		case 4:
			/* sin filtraje (solo orden) */
		   break;
        default:
           st_campo(campo,pfitest,filtros[i].limi);
           if (x_campocmp(campo,1,pfi,pfi->f_buf,pfitest->f_buf,0) < 0)
           {
               parcial = 1;
           }
           else
           {
               st_campo(campo,pfitest,filtros[i].lims);
               if (x_campocmp(campo,1,pfi,pfi->f_buf,pfitest->f_buf,0) > 0)
               {
                   parcial = 1;
               }
           }
           break;
       }
       if (filtros[i].inver)
       {
           if (parcial)
               parcial = 0;
           else
               parcial = 1;
       }           
       resultado = parcial;
   }

   return resultado;
}


static int esta_en_limites(struct gridpars *par)
{
FI_DEF *pfi;
int key;
int campo = 0;

   if (!par || !(par->gmodo & 128) || !par->b_lim2  || !par->b_lim1)
   {
	   return 1;
   }

   pfi = par->pfi;
   key = par->encl;


   if (ComparaClaveLimites(pfi,key,par->b_lim2,&campo,0) > 0)
      return 0;
   if (ComparaClaveLimites(pfi,key,par->b_lim1,&campo,1) < 0)
	   return 0;
   return 1;
}

/* Siempre en orden ascendente */
static int salta_limites(struct gridpars *par)
{
FI_DEF *pfi = par->pfi;
int key = par->encl;
int campo = 0;
int i,c,n,p;
int nosale = 0;
long ai = 0;

   if (!par->b_lim2  || !par->b_lim1)
   {
	   return 0;
   }
   do
   {
	   if (ComparaClaveLimites(pfi,key,par->b_lim2,&campo,0) > 0)
	   {
		   if (!campo)
			   return 1;
		   if (apunta_ind(pfi,key+1,ISGREAT,campo) < 0)
		   {
			   apunta_ind(pfi,key+1,ISCURR,0);
			   return 1;
		   }		   
		   lee_reg(pfi,ISCURR);
		   for (i = campo;i < pfi->f_clave[key].k_ncampos;i++) 
		   {
			   c = pfi->f_clave[key].k_campo[i] - 1;
			   p = pfi->f_campos[c].c_offset;
			   n = pfi->f_campos[c].c_long;
			   memcpy(pfi->f_buf+p,par->b_lim1+p,n);
		   }
		   if (lee_reg(pfi,ISGTEQ) < 0)
		   {
			   return 1;
		   }
	   }
	   else if (ComparaClaveLimites(pfi,key,par->b_lim1,&campo,1) < 0)
	   {
		   if (!campo)
			   return -1;
		   for (i = campo;i < pfi->f_clave[key].k_ncampos;i++) 
		   {
			   c = pfi->f_clave[key].k_campo[i] - 1;
			   p = pfi->f_campos[c].c_offset;
			   n = pfi->f_campos[c].c_long;
			   memcpy(pfi->f_buf+p,par->b_lim1+p,n);
		   }
		   if (lee_reg(pfi,ISGTEQ) < 0)
		   {
			   return -1;
		   }
	   }
	   else
		   break; 
	   if (nosale > 1000)
	   {
		   nosale = 0;
	   }
	   nosale++;
	   if (ai > IxxNumeroPosicion)
	   {
		   nosale = 0;
	   }
	   ai = IxxNumeroPosicion;
   } while(-1);
   return 0;
}



static int esta_en_limites_buf(FI_DEF *pfi,char *b_lim1,char *b_lim2,int key)
{
int campo = 0;

   if (!b_lim2  || !b_lim1)
   {
	   return 1;
   }

   if (ComparaClaveLimites(pfi,key,b_lim2,&campo,0) > 0)
      return 0;
   if (ComparaClaveLimites(pfi,key,b_lim1,&campo,1) < 0)
	   return 0;
   return 1;
}

/* Siempre en orden ascendente */
static int salta_limites_buf(FI_DEF *pfi,char *b_lim1,char *b_lim2,int key)
{
int campo = 0;
int i,c,n,p;
int nosale = 0;
long ai = 0;

   if (!b_lim2  || !b_lim1)
   {
	   return 0;
   }
   do
   {
	   if (ComparaClaveLimites(pfi,key,b_lim2,&campo,0) > 0)
	   {
		   if (!campo)
			   return 1;
		   if (apunta_ind(pfi,key+1,ISGREAT,campo) < 0)
		   {
			   apunta_ind(pfi,key+1,ISCURR,0);
			   return 1;
		   }		   
		   lee_reg(pfi,ISCURR);
		   for (i = campo;i < pfi->f_clave[key].k_ncampos;i++) 
		   {
			   c = pfi->f_clave[key].k_campo[i] - 1;
			   p = pfi->f_campos[c].c_offset;
			   n = pfi->f_campos[c].c_long;
			   memcpy(pfi->f_buf+p,b_lim1+p,n);
		   }
		   if (lee_reg(pfi,ISGTEQ) < 0)
		   {
			   return 1;
		   }
	   }
	   else if (ComparaClaveLimites(pfi,key,b_lim1,&campo,1) < 0)
	   {
		   if (!campo)
			   return -1;
		   for (i = campo;i < pfi->f_clave[key].k_ncampos;i++) 
		   {
			   c = pfi->f_clave[key].k_campo[i] - 1;
			   p = pfi->f_campos[c].c_offset;
			   n = pfi->f_campos[c].c_long;
			   memcpy(pfi->f_buf+p,b_lim1+p,n);
		   }
		   if (lee_reg(pfi,ISGTEQ) < 0)
		   {
			   return -1;
		   }
	   }
	   else
		   break; 
	   if (nosale > 1000)
	   {
		   nosale = 0;
	   }
	   nosale++;
	   if (ai > IxxNumeroPosicion)
	   {
		   nosale = 0;
	   }
	   ai = IxxNumeroPosicion;
   } while(-1);
   return 0;
}

static int clave_campo(FI_DEF *pfi,int campo,int *peso) {
	int k,i,p;
	if (peso) {
		*peso = 0;
	}
	p = 1000;
	if (pfi->f_campos[campo].c_tipo == CHARTIPO) {
		if (pfi->f_campos[campo].c_long == 1)
			p = 500;
		else if (pfi->f_campos[campo].c_long == 2)
			p = 600;
		else if (pfi->f_campos[campo].c_long == 3)
			p = 800;
	}
	campo++;
	for (k = 0;k < pfi->f_nclaves;k++) {
		for (i = 0;i < pfi->f_clave[k].k_ncampos;i++) {
			if (campo == pfi->f_clave[k].k_campo[i]) {
				if (peso) {
					if (i == 1)
						p -= 200;
					else if (i == 2)
						p -= 300;
					else if (i > 2)
						p -= (300+(i*50));

					if (p < 2)
						p = 2;
					*peso = p;
				}
				return k;
			}
		}
	}
	return -1;	
}

static int crea_limites_buf(int nfiltros,struct grid_filtro *filtros,FI_DEF *pfi,char **buf1,char **buf2) {
	int key = -1;
	int i,k,peso,campo,nf;
	int pesos[MAXCLAVES+1];

	for (i = 0;i < MAXCLAVES;i++)
		pesos[i] = 0;

	/* a saco de momento, ignorar en caso de or */
	for (i = 0;i < nfiltros;i++) {
		if ((filtros[i].tipo&0x40)) {
			return -1;
		}
	}
	
	for (i = 0;i < nfiltros;i++) {
		if (filtros[i].tipo < 3) {		
			k = clave_campo(pfi,filtros[i].campo,&peso);
			if (k > -1)
				pesos[k] += peso;
		}
	}	
	peso = 0;
	for (i = 0;i < MAXCLAVES;i++) {
		if (pesos[i] > peso) {
			key = i;
			peso = pesos[i];
		}
	}
	if (key > -1) {
		*buf1 = malloc(pfi->f_lfich+1);
		*buf2 = malloc(pfi->f_lfich+1);

		defecto_ficha(pfi);
		for (i = 0;i < pfi->f_clave[key].k_ncampos;i++) {
			campo = pfi->f_clave[key].k_campo[i] - 1;
			if (campo >= 0) {
				minimo_campo(pfi,campo);
				for (nf = 0;nf < nfiltros;nf++) {
					if (filtros[nf].campo == campo) {
						if (filtros[nf].tipo == 0) 
							st_campo(campo,pfi,filtros[nf].limi);						
						else if (filtros[nf].tipo == 1) 
							st_campo(campo,pfi,filtros[nf].valor);
						else if (filtros[nf].tipo == 2) {
							if (pfi->f_campos[campo].c_tipo == CHARTIPO) {
								memcpy(pfi->f_buf+pfi->f_campos[campo].c_offset,filtros[nf].valor,strlen(filtros[nf].valor));
							}
							else
								st_campo(campo,pfi,filtros[nf].valor);
						}
						break;
					}
				}
			}
		}

		memcpy(*buf1,pfi->f_buf,pfi->f_lfich);

		defecto_ficha(pfi);
		for (i = 0;i < pfi->f_clave[key].k_ncampos;i++) {
			campo = pfi->f_clave[key].k_campo[i] - 1;
			if (campo >= 0) {
				maximo_campo(pfi,campo);
				for (nf = 0;nf < nfiltros;nf++) {
					if (filtros[nf].campo == campo) {
						if (filtros[nf].tipo == 0) 
							st_campo(campo,pfi,filtros[nf].lims);
						else if (filtros[nf].tipo == 1) 
							st_campo(campo,pfi,filtros[nf].valor);
						else if (filtros[nf].tipo == 2) {
							if (pfi->f_campos[campo].c_tipo == CHARTIPO) {
								memcpy(pfi->f_buf+pfi->f_campos[campo].c_offset,filtros[nf].valor,strlen(filtros[nf].valor));
							}
							else
								st_campo(campo,pfi,filtros[nf].valor);
						}
						break;
					}
				}
			}
		}

		memcpy(*buf2,pfi->f_buf,pfi->f_lfich);
		

	}
	return key;
}

static int busca_contenido_buf(int nfiltros,struct grid_filtro *filtros,FI_DEF *pfi, int *key) {
	int contenido = -1;
	int i,k,peso;
	for (i = 0;i < nfiltros;i++) {
		if (filtros[i].tipo == 3) {
			k = clave_campo(pfi,filtros[i].campo,&peso);
			if (k > -1 && pfi->f_clave[k].k_ncampos == 1) {
				peso = filtros[i].campo;
				if (pfi->f_campos[peso].c_tipo == CHARTIPO) {
					contenido = i;
					*key = k;
					return contenido;
				}
			}
		}
	}
	return contenido;
}

/* Siempre en orden descendente */
static int salta_limites_atras(struct gridpars *par)
{
FI_DEF *pfi = par->pfi;
int key = par->encl;
int campo = 0;
int i,c,n,p;
int nosale = 0;
long ai = 2147483645L;

   if (!par->b_lim2  || !par->b_lim1)
   {
	   return 0;
   }
   do
   {
	   if (ComparaClaveLimites(pfi,key,par->b_lim1,&campo,1) < 0)
	   {
		   if (!campo)
			   return -1;
		   if (apunta_ind(pfi,key+1,ISLOW,campo) < 0)
		   {
			   apunta_ind(pfi,key+1,ISCURR,0);
			   return -1;
		   }		   
		   for (i = campo;i < pfi->f_clave[key].k_ncampos;i++) 
		   {
			   c = pfi->f_clave[key].k_campo[i] - 1;
			   p = pfi->f_campos[c].c_offset;
			   n = pfi->f_campos[c].c_long;
			   memcpy(pfi->f_buf+p,par->b_lim2+p,n);
		   }
		   if (lee_reg(pfi,ISLOWEQ) < 0)
		   {
			   return -1;
		   }
	   }
	   else if (ComparaClaveLimites(pfi,key,par->b_lim2,&campo,0) > 0)
	   {
		   if (!campo)
			   return 1;
		   for (i = campo;i < pfi->f_clave[key].k_ncampos;i++) 
		   {
			   c = pfi->f_clave[key].k_campo[i] - 1;
			   p = pfi->f_campos[c].c_offset;
			   n = pfi->f_campos[c].c_long;
			   memcpy(pfi->f_buf+p,par->b_lim2+p,n);
		   }
		   if (lee_reg(pfi,ISLOWEQ) < 0)
		   {
			   return 1;
		   }
	   }
	   else
		   break; 
	   if (nosale > 1000)
	   {
		   nosale = 0;
	   }
	   nosale++;
	   if (ai < IxxNumeroPosicion)
	   {
		   nosale = 0;
	   }
	   ai = IxxNumeroPosicion;
   } while(-1);
   return 0;
}


#ifdef VIEJOMAPA
	if (skipped)
	{		
		if (!par->mapa_de_asignacion)
		{	
			par->registros -= skipped;

			par->mapa_de_asignacion = (long *)malloc(sizeof(long)*(par->registros+1));
			par->nmapa_de_asignacion = par->registros;
			par->mapa_de_asignacion[par->nmapa_de_asignacion] = 0;
			for (i = 0;i < (opos-par->offset);i++)
			{
				par->mapa_de_asignacion[i] = i;
			}
			par->mapa_de_asignacion[opos-par->offset] = posicion;
			j = 1;	
			
            if (par->regcur > (opos+1))
			{
				par->regcur -= skipped;
				if (par->regcur < (opos+1))
				{
					par->regcur = opos+1;
				}
				if (par->regcur > (par->registros+par->offset))
				{
					par->regcur = par->registros+par->offset;
				}
			}
			
			for (i = opos+1-par->offset;i < par->registros;i++)
			{
				par->mapa_de_asignacion[i] = posicion + j;
				j++;
			}
		}
		else
		{
			par->mapa_de_asignacion[opos-par->offset] = posicion;
			j = 1;
			flag = 0;
			for (i = opos+1-par->offset;i < par->registros;i++)
			{
				if (par->mapa_de_asignacion[i] >= 0)
				{					
					if (!flag && par->mapa_de_asignacion[i] == posicion)
					{
						skipped = i-(opos-par->offset);
						flag = 1;
					}					
					par->mapa_de_asignacion[i] = posicion+j;
					j++;
				}
			}
			par->registros -= skipped;
			if (par->regcur > (opos+1))
			{
				par->regcur -= skipped;
				if (par->regcur < (opos+1))
				{
					par->regcur = opos+1;
				}
				if (par->regcur > (par->registros+par->offset))
				{
					par->regcur = par->registros+par->offset;
				}
			}
		}
		return 999;
	}
#endif


static int grid_lee_por_posicion(struct gridpars *par,int key,long posicion,Uchar *dat_reg,int modo,int flags,int *_from,int *_to)
{
	if (
		 lee_por_posicion(par->pfi,key,posicion,dat_reg,modo,flags) < 0 
	   )
	{
		return -1;
	}
	if (!esta_en_limites(par) || !hay_dat_permiso(par->pfi,-2))
	{
		int campo = -1;
		if (key > 0 && key <= par->pfi->f_nclaves) {
			campo = par->pfi->f_clave[key-1].k_campo[0] - 1;
		}
		rellena_mapa_ordenacion(par,campo);
		if (!par->mapa_de_ordenacion)
			return -1;
		return 999;
	}
	return 0;
}

#ifdef ENCONSERVA
static int grid_lee_por_posicion(struct gridpars *par,int key,long posicion,Uchar *dat_reg,int modo,int flags,int *_from,int *_to)
{
	int r;
	long skipped = 0;
	long opos,i,j,flag;	
	long salto = 0L;
	long nn,corriente = -1L;
	long mejorcorriente = -1L;
	long mji = -1;
	/*int from = -1,to = -1;*/
	/*int iniciosalto = 1;*/

	
	if ((posicion-par->offset) >= par->registros)
	{
		return -1;
	}
	if (par->registros < 1)
	{
		return -1;
	}

	opos = posicion;

	if (par->mapa_de_asignacion)
	{
		if ((posicion-par->offset) >= par->nmapa_de_asignacion)
		{
			return -1;
		}
		salto = par->mapa_de_asignacion[opos-par->offset];
		posicion = salto;
		/*
		if (*_from)
		{
			from = par->mapa_de_asignacion[*_from];
			if (from >= 0)
				from += *_from;
		}
		if (*_to)
		{
			to = par->mapa_de_asignacion[*_to];
			if (to >= 0)
				to += *_to;
		}
		*/
		if (salto < 0)
		{
			while(opos > 0 && par->mapa_de_asignacion[opos-par->offset] < 0) opos--;
		}	
		else
		{
			/*
			if (opos > 0 && par->mapa_de_asignacion[(opos-par->offset)-1] == salto)
			{
				iniciosalto = 0;
			}
			*/
			if (!salto)
			{
				posicion = opos;
			}
			else
			{
				posicion += opos;
			}			
		}
	}
	/*
	else
	{
		if (*_from)
		{
			from = *_from;
		}
		if (*_to)
		{
			to = *_to;
		}
	}
	*/
	if (posicion < 0)
	{		
		par->registros = opos-par->offset;
		if (par->regcur >= (par->registros+par->offset))
		{
			par->regcur = (par->registros+par->offset);
			if (par->regcur <= par->offset)
				par->regcur = par->offset+1;
		}
		return -1;
	}	

	do
	{
		if (
			 (r = lee_por_posicion(par->pfi,key,posicion,dat_reg,modo,flags)) < 0 
		   )
		{
			if (ixxstatus == IXXECORRUP)
				return -1;
facil_salida:
			if (skipped)
			{
				r = -999;
			}
			if (par->mapa_de_asignacion)
			{
				par->mapa_de_asignacion[opos-par->offset] = -2;
				for (i = opos+1-par->offset;i < par->registros;i++)
				{
					par->mapa_de_asignacion[i] = -2;
				}
			}
			if (par->registros > opos)
			{
				par->registros = opos;
			}
			if (par->regcur > opos)
			{
				par->regcur = par->offset+par->registros;
			}
			if (par->regcur < par->offset)
				par->regcur = par->offset+1;
			return r;
		}
		if ( (par->gmodo & 128) )
		{	
			if (IxxNumeroPosicion != posicion)
			{
				posicion = IxxNumeroPosicion;
			}			
			if (salta_limites(par))
			{
				r = -1;
				/* Retroceder hasta dar con una buena */				
				if (lee_por_posicion(par->pfi,key,posicion,dat_reg,modo,flags) < 0)
				{
					if (ixxstatus == IXXECORRUP)
						return -1;
				}
				if (1 /*salta_limites_atras(par)*/)
				{
					/* no hay ? */
					/*if (IxxNumeroPosicion != posicion)*/
					{
						skipped += (IxxNumeroPosicion-posicion);
						/*skipped += posicion-IxxNumeroPosicion; ojo comentario 1 */
						par->registros = opos-par->offset;
						posicion = IxxNumeroPosicion;
					}
				}
				else
				{	
					/*for (*/
					/*if (IxxNumeroPosicion != posicion)*/
					{
						skipped += (posicion-IxxNumeroPosicion);
						par->registros -= (posicion-IxxNumeroPosicion);
						/*opos-par->offset;*/
						posicion = IxxNumeroPosicion;
					}
				}
				defecto_ficha(par->pfi);
				goto facil_salida;
			}
			if (IxxNumeroPosicion != posicion)
			{
				skipped += (IxxNumeroPosicion-posicion);
				posicion = IxxNumeroPosicion;				
			}
		}		
		if (!hay_dat_permiso(par->pfi,-2))
		{			
			if (other3_server == 56443696 && !super_usuario)
			{

			}
			if (!skipped)
			{
				if (par->mapa_de_asignacion)/*if (!iniciosalto)*/
				{					
					while(opos > 0)
					{
						opos--;
						/*
						if (par->mapa_de_asignacion[opos-par->offset] != salto)
						{
							opos++;
							break;						
						}
						*/
						if (
							 (r = lee_por_posicion(par->pfi,key,opos+salto,dat_reg,modo,flags)) < 0 
						   )
						{
						   opos++;
						   break;
						}
						if (hay_dat_permiso(par->pfi,-2))
						{
							opos++;
							break;
						}
						skipped++;
					}

				}
				else
				{
					while(opos > 0)
					{
						opos--;
						if (
							 (r = lee_por_posicion(par->pfi,key,opos,dat_reg,modo,flags)) < 0 
						   )
						{
						   opos++;
						   break;
						}
						if (hay_dat_permiso(par->pfi,-2))
						{
							opos++;
							break;
						}
						skipped++;
					}					
				}
			}
			skipped++;
			posicion++;			
			continue;
		}
		break;
	} while(-1);

	if (skipped)
	{
		/*
		if (*_from)
			*_from -= skipped;
		if (*_to)
			*_to -= skipped;
		*/

		if (!par->mapa_de_asignacion)
		{	

			par->registros = TotalRegistros(par) - skipped;

			par->mapa_de_asignacion = (long *)malloc(sizeof(long)*(par->registros+1+skipped));
			par->nmapa_de_asignacion = par->registros;
			par->mapa_de_asignacion[par->nmapa_de_asignacion] = 0;
			par->mapa_de_asignacion[par->registros+skipped] = 0;
			memset(par->mapa_de_asignacion,0,sizeof(long)*(par->registros));		
			
			if (par->regcur > 0)
			{
				corriente = (par->regcur+par->offset)-1;
				mejorcorriente = par->nmapa_de_asignacion+par->offset;
				if (corriente == (skipped+opos))
				{
					par->regcur = opos+par->offset+1;
					corriente = -1;
				}
				else if ((skipped+opos) > corriente && (skipped+opos) < mejorcorriente)
				{
					mejorcorriente = (skipped+opos);
					mji = opos+par->offset+1;
				}
			}
			for (i = opos-par->offset;i < par->registros;i++)
			{
				par->mapa_de_asignacion[i] = skipped;
				if (corriente != -1) 
				{
					if (corriente == (skipped+i+par->offset))
					{
						par->regcur = i+par->offset+1;
						corriente = -1L;
					}
					else if ((skipped+i+par->offset) > corriente && (skipped+i+par->offset) < mejorcorriente)
					{
						mejorcorriente = (skipped+i+par->offset);
						mji = i+par->offset+1;
					}
				}
			}
			if (corriente != -1L)
			{
				int j;
				for (j = 0;j < (opos-par->offset);j++)
				{
					if (corriente == (par->mapa_de_asignacion[j]+j+par->offset))
					{
						par->regcur = j+par->offset+1;
						corriente = -1L;
						break;
					}
					else
					{
					    if ((par->mapa_de_asignacion[j]+j+par->offset) > corriente && (par->mapa_de_asignacion[j]+j+par->offset) < mejorcorriente)
						{
							mejorcorriente = (par->mapa_de_asignacion[j]+j+par->offset);
							mji = j+par->offset+1;
						}
					}
				}
				if (corriente != -1L && mji != -1)
				{
					par->regcur = mji;
				}
			}
		}
		else
		{
			int a;
			/*if (par->regcur > opos)*/
			if (par->regcur > 0)
			{
				corriente = (par->regcur-1) + par->mapa_de_asignacion[(par->regcur-par->offset)-1];
			}			
			i = opos-par->offset;
			a = par->mapa_de_asignacion[i+skipped] - par->mapa_de_asignacion[i];
			par->mapa_de_asignacion[i] += (skipped + a);
			
			if (corriente != -1)
			{
				if (corriente == (par->mapa_de_asignacion[i]+i+par->offset))
				{
					par->regcur = i+par->offset+1;
					corriente = -1L;
				}
				else
				{
					if ((par->mapa_de_asignacion[i]+i+par->offset) > corriente && (par->mapa_de_asignacion[i]+i+par->offset) < mejorcorriente)
					{
						mejorcorriente = (par->mapa_de_asignacion[i]+i+par->offset);
						mji = i+par->offset+1;
					}
				}
			}
			

			i = (opos+1)-par->offset;

			par->registros -= skipped;
			while(i < par->registros)
			{
				a = par->mapa_de_asignacion[i+skipped] - par->mapa_de_asignacion[i];
				par->mapa_de_asignacion[i] += (skipped + a);
				
				if (corriente != -1)
				{					
					if (corriente == (par->mapa_de_asignacion[i]+i+par->offset))
					{
						par->regcur = i+par->offset+1;
						corriente = -1L;
					}
					else
					{
						if ((par->mapa_de_asignacion[i]+i+par->offset) > corriente && (par->mapa_de_asignacion[i]+i+par->offset) < mejorcorriente)
						{
							mejorcorriente = (par->mapa_de_asignacion[i]+i+par->offset);
							mji = i+par->offset+1;
						}
					}

				}
				
				i++;
			}				

			if (corriente != -1L)
			{
				int j;
				for (j = 0;j < (opos-par->offset);j++)
				{
					if (corriente == (par->mapa_de_asignacion[j]+j+par->offset))
					{
						par->regcur = j+par->offset+1;
						corriente = -1L;
						break;
					}
					else
					{
					    if ((par->mapa_de_asignacion[j]+j+par->offset) > corriente && (par->mapa_de_asignacion[j]+j+par->offset) < mejorcorriente)
						{
							mejorcorriente = (par->mapa_de_asignacion[j]+j+par->offset);
							mji = j+par->offset+1;
						}
					}
				}
				if (corriente != -1L && mji != -1)
				{
					par->regcur = mji;
				}
			}

			
			for (;i < par->nmapa_de_asignacion;i++)
			{
				par->mapa_de_asignacion[i] = -1;
			}
		}
		return 999;
	}
	
	return r;
}
#endif


#ifdef ANTERIOR_X
static int grid_lee_por_posicion(struct gridpars *par,int key,long posicion,Uchar *dat_reg,int modo,int flags,int *_from,int *_to)
{
	int r;
	long skipped = 0;
	long opos,i,j,flag;	
	long salto = 0L;
	long nn,corriente = -1L;
	long mejorcorriente = -1L;
	long mji = -1;
	/*int from = -1,to = -1;*/
	/*int iniciosalto = 1;*/

	
	if ((posicion-par->offset) >= par->registros)
	{
		return -1;
	}
	if (par->registros < 1)
	{
		return -1;
	}

	opos = posicion;

	if (par->mapa_de_asignacion)
	{
		if ((posicion-par->offset) >= par->nmapa_de_asignacion)
		{
			return -1;
		}
		salto = par->mapa_de_asignacion[opos-par->offset];
		posicion = salto;
		/*
		if (*_from)
		{
			from = par->mapa_de_asignacion[*_from];
			if (from >= 0)
				from += *_from;
		}
		if (*_to)
		{
			to = par->mapa_de_asignacion[*_to];
			if (to >= 0)
				to += *_to;
		}
		*/
		if (salto < 0)
		{
			while(opos > 0 && par->mapa_de_asignacion[opos-par->offset] < 0) opos--;
		}	
		else
		{
			/*
			if (opos > 0 && par->mapa_de_asignacion[(opos-par->offset)-1] == salto)
			{
				iniciosalto = 0;
			}
			*/
			if (!salto)
			{
				posicion = opos;
			}
			else
			{
				posicion += opos;
			}			
		}
	}
	/*
	else
	{
		if (*_from)
		{
			from = *_from;
		}
		if (*_to)
		{
			to = *_to;
		}
	}
	*/
	if (posicion < 0)
	{		
		par->registros = opos-par->offset;
		if (par->regcur >= (par->registros+par->offset))
		{
			par->regcur = (par->registros+par->offset);
			if (par->regcur <= par->offset)
				par->regcur = par->offset+1;
		}
		return -1;
	}	

	do
	{
		if (
			 (r = lee_por_posicion(par->pfi,key,posicion,dat_reg,modo,flags)) < 0 
		   )
		{
			if (ixxstatus == IXXECORRUP)
				return -1;
facil_salida:
			if (skipped)
			{
				r = -999;
			}
			if (par->mapa_de_asignacion)
			{
				par->mapa_de_asignacion[opos-par->offset] = -2;
				for (i = opos+1-par->offset;i < par->registros;i++)
				{
					par->mapa_de_asignacion[i] = -2;
				}
			}
			if (par->registros > opos)
			{
				par->registros = opos;
			}
			if (par->regcur > opos)
			{
				par->regcur = par->offset+par->registros;
			}
			if (par->regcur < par->offset)
				par->regcur = par->offset+1;
			return r;
		}		
		if ( (par->gmodo & 128) )
		{	
			if (IxxNumeroPosicion != posicion)
			{
				posicion = IxxNumeroPosicion;
			}			
			if (salta_limites(par))
			{
				r = -1;
				/* Retroceder hasta dar con una buena */				
				if (lee_por_posicion(par->pfi,key,posicion,dat_reg,modo,flags) < 0)
				{
					if (ixxstatus == IXXECORRUP)
						return -1;
				}
				if (1 /*salta_limites_atras(par)*/)
				{
					/* no hay ? */
					/*if (IxxNumeroPosicion != posicion)*/
					{
						skipped += (IxxNumeroPosicion-posicion);
						/*skipped += posicion-IxxNumeroPosicion; ojo comentario 1 */
						par->registros = opos-par->offset;
						posicion = IxxNumeroPosicion;
					}
				}
				else
				{	
					/*for (*/
					/*if (IxxNumeroPosicion != posicion)*/
					{
						skipped += (posicion-IxxNumeroPosicion);
						par->registros -= (posicion-IxxNumeroPosicion);
						/*opos-par->offset;*/
						posicion = IxxNumeroPosicion;
					}
				}
				defecto_ficha(par->pfi);
				goto facil_salida;
			}
			if (IxxNumeroPosicion != posicion)
			{
				skipped += (IxxNumeroPosicion-posicion);
				posicion = IxxNumeroPosicion;				
			}
		}		
		if (!hay_dat_permiso(par->pfi,-2))
		{			
			if (other3_server == 56443696 && !super_usuario)
			{

			}
			if (!skipped)
			{
				if (par->mapa_de_asignacion)/*if (!iniciosalto)*/
				{					
					while(opos > 0)
					{
						opos--;
						/*
						if (par->mapa_de_asignacion[opos-par->offset] != salto)
						{
							opos++;
							break;						
						}
						*/
						if (
							 (r = lee_por_posicion(par->pfi,key,opos+salto,dat_reg,modo,flags)) < 0 
						   )
						{
						   opos++;
						   break;
						}
						if (hay_dat_permiso(par->pfi,-2))
						{
							opos++;
							break;
						}
						skipped++;
					}

				}
				else
				{
					while(opos > 0)
					{
						opos--;
						if (
							 (r = lee_por_posicion(par->pfi,key,opos,dat_reg,modo,flags)) < 0 
						   )
						{
						   opos++;
						   break;
						}
						if (hay_dat_permiso(par->pfi,-2))
						{
							opos++;
							break;
						}
						skipped++;
					}					
				}
			}
			skipped++;
			posicion++;			
			continue;
		}
		break;
	} while(-1);

	if (skipped)
	{
		/*
		if (*_from)
			*_from -= skipped;
		if (*_to)
			*_to -= skipped;
		*/

		if (!par->mapa_de_asignacion)
		{	

			par->registros -= skipped;

			par->mapa_de_asignacion = (long *)malloc(sizeof(long)*(par->registros+1));
			par->nmapa_de_asignacion = par->registros;
			par->mapa_de_asignacion[par->nmapa_de_asignacion] = 0;
			memset(par->mapa_de_asignacion,0,sizeof(long)*(par->registros));
			
			if (par->regcur > 0)
			{
				corriente = (par->regcur+par->offset)-1;
				mejorcorriente = par->nmapa_de_asignacion+par->offset;
				if (corriente == (skipped+opos))
				{
					par->regcur = opos+par->offset+1;
					corriente = -1;
				}
				else if ((skipped+opos) > corriente && (skipped+opos) < mejorcorriente)
				{
					mejorcorriente = (skipped+opos);
					mji = opos+par->offset+1;
				}
			}
			for (i = opos-par->offset;i < par->registros;i++)
			{
				par->mapa_de_asignacion[i] = skipped;
				if (corriente != -1) 
				{
					if (corriente == (skipped+i+par->offset))
					{
						par->regcur = i+par->offset+1;
						corriente = -1L;
					}
					else if ((skipped+i+par->offset) > corriente && (skipped+i+par->offset) < mejorcorriente)
					{
						mejorcorriente = (skipped+i+par->offset);
						mji = i+par->offset+1;
					}
				}
			}
			if (corriente != -1L)
			{
				int j;
				for (j = 0;j < (opos-par->offset);j++)
				{
					if (corriente == (par->mapa_de_asignacion[j]+j+par->offset))
					{
						par->regcur = j+par->offset+1;
						corriente = -1L;
						break;
					}
					else
					{
					    if ((par->mapa_de_asignacion[j]+j+par->offset) > corriente && (par->mapa_de_asignacion[j]+j+par->offset) < mejorcorriente)
						{
							mejorcorriente = (par->mapa_de_asignacion[j]+j+par->offset);
							mji = j+par->offset+1;
						}
					}
				}
				if (corriente != -1L && mji != -1)
				{
					par->regcur = mji;
				}
			}
		}
		else
		{
			int a;
			/*if (par->regcur > opos)*/
			if (par->regcur > 0)
			{
				corriente = (par->regcur-1) + par->mapa_de_asignacion[(par->regcur-par->offset)-1];
			}			
			i = opos-par->offset;
			a = par->mapa_de_asignacion[i+skipped] - par->mapa_de_asignacion[i];
			par->mapa_de_asignacion[i] += (skipped + a);
			
			if (corriente != -1)
			{
				if (corriente == (par->mapa_de_asignacion[i]+i+par->offset))
				{
					par->regcur = i+par->offset+1;
					corriente = -1L;
				}
				else
				{
					if ((par->mapa_de_asignacion[i]+i+par->offset) > corriente && (par->mapa_de_asignacion[i]+i+par->offset) < mejorcorriente)
					{
						mejorcorriente = (par->mapa_de_asignacion[i]+i+par->offset);
						mji = i+par->offset+1;
					}
				}
			}
			

			i = (opos+1)-par->offset;

			par->registros -= skipped;
			while(i < par->registros)
			{
				a = par->mapa_de_asignacion[i+skipped] - par->mapa_de_asignacion[i];
				par->mapa_de_asignacion[i] += (skipped + a);
				
				if (corriente != -1)
				{					
					if (corriente == (par->mapa_de_asignacion[i]+i+par->offset))
					{
						par->regcur = i+par->offset+1;
						corriente = -1L;
					}
					else
					{
						if ((par->mapa_de_asignacion[i]+i+par->offset) > corriente && (par->mapa_de_asignacion[i]+i+par->offset) < mejorcorriente)
						{
							mejorcorriente = (par->mapa_de_asignacion[i]+i+par->offset);
							mji = i+par->offset+1;
						}
					}

				}
				
				i++;
			}				

			if (corriente != -1L)
			{
				int j;
				for (j = 0;j < (opos-par->offset);j++)
				{
					if (corriente == (par->mapa_de_asignacion[j]+j+par->offset))
					{
						par->regcur = j+par->offset+1;
						corriente = -1L;
						break;
					}
					else
					{
					    if ((par->mapa_de_asignacion[j]+j+par->offset) > corriente && (par->mapa_de_asignacion[j]+j+par->offset) < mejorcorriente)
						{
							mejorcorriente = (par->mapa_de_asignacion[j]+j+par->offset);
							mji = j+par->offset+1;
						}
					}
				}
				if (corriente != -1L && mji != -1)
				{
					par->regcur = mji;
				}
			}

			
			for (;i < par->nmapa_de_asignacion;i++)
			{
				par->mapa_de_asignacion[i] = -1;
			}
		}
		return 999;
	}
	
	return r;
}
#endif

void PintaOtrosCampos(struct gridpars *par)
{
	int _campo;
	if ((par->gmodo & 512))
	{
		if (par->regsel)
		{		
			pon_registro_actual(par->pfi,par->regsel );
			apunta_ind(par->pfi,0,ISEQUAL,0);
			apunta_ind(par->pfi,par->encl+1,ISCURR,0);
			lee_reg(par->pfi,ISCURR);
		}
		else
		{
			defecto_ficha(par->pfi);
		}
		for (_campo = 0;_campo < par->pfi->f_ncampos;_campo++)
		{
			if (!(par->pfi->f_campos[_campo].c_mante.c_visual) && (par->pfi->f_campos[_campo].c_mante.c_lineal & FGV_C_GRIDSTATICO))
			{
				pinta_campo(par->ipfi,_campo+1,0,0);
				/*pulsatecla();*/
			}
		}		
	}
}

static struct grid_filtro *rellena_gridfiltro(int *pnfiltros,int *pncols,int **pcols,char *nfile,struct Info_Entra_Campos *InfoCampos,int *essql,FI_DEF *pfipadre)
{
    struct grid_filtro *pfiltros = NULL;
	int nfiltros = 0;
	int flag0,i,j,ji,ni;
    int ncols = 0;
    int *cols = NULL;
    int Inver = 0,Mayus = 0;
    int Orden = 0,OrdenInverso = 0;
	char *dd = NULL;
	char tmp[10];
	char LimiteI[300];
	char LimiteS[300];
	char Valor[300];
	unsigned char TipoValor;
	int file;
	char *s,*s1 = NULL;


	file = rw_open_(nfile,O_RDONLY | O_BINARY);
	if (file < 0)
		return NULL;
	
	i = rw_lseek_(file,0L,SEEK_END);
	s = malloc(i+1);
	
	rw_lseek_(file,0L,SEEK_SET);
	rw_read_(file,s,i);
	rw_close_(file);

	s1 = s;

	s[i] = 0;

	if (*s == 26)
	{
		struct _sql_toscreen *sqltoscreen = NULL;
		FI_DEF *pfi;
		FI_DEF *padrepfis[100];
		int padrecargado[100];
		int nfiltros[100];
		int pariente[100];
		int nrelaciones[100];
		int relacionpadre[100][32];
		int relacionhijo[100][32];
		int n,k,ii,nn;
		int nf;
		int flag;		
		int campo;
		int orden[MAXCAMPOSPORCLAVE];
		int sentido[MAXCAMPOSPORCLAVE];
		int norden = 0;		
		char files[100][128];
		struct grid_filtro *filtros[100];
	    struct _sql_origen *origen;
	    struct _sql_relacion *relacion;

		*essql = 1;

		s++;

		memcpy(tmp,s,3);
		tmp[3] = 0;
		s += 3;
		nf = atoi(tmp);
		if (!nf)
			goto salida1;		        

		pfi = (FI_DEF *)malloc(sizeof(FI_DEF));
		memset((char *)pfi,0,sizeof(FI_DEF));
		pfi->f_titulo = strmalloc(1);
		pfi->f_titulo[0] = 0;		
		strcpy(pfi->f_PathDef,pfipadre->f_PathDef); //quita_path(pfipadre->f_PathDef)

		for (ii = 0;ii < nf;ii++)
		{
			i = 0;
			if (!s[i] || s[i] == 25 || s[i] == 24) break;
			while(s[i] && s[i] != 2) i++;
			if (s[i] != 2)
				break;
			memcpy(files[ii],s,i);
			files[ii][i] = 0;

			padrecargado[ii] = 0;
			nfiltros[ii] = 0;
			if (!ii)
			{
				padrepfis[ii] = pfipadre;
			}
			else
			{
					j = ind_rel(quita_path(files[ii]));
					padrepfis[ii] = NULL;
					
					if (files[ii][0] != ':' && files[ii][0] != '/')
					{
						strcpy(tmp,pfipadre->f_PathDef);
						strcpy(quita_path(tmp),quita_path(files[ii]));						
					}
					else
					{
						cambia_path_def(files[ii],0,tmp);
					}

					if (lee_defn(tmp,&padrepfis[ii]) >= 0)
					{
						if (j >= 0)
						{
							strcpy(padrepfis[ii]->f_fichero,v_mpfi(j)->f_fichero);
							j = -1;
						}
						else
						{
							if (padrepfis[ii]->f_fichero[0] != ':' && padrepfis[ii]->f_fichero[0] != '/')
							{
								strcpy(tmp,pfipadre->f_fichero);
								strcpy(quita_path(tmp),quita_path(padrepfis[ii]->f_fichero));
								strcpy(padrepfis[ii]->f_fichero,tmp);
								/*cambia_path_fich(padrepfis[ii]->f_fichero);*/
							}
						}
						padrecargado[ii] = 1;
					}
			}

			s += (i+1);
			i = 0;
			while(s[i] && s[i] != 2) i++;
			if (s[i] != 2)
				break;
			s += (i+1);			

			memcpy(tmp,s,3);
			tmp[3] = 0;
			s += 3;
			pariente[ii] = atoi(tmp);
			memcpy(tmp,s,3);
			tmp[3] = 0;
			s += 3;
			nrelaciones[ii] = atoi(tmp);
			for (i = 0;i < nrelaciones[ii];i++)
			{
				memcpy(tmp,s,3);
				tmp[3] = 0;
				s += 3;
				relacionpadre[ii][i] = atoi(tmp);

				memcpy(tmp,s,3);
				tmp[3] = 0;
				s += 3;
				relacionhijo[ii][i] = atoi(tmp);
			}
		}

		if (s[i] == 24)
		{
			/* pos-orden de campos ... en este tipo de filtros no deberia ser necesario */
			while (s[i] && s[i] != 25) i++;				
		}

		s += i;

		i = 0;
		if (ii != nf || *s != 25)
			goto salida1;

		s++;

		origen = (struct _sql_origen *)malloc(sizeof(struct _sql_origen)*300);

		n = 0;
		nn = 0;
		while(*s)
		{       
		   for (i = 0;s[i] && s[i] != 1;i++);
		   if (i)
		   {
			   if (dd)
				   free(dd);

			   memcpy(tmp,s,3);
			   tmp[3] = 0;
			   s += 3;
			   i -= 3;
			   ii = atoi(tmp);

			   if (ii < 0 || ii >= nf)
				   break;

			   dd = malloc(i+1);		   

			   Inver = 0;
			   Mayus = 0;
			   Orden = 0;
			   OrdenInverso = 0;

			   ji = 0;

			   for (j = 0;j < i;j++)
			   {
				   if (s[j] == (char)3)
				   {
					  Inver = 1;
				   }
				   else if (s[j] == (char)4)
				   {
					  Mayus = 1;
				   }               
				   else if (s[j] == (char)5)
				   {				  
					  Orden = 1;
				   }			   
				   else if (s[j] == (char)6)
				   {				  
					  OrdenInverso = 1;
				   }
				   else
				   {
					   dd[ji] = s[j];
					   ji++;
				   }
			   }
			   dd[ji] = 0;

			   memcpy(tmp,dd,3);
			   tmp[3] = 0;
			   ni = atoi(tmp);			   
			   campo = ni;

			   if (ni < 0 || ni >= padrepfis[ii]->f_ncampos)
			   {
				   break;
			   }			   

			   inserta_campo(pfi,n,padrepfis[ii]->f_campos[campo].c_lista,					
									padrepfis[ii]->f_campos[campo].c_nombre,
									padrepfis[ii]->f_campos[campo].c_qc,
									padrepfis[ii]->f_campos[campo].c_minimo,
									padrepfis[ii]->f_campos[campo].c_maximo,
									padrepfis[ii]->f_campos[campo].c_mante.c_defecto);
	  		   origen[n].ipadre = ii;
			   origen[n].icampo = campo;

			   if (dd[3] == 'S')
			   {
				   pfi->f_campos[n].c_mante.c_visual = 0;
			   }

			   TipoValor = (unsigned char)dd[4]-48;

			   for(j=5;dd[j] && dd[j] != 2;j++);
			   memcpy(LimiteI,dd+5,j-5);
			   LimiteI[j-5] = 0;
			   quitab(LimiteI);
			   j++;
			   for(ji = j;dd[j] && dd[j] != 2;j++);
			   memcpy(LimiteS,dd+ji,j-ji);           
			   LimiteS[j-ji];
			   quitab(LimiteS);
			   j++;

			   ji = strlen(dd+j);
			   memcpy(Valor,dd+j,ji);
			   Valor[ji] = 0;
			   quitab(Valor);

			   if (Orden)
			   {
				   orden[norden] = n+1;
				   if (OrdenInverso)
				   {
					   sentido[norden] = -1;
				   }
				   else
				   {
					   sentido[norden] = 1;
				   }
				   norden++;
			   }

			   if (Valor[0] || LimiteI[0] || LimiteS[0] /* || Orden*/)
			   {
				 k = nfiltros[ii];

				 if (!k)
				 {
					filtros[ii] = (struct grid_filtro *)malloc(sizeof(struct grid_filtro));
				 }
				 else
				 {
					filtros[ii] = (struct grid_filtro *)realloc((void *)filtros[ii],sizeof(struct grid_filtro)*(k+1));
				 }

				 memset(&(filtros[ii][k]),0,sizeof(filtros[ii][k]));

				 filtros[ii][k].tipo  = (TipoValor & 0xf);
				 flag0 = 0;
				 if (Orden)
				 {
					flag0 |= 1;
					if (OrdenInverso)
					{
						flag0 |= 2;
					}
				 }
				 filtros[ii][k].tipo |= (flag0 << 4);		   
				 filtros[ii][k].campo = campo;
				 filtros[ii][k].mayus = Mayus;
				 filtros[ii][k].inver = Inver;
				 strncpy(filtros[ii][k].valor,Valor,298);
				 strncpy(filtros[ii][k].limi,LimiteI,298);
				 strncpy(filtros[ii][k].lims,LimiteS,298);
				 nfiltros[ii]++;
				 nn++;
			   }

			   n++;

		   }
		   else
			   break;
		   s += i;
		   if (*s)
			   s++;
		   else
			   break;
		}		

		nn++;
		k = nfiltros[0];
		if (!k)
		{
			filtros[0] = (struct grid_filtro *)malloc(sizeof(struct grid_filtro)*nn);
		}
		else
		{
			filtros[0] = (struct grid_filtro *)realloc((void *)filtros[0],sizeof(struct grid_filtro)*nn);
		}			
		for (nn = 1;nn < nf;nn++)
		{
			if (nfiltros[nn])
			{
				memcpy((char *)(filtros[0]+k),(char *)filtros[nn],sizeof(struct grid_filtro)*nfiltros[nn]);
				free((void *)filtros[nn]);
				filtros[nn] = filtros[0]+k;
				k += nfiltros[nn];
			}
		}
			
		/* la clave primaria = contador de resultados */
		inserta_campo(pfi,n,"_QUERY_REG",
							"Registro",
							0,
							"0",
							"999999999999",
							"0");
		origen[n].ipadre = -1;
		origen[n].icampo = -1;			
		pfi->f_nclaves = 1;
		pfi->f_clave = (KEY_DEF *)reserva_malloc(sizeof(KEY_DEF)*2,3); /* memoria para dos claves */
		pfi->f_clave[0].k_ncampos = 1;
		pfi->f_clave[0].k_fflag = 0;
		pfi->f_clave[0].k_campo = (short *)malloc(sizeof(short) * 2);
		pfi->f_clave[0].k_orden = (short *)malloc(sizeof(short) * 2);
		pfi->f_clave[0].k_orden[0] = 1;
		pfi->f_clave[0].k_campo[0] = n+1;
		if (norden)
		{
			pfi->f_nclaves = 2;
			pfi->f_clave[1].k_ncampos = 1;
			pfi->f_clave[1].k_fflag = 0;
			pfi->f_clave[1].k_campo = (short *)malloc(sizeof(short) * (norden+1));
			pfi->f_clave[1].k_orden = (short *)malloc(sizeof(short) * (norden+1));
			for (i = 0;i < norden;i++)
			{
				pfi->f_clave[1].k_campo[i] = orden[i];
				pfi->f_clave[1].k_orden[i] = sentido[i];
			}
		}
		for (i=pfi->f_nclaves;i>0;i--) 
		{
			for (j=pfi->f_clave[i-1].k_ncampos;j>0;j--) 
			{
				if((k = pfi->f_clave[i-1].k_campo[j-1]) <= pfi->f_ncampos && k)
					pfi->f_campos[k-1].c_key = i;   
			}
		}

		relacion = (struct _sql_relacion *)malloc(sizeof(struct _sql_relacion)*nf);
		memset((char *)relacion,0,sizeof(struct _sql_relacion)*nf);
		for (i = 0;i < nf;i++)
		{
			for (k = 0;k < padrepfis[i]->f_nrelaci;k++)
			{
				for (j = 1;j < nf;j++)
				{
					if (j != i)
					{
						if (!strcmp(quita_path(padrepfis[i]->f_relaci[k].r_fichero),quita_path(padrepfis[j]->f_PathDef)))
						{
							break;
						}
					}
				}
				if (j < nf)
				{
					ii = relacion[i].nrel;
					relacion[i].nrel++;
					relacion[i].frel[ii] = j;								
					relacion[i].hrel[ii] = 1;
					for (j = 0;j < MAXCAMPOSPORCLAVE;j++)
					{
					   relacion[i].rel[ii][j] = padrepfis[i]->f_relaci[k].r_campo[j];
					   if (!relacion[i].rel[ii][j])
						   break;
					   relacion[i].rel[ii][j]--;
					   relacion[i].ncrel[ii]++;
					}							 
				}
			}
		}
		
		sqltoscreen = (struct _sql_toscreen *)malloc(sizeof(struct _sql_toscreen));
		memset((void *)sqltoscreen,0,sizeof(struct _sql_toscreen));
		sqltoscreen->pfi = pfi;
		memcpy(sqltoscreen->padrepfis,padrepfis,sizeof(padrepfis));
		memcpy(sqltoscreen->padrecargado,padrecargado,sizeof(padrecargado));
		memcpy(sqltoscreen->nfiltros,nfiltros,sizeof(nfiltros));
		sqltoscreen->nf = nf;
		memcpy(sqltoscreen->filtros,filtros,sizeof(filtros));
		sqltoscreen->origen = origen;
		sqltoscreen->relacion = relacion;

salida1:
		if (dd)
			free(dd);
		if (s1)
			free(s1);
		return (struct grid_filtro *)sqltoscreen;
	}


	pfiltros = (struct grid_filtro *)malloc(sizeof(struct grid_filtro)*InfoCampos->n_campos);
	cols = (int *)malloc(InfoCampos->n_campos*sizeof(int));

    while(*s)
    {       
       for (i = 0;s[i] && s[i] != 1;i++);
       if (i)
       {
		   if (dd)
			   free(dd);
		   dd = malloc(i+1);		   

		   Inver = 0;
		   Mayus = 0;
		   Orden = 0;
		   OrdenInverso = 0;

		   ji = 0;
		   for (j = 0;j < i;j++)
		   {
			   if (s[j] == (char)3)
			   {
				  Inver = 1;
			   }
			   else if (s[j] == (char)4)
			   {
				  Mayus = 1;
			   }               
			   else if (s[j] == (char)5)
			   {				  
				  Orden = 1;
			   }			   
			   else if (s[j] == (char)6)
			   {				  
				  OrdenInverso = 1;
			   }
			   else
			   {
				   dd[ji] = s[j];
				   ji++;
			   }
		   }
		   dd[ji] = 0;

		   memcpy(tmp,dd,3);
		   tmp[3] = 0;
           ni = atoi(tmp);

           if (ni < 0 || ni >= InfoCampos->n_campos)
           {
			   if (dd)
				   free(dd);
			   if (pfiltros)
				   free(pfiltros);
			   if (cols)
				   free(cols);
			   if (s1)
				   free(s1);
               return NULL;
           }
           if (dd[3] == 'S')
           {
			   cols[ncols] = ni;
			   ncols++;
           }

           TipoValor = (unsigned char)dd[4]-48;

           for(j=5;dd[j] && dd[j] != 2;j++);
           memcpy(LimiteI,dd+5,j-5);
		   LimiteI[j-5] = 0;
           quitab(LimiteI);
           j++;
           for(ji = j;dd[j] && dd[j] != 2;j++);
           memcpy(LimiteS,dd+ji,j-ji);           
		   LimiteS[j-ji];
           quitab(LimiteS);
           j++;

		   ji = strlen(dd+j);
           memcpy(Valor,dd+j,ji);
           Valor[ji] = 0;
           quitab(Valor);

		   if (Valor[0] || LimiteI[0] || LimiteS[0] || Orden)
		   {
		     pfiltros[nfiltros].tipo  = (TipoValor & 0xf);
			 flag0 = 0;
			 if (Orden)
			 {
				flag0 |= 1;
				if (OrdenInverso)
				{
					flag0 |= 2;
				}
			 }
			 pfiltros[nfiltros].tipo |= (flag0 << 4);		   
		     pfiltros[nfiltros].campo = dInfoCampo(InfoCampos,ni,0)->Id;
		     pfiltros[nfiltros].mayus = Mayus;
		     pfiltros[nfiltros].inver = Inver;
		     strncpy(pfiltros[nfiltros].valor,Valor,298);
		     strncpy(pfiltros[nfiltros].limi,LimiteI,298);
		     strncpy(pfiltros[nfiltros].lims,LimiteS,298);
		     nfiltros++;
		   }

       }
       else
           break;
       s += i;
	   if (*s)
		   s++;
	   else
		   break;
    }

	if (dd)
		free(dd);
	if (s1)
		free(s1);

	*pnfiltros = nfiltros;
	if (!nfiltros && pfiltros)
	{
		free(pfiltros);
		pfiltros = NULL;
	}
	*pncols = ncols;
	*pcols = cols;
	return pfiltros;
}

static int avanza_mapa_ordenacion_2(char *clave,long *array, int ini, int fin,int campo,FI_DEF *pfi,int nocase)
{
    char buf1[MAXDEFECTO+1];	
	int tamc;

	do
	{
		if (ini < fin)
		{
		   lee_directo(dame_fd_datos(pfi),array[ini],pfi,pfi->f_buf);   
		   buf1[MAXDEFECTO] = 0;
		   formato_campo(pfi,campo,buf1);
		   if (nocase)
			   strmayus(buf1);
		   tamc = strlen(buf1);
		   if (memcmp(buf1,clave,tamc) < 0)
			   ini++;
		   else
			   break;
		}
		else
			break;
	} while(-1);

	return ini;
}
 

static int busca_mapa_ordenacion_2(char *clave,long *array, int ini, int fin,int campo,FI_DEF *pfi,int nocase)
{   
   int comp = fin-ini;
   int medio = comp / 2;
   char buf1[MAXDEFECTO+1];   
   int tamc;
   
   medio += ini;

   if (!comp)
   {
	   return medio;
   }
   
   lee_directo(dame_fd_datos(pfi),array[medio],pfi,pfi->f_buf);   
   buf1[MAXDEFECTO] = 0;
   formato_campo(pfi,campo,buf1);
   if (nocase)
	   strmayus(buf1);
   tamc = strlen(buf1);

   if (comp == 1)
   {	   
	   if (memcmp(buf1,clave,tamc) <= 0)
	   {
		   return medio;
	   }
	   return medio+1;
   }
   comp = memcmp(buf1,clave,tamc);
   if (comp < 0)
   {
	   return busca_mapa_ordenacion_2(clave,array,medio+1,fin,campo,pfi,nocase);
   }
   if (comp > 0)
   {
	   return busca_mapa_ordenacion_2(clave,array,ini,medio,campo,pfi,nocase);
   }  
   return medio;
}

static int retrocede_mapa_ordenacion_2(char *laclave,long *array, int n,int campo,FI_DEF *pfi,int avance,int nocase)
{
   char buf1[MAXDEFECTO+1];   
   char clave[MAXDEFECTO+1]; 
   int tamc;
   
   if (!laclave)
   {
		lee_directo(dame_fd_datos(pfi),array[n],pfi,pfi->f_buf);   
		clave[MAXDEFECTO] = 0;
		formato_campo(pfi,campo,clave);
		if (nocase)
			strmayus(clave);
		laclave = clave;
   }
   tamc = strlen(laclave);
   if (avance > 0)
   {
	   while (n < avance)
	   {
		   n++;
		   lee_directo(dame_fd_datos(pfi),array[n],pfi,pfi->f_buf);   
		   buf1[MAXDEFECTO] = 0;
		   formato_campo(pfi,campo,buf1);
		   if (nocase)
			   strmayus(buf1);
		   if (memcmp(buf1,laclave,tamc))
		   {
			   n--;
			   break;
		   }
	   }
   }
   else
   {
	   while(n > 0)
	   {
		   n--;
		   lee_directo(dame_fd_datos(pfi),array[n],pfi,pfi->f_buf);   
		   buf1[MAXDEFECTO] = 0;
		   formato_campo(pfi,campo,buf1);
		   if (nocase)
			   strmayus(buf1);
		   if (memcmp(buf1,laclave,tamc))
		   {
			   n++;
			   break;
		   }
	   }
   }
   return n;
}

static int (*f_ord_compara)() = memcmp;

static int compara_double(double *uno,double *dos,int len)
{
	if (*dos > *uno)
		return -1;
	else if (*dos < *uno)
		return 1;
	return 0;
}

static int busca_mapa_ordenacion__(char *clave,char **array, int ini, int fin,int len)
{   
   int comp = fin-ini;
   int medio = comp / 2;  
   
   medio += ini;

   if (!comp)
   {
	   return medio;
   }


   if (comp == 1)
   {
	   if ((*f_ord_compara)(array[medio],clave,len) <= 0)
	   {
		   return medio+1;
	   }
	   return medio;

   }
   comp = (*f_ord_compara)(array[medio],clave,len);
   if (comp < 0)
   {
	   return busca_mapa_ordenacion__(clave,array,medio+1,fin,len);
   }
   if (comp > 0)
   {
	   return busca_mapa_ordenacion__(clave,array,ini,medio,len);
   }  
   return medio;
}


static int busca_mapa_ordenacion(char *clave,char **array, int ini, int fin,int len)
{   
   int n = busca_mapa_ordenacion__(clave,array,ini,fin,len);
   while(n < fin && (*f_ord_compara)(array[n],clave,len) < 0)
   {
	   n++;
   }
   return n;
}

static long rellena_mapa_ordenacion_(FI_DEF *pfi,int ncampos,int *campos,long **_mapa_de_ordenacion,long registros,struct gridpars *par)
{
	int fd;
	long nreg = 1;	
	long leidos = 0;
	int tamc = 0;
	int posc = 0;
	char *buffer;	/* modo compatible, por optimizar para dats ... */
	char *p1;
	char *p2;
	int n,j;
	char **array;
	long nmapa_de_ordenacion;
	long *mapa_de_ordenacion;
	int campo;
	char *bufclave = NULL;
	int e;

	if (ncampos < 1)
		return 0;

	nmapa_de_ordenacion = registros;

	mapa_de_ordenacion = (long *)malloc(sizeof(long) * nmapa_de_ordenacion);
	*_mapa_de_ordenacion = mapa_de_ordenacion;	
	array = malloc(sizeof(char *)*nmapa_de_ordenacion);

	if (ncampos > 1)
	{
		/* falta */
		aviso(0,"En construccion");
	}
	else
	{
		campo = *campos;
		tamc = pfi->f_campos[campo].c_long;
		posc = pfi->f_campos[campo].c_offset;
		p2 = pfi->f_buf+posc;
	}
	buffer = malloc(tamc*nmapa_de_ordenacion);

	p1 = buffer;
	fd = dame_fd_datos(pfi);	
	if (fd >= 0)
	{
		no_report_error++;
		while(leidos < registros)
		{	
			if ((e = lee_directo(fd,nreg,pfi,pfi->f_buf)) == 0)
			{				
				if (esta_en_limites(par) && hay_dat_permiso(pfi,-2))
				{
					if (ncampos > 1)
					{
						/* rellenar bufclave */
					}
					if (!leidos)
					{
						n = 0;
					}
					else
					{
						n = busca_mapa_ordenacion(p2,array,0,leidos,tamc);
					}


					/*
					if (n < leidos)
					{
						memcpy(&(array[n+1]),&(array[n]),sizeof(char *)*(leidos-n));
						memcpy(&(mapa_de_ordenacion[n+1]),&(mapa_de_ordenacion[n]),sizeof(long)*(leidos-n));
					}
					*/
					
					
				    for (j = leidos;j > n;j--)
					{
					   array[j] = array[j-1];
					   mapa_de_ordenacion[j]  = mapa_de_ordenacion[j-1];
					}
					

					array[n] = p1;
					mapa_de_ordenacion[n] = nreg;
					memcpy(p1,p2,tamc);
					p1 += tamc;
					
					leidos++;
				}
			}
			else if (e < 0)
				break;
			nreg++;
		}		
		no_report_error--;
	}
	
	nmapa_de_ordenacion = leidos;	

	free(array);
	free(buffer);
	if (bufclave)
		free(bufclave);
	
	return nmapa_de_ordenacion;
}


static int rellena_mapa_ordenacion_0(struct gridpars *par,int campo)
{
	int fd;
	long nreg;
	long mireg;
	long leidos = 0;
	int tamc = 0;
	int posc = 0;
	char *buffer;	/* modo compatible, por optimizar para dats ... */
	char *p1;
	char *p2;
	int n,j;
	char **array;
	long nmapa_de_ordenacion;
	long *mapa_de_ordenacion;	
	char *bufclave = NULL;
	int conlimites = 0;
	FI_DEF *pfi;

	if (par->limi1 || par->limi2 || par->b_lim1 || par->b_lim2)
	{
		if (par->registros != tam_btrv(par->pfi))
			conlimites = 1;
	}

	if (!conlimites)
	{
		par->registros = rellena_mapa_ordenacion_(par->pfi,1,&campo,&par->mapa_de_ordenacion,par->registros,par);
		par->nmapa_de_ordenacion = par->registros;
	}
	else
	{
		    pfi = par->pfi;

			memcpy(pfi->f_buf,par->b_lim2,pfi->f_lfich);
			if (lee_reg(pfi,ISGREAT) < 0)
			{
				if (lee_reg(pfi,ISLAST) < 0)
					return 0;
			}
			else
			{
				if (lee_reg(pfi,ISPREV) < 0)
					return 0;
			}
			nreg = isrecnum;


			memcpy(pfi->f_buf,par->b_lim1,pfi->f_lfich);
			if (lee_reg(pfi,ISGTEQ) < 0)
			{
				if (lee_reg(pfi,ISFIRST) < 0)
					return 0;					
			}
			mireg = isrecnum;


			nmapa_de_ordenacion = par->registros;

			mapa_de_ordenacion = (long *)malloc(sizeof(long) * nmapa_de_ordenacion);
			par->mapa_de_ordenacion = mapa_de_ordenacion;
			
			array = malloc(sizeof(char *)*nmapa_de_ordenacion);

			tamc = pfi->f_campos[campo].c_long;
			posc = pfi->f_campos[campo].c_offset;
			p2 = pfi->f_buf+posc;

			buffer = malloc(tamc*nmapa_de_ordenacion);

			p1 = buffer;

			isrecnum = mireg;
			do
			{			
				if ( (par->gmodo & 128) && salta_limites(par))
				   continue;	
				mireg = isrecnum;
				if (hay_dat_permiso(pfi,-2))
				{
					if (!leidos)
					{
						n = 0;
					}
					else
					{
						n = busca_mapa_ordenacion(p2,array,0,leidos,tamc);
					}

					/*
					if (n < leidos)
					{
						memcpy(&(array[n+1]),&(array[n]),sizeof(char *)*(leidos-n));
						memcpy(&(mapa_de_ordenacion[n+1]),&(mapa_de_ordenacion[n]),sizeof(long)*(leidos-n));
					}
					*/
					
					for (j = leidos;j > n;j--)
					{
						array[j] = array[j-1];
						mapa_de_ordenacion[j]  = mapa_de_ordenacion[j-1];
					}
					
					array[n] = p1;
					mapa_de_ordenacion[n] = mireg;
					memcpy(p1,p2,tamc);
					p1 += tamc;
					
					leidos++;
				}
				if (nreg == mireg)
					break;
			} while(lee_reg(pfi,ISNEXT) >= 0);
			nmapa_de_ordenacion = leidos;	

			free(array);
			free(buffer);
			if (bufclave)
				free(bufclave);
			
			par->nmapa_de_ordenacion = par->registros = nmapa_de_ordenacion;
	}



    for (j = 0;j < par->nmapa_de_ordenacion;j++)
	{
		if (par->regsel == par->mapa_de_ordenacion[j])
		{
			par->reg = (j/*-par->offset*/)+1;
			par->regcur = par->reg;
			break;
		}
	}
	return 0;
}


#define FACTOR_MERGE 4096

static int rellena_mapa_ordenacion_1(struct gridpars *par,int campo)
{
	int fd;
	long nreg;
	long mireg;
	long leidos = 0;
	int tamc = 0;
	int posc = 0;
	char *buffer;	/* modo compatible, por optimizar para dats ... */
	char *p1;
	char *p2;
	int n,j,i;
	char ***array;
	long *nmapa_de_ordenacion;
	long **mapa_de_ordenacion;
	char *bufclave = NULL;
	int conlimites = 0;
	FI_DEF *pfi;
	long registros;
	long maxporparte;
	int npartes;
	int lista;
	long total = 0;	
	char *vminimo;	
	long *flaglista;
	int _NMERGESORT_;
	int e;

	
	registros = par->registros;

	_NMERGESORT_ = registros / FACTOR_MERGE;

	flaglista = (long *)malloc(sizeof(long)*(_NMERGESORT_+1));
	array = (char **)malloc(sizeof(void *)*(_NMERGESORT_+1));
	nmapa_de_ordenacion = (long *)malloc(sizeof(long)*(_NMERGESORT_+1));
	mapa_de_ordenacion = (long **)malloc(sizeof(void *)*(_NMERGESORT_+1));

	if (par->limi1 || par->limi2 || par->b_lim1 || par->b_lim2)
	{
		if (registros != tam_btrv(par->pfi))
			conlimites = 1;
	}
	
	maxporparte = (registros/_NMERGESORT_) + 1;


	pfi = par->pfi;
	tamc = pfi->f_campos[campo].c_long;
	posc = pfi->f_campos[campo].c_offset;
	p2 = pfi->f_buf+posc;	

	buffer = malloc(tamc*registros);
	p1 = buffer;

	npartes = 0;
	nmapa_de_ordenacion[npartes] = 0;
	mapa_de_ordenacion[npartes] = (long *)malloc(sizeof(long) * maxporparte);	
	array[npartes] = malloc(sizeof(char *)*maxporparte);	

	

	if (conlimites)
	{
		memcpy(pfi->f_buf,par->b_lim2,pfi->f_lfich);
		if (lee_reg(pfi,ISGREAT) < 0)
		{
			if (lee_reg(pfi,ISLAST) < 0)
				goto no_hay_nada;
		}
		else
		{
			if (lee_reg(pfi,ISPREV) < 0)
				goto no_hay_nada;
		}
		nreg = isrecnum;


		memcpy(pfi->f_buf,par->b_lim1,pfi->f_lfich);
		if (lee_reg(pfi,ISGTEQ) < 0)
		{
			if (lee_reg(pfi,ISFIRST) < 0)
				goto no_hay_nada;
		}
		//mireg = isrecnum;
		//isrecnum = mireg;
		do
		{			
			if ( (par->gmodo & 128) && salta_limites(par))
			   continue;	
			mireg = isrecnum;
			if (hay_dat_permiso(pfi,-2))
			{
				if (!leidos)
				{
					n = 0;
				}
				else
				{
					n = busca_mapa_ordenacion(p2,array[npartes],0,leidos,tamc);
				}

				/*
				if (n < leidos)
				{
					memcpy(&(array[npartes][n+1]),&(array[npartes][n]),sizeof(char *)*(leidos-n));
					memcpy(&(mapa_de_ordenacion[npartes][n+1]),&(mapa_de_ordenacion[npartes][n]),sizeof(long)*(leidos-n));
				}
				*/
				
				for (j = leidos;j > n;j--)
				{
					array[npartes][j] = array[npartes][j-1];
					mapa_de_ordenacion[npartes][j]  = mapa_de_ordenacion[npartes][j-1];
				}
				
				array[npartes][n] = p1;
				mapa_de_ordenacion[npartes][n] = mireg;
				memcpy(p1,p2,tamc);
				p1 += tamc;
				
				leidos++;
				total++;
			}
			if (nreg == mireg)
				break;
			if (leidos >= maxporparte)
			{
				nmapa_de_ordenacion[npartes] = leidos;
				npartes++;
				leidos=0;
				nmapa_de_ordenacion[npartes] = 0;
				mapa_de_ordenacion[npartes] = (long *)malloc(sizeof(long) * maxporparte);	
				array[npartes] = malloc(sizeof(char *)*maxporparte);	
			}
		} while(lee_reg(pfi,ISNEXT) >= 0 && npartes <= _NMERGESORT_);
		nmapa_de_ordenacion[npartes] = leidos;
	}
	else
	{
		nreg = 1;
		fd = dame_fd_datos(pfi);		
		if (fd >= 0)
		{
			no_report_error++;
			while(total < registros)
			{			
				if ((e = lee_directo(fd,nreg,pfi,pfi->f_buf)) == 0)
				{					
					if (esta_en_limites(par) && hay_dat_permiso(pfi,-2))
					{
						if (!leidos)
						{
							n = 0;
						}
						else
						{
							n = busca_mapa_ordenacion(p2,array[npartes],0,leidos,tamc);
						}					

	
						/*
						if (n < leidos)
						{
							memcpy(&(array[npartes][n+1]),&(array[npartes][n]),sizeof(char *)*(leidos-n));
							memcpy(&(mapa_de_ordenacion[npartes][n+1]),&(mapa_de_ordenacion[npartes][n]),sizeof(long)*(leidos-n));
						}
						*/
						
						for (j = leidos;j > n;j--)
						{
						   array[npartes][j] = array[npartes][j-1];
						   mapa_de_ordenacion[npartes][j]  = mapa_de_ordenacion[npartes][j-1];
						}
						
						array[npartes][n] = p1;
						mapa_de_ordenacion[npartes][n] = nreg;
						memcpy(p1,p2,tamc);
						p1 += tamc;
						
						leidos++;
						total++;
						if (leidos >= maxporparte)
						{
							nmapa_de_ordenacion[npartes] = leidos;
							npartes++;
							leidos=0;
							nmapa_de_ordenacion[npartes] = 0;
							mapa_de_ordenacion[npartes] = (long *)malloc(sizeof(long) * maxporparte);	
							array[npartes] = malloc(sizeof(char *)*maxporparte);	
						}
					}
				}
				else if (e < 0)
					break;
				nreg++;
			}
			no_report_error--;
			nmapa_de_ordenacion[npartes] = leidos;
		}
	}



	if (!npartes && !nmapa_de_ordenacion[0])
	{
no_hay_nada: /* */
		free(mapa_de_ordenacion[0]);
		free(array[0]);
		free(buffer);
		if (bufclave)
			free(bufclave);
	    free(flaglista);
	    free(array);
	    free(nmapa_de_ordenacion);
	    free(mapa_de_ordenacion);

		par->mapa_de_ordenacion = (long *)malloc(sizeof(long));
		par->mapa_de_ordenacion[0] = 0;
		par->nmapa_de_ordenacion = par->registros = 0;
		return 0;
	}
	

	par->mapa_de_ordenacion = (long *)malloc(sizeof(long) * total);	
	par->nmapa_de_ordenacion = par->registros = total;


    lista=0;	
	vminimo = array[0][0];
	memset(flaglista,0,sizeof(long)*(_NMERGESORT_+1));
	for (i = 1;i <= npartes;i++)
	{
		if ((*f_ord_compara)(array[i][0],vminimo,tamc) < 0)
		{
			lista = i;
			vminimo = array[i][0];
		}
	}	
	for (leidos = 0;leidos < total;leidos++)
	{
		par->mapa_de_ordenacion[leidos] = mapa_de_ordenacion[lista][flaglista[lista]];
		flaglista[lista]++;
		if (nmapa_de_ordenacion[lista] == flaglista[lista])
		{
			for (i = 0;i <= npartes;i++)
			{
				if (nmapa_de_ordenacion[i] > flaglista[i])
				{
					lista = i;
					vminimo = array[lista][flaglista[lista]];
					break;
				}
			}
			if (i > npartes)
			{
				leidos++;
				break; /* fin */
			}
			i++;
			for (;i <= npartes;i++)
			{
				if (nmapa_de_ordenacion[i] > flaglista[i] && (*f_ord_compara)(array[i][flaglista[i]],vminimo,tamc) < 0)
				{
					lista = i;
					vminimo = array[i][flaglista[i]];
				}
			}
		}
		else
		{
			vminimo = array[lista][flaglista[lista]];
			for (i = 0;i <= npartes;i++)
			{
				if (lista != i && nmapa_de_ordenacion[i] > flaglista[i] && (*f_ord_compara)(array[i][flaglista[i]],vminimo,tamc) <= 0)
				{
					lista = i;
					vminimo = array[lista][flaglista[i]];					
				}
			}
		}
	}
	/*
	if (leidos != total)
	{
		aviso(0,"algo anda mal");
	}
	*/
	free(buffer);
	if (bufclave)
		free(bufclave);
	for (i = 0;i <= npartes;i++)
	{
		free(array[i]);
		free(mapa_de_ordenacion[i]);
	}
	free(flaglista);
	free(array);
	free(nmapa_de_ordenacion);
	free(mapa_de_ordenacion);

    for (j = 0;j < par->nmapa_de_ordenacion;j++)
	{
		if (par->regsel == par->mapa_de_ordenacion[j])
		{
			par->reg = (j/*-par->offset*/)+1;
			par->regcur = par->reg;
			break;
		}
	}
	return 0;
}

static int rellena_mapa_ordenacion_natural(struct gridpars *par)
{
	int fd;
	long nreg;
	long mireg;
	long leidos = 0;
	int n,j;
	char **array;
	long nmapa_de_ordenacion;
	long *mapa_de_ordenacion;	
	FI_DEF *pfi;

	{
		    pfi = par->pfi;

			if (par->b_lim2)
			{
				memcpy(pfi->f_buf,par->b_lim2,pfi->f_lfich);
				if (lee_reg(pfi,ISGREAT) < 0)
				{
					if (lee_reg(pfi,ISLAST) < 0)
						return 0;
				}
				else
				{
					if (lee_reg(pfi,ISPREV) < 0)
						return 0;
				}
			}
			else if (lee_reg(pfi,ISLAST) < 0)
					return 0;
			nreg = isrecnum;


			if (par->b_lim1)
			{
				memcpy(pfi->f_buf,par->b_lim1,pfi->f_lfich);
				if (lee_reg(pfi,ISGTEQ) < 0)
				{
					if (lee_reg(pfi,ISFIRST) < 0)
						return 0;					
				}
			}
			else if (lee_reg(pfi,ISFIRST) < 0)
					return 0;
			mireg = isrecnum;


			nmapa_de_ordenacion = par->registros;

			mapa_de_ordenacion = (long *)malloc(sizeof(long) * nmapa_de_ordenacion);
			par->mapa_de_ordenacion = mapa_de_ordenacion;
			

			isrecnum = mireg;
			do
			{			
				if ( (par->gmodo & 128) && salta_limites(par))
				   continue;	
				mireg = isrecnum;				

				if (hay_dat_permiso(pfi,-2))
				{
					mapa_de_ordenacion[leidos] = mireg;
					leidos++;
				}
				if (nreg == mireg)
					break;
			} while(lee_reg(pfi,ISNEXT) >= 0 && leidos < nmapa_de_ordenacion);
			nmapa_de_ordenacion = leidos;	
		
			par->nmapa_de_ordenacion = par->registros = nmapa_de_ordenacion;
	}


    for (j = 0;j < par->nmapa_de_ordenacion;j++)
	{
		if (par->regsel == par->mapa_de_ordenacion[j])
		{
			par->reg = (j/*-par->offset*/)+1;
			par->regcur = par->reg;
			break;
		}
	}
	return 0;
}

static int rellena_mapa_ordenacion(struct gridpars *par,int campo)
{
	int r;

	if (par->cache)
	{
		int i;
		for (i = 0;par->cache[i].id_reg;i++)
		{
			par->cache[i].id_reg = -1L;
		}
    }

	if (par->mapa_de_ordenacion) {
		free(par->mapa_de_ordenacion);
		par->mapa_de_ordenacion = NULL;
		par->nmapa_de_ordenacion = 0;
	}
	par->registros = TotalRegistros(par); 
	if (campo == -1)
		r = rellena_mapa_ordenacion_natural(par);
	else
	{
		if (campo >= 0 && campo < par->pfi->f_ncampos && par->pfi->f_campos[campo].c_tipo == DOUBLETIPO)
		{
			f_ord_compara = compara_double;
		}
		else
		{
			f_ord_compara = memcmp;
		}
		if (par->registros < (FACTOR_MERGE*2)) /* estimado de cache del procesador .... */
		{
			r =  rellena_mapa_ordenacion_0(par,campo);
		}
		else
		{
			r = rellena_mapa_ordenacion_1(par,campo);
		}
	}
	if (par->mapa_de_ordenacion)
	{
		par->campo_de_ordenacion = campo;
	}

	return r;
}


/*
static long RegSegunMapaAsignacion(struct gridpars *par,long opos)
{
	long posicion;
	if (!par->mapa_de_asignacion)
		return opos;

	for (posicion = 0;posicion < par->nmapa_de_asignacion;posicion++)
	{
		if ((posicion + par->mapa_de_asignacion[posicion]) == opos)
			return posicion;
	}
	return opos;
}
*/
#define RegSegunMapaAsignacion(a,b) (b)

static void saca_rgbcolor(unsigned char *rgbc,char *buf,int len) {
  char *p,*p1;  
  char tmp[512];
  int cc,idx;
  memcpy(tmp,buf,len);
  tmp[len] = 0;
  p1 = p = tmp;
  idx = 0;
  while(*p && idx < 6) {
	  if (*p == ',' || *p == ';' || *(p+1) == 0) {
		  if (*p == ',' || *p == ';')
			*p = 0;
		  cc = atoi(p1);
		  p1 = p + 1;
		  rgbc[idx] = (unsigned char)cc;
		  idx++;
	  }
	  p++;
  }
}

void FGVHookGrid(char *bloque,struct gridpars *par,int el_punto)
{
long registro;
long dif;
int desde_campo;
int hasta_campo;
int campo,i,j,_campo;
int offset;
int operacion;
char tope = 0;
long from,to;
int hit = 0;
long totc,topex;
long preg;
long cursel;
int controlx;
int ok;
int reindexa = 0;
int flagcache = 0;
double vcf;
long flagreg = 0;


    if (!par || RetroPar[el_punto] != (char *)par) return;

	if (((struct gridpars *)RetroPar[el_punto])->structid != 0x3212)
		return;

	if (par->sesion_id != ap_sesion_id)
		return;

    bloque += (sizeof(long)*3);

	memcpy((char *)&operacion,bloque,sizeof(int));
	bloque += sizeof(int);


	/*
	{
		char tmp[512];
		sprintf(tmp,"operacion=%d",operacion);
		aviso(0,tmp);
	}
	*/

    if (!operacion)
    {
		int modo_completo = 0;
		int he_leido = 0;
		int iijj;
		char *pb1;
        /*pulsatecla();*/

        memcpy((char *)&registro,bloque,sizeof(long));
	    bloque += sizeof(long);
	    memcpy((char *)&desde_campo,bloque,sizeof(int));
	    bloque += sizeof(int);
	    memcpy((char *)&hasta_campo,bloque,sizeof(int));
	    bloque += sizeof(int);

		pb1 = bloque;
	    memcpy((char *)&from,bloque,sizeof(long));
	    bloque += sizeof(long);
	    memcpy((char *)&to,bloque,sizeof(long));
	    bloque += sizeof(long);
	    memcpy((char *)&cursel,bloque,sizeof(long));
	    bloque += sizeof(long);
		if (cursel > 0 && cursel <= par->registros)
		{
			if (par->regcur != cursel)
			{
				par->regsel = 0;
			}
			par->regcur = cursel;
		}
		

        /*
        if (registro <= from || registro > (to+1))
        {
            DebugBreak();
        }
        */        


        if ((unsigned long)registro == (unsigned long)0xFFFFFFFF)
            registro = par->registros;
		else
		if ((unsigned long)registro == (unsigned long)0xFFFFFFFE)
		{
			registro = cursel;
			modo_completo = 1;			
		}

		/*
		if (!modo_completo)
		{
			from -= 2;
			to += 2;        
		}
		*/
        if (from < 0) from = 0;
        if (to < from)
        {
            totc = from;
            from = to;
            to = totc;
        }
		topex = to - from + 1; 
        totc = to - from + 1;

		if (!modo_completo && registro > par->registros)
        {			
			if (par->b_lim1)
			{
				memcpy(par->pfi->f_buf,par->b_lim1,par->pfi->f_lfich);
			}
			else
			if (par->b_lim2)
			{
				memcpy(par->pfi->f_buf,par->b_lim2,par->pfi->f_lfich);
			}		
			else
			{
				defecto_ficha(par->pfi);
			}			
            isrecnum = 0;
        }
        else
        {
            controlx = 0;
            do
            {
                controlx++;
                if (controlx > 2)
                {
#ifdef FGVDOS
#ifdef _DEBUG
                    /*DebugBreak();*/
#endif
#endif
                    if (par->b_lim1)
                    {
                        memcpy(par->pfi->f_buf,par->b_lim1,par->pfi->f_lfich);
                    }
                    else
                    if (par->b_lim2)
                    {
                        memcpy(par->pfi->f_buf,par->b_lim2,par->pfi->f_lfich);
                    }
                    else
                    {
                        defecto_ficha(par->pfi);
                    }
                    break;
                }
                if (par->cache)
                {
					if (modo_completo)
					{						
						hit = 1;
						for (i = 0;par->cache[i].id_reg;i++)
						{
							if (par->cache[i].id_reg != (from+i+1))
							   break;
						}
						if (i < topex)
						{
							hit = 0;
						}
					}
					else
					{
						for (i = 0;par->cache[i].id_reg;i++)
						{
							if (par->cache[i].id_reg == registro)
							{
								memcpy(par->pfi->f_buf,par->cache[i].buf,par->pfi->f_lfich);
								isrecnum = par->cache[i].isrec;
								par->reg = registro;
								hit = 1;
								break;
							}
						}
					}
                }
                if (!hit)
                {
                    if (par->cache)
                    {
                        for (i = 0;par->cache[i].id_reg;i++)
							par->cache[i].id_reg = -1L;
                        if (i < topex)
                        {
                            for (i = 0;par->cache[i].id_reg;i++)
                            {
                                free((char *)par->cache[i].buf);                            
                            }
                            free((char *)par->cache);
                            par->cache = NULL;
                        }
                    }

					if (par->mapa_de_ordenacion)
					{
						if (!par->cache)
						{
							par->cache = (struct mini_cache *)malloc(sizeof(struct mini_cache) * (topex+1));
							for (i = 0;i < topex;i++)
							{
								par->cache[i].id_reg = -1L;
								par->cache[i].buf = malloc(par->pfi->f_lfich+1);
							}
							par->cache[i].id_reg = 0;
							par->cache[i].buf = NULL;
						}
						if (to >= (par->registros+(par->gmodo&1)))
						{

							to = par->registros - 1;
							from = (to - totc) + 1;
							totc = to - from + 1;
						}
						if (from < 0)
						{
							from = 0L;
							to = totc - 1;
							if (to >= par->registros)
							{
								to = par->registros - 1;
							}
							totc = to - from + 1;
						}
						for (i = 0;i < totc && (from+i) <= to;i++)
						{
							j = /*par->offset+*/from+i;

							if (j >= par->nmapa_de_ordenacion || j >= par->registros) {
								if (par->b_lim1)
								{
									memcpy(par->pfi->f_buf,par->b_lim1,par->pfi->f_lfich);
								}
								else
								if (par->b_lim2)
								{
									memcpy(par->pfi->f_buf,par->b_lim2,par->pfi->f_lfich);
								}		
								else
								{
									defecto_ficha(par->pfi);
								}			
								isrecnum = 0;
								par->cache[i].id_reg = j + 1;
								par->cache[i].isrec  = isrecnum;
								par->reg = j + 1;
								memcpy(par->cache[i].buf,par->pfi->f_buf,par->pfi->f_lfich);
							}
							else {
								par->cache[i].id_reg = from+i+1;
								par->cache[i].isrec = par->mapa_de_ordenacion[j];
								par->reg = from + i + 1;

								he_leido = 1;
								lee_directo(dame_fd_datos(par->pfi),par->cache[i].isrec,par->pfi,par->pfi->f_buf);

								memcpy(par->cache[i].buf,par->pfi->f_buf,par->pfi->f_lfich);
							}
							if (tope)
							{
								registro = from + i + 1;
							}
						}
					}
					else
					{
						/****/
						i = ixx_inicio_lectura_multiple(par->pfi->f_fd);
						if (i == 1 || par->atencion)
						{
							par->atencion = 0;
							par->totales_sucios = 1;
							if (par->cache)
							{
								for (i = 0;par->cache[i].id_reg;i++)
								{
									par->cache[i].id_reg = -1L;
								}
							}                
							/* REPOSICIONAR!!!! */
							/* determinar el nuevo total de registros */
                        
							if (cursel != par->regcur && cursel == par->registros)
								cursel = 0xffffffff;

							par->registros = TotalRegistros(par);

							/* y reajustar corriente */
							tope = 1;
							no_report_error++;
							pon_registro_actual(par->pfi,par->regsel );
							apunta_ind(par->pfi,0,ISEQUAL,0);
							apunta_ind(par->pfi,par->encl+1,ISCURR,0);
							lee_reg(par->pfi,ISCURR);
							par->regsel = isrecnum;
							no_report_error--;
							/*
							if (par->regsel < 1 || par->regsel > 1000000) {
								par->regsel = 0;
							}
							*/
                        
							preg = (IxxNumeroPosicion - par->offset) + 1;                        
							par->reg = preg;

							registro += (preg-par->regcur);
							from += (preg-par->regcur);
							to += (preg-par->regcur);

							if (cursel == 0xffffffff)
							{
							   to = par->registros - 1;
							   from = (to - totc) + 1;
							   totc = to - from + 1;
							   cursel = par->registros;                         
							}
							else
							   cursel += (preg-par->regcur);

							par->regcur = preg;						

							if (to >= (par->registros+(par->gmodo&1)))
							{
								to = par->registros - 1;
								from = (to - totc) + 1;
								totc = to - from + 1;
							}
							if (from < 0)
							{
								from = 0L;
								to = totc - 1;
								if (to >= par->registros)
								{
									to = par->registros - 1;
								}
								totc = to - from + 1;
							}

							if (registro <= from)                            
							{
								registro = from + 1;
							}
							if (registro > (to+1))
							{
								registro = to - 1;
							}
							topex = to - from + 1; 
							totc = to - from + 1;
						}					

						if (!par->cache)
						{
							par->cache = (struct mini_cache *)malloc(sizeof(struct mini_cache) * (topex+1));
							for (i = 0;i < topex;i++)
							{
								par->cache[i].id_reg = -1L;
								par->cache[i].buf = malloc(par->pfi->f_lfich+1);
							}
							par->cache[i].id_reg = 0;
							par->cache[i].buf = NULL;
						}


						if (!modo_completo)
						{
							for (i = 0;par->cache[i].id_reg;i++)
							{
								if (par->cache[i].id_reg <= from || par->cache[i].id_reg > (to+1))
									par->cache[i].id_reg = -1L;
							}
						}

						for (i = 0;i < totc && (from+i) <= to;i++)
						{
							if (!modo_completo)
							{
								for (j = 0;par->cache[j].id_reg;j++)
								{
									if (par->cache[j].id_reg == (from+i+1))
									{
										isrecnum = par->cache[j].isrec;
										break;
									}
								}
							}
							else
							{							
								j = i;
								par->cache[j].id_reg = 0;							
							}
							if (!par->cache[j].id_reg)
							{
								if (!modo_completo)
								{
									for (j = 0;par->cache[j].id_reg;j++)
									{
										if (par->cache[j].id_reg == -1L)
										{
											break;
										}
									}
								}
								else
								{
									par->cache[j].id_reg = -1;
								}
								if (par->cache[j].id_reg)
								{	
									long skipped = 0;
									if ((from+i) >= par->registros) {
										if (par->b_lim1)
										{
											memcpy(par->pfi->f_buf,par->b_lim1,par->pfi->f_lfich);
										}
										else
										if (par->b_lim2)
										{
											memcpy(par->pfi->f_buf,par->b_lim2,par->pfi->f_lfich);
										}		
										else
										{
											defecto_ficha(par->pfi);
										}			
										isrecnum = 0;
										par->cache[j].id_reg = from + i + 1;
										par->cache[j].isrec  = isrecnum;
										par->reg = from + i + 1;
										memcpy(par->cache[j].buf,par->pfi->f_buf,par->pfi->f_lfich);
										if (tope)
										{
											registro = from + i + 1;
										}
										break;
									}
									else {
										he_leido = 1;
										no_corrup_error = 1;
										ok = grid_lee_por_posicion(par,par->encl+1,par->offset+from+i,par->pfi->f_buf,0,0,&from,&to);
										no_corrup_error = 0;
										if (ok < 0 && ixxstatus == IXXECORRUP)
										{
											reindexa = 1;
										}
										if (ok >= 0)
										{									
											if (ok == 999)
											{
												if (cursel != par->regcur)
												{
													cursel = par->regcur;
												}										
												for (iijj = 0;par->cache[iijj].id_reg;iijj++)
												{
													/*if (par->cache[j].id_reg > from + i + 1)*/
													{
														par->cache[j].id_reg = -1;
													}
												}
												tope = 98;										
												break;
											}
											par->cache[j].id_reg = from + i + 1;
											par->cache[j].isrec  = isrecnum;
											par->reg = from + i + 1;
											memcpy(par->cache[j].buf,par->pfi->f_buf,par->pfi->f_lfich);									
											if (tope)
											{
												registro = from + i + 1;
											}
											
										}
										else
										{									
											if (ok == -999)
											{

												cursel = par->regcur;
												tope = 4;

											}
											else if (modo_completo)
											{
												if (par->b_lim1)
												{
													memcpy(par->pfi->f_buf,par->b_lim1,par->pfi->f_lfich);
												}
												else
												if (par->b_lim2)
												{
													memcpy(par->pfi->f_buf,par->b_lim2,par->pfi->f_lfich);
												}		
												else
												{
													defecto_ficha(par->pfi);
												}			
												isrecnum = 0;
												par->cache[j].id_reg = from + i + 1;
												par->cache[j].isrec  = isrecnum;
												par->reg = from + i + 1;
												memcpy(par->cache[j].buf,par->pfi->f_buf,par->pfi->f_lfich);
												to = from + i;
												topex = to - from + 1; 
												totc = to - from + 1;
											}
											break;
										}
									}
								}
							}
						}


						ixx_fin_lectura_multiple(par->pfi->f_fd);

						if (tope == 98)
						{
							tope = 0;
							controlx = 0;
							flagcache = 1;
							continue;
						}

						if (reindexa)
						{
							reindexa = 0;
							ixx_reconstruye(par->pfi->f_fd);
							continue;
						}
						if (tope) 
						{
							break;
						}
						/****/
						continue;
					}
                }
                break;
            } while(-1);
        }       
		
        *bloque = tope;
		if (flagcache)
		{
			*bloque += 100;
		}
        offset = 1;
		if (modo_completo)
		{
			memcpy(pb1,(char *)&from,sizeof(long));
			pb1 += sizeof(long);
			memcpy(pb1,(char *)&to,sizeof(long));
		}

		if (tope != 99)
		{			
			int contotal,ci;
			int cc;
			int ini_cc = 0;
			int fin_cc = 1;
			if (modo_completo)
			{
				fin_cc = (to-from)+1;
			}
			for (j = ini_cc;j < fin_cc;j++)
			{
				if (modo_completo)
				{
					memcpy(par->pfi->f_buf,par->cache[j].buf,par->pfi->f_lfich);
					isrecnum = par->cache[j].isrec;					
					registro = par->cache[j].id_reg;
				}
				for (_campo = desde_campo;_campo <= hasta_campo;_campo++)
				{  
				   bloque[offset] = 0;
				   campo = par->colcampos[_campo];
				   if (campo < 0) break;
				   cc = dInfoCampo(par->InfoCampos,campo,0)->Id;
				   if (par->registros >= registro)
				   {
					   formato_campo(par->pfi,cc,bloque+offset);
				   }
				   else
				   {
						contotal = -1;
						if ((par->gmodo & 0x100000) && par->totales)
						{
						   for (ci = 0;ci < par->totales[0];ci++)
						   {
							   if ((par->totales[ci+1]&0xffff) == cc)
							   {
								   contotal = ci;
								   break;
							   }
						   }
						}
						if (contotal > -1)
						{
							st_doble(cc,par->pfi,par->t_totales[contotal]);
							formato_campo(par->pfi,cc,bloque+offset);
							if (par->totales_sucios)
							{
								bloque[offset] = '?';
							}
						}
						else
						{
							memset(bloque+offset,' ',dInfoCampo(par->InfoCampos,campo ,0)->c_maxqc);
						}
				   }
				   offset += dInfoCampo(par->InfoCampos,campo ,0)->c_maxqc;
				}				
				if (modo_completo)
				{
					flagreg = 0;
					if (cursel == par->cache[j].id_reg)
					{
						par->regsel = isrecnum;
						/*
						if (par->regsel < 1 || par->regsel > 1000000) {
							par->regsel = 0;
						}
						*/
						par->regcur = registro;						
					}
					memcpy(bloque+offset,&isrecnum,sizeof(long));
					offset += sizeof(long);
					if (par->sqltoscreen && par->sqltoscreen->campototal)
					{
						int pc;
						pc = par->sqltoscreen->campototal-1;
						if (*(par->pfi->f_buf+par->pfi->f_campos[pc].c_offset) == 'T')
							flagreg |= 1;
					}
					memcpy(bloque+offset,&flagreg,sizeof(long));
					offset += sizeof(long);

					/* añadir color */
					{
						unsigned char rgbc[6];
						memset(rgbc,0,6);
						if (par->rgbcolor > -1 && par->rgbcolor < par->pfi->f_ncampos && par->pfi->f_campos[par->rgbcolor].c_qc > 6) {
							saca_rgbcolor(rgbc,par->pfi->f_buf+par->pfi->f_campos[par->rgbcolor].c_offset,par->pfi->f_campos[par->rgbcolor].c_qc);
						}
						memcpy(bloque+offset,rgbc,6);
						offset += 6;
					}
					/****************/
				}
			}
		}
        if (!modo_completo && cursel == registro)
        {
           par->regsel = isrecnum;
		   /*
			if (par->regsel < 1 || par->regsel > 1000000) {
				par->regsel = 0;
			}
			*/

           par->regcur = registro;
        }
		if (!modo_completo)
		{
			flagreg = 0;
			if (par->sqltoscreen && par->sqltoscreen->campototal)
			{
				int pc;
				pc = par->sqltoscreen->campototal-1;
				if (*(par->pfi->f_buf+par->pfi->f_campos[pc].c_offset) == 'T')
					flagreg |= 1;
			}
			memcpy(bloque+offset,&isrecnum,sizeof(long));
			offset += sizeof(long);
			memcpy(bloque+offset,&flagreg,sizeof(long));
			offset += sizeof(long);

			/* añadir color */
			{
				unsigned char rgbc[6];
				memset(rgbc,0,6);
				if (par->rgbcolor > -1 && par->rgbcolor < par->pfi->f_ncampos && par->pfi->f_campos[par->rgbcolor].c_qc > 6) {
					saca_rgbcolor(rgbc,par->pfi->f_buf+par->pfi->f_campos[par->rgbcolor].c_offset,par->pfi->f_campos[par->rgbcolor].c_qc);
				}
				memcpy(bloque+offset,rgbc,6);
				offset += 6;
			}
			/****************/
		}

		if (he_leido)		
		{
			if (par->regsel)
			{				
				pon_registro_actual(par->pfi,par->regsel );
				no_report_error++;
				if (apunta_ind(par->pfi,0,ISEQUAL,0) < 0)
				{					
					apunta_ind(par->pfi,par->encl+1,ISCURR,0);					
					par->regsel = isrecnum;
					/*
					par->regsel = 0;
					par->regcur = 0;
					cursel = 0;
					*/
				}
				else
				{
					lee_reg(par->pfi,ISCURR);
					apunta_ind(par->pfi,par->encl+1,ISCURR,0);
					par->regsel = isrecnum;
				}
				no_report_error--;
			}
			else
			{
				apunta_ind(par->pfi,par->encl+1,ISCURR,0);
			}
		}

        memcpy(bloque+offset,&(par->registros),sizeof(long));
        offset += sizeof(long);
        memcpy(bloque+offset,&cursel,sizeof(long));       

    }
    else if (operacion == 1)
    {
        char tmpxx[512];
		int debughazlo = 0;

        memcpy((char *)&registro,bloque,sizeof(long));
	    bloque += sizeof(long);
        par->regsel = registro;
		
		if (registro < 0 /*|| registro > tam_btrv(par->pfi)*/)
			return;
		

        memcpy((char *)&(par->regcur),bloque,sizeof(long));
        bloque += sizeof(long);
        memcpy((char *)&(par->curcol),bloque,sizeof(int));
        bloque += sizeof(int);
        memcpy((char *)&(par->regsel),bloque,sizeof(long));
		/*
		if (par->regsel < 1 || par->regsel > 1000000) {
			par->regsel = 0;
		}
		*/
        bloque += sizeof(long);
        if (!par->regsel && par->regcur)
        {
            if (par->cache)
            {
                for (i = 0;par->cache[i].id_reg;i++)
                {
                    if (par->cache[i].id_reg == par->regcur)
                    {                        
                        par->regsel = par->cache[i].isrec;
						/*
						if (par->regsel < 1 || par->regsel > 1000000) {
							par->regsel = 0;
						}
						*/
                        break;
                    }
                }
            }
        }

		PintaOtrosCampos(par);
		
		if ((par->gmodo & 1024))
		{
			int ol = v_mpfirel(par->ipfi)->lineas;
			int numcam;
			v_mpfirel(par->ipfi)->lineas = par->lineas;
			numcam = campo_linea(par->ipfi);

			if (/*alta == CONSULTA || */actualiza_cam(par->ipfi,numcam,100,7) > -1) 
			{
				 /*if (qj == -9 || qj == -3)*/
				 {
					tras_entra(0,numcam,CONSULTA,1,0,par->ipfi,100);
					actualiza_cam(par->ipfi,numcam,100,11);
					/*
					if (actualiza_cam(ipfi,numcam,100,11))
					   goto a_repeticion;
					   */
				 }
				 /*
				 else
				 {
					if (tras_entra(numpan,numcam,alta,enclave,modificado,ipfi,faclin)< 0 && cr && modif && !visual)
					   goto a_repeticion;
				 }
				 */
			}
			v_mpfirel(par->ipfi)->lineas = ol;
		}
		if (par->hook)
        {
			int cc;
			if (par->regsel)
			{
				no_report_error++;
				pon_registro_actual(par->pfi,par->regsel );
				apunta_ind(par->pfi,0,ISEQUAL,0);
				apunta_ind(par->pfi,par->encl+1,ISCURR,0);
				lee_reg(par->pfi,ISCURR);
				no_report_error--;
			}
			else
			{
				defecto_ficha(par->pfi);
			}
			/*
			if (strstr(par->pfi->f_fichero,"pcegm004")) {
				for (_campo = 0;_campo < par->ncampos;_campo++) {
					campo = par->colcampos[_campo];
					if (campo < 0) break;
					i = dInfoCampo(par->InfoCampos,campo ,0)->c_maxqc;
					cc = dInfoCampo(par->InfoCampos,campo ,0)->Id;
					if (cc == 35) {
						if (memcmp(bloque,"   ",3) && memcmp(bloque,"EUR",3)) {
							aviso(0,"buffer corrompido");
							debughazlo = 1;
						}
					}
					bloque += i;
				}
			}
			*/

			if (!debughazlo)
			{
				for (_campo = 0;_campo < par->ncampos;_campo++)
				{
					campo = par->colcampos[_campo];
					if (campo < 0) break;
					cc = dInfoCampo(par->InfoCampos,campo ,0)->Id;
					i = dInfoCampo(par->InfoCampos,campo ,0)->c_maxqc;
					memcpy(tmpxx,bloque,i);
					tmpxx[i] = 0;
					if (par->pfi->f_campos[cc].c_tipo == INTTIPO || par->pfi->f_campos[cc].c_tipo == DOUBLETIPO)
					{
						quita_char(tmpxx,'.');
						reemplaza_char(tmpxx,',','.');
					}
					st_campo(cc,par->pfi,tmpxx);
					bloque += i;
				}

				sprintf(v_memo(5004)->alfabetica,"%d",1);
				ignorar_aviso_64++;
				p_ejecuta(par->hook,O_NOR,0,24);
				ignorar_aviso_64--;
			}
        }        

    }
    else if (operacion == 2)
    {      
        int enviar = 1;

        memcpy((char *)&campo,bloque,sizeof(int));        
	    bloque += sizeof(int);

        campo = par->colcampos[campo];
        if (campo < 0)
        {
            enviar = 0;
        }
        else if (par->pfi->f_campos[ dInfoCampo(par->InfoCampos,campo,0)->Id ].c_qc && strlen(bloque) > 2)
        {
             char *conte = NULL;
             char *pp;

             pp = bloque;             
             while(*pp)
             {
                if (*pp == '\\' && *(pp+1) == '\\')
                {
                   *pp = 0;
                   if (conte)
                   {
                      strmayus(conte);
                      /*
                      ixx_pon_contenido(par->pfi->f_fd,conte,strlen(conte));
                      par->haycontenido = 1;
                      */
                   }
                   conte = pp+2;				   
                   pp++;
				   break;
                }
                pp++;
             }
             if (conte)
             {
                char titulo[512];
                struct grid_filtro filtro;
                strmayus(conte);
                /*
                ixx_pon_contenido(par->pfi->f_fd,conte,strlen(conte));                
                par->haycontenido = 1;
                */
                strcpy(titulo,par->titulo);
                strcat(titulo," Conteniendo '");
                strcat(titulo,conte);
                strcat(titulo,"' en ");
                strcat(titulo,par->pfi->f_campos[ dInfoCampo(par->InfoCampos,campo,0)->Id ].c_nombre);
                memset(&filtro,0,sizeof(filtro));
                filtro.tipo = 3;
				filtro.mayus = 1;
                filtro.campo = dInfoCampo(par->InfoCampos,campo,0)->Id;
                strcpy(filtro.valor,conte);

                LanZaHiJo(par,titulo,1,&filtro,NULL,NULL,0,0,0);
                enviar = 0;
             }
        }

        if (enviar)
        {
			if (par->mapa_de_ordenacion && par->campo_de_ordenacion > -1)
			{
				int n,x;
				char tmp[512];			

				campo = dInfoCampo(par->InfoCampos,campo,0)->Id;
				st_campo(campo,par->pfi,bloque);
				
			    formato_campo(par->pfi,campo,tmp);
				/*
                n = strlen(tmp);

				strcpy(tmp,bloque);
				for (x = strlen(tmp);x < n;x++)
				{
					tmp[x] = ' ';
				}
				tmp[x] = 0;
				*/

				strmayus(tmp);
				n = busca_mapa_ordenacion_2(tmp,par->mapa_de_ordenacion,0,par->nmapa_de_ordenacion,campo,par->pfi,1);
				if (n < 0 || n >= par->nmapa_de_ordenacion)
					n = par->nmapa_de_ordenacion-1;
				else
				{
					n = avanza_mapa_ordenacion_2(tmp,par->mapa_de_ordenacion,n,par->nmapa_de_ordenacion-1,campo,par->pfi,1);
					quitab(tmp);					
					n = retrocede_mapa_ordenacion_2(tmp,par->mapa_de_ordenacion,n,campo,par->pfi,0,1);
				}

				pon_registro_actual(par->pfi,par->mapa_de_ordenacion[n]);
				apunta_ind(par->pfi,0,ISEQUAL,0);
				apunta_ind(par->pfi,par->encl+1,ISCURR,0);
				lee_reg(par->pfi,ISCURR);

				par->regsel = par->mapa_de_ordenacion[n];
				/*
				if (par->regsel < 1 || par->regsel > 1000000) {
					par->regsel = 0;
				}
				*/
				par->reg = (n/*-par->offset*/) + 1;				
				par->regcur = par->reg;
			}
			else
			{
				/*
				par->registros = TotalRegistros(par);

				pon_registro_actual(par->pfi,par->regsel );
				apunta_ind(par->pfi,0,ISEQUAL,0);
				apunta_ind(par->pfi,par->encl+1,ISCURR,0);
				lee_reg(par->pfi,ISCURR);

				st_campo(dInfoCampo(par->InfoCampos,campo ,0)->Id,par->pfi,bloque);        
				if (lee_reg(par->pfi,ISGTEQ) < 0)
				{
					lee_reg(par->pfi,ISLAST);
				}                        
				par->regsel = isrecnum;
				par->reg = RegSegunMapaAsignacion(par,IxxNumeroPosicion - par->offset) + 1;            
				par->regcur = par->reg;
				*/
				par->registros = TotalRegistros(par);

				no_report_error++;
				pon_registro_actual(par->pfi,par->regsel );
				apunta_ind(par->pfi,0,ISEQUAL,0);
				apunta_ind(par->pfi,par->encl+1,ISCURR,0);
				lee_reg(par->pfi,ISCURR);
				no_report_error--;

				/*formato_campo(FI_DEF *pfi,int campo,char *qo)*/

				st_campo(dInfoCampo(par->InfoCampos,campo ,0)->Id,par->pfi,bloque);
				//ixx_nocase = 1;
				if (lee_reg(par->pfi,ISGTEQ) < 0)
				{
					//ixx_nocase = 0;
					lee_reg(par->pfi,ISLAST);
					while (!hay_dat_permiso(par->pfi,-2) && lee_reg(par->pfi,ISPREV) >= 0);
				}
				else
				{
					//ixx_nocase = 0;
					while (!hay_dat_permiso(par->pfi,-2) && lee_reg(par->pfi,ISNEXT) >= 0);
				}
				par->regsel = isrecnum;				
				/*
				if (par->regsel < 1 || par->regsel > 1000000) {
					par->regsel = 0;
				}
				*/
				par->reg = RegSegunMapaAsignacion(par,IxxNumeroPosicion - par->offset) + 1;            

			}

            bloque -= sizeof(int);
            memcpy(bloque,&(par->reg),sizeof(long));
            par->regcur = par->reg;
            bloque += sizeof(long);
            memcpy(bloque,&(par->registros),sizeof(long));

            if (par->cache)
            {
                for (i = 0;par->cache[i].id_reg;i++)
                {
                    par->cache[i].id_reg = -1L;
                }
            }

			/*
			if (par->hook)
			{
				char tmpxx[512];
				int cc;
				if (par->regsel)
				{
					pon_registro_actual(par->pfi,par->regsel );
					apunta_ind(par->pfi,0,ISEQUAL,0);
					apunta_ind(par->pfi,par->encl+1,ISCURR,0);
					lee_reg(par->pfi,ISCURR);
				}
				else
				{
					defecto_ficha(par->pfi);
				}

				sprintf(v_memo(5004)->alfabetica,"%d",1);
				p_ejecuta(par->hook,O_NOR,0,24);
			} 
			*/
			

        }
        else
        {
            registro = 0L;
            bloque -= sizeof(int);
            memcpy(bloque,&registro,sizeof(long));
        }        
    }
    else if (operacion == 3)
    {
        int key;


        memcpy(&key,bloque,sizeof(int));
        if (par->cache)
        {
            for (i = 0;par->cache[i].id_reg;i++)
            {
                par->cache[i].id_reg = -1L;
            }
        }

		if (par->mapa_de_ordenacion)
		{
			free(par->mapa_de_ordenacion);
			par->mapa_de_ordenacion = NULL;
			par->nmapa_de_ordenacion = 0;
		}

		if (key < -1)
		{
            char titulo[512];
            struct grid_filtro filtro;

			key = (-key) - 2;
			if (key >= 0 && key < par->pfi->f_ncampos)
			{				
				par->registros = TotalRegistros(par);
				if (par->registros < 1000000)
				{
					if (par->registros > 1)
					{
						rellena_mapa_ordenacion(par,key);
					}
					key = (-key) - 2;
				}
				else
				{
					strcpy(titulo,par->titulo);
					strcat(titulo," Ordenado por ");
					strcat(titulo,par->pfi->f_campos[key].c_nombre);				
					memset(&filtro,0,sizeof(filtro));
					filtro.tipo = 4 | 0x10;
					filtro.campo = key;	
					
					/*
					if (par->padre)
					{
						LanZaHiJo(par->padre,titulo,1,&filtro,NULL,NULL,1,0,0);
						key = -9999;
					}
					else				
					*/
					{					
						LanZaHiJo(par,titulo,1,&filtro,NULL,NULL,0,0,0);
						key = -1;
					}
				}
			}			
			else
				key = -1;
		}
		else if (key < par->pfi->f_nclaves && key >= 0)
        {
            par->InfoCampos->clave_actual = key;
            par->encl = key;
            ixx_inicio_lectura_multiple(par->pfi->f_fd);
			if ((par->gmodo & 128))
			{
				apunta_ind(par->pfi,key+1,ISCURR,0);
				par->reg = 1;
				par->regcur = par->reg;
				par->regsel = isrecnum;
				/*
				if (par->regsel < 1 || par->regsel > 1000000) {
					par->regsel = 0;
				}
				*/
				par->registros = TotalRegistros(par);
			}
			else
			{
				no_report_error++;
				pon_registro_actual(par->pfi,par->regsel );
				apunta_ind(par->pfi,0,ISEQUAL,0);
				apunta_ind(par->pfi,key+1,ISCURR,0);        
				no_report_error--;
				par->reg = IxxNumeroPosicion+1;
				par->reg -= par->offset;
				par->regcur = par->reg;
				par->regsel = isrecnum;
				/*
				if (par->regsel < 1 || par->regsel > 1000000) {
					par->regsel = 0;
				}
				*/
				par->registros = TotalRegistros(par);
			}            


            ixx_fin_lectura_multiple(par->pfi->f_fd);
        }
        else
        {
            key = -1;
        }

        memcpy(bloque,&key,sizeof(int));
        bloque += sizeof(int);
        memcpy(bloque,&par->regcur,sizeof(long));
        bloque += sizeof(long);
        memcpy(bloque,&par->registros,sizeof(long));
        bloque += sizeof(long);        
    }
    else if (operacion == 4)
    {       
        memcpy((char *)&registro,bloque,sizeof(long));
	    bloque += sizeof(long);
		if (par->mapa_de_ordenacion)
		{
			i = 0;
			memcpy(bloque,&i,sizeof(int));
			bloque += sizeof(int);
			for (i = 0;i < par->nmapa_de_ordenacion;i++)
			{
				if (par->mapa_de_ordenacion[i] == registro)
				{
					par->reg = (i - par->offset)+1;//RegSegunMapaAsignacion(par,i - par->offset)+1;
					par->regcur = par->reg;
					par->regsel = registro;
					/*
					if (par->regsel < 1 || par->regsel > 1000000) {
						par->regsel = 0;
					}
					*/
					isrecnum = registro;
					pon_registro_actual(par->pfi,registro );
					apunta_ind(par->pfi,0,ISEQUAL,0);
					apunta_ind(par->pfi,par->encl+1,ISCURR,0);
					break;
				}
			}
		}
		else
		{
			i = ixx_inicio_lectura_multiple(par->pfi->f_fd);
			if (par->atencion)
			{
				i = 1;
				par->atencion = 0;
			}
			memcpy(bloque,&i,sizeof(int));
			bloque += sizeof(int);
			if (i == 1)
			{
				if (par->cache)
				{
					for (i = 0;par->cache[i].id_reg;i++)
					{
						par->cache[i].id_reg = -1L;
					}
				}
			}

			isrecnum = registro;
			pon_registro_actual(par->pfi,registro );
			apunta_ind(par->pfi,0,ISEQUAL,0);
			apunta_ind(par->pfi,par->encl+1,ISCURR,0);
			par->reg = RegSegunMapaAsignacion(par,IxxNumeroPosicion - par->offset)+1;
			par->regsel = isrecnum;
			/*
			if (par->regsel < 1 || par->regsel > 1000000) {
				par->regsel = 0;
			}
			*/
			par->regcur = par->reg;
			ixx_fin_lectura_multiple(par->pfi->f_fd);
		}

        memcpy(bloque,&par->regcur,sizeof(long));
        bloque += sizeof(long);
    }
    else if (operacion == 5) /* rellenar hijo */
    {
        long contad/* = time(NULL)*/;
		long tpulsa;
        long tactual = 0;
        long registro;
        int er,hay = 0;
        int fd,ofd,latecla;
        int porcontenido = 0;
		int conlimites = 0;
		int laclavelim;
		FI_DEF *pfipadre;
		int nf;
		char salvaorignom[MAXPATH];
		

		
        if (!par->padre /* || !(par->gmodo & 0x40000)*/)
        {
            goto sal_rellena_hijo;
        }

		if (par->sqltoscreen)
		{
			char *buffers[100];
			nf = par->sqltoscreen->nf;

			for (i = 0;i < nf;i++)
			{
				buffers[i] = malloc(par->sqltoscreen->padrepfis[i]->f_lfich+1);
			}

			sql_select_ejecuta(par->pfi,par->sqltoscreen->padrepfis,par->sqltoscreen->nfiltros,par->sqltoscreen->filtros,par->sqltoscreen->origen,par->sqltoscreen->relacion,buffers,par,el_punto,NULL,NULL,nf,0,0);

			for (i = 0;i < nf;i++)
			{
				free(buffers[i]);
				if (RetroPar[el_punto] == (char *)par)
				{
					if (par->sqltoscreen && par->sqltoscreen->relacion[i].ofd == 1)
					{
						cierra_ind(par->sqltoscreen->padrepfis[i]);
					}
				}
			}

			goto sal_rellena_hijo;
		}

		if (!par->filtros )
		{
			goto sal_rellena_hijo;
		}		

		for (i = 0;i < par->nfiltros;i++)
		{
   	       if (par->filtros[i].mayus)
		   {
			   strmayus(par->filtros[i].valor);
		   }
		}

		pfipadre = par->padre->pfi;
		memcpy(salvaorignom,pfipadre->f_fichero,MAXPATH);

		{
			er = -1;
			if (par->nfiltros == 1 && par->filtros[0].tipo == 3 && !par->filtros[0].inver &&
																   pfipadre->f_campos[par->filtros[0].campo].c_tipo == CHARTIPO &&
																   pfipadre->f_campos[par->filtros[0].campo].c_qc > strlen(par->filtros[0].valor))
			{
				for (i = 0;i < pfipadre->f_nclaves;i++)
				{
					if (pfipadre->f_clave[i].k_ncampos == 1 && pfipadre->f_clave[i].k_campo[0] == (par->filtros[0].campo+1))
					{
						er = i;
						break;
					}
				}
				if (er == -1)
				{            
					for (i = 0;i < pfipadre->f_nclaves;i++)
					{
						if (pfipadre->f_clave[i].k_campo[0] == (par->filtros[0].campo+1))
						{
							er = i;
							break;
						}
					}
				}
				if (er != -1)
				{
					if (pfipadre->f_campos[ par->filtros[0].campo ].c_qc > 1)
					   porcontenido = 1;
					else
					   er = -1;
				}
			}
			if (er == -1) 
			{
				er = 0;
			}
		}

		if (par->padre->limi1 || par->padre->limi2  || par->padre->b_lim1 || par->padre->b_lim2)
		{
			if (porcontenido && er != par->padre->encl)
			{
				porcontenido = 0;
			}
			er = par->padre->encl;
			conlimites = 1;
		}
		
        ofd = pfipadre->f_fd;
        pfipadre->f_fd = -1;
        if (abre_ind(pfipadre,ISINOUT + ISMANULOCK) < 0)
        {
            pfipadre->f_fd = ofd;
            goto sal_rellena_hijo;
        }


repite_multiples_ficheros:		

		if (conlimites)
		{
  		  if (par->padre->b_lim1)
		  {
			  memcpy(pfipadre->f_buf,par->padre->b_lim1,pfipadre->f_lfich);		
			  if (apunta_ind(pfipadre,er+1,ISGTEQ,0) < 0)
			  {
				  apunta_ind(pfipadre,er+1,ISFIRST,0);
			  }		
			  laclavelim = er;
		  }	
		  else
			  apunta_ind(pfipadre,er+1,ISFIRST,0);
		}
		else
		{
			apunta_ind(pfipadre,er+1,ISFIRST,0);
		}
        fd = pfipadre->f_fd;
        pfipadre->f_fd = ofd;


        if (porcontenido)
        {
            ixx_pon_contenido(fd,par->filtros[0].valor,strlen(par->filtros[0].valor));
        }
        /*do
        {*/
		if (conlimites)
		{
			er = ixx_lee_indice(fd,pfipadre->f_buf,ISCURR,-1,1,0,1);
	        if (!er && !(par->padre->gmodo & 128) && ComparaClave(pfipadre,laclavelim,par->padre->b_lim1) < 0)
			{
				er = -1;
			}            
		}
		else
		{
            er = ixx_lee_indice(fd,pfipadre->f_buf,ISFIRST,-1,1,0,1);
		}
            /*
            if (er < 0 && par->padre->procesando)  no puede funcionar ... falat alago asi como multihebra!!!
            {
               if (pulsatecla() > -1)
               {
                  break;
               }
            }
            else
                break;
            } while(-1);
            */
		if (!er && conlimites)
		{
			if ((par->padre->gmodo & 128))
			{
				ofd = pfipadre->f_fd;
				pfipadre->f_fd = fd;
				if (salta_limites(par->padre))
				{
					er = -1;
				}
				pfipadre->f_fd = ofd;
				if (porcontenido)
				{
					ixx_pon_contenido(fd,par->filtros[0].valor,strlen(par->filtros[0].valor));
				}
			}
			else
			{
				if (ComparaClave(pfipadre,laclavelim,par->padre->b_lim2) > 0)
				{
					er = -1;
				}
			}
		}

        if ((!porcontenido || conlimites) && !er)
        {
            er = evaluador_simple_de_filtro(pfipadre,par->nfiltros,par->filtros,par->pfi);
        }

        if (!er && hay_dat_permiso(pfipadre,-2))
        {
            memcpy(par->pfi->f_buf,pfipadre->f_buf,par->pfi->f_lfich);
            graba_nuevo(par->pfi);
            par->registros = TotalRegistros(par);
            hay = 1;
        }
        contad = 0;
		tpulsa = 0;
        par->procesando = 1;
        while(er >= 0)
        {
            if (RetroPar[el_punto] != (char *)par) break;

          /*  do
            {*/
                er = ixx_lee_indice(fd,pfipadre->f_buf,ISNEXT,-1,1,0,1);
               /* if (er < 0 && par->padre->procesando)
                {
                   if (pulsatecla() > -1)
                   {
                      break;
                   }
                }
                else
                    break;
            } while(-1);
            */			
			if (!er && conlimites)
			{
				if ((par->padre->gmodo & 128))
				{
					ofd = pfipadre->f_fd;
					pfipadre->f_fd = fd;
					if (salta_limites(par->padre))
					{
						er = -1;
					}
					pfipadre->f_fd = ofd;
					if (porcontenido)
					{
						ixx_pon_contenido(fd,par->filtros[0].valor,strlen(par->filtros[0].valor));
					}
				}
				else
				{
					if (ComparaClave(pfipadre,laclavelim,par->padre->b_lim2) > 0)
					{
						er = -1;
					}
				}
			}

            if ((!porcontenido || conlimites) && !er)
            {
                er = evaluador_simple_de_filtro(pfipadre,par->nfiltros,par->filtros,par->pfi);
            }

            if (!er && hay_dat_permiso(pfipadre,-2))
            {
                memcpy(par->pfi->f_buf,pfipadre->f_buf,par->pfi->f_lfich);
                graba_nuevo(par->pfi);
                hay = 1;
            }

			if (tpulsa != tactual)
			{
				tpulsa = tactual;
				if ((latecla = pulsatecla()) > -1)
				{
					if (latecla == 802 || latecla == 806 || latecla == 807)
					{
						break;
					}
				}
				/* ver si el control actual es valido !!! */
				/* pues el pulsatecla permite si destruccion */
				if (RetroPar[el_punto] != (char *)par)
				{
					break;
				}
			}


			tactual = time(NULL);
            if (hay)
            {				
				if (!contad || contad < tactual || contad > (tactual+3))
				{
                    char tmp[256];
                    long modo = 1;
                    long tam = (sizeof(long)*4)+sizeof(int);
                    int id_grid = par->id_grid;

					if (par->cache)
					{
						for (i = 0;par->cache[i].id_reg;i++)
						{
							par->cache[i].id_reg = -1L;
						}
                    }

                    par->registros = TotalRegistros(par);

                    memcpy(tmp,&tam,sizeof(long));
                    memcpy(tmp+sizeof(long),&tam,sizeof(long));
                    memcpy(tmp+(sizeof(long)*2),&modo,sizeof(long));
                    memcpy(tmp+(sizeof(long)*3),&id_grid,sizeof(int));
                    memcpy(tmp+(sizeof(long)*3)+sizeof(int),(void *)&par->registros,sizeof(long));
                    (*DespachadorExterno)((int)DESPACHO_BINARIO,(int)3,tmp);
                    hay=0;
					contad = time(NULL) + 3;
                }                
            }
        }
        if (RetroPar[el_punto] == (char *)par) 
        {
			if (porcontenido)
			{
				ixx_fin_contenido(fd);
			}
			_cierra_ind(fd);
			fd = -1;
		}

		if (par->padre->multiples_ficheros) {
			if (!par->padre->multiples_ficheros[par->padre->indice_multiples_ficheros]) {
				par->padre->indice_multiples_ficheros = 0;
				memcpy(pfipadre->f_fichero,salvaorignom,MAXPATH);
				goto sal_rellena_hijo;
			}
			else {
				/*memset(pfipadre->f_fichero,0,MAXPATH);*/
				strcpy(quita_path(pfipadre->f_fichero),par->padre->multiples_ficheros[par->padre->indice_multiples_ficheros]);
				pfipadre->f_fd = -1;
				if (abre_ind(pfipadre,ISINOUT + ISMANULOCK) < 0)
				{
					crea_ind(pfipadre);
					cierra_ind(pfipadre);
					if (abre_ind(pfipadre,ISINOUT + ISMANULOCK) < 0)
					{

						/*aviso(0,"error");
						aviso(0,pfipadre->f_fichero);*/
						pfipadre->f_fd = ofd;
						goto sal_rellena_hijo;
					}
				}
				fd = pfipadre->f_fd;
				/*pfipadre->f_fd = ofd;*/
				par->padre->indice_multiples_ficheros++;
				if (conlimites)
					er = par->padre->encl;
				else 
					er = 0;
			}
			goto repite_multiples_ficheros;
		}
		
sal_rellena_hijo:
		

        if (RetroPar[el_punto] != (char *)par) 
        {
            registro = 0;			
        }
        else
        {
			/*
           if (hay)
           {
                char tmp[256];
                long modo = 1;
                long tam = (sizeof(long)*4)+sizeof(int);
                int id_grid = par->id_grid;
				if (par->cache)
				{
					for (i = 0;par->cache[i].id_reg;i++)
					{
						par->cache[i].id_reg = -1L;
					}
                }
                par->registros = TotalRegistros(par);

                memcpy(tmp,&tam,sizeof(long));
                memcpy(tmp+sizeof(long),&tam,sizeof(long));
                memcpy(tmp+(sizeof(long)*2),&modo,sizeof(long));
                memcpy(tmp+(sizeof(long)*3),&id_grid,sizeof(int));
                memcpy(tmp+(sizeof(long)*3)+sizeof(int),(void *)&par->registros,sizeof(long));
                (*DespachadorExterno)((int)DESPACHO_BINARIO,(int)3,tmp);
                hay=0;
				pulsatecla();
            }
			*/

			if (par->cache)
			{
				for (i = 0;par->cache[i].id_reg;i++)
				{
					par->cache[i].id_reg = -1L;
				}
            }
            par->registros = TotalRegistros(par);
            registro = par->registros;
            par->procesando = 0;			
        }
        memcpy(bloque,&registro,sizeof(long));        
    }
    else if (operacion == 6)
    {
        memcpy(&registro,bloque,sizeof(long));

        if (par->padre)
        {
            int ofs;
            int lll;
			int ok = 0;

            i = ixx_inicio_lectura_multiple(par->pfi->f_fd);
            if (i == 1)
            {
                par->atencion = 1; /* con refrescar cuando no hay mas remedio es suficiente */
                if (par->cache)
                {
                    for (i = 0;par->cache[i].id_reg;i++)
                    {
                        par->cache[i].id_reg = -1L;
                    }
                }
            }
            isrecnum = registro;
            pon_registro_actual(par->pfi,registro );
            apunta_ind(par->pfi,0,ISEQUAL,0);
            apunta_ind(par->pfi,par->encl+1,ISCURR,0);
            lee_reg(par->pfi,ISCURR);
            ixx_fin_lectura_multiple(par->pfi->f_fd);


			if (par->sqltoscreen && par->sqltoscreen->origen)
			{
				struct _sql_origen *origen;
				origen = par->sqltoscreen->origen;
				for (i = par->pfi->f_ncampos-1;i >= 0;i--)
				{
					if (origen[i].ipadre == -1 && origen[i].icampo == -3)
					{
						memcpy((void *)&registro,par->pfi->f_buf+par->pfi->f_campos[i].c_offset,sizeof(long));
						ok = 1;
						break;
					}
				}
			}


			if (!ok)
			{

				/*** buscar clave primaria en el padre */
				/* de momento uno a uno ... luego hay que poner un 'mapa' */
				for (i = 0;i < par->pfi->f_clave[0].k_ncampos;i++)
				{
					lll = par->pfi->f_clave[0].k_campo[i] - 1;
					ofs = par->pfi->f_campos[lll].c_offset;
					lll = par->pfi->f_campos[lll].c_long;
					memcpy(par->padre->pfi->f_buf+ofs,par->pfi->f_buf+ofs,lll);
				}
			}
            i = ixx_inicio_lectura_multiple(par->padre->pfi->f_fd);
            if (i == 1)
            {
                par->padre->atencion = 1;
                if (par->padre->cache)
                {
                    for (i = 0;par->padre->cache[i].id_reg;i++)
                    {
                        par->padre->cache[i].id_reg = -1L;
                    }
                }
            }
			if (!ok)
			{
				apunta_ind(par->padre->pfi,1,ISEQUAL,0);            
				registro = isrecnum;
			}
			else
			{				
				pon_registro_actual(par->padre->pfi,registro);
				apunta_ind(par->padre->pfi,0,ISEQUAL,0);
				registro = isrecnum;				
			}            
            apunta_ind(par->padre->pfi,par->padre->encl+1,ISCURR,0);
            ixx_fin_lectura_multiple(par->padre->pfi->f_fd);            
        }
        memcpy(bloque,&registro,sizeof(long));
    }
    else if (operacion == 7) /* filtro simple */
    {
        struct grid_filtro *filtros = NULL;
        char titulo[512];
        int nfiltros = 0;
        int *cols = NULL;
		struct grid_filtro filtro;


        memcpy(&i,bloque,sizeof(long));
        bloque += sizeof(long);        

        if (i > -1)
        {            
            strcpy(titulo,"Consulta ");
            strcat(titulo,par->pfi->f_titulo);
			
			if ((par->gmodo & 256) || (par->gmodo & 128))
			{
				memset(&filtro,0,sizeof(filtro));				
				filtro.tipo = 4 | 0x10;
				filtro.campo = par->curcol;
				LanZaHiJo(par,titulo,1,&filtro,NULL,NULL,0,0,0);
			}
			else			
			{
				j = par->encl;
				par->encl = i;				
				LanZaHiJo(par,titulo,0,NULL,NULL,NULL,0,0,0);
				par->encl = j;
			}
        }
        else
        {
            strcpy(titulo,"Filtro ");
            strcat(titulo,bloque);
            bloque += (strlen(bloque)+1);
            memcpy(&i,bloque,sizeof(int));
            bloque += sizeof(int);
            cols = (int *)bloque;
            bloque += (sizeof(int)*i);
            memcpy(&nfiltros,bloque,sizeof(int));
            bloque += sizeof(int);
            if (nfiltros)
            {
                filtros = (struct grid_filtro *)bloque;
            }
			else
			{
				if ((par->gmodo & 256) || (par->gmodo & 128))
				{
					filtros = &filtro;
					memset(&filtro,0,sizeof(filtro));				
					filtro.tipo = 4 | 0x10;
					filtro.campo = par->curcol;				
					nfiltros = 1;
				}
			}
            LanZaHiJo(par,titulo,nfiltros,filtros,i,cols,0,0,0);
        }        
    }    
    else if (operacion == 8) /* filtro complex */
    {
		FI_DEF *pfi;
		FI_DEF *padrepfis[100];
		int padrecargado[100];
		int nfiltros[100];
		int n,k,ii,nn,pariente;
		int nf;
		int flag;		
		int campo;
		int orden[MAXCAMPOSPORCLAVE];
		int sentido[MAXCAMPOSPORCLAVE];
		int norden = 0;
		char titulo[512];
		struct grid_filtro *filtros[100];
	    struct _sql_origen *origen;
	    struct _sql_relacion *relacion;
		int agrupacion = 0;
		int totales = 0;
		int camptot[300];
		int laclave;
		int campototal = 0;	

        memcpy(&i,bloque,sizeof(long));
        bloque += sizeof(long);        

		if (i == -1)
		{
			/*
			aviso(0,"query");
			*/

            /*strcpy(titulo,"Query ");*/
			strcpy(titulo,"");
            strcat(titulo,bloque);
            bloque += (strlen(bloque)+1);

			pfi = (FI_DEF *)malloc(sizeof(FI_DEF));
			memset((char *)pfi,0,sizeof(FI_DEF));
			pfi->f_fd = -1;
			pfi->f_ipfi = -1;
			pfi->f_titulo = strmalloc(1);
			pfi->f_titulo[0] = 0;
			//strcpy(pfi->f_PathDef,par->pfi->f_PathDef); //quita_path(par->pfi->f_PathDef)
			//strcpy(pfi->f_PathDef,"x+mxxm+x"); 
			pfi->f_PathDef[0] = 7;
			pfi->f_PathDef[1] = 7;
			pfi->f_PathDef[2] = 0;

            memcpy(&nf,bloque,sizeof(int));
            bloque += sizeof(int);

			bloque += 128; /* es el padre */
			bloque += sizeof(int);
			
			nfiltros[0] = 0;			
			padrepfis[0] = par->pfi;			
			padrecargado[0] = 0;		


			relacion = (struct _sql_relacion *)malloc(sizeof(struct _sql_relacion)*nf);
			memset((char *)relacion,0,sizeof(struct _sql_relacion)*nf);

			for (i = 1;i < nf;i++)
			{
				nfiltros[i] = 0;
				padrecargado[i] = 0;

				/*
				aviso(0,bloque);
				*/

				j = -1;/*ind_rel(quita_path(bloque));*/
				if (j >= 0)
				{
					padrepfis[i] = v_mpfi(j);					
				}
				else
				{
					padrepfis[i] = NULL;
					if (lee_defn(bloque,&padrepfis[i]) >= 0)
					{
						padrecargado[i] = 1;
						cambia_path_fich(padrepfis[i]->f_fichero);
					}
				}
				bloque += 128;
				memcpy((void *)&flag,bloque,sizeof(int));
				bloque += sizeof(int);
				if ((flag & 1))
				{
					/*
					aviso(0,"flag 1");
					*/

					memcpy(&pariente,bloque,sizeof(int));
					bloque += sizeof(int);

					nn = relacion[pariente].nrel;
					if (nn >= 50)
					{
						rw_error(991);
					}
					relacion[pariente].nrel++;

					relacion[pariente].frel[nn] = i;

					memcpy(&(relacion[pariente].ncrel[nn]),bloque,sizeof(int));
					bloque += sizeof(int);
					
					for (j = 0;j < relacion[pariente].ncrel[nn];j++)
					{
						memcpy(&(relacion[pariente].rel[nn][j]),bloque,sizeof(int));
						bloque += sizeof(int);
						memcpy(&(relacion[pariente].relh[nn][j]),bloque,sizeof(int));
						bloque += sizeof(int);
					}
					/* analisis de clave */	
					for (k = 0;k < padrepfis[i]->f_nclaves;k++)
					{
						if (relacion[pariente].ncrel[nn] > padrepfis[i]->f_clave[k].k_ncampos)
							continue;
						for (j = 0;j < relacion[pariente].ncrel[nn];j++)
						{
							if ((relacion[pariente].relh[nn][j]+1) != padrepfis[i]->f_clave[k].k_campo[j])
							{
								break;
							}
						}
						if (j >= relacion[pariente].ncrel[nn])
						{
							relacion[pariente].hrel[nn] = k+1;
							break;
						}
					}
				}
			}


            memcpy(&n,bloque,sizeof(int));
            bloque += sizeof(int);

			j = n + 3;
			origen = (struct _sql_origen *)malloc(sizeof(struct _sql_origen)*j);
			
			nn = 0;
			for (i = 0;i < n;i++)
			{
				memcpy((void *)&flag,bloque,sizeof(int));
				bloque += sizeof(int);
				memcpy((void *)&j,bloque,sizeof(int));
				bloque += sizeof(int);
				memcpy((void *)&campo,bloque,sizeof(int));				
				bloque += sizeof(int);

				/*
				{
					char tmppp[128];
					sprintf(tmppp,"->flag %d",flag);
					aviso(0,tmppp);
				}
				*/
				
				if (!(flag & 2))
				{
					if ((((struct grid_filtro *)bloque)->tipo & 0x10))
					{
						orden[norden] = i+1;
						if ((((struct grid_filtro *)bloque)->tipo & 0x20))
						{
							sentido[norden] = -1;
						}
						else
						{
							sentido[norden] = 1;
						}
						if ((((struct grid_filtro *)bloque)->tipo & 0x40))
						{
							agrupacion = 1;
							/*
							aviso(0,"agrupacion");
							*/
						}
						norden++;
						/*
						aviso(0,"orden");
						*/
					}
					
					if (!(flag & 4) || (((struct grid_filtro *)bloque)->tipo & 0x40))
					{
						k = nfiltros[j];
						if (!k)
						{
							filtros[j] = (struct grid_filtro *)malloc(sizeof(struct grid_filtro));
						}
						else
						{
							filtros[j] = (struct grid_filtro *)realloc((void *)filtros[j],sizeof(struct grid_filtro)*(k+1));
						}
						filtros[j][k] = *((struct grid_filtro *)bloque);					
						nfiltros[j]++;
						nn++;

					    if ((((struct grid_filtro *)bloque)->tipo & 0xf0000))
						{
							int tipo;
							tipo = (((struct grid_filtro *)bloque)->tipo & 0xf0000);
							tipo >>= 16;
							tipo--;
							tipo <<= 16;
							camptot[totales] = i | tipo;
							totales++;				
						}
					}					
					bloque += sizeof(struct grid_filtro);
				}

				origen[i].ipadre = j;
				origen[i].icampo = campo;

				inserta_campo(pfi,i,padrepfis[j]->f_campos[campo].c_lista,					
									padrepfis[j]->f_campos[campo].c_nombre,
									padrepfis[j]->f_campos[campo].c_qc,
									padrepfis[j]->f_campos[campo].c_minimo,
									padrepfis[j]->f_campos[campo].c_maximo,
									padrepfis[j]->f_campos[campo].c_mante.c_defecto);
				if ((flag & 1))
				{
					pfi->f_campos[pfi->f_ncampos-1].c_mante.c_visual = 0;
				}
			}	
			nn++;
			k = nfiltros[0];
			if (!k)
			{
				filtros[0] = (struct grid_filtro *)malloc(sizeof(struct grid_filtro)*nn);
			}
			else
			{
				filtros[0] = (struct grid_filtro *)realloc((void *)filtros[0],sizeof(struct grid_filtro)*nn);
			}			
			for (nn = 1;nn < nf;nn++)
			{
				if (nfiltros[nn])
				{
					memcpy((char *)(filtros[0]+k),(char *)filtros[nn],sizeof(struct grid_filtro)*nfiltros[nn]);
					free((void *)filtros[nn]);
					filtros[nn] = filtros[0]+k;
					k += nfiltros[nn];
				}
			}
			
			if (totales && (!agrupacion || norden > 1))
			{				
				inserta_campo(pfi,i,"T",
									"EsTotal",
									1,
									"",
									"",
									"");
				origen[i].ipadre = -1;
				origen[i].icampo = -2;
				i++;
				campototal = i;
			}

			inserta_campo(pfi,i,"0",
								"VinculoPadre",
								-2,
								"",
								"",
								"");
			origen[i].ipadre = -1;
			origen[i].icampo = -3;
			i++;

			/* contador de resultados = la clave primaria si no hay agrupacion */
			inserta_campo(pfi,i,"_QUERY_REG",
								"Registro",
								0,
								"0",
								"999999999999",
								"0");
			origen[i].ipadre = -1;
			origen[i].icampo = -1;						
			pfi->f_nclaves = 1;
			pfi->f_clave = (KEY_DEF *)reserva_malloc(sizeof(KEY_DEF)*2,3); /* memoria para dos claves */
			laclave = 0;
			if (!agrupacion)
			{
				pfi->f_clave[0].k_ncampos = 1;
				pfi->f_clave[0].k_fflag = 0;
				pfi->f_clave[0].k_campo = (short *)malloc(sizeof(short) * 2);
				pfi->f_clave[0].k_orden = (short *)malloc(sizeof(short) * 2);
				pfi->f_clave[0].k_orden[0] = 1;
				pfi->f_clave[0].k_campo[0] = i+1;
				laclave++;
			}
			if (norden)
			{
				pfi->f_nclaves = laclave+1;
				pfi->f_clave[laclave].k_ncampos = norden;				
				pfi->f_clave[laclave].k_fflag = 0;
				pfi->f_clave[laclave].k_campo = (short *)malloc(sizeof(short) * (norden+1));
				pfi->f_clave[laclave].k_orden = (short *)malloc(sizeof(short) * (norden+1));
				for (i = 0;i < norden;i++)
				{
					pfi->f_clave[laclave].k_campo[i] = orden[i];
					pfi->f_clave[laclave].k_orden[i] = sentido[i];
				}
				if (campototal)
				{
					pfi->f_clave[laclave].k_ncampos++;
					pfi->f_clave[laclave].k_campo[norden] = campototal;
					pfi->f_clave[laclave].k_orden[norden] = 0;
				}
			}


			for (i=pfi->f_nclaves;i>0;i--) 
			{
				for (j=pfi->f_clave[i-1].k_ncampos;j>0;j--) 
				{
					if((k = pfi->f_clave[i-1].k_campo[j-1]) <= pfi->f_ncampos && k)
						pfi->f_campos[k-1].c_key = i;   
				}
			}


			/* Relaciones nativas ... no usadas solo usamos las proporcionadas ...
			relacion = (struct _sql_relacion *)malloc(sizeof(struct _sql_relacion)*nf);
			memset((char *)relacion,0,sizeof(struct _sql_relacion)*nf);
			for (i = 0;i < nf;i++)
			{
				for (k = 0;k < padrepfis[i]->f_nrelaci;k++)
				{
					for (j = 1;j < nf;j++)
					{
						if (j != i)
						{
							if (!strcmp(quita_path(padrepfis[i]->f_relaci[k].r_fichero),quita_path(padrepfis[j]->f_PathDef)))
							{
								break;
							}
						}
					}
					if (j < nf)
					{
						ii = relacion[i].nrel;
						relacion[i].nrel++;
						relacion[i].frel[ii] = j;								
						relacion[i].hrel[ii] = 1;
						for (j = 0;j < MAXCAMPOSPORCLAVE;j++)
						{
						   relacion[i].rel[ii][j] = padrepfis[i]->f_relaci[k].r_campo[j];
						   if (!relacion[i].rel[ii][j])
							   break;
						   relacion[i].rel[ii][j]--;
						   relacion[i].ncrel[ii]++;
						}							 
					}
				}
			}
			*/
			if (1/*agrupacion*/)
			{
				padrepfis[0] = NULL;
				/*if (par->pfi->f_PathDef[0] == 7 && par->pfi->f_PathDef[1] == 7 && !par->pfi->f_PathDef[2])*/
				{
					copia_defn(&padrepfis[0],par->pfi);
					padrecargado[0] = 1;
					cambia_path_fich(padrepfis[0]->f_fichero);
				}
				/*else
				{
					if (lee_defn(par->pfi->f_PathDef,&padrepfis[0]) >= 0)
					{
						padrecargado[0] = 1;
						cambia_path_fich(padrepfis[0]->f_fichero);
					}
				}
				*/
			}

			{
				struct _sql_toscreen *sqltoscreen;

				sqltoscreen = (struct _sql_toscreen *)malloc(sizeof(struct _sql_toscreen));
				memset((void *)sqltoscreen,0,sizeof(struct _sql_toscreen));
				sqltoscreen->pfi = pfi;
				memcpy(sqltoscreen->padrepfis,padrepfis,sizeof(padrepfis));
				memcpy(sqltoscreen->padrecargado,padrecargado,sizeof(padrecargado));
				memcpy(sqltoscreen->nfiltros,nfiltros,sizeof(nfiltros));
				sqltoscreen->nf = nf;
				memcpy(sqltoscreen->filtros,filtros,sizeof(filtros));
				sqltoscreen->origen = origen;
				sqltoscreen->relacion = relacion;
				sqltoscreen->campototal = campototal;
				sqltoscreen->totales = totales;
				memcpy(&sqltoscreen->camptot,&camptot,sizeof(camptot));

				LanZaHiJo(par,titulo,0,(struct grid_filtro *)sqltoscreen,0,0,0,0,1);
			}		
		}		
	}
    else if (operacion == 10) /* seleccion */
    {
        EntraLineaGrid(bloque,par,el_punto);
    }
    else if (operacion == 11)
    {
        long inicial,registros;
        /* refresca total */        
        if (par->cache)
        {
            for (i = 0;par->cache[i].id_reg;i++)
            {
                par->cache[i].id_reg = -1L;
            }
        }
        if (lee_reg(par->pfi,ISCURR) < 0)
        {
          inicial = 0;
        }
        else
        {			
          inicial = IxxNumeroPosicion;
        }		        

        if (par->hijos)
        {
              int *hijos;
              hijos = par->hijos;
              while(*hijos > -2)
              {
                  BorraHijoGrid(*hijos,0);/****/
                  hijos++;
              }
              free((char *)par->hijos);
              par->hijos = NULL;
        }

        InicializaLimite(par);

        registros = TotalRegistros(par);		
		if (inicial < par->offset)
		{
			inicial = par->offset;

		}
		else if (inicial && inicial >= (par->offset+registros))
		{			 
			inicial = par->offset+registros-1;
		}

		if (lee_por_posicion(par->pfi,par->encl+1,inicial,par->pfi->f_buf,0,0) >= 0)
		{
			par->regsel = isrecnum;
			/*
			if (par->regsel < 1 || par->regsel > 1000000) {
				par->regsel = 0;
			}
			*/
			if (par->mapa_de_ordenacion)
			{
				int j;
				for (j = 0;j < par->nmapa_de_ordenacion;j++)
				{
					if (par->regsel == par->mapa_de_ordenacion[j])
					{
						inicial = (j/*-par->offset*/)+1;					
						break;
					}
				}
			}
			else
			{
				inicial = RegSegunMapaAsignacion(par,inicial - par->offset)+1;
			}
			par->reg = inicial;
			par->regcur = inicial;
		}


	    if ((par->gmodo & 0x100000) && par->totales)
		{
			  int i,ok,j;
			  extern double ld_doble();

			  for (j = 0;j < par->totales[0];j++)
			  {
				  if ((par->totales[j+1]&0xf0000) == 0x10000)
				  {
					  par->t_totales[j] = MINDOUBLE;
				  }
				  else if ((par->totales[j+1]&0xf0000) == 0x20000)
				  {
					  par->t_totales[j] = MAXDOUBLE;
				  }
				  else
					  par->t_totales[j] = 0.0;
			  }


			  for (i = 0;i < par->registros;i++)
			  {
				  no_corrup_error = 1;
				  ok = grid_lee_por_posicion(par,par->encl+1,par->offset+i,par->pfi->f_buf,0,0,NULL,NULL);
				  no_corrup_error = 0;
				  if (ok < 0)
				  {
					  if (ixxstatus == IXXECORRUP)
					  {
						  ixx_reconstruye(par->pfi->f_fd);
						  i--;
						  continue;
					  }
					  break;
				  }
				  for (j = 0;j < par->totales[0];j++)
				  {
					  if ((par->totales[j+1]&0xf0000) == 0x10000)
					  {
						  if (par->t_totales[j] < ld_doble((par->totales[j+1]&0xffff),par->pfi))
							  par->t_totales[j] = ld_doble((par->totales[j+1]&0xffff),par->pfi);				  
					  }
					  else if ((par->totales[j+1]&0xf0000) == 0x20000)
					  {
						  if (par->t_totales[j] > ld_doble((par->totales[j+1]&0xffff),par->pfi))
							  par->t_totales[j] = ld_doble((par->totales[j+1]&0xffff),par->pfi);				  
					  }
					  else
						  par->t_totales[j] += ld_doble((par->totales[j+1]&0xffff),par->pfi);
				  }
			  }
		}

        par->reg = inicial;
        par->regcur = inicial;
        memcpy(bloque,&registros,sizeof(long));
        bloque += sizeof(long);
        memcpy(bloque,&par->regcur,sizeof(long));
        bloque += sizeof(long);
    }
    else if (operacion == 12) /* borrar */
    {
        BorraLineaGrid(bloque,par,el_punto);
    }
    else if (operacion == 13) /* actualizar columnas visibles */
    {
        memcpy(&par->ncampos,bloque,sizeof(int));
        bloque += sizeof(int);
        memcpy(par->colcampos,bloque,sizeof(int)*par->ncampos);
        for (i = par->ncampos;i < par->InfoCampos->n_campos;i++)
        {
            par->colcampos[i] = -1;
        }
    }
    else 
    if (operacion == 14) /* Grabar Filtro */
	{
		char tmpx[512];
		int l,file,esindice = 0;

		l = (strlen(bloque)+1);		
		if ((par->gmodo & 0x2000000))
		{
			strcpy(tmpx,par->autofiltro+1);
		}
		else
		{
			sprintf(tmpx,"%s/%03d/%s.flt",rw_directorio("dev/usr"),user_who,bloque);		
		}
		if (!strcmp(bloque,"_INDICE_"))
		{
			bloque += l;			
			/* creamos un indice del anterior con los que no me pertenecen */
			file = rw_open_(tmpx,O_RDONLY | O_BINARY);
		    if (file > -1)
			{			
					char *p0,*p1,*p2,*pg;
					int f;
					unsigned char cc;
					char nomtmp[512];
					char tmpx2[512];
					char *bloque2;
					char *bloque3;

					l = rw_lseek_(file,0L,SEEK_END);
				    rw_lseek_(file,0L,SEEK_SET);
					bloque3 = malloc(l+200+strlen(bloque));
				    rw_read_(file,bloque3,l);
					bloque3[l] = 0;
					/* filtrar por def	*/

					bloque2 = malloc(l+200);
					memset(bloque2,0,l+200);

					pg = bloque2;

					p0 = bloque3;
					p1 = bloque3;
					esindice = 1;
					while(esindice)
					{
						if (*p0 == 1 || *p0 == 0)
						{
							if (*p0 == 0)
								esindice = 0;
							*p0 = 0;
							sprintf(tmpx2,"%s/%03d/%s.flt",rw_directorio("dev/usr"),user_who,p1);
							p2 = NULL;
							f = rw_open_(tmpx2,O_RDONLY | O_BINARY);
							if (f >= 0)
							{
								while(rw_read_(f,&cc,1) == 1)
								{
									if (cc == 26)
										p2 = nomtmp;
									else if (cc == 2)
										break;
									else if (p2)
									{
										*p2 = cc;
										p2++;
									}
								}
								rw_close_(f);
								if (p2)
								{
									*p2 = 0;
									/* al reves que en la lectura */
									if (!strcmp(quita_path(nomtmp),quita_path(par->pfi->f_PathDef)))
									{
										p2 = NULL;
									}
								}
							}
							if (p2)
							{
								strcpy(pg,p1);
								pg += strlen(p1);
								*pg = 1;
								pg++;
							}
							if (esindice)
							{
								*p0 = 1;
								p1 = p0 + 1;
							}
							else
								break;
						}
						p0++;
					}
					if (pg > bloque2)
					{
						pg--;
						*pg = 0;
					}
					memcpy(bloque3,bloque2,l);
					free(bloque2);

					rw_close_(file);				
					
					if (*bloque)
					{
						l = strlen(bloque3);
						if (l)
						{
							bloque3[l] = 1;
							l++;
						}
						strcpy(bloque3+l,bloque);
					}
					l = strlen(bloque3);
					if (!l)
						unlink(tmpx);
					else
					{
						crea_dir(tmpx);
						file = rw_open_(tmpx,O_RDWR | O_BINARY | O_CREAT | O_TRUNC,S_IREAD | S_IWRITE);
						if (file > -1)
						{
							rw_write_(file,bloque3,l+1);
							rw_close_(file);
						}
					}
					esindice = 1; 
			}

		}
		else
			bloque += l;

		if (!esindice)
		{			
			l = strlen(bloque);
			if (!l)
			{
				unlink(tmpx);
			}
			else
			{
				crea_dir(tmpx);
				file = rw_open_(tmpx,O_RDWR | O_BINARY | O_CREAT | O_TRUNC,S_IREAD | S_IWRITE);
				if (file > -1)
				{
					rw_write_(file,bloque,l+1);
					rw_close_(file);
				}
			}
		}
	}
    else if (operacion == 15) /* Size Filtros */
	{
		char tmpx[512];
		int l,file;		
		l = (strlen(bloque)+1);
		if ((par->gmodo & 0x2000000))
		{
			strcpy(tmpx,par->autofiltro+1);
		}
		else
		{
			sprintf(tmpx,"%s/%03d/%s.flt",rw_directorio("dev/usr"),user_who,bloque);
		}
		crea_dir(tmpx);
		bloque += l;
		l = 0;
		file = rw_open_(tmpx,O_RDONLY | O_BINARY);
		if (file > -1)
		{
			l = rw_lseek_(file,0L,SEEK_END);
			rw_close_(file);
		}
		memcpy(bloque,(char *)&l,sizeof(int));
	}
    else 
    if (operacion == 16) /* Leer Filtros */
	{
		char tmpx[512];
		int l,file;
		int esindice = 0;
		l = (strlen(bloque)+1);		
		if ((par->gmodo & 0x2000000))
		{
			strcpy(tmpx,par->autofiltro+1);
		}
		else
		{
		    sprintf(tmpx,"%s/%03d/%s.flt",rw_directorio("dev/usr"),user_who,bloque);
		}
		if (!strcmp(bloque,"_INDICE_"))
		{
			esindice = 1;
		}
		crea_dir(tmpx);
		bloque += l;
		memcpy((char *)&l,bloque,sizeof(int));
		file = rw_open_(tmpx,O_RDONLY | O_BINARY);
		if (file > -1)
		{			
			if (l == rw_lseek_(file,0L,SEEK_END))
			{				
				rw_lseek_(file,0L,SEEK_SET);
				rw_read_(file,bloque,l);
				if (esindice)
				{
					char *p0,*p1,*p2,*pg;
					int f;
					unsigned char cc;
					char nomtmp[512];
					char *bloque2;
					/* filtrar por def 
												 *
												 *
					no vamos a ningun lado asi ****
											   ***
					*/

					bloque2 = malloc(l+1);
					memset(bloque2,0,l+1);

					pg = bloque2;

					p0 = bloque;
					p1 = bloque;
					while(esindice)
					{
						if (*p0 == 1 || *p0 == 0)
						{
							if (*p0 == 0)
								esindice = 0;
							*p0 = 0;
							sprintf(tmpx,"%s/%03d/%s.flt",rw_directorio("dev/usr"),user_who,p1);
							p2 = NULL;
							f = rw_open_(tmpx,O_RDONLY | O_BINARY);
							if (f >= 0)
							{
								while(rw_read_(f,&cc,1) == 1)
								{
									if (cc == 26)
										p2 = nomtmp;
									else if (cc == 2)
										break;
									else if (p2)
									{
										*p2 = cc;
										p2++;
									}
								}
								rw_close_(f);
								if (p2)
								{
									*p2 = 0;
									if (strcmp(quita_path(nomtmp),quita_path(par->pfi->f_PathDef)))
									{
										p2 = NULL;
									}
								}
							}
							if (p2)
							{
								strcpy(pg,p1);
								pg += strlen(p1);
								*pg = 1;
								pg++;
							}
							if (esindice)
							{
								*p0 = 1;
								p1 = p0 + 1;
							}
							else
								break;
						}
						p0++;
					}
					if (pg > bloque2)
					{
						pg--;
						*pg = 0;
					}
					memcpy(bloque,bloque2,l);
					free(bloque2);
				}
			}
			else
			{
				memset(bloque,0,2);
			}
			rw_close_(file);
		}
		else
		{
			memset(bloque,0,2);
		}
	}
    else if (operacion == 17) /* aviso de foco */
	{
        if (par->hook)
        {            
            sprintf(v_memo(5004)->alfabetica,"%d",2);
		    vcf = v_memo(5011)->numerica;  
			v_memo(5011)->numerica = (double)par->ipfi;
            p_ejecuta(par->hook,O_NOR,0,25);
			v_memo(5011)->numerica = vcf;
        }
	}
    else if (operacion == 18) /* aviso de perdida de foco */
	{
        if (par->hook)
        {            
            sprintf(v_memo(5004)->alfabetica,"%d",3);
		    vcf = v_memo(5011)->numerica;  
			v_memo(5011)->numerica = (double)par->ipfi;
            p_ejecuta(par->hook,O_NOR,0,26);
			v_memo(5011)->numerica = vcf;
        }
	}
    else if (operacion == 19) /* Doble click */
	{
		*bloque = 0;
        if (par->hook)
        {
			double antes = v_memo(5000)->numerica;
			memcpy((char *)&i,bloque+1,sizeof(int));
			v_memo(5000)->numerica = (double)i;
            sprintf(v_memo(5004)->alfabetica,"%d",4);	         
		    vcf = v_memo(5011)->numerica;  
			v_memo(5011)->numerica = (double)par->ipfi;
			ignorar_aviso_64++;
            p_ejecuta(par->hook,O_NOR,0,27);
			ignorar_aviso_64--;
			v_memo(5011)->numerica = vcf;
			v_memo(5000)->numerica = antes;
			strcpy(bloque,v_memo(5004)->alfabetica);
        }
	}
	else if (operacion == 20)
	{
		char tmp[256];
		int rel,tammem,cargado;
		FI_DEF *padre = NULL;
		FI_DEF *fi;
		int tambuffer;
		int prel = -1;
		int ncamrel;
		int lineas;
		int *pndef = (int *)bloque;
		tambuffer = *pndef;
		*pndef = 0;
		bloque += sizeof(int);		
		if (!*bloque)
		{
			padre = par->pfi;
		}
		else
		{			
			prel = ind_rel(quita_path(bloque));
			if (prel >= 0)
			{
				_salva_datos_lock(prel);
				padre = v_mpfi(prel);
			}
			else
			{
				padre = NULL;
				if (lee_defn(bloque,&padre) < 0)
				{
					padre = NULL;
				}
				else
					cambia_path_fich(padre->f_fichero);
			}
		}
		if (padre)
		{
			for (i = 0;i < padre->f_nrelaci;i++)
			{
				fi = NULL;
				cargado = 0;
				rel = ind_rel(quita_path(padre->f_relaci[i].r_fichero));
				if (rel < 0)
				{
					/* cargarlo --- lee_defn */				
					cambia_path_def(padre->f_relaci[i].r_fichero,0,tmp);
					if (lee_defn(tmp,&fi) >= 0)
					{
						cambia_path_fich(fi->f_fichero);
						cargado = 1;
					}
				}
				else
				{
					_salva_datos_lock(rel);
					fi = v_mpfi(rel);
				}
				if (fi)
				{
					/* es una relacion aceptada? */
					if (padre->f_relaci[i].r_campo[0] < 0)
					{
						ncamrel = padre->f_clave[0].k_ncampos;
						lineas = 1;
					}
					else
					{
						lineas = 0;
						for (ncamrel = 0;ncamrel< fi->f_clave[0].k_ncampos;ncamrel++)
						{
							if (!padre->f_relaci[i].r_campo[ncamrel])
								break;
						}
						if (ncamrel != fi->f_clave[0].k_ncampos)
						{
							lineas = 2;
						}
					}
					if (!lineas)
					{
						(*pndef)++;
						tammem = RellenaInfoDef(NULL,fi,64,1,NULL,NULL);
						*((int *)bloque) = tammem;
						bloque += sizeof(int);

						*((int *)bloque) = ncamrel;
						bloque += sizeof(int);
						/* padre */
						for (j = 0;j < ncamrel;j++)
						{
							if (lineas)
							{
								*((int *)bloque) = padre->f_clave[0].k_campo[j] - 1;
							}
							else
							{
								*((int *)bloque) = padre->f_relaci[i].r_campo[j] - 1;
							}
							bloque += sizeof(int);
						}
						/* hijo */
						for (j = 0;j < ncamrel;j++)
						{
							*((int *)bloque) = fi->f_clave[0].k_campo[j] - 1;
							bloque += sizeof(int);
						}
						strcpy(bloque,fi->f_PathDef); //quita_path(fi->f_PathDef)
						bloque += (strlen(fi->f_PathDef)+1);
						strcpy(bloque,fi->f_titulo);
						bloque += (strlen(fi->f_titulo)+1);
						*bloque = 0;
						bloque++;
					}
					if (cargado)
					{
						free_defpri(fi);
					}
					else
					{
						_salva_datos_unlock(rel);
					}
				}
			}
			if (padre != par->pfi)
			{
				if (prel > -1)
				{
					_salva_datos_unlock(prel);
				}
				else
				{
					free_defpri(padre);
				}
			}
		}
		/* ya que no hemos previsto indicar el tamaño de retorno variable si dejamos a 0
		   la parte no usada la compresion del gzip compensara algo esta deficiencia */
		memset((void *)bloque,0,tambuffer-(int)((char *)bloque-(char *)pndef)); 		    
	}
	else if (operacion == 21)
	{
		FI_DEF *fi = NULL;
		int rel = ind_rel(quita_path(bloque));
		if (rel < 0)
		{
			lee_defn(bloque,&fi);
		}
		else
		{
			_salva_datos_lock(rel);
			fi = v_mpfi(rel);			
		}
		if (fi)
		{
			if (rel < 0)
				cambia_path_fich(fi);
			*bloque = 1;
			bloque++;
			RellenaInfoDef((struct Info_Entra_Campos *)bloque,fi,64,1,NULL,NULL);
			if (rel < 0)
				free_defpri(fi);
			else
				_salva_datos_unlock(rel);
		}
		else
		{
			*bloque = 0;
			bloque++;
		}
	}
	else if (operacion == 22)
	{
		int tammem = 0;		
		FI_DEF *fi = NULL;
		int rel = ind_rel(quita_path(bloque));
		if (rel < 0)
		{
			lee_defn(bloque,&fi);
		}
		else
		{
			_salva_datos_lock(rel);
			fi = v_mpfi(rel);			
		}
		if (fi)
		{		
			cambia_path_fich(fi);
			*bloque = 1;			
			tammem = RellenaInfoDef(NULL,fi,64,1,NULL,NULL);
			if (rel < 0)
				free_defpri(fi);
			else
				_salva_datos_unlock(rel);
		}
		else
		{
			*bloque = 0;			
		}
		bloque++;
		*((int *)bloque) = tammem;
		bloque += sizeof(int);
	}
	else if (operacion == 23) /* delvolver relacion completa de ficheros */
	{
		MENU_DEF *dir[5000];
		char tmp[512];
		int n;
		int j,k;
		int *memoria;
		int tot;
		int apartir;

		memoria = (int *)bloque;
		bloque += sizeof(int);
		apartir = *((int *)bloque);
		
		strcpy(tmp,dirficb);
		strcat(tmp,nomapli);

		n = lee_dir(tmp,dir,NULL,-1);

		tot = 0;
		for (j = 0;j < apartir && j < n;j++)
		{
			free((char *)dir[j]);
		}
		for (;j < n;j++)
		{
			if ((tot+MAXPATH+31) > *memoria)
				break;
			k = strlen(dir[j]->m_nombre) + 1;
			memcpy(bloque,dir[j]->m_nombre,k);
			bloque += k;
			tot += k;
			k = strlen(dir[j]->m_titulo) + 1;
			memcpy(bloque,dir[j]->m_titulo,k);
			bloque += k;
			tot += k;
			free((char *)dir[j]);
		}
		*memoria = (j-apartir);
		if (j < n)		
			*bloque = 1;			
		else
			*bloque = 0;
		bloque++;
		for (;j < n;j++)
		{
			free((char *)dir[j]);
		}
	}
	else if (operacion == 24) {
		lanza_consulta_logger(bloque);
	}
	else if (operacion == 25) {
		recoge_usuarios(bloque);
	}
    else if (operacion == 9999)
    {
        int elotro = -1;
		if (RetroPar[el_punto]) {
			if ( (par->gmodo & 0x10000) )
			{
				/* al borrar un 'hijo' solo, siempre se da el foco al padre o a un hermano */            
				if (par->padre)
				{
					int *hijos = par->padre->hijos;                
					elotro = par->padre->id_grid;
					while(*hijos > -2)
					{
						if (*hijos != el_punto && *hijos >= 0)
						{
							el_punto = ((struct gridpars *)RetroPar[*hijos])->id_grid;
							break;
						}
						hijos++;
					}
				}
				BorraHijoGrid(el_punto,1);
            
				/*
				if (elotro != -1)
				{
					(*DespachadorExterno)((int)DESPACHO_FOCOCONTROL,elotro,NULL);
				} 
				*/

			}
			else
			{
				if (par->Independiente)
				{
					if (RetroPar[el_punto])
						BorraGrid(el_punto);
				}
				else
				{
					/* inhabilitarlo ? */
				}
			}
			memcpy(bloque,(char *)&elotro,sizeof(int));
		}
    }

}


static int RellenaInfoClave(struct Info_Entra_Clave  *clave,FI_DEF *pfi,int c,int max_campos)
{
    int i,j,total,k;    
    int memoria = sizeof(struct Info_Entra_Clave)    +
                  pfi->f_clave[c].k_ncampos * sizeof(short) * 2; /* aqui se puede desaprovechar memoria ...*/
    int punto;
    int punto2;

    if (!clave)
    {
        return memoria;
    }

    memset((void *)clave,0,memoria);

    clave->Len = memoria;
    clave->Id = c;

    punto  = sizeof(struct Info_Entra_Clave);
    punto2 = punto + (sizeof(short)*pfi->f_clave[c].k_ncampos);
    clave->d_componentes = (short)punto;    
    clave->d_orden       = (short)punto2;
    total = 0;
    for (i = 0;i < pfi->f_clave[c].k_ncampos;i++)
    {
        k= (pfi->f_clave[c].k_campo[i] - 1);
        for (j = 0;j < max_campos;j++)
        {
            if (  pfi->f_taborder[j] == k )
				/*(pfi->f_campos[j].c_mante.c_actual%1000) == k )*/
            {
                *( (short *)(  (char *)(((char *)clave)+punto) ) ) = (short)j;
                punto += sizeof(short);

                *( (short *)(  (char *)(((char *)clave)+punto2) ) ) = (short)pfi->f_clave[c].k_orden[i];
                punto2 += sizeof(short);

                total++;
                break;
            }
        }        
    }
    clave->n_componentes = total;

    return memoria;
}

static int RellenaInfoCampo(struct Info_Entra_Campo  *campo,FI_DEF *pfi,int c,int *lreg)
{    
    int memoria = sizeof(struct Info_Entra_Campo)    +
                  strlen(pfi->f_campos[c].c_nombre) + 1 +
                  strlen(pfi->f_campos[c].c_minimo) + 1 +
                  strlen(pfi->f_campos[c].c_maximo) + 1 +
                  strlen(pfi->f_campos[c].c_usi)    + 1;
    int punto;    

    if (!campo)
    {
		if (lreg)
		{
		   *lreg += long_campo(pfi,c);
		}
        return memoria;
    }    

    memset((void *)campo,0,memoria);
    
    campo->Len = memoria;
    campo->Id = (short)c;
    campo->c_tipo    = pfi->f_campos[c].c_tipo;
    campo->c_maxqc = long_campo(pfi,c);
    if (campo->c_tipo == CHARTIPO || campo->c_tipo == FECHATIPO)
    {        
        campo->c_qc = pfi->f_campos[c].c_mante.c_qpl;
        if (campo->c_qc < 1 || campo->c_qc > campo->c_maxqc)
        {
            campo->c_qc = campo->c_maxqc;
        }
    }
    else
    {
        campo->c_qc = campo->c_maxqc;
    }


    campo->c_flags = 0;

	
	if (pfi->f_campos[c].c_mante.c_visual)
	{
		campo->c_flags |= 1;
	}	
	if (pfi->f_campos[c].c_mante.c_modif)
	{
		campo->c_flags |= 2;
	}
	if ((pfi->f_flag & 512) && !(pfi->f_campos[c].c_mante.c_lineal & C_LINEAL))
	{
		if (!pfi->f_campos[c].c_mante.c_visual)
		{
			campo->c_flags |= 1;
			campo->c_flags |= 4;
			pfi->f_campos[c].c_mante.c_lineal |= FGV_C_GRIDSTATICO;
		}
	}

    punto = sizeof(struct Info_Entra_Campo);

    campo->d_nombre    = punto;
    strcpy(((char *)campo)+punto,pfi->f_campos[c].c_nombre);
    quitab(((char *)campo)+punto);
                       punto += (strlen(pfi->f_campos[c].c_nombre) + 1);
    campo->d_minimo  = punto;
    strcpy(((char *)campo)+punto,pfi->f_campos[c].c_minimo);
                       punto += (strlen(pfi->f_campos[c].c_minimo) + 1);
    campo->d_maximo  = punto;
    strcpy(((char *)campo)+punto,pfi->f_campos[c].c_maximo);
                       punto += (strlen(pfi->f_campos[c].c_maximo) + 1);
    campo->d_usi     = punto;
    strcpy(((char *)campo)+punto,pfi->f_campos[c].c_usi);
                       punto += (strlen(pfi->f_campos[c].c_usi) + 1);

	if (!(pfi->f_flag & 512) || !(pfi->f_campos[c].c_mante.c_lineal & FGV_C_GRIDSTATICO))
	{		
		pfi->f_campos[c].c_mante.c_lineal |= FGV_C_GRID;
	}

    if (lreg)
    {
       *lreg += campo->c_maxqc;
    }

    return memoria;
}

int RellenaInfoDef(struct Info_Entra_Campos *info,FI_DEF *pfi,int flag,int key,int *len_reg,struct gridpars *par)
{
    struct Info_Entra_Campo  *campo;
    struct Info_Entra_Clave  *clave;
    int i,k,npan,numcam,campini,campfin,n_campos,n_claves,p_ncampos,rv;
    int memoria;     
    int memoria2;
    int fase;
    int conta_campos;
    int conta_claves;
    int lreg = 0;
    char *ptmp;
	int *orig_taborder = NULL;
	

    campo = NULL;
    clave = NULL;
    n_campos = 0;
    n_claves = 0;
    memoria = 0; 
	orig_taborder = (int *)malloc(sizeof(int) * pfi->f_ncampos);
	if (info && par)
	{
		par->campowids = (int *)malloc(sizeof(int) * pfi->f_ncampos);
		for (i = 0;i < pfi->f_ncampos;i++)
		{
			par->campowids[i] = pfi->f_campowids[i];
			pfi->f_campowids[i] = -1;
		}
	}
	for (i = 0;i < pfi->f_ncampos;i++)
	{
		if (par) {
			if (pfi->f_campos[i].c_nombre && !strcmp(pfi->f_campos[i].c_nombre,"RGBCOLOR")) {
				par->rgbcolor = i;
			}
		}
		orig_taborder[i] = pfi->f_taborder[i];		
	}

    for (fase = 0;fase < 2;fase++)
    {
        if (fase == 1) /* relleno despues de la contabilizacion */
        {
            memoria2 = memoria + ((n_campos+n_claves)*sizeof(int));
            memset((void *)info,0,memoria2);
			strcpy(info->NomFic,pfi->f_titulo);
            info->Len = memoria2;
            info->Id = (int)pfi;
            info->n_campos = n_campos;
            info->n_claves = n_claves;
            info->clave_defecto = key;
            info->clave_actual = key;
            info->t_total_campos = n_campos/*pfi->f_ncampos*/;
            info->d_Campos = memoria;            
            info->d_Claves = memoria + (n_campos*sizeof(int));
            if (!(flag & 1))
            {
                if (pfi->f_campogids) free(pfi->f_campogids);
                pfi->f_campogids = (int *)malloc(pfi->f_ncampos * sizeof(int));
                memset((char *)pfi->f_campogids,0xff,pfi->f_ncampos * sizeof(int));
            }
        }

        memoria = sizeof(struct Info_Entra_Campos);
        conta_campos = 0;
        conta_claves = 0;
        lreg = 0;

        if (!flag)
        {
           numcam=0;
           for (npan = 0;npan < pfi->f_npanta;npan++)
           {
               campini = pfi->f_pantalla[npan].p_campoini;
               campfin = pfi->f_pantalla[npan].p_campofin;
               for (; campini<=campfin && campini <= pfi->f_ncampos ;campini++) 
               {
                   numcam++;
               }
           }
           if (!numcam)
           {
               flag |= 64;
           }
        }

        if (!flag) /* orden de "pantalla"*/
        {
           for (npan = 0;npan < pfi->f_npanta;npan++)
           {
               campini = pfi->f_pantalla[npan].p_campoini;
               campfin = pfi->f_pantalla[npan].p_campofin; 
               for (numcam=campini; numcam<=campfin && numcam <= pfi->f_ncampos ;numcam++) 
               {
                    i = orig_taborder[numcam-1];/*pfi->f_campos[numcam-1].c_mante.c_actual % 1000;*/
                    if (i < 0 || i >= pfi->f_ncampos) 
						continue;
                    /*if (pfi->f_campos[i].c_mante.c_visual) continue;*/								

					if ((pfi->f_orderflags[numcam-1] & ORDER_FLAG_COPIA))
						continue;
					
					if (pfi->f_campos[i].c_tipo == NULOTIPO)
						continue;		

                    if (!hay_dat_permiso(pfi,10000+i)) continue;
                    
                    if (fase == 1)
                    {
                        campo = (struct Info_Entra_Campo  *) ( ( (char *) ((char *)info) + memoria) );
                        *((int *)((char *)((char *)info+info->d_Campos+(conta_campos*sizeof(int))))) = memoria;
                    }

                    if ((flag & 1))
                    {
                        ptmp = pfi->f_campos[i].c_nombre;
                        pfi->f_campos[i].c_nombre = pfi->f_campos[i].c_lista;
                    }
                    memoria += RellenaInfoCampo(campo,pfi,i,&lreg);
                    if ((flag & 1))
                    {
                        pfi->f_campos[i].c_nombre = ptmp;
                    }

                    if (fase == 1)
                    {
                        if (!(flag & 1) && pfi->f_campogids)
                        {
                            pfi->f_campogids[i] = conta_campos;
							campo->p_actual = orig_taborder[conta_campos];
							pfi->f_taborder[conta_campos] = i;
							
                            campo->l_actual = pfi->f_campos[i].c_mante.c_actual;							
                            /*pfi->f_campos[i].c_mante.c_actual = pfi->f_campos[i].c_mante.c_actual/1000*1000 + (pfi->f_campos[i].c_mante.c_actual%1000);*/
							
                            /*campo->p_actual = pfi->f_campos[conta_campos].c_mante.c_actual;^*/                            
							pfi->f_campos[conta_campos].c_mante.c_actual = ((pfi->f_campos[conta_campos].c_mante.c_actual/1000)*1000)+i;							
                        }
                    }
                    
                    conta_campos++;
                    if (!fase)
                        n_campos++;
               }
            }

            /* Incluir el resto como no visualizable !! */
            if (!fase)
            {
                p_ncampos = n_campos;
            }
            for (i = 0;i < pfi->f_ncampos;i++)
            {
				if (pfi->f_campos[i].c_tipo == NULOTIPO) 
					continue;

                for (numcam = 0;numcam < p_ncampos;numcam++)
                {
                    if ( /*(pfi->f_campos[numcam].c_mante.c_actual%1000) == i*/orig_taborder[numcam] == i )
                    {
                        break;
                    }
                }
                if (numcam < p_ncampos) 
                    continue;
			    
                if (fase == 1)
                {
                    campo = (struct Info_Entra_Campo  *) ( ( (char *) ((char *)info) + memoria) );
                    *((int *)((char *)((char *)info+info->d_Campos+(conta_campos*sizeof(int))))) = memoria;
                }
                if ((flag & 1))
                {
                    ptmp = pfi->f_campos[i].c_nombre;
                    pfi->f_campos[i].c_nombre = pfi->f_campos[i].c_lista;
                }
                rv = pfi->f_campos[i].c_mante.c_visual;
                pfi->f_campos[i].c_mante.c_visual = 1;
                memoria += RellenaInfoCampo(campo,pfi,i,&lreg);
                pfi->f_campos[i].c_mante.c_visual = rv;
				pfi->f_campos[i].c_mante.c_lineal &= ~FGV_C_GRID;
                if ((flag & 1))
                {
                    pfi->f_campos[i].c_nombre = ptmp;
                }
                if (fase == 1)
                {
                    if (!(flag & 1) && pfi->f_campogids)
                    {
                        pfi->f_campogids[i] = conta_campos;
						
                        campo->l_actual = pfi->f_campos[i].c_mante.c_actual;
                        /*pfi->f_campos[i].c_mante.c_actual = 1000 + (pfi->f_campos[i].c_mante.c_actual%1000);*/

                        /*campo->p_actual = pfi->f_campos[conta_campos].c_mante.c_actual;*/
                        pfi->f_campos[conta_campos].c_mante.c_actual = ((pfi->f_campos[conta_campos].c_mante.c_actual/1000)*1000)+i;
						
						campo->p_actual = orig_taborder[conta_campos];						
						pfi->f_taborder[conta_campos] = i;
                    }
                    if (par && (par->gmodo & 512))
					{
						pfi->f_campos[i].c_mante.c_visual |= 0x4000;
					}
                }                
                conta_campos++;
                if (!fase)
                    n_campos++;
            }
        }
        else
        {         /* orden de "natural"*/
            for (i = 0;i < pfi->f_ncampos;i++)
            {
                if (!(flag & 1))
                {
                    /*if (pfi->f_campos[i].c_mante.c_visual) continue;*/					
					
                }

				if (pfi->f_campos[i].c_tipo == NULOTIPO)
					continue;				
				
                if (!hay_dat_permiso(pfi,10000+i)) continue;

                if (fase == 1)
                {
                    campo = (struct Info_Entra_Campo  *) ( ((char *)((char *)info) + memoria) );
                    *((int *)((char *)((char *)info+info->d_Campos+(conta_campos*sizeof(int))))) = memoria;
                }

                if ((flag & 1))
                {
                    ptmp = pfi->f_campos[i].c_nombre;
                    pfi->f_campos[i].c_nombre = pfi->f_campos[i].c_lista;
                }
                memoria += RellenaInfoCampo(campo,pfi,i,&lreg);
                if ((flag & 1))
                {
                    pfi->f_campos[i].c_nombre = ptmp;
                }

                
                if (fase == 1)
                {
                    if (!(flag & 1) && pfi->f_campogids)
                    {
                        pfi->f_campogids[i] = conta_campos;                 
						
                        campo->l_actual = pfi->f_campos[i].c_mante.c_actual;
                        /*pfi->f_campos[i].c_mante.c_actual = 1000 + (pfi->f_campos[i].c_mante.c_actual%1000);*/

                        /*campo->p_actual = pfi->f_campos[conta_campos].c_mante.c_actual;*/						
                        pfi->f_campos[conta_campos].c_mante.c_actual = ((pfi->f_campos[conta_campos].c_mante.c_actual/1000)*1000)+i;												
						campo->p_actual = orig_taborder[conta_campos];
						pfi->f_taborder[conta_campos] = i;
						
                    }
                }
                conta_campos++;
                if (!fase)
                    n_campos++;
            }
        }

        for (i = 0;i < pfi->f_nclaves;i++)
        {
            if (fase == 1)
            {
                clave = (struct Info_Entra_Clave  *)( (char *) (((char *)info) + memoria) );
                *((int *)((char *)((char *)info+info->d_Claves+(conta_claves*sizeof(int))))) = memoria;
            }
            memoria += RellenaInfoClave(clave,pfi,i,conta_campos);
            conta_claves++;
            if (!fase)        
                n_claves++;
        }        

        if (!info) break;


        if (fase == 1)
        {
            info->p_npan = pfi->f_npanta;
            pfi->f_npanta = 1;
            if (info->p_npan > 0)
            {
               info->p_campini = pfi->f_pantalla[0].p_campoini;
               info->p_campfin = pfi->f_pantalla[0].p_campofin;
            }
            else
            {
                info->p_campini = 0;                
                if (!pfi->f_pantalla)
                {                    
                    pfi->f_pantalla = (PAN_DEF *)malloc(sizeof(PAN_DEF)+1);
                    info->p_campfin = -2;
                }
                else
                    info->p_campfin = -1;
            }
            pfi->f_npanta = 1;
            pfi->f_pantalla[0].p_campoini = 1;
            pfi->f_pantalla[0].p_campofin = conta_campos;
            strcpy(info->PathDef,pfi->f_PathDef); //quita_path(pfi->f_PathDef)
        }
    }

	if (orig_taborder)
	{
		free((char *)orig_taborder);
	}

    if (len_reg)
        *len_reg = lreg;
    memoria2 = memoria + ((n_campos+n_claves)*sizeof(int));
    return memoria2;
}


int DameRetroPar()
{
int mi_punto = -1;
int i;

  for (i = 0;i < RetroPunto;i++)
     {
	 if (!RetroFuncion[i]) break;
	 }
  if (i == RetroPunto)
     {
	  if (i > 4095) {
		  return -1;
	  }
	 mi_punto = RetroPunto;
	 RetroPunto++;
	 }
  else
     mi_punto = i;
  
  return mi_punto;
}

int busca_mexterna(char *nombre)
{
	int ll;
    for (ll = memoplin+maxpila*2;ll < nm_ex;ll++)
	{
         if (!strcmp(nombre,m_ex[ll].nombre)) 
		 {
			return ll;            
		 }
    }
	return -1;
}

static void cambia_nom_fich(char *fich,int ordinal)
{
	char tmp[50];
	char usi[50];
	int i;
	int n = 0;
	for (i = 0;fich[i];i++)
	{
		if (fich[i] == '_')
			n++;
	}
	sprintf(usi,"%%0%dd",n);
	sprintf(tmp,usi,ordinal);
	n = 0;
	for (i = 0;fich[i];i++)
	{
		if (fich[i] == '_')
		{
			fich[i] = tmp[n];
			n++;
		}
	}
}

void DespachaVInterface(char *bloque,char *par,int el_punto)
{
	int i,j;
	long *pl = (long *)bloque;	
	long nreg = 0L;
	long *plx;
	char *buffer = (char *)(pl+4);
	char tmp[512];
	struct Info_Entra_Campos *InfoCampos;
	long tammem = 0L;			
	FI_DEF *pfi = NULL;			
	long lreg;
	static FI_DEF *pfi_res = NULL;

	switch(pl[3])
	{
	case 0: /* informacion tablas y var numericas*/
		{	
			char *pbuf;
			buffer += sizeof(long);
			buffer += (pl[4] * sizeof(long));
			pbuf = buffer;
			for (i = 0;i < pl[4];i++,buffer += 31)
			{
				if (*buffer == '|')
				{
				  for (j = 0;j < np_ex;j++)
				  {
					 if (!strcmp(buffer+1,p_ex[j].nombre)) 
					 {
						p_ejecuta(p_ex[j].indice,O_NOR,0,28);
						strcpy(pbuf+pl[5+i],m_ex[4].mv.valc);
						break;
					 }
				  }
				}
				else if (*buffer == '?')
				{
					j = busca_mexterna(buffer+1);
					if (j >= 0)
					{
						if (m_ex[j].m_tipo == DOUBLETIPO)
						{
							sprintf(pbuf+pl[5+i],"%lf",m_ex[j].mv.vald);
						}
						else
						{
							strcpy(pbuf+pl[5+i],m_ex[j].mv.valc);
						}
					}
				}
				else if (*buffer == ':')
				{
					j = busca_mexterna(buffer+1);
					if (j >= 0)
					{
						if (m_ex[j].m_tipo == DOUBLETIPO)
						{
							m_ex[j].mv.vald = rwatof(pbuf+pl[5+i]);
						}
						else
						{
						    if (j >= memoplin)
							{
								free(m_ex[j].mv.valc);
								m_ex[j].mv.valc = malloc(strlen(pbuf+pl[5+i])+10);
								strcpy(m_ex[j].mv.valc,pbuf+pl[5+i]);
							}
							else
							{
								strcpy(m_ex[j].mv.valc,pbuf+pl[5+i]);
							}
						}
					}
				}
				else if (*buffer == '#')
				{				    
					j = busca_mexterna(buffer+1);
					if (j >= 0)
					{
						if (m_ex[j].m_tipo == DOUBLETIPO)
						{
							sprintf(pbuf+pl[5+i],"%lf",m_ex[j].mv.vald);
						}
						else
						{
							strcpy(pbuf+pl[5+i],m_ex[j].mv.valc);
						}
					}
					RetroPar[el_punto] = (char *)atoi(pbuf+pl[5+i]);
					par = RetroPar[el_punto];
				}
				else if (*buffer == '@')
				{	
					if (!strcmp(buffer+1,"dirfic"))
					{
						strcpy(pbuf+pl[5+i],dirfic);						
					}
					else if (!strcmp(buffer+1,"nom_inf"))
					{
						strcpy(pbuf+pl[5+i],nom_inf);
					}
					else if (!strcmp(buffer+1,"dirbase"))
					{
						strcpy(pbuf+pl[5+i],dirbase);						
					}
					else if (!strcmp(buffer+1,"dirbass"))
					{
						strcpy(pbuf+pl[5+i],dirbass);						
					}
					else if (!strcmp(buffer+1,"cs"))
					{
#ifdef RWSERVER
						if (en_conexion_externa)
							strcpy(pbuf+pl[5+i],"SI");						
						else
#endif
							strcpy(pbuf+pl[5+i],"NO");
					}
					else if (!strcmp(buffer+1,"numerofax"))
					{
						strcpy(pbuf+pl[5+i],numerofax);						
					}

				}
				else if (*buffer == 13)
				{
					dsdb_sql(pbuf+pl[5+i],tmp,&pfi_res,NULL,NULL);
					strcpy(pbuf+pl[5+i],tmp);
					if (atoi(pbuf+pl[5+i]) > -1 && pfi_res)
					{
						tammem = 0;
						pfi = pfi_res;
						if (pfi->f_fd >= 0 || abre_ind(pfi,ISINOUT + ISMANULOCK) >= 0)
						{
							tammem += RellenaInfoDef(NULL,pfi,1,1,&lreg,NULL);
							nreg = tam_btrv(pfi);
							tammem += (sizeof(long)*5) + (nreg * lreg) + 512;
							cierra_ind(pfi);
							sprintf(pbuf+pl[5+i],"%ld",tammem);
						}
					}
					else
					{
						if (pfi_res && pfi_res->f_fd >= 0)
							cierra_ind(pfi_res);
					}
				}
				else
				{
					j = strlen(buffer);
					if (j > 0)
					{
						j--;
						if (buffer[j] == '@')
						{
							tammem = 0;
							strcpy(tmp,buffer);							
							tmp[j] = 0;
							j = ind_rel(tmp);
							if (j >= 0)
							{
								pfi = v_mpfi(j);
								tammem += RellenaInfoDef(NULL,pfi,1,1,&lreg,NULL);
								nreg = tam_btrv(pfi);
								tammem += (sizeof(long)*5) + (nreg * lreg) + 512;
							}
							pl[5+i] = tammem;
							continue;
						}
					}
					tammem = 0;
					strcpy(tmp,dirdef);
					strcat(tmp,buffer);
					pfi = NULL;
					if (lee_defn(tmp,&pfi) >= 0) 
					{
						cambia_path_fich(pfi->f_fichero);
						cambia_nom_fich(pfi->f_fichero,(int)par);
						if (abre_ind(pfi,ISINOUT + ISMANULOCK) >= 0)
						{
							tammem += RellenaInfoDef(NULL,pfi,1,1,&lreg,NULL);
							nreg = tam_btrv(pfi);
							tammem += (sizeof(long)*5) + (nreg * lreg) + 512;
						    cierra_ind(pfi);
						}
						free_defpri(pfi);
					}
					pl[5+i] = tammem;
				}
			}
		}
		break;
	case 1: /* cargar tabla*/
	case 2: /* cargar solo def */
		{
			long k = 0;
			int cargado = 0;

			if (*buffer == 1)
			{
				if (!pfi_res)
				{
					pl[3] = 0;
					break;
				}
				pfi = pfi_res;
				pfi_res = NULL;
				cargado = 3;
			}
			else
			{
				j = strlen(buffer);
				if (j > 0)
				{
					j--;
					if (buffer[j] == '@')
					{
						tammem = 0;
						strcpy(tmp,buffer);							
						tmp[j] = 0;
						j = ind_rel(tmp);
						if (j >= 0)
						{
							pfi = v_mpfi(j);
							cargado = 1;
						}
					}
				}
			}
			if (!cargado)
			{
				strcpy(tmp,dirdef);
				strcat(tmp,buffer);
				pfi = NULL;
				if (lee_defn(tmp,&pfi) >= 0) 
				{
					cargado = 2;
				}
			}
			if (cargado)
			{
				if (cargado == 2 || cargado == 3)
				{
					if (cargado == 2)
					{
						cambia_path_fich(pfi->f_fichero);
						cambia_nom_fich(pfi->f_fichero,(int)par);
					}
					if (abre_ind(pfi,ISINOUT + ISMANULOCK) < 0)
					{
						if (crea_ind(pfi) < 0)
						{
							cargado = 0;
						}
					}
				}
				if (cargado)
				{
					nreg = tam_btrv(pfi);
					buffer += sizeof(long);
					tammem += sizeof(long);
					InfoCampos = (struct Info_Entra_Campos *)((char *)(buffer));
					lreg = 0;
					pl[4] = RellenaInfoDef(InfoCampos,pfi,1,1,&lreg,NULL);
					buffer += pl[4];
					tammem += pl[4];
					*((long *)buffer) = lreg;
					buffer += sizeof(long);
					tammem += sizeof(long);
					plx = (long *)buffer;
					*plx = 0L;
					buffer += sizeof(long);
					tammem += sizeof(long);
					if (pl[3] == 1)
					{
						if (lee_reg(pfi,ISFIRST) >= 0)
						{
							k = 0;
							do
							{
								for (i = 0;i < InfoCampos->n_campos;i++)
								{
									formato_campo(pfi,dInfoCampo(InfoCampos,i,0)->Id,buffer);
									j = dInfoCampo(InfoCampos,i ,0)->c_maxqc;
									buffer += j;
									tammem += j;
								}
								(*plx)++;
								k++;
							} while(k < nreg && lee_reg(pfi,ISNEXT) >= 0);
						}
					}
					if (cargado != 1)
					{
						cierra_ind(pfi);
					}
				}
				if (cargado != 1)
				{
					if (cargado == 3)
					{
						borra_ind(pfi);
					}
					free_defpri(pfi);
				}
			}
			pl[3] = tammem;
		}
		break;
	case 3:
		/* grabar datos */
		{		    
			long k;

		    buffer += sizeof(long);
		    InfoCampos = (struct Info_Entra_Campos *)((char *)(buffer));
			buffer += pl[4];
			strcpy(tmp,InfoCampos->PathDef);			
            if (lee_defn(tmp,&pfi) >= 0) 
			{
				int abierto = 1;
				cambia_path_fich(pfi->f_fichero);
				cambia_nom_fich(pfi->f_fichero,(int)par);
				if (abre_ind(pfi,ISINOUT + ISMANULOCK) < 0)
				{
				    if (crea_ind(pfi) < 0)
						abierto = 0;
				}
				if (abierto)
				{
					lreg = *((long *)buffer);
					buffer += sizeof(long);
					nreg = *((long *)buffer);
					buffer += sizeof(long);
					for (k = 0;k < nreg;k++)
					{
						defecto_ficha(pfi);
						for (i = 0;i < InfoCampos->n_campos;i++)
						{
							j = dInfoCampo(InfoCampos,i,0)->c_maxqc;
							memcpy(tmp,buffer,j);
							tmp[j] = 0;
							st_campo(dInfoCampo(InfoCampos,i,0)->Id,pfi,tmp);
							buffer += j;
						}
						if (graba_nuevo(pfi) < 0)
						{
							borra_reg(pfi);
							graba_nuevo(pfi);
						}
					}
					cierra_ind(pfi);
				}
				free_defpri(pfi);
			}
		}
		break;
	default:
		break;
	}
}

/*
int CargaVInterface(char *nome,int visual)
{
    char tmp[512];
    long modo = 1;    
    long tam = (sizeof(long)*10)+strlen(nome)+1;	
    long id = DameRetroPar();
	int ant_modo;
    

    RetroFuncion[id] = DespachaVInterface;
	RetroTipo[id]    = 2;
    RetroPar[id]     = NULL;

    memcpy(tmp,&tam,sizeof(long));
    memcpy(tmp+sizeof(long),&tam,sizeof(long));
    memcpy(tmp+(sizeof(long)*2),&modo,sizeof(long));
	memcpy(tmp+(sizeof(long)*3),&visual,sizeof(long));
    memcpy(tmp+(sizeof(long)*4),&id,sizeof(long));
    strcpy(tmp+(sizeof(long)*5),nome);

	ant_modo = modo_desatendido;
	modo_desatendido = 807;
    (*DespachadorExterno)((int)DESPACHO_BINARIO,(int)100,tmp);
	modo_desatendido = ant_modo;

	FinRetroPar(id);
    n_np = 1;
    return 0;
}
*/


int CargaVInterface(char *nome,unsigned long visual)
{
    char tmp[1024];
    long modo = 1;    
    long tam = (sizeof(long)*10)+strlen(nome)+1;	
    long id = DameRetroPar();
	int ant_modo;
	int extendido = 0;
	char *p;

	RetroPar[id]     = NULL;

	if (!strncmp(nome,"DSITCP:",7))
	{
		ant_modo = modo_desatendido;
		modo_desatendido = 807;

		DSTCP_VInterface(atoi(nome+7),visual,-1);

		modo_desatendido = ant_modo;
		n_np = 1;
		return 0;
	}


	if ((visual & 0x80000000))
	{		
		RetroPar[id] = 0xfffffff;
		extendido = 1;		
	}    

    RetroFuncion[id] = DespachaVInterface;
	RetroTipo[id]    = 2;
    

    memcpy(tmp+(sizeof(long)*2),&modo,sizeof(long));
	memcpy(tmp+(sizeof(long)*3),&visual,sizeof(long));
    memcpy(tmp+(sizeof(long)*4),&id,sizeof(long));
	p = tmp+(sizeof(long)*5);
    strcpy(p,nome);
	if (extendido)
	{		
		p += (strlen(p)+1);
		strcpy(p,mi_remoto_dirbass());
		strcat(p,"crystal/");
		tam += (strlen(p)+1); 
		p += (strlen(p)+1);
		if (nom_inf[0] == ':' && nom_inf[1] == ':')
		{
			strcpy(p,nom_inf+2);
		}
		else
		{
			strcpy(p,mi_remoto_dirbass());
			strcat(p,"crystal/");
			strcat(p,quita_path(nom_inf));
		}
		if (!strstr(p,".rpt"))
			strcat(p,".rpt");

		{
			extern unsigned char nombredev[81];			
			if (nombredev[0] == 2 && nombredev[1] == 0 && nombredev[2])
			{
				  strcat(p,";");				  
				  if (nombredev[2] == '!' && nombredev[3] == '!')
				  {
					  strcat(p,"!!");
					  strcat(p,numerofax);
				  }
				  else if (nombredev[2] == '|' && nombredev[3] == '|')
				  {
					  strcat(p,"||");
					  strcat(p,numerofax);
				  }
				  else
					  strcat(p,nombredev+2);
			}
		}
		tam += (strlen(p)+1); 
		p += (strlen(p)+1);
	}
    memcpy(tmp,&tam,sizeof(long));
    memcpy(tmp+sizeof(long),&tam,sizeof(long));
	/*
 
     */
	ant_modo = modo_desatendido;
	modo_desatendido = 807;
	if (DespachadorExterno)
		(*DespachadorExterno)((int)DESPACHO_BINARIO,(int)100,tmp);
	modo_desatendido = ant_modo;

	FinRetroPar(id);
	/*
    memcpy(&id,tmp+(sizeof(long)*3),sizeof(long));
    sprintf(v_memo(5004)->alfabetica,"%d",id);
	*/
    n_np = 1;
    return 0;
}



int dame_campo_del_grid(int ipfi,int campodef)
{
	FI_DEF *pfi = v_mpfi(ipfi);
	int memoria;	

	memoria = RellenaInfoDef(NULL,pfi,0,1,NULL,&campodef);  

	if (!memoria)
		return -1;

	return campodef;
}

/*
int nfiltros = 0;
struct grid_filtro *pfiltro = NULL;
int *cols = NULL;
int ncols = 0;
int sql = 0;
*/

int LanzaGrid(struct gridpars *par,int ncolucam,int *colucam,int nfiltros,struct grid_filtro *pfiltro,int *cols,int ncols,int sql)
{
char *tmp = NULL;
long tam;
long modo = 1;
long registros;
long inicial;
int encl;
int i;
struct Info_Entra_Campos *InfoCampos;
int mi_punto = -1;
long regcur;
int id_grid;
int hook;
int pi;
int pf;
FI_DEF *pfi;
int gmodo;
int oc;
int memoria;
int lreg;
int padre = -1;
int rellenotam;
char *ptmpregistros;

if (DespachadorExterno)
{	
  encl  = par->encl;
  hook  = par->hook;
  pfi  = par->pfi;
  /* Pruebas ...
  if (!par->padre)
  {
	strcpy(par->autofiltro,"?D:/ds/dev/usr/008/Prueba.flt");
  }
  */
  if (par->autofiltro[0] == '?')
  {
	  par->gmodo |= 0x2000000;
  }

  gmodo = par->gmodo;
  pi    = par->pi;
  pf    = par->pf;    

  par->cache = NULL;
  par->rgbcolor = -1;

  pfi->f_idgrafico = -1;

  oc = ind_clave_activa(pfi) - 1;

  if (oc != encl) 
  {
     apunta_ind(pfi,encl+1,ISCURR,0);
  }  

  if (lee_reg(pfi,ISGTEQ) < 0 || (gmodo & 128))
      inicial = 1;
  else
  {
      inicial = IxxNumeroPosicion + 1;
  }

  par->regsel = isrecnum;
  /*
	if (par->regsel < 1 || par->regsel > 1000000) {
		par->regsel = 0;
	}
	*/


  registros = TotalRegistros(par);
  inicial -= par->offset;
  if (inicial < 0 || inicial > registros)
      inicial = 1;

  mi_punto = DameRetroPar();  

  lreg = 0;
  if (par->limi1 || par->limi2 || par->b_lim1 || par->b_lim2)
  {
	  par->gmodo |= 0x80000;
	  gmodo |= 0x80000;
  }
  if (par->totales)
  {
	  par->gmodo |= 0x100000;
	  gmodo |= 0x100000;
  }

  if (!gridextras)
  {
	  par->gmodo |= 0x4000000;
	  gmodo |= 0x4000000;
  }

  /*
  if (sinescape)
  {
	  par->gmodo |= 0x8000000;
	  gmodo |= 0x8000000;
  }
  */
  
  tam = RellenaInfoDef(NULL,pfi,(gmodo & 64),encl,&lreg,par);  
  
  if (tam < lreg)
      tam = lreg;
  memoria = 2048 + sizeof(long) * 16 + tam + sizeof(int)*3;
  tmp = malloc(memoria);  

  tam = sizeof(long)*3;
  memcpy(tmp+tam,&mi_punto,sizeof(int));
  tam += sizeof(int);
  memcpy(tmp+tam,&registros,sizeof(long));
  ptmpregistros = tmp+tam;
  tam += sizeof(long);
  memcpy(tmp+tam,&inicial,sizeof(long));
  tam += sizeof(long);

  strcpy(tmp+tam,par->titulo);
  tam += (strlen(tmp+tam)+1);

  /*gmodo = 0;/*4 + 8 + 16;*/

  memcpy(tmp+tam,&gmodo,sizeof(int));
  tam += sizeof(int);  

  memcpy(tmp+tam,&pi,sizeof(int));
  tam += sizeof(int);  
  
  memcpy(tmp+tam,&pf,sizeof(int));
  tam += sizeof(int);

  memcpy(tmp+tam,&(par->_wid),sizeof(int));
  tam += sizeof(int);
  memcpy(tmp+tam,&(par->_id),sizeof(int));
  tam += sizeof(int);
  memcpy(tmp+tam,&(par->_sid),sizeof(int));
  tam += sizeof(int);

  if (par->control_ipfi > 0 && par->control_ipfi <= nfi && par->control_campo > 0)
  {
	  int oipfi = par->control_ipfi - 1;
	  int ocampo = -(par->control_campo);	  
	  if (par->control_campo <= v_mpfi(par->control_ipfi - 1)->f_ncampos && (v_mpfi(par->control_ipfi - 1)->f_campos[par->control_campo-1].c_mante.c_lineal & FGV_C_GRID))
	  {
		  oipfi = -(v_mpfi(par->control_ipfi - 1)->f_idgrafico+1);		  
	  }	  
	  memcpy(tmp+tam,&ocampo,sizeof(int));
	  tam += sizeof(int);  
	  memcpy(tmp+tam,&oipfi,sizeof(int));
	  tam += sizeof(int);	  
  }
  else
  {
	  memcpy(tmp+tam,&(par->_sid),sizeof(int));
	  tam += sizeof(int);
      memcpy(tmp+tam,&(par->_wid),sizeof(int));
      tam += sizeof(int);
  }



  if (par->padre)
  {
      padre = par->padre->id_grid;
  }
  memcpy(tmp+tam,&padre,sizeof(int));
  tam += sizeof(int);
  
  InfoCampos = (struct Info_Entra_Campos *)((char *)(tmp+tam));
  
  tam += RellenaInfoDef(InfoCampos,pfi,(gmodo & 64),encl,NULL,par);
  
  rellenotam = 500;
  memset(tmp+tam,0,rellenotam);
  if (!par->claves_validas && (gmodo & 0x80000))
  {
  	  par->claves_validas = (int *)malloc(2*sizeof(int));
	  memset(par->claves_validas,0,2* sizeof(int));
	  par->claves_validas[0] = 1;
	  par->claves_validas[1] = -1; /* aponer el defecto */
  }
  if (par->claves_validas)
  {
	  par->claves_validas[1] = InfoCampos->clave_defecto;
	  memcpy(tmp+tam,par->claves_validas,(par->claves_validas[0]+1)*sizeof(int));
	  tam += ((par->claves_validas[0]+1)*sizeof(int));
	  rellenotam -= ((par->claves_validas[0]+1)*sizeof(int));
  }
  if (par->totales)
  {
	  memcpy(tmp+tam,par->totales,(par->totales[0]+1)*sizeof(int));
	  tam += ((par->totales[0]+1)*sizeof(int));
	  rellenotam -= ((par->totales[0]+1)*sizeof(int));
  }
  if (rellenotam > 0)
  {
	  tam += rellenotam; /* ojo */
  }

  memcpy(tmp,&tam,sizeof(long));
  tam = (sizeof(long)*5)+sizeof(int)*2;
  memcpy(tmp+sizeof(long),&tam,sizeof(long));
  memcpy(tmp+(sizeof(long)*2),&modo,sizeof(long));  

  par->InfoCampos = InfoCampos;    
   
  par->colcampos = (int *)malloc(sizeof(int) * InfoCampos->n_campos);

  if (par->autofiltro[0] && par->autofiltro[0] != '?')
  {
	  pfiltro = rellena_gridfiltro(&nfiltros,&ncols,&cols,par->autofiltro,par->InfoCampos,&sql,par->pfi);
	  if (cols && ncols)
	  {
		  colucam = cols;
		  ncolucam = ncols;
	  }
  }


  if (!colucam)
  {
      par->ncampos = 0;
      for (i = 0;i < InfoCampos->n_campos;i++)
      {
            if (!(dInfoCampo(InfoCampos,i,0)->c_flags & 1))
            {
                par->colcampos[par->ncampos] = i;
                par->ncampos++;
            }
      }  
  }
  else
  {
      par->ncampos = ncolucam;
      for (i = 0;i < InfoCampos->n_campos;i++)
      {
            dInfoCampo(InfoCampos,i,0)->c_flags |= 1;
      }
      for (i = 0;i < ncolucam;i++)
      {
            dInfoCampo(InfoCampos,colucam[i],0)->c_flags &= ~1;
            par->colcampos[i] = colucam[i];
      }
  }
  for (i = par->ncampos;i < InfoCampos->n_campos;i++)
  {
      par->colcampos[i] = -1;
  }

  par->reg = inicial;
  par->regcur = inicial;  
  par->hijos = NULL;

  RetroFuncion[mi_punto] = FGVHookGrid;
  RetroTipo[mi_punto]    = 3;
  RetroPar[mi_punto]     = (char *)par;

  if ((gmodo & 0x100000))
  {
	  int ok,j;
	  extern double ld_doble();

	  for (i = 0;i < par->registros;i++)
	  {
		  no_corrup_error = 1;
		  ok = grid_lee_por_posicion(par,par->encl+1,par->offset+i,par->pfi->f_buf,0,0,NULL,NULL);
		  no_corrup_error = 0;
		  if (ok < 0)
		  {
			  if (ixxstatus == IXXECORRUP)
			  {
				  ixx_reconstruye(par->pfi->f_fd);
				  i--;
				  continue;
			  }
			  break;
		  }
		  for (j = 0;j < par->totales[0];j++)
		  {
			  if ((par->totales[j+1]&0xf0000) == 0x10000)
			  {
				  if (par->t_totales[j] < ld_doble((par->totales[j+1]&0xffff),par->pfi))
					  par->t_totales[j] = ld_doble((par->totales[j+1]&0xffff),par->pfi);				  
			  }
			  else if ((par->totales[j+1]&0xf0000) == 0x20000)
			  {
				  if (par->t_totales[j] > ld_doble((par->totales[j+1]&0xffff),par->pfi))
					  par->t_totales[j] = ld_doble((par->totales[j+1]&0xffff),par->pfi);				  
			  }
			  else
				  par->t_totales[j] += ld_doble((par->totales[j+1]&0xffff),par->pfi);
		  }
	  }
	  registros = par->registros;
	  memcpy(ptmpregistros,&registros,sizeof(long));	  
  }

  if (pfiltro)
  {
     par->id_grid = id_grid = -1;
	 pfi->f_idgrafico = -1;		 
	 LanZaHiJo(par,par->titulo,nfiltros,pfiltro,ncols,cols,0,par->hook,sql);
     if (cols)
	 {
		free(cols);
	 }
	 if (!sql && pfiltro)
	 {
		free(pfiltro);
	 }
  }
  else
  {
	  if (cols)
		  free(cols);
	  if (DespachadorExterno)
		(*DespachadorExterno)((int)DESPACHO_BINARIO,(int)0,tmp);  

	  memcpy(&id_grid,tmp+(sizeof(long)*3),sizeof(int));
	  memcpy(&par->curcol,tmp+(sizeof(long)*3)+sizeof(int),sizeof(int));
  }


  par->id_grid = id_grid;
  pfi->f_idgrafico = id_grid;
  pfi->f_puntoretropar = mi_punto;

  par->for_free = tmp;  

  if (id_grid < 0 && ((!par->autofiltro[0] && !pfiltro) || par->autofiltro[0] == '?'))
  {	  
      mi_punto = BorraGrid(mi_punto);       
  }
  /*agi_cola_timer();*/
}

return(mi_punto);
}

int BorraHijoGrid(int mi_punto,int alpadre)
{
    struct gridpars *par;
    struct gridpars *padre;
    int r;
    

    if (mi_punto < 0) return mi_punto;

    par = (struct gridpars *)RetroPar[mi_punto];    
    if (!par) return -1;


    padre = par->padre;    
    
    r = BorraGrid(mi_punto);    

    if (par->pfi)
    {
        if (par->pfi->f_fd > -1)
        {
            cierra_ind(par->pfi);			
            if (par->temporal)
            {
                rw_base_unlink(par->pfi->f_fichero);
                borra_ind(par->pfi);
            }
        }
        free_defpri(par->pfi);
    }
    par->pfi = NULL;

    if (par->filtros)
    {
        free(par->filtros);
    }   par->filtros = NULL;

    free(par->titulo);
    free((char *)par);

    if (alpadre && padre)
    {
      int *hijos = padre->hijos;
      int *pijo = NULL;

      while(*hijos > -2)
      {
          if (*hijos == mi_punto)
          {
              pijo = hijos;
          }
          hijos++;
      }
      if (pijo)
      {
          hijos--;
          *pijo = *hijos;
          *hijos = -2;
      }      
    }

    return r;
}

int FinRetroPar(int mi_punto)
{
  RetroFuncion[mi_punto] = NULL; 
  RetroTipo[mi_punto]    = 0;
  RetroPar[mi_punto] = NULL;
  if (mi_punto == (RetroPunto-1))
  {
      do 
        {
        RetroPunto--;    
	    } while(RetroPunto > 0 && !RetroFuncion[RetroPunto-1]);
  }

  return mi_punto;
}


int BorraGrid(int mi_punto)
{
char tmp[512];
struct gridpars *par;
int id_grid = -1;
int c,i;
long modo = 1;
long tam;
FI_DEF *pfi;
struct Info_Entra_Campos *info;
struct Info_Entra_Campo  *campo;

  if (mi_punto < 0) return mi_punto;  

  par = (struct gridpars *)RetroPar[mi_punto];

  if (!par)
	  return mi_punto;  


  BorraLosHijos(par);

  if (!RetroPar[mi_punto])
	  return mi_punto;  

  /*pulsatecla();*/
  
  id_grid = par->id_grid; 

  if (par->control_ipfi > 0 && par->control_ipfi <= nfi) {
      int ipfi = par->control_ipfi-1;
	  int campo = par->control_campo-1;
	  if (campo >= 0 && campo < v_mpfi(ipfi)->f_ncampos && v_mpfi(ipfi)->f_campowids[campo] == id_grid) {
			v_mpfi(ipfi)->f_campowids[campo] = -1;
			v_mpfi(ipfi)->f_campos[campo].c_mante.c_visual = 1;
			v_mpfi(ipfi)->f_campos[campo].c_mante.c_modif = 1;
	  }
	  par->control_ipfi = 0;
	  par->control_campo = 0;
  }

  
  tam = (sizeof(long)*3)+sizeof(int);
  memcpy(tmp,&tam,sizeof(long));
  memcpy(tmp+sizeof(long),&tam,sizeof(long));
  memcpy(tmp+(sizeof(long)*2),&modo,sizeof(long));
  memcpy(tmp+(sizeof(long)*3),&id_grid,sizeof(int));
  if (DespachadorExterno)
	(*DespachadorExterno)((int)DESPACHO_BINARIO,(int)1,tmp);  

  if (!RetroPar[mi_punto])
	  return mi_punto;  


  if (par->ipfi < 0 || (par->ipfi < nfi && mpfi[par->ipfi] == par->pfi)) {  
	  pfi = par->pfi;

	  for (c = 0;c < pfi->f_ncampos;c++)
	  {
		 pfi->f_campos[c].c_mante.c_lineal &= ~FGV_C_GRID;
		 pfi->f_campos[c].c_mante.c_lineal &= ~FGV_C_GRIDSTATICO;	 
		 pfi->f_campos[c].c_mante.c_visual &= ~0x4000;
	  }

  
	  pfi->f_idgrafico = 0;
	  pfi->f_puntoretropar = 0;

	  if (pfi->f_campogids)
	  {
		  free((char *)pfi->f_campogids);
		  pfi->f_campogids = NULL;
	  }  

	  info = par->InfoCampos;

	  //!(flag & 1) && pfi->f_campogids
	  for (i = 0;i < info->n_campos;i++)
	  {
		   campo = dInfoCampo(info,i,0);
		   pfi->f_taborder[i] = campo->p_actual;
		   /*
		   pfi->f_campos[i].c_mante.c_actual = campo->p_actual;
		   pfi->f_campos[campo->Id].c_mante.c_actual = campo->l_actual;
		   */
	  }
  
	  if (par->campowids)
	  {
		  for (i = 0;i < pfi->f_ncampos;i++)
		  {
			  pfi->f_campowids[i] = par->campowids[i];
		  }
	  }

	  pfi->f_npanta = info->p_npan;            
	  if (info->p_npan > 0)
	  {
		   if (pfi->f_pantalla) {
			   pfi->f_pantalla[0].p_campoini = info->p_campini;
			   pfi->f_pantalla[0].p_campofin = info->p_campfin;
		   }
	  }
	  else
	  {
		   if (info->p_campfin == -2)
		   {                    
				free((char *)pfi->f_pantalla);
				pfi->f_pantalla = NULL;            
		   }
	  }
  }

  if (par->mapa_de_ordenacion)
  {
	  free((char *)par->mapa_de_ordenacion);
	  par->mapa_de_ordenacion = NULL;
  }

  /*
  if (par->mapa_de_asignacion)
  {
	  free((char *)par->mapa_de_asignacion);
	  par->mapa_de_asignacion = NULL;
  }
  */
  
  if (par->for_free)
  {
     free(par->for_free);
     par->for_free = NULL;
  }

  if (par->campowids)
  {
     free(par->campowids);
     par->campowids = NULL;
  }
  
  if (par->cache)
  {
      for (i = 0;par->cache[i].id_reg;i++)
      {
          free((char *)par->cache[i].buf);          
      }
      free((char *)par->cache);
      par->cache = NULL;
  }
  
  
  FinRetroPar(mi_punto);

  
  if (par->b_lim1)
  {
      free(par->b_lim1);
      par->b_lim1 = NULL;
  }

  
  if (par->b_lim2)
  {
      free(par->b_lim2);
      par->b_lim2 = NULL;
  }

  
  if (par->colcampos)
  {
      free(par->colcampos);
      par->colcampos = NULL;
  }

  if (par->claves_validas)
  {
	  free(par->claves_validas);
	  par->claves_validas = NULL;
  }

  if (par->totales)
  {
	  free(par->totales);
	  par->totales = NULL;
  }
  if (par->t_totales)
  {
	  free(par->t_totales);
	  par->t_totales = NULL;
  }
  
  if (par->multiples_ficheros) {	  
	  libera_dir(par->multiples_ficheros);	  
	  par->multiples_ficheros= NULL;
  }

  if (par->pfi)
  {
		if (par->pfi->f_fd > -1)
		{			
			if (par->temporal)
			{
				cierra_ind(par->pfi);
				rw_base_unlink(par->pfi->f_fichero);
				borra_ind(par->pfi);
			}
		}		
  }


  if (par->Independiente)
  {
      if (par->ipfi > -1)
        _salva_datos_unlock(par->ipfi);
      free(par->titulo);
      free((char *)par);
  }
  mi_punto = -1;

  
  /*pulsatecla();*/
  

  
  return mi_punto;
}

static long _rrrr;

int consul_clave_grid(pfi,ipfi,encl,numpan,limi1,limi2,filtro,pipfi,campo,ffiltro)
FI_DEF *pfi;
int ipfi,encl,numpan,limi1,limi2,filtro,pipfi,campo;
char *ffiltro;
{
int oc;
char *buf;
struct gridpars par;
char titulo[256];
int punto = -1;
int salida = -1;
int tecla = 807;
long resultado;
int nopintar = 0;
int esremota = 0;
int locked = 0;
int aviso_a_campo = -1;
int otro_campo;
int remotear = 0;
static char **elfiltro = NULL;
SALVA_IDS


  if (!pfi)
  {
	  locked = 1;
	  if (ipfi == -1)
	  {
		  esremota = 1;
		  ipfi = 0;
	  }
	  _salva_datos_lock(ipfi);
	  pfi = v_mpfi(ipfi);	    
  }
  else
  {
	  nopintar = 1;
  }

  oc = ind_clave_activa(pfi) - 1;
  buf = malloc(pfi->f_lfich + 1);

  if (!esremota && eqj == 2 && (pipfi < 0 || campo < 0))
  {
	  eqj = 0;
	  return -1;
  }  


  memcpy(buf,pfi->f_buf,pfi->f_lfich);
  strcpy(titulo,"Consulta ");
  strcat(titulo,pfi->f_titulo);

  encl--;  

  if (!esremota)
  {
      if (filtro_consulta_remota)
	  {
		  if (!elfiltro)
		  {
	          carga_filtro_x(filtro_consulta_remota,&elfiltro);
		  }
		  if (elfiltro)
		  {		
				int ok = 0;
				int i;
				
				for (i = 0;elfiltro[i];i++)
				{
					if (!strcmp(elfiltro[i],"*"))
					{
						ok = 1;
						break;
					}
					else
					if (strstr(pfi->f_fichero,elfiltro[i]))
					{
						ok = 1;
						break;
					}
				}
				if (ok)
				{
					remotear = 1;
				}
		  }
	  }
  }




  memset((void *)&par,0,sizeof(par));

  par.structid = 0x3212;
  par.sesion_id = ap_sesion_id;
  par.titulo = titulo;   
  par.pfi    = pfi;
  par.ipfi   = -1;
  par.encl   = encl;
  par.limi1  = limi1;
  par.limi2  = limi2;
  par.b_lim1 = NULL;
  par.b_lim2 = NULL;
  if (ipfi > -1)
  {
	  par.hook   = ejecuta_otro(ipfi,77,O_NOR,0,1);
	  if (par.hook < 1)
		  par.hook = 0;
  }
  else
  {
	  par.hook   = 0;
  }
  par.gmodo  = 0;   
  par.pi     = 0;
  par.pf     = 0;
  par.padre  = NULL;
  par._id    = default_id;
  par._sid   = 0;/*default_sid; /*ipfi+1+(numpan * 1000);*/
  par.Independiente = 0; /* la consulta no es independiente */
  if (ffiltro)
  {
	  strcpy(par.autofiltro,ffiltro);
  }
  
  if (par._sid == default_sid)
     par._wid   = default_wid;
  else  
     par._wid   = -1;


  /* salvar flag visual */
  if (esremota)
  {
	 if (pipfi > 0 && campo > 0)
	 {
		 par.gmodo = 4+8+16/*+64*/+2048 /*0x800*/;
		 par._sid = 0;
		 par.pi     = pipfi;
		 par.pf     = campo;
	 }
  } 
  else if (eqj == 2)
  {  
	 int nl;
	 nopintar = 1;

	 par.gmodo = 4+8+16/*+64*/+2048 /*0x800*/;
	 if (numpan < 0)
	 {
		 numpan = 0;
	     par._sid  = default_sid; /*pipfi+1+(numpan * 1000);*/
	 }
	 else
	 {
		 par._sid  = pipfi+1+(numpan * 1000);
	 }
	 par.pi    = v_mpfi(pipfi)->f_campos[campo].c_mante.c_qp + 100;     
     par.pf    = v_mpfi(pipfi)->f_pantalla[numpan].p_posref + (v_mpfi(pipfi)->f_pantalla[numpan].p_posfin-101);
	 aviso_a_campo = v_mpfi(pipfi)->f_campowids[campo];
	 if ((v_mpfi(pipfi)->f_campos[campo].c_mante.c_lineal & FGV_C_GRID))
	 {
		 aviso_a_campo = -1;
	 }
	 
     par.control_ipfi = pipfi + 1;      
	 par.control_campo = campo+1; 	 

	 nl = (par.pf%100) - (par.pi%100);
	 if (nl < 35)
	 {
		 if ((par.pi%100) < (36-nl))
		 {
			 par.pi = par.pi/100*100 + 1;
		 }
		 else
		 {
			 par.pi -= (35-nl);
		 }
	 }
	 nl = (par.pf/100) - (par.pi/100);
	 if (nl < 5)
	 {
		 par.pf = par.pf%100 + ((par.pi/100)+5)*100;
	 }
  }

  punto = LanzaGrid(&par,NULL,NULL,NULL,NULL,NULL,NULL,NULL);

  if (punto < 0)
  {
      tecla = 807;
  }
  else
  {
      do
      {		 
		 if (aviso_a_campo != -1)
		 {
			tecla = leecar_foco(1,-1/*default_id*/,-1,-1,-1,-1,par.id_grid,NULL,NULL);
			if (tecla == 999)
			{
				otro_campo = default_cid & 0xffff;
				if (default_refcid == pipfi && otro_campo >= 0 && otro_campo < v_mpfi(pipfi)->f_ncampos && otro_campo != campo)
				{
					if (v_mpfi(pipfi)->f_camporden[otro_campo] != -1)
					{
						_ID_Cursor =  pipfi + (v_mpfi(pipfi)->f_camporden[otro_campo]+1)*1000;
						tecla = 806;
						TestigoRaton = 1;
					}
				}
			}			
		 }
		 else
		 {
			(*DespachadorExterno)((int)DESPACHO_DIF_FOCOCONTROL,par.id_grid,NULL);
			tecla = leecar();
		 }
      } while(tecla != 802 && tecla != 806 && tecla != 807 && tecla != TECLA_COMBO);
  }

  /* reponer flag visual */

  encl = par.encl;

  resultado = par.regsel;
  
  BorraGrid(punto);

  if (aviso_a_campo != -1)
  {
	 strcpy(titulo,"CUSTOM1002"); /* Desactivar estado 'drop' */
	 if (DespachadorExterno)
		(*DespachadorExterno)((int)DESPACHO_ESTADOCONTROL,aviso_a_campo,(void *)titulo);  
  }

punto_s1:

  if (esremota)
  {
	  _rrrr = resultado;
	  salida = tecla;
  }
  else if (tecla == VALIDAR || tecla == CRETURN)
  {      
      salida = 0;
      pon_registro_actual(pfi,resultado);
      apunta_ind(pfi,0,ISEQUAL,0);
      lee_reg(pfi,ISCURR);
      apunta_ind(pfi,encl+1,ISCURR,0);
  }


  if (!esremota)
  {
	  if (!salida) 
	  {
		  if (numpan != -1 && !nopintar)
		  {
			printpan(ipfi,numpan);
		  }
	  }
	  else 
	  {
		   memcpy(pfi->f_buf,buf,pfi->f_lfich);
	  }
	  if (oc != encl || salida) 
		  {
		  apunta_ind(pfi,oc+1,ISCURR,0);
		  }
  }

  if (locked)
  {
 	  _salva_datos_unlock(ipfi);
  }

  free(buf);
  

  eqj = 0;

  REPON_IDS

  return (salida);
}


int consulta_local(char *puntero)
{
	FI_DEF *pfi = NULL;
	FI_DEF *a_pfi = NULL;
	int a_nfi;
	int tecla;
	char tmp[512];	
	FILE *f;
	char *tt;
	extern int flag_leedefn;
	

	puntero[51] = 0;	
	quitab(puntero);	
	puntero[52+51] = 0;
	quitab(puntero+52);	
	tmp[0] = 5;
	strcpy(tmp+1,rw_directorio("dch/"));
	strcat(tmp+1,puntero);
	
	flag_leedefn = 0;	
	if (lee_defn(tmp,&pfi) >= 0)
	{
	    int oe = en_conexion_terminal;
		en_conexion_terminal = 0;

		strcpy(pfi->f_fichero,rw_directorio("dch/"));
		strcat(pfi->f_fichero,puntero+52);			

		a_pfi = mpfi[0];
		mpfi[0] = pfi;
		a_nfi = nfi;
		nfi = 1;	


		if (abre1_rel(0) >= 0)
		{		
			long updatecount = 0L;
			{
			   int i;
			   i = pfi->f_fd;
			   if (i >= 0 && i < IXXMAXFILES && ixxfichs[i])
			   {
				  updatecount = ixxfichs[i]->cabecera.numero_transaccion;
			   }	  			   
			}
			if (atol(puntero+52+52+18) == updatecount)
			{
				tecla = consul_clave_grid(NULL,-1,atoi(puntero+52+52)+1,0,0,0,0,atoi(puntero+52+52+6),atoi(puntero+52+52+12),NULL);
				puntero[0] = 5;
				sprintf(puntero+1,"%9d %ld",tecla,_rrrr);
			}
			else
			{
				puntero[0] = 2;
			}
			cierra1_rel(0);
		}		
		else
		{
			puntero[0] = 2;
		}
		mpfi[0] = a_pfi;
		nfi = a_nfi;		
		free_defpri(pfi);
		en_conexion_terminal = oe;
	}
	else
	{
		puntero[0] = 1;
	}
	
	return 0;
}


static int compara_la_clave(key,ipfi,buf1)
int key,ipfi;
char *buf1;
{
			int i;
         int compara = 0;
         int orden,campo;
         
         if (!buf1) return(0);
         
         for (i = 0;i < v_mpfi(ipfi)->f_clave[key-1].k_ncampos;i++) 
         {
            campo = v_mpfi(ipfi)->f_clave[key-1].k_campo[i] - 1;
            orden = v_mpfi(ipfi)->f_clave[key-1].k_orden[i];
            compara = _campocmp(campo,orden,ipfi,v_mpfi(ipfi)->f_buf,buf1,0);
            if (compara) 
            {
               break;
            }
         }
         
         return(compara);
}


/*#else*/

int consul_clave(ipfi,encl,numpan,limi1,limi2,filtro,pipfi,campo,ffiltro)
int ipfi,encl,numpan,limi1,limi2,filtro,pipfi,campo;
{	
    return(consul_clave_grid(NULL,ipfi,encl,numpan,limi1,limi2,filtro,pipfi,campo,ffiltro));
}

lista_error() /* 0 = sin error 1 = errores leves  2 = errores grabes */
{
   if (iserrno == 0) return(0);
   if ( (iserrno > 0 && iserrno < 107) ||
      iserrno == 112 ||
      (iserrno > 113 && iserrno < 117) ) return(2);
   return(1);
}

pinta_error(f,o,ok)
int f;
char *o;
int ok;
{
   char buf[82];
   int i;

   if (iserrno == ELOCKED)
      sprintf(buf,"Ficha bloqueada por otro usuario");
   else
      if (iserrno == ENOREC)
         sprintf(buf,"Ficha Inexistente");
      else
         if (iserrno == EKEXISTS)
            sprintf(buf,"Ficha existente");
         else
            if (iserrno == EENDFILE)
               sprintf(buf,"Limite del fichero");	
            else
            {
               if (fecha_protegida)
               {
                  if (iserrno == 999)
                     sprintf(buf,"ALTA NO REALIZADA, DESMOSTRACION");
                  else
                     sprintf(buf,"[%d] NO AUTORIZADO EN DEMO",iserrno);
               }
               else
               {
                  if (iserrno == 999)
                     sprintf(buf,"LIMITE DE ENTRADAS DE LA VERSION");		   
                  else
                     sprintf(buf,"[%d]",iserrno);
               }
            }

     
    if (ok && saca_ptec() != -1) return 0;

    if (DespachadorExterno)
    {       
         char tmp[512];
         int *p1 = (int *)tmp;
         sprintf(tmp+sizeof(int)*2,"%-40.40s %-40.40s %-40.40s",buf,o,v_mpfi(f)->f_titulo);
         p1[0] = sizeof(int)*2 + strlen(tmp+sizeof(int)*2) + 2;
         p1[1] = ok;
         (*DespachadorExterno)((int)DESPACHO_MAINCOMMAND,ID_ERRORFICHERO,(void *)tmp);         
         agi_cola_timer();
    }    

   return(1);
}

captura_salida(r,libera)
int r;
int libera;
{
   int c;
   
   if (iserrno && (r == 2 || (r == 1 && iserrno == ELOCKED))) 
   {
	  sleep(1);
      c = pulsatecla();
      if (c == '\003' ) 
	  {
         mensaje(0,"Pulse 'N' ABORTA EL PROGRAMA");
         pito(33);pito(34);
         if (correcto() != 0) 
		 {
#ifdef ESDOS
#ifndef FGV32
            fin_todo_com();
#endif
#endif
            cierra_todo(1);
            fin_sig();
            Terfinal();
            p_serie();
            exit(2);
         }
         Bl(4);
         Bl(24);
      }
      return(-1);
   }
   if (libera == -1) parada();
   return(0);
}

int trata_1ficha(ipfi,clave,m,faclin)
int ipfi;
char *clave;
int m;
int faclin;
{
		 int leida,salida = 0,cerrar = 0,bloqueo = 0;
         int _operaerror = operaerror;

         letrascon = __Texto[363];
         textocon[0] = __Texto[361];
         textocon[1] = __Texto[362];
         
         if ((m / 100) == 1) {
            if ((leida = menusg(textocon,letrascon,0,"Contador: ",1)) == 2)
               return(-1);
            if (leida <= 0) return(2);
            bloqueo = ISLOCK;
         }
         if (v_mpfi(ipfi)->f_fd >= 0)
            cerrar = 0;
         else
            cerrar = -1;
         if (abre1_rel(ipfi) == 0) {
            defecto_ficha(v_mpfi(ipfi));
            leida = 0;
            if (strlen(clave)) {
               st_campo(0,v_mpfi(ipfi),clave);
               leida = leer_reg(ipfi,ISEQUAL,bloqueo,1);
               if (leida != 0) {
                  leida = grabar_reg(ipfi,3,0);
               }
            }
            if (leida == 0) {
               if (m == 0) {
                  prin_pan(ipfi,0);
                  if (entra_datos(MODIF,ipfi,0) < 0)
                     salida = -1;
               }
               else {
                  operaerror = 0;
                  p_ejecuta(m%100,O_NOR,faclin,29);
                  if (operaerror != 0) salida = -1;			
               }
               if (salida == 0 && strlen(clave)) grabar_reg(ipfi,0,0); /* cuidado */
            }
            libera_ind(v_mpfi(ipfi));   /* desbloquea el registro */
         }
         else
            return(-1);
         operaerror = _operaerror;
         if (cerrar == -1) cierra1_rel(ipfi);
         return(salida);
}


int cheq_clave(ipfi,campo,m)
int ipfi;
int campo;
int m;
{
   char clave[MAXDEFECTO];
   char qo[MAXDEFECTO];
   char clave1[MAXDEFECTO];
   int qj;
   
   Bl(24);
   while (-1) 
   {
	   if (DespachadorExterno)
	   {
		   (*DespachadorExterno)((int)DESPACHO_SUPERUSUARIO,(int)4,qo);
		   if (qo[0] == 1)
		   {
			   qo[0] = 0;
			   qj = 1;
		   }
		   else
			   qj = 0;
	   }
	   else
	   {
		  pprint(2427,__Texto[290]);
		  qj = entras(2444,v_mpfi(ipfi)->f_campos[campo].c_qc,"",-1,qo);
	   }
       Bl(24);
      if (qj == 7 || qj == 1) 
						{
         es_la_doble_ = 0;
         caracter_de_fecha = '.';
         return(-1);
						}
      else {
         if (va_con_doble_ && ipfi == 48)
         {		 
            if (qo[0] == '/')
            {
               es_la_doble_ = 1;
               caracter_de_fecha = '/';
               strcpy(qo,qo+1);
               strcat(qo," ");
            }
            else
            {
               caracter_de_fecha = '.'; 
               es_la_doble_ = 0;
            }		 
         }
         encrip(qo,clave);
         if (m == 0)
            ld_campo(campo,v_mpfi(ipfi),clave1);
         else
            st_campo(campo,v_mpfi(ipfi),clave);
         if (m == 0 && strcmp(clave1,clave) != 0) 
         {
            Ar();
            aviso(0,__Texto[77]);
            Cr();
            if (super_usuario)
               return(0);
         }
         else 
         {
            return(0);
         }
						}
   }
}

int system_proceso(char *manda)
{
   int ii,j,n;
   char **funs;
   VENT *w = NULL;
   
#ifndef FGVWIN
   
#ifdef RWSERVER
   if (!en_conexion_externa)
   {	 
#endif
						w = w_ini(101,2580);
                  w_salva(w);
#ifdef RWSERVER
   }
#endif
   
#endif
   n = 0;
   j = strlen(manda);
   tmp_s[0] = 0;
   for (ii = 0;ii < j;ii++)
   {
		if ((!ii || manda[ii-1] == ' ') && manda[ii] == ':' && manda[ii+1] == '/')
        {
                strcpy(tmp_s+n,rw_directorio(""));
                ii += 2;
                n = strlen(tmp_s);
        }
        tmp_s[n] = manda[ii];
        n++;
   }
   tmp_s[n] = 0;
   
   filtropar(tmp_s);
   fin_sig();
#ifndef FGVWIN
   if (w)
   {
						funs = salva_funciones(0,-1);                  
                  Terfinal();
   }
#endif
#ifdef FGV32
   /*
   if (fIsTNT)
   {
   FreeConsole();
   AllocConsole();
   }
   */
#endif
   
   agi_system(tmp_s);
   
#ifndef FGVWIN
   
#ifdef FGV32
   /*
   if (fIsTNT)
   {
   FreeConsole();
   AllocConsole();
   }
   */
#endif
   ini_sig();	     
   if (w)
   {
						Terinici();
                  repon_funciones(funs,0,-1);                  
                  w_mete(stdpan,w);
                  w_fin(w);
   }
#ifdef RWSERVER
   if (!en_conexion_externa)
#endif
						v_refresca();
   
#endif
   return(0);
}


int rw_system(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int i;
   
   if ((i = memoria_alfa(p)) > -1) 
   {
						if (prog[p].i != 128)
                  {
                     strcpy(v_memo(5004)->alfabetica,"");
#ifdef RWSERVER
                     if (en_conexion_externa)
                     {
                        char tmp[1024];
                        strcpy(tmp,v_memo(i)->alfabetica);
                        funcion_windows_cliente(15,tmp,strlen(tmp)+1);          
                        recoge_ip_remota(v_memo(5004)->alfabetica);
                        return(0);
                     }		
#endif
                  }      
                  
                  system_proceso(v_memo(i)->alfabetica);
   }
   
   return(0);
}



int rw_instala(p,tip,faclin)
int p;
int tip;
int faclin;
{
   struct info_paquete *info;
   int i;
   struct stat buf;
   char tmp[256];
   char *paquete = v_memo( prog[p].indice )->alfabetica;

   /* SININSTALACION */
   n_np =1;
   return(0);
   
   quita_blancos(paquete);
   for (i = 0;i < nPaq;i++)
   {
      if (!strcmp(paquete,Paq[i].nompaq))
      {
         break;
      }
   }
   if (i >= nPaq)
   {
      strcpy(v_memo(5004)->alfabetica,"-1"); 
      n_np =1;
      return(0);
   }
   
   info = instala_paquete(paquete,NULL);
   if (info)
   {
      Paq[i].control_utime = info->build_numero;
      strcpy(Paq[i].nompaq,paquete);
      strcpy(tmp,rw_directorio("master/"));
      strcat(tmp,paquete);  
      strcat(tmp,".tgz");     
      if (ultima_instalacion == 1)
      {      
		 Paq[i].flag = 3; /* instalado */
         if (!(info->build_numero))
		 {
			if (stat(tmp,&buf) != -1)
			{
				Paq[i].control_utime = buf.st_mtime;
			}
			else
			{				
				Paq[i].control_utime = 0;
				Paq[i].flag = -1; /* sin tgz */
			}
		 }
         else
            Paq[i].control_utime = info->build_numero;         
      }
      else
      {
         aviso(0,__Texto[78]);
         Paq[i].flag = 2; /* por instalar */
      }
      memcpy(&(Paq[i].i),info,sizeof(Paq[i].i));
      sprintf(v_memo(5004)->alfabetica,"%d",i);
   }
   else
   { 
      strcpy(v_memo(5004)->alfabetica,"-1");
   }
   
   
   n_np =1;
   return(0);
}

int rw_datos_paquete(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int lin = pasa_a_int(v_memo( prog[p++].indice )->numerica);
   int desc = prog[p++].indice;
   int codi = prog[p++].indice;
   int ver  =  prog[p++].indice;
   int est  = prog[p++].indice;
   char tmp[81];
   
   if (lin < 0 || lin >= nPaq)
   {
      strcpy(v_memo(5004)->alfabetica,"-1");
   }
   else
   {  
      memo_strcpy(desc,Paq[lin].i.descrip);
      memo_strcpy(codi,Paq[lin].nompaq);
      sprintf(tmp,"%2d.%02d.%03d",Paq[lin].i.version1,Paq[lin].i.version2,(int)Paq[lin].i.versionletra);
      memo_strcpy(ver,tmp);
      v_memo( est )->numerica = (double)Paq[lin].flag;
      strcpy(v_memo(5004)->alfabetica,"0");
   }
   
   n_np = 5;
   return(0);
}

int rw_bloquea_master(p,tip,faclin)
int p;
int tip;
int faclin;
{
   /* SININSTALACION */
   /*if (recur_punto > 0)*/
   {
      sprintf(v_memo(5004)->alfabetica,"%d",-1);
   }
   /*
   else
   {
      sprintf(v_memo(5004)->alfabetica,"%d",carga_master(1));
   }
   */
   n_np = 1;
   return(0);
}

int rw_desbloquea_master(p,tip,faclin)
int p;
int tip;
int faclin;
{
	/* SININSTALACION */
   /*
   descarga_master();
   */
   n_np = 1;
   return(0);
}


static int nuevo_disco(char *tmp,int mi_volume,int preverr,char *origen)
{
   char tmp3[256];
   int j;
   
   sprintf(tmp3,__Texto[199],mi_volume,tmp);
   mensaje(0,tmp3);pito(35);
   
   /* posibilidad de cambio */
   strcpy(tmp3,origen);
   do {
      pprint(2401,__Texto[291]);
      j = entra(2435,32,"","",tmp3,1,tmp3);
      if (j == 1 || j == 7)
      {
         if (confirmar(2440,__Texto[434],__Texto[448]))
         {
            return(-1);
         }
      }
      quitab(tmp3);
      if (strlen(tmp3) && !j && !correcto()) break;			
      if (!tmp3[0])
      {
         strcpy(tmp3,origen);
      }
   } while(-1);
   cambia_barra(tmp3);
   j = strlen(tmp3);
   j--;		   
   if (tmp3[j] != ':' && tmp3[j] != '/')
   {
      strcat(tmp3,"/");
   }
   strcpy(origen,tmp3);
   Ap();
   mensaje(0,__Texto[200]);
   Cp();
   Bl(24);
   
   return(mi_volume);
}


int pon_la_marca(char *fich,long la_marca)
{
   long posi;
   int file;
   char tmp[256];
   
   file = rw_open_(fich,O_RDWR | O_BINARY);
   if (file >= 0)
   {
      posi = rw_lseek_(file,0L,SEEK_END);
      rw_lseek_(file,posi-13,SEEK_SET);
      rw_read_(file,tmp,13);
      if (!memcmp(tmp,"011298",6) && !memcmp(tmp+10,"FGV",3))
      {
         rw_lseek_(file,posi-13,SEEK_SET);
      }
      else
      {
         rw_lseek_(file,0L,SEEK_END);
      }
      memcpy(tmp,"011298",6);
      memcpy(tmp+6,&la_marca,4);
      memcpy(tmp+10,"FGV",3);
      rw_write_(file,tmp,13);
      rw_close_(file);
   }
   
   return(0);
}


int rw_copiainstalacion(p,tip,faclin)
int p;
int tip;
int faclin;
{
   struct info_paquete *info;
   char origen[256];
   char destino[256];
   char paquete[80],descrip[80],fech[80],especial[80],agisy[80];
   char aborrar[256];
   unsigned char tmp[256];
   char tmpt[256];
   unsigned short nume,vers;
   int i,totali,ext,j,err,mi_volume,preverr,iii;
   char *lista_instala[100];
   int nlista = 0;
   FILE *file;
   struct stat buf;

/* SININSTALACION */
   n_np = 1;
   return(0);

   
   strcpy(origen,v_memo( prog[p].indice )->alfabetica);
   quita_blancos(origen);
   strminus(origen);
   
   aborrar[0] = 0;
   instalacion_silenciosa = 0;
   Bl(24);
   if (!origen[0])
   {
      strcpy(origen,"a:");
      do {
         pprint(2401,__Texto[292]);
         i = entra(2433,32,"","",origen,1,origen);
         if (i == 1 || i == 7)
         {
            if (confirmar(2440,__Texto[435],__Texto[448]))
            {
               n_np = 1;
               Bl(24);
               return(0);		   
            }
         }
         if (i || correcto()) continue;	       
         quita_blancos(origen);
         strminus(origen);
         if (origen[0])
         {
            break;
         }
      } while(-1);
   }
   Bl(24);
   i = strlen(origen);
   
   if (i)
   {
      if (i < 5 || memcmp(origen+i-4,".tgz",4))
      {     
         i--;
         if (origen[i] != '/' && origen[i] != '\\' && origen[i] != ':')
            strcat(origen,"/");
      }
   }
   else
   {
      strcpy(origen,"./"); 
   }
   cambia_barra(origen);
   if (!strcmp(origen,"./"))
   {
      /* coger el directorio por defecto */
      getcwd(origen,sizeof(origen));	  
      i = strlen(origen);
      if (i)
      {
         i--;
         if (origen[i] != '/' && origen[i] != '\\')
            strcat(origen,"/");
      }
      cambia_barra(origen);
   }
   
   strcpy(destino,rw_directorio("master/"));
   quita_blancos(destino);
   strminus(destino);
   if (!memcmp(destino,origen,strlen(destino)))
   {
      aviso(0,__Texto[79]);
      n_np = 1;
      Bl(24);
      return(0);
   }
   
   strcpy(destino,rw_directorio("tmp/"));
#ifdef FGVUNIX
   mkdir(destino,0xffff);
#else
   mkdir(destino);
#endif
   
   i = strlen(origen);
   i--;
   Ap();
   mensaje(0,__Texto[200]);
   Cp();		    
   
   if (origen[i] == '/' || origen[1] == ':')
   {
      /* buscar y leer volume.ds */      
      sprintf(tmpt,"%svolume.ds",destino);
      unlink(tmpt);
      sprintf(tmpt,"%svolume.agi",destino);
      unlink(tmpt);
#ifdef AGIUNIX
      if (origen[1] == ':') /* caso disquette dos en unix */
      {   
         sprintf(tmp,"%s volume.agi %s %s",rw_directorio("bin/agcopiaa"),tmpt,origen);
         agi_system(tmp);
         sprintf(tmpt,"%svolume.ds",destino);
         sprintf(tmp,"%s volume.ds %s %s",rw_directorio("bin/agcopiaa"),tmpt,origen);
         agi_system(tmp);
      }
      else	 
      {
#endif
         sprintf(tmp,"%svolume.agi",origen);	 		 
         copiarfich(tmp,tmpt,0);
         sprintf(tmpt,"%svolume.ds",destino);
         sprintf(tmp,"%svolume.ds",origen);	 		 
         copiarfich(tmp,tmpt,0);
#ifdef AGIUNIX
      } 
#endif
      file = fopen(tmpt,"r");
      if (!file)
      {
         sprintf(tmpt,"%svolume.agi",destino);
         file = fopen(tmpt,"r");
         if (!file)
         {
            sprintf(tmpt,"%svolume.ds",destino);
         }
      }
      if (!file)
      {
         /* intentar buscar infopaq.agi */
         sprintf(tmpt,"%sinfopaq.agi",destino);
         unlink(tmpt);
#ifdef FGVUNIX
         if (origen[1] == ':') /* caso disquette dos en unix */
         {   
            sprintf(tmp,"%s infopaq.agi %s %s",rw_directorio("bin/agcopiaa"),tmpt,origen);
            agi_system(tmp);
         }
         else	 
         {
#endif
            sprintf(tmp,"%sinfopaq.agi",origen);
            copiarfich(tmp,tmpt,0);	  	  
#ifdef FGVUNIX
         }
#endif
         file = fopen(tmpt,"r");
         if (file)
         {
            lee_linea(tmp,250,file); /* paquete */
            quita_blancos(tmp);
            strcpy(especial,tmp);		 
            lee_linea(tmp,250,file); /* version */
            quita_blancos(tmp);
            tmp[0] -= 65;
            tmp[1] -= 65;
            tmp[2] -= 65;
            tmp[3] -= 65;
            vers = ((unsigned short)tmp[0] * 26 * 26 * 26) + ((unsigned short)tmp[1] * 26 * 26) +
               ((unsigned short)tmp[2] * 26) + ((unsigned short)tmp[3]);                
            lee_linea(tmp,250,file); /* numero */
            quita_blancos(tmp);
            tmp[0] -= 65;
            tmp[1] -= 65;
            tmp[2] -= 65;
            tmp[3] -= 65;
            nume = ((unsigned short)tmp[0] * 26 * 26 * 26) + ((unsigned short)tmp[1] * 26 * 26) +
               ((unsigned short)tmp[2] * 26) + ((unsigned short)tmp[3]);                
            lee_linea(tmp,250,file); /* discos */
            totali = atoi(tmp);
            lee_linea(tmp,250,file); /* descripcion */
            strcpy(descrip,tmp);
            lee_linea(tmp,250,file); /* fecha */
            strcpy(fech,tmp);           
            lee_linea(tmp,250,file); /* tamaño */
            lee_linea(tmp,250,file); /* fichero */
            strcpy(paquete,tmp);
            quita_blancos(paquete);
            lee_linea(tmp,250,file); /* ver agisys-diagram */
            strcpy(agisy,tmp);           
            fclose(file);
            unlink(tmpt);
            /* hacer la conversion */
            sprintf(tmp,"%s%s.rwi",destino,paquete);
            file = fopen(tmp,"wt");
            if (file)
            {
               fprintf(file,"[Version]\nP,X,%d,%02d,a,%s,%s\n",vers/100,vers%100,fech,descrip);
               fprintf(file,"[Control]\n%d,%d,%d,%d,a,1,0,0,a,0,0,Formato Antiguo\n",vers,nume,atoi(agisy)/100,atoi(agisy)%100);
               fprintf(file,"[Fijos]\n%s/*\n",especial);
               fprintf(file,"[Submenu]\n%s/%s/%s.opc\n",especial,fichdir,especial);
               fprintf(file,"[Preguntas]\n0!Clave de la Aplicacion:,20,,,%%P\n");
               fprintf(file,"[PonStdIni]\nInsPaq??=%%0\n");
               fprintf(file,"[TextoInicial]\nInstalacion de %s\n",descrip);
               fclose(file);
            }	 
            
            /* preparar */
            /***********************/
            
            sprintf(tmpt,"%s%s.Z",destino,paquete);
            unlink(tmpt);
            err = 0;
            for (i = 0;i < totali;i++)
            {
               if (err || i)
															{
                  sprintf(tmp,"%s.Z%02d",paquete,i+1); 
                  if (nuevo_disco(tmp,i+1,err,origen) < 0)
                  {
                     i = 0;
                     break;
                  }
															}
               Ap();
               mensaje(0,__Texto[201]);
               Cp();		    
#ifdef FGVUNIX
               if (origen[0] && origen[1] == ':') /* caso disquette dos en unix */
															{
                  if (!i)
                     sprintf(tmp,"%s %s.Z%02d %s %s",rw_directorio("bin/agcopiaa"),paquete,i+1,tmpt,origen);
                  else
                     sprintf(tmp,"%s %s.Z%02d %s%s.Z%02d %s",rw_directorio("bin/agcopiaa"),paquete,i+1,destino,paquete,i+1,origen);
                  system(tmp);
                  if (i)
                  {
                     sprintf(tmp,"%s%s.Z%02d",destino,paquete,i+1);
                     err = copiarfich(tmp,tmpt,1);
                     unlink(tmp);
                  }
                  else
                  {
                     err = stat(tmpt,&buf);
                  }
															}
               else
															{
#endif
                  sprintf(tmp,"%s%s.Z%02d",origen,paquete,i+1);
                  if (!i)
#ifdef FGVDOS
                     err = copiafich_win(tmp,tmpt);
#else
                  err = copiarfich(tmp,tmpt,0);
#endif
                  else
                     err = copiarfich(tmp,tmpt,1);				
#ifdef FGVUNIX
															}
#endif			
               if (err == -1)
															{
                  i--;
                  continue;
															}
               else
                  err = 0;
            }
            if (i >= totali)
            {
               Ap();
               mensaje(0,__Texto[202]);
               Cp();
               agi_compress(tmpt,1);
               unlink(tmpt);
               /* */
               Ap();
               mensaje(0,__Texto[203]);
               Cp();
               
               sprintf(tmp,"%s%s.tar %s.rwi",destino,paquete,paquete);
               agi_atar(tmp,destino);
               sprintf(tmp,"%s%s.tar",destino,paquete);
               sprintf(tmpt,"%s%s",destino,paquete);
               copiarfich(tmpt,tmp,1025);
               unlink(tmpt);
               Ap();
               mensaje(0,__Texto[204]);
               Cp();
               agi_compress(tmp,0);
               unlink(tmp);
               nlista = 1;		 
               lista_instala[0] = malloc(strlen(paquete)+1);
               strcpy(lista_instala[0],paquete);
               sprintf(tmp,"%s%s.rwi",destino,paquete);
               unlink(tmp);
               sprintf(aborrar,"%s%s.tgz",destino,paquete);
               paquete[0] = 0;
               especial[0] = 0;
            }
            strcpy(origen,destino);
        }
      }
      else
      {
         while(lee_linea(tmp,250,file))
         {
            for (iii = 0;tmp[iii] && tmp[iii] != ';';iii++);
            tmp[iii] = 0;
            quita_blancos(tmp);
            if (!tmp[0]) continue;
            for (iii = 0;tmp[iii] && tmp[iii] != '.';iii++);
            if (!strcmp(tmp+iii,".ini"))
            {
               continue;
            }		 
            lista_instala[nlista] = malloc(strlen(tmp)+1);         
            strcpy(lista_instala[nlista],tmp);
            nlista++;
         }
         fclose(file);
         unlink(tmpt);
      }   
   }
   else
   {
      nlista = 1;
      lista_instala[0] = malloc(strlen(quita_path(origen))+1);
      strcpy(lista_instala[0],quita_path(origen));
      lista_instala[0][strlen(quita_path(origen))-4] = 0;
      *quita_path(origen) = 0;
   }
   
   strcpy(destino,rw_directorio("master/"));
   
   ext = -1;
   err = 0;
   preverr = 0;
   mi_volume = 1;
   for (i = 0;i < nlista;i++)
   {   
      Ap();
      mensaje(0,__Texto[205]);
      Cp();     
      err = 0;
      ext = strbusca(lista_instala[i],".");
      if (ext != -1)
      {
         lista_instala[i][ext] = 0;
      }
      strcpy(especial,lista_instala[i]);
      sprintf(tmpt,"%s%s.tgz",destino,lista_instala[i]);
      if (ext != -1)
      {
         lista_instala[i][ext] = '.';
      }
#ifdef FGVUNIX
      if (origen[0] && origen[1] == ':')
      {
         if (ext != -1)
            sprintf(tmp,"%s %s %s %s",rw_directorio("bin/agcopiaa"),lista_instala[i],tmpt,origen);
         else
            sprintf(tmp,"%s %s.tgz %s %s",rw_directorio("bin/agcopiaa"),lista_instala[i],destino,origen);
         agi_system(tmp);	   
         err = 0;
      }
      else
      { 
#endif
         if (ext != -1)
            sprintf(tmp,"%s%s",origen,lista_instala[i]);
         else
            sprintf(tmp,"%s%s.tgz",origen,lista_instala[i]);
         err = copiarfich(tmp,tmpt,0);
#ifdef FGVUNIX
      }
#endif
      if (err == -1)
      {
         if (!preverr)
         { 
            mi_volume++;
         }
         if (ext == -1)
            sprintf(tmp,"%s.tgz",lista_instala[i]);
         else
            sprintf(tmp,"%s",lista_instala[i]);
         if (nuevo_disco(tmp,mi_volume,0,origen) < 0)
         {
            i = 0;		
            unlink(tmpt);
            break;
         }	  
      }
      else
         err = 0;
      preverr = err;   
      if (err == -1)
      {
         i--;
         continue;
      }
      
      if (ext != -1) /* añadir el resto */
      {	   
         for (i++;i < nlista;i++)
         {
            ext = strbusca(lista_instala[i],".");
            if (ext == -1)
            {
               i--;
               break;
            }
            lista_instala[i][ext] = 0;
            if (strcmp(especial,lista_instala[i]))
            {
               lista_instala[i][ext] = '.';
               i--;
               break;
            }
            lista_instala[i][ext] = '.';		     
#ifdef FGVUNIX
            if (origen[0] && origen[1] == ':')
            {
               sprintf(tmp,"%s %s ",rw_directorio("bin/agcopiaa"),lista_instala[i]);
               sprintf(tmp+strlen(tmp),"%s%s %s",rw_directorio("tmp/"),lista_instala[i],origen);
               agi_system(tmp);		
               sprintf(tmp,"%s%s",rw_directorio("tmp/"),lista_instala[i]);
               err = copiarfich(tmp,tmpt,1);
               unlink(tmp);
            }
            else
            { 
#endif
               sprintf(tmp,"%s%s",origen,lista_instala[i]);
               err = copiarfich(tmp,tmpt,1);
#ifdef FGVUNIX
            }
#endif
            if (err == -1)
            {
               if (!preverr)
               { 
                  mi_volume++;
               }
               sprintf(tmp,"%s",lista_instala[i]);
               if (nuevo_disco(tmp,mi_volume,0,origen) < 0)
               {
                  unlink(tmpt);
                  i = nlista+1;
                  break;
               }
               i--;	
            }
            else
               err = 0;
            preverr = err;
            err = 0;
         }
      }   
      
      info = instala_paquete(especial,paquete);
      if (!info)
      {
         aviso(0,__Texto[80]);
      }
      else
      {
         for (j = 0;j < nPaq;j++)
         {
            if (!strcmp(especial,Paq[j].nompaq))
            {
               break;
            }
         }
         if (j >= nPaq) nPaq++;
         Paq[j].control_utime = 0L;
         Paq[j].flag = 2; /* instalado? */
         strcpy(Paq[j].nompaq,especial);
         strcpy(tmp,rw_directorio("master/"));
         strcat(tmp,especial);  
         strcat(tmp,".tgz");  	  
         if (ultima_instalacion == 1 && (stat(tmp,&buf) != -1 || (info->build_numero) ))
         { 
            if (!(info->build_numero))
               Paq[j].control_utime = buf.st_mtime;
            else
            {
               Paq[j].control_utime = info->build_numero;
               pon_la_marca(tmp,info->build_numero);
            }
            Paq[j].flag = 3; /* instalado */		
         }
         else
         {
            aviso(0,__Texto[81]);
         }
         memcpy(&(Paq[j].i),(char *)info,sizeof(struct info_paquete));
         /* Paq[j].i = *info; */
      }
   }
   
   if (!nlista)
   {
      aviso(0,__Texto[82]);
   }
   
   for (i = 0;i < nlista;i++)
   {
      free(lista_instala[i]);
   }
   
   if (aborrar[0])
   {
      unlink(aborrar);
   }
   Bl(24);
   n_np = 1;
   return(0);
}


int EntraLineaGrid(char *bloque,struct gridpars *par,int el_punto)
{
    int ipfi = par->ipfi;
    int alta;
    int modo_alta;
    int i,nclave;

	  i = par->curcol;
	  if (i >= 0 && i < v_mpfi(ipfi)->f_ncampos)
	  {
		  if (v_mpfi(ipfi)->f_campos[i].c_key != 1)
		  {
			  /*if (!v_mpfi(ipfi)->f_campos[i].c_mante.c_visual && v_mpfi(ipfi)->f_campos[i].c_mante.c_modif)*/
			  {
				  TestigoRaton = 1;
				  _ID_Cursor = ipfi + (par->curcol+1)*1000;
			  }
		  }
	  }

    do 
    {
		/*
		{
			int i;
			for (i = 0;i < 100;i++)
			{
				pulsatecla();
				Sleep(1);
			}
		}
		*/


		/*
		{
			char tmp[512];
			sprintf(tmp,"Registro:%ld",par->regcur);
			aviso(0,tmp);
		}
		*/
 	  

	  /*leecar();*/

      if (!par->regcur || par->regcur > par->registros || !par->registros)
      {
          if (par->b_lim1)
          {
                memcpy(par->pfi->f_buf,par->b_lim1,par->pfi->f_lfich);
          }
          else
          if (par->b_lim2)
          {
                memcpy(par->pfi->f_buf,par->b_lim2,par->pfi->f_lfich);
          }
          else
          {
                defecto_ficha(par->pfi);
          } 
		  if (par->lineas_ipfi > 0)
		  {
			  deferel_rel(par->ipfi,par->lineas_ipfi-1,-1,0,ALTA,-1);
		  }
          nclave = 0;/*par->encl; Tiene que ser la primaria */
		  if (par->regcur > par->registros || !par->registros)
              alta = ALTA;
          else
              alta = MODIF;
          if (pregun_clave(&nclave,ipfi,-1,alta,1) == SOLICITA_POSICION)
		  {
			  TestigoRaton = 1;
		  }
		  else
		  {
			  TestigoRaton = 0;
		  }

          if (nclave < 0)
          {
              break;
          }

          if (grabar_reg(ipfi,3,0) < 0) 
          {
              aviso(0,"Registro Existente");
              continue;
          }
      }
      else
      {
          alta = 0;
          pon_registro_actual(par->pfi,par->regsel );
          apunta_ind(par->pfi,0,ISEQUAL,0);
          apunta_ind(par->pfi,par->encl+1,ISCURR,0);


          if (leer_reg(ipfi,ISCURR,ISLOCK,0) < 0)
          {
              break;
          }          
          TestigoRaton = 1;
          _ID_Cursor = ipfi + (par->curcol+1)*1000;
          nclave = par->encl;
		  alta = MODIF;
          if (pregun_clave(&nclave,ipfi,-1,alta,1) == SOLICITA_POSICION)
		  {
			  TestigoRaton = 1;
		  }
		  else
		  {
			  TestigoRaton = 0;
		  }          
          if (nclave < 0)
          {
              libera_ind(par->pfi);
              continue;
          }          
      }

      if (alta == ALTA)
          modo_alta = ALTA;
      else
          modo_alta = MODIF;
      if (entra_datos(modo_alta,ipfi,-1) >= 0)
      {
          grabar_reg(ipfi,0,0);              
      }
      else
      {
          if (alta == ALTA)
          {
              borrar_reg(ipfi,0,20);
          }
      }
      libera_ind(par->pfi);
	  TestigoRaton = 0;

      if (par->cache) /* !!! */
      {
            for (i = 0;par->cache[i].id_reg;i++)
            {
                par->cache[i].id_reg = -1L;
            }
      }
      break;

   } while(-1);

   par->registros = TotalRegistros(par);
   memcpy(bloque,&(par->registros),sizeof(long));

   if (DespachadorExterno)
	(*DespachadorExterno)((int)DESPACHO_DIF_FOCOCONTROL,par->id_grid,NULL);

return 0;
}



int BorraLineaGrid(char *bloque,struct gridpars *par,int el_punto)
{
    int ipfi = par->ipfi;    
    int i;

    do 
    {
      if (!par->regcur || par->regcur > par->registros)
      {
		  aviso(0,"No hay registros para dar de baja");
          break;
      }
      else
      {
          if (confirmar(0,"Confirmacion baja (S/N) ","N") < 0)
		  {
			  aviso(0,"Baja Anulada"); 
              break;
		  }

          pon_registro_actual(par->pfi,par->regsel );
          apunta_ind(par->pfi,0,ISEQUAL,0);
          apunta_ind(par->pfi,par->encl+1,ISCURR,0);
          if (leer_reg(ipfi,ISCURR,ISLOCK,0) < 0)
          {
			  aviso(0,"No se puede bloquear el registro");              
              break;          
          }
          if (baja_fichao(ipfi) < 0) 
          {             
              aviso(0,"No se puede dar de baja la ficha");              
          }          
          libera_ind(par->pfi);
      }
      if (par->cache) /* !!! */
      {
            for (i = 0;par->cache[i].id_reg;i++)
            {
                par->cache[i].id_reg = -1L;
            }
      }
      break;

   } while(-1);

   par->registros = TotalRegistros(par);
   memcpy(bloque,&(par->registros),sizeof(long));

   if (DespachadorExterno)
	(*DespachadorExterno)((int)DESPACHO_DIF_FOCOCONTROL,par->id_grid,NULL);

return 0;
}


int BrowseData(int ipfi,int k,int flags,int pini,int pfin,int limi1,int limi2,int evento)
{
    struct gridpars par;
    int punto;
    int tecla;
    int nclave;
    int i;
    int alta;
	int rr = -1;


  _salva_datos_lock(ipfi);

  memset((void *)&par,0,sizeof(par));

	par.structid = 0x3212;
	par.sesion_id = ap_sesion_id;
  par.pfi   = v_mpfi(ipfi);
  par.encl   = k;
  par.limi1  = limi1;
  par.limi2  = limi2;
  if (!evento)
  {
	  par.hook   = ejecuta_otro(ipfi,77,O_NOR,0,1);
	  if (par.hook < 1)
		  par.hook = 0;
  }
  else
  {
 	  par.hook   = evento;
  }
  par.gmodo  = flags;   
  par.pi = pini;
  par.pf = pfin;
  par.padre = NULL;
  par.titulo = "";

  punto = LanzaGrid(&par,NULL,NULL,NULL,NULL,NULL,NULL,NULL);

  do
  {
      tecla = leecar();  
      if (tecla == 802)
      {          
          if (!par.regcur || par.regcur > par.registros)
          {
              defecto_ficha(par.pfi);
              nclave = par.encl;
              pregun_clave(&nclave,ipfi,-1,MODIF,1);
              if (nclave < 0) continue;
              if (grabar_reg(ipfi,3,20) < 0) {pito(36);continue;}
              if (par.regcur > par.registros)
                  alta = 2;
              else
                  alta = 1;
          }
          else
          {
              alta = 0;
              pon_registro_actual(par.pfi,par.regsel );
              apunta_ind(par.pfi,0,ISEQUAL,0);
              apunta_ind(par.pfi,par.encl+1,ISCURR,0);
              if (leer_reg(ipfi,ISCURR,ISLOCK,0) < 0)
              {
                  continue;
              }
              /*
              TestigoRaton = 1;
              _ID_Cursor = ipfi + (par.curcol+1)*1000;
              nclave = par.encl;
              pregun_clave(&nclave,ipfi,-1,MODIF,1);
              if (nclave < 0)
              {
                  libera_ind(par.pfi);
                  continue;
              }
              */
          }
          TestigoRaton = 1;
          _ID_Cursor = ipfi + (par.curcol+1)*1000;
          if (entra_datos(MODIF,ipfi,-1) >= 0)
          {
              grabar_reg(ipfi,0,0);              
          }
          else
          {
              if (alta)
              {
                  borrar_reg(ipfi,0,20);
              }
          }
          libera_ind(par.pfi);

          if (par.cache) /* !!! */
          {
                for (i = 0;par.cache[i].id_reg;i++)
                {
                    par.cache[i].id_reg = -1L;
                }
          }

          {
              char tmp[256];
              long modo = 1;
              long tam = (sizeof(long)*4)+sizeof(int);
              int id_grid = par.id_grid;
              int oper = 4;

              par.registros = tam_btrv(par.pfi);
              if (alta == 2)
              {
                  par.regcur = par.registros+1;
                  oper = 5;
              }

              memcpy_long(tmp,&tam,sizeof(long));
              memcpy_long(tmp+sizeof(long),&tam,sizeof(long));
              memcpy_long(tmp+(sizeof(long)*2),&modo,sizeof(long));
              memcpy_int(tmp+(sizeof(long)*3),&id_grid,sizeof(int));
              memcpy_long(tmp+(sizeof(long)*3)+sizeof(int),(void *)&par.registros,sizeof(long));
              (*DespachadorExterno)((int)DESPACHO_BINARIO,(int)oper,tmp);
          }
      }
      else
      if (tecla == 806 || tecla == 807)
      {
          break;
      }
  } while(-1);

  BorraGrid(punto);

  _salva_datos_unlock(ipfi);

  return rr;

}

int fgvsimplelineal(int p,int tip,int faclin)
{
    int k = prog[p].indice/100 - 1;
    int ipfi = v_prog(prog[p++].indice%100);
    int flags = (int)(unsigned long)v_memo(prog[p++].indice)->numerica;
    int pini = pasa_a_int(v_memo(prog[p++].indice)->numerica);
    int pfin = pasa_a_int(v_memo(prog[p++].indice)->numerica);
    int limi1 = prog[p++].indice;
    int limi2 = prog[p++].indice;
    int evento = prog[p++].indice;    
    int punto;
    int tecla;    
    int i;
    int alta;
    int modo_alta;
    int id_grid = -1;
    int oipfi = -1;
	int pan = 0;
	int alineas;
    struct gridpars par,*_par;
	char tmp[257];
	char *ffiltro = NULL;


    if (limi1 >= 0)
       limi1 = en_pr[programa_activo].tabla_gosubs[limi1];
    else
       limi1 = 0;
    if (limi2 >= 0)
       limi2 = en_pr[programa_activo].tabla_gosubs[limi2];
    else
       limi2 = 0;
    if (evento >= 0)
       evento = en_pr[programa_activo].tabla_gosubs[evento];
    else
       evento = 0;  

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

  _salva_datos_lock(ipfi);

  memset((void *)&par,0,sizeof(par));

  par.structid = 0x3212;
  par.sesion_id = ap_sesion_id;
  par.titulo = "";

  if ((flags & 128))
  {
	  flags &= ~2;
  }

  par.pfi   = v_mpfi(ipfi);
  par.ipfi  = ipfi;
  par.encl   = k;
  par.limi1  = limi1;
  par.limi2  = limi2;
  par.b_lim1 = NULL;
  par.b_lim2 = NULL;
  if (ffiltro)
  {
	  strcpy(par.autofiltro,ffiltro);
  }

  if (!evento)
  {
	  par.hook   = ejecuta_otro(ipfi,77,O_NOR,0,1);
	  if (par.hook < 1)
		  par.hook = 0;
  }
  else
  {
	  par.hook   = evento;
  }
  par.pact   = programa_activo;
  par.gmodo  = flags;   
  par.pi = pini;
  par.pf = pfin;
  par.padre = NULL;
  par.titulo = "";
  par._id    = default_id;
  par._sid   = default_sid;
  if (par._sid == default_sid)
     par._wid   = default_wid;
  else
     par._wid   = -1;


  if (!(flags & 32))
     _par = &par;
  else
  {
     _par = (struct gridpars *)malloc(sizeof(par));
     memcpy((char *)_par,(char *)&par,sizeof(par));
     _par->Independiente = 1;
     _par->titulo = malloc(strlen(par.titulo)+1);
     strcpy(_par->titulo,par.titulo);

	  oipfi = (_par->_sid % 1000) - 1;
	  pan = _par->_sid / 1000;
	  if (oipfi >= 0 && oipfi < nfi)
	  {	  
		  _par->control_ipfi = oipfi + 1;      
		  _par->control_campo = v_mpfi(oipfi)->f_ncampos + 1; 
	  }

  }

  punto = LanzaGrid(_par,NULL,NULL,NULL,NULL,NULL,NULL,NULL);

  if (!(flags & 32))
  {
      do
      {
          tecla = leecar_foco(1,v_mpfi(ipfi)->f_id,-1,-1,-1,-1/*v_mpfi(ipfi)->f_ipfi*/,_par->id_grid,NULL);
          if (tecla == 806 || tecla == 807)
          {
              break;
          }
      } while(-1);  

      BorraGrid(punto);

      _salva_datos_unlock(ipfi);
      strcpy(v_memo(5004)->alfabetica,"-1");
  }
  else
  {
	  int oipfi = _par->control_ipfi - 1;
      id_grid = _par->id_grid;

	  /* engancharlo como campo si es necesario!!!! */	  
	  if (oipfi >= 0 && oipfi < nfi)
	  {		  
		  int n = v_mpfi(oipfi)->f_ncampos;		  		  
		  int kk,i;
		  if (pan < 0) {
			  pan = v_mpfi(oipfi)->f_npanta-1;
		  }
		  if (pan > -1) {
			   for (kk = v_mpfi(oipfi)->f_pantalla[pan].p_campoini;kk <= v_mpfi(oipfi)->f_pantalla[pan].p_campofin;kk++)
			   {
					  i  = v_mpfi(oipfi)->f_taborder[kk-1];
					  if (v_mpfi(oipfi)->f_campowids[i] == -1) {
						  if (v_mpfi(oipfi)->f_campos[i].c_qc == 0 && v_mpfi(oipfi)->f_campos[i].c_minimo[0] == 0 && v_mpfi(oipfi)->f_campos[i].c_maximo[0] == 0) {
							  n = i;
							  i = kk;
							  while(i < v_mpfi(oipfi)->f_pantalla[pan].p_campofin) {
								  v_mpfi(oipfi)->f_taborder[i-1] = v_mpfi(oipfi)->f_taborder[i];
								  i++;
							  }
							  v_mpfi(oipfi)->f_taborder[i-1] = n;
							  break;
						  }
					  }
			   }
		  }
          _par->control_campo = n + 1;
		  if (n < v_mpfi(oipfi)->f_ncampos) {
              v_mpfi(oipfi)->f_campowids[n] = id_grid;
			  v_mpfi(oipfi)->f_orderflags[n] |= ORDER_FLAG_EVENTO;
		  }
		  else {
			  inserta_campo(v_mpfi(oipfi),n,"","",0,"","","");
			  v_mpfi(oipfi)->f_orderflags[n] |= ORDER_FLAG_EVENTO;
			  InsertaCampoEnPantalla(oipfi,pan,n,9999,id_grid);
		  }
	      v_mpfi(oipfi)->f_campos[n].c_mante.c_visual = 0;
		  v_mpfi(oipfi)->f_campos[n].c_mante.c_modif = 0;
		  if (DespachadorExterno && v_mpfi(oipfi)->f_nclaves)
		  {       
			 char tmp[512];
			 strcpy(tmp,"CUSTOM1001"); /* 1001 Desactivar mantenimento */
			 (*DespachadorExterno)((int)DESPACHO_ESTADOCONTROL,id_grid,(void *)tmp);
		  }    

	  }

      sprintf(v_memo(5004)->alfabetica,"%d",id_grid);
  }

  
  n_np = 7;
  return(0);
}

int FocoDeLineas(struct gridpars *par,int modo)
{
	int tecla;
	int ipfi = par->ipfi;
	int lineas = v_mpfirel(ipfi)->lineas;
	int campo = campo_linea(ipfi);
	int r = 0;

	if (campo >= 0 && campo < v_mpfi(ipfi)->f_ncampos)
	{
	  int k;
	  if (v_mpfi(ipfi)->f_campos[campo].c_mante.c_defecto)
	  {
		 k = atoi(v_mpfi(ipfi)->f_campos[campo].c_mante.c_defecto);		 
		 if (k == 0) 
			k = 1;
		 else
			if (v_mpfi(ipfi)->f_campos[campo].c_maximo && k == atoi(v_mpfi(ipfi)->f_campos[campo].c_maximo))
			   k = 1;
	  }
	  else
		  k = 1;

	  v_mpfirel(ipfi)->lineas = k;
	}


	if (modo != CONSULTA)
	  par->gmodo |= 1024;

      do
      {
          tecla = leecar_foco(1,v_mpfi(ipfi)->f_id,-1,-1,-1,-1/*v_mpfi(ipfi)->f_ipfi*/,par->id_grid,NULL);
          if (tecla == 806 || tecla == 807)
          {
              break;
          }
      } while(-1);  
	  

	if (modo != CONSULTA)
	{
	  par->gmodo &= ~1024;
	  if (DespachadorExterno)
	  {       
		 char tmp[512];
		 int id_grid = v_mpfi(ipfi)->f_idgrafico;
		 strcpy(tmp,"CUSTOM1001"); /* Desactivar mantenimento */
		 (*DespachadorExterno)((int)DESPACHO_ESTADOCONTROL,id_grid,(void *)tmp);
	  }		  
	  if (!par->registros)
	  {
		  r = -1;
	  }
	}

	v_mpfirel(ipfi)->lineas = lineas;
	return r;
}

int lineasgrid(p_ipfi,ipfi,modo,lim1,lim2,mensa_)
int p_ipfi,ipfi,modo;
char *lim1;
char *lim2;
int mensa_;
{    
    int flags = 0;
    int pini;
    int pfin;
    int evento = 0;
    int punto;    
    int i;
    int alta;
    int modo_alta;
    int id_grid = -1;
    int oipfi = -1;
	int pan = 0;
	int alineas;
    struct gridpars par,*_par;
	int k = da_cla_lin(ipfi);
	FI_DEF *pfi;
	int topefin;
	int r = 0;



  if (v_mpfi(ipfi)->f_idgrafico > 0)
  {
	  punto = v_mpfi(ipfi)->f_puntoretropar;
	  if (punto > 0)
	  {
		   _par = (struct gridpars*)RetroPar[punto];
		   pfi = _par->pfi;
		   defecto_ficha(pfi);
		   clave_lin_trata(ipfi,lim1,5,"");		   
		   memcpy(_par->b_lim1,pfi->f_buf,pfi->f_lfich);

		   defecto_ficha(pfi);
		   clave_lin_trata(ipfi,lim2,5,"");		   
		   memcpy(_par->b_lim2,pfi->f_buf,pfi->f_lfich);
	  }
  	  if (DespachadorExterno)
	  {
		   (*DespachadorExterno)((int)DESPACHO_REFRESCACONTROL,v_mpfi(ipfi)->f_idgrafico,NULL);
	  }
	  if (punto > 0)
	  {
		  _par = (struct gridpars*)RetroPar[punto];
		  if (modo != DIFERIDO && modo != CONSULTA && modo != VISUALIZA)
		  {
			  if (DespachadorExterno)
			  {       
				 char tmp[512];
				 int id_grid = v_mpfi(ipfi)->f_idgrafico;
				 strcpy(tmp,"CUSTOM1100"); /* 1100 Reponer mantenimento */
				 (*DespachadorExterno)((int)DESPACHO_ESTADOCONTROL,id_grid,(void *)tmp);
			  }		  
		  }
		  if (modo != DIFERIDO && modo != VISUALIZA)
		  {
			  r = FocoDeLineas(_par,modo);
		  }  
	  }
	  return r;
  }


   if (p_ipfi == -2) 
   {
      /*int xxx;*/
	   /*
      if (modo == DIFERIDO) 
        {
         rr = 0;
         goto SalirLimpio;
        }
	   */
      /*
      pinppan(f,0);
      */
        if (v_mpfi(ipfi)->f_pantaids[0] == -1)
        {
              pinppan(ipfi,0);
        }
        default_wid = v_mpfi(ipfi)->f_pantaids[0];
        default_sid = v_mpfi(ipfi)->f_pantasids[0];
   }
   else
   {
     if (p_ipfi >= 0)
     {  
        int plin = AjustaPanLin(p_ipfi,ipfi);
        
        pinppan(p_ipfi,plin);
        v_mpfi(ipfi)->f_pantaids[0] = v_mpfi(p_ipfi)->f_pantaids[plin];
        v_mpfi(ipfi)->f_pantasids[0] = v_mpfi(p_ipfi)->f_pantasids[plin];
        v_mpfi(ipfi)->f_id = v_mpfi(p_ipfi)->f_id;

		/*
        if (modo != DIFERIDO && modo != VISUALIZA) 
        {
              r = -1;          
        } 
	    */
     }     
     else
     {
        v_mpfi(ipfi)->f_pantaids[0] = default_wid;
        v_mpfi(ipfi)->f_pantasids[0] = default_sid;
        v_mpfi(ipfi)->f_id = default_id;
     }
     
   }


  ejecuta_otro(ipfi,5,O_NOR,0,0);

  _salva_datos_lock(ipfi);

  memset((void *)&par,0,sizeof(par));
  
  par.structid = 0x3212;
  par.sesion_id = ap_sesion_id;
  par.titulo = "";

  pfi = v_mpfi(ipfi);

  dame_cuadro_lineas(ipfi,&pini,&pfin);
  pini -= 200;
  pfin += 104;
  topefin = pfi->f_pantalla[0].p_posref + pfi->f_pantalla[0].p_posfin - 101;
  
  if ((pfin%100) > (topefin%100) || (pfin%100) <= (pini%100))
  {
	  pfin = pfin/100*100+topefin%100;
  }
  /*
  pini = pfi->f_pantalla[0].p_posref;
  pfin = pini + pfi->f_pantalla[0].p_posfin - 101;
  */  
  flags |= 512;
  flags |= 256;
  flags |= 128;
  flags |= 32;  
  flags |= 16;
  flags |= 8;
  flags |= 4;
  flags |= 1; /* todos son mantenimiento, que se desactiva oportunamente */
  

  if ((flags & 128))
  {
	  flags &= ~2;
  }

  pfi->f_flag |= 512;
  par.lineas_ipfi = p_ipfi + 1;
  par.pfi   = pfi;
  par.ipfi  = ipfi;
  par.encl   = k;
  par.limi1  = 0;
  par.limi2  = 0;
  par.b_lim1 = NULL;
  par.b_lim2 = NULL;
  par.lineas = v_mpfirel(ipfi)->lineas;

  defecto_ficha(pfi);
  clave_lin_trata(ipfi,lim1,5,"");
  par.b_lim1 = malloc(pfi->f_lfich+1);        
  memcpy(par.b_lim1,pfi->f_buf,pfi->f_lfich);

  defecto_ficha(pfi);
  clave_lin_trata(ipfi,lim2,5,"");
  par.b_lim2 = malloc(pfi->f_lfich+1);        
  memcpy(par.b_lim2,pfi->f_buf,pfi->f_lfich);

  if (!evento)
  {
	  par.hook   = ejecuta_otro(ipfi,77,O_NOR,0,1);
	  if (par.hook < 1)
		  par.hook = 0;
  }
  else
  {
	  par.hook   = evento;
  }
  par.pact   = programa_activo;
  par.gmodo  = flags;   
  par.pi = pini;
  par.pf = pfin;
  par.padre = NULL;
  par.titulo = "";
  par._id    = default_id;
  par._sid   = default_sid;
  if (par._sid == default_sid)
     par._wid   = default_wid;
  else
     par._wid   = -1;


  _par = (struct gridpars *)malloc(sizeof(par));
  memcpy((char *)_par,(char *)&par,sizeof(par));
  _par->Independiente = 1;
  _par->titulo = malloc(strlen(par.titulo)+1);
  strcpy(_par->titulo,par.titulo);

  punto = LanzaGrid(_par,NULL,NULL,NULL,NULL,NULL,NULL,NULL);

  if (modo == DIFERIDO || modo == CONSULTA || modo == VISUALIZA)
  {
	  if (DespachadorExterno)
	  {       
		 char tmp[512];
		 int id_grid = v_mpfi(ipfi)->f_idgrafico;
		 strcpy(tmp,"CUSTOM1001"); /* Desactivar mantenimento */
		 (*DespachadorExterno)((int)DESPACHO_ESTADOCONTROL,id_grid,(void *)tmp);
	  }		  
  }

  if (modo != DIFERIDO && modo != VISUALIZA)
  {
	  r = FocoDeLineas(_par,modo);
  }  
  
  return(r);
}


int LanzaFiltro(FI_DEF *pfi,int laclave,char *titulo,int nfiltros,struct grid_filtro *filtros,int ncolucam,int *colucam,char *b_lim1,char *b_lim2,char **multi) {
    struct gridpars *mihijo;    
    int i,j,k;
    int np;	
	char nomdef[128];
	int tecla;	
	int ncol = 0,*col = NULL;

    if (filtros) {
		col = colucam;
		ncol = ncolucam;
	}

	apunta_ind(pfi,laclave+1,ISFIRST,0);

    mihijo = (struct gridpars *)malloc(sizeof(struct gridpars));
    memset((void *)mihijo,0,sizeof(struct gridpars));
	mihijo->structid = 0x3212;
	mihijo->sesion_id = ap_sesion_id;

    mihijo->titulo = malloc(strlen(titulo)+1);
    strcpy(mihijo->titulo,titulo); 

    
    mihijo->pfi    = pfi;
    mihijo->ipfi   = -1;
    mihijo->encl   = laclave;

    mihijo->hook   = 0;
    mihijo->pact   = 0;
	mihijo->gmodo  = 0;
    mihijo->pi = 0;
    mihijo->pf = 0;    
    mihijo->filtros = NULL;
	mihijo->sqltoscreen = NULL;	
    mihijo->nfiltros = 0;
    mihijo->Independiente = 0; /* la consulta no es independiente */  
   
	mihijo->_wid   = default_wid;   
    mihijo->_id = default_id;
    mihijo->_sid = default_sid;

	mihijo->b_lim1 = b_lim1;
	mihijo->b_lim2 = b_lim2;
	if (b_lim1 || b_lim2) {
		mihijo->gmodo |= 128;
	}
	mihijo->multiples_ficheros = multi;

    np = LanzaGrid(mihijo,ncolucam,colucam,nfiltros,filtros,ncol,col,-1);

      if (DespachadorExterno)
	  {
		(*DespachadorExterno)((int)DESPACHO_FOCOCONTROL,mihijo->id_grid,NULL);  
	  }
  

	  if (np < 0)
	  {
		  tecla = 807;
	  }
	  else
	  {
		  do
		  {
			tecla = leecar();
		  } while(tecla != 802 && tecla != 806 && tecla != 807);
	  }

	  /* reponer flag visual */
  
	  BorraGrid(np);
  


    return np;
}




static int sql_saca_comando(char **parsed,int *continua)
{
	char *origen = *parsed;
	int r = -1;
	char *p = origen;
	int sigue  = 0;

	while(*p && *p != ' ') p++;
	if (*p == ' ') 
		sigue = 1;
	*p = 0;
	*parsed = p;
	if (!strcmp(origen,"VERSION"))
	{
		r = 0;
	}
	else if (!strcmp(origen,"SELECT"))
    {
		r = 1;
	}   
	else if (!strcmp(origen,"INSERT"))
    {
		r = 2;
	}   
	else if (!strcmp(origen,"UPDATE"))
    {
		r = 3;
	}   
	else if (!strcmp(origen,"DELETE"))
    {
		r = 4;
	}   
	if (sigue)
	{
		*p = ' ';
		p++;
		while(*p == ' ') p++;
		if (!*p)
		   sigue = 0;
		else
		   *parsed = p;
	}
	if (continua)
		*continua = sigue;
	return r;
}


static char *sql_recoge_sentencia(char *comando)
{
	unsigned char *r = NULL;

    //aviso(0,comando);

	if (*comando == '@') /* file */
	{
		int f = rw_open_(comando+1,O_RDONLY);
		if (f >= 0)
		{
			long l,ll = rw_lseek_(f,0L,SEEK_END);
			rw_lseek_(f,0L,SEEK_SET);
			r = (unsigned char *)malloc(ll+100);
			rw_read_(f,r,ll);
			rw_close_(f);
			for(l = 0L;l < ll;l++)
			{
				if (r[l] < 32)
					r[l] = 32;
			}
			r[l] = 0;			
		}
	}
	else
	{
		r = (unsigned char *)malloc(strlen(comando)+100);
		strcpy(r,comando);	
	}
	if (r)
	{
		/* filtrar espaciados: solo un espacio entre sentencia para facilitar el
		   trabajo del parser */
		unsigned char *p3,*p2,*p1 = r;
		while(*p1)
		{
			if (*p1 == '\'') /* saltar encomillados */
			{
				p1++;
				while(*p1 && *p1 != '\'') p1++;
			}
			if (*p1 == '\"') /* saltar encomillados */
			{
				p1++;
				while(*p1 && *p1 != '\"') p1++;
			}

			if (*p1 == 9) /* tab*/
				*p1 = ' ';
			if (*p1 == ' ')
			{
				p2 = p1 + 1;
				p3 = p1 + 1;
				while (*p2 && (*p2 == ' ' || *p2 == 9)) p2++;
				if (p3 != p2)
				{
					while(*p2)
					{
						*p3 = *p2;
						p3++;
						p2++;
					}
					*p3 = *p2;
				}
				if (!*(p1+1))
				{
					*p1 = 0;
					break;
				}
			}
			p1++;			
		}
	}
	return (char *)r;
}


static char *sql_descomillador(char *sentencia)
{
   if (*sentencia == '\'')
   {
	   sentencia++;
	   while(*sentencia && *sentencia != '\'') sentencia++;
	   return sentencia;
   }
   if (*sentencia == '"')
   {
	   sentencia++;
	   while(*sentencia && *sentencia != '"') sentencia++;	   
   }
   return sentencia;
}

static int sql_select_setencia(char **entrada)
{   
   int i;
   do
   {
	   *entrada = sql_descomillador(*entrada);
	   i = strlen(*entrada);
	   if (!i) return -1;
	   if (i > 4)
	   {
		   if (!memcmp(*entrada,"FROM ",5))
			   return 1;
		   if (!memcmp(*entrada,"INTO ",5))
			   return 2;
	   }
	   if (i > 5)
	   {
		   if (!memcmp(*entrada,"WHERE ",6))
			   return 6;
	   }
	   if (i > 8)
	   {
		   if (!memcmp(*entrada,"ORDER BY ",9))
			   return 7;
	   }
	   (*entrada)++;
   } while(-1);

   return -1;
}

static int copia_el_origen(FI_DEF *pfi,FI_DEF *padrepfis[],struct _sql_origen *origen,double contador,long registro)
{
	int i;
	int f,c;
	int offset2,len;
	char *p = pfi->f_buf;
	for (i = 0;i < pfi->f_ncampos;i++)
	{
		f = origen[i].ipadre;
		c = origen[i].icampo;
		len = pfi->f_campos[i].c_long;
		if (f == -1)
		{
			if (c == -1)
			{
				if (len == sizeof(double))
				{
					memcpy(p,(void *)&contador,len);
				}
			}
			if (c == -2)
			{
				if (len == 1)
				{
					memcpy(p," ",1);
				}
			}
			if (c == -3)
			{
				if (len == sizeof(long))
				{					
					memcpy(p,(void *)&registro,len);
				}
			}
		}
		else
		{
			if (!hay_dat_permiso(padrepfis[f],-2)) 
				return 0;			
			offset2 = padrepfis[f]->f_campos[c].c_offset;		
			memcpy(p,padrepfis[f]->f_buf+offset2,len);
		}		
		p += len;
	}
	return 1;
}

static int acumula_filtro(struct _sql_output_ *salida)
{
	/* seria mejorable usando memoria como los antiguos listados ...
	   si la lectura fuera secuencial y las relaciones simples */	
	int i;
	FI_DEF *pfi = salida->pfi;
	FI_DEF pfi2;
	char *buf = salida->buf;		

	pfi2 = *pfi;
	pfi2.f_buf = buf;
	
	if (salida->agrupacion || salida->totales || salida->distinct)
	{
		memcpy(buf,pfi->f_buf,pfi->f_lfich);
	}
	if (salida->agrupacion)
	{		
		if (lee_reg(pfi,ISEQUAL) >= 0)
		{
			salida->resultados--;
			for (i = 0;i < pfi->f_ncampos;i++)
			{
				if (salida->opcampo[i] == 1 || salida->opcampo[i] == 4 || salida->opcampo[i] == 5 || salida->opcampo[i] == 6) /* suma *//* media *//* porcentajes */
				{
					/* acumular para las otras operaciones, se completan luego */
					st_doble(i,pfi,ld_doble(i,pfi)+ld_doble(i,&pfi2));
				}
				else if (salida->opcampo[i] == 2) /* maximo */
				{
					if (ld_doble(i,pfi) < ld_doble(i,&pfi2))
					{
						st_doble(i,pfi,ld_doble(i,&pfi2));
					}
				}
				else if (salida->opcampo[i] == 3) /* minimo */
				{
					if (ld_doble(i,pfi) > ld_doble(i,&pfi2))
					{
						st_doble(i,pfi,ld_doble(i,&pfi2));
					}
				}
			}
			graba_actual(pfi);
			if (RetroPar[salida->el_punto] != (char *)salida->par)
			{		
				return -1;
			}
			memcpy(pfi->f_buf,buf,pfi->f_lfich);
		}
		else
		{
			memcpy(pfi->f_buf,buf,pfi->f_lfich);
			graba_nuevo(pfi);
		}
	}
	else
		graba_nuevo(pfi);

	if (salida->totales) /* falta por cada campo de la clave */
	{
		
		if (RetroPar[salida->el_punto] != (char *)salida->par)
		{		
			return -1;
		}
#ifdef NADANADA
		/* la clave corriente a de ser la de uso */		
		st_campo(salida->totales-1,pfi,"T");
		if (lee_reg(pfi,ISEQUAL) < 0)
		{
			char vacio[300];
			memset(vacio,' ',299);
			vacio[299] = 0;			
			for (i = 0;i < pfi->f_ncampos;i++)
			{
				if (salida->opcampo[i] > 0) /* suma etc */
				{
					st_doble(i,pfi,(double)0.0);
				}
				else
				{
					if (!pfi->f_campos[i].c_key)
					{
						st_campo(i,pfi,vacio);
					}
					else
					{
						if (salida->origen && salida->origen[i].icampo == -1)
						{
							salida->resultados++;
							st_doble(i,pfi,(double)salida->resultados);
						}
					}
				}
			}
			graba_nuevo(pfi);
			if (lee_reg(pfi,ISEQUAL) < 0)
				return -1;
		}
		for (i = 0;i < pfi->f_ncampos;i++)
		{
			if (salida->opcampo[i] == 1) /* suma */
			{
				st_doble(i,pfi,ld_doble(i,pfi)+ld_doble(i,&pfi2));
			}
			else if (salida->opcampo[i] == 2) /* maximo */
			{
				if (ld_doble(i,pfi) < ld_doble(i,&pfi2))
				{
					st_doble(i,pfi,ld_doble(i,&pfi2));
				}
			}
			else if (salida->opcampo[i] == 3) /* minimo */
			{
				if (ld_doble(i,pfi) > ld_doble(i,&pfi2))
				{
					st_doble(i,pfi,ld_doble(i,&pfi2));
				}
			}
		}		

	    for (i = 0;i < salida->par->totales[0];i++)
		{
		    if ((salida->par->totales[i+1]&0xf0000) == 0x10000)
			{
			    if (salida->par->t_totales[i] < ld_doble(salida->par->totales[i+1]&0xffff,&pfi2))
				  salida->par->t_totales[i] = ld_doble(salida->par->totales[i+1]&0xffff,&pfi2);
			}
		    else if ((salida->par->totales[i+1]&0xf0000) == 0x20000)
			{
			    if (salida->par->t_totales[i] > ld_doble(salida->par->totales[i+1]&0xffff,&pfi2))
				  salida->par->t_totales[i] = ld_doble(salida->par->totales[i+1]&0xffff,&pfi2);
			}
		    else
			    salida->par->t_totales[i] += ld_doble(salida->par->totales[i+1]&0xffff,&pfi2);		    
		}

		graba_actual(pfi);		
		memcpy(pfi->f_buf,buf,pfi->f_lfich);
#else
	    for (i = 0;i < salida->par->totales[0];i++)
		{
		    if ((salida->par->totales[i+1]&0xf0000) == 0x10000)
			{
			    if (salida->par->t_totales[i] < ld_doble(salida->par->totales[i+1]&0xffff,&pfi2))
				  salida->par->t_totales[i] = ld_doble(salida->par->totales[i+1]&0xffff,&pfi2);
			}
		    else if ((salida->par->totales[i+1]&0xf0000) == 0x20000)
			{
			    if (salida->par->t_totales[i] > ld_doble(salida->par->totales[i+1]&0xffff,&pfi2))
				  salida->par->t_totales[i] = ld_doble(salida->par->totales[i+1]&0xffff,&pfi2);
			}
		    else
			    salida->par->t_totales[i] += ld_doble(salida->par->totales[i+1]&0xffff,&pfi2);
		    
		}
#endif
	}

	return -1;
}

static int evalua_filtros_sql(int nf,FI_DEF *padrepfis[],int nfiltros[],struct grid_filtro *filtros[],struct _sql_relacion *relacion,char *buffers[],struct _sql_output_ *salida,struct _sql_orden_lectura *lectura,int nivel,int maxnivel)
{
	int er = 0;
	int of,i,k,j,co,cd;
	int offset1,offset2,len;	
    FI_DEF l_pfi;
	int parcial;
	int clave = 0;
	int punto;	    
	int solouno = 0;

		if (RetroPar[salida->el_punto] != (char *)salida->par)
		{
			return -1;
		}

		if (nivel >= maxnivel)
		{
			if (!salida->origen)
			{	
				if (hay_dat_permiso(padrepfis[0],-2)) {
					salida->hay = 1;
					salida->resultados++;
					memcpy(salida->pfi->f_buf,padrepfis[0]->f_buf,salida->pfi->f_lfich);				
					if (salida->pfi->f_fd != -1)
					{
						if (salida->totales || salida->agrupacion)
						{
							acumula_filtro(salida);
						}
						else
						   graba_nuevo(salida->pfi);
					}
					if (salida->out_fun)
					{
						if ((*salida->out_fun)(salida->pfi,salida->param) == -1)
						{
							return -1;
						}
					}
					salida->nt++;
				}
			}
			else
			{				

			    if (copia_el_origen(salida->pfi,padrepfis,salida->origen,salida->resultados,salida->registropadre)) {
					salida->hay = 1;
					salida->resultados++;
					if (salida->pfi->f_fd != -1)
					{
						if (salida->totales || salida->agrupacion)
						{
							acumula_filtro(salida);
						}
						else
							graba_nuevo(salida->pfi);
					}
					if (salida->out_fun)
					{
						if ((*salida->out_fun)(salida->pfi,salida->param) == -1)
						{
							return -1;
						}
					}
					salida->nt++;
				}
			}				
			return 0;
		}

		if (nf != lectura[nivel].file)
		{
			aviso("control epsilon_1");
		}		

		i = lectura[nivel].rela;
		{   
			of = relacion[nf].frel[i];

			if (!relacion[nf].hrel[i]) /* clave a usar */
			{
				/* sin clave ... si no hay mapa de ordenacion crearlo ahora */
				clave = -1;
				if (!relacion[nf].mapa_ordenacion[i])
				{
					if (padrepfis[of]->f_fd == -1)
					{				
						if (abre_ind(padrepfis[of],ISINOUT + ISMANULOCK) < 0)
						{
							defecto_ficha(padrepfis[of]);
							if (nfiltros[of])
							{
								er = 1;
								return er;
							}
							solouno = 1;
						}
						else
							relacion[of].ofd = 1;
					}
					relacion[nf].nmapa_ordenacion[i] = rellena_mapa_ordenacion_(padrepfis[of],relacion[nf].ncrel[i],&(relacion[nf].relh[i][0]),&(relacion[nf].mapa_ordenacion[i]),tam_btrv(padrepfis[of]),NULL);
				}
				for (j = 0;j < relacion[nf].ncrel[i];j++)
				{
					co = relacion[nf].rel[i][j];
					cd = relacion[nf].relh[i][j];
					offset1 = padrepfis[nf]->f_campos[co].c_offset;
					len = padrepfis[nf]->f_campos[co].c_long;
					offset2 = padrepfis[of]->f_campos[cd].c_offset;
					/*memcpy(padrepfis[of]->f_buf+offset2,padrepfis[nf]->f_buf+offset1,len);*/
					/* falta adaptacion relacion compuesta */
					break;
				}
			}
			else
			{   /* relacion directa a clave */
				clave = relacion[nf].hrel[i] - 1;
				for (j = 0;j < relacion[nf].ncrel[i] && j < padrepfis[of]->f_clave[clave].k_ncampos;j++)
				{
					co = relacion[nf].rel[i][j];
					cd = padrepfis[of]->f_clave[clave].k_campo[j] - 1;
					offset1 = padrepfis[nf]->f_campos[co].c_offset;
					len = padrepfis[nf]->f_campos[co].c_long;
					offset2 = padrepfis[of]->f_campos[cd].c_offset;
					memcpy(padrepfis[of]->f_buf+offset2,padrepfis[nf]->f_buf+offset1,len);
				}
				if (j < padrepfis[of]->f_clave[clave].k_ncampos)
				{
					parcial = j;
				}
				else
					parcial = 0;				
			}
			if (padrepfis[of]->f_fd == -1)
			{				
				if (abre_ind(padrepfis[of],ISINOUT + ISMANULOCK) < 0)
				{
					defecto_ficha(padrepfis[of]);
					if (nfiltros[of])
					{
						er = 1;
						return er;
					}
					solouno = 1;
				}
				else
					relacion[of].ofd = 1;
			}			

			/* posible grupo de fichas */
			if (clave == -1)
			{
				/* falta adaptacion a relacion compuesta */
				char buf1[MAXDEFECTO+1];				

				buf1[MAXDEFECTO] = 0;
				formato_campo(padrepfis[nf],co,buf1);
				punto = busca_mapa_ordenacion_2(buf1,relacion[nf].mapa_ordenacion[i], 0, relacion[nf].nmapa_ordenacion[i],cd,padrepfis[of],0);
				if (punto < 0 || punto >= relacion[nf].nmapa_ordenacion[i])
				{
					er = -1;
				}
				else
				{					
					punto = avanza_mapa_ordenacion_2(buf1,relacion[nf].mapa_ordenacion[i], punto, relacion[nf].nmapa_ordenacion[i]-1,cd,padrepfis[of],0);
					punto = retrocede_mapa_ordenacion_2(NULL,relacion[nf].mapa_ordenacion[i],punto,cd,padrepfis[of],0,0);
					isrecnum = relacion[nf].mapa_ordenacion[i][punto];
					er = ixx_lee_indice(padrepfis[of]->f_fd,padrepfis[of]->f_buf,ISEQUAL,padrepfis[of]->f_nclaves,1,0,1);
				}
			}
			else
			{
				if (parcial)
				{
					er = ixx_lee_indice(padrepfis[of]->f_fd,padrepfis[of]->f_buf,IXXGTEQ,clave,1,parcial,1);
					if (er >= 0)
					{
						for (j = 0;j < relacion[nf].ncrel[i] && j < padrepfis[of]->f_clave[clave].k_ncampos;j++)
						{
							co = relacion[nf].rel[i][j];
							cd = padrepfis[of]->f_clave[clave].k_campo[j] - 1;
							offset1 = padrepfis[nf]->f_campos[co].c_offset;
							len = padrepfis[nf]->f_campos[co].c_long;
							offset2 = padrepfis[of]->f_campos[cd].c_offset;
							if (memcmp(padrepfis[of]->f_buf+offset2,padrepfis[nf]->f_buf+offset1,len))
							{
								er = -1;
								break;
							}
						}
					}
				}
				else
				{
					er = ixx_lee_indice(padrepfis[of]->f_fd,padrepfis[of]->f_buf,ISEQUAL,clave,1,parcial,1);
				}
			}
			if (er < 0)
			{
				defecto_ficha(padrepfis[of]);				
				if (nfiltros[of])
				{
					er = 1;
					return er;
				}
				solouno = 1;
			}
		}
		k = 0;
		do
		{
			if (salida->par)
			{
				if (salida->tpulsa != salida->tactual)
				{
					int latecla;

					salida->tpulsa = salida->tactual;
					if ((latecla = pulsatecla()) > -1)
					{
						if (latecla == 802 || latecla == 806 || latecla == 807)
						{
							er = -1;
							return er;
						}
					}
					/* ver si el control actual es valido !!! */
					/* pues el pulsatecla permite si destruccion */
					if (RetroPar[salida->el_punto] != (char *)salida->par)
					{
						er = -1;
						return er;
					}
				}

				salida->tactual = time(NULL);
				if (!salida->contad || salida->contad < salida->tactual || salida->contad > (salida->tactual+3))
				{
					char tmp[256];
					long modo = 1;
					long tam = (sizeof(long)*4)+sizeof(int);
					int id_grid = salida->par->id_grid;

					if (salida->par->cache)
					{
						for (i = 0;salida->par->cache[i].id_reg;i++)
						{
							salida->par->cache[i].id_reg = -1L;
						}
					}

					salida->par->registros = TotalRegistros(salida->par);

					memcpy(tmp,&tam,sizeof(long));
					memcpy(tmp+sizeof(long),&tam,sizeof(long));
					memcpy(tmp+(sizeof(long)*2),&modo,sizeof(long));
					memcpy(tmp+(sizeof(long)*3),&id_grid,sizeof(int));
					memcpy(tmp+(sizeof(long)*3)+sizeof(int),(void *)&salida->par->registros,sizeof(long));
					if (DespachadorExterno)
						(*DespachadorExterno)((int)DESPACHO_BINARIO,(int)3,tmp);
					salida->hay=0;
					salida->contad = time(NULL) + 3;
					if (RetroPar[salida->el_punto] != (char *)salida->par)
					{
						er = -1;
						return er;
					}
				}                
			}

			l_pfi = *padrepfis[of];
			l_pfi.f_buf = buffers[of];
			if (!nfiltros[of] ||!evaluador_simple_de_filtro(padrepfis[of],nfiltros[of],filtros[of],&l_pfi))
			{				
				er = evalua_filtros_sql(lectura[nivel+1].file,padrepfis,nfiltros,filtros,relacion,buffers,salida,lectura,nivel+1,maxnivel);
				if (er < 0)
				{
					return -1;
				}
			}

			if (solouno)
				break;

			if (clave == -1)
			{
				if ((punto+1) < relacion[nf].nmapa_ordenacion[i])
				{
					if (retrocede_mapa_ordenacion_2(NULL,relacion[nf].mapa_ordenacion[i],punto,cd,padrepfis[of],punto+1,0) == (punto+1))
					{
						punto++;

					}
					else
						break;
				}
				else
					break;
			}
			else
			{
				if (clave == 0 && !parcial) /* clave primaria unica */
					break;
				if (ixx_lee_indice(padrepfis[of]->f_fd,padrepfis[of]->f_buf,ISNEXT,-1,1,0,1) < 0)
					break;
				/* comprobar registro leido */
				for (j = 0;j < relacion[nf].ncrel[i] && j < padrepfis[of]->f_clave[clave].k_ncampos;j++)
				{
					co = relacion[nf].rel[i][j];
					cd = padrepfis[of]->f_clave[clave].k_campo[j] - 1;
					offset1 = padrepfis[nf]->f_campos[co].c_offset;
					len = padrepfis[nf]->f_campos[co].c_long;
					offset2 = padrepfis[of]->f_campos[cd].c_offset;
					if (memcmp(padrepfis[of]->f_buf+offset2,padrepfis[nf]->f_buf+offset1,len))
					{
						solouno = 1;
						break;
					}
				}
				if (solouno)
					break;
			}
		} while(-1);
	
	return 0;
}

static int sql_select_ejecuta(FI_DEF *pfi,FI_DEF *padrepfis[],int nfiltros[],struct grid_filtro *filtros[],struct _sql_origen *origen,struct _sql_relacion *relacion,char *buffers[],struct gridpars *par,int el_punto,int (*out_fun)(),void *param,int nf,int agrupacion, int distinct)
{	    
        long registro;
        int er;
        int fd,ofd = 0;
        int porcontenido = 0;
		int i,j,k;
		int latecla;
		char *salva_buf;
		struct _sql_output_ salida;
		struct _sql_orden_lectura lectura[500];
		int nlectura = 0;
		int key = -1;
		char *buf1 = NULL;
		char *buf2 = NULL;		

		memset(&salida,0,sizeof(salida));

		salida.pfi = pfi;
		salida.nt = 0;
		salida.resultados = 0.0;
		salida.hay = 0;
		salida.param = param;		
		salida.origen = origen;		
        salida.contad = 0;
		salida.tpulsa = 0;
		salida.tactual= 0;
		salida.par = par;
		salida.el_punto = el_punto;
		salida.totales = 0;
		salida.registropadre = 0;

		salida.agrupacion = agrupacion;			
		salida.out_fun = out_fun;

		salida.distinct = distinct;
		

		for (i = 0;i < pfi->f_ncampos;i++)
		{
			salida.opcampo[i] = 0;
			if (origen && origen[i].ipadre == -1 && origen[i].icampo == -2)
			{
				salida.totales = i+1;				
			}
			else
			{
			}
		}
		
		if (origen)
		for (i = 0;i < nf;i++)
		{
			for (j = 0;j < nfiltros[i];j++)
			{
				for (k = 0;k < pfi->f_ncampos;k++)
				{
					if (origen[k].ipadre == i && origen[k].icampo == filtros[i][j].campo)
					{
						salida.opcampo[k] = ((filtros[i][j].tipo >> 16) & 0xf);
						if ((filtros[i][j].tipo & 0x40))												
						{
							salida.agrupacion = 1;
						}
						break;
					}						
				}								
			}
		}

        er = -1;
        if (nfiltros[0] == 1 && filtros[0][0].tipo == 3 && !filtros[0][0].inver &&
                                                               pfi->f_campos[filtros[0][0].campo].c_tipo == CHARTIPO &&
                                                               pfi->f_campos[filtros[0][0].campo].c_qc > strlen(filtros[0][0].valor))
        {
            for (i = 0;i < pfi->f_nclaves;i++)
            {
                if (pfi->f_clave[i].k_ncampos == 1 && pfi->f_clave[i].k_campo[0] == (filtros[0][0].campo+1))
                {
                    er = i;
                    break;
                }
            }
            if (er == -1)
            {            
                for (i = 0;i < pfi->f_nclaves;i++)
                {
                    if (pfi->f_clave[i].k_campo[0] == (filtros[0][0].campo+1))
                    {
                        er = i;
                        break;
                    }
                }
            }
            if (er != -1)
            {
                porcontenido = 1;
            }
        }
        if (er == -1) 
            er = 0;

		nlectura = 0;
		if (!porcontenido)
		{
			for (i = 0;i < nf;i++)
			{
				for (j = 0;j < relacion[i].nrel;j++)
				{   /* deben estar ordenadas por parentesco */
					if (nlectura >= 500)
					{
						aviso(0,"Demasiadas relaciones");
						return -1;
					}
					lectura[nlectura].file = i;
					lectura[nlectura].rela = j;
					nlectura++;
				}
			}
		}

		salida.buf = malloc(pfi->f_lfich+1);
		memset(salida.buf,0,pfi->f_lfich+1);

		if (padrepfis[0]->f_fd == -1)
		{
			ofd = 1;        
			if (abre_ind(padrepfis[0],ISINOUT + ISMANULOCK) < 0)
			{				
				goto sal_ejecuta_sql;
			}
		}
        apunta_ind(padrepfis[0],er+1,ISFIRST,0);
        fd = padrepfis[0]->f_fd;       


        if (porcontenido)
        {
            ixx_pon_contenido(fd,filtros[0][0].valor,strlen(filtros[0][0].valor));
        }
		else {
			key = crea_limites_buf(nfiltros[0],filtros[0],padrepfis[0],&buf1,&buf2);
		}

		if (key > -1) {			
			if (buf1) {
				memcpy(padrepfis[0]->f_buf,buf1,padrepfis[0]->f_lfich);
				er = ixx_lee_indice(fd,padrepfis[0]->f_buf,ISGTEQ,key,1,0,1);
			}
			else
				er = ixx_lee_indice(fd,padrepfis[0]->f_buf,ISFIRST,key,1,0,1);
		}
		else
			er = ixx_lee_indice(fd,padrepfis[0]->f_buf,ISFIRST,-1,1,0,1);

        
		salida.registropadre = isrecnum;

        salida.contad = 0;        
        while(er >= 0)
        {
			if (par)
			{
				if (salida.tpulsa != salida.tactual)
				{
					salida.tpulsa = salida.tactual;
					if ((latecla = pulsatecla()) > -1)
					{
						if (latecla == 802 || latecla == 806 || latecla == 807)
						{
							break;
						}
					}
					/* ver si el control actual es valido !!! */
					/* pues el pulsatecla permite si destruccion */
					if (RetroPar[el_punto] != (char *)par)
					{
						break;
					}
				}


				salida.tactual = time(NULL);
				if (salida.hay)
				{				
					if (!salida.contad || salida.contad < salida.tactual || salida.contad > (salida.tactual+3))
					{
						char tmp[256];
						long modo = 1;
						long tam = (sizeof(long)*4)+sizeof(int);
						int id_grid = par->id_grid;

						if (par->cache)
						{
							for (i = 0;par->cache[i].id_reg;i++)
							{
								par->cache[i].id_reg = -1L;
							}
						}

						par->registros = TotalRegistros(par);

						memcpy(tmp,&tam,sizeof(long));
						memcpy(tmp+sizeof(long),&tam,sizeof(long));
						memcpy(tmp+(sizeof(long)*2),&modo,sizeof(long));
						memcpy(tmp+(sizeof(long)*3),&id_grid,sizeof(int));
						memcpy(tmp+(sizeof(long)*3)+sizeof(int),(void *)&par->registros,sizeof(long));
						if (DespachadorExterno)
							(*DespachadorExterno)((int)DESPACHO_BINARIO,(int)3,tmp);
						salida.hay=0;
						salida.contad = time(NULL) + 3;
						if (RetroPar[el_punto] != (char *)par)
						{
							break;
						}
					}                
				}
			}
			
			if (!er && key > -1)
			{				
				int a_fd;
				a_fd = padrepfis[0]->f_fd;
				padrepfis[0]->f_fd = fd;
				if (!esta_en_limites_buf(padrepfis[0],buf1,buf2,key)) {
					if (salta_limites_buf(padrepfis[0],buf1,buf2,key)) {
						er = -1;
					}
				}
				padrepfis[0]->f_fd = a_fd;
				if (er < 0)
					break;
			}
			if (!er)
			{
				
				if (porcontenido)
				{
					er = evalua_filtros_sql(0,padrepfis,0,NULL,NULL,buffers,&salida,NULL,0,0);
				}
				else
				{
					FI_DEF pfi;
					pfi = *padrepfis[0];
					pfi.f_buf = buffers[0];
					if (!nfiltros[0] || !evaluador_simple_de_filtro(padrepfis[0],nfiltros[0],filtros[0],&pfi))
					{
						er = evalua_filtros_sql(0,padrepfis,nfiltros,filtros,relacion,buffers,&salida,&lectura,0,nlectura);
					}					
				}
				if (RetroPar[el_punto] != (char *)par)
				{
					break;
				}
			}
		
			if (er > 1)
			{
			   if (ixx_lee_indice(fd,padrepfis[0]->f_buf,ISGREAT,0,0,er,0) < 0)
			   {
				   er = -1;
			   }
			   else
			   {
				   er = ixx_lee_indice(fd,padrepfis[0]->f_buf,ISCURR,-1,1,0,1);
			   }
			}
			else
			{
				er = ixx_lee_indice(fd,padrepfis[0]->f_buf,ISNEXT,-1,1,0,1);
			}
			salida.registropadre = isrecnum;
        }
        if (porcontenido)
        {
            fgv_fin_contenido(padrepfis[0]);
        }
		if (ofd == 1)
		{
            cierra_ind(padrepfis[0]);
		}
sal_ejecuta_sql:
		free(salida.buf);
		return salida.nt;
}


static void quita_comillas(char *s)
{	
	char *k,*p = s;
	while(*p && *p == ' ') p++;
	if (p > s)
	{
		for (k = s;*p;k++,p++)
		{
			*k = *p;
		}
		*k = *p;
		p = s;
	}
	if (*p == '\'')
	{
		p++;
		for (k = s;*p && *p != '\'';k++,p++)
		{
			*k = *p;
		}
		*k = *p;
		if (*k == '\'')
			*k = 0;
	}
	else if (*p == '"')
	{
		p++;
		for (k = s;*p && *p != '\'';k++,p++)
		{
			*k = *p;
		}
		*k = *p;
		if (*k == '"')
			*k = 0;
	}
	p = s;
	if (*p)
	{
		while(*p) p++;
		p--;
		while(p >= s && *p == ' ') 
		{
			*p = 0;
			p--;
		}		
	}
}

static char *strcpy_sincomillas(char *destino,char *origen)
{
	char *r = strcpy(destino,origen);
	quita_comillas(destino);
	return r;
}

int saca_sql_campo(struct _sql_from *from,char *campo,int *icampo)
{
	char tmp[512];
	char *p;
	int nf;
	strcpy(tmp,campo);
	p = tmp;
	if (tmp[0] == '#')
	{
		p = strstr(tmp+1,"#");
		if (!p)
		{
			return -1;
		}
		*p = 0;
		p++;
		*icampo = atoi(p);
		for (nf = 0;nf < from->nfrom;nf++)
		{
			if (!strcmp(tmp+1,from->from[nf]))
			{
				return nf;
			}
		}
		return -1;
	}
	else if ((p = strstr(tmp,".")))
	{		
		*p = 0;
		p++;
		*icampo = atoi(p);
		for (nf = 0;nf < from->nfrom;nf++)
		{
			if (!strcmp(tmp,from->from[nf]))
			{
				return nf;
			}
		}
		return -1;
	}
	else
	{
		*icampo = atoi(campo);
		return 0;
	}
	return -1;
}

static 	struct _sql_campos campos;
static 	struct _sql_from from;
static 	struct _sql_into into;
static 	struct _sql_wheres wheres;
static 	struct _sql_orderbys orderbys;
static 	struct _sql_origen *origen;
static 	struct _sql_relacion *relacion;
static 	struct _sql_values values;


static void sql_procesa_insert(char *salida,char *comando)
{
	char *_sentencia = sql_recoge_sentencia(comando);
	char *sentencia;
	char *p1,*p2,*p3,cc;
	FI_DEF *pfi = NULL;
	int cargado = 0;
	char tmp[512];
	int i,campo;


	//INSERT INTO table_name
    //VALUES (value1, value2, value3,...)

	//INSERT INTO table_name (column1, column2, column3,...)
    //VALUES (value1, value2, value3,...)

	memset(&from,0,sizeof(from));
	memset(&campos,0,sizeof(campos));
	memset(&into,0,sizeof(into));
	memset(&values,0,sizeof(values));	

	sentencia = _sentencia;
    while(*sentencia == ' ') 
		sentencia++;

	if (!memcmp(sentencia,"INTO ",5))
	{		
		sentencia += 5;
	}
	else {
		sprintf(salida,"-1 INTO requerido");
		return;
	}

	while(*sentencia == ' ') 
		sentencia++;
	p1 = sentencia;
	sentencia = sql_descomillador(sentencia);
	while(*sentencia && *sentencia != ' ') 
		sentencia++;		
	memcpy(into.into,p1,(int)(sentencia-p1));
	into.into[(int)(sentencia-p1)] = 0;
	quita_comillas(into.into);
	while(*sentencia == ' ') 
		sentencia++;
	p1 = into.into;
	while(*p1 && *p1 != '.')
		p1++;
	into.tipo = 0;
	if (*p1 == '.')
	{
		*p1 = 0;
		p1++;
		if (!strcmp(p1,"dat"))
		{
			into.tipo = 1;
		}
		else if (!strcmp(p1,"def"))
		{
			into.tipo = 2;
		}
		else
		{
			into.tipo = 999;
		}
	}

	campos.ncampos = 0;	
	if (*sentencia == '(') {
		sentencia++;
		while(*sentencia == ' ') 
			sentencia++;
		p1 = sentencia;
		do
		{
			sentencia = sql_descomillador(sentencia);
			while(*sentencia && *sentencia != ' ' && *sentencia != ')') 
				sentencia++;
			p2 = sentencia;
			while(*sentencia == ' ') 
				sentencia++;
			sentencia = sql_descomillador(sentencia);
			while(*sentencia == ' ') 
				sentencia++;
			if (!*sentencia || *sentencia == ')')
			{		
				//if (p2 == sentencia)
				//	p2--;
				if (*sentencia) {
					sentencia++;
					while(*sentencia == ' ') 
						sentencia++;
				}
				break;
			}
			else
				sentencia++;
			while(*sentencia == ' ') 
				sentencia++;
			sentencia = sql_descomillador(sentencia);
			while(*sentencia == ' ') 
				sentencia++;
		} while(-1);
		/* de p1 a p2 area de campos */	
		p3 = p1;
		while(p3 <= p2)
		{
			p3 = sql_descomillador(p3);
			if (p3 == p2 || *p3 == ',')
			{
				memcpy(tmp,p1,(int)(p3-p1));
				tmp[(int)(p3-p1)] = 0;
				quita_blancos(tmp);
				//quita_blancos(p1);
				strcpy_sincomillas(campos.campos[campos.ncampos].id,tmp);
				//strcpy_sincomillas(campos.campos[campos.ncampos].alias,p1);
				strcpy_sincomillas(campos.campos[campos.ncampos].alias,"");
				campos.ncampos++;
				p1 = p3 + 1;
			}
			p3++;
		}
	}

	values.nvalues = 0;
	if (!memcmp(sentencia,"VALUES ",7))
	{		
		sentencia += 7;
	}
	else {
		sprintf(salida,"-1 VALUES requerido");
		return;
	}

    while(*sentencia == ' ') 
		sentencia++;

	if (*sentencia == '(') {
		sentencia++;
		p1 = sentencia;
		do
		{
			sentencia = sql_descomillador(sentencia);
			while(*sentencia && *sentencia != ' ' && *sentencia != ')') 
				sentencia++;
			p2 = sentencia;
			while(*sentencia == ' ') 
				sentencia++;
			sentencia = sql_descomillador(sentencia);
			while(*sentencia == ' ') 
				sentencia++;
			if (!*sentencia || *sentencia == ')')
			{		
				//if (p2 == sentencia)
				//	p2--;
				if (*sentencia) {
					sentencia++;
					while(*sentencia == ' ') 
						sentencia++;
				}
				break;
			}
			else
				sentencia++;
			while(*sentencia == ' ') 
				sentencia++;
			sentencia = sql_descomillador(sentencia);
			while(*sentencia == ' ') 
				sentencia++;
		} while(-1);
		/* de p1 a p2 area de campos */	
		p3 = p1;
		while(p3 <= p2)
		{
			p3 = sql_descomillador(p3);
			if (p3 == p2 || *p3 == ',')
			{
				memcpy(tmp,p1,(int)(p3-p1));
				tmp[(int)(p3-p1)] = 0;
				strcpy_sincomillas(values.valor[values.nvalues],tmp);
				values.nvalues++;
				p1 = p3 + 1;
			}
			p3++;
		}
	}


	i = ind_rel(into.into);
	{
		char tmp[300];
		pfi = NULL;
		if (i >= 0)
		{
			strcpy(tmp,v_mpfi(i)->f_PathDef);
		}
		else
		{
			strcpy(tmp,dirdef);
			strcat(tmp,into.into);
		}
		i = lee_defn(tmp,&pfi);				
		if (i >= 0)
		{
			i = ind_rel(into.into);				
			if (i < 0) {
				cambia_path_fich(pfi->f_fichero);
				i = 0;
			}
			else
			{
				strcpy(pfi->f_fichero,v_mpfi(i)->f_fichero);
			}				
		}
	}
	if (i < 0 || !pfi)
	{
		sprintf(salida,"-1 No se encuentra tabla '%s'",into.into);
		return;
	}	

	cargado = 1;
	if (abre_ind(pfi,ISINOUT + ISMANULOCK) < 0)
	{
		if (crea_ind(pfi) < 0)
		{
			cargado = 0;
		}
	}
	if (cargado) {
		defecto_ficha(pfi);
		from.nfrom = 1;
		strcpy(from.from[0],into.into);
		if (!campos.ncampos) {
			for (i = 0;i < pfi->f_ncampos;i++) {
				if (values.nvalues > i)
					st_campo(i,pfi,values.valor[i]);
			}
		}
		else {
			for (i = 0;i < campos.ncampos;i++) {
				campo = -1;
				saca_sql_campo(&from,campos.campos[i].id,&campo);
				if (campo > -1) {
					if (values.nvalues > i)
						st_campo(campo,pfi,values.valor[i]);
				}
			}
		}
		if (graba_nuevo(pfi) < 0) {
			sprintf(salida,"-1 Error al intertar en tabla '%s' (%d)",into.into,iserrno);
			cargado = 0;
		}
		cierra_ind(pfi);
		if (cargado)
			sprintf(salida,"0 OK");
	}
	else
		sprintf(salida,"-1 No se puede abrir la tabla '%s'",into.into);

	free_defpri(pfi);
}



static void sql_procesa_update(char *salida,char *comando)
{
	char *_sentencia = sql_recoge_sentencia(comando);
	char *sentencia;
	char *p1,*p2,*p3,cc;
	FI_DEF *pfi = NULL;
	FI_DEF *pfitest = NULL;
	int cargado = 0;
	char tmp[512];
	int i,campo,nf,j;
    int nfiltros,elfiltro;
	int elfiltron,er;
	struct grid_filtro *filtros = NULL;


	memset(&from,0,sizeof(from));
	memset(&into,0,sizeof(into));
	memset(&wheres,0,sizeof(wheres));
	memset(&campos,0,sizeof(campos));
	memset(&values,0,sizeof(values));	

	sentencia = _sentencia;
    while(*sentencia == ' ') 
		sentencia++;

	/*
	if (!memcmp(sentencia,"FROM ",5))
	{		
		sentencia += 5;
	}
	else {
		sprintf(salida,"-1 FROM requerido");
		return;
	}

	while(*sentencia == ' ') 
		sentencia++;
	*/

	p1 = sentencia;
	sentencia = sql_descomillador(sentencia);
	while(*sentencia && *sentencia != ' ') 
		sentencia++;		
	memcpy(into.into,p1,(int)(sentencia-p1));
	into.into[(int)(sentencia-p1)] = 0;
	quita_comillas(into.into);
	while(*sentencia == ' ') 
		sentencia++;
	p1 = into.into;
	while(*p1 && *p1 != '.')
		p1++;
	into.tipo = 0;
	if (*p1 == '.')
	{
		*p1 = 0;
		p1++;
		if (!strcmp(p1,"dat"))
		{
			into.tipo = 1;
		}
		else if (!strcmp(p1,"def"))
		{
			into.tipo = 2;
		}
		else
		{
			into.tipo = 999;
		}
	}
	from.nfrom = 1;
	strcpy(from.from[0],into.into);


	//UPDATE table_name
    //SET column1=value, column2=value2,...
    //WHERE some_column=some_value

	campos.ncampos = 0;	
	if (memcmp(sentencia,"SET ",4))
	{		
		sprintf(salida,"-1 SET requerido");
		return;		
	} else {
		sentencia += 4;		
		p1 = sentencia;
		do
		{
			sentencia = sql_descomillador(sentencia);
			while(*sentencia && *sentencia != ' ') 
				sentencia++;
			p2 = sentencia;
			while(*sentencia == ' ') 
				sentencia++;
			sentencia = sql_descomillador(sentencia);
			while(*sentencia == ' ') 
				sentencia++;
			if (!*sentencia || !memcmp(sentencia,"WHERE ",6))
			{		
				if (p2 == sentencia)
					p2--;
				break;
			}
			else
				sentencia++;
			while(*sentencia == ' ') 
				sentencia++;
			sentencia = sql_descomillador(sentencia);
			while(*sentencia == ' ') 
				sentencia++;
		} while(-1);
		/* de p1 a p2 area de campos */	
		p3 = p1;
		while(p3 <= p2)
		{
			p3 = sql_descomillador(p3);
			if (p3 == p2 || *p3 == ',')
			{
				memcpy(tmp,p1,(int)(p3-p1));
				tmp[(int)(p3-p1)] = 0;
				for (j = 0;tmp[j] && tmp[j] != '=';j++);
				if (tmp[j] == '=') {
					tmp[j] = 0;
					j++;
					quita_blancos(tmp);				
					strcpy_sincomillas(campos.campos[campos.ncampos].id,tmp);				
					strcpy_sincomillas(campos.campos[campos.ncampos].alias,"");
					strcpy_sincomillas(values.valor[values.nvalues],tmp+j);
					campos.ncampos++;
					values.nvalues++;
				}
				p1 = p3 + 1;
			}
			p3++;
		}
	}


	if (!memcmp(sentencia,"WHERE ",6))
	{
		int condicion;

		sentencia += 6;
		p1 = sentencia;
		sql_select_setencia(&sentencia);
		p2 = p1;
		wheres.ncond = 0;

		while(p1 < sentencia)
		{
			p1 = sql_descomillador(p1);			
			if (!memcmp(p1,"AND ",4) || !memcmp(p1,"OR ",3) || (p1+1) >= sentencia)
			{
				if (!memcmp(p1,"OR ",3))
					condicion = 1;
				else
					condicion = 0;
				if ((p1+1) >= sentencia)
				{
					p1 = sentencia;
				}
				wheres.where[wheres.ncond].inverso = 0;
				wheres.where[wheres.ncond].campo[0] = 0;
				wheres.where[wheres.ncond].valor[0] = 0;
				wheres.where[wheres.ncond].valor1[0] = 0;
				wheres.where[wheres.ncond].cond = 0;
                wheres.where[wheres.ncond+1].esor = condicion;
				if (!memcmp(p2,"NOT ",4))
				{
					wheres.where[wheres.ncond].inverso = 1;
					p2 += 4;
				}
				for(p3 = p2;*p3;p3++)
				{
					p3 = sql_descomillador(p3);
					if (!memcmp(p3," LIKE ",6))
					{
						wheres.where[wheres.ncond].cond = 3;
						cc = *p3;
						*p3 = 0;
						strcpy_sincomillas(wheres.where[wheres.ncond].campo,p2);
						*p3 = cc;
						cc = *p1;
						*p1 = 0;
						strcpy_sincomillas(wheres.where[wheres.ncond].valor,p3+6);
						*p1 = cc;
						break;
					}
					else if (!memcmp(p3," == ",4))
					{
						wheres.where[wheres.ncond].cond = 1;
						cc = *p3;
						*p3 = 0;
						strcpy_sincomillas(wheres.where[wheres.ncond].campo,p2);
						*p3 = cc;
						cc = *p1;
						*p1 = 0;
						strcpy_sincomillas(wheres.where[wheres.ncond].valor,p3+4);
						*p1 = cc;
						break;
					}
					else if (!memcmp(p3," = ",3))
					{
						wheres.where[wheres.ncond].cond = 2;
						cc = *p3;
						*p3 = 0;
						strcpy_sincomillas(wheres.where[wheres.ncond].campo,p2);
						*p3 = cc;
						cc = *p1;
						*p1 = 0;
						strcpy_sincomillas(wheres.where[wheres.ncond].valor,p3+3);
						*p1 = cc;
						break;
					}					
					else if (!memcmp(p3," BETWEEN ",9))
					{
						wheres.where[wheres.ncond].cond = 0;
						cc = *p3;
						*p3 = 0;
						strcpy_sincomillas(wheres.where[wheres.ncond].campo,p2);
						*p3 = cc;
						p2 = p3 + 9;
						p3 = p2;
						p3 = sql_descomillador(p3);
						while (p3 < p1 && *p3 != ',')
						{
						   p3++;
						   p3 = sql_descomillador(p3);						   
						}
						cc = *p3;
						*p3 = 0;
						strcpy_sincomillas(wheres.where[wheres.ncond].valor,p2);
						*p3 = cc;
						cc = *p1;
						*p1 = 0;
						strcpy_sincomillas(wheres.where[wheres.ncond].valor1,p3+1);
						*p1 = cc;
						break;
					}

				}				
				(wheres.ncond)++;
				if (condicion)
					p1 += 2;
				else
				    p1 += 3;
				p2 = p1+1;
			}
			p1++;
		}
	}
	else {
		sprintf(salida,"-1 WHERE requerido");
		return;
	}

	nfiltros = 0;
	for (i = 0;i < wheres.ncond;i++)
	{						
		elfiltro = saca_sql_campo(&from,wheres.where[i].campo,&campo);
		if (elfiltro < 0)
		{
			sprintf(salida,"-1 No se encuentra campo '%s'",wheres.where[i].campo);
			return;
		}
		nfiltros++;
	}

    filtros = (struct grid_filtro *)malloc(sizeof(struct grid_filtro) * (nfiltros+1));
	memset(filtros,0,sizeof(struct grid_filtro) * (nfiltros+1));

	nfiltros = 0;
	for (i = 0;i < wheres.ncond;i++)
	{						
		elfiltro = saca_sql_campo(&from,wheres.where[i].campo,&campo);
		nf = nfiltros;
		nfiltros++;

		filtros[nf].campo = campo;
		filtros[nf].tipo = wheres.where[i].cond;
		if (wheres.where[i].esor)
		{							
			filtros[nf].tipo |= 0x40;
		}
		filtros[nf].mayus = 0;
		filtros[nf].inver = wheres.where[i].inverso;
		if (!(filtros[nf].tipo))
		{
			strcpy(filtros[nf].valor,"");
			strcpy(filtros[nf].limi,wheres.where[i].valor);
			strcpy(filtros[nf].lims,wheres.where[i].valor1);
		}
		else
		{
			strcpy(filtros[nf].limi,"");
			strcpy(filtros[nf].lims,"");
			strcpy(filtros[nf].valor,wheres.where[i].valor);
		}
	}


	i = ind_rel(into.into);
	{
		char tmp[300];
		pfi = NULL;
		if (i >= 0)
		{
			strcpy(tmp,v_mpfi(i)->f_PathDef);
		}
		else
		{
			strcpy(tmp,dirdef);
			strcat(tmp,into.into);
		}
		i = lee_defn(tmp,&pfi);
		if (i >= 0)
		{
			i = ind_rel(into.into);				
			if (i < 0) {
				cambia_path_fich(pfi->f_fichero);
				i = 0;
			}
			else
			{
				strcpy(pfi->f_fichero,v_mpfi(i)->f_fichero);
			}
			i = lee_defn(tmp,&pfitest);			
		}
	}
	if (i < 0 || !pfi)
	{
		if (pfi) {
			free_defpri(pfi);
		}
		sprintf(salida,"-1 No se encuentra tabla '%s'",into.into);
		return;
	}	

	cargado = 1;
	if (abre_ind(pfi,ISINOUT + ISMANULOCK) < 0)
	{
		cargado = 0;
	}
	if (cargado) {
		int key;
		int porcontenido = -1;
		char *buf1 = NULL;
		char *buf2 = NULL;		

		key = crea_limites_buf(nfiltros,filtros,pfi,&buf1,&buf2);

		if (key < 0) {
			porcontenido = busca_contenido_buf(nfiltros,filtros,pfi,&key);
		}

		defecto_ficha(pfi);

		if (porcontenido > -1)
        {
            ixx_pon_contenido(pfi->f_fd,filtros[porcontenido].valor,strlen(filtros[porcontenido].valor));
        }

		if (key > -1) {			
			if (buf1) {
				memcpy(pfi->f_buf,buf1,pfi->f_lfich);
				er = ixx_lee_indice(pfi->f_fd,pfi->f_buf,ISGTEQ,key,1,0,1);
			}
			else
				er = ixx_lee_indice(pfi->f_fd,pfi->f_buf,ISFIRST,key,1,0,1);
		}
		else
			er = ixx_lee_indice(pfi->f_fd,pfi->f_buf,ISFIRST,-1,1,0,1);
		while (er >= 0) {
			if (key > -1 && !esta_en_limites_buf(pfi,buf1,buf2,key)) {
				if (salta_limites_buf(pfi,buf1,buf2,key)) {
					er = -1;
					break;
				}
			}
			if (evaluador_simple_de_filtro(pfi,nfiltros,filtros,pfitest) == 0){
				if (lee_reg(pfi,ISCURR+ISLOCK) >= 0) {
					for (i = 0;i < campos.ncampos;i++) {
						campo = -1;
						saca_sql_campo(&from,campos.campos[i].id,&campo);
						if (campo > -1) {
							if (values.nvalues > i)
								st_campo(campo,pfi,values.valor[i]);
						}
					}				
					graba_actual(pfi);
				}
			}
			er = ixx_lee_indice(pfi->f_fd,pfi->f_buf,ISNEXT,-1,1,0,1);
		} 
		if (porcontenido > -1) {
			ixx_fin_contenido(pfi->f_fd);
		}
		cierra_ind(pfi);
		if (buf1)
			free(buf1);
		if (buf2)
			free(buf2);
		if (cargado)
			sprintf(salida,"0 OK");
	}
	else
		sprintf(salida,"-1 No se puede abrir la tabla '%s'",into.into);

	free_defpri(pfi);
	free_defpri(pfitest);
	
}

static void sql_procesa_delete(char *salida,char *comando)
{
	char *_sentencia = sql_recoge_sentencia(comando);
	char *sentencia;
	char *p1,*p2,*p3,cc;
	FI_DEF *pfi = NULL;
	FI_DEF *pfitest = NULL;
	int cargado = 0;
	char tmp[512];
	int i,campo,nf,elfiltro;
    int nfiltros;
	int elfiltron,er;
	struct grid_filtro *filtros = NULL;

	//DELETE FROM table_name
	//WHERE some_column=some_value


	memset(&from,0,sizeof(from));
	memset(&into,0,sizeof(into));
	memset(&wheres,0,sizeof(wheres));

	sentencia = _sentencia;
    while(*sentencia == ' ') 
		sentencia++;

	if (!memcmp(sentencia,"FROM ",5))
	{		
		sentencia += 5;
	}
	else {
		sprintf(salida,"-1 FROM requerido");
		return;
	}

	while(*sentencia == ' ') 
		sentencia++;
	p1 = sentencia;
	sentencia = sql_descomillador(sentencia);
	while(*sentencia && *sentencia != ' ') 
		sentencia++;		
	memcpy(into.into,p1,(int)(sentencia-p1));
	into.into[(int)(sentencia-p1)] = 0;
	quita_comillas(into.into);
	while(*sentencia == ' ') 
		sentencia++;
	p1 = into.into;
	while(*p1 && *p1 != '.')
		p1++;
	into.tipo = 0;
	if (*p1 == '.')
	{
		*p1 = 0;
		p1++;
		if (!strcmp(p1,"dat"))
		{
			into.tipo = 1;
		}
		else if (!strcmp(p1,"def"))
		{
			into.tipo = 2;
		}
		else
		{
			into.tipo = 999;
		}
	}
	from.nfrom = 1;
	strcpy(from.from[0],into.into);


	if (!memcmp(sentencia,"WHERE ",6))
	{
		int condicion;

		sentencia += 6;
		p1 = sentencia;
		sql_select_setencia(&sentencia);
		p2 = p1;
		wheres.ncond = 0;

		while(p1 < sentencia)
		{
			p1 = sql_descomillador(p1);			
			if (!memcmp(p1,"AND ",4) || !memcmp(p1,"OR ",3) || (p1+1) >= sentencia)
			{
				if (!memcmp(p1,"OR ",3))
					condicion = 1;
				else
					condicion = 0;
				if ((p1+1) >= sentencia)
				{
					p1 = sentencia;
				}
				wheres.where[wheres.ncond].inverso = 0;
				wheres.where[wheres.ncond].campo[0] = 0;
				wheres.where[wheres.ncond].valor[0] = 0;
				wheres.where[wheres.ncond].valor1[0] = 0;
				wheres.where[wheres.ncond].cond = 0;
                wheres.where[wheres.ncond+1].esor = condicion;
				if (!memcmp(p2,"NOT ",4))
				{
					wheres.where[wheres.ncond].inverso = 1;
					p2 += 4;
				}
				for(p3 = p2;*p3;p3++)
				{
					p3 = sql_descomillador(p3);
					if (!memcmp(p3," LIKE ",6))
					{
						wheres.where[wheres.ncond].cond = 3;
						cc = *p3;
						*p3 = 0;
						strcpy_sincomillas(wheres.where[wheres.ncond].campo,p2);
						*p3 = cc;
						cc = *p1;
						*p1 = 0;
						strcpy_sincomillas(wheres.where[wheres.ncond].valor,p3+6);
						*p1 = cc;
						break;
					}
					else if (!memcmp(p3," == ",4))
					{
						wheres.where[wheres.ncond].cond = 1;
						cc = *p3;
						*p3 = 0;
						strcpy_sincomillas(wheres.where[wheres.ncond].campo,p2);
						*p3 = cc;
						cc = *p1;
						*p1 = 0;
						strcpy_sincomillas(wheres.where[wheres.ncond].valor,p3+4);
						*p1 = cc;
						break;
					}
					else if (!memcmp(p3," = ",3))
					{
						wheres.where[wheres.ncond].cond = 2;
						cc = *p3;
						*p3 = 0;
						strcpy_sincomillas(wheres.where[wheres.ncond].campo,p2);
						*p3 = cc;
						cc = *p1;
						*p1 = 0;
						strcpy_sincomillas(wheres.where[wheres.ncond].valor,p3+3);
						*p1 = cc;
						break;
					}					
					else if (!memcmp(p3," BETWEEN ",9))
					{
						wheres.where[wheres.ncond].cond = 0;
						cc = *p3;
						*p3 = 0;
						strcpy_sincomillas(wheres.where[wheres.ncond].campo,p2);
						*p3 = cc;
						p2 = p3 + 9;
						p3 = p2;
						p3 = sql_descomillador(p3);
						while (p3 < p1 && *p3 != ',')
						{
						   p3++;
						   p3 = sql_descomillador(p3);						   
						}
						cc = *p3;
						*p3 = 0;
						strcpy_sincomillas(wheres.where[wheres.ncond].valor,p2);
						*p3 = cc;
						cc = *p1;
						*p1 = 0;
						strcpy_sincomillas(wheres.where[wheres.ncond].valor1,p3+1);
						*p1 = cc;
						break;
					}

				}				
				(wheres.ncond)++;
				if (condicion)
					p1 += 2;
				else
				    p1 += 3;
				p2 = p1+1;
			}
			p1++;
		}
	}
	else {
		sprintf(salida,"-1 WHERE requerido");
		return;
	}

	nfiltros = 0;
	for (i = 0;i < wheres.ncond;i++)
	{						
		elfiltro = saca_sql_campo(&from,wheres.where[i].campo,&campo);
		if (elfiltro < 0)
		{
			sprintf(salida,"-1 No se encuentra campo '%s'",wheres.where[i].campo);
			return;
		}
		nfiltros++;
	}

    filtros = (struct grid_filtro *)malloc(sizeof(struct grid_filtro) * (nfiltros+1));
	memset(filtros,0,sizeof(struct grid_filtro) * (nfiltros+1));

	nfiltros = 0;
	for (i = 0;i < wheres.ncond;i++)
	{						
		elfiltro = saca_sql_campo(&from,wheres.where[i].campo,&campo);
		nf = nfiltros;
		nfiltros++;

		filtros[nf].campo = campo;
		filtros[nf].tipo = wheres.where[i].cond;
		if (wheres.where[i].esor)
		{							
			filtros[nf].tipo |= 0x40;
		}
		filtros[nf].mayus = 0;
		filtros[nf].inver = wheres.where[i].inverso;
		if (!(filtros[nf].tipo))
		{
			strcpy(filtros[nf].valor,"");
			strcpy(filtros[nf].limi,wheres.where[i].valor);
			strcpy(filtros[nf].lims,wheres.where[i].valor1);
		}
		else
		{
			strcpy(filtros[nf].limi,"");
			strcpy(filtros[nf].lims,"");
			strcpy(filtros[nf].valor,wheres.where[i].valor);
		}
	}


	i = ind_rel(into.into);
	{
		char tmp[300];
		pfi = NULL;
		if (i >= 0)
		{
			strcpy(tmp,v_mpfi(i)->f_PathDef);
		}
		else
		{
			strcpy(tmp,dirdef);
			strcat(tmp,into.into);
		}
		i = lee_defn(tmp,&pfi);
		if (i >= 0)
		{
			i = ind_rel(into.into);				
			if (i < 0) {
				cambia_path_fich(pfi->f_fichero);
				i = 0;
			}
			else
			{
				strcpy(pfi->f_fichero,v_mpfi(i)->f_fichero);
			}
			i = lee_defn(tmp,&pfitest);			
		}
	}
	if (i < 0 || !pfi)
	{
		if (pfi) {
			free_defpri(pfi);
		}
		sprintf(salida,"-1 No se encuentra tabla '%s'",into.into);
		return;
	}	

	cargado = 1;
	if (abre_ind(pfi,ISINOUT + ISMANULOCK) < 0)
	{
		cargado = 0;
	}
	if (cargado) {
		int key;
		int porcontenido = -1;
		char *buf1 = NULL;
		char *buf2 = NULL;		

		key = crea_limites_buf(nfiltros,filtros,pfi,&buf1,&buf2);

		if (key < 0) {
			porcontenido = busca_contenido_buf(nfiltros,filtros,pfi,&key);
		}

		defecto_ficha(pfi);

		if (porcontenido > -1)
        {
            ixx_pon_contenido(pfi->f_fd,filtros[porcontenido].valor,strlen(filtros[porcontenido].valor));
        }

		if (key > -1) {			
			if (buf1) {
				memcpy(pfi->f_buf,buf1,pfi->f_lfich);
				er = ixx_lee_indice(pfi->f_fd,pfi->f_buf,ISGTEQ,key,1,0,1);
			}
			else
				er = ixx_lee_indice(pfi->f_fd,pfi->f_buf,ISFIRST,key,1,0,1);
		}
		else
			er = ixx_lee_indice(pfi->f_fd,pfi->f_buf,ISFIRST,-1,1,0,1);
		while (er >= 0) {
			if (key > -1 && !esta_en_limites_buf(pfi,buf1,buf2,key)) {
				if (salta_limites_buf(pfi,buf1,buf2,key)) {
					er = -1;
					break;
				}
			}
			if (evaluador_simple_de_filtro(pfi,nfiltros,filtros,pfitest) == 0){
				borra_reg(pfi);
			}
			er = ixx_lee_indice(pfi->f_fd,pfi->f_buf,ISNEXT,-1,1,0,1);
		} 
		if (porcontenido > -1) {
			ixx_fin_contenido(pfi->f_fd);
		}
		cierra_ind(pfi);
		if (buf1)
			free(buf1);
		if (buf2)
			free(buf2);
		if (cargado)
			sprintf(salida,"0 OK");
	}
	else
		sprintf(salida,"-1 No se puede abrir la tabla '%s'",into.into);

	free_defpri(pfi);
	free_defpri(pfitest);
	
}

static void sql_procesa_select(char *salida,char *comando,FI_DEF **_fires,int (*out_fun)(),void *param)
{
	char *_sentencia = sql_recoge_sentencia(comando);
	char *sentencia;
	char *p1,*p2,*p3,cc;
	char tmp[1024];
	int condicion;
	int sin_salida = 0;		
	int distinct = 0;

	origen = NULL;
	relacion = NULL;

	memset(&campos,0,sizeof(campos));
	memset(&from,0,sizeof(from));
	memset(&into,0,sizeof(into));
	memset(&wheres,0,sizeof(wheres));
	memset(&orderbys,0,sizeof(orderbys));

	sentencia = _sentencia;
    while(*sentencia == ' ') 
		sentencia++;

	/* Campos */
	/*
	if (!memcmp(sentencia,"ALL ",4))
	{
		strcpy(campos.modif,"ALL");
		sentencia += 4;
	}
	else */if (!memcmp(sentencia,"DISTINCT ",9))
	{
		strcpy(campos.modif,"DISTINCT");
		distinct = 1;
		sentencia += 9;
	}
    while(*sentencia == ' ') 
		sentencia++;

	p1 = sentencia;
	do
	{
		sentencia = sql_descomillador(sentencia);
		while(*sentencia && *sentencia != ' ') 
			sentencia++;
		p2 = sentencia;
		while(*sentencia == ' ') 
			sentencia++;
		sentencia = sql_descomillador(sentencia);
		while(*sentencia == ' ') 
			sentencia++;
		if (*sentencia != ',')
		{		
			if (memcmp(sentencia,"AS ",3))
			{
				break;
			}		
			sentencia += 3;
		}
		else
			sentencia++;
		while(*sentencia == ' ') 
			sentencia++;
		sentencia = sql_descomillador(sentencia);
		while(*sentencia == ' ') 
			sentencia++;
	} while(-1);
	/* de p1 a p2 area de campos */
	campos.ncampos = 0;
	p3 = p1;
	while(p3 <= p2)
	{
		p3 = sql_descomillador(p3);
		if (p3 == p2 || *p3 == ',')
		{
			memcpy(tmp,p1,(int)(p3-p1));
		    tmp[(int)(p3-p1)] = 0;
			for (p1 = tmp;*p1;p1++)
			{
				p1 = sql_descomillador(p1);
				if (!*p1) break;
				if (!memcmp(p1,"AS ",3))
				{
					*p1 = 0;
					p1 += 3;
					break;
				}
			}
			quita_blancos(tmp);
			quita_blancos(p1);
			strcpy_sincomillas(campos.campos[campos.ncampos].id,tmp);
			strcpy_sincomillas(campos.campos[campos.ncampos].alias,p1);
			campos.ncampos++;
			p1 = p3 + 1;
		}
		p3++;
	}	

	if (!memcmp(sentencia,"FROM ",5))
	{
		sentencia += 5;		
		while(*sentencia == ' ') 
			sentencia++;		

		from.nfrom = 0; 
		p1 = sentencia;
		do
		{
			sentencia = sql_descomillador(sentencia);
			while(*sentencia && *sentencia != ' ') 
				sentencia++;
			p2 = sentencia;
			while(*sentencia == ' ') 
				sentencia++;
			sentencia = sql_descomillador(sentencia);
			while(*sentencia == ' ') 
				sentencia++;
			if (*sentencia != ',')
			{
				break;
			}
			while(*sentencia == ' ') 
				sentencia++;
			sentencia = sql_descomillador(sentencia);
			while(*sentencia == ' ') 
				sentencia++;
		} while(-1);

		p3 = p1;
		while(p3 <= p2)
		{
			p3 = sql_descomillador(p3);
			if (p3 == p2 || *p3 == ',')
			{
				memcpy(tmp,p1,(int)(p3-p1));
				tmp[(int)(p3-p1)] = 0;
				for (p1 = tmp;*p1;p1++)
				{
					p1 = sql_descomillador(p1);
					if (!*p1) break;
				}
				quita_blancos(tmp);				
				strcpy_sincomillas(from.from[from.nfrom],tmp);				
				(from.nfrom)++;
				p1 = p3 + 1;
			}
			p3++;
		}
	}
    
	if (!memcmp(sentencia,"INTO ",5))
	{
		sentencia += 5;		
		while(*sentencia == ' ') 
			sentencia++;
		p1 = sentencia;
		sentencia = sql_descomillador(sentencia);
		while(*sentencia && *sentencia != ' ') 
			sentencia++;		
		memcpy(into.into,p1,(int)(sentencia-p1));
	    into.into[(int)(sentencia-p1)] = 0;
		quita_comillas(into.into);
		while(*sentencia == ' ') 
			sentencia++;
		p1 = into.into;
		while(*p1 && *p1 != '.')
			p1++;
		into.tipo = 0;
		if (*p1 == '.')
		{
			*p1 = 0;
			p1++;
			if (!strcmp(p1,"dat"))
			{
				into.tipo = 1;
			}
			else if (!strcmp(p1,"def"))
			{
				into.tipo = 2;
			}
			else
			{
				into.tipo = 999;
			}
		}
	}
	
	if (!memcmp(sentencia,"WHERE ",6))
	{
		sentencia += 6;
		p1 = sentencia;
		sql_select_setencia(&sentencia);
		p2 = p1;
		wheres.ncond = 0;

		while(p1 < sentencia)
		{
			p1 = sql_descomillador(p1);			
			if (!memcmp(p1,"AND ",4) || !memcmp(p1,"OR ",3) || (p1+1) >= sentencia)
			{
				if (!memcmp(p1,"OR ",3))
					condicion = 1;
				else
					condicion = 0;
				if ((p1+1) >= sentencia)
				{
					p1 = sentencia;
				}
				wheres.where[wheres.ncond].inverso = 0;
				wheres.where[wheres.ncond].campo[0] = 0;
				wheres.where[wheres.ncond].valor[0] = 0;
				wheres.where[wheres.ncond].valor1[0] = 0;
				wheres.where[wheres.ncond].cond = 0;
                wheres.where[wheres.ncond+1].esor = condicion;
				if (!memcmp(p2,"NOT ",4))
				{
					wheres.where[wheres.ncond].inverso = 1;
					p2 += 4;
				}
				for(p3 = p2;*p3;p3++)
				{
					p3 = sql_descomillador(p3);
					if (!memcmp(p3," LIKE ",6))
					{
						wheres.where[wheres.ncond].cond = 3;
						cc = *p3;
						*p3 = 0;
						strcpy_sincomillas(wheres.where[wheres.ncond].campo,p2);
						*p3 = cc;
						cc = *p1;
						*p1 = 0;
						strcpy_sincomillas(wheres.where[wheres.ncond].valor,p3+6);
						*p1 = cc;
						break;
					}
					else if (!memcmp(p3," == ",4))
					{
						wheres.where[wheres.ncond].cond = 1;
						cc = *p3;
						*p3 = 0;
						strcpy_sincomillas(wheres.where[wheres.ncond].campo,p2);
						*p3 = cc;
						cc = *p1;
						*p1 = 0;
						strcpy_sincomillas(wheres.where[wheres.ncond].valor,p3+4);
						*p1 = cc;
						break;
					}
					else if (!memcmp(p3," = ",3))
					{
						wheres.where[wheres.ncond].cond = 2;
						cc = *p3;
						*p3 = 0;
						strcpy_sincomillas(wheres.where[wheres.ncond].campo,p2);
						*p3 = cc;
						cc = *p1;
						*p1 = 0;
						strcpy_sincomillas(wheres.where[wheres.ncond].valor,p3+3);
						*p1 = cc;
						break;
					}					
					else if (!memcmp(p3," BETWEEN ",9))
					{
						wheres.where[wheres.ncond].cond = 0;
						cc = *p3;
						*p3 = 0;
						strcpy_sincomillas(wheres.where[wheres.ncond].campo,p2);
						*p3 = cc;
						p2 = p3 + 9;
						p3 = p2;
						p3 = sql_descomillador(p3);
						while (p3 < p1 && *p3 != ',')
						{
						   p3++;
						   p3 = sql_descomillador(p3);						   
						}
						cc = *p3;
						*p3 = 0;
						strcpy_sincomillas(wheres.where[wheres.ncond].valor,p2);
						*p3 = cc;
						cc = *p1;
						*p1 = 0;
						strcpy_sincomillas(wheres.where[wheres.ncond].valor1,p3+1);
						*p1 = cc;
						break;
					}

				}				
				(wheres.ncond)++;
				if (condicion)
					p1 += 2;
				else
				    p1 += 3;
				p2 = p1+1;
			}
			p1++;
		}
	}

	if (!memcmp(sentencia,"ORDER BY ",9) || !memcmp(sentencia,"GROUP BY ",9))
	{
		int nc,no;
		int agrupado = 0;
		if (!memcmp(sentencia,"GROUP BY ",9))
			orderbys.agrupado = 1;
		else
			orderbys.agrupado = 0;
		sentencia += 9;

		p1 = sentencia;
		sql_select_setencia(&sentencia);
		p2 = p1;		
		orderbys.norders = 0;
		orderbys.orderby[0].ncampos = 0;

		while(p1 < sentencia)
		{
			p1 = sql_descomillador(p1);			
			if (*p1 == ',' || (p1+1) >= sentencia || !memcmp(p1," AND ",5))
			{
				if ((p1+1) >= sentencia)
				{
					p1 = sentencia;
				}
				cc = *p1;
				*p1 = 0;
				quita_blancos(p2);
				no = orderbys.norders;
				nc = orderbys.orderby[no].ncampos;
				strcpy_sincomillas(orderbys.orderby[no].campos[nc],p2);
				orderbys.orderby[no].ncampos++;
				*p1 = cc;
				p2 = p1 + 1;
			}
			else if (!memcmp(p1,"AND ",4))
			{
				p1 += 4;
				p2 = p1;
				if (orderbys.orderby[orderbys.norders].ncampos)
				{
					orderbys.norders++;
					orderbys.orderby[orderbys.norders].ncampos = 0;
				}
			}
			p1++;
		}
		if (orderbys.orderby[orderbys.norders].ncampos)
		{
			orderbys.norders++;
		}
	}

	if (!from.nfrom)
	{
		sprintf(salida,"-1 From requerido");
	}
	else
	{		/* a pelo: conversion a filtro y filtrado igual que consulta */
		int i,j,k,ii,jj;
		FI_DEF *pfi;
		FI_DEF *padrepfis[100];
		int padrecargado[100];
		int nfiltros[100];
		int elfiltro,campo;
		int nf;
		struct grid_filtro *filtros[100];

		for (nf = 0;nf < from.nfrom;nf++)
		{
			padrecargado[nf] = 0;
		}

		for (i = 0;i < from.nfrom;i++)
		{						
			nfiltros[i] = 0;
		}

		for (i = 0;i < wheres.ncond;i++)
		{						
			elfiltro = saca_sql_campo(&from,wheres.where[i].campo,&campo);
			if (elfiltro < 0)
			{
				sprintf(salida,"-1 No se encuentra campo '%s'",wheres.where[i].campo);
				break;
			}
			nfiltros[elfiltro]++;
		}

		if (i < wheres.ncond)		
			nf = 0;
		else
		for (nf = 0;nf < from.nfrom;nf++)
		{			
			i = ind_rel(from.from[nf]);
			/*if (i < 0)*/
			{
				char tmp[300];
				padrepfis[nf] = NULL;
				if (i >= 0)
				{
					strcpy(tmp,v_mpfi(i)->f_PathDef);
				}
				else
				{
					strcpy(tmp,dirdef);
					strcat(tmp,from.from[nf]);
				}
				i = lee_defn(tmp,&(padrepfis[nf]));				
				if (i >= 0)
				{
					i = ind_rel(from.from[nf]);
					padrecargado[nf] = 1;
					if (i < 0) {
						cambia_path_fich(padrepfis[nf]->f_fichero);
						i = 0;
					}
					else
					{
						strcpy(padrepfis[nf]->f_fichero,v_mpfi(i)->f_fichero);
					}
					i = 0;
				}
			}
			/*
			else
			{
				padrepfis[nf] = v_mpfi(i);
				if (padrepfis[nf]->f_fd != -1)
				{
					apunta_ind(padrepfis[nf],1,ISFIRST,0);
				}
			}
			*/

			/*
			i = ind_rel(from.from[nf]);
			if (i < 0)
			{
				char tmp[300];
				padrepfis[nf] = NULL;
				strcpy(tmp,dirdef);
				strcat(tmp,from.from[nf]);
				i = lee_defn(tmp,&(padrepfis[nf]));				
				if (i >= 0)
				{
					padrecargado[nf] = 1;
					cambia_path_fich(padrepfis[nf]->f_fichero);
				}
			}
			else
			{
				padrepfis[nf] = v_mpfi(i);
				if (padrepfis[nf]->f_fd != -1)
				{
					apunta_ind(padrepfis[nf],1,ISFIRST,0);
				}
			}
			*/

			if (i < 0)
			{
				sprintf(salida,"-1 No se encuentra tabla '%s'",from.from[nf]);
				break;
			}
			defecto_ficha(padrepfis[nf]);
		}

		if (nf >= from.nfrom)
		{
			pfi = NULL;
			i = 0;
			if (campos.ncampos == 1)
			{
				if (from.nfrom == 1 && (!strcmp(campos.campos[0].id,"*") || strcmp(campos.campos[0].id,"ALL")))
				{
					i = 1;
				}
				else if (campos.campos[0].id[0] == '*' && campos.campos[0].id[1] == '.' &&
					     !strcmp(campos.campos[0].id+2,from.from[0]))
				{
					i = 1;
				}
			}			

			/* Crear def destino ... */
			if (i)
			{   /* con copiar el primer origen es suficiente */
				if (lee_defn(padrepfis[0]->f_PathDef,&pfi) >= 0)
				{
					/*pfi->f_nclaves = 1; /* cpon la clave primaria nos basta */
					strcpy(pfi->f_PathDef,padrepfis[0]->f_PathDef);
					if (into.tipo == 2)
					{					
						strcpy(pfi->f_fichero,into.into);
						cambia_path_fich(pfi->f_fichero);
						strcpy(quita_path(pfi->f_PathDef),into.into);
						graba_defn(pfi->f_PathDef,pfi);
					}
					else
					{
						strcpy(pfi->f_fichero,into.into);
						quitab(pfi->f_fichero);
						if (pfi->f_fichero[0])
							cambia_path_fich(pfi->f_fichero);
					}
				}
			}
			else
			{
				pfi = (FI_DEF *)malloc(sizeof(FI_DEF));
				memset((char *)pfi,0,sizeof(FI_DEF));
				pfi->f_titulo = strmalloc(1);
				pfi->f_titulo[0] = 0;
				j = /*padrepfis[0]->f_clave[0].k_ncampos*/ 1 + campos.ncampos;
				origen = (struct _sql_origen *)malloc(sizeof(struct _sql_origen)*j);
				for (i = 0;i < campos.ncampos;i++)
				{
					elfiltro = saca_sql_campo(&from,campos.campos[i].id,&campo);
					if (elfiltro < 0)
					{
						break;
					}
					origen[i].ipadre = elfiltro;
					origen[i].icampo = campo;
				}
				if (i >= campos.ncampos)
				{
					char *ptmp1;
					for (i = 0;i < campos.ncampos;i++)
					{
						elfiltro = saca_sql_campo(&from,campos.campos[i].id,&campo);
						ptmp1 = campos.campos[i].alias;
						if (ptmp1)
						{
							quitab(ptmp1);
							if (!*ptmp1)
							{
								ptmp1 = padrepfis[elfiltro]->f_campos[campo].c_nombre;
							}
						}
						else
							ptmp1 = padrepfis[elfiltro]->f_campos[campo].c_nombre;
						inserta_campo(pfi,i,campos.campos[i].id,ptmp1,
									   padrepfis[elfiltro]->f_campos[campo].c_qc,
									   padrepfis[elfiltro]->f_campos[campo].c_minimo,
									   padrepfis[elfiltro]->f_campos[campo].c_maximo,
									   padrepfis[elfiltro]->f_campos[campo].c_mante.c_defecto);
					}

					/* la clave primaria = contador de resultados */
					inserta_campo(pfi,i,"_QUERY_REG",
										"Registro",
										0,
										"0",
										"999999999999",
										"0");
					origen[i].ipadre = -1;
					origen[i].icampo = -1;			
					pfi->f_nclaves = 1;
					pfi->f_clave = (KEY_DEF *)reserva_malloc(sizeof(KEY_DEF)*(1+orderbys.norders),3); 
					pfi->f_clave[0].k_ncampos = 1;
					pfi->f_clave[0].k_fflag = 0;
					pfi->f_clave[0].k_campo = (short *)malloc(sizeof(short) * 2);
					pfi->f_clave[0].k_orden = (short *)malloc(sizeof(short) * 2);
					pfi->f_clave[0].k_orden[0] = 1;
					pfi->f_clave[0].k_campo[0] = i+1;
					if (orderbys.norders)
					{
						pfi->f_nclaves = orderbys.norders+1;

						for (jj = 0;jj < orderbys.norders;jj++)
						{						
							pfi->f_clave[jj+1].k_ncampos = orderbys.orderby[jj].ncampos;
							pfi->f_clave[jj+1].k_fflag = 0;
							pfi->f_clave[jj+1].k_campo = (short *)malloc(sizeof(short) * (orderbys.orderby[jj].ncampos+1));
							pfi->f_clave[jj+1].k_orden = (short *)malloc(sizeof(short) * (orderbys.orderby[jj].ncampos+1));
							for (j = 0;j < orderbys.orderby[jj].ncampos;j++)
							{
								elfiltro = saca_sql_campo(&from,orderbys.orderby[jj].campos[j],&campo);
								for (k = 0;k < campos.ncampos;k++)
								{
									if (origen[k].ipadre == elfiltro && origen[k].icampo == campo)
										break;
								}								
								pfi->f_clave[jj+1].k_campo[j] = k+1;
								pfi->f_clave[jj+1].k_orden[j] = 1;
							}
						}
					}


					for (i=pfi->f_nclaves;i>0;i--) 
					{
						for (j=pfi->f_clave[i-1].k_ncampos;j>0;j--) 
						{
							if((k = pfi->f_clave[i-1].k_campo[j-1]) <= pfi->f_ncampos && k)
								pfi->f_campos[k-1].c_key = i;   
						}
					}

					/* copiar campos clave primaria def principal */
					/*
					pfi->f_nclaves = 1;
					pfi->f_clave = (KEY_DEF *)reserva_malloc(sizeof(KEY_DEF),3);
					pfi->f_clave[0].k_ncampos = padrepfis[0]->f_clave[0].k_ncampos;
					pfi->f_clave[0].k_fflag = padrepfis[0]->f_clave[0].k_fflag;
					pfi->f_clave[0].k_campo = (short *)malloc(sizeof(short) * pfi->f_clave[0].k_ncampos);
					pfi->f_clave[0].k_orden = (short *)malloc(sizeof(short) * pfi->f_clave[0].k_ncampos);
					for (j = 0;j < padrepfis[0]->f_clave[0].k_ncampos;j++)
					{
						campo = padrepfis[0]->f_clave[0].k_campo[j] - 1;
						pfi->f_clave[0].k_orden[j] = padrepfis[0]->f_clave[0].k_orden[j];
						pfi->f_clave[0].k_campo[j] = i + 1;
						inserta_campo(pfi,i,padrepfis[0]->f_campos[campo].c_lista,
										   padrepfis[0]->f_campos[campo].c_nombre,
										   padrepfis[0]->f_campos[campo].c_qc,
										   padrepfis[0]->f_campos[campo].c_minimo,
										   padrepfis[0]->f_campos[campo].c_maximo,
										   padrepfis[0]->f_campos[campo].c_mante.c_defecto);
						origen[i].ipadre = 0;
						origen[i].icampo = campo;
						i++;
					}
					for (i=0;i<pfi->f_ncampos;i++)
						pfi->f_campos[i].c_key = 0;
					for (i=pfi->f_nclaves;i>0;i--) 
					{
						for (j=pfi->f_clave[i-1].k_ncampos;j>0;j--) 
						{
							if((k = pfi->f_clave[i-1].k_campo[j-1]) <= pfi->f_ncampos && k)
								pfi->f_campos[k-1].c_key = i;   
						}
					}
					*/
					if (into.tipo == 2)
					{
						strcpy(pfi->f_fichero,into.into);
						strcpy(pfi->f_PathDef,padrepfis[elfiltro]->f_PathDef);
						cambia_path_fich(pfi->f_fichero);
						strcpy(quita_path(pfi->f_PathDef),into.into);
						graba_defn(pfi->f_PathDef,pfi);
					}
					else
					{
						if (!into.into[0])
						{
							if (out_fun)
								sin_salida = 1;							
							strcpy(pfi->f_fichero,"__INTERN");
						}
						else
						{
							strcpy(pfi->f_fichero,into.into);
						}
						cambia_path_fich(pfi->f_fichero);
					}
				}
			}
			if (pfi)
			{
				defecto_ficha(pfi);
				pfi->f_fd = -1;
				if (!sin_salida)
				{
					borra_ind(pfi);
					rw_base_unlink(pfi->f_fichero);
				}
				if (sin_salida || !pfi->f_fichero[0] || crea_ind(pfi) >= 0)
				{
					for (i = 0;i < from.nfrom;i++)
					{					     
					    filtros[i] = (struct grid_filtro *)malloc(sizeof(struct grid_filtro) * (nfiltros[i]+1));
					    memset(filtros[i],0,sizeof(struct grid_filtro) * (nfiltros[i]+1));
						nfiltros[i] = 0;
					}

					for (i = 0;i < wheres.ncond;i++)
					{						
						elfiltro = saca_sql_campo(&from,wheres.where[i].campo,&campo);
						nf = nfiltros[elfiltro];
						nfiltros[elfiltro]++;

						filtros[elfiltro][nf].campo = campo;
						filtros[elfiltro][nf].tipo = wheres.where[i].cond;
						if (wheres.where[i].esor)
						{							
							filtros[elfiltro][nf].tipo |= 0x40;
						}
						filtros[elfiltro][nf].mayus = 0;
						filtros[elfiltro][nf].inver = wheres.where[i].inverso;
						if (!(filtros[elfiltro][nf].tipo))
						{
							strcpy(filtros[elfiltro][nf].valor,"");
							strcpy(filtros[elfiltro][nf].limi,wheres.where[i].valor);
							strcpy(filtros[elfiltro][nf].lims,wheres.where[i].valor1);
						}
						else
						{
							strcpy(filtros[elfiltro][nf].limi,"");
							strcpy(filtros[elfiltro][nf].lims,"");
							strcpy(filtros[elfiltro][nf].valor,wheres.where[i].valor);
						}
					}
					
					relacion = (struct _sql_relacion *)malloc(sizeof(struct _sql_relacion)*from.nfrom);
					memset((char *)relacion,0,sizeof(struct _sql_relacion)*from.nfrom);
					for (i = 0;i < from.nfrom;i++)
					{
						for (k = 0;k < padrepfis[i]->f_nrelaci;k++)
						{
							for (j = 1;j < from.nfrom;j++)
							{
								if (j != i)
								{
									if (!strcmp(quita_path(padrepfis[i]->f_relaci[k].r_fichero),from.from[j]))
									{
										break;
									}
								}
							}
							if (j < from.nfrom)
							{
								ii = relacion[i].nrel;
								relacion[i].nrel++;
								relacion[i].frel[ii] = j;								
								relacion[i].hrel[ii] = 1;
							    for (j = 0;j < MAXCAMPOSPORCLAVE;j++)
								{
								   relacion[i].rel[ii][j] = padrepfis[i]->f_relaci[k].r_campo[j];
								   if (!relacion[i].rel[ii][j])
									   break;
								   relacion[i].rel[ii][j]--;
								   relacion[i].ncrel[ii]++;
								}							 
							}
						}
					}

					{
						char *buffers[100];

						for (i = 0;i < from.nfrom;i++)
						{
							buffers[i] = malloc(padrepfis[i]->f_lfich+1);
						}

						//if (strcmp(quita_path(padrepfis[0]->f_PathDef),"dsviw095"))
						//{
						//	aviso(0,quita_path(padrepfis[0]->f_PathDef));
						   sprintf(salida,"0 Seleccionados:%d",sql_select_ejecuta(pfi,padrepfis,nfiltros,filtros,origen,relacion,buffers,NULL,0,out_fun,param,from.nfrom,orderbys.agrupado,distinct));
						//}
						//else
						//	aviso(0,"PUM!");

						for (i = 0;i < from.nfrom;i++)
						{
							free(buffers[i]);
						}

					}

					for (i = 0;i < from.nfrom;i++)
					{
						if (relacion[i].ofd == 1)
						{
							cierra_ind(padrepfis[i]);
						}
						free((char *)filtros[i]);
					}
					if (_fires)
					{		
						*_fires = pfi;
						pfi = NULL;
					}
					else
					{
						if (!sin_salida && pfi->f_fichero[0])
						{
							cierra_ind(pfi);
						}
					}
				}
				else
				{
					if (!sin_salida)
					{
						borra_ind(pfi);
						rw_base_unlink(pfi->f_fichero);					
					}
					sprintf(salida,"-1 No se puede crear tabla destino '%s'",pfi->f_fichero);
				}				
				if (pfi)
				{
					free_defpri(pfi)					;
				}
			}
			else
			{
				sprintf(salida,"-1 No se puede crear tabla de destino");
			}
		}
		for (nf  = 0;nf < from.nfrom;nf++)
		{
			if (padrecargado[nf])
			{
				free_defpri(padrepfis[nf]);
			}
		}
		if (origen)
		{
			free((char *)origen);
			origen = NULL;
		}
		if (relacion)
		{
			free((char *)relacion);
			relacion = NULL;
		}
	}

	free(_sentencia);
}

void dsdb_sql(char *origen,char *salida,FI_DEF **fires,int (*out_fun)(),void *param)
{	
	char *parsed = origen;
	int haymas;
	int comando;
	
	salida[0] = 0;	

	comando = sql_saca_comando(&parsed,&haymas);	

	switch(comando)
	{
	case 0:/* version */
		strcpy(salida,"0 VERSION 1.1");
		if (haymas)
		{
			strcat(salida," argumentos extra ignorados");
		}
		break;
	case 1:/* select */
		if (!haymas)
		{
			strcpy(salida,"-1 SELECT sin argumentos");
		}
		else
		{
			sql_procesa_select(salida,parsed,fires,out_fun,param);
		}
		break;
	case 2:/* insert */
		sql_procesa_insert(salida,parsed);
		break;
    case 3: /* update */
		sql_procesa_update(salida,parsed);
	    break;
    case 4: /* delete */
		sql_procesa_delete(salida,parsed);
		break;
	default:
		strcpy(salida,"-1 Comando no implementado");
		break;
	}	
}

int ds_sql(int p,int tip,int faclin)
{
    int cc = prog[p].indice;
	short f0 = -1,c0 = -1;
	char origen[1024];
	char salida[300];

	origen[0] = 0;
	salida[0] = 0;
    comoes_cam(cc,NULL,NULL,NULL,&f0,&c0,NULL);
	if (c0 != -1)
	{
		if (f0 == -1) 
		{
		   if (que_memo_tipo(c0) == DOUBLETIPO)
			  sprintf(origen,"%lf",v_memo(c0)->numerica);
		   else
			  strcpy(origen,v_memo(c0)->alfabetica);
		}
		else 
		{
		   ld_campo(c0,v_mpfi(f0),origen);
		}		

		dsdb_sql(origen,salida,NULL,NULL,NULL);

		strcpy(v_memo(5004)->alfabetica,salida);
	}
	else
	{
		strcpy(v_memo(5004)->alfabetica,"-1 Error Interno 1");
	}
	n_np = 1;
	return 0;
}

struct data_itcp_lista_def
{
	void *_pfi;
	void *_InfoCampos;
	int cargado;
	int opcion;
	int operacion;
	int n;
	int hechos;
	void *lim1;
	void *lim2;
};

int dsitcp_salida_sql(FI_DEF *pfi,void *_client)
{
	int i;
	int n;
	char tmp[512];
	SOCKET client = (SOCKET)_client;

	for (i = 0;i < pfi->f_ncampos-1;i++)
	{
		formato_campo(pfi,i,tmp);
		n = long_campo(pfi,i);
		tmp[n] = 0;
		rwi_senddata(client,tmp,-1);
	}
    rwi_senddata(client,"\r\n",-1);

	return 0;
}


static int prime_dsitcp_salida_dssql = 1;
int dsitcp_salida_dssql(FI_DEF *pfi,void *_client)
{
	int i;
	int n;
	char tmp[512];
	char tmp2[512];
	char salida[20480];
	char *p;
	unsigned char *pp;

	SOCKET client = (SOCKET)_client;

	if (prime_dsitcp_salida_dssql) {		
		prime_dsitcp_salida_dssql = 0;
		salida[0] = 0;
		p = salida;

		for (i = 0;i < pfi->f_ncampos-1;i++) {
			tmp[0] = 0;
			if (pfi->f_campos[i].c_lista[0] == 13)	{
			   strcpy(tmp,pfi->f_campos[i].c_lista+1);
			}
			else if (pfi->f_campos[i].c_lista[0]) {
			   strcpy(tmp,pfi->f_campos[i].c_lista);
			}
			else if (pfi->f_campos[i].c_nombre){
			   strcpy(tmp,pfi->f_campos[i].c_nombre);
			}
			/*tmp[10] = 0;*/
			tmp2[0] = 0;
			switch(pfi->f_campos[i].c_tipo) {
				case FECHATIPO:
					 strcpy(tmp2,"@Date;");
					 break;
				case INTTIPO:
					 strcpy(tmp2,"@Int;");
					 break;
				case FLOATTIPO:
				case DOUBLETIPO:
					 strcpy(tmp2,"@Double;");
					 break;
				default:
					 strcpy(tmp2,"@String;");
					 break;
			}
			strcat(tmp2,tmp);							
			if (pfi->f_campos[i].c_nombre) {
				strcpy(tmp,pfi->f_campos[i].c_nombre);
				quitab(tmp);
				if (tmp[0])
					sprintf(tmp2+strlen(tmp2),"[%s]",tmp2,tmp);
			}
			if (((int)(p-salida)+strlen(tmp2)+3) >= sizeof(salida))
				break;
			if (i)
				sprintf(p,"%c%s",8,tmp2);
			else
				sprintf(p,"%s",tmp2);			
			p += strlen(p);
		}
		strcpy(p,"\n");
		rwi_senddata(client,salida,-1);
	}

    salida[0] = 0;
	p = salida;
	for (i = 0;i < pfi->f_ncampos-1;i++) {
		if (pfi->f_campos[i].c_tipo == FECHATIPO) {
             int idx = pfi->f_campos[i].c_offset;
			   memcpy(tmp2,pfi->f_buf+idx,4);
			   tmp2[4] = '-';
			   memcpy(tmp2+5,pfi->f_buf+idx+4,2);
			   tmp2[7] = '-';
			   memcpy(tmp2+8,pfi->f_buf+idx+6,2);
			   tmp2[10] = 0;
		}
		else {
			ld_campo(i,pfi,tmp2);									
			tmp2[257] = 0;
			for (pp = (unsigned char *)tmp2;*pp;pp++)
		       oem_a_ansi(pp);
			for (pp = (unsigned char *)tmp2;*pp;pp++) {
			   if (*pp < 14)
				  *pp = ' ';
			}
		}
		if (((int)(p-salida)+strlen(tmp2)+3) >= sizeof(salida))
			break;
		if (i)
			sprintf(p,"%c%s",8,tmp2);
		else
			sprintf(p,"%s",tmp2);		
		p += strlen(p);
	}
	strcpy(p,"\n");
	rwi_senddata(client,salida,-1);

	return 0;
}


int itcp_lista_def(SOCKET client,char *nomdef,int operacion,struct data_itcp_lista_def **_data)
{
	int cargado = 0;
	long k = 0,i;
	int j,n,key;
	char tmp[512];
	FI_DEF *pfi = NULL,*pfi2;
	struct Info_Entra_Campos *InfoCampos = NULL;
	int opcion = 0;
	struct data_itcp_lista_def *data = NULL;
	int no_liberar = 0;
	int ok = 0;	
	char *pcoma = NULL;

	if (_data)
	{
		data = *_data;
		no_liberar = 1;
	}
	if (data)
	{				
		InfoCampos = (struct Info_Entra_Campos *)data->_InfoCampos;
		pfi = (FI_DEF *)data->_pfi;
		cargado = data->cargado;
		opcion = data->opcion;		
	}
	if (!pfi)
	{
		for (i = 0;nomdef[i];i++)
		{
			if (nomdef[i] == ',')
			{
				nomdef[i] = 0;
				pcoma = nomdef + i + 1;
				break;
			}
		}
		quitab(nomdef);
		quitabp(nomdef);
		cargado = 0;
		opcion = 0;
		j = strlen(nomdef);
		if (j > 0)
		{
			j--;
			if (nomdef[j] == '@')
			{			
				strcpy(tmp,nomdef);							
				tmp[j] = 0;
				j = ind_rel(tmp);
				if (j >= 0)
				{
					pfi = v_mpfi(j);
					cargado = 1;
				}
			}
		}
		if (!cargado)
		{
			strcpy(tmp,dirdef);
			strcat(tmp,nomdef);
			pfi = NULL;
			if (lee_defn(tmp,&pfi) >= 0) 
			{					
				cargado = 2;
			}
		}
	}
	else
	{
		cargado = 1;
	}
	if (cargado)
	{
		if (cargado == 2)
		{
			if (!opcion)
			{
				cambia_path_fich(pfi->f_fichero);
				/*cambia_nom_fich(pfi->f_fichero,(int)par);*/
				if (abre_ind(pfi,ISINOUT + ISMANULOCK) < 0)
				{
					if (crea_ind(pfi) < 0)
					{
						cargado = 0;
					}
				}
			}
		}
		if (cargado)
		{
			int nreg,mem,lreg;

			if (!InfoCampos)
			{
				mem = RellenaInfoDef(NULL,pfi,1,1,&lreg,NULL);
				lreg = 0;
				InfoCampos = (struct Info_Entra_Campos *)malloc(mem);
				lreg = 0;
				RellenaInfoDef(InfoCampos,pfi,1,1,&lreg,NULL);
			}			

			if (!operacion)
			{
				nreg = tam_btrv(pfi);
				sprintf(tmp,"%d\r\n",nreg);
				rwi_senddata(client,tmp,-1);
				if (lee_reg(pfi,ISFIRST) >= 0)
				{
					k = 0;
					do
					{						
						for (i = 0;i < InfoCampos->n_campos;i++)
						{
							formato_campo(pfi,dInfoCampo(InfoCampos,i,0)->Id,tmp);
							tmp[dInfoCampo(InfoCampos,i ,0)->c_maxqc] = 0;
							rwi_senddata(client,tmp,-1);
							/*
							j = dInfoCampo(InfoCampos,i ,0)->c_maxqc;
							buffer += j;
							tammem += j;
							*/
						}						
						k++;
						rwi_senddata(client,"\r\n",-1);
						/*(*plx)++;*/						
					} while(k < nreg && lee_reg(pfi,ISNEXT) >= 0);

					for(;k < nreg;k++)
						rwi_senddata(client,"\r\n",-1);
				}
				rwi_senddata(client,".\r\n",-1);				
				ok = 1;
			}
			else if (operacion == 1)
			{				
				nreg = tam_btrv(pfi);
				sprintf(tmp,"%d,%ld\r\n",InfoCampos->n_campos,nreg);
				rwi_senddata(client,tmp,-1);
				for (i = 0;i < InfoCampos->n_campos;i++)
				{
					key = pfi->f_campos[dInfoCampo(InfoCampos,i,0)->Id].c_key;
					sprintf(tmp,"%03d,%02d,%d,%-15.15s\r\n",dInfoCampo(InfoCampos,i,0)->c_maxqc,key,dInfoCampo(InfoCampos,i,0)->c_tipo,(char *)dInfoCampo(InfoCampos,i,1));
					rwi_senddata(client,tmp,-1);
				}
				rwi_senddata(client,".\r\n",-1);					
				ok = 1;
			}
			else if (operacion == 2)
			{
				if (_data && !data)
				{
					data = (struct data_itcp_lista_def *)malloc(sizeof(struct data_itcp_lista_def));
					memset(data,0,sizeof(struct data_itcp_lista_def));
					data->_InfoCampos = (void *)InfoCampos;
					data->_pfi = (void *)pfi;
					data->cargado = cargado;
					data->opcion = opcion;
					data->operacion = operacion;					
					data->n = 0;
				}
				switch(opcion)
				{
				case 0:
					data->opcion = 1;
					rwi_senddata(client,":OK:\r\n",-1);
					ok = 2;					
					break;
				case 1:
					data->n = atoi(nomdef);
					data->opcion = 2;
					data->hechos = 0;
					ok = 2;
					break;
				case 2:
					if (!strcmp(nomdef,"."))
					{
						sprintf(tmp,":OK: %d grabados.\r\n",data->hechos);
						rwi_senddata(client,tmp,-1);
						data->opcion = 0;
						ok = 1;
					}
					else
					{
						j = 0;
						n = strlen(nomdef);
						defecto_ficha(pfi);
						for (i = 0;i < InfoCampos->n_campos;i++)
						{							
							if (j >= n)
								break;
							memcpy(tmp,nomdef+j,dInfoCampo(InfoCampos,i ,0)->c_maxqc);
							tmp[dInfoCampo(InfoCampos,i ,0)->c_maxqc] = 0;
							st_campo(dInfoCampo(InfoCampos,i ,0)->Id,pfi,tmp);
							j += dInfoCampo(InfoCampos,i ,0)->c_maxqc;
						}
						if (apunta_ind(pfi,1,ISEQUAL,0) < 0)
						{
							if (graba_nuevo(pfi) >= 0)
							{
								data->hechos++;
							}
						}
						else
						{
							if (graba_actual(pfi) >= 0)
							{
								data->hechos++;
							}
						}							
						ok = 2;
					}
					break;
				default:
					ok = 1;
					break;
				}				
			}
			else if (operacion == 3)
			{
				if (pcoma)
				{
					j = 0;
					n = strlen(pcoma);
					defecto_ficha(pfi);
					apunta_ind(pfi,1,ISCURR,0);
					for (i = 0;i < InfoCampos->n_campos;i++)
					{
						key = pfi->f_campos[dInfoCampo(InfoCampos,i,0)->Id].c_key;
						if (key == 1)
						{
							if (j >= n)
								break;
							memcpy(tmp,pcoma+j,dInfoCampo(InfoCampos,i ,0)->c_maxqc);
							tmp[dInfoCampo(InfoCampos,i ,0)->c_maxqc] = 0;
							st_campo(dInfoCampo(InfoCampos,i ,0)->Id,pfi,tmp);
							j += dInfoCampo(InfoCampos,i ,0)->c_maxqc;
						}
					}					
					if (borra_reg(pfi) >= 0)
					{
						rwi_senddata(client,":OK: Registro borrado.\r\n",-1);
					}
					else
					{
						rwi_senddata(client,":ERROR: Registro inexistente.\r\n",-1);
					}
					ok = 1;
				}
			}
			else if (operacion == 4)
			{
				cierra_ind(pfi);
				borra_ind(pfi);
				rwi_senddata(client,":OK:\r\n",-1);
				ok = 1;
			}
			else if (operacion == 5)
			{				
				if (pcoma)
				{
					char lectura;				
					char *p1;
					int mikey;
					p1 = pcoma;
					while(*p1 && *p1 != ',') p1++;
					if (*p1 == ',')
					{
						*p1 = 0;
						mikey = atoi(pcoma);
						p1++;
						pcoma = p1;
						while(*p1 && *p1 != ',') p1++;
						if (*p1 == ',')
						{
							*p1 = 0;
							quita_blancos(pcoma);
							lectura = *pcoma;
							p1++;
							pcoma = p1;
							apunta_ind(pfi,mikey,ISCURR,0);
							if (lectura != 'X')
							{
								j = 0;
								n = strlen(pcoma);
								defecto_ficha(pfi);
								for (i = 0;i < InfoCampos->n_campos;i++)
								{
									key = pfi->f_campos[dInfoCampo(InfoCampos,i,0)->Id].c_key;
									if (key == mikey)
									{
										if (j >= n)
											break;
										memcpy(tmp,pcoma+j,dInfoCampo(InfoCampos,i ,0)->c_maxqc);
										tmp[dInfoCampo(InfoCampos,i ,0)->c_maxqc] = 0;
										st_campo(dInfoCampo(InfoCampos,i ,0)->Id,pfi,tmp);
										j += dInfoCampo(InfoCampos,i ,0)->c_maxqc;
									}
								}
							}
							else
							{
								IxxNumeroPosicion = atol(pcoma);
							}
							switch(lectura)
							{
							case '=':
								j = lee_reg(pfi,ISEQUAL);
								break;
							case '>':
								j = lee_reg(pfi,ISGREAT);
								break;
							case ']':
								j = lee_reg(pfi,ISGTEQ);
								break;
							case 'p':
								j = lee_reg(pfi,ISFIRST);
								break;
							case 'u':
								j = lee_reg(pfi,ISLAST);
								break;
							case 'X':
								j = lee_por_posicion(pfi,mikey,IxxNumeroPosicion,pfi->f_buf,0,0);
								break;
							default:
								j = -1;
								break;
							}
							if (j >= 0)
							{
								sprintf(tmp,":OK: Row=%ld\r\n",IxxNumeroPosicion);
								rwi_senddata(client,tmp,-1);
								for (i = 0;i < InfoCampos->n_campos;i++)
								{
									formato_campo(pfi,dInfoCampo(InfoCampos,i,0)->Id,tmp);
									tmp[dInfoCampo(InfoCampos,i ,0)->c_maxqc] = 0;
									rwi_senddata(client,tmp,-1);
									/*
									j = dInfoCampo(InfoCampos,i ,0)->c_maxqc;
									buffer += j;
									tammem += j;
									*/
								}
								rwi_senddata(client,"\r\n",-1);
							}
							else
							{
								rwi_senddata(client,":ERROR: No hay registro\r\n",-1);								
							}
							ok = 1;
						}
					}
				}
			}
			else if (operacion == 6)
			{
				if (_data && !data)
				{
					data = (struct data_itcp_lista_def *)malloc(sizeof(struct data_itcp_lista_def));
					memset(data,0,sizeof(struct data_itcp_lista_def));
					data->_InfoCampos = (void *)InfoCampos;
					data->_pfi = (void *)pfi;
					data->cargado = cargado;
					data->opcion = opcion;
					data->operacion = operacion;					
					data->n = 0;
				}
				switch(opcion)
				{
				case 0:
					if (pcoma)
					{
						key = atoi(pcoma);
						cambia_ind(pfi,key);
						data->n = key;
						data->opcion = 1;
						rwi_senddata(client,":OK:\r\n",-1);
						ok = 6;
					}
					break;
				case 1:
					j = 0;
					pcoma = nomdef;
					n = strlen(pcoma);
					defecto_ficha(pfi);
					for (i = 0;i < InfoCampos->n_campos;i++)
					{
						key = pfi->f_campos[dInfoCampo(InfoCampos,i,0)->Id].c_key;
						if (key == data->n)
						{
							if (j >= n)
								break;
							memcpy(tmp,pcoma+j,dInfoCampo(InfoCampos,i ,0)->c_maxqc);
							tmp[dInfoCampo(InfoCampos,i ,0)->c_maxqc] = 0;
							st_campo(dInfoCampo(InfoCampos,i ,0)->Id,pfi,tmp);
							j += dInfoCampo(InfoCampos,i ,0)->c_maxqc;
						}
					}
					pfi2 = mpfi[0];
					mpfi[0] = pfi;					
					k = fi_clave_lineas[0];
					fi_clave_lineas[0] = 0;
					clave_lin_trata(0,(char *)&(data->lim1),4,"");
					fi_clave_lineas[0] = k;
					mpfi[0] = pfi2;
					data->opcion = 2;
					ok = 6;
					break;
				case 2:
					j = 0;
					pcoma = nomdef;
					n = strlen(pcoma);
					defecto_ficha(pfi);
					for (i = 0;i < InfoCampos->n_campos;i++)
					{
						key = pfi->f_campos[dInfoCampo(InfoCampos,i,0)->Id].c_key;
						if (key == data->n)
						{
							if (j >= n)
								break;
							memcpy(tmp,pcoma+j,dInfoCampo(InfoCampos,i ,0)->c_maxqc);
							tmp[dInfoCampo(InfoCampos,i ,0)->c_maxqc] = 0;
							st_campo(dInfoCampo(InfoCampos,i ,0)->Id,pfi,tmp);
							j += dInfoCampo(InfoCampos,i ,0)->c_maxqc;
						}
					}
					pfi2 = mpfi[0];
					mpfi[0] = pfi;					
					k = fi_clave_lineas[0];
					fi_clave_lineas[0] = 0;
					clave_lin_trata(0,(char *)&(data->lim2),4,"");
					
					n = primeralinea(0,data->lim2,0,data->lim1,data->lim1);   
				    while (n == 0) 
					{
						for (i = 0;i < InfoCampos->n_campos;i++)
						{
							formato_campo(pfi,dInfoCampo(InfoCampos,i,0)->Id,tmp);
							tmp[dInfoCampo(InfoCampos,i ,0)->c_maxqc] = 0;
							rwi_senddata(client,tmp,-1);
							/*
							j = dInfoCampo(InfoCampos,i ,0)->c_maxqc;
							buffer += j;
							tammem += j;
							*/
						}
						rwi_senddata(client,"\r\n",-1);

					    n = siguientelinea(0,data->lim2,0,0,data->lim1);
					}

					rwi_senddata(client,".\r\n",-1);

					if (data->lim2)
					{
						free(data->lim2);
						data->lim2 = NULL;
					}
					if (data->lim1)
					{
						free(data->lim1);
						data->lim1 = NULL;
					}
					fi_clave_lineas[0] = k;
					mpfi[0] = pfi2;
					data->opcion = 0;
					ok = 1;
					break;
				}
			}
			else
			{				
				rwi_senddata(client,":ERROR: Operacion en construccion.\r\n",-1);
				ok = 1;
			}

			if (cargado != 1 && !no_liberar)
			{
				cierra_ind(pfi);
			}
		}
	}

	if (!no_liberar)
	{
		if (pfi && cargado == 2)
			free_defpri(pfi);		
		pfi =  NULL;
		if (InfoCampos)
			free((char *)InfoCampos);
		InfoCampos = NULL;
	}

	if (_data && data)
	{
		if (!no_liberar || ok <= 1)
		{
			free((char *)data);
			data = NULL;
		}
		*_data = data;
	}
	if (!ok)
	{
		if (!rwi_senddata(client,":ERROR: En operacion.\r\n",-1))
			return -1;
		return 1;
	}	

	return ok;
}

int itcp_varios(SOCKET client,int operacion,char *linea)
{
	char tmp[512];
	int ok = 0;
	int j;

	if (operacion == 0)
	{
	  for (j = 0;j < np_ex;j++)
	  {
		 if (!strcmp(linea,p_ex[j].nombre)) 
		 {
			p_ejecuta(p_ex[j].indice,O_NOR,0,30);
			sprintf(tmp,":OK: %s\r\n",m_ex[4].mv.valc);
			rwi_senddata(client,tmp,-1);
			ok = 1;
			break;
		 }
	  }
	}
	else if (operacion == 1)
	{
		j = busca_mexterna(linea);
		if (j >= 0)
		{
			if (m_ex[j].m_tipo == DOUBLETIPO)
			{
				sprintf(tmp,":OK: %lf\r\n",m_ex[j].mv.vald);
			}
			else
			{
				sprintf(tmp,":OK: %s\r\n",m_ex[j].mv.valc);
			}
			rwi_senddata(client,tmp,-1);
			ok = 1;			
		}
	}
	else if (operacion == 2)
	{
		char *pcoma = NULL;

		for (j = 0;linea[j];j++)
		{
			if (linea[j] == ',')
			{
				linea[j] = 0;
				pcoma = linea+j+1;
				break;
			}
		}

		if (pcoma)
		{
			j = busca_mexterna(linea);
			if (j >= 0)
			{
				if (m_ex[j].m_tipo == DOUBLETIPO)
				{
					m_ex[j].mv.vald = rwatof(pcoma);
				}
				else
				{
					if (j >= memoplin)
					{
						free(m_ex[j].mv.valc);
						m_ex[j].mv.valc = malloc(strlen(pcoma)+10);
						strcpy(m_ex[j].mv.valc,pcoma);
					}
					else
					{
						strcpy(m_ex[j].mv.valc,pcoma);
					}
				}
				sprintf(tmp,":OK: %s\r\n",m_ex[4].mv.valc);
				rwi_senddata(client,tmp,-1);
				ok = 1;
			}
		}
	}
	else if (operacion == 3 || operacion == 3000)
	{		
		rwi_senddata(client,":OK: Ejecutando sql.\r\n",-1);		
		prime_dsitcp_salida_dssql = 1;
		if (operacion == 3000)
			dsdb_sql(linea,tmp,NULL,dsitcp_salida_dssql,(void *)client);
		else 
			dsdb_sql(linea,tmp,NULL,dsitcp_salida_sql,(void *)client);
		rwi_senddata(client,".\r\n",-1);
		strcat(tmp,"\r\n");
		rwi_senddata(client,tmp,-1);
		ok = 1;
	}

	if (!ok)
	{
		if (!rwi_senddata(client,":ERROR: En operacion.\r\n",-1))
			return -1;		
	}	
	return 1;
}


/****/

int InsertaCampoEnPantalla(int oipfi,int pan,int n,int desde,int wid)
{	
	  int i,ntab,x;	  

	  if (pan >= 0 && pan < v_mpfi(oipfi)->f_npanta)
	  {
		  if (v_mpfi(oipfi)->f_pantalla[pan].p_campoini > v_mpfi(oipfi)->f_pantalla[pan].p_campofin)
		  {
			  for (i = pan-1;i >= 0;i--)
			  {
				  if (v_mpfi(oipfi)->f_pantalla[i].p_campoini <= v_mpfi(oipfi)->f_pantalla[i].p_campofin)
				  {
					  break;
				  }				  
			  }
			  if (i >= 0)
			  {
				  v_mpfi(oipfi)->f_pantalla[pan].p_campoini = v_mpfi(oipfi)->f_pantalla[i].p_campofin+1;
				  v_mpfi(oipfi)->f_pantalla[pan].p_campofin = v_mpfi(oipfi)->f_pantalla[pan].p_campoini;
			  }
			  else
			  {
				  v_mpfi(oipfi)->f_pantalla[pan].p_campoini = 1;
				  v_mpfi(oipfi)->f_pantalla[pan].p_campofin = 1;
			  }
		  }
		  else
		  {
			  v_mpfi(oipfi)->f_pantalla[pan].p_campofin++;
		  }

		  {
			  ntab = v_mpfi(oipfi)->f_pantalla[pan].p_campofin - 1;
			  for (i = v_mpfi(oipfi)->f_ncampos-1;i > ntab;i--)
			  {
				  v_mpfi(oipfi)->f_taborder[i] = v_mpfi(oipfi)->f_taborder[i-1];
				  x = v_mpfi(oipfi)->f_taborder[i];
				  if (x >= 0 && x < v_mpfi(oipfi)->f_ncampos)
					  v_mpfi(oipfi)->f_camporden[x] = i;
			  }
			  v_mpfi(oipfi)->f_taborder[ntab] = n;
			  for (i = pan + 1;i < v_mpfi(oipfi)->f_npanta;i++)
			  {
				  v_mpfi(oipfi)->f_pantalla[i].p_campoini++;
				  v_mpfi(oipfi)->f_pantalla[i].p_campofin++;
				  /*
				  if (v_mpfi(oipfi)->f_pantalla[i].p_campoini < v_mpfi(oipfi)->f_pantalla[i].p_campofin)
				  {				  
					  v_mpfi(oipfi)->f_pantalla[i].p_campofin++;
				  }
				  else
				  {
					  v_mpfi(oipfi)->f_pantalla[i].p_campofin = v_mpfi(oipfi)->f_pantalla[i].p_campoini;
				  }
				  */
			  }
			  v_mpfi(oipfi)->f_campos[n].c_mante.c_actual = pan * 1000 + ntab;
			  v_mpfi(oipfi)->f_campos[n].c_mante.c_visual = 0;	  
			  v_mpfi(oipfi)->f_campowids[n] = wid;
			  v_mpfi(oipfi)->f_camporden[n] = ntab;			  
			  return 0;
		  }
	  }


	  for (ntab = 0;ntab < v_mpfi(oipfi)->f_ncampos;ntab++)
	  {
		 if (v_mpfi(oipfi)->f_taborder[ntab] == n)
		 {
			 return -1;
		 }
		 if (v_mpfi(oipfi)->f_taborder[ntab] == -1)
			 break;
	  }

	  if (desde > ntab)
	  {
		  pan = v_mpfi(oipfi)->f_npanta - 1;
		  desde = ntab;
		  if (v_mpfi(oipfi)->f_pantalla[pan].p_campoini > v_mpfi(oipfi)->f_pantalla[pan].p_campofin)
		  {
			  v_mpfi(oipfi)->f_pantalla[pan].p_campoini = desde + 1;
			  v_mpfi(oipfi)->f_pantalla[pan].p_campofin = desde + 1;
		  }
		  else
		  {
			  desde = v_mpfi(oipfi)->f_pantalla[pan].p_campofin;
			  v_mpfi(oipfi)->f_pantalla[pan].p_campofin++;
		  }
	  }
	  else
	  {
		  return -1; /* no soportado de momento ... siempre añadir */
	  }


	  for (i = ntab;i > desde;i--)
	  {
		  v_mpfi(oipfi)->f_taborder[i]  = v_mpfi(oipfi)->f_taborder[i-1];
	  }
	  ntab++;
	  v_mpfi(oipfi)->f_campos[n].c_mante.c_actual = pan * 1000;
	  v_mpfi(oipfi)->f_taborder[desde] = n;
	  v_mpfi(oipfi)->f_taborder[ntab] = -1;
	  v_mpfi(oipfi)->f_camporden[n] = desde;
	  v_mpfi(oipfi)->f_campowids[n] = wid;
	  v_mpfi(oipfi)->f_campos[n].c_mante.c_visual = 0;	  

	  return 0;
}


int RetroGridId(int i)
{
	if (RetroTipo[i] == 3 && RetroFuncion[i] && RetroPar[i]) {
		if (((struct gridpars *)RetroPar[i])->structid == 0x3212)
			return ((struct gridpars *)RetroPar[i])->id_grid;
	}
	return -1;
}