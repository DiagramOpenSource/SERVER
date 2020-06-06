

/* Autor: Francisco Gonzalez Valiente */
#include <fgvsys.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef O_BINARY
#include <io.h>
#endif
#include <stdio.h>
#include <string.h>


#define _GENERADOR
#include <rdef.h> /* trabajo con la estructura extensiva */
#include <rmante.h>
#include <rcalcul.h>
#include <rcompi.h>


#ifndef O_BINARY  /* compatibilidad unix - dos */

#define O_BINARY 0
#define SEEK_SET 0
#define SEEK_CUR 1
#ifndef LARGESEEK
extern long rw_lseek_();
#endif
extern unsigned int rw_write_();

#endif

extern char *quita_path();
extern short tipo_de_def;
extern short _fgv_version;
extern short i3;

short tipo_de_apli = 0;
long numero_generacion = 0L;

int modo_gen_antiguo = 1;

char *nombre_defs[MAXDEF*2] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,};
char ventana_defs[MAXDEF*2] = {0,0,0,};
short fini_par[MAXDEF];
unsigned short tam_gene_def[MAXDEF];


extern char **llamada_interna;
extern short nllamada_interna;


#ifdef ESMOTOROLA
static unsigned int graba_short(file,p,n)
int file;
short *p;
int n;
{
short tmp;
unsigned int r = 0;

while(n > 0 && r >= 0)
  {
  tmp = *p;
  moto_short(&tmp);     
  r += rw_write_(file,&tmp,2);
  p++;
  n -= 2;
  }
return(r);
}
#else
static unsigned int graba_short(file,p,n)
int file;
char *p;
int n;
{
	return(rw_write_(file,p,n));
}
#endif


#ifndef FGV_SOLOCOMP
static grabas_relas(file1,rel)
int file1,rel;
{
short i;
int j;

/* lo que sigue es para mantener la compatibilidad de estructuras */
rw_write_(file1,(char *)mpfirel[rel],sizeof(char *));
rw_write_(file1,(char *)mpfirel[rel],sizeof(char *));
rw_write_(file1,(char *)mpfirel[rel],sizeof(char *));
rw_write_(file1,(char *)mpfirel[rel],sizeof(char *));
rw_write_(file1,(char *)mpfirel[rel],sizeof(char *));
rw_write_(file1,(char *)mpfirel[rel],sizeof(char *));
rw_write_(file1,(char *)mpfirel[rel],sizeof(char *));
graba_short(file1,(char *)&mpfirel[rel]->nrel,sizeof(short));
graba_short(file1,(char *)&mpfirel[rel]->nrelact,sizeof(short));
graba_short(file1,(char *)&mpfirel[rel]->nreldef,sizeof(short));
graba_short(file1,(char *)&mpfirel[rel]->nrelpin,sizeof(short));
graba_short(file1,(char *)&mpfirel[rel]->lineas,sizeof(short));
graba_short(file1,(char *)&mpfirel[rel]->clave,sizeof(short));


if ((i = mpfirel[rel]->nrel) > 0) {
   graba_short(file1,(char *)mpfirel[rel]->relacis,sizeof(short) * i);
   graba_short(file1,(char *)mpfirel[rel]->camrela,sizeof(short) * i);
   graba_short(file1,(char *)mpfirel[rel]->camminu,sizeof(short) * i);
   graba_short(file1,(char *)mpfirel[rel]->numrela,sizeof(short) * i);
   }

if ((i = mpfirel[rel]->nrelact) > 0)
  {
#ifdef ESMOTOROLA
  for (j = 0;j < i;j++)
     {
     moto_short(&(mpfirel[rel]->relact[j].campo));
     moto_short(&(mpfirel[rel]->relact[j].ambito));
     moto_short(&(mpfirel[rel]->relact[j].tabla));
     moto_short(&(mpfirel[rel]->relact[j].fflag));               
     }
  rw_write_(file1,(char *)mpfirel[rel]->relact,sizeof(REL_ACT) * i);
  for (j = 0;j < i;j++)
     {
     moto_short(&(mpfirel[rel]->relact[j].campo));
     moto_short(&(mpfirel[rel]->relact[j].ambito));
     moto_short(&(mpfirel[rel]->relact[j].tabla));
     moto_short(&(mpfirel[rel]->relact[j].fflag));
     }  
#else 
  rw_write_(file1,(char *)mpfirel[rel]->relact,sizeof(REL_ACT) * i);
#endif  
  }
if ((i = mpfirel[rel]->nreldef) > 0)
  {
#ifdef ESMOTOROLA
  for (j = 0;j < i;j++)
     {
     moto_short(&(mpfirel[rel]->reldef[j].campo));
     moto_short(&(mpfirel[rel]->reldef[j].nfirel));
     moto_short(&(mpfirel[rel]->reldef[j].nficampo));
     moto_short(&(mpfirel[rel]->reldef[j].numrela));
     }
  rw_write_(file1,(char *)mpfirel[rel]->reldef,sizeof(REL0_DEF) * i);
  for (j = 0;j < i;j++)
     {
     moto_short(&(mpfirel[rel]->reldef[j].campo));
     moto_short(&(mpfirel[rel]->reldef[j].nfirel));
     moto_short(&(mpfirel[rel]->reldef[j].nficampo));
     moto_short(&(mpfirel[rel]->reldef[j].numrela));
     }  
#else
  rw_write_(file1,(char *)mpfirel[rel]->reldef,sizeof(REL0_DEF) * i);
#endif  
  }
if ((i = mpfirel[rel]->nrelpin) > 0)
  {
#ifdef ESMOTOROLA
  for (j = 0;j < i;j++)
     {
     moto_short(&(mpfirel[rel]->relpin[j].campo));
     moto_short(&(mpfirel[rel]->relpin[j].qp));
     moto_short(&(mpfirel[rel]->relpin[j].nfirel));
     moto_short(&(mpfirel[rel]->relpin[j].nficampo));
     moto_short(&(mpfirel[rel]->relpin[j].numrela));                    
     moto_short(&(mpfirel[rel]->relpin[j].fflag));      
     }
  rw_write_(file1,(char *)mpfirel[rel]->relpin,sizeof(REL_PIN) * i);
  for (j = 0;j < i;j++)
     {
     moto_short(&(mpfirel[rel]->relpin[j].campo));
     moto_short(&(mpfirel[rel]->relpin[j].qp));
     moto_short(&(mpfirel[rel]->relpin[j].nfirel));
     moto_short(&(mpfirel[rel]->relpin[j].nficampo));
     moto_short(&(mpfirel[rel]->relpin[j].numrela));                    
     moto_short(&(mpfirel[rel]->relpin[j].fflag));      
     }  
#else   
  rw_write_(file1,(char *)mpfirel[rel]->relpin,sizeof(REL_PIN) * i);
#endif  
  }
return(0);
}
#endif

