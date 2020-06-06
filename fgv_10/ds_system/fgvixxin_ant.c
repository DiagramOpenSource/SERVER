
#include <fgvsys.h>
/* INDEXADO RW */



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
#include <malloc.h>
#include <rwmalloc.h>
#include <memory.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

#include <rtermin.h>
#define _NUEVODEF
#include <rdef.h>      /* ya incluye el isam.h */

#include "fgvixx.h"


static long f_registro[IXXMAXFILES];
static short modos_locales[IXXMAXFILES];
extern int fecha_protegida;
long max_prote_reg = 25L;
static int in_fin_trans = 0;
extern int mddebug;
extern char *cabeza_especial;


extern int con_respaldo_externo;

extern int issingleuser = 0;
extern int trans_save_block = 0;
extern int auto_trans = 0;
extern int translevel = 0;
extern int no_read_error;

extern int defecto_modo_datos;

extern char *definicion_tipo_datos;

static char **tabla_tipo_datos = NULL;

extern char *quita_path();

static int llama_a_odbc = 0;

static int primerisimo = 1;


#ifdef RWXARXA
extern int _en_conexion_server_;
#endif

static int fd_join[IXXMAXFILES];

int cierra_join_(fd)
int fd;
{
   if (fd_join[fd] != -1)
   {
      int otrofd = fd_join[fd];
      fd_join[fd] = -1;    
#ifdef FGVDOS
      if (otrofd >= IXXMAXFILES)
         sqlsrv__cierra_ind(cierra_join_(otrofd));
      else
#endif
         ixx_cierra_indice(cierra_join_(otrofd));   
   }
   return(fd);
}

int establece_join_(fdo,fdj)
int fdo,fdj;
{
   int i;
   if (fd_join[fdo] != -1) return(-1);
   i = fdj;
   while(fd_join[i] != -1)
			{
      if (fd_join[i] == fdo)
      {
         return(-2);
      }
      i = fd_join[i];
			}
   fd_join[fdo] = fdj;
   return(0);
}

static int inde_aviso(m)
char *m;
{
	aviso(0,m);
    return(0);
}

static int reinde_espera(m)
int m;
{   
   static int modo = 0;
   
   if (!m)
   {
      if (!modo)
      {
        modo = 1;
        if (DespachadorExterno)
        {
             char tmp[512];
             int *p1 = (int *)tmp;
             char *p = tmp+sizeof(int)*2;
             sprintf(p,"%-20.20s %s","","SE ESTA RECONSTRUYENDO EL INDICE\r\nESPERE, POR FAVOR");
             p1[0] = sizeof(int)*2 + strlen(p) + 2;     
             p1[1] = 0;
             (*DespachadorExterno)((int)DESPACHO_MAINCOMMAND,ID_DLGINDICES,(void *)tmp);
        }
      }
      return(0);
   }
   if (!modo) return(0);
   modo = 0;
   if (DespachadorExterno)
   {       
      char tmp[512];
      int *p1 = (int *)tmp;
      char *p = tmp+sizeof(int)*2;
      *p = 0;         
      p1[0] = sizeof(int)*2 + strlen(p) + 2;     
      p1[1] = 99;
      (*DespachadorExterno)((int)DESPACHO_MAINCOMMAND,ID_DLGINDICES,(void *)tmp);
   }
   return(0);
}

static int reinde_aviso(m,reg,greg)
int m;
long reg,greg;
{
   char tmp[256];
   int i;
   char *p;
   long tim;
   static VENT *w;
   static long tiempo;
   static long tiem;
   static long previs;
   static long total;
   
   if (m == 1)
   {
#ifdef FGVDOS
	  ltoa(reg,tmp,10);
#else
      sprintf(tmp,"%ld",reg);
#endif
      if (DespachadorExterno)
      {
         char _tmp[512];
         int *p1 = (int *)_tmp;
         char *_p = _tmp+sizeof(int)*2;
         sprintf(_p,"Registro actual:%s",tmp);
         p1[0] = sizeof(int)*2 + strlen(_p) + 2;     
         p1[1] = 1;
         (*DespachadorExterno)((int)DESPACHO_MAINCOMMAND,ID_DLGINDICES,(void *)_tmp);
      }      
      greg--;
   }
   else
   if (!m)
   {
  	    p = (char *)greg;
        if (DespachadorExterno)
        {
             char _tmp[512];
             int *p1 = (int *)_tmp;
             char *_p = _tmp+sizeof(int)*2;
             sprintf(_p,"%-20.20s INDEXACION de %s\r\nTotal posibles registros:%7ld Incluyendo ya borrados","Registro actual:",p,reg);
             p1[0] = sizeof(int)*2 + strlen(_p) + 2;     
             p1[1] = 0;
             (*DespachadorExterno)((int)DESPACHO_MAINCOMMAND,ID_DLGINDICES,(void *)_tmp);
        }
        tiempo = time(NULL);
        tiem = 0L;
        previs = ((reg / 10) / 60)+1;
        total = reg;
   }
   else
   if (m == 2)
   {
       if (DespachadorExterno)
       {       
          char _tmp[512];
          int *p1 = (int *)_tmp;
          char *_p = _tmp+sizeof(int)*2;
          *_p = 0;         
          p1[0] = sizeof(int)*2 + strlen(_p) + 2;     
          p1[1] = 99;
          (*DespachadorExterno)((int)DESPACHO_MAINCOMMAND,ID_DLGINDICES,(void *)_tmp);
       }
   }
   return(1);
}


