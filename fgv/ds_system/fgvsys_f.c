
/**********************************************************************/
/**********************************************************************/
/*                    DIAGRAM9                                   */
/**********************************************************************/
/**********************************************************************/

/* otras rutinas para posible overlay */

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
#include <errno.h>

#include <rwmalloc.h>
#include <rtermin.h>
#include <rentra.h>
#define _NUEVODEF
#include <rfecha.h>
#include <rdef.h>       /* ya incluye el isam.h */
#include <rmante.h>
#include <rcalcul.h>

extern int is_alreves;

#include <stdlib.h>
/*extern long atol();*/

extern char empresa[MAXDEFECTO];
extern char proceso[81];
extern char titulo[81];
extern char bufpan[PANBUFSIZE];

extern char rw_version[256];

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

extern int rentra_atrib;
extern int rentra_sal_atrib;
extern int rentra_cc;
extern int rentra_cf;
extern int rentra_sal_cc;
extern int rentra_sal_cf;

extern int apli_aborto; /* flag para abortar la aplicacion (QUIT) */

extern char *quita_path();

extern int pasa_a_int(double);

extern char *aginfo;
extern int LEERTERMINAL;
extern char nmen0[551];

extern int e_contador; /* control de contadores */

extern long trata_memo();

extern long que_registro();

extern long da_registo_actual();

extern double ld_doble();

extern int *puntero_a_cual;

#define MAXBUFFERS 30 /* ojo */
extern char **buff_ers[MAXBUFFERS];
extern unsigned char tip_buf[MAXBUFFERS];

extern int n_np;
extern int mimodo_debug;

extern int tracer_int;
extern int ntr_br;
extern int tracer_break[];

extern long IxxNumeroPosicion;

#ifdef ESDOS
extern int (*timer_especial_int)();
#endif

extern int es_la_doble_;
extern int va_con_doble_;

extern int laszetassonfinal;

static char nomtem[40] = "TMP00000\0   "; /* algun caracter mas por si acaso */

#ifndef DSMINIBASICO

int temporal_(tmp)
char *tmp;
{
   int prog;
   int nf;
   char ext[5];
   char *pro;
   
   pro=getenv("ACPTMP");
   
   strcpy(ext,tmp);
   nf = atoi(nomtem+3);
   do {
      sprintf(nomtem+3,"%05d",nf);
      nf++;
      if (pro)
         strcpy(tmp,pro);
      else
         strcpy(tmp,dirfic);
      strcat(tmp,nomtem);
      strcat(tmp,ext);
   }while((prog = rw_open_(tmp,O_CREAT | O_EXCL | O_RDWR | O_BINARY,S_IREAD | S_IWRITE))<0);
   rw_close_(prog);
   agichmod(tmp,"","","");
   return(0);
}


/*******************************************************/
/*** rutinas de lectura de ficheros ***/
/*   rutina de ordenacion */


int bucle_break = 1; /* permitir o no interrupcion en los bucle_index */

int factor_secuencia = 27;

long bucle_conti[5] = {0L,0L,0L,0L,0L}; /* contador general de informacion usuario */
/* contra efecto psicologico */

static int numero_secuencia = 0;

extern int s_salida; /* variable de la rutina pausa_fin() */


static int a_cero_num_secuencia()
{
   if (numero_secuencia && numero_secuencia < 6)
			{
      bucle_conti[numero_secuencia-1] = 1L;
			}
   return(0);
}


static long max_progreso = 0;
static long salto_progreso = 0;
static long punto_progreso = 0;
static long cur_progreso = 0;

static int progreso_secuencia()
{
	if (numero_secuencia) return 0;

	if (cur_progreso > punto_progreso)
	{
		punto_progreso += salto_progreso;
	}
	else
		return 1;
    if (DespachadorExterno)
    {
		char tmpp[256];
		sprintf(tmpp,"%ld",cur_progreso);
		(*DespachadorExterno)((int)DESPACHO_MULTIPLE,3,tmpp);
    }
	return 1;
}



static long suma_num_secuencia()
{
   if (numero_secuencia && numero_secuencia < 6)
			{
      bucle_conti[numero_secuencia-1]++;
      return(bucle_conti[numero_secuencia-1]);
			}
   return(1L);
}


static char *saca_num_secuencia()
{
   int i;
   static char tmp[512];
   tmp[0] = '#';
   tmp[1] = 0;
   if (numero_secuencia && numero_secuencia < 6)
			{
      for (i = 0;i < numero_secuencia;i++)
      {
         if (i)
            strcat(tmp,"/");
         sprintf(tmp+strlen(tmp),"%ld",bucle_conti[i]);
      }
      strcat(tmp,"             ");
			}
   return(tmp);
}

static void mensaje_secuencia()
{
	mensaje_en_cabecera(__Texto[194],saca_num_secuencia(),0);
}

static int pasa_un_segundo()
{
	static unsigned long tiempo = 0L;
	unsigned long ahora = time(NULL);
	if (ahora == tiempo)
		return 0;
	tiempo = ahora;
	return 1;
}

static int pasa_un_segundo_1()
{
	static unsigned long tiempo = 0L;
	unsigned long ahora = time(NULL);
	if (ahora == tiempo)
		return 0;
	tiempo = ahora;
	return 1;
}


cambia_indices(tabla,i,j)
int tabla;
long i,j;
{
   long tmp1,tmp2;
   
   rw_lseek_(tabla,(i * sizeof(long)),SEEK_SET);
   rw_read_(tabla,(char *)&tmp1,sizeof(long));
   rw_lseek_(tabla,(j * sizeof(long)),SEEK_SET);
   rw_read_(tabla,(char *)&tmp2,sizeof(long));
   rw_lseek_(tabla,(j * sizeof(long)),SEEK_SET);
   rw_write_(tabla,(char *)&tmp1,sizeof(long));
   rw_lseek_(tabla,(i * sizeof(long)),SEEK_SET);
   rw_write_(tabla,(char *)&tmp2,sizeof(long));
}

static int lee_datos(prog,tabla,indice,buf,_pfi)
int prog,tabla;
long indice;
char *buf;
FI_DEF *_pfi;
{
			long indi;
         int i;
         
#ifdef ESDOS
         if (timer_especial_int)
            (*timer_especial_int)();
#endif
         rw_lseek_(tabla,(indice * sizeof(long)),SEEK_SET);
         rw_read_(tabla,(char *)&indi,sizeof(long));
         lee_directo(prog,indi,_pfi,buf);
}


int compara_campos(prog,tabla,indice,campos,_ipfi,buf,tamr)
int prog;
int tabla;
long indice;
short *campos;
int _ipfi;
char *buf;
int tamr;
{
   int i,ord;
   int compara = 0;
   
   lee_datos(prog,tabla,indice,buf,v_mpfi(_ipfi));
   
   while( (i = *campos) != -32535) {
      if (i < 0) {
         i = (-i) - 2;
         ord = -1;
						}
      else
         ord = 1;
      if ((compara = _campotrat(i,i,ord,_ipfi,_ipfi,v_mpfi(_ipfi)->f_buf,buf,0) ) == 0)
         campos++;
      else
         break;
   }
   
   return(compara);
}

int usa_sel(nc,co,m,salto)
short nc,*co,m,salto;
{
   int i;
   int c;
   int d;
   
   for (i = salto;i < nc;i++) {
						c =  *(co + i);d = c / 10000;c %= 10000;
                  if ( (m < 0 && d >= (-m)) || d == m)
                     if (condicion(c) == -1) {i = -1;break;}
   }
   return((i != -1) ? 0 : -1);
}

static long crea_tabla_(ipfi,prog,tabla,tamr,nsel,sel)
int ipfi;
int prog,tabla;
int tamr;
short nsel,*sel;
{
   long cont = 0L;
   int modo = 0;
   long pos;
   
   rw_lseek_(tabla,0L,SEEK_SET);
   
   while (!lee_secuencia(v_mpfi(ipfi),prog,&pos,&modo,tamr,v_mpfi(ipfi)->f_buf))
   {
#ifdef ESDOS
      if (timer_especial_int)
         (*timer_especial_int)();
#endif	  
      if (usa_sel(nsel,sel,0,0) >= 0)
      {		 
         rw_write_(tabla,(char *)&pos,sizeof(long));
         cont++;
      }
   }
   return(cont);
}

