
#include <fgvsys.h>

#ifndef DSPROFUIS9

#include <stdio.h>
#include <windows.h>
#include <windowsx.h>
#ifndef FGV32
#include <print.h>
#endif
#include <shellapi.h>
#include <commdlg.h>

#include <io.h>
#define __MSC
#include <dos.h>


#include <time.h>
#include <sys/timeb.h>

#include <rtermin.h>
#include <teclas.h>

#include <fgvwin.h>
#include <fgvmnexp.h>

#define DS_WINSYS_API __declspec(dllexport)


#ifdef FGV32
#define MoveTo(a,b,c) MoveToEx(a,b,c,NULL)
#endif

#ifdef FGVDOS
extern unsigned long nt_version;
#endif

#ifdef FGV32
#ifndef FGVWIN
extern HANDLE myconsola;
extern HANDLE hStdIn;
#endif
#endif

#ifdef RWXARXA
#ifdef RWSERVER
extern int en_conexion_externa;
extern int en_conexion_externa_stop;
#endif
extern int en_conexion_terminal;
#endif


extern int tracer_int;

extern char *i_nombre();
extern int i_anchocarro;

extern int fgv_ver_euro;


extern int NO_PINTAR_AHORA;

extern int os_subtipo;

extern int modo_desatendido;

int un_fondo_especial = 0;

int SinColaMensajes = 0;

extern int (*timer_especial_int)();

extern int PCcolor;
extern int PFcolor;
extern int _PCcolor;
extern int _PFcolor;

extern int win_fondo_neutro;

extern int proporcion_especial_bmp;

extern char *desc_funs[200];

static HBRUSH agi_oldbrush = NULL;
static HFONT f_ant_Font;
HACCEL agi_acelerador = NULL;
HACCEL orig_acelerador = NULL;

static WORD PcxInch = 0;
static WORD PcyInch = 0;

extern char *quita_path();

int agi_cola_win();
int agi_cola_timer();

static short era_icono = 0;
static int desactivada = 0;
static unsigned int para_pintar = 0;
static unsigned int cursorx = 0;
static unsigned int cursory = 0;
unsigned int salida_caracter = 0;
static int inix = 0;
static int iniy = 0;
static unsigned int topesx = 0;
static unsigned int topesy = 0;
static unsigned int paginax = 0;
static unsigned int paginay = 0;
static int caracterdy = 0;
static int caracterdy1 = 0;
static int caracterdy2 = 0;
static int caracterdy3 = 0;
static int caracterdy4 = 0;
static int caracterdy5 = 0;
static int caracterdy6 = 0;

static int PuedoCambiarCursor = 1;

extern char NombreClase[81];

extern char empresa[256];

static int espacio_abajo = -1;

static int boton_actual = 0;

static HFONT pr_f_n = NULL;
static HFONT pr_f_c = NULL;
static HFONT pr_f_e = NULL;
static HFONT pr_f_b = NULL;
static HFONT old_pr_f = NULL;

COLORREF color_custom_subrayado = RGB(0,0,248);
COLORREF color_fondo_campo = RGB(255,255,255);
COLORREF color_texto_campo = RGB(0,0,0);

extern int duplicarusuario;

int FAR PASCAL AbortProc(HDC, int);
int FAR PASCAL AbortDlg(HWND, unsigned, WORD, LONG);

int LINEA24_Y = 24;

int EXTRAIZQUIERDA = 0;
int EXTRADERECHA = 17; // reservado para posible scroll-bar


int No_Procesar_Mensajes = 0;

extern int ModoProfundo;

extern int abandonarelbarco;

extern int Pseudo10;			 
extern int usar_grid;


extern int recoge_todo_caracter;

extern char *dato_entra;
extern int dato_entra_len;


extern char *telefonos_claves;


static PRINTDLG pd;
HDC hPr;                            /* handle for printer device context     */
int LineSpace;                      /* spacing between lines          */
int LinesPerPage;                   /* lines per page                 */
int CurrentLine;                    /* current line                   */
int LineLength;                     /* line length                    */
int Carlen;
WORD wLines;                        /* number of lines to print       */
WORD wIndex;                        /* index into lines to print      */
BOOL bAbort;                        /* FALSE if user cancels printing      */
HWND hAbortDlgWnd;
FARPROC lpAbortDlg, lpAbortProc;
extern int i_nlineas;
extern int i_anchocarro;

static HANDLE hData = NULL,hClipData = NULL;                // handles to clip data
static LPSTR lpClipData;               // pointers to clip data


static int posible_char = 0;

static char *_num1 = NULL;
static char *_num2 = NULL;
static char *_res = NULL;


static HCURSOR cur_flecha = NULL; // 1
static HCURSOR cur_mano = NULL;   // 2
static HCURSOR cur_boton = NULL;  // 3
static HCURSOR cur_prohibido = NULL; // 4
static HCURSOR cur_espera = NULL;    // 5
static HCURSOR cur_pulsado = NULL;    // 6
static HCURSOR cur_ini = NULL;
static int tipo_cursor = 0;

static char C_agigrafico[128] = {
   /*CODIGO INTERNO*/ /* Gráfico */
   /* 128*/                  0,
   /* 129*/                  0,
   /* 130*/                  0,
   /* 131*/                  0,
   /* 132*/                  0,
   /* 133*/                  0,
   /* 134*/                  0,
   /* 135*/                  0,
   /* 136*/                  0,
   /* 137*/                  0,
   /* 138*/                  0,
   /* 139*/                  0,
   /* 140*/                  0,
   /* 141*/                  0,
   /* 142*/                  0,
   /* 143*/                  0,
   /* 144*/                  0,
   /* 145*/                  0,
   /* 146*/                  0,
   /* 147*/                  0,
   /* 148*/                  0,
   /* 149*/                  0,
   /* 150*/                  0,
   /* 151*/                  0,
   /* 152*/                  0,
   /* 153*/                  0,
   /* 154*/                  0,
   /* 155*/                  0,
   /* 156*/                  0,
   /* 157*/                  0,
   /* 158*/                  0,
   /* 159*/                  0,
   /* 160*/                  0,
   /* 161*/                  0,
   /* 162*/                  0,
   /* 163*/                  0,
   /* 164*/                  0,
   /* 165*/                  0,
   /* 166*/                  0,
   /* 167*/                  0,
   /* 168*/                  0,
   /* 169*/                  0,
   /* 170*/                  0,
   /* 171*/                  0,
   /* 172*/                  0,
   /* 173*/                  0,
   /* 174*/                  0,
   /* 175*/                  0,
   /* 176*/                  1,
   /* 177*/                  1,
   /* 178*/                  1,
   /* 179*/                  3,
   /* 180*/                  2,
   /* 181*/                  2,
   /* 182*/                  2,
   /* 183*/                  2,
   /* 184*/                  2,
   /* 185*/                  2,
   /* 186*/                  3,
   /* 187*/                  2,
   /* 188*/                  2,
   /* 189*/                  2,
   /* 190*/                  2,
   /* 191*/                  2,
   /* 192*/                  2,
   /* 193*/                  2,
   /* 194*/                  2,
   /* 195*/                  2,
   /* 196*/                  3,
   /* 197*/                  1,
   /* 198*/                  2,
   /* 199*/                  2,
   /* 200*/                  2,
   /* 201*/                  2,
   /* 202*/                  2,
   /* 203*/                  2,
   /* 204*/                  2,
   /* 205*/                  3,
   /* 206*/                  1,
   /* 207*/                  2,
   /* 208*/                  2,
   /* 209*/                  2,
   /* 210*/                  2,
   /* 211*/                  2,
   /* 212*/                  2,
   /* 213*/                  2,
   /* 214*/                  2,
   /* 215*/                  1,
   /* 216*/                  1,
   /* 217*/                  2,
   /* 218*/                  2,
   /* 219*/                  1,
   /* 220*/                  1,
   /* 221*/                  1,
   /* 222*/                  1,
   /* 223*/                  1,
   /* 224*/                  0,
   /* 225*/                  0,
   /* 226*/                  0,
/* 227*/                  0,
/* 228*/                  0,
/* 229*/                  0,
/* 230*/                  0,
/* 231*/                  0,
/* 232*/                  0,
/* 233*/                  0,
/* 234*/                  0,
/* 235*/                  0,
/* 236*/                  0,
/* 237*/                  0,
/* 238*/                  0,
/* 239*/                  0,
/* 240*/                  0,
/* 241*/                  0,
/* 242*/                  0,
/* 243*/                  0,
/* 244*/                  0,
/* 245*/                  0,
/* 246*/                  0,
/* 247*/                  0,
/* 248*/                  0,
/* 249*/                  0,
/* 250*/                  0,
/* 251*/                  0,
/* 252*/                  0,
/* 253*/                  0,
/* 254*/                  0,
/* 255*/                  0
    };
#define MAXMOSAICO 48
    static int C_mosaico[128] = {
       /*CODIGO INTERNO*/ /* Gráfico */
       /* 128*/                  0,
       /* 129*/                  0,
       /* 130*/                  0,
       /* 131*/                  0,
       /* 132*/                  0,
       /* 133*/                  0,
       /* 134*/                  0,
       /* 135*/                  0,
       /* 136*/                  0,
       /* 137*/                  0,
       /* 138*/                  0,
       /* 139*/                  0,
       /* 140*/                  0,
       /* 141*/                  0,
       /* 142*/                  0,
       /* 143*/                  0,
       /* 144*/                  0,
       /* 145*/                  0,
       /* 146*/                  0,
       /* 147*/                  0,
       /* 148*/                  0,
       /* 149*/                  0,
       /* 150*/                  0,
       /* 151*/                  0,
       /* 152*/                  0,
       /* 153*/                  0,
       /* 154*/                  0,
       /* 155*/                  0,
       /* 156*/                  0,
       /* 157*/                  0,
       /* 158*/                  0,
       /* 159*/                  0,
       /* 160*/                  0,
       /* 161*/                  0,
       /* 162*/                  0,
       /* 163*/                  0,
       /* 164*/                  0,
       /* 165*/                  0,
       /* 166*/                  0,
       /* 167*/                  0,
       /* 168*/                  0,
       /* 169*/                  0,
       /* 170*/                  0,
       /* 171*/                  0,
       /* 172*/                  0,
       /* 173*/                  0,
       /* 174*/                  0,
       /* 175*/                  0,
       /* 176*/                  1,
       /* 177*/                  2,
       /* 178*/                  3,
       /* 179*/                  4,
       /* 180*/                  5,
       /* 181*/                  6,
       /* 182*/                  7,
       /* 183*/                  8,
       /* 184*/                  9,
       /* 185*/                  10,
       /* 186*/                  11,
       /* 187*/                  12,
       /* 188*/                  13,
       /* 189*/                  14,
       /* 190*/                  15,
       /* 191*/                  16,
       /* 192*/                  17,
       /* 193*/                  18,
       /* 194*/                  19,
       /* 195*/                  20,
       /* 196*/                  21,
       /* 197*/                  22,
       /* 198*/                  23,
       /* 199*/                  24,
       /* 200*/                  25,
       /* 201*/                  26,
       /* 202*/                  27,
       /* 203*/                  28,
       /* 204*/                  29,
       /* 205*/                  30,
       /* 206*/                  31,
       /* 207*/                  32,
       /* 208*/                  33,
       /* 209*/                  34,
       /* 210*/                  35,
       /* 211*/                  36,
       /* 212*/                  37,
       /* 213*/                  38,
       /* 214*/                  39,
       /* 215*/                  40,
       /* 216*/                  41,
       /* 217*/                  42,
       /* 218*/                  43,
       /* 219*/                  44,
       /* 220*/                  45,
       /* 221*/                  46,
       /* 222*/                  47,
       /* 223*/                  48,
       /* 224*/                  0,
       /* 225*/                  0,
       /* 226*/                  0,
/* 227*/                  0,
/* 228*/                  0,
/* 229*/                  0,
/* 230*/                  0,
/* 231*/                  0,
/* 232*/                  0,
/* 233*/                  0,
/* 234*/                  0,
/* 235*/                  0,
/* 236*/                  0,
/* 237*/                  0,
/* 238*/                  0,
/* 239*/                  0,
/* 240*/                  0,
/* 241*/                  0,
/* 242*/                  0,
/* 243*/                  0,
/* 244*/                  0,
/* 245*/                  0,
/* 246*/                  0,
/* 247*/                  0,
/* 248*/                  0,
/* 249*/                  0,
/* 250*/                  0,
/* 251*/                  0,
/* 252*/                  0,
/* 253*/                  0,
/* 254*/                  0,
/* 255*/                  0
    };
    
    static HBITMAP C_agibmp = NULL; /* bitmap mosaico ara componer las pantallas */
    
    extern int (*f_util)();
    
    extern int en_lee_teclado;
    
    /* DEFINICIONES PARA LECTURA DE GRAFICOS */
    
#define BFT_ICON   0x4349   /* 'IC' */
#define BFT_BITMAP 0x4d42   /* 'BM' */
#define BFT_CURSOR 0x5450   /* 'PT' */
#define MAXREAD  32768
#define WIDTHBYTES(i)   ((i+31)/32*4)      /* ULONG aligned ! */
#define ISDIB(bft) ((bft) == BFT_BITMAP)
#define PALVERSION      0x300
#define MAXPALETTE      256
    
    static HANDLE     hbiCurrent = NULL;
    static DWORD      dwOffset;
    static HPALETTE   hpalCurrent = NULL;
    static HPALETTE   OldPalette = NULL;
    static BOOL       fPalColors = FALSE;
    static BOOL       bLegitDraw = FALSE;
    
    static int en_espera_de_pinta = 0;
    
    
    extern int pulsa_en_espacio;
    
    
    
#define SL_BOX    1             /* Draw a solid border around the rectangle  */
#define SL_BLOCK  2             /* Draw a solid rectangle                    */
    
#define SL_EXTEND 256           /* Extend the current pattern                */
    
#define SL_TYPE    0x00FF       /* Mask out everything but the type flags    */
#define SL_SPECIAL 0xFF00       /* Mask out everything but the special flags */
    
    // Temporary porting macros
    
#define INT2DWORD               DWORD
    typedef POINTS MPOINT;
#define MAKEMPOINT(l)           (*((MPOINT *)&(l)))
#define MPOINT2POINT(mpt, pt)   ((pt).x = (mpt).x, (pt).y = (mpt).y)
#define POINT2MPOINT(pt, mpt)   ((mpt).x = (SHORT)(pt).x, (mpt).y = (SHORT)(pt).y)
    
    
    int StartSelection(HWND, MPOINT, LPRECT, int);
    int UpdateSelection(HWND, MPOINT, LPRECT, int);
    int EndSelection(MPOINT, LPRECT);
    int ClearSelection(HWND, LPRECT, int);
    
    
    
    static int busca_repintado()
    {
       en_espera_de_pinta = 1;
       while(en_espera_de_pinta == 1) agi_cola_win();
    }
    
    int w_mensaje(LPSTR mensa)
    {
       int r;
       char tmp[256];
       tmp[0] = 0;
       strcpy(tmp+1,mensa);
       MessageBeep(MB_ICONEXCLAMATION);
       r = DialogBoxParam(hInst, "WAGIAVISO", principal_w /*GetParent(principal_w)*/, agilpDlgProcx,(LPARAM)tmp);
       busca_repintado();
       return(r);
    }
    
    
    static int old_mensaje(pos,str)
       int pos;
    char *str;
    {
       int f1,c1;
       
       f1 = pos/100;
       c1 = pos%100;
       if (f1 <= 0)
          f1 = 4;                    /* fila en pantalla del mensaje */
       if (c1 <= 0)
          c1 = (80-strlen(str))/2;   /* columna         "            */
       Bl(f1);
       if (*str != '\0') {
          pprint(f1*100+c1,str);
          fflush(stdout);
       }
       return(f1);
    }
    
#ifdef COMENTARIO
    static BOOL AgiGetClientRect(HWND principal_w,LPRECT principal_rect)
			 {
       int tamax,tamay;
       
       GetClientRect(principal_w,principal_rect);
       tamax = principal_rect->right - principal_rect->left;
       tamay = principal_rect->bottom - principal_rect->top;
       if (tamay > 480)
       {
          principal_rect->bottom = principal_rect->top + 480;
       }
       if (tamax > 640)
       {
          principal_rect->right = principal_rect->left + 640;
       }  
       return TRUE;
			 }
#endif
    
    static int __mensaje_persistente = 0;
    
    
    int mensaje_persistente(str,tiempo)
       char *str;
    int tiempo;
			 {
             __mensaje_persistente = -1;
             mensaje_especial(str);
             __mensaje_persistente = time(NULL) + tiempo;
             pito(183);
             return(0);
          }
          
          int mensaje_especial(str)
             char *str;
          {
             int ntext = strlen(str);
             HDC hdc;
             HFONT oldfont;
             RECT espacio;
             COLORREF hcolor = RGB(0,0,248);
             SIZE ss;
             int cx,cy;
             int tx;
             static hay_mensa = 0;
             int mueve_mueve = 0;
             
             if (__mensaje_persistente < 0)
             {
                mueve_mueve = 1;
                __mensaje_persistente = 0;
             }
             else
                if (__mensaje_persistente && __mensaje_persistente >= time(NULL)) 
                   return(0);
                else
                   __mensaje_persistente = 0;
                
                if (!hay_mensa && !str[0]) return(0);
                if (str[0])
                   hay_mensa = 1;
                else
                   hay_mensa = 0;
                
                if (DespachadorExterno)
                {
                   (*DespachadorExterno)((int)DESPACHO_MENSAJE_CABECERA,-1,str);
                }
                
                /*mueve_mueve ??? */
                
                return(0);
          }
          
          
          int mensaje(pos,str)
             int pos;
          char *str;
          {
             int ntext = strlen(str);
             HDC hdc;
             HFONT oldfont;
             RECT espacio;
             COLORREF hcolor = RGB(0,0,128);
             SIZE ss;
             int cx,cy;
             char tmp[128];
             int pp,pp2;
             
             pp2 = 0;
             for (pp = 0;str[pp];pp++)
             {
                tmp[pp2] = str[pp];
                if (str[pp] == 'F' && str[pp+1] == '1' && (str[pp+2] < '0' || str[pp+2] > '9'))
                {
                   pp++;
                   pp2++;
                   tmp[pp2] = '1';
                   pp2++;
                   tmp[pp2] = '2';
                }
                pp2++;
             }
             if (pp2 > 80) pp2 = 80;
             tmp[pp2] = 0;
             str = tmp;
             
             if (pos && pos/100 != 4) return(old_mensaje(pos,str));
             SetTextColor(stdpan->vhdc,hcolor);
             
             hOldBitmap = SelectObject(hMemoryDC,agitapiz);
             BitBlt(stdpan->vhdc,0,espacio_arriba+scaracter.cy*3,640,scaracter.cy,hMemoryDC,0,espacio_arriba+scaracter.cy*3,SRCCOPY);
             SelectObject(hMemoryDC,hOldBitmap);
             hOldBitmap = NULL;
             memset(stdpan->vv+240,0,80);
             
             oldfont = SelectObject(stdpan->vhdc,agi_fmensa);
             GetTextExtentPoint(stdpan->vhdc,str,ntext,&ss);
             cx = 6+(628 - ss.cx) / 2;
             cy = espacio_arriba+scaracter.cy*3+1+(16 - ss.cy) / 2;
             SetRect(&espacio,cx-2,espacio_arriba+scaracter.cy*3+1,cx+ss.cx+2,espacio_arriba+scaracter.cy*4-2);
             
             MoveTo(stdpan->vhdc,espacio.left,espacio.bottom);
             LineTo(stdpan->vhdc,espacio.right,espacio.bottom);
             //FrameRect(stdpan->vhdc,&espacio,(HBRUSH)GetStockObject(BLACK_BRUSH));
             
             SetRect(&espacio,6,espacio_arriba+scaracter.cy*3,628,espacio_arriba+scaracter.cy*4-1);
             ExtTextOut(stdpan->vhdc,cx,cy,ETO_CLIPPED,&espacio,str,ntext,NULL);
             SelectObject(stdpan->vhdc,oldfont);
             if (!NO_PINTAR_AHORA)
             {
                hdc = GetDC(principal_w);
                BitBlt(hdc,principal_rect.left,principal_rect.top+espacio_arriba+scaracter.cy*3,640,scaracter.cy,stdpan->vhdc,0,espacio_arriba+scaracter.cy*3,SRCCOPY);
                ReleaseDC(principal_w,hdc);
             }
             return(0);
          }
          
          void pito(int codigo)
          {
             MessageBeep(0);
          }
          
          void aviso(p,text)
             int p;
          char *text;
          {
			 if (!abandonarelbarco)
			 {
				 MessageBeep(0);
				 w_mensaje(text);
			 }
          }
          
          
          void w_wcursor(c,f)
             int c,f;
          {
             cursorx = principal_rect.left+scaracter.cx * (c-1);
             cursory = principal_rect.top+espacio_arriba+scaracter.cy * (f-1);
#ifdef COMENTARIO
             if (f == 24)
             {
                cursorx += 3;
                cursory += 5;
             }
             else
#endif
                if (f == 25)
                {
                   cursorx = 0;
                   cursory = 0;
                }
                SetCaretPos(cursorx,cursory);
          }
          
          static RECT agi_boton[MAXBOTON];
          static HBITMAP agi_boton_up[MAXBOTON];
          static HBITMAP agi_boton_down[MAXBOTON];
          static HBITMAP agi_boton_updefecto[MAXBOTON];
          static short agi_boton_id[MAXBOTON];
          static char agi_boton_flag[MAXBOTON];
          
          
          static int boton_recur = 0;
          static RECT *_agi_boton[100];
          static HBITMAP *_agi_boton_up[100];
          static HBITMAP *_agi_boton_down[100];
          static HBITMAP *_agi_boton_updefecto[100];
          static short *_agi_boton_id[100];
          static char *_agi_boton_flag[100];
          HBITMAP icono_opcion[100] = {NULL,};
          DWORD icono_pos[100] = {0L,};
          static HBITMAP *_icono_opcion[100];
          static DWORD *_icono_pos[100];
          
          
          
          int recurre_boton(m)
             int m;
          {
             int i;
             
             if (!m)
             {
                if (boton_recur >= 100) 
                   return(-1);
                _agi_boton[boton_recur] = malloc(sizeof(RECT) * MAXBOTON);
                _agi_boton_up[boton_recur] = malloc(sizeof(HBITMAP) * MAXBOTON);
                _agi_boton_down[boton_recur] = malloc(sizeof(HBITMAP) * MAXBOTON);
                _agi_boton_updefecto[boton_recur] = malloc(sizeof(HBITMAP) * MAXBOTON);
                _agi_boton_id[boton_recur] = malloc(sizeof(short) * MAXBOTON);
                _agi_boton_flag[boton_recur] = malloc(MAXBOTON);
                
                _icono_opcion[boton_recur] = malloc(sizeof(HBITMAP) * 100);
                _icono_pos[boton_recur] = malloc(sizeof(DWORD) * 100);
                memcpy(_icono_opcion[boton_recur],icono_opcion,sizeof(HBITMAP) * 100);
                memcpy(_icono_pos[boton_recur],icono_pos,sizeof(DWORD) * 100);
                
                memcpy(_agi_boton[boton_recur],agi_boton,sizeof(RECT) * MAXBOTON);
                memcpy(_agi_boton_up[boton_recur],agi_boton_up ,sizeof(HBITMAP) * MAXBOTON);
                memcpy(_agi_boton_down[boton_recur],agi_boton_down,sizeof(HBITMAP) * MAXBOTON);
                memcpy(_agi_boton_updefecto[boton_recur],agi_boton_updefecto,sizeof(HBITMAP) * MAXBOTON);
                memcpy(_agi_boton_id[boton_recur],agi_boton_id,sizeof(short) * MAXBOTON);
                memcpy(_agi_boton_flag[boton_recur],agi_boton_flag,MAXBOTON);
				/*
                for (i = 0;i < 29;i++)
                {
                   if (IsRectEmpty(agi_boton+i))
                   {
                      //aviso(0,"boton!!");
                   }
                }
				*/
                for (i =0;i < MAXBOTON;i++)
                {
                   SetRectEmpty(agi_boton+i);
                   agi_boton_up[i] = NULL;
                   agi_boton_updefecto[i] = NULL;
                   agi_boton_down[i] = NULL;
                   agi_boton_id[i] = 0;
                   agi_boton_flag[i] = 0;
                }
                for (i = 0;i < 100;i++)
                {
                   icono_opcion[i] = NULL;
                }
                boton_recur++;
             }
             else
             {
                if (!boton_recur) return(-1);
                boton_recur--;
                for (i = 0;i < MAXBOTON;i++)
                {
                   if (!IsRectEmpty(agi_boton+i))
                   {
                      DeleteObject(agi_boton_up[i]);
                      DeleteObject(agi_boton_down[i]);
                      if (agi_boton_updefecto[i])
                         DeleteObject(agi_boton_updefecto[i]);
                      SetRectEmpty(agi_boton+i);
                      agi_boton_up[i] = NULL;
                      agi_boton_down[i] = NULL;
                      agi_boton_updefecto[i] = NULL;
                      agi_boton_id[i] = 0;
                      agi_boton_flag[i] = 0;
                   }
                }
                
                for (i = 0;i < 100;i++)
                {
                   if (icono_opcion[i])
                   { // aqui nno se repinta ... esto es por seguridad
                      DeleteObject(icono_opcion[i]);
                      icono_opcion[i]=NULL;
                   }
                }
                memcpy(icono_opcion,_icono_opcion[boton_recur],sizeof(HBITMAP) * 100);
                memcpy(icono_pos,_icono_pos[boton_recur],sizeof(DWORD) * 100);
                free(_icono_opcion[boton_recur]);
                free(_icono_pos[boton_recur]);
                
                memcpy(agi_boton,_agi_boton[boton_recur],sizeof(RECT) * MAXBOTON);
                memcpy(agi_boton_up,_agi_boton_up[boton_recur],sizeof(HBITMAP) * MAXBOTON);
                memcpy(agi_boton_down,_agi_boton_down[boton_recur],sizeof(HBITMAP) * MAXBOTON);
                memcpy(agi_boton_updefecto,_agi_boton_updefecto[boton_recur],sizeof(HBITMAP) * MAXBOTON);
                memcpy(agi_boton_id,_agi_boton_id[boton_recur],sizeof(short) * MAXBOTON);
                memcpy(agi_boton_flag,_agi_boton_flag[boton_recur],MAXBOTON);
                free(_agi_boton[boton_recur]);
                free(_agi_boton_up[boton_recur]);
                free(_agi_boton_down[boton_recur]);
                free(_agi_boton_updefecto[boton_recur]);
                free(_agi_boton_id[boton_recur]);
                free(_agi_boton_flag[boton_recur]);

                for (i = 0;i < MAXBOTON;i++)
                {
                   if (!IsRectEmpty(agi_boton+i))
                   {
					  boton_arriba(i+1);
				   }
				}
             }
             return(0);
          }
          
          
          int crea_boton(HBITMAP dibujoarriba,HBITMAP dibujoabajo,HBITMAP dibujodefecto,short x,short y,short id,char flag)
		  {
             int boton;
             BITMAP bitmap;
             
             GetObject(dibujoarriba, sizeof(BITMAP), (LPSTR) &bitmap);
             
             for (boton = 0;boton < MAXBOTON;boton++)
             {
                if (IsRectEmpty(agi_boton+boton))
                   break;
             }
             /* no se controla el maximo ... */
             if (boton >= MAXBOTON)
             {
                if (stdpan)
                   aviso(0,__Texto[146]);
                return(-1);
             }
             agi_boton_up[boton] = dibujoarriba;
             agi_boton_updefecto[boton] = dibujodefecto;
             agi_boton_down[boton] = dibujoabajo;
             agi_boton_flag[boton] = flag;
             agi_boton[boton].left = x;
             agi_boton[boton].top = y;
             agi_boton[boton].right = x+bitmap.bmWidth;
             agi_boton[boton].bottom = y+bitmap.bmHeight;
             agi_boton_id[boton] = id;
             
             if (stdpan)
                boton_arriba(boton+1);
             return(boton);
		  }
          
          DestruyeBoton(boton)
             int boton;
		 {
                               HDC hdc;
                               int nx,ny,x,y;
                               if (!boton) return(0);
                               boton--;
                               if (IsRectEmpty(agi_boton+boton)) 
							   {
								   return(0);
							   }
                               
                               x = agi_boton[boton].left;
                               nx = agi_boton[boton].right-agi_boton[boton].left;
                               y = agi_boton[boton].top;
                               ny = agi_boton[boton].bottom-agi_boton[boton].top;
                               
                               hOldBitmap = SelectObject(hMemoryDC,agitapiz);
                               BitBlt(stdpan->vhdc,x,y,nx,ny,hMemoryDC,x,y,SRCCOPY);
                               SelectObject(hMemoryDC,hOldBitmap);
                               hOldBitmap = NULL;
                               if (!IsIconic(principal_w))
                               {
                                  if (!NO_PINTAR_AHORA)
                                  {
                                     hdc = GetDC(principal_w);
                                     BitBlt(hdc,principal_rect.left+x,principal_rect.top+y,nx,ny,stdpan->vhdc,x,y,SRCCOPY);
                                     ReleaseDC(principal_w,hdc);
                                  }
                               }
                               
                               
                               /* Atencion esto se repite en la fin_bmp_base */
                               SetRectEmpty(agi_boton+boton);
                               DeleteObject(agi_boton_up[boton]);
                               DeleteObject(agi_boton_down[boton]);
                               if (agi_boton_updefecto[boton])
                                  DeleteObject(agi_boton_updefecto[boton]);
                               agi_boton_up[boton] = NULL;
                               agi_boton_down[boton] = NULL;
                               agi_boton_updefecto[boton] = NULL;
                               agi_boton_id[boton] = 0;
                               agi_boton_flag[boton] = 0;
                               return(1);
          }
                            
                            int boton_arriba(boton)
                               int boton;
                            {
                               HDC hdc;
                               int nx,ny,x,y;
                               if (!boton) return(0);
                               boton--;
                               if (IsRectEmpty(agi_boton+boton)) return(0);
                               x = agi_boton[boton].left;
                               nx = agi_boton[boton].right-agi_boton[boton].left;
                               y = agi_boton[boton].top;
                               ny = agi_boton[boton].bottom-agi_boton[boton].top;
                               if (agi_boton_flag[boton] && agi_boton_updefecto[boton])
                                  hOldBitmap = SelectObject(hMemoryDC,agi_boton_updefecto[boton]);
                               else
                                  hOldBitmap = SelectObject(hMemoryDC,agi_boton_up[boton]);
                               BitBlt(stdpan->vhdc,x,y,nx,ny,hMemoryDC,0,0,SRCCOPY);
                               SelectObject(hMemoryDC,hOldBitmap);
                               hOldBitmap = NULL;
                               if (!IsIconic(principal_w))
                               {
                                  if (!NO_PINTAR_AHORA)
                                  {
                                     hdc = GetDC(principal_w);
                                     BitBlt(hdc,principal_rect.left+x,principal_rect.top+y,nx,ny,stdpan->vhdc,x,y,SRCCOPY);
                                     ReleaseDC(principal_w,hdc);
                                  }
                               }
                               return(1);
                            }
                            
                            int boton_abajo(boton)
                               int boton;
                            {
                               HDC hdc;
                               int nx,ny,x,y;
                               if (!boton) return(0);
                               boton--;
                               if (IsRectEmpty(agi_boton+boton)) return(0);
                               x = agi_boton[boton].left;
                               nx = agi_boton[boton].right-agi_boton[boton].left;
                               y = agi_boton[boton].top;
                               ny = agi_boton[boton].bottom-agi_boton[boton].top;
                               hOldBitmap = SelectObject(hMemoryDC,agi_boton_down[boton]);
                               BitBlt(stdpan->vhdc,x,y,nx,ny,hMemoryDC,0,0,SRCCOPY);
                               SelectObject(hMemoryDC,hOldBitmap);
                               hOldBitmap = NULL;
                               if (!IsIconic(principal_w))
                               {
                                  if (!NO_PINTAR_AHORA)
                                  {
                                     hdc = GetDC(principal_w);
                                     BitBlt(hdc,principal_rect.left+x,principal_rect.top+y,nx,ny,stdpan->vhdc,x,y,SRCCOPY);
                                     ReleaseDC(principal_w,hdc);
                                  }
                               }
                               return(1);
                            }
                            
                            int boton_flag(boton,flag)
                               int boton;
                            char flag;
                            {
                               if (!boton) return(0);
                               boton--;
                               if (IsRectEmpty(agi_boton+boton)) return(0);
                               if (agi_boton_flag[boton] != flag)
                               {
                                  agi_boton_flag[boton] = flag;
                                  boton_arriba(boton+1);
                               }
                               return(0);
                            }
                            
                            int esta_en_icono(x,y)
                               int x,y;
                            {
                               int i,x1,y1,x2,y2;
                               for (i = 0;i <100;i++)
                               {
                                  if (icono_opcion[i])
                                  {
                                     x1 = HIWORD(icono_pos[i]);
                                     x2 = HIWORD(icono_pos[i])+70;
                                     y1 = LOWORD(icono_pos[i]);
                                     y2 = LOWORD(icono_pos[i])+70;
                                     if (x >=x1 && x < x2 &&
                                        y >= y1 && y < y2)
                                     {
                                        return(i+1);
                                     }
                                  }
                               }
                               return(0);
                            }
                            
                            int esta_en_boton(x,y)
                               int x,y;
                            {
                               int i;
                               
                               for (i = 0;i < MAXBOTON;i++)
                               {
                                  if (x >= agi_boton[i].left && x < agi_boton[i].right &&
                                     y >= agi_boton[i].top && y < agi_boton[i].bottom)
                                  {
                                     return(i+1);
                                  }
                               }
                               return(0);
                            }
                            
                            int crea_boton_menusg(int,int,char *,int,short,char);
 
							
        int crea_boton_menusg(linea,x,texto,nx,id,flag)
           int linea,x;
        char *texto;
        int nx;
        short id;
        char flag;
        {
           short ny,y,l = strlen(texto);
           HBITMAP dibujou,dibujod,dibujof,old;
           HDC hdc,hdct;
           HFONT oldfont;
           HFONT menusg_font;
           SIZE ss;
           RECT rt;
           RECT r_texto;
           HBRUSH bru = CreateSolidBrush(RGB(192,192,192));
           HBRUSH bru1 = CreateSolidBrush(RGB(0,0,0));
           HBRUSH bru2 = CreateHatchBrush(HS_FDIAGONAL,RGB(0,0,0));
           HPEN pen2 = CreatePen(PS_SOLID,1,RGB(255,255,255));
           HPEN pen3 = CreatePen(PS_SOLID,1,RGB(128,128,128));
           HPEN penold;
           
           /*
           if (linea == 24)
           {
           y = espacio_arriba+scaracter.cy*23;
           ny = LINEA24_Y;
           menusg_font = agi_fboton;
           }
           else*/
           {
              y = espacio_arriba+scaracter.cy*(linea-1);
              ny = scaracter.cy;
              menusg_font = agi_fboton;
           }
           
           hdc = GetDC(principal_w);
           hdct = CreateCompatibleDC(hdc);
           dibujou = CreateCompatibleBitmap(hdc,nx,ny);
           dibujod = CreateCompatibleBitmap(hdc,nx,ny);
           dibujof = CreateCompatibleBitmap(hdc,nx,ny);
           ReleaseDC(principal_w,hdc);
           hOldBitmap = SelectObject(hMemoryDC,dibujou);
           oldfont = SelectObject(hMemoryDC,menusg_font);
           GetTextExtentPoint(hMemoryDC,texto,l,&ss);
           SetRect(&rt,0,0,nx,ny);
           
           FillRect(hMemoryDC,&rt,bru);
           SetRect(&r_texto,(nx-ss.cx)/2,(ny-ss.cy)/2,(nx-ss.cx)/2+ss.cx,(ny-ss.cy)/2+ss.cy);
           ExtTextOut(hMemoryDC,r_texto.left,r_texto.top,ETO_CLIPPED,&r_texto,texto,l,NULL);
           old = SelectObject(hdct,dibujod);
           BitBlt(hdct,1,1,nx,ny,hMemoryDC,0,0,SRCCOPY);
           FrameRect(hMemoryDC,&rt,bru1);
           FrameRect(hdct,&rt,bru1);
           penold = SelectObject(hMemoryDC,pen2);
           MoveToEx(hMemoryDC,2,2,NULL);
           LineTo(hMemoryDC,nx-2,2);
           
           MoveToEx(hMemoryDC,2,2,NULL);
           LineTo(hMemoryDC,2,ny-2);
           SelectObject(hMemoryDC,pen3);
           MoveToEx(hMemoryDC,3,ny-2,NULL);
           LineTo(hMemoryDC,nx-2,ny-2);
           LineTo(hMemoryDC,nx-2,3);
           SelectObject(hMemoryDC,penold);
           
           SelectObject(hMemoryDC,oldfont);
           
           penold = SelectObject(hdct,pen3);
           MoveToEx(hdct,2,2,NULL);
           LineTo(hdct,nx-2,2);
           MoveToEx(hdct,2,2,NULL);
           LineTo(hdct,2,ny-2);
           SelectObject(hdct,pen2);
           MoveToEx(hdct,3,ny-2,NULL);
           LineTo(hdct,nx-2,ny-2);
           LineTo(hdct,nx-2,3);
           SelectObject(hdct,penold);
           
           SelectObject(hdct,dibujof);
           BitBlt(hdct,0,0,nx,ny,hMemoryDC,0,0,SRCCOPY);
           SetBkColor(hdct,RGB(128,128,128));
           FrameRect(hdct,&r_texto,bru2);
           r_texto.right++;
           r_texto.left--;
           FrameRect(hdct,&r_texto,bru2);
           SetBkColor(hdct,RGB(0,0,0));
           rt.right--;
           rt.left++;
           rt.top++;
           rt.bottom--;
           FrameRect(hdct,&rt,bru1);
           
           SelectObject(hdct,old);
           SelectObject(hMemoryDC,hOldBitmap);
           hOldBitmap = NULL;
           
           
           
           DeleteDC(hdct);
           DeleteObject(bru);
           DeleteObject(bru1);
           DeleteObject(bru2);
           DeleteObject(pen2);
           DeleteObject(pen3);
           return(crea_boton(dibujou,dibujod,dibujof,x,y,id,flag));
        }
                            
                            
