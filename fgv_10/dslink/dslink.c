
#include <fgvsys.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef O_BINARY

#include <io.h>

#else

#define ES_UNIX

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

#include <signal.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <time.h>
#include <sys/timeb.h>

/*#include <rwmalloc.h>*/
#include <rtermin.h>
#include <rdef.h>
#include <teclas.h>
#include <rfecha.h>
#include <rentra.h>
#define _GENERADOR
#include <rmante.h>
#include <rcalcul.h>
#include <rcompi.h>


extern int  indice_def_local[48];
extern int  nindice_def_local;
extern int en_ini_par;
extern int compilando_un_prc;
extern int minimun;
extern short fini_par[MAXDEF];
extern int c_linea;
extern FILE *c_error;
extern int e_nivel;
extern int c_nerror;
extern int error_break;
extern char *tabla_defbucle[MAX_BUCLE];

extern int oculto;
extern int primero;

extern char *nombre_defs[MAXDEF*2];
extern char ventana_defs[MAXDEF*2];

extern char dirbase[51];
extern char dirbass[51];
extern char dirbase_[51];

extern unsigned short tam_gene_def[MAXDEF];
extern unsigned short def_varmemo;
extern int cont_defs;

extern int ya_esta_avisado;

extern long numero_generacion;
extern int modo_gen_antiguo;

extern unsigned short tipo_de_apli;

extern FI_DEF *mpfi_maximo;

extern long nume_serie;

extern unsigned short local_term[52];
extern unsigned short local_term_[52];

extern short _fgv_version;
extern short i3;

unsigned short _Posicion();


extern char notacion_dir_aplicacion[256];



extern char c_nombrecal[300];


/* ahorrar memoria luego!!!*/
static int nlista_lib = 0;
static char lista_lib[3000][50];


int donde_esta(pfi,camp)
FI_DEF *pfi;
int camp;
{
int i,pan,fpan,ac2;

if (camp < 1) return(camp);

if (pfi->f_campos[camp-1]->c_mante->c_visual) /* si no visualizable no entry */
   return(0);

if (pfi->f_campos[camp-1]->c_key == 1) { /* en clave primaria */

   /* orden segun clave primaria */
   for (i = 0;i < pfi->f_clave[0].k_ncampos;i++) {
      if (pfi->f_clave[0].k_campo[i] == camp)
	 return(i+1);
      }
   un_error("ERROR i0004 EN DEF",pfi->f_fichero);
   return(0);
   }

for (i = 0;i<pfi->f_npanta;i++) {
  fpan = pfi->f_pantalla[i].p_campofin;
  for (pan = pfi->f_pantalla[i].p_campoini - 1;pan < fpan;pan ++) {
     ac2 = (pfi->f_campos[pan]->c_mante->c_actual % 1000) + 1;
	  if (ac2 == camp) {
	return(pan+31); /* los que no estan en clave +30 */
	}
     }
  }
return(0);
}


int ini_par()
{
FI_DEF *ifi;
int i,irel,frel,l,j,mayor,menor,cmayor,cmenor,kk;
char tmp[81];
char fichero[81];
char path[81];
char prueba;
int ordenado,oo,encl;

fini_par[primero] = 0;


for (irel = primero;irel < nfi;irel++) {

   if (irel == 48) continue;

   if (minimun && irel >= 4) break;

   for (i=0;i<vg_mpfi(irel)->f_nclaves;i++) {
      for (j = 0;j < vg_mpfi(irel)->f_clave[i].k_ncampos;j++) {
	 if (vg_mpfi(irel)->f_clave[i].k_campo[j] > vg_mpfi(irel)->f_ncampos ||
	     vg_mpfi(irel)->f_clave[i].k_campo[j] < 1) {
	     un_error("Campo ilegal en CLAVE","");
	     if (error_break)
		return(0); /* abandonar */
	     }
	 }
      }

   if (error_break)
       return(0); /* abandonar */

/*   if (irel == 15) {
      if (c_c_c != i_c_c)
	 exit(0);
      }
*/
   mpfirel[irel]->nrel    = 0;
   mpfirel[irel]->nrelact = 0;
   mpfirel[irel]->nreldef = 0;
   mpfirel[irel]->nrelpin = 0;

   if (nombre_defs[irel] != quita_path(nombre_defs[irel])  && nombre_defs[irel][0] == ':' && nombre_defs[irel][1] != '/') {
      kk = *(quita_path(nombre_defs[irel]));
      *(quita_path(nombre_defs[irel])) = 0;
      strcpy(path,nombre_defs[irel]);
      *(quita_path(nombre_defs[irel])) = kk;
      }
   else
      path[0] = 0;
   for (i=0;i<vg_mpfi(irel)->f_nrelaci;i++) {
      strcpy(fichero,vg_mpfi(irel)->f_relaci[i]->r_fichero);
      if (fichero[0] == '\0' || nfi >= MAXDEF) continue;

		if (fichero == quita_path(fichero)) {
	 strcatd(fichero,path);
	 }

      menor = 10000;
      mayor = 0;
      cmenor = 10000;
      cmayor = 0;
      encl = 0;
      for (j=0;j<(MAXCAMPOSPORCLAVE+1);j++) {
	 if (vg_mpfi(irel)->f_relaci[i]->r_campo[j] == 0) continue;

	 if (vg_mpfi(irel)->f_relaci[i]->r_campo[j] > vg_mpfi(irel)->f_ncampos) {
	    un_error("Campo ilegal en Relacion","");
	    continue;
	    }

	 ordenado = donde_esta(vg_mpfi(irel),vg_mpfi(irel)->f_relaci[i]->r_campo[j]);

	 if (ordenado != 0 && cmayor < ordenado) {
	    cmayor = ordenado;
	    mayor = vg_mpfi(irel)->f_relaci[i]->r_campo[j];
	    }
	 if (ordenado != 0 && cmenor > ordenado) {
	    cmenor = ordenado;
	    menor = vg_mpfi(irel)->f_relaci[i]->r_campo[j];
	    }
	 if (encl == 0) {
	   encl = vg_mpfi(irel)->f_relaci[i]->r_campo[j];
	   if (encl > 0)
			encl = vg_mpfi(irel)->f_campos[encl-1]->c_key;
	   else
	      encl = 0;
	   }
	 }
      if (fini_par[irel] == -1 && cmenor > 0 && encl == 0) continue;

      /* quitado temporalmente por asunto de contadores relacionados */
      /*
      if (memcmp(vg_mpfi(irel)->f_relaci[i]->r_fichero+1,"F:",2) == 0 && irel > 0)
	  continue;
      */
      if (memcmp(vg_mpfi(irel)->f_relaci[i]->r_fichero+1,"F:",2) == 0 && fini_par[irel] == -1)
	  continue;

      l = mpfirel[irel]->nrel;
	  if (l >= MAXRELAS)
		   {		   
		   un_error("LIMITE INTERNO DE 'MAXRELAS' SUPERADO EN nrel !!INESTABILIDAD!!",vg_mpfi(i)->f_titulo);
		   }


      if (menor < 0) mpfirel[irel]->lineas  = 0;

      mpfirel[irel]->camrela[l] = (mayor - 1); /* la relacion se activa */
      mpfirel[irel]->numrela[l] = i;
      mpfirel[irel]->camminu[l] = (menor - 1);
      if (memcmp(vg_mpfi(irel)->f_relaci[i]->r_fichero+1,"F:",2) == 0) {
	 memcpy(fichero,vg_mpfi(irel)->f_relaci[i]->r_fichero+3,8);
	 fichero[8] = '\0';
	 quitab(fichero);
	 strminus(fichero);
	 }

      if (menor < 0)
	frel = ind_def_rel(fichero);
		else
	frel = ind_rel(fichero);

		if (frel == -1) {
			if (minimun && nfi >= 4)
				{
				aviso(0,"EXCESO DE FICHEROS");
            continue;
				}

      ifi = (FI_DEF *)malloc(sizeof(FI_DEF));
	  memset(ifi,0,sizeof(FI_DEF));
      ventana_defs[nfi] = 1;
      if (lee__def(fichero,ifi,1,2) < 0) {
       ventana_defs[nfi] = 0;
       if (lee__def(fichero,ifi,1,1) < 0) {
	 free((char *)ifi);
	 sprintf(tmp,"Fichero '%s' inaccesible o inexistente"
		    ,fichero);
	 aviso(0,tmp);
	 continue;
	 }
       else {
	 pon_v_nombre(nfi,fichero,1);
	 }
       }
      else {
       pon_v_nombre(nfi,fichero,2);
       }

      mpfi[nfi] = ifi;
      comprueba_def(fichero);

      mpfirel[nfi] = (RELAS *)malloc(sizeof(RELAS));
      mflagfi[nfi] = RCERRADO;
      frel = nfi;
		mpfirel[frel]->lineas = -1;
      mpfirel[frel]->clave = -1;
      if (vg_mpfi(irel)->f_relaci[i]->r_leer != 0 || fini_par[irel] != 0) {
	  fini_par[nfi] = -1;
	  libera_extras(vg_mpfi(nfi));
	  }
       else
	  fini_par[nfi] = 0;
      nfi++;
      if (nfi == 48) nfi++;
      }
      if (menor < 0) mpfirel[frel]->lineas  = (menor - 1)*160 - 22*6 - 5;
      if (memcmp(vg_mpfi(irel)->f_relaci[i]->r_fichero+1,"F:",2) == 0) {
	 oo = (vg_mpfi(irel)->f_relaci[i]->r_fichero[0] - '0');
	 oo = (oo * 10000);oo = (oo + 9900);
	 frel = (frel + oo);
	 frel = (-frel);
	 }
      mpfirel[irel]->relacis[l] = frel;
      mpfirel[irel]->nrel++;             /* con el ultimo campo   */
      }
   }
return(0);
}


