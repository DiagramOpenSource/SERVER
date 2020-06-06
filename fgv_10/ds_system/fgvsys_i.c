
/**********************************************************************/
/**********************************************************************/
/*                          DIAGRAM9                                  */
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
/*
#ifndef LARGESEEK
extern long rw_lseek_();
#endif
extern unsigned int rw_read_();
*/
#endif


#ifdef ESDOS
#include <dos.h>
#endif
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <errno.h>

#ifndef FGVDOS
#ifdef ESXENIX
#include <sys/ndir.h>
#else
#include <dirent.h>
#endif
#endif

#include <rwmalloc.h>
#include <rtermin.h>
#include <rentra.h>
#define _NUEVODEF
#include <rfecha.h>
#include <rdef.h>       /* ya incluye el isam.h */
#include <rmante.h>
#include <rcalcul.h>
#ifdef FGVIXX
#include "fgvixx.h"
#endif

#include <stdlib.h>
/*extern long atol();*/
#ifdef FGV32
extern BOOL fIsTNT;
#endif

extern int rentra_atrib;
extern int rentra_sal_atrib;
extern int rentra_cc;
extern int rentra_cf;
extern int rentra_sal_cc;
extern int rentra_sal_cf;
extern int redita_atrib;
extern int redita_cc;
extern int redita_cf;
extern long redita_linea;
extern int redita_col;

extern int ixx_control_diu;

extern long _maximo_dsk_;

extern char *rw_usuario;

extern int pendiente_de_instalar;
extern char server_request[128];

extern char rw_version[256];

extern char *lee_linea();

extern int repinta_texto;

extern int (*general_int)();

extern FILE *_imp;
extern int atento_imp; /* control esclava */
extern char _fispool[MAXPATH];
extern char _ejspool[MAXPATH];

extern char *quita_path();
extern double ld_doble();
extern long da_registo_actual();

extern char *act_key;


/* de MOMENTO para pruebas */
extern int default_id;
extern int default_sid;
extern int default_wid;
extern int default_wcid;
extern int default_ctipo;
extern int default_cid;
extern int default_sid_padre;

/****************************/
/*agicont,I,X,6,0,A,0,fecha,Contabilidad Gorda,control1,control2,0,0,a,i+d.alcoy,agicont/fich/agicont.opc,0*/

extern int ultima_instalacion;

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

/********************************************************************
CON MODULO TRACER
********************************************************************/
extern char *logger_salida;
extern int mimodo_debug;

extern int tracer_int;
extern int ntr_br;
extern int tracer_break[];


int pasa_a_int(double);

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
extern int apli_aborto;

extern char no_instala[30];

extern int n_np;

extern int que_version_ciu;

extern int instalaservidor;

extern char *ds_mysql;


/* Buffers para calculos ojo esto esta repetido en otros fuentes */
#define MAXBUFFERS 30
extern char **buff_ers[MAXBUFFERS];
extern unsigned char tip_buf[MAXBUFFERS];


int rw_alarma(p,tip,faclin)
int p;
int tip;
int faclin;
{
/*
int i,j;
int m = 2;
char *par[3];

  for (j = 0;j < 3;j++,p++) {
  i = prog[p].indice;
  par[j] = v_memo(i)->alfabetica;
  }
  if (!strlen(par[2]))
  m = 4;
  sprintf(v_memo(5004)->alfabetica,"%d",alarma(m,par[0],par[1],par[2])); 
   */
   strcpy(v_memo(5004)->alfabetica,"-1");
   n_np = 3;
   return(0);
}


int rw_sialarma(p,tip,faclin)
int p;
int tip;
int faclin;
{
   n_np = 1;
   return(0);
}

extern int recur_punto;

int rw_noop(p,tip,faclin)
int p;
int tip;
int faclin;
{
			char tmp[81];
         /* esta instruccion solo se ejecuta en caso de corrupcion o version anterior */
         /* printf("NOOP");getchar();*/
         sprintf(tmp,"Aviso de Control Interno 42 [%d][%d][%d][%d][%d][%d][%d]",programa_activo,recur_punto,p,tip,faclin,prog[p].i,prog[p].indice);
         aviso(0,tmp);
         /* rw_error(42); */
         return(-1);
}



extern int rw_debug()
{
   if (mimodo_debug)
   {
      tracer_int = 1;
   }
   return(0);
}

int llama_entra(p,qh,resu,m)
int p,qh;
M_VAL *resu;
int m;
{
   char qo[MAXDEFECTO];
   char qs[50];
   char qi[50];
   char qd[MAXDEFECTO];
   int qc = 0;
   
   if (m == 0) {
      sprintf(qd,"%lf",resu->vald);
      quitadec(qd);
   }
   else {
      strcpy(qd,resu->valc);
      qc = atoi(v_memo(5005)->alfabetica);
   }
   if (m == 2) qc = -1;
   strcpy(qs,v_memo(5005)->alfabetica);
   strcpy(qi,v_memo(5006)->alfabetica);
   if (m == 0 && rwatof(qs) < rwatof(qi)) return(0);
   
   sprintf(v_memo(5004)->alfabetica,"%d",entra(p,qc,qi,qs,qd,qh,qo));
   if (m == 0)
      resu->vald = rwatof(qo);
   else
      strcpy(resu->valc,qo);
   
   return(0);
}

int rw_corre(p,tip,faclin)
int p;
int tip;
int faclin; /* de momento no aborta */
{
   int f;
   
   if ((f = memoria_alfa(p)) > -1) {
						quita_blancos(v_memo(f)->alfabetica);
                  if (strlen(v_memo(f)->alfabetica) < 25)
                     strcpy(apli_corre,v_memo(f)->alfabetica);
                  rw_aborta(); /* abandonar el corriente mdr */
   }
   return(0);
}

int rw_menu(p,tip,faclin)
int p;
int tip;
int faclin;
{
   char **opcis,*letras,*texto;
   int i,n,pos,defe,j,m;
   
   m = prog[p].i;
   
   
   i = pasa_a_int(v_memo(prog[p].indice)->numerica);
   if (m == 19) 
   {
      pos    = atoi(v_memo(i)->alfabetica);
      defe   = atoi(v_memo(i+1)->alfabetica);
      texto  = v_memo(i+2)->alfabetica;
      letras = v_memo(i+3)->alfabetica;
      n      = strlen(letras);
   }
   else 
   {
      pos    = atoi(v_memo(i)->alfabetica);
      defe   = atoi(v_memo(i+1)->alfabetica);
      n      = atoi(v_memo(i+2)->alfabetica);
      i--;
   }
   opcis = (char **)malloc(sizeof(char *) * n + 1);
   for (j = 0;j < n;j++)
      opcis[j] = v_memo(i+4+j)->alfabetica;
   if (m == 19)
      sprintf(v_memo(5004)->alfabetica,"%d",menusg(opcis,letras,pos,texto,defe));
   else
   {
      if (m == 255)
      {
         sprintf(v_memo(5004)->alfabetica,"%d",menug_tam(opcis,n,pos,defe,pasa_a_int(v_memo(prog[p+1])->numerica)));
         n_np = 2;
      }
      else
      {
         sprintf(v_memo(5004)->alfabetica,"%d",menug(opcis,n,pos,defe));
      }
   }
   free(opcis);
   
   return(0);
}


int rw_creind(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int ipfi = v_prog(prog[p].indice);
   iserrno = 0;
   crea_ind(v_mpfi(ipfi));
   if (!iserrno) {
      cierra_ind(v_mpfi(prog[p].indice));
      v_mpfi(prog[p].indice)->f_fd = -1;
      agichmod(v_mpfi(prog[p].indice)->f_fichero,".*","","");
      abre_ind(v_mpfi(prog[p].indice),ISINOUT + ISEXCLLOCK);
   }
   sprintf(v_memo(5004)->alfabetica,"%d",iserrno);
   return(0);
}

int rw_borind(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int ipfi = v_prog(prog[p].indice);
   int como = 0;
   
   if (v_mpfi(ipfi)->f_fd >= 0)
   {
      punto_abierto[ipfi] = 0;
      libera_ind(v_mpfi(ipfi));
      f_n(ipfi,RNOCERRAR);
      cierra1_rel(ipfi);		
      como = 10000;
   }
   
   iserrno = 0;
   borra_ind(v_mpfi(ipfi));	 
   sprintf(v_memo(5004)->alfabetica,"%d",iserrno+como);
   return(0);
}

int rw_edita(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int i,ibuf,inib,finb,qpi,qpf,mc_v,c_v;
   char *tit,**txt;
   int maxqc,ipp1,ipp2,pp2,ntx,nn;
   long pp1;
   int noreponer = 0;
   
   ibuf = pasa_a_int(v_memo(prog[p++].indice)->numerica);
   inib = pasa_a_int(v_memo(prog[p++].indice)->numerica);
   finb = pasa_a_int(v_memo(prog[p++].indice)->numerica);
   qpi = pasa_a_int(v_memo(prog[p++].indice)->numerica);
   qpf = pasa_a_int(v_memo(prog[p++].indice)->numerica);
   mc_v = prog[p++].indice;
   c_v = pasa_a_int(v_memo(mc_v)->numerica);
   tit = v_memo(prog[p++].indice)->alfabetica;
   if (!strlen(tit)) tit = NULL;
   maxqc = pasa_a_int(v_memo(prog[p++].indice)->numerica);
   ipp1 = prog[p++].indice;
   ipp2 = prog[p++].indice;
   pp1 = (long)v_memo(ipp1)->numerica;
   pp2 = pasa_a_int(v_memo(ipp2)->numerica);
   ntx  = finb - inib + 1;
   if (!pp1 || !pp2)
   {
      noreponer = 1;
      pp1 = 1L;
      pp2 = 1;
   }
   
   if (ibuf < 0 || ibuf >= MAXBUFFERS || !buff_ers[ibuf] || tip_buf[ibuf] != CHARTIPO) {
      strcpy(v_memo(5004)->alfabetica,"-1");
      n_np = 8;
      return(0);
   }
   i = *((int *)(buff_ers[ibuf]));
   if ((inib+ntx) >= i || inib > finb || inib < 0 || finb < 0) {
      ntx = i - 1;
      inib = 0;
   }
   txt = (char **)(((char *)(buff_ers[ibuf])) + sizeof(int)) + inib;
   
   nn = 0;
   for (i = ntx - 1;i > -1;i--)
   {
      if (!txt[i])
      {
         txt[i] = strmalloc(1);
         txt[i][0] = 0;
      }
   }
   
   sprintf(v_memo(5004)->alfabetica,"%d",edita_texto(&txt,qpi,qpf,-ntx,tit,&c_v,maxqc,&pp1,&pp2));
   v_memo(mc_v)->numerica = (double)c_v;
   if (!noreponer)
   {
      v_memo(ipp1)->numerica = (double)pp1;
      v_memo(ipp2)->numerica = (double)pp2;
   }
   n_np = 10;
   return(0);
}

extern long trata_memo();

int rw_memo(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int i,mo,ppos,qpi,qpf,c_v,inib,finb,maxqc,ibuf,ntx;
   long pos;
   char *fic,*tit,**txt;
   
   mo  = pasa_a_int(v_memo(prog[p++].indice)->numerica);
   fic = v_memo(prog[p++].indice)->alfabetica;
   ppos =  prog[p++].indice;
   pos = (long)v_memo(ppos)->numerica;
   qpi  = pasa_a_int(v_memo(prog[p++].indice)->numerica);
   qpf  = pasa_a_int(v_memo(prog[p++].indice)->numerica);
   c_v  = pasa_a_int(v_memo(prog[p++].indice)->numerica);
   inib = pasa_a_int(v_memo(prog[p++].indice)->numerica);
   finb = pasa_a_int(v_memo(prog[p++].indice)->numerica);
   maxqc = pasa_a_int(v_memo(prog[p++].indice)->numerica);
   ibuf = pasa_a_int(v_memo(prog[p++].indice)->numerica);
   tit = v_memo(prog[p++].indice)->alfabetica;
   ntx  = finb - inib + 1;
   
   
   if (mo != 6) {
      if (mo < 0 || mo > 6 || ibuf < 0 || ibuf >= MAXBUFFERS || !buff_ers[ibuf] || tip_buf[ibuf] != CHARTIPO) {
         v_memo(ppos)->numerica = -1L;
         n_np = 11;
         return(0);
      }
      i = *((int *)(buff_ers[ibuf]));
      if (ntx > i || finb >= i || inib > finb || inib < 0 || finb < 0) {
         ntx = i;
         inib = 0;
      }
      txt = (char **)(((char *)(buff_ers[ibuf])) + sizeof(int)) + inib;
      for (i = 0;i < ntx;i++)
         if (!txt[i]) {
            txt[i] = strmalloc(1);
            txt[i][0] = 0;
         }
   }
   v_memo(ppos)->numerica = (double)trata_memo(fic,pos,100+mo,qpi,qpf,c_v,-ntx,tit,maxqc,&txt,0);
   n_np = 11;
   return(0);
}

static asigna_1(p,pp,_pfi)
int p,pp;
FI_DEF *_pfi;
{
   short i,cc,f0,c0,c;
   char tmp[300];
   
   for (i = 0;i < pp && (i+1) < pp;i += 2) {
      c = pasa_a_int(v_memo(prog[p + i].indice)->numerica);
      if (c >= 0 && c < _pfi->f_ncampos) {
         cc = prog[p + i + 1].indice;
         comoes_cam(cc,NULL,NULL,NULL,&f0,&c0,NULL);
         if (f0 == -1) {
            if (que_memo_tipo(c0) == DOUBLETIPO)
															sprintf(tmp,"%lf",v_memo(c0)->numerica);
            else
															strcpy(tmp,v_memo(c0)->alfabetica);
         }
         else {
            ld_campo(c0,v_mpfi(f0),tmp);
         }
         st_campo(c,_pfi,tmp);
      }
   }
}

extern char *cambia_path_fich();

int ajusta_path_def(d,tmp)
char *d,*tmp;
{
   short c;
   
   if (d == quita_path(d))
   {
      strcpy(tmp,dirbase);
      strcat(tmp,"def/");
      strcat(tmp,d);
   }
   else
   {
      if (*d == ':')
      {
         if (*(d+1) == '/')
         {
            strcpy(tmp,dirbass);
            strcat(tmp,d+2);
         }
         else
         {
            c = *(quita_path(d));
            *(quita_path(d)) = 0;
            strcpy(tmp,dirbass);
            strcat(tmp,d+1);
            *(quita_path(d)) = c;
            strcat(tmp,"def/");
            strcat(tmp,quita_path(d));
         }
      }
      else
         strcpy(tmp,d);
   }
   return(0);
}