void CuadroTab(int pos,RECT *r)
{
   r->top = principal_rect.top+espacio_arriba+(pos - 1)*scaracter.cy;
   r->bottom = r->top + 25;
   r->left = principal_rect.left;
   r->right = principal_rect.left+80*scaracter.cx+EXTRADERECHA;
}

void AreaGrafica(char *tmpx,int pi,int pf)
{
	int fil = (stdpan->vfin)%100 - (stdpan->vini)%100 + 1;
    int h = ((pi/100) - (stdpan->vini)/100) * fil;
    int g = (pi%100) - ((stdpan->vini)%100);
	unsigned char *wvv = stdpan->vv;
	int i,j;

	memcpy(tmpx,(void *)&(stdpan->vhdc),sizeof(HDC));
			  
    for (i = 0;i <= (pf/100)-(pi/100);i++)
	{                                            
	   for (j = 0;j <= (pf%100)-(pi%100);j++)
	   {
		   *((wvv) + h + g + j) = 0;
		   *((wvv) + (stdpan->size) + h + g + j) = 0;
		   *((wvv) + 2 * (stdpan->size) + h + g + j) = 0;
	   }
	   h += fil;
	}
}
                            
void TxtARect(int pi,int pf,RECT *r)
{
   int li = pi/100;
   int ci = pi%100;
   int lf = pf/100;
   int cf = pf%100;

   if (!li)
        r->top = principal_rect.top;
   else
        r->top = principal_rect.top+espacio_arriba+(li - 1)*scaracter.cy;
   if (lf > 25)
        r->bottom = principal_rect.bottom;
   else
        r->bottom = principal_rect.top+espacio_arriba+(lf)*scaracter.cy;
   if (!ci)   
        r->left = principal_rect.left;
   else
        r->left = principal_rect.left+(ci-1)*scaracter.cx;
   if (cf > 80)
        r->right = principal_rect.right;
   else
        r->right = principal_rect.left+(cf)*scaracter.cx;
}
                            
void ConvierteTxtARect(int pi,int pf,RECT *r)
{ /* especial para lineas!!! */
   if (stdpan)
   {
      int i,j,k;
      int x1 = (pf%100);
      int y1 = (pi/100) - 1;
      unsigned char xx;
      unsigned char *s = stdpan->vv + (stdpan->size*2);
      for (j = y1;j < (pf/100);j++)
      {
         for (k = x1;k < 80;k++)
         {
            xx = *(s+j*80+k);
            if ((xx & 128))
            {
               pf = (pf/100*100)+80;
               j = 25;
               break;
            }
         }
      }   
   }
   TxtARect(pi,pf,r);
}
                            
                            extern int fecha_protegida;
                            extern int linea_25;
                            char *cabeza_especial = (char *)0;
                            char *rw_usuario = NULL;
							char *rw_modulo = NULL;
                            int *rw_extranumuser = NULL;
                            
                            
                            int cabecera_v(version,proceso,titulo,_empresa)
                               char *version,*proceso,*titulo,*_empresa;
                            {
                               HDC hdc;
                               HFONT oldfont;
                               RECT espacio;
                               SIZE ss;
                               int cx,cy;
                               char tmp[256];

							   if (en_conexion_terminal)
							   {
								   strcpy(empresa,_empresa);
							   }

                               if (version)
                               {
                                  if (!proceso[0] && !titulo[0] && !_empresa[0])
                                  {
                                     sprintf(tmp,"%s",version);
                                  }
                                  else
                                  {
                                     sprintf(tmp,"%s %s %s",proceso,_empresa,version);
                                  }
                               }
                               else
                               {
                                  sprintf(tmp,"%s %s %s",titulo,proceso,_empresa);
                               }
                               tmp[129] = 0;
                               if (DespachadorExterno)
                               {
								  for(cx = 0;tmp[cx];cx++)
								  {
									tmp[cx] = convierte_caracter(tmp[cx]);
								  }
                                  (*DespachadorExterno)((int)DESPACHO_TITULO,0,tmp);
                                  (*DespachadorExterno)((int)DESPACHO_PANTALLAS,0xffff,proceso);
                               }
                               //SendMessage(GetParent(principal_w),WM_SETTEXT,0,tmp);
                               //SetWindowText(GetParent(GetParent(principal_w)),tmp);
                               
                               return(0);
                               
                               
}

int cabecera(proceso,titulo,empresa)
char *proceso,*titulo,*empresa;
{
   return(cabecera_v((char *)0,proceso,titulo,empresa));
}

static char *mensa_en_cabeza[100][3];
static int recur_mensa = 0;


int recurre_mensaje_cabecera(m)
int m;
{
   int lado;
   int hay;
   HBITMAP hbmp;
   HBITMAP hbmp2;
   if (!m)
			{
      if (recur_mensa < 100)
      {
         hay = 0;
         for (lado = 0;lado < 3;lado++)
         {
            if (mensa_en_cabeza[recur_mensa][lado])
            {
               if (DespachadorExterno)
               {
                  (*DespachadorExterno)((int)DESPACHO_MENSAJE_CABECERA,lado,NULL);
               }
               hay = 1;
            }
         }
         if (hay || recur_mensa)
            recur_mensa++;
      }
			}
   else
			{
      if (recur_mensa)
      {
         recur_mensa--;
         for (lado = 0;lado < 3;lado++)
         {
            if (mensa_en_cabeza[recur_mensa][lado])
            {
               if (DespachadorExterno)
               {
                  (*DespachadorExterno)((int)DESPACHO_MENSAJE_CABECERA,lado,mensa_en_cabeza[recur_mensa][lado]);
               }
            }
         }
      }
			}
   return(0);
}


borra_mensaje_cabecera(l)
int l;
{
			int lado;
         
         for (lado = 0;lado < 3;lado++)
         {
            if (l != -1 && l != lado) continue;
            if (mensa_en_cabeza[recur_mensa][lado])
            {
               free(mensa_en_cabeza[recur_mensa][lado]);
               mensa_en_cabeza[recur_mensa][lado] = NULL;
               if (DespachadorExterno)
               {
                  (*DespachadorExterno)((int)DESPACHO_MENSAJE_CABECERA,lado,NULL);
               }
            }
         }
}

