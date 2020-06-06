/*---------------------------------------------------------------------------
* Programa ..: rlclinea.c
* Fecha .....:
* Version ...:
*              char *lee_linea(),
*---------------------------------------------------------------------------
*/

#include <fgvsys.h>
#ifdef ESDOS
#ifndef FGVWIN
#ifndef FGV32
#include <bios.h>
#endif
#endif
#endif
#include <stdio.h>
#include <rtermin.h>

char no_imprime[3] = {0,0,0};

int (*externo_dll)() = NULL;
char *el_dll = NULL;


#ifdef FGV32
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
#endif
#endif


/* ----------------------------------------------------------------------------
*  lee_linea
*     lee una linea de un fichero secuencial abierto en 'file',
*     lee hasta 'num' caracteres o bien hasta final de linea,
*     'texto' contiene el string leido sin el '\n' del final.
*     Devuelve un puntero a 'texto'.
*     En caso de error devuelve NULL que es un (char *)0.
* ----------------------------------------------------------------------------
*/
char *lee_linea(texto,num,file)
char *texto;
int num;
FILE *file;
{
   
   if (fgets(texto,num,file) == NULL)
      return(NULL);
   quitan(texto);    /* quita los '\n' de detras */
   quitab(texto);   /* quita blancos de detras  */
   if (texto[0] == 4 || texto[0] == 26) /* tanto el Ctrl-D como el Ctrl-Z acaban */ 
      return(NULL);
   return(texto);
}

/*  --------------------------------------------------------------
*  graba_linea
*  graba una linea en el fichero secuencial abierto en 'file',
*  la linea grabada es el contenido del string 'texto',
*  mas el '\n' de final de linea.
*  Devuelve un integer 0 si ha ido bien.
*  En caso de error devuelve EOF.
*  --------------------------------------------------------------
*/

int graban_linea(text,file)
unsigned char *text;
FILE *file;
{
   return(_graba_linea(text,file,0));
}

int graba_linea(text,file)
unsigned char *text;
FILE *file;
{
			return(_graba_linea(text,file,1));
}


/*#ifdef FGVWIN
//int printa_w_linea(char *,unsigned short);
//#endif*/

int _graba_linea(text,file,x)
unsigned char *text;
FILE *file;
int x;
{
   VENT *w;
   unsigned char texto[2000];
   short i,p,ec,rr;
   unsigned short len;
   unsigned char o,e;
   unsigned long ff = (unsigned long)file;
   
   if (x > 1)
      len = x;
   else
      len = strlen(text) + 1;
   memcpy(texto,text,len);
   if (x == 1)
   {
      quitab(texto);
      quitan(texto);
      quitab(texto);
#ifdef ESDOS
      strcat(texto,"\n");
#else
      strcat(texto,"\r\n");
#endif
   }
   if (x > 1)
   {
      len = (unsigned short)texto[x-1];
   }
   else
   {
      len = strlen(texto);
   }

#ifdef RWXARXA
#ifdef RWSERVER
   if (en_conexion_externa && ff == 3L)
   {
      return(printa_en_terminal(texto,x));
   }
#endif
#endif

   if (!file) 
   {
        if (DespachadorExterno)
        {	 
           texto[x] = 0;
           (*DespachadorExterno)((int)DESPACHO_LISTADO,0,texto);
        }
       return(0);
   }      

#ifdef FGVWIN
   if (ff < 0x00000004L)
   {
      printa_w_linea(texto,len);
      return(0);
   }
#endif
   
#ifdef ESDOS
#ifndef FGVWIN
#ifndef FGV32
   if (ff < 0x00000004L)
   {
      i = 0;
      p = (short)(ff & 0x0000FFFFL);
      p--;
      if (no_imprime[p]) return(EOF);
      rr = 0;
      while(len)
						{
         len--;
         if (rr)
         {
            i--;
            o = '\n';
            rr = 0;
         }
         else
         {
            o = texto[i];
            if (o == '\n')
            {
               o = '\r';
               rr = 1;
               len++;
            }
         }
         e = _bios_printer(_PRINTER_STATUS ,p,0);
#ifdef COMENTARIO
         asm {
            push ax
               push dx
               mov ah,02h;
            mov dx,p;
            int 17h
               mov e,ah
               pop dx
               pop ax
         }
#endif
         if ((e & 0x20) || (e & 0x8) || !(e & 0x10))
            ec = 1;
         else
            ec = 0;
         
         for (;ec < 2;ec++)
         {
            if (ec >= 1) {
               w=w_ini(2401,2480);
               w_salva(w);
               Bl(24);
               pito(147);
               if (confirmar(2400,__Texto[437],__Texto[448])
               {
                  no_imprime[p] = 1;
               }
               w_pinta(w);
               w_fin(w);
               if (no_imprime[p])
                  return(EOF);
               ec = 0;
               e = _bios_printer(_PRINTER_STATUS ,p,0);
#ifdef COMENTARIO
               asm {
                  push ax
                     push dx
                     mov ah,02h;
                  mov dx,p;
                  int 17h
                     mov e,ah
                     pop dx
                     pop ax
               }
#endif
               if ((e & 0x20) || (e & 0x8) || !(e & 0x10))
               {
                  ec = 0;
                  continue; /* ya incermenta */
               }
            }
            e = _bios_printer(_PRINTER_WRITE,p,o);
#ifdef COMENTARIO
            asm {
               push ax;
               push dx;
               xor ah,ah;
               mov al,o;
               mov dx,p;
               int 17h;
               mov e,ah;
               pop dx;
               pop ax;
            }
#endif
            if (!(e & 0x8))
               break;
         }
         i++;
						}
      return(0);
   }
#endif
#endif
#endif
   
   if ((unsigned long)file == (unsigned long)0x00000004L)
   {
		if (externo_dll)
           (*externo_dll)(el_dll,"imprime",texto);
   }
   else
   {
						
						if (x > 1)
                  {
                     i = 0;
                     while(len)
                     {
                        len--;
                        if (fputc(texto[i],file) == EOF) return(EOF);
                        i++;
                     }
                     return(0);
                  }
                  else
                     if (fputs(texto,file) == EOF)
                        return(EOF);
                     
   }
   
   return(0);
}

