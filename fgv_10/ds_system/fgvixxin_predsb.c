
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


int (*_ejecuta_evento_fichero15)(int ipfi) = NULL;
int (*_ejecuta_evento_fichero16)(int ipfi) = NULL;
int (*_ejecuta_evento_fichero17)(int ipfi) = NULL;

static long f_registro[IXXMAXFILES] = {0,0,0,0,0,};
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

extern long IxxNumeroPosicion;

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
		 int nt;
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
		 nt = 0;
         for (i = 0;i < pfi->f_ncampos;i++)
         {
			if (pfi->f_campos[i].c_tipo == NULOTIPO)
			{
				(*dtabla)->ncampos--;
				continue;
			}			
            (*dtabla)->Campo[nt].longitud = long_campo(pfi,i);
            (*dtabla)->Campo[nt].decimales = deci_campo(pfi,i);
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
               (*dtabla)->Campo[nt].longitud = pfi->f_campos[i].c_long;
               break;
            default:
               tipo = IXXCALFA;
               (*dtabla)->Campo[nt].longitud = pfi->f_campos[i].c_long;
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
                  descamp[nt] = malloc(strlen(tmp)+1);
                  strcpy(descamp[nt],tmp);
                  tmp[18] = 0;
                  strcpy((*dtabla)->Campo[nt].ident,tmp+8);
                  (*dtabla)->Campo[nt].tipo = pfi->f_campos[i].c_tipo + 48;
                  (*dtabla)->Campo[nt].len = pfi->f_campos[i].c_long;  
			nt++;
         }         
		 for (i = nt;i < pfi->f_ncampos;i++)
		 {
			 descamp[i] = NULL;
		 }
		 descamp[i] = NULL;
         p1 = (short *)&(descamp[nt+1]);
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
	   if (descamp[i])
	   {
		   free(descamp[i]);
	   }
	   else
		   break;
   }
   free(descamp);
   *_descamp = NULL;
   libera_agitabla(*dtabla);
   *dtabla = NULL;
}


