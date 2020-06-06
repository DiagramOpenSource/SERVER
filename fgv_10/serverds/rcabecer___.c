/*---------------------------------------------------------------------------
* Programa ..: rcabecer.c
* Fecha .....:
* Version ...:
* Notas .....: Rutina de cabecera en pantalla
* Contenido .: int  cabecera(),
*---------------------------------------------------------------------------*/

#include <fgvsys.h>
#ifndef FGVWIN

#include <stdio.h>
#include <rtermin.h>
#include <malloc.h>
#include <time.h>
#include <sys/timeb.h>
#include <rwmalloc.h>

#ifdef FGV32
#ifndef WATCOMNLM
extern HANDLE myconsola;
extern HANDLE hStdIn;
#endif
#endif

#ifdef RWXARXA
#ifdef RWSERVER
extern int en_conexion_externa;
extern int en_conexion_externa_stop;
#endif
#endif


extern int fecha_protegida;
extern int linea_25;
extern int duplicarusuario;

extern int modo_desatendido;

char *cabeza_especial = (char *)0;
int *rw_extranumuser = NULL;
char *rw_usuario = NULL;

static int posi_lado[3] = {201,222,260};
static char *mensa_en_cabeza[100][3];
static int recur_mensa = 0;

static int __mensaje_persistente = 0;

int mensaje_persistente(str,tiempo)
char *str;
int tiempo;
{
   Bl(25);
   pprint(2501,str);
   pito();
   __mensaje_persistente = time(NULL) + tiempo;
   return(0);
}

int recurre_mensaje_cabecera(m)
int m;
{
   int lado;
   int hay;
   
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      recurre_mensaje_cab_terminal(m);
   }
   en_conexion_externa_stop++;
#endif
#endif
   
   if (!m)
   {
      if (recur_mensa < 100)
      {
         hay = 0;
         for (lado = 0;lado < 3;lado++)
         {
            if (mensa_en_cabeza[recur_mensa][lado])
            {
               Ar();
               bi_cuadro(posi_lado[lado],posi_lado[lado]+strlen(mensa_en_cabeza[recur_mensa][lado])-1,1);
               Cr();
               hay = 1;
            }
         }
         if (hay || recur_mensa)
            recur_mensa++;
      }
   }
   else
   {
      if (recur_mensa)
      {
         recur_mensa--;
         for (lado = 0;lado < 3;lado++)
         {
            if (mensa_en_cabeza[recur_mensa][lado])
            {
               Ar();
               pprint(posi_lado[lado],mensa_en_cabeza[recur_mensa][lado]);
               Cr();
            }
         }
      }
   }
#ifdef RWXARXA
#ifdef RWSERVER
   en_conexion_externa_stop--;
#endif
#endif
}

borra_mensaje_cabecera(l)
int l;
{
   int lado;
   
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      borra_mensaje_cab_terminal(l);
   }
   en_conexion_externa_stop++;
#endif
#endif
   
   
   for (lado = 0;lado < 3;lado++)
   {
      if (l != -1 && l != lado) continue;
      if (mensa_en_cabeza[recur_mensa][lado])
      {
         Ar();
         bi_cuadro(posi_lado[lado],posi_lado[lado]+strlen(mensa_en_cabeza[recur_mensa][lado])-1,1);
         Cr();
         free(mensa_en_cabeza[recur_mensa][lado]);
         mensa_en_cabeza[recur_mensa][lado] = NULL;
      }
   }
#ifdef RWXARXA
#ifdef RWSERVER
   en_conexion_externa_stop--;
#endif
#endif
}

mensaje_en_cabecera(usi,men,lado)
char *usi,*men;
int lado;
{
   char mensa[81];
   
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      mensaje_en_cab_terminal(usi,men,lado);
   }
   en_conexion_externa_stop++;
