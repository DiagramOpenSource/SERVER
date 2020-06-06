
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

extern int usar_grid;

extern FILE *_imp;
extern char *aginfo;
extern int LEERTERMINAL;
extern char nmen0[51];

extern char *rw_usuario;

extern char *lee_linea();

extern int user_who;
extern int super_usuario;
extern char rw_user[11];
extern char *quita_path();


extern int default_id;
extern int default_sid;
extern int default_wid;


extern int recur_punto;

extern int ultima_instalacion;

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
   char versionletra;  
   char tipo;
   char sistema[16];
   char fecha[21];
   char descrip[31];
   char rdependeletra;
   char pdependeletra;
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

extern void (*RetroFuncion[512])(char *bloque,char *par,int el_punto);
extern char *RetroPar[512];
extern int RetroPunto;


/* ********* Clave de HELP ********** */

extern char clave_help[];

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


struct gridpars
{
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
};


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

  buf = malloc(pfi->f_lfich+1);
  memcpy(buf,pfi->f_buf,pfi->f_lfich);

    if (limi1)
    {
        defecto_ficha(pfi);
        if (par->b_lim1) 
           free(par->b_lim1);
        par->b_lim1 = malloc(pfi->f_lfich+1);
        p_ejecuta(limi1,O_NOR,0,31);      
        memcpy(par->b_lim1,pfi->f_buf,pfi->f_lfich);
    }

    if (limi2)
    {
        defecto_ficha(pfi);
        if (par->b_lim2) 
           free(par->b_lim2);
        par->b_lim2 = malloc(pfi->f_lfich+1);
        p_ejecuta(limi2,O_NOR,0,32);
        memcpy(par->b_lim2,pfi->f_buf,pfi->f_lfich);
    }

   memcpy(pfi->f_buf,buf,pfi->f_lfich);
   free(buf);

return 0;
}

static int LanZaHiJo(struct gridpars *par,char *titulo,int nfiltros,struct grid_filtro *filtros,int ncolucam,int *colucam)
{
    struct gridpars *mihijo;
    FI_DEF *pfi;
    int *hijos;    
    int i;
    int np;

    
    if (par->procesando)
    {        
        aviso(0,"No se puede activar otra busqueda mientras no haya acabado la anterior");
        return -1;
    }
    

    pfi = NULL;
    if (lee_defn(par->pfi->f_PathDef,&pfi) < 0) return -1;

    if (filtros)
    {
        strcpy(pfi->f_fichero,"__XXXXXX");
        cambia_path_fich(pfi->f_fichero);
        mktemp(pfi->f_fichero);
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
            unlink(pfi->f_fichero);
            free_defpri(pfi);        
            return -1;
        }
        cierra_ind(pfi);
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
            unlink(pfi->f_fichero);
        }
        free_defpri(pfi);
        return -1;
    }

    apunta_ind(pfi,par->encl+1,ISFIRST,0);

    mihijo = (struct gridpars *)malloc(sizeof(struct gridpars));
    memset((void *)mihijo,0,sizeof(struct gridpars));

    mihijo->titulo = malloc(strlen(titulo)+1);
    strcpy(mihijo->titulo,titulo); 
    
    mihijo->pfi    = pfi;
    mihijo->ipfi   = -1;
    mihijo->encl   = par->encl;
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
    mihijo->hook   = 0;
    mihijo->pact   = 0;
    mihijo->gmodo  = 0x10000;
    mihijo->_id    = par->_id;
    mihijo->_sid   = par->_sid;
    mihijo->_wid   = par->_wid;
    mihijo->Independiente = 0; /* los hijo no son independientes del todo, deben ser destruidos con la 
                                 funcion borrahijogrid */

    if ( (par->gmodo & 0x1) || (par->gmodo & 0x20000) || (par->gmodo & 0x8))
    {
        mihijo->gmodo |= 0x20000;
    }
    mihijo->pi = 0;
    mihijo->pf = 0;
    mihijo->padre = par;
    if (filtros)
    {
        mihijo->filtros = (struct grid_filtro *)malloc(sizeof(struct grid_filtro) * nfiltros);
        memcpy(mihijo->filtros,filtros,sizeof(struct grid_filtro) * nfiltros);
        mihijo->gmodo |= 0x40000;
    }
    else
        mihijo->filtros = NULL;
    mihijo->nfiltros = nfiltros;

    if (!par->hijos)
    {
        par->hijos = (int*)malloc(sizeof(int)*100);
        par->hijos[0] = -2;
    }
    
    hijos = par->hijos;

    while(*hijos > -2) hijos++;

    np = LanzaGrid(mihijo,ncolucam,colucam);

    *hijos = np;
    hijos++;
    *hijos = -2;
    
    return np;
}


