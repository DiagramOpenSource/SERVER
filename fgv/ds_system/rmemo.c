/* #define MAIN_ /* para test locales */
/****************************************************************************/
/*                   RUTINAS DE CAMPOS MEMO                                 */
/****************************************************************************/

/* Autor: Francisco Gonzalez Valiente */

#define MEMO_VERSION 1
#define LASTBYTE 0X7FFFFFFAL            /* -3L */

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


#define LF 10
#define FT 4
#define SIGUE 16

#ifdef MAIN_
static int rw_error(n)
int n;
{
   n -= 9;
   printf(__Texto[327],n);
   pito(160);
   errores(n);
   leecar();
}
#endif

extern char **nueva_memo();


extern int redita_atrib;
extern int redita_cc;
extern int redita_cf;

static int mala_pos(fd,pos)
int fd;
long pos;
{
   char c = 0;
   
   if (pos < (sizeof(long) + 1))
      return(-1);
   if (pos > (sizeof(long) + 1)) {
      rw_lseek_(fd,pos - sizeof(long) - 1,SEEK_SET);
      rw_read_(fd,&c,1);
      if (c != FT && c != SIGUE)
         return(-1);
			}
   return(0);
}


static int graba_caracter(fd,car)
int fd;
unsigned char car;
{
			if (!car)
            car = LF;
         else
            if (car < 32) car = '$'; /* limpiar caracteres ilegales */
            return(rw_write_(fd,&car,1)); /* escribir texto */
}

static int bloquea_memo(fd)
int fd;
{
   rw_lseek_(fd,LASTBYTE,SEEK_SET);
   if (locking(fd,2,1L)) {
      pito(161);
      return(-1);
   }
   return(0);
}

static int desbloquea_memo(fd)
int fd;
{
   rw_lseek_(fd,LASTBYTE,SEEK_SET);
   if (locking(fd,0,1L)) {
						pito(162);
                  return(-1);
   }
   return(0);
}

