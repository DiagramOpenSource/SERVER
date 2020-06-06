
#include <fgvsys.h>

/* Autor: Francisco Gonzalez Valiente */

#include <stdio.h>
#include <errno.h>
#include <malloc.h>

#ifdef FGVDOS
#ifndef WATCOM
#ifdef BORLANDWORLD
#include <alloc.h>
#else
heapcheck()
{
}
heapchecknode()
{
}
coreleft()
{
}
#endif
#endif
#endif

/* #define MEMODEBUG /* activacion */

#define _RWMALLOC
#include <rwmalloc.h>

#include <rtermin.h>

#ifdef MEMODEBUG
extern long maxasign = 0L;
#endif

static int pinta_free = -1;

extern int (*libera_virtual)() = NULL;

/* #define N_HEAP */

/*
static char *mi_reserva[20] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
static char flag_reserva[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static unsigned short size_reserva[20] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
*/

#ifdef ESDOS
#define HUGE (unsigned long)
#else
#define HUGE
#endif

#ifdef MEMODEBUG
busca_puntero(punt)
char *punt;
{
   int i;
   
   for (i=0;i<mem->totalpunteros;i++) {
      if ( punt == *(mem->punteros + i) ) return(i);
   }
   return(-1);
}
#endif


extern int balance = 0;

error_malloc()
{
   printf("\n\r ERROR EN ASIGNACION DE MEMORIA ...\n\r");
   printf(" NO HAY MEMORIA PARA OPERAR : OPERACION ABORTADA. \n\r");
   if (stdpan) {
      cierra_todo(1);
      fin_sig();
      Terfinal();
   }
   exit(2);
}

static char nulo[1024] = "\0";

#ifdef FGVDOS
#ifndef FGVWIN
#ifndef FGV32

extern char *getenv();

static void lo_que_hay()
{
   int i;
   char *pro;
   
   if (pinta_free == -1)
			{
      pro=getenv("MEMOCHECK");
      if (pro)
         pinta_free = 1;
      else
         pinta_free = 0;
			}
   
   
   if (stdpan && pinta_free)
   {
      i = Pcursor;
      cursor(2501);
      v_printd("%2d:",heapcheck());
      v_printl("%6ld ",coreleft());
      cursor(i/80*100+i%80+101);
   }
}

#endif
#endif
#endif

void rwfree(punt)
char *punt;
{
   int indice;
   
   balance--;
   
   if (!punt)
			{
      /* printf("free NULO");getchar(); */
      return;
			}
   if (punt == nulo) return;
   
#ifdef FGVDOS
#ifndef AGIEXTEND
#ifndef FGVWIN
#ifndef FGV32
#ifdef BORLANDWORLD
   if (pinta_free > 0 && (indice = heapchecknode(punt)) != 4)
			{
      printf("heapchecknode erroneo=[%d]",indice);
      return;
			}
#endif
#endif
#endif
#endif
#endif
   
   if (rwdebug != 1) {
      free(punt);
#ifdef FGVDOS
#ifndef FGVWIN
#ifndef FGV32
      lo_que_hay();
#endif
#endif
#endif
      return;
			}
   
#ifdef MEMODEBUG
   
   if (rw_puntero != (char *)0 && rw_puntero == punt) {
      printf("\n PUNTERO %lx ENCONTRADO EN FREE\n",punt);
      getchar();
			}
   
   if ((indice = busca_puntero(punt)) == -1) {
      printf("\n FREE %lx ILEGAL \n",punt);
      getchar();
      return;
			}
   
   *(mem->punteros + indice) = NULL;
   mem->totalasignada -= *(mem->longitudes + indice);
   *(mem->longitudes + indice) = 0;
   
#endif
   free(punt);
#ifdef FGVDOS
#ifndef FGVWIN
#ifndef FGV32
   lo_que_hay();
#endif
#endif
#endif
}