int sortea_(campos,_ipfi,prog,tabla,buf,tamr,izquierda,derecha)
short *campos;
int _ipfi;
int prog,tabla;
char *buf;
int tamr;
long izquierda,derecha;
{
   long i,j;
   long *st = (long *)malloc(1000 * sizeof(long));
   long *fl = (long *)malloc(1000 * sizeof(long));
   int  *ha = (int  *)malloc(500 * sizeof(int));
   /* 500 recursiones */
   int  ri = 0, salida = 0;
   
   *st     = izquierda;
   *(st + 1) = derecha  ;
   *fl     = -1L;
   *(fl + 1) = -1L;
   *ha = 0;
   
   do {
      
      izquierda = *(st + ri);
      derecha   = *(st + ri + 1);
      
#ifdef ESDOS
      if (timer_especial_int)
									(*timer_especial_int)();
#endif
      
      if (*(fl + ri) == -1L) {
									i = izquierda;
                           j = derecha;
                           lee_datos(prog,tabla,(izquierda+derecha)/2,v_mpfi(_ipfi)->f_buf,v_mpfi(_ipfi));
                           do {
                              while (i < derecha &&
                                 compara_campos(prog,tabla,i,campos,_ipfi,buf,tamr) > 0) i++;
                              while (j > izquierda &&
                                 compara_campos(prog,tabla,j,campos,_ipfi,buf,tamr) < 0) j--;
#ifdef ESDOS
                              if (timer_especial_int)
                                 (*timer_especial_int)();
#endif
                              
                              if (i <=j) {
                                 cambia_indices(tabla,i,j);
                                 i++;j--;
                              }
                           }while(i <=j);
                           
                           *(fl + ri)     = i  ;
                           *(fl + ri + 1) = j  ;
      }
      else {
									i = *(fl + ri);
                           j = *(fl + ri + 1);
      }
      
      if (izquierda < j && ha[ri / 2] == 0) {
									ha[ri / 2] = 1;
                           ri += 2;
                           if (ri >= 500) {salida = -1;break;}
                           *(st + ri)     = izquierda;
                           *(st + ri + 1) = j  ;
                           *(fl + ri)     = -1L;
                           *(fl + ri + 1) = -1L;
                           ha[ri / 2] = 0;
                           continue;
      }
      
      if (i < derecha && ha[ri / 2] < 2) {
									ha[ri / 2] = 2;
                           ri += 2;
                           if (ri >= 500) {salida = -1;break;}
                           *(st + ri)     = i  ;
                           *(st + ri + 1) = derecha ;
                           *(fl + ri)     = -1L;
                           *(fl + ri + 1) = -1L;
                           ha[ri / 2] = 0;
                           continue;
      }
      
      ri -= 2;
   } while(ri >= 0);
   
   free((char *)fl);
   free((char *)st);
   free((char *)ha);
   return(salida);
}


#ifdef RWXARXAIDX

typedef struct _MSGHDR
{
   BYTE            signature;
   BYTE            command;
   WORD            length;
} MSGHDR;

typedef struct _DATAMSG
{
   MSGHDR          hdr;
   BYTE            *data;
} DATAMSG;

extern int _en_conexion_server_;
extern DATAMSG xsndbuf;
extern int respuesta_xarxa;
#define AGI_SORT                  7
#define RW_INDEX                  8

#endif

static int convierte_condicion(f,ipfi,pdata)
int f;
int ipfi;
char *pdata;
{
   int n = 0;
   M_VAL val1;
   M_VAL val2;
   short tipo1;
   short tipo2;
   int f1,c1,f2,c2,sig,salida;
   short tip;
   char con;
   f1 = v_prog(cond[f].fichero1);
   c1 = cond[f].campo1;
   f2 = v_prog(cond[f].fichero2);
   c2 = cond[f].campo2;
   sig = cond[f].sigue;
   tip = cond[f].tipo; /* 0 = Y */
   con = cond[f].condicion;
   tipo1 = busca_valor(&f1,&c1,&val1);
   tipo2 = busca_valor(&f2,&c2,&val2);
   
   if (f1 == ipfi)
   {
      n += 4;
	  sprintf(pdata,"K%03d",c1);
      pdata += 4;
   }
   else
   {
	  *pdata = '\'';
	  pdata++;
	  n++;
	  if (tipo1 == CHARTIPO || tipo1 == FECHATIPO)
	  {
		  if (tipo1 == FECHATIPO || tipo2 == FECHATIPO)
		  {
			  stfecha(val1.valc,pdata);
			  pdata[8] = 0;			  
		  }
		  else
		  {
			  strcpy(pdata,val1.valc);
		  }
	  }
	  else
	  {
		  sprintf(pdata,"%lf",val1.vald);
		  quitadec(pdata);
	  }
	  n += strlen(pdata);
	  pdata += strlen(pdata);
	  *pdata = '\'';
	  pdata++;
	  n++;
   }
   switch(con)
   {
   case '>':
	   strcpy(pdata,">");
	   break;
   case '<':
	   strcpy(pdata,"<");
	   break;
   case ']':
	   strcpy(pdata,">=");
	   break;
   case '[':
	   strcpy(pdata,"<=");
	   break;
   case '!':
	   strcpy(pdata,"!=");
	   break;
   default:
	   strcpy(pdata,"=");
	   break;
   }
   n += strlen(pdata);
   pdata += strlen(pdata);

   if (f2 == ipfi)
   {
      n += 4;
	  sprintf(pdata,"K%03d",c2);
      pdata += 4;
   }
   else
   {
	  *pdata = '\'';
	  pdata++;
	  n++;
	  if (tipo2 == CHARTIPO || tipo2 == FECHATIPO)
	  {		  
		  if (tipo1 == FECHATIPO || tipo2 == FECHATIPO)
		  {
			  stfecha(val2.valc,pdata);
			  pdata[8] = 0;			  
		  }
		  else
		  {
			  strcpy(pdata,val2.valc);
		  }
	  }
	  else
	  {
		  sprintf(pdata,"%lf",val2.vald);
		  quitadec(pdata);
	  }
	  n += strlen(pdata);
	  pdata += strlen(pdata);
	  *pdata = '\'';
	  pdata++;
	  n++;
   }
   
   if (sig < 0)
   {
      *pdata = 0;
      return(n);
   }
   if (!tip)
   {
	   strcpy(pdata," AND ");
   }
   else
   {
	   strcpy(pdata," OR ");
   }
   n += strlen(pdata);
   pdata += strlen(pdata);

   return(n+convierte_condicion(sig,ipfi,pdata));
}

int _ordena(ipfi,campos,tempo,nsel,sel)
int ipfi;
short *campos;
char *tempo;
short nsel,*sel;
{
   char nom[MAXDEFECTO];
   unsigned long t = 0L;
   int prog , tabla;
   int tamr;
   char *buf;
   int i,n,nn,r;
   
#ifdef RWXARXAIDX
   short k;
   int c;
   int d;
   char *pdata;
   short *p;
   
   if (_en_conexion_server_)
   {
      
      if (( prog = abre_directo(v_mpfi(ipfi),nom)) < 0) 
	  {
         return(-1);
      }
      strcpy(tempo,".ord");
      temporal_(tempo);
      if (( tabla = rw_open_(tempo,O_BINARY | O_RDWR,
         S_IREAD | S_IWRITE)) < 0) {
         cierra_directo(prog);
         return(-1);
      }
      
      tamr = v_mpfi(ipfi)->f_lfich;
      
      mensaje_en_cabecera(__Texto[192],"",0);
      
      pdata = xsndbuf.data;
      strcpy(pdata,dirfic);
      pdata += 51;
      strcpy(pdata,tempo);
      pdata += 51;
      memcpy(pdata,(char *)v_mpfi(ipfi),sizeof(FI_DEF));
      pdata += sizeof(FI_DEF);
      memcpy(pdata,(char *)(v_mpfi(ipfi)->f_campos),sizeof(CAMPO_DEF) * v_mpfi(ipfi)->f_ncampos);
      pdata += (sizeof(CAMPO_DEF) * v_mpfi(ipfi)->f_ncampos);
      d = 0;
      
      while((r = *(campos+d)) != -32535) d++;
      
      d++;
      *((short *)pdata) = (short)d;
      pdata += sizeof(short);
      memcpy(pdata,campos,sizeof(short)*d);
      pdata += (sizeof(short)*d);
      n = 51 + 51 + sizeof(FI_DEF);
      n += sizeof(CAMPO_DEF) * v_mpfi(ipfi)->f_ncampos;
      n += (sizeof(short)*(d+2));
      p = (short *)pdata;
      pdata += sizeof(short);
      k = 0;
      for (i = 0;i < nsel;i++)
      {
         c =  *(sel + i);
         d = c / 10000;
         c %= 10000;
         if (d == 0)
         {
            nn = examina_condicion(c,ipfi,pdata);
            n += nn;
            pdata += nn;
            k++;
         }
      }
      *p = k;
      xsndbuf.hdr.length = sizeof(MSGHDR) + n;
      xsndbuf.hdr.command = AGI_SORT;
      /* respuesta_xarxa = 1;*/
      if (!envia_xarxa(&xsndbuf))
      {
         
      /*
      while(respuesta_xarxa == 1)
      {
      agi_cola_win();
      }
      if (respuesta_xarxa == -1)
      {
      respuesta_xarxa = 0;
      return(-1);
      }
      respuesta_xarxa = 0;
         */
         if (responde_xarxa(AGI_SORT,NULL))
         {
            cierra_directo(prog);
            rw_close_(tabla);
            unlink(tempo);
            return(-1);
         }
         borra_mensaje_cabecera(0);
         return(tabla * 100 + prog);
      }
      else
      {
         aviso(0,__Texto[51]);
         cierra_directo(prog);
         rw_close_(tabla);
         unlink(tempo);
      }
   }
   
#endif
   

   if (( prog = abre_directo(v_mpfi(ipfi),nom)) < 0) 
   {
      return(-1);
   }
   strcpy(tempo,".ord");
   temporal_(tempo);
   if (( tabla = rw_open_(tempo,O_BINARY | O_RDWR,
      S_IREAD | S_IWRITE)) < 0) 
   {
      cierra_directo(prog);
      return(-1);
   }
   
   
   mensaje_en_cabecera(__Texto[193],"",0);
   
   tamr = v_mpfi(ipfi)->f_lfich;
   
   buf = malloc(tamr);
   
#ifdef FGV32
#ifndef WATCOMNLM
   n = GetPriorityClass(GetCurrentProcess());
   i = GetThreadPriority(GetCurrentThread());
   if (n > NORMAL_PRIORITY_CLASS)
   {
      SetPriorityClass(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);
   }
   if (n >= NORMAL_PRIORITY_CLASS)
   {
      SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_BELOW_NORMAL);
   }