static reset_prog()
{
int i,j;

for (j=0;j < totalfichs;j++) {
   free(fichs[j]->fichero);
   free((char *)fichs[j]);
   }
totalfichs = 0;

for (i=0;i<np_ex;i++) {
	free(p_ex[i]->nombre);
   free((char *)p_ex[i]);
   }
np_ex = 0;
for (i=0;i<nm_ex;i++) {
   free(m_ex[i]->nombre);
   free((char *)m_ex[i]);
   }
nm_ex = 0;

for (i=0;i<ncond;i++)
   free((char *)cond[i]);
for (i=0;i<nbucl;i++)
   free((char *)bucl[i]);
for (i=0;i<noper;i++)
   free((char *)oper[i]);
for (i=0;i<nmemo;i++) {
   if (que_memo_tipo(i) == CHARTIPO || que_memo_tipo(i) == FECHATIPO)
      free(memo[i]->mv.valc);
   free((char *)memo[i]);
   }

for (i=0;i<nf_B;i++) {
   free((char *)f_B[i]->ord1);
   free((char *)f_B[i]->calc);
   if (f_B[i]->ncomp > 0)
      free((char *)f_B[i]->comp);
   if (f_B[i]->nord2 > 0)
      free((char *)f_B[i]->ord2);
   free((char *)f_B[i]);
	}

for (i = 0;i < nindice_def_local;i++)
   {
   indice_def_local[i] = -1;
   }
nindice_def_local = 0;

nprog = 0;
ncond = 0;
nbucl = 0;
noper = 0;
nmemo = 0;
nf_B = 0;
reset_tabla();
return(0);
}

