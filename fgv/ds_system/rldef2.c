
/* Autor: Francisco Gonzalez Valiente */
#include <fgvsys.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef O_BINARY
#include <io.h>
#endif

#include <stdio.h>
#include <string.h>
#include <malloc.h>

#ifndef O_BINARY  /* compatibilidad xenix - dos */

#define O_BINARY 0
#define SEEK_SET 0
#define SEEK_CUR 1
#ifndef LARGESEEK
extern long rw_lseek_();
#endif
extern unsigned int rw_read_();

#endif


#include <rwmalloc.h>
#define LOS_DEF
#define _NUEVODEF
#include <rdef.h>


int flag_leedefn = 0;

extern long nume_serie;
extern unsigned short local_term[52];
extern unsigned short local_term_[52];

int es_un_caso_especial = 1;
extern int fecha_protegida; /* control */

extern char *quita_path();


static int lee_texto(file1,texto)
int file1;
char *texto;
{
   int j;
   for(j = 0;j < 250;j++) {
      rw_read_(file1,texto+j,1);
      if ( !*(texto+j) ) break;
   }
   return(j);
}


int copia_defn(FI_DEF **_fi,FI_DEF *fo)
{
	FI_DEF *fi;
	int i,j;
	char *apunta,*final;

    fi = (FI_DEF *)reserva_malloc(sizeof(FI_DEF),0);
	memcpy((void *)fi,(void *)fo,sizeof(FI_DEF));
    fi->f_campogids = NULL;
    fi->f_cactual= NULL;
    fi->f_data = NULL;


	*_fi = fi;   

	fi->f_pantalla = (PAN_DEF *)malloc(sizeof(PAN_DEF) * fi->f_npanta);
	memcpy((void *)fi->f_pantalla,(void *)fo->f_pantalla,sizeof(PAN_DEF) * fi->f_npanta);
    fi->f_campos = (CAMPO_DEF *)reserva_malloc(sizeof(CAMPO_DEF) * fi->f_ncampos,2);
    memcpy((char *)fi->f_campos,(char *)fo->f_campos,sizeof(CAMPO_DEF) * fi->f_ncampos);
    fi->f_clave = (KEY_DEF *)reserva_malloc(sizeof(KEY_DEF) * fi->f_nclaves,3);
    memcpy((char *)fi->f_clave,(char *)fo->f_clave,sizeof(KEY_DEF) * fi->f_nclaves);
    for (i=0;i<fi->f_nclaves;i++) 
	{        
        fi->f_clave[i].k_campo = (short *)malloc(sizeof(short) * fi->f_clave[i].k_ncampos);
        memcpy((char *)fi->f_clave[i].k_campo,(char *)fo->f_clave[i].k_campo,sizeof(short) * fi->f_clave[i].k_ncampos);                
        fi->f_clave[i].k_orden = (short *)malloc(sizeof(short) * fi->f_clave[i].k_ncampos);
        memcpy((char *)fi->f_clave[i].k_orden,(char *)fo->f_clave[i].k_orden,sizeof(short) * fi->f_clave[i].k_ncampos);        
    }
         
    fi->f_relaci = (REL_DEF *)reserva_malloc(sizeof(REL_DEF) * fi->f_nrelaci,4);
    memcpy((char *)fi->f_relaci,(char *)fo->f_relaci,sizeof(REL_DEF) * fi->f_nrelaci);

    
	if (fi->f_lenmem)
	{
		apunta = reserva_malloc(fi->f_lenmem,5);
		if (apunta == NULL) 
		{
			return(-1);
		}
		memcpy(apunta,fo->f_titulo,fi->f_lenmem-sizeof(short));         
		/* todos los punteros  nulos apuntaran aqui */
		final = (apunta+(fi->f_lenmem-sizeof(short)));
		memset(final,0,sizeof(short));
	}
	else
	{
		apunta = NULL;
		final = "";
	}
    
	if (apunta)
	{
	    fi->f_titulo = apunta;
        apunta += (strlen(apunta)+1);         
	}
	else
	{
		j = strlen(fo->f_titulo)+1; 
		fi->f_titulo = malloc(j);
		strcpy(fi->f_titulo,fo->f_titulo);
	}
         
    for (i=0;i<fi->f_ncampos;i++) 
	{        
        fi->f_campos[i].c_nombre = final; /* no se usa */        
        fi->f_campos[i].c_mante.c_relacion = final;  /* no se usa */        
        fi->f_campos[i].c_lista = final;        

        fi->f_campos[i].c_usi = strmalloc(strlen(fo->f_campos[i].c_usi)+1);
        strcpy(fi->f_campos[i].c_usi,fo->f_campos[i].c_usi);        

		if (apunta)
		{
			apunta += (strlen(apunta)+1);
			fi->f_campos[i].c_mante.c_mensaje = apunta;        
			apunta += (strlen(apunta)+1);
			fi->f_campos[i].c_mante.c_defecto = apunta;        
			apunta += (strlen(apunta)+1);
		}
		else
		{
			fi->f_campos[i].c_mante.c_mensaje = final;
			fi->f_campos[i].c_mante.c_defecto = final;        
		}
        
        fi->f_campos[i].c_minimo = strmalloc(strlen(fo->f_campos[i].c_minimo)+1);
        strcpy(fi->f_campos[i].c_minimo,fo->f_campos[i].c_minimo);

		if (apunta)
			apunta += (strlen(apunta)+1);

        fi->f_campos[i].c_maximo = strmalloc(strlen(fo->f_campos[i].c_maximo)+1);
        strcpy(fi->f_campos[i].c_maximo,fo->f_campos[i].c_maximo);

		if (apunta)
			apunta += (strlen(apunta)+1);
     }
     
	 if (apunta)
	 {
		 for (i=0;i<fi->f_nrelaci;i++) 
		 {
			fi->f_relaci[i].r_fichero = apunta;
			apunta += (strlen(fi->f_relaci[i].r_fichero)+1);
			fi->f_relaci[i].r_mfichero = apunta;
			apunta += (strlen(fi->f_relaci[i].r_mfichero)+1);
			fi->f_relaci[i].r_campo = (short *)apunta;
			for (j = 0;j < (MAXCAMPOSPORCLAVE +1);j++) 
			{
			   if (*( (short *)apunta ) == 0) {apunta += sizeof(short);break;}
			   apunta += sizeof(short);
			}
		 }
	 }
	 else
		 fi->f_nrelaci = 0;


     for (i=0;i<fi->f_ncampos;i++) 
     {
        j = strlen(fo->f_campos[i].c_nombre);
        fi->f_campos[i].c_nombre = strmalloc(j+1);
        strcpy(fi->f_campos[i].c_nombre,fo->f_campos[i].c_nombre);      
        
		j = strlen(fo->f_campos[i].c_lista);
        fi->f_campos[i].c_lista = strmalloc(j+1);
        strcpy(fi->f_campos[i].c_lista,fo->f_campos[i].c_lista);
        
        j = strlen(fo->f_campos[i].c_mante.c_relacion);    
        fi->f_campos[i].c_mante.c_relacion = strmalloc(j+1); /* para el free_def */
        strcpy(fi->f_campos[i].c_mante.c_relacion,fo->f_campos[i].c_mante.c_relacion);        
     }        
	 
	 if (fo->f_buf)
	 {        
        if (!(fi->f_buf = malloc(fi->f_lfich + 1))) 
		{
           free((char *)fi);
           *_fi = NULL;
            return(-1);
        }
     }         
     fi->f_fd = -1; /* inicializo el file descriptor a not open */         
         

     fi->f_ipfi = -1;         

     return 0;
}