mensaje_en_cabecera(usi,men,lado)
char *usi,*men;
int lado;
{
   char mensa[81];
   
   if (lado >= 0 && lado < 3)
   {
    sprintf(mensa,usi,men);
    borra_mensaje_cabecera(lado);
    mensa_en_cabeza[recur_mensa][lado] = malloc(strlen(mensa)+1);
    strcpy(mensa_en_cabeza[recur_mensa][lado],mensa);  
   
   if (DespachadorExterno)
   {
      (*DespachadorExterno)((int)DESPACHO_MENSAJE_CABECERA,lado,mensa_en_cabeza[recur_mensa][lado]);
   }
  }
}

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
	extern char finalizo[37];

			   if (Pseudo10)
			   {
					  /*char *_fispo,char *__ejspool,char *_rw_user,int *especial_servidor,char *__fispool;*/
					  /* impresora "antigua" */
					  char tmp[512];
					  extern char queimpresora[512];

					  strcpy(tmp,_fispo);
					  quitab(tmp);
					  strcpy(tmp+256,_rw_user);
					  quitab(tmp+256);
					  (*DespachadorExterno)((int)DESPACHO_FUNCIONES_IMPRESION,5,(void *)tmp);
					  if (!tmp[0])
						 return -1;					  

					  if (!strcmp(tmp,"Estandard"))
					  {
						  strcpy(queimpresora,"Estandard");
					      r = 0;
					  }
					  else if (!strcmp(tmp,"Excel Estandard"))
					  {
						  strcpy(queimpresora,"Excel Estandard");
						  r = 2;
					  }
					  else if (!strcmp(tmp,"Pantalla Estandard"))
					  {
						  strcpy(queimpresora,"Pantalla Estandard");
						  r = 3;
					  }
					  else if (!strcmp(tmp,"Crystal Reports"))
					  {						  
						  strcpy(queimpresora,"Crystal Reports");
						  r = 4;
					  }
					  else
					  {	
						  strcpy(_fispo,rw_directorio("dev/imp/"));
						  strcat(_fispo,tmp);
						  if ((r = i_leeimpre(_fispo)) < 0)
						  {
							  return -1;
						  }

						  r = 0;

						  strcpy(_fispo,i_nombre());

						  
						  if (_fispo[0] == '@')
						  {
							*especial_servidor = 1;							
						  }

						  if (_fispo[0] == '*' && strlen(tmp+256))
						  {
							 strcpy(__ejspool,"bin/");					 
							 quitab(_fispo);
							 if (_fispo[1] == 0)
								strcat(__ejspool,"agispool");
							 else
								strcat(__ejspool,_fispo+1);
							 strcpy(_fispo,tmp+256);
							 quitab(_fispo);
							 strcatd(_fispo,rw_directorio("spool/"));
							 strcat(_fispo,".spo");
							 if (__ejspool[0])
							 {
								strcat(__ejspool," ");
								strcat(__ejspool,_fispo);
							 }					 
							 strcpy(__fispool,_fispo);
						  }
						  else
						  {	
							  if (*especial_servidor)
							  {
								 strcpy(__fispool,tmp+256);
								 if (__fispool[0])
								 {
									 strcat(__fispool,".spo");
									 strcpy(__ejspool,"bin/");
									 if (_fispo[2] == 0)
										strcat(__ejspool,"agispool");
									 else
										strcat(__ejspool,_fispo+2);
								 }
								 else
								 {
									 __ejspool[0] = 0;
								 }
							  }
							  else
							  {
								 __fispool[0] = 0;
								 __ejspool[0] = 0;
							  }
							 strcpy(_fispo,i_nombre());
						  }      
						  return r;
					  }
			   }
			   else
			   {

                  r = 1;
                  (*DespachadorExterno)((int)DESPACHO_FUNCIONES_IMPRESION,0,(void *)&r);
				  agi_cola_timer();
			   }
				  
                  if (r == 3)
                  {
                      /* pantalla */
                      _fispo[0] = 1;
                      _fispo[1] = 0;
                      __fispool[0] = 0;
                      __ejspool[0] = 0;                      
                      return 0;
                  } else if (r == 2)
                  {
					 int yaini = 0;
					 i_leeimpre((char *)1);
					 if (_fispo[0] == '?')
					 {
						 yaini = 1;
						 strcpy(inicializo,_fispo+1);
						 inicializo[35] = strlen(inicializo);
						 strcpy(finalizo,"salva");
						 finalizo[35] = strlen(finalizo);
					 }                     
                     strcpy(_fispo,i_nombre());
                     __fispool[0] = 0;
                     __ejspool[0] = 0;
					 if (!yaini)
					 {
						(*DespachadorExterno)((int)DESPACHO_FUNCIONES_IMPRESION,4,(void *)inicializo);
                        inicializo[35] = strlen(inicializo);
					 }
#ifdef RWXARXA
                     if (en_conexion_terminal)
                        *especial_servidor = 0;
#endif
                     return(0);
                  }
                  else if (!r || r == 4)
                  { // usar la impresora windows interna
                        i_leeimpre(NULL);
                        strcpy(_fispo,i_nombre());						

						if (r == 4 && !_fispo[0])
						{
							_fispo[0] = 2;
							_fispo[1] = 0;
							{
								extern unsigned char nombredev[81];
								nombredev[0] = 2;
								nombredev[1] = 0;
							}
						}
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
   for (qj = 1;tmp[qj];qj++)
	   ansi_a_oem(tmp+qj);
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


int parada()
{
   int c;
   recurre_raton(0);
   pprint(2460,__Texto[397]);
   c = que_qjes(leecar(),0);
   pprint(2460,"                   ");
   recurre_raton(1);
   return(c);
}


#define OPCIONES 100
extern int  menusg_virtual;
int *menusg_botones = NULL;
int menusg_w_dato = 0;

int menus(opcion,letras,pos,texto,defecto,lineas)
char *opcion[];    /* texto de las opciones */
char *letras;              /* caracteres de acceso a las opciones */
int pos;                   /* posicion para pintar las opciones */
/* si es 0 se calcula automaticamente */
/* centrado en la linea 24 */
char *texto;               /* texto indicativo */
int defecto;               /* opcion por defecto >0 y <=numero */
/* en caso contrario toma 1 */
int lineas;                /* numero de lineas de menu */
/* solo se usa cuando se llama desde menusg */
/* en caso contrario se pone a cero con lo */
/* que se desactivan pgup y pgdn */
{
   int l=0;     /* suma de las longitudes de cada opcion */
   int l1,f1,c1;
   int i,micol,macol,varlin;
   int a,c,ii;
   int mientras = -1;
   int elegida = ((defecto == 0) ? 1 : defecto);
   int anterior = 0;
   int col[15];
   int numero=strlen(letras);                /* numero de opciones */
   int d = 0;
   int b_opcion[OPCIONES];
   int ojo_al_dato = 0;
   
   for (i = 0;i <(numero+2);i++)
      b_opcion[i] = -1;
   
   strmayus(letras);
   
   l = strlen(texto)+1;
   for (i=0;i<numero;i++) {
      col[i] = l;
      l += strlen(opcion[i]);
      l++;
   }
   
   if (pos != -1) {
      f1 = pos/100;
      c1 = pos%100;
      if (f1 <= 0)
									f1 = 24;  /* fila en pantalla del menu */
      if (c1 <= 0)
									c1 = (80-l)/2+1;     /* columna         "                     */
      for (i=0;i<numero;i++)
									col[i] = f1 * 100 + col[i] + c1;
   }
   else {
      if (strlen(texto)) {
									f1 = (texto[0] - 48) * 10 + (texto[1] - 48);
                           c1 = (texto[2] - 48) * 10 + (texto[3] - 48);
      }
      else {
									f1 = (opcion[0][0] - 48) * 10 + (opcion[0][1] - 48);
                           c1 = (opcion[0][2] - 48) * 10 + (opcion[0][3] - 48);
      }
      d = 4;
      for (i=0;i<numero;i++) {
									pos = opcion[i][4];
                           opcion[i][4] = 0;
                           col[i] = atoi(opcion[i]);
                           opcion[i][4] = pos;
      }
   }
   
   /* con menusg_virtual los haremos permanentes (luego) */
   micol = 80;
   macol = 1;
   varlin = col[0]/100;
   for (i = 0;i < numero;i++)
   {
      if (varlin != (col[i]/100))
      {
									varlin = -1;
                           break;
      }
      if ((col[i]%100) < micol) micol = col[i]%100;
      if ((col[i]%100 + strlen(opcion[i]+d)) > macol) macol = (col[i]%100 + strlen(opcion[i]+d));
   }
   macol -= micol;
   macol++;
   macol /= numero;
   
   if (!menusg_virtual || menusg_virtual == CRETURN || menusg_virtual == -1)
   {
      
      if (!menusg_botones || menusg_botones == (int *)0x00000001L)
      {
						   if (!menusg_virtual || lineas)
                              Bl(f1);
                           if (strlen(texto)) {
                              cursor(f1*100+c1);
                              v_print(texto+d);
                           }
      }
      
      if (varlin == -1)
      {
									if (lineas)      /* hay varias lineas  */
                           {
                              b_opcion[numero] = crea_boton_menusg(col[numero]/100,(col[numero]%100-1)*scaracter.cx,">",scaracter.cx,PGDN,0);
                           }
                           for (i=0;i<numero;i++)
                           {
                              b_opcion[i] = crea_boton_menusg(col[i]/100,(col[i]%100-1)*scaracter.cx,opcion[i]+d,strlen(opcion[i]+d)*scaracter.cx,MID_MENUSG+i,(((i+1) == elegida) ? 1 : 0));
                           }
      }
      else
      {
									if (lineas)      /* hay varias lineas  */
                           {
                              b_opcion[numero] = crea_boton_menusg(col[0]/100,(col[0]%100+numero*macol+1)*scaracter.cx,"->",scaracter.cx,PGDN,0);
                              b_opcion[numero+1] = crea_boton_menusg(col[0]/100,(col[0]%100-3)*scaracter.cx,"<-",scaracter.cx,PGUP,0);
                              for (i=0;i<numero;i++)
                              {
                                 b_opcion[i] = crea_boton_menusg(col[0]/100,(col[0]%100+i*macol-1)*scaracter.cx,opcion[i]+d,macol*scaracter.cx-4,MID_MENUSG+i+menusg_w_dato,(((i+1) == elegida) ? 1 : 0));
                              }
                           }
                           else
                           {
                              if (menusg_botones == (int *)0x00000001L)
                              {
                                 ojo_al_dato = 1;
                              }
                              else
                                 if (menusg_botones)
                                 {
                                    ojo_al_dato = 2;
                                 }
                                 
                                 if (ojo_al_dato == 2)
                                 {
                                    for (i=0;i<numero;i++)
                                    {
                                       b_opcion[i] =  menusg_botones[i];
                                    }
                                 }
                                 else
                                    for (i=0;i<numero;i++)
                                    {
                                       b_opcion[i] = crea_boton_menusg(col[0]/100,(col[0]%100+i*macol-1)*scaracter.cx,opcion[i]+d,macol*scaracter.cx-4,MID_MENUSG+i+menusg_w_dato,(((i+1) == elegida) ? 1 : 0));
                                    }
                           }
      }
   }
   else
   {	   
      if (!menusg_botones || menusg_botones == (int *)0x00000001L)
									aviso(0,__Texto[147]);
      else	  
      {
									ojo_al_dato = 2;
                           for (i=0;i<numero;i++)
                           {
                              b_opcion[i] =  menusg_botones[i];
                           }
      }
   }
   
   anterior = elegida;
   do {
      if (elegida < 1) elegida = numero;
      if (elegida > numero) elegida = 1;
      
      if (anterior != elegida)
      {
									if (anterior > 0)
                           {
                              boton_flag(b_opcion[anterior-1]+1,0);
                              boton_flag(b_opcion[elegida-1]+1,1);
                           }
                           anterior = elegida;
      }
      
      if (menusg_virtual)
      {
						   if (menusg_virtual == -1)
                           {
							   
                              if (!ojo_al_dato)
                              {
                                 aviso(0,__Texto[148]);
                              }
                              break;
							  
                           }
                           c = menusg_virtual;
                           menusg_virtual = -1;
      }
      else
      {
									pulsa_en_espacio = b_opcion[elegida-1]+1;
                           c = leecar();
                           pulsa_en_espacio = -1;
      }
      
      if (c == SOLICITA_POSICION)
      {
									elegida = SOLICITA_POSICION;
                           mientras = 0;
                           break;
      }
      
      if (c >= MID_MENUSG && c < (MID_MENUSG+numero))
      {
									elegida = c - MID_MENUSG + 1;
                           mientras = 0;
                           break;
      }
      
      switch(c)
      {
      case HOME :
         elegida = 1;
         break;
      case END  :
         elegida = numero;
         break;
      case PGUP :
         //if (lineas) {    /* hay varias lineas  */
         elegida = -2;
         mientras = 0;
         //      }
         //else {
         //      anterior = 0;
         //      }
         break;
      case PGDN :
         //if (lineas) {    /* hay varias lineas  */
         elegida = -3;
         mientras = 0;
         //      }
         //else {
         //      anterior = 0;
         //      }
         break;
      case SOLICITA_AVANCE:
      case SOLICITA_RETROCESO:				  				  
         elegida =  c;
         mientras = 0;
         break;
      case ARRIBA :
      case IZQUIERDA :
      case RETROCEDER :
      case BACKSPACE :
      case '-' :
         elegida--;
         break;
      case ABAJO :
      case DERECHA :      
      case '+' :
         elegida++;
         break;
      case ' ':
      case VALIDAR :
      case CRETURN :
         mientras = 2;
         break;
      case ACABAR :
      case ESC :
         elegida = 0;
         mientras = 2;
         break;
      case ABORTAR :
      case CTRLC :
         elegida = -1;
         mientras = 2;
         break;
      default :
         if (c > 96) c -= 32;
         if ((a = enstr(letras,c)) < 0) {
            anterior = 0;
            break;
         }
         else {
            elegida = a+1;
            if (!menusg_virtual)
               mientras = 2;
            break;
         }
      }
   } while(mientras && mientras != 2);
   
   if (ojo_al_dato)
   {
      if (anterior != elegida)
      {
         if (anterior > 0)
         {
            boton_flag(b_opcion[anterior-1]+1,0);
            if (elegida > 0)
               boton_flag(b_opcion[elegida-1]+1,1);
         }
      }
   }
   
   if (ojo_al_dato == 1)
   {   
      menusg_botones = (int *)malloc(sizeof(int) * (numero+1));
      memcpy(menusg_botones,b_opcion,sizeof(int) * numero);
      menusg_botones[numero] = -1;
   }
   else
      if (!ojo_al_dato)
      {
         for (i = 0;i <= (numero+1);i++)
         {
            if (b_opcion[i] != -1)
            {
               DestruyeBoton(b_opcion[i]+1);
            }
         }
         Bl(f1);
      }
      
      return(elegida);
}


/********** Para impresoras **************************/
int FAR PASCAL AbortProc(hPr, Code)
HDC hPr;
int Code;
{
   MSG msg;
   
   if (!hAbortDlgWnd)
      return(TRUE);
   
   while (!bAbort && PeekMessage((LPMSG)&msg, (HWND)0, 0, 0,TRUE))
      if (!IsDialogMessage(hAbortDlgWnd, &msg)) {
         TranslateMessage(&msg);
         DispatchMessage(&msg);
      }
      
      return (!bAbort);
}

int FAR PASCAL AbortDlg(hDlg, msg, wParam, lParam)
HWND hDlg;
unsigned msg;
WORD wParam;
LONG lParam;
{
   switch(msg) {
			case WM_COMMAND:
            return (bAbort = TRUE);
         case WM_INITDIALOG:
            SetFocus(GetDlgItem(hDlg,IDCANCEL));
            return (TRUE);
			}
   return (FALSE);
}
/***************************************/


BOOL CALLBACK AGIMPRESORA(HWND hWnd, UINT message,
                          WPARAM wParam, LPARAM lParam)
{
			char tmp[81];
         char opc[81];
         FILE *file;
         DWORD dwIndex;
         int i;
         int defe;
         static char *impres[200];
         
         
         switch (message)
         {
         case WM_SYSCOMMAND:
            //SendMessage(principal_w, message, wParam, lParam);
            break;
         case WM_CHAR:
            break;
         case WM_COMMAND:
#ifdef FGV32
            if (LOWORD(wParam) == IDOK)
#else
               if (wParam == IDOK)
#endif
               {
                  for (i = 0;i < 200;i++)
                  {
                     if (impres[i]) {free(impres[i]);impres[i] = NULL;}
                  }
                  GetDlgItemText(hWnd,AGIDINPUT,(LPSTR)tmp,30);
                  quita_blancos(tmp);
                  if (fispo_[0] == '*' && strlen(tmp))
                  {
                     strcpy(_ejspool_,"bin/");					 
                     quitab(fispo_);
                     if (fispo_[1] == 0)
                        strcat(_ejspool_,"agispool");
                     else
                        strcat(_ejspool_,fispo_+1);
                     strcpy(fispo_,tmp);
                     quitab(fispo_);
                     strcatd(fispo_,rw_directorio("spool/"));
                     strcat(fispo_,".spo");
                     if (_ejspool_[0])
                     {
                        strcat(_ejspool_," ");
                        strcat(_ejspool_,fispo_);
                     }					 
                     strcpy(_fispool_,fispo_);
                  }
                  else
                  {
                     fispo_ = fispo__;
                     _fispool_[0] = 0;
                     _ejspool_[0] = 0;
                     strcpy(fispo_,i_nombre());
                  }				   
                  EndDialog(hWnd,0);
                  SetFocus(principal_w);
               }
               else
#ifdef FGV32
                  if (LOWORD(wParam) == IDCANCEL)
#else
                     if (wParam == IDCANCEL)
#endif
                     {
                        for (i = 0;i < 200;i++)
                        {
                           if (impres[i]) {free(impres[i]);impres[i] = NULL;}
                        }
                        EndDialog(hWnd,-1);
                        SetFocus(principal_w);
                     }
                     else
#ifdef FGV32
                        if (LOWORD(wParam) == AGIDLISTA)
#else
                           if (wParam == AGIDLISTA)
#endif
                           {
#ifdef FGV32
                              if (HIWORD(wParam) ==  CBN_SELCHANGE)
#else
                                 if (HIWORD(lParam) ==  CBN_SELCHANGE)
#endif
                                 {
                                    dwIndex = SendDlgItemMessage(hWnd,AGIDLISTA,CB_GETCURSEL, 0, 0);
                                    strcpy(tmp,rw_directorio("dev/imp/"));
                                    strcat(tmp,impres[dwIndex]);
                                    if (i_leeimpre(tmp)) {
                                       fispo_ = fispo__;
                                       strcpy(fispo_,i_nombre());
                                       if (fispo_[0] == '@') 
                                       {
                                          _especial_servidor = 1;
                                          fispo_++;
                                       }
                                       else
                                          _especial_servidor = 0;
                                       if (fispo_[0] == '*')
                                       {
                                          SendDlgItemMessage(hWnd,AGIDLISTA,CB_LIMITTEXT,8,0L);
                                          SetDlgItemText(hWnd,AGIDTEXTO,(LPSTR)"Fichero Spool ...:");
                                          SetDlgItemText(hWnd,AGIDINPUT,(LPSTR)rw_user_);
                                       }
                                       else
                                       {
                                          SetDlgItemText(hWnd,AGIDINPUT,(LPSTR)"");
                                          SetDlgItemText(hWnd,AGIDTEXTO,(LPSTR)"");
                                          SendDlgItemMessage(hWnd,AGIDLISTA,CB_LIMITTEXT,0,0L);
                                       }
                                    }				  
                                 }
                           }
                           else
#ifdef FGV32
                              if (LOWORD(wParam) == AGIDINPUT)
#else
                                 if (wParam == AGIDINPUT)
#endif
                                 {
#ifdef FGV32
                                    if (HIWORD(wParam) == CBN_EDITCHANGE ||
                                       HIWORD(wParam) == CBN_EDITUPDATE)
#else
                                       if (HIWORD(lParam) == CBN_EDITCHANGE ||
                                          HIWORD(lParam) == CBN_EDITUPDATE)
#endif
                                       {
                                       }
                                 }
                                 break;
    case WM_INITDIALOG:
       if (!lParam)
       {
          quitab(fispo_);
          SetFocus(hWnd);
          strcpy(tmp,rw_directorio("dev/imp/imp.d"));
          for (i = 0;i < 200;i++)
             impres[i] = NULL;
          file = fopen(tmp,"r");
          defe = 0;
          if (file)
          {
             while(fgets(opc,80,file))
             {
                quitan(opc);
                quitab(opc);
                if (!fgets(tmp,80,file)) break;
                quitan(tmp);
                dwIndex = SendDlgItemMessage(hWnd,AGIDLISTA,CB_ADDSTRING,0,(LPARAM)tmp);
                impres[dwIndex] = malloc(strlen(opc)+1);
                strcpy(impres[dwIndex],opc);
                if (!strcmp(fispo_,opc))
                   defe = (int)dwIndex;
             }
             fclose(file);
             if (!impres[0])
             {
                EndDialog(hWnd,-1);
                SetFocus(principal_w);
             }
             else
             {
                SendDlgItemMessage(hWnd,AGIDLISTA,CB_SETCURSEL,defe,0L);
                SendDlgItemMessage(hWnd,AGIDLISTA,CB_SHOWDROPDOWN,TRUE,0L);
                strcpy(tmp,rw_directorio("dev/imp/"));
                strcat(tmp,impres[defe]);
                if (i_leeimpre(tmp) >= 0) {
                   fispo_ = fispo__;
                   strcpy(fispo_,i_nombre());
                   if (fispo_[0] == '@') 
                   {
                      _especial_servidor = 1;
                      fispo_++;
                   }
                   else
                      _especial_servidor = 0;
                   
                   if (fispo_[0] == '*')
                   {
                      SendDlgItemMessage(hWnd,AGIDLISTA,CB_LIMITTEXT,8,0L);
                      SetDlgItemText(hWnd,AGIDTEXTO,(LPSTR)"Fichero Spool ...:");
                      SetDlgItemText(hWnd,AGIDINPUT,(LPSTR)rw_user_);
                   }
                   else
                   {
                      SetDlgItemText(hWnd,AGIDINPUT,(LPSTR)"");
                      SetDlgItemText(hWnd,AGIDTEXTO,(LPSTR)"");
                      SendDlgItemMessage(hWnd,AGIDLISTA,CB_LIMITTEXT,0,0L);
                   }
                }			  
             }
          }
       }
       else
       {
          EndDialog(hWnd,-1);
          SetFocus(principal_w);
       }
       break;
    default:
       return FALSE;
    }
    
    return 0;
}


BOOL CALLBACK AGIDIALOGO(HWND hWnd, UINT message,
                         WPARAM wParam, LPARAM lParam)
{
   char tmp[81];
   
   switch (message)
   {
   case WM_SYSCOMMAND:	  
      return FALSE;
      SendMessage(principal_w, message, wParam, lParam);
      break;
   case WM_CHAR:
      //sprintf(tmp,"%d",wParam);
      //mensaje_especial(tmp);
      break;
   case WM_COMMAND:
      //sprintf(tmp,"%d",wParam);
      //mensaje_especial(tmp);
#ifdef FGV32
      EndDialog(hWnd,LOWORD(wParam));
#else
      EndDialog(hWnd,wParam);
#endif
      SetFocus(principal_w);
      break;
   case WM_INITDIALOG:
      tmp[0] = 'S';
      if (lParam)
      {
         tmp[0] = *((char *)lParam);
         SetDlgItemText(hWnd,AGIDTEXTO,((LPSTR)lParam)+1);
      }
      SetFocus(hWnd);
      if (tmp[0])
      {
         if (tmp[0] == 'S' || tmp[0] == 's')
         {
            PostMessage(GetDlgItem (hWnd, AGIDSI), BM_SETSTYLE,(WPARAM)BS_DEFPUSHBUTTON, TRUE);
            SetFocus (GetDlgItem (hWnd, AGIDSI));
            return(0);
         }
         else
            if (tmp[0] == 'N' || tmp[0] == 'n')
            {
               PostMessage(GetDlgItem (hWnd, AGIDNO), BM_SETSTYLE,(WPARAM)BS_DEFPUSHBUTTON, TRUE);
               SetFocus (GetDlgItem (hWnd, AGIDNO));
               return(0);
            }
      }
      return(-1);
      break;
   default:
      //sprintf(tmp,"[%d]->%d",message,wParam);
      //mensaje_especial(tmp);
      return FALSE;
   }
   
   return FALSE;
}


BOOL CALLBACK AGINPUT_(HWND hWnd, UINT message,
                       WPARAM wParam, LPARAM lParam)
{
   static char *tmp = NULL;
   
   switch (message)
   {
   case WM_SYSCOMMAND:
      //SendMessage(principal_w, message, wParam, lParam);
      break;
   case WM_CHAR:
      break;
   case WM_COMMAND:
#ifdef FGV32
      if (LOWORD(wParam) != AGIDINPUT)
#else
         if (wParam != AGIDINPUT)
#endif
         {
#ifdef FGV32
            if (LOWORD(wParam) != IDCANCEL && tmp)
#else
               if (wParam != IDCANCEL && tmp)
#endif
               {
                  GetDlgItemText(hWnd,AGIDINPUT,(LPSTR)tmp,80);
               }
#ifdef FGV32
               EndDialog(hWnd,LOWORD(wParam));
#else
               EndDialog(hWnd,wParam);
#endif
               SetFocus(principal_w);
         }
         break;
   case WM_INITDIALOG:
      if (lParam)
      {
         tmp = (LPSTR)lParam;
         SetDlgItemText(hWnd,AGIDTEXTO,(LPSTR)tmp);
         SetFocus(hWnd);
         SetFocus (GetDlgItem (hWnd, AGIDINPUT));
      }
      else
      {
         tmp = NULL;
         EndDialog(hWnd,-1);
         SetFocus(principal_w);
      }
      
      break;
   default:
      //sprintf(tmp,"[%d]->%d",message,wParam);
      //mensaje_especial(tmp);
      return FALSE;
   }
   
   return FALSE;
}


BOOL CALLBACK _WACTIVACION(HWND hWnd, UINT message,WPARAM wParam, LPARAM lParam)
{
   
   
   
   switch (message)
   {
   case WM_SYSCOMMAND:
      //SendMessage(principal_w, message, wParam, lParam);
      break;
   case WM_CHAR:
      //sprintf(tmp,"%d",wParam);
      //mensaje_especial(tmp);
      break;
   case WM_COMMAND:
      //sprintf(tmp,"%d",wParam);
      //mensaje_especial(tmp);
      switch(wParam)
      {
      case IDOK:
         if (_res)
         {
            GetDlgItemText(hWnd,IDC_ACTIVACION,(LPSTR)_res,50);
         }
      case IDCANCEL:
#ifdef FGV32
         EndDialog(hWnd,LOWORD(wParam));
#else
         EndDialog(hWnd,wParam);
#endif
         SetFocus(principal_w);
         break;
      }	 
      break;
      case WM_INITDIALOG:
         if (_num1)
         {
            SetDlgItemText(hWnd,IDC_IDENTIFICA,(LPSTR)_num1);
         }
         if (_num2)
         {
            SetDlgItemText(hWnd,IDC_NUMERO,(LPSTR)_num2);
         }		         
		 
		 if (!telefonos_claves)
			 SetDlgItemText(hWnd,IDC_TELEFONOS,(LPSTR)"902100177 / 965337743");
		 else			 
			 SetDlgItemText(hWnd,IDC_TELEFONOS,(LPSTR)telefonos_claves);
         
         SetFocus(hWnd);
         return(-1);
         break;
      default:
         //sprintf(tmp,"[%d]->%d",message,wParam);
         //mensaje_especial(tmp);
         return FALSE;
   }
   
   return FALSE;
}

int dialogo_activacion(char *num1,char *num2,char *res)
{
   int r;
   _num1 = num1;
   _num2 = num2;
   _res  = res;
   if (!abandonarelbarco)
   {
		r = DialogBoxParam(hInst,"WACTIVACION", principal_w /*GetParent(principal_w)*/, _WACTIVACION ,(LPARAM)NULL);
   }
   _num1 = NULL;
   _num2 = NULL;
   _res  = NULL;
   return(r);
}

void cmAcerca(void)
{
   DialogBoxParam(hInst, "Acerca", principal_w /*GetParent(principal_w)*/, agilpDlgProcx,(LPARAM)NULL);
}


DWORD PASCAL lread (fh, pv, ul)
int           fh;
VOID far      *pv;
DWORD         ul;
{
   DWORD     ulT = ul;
#ifdef FGV32
   BYTE *hp = pv;
#else
   BYTE huge *hp = pv;
#endif
   
   while (ul > (DWORD)MAXREAD) {
      if (_lread(fh, (LPSTR)hp, (WORD)MAXREAD) != MAXREAD)
         return 0;
      ul -= MAXREAD;
      hp += MAXREAD;
   }
   if (_lread(fh, (LPSTR)hp, (WORD)ul) != (WORD)ul)
      return 0;
   return ulT;
}


DWORD PASCAL lwrite (fh, pv, ul)
int          fh;
VOID FAR     *pv;
DWORD        ul;
{
   DWORD     ulT = ul;
#ifdef FGV32
   BYTE *hp = pv;
#else
   BYTE huge *hp = pv;
#endif
   
   while (ul > MAXREAD) {
      if (_lwrite(fh, (LPSTR)hp, (WORD)MAXREAD) != MAXREAD)
         return 0;
      ul -= MAXREAD;
      hp += MAXREAD;
			}
   if (_lwrite(fh, (LPSTR)hp, (WORD)ul) != (WORD)ul)
      return 0;
   return ulT;
}



WORD DibNumColors(
                  VOID FAR * pv)          // Pointer to the bitmap info header structure
{
#define lpbi ((LPBITMAPINFOHEADER)pv)
#define lpbc ((LPBITMAPCOREHEADER)pv)
			
			int nBitCount;
         
         /* With a BITMAPINFOHEADER structure, the number of palette entries
         * is in biClrUsed; otherwise, the count depends on the number of bits
         * per pixel.
         */
         if (lpbi->biSize != sizeof(BITMAPCOREHEADER))
         {
            if(lpbi->biClrUsed != 0)
               return (WORD)lpbi->biClrUsed;
            
            nBitCount = lpbi->biBitCount;
         }
         else
         {
            nBitCount = lpbc->bcBitCount;
         }
         
         switch (nBitCount)
         {
         case 1:
            return 2;
            
         case 4:
            return 16;
            
         case 8:
            return 256;
            
         default:
            return 0;
         }
         
#undef lpbi
#undef lpbc
}


WORD PaletteSize(
                 VOID FAR * pv)          // Pointer to the bitmap info header structure
{
#define lpbi ((LPBITMAPINFOHEADER)pv)
			
			WORD    NumColors;
         
         NumColors = DibNumColors(lpbi);
         
         if (lpbi->biSize == sizeof(BITMAPCOREHEADER))
            return NumColors * sizeof(RGBTRIPLE);
         else
            return NumColors * sizeof(RGBQUAD);
         
#undef lpbi
}


HPALETTE CreateBIPalette(
                         LPBITMAPINFOHEADER lpbi)        // Info header containing color table
{
			LOGPALETTE          *pPal;
         HPALETTE            hpal = NULL;
         WORD                nNumColors;
         BYTE                red;
         BYTE                green;
         BYTE                blue;
         WORD                i;
         RGBQUAD        FAR *pRgb;
         BOOL                fCoreHeader;
         
         if (!lpbi)
            return NULL;
         
         fCoreHeader = (lpbi->biSize == sizeof(BITMAPCOREHEADER));
         
         pRgb = (RGBQUAD FAR *)((LPSTR)lpbi + (WORD)lpbi->biSize);
         nNumColors = DibNumColors(lpbi);
         
         if (nNumColors)
         {
         /* Allocate a new color table, copy the palette entries to
         * it, and create the palette.
            */
            pPal = (LOGPALETTE *)malloc(sizeof(LOGPALETTE) + nNumColors * sizeof(PALETTEENTRY));
            
            if (!pPal)
               goto exit;
            
            pPal->palNumEntries = nNumColors;
            pPal->palVersion    = PALVERSION;
            
            for (i = 0; i < nNumColors; i++)
            {
               pPal->palPalEntry[i].peRed   = pRgb->rgbRed;
               pPal->palPalEntry[i].peGreen = pRgb->rgbGreen;
               pPal->palPalEntry[i].peBlue  = pRgb->rgbBlue;
               pPal->palPalEntry[i].peFlags = (BYTE)0;
               
               if (fCoreHeader)
                  ((LPSTR)pRgb) += sizeof(RGBTRIPLE) ;
               else
                  pRgb++;
            }
            
            hpal = CreatePalette(pPal);
            free(pPal);
         }
         else if (lpbi->biBitCount == 24)
         {
         /* A DIB with a bit count of 24 has no color table entries. Set
         * the number of entries to the maximum (256).
            */
            nNumColors = MAXPALETTE;
            pPal = (LOGPALETTE *)malloc(sizeof(LOGPALETTE) + nNumColors * sizeof(PALETTEENTRY));
            
            if (!pPal)
               goto exit;
            
            pPal->palNumEntries = nNumColors;
            pPal->palVersion    = PALVERSION;
            
            red = green = blue = 0;
            
            /* Generate 256 (8 * 8 * 4) RGB combinations to fill the palette
            * entries.
            */
            for (i = 0; i < pPal->palNumEntries; i++)
            {
               pPal->palPalEntry[i].peRed   = red;
               pPal->palPalEntry[i].peGreen = green;
               pPal->palPalEntry[i].peBlue  = blue;
               pPal->palPalEntry[i].peFlags = (BYTE)0;
               
               if (!(red += 32))
                  if (!(green += 32))
                     blue += 64;
            }
            
            hpal = CreatePalette(pPal);
            free(pPal);
         }
         
exit:
         return hpal;
}

HANDLE ReadDibBitmapInfo(
                         HFILE fh)                             // Handle to open DIB file
{
			DWORD     off;
         HANDLE    hbi = NULL;
         int       size;
         int       i;
         WORD      nNumColors;
         
         RGBQUAD FAR       *pRgb;
         BITMAPINFOHEADER   bi;
         BITMAPCOREHEADER   bc;
         LPBITMAPINFOHEADER lpbi;
         BITMAPFILEHEADER   bf;
         
         if (fh == HFILE_ERROR)
            return NULL;
         
         off = _llseek(fh,0L,SEEK_CUR);
         
         /* Read bitmap file header.
         */
         if (sizeof(bf) != _lread(fh,(LPSTR)&bf,sizeof(bf)))
            return FALSE;
         
            /*
            * Do we have a RC HEADER?
         */
         if (!ISDIB(bf.bfType))
         {
            bf.bfOffBits = 0L;
            _llseek(fh,off,SEEK_SET);
         }
         
         /* Read bitmap info header.
         */
         if (sizeof(bi) != _lread(fh,(LPSTR)&bi,sizeof(bi)))
            return FALSE;
         
         nNumColors = DibNumColors(&bi);
         
         /* Determine type of bitmap info (BITMAPINFOHEADER or BITMAPCOREHEADER).
         */
         switch (size = (int)bi.biSize)
         {
         case sizeof(BITMAPINFOHEADER):
            break;
            
         case sizeof(BITMAPCOREHEADER):
            
         /* Fill in the missing fields and seek back to the start of
         * the color table.
            */
            bc = *(BITMAPCOREHEADER*)&bi;
            bi.biSize               = sizeof(BITMAPINFOHEADER);
            bi.biWidth              = (DWORD)bc.bcWidth;
            bi.biHeight             = (DWORD)bc.bcHeight;
            bi.biPlanes             =  (WORD)bc.bcPlanes;
            bi.biBitCount           =  (WORD)bc.bcBitCount;
            bi.biCompression        = BI_RGB;
            bi.biSizeImage          = 0;
            bi.biXPelsPerMeter      = 0;
            bi.biYPelsPerMeter      = 0;
            bi.biClrUsed            = nNumColors;
            bi.biClrImportant       = nNumColors;
            
            _llseek(fh, (LONG)sizeof(BITMAPCOREHEADER)-
               sizeof(BITMAPINFOHEADER), SEEK_CUR);
            break;
            
         default:
            return NULL;       /* not a DIB */
         }
         
         /* Fill in some default values.
         */
         if(bi.biSizeImage == 0)
         {
            bi.biSizeImage = WIDTHBYTES((DWORD)bi.biWidth * bi.biBitCount) *
               bi.biHeight;
         }
         
         if(bi.biClrUsed == 0)
         {
            bi.biClrUsed = DibNumColors(&bi);
         }
         
         /* Allocate space for the bitmap info header and color table.
         */
         hbi = GlobalAlloc(GMEM_MOVEABLE,
            (LONG)bi.biSize + nNumColors * sizeof(RGBQUAD));
         if (!hbi)
            return NULL;
         
         lpbi = (VOID FAR *)GlobalLock(hbi);
         
         *lpbi = bi;                     // Copy the bitmap header information.
         
         /* Read the color table, if it exists. */
         
         if(nNumColors)
         {
            pRgb = (RGBQUAD FAR *)((LPSTR)lpbi + bi.biSize);
            if (size == sizeof(BITMAPCOREHEADER))
            {
            /* Read an old color table (with 3-byte entries) and convert
            * to the new color table format (with 4-byte entries).
               */
               _lread(fh,(LPSTR)pRgb,nNumColors * sizeof(RGBTRIPLE));
               
               for (i=nNumColors-1; i>=0; i--)
               {
                  RGBQUAD rgb;
                  
                  rgb.rgbRed      = ((RGBTRIPLE FAR *)pRgb)[i].rgbtRed;
                  rgb.rgbBlue     = ((RGBTRIPLE FAR *)pRgb)[i].rgbtBlue;
                  rgb.rgbGreen    = ((RGBTRIPLE FAR *)pRgb)[i].rgbtGreen;
                  rgb.rgbReserved = (BYTE)0;
                  
                  pRgb[i] = rgb;
               }
            }
            else
            {
            /* Read a new color table.
               */
               _lread(fh,(LPSTR)pRgb,nNumColors * sizeof(RGBQUAD));
            }
         }
         
         /* Seek to the start of the bitmap data.
         */
         if (bf.bfOffBits != 0L)
            _llseek(fh, off + bf.bfOffBits, SEEK_SET);
         
         GlobalUnlock(hbi);
         return hbi;
}


BOOL DibInfo(
             HANDLE hbi,                     // Handle to bitmap
             LPBITMAPINFOHEADER lpbi)        // Info header structure to fill in
{
   if(!hbi)
      return FALSE;
   
   *lpbi = *(LPBITMAPINFOHEADER)GlobalLock(hbi);
   GlobalUnlock(hbi);
   
   if(lpbi->biSize == sizeof(BITMAPCOREHEADER))
   {
      BITMAPCOREHEADER bc;
      
      bc = *(LPBITMAPCOREHEADER)lpbi;
      
      lpbi->biSize          = sizeof(BITMAPINFOHEADER);
      lpbi->biWidth         = (DWORD)bc.bcWidth;
      lpbi->biHeight        = (DWORD)bc.bcHeight;
      lpbi->biPlanes        =  (WORD)bc.bcPlanes;
      lpbi->biBitCount      =  (WORD)bc.bcBitCount;
      lpbi->biCompression   = BI_RGB;
      lpbi->biSizeImage     = 0;
      lpbi->biXPelsPerMeter = 0;
      lpbi->biYPelsPerMeter = 0;
      lpbi->biClrUsed       = 0;
      lpbi->biClrImportant  = 0;
   }
   
   /* Fill in the default fields
	  */
   if (lpbi->biSize != sizeof(BITMAPCOREHEADER))
   {
      if (lpbi->biSizeImage == 0L)
         lpbi->biSizeImage = WIDTHBYTES(lpbi->biWidth*lpbi->biBitCount) *
         lpbi->biHeight;
      
      if (lpbi->biClrUsed == 0L)
         lpbi->biClrUsed = DibNumColors(lpbi);
   }
   return TRUE;
}

HPALETTE CreateDibPalette(HANDLE hbi)
{
   HPALETTE hpal;
   
   if (!hbi)
      return NULL;
   
   hpal = CreateBIPalette((LPBITMAPINFOHEADER)GlobalLock(hbi));
   GlobalUnlock(hbi);
   return hpal;
}

void FreeDib()
{
   if (hbiCurrent)
   {
      GlobalFree(hbiCurrent);
      hbiCurrent = NULL;
   }
}


HANDLE  Otro_ReadDIB(FileName)
LPSTR FileName;
{
   unsigned           fh;
   HANDLE              h;
   DWORD               dwLen = 0;
   DWORD               dwBits;
   LPBITMAPINFOHEADER lpbi;
   WORD FAR *         pw;
   int                i;
   BITMAPINFOHEADER   bi;
   OFSTRUCT           of;
   HDC hdc;
   HANDLE     hbi;
   DWORD      dwOff;
   
   
   /* Open the file and get a handle to it's BITMAPINFO */
   
   fh = OpenFile (FileName, (LPOFSTRUCT)&of, OF_READ);
   if (fh == -1) {
      return NULL;
   }
   hbi = ReadDibBitmapInfo(fh);
   dwOff = _llseek(fh, 0L, SEEK_CUR);
   
   DibInfo(hbi,&bi);
   
   /* Calculate the memory needed to hold the DIB */
   dwBits = bi.biSizeImage;
   dwLen  = bi.biSize + (DWORD)PaletteSize (&bi) + dwBits;
   
   /* Try to increase the size of the bitmap info. buffer to hold the DIB */
   h = GlobalReAlloc(hbi, dwLen, GHND);
   if (!h){
      GlobalFree(hbi);
      hbi = NULL;
   }
   else
   {
      hbi = h;
      lpbi = (VOID FAR *)GlobalLock(hbi);
      lread(fh, (LPSTR)lpbi + (WORD)lpbi->biSize + PaletteSize(lpbi), dwBits);
      GlobalUnlock(hbi);
   }
   
   _lclose (fh);
   
   return hbi;
}



int ReadDIB(FileName)
LPSTR FileName;
{
   unsigned           fh;
   HANDLE              h;
   DWORD               dwLen = 0;
   DWORD               dwBits;
   LPBITMAPINFOHEADER lpbi;
   WORD FAR *         pw;
   int                i;
   BITMAPINFOHEADER   bi;
   OFSTRUCT           of;
   HDC hdc;
   
   
   /* Open the file and get a handle to it's BITMAPINFO */
   
   fh = OpenFile (FileName, (LPOFSTRUCT)&of, OF_READ);
   if (fh == -1) {
      return FALSE;
			}
   hbiCurrent = ReadDibBitmapInfo(fh);
   dwOffset = _llseek(fh, 0L, SEEK_CUR);
   
   DibInfo(hbiCurrent,&bi);
   
   /* Calculate the memory needed to hold the DIB */
   dwBits = bi.biSizeImage;
   dwLen  = bi.biSize + (DWORD)PaletteSize (&bi) + dwBits;
   
   /* Try to increase the size of the bitmap info. buffer to hold the DIB */
   h = GlobalReAlloc(hbiCurrent, dwLen, GHND);
   if (!h){
      GlobalFree(hbiCurrent);
      hbiCurrent = NULL;
			}
   else
			{
      hbiCurrent = h;
      lpbi = (VOID FAR *)GlobalLock(hbiCurrent);
      lread(fh, (LPSTR)lpbi + (WORD)lpbi->biSize + PaletteSize(lpbi), dwBits);
      GlobalUnlock(hbiCurrent);
			}
   
   _lclose (fh);
   
   if (hbiCurrent == NULL) {
      aviso(0,"Realmente esto no es un DIB legitimo!");
      return FALSE;
			}
   
   
   /* Set up the palette */
   if (hpalCurrent)
			{
      if (OldPalette)
      {
         hdc = GetDC(principal_w);
         SelectPalette(hdc, OldPalette, FALSE);
         RealizePalette(hdc);
         ReleaseDC(principal_w,hdc);
         OldPalette = NULL;
         if (stdpan->Oldpalette)
         {
            SelectPalette(stdpan->vhdc,stdpan->Oldpalette, FALSE);
            stdpan->Oldpalette = NULL;
         }
      }
      DeleteObject(hpalCurrent);
			}
   hpalCurrent = CreateDibPalette(hbiCurrent);
   if (hpalCurrent == NULL) {
      aviso(0,__Texto[149]);
      return FALSE;
			}
   
         /*  Convert the DIB color table to palette relative indexes, so
         *  SetDIBits() and SetDIBitsToDevice() can avoid color matching.
         *  We can do this because the palette we realize is identical
         *  to the color table of the bitmap, ie the indexes match 1 to 1
         *
         *  Now that the DIB color table is palette indexes not RGB values
         *  we must use DIB_PAL_COLORS as the wUsage parameter to SetDIBits()
   */
   fPalColors = FALSE;
   
   lpbi = (VOID FAR *)GlobalLock(hbiCurrent);
   if (lpbi->biBitCount != 24) {
      fPalColors = TRUE;
      
      pw = (WORD FAR *)((LPSTR)lpbi + lpbi->biSize);
      
      for (i=0; i<(int)lpbi->biClrUsed; i++)
         *pw++ = (WORD)i;
			}
   GlobalUnlock(hbiCurrent);
   
   bLegitDraw = TRUE;
   
   return TRUE;
}

BOOL StretchDibBlt(
                   HDC hdc,               // Destination device context
                   int x, int y,          // Destination rect origin
                   int dx, int dy,        // Destination rect extents
                   HANDLE hdib,           // Bitmap to display
                   int xSrc, int ySrc,    // Source rect origin
                   int dxSrc, int dySrc,  // Source rect extents
                   LONG rop,              // Raster operation
                   WORD wUsage,int directo)           // Pal usage DIB_PAL_COLORS or DIB_RGB_COLORS
{
			LPBITMAPINFOHEADER lpbi;
         LPSTR        pBuf;
         BOOL         f;
         
         if (!hdib)
            return PatBlt(hdc,x,y,dx,dy,PATCOPY);
         
         if (wUsage == 0)
         {
            if (fPalColors)
               wUsage = DIB_PAL_COLORS;
            else
               wUsage = DIB_RGB_COLORS;
         }
         
         lpbi = (VOID FAR *)GlobalLock(hdib);
         
         if (!lpbi)
            return FALSE;
         
         if (dxSrc == -1 && dySrc == -1)
         {
            if (lpbi->biSize == sizeof(BITMAPCOREHEADER))
            {
               dxSrc = ((LPBITMAPCOREHEADER)lpbi)->bcWidth;
               dySrc = ((LPBITMAPCOREHEADER)lpbi)->bcHeight;
            }
            else
            {
               dxSrc = (int)lpbi->biWidth;
               dySrc = (int)lpbi->biHeight;
            }
         }
         
         if (dx == -1 && dy == -1)
         {
            dx = dxSrc;
            dy = dySrc;
         }
         
         pBuf = (LPSTR)lpbi + (WORD)lpbi->biSize + PaletteSize(lpbi);
         
#ifdef FGV32
         SetStretchBltMode(hdc,COLORONCOLOR);
#else
         SetStretchBltMode(hdc,STRETCH_DELETESCANS);
#endif
         /*x = x / GetDeviceCaps(hdc,LOGPIXELSX);
         y = y / GetDeviceCaps(hdc,LOGPIXELSY);
         dx = dx / GetDeviceCaps(hdc,LOGPIXELSX);
         dy = dy / GetDeviceCaps(hdc,LOGPIXELSY);*/
         
#ifdef FGV32
#ifdef RWXARXA
         if (nt_version >= 0x80000000 && en_conexion_terminal)              
            Sleep(500);
#endif
#endif
    if (directo)
    {
        f = SetDIBitsToDevice(
		    hdc,
		    x,y,
            dxSrc,dySrc,
		    /*dx,dy,*/
		    xSrc,ySrc,
		    0,dySrc,
		    pBuf,
		    (LPBITMAPINFO)lpbi,
		    wUsage);
		    
    }
    else
    {


         f = StretchDIBits (
            hdc,
            x,y,
            dx,dy,
            xSrc,ySrc,
            dxSrc,dySrc,
            pBuf,
            (LPBITMAPINFO)lpbi,
            wUsage,
            rop);

     }
         
         GlobalUnlock(hdib);
         return f;
}



int PonLogo()
{
			HANDLE mi_bmp;
         HBITMAP old;
         char tmp[128];
         
         
         if (!stdpan) return -1;
         
		 strcpy(tmp,rw_directorio("logo.bmp"));

         mi_bmp = LoadImage(hInst,tmp,IMAGE_BITMAP,0,0,LR_DEFAULTSIZE | LR_LOADFROMFILE );
         /*
         if (!mi_bmp)
         mi_bmp = LoadImage(hInst,"DIAGRAM",IMAGE_BITMAP,0,0,LR_DEFAULTSIZE);
         */
         if (mi_bmp)
         {
            int nx,ny,ony,onx;
            int tbx = 800;
            int tby = 410;
            BITMAP bmp;
            
            
            
            if (GetObject (mi_bmp, sizeof(BITMAP), &bmp))
            {
               tbx = bmp.bmWidth;
               tby = bmp.bmHeight;	  
            }
            
            
            old = SelectObject(hMemoryDC,mi_bmp);   
            
            
            
            SetStretchBltMode(stdpan->vhdc,COLORONCOLOR);
            
            onx = principal_rect.right-principal_rect.left+1;
            ony = principal_rect.bottom-principal_rect.top+1;
            
            nx = onx;
            ny = onx * tby / tbx;
            
            if (ny > ony)
            {
               ny = ony;
               nx = ony * tbx / tby;
            }
            
            StretchBlt(stdpan->vhdc,principal_rect.left+((onx-nx)/2),principal_rect.top+((ony-ny)/2),nx,
               ny,  
               hMemoryDC,
               0,0,
               tbx,tby,
               SRCCOPY);
            
            
            SelectObject(hMemoryDC,old);
            DeleteObject(mi_bmp);
            memset(stdpan->vv,0,stdpan->size);
            v_refresca();
         }	  
         return 0;
}


int carga_grafico(FileName,w)
char *FileName;
VENT *w;
{
			char tmp[128];
         int alto  = (w->vfin)/100 - (w->vini)/100 + 1;
         int ancho = (w->vfin)%100 - (w->vini)%100 + 1;
         BITMAPINFOHEADER   bi;
         HDC hdc;
         int ajuste;
         long nx,ny,dx,dy;
         long ofx = 0,ofy = 0;
         RECT espacio;
         int directo = 0;
         
         
         strcpy(tmp,FileName);
         strcat(tmp,".bmp");
         if (ReadDIB(tmp) == FALSE)
         {
            return -1;
         }
         else
         {
            DibInfo(hbiCurrent,&bi);
            
            if (hpalCurrent)
            {
               hdc = GetDC(principal_w);
               OldPalette = SelectPalette(hdc, hpalCurrent, FALSE);
               RealizePalette(hdc);
               ReleaseDC(principal_w,hdc);
               stdpan->Oldpalette = SelectPalette(stdpan->vhdc, hpalCurrent, FALSE);
               RealizePalette(stdpan->vhdc);
               if (w != stdpan)
               {
                  w->Oldpalette = SelectPalette(w->vhdc, hpalCurrent, FALSE);
                  RealizePalette(w->vhdc);
               }
            }
         }
         
         
         if (w == stdpan)
         {    
            if (alto == 25)
               alto = 24;  // Ya no existe la linea 24 diferente
         }
         
         ajuste = 0;
         
         
         nx = scaracter.cx * ancho;
         ny = scaracter.cy * alto - ajuste;
         dx = (nx-bi.biWidth)/bi.biWidth;
         dy = (ny-bi.biHeight)/bi.biHeight;
 
         if (!proporcion_especial_bmp)
         {
             if ( (dx < 0 && dy < 0 && dx < dy) || (dx >= 0 && dy >= 0 && dx < dy) || (dx < 0 && dy >= 0) )
             {
                dy = ny;
                ny = bi.biHeight + bi.biHeight * (nx-bi.biWidth) / bi.biWidth;
                ofy = (dy - ny) / 2;
             }
             else
             if (dx != 0 || dy != 0)
             {
               dx = nx;
               nx = bi.biWidth + bi.biWidth * (ny-bi.biHeight) / bi.biHeight;
               ofx = (dx - nx) / 2;
             }
         }
         else
         if (proporcion_especial_bmp == 2)
            directo = 1;
            
            if (w == stdpan)
            {
               StretchDibBlt(w->vhdc,
                  (WORD)ofx,espacio_arriba+ajuste+(WORD)ofy,          // Destination rect origin
                  (WORD)nx,(WORD)ny,        // Destination rect extents
                  hbiCurrent,           // Bitmap to display
                  0,0,    // Source rect origin
                  bi.biWidth,bi.biHeight,
                  SRCCOPY,              // Raster operation
                  0,directo);           // Pal usage DIB_PAL_COLORS or DIB_RGB_COLORS	   
            }
            else
            {
               if (ajuste)
               {
                  BitBlt(w->vhdc,0,0,scaracter.cx * ancho,ajuste,stdpan->vhdc,(w->vini%100-1)*scaracter.cx,espacio_arriba,SRCCOPY);
               }
               if (ofx ||ofy)
               {
                  SetRect(&espacio,0,ajuste,scaracter.cx * ancho,scaracter.cy * alto);
                  FillRect(w->vhdc,&espacio,agi_brush_gris);
               }
               
               StretchDibBlt(w->vhdc,
                  (WORD)ofx,ajuste+(WORD)ofy,          // Destination rect origin
                  (WORD)nx,(WORD)ny,        // Destination rect extents
                  hbiCurrent,           // Bitmap to display
                  0,0,    // Source rect origin
                  bi.biWidth,bi.biHeight,
                  SRCCOPY,              // Raster operation
                  0,directo);           // Pal usage DIB_PAL_COLORS or DIB_RGB_COLORS
               
               
               w->sacada = 1;
            }
            
            FreeDib();
            memset(w->vv,0,w->size);
            memset(w->vv+w->size,(((PFcolor << 4) & 0xf0) | PCcolor),w->size);
            memset(w->vv+2*w->size,Patrib,w->size);
            return(0);
}


#ifdef COMENTARIO
int old_printa_grafico(FileName)
LPSTR FileName;
{
   char tmp[81];
   BITMAPINFOHEADER   bi;
   HDC hdc;
   int ajuste;
   RECT espacio;
   long nx,ny,dx,dy;
   long ofx = 0,ofy = 0;
   HPALETTE holdpal1 = hpalCurrent;
   HPALETTE holdpal2 = OldPalette;
   
   if (!hPr) return(-1);
   
   strcpy(tmp,FileName);
   strcat(tmp,".bmp");
   
   hpalCurrent = NULL;
   OldPalette = NULL;
   if (ReadDIB(tmp) == FALSE)
   {
      hpalCurrent = holdpal1;
      OldPalette = holdpal2;
      return(-1);
   }
   if (hpalCurrent)
      DeleteObject(hpalCurrent);
   hpalCurrent = holdpal1;
   OldPalette = holdpal2;
   DibInfo(hbiCurrent,&bi);
   nx = (((long)PcxInch) * 827L / 100L);// GetDeviceCaps (hPr, HORZRES);
   ny = (((long)PcyInch) * 1168L / 100L);// GetDeviceCaps (hPr, VERTRES);
   dx = (nx-bi.biWidth)/bi.biWidth;
   dy = (ny-bi.biHeight)/bi.biHeight;
   if ( (dx < 0 && dy < 0 && dx < dy) || (dx >= 0 && dy >= 0 && dx < dy) || (dx < 0 && dy >= 0) )
   {
      dy = ny;
      ny = (long)bi.biHeight + (long)bi.biHeight * (nx-(long)bi.biWidth) / (long)bi.biWidth;
      ofy = (dy - ny) / 2;
   }
   else
   {
      dx = nx;
      nx = (long)bi.biWidth + (long)bi.biWidth * (ny-(long)bi.biHeight) / (long)bi.biHeight;
      ofx = (dx - nx) / 2;
   }
   
   StretchDibBlt(hPr,
      (WORD)ofx,(WORD)ofy,      // Destination rect origin
      (WORD)nx,(WORD)ny,        // Destination rect extents
      hbiCurrent,           // Bitmap to display
      0,0,    // Source rect origin
      bi.biWidth,bi.biHeight,
      SRCCOPY,              // Raster operation
      0,0);           // Pal usage DIB_PAL_COLORS or DIB_RGB_COLORS
   FreeDib();
   return(0);
}
#endif

pinta_linea(hdc,color,x,y,cx,cy)
HDC hdc;
COLORREF color;
int x,y,cx,cy;
{
   HPEN hpen,hpenold;
   hpen = CreatePen(PS_SOLID, 1, color);
   hpenold = SelectObject(hdc,hpen);
   MoveToEx(hdc,x,y,NULL);
   LineTo(hdc,cx,cy);
   SelectObject(hdc,hpenold);
   return(0);
}




void init_bmp_base(BMP_BASE_PAR *pPar)
{
		 HDC hDC;
         HDC hmdc;
         HBITMAP hbmp;
         HBITMAP hbmp2;
         HBITMAP old_bmp;
         HBITMAP agimo;
         int i;
         int nbits;
         int nplan;
         OUTLINETEXTMETRIC *FontMetric;
         int tamafo;
         int cx,cy;
         char tmp[81];
         HBITMAP tapiz;
         BITMAP Bitmap;
         SIZE ss;
         RECT espacio;
         RECT tmpr;
         int mi_tapiz = 0;
         int LINEAS = 22;
         int COLUMNAS = 80;
         int px,py,flags;
         RECT *pRorigen = &(pPar->marco);
         SIZE cs;
         TEXTMETRIC tm;
         
         
		 if (getenv("DSSINCURSOR")) PuedoCambiarCursor = 0;


		 if ((pPar->flags&1))
		 {
			 Pseudo10 = 1;
			 ModoProfundo = 1;
			 usar_grid = 777;
		 }
         
         agi_acelerador = LoadAccelerators(hInst,"AGIXTECLADO");
         orig_acelerador = agi_acelerador;
         
         /* crear el font base para caracteres */
         
         agi_fcampo = CreateFontIndirect(&(pPar->campo));
         agi_Font   = CreateFontIndirect(&(pPar->texto));
         agi_finten = CreateFontIndirect(&(pPar->intenso));
         agi_frever = CreateFontIndirect(&(pPar->reverse));
         agi_fmensa = CreateFontIndirect(&(pPar->mensaje));
         agi_fboton = CreateFontIndirect(&(pPar->boton));

         color_fondo_campo = pPar->cfondillo;
         color_texto_campo = pPar->ctextillo;

		 color_custom_subrayado = pPar->c_custom_subrayado;         
         
         if (!agi_fcampo || !agi_Font || !agi_finten || !agi_frever || !agi_fmensa || !agi_fboton)
         {
            MessageBox(principal_w,"Fallo en Font",NULL, MB_OK | MB_ICONHAND);
         }
         
         hDC = GetDC(principal_w);
         SetMapMode(hDC,MM_TEXT);
         SetBkMode(hDC,TRANSPARENT);
         SetTextAlign(hDC,TA_NOUPDATECP | TA_LEFT );
         /*f_ant_Font = (HFONT)SelectObject( hDC, agi_fcampo );*/
         f_ant_Font = (HFONT)SelectObject( hDC, agi_Font );
         if (GetTextMetrics(hDC,&tm))
         {
             scaracter.cy = tm.tmHeight;
             scaracter.cx = tm.tmMaxCharWidth; 
         }
         else
             GetTextExtentPoint(hDC,"M",1,&scaracter);
         SelectObject( hDC, f_ant_Font );
         
         if (pRorigen && (pRorigen->left != pRorigen->right ||  pRorigen->top != pRorigen->bottom))
         {
            //i = scaracter.cx * ((pRorigen->bottom - pRorigen->top)*25) / ((pRorigen->right - pRorigen->left)*80);
            if (ModoProfundo)
               /*i = scaracter.cx * (530/25) / (640/80);*/
			   i = scaracter.cx * (480/25) / (640/80);
            else
               i = scaracter.cx * (480/25) / (640/80);
         }
         else
         {
            if (ModoProfundo)
               /*i = scaracter.cx * (530/25) / (640/80);*/
			   i = scaracter.cx * (480/25) / (640/80);
            else
                i = scaracter.cx * (425/25) / (640/80);
         }
         
             cy = scaracter.cy;
             if (i > scaracter.cy)
                scaracter.cy = i;
             i = i - cy;
             if (i > 1)
             {
                caracterdy = i/2;
                if (i == (caracterdy*2))
                   caracterdy--;
             }
             else
                caracterdy = 0;


         caracterdy1 = caracterdy;
         caracterdy5 = caracterdy;
         caracterdy6 = caracterdy;
         


         {

             f_ant_Font = (HFONT)SelectObject( hDC, agi_finten );
             if (GetTextMetrics(hDC,&tm))
             {
                 cs.cy = tm.tmHeight;
                 cs.cx = tm.tmMaxCharWidth; 
             }
             else
                 GetTextExtentPoint(hDC,"M",1,&cs);
             SelectObject( hDC, f_ant_Font );             

             if (pRorigen && (pRorigen->left != pRorigen->right ||  pRorigen->top != pRorigen->bottom))
             {
                if (ModoProfundo)
               /*i = cs.cx * (530/25) / (640/80);*/
			   i = cs.cx * (480/25) / (640/80);
                else
                   i = cs.cx * (480/25) / (640/80);
             }
             else
             {
                if (ModoProfundo)
               /*i = cs.cx * (530/25) / (640/80);*/
			   i = cs.cx * (480/25) / (640/80);
                else
                    i = cs.cx * (425/25) / (640/80);
             }

             i = i - cs.cy;
             if (i > 1)
             {
                caracterdy = i/2;
                if (i == (caracterdy*2))
                   caracterdy--;
             }
             else
                caracterdy = 0;


             caracterdy2 = caracterdy;
         }

         {

             f_ant_Font = (HFONT)SelectObject( hDC, agi_frever );
             if (GetTextMetrics(hDC,&tm))
             {
                 cs.cy = tm.tmHeight;
                 cs.cx = tm.tmMaxCharWidth; 
             }
             else
                 GetTextExtentPoint(hDC,"M",1,&cs);
             SelectObject( hDC, f_ant_Font );             

             if (pRorigen && (pRorigen->left != pRorigen->right ||  pRorigen->top != pRorigen->bottom))
             {
                if (ModoProfundo)
               /*i = cs.cx * (530/25) / (640/80);*/
			   i = cs.cx * (480/25) / (640/80);
                else
                   i = cs.cx * (480/25) / (640/80);
             }
             else
             {
                if (ModoProfundo)
               /*i = cs.cx * (530/25) / (640/80);*/
			   i = cs.cx * (480/25) / (640/80);
                else
                    i = cs.cx * (425/25) / (640/80);
             }

             i = i - cs.cy;
             if (i > 1)
             {
                caracterdy = i/2;
                if (i == (caracterdy*2))
                   caracterdy--;
             }
             else
                caracterdy = 0;


             caracterdy3 = caracterdy;
         }


         {
             

             f_ant_Font = (HFONT)SelectObject( hDC, agi_fcampo );             

             if (GetTextMetrics(hDC,&tm))
             {
                 cs.cy = tm.tmHeight;
                 cs.cx = tm.tmMaxCharWidth; 
             }
             else
                 GetTextExtentPoint(hDC,"M",1,&cs);

             SelectObject( hDC, f_ant_Font );             

#ifdef OTROFONT			 

             if (pRorigen && (pRorigen->left != pRorigen->right ||  pRorigen->top != pRorigen->bottom))
             {
                if (ModoProfundo)
               /*i = cs.cx * (530/25) / (640/80);*/
			   i = cs.cx * (480/25) / (640/80);
                else
                   i = cs.cx * (480/25) / (640/80);
             }
             else
             {
                if (ModoProfundo)
               /*i = cs.cx * (530/25) / (640/80);*/
			   i = cs.cx * (480/25) / (640/80);
                else
                    i = cs.cx * (425/25) / (640/80);
             }

             i = i - cs.cy;
             if (i > 1)
             {
                caracterdy = i/2;
                if (i == (caracterdy*2))
                   caracterdy--;
             }
             else
                caracterdy = 0;

             caracterdy4 = caracterdy;
#else
			 caracterdy4 = caracterdy1;
#endif
         }

         
         ReleaseDC(principal_w, hDC);
         
         
         GetClientRect(principal_w,&tmpr);
         
         principal_rect.top = tmpr.top;
         principal_rect.left = tmpr.left + EXTRAIZQUIERDA;
         principal_rect.right = tmpr.left + (scaracter.cx * COLUMNAS - 1) + EXTRADERECHA + EXTRAIZQUIERDA;
         principal_rect.bottom = tmpr.top + (scaracter.cy * LINEAS)/* + espacio_arriba + espacio_abajo*/;
         espacio_arriba = -(scaracter.cy * 3);
         
         
         //AdjustWindowRect(&principal_rect,ESTILO,FALSE);
         
         
         //principal_rect.bottom -= principal_rect.top;
         //principal_rect.right -= principal_rect.left;
         //principal_rect.top = 0;
         //principal_rect.left = 0;
         
         //espacio_arriba = alto - principal_rect.bottom - principal_rect.top;
         //principal_rect.bottom += espacio_arriba;
         //espacio_arriba = 26; /* !!!!! */
         //espacio_arriba = -8;
         
         
         //ancho = (ancho - principal_rect.right) / 2;
         //alto = (alto - principal_rect.bottom) / 2;
         px = 0;
         py = 0;
         
         if (!(pPar->flags&1) && (!pRorigen || pRorigen->top || pRorigen->bottom || pRorigen->left || pRorigen->right))
            flags = SWP_SHOWWINDOW;
         else
            flags = SWP_HIDEWINDOW;
         
         
         ///*SWP_NOZORDER*/ SWP_SHOWWINDOW /*| SWP_DRAWFRAME | SWP_FRAMECHANGED*/
         if (pRorigen && (pRorigen->left != pRorigen->right ||  pRorigen->top != pRorigen->bottom))
         {
            px = pRorigen->left + ((pRorigen->right + 1 - pRorigen->left) - (principal_rect.right+1-principal_rect.left)) / 2;
            py = pRorigen->top + ((pRorigen->bottom + 1 - pRorigen->top) - (principal_rect.bottom+1-principal_rect.top)) / 2;
         }
         else
            flags |= SWP_NOMOVE;
         
         
         SetWindowPos (principal_w, HWND_TOP , px , py,
            principal_rect.right+1-principal_rect.left,principal_rect.bottom+1-principal_rect.top,
            flags/* | SWP_ASYNCWINDOWPOS*/);
         
         
         GetClientRect(principal_w,&principal_rect);
         
         principal_nx = principal_rect.right-principal_rect.left;
         principal_ny = principal_rect.bottom-principal_rect.top;
         
         
         
         hDC = GetDC(principal_w);
         hMemoryDC = CreateCompatibleDC(hDC);
         SetMapMode(hMemoryDC,MM_TEXT);
         SetBkMode(hMemoryDC,TRANSPARENT);
         hmdc = CreateCompatibleDC(hDC);
         agitapiz = CreateCompatibleBitmap(hDC,principal_nx,principal_ny);
         C_agibmp = CreateCompatibleBitmap(hDC,MAXMOSAICO*14,6*32);
         ReleaseDC(principal_w, hDC);
         
         hOldBitmap = SelectObject(hMemoryDC,C_agibmp);
         for (i = 0;i < 128;i++)
         {
            if (C_agigrafico[i])
            {
               sprintf(tmp,"AGDSG%3d",i+128);
               agimo = LoadBitmap(hInst,tmp);
               old_bmp = SelectObject(hmdc,agimo);
               BitBlt(hMemoryDC,(C_mosaico[i]-1)*14,0,14,32,hmdc,0,0,SRCCOPY);
               SelectObject(hmdc,old_bmp);
               DeleteObject(agimo);
               
               if (C_agigrafico[i] > 1)
               {
                  //sprintf(tmp,"AGFSG%3d",i+128);
                  sprintf(tmp,"AGDSG%3d",i+128);
                  agimo = LoadBitmap(hInst,tmp);
                  old_bmp = SelectObject(hmdc,agimo);
                  BitBlt(hMemoryDC,(C_mosaico[i]-1)*14,32,14,32,hmdc,0,0,SRCCOPY);
                  SelectObject(hmdc,old_bmp);
                  DeleteObject(agimo);
               }
               if (C_agigrafico[i] > 2)
               {
                  //sprintf(tmp,"AGISG%3d",i+128);
                  sprintf(tmp,"AGDSG%3d",i+128);
                  agimo = LoadBitmap(hInst,tmp);
                  old_bmp = SelectObject(hmdc,agimo);
                  BitBlt(hMemoryDC,(C_mosaico[i]-1)*14,64,14,32,hmdc,0,0,SRCCOPY);
                  SelectObject(hmdc,old_bmp);
                  DeleteObject(agimo);
               }
               sprintf(tmp,"AMDSG%3d",i+128);
               agimo = LoadBitmap(hInst,tmp);
               old_bmp = SelectObject(hmdc,agimo);
               BitBlt(hMemoryDC,(C_mosaico[i]-1)*14,96,14,32,hmdc,0,0,SRCCOPY);
               SelectObject(hmdc,old_bmp);
               DeleteObject(agimo);
               if (C_agigrafico[i] > 1)
               {
                  //sprintf(tmp,"AMFSG%3d",i+128);
                  sprintf(tmp,"AMDSG%3d",i+128);
                  agimo = LoadBitmap(hInst,tmp);
                  old_bmp = SelectObject(hmdc,agimo);
                  BitBlt(hMemoryDC,(C_mosaico[i]-1)*14,128,14,32,hmdc,0,0,SRCCOPY);
                  SelectObject(hmdc,old_bmp);
                  DeleteObject(agimo);
               }
               if (C_agigrafico[i] > 2)
               {
                  //sprintf(tmp,"AMISG%3d",i+128);
                  sprintf(tmp,"AMDSG%3d",i+128);
                  agimo = LoadBitmap(hInst,tmp);
                  old_bmp = SelectObject(hmdc,agimo);
                  BitBlt(hMemoryDC,(C_mosaico[i]-1)*14,160,14,32,hmdc,0,0,SRCCOPY);
                  SelectObject(hmdc,old_bmp);
                  DeleteObject(agimo);
               }
            }
         }
         
         
         SelectObject(hMemoryDC,hOldBitmap);
         hOldBitmap = NULL;
         SetFocus (principal_w);
         
         agi_brush = CreateSolidBrush(RGB(192,192,192));
         agi_brush_gris = CreateSolidBrush(RGB(192,192,192));
         
         /* para los bmp */
         hpalCurrent = NULL;
         OldPalette = NULL;
         
         hOldBitmap = SelectObject(hMemoryDC,agitapiz);
         
         FillRect(hMemoryDC, &principal_rect,GetStockObject(LTGRAY_BRUSH));
         
         
         DeleteDC(hmdc);
         
         SelectObject(hMemoryDC,hOldBitmap);
         hOldBitmap = NULL;
         
         for (i =0;i < MAXBOTON;i++)
         {
            SetRectEmpty(agi_boton+i);
            agi_boton_up[i] = NULL;
            agi_boton_updefecto[i] = NULL;
            agi_boton_down[i] = NULL;
            agi_boton_id[i] = 0;
            agi_boton_flag[i] = 0;
         }
         
         
         cur_flecha = LoadCursor(NULL,IDC_ARROW);
         cur_mano = LoadCursor(NULL,IDC_IBEAM);
         cur_boton = LoadCursor(hInst,"AGICBOTON");
         cur_prohibido = LoadCursor(hInst,"AGICPROHIBI");
         cur_espera = LoadCursor(NULL,IDC_WAIT);
         cur_pulsado = LoadCursor(hInst,"AGIPULSADO");
         
         hData = NULL;
         timer_especial_int = agi_cola_timer;
}

void elimina_todo()
{
  
}


static HCURSOR DsSetCursor(HCURSOR cur)
{
if (PuedoCambiarCursor)
   return(SetCursor(cur));
else
   return(cur);
}

void fin_bmp_base()
{
   HDC hDC;
   int i;
   
   timer_especial_int = NULL;
   
   if (cur_ini)
      DsSetCursor(cur_ini);
   cur_ini = NULL;
   //if (cur_flecha)
   //  DestroyCursor(cur_flecha);
   //if (cur_mano)
   //  DestroyCursor(cur_mano);
   if (cur_boton)
      DestroyCursor(cur_boton);
   if (cur_prohibido)
      DestroyCursor(cur_prohibido);
   //if (cur_espera)
   //  DestroyCursor(cur_espera);
   if (cur_pulsado)
      DestroyCursor(cur_pulsado);
   cur_flecha = NULL;
   cur_mano = NULL;
   cur_boton = NULL;
   cur_prohibido = NULL;
   cur_espera = NULL;
   cur_pulsado = NULL;
   
   if (hMemoryDC)
   {
      if (hOldBitmap)
      {
         SelectObject(hMemoryDC,hOldBitmap);
         hOldBitmap = NULL;
      }
      DeleteDC(hMemoryDC);
      hMemoryDC = NULL;
   }
   
   DeleteObject(agitapiz);
   agitapiz = NULL;
   
   
   if (OldPalette)
   {
      hDC = GetDC(principal_w);
      SelectPalette(hDC, OldPalette, FALSE);
      RealizePalette(hDC);
      ReleaseDC(principal_w, hDC);
   }
   
   hDC = GetDC(principal_w);
   
   if (hpalCurrent)
      DeleteObject(hpalCurrent);
   hpalCurrent = NULL;
   OldPalette = NULL;
   if (agi_brush)
   {
      DeleteObject(agi_brush);
      agi_brush = NULL;
   }
   if (agi_brush_gris)
   {
      DeleteObject(agi_brush_gris);
      agi_brush_gris = NULL;
   }
   
   ReleaseDC(principal_w, hDC);
   
   if (agi_Font)
      DeleteObject(agi_Font);
   if (agi_fmensa)
      DeleteObject(agi_fmensa);
   if (agi_finten)
      DeleteObject(agi_finten);
   if (agi_frever)
      DeleteObject(agi_frever);
   if (agi_fcampo)
      DeleteObject(agi_fcampo);
   if (agi_fboton)
      DeleteObject(agi_fboton);
   agi_Font = NULL;
   agi_fmensa = NULL;
   agi_finten = NULL;
   agi_frever = NULL;
   agi_fcampo = NULL;
   agi_fboton = NULL;
   
   DeleteObject(C_agibmp);
   C_agibmp = NULL;
   
   for (i = 0;i < MAXBOTON;i++)
   {
      if (!IsRectEmpty(agi_boton+i))
      {
         DeleteObject(agi_boton_up[i]);
         DeleteObject(agi_boton_down[i]);
         if (agi_boton_updefecto[i])
            DeleteObject(agi_boton_updefecto[i]);
         SetRectEmpty(agi_boton+i);
         agi_boton_up[i] = NULL;
         agi_boton_down[i] = NULL;
         agi_boton_updefecto[i] = NULL;
         agi_boton_id[i] = 0;
         agi_boton_flag[i] = 0;
      }
   }
   
#ifndef FGV32
   if (agilpDlgProcx)
   {
      FreeProcInstance ( (FARPROC) agilpDlgProcx );
      agilpDlgProcx = NULL;
   }
   if (agilpDlgImpre)
   {
      FreeProcInstance ( (FARPROC) agilpDlgImpre );
      agilpDlgImpre = NULL;
   }
   if (agilpDlginput)
   {
      FreeProcInstance ( (FARPROC) agilpDlginput );
      agilpDlginput = NULL;
   }
#endif
   
   if (hData)
   {
      if (OpenClipboard(principal_w))
      {
         EmptyClipboard();
         CloseClipboard();
      }
      GlobalFree(hData);
   }
   hData = NULL;
}



static void activa_color_texto(hdc)
HDC hdc;
{
   COLORREF hcolor;
   int intenso = 0;
   int  color = PCcolor;

   if (!PCcolor && !PFcolor && Patrib != ' ' && Patrib  != 'N' && (Patrib & 128))
   {
	   hcolor = color_custom_subrayado;
	   hcolor = GetNearestColor(hdc,hcolor);
       SetTextColor(hdc,hcolor);
       hcolor = GetNearestColor(hdc,color_fondo_campo);
       SetBkColor(hdc,hcolor);
	   if (agi_brush)
	   {
	  	  DeleteObject(agi_brush);
	   }
	   agi_brush = CreateSolidBrush(hcolor);
	   return;
   }
   
   if (color > 7)
   {
      color -= 8;
      intenso = 1;
   }
   else
   {
      if ((Patrib & (~128)) == 'I')
         intenso = 80;
      else
         intenso = 0;
   }
   switch (color)
   {
   case 1:  /* azul */
      if (intenso)
         hcolor = RGB(0,0,248);
      else
         hcolor = RGB(0,0,128);
      break;
   case 2:  /* verde */
      if (intenso)
         hcolor = RGB(0,252,0);
      else
         hcolor = RGB(0,128,0);
      break;
   case 3:   /* cyan */
      if (intenso)
         hcolor = RGB(0,252,248);
      else
         hcolor = RGB(0,128,128);
      break;
   case 4:   /* rojo */
      if (intenso)
         hcolor = RGB(248,0,0);
      else
         hcolor = RGB(128,0,0);
      break;
   case 5:   /* magenta */
      if (intenso)
         hcolor = RGB(248,0,248);
      else
         hcolor = RGB(128,0,128);
      break;
   case 6:   /* amarillo */
      if (intenso)
         hcolor = RGB(248,252,0);
      else
         hcolor = RGB(128,128,0);
      break;
   case 7:   /* blanco */      
      if (intenso)
      {
         hcolor = RGB(255,255,255);
      }
      else
         hcolor = RGB(128,128,128);
      break;
   case 0:   /* negro */
      //if (intenso)
      //      hcolor = RGB(128,128,128);
      //else
          if ((Patrib & 128))
          {
              hcolor = color_texto_campo;
          }
          else
              hcolor = RGB(0,0,0);
   default:
      break;
   }
	  hcolor = GetNearestColor(hdc,hcolor);
     SetTextColor(hdc,hcolor);
     
     switch (PFcolor)
     {
     case 1:  /* azul */
        hcolor = RGB(0,0,128);
        break;
     case 2:  /* verde */
        hcolor = RGB(64,128,128);
        break;
     case 3:   /* cyan */
        hcolor = RGB(0,252,248);
        break;
     case 4:   /* rojo */
        hcolor = RGB(248,0,0);
        break;
     case 5:   /* magenta */
        hcolor = RGB(248,0,248);
        break;
     case 6:   /* amarillo */
        hcolor = RGB(248,252,0);
        break;
     case 7:   /* blanco */
        if ((Patrib & 128))
         {
             hcolor = color_fondo_campo;
         }
        else
             hcolor = RGB(255,255,255);
        break;
     case 0:   /* negro */
        hcolor = RGB(192,192,192);
     default:
        break;
     }
     hcolor = GetNearestColor(hdc,hcolor);
     SetBkColor(hdc,hcolor);
     if (agi_brush)
     {
        DeleteObject(agi_brush);
     }
     agi_brush = CreateSolidBrush(hcolor);
     
}


static int determina_fondo_especial(w,cx,cy,sg,ini,fin,tapiz,w2)
VENT *w;
int cx,cy,sg;
int ini,fin;
int *tapiz;
VENT *w2;
{
   int sumac = 0,sumaf = 0;
   int restac = 0,restaf = 0;
   int pf,pc;
   unsigned char c;
   unsigned char *s = w->vv; /* he supuesto fijo 80 columnas */
   unsigned char *fondo = w->mapagrafico;
   int ancho = (w2->vfin%100) - (w2->vini%100) + 1;
   int alto = (w2->vfin/100) - (w2->vini/100) + 1;
   unsigned char *s2 = w2->vv;
   
   
   if (win_fondo_neutro || sg == 219 || sg == 176 || sg == 177 || sg == 178 || sg == 220 || sg == 221 || sg == 222 || sg == 223)
      return(0);
   
   if (tapiz)
      *tapiz = 0;
   
   pf = (cy-espacio_arriba) / scaracter.cy;
   pc = cx / scaracter.cx;
   s += (pf * 80 + pc);
   s2 += ( (pf-((w2->vini/100)-1)) * ancho + pc-((w2->vini%100)-1) );
   
   fondo += (pf * 80 + pc);
   
   if (sg == 196 || sg == 205)
   {
      sumaf = 80;
      restaf = 80;
   }
   else
      if (sg == 193 || sg == 202 || sg == 208 || sg == 207)
      {
         sumaf = 80;
      }
      else
         if (sg == 192 || sg == 211 || sg == 200 || sg == 212)
         {
            restac = 1;
            sumaf = 80;
         }
         else
            if (sg == 179 || sg == 186)
            {
               sumac = 1;
               restac = 1;
            }
            else
               if (sg == 180 || sg == 182 || sg == 185 || sg == 181)
               {
                  sumac = 1;
               }
               else
                  if (sg == 191 || sg == 187 || sg == 183 || sg == 184)
                  {
                     sumac = 1;
                     restaf = 80;
                  }
                  else
                     if (sg == 217 || sg == 188 || sg == 189 || sg == 190)
                     {
                        sumaf = 80;
                        sumac = 1;
                     }
                     else
                        if (sg == 195 || sg == 199 || sg == 204 || sg == 198)
                        {
                           restac = 1;
                        }
                        else
                           if (sg == 218 || sg == 214 || sg == 201 || sg == 213)
                           {
                              restac = 1;
                              restaf = 80;
                           }
                           else
                              if (sg == 194 || sg == 210 || sg == 203 || sg == 209)
                              {
                                 restaf = 80;
                              }
                              else
                                 return((*fondo = 0));
                              
                              if (*fondo)
                              {
                                 if (tapiz)
                                    *tapiz = (*fondo)/100;
                                 return(((*fondo)%100));
                              }
                              
                              if (sumaf)
                              {
                                 if (tapiz)
                                 {
                                    if (pf >= 22)
                                       *tapiz = 1;
                                    else
                                    {
                                       c = *(s+sumaf);
                                       if (c == 1 || !c)
                                          *tapiz = 1;
                                    }
                                    *fondo = (*tapiz) * 100;
                                 }
                                 if (un_fondo_especial == 1)
                                    return(((*fondo += 1)%100));
                                 if (pf >= ((fin/100)-1))
                                 {
                                    return( ((*fondo += 1)%100) );
                                 }
                                 c = *(s+sumaf);
                                 if (c == 1 || !c || c == 219 || c == 176 || c == 177 || c == 178)
                                 {
                                    return(((*fondo += 1)%100) );
                                 }
                                 if (*s == *s2)
                                 {
                                    c = *(s2+ancho);
                                    if (c == 1 || !c || c == 219 || c == 176 || c == 177 || c == 178)
                                    {
                                       return(((*fondo += 1)%100) );
                                    }
                                 }
                              }
                              if (sumac)
                              {
                                 if (tapiz)
                                 {
                                    if (pc >= 79)
                                       *tapiz = 1;
                                    else
                                    {
                                       c = *(s+sumac);
                                       if (c == 1 || !c)
                                          *tapiz = 1;
                                    }
                                    *fondo = (*tapiz) * 100;
                                 }
                                 if (un_fondo_especial == 2)
                                    return(((*fondo += 2)%100));
                                 if (pc >= ((fin%100)-1))
                                    return(((*fondo += 2)%100));
                                 c = *(s+sumac);
                                 if (c == 1 || !c || c == 219 || c == 176 || c == 177 || c == 178)
                                    return(((*fondo += 2)%100));
                                 if (*s == *s2)
                                 {
                                    c = *(s2+sumac);
                                    if (c == 1 || !c || c == 219 || c == 176 || c == 177 || c == 178)
                                       return(((*fondo += 2)%100));
                                 }
                              }
                              if (restaf)
                              {
                                 if (un_fondo_especial == 3)
                                    return(((*fondo += 3)%100));
                                 if (pf <= ((ini/100)-1))
                                    return(((*fondo += 3)%100));
                                 c = *(s-restaf);
                                 if (c == 1 || !c || c == 219 || c == 176 || c == 177 || c == 178 )
                                 {
                                    return(((*fondo += 3)%100));
                                 }
                                 if (*s == *s2)
                                 {
                                    c = *(s2-ancho);
                                    if (c == 1 || !c || c == 219 || c == 176 || c == 177 || c == 178 )
                                    {
                                       return(((*fondo += 3)%100));
                                    }
                                 }
                              }
                              if (restac)
                              {
                                 if (un_fondo_especial == 4)
                                    return(((*fondo += 4)%100));
                                 if (pc <= ((ini%100)-1))
                                    return(((*fondo += 4)%100));
                                 c = *(s-restac);
                                 if (c == 1 || !c || c == 219 || c == 176 || c == 177 || c == 178)
                                 {
                                    return(((*fondo += 4)%100));
                                 }
                                 if (*s == *s2)
                                 {
                                    c = *(s2-restac);
                                    if (c == 1 || !c || c == 219 || c == 176 || c == 177 || c == 178)
                                    {
                                       return(((*fondo += 4)%100));
                                    }
                                 }
                              }
                              /* se deja discurrir hasta aqui para recoger el tema del tapiz */
                              /* por si coincide el tema */
                              return((*fondo = 0));
}

/* caso especial trama : para que no se vea el recuadro en caso de que
el tamaño del caracter sea impar las posiciones imepares (se corren) */
static int caso_especial_trama(cy,ncy,c)
int cy,ncy,c;
{
   if ((c != 176 && c !=  177 && c!= 178) || !(ncy%2)) return(0);
   if ( (cy /ncy)%2 ) return(1);
   return(0);
}

static enmarca_texto_old(hdc,r,m,kk)
HDC hdc;
RECT *r;
int m,kk;
{
   HPEN hpen;
   HPEN hpenOld;
   
   
   if (m)
      hpen = CreatePen(PS_SOLID, 1, RGB(128, 128, 128));
   else
      hpen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
   hpenOld = SelectObject(hdc, hpen);
   
   MoveToEx(hdc,r->left,r->top,NULL);
   LineTo(hdc,r->right,r->top);
   
   if (m && !kk)
			{
      MoveToEx(hdc,r->left,r->top,NULL);
      LineTo(hdc,r->left,r->bottom+1);
			}
   
   if (!m)
			{
      MoveToEx(hdc,r->right+1,r->bottom-1,NULL);
      LineTo(hdc,r->left-1,r->bottom-1);
			}
   
   SelectObject(hdc, hpenOld);
   DeleteObject(hpen);
   
   return(0);
}


static enmarca_texto(hdc,r,m,kk,posterior)
HDC hdc;
RECT *r;
int m,kk,posterior;
{
   HPEN hpeno;
   HPEN hpenc;
   HPEN hpenb;
   HPEN hpenn;
   HPEN hpenOld;
   int mueve = 0;
   
   if (!ModoProfundo) return(enmarca_texto_old(hdc,r,m,kk));
   /*if (!m) return(enmarca_texto_old(hdc,r,m,kk));*/
   


   hpeno = CreatePen(PS_SOLID, 1, RGB(128, 128, 128));
   hpenc = CreatePen(PS_SOLID, 1, RGB(192, 192, 192));
   hpenn = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
   hpenb = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
      

   hpenOld = SelectObject(hdc, hpeno);
   
   MoveToEx(hdc,r->left-2,r->top+mueve,NULL);
   LineTo(hdc,r->right+2,r->top+mueve);
   SelectObject(hdc, hpenn);
   MoveToEx(hdc,r->left-1,r->top+1+mueve,NULL);
   LineTo(hdc,r->right+1,r->top+1+mueve);   
   if (!kk)
   {
      MoveToEx(hdc,r->left-1,r->top+1+mueve,NULL);
      LineTo(hdc,r->left-1,r->bottom-2-mueve);
      SelectObject(hdc, hpeno);
      MoveToEx(hdc,r->left-2,r->top+mueve,NULL);
      LineTo(hdc,r->left-2,r->bottom-1-mueve);   
   }

   SelectObject(hdc, hpenc);   
   if (!posterior)
   {
       MoveToEx(hdc,r->right+1,r->top+1+mueve,NULL);
       LineTo(hdc,r->right+1,r->bottom-1-mueve);
   }

   MoveToEx(hdc,r->left-1,r->bottom-2-mueve,NULL);
   LineTo(hdc,r->right+2,r->bottom-2-mueve);     

   SelectObject(hdc, hpenb);
   /*
   MoveToEx(hdc,r->right,r->top+2,NULL);// rellono final 
   LineTo(hdc,r->right,r->bottom-2);
   MoveToEx(hdc,r->right+1,r->top+2,NULL);// rellono final
   LineTo(hdc,r->right+1,r->bottom-2);
   */

   if (!posterior)
   {
       MoveToEx(hdc,r->right+2,r->top+mueve,NULL);
       LineTo(hdc,r->right+2,r->bottom-mueve);
   }
   
   MoveToEx(hdc,r->left-2,r->bottom-1-mueve,NULL);
   LineTo(hdc,r->right+3,r->bottom-1-mueve);

   if (mueve)
   {
       int i;
       SelectObject(hdc, hpenc);

       for (i = 0;i < mueve;i++)
       {
         MoveToEx(hdc,r->left,r->top+i,NULL);
         LineTo(hdc,r->right+1,r->top+i);
         MoveToEx(hdc,r->left,r->bottom-i,NULL);
         LineTo(hdc,r->right+1,r->bottom-i);
       }

   }


   SelectObject(hdc, hpenOld);
   DeleteObject(hpeno);
   DeleteObject(hpenc);
   DeleteObject(hpenn);
   DeleteObject(hpenb);

   
   return(0);
}

HFONT selecciona_font(hdc,a)
HDC hdc;
unsigned char a;
{
			if (a & 128)
         {
            caracterdy = caracterdy4;
            return(SelectObject(hdc,agi_fcampo));
         }
         switch(a)
         {
         case 'I':
            caracterdy = caracterdy2;
            return(SelectObject(hdc,agi_finten));
         case 'R':
            caracterdy = caracterdy3;
            return(SelectObject(hdc,agi_frever));
         default:
            caracterdy = caracterdy1;
            return(SelectObject(hdc,agi_Font));
         }
         return(NULL);
}

int stdpan_es_invisible()
{
   stdpan->visible = 0;
   return(0);
}

/* convierte los textos y los pone en la imagen grafica */
void w_win_texto(w,ini,fin,w2) /* no chequeada */
VENT *w;                       /* ini >= vini y fin <= vfin */
int ini;
int fin;
VENT *w2;
{
   unsigned char *s=w->vv;
   register int i,j;
   int tt;
   int alto;
   int ancho;
   int cini;
   int fini;
   int iini;
   int jini;
   int ifin;
   int jfin;
   int k,ff;
   int g,oa,og;
   int b;
   unsigned char tmp[81];
   int ntext = 0;
   unsigned char c,a;
   int PCcolor_ = PCcolor,PFcolor_ = PFcolor;
   int _PCcolor_ = _PCcolor,_PFcolor_ = _PFcolor;
   int aaa = Patrib;
   int cx,cy;
   HDC hdc;        // HDC for Window
   RECT espacio;
   HFONT oldfont;
   int xbmp;
   int ybmp;
   int xmas;
   int ymas;
   HBRUSH hbr;
   SIZE ss;
   int anta = 0;
   int posterior = 0;
   int posterior2 = 0;
   
   if (!stdpan->visible)
   {
      if (DespachadorExterno)
						{
         (*DespachadorExterno)((int)DESPACHO_HAZ_VISIBLE,0,NULL);
						}
      stdpan->visible = 1;
   }
   //if ((ini/100) < 5)
   //  {
   //  ini = 500 + ini%100;
   //  }
   
   alto  = (w->vfin)/100 - (w->vini)/100 + 1;
   ancho = (w->vfin)%100 - (w->vini)%100 + 1;
   cini = (ini % 100) - 1;
   fini = (ini / 100) - 1;
   iini = (ini / 100) - (w->vini / 100);
   jini = (ini % 100) - (w->vini % 100);
   ifin =  (fin)/100 - (w->vini)/100 + 1;
   jfin = (fin)%100 - (w->vini)%100 + 1;
   b=alto*ancho;
   
#ifdef COMENTARIO
   if (ifin > 23)
   {
      if (ifin == 24)
						{
         hdc=w->vhdc;
         cx = 3;
         cy = espacio_arriba+4 + scaracter.cy * 23;
         
         SelectObject(w->vhdc,agi_frever);
         GetTextExtentPoint(stdpan->vhdc,"M",1,&ss);
         cy += (18 - ss.cy) / 2;
         SetTextColor(w->vhdc,RGB(0,0,0));
         hbr = CreateSolidBrush(RGB(192,192,192));
         i = 0;
         for (j=jini;j<jfin;j++)
         {
            k = 23*ancho+j;
            c = convierte_caracter(*(s+k));
            if (c > 1)
            {
               SetRect(&espacio,cx,cy,cx+ss.cx,cy+18);
               FillRect(hdc,&espacio,hbr);
               ExtTextOut(w->vhdc,cx,cy,ETO_CLIPPED,&espacio,&c,1,NULL);
            }
            cx += ss.cx;
            i++;
         }
         DeleteObject(hbr);
         if (!IsIconic(principal_w))
         {
            if (!NO_PINTAR_AHORA)
            {
               hdc = GetDC(principal_w);
               BitBlt(hdc,principal_rect.left+cx,principal_rect.top+cy,ss.cx*j,18,w->vhdc,cx,cy,SRCCOPY);
               ReleaseDC(principal_w,hdc);
            }
         }
         hdc=w->vhdc;
						}
      ifin = 23;
   }
#else
   if (ifin > 24) ifin = 24;
#endif
   espacio.top = 0;
   espacio.left = 0;
   espacio.bottom = scaracter.cy;
   
   tmp[1] = 0;
   hdc=w->vhdc;
   
   oldfont = SelectObject(hdc,agi_Font);
   
   oa = -1;
   og = -1;
   
   cy = ((w == stdpan) ? espacio_arriba : 0)+fini * scaracter.cy;
   cx = cini * scaracter.cx;
   
   for (i=iini;i<ifin;i++,cy += scaracter.cy)
   {
      cx = cini * scaracter.cx;
      anta = 0;      
      for (j=jini;j<jfin;j++)
						{
         k = i*ancho+j; /* desplazamiento corriente */
         g = *(s+b+k); /* COLOR */
         a = *(s+2*b+k); /* Atributo */
         if ( (a&128) )
            posterior = 1;
         else
            posterior = 0;
         if (j < 79)
         {
             if ( (*(s+2*b+(i*ancho+j+1)) & 128) )
                posterior2 = 1;
             else
                posterior2 = 0;
         }
         if (!ntext)
         {
            if (j && (*(s+2*b+k-1) & 128))
               anta = 1;
            else
               anta = 0;
         }
         if (oa != a || og != g)
         {
            if (ntext)
            {
               tmp[ntext] = 0;
               if (ModoProfundo && (Patrib & 128))
               {
                  SetRect(&espacio,cx-1+anta,cy+2,cx+(scaracter.cx * ntext)+1-posterior,cy+scaracter.cy-2);
                  FillRect(hdc,&espacio,agi_brush);
                  SetRect(&espacio,cx,cy,cx+(scaracter.cx * ntext),cy+scaracter.cy);
               }
               else
               {
                  SetRect(&espacio,cx,cy,cx+(scaracter.cx * ntext),cy+scaracter.cy);
                  FillRect(hdc,&espacio,agi_brush);
               }
               espacio.top += caracterdy;
               DrawText(hdc,tmp,ntext,&espacio,DT_TOP | DT_LEFT | DT_SINGLELINE | DT_NOPREFIX);
               espacio.top -= caracterdy;
               if ((Patrib & 128)/* || (Patrib & (~128)) == 'R'*/)
               {
                  enmarca_texto(hdc,&espacio,((Patrib & (~128)) == 'R') ? 0 : 1,anta,posterior);
               }
               cx += (scaracter.cx * ntext);
               ntext = 0;
            }
            oa = a;
            og = g;
            if (g != 128)
            {
               if (i == 3)
               {
                  PCcolor = 7;
                  PFcolor = 0;
                  Patrib = 'N';
               }
               else
                  if ((a & 128) && (a & (~128)) == 'I')
                  {
                     PCcolor = 0;
                     PFcolor = 7;
                  }
                  else
                  {
                     PFcolor = ((g & 0x70) >> 4);
                     PCcolor = (g & 0x0f);
                  }
            }
            else
            {
               if ((a & 128) && (a & (~128)) == 'I')
               {
                  PCcolor = 0;
                  PFcolor = 7;
               }
               else
                  pon_color_a(a & (~128));
            }
            
            Patrib = a;
            activa_color_texto(hdc);
            selecciona_font(hdc,Patrib);
         }
         if (!(c = *(s+k)) || c == 219 || c == 176 || c == 177 || c == 178 || c == 1) {
            if (ntext)
            {
               tmp[ntext] = 0;
               //if (ModoProfundo && ((Patrib & 128) || (Patrib & (~128)) == 'R'))
               if (ModoProfundo && (Patrib & 128))
               {
                  SetRect(&espacio,cx-1+anta,cy+2,cx+(scaracter.cx * ntext)+1-posterior,cy+scaracter.cy-2);
                  FillRect(hdc,&espacio,agi_brush);
                  SetRect(&espacio,cx,cy,cx+(scaracter.cx * ntext),cy+scaracter.cy);
               }
               else
               {
                   SetRect(&espacio,cx,cy,cx+scaracter.cx * ntext,cy+scaracter.cy);
                   FillRect(hdc,&espacio,agi_brush);
               }
               espacio.top += caracterdy;
               DrawText(hdc,tmp,ntext,&espacio,DT_TOP | DT_LEFT | DT_SINGLELINE | DT_NOPREFIX);
               espacio.top -= caracterdy;
               
               if ((Patrib & 128)/* || (Patrib & (~128)) == 'R'*/)
               {
                  enmarca_texto(hdc,&espacio,((Patrib & (~128)) == 'R') ? 0 : 1,anta,posterior);
               }
               
               cx += (scaracter.cx * ntext);
               ntext = 0;
            }
            if (c == 1) /* tapiz */
            {
               // reponer el tapiz!!!! */
               /* ahora siempre es stdpan */
               if (win_fondo_neutro)
               {
                  SetRect(&espacio,cx,cy,cx+scaracter.cx,cy+scaracter.cy);
                  FillRect(hdc,&espacio,agi_brush_gris);
               }
               else
               {
                  hOldBitmap = SelectObject(hMemoryDC,agitapiz);
                  BitBlt(hdc,cx,cy,
                     scaracter.cx, scaracter.cy,hMemoryDC, cx,cy , SRCCOPY);
                  SelectObject(hMemoryDC,hOldBitmap);
                  hOldBitmap = NULL;
               }
            }
            cx += scaracter.cx;
            continue;
         } /* zona de pintado */
         
         if (c > 127)
         {
            c -= 128;
            if (C_agigrafico[c])
            {
               if (ntext)
               {
                  tmp[ntext] = 0;
                   if (ModoProfundo && (Patrib & 128))
                   {
                      SetRect(&espacio,cx-1+anta,cy+2,cx+(scaracter.cx * ntext)+1-posterior,cy+scaracter.cy-2);
                      FillRect(hdc,&espacio,agi_brush);
                      SetRect(&espacio,cx,cy,cx+(scaracter.cx * ntext),cy+scaracter.cy);
                   }
                   else
                   {
                      SetRect(&espacio,cx,cy,cx+scaracter.cx * ntext,cy+scaracter.cy);
                      FillRect(hdc,&espacio,agi_brush);
                   }
                  espacio.top += caracterdy;
                  DrawText(hdc,tmp,ntext,&espacio,DT_TOP | DT_LEFT | DT_SINGLELINE | DT_NOPREFIX);
                  espacio.top -= caracterdy;
                  
                  if ((Patrib & 128) /* || (Patrib & (~128)) == 'R' */)
                  {
                     enmarca_texto(hdc,&espacio,((Patrib & (~128)) == 'R') ? 0 : 1,anta,posterior);
                  }
                  cx += (scaracter.cx * ntext);
                  ntext = 0;
               }
               
               SetRect(&espacio,cx,cy,cx+scaracter.cx,cy+scaracter.cy);
               if ( !(ff = determina_fondo_especial(stdpan,cx,cy,c+128,ini,fin,&tt,w2)) || ff > 2)
               {
                  FillRect(hdc,&espacio,agi_brush);
               }
               else
               {
                  // reponer el tapiz!!!! */
                  if (tt)
                  {
                     hOldBitmap = SelectObject(hMemoryDC,agitapiz);
                     BitBlt(hdc,cx,cy,scaracter.cx, scaracter.cy,hMemoryDC, cx,cy , SRCCOPY);
                     SelectObject(hMemoryDC,hOldBitmap);
                     hOldBitmap = NULL;
                  }
               }
               
               
               if (!ff || C_agigrafico[c] == 1)
               {
                  xbmp = (C_mosaico[c]-1)*14;
                  ybmp = 0;
                  xmas = (C_mosaico[c]-1)*14;
                  ymas = 96;
               }
               else
                  if (ff == 1 || ff == 2 || C_agigrafico[c] == 2)
                  {
                     xbmp = (C_mosaico[c]-1)*14;
                     ybmp = 32;
                     xmas = (C_mosaico[c]-1)*14;
                     ymas = 128;
                  }
                  else
                  {
                     xbmp = (C_mosaico[c]-1)*14;
                     ybmp = 64;
                     xmas = (C_mosaico[c]-1)*14;
                     ymas = 160;
                  }
                  hOldBitmap = SelectObject(hMemoryDC,C_agibmp);
                  BitBlt(hdc,cx,
                     cy,
                     scaracter.cx, scaracter.cy,
                     hMemoryDC, xmas+caso_especial_trama(cx,scaracter.cx,c+128),
                     ymas+caso_especial_trama(cy,scaracter.cy,c+128), SRCAND);
                  BitBlt(hdc,cx,
                     cy,
                     scaracter.cx, scaracter.cy,
                     hMemoryDC,xbmp+caso_especial_trama(cx,scaracter.cx,c+128),
                     ybmp+caso_especial_trama(cy,scaracter.cy,c+128), SRCPAINT);
                  SelectObject(hMemoryDC,hOldBitmap);
                  hOldBitmap = NULL;
                  
                  cx += scaracter.cx;
                  continue;
            }
         }
         
         tmp[ntext] = convierte_caracter(*(s+k));
         ntext++;
  }

  if (ntext)
  {
     tmp[ntext] = 0;
   if (ModoProfundo && (Patrib & 128))
   {
      SetRect(&espacio,cx-1+anta,cy+2,cx+(scaracter.cx * ntext)+1-posterior2,cy+scaracter.cy-2);
      FillRect(hdc,&espacio,agi_brush);
      SetRect(&espacio,cx,cy,cx+(scaracter.cx * ntext),cy+scaracter.cy);
   }
   else
   {
      SetRect(&espacio,cx,cy,cx+scaracter.cx * ntext,cy+scaracter.cy);
      FillRect(hdc,&espacio,agi_brush);
   }
     espacio.top += caracterdy;
     DrawText(hdc,tmp,ntext,&espacio,DT_TOP | DT_LEFT | DT_SINGLELINE | DT_NOPREFIX);
     espacio.top -= caracterdy;
     
     if ((Patrib & 128)/* || (Patrib & (~128)) == 'R'*/)
     {
        enmarca_texto(hdc,&espacio,((Patrib & (~128)) == 'R') ? 0 : 1,anta,posterior2);
     }
     ntext = 0;
  }
 }
 
 PCcolor = PCcolor_,PFcolor = PFcolor_;
 _PCcolor = _PCcolor_,_PFcolor = _PFcolor_;
 Patrib = aaa;
 SelectObject(hdc,oldfont);
}

/* se pone en la parte grafica de la stdpan... si la llamada es ´normal´
se rellenera luego el texto en la stdpan */
int w_winprint(s,a,m)
unsigned char *s;
unsigned char a;
int m;
{
   RECT espacio;
   int cx,cy,ff;
   unsigned char ans[300];
   unsigned char c;
   HDC hdc;
   int i,j = 0;
   char *v = stdpan->vv + Pcursor + stdpan->size*2; /* en la stdpan coincide */
   char *vg =stdpan->mapagrafico + Pcursor;
   char *qv =stdpan->vv + Pcursor;
   int kk;
   int noinicio = Pcursor%80;
   HFONT oldfont;
   int xbmp;
   int ybmp;
   int xmas;
   int ymas;
   HBRUSH hbr;
   SIZE ss;
   int tapiz;
   int ac1,ac2,ac3;
   int factormenosx =0;
   int factormasx = 0;
   int posterior = 0;
   
   if (!stdpan->visible)
   {
      if (DespachadorExterno)
      {
         (*DespachadorExterno)((int)DESPACHO_HAZ_VISIBLE,0,NULL);
      }
      stdpan->visible = 1;
   }
   
   if (!m) m = strlen(s);
   
   if ((Pcursor/80) == 24)
   {
      for (i = 0;i < m;i++)
         ans[i] = convierte_caracter(s[i]);
      ans[i] = 0;
	  if ((Pcursor%80) > 38)
	  {
         if (DespachadorExterno)
         {
            (*DespachadorExterno)((int)DESPACHO_MENSAJE_CABECERA,3,ans);
         }
	  }
	  else
	  {
		 mensaje_especial(ans);
	  }
      return(m);
   }
   
   
   
#ifdef COMENTARIO
   if ( (Pcursor/80) > 22)
   {
      hdc=stdpan->vhdc;
      cx = 3+(Pcursor%80)*scaracter.cx;
      cy = espacio_arriba+4 + scaracter.cy * 23;
      oldfont = SelectObject(stdpan->vhdc,agi_frever);
      GetTextExtentPoint(stdpan->vhdc,s,m,&ss);
      SetRect(&espacio,cx,cy,cx+ss.cx,cy+18);
      cy += (18 - ss.cy) / 2;
      SetTextColor(stdpan->vhdc,RGB(0,0,0));
      hbr = CreateSolidBrush(RGB(192,192,192));
      FillRect(hdc,&espacio,hbr);
      DeleteObject(hbr);
      for (i = 0;i < m;i++)
         ans[i] = convierte_caracter(s[i]);
      ans[i] = 0;
      ExtTextOut(stdpan->vhdc,cx,cy,ETO_CLIPPED,&espacio,ans,m,NULL);
      SelectObject(stdpan->vhdc,oldfont);
      if (!IsIconic(principal_w))
      {
         if (!NO_PINTAR_AHORA)
         {
            hdc = GetDC(principal_w);
            BitBlt(hdc,principal_rect.left+cx,principal_rect.top+cy,espacio.right - espacio.left,espacio.bottom - espacio.top,stdpan->vhdc,cx,cy,SRCCOPY);
            ReleaseDC(principal_w,hdc);
         }
      }
      return(m);
   }
#endif
   
   cx = scaracter.cx * (Pcursor%80);
   cy = espacio_arriba+scaracter.cy * (Pcursor/80);
   
   hdc=stdpan->vhdc;
   
   if ((Patrib & 128) && (Patrib & (~128)) == 'I')
   {
      PCcolor = 0;
      PFcolor = 7;
   }
   if ((Pcursor/80) == 3)
   {
      ac1 = PCcolor;
      ac2 = PFcolor;
      ac3 = Patrib;
      PCcolor = 7;
      PFcolor = 0;
      Patrib = 'N';
   }
   activa_color_texto(hdc);
   
   oldfont = selecciona_font(hdc,Patrib);
   
   for (i = 0,j = 0;i < m;i++,j++)
   {
      ans[j] = convierte_caracter(s[i]);
      *(vg+j) = 0;
      c = s[i];
      if (!c || c == 219 || c == 176 || c == 177 || c == 178 || c == 1) {
         if (j)
         {
            ans[j] = 0;
           if (ModoProfundo && (Patrib & 128))
           {
              kk = 0;
              if (noinicio && (*(v-1) & 128))
              {
                  kk = 1;
              }
              SetRect(&espacio,cx-1+kk,cy+2,cx+(scaracter.cx * j)/*+1*/,cy+scaracter.cy-2);
              FillRect(hdc,&espacio,agi_brush);
              SetRect(&espacio,cx,cy,cx+(scaracter.cx * j),cy+scaracter.cy);
              factormenosx = 2;
              factormasx = 3;
           }
           else
           {
              SetRect(&espacio,cx,cy,cx+scaracter.cx * j,cy+scaracter.cy);
              FillRect(hdc,&espacio,agi_brush);
           }
            espacio.top += caracterdy;
            DrawText(hdc,ans,j,&espacio,DT_TOP | DT_LEFT | DT_SINGLELINE | DT_NOPREFIX);
            espacio.top -= caracterdy;
            
            if ((Patrib & 128) /*|| (Patrib & (~128)) == 'R'*/)
            {
               kk = 0;
               if (noinicio && (*(v-1) & 128))
               {
                  kk = 1;
               }
               enmarca_texto(hdc,&espacio,((Patrib & (~128)) == 'R') ? 0 : 1,kk,1);
            }
            
            cx += (scaracter.cx * j);
            v += j;
            vg += j;
            qv += j;
            noinicio = 1;
            j = 0;
         }
         if (c == 1)
         {
            if (win_fondo_neutro)
            {
               SetRect(&espacio,cx,cy,cx+scaracter.cx,cy+scaracter.cy);
               FillRect(hdc,&espacio,agi_brush_gris);
            }
            else
            {
               hOldBitmap = SelectObject(hMemoryDC,agitapiz);
               BitBlt(hdc,cx,cy,scaracter.cx, scaracter.cy,hMemoryDC, cx,cy , SRCCOPY);
               SelectObject(hMemoryDC,hOldBitmap);
               hOldBitmap = NULL;
            }
         }
         else
         {
            s[i] = *qv;
         }
         cx += scaracter.cx;
         v++;
         vg++;
         qv++;
         noinicio = 1;
         j = -1;
         continue;
      }
      if (c > 127)
      {
         c -= 128;
         if (C_agigrafico[c])
         {
            if (j)
            {
               ans[j] = 0;
               if (ModoProfundo && (Patrib & 128))
               {
                  kk = 0;
                  if (noinicio && (*(v-1) & 128))
                  {
                     kk = 1;
                  }
                  SetRect(&espacio,cx-1+kk,cy+2,cx+(scaracter.cx * j)/*+1*/,cy+scaracter.cy-2);
                  FillRect(hdc,&espacio,agi_brush);
                  SetRect(&espacio,cx,cy,cx+(scaracter.cx * j),cy+scaracter.cy);
                  factormenosx = 2;
                  factormasx = 3;
               }
               else
               {
                  SetRect(&espacio,cx,cy,cx+scaracter.cx * j,cy+scaracter.cy);
                  FillRect(hdc,&espacio,agi_brush);
               }
               espacio.top += caracterdy;
               DrawText(hdc,ans,j,&espacio,DT_TOP | DT_LEFT | DT_SINGLELINE | DT_NOPREFIX);
               espacio.top -= caracterdy;
               if ((Patrib & 128)/* || (Patrib & (~128)) == 'R'*/)
               {
                  kk = 0;
                  if (noinicio && (*(v-1) & 128))
                  {
                     kk = 1;
                  }
                  enmarca_texto(hdc,&espacio,((Patrib & (~128)) == 'R') ? 0 : 1,kk,1);
               }
               cx += (scaracter.cx * j);
               v += j;
               vg += j;
               qv += j;
               noinicio = 1;
               j = 0;
            }
            
            SetRect(&espacio,cx,cy,cx+scaracter.cx,cy+scaracter.cy);
            if ( !(ff = determina_fondo_especial(stdpan,cx,cy,c+128,101,2380,&tapiz,stdpan)) || ff > 2)
            {
               FillRect(hdc,&espacio,agi_brush);
            }
            else
            {
               if (tapiz)
               {
                  // reponer el tapiz!!!! */
                  hOldBitmap = SelectObject(hMemoryDC,agitapiz);
                  BitBlt(hdc,cx,cy,scaracter.cx, scaracter.cy,hMemoryDC, cx,cy , SRCCOPY);
                  SelectObject(hMemoryDC,hOldBitmap);
                  hOldBitmap = NULL;
               }
            }
            
            if (!ff || C_agigrafico[c] == 1)
            {
               xbmp = (C_mosaico[c]-1)*14;
               ybmp = 0;
               xmas = (C_mosaico[c]-1)*14;
               ymas = 96;
            }
            else
               if (ff == 1 || ff == 2 || C_agigrafico[c] == 2)
               {
                  xbmp = (C_mosaico[c]-1)*14;
                  ybmp = 32;
                  xmas = (C_mosaico[c]-1)*14;
                  ymas = 128;
               }
               else
               {
                  xbmp = (C_mosaico[c]-1)*14;
                  ybmp = 64;
                  xmas = (C_mosaico[c]-1)*14;
                  ymas = 160;
               }
               hOldBitmap = SelectObject(hMemoryDC,C_agibmp);
               BitBlt(hdc,cx,
                  cy,
                  scaracter.cx, scaracter.cy,
                  hMemoryDC, xmas+caso_especial_trama(cx,scaracter.cx,c+128),
                  ymas+caso_especial_trama(cy,scaracter.cy,c+128), SRCAND);
               BitBlt(hdc,cx,
                  cy,
                  scaracter.cx, scaracter.cy,
                  hMemoryDC,xbmp+caso_especial_trama(cx,scaracter.cx,c+128),
                  ybmp+caso_especial_trama(cy,scaracter.cy,c+128), SRCPAINT);
               SelectObject(hMemoryDC,hOldBitmap);
               hOldBitmap = NULL;
               
               cx += scaracter.cx;
               v++;
               vg++;
               qv++;
               j--;
               noinicio = 1;
               continue;
         }
      }
      
   }
   
   if (j > 0)
   {
      ans[j] = 0;
       if (ModoProfundo && (Patrib & 128))
       {
             kk = 0;
             if (noinicio && (*(v-1) & 128))
             {
                kk = 1;
             }
             if ((noinicio+m) < 80 && (*(v+m) & 128))
             {
                 posterior = 1;
             }
             else
                 posterior = 0;
          SetRect(&espacio,cx-1+kk,cy+2,cx+(scaracter.cx * j)+1-posterior,cy+scaracter.cy-2);
          FillRect(hdc,&espacio,agi_brush);
          SetRect(&espacio,cx,cy,cx+(scaracter.cx * j),cy+scaracter.cy);
          factormenosx = 2;
          factormasx = 3;
       }
       else
       {
          SetRect(&espacio,cx,cy,cx+scaracter.cx * j,cy+scaracter.cy);
          FillRect(hdc,&espacio,agi_brush);
       }
      espacio.top += caracterdy;
      DrawText(hdc,ans,j,&espacio,DT_TOP | DT_LEFT | DT_SINGLELINE | DT_NOPREFIX);
      espacio.top -= caracterdy;
      if ((Patrib & 128) /*|| (Patrib & (~128)) == 'R'*/)
      {
         kk = 0;
         if (noinicio && (*(v-1) & 128))
         {
            kk = 1;
         }
         if ((noinicio+m) < 80 && (*(v+m) & 128))
         {
             posterior = 1;
         }
         else
             posterior = 0;
         enmarca_texto(hdc,&espacio,((Patrib & (~128)) == 'R') ? 0 : 1,kk,posterior);
      }
   }
   
   cx = scaracter.cx * (Pcursor%80);
   cy = espacio_arriba+scaracter.cy * (Pcursor/80);
   if (!IsIconic(principal_w))
   {
      if (!NO_PINTAR_AHORA)
      {
         hdc = GetDC(principal_w);
         BitBlt(hdc,principal_rect.left+cx-factormenosx,principal_rect.top+cy,scaracter.cx*m + factormenosx + factormasx,scaracter.cy,stdpan->vhdc,cx-factormenosx,cy,SRCCOPY);
         ReleaseDC(principal_w,hdc);
      }
   }
   if ((Pcursor/80) == 3)
   {
      PCcolor = ac1;
      PFcolor = ac2;
      Patrib  = ac3;
   }
   
   return(m);
}


int curon(i)
int i;
{
   unsigned char a,b;
   static int cur = 1;
   
   if (i == -1)
      return(cur);
   
   if (i==0) {
      a=1;
      b=0;
      if (cur)
      {
         HideCaret(principal_w);
         DestroyCaret();
      }
      cur = 0;
   }
   else if (i==1) {
      a=6;
      b=7;
      DestroyCaret();
      CreateCaret(principal_w,0,scaracter.cx,scaracter.cy);
      SetCaretPos(cursorx,cursory);
      ShowCaret(principal_w);
      cur = 1;
   }
   else if (i==2) {
      a=2;
      b=6;
      DestroyCaret();
      CreateCaret(principal_w,(HBITMAP)1,scaracter.cx,scaracter.cy);
      SetCaretPos(cursorx,cursory);
      ShowCaret(principal_w);
      cur = 2;
   }
   else
      return(-1);
   return(0);
}


static int i_pos = 0;

static int es_impre_error(erres)
int erres;
{
   switch(erres)
			{
			case SP_ERROR:
            return(1);
         case SP_OUTOFDISK:
            return(2);
         case SP_OUTOFMEMORY:
            return(3);
         case SP_USERABORT:
            return(4);
         default:
            break;
			}
   return(0);
}

int ini_w_printer(int m)
{
			extern unsigned char inicializo[37];
         extern int i_nlineas;
         // si m coger impresora por defecto
         if (DespachadorExterno)
         {
            unsigned char tmp[512];
            unsigned short LLength = inicializo[35];
            memcpy(tmp,&m,sizeof(int));
            memcpy(tmp+sizeof(int),&LLength,sizeof(short));
            memcpy(tmp+sizeof(int)+sizeof(short),inicializo,LLength);
            (*DespachadorExterno)((int)DESPACHO_FUNCIONES_IMPRESION,1,(void *)tmp);
            inicializo[0] = 0;
            inicializo[35] = 0;
            memcpy(&m,tmp,sizeof(int));
            if (m < 0)
               return -1;
            if (m)
            {
               i_nlineas = m;
               memcpy(&m,tmp+sizeof(int),sizeof(int));
               i_anchocarro = m;
            }
            return(0);
         }
         return(-1);
}

int printa_w_linea(unsigned char *szLine,unsigned short LLength)
{
			if (DespachadorExterno)
         {	 
            unsigned char tmp[2048];
            memcpy(tmp,&LLength,sizeof(short));
            memcpy(tmp+sizeof(short),szLine,LLength);
            (*DespachadorExterno)((int)DESPACHO_FUNCIONES_IMPRESION,2,(void *)tmp);
            return(*((int *)tmp));
         }
         return(-1);
}

int fin_w_printer()
{
			if (DespachadorExterno)
         {
            int m = 0;	 
            (*DespachadorExterno)((int)DESPACHO_FUNCIONES_IMPRESION,3,(void *)&m);
            return(m);
         }
         return(-1);
}

int printa_grafico(FileName)
LPSTR FileName;
{
			if (DespachadorExterno)
         {
            int m = -1;	 
            (*DespachadorExterno)((int)DESPACHO_FUNCIONES_IMPRESION,4,(void *)FileName);
            return(m);
         }
         return(-1);
}


#ifdef COMENTARIO
int old_ini_w_printer(m)
{
   LPDEVMODE   lpDevMode = NULL;
   LPDEVNAMES  lpDevNames;
   LPSTR       lpszDriverName = NULL;
   LPSTR       lpszDeviceName = NULL;
   LPSTR       lpszPortName = NULL;
   TEXTMETRIC TextMetric;
   DOCINFO di;
   BOOL bola = FALSE;
   int nPageSize;
   HFONT kakafont;
   LOGFONT FontRec;
   int i;
   int erres;
   char tmp[81];
   
   
   if (hPr) fin_w_printer();
   
   memset(&pd,0,sizeof(pd));
   pd.lStructSize    = sizeof(pd);
   pd.hwndOwner      = principal_w;
   pd.hDevMode       = NULL;
   pd.hDevNames      = NULL;
   if (m)
      pd.Flags          = PD_RETURNDC |  PD_RETURNDEFAULT;
   else
      pd.Flags          = PD_RETURNDC |  PD_NOPAGENUMS | PD_ALLPAGES | PD_USEDEVMODECOPIES; // | PD_RETURNDEFAULT; | PD_RETURNDEFAULT; //
   pd.nCopies        = 1;
   
   
   if (PrintDlg((LPPRINTDLG)&pd))
   {
      if (pd.hDC)
						{		     
         hPr = pd.hDC;
						}
      else
						{			 
         if (pd.hDevNames)
         {			  
            lpDevNames = (LPDEVNAMES)GlobalLock(pd.hDevNames);
            lpszDriverName = (LPSTR)lpDevNames + lpDevNames->wDriverOffset;
            lpszDeviceName = (LPSTR)lpDevNames + lpDevNames->wDeviceOffset;
            lpszPortName   = (LPSTR)lpDevNames + lpDevNames->wOutputOffset;
            GlobalUnlock(pd.hDevNames);
            if (pd.hDevMode)
               lpDevMode = (LPDEVMODE)GlobalLock(pd.hDevMode);
            hPr = CreateDC(lpszDriverName, lpszDeviceName, lpszPortName, (LPSTR)lpDevMode);
            if (pd.hDevMode && lpDevMode)
               GlobalUnlock(pd.hDevMode);
         }
         
         if (pd.hDevNames)
         {
            GlobalFree(pd.hDevNames);
            pd.hDevNames=NULL;
         }
         if (pd.hDevMode)
         {
            GlobalFree(pd.hDevMode);
            pd.hDevMode=NULL;
         }
						}
   }
   if (!hPr)
   {
      return(-1);
   }
   
   
   lpAbortDlg =  MakeProcInstance(AbortDlg, hInst);
   lpAbortProc = MakeProcInstance(AbortProc, hInst);
   bAbort = FALSE;
   hAbortDlgWnd = CreateDialog(hInst, "ABORTDLG",principal_w, lpAbortDlg);
   if (!hAbortDlgWnd)
   {
      MessageBox(principal_w /* GetParent(principal_w)*/, "NULL Abort window handle",
         NULL, MB_OK | MB_ICONHAND);
      FreeProcInstance(lpAbortDlg);
      FreeProcInstance(lpAbortProc);
      DeleteDC(hPr);
      hPr = NULL;
      return (-1);
   }
   
   
   SetAbortProc (hPr, AbortProc);
   
   memset(&di,0,sizeof(di));
   di.cbSize      = sizeof(DOCINFO);
   di.lpszDocName = "Impresion";
   di.lpszOutput  = NULL;
   if (StartDoc  (hPr, &di) < 1)
   {
      sprintf(tmp,__Texto[398],GetLastError());
      MessageBox(principal_w /*GetParent(principal_w)*/,tmp,NULL, MB_OK | MB_ICONHAND);
      DestroyWindow(hAbortDlgWnd);
      FreeProcInstance(lpAbortDlg);
      FreeProcInstance(lpAbortProc);
      
      DeleteDC(hPr);
      hPr = NULL;
      return(-1);
   }
   /*
   
     erres = Escape(hPr, SETABORTPROC, (int)NULL,(LPSTR)lpAbortProc,(LPSTR)NULL);
     
						 if (es_impre_error(erres))
                   {
                   sprintf(tmp,"[%d]",GetLastError());
                   MessageBox(principal_w,tmp,NULL, MB_OK | MB_ICONHAND);
                   sprintf(tmp,"No se puede comenzar la rutina de impresion [%d]",es_impre_error(erres));
                   MessageBox(principal_w,tmp,NULL, MB_OK | MB_ICONHAND);
                   }
                   
                     erres = Escape(hPr, STARTDOC, 10, "Impresion",(LPSTR)NULL);
                     
                       
                         if (es_impre_error(erres)) 
                         {
                         sleep(1);
                         erres = Escape(hPr, STARTDOC, 10, "Impresion",(LPSTR)NULL);
                         }
                         
                           if (es_impre_error(erres)) 
                           {
                           sprintf(tmp,"[%d]",GetLastError());
                           MessageBox(principal_w,tmp,NULL, MB_OK | MB_ICONHAND);
                           if (GetLastError() == ERROR_INVALID_PARAMETER)
                           {
                           MessageBox(principal_w,"PARAMETRO INVALIDO",NULL, MB_OK | MB_ICONHAND);
                           }
                           else
                           {
                           MessageBox(principal_w,"SEGUIMOS PERDIDOS",NULL, MB_OK | MB_ICONHAND);
                           }
                           
                             sprintf(tmp,"No se puede comenzar el proceso de impresion [%d]",es_impre_error(erres));
                             MessageBox(principal_w,tmp,NULL, MB_OK | MB_ICONHAND);
                             
                               DestroyWindow(hAbortDlgWnd);
                               FreeProcInstance(lpAbortDlg);
                               FreeProcInstance(lpAbortProc);
                               
                                 DeleteDC(hPr);
                                 hPr = NULL;
                                 return(-1);
                                 }
   */
   
   ShowWindow (hAbortDlgWnd, SW_NORMAL);
   
   Escape(hPr, SAVE_CTM, (int)NULL, NULL, NULL);
   
   SetMapMode(hPr,MM_TEXT);
   SetBkMode(hPr,TRANSPARENT);
   SetTextAlign(hPr,TA_NOUPDATECP | TA_LEFT );
   StartPage (hPr);
   
   PcxInch = GetDeviceCaps (hPr, LOGPIXELSX);
   PcyInch = GetDeviceCaps (hPr, LOGPIXELSY);
   
   FontRec.lfWidth = PcxInch / 10;
   FontRec.lfHeight = 0;
   FontRec.lfEscapement = 0;
   FontRec.lfOrientation = 0;
   FontRec.lfWeight = 0;
   FontRec.lfItalic = 0;
   FontRec.lfUnderline = 0;
   FontRec.lfStrikeOut = 0;
   FontRec.lfCharSet = 0;
   FontRec.lfOutPrecision = 0;
   FontRec.lfClipPrecision = 0;
   FontRec.lfQuality = 0;
   FontRec.lfPitchAndFamily = FIXED_PITCH;
   Escape(hPr, GETFACENAME, (int)NULL,NULL ,FontRec.lfFaceName);
   pr_f_n = CreateFontIndirect(&FontRec);
   
   FontRec.lfWidth = PcxInch / 17;
   FontRec.lfHeight = 0;
   FontRec.lfEscapement = 0;
   FontRec.lfOrientation = 0;
   FontRec.lfWeight = 0;
   FontRec.lfItalic = 0;
   FontRec.lfUnderline = 0;
   FontRec.lfStrikeOut = 0;
   FontRec.lfCharSet = 0;
   FontRec.lfOutPrecision = 0;
   FontRec.lfClipPrecision = 0;
   FontRec.lfQuality = 0;
   FontRec.lfPitchAndFamily = FIXED_PITCH;
   Escape(hPr, GETFACENAME, (int)NULL,NULL ,FontRec.lfFaceName);
   pr_f_c = CreateFontIndirect(&FontRec);
   
   FontRec.lfWidth = PcxInch / 5;
   FontRec.lfHeight = 0;
   FontRec.lfEscapement = 0;
   FontRec.lfOrientation = 0;
   FontRec.lfWeight = 0;
   FontRec.lfItalic = 0;
   FontRec.lfUnderline = 0;
   FontRec.lfStrikeOut = 0;
   FontRec.lfCharSet = 0;
   FontRec.lfOutPrecision = 0;
   FontRec.lfClipPrecision = 0;
   FontRec.lfQuality = 0;
   FontRec.lfPitchAndFamily = FIXED_PITCH;
   Escape(hPr, GETFACENAME, (int)NULL,NULL ,FontRec.lfFaceName);
   pr_f_e = CreateFontIndirect(&FontRec);
   
   
   FontRec.lfWidth = PcxInch / 10;
   FontRec.lfHeight = 0;
   FontRec.lfEscapement = 0;
   FontRec.lfOrientation = 0;
   FontRec.lfWeight = FW_BOLD;
   FontRec.lfItalic = 0;
   FontRec.lfUnderline = 0;
   FontRec.lfStrikeOut = 0;
   FontRec.lfCharSet = 0;
   FontRec.lfOutPrecision = 0;
   FontRec.lfClipPrecision = 0;
   FontRec.lfQuality = 0;
   FontRec.lfPitchAndFamily = FIXED_PITCH;
   Escape(hPr, GETFACENAME, (int)NULL,NULL ,FontRec.lfFaceName);
   pr_f_b = CreateFontIndirect(&FontRec);
   
   
   old_pr_f = SelectObject( hPr, pr_f_n );
   
   GetTextMetrics(hPr, &TextMetric);
   LineSpace = TextMetric.tmHeight+TextMetric.tmExternalLeading;
   nPageSize = GetDeviceCaps (hPr, VERTRES);
   LinesPerPage = nPageSize / LineSpace;
   Carlen = TextMetric.tmMaxCharWidth;
   i_anchocarro = GetDeviceCaps (hPr, HORZRES) / TextMetric.tmMaxCharWidth;
   i_nlineas = LinesPerPage;
   CurrentLine = 1;
   i_pos = 0;
   
   FontRec.lfWidth = TextMetric.tmMaxCharWidth;
   FontRec.lfHeight = TextMetric.tmHeight;
   
   /*
   ABSOLETE:
   
     ABORTDOC	Stops the current print job and erases everything the application has written to the device since the last ENDDOC escape.
     ENDDOC	Ends a print job started by the STARTDOC escape.
     GETPHYSPAGESIZE	Retrieves the physical page size and copies it to the specified location.
     GETPRINTINGOFFSET	Retrieves the offset from the upper-left corner of the physical page where the actual printing or drawing begins.
     GETSCALINGFACTOR	Retrieves the scaling factors for the x-axis and the y-axis of a printer.
     NEWFRAME	Informs the printer that the application has finished writing to a page.
     NEXTBAND	Informs the printer that the application has finished writing to a band.
     PASSTHROUGH	Allows the application to send data directly to a printer.
     SETABORTPROC	Sets the Abort function for a print job.
     STARTDOC
   */
   
   return(0);
}


int old_printa_w_linea(szLine,LLength)
unsigned char *szLine;
unsigned short LLength;
{
   char tmp[500];
   int Status;
   int nn,i;
   RECT rc;
   TEXTMETRIC TextMetric;
   
   if (!hPr) return(-1);
   
   i = 0;
   for (nn = 0;nn < LLength;nn++)
   {
      if (szLine[nn] < 32)
      {
         if (i)
         {
            SetRect(&rc,i_pos,(CurrentLine+1)*LineSpace,Carlen*i,(CurrentLine+1)*LineSpace);
            ExtTextOut(hPr, i_pos, CurrentLine*LineSpace ,0,&rc,(LPSTR)tmp,i,(LPINT)NULL);
            i_pos += (i*Carlen);
            i = 0;
         }
         switch(szLine[nn])
         {
         case 13:
            i_pos = 0;
            break;
         case 10:
            i_pos = 0;
            if (++CurrentLine > LinesPerPage )
            {
               CurrentLine = 1;
               Status = EndPage(hPr);
               /* Status = Escape(hPr, NEWFRAME, 0, 0L, 0L); */
               if (Status < 1 || bAbort)
               {
                  return(-1);
               }
            }
            break;
         case 15:
            SelectObject( hPr, pr_f_c );
            GetTextMetrics(hPr, &TextMetric);
            Carlen = TextMetric.tmMaxCharWidth;
            break;
         case 18:
            SelectObject( hPr, pr_f_n );
            GetTextMetrics(hPr, &TextMetric);
            Carlen = TextMetric.tmMaxCharWidth;
            break;
         case 19:
            SelectObject( hPr, pr_f_b );
            GetTextMetrics(hPr, &TextMetric);
            Carlen = TextMetric.tmMaxCharWidth;
            break;
         case 14:
            SelectObject( hPr, pr_f_e );
            GetTextMetrics(hPr, &TextMetric);
            Carlen = TextMetric.tmMaxCharWidth;
            break;
         case 12:
            CurrentLine = 1;
            i_pos = 0;
            Status = EndPage(hPr);
            /* Status = Escape(hPr, NEWFRAME, 0, 0L, 0L); */
            if (Status < 1 || bAbort)
            {
               return(-1);
            }
            break;
         default:
            break;
         }
      }
      else
      {
         tmp[i] = szLine[nn];
         i++;
      }
   }
   if (i)
   {
      SetRect(&rc,i_pos,(CurrentLine+1)*LineSpace,Carlen*i,(CurrentLine+1)*LineSpace);
      ExtTextOut(hPr, i_pos, CurrentLine*LineSpace ,0,&rc,(LPSTR)tmp,i,(LPINT)NULL);
      i_pos += (i * Carlen);
   }
   
   return(0);
}


int old_fin_w_printer()
{
   
   if (hPr)
			{
      
      if (!bAbort)
      {
         if (CurrentLine > 1)
         {
            /* Escape(hPr, NEWFRAME, 0, 0L, 0L); */
            EndPage(hPr);
         }
         /* Escape(hPr, ENDDOC, 0, 0L, 0L); */
         EndDoc(hPr);
         CurrentLine = 1;
      }
      SelectObject( hPr, old_pr_f );
      DeleteObject( pr_f_n );
      DeleteObject( pr_f_c );
      DeleteObject( pr_f_e );
      DeleteObject( pr_f_b );
      pr_f_n = NULL;
      pr_f_c = NULL;
      pr_f_e = NULL;
      pr_f_b = NULL;
      Escape(hPr, RESTORE_CTM, (int)NULL, NULL, NULL);
      EnableWindow(principal_w, TRUE);
      DestroyWindow(hAbortDlgWnd);
      FreeProcInstance(lpAbortDlg);
      FreeProcInstance(lpAbortProc);
      
      DeleteDC(hPr);
      hPr = NULL;
      return(0);
			}
   return(-1);
}
#endif

extern char *tecla_funcion();

int mensaje_de_boton(tmp,id)
char *tmp;
int id;
{
   char *p;
   if (id >= 823 && id <= 842)
			{
      p = desc_funs[id-823];// tecla_funcion(id-823,NULL,1);
      if (p)
      {
         quitab(p);
         if (!p[0])
            p = NULL;
      }
      if (p)
      {
         sprintf(tmp,__Texto[399],id-822,p);
      }
      else
         if (id < 833)
            sprintf(tmp,__Texto[400],id-822,id-822);
         else
            sprintf(tmp,__Texto[401],id-822,id-832);
			}
   else
      if (id == 801)
         strcpy(tmp,__Texto[402]);
      else
         if (id == 808)
            strcpy(tmp,__Texto[403]);
         else
            if (id == 809)
               strcpy(tmp,__Texto[404]);
            else
               if (id == 807)
                  strcpy(tmp,__Texto[405]);
               else
                  if (id == 821)
                     strcpy(tmp,__Texto[406]);
                  else
                     if (id == 820)
                        strcpy(tmp,__Texto[407]);
                     else
                        if (id == 815)
                           strcpy(tmp,__Texto[408]);
                        else
                           if (id == 814)
                              strcpy(tmp,__Texto[409]);
                           else
                              if (id == 822)
                                 strcpy(tmp,__Texto[410]);
                              else
                                 if (id == 1100)
                                    strcpy(tmp,__Texto[411]);
                                 else
                                    if (id >= 1000 && id < 1100)
                                       strcpy(tmp,__Texto[412]);
                                    else
                                       tmp[0] = 0;
}

static int cosa;

static HGLOBAL CreaClipData(RECT Rect)
{
			HGLOBAL hdata;
         LPSTR lpclip;
         VENT *w = stdpan;
         int ini = stdpan->vini;
         int fin  = stdpan->vfin;
         unsigned char *s=w->vv;
         register int i,j;
         int tt;
         int alto;
         int ancho;
         int cini;
         int fini;
         int iini;
         int jini;
         int ifin;
         int jfin;
         int k,ff;
         int g,oa,og;
         int b;
         unsigned char tmp[81];
         int ntext = 0;
         unsigned char c,a;
         int cx,cy;
         RECT espacio,norect;
         int xbmp;
         int ybmp;
         int xmas;
         int ymas;
         SIZE ss;
         int anta;
         char aloja[5000];
         int naloja;
         int campos;
         
         if ((ini/100) < 5)
         {
            ini = 500 + ini%100;
         }
         alto  = (w->vfin)/100 - (w->vini)/100 + 1;
         ancho = (w->vfin)%100 - (w->vini)%100 + 1;
         cini = (ini % 100) - 1;
         fini = (ini / 100) - 1;
         iini = (ini / 100) - (w->vini / 100);
         jini = (ini % 100) - (w->vini % 100);
         ifin =  (fin)/100 - (w->vini)/100 + 1;
         jfin = (fin)%100 - (w->vini)%100 + 1;
         b=alto*ancho;
         
         espacio.top = 0;
         espacio.left = 0;
         espacio.bottom = scaracter.cy;
         
         tmp[1] = 0;
         
         oa = -1;
         og = -1;
         
         cy = ((w == stdpan) ? espacio_arriba : 0)+fini * scaracter.cy;
         cx = cini * scaracter.cx;
         
         naloja = 0;
         aloja[0] = 0;
         for (i=iini;i<ifin;i++,cy += scaracter.cy)
         {
            cx = cini * scaracter.cx;
            anta = 0;
            campos = 0;
            for (j=jini;j<jfin;j++)
            {
               k = i*ancho+j; /* desplazamiento corriente */
               g = *(s+b+k); /* COLOR */
               a = *(s+2*b+k); /* Atributo */
               if (!ntext)
               {
                  if (j)
                     anta = ((*(s+2*b+k-1) & 128) ? 1 : 0);
                  else
                     anta = 0;
               }
               if (oa != a || og != g)
               {
                  if (ntext)
                  {
                     if ((oa & 128))
                     {     
                        tmp[ntext] = 0;
                        SetRect(&espacio,cx,cy,cx+(scaracter.cx * ntext),cy+scaracter.cy);
                        if (IntersectRect(&norect,&Rect,&espacio))
                        {
                           if (campos)
                           {
                              aloja[naloja] = 9;
                              naloja++;
                           }
                           strcpy(aloja+naloja,tmp);
                           naloja+=ntext;
                           campos = 1;
                        }			  
                        //espacio.top += caracterdy;
                        //DrawText(hdc,tmp,ntext,&espacio,DT_TOP | DT_LEFT | DT_SINGLELINE | DT_NOPREFIX);
                        //espacio.top -= caracterdy;
                     }
                     cx += (scaracter.cx * ntext);
                     ntext = 0;
                  }
                  oa = a;
                  og = g;
               }	
               tmp[ntext] = convierte_caracter(*(s+k));
               ntext++;
            }
            if (ntext)
            {
               if ((oa & 128))
               {     
                  tmp[ntext] = 0;
                  SetRect(&espacio,cx,cy,cx+scaracter.cx * ntext,cy+scaracter.cy);	      
                  if (IntersectRect(&norect,&Rect,&espacio))
                  {
                     if (campos)
                     {
                        aloja[naloja] = 9;
                        naloja++;
                     }
                     strcpy(aloja+naloja,tmp);
                     naloja+=ntext;
                     campos = 1;
                  }			  		  
                  //espacio.top += caracterdy;
                  //DrawText(hdc,tmp,ntext,&espacio,DT_TOP | DT_LEFT | DT_SINGLELINE | DT_NOPREFIX);
                  //espacio.top -= caracterdy;
               }
               cx += (scaracter.cx * ntext);
               ntext = 0;
            }	 
            SetRect(&espacio,cini * scaracter.cx,cy,cx+scaracter.cx,cy+scaracter.cy);
            if (campos || IntersectRect(&norect,&Rect,&espacio))
            {
               aloja[naloja] = 0;
               strcat(aloja,"\n");
               naloja = strlen(aloja);
            }
         }
         
         aloja[naloja] = 0;
         naloja++;
         hdata = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE,naloja);
         if (hdata)
         {
            lpclip = GlobalLock(hdata);
            if (lpclip)
            {
               memcpy(lpclip,aloja,naloja);
               GlobalUnlock(hdata);
            }
         }
         return(hdata);
}