int comprueba_prog(ipfi,nom)
int ipfi;
char *nom;
{
char nombre[81];
int file1;
long num_gen;
short i = 0,k,nex,pex,yaesta,total,kk;
char c;
short indice,fini;
double tmpd;
long salva_p;
long salva_p2;
char salvacal[300];
/* avisar de incongruencias en los tipos de variables externas */
/* para los casos en que ya estando generado el proceso el tipo no
	corresponda */


/* primero comprobar que no haya sido cargado antes */
for (i = 0;i < nen_pr;i++)
  {
  if (!strcmp(en_pr[i].prc,quita_path(nom)))
	  return(0);
  }

strcpy(nombre,nom);
strcat(nombre,".prc");
if ((file1 = rw_open_(nombre,O_RDWR | O_BINARY,
			S_IREAD | S_IWRITE)) < 0) {
    return(-1);
	 }
rw_read_(file1,&i,sizeof(short));
moto_short(&i);
if (i != FGVGEN_ID_PRC) {rw_close_(file1);return(-1);}

memcpy(salvacal,c_nombrecal,300);
strcpy(c_nombrecal,nom);

rw_write_(file1,&numero_generacion,sizeof(long));


rw_lseek_(file1,26L,SEEK_SET);
rw_read_(file1,&(en_pr[nen_pr].e_nprog),sizeof(short));
rw_read_(file1,&(en_pr[nen_pr].e_nmemo),sizeof(short));
rw_read_(file1,&(en_pr[nen_pr].e_ncond),sizeof(short));
rw_read_(file1,&(en_pr[nen_pr].e_nbucl),sizeof(short));
rw_read_(file1,&(en_pr[nen_pr].e_noper),sizeof(short));
rw_read_(file1,&(en_pr[nen_pr].e_nf_B),sizeof(short));
rw_read_(file1,&nex,sizeof(short));
rw_read_(file1,&pex,sizeof(short));
rw_read_(file1,&k,sizeof(short));
en_pr[nen_pr].e_nex = nex;

/* leer ficheros */

salva_p = rw_lseek_(file1,0L,SEEK_CUR);
for (i = 0;i < k;i++)
   {
   rw_read_(file1,&indice,sizeof(short));
   rw_read_(file1,&fini,sizeof(short));
   rw_read_(file1,&c,1);
   rw_read_(file1,nombre,c);
   if (fini >= 2000)
      {
      fini %= 2;
      }
   /* pon_fichero(nombre,indice,fini,ipfi); */
   }
/* variables externas */
kk = k;

k = nm_ex;
for (i = 0;i < nex;i++)
   {
   m_ex[nm_ex] = malloc(sizeof(M_EX));
   rw_read_(file1,(char *)m_ex[nm_ex],sizeof(M_EX));
   nm_ex++;
   }
for (i = k;i < nm_ex;i++) 
   {
   rw_read_(file1,&c,1);
   m_ex[i]->nombre = malloc(c);
   rw_read_(file1,m_ex[i]->nombre,c);

   if (m_ex[i]->m_tipo == CHARTIPO || m_ex[i]->m_tipo == FECHATIPO)
      {
      rw_read_(file1,&c,1);
      m_ex[i]->mv.valc = malloc(c);
      rw_read_(file1,m_ex[i]->mv.valc,c);
      }
   if ((yaesta = busca_ex(m_ex[i]->nombre)) != -1)
      {
		if (m_ex[i]->m_tipo != global_ex[yaesta]->m_tipo)
         {
         un_error("DIFERENTE DEFINICION DE UNA MISMA VARIABLE EXTERNA",m_ex[i]->nombre);
         }
	  m_ex[i]->indice_global = 5000 + memoplin + maxpila*2 + yaesta;
      }
	else
      {
      global_ex[nglobal_ex] = (M_EX *)malloc(sizeof(M_EX));
      global_ex[nglobal_ex]->nombre = malloc(strlen(m_ex[i]->nombre)+1);
      strcpy(global_ex[nglobal_ex]->nombre,m_ex[i]->nombre);
      global_ex[nglobal_ex]->m_tipo = m_ex[i]->m_tipo;
      if (m_ex[i]->m_tipo == DOUBLETIPO)
         {
         tmpd = m_ex[i]->mv.vald;
         global_ex[nglobal_ex]->mv.vald = tmpd;
         }
      else
         {
         global_ex[nglobal_ex]->mv.valc = malloc(strlen(m_ex[i]->mv.valc)+1);
         strcpy(global_ex[nglobal_ex]->mv.valc,m_ex[i]->mv.valc);
         }
      global_ex[nglobal_ex]->indice_global = 0;
	  m_ex[i]->indice_global = 5000 + memoplin + maxpila*2 + nglobal_ex;
      nglobal_ex++;
      }
   }

salva_p2 = rw_lseek_(file1,0L,SEEK_CUR);
rw_lseek_(file1,salva_p,SEEK_SET);
for (i = 0;i < kk;i++)
   {
   rw_read_(file1,&indice,sizeof(short));
   rw_read_(file1,&fini,sizeof(short));
   rw_read_(file1,&c,1);
   rw_read_(file1,nombre,c);
   if (fini >= 2000)
      {
      fini %= 2;
      }
   pon_fichero(nombre,indice,fini,ini_par);
   }

rw_lseek_(file1,salva_p2,SEEK_SET);


rw_read_(file1,&i,sizeof(short));

/* programas */

if (np_ex) 
{
    un_error("aviso interno np_ex-2","");
}
for (i = 0;i < pex;i++) {
   p_ex[np_ex] = malloc(sizeof(P_EX));
   rw_read_(file1,(char *)p_ex[np_ex],sizeof(P_EX));
   moto_short(&(p_ex[np_ex]->indice)); /* indice relativo a nprog */
   np_ex++;
   }

for (i = 0;i < np_ex;i++) {
   rw_read_(file1,&c,1);
   p_ex[i]->nombre = malloc(c);
   rw_read_(file1,p_ex[i]->nombre,c);

   total = 0;
   for (k = 0;k < totalprogr;k++) {
    if (strcmp(progr[k]->nombre,quita_path(p_ex[i]->nombre)) == 0) {
      un_error("nombre de proceso o rutina DUPLICADO",p_ex[i]->nombre);
      break;
      }
	 if (progr[k]->tipo != -1)
      total++;
    }

   if (totalprogr >= 1500) {
     un_error("EXCESO DE PROCESOS-RUTINAS","");
     rw_close_(file1);
	 memcpy(c_nombrecal,salvacal,300);
     return(0);
     }

   if (total >= 1469) {
	  un_error("EXCESO DE PROCESOS","");
     rw_close_(file1);
	 memcpy(c_nombrecal,salvacal,300);
     return(0);
     }

   progr[totalprogr] = (PP *)malloc(sizeof(PP));
   progr[totalprogr]->nombre = malloc(strlen(p_ex[i]->nombre)+1);
   strcpy(progr[totalprogr]->nombre,p_ex[i]->nombre);
   progr[totalprogr]->indice = p_ex[i]->indice;
   progr[totalprogr]->tipo = p_ex[i]->tipo;
   progr[totalprogr]->contador = 0;
	progr[totalprogr]->proceso = nen_pr;
   progr[totalprogr]->exportable = p_ex[i]->exportable;
   totalprogr++;
   }

rw_read_(file1,&(en_pr[nen_pr].nllamada_interna),sizeof(short));
moto_short(&(en_pr[nen_pr].nllamada_interna));
for (i = 0;i < en_pr[nen_pr].nllamada_interna;i++)
  {
   rw_read_(file1,&c,1);
   /*llamada_interna[i] = malloc(c);*/
   if (c >= 30) un_error("Nombre de proceso demasiado largo","");
   rw_read_(file1,en_pr[nen_pr].llamada_interna[i],c);
  }

rw_close_(file1);
nen_pr++;
memcpy(c_nombrecal,salvacal,300);
return(0);
}


static int busca_prog(nom)
char *nom;
{
int i;

if (!strcmp(nom,"PROGRAMA"))  {
   un_error("PROGRAMA no es accesible!","");
   return(-1);
   }
quita_blancos(nom); /* esto puede ser innecesario:  por optimizar */
if (strcmp(nom,"NULL") == 0) return(-1);
for (i = 0;i < totalprogr;i++) {
   if (strcmp(nom,progr[i]->nombre) == 0)
      return((i+1));
   }
return(-1);
}


static int fin_compilacion()
{
	int j;
	
	for (j=0;j < totalgosub;j++) {
		free(gosub[j]->nombre);
		free((char *)gosub[j]);
	}
	totalgosub = 0;
	
	for (j=0;j < totalmemos;j++) {
		free(memos[j]->nombre);
		free((char *)memos[j]);
	}
	totalmemos = 0;
	for (j=0;j < totalcamps;j++) {
		free(camps[j]->nombre);
		free((char *)camps[j]);
	}
	totalcamps = 0;
	return(0);
}