void *rwmalloc(tam)
unsigned int tam;
{
			char *punt;
         char *res0;
         char *res1;
         int indice,i;
#ifdef FGVDOS
         long actmemo;
         static long antmemo = 0;
#endif
         
         if (!tam) {return(NULL);}
         
         punt = malloc(tam);
         
#ifdef FGVDOS
#ifndef FGVWIN
#ifndef FGV32
         lo_que_hay();
#endif
#endif
#endif
         
         
         if (punt == NULL) {
            if (libera_virtual) {
               while ((*libera_virtual)(tam) > -1) {
                  punt = rwmalloc(tam); /* hay una recursion */
                  if (punt)
                  {
                     balance--;
                     break;
                  }
               }
            }
         }
         
         if (punt)
            balance++;
         
         if (punt == NULL  && rwdebug != 0) {
            error_malloc();
         }
         
         if (rwdebug != 1) return(punt);
         
#ifdef MEMODEBUG
         
         if (rw_puntero != (char *)0 && rw_puntero == punt) {
            printf("\n PUNTERO %lx ENCONTRADO EN MALLOC\n",punt);
            getchar();
         }
         
         if ((indice = busca_puntero(NULL)) == -1) {
            if (mem->totalpunteros == mem->totaltabla) {
               mem->totaltabla += 500;
               res0 = (char *)mem->punteros;
               res1 = (char *)mem->longitudes;
               printf("\n\r voy a pedir para %d MAXI ASIGNADA %ld",
                  mem->totaltabla,maxasign);
               if ( (mem->punteros = (char **)malloc(
                  mem->totaltabla * sizeof(char *))) == NULL)
                  error_debug();
               memcpy((char *)mem->punteros,res0,(mem->totaltabla - 500)*sizeof(char *));
               free(res0);
               if ((mem->longitudes = (unsigned int *)malloc(
                  mem->totaltabla * sizeof(unsigned int *))) == NULL)
                  error_debug();
               memcpy((char *)mem->longitudes,res1,(mem->totaltabla - 500)*sizeof(int *));
               
               free(res1);
               printf("\n\r ya he pedido para %d ",mem->totaltabla);
            }
            indice = mem->totalpunteros;
            mem->totalpunteros++;
         }
         *(mem->punteros + indice) = punt;
         *(mem->longitudes + indice) = tam;
         
         mem->totalasignada += (long)tam;
         if (mem->totalasignada > maxasign) maxasign = mem->totalasignada;
         
#endif
         
         return(punt);
}


void reset_reserva()
{
	int i;

	return;
#ifdef CESCKAKA
	for (i = 0;i < 20;i++)
	{
		if (flag_reserva[i])
		{
			printf("INCONSISTENCIA DEL SISTEMA NIVEL[%d]",i);getchar();
			continue;
		}
		if (mi_reserva[i])
		{
			rwfree(mi_reserva[i]);
		}
		mi_reserva[i] = NULL;
		flag_reserva[i] = 0;
		size_reserva[i] = 0;
	}
	return;
#endif
}

int ini_reserva(size,categoria)
unsigned size;
int categoria;
{
	unsigned int atencion = 0;
	char ant_estado = 0;
	void *old_punt;
	
	if (!size) return(0); /* no afecta */


    return(0); /**/
#ifdef CESCKAKA	
	if (mi_reserva[categoria])
	{
		if (size > size_reserva[categoria])
		{
			if (!flag_reserva[categoria])
			{
				rwfree(mi_reserva[categoria]);
				mi_reserva[categoria] = NULL;
				size_reserva[categoria] = 0;
			}
		}
		else
		{
			return(0);
		}
	}
	old_punt = mi_reserva[categoria];
	ant_estado = flag_reserva[categoria];
	mi_reserva[categoria] = NULL;
	flag_reserva[categoria] = 0;
	if ((mi_reserva[categoria] = rwmalloc(size)))
	{
		size_reserva[categoria] = size;
		return(0);
	}
	
	printf("Inicializacion de reserva no aceptada[%d]",categoria);getchar();
	
	if (old_punt)
	{
		mi_reserva[categoria] = old_punt;
		flag_reserva[categoria] = ant_estado;
		return(-1);
	}
	
	if (atencion)
	{
		if ((mi_reserva[categoria] = rwmalloc(atencion)))
		{
			size_reserva[categoria] = atencion;
			return(-1);
		}
		printf("FATAL NO SE PUEDE MANTENER LA MEMORIA MINIMA!!!(SUB_EJECUTA)");
		exit(2);
	}
	return(-1);
#endif
}