static int pasadat_aviso(m,reg,p)
int m;
long reg;
char *p;
{
   char tmp[256];
   int i;
   long tim;
   static VENT *w;
   static long tiempo;
   static long tiem;
   static long previs;
   static long total;
   
   if (m == 1)
   {
#ifdef FGVDOS
      ltoa(reg,tmp,10);
#else
      sprintf(tmp,"%ld",reg);
#endif     
      
      if (DespachadorExterno)
      {
         char _tmp[512];
         int *p1 = (int *)_tmp;
         char *_p = _tmp+sizeof(int)*2;
         sprintf(_p,"Registro actual:%s",tmp);
         p1[0] = sizeof(int)*2 + strlen(_p) + 2;     
         p1[1] = 1;
         (*DespachadorExterno)((int)DESPACHO_MAINCOMMAND,ID_DLGINDICES,(void *)_tmp);
      }      
      
   }
   else
      if (!m)
      {
        if (DespachadorExterno)
        {
             char _tmp[512];
             int *p1 = (int *)_tmp;
             char *_p = _tmp+sizeof(int)*2;
             sprintf(_p,"%-20.20s PASE DE DATOS de %s\r\nTotal registros:%7ld","Registro actual:",p,reg);
             p1[0] = sizeof(int)*2 + strlen(_p) + 2;     
             p1[1] = 0;
             (*DespachadorExterno)((int)DESPACHO_MAINCOMMAND,ID_DLGINDICES,(void *)_tmp);
        }
         tiempo = time(NULL);
         tiem = 0L;
         previs = ((reg / 10) / 60)+1;
         total = reg;
      }
      else
         if (m == 2)
         {
               if (DespachadorExterno)
               {       
                  char _tmp[512];
                  int *p1 = (int *)_tmp;
                  char *_p = _tmp+sizeof(int)*2;
                  *_p = 0;         
                  p1[0] = sizeof(int)*2 + strlen(_p) + 2;     
                  p1[1] = 99;
                  (*DespachadorExterno)((int)DESPACHO_MAINCOMMAND,ID_DLGINDICES,(void *)_tmp);
               }
         }

         return(1);
}


extern int (*ixxmen_aviso)(char *) = inde_aviso;
extern int (*ixxind_aviso)(int,long,long) = reinde_aviso;
extern int (*ixxind_recespera)(int) = reinde_espera;
extern int (*ixxdat_aviso)(int,long,char *) = pasadat_aviso;

static libera_agitabla(dtabla)
FgvStdTabla *dtabla;
{
   if (!dtabla) return(-1);
   if (dtabla->Campo)
			{
      free(dtabla->Campo);
			}
   free(dtabla);
   return(0);
}

char **descrip_camps(pfi,defec,dtabla)
FI_DEF *pfi;
char **defec;
FgvStdTabla **dtabla;
{
			char tmp[MAXDEFECTO+1];
         char **descamp;
         char *defectos;
         char *oldbuf;
         int i,j;
         int tipo;
         short total = 0;
         short *p1;
         
         
         if (*dtabla)
            libera_agitabla(*dtabla);
         
         *dtabla = (FgvStdTabla *)malloc(sizeof(FgvStdTabla));
         memset(*dtabla,0,sizeof(FgvStdTabla));   
         
         (*dtabla)->reclen = pfi->f_lfich;
         (*dtabla)->ncampos = pfi->f_ncampos;       


         strcpy((*dtabla)->ident,quita_path(pfi->f_fichero));
         strcpy((*dtabla)->PathDef,pfi->f_PathDef);

         (*dtabla)->Campo = (CampoTabla *)malloc(sizeof(CampoTabla) * pfi->f_ncampos);
         memset((*dtabla)->Campo,0,sizeof(CampoTabla) * pfi->f_ncampos);
         
         for (i = 0;i < pfi->f_nclaves;i++)
         {
            total += pfi->f_clave[i].k_ncampos;
         }
         descamp = malloc(((pfi->f_ncampos+1) * sizeof(char *)) + ((total+1) * sizeof(short)));
         defectos = malloc(pfi->f_lfich+1);
         for (i = 0;i < pfi->f_ncampos;i++)
         {
            (*dtabla)->Campo[i].longitud = long_campo(pfi,i);
            (*dtabla)->Campo[i].decimales = deci_campo(pfi,i);
            switch(pfi->f_campos[i].c_tipo)
            {
            case INTTIPO:
               tipo = IXXCSHORT;
               break;
            case LONGTIPO:
               tipo = IXXCLONG;
               break;
            case FLOATTIPO:
               tipo = IXXCFLOAT;
               break;
            case DOUBLETIPO:
               tipo = IXXCDOUBLE;
               break;
            case FECHATIPO:
               tipo = IXXCALFA;
               (*dtabla)->Campo[i].longitud = pfi->f_campos[i].c_long;
               break;
            default:
               tipo = IXXCALFA;
               (*dtabla)->Campo[i].longitud = pfi->f_campos[i].c_long;
               break;
            }
            sprintf(tmp,"%1d %05d ",tipo,pfi->f_campos[i].c_long);
            if (pfi->f_campos[i].c_lista[0] == 13)
            {
               strcat(tmp,pfi->f_campos[i].c_lista+1);
            }
            else
               if (pfi->f_campos[i].c_lista[0])
               {
                  strcat(tmp,pfi->f_campos[i].c_lista);
               }
               else
                  if (pfi->f_campos[i].c_nombre)
                  {
                     strcat(tmp,pfi->f_campos[i].c_nombre);
                  }
                  descamp[i] = malloc(strlen(tmp)+1);
                  strcpy(descamp[i],tmp);
                  tmp[18] = 0;
                  strcpy((*dtabla)->Campo[i].ident,tmp+8);
                  (*dtabla)->Campo[i].tipo = pfi->f_campos[i].c_tipo + 48;
                  (*dtabla)->Campo[i].len = pfi->f_campos[i].c_long;  
         }
         descamp[i] = NULL;
         p1 = (short *)&(descamp[i+1]);
         *p1 = total;
         for (i = 0;i < pfi->f_nclaves;i++)
         {
            for (j = 0;j <  pfi->f_clave[i].k_ncampos;j++)
            {
               p1++;
               *p1 = (short)(pfi->f_clave[i].k_campo[j] - 1);
            }
         }
         oldbuf = pfi->f_buf;
         pfi->f_buf = defectos;
         defecto_ficha(pfi);
         pfi->f_buf = oldbuf;
         *defec = defectos;
         return(descamp);
}



void libera_des_tabla(FI_DEF *pfi,char **defectos,char ***_descamp,FgvStdTabla **dtabla)
{
   char **descamp = *_descamp;
   int i;
   free(*defectos);
   *defectos = NULL;
   for (i = 0;i < pfi->f_ncampos;i++)
   {
      free(descamp[i]);
   }
   free(descamp);
   *_descamp = NULL;
   libera_agitabla(*dtabla);
   *dtabla = NULL;
}