static int ComparaClave(pfi,key,buf1)
FI_DEF *pfi;
int key;
char *buf1;
{
   int i,campo,orden,compara;   
   
   for (i = 0;i < pfi->f_clave[key].k_ncampos;i++) 
   {
      campo = pfi->f_clave[key].k_campo[i] - 1;
      orden = pfi->f_clave[key].k_orden[i];
      
      compara = x_campocmp(campo,orden,pfi,pfi->f_buf,buf1,0);
      if (compara) 
          return compara;
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


  registros = tam_btrv(pfi);
  if (!registros)
  {
      return 0L;
  }


  buf = malloc(pfi->f_lfich+1);
  memcpy(buf,pfi->f_buf,pfi->f_lfich);

  rega = da_registo_actual(pfi);

  if (limi2)
  {      
      if (!par->b_lim2)
      {
          InicializaLimite(par);
      }
      memcpy(pfi->f_buf,par->b_lim2,pfi->f_lfich);
      if (lee_reg(pfi,ISGREAT) < 0)
          lee_reg(pfi,ISLAST);
      else
          lee_reg(pfi,ISPREV);

      posfin = IxxNumeroPosicion;
      if (ComparaClave(pfi,key,par->b_lim2) > 0)
      {
          posfin = -1;
      }          
  }
  else
      posfin = registros - 1;
   
   
   if (limi1)
   {      
      if (!par->b_lim1)
      {
          InicializaLimite(par);
      }
      memcpy(pfi->f_buf,par->b_lim1,pfi->f_lfich);
      if (lee_reg(pfi,ISGTEQ) < 0)
          lee_reg(pfi,ISFIRST);
      posini = IxxNumeroPosicion;
      if (ComparaClave(pfi,key,par->b_lim1) < 0)
      {
          posini = -1;
      }          
   }      
   else
      posini = 0L;

   registros = posfin - posini + 1;
   if (registros < 0 || posini == -1L || posfin == -1L)
   {
       registros = 0L;
   }

   par->registros = registros;  
   par->offset = posini;  


   pon_registro_actual(par->pfi,rega);
   apunta_ind(par->pfi,0,ISEQUAL,0);
   apunta_ind(par->pfi,par->encl+1,ISCURR,0);
   memcpy(pfi->f_buf,buf,pfi->f_lfich);
   free(buf);

   IxxNumeroPosicion = posa;
   return(registros);
}

static int evaluador_simple_de_filtro(FI_DEF *pfi,int nfiltros,struct grid_filtro *filtros,FI_DEF *pfitest)
{
   int resultado = 0;
   int parcial;
   int i;   
   int campo;
   char buffer[512];

   for (i = 0;i < nfiltros;i++)
   {
       if (resultado) break;
       parcial = 0;
       campo = filtros[i].campo;
       switch(filtros[i].tipo)
       {
       case 1:
           st_campo(campo,pfitest,filtros[i].valor);
           if (x_campocmp(campo,1,pfi,pfi->f_buf,pfitest->f_buf,0))
              parcial = 1;
           break;
       case 2:
           ld_campo(campo,pfi,buffer);
           if ( strlen(filtros[i].valor) > strlen(buffer) )
               parcial = 1;
           else
           {
               if (memcmp(filtros[i].valor,buffer,strlen(filtros[i].valor)))
                   parcial = 1;
           }
           break;
        case 3:
           ld_campo(campo,pfi,buffer);
           if (strlen(filtros[i].valor) > strlen(buffer))
               parcial = 1;
           else
           {
               if (!strstr(buffer,filtros[i].valor))
                   parcial = 1;
           }           
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

    if (!par || RetroPar[el_punto] != (char *)par) return;

    bloque += (sizeof(long)*3);

	memcpy((char *)&operacion,bloque,sizeof(int));
	bloque += sizeof(int);


    if (!operacion)
    {
        memcpy((char *)&registro,bloque,sizeof(long));
	    bloque += sizeof(long);
	    memcpy((char *)&desde_campo,bloque,sizeof(int));
	    bloque += sizeof(int);
	    memcpy((char *)&hasta_campo,bloque,sizeof(int));
	    bloque += sizeof(int);

	    memcpy((char *)&from,bloque,sizeof(long));
	    bloque += sizeof(long);
	    memcpy((char *)&to,bloque,sizeof(long));
	    bloque += sizeof(long);
	    memcpy((char *)&cursel,bloque,sizeof(long));
	    bloque += sizeof(long);


        from -= 2;
        to += 2;
        topex = to - from + 1; 
        if (from < 0) from = 0;
        if (to < from)
        {
            totc = from;
            from = to;
            to = totc;
        }

        /*
        if (registro <= from || registro > (to+1))
        {
            DebugBreak();
        }
        */        

        totc = to - from + 1;

        if ((unsigned long)registro == (unsigned long)0xFFFFFFFF)
            registro = par->registros;

        if (registro > par->registros)
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
                if (!hit)
                {
                    if (par->cache)
                    {
                        for (i = 0;par->cache[i].id_reg;i++);                        
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

                    /****/
                    i = ixx_inicio_lectura_multiple(par->pfi->f_fd);
                    if (i == 1 || par->atencion)
                    {
                        par->atencion = 0;
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
                        pon_registro_actual(par->pfi,par->regsel );
                        apunta_ind(par->pfi,0,ISEQUAL,0);
                        apunta_ind(par->pfi,par->encl+1,ISCURR,0);
                        lee_reg(par->pfi,ISCURR);
                        par->regsel = isrecnum;
                        
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
                        

                        if (to >= par->registros)
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
                            registro = to + 1;
                        }
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


                    for (i = 0;par->cache[i].id_reg;i++)
                    {
                        if (par->cache[i].id_reg <= from || par->cache[i].id_reg > (to+1))
                            par->cache[i].id_reg = -1L;
                    }

                    for (i = 0;i < totc && (from+i) <= to;i++)
                    {
                        for (j = 0;par->cache[j].id_reg;j++)
                        {
                            if (par->cache[j].id_reg == (from+i+1))
                            {
                                isrecnum = par->cache[j].isrec;
                                break;
                            }
                        }
                        if (!par->cache[j].id_reg)
                        {
                            for (j = 0;par->cache[j].id_reg;j++)
                            {
                                if (par->cache[j].id_reg == -1L)
                                {
                                    break;
                                }
                            }
                            if (par->cache[j].id_reg)
                            {
                                if (ixx_lee_por_posicion(par->pfi->f_fd,par->offset+from+i,par->pfi->f_buf,0,0) >= 0)
                                {
                                    par->cache[j].id_reg = from + i + 1;
                                    par->cache[j].isrec  = isrecnum;
                                    par->reg = from + i + 1;
                                    memcpy(par->cache[j].buf,par->pfi->f_buf,par->pfi->f_lfich);
                                    if (tope)
                                    {
                                        registro = from + i + 1;
                                    }
                                }
                            }
                        }
                    }

                    ixx_fin_lectura_multiple(par->pfi->f_fd);

                    if (tope) break;
                    /****/
                    continue;
                }
                break;
            } while(-1);
        }       

        *bloque = tope;
        offset = 1;

        if (cursel == registro)
        {
           par->regsel = isrecnum;
           par->regcur = registro;
        }        

        for (_campo = desde_campo;_campo <= hasta_campo;_campo++)
        {  
           bloque[offset] = 0;
           campo = par->colcampos[_campo];
           if (campo < 0) break;
		   if (par->registros >= registro)
		   {
				formato_campo(par->pfi,dInfoCampo(par->InfoCampos,campo,0)->Id,bloque+offset);
		   }
		   else
		   {
			    memset(bloque+offset,' ',dInfoCampo(par->InfoCampos,campo ,0)->c_maxqc);
		   }

           offset += dInfoCampo(par->InfoCampos,campo ,0)->c_maxqc;
        }

        memcpy(bloque+offset,&isrecnum,sizeof(long));
        offset += sizeof(long);
        memcpy(bloque+offset,&(par->registros),sizeof(long));
        offset += sizeof(long);
        memcpy(bloque+offset,&cursel,sizeof(long));       

    }
    else
    if (operacion == 1)
    {
        char tmpxx[512];

        memcpy((char *)&registro,bloque,sizeof(long));
	    bloque += sizeof(long);
        par->regsel = registro;
        memcpy((char *)&(par->regcur),bloque,sizeof(long));
        bloque += sizeof(long);
        memcpy((char *)&(par->curcol),bloque,sizeof(int));
        bloque += sizeof(int);
        memcpy((char *)&(par->regsel),bloque,sizeof(long));
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
                        break;
                    }
                }
            }
        }

        if (par->hook)
        {            
            pon_registro_actual(par->pfi,par->regsel );
            apunta_ind(par->pfi,0,ISEQUAL,0);
            apunta_ind(par->pfi,par->encl+1,ISCURR,0);
            lee_reg(par->pfi,ISCURR);
            for (_campo = 0;_campo < par->ncampos;_campo++)
            {
                campo = par->colcampos[_campo];
                if (campo < 0) break;
                i = dInfoCampo(par->InfoCampos,campo ,0)->c_maxqc;
                memcpy(tmpxx,bloque,i);
                tmpxx[i] = 0;
                st_campo(dInfoCampo(par->InfoCampos,campo ,0)->Id,par->pfi,tmpxx);
                bloque += i;
            }

            sprintf(v_memo(5004)->alfabetica,"%d",1);
            p_ejecuta(par->hook,O_NOR,0,33);
        }        

    }
    else
    if (operacion == 2)
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
                filtro.campo = dInfoCampo(par->InfoCampos,campo,0)->Id;
                strcpy(filtro.valor,conte);

                LanZaHiJo(par,titulo,1,&filtro,NULL,NULL);
                enviar = 0;
             }
        }

        if (enviar)
        {
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
            par->reg = (IxxNumeroPosicion - par->offset) + 1;            

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
        }
        else
        {
            registro = 0L;
            bloque -= sizeof(int);
            memcpy(bloque,&registro,sizeof(long));
        }        
    }
    else
    if (operacion == 3)
    {
        int key;


        memcpy(&key,bloque,sizeof(int));                


        if (key < par->pfi->f_nclaves && key >= 0)
        {
            ixx_inicio_lectura_multiple(par->pfi->f_fd);
            par->InfoCampos->clave_actual = key;

            par->encl = key;        
            pon_registro_actual(par->pfi,par->regsel );
            apunta_ind(par->pfi,0,ISEQUAL,0);
            apunta_ind(par->pfi,key+1,ISCURR,0);        
            par->reg = IxxNumeroPosicion+1;

            par->registros = TotalRegistros(par);
            par->reg -= par->offset;

            ixx_fin_lectura_multiple(par->pfi->f_fd);
        }
        else
        {
            key = -1;
        }

        memcpy(bloque,&key,sizeof(int));
        bloque += sizeof(int);
        memcpy(bloque,&par->reg,sizeof(long));
        bloque += sizeof(long);
        memcpy(bloque,&par->registros,sizeof(long));
        bloque += sizeof(long);
        par->regcur = par->reg;

        if (par->cache)
        {
            for (i = 0;par->cache[i].id_reg;i++)
            {
                par->cache[i].id_reg = -1L;
            }
        }
    }
    else
    if (operacion == 4)
    {       
        memcpy((char *)&registro,bloque,sizeof(long));
	    bloque += sizeof(long);

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
        par->reg = (IxxNumeroPosicion - par->offset)+1;
        par->regsel = isrecnum;
        par->regcur = par->reg;
        ixx_fin_lectura_multiple(par->pfi->f_fd);

        memcpy(bloque,&par->regcur,sizeof(long));
        bloque += sizeof(long);
    }
    else
    if (operacion == 5) /* rellenar hijo */
    {
        long contad/* = time(NULL)*/;
        long tactual;
        long registro;
        int er,hay = 0;
        int fd,ofd;
        int porcontenido = 0;

        if (!par->padre || !par->filtros || !(par->gmodo & 0x40000))
        {
            goto sal_rellena_hijo;
        }

        er = -1;
        if (par->nfiltros == 1 && par->filtros[0].tipo == 3 && !par->filtros[0].inver &&
                                                               par->pfi->f_campos[par->filtros[0].campo].c_tipo == CHARTIPO &&
                                                               par->pfi->f_campos[par->filtros[0].campo].c_qc > strlen(par->filtros[0].valor))
        {
            for (i = 0;i < par->pfi->f_nclaves;i++)
            {
                if (par->pfi->f_clave[i].k_ncampos == 1 && par->pfi->f_clave[i].k_campo[0] == (par->filtros[0].campo+1))
                {
                    er = i;
                    break;
                }
            }
            if (er == -1)
            {            
                for (i = 0;i < par->pfi->f_nclaves;i++)
                {
                    if (par->pfi->f_clave[i].k_campo[0] == (par->filtros[0].campo+1))
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

        ofd = par->padre->pfi->f_fd;
        par->padre->pfi->f_fd = -1;
        if (abre_ind(par->padre->pfi,ISINOUT + ISMANULOCK) < 0)
        {
            par->padre->pfi->f_fd = ofd;
            goto sal_rellena_hijo;
        }
        apunta_ind(par->padre->pfi,er+1,ISFIRST,0);
        fd = par->padre->pfi->f_fd;
        par->padre->pfi->f_fd = ofd;


        if (porcontenido)
        {
            ixx_pon_contenido(fd,par->filtros[0].valor,strlen(par->filtros[0].valor));
        }
        /*do
        {*/
            er = ixx_lee_indice(fd,par->padre->pfi->f_buf,ISFIRST,-1,1,0,1);
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

        if (!porcontenido && !er)
        {
            er = evaluador_simple_de_filtro(par->padre->pfi,par->nfiltros,par->filtros,par->pfi);
        }

        if (!er)
        {
            memcpy(par->pfi->f_buf,par->padre->pfi->f_buf,par->pfi->f_lfich);
            graba_nuevo(par->pfi);
            par->registros = TotalRegistros(par);
            hay = 1;
        }
        contad = 0;
        par->procesando = 1;
        while(er >= 0)
        {
            if (RetroPar[el_punto] != (char *)par) break;

          /*  do
            {*/
                er = ixx_lee_indice(fd,par->padre->pfi->f_buf,ISNEXT,-1,1,0,1);
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

            if (!porcontenido && !er)
            {
                er = evaluador_simple_de_filtro(par->padre->pfi,par->nfiltros,par->filtros,par->pfi);
            }

            if (!er)
            {
                memcpy(par->pfi->f_buf,par->padre->pfi->f_buf,par->pfi->f_lfich);
                graba_nuevo(par->pfi);
                hay = 1;
            }

            tactual = time(NULL);
            if (contad != tactual)
            {
                if (hay)
                {
                    char tmp[256];
                    long modo = 1;
                    long tam = (sizeof(long)*4)+sizeof(int);
                    int id_grid = par->id_grid;

                    par->registros = TotalRegistros(par);

                    memcpy(tmp,&tam,sizeof(long));
                    memcpy(tmp+sizeof(long),&tam,sizeof(long));
                    memcpy(tmp+(sizeof(long)*2),&modo,sizeof(long));
                    memcpy(tmp+(sizeof(long)*3),&id_grid,sizeof(int));
                    memcpy(tmp+(sizeof(long)*3)+sizeof(int),(void *)&par->registros,sizeof(long));
					if (DespachadorExterno)
						(*DespachadorExterno)((int)DESPACHO_BINARIO,(int)3,tmp);                    
                    hay=0;
                }
                contad = time(NULL);
                if (pulsatecla() > -1)
                {
                    break;
                }
            }
        }
        if (porcontenido)
        {
            ixx_fin_contenido(fd);
        }
        _cierra_ind(fd);
sal_rellena_hijo:
        if (RetroPar[el_punto] != (char *)par) 
        {
            registro = 0;
        }
        else
        {
            par->registros = TotalRegistros(par);
            registro = par->registros;
            par->procesando = 0;
        }
        memcpy(bloque,&registro,sizeof(long));        
    }
    else
    if (operacion == 6)
    {
        memcpy(&registro,bloque,sizeof(long));

        if (par->padre)
        {
            int ofs;
            int lll;

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

            /*** buscar clave primaria en el padre */
            /* de momento uno a uno ... luego hay que poner un 'mapa' */
            for (i = 0;i < par->pfi->f_clave[0].k_ncampos;i++)
            {
                lll = par->pfi->f_clave[0].k_campo[i] - 1;
                ofs = par->pfi->f_campos[lll].c_offset;
                lll = par->pfi->f_campos[lll].c_long;
                memcpy(par->padre->pfi->f_buf+ofs,par->pfi->f_buf+ofs,lll);
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
            apunta_ind(par->padre->pfi,1,ISEQUAL,0);            
            registro = isrecnum;
            apunta_ind(par->padre->pfi,par->padre->encl+1,ISCURR,0);
            ixx_fin_lectura_multiple(par->padre->pfi->f_fd);            
        }
        memcpy(bloque,&registro,sizeof(long));
    }
    else
    if (operacion == 7)
    {
        struct grid_filtro *filtros = NULL;
        char titulo[512];
        int nfiltros = 0;
        int *cols = NULL;


        memcpy(&i,bloque,sizeof(long));
        bloque += sizeof(long);

        
        if (i > -1)
        {
            j = par->encl;
            par->encl = i;
            strcpy(titulo,"Consulta ");
            strcat(titulo,par->pfi->f_titulo);
            LanZaHiJo(par,titulo,0,NULL,NULL,NULL);
            par->encl = j;
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
            LanZaHiJo(par,titulo,nfiltros,filtros,i,cols);
        }        
    }
    else
    if (operacion == 10) /* seleccion */
    {
        EntraLineaGrid(bloque,par,el_punto);
    }
    else
    if (operacion == 11)
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
          inicial = 1;
        }
        else
        {
          inicial = IxxNumeroPosicion + 1;
        }
        par->regsel = isrecnum;

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
        inicial -= par->offset;
        if (inicial < 1 || inicial > registros)
           inicial = 1;
        par->reg = inicial;
        par->regcur = inicial;
        memcpy(bloque,&registros,sizeof(long));
        bloque += sizeof(long);
        memcpy(bloque,&par->regcur,sizeof(long));
        bloque += sizeof(long);
    }
    else
    if (operacion == 12) /* borrar */
    {
        BorraLineaGrid(bloque,par,el_punto);
    }
    else
    if (operacion == 13) /* actualizar columnas visibles */
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
    if (operacion == 9999)
    {
        int elotro = -1;
        if ( (par->gmodo & 0x10000) )
        {
            /* al borra un 'hijo' solo siempre se da el foco al padre o a un hermano */            
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

        }
        else
        {
            if (par->Independiente)
            {
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
            if ( /*(pfi->f_campos[j].c_mante.c_actual%1000)*/pfi->f_taborder[j] == k )
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

	if (pfi->f_campos[c].c_mante.c_visual || pfi->f_campos[c].c_tipo == NULOTIPO)
	{
		campo->c_flags |= 1;
	}
	if (pfi->f_campos[c].c_mante.c_modif  || pfi->f_campos[c].c_tipo == NULOTIPO)
	{
		campo->c_flags |= 2;
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

    pfi->f_campos[c].c_mante.c_lineal |= FGV_C_GRID;

    if (lreg)
    {
       *lreg += campo->c_maxqc;
    }

    return memoria;
}

int RellenaInfoDef(struct Info_Entra_Campos *info,FI_DEF *pfi,int flag,int key,int *len_reg)
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

    campo = NULL;
    clave = NULL;
    n_campos = 0;
    n_claves = 0;
    memoria = 0; 

    for (fase = 0;fase < 2;fase++)
    {
        if (fase == 1) /* relleno despues de la contabilizacion */
        {
            memoria2 = memoria + ((n_campos+n_claves)*sizeof(int));
            memset((void *)info,0,memoria2);
            info->Len = memoria2;
            info->Id = (int)pfi;
            info->n_campos = n_campos;
            info->n_claves = n_claves;
            info->clave_defecto = key;
            info->clave_actual = key;
            info->t_total_campos = pfi->f_ncampos;
            info->d_Campos = memoria;            
            info->d_Claves = memoria + (n_campos*sizeof(int));
            if (!(flag & 1))
            {
                if (pfi->f_campogids) free(pfi->f_campogids);
                pfi->f_campogids = (short *)malloc(pfi->f_ncampos * sizeof(int));
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
                    i = pfi->f_taborder[numcam-1];/*pfi->f_campos[numcam-1].c_mante.c_actual % 1000;*/
                    if (i < 0 || i >= pfi->f_ncampos) 
						continue;
                    /*if (pfi->f_campos[i].c_mante.c_visual) continue;*/

					/*
					if (pfi->f_campos[i].c_tipo == NULOTIPO)
						continue;
				    */

                    if (!hay_dat_permiso(pfi,i)) continue;
                    
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
							campo->p_actual = pfi->f_taborder[conta_campos];
							pfi->f_taborder[conta_campos] = i;							
                            campo->l_actual = pfi->f_campos[i].c_mante.c_actual;							
                            pfi->f_campos[i].c_mante.c_actual = 1000 + (pfi->f_campos[i].c_mante.c_actual%1000);
							/*
                            campo->p_actual = pfi->f_campos[conta_campos].c_mante.c_actual;
                            pfi->f_campos[conta_campos].c_mante.c_actual = ((pfi->f_campos[conta_campos].c_mante.c_actual/1000)*1000)+i;
							*/
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
                for (numcam = 0;numcam < p_ncampos;numcam++)
                {
                    if ( /*(pfi->f_campos[numcam].c_mante.c_actual%1000)*/pfi->f_taborder[numcam] == i )
                    {
                        break;
                    }
                }
                if (numcam < p_ncampos) 
                    continue;
				/*
				if (pfi->f_campos[i].c_tipo == NULOTIPO) 
					continue;
			    */
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
                        pfi->f_campos[i].c_mante.c_actual = 1000 + (pfi->f_campos[i].c_mante.c_actual%1000);
						campo->p_actual = pfi->f_taborder[conta_campos];
						pfi->f_taborder[conta_campos] = i;
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
					/*
					if (pfi->f_campos[i].c_tipo == NULOTIPO)
						continue;
					*/
                    if (!hay_dat_permiso(pfi,i)) continue;
                }
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
                        pfi->f_campos[i].c_mante.c_actual = 1000 + (pfi->f_campos[i].c_mante.c_actual%1000);
						campo->p_actual = pfi->f_taborder[conta_campos];
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
            strcpy(info->PathDef,pfi->f_PathDef);
        }
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
	 mi_punto = RetroPunto;
	 RetroPunto++; /* sin comprobar tope !!!*/
	 }
  else
     mi_punto = i;
  
  return mi_punto;
}


static int LanzaGrid(struct gridpars *par,int ncolucam,int *colucam)
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

if (DespachadorExterno)
{  
  encl  = par->encl;
  hook  = par->hook;
  pfi  = par->pfi;
  gmodo = par->gmodo;
  pi    = par->pi;
  pf    = par->pf;

  par->cache = NULL;

  pfi->f_idgrafico = -1;

  oc = ind_clave_activa(pfi) - 1;
  
  if (oc != encl) 
  {
     apunta_ind(pfi,encl+1,ISCURR,0);
  }  

  if (lee_reg(pfi,ISGTEQ) < 0)
      inicial = 1;
  else
  {
      inicial = IxxNumeroPosicion + 1;
  }

  par->regsel = isrecnum;

  registros = TotalRegistros(par);
  inicial -= par->offset;
  if (inicial < 0 || inicial > registros)
      inicial = 1;

  mi_punto = DameRetroPar();  

  lreg = 0;
  tam = RellenaInfoDef(NULL,pfi,(gmodo & 64),encl,&lreg);
  if (tam < lreg)
      tam = lreg;
  memoria = 260 + sizeof(long) * 16 + tam + sizeof(int)*3;
  tmp = malloc(memoria);


  tam = sizeof(long)*3;
  memcpy(tmp+tam,&mi_punto,sizeof(int));
  tam += sizeof(int);
  memcpy(tmp+tam,&registros,sizeof(long));
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

  if (par->control_ipfi > 0)
  {
	  int oipfi = par->control_ipfi - 1;
	  int ocampo = -(par->control_campo);
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
  tam += RellenaInfoDef(InfoCampos,pfi,(gmodo & 64),encl,NULL);

  memcpy(tmp,&tam,sizeof(long));
  tam = (sizeof(long)*3)+sizeof(int)*2;
  memcpy(tmp+sizeof(long),&tam,sizeof(long));
  memcpy(tmp+(sizeof(long)*2),&modo,sizeof(long));  

  par->InfoCampos = InfoCampos;    
   
  par->colcampos = (int *)malloc(sizeof(int) * InfoCampos->n_campos);

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

  (*DespachadorExterno)((int)DESPACHO_BINARIO,(int)0,tmp);  

  memcpy(&id_grid,tmp+(sizeof(long)*3),sizeof(int));
  memcpy(&par->curcol,tmp+(sizeof(long)*3)+sizeof(int),sizeof(int));

  par->id_grid = id_grid;
  pfi->f_idgrafico = id_grid;
  pfi->f_puntoretropar = mi_punto;

  par->for_free = tmp;  

  if (id_grid < 0)
  {
      mi_punto = BorraGrid(mi_punto);       
  }
  /*agi_cola_timer();*/
}

return(mi_punto);
}

static int BorraHijoGrid(int mi_punto,int alpadre)
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
            if (par->filtros)
            {
                unlink(par->pfi->f_fichero);
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

static int BorraGrid(int mi_punto)
{
char tmp[512];
struct gridpars *par;
int id_grid = -1;
int c,i;
long modo = 1;
long tam;
int *hijos;
FI_DEF *pfi;
struct Info_Entra_Campos *info;
struct Info_Entra_Campo  *campo;

  if (mi_punto < 0) return mi_punto;

  par = (struct gridpars *)RetroPar[mi_punto];

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

  agi_cola_timer();
  id_grid = par->id_grid; 
  
  tam = (sizeof(long)*3)+sizeof(int);
  memcpy(tmp,&tam,sizeof(long));
  memcpy(tmp+sizeof(long),&tam,sizeof(long));
  memcpy(tmp+(sizeof(long)*2),&modo,sizeof(long));
  memcpy(tmp+(sizeof(long)*3),&id_grid,sizeof(int));
  (*DespachadorExterno)((int)DESPACHO_BINARIO,(int)1,tmp); 
  
  
  pfi = par->pfi;

  for (c = 0;c < pfi->f_ncampos;c++)
  {
     pfi->f_campos[c].c_mante.c_lineal &= ~FGV_C_GRID;
  }

  pfi->f_idgrafico = 0;

  if (pfi->f_campogids)
  {
      free((char *)pfi->f_campogids);
      pfi->f_campogids = NULL;
  }

  info = par->InfoCampos;


  for (i = 0;i < info->n_campos;i++)
  {
       campo = dInfoCampo(info,i,0);
       pfi->f_taborder[i] = campo->p_actual;
       pfi->f_campos[campo->Id].c_mante.c_actual = campo->l_actual;
  }

  pfi->f_npanta = info->p_npan;            
  if (info->p_npan > 0)
  {
       pfi->f_pantalla[0].p_campoini = info->p_campini;
       pfi->f_pantalla[0].p_campofin = info->p_campfin;
  }
  else
  {
       if (info->p_campfin == -2)
       {                    
            free((char *)pfi->f_pantalla);
            pfi->f_pantalla = NULL;            
       }
  }

  if (par->for_free)
  {
     free(par->for_free);
     par->for_free = NULL;
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
  if (par->Independiente)
  {
      if (par->ipfi > -1)
        _salva_datos_unlock(par->ipfi);
      free(par->titulo);
      free((char *)par);
  }
  mi_punto = -1;

  agi_cola_timer();
  
  return mi_punto;
}


int consul_clave_grid(ipfi,encl,numpan,limi1,limi2,filtro,pipfi,campo)
int ipfi,encl,numpan,limi1,limi2,filtro,pipfi,campo;
{
int oc = ind_clave_activa(v_mpfi(ipfi)) - 1;
char *buf = malloc(v_mpfi(ipfi)->f_lfich + 1);
struct gridpars par;
char titulo[256];
int punto = -1;
int salida = -1;
int tecla = 807;
long resultado;

  if (numpan < 0)
      numpan = 0;

  if (eqj == 2 && (pipfi < 0 || campo < 0 || numpan < 0))
  {
	  eqj = 0;
	  return -1;
  }	 
  


  memcpy(buf,v_mpfi(ipfi)->f_buf,v_mpfi(ipfi)->f_lfich);
  strcpy(titulo,"Consulta ");
  strcat(titulo,v_mpfi(ipfi)->f_titulo);

  encl--;

  _salva_datos_lock(ipfi);

  memset((void *)&par,0,sizeof(par));

  par.titulo = titulo;   
  par.pfi    = v_mpfi(ipfi);
  par.ipfi   = -1;
  par.encl   = encl;
  par.limi1  = limi1;
  par.limi2  = limi2;
  par.b_lim1 = NULL;
  par.b_lim2 = NULL;
  par.hook   = 0;
  par.gmodo  = 0;   
  par.pi     = 0;
  par.pf     = 0;
  par.padre  = NULL;
  par._id    = default_id;
  par._sid   = 0;/*default_sid; /*ipfi+1+(numpan * 1000);*/
  par.Independiente = 0; /* la consulta no es independiente */
  if (par._sid == default_sid)
     par._wid   = default_wid;
  else
     par._wid   = -1;


  /* salvar flag visual */

  if (eqj == 2)
  {  
	 int nl;
	 par.gmodo = 4+8+16+64;
	 par._sid  = default_sid;/* pipfi+1+(numpan * 1000);*/
	 par.pi     = v_mpfi(pipfi)->f_campos[campo].c_mante.c_qp + 100;     
     par.pf     = v_mpfi(pipfi)->f_pantalla[numpan].p_posref + (v_mpfi(pipfi)->f_pantalla[numpan].p_posfin-101);
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

  punto = LanzaGrid(&par,NULL,NULL);

  if (punto < 0)
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

  encl = par.encl;

  resultado = par.regsel;


  BorraGrid(punto);


  if (tecla == VALIDAR || tecla == CRETURN)
  {      
      salida = 0;
      pon_registro_actual(v_mpfi(ipfi),resultado);
      apunta_ind(v_mpfi(ipfi),0,ISEQUAL,0);
      leer_reg(ipfi,ISCURR,0,0);
      apunta_ind(v_mpfi(ipfi),encl+1,ISCURR,0);
  }


  if (!salida) 
  {
      pinta_datos(ipfi,0);
  }
  else 
  {
       memcpy(v_mpfi(ipfi)->f_buf,buf,v_mpfi(ipfi)->f_lfich);
  }
  if (oc != encl || salida) 
      {
      apunta_ind(v_mpfi(ipfi),oc+1,ISCURR,0);
      }

  _salva_datos_unlock(ipfi);

  free(buf);
  

  eqj = 0;

  return (salida);
}

int consul_clave(ipfi,encl,numpan,limi1,limi2,filtro,pipfi,campo)
int ipfi,encl,numpan,limi1,limi2,filtro,pipfi,campo;
{
    return(consul_clave_grid(ipfi,encl,numpan,limi1,limi2,filtro,pipfi,campo));
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
   
   if (iserrno && (r == 2 || (r == 1 && iserrno == ELOCKED))) {
      sleep(1);
      c = pulsatecla();
      if (c == '\003' ) {
         mensaje(0,"Pulse 'N' ABORTA EL PROGRAMA");
         pito(37);pito(38);
         if (correcto() != 0) {
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
               if (m == 0) 
               {
                  prin_pan(ipfi,0);
                  if (entra_datos(MODIF,ipfi,0) < 0)
                     salida = -1;
               }
               else {
                  operaerror = 0;
                  p_ejecuta(m%100,O_NOR,faclin,34);
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
   int ant_defsid = default_sid;
   int ant_defwid = default_wid;

   default_sid = 0;
   default_wid = -1;
   
   Bl(24);
   while (-1) 
   {
      pprint(2427,"Entre la CLAVE :");
      qj = entras(2444,v_mpfi(ipfi)->f_campos[campo].c_qc,"",-1,qo);
      Bl(24);
      if (qj == 7 || qj == 1) 
						{
         es_la_doble_ = 0;
         caracter_de_fecha = '.';
         default_sid = ant_defsid;
         default_wid = ant_defwid;
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
            aviso(0,"Clave incorrecta");
            Cr();
            if (super_usuario)
            {
                default_sid = ant_defsid;
                default_wid = ant_defwid;
               return(0);
            }
         }
         else 
         {
             default_sid = ant_defsid;
             default_wid = ant_defwid;
            return(0);
         }
      }
   }
}

int cheq_clave_new(ipfi,campo,m)
int ipfi;
int campo;
int m;
{
   char clave[MAXDEFECTO];
   char qo[MAXDEFECTO];
   char clave1[MAXDEFECTO];

   
      if (m) 
      {
          clave1[0] = 0;
          st_campo(campo,v_mpfi(ipfi),clave1);          
          ld_campo(campo,v_mpfi(ipfi),clave1);
          encrip(clave1,clave);
          st_campo(campo,v_mpfi(ipfi),clave);
          return 0;
      }

      if (DespachadorExterno)
      {
          char tmp[512];
          int *p1 = (int *)tmp;
          ld_campo(campo,v_mpfi(ipfi),clave1);
          encrip(clave1,clave);
          quitab(clave);
          strcpy(tmp+sizeof(int)*2,clave);
          p1[0] = sizeof(int)*2 + strlen(clave) + 1;
          p1[1] = 0;
          (*DespachadorExterno)((int)DESPACHO_MAINCOMMAND,(int)ID_PREGUCLAVE,tmp);
          agi_cola_timer();
          if (p1[1] == IDOK)
              return 0;
          aviso(0,"Clave incorrecta");
          if (super_usuario)
               return(0);
          return -1;
      }
      else
          return 0;
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
      w = w_ini(101,9999);
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
#ifndef FGVWIN
   if (w)
   {
						funs = salva_funciones(0,-1);
                  fin_sig();
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
   if (w)
   {
						Terinici();
                  repon_funciones(funs,0,-1);
                  ini_sig();	     
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
      if (ultima_instalacion == 1 && stat(tmp,&buf) != -1)
      {
         if (!(info->build_numero))
            Paq[i].control_utime = buf.st_mtime;
         else
            Paq[i].control_utime = info->build_numero;
         Paq[i].flag = 3; /* instalado */
      }
      else
      {
         aviso(0,"DEBE REINICIAR PARA QUE SE EFECTUE LA INSTALACION!");
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
      sprintf(tmp,"%2d.%02d%c",Paq[lin].i.version1,Paq[lin].i.version2,Paq[lin].i.versionletra);
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
   if (recur_punto > 0)
   {
      sprintf(v_memo(5004)->alfabetica,"%d",-1);
   }
   else
   {
      sprintf(v_memo(5004)->alfabetica,"%d",carga_master(1));
   }
   n_np = 1;
   return(0);
}

int rw_desbloquea_master(p,tip,faclin)
int p;
int tip;
int faclin;
{
   descarga_master();
   n_np = 1;
   return(0);
}


static int nuevo_disco(char *tmp,int mi_volume,int preverr,char *origen)
{
   char tmp3[256];
   int j;
   
   sprintf(tmp3,"Inserte DISCO o SOPORTE Numero %d u otro que contenga %s",mi_volume,tmp);
   mensaje(0,tmp3);pito(39);
   
   /* posibilidad de cambio */
   strcpy(tmp3,origen);
   do {
      pprint(2401,"Pulse INTRO o entre nuevo origen:");
      j = entra(2435,32,"","",tmp3,1,tmp3);
      if (j == 1 || j == 7)
      {
         if (confirmar(2440," Desea Continuar la Instalacion (S/N)? ","S"))
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
   mensaje(0,"Preparando Instalacion ... Espere!");
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
         pprint(2401,"Entre Origen de la Instalacion:");
         i = entra(2433,32,"","",origen,1,origen);
         if (i == 1 || i == 7)
         {
            if (confirmar(2440," Desea Efectuar la Instalacion (S/N)? ","S"))
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
      aviso(0,"USE LA OPCION DE REINSTALAR");
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
   mensaje(0,"Preparando Instalacion ... Espere!");
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
               fprintf(file,"[Submenu]\n%s/fich/%s.opc\n",especial,especial);
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
               mensaje(0,"Preparando Instalacion FORMATO CLASICO ... Espere!");
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
               mensaje(0,"CONVIRTIENDO INSTALACION A FORMATO STANDARD ... Espere! .");
               Cp();
               agi_compress(tmpt,1);
               unlink(tmpt);
               /* */
               Ap();
               mensaje(0,"CONVIRTIENDO INSTALACION A FORMATO STANDARD ... Espere! ..");
               Cp();
               
               sprintf(tmp,"%s%s.tar %s.rwi",destino,paquete,paquete);
               agi_atar(tmp,destino);
               sprintf(tmp,"%s%s.tar",destino,paquete);
               sprintf(tmpt,"%s%s",destino,paquete);
               copiarfich(tmpt,tmp,1025);
               unlink(tmpt);
               Ap();
               mensaje(0,"CONVIRTIENDO INSTALACION A FORMATO STANDARD ... Espere! ...");
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
      mensaje(0,"Preparando Instalacion ... Espere!");
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
         aviso(0,"No se pudo instalar");
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
            aviso(0,"DEBE REINICIAR PARA QUE SE EFECTUE LA INSTALACION!");
         }
         memcpy(&(Paq[j].i),(char *)info,sizeof(struct info_paquete));
         /* Paq[j].i = *info; */
      }
   }
   
   if (!nlista)
   {
      aviso(0,"NO SE ENCONTRO INSTALACION O DISCO INCORRECTO");
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

    do 
    {
      if (!par->regcur || par->regcur > par->registros)
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
          nclave = par->encl;
          pregun_clave(&nclave,ipfi,-1,MODIF);

          if (nclave < 0)
          {
              break;
          }

          if (grabar_reg(ipfi,3,0) < 0) 
          {
              aviso(0,"Registro Existente");
              continue;
          }
          if (par->regcur > par->registros)
              alta = 2;
          else
              alta = 1;
      }
      else
      {
          alta = 0;
          pon_registro_actual(par->pfi,par->regsel );
          apunta_ind(par->pfi,0,ISEQUAL,0);
          apunta_ind(par->pfi,par->encl+1,ISCURR,0);

          /*
          if (leer_reg(ipfi,ISCURR,0,0) < 0)
          {
              continue;
          }
          
          TestigoRaton = 1;
          _ID_Cursor = ipfi + (par->curcol+1)*100;
          nclave = par->encl;
          pregun_clave(&nclave,ipfi,-1,MODIF);
          
          if (nclave < 0)
          {
              libera_ind(par->pfi);
              continue;
          }
          */         

          if (leer_reg(ipfi,ISCURR,ISLOCK,0) < 0)
          {
              break;
          }          
      }
      TestigoRaton = 1;
      _ID_Cursor = ipfi + (par->curcol+1)*1000;
      if (alta)
          modo_alta = ALTA;
      else
          modo_alta = MODIF;
      if (entra_datos(modo_alta,ipfi,-1) >= 0)
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

   (*DespachadorExterno)((int)DESPACHO_FOCOCONTROL,par->id_grid,NULL);

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
          break;
      }
      else
      {
          if (confirmar(0,"Confirmacion baja (S/N) ","N") < 0)
              break;

          pon_registro_actual(par->pfi,par->regsel );
          apunta_ind(par->pfi,0,ISEQUAL,0);
          apunta_ind(par->pfi,par->encl+1,ISCURR,0);
          if (leer_reg(ipfi,ISCURR,ISLOCK,0) < 0)
          {
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

   (*DespachadorExterno)((int)DESPACHO_FOCOCONTROL,par->id_grid,NULL);

return 0;
}

int InsertaCampoEnPantalla(int oipfi,int pan,int n,int desde,int wid)
{	
	  int i,ntab;

	  v_mpfi(oipfi)->f_campos[n].c_mante.c_actual = pan * 1000;

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
	  v_mpfi(oipfi)->f_taborder[desde] = n;
	  v_mpfi(oipfi)->f_taborder[ntab] = -1;
	  v_mpfi(oipfi)->f_camporden[n] = desde;
	  v_mpfi(oipfi)->f_campowids[n] = wid;
	  v_mpfi(oipfi)->f_campos[n].c_mante.c_visual = 0;

	  return 0;
}

int fgvsimplelineal(int p,int tip,int faclin)
{
    int k = prog[p].indice/100 - 1;
    int ipfi = v_prog(prog[p++].indice%100);
    int flags = pasa_a_int(v_memo(prog[p++].indice)->numerica);
    int pini = pasa_a_int(v_memo(prog[p++].indice)->numerica);
    int pfin = pasa_a_int(v_memo(prog[p++].indice)->numerica);
    int limi1 = prog[p++].indice;
    int limi2 = prog[p++].indice;
    int evento = prog[p++].indice;    
    int punto;
    int tecla;
    int nclave;
    int i;
    int alta;
    int modo_alta;
    int id_grid = -1;
    int oipfi = -1;
	int pan = 0;
    struct gridpars par,*_par;


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
  

  par.titulo = "";   

  _salva_datos_lock(ipfi);

  memset((void *)&par,0,sizeof(par));

  par.pfi   = v_mpfi(ipfi);
  par.ipfi  = ipfi;
  par.encl   = k;
  par.limi1  = limi1;
  par.limi2  = limi2;
  par.b_lim1 = NULL;
  par.b_lim2 = NULL;
  par.hook   = evento;
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

  punto = LanzaGrid(_par,NULL,NULL);

  if (!(flags & 32))
  {
      do
      {
          tecla = leecar_foco(1,v_mpfi(ipfi)->f_id,-1,-1,-1,v_mpfi(ipfi)->f_ipfi,_par->id_grid,NULL);
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
		  /*int campini,campfin;*/		   
          _par->control_campo = n + 1;
		  inserta_campo(v_mpfi(oipfi),n,"","",0,"","","");
		  InsertaCampoEnPantalla(oipfi,pan,n,9999,id_grid);
		  v_mpfi(oipfi)->f_campos[n].c_mante.c_modif = 0;

		  if (DespachadorExterno)
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