#endif
#endif
   
   
   sprintf(mensa,usi,men);
   if (!mensa_en_cabeza[recur_mensa][lado] || strlen(mensa_en_cabeza[recur_mensa][lado]) != strlen(mensa))
   {
      borra_mensaje_cabecera(lado);  
      mensa_en_cabeza[recur_mensa][lado] = malloc(strlen(mensa)+1);
   }
   strcpy(mensa_en_cabeza[recur_mensa][lado],mensa);
   
   Ar();
   pprint(posi_lado[lado],mensa);
   Cr();
#ifndef ESDOS
   fflush(stdout);
#endif
#ifdef RWXARXA
#ifdef RWSERVER
   en_conexion_externa_stop--;
#endif
#endif
}


/* cabecera  pinta la cabecera de pantalla con
*           proceso         titulo              empresa
*           en la linea 2 recuadrado (101,380)
*/
int cabecera(proceso,titulo,empresa)
char *proceso,*titulo,*empresa;
{
   return(cabecera_v((char *)0,proceso,titulo,empresa));
}


int cabecera_v(version,proceso,titulo,empresa)
char *version,*proceso,*titulo,*empresa;
{
   int i,d,ct,ce,dp,cp;
   char usi[30];
   char buf[50];
   char tmp[250];
   
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      cabecera_v_terminal(version,proceso,titulo,empresa);
   }
   en_conexion_externa_stop++;
#endif
#endif

   if (modo_desatendido) return(0);

   
   cp = strlen(proceso);if (cp > 31) cp = 31;
   ct = strlen(titulo); if (ct > 31) ct = 31;
   i = (75 - cp - ct);
   ce = strlen(empresa);if (ce > i) ce = i;
   if (ce > 31) ce = 31;
   
   i = (78  - ct) / 2;
   if (i < cp)
      dp = cp;
   else {
      if (i < ce) i -= (ce - i);
      dp = i;
   }
   
   cursor(101);
   Ar();
   sprintf(usi," %%-%d.%ds ",dp,cp);
   v_prints(usi,proceso);
   sprintf(usi,"%%-%d.%ds ",ct,ct);
   v_prints(usi,titulo);
   dp = 76 - dp - ct;
   sprintf(usi,"%%%d.%ds ",dp,ce);
   v_prints(usi,empresa);
   if (cabeza_especial) {
      pprint(atoi(cabeza_especial),cabeza_especial+5);
   }
   bi_cuadro(201,260,1);
   if (fecha_protegida){
      cursor(210);
      Cr();
      Ap();
      v_prints("[%-17.17s]","MODO DEMOSTRACION");
      Cp();
      Ar();
   }
   if (rw_usuario) {
      rw_usuario[10] = 0;
      sprintf(buf,"[Usr:%s",rw_usuario);
      if (duplicarusuario && rw_extranumuser)
      {
         sprintf(buf+strlen(buf),"(%d)",(*rw_extranumuser)+1);
      }
      cursor(261);
      v_prints("%-19.19s]",buf);
   }
   else
      bi_cuadro(261,280,1);
   cursor(301);
   v_prints("%80s","");
   cursor(401);
   v_prints("%80s","");
   Cr();
   if (version != (char *)0) 
   {
      if (!__mensaje_persistente || __mensaje_persistente < time(NULL))
      {
         __mensaje_persistente = 0;
         Ai();
         if (linea_25)
            pprint(2501,version);
         else
            pprint(2401,version);
         Bi();	 
      }
   }
#ifdef FGV32
#ifndef WATCOMNLM
   if (myconsola != INVALID_HANDLE_VALUE)
   {
      if (version)
      {
         if (!proceso[0] && !titulo[0] && !empresa[0])
         {
            sprintf(tmp,"%s",version);
         }
         else
         {
            sprintf(tmp,"%s %s %s",proceso,empresa,version);
         }
      }
      else
      {
         sprintf(tmp,"%s %s",proceso,empresa);
      }
      tmp[129] = 0;
      SetConsoleTitle(tmp);
   }
#endif
#endif
   fflush(stdout);
#ifdef RWXARXA
#ifdef RWSERVER
   en_conexion_externa_stop--;
#endif
#endif
   return(0);
}


#endif