/* short tipo_datos; /* -1 defecto ... 0 = dat 1 = dbf interno 2 = dbf standard 3 = dbf foxpro 4 = odbc */
static int DeterminaLlamada(FI_DEF *pfi)
{
   
   if (pfi->tipo_datos < 0 || pfi->tipo_datos > 4)
      pfi->tipo_datos = defecto_modo_datos;
   
   switch (pfi->tipo_datos)
   {
   case 0:
						ixx_datos_dbf = 0;
                  default_dbf_id = 0;
                  llama_a_odbc = 0;
                  break;
   case 1:
						ixx_datos_dbf = 1;
                  default_dbf_id = 0;
                  llama_a_odbc = 0;
                  break;
   case 2:
						ixx_datos_dbf = 1;
                  default_dbf_id = 1;
                  llama_a_odbc = 0;
                  break;
   case 3:
						ixx_datos_dbf = 1;
                  default_dbf_id = 2;
                  llama_a_odbc = 0;
                  break;
   case 4:
						ixx_datos_dbf = 0;
                  default_dbf_id = 0;
                  llama_a_odbc = 1;
                  break;
   }
   return(llama_a_odbc);
}


static void CosasDelPrincipio()
{
   int i;
   
   primerisimo = 0;
   for (i = 0;i < IXXMAXFILES;i++)
   {
      fd_join[i] = -1;
   }
   if (definicion_tipo_datos)
   {
      FILE *f;	 
      f = fopen(definicion_tipo_datos,"r");
      if (f)
						{
         int total = 0;
         char texto[1024];
         while(lee_linea(texto,1000,f))
         {
            total++;
         }
         if (total)
         {
            rewind(f);
            i = 0;
            tabla_tipo_datos = (char **)malloc(sizeof(char *) * total + 1);
            while(i < total && lee_linea(texto,1000,f))
            {
               tabla_tipo_datos[i] = malloc(strlen(texto)+1);
               strcpy(tabla_tipo_datos[i],texto);
               i++;
            }
            tabla_tipo_datos[i] = NULL;					    
         }
         fclose(f);
						}
   } 
}

static void AjustaSegunMiTabla(FI_DEF *pfi)
{
   int i,pex;
   char *limpio,*otro,*busca,*ext;
   if (!tabla_tipo_datos) return;
   
   limpio = quita_path(pfi->f_fichero);
   for (i = 0;tabla_tipo_datos[i];i++)
   {
      otro = quita_path(tabla_tipo_datos[i]);
      if (otro == tabla_tipo_datos[i])
						{
         busca = limpio;
						}
      else
						{
         busca = pfi->f_fichero;
         otro = tabla_tipo_datos[i];
						}
      pex = strbusca(otro,".");
      if (pex >= 0)
						{
         char tmp[256];
         char tmp2[256];
         strcpy(tmp,otro);
         strcpy(tmp2,busca);
         tmp[pex] = 0;
         strminus(tmp);
         cambia_barra(tmp);
         strminus(tmp2);
         cambia_barra(tmp2);
         if (!strcmp(tmp,tmp2))
         {
            ext = otro + pex;
            if (!strcmp(ext,".dat"))
            {
               pfi->tipo_datos = 0;
            }
            else
               if (!strcmp(ext,".dbs"))
               {
                  pfi->tipo_datos = 1;
               }
               else
                  if (!strcmp(ext,".db3"))
                  {
                     pfi->tipo_datos = 2;
                  }
                  else
                     if (!strcmp(ext,".dbp"))
                     {
                        pfi->tipo_datos = 3;
                     }
                     else
                        if (!strcmp(ext,".odb"))
                        {
                           pfi->tipo_datos = 4;
                        }
                        return;
         }
						}
   }
}

/* crear un indexado nuevo */

crea_ind(pfi)
register FI_DEF *pfi;
{
   int fd,i,k,campo;
   char *nombre;
   IXXCLAVE *claves;
   short nclaves,lfich;
   short o,l,t;
   char tmp[81];
   char **descamp;
   char *defectos;
   FgvStdTabla *dtabla = NULL;
   
   
   if (primerisimo)
   {
      CosasDelPrincipio();
   }
   
   
   if (tabla_tipo_datos) AjustaSegunMiTabla(pfi);
   
   if (DeterminaLlamada(pfi))
   {
#ifdef FGVDOS
      return(sqlsrv_crea_ind(pfi));
#else
      return(-1);
#endif
   }
   
   AjustaPunterosSegunDatos(-1);
   
   nombre = pfi->f_fichero;             /* nombre del fichero sin extension */
   lfich  = pfi->f_lfich;               /* longitud de la ficha             */
   nclaves = pfi->f_nclaves;
   
   if (nclaves < 1) return(-1);
   
   claves = (IXXCLAVE *)malloc(sizeof(IXXCLAVE) * nclaves);
   memset(claves,0,sizeof(IXXCLAVE) * nclaves);
   for (i = 0;i < nclaves;i++)
   {
      if (!i)
         claves[i].clave_tipo = IXXCLAVENODUP;
      else
         claves[i].clave_tipo = IXXCLAVEDUP;
      claves[i].clave_ncampos = pfi->f_clave[i].k_ncampos;
      claves[i].clave_campos = (IXXCAMPO *)malloc(sizeof(IXXCAMPO) * (claves[i].clave_ncampos+1));
      memset(claves[i].clave_campos,0,sizeof(IXXCAMPO) * (claves[i].clave_ncampos+1));
      for (k = 0;k < claves[i].clave_ncampos;k++)
      {
         campo = pfi->f_clave[i].k_campo[k] - 1;
         o = pfi->f_campos[campo].c_offset;
         l = pfi->f_campos[campo].c_long;
         t = pfi->f_campos[campo].c_tipo;
         claves[i].clave_campos[k].extra_type = IXXCNONE;
         switch(t)
         {
         case INTTIPO:
            claves[i].clave_campos[k].type = IXXCSHORT;
            break;
         case LONGTIPO:
            claves[i].clave_campos[k].type = IXXCLONG;
            break;
         case DOUBLETIPO:
            claves[i].clave_campos[k].type = IXXCDOUBLE;
            break;
         case FLOATTIPO:
            claves[i].clave_campos[k].type = IXXCFLOAT;
            break;
         case CHARTIPO:
         default:
            claves[i].clave_campos[k].type = IXXCALFA;
            if (i && l > 19)
               claves[i].clave_campos[k].extra_type |= IXXCNOCASE;
            break;
         }
         if (pfi->f_campos[campo].c_nombre)
            strcpy(tmp,pfi->f_campos[campo].c_nombre);
         else
            tmp[0] = 0;
         tmp[24] = 0;
         
         if (pfi->f_clave[i].k_orden[k] < 0)
            claves[i].clave_campos[k].modo = 1;
         else
            claves[i].clave_campos[k].modo = 0;
         claves[i].clave_campos[k].len = l;
         claves[i].clave_campos[k].pos_en_reg = o;
         strcpy(claves[i].clave_campos[k].name,tmp);
      }
   }
   
#ifdef NOHAYDDX
   pfi->f_fd = fd = ixx_nuevo_indice(nombre,nclaves,claves,lfich,NULL,NULL,dtabla);
#else
   descamp = descrip_camps(pfi,&defectos,&dtabla);
#ifdef CONTROLNLM
   pprint(2575,"*03*");
   sleep(1);
#endif
   pfi->f_fd = fd = ixx_nuevo_indice(nombre,nclaves,claves,lfich,descamp,defectos,dtabla);
#ifdef CONTROLNLM
   pprint(2575,"    ");
   sleep(1);
#endif
   libera_des_tabla(pfi,&defectos,&descamp,&dtabla);
#endif
   for (i = 0;i < nclaves;i++)
   {
      free(claves[i].clave_campos);
   }
   free(claves);
   
   return(fd);
}