int lee_defn(nomfi,_fi)
char *nomfi;
FI_DEF **_fi;
{
   FI_DEF *fi;
   int i;
   unsigned short tamano = 0;
   char *apunta,*orip;
   char *final;
   int j,k;
   int file1;
   char texto[256];
   unsigned short dato_e;
   unsigned short dato;
   unsigned long n_s;
   unsigned short tipo_de_def    = 0xffff;
   unsigned short version_de_def = 0xffff;
   int control_s = 0;
   short i_nume_code;
   short i_nume_mochi;
   long posi;
   long posi2;
   int libre = 0;
#ifdef ESMOTOROLA 
   short _m_i = 0;
#endif
   
   if (*nomfi == 5)
   {
	   libre = 1;
	   nomfi++;
   }   
   strcpy(texto,nomfi);
   if (strbusca(nomfi,".mas") < 0)
       strcat(texto,".mas");
   
   if ((file1 = rw_open_(texto,O_BINARY | O_RDONLY)) < 0) {
	   flag_leedefn = 0;
      return(-1);
			}
   
   rw_lseek_(file1,0L,SEEK_SET);
   
   rw_read_(file1,(char *)&dato,sizeof(short));
   moto_short(&dato);
   if (dato != FGVGEN_ID_MAS)
			{
      rw_close_(file1);
	  flag_leedefn = 0;
      return(-1);
			}
   
   rw_read_(file1,(char *)&dato,sizeof(short));
   moto_short(&dato);
   rw_read_(file1,(char *)&dato_e,sizeof(short));
   moto_short(&dato_e);
   dato ^= dato_e;
   i_nume_code = dato;
#ifdef AGIMAXPROTEC
   if (!es_un_caso_especial && dato != nume_code())
			{
      control_s++;
      /*
      rw_close_(file1);
      return(-1);
      */
			}
#endif
   
   rw_read_(file1,(char *)&dato,sizeof(short));
   moto_short(&dato);
   n_s = (long)(dato ^ dato_e) * 0xffffL;
   rw_read_(file1,(char *)&dato,sizeof(short));
   moto_short(&dato);
   n_s += (dato ^ dato_e);
#ifdef AGIMAXPROTEC
   if (!es_un_caso_especial && n_s != nume_serie)
			{
      control_s++;
      /*
      rw_close_(file1);
      return(-1);
      */
			}
#endif
   rw_read_(file1,(char *)&dato,sizeof(short));
   moto_short(&dato);
   dato = dato ^ dato_e;
   i_nume_mochi = dato;
#ifdef AGIMAXPROTEC
   if (!es_un_caso_especial && dato != _Posicion())
			{
      control_s++;
      /*
      rw_close_(file1);
      return(-1);
      */
			}
#endif
   rw_read_(file1,(char *)&dato,sizeof(short));
   moto_short(&dato);
   tipo_de_def = dato ^ dato_e;
   rw_read_(file1,(char *)&dato,sizeof(short));
   moto_short(&dato);
   version_de_def = dato ^ dato_e;
   
   if (!libre && tipo_de_def != 850)
   {
	   if (tipo_de_def == 0xffff &&  version_de_def == 0xffff) control_s = 0;
	   for (i = 0;i < 50;i++)
				{
		  if (tipo_de_def == local_term[i] && version_de_def == local_term_[i]) break;
				}
	   if (!fecha_protegida && (control_s || (!es_un_caso_especial && i >= 50)))
				{
		  if (control_s || local_term[50] > tipo_de_def || local_term[51] < tipo_de_def)
		  {
			 rw_close_(file1);
			 flag_leedefn = 0;
			 return(-1);
		  }
				}
   }
   else
	   control_s = 0;
   
   rw_lseek_(file1,48L,SEEK_SET);
   
   rw_read_(file1,(char *)&tamano,sizeof(short));
   moto_short(&tamano);
   
   fi = (FI_DEF *)reserva_malloc(sizeof(FI_DEF),0);
   memset((char *)fi,0,sizeof(FI_DEF));
   fi->f_campogids = NULL;
   fi->f_cactual= NULL;
   fi->f_data = NULL;
   *_fi = fi;   

   rw_read_(file1,(char *)fi,sizeof(FI_DEF_LEE));
   memcpy(fi->f_oldfichero,fi->f_fichero,sizeof(fi->f_fichero));

   tamano -= (sizeof(FI_DEF_LEE) + 48);
   fi->tipo_datos = -1;

   strcpy(fi->f_PathDef,nomfi);
   
			{
            char *ext;
            int pex;
            /* analizar extension */
            pex = strlen(fi->f_fichero);
            ext = fi->f_fichero + pex + 1;
            if (*ext == 1 && *(ext+1) == 2 && *(ext+2) == 3 && *(ext+3) == '.')
            {
               ext += 3;  
               if (!strcmp(ext,".dat"))
               {
                  fi->tipo_datos = 0;
               }
               else
                  if (!strcmp(ext,".dbs"))
                  {
                     fi->tipo_datos = 1;
                  }
                  else
                     if (!strcmp(ext,".db3"))
                     {
                        fi->tipo_datos = 2;
                     }
                     else
                        if (!strcmp(ext,".dbp"))
                        {
                           fi->tipo_datos = 3;
                        }
                        else
                           if (!strcmp(ext,".odb"))
                           {
                              fi->tipo_datos = 4;
                           }		
            }
         }
         
		 fi->f_orig_ncampos = fi->f_ncampos;
         moto_short(&(fi->f_lfich));
         moto_short(&(fi->f_npanta));
         moto_short(&(fi->f_nclaves));
         moto_short(&(fi->f_ncampos));
         moto_short(&(fi->f_nrelaci));
         moto_short(&(fi->f_fd));
         
         if (!es_un_caso_especial && fi->f_fd != (short)tipo_de_def)
         {
            reserva_free((char *)fi,0);
            *_fi = NULL;
            rw_close_(file1);
			flag_leedefn = 0;
            return(-1);
         }
         
         fi->f_fd = -1;
         
         
         
         fi->f_pantalla = (PAN_DEF *)reserva_malloc(sizeof(PAN_DEF) * fi->f_npanta,1);
         rw_read_(file1,(char *)fi->f_pantalla,sizeof(PAN_DEF) * fi->f_npanta);
         tamano -= (sizeof(PAN_DEF) * fi->f_npanta);
         
#ifdef ESMOTOROLA
         for(_m_i = 0;_m_i < fi->f_npanta;_m_i++)
         {
            moto_short(&(fi->f_pantalla[_m_i].p_campoini));
            moto_short(&(fi->f_pantalla[_m_i].p_campofin));
            moto_short(&(fi->f_pantalla[_m_i].p_posfin));
            moto_short(&(fi->f_pantalla[_m_i].p_posref));
         }
#endif
         
         fi->f_campos = (CAMPO_DEF *)reserva_malloc(sizeof(CAMPO_DEF) * fi->f_ncampos,2);
         rw_read_(file1,(char *)fi->f_campos,sizeof(CAMPO_DEF) * fi->f_ncampos);
         tamano -= (sizeof(CAMPO_DEF) * fi->f_ncampos);
         
         fi->f_clave = (KEY_DEF *)reserva_malloc(sizeof(KEY_DEF) * fi->f_nclaves,3);
         rw_read_(file1,(char *)fi->f_clave,sizeof(KEY_DEF) * fi->f_nclaves);
         tamano -= (sizeof(KEY_DEF) * fi->f_nclaves);
         
         for (i=0;i<fi->f_nclaves;i++) {
            
            moto_short(&(fi->f_clave[i].k_ncampos));
            moto_short(&(fi->f_clave[i].k_fflag));
            
            fi->f_clave[i].k_campo = (short *)malloc(sizeof(short) * fi->f_clave[i].k_ncampos);
            rw_read_(file1,(char *)fi->f_clave[i].k_campo,sizeof(short) * fi->f_clave[i].k_ncampos);
            tamano -= (sizeof(short) * fi->f_clave[i].k_ncampos);
            
            fi->f_clave[i].k_orden = (short *)malloc(sizeof(short) * fi->f_clave[i].k_ncampos);
            rw_read_(file1,(char *)fi->f_clave[i].k_orden,sizeof(short) * fi->f_clave[i].k_ncampos);
            tamano -= (sizeof(short) * fi->f_clave[i].k_ncampos);
            
#ifdef ESMOTOROLA
            for(_m_i = 0;_m_i < fi->f_clave[i].k_ncampos;_m_i++)
            {
               moto_short(&(fi->f_clave[i].k_campo[_m_i]));
               moto_short(&(fi->f_clave[i].k_orden[_m_i]));
            }
#endif
         }
         
         fi->f_relaci = (REL_DEF *)reserva_malloc(sizeof(REL_DEF) * fi->f_nrelaci,4);
         rw_read_(file1,(char *)fi->f_relaci,sizeof(REL_DEF) * fi->f_nrelaci);
         tamano -= (sizeof(REL_DEF) * fi->f_nrelaci);
         
         /* segundo bloque */
         apunta = reserva_malloc(tamano,5);
		 fi->f_lenmem = tamano;
         if (apunta == NULL) 
         {
			 flag_leedefn = 0;
             rw_close_(file1);return(-1);
         }
         rw_read_(file1,apunta,(tamano-sizeof(short)));
         /* todos los punteros  nulos apuntaran aqui */
         final = (apunta+(tamano-sizeof(short)));
         memset(final,0,sizeof(short));
         
         fi->f_titulo = apunta;
         apunta += (strlen(fi->f_titulo)+1);
         
         for (i=0;i<fi->f_ncampos;i++) {
            moto_short(&(fi->f_campos[i].c_qc));
            moto_short(&(fi->f_campos[i].c_offset));
            moto_short(&(fi->f_campos[i].c_long));
            moto_short(&(fi->f_campos[i].c_tipo));
            moto_short(&(fi->f_campos[i].c_key));
            moto_short(&(fi->f_campos[i].c_fflag));
            moto_short(&(fi->f_campos[i].c_mante.c_actual));
            moto_short(&(fi->f_campos[i].c_mante.c_modif));
            moto_short(&(fi->f_campos[i].c_mante.c_visual));
            moto_short(&(fi->f_campos[i].c_mante.c_lineal));
            moto_short(&(fi->f_campos[i].c_mante.c_qp));
            moto_short(&(fi->f_campos[i].c_mante.c_qpl));
            
            fi->f_campos[i].c_nombre = final; /* no se usa */
            
            
            fi->f_campos[i].c_mante.c_relacion = final;  /* no se usa */
            
            fi->f_campos[i].c_lista = final;
            
            fi->f_campos[i].c_usi = strmalloc(strlen(apunta)+1);
            strcpy(fi->f_campos[i].c_usi,apunta);
            
            apunta += (strlen(fi->f_campos[i].c_usi)+1);
            fi->f_campos[i].c_mante.c_mensaje = apunta;
            
            
            apunta += (strlen(fi->f_campos[i].c_mante.c_mensaje)+1);
            fi->f_campos[i].c_mante.c_defecto = apunta;
            
            
            apunta += (strlen(fi->f_campos[i].c_mante.c_defecto)+1);
            
            fi->f_campos[i].c_minimo = strmalloc(strlen(apunta)+1);
            strcpy(fi->f_campos[i].c_minimo,apunta);
            apunta += (strlen(fi->f_campos[i].c_minimo)+1);
            
            fi->f_campos[i].c_maximo = strmalloc(strlen(apunta)+1);
            strcpy(fi->f_campos[i].c_maximo,apunta);
            apunta += (strlen(fi->f_campos[i].c_maximo)+1);
            
         }
         
         for (i=0;i<fi->f_nrelaci;i++) {
            
            moto_short(&(fi->f_relaci[i].r_obliga));
            moto_short(&(fi->f_relaci[i].r_leer));
            
            fi->f_relaci[i].r_fichero = apunta;
            apunta += (strlen(fi->f_relaci[i].r_fichero)+1);
            fi->f_relaci[i].r_mfichero = apunta;
            apunta += (strlen(fi->f_relaci[i].r_mfichero)+1);
#ifdef ESMOTOROLA
            fi->f_relaci[i].r_campo = (short *)malloc(sizeof(short) * (MAXCAMPOSPORCLAVE +1));
#else
            fi->f_relaci[i].r_campo = (short *)apunta;
#endif
            for (j = 0;j < (MAXCAMPOSPORCLAVE +1);j++) {
#ifdef ESMOTOROLA
               memcpy(&_m_i,apunta,2);
               moto_short(&_m_i);
               fi->f_relaci[i].r_campo[j] = _m_i;
               if (!_m_i) {apunta += sizeof(short);break;}
#else
               if (*( (short *)apunta ) == 0) {apunta += sizeof(short);break;}
#endif
               apunta += sizeof(short);
            }
         }
         
         
         posi = rw_lseek_(file1,0L,SEEK_CUR);
         posi2 = rw_lseek_(file1,0L,SEEK_END);
         apunta = malloc((int)(posi2-posi+1));
         orip = apunta;
         rw_lseek_(file1,posi,SEEK_SET);
         rw_read_(file1,apunta,(int)(posi2-posi));
         for (i=0;i<fi->f_ncampos;i++) 
         {
            j = strlen(apunta);    
            fi->f_campos[i].c_nombre = strmalloc(j+1);
            strcpy(fi->f_campos[i].c_nombre,apunta);
            apunta += (j+1);
            
            j = strlen(apunta);
            if (*apunta != 13 || !*(apunta+1))
            {   
               /* ajustar compatibilidad con versiones anteriores */
               sprintf(texto,"C%d",i);
               fi->f_campos[i].c_lista = strmalloc(11);
               strcpy(fi->f_campos[i].c_lista,texto);
            }
            else
            {
               fi->f_campos[i].c_lista = strmalloc(j);
               strcpy(fi->f_campos[i].c_lista,apunta+1);
               if (j > 11)
               {
                  fi->f_campos[i].c_lista[10] = 0;
                  quitab(fi->f_campos[i].c_lista);
               } 
            }
            /*
            if (!j)
            {
            strcpy(texto,fi->f_campos[i].c_nombre);
            texto[10] = 0;
            quitab(texto);
            for (k = 0;k < i;k++)
            {
            if (!strcmp(fi->f_campos[k].c_lista,texto))
            {
            sprintf(texto,"C%d",i);
            break;
            }
            }
            fi->f_campos[i].c_lista = strmalloc(11);
            strcpy(fi->f_campos[i].c_lista,texto);	  
            }
            else
            {
            fi->f_campos[i].c_lista = strmalloc(j+1);
            strcpy(fi->f_campos[i].c_lista,apunta);
            if (j > 10)
            {
            fi->f_campos[i].c_lista[10] = 0;
            quitab(fi->f_campos[i].c_lista);
            }
            }
            */
            apunta += (j+1);
            
            j = strlen(apunta);    
            if (flag_leedefn == 2)
            {
               fi->f_campos[i].c_mante.c_relacion = strmalloc(j+1); /* para el free_def */
               strcpy(fi->f_campos[i].c_mante.c_relacion,apunta);
            }
            else
            {
               fi->f_campos[i].c_mante.c_relacion = strmalloc(1);
            }    
            apunta += (j+1);
         }
         free(orip);
         
         if (flag_leedefn == 2)
         {
            flag_leedefn = 1;
         }
         
         
         if (flag_leedefn == 1)
         {
            fi->f_buf = NULL;
            flag_leedefn = 0;
         }
         else
         {
            if (!(fi->f_buf = malloc(fi->f_lfich + 1))) {
               free((char *)fi);
               *_fi = NULL;
               rw_close_(file1);
               return(-1);
            }
         }
         
         fi->f_fd = -1; /* inicializo el file descriptor a not open */

		 fi->f_ipfi = -1;         
         
         rw_close_(file1);

         return(0);
}