static RECT Rect;
static MPOINT ptc;
static BOOL bTrack = FALSE;
static INT Shape = SL_BOX;                          /* Shape to use for rectangle */
static BOOL RetainShape = FALSE;                      /* Retain or destroy shape    */
static BOOL EnDrag = FALSE;
static BOOL HaySeleccion = FALSE;

static int abajo = 0;

DS_WINSYS_API void WINAPI pinta_zona(HDC hdc,RECT zona,VENT *pan)
{
   int i,j,x,y;
   
   if (!pan)
   {
      pan = stdpan;
      if (!pan)
         return;
   }
   
   i = zona.left;
   if (i < principal_rect.left)
      i = principal_rect.left;
   j = zona.top;
   if (j < principal_rect.top)
      j = principal_rect.top;
   x = zona.left - principal_rect.left;
   if (x < 0) x = 0;
   y = zona.top - principal_rect.top;
   if (y < 0) y = 0;
   BitBlt(hdc,i-1,j-1,zona.right-i+3,zona.bottom-j+3,pan->vhdc,x-1,y-1,SRCCOPY);
}



DS_WINSYS_API void WINAPI mensaje_char(UINT nChar, UINT nRepCnt, UINT nFlags)
{
   if (HaySeleccion == TRUE)
   {
      ClearSelection(principal_w, &Rect, (nChar & MK_SHIFT) ? SL_EXTEND | Shape : Shape);
      SetRectEmpty(&Rect);
      HaySeleccion = FALSE;
   }
   if (pulsa_en_espacio != -1 && nChar == ' ') return;
   if (nChar > 0 && nChar < 256)
   {		
      salida_caracter = nChar;
   }
}