/* cerrar indexado */
_cierra_ind(fd)
register int fd;
{
   int er;
#ifdef CONTROLNLM
   pprint(2575,"*04*");
   sleep(1);
#endif
   
#ifdef FGVDOS
   if (fd >= IXXMAXFILES)
      er = sqlsrv__cierra_ind(cierra_join_(fd));
   else
#endif
      er = ixx_cierra_indice(cierra_join_(fd));
#ifdef CONTROLNLM
   pprint(2575,"    ");
   sleep(1);
#endif

   if (con_respaldo_externo)
   {
       respaldo_cierra_ind(fd);
   }

   return(er);
}

int cierra_ind(pfi)
register FI_DEF *pfi;
{
   if (DeterminaLlamada(pfi))
			{
#ifdef FGVDOS
      return(sqlsrv__cierra_ind(pfi->f_fd));
#else
      return(-1);
#endif
			}

   return(_cierra_ind(pfi->f_fd));
}


/*
* cierra_todo
* cierra todos los ficheros indexados abiertos, devuelve cuantos ha cerrado
*/
cierra_todo(como)
int como; /* no esta usado */
{
			int i,mascomo = 0;
         como = 0;        
         
         
         
#ifdef FGVDOS
         mascomo = sqlsrv_cierra_todo(como);
#endif
         
         for (i = 0;i < IXXMAXFILES;i++)
         {
            if (ixxfichs[i])
            {
#ifdef CONTROLNLM
													  pprint(2575,"*05*");
                                         sleep(1);
#endif
                                         if (ixx_cierra_indice(cierra_join_(i)) >= 0)
                                            como++;
#ifdef CONTROLNLM
                                         pprint(2575,"    ");
                                         sleep(1);
#endif
            }
         }
         

         ixx_libera_respaldo();
         ixx_libera_diu();

         if (tabla_tipo_datos)
         {
            for (i = 0;tabla_tipo_datos[i];i++)
               free(tabla_tipo_datos[i]);
            free(tabla_tipo_datos);
            tabla_tipo_datos = NULL;
         }
         
         primerisimo = 1;
         return(como+mascomo);
}


/*
* borra_ind
* borra los ficheros del sistema correspondientes al indexado
* los elimina del directorio
* devuelve 0 si va bien, -1 si hay error
*/
borra_ind(pfi)
register FI_DEF *pfi;
{
   char tmp[256];
   
   if (DeterminaLlamada(pfi))
   {
#ifdef FGVDOS
      return(sqlsrv_borra_ind(pfi));
#else
      return(-1);
#endif
   }
   
   
   ixx_fuera_cache(pfi->f_fichero);
   sprintf(tmp,"%s.ixx",pfi->f_fichero);
   unlink(tmp);
   sprintf(tmp,"%s.dbf",pfi->f_fichero);
   unlink(tmp);
   sprintf(tmp,"%s.dat",pfi->f_fichero);
   unlink(tmp);
   sprintf(tmp,"%s.ddx",pfi->f_fichero);
   unlink(tmp);
   sprintf(tmp,"%s.tmx",pfi->f_fichero);
   unlink(tmp);
   
   ixx_borra_respaldo(pfi->f_fichero);

   ixx_borra_diu(pfi->f_fichero);

   if (con_respaldo_externo)
   {
       respaldo_borra_ind(pfi);
   }

   return(0);
}


/*
* flush_ind
* baja buffers a disco
* devuelve 0 si va bien, -1 si hay error
*/
flush_ind(pfi)
register FI_DEF *pfi;
{
   return(0);
}

/*
* lock_ind
* bloquea todo el indexado
* el fichero debe abrirse con bloqueo manual
* se libera el bloqueo con unlock_ind
* devuelve 0 si va bien, -1 si hay error
*/
lock_ind(pfi)
register FI_DEF *pfi;
{
   return(0);
}


/*
* unlock_ind
* desbloquea todo el indexado
* devuelve 0 si va bien, -1 si hay error
*/
unlock_ind(pfi)
register FI_DEF *pfi;
{
   return(0);
}


