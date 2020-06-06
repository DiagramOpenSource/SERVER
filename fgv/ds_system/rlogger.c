
/* Autor: Francisco Gonzalez Valiente */
#include <fgvsys.h>


#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <stdio.h>
#include <malloc.h>
#include <rwmalloc.h>
#include <memory.h>

#include <rtermin.h>
#define _NUEVODEF
#include <rdef.h>      /* ya incluye el isam.h */


#ifndef O_BINARY  /* compatibilidad xenix - dos */
#define O_BINARY 0
#endif


#include "fgvixx.h"


extern char dirdef[MAXPATH]; /* viene del agisys : esta rutina se usa con agisys o similares !! */

extern char *quita_path();

#ifndef ESDOS
#ifdef ES_DISAM
extern int iSfd; /* handle of actual isam file in use */
#endif
#endif


extern char *lista_incluidos_logger;

static char **ListaLogger = NULL;

extern long loggersize;
extern int tracer_int;
extern int logger_int;

extern int resta_dias_logger	;

extern int tempo_logger;

static int en_logger = 0;
static int primer_logger = 1;

extern char *logger_salida;

static char *salida1 = "logger.spo";
#ifdef ESDOS
static char *salida2 = "PRN";
#else
static char *salida2 = "/dev/prn";
#endif
static char *salida = NULL;

extern char rw_user[11];
extern char *fecha();

extern unsigned char la_ethernet_[6];

extern char *agi_codi_empre;


extern int ixxload_fd_nrecords;
extern double *ixxload_nrecords;

static int nograba_avisado = 0;

static int esta_en_la_lista(char *def) {
	int kk;	
	char tmp2[128];
	if (!lista_incluidos_logger || !def)
		return 1;
	if (!ListaLogger) {
		FILE *f;
		f = fopen(lista_incluidos_logger,"r");
		if (f) {
            char tmp_nom[128];
            int lineas = 0;
            while(fgets(tmp_nom,120,f))
            {
                quitan(tmp_nom);
                quitab(tmp_nom);
				if (tmp_nom[0])
				{
					lineas++;
				}
            }
            rewind(f);
            ListaLogger = (char **)malloc(sizeof(char *)*(lineas+1));
            kk = 0;
            while(kk < lineas && fgets(tmp_nom,120,f))
            {
                quitan(tmp_nom);
                quitab(tmp_nom);
				if (tmp_nom[0])
				{
					ListaLogger[kk] = malloc(strlen(tmp_nom)+1);
					strcpy(ListaLogger[kk],tmp_nom);
					kk++;
				}
            }
            ListaLogger[kk] = NULL;
			fclose(f);
		}
		else
			return 1;
	}
	strcpy(tmp2,quita_path(def));
	quitab(tmp2);
	for (kk = 0;ListaLogger[kk];kk++) {
		if (!strcmp(ListaLogger[kk],tmp2))
			return 1;
	}
	return 0;
}

static void thesizematters(char *file)
{
	struct stat buf;
	char tmp1[512];

	if (loggersize < 1000)
		return;


	strcpy(tmp1,file);
	strcat(tmp1,".dat");
    if (stat(tmp1,&buf) != -1)
    {
		if (buf.st_size > loggersize || buf.st_size < -1)
		{
			char tmp[512];
			int i;
			sprintf(tmp,"%s_%s_%s",file,fecha(),fecha()+11);
			i = strlen(tmp) - strlen(quita_path(tmp));
			for (;tmp[i];i++)
			{
				if (tmp[i] == '.' || tmp[i] == '/' || tmp[i] == '\\' || tmp[i] == ':')
					tmp[i] = '_';
			}
			strcat(tmp,".dat");
			/* fuera de dsdb */
			rename(tmp1,tmp);
			/*agi_compress(tmp,0);
			unlink(tmp);*/
			sprintf(tmp,"%s_%s_%s",file,fecha(),fecha()+11);
			i = strlen(tmp) - strlen(quita_path(tmp));
			for (;tmp[i];i++)
			{
				if (tmp[i] == '.' || tmp[i] == '/' || tmp[i] == '\\' || tmp[i] == ':')
					tmp[i] = '_';
			}
			strcat(tmp,".ixx");
			strcpy(tmp1,file);
			strcat(tmp1,".ixx");
			rename(tmp1,tmp);
		}
	}
}


static int borra_loggers(char *file_)
{
   char file[512];
   char tmp[20];  
   char buf[81];
   char fech[40];
   int resta;
   
   if (!resta_dias_logger) return(0);

   strcpy(file,file_);
   *(quita_path(file)) = 0;

   resta = 0 - resta_dias_logger;
   strcpy(fech,fecha());
   diasfecha(fech,resta,0,0);
   stfecha(fech,tmp);   
   tmp[8] = 0;

   strcpy(buf,file);
   strcat(buf,"*");
   strcat(buf,".dat");
   _lee_y_borra(buf,NULL,tmp);

   strcpy(buf,file);
   strcat(buf,"*");
   strcat(buf,".ddx");
   _lee_y_borra(buf,NULL,tmp);

   strcpy(buf,file);
   strcat(buf,"*");
   strcat(buf,".ixx");
   _lee_y_borra(buf,NULL,tmp);

   return(0);
}