int tabla_cal(nom,c,ipfi)
char *nom;
int c;
int ipfi;
{
int indice = 0,i,kk;
char nombre[81];
char nombre2[81];
char *pr;

quita_blancos(nom);
if (c == 999) {
	pr = nombre_defs[ipfi];
	if (!pr)
	  pr = "?";
   if (pr != quita_path(pr) && *pr == ':' && *(pr+1) != '/') {
      strcpy(nombre,dirbase_);
      kk = *(quita_path(pr));
      *(quita_path(pr)) = 0;
      strcat(nombre,pr+1);
      *(quita_path(pr)) = kk;
      }
   else {
      strcpy(nombre,dirbase);
      }
  strcpy(nombre2,nombre);
  strcat(nombre,"cal/");
  strcat(nombre2,"bin/");
  strcat(nombre2,nom);
  strcat(nombre,nom);

  if (!memoplin) 
     {
	 pon_varsistem();
     }

  strcpy(en_pr[nen_pr].prc,quita_path(nombre2));
  
  compilando_un_prc++;
  if (comprueba_prog(ipfi,nombre2))
     {
	 un_error("No se accede",nombre2);
	 }   
  fin_compilacion();
  compilando_un_prc--;  
  reset_prog();
  }
else {
    if ((indice = busca_prog(nom)) < 0)
    {
        for (kk = 0;kk < nlista_lib;kk++)
        {
            if (!strcmp(nom,lista_lib[kk]))
            {
                  tabla_cal(lista_lib[kk]+25,999,0);
                  break;
            }
        }
        indice = busca_prog(nom);
    }

  if (indice >= 0 && indice <= totalprogr) 
  {
	  i = indice;
	  if (progr[i-1]->tipo == -1) {
	un_error("ilegal uso de una rutina",nom);
	return(-1);
	}
	  if (c == 998)  { /* OJO es contador */
	indice = numero_contador(i);
	if (indice < 0) {
	   un_error("Exceso de contadores","");
	   }
	}
	  else {
	indice = numero_prog(i); /* real en tabla_cal */
	}
     }
  else {
     indice = -1;
	  }
  }
return(indice);
}

secierra(tok)
char *tok;
{
   if (   tok[0] == '\0'
       || tok[strlen(tok)-1] != STRLIMITA
       || (strlen(tok) >= 2 && tok[strlen(tok)-2] == STRMETACAR)
      )
      return(-1);
   return(0);
}


char *leestring(qo,tok,prime)
char *qo;
char *tok;
int prime;
{
   int c;
   char *aux;
   char *aux2;

   aux = malloc(81);
   strcpy(aux,qo+comas_linea(qo,prime,1,(int *)0));

   if (aux[0] == STRLIMITA) {
      c = CIERTO;
      strcpy(tok,aux+1);
      }
   else {
      c = FALSO;
      strcpy(tok,aux);
      }
   if (c == CIERTO) {
      if (secierra(tok) < 0) {
	 do {
	    strcpy(aux,qo+comas_linea(qo,1,1,(int *)0));
	    strcat(tok,",");
	    strcat(tok,aux);
	    } while (aux[0] != '\0' && secierra(aux) < 0);
	 if (aux[0] == '\0') {
	    free(aux);
	    return(NULL);
	    }
	 }
      tok[strlen(tok)-1] = '\0';
      }
   free(aux);
   for (aux=tok;*aux != '\0';aux++)
      if (*aux == STRMETACAR)
	 for (aux2=aux;*aux2 != '\0';aux2++)
	    *aux2 = *(aux2+1);
   quitab(tok);    /* quita blancos de detras  */
   quitabp(tok);   /* quita blancos de delante */
   return(tok);
}