#endif
#endif

   if (DeterminaLlamada(v_mpfi(ipfi)))
   {
#ifdef ESDOS
	   char buffer[10000];
	   char orden[5000];
	   char *pdata = buffer;
	   short *camp = campos;
	   int i;
	   int c;
	   int d,ord;
	   ord = 0;
	   strcpy(buffer,"WHERE ");
	   pdata += strlen(pdata);
	   for (i = 0;i < nsel;i++) 
	   {
			c =  *(sel + i);d = c / 10000;c %= 10000;
			if (d == 0)
			{
			   if (ord)
			   {
				   strcpy(pdata," AND ");
				   pdata += strlen(pdata);
			   }
			   pdata += convierte_condicion(c,ipfi,pdata);				   			   
			   ord++;
			}
	   }
	   pdata = orden;
	   strcpy(orden,"ORDER BY ");
	   pdata += strlen(pdata);
	   d = 0;
	   while( (i = *camp) != -32535) 
	   {
		  if (i < 0) 
		  {
			 i = (-i) - 2;
			 ord = -1;
		  }
		  else
		  {
			 ord = 1;
		  }
		  if (d)
		  {
			  *pdata = ',';
			  pdata++;
		  }
		  sprintf(pdata,"K%03d",i);
		  pdata += strlen(pdata);
		  if (ord < 0)
		  {
			 strcpy(pdata," DESC ");
		  }
		  else
		  {
			 strcpy(pdata," ASC ");
		  }
		  pdata += strlen(pdata);
		  camp++;
		  d++;
		  if (d > 500)
			  break;
	   }
	   rw_close_(tabla);
	   sql_resultado_en_bloque(v_mpfi(ipfi),prog,tempo,buffer,orden);
       tabla = rw_open_(tempo,O_BINARY | O_RDWR,S_IREAD | S_IWRITE);
#endif
   }
   else
   {
	   t = crea_tabla_(ipfi,prog,tabla,tamr,nsel,sel);
   
	   if (bucle_break) 
	   {
		  if (pausa_fin()) 
		  {
#ifdef FGV32
#ifndef WATCOMNLM
			 SetPriorityClass(GetCurrentProcess(),n);
			 SetThreadPriority(GetCurrentThread(),i);
#endif
#endif
			 borra_mensaje_cabecera(0);
			 cierra_directo(prog);
			 rw_close_(tabla);
			 return(-1);
		  }
	   }
#ifdef FGVWIN
	   else
		  agi_cola_win();
#endif
   
	   if (t > 1L && *campos != -32535) 
	   {
		  t--;
		  sortea_(campos,ipfi,prog,tabla,buf,tamr,0L,t);
	   }
   }
   
#ifdef FGV32
#ifndef WATCOMNLM
   SetPriorityClass(GetCurrentProcess(),n);
   SetThreadPriority(GetCurrentThread(),i);
#endif
#endif
   
   borra_mensaje_cabecera(0);
   
   free(buf);
   

   if ( (prog&0x40000000) )
   {
	   prog &= ~0x40000000;
	   return(((tabla * 100 + prog) | 0x40000000));
   }
   else
   {
	   return(tabla * 100 + prog);
   }
}


/* fin rutinas ordenacion */


carga_cam(f1,p,i,mm)
int f1;
int p,i,mm; /* mm == 0 graba mm == 1 lee */
{
   short f,c,tip;
   char tmp[MAXDEFECTO];
   
   comoes_cam(p,(short *)0,&tip,(short *)0,&f,&c,(short *)0);
 
   
   if (f == -1) {
      if (tip == CHARTIPO || tip == FECHATIPO) {
         if (mm == 0)
            st_campo(i,v_mpfi(f1),v_memo(c)->alfabetica);
         else
            ld_campo(i,v_mpfi(f1),v_memo(c)->alfabetica);
      }
      else {
         if (mm == 0) {
            st_doble(i,v_mpfi(f1),v_memo(c)->numerica);
         }
         else {
            v_memo(c)->numerica = ld_doble(i,v_mpfi(f1));
         }
      }
   }
   else {
      if (mm == 0)
         _copia_cam(f1,i,f,c);
      else
         _copia_cam(f,c,f1,i);
   }
   return(0);
}

static int CambiaLasZetas(int f,int c)
{  
  int qc;
  int i;
  char *p;
  if (!laszetassonfinal)
  {
	  return 0;
  }
  if ((qc = v_mpfi(f)->f_campos[c].c_qc) > 0)
  {
	  p = v_mpfi(f)->f_buf + v_mpfi(f)->f_campos[c].c_offset;
	  for (i = 0;i < qc;i++)
	  {
		  if (*p != 'z' && *p != 'Z')
			  break;
		  p++;
	  }
	  if (i >= qc)
	  {
		  p = v_mpfi(f)->f_buf + v_mpfi(f)->f_campos[c].c_offset;
		  memset(p,(unsigned char)0xff,qc);
		  return 1;
	  }
  }
  return 0;
}

/*  ===================================================================== */
int carga_baremo(f,key,cc,buf)
short f,key,*cc;
char *buf;
{
   int i;
   int nc = v_mpfi(f)->f_clave[key-1].k_ncampos;
   int c;
   
   if (buf == (char *)0) return(0);
   
   for (i=0;i < nc;i++) 
   {
      c = v_mpfi(f)->f_clave[key-1].k_campo[i] - 1;
      carga_cam(f,*(cc+i),c,0);
	  if (laszetassonfinal)
	  {
		  if (v_mpfi(f)->f_campos[c].c_qc > 0)
		  {
			  CambiaLasZetas(f,c);
		  }
	  }
   }
   memcpy(buf,v_mpfi(f)->f_buf,v_mpfi(f)->f_lfich);
   return(0);
}


static int compara_doble(ipfi,key,buf1,buf2)
int ipfi,key;
char *buf1,*buf2;
{
		int i,campo,orden,compara;
         
         for (i = 0;i < v_mpfi(ipfi)->f_clave[key-1].k_ncampos;i++) 
         {
            campo = v_mpfi(ipfi)->f_clave[key-1].k_campo[i] - 1;
            orden = v_mpfi(ipfi)->f_clave[key-1].k_orden[i];
            compara = _campocmp(campo,orden,ipfi,buf1,buf2,0);
            if (compara < 0)
               return(-1);
            if (compara > 0)
               return(1);
         }
         return(0);
}


static int compara_baremo(ipfi,key,buf1,buf2,la_doble,cual,buf_simple,buf_doble)
int ipfi,key;
char *buf1,*buf2;
int la_doble,cual;
char *buf_simple,*buf_doble;
{
   int i,campo,orden,compara,menor = 0;
   int salva_fd,res;
   
   for (i = 0;i < v_mpfi(ipfi)->f_clave[key-1].k_ncampos;i++) {
      campo = v_mpfi(ipfi)->f_clave[key-1].k_campo[i] - 1;
      orden = v_mpfi(ipfi)->f_clave[key-1].k_orden[i];
      
      compara = _campocmp(campo,orden,ipfi,v_mpfi(ipfi)->f_buf,buf1,1+menor);
      if (compara < 0 || menor == 1) {
         menor = 1;
         continue;
						}
      else {
         compara = _campocmp(campo,orden,ipfi,v_mpfi(ipfi)->f_buf,buf2,0);
         if (compara > 0) 
         {
            if (i == 0) return(-2);
            if (la_doble != -1 && cual == 1)
            {
               salva_fd = v_mpfi(ipfi)->f_fd;
               v_mpfi(ipfi)->f_fd = la_doble;
            }        
            res = apunta_ind(v_mpfi(ipfi),key,ISGREAT,i);
            if (la_doble != -1)			
            {		   
               if (cual == 0)
               {
                  memcpy(buf_simple,v_mpfi(ipfi)->f_buf,v_mpfi(ipfi)->f_lfich); 
               }
               else
               {
                  memcpy(buf_doble,v_mpfi(ipfi)->f_buf,v_mpfi(ipfi)->f_lfich); 
                  v_mpfi(ipfi)->f_fd = salva_fd;
               }
            }
            if (res < 0)
               return(-2);
            return(2);
         }
						}
   }
   if (menor == 1)
	   return(0);
   return(-1); /* es bueno */
}

