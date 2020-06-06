
/*** Windows ***/

/* para crear fgvwin.h */

#define MAXBOTON 500

#define AGIDSI    101
#define AGIDNO    102
#define AGIDTEXTO 201
#define AGIDLISTA 202
#define AGIDINPUT 203

#define IDC_NUMERO                      1002
#define IDC_ACTIVACION                  1003
#define IDC_IDENTIFICA                  1004
#define IDC_EDIT1                       1005
#define IDC_EDIT2                       1006
#define IDC_TELEFONOS                   1007
#define IDC_PROGRESO                    1008
#define IDC_VER2                        1009
#define IDC_VER1                        1010
#define IDC_ANIMADO                     1011

#define IDC_STATIC                      -1


#define MID_MENUSG 1000
#define MID_DISENO 1100
#define MID_XPRIMERA  8000
#define MID_XOPCION1 30001
#define MID_ACTUALIZAMENU 30996
#define MID_XCLIPCOPY   30997
#define MID_XCLIPPASTE  30998
#define MID_XACERCA     30999


// #define AGIXTECLADO  "_AGIXTECLADO"
#define WIN_TECLA801    801
#define WIN_TECLA802    802
#define WIN_TECLA803    803
#define WIN_TECLA804    804
#define WIN_TECLA805    805
#define WIN_TECLA806    806
#define WIN_TECLA807    807
#define WIN_TECLA808    808
#define WIN_TECLA809    809
#define WIN_TECLA810    810
#define WIN_TECLA811    811
#define WIN_TECLA812    812
#define WIN_TECLA813    813
#define WIN_TECLA814    814
#define WIN_TECLA815    815
#define WIN_TECLA816    816
#define WIN_TECLA817    817
#define WIN_TECLA818    818
#define WIN_TECLA819    819
#define WIN_TECLA820    820
#define WIN_TECLA821    821
#define WIN_TECLA822    822
#define WIN_TECLA823    823
#define WIN_TECLA824    824
#define WIN_TECLA825    825
#define WIN_TECLA826    826
#define WIN_TECLA827    827
#define WIN_TECLA828    828
#define WIN_TECLA829    829
#define WIN_TECLA830    830
#define WIN_TECLA831    831
#define WIN_TECLA832    832
#define WIN_TECLA833    833
#define WIN_TECLA834    834
#define WIN_TECLA835    835
#define WIN_TECLA836    836
#define WIN_TECLA837    837
#define WIN_TECLA838    838
#define WIN_TECLA839    839
#define WIN_TECLA840    840
#define WIN_TECLA841    841
#define WIN_TECLA842    842
#define ESTILO (/*WS_OVERLAPPED |*/ WS_DLGFRAME   | WS_CHILD /*| WS_CLIPSIBLINGS   /*  WS_SYSMENU*/ /*| WS_CAPTION /*| WS_MINIMIZEBOX*/)


#ifndef WMAIN
extern HWND principal_w; /* ventana principal */
extern HINSTANCE hInst;  /* instance del runtime */
extern HMENU principal_menu;
extern char ***wwmenus; /* opciones */
extern int *nwwmenus;  /* numero de opciones para liberacion */
extern int en_menu;
extern int n_wmenus;
extern RECT principal_rect;
extern SIZE scaracter;
extern HBRUSH agi_brush;
extern HBRUSH agi_brush_gris;
extern HDC hMemoryDC;
extern HBITMAP hOldBitmap;

extern HBITMAP agitapiz_n0;
extern HBITMAP agitapiz_n1;
extern HBITMAP agitapiz_n2;
extern HBITMAP agitapiz_n3; /* fondo hasta 3 niveles encima del standar  */

extern HBITMAP bmp_fondo_1; /* fondos segun colores ... para ajustar segun atributo normalmente */
extern HBITMAP bmp_fondo_2;
extern HBITMAP bmp_fondo_3;
extern HBITMAP bmp_fondo_4;
extern HBITMAP bmp_fondo_5;
extern HBITMAP bmp_fondo_6;
extern HBITMAP bmp_fondo_7;
extern HBITMAP bmp_fondo_campo; /* fondo standard de campo editable ... puede que solo este activo cuando no se edita */