/*
* abre_ind
* abre el indexado en el modo especificado
* devuelve el file descriptor (>=0) si va bien, -1 si no
* el acceso al indexado sera por clave primaria
* para cambiar de clave usar cambia_ind
*/
abre_ind(pfi,modo)
register FI_DEF *pfi;
int modo;
{
   char tmp[512];
   int fd,i,j,campo,posi,len,omodo = modo,totc,tipo,orden;
   VENT *w;
   char **descamp;
   char *defectos;
   FgvStdTabla *dtabla = NULL;
   static int un_control = 0;
   
   
   if (primerisimo)
   {
      CosasDelPrincipio();
   }
   
   if (tabla_tipo_datos) AjustaSegunMiTabla(pfi);
   
   if (DeterminaLlamada(pfi))
   {
#ifdef FGVDOS
      return(sqlsrv_abre_ind(pfi,modo));
#else
      return(-1);
#endif
   }
   
   AjustaPunterosSegunDatos(-1);
   
   if (pfi->f_nclaves < 1) return(-1);
   
   if (modo >= 0)
   {
      if (modo & ISEXCLLOCK)
         modo = -4;
      else
         if (modo & ISINPUT)
            modo = -2;
         else
            modo = 0;
   }
#ifdef NOHAYDDX
   fd = ixx_abre_indice(pfi->f_fichero,modo,NULL,NULL,pfi->f_lfich,NULL);
#else
   descamp = descrip_camps(pfi,&defectos,&dtabla);
#ifdef CONTROLNLM
   pprint(2575,"*01*");
   sleep(1);
#endif
   fd = ixx_abre_indice(pfi->f_fichero,modo,descamp,defectos,pfi->f_lfich,dtabla);
#ifdef CONTROLNLM
   pprint(2575,"    ");
   sleep(1);
#endif
   libera_des_tabla(pfi,&defectos,&descamp,&dtabla);
#endif
   pfi->f_fd = fd;
   if (fd < 0)
   {
      if (ixxstatus != IXXENOMEM && ixxstatus != IXXEMUCHOS)
         iserrno = ENOENT;
   }
   /* verificacion del indice */
   if (fd >= 0)
   {
      modo = ixxfichs[fd]->tamreg - pfi->f_lfich;
      if (modo < 0)
      {
         modo = 1;
      }
      else
         if (modo > 0)
         {
            modo = 2;
         }
         else
            if (ixxfichs[fd]->nclaves != pfi->f_nclaves)
												{
               modo = 3;
												}
            else
												{
               for (i = 0;i < pfi->f_nclaves;i++)
               {
                  totc = ixxfichs[fd]->claves[i].clave_ncampos;
                  if (i) totc--;
                  if (pfi->f_clave[i].k_ncampos != totc)
                  {
                     modo = 4;
                     break;
                  }
                  for (j = 0;j < pfi->f_clave[i].k_ncampos;j++)
                  {
                     campo = pfi->f_clave[i].k_campo[j] - 1;
                     orden = pfi->f_clave[i].k_orden[j];
                     if (orden < 0)
                        orden = 1;
                     else
                        orden = 0;
                     posi = pfi->f_campos[campo].c_offset;
                     len = pfi->f_campos[campo].c_long;
                     switch(pfi->f_campos[campo].c_tipo)
                     {
                     case INTTIPO:
                        tipo = IXXCSHORT;
                        break;
                     case LONGTIPO:
                        tipo = IXXCLONG;
                        break;
                     case FLOATTIPO:
                        tipo = IXXCFLOAT;
                        break;
                     case DOUBLETIPO:
                        tipo = IXXCDOUBLE;
                        break;
                     default:
                        tipo = IXXCALFA;
                        break;
                     }
                     
                     if (ixxfichs[fd]->claves[i].clave_campos[j].modo != orden ||
                        posi != ixxfichs[fd]->claves[i].clave_campos[j].pos_en_reg ||
                        len != ixxfichs[fd]->claves[i].clave_campos[j].len ||
                        tipo != ixxfichs[fd]->claves[i].clave_campos[j].type)
                     {
                        modo = 4;
                        break;
                     }
                  }
                  if (j < pfi->f_clave[i].k_ncampos)
                     break;
               }
												}
            if (modo)
            {
               if (un_control)
               {
                  cierra_ind(pfi);
                  aviso(0,"SISTEMA DE FICHEROS INCORRECTO");
                  return(-1);
               }
               strcpy(tmp,"La estructura del fichero no coincide con los datos.\r\nFichero: ");
               strcat(tmp,pfi->f_fichero);
               strcat(tmp,"\r\n");
               strcat(tmp,pfi->f_titulo);
               strcat(tmp,"\r\n");
               if (modo == 1 || modo == 2)
               {
                  if (!ixxfichs[fd]->tamreg)
                  {
                     strcat(tmp,"Los DATOS necesitan un CAMBIO de VERSION!");
                  }
                  else
                  {
                     if (modo == 1)
                        strcat(tmp,"La longitud del REGISTRO es MAYOR!");
                     else
                        strcat(tmp,"La longitud del REGISTRO es MENOR!");
                     strcat(tmp,"\r\n");
                     sprintf(tmp+strlen(tmp),"Long. Fichero = %lu  Long. DEF = %lu",ixxfichs[fd]->tamreg,pfi->f_lfich);
                  }
               }
               else
                  if (modo == 3) 
                  {
                     strcat(tmp,"Ha VARIADO el NUMERO DE CLAVES!");
                     strcat(tmp,"\r\n");
                     sprintf(tmp+strlen(tmp),"Claves Fichero = %lu  Claves DEF = %lu",ixxfichs[fd]->nclaves,pfi->f_nclaves);                     
                  }
                  else
                     if (modo == 4)
                     {
                        strcat(tmp,"Ha VARIADO alguna CLAVE!");
                     }
                     
                     if (modo == 3 || modo == 4)
                     {
#ifdef CONTROLNLM
                        pprint(2575,"*06*");
                        sleep(1);
#endif
                        ixx_cierra_indice(cierra_join_(fd));
#ifdef CONTROLNLM
                        pprint(2575,"    ");
                        sleep(1);
#endif
                        do {
#ifdef CONTROLNLM
                           pprint(2575,"*02*");
                           sleep(1);
#endif
                           descamp = descrip_camps(pfi,&defectos,&dtabla);
                           fd = ixx_abre_indice(pfi->f_fichero,-4,descamp,defectos,0,dtabla);
                           libera_des_tabla(pfi,&defectos,&descamp,&dtabla);
#ifdef CONTROLNLM
                           pprint(2575,"    ");
                           sleep(1);
#endif
                           if (fd < 0)
                           {
                              if (iserrno == EFLOCKED)
                              { 											  
                                 pito();
                                 pito();
                                 if (confirmar(2400,"ASEGURESE DE QUE NO HAYA NADIE USANDO ESTE FICHERO, ¿ABORTAR? ","N") == 0)
                                 {
                                    modo = 1;
                                    break;
                                 }												  
                              }
                              else
                              {
                                 modo = 1;
                                 break;
                              }
                           }
                        } while(fd < 0);
                        if (modo != 1)
                        {
#ifdef CONTROLNLM
                           pprint(2575,"*07*");
                           sleep(1);
#endif
                           ixx_cierra_indice(cierra_join_(fd));
#ifdef CONTROLNLM
                           pprint(2575,"    ");
                           sleep(1);
#endif
                           sprintf(tmp,"%s.ixx",pfi->f_fichero);
                           unlink(tmp);
                           crea_ind(pfi);
                           cierra_ind(pfi);
                           fd = -2;                           
                           strcat(tmp,"\r\nSi se repite con este fichero, pongase  en  contacto");
                        }
                     }
                     if (modo == 1 || modo == 2)
                     {
                        strcat(tmp,"\r\nNo debe continuar en la opcion,  pongase  en  contacto");
#ifdef CONTROLNLM
                        pprint(2575,"*08*");
                        sleep(1);
#endif
                        ixx_cierra_indice(cierra_join_(fd));
#ifdef CONTROLNLM
                        pprint(2575,"    ");
                        sleep(1);
#endif
                        fd = -1;
                        iserrno = EBADFILE;
                     }
                     strcat(tmp,"\r\ncon el servicio de mantenimiento que tenga contratado.");
                     if (modo == 1 || modo == 2)
                     {
                        pito();
                        if (DespachadorExterno)
                        {
                             char _tmp[512];
                             int *p1 = (int *)_tmp;
                             char *_p = _tmp+sizeof(int)*2;
                             sprintf(_p,"%-20.20s %s","ATENCION!!",tmp);
                             p1[0] = sizeof(int)*2 + strlen(_p) + 2;     
                             p1[1] = 3;
                             (*DespachadorExterno)((int)DESPACHO_MAINCOMMAND,ID_DLGINDICES,(void *)_tmp);
                        }                        
                     }
                     if (fd == -2)
                     {
                        un_control = 1;
                        fd = abre_ind(pfi,omodo);
                        un_control = 0;
                     }
                    }
            }

            if (fd >= 0)
            {
               if (con_respaldo_externo)
               {
                   respaldo_abre_ind(pfi);
               }
            }

            return(fd);
}