static int leer_bsecu(ipfi,key,bloq,trato,buf1,buf2,m,regis,la_doble,buf_simple,buf_doble,hay_1,hay_2,cual,salto_doble,es_doble_join,hay_contenido)
int ipfi,key,bloq,trato;
char *buf1,*buf2;
int m;
long *regis;
int la_doble;
char *buf_simple;
char *buf_doble;
int *hay_1,*hay_2,*cual,*salto_doble;
int es_doble_join;
int hay_contenido;
{
   long rr;
   int leo,r,err,jj;
   char wtmp[81];
   int res;
   int salva_fd;
   double numd;
   
   if (bucle_break && s_salida) return(-1);
   
   do {
					
	   
				if (!progreso_secuencia() && !(suma_num_secuencia() % factor_secuencia) && pasa_un_segundo()) 
                  {        
                     mensaje_secuencia();
                  }
        
                  
                  if (m == 0) 
                  {
                     if (buf1 == (char *)0 || hay_contenido)
                        leo = ISFIRST;
                     else
                        leo = ISGTEQ; /* no debe con contenido */
                  }
                  else
                     if (m == 1) 
                     {
                        if (hay_contenido)
                        {
                           leo = ISNEXT;
                        }
                        else
                           if (*regis) 
                           {      
                              pon_registro_actual(v_mpfi(ipfi),*regis);
                              apunta_ind(v_mpfi(ipfi),0,ISEQUAL,0);
                              apunta_ind(v_mpfi(ipfi),key,ISCURR,0);
                              leo = ISCURR;
                              *regis = 0L;
                           }
                           else
                              leo = ISNEXT;
                     }
                     else
                        if (m == 2)
                           leo = ISCURR; 
                        
                        if (la_doble != -1) /* doblete */
                        {
                           *regis = 0L;
                           if (leo != ISNEXT && leo != ISCURR) 
                           { /* leer ambas fichas */	   
                              memcpy(buf_doble,v_mpfi(ipfi)->f_buf,v_mpfi(ipfi)->f_lfich);
                              res = leer_reg(ipfi,leo,0 /*bloq*/,trato); /* la standar */
                              *hay_1 = res;
                              memcpy(buf_simple,v_mpfi(ipfi)->f_buf,v_mpfi(ipfi)->f_lfich);
                              
                              memcpy(v_mpfi(ipfi)->f_buf,buf_doble,v_mpfi(ipfi)->f_lfich);
                              
                              salva_fd = v_mpfi(ipfi)->f_fd;
                              
                              v_mpfi(ipfi)->f_fd = la_doble;
                              res = leer_reg(ipfi,leo,0 /*bloq*/,trato); /* la 'otra' */	   
                              *hay_2 = res;	   
                              memcpy(buf_doble,v_mpfi(ipfi)->f_buf,v_mpfi(ipfi)->f_lfich);	   
                              
                              v_mpfi(ipfi)->f_fd = salva_fd;
                              
                              *salto_doble = 0;
                           }
                           else
                           {
                              if (*cual == 0)
                              {
                                 res = leer_reg(ipfi,leo,0 /*bloq*/,trato); /* la standar */
                                 *hay_1 = res;
                                 memcpy(buf_simple,v_mpfi(ipfi)->f_buf,v_mpfi(ipfi)->f_lfich);
                              }
                              if (*salto_doble)
                              {
                                 memcpy(v_mpfi(ipfi)->f_buf,buf_doble,v_mpfi(ipfi)->f_lfich); /* realmente no ha de ser necesario */
                              }
                              if (*cual == 1 || *salto_doble)
                              {
                                 salva_fd = v_mpfi(ipfi)->f_fd;	   
                                 v_mpfi(ipfi)->f_fd = la_doble;
                                 res = leer_reg(ipfi,leo,0 /*bloq*/,trato); /* la 'otra' */	   
                                 *hay_2 = res;	   
                                 memcpy(buf_doble,v_mpfi(ipfi)->f_buf,v_mpfi(ipfi)->f_lfich);
                                 v_mpfi(ipfi)->f_fd = salva_fd;
                                 *salto_doble = 0;
                              } 
                           }
                           
                           if (*hay_1 && *hay_2) return(-1);
                           
                           res = -1;
                           if (*hay_1)
                           {
                              *cual = 1;
                           } 
                           else
                              if (*hay_2)
                              {
                                 *cual = 0;
                              } 
                              else 
                              {
                                 res = compara_doble(ipfi,key,buf_simple,buf_doble);
                                 if (res > 0 && !(*salto_doble))
                                 {
                                    *cual = 1;
                                 }
                                 else
                                 {
                                    *cual = 0;           
                                 }
                              }
                              if (*cual == 0)	   
                              {	   
                                 if (!res && es_doble_join)
                                 {
                                    /* sumar los campos numericos */
                                    for (jj = 0;jj < v_mpfi(ipfi)->f_ncampos;jj++)
                                    {
                                       if (v_mpfi(ipfi)->f_campos[jj].c_tipo == DOUBLETIPO && v_mpfi(ipfi)->f_campos[jj].c_mante.c_qpl == 77)
                                       {
                                          numd = lddbl(buf_simple+(v_mpfi(ipfi)->f_campos[jj].c_offset));
                                          numd +=lddbl(buf_doble+(v_mpfi(ipfi)->f_campos[jj].c_offset));
                                          stdbl(numd,buf_simple+(v_mpfi(ipfi)->f_campos[jj].c_offset));
                                       }
                                    }
                                    *salto_doble = 1;
                                 }
                                 memcpy(v_mpfi(ipfi)->f_buf,buf_simple,v_mpfi(ipfi)->f_lfich);
                              }
                              else
                                 memcpy(v_mpfi(ipfi)->f_buf,buf_doble,v_mpfi(ipfi)->f_lfich);
                              res = 0;
                        }
                        else
						{						   
                           res = leer_reg(ipfi,leo,0/*bloq*/,trato);
						   cur_progreso = IxxNumeroPosicion;
						}
                        
                        
                        if (res < 0) 
                        {
                           if (iserrno != ELOCKED)
                           {
                              *regis = 0L;
                              return(-1);
                           }
                           else
                           {
                              leer_reg(ipfi,leo,0,trato);
							  cur_progreso = IxxNumeroPosicion;
							  m = 1;
							  *regis = 0L;
							  continue;/* pasar */
							  
                           }
                        }
                        
                        
                        if (buf1 == (char *)0)
                        {
                           *regis = 0L;return(0);
                        }
                        
                        m = compara_baremo(ipfi,key,buf1,buf2,la_doble,*cual,buf_simple,buf_doble);
                        /* si es menor devuelve 0 y sigue en el bucle leyendo con modo ISGTEQ */
                        /* en la clave ya ha cargado el siguiente limite */
                        
                        if (m == -1) { /* bueno! */
                           
                           if (hay_contenido) return(0);
                           
                           
                           if (la_doble != -1) return(0); /* caso doble solo lectura 'limpia' */

						   if (!bloq) return 0;
						   
						   res = leer_reg(ipfi,ISCURR,bloq,trato);
						   if (res < 0) 
						   {
							   if (iserrno != ELOCKED)
							   {
								  *regis = 0L;
								  return(-1);
							   }
							   else
							   {
								  leer_reg(ipfi,leo,0,trato);
								  cur_progreso = IxxNumeroPosicion;
								  m = 1;
								  *regis = 0L;
								  continue;/* pasar */								  
							   }
						   }
                           /* recojo el siguiente */
                           if (lee_soloindice(v_mpfi(ipfi),ISNEXT) < 0)
                           {
                              *regis = 0L;
                              lee_soloindice(v_mpfi(ipfi),ISLAST);
                           }
                           else
                           {
                              *regis = isrecnum;
                              lee_soloindice(v_mpfi(ipfi),ISPREV);
                           }
						   cur_progreso = IxxNumeroPosicion;
                           return(0);
                        }
                        
                        libera_ind(v_mpfi(ipfi)); /* solo queda bloqueado si es bueno */
                        
                        if (m == -2) return(-1);
                        
                        if (!progreso_secuencia() && !(suma_num_secuencia() % 10) && pasa_un_segundo_1()) 
						{
                           if (bucle_break)
                           {
                              if (pausa_fin()) {
                                 return(-1);
                              }
                           }
#ifdef FGVWIN
                           else
								 if (factor_secuencia < 28)
								 {
									agi_cola_win();
								 }
#endif
                        }
                        
 } while(-1);
 return(0);
}



