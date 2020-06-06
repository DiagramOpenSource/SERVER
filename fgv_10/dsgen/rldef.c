

#include <fgvsys.h>

#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef O_BINARY
#include <io.h>
#endif

#include <string.h>
#include <malloc.h>

#ifdef FGVNFS
int x_share_open(char *path,int acceso,unsigned int mode);
int x_rw_close_(int file);
long x_rw_lseek_(int file,long pos,int modo);
int x_rw_read_(int file,char *texto,int tam);
int x_rw_write_(int file,char *texto,int tam);
int x_rw_fputs_(char *texto,FILE *file);

int (*fgv_serial_open_)( const char *filename, int oflag , int pmode ) = x_share_open;
long (*fgv_serial_seek_)( int handle, long offset, int origin ) = x_rw_lseek_;
int (*fgv_serial_read_)(int handle, void *buffer, unsigned int count ) = x_rw_read_;
int (*fgv_serial_write_)(int handle, const void *buffer, unsigned int count) = x_rw_write_;
int (*fgv_serial_close_)( int handle ) = x_rw_close_;
int (*fgv_serial_lock_)( int handle, int mode, long nbytes ) = NULL;

#else
int (*fgv_serial_open_)( const char *filename, int oflag , int pmode ) = rw_open_;
long (*fgv_serial_seek_)( int handle, long offset, int origin ) = _rw_lseek_;
int (*fgv_serial_read_)(int handle, void *buffer, unsigned int count ) = rw_read_;
int (*fgv_serial_write_)(int handle, const void *buffer, unsigned int count) = rw_write_;
int (*fgv_serial_close_)( int handle ) = rw_close_;
int (*fgv_serial_lock_)( int handle, int mode, long nbytes ) = locking;
#endif

#ifndef O_BINARY  /* compatibilidad xenix - dos */

#define O_BINARY 0
#define SEEK_SET 0
#define SEEK_CUR 1
#ifndef LARGESEEK
extern long fgv_serial_seek_();
#endif
extern unsigned int fgv_serial_read_();

#endif


/*#include <rtermin.h>*/
/*#include <rwmalloc.h>*/

#define LOS_DEF
#include <rdef.h>

extern long atol();

extern char *quita_path();

unsigned short tipo_de_def = 0;
short lectura_def_old = 0;
unsigned short def_varmemo = 0;

extern unsigned short local_term[52];
extern unsigned short local_term_[52];

extern long nume_serie;

int es_no_listado = 0;

static unsigned char *apuntador;
static unsigned char *pos_apuntador;

int va_a_bloquear_def = 0;

#define lee_texto(a,b) lee_texto__(a,b,250)

unsigned short nume_code();
unsigned short nume_mochila();


void free_def(FI_DEF *fi,int man)
{
   int i;

	for (i=0;i<fi->f_ncampos;i++) 
	{
      free((char *)(fi->f_campos[i]->c_nombre));
      free((char *)(fi->f_campos[i]->c_lista));
      free((char *)(fi->f_campos[i]->c_minimo));
		free((char *)(fi->f_campos[i]->c_maximo));
      free((char *)(fi->f_campos[i]->c_usi));
      /*if ( man == 1 ) {*/
	 free((char *)(fi->f_campos[i]->c_mante->c_mensaje));
	 free((char *)(fi->f_campos[i]->c_mante->c_defecto));
	 free((char *)(fi->f_campos[i]->c_mante->c_relacion));
	 free((char *)(fi->f_campos[i]->c_mante));
	 /*}*/
      free((char *)(fi->f_campos[i]));
		}
	/*if ( man == 1 || man == 2 ) {*/
      for (i=0;i<fi->f_nrelaci;i++) {
	 free((char *)(fi->f_relaci[i]->r_fichero));
	 free((char *)(fi->f_relaci[i]->r_mfichero));
	 free((char *)(fi->f_relaci[i]));
	 }
		/*}*/
	free((char *)(fi->f_buf));
	free((char *)(fi));
}


static unsigned int agi_carga_(p,n)
unsigned char *p;
unsigned int n;
{
memcpy(p,pos_apuntador,n);
pos_apuntador += n;
return(n);
}

static void agi_salta_(n)
long n;
{
pos_apuntador += n;
}



static int lee_texto__(file1,texto,maximus)
int file1;
char *texto;
int maximus;
{
int j;
  for(j = 0;j < maximus;j++) 
     {
	 agi_carga_(texto+j,1);
	 if ( !*(texto+j) ) break;
	 }
return(j);
}

static int tam_texto(file1)
int file1; /* por  razones historicas ???!!!*/
{
int j;
char texto;

  j = 0;
  while(-1) 
     {
	 agi_carga_(&texto,1);
	 j++;
	 if ( !texto ) break;	 
	 }
  agi_salta_(-j);
return(j);
}