void free_defpri(fi)
FI_DEF *fi;
{
   int i;
   if (fi) 
   {  
  	  



      for (i = 0;i < fi->f_ncampos;i++)
      {
         free(fi->f_campos[i].c_maximo);
         free(fi->f_campos[i].c_minimo);
         free(fi->f_campos[i].c_usi);
         free(fi->f_campos[i].c_nombre);
         free(fi->f_campos[i].c_lista);
         free(fi->f_campos[i].c_mante.c_relacion); 		  
      }

	  
      if (fi->f_buf)
         free(fi->f_buf);

      if (fi->f_campogids)
         free((char *)fi->f_campogids);	  

	  if (fi->f_cactual)
		 free((char *)fi->f_cactual);	  

	  if (fi->f_data)
	  {
		 free(fi->f_data);
	  }

      reserva_free((char *)fi->f_pantalla,1);

	  reserva_free((char *)fi->f_campos,2);

	  for (i = 0;i < fi->f_nclaves;i++) {
         free((char *)fi->f_clave[i].k_campo);
         free((char *)fi->f_clave[i].k_orden);
      }
#ifdef ESMOTOROLA
      for (i = 0;i < fi->f_nrelaci;i++) 
	  {
         free((char *)fi->f_relaci[i].r_campo);
      }
#endif
	  
      reserva_free((char *)fi->f_clave,3);
	  
      reserva_free((char *)fi->f_relaci,4);
	  
      reserva_free((char *)fi->f_titulo,5); /* el famoso apunta */
	  
      reserva_free((char *)fi,0);
   }
}