ini_rel()
{
int i,j,f,c1,c2,qp,pare,fill,l,z,lug;
char *tmp = NULL;
char pin[MAXRELACION1+10];
char tem[60],*tmp2;
char para1[60];
char *_n[5];
int ocur,lll,oc,nnn,frel;
int proceso_creado =0;


for (i=0;i < nfi;i++) {

    if (i == 48) continue;

	if (minimun && i >= 4) break;

	for (j=0;j<vg_mpfi(i)->f_ncampos;j++) {
	  if (error_break)
		 {
		 if (tmp)
			free(tmp);
		 return(0); /* abandonar */
		 }

	  if (tmp)
		 free(tmp);
	  tmp = malloc(strlen(vg_mpfi(i)->f_campos[j]->c_mante->c_relacion)+1);
	  strcpy(tmp,vg_mpfi(i)->f_campos[j]->c_mante->c_relacion);
#ifdef COMENTARIO
	  if (strbusca(tmp,"ENLINEA") > -1) /* esto permite la inclusion de procesos de limite */
		 {
		 fini_par[i] = 0;
		 }
#endif
	  leestring(tmp,pin,0);
	  while (pin[0] != '\0') {
#ifdef COMENTARIO
	if (!memcmp(pin,"ENLINEA",7)) {
	  if (pin[7] != ' ')
	  {
	     un_error("La sintaxis es:ENLINEA",pin);
	  }
	  else
	  {
	   if (mpfirel[i]->clave != -1) {
	     un_error("ENLINEA ilegal",pin);
	     }
	   else {
	     crea_cal_enlinea(i,pin);
	     }
	   }
	  }
	else
#endif
	if (pin[0] == 'L') 
	{
	   lll = atoi(pin+1);
		if (strstr(pin+1,"X"))
		{			
			if (mpfirel[i]->lineas < -1)
			{
				mpfirel[i]->lineas = -(((lll%100)*160)+((lll/100)*6)+4);
			}
			else
			{
				if (mpfirel[i]->lineas == -1)
				{
					mpfirel[i]->lineas = lll;
				}
			}
		}
		else
		{
		   nnn = lll/100;if (nnn == 0) nnn = 5; else nnn -= 1;
		   lll %= 100;
			   if (mpfirel[i]->lineas < -1)
			   {
				  mpfirel[i]->lineas = (mpfirel[i]->lineas/160)*160 - lll*6 - nnn;
			   }
			   else
			   {
				   if (mpfirel[i]->lineas == -1)
				   {
					   mpfirel[i]->lineas = atoi(pin+1);
				   }
			   }
		}
	   }
	else
	if (pin[0] == 'I') {
	   l = mpfirel[i]->nrelpin;
		if (l >= MAXRELAS)
		   {
		   sprintf(tmp2,"LIMITE INTERNO DE 'MAXRELAS' SUPERADO EN nrelpin !!INESTABILIDAD!!");
		   un_error(tmp2,vg_mpfi(i)->f_titulo);
		   }
		mpfirel[i]->relpin[l].campo = 0;
	   mpfirel[i]->relpin[l].qp = -1;
	   mpfirel[i]->relpin[l].nfirel = -1;
	   mpfirel[i]->relpin[l].nficampo = -1;
	   mpfirel[i]->relpin[l].numrela = j;
	   mpfirel[i]->nrelpin++;
	   }
	else
	if (pin[0] == 'K') {
	   if (mpfirel[i]->clave != -1) {
	      un_error("NO SE ADMITEN MAS CAMPO CLAVE","");
	      }
	   else
	      mpfirel[i]->clave = j;
	   }
	else
	if (pin[0] == 'D') {
	   lll = strlen(pin+4);
	   if (lll == 9) {
	      ocur = pin[12] - '0';
	      pin[12] = '\0';
	      if (ocur < 1 || ocur > 9) ocur = 1;
	      }
	   else
	      ocur = 1;
	   strminus(pin+4);
	   f = ind_def_rel(pin+4);
	   if (f == -1)
	      f = ind_rel(pin+4);
	   pin[4]='\0';
		c1 = atoi(pin+1);
	   c1--;
	   if (f != -1) {
	     if (c1 < 0 || c1 >= vg_mpfi(f)->f_ncampos) {
		tmp2 = malloc(80);
		sprintf(tmp2,"Defecto de campo ilegal campo %d fichero",j);
		un_error(tmp2,vg_mpfi(i)->f_titulo);
		free(tmp2);
		}
	     else {
		l = mpfirel[i]->nreldef;
		if (l >= MAXRELAS)
		   {
		   sprintf(tmp2,"LIMITE INTERNO DE 'MAXRELAS' SUPERADO EN nreldef !!INESTABILIDAD!!");
		   un_error(tmp2,vg_mpfi(i)->f_titulo);
		   }
		mpfirel[i]->reldef[l].campo = j;
		mpfirel[i]->reldef[l].nfirel = f;
		mpfirel[i]->reldef[l].nficampo = c1;
		mpfirel[i]->reldef[l].numrela = -1;
		oc = 0;
		for (z=0;z<vg_mpfi(i)->f_nrelaci;z++) {
			if (strcmp(quita_path(vg_mpfi(i)->f_relaci[z]->r_fichero),
		      quita_path(vg_mpfi(f)->f_fichero)) == 0) {
		      oc++;
		      if (oc == ocur) {
			 mpfirel[i]->reldef[l].numrela = z;
			 break;
			 }
		       }
		   }
		mpfirel[i]->nreldef++;
		}
	     }
	   else {
		  if (fini_par[i] != -1) {
		tmp2 = malloc(80);
		sprintf(tmp2,"Defecto de fichero inexistente campo %d fichero",j);
		un_error(tmp2,vg_mpfi(i)->f_titulo);
		free(tmp2);
		}
	     }
	   }
	else
	if (pin[0] == 'P') {
	   lll = strlen(pin+11);
	   if (lll == 9) {
	      ocur = pin[19] - '0';
	      pin[19] = '\0';
	      if (ocur < 1 || ocur > 9) ocur = 1;
	      }
	   else
	      ocur = 1;
	   strminus(pin+11);
	   f = ind_def_rel(pin+11);
	   if (f == -1)
	       f = ind_rel(pin+11);
	   if ( f != -1) {
	      pin[11]='\0';
	      c1 = atoi(pin+8);

	     if (c1 < 1 || c1 > vg_mpfi(f)->f_ncampos) {
		tmp2 = malloc(80);
		sprintf(tmp2,"Pintar campo ilegal campo %d fichero",j);
		un_error(tmp2,vg_mpfi(i)->f_titulo);
		free(tmp2);
		goto seguir;
		}

	      pin[8]='\0';
	      qp = atoi(pin+4);

	     if (qp < 101 || qp > 2480) {
		tmp2 = malloc(80);
		sprintf(tmp2,"Pintar posicion ilegal campo %d fichero",j);
		un_error(tmp2,vg_mpfi(i)->f_titulo);
		free(tmp2);
		goto seguir;
		}

	      pin[4] = '\0';
	      c2 = atoi(pin+1);
	      pare = i;fill = f;
	      if (c1 == 0) {
		pare = f;
		fill = i;
		c1 = j + 1;
		}
	      if (c2 == 0)
		c2 = j + 1;
	      c1--;
	      c2--;
		l = mpfirel[pare]->nrelpin;
		mpfirel[pare]->relpin[l].campo = c2;
		mpfirel[pare]->relpin[l].qp = qp;
		mpfirel[pare]->relpin[l].nfirel = fill;
		mpfirel[pare]->relpin[l].nficampo = c1;
		mpfirel[pare]->relpin[l].numrela = -1;
		oc = 0;
		for (z=0;z<vg_mpfi(pare)->f_nrelaci;z++) {
			if (strcmp(quita_path(vg_mpfi(pare)->f_relaci[z]->r_fichero),
				 quita_path(vg_mpfi(fill)->f_fichero)) == 0) {
		       oc++;
		       if (oc == ocur) {
			 mpfirel[pare]->relpin[l].numrela = z;
			 break;
			 }
		       }
		   }
	      mpfirel[pare]->nrelpin++;
seguir:;
	      }
	   else {
	     if (fini_par[i] != -1)
	       {        
	       tmp2 = malloc(80);
	       sprintf(tmp2,"Pintar de fichero inexistente campo %d fichero",j);
	       un_error(tmp2,vg_mpfi(i)->f_titulo);
	       free(tmp2);
	       }  
	     }
	   }
	else
	if (fini_par[i] == -1); /* A PARTIR DE AQUI ES SOLO PARA DEFS QUE USEN TODO */
	else    
	if (pin[0] == 'Z' || pin[0] == 'E' || pin[0] == 'C') {
	   c1 = 0;
	   if (pin[0] == 'C') { /* viejo estilo!! */
	      nnn = strlen(pin+1);
	      if (nnn > 8) {
		  c1 = atoi(pin+9);
		  pin[9]='\0';
		  }
	      }
	   else
	   if (pin[0] == 'Z') {
	      c1 = 999;
	      }
	   else
	   if (pin[0] == 'E') {
	      if (_separa(pin,_n,1,2)) /* separar solo el primero */
		 c1 = atoi(_n[0]);
	      }
	   if (c1 == 999)
	       strminus(pin+1);
	   strcpy(tem,pin+1);
       para1[0] = 0;
       if (c1 != 999)
       {
           for (l = 0;tem[l] && tem[l] != ';';l++);
           if (tem[l] == ';')
           {
               strcpy(para1,tem+l+1);
               tem[l] = 0;
           }
       }
	   proceso_creado = 1;
	   qp = tabla_cal(tem,c1,i);
	   if (qp < 0 ) {
		  tmp2 = malloc(80);
		  sprintf(tmp2,"Proceso no encontrado %s campo %d fichero",tem,j);
		  un_error(tmp2,vg_mpfi(i)->f_fichero);
		  free(tmp2);
		  }
	   if (qp >= 0 && c1 < 998) {
	     l = mpfirel[i]->nrelact;
		 if (l >= MAXRELAS)
		   {
		   sprintf(tmp2,"LIMITE INTERNO DE 'MAXRELAS' SUPERADO EN nrelact !!INESTABILIDAD!!");
		   un_error(tmp2,vg_mpfi(i)->f_titulo);
		   }
	     mpfirel[i]->relact[l].campo = j;
	     mpfirel[i]->relact[l].tabla = qp;
	     mpfirel[i]->relact[l].ambito = c1;
         mpfirel[i]->relact[l].fflag = atoi(para1);
	     mpfirel[i]->nrelact++;
	     }
	   if (c1 == 998 && qp >= 0) {
	      for (z=0;z<mpfirel[i]->nrel;z++) {
		 if ((frel = mpfirel[i]->relacis[z]) < 0) {
		    memcpy(tem,
		     vg_mpfi(i)->f_relaci[mpfirel[i]->numrela[z]]->r_fichero+12,
		     8);
		    tem[8] = '\0';
		    quita_blancos(tem);
		    quita_blancos(pin+1);
		    if (strcmp(tem,pin+1) == 0) {
		       frel = (-frel);
		       c1 = (frel%100) + (frel/10000)*10000;
		       frel = c1 + qp*100;
		       mpfirel[i]->relacis[z] = frel;
		       }
		    }
		 }
	      }
	   }
	leestring(tmp,pin,1);
	}
     }
   }

if (tmp)
   free(tmp);
return(0);
}



