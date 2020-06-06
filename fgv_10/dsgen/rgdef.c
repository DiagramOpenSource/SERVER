
#include <fgvsys.h>

#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef O_BINARY
#include <io.h>
#endif

#include <string.h>

#ifndef O_BINARY  /* compatibilidad xenix - dos */

#define O_BINARY 0
#define SEEK_SET 0
#define SEEK_CUR 1
#ifndef LARGESEEK
extern long rw_lseek_();
#endif
extern unsigned int rw_read_();
#endif


#define LOS_DEF
#include <rdef.h>


#define min(a,b)  (((a) < (b)) ? (a) : (b))
#define max(a,b)  (((a) > (b)) ? (a) : (b))

extern int (*fgv_serial_open_)( const char *filename, int oflag , int pmode );
extern long (*fgv_serial_seek_)( int handle, long offset, int origin );
extern int (*fgv_serial_read_)(int handle, void *buffer, unsigned int count );
extern int (*fgv_serial_write_)(int handle, const void *buffer, unsigned int count);
extern int (*fgv_serial_close_)( int handle );
extern int (*fgv_serial_lock_)( int handle, int mode, long nbytes );


static int  minint = -32767,
maxint = 32767;


extern short tipo_de_def;

/*  --------------------------------------------------------------
*  calcula_def
*  Calcula los parametros secundarios de los campos.
*  Estos se obtienen a partir de los datos primarios contenidos 
*  en la estructura del fichero de definicion a la cual apunta 'pfi'.
*  Los datos calculados se graban en dicha estructura. 
*  Datos primarios :
*                  qc,minimo,maximo,claves.
*  Datos secundarios :
*                  usi,offset,long,tipo,key.
*  En algunos casos se modifica minimo y maximo para evitar incongruencias.
*  Se utilizan parametros que pueden depender de hardware o de la imple-
*  mentacion de compilador (MAXINT,MAXLONG,...) que estan definidos
*  en el fichero <values.h> propio del sistema.
*  Los tipos y longitud de variables estan parametrizados como :
*      contenido           tipo                longitud
*      ---------------     ---------------     ------------------
*      string              CHARTIPO            CHARTAMA * qc
*      integer             INTTIPO             INTTAMA
*      long integer        LONGTIPO            LONGTAMA
*      float               No se utiliza ................
*      double              DOUBLETIPO          DOUBLETAMA
*      fecha               FECHATIPO           FECHATAMA 
*  --------------------------------------------------------------
*/
void calcula_campo(CAMPO_DEF *pcam)
{
	int qc,tipo,tama,decimal,j;
	double maxi,mini,a1,a2;
	char *maxstr,*minstr;
	char digitos[40];


	
	
	qc = (pcam->c_qc);
	maxstr = pcam->c_maximo;
	minstr = pcam->c_minimo;
	switch (qc) {
	case -2 :     /* es MEMO */
		pcam->c_tipo = LONGTIPO;
		pcam->c_long = LONGTAMA;
		strcpy(pcam->c_usi,"%ld");
		break;
	case -1 :     /* es fecha    */
		pcam->c_tipo = FECHATIPO;
		pcam->c_long = FECHATAMA;
		strcpy(pcam->c_usi,"%-10s");
		strcpy(maxstr,"");
		strcpy(minstr,"");
		break;
	case 0 :      /* es numerico : calculo segun minimo y maximo */
		if (!maxstr[0] && !minstr[0])
		{
				tipo = NULOTIPO;
				tama = NULOTAMA;
				pcam->c_usi[0] = 0;
		}
		else
		{
			a1=rwatof(maxstr);
			a2=rwatof(minstr);
			maxi = max(a1,a2);
			a1=rwatof(maxstr);
			a2=rwatof(minstr);
			mini = min(a1,a2);
			if (strlen(minstr) > 1 && minstr[0] == '0') {
				digitos[0] = '0';
				sprintf(digitos+1,"%d",len_numerico(minstr,maxstr));
			   }
			else
				sprintf(digitos,"%d",len_numerico(minstr,maxstr));
			decimal = max(decimales(maxstr),decimales(minstr));
			if (decimal > 0 || decimales_redondeo_campo(minstr,maxstr)) {
				tipo = DOUBLETIPO;
				tama = DOUBLETAMA;
				sprintf(pcam->c_usi,"%%%s.%dlf",digitos,decimal);
			   }
			else if ((mini < (double)minint) || (maxi > (double)maxint)) {
				tipo = DOUBLETIPO;
				tama = DOUBLETAMA;
				sprintf(pcam->c_usi,"%%%s.0lf",digitos);
			   }
			else {
				tipo = INTTIPO;
				tama = INTTAMA;
				sprintf(pcam->c_usi,"%%%sd",digitos);
			   }
		}
	   pcam->c_tipo = tipo;
	   pcam->c_long = tama;
	   break;
	default :     /* es string */
		pcam->c_tipo = CHARTIPO;
		pcam->c_long = CHARTAMA * qc;
		j = pcam->c_mante->c_qpl;
		if (j > 0 && j < qc)
			qc = j;
		sprintf(pcam->c_usi,"%%-%d.%ds",qc,qc);
		/*strcpy(maxstr,"");*/
		break;
	}
	
}