#ifndef min
#define min(a,b)  (((a) < (b)) ? (a) : (b))
#endif
#ifndef max
#define max(a,b)  (((a) > (b)) ? (a) : (b))
#endif


static char tmp_usi[256];

static void calcula_un_usi(FI_DEF *pfi,int c)
{
   char *minstr = pfi->f_campos[c].c_minimo;
   char *maxstr = pfi->f_campos[c].c_maximo;
   char digitos[40];
   int decimal;
   
   
   if (strlen(minstr) > 1 && minstr[0] == '0') 
			{
      digitos[0] = '0';
      sprintf(digitos+1,"%d",len_numerico(minstr,maxstr));
			}
   else
      sprintf(digitos,"%d",len_numerico(minstr,maxstr));
   
   decimal = max(decimales(maxstr),decimales(minstr));
   sprintf(tmp_usi,"%%%s.%dlf",digitos,decimal);
   
}


void calcula_usi(FI_DEF *pfi,int c)
{
   free(pfi->f_campos[c].c_usi);
   
   calcula_un_usi(pfi,c);
   
   pfi->f_campos[c].c_usi = malloc(strlen(tmp_usi)+1);
   strcpy(pfi->f_campos[c].c_usi,tmp_usi);
}


char *recalcula_usi(FI_DEF *pfi,int campo)
{
   int i,l;
   char *u = pfi->f_campos[campo].c_usi;
   char *p = pfi->f_campos[campo].c_minimo;
   if (pfi->f_campos[campo].c_tipo != DOUBLETIPO)
      return(u);
   l = 0;
   for (i = 0;p[i];i++)
			{
      if (p[i] == '.') l++;
      if (l && (p[i] == '?' || p[i] == '!')) {break;}
			}
   if (!p[i]) return(u);
   
   calcula_un_usi(pfi,campo);
   return(tmp_usi);
}