DS_WINSYS_API void WINAPI mensaje_keydown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	  if (pulsa_en_espacio > -1 && nChar == ' ' && boton_actual != pulsa_en_espacio)
     {
        if (boton_actual)		 
           boton_abajo(boton_actual);
        boton_actual = pulsa_en_espacio;
        boton_abajo(boton_actual);
        pulsa_en_espacio = -2;
     }
     posible_char = 0;	  
}


DS_WINSYS_API void WINAPI mensaje_keyup(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	  if (pulsa_en_espacio == -2)
     {
        pulsa_en_espacio = boton_actual;
        boton_arriba(boton_actual);
        boton_actual = 0;
        salida_caracter = ' ';
     }
     posible_char = 1;	     
}


DS_WINSYS_API void WINAPI mensaje_ldblclk(UINT x,UINT y,UINT Flags)
{
   int i;
   if (boton_actual)
      boton_arriba(boton_actual);
   if ((i = esta_en_icono(x,y)))
   {
      if (i == 1)
         PostMessage(principal_w,WM_COMMAND,806,(LPARAM)NULL);
      else
         PostMessage(principal_w,WM_CHAR,i-2+'A',(LPARAM)NULL);
   }
   else
      if ((boton_actual = esta_en_boton(x,y)))
      {
         boton_abajo(boton_actual);
         boton_arriba(boton_actual);
         if (agi_boton_id[boton_actual-1])
            PostMessage(principal_w,WM_COMMAND,agi_boton_id[boton_actual-1],(LPARAM)NULL);
         boton_actual = 0;
      }
}

