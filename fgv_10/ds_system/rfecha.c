/*---------------------------------------------------------------------------
* Programa ..: rfecha.c                  Autor: Francisco Gonzalez Valiente
*                          Algoritmo ideado por Francisco Gonzalez Valiente
* Fecha .....:
* Version ...:
* Notas .....: Rutina de obtenion de la fecha del dia.
* Contenido .: char *fecha(),
*              int  bienfecha(),
*              int fechafor(),
*              int bienfechatot(),
*              double diasfecha(),
*              int fechadias()
*---------------------------------------------------------------------------*/

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
#ifndef LARGESEEK
extern long rw_lseek_();
#endif
extern unsigned int rw_read_();

#endif

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/timeb.h>


int fecha_protegida = 0;
char fechas[75];

static char *diasem[]={
   "Domingo",
      "Lunes",
      "Martes",
      "Miercoles",
      "Jueves",
      "Viernes",
      "Sabado",
};
static char *mes[]={
   " ",
      "Enero",
      "Febrero",
      "Marzo",
      "Abril",
      "Mayo",
      "Junio",
      "Julio",
      "Agosto",
      "Septiembre",
      "Octubre",
      "Noviembre",
      "Diciembre",
};

static int diames[13]={
   0,31,28,31,30,31,30,31,31,30,31,30,31
};      


double diasfecha(f,diasm,meses,anos)
char *f;
int diasm,meses,anos;
{
   int ano,periodo;
   double dias;
   
   
   if ( (dias = (double)bienfechatot(f,meses,anos,0,0)) < (double)0)
      return((double)-1);
   
   ano = atoi(f+6);
   periodo = (ano % 4);if (periodo == 0) periodo = 4;
   dias += (double)(periodo * 365);
   ano -= periodo;
   periodo = (ano % 100);
   dias += (double)(periodo * 365.25);
   ano -= periodo;
   periodo = (ano % 400);
   dias += (double)(periodo * 365.24);
   ano -= periodo;
   dias += (double)(ano * 365.2425);
   dias += (double)diasm;
   if (diasm != 0) 
   {
      strcpy(f,"00.00.0000"); /* el computo es desde el PRINCIPIO */ 
      fechadias(f,dias);
   }
   return(dias);
}



/* fecha devuelve un puntero a una cadena de strings de la forma
* dd.mm.aaaa_hh:mm:ss_Miercoles_dd de Septiembre de aaaa_
* 012345678901234567890123456789012345678901234567890123456789
* 20.07.1987 13:10:50 Lunes     20 de Julio de 1987
* el underscore indica el caracter nulo de final de string que se
* intercala al final de cada parte,
* si solo queremos la hora usaremos fecha()+11
* si queremos el dia de la semana fecha()+20
* se utiliza la variable externa fechas :
* fechas    : Fecha en numeros
* fechas+11 : Hora
* fechas+20 : Dia de la semana en letras
* fechas+30 : Fecha en letras
*/
int fechafor(p)
char *p;
{
   int dia,meses,ano;
   char s[12];
   int dias = (int)(((long)diasfecha(p,0,0,0) - 1) % 7);
   
   strcpy(s,p);
   s[2]='\0';
   s[5]='\0';
   dia = atoi(s);
   meses = atoi(s+3);
   ano = atoi(s+6);
   
   
   sprintf(p+20,"%s%c",
      diasem[dias] ,
      '\0');
   sprintf(p+30,"%02d de %s de %04d%c",
      dia ,
      mes[meses] ,
      ano,
      '\0');
   sprintf(p+60,"%s%c",
      mes[meses] ,
      '\0');
   return(0);
}