int rw_trataendef(p,tip,faclin)
int p;
int tip;
int faclin;
{
   FI_DEF *_pfi;
   short m = prog[p].i - 110;
   char *d = v_memo( prog[p++].indice )->alfabetica;
   char *dr = v_memo( prog[p++].indice )->alfabetica;
   short r,f,i,c,nn;
   short pp = 0;
   short f0,c0,cc;
   int pon_extra = 0;
   char tmp[300];
   while(prog[p + pp].i == 255) {
      pp++; /* contar parametros */
   }
   
   ajusta_path_def(d,tmp);
   
   r = lee_defn(tmp,&_pfi);
   if (r) {
      /** **/
   }
   else {
      if (strlen(dr))
      {
         if (!*(quita_path(dr)))
         {
            strcpy(tmp,dr);
            strcat(tmp,quita_path(_pfi->f_fichero));
            strcpy(_pfi->f_fichero,tmp);
         }
         else
         {
            strcpy(_pfi->f_fichero,dr);
         }
      }
      cambia_path_fich(_pfi->f_fichero);
      defecto_ficha(_pfi);
         if (!pp)
         {
			if (ds_mysql)
				mysql_create_table(_pfi,0);
		 }
#ifdef FGVIXX
      ixx_fuera_cache(_pfi->f_fichero);
      f = abre_ind(_pfi,ISINOUT + ISMANULOCK);
      if (f >= 0 && !pp && !m)
      {
		 respaldo_externo_ixx_prep(_pfi,f);
         ixx_reconstruye(f);
		 respaldo_externo_ixx_prep(NULL,-1);
      }
#else
      f = abre_ind(_pfi,ISINOUT + ISMANULOCK);
#endif
      if (f < 0) {
         if (iserrno == ENOENT) {
            crea_ind(_pfi);
            cierra_ind(_pfi);
            _pfi->f_fd = -1;
            agichmod(_pfi->f_fichero,".*","","");
            f = abre_ind(_pfi,ISINOUT + ISMANULOCK);
         }
      }
      if (f >= 0) {
         /**** *****/
         if (!pp)
         {
            pon_extra = 1;
#ifdef FGVIXX
            nn = 0;
            for (i = 0;i < ixxfichs[f]->nclaves;i ++)
            {
               nn += ixxfichs[f]->claves[i].clave_ncampos;
            }
            sprintf(tmp,"%15ld%15ld%15d%15d%15d%15ld%15d",ixxfichs[f]->ocupacion_total,
               ixxfichs[f]->cabecera.nrecords-ixxfichs[f]->cabecera.nfreerecords,
               ixxfichs[f]->tamreg,ixxfichs[f]->nclaves,ixxfichs[f]->tamblock,
               ixxfichs[f]->cabecera.nblocks,nn);
#endif
         }
         else
         {
            /* hacer asignacion */
            asigna_1(p,pp,_pfi);
            if (m) {
               if (lee_reg(_pfi,ISEQUAL + ISLOCK) < 0)
																		r = -1;
               /* asignacion */
               for (i = 0;i < pp && (i+1) < pp;i += 2) 
			   {
									c = pasa_a_int(v_memo(prog[p + i].indice)->numerica);
                                                      if (c >= 0 && c < _pfi->f_ncampos) {
                                                         ld_campo(c,_pfi,tmp);
                                                         cc = prog[p + i + 1].indice;
                                                         comoes_cam(cc,NULL,NULL,NULL,&f0,&c0,NULL);
                                                         if (f0 == -1) {
                                                            if (que_memo_tipo(c0) == DOUBLETIPO)
                                                               v_memo(c0)->numerica = rwatof(tmp);
                                                            else {
                                                               if (que_memo_tipo(c0) == CHARTIPO) {
                                                                  if (no_memo_interna(c0)) {
                                                                     free(v_memo(c0)->alfabetica);
                                                                     v_memo(c0)->alfabetica = strmalloc(strlen(tmp)+1);
                                                                  }
                                                               }
                                                               else
                                                                  if (que_memo_tipo(c0) == FECHATIPO)
                                                                     tmp[11] = 0;
                                                                  strcpy(v_memo(c0)->alfabetica,tmp);
                                                            }
                                                         }
                                                         else {
                                                            st_campo(c0,v_mpfi(f0),tmp);
                                                         }
                                                      }
               }
            }
            else 
            {               
               if (mimodo_debug || (logger_salida && *logger_salida))
               {
				  if (t_graba_nuevo(_pfi) < 0) 
                  {
                     if (!lee_reg(_pfi,ISEQUAL + ISLOCK)) 
                     {
                        asigna_1(p,pp,_pfi);
                        if (t_graba_actual(_pfi) < 0)
                           r = -1;
                     }
                     else
                        r = -1;
                  }
               }
               else
               {								
				  if (graba_nuevo(_pfi) < 0) 
                  {
                     if (!lee_reg(_pfi,ISEQUAL + ISLOCK)) 
                     {
                        asigna_1(p,pp,_pfi);
                        if (graba_actual(_pfi) < 0)
                           r = -1;
                     }
                     else
                        r = -1;
                  }
               }               
            }            
            /**** ****/
         }
         libera_ind(_pfi);
         cierra_ind(_pfi);
         _pfi->f_fd = -1;
      }
      else
         r = -1;
      free_defpri(_pfi);
   }
   n_np = 2 + pp;
#ifdef FGVIXX
   if (r >= 0 && pon_extra)
   {
      sprintf(v_memo(5004)->alfabetica,"0 %s",tmp);
   }
   else
#endif
      sprintf(v_memo(5004)->alfabetica,"%d",r);
   return(0);
}


int rw_entcamp(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int  f,i,o,b,encl,ol;
   int enkey;
   int wid,id,sid,oqp;
   
   
   i = v_prog(prog[p].indice%100);
   o = prog[p].indice/100;
   b = 0; /* de momento es el numero de pantalla */
   
   enkey = ((v_mpfi(i)->f_campos[o].c_key == 1) ? 1 : 0);
   
   if (pasa_a_int(v_memo(5007)->numerica) > 0) 
       faclin = pasa_a_int(v_memo(5007)->numerica);

   if (v_mpfi(i)->f_campopos[ o ] != -1 && v_mpfi(i)->f_campopos[ o ] == v_mpfi(i)->f_campos[o].c_mante.c_qp)
   {
       f = entra_campo(i,o+1,b,ALTA,faclin,0,enkey,0,-1,ALTA);
       sprintf(v_memo(5004)->alfabetica,"%d",f);  
       return(0);
   }
   
   ol = v_mpfi(i)->f_campos[o].c_mante.c_lineal;
   v_mpfi(i)->f_campos[o].c_mante.c_lineal |= C_LINEAL;

   oqp = v_mpfi(i)->f_campos[o].c_mante.c_qp;

   v_mpfi(i)->f_campos[o].c_mante.c_qp = v_mpfi(i)->f_campopos[ o ];

   if (!faclin)
   {
      faclin = 100;
   }

   wid = v_mpfi(i)->f_pantaids[b];
   id  = v_mpfi(i)->f_id;
   sid = v_mpfi(i)->f_pantasids[b];
   
   v_mpfi(i)->f_pantaids[b] = default_wid;
   v_mpfi(i)->f_id = default_id;
   v_mpfi(i)->f_pantasids[b] = default_sid;

   f = entra_campo(i,o+1,b,ALTA,faclin,0,enkey,0,-1,ALTA);

   cursor(v_mpfi(i)->f_campopos[ o ]);
   printcam_p(v_mpfi(i),(o + 1));

   v_mpfi(i)->f_campos[o].c_mante.c_lineal = ol;

   v_mpfi(i)->f_pantaids[b] = wid;
   v_mpfi(i)->f_id = id;
   v_mpfi(i)->f_pantasids[b] = sid;

   v_mpfi(i)->f_campos[o].c_mante.c_qp = oqp;
   
   sprintf(v_memo(5004)->alfabetica,"%d",f);     
   
   return(0);
}


int rw_entdat(p,tip,faclin)
int p;
int tip;
int faclin;
{
int nfac = pasa_a_int(v_memo(5007)->numerica);
int sinbotones = 0;

   if (!strcmp(v_memo(5004)->alfabetica,"::ENDATOS::SINBOTONES::")) {
	   strcpy(v_memo(5004)->alfabetica,"");
	   sinbotones = 1;
   }

   if (nfac >= 0)
       faclin = nfac;
   else
       faclin = 0;
   if (__entra_datos(prog[p].indice/100,
      v_prog(prog[p].indice%100),faclin,sinbotones) < 0)
      sprintf(v_memo(5004)->alfabetica,"%d",-1);
   else
      sprintf(v_memo(5004)->alfabetica,"%d",0);
   
   return(0);
}

int rw_locind(p,tip,faclin)
int p;
int tip;
int faclin;
{
			int ipfi = v_prog(prog[p].indice);
         iserrno = 0;
         lock_ind(v_mpfi(ipfi));
         sprintf(v_memo(5004)->alfabetica,"%d",iserrno);
         return(0);
}

int rw_ulocind(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int ipfi = v_prog(prog[p].indice);
   iserrno = 0;
   unlock_ind(v_mpfi(ipfi));
   sprintf(v_memo(5004)->alfabetica,"%d",iserrno);
   
   return(0);
}

int rw_letras(p,tip,faclin)
int p;
int tip;
int faclin;
{
   int m;
   double numero;
   char resultado[256];
   int sexo = 0;
   
   m = prog[p].indice;
   if (!no_memo_interna(m)) return(0);
   
   if (que_memo_tipo(m) == CHARTIPO) {
						if (v_memo(m)->alfabetica[0] == 'M') {
                     sexo = 1;
                     numero = rwatof(v_memo(m)->alfabetica+1);
                  }
                  else
                     numero = rwatof(v_memo(m)->alfabetica);
                  free(v_memo(m)->alfabetica);
                  letras(resultado,numero,sexo);
                  v_memo(m)->alfabetica = strmalloc(strlen(resultado)+1);
                  strcpy(v_memo(m)->alfabetica,resultado);
   }
   else
   {
				printf("operacion letras con numerica es ilegal!!!");getchar();
   }
   
   return(0);
}


int rw_impre(p,tip,faclin)
int p;
int tip;
int faclin;
{
   
   sprintf(v_memo(5004)->alfabetica,"%d",pregu_impre(prog[p].indice));
   return(0);
}

int rw_finimp(p,tip,faclin)
int p;
int tip;
int faclin;
{
   char pro[81];
   
   if (memoplin > 9)
      acaba_impresora(v_memo(5009)->alfabetica);
   else
      acaba_impresora(NULL);
   return(0);
}

int rw_lee_texto(p,tip,faclin)
int p;
int tip;
int faclin;
{
   FILE *f;
   int nom,mb,mn,ii,ff;
   char tmp[256];
   int n = 0,b = -1;
   int r = -1;
   
   nom = prog[p++].indice;
   mb  = prog[p++].indice;
   mn  = prog[p++].indice;
   ii  = pasa_a_int(v_memo( prog[p++].indice )->numerica);
   ff  = pasa_a_int(v_memo( prog[p++].indice )->numerica);
   
   if ((f = fopen(v_memo(nom)->alfabetica,"r"))) {
      while(fgets(tmp,255,f)) n++;
      rewind(f);
      b = crea_buffer(n+ii+ff,CHARTIPO);
      n = 0;
      if (b >= 0) {
         while(lee_linea(tmp,255,f)) {
            if (trata_el_buffer(b,n+ii,tmp,strlen(tmp)+1,0,CHARTIPO)) {
               printf("\n\rError trata el buffer.\n\r");
               getchar();
												}
            n++;
         }
         r = n;
      }
      fclose(f);
   }
   
   v_memo(mb)->numerica = (double)b;
   v_memo(mn)->numerica = (double)n;
   sprintf(v_memo(5004)->alfabetica,"%d",r);
   n_np = 5;
   return(0);
}

int rw_graba_texto(p,tip,faclin)
int p;
int tip;
int faclin;
{
   FILE *f;
   char tmp[258];
   int r = -1;
   int nom,b,ii,ff;
   
   nom = prog[p++].indice;
   b   = pasa_a_int(v_memo( prog[p++].indice )->numerica);
   ii   = pasa_a_int(v_memo( prog[p++].indice )->numerica);
   ff   = pasa_a_int(v_memo( prog[p++].indice )->numerica);
   
   
   if ((f = fopen(v_memo(nom)->alfabetica,"w"))) {
      r = 0;
      while((r+ii) < ff && !trata_el_buffer(b,r+ii,tmp,256,2,CHARTIPO)) {
         graba_linea(tmp,f);
         r++;
      }
      fclose(f);
   }
   sprintf(v_memo(5004)->alfabetica,"%d",r);
   n_np = 4;
   return(0);
}

int rw_sleep(p,tip,faclin)
int p;
int tip;
int faclin;
{
   sleep(pasa_a_int(v_memo(prog[p].indice)->numerica));
   return(0);
}


static long saca_la_marca(char *fich,long portiempo)
{
   struct stat buf;
   long la_marca = 0L;
   long posi;
   int file;
   char tmp[256];
   
   file = rw_open_(fich,O_RDONLY | O_BINARY);
   if (file >= 0)
   {
      posi = rw_lseek_(file,0L,SEEK_END);
      rw_lseek_(file,posi-13,SEEK_SET);
      rw_read_(file,tmp,13);
      if (!memcmp(tmp,"011298",6) && !memcmp(tmp+10,"FGV",3))
      {
         memcpy(&la_marca,tmp+6,4);
      }   
      rw_close_(file);
   }
   
   if (stat(fich,&buf) != -1)
   {
      if (!la_marca)
      {
         la_marca = buf.st_mtime;
      }
      else
      {
         if (
            (buf.st_mtime <= (portiempo+62) && buf.st_mtime >= (portiempo-62))
            )
         {
            la_marca = buf.st_mtime;
         }
      }
   }
   
   return(la_marca);
}

static char paquete_por_instalar[128];
int ejecuta_instalacion()
{
   char tmp[1024];
#ifdef WIN32
   char tmp2[300];
   sprintf(tmp,"%s",rw_directorio("master/instalar.exe"));
   sprintf(tmp2,"%s",rw_directorio("master/actualnt.exe"));
   copiarfich(tmp,tmp2,0);
   sprintf(tmp,"%s %s",rw_directorio("master/actual"),paquete_por_instalar);   
#else
   sprintf(tmp,"%s %s",rw_directorio("master/instalar"),paquete_por_instalar);   
#endif
   ejecutae_xxx(tmp,1);
   exit(3);   
}

int controla_paqueteria(int solo_sys)
{
   char tmp[512];
   int i,puede;
   struct info_paquete *info;
   long la_marca = 0L;
   
   if (act_key && !strcmp(act_key,"DEMO")) return(0);
   
   instalacion_silenciosa = 1;

   /* ATENCION POR ACTUALIZAR EL TEMA DEL DS.REG */
   /*
   if ((puede = carga_master(0)) < 0)
   { 
      return(-1);
   }
   */
   
   if (carga_master(0))
   { 
      return(-1);
   }

   puede = 0;
   
   /* verificar los ficheros , los runtime se instalan al momento */
   if (puede)
   {
      
      for (i = 0;i < nPaq;i++)
      {
         if (no_instala[0] && !strcmp(no_instala+2,Paq[i].nompaq))
         {
            sprintf(tmp,"No se pudo actualizar %s, Posibles runtimes en ejecucion",Paq[i].nompaq);
            aviso(0,tmp);
            no_instala[0] = 0;
            continue;
         }
         strcpy(tmp,rw_directorio("master/"));
         strcat(tmp,Paq[i].nompaq);
         strcat(tmp,".tgz");
         la_marca = saca_la_marca(tmp,Paq[i].control_utime);	
         info = NULL;
         if (la_marca)
         {
            if (		    
															(la_marca > (Paq[i].control_utime+62) || la_marca < (Paq[i].control_utime-62))		   
                                             )
            {
															if ((Paq[i].i.tipo == 'I' && !solo_sys) || Paq[i].i.tipo == '!')
                                             {
                                                if (!comprueba_ins_sistema(Paq[i].i.sistema,Paq[i].i.dependesubsistema))
                                                {			  
                                                   info = instala_paquete(Paq[i].nompaq,NULL);
                                                }
                                                else
                                                {
                                                   info = datos_paquete(Paq[i].nompaq,NULL);
                                                }
                                             }
                                             else
                                                if (Paq[i].i.tipo == 'p' && !solo_sys)
                                                {			
                                                   if (!comprueba_ins_sistema(Paq[i].i.sistema,Paq[i].i.dependesubsistema))
                                                   {
                                                      if (Paq[i].control_utime)
                                                         Paq[i].flag = 1;      
                                                      else
                                                         Paq[i].flag = 2;      
                                                   }
                                                }
                                                else
                                                   if (Paq[i].i.tipo == 'P' && !solo_sys)
                                                   {
                                                      if (!comprueba_ins_sistema(Paq[i].i.sistema,Paq[i].i.dependesubsistema))
                                                      {
                                                         if (Paq[i].control_utime)
                                                            Paq[i].flag = 1;      
                                                         else
                                                            Paq[i].flag = 2;      		      
                                                      } 
                                                   } 
                                                   else
                                                      if (Paq[i].i.tipo == 'C' || (Paq[i].i.tipo == 'S' && instalaservidor))
                                                      {
                                                         if (!strcmp("X",Paq[i].i.sistema))
                                                         {
                                                            info = instala_paquete(Paq[i].nompaq,NULL);
                                                         }
                                                         else
                                                            if (!comprueba_ins_sistema(Paq[i].i.sistema,Paq[i].i.dependesubsistema) && puede == 1)
                                                            {
                                                               Paq[i].flag = 1;		  
                                                               if (solo_sys)
                                                               {		
                                                                  int salir = 0;		 
#ifdef FGVDOS
#ifdef FGVWIN
                                                                  if (Paq[i].i.tipo == 'C')
                                                                     salir = 1;
#else
                                                                  if (Paq[i].i.tipo == 'S')
                                                                     salir = 1;
#endif
#else
                                                                  if (Paq[i].i.tipo == 'S')
                                                                     salir = 1;
#endif				 
                                                                  if (salir)
                                                                  {                    
                                                                     char tmpxx[81];
                                                                     
                                                                     if (server_request[0] == '@' && server_request[2] == 'S' && server_request[3] == ':')
                                                                     {
                                                                        strcpy(paquete_por_instalar,Paq[i].nompaq);
                                                                        pendiente_de_instalar = 1;
                                                                        descarga_master();
                                                                        return(nPaq);
                                                                     }
                                                                     
                                                                     info = datos_paquete(Paq[i].nompaq,NULL);
                                                                     if (info)
                                                                     {
                                                                        Paq[i].i = *info;
                                                                     }
                                                                     sprintf(tmpxx,"Actualizar %s -%s- (S/N)",Paq[i].i.descrip,Paq[i].nompaq);
                                                                     if (!confirmar(2400,tmpxx,"S"))
                                                                     {
                                                                        pendiente_de_instalar = 1;
                                                                        strcpy(paquete_por_instalar,Paq[i].nompaq);
                                                                        ejecuta_instalacion();
                                                                        return(-10);
                                                                     }
                                                                  }
                                                               }			     
                                                            }
                                                            else
                                                            {
                                                               info = datos_paquete(Paq[i].nompaq,NULL);
                                                               if (comprueba_ins_sistema(Paq[i].i.sistema,Paq[i].i.dependesubsistema))
                                                               {
                                                                  ultima_instalacion = 1;
                                                               }
                                                            }
                                                      }
                                                      if (info)
                                                      {
                                                         if (Paq[i].control_utime && Paq[i].i.version1 == info->version1 && Paq[i].i.version2 == info->version2 && 
                                                            Paq[i].i.versionletra == info->versionletra)
                                                         {
                                                            info->autoversion = Paq[i].i.autoversion+1;
                                                         }
                                                         memcpy(&(Paq[i].i),info,sizeof(Paq[i].i));
                                                         if (ultima_instalacion == 1)
                                                         {
                                                            if (!(info->build_numero))
                                                               Paq[i].control_utime = la_marca;
                                                            else
                                                            {
                                                               Paq[i].control_utime = info->build_numero;
                                                               pon_la_marca(tmp,info->build_numero);
                                                            }
                                                            
                                                            Paq[i].flag = 3; /* instalado */
                                                         }
                                                         else
                                                         {
                                                            Paq[i].control_utime = 0L;
                                                            Paq[i].flag = 2; /* instalado */
                                                         }		    
                                                      }
        }
        else
        {
           if (strcmp(Paq[i].nompaq,"agitool") && strcmp(Paq[i].nompaq,"dstool") && strcmp(Paq[i].nompaq,"dsimpre") && (Paq[i].i.tipo == 'P' || Paq[i].i.tipo == 'I' || Paq[i].i.tipo == 'p'))
           {
              strcpy(tmp,rw_directorio("master/"));
              strcat(tmp,Paq[i].nompaq);
              strcat(tmp,".tgz");
              unlink(tmp);
           }
        }
     }
     else
        Paq[i].flag = -1;
   }
   
   if (puede) compon_agi_menu();
  }
  
  descarga_master();
  instalacion_silenciosa = 0;
  /*****************************/
  
  return(nPaq);
}