int fi_tempo(npt,p,cal,m)
short npt,*p,cal,m;
{
   int i,f,c;
   short lon,tip;
   int pos,nf;
   static short *nt[6] = {(short *)0,(short *)0,(short *)0,(short *)0,(short *)0,(short *)0};
   static long ps[6];
   static char *tm[6];
   static int  fd[6];
   short tf,ord;
   int np = npt / 100;
   int cp = npt % 100;
   int tc = ( (np - cp) > MAXCAMPOSPORCLAVE) ? 1 : 2;
   short *campos;
   long post;
   double vca,vcf;
   
   if (m == 0) {
      for(tf = 0;(tf < 6 && nt[tf] != (short *)0);tf++);
      if (tf == 6) return(-1);
      nt[tf] = p;
      tf += (MAXDEF-1);
   }
   else {
      for(tf = 0;(tf < 6 && nt[tf] != p);tf++);
      if (tf == 6) return(-1);
      tf += (MAXDEF-1);
   }
   
   if (m == 0) {
      mpfi[tf] = (FI_DEF *) malloc(sizeof(FI_DEF));
      memset(mpfi[tf],0,sizeof(FI_DEF));
      mpfi[tf]->f_titulo = NULL;
      mpfi[tf]->f_npanta = 0;
      mpfi[tf]->f_nclaves = tc;
      mpfi[tf]->f_ncampos = np;
      mpfi[tf]->f_campos = (CAMPO_DEF *)malloc(np * sizeof(CAMPO_DEF));
      memset(mpfi[tf]->f_campos,0,np * sizeof(CAMPO_DEF));
      mpfi[tf]->f_clave = (KEY_DEF *)malloc(tc * sizeof(KEY_DEF));
      for (i = 0;i < tc;i++) {
         mpfi[tf]->f_clave[i].k_orden = (short *)malloc(MAXCAMPOSPORCLAVE*sizeof(short));
         mpfi[tf]->f_clave[i].k_campo = (short *)malloc(MAXCAMPOSPORCLAVE*sizeof(short));
      }
      mpfi[tf]->f_nrelaci = 0;
      pos = 0;
      for (i = 0;i < np;i++) {
         mpfi[tf]->f_campos[i].c_offset = pos;
         ord = 1;
         comoes_cam(*(p+i),(short *)0,&tip,&lon,(short *)0,(short *)0,&ord);
         mpfi[tf]->f_campos[i].c_long = lon;
         if (tip == FECHATIPO)
            mpfi[tf]->f_campos[i].c_qc = -1;
         else
            mpfi[tf]->f_campos[i].c_qc = lon;
         mpfi[tf]->f_campos[i].c_tipo = tip;
         mpfi[tf]->f_campos[i].c_minimo = "";
         mpfi[tf]->f_campos[i].c_nombre = "";
         mpfi[tf]->f_campos[i].c_lista = "";
         mpfi[tf]->f_campos[i].c_maximo = "";
         mpfi[tf]->f_campos[i].c_usi = "";
         mpfi[tf]->f_campos[i].c_mante.c_mensaje = "";
         mpfi[tf]->f_campos[i].c_mante.c_defecto = "";
         mpfi[tf]->f_campos[i].c_mante.c_relacion = "";
         mpfi[tf]->f_campos[i].c_mante.c_qpl = 80;
         
         
         if (tip == DOUBLETIPO) {
         /* es correcto: es solo para despistar al redondeo que se hace
            en rfidoble */
            mpfi[tf]->f_campos[i].c_usi = "%14.1lf"; /* puntero a literal */
            mpfi[tf]->f_campos[i].c_maximo = "99999999999999.*"; 
            mpfi[tf]->f_campos[i].c_maximo = "-9999999999999.*"; 
            mpfi[tf]->f_campos[i].c_qc = 0;
         }
         else {
            if (tip == INTTIPO) 
            {
               mpfi[tf]->f_campos[i].c_usi = "%6ld"; /* puntero a literal */
               mpfi[tf]->f_campos[i].c_maximo = "32535"; 
               mpfi[tf]->f_campos[i].c_maximo = "-32535"; 
               mpfi[tf]->f_campos[i].c_qc = 0;
            }
            else
               mpfi[tf]->f_campos[i].c_usi = ""; /* puntero a \0 */
         }
         if (i < cp) {
            mpfi[tf]->f_clave[0].k_campo[i] = i + 1;
            mpfi[tf]->f_clave[0].k_orden[i] = ord;
            mpfi[tf]->f_campos[i].c_key = 1;
            mpfi[tf]->f_clave[0].k_ncampos = i + 1;
         }
         else
            if (tc == 2) {
               mpfi[tf]->f_clave[1].k_campo[i - cp] = i + 1;
               mpfi[tf]->f_clave[1].k_orden[i - cp] = ord;
               mpfi[tf]->f_campos[i].c_key = 2;
               mpfi[tf]->f_clave[1].k_ncampos = i - cp + 1;
            }
            pos += lon;
      }
      mpfi[tf]->f_lfich = pos;
      mpfi[tf]->f_buf = malloc(pos+1);
      
      nf = atoi(nomtem+3);
      do {
         sprintf(nomtem+3,"%05d",nf);
         strcpy(mpfi[tf]->f_fichero,dirfic);
         strcat(mpfi[tf]->f_fichero,nomtem);
         i = crea_ind(mpfi[tf]);
         if (iserrno != EEXIST) {break;}
         if (i >= 0) {
            cierra_ind(mpfi[tf]);
            mpfi[tf]->f_fd = -1;
            agichmod(mpfi[tf]->f_fichero,".*","","");
            abre_ind(mpfi[tf],-1);
         }
         nf++;
      } while (i < 0);
      return(0);
   }
   else
      if (m == 1) {
         for (i=0;i<cp;i++) {
            carga_cam(tf,*(p+i),i,0);  /* carga la primaria definida */
         }
         if (leer_reg(tf,ISEQUAL,0,0) < 0)
            nf = 1;
         else
            nf = 0;
         
         if (cal < 0) {
            for (i=cp;i<np;i++) {
               carga_cam(tf,*(p+i),i,0);
            }
         }
         else {
            vca = v_memo(5000)->numerica;
            vcf = v_memo(5011)->numerica;
            v_memo(5000)->numerica = (double)tf;
            v_memo(5011)->numerica = (double)tf;
            p_ejecuta(cal,O_NOR,0,16);
            v_memo(5000)->numerica = vca;
            v_memo(5011)->numerica = vcf;
         }
         
         return(grabar_reg(tf,nf,0));
      }
      else
         if (m == 2 || m == 4 || m == 5) {
            nf = tf - (MAXDEF-1);
            if (m == 4) {
               if (tc == 1) {
                  cierra_ind(mpfi[tf]);
                  campos = (short *)malloc((np + 1)* sizeof(int));
                  for( i = 0;i < (np-cp);i++) 
				  {
                     ord = 0;
                     comoes_cam(*(p+i+cp),(short *)0,(short *)0,(short *)0,(short *)0,(short *)0,&ord);
                     if (ord)
                        ord = -(i + cp + 2);
                     else
                        ord = i + cp;
                     *(campos + i) = ord;
                  }
                  *(campos + i) = -32535;
                  tm[nf] = malloc(51);
                  fd[nf] = _ordena(tf,campos,tm[nf],0,NULL);
                  free((char *)campos);
                  ps[nf] = 0L;
                  if (fd[nf] < 0) return(-1);
               }
               else {
                  tip = ISFIRST;
                  cambia_ind(mpfi[tf],tc);
               }
            }
            else
               if (m == 5) {
                  ps[nf] = 0L;
                  tip = ISFIRST;
               }
               else
                  tip = ISNEXT;
               
               if (tc == 1) 
			   {
                  rw_lseek_((fd[nf] / 100)&~0x40000000,ps[nf],SEEK_SET);
                  if (rw_read_((fd[nf] / 100)&~0x40000000,(char *)&post,sizeof(long)) == 0) return(-1);
				  if ( (fd[nf]&0x40000000) )
				  {
					  lee_directo((fd[nf] % 100) | 0x40000000,post,mpfi[tf],mpfi[tf]->f_buf);
				  }
				  else
				  {
					  lee_directo(fd[nf] % 100,post,mpfi[tf],mpfi[tf]->f_buf);
				  }
                  ps[nf] += sizeof(long);
               }
               else
                  if (leer_reg(tf,tip,0,1) < 0) return(-1);
                  for (i=0;i<np;i++) {
                     carga_cam(tf,*(p+i),i,1);
                  }
                  return(0);
         }
         else
            if (m == 3) 
			{
               nf = tf - (MAXDEF-1);
               if (tc == 1) 
			   {				   
				  if ( (fd[nf]&0x40000000) )
				  {				  
                      cierra_directo(((fd[nf]&~0x40000000) % 100) | 0x40000000);
					  rw_close_((fd[nf]&~0x40000000) / 100);
				  }
				  else
				  {
					  cierra_directo((fd[nf] % 100));
					  rw_close_(fd[nf] / 100);
				  }                  
                  unlink(tm[nf]);
                  free(tm[nf]);
               }
               else
                  cierra_ind(mpfi[tf]);
               borra_ind(mpfi[tf]);
               
               for (i = 0;i < tc;i++) {
                  free((char *)mpfi[tf]->f_clave[i].k_campo);
                  free((char *)mpfi[tf]->f_clave[i].k_orden);
               }
               free((char *)mpfi[tf]->f_campos);
               free((char *)mpfi[tf]->f_clave);
               free((char *)mpfi[tf]->f_buf);
               free((char *)mpfi[tf]);
               tf -= (MAXDEF-1);
               nt[tf] = (short *)0;
               return(0);
            }
            return(-1);
}

