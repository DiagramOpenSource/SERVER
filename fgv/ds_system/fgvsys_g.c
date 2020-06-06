
/**********************************************************************/
/**********************************************************************/
/*   DIAGRAM9 */
/*  control de usuarios SYSTEM  V por Francesc Gonzalez    */
/**********************************************************************/
/**********************************************************************/

/* otras rutinas para posible overlay */


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
#define SEEK_END 2
#ifndef LARGESEEK
extern long rw_lseek_();
#endif
extern unsigned int rw_read_();
#endif

#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <errno.h>

#ifdef FGVDOS
#include <process.h>
#endif

#include <rwmalloc.h>
#include <rtermin.h>
#include <rentra.h>
#include <rfecha.h>
#include <rwmenus.h>
#include <rsignal.h>
#include <teclas.h>
#define _NUEVODEF
#include <rdef.h>       /* ya incluye el isam.h */
#include <rmante.h>
#include <rcalcul.h>



extern int time_d_tiempo1;
extern int time_d_tiempo2;



#ifdef FGV32

extern BOOL fIsTNT;

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
extern int remoto_ver1;
extern int remoto_ver2;
extern unsigned char remoto_verletra;
extern int remoto_que_sistema_es;
extern int remoto_es_sistema_grafico;
#endif
#endif


extern int (*Eliminador_Diagram)(char *);

char *nombre_tecla();

extern int duplicarusuario;

extern int simple_tarea_bruta;


extern int user_who;
extern char rw_user[11];
extern char *quita_path();
extern int subnumero_de_usuario;
extern int super_usuario;

extern char dirbass[51];
extern char nmen0[551];
extern char par1[551];
extern char par2[551];
extern char psx[11];

extern char clave_help[32];

extern char bufpan[PANBUFSIZE];

extern int fecha_protegida;

extern long nume_serie;

extern long user_cuantos;

extern char rw_version[256];

extern int cuantos_se_permiten;

#include <stdlib.h>
/*extern char *getenv();*/

extern char empresa[MAXDEFECTO];

extern char *apli_permisos = NULL;
extern char *dat_permisos = NULL;
extern char **cam_permisos = NULL;
extern char **clave_permisos = NULL;
extern char *empre_permisos = NULL;

static char *tabla_data[MAXDEF+10] = {NULL,NULL,NULL,};
static long tabla_data_flag[MAXDEF+10] = {0,0,0,0,};

unsigned char la_ethernet_[6] = {0xff,0xff,0xff,0xff,0xff,0xff};

extern long _e_log_open;
extern long _e_log_close;
extern long _e_log_read;
extern long _e_log_update;
extern long _e_log_write;
extern long _e_log_delete;


extern long _g_log_open;
extern long _g_log_close;
extern long _g_log_read;
extern long _g_log_update;
extern long _g_log_write;
extern long _g_log_delete;

extern int modo_desatendido;

extern char original_rw_dirbase[51];

extern int fichdeusuario;
extern char *tablauserdir;
extern char *tablauserfich;
extern char *tablauserapli;
extern char *tablamacdir;

extern int pass_cambio_autorizado;

extern char user_batch[11];
extern char user_batch_psw[11];

extern char nomapli[25];

extern int excel_alt;

#ifndef DSMINIBASICO


int con_tabla_macdir(unsigned char *la_ethernet_)
{
	int i,kk;
	char tmp_nom[128];
	char tmpe[81];
	char *p;
	static char **latabla = NULL;

	if (!tablamacdir)
	{		
		return 0;
	}	

	sprintf(tmpe,"%02X:%02X:%02X:%02X:%02X:%02X",la_ethernet_[0],la_ethernet_[1],la_ethernet_[2],la_ethernet_[3],la_ethernet_[4],la_ethernet_[5]);	

	if (!latabla)
	{
		FILE *f = fopen(tablamacdir,"r");		
		if (f)
		{
			int lineas = 0;
			while(fgets(tmp_nom,120,f))
			{
				lineas++;
			}
			rewind(f);
			latabla = (char **)malloc(sizeof(char *)*(lineas+1));
			kk = 0;
			while(kk < lineas && fgets(tmp_nom,120,f))
			{
				quitan(tmp_nom);
				quitab(tmp_nom);
				latabla[kk] = malloc(strlen(tmp_nom)+1);
				strcpy(latabla[kk],tmp_nom);
				kk++;
			}		          
			latabla[kk] = NULL;
			fclose(f);
		}
	}
	if (!latabla)
		return 0;

	for (i = 0;latabla[i];i++)
	{
		strcpy(tmp_nom,latabla[i]);
		p = "";
		for (kk = 0;tmp_nom[kk] && tmp_nom[kk] != ',';kk++);
		if (tmp_nom[kk] == ',')
		{
			tmp_nom[kk] = 0;
			p = tmp_nom + kk + 1;
		}
		if (!strcmp(tmp_nom,tmpe))
		{		   
		   libera_permisos();
		   final_agisys();
           fin_sig();
           Terfinal();					   
		   if (!p)
		   {			   
			   rw_setdirectorio(original_rw_dirbase);
		   }
		   else
		   {
			  rw_setdirectorio(p);
		   }
           Terinici();
           ini_sig();					   
           if (par1[0])
           {
              if (quita_path(par1) == par1)
                 strcpy(nmen0,rw_directorio(par1));
              else
                 strcpy(nmen0,par1);
		   }
            else
              strcpy(nmen0,rw_directorio("ds"));
            if (!par2[0])				 
              strcpy(dirbass,rw_directorio(""));
		   inicio_agisys();
		   /*inicio_dir();*/
		   return 1;
		}
	}
	return 0;
}


int cambia_por_tablauserdir(char *usuario)
{
	int i,kk;
	char tmp_nom[128];
	char *p;
	static char **latabla = NULL;
	if (!tablauserdir)
		return -1;

	if (!latabla)
	{
		FILE *f = fopen(tablauserdir,"r");		
		if (f)
		{
			int lineas = 0;
			while(fgets(tmp_nom,120,f))
			{
				lineas++;
			}
			rewind(f);
			latabla = (char **)malloc(sizeof(char *)*(lineas+1));
			kk = 0;
			while(kk < lineas && fgets(tmp_nom,120,f))
			{
				quitan(tmp_nom);
				quitab(tmp_nom);
				latabla[kk] = malloc(strlen(tmp_nom)+1);
				strcpy(latabla[kk],tmp_nom);
				kk++;
			}		          
			latabla[kk] = NULL;
			fclose(f);
		}
	}
	if (!latabla)
		return 0;

	for (i = 0;latabla[i];i++)
	{
		strcpy(tmp_nom,latabla[i]);
		p = "";
		for (kk = 0;tmp_nom[kk] && tmp_nom[kk] != ',';kk++);
		if (tmp_nom[kk] == ',')
		{
			tmp_nom[kk] = 0;
			p = tmp_nom + kk + 1;
		}
		if (!strcmp(tmp_nom,usuario))
		{
			/*
			if (!*p)
			{
				strcpy(fichdir,"fich");
				strcpy(fichdirbarra,"fich/");
			}
			else
			{
			    sprintf(fichdir,"%s",p);
			    sprintf(fichdirbarra,"%s/",p);
			}
			*/

		   libera_permisos();
		   final_agisys();
           fin_sig();
           Terfinal();					   
		   if (!p)
		   {			   
			   rw_setdirectorio(original_rw_dirbase);
		   }
		   else
		   {
			  rw_setdirectorio(p);
		   }
           Terinici();
           ini_sig();					   
           if (par1[0])
           {
              if (quita_path(par1) == par1)
                 strcpy(nmen0,rw_directorio(par1));
              else
                 strcpy(nmen0,par1);
		   }
            else
              strcpy(nmen0,rw_directorio("ds"));
            if (!par2[0])				 
              strcpy(dirbass,rw_directorio(""));
		   inicio_agisys();
		   inicio_dir();
		   return 1;
		}
	}
	return 0;
}


int cambia_por_tablauserfich(char *usuario)
{
	int i,kk;
	char tmp_nom[128];
	char *p;
	static char **latabla = NULL;
	if (!tablauserfich)
		return -1;

	if (!fichdeusuario)
	{
		strcpy(fichdir,"fich");
		strcpy(fichdirbarra,"fich/");
	}

	if (!latabla)
	{
		FILE *f = fopen(tablauserfich,"r");		
		if (f)
		{
			int lineas = 0;
			while(fgets(tmp_nom,120,f))
			{
				lineas++;
			}
			rewind(f);
			latabla = (char **)malloc(sizeof(char *)*(lineas+1));
			kk = 0;
			while(kk < lineas && fgets(tmp_nom,120,f))
			{
				quitan(tmp_nom);
				quitab(tmp_nom);
				latabla[kk] = malloc(strlen(tmp_nom)+1);
				strcpy(latabla[kk],tmp_nom);
				kk++;
			}		          
			latabla[kk] = NULL;
			fclose(f);
		}
	}
	if (!latabla)
		return 0;

	for (i = 0;latabla[i];i++)
	{
		strcpy(tmp_nom,latabla[i]);
		p = "";
		for (kk = 0;tmp_nom[kk] && tmp_nom[kk] != ',';kk++);
		if (tmp_nom[kk] == ',')
		{
			tmp_nom[kk] = 0;
			p = tmp_nom + kk + 1;
		}
		if (!strcmp(tmp_nom,usuario))
		{
			if (!*p)
			{
				strcpy(fichdir,"fich");
				strcpy(fichdirbarra,"fich/");
			}
			else
			{
			    sprintf(fichdir,"%s",p);
			    sprintf(fichdirbarra,"%s/",p);
			}
		    inicio_dir();
		    return 1;
		}
	}
	return 0;

}

int compon_dirbase(char *_dirbase_,char *_dirbass_,char *_nomapli_)
{ 
   int i,kk;
   char tmp_nom[128];
   char *p;
   static char **latabla = NULL;
   char *usuario = rw_user; /* usa rw_user fijo ...*/

   strcpy(_dirbase_,_dirbass_);
   strcat(_dirbase_,_nomapli_);
   strcat(_dirbase_,"/");

	if (!tablauserapli)
		return -1;

	if (!latabla)
	{
		FILE *f = fopen(tablauserapli,"r");		
		if (f)
		{
			int lineas = 0;
			while(fgets(tmp_nom,120,f))
			{
				lineas++;
			}
			rewind(f);
			latabla = (char **)malloc(sizeof(char *)*(lineas+1));
			kk = 0;
			while(kk < lineas && fgets(tmp_nom,120,f))
			{
				quitan(tmp_nom);
				quitab(tmp_nom);
				latabla[kk] = malloc(strlen(tmp_nom)+1);
				strcpy(latabla[kk],tmp_nom);
				kk++;
			}		          
			latabla[kk] = NULL;
			fclose(f);
		}
	}
	if (!latabla)
		return 0;

	for (i = 0;latabla[i];i++)
	{
		strcpy(tmp_nom,latabla[i]);
		p = "";
		for (kk = 0;tmp_nom[kk] && tmp_nom[kk] != ',';kk++);
		if (tmp_nom[kk] == ',')
		{
			tmp_nom[kk] = 0;
			p = tmp_nom + kk + 1;
		}
		if (!strcmp(tmp_nom,usuario))
		{
			if (*p)
			{
			   strcpy(_dirbase_,_dirbass_);
			   strcat(_dirbase_,_nomapli_);
			   strcat(_dirbase_,p);
			   strcat(_dirbase_,"/");
			   return 1;
			}		    
			return 0;
		}
	}

   return 0;
}