long graba_memo(fd,text,pos)
int fd;
char **text;
long pos;
{
   long p_libre,u_pos,tmp,tmp2,tmp3;
   int alfinal = 0;
   int uso_libre = 0;
   int total_gra = 0;
   int libre_final = 0;
   char buf[10],car;
   char **texto = text;
   char *textos = *text;
   
   if (!*texto) {
      if (pos)
         borra_memo(fd,pos);
      return(0L);
   }
   
   if (bloquea_memo(fd)) return(0L);
   
   /* obtener primera posicion libre y tamano del fichero */
   rw_lseek_(fd,0L,SEEK_SET);
   if (rw_read_(fd,(char *)&p_libre,sizeof(long)) != sizeof(long)) {
      rw_lseek_(fd,0L,SEEK_SET); /* si es nuevo */
      p_libre = sizeof(long) + 1;
      rw_write_(fd,(char *)&p_libre,sizeof(long));
      car = MEMO_VERSION;
      rw_write_(fd,&car,1);
   }
   else
      rw_read_(fd,&car,1);
   
   if (!car || car > MEMO_VERSION) {
      rw_error(9);
      desbloquea_memo(fd);
      return(0L);
   }
   
   u_pos = rw_lseek_(fd,0L,SEEK_END); /* tamano del fichero */
   if (p_libre == u_pos)
      libre_final = 1;
   
   if (!pos) {
      pos = p_libre; /* anadir texto */
      uso_libre = 1;
   }
   else {
      if(mala_pos(fd,pos) || pos == p_libre) {
         rw_error(10);
         desbloquea_memo(fd);
         return(0L);
      }
   }
   
   rw_lseek_(fd,pos,SEEK_SET);
   if (pos == u_pos) {
      alfinal = 1; /* escribir al final */
      if (p_libre == u_pos)
         uso_libre = 1;
   }
   
   while(*texto) {
      textos = *texto;
      do {
         if (!alfinal) { /* comprobar si hay salto */
            if (rw_read_(fd,buf,1) != 1) {
            alfinal = 1; /* esto, aunque pueda producir ampliacion del fichero
                         reduce algo la fragmentacion,(al no mirar el p_libre
            en vez de ampliar) */
            if (p_libre == u_pos)
               uso_libre = 1;
            total_gra += graba_caracter(fd,*textos);
            }
            else {
               if (buf[0] == FT) {
                  rw_lseek_(fd,-1L,SEEK_CUR); /* convertirlo a SIGUE */
                  buf[0] = SIGUE;
                  rw_write_(fd,buf,1); /* grabo marca SIGUE */
                  if (uso_libre) {
                  /* si ya usa el libre ... quiere decir que la ha
                     agotado ... continua en el final del fichero */
                     p_libre = u_pos;
                  }
                  /* sigue en la zona libre (no optimiza zona contigua) */
                  rw_write_(fd,(char *)&p_libre,sizeof(long));
                  rw_lseek_(fd,p_libre,SEEK_SET);
                  if (p_libre == u_pos)
                     alfinal = 1;
                  uso_libre = 1;
               }
               else
                  if (buf[0] == SIGUE) {
                     if (rw_read_(fd,(char *)&tmp,sizeof(long)) != sizeof(long)) {
                        rw_error(11);
                     }
                     if (tmp == rw_lseek_(fd,0L,SEEK_CUR)) {
                        /* zona contigua ... limpiar salto */
                        rw_lseek_(fd,tmp - sizeof(long) - 1,SEEK_SET);
                        memset(buf,0,sizeof(long) + 1);
                        rw_write_(fd,buf,sizeof(long) + 1);
                        rw_lseek_(fd,tmp - sizeof(long) - 2,SEEK_SET);
                     }
                     else
                        rw_lseek_(fd,tmp,SEEK_SET);
                  }
                  else
                     rw_lseek_(fd,-1L,SEEK_CUR);
                  
                  total_gra += graba_caracter(fd,*textos);
            }
									}
         else
            total_gra += graba_caracter(fd,*textos); /* esto es al final del fichero */
         
         if (*textos)
            textos++;
         else
            break;
      } while(-1);
      texto++;
   }
   
   /* grabar marca de final de texto */
   /* en tmp nueva posible zona libre */
   
   if (alfinal) {
      /* memset(buf+1,0,sizeof(long)); */
      buf[0] = FT; /* fin de texto */
      /* rw_write_(fd,buf,sizeof(long) + 1); */
      rw_write_(fd,buf,1);
      rw_write_(fd,(char *)&pos,sizeof(long)); /* puntero de entrada */
      u_pos = rw_lseek_(fd,0L,SEEK_END); /* tamano del fichero */
                                         tmp = u_pos; /* si se ha usado zona libre reajustar puntero
                                         al final del fichero */
   }
   else {
      if (rw_read_(fd,buf,1) != 1) {
         rw_error(12);
      }
      
      if (buf[0] != FT && buf[0] != SIGUE) {
         tmp2 = rw_lseek_(fd,0L,SEEK_CUR);
         while(-1) {
            if (rw_read_(fd,buf,1) != 1) {
               rw_error(13);
               buf[0] = FT;
               break;
            }
            if (buf[0] == SIGUE || buf[0] == FT)
               break;
            rw_lseek_(fd,-1L,SEEK_CUR);
            buf[0] = 0;
            rw_write_(fd,buf,1);
									}
         tmp3 = rw_lseek_(fd,0L,SEEK_CUR);
         if ((tmp3 - tmp2) > (sizeof(long) + 2)) { /* mas 2 por no hacerlo por 1 */
            rw_lseek_(fd,tmp2 - 1,SEEK_SET);
            /* memset(buf+1,0,sizeof(long)); */
            buf[0] = FT; /* fin de texto */
            /* rw_write_(fd,buf,sizeof(long) + 1); */
            rw_write_(fd,buf,1);
            rw_write_(fd,(char *)&pos,sizeof(long)); /* puntero de entrada */
            tmp = tmp2 + sizeof(long);
            if (!uso_libre)
               uso_libre = 2; /* enlazar = 1; /* si se usa la libre solo recortarla */
            buf[0] = 0;
									}
      }
      
      if (buf[0] == FT) {/* ya esta bien , si ha usado al zona libre */
         tmp = u_pos;    /* la ha gastado toda ---> posicionar al final */
         rw_write_(fd,(char *)&pos,sizeof(long)); /* puntero de entrada */
      }
      else
         if (buf[0] == SIGUE) {
            if (rw_read_(fd,(char *)&tmp,sizeof(long)) != sizeof(long)) {
               rw_error(14);
            }
            rw_lseek_(fd,(long)(-sizeof(long)-1),SEEK_CUR);
            /* memset(buf+1,0,sizeof(long)); */
            buf[0] = FT; /* fin de texto */
            /* rw_write_(fd,buf,sizeof(long) + 1); */
            rw_write_(fd,buf,1);
            rw_write_(fd,(char *)&pos,sizeof(long)); /* puntero de entrada */
            if (!uso_libre)
               uso_libre = 2; /* enlazar = 1; /* si se usa la libre solo recortarla */
									}
   }
   
   
   if (uso_libre) {
      /* ATTENCION REENLAZAR tmp CON ANTIGUA ZONA LIBRE si tmp < u_pos y uso_libre == 2*/
      if (tmp < u_pos && uso_libre == 2) {
         rw_lseek_(fd,tmp,SEEK_SET);
         while(-1) {
            if (rw_read_(fd,buf,1) != 1) {
               rw_error(15);
               break;
            }
            if (buf[0] == SIGUE) {
               rw_read_(fd,(char *)&tmp3,sizeof(long));
               rw_lseek_(fd,tmp3,SEEK_SET);
               continue;
            }
            rw_lseek_(fd,-1L,SEEK_CUR);
            if (buf[0] == FT) {
               if (libre_final) {
                  memset(buf+1,0,sizeof(long));
                  rw_write_(fd,buf,sizeof(long) + 1);  /* fin de texto libre */
               }
               else {
                  buf[0] = SIGUE;
                  rw_write_(fd,buf,1);
                  rw_write_(fd,(char *)&p_libre,sizeof(long));
               }
               break;
            }
            buf[0] = 0;
            rw_write_(fd,buf,1);
									}
      }
      rw_lseek_(fd,0L,SEEK_SET); /* nuevo final de fichero */
      rw_write_(fd,(char *)&tmp,sizeof(long));
   }
   
   desbloquea_memo(fd);
   return(pos);
}