lee_relaciones(ipfi)
int ipfi;
{
   int j;
   int i;
   int f;
   int cerrar;
   
   for (j=0;j<v_mpfirel(ipfi)->nrel;j++) {
      i = v_mpfirel(ipfi)->numrela[j];
	  if (i < 0) continue;
      f = v_mpfirel(ipfi)->relacis[j];
      if (f > 99 || v_mpfirel(ipfi)->camminu[j] < 0) continue;
      if (clave_rel(i,f,-1,0,ipfi,-1,DIFERIDO,FALSO) != -1) 
      {
         if (abre1_rel(f) == 0) {
            cerrar = leer_reg(f,ISEQUAL,0,1);
            if (f_v(f,RNOCERRAR) == -1)
               cierra1_rel(f);
            if (cerrar == 0)
               lee_relaciones(f);
            else
               defecto_ficha(v_mpfi(f));
            continue;
         }
      }
      defecto_ficha(v_mpfi(f));
   }
   return(0);
}


int calc_ejecuta(ipfi,modo,calc,tipo,base,z,faclin,m)
short ipfi,modo,*calc,tipo,base,z,faclin,m;
{
   int i;
   int r;
   operaerror = 0;
   while(*calc != -32535 && operaerror == 0) {
      if (((*calc) / base) == tipo) {
         i = (*calc) % base;
         if (m == -1) return(i);
         if (i >= 500) {
            i -= 500;
            r = trata_lineas(ipfi,modo,1,en_pr[programa_activo].tabla_gosubs[i],faclin);
            if (r < 0)
            {
               if (r == -10)
                  operaerror = 10;
               else
                  operaerror = OE_AVISO;
            }
         }
         else       /* z = O_NOR o O_REV */
         {
            p_ejecuta(en_pr[programa_activo].tabla_gosubs[i],z,faclin,17);
         }
         if (m ==  0) return(-1);
      }
      calc++;
			}
   return(-1);
}