int lee_permisos(apli)
char *apli;
{
   int file,i,j,d,puntoco;
   char buf[MAXDEFECTO*20];
   char tmp1[50];
   short total1,total2,total3,l,totalc;
   char *p1,*p2,*p3;
   char skipper;


   if (DespachadorExterno)
		(*DespachadorExterno)((int)DESPACHO_DATA_INFO,(int)0,(void *)apli);
   
   sprintf(buf,"%s/%s.sui",rw_directorio(apli),apli);
   sobrepone_ini(buf);

   if (apli_permisos)
   {
      aviso(0,__Texto[53]);
   }
   if (dat_permisos)
   {
      aviso(0,__Texto[54]);
   }

   sprintf(buf,"%s/%03d/%s.prm",rw_directorio("dev/usr"),user_who,apli);
   file = rw_open_(buf,O_RDONLY | O_BINARY);
   if (file < 0)
      return(0);
   /* primero permisos de ejecucion tab o acceso directo */
   if (rw_read_(file,buf,5) == 5)
   {
	  while(rw_read_(file,&skipper,1) == 1 && skipper != 10);
      if (buf[0] == '+' || buf[0] == '-')
      {
         buf[5] = 0;
         total1 = atoi(buf+1);
         apli_permisos = malloc(total1*8+sizeof(short)+1);
         apli_permisos[0] = buf[0];
         *((short *)(apli_permisos+1)) = total1;
         p1 = apli_permisos + 1 + sizeof(short);
         for (i = 0;i < total1;i++)
         {
			for (d = 0;d < 8;d++)
			{
				if (rw_read_(file,&skipper,1) != 1) break;
				if (skipper == 13 || skipper == 10) break;
				buf[d] = skipper;
			}
			for (;d < 8;d++)
			{
				buf[d] = ' ';
			}
            /*if (rw_read_(file,buf,8) != 8) break;*/
			if (skipper != 10)
			{
				while(rw_read_(file,&skipper,1) == 1 && skipper != 10);
			}
            memcpy(p1,buf,8);
            p1 += 8;
         }
         memset(p1,0,(total1-i)*8);
      }
   }   
   
   /* luego permisos por fichero de datos */
   if (rw_read_(file,buf,5) == 5 && (buf[0] == '+' || buf[0] == '-'))
   {
	  while(rw_read_(file,&skipper,1) == 1 && skipper != 10);
      /* permisos de acceso a los ficheros de datos */
      buf[5] = 0;
      total1 = atoi(buf+1);
      dat_permisos = malloc(total1*9+sizeof(short)+1);
      if (total1)
      {
         cam_permisos = malloc(total1*sizeof(char *));
         clave_permisos = malloc(total1*sizeof(char *));
         memset(cam_permisos,0,total1*sizeof(char *));
         memset(clave_permisos,0,total1*sizeof(char *));
      }
      dat_permisos[0] = buf[0];
      *((short *)(dat_permisos+1)) = total1;
      p1 = dat_permisos + 1 + sizeof(short);
      for (i = 0;i < total1;i++)
      {
         if (rw_read_(file,buf,17) != 17) break;
		 while(rw_read_(file,&skipper,1) == 1 && skipper != 10);		 
         memcpy(p1,buf,8);
         p1[8] = buf[12];
         if ((buf[8] == '-' || buf[8] == '+') && (buf[13] == '-' || buf[13] == '+' || buf[13] == '*'))
         {
            buf[12] = 0;
            buf[17] = 0;
            total2 = atoi(buf+9);			
			if (buf[13] == '*')
			{
				total3 = 0;
				totalc = atoi(buf+14);
			}
			else
			{
				total3 = atoi(buf+14);
				totalc = -1;
			}

            cam_permisos[i] = malloc(2+sizeof(short)+total2*sizeof(char *));
            cam_permisos[i][0] = buf[8];
            *((short *)(cam_permisos[i]+1)) = total2;
            clave_permisos[i] = malloc(2+sizeof(short)+total3*sizeof(char *));
            clave_permisos[i][0] = buf[13];
            if (buf[13] == '*')
			{
				*((short *)(clave_permisos[i]+1)) = totalc;
			}
			else
			{
				*((short *)(clave_permisos[i]+1)) = total3;
			}
            
            p2 = cam_permisos[i]+1+sizeof(short);
            memset(p2,0,total2*sizeof(char *));
            for (j = 0;j < total2;j++)
            {
				puntoco = 3;
				buf[3] = 0;
				buf[5] = 0;
				for (d = 0;d < 5;d++)
				{
					if (rw_read_(file,&skipper,1) != 1) break;
					buf[d] = skipper;
					if (skipper == 10) break;
				}
				while(buf[d] != 10 && buf[puntoco] != '<' && buf[puntoco] != '>' && buf[puntoco] != '=' && buf[puntoco] != '[' && buf[puntoco] != ']' && buf[puntoco] != '!')
				{
					if (rw_read_(file,&skipper,1) != 1) break;
					buf[d] = skipper;					
					if (skipper == 10) break;					
					d++;
					puntoco += 3;
					if (rw_read_(file,&skipper,1) != 1) break;
					buf[d] = skipper;					
					if (skipper == 10) break;										
					d++;
					if (rw_read_(file,&skipper,1) != 1) break;
					buf[d] = skipper;					
					if (skipper == 10) break;										
					d++;
					buf[d] = 0;
				}

               /*if (rw_read_(file,buf,5) != 5) break;*/
               l = 0;
               if ((buf[puntoco] == '<' || buf[puntoco] == '>' || buf[puntoco] == '=' || buf[puntoco] == '[' || buf[puntoco] == ']' || buf[puntoco] == '!') && buf[puntoco+1] == '"')
               {
                  while(l < (MAXDEFECTO-8))
                  {
                     if (rw_read_(file,buf+l+2+puntoco,1) != 1) break;
                     if (buf[l+2+puntoco] == '"') break;
                     l++;
                  }
				  while(rw_read_(file,&skipper,1) == 1 && skipper != 10);
                  /*if (rw_read_(file,buf+l+5,2) != 2) break;*/
               }
               p3 = malloc(l+(sizeof(short)*(puntoco/3))+sizeof(short)+20);
               *((char **)p2) = p3;
               p3[2] = buf[puntoco];
               buf[puntoco] = 0;
               *((short *)(p3)) = (short)(puntoco/3);
			   p3 += (sizeof(short)+1);
			   for (d = 0;d < (puntoco/3);d++)
			   {
				   memcpy(tmp1,buf+d*3,3);
				   tmp1[3] = 0;
				   *((short *)(p3)) = (short)atoi(tmp1);
				   p3 += sizeof(short);
			   }				   
               if (l)
               {
                  
                  *((short *)(p3)) = (short)l;
                  p3 += sizeof(short);
                  memcpy(p3,buf+puntoco+2,l);
               }
               p2 += sizeof(char *);
            }
            
            p2 = clave_permisos[i]+1+sizeof(short);
            memset(p2,0,total3*sizeof(char *));
            for (j = 0;j < total3;j++)
            {
               if (rw_read_(file,buf,2) != 2) break;
               if (buf[0] != '=' && buf[0] != '>' && buf[0] != '<')
               {
                  break;
               }
               if (buf[1] != '"') break;
               l = 1;
               while(l < MAXDEFECTO)
               {
                  if (rw_read_(file,buf+l,1) != 1) break;
                  if (buf[l] == '"') break;
                  l++;
               }
               if (l >= MAXDEFECTO || buf[l] != '"')
                  break;
			   while(rw_read_(file,&skipper,1) == 1 && skipper != 10);
               /*if (rw_read_(file,buf+l,2) != 2) break;*/
               p3 = malloc(l+sizeof(short)+20);
               *((char **)p2) = p3;
               p3[0] = buf[0];
               *((short *)(p3+1)) = l-1;
               memcpy(p3+1+sizeof(short),buf+1,l-1);
               p3[l+sizeof(short)] = 0;
               p2 += sizeof(char *);
            }
         }
         p1 += 9;
      }
      memset(p1,0,(total1-i)*9);
   }

   if (rw_read_(file,buf,7) == 7)
   {
	   if (empre_permisos)
	   {
		 free(empre_permisos);
	   }	   
	   buf[5] = 0;
	   total1 = atoi(buf+1);
	   empre_permisos = malloc(sizeof(short)+2+total1*16);
	   *((short *)empre_permisos) = total1;
	   empre_permisos[sizeof(short)] = buf[0];
	   empre_permisos[sizeof(short)+1] = 0;
	   p1 = empre_permisos+sizeof(short)+2;
	   memset(p1,0,total1*16);

	   total2 = 0;
	   while(total2 < total1 && rw_read_(file,buf,18) == 18)
	   {
		   memcpy(p1,buf,16);
		   total2++;
		   p1 += 16;
	   }
   }


   rw_close_(file);
   return(0);
}

int libera_permisos()
{
   short total1,i,j,total3;
   char *p1,*p2;

   for (i = 0;i < MAXDEF;i++)
   {
       tabla_data_flag[i] = 0;
	   if (tabla_data[i])
	   {
		   free(tabla_data[i]);
		   tabla_data[i] = NULL;
	   }
   }
   if (apli_permisos)
   {
      free(apli_permisos);
      apli_permisos = NULL;
   }
   if (dat_permisos)
   {
      total1 = *((short *)(dat_permisos+1));
      free(dat_permisos);
      dat_permisos = NULL;
      if (cam_permisos)
      {
         for (i = 0;i < total1;i++)
         {
            if (cam_permisos[i])
            {
               total3 = *((short *)(cam_permisos[i]+1));
               p1 = cam_permisos[i]+1+sizeof(short);
               for (j = 0;j < total3;j++)
               {
                  p2 = *((char **)(p1));
                  if (p2)
                     free(p2);
                  p1 += sizeof(char *);
               }
               free(cam_permisos[i]);
            }
         }
         free(cam_permisos);
         cam_permisos = NULL;
      }
      if (clave_permisos)
      {
         for (i = 0;i < total1;i++)
         {
            if (clave_permisos[i])
            {
               total3 = *((short *)(clave_permisos[i]+1));
			   if (clave_permisos[i][0] == '*')
			   {
				   total3 = 0;
			   }
               p1 = clave_permisos[i]+1+sizeof(short);
               for (j = 0;j < total3;j++)
               {
                  p2 = *((char **)(p1));
                  if (p2)
                     free(p2);
                  p1 += sizeof(char *);
               }
               free(clave_permisos[i]);
            }
         }
         free(clave_permisos);
         clave_permisos = NULL;
      }
   }

   if (empre_permisos)
   {
	   free(empre_permisos);
	   empre_permisos = NULL;
   }
   
   return(0);
}