int lee_def(char *nfi,FI_DEF *fi,int man)
{
	CAMPO_DEF *ca[MAXCAM];
	unsigned short i;
	int solo;
	char nomfi[O_MAXPATH];
	short j;
	int file1;
	char texto[250],*tmp;
	unsigned short tamano = 0;
	long pos_final;
	unsigned short dato_e;
	unsigned short dato;
	unsigned long n_s;
	int control_s = 0;    

	fi->tipo_de_def = 0xffff;
	fi->tocado = 0;	

	if ((man/100) == 77) {
		solo = 0;
		file1 = (int)nfi;
		}
	else {	 
		solo = 1;
		strcpy(nomfi,nfi);
		if ((man/100) != 99) 
		   strcat(nomfi,".mas");
		else
		   { 
		   man %= 100;
		   solo++;
		   }
		if ((file1 = fgv_serial_open_(nomfi,O_BINARY | O_RDONLY,0)) < 0) 
		    {
			return(-1);
			}
		}
	if (solo == 0) man = man % 100;

	pos_final = fgv_serial_seek_(file1,0L,SEEK_END);

	fgv_serial_seek_(file1,0L,SEEK_SET);

   fgv_serial_read_(file1,(char *)&dato,sizeof(short));
   moto_short(&dato);
   if (dato != FGVGEN_ID_MAS)
		{
      if (solo)
        {
        fgv_serial_close_(file1);
		  }
		return(-1);
		}

	fgv_serial_read_(file1,(char *)&dato,sizeof(short));
	moto_short(&dato);
	fgv_serial_read_(file1,(char *)&dato_e,sizeof(short));
	moto_short(&dato_e);
	dato ^= dato_e;
#ifndef FGVMEMORYFILE
	if (dato != nume_code())
		{
		control_s++;
/*		if (solo)
		  {
		  fgv_serial_close_(file1);
		  }
		return(-1);
 */
		}
#endif
	fgv_serial_read_(file1,(char *)&dato,sizeof(short));
	n_s = (long)(dato ^ dato_e) * 0xffffL;
	fgv_serial_read_(file1,(char *)&dato,sizeof(short));
	n_s += (dato ^ dato_e);
	/*
	if (n_s != nume_serie)
		{
		control_s++;
 /*
		if (solo)
		  {
		  fgv_serial_close_(file1);
		  }
		return(-1);
 */
	/*	} */    
	fgv_serial_read_(file1,(char *)&dato,sizeof(short));
	dato = dato ^ dato_e;
	dato = _Posicion(); /* !!!! */
/*
	if (dato != _Posicion())
		{
		control_s++;
 
		if (solo)
		  {
		  fgv_serial_close_(file1);
		  }
		return(-1);
 
		}*/
	fgv_serial_read_(file1,(char *)&dato,sizeof(short));
	fi->tipo_de_def = dato ^ dato_e;
	fgv_serial_read_(file1,(char *)&dato,sizeof(short));
	fi->version_de_def = dato ^ dato_e;

#ifndef FGVMEMORYFILE
	if (fi->tipo_de_def != 0xffff && fi->version_de_def != 0xffff && control_s && !es_no_listado)
		{
		if (solo)
			 fgv_serial_close_(file1);
		return(-1);
		}
#endif

	fgv_serial_seek_(file1,33L,SEEK_SET); /* inicio datos ... anterior cabecera */
    fgv_serial_read_(file1,fi->modulo,9);
    fgv_serial_read_(file1,(char *)&tamano,sizeof(short));
	if (tamano == FGVGEN_ID_MAS)
	    {
		fgv_serial_read_(file1,(char *)&fi->listable,sizeof(short));
		fgv_serial_read_(file1,(char *)&fi->generable,sizeof(short));
		}
	else
	    {
		fi->listable = 0;
		fi->generable = 0;
		fgv_serial_read_(file1,(char *)&tamano,sizeof(short));
		fgv_serial_read_(file1,(char *)&tamano,sizeof(short));
		}

	fgv_serial_read_(file1,(char *)&tamano,sizeof(short));
	moto_short(&tamano);
	i = (unsigned short)pos_final - 50;
	apuntador = malloc(i);
	pos_apuntador = apuntador;
	fgv_serial_read_(file1,apuntador,i);


	tamano -= agi_carga_(fi->f_fichero,O_MAXPATH+1);
	agi_salta_((long)(sizeof(char *) * 6));
	tamano -= (sizeof(char *) * 6);
	tamano -= agi_carga_((char *)&fi->f_lfich,sizeof(short));
	moto_short(&fi->f_lfich);
	tamano -= agi_carga_((char *)&fi->f_npanta,sizeof(short));
	moto_short(&fi->f_npanta);
	tamano -= agi_carga_((char *)&fi->f_nclaves,sizeof(short));
	moto_short(&fi->f_nclaves);
	tamano -= agi_carga_((char *)&fi->f_ncampos,sizeof(short));
	moto_short(&fi->f_ncampos);
	tamano -= agi_carga_((char *)&fi->f_nrelaci,sizeof(short));
	moto_short(&fi->f_nrelaci);
	tamano -= agi_carga_((char *)&fi->f_fd,sizeof(short));

#ifndef FGVMEMORYFILE
		if (fi->tipo_de_def != (unsigned short)fi->f_fd && !es_no_listado)
		{
		if (solo)
		  {
		  fgv_serial_close_(file1);
		  }
		return(-1);
		}
#endif

	fi->f_fd = (short)-1;

	for (i = 0;i<fi->f_npanta;i++) {
		 tamano -= agi_carga_((char *)(fi->f_pantalla+i),sizeof(PAN_DEF));
			/* la de pantallas es igual */
		 moto_short(&(fi->f_pantalla[i].p_campoini));
		 moto_short(&(fi->f_pantalla[i].p_campofin));
		 moto_short(&(fi->f_pantalla[i].p_posfin));
		 moto_short(&(fi->f_pantalla[i].p_posref));
		 }


	for (i=0;i<fi->f_ncampos;i++) {
		ca[i] = (CAMPO_DEF *)malloc(sizeof(CAMPO_DEF));
		ca[i]->c_mante = (MANTE_DEF *)malloc(sizeof(MANTE_DEF));
		agi_salta_((long)(sizeof(char *) * 8));
		tamano -= (sizeof(char *) * 8);
		tamano -= agi_carga_((char *)&ca[i]->c_mante->c_actual,sizeof(short));
		moto_short(&ca[i]->c_mante->c_actual);
		tamano -= agi_carga_((char *)&ca[i]->c_mante->c_modif,sizeof(short));
		moto_short(&ca[i]->c_mante->c_modif);
		tamano -= agi_carga_((char *)&ca[i]->c_mante->c_visual,sizeof(short));
		moto_short(&ca[i]->c_mante->c_visual);
		tamano -= agi_carga_((char *)&ca[i]->c_mante->c_lineal,sizeof(short));
		moto_short(&ca[i]->c_mante->c_lineal);
		tamano -= agi_carga_((char *)&ca[i]->c_mante->c_qp,sizeof(short));
		moto_short(&ca[i]->c_mante->c_qp);
		tamano -= agi_carga_((char *)&ca[i]->c_mante->c_qpl,sizeof(short));
		moto_short(&ca[i]->c_mante->c_qpl);
		tamano -= agi_carga_((char *)&ca[i]->c_qc,sizeof(short));
		moto_short(&ca[i]->c_qc);
		tamano -= agi_carga_((char *)&ca[i]->c_offset,sizeof(short));
		moto_short(&ca[i]->c_offset);
		tamano -= agi_carga_((char *)&ca[i]->c_long,sizeof(short));
		moto_short(&ca[i]->c_long);
		tamano -= agi_carga_((char *)&ca[i]->c_tipo,sizeof(short));
		moto_short(&ca[i]->c_tipo);
		tamano -= agi_carga_((char *)&ca[i]->c_key,sizeof(short));
		moto_short(&ca[i]->c_key);
		tamano -= agi_carga_((char *)&ca[i]->c_fflag,sizeof(short));
		moto_short(&ca[i]->c_fflag);
		fi->f_campos[i] = ca[i];
		}

	for (i = 0;i<fi->f_nclaves;i++) {

		 tamano -= agi_carga_((char *)&tmp,sizeof(char *));
		 tamano -= agi_carga_((char *)&tmp,sizeof(char *));

		 tamano -= agi_carga_((char *)&fi->f_clave[i].k_ncampos,sizeof(short));
		 tamano -= agi_carga_((char *)&fi->f_clave[i].k_fflag,sizeof(short));
		 moto_short(&(fi->f_clave[i].k_ncampos));
		 moto_short(&(fi->f_clave[i].k_fflag));
		 }

	for (i = 0;i<fi->f_nclaves;i++) {
		 for (j = 0;j < fi->f_clave[i].k_ncampos;j++)
			 {
	  tamano -= agi_carga_((char *)(fi->f_clave[i].k_campo+j),sizeof(short));
	  moto_short(&(fi->f_clave[i].k_campo[j]));
	  }
		 for (j = 0;j < fi->f_clave[i].k_ncampos;j++)
			 {
	  tamano -= agi_carga_((char *)(fi->f_clave[i].k_orden+j),sizeof(short));
	  moto_short(&(fi->f_clave[i].k_orden[j]));
	  }
		 }

	for (i=0;i<fi->f_nrelaci;i++) {
		fi->f_relaci[i] = (REL_DEF *)malloc(sizeof(REL_DEF));
		agi_salta_((long)(sizeof(char *) * 3));
		tamano -= (sizeof(char *) * 3);
		tamano -= agi_carga_((char *)&fi->f_relaci[i]->r_obliga,sizeof(short));
		moto_short(&fi->f_relaci[i]->r_obliga);
		tamano -= agi_carga_((char *)&fi->f_relaci[i]->r_leer,sizeof(short));
		moto_short(&fi->f_relaci[i]->r_leer);
		}

	/* segundo bloque */
	def_varmemo = tamano - 48;

	i = 0;
	do
	  {
	  agi_carga_(fi->f_titulo+i,1);
	  if (i > 19 || !*(fi->f_titulo+i) ) break;
	  i++;
	  } while(-1);


	for (i=0;i<fi->f_ncampos;i++) {
		j = lee_texto(file1,texto);
		ca[i]->c_usi = malloc(j+1);
		strcpy(ca[i]->c_usi,texto);

		j = lee_texto(file1,texto);
		ca[i]->c_mante->c_mensaje = malloc(j+1);
		strcpy(ca[i]->c_mante->c_mensaje,texto);

		j = lee_texto(file1,texto);
		ca[i]->c_mante->c_defecto = malloc(j+1);
		strcpy(ca[i]->c_mante->c_defecto,texto);

		j = lee_texto(file1,texto);
		ca[i]->c_minimo = malloc(j+1);
		strcpy(ca[i]->c_minimo,texto);

		j = lee_texto(file1,texto);
		ca[i]->c_maximo = malloc(j+1);
		strcpy(ca[i]->c_maximo,texto);
		}


	for (i=0;i<fi->f_nrelaci;i++) {
		j = lee_texto(file1,texto);
		fi->f_relaci[i]->r_fichero = malloc(j+1);
		strcpy(fi->f_relaci[i]->r_fichero,texto);

		j = lee_texto(file1,texto);
		fi->f_relaci[i]->r_mfichero = malloc(j+1);
		strcpy(fi->f_relaci[i]->r_mfichero,texto);

		for (j = 0;j < MAXCAMPOSPORCLAVE +1;j++) {
			 agi_carga_((char *)&fi->f_relaci[i]->r_campo[j],sizeof(short));
			 moto_short(&(fi->f_relaci[i]->r_campo[j]));
			 if (fi->f_relaci[i]->r_campo[j] == 0) break;
	  }
		for (;j < MAXCAMPOSPORCLAVE +1;j++)
			 fi->f_relaci[i]->r_campo[j] = 0;
		}

	for (i=0;i<fi->f_ncampos;i++) {
		j = lee_texto(file1,texto);
		fi->f_campos[i]->c_nombre = malloc(j+1);
		strcpy(fi->f_campos[i]->c_nombre,texto);
		j = lee_texto(file1,texto);	    
		if (j < 2 || texto[0] != 13)
		   {
		   sprintf(texto,"C%d",i);
		   j = strlen(texto);
		   }		
		fi->f_campos[i]->c_lista = malloc(j+1);
		strcpy(fi->f_campos[i]->c_lista,texto);		   

        j = tam_texto(file1);		
		fi->f_campos[i]->c_mante->c_relacion = malloc(j+5); /* para el free_def */
		lee_texto__(file1,fi->f_campos[i]->c_mante->c_relacion,j+5);
		}

	free(apuntador);

	  fi->f_buf = malloc(1); /* solo para free_def */

      if (solo)
        {
	    fgv_serial_close_(file1);
		}

	  for (i = 0;i < 50;i++)
		 {
			 if (fi->tipo_de_def == local_term[i]) break;
		 }

	fi->f_fichero[38] = 3;
	fi->f_fichero[39] = 3;
	
    if (solo == 1)
	   {
	   strcpy(fi->f_fichero+40,quita_path(nfi));
	   }
	

#ifdef CONPROTE
	 if (i >= 50)
		 {
		 if (local_term[50] > fi->tipo_de_def || local_term[51] < fi->tipo_de_def)
		     {
			 if (fi->tipo_de_def == 0xffff)
			     {
                 fi->tipo_de_def = local_term[0];
				 fi->version_de_def = local_term_[0];
				 return (0);
				 }
			 /*
			 if (stdpan)
			     {
				 cursor(2501);
				 v_printd("%05d",fi->tipo_de_def);
			     }
			  */
			 for (i=0;i<fi->f_ncampos;i++) 
			   {
               fi->f_campos[i]->c_mante->c_relacion[0] = 0;
			   }
			 return(1); /* perdida de memoria ... */
		     }
	 }
#endif
return(0);
}


