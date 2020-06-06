
/****************************************************************************/
/*                   RUTINA DE HELPS                                        */
/****************************************************************************/

/* Autor: Francisco Gonzalez Valiente */

#include <fgvsys.h>

#define LASTBYTE 0X7FFFFFFAL            /* the max signed long number  -3 */

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef O_BINARY

#include <io.h>

#else

#define O_BINARY 0
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#ifndef LARGESEEK
extern long rw_lseek_();
#endif
extern unsigned int rw_write_();

#endif

#ifdef FGVDOS
#include <shellapi.h>
#ifndef FGVWIN
extern HWND GetConsoleHwnd();
#endif
#endif

#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <malloc.h>

#include <rglobal.h>
#include <rwmalloc.h>
#include <rtermin.h>
#include <teclas.h>

#ifdef FGVWIN
#include <fgvwin.h>
#ifndef RWSERVER
#include "D:\\Archivos de programa\\HTML Help Workshop\\include\\htmlhelp.h"
#endif
#endif

extern char *quita_path();
extern char *lee_linea();
char *dirbase_ayuda = NULL;
char *nomapli_ayuda = "diagram9";
extern char referencia_help[128];

#ifdef RWSERVER
extern int en_conexion_externa;
#endif


extern char *help_externo;
extern char *url_ayuda_f1;

