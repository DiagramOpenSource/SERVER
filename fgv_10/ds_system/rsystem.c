
#include <fgvsys.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <stdio.h>
#include <errno.h>
#ifdef ESDOS
#include <io.h>
#define __MSC
#include <dos.h>
#else
#ifdef ESXENIX
#include <sys/ndir.h>
#else
#include <dirent.h>
#endif
#endif

extern char *quita_path();

extern FILE *_imp;

#ifdef ESDOS
#include <process.h>
#endif


#ifdef RWXARXA
#ifdef RWSERVER
extern int en_conexion_externa;
#endif
extern char server_request[128];
#endif


int (*agi_especial_system_antes)(void) = NULL;
int (*agi_especial_system_despues)(int) = NULL;

#ifdef FGVWIN
int agi_system_wine(void *mandato)
{
   int i = -1,r,err,punto = 0;
   char *s = mandato;
   char tmm[10];
   int k;
   STARTUPINFO stinfo;
   PROCESS_INFORMATION proi;
   
   for (i = 0;s[i] && s[i] != ' ';i++)
      if (s[i] == '.') punto = i+1;
      
      if (!punto)
      {
         if (!agi_bats_procesor(s)) return(0);
      }
         k = 0; /* esto por el asunto guarro de usar mandatos de sistema directos!!! */
         if (strlen(mandato) > 4)
         {
            memcpy(tmm,mandato,5);
            tmm[5] = 0;
            strlwr(tmm);
            if (!memcmp(tmm,"copy ",5))
               k = 1;
         }
		 if (!k && punto)
		 {
			 if (!memcmp(s+punto-1,".bat",4) || !memcmp(s+punto-1,".BAT",4))
			 {
				 k = 1;
			 }
		 }
         if (k)
         {
            //r = system(mandato);  
			 strcatd(mandato,"wcmd /c ");
         }
		 else
		 {
			 if (punto && !memcmp(s+punto-1,".sh",3))
			 {				 
				 strcatd(mandato,"/bin/sh ");
				 cambia_barra(mandato);
			 }			 
		 }
         //else
        {
			memset((void *)&proi,0,sizeof(proi));			

            memset(&stinfo,0,sizeof(stinfo));
            stinfo.wShowWindow = SW_SHOWMINIMIZED;
            stinfo.cb = sizeof(stinfo);
            if (CreateProcess(NULL,mandato,NULL,NULL,FALSE,NORMAL_PRIORITY_CLASS /*| DETACHED_PROCESS*/,NULL,NULL,&stinfo,&proi) == FALSE)
            {
               r = -1;
            }
            else
            {
				/*
               _cwait(NULL, proi.hProcess,NULL);
			   */
			   WaitForSingleObject(proi.hProcess,INFINITE);
			   CloseHandle(proi.hProcess); 
			   CloseHandle(proi.hThread);
               r = 0;
            }	  
         }
      return(r);
}
#endif