static short lentiraint(c)
short *c;
{
short nc;
for (nc = 0;*(c + nc) != -32535;nc++);
nc++;
return(nc);
}

static grabatiraint(nc,c,file1)
int nc;
short *c;
int file1;
{
unsigned int tamano;
if (nc == -1) {
   for (nc = 0;*(c + nc) != -32535;nc++);
   nc++;
   }
tamano = (unsigned int)graba_short(file1,(char *)c,sizeof(short)*nc);
return(tamano);
}

extern long nume_serie;

#ifndef FGV_SOLOLINK
graba_prog(nom)
char *nom;
{
char nombre[81];
int file1;
int i;
char c;
short j;
#ifdef ESMOTOROLA
double tmpd;
#endif
unsigned short tamano = 0;
long posicion;
unsigned short dato_e;
unsigned short dato;
unsigned long n_s;


if (modo_gen_antiguo)
   {
   return(graba_prog_6(nom));
   }

strcpy(nombre,nom);
strcat(nombre,".prc");
if ((file1 = rw_open_(nombre,O_CREAT | O_TRUNC | O_BINARY | O_RDWR,
			S_IREAD | S_IWRITE)) < 0) {
    return(-1);
    }

   i = FGVGEN_ID_PRC;
   graba_short(file1,(char *)&i,sizeof(short));
/* no hace falta convertirlo a motorola pues es temporal */
   rw_write_(file1,(char *)(&numero_generacion),sizeof(long));
   dato_e = rand();
   (unsigned int)graba_short(file1,(char *)&dato_e,sizeof(short));
   dato_e = nume_code() ^ dato_e;
   (unsigned int)graba_short(file1,(char *)&dato_e,sizeof(short));
   n_s = nume_serie / 0xffff;
   dato = ((unsigned short)n_s) ^ dato_e;
   (unsigned int)graba_short(file1,(char *)&dato,sizeof(short));
   n_s = nume_serie % 0xffff;
   dato = ((unsigned short)n_s) ^ dato_e;
   (unsigned int)graba_short(file1,(char *)&dato,sizeof(short));
   dato = _Posicion() ^ dato_e;
   (unsigned int)graba_short(file1,(char *)&dato,sizeof(short));
   if (nfi)
        dato = vg_mpfi(0)->tipo_de_def ^ dato_e;
   else
        dato = 0 ^ dato_e;
   (unsigned int)graba_short(file1,(char *)&dato,sizeof(short));
   if (nfi)
        dato = vg_mpfi(0)->version_de_def ^ dato_e;
   else
        dato = 0 ^ dato_e;
   (unsigned int)graba_short(file1,(char *)&dato,sizeof(short));

   /* 6 bytes reservados */
   graba_short(file1,(char *)&i,sizeof(short));
   graba_short(file1,(char *)&i,sizeof(short));
   graba_short(file1,(char *)&i,sizeof(short));

graba_short(file1,(char *)&nprog,sizeof(short));
graba_short(file1,(char *)&nmemo,sizeof(short));
graba_short(file1,(char *)&ncond,sizeof(short));
graba_short(file1,(char *)&nbucl,sizeof(short));
graba_short(file1,(char *)&noper,sizeof(short));
graba_short(file1,(char *)&nf_B,sizeof(short));
graba_short(file1,(char *)&nm_ex,sizeof(short));
graba_short(file1,(char *)&np_ex,sizeof(short));

/* grabar ficheros incluidos (van por orden) */
graba_short(file1,(char *)&totalfichs,sizeof(short));

for (i = 0;i < totalfichs;i++) {
   graba_short(file1,(char *)&(fichs[i]->indice),sizeof(short));
   graba_short(file1,(char *)&(fichs[i]->fini),sizeof(short));
   c = strlen( quita_path(fichs[i]->fichero) ) + 1;
   tamano += (unsigned int)rw_write_(file1,&c,1);
   tamano += (unsigned int)rw_write_(file1,quita_path(fichs[i]->fichero),c);
   }

/* grabar memorias externas de este proceso */
for (i = 0;i < nm_ex;i++) {   
   M_EX tmpx;
#ifdef ESMOTOROLA
   memcpy(&tmpd,&(m_ex[i]->mv.vald),8);
   moto_double(&tmpd);
   tamano += (unsigned int)rw_write_(file1,(char *)&tmpd,8);
   tamano += (unsigned int)graba_short(file1,(char *)&(m_ex[i]->m_tipo),2);
   tamano += (unsigned int)graba_short(file1,(char *)&(m_ex[i]->indice_global),2);
   tamano += (unsigned int)rw_write_(file1,(char *)&tmpd,4); /* !!! */
#else
   memcpy(&tmpx,m_ex[i],sizeof(M_EX));
   m_ex[i]->indice_global = 0;
   m_ex[i]->nombre = NULL;
      
   if (m_ex[i]->m_tipo == CHARTIPO || m_ex[i]->m_tipo == FECHATIPO)
      {
	  memset(&(m_ex[i]->mv.vald),0,sizeof(double));
	  }
	
   tamano += (unsigned int)rw_write_(file1,(char *)m_ex[i],sizeof(M_EX));
   memcpy(m_ex[i],&tmpx,sizeof(M_EX));
#endif
   }

for (i = 0;i < nm_ex;i++) {
   c = strlen(m_ex[i]->nombre)+1;
   tamano += (unsigned int)rw_write_(file1,&c,1);
   tamano += (unsigned int)rw_write_(file1,m_ex[i]->nombre,c);
   if (m_ex[i]->m_tipo == CHARTIPO || m_ex[i]->m_tipo == FECHATIPO)
      {
      c = strlen(m_ex[i]->mv.valc)+1;
      tamano += (unsigned int)rw_write_(file1,&c,1);
      tamano += (unsigned int)rw_write_(file1,m_ex[i]->mv.valc,c);
      }
   }

/* punto de salto para lectura */
posicion = rw_lseek_(file1,0L,SEEK_CUR);
tamano = 0;

graba_short(file1,(char *)&tamano,sizeof(short));

/* grabar procesos empleados y sus direcciones (como externo) */

for (i = 0;i < np_ex;i++) {
   char *tmpx;
   tmpx = p_ex[i]->nombre;
   p_ex[i]->nombre = NULL;
   moto_short(&(p_ex[i]->indice));
   tamano += (unsigned int)rw_write_(file1,(char *)p_ex[i],sizeof(P_EX));
   moto_short(&(p_ex[i]->indice)); /* indice relativo a nprog */
   p_ex[i]->nombre = tmpx;
   }

for (i = 0;i < np_ex;i++) {
   c = strlen(p_ex[i]->nombre)+1;
   tamano += (unsigned int)rw_write_(file1,&c,1);
   tamano += (unsigned int)rw_write_(file1,p_ex[i]->nombre,c);
   }

tamano += graba_short(file1,(char *)&nllamada_interna,sizeof(short));
for (i = 0;i < nllamada_interna;i++)
   {
   c = strlen(llamada_interna[i])+1;
   tamano += (unsigned int)rw_write_(file1,&c,1);
   tamano += (unsigned int)rw_write_(file1,llamada_interna[i],c);
   }

rw_lseek_(file1,posicion,SEEK_SET);
graba_short(file1,(char *)&tamano,sizeof(short));
tamano += sizeof(short);
posicion += (long)(unsigned long)tamano;
rw_lseek_(file1,posicion,SEEK_SET);


for (i = 0;i < nprog;i++) {
   moto_short(&(prog[i].indice));
   prog[i].i_plus = 0;
   tamano += (unsigned int)rw_write_(file1,(char *)(prog+i),sizeof(PROGRAM));
   moto_short(&(prog[i].indice));
   }

for (i = 0;i < nmemo;i++) {
   T_MEMORIA tmpx;
#ifdef ESMOTOROLA
   memcpy(&tmpd,&(memo[i]->mv.vald),8);
   moto_double(&tmpd);
   tamano += (unsigned int)rw_write_(file1,(char *)&tmpd,8);
   tamano += (unsigned int)graba_short(file1,(char *)&(memo[i]->m_tipo),2);
   tamano += (unsigned int)graba_short(file1,(char *)&(memo[i]->m_fflag),2);
#else
   memcpy(&tmpx,memo[i],sizeof(T_MEMORIA));
   if (memo[i]->m_tipo == CHARTIPO || memo[i]->m_tipo == FECHATIPO)
     {
     memset(&(memo[i]->mv.vald),0,sizeof(double));	 
	 }
   memo[i]->m_fflag = 0;
   tamano += (unsigned int)rw_write_(file1,(char *)memo[i],sizeof(T_MEMORIA));
   memcpy(memo[i],&tmpx,sizeof(T_MEMORIA));
#endif
   }
for (i = 0;i < ncond;i++) {
   moto_short(&(cond[i]->fichero1));
   moto_short(&(cond[i]->campo1));
   moto_short(&(cond[i]->fichero2));
   moto_short(&(cond[i]->campo2));
   moto_short(&(cond[i]->sigue));
   moto_short(&(cond[i]->tipo));   
   cond[i]->condicion_plus = 0;   
   tamano += (unsigned int)rw_write_(file1,(char *)cond[i],sizeof(CONDIC));
   moto_short(&(cond[i]->fichero1));
   moto_short(&(cond[i]->campo1));
   moto_short(&(cond[i]->fichero2));
   moto_short(&(cond[i]->campo2));
   moto_short(&(cond[i]->sigue));
   moto_short(&(cond[i]->tipo));   
   }
for (i = 0;i < nbucl;i++) {
   moto_short(&(bucl[i]->operacion1));
   moto_short(&(bucl[i]->condicion));
   moto_short(&(bucl[i]->operacion2));
   moto_short(&(bucl[i]->salto));
   tamano += (unsigned int)rw_write_(file1,(char *)bucl[i],sizeof(BUCLE));
   moto_short(&(bucl[i]->operacion1));
   moto_short(&(bucl[i]->condicion));
   moto_short(&(bucl[i]->operacion2));
   moto_short(&(bucl[i]->salto));
   }
for (i = 0;i < noper;i++) {
   moto_short(&(oper[i]->fichero1));
   moto_short(&(oper[i]->campo1));
   moto_short(&(oper[i]->fichero2));
   moto_short(&(oper[i]->campo2));
   moto_short(&(oper[i]->ficheror));
   moto_short(&(oper[i]->campor));      
   oper[i]->operacion_plus = 0;
   tamano += (unsigned int)rw_write_(file1,(char *)oper[i],sizeof(OPERA));
   moto_short(&(oper[i]->fichero1));
   moto_short(&(oper[i]->campo1));
   moto_short(&(oper[i]->fichero2));
   moto_short(&(oper[i]->campo2));
   moto_short(&(oper[i]->ficheror));
   moto_short(&(oper[i]->campor));   
   }
for (i = 0;i < nf_B;i++) {
   struct _filebucle tmpxx;
   moto_short(&(f_B[i]->nfi));
   moto_short(&(f_B[i]->nord1));
   moto_short(&(f_B[i]->nord2));
   moto_short(&(f_B[i]->ncomp));
   moto_short(&(f_B[i]->flag));
   memcpy(&tmpxx,f_B[i],sizeof(struct _filebucle));

   f_B[i]->ord1 = NULL;
   f_B[i]->ord2 = NULL;
   f_B[i]->comp = NULL;
   f_B[i]->calc = NULL;
   f_B[i]->fflag = 0;

   tamano += (unsigned int)rw_write_(file1,(char *)f_B[i],
			   sizeof(struct _filebucle));
   memcpy(f_B[i],&tmpxx,sizeof(struct _filebucle));
   
   moto_short(&(f_B[i]->nfi));
   moto_short(&(f_B[i]->nord1));
   moto_short(&(f_B[i]->nord2));
   moto_short(&(f_B[i]->ncomp));
   moto_short(&(f_B[i]->flag));                               
   }

for (i = 0;i < nf_B;i++) {
   j = lentiraint(f_B[i]->ord1);
   tamano += graba_short(file1,&j,sizeof(short));
   tamano += graba_short(file1,f_B[i]->ord1,sizeof(short)*j);
   tamano += (unsigned int)grabatiraint(f_B[i]->nord2/100,f_B[i]->ord2,file1);
   tamano += (unsigned int)grabatiraint(f_B[i]->ncomp,f_B[i]->comp,file1);
   j = lentiraint(f_B[i]->calc);
   tamano += graba_short(file1,&j,sizeof(short));
   tamano += graba_short(file1,f_B[i]->calc,sizeof(short)*j);
   }

for (i = 0;i < nmemo;i++) {
   if (memo[i]->m_tipo == CHARTIPO || memo[i]->m_tipo == FECHATIPO)
      {
      c = strlen(memo[i]->mv.valc) + 1;
      tamano += (unsigned int)rw_write_(file1,&c,1);
      tamano += (unsigned int)rw_write_(file1,memo[i]->mv.valc,c);
      }
   }

/* grabar llamadas a funciones */

rw_close_(file1);

return(0);
}
#endif

