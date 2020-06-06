
/*---------------------------------------------------------------------------
* Programa ..: rmenug.c
* Fecha .....:
* Version ...:
* Notas .....: Rutina de menus en pantalla
* Contenido .: int  menu(), menug()
hay modificaciones varias
*---------------------------------------------------------------------------*/

#include <fgvsys.h>

#include <stdio.h>
#include <string.h>
#include <memory.h>

#include <teclas.h>
#include <rtermin.h>


#define OPCIONES 1500       /* numero de opciones posibles menus */
#define DEFTAMANO 10
static int TAMANO=10;       /* opciones maximas por un menu */
#define MINTAMANO 4

#include <stdlib.h>
/*extern char *getenv();*/
static int _entra;
static int _pinta,lado1,lado2;
static char **iniop;
static int p1 = 0,p2 = 0,pd = 0;


extern int wmenu_en_alfa;

char *menug_cabe = (char *)0;
char *menug_teclas = (char *)0;;
int  menug_virtual = 0;
int  menug_numera  = 0;
int  menug_letras = 0;
VENT *menug_vent = NULL;
int  menug_salida = 0;
char *menug_consulta = NULL;

/* -----------------------------------------------------------------*/

/* menu   rutina de menus
* opcion[0]="Primera opcion"; opcion[1]="Segunda opcion";
* numero=2; pos=0; paginas=0;
* menu(opcion,numero,pos,paginas);
* devuelve el numero de opcion elegida
* o bien 0=esc  -1=ctrl-c  -2=pgup  -3=pgdn
*/
static int menu(opcion,numero,pos,paginas,defecto,l)
char **opcion;    /* texto de las opciones */
int numero;                /* numero de opciones */
int pos;                   /* posicion para pintar las opciones */
/* si es 0 se calcula automaticamente */
int paginas;               /* numero de paginas-1 del menu */
/* solo se usa cuando se llama desde menug */
/* en caso contrario se pone a cero con lo */
/* se desactivan pgup y pgdn */
int defecto;               /* opcion por defecto, si es 0 se toma 1 */
int l;
{
   int f1,c1;
   int i;
   int a,c;
   int mientras = -1;
   int elegida = ((defecto == 0) ? 1 : defecto);
   int anterior = 0;
   char usi[10];
   char *pros;
   
   recurre_raton(0);
   recurre_boton(0);
   
   
   f1 = pos/100;
   c1 = pos%100;
   if (f1 <= 0)
      f1 = (28-numero)/2;  /* fila en pantalla de la primera opcion */
   if (c1 <= 0)
      c1 = (80-l)/2+2;     /* columna         "                     */
   
   if (_entra)
      sprintf(usi,"%%-%ds",l);
   else
      sprintf(usi," %%-%ds ",l);  /* using para el print */
   
   
   if (!_pinta) {
      
      if (menug_virtual && !menug_letras)
         c = TAMANO;
      else
         c = numero;
      zonablan((f1- ((menug_letras) ? 1 : 2) )*100+c1+lado1,(f1+c+((menug_letras && !paginas) ? 0 : 1))*100+c1+l+lado2);
      cuadro((f1- ((menug_letras) ? 1 : 2) )*100+c1+lado1,(f1+c+((menug_letras && !paginas) ? 0 : 1))*100+c1+l+lado2);
      if (menug_letras) {
         cursor((f1 - 1) * 100+c1+lado1+1+pd);
         Gon();
         Graf(9);
         Graf(9);
         Goff();
      }
      if (menug_cabe) {
         c = (lado2 - lado1 - 2) + l - strlen(menug_cabe);
         c /= 2;
         Ar();
         pprint((f1- ((menug_letras) ? 1 : 2) )*100+c1+lado1+1+c,menug_cabe);
         Cr();
      }
      
      anula_mapa_raton(-1);
      for (i=0;i<numero;i++) {
         if (!menug_letras) {
            cursor((f1+i)*100+c1-5);
            pon_mapa_raton((f1+i)*100+c1-5,(f1+i)*100+c1+l+3,i+1,1);
            
            Ar();
            if (menug_numera)
               v_printd("%3d",(int)(opcion - iniop)+i);
            else
            {
               if (wmenu_en_alfa)
                  v_printc(" %c ",'A'+i);
               else
                  v_printc(" %c ",'0'+i);
            }
            Cr();
            pprint(0,"..");
         }
         cursor((f1+i)*100+c1+_entra);
         v_prints(usi,opcion[i]);
      }
      if (paginas) {
         cursor((f1+numero)*100+c1 - ((menug_letras) ? 0 : 5));
         pprint(0,"   ..");
      }
      
   }
   else
      _pinta = 0;
   
   
   while (mientras) {
      if (anterior != 0 && anterior != elegida) {
         cursor((f1+anterior-1)*100+c1+_entra);
         v_prints(usi,opcion[anterior-1]);
      }
      if (elegida < 1) {
         if (paginas) {     /* hay varias pantallas */
            elegida = -2;
            break;
         }
         else
            elegida = numero;
      }
      else if (elegida > numero) {
         if (paginas) {     /* hay varias pantallas */
            elegida = -3;
            break;
         }
         else
            elegida = 1;
      }
      
      if (_entra) {
         c = -entra((f1+elegida-1)*100+c1+1,l,"","",
            opcion[elegida-1],1,opcion[elegida-1]);
      }
      cursor((f1+elegida-1)*100+c1+_entra);
      Ar();
      v_prints(usi,opcion[elegida-1]);
      Cr();
      
      anterior = elegida;
      
      if (menug_virtual) {
         if (menug_virtual == -1) {
            recurre_raton(1);
            recurre_boton(1);
            return(elegida);
         }
         c = menug_virtual;
         menug_virtual = -1;
      }
      else
         if (!_entra)
         {
            c = leecar();
            if (c == SOLICITA_POSICION)
            {
               elegida = _ID_Cursor;
               _ID_Cursor = -1;
               if (elegida == anterior)
                  c = CRETURN;
               else
                  continue;
            }
         }
         switch (c) {
         case HOME :
            elegida = 1;
            break;
         case END  :
            elegida = numero;
            break;
         case PGUP :
         case -4:
            if (paginas) {    /* hay varias paginas */
               elegida = -2;
               mientras = 0;
            }
            else {
               anterior = 0;
            }
            break;
         case PGDN :
         case -5:
            if (paginas) {    /* hay varias paginas */
               elegida = -3;
               mientras = 0;
            }
            else {
               anterior = 0;
            }
            break;
         case ARRIBA :
         case RETROCEDER :
         case BACKSPACE :
         case '-' :
         case -2:
            elegida--;
            break;
         case ABAJO :
         case ' ' :
         case '+' :
         case -3:
            elegida++;
            break;
         case FUNCI_1:
            if (menug_salida == -1) {
               menug_salida = 3;
               mientras = 0;
            }
            break;
         case INS:
            if (menug_salida == -1) {
               menug_salida = 1;
               mientras = 0;
            }
            break;
         case DEL:
            if (menug_salida == -1) {
               menug_salida = 2;
               mientras = 0;
            }
            break;
         case VALIDAR :
         case CRETURN :
         case 0 :
            menug_salida = 0;
            mientras = 0;
            break;
         case ACABAR :
         case ESC :
         case -1 :
            elegida = 0;
            mientras = 0;
            break;
         case ABORTAR :
         case CTRLC :
         case -7 :
            elegida = -1;
            mientras = 0;
            break;
         case CONSULTAR:
         case -6:
            if (paginas) {
               elegida += 10000;
               mientras = 0;
            }
            break;
         case FTAB:
         case BTAB:
            if (menug_teclas) {
               elegida += 1000;
               mientras = 0;
               break;
            }
            break;
         case DERECHA :
            if (menug_teclas) {
               elegida += 2000;
               mientras = 0;
            }
            break;
         case IZQUIERDA :
            if (menug_teclas) {
               elegida += 3000;
               mientras = 0;
            }
            break;
         default :
            c = toupper(c);
            if (menug_teclas) {
               for (a = 0;(a < 6 && *(menug_teclas+a));a++) {
                  if (*(menug_teclas+a) == (char)c) {
                     a += 4;
                     recurre_raton(1);
                     recurre_boton(1);
                     return(elegida + (a * 1000));
                  }
               }
            }
            if (wmenu_en_alfa)
               a = c - 'A' + 1;
            else
               a = c - '0' + 1;
            if (a > 0 && a <= numero) {
               elegida = a;
            }
            else {
               anterior = 0;
            }
            break;
      }
   }
   recurre_raton(1);
   recurre_boton(1);
   return(elegida);
}