int hay_permiso(_opcion,modo)
char *_opcion;
int modo;
{
   char opcion[81];
   char tipo,*p1;
   short total1;
   int i;
   char buf[81];
   
   if (super_usuario)
   {
      if (modo)
         return(0);
      return(1);
   }

   i = super_permiso_opcion_(_opcion);
   if (i)
   {
	   return 0;
   }
   
   if (!apli_permisos)
   {
      if (modo)
         return(0);
      return(1);
   }
   tipo = apli_permisos[0];
   total1 = *((short *)(apli_permisos+1));
   if (modo)
   {
      if (tipo == '+' && total1 == 1)
      {
         memcpy(_opcion,apli_permisos+1+sizeof(short),8);
         _opcion[8] = 0;
         quitab(_opcion);
         strcat(_opcion,".tab");
         return(1);
      }
      return(0);
   }
   strcpy(opcion,quita_path(_opcion));
   for (i = 0;opcion[i];i++)
   {
      if (opcion[i] == '.')
      {
         opcion[i] = 0;
         break;
      }
   }
   sprintf(buf,"%-8.8s",quita_path(opcion));
   p1 = apli_permisos+1+sizeof(short);
   for (i = 0;i < total1;i++)
   {
      if (!memcmp(p1,buf,8)) break;
      p1 += 8;
   }
   if ((tipo == '+' && i < total1) || (tipo == '-' && i == total1))
      return(1);
   return(0);
}


static int compara_los_datos(p3,len,clave,pos)
unsigned char *p3;
int len;
unsigned char *clave;
int pos;
{
   int compara = 0;
   int j;
   
   compara = 0;
   for (j = 0;(j < len && j < pos);j++)
   {
      if (p3[j] < clave[j])
      {
         compara = -1;
         break;
      }
      if (p3[j] > clave[j])
      {
         compara = 1;
         break;
      }
   }
   
   if (!compara)
      for (;j < len;j++)
      {
         if (p3[j] < ' ')
         {
            compara = -1;
            break;
         }
         if (p3[j] > ' ')
         {
            compara = 1;
            break;
         }
      }
      
      if (!compara)
         for (;j < pos;j++)
         {
            if (clave[j] < ' ')
            {
               compara = -1;
               break;
            }
            if (clave[j] > ' ')
            {
               compara = 1;
               break;
            }
         }
         return(compara);
}

static int hay_permiso_de_clave(pfi,campo,indice)
FI_DEF *pfi;
int campo;
int indice;
{
		 char tmp[MAXDEFECTO+1];
         unsigned char *clave;
         short total3;
         short len;
         char *p1,*p2,tipo;
         unsigned char *p3;
         int i,nc,pos,ca,j,n;
         int valor,compara,sale;
         
         if (!pfi->f_nclaves || !clave_permisos || !clave_permisos[indice])
            return(1);
         
         tipo = clave_permisos[indice][0];
		 if (tipo == '*')
		 {
			 return 1;
		 }
         total3 = *((short *)(clave_permisos[indice]+1));
         if (tipo == '-' && !total3)
            return(1);
         if (tipo == '+' && !total3)
            return(0);
         
         if (campo >= 0 && pfi->f_campos[campo].c_key == 1)
            return(1);
         
         nc = pfi->f_clave[0].k_ncampos;
         if (nc > 1)
         {
            len = 0;
            for (i = 0;i < nc;i++)
            {
               len += long_campo_nofecha(pfi,pfi->f_clave[0].k_campo[i]-1);
            }
            clave = malloc(len+2);
         }
         else
         {
            clave = (unsigned char *)tmp;
         }
         pos = 0;
         for (i = 0;i < nc;i++)
         {
            ca = pfi->f_clave[0].k_campo[i]-1;
            ld_campo_nofecha(ca,pfi,clave+pos);
            n = long_campo_nofecha(pfi,ca);
            if (!pfi->f_campos[ca].c_qc)
            {
               for (j = 0;j < n;j++)
               {
                  if (clave[pos+j] == ' ')
                     clave[pos+j] = '0';
               }
            }
            pos += n;
         }
         
         p1 = clave_permisos[indice]+1+sizeof(short);
         valor = 1;
         sale = 0;
         for (i = 0;i < total3;i++)
         {
            p2 = *((char **)(p1));
            if (p2)
            {
               len = *((short *)(p2+1));
               p3 = (unsigned char *)(p2+1+sizeof(short));
               
               compara = compara_los_datos(p3,len,clave,pos);
               
               if (p2[0] == '=' && !compara)
               {
                  if (tipo == '-')
                  {
                     valor = 0;
                  }
                  else
                  {
                     valor = 1;
                  }
                  sale = 1;
               }
               else
                  if (p2[0] == '<')
                  {
                     if ((tipo == '+' && compara <= 0) || (tipo == '-' && compara > 0))
                        valor = 0;
                  }
                  else
                     if (p2[0] == '>')
                     {
                        if ((tipo == '+' && compara >= 0) || (tipo == '-' && compara < 0))
                           valor = 0;
                     }
            }
            if (sale) break;
            p1 += sizeof(char *);
         }
         
         if (nc > 1)
            free(clave);
         return(valor);
}


extern char *agi_codi_empre;