struct Registro_Logger
{
	char usuario[10];
	char fecha[8];
	char hora[8];
	char tipo[1];
	char accion[2];
	char tabdef[12];
	char campo[3];
	char clave[50];
	char mac[17];
	char empresa[10];
	char titulo[50];
	char valant[30];
	char valnue[30];
    char tickcount[8];
	char lf[1];
	char relleno_valor[512];
};	
#define TAMREGLOG 240


static void DeterminaSalidaLogger(char *def,int accion,char *inidef) {
   if (logger_salida)
   {
	   if (*logger_salida)
	   {
		   salida = logger_salida;
		   if (accion < 3 || accion > 99 || esta_en_la_lista(def))
			  en_logger = 1;
		   else
			  en_logger = 0;
	   }
	   else
	   {
		   en_logger = 0;
	   }
   }   
   else if (logger_int) 
   {
      if (logger_int == 1)
         salida = salida1;
      else
         if (logger_int == 2)
            salida = salida2;
         if (logger_int == en_logger)
            en_logger = 0;
         else
            en_logger = logger_int;
         logger_int = 0;
   }
   if (!tempo_logger && en_logger && def && inidef && *inidef) {
	   if (strcmp(def,inidef))
		   en_logger = 0;
   }
}

/*
int consul_clave_grid(pfi,ipfi,encl,numpan,limi1,limi2,filtro,pipfi,campo)
FI_DEF *pfi;
int ipfi,encl,numpan,limi1,limi2,filtro,pipfi,campo;
{

 */
/*
int consul_clave_grid(pfi,ipfi,encl,numpan,limi1,limi2,filtro,pipfi,campo,ffiltro)
FI_DEF *pfi;
int ipfi,encl,numpan,limi1,limi2,filtro,pipfi,campo;
char *ffiltro;
*/
/*int inserta_campo(FI_DEF *fi,int n,char *id,char *nombre,int qc,char *minimo,char *maximo,char *defecto)*/
/*int inserta_campo(FI_DEF *fi,int n,char *id,char *nombre,int qc,char *minimo,char *maximo,char *defecto)*/
static FI_DEF *crea_def_logger()
{
	FI_DEF *pfi;
	int i;

	DeterminaSalidaLogger(NULL,0,NULL);
	if (!salida)
		return NULL;

	pfi = (FI_DEF *)malloc(sizeof(FI_DEF));
	memset((char *)pfi,0,sizeof(FI_DEF));
#ifdef FGVX
     pfi->f_pantaids = (int *)malloc(sizeof(int) * (pfi->f_npanta+1));
     memset((char *)pfi->f_pantaids,0xff,sizeof(int) * (pfi->f_npanta+1));

     pfi->f_pantasids = (int *)malloc(sizeof(int) * (pfi->f_npanta+1));
     memset((char *)pfi->f_pantasids,0xff,sizeof(int) * (pfi->f_npanta+1));

     pfi->f_pantamix = (PAN_DEF **)malloc(sizeof(PAN_DEF *) * (pfi->f_npanta+1));
     memset((char *)pfi->f_pantamix,0,sizeof(PAN_DEF *) * (pfi->f_npanta+1));
         
     pfi->f_id = -1;


     pfi->f_campowids = (int *)malloc(sizeof(int) * (pfi->f_ncampos+1));
     memset((char *)pfi->f_campowids,0xff,sizeof(int) * (pfi->f_ncampos+1));

     pfi->f_camporden = (int *)malloc(sizeof(int) * (pfi->f_ncampos+1));
     memset((char *)pfi->f_camporden,0xff,sizeof(int) * (pfi->f_ncampos+1));                  

     pfi->f_taborder  = (int *)malloc(sizeof(int) * (pfi->f_ncampos+1));
     memset((char *)pfi->f_taborder,0xff,sizeof(int) * (pfi->f_ncampos+1));

     pfi->f_orderflags  = (int *)malloc(sizeof(int) * (pfi->f_ncampos+1));
     memset((char *)pfi->f_orderflags,0,sizeof(int) * (pfi->f_ncampos+1));

     pfi->f_fi_clave_lineas = 0;
     pfi->f_fi_modo_lineas  = 0;
     pfi->f_fi_scroll_lineas= -1;
     pfi->f_ipfi = -1;
#endif
	pfi->f_titulo = strmalloc(strlen(salida)+1);
	strcpy(pfi->f_titulo,salida);
	strcpy(pfi->f_fichero,salida);
	strcpy(pfi->f_PathDef,salida);
	inserta_campo(pfi,0,"Usuario","Usuario",10,"","","");
	inserta_campo(pfi,1,"Fecha","Fecha",-1,"","","");
	inserta_campo(pfi,2,"Hora","Hora",8,"","","");
	inserta_campo(pfi,3,"Tipo","Tipo",1,"","","");
	inserta_campo(pfi,4,"Accion","Accion",2,"","","");
	inserta_campo(pfi,5,"Tab-Def","Tab-Def",12,"","","");
	inserta_campo(pfi,6,"Campo","Campo",3,"","","");
	inserta_campo(pfi,7,"Clave","Clave",50,"","","");
	inserta_campo(pfi,8,"Mac","Mac",17,"","","");
	inserta_campo(pfi,9,"Empresa","Empresa",10,"","","");
	inserta_campo(pfi,10,"Titulo","Titulo",50,"","","");
	inserta_campo(pfi,11,"ValorAnt","Valor Anterior",30,"","","");
	inserta_campo(pfi,12,"ValorNue","Valor Nuevo",30,"","","");
    inserta_campo(pfi,13,"Registro","Registro",0,"0","999999999999","0");

	for (i = 0;i < 14;i++) {
		pfi->f_campos[i].c_mante.c_modif  = 0;
		pfi->f_campos[i].c_mante.c_visual = 0;
	}

	pfi->f_nclaves = 2;
	pfi->f_clave = (KEY_DEF *)malloc(sizeof(KEY_DEF)*2); /* memoria para dos claves */	
	pfi->f_clave[0].k_ncampos = 1;
	pfi->f_clave[0].k_fflag = 0;
	pfi->f_clave[0].k_campo = (short *)malloc(sizeof(short) * 2);
	pfi->f_clave[0].k_orden = (short *)malloc(sizeof(short) * 2);
	pfi->f_clave[0].k_orden[0] = 1;
	pfi->f_clave[0].k_campo[0] = 14;

	pfi->f_clave[1].k_ncampos = 5;
	pfi->f_clave[1].k_fflag = 0;
	pfi->f_clave[1].k_campo = (short *)malloc(sizeof(short) * 6);
	pfi->f_clave[1].k_orden = (short *)malloc(sizeof(short) * 6);
	pfi->f_clave[1].k_orden[0] = 1;
	pfi->f_clave[1].k_campo[0] = 1;
	pfi->f_clave[1].k_orden[1] = 1;
	pfi->f_clave[1].k_campo[1] = 6;
	pfi->f_clave[1].k_orden[2] = 1;
	pfi->f_clave[1].k_campo[2] = 7;
	pfi->f_clave[1].k_orden[3] = 1;
	pfi->f_clave[1].k_campo[3] = 2;
	pfi->f_clave[1].k_orden[4] = 1;
	pfi->f_clave[1].k_campo[4] = 3;

	return pfi;
}

