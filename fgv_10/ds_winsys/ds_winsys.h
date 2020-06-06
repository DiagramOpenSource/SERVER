
//#ifdef DS_WINSYS_EXPORTS
#define DS_WINSYS_API __declspec(dllexport)
//#else
//#define DS_WINSYS_API __declspec(dllimport)
//#endif

extern "C"
{
#include <fgvmnexp.h>
}

DS_WINSYS_API int WINAPI fInicializa(HINSTANCE hInstance,HWND Padre,int argc,char *argv[],BMP_BASE_PAR *marco);
DS_WINSYS_API BOOL WINAPI fFinaliza(void);
DS_WINSYS_API int WINAPI fLogin(char *usuario,char *password,char *cambio);
DS_WINSYS_API BOOL WINAPI fOpcion(void (*Otro)());
DS_WINSYS_API BOOL WINAPI fLogout(void);
DS_WINSYS_API HWND WINAPI fDevuelveVentana(void);
DS_WINSYS_API BOOL WINAPI fNuevaVentana(HINSTANCE hInstance,HWND Padre,BMP_BASE_PAR *marco);
DS_WINSYS_API BOOL WINAPI fPreparaEjecucion(char *exe,char *tmp,char *user,char *pass,int opcion);

extern "C"
{
   long FAR PASCAL AGI_principal_wnd(HWND,unsigned,unsigned,LONG);
   int init_bmp_base(BMP_BASE_PAR *pRorigen);
   int quitab(char *s);
   int quitabp(char *s);
   int pre_inicializa_systema(int *agi_argc,char *agi_argv[]);
   int inicializa_systema(int agi_argc,char *agi_argv[]);
   int login_usuario(int p,char *usuario,char *password,char *cambio);
   int opcion_usuario(int p);
   int logout_usuario(void);
   int finaliza_systema(void);
}
