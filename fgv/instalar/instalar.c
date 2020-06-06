
#include <windows.h>

int PASCAL WinMain(HANDLE hInstance,HANDLE hPrevInstance,
						 LPSTR lpCmdline,int nCmdShow)
{
  char mandato[1024];
  char *args[100];
  int argc = 0,i;
   
  strcpy(mandato,GetCommandLine());

  args[argc] = mandato;
  argc++;
  for (i = 0;mandato[i];i++)
     {
	 if (mandato[i] == ' ')
	    {
        mandato[i] = 0;
		i++;
		while(mandato[i] == ' ') i++;
		if (mandato[i])
		   {
		   args[argc] = mandato + i;
		   argc++;
		   }
		}
	 }
  return(main(argc,args));

}