int lanza_consulta_logger(char *bloque) {
	FI_DEF *pfi = crea_def_logger();
	if (!pfi) {
		aviso(0,"Opcion no configurada");
		return -1;
	}
	if (abre_ind(pfi,0) >= 0) {
		int i,n,j;
        struct grid_filtro *filtros = NULL;
        char titulo[512];
        int nfiltros = 0;
        int *cols = NULL;
		struct grid_filtro filtro;
		char *b_lim1 = NULL;
		char *b_lim2 = NULL;
		int usarfiltro = 0;
		char desde[128];
		int multiples = 1;		
		char **multi = NULL;
		char *p;

		desde[0] = 0;


        memcpy(&i,bloque,sizeof(long));
        bloque += sizeof(long);        

        strcpy(titulo,"Filtro ");
        strcat(titulo,bloque);
        bloque += (strlen(bloque)+1);
        memcpy(&i,bloque,sizeof(int));
        bloque += sizeof(int);
        cols = (int *)bloque;
        bloque += (sizeof(int)*i);
        memcpy(&nfiltros,bloque,sizeof(int));
        bloque += sizeof(int);
        if (nfiltros)
        {
            filtros = (struct grid_filtro *)bloque;
        }
		pfi->f_npanta = 1;
		pfi->f_pantalla = (PAN_DEF *)malloc(sizeof(PAN_DEF)+1);
		memset(pfi->f_pantalla,0,sizeof(PAN_DEF));
		pfi->f_pantalla[0].p_campoini = 1;
		pfi->f_pantalla[0].p_campofin = i;
		for (n = 0;n < i;n++) {
			pfi->f_campos[n].c_mante.c_actual = 1000+cols[n];
		}

#ifdef FGVX
		{
			int k;
			k = 0;
			for (i = pfi->f_pantalla[0].p_campoini-1;i < pfi->f_pantalla[0].p_campofin;i++) {
				 if (k >= pfi->f_ncampos) break;
				 k = pfi->f_campos[i].c_mante.c_actual % 1000;
				 pfi->f_camporden[k] = i;
				 pfi->f_taborder[i] = k;
			}
		}
#endif

		if (nfiltros) {
			memset(pfi->f_buf,0,pfi->f_lfich);
			for (n = 0;n < nfiltros;n++) {
				for (j = 0;j < pfi->f_clave[1].k_ncampos;j++) {
					if (pfi->f_clave[1].k_campo[j] == (filtros[n].campo+1))
						break;
				}
				if (j >= pfi->f_clave[1].k_ncampos)
					usarfiltro = 1;

				if (filtros[n].tipo > 0) {
					st_campo(filtros[n].campo,pfi,filtros[n].valor);					
				}
				else {					
					st_campo(filtros[n].campo,pfi,filtros[n].limi);
					if (filtros[n].campo == 1) {
						memcpy(desde,pfi->f_buf+10,8);
						desde[8] = 0;
					}	
				}
			}		        
			b_lim1 = malloc(pfi->f_lfich+1);        
			memcpy(b_lim1,pfi->f_buf,pfi->f_lfich);

			memset(pfi->f_buf,255,pfi->f_lfich);
			for (n = 0;n < pfi->f_ncampos;n++) {
				if (pfi->f_campos[n].c_tipo == DOUBLETIPO) {
					st_doble(n,pfi,(double)(MAXDOUBLE-1));
				} else if (pfi->f_campos[n].c_tipo == INTTIPO) {
					st_doble(n,pfi,(double)32534);
				}
			}
			for (n = 0;n < nfiltros;n++) {
				if (filtros[n].tipo > 0) {
					st_campo(filtros[n].campo,pfi,filtros[n].valor);
				}
				else {
					st_campo(filtros[n].campo,pfi,filtros[n].lims);
				}
			}		        
			b_lim2 = malloc(pfi->f_lfich+1);        
			memcpy(b_lim2,pfi->f_buf,pfi->f_lfich);
		}

		if (desde[0]) {
			/*if (abre_ind(pfi) >= 0) */{
				if (lee_reg(pfi,ISFIRST) >= 0) {
					if (memcmp(desde,pfi->f_buf+10,8) > 0)
						multiples = 0;
				}
				/*cierra_ind(pfi);*/
			}
		}
		if (multiples) {
			strcpy(desde,pfi->f_fichero);
			i = strlen(desde) - strlen(quita_path(desde));
			for (;desde[i];i++)
			{
				if (desde[i] == '.' || desde[i] == '/' || desde[i] == '\\' || desde[i] == ':')
					desde[i] = '_';
			}
			strcat(desde,"_*.dat");
			multi = lee_direct(desde,0);
			if (multi) {
				for(multiples = 0;multi[multiples];multiples++) {
					p = strstr(multi[multiples],".dat");
					if (p) {						
						*p = 0;
						/*aviso(0,multi[multiples]);*/
					}
				}
				usarfiltro = 1;
			}
		}
		
		if (!usarfiltro) {
			nfiltros = 0;
			filtros = NULL;
		}
		

        LanzaFiltro(pfi,1,titulo,nfiltros,filtros,0,NULL,b_lim1,b_lim2,multi);
		/*consul_clave_grid(pfi,-1,1,0,NULL,NULL,NULL,-1,-1,NULL);*/
		cierra_ind(pfi);
	}
	free_defpri(pfi);
	return 0;
}