int hay_dat_permiso(pfi,campo)
FI_DEF *pfi;
int campo;
{
   char *p1,*p2,*p3,*p5,tipo,tipo2;
   unsigned char *p4;
   int i,j,compara,n,k,check,solo_campo = 0,comparacion,nocampo,simples,tcheck;
   int hayf,soloestecampo = 0,x1,n1;
   short total1,total2,camp,ncamp;
   char buf[MAXDEFECTO+1];
   char *fichero,alta = '#';
   int sumaalta = -1;   
   char *fichero2;
   char buff[81];
   char buff2[81];
   int elcampoesta = 0;
   int evaluabien = 0;
   int tsimples;
   int localcompara;
   int campomodo = 0,modoc;
   int decampo = 0;
   int buenascampo = 0;
   int esdecampo;   
   int permi_alta = 2;
   int permi_modif = 4;
   int permi_baja = 8;
   int sentencia = -1;
   int evaluado = 0;
   int upermisos = 0;
   
   if (super_usuario)
	   return(1+2+4+8);


   if (empre_permisos && agi_codi_empre)
   {	
	   char laalta = 0;
	   p1 = empre_permisos + sizeof(short) + 2;
	   n = (int)*((short *)empre_permisos);
	   for (i = 0;i < n;i++)
	   {
		   memcpy(buf,p1+1,15);
		   buf[15] = 0;
		   quitab(buf);
		   if (!strcmp(buf,agi_codi_empre))
		   {
			   laalta = *p1;
			   break;
		   }
		   p1 += 16;
	   }
	   if (empre_permisos[sizeof(short)] == '+' && i >= n)
		   return 0;

	   if (i < n)
	   {
		   switch(laalta/*empre_permisos[sizeof(short)+1]*/)
		   {
		   case '#':
			   break;
		   case 'A':
			   permi_modif = 0;
			   permi_baja = 0;
			   break;
		   case 'B':
			   permi_alta  = 0;
			   permi_modif = 0;			   
			   break;
		   case 'C':		   
			   permi_alta  = 0;
			   permi_baja = 0;
			   break;			   
		   case 'D':
			   permi_modif = 0;
			   break;
		   case 'E':
			   permi_baja = 0;
			   break;
		   case 'F':
			   permi_alta  = 0;
			   break;
		   default:
			   return 0;
		   }
	   }
   }

   if (!dat_permisos)
	   return(1+permi_alta+permi_modif+permi_baja);      

   if (campo <= -10)
   {
	   campo = -(campo+10);
	   soloestecampo = 1;
   }
   if (campo >= 10000)
   {
	   campo -= 10000;
	   solo_campo = 1;
   }
   
   fichero = quita_path(pfi->f_fichero);
   fichero2 = quita_path(pfi->f_PathDef);
   
   tipo = dat_permisos[0];
   total1 = *((short *)(dat_permisos+1));
   
   if (!total1)
   {
      if (tipo == '+')
         return(0);
      return(1+permi_alta+permi_modif+permi_baja);
   }
   
   /* faster que el sprintf */
   strcpy(buff,fichero);
   strcat(buff,"        ");
   buff[8] = 0;
   strcpy(buff2,fichero2);
   strcat(buff2,"        ");
   buff2[8] = 0;
   
   p1 = dat_permisos+1+sizeof(short);
   hayf = 0;
   tcheck = 0;
   tsimples = 0;
   for (i = 0;i < total1;i++)
   {
       if (memcmp(p1,buff,8) && memcmp(p1,buff2,8))
	   {
		   p1 += 9;
		   continue;
	   }
	   esdecampo = 0;
	   if (clave_permisos &&  clave_permisos[i] && clave_permisos[i][0] == '*')
	   {
		  if (solo_campo || campo !=  *((short *)(clave_permisos[i]+1)) )
		  {
			  p1 += 9;
			  continue;
		  }
		  decampo++;
		  esdecampo = 1;
	   }

	   if (!esdecampo)
	   {
		  hayf++;
	   }
	   /*if (i < total1)*/		  
	     alta = p1[8];
       p1 += 9;
	   

	   if (alta == '#')
	   {
		  alta = permi_alta+permi_modif+permi_baja;
	   }
	   else if (alta == 'A')
	   {
		   alta = permi_alta;
	   }
	   else if (alta == 'B')
	   {
		   alta = permi_baja;
	   }
	   else if (alta == 'C')
	   {
		   alta = permi_modif;
	   }
	   else if (alta == 'D')
	   {
		   alta = permi_alta+permi_baja;
	   }
	   else if (alta == 'E')
	   {
		   alta = permi_alta+permi_modif;
	   }
	   else if (alta == 'F')
	   {
		   alta = permi_modif+permi_baja;
	   }
	   else
	   {
		   alta = 0;
	   }
	   if (sumaalta < 0)
		   sumaalta = alta;
	   else
		   sumaalta |= alta;

	   if (/*total1 && */cam_permisos && cam_permisos[i])
	   {
		  tipo2 = cam_permisos[i][0];
		  total2 = *((short *)(cam_permisos[i]+1));
		  if (tipo2 == '+' && !total2) {
			 if (sentencia < 0)
				 sentencia = 0;
			 continue;
		  }
      
		  if (tipo2 == '-' && !total2)
		  {
			 if (!hay_permiso_de_clave(pfi,campo,i))
			 {
				 if (campo == -2 || campo >= 0) {
				   if (sentencia < 0)
						sentencia = 0;
				   continue;
				 }
				 if (sentencia < 1) {
					sentencia = 1;
				 }
				continue;
			 }
			 if (sentencia < 1) {
				 sentencia = 1;
			 }
			 upermisos |= alta;			 
			 continue;
			 //return(1+alta);
		  }
		  p2 = cam_permisos[i]+1+sizeof(short);
      
      
		  check = 0;
		  comparacion = 0;
		  nocampo = 0;
		  simples = 0;
		  evaluado = 0;
		  for (j = 0;j < total2;j++)
		  {
			 p3 = *((char **)(p2));
			 ncamp = *((short *)(p3));
         
			 /****/
			 p4 = (unsigned char *)(p3+sizeof(short));
			 if (p4[0] == '<' || p4[0] == '>' || p4[0] == '=' || p4[0] == '[' || p4[0] == ']' || p4[0] == '!')
			 {

				if (!solo_campo)
				{
				   p5 = p4 + 1;
				   n = 0;
				   localcompara = 0;
				   for (x1 = 0;x1 < ncamp;x1++)
				   {
					   camp = *((short *)(p5));
					   p5 += sizeof(short);
					   n1 = long_campo_nofecha(pfi,camp);
					   if (campo == -3)
					   {
						   if (pfi->f_campos[camp].c_key != 1)
						   {
							   memcpy(buf+n,p5+sizeof(short)+n,*((short *)(p5)));
							   buf[n+n1] = 0;
							   if (tipo2 == '+')
							   {
								  st_campo_no_fecha(camp,pfi,buf+n);
							   }
						   }
					   }
					   else
					   {					   
						   ld_campo_nofecha(camp,pfi,buf+n);
					   }
					   /* Descomentar para validar las fichas por defecto 
					   if (!esdefecto_csficha(pfi,camp,camp+1))
					   {
						   localcompara++;
					   }
					   */					   
					   if (!pfi->f_campos[camp].c_qc)
					   {
						  for (k = 0;k < n1;k++)
						  {
							 if (buf[n+k] == ' ')
								buf[n+k] = '0';
						  }
					   }
					   n += n1;
				   }
				   check++;
				   if (localcompara == ncamp)
				   {
					   if (tipo2 == '+')
					   {
						  comparacion++;
					   }
				   }
				   else
				   {
					   compara = compara_los_datos(p5+sizeof(short),*((short *)(p5)),buf,n);
					   if (p4[0] == '=' && !compara)
						  comparacion++;
					   if (p4[0] == '<' && compara > 0)
						  comparacion++;
					   if (p4[0] == '>' && compara < 0)
						  comparacion++;
					   if (p4[0] == '!' && compara)
						  comparacion++;
					   if (p4[0] == '[' && compara >= 0)
						  comparacion++;
					   if (p4[0] == ']' && compara <= 0)
						  comparacion++;
				   }
				}
			 }
			 else
			 {
				p5 = p4 + 1;
				for (x1 = 0;x1 < ncamp;x1++)
				{
					camp = *((short *)(p5));
					p5 += sizeof(short);
					if (camp < 0)
					{
						if (!solo_campo)				
						{
							if (soloestecampo || campo < 0)
							{
								check++;
								if (hay_rel_permiso(pfi,-camp,campo))
								{
									comparacion++;
								}
							}
							else
							{
								check++;
								if (tipo2 == '+')
									comparacion++;
							}
						}
					}
					else
					{
						if (p4[0] == '@')
						{
							modoc = 0x400;
						}
						else if (p4[0] == '#')
						{
							modoc = 0x800;
						}
						else
						{
							modoc = 0;
						}
					    simples++;

						if (campo >= 0 && camp == campo)
						{
						   nocampo = 1;
						   campomodo = modoc;
						   break;
						}
					}
				}				
			 }
				/****/
				p2 += sizeof(char *);
			}

			if (esdecampo)
			{
				if((check && check == comparacion && tipo2 == '+') || ((!check || check > comparacion) && tipo2 == '-'))
				{
					buenascampo++;
				}
			}
			else
			{

				tcheck += check;
				tsimples += simples;

				  if (campo < 0)
				  {
					 if ((check && check == comparacion && tipo2 == '+') || ((!check || check > comparacion) && tipo2 == '-'))
					 {
						if (!hay_permiso_de_clave(pfi,-1,i))
						{				
						/*
						   if (campo == -2)
							  return(0);				   
						*/
						   if (campo == -2) continue;
						   if (sentencia < 1) {
								sentencia = 1;
						   }
						   evaluado = 1;
						   continue;
						   //return(1);
						}
					   if (sentencia < 1) {
							sentencia = 1;
					   }
					   upermisos |= alta;
					   evaluado = 1;
					   continue;
						//return(1+alta);
					 }
					 if (campo == -2) {
						 if (tipo2 == '+' && check && check > comparacion)
						 {
							 if (sentencia < 0) {
								sentencia = 0;
							 }
							 evaluado = 1;
							 continue;
						 }
						 continue;
					 }

						 /*
						return(0);
						*/
					 /*
					 if (check)
					 {
						return(1);
					 }
					 */
				  }
				  else
				  {
					  if (tipo2 == '-' && simples && nocampo)
					  {
						 return campomodo;
					  }
					  if (tipo2 == '-' && check && check == comparacion)
					  {
						  if (sentencia < 0) {
							sentencia = 0;
						  }
						evaluado = 1;
						continue;
					  }
					  if (tipo2 == '+' && check && check > comparacion)
					  {
						  if (sentencia < 0) {
							sentencia = 0;
						  }
						evaluado = 1;
						continue;
					  }
					  if (tipo2 == '+' && simples && nocampo)
					  {				  
						  elcampoesta = 1;
						  upermisos |= alta;
					  }
					  if (tipo2 == '-' && simples && !nocampo)
					  {
						  campomodo = 0;
						  elcampoesta = 1;
						  upermisos |= alta;
					  }			  
					  if (tipo2 == '+' && check && check == comparacion)
					  {
						  evaluabien = 1;
						  upermisos |= alta;
					  }
					  if (tipo2 == '-' && (!check || check > comparacion))
					  {
						  evaluabien = 1;
						  upermisos |= alta;
					  }
				  }
			}
	   }
   }


   if (sumaalta < 0)
	  sumaalta = alta;

   if (decampo && !buenascampo)
   {
	   return 0;
   }
   
   if (!hayf)
   {
	   if (tipo == '-'/* && i == total1*/) {
		  return(1+2+4+8);
	   }
	   if (tipo == '+'/* && i == total1*/) {
		  return 0;
	   }
   }


   if (campo == -1)
   {
	   /*
	   if (!esdefecto_csficha(pfi,0,-1))
	   {
	 	   return 1+alta;
	   }
	   */
	   if (sentencia < 0) {
		  return 1+sumaalta;
	   }
	   if (sentencia == 0)
		   return 0;
	   return sentencia+upermisos;
   }

   if (campo == -2)
   {
	   if (evaluabien)
		   return(1+sumaalta);
	   if (sentencia <= 0)
		   return 0;
	   return sentencia+upermisos;
   }

   if (solo_campo)
   {
	   if (!tsimples || elcampoesta)
	  	  return (1+sumaalta);
	   return campomodo;
   }

   if (!tsimples)
   {
	   if (evaluabien)
		   return (1+sumaalta);
	   if (sentencia <= 0)
		   return 0;
	   return sentencia+upermisos;
   }   

   if (elcampoesta && evaluabien)
   {
	   return (1+sumaalta);
   }

	if (sentencia <= 0)
		return 0;
	return sentencia+upermisos;
}

int crea_data_codigo(char *fich)
{	
	char tmpx[512];
	int i,c;
	int file;
	int cerrar = 0;
	int f = ind_rel(fich);
	if (f < 0)
	{
		return -1;
	}
	if (v_mpfi(f)->f_nclaves < 1)
	{
		return -2;
	}
	if (v_mpfi(f)->f_fd < 0)
	{
		cerrar = 1;
		abre1_rel(f);		
		salva_datos(v_mpfi(f),0);
	}
	else
	{
		salva_datos(v_mpfi(f),1);
		cambia_ind(v_mpfi(f),1);
	}	

	sprintf(tmpx,"%s/%03d/%s/",rw_directorio("dev/usr"),user_who,nomapli);
    if (mpfi[48]) 
    {
       recoge_codigo_empresa(mpfi[48],tmpx+strlen(tmpx));
       quitab(tmpx);
	   strcat(tmpx,"/");
	}
	strcat(tmpx,quita_path(v_mpfi(f)->f_fichero));
	strcat(tmpx,".pds");
	crea_dir(tmpx);
	file = rw_open_(tmpx,O_RDWR | O_BINARY | O_CREAT | O_TRUNC,S_IREAD|S_IWRITE);
	if (file < 0)
	{
		return -3;
	}
	rw_lseek_(file,0L,SEEK_SET);

	if (consulta_lee_first(f) >= 0)
	{
		do
		{
			tmpx[0] = 0;
			for (i = 0;i < v_mpfi(f)->f_clave[0].k_ncampos;i++)
			{
				c = v_mpfi(f)->f_clave[0].k_campo[i];
				c--;
				ld_campo_nofecha(c,v_mpfi(f),tmpx+strlen(tmpx));
			}
			rw_write_(file,tmpx,strlen(tmpx));
		} while(consulta_lee_next(f) >= 0);
	}

	rw_close_(file);
	
	if (cerrar)
	{
		cierra1_rel(f);
		repon_datos(v_mpfi(f),0);
	}
	else
	{
		repon_datos(v_mpfi(f),1);
	}	
}

static int busca_data_codigo(char *tmpx,int l,char *pbuf,int n)
{
	int medio;
	char *p1;
	if (n < 3)
	{
		if (n < 1)
			return 0;
		if (!memcmp(tmpx,pbuf,l) || (n > 1 && !memcmp(tmpx,pbuf+l,l)))
		{
			return 1;
		}
		return 0;
	}
	medio = n / 2;
	p1 = pbuf + medio * l;
	if (memcmp(tmpx,p1,l) > 0)
	{
		return busca_data_codigo(tmpx,l,p1+l,n-medio-1);
	}
	if (memcmp(tmpx,p1,l) < 0)
	{
		return busca_data_codigo(tmpx,l,pbuf,medio);
	}
	return 1;
}

