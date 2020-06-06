/* #define MAIN_ /* para test locales */
/****************************************************************************/
/*                   RUTINAS DE CAMPOS MEMO                                 */
/****************************************************************************/
/* Autor: Francisco Gonzalez Valiente */


/* configurar saltos en terminales !!! */
#define EDISALTO 100    /* numero de lineas*100 de salto en scroll vertical */
#define EDIMUEVE 5      /* desplazamiento horizontal */

#include <fgvsys.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef O_BINARY

#include <io.h>

#else

#define O_BINARY 0
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#ifndef LARGESEEK
extern long rw_lseek_();
#endif
extern unsigned int rw_write_();

#endif

#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <malloc.h>
#include <rwmalloc.h>
#include <rtermin.h>
#include <teclas.h>

extern void (*rentra_pos)();
extern int rentra_edicion;
extern int rentra_offqp;
extern int rentra_offini;
extern int rentra_insert;
extern int rentra_atrib;
extern int rentra_cc;
extern int rentra_cf;
extern int rentra_sal_atrib;
extern int rentra_sal_cc;
extern int rentra_sal_cf;
extern FILE *_imp;

extern int redita_atrib;
extern int redita_cc;
extern int redita_cf;
extern long redita_linea;
extern int redita_col;
extern int repinta_texto;

extern int PCcolor;
extern int PFcolor;

extern int (*memo_pregu_imp)() = NULL;
extern int (*memo_printa_linea)() = NULL;
extern int (*memo_finimp)() =    NULL;

extern int (*ext_dencrp)() =    NULL;
extern int (*ext_encrp)() =    NULL;
extern char *encrp_buf = NULL;

extern char *lee_linea();
extern char **salva_funciones();

static char **texto;
static char **blo;
static int v;
static int qp;
int qpi_rt = 0;
static int tlin;
static int tpan;
static int nlin;
static int lin;
static int ntlin;
static int tini;
static int l_inibloque,
l_finbloque;
static int c_inibloque,
c_finbloque;
static int i_margen;
static int d_margen;
static int tex_a_tope = 0;

extern char **edi_text = NULL;

extern char *puntero_especial_rentra = NULL;

#define W_BLOQUE    11
#define W_RAPIDO    17
#define W_PANTALLA  15
#define W_IMPRESION 16
#define W_BORRALIN  25
#define W_SIGUIENTE 14


/* #define text edi_text */

char **nueva_memo(text,ntlin,n,tex_a_tope)
char **text;
int ntlin,n,tex_a_tope;
{
   /* char *m_tmp; */
   
   if (!tex_a_tope) {
      text = (char **)texrealloc((char *)text,sizeof(char *) * (ntlin + 1 + n));
   }
   return(text);
}


memoria_error()
{
   pito();
   Bl(24);
   pprint(2420,"NO HAY MEMORIA!!!!!!!");
}

static char *_el_titulo = NULL;

void pinta_col(qp,contai,conta,insert)
int qp,contai,conta,insert;
{
   int att = Patrib;
   int atc = PCcolor;
   int atf = PFcolor;
   redita_col = conta + contai + 1;
   
   if (!_el_titulo) return;
   
   atributo(tolower(Patrib));
   Ai();
   cursor(qpi_rt - 194);
   v_printd("%03d",redita_col);
   v_printc("%c ",(insert) ? 'I' : 'O');
   Bi();
   Color(atc,atf);
   atributo(att);
}


static int pinta_pos(qpi_rt,qpf,lin,ntlin,v,titulo)
int qpi_rt,qpf,lin,ntlin,v;
char *titulo;
{
			int e;
         char tmp[81];
         if (!titulo) return(0);
         
         if (v) {
            e = qpf / 100 - qpi_rt / 100 + 1;
            lin = (lin-1) / e + 1;
            ntlin = (ntlin-1) / e + 1;
            e = 'P';
         }
         else {
            e = 'L';
         }
         
         sprintf(tmp,"%c%04d:",e,lin);
         Ai();
         pprint(qpi_rt-200,tmp);
         cursor(qpi_rt - 194);
         v_printd("%03d",rentra_offini + rentra_offqp + 1);
         Bi();
         return(0);
}

int entra_a_que_linea(qpi_rt,lin,ntlin)
int qpi_rt;
int lin;
int ntlin;
{
   int newlin;
   char buf[81];
   lin--;
   sprintf(buf,"%d",lin+1);
   l_entra(qpi_rt-199,0,"0","9999",buf,0,buf);
   newlin = atoi(buf);
   if (newlin > ntlin || newlin < 1)
   {
      pito();
      return(lin);
   }
   return(newlin-1);
}

static int que_atributo_de_bloque()
{
   if (redita_atrib == 'R')
						return('N');
   else
						if (redita_atrib == 'I')
                     return('R');
                  else
                     return('I');
}

static int hay_bloque()
{
   if (l_inibloque < 0 ||
						l_finbloque < 0 ||
                  c_inibloque < 0 ||
                  c_finbloque < 0)
                  return(0);
   return(1);
}

static int esta_en_el_bloque(l_n)
int l_n;
{
   if ( hay_bloque() &&
						l_n >= l_inibloque && l_n <= l_finbloque &&
                  ( (c_inibloque >= tini && c_inibloque < (tini+tpan)) ||
                  (c_finbloque >= tini && c_inibloque < (tini+tpan)) )
                  ) {
						/* LA LINEA COMPRENDE UN TROZO DEL BLOQUE */
						if (c_inibloque <= tini && c_finbloque >= (tini+tpan-1))
                     return(2); /* TODO LO QUE SE VE DE LA LINEA ES BLOQUE */
                  return(1);
   }
   return(0);
}

int pinta_hoja_texto(texto,qpi_rt,qpf,titulo,lin,ntlin,v,tpan,tini,b1,b2,i_margen,d_margen)
char **texto;
int qpi_rt,qpf;
char *titulo;
int lin,ntlin,v,tpan,tini;
char *b1,*b2;
int i_margen,d_margen;
{
   char usi[82];
   char usi_b[82];
   char usi2[82];
   char *b;
   int i,j,k,b_i,b_f;
   char c;
   int ua = 0;
   int atri_blo = que_atributo_de_bloque();
   
   if (titulo) {
      sprintf(usi,"%%-%d.%ds",tpan-11,tpan-11);
      Ai();
      cursor(qpi_rt - 189);
      v_prints(usi,titulo);
      Bi();
      c = ((v) ? '-' : '+');
      j = 0;
      k = tini % 5;
      d_margen--;
      for (i = 1 + k,k += tpan;i <= k;i++,j++) {
         if ((j+tini) == i_margen || (j+tini) == d_margen)
            usi[j] = '|';
         else
            if (!(i%5))
               usi[j]= c;
            else
               usi[j]='-';
      }
      usi[j] = 0;
      Ar();pprint(qpi_rt-100,usi);Cr();
      pinta_pos(qpi_rt,qpf,lin,ntlin,v,titulo);
   }
   
   while (qpi_rt < qpf && *texto) {
      b = *texto;
      if (b1 && b2 == b) {
         b = b1;
         b1 = NULL;
      }
      if ((k = strlen(b) - tini) > 0) {
         if (k > tpan)
            k = tpan;
         memcpy(usi,b+tini,k);
      }
      else
         k = 0;
      
      memset(usi+k,' ',tpan-k);
      usi[tpan] = 0;
      usi_b[0] = 0;
      usi2[0] = 0;
      
      if (!v && esta_en_el_bloque(texto - edi_text)) {
         if (c_inibloque <= tini)
            b_i = 0;
         else
            b_i = c_inibloque - tini;
         
         if (c_finbloque >= (tini+tpan))
            b_f = tpan - b_i;
         else
            b_f = c_finbloque - tini - b_i + 1;
         
         if ((b_f + b_i) > tpan) {
            rw_error(59);
            b_f = tpan - b_i;
									}
         memcpy(usi_b,usi+b_i,b_f);
         usi_b[b_f] = 0;
         memcpy(usi2,usi+b_i+b_f,tpan-b_f-b_i);
         usi2[tpan-b_f-b_i] = 0;
         usi[b_i] = 0;
      }
      
      cursor(qpi_rt);
      if (usi[0]) {
         if (ua != redita_atrib) {
            if (redita_cc != redita_cf)
               Color(redita_cc,redita_cf);
            atributo(redita_atrib);
            ua = redita_atrib;
									}
         v_print(usi);
      }
      if (usi_b[0]) {
         if (ua != atri_blo) {
            atributo(atri_blo);
            ua = atri_blo;
									}
         v_print(usi_b);
      }
      if (usi2[0]) {
         if (ua != redita_atrib) {
            if (redita_cc != redita_cf)
               Color(redita_cc,redita_cf);
            atributo(redita_atrib);
            ua = redita_atrib;
									}
         v_print(usi2);
      }
      
      qpi_rt += 100;
      texto++;
   }
   
   j = 1;
   while (qpi_rt < qpf) {
      if (ua != redita_atrib) {
         if (redita_cc != redita_cf)
            Color(redita_cc,redita_cf);
         atributo(redita_atrib);
         ua = redita_atrib;
      }
      /* OJO quiza queda bloque por pintar con su atributo ! */
      if (b1) {
         if ((k = strlen(b1) - tini) > 0) {
            if (k > tpan)
               k = tpan;
            memcpy(usi,b1+tini,k);
									}
         else
            k = 0;
         memset(usi+k,' ',tpan-k);
         usi[tpan] = 0;
         b1 = NULL;
         j = 1;
      }
      else {
         if (j) {
            memset(usi,' ',tpan);
            usi[tpan] = 0;
									}
         else
            j = 0;
      }
      cursor(qpi_rt);
      v_print(usi);
      qpi_rt += 100;
   }
   
   if (ua)
      atributo(tolower(ua));
}