int recoge_usuarios(char *bloque) {
	 int mem,nm,*pn;
	 char buf[512];
	 int file;
	 FILE *f;
	 int oktodos = 0;

	 mem = *((int *)bloque);
	 if (mem < 512) {
		 return -1;
	 }

	 bloque += sizeof(int);
	 pn = (int *)bloque;
	 bloque += sizeof(int);
	 *pn = 0;

	 DeterminaSalidaLogger(NULL,0,NULL);
	 if (!salida)
		return -1;

	 f = fopen(rw_directorio("dev/usr/ds_log.usr"),"r");
	 if (f) {
		 while(lee_linea(buf,250,f)) {
			quita_blancos(buf);
			if (!strcmp(rw_user,buf)) {
				oktodos = 1;
				break;
			}
		 }
		 fclose(f);
	 }
	 if (!oktodos) {
		(*pn)++;
		strcpy(bloque,"000");
		bloque += 3;
		strcpy(bloque,rw_user);
		return 0;
	 }
	 
	 file = rw_open_(rw_directorio("dev/usr/ds_sys.psw"),O_RDONLY | O_BINARY);
	 if (file < 0)
		 return -1;
	 rw_lseek_(file,0L,SEEK_SET);
	 nm = sizeof(int);
	 while(rw_read_(file,buf,36) == 36) {
		 nm += 36;
		 if (nm > mem) {
			 rw_close_(file);
			 return -2;
		 }
		 memcpy(bloque,buf,36);
		 bloque += 36;
		 (*pn)++;
	 }
	 rw_close_(file);
	 return 0;
}

