/*---------------------------------------------------------------------------
* Programa ..: rmenusg.c
* Fecha .....: 15-12-87
* Version ...: 1.00
* Notas .....: Rutina de menus en linea
* Contenido .: int  menus(), menusg()
*---------------------------------------------------------------------------*/

#include <fgvsys.h>

#include <stdio.h>
#include <string.h>
#include <memory.h>

#include <teclas.h>
#include <rtermin.h>

int  menusg_virtual = 0;

#define OPCIONES 100       /* numero de opciones posibles menus */



#ifdef FGV32
#ifndef FGVWIN
#ifndef WATCOMNLM
extern HANDLE myconsola;
extern HANDLE hStdIn;
#endif
#endif
#endif
#ifdef RWXARXA
#ifdef RWSERVER
extern int en_conexion_externa;
extern int en_conexion_externa_stop;
#endif
#endif


/* -----------------------------------------------------------------*/
extern int fgv_ver_euro;

/*
* enstr
* devuelve la posicion que ocupa el caracter 'c' dentro
* del string 's',(de 0 a strlen(s)-1)
* o bien -1 si no existe
*/
int enstr(s,c)
char *s;
int c;
{
   char *p;
   
   if ((p = strchr(s,c)) == NULL)
      return(-1);
   else
      return((int)(p-s));
}

/* -----------------------------------------------------------------*/