static int inserta_linea(undo,topelin,margen,m)
char *undo;
int topelin;
int margen;
int m;
{
   char **tmp = edi_text;
   int i;
   
   i = (texto - edi_text);
   if (ntlin < topelin && !m) {
      tmp = nueva_memo(edi_text,ntlin,1,tex_a_tope);
      if (tmp)
         edi_text = tmp;
      else
         memoria_error();
   }
   texto = edi_text + i;
   i = ntlin - i;
   while (i) {
      i--;
      *(texto+i+1) = *(texto+i);
   }
   if (!strlen(undo))
      margen = 0;
   if ((*texto = texmalloc(strlen(undo) + 1 + margen)) != NULL) {
      memset(*texto,' ',margen);
      strcpy(*texto + margen,undo);
   }
   else
      memoria_error();
   
   if (ntlin < topelin && tmp)
      ntlin++;
   else
   {
      if (*(edi_text + ntlin))
      {
         free(*(edi_text + ntlin));
      }
   }
   
   *(edi_text + ntlin) = NULL;
   
   nlin = ntlin + 1;
   if (nlin > topelin) nlin = topelin;
   return(ntlin);
}


static int borra_linea(text,texto,ntlin)
char **text,**texto;
int ntlin;
{
   free(*texto);
   memcpy((char *)texto,(char *)(texto+1),(int)((text + ntlin) - texto) * sizeof(char *));
   ntlin--;
   *(text + ntlin) = NULL;
   return(ntlin);
}


static int imprime_texto(text) /* ATENCION se usan funciones del agisys-diagram!!! */
char **text;
{
			int a = rentra_edicion;
         int b = rentra_offqp;
         int c = rentra_insert;
         int d = rentra_offini;
         int att = rentra_atrib;
         
         if (!memo_pregu_imp || !memo_finimp) return(0);
         
         rentra_edicion = 0;
         rentra_offqp = 0;
         rentra_insert = 0;
         rentra_atrib = 'R';
         
         
         if (!(*memo_pregu_imp)(0)) {
            if (_imp)
               while(*text) {
                  if (memo_printa_linea)
                  {
                     (*memo_printa_linea)(*text,0);
                  }
                  else
                  {
                     graba_linea(*text,_imp);
                  }
                  text++;
               }
               (*memo_finimp)();
         }
         
         rentra_edicion = a;
         rentra_offqp = b;
         rentra_insert = c;
         rentra_offini = d;
         rentra_atrib = att;
         return(0);
}


static int pinta_subti(c_v)
int c_v;
{
   static char *_memo_edit = "F12=Bor Fn2=Rec Fn3=Ini Fn4=Fin Fn5=Bus Fn6=Ree Fn7=Ibl Fn8=Fbl Fn9=Mar ";
   static char *_memo_visu = "Return para Editar";
   Ai();
   if (c_v == 2)
      pprint(2401,_memo_visu);
   else
      if (!c_v) {
         pprint(2401,_memo_edit);
         if (memo_pregu_imp)
            pprint(2473,"F10=Imp");
						}
      Bi();
}


static int ajusta_off(texto,i_margen)
char *texto;
int i_margen;
{
   int i,j = rentra_offqp;
   
   if (!texto)
   {
						rentra_offini = 0;
                  rentra_offqp = 0;
                  return(0);
   }
   
   if ((j || rentra_offini) && (i = strlen(texto)) < (rentra_offini+j)) {
						if (((rentra_offqp = i) - rentra_offini) < 0) {
                     rentra_offini = 0;
                  }
                  else
                     rentra_offqp -= rentra_offini;
                  if ( (rentra_offqp + rentra_offini) < i_margen ) {
                     rentra_offqp = i_margen - rentra_offini;
                     if (rentra_offqp < 0) {
                        rentra_offini += rentra_offqp;
                        rentra_offqp = 0;
                     }
                  }
   }
   return(0);
}

static f_pprint(qp,buf,tpan,at)
int qp;
char *buf;
int tpan;
{
   int i,j,ua = 0;
   
   if ((j = tpan - strlen(buf)) < 0) {
      i = buf[tpan];
      buf[tpan] = 0;
   }
   
   if (!at)
   {
      if (redita_cc != redita_cf)
         Color(redita_cc,redita_cf);
      atributo(redita_atrib);
      ua = redita_atrib;
   }
   
   pprint(qp,buf);
   if (j < 0)
      buf[tpan] = i;
   else {
      while(j) {
         v_print(" ");
         j--;
      }
   }
   
   if (ua)
      atributo(tolower(ua));
   
}

static int l_entra(qp,maxqc,qi,qs,qd,qh,qo)
int qp,
maxqc,
qh;
char *qi,*qs,*qd,*qo;
{
   int a = rentra_edicion;
   int b = rentra_offqp;
   int d = rentra_offini;
   int c = rentra_insert;
   int att = rentra_atrib;
   int qj;
   
   rentra_edicion = 0;
   rentra_offqp = 0;
   rentra_insert = 0;
   rentra_offini = 0;
   rentra_atrib = 'R';
   
   qj = entra(qp,maxqc,qi,qs,qd,qh,qo);
   
   rentra_atrib = att;
   rentra_edicion = a;
   rentra_offqp = b;
   rentra_offini = d;
   rentra_insert = c;
   return(qj);
}