static char C_agigrafico[128] = {
	/*CODIGO INTERNO*/ /* Gráfico */
	/* 128*/                  0,
	/* 129*/                  0,
	/* 130*/                  0,
	/* 131*/                  0,
	/* 132*/                  0,
	/* 133*/                  0,
	/* 134*/                  0,
	/* 135*/                  0,
	/* 136*/                  0,
	/* 137*/                  0,
	/* 138*/                  0,
	/* 139*/                  0,
	/* 140*/                  0,
	/* 141*/                  0,
	/* 142*/                  0,
	/* 143*/                  0,
	/* 144*/                  0,
	/* 145*/                  0,
	/* 146*/                  0,
	/* 147*/                  0,
	/* 148*/                  0,
	/* 149*/                  0,
	/* 150*/                  0,
	/* 151*/                  0,
	/* 152*/                  0,
	/* 153*/                  0,
	/* 154*/                  0,
	/* 155*/                  0,
	/* 156*/                  0,
	/* 157*/                  0,
	/* 158*/                  0,
	/* 159*/                  0,
	/* 160*/                  0,
	/* 161*/                  0,
	/* 162*/                  0,
	/* 163*/                  0,
	/* 164*/                  0,
	/* 165*/                  0,
	/* 166*/                  0,
	/* 167*/                  0,
	/* 168*/                  0,
	/* 169*/                  0,
	/* 170*/                  0,
	/* 171*/                  0,
	/* 172*/                  0,
	/* 173*/                  0,
	/* 174*/                  0,
	/* 175*/                  0,
	/* 176*/                  1,
	/* 177*/                  1,
	/* 178*/                  1,
	/* 179*/                  3,
	/* 180*/                  2,
	/* 181*/                  2,
	/* 182*/                  2,
	/* 183*/                  2,
	/* 184*/                  2,
	/* 185*/                  2,
	/* 186*/                  3,
	/* 187*/                  2,
	/* 188*/                  2,
	/* 189*/                  2,
	/* 190*/                  2,
	/* 191*/                  2,
	/* 192*/                  2,
	/* 193*/                  2,
	/* 194*/                  2,
	/* 195*/                  2,
	/* 196*/                  3,
	/* 197*/                  1,
	/* 198*/                  2,
	/* 199*/                  2,
	/* 200*/                  2,
	/* 201*/                  2,
	/* 202*/                  2,
	/* 203*/                  2,
	/* 204*/                  2,
	/* 205*/                  3,
	/* 206*/                  1,
	/* 207*/                  2,
	/* 208*/                  2,
	/* 209*/                  2,
	/* 210*/                  2,
	/* 211*/                  2,
	/* 212*/                  2,
	/* 213*/                  2,
	/* 214*/                  2,
	/* 215*/                  1,
	/* 216*/                  1,
	/* 217*/                  2,
	/* 218*/                  2,
	/* 219*/                  1,
	/* 220*/                  1,
	/* 221*/                  1,
	/* 222*/                  1,
	/* 223*/                  1,
	/* 224*/                  0,
	/* 225*/                  0,
	/* 226*/                  0,
/* 227*/                  0,
/* 228*/                  0,
/* 229*/                  0,
/* 230*/                  0,
/* 231*/                  0,
/* 232*/                  0,
/* 233*/                  0,
/* 234*/                  0,
/* 235*/                  0,
/* 236*/                  0,
/* 237*/                  0,
/* 238*/                  0,
/* 239*/                  0,
/* 240*/                  0,
/* 241*/                  0,
/* 242*/                  0,
/* 243*/                  0,
/* 244*/                  0,
/* 245*/                  0,
/* 246*/                  0,
/* 247*/                  0,
/* 248*/                  0,
/* 249*/                  0,
/* 250*/                  0,
/* 251*/                  0,
/* 252*/                  0,
/* 253*/                  0,
/* 254*/                  0,
/* 255*/                  0
	 };

static int MirameLosChinos(unsigned char *databuf,int l_reg,FgvStdTabla *itabla)
{
	int i,len,j;
	int chinos = 0;
	char tmp[15];
	
	for (i = 0;i < itabla->ncampos;i++)
	{
		len = itabla->Campo[i].len;
		if (itabla->Campo[i].tipo == CHARTIPO+48)
		{
			for (j = 0;j < len;j++)
			{
				/*
				if (databuf[j] >= 128 && C_agigrafico[databuf[j]-128])
				{
					chinos++;
					break;
				}
				else */if (databuf[j] && databuf[j] < 32)
				{
					chinos++;					
				}
			}
		}
		else if (itabla->Campo[i].tipo == FECHATIPO+48)
		{
			for (j = 0;j < 8;j++)
			{
				if (databuf[j] && databuf[j] != 32 && databuf[j] != '.' && databuf[j] != '/' && (databuf[j] < '0' || databuf[j] > '9'))
				{
					chinos++;
				}
			}
			/*
			ldfecha(databuf,tmp);			
			if (bienfecha(tmp) < 0)
			{
				chinos++;
			}
			*/
		}
		databuf += len;
	}
	return chinos;
}