int registro_logger(char *tipo,int accion,char *def,int campo,char *clave,char *titulo,char *valant,char *valnue,char *inidef)
{
   int file;
   int fd;
   char tmp[300];
   /*double tt;*/
   int n;
   int ant_iserrno = iserrno;
   struct Registro_Logger registro;
   FgvStdTabla *dtabla = NULL;
   int _ixx_datos_dbf = ixx_datos_dbf;
   int _default_dbf_id = default_dbf_id;
   

   DeterminaSalidaLogger(def,accion,inidef);
   
   if (!en_logger) {
	   iserrno = ant_iserrno;
	   return;
   }

   if (primer_logger)
   {
	   primer_logger = 0;
	   borra_loggers(salida);
   }
 
   memset((void *)&registro,' ',sizeof(registro));   

   memcpy(registro.usuario,rw_user,strlen(rw_user));
   stfecha(fecha(),registro.fecha);
   memcpy(registro.hora,fecha()+11,sizeof(registro.hora));
   registro.tipo[0] = *tipo;
   sprintf(tmp,"%02d",accion);
   memcpy(registro.accion,tmp,sizeof(registro.accion));
   n = strlen(def);
   if (n > sizeof(registro.tabdef))
   {
	   n = sizeof(registro.tabdef);
   }
   memcpy(registro.tabdef,def,n);
   sprintf(tmp,"%03d",campo);
   memcpy(registro.campo,tmp,sizeof(registro.campo));
   n = strlen(clave);
   if (n > sizeof(registro.clave))
   {
	   n = sizeof(registro.clave);
   }
   memcpy(registro.clave,clave,n);

   sprintf(tmp,"%02X:%02X:%02X:%02X:%02X:%02X",la_ethernet_[0],la_ethernet_[1],la_ethernet_[2],la_ethernet_[3],la_ethernet_[4],la_ethernet_[5]);
   memcpy(registro.mac,tmp,sizeof(registro.mac));
   if (agi_codi_empre)
   {
	   n = strlen(agi_codi_empre);
	   if (n > sizeof(registro.empresa))
	   {
		   n = sizeof(registro.empresa);
	   }
	   memcpy(registro.empresa,agi_codi_empre,n);
   }
   n = strlen(titulo);
   if (n > sizeof(registro.titulo))
   {
	   n = sizeof(registro.titulo);
   }
   memcpy(registro.titulo,titulo,n);   
   n = strlen(valant);
   if (n > sizeof(registro.valant))
   {
	   n = sizeof(registro.valant);
   }
   memcpy(registro.valant,valant,n);   
   n = strlen(valnue);
   if (n > sizeof(registro.valnue))
   {
	   n = sizeof(registro.valnue);
   }
   memcpy(registro.valnue,valnue,n);

   /*
   tt = time(NULL) * 10000 + (GetTickCount() % 10000);
   memcpy(registro.tickcount,(char *)&tt,sizeof(tt));
   */

   registro.lf[0] = 10;

   
   thesizematters(salida);


   ixx_datos_dbf = 0;
   default_dbf_id = 0;
   AjustaPunterosSegunDatos(-1);

   fd = ixx_abre_indice(salida,0/*ISINOUT + ISMANULOCK*/,NULL,NULL,TAMREGLOG-1,NULL,salida);
   if (fd < 0) {
	   int nclaves = 2,i,k;
	   IXXCLAVE *claves;

	   claves = (IXXCLAVE *)malloc(sizeof(IXXCLAVE) * nclaves);
	   memset(claves,0,sizeof(IXXCLAVE) * nclaves);
	   for (i = 0;i < nclaves;i++)
	   {
		   if (!i) {
			 claves[i].clave_tipo = IXXCLAVENODUP;
			 claves[i].clave_ncampos = 1;
		   }
		   else {
			 claves[i].clave_tipo = IXXCLAVEDUP;
			 claves[i].clave_ncampos = 5;
		   }		  
		   claves[i].clave_campos = (IXXCAMPO *)malloc(sizeof(IXXCAMPO) * (claves[i].clave_ncampos+1));
		   memset(claves[i].clave_campos,0,sizeof(IXXCAMPO) * (claves[i].clave_ncampos+1));
		   if (!i) {
			   k = 0;
			   claves[i].clave_campos[k].type = IXXCDOUBLE;
			   claves[i].clave_campos[k].modo = 0;
			   claves[i].clave_campos[k].len = 8;
			   claves[i].clave_campos[k].pos_en_reg = 231;
			   strcpy(claves[i].clave_campos[k].name,"count");
		   }
		   else {
			   k = 0;
			   claves[i].clave_campos[k].type = IXXCALFA;
			   claves[i].clave_campos[k].modo = 0;
			   claves[i].clave_campos[k].len = 10;
			   claves[i].clave_campos[k].pos_en_reg = 0;
			   strcpy(claves[i].clave_campos[k].name,"usuario");
			   k = 1;
			   claves[i].clave_campos[k].type = IXXCALFA;
			   claves[i].clave_campos[k].modo = 0;
			   claves[i].clave_campos[k].len = 12;
			   claves[i].clave_campos[k].pos_en_reg = 29;
			   strcpy(claves[i].clave_campos[k].name,"tabdef");			   
			   k = 2;
			   claves[i].clave_campos[k].type = IXXCALFA;
			   claves[i].clave_campos[k].modo = 0;
			   claves[i].clave_campos[k].len = 3;
			   claves[i].clave_campos[k].pos_en_reg = 41;
			   strcpy(claves[i].clave_campos[k].name,"campo");
			   k = 3;
			   claves[i].clave_campos[k].type = IXXCALFA;
			   claves[i].clave_campos[k].modo = 0;
			   claves[i].clave_campos[k].len = 8;
			   claves[i].clave_campos[k].pos_en_reg = 10;
			   strcpy(claves[i].clave_campos[k].name,"fecha");
			   k = 4;
			   claves[i].clave_campos[k].type = IXXCALFA;
			   claves[i].clave_campos[k].modo = 0;
			   claves[i].clave_campos[k].len = 8;
			   claves[i].clave_campos[k].pos_en_reg = 18;
			   strcpy(claves[i].clave_campos[k].name,"hora");
		   }
	   }
	   fd = ixx_nuevo_indice(salida,nclaves,claves,TAMREGLOG-1,NULL,NULL,NULL,salida);
	   if (fd < 0) {
			FI_DEF *pfi = crea_def_logger();
			if (pfi)  {
				abre_ind(pfi,0);
				fd = pfi->f_fd;
				pfi->f_fd = -1;
				free_defpri(pfi);
			}
		    /*fd = ixx_abre_indice(salida,0,NULL,NULL,TAMREGLOG-1,NULL,salida);*/
	   }
   }
   if (fd >= 0) {
	   ixxload_fd_nrecords = fd;
	   ixxload_nrecords = &(registro.tickcount);
	   if (ixx_graba_indice(fd,(char *)&registro,IXXGNUEVO) < 0) {
		   if (iserrno == EKEXISTS || iserrno == EDUPL) {
			   sleep(1);
			   ixx_graba_indice(fd,(char *)&registro,IXXGNUEVO);
		   }
	   }
	   ixxload_fd_nrecords = -1;
	   ixxload_nrecords = NULL;
	   ixx_cierra_indice(fd);
   }
   else if (!nograba_avisado) {
	   nograba_avisado = 1;
#ifdef FGVX
	   aviso(0,"Registro de cambios inaccesible. Consulte con su administrador.");
#endif
   }

   ixx_datos_dbf = _ixx_datos_dbf;
   default_dbf_id = _default_dbf_id;
   AjustaPunterosSegunDatos(-1);


   /*
   file = rw_open_(tmp,O_RDWR | O_BINARY | O_CREAT | O_APPEND,S_IREAD|S_IWRITE);
   if (file < 0) {pito(150);return;}

   rw_write_(file,(char *)&registro,TAMREGLOG);
   
   rw_close_(file);
   */
   iserrno = ant_iserrno;
}