static POINTS pi_c = {0,0};

DS_WINSYS_API void WINAPI mensaje_lbuttondown(UINT x,UINT y,UINT Flags)
{
   int i;
   
   pi_c.x = x;
   pi_c.y = y;
   SetFocus(principal_w);	 
 	 if (HaySeleccion == TRUE)
    {
       POINT pt;
       pt.x = x;
       pt.y = y;
       if (PtInRect(&Rect,pt) != FALSE)
       {
          hData = CreaClipData(Rect);
          EnDrag = TRUE;		 
          SetCapture(principal_w);
          AgiDoDrag(hData);
          ReleaseCapture();
          EnDrag = FALSE;
          ClearSelection(principal_w, &Rect, (Flags & MK_SHIFT) ? SL_EXTEND | Shape : Shape);
          SetRectEmpty(&Rect);
          HaySeleccion = FALSE;           
          return;
       }
       else
       {
          ClearSelection(principal_w, &Rect, (Flags & MK_SHIFT) ? SL_EXTEND | Shape : Shape);
          SetRectEmpty(&Rect);
          HaySeleccion == FALSE;
       }
    }
    if (boton_actual)
       boton_arriba(boton_actual);
    if ((i = esta_en_icono(x,y)))
    {
       if (i == 1)
          PostMessage(principal_w,WM_COMMAND,806,(LPARAM)NULL);
       else
          PostMessage(principal_w,WM_CHAR,i-2+'A',(LPARAM)NULL);
    }
    else
       if ((boton_actual = esta_en_boton(x,y)))
       {
          boton_abajo(boton_actual);
          if (tipo_cursor != 6)
          {
             DsSetCursor(cur_pulsado);
             tipo_cursor = 6;
          }
       }
       else
       {
          abajo = 1;
          ptc.x = x;
          ptc.y = y;
       }	 
}