int agiayuda(clave)
char *clave;
{
   /* SHELLEXECUTEINFO ee; */
   int linea = clave[31];
   int /* c_v = 2*/ c_v = 1,j,i;
   int ayuda_de_basico = 0;
   int posi,posf;
   char bufpagina[300];
   char nombre[256];
   char nombre2[256];
   char nombre3[512];
   int jj,kk,rr,nn,err;
   char envio[2048];
   VENT *w;
   FILE *f,*f2;
   char **tx;
#ifdef FGVDOS;
   HWND miw;
#endif
   static long ulpos = 0L;
   static int  ucpos = 0;
   static char unombre[51] = {0,};
   static int activa = 0;
   if (activa) return(0);
   
   if (!strcmp(nomapli_ayuda,"diagram9"))
   {
      ayuda_de_basico = 1;
   }
   
   if (url_ayuda_f1)
   {
	   strcpy(envio,url_ayuda_f1);
	   if (DespachadorExterno)
			(*DespachadorExterno)((int)DESPACHO_MULTIPLE,(int)17,(void *)envio);	   
	   return 0;
   }

#ifdef RWSERVER
   if (en_conexion_externa)
   {      
      /* en caso de servidor la ayuda se pasa al cliente */
      /* la ayuda html en caso de servidor ya no pasa por aqui (f_ayuda = ayuda_remota) */
      if (!ayuda_de_basico)
      {
         long registro = time(NULL);
         long remoto_registro = 0L;
         struct stat buf_stat1,buf_stat2;
         FILE *ff;
         char tmpx[300];
         char tmpy[300];	  		 
         int hacer = 0;            
         strcpy(tmpx,(!dirbase_ayuda) ? rw_directorio("") : dirbase_ayuda);
         strcat(tmpx,"ayuda/");
         strcpy(tmpy,tmpx); 
         strcat(tmpy,"flag.upd");
         if (stat( tmpy , &buf_stat1 ) >= 0)
         {
            int i2;
            /* incrementar registro de ayudas */
            hacer = 1;
            unlink(tmpy);
            strcpy(tmpy,tmpx);
            strcat(tmpy,"ayuda.srv");         
            f = fopen(tmpy,"w");
            if (f)
            {			
               fprintf(f,"%ld\n",registro);
               fclose(f);
            }
         }
         /*comprobar registro de ayudas en el cliente  */
         strcpy(tmpy,tmpx);
         strcat(tmpy,"ayuda.srv");
         f = fopen(tmpy,"r");
         if (f)
         {
            if (lee_linea(tmpy,50,f))
            {
               registro = atol(tmpy);
            }
            fclose(f);
         }
         else
         {
            f = fopen(tmpy,"w");
            if (f)
            {			
               fprintf(f,"%ld\n",registro);
               fclose(f);
            }
         }
         strcpy(tmpy,nomapli_ayuda);
         funcion_windows_cliente(29,tmpy,strlen(tmpy)+1);
         memcpy_long(&remoto_registro,tmpy,sizeof(long));
         if (!remoto_registro || remoto_registro < registro)
            hacer = 1;
         if (hacer)
         {
            char tmpz[300];
            /* mensaje de espera windows !!! */				
            strcpy(tmpy,nomapli_ayuda);
            funcion_windows_cliente(32,tmpy,strlen(tmpy)+1);
            
            sprintf(tmpy,"%s%s/ayuda/%s.tar",rw_directorio("remoto/"),nomapli_ayuda,nomapli_ayuda);
            crea_dir(tmpy);
            sprintf(envio,"%s *.txt",tmpy,tmpx);
            
            agi_atar(envio,tmpx);		
            
            agi_compress(tmpy,0);
            unlink(tmpy);
            
            
            sprintf(tmpy,"remoto/%s/ayuda/%s.tgz",nomapli_ayuda,nomapli_ayuda);
            rw_envia_fichero(tmpy,666); /* el envio ya borra el fichero !!! */
            
            strcpy(tmpx,(!dirbase_ayuda) ? rw_directorio("") : dirbase_ayuda);
            strcat(tmpx,nomapli_ayuda);
            strcat(tmpx,".men");
            sprintf(tmpz,"%s%s/%s.men",rw_directorio("remoto/"),nomapli_ayuda,nomapli_ayuda);
            copiarfich(tmpx,tmpz,0);
            sprintf(tmpz,"remoto/%s/%s.men",nomapli_ayuda,nomapli_ayuda);
            rw_envia_fichero(tmpz,666); /* el envio ya borra el fichero !!! */
            
            memcpy(envio,&registro,sizeof(long));
            strcpy(envio+sizeof(long),nomapli_ayuda);
            strcpy(envio+100+sizeof(long),tmpy);
            funcion_windows_cliente(30,envio,strlen(tmpy)+101+sizeof(long));
         }
      }   
      memset(envio,0,sizeof(envio));
      strcpy(envio,nomapli_ayuda);
      strcpy(envio+100,clave);
      strcpy(envio+200,referencia_help);      
      funcion_windows_cliente(31,envio,300);
      return(0);
   }
#endif
   
#ifdef FGVDOS 

   /*****/
   
   if (help_externo)
			{
      /* la ayuda se llama por html externamente */
      if (!strcmp(help_externo,"default"))
      {
         sprintf(nombre2,"%s/ayuda/html/",rw_directorio(nomapli_ayuda));
         cambia_barra(nombre2);
         if (nombre2[0] == '/')
         {          
            sprintf(nombre,"file:/%s",nombre2);
         }
         else
         {
            sprintf(nombre,"file://%s",nombre2);
         }	  
      }
      else
      {
         strcpy(nombre,help_externo);
         if (nombre[0] && nombre[strlen(nombre)-1] != '/')
            strcat(nombre,"/");
      }
      
      strcpy(nombre2,clave);
      quita_blancos(nombre2);
      i = strlen(nombre2);
      while (i) 
      {
         i--;
         if (nombre2[i] == '.') 
         {
            nombre2[i] = 0;
            break;
         }
      }
      strcpy(bufpagina,rw_directorio("defhelp.agh"));
      f = fopen(bufpagina,"r");
      if (f)
      {
         sprintf(bufpagina,"%s/ayuda/html/~%d~%d.html",rw_directorio(nomapli_ayuda),user_who,subnumero_de_usuario);
         f2 = fopen(bufpagina,"w");
         if (f2)
         {
            while(lee_linea(bufpagina,256,f))
            {
               /*  */
               j = 0;
               for (i = 0;bufpagina[i];i++)
               {
                  if (bufpagina[i] == '[' && bufpagina[i+1] == '%' && bufpagina[i+3] == ']')
                  {
                     switch(bufpagina[i+2])
                     {
                     case '1':
                        strcpy(nombre3+j,nombre);
                        j += strlen(nombre);
                        break;				
                     case '2':    
                        strcpy(nombre3+j,nomapli_ayuda);
                        j += strlen(nomapli_ayuda);
                        break;		
                     case '3':    
                        strcpy(nombre3+j,nombre2);
                        j += strlen(nombre2);
                        break;		
                     case '4':
                        strcpy(nombre3+j,referencia_help);
                        j += strlen(referencia_help);
                        break;					
                     default:
                        break;
                     }
                     i += 3;
                  }
                  else
                  {
                     nombre3[j] = bufpagina[i];
                     j++;
                  }
               }  
               nombre3[j] = 0; 
               fprintf(f2,"%s\n",nombre3);
               /*  */
            }
            fclose(f2);
         }
         fclose(f);
         sprintf(bufpagina,"%s~%d~%d.html",nombre,user_who,subnumero_de_usuario);
      }
      else
      {
         sprintf(bufpagina,"%s/ayuda/html/~%d~%d.html",rw_directorio(nomapli_ayuda),user_who,subnumero_de_usuario);
         f = fopen(bufpagina,"w");
         if (f)
         {
            fprintf(f,"<HTML>\n");
            fprintf(f,"<HEAD>\n");
            fprintf(f,"<TITLE>DIAGRAM Ayuda</TITLE>\n");
            fprintf(f,"<NOFRAMES>\n");
            fprintf(f,"<META HTTP-EQUIV=\"REFRESH\" CONTENT=\"0; URL=%s.html#%s\">\n",nombre2,referencia_help);
            fprintf(f,"<BGCOLOR=\"#FFFFFF\">\n");
            fprintf(f,"<FONT COLOR=\"#FFFFFF\">\n");
            fprintf(f,"</NOFRAMES>\n");
            fprintf(f,"</HEAD>\n");
            fprintf(f,"<FRAMESET COLS='206,*'  ROWS='40,*'  FRAMEBORDER=0>\n");
            fprintf(f,"<FRAME SRC=~home.html     NAME=Frame0 SCROLLING=NO     MARGINWIDTH=0 MARGINHEIGHT=0 NORESIZE>\n");
            fprintf(f,"<FRAME SRC=~search.html   NAME=Frame1 SCROLLING=AUTO MARGINWIDTH=0 MARGINHEIGHT=3>\n");
            fprintf(f,"<FRAME SRC=~menu.html     NAME=Frame2 SCROLLING=NO     MARGINWIDTH=0 MARGINHEIGHT=0>\n");
            fprintf(f,"<FRAME SRC=%s.html#%s     NAME=Frame3 SCROLLING=AUTO >\n",nombre2,referencia_help);
            fprintf(f,"</FRAMESET>\n");
            fprintf(f,"</HTML>\n");
            fclose(f);		
         }	  
         sprintf(bufpagina,"%s~%d~%d.html",nombre,user_who,subnumero_de_usuario);
      }   
      cambia_barra(bufpagina);
      ShellExecute(NULL,"open",bufpagina,"","",SW_SHOW);
      return(0);
   }
   
#ifdef FGVWIN
   miw = principal_w;
#else
   miw = GetConsoleHwnd();
#endif
   
#ifdef FGVWIN
#ifndef RWSERVER
   /*****/
   {
         char tmpx[300];                  
		 char tmpy[300];                  
		 FILE *f;
         strcpy(tmpx,(!dirbase_ayuda) ? rw_directorio("") : dirbase_ayuda);   
         strcat(tmpx,"ayuda/ds.chm");
		 f = fopen(tmpx,"r");
		 if (f)
		 {
			 fclose(f);			 
			 if (!ayuda_de_basico)
			 {
				 strcpy(tmpy,nomapli_ayuda);
				 strcat(tmpy,"_");
				 strcat(tmpy,clave);			 
				 quita_blancos(tmpy);
				 i = strlen(tmpy);
				 while (i) 
				 {
					 i--;
					 if (tmpy[i] == '.') 
					 {
						tmpy[i] = 0;
						break;
					 }
				 }
				 HtmlHelp(miw, tmpx, HH_DISPLAY_TOPIC, (DWORD)tmpy);
			 }
			 else
				 HtmlHelp(miw, tmpx, HH_DISPLAY_TOPIC, (DWORD)NULL);
			 /*HWND HtmlHelp(HWND hwndCaller, LPCSTR pszFile, UINT uCommand, DWORD dwData); */
			 
			 return 0;
		 }
   }
#endif
#endif
   
   /* buscar .hlp */
   
   strcpy(nombre,quita_path(clave));
   quita_blancos(nombre);
   i = strlen(nombre);
   while (i) {
      i--;
      if (nombre[i] == '.') {
         nombre[i] = 0;
         break;
      }
   }
   
   
   if (!ayuda_de_basico) 
   {
      int crearla = 0;
      struct stat buf_stat1;
      strcpy(bufpagina,(!dirbase_ayuda) ? rw_directorio("") : dirbase_ayuda); 
      strcat(bufpagina,"ayuda");
      mkdir(bufpagina);
      strcat(bufpagina,"/");
      strcat(bufpagina,nombre);
      strcat(bufpagina,".txt");
      buf_stat1.st_size = 0L;
      _stat( bufpagina , &buf_stat1 );
      if (!buf_stat1.st_size)
      {
         if (!confirmar(2400,__Texto[436],__Texto[448]))
         {
            aviso(0,__Texto[514]);
            crearla = 1;
         }
      }
      
      if (crearla)
      {
         f = fopen(bufpagina,"w");
         if (f) fclose(f);
         ShellExecute(NULL,"open",bufpagina,"","",SW_SHOW);
         return(0);
      } 
   }
   
   /* la ayuda actualizable por paquete se comprueba */
   if (!ayuda_de_basico) 
   {
      strcpy(bufpagina,(!dirbase_ayuda) ? rw_directorio("") : dirbase_ayuda);
      strcpy(nombre2,bufpagina);
      strcat(bufpagina,"ayuda");
      mkdir(bufpagina);
      strcat(bufpagina,"/");
      strcat(bufpagina,nomapli_ayuda);
      strcat(bufpagina,".hlp");
      f = fopen(bufpagina,"r");
      
      if (!f)
      {
         ds_rtfhlp(nombre2,__Texto[481]);
         f = fopen(bufpagina,"r");
      }
      
      if (f)
      {   
         struct stat buf_stat1,buf_stat2;
         FILE *ff;
         char tmpx[300];
         char tmpy[300];
         int hacer = 0;
         fclose(f);
         cambia_barra_dos(bufpagina);
         strcpy(tmpx,(!dirbase_ayuda) ? rw_directorio("") : dirbase_ayuda);   
         strcat(tmpx,"ayuda/");
         strcpy(tmpy,tmpx); 
         strcat(tmpy,"flag.upd");
         if (stat( tmpy , &buf_stat1 ) >= 0)
         {
            hacer = 1;
            unlink(tmpy);
         }
         strcat(tmpx,nombre);      
         strcpy(tmpy,tmpx); 
         strcat(tmpx,".top");
         strcat(tmpy,".txt");
         
         if (!hacer && _stat( tmpx , &buf_stat1 ) >= 0)
         {
            if (_stat( tmpy , &buf_stat2 ) >= 0)
            {
               if (buf_stat1.st_mtime < buf_stat2.st_mtime)
               {
                  hacer = 1;
               }
            }
         }
         else
         {
            if (!hacer && _stat( tmpy , &buf_stat2 ) >= 0)
               hacer = 1;
         }
         
         if (hacer) ds_rtfhlp(nombre2,__Texto[482]);
         
         
         //   nombre2[0] = 'Z';
         //   nombre2[1] = 'Z';
         //   nombre2[2] = '_';
         //   strcpy(nombre2+3,nombre);
         //   strcat(nombre2,"_ref");
         strcpy(nombre2,nombre);
         ff = fopen(tmpx,"r");
         if (ff)
         {	  
            lee_linea(nombre2,256,ff);
            fclose(ff);
         }	        
         WinHelp(miw,bufpagina,HELP_PARTIALKEY,nombre2);
         return(0);
      }
   }
   

   /* ver si en todo caso hay ayuda maestra fija desde basico */
   sprintf(bufpagina,"%s%s.hlp",rw_directorio("ayuda/"),nomapli_ayuda);
   f = fopen(bufpagina,"r");   
   if (f)
   {
      FILE *ff;
      char tmpx[300];
      fclose(f);   
      cambia_barra_dos(bufpagina);
      sprintf(tmpx,"%s%s.top",rw_directorio("ayuda/"),nombre);
      //   nombre2[0] = 'Z';
      //   nombre2[1] = 'Z';
      //   nombre2[2] = '_';
      //   strcpy(nombre2+3,nombre);
      //   strcat(nombre2,"_ref");
      strcpy(nombre2,nombre);
      ff = fopen(tmpx,"r");
      if (ff)
      {	  
         lee_linea(nombre2,256,ff);
         fclose(ff);
      }	        
      WinHelp(miw,bufpagina,HELP_PARTIALKEY,nombre2);
      return(0);
   } 
   
   /* como ultimo recurso intentar presentar la ayuda 'basica' */ 
   sprintf(bufpagina,"%sdiagram9.hlp",rw_directorio("ayuda/"));
   f = fopen(bufpagina,"r");
   if (f)
   {
      FILE *ff;
      char tmpx[300];
      fclose(f);   
      cambia_barra_dos(bufpagina);
      sprintf(tmpx,"%s%s.top",rw_directorio("ayuda/"),nombre);
      //   nombre2[0] = 'Z';
      //   nombre2[1] = 'Z';
      //   nombre2[2] = '_';
      //   strcpy(nombre2+3,nombre);
      //   strcat(nombre2,"_ref");
      strcpy(nombre2,nombre);
      ff = fopen(tmpx,"r");
      if (ff)
      {	  
         lee_linea(nombre2,256,ff);
         fclose(ff);
      }	        
      WinHelp(miw,bufpagina,HELP_PARTIALKEY,nombre2);
      return(0);
   } 
   
   aviso(0,__Texto[83]);
   return(0);
#else
   
   
   
   strcpy(nombre,quita_path(clave));
   quita_blancos(nombre);
   i = strlen(nombre);
   while (i) {
      i--;
      if (nombre[i] == '.') {
         nombre[i] = 0;
         break;
      }
   }
   
   strcat(nombre,".txt");
   strcpy(bufpagina,(!dirbase_ayuda) ? rw_directorio("") : dirbase_ayuda);
   /* strcpy(bufpagina,(!dirbase_ayuda) ? dirbase : dirbase_ayuda);*/
   strcat(bufpagina,"ayuda/");
   strcat(bufpagina,nombre);
   if (strcmp(nombre,unombre))
   {
      ulpos = 0L;
      ucpos = 0;
   }
   
   i = 0;
   f = fopen(bufpagina,"r");
   /*
   if (!f)
   {   
   strcpy(bufpagina,rw_directorio(""));   
   strcat(bufpagina,"ayuda/");
   strcat(bufpagina,"diagram9.txt");
   f = fopen(bufpagina,"r");
   }
   */
   
   if (f) {
      j = 0;
      while(lee_linea(bufpagina,256,f)) j++;
      rewind(f);
      tx = (char **)malloc(sizeof(char *) * (j + 1));
      while(i < j && lee_linea(nombre2,256,f)) {
         jj = 0;
         for( kk = 0;nombre2[kk];kk++)
         {
            if ((nombre2[kk] == '(' || nombre2[kk] == '{' || nombre2[kk] == '[') && nombre2[kk] == nombre2[kk+1])
            {
               for (kk += 2;nombre2[kk] && nombre2[kk] != ')' && nombre2[kk] != '}' && nombre2[kk] != ']';kk++)
               {
                  bufpagina[jj] = nombre2[kk];
                  jj++;
               }
               if (nombre2[kk])
               {
                  kk += 2;
                  /* busca segundos corchetes */
                  rr = kk;
                  while (nombre2[rr] && nombre2[rr] != '(' && nombre2[rr] != '{' && nombre2[rr] != '[' && nombre2[rr] == nombre2[rr+1]) rr++;
                  if (nombre2[rr])
                  {
                     nn = rr + 2;
                     while (nombre2[rr] && nombre2[rr] != ')' && nombre2[rr] != '}' && nombre2[rr] != ']') rr++;
                     if (nombre2[rr] == ']')
                     {
                        nombre2[rr] = 0;
                        quita_blancos(nombre2+nn);
                        strminus(nombre2+nn);
                        if (!strcmp(nombre2+nn,referencia_help))
                        {
                           ulpos = i + 1;
                        }
                        nombre2[rr] = ']';
                     }
                     if (nombre2[rr]) rr++;
                     kk = rr;
                  }
               }
               if (!nombre2[kk]) kk--;
            }
            else
            {
               if (nombre2[kk] == 9)
               {
                  strcpy(bufpagina+jj,"         ");
                  jj += 7;
               }
               else
               {
                  bufpagina[jj] = nombre2[kk];
                  jj++;
               }
            }
         }
         bufpagina[jj] = 0;
         tx[i] = malloc(strlen(bufpagina)+1);
         strcpy(tx[i],bufpagina);
         i++;
      };
      fclose(f);
   }
   else
   {
      tx = (char **)malloc(sizeof(char *));
   }
   tx[i] = NULL;
   
   
   activa = 1;
   
   posi = 602;posf = 2279;
   w = w_ini(401,2380);
   w_salva(w);
   zonablan(401,2380);
   bi_cuadro(501,2380,1);
   
   
   strcpy(bufpagina,__Texto[480]);
   strcpy(bufpagina+6,nombre);
   
   
   /* no esta puesta la posicion */
   if (!edita_texto(&tx,posi,posf,8000,bufpagina,&c_v,256,&ulpos,&ucpos) && !c_v) {
      strcpy(bufpagina,(!dirbase_ayuda) ? rw_directorio("") : dirbase_ayuda);
      /*strcpy(bufpagina,(!dirbase_ayuda) ? dirbase: dirbase_ayuda);*/
      strcat(bufpagina,"ayuda/");
      strcat(bufpagina,nombre);
      strcpy(unombre,nombre);
      if ( (f = fopen(bufpagina,"w")) != NULL) {
         i = 0;
         while(tx[i]) {
            graba_linea(tx[i],f);
            i++;
         }
         fclose(f);
      }
   }
   else 
   {
      ulpos = 0L;
      ucpos = 0;
   }
   
   
   libera_texto(tx);
   
   w_pinta(w);
   w_fin(w);
   activa = 0;
   
#endif
   return(0);
}