/* cargador de la aplicacion configurada en el fichero de parametros */

#ifndef FGV_SOLOCOMP
gra_aplic(aplic)
char *aplic;
{
FI_DEF *pfi;
int file1;
short rel;
short i,m;
char c;
unsigned short tamano = 0;
short _v_ = -32000 + _fgv_version;
short max_panta,max_campos,max_clave,max_relaci,max_gene,max_prog,max_cond;
short max_bucl,max_oper,max_f_B,max_ex;
   unsigned short dato_e;
   unsigned short dato;
   unsigned long n_s;

if (modo_gen_antiguo)
   {
   return(gra_aplic_6(aplic));
   }

if ((file1 = rw_open_(aplic,O_CREAT | O_TRUNC | O_BINARY | O_RDWR,
			S_IREAD | S_IWRITE)) < 0) 
   {
   return(-1);
   }
rw_lseek_(file1,0L,SEEK_SET);

i = ID_TAB;
graba_short(file1,(char *)&i,sizeof(short));

graba_short(file1,(char *)&_v_,sizeof(short));
i = -1;
graba_short(file1,(char *)&i,sizeof(short));
graba_short(file1,(char *)&i3,sizeof(short));
graba_short(file1,(char *)&i3,sizeof(short));
for (m = 0;m < nfi;m++)
   {
   if (m == 48) 
      {
      i = vg_mpfi(m-1)->tipo_de_def ^ nume_code();
      graba_short(file1,(char *)&i,sizeof(short));
      continue;
      }
   i = vg_mpfi(m)->tipo_de_def ^ nume_code();
   graba_short(file1,(char *)&i,sizeof(short));
   }
i = 0xffff;
  for (;m < 103;m++)
      graba_short(file1,(char *)&i,sizeof(short));

   dato_e = rand();
   (unsigned int)graba_short(file1,(char *)&dato_e,sizeof(short));
   dato_e = nume_code() ^ dato_e;
   (unsigned int)graba_short(file1,(char *)&dato_e,sizeof(short));
   n_s = nume_serie / 0xffff;
   dato = ((unsigned short)n_s) ^ dato_e;
   (unsigned int)graba_short(file1,(char *)&dato,sizeof(short));
   n_s = nume_serie % 0xffff;
   dato = ((unsigned short)n_s) ^ dato_e;
   (unsigned int)graba_short(file1,(char *)&dato,sizeof(short));
   dato = _Posicion() ^ dato_e;
   (unsigned int)graba_short(file1,(char *)&dato,sizeof(short));
   dato = vg_mpfi(0)->version_de_def ^ dato_e;
   (unsigned int)graba_short(file1,(char *)&dato,sizeof(short));

/* a partir de aqui encriptado */

graba_short(file1,(char *)&nfi,sizeof(short));
for (rel = 0;rel < nfi;rel++) {
  if (rel == 48) continue;
  if (nombre_defs[rel])
     {
     c = ventana_defs[rel];
     rw_write_(file1,&c,1);
     c = strlen(nombre_defs[rel]) + 1;
     rw_write_(file1,&c,1);
     rw_write_(file1,nombre_defs[rel],c);
     c = strlen(quita_path(vg_mpfi(rel)->f_fichero)) + 1;
     rw_write_(file1,&c,1);
     rw_write_(file1,quita_path(vg_mpfi(rel)->f_fichero),c);
     }
  else
     {
     printf("atencion def sin nombre!!!!!!!");getchar();
     }
     /* vg_mpfi(rel) */
   graba_short(file1,(char *)&mflagfi[rel],sizeof(short));
   if (rel == 0) {
      c = '\376';
      rw_write_(file1,&c,1); /* para que no hayan dos claves juntas */
      rw_write_(file1,"\332\'\370\211\342",5);
      tipo_de_apli = rand();
      i = nume_code();
      m = nume_mochila();
      i ^= m;
      m ^= (*((char *)&tipo_de_apli) & 0xff);
      graba_short(file1,(char *)&i,sizeof(short));
      graba_short(file1,(char *)&m,sizeof(short));
      rw_write_(file1,&tipo_de_apli,1);
      rw_write_(file1,"\144",1);
      }
   }

for (rel = 0;rel < nfi;rel++)
   {
   if (rel == 48) continue;
   grabas_relas(file1,rel);
   graba_short(file1,&(vg_mpfi(rel)->f_lfich),sizeof(short));
   }

if (p_tabla[100] != -1)
  rel = 101;
else
if (p_tabla[70] != -1)
  rel = 71;
else
  rel = 1;
for (;rel < 1500;rel++)
  {
  if (p_tabla[rel] == -1) break;
  }
n_p_tabla = rel;
graba_short(file1,(char *)&n_p_tabla,sizeof(short));
graba_short(file1,(char *)p_tabla,sizeof(short)*rel);
rw_write_(file1,proc_tabla,rel);
rw_write_(file1,tipo_tabla,rel);
graba_short(file1,(char *)&memoplin,sizeof(short)); /* conjunto de memoria global */
graba_short(file1,(char *)&maxpila,sizeof(short));

/* grabar programas externos (ojo no es lo mismo que en graba_prog)*/
graba_short(file1,(char *)&np_ex,sizeof(short));
for (i = 0;i < np_ex;i++) {
   moto_short(&(p_ex[i]->indice));
   tamano += (unsigned int)rw_write_(file1,(char *)p_ex[i],sizeof(P_EX));
   moto_short(&(p_ex[i]->indice)); /* indice relativo a nprog */
   }
for (i = 0;i < np_ex;i++) {
   c = strlen(p_ex[i]->nombre)+1;
   tamano += (unsigned int)rw_write_(file1,&c,1);
   tamano += (unsigned int)rw_write_(file1,p_ex[i]->nombre,c);
   }


graba_short(file1,(char *)&nglobal_ex,sizeof(short));
/* grabar memorias externas */
for (i = 0;i < nglobal_ex;i++) {
#ifdef ESMOTOROLA
   memcpy(&tmpd,&(global_ex[i]->mv.vald),8);
   moto_double(&tmpd);
   tamano += (unsigned int)rw_write_(file1,(char *)&tmpd,4); /* !!! */
   tamano += (unsigned int)rw_write_(file1,(char *)&tmpd,8);
   tamano += (unsigned int)graba_short(file1,(char *)&(global_ex[i]->m_tipo),2);
   tamano += (unsigned int)graba_short(file1,(char *)&(global_ex[i]->indice_global),2);
#else
   tamano += (unsigned int)rw_write_(file1,(char *)global_ex[i],sizeof(M_EX));
#endif
   }

for (i = 0;i < nglobal_ex;i++) {
   c = strlen(global_ex[i]->nombre)+1;
   tamano += (unsigned int)rw_write_(file1,&c,1);
   tamano += (unsigned int)rw_write_(file1,global_ex[i]->nombre,c);
   if (global_ex[i]->m_tipo == CHARTIPO || global_ex[i]->m_tipo == FECHATIPO)
      {
      c = strlen(global_ex[i]->mv.valc)+1;
      tamano += (unsigned int)rw_write_(file1,&c,1);
      tamano += (unsigned int)rw_write_(file1,global_ex[i]->mv.valc,c);
      }
   }

/* ficheros de procesos incluidos */
graba_short(file1,(char *)&nen_pr,sizeof(short));
/* calcular maximos de programa */
max_prog = 0;
max_cond = 0;
max_bucl = 0;
max_oper = 0;
max_f_B = 0;
max_ex = 0;
for (rel = 0;rel < nen_pr;rel++)
   {
   if (en_pr[rel].e_nprog > max_prog)
      max_prog = en_pr[rel].e_nprog;
   if (en_pr[rel].e_ncond > max_cond)
      max_cond = en_pr[rel].e_ncond;
   if (en_pr[rel].e_nbucl > max_bucl)
      max_bucl = en_pr[rel].e_nbucl;
   if (en_pr[rel].e_noper > max_oper)
      max_oper = en_pr[rel].e_noper;
   if (en_pr[rel].e_nf_B > max_f_B)
      max_f_B = en_pr[rel].e_nf_B;
   if (en_pr[rel].e_nex > max_ex)
      max_ex = en_pr[rel].e_nex;
   c = strlen(en_pr[rel].prc)+1;
   rw_write_(file1,&c,1);
   rw_write_(file1,en_pr[rel].prc,c);
   graba_short(file1,&(en_pr[rel].e_nmemo),sizeof(short));
   graba_short(file1,&(en_pr[rel].ninterna),sizeof(short));
   for (i = 0;i < en_pr[rel].ninterna;i++)
      {
      graba_short(file1,&(en_pr[rel].interna[i]),sizeof(short));
      }
   }

max_panta = 0;
max_campos = 0;
max_clave = 0;
max_relaci = 0;
max_gene = 0;
/* calcular maximos de fichero */
for (rel = 0;rel < nfi;rel++)
   {
   if (rel == 48) continue;
   if (vg_mpfi(rel)->f_npanta > max_panta)
       max_panta = vg_mpfi(rel)->f_npanta;
   if (vg_mpfi(rel)->f_ncampos > max_campos)
       max_campos = vg_mpfi(rel)->f_ncampos;
   if (vg_mpfi(rel)->f_nclaves > max_clave)
       max_clave = vg_mpfi(rel)->f_nclaves;
   if (vg_mpfi(rel)->f_nrelaci > max_relaci)
       max_relaci = vg_mpfi(rel)->f_nrelaci;
   if (tam_gene_def[rel] > max_gene)
       max_gene = tam_gene_def[rel];
   }

/* grabar maximos para la memoria de reserva */
graba_short(file1,&(max_panta),sizeof(short));
graba_short(file1,&(max_campos),sizeof(short));
graba_short(file1,&(max_clave),sizeof(short));
graba_short(file1,&(max_relaci),sizeof(short));
graba_short(file1,&(max_gene),sizeof(short));
graba_short(file1,&(max_prog),sizeof(short));
graba_short(file1,&(max_cond),sizeof(short));
graba_short(file1,&(max_bucl),sizeof(short));
graba_short(file1,&(max_oper),sizeof(short));
graba_short(file1,&(max_f_B),sizeof(short));
graba_short(file1,&(max_ex),sizeof(short));

rw_close_(file1);
return(0);
}
#endif