DS_WINSYS_API void WINAPI mensaje_mousemove(UINT x1,UINT y1,UINT Flags)
{
   int ti,x,estado;
   char tmp[512];
   
   
   if (EnDrag == TRUE) return;
   if (0 && abajo == 1 && !bTrack && (Flags & MK_LBUTTON) /*&& ((Flags & MK_CONTROL) || (Flags & MK_SHIFT))*/)
   {
      bTrack = TRUE;
      ClearSelection(principal_w, &Rect, (Flags & MK_SHIFT) ? SL_EXTEND | Shape : Shape);
      SetRectEmpty(&Rect);
      StartSelection(principal_w, ptc, &Rect,
         (Flags & MK_SHIFT) ? SL_EXTEND | Shape : Shape);
      abajo = 0;
      return;
   }	
   if (bTrack)
   {
      POINTS p;
      p.x = x1;
      p.y = y1;
      UpdateSelection(principal_w, p , &Rect, Shape);
      return;
   }	
   
   pi_c.x = x1;
   pi_c.y = y1;
   
   if (!cur_ini)
   {
      cur_ini = DsSetCursor(cur_flecha);
      tipo_cursor = 1;
   }
   ti = esta_en_icono(x1,y1);
   if (ti)
   {
      if (tipo_cursor != 3)
      {
         DsSetCursor(cur_boton);
         tipo_cursor = 3;
      }
      mensaje_especial(__Texto[217]);
      return; // nada mas
   }
   ti = esta_en_boton(x1,y1);
   if (boton_actual)
   {
      if (boton_actual != ti)
      {
         boton_arriba(boton_actual);
         boton_actual = 0;
      }
   }
   if (ti)
   {
      if (tipo_cursor != 3)
      {
         DsSetCursor(cur_boton);
         tipo_cursor = 3;
      }
      mensaje_de_boton(tmp,agi_boton_id[ti-1]);
      mensaje_especial(tmp);
   }
   else
   {
      x = (((x1-principal_rect.left) / scaracter.cx) + 1) + (( ((y1-principal_rect.top-espacio_arriba) / scaracter.cy)+1)*100);
      if (esta_en_el_raton(x,&estado) != -1)
      {
         if (!estado) // caso campo
         {
            if (tipo_cursor != 2)
            {
               DsSetCursor(cur_mano);
               tipo_cursor = 2;
            }
            mensaje_especial("");
         }
         else
         {
            if (tipo_cursor != 3)
            {
               DsSetCursor(cur_boton);
               tipo_cursor = 3;
            }
            mensaje_especial(__Texto[218]);
         }
      }
      else
      {
         if (x < 501 || x > 2380)
         {
            if (tipo_cursor != 1)
            {
               DsSetCursor(cur_flecha);
               tipo_cursor = 1;
            }
         }
         else
         {
            if (tipo_cursor != 4)
            {
               DsSetCursor(cur_prohibido);
               tipo_cursor = 4;
            }
         }
         mensaje_especial("");
      }
   }
}

DS_WINSYS_API void WINAPI mensaje_lbuttonup(UINT x,UINT y,UINT Flags)
{
   if (EnDrag == TRUE)
   {
      EnDrag = FALSE;
      ReleaseCapture();	   
      abajo = 0;
      return;
   }
   
   if (bTrack) 
   {
      POINTS p;
      p.x = x;
      p.y = y;
      EndSelection(p, &Rect);
      bTrack = FALSE;
      abajo = 0;
      HaySeleccion = TRUE;
      /* hacer el drag & drop */
      //ClearSelection(hWnd, &Rect, (wParam & MK_SHIFT) ? SL_EXTEND | Shape : Shape);
      //SetRectEmpty(&Rect);       
      return;
   }
   
   if (boton_actual)
   {
      boton_arriba(boton_actual);
      if (boton_actual == esta_en_boton(x,y) && agi_boton_id[boton_actual-1])
      {
         posible_char = 1;
         PostMessage(principal_w,WM_COMMAND,agi_boton_id[boton_actual-1],(LPARAM)NULL);
         if (tipo_cursor != 3)
         {
            DsSetCursor(cur_boton);
            tipo_cursor = 3;
         }
      }
      boton_actual = 0;
      abajo = 0;
   }
   else
      if (abajo == 1)
      {
         //if (_ID_Cursor == -1)
         //  {
         x = (( (x-principal_rect.left) / scaracter.cx) + 1) + (( ((y-principal_rect.top-espacio_arriba) / scaracter.cy)+1)*100);
         if ((_ID_Cursor = esta_en_el_raton(x,NULL)) != -1)
         {
            salida_caracter = SOLICITA_POSICION;
            _M_Cursor = x;
            return;
         }
         //  }
         abajo = 0;
      }
}


extern void (*f_disparo)();

DS_WINSYS_API BOOL WINAPI mensaje_command(WPARAM __wParam,LPARAM lParam)
{
   int i,j,x,y;
   short wParam;
   wParam = (short)__wParam;
/*
   if (HaySeleccion == TRUE && wParam != MID_XCLIPCOPY)
   {
      ClearSelection(principal_w, &Rect, (wParam & MK_SHIFT) ? SL_EXTEND | Shape : Shape);
      SetRectEmpty(&Rect);
      HaySeleccion = FALSE;
   }
   
*/   
	  switch(wParam)
     {
     case MID_XACERCA:
        cmAcerca();
        return TRUE;
        break;
     case MID_XOPCION1:
        pito(184);
        DialogBoxParam(hInst, "WAGINODISPONE", principal_w /*GetParent(principal_w)*/, agilpDlgProcx,(LPARAM)" CONFIGURACION");
        busca_repintado();
        return TRUE;
        break;
     case MID_XCLIPCOPY:
        if (hData)
        {
           if (OpenClipboard(principal_w))
           {
              EmptyClipboard();
              CloseClipboard();
              GlobalFree(hData);
              hData = NULL;
           }
           else
           {
              return TRUE;
              break;
           }
        }
        if (HaySeleccion == TRUE)
        {
           hData = CreaClipData(Rect);
           if (OpenClipboard(principal_w))
           {
              EmptyClipboard();
              SetClipboardData(CF_TEXT, hData);
              CloseClipboard();
           }
           else
           {
              GlobalFree(hData);
              hData = NULL;
           }
           ClearSelection(principal_w, &Rect, (wParam & MK_SHIFT) ? SL_EXTEND | Shape : Shape);
           SetRectEmpty(&Rect);
           HaySeleccion = FALSE;
           return TRUE;
           break;
        }
        if (!dato_entra) {return TRUE;break;}
        hData = GlobalAlloc(GMEM_MOVEABLE | GMEM_SHARE,dato_entra_len+1);
        lpClipData = GlobalLock(hData);
        memcpy(lpClipData,dato_entra,dato_entra_len+1);
        GlobalUnlock(hData);
        if (OpenClipboard(principal_w))
        {
           EmptyClipboard();
           SetClipboardData(CF_TEXT, hData);
           CloseClipboard();
        }
        return TRUE;
        break;
     case MID_XCLIPPASTE:
           if (HaySeleccion == TRUE && wParam != MID_XCLIPCOPY)
           {
              ClearSelection(principal_w, &Rect, (wParam & MK_SHIFT) ? SL_EXTEND | Shape : Shape);
              SetRectEmpty(&Rect);
              HaySeleccion = FALSE;
           }
        if (!dato_entra) {return TRUE;break;}
        if (OpenClipboard(principal_w))
        {
           j = CountClipboardFormats();
           
           
           if (!(hClipData = GetClipboardData(CF_TEXT)))
           {
              CloseClipboard();
              return TRUE;
              break;
           }
           if (!(lpClipData = GlobalLock(hClipData)))
           {
              CloseClipboard();
              return TRUE;
              break;
           }
           j = GlobalSize(hClipData);
           for (i = 0;i < j && i < dato_entra_len && lpClipData[i] && lpClipData[i] != 13;i++)
           {
              dato_entra[i] = lpClipData[i];
           }
           dato_entra[i] = 0;
           GlobalUnlock(hClipData);
           CloseClipboard();
           salida_caracter = PFUNCIONENTRA;			  
           /* ptec(PFUNCIONENTRA); /* manera un poco torpe */
        }
        return TRUE;
        break;
     case SOLICITA_AVANCE:
     case SOLICITA_RETROCESO:
        _M_EXTRA_AVANCE = (int)lParam;
     case SOLICITA_FINAL:
     case SOLICITA_INICIO:
     case SOLICITA_INSERCION:
     case SOLICITA_SUPRESION:
     case SOLICITA_SCROLLUP:
     case SOLICITA_SCROLLDOWN:
     case SOLICITA_VALIDACION:
     case SOLICITA_ANULACION:
           if (HaySeleccion == TRUE && wParam != MID_XCLIPCOPY)
           {
              ClearSelection(principal_w, &Rect, (wParam & MK_SHIFT) ? SL_EXTEND | Shape : Shape);
              SetRectEmpty(&Rect);
              HaySeleccion = FALSE;
           }
        posible_char = 1;
        salida_caracter = wParam;
        return TRUE;
        break;
     case SOLICITA_POSICION:
           if (HaySeleccion == TRUE && wParam != MID_XCLIPCOPY)
           {
              ClearSelection(principal_w, &Rect, (wParam & MK_SHIFT) ? SL_EXTEND | Shape : Shape);
              SetRectEmpty(&Rect);
              HaySeleccion = FALSE;
           }
        x = HIWORD(lParam);
        y = LOWORD(lParam);
        x = (( (x-principal_rect.left) / scaracter.cx) + 1) + (( ((y-principal_rect.top-espacio_arriba) / scaracter.cy)+1)*100);
        if ((_ID_Cursor = esta_en_el_raton(x,NULL)) != -1)
        {
           salida_caracter = SOLICITA_POSICION;
           _M_Cursor = x;			   
           return TRUE;
        }            
        break;
     default:
        if ((__wParam >= DISPARO_CONTROL && __wParam < MAX_DISPARO_CONTROL) || (__wParam >= PREDISPARO_CONTROL && __wParam < PREMAX_DISPARO_CONTROL))
        {
           if (en_lee_teclado && __wParam >= DISPARO_CONTROL && __wParam < MAX_DISPARO_CONTROL)
           {
               if (HaySeleccion == TRUE && wParam != MID_XCLIPCOPY)
               {
                  ClearSelection(principal_w, &Rect, (wParam & MK_SHIFT) ? SL_EXTEND | Shape : Shape);
                  SetRectEmpty(&Rect);
                  HaySeleccion = FALSE;
               }
              posible_char = 1;
              SetFocus (principal_w);				   
              salida_caracter = __wParam;
              return TRUE;
           }
           
           SetFocus (principal_w);				   
           if (HaySeleccion == TRUE && wParam != MID_XCLIPCOPY)
           {
              ClearSelection(principal_w, &Rect, (wParam & MK_SHIFT) ? SL_EXTEND | Shape : Shape);
              SetRectEmpty(&Rect);
              HaySeleccion = FALSE;
           }
#ifdef RWXARXA
           if (en_conexion_terminal)
           {
              extern void dispara_servidor();
              llama_fun_int(dispara_servidor,__wParam);
              return TRUE;
           }
#endif
           if (f_disparo)
           {	  
              llama_fun_int(f_disparo,__wParam);
           }
           return TRUE;
        }
        else
        if(__wParam == PIDO_EMPRESA)
        {            
               if (HaySeleccion == TRUE && wParam != MID_XCLIPCOPY)
               {
                  ClearSelection(principal_w, &Rect, (wParam & MK_SHIFT) ? SL_EXTEND | Shape : Shape);
                  SetRectEmpty(&Rect);
                  HaySeleccion = FALSE;
               }
            posible_char = 1;
            SetFocus (principal_w);				   
            salida_caracter = 831;
            return TRUE;
        }
        else
        if(__wParam == PIDO_SUPERUSUARIO || __wParam == PIDO_VEREURO || __wParam == PIDO_SUPERBLOQUEO || __wParam == PIDO_SUPERDESBLOQUEO ||
		   __wParam == PIDO_SUPERCAMBIO  || __wParam == PIDO_SUPERTODO || __wParam == PIDO_SUPERDESTODO || __wParam == TECLA_COMBO || __wParam == ID_BUSCON || __wParam == ID_ACTIVAR_ERP || __wParam == ID_ACTIVAR_EVT)
        {            
               if (HaySeleccion == TRUE && wParam != MID_XCLIPCOPY)
               {
                  ClearSelection(principal_w, &Rect, (wParam & MK_SHIFT) ? SL_EXTEND | Shape : Shape);
                  SetRectEmpty(&Rect);
                  HaySeleccion = FALSE;
               }
            posible_char = 1;
            SetFocus (principal_w);				   
            salida_caracter = __wParam;
            return TRUE;
        }
        else if (__wParam == BUSCA_PTECS || __wParam == PIDO_TRASMISION || __wParam == PIDO_USUARIOS)
           {
               if (HaySeleccion == TRUE && wParam != MID_XCLIPCOPY)
               {
                  ClearSelection(principal_w, &Rect, (wParam & MK_SHIFT) ? SL_EXTEND | Shape : Shape);
                  SetRectEmpty(&Rect);
                  HaySeleccion = FALSE;
               }
              posible_char = 1;
              /*SetFocus (principal_w);*/
              salida_caracter = __wParam;
              return TRUE;
           }
           else if (wParam <= -MID_XPRIMERA && wParam > -MID_XACERCA)
           {
               if (HaySeleccion == TRUE && wParam != MID_XCLIPCOPY)
               {
                  ClearSelection(principal_w, &Rect, (wParam & MK_SHIFT) ? SL_EXTEND | Shape : Shape);
                  SetRectEmpty(&Rect);
                  HaySeleccion = FALSE;
               }
              posible_char = 1;
              salida_caracter = wParam;
              return TRUE;
           }
           else if ((wParam >= 800 && wParam < IDW_STDDLG) || (wParam >= MID_XPRIMERA && wParam < MID_XOPCION1))
              {
                   if (HaySeleccion == TRUE && wParam != MID_XCLIPCOPY)
                   {
                      ClearSelection(principal_w, &Rect, (wParam & MK_SHIFT) ? SL_EXTEND | Shape : Shape);
                      SetRectEmpty(&Rect);
                      HaySeleccion = FALSE;
                   }
                 if (wParam > 999)
                 {
                    posible_char = 1;
                    SetFocus (principal_w);
                 }
                 if (wParam == 999)
                 {
					if (lParam > 0)
					{
						posible_char = 1;
						salida_caracter = lParam;
					}
                 }
                 else
                    salida_caracter = wParam;
                 return TRUE;
              }
              break;
       }
       return FALSE;
}