char **lee_memo(fd,pos)
int fd;
long pos;
{
   char **texto;
   char **txt;
   char maxl[260];
   int nc = 0;
   int total_lin = 0,fs = 0;
   long tmp;
   
   if (pos) {
      if(mala_pos(fd,pos)) {
         rw_error(10);
         return(NULL);
      }
   }
   
   if (!(texto = (char **)texmalloc( sizeof(char *) ))) {
      memoria_error();
      return(texto);
   }
   *texto = NULL;
   if (!pos) return(texto);
   
   rw_lseek_(fd,pos,SEEK_SET);
   while(-1) {
      if (rw_read_(fd,maxl+nc,1) != 1) {
         rw_error(16);
         break;
      }
      if (maxl[nc] == FT) {
         rw_read_(fd,(char *)&tmp,sizeof(long));
         if (pos != tmp) {
            rw_error(17);
         }
         if (!nc)
            break;
         fs = 1; /* flag de salida */
         maxl[nc] = LF; /* recoger linea */
      }
      if (maxl[nc] == SIGUE) {
         rw_read_(fd,(char *)&tmp,sizeof(long));
         rw_lseek_(fd,tmp,SEEK_SET);
         continue;
      }
      if (maxl[nc] == LF || nc >= 250) 
	  {
         maxl[nc] = 0;
         
         txt = nueva_memo(texto,total_lin,1,0);
         if (txt) {
            texto = txt;
            if ((*(texto + total_lin) = texmalloc(strlen(maxl) + 1)) != NULL)
               strcpy(*(texto + total_lin),maxl);
            else
               memoria_error();
            total_lin++;
         }
         if (fs)
            break;
         nc = 0;
      }
      else
         nc++;
      if (nc == 258) {
         rw_error(18);
         nc = 0;
      }
   }
   
   /* quitar lineas vacias del final (de relleno) */
   
   while (total_lin) {
      if (!*(texto+total_lin-1))
         break;
      if (!strlen(*(texto+total_lin-1))) {
         free(*(texto+total_lin-1));
         total_lin--;
      }
      else
         break;
   }
   
   *(texto + total_lin) = NULL;
   return(texto);
}