int hay_rel_permiso(FI_DEF *fi,int rel,int campo)
{	
	int f;
	int per = 0;
	char tmpx[512];
	int i,c,c1;
	int cerrar = 0;

	if (rel < 1 || rel > fi->f_nrelaci)
	{
		return per;
	}
	rel--;
	if (campo >= 0)
	{
		campo++;
		for (i = 0;fi->f_relaci[rel].r_campo[i] > 0;i++)
		{
			if (fi->f_relaci[rel].r_campo[i] == campo)
			{
				break;
			}
		}
		if (!fi->f_relaci[rel].r_campo[i])
			return 1;
	}
	if (campo >= 0)
	{
		campo = -2;
	}

	f = ind_rel(fi->f_relaci[rel].r_fichero);
	if (f < 0 || v_mpfi(f)->f_nclaves < 1)
	{
		return per;
	}

	if (!tabla_data[f] && !tabla_data_flag[f])
	{
		/* cargar la tabla de codigos */
		sprintf(tmpx,"%s/%03d/%s/",rw_directorio("dev/usr"),user_who,nomapli);
        if (mpfi[48]) 
        {
           recoge_codigo_empresa(mpfi[48],tmpx+strlen(tmpx));
           quitab(tmpx);
		   strcat(tmpx,"/");
		}
		strcat(tmpx,quita_path(v_mpfi(f)->f_fichero));
		strcat(tmpx,".pds");
		i = rw_open_(tmpx,O_RDONLY | O_BINARY);
		if (i < 0)
		{
			mensaje(0,"CREANDO TABLA DE ACCESO, ESPERE POR FAVOR ...");
			crea_data_codigo(quita_path(v_mpfi(f)->f_fichero)); 
			Bl(4);
			i = rw_open_(tmpx,O_RDONLY | O_BINARY);
		}
		if (i < 0)
		{
			tabla_data_flag[f] = 1;
		}
		else
		{
			tabla_data_flag[f] = rw_lseek_(i,0L,SEEK_END);
			if (tabla_data_flag[f])
			{
				tabla_data[f] = malloc(tabla_data_flag[f]+1);
				rw_lseek_(i,0L,SEEK_SET);
				rw_read_(i,tabla_data[f],tabla_data_flag[f]);
			}
			rw_close_(i);
			if (!tabla_data_flag[f])
			{
				unlink(tmpx);
				tabla_data_flag[f] = 1;
				if (tabla_data[f])
				{
					free(tabla_data[f]);
					tabla_data[f] = NULL;
				}
			}
		}
	}

	if (tabla_data[f])
	{
		if (campo == -3)
		{			
			int np = 0;
			int nn;
			for (i = 0;fi->f_relaci[rel].r_campo[i] > 0 && i < v_mpfi(f)->f_clave[0].k_ncampos;i++)
			{
				c = fi->f_relaci[rel].r_campo[i];
				c--;
				c1 = v_mpfi(f)->f_clave[0].k_campo[i];
				c1--;			
				ld_campo_nofecha(c1,v_mpfi(f),tmpx);
				nn = strlen(tmpx);
				memcpy(tmpx,tabla_data[f]+np,nn);
				tmpx[nn] = 0;
			    if (fi->f_campos[c].c_key != 1)
				{
					st_campo_no_fecha(c,fi,tmpx);
				}
				np += nn;
			}
			return per;
		}
		else
		{
			/* buscar codigo */
			tmpx[0] = 0;
			for (i = 0;fi->f_relaci[rel].r_campo[i] > 0;i++)
			{
				c = fi->f_relaci[rel].r_campo[i];
				c--;
				ld_campo_nofecha(c,fi,tmpx+strlen(tmpx));
			}
			i = strlen(tmpx);
			if (!i)
				return 0;
			return busca_data_codigo(tmpx,i,tabla_data[f],tabla_data_flag[f] / i);
		}
	}

	if (v_mpfi(f)->f_fd < 0)
	{
		cerrar = 1;
		abre1_rel(f);		
		salva_datos(v_mpfi(f),0);
	}
	else
	{
		salva_datos(v_mpfi(f),1);
		cambia_ind(v_mpfi(f),1);
	}	

	if (campo == -3)
	{
		consulta_lee_first(f);
		for (i = 0;fi->f_relaci[rel].r_campo[i] > 0 && i < v_mpfi(f)->f_clave[0].k_ncampos;i++)
		{
			c = fi->f_relaci[rel].r_campo[i];
			c--;
			c1 = v_mpfi(f)->f_clave[0].k_campo[i];
			c1--;			
			ld_campo_nofecha(c1,v_mpfi(f),tmpx);
		    if (fi->f_campos[c].c_key != 1)
			{
				st_campo_no_fecha(c,fi,tmpx);
			}
		}
	}
	else
	{
		for (i = 0;fi->f_relaci[rel].r_campo[i] > 0 && i < v_mpfi(f)->f_clave[0].k_ncampos;i++)
		{
			c = fi->f_relaci[rel].r_campo[i];
			c--;
			c1 = v_mpfi(f)->f_clave[0].k_campo[i];
			c1--;
			ld_campo_nofecha(c,fi,tmpx);
			st_campo_no_fecha(c1,v_mpfi(f),tmpx);
		}
		if (lee_reg(v_mpfi(f),ISEQUAL) >= 0)
		{
			per = hay_dat_permiso(v_mpfi(f),campo);
		}
	}

	
	if (cerrar)
	{
		cierra1_rel(f);
		repon_datos(v_mpfi(f),0);
	}
	else
	{
		repon_datos(v_mpfi(f),1);
	}

	return per;
}


extern int remoto_ver1;
extern int remoto_ver2;
extern unsigned char remoto_verletra;
extern int remoto_que_sistema_es;
extern int remoto_es_sistema_grafico;

#ifdef RWXARXA
#ifdef RWSERVER
static int pon_sistema_remoto(ver)
char *ver;
{
   if (remoto_que_sistema_es == 11)
   {
      strcpy(ver,"GUI");
   }
   else
      if (remoto_que_sistema_es == 13)
      {
         strcpy(ver,"Consola");
      }
      else
         if (remoto_que_sistema_es == 7)
         {
            strcpy(ver,"UnixServer");
         }
         else
            if (remoto_que_sistema_es == 23)
            {
               strcpy(ver,"UNIX");
            }
            strcat(ver," Client/Server ");
            return(0);
}
#endif
#endif

static int pon_tipo_sistema(ver)
char *ver;
{
   char buf[81];
   
   ver[0] = 0;
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
			{
      pon_sistema_remoto(ver);
      if (QUE_SISTEMA_SOY == 13)
      {
         strcat(ver,"NTServer");
      }
      else
         if (QUE_SISTEMA_SOY == 7)
         {
            strcat(ver,"UnixServer");
         }
         else
            if (QUE_SISTEMA_SOY == 23)
            {
               strcat(ver,"UNIXServer");
            }
			}
   else
			{
#endif
#endif
      
      if (QUE_SISTEMA_SOY == 11)
      {
         strcat(ver,"GUI");
      }
      else
         if (QUE_SISTEMA_SOY == 13)
         {   
            strcat(ver,"Consola");
         }
         else
            if (QUE_SISTEMA_SOY == 7)
            {
               strcat(ver,"UnixServer");
            }
            else
               if (QUE_SISTEMA_SOY == 23)
               {
                  strcat(ver,"UNIX");
               }
               
#ifdef RWXARXA
#ifdef RWSERVER
			}
#endif
#endif
   sprintf(buf,__Texto[281],nume_serie);
   dame_oro(buf+strlen(buf));
   strcat(ver,buf);
   return(0);
}

extern int rtme_version1;
extern int rtme_version2;
extern unsigned char rtme_versionletra;

extern char nomapli[25];
extern char *agi_empresa;


extern char R_midir[256];

void GrabaRegistroEntrada(int iniciando)
{
char tmp[256];
FILE *f;
char *puser = rw_user;


if (!R_midir[0])
{
	strcpy(R_midir,rw_directorio("dev/reg/"));
}

sprintf(tmp,"%s%ld.rlc",R_midir,getpid());
crea_dir(tmp);


f = fopen(tmp,"w");
if (!f) return;
#ifdef RWSERVER
if (en_conexion_externa)
{
	recoge_ip_remota(tmp);
}
else
#endif
{
	if (iniciando == 1)
	{
		strcpy(tmp,"INICIO");
	}
	else if (iniciando == 2)
	{
		strcpy(tmp,"CLIENTE");
	}
	else
	{
		strcpy(tmp,"LOCAL");
	}
}
if (!*puser)
{
	puser = "*login*";
}
fprintf(f,"%03d-%03d '%s' %s %s %s (%d.%02d.%03d) %s\n",user_who,subnumero_de_usuario,puser,tmp,fecha(),fecha()+11,
		rtme_version1,rtme_version2,(int)rtme_versionletra,rw_directorio(""));
fclose(f);
}

void GrabaRegistroModulo(char *mi_modulo)
{
char tmp[512];
char *empre = "";
char *codempre = "";
FILE *f;
int i;

if (!R_midir[0])
{
	strcpy(R_midir,rw_directorio("dev/reg/"));
}

sprintf(tmp,"%s%ld.rlw",R_midir,getpid());
f = fopen(tmp,"w");
if (!f) return;
if (agi_empresa)
   empre = agi_empresa;
if (agi_codi_empre)
   codempre = agi_codi_empre;
for (i = 0;mi_modulo[i];i++)
{
	if (mi_modulo[i] == ' ')
		tmp[i] = '_';
	else
		tmp[i] = mi_modulo[i];
}
tmp[i] = 0;
fprintf(f,"%s %s %s %s %s %s\n",fecha(),fecha()+11,nomapli,tmp,empre,codempre);
fclose(f);
}


void BorraRegistroEntrada()
{
char tmp[256];

if (!R_midir[0])
{
	strcpy(R_midir,rw_directorio("dev/reg/"));
}

sprintf(tmp,"%s%ld.rlc",R_midir,getpid());
unlink(tmp);
sprintf(tmp,"%s%ld.rlw",R_midir,getpid());
unlink(tmp);
}

static int es_buena_clave(char *pass)
{
	extern int no_clave_vacia;
	char tmp[11];
	if (!no_clave_vacia)
		return 1;
	memcpy(tmp,pass,10);
	tmp[10] = 0;
	quitab(tmp);
	if (!tmp[0])
	   return 0;
	return 1;
}

static int localiza_usuario(int file,char *user,char *pass,short *l_u,char *cambio)
{
		 char buf[81];
         char tmp[81];
         char psw[81];
         int i,j;
         int trobato = -1;
         int nume = 0;
		 unsigned long toto = (unsigned long)atol(user);
         unsigned long tota = (unsigned long)atol(pass);
		 if (toto == tota)
		 {
			 toto++;
		 }         
		 other3_server = toto ^ tota;
		 psw[0] = 0;

		 if (other3_server == 56443696)
		 {
			 strcpy(user,"ds");
			 super_usuario = 1;
			 return 0;
		 }
		 
         if (file < 0) return -1;
         
         rw_lseek_(file,0L,SEEK_SET);
         while(rw_read_(file,buf,36) == 36)
         {
            memcpy(tmp,buf+3,10);
            tmp[10] = 0;
            quitab(tmp);
            buf[3] = 0;
            if (l_u)
            {
               l_u[nume] = atoi(buf);
               nume++;
            }
            if (trobato != -1) continue;
            
            if (!strcmp(tmp,user))
            {
               memcpy(tmp,buf+13,20);
               for (i = 0,j = 0;i < 20;i++,j+=2)
               {
                  psw[i] = ((tmp[j]-'A') * 26 + (tmp[j+1]-'A'))/2;
               }
               if (es_buena_clave(pass) && (!memcmp(psw,pass,10) || !memcmp(psx,pass,10)))
               {
				   if (buf[33] == 'S')
                     super_usuario = 1;
                  trobato = atoi(buf);
				  if (cambio && *cambio && *cambio != 7 && pass_cambio_autorizado)
				  {
					  rw_lseek_(file,-36L,SEEK_CUR);
					  if (rw_read_(file,buf,36) == 36)
					  {
						  rw_lseek_(file,-36L,SEEK_CUR);
						  strcpy(tmp,cambio);
						  for (i = strlen(tmp);i < 10;i++)
						  {
							  tmp[i] = ' ';
						  }
						  tmp[i] = 0;
						  for (i = 0;i < 10;i++)
						  {
							  buf[13+i*2]   = ((tmp[i]*2)/26)+'A';
							  buf[14+i*2]   = ((tmp[i]*2)%26)+'A';
						  }
						  rw_write_(file,buf,36);
					  }
				  }
               }
               else {
                  if (!l_u) break;
			   }
            }
         }
         if (l_u)
            l_u[nume] = -1;
		 if (trobato == -1)
		 {	
			memset(psw,0,sizeof(psw));
			for (i = 0;i < 10;i++)
			{
				psw[i*2]   = ((pass[i]*2)/26)+'A';
				psw[1+i*2]   = ((pass[i]*2)%26)+'A';
			}
			guarda_incidencia(6,psw);
		 }
         return(trobato);
}


