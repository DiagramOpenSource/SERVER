/* --------------------------------------------------------------------------
* Programa ..: rletras.c                  Autor: Francisco Gonzalez Valiente
* Fecha .....: 07-06-1988
* Version ...: 1.00
* Notas .....: Rutina de conversion de numeros a su equivalente castellano
* Contenido .: int letras()
*---------------------------------------------------------------------------*/

#include <fgvsys.h>

#include <string.h>


extern double valsd();
extern long vals();

/*---------------------------------------------------------------------------
* Contenido .: int letras(resultado,numero,sexo)
*   Entradas:
*    resultado   puntero a char donde se alojara el string de resultado
*    numero      valor (double) de entrada para ser convertido a string
*    sexo        int de control para determinar el genero del numero
*                0 = femenino , 1 = masculino
*
*    Ej.:  letras(resultado,10010L,0)
*          resultado = DIEZ MIL DIEZ
*
*/



int letras(char *resultado,double numero,int sexo,int euro,int _decimales)
/* sexo; 1 es numero en masculino  0 es numero en femenino */
{
   static char *unidades[9]={"UN","DOS","TRES","CUATRO","CINCO","SEIS","SIETE","OCHO","NUEVE"};
   static char *dunidades[9]={"ONCE","DOCE","TRECE","CATORCE","QUINCE","DIECISEIS","DIECISIETE","DIECIOCHO","DIECINUEVE"};
   static char *decenas[9]={"DIEZ","VEINTE","TREINTA","CUARENTA","CINCUENTA","SESENTA","SETENTA","OCHENTA","NOVENTA"};
   static char *centenas[9]={"CIEN","DOSCIENT","TRESCIENT","CUATROCIENT","QUINIENT","SEISCIENT","SETECIENT","OCHOCIENT","NOVECIENT"};
   static char pila[1024];
   static int bloques = 0;
   static int control = 0;
   static int longi = 0;
   char cifra[256];
   char nn1[256];
   char nn2[256];
   char c1[20],c2[20],c3[20];
   char c[60];
   char temporal;
   int c0,si,ps,grupo,signo;
   int losdecimales;
   int ponceros = 0;
   char *numeros[2];
   int nnum,ki;
   int puestoeuro = 0;

   if (resultado[0])
   {
	   char *pp = resultado;
	   while(*pp)
	   {
		   if (*pp == ',')
			   *pp = '.';
		   pp++;
	   }
	   if (!_decimales && decimales(resultado) > 0)
		   _decimales = decimales(resultado);
   }

   strcpy(resultado,"");
   numeros[0] = nn1;
   numeros[1] = nn2;   
   if (sexo > 1) 
   {
      if (sexo < 7) 
	  {
         longi = (sexo / 2)*15;
         longi += 15;
         sexo %= 2;
      }
      if (control > 0) 
	  {
         strcpy(resultado,pila+(control * (longi + 1)));
         if (bloques >= control) 
		 {
            control = 0;
            bloques = 0;
         }
         else
            control++;
         return(0);
      }
      if (sexo == 7) return(0);
   }
   else 
   {
      longi = 0;
      control = 0;
      bloques = 0;
   }   

   if (euro)
	   sexo = 1;   

   sprintf(cifra,"%lf",numero);
   quitadec(cifra);
   losdecimales = decimales(cifra);   	    

   if (!losdecimales)
   {
	   nnum = 1;	   
	   sprintf(numeros[0],"%.0lf",rwatof(cifra));
   }
   else
   {
	   char *pp = cifra;
	   int nd;

	   losdecimales = _decimales;
	   nnum = 2;
	   while (*pp && *pp != '.' && *pp != ',') pp++;	   
	   if (*pp)
	   {
		   *pp = 0;
		   pp++;
	   }
	   nd = strlen(pp);
	   while(nd < losdecimales)
	   {
		   pp[nd] = '0';
		   nd++;
	   }
	   pp[nd] = 0;
	   strcpy(numeros[0],pp);
	   strcpy(numeros[1],cifra);
   }

   for (ki = 0;ki < nnum;ki++)
   {

	   numero = rwatof(numeros[ki]);
	   if (nnum == 1 || ki == 1)
	   {
		   if ( numero<0.0 )
			  signo=1;
		   else
			  signo=0;
	   }
	   else
		   signo = 0;
	   if (numero < 0.0)
	   {
		   numero = -numero;
		   sprintf(cifra,"%.0lf",numero);
	   }
	   else
	   {	   
		   char *pp = numeros[ki];	       
		   if (_decimales || nnum == 1 || ki == 1)
		   {
			  while(*pp && *pp == '0') pp++;
		   }
		   if (*pp)
		      strcpy(cifra,pp);
		   else
			  strcpy(cifra,"0");
	   }

	   for (ponceros = 0;cifra[ponceros] == '0';ponceros++);
	   if (ponceros)
	   {
		   strcpy(cifra,cifra+ponceros);
	   }
   
	   /* invierto el orden del string cifra en bloques de tres para su posterior
	   manejo ,anadiendo 0 delante si es necesario para tener los bloques 3    */
   
	   si=strlen(cifra);
	   c0=0;
	   if (( si % 3 )>0)
		  c0=3-si % 3;
	   for(ps=si;ps>=0;ps--)
		  cifra[ps+c0]=cifra[ps];
	   si += c0;
	   for(ps=0;ps<c0;ps++)
		  cifra[ps]='0';
	   for(c0=0;c0<si/6;c0++) {
		  for(ps=0;ps<3;ps++) {
			 temporal=cifra[c0*3+ps];
			 cifra[c0*3+ps]=cifra[si-(c0+1)*3+ps];
			 cifra[si-(c0+1)*3+ps]=temporal;
		  }
	   }
	   for(grupo=1;grupo<=si/3;grupo++) 
	   {      
		  c1[0]='\0';
		  c2[0]='\0';
		  c3[0]='\0';
		  c[0]='\0';
		  while(vals(cifra,(grupo-1)*3,3)>0) {
         
			 /* centenas  */
			 c0=cifra[grupo*3-3]-49;
			 if (c0>-1)
				strcpy(c1,centenas[c0]);
			 if ( c0>0 ) {
				if ( sexo==1 || grupo>2 )
				   strcat(c1,"OS");
				else
				   strcat(c1,"AS");
			 }
			 if ( vals(cifra,grupo*3-2,2)>0 ) {
				if ( c0==0 )
				   strcat(c1,"TO");
			 }
			 else
				break;
         
			 /* decenas */
			 c0=cifra[grupo*3-2]-49;
			 if ( c0>-1 )
				strcpy(c2,decenas[c0]);
			 if ( vals(cifra,grupo*3-1,1)>0 ) {
				switch (c0) {
				case -1 :
				   break;
				case 0  :
				   c0=cifra[grupo*3-1]-49;
				   strcpy(c2,dunidades[c0]);
				   c0=-2;
				   break;
				case 1  :
				   strcpy(c2,"VEINTI");
				   break;
				default :
				   strcat(c2," Y ");
				}
			 }
         
			 /*  unidades  */
			 if ( c0 != -2 ) {
				c0=cifra[grupo*3-1]-49;
				if (c0>-1)
				   strcpy(c3,unidades[c0]);
				if ( c0==0 ) {
				   if (grupo==1) 
				   {
					  if (sexo==1)
					  {
						 if (!euro)
						 {
							strcat(c3,"O");
						 }
					  }
					  else
						 strcat(c3,"A");
				   }
				   if ( (grupo % 2)==0 && vals(cifra,grupo*3-3,3)==1 )
					  c3[0]='\0';
				}
			 }
			 break;
         
		  }  /* fin del bucle centenas - decenas - unidades */
		  if ( strlen(c1) > 0 )
			 strcat(c1," ");
		  strcpy(c,c1);
		  strcat(c,c2);
		  strcat(c,c3);
		  if ( grupo % 2 != 0 ) {
			 switch (grupo) {
			 case 1 :
				break;
			 case 3 :
				if ( vals(cifra,grupo*3,3)*100+vals(cifra,grupo*3-3,3) >0 ) {
				   strcat(c," MILLON");
				   if ( vals(cifra,grupo*3,3)*100+vals(cifra,grupo*3-3,3) >1 )
					  strcat(c,"ES ");
				   else
					  strcat(c," ");
				}
				break;
			 case 5 :
				strcat(c," BILLON");
				if ( vals(cifra,grupo*3,3)*100+vals(cifra,grupo*3-3,3) )
				   strcat(c,"ES ");
				else
				   strcat(c," ");
				break;
			 default :
				strcpy(c," ");
			 }
		  }
		  else {
			 if ( vals(cifra,grupo*3-3,3)>0 )
				strcat(c," MIL ");
		  }
		  if ((strlen(resultado) + strlen(c)) < 255) strcatd(resultado,c);
	   } /*  aqui se acaba el for de grupo */

   
	   if (signo == 1 && strlen(resultado) < 248) strcatd(resultado,"MENOS ");
	   if (strlen(resultado) == 0)
		  strcpy(resultado,"CERO");
	   else
	   {
		  while(ponceros > 0)
		  {
			  if (resultado[0] != ' ')
				 strcatd(resultado,"CERO ");
			  else
				 strcatd(resultado,"CERO");
			  ponceros--;
		  }
	   }
	   if (!ki && nnum == 2 && strlen(resultado) < 240)
	   {
		  if (euro)
		  {
			  int nn = strlen(resultado);
			  if (nn) nn--;
			  if (resultado[nn] != ' ')
				  strcat(resultado," ");
			  strcat(resultado,"CENTS.");
			  if (resultado[0] != ' ')
				 strcatd(resultado," EUROS ");
			  else
				 strcatd(resultado," EUROS");
			  puestoeuro = 1;
		  }
		  else
		  {
			  if (resultado[0] != ' ')
			     strcatd(resultado," COMA ");
			  else
				 strcatd(resultado," COMA");
		  }
	   }
   }

   if (euro && !puestoeuro)
   {
	   int nn = strlen(resultado);
	   if (nn) nn--;
	   if (resultado[nn] != ' ')
		  strcat(resultado," ");
	   strcat(resultado,"EUROS");
   }
   
   if (longi > 0) 
   {
      control = 1;
      if ((c0 = strlen(resultado)) < longi) 
	  {
         strcat(resultado," ");
         for (;longi > strlen(resultado);strcat(resultado,"-"));
         bloques = 1;
         for (c0 = 0;c0 < longi;c0++)
            strcat(resultado,"-");
      }
      else 
	  {
         bloques = 0;
         while (-1) {
            bloques++;
            for (c0 = longi * bloques;*(resultado+c0-1) != ' ';c0--);
            for (;c0 <= longi * bloques;c0++) {
               if (strlen(resultado) == 254) return(0);
               strcatd(resultado+c0," ");
            }
            if ( strlen(resultado) < (longi * (bloques+1)) ) break;
         }
         strcat(resultado," ");
         for(;strlen(resultado) < (longi * (bloques+1));strcat(resultado,"-"));
      }
   

      strcpy(pila,resultado);
      for (c0 = 1;c0 <= bloques;c0++)
         *(pila + (longi*c0)) = '\0';
      strcpy(resultado,pila);
   }			
   return(0);
}

