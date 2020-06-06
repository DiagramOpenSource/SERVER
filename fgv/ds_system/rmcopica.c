/*---------------------------------------------------------------------------
* Programa ..: rcopicam.c
* Fecha .....: 19-4-88
* Version ...: 2.00                     Autor: Francisco Gonzalez Valiente
* Notas .....: rutina que copia contenido de un campo de un fichero a otro
* Contenido .: int  copia_cam(),
*---------------------------------------------------------------------------
*/



#include <fgvsys.h>

#include <memory.h>

#define _NUEVODEF
#include <rdef.h>
#include <rmante.h>

/* La mayoria de rutinas son de interface para mantener conpatibilidad */

/* ---------------------------------------------------------------------------
*  copia_cam
*    copia el contenido de un campo de un fichero a otro.
*    devuelve: -1 si hay error en los campos: estan fuera del fichero o no
*                 son de la misma longitud.
*               1 si eran los campos iguales.
*               0 si no hay error y no eran iguales --> lo copia.
* ---------------------------------------------------------------------------
*/
int _copias_cam(ipfd,id,ipfo,io,buf1,buf2)
int ipfd;
int id;
int ipfo;
int io;
char *buf1,*buf2;
{
   int cc = 2; /* copia */
   if (io < 0) {io = (-io) - 1;cc = 0;} /* solo compara */
   cc = _campotrat(id,io,1,ipfd,ipfo,buf1,buf2,cc);
   return(cc == 0 ? 1 : 0);
}

int _copia_cam(ipfd,id,ipfo,io)
int ipfd;
int id;
int ipfo;
int io;
{
   /* LOS BUFFERS SON ESTATICOS */
   return(_copias_cam(ipfd,id,ipfo,io,v_mpfi(ipfd)->f_buf,v_mpfi(ipfo)->f_buf));
}


int _campocmp(campo,orden,ipfi,buf1,buf2,modo)
int campo;  /* numero de campo con origen en 1 */
int orden;  /* 1 ascendente , -1 descendente */
int ipfi;
char *buf1,*buf2;
int modo; /* 0 = compara ,1 = compara y si es menor pone el campo al prime */
{         /* 2 = copia campo , 3 = compara y si es mayor '' */
			return(_campotrat(campo,campo,orden,ipfi,ipfi,buf1,buf2,modo));
}

/*
* compara un mismo campo de dos claves teniendo en cuenta el orden
* (ascendente o descendente)
* devuelve :   0 - iguales
*             >0 - clave 1 > clave 2
*             <0 - clave 1 < clave 2
*/
int _campotrat(campd,campo,orden,ipfid,ipfio,buf1,buf2,modo)
int campd;  /* numero de campo con origen en 1 */
int campo;
int orden;  /* 1 ascendente , -1 descendente */
int ipfid;
int ipfio;
char *buf1,*buf2;
int modo; /* 0 = compara ,1 = compara y si es menor pone el campo al prime */
{         /* 2 = copia campo , 3 = compara y si es mayor '' */
   int tipo,offset,longit,offseto,longito,compara;
   char q1[MAXDEFECTO+1],q2[MAXDEFECTO+1],*qc1,*qc2;
   char qo1[MAXDEFECTO+1],qo2[MAXDEFECTO+1];
   double d1,d2;
   
   if (orden == 0) orden = 1;
   
   if ( (tipo = v_mpfi(ipfid)->f_campos[campd].c_tipo) !=
      v_mpfi(ipfio)->f_campos[campo].c_tipo) return(0);
   
   offset = v_mpfi(ipfid)->f_campos[campd].c_offset;
   longit = v_mpfi(ipfid)->f_campos[campd].c_long;
   
   offseto = v_mpfi(ipfio)->f_campos[campo].c_offset;
   longito = v_mpfi(ipfio)->f_campos[campo].c_long;
   
   switch (tipo) {
   case CHARTIPO :
						ldchar(buf1+offset,longit,q1);
                  ldchar(buf2+offseto,longito,q2);
                  if (longito > 19 && v_mpfi(ipfio)->f_campos[campo].c_key != 1)
                  {
                     strcpy(qo2,q2);
                     strmayus(qo2);		 
                     qc2 = qo2;
                  }
                  else
                     qc2 = q2;
                  if (longit > 19 && v_mpfi(ipfid)->f_campos[campd].c_key != 1)
                  {
                     strcpy(qo1,q1);
                     strmayus(qo1);
                     qc1 = qo1;
                  }	 
                  else
                     qc1 = q1;
                  compara = orden * agistrcmp(qc1,qc2);
                  if (modo == 2 || (modo == 1 && compara < 0) ||
                     (modo == 3 && compara > 0))
                     stchar(q2,buf1+offset,longit);
                  break;
   case FECHATIPO :
						ldfecha(buf1+offset,q1);
                  ldfecha(buf2+offseto,q2);
                  compara = orden * fechacmp(q1,q2);
                  if (modo == 2 || (modo == 1 && compara < 0) ||
                     (modo == 3 && compara > 0))
                     stfecha(q2,buf1+offset);
                  break;
   case INTTIPO :
						d1 = (double)ldint(buf1+offset);
                  d2 = (double)ldint(buf2+offseto);
                  if (d1 > d2)       compara = 1;
                  else if (d1 < d2)  compara = -1;
                  else               compara = 0;
                  compara *= orden;
                  if (modo == 2 || (modo == 1 && compara < 0) ||
                     (modo == 3 && compara > 0))
                     stint((int)d2,buf1+offset);
                  break;
   case DOUBLETIPO :
						d1 = lddbl(buf1+offset);
                  d2 = lddbl(buf2+offseto);
                  if (d1 > d2)       compara = 1;
                  else if (d1 < d2)  compara = -1;
                  else               compara = 0;
                  compara *= orden;
                  if (modo == 2 || (modo == 1 && compara < 0) ||
                     (modo == 3 && compara > 0))
                     stdbl(d2,buf1+offset);
                  break;
   case FLOATTIPO :
   case LONGTIPO :
   default:
						compara = 0;
                  break;
   }
   return(compara);
}