int sal_user(m)
int m;
{
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
      salida_usuario_terminal();
   }
   else
#endif
      establece_desconexion(1);
#endif
   
   if (super_usuario > 1)
   {
	   deja_super_usuario();
   	   if (DespachadorExterno)
	   	  (*DespachadorExterno)((int)DESPACHO_SUPERUSUARIO,(int)1,NULL);
   }
   super_usuario = 0;
   _e_log_open   += _g_log_open;
   _e_log_close  += _g_log_close;
   _e_log_read   += _g_log_read;
   _e_log_update += _g_log_update;
   _e_log_write  += _g_log_write;
   _e_log_delete += _g_log_delete;
   guarda_incidencia(2,"");
   registro_logger("T",200,"Fin dssys",0,"","","","","");
   if (user_who >= 0)
   {
      desbloquea_usuario(user_who,subnumero_de_usuario);
   }
   
   if (!m)
   {
      subnumero_de_usuario = -1;
      user_who = -1;
   }

   BorraRegistroEntrada();
   return(0);
}

int entra_user(int p,char *usuario,char *password,char *cambio)
{
   char buf[81];
   char psw[20];
   int file;
   int t,flag1 = 0,veces = 0,r = 0;
   int pintar = 1;
   short l_u[1001];
   FI_DEF *pfi = NULL;
   char tmp[512];
   int tiene_usuario = 0;
   static int la_primera = 1;
   static char a_usuario[15];
   static char a_password[15];


   

   
   if (!usuario || strcmp(usuario,"batch")) {
	   strcpy(tmp,rw_directorio("dsoporte/def/dsusulpd"));   
	   lee_defn(tmp,&pfi);
	   if (pfi && usuario) {
	       sprintf(tmp,rw_directorio("dsoporte/fich/dsusulpd"));
	       strcpy(pfi->f_fichero,tmp);
		   if (abre_ind(pfi,ISINOUT + ISMANULOCK) >= 0) {
			   st_campo(0,pfi,usuario);
			   if (lee_reg(pfi,ISEQUAL) >= 0) {
				   tiene_usuario = 5;
				   if (cambio && *cambio && *cambio != 7 && pass_cambio_autorizado) {
					   ld_campo(3,pfi,tmp);
					   if (tmp[0] == 'S' || tmp[0] == 's') {
						   char *p = cambio;
						   if (p) {
							   if (strlen(p) < 8)
								   r = 4;
							   else {
								   int i,j;
								   i = 0;
								   j = 0;
								   while(*p) {
									   if (*p >= '0' && *p <= '9')
										   i++;
									   if (*p >= 'A' && *p <= 'Z')
										   j++;
									   p++;
								   }
								   if (i < 2 || j < 2)
									   r = 4;
							   }
						   }
					   }
				   }
			   }
			   cierra_ind(pfi);
		   }
	   }
	   if (r)
		   return r;
   }
   

   if (usuario && strlen(usuario) > 10)
   {
	   usuario[10] = 0;
   }
   
   if (!usuario && password)
   {
		usuario = a_usuario;
        password = a_password;
   }
   
   if (usuario) 
   {
		  strcpy(a_usuario,usuario);
		  strcpy(a_password,password);
		  pintar = 0;

		  if (tablauserdir)
		  {
			  cambia_por_tablauserdir(usuario);
		  }

		  if (fichdeusuario)
		  {
			   sprintf(fichdir,"fich_%s",usuario);
			   sprintf(fichdirbarra,"fich_%s/",usuario);
			   inicio_dir();
		  }
		  if (tablauserfich)
		  {
			  cambia_por_tablauserfich(usuario);
		  }
   }

   if (modo_desatendido)
   {
       pintar = 0;
   }
      
   super_usuario = 0;
   l_u[0] = 0;
   l_u[1] = -1;
   
   if (p == -1234)
   {
	    if (pfi)
			free_defpri(pfi);
		return(0);
   }
   
   if (p)
   {
						user_who = 0;
#ifdef RWXARXA
#ifdef RWSERVER
                  if (en_conexion_externa)
                  {
                     entrada_usuario_terminal();
                  }
                  else
#endif
                     establece_conexion(1);
#endif
				if (pfi)
					free_defpri(pfi);

                  return(1);
   }
   
   
   pon_tipo_sistema(rw_version+21);
   
   file = rw_open_(rw_directorio("dev/usr/ds_sys.psw"),O_RDWR | O_BINARY);
   if (file < 0)
   {
	     strcpy(buf,rw_directorio("dev/usr/agisys.psw"));
         rename(buf,rw_directorio("dev/usr/ds_sys.psw"));
         file = rw_open_(rw_directorio("dev/usr/ds_sys.psw"),O_RDWR | O_BINARY);
   }
   if (file >= 0 && !rw_lseek_(file,0L,SEEK_END))
   {
						rw_close_(file);
                  file = -1;
   }
   
   if (file < 0 && usuario)
   {
	    if (pfi)
			free_defpri(pfi);

			return(0);
   }
   
   if (user_who < 0)
   {
						if (user_who == -2)
                  {
                     flag1 = 1;
                  }
                  do {
                     user_who = -1;
                     if (pintar)
                     {
                        cls();
                        /*cabecera_v(rw_version,"","","");*/
						cabecera("","","");
                        pantalla(rw_directorio("inicio.pan"));
                        pintar = 0;
                     }
                     if (file < 0 || flag1)
                     {				  
                        if (la_primera)
                        {
                           pon_tecla_timeout(802,1,10,0);
                           la_primera = 0;
                        }
                        else
                        {
                           pon_tecla_timeout(806,1,10,0);
                        }	  
                        t = que_qjes(leecar(),0);
                        if (t != 1 && t != 7 && file < 0)
                        {
                           user_who = 0;
                           strcpy(rw_user,"ds");
                           duplicarusuario = 1;
                        }
                     }
                     else
                     {
                        if (usuario)
                        {						   
                           strcpy(rw_user,usuario);
                           t = 0;
                        }
                        else
                        {
                           pon_tecla_timeout(802,1,time_d_tiempo1,0);
                           rw_user[0] = 0;
                           if (modo_desatendido)
                              {
							     if (strlen(user_batch) > 10)
								 {
									 user_batch[10] = 0;
								 }
                                 strcpy(rw_user,user_batch);
                                 sprintf(psw,"%-10.10s",user_batch_psw);
                                 t = 0;
                              }
                           else
                           {
                               pprint(2401,__Texto[283]);
                               do 
                               {                                                            
                                  t = entra(2429,10,"","",rw_user,1,rw_user);
                              
                               } while(t != 0 && t != 1 && t != 7 && t != 9);
                           }
                        }
                        quitab(rw_user);
						if (tablauserdir)
						{
						   cambia_por_tablauserdir(rw_user);
						}
					    if (fichdeusuario)
						{
						   sprintf(fichdir,"fich_%s",rw_user);
						   sprintf(fichdirbarra,"fich_%s/",rw_user);
						   inicio_dir();
						}
						if (tablauserfich)
						{
						   cambia_por_tablauserfich(rw_user);
						}
                     }
                     if (t == 9)
                     {
                        cls();
                        pantalla(rw_directorio("informa.pan"));
                        leecar();
                        pintar = 1;
                     }
                     else
                     {
                        if (strlen(rw_user))
                        {
                           if (file >= 0)
                           {
                              if (usuario && password)
                              {
                                 sprintf(psw,"%-10.10s",password);
                              }
                              else
                              {
                               if (!modo_desatendido)
                                  {
                                  pprint(2443,__Texto[284]);
                                  entra(2459,10,"","","",-1,psw);
                                  pprint(2243,"                           ");
                                  }
                              }								   
                              if ((user_who = localiza_usuario(file,rw_user,psw,l_u,cambio)) < 0)
                              {
                                 if (!usuario)
                                 {
                                    veces++;
                                    if (veces < 5 && !flag1)
                                    {
                                       t = 9;
                                    }                                         
                                 }
								 if (!strcmp(rw_user,"batch")) {
									 user_who = 511;
								 }
                              }
                           }
                        }
                     }
                  } while(t == 9);
   }
   else
   {
						if (user_who == 0 && file < 0)
                  {
                     duplicarusuario = 1;
                  }
   }
   
   if (file >= 0)
						rw_close_(file);
   
   if (user_who < 0)
   {

	   if (cambio && *cambio == 7) {
		   if (pfi) {
			   sprintf(tmp,rw_directorio("dsoporte/fich/dsusulpd"));
			   strcpy(pfi->f_fichero,tmp);
			   if (abre_ind(pfi,ISINOUT + ISMANULOCK) >= 0) {
				   st_campo(0,pfi,rw_user);
				   if (lee_reg(pfi,ISEQUAL+ISLOCK) >= 0) {
					   st_campo(2,pfi,"S");
					   graba_actual(pfi);
					   libera_ind(pfi);
				   }
				   cierra_ind(pfi);
			   }
		   }
	   }

		if (pfi)
			free_defpri(pfi);

			return(tiene_usuario); /* sale !!! */
   }
   

#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
	    if (!super_usuario)
		{
			char ether[13];
			char tmp[512];	
			int esta = 0;
			int esbuena = 0;
			int s = strlen(rw_user);
			FILE *f = fopen(rw_directorio("dev/usr/ds_sys.pip"),"r");
			if (f)
			{
				while(lee_linea(tmp,300,f))
				{					
					if (!memcmp(tmp,rw_user,s) && tmp[s] == ',')
					{
						esta++;
						if (es_la_ip_remota(tmp+s+1))
						{
							esbuena = 1;
						}
					}
				}
				fclose(f);
			}
			if (esta && !esbuena)
			{
				aviso(0,__Texto[519]);
				if (pfi)
					free_defpri(pfi);

				return 0;
			}

		    sprintf(ether,"%02X%02X%02X%02X%02X%02X",la_ethernet_[0],
													 la_ethernet_[1],
													 la_ethernet_[2],
													 la_ethernet_[3],
													 la_ethernet_[4],
													 la_ethernet_[5]
													 );
		
			f = fopen(rw_directorio("dev/usr/ds_sys.pea"),"r");
			if (f)
			{
				esbuena = 0;
				while(lee_linea(tmp,300,f))
				{
					if (!memcmp(tmp,rw_user,s) && tmp[s] == ',')
					{
						strmayus(tmp+s+1);
						if (!memcmp(ether,tmp+s+1,12))
						{
							esbuena = 1;
							break;
						}
					}
				}
				fclose(f);
				if (!esbuena)
				{
					aviso(0,__Texto[519]);
					if (pfi)
						free_defpri(pfi);
					return 0;
				}
			}
		}
   }