reset_tabla()
{
int i;

for (i=0;i<1500;i++)
   {
   p_tabla[i] = -1;
   proc_tabla[i] = 0;
   tipo_tabla[i] = -1;
   }
return(0);
}

int crea_tabla()
{
int i,c;

reset_tabla();
c = 1;
for (i = 0;i < totalprogr;i++) {

   if (strcmp(progr[i]->nombre,"PROGRAMA") == 0)  {
      if (p_tabla[0] != -1) {
	 un_error("PROGRAMA","REDEFINIDO");
	 }
      else {
	 p_tabla[0] = progr[i]->indice;
         proc_tabla[0] = progr[i]->proceso;
         tipo_tabla[0] = -1;
         }
      }
   else {
      if (progr[i]->tipo == -1) continue; /* es una rutina */

      p_tabla[c] = progr[i]->indice;
      proc_tabla[c] = progr[i]->proceso;
      tipo_tabla[c] = progr[i]->tipo;
      c++;
      if (c == 70) c = 100; /* SALTA ESPACIO RESERVADO A CONTADORES */
      if (progr[i]->contador) { /* tabla para contadores */
	 p_tabla[69 + progr[i]->contador] = progr[i]->indice;
         proc_tabla[69 + progr[i]->contador] = progr[i]->proceso;
	 }
      }
   }
return(0);
}


static int gra__aplic(nom)
char *nom;
{
char nombre[81];
int r;

strcpy(nombre,dirbase);
strcat(nombre,"bin/");
strcat(nombre,quita_path(nom));
strcat(nombre,".tab");
r = gra_aplic(nombre);

return(r);
}



static int linka_apl(nom)
char *nom;
{
FI_DEF *pfi;
int r,i;


   c_linea = 0;
   error_break = 0;

   for (i = 0;i < MAX_BUCLE;i++) 
     {
     tabla_defbucle[i] = NULL;
     }

   ini_nombres();
   nfi = 0;
   nprog = 0;
   ncond = 0;
   nbucl = 0;
   noper = 0;
   nmemo = 0;
   nf_B = 0;
   reset_tabla();
   memoplin = 0;

   notacion_dir_aplicacion[0] = 0;
   if (nom[0] == ':' && nom[1] != '/')
   {
       strcpy(notacion_dir_aplicacion,nom);
   }      


   if (!oculto) 
     {  
     char nombre[256];
     strcpy(nombre,dirbase);
     strcat(nombre,"bin/");
     strcat(nombre,quita_path(nom));
     strcat(nombre,".tab");
     strcpy(c_nombrecal,nombre);

     v_prints("Generando %s",nombre);
     }


   pfi = (FI_DEF *)malloc(sizeof(FI_DEF));
   memset(pfi,0,sizeof(FI_DEF));
   ventana_defs[0] = 0;
   if ( (r = lee__def(nom,pfi,1,1))) {
      un_error("Def principal inaccesible",nom);
      free((char *)pfi);      
      if (!error_break)
	    return(1);
      else
	    return(-1);
      }

 

   tipo_de_apli = pfi->tipo_de_def;
   mpfi[0] = pfi;   
   comprueba_def(nom);
   mpfirel[0] = (RELAS *)malloc(sizeof(RELAS));
   mpfirel[0]->lineas = -1;
   mpfirel[0]->clave = -1;
   nfi = 1;
   primero = 0;
   mpfirel[0]->nrel    = 0;
   mpfirel[0]->nrelact = 0;
   mpfirel[0]->nreldef = 0;
   mpfirel[0]->nrelpin = 0;
   mflagfi[0] = RCERRADO;
   c_nerror = 0;   

   en_ini_par++; /* anula efecto de localidad en compilacion */
   ini_par();
   en_ini_par--;
   if (!error_break)
   {
       int kki;
       int kkj;
       int kk;

      ini_rel();


      for (kki = 0;kki < nen_pr;kki++)
      {
          en_pr[kki].interna[0] = 0;
          for (kkj = 0;kkj < en_pr[kki].nllamada_interna;kkj++)
          {
	          kk = busca_prog(en_pr[kki].llamada_interna[kkj]);
	          if (kk < 0)
              {
                  /* buscar en tabla de localizacion */
                  for (kk = 0;kk < nlista_lib;kk++)
                  {
                      if (!strcmp(en_pr[kki].llamada_interna[kkj],lista_lib[kk]))
                      {
                          tabla_cal(lista_lib[kk]+25,999,0);
                          break;
                      }
                  }
                  kk = busca_prog(en_pr[kki].llamada_interna[kkj]);
                  if (kk < 0)
                  {
                      un_error("PROCESO NO ENCONTRADO AL ENLAZAR",en_pr[kki].llamada_interna[kkj]);
		              /*printf("[%d][%d]",i,kk);getchar();^*/
		              continue;
                  }
               }
               en_pr[kki].interna[kkj+1] = numero_prog(kk);
          }
          en_pr[kki].ninterna = en_pr[kki].nllamada_interna+1;
      }


	  crea_tabla(); /* llena p_tabla !! */
    }	
	  

    for (i = 0;i < MAX_BUCLE;i++) 
	  {
      if (!tabla_defbucle[i]) continue;
      free(tabla_defbucle[i]);
      tabla_defbucle[i] = NULL;
      }

    if (!error_break) 
	  {
	  if (gra__aplic(nom)) 
	     {
		 un_error("Error de grabacion",nom);
		 }
      if (!oculto)
        {   
	    if (!c_nerror)
	     {
		 pprint(101,"ok");
		 }
	    else
	     {
         char tmp_x[128];
         sprintf(tmp_x,"errores(%3d)\n",c_nerror);
         pprint(101,tmp_x);
         }	   
	    }
	  }

    fin_nombres();
    fin_aplic();
    reset_prog();
    fin_prog();

    if (error_break) return(-1);

return(0);
}