/* lo mismo por con pfi */

int x_campocmp(campo,orden,pfi,buf1,buf2,modo)
int campo;  /* numero de campo con origen en 1 */
int orden;  /* 1 ascendente , -1 descendente */
FI_DEF *pfi;
char *buf1,*buf2;
int modo; /* 0 = compara ,1 = compara y si es menor pone el campo al prime */
{         /* 2 = copia campo , 3 = compara y si es mayor '' */
			return(x_campotrat(campo,campo,orden,pfi,pfi,buf1,buf2,modo));
}

/*
* compara un mismo campo de dos claves teniendo en cuenta el orden
* (ascendente o descendente)
* devuelve :   0 - iguales
*             >0 - clave 1 > clave 2
*             <0 - clave 1 < clave 2
*/
int x_campotrat(campd,campo,orden,pfid,pfio,buf1,buf2,modo)
int campd;  /* numero de campo con origen en 1 */
int campo;
int orden;  /* 1 ascendente , -1 descendente */
FI_DEF *pfid;
FI_DEF *pfio;
char *buf1,*buf2;
int modo; /* 0 = compara ,1 = compara y si es menor pone el campo al prime */
{         /* 2 = copia campo , 3 = compara y si es mayor '' */
   int tipo,offset,longit,offseto,longito,compara;
   char q1[MAXDEFECTO+1],q2[MAXDEFECTO+1],*qc1,*qc2;
   char qo1[MAXDEFECTO+1],qo2[MAXDEFECTO+1];
   double d1,d2;
   
   if (orden == 0) orden = 1;
   
   if ( (tipo = pfid->f_campos[campd].c_tipo) !=
      pfio->f_campos[campo].c_tipo) return(0);
   
   offset = pfid->f_campos[campd].c_offset;
   longit = pfid->f_campos[campd].c_long;
   
   offseto = pfio->f_campos[campo].c_offset;
   longito = pfio->f_campos[campo].c_long;
   
   switch (tipo) {
   case CHARTIPO :
						ldchar(buf1+offset,longit,q1);
                  ldchar(buf2+offseto,longito,q2);
                  if (longito > 19 && pfio->f_campos[campo].c_key != 1)
                  {
                     strcpy(qo2,q2);
                     strmayus(qo2);		 
                     qc2 = qo2;
                  }
                  else
                     qc2 = q2;
                  if (longit > 19 && pfid->f_campos[campd].c_key != 1)
                  {
                     strcpy(qo1,q1);
                     strmayus(qo1);
                     qc1 = qo1;
                  }	 
                  else
                     qc1 = q1;
                  compara = orden * agistrcmp(qc1,qc2);
                  if (modo == 2 || (modo == 1 && compara < 0) ||
                     (modo == 3 && compara > 0))
                     stchar(q2,buf1+offset,longit);
                  break;
   case FECHATIPO :
						ldfecha(buf1+offset,q1);
                  ldfecha(buf2+offseto,q2);
                  compara = orden * fechacmp(q1,q2);
                  if (modo == 2 || (modo == 1 && compara < 0) ||
                     (modo == 3 && compara > 0))
                     stfecha(q2,buf1+offset);
                  break;
   case INTTIPO :
						d1 = (double)ldint(buf1+offset);
                  d2 = (double)ldint(buf2+offseto);
                  if (d1 > d2)       compara = 1;
                  else if (d1 < d2)  compara = -1;
                  else               compara = 0;
                  compara *= orden;
                  if (modo == 2 || (modo == 1 && compara < 0) ||
                     (modo == 3 && compara > 0))
                     stint((int)d2,buf1+offset);
                  break;
   case DOUBLETIPO :
						d1 = lddbl(buf1+offset);
                  d2 = lddbl(buf2+offseto);
                  if (d1 > d2)       compara = 1;
                  else if (d1 < d2)  compara = -1;
                  else               compara = 0;
                  compara *= orden;
                  if (modo == 2 || (modo == 1 && compara < 0) ||
                     (modo == 3 && compara > 0))
                     stdbl(d2,buf1+offset);
                  break;
   case FLOATTIPO :
   case LONGTIPO :
   default:
						compara = 0;
                  break;
   }
   return(compara);
}