static int busca_texto(buf,titulo,qpi_rt,qpf,tcol,_susti)
char *buf;
char *titulo;
int qpi_rt,
qpf,
tcol,
*_susti;
{
   char tmp[257];
   char sus[32];
   int i,
      j,
      jj,
      nn,
      k,
      n,
      off,
      pintar,
      ol,
      of,
      pas,
      si_sus,
      susti = *_susti,
      nlin = ntlin + 1 - v;
   int b = rentra_offqp;
   int d = rentra_offini;
   int si_salir = 0;
   int encontrado = 0;
   
   if (!*texto) texto--;
   
   if (texto < edi_text) return(0);
   
   if (susti == 2 || susti == 3) {
      susti = 0;
      si_salir = 1;
   }
   
   if (!si_salir || !strlen(buf) || *_susti == 3) {
      Bl(24);
      pprint(2401,"Busca:");
      l_entra(2407,20,"","",buf,0,buf);
      quitab(buf);
      if (si_salir)
         *_susti = 0;
   }
   
   if (strlen(buf)) {
      if (susti) {
         pprint(2428,"Reemplaza:");
         l_entra(2438,20,"","","",0,sus);
         quitab(sus);
      }
      strminus(buf);
      ol = (int)(texto - edi_text); /* equivale a lin + 1 */
      of = (((d+b+1) < (of = strlen(*texto))) ? d + b + 1 : of);
      pas = 0; /* evitar reciclo en sustitucion */
      si_sus = 0;
      do {
         i = (int)(texto - edi_text); /* equivale a lin + 1 */
         k = 0;
         pintar = 0;
         off = (((d+b+1) < (off = strlen(*texto))) ? d + b + 1 : off);
         if (!si_sus) {
            jj = 0;
            nn = ((off) ? ntlin + 1 : ntlin);
         }
         while(jj < nn) {
            if (*(edi_text+i)) {
               strcpy(tmp,*(edi_text+i));
               strminus(tmp);
               if ((k = strbusca(tmp+off,buf)) != -1) {
                  k += off;
                  break;
               }
               off = 0;
               jj++;
               i++;
												}
            else {
               i = 0;
               pas = 1;
												}
         }
         
         if (jj < nn && (!susti || !pas || i < ol || k < of)) {
            encontrado = 1;
            i -= (int)(texto - edi_text);
            qp += i*100;
            texto += i;
            lin += i;
            j = strlen(buf);
            if (k < d) {
               d = k;
               b = 0;
               pintar = 1;
												}
            else
               if (k > (d+tpan-j)) {
                  d = (k-tpan+j);
                  b = tpan - j;
                  pintar = 1;
               }
               else
                  b = k - d;
               
               if (qp <= qpi_rt) {
                  pintar = 1;
                  qp = qpi_rt + (tlin/2)*100;
               }
               else
                  if (qp >= qpf) {
                     qp = qpi_rt + (tlin/2)*100;
                     pintar = 1;
                  }
                  if (edi_text > (texto - ((qp/100) - (qpi_rt/100)))) {
                     qp = qpi_rt + (texto - edi_text) * 100;
                     pintar = 1;
                  }
                  
                  if (pintar) {
                     pinta_hoja_texto(texto - (qp / 100) + (qpi_rt / 100),qpi_rt,qpf,titulo,(int)(texto-edi_text+1),nlin,v,tpan,d,NULL,NULL,i_margen,d_margen);
                     pintar = 0;
                  }
                  if (!si_salir) {
                     Ar();f_pprint(qp+b,*texto + k,j,1);Cr();
                     i = 0;
                     
                     if (!si_sus)
                     {
                        Ai();
                        pprint(2401,"B");
                        if (susti)
                        {
                           pprint(2428,"R");
                           pprint(2459,"Todos");
                        }
                        Bi();
                        curon(1);
                        cursor(qp+b);
                        if ((i = leecar()) > 64 && i < 91) i += 32;
                        if (i == W_SIGUIENTE) {
                           i = 'b';
                        }
                        curon(0);
                     }
                  }
                  else
                     break; /* salir por ser busqueda de uno */
                  
                  if (susti && i == 'r' || i == 't' || si_sus) {
                     if (i == 't')
                        si_sus = 1;
                     /* sustitucion !!! */
                     i = strlen(sus);
                     if (i == j) {
                        memcpy(*texto+k,sus,j);
                     }
                     else {
                        if (ol == (int)(texto - edi_text) && (k + ((i < j) ? i : j)) < of)
                           of += (i - j); /* reajustar posicion original */
                        n = strlen(*texto);
                        if ( (n + i - j) > tcol ) {
                           memoria_error();
                           break;
                        }
                        else {
                           memcpy(tmp,*texto,k);
                           memcpy(tmp+k,sus,i);
                           memcpy(tmp+k+i,*texto+k+j,(n -= (k + j)));
                           tmp[ (i += (k+n)) ] = 0; /* nuevo len de *texto */
                           free(*texto);
                           if ((*texto = texmalloc(i+1)) != NULL) {
                              memcpy(*texto,tmp,i+1);
                           }
                           else
                              memoria_error();
                           j = tpan - k + d;
                        }
                     }
                     i = 'b';
                  }
                  
                  f_pprint(qp+b,*texto + k,j,0);
                  if (!si_sus && i != 'b')
                     break;
           }
           else
              break;
       } while(-1);
    }
    if (!encontrado)
       pito();
    if (*_susti < 2)
       Bl(24);
    rentra_offqp = b;
    rentra_offini = d;
    return(0);
}

static int entra_margen(tcol,m)
int tcol;
int m;
{
   char buf[10];
   char li1[10];
   char li2[10];
   
   Bl(24);
   if (!m || m == 1) {
      pprint(2401,"Margen Izquierdo:");
      sprintf(buf,"%d",(i_margen)+1);
      sprintf(li1,"%d",((d_margen <= tpan) ? d_margen - 1 : tpan));
      l_entra(2418,0,"1",li1,buf,0,buf);
      quitab(buf);
      i_margen = atoi(buf) - 1;
   }
   
   if (!m || m == 2) {
      pprint(2422,"Margen Derecho:");
      sprintf(li1,"%d",i_margen + 1);
      sprintf(li2,"%d",tcol);
      sprintf(buf,"%d",d_margen);
      l_entra(2437,0,li1,li2,buf,0,buf);
      quitab(buf);
      d_margen = atoi(buf);
   }
}

static char **libera_bloque(blo)
char **blo;
{
   int i = 0;
   
   if (blo) {
      while(*(blo+i)) {
         free(*(blo+i));
         i++;
      }
      free(blo);
			}
   return(NULL);
}

static int ajusta_bloque(m)
int m;
{
			int t;
         
         
         if (l_inibloque == l_finbloque && c_inibloque == c_finbloque) {
            l_finbloque = -1;
            c_finbloque = -1;
            if (m) { /* si coninciden al marcar el fin del bloque se anula */
               l_inibloque = -1;
               c_inibloque = -1;
            }
         }
         else {
            if (l_inibloque > -1 && l_finbloque > -1) {
               t  = ((l_inibloque < l_finbloque) ? l_inibloque : l_finbloque);
               l_finbloque = ((l_finbloque > l_inibloque) ? l_finbloque : l_inibloque);
               l_inibloque = t;
            }
            if (c_inibloque > -1 && c_finbloque > -1) {
               t  = ((c_inibloque < c_finbloque) ? c_inibloque : c_finbloque);
               c_finbloque = ((c_finbloque > c_inibloque) ? c_finbloque : c_inibloque);
               c_inibloque = t;
            }
         }
}


static int es_final(c)
char c;
{
   if (c == '.' || c == ':')
      return(1);
   return(0);
}

static int limpia_buf(buf)
char *buf;
{
   int i,j,n;
   /* quitar blancos innecesarios excepto los iniciales */
   j = 0;
   while(buf[j] == ' ') j++; /* saltar blancos iniciales */
   i = 0;
   while(j < strlen(buf)) {
						if (i == ' ' && buf[j] == ' ') {
                     for(n = j;n < strlen(buf);n++)
                        buf[n] = buf[n+1];
                  }
                  else {
                     i = buf[j];
                     j++;
                  }
   }
}

static char **recupera_bloque(topelin,blo,tcol,qpi_rt,modo)
int topelin;
char **blo;
int tcol,qpi_rt;
int modo; /* 0 = memoria 1 = fichero < 0 encriptado */
{
   char **bl = blo;
   int inic = rentra_offini + rentra_offqp;
   int i,j,car,n = (int)(texto - edi_text);
   int m,max = 0,kk;
   char tmp[514];
   char **tx1;
   int _tlin = (tlin - 1) * 100;
   int control_tope;
   
   if (!blo) return(blo);
   
   /* establecer nuevo bloque */
   l_inibloque = n;
   c_inibloque = inic;
   l_finbloque = 0;
   m = 0;
   if (modo || rentra_insert || !*texto) { /* alojar la memoria antes */
      j = 0;
      if (modo) {
      /*
      saco el numero exacto de lineas ---> va el doble de lento
      pero evita problemas en memoria si el fichero es muy grande
         */
         if (modo < 0)
									{
            while (rw_read_((int)blo,encrp_buf,512) == 512)
            {
               (*ext_dencrp)(encrp_buf);
               kk = 0;
               while(kk < 510 && encrp_buf[kk] != 4)
               {
                  strcpy(tmp,encrp_buf+kk);
                  kk += (strlen(tmp) + 1);
                  j++;
               }
            }
            rw_lseek_((int)blo,0L,SEEK_SET);
									}
         else
									{
            while(lee_linea(tmp,259,(FILE *)blo)) j++;
            rewind((FILE *)blo);
									}
      }
      else {
         while(*(bl+j)) j++; /* contar ampliaciones necesarias */
      }
      
      if ((j + ntlin) > topelin) j = topelin - ntlin;
      
      if (j > 0) {
         tx1 = edi_text;
         edi_text = nueva_memo(edi_text,ntlin,j,tex_a_tope); /* ampliar de una vez */
         if (!edi_text) {
            memoria_error();
            edi_text = tx1;
            return(blo);
									}
         texto = edi_text + n;
         m = 1;
      }
      control_tope = j;
   }
   
   while( -1 ) {
      if (!modo)
      {
         if (!*bl) break;
      }
      else
      {
         if (!control_tope)
									{
            break;
									}
         control_tope--;
         if (modo < 0)
									{
            if (rw_read_((int)blo,encrp_buf,512) != 512) break;
            (*ext_dencrp)(encrp_buf);
            kk = 0;
									}
         else
									{
            if (!lee_linea(tmp,259,(FILE *)blo)) break;
									}
      }
      do {
         if (modo < 0)
									{
            if (kk >= 510 || encrp_buf[kk] == 4) break;
            strcpy(tmp,encrp_buf+kk);
            kk += (strlen(tmp) + 1);
									}
         
         if (!modo)
            j = strlen(*bl);
         else
            j = strlen(tmp);
         if (j > max) max = j;
         
         if (!modo && !rentra_insert && *texto) {
            if ((j = strlen(*texto)) > inic) {
               memcpy(tmp,*texto,inic);
               strcpy(tmp+inic,*bl);
               strcat(tmp,*texto+inic);
            }
            else {
               memcpy(tmp,*texto,j);
               memset(tmp+j,' ',inic-j);
               strcpy(tmp+inic,*bl);
            }
            quitab(tmp);
            j = strlen(tmp);
            if (j > tcol) {j = tcol;tmp[j] = 0;pito();}
            free(*texto);
            if ((*texto = texmalloc(j + 1)) != NULL)
               strcpy(*texto,tmp);
            else
               memoria_error();
									}
         else {
            ntlin = inserta_linea(((modo) ? tmp : *bl),topelin,inic,m);
            if (!m)
               texto = edi_text + n;
            if (n >= (int)(texto - edi_text)) {
               if (n < ntlin) {
                  n++;
                  if (qp < (qpi_rt + _tlin)) qp += 100;
               }
            }
									}
         texto++;
         bl++;
         l_finbloque++;
         if (modo >= 0)
            break;
      } while(-1);
   }
   
   /* texto = edi_text + n; */
   
   if (rentra_insert) {
      c_inibloque = 0;
      c_finbloque = tcol-1;
   }
   else {
      c_finbloque = c_inibloque + max;
      c_finbloque--;
   }
   
   l_finbloque += l_inibloque;
   l_finbloque--;
   ajusta_bloque(1);
   if (!modo)
      libera_bloque(blo);
   return(NULL);
}