int borra_memo(fd,pos)
int fd;
long pos;
{
   char buf[10],car;
   long p_libre,u_pos,tmp,tmp2,tmp3;
   
   if (!pos) return(0);
   
   if (bloquea_memo(fd)) return(-1);
   
   rw_lseek_(fd,0L,SEEK_SET);
   if (rw_read_(fd,(char *)&p_libre,sizeof(long)) != sizeof(long)) {
      rw_lseek_(fd,0L,SEEK_SET); /* si es nuevo */
      p_libre = sizeof(long) + 1;
      rw_write_(fd,(char *)&p_libre,sizeof(long));
      car = MEMO_VERSION;
      rw_write_(fd,&car,1);
      desbloquea_memo(fd);
      return(-1);
			}
   else
      rw_read_(fd,&car,1);
   
   u_pos = rw_lseek_(fd,0L,SEEK_END); /* tamano del fichero */
   
   if (mala_pos(fd,pos)) {
      rw_error(10);
      desbloquea_memo(fd);
      return(-1);
			}
   
   rw_lseek_(fd,pos,SEEK_SET);
   
   while(-1) {
      if (rw_read_(fd,buf,1) != 1) {
         rw_error(19);
         break;
      }
      if (buf[0] == SIGUE) {
         rw_read_(fd,(char *)&tmp3,sizeof(long));
         rw_lseek_(fd,tmp3,SEEK_SET);
         continue;
      }
      
      rw_lseek_(fd,-1L,SEEK_CUR);
      if (buf[0] == FT) {
         if (p_libre < u_pos) {
            buf[0] = SIGUE;
            rw_write_(fd,buf,1);
            rw_write_(fd,(char *)&p_libre,sizeof(long));
         }
         else {
            memset(buf+1,0,sizeof(long)); /* fin de texto libre */
            rw_write_(fd,buf,sizeof(long) + 1);
         }
         break;
      }
      buf[0] = 0;
      rw_write_(fd,buf,1);
			}
   
   rw_lseek_(fd,0L,SEEK_SET); /* nuevo final de fichero */
   rw_write_(fd,(char *)&pos,sizeof(long));
   
   desbloquea_memo(fd);
   return(0);
}


long trata_memo(fic,pos,m,qpi,qpf,v,topelin,titulo,tcol,tx,defcolor)
char *fic;
long pos;
int m; /* m == 0 entrar
       m == 1 pintar
       m == 2 pintar y entrar
       m == 3 presentar primera hoja
       m == 4 leer
       m == 5 grabar
       m == 6 borrar
       
        m / 100 == 1 No liberar a la salida
*/
int qpi,qpf,v; /* v == 0 no window */
int topelin;
char *titulo;
int tcol;
char ***tx;
short defcolor;
{
			char **text;
         int fd,i = 0,j = 0,qpiv = qpi,qpfv = qpf,graba = 1,nolibera = m / 100;
         VENT *w;
         char fich[51];
         int a_tope = 1;
         int r1 = redita_atrib;
         int r2 = redita_cc;
         int r3 = redita_cf;
         int at = *(((char *)&(defcolor)) + 1);
         int cc = (*(((char *)&(defcolor))) & 0x0f);
         int ff = ((*(((char *)&(defcolor))) & 0x70) >> 4);
         
         if (topelin < 0) {a_tope = -1;topelin = -topelin;}
         if (topelin > 8000) topelin = 8000;
         
         m %= 100;
         
         if ( (m < 4 && (qpi >= qpf || (qpi % 100) >= (qpf % 100))) || (m == 4 && v))
            return(0L);
         
         if (tx) text = *tx;
         strcpy(fich,fic);strcat(fich,".mem");
         
         /* abrir fichero */
         if ( (fd = rw_open_(fich,O_BINARY | O_RDWR)) < 0 ) {
            if ( (fd = rw_open_(fich,O_CREAT | O_BINARY | O_RDWR,S_IWRITE | S_IREAD)) < 0 )
               return(0L);
            rw_close_(fd);
            agichmod(fich,"","","");
            if ( (fd = rw_open_(fich,O_BINARY | O_RDWR)) < 0 )
               return(0L);
         }
         
         
         if (defcolor && at != 'D') {
            redita_atrib = at;
            redita_cc = cc;
            redita_cf = ff;
         }
         
         if (m < 3 && titulo && !v) {
            if ((qpi/100) > 2) {
               v = 2;
               qpiv = qpi - 200;
               qpfv = (qpi/100-1)*100 + qpf%100;
            }
            else
               qpi += 200;
         }
         
         if (v && m < 3) {
            w = w_ini(qpiv,qpfv);
            w_salva(w);
            if (v != 2) {
               Ai();
               cuadro(qpi,qpf);
               Bi();
               qpi += 101;
               qpf -= 101;
               if (titulo)
                  qpi += 200;
            }
         }
         
         if (m < 5)
            text = lee_memo(fd,pos);
         
         if (m == 3)
            pinta_hoja_texto(text,qpi,qpf,NULL,0,0,1,(qpf % 100) - (qpi % 100) + 1,0,NULL,NULL,-1,256);
         else
            if (m < 3) {
               if ( edita_texto(&text,qpi,qpf,topelin*a_tope,titulo,&m,tcol,NULL,NULL) )
                  graba = 0;
            }
            else
               if (m == 6) {
                  borra_memo(fd,pos);
                  pos = 0;
               }
               
               if (graba && (!m || m == 5))
                  pos = graba_memo(fd,text,pos);
               
               if (m != 6 && !nolibera)
                  libera_texto(text);
               
               if (m < 3 && titulo && !v) {
                  pinta_hoja_texto(text,qpi,qpf,NULL,0,0,1,(qpf % 100) - (qpi % 100) + 1,0,NULL,NULL,-1,256);
               }
               
               if (v && m < 3) {
                  w_pinta(w);
                  w_fin(w);
               }
               
               /* cerrar fichero */
               rw_close_(fd);
               if (tx) *tx = text;
               
               if (defcolor && at != 'D') {
                  redita_atrib = r1;
                  redita_cc = r2;
                  redita_cf = r3;
               }
               return(pos);
}