int agi_system(void *mandato)
{
   int i = -1,r,err,punto = 0;
   char *s = mandato;
   char tmm[10];
   int k;
   int oculto = 1;
#ifdef ESDOS
/*#ifdef RWSERVER*/
   STARTUPINFO stinfo;
   PROCESS_INFORMATION proi;
/*#endif*/
#endif
#ifdef FGVWIN
   if (getenv("DSWINE"))
   {
	   return agi_system_wine(s);
   }
#endif

   for (i = 0;s[i] && s[i] != ' ';i++)
      if (s[i] == '.') punto = i+1;
      
      if (!punto)
      {
         if (!agi_bats_procesor(s)) return(0);
      }
      i = -1;
      if (agi_especial_system_antes)
      {
         i = (*agi_especial_system_antes)();
      }
      if (_imp && (unsigned long)_imp > 4)
         fflush(_imp);
#ifdef ESDOS
/*#ifdef RWSERVER*/
	  {
		  char *p1,*p2;
		  p1 = strchr(mandato,' ');
		  if (p1)
		  {
			  p2 = strstr(mandato,"agilis");
			  if (p2 && p2 < p1)
				  oculto = 0;
			  p2 = strstr(mandato,"agigen");
			  if (p2 && p2 < p1)
				  oculto = 0;
			  p2 = strstr(mandato,"agisys");
			  if (p2 && p2 < p1)
				  oculto = 0;
		  }
	  }

      if ( oculto )	  
      {
         k = 0; /* esto por el asunto guarro de usar mandatos de sistema directos!!! */
         if (strlen(mandato) > 4)
         {
            memcpy(tmm,mandato,5);
            tmm[5] = 0;
            strlwr(tmm);
            if (!memcmp(tmm,"copy ",5))
               k = 1;
         }
		 if (!k && punto)
		 {
			 if (!memcmp(s+punto-1,".bat",4) || !memcmp(s+punto-1,".BAT",4))
			 {
				 k = 1;
			 }
		 }
         if (k)
         {
            r = system(mandato);  
         }
         else
         {
            memset(&stinfo,0,sizeof(stinfo));
            stinfo.wShowWindow = SW_SHOWMINIMIZED;
            stinfo.cb = sizeof(stinfo);
            if (CreateProcess(NULL,mandato,NULL,NULL,FALSE,NORMAL_PRIORITY_CLASS | DETACHED_PROCESS,NULL,NULL,&stinfo,&proi) == FALSE)
            {
			   r = system(mandato);                 
            }
            else
            {
			   DWORD hinfo;
			   _cwait(NULL, proi.hProcess,NULL);

			   if (GetHandleInformation(proi.hProcess,&hinfo))
					CloseHandle(proi.hProcess); 
			   if (GetHandleInformation(proi.hThread,&hinfo))
					CloseHandle(proi.hThread);
               r = 0;
            }	  
         }
      }
      else
         r = system(mandato);
#else
      r = system(mandato);
#endif
      err = errno;
      if (agi_especial_system_despues)
      {
         (*agi_especial_system_despues)(i);
      }
      errno = err;
      return(r);
}


int fgv_system_oculto(void *mandato)
{
#ifdef FGVDOS
   STARTUPINFO stinfo;
   PROCESS_INFORMATION proi;
   int r;
   
   memset(&stinfo,0,sizeof(stinfo));
   stinfo.wShowWindow = SW_SHOWMINIMIZED;
   stinfo.cb = sizeof(stinfo);
   if (CreateProcess(NULL,mandato,NULL,NULL,FALSE,HIGH_PRIORITY_CLASS | DETACHED_PROCESS,NULL,NULL,&stinfo,&proi) == FALSE)
   {
      r = -1;
   }
   else
   {
      _cwait(NULL, proi.hProcess,NULL);
      CloseHandle(proi.hProcess); 
	  CloseHandle(proi.hThread);
      r = 0;
   }
   return(r);
#else
   return(system(mandato));
#endif
}


int fgv_arranca(char *mandato_)
{

#ifdef RWSERVER
   if (en_conexion_externa)
   {
       char tmp[2048];
       strcpy(tmp,mandato_);
       funcion_windows_cliente(35,tmp,strlen(tmp)+1);
       return (*((int *)tmp));
   }
#endif

   {
#ifdef FGVDOS
   char mandato[1024];
   STARTUPINFO stinfo;
   PROCESS_INFORMATION proi;
   int r,n,i;
   int j = strlen(mandato_);   
   int pp = 0;

   quitab(mandato_);
   mandato[0] = 0;
   n = 0;
   for (i = 0;i < j;i++)
   {
      if ((!i || mandato_[i-1] == ' ') && mandato_[i] == ':' && mandato_[i+1] == '/')
      {
         strcpy(mandato+n,rw_directorio(""));
         i++;
         n = strlen(mandato);
         continue;
      }
      if (!pp && mandato_[i] == ' ')
      {
          pp = 1;
          strcpy(mandato+n," /DS:\"");
          n = strlen(mandato);
          if (server_request[0])
          {
             strcpy(mandato+n,server_request);              
             n = strlen(mandato);
             strcpy(mandato+n," ");
             n++;
          }
          while(mandato_[i] && mandato_[i] == ' ') i++;
      }
      mandato[n] = mandato_[i];
      n++;
   }
   if (pp)
   {
       mandato[n] = '"';
       n++;
   }
   mandato[n] = 0;

   
 
   
   memset(&stinfo,0,sizeof(stinfo));
   stinfo.wShowWindow = SW_SHOW;
   stinfo.cb = sizeof(stinfo);
   if (CreateProcess(NULL,mandato,NULL,NULL,FALSE,NORMAL_PRIORITY_CLASS | DETACHED_PROCESS,NULL,NULL,&stinfo,&proi) == FALSE)
   {
      r = -1;
   }
   else
   {   
      r = 0;
   }
   return(r);
#else
   return(-1);
#endif
   }
}