/* short tipo_datos; /* -1 defecto ... 0 = dat 1 = dbf interno 2 = dbf standard 3 = dbf foxpro 4 = odbc */
int DeterminaLlamada(FI_DEF *pfi)
{
   
   if (pfi->tipo_datos < 0 || pfi->tipo_datos > 5)
      pfi->tipo_datos = defecto_modo_datos;
   
   switch (pfi->tipo_datos)
   {
   case 5:
				  /* fichero en memoria */
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

   if (pfi->tipo_datos == 5)
   {
	   return -1; /* los ficheros en memoria no se crean de esta manera */
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
	int r;
   if (DeterminaLlamada(pfi))
			{
#ifdef FGVDOS
      return(sqlsrv__cierra_ind(pfi->f_fd));
#else
      return(-1);
#endif
			}

   if (pfi->tipo_datos == 5)
   {
	   fgv_fin_contenido(pfi);	   
	   pfi->f_fd = -1;
	   return 0; /* ni se abren ni se cierran */
   }

   r = _cierra_ind(pfi->f_fd);
   if (r >= 0)
	   pfi->f_fd = -1;
   return(r);
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

   if (pfi->tipo_datos == 5)
   {
	   if (!pfi->f_data)
			return -1;
	   free(pfi->f_data);
	   pfi->f_data = NULL;
	   pfi->f_fd = -1;
	   pfi->tipo_datos = 0;
	   return 0;
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

   sprintf(tmp,"%s.diu",pfi->f_fichero);
   unlink(tmp);

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

#ifdef NUEVO_PORVER
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
   char tmp[256];
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

   if (pfi->tipo_datos == 5)
   {	   
	   pfi->f_fd = 5000;
	   cambia_ind(pfi,1);
	   return 5000; /* fichero en memoria */
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
	  f_registro[pfi->f_fd] = 0L;
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
         else if (ixxfichs[fd]->nclaves != pfi->f_nclaves)
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
                  aviso(0,__Texto[510]);
                  return(-1);
               }
               w = w_ini(1010,2070);
               w_salva(w);
               Ai();cuadro(1010,2070);Bi();
               zonablan(1111,1969);
               Ap();pprint(1111,__Texto[282]);Cp();
               pprint(1311,__Texto[242]);
               v_print(pfi->f_fichero);
               pprint(1420,pfi->f_titulo);
               if (modo == 1 || modo == 2)
               {
                  if (!ixxfichs[fd]->tamreg)
                  {
                     pprint(1511,__Texto[243]);
                  }
                  else
                  {
                     if (modo == 1)
                        pprint(1511,__Texto[244]);
                     else
                        pprint(1511,__Texto[245]);
                     cursor(1612);
                     sprintf(tmp,__Texto[246],ixxfichs[fd]->tamreg,pfi->f_lfich);
                     v_print(tmp);
                  }
               }
               else
                  if (modo == 3) {
                     pprint(1511,__Texto[247]);
                     pprint(1611," ");
                     sprintf(tmp,__Texto[248],ixxfichs[fd]->nclaves,pfi->f_nclaves);
                     v_print(tmp);
                  }
                  else
                     if (modo == 4)
                     {
                        pprint(1511,__Texto[249]);
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
                                 if (confirmar(2400,__Texto[432],__Texto[447]) == 0)
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
                           pprint(1813,__Texto[250]);
                        }
                     }
                     if (modo == 1 || modo == 2)
                     {
                        pprint(1813,__Texto[251]);
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
                     pprint(1913,__Texto[252]);
                     if (modo == 1 || modo == 2)
                     {
                        pito();
                        parada();
                     }
                     w_pinta(w);
                     w_fin(w);
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

#endif

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
 *
 * cambia_ind
 * selecciona un indice a usar en las siquientes llamadas
 * se situa en la primera clave del indice deseado
 * devuelve 0 si va bien, -1 si hay error
 *
 */
int cambia_ind(pfi,clave)
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

int apunta_ind(pfi,clave,modo,trozo)
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
   
   if (pfi->tipo_datos == 5)
   {	   
	   return memory_apunta_ind(pfi,clave,modo,trozo);
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

   if (pfi->tipo_datos == 5)
   {
	   return 0;
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
   
   if (pfi->tipo_datos == 5)
   {	   
	   return memory_lee_reg(pfi,modo);
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

   if (pfi->f_ipfi > -1 && _ejecuta_evento_fichero15)   
   {
	   r = (*_ejecuta_evento_fichero15)(pfi->f_ipfi);
	   if (r > 0)
	   {
		   iserrno = r;
		   return -1;
	   }
   }
   

   if (DeterminaLlamada(pfi))
   {
#ifdef FGVDOS
						return(sqlsrv_graba_nuevo(pfi));
#else
                  return(-1);
#endif
   }
   
   if (pfi->tipo_datos == 5)
   {
	   return memory_graba_nuevo(pfi);
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

int graba_nuevo_en_posicion(register FI_DEF *pfi,long reg)
{
   int r;

   if (pfi->f_ipfi > -1 && _ejecuta_evento_fichero15)   
   {
	   r = (*_ejecuta_evento_fichero15)(pfi->f_ipfi);
	   if (r > 0)
	   {
		   iserrno = r;
		   return -1;
	   }
   }

   if (DeterminaLlamada(pfi))
   {
#ifdef FGVDOS
						return(sqlsrv_graba_nuevo(pfi));
#else
                  return(-1);
#endif
   }
   
   if (pfi->tipo_datos == 5)
   {
	   return memory_graba_nuevo(pfi);
   }
   isrecnum = reg;
   r = ixx_graba_indice(pfi->f_fd,pfi->f_buf,IXXGPOSIC);
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
   
   if (pfi->f_ipfi > -1 && _ejecuta_evento_fichero16)   
   {
	   r = (*_ejecuta_evento_fichero16)(pfi->f_ipfi);
	   if (r > 0)
	   {
		   iserrno = r;
		   return -1;
	   }
   }

   if (DeterminaLlamada(pfi))
   {
#ifdef FGVDOS
    return(sqlsrv_graba_actual(pfi));
#else
    return(-1);
#endif
   }

   if (pfi->tipo_datos == 5)
   {
	   return memory_graba_actual(pfi);
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

   if (pfi->f_ipfi > -1 && _ejecuta_evento_fichero17)   
   {
	   r = (*_ejecuta_evento_fichero17)(pfi->f_ipfi);
	   if (r > 0)
	   {
		   iserrno = r;
		   return -1;
	   }
   }

   if (DeterminaLlamada(pfi))
   {
#ifdef FGVDOS
      return(sqlsrv_borra_actual(pfi));
#else
      return(-1);
#endif
   }

   if (pfi->tipo_datos == 5)
   {
	   return memory_borra_actual(pfi);
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

   if (pfi->f_ipfi > -1 && _ejecuta_evento_fichero17)   
   {
	   r = (*_ejecuta_evento_fichero17)(pfi->f_ipfi);
	   if (r > 0)
	   {
		   iserrno = r;
		   return -1;
	   }
   }

   if (DeterminaLlamada(pfi))
   {
#ifdef FGVDOS
      return(sqlsrv_borra_reg(pfi));
#else
      return(-1);
#endif
   }
   if (pfi->tipo_datos == 5)
   {
	   return memory_borra_reg(pfi);
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
   
   if (pfi->tipo_datos == 5)
   {
	   return memory_da_registo_actual(pfi);
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
   if (pfi->tipo_datos == 5)
   {
	   return memory_pon_registro_actual(pfi,r);
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
   
   if (pfi->tipo_datos == 5)
   {
	   return memory_clave_activa(pfi);
   }

   if (ixxfichs[fd])
      return(ixxfichs[fd]->clave_activa+1);
   return(0);
}

int lee_soloindice(pfi,modo)
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
   
   if (pfi->tipo_datos == 5)
   {
	   return memory_lee_soloindice(pfi,modo);
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

   if (pfi->tipo_datos == 5)
   {
	  return 5000;
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
   if (fd >= IXXMAXFILES && fd != 5000)
      return(sqlsrv_lee_directo(fd,pos,pfi,buf));
#endif
   if (pfi->tipo_datos == 5)
   {
	   char *pdata;
	   long *plong;
	   if (!pfi->f_data)
		   return -1;
	   plong = (long *)pfi->f_data;	   
	   if (pos < 1 || pos > plong[2])
	   {
		   return 0;
	   }
	   pos--;
	   pdata = pfi->f_data + plong[0] + pos * pfi->f_lfich;
	   memcpy(pfi->f_buf,pdata,pfi->f_lfich);
	   return pfi->f_lfich;
   }

   
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

int lee_secuencia(pfi,fd,po,md,tamr,buf)
FI_DEF *pfi;
int fd;
long *po;
int *md;
int tamr;
char *buf;
{
   char c;
   int e;
   
#ifdef FGVDOS
   if (fd >= IXXMAXFILES && fd != 5000)
      return(sqlsrv_lee_secuencia(fd,po,md,tamr,buf));
#endif
   if (pfi->tipo_datos == 5)
   {
	   char *pdata;
	   long *plong;
	   if (!pfi->f_data)
		   return -1;
       if (!*md)
	   {
         *md = 1;
         *po = 0L;         
	   }
	   plong = (long *)pfi->f_data;	   
	   if (*po >= plong[2] || pfi->f_lfich != tamr)
	   {
		   return -1;
	   }	   
	   pdata = pfi->f_data + plong[0] + (*po) * tamr;
	   memcpy(buf,pdata,pfi->f_lfich);
	   (*po)++;	   
	   return 0;
   }
   
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
      do 
	  {
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
      do 
	  {
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
   
   if (pfi->tipo_datos == 5)
   {
	   return memory_tam_btrv(pfi);
   }
   
   ixx_tam_fichero(ixx_fd,&tam);
   return(tam);
}


int fgv_pon_contenido(FI_DEF *pfi,unsigned char *con,int len)
{
	if (pfi->tipo_datos == 5)
	{
		return memory_pon_contenido(pfi,con,len);
	}
	return ixx_pon_contenido(pfi->f_fd,con,len);
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
	   /*
	   int r;		
	   no_corrup_error = 1;
	   r = ixx_lee_por_posicion(ixx_fd,posicion,dat_reg,modo,flags);
	   no_corrup_error = 0;
	   if (r < 0 && ixxstatus == IXXECORRUP)
	   {
		  ixx_reconstruye(pfi->f_fd);		  
		  r = ixx_lee_por_posicion(ixx_fd,posicion,dat_reg,modo,flags);
	   }
	   return r;
	   */
	}
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

static int memory_compara_clave(char *pdata,char *pbuf,int ncampos,int *campos,int *longs,int *tipos)
{
	int i;
	int r = 0;
	char *p1,*p2;
	for (i = 0;i < ncampos;i++)
	{
		p1 = pdata + campos[i];
		p2 = pbuf  + campos[i];
		switch(tipos[i])
		{		
		case DOUBLETIPO:
			if ( *((double *)p1) < *((double *)p2) )
				r = -1;
			else if ( *((double *)p1) > *((double *)p2) )
				r = 1;
			break;
		default:
			r = memcmp(p1,p2,longs[i]);
			break;
		}
		if (r)
			return r;
	}
	return r;
}

static int memory_busca_clave(int ini,int fin,char *pdata,long *pkey,int ncampos,int *campos,int *longs,int *tipos,char *pbuf,int lreg,long *reg,int alprincipio)
{
	int r;
	int medio = fin-ini;	

	if (alprincipio)
	{	
		r = memory_compara_clave(pdata+pkey[ini]*lreg,pbuf,ncampos,campos,longs,tipos);
		if (!r || r > 0 || medio < 2)
		{
			*reg = ini;
			return r;
		}
		r = memory_compara_clave(pdata+pkey[fin-1]*lreg,pbuf,ncampos,campos,longs,tipos);
		if (!r || r < 0 || medio == 2)
		{
			*reg = fin-1;
			return r;
		}
	}
	else
	{
		r = memory_compara_clave(pdata+pkey[fin-1]*lreg,pbuf,ncampos,campos,longs,tipos);
		if (!r || r < 0 || medio < 2)
		{
			*reg = fin-1;
			return r;
		}
		r = memory_compara_clave(pdata+pkey[ini]*lreg,pbuf,ncampos,campos,longs,tipos);
		if (!r || r > 0 || medio == 2)
		{
			*reg = ini;
			return r;
		}
	}	
	medio /= 2;
	r = memory_compara_clave(pdata+pkey[ini+medio]*lreg,pbuf,ncampos,campos,longs,tipos);
	if (!r || medio == 1)
	{
		*reg = ini+medio;
		return r;
	}
	if (r < 0)
	{
		/*r =*/ return memory_busca_clave(ini+medio+1,fin-1,pdata,pkey,ncampos,campos,longs,tipos,pbuf,lreg,reg,alprincipio);
		/*
		if (!r)
		{

		}		
		return r;
		*/
	}
	/*r =*/ return memory_busca_clave(ini+1,ini+medio,pdata,pkey,ncampos,campos,longs,tipos,pbuf,lreg,reg,alprincipio);
	/*
	if (!r)
	{
	}	
	return r;
	*/
}


static int memory_busca_reg(int clave,long *preg,FI_DEF *pfi,int trozo,int alprincipio)
{
	char *pdata;
	long *pkey;
	long *plong = (long *)pfi->f_data;
	int campos[300];
	int tipos[300];
	int longs[300];
	int ncampos;
	int i,cc;
	int r;
	int exacta = 0;

	if (!plong[2])
	{		
		*preg = 0;
		return 1;
	}

	if (alprincipio < 0)
	{
		alprincipio = 1;
		exacta = 1;
	}
	pdata = pfi->f_data + plong[1] + clave * sizeof(long) * plong[3];
	pkey = (long)pdata;
	pdata = pfi->f_data + plong[0];

	ncampos = pfi->f_clave[clave].k_ncampos;
	for (i = 0;i < ncampos;i++)
	{		
		if (trozo && i >= trozo) break;
		cc = pfi->f_clave[clave].k_campo[i] - 1;
		if (cc < 0 || cc >= pfi->f_ncampos)
		{
			return -1;
		}
		campos[i] = pfi->f_campos[cc].c_offset;
		longs[i]  = pfi->f_campos[cc].c_long;
		tipos[i]  = pfi->f_campos[cc].c_tipo;		
	}	
	ncampos = i;

	r = memory_busca_clave(0,plong[2],pdata,pkey,ncampos,campos,longs,tipos,pfi->f_buf,pfi->f_lfich,preg,alprincipio);	
	
	if (plong[2] && (!r || !exacta))
	{
		int quedan = 0;
		int punto;
		int ini;
		int fin;
		char *pxbuf;
		if (r)
		{			
			if ((r > 0 && !alprincipio) || (r < 0 && alprincipio))
			{
				alprincipio = !alprincipio;
				/*
				if (clave == 2)
				{
					clave = clave;
				}
				*/
			}
			pxbuf = pdata+pkey[(*preg)]*pfi->f_lfich;
		}
		else
		{
			pxbuf = pfi->f_buf;
		}
		if (alprincipio)
		{
			if (*preg > 0)
			{
				punto = (*preg)-1;
				if (memory_compara_clave(pdata+pkey[punto]*pfi->f_lfich,pxbuf,ncampos,campos,longs,tipos) != 0)
				{
					return r;
				}
				*preg = punto;
				ini = 0;
				fin = punto;
				quedan = fin-ini;
				while (quedan > 1)
				{
					punto = ini + quedan/2;
					if (memory_compara_clave(pdata+pkey[punto]*pfi->f_lfich,pxbuf,ncampos,campos,longs,tipos) != 0)
					{
						ini = punto + 1;
						quedan = fin-ini;
					}
					else
					{
						*preg = punto;
						fin = punto;
						quedan = fin-ini;
					}
				}
				if (quedan == 1)
				{
					if (memory_compara_clave(pdata+pkey[ini]*pfi->f_lfich,pxbuf,ncampos,campos,longs,tipos) == 0)
					{
						*preg = ini;
					}
				}
			}
		}
		else
		{
			if (*preg < (plong[2]-1))
			{
				punto = (*preg)+1;
				if (memory_compara_clave(pdata+pkey[punto]*pfi->f_lfich,pxbuf,ncampos,campos,longs,tipos) != 0)
				{
					return r;
				}
				*preg = punto;
				ini = punto+1;
				fin = plong[2];
				quedan = fin-ini;
				while (quedan > 1)
				{
					punto = ini + quedan/2;
					if (memory_compara_clave(pdata+pkey[punto]*pfi->f_lfich,pxbuf,ncampos,campos,longs,tipos) != 0)
					{
						fin = punto;
						quedan = fin-ini;
					}
					else
					{
						*preg = punto;
						ini = punto + 1;
						quedan = fin-ini;
					}
				}
				if (quedan == 1)
				{
					if (memory_compara_clave(pdata+pkey[ini]*pfi->f_lfich,pxbuf,ncampos,campos,longs,tipos) == 0)
					{
						*preg = ini;
					}
				}
			}
		}
	}
	
	return r;
}

long memory_busca_contenido(int modo,FI_DEF *pfi,long *pkey,long preg,int clave)
{
   long *plong;
   char *pdata;
   long reg;
   int campos[300];   
   int longs[300];
   char *cons[300];
   char tmp[512];
   int ncons;
   int ncampos,i,cc;
   int esta;

   plong = (long *)pfi->f_data;
   if (modo == ISFIRST)
   {
	   preg = 0;
   }
   else
   {
		/* solo adelante ....!!! */
	   if (modo != ISEQUAL || modo != ISGTEQ)
	   {
		   preg++;
	   }
   }
   if (preg >= plong[2])
	   return -1;

   ncampos = 0;
   for (i = 0;i < pfi->f_clave[clave].k_ncampos;i++)
   {
		cc = pfi->f_clave[clave].k_campo[i] - 1;
		if (pfi->f_campos[cc].c_tipo == CHARTIPO)
		{
			longs[ncampos]  = pfi->f_campos[cc].c_long;		
		    campos[ncampos] = pfi->f_campos[cc].c_offset;
			ncampos++;
		}	
   }

   if (!ncampos)
	   return -2; /* lectura normal */

   pdata = pfi->f_data + plong[1] - plong[8];
   ncons = 1;
   cons[0] = pdata;
   for (i = 0;i < plong[9];i++)
   {
	   if (!pdata[i] && pdata[i+1] && (i+1) < plong[9])
	   {
		   if (strlen(cons[ncons-1]))
		   {
			   cons[ncons] = pdata+i+1;
			   ncons++;
		   }
		   else
		   {
			   cons[ncons-1] = pdata+i+1;
		   }
	   }
   }
   if (ncons == 1 && !strlen(cons[0]))
   {
	   return -2; /* lectura noraml contenido vacio */
   }

   while(preg < plong[2])
   {
	   esta = 1;
	   reg = pkey[preg];

	   pdata = pfi->f_data + plong[0] + reg * pfi->f_lfich;
	   for (i = 0;i < ncampos && esta;i++)
	   {
		   memcpy(tmp,pdata+campos[i],longs[i]);
		   tmp[ longs[i] ] = 0;
		   for (cc = 0;cc < ncons;cc++)
		   {
			   if (strbusca(tmp,cons[cc]) < 0)
			   {
				   esta = 0;
				   break;
			   }
		   }
	   }
	   if (esta)
	   {
		   return preg;
	   }
	   preg++;
   }   

   return -3;
}

int memory_apunta_ind(pfi,clave,modo,trozo)
register FI_DEF *pfi;
int clave,modo,trozo;
{
	if (!pfi->f_data)
		return -1;
	{	   
	   long *plong;
	   long *pkey;
	   long *pkx;	   
	   char *pdata;
	   long reg;
	   long preg,xreg;
	   int r = 0,i;
	   int cc,longs,offset;	   

	   plong = (long *)pfi->f_data;

	   if (clave < 0)
	   {
		   clave = plong[4];
	   }
	   else
	   {
		   if (!clave)
			  clave = pfi->f_nclaves;
		   else
			  clave--;
	   }

	   /*if (trozo)*/
	   {
		   plong[7] = trozo;
	   }
	   /*
	   else
	   {
		   trozo = plong[7];
	   }
	   */
  	   modo &= ~ISLOCK;
	   modo &= ~ISWAIT;

	   if (!plong[2])
	   {
		   if (modo == ISFIRST)
		   {
			   plong[4] = clave;
			   iserrno = 0;
			   return 0;
		   }
		   iserrno = ENOREC;
		   return -1;
	   }

	   if (clave < pfi->f_nclaves)
	   {		   
		   preg = plong[5];
		   if (plong[4] != clave && (modo == ISCURR || modo == ISNEXT || modo == ISPREV))
		   {
			   /* reajustar corriente */
			   if (plong[4] < pfi->f_nclaves)
			   {
				   
				   pkey = (long *)((char *)pfi->f_data + plong[1] + plong[4] * sizeof(long) * plong[3]);
				   reg = pkey[preg];			   
			   }
			   else
			   {
				   reg = plong[6];
			   }
		   }   
		   pkey = (long *)((char *)pfi->f_data + plong[1] + clave * sizeof(long) * plong[3]);
		   if (plong[4] != clave && (modo == ISCURR || modo == ISNEXT || modo == ISPREV))
		   {			   
			   preg = 0;
			   pkx = pkey;			   
			   for (cc = 0;cc < plong[2];cc++,pkx++)
			   {
				   if (*pkx == reg)
				   {
					   preg = cc;
					   break;
				   }
			   }
		   }
	   }
	   else
	   {
		   pkey = NULL;
		   preg = plong[6];
		   switch(modo)
		   {
			   case ISEQUAL:
				  modo = ISCURR;
				  break;
			   case ISGREAT:	
				  modo = ISNEXT;
				  break;
			   case ISGTEQ:		
				  modo = ISCURR;
				  break;
			   case ISLOW:		  
				  modo = ISPREV;
				  break;
			   case ISLOWEQ:		  
			   default:
				  modo = ISCURR;
				  break;
		   }
	   }

	   if (pkey && plong[9] && (xreg = memory_busca_contenido(modo,pfi,pkey,preg,clave)) != -2)
	   {		   
		   if (xreg == -1)
		   {
			  iserrno = ENOENT;
			  return -1;
		   }
		   if (xreg == -3)
		   {
			  iserrno = EENDFILE;
			  return -1;
		   }
		   preg = xreg;
	   }
	   else
	   {
		   switch(modo)
		   {
		   case ISFIRST:		  
			  preg = 0;
			  break;
		   case ISLAST:
			  preg = plong[2]-1;
			  break;
		   case ISNEXT:
			  if (preg < (plong[2]-1))
			  {
				  preg++;
			  }
			  else
			  {
				  iserrno = EENDFILE;
				  return -1;
			  }
			  break;
		   case ISPREV:		  
			  if (preg > 0)
			  {
				  preg--;
			  }
			  else
			  {
				  iserrno = EENDFILE;
				  return -1;
			  }
			  break;
		   case ISCURR:
			  break;
		   case ISEQUAL:
			  r = memory_busca_reg(clave,&preg,pfi,trozo,-1);
			  if (r)
			  {
				  iserrno = ENOENT;
				  return -1;
			  }
			  break;
		   case ISGREAT:			  
			  r = memory_busca_reg(clave,&preg,pfi,trozo,0);
			  if (!r || r < 0)
			  {
				  if (preg < (plong[2]-1))
					  preg++;
				  else
				  {
					  iserrno = ENOENT;
					  return -1;
				  }
			  }
			  break;
		   case ISGTEQ:		  
			  r = memory_busca_reg(clave,&preg,pfi,trozo,1);
			  if (r < 0)
			  {
				  if (preg < (plong[2]-1))
					  preg++;
				  else
				  {
					  iserrno = ENOENT;
					  return -1;
				  }
			  }
			  break;
		   case ISLOW:		  
			  r = memory_busca_reg(clave,&preg,pfi,trozo,1);
			  if (!r || r > 0)
			  {
				  if (preg > 0)
					  preg--;
				  else
				  {
					  iserrno = ENOENT;
					  return -1;
				  }
			  }
			  break;
		   case ISLOWEQ:		  
			  r = memory_busca_reg(clave,&preg,pfi,trozo,0);
			  if (r > 0)
			  {
				  if (preg > 0)
					  preg--;
				  else
				  {
					  iserrno = ENOENT;
					  return -1;
				  }
			  }
			  break;
		   default:
			  break;
		   }
	   }
	   plong[4] = clave;
	   plong[5] = preg;
	   /*
	   if (clave == 2 && preg > 10390)
	   {
		   preg = preg;
	   }
	   */
	   if (pkey)
	   {
		   reg = pkey[preg];
	   }
	   else
	   {
		   reg = preg;
	   }
	   plong[6] = reg;
	   isrecnum = reg+1;
	   IxxNumeroPosicion = preg+1;
	   pdata = pfi->f_data + plong[0] + reg * pfi->f_lfich;
	   if (clave < pfi->f_nclaves)
	   {
		   for (i = 0;i < pfi->f_clave[clave].k_ncampos;i++)
		   {			    
				cc = pfi->f_clave[clave].k_campo[i] - 1;
				offset = pfi->f_campos[cc].c_offset;
				longs  = pfi->f_campos[cc].c_long;
				memcpy(pfi->f_buf+offset,pdata+offset,longs);
		   }
	   }
	   iserrno = 0;
	}
	return 0;
}

int memory_lee_reg(pfi,modo)
register FI_DEF *pfi;
int modo; 
{
	if (!pfi->f_data)
		return -1;	
	{
	   int clave;
	   long *plong;
	   long *pkey;
	   char *pdata;
	   long reg;
	   long preg,xreg;
	   int r = 0;	   
	   
	   plong = (long *)pfi->f_data;

	   clave = plong[4];

	   plong[7] = 0; /* se anula el "trozo" */

  	   modo &= ~ISLOCK;
	   modo &= ~ISWAIT;

	   if (!plong[2])
	   {
		   iserrno = ENOREC;
		   return -1;
	   }
	   if (clave < pfi->f_nclaves)
	   {
		   pkey = (long *)((char *)pfi->f_data + plong[1] + clave * sizeof(long) * plong[3]);	   
		   preg = plong[5];	   
	   }
	   else
	   {
		   pkey = NULL;
		   preg = plong[6];
		   switch(modo)
		   {
			   case ISEQUAL:
				  modo = ISCURR;
				  break;
			   case ISGREAT:	
				  modo = ISNEXT;
				  break;
			   case ISGTEQ:		
				  modo = ISCURR;
				  break;
			   case ISLOW:		  
				  modo = ISPREV;
				  break;
			   case ISLOWEQ:		  
			   default:
				  modo = ISCURR;
				  break;
		   }
	   }

	   if (pkey && plong[9] && (xreg = memory_busca_contenido(modo,pfi,pkey,preg,clave)) != -2)
	   {		   
		   if (xreg == -1)
		   {
			  iserrno = ENOENT;
			  return -1;
		   }
		   if (xreg == -3)
		   {
			  iserrno = EENDFILE;
			  return -1;
		   }
		   preg = xreg;
	   }
	   else
	   {
		   switch(modo)
		   {
		   case ISFIRST:		  
			  preg = 0;
			  break;
		   case ISLAST:
			  preg = plong[2]-1;
			  break;
		   case ISNEXT:
			  if (preg < (plong[2]-1))
			  {
				  preg++;
			  }
			  else
			  {
				  iserrno = EENDFILE;
				  return -1;
			  }
			  break;
		   case ISPREV:		  
			  if (preg > 0)
			  {
				  preg--;
			  }
			  else
			  {
				  iserrno = EENDFILE;
				  return -1;
			  }
			  break;
		   case ISCURR:
			  break;
		   case ISEQUAL:
			  r = memory_busca_reg(clave,&preg,pfi,0,-1);
			  if (r)
			  {
				  iserrno = ENOENT;
				  return -1;
			  }
			  break;
		   case ISGREAT:			  
			  r = memory_busca_reg(clave,&preg,pfi,0,0);
			  if (!r || r < 0)
			  {
				  if (preg < (plong[2]-1))
					  preg++;
				  else
				  {
					  iserrno = ENOENT;
					  return -1;
				  }
			  }
			  break;
		   case ISGTEQ:		  
			  r = memory_busca_reg(clave,&preg,pfi,0,1);
			  if (r < 0)
			  {
				  if (preg < (plong[2]-1))
					  preg++;
				  else
				  {
					  iserrno = ENOENT;
					  return -1;
				  }
			  }
			  break;
		   case ISLOW:		  
			  r = memory_busca_reg(clave,&preg,pfi,0,1);
			  if (!r || r > 0)
			  {
				  if (preg > 0)
					  preg--;
				  else
				  {
					  iserrno = ENOENT;
					  return -1;
				  }
			  }
			  break;
		   case ISLOWEQ:		  
			  r = memory_busca_reg(clave,&preg,pfi,0,0);
			  if (r > 0)
			  {
				  if (preg > 0)
					  preg--;
				  else
				  {
					  iserrno = ENOENT;
					  return -1;
				  }
			  }
			  break;
		   default:
			  break;
		   }
	   }
	   plong[5] = preg;
	   if (pkey)
	   {
		   reg = pkey[preg];
	   }
	   else
	   {
		   reg = preg;
	   }
	   plong[6] = reg;
	   isrecnum = reg+1;
	   IxxNumeroPosicion = preg+1;
	   pdata = pfi->f_data + plong[0] + reg * pfi->f_lfich;
	   memcpy(pfi->f_buf,pdata,pfi->f_lfich);
	   iserrno = 0;
	}
	return 0;
}


int memory_graba_nuevo(pfi)
register FI_DEF *pfi;
{
	long *plong;
	char *pdata;
	long *pkey;
	long reg;
	long preg;
	int i,j;
	int f;
	if (!pfi->f_data)
		return -1;

    plong = (long *)pfi->f_data;

	if (plong[2] >= plong[3])
	{
		iserrno = EBADFILE;
		return -1;
	}

	reg = plong[2];
	if (reg < 0 || reg >= plong[3])
	{
		iserrno = EDUPL;
		return -1;		
	}	
	
	for (i = 0;i < pfi->f_nclaves;i++)
	{
		preg = 0;		
		pkey = (long *)((char *)pfi->f_data + plong[1] + i * sizeof(long) * plong[3]);
 		f = memory_busca_reg(i,&preg,pfi,0,0);
		if (!i && !f)
		{
			iserrno = EDUPL;
			return -1;
		}
		if (preg < 0 || preg >= plong[3])
		{
			iserrno = EDUPL;
			return -1;		
		}
		if (f <= 0)
		{
			preg++;
		}
		for (j = plong[2];j > preg;j--)
		{
			pkey[j] = pkey[j-1];
		}
		pkey[preg] = reg;
	}	
	
	pdata = pfi->f_data + plong[0] + reg * pfi->f_lfich;
	memcpy(pdata,pfi->f_buf,pfi->f_lfich);
	
	plong[2]++;
	
	iserrno = 0;
	return 0;
}

static int memory_compara_la_clave(FI_DEF *pfi,int clave)
{
	char *pdata;
	long *pkey;
	long *plong = (long *)pfi->f_data;
	int campos[300];
	int tipos[300];
	int longs[300];
	int ncampos;
	int i,cc;
	int lreg = pfi->f_lfich;
	char *pbuf = pfi->f_buf;
	long preg = plong[5];	

	if (clave >= pfi->f_nclaves || preg >= plong[2]) return 0;

	pdata = pfi->f_data + plong[1] + clave * sizeof(long) * plong[3];
	pkey = (long)pdata;
	pdata = pfi->f_data + plong[0];

	ncampos = pfi->f_clave[clave].k_ncampos;
	for (i = 0;i < ncampos;i++)
	{		
		cc = pfi->f_clave[clave].k_campo[i] - 1;
		campos[i] = pfi->f_campos[cc].c_offset;
		longs[i]  = pfi->f_campos[cc].c_long;
		tipos[i]  = pfi->f_campos[cc].c_tipo;
	}
	return memory_compara_clave(pdata+pkey[preg]*lreg,pbuf,ncampos,campos,longs,tipos);
}

int memory_graba_actual(pfi)
register FI_DEF *pfi;
{
	long *plong;
	char *pdata;
	long *pkey;
	long *pkx;
	long reg;
	long preg,opreg;
	int i,j,cc;
	int f;	
	if (!pfi->f_data)
		return -1;	

    plong = (long *)pfi->f_data;

	reg = plong[6];
	pdata = pfi->f_data + plong[0] + reg * pfi->f_lfich;	
	
	for (i = 0;i < pfi->f_nclaves;i++)
	{
		if (memory_compara_la_clave(pfi,i))
		{
			preg = 0;
			pkey = (long *)((char *)pfi->f_data + plong[1] + i * sizeof(long) * plong[3]);
			opreg = -1;
			pkx = pkey;			   
			for (cc = 0;cc < plong[2];cc++,pkx++)
			{
			    if (*pkx == reg)
				{
				   opreg = cc;
				   break;
				}
			}
			f = memory_busca_reg(i,&preg,pfi,0,0);
			if (!f && pkey[preg] == reg)
			{
				continue;
			}
			if (f <= 0)
			{
				preg++;
			}
			if (opreg < 0)
			{
				pkey[preg] = reg;
				continue;
			}
			if (opreg > preg)
			{			
				for (j = opreg;j > preg;j--)
				{
					pkey[j] = pkey[j-1];
				}
			}
			else
			{
				for (j = opreg;j < preg;j++)
				{
					pkey[j] = pkey[j+1];
				}
			}
			pkey[preg] = reg;
		}
	}
	memcpy(pdata,pfi->f_buf,pfi->f_lfich);
	
	iserrno = 0;
	return 0;
}

int memory_borra_actual(pfi)
register FI_DEF *pfi;
{
	long *plong;
	char *pdata;
	char *pdata2;
	long *pkey;
	long *pkx;
	long reg;
	long preg;
	long plast;
	int i,j,cc;
	int f;	
	if (!pfi->f_data)
		return -1;

    plong = (long *)pfi->f_data;

	if (!plong[2])
	{
		iserrno = ENOREC;
		return -1;
	}

	reg = plong[6];
	pdata = pfi->f_data + plong[0] + reg * pfi->f_lfich;	
	plong[2]--;
	
	for (i = 0;i < pfi->f_nclaves;i++)
	{		
		pkey = (long *)((char *)pfi->f_data + plong[1] + i * sizeof(long) * plong[3]);			
		preg = -1;
		plast = -1;
		pkx = pkey;			   
		for (cc = 0;cc <= plong[2];cc++,pkx++)
		{
			if (*pkx == reg)
			{
			   preg = cc;
			   if (plast != -1 || reg == plong[2])
				 break;
			}
			if (*pkx == plong[2])
			{
			   plast = cc;
			   if (preg != -1)
				 break;
			}
		}
		if (preg < 0)
		{
			continue;
		}
		if (reg != plong[2] && plast != -1)
		{
			pkey[plast] = reg;
		}
		for (j = preg;j < plong[2];j++)
		{
			pkey[j] = pkey[j+1];
		}		
	}
    pdata2 = pfi->f_data + plong[0] + plong[2] * pfi->f_lfich;	
    if (reg != plong[2])
	{		
		memcpy(pdata,pdata2,pfi->f_lfich);		
	}
	plong[5] = 0;
	plong[6] = 0;
	memset(pdata2,0,pfi->f_lfich);	
	iserrno = 0;
	return 0;
}

int memory_borra_reg(pfi)
register FI_DEF *pfi;
{
	int r;
	long *plong;
	long *pkey;
	long regcurr;	
	long pcurr;
	int cc;
	if (!pfi->f_data)
		return -1;
	plong = (long *)pfi->f_data;
	pcurr = plong[5];
	regcurr = plong[6];	
	if (memory_apunta_ind(pfi,-1,ISEQUAL,0) < 0)
	{
		return -1;
	}
	r = memory_borra_actual(pfi);
	if (plong[4] < pfi->f_nclaves)
	{
		pkey = (long *)((char *)pfi->f_data + plong[1] + plong[4] * sizeof(long) * plong[3]);
		for (cc = 0;cc < plong[2];cc++,pkey++)
		{
		    if (*pkey == regcurr)
			{
			   pcurr = cc;			   
			   break;
			}
		}
	}
	plong[5] = pcurr;
	plong[6] = regcurr;
	return r;
}

long memory_da_registo_actual(pfi)
FI_DEF *pfi;
{
	long *plong;
	if (!pfi->f_data)
		return -1;	
	plong = (long *)pfi->f_data;
	return plong[6]+1;
}

int memory_pon_registro_actual(pfi,r)
FI_DEF *pfi;
long r;
{
	long *plong;
	if (!pfi->f_data)
		return -1;
	plong = (long *)pfi->f_data;
	plong[6] = r-1;
	return 0;
}

int memory_clave_activa(pfi)
FI_DEF *pfi;
{
	long *plong;
	int clave;
	if (!pfi->f_data)
		return -1;
	plong = (long *)pfi->f_data;
	clave = plong[4];
	if (clave < pfi->f_nclaves)
		return clave+1;
	return 0;
}

int memory_lee_soloindice(pfi,modo)
register FI_DEF *pfi;
int modo;
{	
	return memory_apunta_ind(pfi,-1,modo,0);	
}

long memory_tam_btrv(pfi)
register FI_DEF *pfi;
{
	long *plong;
	if (!pfi->f_data)
		return -1;	
	plong = (long *)pfi->f_data;
	return plong[2];
}

int memory_pon_contenido(FI_DEF *pfi,unsigned char *con,int len)
{
	long *plong;
	char *pdata;
	int i;
	if (!pfi->f_data)
		return -1;	
	
	plong = (long *)pfi->f_data;
	pdata = pfi->f_data + plong[1] - plong[8];
	if ((len + plong[9] + 1) > plong[8])
	{
		return -1;
	}
	pdata += plong[9];
	for (i = 0;i < len;i++)
	{
		if (con[i] == '\\' && i < (len-1) && con[i+1] == '\\')
		{
			*pdata = 0;
			i++;			
		}
		else
		{
			*pdata = con[i];
		}
		pdata++;
		plong[9]++;
	}
	*pdata = 0;
	plong[9]++;
	return 0;
}