int CreaPaqueteCiu(char *_queciu,char *_paquete)
{
    int _ixx_haydiu = ixx_haydiu;
    char *_ixx_ciudiu = ixx_ciudiu;    
    int fd_diu;
    int fd_paq;
    int fd;
    int rr = -1,i,g;    
    long hora;
    long ficheros,aficheros;
    FI_DEF *fi;
    unsigned char registro[112];
	unsigned char xregistro[112];
    unsigned char *dato,*p;
    long reg,tam;
    long nreg,nnreg;
    long preg;
    long modo_diu;
    char tmp[256];
	char tmp3[81];
	char tmpdiu[256];
    char queciu[256];
	char filtrar[256];
    char *registros = NULL;
    char usuario[20];
    unsigned long numero_secuencial,tmp1;
	long version_ciu = 1;
	long pos_seek;
	int procesados;
	int pasados;
	char buffer_vacio[256];
	char paquete[256];
	long pos_nf;	
	long tamactual;
	long totreg;
	int noborradiu;	
	int extra_diu = 0;
	long cero = 0;
	long paqpreg2,paqpreg1;
	long xxlreg;
	long completo = 0;
	long escopia = 0;
	long tamtot1,tamtot2;
	long pos_registro1,pos_registro2;
	char c = 0;
	static int grabartiempo = 0;
	unsigned char md5out[16];
	long l_reg;	
	VENT *w = NULL;
	FgvStdTabla *dtabla = NULL;
	char **descamp = NULL;
	char *defectos = NULL;							
	static long ciupreg1 = 0L;
	static int naborrar = 0;
	static char **aborrar = NULL;
	static int fd_ciu = -1;
	static int nlosprocesados = 0;
	static char *losprocesados = NULL;
	static long tiempo;
	static long control_ciu;    
	static long id;    
	static int npaquetes = 0;
	static char *paquetes[512];
	static int hay_errores = 0;
	

	if (ixx_control_diu > 1)
	{
		version_ciu = 2; /* no vamos a hacer caso de que_version_ciu, la minima es la 2!!*/
	}
	if (que_version_ciu > 2)
		version_ciu = 3; /* maxima soportada por este básico */

	memset(buffer_vacio,0,sizeof(buffer_vacio));

    hora = time(NULL);
    
	filtrar[0] = 0;	
    
	if (*_paquete == '!')
	{
		version_ciu = 3; /* obligado! */
		completo = 1;
		_paquete++;
	}
    strcpy(paquete,_paquete);
	for (i = 0;paquete[i];i++)
	{
		if (paquete[i] == ';')
		{
			paquete[i] = 0;
			strcpy(filtrar,paquete+i+1);
			break;
		}
	}

	strcpy(queciu,_queciu);
    strcat(queciu,".ciu");

    ixx_ciudiu = NULL; /* no importa si queciu != ixx_ciudiu */
    ixx_haydiu = 0;	

	if (fd_ciu < 0)
	{
		hay_errores = 0;
		grabartiempo = 0;
		fd_ciu = rw_base_open_(queciu,O_RDWR | O_BINARY);
		if (fd_ciu >= 0)
		{
			if (rw_base_read_(fd_ciu,&id,sizeof(long)) != sizeof(long) || id != FGV_ID_CIU)
			{
				rw_base_close_(fd_ciu);
				if (registros)
					free(registros);
				ixx_ciudiu = _ixx_ciudiu;
				ixx_haydiu = _ixx_haydiu;
				return(-1);
			}

			while(-1)
			{
				rw_base_lseek_(fd_ciu,(long)sizeof(long),SEEK_SET);
				if (rw_base_read_(fd_ciu,&control_ciu,sizeof(long)) == sizeof(long))
				{
					rw_base_lseek_(fd_ciu,(long)sizeof(long)*2,SEEK_SET);
					if (rw_base_read_(fd_ciu,&tiempo,sizeof(long)) != sizeof(long))
					{
						rw_base_close_(fd_ciu);
						if (registros)
							free(registros);
						ixx_ciudiu = _ixx_ciudiu;
						ixx_haydiu = _ixx_haydiu;                    
						aviso(0,"Ya hay un proceso de replica en curso");
						return(-1);
					}
					rw_base_lseek_(fd_ciu,(long)sizeof(long)*2,SEEK_SET);
					if (!rw_base_locking(fd_ciu,LK_WAIT,1)) break;
					if (errno != EACCES && errno != EDEADLOCK && errno != EAGAIN  && errno != 13 && errno != 35)
					{
						rw_base_close_(fd_ciu);
						if (registros)
							free(registros);
						ixx_ciudiu = _ixx_ciudiu;
						ixx_haydiu = _ixx_haydiu;
						return(-1);
					}
				}
				if (ixxdat_micro_espera)
				   (*ixxdat_micro_espera)();
			}
			sleep(5);
		}
	}
	if (fd_ciu >= 0)
	{
        rw_base_lseek_(fd_ciu,(long)(sizeof(long)*3),SEEK_SET);
        rw_base_read_(fd_ciu,&ficheros,sizeof(long));
        rw_base_lseek_(fd_ciu,(long)(sizeof(long)*4),SEEK_CUR);		
		aficheros = ficheros;

		if (ficheros)
		{
			if (aborrar && naborrar != ficheros)
			{
				if (aborrar)
				{
					int xxi,xxfd_diu;
					for (xxi = 0;xxi < naborrar;xxi++)
					{
						if (aborrar[xxi])
						{
							if ((xxfd_diu = rw_base_open_(aborrar[xxi],O_RDWR | O_BINARY | O_TRUNC,S_IREAD | S_IWRITE)) >= 0)
							   rw_base_close_(xxfd_diu);
							rw_base_unlink(aborrar[xxi]+256);
							free(aborrar[xxi]);
						}
					}
					free((char *)aborrar);
					aborrar = NULL;
					naborrar = 0;
				}				
			}

			if (!aborrar)
			{
				aborrar = (char **)malloc(sizeof(char *)*ficheros);
				memset((char *)aborrar,0,sizeof(char *)*ficheros);
				naborrar = ficheros;
			}
		}
		else
		{  /* nada que hacer */

			while(npaquetes > 0)
			{
				npaquetes--;
				if (!hay_errores)
				{
					strcpy(paquete,paquetes[npaquetes]);
					strcat(paquete,".tmp");
					rw_base_unlink(paquetes[npaquetes]);
					rw_base_rename(paquete,paquetes[npaquetes]); 					
				}
				free(paquetes[npaquetes]);
			}			
			if (aborrar)
			{
				int xxi,xxfd_diu;
				for (xxi = 0;xxi < naborrar;xxi++)
				{
					if (aborrar[xxi])
					{
						if (!hay_errores)
						{
							if ((xxfd_diu = rw_base_open_(aborrar[xxi],O_RDWR | O_BINARY | O_TRUNC,S_IREAD | S_IWRITE)) >= 0)
							   rw_base_close_(xxfd_diu);
							rw_base_unlink(aborrar[xxi]+256);
						}
						free(aborrar[xxi]);
					}
				}
				free((char *)aborrar);
				aborrar = NULL;
				naborrar = 0;
			}
			procesados = 0;
			if (!hay_errores && losprocesados && ciupreg1)
			{
				int xxi;
				rw_base_lseek_(fd_ciu,ciupreg1,SEEK_SET);
				for (xxi = 0;xxi < nlosprocesados;xxi++)
				{
					if (losprocesados[xxi] == 1)
					{
						rw_base_write_(fd_ciu,buffer_vacio,112);
						procesados++;
					}
					else
					{
						rw_base_lseek_(fd_ciu,112,SEEK_CUR);
					}
				}			
			}
			if (!hay_errores && grabartiempo)
			{
				rw_base_lseek_(fd_ciu,(long)sizeof(long),SEEK_SET);
				moto_long(&control_ciu);
				rw_base_write_(fd_ciu,&control_ciu,sizeof(long));
				moto_long(&control_ciu);			
				if (nlosprocesados && procesados == nlosprocesados)
				{
					ficheros = 0L;
					rw_base_lseek_(fd_ciu,(long)sizeof(long)*3,SEEK_SET);
					moto_long(&ficheros);
					rw_base_write_(fd_ciu,&ficheros,sizeof(long));
					moto_long(&ficheros);				
				}
			}
			rw_base_lseek_(fd_ciu,(long)sizeof(long)*2,SEEK_SET);
			rw_base_locking(fd_ciu,LK_UNLCK,1);
			if (!hay_errores && grabartiempo)
			{
				grabartiempo = 0;
				tiempo = time(NULL);
				moto_long(&tiempo);
				rw_base_lseek_(fd_ciu,(long)sizeof(long)*2,SEEK_SET);
				rw_base_write_(fd_ciu,&tiempo,sizeof(long));
				moto_long(&tiempo);
			}
            rw_base_close_(fd_ciu);
			fd_ciu = -1;
			if (registros)
				free(registros);			
			if (losprocesados)
			{
				free((char *)losprocesados);
				losprocesados = NULL;
			}
			nlosprocesados = 0;
			ixx_ciudiu = _ixx_ciudiu;
			ixx_haydiu = _ixx_haydiu;
			if (hay_errores)
			{
				return 2;
			}			
            return(1);
		}

		tamactual = 0L;
		paquetes[npaquetes] = malloc(strlen(paquete)+1);
		strcpy(paquetes[npaquetes],paquete);
		npaquetes++;
		strcat(paquete,".tmp");
        fd_paq = rw_open_(paquete,O_RDWR | O_BINARY | O_CREAT | O_TRUNC,S_IREAD | S_IWRITE);
        if (fd_paq >= 0)
        {
            rr = 0;

            w = w_ini(610,1970);
            w_salva(w);
            zonablan(610,1970);
            cuadro(610,1970);
            sprintf(tmp3,"Paquete: %-48.48s",paquete);			
            pprint(812,tmp3);

			/* Nueva Version */
            /*id = FGV_ID_CIU_PAQ;*/
			id = FGV_ID_CIU2_PAQ;
			moto_long(&id);
            tamactual += rw_write_(fd_paq,&id,sizeof(long));
			
			moto_long(&version_ciu);
			tamactual += rw_write_(fd_paq,&version_ciu,sizeof(long));
			moto_long(&version_ciu);

			moto_long(&control_ciu);
            tamactual += rw_write_(fd_paq,&control_ciu,sizeof(long));
			moto_long(&control_ciu);
			moto_long(&tiempo);
            tamactual += rw_write_(fd_paq,&tiempo,sizeof(long));			
            tiempo = time(NULL);
			moto_long(&tiempo);
            tamactual += rw_write_(fd_paq,&tiempo,sizeof(long));            
			moto_long(&tiempo);
			moto_long(&ficheros);
			pos_nf = rw_lseek_(fd_paq,0L,SEEK_CUR);
            tamactual += rw_write_(fd_paq,&ficheros,sizeof(long));
			moto_long(&ficheros);


            control_ciu++;

            /*{PathDat(52)PathDef(52)regsize(4)flags(4)}*/			
			pasados = 0;

			if (!losprocesados)
			{
				nlosprocesados = ficheros;
				losprocesados  = malloc(nlosprocesados);
				memset(losprocesados,0,nlosprocesados);
			}
			ciupreg1 = rw_base_lseek_(fd_ciu,0L,SEEK_CUR);
            for(i = 0;i < ficheros;i++)
            {				
				escopia = 0;				
                rw_base_read_(fd_ciu,registro,112);

				if (registro[0] && tamactual >= _maximo_dsk_)
				{
					continue;
				}
				if (registro[0] && filtrar[0])
				{
					if (strbusca(registro,filtrar) < 0)
					{
						continue;
					}
				}

				if (!registro[0] || losprocesados[i] == 1) 
				{
					losprocesados[i] = 1; /* regulariza */
					continue;
				}

                sprintf(tmp3,"Fichero: %-48.48s",registro);
                pprint(1012,tmp3);
                sprintf(tmp3,"Registro:%-48.48s","");
				pprint(1212,tmp3);


                fi = NULL;				
				sprintf(tmpdiu,"%s.diu",registro);
                sprintf(tmp,"%s.xiu",registro);
				if (losprocesados[i] != 2)
				{
					copiarfich(tmpdiu,tmp,0);
				}
				if (completo)
				{
					fd_diu = -1;
					escopia = 1;
				}
				else
				{
					fd_diu = -1;
					escopia = 0;
					if (*quita_path(registro+52) != 'x')
					{
						fd_diu = rw_base_open_(tmp,O_RDWR | O_BINARY);										
						if (fd_diu > -1 && rw_base_read_(fd_diu,&id,sizeof(long)) != sizeof(long) && id != FGV_ID_DIU)
						{
							rw_base_close_(fd_diu);
							rw_base_unlink(tmp);
							fd_diu = -1;
							if (version_ciu > 2)
							{
								escopia = 1; /* mas vale!!! */
							}
						}
					}
					else
					{
						escopia = 0;
					}
					if (!escopia)
					{						
						if (fd_diu > -1)
						{
                            tam = rw_base_lseek_(fd_diu,0L,SEEK_END) - (sizeof(long)*4);
							rw_base_lseek_(fd_diu,(long)(sizeof(long)*3),SEEK_SET);
							rw_base_read_(fd_diu,&modo_diu,sizeof(long));
							if (tam < 5)
							{
								rw_base_close_(fd_diu);
								fd_diu = -1;								
							}
						}

						if (fd_diu < 0)
						{
							losprocesados[i] = 1;
							if (aborrar)
							{
								if (aborrar[i])
									free(aborrar[i]);
								aborrar[i] = malloc(512);
								strcpy(aborrar[i],tmpdiu);
								strcpy(aborrar[i]+256,tmp);
							}
							else
							{				
								/* no borramos el diu real ... */
								if ((fd_diu = rw_base_open_(tmp,O_RDWR | O_BINARY | O_TRUNC,S_IREAD | S_IWRITE)) >= 0)
								   rw_base_close_(fd_diu);
							}
							continue;
						}
					}					
				}			

				pasados++;

				moto_long(registro+104);
				moto_long(registro+108);
				pos_registro1 = rw_lseek_(fd_paq,0L,SEEK_CUR);
                tamactual += rw_write_(fd_paq,registro,112);
				moto_long(registro+104);
				moto_long(registro+108);
                preg = rw_lseek_(fd_paq,0L,SEEK_CUR);
                nreg = 0;
				nnreg = 0;
				totreg = nreg;
				moto_long(&nreg);
                tamactual += rw_write_(fd_paq,&nreg,sizeof(long));
				moto_long(&nreg);


                numero_secuencial = 0L;

                if (lee_defn(registro+52,&fi))
				{
					/* error */
					if (version_ciu > 0)
					{
						long tmpl = rw_lseek_(fd_paq,0L,SEEK_CUR) + sizeof(long); 
						rw_write_(fd_paq,&tmpl,sizeof(long));
					}
				}
				else
                {
					
					memcpy(&l_reg,registro+104,sizeof(long));
					
					if (l_reg != fi->f_lfich)
					{
						pos_registro2 = rw_lseek_(fd_paq,0L,SEEK_CUR);
						rw_lseek_(fd_paq,pos_registro1,SEEK_SET);
						l_reg = fi->f_lfich;
						memcpy(registro+104,&l_reg,sizeof(long));
						rw_write_(fd_paq,registro,112);
						rw_lseek_(fd_paq,pos_registro2,SEEK_SET);
					}					
					

                    strcpy(fi->f_fichero,registro);
                    fd = abre_ind(fi,ISINOUT + ISMANULOCK);
                    if (fd < 0)
					{
						/* error */
						if (version_ciu > 0)
						{
							long tmpl = rw_lseek_(fd_paq,0L,SEEK_CUR) + sizeof(long); 
							rw_write_(fd_paq,&tmpl,sizeof(long));
						}
					}
					else
                    {
						if (version_ciu > 0)
						{
							long tmpl = rw_lseek_(fd_paq,0L,SEEK_CUR); 
							pos_seek = 0;

							descamp = NULL;
							defectos = NULL;
							dtabla = NULL;
							

							descamp = descrip_camps(fi,&defectos,&dtabla);
							tamactual += rw_write_(fd_paq,&pos_seek,sizeof(long));
							
							tamactual += rw_write_(fd_paq,dtabla,sizeof(FgvStdTabla));							
							tamactual += rw_write_(fd_paq,dtabla->Campo,sizeof(CampoTabla)*dtabla->ncampos);

							if (version_ciu > 1)
							{
					            paqpreg1 = rw_lseek_(fd_paq,0L,SEEK_CUR);
				                rw_write_(fd_paq,&cero,sizeof(long));
							}
							if (version_ciu > 2)
							{
								rw_write_(fd_paq,&escopia,sizeof(long));								
								rw_write_(fd_paq,md5out,16);
							}
							
							if (dtabla)
							{
								int jodido = 0;					
								int j;
								if (dtabla->ncampos < 1 || dtabla->ncampos > 255)
								{						
									jodido = 1;
								}
								if (!jodido)
								{									
									for (j = 0;j < dtabla->ncampos;j++)
									{
										jodido += dtabla->Campo[j].len;
									}
									if (jodido != dtabla->reclen)
									{
										jodido = 1;
									}
									else
										jodido = 0;
								}
								if (jodido)
								{
									hay_errores++;
								}
							}
							
							pos_seek = rw_lseek_(fd_paq,0L,SEEK_CUR); 
							rw_lseek_(fd_paq,tmpl,SEEK_SET);
							rw_write_(fd_paq,&pos_seek,sizeof(long));
							rw_lseek_(fd_paq,pos_seek,SEEK_SET);
						}

                        fi->f_fd = fd;                        
                        apunta_ind(fi,1,ISFIRST,0);
                        apunta_ind(fi,0,ISCURR,0);

						if (fd_diu < 0)
						{
							if (escopia)
							{
								tam = ixxfichs[fi->f_fd]->cabecera.nrecords;
							}
							else
							{
								tam = 0;
							}
						}

                        if (registros)
                           free(registros);
						registros = NULL;
						if (tam > 0)
						{
							registros = malloc(tam);
							/*
							if (modo_diu == 1)
							{
								modo_diu = fi->f_lfich;
							}
							else
							{
								modo_diu = 0;
							}
							*/

							if (escopia || rw_base_read_(fd_diu,registros,tam) == tam)
							{
								if (!escopia && modo_diu)
								{
									if (version_ciu > 1)
									{
										paqpreg2 = rw_lseek_(fd_paq,0L,SEEK_CUR);
										rw_lseek_(fd_paq,paqpreg1,SEEK_SET);
										rw_write_(fd_paq,&modo_diu,sizeof(long));
										if (version_ciu > 2)
										{ /* SE ANULA EL MODO COPIA !!! (innecesario ...)*/
											rw_write_(fd_paq,&cero,sizeof(long));
										}
										rw_lseek_(fd_paq,paqpreg2,SEEK_SET);
									}
									if (modo_diu > 1)
									{
										extra_diu = 8;
									}
									else
									{
										extra_diu = 0;
									}
									nreg = tam / (fi->f_lfich+5+(sizeof(long)*2)+10+extra_diu);
									totreg = nreg;
	#ifdef ESMOTOROLA
									{
										long nn;
										long tt = (fi->f_lfich+5+(sizeof(long)*2)+10+extra_diu);
										dato = registros;
										for (nn = 0L;nn < nreg;nn++)
										{
											moto_long(dato+1);
											moto_long(dato+fi->f_lfich+5);
											moto_long(dato+dato+fi->f_lfich+19);
											dato += tt;
										}
									}
	#endif
									tamactual += rw_write_(fd_paq,registros,tam);
	#ifdef ESMOTOROLA
									{
										long nn;
										long tt = (fi->f_lfich+5+(sizeof(long)*2)+10+extra_diu);
										dato = registros;
										for (nn = 0L;nn < nreg;nn++)
										{
											moto_long(dato+1);
											moto_long(dato+fi->f_lfich+5);
											moto_long(dato+dato+fi->f_lfich+19);
											dato += tt;
										}
									}
	#endif
								}
								else
								{
									if (!escopia && version_ciu > 2)
									{
										totreg = tam / 5;
										dato = registros;										
										while(dato < (registros+tam))
										{
											for (p = dato+5;p < registros;p+=5)
											{
												if (!memcmp(dato,p,5))
												{
													totreg--;
													break;
												}
											}
											dato += 5;
										}
										tamtot1 = (fi->f_lfich+31)*totreg;
										tamtot2 = ixxfichs[fi->f_fd]->cabecera.nrecords*(fi->f_lfich+1);
										tamtot2 -= (tamtot2*20/100); /* aunque falte un 20% aun justifica la copia */
										if (tamtot2 <= tamtot1)
										{
											rw_base_close_(fd_diu);
											fd_diu = -1;
											escopia = 1;
											paqpreg2 = rw_lseek_(fd_paq,0L,SEEK_CUR);
											rw_lseek_(fd_paq,paqpreg1,SEEK_SET);
											rw_write_(fd_paq,&modo_diu,sizeof(long));
											rw_write_(fd_paq,&escopia,sizeof(long));										
											rw_lseek_(fd_paq,paqpreg2,SEEK_SET);
											tam = ixxfichs[fi->f_fd]->cabecera.nrecords;
										}
									}
									if (escopia)
									{
										totreg = tam;
										for (nreg = 0;nreg < totreg;nreg++)
										{
											if (ixx_lee_datos(fi->f_fd,fi->f_buf,nreg+1) == 1)
											{
												c = 0;
												ixx_borra_datos(fi->f_fd,nreg+1);
											}
											else
											{
												c = 10;
											}
											if (dtabla)
											{
												if (MirameLosChinos((unsigned char *)fi->f_buf,fi->f_lfich,dtabla))
												{
													hay_errores++;
												}
											}
											tamactual += rw_write_(fd_paq,fi->f_buf,fi->f_lfich);
											tamactual += rw_write_(fd_paq,&c,1);
											if (!(nnreg % 27))
											{
								                sprintf(tmp3,"%ld",nnreg);
												pprint(1222,tmp3);
											}
											nnreg++;
										}
									}
									else
									{									
										totreg = tam / 5;
										dato = registros;/*registros + ((reg/5)-1)*5*/;
										while(dato < (registros+tam))
										{
											for (p = dato+5;p < registros;p+=5)
											{
												if (!memcmp(dato,p,5))
												{
													totreg--;
													break;
												}
											}
											if (p >= registros)
											{                                
												memcpy(&reg,dato+1,4);

												moto_long(dato+1);

												tamactual += rw_write_(fd_paq,dato,5);

												moto_long(dato+1);

												g = 0;

												if (reg)
												{
													if (dato[0] != 1) /* registro valido */
													{
														pon_registro_actual(fi,reg);
														if (lee_reg(fi,ISEQUAL) >= 0)
														{                                                
															g = 1;
														}
													}
												}
												if (!g)
												{
													memset(fi->f_buf,0,fi->f_lfich);                                        
												}
												if (dtabla)
												{
													if (MirameLosChinos((unsigned char *)fi->f_buf,fi->f_lfich,dtabla))
													{
														hay_errores++;
													}
												}
												tamactual += rw_write_(fd_paq,fi->f_buf,fi->f_lfich);
												moto_long(&hora);
												tamactual += rw_write_(fd_paq,&hora,sizeof(long));
												moto_long(&hora);
												memset(usuario,0,10);
												if (rw_usuario)
												{
													strcpy(usuario,rw_usuario);
												}
												tamactual += rw_write_(fd_paq,usuario,10);

												numero_secuencial++;
												tmp1 = (control_ciu << 16) | numero_secuencial;
												moto_long(&tmp1);
												tamactual += rw_write_(fd_paq,&tmp1,sizeof(long));
												moto_long(&tmp1);
												nreg++;
												if (tamactual >= _maximo_dsk_)
												{
													nnreg++;
													break;
												}
											}
											if (!(nnreg % 27))
											{
								                sprintf(tmp3,"%ld",nnreg);
												pprint(1222,tmp3);
											}
											nnreg++;
											dato += 5;
										}
									}
								}
							}
						}

						if (dtabla)
						{
							libera_des_tabla(fi,&defectos,&descamp,&dtabla);
							descamp = NULL;
							defectos = NULL;
							dtabla = NULL;
						}

						if (version_ciu > 2)
						{
							fi_calcula_md5(ixxfichs[fi->f_fd]->fd_dat,md5out);
							paqpreg2 = rw_lseek_(fd_paq,0L,SEEK_CUR);
							rw_lseek_(fd_paq,paqpreg1+sizeof(long)*2,SEEK_SET);
							rw_write_(fd_paq,md5out,16);
							rw_lseek_(fd_paq,paqpreg2,SEEK_SET);
						}

                        cierra_ind(fi);
                        tiempo = rw_lseek_(fd_paq,0L,SEEK_CUR);
                        rw_lseek_(fd_paq,preg,SEEK_SET);
						moto_long(&nreg);
                        rw_write_(fd_paq,&nreg,sizeof(long));
						moto_long(&nreg);						
                        rw_lseek_(fd_paq,tiempo,SEEK_SET);
                    }
                    free_defpri(fi);
                }   
				
				if (tamactual >= _maximo_dsk_ && totreg && totreg > nnreg)
				{					
					if (nnreg) /* reajustar diu restante */
					{
                        if (registros)
                           free(registros);
                        registros = malloc((sizeof(long)*4)+((totreg-nnreg)*5));
						rw_base_lseek_(fd_diu,0L,SEEK_SET);
						rw_base_read_(fd_diu,registros,(sizeof(long)*4));
						rw_base_lseek_(fd_diu,(long)(sizeof(long)*4)+(nnreg*5),SEEK_SET);
						rw_base_read_(fd_diu,registros+(sizeof(long)*4),((totreg-nnreg)*5));
						rw_base_close_(fd_diu);
						fd_diu = rw_base_open_(tmp,O_RDWR | O_BINARY | O_TRUNC,S_IREAD | S_IWRITE);
						if (fd_diu >= 0)
						{
							rw_base_lseek_(fd_diu,0L,SEEK_SET);
							rw_base_write_(fd_diu,registros,(sizeof(long)*4)+((totreg-nnreg)*5));
						}
					}
					if (modo_diu)
					{
						/* error interno */						
						noborradiu = 0;
					}
					else
					    noborradiu = 1;
				}
				else
					noborradiu = 0;

				if (fd_diu >= 0)
				{
					rw_base_close_(fd_diu);
				}
				if (!noborradiu)
				{
					losprocesados[i] = 1;

					if (aborrar)
					{
						if (aborrar[i])
							free(aborrar[i]);
						aborrar[i] = malloc(512);
						strcpy(aborrar[i],tmpdiu);
						strcpy(aborrar[i]+256,tmp);
					}
					else
					{				
						/* no borramos el diu real ... */
						if ((fd_diu = rw_base_open_(tmp,O_RDWR | O_BINARY | O_TRUNC,S_IREAD | S_IWRITE)) >= 0)
						   rw_base_close_(fd_diu);
					}
				}
				else
				{
					losprocesados[i] = 2;
				}
            }

			rw_lseek_(fd_paq,pos_nf,SEEK_SET);
			moto_long(&pasados);
			rw_write_(fd_paq,&pasados,sizeof(long));
			moto_long(&pasados);

            rw_close_(fd_paq);

			if (!pasados)
			{
				rw_base_unlink(paquete);
				rr = 1;
			}
			
			grabartiempo = 1;
        }

		
		if (rr != 0)
		{
			if (npaquetes > 0)
			{
				npaquetes--;
				free(paquetes[npaquetes]); /* el ultimo no vale */
				while(npaquetes > 0)
				{
					npaquetes--;
					if (rr >= 0 && !hay_errores)
					{
						strcpy(paquete,paquetes[npaquetes]);
						strcat(paquete,".tmp");
						rw_base_unlink(paquetes[npaquetes]);
						rw_base_rename(paquete,paquetes[npaquetes]); 
					}
					free(paquetes[npaquetes]);
				}
			}

			if (aborrar)
			{
				int xxi,xxfd_diu;
				for (xxi = 0;xxi < naborrar;xxi++)
				{
					if (aborrar[xxi])
					{
						if (!hay_errores)
						{
							if (rr >= 0)
							{
								if ((xxfd_diu = rw_base_open_(aborrar[xxi],O_RDWR | O_BINARY | O_TRUNC,S_IREAD | S_IWRITE)) >= 0)
								   rw_base_close_(xxfd_diu);
							}
							rw_base_unlink(aborrar[xxi]+256);
						}
						free(aborrar[xxi]);
					}
				}
				free((char *)aborrar);
				aborrar = NULL;
				naborrar = 0;
			}

			procesados = 0;
			if (rr >= 0 && !hay_errores)
			{
				if (losprocesados && ciupreg1)
				{
					int xxi;
					rw_base_lseek_(fd_ciu,ciupreg1,SEEK_SET);
					for (xxi = 0;xxi < nlosprocesados;xxi++)
					{
						if (losprocesados[xxi] == 1)
						{
							rw_base_write_(fd_ciu,buffer_vacio,112);
							procesados++;
						}
						else
						{
							rw_base_lseek_(fd_ciu,112,SEEK_CUR);
						}
					}			
				}
			}
			if (grabartiempo && !hay_errores)
			{
				rw_base_lseek_(fd_ciu,(long)sizeof(long),SEEK_SET);
				moto_long(&control_ciu);
				rw_base_write_(fd_ciu,&control_ciu,sizeof(long));
				moto_long(&control_ciu);			
				if (nlosprocesados && procesados == nlosprocesados)
				{
					ficheros = 0L;
					rw_base_lseek_(fd_ciu,(long)sizeof(long)*3,SEEK_SET);
					moto_long(&ficheros);
					rw_base_write_(fd_ciu,&ficheros,sizeof(long));
					moto_long(&ficheros);				
				}
			}
			rw_base_lseek_(fd_ciu,(long)sizeof(long)*2,SEEK_SET);
			rw_base_locking(fd_ciu,LK_UNLCK,1);		
			if (grabartiempo && !hay_errores)
			{
				grabartiempo = 0;
				tiempo = time(NULL);
				moto_long(&tiempo);
				rw_base_lseek_(fd_ciu,(long)sizeof(long)*2,SEEK_SET);
				rw_base_write_(fd_ciu,&tiempo,sizeof(long));
				moto_long(&tiempo);
			}
			rw_base_close_(fd_ciu);
			if (losprocesados)
			{
				free((char *)losprocesados);
				losprocesados = NULL;
			}
			nlosprocesados = 0;
			fd_ciu = -1;		
		}
    }


	if (w)
	{
        w_pinta(w);
        w_fin(w);
		w = NULL;
	}
    if (registros)
        free(registros);
    ixx_ciudiu = _ixx_ciudiu;
    ixx_haydiu = _ixx_haydiu;
	if (rr > 0 && hay_errores)
	{
		return 2;
	}
    return(rr);
}