#ifdef ESMOTOROLA
static unsigned int graba_short(file,p,n)
int file;
short *p;
int n;
{
	short tmp;
	unsigned int r = 0;
	
	while(n > 0 && r >= 0)
	{
		tmp = *p;
		moto_short(&tmp);  	
		r += rw_write_(file,&tmp,2);
		p++;
		n -= 2;
	}
	return(r);
}
#else
static unsigned int graba_short(file,p,n)
int file;
char *p;
int n;
{
	return(rw_write_(file,p,n));
}
#endif


int graba_defn(nfi,fi)
char *nfi;
FI_DEF *fi;
{	
	char nomfi[MAXPATH];
	short i;
	unsigned short tamano = 0;
	unsigned short dato_e;
	unsigned short dato;
	unsigned long n_s;
	char *nulo = NULL;
    char *p;
	unsigned short listable = 0,generable = 0,tipo_de_def = 0xffff,version_de_def = 0xffff;
	char modulo[10];	
	long posicion;
	int j,file1;
	char *tmp;
	

	modulo[0] = 0;
	
	strcpy(nomfi,nfi);
	if ( !(p = strstr(nomfi,".mas")) || *(p+4) )
		strcat(nomfi,".mas");
	if ((file1 = rw_open_(nomfi,O_CREAT | O_TRUNC | O_BINARY | O_RDWR,
		S_IREAD | S_IWRITE)) < 0) 
	{
		if (fi->f_fd > -1) fi->f_fd = -1;
		return(-1);
	}	
	
	dato_e = FGVGEN_ID_MAS;
	tamano += (unsigned int)graba_short(file1,(char *)&dato_e,sizeof(short));
	dato_e = rand();
	tamano += (unsigned int)graba_short(file1,(char *)&dato_e,sizeof(short));
	dato_e = nume_code() ^ dato_e;
	tamano += (unsigned int)graba_short(file1,(char *)&dato_e,sizeof(short));
	n_s = nume_serie / 0xffff;
	dato = ((unsigned short)n_s) ^ dato_e;
	tamano += (unsigned int)graba_short(file1,(char *)&dato,sizeof(short));
	n_s = nume_serie % 0xffff;
	dato = ((unsigned short)n_s) ^ dato_e;
	tamano += (unsigned int)graba_short(file1,(char *)&dato,sizeof(short));
	dato = _Posicion() ^ dato_e;
	tamano += (unsigned int)graba_short(file1,(char *)&dato,sizeof(short));
	dato = tipo_de_def ^ dato_e;
	tamano += (unsigned int)graba_short(file1,(char *)&dato,sizeof(short));
	dato = version_de_def ^ dato_e;
	tamano += (unsigned int)graba_short(file1,(char *)&dato,sizeof(short));
	{
		char tmpxx[17];
		memset(tmpxx,0,17);
		tamano += (unsigned int)rw_write_(file1,tmpxx,17);
		strcpy(tmpxx,modulo);
		tamano += (unsigned int)rw_write_(file1,tmpxx,9);
	}   
	dato_e = FGVGEN_ID_MAS;
	tamano += (unsigned int)graba_short(file1,(char *)&dato_e,sizeof(short)); 
	tamano += (unsigned int)graba_short(file1,(char *)&listable,sizeof(short));
	tamano += (unsigned int)graba_short(file1,(char *)&generable,sizeof(short));
	
	posicion = rw_lseek_(file1,0L,SEEK_CUR);
	
	tamano += (unsigned int)graba_short(file1,(char *)&tamano,sizeof(short));
	
	tamano += (unsigned int)rw_write_(file1,fi->f_fichero,MAXPATH+1);
	tamano += (unsigned int)rw_write_(file1,(char *)&nulo,sizeof(char *));
	tamano += (unsigned int)rw_write_(file1,(char *)&nulo,sizeof(char *));
	tamano += (unsigned int)rw_write_(file1,(char *)&nulo,sizeof(char *));
	tamano += (unsigned int)rw_write_(file1,(char *)&nulo,sizeof(char *));
	tamano += (unsigned int)rw_write_(file1,(char *)&nulo,sizeof(char *));
	tamano += (unsigned int)rw_write_(file1,(char *)&nulo,sizeof(char *));
	tamano += (unsigned int)graba_short(file1,(char *)&fi->f_lfich,sizeof(short));
	tamano += (unsigned int)graba_short(file1,(char *)&fi->f_npanta,sizeof(short));
	tamano += (unsigned int)graba_short(file1,(char *)&fi->f_nclaves,sizeof(short));
	tamano += (unsigned int)graba_short(file1,(char *)&fi->f_ncampos,sizeof(short));
	tamano += (unsigned int)graba_short(file1,(char *)&fi->f_nrelaci,sizeof(short));
	tamano += (unsigned int)graba_short(file1,(char *)&tipo_de_def,sizeof(short));	

	
	for (i = 0;i<fi->f_npanta;i++) 
	{
		moto_short(&(fi->f_pantalla[i].p_campoini));
		moto_short(&(fi->f_pantalla[i].p_campofin));
		moto_short(&(fi->f_pantalla[i].p_posfin));
		moto_short(&(fi->f_pantalla[i].p_posref));
		tamano += (unsigned int)rw_write_(file1,(char *)(fi->f_pantalla+i),
			sizeof(PAN_DEF)); /* la de pantallas es igual */
		moto_short(&(fi->f_pantalla[i].p_campoini));
		moto_short(&(fi->f_pantalla[i].p_campofin));
		moto_short(&(fi->f_pantalla[i].p_posfin));
		moto_short(&(fi->f_pantalla[i].p_posref));
	}
	
	
	for (i=0;i<fi->f_ncampos;i++) 
	{
		tamano += (unsigned int)rw_write_(file1,(char *)&nulo,
			sizeof(char *));
		tamano += (unsigned int)rw_write_(file1,(char *)&nulo,
			sizeof(char *));
		tamano += (unsigned int)rw_write_(file1,(char *)&nulo,
			sizeof(char *));
		tamano += (unsigned int)rw_write_(file1,(char *)&nulo,
			sizeof(char *));
		tamano += (unsigned int)rw_write_(file1,(char *)&nulo,
			sizeof(char *));
		tamano += (unsigned int)rw_write_(file1,(char *)&nulo,
			sizeof(char *));
		tamano += (unsigned int)rw_write_(file1,(char *)&nulo,
			sizeof(char *));
		tamano += (unsigned int)rw_write_(file1,(char *)&nulo,
			sizeof(char *));
		tamano += (unsigned int)graba_short(file1,(char *)&fi->f_campos[i].c_mante.c_actual,
			sizeof(short));
		tamano += (unsigned int)graba_short(file1,(char *)&fi->f_campos[i].c_mante.c_modif,
			sizeof(short));
		tamano += (unsigned int)graba_short(file1,(char *)&fi->f_campos[i].c_mante.c_visual,
			sizeof(short));
		tamano += (unsigned int)graba_short(file1,(char *)&fi->f_campos[i].c_mante.c_lineal,
			sizeof(short));
		tamano += (unsigned int)graba_short(file1,(char *)&fi->f_campos[i].c_mante.c_qp,
			sizeof(short));
		tamano += (unsigned int)graba_short(file1,(char *)&fi->f_campos[i].c_mante.c_qpl,
			sizeof(short));
		tamano += (unsigned int)graba_short(file1,(char *)&fi->f_campos[i].c_qc,
			sizeof(short));
		tamano += (unsigned int)graba_short(file1,(char *)&fi->f_campos[i].c_offset,
			sizeof(short));
		tamano += (unsigned int)graba_short(file1,(char *)&fi->f_campos[i].c_long,
			sizeof(short));
		tamano += (unsigned int)graba_short(file1,(char *)&fi->f_campos[i].c_tipo,
			sizeof(short));
		tamano += (unsigned int)graba_short(file1,(char *)&fi->f_campos[i].c_key,
			sizeof(short));
		tamano += (unsigned int)graba_short(file1,(char *)&fi->f_campos[i].c_fflag,
			sizeof(short));
	}
	
	for (i = 0;i<fi->f_nclaves;i++) 
	{
		tamano += (unsigned int)rw_write_(file1,(char *)&nulo,sizeof(char *));
		tamano += (unsigned int)rw_write_(file1,(char *)&nulo,sizeof(char *));
		tamano += (unsigned int)graba_short(file1,(char *)&fi->f_clave[i].k_ncampos,
			sizeof(short));
		tamano += (unsigned int)graba_short(file1,(char *)&fi->f_clave[i].k_fflag,
			sizeof(short));
	}
	
	for (i = 0;i<fi->f_nclaves;i++) 
	{
		for (j = 0;j < fi->f_clave[i].k_ncampos;j++)
			tamano += (unsigned int)graba_short(file1,(char *)(fi->f_clave[i].k_campo+j),
			sizeof(short));
		for (j = 0;j < fi->f_clave[i].k_ncampos;j++)
			tamano += (unsigned int)graba_short(file1,(char *)(fi->f_clave[i].k_orden+j),
			sizeof(short));
	}
	
	for (i=0;i<fi->f_nrelaci;i++) 
	{
		tamano += (unsigned int)rw_write_(file1,(char *)&nulo,
			sizeof(char *));
		tamano += (unsigned int)rw_write_(file1,(char *)&nulo,
			sizeof(char *));
		tamano += (unsigned int)rw_write_(file1,(char *)&nulo,
			sizeof(char *));
		tamano += (unsigned int)graba_short(file1,(char *)&fi->f_relaci[i].r_obliga,
			sizeof(short));
		tamano += (unsigned int)graba_short(file1,(char *)&fi->f_relaci[i].r_leer,
			sizeof(short));
	}
	
    /* segundo bloque */
	tamano += (unsigned int)rw_write_(file1,fi->f_titulo,
		strlen(fi->f_titulo)+1);
	
	for (i=0;i<fi->f_ncampos;i++) 
	{
		tamano += (unsigned int)rw_write_(file1,fi->f_campos[i].c_usi,
			strlen(fi->f_campos[i].c_usi)+1);
		tamano += (unsigned int)rw_write_(file1,fi->f_campos[i].c_mante.c_mensaje,
			strlen(fi->f_campos[i].c_mante.c_mensaje)+1);
		tamano += (unsigned int)rw_write_(file1,fi->f_campos[i].c_mante.c_defecto,
			strlen(fi->f_campos[i].c_mante.c_defecto)+1);
		tamano += (unsigned int)rw_write_(file1,fi->f_campos[i].c_minimo,
			strlen(fi->f_campos[i].c_minimo)+1);
		tamano += (unsigned int)rw_write_(file1,fi->f_campos[i].c_maximo,
			strlen(fi->f_campos[i].c_maximo)+1);
	}
	
	for (i=0;i<fi->f_nrelaci;i++) 
	{
		tamano += (unsigned int)rw_write_(file1,fi->f_relaci[i].r_fichero,
			strlen(fi->f_relaci[i].r_fichero)+1);
		tamano += (unsigned int)rw_write_(file1,fi->f_relaci[i].r_mfichero,
			strlen(fi->f_relaci[i].r_mfichero)+1);
		for (j = 0;j < MAXCAMPOSPORCLAVE +1;j++) {
			tamano += (unsigned int)graba_short(file1,
				(char *)&fi->f_relaci[i].r_campo[j],
				sizeof(short));
			if (fi->f_relaci[i].r_campo[j] == 0) break;
		}
	}
	
	for (i=0;i<fi->f_ncampos;i++) 
	{
		rw_write_(file1,fi->f_campos[i].c_nombre,strlen(fi->f_campos[i].c_nombre)+1);
		rw_write_(file1,fi->f_campos[i].c_lista,strlen(fi->f_campos[i].c_lista)+1);
		rw_write_(file1,fi->f_campos[i].c_mante.c_relacion,strlen(fi->f_campos[i].c_mante.c_relacion)+1);
	}
	
	rw_lseek_(file1,posicion,SEEK_SET);
	/* ojo tiene 50 bytes de mas */
	graba_short(file1,(char *)&tamano,sizeof(short));
	
	
	{
		char *pi;
		long tam1;
		char *total;
		unsigned short checksum = 0;
		unsigned short suma = 0;
		/* grabar el check-sum */
		tam1 = rw_lseek_(file1,0L,SEEK_END) - 40L;
		total = malloc(tam1);
		memset(total,0,tam1);
		rw_lseek_(file1,42L,SEEK_SET);
		rw_read_(file1,total,tam1-2);
		for (pi = total;pi < (total+tam1-1);pi += sizeof(short))
		{
			checksum ^= *((unsigned short *)pi);
			suma += *((unsigned short *)pi);
		}
		free(total);
		rw_lseek_(file1,16L,SEEK_SET);
		graba_short(file1,&suma,sizeof(short));
		graba_short(file1,&checksum,sizeof(short));
	}
	
	rw_lseek_(file1,posicion+sizeof(short),SEEK_SET);
	
    rw_close_(file1);	
	
	return(0);
}


