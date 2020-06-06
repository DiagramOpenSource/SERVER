

/* Autor: Francisco Gonzalez Valiente */
#include <fgvsys.h>

#include <stdio.h>

char *salvapar;
extern int LEERTERMINAL;

extern char original_rw_dirbase[51];
extern char other_server[128];
extern char server_request[128];
extern char segundo_dir[128];
int seted_dir = 0;
extern int server_id;

extern char no_instala[30];

menusep(argc,argv)
int  *argc;
char *argv[];
{
   int i,j;
   char *salvador,*pp;
   
   
   server_request[0] = 0;
   no_instala[0] = 0;
   
   salvapar = (char *)0;
   for (i=0;i<*argc;i++) {
      if (*argv[i]=='$' && *(argv[i]+1)=='$') {
         salvapar = argv[i];
         for (j=i;j<((*argc)-1);j++)
            argv[i] = argv[i+1];
         argv[j]=salvapar;
         (*argc)--;
         break;
      }
   }
   
   for (i=0;i<*argc;i++) {
      if (*argv[i]=='~') {
         LEERTERMINAL = 1;
         salvador = argv[i];
         for (j=i;j<((*argc)-1);j++)
            argv[j] = argv[j+1];
         argv[j]=salvador;
         (*argc)--;
         break;
      }
   }
   
   for (i=0;i<*argc;i++) 
   {
      if (*argv[i]=='@') 
      { 
         salvador = argv[i];
         pp = argv[i];
         pp++;
         if (*pp == '$' && *(pp+1) == '$')
         {
         /*
         user_who = atoi(pp+2);
         strcpy(rw_user,pp+6);
         quitab(rw_user);
            */
         }
         else
            if (*pp == '!')
            {
               strcpy(segundo_dir,salvador);
            }
            else
               if (*pp == '#')
               {
                  server_id = atoi(pp+1);
               }
               else
                  if (*pp == '%')
                  {
                     strcpy(no_instala,salvador);
                  }
                  else
                     if (*pp == ':')
                     {
                        strcpy(other_server,salvador);		
                     }
                     else
                        if (*pp == '(' || *pp == '-') /* may be client-server request */
                        {
                           strcpy(server_request,salvador);
                           quita_blancos(server_request);
                           cambia_s_req(server_request,1);
                        }
                        else
                        {
                           rw_setdirectorio(pp); 
						   if (!seted_dir)
						   {
							   strcpy(original_rw_dirbase,rw_directorio(""));
						   }
                           seted_dir = 1;
                        }      
                        for (j=i;j<((*argc)-1);j++)
                           argv[j] = argv[j+1];
                        argv[j]=salvador;
                        (*argc)--;
                        i--;
      }
   }
   
   return(0);
}