static char *agibats[] = {
   "agirm ",
      "agiren ",
      "agimkdir ",
      "agicp ",
      "agichmod ",
      "agcopiaa ",
      "agicopy ",
      NULL,
};



int agi_bats_procesor(d)
char *d;
{
   int o,i,mientras,j;
   char nom[128];
   char dir[128];
   char _s[128];
   char *s = _s;
   
   quitabp(d);
   for (i=0;d[i] && d[i] != ' ';i++);
   
   for (;i > 0 && d[i] != '\\' && d[i] != '/';i--);
   
   if (d[i] == '\\' || d[i] == '/')
      i++;
   else
      i = 0;
   strcpy(s,d+i);
   quitabp(s);
   for (o = 0;agibats[o];o++)
      if (!memcmp(agibats[o],s,strlen(agibats[o])))
         break;
      
      if (agibats[o])
      {
         /* quitar > */
         for (i = 0;s[i];i++)
            if (s[i] == '>')
            {
               s[i] = 0;
               quitab(s);
               break;
            }
            s += strlen(agibats[o]);
            quitabp(s);
            quitab(s);
      }
      else
         return(-1);
      
      switch(o)
      {
      case 0:
         if (!memcmp(s,"-r",2))
         {
            s+=2;
            quitabp(s);
            strcpy(dir,s);
            lee_y_borra(dir);
            rmdir(s);
         }
         else
            unlink(s);
         return(0);
      case 1:
         for (i = 0;s[i] && s[i] != ' ';i++);
         if (s[i] == ' ')
         {
            s[i] = 0;
            i++;
            quitabp(s+i);
         }
         rename(s,s+i);
         return(0);
      case 2:
#ifdef FGVUNIX
         mkdir(s,255);
#else
         mkdir(s);
#endif
         return(0);
      case 3:
         /* agicp(); */
         for (i = 0;s[i] && s[i] != ' ';i++);
         if (s[i] == ' ')
         {
            s[i] = 0;
            i++;
            quitabp(s+i);
         }
         copiarfich(s,s+i,0);
         return(0);
      case 4:
         /* agichmod(); */
#ifdef FGVUNIX
         return(-1);
#else
         return(0);
#endif
      case 5:
         /* agcopiaa(); */
#ifdef FGVUNIX
         return(-1);
#else
         for (i = 0;s[i] && s[i] != ' ';i++);
         if (s[i] == ' ')
         {
            s[i] = 0;
            i++;
            quitabp(s+i);
         }
         for (j = i;s[j] && s[j] != ' ';j++);
         if (s[j] == ' ')
         {
            s[j] = 0;
            j++;
            quitabp(s+j);
         }
         strcpy(nom,s+j);
         if (!strlen(nom))
         {
            strcpy(nom,"a:");
         }
         strcat(nom,s);
         copiarfich(nom,s+i,0);
         return(0);
#endif
      case 6:
         /* agicopy(); */
         for (i = 0;s[i] && s[i] != ' ';i++);
         if (s[i] == ' ')
         {
            s[i] = 0;
            i++;
            quitabp(s+i);
         }
         strcpy(nom,s);
         strcat(nom,".ixx");
         strcpy(dir,s+i);
         strcat(dir,".ixx");
         copiarfich(nom,dir,0);
         memcpy(dir+strlen(dir)-3,"dat",3);
         memcpy(nom+strlen(nom)-3,"dat",3);
         copiarfich(nom,dir,0);
         memcpy(dir+strlen(dir)-3,"ddx",3);
         memcpy(nom+strlen(nom)-3,"ddx",3);
         copiarfich(nom,dir,0);
         memcpy(dir+strlen(dir)-3,"dbf",3);
         memcpy(nom+strlen(nom)-3,"dbf",3);
         copiarfich(nom,dir,0);
         return(0);
      default:
         return(-1);
  }
  return(-1);
}
