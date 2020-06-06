
#include <windows.h>
#include <stdio.h>
#include "ds_winsys.h"

#define WMAIN
#define WMAIN_CPP
#include <fgvwin.h>


#define DESPACHO_PROGRESO               4018
#define DESPACHO_TRAEFICHERO            4032

extern "C"
{
HINSTANCE hInst = NULL;
HWND principal_w = NULL;
char ***wwmenus = NULL;
int *nwwmenus = NULL;  
HMENU principal_menu = NULL;
extern int en_menu;
int n_wmenus = 0;

int ptec(int t);

char *i_nombre();
int i_leeimpre(char *f);
char *rw_directorio(char *p);
void *AccesoPython(int f,void *p,int *flag);
int agi_detar(char *pp,char *destino);
int envia_control_a_server(char *,int,int);
int modo_terminal(void);
extern unsigned char la_ethernet_[6];
}


BOOL APIENTRY DllMain( HANDLE hModule, 
                      DWORD  ul_reason_for_call, 
                      LPVOID lpReserved
                      )
{
   switch (ul_reason_for_call)
   {
   case DLL_PROCESS_ATTACH:
   case DLL_THREAD_ATTACH:
   case DLL_THREAD_DETACH:
   case DLL_PROCESS_DETACH:
      break;
   }
   return TRUE;
}


extern "C"
{
   typedef void (*_despachador_)(int,int,void *);   
   extern _despachador_ DespachadorExterno;
   extern int SinColaMensajes;
   extern void montapar_x(char *exe,char *tmp,char *user,char *pass,int opcion);
   extern void Terinici();
   extern void Terfinal();
   extern void fin_bmp_base();
   extern int PonLogo();
   extern int rtme_version1;
   extern int rtme_version2;
   extern char rtme_versionletra;
   extern char *desc_funs[20];
   extern void CuadroTab(int pi,RECT *r);
   extern void Coge_StdPan(void *puntero);
   extern void RePon_StdPan(void *puntero);
   extern int agi_cola_timer(void);
   extern int stdpan_es_invisible(void);
   extern void TxtARect(int pi,int pf,RECT *r);   

   extern HACCEL agi_acelerador;
   extern int leecar();
   extern int que_qjes(int tecla,int flag);


   extern int noboton_usuarios;
   
   extern int en_conexion_terminal;
   
   extern char **lee_direct(char *,int);
   extern void libera_dir(char **dir);
   
   extern int rw_envia_fichero(char *,int);

   extern int RetroDespachador(void *puntero);

   extern int numero_build;

   extern int ModoProfundo;

   extern char dirbass[51];

   extern char server_dirbase1[300];

   extern int envia_subejecuta_a_server();
   extern int entra_util();
   int llama_fun_int(int (*funcion)(),char *par);
   extern int pon_ayuda_de_campo(void *puntero);

   extern int ListaTareas(void *puntero);

   extern char *tecla_funcion(int t,char *p,int m);

   int dame_valor_ini(char *_clave,char *valor);


    extern long server_nume_serie;

	extern long nume_serie;	
	extern long nume_serie_real;
	extern unsigned short local_term[52];
	extern unsigned short local_term_[52];
	extern unsigned short local_acce_[52];	

	struct datos_comunes
	{
		long *nume_serie;
		long *server_nume_serie;
		long *nume_serie_real;
		unsigned short *local_term;
		unsigned short *local_term_;
		unsigned short *local_acce_;
		int *en_conexion_terminal;
		char reservado[1024];		
	} datoscomunes;


   extern int server_internal_build;
   extern int server_fgvid;
   extern int fgv_internal_build;


   int leemensaje_oob_1(int *contador);

   int envia_pingping();

   extern int user_who,subnumero_de_usuario;
}


static BOOL agi_inicio_win(HINSTANCE hInstance,HWND Ventana,int nCmdShow,BMP_BASE_PAR *marco)
{
   hInst = hInstance;
//   principal_w = Ventana;
//   principal_menu = NULL;
   
   init_bmp_base(marco);
   
//   ShowWindow(principal_w,nCmdShow); /* mostrar window */
   return(TRUE); 
}

DS_WINSYS_API int nStatus=0;

static int p;

DS_WINSYS_API BOOL WINAPI fNuevaVentana(HINSTANCE hInstance,HWND Ventana,BMP_BASE_PAR *marco)
{
//   if (principal_w)
//   {
//      Terfinal();
//      fin_bmp_base();
//   }
//   if (hInstance)
//      hInst = hInstance;
//   principal_w = Ventana;
    
//   if (Ventana)
//   {
//      init_bmp_base(marco);
//      Terinici();
//      PonLogo();
//      if (!marco || marco->marco.top || marco->marco.bottom || marco->marco.left || marco->marco.right)
//         ShowWindow(Ventana,SW_SHOW); /* mostrar window */
//      else
//         ShowWindow(Ventana,SW_HIDE);
//   }
   return TRUE;
}

DS_WINSYS_API int WINAPI fPreInicializa(void (*Otro)(),int *argc,char **argv)
{  
   DespachadorExterno = (_despachador_)Otro;
   return pre_inicializa_systema(argc,argv);
}

