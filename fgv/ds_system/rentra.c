
/* #define _MAIN /* test locales */

/*---------------------------------------------------------------------------
 * Programa ..: rentra.c                   Autor: Francisco Gonzalez Valiente
 * Fecha .....:
 * Version ...:
 * Notas .....: Rutina de entrada de datos
 * Contenido .: int  entra(),
 *---------------------------------------------------------------------------*/


#include <fgvsys.h>

#include <stdio.h>
#include <string.h>
#include <memory.h>

#include <teclas.h>
#include <rfecha.h>
#include <rtermin.h>

#define min(a,b)  (((a) < (b)) ? (a) : (b))
#define max(a,b)  (((a) > (b)) ? (a) : (b))

#ifdef RWXARXA
#ifdef RWSERVER
extern int en_conexion_externa;
extern int en_conexion_externa_stop;
#endif
extern int en_conexion_terminal;
#endif


extern char *poncoma();
extern int data_hiden_indicator;

extern char clave_help[32];
extern char *dato_entra;
extern int dato_entra_len;

extern int PCcolor;
extern int PFcolor;

extern int TeclaScanner;
extern void (*rentra_pos)();
extern int rentra_autosal;
extern int rentra_edicion; /* funciones especiales de edicion de textos */
extern int rentra_atrib;
extern int rentra_sal_atrib;
extern int rentra_cc;
extern int rentra_cf;
extern int rentra_sal_cc;
extern int rentra_sal_cf;
extern int rentra_offqp;  /* 0 = modo normal de entrada de un campo */
/* -1 = entrada normal pero salida dejando colro por defecto de entrada */
/* -2 o > 0 se supone pintado bien y se sale dejando
color por defecto de entrada */
/* ( !0 es para modos de edicion de texto ) */
extern int rentra_offini;  /* para cuando la edicion es de lineas largas */
extern int rentra_insert;

extern int especial_tecla_linea;


extern int data_fecha_conmutar;

int id_g_control = -1;
int id_g_campo   = -1;

#define W_BLOQUE    11
#define W_RAPIDO    17
#define W_PANTALLA  15
#define W_IMPRESION 16
#define W_BORRALIN  25
#define W_SIGUIENTE 14


extern int Pseudo10;
/* -----------------------------------------------------------------*/
/*
*  llenabla   pone un string s de longitud i a blancos
*/

static char *llenabla(i,s)
int i;
char *s;
{
   memset(s,' ',i);s[i] = 0;
   return (s);
}


/* numblan devuelve el numero de blancos que hay encabezando un string
* por ejemplo  numblan("  abc") devuelve 2
*/
static int numblan(s)
char *s;
{
   int i=0;
   
   while (*s++ == ' ')
      i++;
   return(i);
}
/* numero  devuelve 0 si el caracter es signo,punto,blanco o numero
*                  -1 en caso contrario
*/
static int numero(c)
char c;
{
			if (c >= '0' && c <= '9') return(c);
         else if (c == ' ') return(c);
         else if (c == '+') return(c);
         else if (c == '-') return(c);
         else if (c == ',') return(c);
         else if (c == '.') return(',');
         else               return(-1);
}

/*
* bienstring  devuelve -1 si el string no es correcto, 0 si lo es
* para verificar si es correcto, comprueba el string con sus posibles
* valores que se encuentran en qi (que esta dividido en varios valores
* permitidos de longitud la de qo). Por ejemplo si qi es "SINO" y el
* qo es longitud 2 verificara que sea SI o NO para que sea correcto;
* en caso de que qi sea nulo devolvera correcto sin verificar.
*/
static int bienstring(qo,qi)
char *qo,*qi;
{
   int lo=strlen(qo);
   int li=strlen(qi);
   int i;
   
   if (li == 0)
      return(0);        /* no hay restriccion a qo */
   for (i=0;i<li;i += lo) {
      if (memcmp(qo,qi+i,lo) == 0)
         return(0);
   }
   return(-1);
}