/*
* cambia_ind
* selecciona un indice a usar en las siquientes llamadas
* se situa en la primera clave del indice deseado
* devuelve 0 si va bien, -1 si hay error
*/
cambia_ind(pfi,clave)
register FI_DEF *pfi;
int clave;
{
   if (DeterminaLlamada(pfi))
   {
#ifdef FGVDOS
      return(sqlsrv_cambia_ind(pfi,clave));
#else
      return(-1);
#endif
   }
   return(apunta_ind(pfi,clave,ISFIRST,0));
}

apunta_ind(pfi,clave,modo,trozo)
register FI_DEF *pfi;
int clave,modo,trozo;
{
   int r;
   
   if (DeterminaLlamada(pfi))
			{
#ifdef FGVDOS
      return(sqlsrv_apunta_ind(pfi,clave,modo,trozo));
#else
      return(-1);
#endif
			}
   
   
   if (clave < 0 || clave > pfi->f_nclaves)
			{
      iserrno = EBADKEY;
      return(-1);
			}
   modo &= ~ISLOCK;
   modo &= ~ISWAIT;
   if (!clave)
      clave = pfi->f_nclaves;
   else
      clave--;
   isrecnum = f_registro[pfi->f_fd];
   
   no_corrup_error = 1;
   r = ixx_lee_indice(pfi->f_fd,pfi->f_buf,modo,clave,0,trozo,0);
   no_corrup_error = 0;
   if (r < 0 && ixxstatus == IXXECORRUP)
			{
      ixx_reconstruye(pfi->f_fd);
      isrecnum = f_registro[pfi->f_fd];
      r = ixx_lee_indice(pfi->f_fd,pfi->f_buf,modo,clave,0,trozo,0);
			}
   f_registro[pfi->f_fd] = isrecnum;
   return(r);
}


/*
* libera_ind
* desbloquea registros bloqueados en sucesivas llamadas
* de lectura con la opcion de bloqueo ISLOCK
* devuelve 0 si va bien, -1 si hay error
*/
libera_ind(pfi)
register FI_DEF *pfi;
{
			if (DeterminaLlamada(pfi))
         {
#ifdef FGVDOS
            return(sqlsrv_libera_ind(pfi));
#else
            return(-1);
#endif
         }
         return(ixx_libera(pfi->f_fd,0L));
}

/*
* 
* lee del indexado el registro correspondiente a la clave activa
* en el buf de pfi segun el modo (igual,mayor,menor,....)
* devuelve 0 si va bien, -1 si hay error
*/
lee_reg(pfi,modo)
register FI_DEF *pfi;
int modo; /* modos compatibles C-ISAM */
{
   int r;
   
   if (DeterminaLlamada(pfi))
   {
#ifdef FGVDOS
      return(sqlsrv_lee_reg(pfi,modo));
#else
      return(-1);
#endif
   }
   
   isrecnum = f_registro[pfi->f_fd];
   no_corrup_error = 1;
   r = ixx_lee_indice(pfi->f_fd,pfi->f_buf,modo,-1,1,0,0);
   no_corrup_error = 0;
   if (r < 0 && ixxstatus == IXXECORRUP)
   {
      ixx_reconstruye(pfi->f_fd);
      isrecnum = f_registro[pfi->f_fd];
      r = ixx_lee_indice(pfi->f_fd,pfi->f_buf,modo,-1,1,0,0);
   }
   f_registro[pfi->f_fd] = isrecnum;
   return(r);
}

/*
* graba_nuevo
* da alta un nuevo registro en el indexado
* devuelve 0 si va bien, -1 si hay error
*/
graba_nuevo(pfi)
register FI_DEF *pfi;
{
   int r;
   if (DeterminaLlamada(pfi))
   {
#ifdef FGVDOS
						return(sqlsrv_graba_nuevo(pfi));
#else
                  return(-1);
#endif
   }

   
   isrecnum = f_registro[pfi->f_fd];
   no_corrup_error = 1;
   r = ixx_graba_indice(pfi->f_fd,pfi->f_buf,IXXGNUEVO);
   no_corrup_error = 0;
   if (r < 0 && ixxstatus == IXXECORRUP)
   {
		ixx_reconstruye(pfi->f_fd);
        isrecnum = f_registro[pfi->f_fd];
        r = ixx_graba_indice(pfi->f_fd,pfi->f_buf,IXXGNUEVO);
   }
   
   if (!r && fecha_protegida)
   {
		if (isrecnum > max_prote_reg)
        {
             ixx_borra_indice(pfi->f_fd,pfi->f_buf,IXXBCLAVE);
             iserrno = 999;
             return(-1);
        }
   }
   if (r >= 0)
   {
       if (con_respaldo_externo)
       {
           respaldo_graba_nuevo(pfi);
       }
   }
   return(r);
}