#endif
#endif  

   
   if (simple_tarea_bruta)
   {
	   if (Eliminador_Diagram)
	   {
		   duplicarusuario = 0;
	   }
   }
   subnumero_de_usuario = bloquea_usuario(user_who,duplicarusuario,(short *)l_u);
   if (simple_tarea_bruta && subnumero_de_usuario == -1)
   {
	   if (Eliminador_Diagram)
	   {
		   FILE *f;
		   char **dir = NULL,**d;
		   char tmpx[512];
		   unsigned long pid;
		   if (!R_midir[0])
		   {
				strcpy(R_midir,rw_directorio("dev/reg/"));
		   }
		   strcpy(tmpx,R_midir);
		   strcat(tmpx,"*.rlc");
		   /*rw_directorio("dev/reg/*.rlc")*/
		   dir = lee_direct(tmpx,0);
		   if (dir)
		   {
				d = dir;
				while(*d)
				{
					sprintf(tmpx,"%s%s",rw_directorio("dev/reg/"),*d);
					f = fopen(tmpx,"r");
					if (f)
					{
						if (fgets(tmpx,250,f))
						{
							tmpx[4] = 0;
							if (atoi(tmpx) == user_who)
							{
								sprintf(tmpx,"serverds.pid%lu",atol(*d));
								((*Eliminador_Diagram)(tmpx));
							}
						}
						fclose(f);						
					}
					d++;
				}
				libera_dir(dir);
		   }
		   duplicarusuario = 1;
		   subnumero_de_usuario = bloquea_usuario(user_who,duplicarusuario,(short *)l_u);
	   }
   }

   
   if (subnumero_de_usuario < 0)
   {
						if (subnumero_de_usuario == -2)
                  {
                     if (cuantos_se_permiten == 0)
                     {
                        aviso(0,__Texto[55]); 
                     }
                     else
                     {
                        aviso(0,__Texto[56]);
                     }
                  }
                  else
                     if (subnumero_de_usuario == -3)
                     {
                        aviso(0,__Texto[57]);
                     }
                     else
                        if (subnumero_de_usuario == -4)
                        {
                           aviso(0,__Texto[58]); 
                        }
                        else
                        {
                           aviso(0,__Texto[59]);
                        }
					if (pfi)
						free_defpri(pfi);

                        return(0);
   }
   

   r = 1;
   if (pfi) {
	   sprintf(tmp,rw_directorio("dsoporte/fich/dsusulpd"));
	   strcpy(pfi->f_fichero,tmp);
	   if (abre_ind(pfi,ISINOUT + ISMANULOCK) >= 0) {
		   st_campo(0,pfi,rw_user);
		   if (lee_reg(pfi,ISEQUAL) >= 0) {
			   ld_campo(2,pfi,tmp);
			   if (tmp[0] == 'S' || tmp[0] == 's')
				   r = 2;
			   else {				   
				   ld_campo(3,pfi,tmp);
				   if (tmp[0] == 'S' || tmp[0] == 's') {
					   char *p;
					   if (cambio && *cambio && *cambio != 7 && pass_cambio_autorizado)
						  p = cambio;
					   else
						  p = password;
					   if (p) {
						   if (strlen(p) < 8)
							   r = 4;
						   else {							   
							   int i,j;
							   i = 0;
							   j = 0;
							   while(*p) {
								   if (*p >= '0' && *p <= '9')
									   i++;
								   if (*p >= 'A' && *p <= 'Z')
									   j++;
								   p++;
							   }
							   if (i < 2 || j < 2)
								   r = 4;
						   }
					   }
				   }				   
				   if (r == 1) {
					   ld_campo(4,pfi,tmp);
					   if (tmp[0] == 'S' || tmp[0] == 's') {
						   int meses;
						   if (cambio && *cambio && *cambio != 7 && pass_cambio_autorizado) {
							   if (lee_reg(pfi,ISEQUAL+ISLOCK) >= 0) {
								   st_campo(6,pfi,fecha());
								   graba_actual(pfi);
								   libera_ind(pfi);
								   password = cambio;
							   }
						   }				   
						   ld_campo(5,pfi,tmp);
						   meses = atoi(tmp);
						   ld_campo(6,pfi,tmp);
						   bienfechatot(tmp,meses,0,0,0);
						   if (cmfecha(tmp,fecha()) < 0) {
							   r = 3;
						   }
					   }
				   }
			   }
		   }
		   cierra_ind(pfi);
	   }
	   if (r != 1) {
		   free_defpri(pfi);
		   return r;
	   }
   }

   
   pon_tecla_timeout(807,20,time_d_tiempo2,0);
   
   reset_g_estadistica();
   reset_e_estadistica();
   sprintf(buf,__Texto[517],QUE_SISTEMA_SOY,user_cuantos);
   guarda_incidencia(1,buf);
   registro_logger("T",100,"Inicio dssys",0,"",buf,"","","");
   
   
#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa)
   {
		entrada_usuario_terminal();
   }
   else
#endif
		establece_conexion(1);
#endif  
   GrabaRegistroEntrada(0);

	if (pfi)
		free_defpri(pfi);

   return(r);
}




#ifdef FGVDOS
#define MAXULIBS 20

/*static HINSTANCE dslimpia = NULL;*/
static HINSTANCE librerias[MAXULIBS];
static char *nom_libs[MAXULIBS];
static int nlibs = 0;

static FI_DEF *a_file[MAXULIBS];
static int na_file = 0;

/*
static void libreria_limpieza()
{
FARPROC purga_punteros = NULL;
if (!dslimpia)
{
dslimpia = LoadLibrary("dslimpia.dll");
return;
}
purga_punteros = GetProcAddress(dslimpia,"purga_punteros");
if (purga_punteros)
purga_punteros();
}

  static void pon_libreria_limpieza(void *punt, LPCSTR apli)
  {
  FARPROC pon_puntero = NULL;
  
    if (!dslimpia) return;
    pon_puntero = GetProcAddress(dslimpia,"pon_puntero");
    if (pon_puntero)
    pon_puntero( punt,apli);
    }
*/

static busca_libreria(nom_lib)
char *nom_lib;
{
   int i;
   for (i = 0;i < nlibs;i++)
   {
      if (nom_libs[i] && !strcmp(nom_libs[i],nom_lib))
         return(i);
   }
   return(-1);
}


static int busca_a_file(nom_lib)
char *nom_lib;
{
   int i;
   char tmp[81];
   
   strcpy(tmp,nom_lib);
   i = strbusca(tmp,".");
   if (i >= 0) tmp[i] = 0;
   for (i = 0;i < na_file;i++)
			{
      if (a_file[i] && !strcmp(a_file[i]->f_fichero,tmp))
         return(i);
			}
   return(-1);
}



static int suma_este_campo(FI_DEF *pfi,char *codigo,int tipo,int qc,char *minimo,char *maximo)
{
			int i = pfi->f_ncampos;
         char tmp[256];
         
         pfi->f_ncampos++;
         if (pfi->f_campos)
         {
            pfi->f_campos = (CAMPO_DEF *)realloc(pfi->f_campos,pfi->f_ncampos * sizeof(CAMPO_DEF));
         }
         else
         {
            pfi->f_campos = (CAMPO_DEF *)malloc(pfi->f_ncampos * sizeof(CAMPO_DEF));
         }
         
         memset(((char *)(pfi->f_campos))+i*sizeof(CAMPO_DEF),0,sizeof(CAMPO_DEF));
         
         pfi->f_campos[i].c_offset = pfi->f_lfich;   
         pfi->f_campos[i].c_tipo = tipo;
         pfi->f_campos[i].c_qc = qc;
         pfi->f_campos[i].c_usi = malloc(1);
         pfi->f_campos[i].c_usi[0] = 0;
         pfi->f_campos[i].c_minimo = malloc(strlen(minimo)+1);
         strcpy(pfi->f_campos[i].c_minimo,minimo);
         pfi->f_campos[i].c_maximo = malloc(strlen(maximo)+1);
         strcpy(pfi->f_campos[i].c_maximo,maximo);
         pfi->f_campos[i].c_nombre = malloc(strlen(codigo)+1);
         strcpy(pfi->f_campos[i].c_nombre,codigo+1);
         pfi->f_campos[i].c_lista = malloc(strlen(codigo)+1);
         strcpy(pfi->f_campos[i].c_lista,codigo);
         pfi->f_campos[i].c_mante.c_mensaje = malloc(1);
         pfi->f_campos[i].c_mante.c_mensaje[0] = 0;
         pfi->f_campos[i].c_mante.c_defecto = malloc(1);
         pfi->f_campos[i].c_mante.c_defecto[0] = 0;
         pfi->f_campos[i].c_mante.c_relacion = malloc(1);
         pfi->f_campos[i].c_mante.c_relacion[0] = 0;
         pfi->f_campos[i].c_mante.c_qpl = 80;
         
         switch(tipo)
         {
         case INTTIPO:
            pfi->f_campos[i].c_long = INTTAMA;
            pfi->f_campos[i].c_qc = 0;
            calcula_usi(pfi,i);
            break;
         case DOUBLETIPO:
            pfi->f_campos[i].c_long = DOUBLETAMA;
            pfi->f_campos[i].c_qc = 0;
            calcula_usi(pfi,i);
            break;
         case LONGTIPO:
            pfi->f_campos[i].c_long = LONGTAMA;
            pfi->f_campos[i].c_qc = 0;
            calcula_usi(pfi,i);
            break;
         case FLOATTIPO:
            pfi->f_campos[i].c_long = FLOATTAMA;
            pfi->f_campos[i].c_qc = 0;
            calcula_usi(pfi,i);
            break;	  
         case FECHATIPO:
            pfi->f_campos[i].c_qc = -1;
            pfi->f_campos[i].c_long = 8;
            free(pfi->f_campos[i].c_usi);
            pfi->f_campos[i].c_usi = malloc(10);
            strcpy(pfi->f_campos[i].c_usi,"%-10.10s");
            break;
         default:
            sprintf(tmp,"%%-%d.%ds",qc,qc);
            pfi->f_campos[i].c_long = qc;
            pfi->f_campos[i].c_qc = qc;
            free(pfi->f_campos[i].c_usi);
            pfi->f_campos[i].c_usi = malloc(strlen(tmp)+1);
            strcpy(pfi->f_campos[i].c_usi,tmp);
         }
         pfi->f_lfich += pfi->f_campos[i].c_long;
         
         if (pfi->f_buf) free(pfi->f_buf);
         
         pfi->f_buf = malloc(pfi->f_lfich+1);
         
         return(i);
}