void calcula_def(fi)
FI_DEF *fi;
{
	int i,lfich,k,j;
	
	lfich = 0;
	for (i=0;i<fi->f_ncampos;i++) {
		calcula_campo(fi->f_campos[i]);
		fi->f_campos[i]->c_offset = lfich;
		lfich += fi->f_campos[i]->c_long;
	}
	fi->f_lfich = lfich;
	
	for (i=0;i<fi->f_ncampos;i++)
		fi->f_campos[i]->c_key = 0;
	for (i=fi->f_nclaves;i>0;i--) {
		for (j=fi->f_clave[i-1].k_ncampos;j>0;j--) {
			if((k = fi->f_clave[i-1].k_campo[j-1]) <= fi->f_ncampos && k)
				fi->f_campos[k-1]->c_key = i;   /* numero de orden de clave */
		}
	}
}


#ifdef ESMOTOROLA
static unsigned int graba_short(file,p,n)
int file;
short *p;
int n;
{
	short tmp;
	unsigned int r = 0;
	
	while(n > 0 && r >= 0)
	{
		tmp = *p;
		moto_short(&tmp);  	
		r += fgv_serial_write_(file,&tmp,2);
		p++;
		n -= 2;
	}
	return(r);
}
#else
static unsigned int graba_short(file,p,n)
int file;
char *p;
int n;
{
	return(fgv_serial_write_(file,p,n));
}
#endif


/*  --------------------------------------------------------------
 *  graba_def
 *  Graba los datos del fichero .def de nombre 'nfi'
 *  contenidos en la estructura '*fi'.
 *  Si todo va bien devuelve 0.
 *  Devuelve -1 si el fichero no es accesible o no se puede crear.
 *  --------------------------------------------------------------
 */
extern long nume_serie;