#ifndef FGVWIN
/* menus   rutina de menu en linea con caracter de acceso
* opcion[0]="Primera opcion"; opcion[1]="Segunda opcion";
* letras="ABC"; pos=2401; texto="Elija opcion ..:"; defecto=2;
* menul(opcion,letras,pos,defecto);
* devuelve el numero de opcion elegida
* o bien 0=esc  -1=ctrl-c  -2=pgup  -3=pgd
* toma como opcion por defecto la indicada en el numero 'defecto',
* si no existe toma la primera '1'
* el numero de opciones es la longitud de letras
* letras es el conjunto de caracteres ordenado que dan acceso a 
* las diferentes opciones
*/
int menus(opcion,letras,pos,texto,defecto,lineas)
char *opcion[];    /* texto de las opciones */
char *letras;              /* caracteres de acceso a las opciones */
int pos;                   /* posicion para pintar las opciones */
/* si es 0 se calcula automaticamente */
/* centrado en la linea 24 */
char *texto;               /* texto indicativo */
int defecto;               /* opcion por defecto >0 y <=numero */
/* en caso contrario toma 1 */
int lineas;                /* numero de lineas de menu */
/* solo se usa cuando se llama desde menusg */
/* en caso contrario se pone a cero con lo */
/* que se desactivan pgup y pgdn */
{
   int l=0;     /* suma de las longitudes de cada opcion */
   int l1,f1,c1;
   int i;
   int a,c,ii;
   int mientras = -1;
   int elegida = ((defecto == 0) ? 1 : defecto);
   int anterior = 0;
   int col[50];
   int numero=strlen(letras);                /* numero de opciones */
   int d = 0;
   char tmpx[100];
   
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
			{
      elegida = menusg_en_terminal(opcion,letras,pos,texto,defecto,lineas);
      return(elegida);
			}
#endif
#endif
   
   
   strmayus(letras);
   
   l = strlen(texto)+1;
   for (i=0;i<numero;i++) {
      col[i] = l;
      l += strlen(opcion[i]);
      l++;
			}
   
   if (pos != -1) {
      f1 = pos/100;
      c1 = pos%100;
      if (f1 <= 0)
         f1 = 24;  /* fila en pantalla del menu */
      if (c1 <= 0)
         c1 = (80-l)/2+1;     /* columna         "                     */
      for (i=0;i<numero;i++)
         col[i] = f1 * 100 + col[i] + c1;
   }
   else {
      if (strlen(texto)) {
         f1 = (texto[0] - 48) * 10 + (texto[1] - 48);
         c1 = (texto[2] - 48) * 10 + (texto[3] - 48);
      }
      else {
         f1 = (opcion[0][0] - 48) * 10 + (opcion[0][1] - 48);
         c1 = (opcion[0][2] - 48) * 10 + (opcion[0][3] - 48);
      }
      d = 4;
      for (i=0;i<numero;i++) 
      {
         memcpy(tmpx,opcion[i],4);
         tmpx[4] = 0;
         col[i] = atoi(tmpx);
      }
   }
   
   if (!menusg_virtual || menusg_virtual == CRETURN || menusg_virtual == -1) {
      if (!menusg_virtual || lineas)
         Bl(f1);
      if (strlen(texto)) {
         cursor(f1*100+c1);
         v_print(texto+d);
      }
      if (lineas)      /* hay varias lineas  */
         pprint(f1*100+78,"..");
      for (i=0;i<numero;i++) {
         cursor(col[i]);
         v_print(opcion[i]+d);
         if ((ii = enstr(opcion[i]+d,*(letras+i))) != -1) {
            Ai();
            cursor(col[i]+ii);
            v_printc("%c",*(letras+i));
            Bi();
         }
      }
			}
   
   while (mientras) {
      if (anterior != 0 && anterior != elegida) {
         cursor(col[anterior-1]);
         v_prints("%s",opcion[anterior-1]+d);
         if ((ii = enstr(opcion[anterior-1]+d,*(letras+anterior-1))) != -1) {
            Ai();
            cursor(col[anterior-1]+ii);
            v_printc("%c",*(letras+anterior-1));
            Bi();
         }
      }
      if (mientras == 2) /* asi alvolver por aqui se borra el reverse de la opcion */
         break;
      
      if (elegida < 1) {
         if (lineas) {    /* hay varias lineas  */
            elegida = -2;
            break;
         }
         else
            elegida = numero;
      }
      else if (elegida > numero) {
         if (lineas) {    /* hay varias lineas  */
            elegida = -3;
            break;
         }
         else
            elegida = 1;
      }
      
      if (!menusg_virtual || menusg_virtual == -1 || menusg_virtual == CRETURN) {
         cursor(col[elegida-1]);
         Ar();
         v_prints("%s",opcion[elegida-1]+d);
         Cr();
      }
      if (menusg_virtual) {
         if (menusg_virtual == -1) {
            return(elegida);
         }
         c = menusg_virtual;
         menusg_virtual = -1;
      }
      else 
	  {
         c = leecar();
      }
      
      anterior = elegida;
      
      if (c == SOLICITA_POSICION)
      {
         for (i=0;i<numero;i++) {
            if ( (_M_Cursor/100) == (col[i]/100) && (_M_Cursor%100) >= (col[i]%100) )
            {
               if ((_M_Cursor%100) < (strlen(opcion[i]+d)+col[i]%100) )
               {
                  elegida = i + 1;
                  /*if (elegida == anterior) */
                  c = CRETURN;
                  break;
               }
            }
         }
         /* sino le pertenece sale y transfiere el control */
         elegida = SOLICITA_POSICION;
         mientras = 0;
         break;
      }
      
      switch (c) {
      case HOME :
         elegida = 1;
         break;
      case END  :
         elegida = numero;
         break;
      case PGUP :
         /* if (lineas) {    /* hay varias lineas  */
         elegida = -2;
         mientras = 0;
         /*  }
         // else {
         //   anterior = 0;
         //   }*/
         break;
      case PGDN :				
         /*if (lineas) {    /* hay varias lineas  */
         elegida = -3;
         mientras = 0;
         /*   }
         //else {
         //   anterior = 0;
         //   }*/
         break;
      case SOLICITA_AVANCE:
      case SOLICITA_RETROCESO:				  				  
         elegida =  c;
         mientras = 0;
         break;
      case ARRIBA :
      case IZQUIERDA :
      case RETROCEDER :
      case BACKSPACE :
      case '-' :
         elegida--;
         break;
      case ABAJO :
      case DERECHA :                
      case '+' :
         elegida++;
         break;
      case VALIDAR :
      case ' ' :
      case CRETURN :
         mientras = 2;
         break;
      case ACABAR :
      case ESC :
         elegida = 0;
         mientras = 2;
         break;
      case ABORTAR :
      case CTRLC :
         elegida = -1;
         mientras = 2;
         break;
      default :
         if (c > 96) c -= 32;
         if ((a = enstr(letras,c)) < 0) {
            anterior = 0;
            break;
         }
         else {
            elegida = a+1;
            if (!menusg_virtual)
               mientras = 2;
            break;
         }
      }
        }
        
        if (elegida > -2 && !menusg_virtual)
           Bl(f1);
        return(elegida);
}