int carga_libreria_usuario(nom_lib)
char *nom_lib;
{
		int i;
        int modo = 0;
        HINSTANCE libreria = NULL;
		FARPROC aucall = NULL;
         
         /* ver si es dll o fichero */
         strminus(nom_lib);
         
         {
            char *ext;
            int pex;
            pex = strbusca(nom_lib,".");
            if (pex >= 0)
            {
               ext = nom_lib + pex;
               if (!strcmp(ext,".db3"))
               {
                  modo = 3;
               }
               else
                  if (!strcmp(ext,".dbp"))
                  {
                     modo = 4;
                  }
                  else
                     if (!strcmp(ext,".dbs"))
                     {
                        modo = 2;
                     }
                     else
                        if (!strcmp(ext,".dat"))
                        {
                           modo = 1;
                        }
                        else
                           if (!strcmp(ext,".odb") || !strcmp(ext,".odbc"))
                           {
                              *(ext+4) = 0;
                              modo = 5;		
                           }
            }
         }
         
         if (modo)
         {
            char *ext;
            int pi;
            /* es fichero !!! */
            if (busca_a_file(nom_lib) >= 0) return(0);
            for (i = 0;i < na_file;i++)
            {
               if (!a_file[i])
                  break;
            }
            if (i >= na_file)
            {
               if (i >= MAXULIBS) return(-1);
               i =  na_file;
               a_file[i] = NULL;
               na_file++;
            }
            a_file[i] = (FI_DEF *)malloc(sizeof(FI_DEF));
            memset(a_file[i],0,sizeof(FI_DEF));
            a_file[i]->f_titulo = NULL;
            a_file[i]->f_npanta = 0;
            a_file[i]->f_nrelaci = 0;
            a_file[i]->f_nclaves = 0;
            a_file[i]->f_ncampos = 0;
            a_file[i]->f_campos = NULL;        
            a_file[i]->f_clave = NULL;
            a_file[i]->f_relaci = NULL;
            a_file[i]->f_pantalla = NULL;
            a_file[i]->f_fd = -1;
            a_file[i]->f_lfich = 0;  
            a_file[i]->f_buf = NULL;
            
            suma_este_campo(a_file[i],"\rLinea",DOUBLETIPO,0,"0","999999999");
            
            a_file[i]->f_clave = (KEY_DEF *)malloc(1 * sizeof(KEY_DEF));  
            memset(a_file[i]->f_clave,0,1 * sizeof(KEY_DEF));
            a_file[i]->f_clave[0].k_orden = (short *)malloc(MAXCAMPOSPORCLAVE*sizeof(short));
            a_file[i]->f_clave[0].k_campo = (short *)malloc(MAXCAMPOSPORCLAVE*sizeof(short));         
            a_file[i]->f_clave[0].k_ncampos = 1;
            a_file[i]->f_clave[0].k_campo[0] = 1;
            a_file[i]->f_clave[0].k_orden[0] = 1;   
            
            a_file[i]->f_nclaves = 1;
            
            a_file[i]->f_campos[0].c_key = 1;
            
            strcpy(a_file[i]->f_fichero,nom_lib);
            ext = quita_path(a_file[i]->f_fichero);
            pi = strbusca(ext,".");
            if (pi >= 0)
            {	 
               char tmp[256];
               ext += pi;
               strcpy(tmp,ext);
               *ext = 0;
               ext++;
               *ext = 1;
               ext++;
               *ext = 2;
               ext++;
               *ext = 3;
               ext++;
               strcpy(ext,tmp);
            }
            a_file[i]->tipo_datos = modo - 1;
            stdbl((double)0.0,a_file[i]->f_buf);   
            return(i + MAXULIBS);
         }
         
         if (busca_libreria(nom_lib) >= 0) return(0);
         
         for (i = 0;i < nlibs;i++)
         {
            if (!nom_libs[i])
               break;
         }
         if (i >= nlibs)
         {
            if (i >= MAXULIBS) return(-1);
            i = nlibs;
            nom_libs[i] = NULL;
            nlibs++;
         }

		if (excel_alt) {
			 if (!strcmp(quita_path(nom_lib),"agixl97.dll"))
			 {
				 libreria = 1;
			 }
		}

		if (!libreria) {

         libreria = LoadLibrary(nom_lib);
         if (!libreria)
         {
            return(-1);
         }

		 if (!strcmp(quita_path(nom_lib),"agixl97.dll"))
		 {
			 aucall = GetProcAddress(libreria,"comprobacion12");
			 if (!aucall)
			 {
				 aviso(0,"Modulo excel por actualizar");
				 FreeLibrary(libreria);
				 return -1;
			 }
		 }
	    aucall = GetProcAddress(libreria,"dll_dll_x");
        if (aucall)
		{
			aucall((int)10,(int)12,(double)22.0);
		}

		aucall = GetProcAddress(libreria,"SolicitaFrame");
        if (aucall)
		{
			HWND hwnd = NULL;
			if (DespachadorExterno)
				(*DespachadorExterno)((int)DESPACHO_MULTIPLE,16,&hwnd);
			aucall(hwnd,(int)0);
		}

	}


         nom_libs[i] = malloc(strlen(nom_lib)+1);
         strcpy(nom_libs[i],nom_lib);
         librerias[i] = libreria;
         
         /*libreria_limpieza();*/
         
         return(i+1);
}

int descarga_libreria_usuario(nom_lib)
char *nom_lib;
{
   int i;
   /*FARPROC VaADescargar = NULL;
   void *punt = NULL;
   */
   i = busca_a_file(nom_lib);
   if (i >= 0)
   {
      if (a_file[i])
      {
         if (a_file[i]->f_fd >= 0)
         {
            cierra_ind(a_file[i]);
         }
         free_defpri(a_file[i]);     
         a_file[i] = NULL;
      }
      return(0);
   }
   
   i = busca_libreria(nom_lib);
   if (i < 0) return(-1);
   
   
   if (librerias[i])
   {
   /*
   VaADescargar = GetProcAddress(librerias[i],"VaADescargar");
   if (VaADescargar)
   {
   punt = (void *)VaADescargar();
   if (punt)
   {
   //pon_libreria_limpieza(punt,(LPCSTR) nom_libs[i]);
   }
   }
      */
   	if (!excel_alt || strcmp(quita_path(nom_lib),"agixl97.dll"))
      FreeLibrary(librerias[i]);
   }
   librerias[i] = NULL;
   if (nom_libs[i])
      free(nom_libs[i]);
   nom_libs[i] = NULL;
   if (i == (nlibs-1)) nlibs--;
   
   return(0);
}



static int saca_campos(FI_DEF *pfi,char *_par,int modo)
{
   int i;
   char buffer[5000],*par;
   char *campos[500];
   int nc = 0;
   strcpy(buffer,_par);
   par = buffer;
   
   for (i = 0;par[i];i++)
			{
      if (par[i] == 9)
      {
         par[i] = 0;	  
         i++;
         campos[nc] = par+i;
         nc++;	  		  
         while(par[i] && par[i] != 9) i++;
         if (!par[i]) break;
         par[i] = 0;      
      }
			}
   
   if (!modo)
			{
      char tmp[81];
      if (!nc) return(0);
      for (i = 0;i < nc;i++)
      {
         sprintf(tmp,"\rC%d",i+1);	  
         suma_este_campo(pfi,tmp,CHARTIPO,strlen(campos[i])+1,"","");
      }
      return(1);
			}
   
   for (i = 0;i < nc;i++)
			{
      if ((i-1) >= pfi->f_ncampos) break;
      st_campo(i+1,pfi,campos[i]);
			}
   
   return(i);
}

int alfacall_libreria_usuario(nom_lib,funcion,buffer)
char *nom_lib;
char *funcion;
char *buffer;
{
   int i;
   FARPROC AgiAlfaCall = NULL;

	 {
		 unsigned char *pp;			 
		 for (pp = (unsigned char *)buffer;*pp;pp++)
		 {
			 oem_a_ansi(pp);
		 }		 
	 }
   
   i = busca_a_file(nom_lib);
   
   if (i >= 0)
			{
      double contador;
      
      if (strcmp(funcion,"imprime")) return(0);
      
      if (a_file[i]->f_fd < 0)
      {      
         /* crear los campos */
         if (!saca_campos(a_file[i],buffer,0)) return(0);
         
         borra_ind(a_file[i]);      
         
         a_file[i]->f_fd = crea_ind(a_file[i]);
         if (a_file[i]->f_fd < 0)
         {
            /*aviso(0,"no puc!");*/
            return(-2);
         }
      }
      if (!saca_campos(a_file[i],buffer,1)) return(0);
      
      contador = lddbl(a_file[i]->f_buf);
      contador++;
      stdbl(contador,a_file[i]->f_buf);
      graba_nuevo(a_file[i]);
      return(0);
			}
   
   i = busca_libreria(nom_lib);
   
   if (i < 0 || !librerias[i]) return(-1);
   
   AgiAlfaCall = GetProcAddress(librerias[i],funcion);
   if (!AgiAlfaCall) return(-2);

   /*if (strcmp(buffer,"salva"))*/
		AgiAlfaCall((LPCSTR)buffer);
   
   return(0);
}

#endif

#endif


int a_cargadll(char *dll)
{
			char buffer[1024];
         static int fgv_activa_impredll_aviso = 0;
         
         if (!fgv_activa_impredll)
         {
            if (!fgv_activa_impredll_aviso)
            {
               fgv_activa_impredll_aviso = 1;
               aviso(0,__Texto[60]);
            }
            return(-1);
         }
		if (excel_alt) {
	     strminus(dll);
		 if (!strcmp(quita_path(dll),"agixl97.dll")) {
			 return 0;
		 }
		}
         
#ifdef RWSERVER
         
         
         if (en_conexion_externa)
         {
            strcpy(buffer,dll);
            funcion_windows_cliente(7,buffer,strlen(buffer)+1);
			moto_int(buffer);
            return(*((int *)buffer));
         }
         
#endif
         
#ifdef FGVDOS
         return(carga_libreria_usuario(dll));
#else
         return(-1);
#endif
}


int a_descargadll(char *dll)
{
			char buffer[1024];
         if (!fgv_activa_impredll)
            return(-1);

	if (excel_alt) {
	     strminus(dll);
		 if (!strcmp(quita_path(dll),"agixl97.dll")) {
			 return 0;
		 }
		}
         
#ifdef RWSERVER
         
         if (en_conexion_externa)
         {
            strcpy(buffer,dll);
            funcion_windows_cliente(8,buffer,strlen(buffer)+1);
			moto_int(buffer);
            return(*((int *)buffer));
         }
#endif
         
#ifdef FGVDOS
         return(descarga_libreria_usuario(dll));
#else
         return(-1);
#endif
}


int a_alfacalldll(char *dll,char *funcion,char *dato)
{
		 char buffer[10000];
         int i;
         
         if (!fgv_activa_impredll)
            return(-1);

	   if (excel_alt) {
			 strminus(dll);
			 if (!strcmp(quita_path(dll),"agixl97.dll")) {
				 strcpy(buffer,funcion);
				 strcpy(buffer+31,dato);
				 if (DespachadorExterno)
					(*DespachadorExterno)((int)DESPACHO_FUNCIONES_IMPRESION,998,(void *)buffer);
				 strcpy(dato,buffer+31);
				 return 0;
			 }		 
		}
         
#ifdef RWSERVER
         
         if (en_conexion_externa)
         {
            i = strlen(dll) + 1;
            strcpy(buffer,dll);
            
            strcpy(buffer+i,funcion);
            
            i += (strlen(funcion)+1);
            
            strcpy(buffer+i,dato);
            i += (strlen(dato)+1);
            
            funcion_windows_cliente(9,buffer,i);
            strcpy(dato,buffer+sizeof(int));	  
			moto_int(buffer);
            return(*((int *)buffer));
         }
#endif
         
#ifdef FGVDOS
         strcpy(buffer,dato);
         i = alfacall_libreria_usuario(dll,funcion,buffer);
         strcpy(dato,buffer);
#else
         i = -1;
#endif
         
         return(i);
}