/* biennumero  devuelve -1 si el numero no es correcto
*                       0 si es correcto
* si es correcto pone en qo el numero formateado
*/
static int biennumero(qo,qi,qs,decimal)
char *qo,*qi,*qs;          /* numero,minimo,maximo (como strings) */
int decimal;               /* numero de decimales */
{
   double tmin,tmax,tnum,vmin,vmax;
   int l = -1;
   int qc;
   int yacoma = 0;
   
   qc = 0; /* strlen mas posicion de la coma */
   while(qo[qc]) {
						if (qo[qc] == ',') {
                     l = qc;
                     qo[qc] = '.';
                  }
                  if (qo[qc] == '.') {
                     if (!yacoma)
                        yacoma = 1;
                     else {
                        if (l > -1) qo[l] = ',';
                        return(-1);
                     }
                  }
                  qc++;
   }
   
   tmin = rwatof(qi);
   tmax = rwatof(qs);
   
   tnum = rwatof(qo);
   
   vmin = min(tmin,tmax);    /* valor minimo */
   vmax = max(tmin,tmax);    /* valor maximo */
   if ((tnum<vmin) || (tnum>vmax)) {/* no esta entre los limites */
						if (l > -1) qo[l] = ',';
                  return(-1);
   }
   return(0);
}


/* chequea  comprueba si la entrada de numero o de fecha es coherente
*          si lo es devuelve un numero >= 0
*          en caso contrario devuelve -1
*/
static int chequea(tipo,qo,qi,qs,decimal,maxqc)
int tipo,decimal;
char *qo,*qi,*qs;
int maxqc;
{
   if (tipo == 1)          /* string */
      return(bienstring(qo,qi));
   else if (tipo == 2)     /* fecha */
      return(bienfecha_x(qo,maxqc));
   else if (tipo == 3)     /* numero */
      return(biennumero(qo,qi,qs,decimal));
   else
      return(0);
}


/*
* cambia    intercambia los contenidos de dos punteros a strings
*/
static void cambia(r,s)
char *r,*s;
{
   char u;
   while (*r) {
      u=*r;
      *r=*s;
      *s=u;
      r++;
      s++;
   }
}

static pstr(s,conta,qc)
char *s;
int conta,qc;
{
   int c;
   c = s[qc];
   s[qc] = 0;
   v_print(s+conta);
   s[qc] = c;
}

/*
*  prom    abre reverse y pinta el string s
*/
static void prom(s,qc)
char *s;
{
   char c = s[qc];
   s[qc] = 0;
   v_print(s);
   s[qc] = c;
}


/*
*  llenastr   pone en un string s de longitud i el string r y el resto blancos
*/
static char *llenastr(i,r,s)
int i;
char *r,*s;
{
   int j=0;
   while ((*s++ = *r++) != '\0' && j<i)  /* poner en s el string r */
      j++;
   s--;
   while (j++ < i)
      *s++ = '\040';    /* rellenar con blancos */
   *s++='\0';
   return(s-j);
}

/*
* del1  desplaza el string 1 caracter a la izquierda y pone un blanco
*       al final
*/
static int del1(s)
char *s;
{
   int j=0,i = 1;
   while (*(s+j+1)) {
      if (*(s+j) != ' ')
         i = 0;
      *(s+j) = *(s+j+1);
      j++;
   }
   if (*(s+j) != ' ')
      i = 0;
   *(s+j) = '\040';
   *(s+j+1) = '\0';
   return(i);
}

/*
*  corre1  desplaza el string 1 caracter a la derecha y pone un blanco
*          al principio
*/
static int corre1(s)
char *s;
{
   int j=strlen(s),i=0;
   
   if (rentra_edicion && *(s+j-1) != ' ') {j += 2;i=1;}
   
   while (--j)
      *(s+j) = *(s+j-1);
   *s='\040';
   return (i);
}



