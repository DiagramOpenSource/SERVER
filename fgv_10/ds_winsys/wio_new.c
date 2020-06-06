
#include <fgvsys.h>
#include <windows.h>

#include <fgvwin.h>
#include <rtermin.h>
#include <teclas.h>

#define DS_WINSYS_API __declspec(dllexport)

#ifdef RWXARXA
#ifdef RWSERVER
extern int en_conexion_externa;
extern int en_conexion_externa_stop;
#endif
extern int en_conexion_terminal;
#endif


extern char *i_nombre();



DS_WINSYS_API void WINAPI pinta_zona(HDC hdc,RECT zona,VENT *pan)
{

}

DS_WINSYS_API void WINAPI mensaje_char(UINT nChar, UINT nRepCnt, UINT nFlags)
{
}

DS_WINSYS_API void WINAPI mensaje_keydown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
}


DS_WINSYS_API void WINAPI mensaje_keyup(UINT nChar, UINT nRepCnt, UINT nFlags)
{
}


DS_WINSYS_API void WINAPI mensaje_ldblclk(UINT x,UINT y,UINT Flags)
{
}


DS_WINSYS_API void WINAPI mensaje_lbuttondown(UINT x,UINT y,UINT Flags)
{
}


DS_WINSYS_API void WINAPI mensaje_mousemove(UINT x1,UINT y1,UINT Flags)
{
}

DS_WINSYS_API void WINAPI mensaje_lbuttonup(UINT x,UINT y,UINT Flags)
{
}


DS_WINSYS_API BOOL WINAPI mensaje_command(WPARAM __wParam,LPARAM lParam)
{
    return TRUE;
}

/********************************************************************************************/

static char *fispo_;
static char *fispo__;
static char *_ejspool_;
static char *_fispool_;
static char *rw_user_;
static int _especial_servidor = 0;

int pregunta_impresion(_fispo,__ejspool,_rw_user,especial_servidor,__fispool)
char *_fispo;
char *__ejspool;
char *_rw_user;
int *especial_servidor;
char *__fispool;
{
   int r;
   
   // Preguntar si usar la impresora interna o las antiguas
   if (DespachadorExterno)
   {
	extern char inicializo[37];
                  r = 1;
                  (*DespachadorExterno)((int)DESPACHO_FUNCIONES_IMPRESION,0,(void *)&r);
                  if (r == 2)
                  {
                     i_leeimpre((char *)1);
                     strcpy(_fispo,i_nombre());
                     __fispool[0] = 0;
                     __ejspool[0] = 0;
                     (*DespachadorExterno)((int)DESPACHO_FUNCIONES_IMPRESION,4,(void *)inicializo);
                     inicializo[35] = strlen(inicializo);
#ifdef RWXARXA
                     if (en_conexion_terminal)
                        *especial_servidor = 0;
#endif
                     return(0);
                  }
                  else
                     if (!r)
                     { // usar la impresora windows interna
                        i_leeimpre(NULL);
                        strcpy(_fispo,i_nombre());
                        __fispool[0] = 0;
                        __ejspool[0] = 0;
#ifdef RWXARXA
                        if (en_conexion_terminal)
                           *especial_servidor = 0;
#endif
                        return(0);
                     }
   }
   
   fispo__ = _fispo;
   fispo_ = _fispo;
   _ejspool_ = __ejspool;
   rw_user_ = _rw_user;
   _fispool_ = __fispool;
   _especial_servidor = *especial_servidor;
   r = DialogBoxParam(hInst, "WAGIMPRE", principal_w /*GetParent(principal_w)*/, agilpDlgImpre,(LPARAM)0L);
   busca_repintado();
#ifdef RWXARXA
   if (en_conexion_terminal)
	  *especial_servidor = _especial_servidor;
#endif
   return(r);
}


int confirmar(pos,mensa,defec)
int pos;
char *mensa;
char *defec;
{
   int qj;
   char tmp[256];
   
   if (que_ptec_hay() == CRETURN)
   {
      leecar();
      if (*defec == 'S' || *defec == 's')
         return(0);
      return(-1);
   }
   
   if (que_ptec_hay() == 'S' || que_ptec_hay() == 's')
   {
      leecar();
      return(0);
   }
   if (que_ptec_hay() == 'N' || que_ptec_hay() == 'n')
   {
      leecar();
      return(-1);
   }
   
   tmp[0] = *defec;
   strcpy(tmp+1,mensa);
   qj = DialogBoxParam(hInst, "WAGICONFIRMAR", principal_w /*GetParent(principal_w)*/, agilpDlgProcx,(LPARAM)tmp);
   busca_repintado();
   if (qj == IDYES || qj == AGIDSI)
      return(0);
   return(-1);
}