extern HBITMAP ds_bmp_fondo; /* referencia al fondo con el color actual para escribir */

extern HBITMAP agitapiz;  /* referencia al fondo con color standard, usado en cls ... etc*/


extern HFONT agi_Font;   // fuente fixed pitch
extern HFONT agi_fmensa; // fuente para mensajes
extern HFONT agi_finten; // fuente para intenso
extern HFONT agi_frever; // fuente para reverse
extern HFONT agi_fcampo; // fuente para campos
extern HFONT agi_fboton; // fuente para botones
extern int principal_nx;
extern int principal_ny;
extern int espacio_arriba;
extern DLGPROC agilpDlgProcx;
extern DLGPROC agilpDlgImpre;
extern DLGPROC agilpDlginput;
#else
#ifdef WMAIN_CPP
extern "C" 
{
#endif
   
   
   DLGPROC agilpDlginput = NULL;
   DLGPROC agilpDlgProcx = NULL;
   DLGPROC agilpDlgImpre = NULL;
   int espacio_arriba = 0;
   int principal_nx = 0;
   int principal_ny = 0;
   HFONT agi_fmensa = NULL; // fuente para mensajes
   HFONT agi_finten = NULL; // fuente para intenso
   HFONT agi_frever = NULL; // fuente para reverse
   HFONT agi_fcampo = NULL; // fuente para campos
   HFONT agi_fboton = NULL; // fuente para botones



	HBITMAP agitapiz_n0 = NULL;
	HBITMAP agitapiz_n1 = NULL;
	HBITMAP agitapiz_n2 = NULL;
	HBITMAP agitapiz_n3 = NULL; /* fondo hasta 3 niveles encima del standar  */

	HBITMAP bmp_fondo_1 = NULL; /* fondos segun colores ... para ajustar segun atributo normalmente */
	HBITMAP bmp_fondo_2 = NULL;
	HBITMAP bmp_fondo_3 = NULL;
	HBITMAP bmp_fondo_4 = NULL;
	HBITMAP bmp_fondo_5 = NULL;
	HBITMAP bmp_fondo_6 = NULL;
	HBITMAP bmp_fondo_7 = NULL;
	HBITMAP bmp_fondo_campo = NULL; /* fondo standard de campo editable ... puede que solo este activo cuando no se edita */

	HBITMAP ds_bmp_fondo = NULL; /* referencia al fondo con el color actual para escribir */

   HBITMAP agitapiz = NULL;

   HDC hMemoryDC = NULL;
   HBITMAP hOldBitmap = NULL;
   HFONT agi_Font = NULL;
   HBRUSH agi_brush = NULL;
   HBRUSH agi_brush_gris = NULL;
   SIZE scaracter;
   RECT principal_rect;
   HINSTANCE hInst = NULL;
   HWND principal_w = NULL;
   char ***wwmenus = NULL; /* opciones */
   int *nwwmenus = NULL;  /* numero de opciones para liberacion */
   HMENU principal_menu = NULL;
   extern int en_menu = 0;
   extern int n_wmenus = 0;
#ifdef WMAIN_CPP
}
#endif
#endif
/***************/
#ifdef FGV32
#ifdef WMAIN_CPP
extern "C" 
{
#endif
   BOOL CALLBACK AGIDIALOGO(HWND,UINT,WPARAM,LPARAM);
   BOOL CALLBACK  AGIMPRESORA(HWND,UINT,WPARAM,LPARAM);
   BOOL CALLBACK AGINPUT_(HWND,UINT,WPARAM,LPARAM);
#define agilpDlgProcx AGIDIALOGO
#define agilpDlgImpre AGIMPRESORA
#define agilpDlginput AGINPUT_
#ifdef WMAIN_CPP
}
#endif
#endif