int es_generable(char *m_nombre)
{
int file;
char tmp[1024];
short xx = 0;

sprintf(tmp,"%sdef/%s",dirbase,m_nombre);
file = rw_open_(tmp,O_BINARY | O_RDONLY);
if (file >= 0)
   {
   rw_lseek_(file,42L,SEEK_SET); 
   rw_read_(file,&xx,sizeof(short));
   if (xx == FGVGEN_ID_MAS)
      {
      rw_lseek_(file,46L,SEEK_SET);   
      rw_read_(file,&xx,sizeof(short));
	  }
   else
      xx = 0;
   rw_close_(file);
   }
return(xx);
}


static int saca_infodir_deldef(char *eldef,char *titulo,char *mi_modulo)
{
int file;
int datos = 0;
short xx;
short claves;
short pantallas;
short campos;
short relaciones;
titulo[0] = 0;
mi_modulo[0] = 0;

file = rw_open_(eldef,O_BINARY | O_RDONLY);
if (file >= 0)
  {
  rw_lseek_(file,33L,SEEK_SET);
  rw_read_(file,mi_modulo,9);
  rw_lseek_(file,42L,SEEK_SET);
  xx = 0;
  rw_read_(file,&xx,sizeof(short));
  if (xx == FGVGEN_ID_MAS)
     {
	 xx = 0;
  	 rw_read_(file,(char *)&xx,sizeof(short));
	 if (xx)
	   {
	   datos |= 0x1;
	   }
	 xx = 0;
	 rw_read_(file,(char *)&xx,sizeof(short));
	 if (xx)
	   {
	   datos |= 0x2;
	   }
	 }
  else
     {
	 datos |= 0x8;
	 }
  pantallas = 0;
  claves = 0;
  campos = 0;
  relaciones = 0;
  rw_lseek_(file,49L + O_MAXPATH + (sizeof(char *) * 6) + (sizeof(short) * 2),SEEK_SET);
  rw_read_(file,(char *)&pantallas,sizeof(short));
  rw_read_(file,(char *)&claves,sizeof(short));
  rw_read_(file,(char *)&campos,sizeof(short));
  rw_read_(file,(char *)&relaciones,sizeof(short));
  if (claves)
     {
     datos |= 0x4;
	 }
  {
	int tamcampo = (sizeof(char *) * 8) + (sizeof(short) * 12);
	int tamclave = (sizeof(char *) * 2) + (sizeof(short) * 2);
	int tamrelaci = (sizeof(char *) * 3) + (sizeof(short) * 2);
	short ncam;
	int j;
	long offset,off2;

    offset = 49L + O_MAXPATH + (sizeof(char *) * 6) + (sizeof(short) * 7) + (sizeof(PAN_DEF) * pantallas) + (tamcampo * campos);    
	off2 = 0;
    for (j = 0;j < claves;j++)
	   {
       rw_lseek_(file,offset+(sizeof(char *)*2),SEEK_SET);
	   ncam = 0;
	   rw_read_(file,(char *)&ncam,sizeof(short));
       offset += tamclave;
	   off2 += (ncam * 2 * sizeof(short));
	   }
	offset += (off2 + (relaciones * tamrelaci));
	rw_lseek_(file,offset,SEEK_SET);
	rw_read_(file,titulo,129);
  }

  rw_close_(file);	 
  }
else
  datos |= 0x8;
return(datos);
}

static int inserta_lista_dir_link(int total,char **dir_xxx,MENU_DEF *dir[],char *subdir)
{
char titulo[256];
char nombre[256];
char tmp[1024];
int r;
char **p;
char mi_modulo[10];

for (p = dir_xxx;p && *p;p++)
  {
  strcpy(nombre,*p);
  {
  int j;
    for (j = 0;nombre[j];j++)
	   {
	   if (nombre[j] == '.') 
	      {
	      nombre[j] = 0;
		  break;
		  }
	   }
  }
  sprintf(tmp,"%s%s%s",dirbase,subdir,*p);
  r = saca_infodir_deldef(tmp,titulo,mi_modulo);
  dir[total] = (MENU_DEF *)malloc(sizeof(MENU_DEF));  
  titulo[30] = 0;
  strcpy(dir[total]->m_nombre,nombre);
  strcpy(dir[total]->m_titulo,titulo);
  if ((r & 1))
     dir[total]->m_lista = 0;
  else
     dir[total]->m_lista = 1;

  if ((r & 2))
     dir[total]->m_manten = 3;
  else
     dir[total]->m_manten = 4;

  if ((r & 4))
     dir[total]->m_manten -= 2;

  dir[total]->m_fichas = 1L;
  strcpy(dir[total]->m_passwd,mi_modulo);
  total++;
  }
return(total);
}

graba_dir(ndir,dir,nfich,modulo)
char *ndir;
MENU_DEF *dir[];
int nfich;
char *modulo;
{
   FILE *file1;
   char nomfi[O_MAXPATH];   
   int i;

   strcpy(nomfi,ndir);
   strcat(nomfi,".dir");
   if (( file1 = fopen(nomfi,"w")) == (FILE *) 0)
      return(-1);

   for (i=0;i<nfich;i++) {      
      if (modulo)
	     {
		 if (dir[i]->m_passwd[0])
		   {
		   if (strcmp(dir[i]->m_passwd,modulo))
		      continue;
		   dir[i]->m_passwd[0] = 1;
		   }		 
		 }      
      fprintf(file1,"%s\n",dir[i]->m_nombre);
	  fprintf(file1,"%s\n",dir[i]->m_titulo);
	  fprintf(file1,"\n");/*,dir[i]->m_passwd);*/
      fprintf(file1,"%c%c\n",(char)(dir[i]->m_manten+'0'),
			   (char)(dir[i]->m_lista +'0') );      
      fprintf(file1,"%ld\n",dir[i]->m_fichas);      
      }
   fclose(file1);
   return(0);
}

static int abortar = 0;

int aborta_linka()
{
return(abortar = 1);
}