static int  minint = -32767,
maxint = 32767;

void calcula_campo(CAMPO_DEF *pcam)
{
	int qc,tipo,tama,decimal,j;
	double maxi,mini,a1,a2;
	char *maxstr,*minstr;
	char digitos[40];
	
	qc = (pcam->c_qc);
	maxstr = pcam->c_maximo;
	minstr = pcam->c_minimo;
	switch (qc) {
	case -2 :     
		pcam->c_tipo = LONGTIPO;
		pcam->c_long = LONGTAMA;
		strcpy(pcam->c_usi,"%ld");
		break;
	case -1 :     
		pcam->c_tipo = FECHATIPO;
		pcam->c_long = FECHATAMA;
		strcpy(pcam->c_usi,"%-10s");
		strcpy(maxstr,"");
		strcpy(minstr,"");
		break;
	case 0 :      
		if (!maxstr[0] && !minstr[0])
		{
				tipo = NULOTIPO;
				tama = NULOTAMA;
				pcam->c_usi[0] = 0;
		}
		else
		{
			a1=rwatof(maxstr);
			a2=rwatof(minstr);
			maxi = max(a1,a2);
			a1=rwatof(maxstr);
			a2=rwatof(minstr);
			mini = min(a1,a2);
			if (strlen(minstr) > 1 && minstr[0] == '0') {
				digitos[0] = '0';
				sprintf(digitos+1,"%d",len_numerico(minstr,maxstr));
			   }
			else
				sprintf(digitos,"%d",len_numerico(minstr,maxstr));
			decimal = max(decimales(maxstr),decimales(minstr));
			if (decimal > 0 || decimales_redondeo_campo(minstr,maxstr)) {
				tipo = DOUBLETIPO;
				tama = DOUBLETAMA;
				sprintf(pcam->c_usi,"%%%s.%dlf",digitos,decimal);
			   }
			else if ((mini < (double)minint) || (maxi > (double)maxint)) {
				tipo = DOUBLETIPO;
				tama = DOUBLETAMA;
				sprintf(pcam->c_usi,"%%%s.0lf",digitos);
			   }
			else {
				tipo = INTTIPO;
				tama = INTTAMA;
				sprintf(pcam->c_usi,"%%%sd",digitos);
			   }
		}
	    pcam->c_tipo = tipo;
	    pcam->c_long = tama;
	    break;
	default :     
		pcam->c_tipo = CHARTIPO;
		pcam->c_long = CHARTAMA * qc;
		j = pcam->c_mante.c_qpl;
		if (j > 0 && j < qc)
			qc = j;
		sprintf(pcam->c_usi,"%%-%d.%ds",qc,qc);		
		break;
	}
}


int inserta_campo(FI_DEF *fi,int n,char *id,char *nombre,int qc,char *minimo,char *maximo,char *defecto)
{
	int i = fi->f_ncampos,j;
	int lfich = fi->f_lfich;

	fi->f_ncampos++;
	fi->f_campos = (CAMPO_DEF *)realloc(fi->f_campos,sizeof(CAMPO_DEF) * (fi->f_ncampos+1));

	for (;i > n;i--)
	{
		fi->f_campos[i] = fi->f_campos[i-1]; /* desestructura la pantalla!!! */
	}

	memset(&fi->f_campos[n],0,sizeof(CAMPO_DEF));

	fi->f_campos[n].c_qc = qc;
	fi->f_campos[n].c_minimo = malloc(strlen(minimo)+10);
	strcpy(fi->f_campos[n].c_minimo,minimo);
	fi->f_campos[n].c_maximo = malloc(strlen(maximo)+10);
	strcpy(fi->f_campos[n].c_maximo,maximo);
	fi->f_campos[n].c_mante.c_qpl = 80;
	fi->f_campos[n].c_mante.c_modif  = 1;
	fi->f_campos[n].c_mante.c_visual = 1;
	fi->f_campos[n].c_mante.c_lineal = 0;
    fi->f_campos[n].c_lista = malloc(strlen(id)+12);
	fi->f_campos[n].c_lista[0] = 13;
	strcpy(fi->f_campos[n].c_lista+1,id);
	fi->f_campos[n].c_mante.c_relacion = malloc(50);
	fi->f_campos[n].c_mante.c_relacion[0] = 0;

	fi->f_campos[n].c_usi = strmalloc(50);
	fi->f_campos[n].c_usi[0] = 0;

    fi->f_campos[n].c_mante.c_mensaje = "";

    fi->f_campos[n].c_nombre = malloc(strlen(nombre)+ 1);
	strcpy(fi->f_campos[n].c_nombre,nombre);

   /*
    fi->f_titulo = realloc(strlen(nombre)+ 1 + strlen(defecto) + 1);
	strcpy(fi->f_campos[n].c_titulo,nombre);
	fi->f_campos[n].c_mante.c_defecto = fi->f_campos[n].c_titulo + strlen(nombre) + 1;
	strcpy(fi->f_campos[n].c_mante.c_defecto,defecto);
	*/

	fi->f_campos[n].c_mante.c_defecto = "";

	fi->f_campos[n].c_fflag = 0;

   /*
	*(((char *)&(pcam->c_fflag)) + 1) = cam[15].qo[0];
	*(((char *)&(pcam->c_fflag))) = (unsigned char)(((atoi(cam[17].qo) << 4) & 0xf0) | atoi(cam[16].qo));
	*/

    fi->f_campopos  = (int *)realloc(fi->f_campopos,sizeof(int) * (fi->f_ncampos+2));
	fi->f_campopos[fi->f_ncampos] = -1;
	fi->f_campopos[n] = -1;

    fi->f_campowids = (int *)realloc(fi->f_campowids,sizeof(int) * (fi->f_ncampos+2));
	fi->f_campowids[fi->f_ncampos] = -1;	
	fi->f_campowids[n] = -1;	

	fi->f_camporden = (int *)realloc(fi->f_camporden,sizeof(int) * (fi->f_ncampos+2));
	fi->f_camporden[fi->f_ncampos] = -1;	
	fi->f_camporden[n] = -1;	

	fi->f_taborder  = (int *)realloc(fi->f_taborder,sizeof(int) * (fi->f_ncampos+2));
	fi->f_taborder[fi->f_ncampos] = -1;	
	fi->f_taborder[n] = -1;	

	fi->f_orderflags  = (int *)realloc(fi->f_orderflags,sizeof(int) * (fi->f_ncampos+2));	
	fi->f_orderflags[fi->f_ncampos] = 0;
	fi->f_orderflags[n] = 0;


	fi->f_campos[n].c_offset = fi->f_lfich;
	
	calcula_campo(&fi->f_campos[n]);	

	fi->f_lfich += fi->f_campos[n].c_long;

	if (lfich != fi->f_lfich)
	{
		fi->f_buf = realloc(fi->f_buf,fi->f_lfich+1);
	}
	if (fi->f_bufcompara)
	{
		free(fi->f_bufcompara);
		fi->f_bufcompara = NULL;
	}
	if (fi->f_crccampos)
	{
		free(fi->f_crccampos);
		fi->f_crccampos = NULL;
	}

	return n;
}


static void ReasignaCampos(int *asignacion,FI_DEF *fi,char *buf,FgvStdTabla *itabla)
{
	int i,j;
	int offset1 = 0,offset2 = 0;	
	struct dbf_campo campo1;
    struct dbf_campo campo2;

	memset(&campo1,0,sizeof(campo1));
	memset(&campo2,0,sizeof(campo2));

	for (i = 0;asignacion[i] != -1 && i < 256;i++)
	{
		j = asignacion[i];

        campo1.tipo = fi->f_campos[i].c_tipo+48;
        campo1.len = fi->f_campos[i].c_long;
        campo1.decimales = deci_campo(fi,i);
		campo1.data_address = offset1;

        campo2.tipo = itabla->Campo[j].tipo;
        campo2.len = itabla->Campo[j].len;
        campo2.decimales = itabla->Campo[j].decimales;
		campo2.data_address = offset2;

		ixx_dbf_pasa_campo(buf+offset2,&campo2,fi->f_buf+offset1,&campo1);

	    offset1 += fi->f_campos[i].c_long;
		offset2 += itabla->Campo[j].len;
	}
}


static int graba_ficontrol(int modo,FI_DEF *fi,FI_DEF *ficontrol)
{
	int r = -1,i;
	int campo1,campo2;
	char buf[512];

	if (!ficontrol || ficontrol->f_fd < 0)
		return r;

	defecto_ficha(ficontrol);
	for (i = 0;i < fi->f_clave[0].k_ncampos && i < ficontrol->f_clave[0].k_ncampos;i++)
	{
		campo1 = fi->f_clave[0].k_campo[i] - 1;
		campo2 = ficontrol->f_clave[0].k_campo[i] - 1;
		ld_campo(campo1,fi,buf);
		st_campo(campo2,ficontrol,buf);
	}

	if (!lee_soloindice(ficontrol,ISEQUAL)) 
	{
		if (!modo)
		{
			r = graba_actual(ficontrol);
		}
		else if (modo == 1)
		{
			for (i = 0;i < ficontrol->f_ncampos;i++)
			{
				if (ficontrol->f_campos[i].c_key != 1 && ficontrol->f_campos[i].c_qc == 1)
				{
					st_campo(i,ficontrol,"X");
					break;
				}
			}
			if (i < ficontrol->f_ncampos)
			{
				r = graba_actual(ficontrol);
			}
			else
			{
				r = borra_actual(ficontrol);
			}
		}
	}
	else
	{
		if (modo ==  1)
		{
			for (i = 0;i < ficontrol->f_ncampos;i++)
			{
				if (ficontrol->f_campos[i].c_key != 1 && ficontrol->f_campos[i].c_qc == 1)
				{
					st_campo(i,ficontrol,"X");
					break;
				}
			}	
			if (i < ficontrol->f_ncampos)
			{
				r = graba_nuevo(ficontrol);
			}
		}
		else
			r = graba_nuevo(ficontrol);
		
	}
	return r;
}

static char *ListaNegra[] = {"dsgasm52","dsgasm53",NULL};