/*---------------------------------------------------------------------------
* Contenido .: int entra(qp,qc,qi,qs,qd,qh,qo)
*   Entradas:
*    qp  posicion (fila*100+columna,  1<=fila<=24 ,  1<=columna<=80 )
*    qc  longitud del string
*        si es -1 es fecha dd.mm.aaaa (10 caracteres)
*        si es  0 es numero
*    qi  valor minimo solo para numeros en caso contrario es ""
*    qs  valor maximo               "
*    qd  valor por defecto
*        en fechas, si es nulo("") se toma la fecha del sistema
*    qh%10  flag ( 0-desactiva ESC, flechas, paginas, help)
*             ( 1-las activa)
*             ( 2-igual que 1 mas controles word-star
*             (-1 para password)
*        el Ctlc-C (abort) esta activado siempre
*    qh / 10 Longitud Maxima del campo hasta 256 caracteres
*   Salidas:
*    qo  string de salida
*   Return:
*    (0-validar 1-esc 2-arriba 3-abajo 4-pgup 5-pgdn 6-help 7-abort)
*     -1 error en la llamada (p.e. qp erronea)
* ejemplo:
*  flag = entra(1020,30,"","","valor defecto",1,qo);
*    entrada de un string de 30 caracteres en la posicion 10,20
*    donde estan activadas las salidas opcionales (esc,pgup,help,...)
*    el parametro que identifica esta salida esta en flag y se comprueba
*    con if (flag==1) hemos salido con esc, etc...
*    qo es la direccion donde esta la salida
*  entra(1020,-1,"","","22.07.1987",1,qo);  para fechas
*  entra(1020,0,"-9999.99","99999.99","1000",1,qo); para numeros
*---------------------------------------------------------------------------*/
int i_entra(qp,maxqc,qi,qs,qd,qh,qo,hflag,htext)
int qp,
maxqc, /* longitud real del dato */
qh;
char *qi,*qs,*qd,*qo;
int hflag;
char *htext;
{
   int qc = qh / 10; /* longitud en pantalla */
   int qj;
   int tipo,i,j;
   int mientras=-1;
   int c,conta=rentra_offqp;
   int contai = 0,si_pintar;
   char antic[257];
   char usi[15];
   char *antiguo,*o_calculadora = dato_entra;
   int _dato_entra_len = dato_entra_len;
   int insert = FALSO,decmax,decmin,decimal,lmin;
   int tocado = 0;
   int indicator__ = 0;
   int flag_extrafuncion = qp / 10000;
   int flag_delineas = 0;
   int euro_campo = 0;   
   
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      return(entra_en_terminal(qp,maxqc,qi,qs,qd,qh,qo));
   }
#endif
#endif

   
   if (flag_extrafuncion & 1)
   {
      flag_delineas = 1;
      flag_extrafuncion &= ~1;
   }
   qp %= 10000;
   qh %= 10;
   
   antiguo=antic;
   qj=0;
   
   if (rentra_offqp)
   {
      si_pintar = 0;
	  if (!maxqc)
	  {
		int l = 0;
		char *min = qi;
		for (i = 0;min[i];i++)
		{
		   if (min[i] == '.') l++;
		   if (l && (min[i] == '?' || min[i] == '!')) {break;}
		}
		if (min[i]) 
		{
			i++;
			while(min[i] == '?') i++;
			if (!es_euro(min+i))
			{
				euro_campo = 1;
			}
		}
	  }
   }
   else
      si_pintar = 1;
   if (rentra_offqp == -3) {
      rentra_offqp = 0;
      conta = 0;
   }
   
   if (maxqc > 256)
      return(-1);
   
   if (rentra_offqp || rentra_edicion) {
      tocado = 1;
      if (rentra_insert)
         insert = CIERTO;
      contai = rentra_offini;
   }
   else
      rentra_offini = 0;
   
   if (maxqc > 0)
   {
      tipo=1;                   /* string */
   }
   else 
      if (maxqc==-1) 
      {   
         tipo=2;                   /* fecha dd.mm.aaaa */   
         if (qc == 6) 
         {
            maxqc = 6;
         }
         else
            if (qc == 8) 
            {
               maxqc = 8;
            }
            else
            {
               maxqc = 10;
            }
            if (maxqc != 6 && maxqc != 8 && strlen(qd) != maxqc)
            { 
               minimo_fecha(&qd,maxqc); /* fecha del sistema segun formato */
            }
      }
      else if (maxqc == 0) {
         tipo=3;                   /* numero */
         i = strlen(qi);
         if (i > 1 && qi[i-2] == '.' && qi[i-1] == '*') {
            decimal = -1;
         }
         else {
            decmin = decimales(qi);   /* numero de decimales de qi */
            decmax = decimales(qs);
            decimal = max(decmin,decmax); /* numero de decimales posibles */
         }
         maxqc = len_numerico(qi,qs);
         c = numblan(qd);
         qd += c;
      }
      else {
         tipo=0;                 /* erroneo */
         *qo = '\0';
         return(-1);
      }
      
      
      if (tipo != 1 || !qc || qc > maxqc) qc = maxqc;

      if (tipo == 1)
      {
		   i = strbusca(qs,"#");
		   if (i >= 0)
		   {
			   maxqc = qc;
			   i++;
			   j = 0;
			   while(j < 14 && qs[i] && qs[i] >= '0' && qs[i] <= '9')
			   {
				   usi[j] = qs[i];
				   j++;
				   i++;
			   }
			   if (j)
			   {
				  usi[j] = 0;
				  j = atoi(usi);
				  if (j > 0)
				  {
					  maxqc = j;
				  }
			   }
		   }          
      }

      dato_entra_len = maxqc;
      dato_entra = qo; /* campo rellenable desde un proceso inferior */

      if (qp%100+qc > 81 && tipo == 1)
         qc = 81 - qp%100;
      
      if (qp/100 < 1 || qp/100 > 24 || qp%100 < 1 || qp%100+qc > 81)
         qp = 101;
      
      clave_help[31] = qp / 100;
      
      if (rentra_edicion) { /* no usar el qd */
         llenastr(maxqc,qo,qo);
         llenastr(maxqc,qd,antiguo);
      }
      else {
         llenastr(maxqc,qd,qo);  /*  qo=qd+blancos  */
         llenastr(maxqc,qo,antiguo);
      }