DS_WINSYS_API int WINAPI fInicializa(HINSTANCE hInstance,HWND Ventana,int argc,char **argv,BMP_BASE_PAR *marco,void (*Otro)())
{   
   if (!agi_inicio_win(hInstance,Ventana,SW_SHOW,marco))
   {
      return 0;
   }   
   
   if ((p = inicializa_systema(argc,argv)) < 0)
   {
      if (p == -10)
      {
         return p;
      }
      return 0;
   }
   
   return 1;
}

DS_WINSYS_API BOOL WINAPI fFinaliza()
{
//   if (!principal_w) return FALSE;
   finaliza_systema();
//   fin_bmp_base();
//   principal_w = NULL;
   DespachadorExterno = NULL;
   return TRUE;
}


DS_WINSYS_API HWND WINAPI fDevuelveVentana()
{
   return(principal_w);
}

DS_WINSYS_API int WINAPI fLogin(char *usuario,char *password,char *cambio)
{
   
   return login_usuario(p,usuario,password,cambio);
}

DS_WINSYS_API BOOL WINAPI fOpcion(void (*Otro)())
{
   //SinColaMensajes = 1;
   //DespachadorExterno = Otro;   
   if (opcion_usuario(p))
   {
      SinColaMensajes = 0;
      return TRUE;
   }
   SinColaMensajes = 0;
   //DespachadorExterno = NULL;
   return FALSE;
}


DS_WINSYS_API BOOL WINAPI fLogout()
{
   //DespachadorExterno = NULL;
   if (logout_usuario())
   {
      return TRUE;
   }   
   return FALSE;
}

DS_WINSYS_API BOOL WINAPI fPreparaEjecucion(char *exe,char *tmp,char *user,char *pass,int opcion)
{
   montapar_x(exe,tmp,user,pass,opcion);
   return TRUE;
}