/* bucle_index trata un fichero leyendolo secuencialmente dentro
de unos limites marcados y ordenandolo segun unos campos para
lo que se usa un fichero temporal */
static int bucle_index(b,faclin,se_pide_bucle_doble,es_doble_join)
int b,faclin,se_pide_bucle_doble,es_doble_join;
{
	     short ipfi = v_prog(f_B[b].nfi);
         short  n1 = f_B[b].nord1;
         short *o1 = f_B[b].ord1;
         short  n2 = f_B[b].nord2;
         short *o2 = f_B[b].ord2;
         short  nc = f_B[b].ncomp;
         short *co = f_B[b].comp;
         short flag = f_B[b].flag;
         short *calc = f_B[b].calc;
         int i,f,bloq,trato,cerrar,modo,calor,lr,salto,doble,np1,np2,salir = 0,err,key; /* ,lrs; */
         char *buf1 = (char *)0;
         char *buf2 = (char *)0;
         char tempo[51];
         long pos;
         long regis;
         char wtmp[81];
         int res;
         int la_doble = -1,cual = 0,hay_1 = 0,hay_2 = 0,salto_doble = 0;
         char *buf_simple = NULL;
         char *buf_doble = NULL;
         int *_puntero_a_cual = puntero_a_cual;
         int _operaerror = operaerror;
         int hay_contenido = 0;
         char contenido[300];
         /*int l_fd;*/
         int l_con;
		 int ant_factor = factor_secuencia;		 
		 int con_progreso = 0;
         
         
         puntero_a_cual = NULL;		 

         
         /*recurre_mensaje_cabecera(0);*/
         
         s_salida = 0;
         
         if ((flag%10) == 1) 
		 {
            bloq = ISLOCK;
            modo = ALTA;
         }
         else 
            if ((flag%10) == 2) 
			{
               bloq = 0;
               modo = CONSULTA;
               hay_contenido = 1;
               strcpy(contenido,v_memo(5004)->alfabetica);
               strmayus(contenido);
               /*
               strmayus(conte);
               ixx_pon_contenido(v_mpfi(ipfi)->f_fd,conte,strlen(conte));
               */
            }
            else
            {
               bloq = 0;
               modo = CONSULTA;
            }

			 if (v_mpfi(ipfi)->tipo_datos == 5)
			 {
				factor_secuencia = 527;
				if (!max_progreso && !numero_secuencia)
				{
					max_progreso = tam_btrv(v_mpfi(ipfi));
					salto_progreso = max_progreso / 100;
					punto_progreso = 0;
					cur_progreso = 0;
					con_progreso = 1;
					if (DespachadorExterno)
					{
						char tmpp[256];
						sprintf(tmpp,"%ld",max_progreso);
						(*DespachadorExterno)((int)DESPACHO_MULTIPLE,2,tmpp);
					}
				}
			 }
			 else
			 {
				 factor_secuencia = 27;
			 }

            trato = ((flag/10)%10);
            lr =    ((flag/100)%10); /* 0 == siempre 1 == solo doble pasada 2 == nunca*/

          
            np1 =   ((flag/1000)%10);
            np2 =  ((flag/10000)%10);            
            
            calor = calc_ejecuta(ipfi,modo,calc,30,1000,0,0,-1);
            
			if (!con_progreso)
			{
				numero_secuencia++;
				a_cero_num_secuencia();
				mensaje_en_cabecera(__Texto[195],saca_num_secuencia(),0);
			}            
            
            if (n1 < 0) 
            {
               n1 = -(n1);
               salto = v_mpfi(ipfi)->f_clave[n1-1].k_ncampos;
               if (*o1 != F_INVALIDO) {				   
                  buf1 = malloc(v_mpfi(ipfi)->f_lfich+1);
                  buf2 = malloc(v_mpfi(ipfi)->f_lfich+1);
               }
               if ((cerrar = f_v(ipfi,RNOCERRAR)) == -1)
               {
                  if (v_mpfi(ipfi)->f_fd >= 0)
                     cerrar = 1;
                  f_s(ipfi,RNOCERRAR);
               }
               
               
               res = abre1_rel(ipfi);

			   /*
			   {
				   char xbuf[512];
				   unsigned long tiempo;
				   int xi;
				   FI_DEF *xfi;

				   xfi = v_mpfi(ipfi);
				   tiempo = time(NULL);
				   for (xi = 0;xi < 10;xi++)
				   {
					   lee_reg(xfi,ISFIRST);
					   while(lee_reg(xfi,ISNEXT) >= 0);
				   }
				   lee_reg(xfi,ISFIRST);
				   tiempo = time(NULL) - tiempo;
				   sprintf(xbuf,"tiempo=%ld",tiempo);
				   aviso(0,xbuf);
			   }
			   */
               
               if (!res) 
               {
                  if (va_con_doble_ && se_pide_bucle_doble)         
                  {
                     la_doble = abre_la_doble(ipfi,n1);
                     if (la_doble != -1)
                     {
                        buf_simple = malloc(v_mpfi(ipfi)->f_lfich+1);
                        buf_doble = malloc(v_mpfi(ipfi)->f_lfich+1);
                        puntero_a_cual = &cual;
                        if (!buf_doble || !buf_simple)
                        {
                           aviso(0,__Texto[52]);
                           _cierra_ind(la_doble);
                           la_doble = -1;
                        }
                     }
                  }
                  carga_baremo(ipfi,n1,co,buf2);
                  cambia_ind(v_mpfi(ipfi),n1);
                  
                  if (hay_contenido)
                  {      
                     /*l_fd = v_mpfi(ipfi)->f_fd;*/
					 fgv_fin_contenido(v_mpfi(ipfi));
                     l_con = strlen(contenido);
                     fgv_pon_contenido(v_mpfi(ipfi),contenido,l_con);
                  }
                  
                  if (n2 > 0 && fi_tempo(n2,o2,calor,0) < 0);
                  else
                     for (doble = 0;doble < np1 && !salir;doble++) { /* doble pasada */
                        calc_ejecuta(ipfi,modo,calc,1 + doble,1000,O_NOR,faclin,1);
                        carga_baremo(ipfi,n1,o1,buf1);
                        regis = 0L;
                        if (leer_bsecu(ipfi,n1,bloq,trato,buf1,buf2,0,&regis,la_doble,buf_simple,buf_doble,&hay_1,&hay_2,&cual,&salto_doble,es_doble_join,hay_contenido) == 0) 
                        {

                           do { /* aqui en las dos pasadas se hace usa_sel 0 */

#ifdef ESDOS
                              if (timer_especial_int)
                                 (*timer_especial_int)();
#endif
                              if (usa_sel(nc,co,0,salto) == 0) {
                                 if (lr <= doble) lee_relaciones(ipfi);
                                 calc_ejecuta(ipfi,modo,calc,10,1000,O_NOR,faclin,1);
                                 if (operaerror == 0 && usa_sel(nc,co,1 + doble,salto) == 0) {
                                    calc_ejecuta(ipfi,modo,calc,11+doble,1000,O_NOR,faclin,1);
                                    if (operaerror == 0 && doble == 0 && n2 > 0)
                                       fi_tempo(n2,o2,calor,1);
                                    if (operaerror == 10) {
                                       libera_ind(v_mpfi(ipfi));
                                       salir = -2;break;
                                    }
                                 }
                              }
                              if (bloq) {
                                 libera_ind(v_mpfi(ipfi));
                              }
                              
                              if (apli_aborto) {salir = -2;break;}
                              
                              if (bucle_break) 
							  {
                                 if (!(salir = s_salida)) 
								 {
									if (pasa_un_segundo_1())
									{
										if ((salir = pausa_fin()))  break;
									}
                                 }
                                 else
                                    break;
                              }
#ifdef FGVWIN
                              else
							  {
								 if (factor_secuencia < 28 && pasa_un_segundo_1())
								 {
									agi_cola_win();
								 }
							  }
#endif



                           } while (leer_bsecu(ipfi,n1,bloq,trato,buf1,buf2,1,&regis,la_doble,buf_simple,buf_doble,&hay_1,&hay_2,&cual,&salto_doble,es_doble_join,hay_contenido) == 0);


                           calc_ejecuta(ipfi,modo,calc,3 + doble,1000,O_NOR,faclin,1);
                        } /*****/
                     }
                     libera_ind(v_mpfi(ipfi));
                     cambia_ind(v_mpfi(ipfi),1);
               }
			   if (hay_contenido)
			   {
			  	   fgv_fin_contenido(v_mpfi(ipfi));
				   hay_contenido = 0;
			   }

               if (cerrar)
               {
                  f_n(ipfi,RNOCERRAR);
                  if (cerrar == -1)
                     cierra1_rel(ipfi);
               }
               if (buf1 != (char *)0)
                  free(buf1);
               if (buf2 != (char *)0)
                  free(buf2);
   }
   else 
   {
      salto = 0;
      tempo[0] = 0;
      if (n2 > 0 && fi_tempo(n2,o2,calor,0) < 0);
      else /* aqui el usa sel 0 solo se hace una vez */
         if ((i = _ordena(ipfi,o1,tempo,nc,co)) >= 0) 
		 {/*se presupone salto = 0*/
			if ((i&0x40000000))
			{
				i &= ~0x40000000;
				f = i % 100;
				f |= 0x40000000;
				i /= 100;
			}
			else
			{
				f = i % 100;
				i /= 100;
			}
            for (doble = 0;doble < np1 && !salir;doble++) 
			{ /* doble pasada */
               calc_ejecuta(ipfi,modo,calc,1 + doble,1000,O_NOR,faclin,1);
               rw_lseek_(i,0L,SEEK_SET);
               while (rw_read_(i,(char *)&pos,sizeof(long)) > 0) {
                  if (apli_aborto) {salir = -2;break;}
                  
				  
                  if (!progreso_secuencia() && !(suma_num_secuencia() % factor_secuencia) && pasa_un_segundo())
				  {                  
                     mensaje_secuencia();
                  }
				  
                  
                  if (bucle_break) {
                     if (!(salir = s_salida)) 
					 {
						if (pasa_un_segundo_1())
						{
							if ((salir = pausa_fin()))  break;
						}
                     }
                     else
                        break;
                  }
                  
#ifdef ESDOS
                  if (timer_especial_int)
                     (*timer_especial_int)();
#endif
                  
#ifdef FGVWIN
                  else
				  {
					 if (factor_secuencia < 28 && pasa_un_segundo_1())
					 {
						agi_cola_win();
					 }
				  }
#endif                  

                  key = ind_clave_activa(v_mpfi(ipfi));
                  pon_registro_actual(v_mpfi(ipfi),pos);
                  err = -1;	  
				  

                  if (!apunta_ind(v_mpfi(ipfi),0,ISEQUAL,0)) {
                     err = leer_reg(ipfi,ISCURR,bloq,trato);
                  }

                  apunta_ind(v_mpfi(ipfi),key,((!err) ? ISCURR : ISEQUAL),0);
                  
                  if (!err) {
                     if (lr <= doble) lee_relaciones(ipfi);
                     operaerror = 0;
                     calc_ejecuta(ipfi,modo,calc,10+doble*3,1000,O_NOR,faclin,1);

                     if (operaerror == 0 && usa_sel(nc,co,1 + doble,salto) == 0) {
                           
                        calc_ejecuta(ipfi,modo,calc,11+doble,1000,O_NOR,faclin,1);
                        if (operaerror == 0 && doble == 0 && n2 > 0)
                           fi_tempo(n2,o2,calor,1);
                        if (operaerror == 10) {
                           libera_ind(v_mpfi(ipfi));
                           salir = -2;break;
                        }
                     }
                     if (bloq) {
                        libera_ind(v_mpfi(ipfi));
                     }
                  }
               }
               calc_ejecuta(ipfi,modo,calc,3 + doble,1000,O_NOR,faclin,1);
               if (bloq) {
                  libera_ind(v_mpfi(ipfi));
               }
            } /****/
            cierra_directo(f);
            rw_close_(i);
         }/* if de ordena */
         unlink(tempo); 
   }
   
   if (n2 > 0) 
   {
      if ((cerrar = f_v(ipfi,RNOCERRAR)) == -1)
      {
         if (v_mpfi(ipfi)->f_fd >= 0)
            cerrar = 1;
         f_s(ipfi,RNOCERRAR);
      }
      if (abre1_rel(ipfi) == 0) 
      {
         cambia_ind(v_mpfi(ipfi),1);
         for (doble = 0;doble < np2 && !salir;doble++) { /* doble pasada */
            calc_ejecuta(ipfi,modo,calc,5 + doble,1000,O_NOR,faclin,1);
            if (fi_tempo(n2,o2,calor,4 + doble) >= 0)
               do {
                  
				  
                  if (!progreso_secuencia() && !(suma_num_secuencia() % factor_secuencia) && pasa_un_segundo()) 
				  {          
                     mensaje_secuencia();
                  }
				  
                  
                  if (apli_aborto) {salir = -2;break;}
                  
                  if (bucle_break) {
                     if (!(salir = s_salida)) 
					 {
						if (pasa_un_segundo_1())
						{
							if ((salir = pausa_fin()))  break;
						}
                     }
                     else
                        break;
                  }
#ifdef FGVWIN
                  else
				  {
					 if (factor_secuencia < 28 && pasa_un_segundo_1())
					 {
						agi_cola_win();
					 }
				  }
#endif
                  
                  if (leer_reg(ipfi,ISEQUAL,bloq,trato) < 0) {
                     if (iserrno != ELOCKED)
                     {
                        defecto_ficha(v_mpfi(ipfi)); /* break;/*  ATENCION  */
                     }
                     else
                        continue;
                  }
                  if (lr <= doble) lee_relaciones(ipfi);
                  calc_ejecuta(ipfi,modo,calc,20+doble*3,1000,O_NOR,faclin,1);
                  if (operaerror == 0 && usa_sel(nc,co,3 - doble,salto) == 0) {
                     calc_ejecuta(ipfi,modo,calc,21 + doble,1000,O_NOR,faclin,1);
                     if (operaerror == 10) {
                        libera_ind(v_mpfi(ipfi));
                        salir = -2;break;
                     }
                  }
                  if (bloq) {
                     libera_ind(v_mpfi(ipfi));
                  }
                  
               } while (fi_tempo(n2,o2,calor,2) >= 0);
               calc_ejecuta(ipfi,modo,calc,7 + doble,1000,O_NOR,faclin,1);
         } /*****/
         
         fi_tempo(n2,o2,calor,3);
         
         if (hay_contenido)
         {
            fgv_fin_contenido(v_mpfi(ipfi));
            hay_contenido = 0;
         }
         
         if (cerrar) {
            f_n(ipfi,RNOCERRAR);
            if (cerrar == -1)
               cierra1_rel(ipfi);
         }
         
      } /* abre1_rel*/
      
   }
   
   if (la_doble != -1)
   {
      _cierra_ind(la_doble);
      la_doble = -1;
      if (buf_doble)
      {
         free(buf_doble);
         buf_doble = NULL;
      }
      if (buf_simple)
      {
         free(buf_simple);
         buf_simple = NULL;
      }
   }
   
   
   operaerror = _operaerror;
   
   puntero_a_cual = _puntero_a_cual;
   
   if (con_progreso)
   {
		if (DespachadorExterno)
		{
			char tmpp[256];
			sprintf(tmpp,"%ld",max_progreso);
			(*DespachadorExterno)((int)DESPACHO_MULTIPLE,4,tmpp);
		}
		max_progreso = 0;
		salto_progreso = 0;
		punto_progreso = 0;
		cur_progreso = 0;
		con_progreso = 0;
   }
   if (numero_secuencia)
   {
	  numero_secuencia--;
      if (!numero_secuencia)
         borra_mensaje_cabecera(0);
   }
   /*recurre_mensaje_cabecera(1);*/
   factor_secuencia = ant_factor;
   return(0);
}