/*
static void logger_identifica(FILE *f)
{
	fprintf(f,"%-10.10s %s %s [%02X:%02X:%02X:%02X:%02X:%02X]\r\n",rw_user,fecha(),fecha()+11,la_ethernet_[0],la_ethernet_[1],la_ethernet_[2],la_ethernet_[3],la_ethernet_[4],la_ethernet_[5]);
}


extern void pinta_en_logger(s)
unsigned char *s;
{
   FILE *f;
   
   if (logger_salida)
   {
	   if (*logger_salida)
	   {
		   salida = logger_salida;
		   en_logger = 1;
	   }
	   else
	   {
		   en_logger = 0;
	   }
   }   
   else if (logger_int) 
   {
      if (logger_int == 1)
         salida = salida1;
      else
         if (logger_int == 2)
            salida = salida2;
         if (logger_int == en_logger)
            en_logger = 0;
         else
            en_logger = logger_int;
         logger_int = 0;
   }
   
   if (!en_logger) return;

   thesizematters(salida);
   
   f = fopen(salida,"a");
   if (!f) {pito(151);return;}
   logger_identifica(f);
   fprintf(f,"%s\n",s);
   fclose(f);
   return;
}

*/

static void logger_actualiza(pfi,pfo)
FI_DEF *pfi;
FI_DEF *pfo;
{
   FI_DEF *pft;
   char buf[MAXDEFECTO*2];
   char buf2[MAXDEFECTO*2];
   char clave[1024];
   char tmp[512];
   int i,k,j;
   double rr;      
   
   /*
   strcpy(buf,dirdef);
   strcat(buf,quita_path(pfi->f_fichero));
   */
   pft = NULL;
   /*
   if (lee_defn(pfi->f_PathDef,&pft)) 
   {
      if (pft)
      {
         free((char *)pft);
      }
      pft = NULL;
   }
   */
   
   /*
   fprintf(f,__Texto[311],pfi->f_fichero);
   fprintf(f,__Texto[312]);
   */
   clave[0] = 0;
   for (i = 0;i < pfi->f_clave[0].k_ncampos;i++) {
      ld_campo(pfi->f_clave[0].k_campo[i]-1,pfi,buf);
	  if ((strlen(clave)+strlen(buf)+1) < MAXDEFECTO)
	  {
		  if (clave[0])
			  strcat(clave," ");
		  strcat(clave,buf);
	  }
      /*fprintf(f,"%s ",buf);*/
			}
   /*
   fprintf(f,"\n");
   */
   sprintf(tmp,__Texto[311],pfi->f_fichero);
   registro_logger("R",33,quita_path(pfi->f_fichero),0,clave,tmp,"","",quita_path(pfi->f_oldfichero));
   
   /* buscar diferencias en la actualizacion */
   k = 0;
   for(i = 0;i < pfi->f_ncampos;i++) {
      if (campocmp(i,1,pfi,pfi->f_buf,pfo->f_buf,0)) {
         k++;
         buf[0] = 0;
		 /*
         if (pft) 
            strcpy(buf,pft->f_campos[i].c_nombre);
		 */
		 strcpy(buf,pfi->f_campos[i].c_nombre);
         /*fprintf(f,__Texto[313],i,buf);*/
		 sprintf(tmp,__Texto[313],i,buf);
         ld_campo(i,pfi,buf2);
         ld_campo(i,pfo,buf);
		 /*
         fprintf(f,"[%s] -> [%s]",buf,buf2);
		 */
		 /*
         j = pfi->f_campos[i].c_tipo;
         if (j != CHARTIPO && j != FECHATIPO) 
		 {
            rr = rwatof(buf2) - rwatof(buf);
            sprintf(buf,"%lf",rr);
            quitadec(buf);
            fprintf(f," : [%s]",buf);
         }
         fprintf(f,".\n");
		 */
		 registro_logger("C",34,quita_path(pfi->f_fichero),i,clave,tmp,buf,buf2,quita_path(pfi->f_oldfichero));
      }
   }
   /*
   if (!k)
      fprintf(f,__Texto[314]);
   else
      fprintf(f,__Texto[315],k);
   fclose(f);
   */
   
   if (pft)
      free_defpri(pft);
}