char *fecha()
{
		 time_t a; 
         char *p;
         struct tm *lc;
         int ann;
         
         p = fechas;
         a = time(NULL);
         if (a == -1L) {
            memset(fechas,0,sizeof(fechas));
            return(p);
         }
         
         /* ------
         * seg.transcurridos  *b
         * segundos           localtime(b)->tm_sec
         * minutos            localtime(b)->tm_min 
         * hora               localtime(b)->tm_hour 
         * dia del mes        localtime(b)->tm_mday 
         * mes                (localtime(b)->tm_mon)+1 
         * ano -1900          localtime(b)->tm_year
         * dia de la semana   localtime(b)->tm_wday 
         * dia del ano        localtime(b)->tm_yday 
         * activado DST       localtime(b)->tm_isdst 
         */ 
         lc = localtime(&a);
         if (!lc) 
         {
            memset(fechas,0,sizeof(fechas));
            return(p);
         }
         
         ann = lc->tm_year;
         if (ann > 50)
            ann += 1900;
         else
            ann += 2000;
         
         sprintf(p,"%02d%c%02d%c%04d%c",
            lc->tm_mday ,caracter_de_fecha,
            (lc->tm_mon)+1 ,caracter_de_fecha,
            ann,
            '\0');
         sprintf(p+11,"%02d:%02d:%02d%c",
            lc->tm_hour ,
            lc->tm_min  ,
            lc->tm_sec  ,
            '\0');
         fechafor(p);
         return(p);
}


int minimo_fecha(qd,x)
char **qd;
int x;
{
			fecha();
         if (!qd) return(0);
         if (x == 6)
         {
            memcpy(fechas+2,fechas+3,7);
            memcpy(fechas+4,fechas+7,2);
            fechas[6] = 0;
         }
         else
            if (x == 8)
            {
               memcpy(fechas+6,fechas+8,2);
               fechas[8] = 0;
            }
            *qd = fechas;
            return(0);
}


int es_bisi(ano)
int ano;
{
   if ((ano%4 == 0) && (ano%100 != 0) || (ano%400 == 0))
      return(1);
   return(0);
}

/* bienfecha devuelve -1 si la fecha no es correcta
* en caso contrario devuelve el numero de dia del anyo
* se le pasa el string de la forma dd.mm.aaaa
*/
int bienfecha(f)
char *f;
{
   return(bienfechatot(f,0,0,1,0));
}

int bienfecha_x(f,x)
char *f;
int x;
{
   return(bienfechatot(f,0,0,1,x));
}


int desarregla_fecha(f,x)
char *f;
int x;
{
   if (x == 6)
   {
      memcpy(f+2,f+3,7);
      memcpy(f+4,f+7,2);
      fechas[6] = 0;
   }
   else
      if (x == 8)
      {
									memcpy(f+6,f+8,2);
                           fechas[8] = 0;
      }
      return(0);
}

int arregla_fecha(f,x)
char *f;
int x;
{
   char s[12];
   int dia;
   int mes;
   int ano;
   int kk;
   
   strcpy(s,f);
   if (x == 6)
   {
      memcpy(s,f,2);
      s[2]='\0';  
      dia = atoi(s);
      memcpy(s,f+2,2);
      s[2]='\0';  
      mes = atoi(s);
      memcpy(s,f+4,2);
      s[2]='\0';  
      kk = atoi(s);
      if (kk < 50)    
         kk = 2000;    
      else
         kk = 1900;
      ano = (kk+atoi(s));
      sprintf(f,"%02d%c%02d%c%04d",dia,caracter_de_fecha,mes,caracter_de_fecha,ano);
   }
   else
      if (x == 8)
      {
         s[2]='\0';
         s[5]='\0';
         dia = atoi(s);
         mes = atoi(s+3);
         kk = atoi(s+6);
         if (kk < 50)    
            kk = 2000;    
         else
            kk = 1900;
         ano = (kk+atoi(s+6));
         sprintf(f,"%02d%c%02d%c%04d",dia,caracter_de_fecha,mes,caracter_de_fecha,ano);
      }
      return(0);
}