/*
* graba_actual
* regraba un registro previamente leido
* devuelve 0 si va bien, -1 si hay error
*/
graba_actual(pfi)
register FI_DEF *pfi;
{
   int r;
   
   if (DeterminaLlamada(pfi))
   {
#ifdef FGVDOS
      return(sqlsrv_graba_actual(pfi));
#else
      return(-1);
#endif
   }

   isrecnum = f_registro[pfi->f_fd];
   no_corrup_error = 1;
   r = ixx_graba_indice(pfi->f_fd,pfi->f_buf,IXXGACTUA);
   no_corrup_error = 0;
   if (r < 0 && ixxstatus == IXXECORRUP)
   {
      ixx_reconstruye(pfi->f_fd);
      isrecnum = f_registro[pfi->f_fd];
      r = ixx_graba_indice(pfi->f_fd,pfi->f_buf,IXXGACTUA);
   }
   f_registro[pfi->f_fd] = isrecnum;

   if (r >= 0)
   {
       if (con_respaldo_externo)
       {
           respaldo_graba_actual(pfi);
       }
   }
   return(r);
}

/*
* borra_actual
* borra un registro previamente leido
* devuelve 0 si va bien, -1 si hay error
*/
borra_actual(pfi)
register FI_DEF *pfi;
{
   int r;
   if (DeterminaLlamada(pfi))
   {
#ifdef FGVDOS
      return(sqlsrv_borra_actual(pfi));
#else
      return(-1);
#endif
   }   

   isrecnum = f_registro[pfi->f_fd];
   no_corrup_error = 1;
   r = ixx_borra_indice(pfi->f_fd,pfi->f_buf,IXXBACTUA);
   no_corrup_error = 0;
   if (r < 0 && ixxstatus == IXXECORRUP)
   {
      ixx_reconstruye(pfi->f_fd);
      isrecnum = f_registro[pfi->f_fd];
      r = ixx_borra_indice(pfi->f_fd,pfi->f_buf,IXXBACTUA);
   }
   f_registro[pfi->f_fd] = isrecnum;


   if (r >= 0 && con_respaldo_externo)
   {
       respaldo_borra_actual(pfi);
   }   

   return(r);
}

/*
* borra_reg
* borra un registro mediante la clave primaria
* devuelve 0 si va bien, -1 si hay error
*/
borra_reg(pfi)
register FI_DEF *pfi;
{
   int r;
   if (DeterminaLlamada(pfi))
   {
#ifdef FGVDOS
      return(sqlsrv_borra_reg(pfi));
#else
      return(-1);
#endif
   }

   isrecnum = f_registro[pfi->f_fd];
   no_corrup_error = 1;
   r = ixx_borra_indice(pfi->f_fd,pfi->f_buf,IXXBCLAVE);
   no_corrup_error = 0;
   if (r < 0 && ixxstatus == IXXECORRUP)
   {
      ixx_reconstruye(pfi->f_fd);
      isrecnum = f_registro[pfi->f_fd];
      r = ixx_borra_indice(pfi->f_fd,pfi->f_buf,IXXBCLAVE);
   }
   f_registro[pfi->f_fd] = isrecnum;

   if (r >= 0 && con_respaldo_externo)
   {
       respaldo_borra_reg(pfi);
   }
   return(r);
}

long da_registo_actual(pfi)
FI_DEF *pfi;
{
   if (DeterminaLlamada(pfi))
   {
#ifdef FGVDOS
      return(sqlsrv_da_registo_actual(pfi));
#else
      return(-1);
#endif
   }
   
   if (pfi->f_fd < 0) return(0L);
   return(f_registro[pfi->f_fd]);
}

int pon_registro_actual(pfi,r)
FI_DEF *pfi;
long r;
{
   if (DeterminaLlamada(pfi))
   {
#ifdef FGVDOS
      return(sqlsrv_pon_registro_actual(pfi,r));
#else
      return(-1);
#endif
   }
   if (pfi->f_fd < 0) return(-1);
   f_registro[pfi->f_fd] = r;
   return (0);
}

int ind_clave_activa(pfi)
FI_DEF *pfi;
{
   int fd = pfi->f_fd;
   
   if (DeterminaLlamada(pfi))
   {
#ifdef FGVDOS
      return(sqlsrv_ind_clave_activa(pfi));
#else
      return(-1);
#endif
   }
   
   if (ixxfichs[fd])
      return(ixxfichs[fd]->clave_activa+1);
   return(0);
}

lee_soloindice(pfi,modo)
register FI_DEF *pfi;
int modo;
{
   int r;
   
   if (DeterminaLlamada(pfi))
   {
#ifdef FGVDOS
      return(sqlsrv_lee_soloindice(pfi,modo));
#else
      return(-1);
#endif
   }
   
   isrecnum = f_registro[pfi->f_fd];
   no_corrup_error = 1;
   r = ixx_lee_indice(pfi->f_fd,pfi->f_buf,modo,-1,0,0,0);
   no_corrup_error = 0;
   if (r < 0 && ixxstatus == IXXECORRUP)
   {
      ixx_reconstruye(pfi->f_fd);
      isrecnum = f_registro[pfi->f_fd];
      r = ixx_lee_indice(pfi->f_fd,pfi->f_buf,modo,-1,0,0,0);
   }
   f_registro[pfi->f_fd] = isrecnum;
   return(r);
}

int abre_directo(pfi,nom)
FI_DEF *pfi;
char *nom;
{
   int fd,i,l;
   char **descamp;
   char *defectos;
   FgvStdTabla *dtabla = NULL;
   
   
   if (DeterminaLlamada(pfi))
   {
#ifdef FGVDOS
      return(sqlsrv_abre_directo(pfi,nom));
#else
      return(-1);
#endif
   }
   AjustaPunterosSegunDatos(-1);
   
   if (ixx_datos_dbf == 1)
   {   
      descamp = descrip_camps(pfi,&defectos,&dtabla);   
      l = pfi->f_lfich;
      fd = dbf_abre(pfi->f_fichero,descamp,defectos,&l,(IXXFILE *)NULL,(int *)NULL,dtabla);
      libera_des_tabla(pfi,&defectos,&descamp,&dtabla);      
   }
   else
   {
      sprintf(nom,"%s.dat",pfi->f_fichero);   
      fd = rw_open_(nom,O_RDONLY | O_BINARY);
   }
   if (fd >= 0)
      modos_locales[fd] = ixx_datos_dbf + default_dbf_id;
   return(fd);
}

