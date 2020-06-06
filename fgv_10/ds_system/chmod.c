
#include <fgvsys.h>

#include <string.h>
#include <stdio.h>

int agichmod(dir1,dir2,dir3,dir4)
char *dir1,*dir2,*dir3,*dir4;
{
   char mandato[128];
   
   sprintf(mandato,"%s go+rw %s%s%s%s",rw_directorio("bin/agichmod"),dir1,dir2,dir3,dir4);
   filtropar(mandato);
   
#ifndef FGVWIN
#ifndef FGV32
   fin_sig();
#endif
#endif
   
   agi_system(mandato);
   
#ifndef FGVWIN
#ifndef FGV32
   ini_sig();
#endif
#endif
   
   return(0);
}