int ProcesaPaqueteCiu(char *paquete_,int volcado)
{
    int rr = -1;
    int fd_paq,fd;
    long id;
    long control_ciu;
	long version_ciu = 0;
    long tiempo1;
    long tiempo2;
    long ficheros;
    long nreg;
    long reg;
    long l_reg;
    long hora;
    char usuario[20];
	char modulo[10];
    int i,j;
    unsigned char registro[112];
    unsigned char dato[5];
    unsigned long tmp1;
	int modo_path = 0;
	char uno[256];
	char dos[256];
	char paquete[512];
	char tmpf[512];
	char tmp3[81];
    FI_DEF *fi;
	FI_DEF *ficontrol = NULL;
	long pos_seek;
	FgvStdTabla itabla;
	char *databuf = NULL;
	int tmpl;
	int *asignacion = NULL;	
	long modo_diu = 0;
	int laclave;
	long escopia = 0;
	unsigned char md5nulo[16];
	unsigned char md5in[16];
	unsigned char md5out[16];
	FILE *file_log = NULL;
	int nerrores = 0;
	int calcularmd5 = 0;
	long preposicion;
	int  pasada;
	int  pasadabuena;
	int  topepasadas;
	int  enlistanegra;
	char *ptmp1;
	long registrosantes,registrosdespues;
	VENT *w = NULL;

	memset(md5nulo,0,16);

	if (volcado)
	{
		pasadabuena = 0;
		topepasadas = 1;
	}
	else
	{
		pasadabuena = 1;
		topepasadas = 2;
	}
	if (*paquete_ == '*')
	{
		paquete_++;
		calcularmd5 = 1;
	}
	strcpy(paquete,paquete_);
	for (i = 0;paquete[i] && paquete[i] != ' ';i++);
	if (paquete[i] == ' ')
	{
		paquete[i] = 0;
		i++;
		if (paquete[i] == 'S')
		{
			for (j = i;paquete[j] && paquete[j] != ' ';j++);
			if (paquete[j] == ' ')
			{
				paquete[j] = 0;
				modo_path = 1;			
				strcpy(uno,paquete+i+1);
				strcpy(dos,paquete+j+1);
			}
		}
		else if (paquete[i] == 'A')
		{
			for (j = i;paquete[j] && paquete[j] != ' ';j++);
			if (paquete[j] == ' ')
			{
				paquete[j] = 0;
				modo_path = 2;
				strcpy(uno,paquete+i+1);
				strcpy(dos,paquete+j+1);
			}
		}
	}

	itabla.Campo = NULL;

    fd_paq = rw_open_(paquete,O_RDWR | O_BINARY);

    if (fd_paq >= 0)
    {
        rw_read_(fd_paq,&id,sizeof(long));
		moto_long(&id);
        if (id == FGV_ID_CIU_PAQ || id == FGV_ID_CIU2_PAQ)
        {
			if (id == FGV_ID_CIU2_PAQ)
			{
				rw_read_(fd_paq,&version_ciu,sizeof(long));
				moto_long(&version_ciu);
			}
			rr = 0;
            rw_read_(fd_paq,&control_ciu,sizeof(long));
			moto_long(&control_ciu);
            rw_read_(fd_paq,&tiempo1,sizeof(long));            
			moto_long(&tiempo1);
            rw_read_(fd_paq,&tiempo2,sizeof(long));            
			moto_long(&tiempo2);
            rw_read_(fd_paq,&ficheros,sizeof(long));
			moto_long(&ficheros);

			strcpy(tmpf,paquete);
			strcat(tmpf,".log");
			file_log = fopen(tmpf,"w");
			if (file_log)
			{
				if (version_ciu > 3)
				{
					fprintf(file_log,"Version:%d ERROR:Soportada=3,Control:%lu,T1:%lu,T2:%lu,Nfiles:%d\r\n",version_ciu,control_ciu,tiempo1,tiempo2,ficheros);
				}
				else
				{
					fprintf(file_log,"Version:%d,Control:%lu,T1:%lu,T2:%lu,Nfiles:%d\r\n",version_ciu,control_ciu,tiempo1,tiempo2,ficheros);
				}
			}

            w = w_ini(610,1970);
            w_salva(w);


			preposicion = rw_lseek_(fd_paq,0L,SEEK_CUR);			
			rr = 0;
			for (pasada = 0;pasada < topepasadas && rr == 0;pasada++)
			{
                zonablan(610,1970);
                cuadro(610,1970);
				sprintf(tmp3,"[%d]Paquete: %-45.45s",pasada,paquete);			
                pprint(812,tmp3);
				if (file_log)
				{
					if (pasada == pasadabuena)
					{
						fprintf(file_log,"****VALIDACION****\r\n");
					}
				}
				rr = 0;
				rw_lseek_(fd_paq,preposicion,SEEK_SET);
				for (i = 0;i < ficheros && rr == 0;i++)
				{
					registrosantes   = 0L;
					registrosdespues = 0L;
					enlistanegra = 0;
					escopia = 0;
					memset(md5in,0,16);
					memset(md5out,0,16);
					if (itabla.Campo)
					{
						free(itabla.Campo);
						itabla.Campo = NULL;
					}				

					if (rw_read_(fd_paq,registro,112) != 112) 
					{
						if (file_log)
						{
							fprintf(file_log,"ERROR:PAQUETE TRUNCADO\r\n");
						}
						rr = -5;
						break;
					}
					/*
					if (*quita_path(registro+52) == 'x')
						continue;
					*/

					sprintf(tmp3,"Fichero: %-48.48s",registro);
					pprint(1012,tmp3);
					sprintf(tmp3,"Registro:%-48.48s","");
					pprint(1212,tmp3);

					if (file_log)
					{
						fprintf(file_log,"Fichero: %d,",i);
					}
					if (file_log)
					{
						fprintf(file_log,"%s,%s,",registro,registro+52);
					}
					rw_read_(fd_paq,&nreg,sizeof(long));
					if (nreg < 0 || nreg > 50000000) 
					{
						if (file_log)
						{
							fprintf(file_log,"ERROR:REGISTROS:%ld\r\n",nreg);
						}
						rr = -6;
						break; 
					}
					moto_long(&nreg);
					if (file_log)
					{
						fprintf(file_log,"%ld,",nreg);
					}
					if (version_ciu > 0)
					{
						/* leer estructura de datos */
						rw_read_(fd_paq,&pos_seek,sizeof(long));
						moto_long(&pos_seek);
						if (pos_seek < 0 || pos_seek > 50000000) 
						{
							if (file_log)
							{
								fprintf(file_log,"ERROR:POS_SEEK:%ld\r\n",pos_seek);
							}
							rr = -6;
							break; 
						}

						enlistanegra = 0;
						ptmp1 = quita_path(registro+52);
						for (j = 0;ListaNegra[j];j++)
						{
							if (!strcmp(ptmp1,ListaNegra[j]))
							{
								enlistanegra = 1;
								break;
							}							
						}

						rw_read_(fd_paq,&itabla,sizeof(itabla));
						itabla.Campo = (CampoTabla *)malloc(sizeof(CampoTabla)*itabla.ncampos);
						rw_read_(fd_paq,itabla.Campo,sizeof(CampoTabla)*itabla.ncampos);
						if (version_ciu > 1)
						{
							rw_read_(fd_paq,&modo_diu,sizeof(long));
						}
						if (version_ciu > 2)
						{
							rw_read_(fd_paq,&escopia,sizeof(long));
							rw_read_(fd_paq,md5in,16);
						}					
						/******************************************/
						rw_lseek_(fd_paq,pos_seek,SEEK_SET);
						/***** COMPATIBILIDAD ENTRE VERSIONES *****/
					}
					else
					{
						if (file_log)
						{
							fprintf(file_log,"ERROR:DSK MUY ANTIGUO, NO HAY DEF\r\n");
						}
						rr = -2;
						break;
					}
					memcpy_long(&l_reg,registro+104,sizeof(long));
					if (l_reg < 0 || l_reg > 5000000)
					{
						if (file_log)
						{
							fprintf(file_log,"ERROR:LONGITUD FICHA:%ld\r\n,",l_reg);
						}
						rr = -6;
						break; 
					}
					if (file_log)
					{
						fprintf(file_log,"%ld,",l_reg);
					}
					if (databuf)
					{
						free(databuf);					
					}
					databuf = malloc(l_reg+1);
					if (modo_path == 1)
					{
						if (!memcmp(registro+52,uno,strlen(uno)))
						{
							strcpy(tmpf,dos);
							strcat(tmpf,registro+52+strlen(uno));
						}
						else
							strcpy(tmpf,registro+52);
					} else if (modo_path == 2)
					{
						strcpy(tmpf,uno);
						strcat(tmpf,quita_path(registro+52));
					} else
						strcpy(tmpf,registro+52);
					
					if (!lee_defn(tmpf,&fi)) 
					{
						int jodido = 0;
						if (asignacion)
						{
							free(asignacion);
							asignacion = NULL;
						}
						defecto_ficha(fi);
						if (itabla.Campo)
						{
							int difiere = 0;						
							if (itabla.ncampos < 1 || itabla.ncampos > 255)
							{						
								jodido = 1;
							}
							if (!jodido)
							{
								jodido = 0;
								for (j = 0;j < itabla.ncampos;j++)
								{
									jodido += itabla.Campo[j].len;
								}
								if (jodido != itabla.reclen)
								{
									jodido = 1;
								}
								else
								{
									jodido = 0;
									if (fi->f_ncampos != itabla.ncampos)
									{
										difiere = 1;
									}
									else
									{
										for (j = 0;j < fi->f_ncampos;j++)
										{
											if ((fi->f_campos[j].c_tipo+48) != itabla.Campo[j].tipo)
											{
												difiere = 1;
												break;
											}
											else if (fi->f_campos[j].c_long != itabla.Campo[j].len)
											{
												difiere = 1;
												break;
											}
										}
									}
								}
							}
							if (difiere)
							{
								asignacion = (int *)malloc((fi->f_ncampos+1)*sizeof(int));
								memset(asignacion,0xff,(fi->f_ncampos+1)*sizeof(int));
								for (j = 0;j < fi->f_ncampos;j++)
								{
									if (j < itabla.ncampos)
										asignacion[j] = j;
									else
										break;
								}
							}
							if (!jodido && itabla.reclen != l_reg)
							{
								l_reg = itabla.reclen;
								jodido = 2;
							}
						}
						if (file_log)
						{
							if (jodido == 1)
							{
								fprintf(file_log,"=???,");
							}
							else
							{
								if (jodido == 2)
								{
									fprintf(file_log,"|||");
								}
								if (!asignacion)
								{								
									fprintf(file_log,"=,");
								}
								else
								{
									fprintf(file_log,"!=CAMBIO VERSION,");
								}
							}
						}
						if (jodido == 1 && !enlistanegra)
						{
							if (file_log)
							{
								fprintf(file_log,"ERROR:DEF INTERNO INCORRECTO\r\n");
							}
							rr = -3;							
							goto errorinterior;
						}
						if (modo_path == 1)
						{
							if (!memcmp(registro,uno,strlen(uno)))
							{
								strcpy(fi->f_fichero,dos);
								strcat(fi->f_fichero,registro+strlen(uno));
							}
							else
								strcpy(fi->f_fichero,registro);
						} else if (modo_path == 2)
						{
							strcpy(tmpf,uno);
							strcat(tmpf,quita_path(registro));
							strcpy(fi->f_fichero,dos);
							strcat(fi->f_fichero,quita_path(registro));
						} else
							strcpy(fi->f_fichero,registro);

						if (volcado)
						{
							*(quita_path(fi->f_fichero)) = 'k';
							*(quita_path(registro)) = 'k';
							*(quita_path(tmpf)) = 'k';
							
							/* campos de control: */
							/* 1 alfa flag de alta mod baja */
							/* double registro */
							/* double fecha-hora en formato 'time()' */
							/* 10 bytes usuario */
							/* double numero de orden (la clave *)*/

							inserta_campo(fi,fi->f_ncampos,"kflag","Flag",1,"","","");
							inserta_campo(fi,fi->f_ncampos,"kreg","Registro",0,"0","999999999","0");
							inserta_campo(fi,fi->f_ncampos,"ktiempo","Tiempo",0,"0","999999999","0");
							inserta_campo(fi,fi->f_ncampos,"kuser","Usuario",10,"","","");
							inserta_campo(fi,fi->f_ncampos,"korden","Orden",0,"0","999999999","0");
							laclave = fi->f_ncampos;
							if (modo_diu > 1)
							{
								inserta_campo(fi,fi->f_ncampos,"kmodulo","Modulo",8,"","","");
							}

							while(fi->f_nclaves > 1)
							{
								fi->f_nclaves--;
								free((char *)fi->f_clave[fi->f_nclaves].k_campo);
								free((char *)fi->f_clave[fi->f_nclaves].k_orden);
							}
							
							fi->f_nclaves = 1;
							fi->f_clave[0].k_ncampos = 1;
							fi->f_clave[0].k_campo[0] = laclave;
							fi->f_clave[0].k_orden[0] = 1;
							fi->f_campos[laclave-1].c_key = 1;

							graba_defn(tmpf,fi);
						}
						else if (escopia)
						{
							if (pasada == pasadabuena)
							{
								borra_ind(fi);
							}
						}					

						if (file_log)
						{
							if (escopia)
							{
								fprintf(file_log,"COPIA,");
							}
							else
							{
								fprintf(file_log,"INCREMENTAL,");
							}
						}

						nerrores = 0;
						if (pasada == pasadabuena)
						{
							fd = abre_ind(fi,ISINOUT + ISMANULOCK);
							if (fd < 0)
							{
								fi->f_fd = crea_ind(fi);
								cierra_ind(fi);
								fd = abre_ind(fi,ISINOUT + ISMANULOCK);
							}
							if (fd >= 0)
							{
								registrosantes = tam_btrv(fi);
							}
						}
						else
						{
							fd = 99999;
							registrosantes   = 0L;
							registrosdespues = 0L;
						}

						if (fd >= 0)
						{							
							fi->f_fd = fd;
							if (volcado)
							{
								for (j = 0;j <nreg;j++)
								{
									if (!(j % 27))
									{
								        sprintf(tmp3,"%ld",j);
										pprint(1222,tmp3);
									}

									if (!escopia)
									{
										rw_read_(fd_paq,dato,5);
										moto_long(dato+1);
									}
									rw_read_(fd_paq,databuf,l_reg);
									if (escopia)
									{
										rw_read_(fd_paq,dato,1);
										if (dato[0] == 10)
										{
											dato[0] = 0;
										}
										else
										{
											dato[0] = 1;
										}
										*((long *)(dato+1)) = (j+1);
									}
									if (!asignacion)
									{
										if (l_reg < fi->f_lfich)
											tmpl = l_reg;
										else
											tmpl = fi->f_lfich;
										memcpy(fi->f_buf,databuf,tmpl);							
										/*rw_read_(fd_paq,fi->f_buf,l_reg);*/
									}
									else
									{
										ReasignaCampos(asignacion,fi,databuf,&itabla);
									}
									if (escopia)
									{									
										/* falta poner a defectos para el volcado ... no es necesario de momento... */
									}
									else
									{
										rw_read_(fd_paq,&hora,sizeof(long));
										moto_long(&hora);
										memset(usuario,0,20);
										rw_read_(fd_paq,usuario,10);
										rw_read_(fd_paq,&tmp1,sizeof(long));
										moto_long(&tmp1);
										fi->f_buf[l_reg] = dato[0];
										stdbl( (double)*((long *)(dato+1)),fi->f_buf+l_reg+1);
										stdbl( (double)hora,fi->f_buf+l_reg+1+sizeof(double));
										stchar(usuario,fi->f_buf+l_reg+1+sizeof(double)*2,10);
										stdbl( (double)tmp1,fi->f_buf+l_reg+1+sizeof(double)*2+10);
										memset(modulo,0,sizeof(modulo));
										if (modo_diu > 1)
										{
											rw_read_(fd_paq,modulo,8);
											stchar( modulo,fi->f_buf+l_reg+1+sizeof(double)*3+10,8);
										}
									}
									if (pasada == pasadabuena)
									{
										if (graba_nuevo(fi) < 0)
											nerrores++;
									}
								}
							}
							else
							{
								if (pasada == pasadabuena)
								{
									if (ficontrol)
									{
										if (ficontrol->f_fd > -1)
										{
											cierra_ind(ficontrol);
										}
										free_defpri(ficontrol);
									}
									ficontrol = NULL;
									*(quita_path(tmpf)) = 'x';
									lee_defn(tmpf,&ficontrol);
									if (ficontrol)
									{
										strcpy(tmpf,quita_path(ficontrol->f_fichero));
										strcpy(ficontrol->f_fichero,fi->f_fichero);
										strcpy(quita_path(ficontrol->f_fichero),tmpf);
										if (escopia)
										{
											borra_ind(ficontrol);										
										}								
										if (abre_ind(ficontrol,ISINOUT + ISMANULOCK) < 0)
										{
											ficontrol->f_fd = crea_ind(ficontrol);
											cierra_ind(ficontrol);
											abre_ind(ficontrol,ISINOUT + ISMANULOCK);
										}										
									}
								}
								if (escopia)
								{
									for (j = 0;j <nreg;j++)
									{
										if (!(j % 27))
										{
								            sprintf(tmp3,"%ld",j);
											pprint(1222,tmp3);
										}
										if (rw_read_(fd_paq,databuf,l_reg+1) != l_reg+1)
										{
											if (file_log)
											{
												fprintf(file_log,"ERROR:PAQUETE TRUNCADO\r\n");
											}
											rr = -5;
											goto errorinterior;
										}
										if (pasada == pasadabuena)
										{
											if (databuf[l_reg])
											{
												if (!asignacion)
												{
													if (l_reg < fi->f_lfich)
														tmpl = l_reg;
													else
														tmpl = fi->f_lfich;
													memcpy(fi->f_buf,databuf,tmpl);							
												}
												else
												{
													ReasignaCampos(asignacion,fi,databuf,&itabla);
												}
												if (ixx_graba_datos(fi->f_fd,fi->f_buf,j+1) < 0)
													nerrores++;
												else
													graba_ficontrol(0,fi,ficontrol);
											}
											else
											{
												if (ixx_borra_datos(fi->f_fd,j+1) < 0)
													nerrores++;
											}
										}
										else
										{
											if (!enlistanegra && MirameLosChinos((unsigned char *)databuf,l_reg,&itabla))
											{
												if (file_log)
												{
													fprintf(file_log,"INCIDENCIA DE TIPOS\r\n");
												}
												rr = -4;
												goto errorinterior;
											}
										}
									}
									if (pasada == pasadabuena)
									{
										ixx_reconstruye(fi->f_fd);
									}
								}
								else for (j = 0;j <nreg;j++)
								{
									if (!(j % 27))
									{
								        sprintf(tmp3,"%ld",j);
										pprint(1222,tmp3);
									}
									if (rw_read_(fd_paq,dato,5) != 5)
									{
										if (file_log)
										{
											fprintf(file_log,"ERROR:PAQUETE TRUNCADO\r\n");
										}
										rr = -5;
										goto errorinterior;
									}
									moto_long(dato+1);
									reg = *((long *)(dato +1));
									if (reg < 0 || reg > 256000000)
									{
										if (file_log)
										{
											fprintf(file_log,"ERROR:NUMERO REGISTRO:%ld\r\n",reg);
										}
										rr = -6;
										goto errorinterior;
									}
									if (dato[0] == 1 && !reg)
									{
										if (pasada == pasadabuena)
										{
											cierra_ind(fi);
											borra_ind(fi);
											
											fi->f_fd = crea_ind(fi);
											cierra_ind(fi);
											fd = abre_ind(fi,0);
											fi->f_fd = fd;
										}										
										rw_read_(fd_paq,databuf,l_reg);
										/*apunta_ind(fi,1,ISFIRST,0);*/
									}
									else
									{
										if (pasada == pasadabuena)
										{
											apunta_ind(fi,0,ISCURR,0);
											pon_registro_actual(fi,reg);
											if (lee_reg(fi,ISEQUAL+ISLOCK) >= 0)
											{
												apunta_ind(fi,1,ISCURR,0);
												if (dato[0] != 1)
												{
													rw_read_(fd_paq,databuf,l_reg);
													if (!asignacion)
													{
														if (l_reg < fi->f_lfich)
															tmpl = l_reg;
														else
															tmpl = fi->f_lfich;
														memcpy(fi->f_buf,databuf,tmpl);							
														/*rw_read_(fd_paq,fi->f_buf,l_reg);*/
													}									
													else
													{
														ReasignaCampos(asignacion,fi,databuf,&itabla);
													}
													/*  rw_read_(fd_paq,fi->f_buf,l_reg); */
													if (graba_actual(fi) < 0)
														nerrores++;
													else
														graba_ficontrol(0,fi,ficontrol);
												}
												else
												{
													if (borra_actual(fi) < 0)
														nerrores++;

													graba_ficontrol(1,fi,ficontrol);

													rw_read_(fd_paq,databuf,l_reg);
													/*rw_read_(fd_paq,fi->f_buf,l_reg);*/
												}
												libera_ind(fi);
											}
											else                                
											{
												apunta_ind(fi,1,ISCURR,0);
												rw_read_(fd_paq,databuf,l_reg);
												if (!asignacion)
												{
													if (l_reg < fi->f_lfich)
														tmpl = l_reg;
													else
														tmpl = fi->f_lfich;
													memcpy(fi->f_buf,databuf,tmpl);
												}									
												else
												{
													ReasignaCampos(asignacion,fi,databuf,&itabla);
												}                                        
												if (dato[0] != 1)
												{											
													if (graba_nuevo_en_posicion(fi,reg) < 0)
														nerrores++;
													else
														graba_ficontrol(0,fi,ficontrol);
												}
												/*
												else
												{
													nerrores++;
												}
												*/
											}
										}
										else
										{
											if (rw_read_(fd_paq,databuf,l_reg) != l_reg)
											{
												if (file_log)
												{
													fprintf(file_log,"ERROR:PAQUETE TRUNCADO\r\n");
												}
												rr = -5;
												goto errorinterior;
											}
											if (!enlistanegra && MirameLosChinos((unsigned char *)databuf,l_reg,&itabla))
											{
												if (file_log)
												{
													fprintf(file_log,"INCIDENCIA DE TIPOS\r\n");
												}
												rr = -4;
												goto errorinterior;
											}
										}
									}
									if (modo_diu > 1)
									{
										rw_lseek_(fd_paq,(long)(10+8+(sizeof(long)*2)),SEEK_CUR);
									}
									else
									{
										rw_lseek_(fd_paq,(long)(10+(sizeof(long)*2)),SEEK_CUR);
									}
								}
							}
							if (pasada == pasadabuena && calcularmd5 && memcmp(md5in,md5nulo,16))
							{							
								fi_calcula_md5(ixxfichs[fi->f_fd]->fd_dat,md5out);
							}
							if (pasada == pasadabuena)
							{
								registrosdespues = tam_btrv(fi);
								cierra_ind(fi);	
							}
							if (file_log)
							{
								if (pasada == pasadabuena)
								{
									fprintf(file_log,"ANTES:%ld,DESPUES:%ld,",registrosantes,registrosdespues);
								}
								else
								{
									fprintf(file_log,"NONE,NONE,");
								}
								fprintf(file_log,"Incidencias:%d,",nerrores);
								for (j = 0;j < 16;j++)
								{
									fprintf(file_log,"%02x",md5in[j]);
								}
								fprintf(file_log,",");
								for (j = 0;j < 16;j++)
								{
									fprintf(file_log,"%02x",md5out[j]);
								}
								fprintf(file_log,",");
								if (memcmp(md5in,md5nulo,16))
								{
									if (calcularmd5)
									{
										if (!memcmp(md5in,md5out,16))
										{
											fprintf(file_log,"ACK");
										}
										else
										{
											fprintf(file_log,"NACK");
										}
									}
									else
									{
										fprintf(file_log,"NONE");
									}
								}
								else
								{
									fprintf(file_log,"??");
								}
								fprintf(file_log,"\r\n");
							}
						}   /* de if volcado */
						else
						{
							rr = -7;
							if (file_log)
							{
								fprintf(file_log,"ERROR: AL ABRIR EL FICHERO DE DATOS\r\n");
							}							
							break;
						}
errorinterior:
						if (ficontrol)
						{
							if (ficontrol->f_fd > -1)
							{
								cierra_ind(ficontrol);
							}
							free_defpri(ficontrol);
							ficontrol = NULL;
						}							
						free_defpri(fi);
					}
					else
					{
						if (file_log)
						{
							fprintf(file_log,"ERROR: AL LEER DEF\r\n");
						}
						rr = -8; 
						if (modo_diu > 1)
						{
							rw_lseek_(fd_paq,nreg*(long)(l_reg+15+8+(sizeof(long)*2)),SEEK_CUR);
						}
						else
						{
							rw_lseek_(fd_paq,nreg*(long)(l_reg+15+(sizeof(long)*2)),SEEK_CUR);
						}						
					}
				} /* ficheros */
			} /* pasada */
        }
		else
		{
			if (file_log)
			{
				fprintf(file_log,"EL FICHERO NO ES UN PAQUETE DE REPLICA (o esta corrompido)\r\n");
			}
			rr = -4;
		}
        rw_close_(fd_paq);
    }

	if (file_log)
		fclose(file_log);

	if (!rr)
		rw_base_unlink(paquete);

	if (itabla.Campo)
	{
		free(itabla.Campo);
	}

	if (databuf)
	{
		free(databuf);
	}

	if (asignacion)
	{
		free(asignacion);
	}

	if (w)
	{
        w_pinta(w);
        w_fin(w);
		w = NULL;
	}

    return(rr);
}