int linkar(int argc,char *argv[])
{
int i;
int bien = 1;
int crea_dir = 0;
char **dir_def;
char tmp[1024];
char n_modulo[128];
char solo_uno[256];

abortar = 0;
c_nombrecal[0] = 0;
solo_uno[0] = 0;

modo_gen_antiguo = 0;
crea_dir = 0;
dirbase[0] = 0;
for (i = 1;i < argc;i++)
   {
   if (argv[i][0] == '-')
      {
      if (!strcmp(argv[i]+1,"5"))
	     {
         modo_gen_antiguo = 1;
		 }
	  else
      if (!strcmp(argv[i]+1,"6"))
	     {
         modo_gen_antiguo = 0;
		 }
	  else
      if (!strcmp(argv[i]+1,"dir"))
	     {
         crea_dir = 1;
		 }
	  else
	     {
		 bien = 0;
		 break;
		 }
	  }
   else
      {
	  if (dirbase[0])
	     {
         if (solo_uno[0])
         {
		     bien = 0;
		     break;
         }
         else
         {
            strcpy(solo_uno,argv[i]);
            strlwr(solo_uno);
         }
		 }
      else
	     strcpy(dirbase,argv[i]);
	  }
   }

if (!bien || !dirbase[0]) 
   {
   pprint(101,"dslink Version 8");
   pprint(101,"dslink [-5 o -6 -dir] directorio_aplicacion");
   exit(2);
   }

cambia_barra(dirbase);
strcpy(dirbase_,dirbase);
i = strlen(dirbase_) - 1;
if (dirbase_[i] == '/')
   {
   dirbase_[i] = 0;
   }
else
   {
   strcat(dirbase,"/");
   }

n_modulo[0] = 0;
strcpy(tmp,dirbase);
if (tmp[0])
{
    tmp[strlen(tmp)-1] = 0;
    strcpy(n_modulo,quita_path(tmp));
}


strcpy(dirbass,quita_path(dirbase_));
*(quita_path(dirbase_)) = 0;


local_term[0]  = 0xffff;
local_term[50] = 0;
local_term[51] = 0xffff;

mpfi_maximo = (FI_DEF *)malloc(sizeof(FI_DEF));
memset(mpfi_maximo,0,sizeof(FI_DEF));
mpfi_maximo->f_npanta = MAXPANTA;
mpfi_maximo->f_nclaves = MAXCLAVES;
mpfi_maximo->f_ncampos = MAXCAM;
mpfi_maximo->f_nrelaci = MAXRELACI;
for (i  = 0;i < MAXCAM;i++)
   {
   mpfi_maximo->f_campos[i] = (CAMPO_DEF  *)malloc(sizeof(CAMPO_DEF));
   memset(mpfi_maximo->f_campos[i],0,sizeof(CAMPO_DEF));
   }


fichs = (FF **)malloc(sizeof(char *) * 150);
memos = (MM **)malloc(sizeof(char *) * MAX_MEMOS);
progr = (PP **)malloc(sizeof(char *) * 1500);
gosub = (GSS **)malloc(sizeof(char *) * MAX_GOSUB);
en_pr = (struct enlaza_prc *)malloc(sizeof(struct enlaza_prc) * MAX_PROCES);
prog = (PROGRAM *)malloc(sizeof(PROGRAM) * MAX_PROG);
memo = (T_MEMORIA **)malloc(sizeof(T_MEMORIA *) * MAX_MEMOS);
global_ex = (M_EX **)malloc(sizeof(M_EX *) * MAX_MEX);
m_ex = (M_EX **)malloc(sizeof(M_EX *) * MAX_MEX);
p_ex = (P_EX **)malloc(sizeof(P_EX *) * MAX_PEX);

camps = NULL;
bucles = NULL;
etiqs = NULL;
gotos = NULL;
cond = NULL;
bucl = NULL;
oper = NULL;
f_B = NULL;

ya_esta_avisado = 0;
numero_generacion = time(NULL);


{
    FILE *f;
    sprintf(tmp,"%s%s.plb",dirbase,n_modulo);
    f = fopen(tmp,"r");
    if (f)
    {
        int ii;
        while(fgets(tmp,100,f))
        {
            quitan(tmp);
            quitab(tmp);
            quitan(tmp);
            quitab(tmp);
            for (ii = 0;tmp[ii] && tmp[ii] != ',';ii++);
            if (tmp[ii] == ',')
            {
                memcpy(lista_lib[nlista_lib],tmp,ii);
                lista_lib[nlista_lib][ii] = 0;
                quita_blancos(lista_lib[nlista_lib]);
                strcpy(lista_lib[nlista_lib]+25,tmp+ii+1);
                quita_blancos(lista_lib[nlista_lib]+25);
                nlista_lib++;
            }
        }
        fclose(f);
    }

}


/* leer directorio ... */
sprintf(tmp,"%sdef/*.mas",dirbase);
dir_def = lee_direct(tmp,0);
if (dir_def)
   {
   char **p1;
   for (p1 = dir_def;*p1 && !abortar;p1++)
      {
      if (solo_uno[0] && strcmp(quita_path(solo_uno),quita_path(*p1)))
          continue;
	  if (es_generable(*p1))
	     {
		 char nombre[81];
		 char *p = nombre;
		 strcpy(nombre,*p1);
		 while(*p && *p != '.') p++;
		 *p = 0;
		 linka_apl(nombre);
		 }
	  }
   if (crea_dir && !abortar)
      {
      MENU_DEF *dir[1500];
	  int numfi = 0;
      numfi = inserta_lista_dir_link(numfi,dir_def,dir,"def/");
	  if (numfi)
	     {
		 sprintf(tmp,"%sfich/%s",dirbase,dirbass);
		 graba_dir(tmp,dir,numfi,NULL);
         for (i = 0;i < numfi;i++)
		    {
		    if (dir[i]->m_passwd[0] > 1)
		      {
			  char xmodulo[20];
			  strcpy(xmodulo,dir[i]->m_passwd);
			  sprintf(tmp,"%sfich/%s",dirbase,dir[i]->m_passwd);
			  graba_dir(tmp,dir,numfi,xmodulo);
			  }		  
			}
         for (i = 0;i < numfi;i++)
		    {
			free((char *)dir[i]);
			}
		 }
	  }
   libera_dir(dir_def);
   }

abortar = 0;

if (prog)
  free((char *)prog);
if (memo)
  free((char *)memo);
if (cond)
  free((char *)cond);
if (bucl)
  free((char *)bucl);
if (oper)
  free((char *)oper);
if (f_B)
  free((char *)f_B);
if (m_ex)
  free((char *)m_ex);
if (p_ex)
  free((char *)p_ex);
if (global_ex)
  free((char *)global_ex);
if (en_pr)
  free((char *)en_pr);
if (fichs)
  free((char *)fichs);
if (memos)
  free((char *)memos);
if (camps)
  free((char *)camps);
if (progr)
  free((char *)progr);
if (bucles)
  free((char *)bucles);
if (etiqs)
   free((char *)etiqs);
if (gotos)
   free((char *)gotos);
if (gosub)
   free((char *)gosub);

for (i  = 0;i < MAXCAM;i++)
   {
   free(mpfi_maximo->f_campos[i]);
   }
free(mpfi_maximo);
return(0);
}