static void logger_nueva(pfi)
FI_DEF *pfi;
{
   char buf[MAXDEFECTO*2];
   char clave[MAXDEFECTO];
   char tmp[512];

         int i;
		 /*
		 FILE *f;


		 thesizematters(salida);

         f = fopen(salida,"a");
         if (!f) {pito(152);return;}

		 logger_identifica(f);        
		 

         fprintf(f,__Texto[316],pfi->f_fichero);
         fprintf(f,__Texto[317]);
		 */
		 clave[0] = 0;
         for (i = 0;i < pfi->f_clave[0].k_ncampos;i++) {
            ld_campo(pfi->f_clave[0].k_campo[i]-1,pfi,buf);
		  if ((strlen(clave)+strlen(buf)+1) < MAXDEFECTO)
		  {
			  if (clave[0])
				  strcat(clave," ");
			  strcat(clave,buf);
		  }
		 }
            /*fprintf(f,"%s ",buf);*/
         
		 sprintf(tmp,__Texto[316],pfi->f_fichero);
		 registro_logger("R",31,quita_path(pfi->f_fichero),0,clave,tmp,"","",quita_path(pfi->f_oldfichero));

		   for(i = 0;i < pfi->f_ncampos;i++) 
		   {
				 buf[0] = 0;				 
				 strcpy(buf,pfi->f_campos[i].c_nombre);
				 sprintf(tmp,__Texto[313],i,buf);
				 ld_campo(i,pfi,buf);
				 registro_logger("C",34,quita_path(pfi->f_fichero),i,clave,tmp,"",buf,quita_path(pfi->f_oldfichero));			  
		   }

		 /*
         fprintf(f,"\n");
         fprintf(f,"********************************************************\n");
         
         fclose(f);
		 */
}


static void logger_baja(pfi)
FI_DEF *pfi;
{
		 char buf[MAXDEFECTO*2];
         int i;
   char clave[MAXDEFECTO];
   char tmp[512];
		 /*
		 FILE *f;

		 thesizematters(salida);

         f = fopen(salida,"a");
         if (!f) {pito(153);return;}

		 logger_identifica(f);
         
         fprintf(f,"Baja ficha %s.\n",pfi->f_fichero);
         fprintf(f,__Texto[317]);
		 */
		 clave[0] = 0;
         for (i = 0;i < pfi->f_clave[0].k_ncampos;i++) {
            ld_campo(pfi->f_clave[0].k_campo[i]-1,pfi,buf);
            /*fprintf(f,"%s ",buf);*/
		  if ((strlen(clave)+strlen(buf)+1) < MAXDEFECTO)
		  {
			  if (clave[0])
				  strcat(clave," ");
			  strcat(clave,buf);
		  }
		 }
         

		 sprintf(tmp,"Baja ficha %s.\n",pfi->f_fichero);
		 registro_logger("R",32,quita_path(pfi->f_fichero),0,clave,tmp,"","",quita_path(pfi->f_oldfichero));

		   for(i = 0;i < pfi->f_ncampos;i++) 
		   {
				 buf[0] = 0;				 
				 strcpy(buf,pfi->f_campos[i].c_nombre);
				 sprintf(tmp,__Texto[313],i,buf);
				 ld_campo(i,pfi,buf);
				 registro_logger("C",34,quita_path(pfi->f_fichero),i,clave,tmp,buf,"",quita_path(pfi->f_oldfichero));			  
		   }

		 /*
         fprintf(f,"\n");
         fprintf(f,"********************************************************\n");

         
         fclose(f);
		 */
}

static void pinta_clave_primaria(pfi)
FI_DEF *pfi;
{
   int i;
   char buf[MAXDEFECTO];
   
   /*
   for (i = 0;i < pfi->f_clave[0].k_ncampos;i++) {
      ld_campo(pfi->f_clave[0].k_campo[i]-1,pfi,buf);
      v_print(buf);
      v_print(" ");
   }
   */
   
}

int t_graba_nuevo(pfi)
register FI_DEF *pfi;
{
   int e;
   
   
   /*
   if (logger_salida)
   {
	   if (*logger_salida)
	   {
		   salida = logger_salida;
		   en_logger = 1;
	   }
	   else
	   {
		   en_logger = 0;
	   }
   }
   else if (logger_int) 
   {
		if (logger_int == 1)
            salida = salida1;
           else
                     if (logger_int == 2)
                        salida = salida2;
                     if (logger_int == en_logger)
                        en_logger = 0;
                     else
                        en_logger = logger_int;
                     logger_int = 0;
   } 
   */
   
   e = graba_nuevo(pfi);
   
   DeterminaSalidaLogger(quita_path(pfi->f_fichero),50/*irrelevante*/,quita_path(pfi->f_oldfichero));
   if (en_logger) {
				if (!e) {
					/*
                     Bl(1);
                     pprint(110,__Texto[318]);
                     v_print(pfi->f_fichero);
                     Bl(2);
                     pprint(201,__Texto[319]);
                     pinta_clave_primaria(pfi);
					 */
                     logger_nueva(pfi);
                  }
                  else {
					  /*
                     pito(154);
                     Bl(1);
                     pprint(110,__Texto[320]);
                     v_print(pfi->f_fichero);
                     Bl(2);
                     pprint(201,__Texto[321]);
					 */
                     pinta_clave_primaria(pfi);
                  }
   }
   
   return(e);
}