/* correcto  pregunta si es correcto o no
*           devuelve  0 si responde S s
*                    -1 si responde N n
*/
int correcto()
{
   int qj;
   
   if (que_ptec_hay() == CRETURN || que_ptec_hay() == 'S' || que_ptec_hay() == 's')
   {
      leecar();
      return(0);
   }
   
   if (que_ptec_hay() == 'N' || que_ptec_hay() == 'n')
   {
      leecar();
      return(-1);
   }
   
   // qj = DialogBoxParam(hInst, "AGICORRECTO", principal_w, agilpDlgProcx,(LPARAM)NULL);
   qj = DialogBoxParam(hInst,"WAGICORRECTO", principal_w /*GetParent(principal_w)*/,agilpDlgProcx,(LPARAM)NULL);
   busca_repintado();
   if (qj == IDYES || qj == AGIDSI)
      return(0);
   if (qj == -1) return(0);
   return(-1);
}


void pito()
{
 MessageBeep(0);
}

void aviso(p,text)
 int p;
char *text;
{
 MessageBeep(0);
 w_mensaje(text);
}


int agi_system(s)
char *s;
{
   HINSTANCE insta = 0;
   int i,j;
   char tmp[128];
   int punto = 0;
   
   for (i = 0;s[i] && s[i] != ' ';i++)
      if (s[i] == '.') punto = 1;
      
      if (!punto)
      {
         if (agi_bats_procesor(s))
         {
#ifdef FGV32
            system(s);
            /*
            for (i = 0;s[i] && s[i] != ' ';i++);
            memcpy(tmp,s,i);
            tmp[i] = 0;
            for (j = i;s[j] && s[j] == ' ';j++);
            strcat(tmp,".exe");
            insta = ShellExecute(principal_w,NULL,tmp,s+j,"",SW_MINIMIZE);
            if (insta < 33)
            {
            memcpy(tmp,s,i);
            tmp[i] = 0;
            strcat(tmp,".bat");
            insta = ShellExecute(principal_w,NULL,tmp,s+j,"",SW_MINIMIZE);
            }
            if (insta > 32)
            {
            _cwait(NULL, insta,NULL);
            }
            */
#else
            insta = ShellExecute(principal_w,NULL,"agishell.pif",s,"",SW_MINIMIZE);
#endif
            for (i = 0;i < 100;i++) /* aligerar sistema */
               agi_cola_win();
         }
         return(0);
      }
      
      
      system(s);
      /*
      memcpy(tmp,s,i);
      tmp[i] = 0;
      while(s[i] == ' ') i++;
      insta = ShellExecute(principal_w,NULL,tmp,s+i,"",SW_SHOW);
      */
      return(0);
}

int agi_execv(s,arg)
char *s;
char *arg[];
{
			HINSTANCE insta;
         char tmp[300];
         char mando[128];
         int i = strlen(s);
         
         strcpy(mando,s);
         if (i)
         {
            i--;
            strmayus(mando);
            if (mando[i] != 'W')
            {
               if (strlen(quita_path(mando)) < 8)
                  strcat(mando,"W"); // !ojo a esto
               else
                  mando[i] = 'W';
               strcat(mando,".EXE");
            }
         }
         
         if (arg[0]) arg[0] = "W";
         tmp[0] = 0;
         for (i = 0;arg[i];i++)
         {
            if (i)
               strcat(tmp," ");
            strcat(tmp,arg[i]);
         }
         insta = ShellExecute(principal_w,NULL,mando,tmp,"", SW_SHOW);
         if (insta < 33)
         {
            strcpy(mando,s);
            strmayus(mando);
            strcat(mando,".EXE");
            // strcpy(tmp,mando);
            tmp[0] = 0;
            for (i = 1;arg[i];i++)
            {
               if (i)
                  strcat(tmp," ");
               strcat(tmp,arg[i]);
            }
            insta = ShellExecute(principal_w,NULL,mando,tmp,"", SW_SHOW);
         }
         else
            ShowWindow(principal_w,SW_MINIMIZE);
         return(0);
}

int rw_error(n)
int n;
{
   char tmp[81];
   sprintf(tmp,"Aviso de Control Interno de Sistema Número %d",n);
   MessageBox(principal_w /*GetParent(principal_w)*/, tmp,NULL, MB_OK | MB_ICONSTOP);
}

/********************************************************************************************/