long FAR PASCAL AGI_principal_wnd(HWND hWnd,unsigned message,WORD wParam,LONG lParam)
{
   int i,j,x,y,nx,ny,ti,estado,kk;
   PAINTSTRUCT ps; // Paint Struct for BeginPaint call
   HDC hdc;
   RECT rtmp,rtmp2;
   long vuelta = NULL;
   char tmp[100];
   
   
   
   
   if (principal_w != hWnd) 
   {	  
      return(DefWindowProc(hWnd,message,wParam,lParam));
   }
   
   switch (message) /* punto principal de decision */
   {
   case MID_XARXA:
#ifdef RWXARXA			
      /* recibe_mensaje_udp();*/
#endif
      break;
   case WM_PAINT:
      hdc = BeginPaint(principal_w,&ps);
      pinta_zona(hdc,ps.rcPaint,stdpan);
      EndPaint(principal_w,&ps);						 			 		 
      /*
      if (os_subtipo == 1)
      {
      RedrawWindow(hWnd,NULL,NULL,RDW_FRAME);
      vuelta = DefWindowProc(hWnd,message,wParam,lParam);
      }
      */
      break;
   case WM_MOVE:
      RedrawWindow(principal_w,NULL,NULL,RDW_FRAME | RDW_INVALIDATE);
      vuelta = DefWindowProc(hWnd,message,wParam,lParam);
      break;
#ifdef ABSOLETE_KK
   case WM_SIZE: /* ignorar cambios de tamaño */
      break;
      
   case WM_MOVE:
      if (IsIconic(principal_w)) {
         era_icono = 1;
         break;
      } // esto es muy importante
      /* no permnitir mover fuera de pantalla */
      i = 0;
      hdc = GetDC(NULL);
      x = GetDeviceCaps(hdc,HORZRES);
      y = GetDeviceCaps(hdc,VERTRES);
      ReleaseDC(NULL,hdc);
      GetWindowRect(principal_w,&rtmp);
      GetClientRect(principal_w,&rtmp2);
      kk = rtmp.left - rtmp2.left;
      nx = rtmp.right - rtmp.left;
      ny = rtmp.bottom - rtmp.top;
      if (rtmp.bottom > y)
      {
         i = 1;
         rtmp.bottom = y;
         rtmp.top = y - ny;
      }
      if (rtmp.top < kk)
      {
         i = 1;
         rtmp.top = kk;
         rtmp.bottom = ny+kk;
      }
      if (rtmp.left < kk)
      {
         i = 1;
         rtmp.left = kk;
         rtmp.right = nx+kk;
      }
      if (rtmp.right > x)
      {
         i = 1;
         rtmp.left = x - nx + kk;
         rtmp.right = x + kk;
      }
      if (i)
      {
         MoveWindow(principal_w,rtmp.left ,rtmp.top,
            nx,ny,TRUE);		
      }
      break;
#endif
   case WM_CHAR:	 	 
      mensaje_char((UINT)wParam,(UINT)(lParam & 0xffff), (UINT)lParam);
      break;
   case WM_KEYDOWN:
      mensaje_keydown((UINT)wParam,(UINT)(lParam & 0xffff), (UINT)lParam);
      break;
   case WM_KEYUP:	  
      mensaje_keyup((UINT)wParam,(UINT)(lParam & 0xffff), (UINT)lParam);
      break;
   case WM_LBUTTONDBLCLK:
      mensaje_ldblclk((UINT)LOWORD(lParam),(UINT)HIWORD(lParam),(UINT)wParam);
      break;
   case WM_LBUTTONDOWN:
      mensaje_lbuttondown((UINT)LOWORD(lParam),(UINT)HIWORD(lParam),(UINT)wParam);
      break;
   case WM_MOUSEMOVE:
      mensaje_mousemove((UINT)LOWORD(lParam),(UINT)HIWORD(lParam),(UINT)wParam);
      break;
   case WM_LBUTTONUP:
      mensaje_lbuttonup((UINT)LOWORD(lParam),(UINT)HIWORD(lParam),(UINT)wParam);
      break;
   case WM_RBUTTONDOWN:
      break;
   case WM_RBUTTONDBLCLK:
   case WM_MBUTTONDBLCLK:
      break;
   case WM_ACTIVATE:
      switch(wParam) {
      case WA_INACTIVE:
         if (hpalCurrent)
         {
            if (OldPalette)
            {
               hdc = GetDC(principal_w);
               SelectPalette(hdc, OldPalette, FALSE);
               RealizePalette(hdc);
               ReleaseDC(principal_w,hdc);
            }
         }
         if (tipo_cursor && cur_ini)
         {
            DsSetCursor(cur_ini);
         }
         cur_ini = NULL;
         tipo_cursor = 0;
         
         desactivada = 1;
         if (curon(-1))
            DestroyCaret();
         break;
      case WA_ACTIVE:
      case WA_CLICKACTIVE:
         if (hpalCurrent)
         {
            hdc = GetDC(principal_w);
            OldPalette = SelectPalette(hdc, hpalCurrent, FALSE);
            RealizePalette(hdc);
            ReleaseDC(principal_w,hdc);
         }
         
         if (!cur_ini)
         {
            cur_ini = DsSetCursor(cur_flecha);
            tipo_cursor = 1;
         }
         if (desactivada)
         {
            desactivada = 0;
            if (curon(-1))
            {
               CreateCaret(principal_w,(HBITMAP)(curon(-1)-1),scaracter.cx,scaracter.cy);
               SetCaretPos(cursorx,cursory);
            }
            SetFocus (principal_w);
         }
         break;
      default:
         break;
      }		
      break;
      case WM_SETFOCUS:
         if (curon(-1))
            ShowCaret(hWnd);
         break;
      case WM_KILLFOCUS:
         if (curon(-1))
            HideCaret(hWnd);
         break;
      case WM_COMMAND:	  
         vuelta = mensaje_command(wParam,lParam);
         break;
      case WM_QUIT:
      case WM_DESTROY:      // message: window being destroyed
         if (hData)
         {
            if (OpenClipboard(principal_w))
            {
               EmptyClipboard();
               CloseClipboard();
            }
            GlobalFree(hData);
         }
         hData = NULL;
         
         DestroyCaret();
         break;
      case WM_TIMER:
         if (wParam == 12121)
         {
            salida_caracter = -2;
         }
         break;
      default:
         return(DefWindowProc(hWnd,message,wParam,lParam));
  } 
  return(vuelta);
}



/***** FUNCIONES STANDAR REEMPLAZADAS *****/
int c_rw_error = 0;
int no_rw_error = 0;

int rw_error(n)
int n;
{
   char tmp[81];

   c_rw_error++;
   if (no_rw_error)
	   return 0;

   sprintf(tmp,__Texto[150],n);
   MessageBox(principal_w /*GetParent(principal_w)*/, tmp,NULL, MB_OK | MB_ICONSTOP);
   tracer_int = 1;
}


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

extern int (*agi_especial_system_antes)(void);
extern int (*agi_especial_system_despues)(int);
extern FILE *_imp;

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
   if (getenv("DSWINEX"))
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
	  /*
	  if (punto > 8 && (!memcmp((char *)mandato+punto-9,"agilis",6) || !memcmp((char *)mandato+punto-9,"agisys",6) || !memcmp((char *)mandato+punto-9,"agigen",6)) )
		  oculto = 0;
	  */
/*#ifdef RWSERVER*/
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


#ifdef KAKA
int agi_system(s)
char *s;
{
   HINSTANCE insta = 0;
   int i,j;
   char tmp[128];
   int punto = 0;

   if (getenv("DSWINEX"))
   {
	   return agi_system_wine(s);
   }
   
   for (i = 0;s[i] && s[i] != ' ';i++)
      if (s[i] == '.') punto = 1;
      
      if (!punto)
      {
         if (agi_bats_procesor(s))
         {
#ifdef FGV32
			/*
			MessageBox(NULL,"anTEs1","ojo",MB_OK);
			SendMessage(principal_w,WM_COMMAND,44422,(LPARAM)s);
			MessageBox(NULL,"Despues1","ojo",MB_OK);
			*/
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
      
	  /*
			MessageBox(NULL,"anTEs2","ojo",MB_OK);
			SendMessage(principal_w,WM_COMMAND,44422,(LPARAM)s);
			MessageBox(NULL,"Despues2","ojo",MB_OK);
	   */
      
      system(s);
      /*
      memcpy(tmp,s,i);
      tmp[i] = 0;
      while(s[i] == ' ') i++;
      insta = ShellExecute(principal_w,NULL,tmp,s+i,"",SW_SHOW);
      */
      return(0);
}

#endif

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

#ifndef FGV32
void sleep(n)
unsigned int n;
{
   long t = time(NULL) + n;
   int oe = en_menu;
   en_menu = 1;
   while(t > time(NULL))
      agi_cola_win();
   en_menu = oe;
   return(0);
}
#endif

extern char *agi_aplicacion;
extern char *agi_empresa;
extern char *agi_codi_empre;

static int donde_pulsado = 0;
static int cola_caracteres[110];

static int _pulsatecla(m) /* reemplaza a la getchar() */
int m;
{
   int i,r;
   _agi_cola_win(m);
   
   if (modo_desatendido && !m)
   {
						return(-1);
   }
   
   if (donde_pulsado)
   {
						r = cola_caracteres[0];
                  for (i = 0;i < (donde_pulsado-1);i++)
                  {
                     cola_caracteres[i] = cola_caracteres[i+1];
                  }
                  donde_pulsado--;
                  return(r);
   }
   return(-1);
}

int pon_pulsado(pulsacion)
int pulsacion;
{
   int i;
   if (pulsacion != -1)
   {
      if (donde_pulsado == 100)
      {
         for (i = 0;i< 99;i++)
            cola_caracteres[i] = cola_caracteres[i+1];
         donde_pulsado--;
      }
      cola_caracteres[donde_pulsado] = pulsacion;
      donde_pulsado++;
   }
   return(pulsacion);
}

int pulsatecla()
{
   return(_pulsatecla(0));
}

extern int que_menu;
#ifdef RWXARXA
extern int envia_subejecuta_a_server();
#endif



static int _agi_cola_win(m)
int m;
{
   int i;   
   int j,k,qj;
   MSG msg;
   char tmp[128];
   char tmp2[81];
   char mi_para[128],*pp;
   int en_programa = 0;
   int que_programa = 0;
   VENT *w = NULL;
   static int unavez = 0;     


   if (No_Procesar_Mensajes)
   {
	   if (donde_pulsado)
	   {
		  en_espera_de_pinta = 0;
		  return(-1);
	   }
	   else
	   {
		  salida_caracter = 807;
		  en_espera_de_pinta = 0;
		  pon_pulsado(salida_caracter);
	   }
   }
   
   salida_caracter = -1;
   
   if (era_icono && !IsIconic(principal_w))
   {
      if (en_menu)
      {
									salida_caracter = MID_ACTUALIZAMENU;
                           era_icono = 0;
                           return(pon_pulsado(salida_caracter));
      }
   }
   
   if (!m || m == -5)
   {
      if (!SinColaMensajes)
      {
						if (!PeekMessage(&msg,NULL,0, 0, PM_NOREMOVE))
                           {
                              en_espera_de_pinta = 0;
                              return(-1);
                           }
      }
   }
   
   
   posible_char = 0;   
   while (SinColaMensajes || !abandonarelbarco)
   {
	/*
	  if (unavez == 0)
	  {
		 unavez = 1;
	 	 return(pon_pulsado(BUSCA_PTECS));
	  }
	  unavez = 0;
	*/
	  if (abandonarelbarco || !GetMessage((LPMSG)&msg,NULL,0,0))
	  {
		  abandonarelbarco = 1;		
		  salida_caracter = 807;
		  for (i = 0;i < 20;i++)
			 ptec(807);		  
	  }
	  
      
      if (SinColaMensajes)
		 Sleep(100);
      
      //if (msg.message == WM_PAINT) en_espera_de_pinta = 0;
      /* pon aqui el Accelerator */
      if (!SinColaMensajes)
      {          
         if (DespachadorExterno)
         {
                (*DespachadorExterno)((int)DESPACHO_PRETRANSLATE,0,&msg);
         }
	     if (msg.hwnd)
         {
            if ((msg.hwnd == principal_w || !IsDialogMessage(principal_w,&msg)) && (msg.hwnd != principal_w || !agi_acelerador || !TranslateAccelerator(msg.hwnd, agi_acelerador, (LPMSG)&msg)))
            {

                TranslateMessage((LPMSG)&msg);		 
                DispatchMessage((LPMSG)&msg);
			}
         }         
                           
      }
      
      if (salida_caracter == -2)
      {
				m = 0;
                salida_caracter = -1;
      }	
      
      if (!recoge_todo_caracter && m == -5 && !posible_char) salida_caracter = -1;
      
      if (en_programa && salida_caracter == -1) continue;
      
      if (salida_caracter == MID_DISENO)
      {
									en_programa = 1;
                           w = w_ini(401,480);
                           w_salva(w);
                           mensaje(0,__Texto[219]);
                           MessageBeep(0);		  
                           salida_caracter = -1;
                           continue;
      }
      if (en_programa == 1 && salida_caracter > 822 && salida_caracter < 833)
      {
									aviso(0,__Texto[151]);
      }
      if (en_programa == 1 && salida_caracter > 832 && salida_caracter < MID_MENUSG)
      {
									en_programa = 2;
                           mensaje(0,__Texto[220]);
                           que_programa = salida_caracter - 823;
                           MessageBeep(0);
                           salida_caracter = -1;
                           continue;
      }
      if (en_menu && en_programa != 2)
      {
									if (w)
                           {
                              aviso(0,__Texto[152]);
                              w_pinta(w);
                              w_fin(w);
                              w = NULL;
                           }
                           en_programa = 0;
                           if (m && m != -5 && salida_caracter == -1) continue;
                           return( pon_pulsado(salida_caracter) );
      }
      if (en_menu && salida_caracter >= 'A' && salida_caracter < 999)
      {
									// desactivado en 9
									//salida_caracter = MID_XPRIMERA + que_menu * 100 + (salida_caracter - 'A');
      }
      if (salida_caracter >= MID_XPRIMERA && salida_caracter < MID_XACERCA)
      {
									switch(salida_caracter)
                           {
                           default:
                              if (salida_caracter >= MID_XPRIMERA && salida_caracter < MID_XOPCION1)
                              {
                                 k = salida_caracter - MID_XPRIMERA;
                                 j = k / 100;
                                 k %= 100;
                                 tmp[0] = '|';
                                 tmp[1] = 0;
                                 if (en_programa == 2)
                                 {
                                    strcpy(tmp2,wwmenus[j][k]);
                                    pp = tmp2;
                                    mi_para[0] = 0;
                                    while (*pp && *pp != ';') pp++;
                                    if (*pp == ';') {*pp = 0;pp++;mi_para[0] = 127;strcpy(mi_para+1,pp);}
                                    
                                    if (!strlen(tmp2))
                                    {
                                       aviso(0,__Texto[153]);
                                       break;
                                    }
									quitab(tmp2);
                                    quitabp(tmp2);
                                    if ((tmp2[0] < '0' || tmp2[0] > '9' || tmp2[1] != ' ') && tmp2[0] != ':' && tmp2[0] != '|')
                                    {
                                       if (agi_aplicacion)
                                       {
                                          strcat(tmp,":");
                                          strcat(tmp,agi_aplicacion);
                                          strcat(tmp,"/");
                                       }
                                       strcat(tmp,tmp2);
                                       if (agi_empresa)
                                       {
                                          strcat(tmp," ");
                                          strcat(tmp,agi_empresa);
                                          if (agi_codi_empre)
                                          {
                                             strcat(tmp," ");
                                             strcat(tmp,agi_codi_empre);
                                          }
                                       }
                                    }
                                    else
									{
										if (tmp2[0] == '|')
										{
										  char *ipp;
										  ipp = tmp2+strlen(tmp2);
										  while(ipp > tmp2)
										  {
											  ipp--;
											  if (*ipp == ' ')
												  break;
										  }
										  if (*ipp == ' ' && agi_codi_empre)
										  {
											 strcat(tmp2," ");
											 strcat(tmp2,agi_codi_empre);
										  }
										}

                                       strcat(tmp,tmp2);
									}
                                    strcat(tmp,mi_para);
                                    
                                    sprintf(tmp2,__Texto[413],que_programa+1);
                                    qj = DialogBoxParam(hInst,"WAGINPUT", principal_w /*GetParent(principal_w)*/, agilpDlginput,(LPARAM)tmp2);
                                    if (qj != IDOK)
                                    {														
                                       if (w)
                                       {
                                          w_pinta(w);
                                          w_fin(w);
                                          w = NULL;
                                       }
                                       salida_caracter = -1;
                                       en_programa = 0;
                                       break;
                                    }
                                    
                                    tecla_funcion(que_programa,tmp,0);
                                    
                                    if (desc_funs[que_programa])
                                    {
                                       free(desc_funs[que_programa]);
                                    }
                                    desc_funs[que_programa] = malloc(strlen(tmp2)+1);
                                    strcpy(desc_funs[que_programa],tmp2);
                                    MessageBeep(0);
                                    MessageBeep(0);
                                    MessageBeep(0);
                                    MessageBeep(0);
                                    sprintf(tmp2,__Texto[154],que_programa+1,desc_funs[que_programa]);
                                    aviso(0,tmp2);
                                    en_programa = 0;

									{
										int xi;
										extern char *_xtitu_tecla[20];
									    for (xi = 0;xi < 20;xi++)
										{
											_xtitu_tecla[xi] = tecla_funcion(xi+_FUNCIONES,NULL,1);
										}
									}
                                    
                                    if (graba_fgv(1))
                                    {
                                       aviso(0,__Texto[155]);
                                    }
									{
										int xi;
										extern char *_xtitu_tecla[20];
									    for (xi = 0;xi < 20;xi++)
										{
											_xtitu_tecla[xi] = NULL;
										}
									}
                                    if (DespachadorExterno)
                                    {
                                       (*DespachadorExterno)((int)NUEVA_TECLA_PROGRAMADA,0,NULL);
                                    }
                                    
                                    if (w)
                                    {
                                       w_pinta(w);
                                       w_fin(w);
                                       w = NULL;
                                    }
                                 }
                                 else
                                 {
                                    if (wwmenus)
                                    {
                                       if (!en_lee_teclado)
                                       {
                                          aviso(0,__Texto[156]);
                                       }
                                       else
                                       {
                                          strcpy(tmp+1,wwmenus[j][k]);
                                          i = en_lee_teclado;
#ifdef RWXARXA
                                          
                                          en_lee_teclado = 0;						   
                                          if (en_conexion_terminal)
                                          {
                                             llama_fun_int(envia_subejecuta_a_server,tmp);
                                             /* envia_subejecuta_a_server(tmp); */
                                          }
                                          else
#endif
                                             if (f_util)
                                                llama_fun_int(f_util,tmp);
                                             en_lee_teclado = i;
                                       }
                                    }
                                 }
                }
                salida_caracter = -1;
                en_programa = 0;
                break;
          }
        }
        if (w)
        {
           aviso(0,__Texto[157]);
           w_pinta(w);
           w_fin(w);
           w = NULL;
        }
        if (m == -5)
        {
           if (!SinColaMensajes)
           {
              if (PeekMessage(&msg, NULL,0, 0, PM_NOREMOVE))
              {
                 if (salida_caracter != -1)
                    pon_pulsado(salida_caracter);
                 salida_caracter = -1;
                 continue;
              }
           }
           m = 0;
        }
        if (m && salida_caracter == -1) 
        {	  
           continue;
        }
        
        return(pon_pulsado(salida_caracter));
  }
  
  /* anormal !!! */
  
  /*aviso(0,__Texto[158]);*/

  en_espera_de_pinta = 0;
  if (w)
  {
     w_pinta(w);
     w_fin(w);
     w = NULL;
  }
  /* poner ptec de abandonar aplicacion */
  for (i = 0;i < 50;i++)
     ptec(807);
  return(807);
}

int agi_cola_win()
{
   return(_agi_cola_win(0));
}

static int timer_en_windows = 0;
int agi_cola_timer()
{
   //int tiempo = clock() / CLOCKS_PER_SEC;
   //if (tiempo > timer_en_windows)
   //   {
   //   timer_en_windows = tiempo + 2; /* la actualizacion cada 2 segundos!! */
   return(_agi_cola_win(-5));
   //   }
   //return(0);
}


int win_leech(timeout)
int timeout;
{
   int r;
   int i;
   
   
   if (donde_pulsado)
   {
      r = cola_caracteres[0];
      for (i = 0;i < (donde_pulsado-1);i++)
      {
         cola_caracteres[i] = cola_caracteres[i+1];
      }
      donde_pulsado--;  
      return(r);
   }
   
   if (timeout)
   {
      /* crear timer */
      SetTimer(principal_w,12121,timeout * 1000,NULL);
   }
   r = _pulsatecla(1);
   if (timeout)
   {
      KillTimer(principal_w,12121);
   } 
   _agi_cola_win(-5); /* limpia todos los mensajes ! */
   return(r);
}


void Coge_StdPan(void *puntero)
{
   char *p = (char *)puntero;
   memcpy(p,&principal_w,sizeof(HWND));
   p += sizeof(HWND);
   memcpy(p,&stdpan,sizeof(void *));
   p += sizeof(void *);
   memcpy(&principal_w,p,sizeof(HWND));
   p += sizeof(HWND);
   memcpy(&stdpan,p,sizeof(void *));
   if (!stdpan)
			{
      stdpan = w_ini(-1,-1);
      w_cls(stdpan);
			}
}

void RePon_StdPan(void *puntero)
{
   char *p = (char *)puntero;
   VENT *ant_stdpan = stdpan;
   
   memcpy(&principal_w,p,sizeof(HWND));
   p += sizeof(HWND);
   memcpy(&stdpan,p,sizeof(void *));
   w_fin(ant_stdpan);
}



INT StartSelection(
                   HWND hWnd,
                   MPOINT ptCurrent,
                   LPRECT lpSelectRect,
                   INT fFlags)
{
   if (lpSelectRect->left != lpSelectRect->right ||
      lpSelectRect->top != lpSelectRect->bottom)
      ClearSelection(hWnd, lpSelectRect, fFlags);
   
   lpSelectRect->right = ptCurrent.x;
   lpSelectRect->bottom = ptCurrent.y;
   
   /* If you are extending the box, then invert the current rectangle */
   
   if ((fFlags & SL_SPECIAL) == SL_EXTEND)
      ClearSelection(hWnd, lpSelectRect, fFlags);
   
   /* Otherwise, set origin to current location */
   
   else {
      lpSelectRect->left = ptCurrent.x;
      lpSelectRect->top = ptCurrent.y;
			}
   SetCapture(hWnd);
   return 1;
}

/****************************************************************************

  FUNCTION: UpdateSelection(HWND, POINT, LPRECT, int) - update selection area
  
			 PURPOSE: Update selection
          
****************************************************************************/

INT UpdateSelection(
                    HWND hWnd,
                    MPOINT ptCurrent,
                    LPRECT lpSelectRect,
                    INT fFlags)
{
   HDC hDC;
   SHORT OldROP;
   
   hDC = GetDC(hWnd);
   
   switch (fFlags & SL_TYPE) {
      
			case SL_BOX:
            OldROP = (SHORT)SetROP2(hDC, R2_NOTXORPEN);
            MoveToEx(hDC, lpSelectRect->left, lpSelectRect->top, NULL);
            LineTo(hDC, lpSelectRect->right, lpSelectRect->top);
            LineTo(hDC, lpSelectRect->right, lpSelectRect->bottom);
            LineTo(hDC, lpSelectRect->left, lpSelectRect->bottom);
            LineTo(hDC, lpSelectRect->left, lpSelectRect->top);
            LineTo(hDC, ptCurrent.x, lpSelectRect->top);
            LineTo(hDC, ptCurrent.x, ptCurrent.y);
            LineTo(hDC, lpSelectRect->left, ptCurrent.y);
            LineTo(hDC, lpSelectRect->left, lpSelectRect->top);
            SetROP2(hDC, OldROP);
            break;
            
         case SL_BLOCK:
            PatBlt(hDC,
               lpSelectRect->left,
               lpSelectRect->bottom,
               lpSelectRect->right - lpSelectRect->left,
               ptCurrent.y - lpSelectRect->bottom,
               DSTINVERT);
            PatBlt(hDC,
               lpSelectRect->right,
               lpSelectRect->top,
               ptCurrent.x - lpSelectRect->right,
               ptCurrent.y - lpSelectRect->top,
               DSTINVERT);
            break;
			}
   lpSelectRect->right = ptCurrent.x;
   lpSelectRect->bottom = ptCurrent.y;
   ReleaseDC(hWnd, hDC);
   return 1;
}

/****************************************************************************

  FUNCTION: EndSelection(POINT, LPRECT)
  
			 PURPOSE: End selection of region, release capture of mouse movement
          
****************************************************************************/

INT EndSelection(
                 MPOINT ptCurrent,
                 LPRECT lpSelectRect)
{
   lpSelectRect->right = ptCurrent.x;
   lpSelectRect->bottom = ptCurrent.y;
   ReleaseCapture();
   return 1;
}

/****************************************************************************

  FUNCTION: ClearSelection(HWND, LPRECT, int) - clear selection area
  
			 PURPOSE: Clear the current selection
          
****************************************************************************/

INT ClearSelection(
                   HWND hWnd,
                   LPRECT lpSelectRect,
                   INT fFlags)
{
   HDC hDC;
   INT2DWORD OldROP;
   
   hDC = GetDC(hWnd);
   switch (fFlags & SL_TYPE) {
      
			case SL_BOX:
            OldROP = SetROP2(hDC, R2_NOTXORPEN);
            MoveToEx(hDC, lpSelectRect->left, lpSelectRect->top, NULL);
            LineTo(hDC, lpSelectRect->right, lpSelectRect->top);
            LineTo(hDC, lpSelectRect->right, lpSelectRect->bottom);
            LineTo(hDC, lpSelectRect->left, lpSelectRect->bottom);
            LineTo(hDC, lpSelectRect->left, lpSelectRect->top);
            SetROP2(hDC, OldROP);
            break;
            
         case SL_BLOCK:
            PatBlt(hDC,
               lpSelectRect->left,
               lpSelectRect->top,
               lpSelectRect->right - lpSelectRect->left,
               lpSelectRect->bottom - lpSelectRect->top,
               DSTINVERT);
            break;
			}
   ReleaseDC(hWnd, hDC);
   return 1;
}

#endif