DS_WINSYS_API int WINAPI fDespachadorInterno(int funcion,void *puntero)
{
   switch (funcion)
   {
   case 0: // recoger version       
      return( ((rtme_version1*100+rtme_version2)*256+rtme_versionletra) | (numero_build << 24) );
   case 1: // recoger funciones programadas
      *((char ***)puntero) = (char **)desc_funs;
      return(20);
      break;
   case 2:
      {
         int pi;
         RECT r;
         char *punt = (char *)puntero;
         memcpy(&pi,punt,sizeof(int));		 
         CuadroTab(pi,&r);
         memcpy(punt,&r,sizeof(RECT));
      }
      break;
   case 3:
      Coge_StdPan(puntero);
      break;
   case 4:
      RePon_StdPan(puntero);
      break;
   case 5:
      agi_cola_timer();
      break;
   case 6:
      stdpan_es_invisible();
      break;
   case 7:
      {
         int pi,pf,flag1,flag2;
         pi = ((RECT *)puntero)->left;
         pf = ((RECT *)puntero)->top;
         flag1 = ((RECT *)puntero)->right;
         flag2 = ((RECT *)puntero)->bottom;
         TxtARect(pi,pf,(RECT *)puntero);
         if (!flag1 && !flag2 && ModoProfundo)
         {
             ((RECT *)puntero)->top += 2;
             ((RECT *)puntero)->bottom -= 2;
         }
      }
      break;
   case 8:
      {
         short sqj;
         unsigned int qj;
         short tecla;
         HACCEL _agi_acelerador = agi_acelerador;
         memcpy((void*)&qj,(void*)puntero,sizeof(int));
         agi_acelerador = NULL;
         tecla = (short)leecar();
         sqj = (short)que_qjes(tecla,qj);
         qj = ((((unsigned int)tecla) << 16) & 0xffff0000);         
         qj |= (((unsigned int)sqj) & 0xffff);
         memcpy((void*)puntero,(void*)&qj,sizeof(int));
         agi_acelerador = _agi_acelerador;
      }
      break;
   case 9:
      return(en_conexion_terminal);
      break;
   case 10:
      {
         char **_dir = lee_direct(*((char **)puntero),3);
         *((char **)puntero) = (char *)_dir;
      }
      break;
   case 11:
      libera_dir((char **)puntero);
      break;
   case 12:
      return (rw_envia_fichero((char *)puntero,777));
      break;
    case 13: /* trasmision binaria inversa */
		     /* formato = int(funcion) long(tamano-in) long(tamano-out) long(modo)*/
	  if (RetroDespachador(puntero) < 0)
	  {
		  return -1;
	  } 
	  break;
    case 14:
      strcpy((char *)puntero,dirbass);
      break;
    case 15:
      break;
	case 16:
	  if (en_conexion_terminal)
	  {
		  llama_fun_int(envia_subejecuta_a_server,(char *)puntero);
	  }
	  else
	  {
		  llama_fun_int(entra_util,(char *)puntero);
	  }
	  break;
	case 17:
	{
	    pon_ayuda_de_campo(puntero);
	}
	  break;
	case 18:
	{
		/* enviar y borrar teclas de funcion programadas */
		char **pp = (char **)puntero;		
		char *p;
		char **p1;
		int i;
		*((int *)*pp) = 0/*FUNCIONES*/;
		pp++;
		p1 = NULL;/*(char **)malloc(sizeof(char *)*FUNCIONES);*/
		*pp = (char *)p1;

		for (i = 0;i < 0/*FUNCIONES*/;i++)
		{
			p = NULL;
			p = tecla_funcion(i,p,1);
			if (p && *p != '@' && *p != '#')
			{
				p1[i] = p;
				tecla_funcion(i,NULL,99);
			}
			else
				p1[i] = NULL;
		}
	}
	break;
	case 19:
		if (puntero)
			free(puntero);
		break;
	case 20:	
		/*
	  if (en_conexion_terminal)
	  {
		  memcpy(puntero,&remoto_nume_serie,sizeof(long));
	  }
	  else
	  {
		  memcpy(puntero,&nume_serie,sizeof(long));
	  }
	  */
	break;
   case 21:
      return(noboton_usuarios);
      break;
	case 22:
		/* datos ini */
		{
			char clave[256];
			strcpy(clave,(char *)puntero);
		    return dame_valor_ini(clave,(char *)puntero);
		}
	   break;
	case 23:
	{
		datoscomunes.nume_serie = &nume_serie;		
		datoscomunes.server_nume_serie = &server_nume_serie;
		datoscomunes.nume_serie_real = &nume_serie_real;
		datoscomunes.local_term = &(local_term[0]);
		datoscomunes.local_term_ = &(local_term_[0]);
		datoscomunes.local_acce_ = &(local_acce_[0]);
		datoscomunes.en_conexion_terminal = &en_conexion_terminal;		
		memset(datoscomunes.reservado,0,sizeof(datoscomunes.reservado));
		return (int)&datoscomunes;
	}
	break;
	case 24: /* ! */
	  break;
	case 25:
	{
	  if (!en_conexion_terminal)
	  {
		  memcpy(puntero,&fgv_internal_build,sizeof(int));
		  return 2147;
	  }
	  else
	  {
		  memcpy(puntero,&server_internal_build,sizeof(int));
		  return server_fgvid;
	  }	
	}
	break;
	case 26:
	{
	  char tmp[512],destino[128];		 
	  unsigned long tam;

	  tam = 3;
		/* instalacion python */	  
	  if (en_conexion_terminal)
	  {		 
		 strcpy(tmp,rw_directorio("agitool/python.tgz"));         
		 unlink(tmp);
		 tmp[0] = 3;
		 strcpy(tmp+1,"agitool/python.tgz");         
		 envia_control_a_server(tmp,strlen(tmp)+1,170);
		 modo_terminal();
	  }
	  /*
	  (*DespachadorExterno)((int)DESPACHO_PROGRESO,0,(void *)&tam);
	  tam = 1;
	  (*DespachadorExterno)((int)DESPACHO_PROGRESO,1,(void *)&tam);
      agi_cola_timer();
	  */
	  tmp[0] = 7;
	  strcpy(tmp+1,rw_directorio("agitool/python.tgz"));
	  strcpy(destino,rw_directorio(""));
	  agi_detar(tmp,destino);
	  /*unlink(rw_directorio("python/python.tgz"));*/
	  /*(*DespachadorExterno)((int)DESPACHO_PROGRESO,2,NULL);*/
	  return 0;
	}
	break;
	case 27:
	{
		char *pdbs;
		if (en_conexion_terminal)		  		
			pdbs = server_dirbase1;
		else
			pdbs = dirbass;		
		sprintf((char *)puntero,"%s %03d%03d",pdbs,user_who,subnumero_de_usuario);
		return 0;
	}
	break;
	case 28:
	{
		memcpy((char*)puntero,la_ethernet_,6);
	}
	break;
	case 29:
	{
		/* siguiente libre */
	}
	break;
    case 99:
      ptec(807);
      ptec(807);
      ptec(807);
      ptec(807);
      ptec(807);
      ptec(807);
      ptec(807);
      ptec(807);
      ptec(807);
      ptec(807);
      ptec(807);
      ptec(807);
      ptec(807);
      ptec(807);
      ptec(807);
      ptec(807);
      ptec(807);
      break;
	case 199:
		return leemensaje_oob_1((int *)puntero);
	case 299:
		/* ping */
	  if (en_conexion_terminal)
	  {
		 envia_pingping();
	  }
	  break;

		break;
	case 500:
		return ListaTareas(puntero);
	case 515:
      {
        char tmp[256];
        int *p1 = (int *)puntero;
        strcpy(tmp,rw_directorio("dev/imp/"));
        strcat(tmp,(char *)(p1+1));        
        int r = i_leeimpre(tmp);
        *p1 = r;
        if (r >= 0)
        {
            strcpy((char *)(p1+1),i_nombre());
        }
      }
	  break;
   case 600:
	  return (int)AccesoPython;
	  break;
   default:
      break;
   }
   return 0;
}


//agi_main(argc,argv);