int t_graba_actual(pfi)
register FI_DEF *pfi;
{
   FI_DEF pfi2;
   int e;
   int i;
   int b = 0;
   int fd = pfi->f_fd;
   
   pfi2 = *pfi;
   
   /*
   if (logger_salida)
   {
	   if (*logger_salida)
	   {
		   salida = logger_salida;
		   en_logger = 1;
	   }
	   else
	   {
		   en_logger = 0;
	   }
   }
   else if (logger_int) 
   {
      if (logger_int == 1)
         salida = salida1;
      else
         if (logger_int == 2)
            salida = salida2;
         if (logger_int == en_logger)
            en_logger = 0;
         else
            en_logger = logger_int;
         logger_int = 0;
   }
   */
   
   pfi2.f_buf = malloc(pfi2.f_lfich+1);
   if (!lee_reg(&pfi2,ISCURR)) {
      /* comprobar si el registro esta bloqueado */
#ifndef ESDOS
#ifdef ES_DISAM
      /* de momento en espera de solucionar con btrieve */
      iSfd = fd;
      b = iSLdcheck( isrecnum );
      if (!b) {
		  /*
         Bl(24);
         pito(155);
         pprint(2401,__Texto[322]);
         v_print(pfi->f_fichero);
		 */
         /*  tracer_int = 1; */
      }
#endif
#endif
   }
   
   
   e = graba_actual(pfi);
   
   DeterminaSalidaLogger(quita_path(pfi->f_fichero),50/*irrelevante*/,quita_path(pfi->f_oldfichero));
   if (en_logger) {
      if (!e) {
		  /*
         Bl(1);
         pprint(110,__Texto[323]);
         v_print(pfi->f_fichero);
         Bl(2);
         pprint(201,__Texto[324]);
         pinta_clave_primaria(pfi);
		 */
         logger_actualiza(pfi,&pfi2);
      }
      else {
		  /*
         pito(156);
         Bl(1);
         pprint(110,__Texto[325]);
         v_print(pfi->f_fichero);
         Bl(2);
         pprint(201,__Texto[326]);
		 */
         pinta_clave_primaria(pfi);
      }
   }
   free(pfi2.f_buf);
   return(e);
}

int t_borra_actual(pfi)
register FI_DEF *pfi;
{
   FI_DEF pfi2;
   int e;
   int i;
   int b = 0;
   int fd = pfi->f_fd;
   
   pfi2 = *pfi;
   
   /*
   if (logger_salida)
   {
	   if (*logger_salida)
	   {
		   salida = logger_salida;
		   en_logger = 1;
	   }
	   else
	   {
		   en_logger = 0;
	   }
   }
   else if (logger_int) 
   {
      if (logger_int == 1)
         salida = salida1;
      else
         if (logger_int == 2)
            salida = salida2;
         if (logger_int == en_logger)
            en_logger = 0;
         else
            en_logger = logger_int;
         logger_int = 0;
   }
   */
   
   pfi2.f_buf = malloc(pfi2.f_lfich+1);
   if (!lee_reg(&pfi2,ISCURR)) {
      /* comprobar si el registro esta bloqueado */
#ifndef ESDOS
#ifdef ES_DISAM
      /* de momento en espera de solucionar con btrieve */
      iSfd = fd;
      b = iSLdcheck( isrecnum );
      if (!b) {
		  /*
         Bl(24);
         pito(157);
         pprint(2401,__Texto[322]);
         v_print(pfi->f_fichero);
		 */
         /*  tracer_int = 1; */
      }
#endif
#endif
   }
   
   
   e = borra_actual(pfi);
   
   DeterminaSalidaLogger(quita_path(pfi->f_fichero),50/*irrelevante*/,quita_path(pfi->f_oldfichero));
   if (en_logger)
   {
      if (!e) {
		  /*
         Bl(1);
         pprint(110,__Texto[323]);
         v_print(pfi->f_fichero);
         Bl(2);
         pprint(201,__Texto[324]);
         pinta_clave_primaria(&pfi2);
		 */
         logger_baja(&pfi2);
      }
      else {
		  /*
         pito(158);
         Bl(1);
         pprint(110,__Texto[325]);
         v_print(pfi->f_fichero);
         Bl(2);
         pprint(201,__Texto[326]);		 
         pinta_clave_primaria(pfi);
		 */
      }
   }
   free(pfi2.f_buf);
   return(e);
}

int t_borra_reg(pfi)
register FI_DEF *pfi;
{
   int e;
   

   /*
   if (logger_salida)
   {
	   if (*logger_salida)
	   {
		   salida = logger_salida;
		   en_logger = 1;
	   }
	   else
	   {
		   en_logger = 0;
	   }
   }
   else if (logger_int) 
   {
						if (logger_int == 1)
                     salida = salida1;
                  else
                     if (logger_int == 2)
                        salida = salida2;
                     if (logger_int == en_logger)
                        en_logger = 0;
                     else
                        en_logger = logger_int;
                     logger_int = 0;
   }
   */
   
   e = borra_reg(pfi);
   
   DeterminaSalidaLogger(quita_path(pfi->f_fichero),50/*irrelevante*/,quita_path(pfi->f_oldfichero));
   if (en_logger) {
						if (!e) {
					/*
                     Bl(1);
                     pprint(110,__Texto[318]);
                     v_print(pfi->f_fichero);
                     Bl(2);
                     pprint(201,__Texto[319]);
                     pinta_clave_primaria(pfi);
					 */
                     logger_baja(pfi);
                  }
                  else {
					  /*
                     pito(159);
                     Bl(1);
                     pprint(110,__Texto[320]);
                     v_print(pfi->f_fichero);
                     Bl(2);
                     pprint(201,__Texto[321]);
                     pinta_clave_primaria(pfi);
					 */
                  }
   }
   
   return(e);
}