#ifdef FGVWIN
      if (TeclaScanner)
	  {
		  if (DameScanner(qo))
		  {
			 return 0;
		  }
		  ScannerGet();
	  }
#endif

      if (Pseudo10 && !rentra_edicion && DespachadorExterno)
      {
          char tmp[512];
          int posi;
          int flag = flag_delineas+especial_tecla_linea;
          int total;


          if ( (flag_extrafuncion & 2) ) /* no usar qp */
          {
              flag |= 8; /* es control */
          }

          if (rentra_autosal)
          {
              flag |= 4;
          }

		  /* flag de ayuda de teclas */
		  flag |= ((hflag << 16) & 0xffff0000);
		  if (!htext)
			  htext = "";

          do 
          {
          posi = 0;

          total = sizeof(int)*8 + strlen(qi) + strlen(qs) + strlen(qo) + strlen(htext) + 4;

          memcpy_int(tmp+posi,&total,sizeof(int));
          posi += sizeof(int);

          memcpy_int(tmp+posi,&maxqc,sizeof(int));
          posi += sizeof(int);
          memcpy_int(tmp+posi,&qh,sizeof(int));         
          posi += sizeof(int);
          memcpy_int(tmp+posi,&tipo,sizeof(int));         
          posi += sizeof(int);
          memcpy_int(tmp+posi,&decimal,sizeof(int));         
          posi += sizeof(int);
          memcpy_int(tmp+posi,&flag,sizeof(int));
          posi += sizeof(int);

          memcpy_int(tmp+posi,&id_g_control,sizeof(int));
          posi += sizeof(int);
          memcpy_int(tmp+posi,&id_g_campo,sizeof(int));
          posi += sizeof(int);          

          strcpy(tmp+posi,qi);
          posi += (strlen(qi)+1);
          strcpy(tmp+posi,qs);
          posi += (strlen(qs)+1);

          quitab(qo);

          strcpy(tmp+posi,qo);
          posi += (strlen(qo)+1);
		  strcpy(tmp+posi,htext);
		  posi += (strlen(htext)+1);
          
          /*chequea(tipo,qo,qi,qs,decimal,maxqc);*/

          if ( (flag_extrafuncion & 2) ) /* es control */
          {
              (*DespachadorExterno)((int)DESPACHO_EDITA,(int)0/* de momento solo grid */,tmp);
          }
          else
          {
              (*DespachadorExterno)((int)DESPACHO_EDITA,(int)qp*10000+qp+qc-1,tmp);
          }
		  
		  /* recoge los dos bytes menos significativos */
          memcpy_int(&qj,tmp,sizeof(qj));
		  /*qj = (int)((short)qj);*/

          llenastr(maxqc,tmp+sizeof(int),qo);

          if (qj >= 0 && qj != 7)
          {
            if (chequea(tipo,qo,qi,qs,decimal,maxqc) == -1) 
            {
               pito(139);               
               llenastr(maxqc,qd,qo);  /*  qo=qd+blancos  */
               continue;
            }            		    	
            break;
          }
          else
            break;

          } while(-1);         
          
          
          goto entra_sale;
      }


      
      if (tipo == 3 && (c = decimales(qo)) != 0)
         qo[qc - c - 1] = ',';
      
      
      if (Patrib != rentra_atrib || PCcolor != rentra_cc || PFcolor != rentra_cf) {
         atributo(tolower(Patrib));
         if (rentra_cc != rentra_cf)
            Color(rentra_cc,rentra_cf);
         atributo(rentra_atrib);
      }

      Patrib |= 128;
      
      if (rentra_offqp < 1 && rentra_offqp > -2) 
	  {
         cursor(qp);
         prom(qo+contai,qc);            /*  pintar valor por defecto */
      }
      
      if (conta < 0 || conta >= maxqc) conta = 0; /* permite al flag -1 de rentra_offqp */
      
      cursor(qp+conta);
      
      while (mientras) {
         if (conta >= qc) 
         {
            c = conta - qc;
            conta = qc - 1;
            if ( (contai + qc + c) < maxqc) 
            {
               contai += c + 1;
               if (rentra_edicion) 
               {
																		qj = 1000; /* se¤al de desplazamiento */
                                                      break; /* el desplazamiento se produce fuera !! */
               }
               cursor(qp);
               pstr(qo+contai,0,qc);
            }
         }
         
         if (rentra_pos)
            (*rentra_pos)(qp,contai,conta,insert);
         
         cursor(qp+conta);
         if (insert == CIERTO)
            curon(2);
         else
            curon(1);
         c = leecar();
         curon(0);
         
         if (c == SOLICITA_POSICION)
         {
            if ((_M_Cursor/100) == (qp/100))
            {
               if ((_M_Cursor%100) >= (qp%100)  && (_M_Cursor%100) < (qc+qp%100) )
               {
																		conta = (_M_Cursor%100) - (qp%100);
                                                      continue;
               }
            }
         }
         
         if (qh == 2) 
		 {
            qj = 0;
            switch(c) 
			{
            case W_BLOQUE:
               qj = 100;
               break;
            case W_RAPIDO:
               qj = 700;
               break;
            case W_PANTALLA:
               qj = 500;
               break;
            case W_IMPRESION:
               qj = 600;
               break;
            case W_SIGUIENTE:
               qj = 998;
               break;
            case W_BORRALIN:
               qj = 999;
               break;
            }
            if (qj) break; /* salir del while (sin chequea) */
         }
         if ((qj = que_qjes(c,flag_delineas+especial_tecla_linea)) != -1 && qj != 28)
         { /* funcion calculadora -- repintar qo */
            if (qj == 0 || qh != 0) {
               if (chequea(tipo,qo,qi,qs,decimal,maxqc) == -1) {
																		if (qj == 7)
                                                      {
                                                         if (!rentra_edicion)
                                                            llenastr(maxqc,qd,qo);
                                                         mientras = 0;
                                                      }
                                                      else
                                                      {
                                                         pito(140);
                                                         conta=0;
                                                      }
               } 
               else
																		mientras = 0;
            }
         }
         else
            switch (c) {
      case DERECHA :
         tocado = 1;
         conta++;
         if (rentra_edicion && insert == FALSO && (contai + conta) < maxqc) {
            if (qo[contai+conta] == ' ') {
               qj = maxqc - 1;
               while(qj && qo[qj-1] == ' ' && qo[qj] == ' ') qj--;
               if (qj < (contai+conta))
                  conta = maxqc;  /* salir por final !!! */
               qj = -1;
            }
         }
         if ((conta+contai) >= maxqc && !rentra_edicion && !rentra_offqp)
         {
            qj = 3;
            mientras = 0;
         }
         break;
      case IZQUIERDA :
         tocado = 1;
         if (conta > 0) {
            conta--;
         }
         else {
            if (contai) {
               contai--;
               if (rentra_edicion) {
                  mientras = 0;
                  qj = 2000; /* desplazar a la izquierda */
                  break;
               }
               cursor(qp);
               pstr(qo+contai,0,qc);
            }
            else
               if (rentra_offqp) {
                  conta = maxqc; /* para salir del bucle */
               }
               else
               {
                  if (!rentra_edicion)
                  {
                     qj = 2;
                     mientras = 0;
                  }
               }
         }
         break;
      case RETROCEDER :
      case BACKSPACE :
         tocado = 1;
         if (conta || contai) {
            if (!conta) {
               contai--;
               if (rentra_edicion) {
                  mientras = 0;
                  qj = 2000; /* desplazar a la izquierda */
               }
            }
            else
               conta--;
            del1(qo+conta+contai);
            if (rentra_edicion && qj == 2000)
               break;
            cursor(qp+conta);
            pstr(qo+contai,conta,qc);
         }
         else
            if (rentra_edicion) {
               qj = -4; /* salir borrando por delante */
               mientras = 0;
            }
            break;
      case FTAB:
         if (qh > 0 && !rentra_edicion && !rentra_offqp)
         {
            if (chequea(tipo,qo,qi,qs,decimal,maxqc) == -1) 
			{
               pito(141);
               conta=0;            
               break;
            }
            qj = 3;
            mientras = 0;
            break; 
         }
         tocado = 1;
         conta += ETAB;
         if (rentra_edicion) {
            conta -= (conta % ETAB);
            if (insert == FALSO && (conta+contai) < maxqc) {
               if (qo[contai+conta] == ' ') {
                  qj = maxqc - 1;
                  while(qj && qo[qj-1] == ' ' && qo[qj] == ' ') qj--;
                  if (qj < (contai+conta))
                     conta = maxqc;  /* salir por final !!! */
                  qj = -1;
               }
            }
         }
         break;
      case BTAB:
         if (qh > 0 && !rentra_edicion && !rentra_offqp)
         {
            if (chequea(tipo,qo,qi,qs,decimal,maxqc) == -1) {
               pito(142);
               conta=0;
               break;
            }            	
            qj = 2;
            mientras = 0;
            break; 
         }      
         tocado = 1;
         if (rentra_edicion && conta) 
         {
            conta -= ((conta-1) % ETAB + 1);
         }
         else
            conta -= ETAB;
         if (conta < 0) {
            if (contai) {
               contai += conta;
               if (contai < 0)
                  contai = 0;
               if (rentra_edicion) {
                  mientras = 0;
                  qj = 2000; /* desplazar a la izquierda */
                  break;
               }
               cursor(qp);
               pstr(qo+contai,0,qc);
            }
            else
               if (rentra_offqp) {
                  conta = maxqc; /* para salir del bucle */
                  break;
               }
               conta = 0;
         }
         break;
      case END :
         tocado = 1;
         conta = maxqc - 1;
         while(conta > 0 && qo[conta-1] == ' ' && qo[conta] == ' ') conta--;
         if (contai) {
            contai = 0;
            if (conta < qc) {
               if (rentra_edicion) {
                  mientras = 0;
                  qj = 2000; /* desplazar a la izquierda */
                  break;
               }
               cursor(qp);
               pstr(qo,0,qc);
            }
         }
         break;
      case HOME:
         tocado = 1;
         conta=0;
         while(qo[conta] == ' ' && conta < qc) /* no se usa maxqc sino qc */
            conta++;
         if (conta == qc) conta = 0;
         if (contai) {
            contai = 0;
            if (rentra_edicion) {
               mientras = 0;
               qj = 2000; /* desplazar a la izquierda */
               break;
            }
            cursor(qp);
            pstr(qo,0,qc);
         }
         break;
      case CTRLHOME :
         llenastr(maxqc,qo,antiguo); /* pone en antiguo el string qo */
         llenastr(maxqc,qd,qo);
         
      case PFUNCIONENTRA: /* caso devuelto por una funcion programado */
         llenastr(maxqc,qo,qo);
         tocado = 1;
         cursor(qp);
         pstr(qo,0,qc);
         contai = 0;
         conta=0;
         if (rentra_edicion) 
         {
            mientras = 0;
            qj = 2000; /* desplazar a la izquierda */
         }
         break;
         
      case CTRLEND :    /*  borrar hasta final */
         tocado = 1;
         llenastr(maxqc,qo,antiguo); /* pone en antiguo el string qo */
         llenabla(maxqc-conta-contai,qo+conta+contai);  /* borrar final de qo */
         pstr(qo+contai,conta,qc);
         break;
      case DEL :
         tocado = 1;  /* borra el primero y pone un blanco al final */
         if (del1(qo+conta+contai) && rentra_edicion) 
         {
            mientras = 0;
            qj = -2; /* salir por delete final (sin chequea) */
         }
         pstr(qo+contai,conta,qc);
         break;
      case INS :
         tocado = 1;
         if (insert == FALSO) {
            insert = CIERTO;
         }
         else {
            insert = FALSO;
         }
         break;
      default :
         if (c<' ') {/****/
            pito(143);
            break;
         }
         if (tipo == 2) 
         {        /* fecha */
            if (((c == '.' || c == '/') && conta == 0 && maxqc == 6) || ((maxqc == 10 || maxqc == 8) && (conta == 2 || conta == 5)))
            {		   
               if (data_fecha_conmutar && c != caracter_de_fecha && (c == '.' || c == '/'))
               {			  
                  caracter_de_fecha = c;
                  if (c == '.')
                     data_fecha_conmutar = 2;
                  else
                     data_fecha_conmutar = 3;
                  if (conta == 2)
                  { 				 
                     qo[2] = caracter_de_fecha; 
                     qo[5] = caracter_de_fecha;
                     cursor(qp);
                     pstr(qo,0,qc);
                     cursor(qp+conta);
                  }
               }
               if (maxqc == 6)
               {
                  break;
               }
               c = caracter_de_fecha;
            }
            else 
               if (c < '0' || c >'9') 
               {
                  if (c == ':')
                  {
                     indicator__ = 1;
                  }
                  else
                     pito(144);
                  break;
               }
         }
         else if (tipo == 3) 
		 {     /* numero */
			int oc;
            if ((oc = numero(c)) == -1) 
			{
			   if (c == 'e')
				   c = 'E';
			   else if (c == 'p')
				   c = 'P';
			   if (conta || !euro_campo || (c != 'E' && c != 'P'))
			   {
				   c = -1;
				   pito(145);
				   break;
			   }
            }
			else
			  c = oc;
         }
         
         if (rentra_edicion) {
            i = conta + contai + 1 + insert; /* insert si CIERTO -1 */
            while(i < maxqc && qo[i] == ' ') i++;
         }
         else
            i = 0;
         
         if (insert == CIERTO) {
            /* correr hacia la derecha 1 caracter */
            if (corre1(qo+conta+contai)) {
               qj = -3;
               mientras = 0; /* salir por insertar texto */
               lmin = qo[maxqc];
               qo[maxqc] = 0;
            }
            if (i < maxqc) {
               pstr(qo+contai,conta,qc); /* solo pinta si hay algo */
               cursor(qp+conta);
            }
            if (qj == -3) qo[maxqc] = lmin;
         }
         
         if (!tocado && !conta && (tipo == 3 || tipo == 1)) {
         /* si es numero y si es la primera tecla que se pulsa
            * se borra el valor por defecto */
            tocado = 1;
            llenastr(maxqc,qo,antiguo); /* pone en antiguo el string qo */
            llenabla(maxqc-conta-contai,qo+conta+contai);  /* borrar final de qo */
            pstr(qo+contai,conta,qc);
            cursor(qp+conta);
         }
         /* Pcursor = ((qp+conta)/100-1)*80 + (qp+conta)%100 - 1; */
         qo[contai+conta]=(char) c;
         conta++;
         if (qh != -1) {
            Conversion((char) c);
            v_putc((char) c);
         }
         else {
            Conversion('*');
            v_putc('*');
         }
         
         if (qh > 0 && !rentra_edicion && !insert && (conta+contai) >= maxqc && !rentra_offqp && rentra_autosal)
         {
            if (chequea(tipo,qo,qi,qs,decimal,maxqc) == -1) {
               pito(146);
               conta=0;            
               continue;
            }            		    	
            mientras = 0;
            qj = 0;
         }
         
         if ( tipo == 2 && (maxqc == 8 || maxqc == 10) && (conta == 2 || conta == 5) ) 
         {
            conta++;
            if (conta == 6 && maxqc == 10)
               conta += 2;
         }
         if (i == maxqc && (contai + conta) == rentra_edicion) {
            if (insert) {
               qj = -3;
               mientras = 0;
            }
            conta = maxqc;
         }
      }
      if (rentra_edicion && (contai + conta) >= maxqc && qj == -1)
      { /* salir por teclear al final del margen */
        conta = maxqc;
        if (c == DERECHA || c == FTAB)
           qj = 3; /* abajo */
        else
        if (c == IZQUIERDA || c == BTAB)
           qj = 2; /* arriba */
        else
           qj = 0;
        mientras = 0;
      }
   }
   
   atributo(tolower(rentra_atrib));
   Patrib &= ~128;