int rw_bucind(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int r;
   int dd,dj;
   
   if (prog[p].i == 199)
   { 
      dd = 1;
      dj = 1;
   }
   else
      if (prog[p].i == 200)
      { 
         dd = 1;
         dj = 0;
      }
      else
      {
         dd = 0;
         dj = 0;
      }
      
      if (prog[p].indice < 0 || prog[p].indice >= nf_B)
         r = -1;
      else
         r = bucle_index(prog[p].indice,faclin,dd,dj);
      sprintf(v_memo(5004)->alfabetica,"%d",r);
      n_np = 1;
      return(0);
}


/***********************************************************************************/
static int    sl_tot_pfis   = 0;
static FI_DEF *sl_pfi[MAXDEF];
static int    sl_pfi_status[MAXDEF];
static int    *sl_campos[MAXDEF];
static int    sl_ncampos[MAXDEF];
static int    sl_main = -1;
static int    **sl_rel_campos;
/***********************************************************************************/

int saca_seleccion(char *fichero,char *datos,char *ptmp,char *campo)
{
   char dir[256];
   char dat[256];
   char nom_dat[256];
   char tmp[256];
   char *p;
   int i;
   
   dat[0] = 0;
   nom_dat[0] = 0;
   for (i = 0;ptmp[i] && ptmp[i] != '%';i++);
   if (ptmp[i] == '%')
			{
      p = tmp;
      for (i++;ptmp[i] && ptmp[i] != '#' && ptmp[i] != '.';i++,p++) 
         *p = ptmp[i];
      *p = 0;
      while(p > tmp)
      {
         p--;
         if (*p == '%')
         {
            *p = 0;
            strcpy(nom_dat,p+1);
            strcpy(dat,tmp);
            break;
         }
      }
			}
   for (i = 0;dat[i];i++)
			{
      if (dat[i] == '%') dat[i] = '/';
			}
   if (dat[0] == ':')
			{
      if (dat[1] == '/')
      {
         strcpy(tmp,dat+2);
         strcpy(dat,dirbass);
         strcat(dat,tmp);	  
      }
      else
      {
         strcpy(tmp,dir+1);
         strcpy(dat,dirbass);		          
         strcat(dat,tmp);
         strcat(dat,fichdirbarra);
      }
			}
   
   if (quita_path(ptmp) == ptmp)
			{
      strcpy(dir,dirdef);
      if (!dat[0])
      {
         strcpy(dat,dirfic);
      }
      else
      {
         if (dat[0] != '/')
         {
            strcpy(tmp,dat);
            strcpy(dat,dirficb);
            strcat(dat,tmp);
         }
      }
			}
   else
			{
      strcpy(dir,ptmp);
      ptmp = quita_path(ptmp);
      *(quita_path(dir)) = 0;   
      if (dir[0] == ':')
      {
         if (dir[1] == '/')
         {		 
            strcpy(tmp,dir+2);
            strcpy(dir,dirbass);
            strcat(dir,tmp);
            strcpy(dat,dir);
            if (!dat[0])
            {
               strcpy(dat,dir);
            }
            else
            {
               if (dat[0] != '/')
               {
                  strcpy(tmp,dat);
                  strcpy(dat,dir);
                  strcat(dat,tmp);
               }
            }
         }
         else
         {
            strcpy(tmp,dir+1);
            strcpy(dir,dirbass);		          
            strcat(dir,tmp);		 
            if (!dat[0])
            {
               strcpy(dat,dir);
               strcat(dat,fichdirbarra);
            }
            else
            {
               if (dat[0] != '/')
               {
                  strcpy(tmp,dat);
                  strcpy(dat,dir);
                  strcat(dat,fichdirbarra);
                  strcat(dat,tmp);
               }
            }
            strcat(dir,"def/");		 
         }
      }
      else
      {
         if (!dat[0])
         {
            strcpy(dat,dir);
         }
         else
         {
            strcpy(tmp,dat);
            strcpy(dat,dir);
            strcat(dat,tmp);
         }
      }
			}
   strcpy(fichero,dir);
   strcpy(datos,dat);
   
   
   p = fichero + strlen(fichero);
   if (*ptmp == '#')
			{
      ptmp++;
			}
   i = 0;
   for (;*ptmp && *ptmp != '#' && *ptmp != '.' != *ptmp != '%';ptmp++,i++) p[i] = *ptmp;
   p[i] = 0;
   if (!nom_dat[0])
			{
      strcpy(nom_dat,p);
			}
   strcat(datos,nom_dat);
   
   if (*ptmp == '%')
			{
      for (;*ptmp && *ptmp != '#' && *ptmp != '.';ptmp++);   
			}
   if (*ptmp)
			{
      ptmp++;
			}
   strcpy(campo,ptmp);
   return(0);
}

int analiza_campo(pfi,campo)
FI_DEF *pfi;
char *campo;
{
   int i,a,c;
   
   a = 0;
   for (i = 0;campo[i];i++)
   {
      if (campo[i] < '0' || campo[i] > '9')
      {
         a = 1;
         break;
      }
   }
   if (a)
   {
      c = -1;
      for (i = 0;i < pfi->f_ncampos;i++)
      {
         if (pfi->f_campos[i].c_lista[0] != 13)
         {
            if (!strcmp(campo,pfi->f_campos[i].c_nombre))
            {
               c = i;
               break;
            }
         }
         else
         {
            if (!strcmp(campo,pfi->f_campos[i].c_lista+1))
            {
               c = i;
               break;
            }
         }
      }
   }
   else
   {
      c = atoi(campo);
      if (c < 0 || c >= pfi->f_ncampos)
      {
         c = -1;
      }
   }   
   return(c);
}

int select___(char **seleccion,int modo,char *into,char **where,
              char **group,char **having,char **orderby,char **compute,int browse)
{
   int i,j,c,n,r,k;
   char def[256];
   char dat[256];
   char campo[256];
   char *ptmp;
   FI_DEF *pfi;
   
   if (!seleccion) return(-1);
   
   sl_tot_pfis = 0;
   for (i = 0;i < MAXDEF;i++)
      sl_campos[i] = NULL;
   
   /* cargar defs */
   for (i = 0;seleccion[i];i++)
			{
      ptmp = seleccion[i];
      saca_seleccion(def,dat,ptmp,campo);
      for (j = 0;j < sl_tot_pfis;j++)
      {
         if (!strcmp(sl_pfi[j]->f_fichero,dat))
            break;
      }
      if (j >= sl_tot_pfis)
      {
         pfi = NULL;
         r = lee_defn(def,&pfi);
         if (r >= 0)
         {
            sl_pfi[sl_tot_pfis] = pfi;
            sl_pfi_status[sl_tot_pfis] = 1; /* 1 = cargado 2 = referenciado */
            strcpy(pfi->f_fichero,dat);
            sl_ncampos[sl_tot_pfis] = 0;
            j = sl_tot_pfis;
            sl_tot_pfis++;
         }	  
      }
      else
      {
         pfi = sl_pfi[j];
      }
      if (!sl_campos[j])
      {
         sl_campos[j] = (int *)malloc(sizeof(int) * 255);
      }
      /* analizar el campo */
      c = analiza_campo(pfi,campo);
      if (c >= 0)
      {
         for (n = 0;n < sl_ncampos[j];n++)
         {
            if (c == sl_campos[j][n])
            {
               break;
            }
         }
         if (n >= sl_ncampos[j])
         {
            sl_campos[j][ sl_ncampos[j] ] = c;
            sl_ncampos[j]++;
         }
      }
			}
   
   /* analizar campos iguales en diferentes ficheros para establecer enlaces */
   /* primero por sistema antiguo de relaciones */
#ifdef NUEVONUEVO
   sl_nrel_campos = 0;   
   sl_rel_campos = (int **)malloc(sizeof(int **) * 100);
   for (i = 0;i < sl_tot_pfis;i++)
			{
      pfi = sl_pfi[i];
      for (j = 0;j < pfi->f_nrelaci;j++)
      {
         
         for (k = 0;k < MAXCAMPOSPORCLAVE && pfi->f_relaci[j].r_campo[k];k++)
         {
            c = pfi->f_relaci[j].r_campo[k] - 1;
            for (n = 0;n < sl_ncampos[i];n++)
            {
               if (c == sl_campos[i][n])
               {
                  if (sl_nrel_campos && !(sl_nrel_campos%100))
                  {
                     sl_rel_campos = (int **)realloc(sl_rel_campos,sizeof(int **) * (sl_nrel_campos+100));
                  }
                  sl_rel_campos
               }
            }
         }
      }
			}
#endif
   /* luego por codigo de campo */
   
   /* analizar secuencia de lectura */
   
   /* establecer vinculos entre los ficheros usando las relaciones 'basicas' existentes */
   
   
   /*********************************/
   
   for (i = 0;i < MAXDEF;i++)
			{
      if (sl_campos[i])
      {
         free(sl_campos[i]);
         sl_campos[i] = NULL;
      }
			}
   
   /* liberar defs */
   for (i = 0;i < sl_tot_pfis;i++)
			{
      if (sl_pfi_status[i] == 1)
      {
         free_defpri(sl_pfi[i]);
      }
      sl_pfi[i] = NULL;
			}
   sl_tot_pfis = 0;
   return(0);
}

/***********************************************************************************/

/****** fin rutinas lecturas especiales ficheros *******/
#endif