int cierra_directo(fd)
int fd;
{
   
#ifdef FGVDOS
   if (fd >= IXXMAXFILES)
      return(sqlsrv_cierra_directo(fd));
#endif
   
   switch (modos_locales[fd])
   {
   case 0:
      ixx_datos_dbf = 0;
      default_dbf_id = 0;	  
      break;
   case 1:
      ixx_datos_dbf = 1;
      default_dbf_id = 0;	  
      break;
   case 2:
      ixx_datos_dbf = 1;
      default_dbf_id = 1;	  
      break;
   case 3:
      ixx_datos_dbf = 1;
      default_dbf_id = 2;
      break;
   }    
   AjustaPunterosSegunDatos(-1);
   
   if (ixx_datos_dbf == 1)
   {
      return(dbf_cierra(fd));
   }
   else
   {
      return(rw_close_(fd));
   }
}

int lee_directo(fd,pos,pfi,buf)
int fd;
long pos;
FI_DEF *pfi;
char *buf;
{
   int e;
   
#ifdef FGVDOS
   if (fd >= IXXMAXFILES)
      return(sqlsrv_lee_directo(fd,pos,pfi,buf));
#endif
   
   switch (modos_locales[fd])
   {
   case 0:
      ixx_datos_dbf = 0;
      default_dbf_id = 0;	  
      break;
   case 1:
      ixx_datos_dbf = 1;
      default_dbf_id = 0;	  
      break;
   case 2:
      ixx_datos_dbf = 1;
      default_dbf_id = 1;	  
      break;
   case 3:
      ixx_datos_dbf = 1;
      default_dbf_id = 2;
      break;
   }    
   AjustaPunterosSegunDatos(-1);
   
   if (ixx_datos_dbf == 1)
   {
      no_read_error++;
      e = dbf_lee(fd,pos,buf,pfi->f_lfich);
      no_read_error--;
      return(e);
   }
   else
   {
      if (pos < 1L) return(-1);
      rw_lseek_(fd,(pos-1) * (pfi->f_lfich + 1),SEEK_SET);
      return(rw_read_(fd,buf,pfi->f_lfich));
   }
}

int lee_secuencia(fd,po,md,tamr,buf)
int fd;
long *po;
int *md;
int tamr;
char *buf;
{
   char c;
   int e;
   
#ifdef FGVDOS
   if (fd >= IXXMAXFILES)
      return(sqlsrv_lee_secuencia(fd,po,md,tamr,buf));
#endif
   
   switch (modos_locales[fd])
   {
   case 0:
      ixx_datos_dbf = 0;
      default_dbf_id = 0;	  
      break;
   case 1:
      ixx_datos_dbf = 1;
      default_dbf_id = 0;	  
      break;
   case 2:
      ixx_datos_dbf = 1;
      default_dbf_id = 1;	  
      break;
   case 3:
      ixx_datos_dbf = 1;
      default_dbf_id = 2;
      break;
   }    
   AjustaPunterosSegunDatos(-1);
   if (ixx_datos_dbf == 1)
   {
      if (!*md)
      {
         *md = 1;
         *po = 0L;
      }
      do {
         (*po)++;
         no_read_error++;
         e = dbf_lee(fd,*po,buf,tamr);
         no_read_error--;
         if (e < 0) return(-1);
      } while(e == 1);
   }
   else
   {
      if (!*md)
      {
         *md = 1;
         *po = 0L;
         rw_lseek_(fd,0L,SEEK_SET);
      }
      do {
         (*po)++;
         if (rw_read_(fd,buf,tamr) != tamr) return(-1);
         if (rw_read_(fd,&c,1) != 1) return(-1);
      } while(c != '\n');
   }
   return(0);
}


/* !!! */
long tam_btrv(pfi)
FI_DEF *pfi;
{
   long tam;
   int ixx_fd = pfi->f_fd;
   
   if (DeterminaLlamada(pfi))
   {
#ifdef FGVDOS
      return(sqlsrv_tam_btrv(pfi));
#else
      return(-1);
#endif
   }
   
   
   ixx_tam_fichero(ixx_fd,&tam);
   return(tam);
}



int empieza_trans()
{
   /* no soportado de momento */
}

int anula_trans()
{
   /* no soportado de momento */
}

int acaba_trans()
{
   /* no soportado de momento */
}

int lee_por_posicion(FI_DEF *pfi,int key,long posicion,Uchar *dat_reg,int modo,int flags)
{
	int ixx_fd = pfi->f_fd;
	if (pfi->tipo_datos == 5)
	{
		long *plong;
		char *buf = pfi->f_buf;
		int r;
		plong = (long *)pfi->f_data;
		plong[5] = posicion-1;
		pfi->f_buf = dat_reg;
		/* modo ? flags ? */
	    
		r = memory_lee_reg(pfi,ISCURR);
		
		pfi->f_buf = buf;
		return r;
	}
#ifdef ESDOS
	if (ixx_fd >= IXXMAXFILES)
	{
		if (sqlsrv_estalimpio(ixx_fd) == 0)
		{
			apunta_ind(pfi,key,ISFIRST,0);
		}
		return sqlsrv_lee_por_posicion(ixx_fd,posicion,dat_reg,modo,flags);
	}
	else
#endif
	{
		return ixx_lee_por_posicion(ixx_fd,posicion,dat_reg,modo,flags);
	}
}


int fgv_pon_contenido(FI_DEF *pfi,unsigned char *con,int len)
{
	if (pfi->tipo_datos == 5)
	{
		return memory_pon_contenido(pfi,con,len);
	}
	return ixx_pon_contenido(pfi->f_fd,con,len);
}

int memcpy_int(char *dest,void *src,int len)
{
int i;
   memcpy(dest,src,len);
   for (i = 0;i < (len/sizeof(int));i++)
   {
	  moto_int(dest);
	  dest += sizeof(int);
   }
   return len;
}


int fgv_fin_contenido(FI_DEF *pfi)
{
	if (pfi->tipo_datos == 5)
	{
	   char *pdata;
	   long *plong;
	   if (!pfi->f_data)
		   return -1;
	   plong = (long *)pfi->f_data;
	   plong[9] = 0;
  	   return 0;
	}
	return ixx_fin_contenido(pfi->f_fd);
}