void reserva_free(punt,categoria)
char *punt;
int categoria;
{
	if (!punt) return;
	if (punt == nulo) return;

	rwfree(punt);/**/
	return;

#ifdef CESCKAKA
	if (mi_reserva[categoria] && punt == mi_reserva[categoria])
	{
		if (!flag_reserva[categoria])
		{
			printf("[%d]memoria reliberada",categoria);getchar();
			return;
		}
		flag_reserva[categoria] = 0;
		return;
	}
	rwfree(punt);
	return;
#endif
}

void *reserva_malloc(size,categoria)
unsigned size;
int categoria;
{
	void *punt;
	int omd = 0;
	
	if (!size) return(nulo); /* no afecta */

punt = rwmalloc(size);/**/
return(punt);

#ifdef CESCKAKA
	
	if (mi_reserva[categoria])
	{
	/*****
	if (size > size_reserva[categoria])
    {
    printf("NO ESTAN CALCULADOS LOS MAXIMOS DE RESERVA CORRECTOS!!!![%d][%d][%d]",size,size_reserva[categoria],categoria);getchar();
    }
		*****/
		if (!flag_reserva[categoria])
		{
			if (size > size_reserva[categoria])
			{
				if (ini_reserva(size,categoria))
					omd = 1;
			}
			if (!omd)
			{
				flag_reserva[categoria] = 1; /* asignado */
				return(mi_reserva[categoria]);
			}
		}
	}
	omd = rwdebug;
	rwdebug = 0;
	punt = rwmalloc(size);
	rwdebug = omd;
	if (!punt && mi_reserva[categoria] && !flag_reserva[categoria])
	{
		if (size > size_reserva[categoria])
			ini_reserva(size,categoria);
		flag_reserva[categoria] = 1; /* asignado */
		return(mi_reserva[categoria]);
	}
	return(punt);
#endif
}


/* esta funcion optimiza las malloc para strings en las cuales de momento solo
se les reserva un byte para el cero final, son strings vacios */

void *strmalloc(size)
unsigned size;
{
   char *pp;
	
   if (size == 1) {
      balance++;
	  nulo[0] = 0;
      return(nulo);
   }
   
   pp = rwmalloc(size);
   if (pp)
	   *pp = 0;
   return(pp);
}

void *texmalloc(size)
unsigned size;
{
   int i = rwdebug;
   char *p;
   rwdebug = 0;
   p = rwmalloc(size);
   rwdebug = i;
   return(p);
}

void *dirmalloc(tam)
unsigned tam;
{
   void *punt;
   punt = malloc(tam);
   if (punt)
      balance++;
   return(punt);
}


void *rwrealloc(punt,size)
unsigned char *punt;
unsigned size;
{
   unsigned char *pp;
   unsigned int  o_size;
   
   if (punt == nulo) {
      pp = rwmalloc(size);
      if (pp) {
         balance--;
         *pp = 0;
      }
      return(pp);
   }
   
   pp = realloc(punt,size);
   
   if (pp == NULL) {
      if (libera_virtual) {
         while ((*libera_virtual)(size) > -1) {
            pp = rwrealloc(punt,size);
            if (pp)
            {
               break;
            }
         }
      }
   }
   
   if (pp == NULL  && rwdebug != 0) {
      error_malloc();
   }
   return(pp);
}


void *texrealloc(punt,size)
unsigned char *punt;
unsigned size;
{
   int i = rwdebug;
   char *p;
   rwdebug = 0;
   p = rwrealloc(punt,size);
   rwdebug = i;
   return(p);
}


#ifdef MEMODEBUG

error_debug()
{
   pito(70);
   pito(71);
   pito(72);
   pito(73);
   pito(74);
   exit(2);
}

#endif