static char **borra_bloque(topelin,blo,m,qpi_rt)
int topelin;
char **blo;
int m, /* 0 = saca y borra , 1 = solo saca , 2 = graba lineas */
qpi_rt;
{
   int nl = l_finbloque - l_inibloque;
   int nc = c_finbloque - c_inibloque;
   int inic = c_inibloque;
   char **tx = edi_text + l_inibloque;
   int i,j,car,jj,hh;
   int cripto = 0;
   if (m < 0)
   {
      m = 2;
      cripto = 1;
      memset(encrp_buf,0,512);
      hh = 0;
      jj = 0;
   }
   
   if (m != 2 && blo) libera_bloque(blo);
   
   if (l_inibloque >= ntlin) {
      l_inibloque = -1;
      l_finbloque = -1;
      c_inibloque = -1;
      c_finbloque = -1;
      return(NULL);
   }
   
   nl++;nc++;
   if (m != 2) {
      blo = (char **)texmalloc(sizeof(char *) * (nl+2));
      if (!blo) {
         memoria_error();
         return(blo);
      }
   }
   for(i = 0;i < nl;i++,tx++) {
      if (!*tx) {
         nl = i;
         break;
      }
      if ((j = strlen(*tx) - inic) <= 0)
         car = 0;
      else
         car = ((j > nc) ? nc : j);
      
      if (m == 2) {
         if (!cripto)
            graba_linea(*tx,(FILE *)blo);
         else
         {
            jj = strlen(*tx);
            if ( (hh + jj + 1) > 510 )
            {
               (*ext_encrp)(encrp_buf,hh);
               rw_write_((int)blo,encrp_buf,512);
               hh = 0;
            }
            memcpy(encrp_buf+hh,*tx,jj+1);
            hh += (jj+1);
         }
      }
      else {
         if (!rentra_insert) {
            if ((*(blo+i) = texmalloc( nc+2 )) != NULL) {
               memcpy(*(blo+i),*tx+inic,car);
               memset(*(blo+i) + car,' ',nc - car);
               *(*(blo+i) + nc) = 0;
            }
            else
               memoria_error();
         }
         else {
            if ((*(blo+i) = texmalloc( car+2 )) != NULL) {
               if (car)
                  strcpy(*(blo+i),*tx+inic);
               else
                  *(*(blo+i)) = 0;
            }
            else
               memoria_error();
         }
      }
      if (!m) {  /* borrado efectivo del bloque */
         if (!inic && j <= nc) {
            ntlin = borra_linea(edi_text,tx,ntlin);
            if (texto > tx) {
               texto--;
               if (qp > qpi_rt) qp -= 100;
            }
            tx--;
         }
         else {
            if (car)
               strcpy(*tx + inic,*tx + inic + car);
         }
      }
   }
   if (m != 2)
      *(blo+nl) = NULL;
   
   if (cripto && hh)
   {
      (*ext_encrp)(encrp_buf,hh);
      rw_write_((int)blo,encrp_buf,512);
   }
   
   nlin = ntlin + 1;
   if (l_inibloque >= ntlin) {
      l_inibloque = -1;
      l_finbloque = -1;
      c_inibloque = -1;
      c_finbloque = -1;
   }
   return(blo);
}

static int justi_linea(buf,col,siblo)
char *buf;
int col,siblo;
{
   int i,j,r,n,k,h,s,c;
   
   r = -1;
   j = 0;
   while(strlen(buf+j)) {
      r++;
      /* primero ver si hay necesidad de justificacion */
      if ((i = strlen(buf+j)) > col)
         i = col;
      else            /* la linea es mas peque¤a!!! */
         return(r); /* si es mas peque¤a no se justifica */
      
      while(i && buf[j+i] != ' ') i--;
      if (!i) {  /* todo es una sola palabra!! */
         while(buf[j+i] && buf[j+i] != ' ') i++;
         if (buf[j+i]) {
            buf[j+i] = 0;
            if (strlen(buf+j+i+1))
               r++;
         }
         return(r);
      }
      
      /* correr el resto */
      for (n = strlen(buf+j+i+1);n > -1;n--)
         buf[j+col+n+1] = buf[j+i+n+1];
      
      n = col-i;
      if (n)
         memset(buf+j+i+1,' ',col-i);
      /* justificar desde atras */
      buf[j+col] = 0;
      k = 0;
      while(buf[j+k] == ' ') k++; /* no usar espacios delanteros */
      while(n) {
         s = 0;
         c = 0;
         for (h = i-1;h > k && n;h--) {
            if (!s) {
               if (buf[j+h] == ' ') {
                  i++;
                  for (s = i-1;s > h;s--)
                     buf[j+s] = buf[j+s-1]; /* corre y pone el espacio */
                  s = 1;
                  n--;
                  c = 1;
               }
            }
            else {
               if (buf[j+h] != ' ')
                  s = 0;
            }
         }
         if (!c)
            break; /* no se puede justificar */
      }
      
      j += (col+1);
   }
   return(r);
}

static void justifica(topelin,qpi_rt)
int topelin;
int qpi_rt;
{
   int i = 0,j,n,k,siblo;
   int col = d_margen - i_margen;
   char *buf = texmalloc(1024);
   char *tmp;
   
   if (!buf) {
      memoria_error();
      return;
			}
   
   while((tmp = *(edi_text + i) && i < topelin) != NULL) {
      if ((n = strlen(tmp)) != 0) {
         for (j = 0;j < i_margen && j < n;j++) {
            if (tmp[j] != ' ')
               break;
         }
         strcpy(buf,tmp+j);
         free(tmp);
         do {
            if (*(edi_text + i + 1) && strlen(*(edi_text + i + 1)))
               siblo = 1;
            else
               siblo = 0;
            limpia_buf(buf);
            quitab(buf);
            n = strlen(buf);
            if (siblo && !es_final(buf[n-1]) && n < col) {
               strcat(buf," ");
               tmp = *(edi_text + i + 1);
               n = 0;
               while(tmp[n] && tmp[n] == ' ') n++;
               strcat(buf,tmp+n);
               texto = edi_text + i;
               ntlin = borra_linea(edi_text,texto+1,ntlin);
            }
            else
               break;
         } while(-1);
         j = justi_linea(buf,col,siblo);
         k = strlen(buf) + 1;
         tmp = texmalloc(i_margen+k);
         if (!tmp) {
            memoria_error();
            return;
         }
         memset(tmp,' ',i_margen);
         strcpy(tmp+i_margen,buf);
         *(edi_text + i) = tmp;
         i++;
         if (j > 0) { /* !!! lo que sobra */
            while(j > 1) {
               texto = edi_text + i; /* insertar */
               ntlin = inserta_linea(buf+k,topelin,i_margen,0);
               j--;
               k += (strlen(buf+k) + 1);
               i++; /* estas lineas ya estan justificadas */
            }
            if (strlen(buf+k)) {
               if (strlen(buf+k) > col) {
                  texto = edi_text + i;
                  ntlin = inserta_linea(buf+k,topelin,i_margen,0);
                  /* se inserta per luego se justifica !! */
               }
               else
                  if (siblo) {
                     tmp = *(edi_text + i);
                     for (j = 0;j < i_margen && j < n;j++) {
                        if (tmp[j] != ' ')
                           break;
                     }
                     strcpy(buf,buf+k);
                     if (tmp[j] != ' ')
                        strcat(buf," ");
                     strcat(buf,tmp+j);
                     free(tmp);
                     tmp = texmalloc(strlen(buf)+j+1);
                     if (!tmp) {
                        memoria_error();
                        return;
                     }
                     memset(tmp,' ',j);
                     strcpy(tmp+j,buf);
                     *(edi_text + i) = tmp;
                  }
                  else {
                     texto = edi_text + i; /* insertar */
                     ntlin = inserta_linea(buf+k,topelin,i_margen,0);
                     i++; /* esto no se ha de justificar ! */
                  }
            }
         }
      }
      else
         i++;
			}
   
   /* ajustar qp al final de la pantalla */
   if (*texto)
      texto++;
   i = (int)(texto - edi_text);
   if (i >= tlin)
      i = tlin - 1;
   qp = qpi_rt + i*100;
   
   nlin = ntlin + 1;
   free(buf);
}