int graba_def(nfi,fi)
char *nfi;
FI_DEF *fi;
{
	CAMPO_DEF *ca[MAXCAM];
	char nomfi[O_MAXPATH];
	short i;
	unsigned short tamano = 0;
	unsigned short dato_e;
	unsigned short dato;
	unsigned long n_s;
	char *nulo = NULL;
    char *p;	
	long posicion;
	int j,file1;
	char *tmp;
	
	
	if (fi->f_fd == -1 || fi->grabable)
	{
		strcpy(nomfi,nfi);
        if ( !(p = strstr(nomfi,".mas")) || *(p+4) )
		    strcat(nomfi,".mas");
		if (fi->f_fd > -1)
		{
			fgv_serial_seek_(fi->f_fd,1000000000L,SEEK_SET);
			fgv_serial_lock_(fi->f_fd,0,1L);	    
			fgv_serial_close_(fi->f_fd);
		}
		if ((file1 = fgv_serial_open_(nomfi,O_CREAT | O_TRUNC | O_BINARY | O_RDWR,
			S_IREAD | S_IWRITE)) < 0) 
		{
			if (fi->f_fd > -1) fi->f_fd = -1;
			return(-1);
		}
		if (fi->f_fd > -1) 
		{
			fi->f_fd = file1;	 
			fgv_serial_seek_(fi->f_fd,1000000000L,SEEK_SET);
			if (fgv_serial_lock_(fi->f_fd,2,1L))
			{
				fi->grabable = 0;		   
				/* mal menor!! */
			}
			fgv_serial_seek_(fi->f_fd,0L,SEEK_SET);
		}
	}
	else
	{	 
		return(-1);
	}
	
	fi->tocado = 0;
	
	dato_e = FGVGEN_ID_MAS;
	tamano += (unsigned int)graba_short(file1,(char *)&dato_e,sizeof(short));
	dato_e = rand();
	tamano += (unsigned int)graba_short(file1,(char *)&dato_e,sizeof(short));
	dato_e = nume_code() ^ dato_e;
	tamano += (unsigned int)graba_short(file1,(char *)&dato_e,sizeof(short));
	n_s = nume_serie / 0xffff;
	dato = ((unsigned short)n_s) ^ dato_e;
	tamano += (unsigned int)graba_short(file1,(char *)&dato,sizeof(short));
	n_s = nume_serie % 0xffff;
	dato = ((unsigned short)n_s) ^ dato_e;
	tamano += (unsigned int)graba_short(file1,(char *)&dato,sizeof(short));
	dato = _Posicion() ^ dato_e;
	tamano += (unsigned int)graba_short(file1,(char *)&dato,sizeof(short));
	dato = fi->tipo_de_def ^ dato_e;
	tamano += (unsigned int)graba_short(file1,(char *)&dato,sizeof(short));
	dato = fi->version_de_def ^ dato_e;
	tamano += (unsigned int)graba_short(file1,(char *)&dato,sizeof(short));
	{
		char tmpxx[17];
		memset(tmpxx,0,17);
		tamano += (unsigned int)fgv_serial_write_(file1,tmpxx,17);
		strcpy(tmpxx,fi->modulo);
		tamano += (unsigned int)fgv_serial_write_(file1,tmpxx,9);
	}   
	dato_e = FGVGEN_ID_MAS;
	tamano += (unsigned int)graba_short(file1,(char *)&dato_e,sizeof(short)); 
	tamano += (unsigned int)graba_short(file1,(char *)&fi->listable,sizeof(short));
	tamano += (unsigned int)graba_short(file1,(char *)&fi->generable,sizeof(short));
	
	posicion = fgv_serial_seek_(file1,0L,SEEK_CUR);
	
	tamano += (unsigned int)graba_short(file1,(char *)&tamano,sizeof(short));
	
	tamano += (unsigned int)fgv_serial_write_(file1,fi->f_fichero,O_MAXPATH+1);
	tamano += (unsigned int)fgv_serial_write_(file1,(char *)&nulo,sizeof(char *));
	tamano += (unsigned int)fgv_serial_write_(file1,(char *)&nulo,sizeof(char *));
	tamano += (unsigned int)fgv_serial_write_(file1,(char *)&nulo,sizeof(char *));
	tamano += (unsigned int)fgv_serial_write_(file1,(char *)&nulo,sizeof(char *));
	tamano += (unsigned int)fgv_serial_write_(file1,(char *)&nulo,sizeof(char *));
	tamano += (unsigned int)fgv_serial_write_(file1,(char *)&nulo,sizeof(char *));
	tamano += (unsigned int)graba_short(file1,(char *)&fi->f_lfich,sizeof(short));
	tamano += (unsigned int)graba_short(file1,(char *)&fi->f_npanta,sizeof(short));
	tamano += (unsigned int)graba_short(file1,(char *)&fi->f_nclaves,sizeof(short));
	tamano += (unsigned int)graba_short(file1,(char *)&fi->f_ncampos,sizeof(short));
	tamano += (unsigned int)graba_short(file1,(char *)&fi->f_nrelaci,sizeof(short));
	tamano += (unsigned int)graba_short(file1,(char *)&fi->tipo_de_def,sizeof(short));
	
	
	for (i = 0;i<fi->f_npanta;i++) 
	{
		moto_short(&(fi->f_pantalla[i].p_campoini));
		moto_short(&(fi->f_pantalla[i].p_campofin));
		moto_short(&(fi->f_pantalla[i].p_posfin));
		moto_short(&(fi->f_pantalla[i].p_posref));
		tamano += (unsigned int)fgv_serial_write_(file1,(char *)(fi->f_pantalla+i),
			sizeof(PAN_DEF)); /* la de pantallas es igual */
		moto_short(&(fi->f_pantalla[i].p_campoini));
		moto_short(&(fi->f_pantalla[i].p_campofin));
		moto_short(&(fi->f_pantalla[i].p_posfin));
		moto_short(&(fi->f_pantalla[i].p_posref));
	}
	
	
	for (i=0;i<fi->f_ncampos;i++) 
	{
		ca[i] = fi->f_campos[i];
		tamano += (unsigned int)fgv_serial_write_(file1,(char *)&nulo,
			sizeof(char *));
		tamano += (unsigned int)fgv_serial_write_(file1,(char *)&nulo,
			sizeof(char *));
		tamano += (unsigned int)fgv_serial_write_(file1,(char *)&nulo,
			sizeof(char *));
		tamano += (unsigned int)fgv_serial_write_(file1,(char *)&nulo,
			sizeof(char *));
		tamano += (unsigned int)fgv_serial_write_(file1,(char *)&nulo,
			sizeof(char *));
		tamano += (unsigned int)fgv_serial_write_(file1,(char *)&nulo,
			sizeof(char *));
		tamano += (unsigned int)fgv_serial_write_(file1,(char *)&nulo,
			sizeof(char *));
		tamano += (unsigned int)fgv_serial_write_(file1,(char *)&nulo,
			sizeof(char *));
		tamano += (unsigned int)graba_short(file1,(char *)&ca[i]->c_mante->c_actual,
			sizeof(short));
		tamano += (unsigned int)graba_short(file1,(char *)&ca[i]->c_mante->c_modif,
			sizeof(short));
		tamano += (unsigned int)graba_short(file1,(char *)&ca[i]->c_mante->c_visual,
			sizeof(short));
		tamano += (unsigned int)graba_short(file1,(char *)&ca[i]->c_mante->c_lineal,
			sizeof(short));
		tamano += (unsigned int)graba_short(file1,(char *)&ca[i]->c_mante->c_qp,
			sizeof(short));
		tamano += (unsigned int)graba_short(file1,(char *)&ca[i]->c_mante->c_qpl,
			sizeof(short));
		tamano += (unsigned int)graba_short(file1,(char *)&ca[i]->c_qc,
			sizeof(short));
		tamano += (unsigned int)graba_short(file1,(char *)&ca[i]->c_offset,
			sizeof(short));
		tamano += (unsigned int)graba_short(file1,(char *)&ca[i]->c_long,
			sizeof(short));
		tamano += (unsigned int)graba_short(file1,(char *)&ca[i]->c_tipo,
			sizeof(short));
		tamano += (unsigned int)graba_short(file1,(char *)&ca[i]->c_key,
			sizeof(short));
		tamano += (unsigned int)graba_short(file1,(char *)&ca[i]->c_fflag,
			sizeof(short));
	}
	
	for (i = 0;i<fi->f_nclaves;i++) 
	{
		tamano += (unsigned int)fgv_serial_write_(file1,(char *)&nulo,sizeof(char *));
		tamano += (unsigned int)fgv_serial_write_(file1,(char *)&nulo,sizeof(char *));
		tamano += (unsigned int)graba_short(file1,(char *)&fi->f_clave[i].k_ncampos,
			sizeof(short));
		tamano += (unsigned int)graba_short(file1,(char *)&fi->f_clave[i].k_fflag,
			sizeof(short));
	}
	
	for (i = 0;i<fi->f_nclaves;i++) {
		for (j = 0;j < fi->f_clave[i].k_ncampos;j++)
			tamano += (unsigned int)graba_short(file1,(char *)(fi->f_clave[i].k_campo+j),
			sizeof(short));
		for (j = 0;j < fi->f_clave[i].k_ncampos;j++)
			tamano += (unsigned int)graba_short(file1,(char *)(fi->f_clave[i].k_orden+j),
			sizeof(short));
	}
	
	for (i=0;i<fi->f_nrelaci;i++) {
		tamano += (unsigned int)fgv_serial_write_(file1,(char *)&nulo,
			sizeof(char *));
		tamano += (unsigned int)fgv_serial_write_(file1,(char *)&nulo,
			sizeof(char *));
		tamano += (unsigned int)fgv_serial_write_(file1,(char *)&nulo,
			sizeof(char *));
		tamano += (unsigned int)graba_short(file1,(char *)&fi->f_relaci[i]->r_obliga,
			sizeof(short));
		tamano += (unsigned int)graba_short(file1,(char *)&fi->f_relaci[i]->r_leer,
			sizeof(short));
	}
	
    /* segundo bloque */
	tamano += (unsigned int)fgv_serial_write_(file1,fi->f_titulo,
		strlen(fi->f_titulo)+1);
	
	for (i=0;i<fi->f_ncampos;i++) {
		tamano += (unsigned int)fgv_serial_write_(file1,ca[i]->c_usi,
			strlen(ca[i]->c_usi)+1);
		tamano += (unsigned int)fgv_serial_write_(file1,ca[i]->c_mante->c_mensaje,
			strlen(ca[i]->c_mante->c_mensaje)+1);
		tamano += (unsigned int)fgv_serial_write_(file1,ca[i]->c_mante->c_defecto,
			strlen(ca[i]->c_mante->c_defecto)+1);
		tamano += (unsigned int)fgv_serial_write_(file1,ca[i]->c_minimo,
			strlen(ca[i]->c_minimo)+1);
		tamano += (unsigned int)fgv_serial_write_(file1,ca[i]->c_maximo,
			strlen(ca[i]->c_maximo)+1);
	}
	
	for (i=0;i<fi->f_nrelaci;i++) {
		tamano += (unsigned int)fgv_serial_write_(file1,fi->f_relaci[i]->r_fichero,
			strlen(fi->f_relaci[i]->r_fichero)+1);
		tamano += (unsigned int)fgv_serial_write_(file1,fi->f_relaci[i]->r_mfichero,
			strlen(fi->f_relaci[i]->r_mfichero)+1);
		for (j = 0;j < MAXCAMPOSPORCLAVE +1;j++) {
			tamano += (unsigned int)graba_short(file1,
				(char *)&fi->f_relaci[i]->r_campo[j],
				sizeof(short));
			if (fi->f_relaci[i]->r_campo[j] == 0) break;
		}
	}
	
	for (i=0;i<fi->f_ncampos;i++) {
		fgv_serial_write_(file1,fi->f_campos[i]->c_nombre,strlen(fi->f_campos[i]->c_nombre)+1);
		fgv_serial_write_(file1,fi->f_campos[i]->c_lista,strlen(fi->f_campos[i]->c_lista)+1);
		fgv_serial_write_(file1,fi->f_campos[i]->c_mante->c_relacion,strlen(fi->f_campos[i]->c_mante->c_relacion)+1);
	}
	
	fgv_serial_seek_(file1,posicion,SEEK_SET);
	/* ojo tiene 50 bytes de mas */
	graba_short(file1,(char *)&tamano,sizeof(short));
	
	
	{
		char *pi;
		long tam1;
		char *total;
		unsigned short checksum = 0;
		unsigned short suma = 0;
		/* grabar el check-sum */
		tam1 = fgv_serial_seek_(file1,0L,SEEK_END) - 40L;
		total = malloc(tam1);
		memset(total,0,tam1);
		fgv_serial_seek_(file1,42L,SEEK_SET);
		fgv_serial_read_(file1,total,tam1-2);
		for (pi = total;pi < (total+tam1-1);pi += sizeof(short))
		{
			checksum ^= *((unsigned short *)pi);
			suma += *((unsigned short *)pi);
		}
		free(total);
		fgv_serial_seek_(file1,16L,SEEK_SET);
		fgv_serial_write_(file1,&suma,sizeof(short));
		fgv_serial_write_(file1,&checksum,sizeof(short));
	}
	
	fgv_serial_seek_(file1,posicion+sizeof(short),SEEK_SET);
	
	if (fi->f_fd == -1)
	{
		fgv_serial_close_(file1);
	}
	
	return(0);
}

