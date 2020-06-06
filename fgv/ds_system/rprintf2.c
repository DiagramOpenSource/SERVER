
/* Autor: Francisco Gonzalez Valiente */
#include <fgvsys.h>

#include <stdio.h>

#include <rtermin.h>
#define _NUEVODEF
#include <rdef.h>      /* ya incluye el isam.h */


/*
* printusi_i
* igual que printusi pero en alta intensidad
*/
printusi_i(pfi,campo)
register FI_DEF *pfi;
int campo;
{
   a_atri_campo(pfi,campo-1,' ',0,0,0);
   printcam(pfi,campo);
   a_atri_campo(pfi,campo-1,' ',1,0,0);
   return(0);
}