int edita_texto(tx,_qpi,qpf,topelin,titulo,c_v,tcol,ol,oc)
char ***tx;
int _qpi,qpf,topelin;
char *titulo;
int *c_v;
int tcol;
long *ol;
int *oc;
{
   char **tmp;
   char buf[514],*ant;
   char busca[31];
   int qc = -1,posi;
   int i,j,k,r = 0;
   int pintar = 1;
   char *undo;
   VENT *l24;
   int atope = tex_a_tope;
   char **_text  = edi_text;
   char **_texto = texto;
   char **_blo = blo;
   int _v = v;
   int _qp = qp;
   int _tlin = tlin;
   int _tpan = tpan;
   int _nlin = nlin;
   int _lin = lin;
   int _ntlin = ntlin;
   int _tini = tini;
   int _l_inibloque = l_inibloque;
   int _l_finbloque = l_finbloque;
   int _c_inibloque = c_inibloque;
   int _c_finbloque = c_finbloque;
   int _i_margen = i_margen;
   int _d_margen = d_margen;
   long ool = redita_linea;
   int ooc = redita_col;
   int ata = rentra_edicion;
   int atb = rentra_offqp;
   int atc = rentra_insert;
   int atd = rentra_offini;
   int att = rentra_atrib;
   int acc = rentra_cc;
   int acf = rentra_cf;
   int atts = rentra_sal_atrib;
   int accs = rentra_sal_cc;
   int acfs = rentra_sal_cf;
   int rept = repinta_texto;
   int __qpi = qpi_rt;
   int cripto = 1;
   void (*atp)() = rentra_pos;
   char **fns = salva_funciones(0,8 + ((memo_pregu_imp) ? 1 : 0));
   void *salva_x = NULL;
   char *ant_texto;
   char *p_e_r_e = puntero_especial_rentra;
   char *__el_titulo = _el_titulo;
   int modo_externo = (*c_v) / 100;
   
   *c_v %= 100;
   
   _el_titulo = titulo;
   
   puntero_especial_rentra = buf;
   
   if (tcol < 0)
   {
      tcol = (-tcol) - 1;
      if (ext_dencrp && ext_encrp && encrp_buf)
         cripto = -1;
   }
   qpi_rt = _qpi;
   
   repinta_texto = 0;
   
   rentra_pos = pinta_col;
   edi_text  = *tx;
   texto = edi_text;
   blo = NULL;
   v = ((*c_v) ? 1 : 0);
   qp = qpi_rt;
   tlin = (qpf / 100) - (qpi_rt / 100) + 1;
   tpan = (qpf % 100) - (qpi_rt % 100) + 1;
   nlin = 1;
   lin = 1;
   ntlin = 0;
   tini = 0;
   l_inibloque = -1;
   l_finbloque = -1;
   c_inibloque = -1;
   c_finbloque = -1;
   i_margen = 0;
   d_margen = 0;
   
   
   if (topelin < 0) {tex_a_tope = 1;topelin = -topelin;}
   if (topelin > 8000) topelin = 8000;
   
   if (modo_externo && DespachadorExterno)
   {
      char xxtmp[5300];
      int area = _qpi * 10000 + qpf;   
      char *ppp = xxtmp + sizeof(int);
      int cuenta_l;
      r = 0;
      memcpy(xxtmp,&r,sizeof(int));
      ppp[0] = 0;
      for (i = 0;texto[i];i++)
      {
         if (ppp < (xxtmp+5000))
         {
            strcpy(ppp,texto[i]);
            quitab(ppp);
            ppp += strlen(ppp);
            *ppp = 13;
            ppp++;
            *ppp = 10;
            ppp++;
            *ppp = 0;
         }
         free(texto[i]);
      }
      if (!tex_a_tope)
         free(texto);
      (*DespachadorExterno)((int)DESPACHO_EDITA,(int)area,xxtmp);
      r = 0;
      cuenta_l = 0;
      memcpy(&r,xxtmp,sizeof(int));
      for (i = sizeof(int);xxtmp[i];i++)
      {
         if (xxtmp[i] == 13 && (xxtmp[i+1] == 10 || xxtmp[i+1] == 13))
         {
            cuenta_l++;
         }
      }
      if (!tex_a_tope)      
         texto = (char **)malloc(sizeof(char *) * (cuenta_l+1));
      
      edi_text = texto;
      cuenta_l = 0;
      j = sizeof(int);
      for (i = sizeof(int);xxtmp[i] && cuenta_l < topelin;i++)
      {
         if (tex_a_tope && cuenta_l >= topelin) break;

         if (xxtmp[i] == 13 && (xxtmp[i+1] == 10 || xxtmp[i+1] == 13))
         {
            xxtmp[i] = 0;
            quitab(xxtmp+j);
            texto[cuenta_l] = malloc(strlen(xxtmp+j)+1);
            strcpy(texto[cuenta_l],xxtmp+j);
            if (xxtmp[i+1] == 10)
            {
               j = i + 2;
               i++;
            }
            else
            {
               j = i + 3;
               i += 2;
            }
            cuenta_l++;
            if (!xxtmp[i]) 
                break;
         }
      }      
      texto[cuenta_l] = NULL;
      *tx = texto;
      goto memo_salida;
   }
   
   if (!edi_text || tpan < 20 || tlin < 2 || !topelin) {r = -1;goto memo_salida;}
   
   busca[0] = 0;
   
   if (ol)
      redita_linea = *ol;
   if (oc)
      redita_col = *oc;
   
   if (!tcol) tcol = tpan;
   l24 = w_ini(2401,2480);
   w_salva(l24);
   Bl(24);
   
   d_margen = tcol;
   undo = texmalloc(tcol + 1);
   if (!undo) {
      memoria_error();
      r = -1;goto memo_salida;
   }
   
   *undo = 1;
   
   while ( *(texto + ntlin) && ntlin < topelin) ntlin++;
   if (ntlin == topelin && *(texto + ntlin))
   {
      salva_x = *(texto + ntlin);
      *(texto + ntlin) = NULL;
   }
   
   nlin = ntlin + 1 - v;
   if (nlin > topelin) nlin = topelin;
   
   pinta_subti(*c_v);
   
   tini = rentra_offini = 0;/* estos valores se deberan poder pasar como paarametros */
   rentra_offqp = i_margen;
   
   if (redita_linea) {
      redita_linea--;
      if (redita_linea < nlin)
         texto += redita_linea;
      else
         texto += nlin;
      qp = qpi_rt + (tlin/2) * 100;
      if ( (texto - edi_text) < ((qp/100) - (qpi_rt/100)) )
         qp = qpi_rt + (texto - edi_text) * 100;
   }
   if (redita_col) {
      redita_col--;
      if (redita_col < tcol)
         rentra_offqp = redita_col;
      else
         rentra_offqp = tcol;
   }
   
   rentra_insert = 1;
   
   while(-1) {
      if (repinta_texto) {
         if (redita_linea) {
            redita_linea--;
            if (redita_linea < nlin)
            {
               texto = edi_text+redita_linea;
            }
            else
            {
               texto = edi_text+nlin;
            }
         }
         if (redita_col) {
            redita_col--;
            rentra_offini = 0;
            if (redita_col < tcol)
               rentra_offqp = redita_col;
            else
               rentra_offqp = tcol;
         }
         repinta_texto = 0;
         pintar = 1;
      }
      
      redita_linea = texto-edi_text+1;
      
      
      if (l_inibloque >= ntlin) {
         l_inibloque = -1;
         l_finbloque = -1;
         c_inibloque = -1;
         c_finbloque = -1;
      }
      else
         if (l_inibloque == l_finbloque && c_inibloque == c_finbloque) {
            l_inibloque = l_finbloque = c_inibloque = c_finbloque = -1;
         }
         
         if (pintar) {
            if (tini == rentra_offini) /* en caso contrario ya se encarga qj 1000 y qj 2000 */
               pinta_hoja_texto(texto - (qp / 100) + (qpi_rt / 100),qpi_rt,qpf,titulo,(int)(texto-edi_text+1),nlin,v,tpan,tini,NULL,NULL,i_margen,d_margen);
            pintar = 0;
         }
         else
            pinta_pos(qpi_rt,qpf,(int)(texto-edi_text+1),nlin,v,titulo);
         
         
         if (v) {
            curon(1);
            cursor(qpi_rt);
            qc = que_qjes((i = leecar()),0);
            curon(0);
            if (i == IZQUIERDA) {
               if (tini) {
                  tini -= EDIMUEVE;
               }
               else {
                  tini = tcol - tpan;
               }
               pintar = 1;
            }
            else
               if (i == DERECHA) {
                  if (tini == (tcol - tpan))
                     tini = 0;
                  else
                     tini += EDIMUEVE;
                  pintar = 1;
               }
               else
                  if (i == HOME) {
                     tini = 0;
                     pintar = 1;
                  }
                  if (tini < 0)
                     tini = 0;
                  else
                     if (tini > (tcol - tpan))
                        tini = tcol - tpan;
                     rentra_offini = tini;
         }
         else {
            /******* EDICION *********/
            
            if ( *texto ) {
               ant = *texto;
               strcpy(buf,ant);
            }
            else {
               buf[0] = 0;
               ant = buf;
               if (qc) {
                  rentra_offini = 0;
                  rentra_offqp = i_margen;
                  memset(buf,' ',i_margen);
                  buf[i_margen] = 0;
               }
            }
            
            do {
               if (tini > rentra_offini && rentra_offqp < tpan) {
                  qc = 2000;
                  strcpy(buf,ant);
               }
               else {/* para el caso cr se puede usar un control para la entra() */
                  if (!rentra_offqp || (rentra_offqp+rentra_offini) >= tcol )
                     rentra_offqp = -2;
                  rentra_edicion = d_margen;
                  
                  if (esta_en_el_bloque(texto-edi_text) == 2) {
                     rentra_atrib = que_atributo_de_bloque();
                     rentra_cc = 0;
                     rentra_cf = 0;
                  }
                  else {
                     rentra_atrib = redita_atrib;
                     rentra_cc = redita_cc;
                     rentra_cf = redita_cf;
                  }
                  
                  ant_texto = *texto;
                  *texto = buf; /* esto es para que se pueda cambiar
                  desde proceso externo */
                  qc = entra(qp,tcol,"","",ant,tpan*10+2,buf);
                  *texto = ant_texto;
                  /* se hace en funcion de pintar posicion */
                  /* redita_col = rentra_offini + rentra_offqp + 1; */
               }
               quitab(buf);
               if (qc < 1000) {
                  if (rentra_offini != tini) {
                     rw_error(20);
                     /* printf("\n\rerror interno memo desp.xxx.\n\r");
                     getchar(); */
                  }
                  break;
               }
               if (qc == 1000) { /* desplazamiento a la derecha */
                  if ((tini = EDIMUEVE - rentra_offini + tini) > 0) {
                     if ((rentra_offini + tpan + tini) > tcol) {
                        tini = tcol - rentra_offini - tpan;
                     }
                     rentra_offini += tini;
                     if (rentra_offqp < tini) {
                        rw_error(21);
                        /* printf("\n\rerror interno mem desp.1.xxx.\n\r");
                        getchar(); */
                     }
                     else
                        rentra_offqp -= tini;
                  }
               }
               else
                  if (qc == 2000) { /* desplazamiento a la izquierda */
                     if ((tini = EDIMUEVE - tini + rentra_offini) > 0) {
                        if ((rentra_offini - tini) < 0) {
                           tini = rentra_offini;
                        }
                        rentra_offini -= tini;
                        rentra_offqp += tini;
                     }
                  }
                  tini = rentra_offini;
                  pinta_hoja_texto(texto - (qp / 100) + (qpi_rt / 100),qpi_rt,qpf,titulo,(int)(texto-edi_text+1),nlin,v,tpan,tini,buf,ant,i_margen,d_margen);
            } while(-1);
            
            posi = 0;
            if ((!qc || qc == -3) && rentra_insert) {
               /* insercion de texto */
               if (qc == -3) 
               {
                  if ((i = strlen(buf)) > tcol)
                     j = i = tcol;
                  else
                     j = i - 1;
                     /* siempre que se pone texto de una linea a otra se
                  respeta la palabra! */
                  while(j && buf[j] != ' ') j--;
                  if (j)
                  {
                     i = ++j;
                     j -= rentra_offini;
                     if (j < tpan)
                     {
                        buf[512] = 0;
                        f_pprint(qp+j,buf+512,tpan-j,0);
                     }					   
                     strcat(buf+i," ");
                  }
                  if (rentra_offqp == tcol)
                  {
                     quitab(buf+i);
                     posi = strlen(buf+i);
                  }
               }
               else
                  i = rentra_offqp+rentra_offini;
               if (i >= strlen(buf)) {
                  buf[i] = 0;
               }
               if (!*texto)
               {
                  tmp = nueva_memo(edi_text,ntlin,1,tex_a_tope);
                  if (!tmp)
                  {
                     memoria_error();
                     continue;
                  }
                  edi_text = tmp;
                  texto = edi_text + ntlin;
                  if (ntlin < topelin)
                  {
                     ntlin++;
                     *(edi_text+ntlin) = NULL;
                  }
                  if (lin < topelin)
                     texto++;
                  lin++;
                  nlin = ntlin + 1;
                  if (nlin > topelin) nlin = topelin;
               }
               else
               {
                  if (lin < topelin)
                     texto++; /* insertar detras */
                  lin++;
               }
               if (qc == -3 && (!*texto || (strlen(*texto)+i_margen+strlen(buf+i)) < tcol))
               {		   
                  if (lin <= topelin)
                  {			 
                     if (*texto)
                     {
                        j = 0;
                        while(j < i_margen && *(*texto + j) == ' ') j++;
                        strcat(buf+i,*texto + j);			   
                        free(*texto);
                        *texto = texmalloc(strlen(buf+i)+i_margen+1);
                        if (!*texto)
                        {
                           memoria_error();
                           continue;
                        }
                        memset(*texto,' ',i_margen);
                        strcpy(*texto + i_margen,buf+i);
                     }
                     else
                     {
                        ntlin = inserta_linea(buf+i,topelin,i_margen,0);
                        pintar =1;
                     }
                     if ((qp / 100) < (qpf / 100) && strlen(*texto) > tini)
                     {
                        f_pprint((1 + qp / 100) * 100 + (qpi_rt % 100),*texto + tini,tpan,0);
                     }
                  }
               }
               else
               {
                  if (lin <= topelin)
                  {
                     if (*texto)
                        pintar = 1;
                     ntlin = inserta_linea(buf+i,topelin,i_margen,0);
                  }
                  else
                     pintar = 1;
               }
               if (lin <= topelin)
                  texto--; /* reponer a corriente */
               lin--;
               if ((rentra_offqp+rentra_offini) >= tcol)
                  qc = 0;
               buf[i] = 0;
               quitab(buf);
              }
              else  /* no se pueden dar las dos circunstancias */
                 if (qc == -4 && texto > edi_text) {
                    texto--;
                    lin--;
                    if (strlen(*texto) >= tcol) {
                       *((*texto) + tcol - 1) = 0;/* quitar uno si la linea previa */
                       /* esta a tope */
                       if (rentra_offqp < tpan && (qp / 100) > (qpi_rt / 100) && (tini+tpan) >= tcol) {
                          pprint(qp-100+tpan-1," ");
                       }
                    }
                    rentra_offqp = strlen(*texto); /*al final de la linea anterior*/
                    rentra_offini = 0;
                    if (strlen(buf)) {			    
                       strcatd(buf,*texto);
                       free(*texto);
                       i = strlen(buf);
                       if (i > tcol) { /* no cabe toda la linea */
                          i = tcol;   /* respetar la palabra */
                          while(i && buf[i] != ' ') i--;
                          if (i < rentra_offqp)
                             i = rentra_offqp; /* el efecto es como si no se hubiera hecho nada */
                       }
                       *texto = texmalloc(i+1);
                       if (!*texto) {
                          memoria_error();
                          continue;
                       }
                       memcpy(*texto,buf,i);
                       *((*texto) + i) = 0;
                       /* quita espacios delanteros */
                       while(buf[i] && buf[i] == ' ') i++;
                       strcpy(buf,buf+i);
                       
                       if (rentra_offqp < tpan) {
                          cursor(qp);
                          f_pprint(qp,buf+tini,tpan,0);
                          if ( (strlen(buf) - tini) < tpan )
                          {
                             /* falta atributo */
                             zonablan(qp+(strlen(buf)-tini),qp+tpan-1);
                          }
                          if ((qp / 100) > (qpi_rt / 100)) {
                             f_pprint(qp-100,(*texto)+tini,tpan,0);
                          }
                       }
                    }
                    qc = 2; /* arriba */
                    texto++;
                 }
                 
                 if ( *texto ) {
                    if (qc == -2 && *(texto+1)) {
                       /* enganchar detras al borrar ultimo catacter */
                       if ((rentra_offqp+rentra_offini) > (j = strlen(buf))) {
                          memset(buf+j,' ',rentra_offqp+rentra_offini - j);
                          buf[rentra_offqp+rentra_offini] = 0;
                       }
                       j = strlen(buf);
                       i = 0;
                       while(*(*(texto+1) + i) == ' ') i++;
                       strcat(buf,*(texto+1) + i);
                       i = strlen(buf);
                       if (i > tcol) {
                          i = tcol;
                          while(i && buf[i] != ' ') i--;
                          if (i < j)
                             i = j;
                          else
                             if (buf[i] == ' ')
                                i++;
                             
                             if ( (strlen(buf+i)+i_margen) > tcol)
                                j = 0;
                             else
                                j = i_margen;
                             memset(*(texto+1),' ',j);
                             strcpy(*(texto+1) + j,buf+i);
                             buf[i] = 0;
                             quitab(buf);
                       }
                       else {
                          ntlin = borra_linea(edi_text,texto+1,ntlin);
                          nlin = ntlin + 1;
                       }
                       pintar = 1;
                    }
                    if (strcmp(buf,*texto)) { /* solo si es necesario */
                       if (strlen(buf) > strlen(*texto)) {
                          free(*texto);
                          *texto = texmalloc( strlen(buf) + 1 );
                          if (!*texto) {
                             memoria_error();
                             continue;
                          }
                       }
                       strcpy(*texto,buf);
                    }
                 }
                 else {
                    if ( strlen(buf) || !qc || qc == -3) {
                       if (ntlin < topelin && (!rentra_insert || (qc && qc != -3)))
                       {
                          tmp = nueva_memo(edi_text,ntlin,1,tex_a_tope);
                          if (!tmp) {
                             memoria_error();
                             continue;
                          }
                          edi_text = tmp;
                          texto = edi_text + ntlin;
                          ntlin++;
                          lin = ntlin;
                          nlin = ntlin + 1;
                          if (nlin > topelin) nlin = topelin;
                          *(edi_text+ntlin) = NULL;
                       }
                       quitab(buf);
                       *texto = texmalloc( strlen(buf) + 1 );
                       if (!*texto) {
                          memoria_error();
                          continue;
                       }
                       strcpy(*texto,buf);
                    }
                 }
                 
                 if (!qc)
                 {
                    rentra_offini = 0;
                    rentra_offqp = 0;
                    if (!*(texto+1) || !strlen(*(texto+1)))
                    {
                       while(*( *texto + rentra_offqp ) == ' ') rentra_offqp++;
                       if (rentra_offqp < i_margen) rentra_offqp = i_margen;
                    }
                    else
                    {
                       while(rentra_offqp < i_margen && *( *(texto+1) + rentra_offqp ) == ' ') rentra_offqp++;
                       if (posi) rentra_offqp += posi;
                    }
                 }
                 
                 if (qc > 99 && qc < 1000) { /* WORDSTAR comandos!!!!! */
                    if (qc == 999)
                       qc = 9;
                    else
                       if (qc == 998)
                          qc = 901;
                       else
                          if (qc == 600)
                             qc = 18;
                          else {
                             Ar();
                             pprint(qpi_rt-184,"^");
                             v_printc("%c",74 + qc / 100);
                             Cr();
                             if ((i = leecar()) > 64 && i < 91) i += 32;
                             switch(qc) {
                             case 100: /* bloques */
                                if (i == 'b')
                                   qc = 15;
                                else
                                   if (i == 'k')
                                      qc = 16;
                                   else
                                      if (i == 'c')
                                         qc = 101;
                                      else
                                         if (i == 'y')
                                            qc = 102;
                                         else
                                            if (i == 'v')
                                               qc = 103;
                                            else
                                               if (i == 'w')
                                                  qc = 104;
                                               else
                                                  if (i == 'r')
                                                     qc = 105;
                                                  else
                                                     if (i == 'g')
                                                        qc = 188;
                                                     else
                                                        qc = 0;
                                                     break;
                             case 700: /* rapido */
                                if (i == 'f')
                                   qc = 13;
                                else
                                   if (i == 'a')
                                      qc = 14;
                                   else
                                      if (i == 'r')
                                         qc = 11;
                                      else
                                         if (i == 'c')
                                            qc = 12;
                                         else
                                            if (i == 'b')
                                               qc = 701;
                                            else
                                               if (i == 'k')
                                                  qc = 702;
                                               else
                                                  qc = 0;
                                               break;
                             case 500: /* pantalla */
                                if (i == 'l')
                                   qc = 401;
                                else
                                   if (i == 'r')
                                      qc = 402;
                                   else
                                      if (i == 'j') /* justificar */
                                         qc = 403;
                                      else
                                         qc = 0;
                                      break;
                             default:
                                qc = 0;
                             }
                             if (qc) {
                                v_printc("%c",qc);
                             }
                             else
                                pito();
                             pprint(qpi_rt-184,"   ");
                          }
                 }
                 
         }
         /*************************************/
         
         switch(qc) {
         case 0: /* return */
            if (*c_v == 2) {
               *c_v = 0;
               pinta_subti(*c_v);
               v = 0; /* cambiar a edicion */
               nlin = ntlin + 1 - v;
               if (nlin > topelin) nlin = topelin;
               pintar = 1;
               break;
            }
         case 3: /* abajo */
            if (!*texto)
               break;
            if (texto >= (edi_text + ntlin))
               break;
            if (rentra_offqp == tcol) { /* posicionar al principio */
               rentra_offqp = i_margen;
               rentra_offini = 0;
            }
            qp += 100;
            texto++;
            lin++;
            if (lin > topelin) {
               texto--;
               qp -= 100;
               lin--;
               rentra_offqp = i_margen;
               rentra_offini = 0;
               break;
            }
            if (qc)
               ajusta_off(*texto,i_margen);
            if (v) {
               for (i = 0;i < ((qpf/100 - qp/100) + 1);i++) {
                  if (*texto)
                     texto++;
                  else
                     break;
               }
               qp += (i * 100);
            }
            if (qp <= qpf) {
               break;
            }
            qp -= EDISALTO;
            pintar = 1;
            break;
         case 5: /* pagina abajo */
            i = qpf / 100 - qp / 100 + 1;
            if (lin < topelin &&  (texto + i) <= (edi_text + ntlin) ) {
               pintar = 1;
               if ( (texto - edi_text + tlin) <= ntlin) {
                  texto += tlin; /* qp igual */
                  lin += tlin;
               }
               else {
                  texto += i;
                  lin += i;
                  qp = qpi_rt + ( edi_text - texto + ntlin ) * 100;
                  texto = edi_text + ntlin;
               }
               ajusta_off(*texto,i_margen);
            }
            break;
         case 2: /* arriba */
            if (texto <= edi_text) break;
            texto--;
            qp -= 100;
            lin--;
            if (rentra_offqp == tcol) { /* posicionar al final de la linea anterior */
               rentra_offini = 0;
               rentra_offqp = strlen(*texto);
            }
            ajusta_off(*texto,i_margen);
            if (v) {
               for (i = 0;i < ((qp/100 - qpi_rt/100) + 1);i++) {
                  if (texto > edi_text)
                     texto--;
                  else
                     break;
               }
               qp -= (i * 100);
            }
            if (qp >= qpi_rt) {
               break;
            }
            if ((texto - (EDISALTO/100)) < edi_text) {
               i = texto - edi_text;
               qp = qpi_rt + i * 100;
            }
            else
               qp = qpi_rt + EDISALTO - 100;
            pintar = 1;
            break;
         case 4: /* pagina arriba */
            if (texto > edi_text) {
               i = (qp / 100) - (qpi_rt / 100) + tlin;
               if (texto >= (edi_text + i))
                  i = tlin;
               else
                  i = tlin - (int)(edi_text - texto + i);
               texto -= i;
               pintar = 1;
               lin -= i;
               ajusta_off(*texto,i_margen);
            }
            break;
         case 9: /* f1 = borrar linea */
            if (!*texto || v) break;
            strcpy(undo,*texto);
            ntlin = borra_linea(edi_text,texto,ntlin);
            pintar = 1;
            nlin = ntlin + 1;
            ajusta_off(*texto,i_margen);
            break;
         case 10: /* f2 = undelete */
            if (v || (*undo == 1 && !blo)) break;
            if (blo) {
               blo = recupera_bloque(topelin,blo,tcol,qpi_rt,0);
            }
            else {
               ntlin = inserta_linea(undo,topelin,i_margen,0);
            }
            pintar = 1;
            ajusta_off(*texto,i_margen);
            break;
         case 11: /* f3 = principio del texto */
            lin = 1;
            texto = edi_text;
            qp = qpi_rt;
            pintar = 1;
            rentra_offqp = i_margen;
            rentra_offini = 0;
            break;
         case 12: /* f4 = final del texto */
            lin = nlin + 1;
            texto = edi_text + ntlin;
            qp = qpi_rt + (ntlin % tlin) * 100;
            if (qp > qpf) {
               qp = qpi_rt + (tlin - 1) * 100;
               texto--;
            }
            ajusta_off(*texto,i_margen);
            pintar = 1;
            break;
         case 14: /* f6 = busqueda y sustituicion de un texto */
            if (v) break;
         case 13: /* f5 = busqueda de un texto */
         case 901: /* siguiente en edicion */
            i = ((qc == 901) ? 2 : qc-13);
            if (!i && !v) i = 3;
            busca_texto(busca,titulo,qpi_rt,qpf,tcol,&i);
            if (i < 2)
               pinta_subti(*c_v);
            tini = rentra_offini;
            break;
         case 15: /* f7 inicio bloque */
            if (v) break;
            l_inibloque = (int)(texto - edi_text);
            if (rentra_insert)
               c_inibloque = 0;
            else
               c_inibloque = rentra_offini + rentra_offqp;
            ajusta_bloque(0);
            pintar = 1;
            break;
         case 16: /* f8 fin bloque */
            if (v) break;
            l_finbloque = (int)(texto - edi_text);
            if (rentra_insert)
               c_finbloque = tcol - 1;
            else
               c_finbloque = rentra_offini + rentra_offqp;
            ajusta_bloque(1);
            pintar = 1;
            break;
         case 17: /* f9 Margenes */
         case 401:
         case 402:
            if (v) break;
            entra_margen(tcol,((qc > 400) ? qc - 400 : 0));
            pinta_hoja_texto(texto - (qp / 100) + (qpi_rt / 100),qpi_rt,qpf,titulo,(int)(texto-edi_text+1),nlin,v,tpan,tini,NULL,NULL,i_margen,d_margen);
            pinta_subti(*c_v);
            break;
         case 403: /* JUSTIFICAR TEXTO */
            if (v) break;
            justifica(topelin,qpi_rt);
            pintar = 1;
            break;
         case 18: /* f10 = imprimir */
            imprime_texto(edi_text);
            pinta_subti(*c_v);
            break;
         case 188: /* ir a una linea */
            lin = entra_a_que_linea(qpi_rt,(int)(texto-edi_text+1),ntlin);
            texto = edi_text + lin;
            if ((tlin/2) >= lin)
            {
               qp = qpi_rt + (tlin/2) * 100;
            }
            else
            {
               qp = qpi_rt + (lin*100);
            }
            lin++;
            ajusta_off(*texto,i_margen);
            pintar = 1;
            break;
         case 101:  /* copiar bloque */
            if (v) break;
            if (!hay_bloque())
               break;
            blo = borra_bloque(topelin,blo,1,qpi_rt); /* solo saca */
            blo = recupera_bloque(topelin,blo,tcol,qpi_rt,0);
            pintar = 1;
            ajusta_off(*texto,i_margen);
            break;
         case 102:  /* borrar bloque */
            if (v) break;
            if (!hay_bloque())
               break;
            blo = borra_bloque(topelin,blo,0,qpi_rt);
            l_finbloque = -1;
            l_inibloque = -1;
            c_finbloque = -1;
            c_inibloque = -1;
            pintar = 1;
            ajusta_off(*texto,i_margen);
            break;
         case 103:  /* mover bloque */
            if (v) break;
            if (!hay_bloque())
               break;
            blo = borra_bloque(topelin,blo,0,qpi_rt);
            blo = recupera_bloque(topelin,blo,tcol,qpi_rt,0);
            pintar = 1;
            ajusta_off(*texto,i_margen);
            break;
         case 104:  /* grabar bloque a fichero */
            if (v) break;
            if (!hay_bloque())
               break;
            Bl(24);
            pprint(2401,"Grabar al fichero:");
            l_entra(2419,50,"","","",0,buf);
            quitab(buf);
            if (strlen(buf))
            {
               if (cripto >= 0)
                  ant = (char *)fopen(buf,"w");
               else
               {
                  strcat(buf,".cal");
                  ant = NULL;
                  ant = (char *)rw_open_(buf,O_RDWR | O_BINARY | O_TRUNC | O_CREAT,S_IREAD | S_IWRITE);
               }
               if (ant)
               {
                  borra_bloque(topelin,ant,((cripto >= 0) ? 2 : -1),qpi_rt); /* grabar */
                  if (cripto >= 0)
                     fclose((FILE *)ant);
                  else
                     rw_close_((int)ant);
               }
            }
            pinta_subti(*c_v);
            break;
         case 105:  /* leer del fichero */
            if (v) break;
            Bl(24);
            pprint(2401,"Leer del fichero:");
            l_entra(2418,50,"","","",0,buf);
            quitab(buf);
            if (strlen(buf))
            {
               if (cripto >= 0)
                  ant = (char *)fopen(buf,"r");
               else
               {
                  strcat(buf,".cal");
                  ant = NULL;
                  ant = (char *)rw_open_(buf,O_RDWR | O_BINARY);
               }
               if (ant)
               {
                  recupera_bloque(topelin,ant,tcol,qpi_rt,cripto);
                  if (cripto >= 0)
                     fclose((FILE *)ant);
                  else
                     rw_close_((int)ant);
                  pintar = 1;
                  ajusta_off(*texto,i_margen);
               }
            }
            pinta_subti(*c_v);
            break;
         case 701:
            if (v) break;
            if (l_inibloque > -1 && c_inibloque > -1) {
               tini = 0;
               rentra_offqp = c_inibloque;
               i = l_inibloque - (int)(texto - edi_text);
               qp += (i * 100);
               if (qp < qpi_rt)
                  qp = qpi_rt;
               else
                  if (qp > qpf)
                     qp = qpi_rt + tlin * 100 - 100;
                  texto += i;
                  pintar = 1;
            }
            break;
         case 702:
            if (v) break;
            if (l_finbloque > -1 && c_finbloque > -1) {
               tini = 0;
               rentra_offqp = c_finbloque;
               i = l_finbloque - (int)(texto - edi_text);
               qp += (i * 100);
               if (qp < qpi_rt)
                  qp = qpi_rt;
               else
                  if (qp > qpf)
                     qp = qpi_rt + tlin * 100 - 100;
                  texto += i;
                  pintar = 1;
            }
            break;
         default:
            break;
         }
         if (qc == 1)
         {
            if (!repinta_texto)
               break;
         }
         if (qc == 7) {
            r = -1;
            break;
         }
      }
      
      
      if (salva_x)
         *(edi_text + topelin) = salva_x;
      
      rentra_offqp = 0;
      /* quitar lineas vacias al final */
      texto = edi_text + ntlin - 1;
      while (texto >= edi_text) {
         if (!*texto)
            break;
         if (!strlen(*texto)) {
            free(*texto);
            *texto = NULL;
            texto--;
         }
         else
            break;
      }
      *tx = edi_text;
      
      free(undo);
      w_pinta(l24);
      w_fin(l24);
      libera_bloque(blo);
      
memo_salida:
      
      repon_funciones(fns,0,8 + ((memo_pregu_imp) ? 1 : 0));
      if (ol)
         *ol = redita_linea;
      if (oc)
         *oc = redita_col;
      
      qpi_rt = __qpi;
      rentra_pos = atp;
      rentra_edicion = ata;
      rentra_offqp = atb;
      rentra_insert = atc;
      rentra_offini = atd;
      rentra_atrib = att;
      rentra_cc = acc;
      rentra_cf = acf;   
      rentra_sal_atrib = atts;
      rentra_sal_cc = accs;
      rentra_sal_cf = acfs;
      redita_linea = ool;
      redita_col = ooc;
      repinta_texto = rept;
      edi_text  = _text;
      texto = _texto;
      blo = _blo;
      v = _v;
      qp = _qp;
      tlin = _tlin;
      tpan = _tpan;
      nlin = _nlin;
      lin = _lin;
      ntlin = _ntlin;
      tini = _tini;
      l_inibloque = _l_inibloque;
      l_finbloque = _l_finbloque;
      c_inibloque = _c_inibloque;
      c_finbloque = _c_finbloque;
      i_margen = _i_margen;
      d_margen = _d_margen;
      tex_a_tope = atope;
      puntero_especial_rentra = p_e_r_e;
      _el_titulo = __el_titulo;
      return(r);
}


int libera_texto(text)
char **text;
{
   char **texto = text;
   
   if (!text) return(0);
   
   while(*texto) {
      free(*texto);
      texto++;
   }
   free((char *)text);
   
   return(0);
}