/* menug   rutina de menus para muchas opciones
* strcpy(opcion[0],"Primera opcion");...strcpy(opcion[29],"Opcion numero 29");
* numero=30; pos=1010; defecto=15; (opcion por defecto)
* menug(opcion,numero,pos,defecto);
* devuelve el numero de opcion elegida
* o bien 0=esc  -1=ctrl-c
*/
int menug_tam(opcion,numero,pos,defecto,tamano)
char **opcion;    /* texto de las opciones */
int numero;                /* numero de opciones */
int pos;                   /* posicion para pintar las opciones */
/* si es 0 se calcula automaticamente */
int defecto;               /* opcion por defecto, si es 0 se toma 1 */
{
   int l=0;     /* longitud maxima de una opcion */
   int l1,l2,f1,c1;
   int i,j;
   int c;
   int mientras = -1;
   int elegida;
   int defec;
   int anterior = 0;
   int salida;
   int bloques;
   int posic;
   int cantidad[OPCIONES/MINTAMANO+1]; /* numero de opciones de cada pantalla */
   VENT *wn = NULL;
   int venta = 0;
   int _tmp1 = 0;
   char tmp1[81];
   char tmp2[81];
   tmp1[0] = 0;
   
   TAMANO = tamano;
   
   if (!numero) {
      if (menug_vent) {
         w_pinta(menug_vent);
         w_fin(menug_vent);
         menug_vent = NULL;
      }
      else {
         if (p1 && p2) {
            zonablan(p1,p2);
         }
      }
      return(0);
   }
   
   if (!menug_consulta) {
      _tmp1 = 1;
      menug_consulta = tmp1;
   }
   
   
   iniop = opcion;
   
   if (menug_virtual) {
      lado1 = -6;
      lado2 = 3;
   }
   else {
      lado1 = -8;
      lado2 = 5;
   }
   
   if (menug_letras) {
      lado1 = -1;
      lado2 = 2;
   }
   
   if ((defec = defecto) < 0) {
      defec = (-defecto) - 1;
      _pinta = -1;
   }
   else
      _pinta = 0;
   
   elegida = ((defec == 0) ? 1 : ((defec-1)/TAMANO+1));
   defec = ((defec == 0) ? 1 : ((defec-1)%TAMANO+1));
   
   if (pos < 0) {
      pos = (-pos) - 1;
      venta = -1;
   }
   
   if (numero < 0)
   {_entra = 1;numero = -numero;}
   else
      _entra = 0;
   
   
   bloques = (numero-1)/TAMANO+1; /* numero de pantallas */
   for (i=0;i<bloques;i++)
      cantidad[i] = TAMANO;
   i = numero % TAMANO; /* el ultimo menu tiene un numero */
   /* variable de opciones */
   if (i)
      cantidad[bloques-1] = i;
   
   if (menug_cabe)
      l = strlen(menug_cabe) + lado1 - lado2 + 1;
   
   for (i=0;i<numero;i++)
      if (l < (l1 = strlen(opcion[i])))
         l = l1;
      if (menug_letras > l) l = menug_letras;
      f1 = pos/100;
      c1 = pos%100;
      if (f1 <= 0)
         f1 = (28-TAMANO)/2;  /* fila en pantalla de la primera opcion */
      if (c1 <= 0)
         c1 = (80-l)/2+3;     /* columna         "                     */
      posic = f1*100+c1;
      
      if (menug_virtual && !menug_letras)
         c = TAMANO;
      else
         c = cantidad[0];
      
      p1 = (f1- ((menug_letras) ? 1 : 2) )*100+c1+lado1;
      p2 = (f1+c+((menug_letras && bloques == 1) ? 0 : 1))*100+c1+l+lado2;
      
      
      if ( (p2 % 100) > 80 ) {
         pd = (p2 % 100) - 80;
         p2 -= pd;
         p1 -= pd;
         posic -= pd;
      }
      else
         pd = 0;
      
      if (venta) { /* guarda el maximo de ventana que podria usarse */
         if (!menug_virtual) {
            wn = w_ini(p1,p2);
            w_salva(wn);
         }
         else
            if (menug_virtual == CRETURN) {
               if (menug_vent) {
                  rw_error(5);
                  /* printf("\n\r Error interno en menug (venta)\n\r");
                  getchar(); */
               }
               menug_vent = w_ini(p1,p2);
               w_salva(menug_vent);
            }
      }
      
      while (mientras) {
         if (anterior || (venta && !menug_virtual)) {
            zonablan(p1,p2);
         }
         if (elegida < 1)
            elegida = bloques;
         else if (elegida > bloques)
            elegida = 1;
         anterior = elegida;
         i = (elegida-1)*TAMANO;
         c = menu(opcion+i,cantidad[elegida-1],posic,bloques-1,defec,l);
         if (c > 10000) { /* funcion del la tecla CONSULTA : busqueda */
            defec = c % 10000;
            if ((c = entra((f1+cantidad[elegida-1])*100+c1+1,l,"","",menug_consulta,1,menug_consulta)) != 1 && c != 7) {
               quitab(menug_consulta);
               if (strlen(menug_consulta)) {
                  strmayus(menug_consulta);
                  j = 0;
                  c = i+defec;
                  while(j < numero) {
                     if (c >= numero) c = 0;
                     strcpy(tmp2,opcion[c]);
                     strmayus(tmp2);
                     if (strbusca(tmp2,menug_consulta) != -1) {
                        break;
                     }
                     j++;
                     c++;
                  }
                  if (j < numero) {
                     elegida = c / TAMANO + 1;
                     defec   = c % TAMANO + 1;
                  }
               }
            }
            continue;
         }
         defec = 0;              /* solo toma valor la primera vez */
         switch (c) {
         case 0 :                 /* escape */
            salida = 0;
            mientras = 0;
            break;
         case -1 :                /* ctrl-c */
            salida = -1;
            mientras = 0;
            break;
         case -2 :                /* pgup */
            elegida--;
            if (elegida < 1)
               elegida = bloques;
            defec = cantidad[elegida-1];
            break;
         case -3 :                /* pgdn */
            elegida++;
            break;
         default :
            salida = c+(elegida-1)*TAMANO;
            mientras = 0;
            break;
         }
      }
      
      if (venta) {
         if (!menug_virtual) {
            w_pinta(wn);
            w_fin(wn);
            wn = NULL;
         }
      }
      
      menug_virtual = 0;
      menug_cabe = (char *)0;
      menug_teclas = (char *)0;
      menug_numera = 0;
      if (_tmp1) {
         menug_consulta = NULL;
      }
      return(salida);
}


int menug(opcion,numero,pos,defecto)
{
   return(menug_tam(opcion,numero,pos,defecto,DEFTAMANO));
}