#ifdef MAIN_

int errores(n)
int n;
{
   static char *error[] = {
	   "\n\rVERSION DE FICHERO MEMO NO SOPORTADA!\n\r",
      "\n\rERROR MEMO posicion ilegal\n\r",
      "\n\rERROR INTERNO MEMO x1 FICHERO CORRUPTO\n\r",
      "\n\rERROR INTERNO MEMO x2 FICHERO CORRUPTO\n\r",
      "\n\rERROR INTERNO MEMO x4 FICHERO CORRUPTO\n\r",
      "\n\rERROR INTERNO MEMO x3 FICHERO CORRUPTO\n\r",
      "\n\rERROR INTERNO MEMO x5 FICHERO CORRUPTO\n\r",
      "\n\rERROR INTERNO MEMO x6 FICHERO CORRUPTO\n\r",
      "\n\rAVISO: MEMO LEIDA CON PUNTERO DIFERENTE\n\r",
      "\n\rERROR MEMO linea ilegal\n\r",
      "\n\rERROR INTERNO MEMO x7 FICHERO CORRUPTO\n\r"
   };
   
   printf("%s\n\r",error[n]);
}


cierra_todo() /* no isam */
{
}

extern int (*f_ayuda)();
extern char clave_help[32];
extern int agiayuda();

main()
{
   long pos = 0L,pos1 = 0L,pos2 = 0L;
   int qpi = 501;
   int qpf = 2380;
   int tope = 8000;
   
   /* RWdebug(); */
   /* rwdebug = 2; */
   
   Terinici();
   ini_sig();
   cls();
   
   f_ayuda = agiayuda; /* asignar puntero a funcion con la direccion adecuada */
   
   pos = trata_memo(rw_directorio("dev/agdnotas"),pos,0,qpi,qpf,1,tope,"ENTRAR 1",256,NULL);
   pos1 = trata_memo(rw_directorio("dev/agdnotas"),pos1,0,qpi,qpf,1,tope,"ENTRAR 2",256,NULL);
   
   pos = trata_memo(rw_directorio("dev/agdnotas"),pos,0,qpi,qpf,1,tope,"REENTRAR 1",256,NULL);
   pos1 = trata_memo(rw_directorio("dev/agdnotas"),pos1,0,qpi,qpf,1,tope,"REENTRAR 2",256,NULL);
   
   pos = trata_memo(rw_directorio("dev/agdnotas"),pos,2,qpi,qpf,1,tope,"PINTAR 1",256,NULL);
   pos1 = trata_memo(rw_directorio("dev/agdnotas"),pos1,1,qpi,qpf,1,tope,"PINTAR 2",256,NULL);
   
   fin_sig();
   Terfinal();
   
   /* RWfindebug(); */
   
   return(0);
}

#endif