#endif

/* menusg   rutina de menu en linea con caracter de acceso para muchas opciones
* opcion[0]="Primera opcion"; opcion[1]="Segunda opcion";
* letras="ABC"; pos=2401; texto="Elija opcion ..:"; defecto=2;
* menul(opcion,letras,pos,defecto);
* devuelve el numero de opcion elegida
* o bien 0=esc  -1=ctrl-c
* toma como opcion por defecto la indicada en el numero 'defecto',
* si no existe toma la primera '1'
* el numero de opciones es la longitud de letras
* letras es el conjunto de caracteres ordenado que dan acceso a 
* las diferentes opciones
*/
int menusg(opcion,letras,pos,texto,defecto)
char *opcion[];    /* texto de las opciones */
char *letras;              /* caracteres de acceso a las opciones */
int pos;                   /* posicion para pintar las opciones */
/* si es 0 se calcula automaticamente */
/* centrado en la linea 24 */
char *texto;               /* texto indicativo */
int defecto;               /* opcion por defecto >0 y <=numero */
/* en caso contrario toma 1 */
{
   int l=0;
   int i,p=0,q=0;
   int c;
   int mientras = -1;
   int elegida;
   int defec;
   int anterior = 0;
   int salida;
   int bloques;
   int numero = strlen(letras);
   int linea[OPCIONES];
   int def[OPCIONES];
   char letra[OPCIONES/15][15];
   int comienzo[OPCIONES/15];
   
   p = 0;
   q = 0;
   comienzo[0] = 0;
   l = strlen(texto)+1;
   
   if (l >= OPCIONES) return(-1);
   
   for (i=0;i<numero;i++) 
   {
      if (pos != -1) 
	  {
         l += strlen(opcion[i]);
         l++;
      }
      else 
	  {
         if ((i+1) < numero) 
		 {
            l = ((opcion[i][2] - 48) * 10 + (opcion[i][3] - 48));
            c = (opcion[i+1][2] - 48) * 10 + (opcion[i+1][3] - 48);
            if (l > c)
               l = 76;
         }
      }
      if (l > 75) 
	  {
         l = strlen(texto)+strlen(opcion[i])+2;
         letra[p][q] = '\0';
         q = 0;
         p++;
         comienzo[p] = i;
      }
      linea[i+1] = p+1;   /* linea a la que asignamos la opcion 'i' */
      def[i+1] = q+1;
      letra[p][q] = *(letras+i);
      q++;
   }
   letra[p][q] = '\0';
   bloques = p+1;
   
   elegida = ((defecto == 0) ? 1 : (linea[defecto]));
   defec = ((defecto == 0) ? 1 : (def[defecto]));
   while (mientras) 
   {
      if (elegida < 1)
         elegida = bloques;
      else if (elegida > bloques)
         elegida = 1;
      anterior = elegida;
      i = comienzo[elegida-1];
      c = menus(opcion+i,letra[elegida-1],pos,texto,defec,bloques-1);
      defec = 0;              /* solo toma valor la primera vez */
      if (c == SOLICITA_POSICION)
      {
         salida = SOLICITA_POSICION;
         break;
      }
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
         if (bloques < 2)
         {
            salida = -(3 + anterior*10);
            mientras = 0;
         }
         else
         {
            elegida--;
            if (elegida < 1)
               elegida = bloques;
            defec = strlen(letra[elegida-1]);
         }
         break;
      case -3 :                /* pgdn */
         if (bloques < 2)
         {
            salida = -(4 + anterior*10);
            mientras = 0;
         }
         else
         {
            elegida++;
            defec = 1;
         }
         break;
      default :
         salida = c+comienzo[elegida-1];
         mientras = 0;
         break;
      }
   }
   menusg_virtual = 0;
   return(salida);
}