int bienfechatot(f,mes,ano,modo,x)
char *f;
int mes;
int ano;
int modo;
int x;
{
   char s[12];
   int dia,bisiesto;
   int i = 0,c = 0;
   int kk;
   
   strcpy(s,f);
   if (x == 6)
   {
      memcpy(s,f,2);
      s[2]='\0';  
      dia = atoi(s);
      memcpy(s,f+2,2);
      s[2]='\0';  
      mes += atoi(s);
      memcpy(s,f+4,2);
      s[2]='\0';  
      kk = atoi(s);
      if (kk < 50)    
         kk = 2000;    
      else
         kk = 1900;
      ano += (kk+atoi(s));
   }
   else
      if (x == 8)
      {
         s[2]='\0';
         s[5]='\0';
         dia = atoi(s);
         mes += atoi(s+3);
         kk = atoi(s+6);
         if (kk < 50)    
												kk = 2000;    
         else
												kk = 1900;
         ano += (kk+atoi(s+6));
      }
      else
      {
         s[2]='\0';
         s[5]='\0';
         dia = atoi(s);
         mes += atoi(s+3);
         ano += atoi(s+6);
      }
      
      if (!modo) {
         while (mes < 1) {
												ano -= 1;
                                    mes += 12;
                                    c = 1;
         }
         while (mes > 12) {
												ano += 1;
                                    mes -= 12;
                                    c = 1;
         }
      }
      
      if ((bisiesto = es_bisi(ano)) == 1 && mes == 2) i = 1;
      
      if (mes != 0 || dia != 0 || ano != 0) { /* permitir fecha nula */
         if ((mes < 1) || (mes > 12) || (modo && fecha_protegida && mes > 2))
												return(-1);
         if (ano < 0)
												return(-1);
         if (dia < 1)
												return(-1);
         if ( dia > (diames[mes] + i) ) {
												
												if (modo) return(-1);
                                    
                                    dia = (diames[mes] + i);
                                    c = 1;
         }
      }   
      
      if (x == 6)
         sprintf(f,"%02d%02d%02d",dia,mes,ano%100);
      else
         if (x == 8)   
												sprintf(f,"%02d%c%02d%c%02d",dia,caracter_de_fecha,mes,caracter_de_fecha,ano%100);
         else
												sprintf(f,"%02d%c%02d%c%04d",dia,caracter_de_fecha,mes,caracter_de_fecha,ano);
         
         for (i=1;i<mes;i++)
												dia += diames[i];
         if (mes > 2) dia += bisiesto;
         
         
         return(dia);
}

extern long vals();

int fechadias(f,dias)
char *f;
double dias;
{
   int c400 = 0 ,c100 = 0,c4 = 0,anos = 0,meses = 0,bisiesto = 0;
   int cont = 0;
   if (dias < (double)0) return(-1);
   
   if (dias > 366.0) {
      c400 = (int)((dias - 366) / 146097L) ;
      dias -= ((double)c400 * 146097L);
   }
   if (dias > 365.0) {
      c100 = (int)((dias - 365) / 36524L);
      dias -= ((double)c100 * (double)36524.0);
   }
   if (dias > 366.0) {
      c4 = (int)((dias - 366) / 1461);
      dias -= ((double)c4 * (double)1461);
   }
   if (c100 < 4 && dias > 1.0) {
      anos = (int)((dias - 1)/ 365);
      if (anos == 5) anos--;
      dias -= ((double)anos * (double)365);
   }
   
   anos += (c400 * 400) + (c100 * 100) + (c4 * 4);
   bisiesto = es_bisi(anos);
   
   meses = 1;
   do {
      c4 = diames[meses];
      if (meses == 2) c4 += bisiesto;
      if (dias > c4)  {
         dias -= (double)c4;
         meses++;
      }
      else
         break;
   } while(-1);
   
   if ( (c100 = (int)vals(f,0,2)) <= (int)dias)
      dias -= (double)c100;
   else {
      dias = (double)(c4 - c100) + dias;
      meses--;
   }
   
   if ( (c100 = (int)vals(f,3,2)) <= meses)
      meses -= c100;
   else {
      meses = (12 - c100) + meses;
      anos--;
   }
   
   if ( (c100 = atoi(f+6)) < anos)
      anos -= c100;
   else
      anos = c100 - anos;
   
   sprintf(f,"%02d%c%02d%c%04d",((int)dias),caracter_de_fecha,meses,caracter_de_fecha,anos);
   return(0);
}