int fgvCreaPaqueteCiu(int p,int tip,int faclin)
{
    int r = -1;
    if (ixx_ciudiu)
    {
       r = CreaPaqueteCiu(ixx_ciudiu,v_memo(prog[p].indice)->alfabetica);
    }
    sprintf(v_memo(5004)->alfabetica,"%d",r);
    n_np = 1;
    return(0);
}

int fgvProcesaPaqueteCiu(int p,int tip,int faclin)
{
    int r = ProcesaPaqueteCiu(v_memo(prog[p].indice)->alfabetica,0);
    sprintf(v_memo(5004)->alfabetica,"%d",r);
    n_np = 1;
    return(0);
}

int fgvLeePaqueteCiu(int p,int tip,int faclin)
{
    int r = ProcesaPaqueteCiu(v_memo(prog[p].indice)->alfabetica,1);
    sprintf(v_memo(5004)->alfabetica,"%d",r);
    n_np = 1;
    return(0);
}

/********************************************************************************************************/
/* Manejo de ficheros raw
/********************************************************************************************************/
typedef struct _TABLA_XFICHEROS_
{
  char Path[256];
  char Modo[128];
  int  Remoto;
  int  Binario;
  union _TABLA_XFICHEROS_HANDLE_
  {
      int fd;
      FILE *f;
  } h;
} XFichero;

XFichero *XFicheros[IXXMAXFILES];

static int XTotal = 0;

int fgv_xabre(int p,int tip,int faclin)
{
    /*"XABRE"          ,"959598"          ,254,18, /* 12  */
    /* MODOS: L o C(local o cliente), T o B(texto o binario), A(añadir,lo crea si no existe), W(escritura,trunc lo crea si no existe), U(Update,debe existir no trunc), E(comprobar si existe,no abrir)*/
    /* DEFECTO: LT = Local, Texto y solo lectura */

   char *modo;
   char *fichero;
   int memo_canal;
   FILE *f;
   int mm;
   int hf;
   int fd;
   int binario = 0;
   int remoto  = 0;
   int comprobar = 0;
   char mmm[10];
   
   modo       = v_memo( prog[p++].indice )->alfabetica;
   fichero    = v_memo( prog[p++].indice )->alfabetica;
   memo_canal = prog[p++].indice;

   
   if (strbusca(modo,"C") >= 0 || strbusca(modo,"c") >= 0)
       remoto = 1;

   if (strbusca(modo,"E") >= 0 || strbusca(modo,"e") >= 0)
   {
       fd = quiza_remoto_rw_open_(fichero,3,&remoto);
       if (fd >= 0)
       {
           quiza_remoto_rw_close_(fd,&remoto);           
           if (remoto)
               fd = 1;
           else
               fd = 0;
       }
       sprintf(v_memo(5004)->alfabetica,"%d",fd);
       n_np = 3;
       return(0);
   }

   if (strbusca(modo,"B") >= 0 || strbusca(modo,"b") >= 0)
       binario = 1;
   
   for (fd = 0;fd < XTotal;fd++)
   {
       if (!XFicheros[fd]) break;
   }

   if (fd >= IXXMAXFILES)
   {
       strcpy(v_memo(5004)->alfabetica,"-2");
       n_np = 3;
       return(0);
   }
   if (fd == XTotal) XTotal++;

   if (binario)
   {
       mm = 3;
       if (strbusca(modo,"A") >= 0 || strbusca(modo,"a") >= 0)
          mm = 1;
       else
       if (strbusca(modo,"W") >= 0 || strbusca(modo,"w") >= 0)
          mm = 0;
       else
       if (strbusca(modo,"U") >= 0 || strbusca(modo,"u") >= 0)
          mm = 2;       
       hf = quiza_remoto_rw_open_(fichero,mm,&remoto);
       if (hf < 0)
       {
           strcpy(v_memo(5004)->alfabetica,"-1");
           n_np = 3;
           return(0);
       }
   }
   else
   {
       strcpy(mmm,"r");
       if (strbusca(modo,"A") >= 0 || strbusca(modo,"a") >= 0)
          strcpy(mmm,"a");
       else
       if (strbusca(modo,"W") >= 0 || strbusca(modo,"w") >= 0)
          strcpy(mmm,"w");
       else
       if (strbusca(modo,"U") >= 0 || strbusca(modo,"u") >= 0)
          strcpy(mmm,"r+");
       f = quiza_remoto_fopen(fichero,mmm,&remoto);
       if (!f)
       {
           strcpy(v_memo(5004)->alfabetica,"-1");
           n_np = 3;
           return(0);
       }
   }

   XFicheros[fd] = (XFichero *)malloc(sizeof(XFichero));
   if (binario)
   {
       XFicheros[fd]->h.fd = hf;
   }
   else
   {
       XFicheros[fd]->h.f = f;
   }

   strcpy(XFicheros[fd]->Path,fichero);
   strcpy(XFicheros[fd]->Modo,modo);   
   XFicheros[fd]->Remoto = remoto;
   XFicheros[fd]->Binario = binario;

   v_memo( memo_canal )->numerica = (double)fd;
   sprintf(v_memo(5004)->alfabetica,"%d",fd);
   n_np = 3;
   return(0);
}

int fgv_xcierra(int p,int tip,int faclin)
{
    /* "XCIERRA"        ,"98"              ,254,19, /* 14  */
    int fd = pasa_a_int( v_memo( prog[p++].indice )->numerica );
    int remoto;

    if (fd < 0 || fd >= XTotal || !XFicheros[fd])
    {
        strcpy(v_memo(5004)->alfabetica,"-1");
        n_np = 1;
        return(0);
    }

    remoto = XFicheros[fd]->Remoto;
    if (XFicheros[fd]->Binario)
    {
        quiza_remoto_rw_close_(XFicheros[fd]->h.fd,&remoto);
    }
    else
    {
        quiza_remoto_fclose(XFicheros[fd]->h.f,&remoto);
    }
    free((char *)XFicheros[fd]);
    XFicheros[fd] = NULL;
    while(XTotal > 0 && !XFicheros[XTotal-1]) XTotal--;

    strcpy(v_memo(5004)->alfabetica,"0");
    n_np = 1;
    return(0);
}

extern char *quiza_remoto_lee_linea();
int fgv_xlee(int p,int tip,int faclin)
{
	/* "XLEE"           ,"989895"          ,254,20, /* 97  */
    int fd = pasa_a_int( v_memo( prog[p++].indice )->numerica );
    int total = pasa_a_int( v_memo( prog[p++].indice )->numerica );
    int mdatos = prog[p++].indice;
    char *datos = v_memo( mdatos )->alfabetica;
    int remoto;
    int resultado;

    if (fd < 0 || fd >= XTotal || !XFicheros[fd])
    {
        strcpy(v_memo(5004)->alfabetica,"-1");
        n_np = 3;
        return(0);
    }

    if (strlen(datos) < (total+3))
    {
       if (no_memo_interna(mdatos)) 
       {
          free(v_memo(mdatos)->alfabetica);
          v_memo(mdatos)->alfabetica = strmalloc(total+4);
          strcpy(v_memo(mdatos)->alfabetica,"");
       }
       datos = v_memo(mdatos)->alfabetica;
    }    

    remoto = XFicheros[fd]->Remoto;
    if (XFicheros[fd]->Binario)
    {
        resultado = quiza_remoto_rw_read_(XFicheros[fd]->h.fd,datos,total,&remoto);        
        if (resultado >= 0 && resultado <= total)
        {
            datos[resultado] = 0;
        }
    }
    else
    {
        if (!quiza_remoto_lee_linea(datos,total,XFicheros[fd]->h.f,&remoto))
            resultado = -1;
        else
            resultado = strlen(datos);
    }
    
    sprintf(v_memo(5004)->alfabetica,"%d",resultado);
    n_np = 3;
    return(0);
}

int fgv_xgraba(int p,int tip,int faclin)
{
    int fd = pasa_a_int( v_memo( prog[p++].indice )->numerica );    
    char *datos = v_memo( prog[p++].indice )->alfabetica;
    int remoto;
    int resultado;

    if (fd < 0 || fd >= XTotal || !XFicheros[fd])
    {
        strcpy(v_memo(5004)->alfabetica,"-1");
        n_np = 2;
        return(0);
    }

    remoto = XFicheros[fd]->Remoto;
    if (XFicheros[fd]->Binario)
    {
        resultado = quiza_remoto_rw_write_(XFicheros[fd]->h.fd,datos,strlen(datos),&remoto);
    }
    else
    {
        resultado = quiza_remoto_graba_linea(datos,XFicheros[fd]->h.f,&remoto);
    }
    
    sprintf(v_memo(5004)->alfabetica,"%d",resultado);
    n_np = 2;
    return(0);
}

int fgv_xleenumero(int p,int tip,int facli)
{
	 /* "XLEE_NUMERO",   "9898"             ,254,23, /* 225 */
    int fd = pasa_a_int( v_memo( prog[p++].indice )->numerica );
    int modo = pasa_a_int( v_memo( prog[p++].indice )->numerica );
    double *d = &v_memo( prog[p++].indice )->numerica;
    int remoto;
    int resultado;
    unsigned char c;
    unsigned short s;
    unsigned long l;
    float f;

    if (fd < 0 || fd >= XTotal || !XFicheros[fd])
    {
        strcpy(v_memo(5004)->alfabetica,"-1");
        n_np = 3;
        return(0);
    }

    remoto = XFicheros[fd]->Remoto;
    if (XFicheros[fd]->Binario)
    {
        switch(modo)
        {
        case 0:            
            resultado = quiza_remoto_rw_read_(XFicheros[fd]->h.fd,&c,1,&remoto);
            *d = (double)c;
            break;
        case 1:            
            resultado = quiza_remoto_rw_read_(XFicheros[fd]->h.fd,&s,sizeof(s),&remoto);
            *d = (double)s;
            break;
        case 2:            
            resultado = quiza_remoto_rw_read_(XFicheros[fd]->h.fd,&l,sizeof(l),&remoto);
            *d = (double)l;
            break;
        case 3:            
            resultado = quiza_remoto_rw_read_(XFicheros[fd]->h.fd,&f,sizeof(f),&remoto);
            *d = (double)f;
            break;
        default:
            resultado = quiza_remoto_rw_read_(XFicheros[fd]->h.fd,(char *)d,sizeof(double),&remoto);
            break;
        }        
    }
    else
    {
        resultado = -1; /* solo binarios */
    }
    
    sprintf(v_memo(5004)->alfabetica,"%d",resultado);
    n_np = 3;
    return(0);
}