entra_sale:

#ifdef FGVWIN
  if (TeclaScanner)
  {
	  int _eqj = eqj;
	  ScannerUnGet();
	  if (qj == que_qjes(TeclaScanner))
	  {
		 DameScanner(qo);
	  }
	  eqj = _eqj;
  }
#endif
   
  if (tipo == 1 && *qs)
  {
      unsigned char relleno = ' ';      

      i = strbusca(qs,"\\");
      if (i >= 0)
      {
          i = (int)((unsigned char)qs[i+1]);
          if (i)
          {
              relleno = (unsigned char)i;
          }
      }
      
      if (strbusca(qs,"+") >= 0)
      {
          int dd;          
          
          quitab(qo);
          quitabp(qo);
          i = strlen(qo);

          dd = maxqc-i;
          for (c = i;c >= 0;c--)
          {
              qo[dd + c] = qo[c];
          }
          for (c = 0;c < dd;c++)
              qo[c] = relleno;
      }
      else
      {
          if (relleno != ' ')
          {
              quitab(qo);
              i = strlen(qo);
              for (;i < maxqc;i++)
                  qo[i] = relleno;
              qo[i] = 0;
          }
      }

      if (strbusca(qs,">") >= 0)
      {
          strmayus(qo);
      }
      else
      if (strbusca(qs,"<") >= 0)
      {
          strminus(qo);
      }

      
  }

  if (euro_campo && (qo[0] == 'E' || qo[0] == 'P'))
  {
	  char tmpxx[350];
	  /*
      if (decimal == -1) 
      {
          decimal = decimales(qo);
      }
      sprintf(usi,"%%%d.%dlf",qc,decimal);
	  */
      if (qo[0] == 'E')
	  {
		  sprintf(tmpxx,"%lf",rwatof(qo+1)*166.386);
		  quitadec(tmpxx);
		  strcpy(qo,tmpxx);
	  } else if (qo[0] == 'P')
	  {
		  double euros = rwatof(qo+1);
		  euros /= (double)166.386;
		  sprintf(tmpxx,"%lf",euros);
		  quitadec(tmpxx);
		  strcpy(qo,tmpxx);
	  }

  }

   if (si_pintar) { /* solo en  modo normal */
      
      if (rentra_sal_cc != rentra_sal_cf)
         Color(rentra_sal_cc,rentra_sal_cf);
      atributo(rentra_sal_atrib);
      Patrib |= 128;
      
      cursor(qp);
      if (tipo == 3) 
      {
          if (decimal == -1) 
          {
              decimal = decimales(qo);
          }
          sprintf(usi,"%%%d.%dlf",qc,decimal);
          sprintf(qo,usi,rwatof(qo)); /* ponemos en qo el resultado formateado */
          if (strlen(qi) > 1) 
          {
              if (qi[0] == '0') 
              {
                  for(c = 0;*(qo+c) != '\0';c++)
                      if (*(qo+c) == ' ') *(qo+c) = '0';
              }
              else
              if (qi[0] == ' ' && rwatof(qo) == 0.0) 
              {
                  llenabla(maxqc,qo);
              }
          }
          strcpy(antic,qo);
          poncoma(antic,antic);
          pstr(antic,0,qc);
      }
      else 
      {
          if (qh != -1) 
          {
                pstr(qo,0,qc);
          }
          else
                pstr(llenabla(maxqc,antiguo),0,qc);
      }
      
      atributo(tolower(rentra_sal_atrib));
      Patrib &= ~128;
   }
   else {
      if (rentra_offqp) {
         rentra_offqp = conta;
         rentra_offini= contai;
         if (insert == CIERTO)
            rentra_insert = 1;
         else
            rentra_insert = 0;
      }
   }
   
   fflush(stdout);
   
   dato_entra = o_calculadora;
   dato_entra_len = _dato_entra_len;
   /* if (tipo == 2) */
   data_hiden_indicator = indicator__;
   return(qj);
}

int  entra(qp,maxqc,qi,qs,qd,qh,qo)
{
	return i_entra(qp,maxqc,qi,qs,qd,qh,qo,0,NULL);
}



#ifdef _MAIN

main()
{
   char qo[257];
   int qj;
   
   Terinici();
   cls();
   
   qj = entra(1001,256,"","","",101,qo);
   
   Terfinal();
}


#endif