int fgv_xgrabanumero(int p,int tip,int faclin)
{
	/* "XGRABA_NUMERO", "9898"             ,254,24, /* 226 */
    int fd = pasa_a_int( v_memo( prog[p++].indice )->numerica );
    int modo = pasa_a_int( v_memo( prog[p++].indice )->numerica );
    double *d = &v_memo( prog[p++].indice )->numerica;
    int remoto;
    int resultado;
    unsigned char c;
    unsigned short s;
    unsigned long l;
    float f;

    if (fd < 0 || fd >= XTotal || !XFicheros[fd])
    {
        strcpy(v_memo(5004)->alfabetica,"-1");
        n_np = 3;
        return(0);
    }

    remoto = XFicheros[fd]->Remoto;
    if (XFicheros[fd]->Binario)
    {
        switch(modo)
        {
        case 0:
            c = (unsigned char)(*d);
            resultado = quiza_remoto_rw_write_(XFicheros[fd]->h.fd,&c,1,&remoto);
            break;
        case 1:
            s = (unsigned short)(*d);
            resultado = quiza_remoto_rw_write_(XFicheros[fd]->h.fd,&s,sizeof(s),&remoto);
            break;
        case 2:
            l = (unsigned long)(*d);
            resultado = quiza_remoto_rw_write_(XFicheros[fd]->h.fd,&l,sizeof(l),&remoto);
            break;
        case 3:
            f = (float)(*d);
            resultado = quiza_remoto_rw_write_(XFicheros[fd]->h.fd,&f,sizeof(f),&remoto);
            break;
        default:
            resultado = quiza_remoto_rw_write_(XFicheros[fd]->h.fd,(char *)d,sizeof(double),&remoto);
            break;
        }
    }
    else
    {
        resultado = -1; /* solo binarios */
    }
    
    sprintf(v_memo(5004)->alfabetica,"%d",resultado);
    n_np = 3;
    return(0);
}


extern long quiza_remoto_rw_lseek_();
extern long quiza_remoto_fseek();
int fgv_xposicion(int p,int tip,int faclin)
{
    /* "XPOSICION"     ,  "989898"         ,254,31, */
    int fd = pasa_a_int( v_memo( prog[p++].indice )->numerica );    
    int pd = prog[p++].indice;
    int modo = pasa_a_int( v_memo( prog[p++].indice )->numerica );
    int remoto;
    long resultado;

    if (fd < 0 || fd >= XTotal || !XFicheros[fd])
    {
        strcpy(v_memo(5004)->alfabetica,"-1");
        n_np = 3;
        return(0);
    }

    remoto = XFicheros[fd]->Remoto;

    resultado = (long)v_memo(pd)->numerica;

    if (XFicheros[fd]->Binario)
    {
        resultado = quiza_remoto_rw_lseek_(XFicheros[fd]->h.fd,resultado,modo,&remoto);        
    }
    else
    {
        resultado = quiza_remoto_fseek(XFicheros[fd]->h.f,resultado,modo,&remoto);
    }
    
    v_memo(pd)->numerica = (double)resultado;

    sprintf(v_memo(5004)->alfabetica,"%ld",resultado);

    n_np = 3;
    return(0);
}

int fgv_xleededef(int p,int tip,int faclin)
{
    /* "XLEEDEDEF"     ,  "9849"           ,254,32, */
    int fd = pasa_a_int( v_memo( prog[p++].indice )->numerica );
    int ipfi = v_prog(prog[p++].indice);
    int total = v_mpfi(ipfi)->f_lfich;
    char *datos = v_mpfi(ipfi)->f_buf;
    int resultado;
    int remoto;

    if (fd < 0 || fd >= XTotal || !XFicheros[fd])
    {
        strcpy(v_memo(5004)->alfabetica,"-1");
        n_np = 2;
        return(0);
    }
    
    remoto = XFicheros[fd]->Remoto;
    if (XFicheros[fd]->Binario)
    {
        resultado = quiza_remoto_rw_read_(XFicheros[fd]->h.fd,datos,total,&remoto);
    }
    else
    {
        resultado = -1;
    }
    
    sprintf(v_memo(5004)->alfabetica,"%d",resultado);

    n_np = 2;
    return(0);
}

int fgv_xgrabaadef(int p,int tip,int faclin)
{
    /* "XGRABAADEF"    ,  "9849"           ,254,33, */
    int fd = pasa_a_int( v_memo( prog[p++].indice )->numerica );
    int ipfi = v_prog(prog[p++].indice);
    int total = v_mpfi(ipfi)->f_lfich;
    char *datos = v_mpfi(ipfi)->f_buf;
    int resultado;
    int remoto;

    if (fd < 0 || fd >= XTotal || !XFicheros[fd])
    {
        strcpy(v_memo(5004)->alfabetica,"-1");
        n_np = 2;
        return(0);
    }
    
    remoto = XFicheros[fd]->Remoto;
    if (XFicheros[fd]->Binario)
    {
        resultado = quiza_remoto_rw_write_(XFicheros[fd]->h.fd,datos,total,&remoto);
    }
    else
    {
        resultado = -1;
    }
    
    sprintf(v_memo(5004)->alfabetica,"%d",resultado);
    n_np = 2;
    return(0);


    n_np = 2;
    return(0);
}

  
int fgv_dscorreo_envia(int p,int tip,int faclin)
{
char *servidor;
char *servicio;
char *from;
char *to;
char *subject;
char *message;
char *files;
FILE *f = NULL;
unsigned char tmp[512];
int n;
int i;
int xx;
char version[21];

//aviso(0,"dscorreo_envia -1");

servidor = v_memo( prog[p++].indice )->alfabetica;
servicio = v_memo( prog[p++].indice )->alfabetica;
from    = v_memo( prog[p++].indice )->alfabetica;
to      = v_memo( prog[p++].indice )->alfabetica;
subject = v_memo( prog[p++].indice )->alfabetica;
message = v_memo( prog[p++].indice )->alfabetica;
files   = v_memo( prog[p++].indice )->alfabetica;

//aviso(0,"dscorreo_envia -2");

if (strlen(message) > 3 && message[0] == '$' && message[1] == '$' && message[2] == '$' && message[3] == '$')
{
    quitab(message+4);
    f = fopen(message+4,"r");
    if (f)
    {         
         message = malloc(32000);
         n = 0;
         while(lee_linea(tmp,255,f)) 
         {             

             for (i = 0;tmp[i];i++)
             {
                 xx = (int)(unsigned int)tmp[i];
                 switch(xx)
                 {
                  case 179: /* raya vertical */
                  case 186:                      
                  case 196: /* raya horizontal */
                  case 205:             
                  case 218: /* esquina i-a */
                  case 201:
                  case 214:
                  case 213:             
                  case 194: /* interseccion c-a */
                  case 203:
                  case 210:
                  case 209:             
                  case 191: /* esquina d-a */
                  case 187:
                  case 183:
                  case 184:              
                  case 195: /* interseccion i-c */
                  case 204:
                  case 199:
                  case 198:              
                  case 197: /* interseccion central */
                  case 206:
                  case 215:
                  case 216:              
                  case 180: /* interseccion c-d */
                  case 185:
                  case 182:
                  case 181:              
                  case 192: /* esquina i-abajo */
                  case 200:
                  case 211:
                  case 212:              
                  case 193: /* interseccion c-abajo */
                  case 202:
                  case 208:
                  case 207:              
                  case 217: /* esquina d-abajo */
                  case 188:
                  case 189:
                  case 190:
                      tmp[i] = ' ';
                      break;
                  default:
                      break;
                 }
             }
             strcpy(message+n,tmp);
             strcat(message+n,"\r\n");
             n += strlen(message+n);
             if (n >= 31600) break;
         }
         fclose(f);
    }
}

//aviso(0,"dscorreo_envia -3");
strcpy(version,"[INTERNA]");
//aviso(0,"dscorreo_envia -4");
if (!servidor[0])
{
    sprintf(v_memo(5004)->alfabetica,"%d",dscorreo_local_envia(servicio,from,to,subject,message,files));
}
else
{
    sprintf(v_memo(5004)->alfabetica,"%d",dscorreo_envia(servidor,servicio,from,to,subject,message,files,version));
}
//aviso(0,"dscorreo_envia -5");
strcat(v_memo(5004)->alfabetica," ");
//aviso(0,"dscorreo_envia -6");
strcat(v_memo(5004)->alfabetica,version);
//aviso(0,"dscorreo_envia -7");
if (f)
{
    free(message);
}
//aviso(0,"dscorreo_envia -fin");
n_np = 7;
return(0);
}




int fgv_dscorreo_recibe(int p,int tip,int faclin)
{
char *servidor = v_memo( prog[p++].indice )->alfabetica;
char *servicio = v_memo( prog[p++].indice )->alfabetica;
char *usuario  = v_memo( prog[p++].indice )->alfabetica;
char *password = v_memo( prog[p++].indice )->alfabetica;
char *path = v_memo( prog[p++].indice )->alfabetica;
int np = prog[p++].indice;
char buf_r[300];
char sujeto[2048];
char version[30];

strcpy(buf_r,v_memo( np )->alfabetica);

sujeto[0] = 0;
if (!servidor[0])
{
	strcpy(version,"[INTERNA]");
    sprintf(v_memo(5004)->alfabetica,"%d ",dscorreo_local_recibe(servicio,usuario,password,path,buf_r,sujeto));
}
else
{
    sprintf(v_memo(5004)->alfabetica,"%d ",dscorreo_recibe(servidor,servicio,usuario,password,path,buf_r,sujeto,version));
}

memo_strcpy(np,buf_r);

sujeto[230] = 0;
strcat(v_memo(5004)->alfabetica,version);
strcat(v_memo(5004)->alfabetica," ");
strcat(v_memo(5004)->alfabetica,sujeto);

n_np = 6;
return(0);
}

int fgv_browse(int p,int tip,int faclin)
{
    char tmp[512];
    int m = prog[p++].indice ;
    strcpy(tmp,v_memo(m )->alfabetica);
    if (DespachadorExterno)
    {
        (*DespachadorExterno)((int)DESPACHO_BROWSE,(int)0,tmp);
    }
    memo_strcpy(m,tmp);
n_np = 1;
return (0);
}

FI_DEF *pfi_llamada_erp = NULL;
int consulta_ficha_erp(char *nomdef)
{
	FI_DEF *pfi = NULL;
	FI_DEF *pfil = NULL;
	int ipfi,ipfil;
	int r,i,j;
	char tmp[512];	
	/*int camlinea,relaci;*/
	extern int flag_leedefn;

	if (nomdef[0] == ':')
	{
		strcpy(tmp,quita_path(nomdef));
		for(i = 0;tmp[i];i++)
		{
			if (tmp[i] == '.' || tmp[i] == ' ')
			{
				tmp[i] = 0;
				break;
			}
		}
		ipfi = ind_rel(tmp);
		if (ipfi >= 0)
		{
			pfi_llamada_erp = v_mpfi(ipfi);
			entra_int(nomdef,0);
			pfi_llamada_erp = NULL;
		}
		return 0;
	}	
		
	ipfi = ind_rel(nomdef);
	if (ipfi < 0)
		return -1;	

	flag_leedefn = 2;
	r = lee_defn(v_mpfi(ipfi)->f_PathDef,&pfi);
	if (r < 0)
		return -1;


    if (nfi == 48) nfi++;
    mpfi[nfi] = pfi;
    nfi++;
	inicializa_este_def(nomdef,v_mpfi(ipfi)->f_PathDef,nfi-1,0);
	memcpy(pfi->f_fichero,v_mpfi(ipfi)->f_fichero,sizeof(pfi->f_fichero));
	for (i = 0;i < pfi->f_npanta;i++)
	{
		memcpy(pfi->f_pantalla[i].p_nombrepan,v_mpfi(ipfi)->f_pantalla[i].p_nombrepan,sizeof(pfi->f_pantalla[i].p_nombrepan));
	}

	ipfi = nfi-1;

    if (!hay_dat_permiso(v_mpfi(ipfi),-1))
       return(-1);
	if (!hay_dat_permiso(v_mpfi(ipfi),-2))
	   return -1;	

#ifdef CHURROLINEAS	
	relaci = -1;
	camlinea = 0;
	for (i = 0;i < pfi->f_nrelaci && relaci == -1;i++)
	{
		for (j = 0;j < MAXCAMPOSPORCLAVE;j++)
		{
			if (!pfi->f_relaci[i].r_campo[j])
				break;
			if (pfi->f_relaci[i].r_campo[j] < 0)
			{	
				relaci = i;
				camlinea = pfi->f_relaci[i].r_campo[j];
				break;
			}
		}
	}
	if (relaci > -1)
	{
		/* hay lineas ... */
		strcpy(tmp,pfi->f_PathDef);
		strcpy(quita_path(tmp),quita_path(pfi->f_relaci[relaci].r_fichero));
		flag_leedefn = 2;
		r = lee_defn(tmp,&pfil);
		if (r >= 0)
		{
			if (nfi == 48) nfi++;
			mpfi[nfi] = pfil;
			ipfil = nfi;
			nfi++;
			inicializa_este_def(quita_path(pfi->f_relaci[relaci].r_fichero),pfil->f_PathDef,nfi-1,0);
			memcpy(tmp,v_mpfi(ipfi)->f_fichero,sizeof(pfil->f_fichero));
			strcpy(quita_path(tmp),quita_path(pfil->f_fichero));
			memcpy(pfil->f_fichero,tmp,sizeof(pfil->f_fichero));
			/* falta path pantallas lineas */

			/* RELAS de la cabecera */
			mpfirel[ipfi]->lineas = 0;

            mpfirel[ipfi]->nrel = 1;
            mpfirel[ipfi]->relacis = (short *)malloc(sizeof(short));
            mpfirel[ipfi]->camrela = (short *)malloc(sizeof(short));
            mpfirel[ipfi]->camminu = (short *)malloc(sizeof(short));
            mpfirel[ipfi]->numrela = (short *)malloc(sizeof(short));
			mpfirel[ipfi]->numrela[0] = relaci;
			mpfirel[ipfi]->camrela[0] = 0;
			mpfirel[ipfi]->camminu[0] = camlinea;
			mpfirel[ipfi]->relacis[0] = ipfil;

			/* RELAS Lineas  Nos inventamos las "D" de momento */
			mpfirel[ipfil]->lineas = (camlinea - 1)*160 - 22*6 - 5;
			for (i = 0;i < pfil->f_nrelaci;i++)
			{
				if (!strcmp(quita_path(pfil->f_relaci[i].r_fichero),quita_path(pfi->f_relaci[relaci].r_fichero)) || ind_rel(quita_path(pfil->f_relaci[i].r_fichero)) == ipfi)
				{
					mpfirel[ipfil]->nrel = 1;
					mpfirel[ipfil]->relacis = (short *)malloc(sizeof(short));
					mpfirel[ipfil]->camrela = (short *)malloc(sizeof(short));
					mpfirel[ipfil]->camminu = (short *)malloc(sizeof(short));
					mpfirel[ipfil]->numrela = (short *)malloc(sizeof(short));
					mpfirel[ipfil]->numrela[0] = i;
					mpfirel[ipfil]->camrela[0] = 0;
					mpfirel[ipfil]->camminu[0] = 0;
					mpfirel[ipfil]->relacis[0] = ipfi;

					mpfirel[ipfil]->reldef = (REL0_DEF *)malloc(sizeof(REL0_DEF) * MAXCAMPOSPORCLAVE);
					for (j = 0;j < MAXCAMPOSPORCLAVE;j++)
					{
					    if (!pfil->f_relaci[i].r_campo[j])
						    break;
						mpfirel[ipfil]->nreldef = j+1;
						mpfirel[ipfil]->reldef[j].campo = pfil->f_clave[0].k_campo[j]-1;
						mpfirel[ipfil]->reldef[j].nfirel = ipfi;
						mpfirel[ipfil]->reldef[j].nficampo = pfil->f_relaci[i].r_campo[j]-1;
						mpfirel[ipfil]->reldef[j].numrela = i;
					}					
					break;
				}
			}
		}
	}
#endif


	  Bl(24);
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
    
	return 0;
}
