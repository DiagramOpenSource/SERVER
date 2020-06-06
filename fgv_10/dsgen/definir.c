
#include <fgvsys.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef O_BINARY

#include <io.h>

#else

#define ES_UNIX

#endif


#ifndef O_BINARY  /* compatibilidad xenix - dos */

#define O_BINARY 0
#define SEEK_SET 0
#define SEEK_CUR 1
#ifndef LARGESEEK
extern long rw_lseek_();
#endif
extern unsigned int rw_read_();
#endif

#include <signal.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <time.h>
#include <sys/timeb.h>

/*#include <rwmalloc.h>*/
#include <rtermin.h>
#include <rdef.h>
#include <teclas.h>
#include <rfecha.h>
#include <rentra.h>
#define _GENERADOR
#define _MANTENIMIENTO
#include <rmante.h>
#define _CALCULOS
#include <rcalcul.h>
#define _COMPILADOR
#include <rcompi.h>

#define strmalloc malloc

int  indice_def_local[48] = {-1,-1,-1,-1,}; /* MAXIMO ACTUAL POR SISTEMA DE CODIFICACION DE CAMPOS FICHERO+VARIABLE */
int  nindice_def_local = 0;
int en_ini_par = 0;
int compilando_un_prc = 0;
int minimun = 0;
short fini_par[MAXDEF];
int c_linea = 0;
FILE *c_error = stdout;
int e_nivel = 0;
int c_nerror;
int error_break = 0;
char *tabla_defbucle[MAX_BUCLE] = {NULL,NULL,};
    
char *nombre_defs[MAXDEF*2];
char ventana_defs[MAXDEF*2];

char dirbase[51];
char dirbass[51];
char dirbase_[51];

int cont_defs = 0;

char notacion_dir_aplicacion[256];

char c_nombrecal[300];

extern unsigned short tam_gene_def[MAXDEF];
extern unsigned short def_varmemo;

static RELAS local_mpfirel;

FI_DEF *mpfi_maximo = NULL;

long nume_serie = 999001L;

unsigned short local_term[52];
unsigned short local_term_[52];

short _fgv_version = 6;
short i3 = 11;

int ya_esta_avisado = 0;

unsigned short _Posicion() {return(777);}


char **llamada_interna = NULL;
short nllamada_interna = 0;

int oculto = 0;
int primero = 0;

int nume_code()
{
return( 00000 );
}

int nume_mochila()
{
return( 00000 );
}

int _un_error(p1,p2,m)
char *p1,*p2,*m;
{
int i;
char tmp[1024];

c_nerror++;
if ((strlen(p1) + strlen(p2)) > 60) {
   i = 60 - strlen(p1);
   if (i > -1)
      p2[i] = '\0';
   else {
      p2[0] = '\0';
      p1[59] = '\0';
      }
   }
sprintf(tmp,"<%s>[%d] %s %s \"%s\"",c_nombrecal,c_linea,m,p1,p2);
if (!oculto) 
   {
   pprint2(101,tmp);
   }
return(0);
}

int un_error(p1,p2)
char *p1,*p2;
{
return(_un_error(p1,p2,"ERROR"));
}

int un_aviso(p1,p2)
char *p1,*p2;
{
return(_un_error(p1,p2,"AVISO"));
}

int error_sintaxis(pr)
char *pr;
{
return(un_error("de sintaxis",pr));
}

int aviso(int p,char *m)
{
	char tmp[10];
	tmp[0] = '!';
	tmp[1] = 0;
return(un_aviso(m,tmp));
}


/* buscal el fichero por el nombre de def */
int ind_def_rel(fichero)
char *fichero;
{
	int r;
	
    for (r = 0;r < nfi;r++) {
		
		if (r == 48) continue;
		
		if (!nombre_defs[r]) continue;
		
		if (!strcmp(quita_path(nombre_defs[r]),quita_path(fichero))) {
			return(r);
		}
		
	}
	
	return(-1);
}

int ind_def_rel_x(fichero)
char *fichero;
{
	int r;
	
    for (r = 0;r < nfi;r++) {
		
		if (r == 48) continue;
		
		if (!nombre_defs[r]) continue;
		
		if (!strcmp(quita_path(nombre_defs[r]),quita_path(fichero))) 
        {
            if (!r && quita_path(nombre_defs[r]) == nombre_defs[r] && fichero[0] == ':')
            {
                free(nombre_defs[r]);
                nombre_defs[r] = malloc(strlen(fichero)+2);
                strcpy(nombre_defs[r],fichero);
            }
			return(r);
		}
		
	}
	
	return(-1);
}


int ind_def_rel_f(fichero)
char *fichero;
{
	int i;
	for (i = 0;i < totalfichs;i++) {
		if (!strcmp(quita_path(fichs[i]->fichero),quita_path(fichero)))
		{
			return(fichs[i]->indice);
		}
	}
	return(-1);
}

/* Busca el fichero por el nombre de datos */
int ind_rel(fichero)
char *fichero;
{
	register int j;
	
	for ( j=0;
	j<nfi;
	j++ )
    {
		if (j == 48) continue;
		if ( !strcmp( quita_path(vg_mpfi(j)->f_fichero),quita_path(fichero)) ) break;
    };
	if (j<nfi)
		return(j);
	else {
		if (!cont_defs) {
			return(ind_def_rel(fichero));
		}
		return(-1);
	}
}


int ind_rel_x(fichero)
char *fichero;
{
	register int j;
	
	for ( j=0;
	j<nfi;
	j++ )
    {
		if (j == 48) continue;
		if ( !strcmp( quita_path(vg_mpfi(j)->f_fichero),quita_path(fichero)) ) break;
    };
	if (j<nfi)
    {
        if (fichero[0] == ':' && !j)
        {
            int r = j;
            if (quita_path(nombre_defs[r]) == nombre_defs[r])
            {
                free(nombre_defs[r]);
                nombre_defs[r] = malloc(strlen(fichero)+2);
                strcpy(nombre_defs[r],fichero);
            }

        }
		return(j);
    }
	else {
		if (!cont_defs) {
			return(ind_def_rel_x(fichero));
		}
		return(-1);
	}
}


int libera_extras(pfi)
FI_DEF *pfi;
{
	int nc = pfi->f_ncampos;
	int i;

    if (pfi == mpfi_maximo) return(0);
	
	for(i = 0;i < nc;i++) {
		if (!(pfi->f_campos[i]->c_key)) {
			free(pfi->f_campos[i]->c_mante->c_mensaje);
			pfi->f_campos[i]->c_mante->c_mensaje = strmalloc(1);
			*(pfi->f_campos[i]->c_mante->c_mensaje) = '\0';
		}
	}

    return(0);
	
}

/* esto se ejecuta solo cuendo se a¤ade un def nuevo */
int comprueba_def(nombre)
char *nombre;
{
	char nombre_1[82];
	int r;
	for (r = 0;r < nfi;r++) {
		
		if (r == 48) continue;
		
		if (!nombre_defs[r]) continue;
		
		if (!strcmp(quita_path(nombre_defs[r]),quita_path(nombre))) {
			if (!cont_defs) {
				un_error("­­­­­ DEF REPETIDO !!!!! ->",nombre);
				return(0);
			}
			else
				un_aviso("Def repetido ->",nombre);
		}
	}
	
	if (nombre_defs[nfi]) 
    {
		rw_error(60);
	}
	
	/***
	if (nombre != quita_path(nombre) && nombre[0] == ':' && nombre[1] != '/') {
	nombre_defs[nfi] = strmalloc(strlen(nombre) + 1);
	strcpy(nombre_defs[nfi],nombre);
	}
	else {
	**/
	nombre_defs[nfi] = strmalloc(strlen(nombre) + 1);
	strcpy(nombre_defs[nfi],nombre);
	/*
	strcpy(nombre_defs[nfi],quita_path(nombre));
	}
	*/
	mpfi[nfi]->f_fichero[38] = 3;
	mpfi[nfi]->f_fichero[39] = 3;
	strcpy(mpfi[nfi]->f_fichero+40,quita_path(nombre_defs[nfi]));
	tam_gene_def[nfi] = def_varmemo;
}

int localiza_ipfi(ipfi)
	int ipfi;
{
	if (en_ini_par)
		return(ipfi);
	if (!compilando_un_prc)
		return(ipfi);
	if (ipfi < 0 || ipfi >= nindice_def_local)
		return(ipfi);
	if (indice_def_local[ipfi] == -1)
	{
		aviso(0,"Error indice def local");
		return(48);
	}
	return(indice_def_local[ipfi]);
}



/* no virtual */
extern FI_DEF *vg_mpfi(ipfi)
	int ipfi;
{
	ipfi = localiza_ipfi(ipfi);
	if (ipfi >= 1000 || ipfi == 48)
	{
		/* referencia a fichero */
		return(mpfi_maximo);
	}
	if (!mpfi[ipfi])
	{
		return(mpfi_maximo);
	}
	return(mpfi[ipfi]);
}

extern RELAS *vg_mpfirel(ipfi)
int ipfi;
{
    ipfi = localiza_ipfi(ipfi);
	if (ipfi >= 1000 || !mpfirel[ipfi])
	{
		memset(&local_mpfirel,sizeof(local_mpfirel),0);        
		return( &local_mpfirel );
		
		/* referencia a fichero */
		/*return(mpfi_maximo);*/
        /*aviso(0,"error interno de mpfirel");
		*/
	}
	return(mpfirel[ipfi]);
}

int pon_v_nombre(ipfi,nom,modo)
	int ipfi;
char *nom;
int modo;
{
	return(0);
}
/************/

int minimo_campo_gen(_pfi,campo,tmp) /* pone el valor minimo de un campo */
FI_DEF *_pfi;
int campo;
char *tmp;
{
	int i;
	
	switch(_pfi->f_campos[campo]->c_tipo) {
	case CHARTIPO:
		i = _pfi->f_campos[campo]->c_qc;
		memset(tmp+1,' ',i);
		tmp[i+1]=0;
		tmp[0]='A';
		break;
	case FECHATIPO:
		strcpy(tmp+1,"01.01.0001");
		tmp[0]='@';
		break;
	case INTTIPO:
		sprintf(tmp,_pfi->f_campos[campo]->c_usi,atoi(_pfi->f_campos[campo]->c_minimo));
		break;
	case DOUBLETIPO:
		sprintf(tmp,_pfi->f_campos[campo]->c_usi,rwatof(_pfi->f_campos[campo]->c_minimo));
		break;
	case NULOTIPO:
		tmp[0] = 0;
		break;
	}
}

int maximo_campo_gen(_pfi,campo,tmp) /* pone el valor maximo de un campo */
FI_DEF *_pfi;
int campo;
char *tmp;
{
	int i;
	
	switch(_pfi->f_campos[campo]->c_tipo) {
	case CHARTIPO:
		i = _pfi->f_campos[campo]->c_qc;
		memset(tmp+1,255,i);
		tmp[i+1]=0;
		tmp[0]='A';
		break;
	case FECHATIPO:
		strcpy(tmp+1,"31.12.9999");
		tmp[0]='@';
		break;
	case INTTIPO:
		sprintf(tmp,_pfi->f_campos[campo]->c_usi,atoi(_pfi->f_campos[campo]->c_maximo));
		break;
	case DOUBLETIPO:
		sprintf(tmp,_pfi->f_campos[campo]->c_usi,rwatof(_pfi->f_campos[campo]->c_maximo));
		break;
	case NULOTIPO:
		tmp[0] = 0;
		break;
	}
}

int ini_nombres()
{
	
	fin_nombres();
	if (!(e_nivel%2))
		cont_defs = 0;
	else
		cont_defs = 1;
	return(0);
}


int fin_nombres()
{
	int r;
	
	for (r = 0;r < (MAXDEF*2);r++) 
    {
		if (!nombre_defs[r]) continue;
		free(nombre_defs[r]);
		nombre_defs[r] = NULL;
		ventana_defs[r] = 0;
	}
	
	cont_defs = 0;
	
}


int libera_bloqueo_def(pfi)
FI_DEF *pfi;
{
	if (pfi->f_fd > -1)
	{
		if (pfi->grabable)
		{
			rw_lseek_(pfi->f_fd,1000000000L,SEEK_SET);
			locking(pfi->f_fd,0,1L);
			pfi->grabable = 0;
		}
		rw_close_(pfi->f_fd);
	}
	return(0);
}


lee__def(nom,pfi,i,modo)
char *nom;
FI_DEF *pfi;
int i,modo;
{
	char nombre[256];
	char nomfi[256];
	FI_DEF *tmp;
	char *tmm;
	int r = 0,kk;
	int nosolo;
	short bloqueado = 0;
	int fd;
	
	nosolo = modo / 100;
	modo %= 100;
	
	
	if (nom == quita_path(nom)) {
		if (modo == 2) {
			strcpy(nombre,dirbase);
			strcat(nombre,"vent/");
			strcat(nombre,nom);
		}
		else {
			strcpy(nombre,dirbase);
			strcat(nombre,"def/");
			strcat(nombre,nom);
		}
	}
	else {
		if (*nom == ':') 
        {
			if (*(nom+1) == '/') 
            {
				strcpy(nombre,dirbase_);
				strcat(nombre,nom+2);
			}
			else 
            {
				kk = *(quita_path(nom));
				*(quita_path(nom)) = 0;
				strcpy(nombre,dirbase_);
				strcat(nombre,nom+1);
				*(quita_path(nom)) = kk;
				if (modo == 2)
					strcat(nombre,"vent/");
				else
					strcat(nombre,"def/");
				strcat(nombre,quita_path(nom));
			}
		}
		else
			strcpy(nombre,nom);
	}
	
	/* asegurarse que hay memoria para el fopen */
	/***
	tmm = malloc(2048);
	free(tmm);
	***/
	
		if (nosolo)
		{
			strcpy(nomfi,nombre);
			strcat(nomfi,".mas");
			if ((fd = rw_open_(nomfi,O_BINARY | O_RDWR)) < 0) 
			{
				return(-1);
			}
			rw_lseek_(fd,1000000000L,SEEK_SET);
			if (!locking(fd,2,1L))
			{
				/* se puede grabar */
				bloqueado = 1;
				/**********************/
			}
			r = lee_def((char *)fd,pfi,7700+i);
			if (r < 0 || (r && modo != 2))
			{
				rw_close_(fd);
				return(r);
			}
			else
			{
				if (r == 1)
				{
					rw_lseek_(fd,1000000000L,SEEK_SET);
					locking(fd,0,1L);
					/* se puede grabar */
					bloqueado = 0;
					/**********************/
					r = 0;
				}
				pfi->f_fd = fd;
				strcpy(pfi->f_fichero+40,quita_path(nombre));
			}
		}
		else
			r = lee_def(nombre,pfi,i);
	
	if (!r)
	{
		pfi->grabable = bloqueado;
	}
	return(r);
}


int numero_prog(n) /* da el verdadero numero en tabla_cal (+1) */
int n;
{
	int i,total;
	
	total = 0;
	for (i = 0;i < n;i++) {
		if (progr[i]->tipo != -1)
			total++;
		if (total == 70) /* ESPACIO RESERVADO PARA CONTADORES */
			total = 100;
	}
	
	return(total);
}

int numero_contador(n)
int n;
{
	int i,total;
	
	if (progr[n-1]->contador)
		return(progr[n-1]->contador + 69);
	
	total = 0;
	for (i = 0;i < totalprogr;i++) {
		if (progr[i]->tipo == -1)
			continue;
		if (progr[i]->contador) {
			if (total < progr[i]->contador)
				total = progr[i]->contador;
		}
	}
	total++;
	if (total > 30)
		total = -1;   /* demasiados contadores */
	else {
		progr[n-1]->contador = total;
		total += 69;
	}
	return(total);
}

/* comprueba que el nombre sea legal para una variable-proceso-campo */
int nombre_bueno(n,m,ex)
unsigned char *n,*m,*ex;
{
	char tmp[81];
	int i,j,l = strlen(n);
	
	for (i = 0;i < l;i++) {
		if (!i) { /* no deben empezar con un numero */
			if (n[i] >= '0' && n[i] <= '9') {
				/* a menos que sea una excepcion */
				for(j=0;ex[j];j++) { /* excepciones */
					if (n[i] == ex[j]) break;
				}
				if (!ex[j])
					break;
			}
		}
		if (
			(n[i] >= 'a' && n[i] <= 'z') || /* se adminten minusculas */
			(n[i] >= 'A' && n[i] <= 'Z') || /* se adminten mayusculas */
			(n[i] >= '0' && n[i] <= '9') || /* se adminten numeros */
			n[i] == (unsigned char)'¤' ||                  /*     "       e¤es */
			n[i] == (unsigned char)'¥' ||
			n[i] == (unsigned char)'‡' ||                   /*     "       Ce trencades */
			n[i] == (unsigned char)'€' ||
			n[i] == '_'                     /*     "       underlines */
			)
			/* es bueno */;
			else {
			for(j=0;ex[j];j++) { /* excepciones */
				if (n[i] == ex[j]) break;
			}
			if (!ex[j]) { /* es definitivamente malo */
				i = 0;
				break;
			}
		}
	}
	if (!i) {
		sprintf(tmp,"Caracteres ilegales en %s",m);
		un_error(tmp,n);
		return(0);
	}
	return(1);
}


int fin_aplic()
{
	int r;
	
	for (r = 0;r < nfi;r++) {
		if (mpfi[r]) {
			free_def(mpfi[r],1);
		}
		free((char *)mpfirel[r]);
		mpfi[r] = NULL;
	}
	
	
	nfi = 0;
	return(0);
}


int pon_varia(nombre,valor,arrai)
unsigned char *nombre;
unsigned char *valor;
int arrai;
{
	int numero;
	int in = totalmemos;
	int ll;
	int externo = 0;
	int rep = arrai;
	int yaesta = -1;
	int mem,k;
	double tmpd;
	char _nom[81];
	char _no[81];
	
	strcpy(_no,nombre);
	nombre = _no;
	if (rep < 0) rep = 0;
	
	numero = strlen(nombre);
	
	/* chequeo especial variables */
	for (ll = 0;ll < numero;ll++)
		if (!ll && nombre[ll] == 'I' && strcmp(nombre,"Impresora")) {
			un_error("Nombre de variable ILEGAL",nombre);
			break;
		}
		
		if (nombre[0] == '@' && valor[0] == '@' && strlen(valor) > 10) {
			/* caso especial de fecha auto declarada */
			if (!nombre_bueno(nombre,"VARIABLE","&@."))
				return(0);
			
		}
		else {
			if (!nombre_bueno(nombre,"VARIABLE","&@")) {
				return(0);
			}
		}
		
		if (numero > 30) {
			nombre[30] = '\0';
			un_error("Nombre de variable muy largo recortado",nombre);
		}
		
		if (!numero) {
			un_error("Intento de definir una variable sin nombre","");
			return(0);
		}
		
		if (arrai && !strcmp(nombre,"mpresora"))
			un_error("Nombre de variable puntero ILEGAL",nombre);
		
		if (nombre[0] == '&') {
		/*   if (arrai)
		{
		un_error("Las variables externas deben ser simples",nombre);
		return(0);
		}
			*/
			if (!strcmp(nombre,valor))
			{
				un_error("No puede ser externa una constante",nombre);
				return(0);
			}
			externo = -1;
			nombre++;
			yaesta = busca_ex(nombre);
			if (arrai)
			{
				strcpy(_nom,nombre);
			}
			
			for (;rep >= 0;rep--)
			{
				
				if (arrai)
				{
					if (rep)
						sprintf(nombre,"%s%d",_nom,arrai - rep + 1);
					else
						strcpy(nombre,_nom);
				}
				
				
				for (k = 0;k < totalmemos;k++) {
					if (strcmp(nombre,memos[k]->nombre) == 0)
					{
						un_error("Variable DUPLICADA",nombre);
						break;
					}
				}
				if (nm_ex >= MAX_MEX || nglobal_ex >= MAX_MEX) 
				{
					un_error("EXCESO DE VARIABLES EXTERNAS","");
					return(0);
				}
				else
				{
					if (arrai && yaesta == -1 && busca_ex(nombre) != -1)
					{
						un_error("VARIABLE EXTERNA DE ARRAI DESGLOSADA YA EXISTE","");
					}
					
					yaesta = busca_ex(nombre);
					
					m_ex[nm_ex] = (M_EX *)malloc(sizeof(M_EX));
					m_ex[nm_ex]->nombre = strmalloc(strlen(nombre)+1);
					strcpy(m_ex[nm_ex]->nombre,nombre);
					if (yaesta != -1)
					{ /* coger tipo del global!!! */
						m_ex[nm_ex]->m_tipo = global_ex[yaesta]->m_tipo;
						if (m_ex[nm_ex]->m_tipo == DOUBLETIPO)
						{
							tmpd = global_ex[yaesta]->mv.vald;
							m_ex[nm_ex]->mv.vald = tmpd;
						}
						else
						{
							m_ex[nm_ex]->mv.valc = malloc(strlen(global_ex[yaesta]->mv.valc)+1);
							strcpy(m_ex[nm_ex]->mv.valc,global_ex[yaesta]->mv.valc);
						}
						m_ex[nm_ex]->indice_global = 5000 + memoplin + maxpila*2 + yaesta;
					}
					else
					{
						if (valor[0] == 'A' && (!arrai || rep)) {
							m_ex[nm_ex]->m_tipo = CHARTIPO;
							m_ex[nm_ex]->mv.valc = strmalloc(strlen(valor+1)+1);
							strcpy(m_ex[nm_ex]->mv.valc,valor+1);
						}
						else
							if (valor[0] == '@' && (!arrai || rep)) {
								m_ex[nm_ex]->m_tipo = FECHATIPO;
								ll = strlen(valor+1)+1;if (ll < 11) {ll = 11;strcpy(valor+1,"00.00.0000");}
								m_ex[nm_ex]->mv.valc = strmalloc(ll);
								strcpy(m_ex[nm_ex]->mv.valc,valor+1);
								if (bienfecha(valor+1) < 0) {
									un_error("Valor ilegal para una FECHA",valor+1);
								}
							}
							else {
								m_ex[nm_ex]->m_tipo = DOUBLETIPO;
								if (arrai && !rep)
									m_ex[nm_ex]->mv.vald = 5000 + memoplin + maxpila*2 + nm_ex - arrai;
								else
									m_ex[nm_ex]->mv.vald = rwatof(valor);
							}
							m_ex[nm_ex]->indice_global = 5000 + memoplin + maxpila*2 + nglobal_ex;
							
							global_ex[nglobal_ex] = (M_EX *)malloc(sizeof(M_EX));
							global_ex[nglobal_ex]->nombre = strmalloc(strlen(nombre)+1);
							strcpy(global_ex[nglobal_ex]->nombre,nombre);
							global_ex[nglobal_ex]->m_tipo = m_ex[nm_ex]->m_tipo;
							if (m_ex[nm_ex]->m_tipo == DOUBLETIPO)
								global_ex[nglobal_ex]->mv.vald = rwatof(valor);
							else
							{
								global_ex[nglobal_ex]->mv.valc = malloc(strlen(m_ex[nm_ex]->mv.valc)+1);
								strcpy(global_ex[nglobal_ex]->mv.valc,m_ex[nm_ex]->mv.valc);
							}
							global_ex[nglobal_ex]->indice_global = 0;
							nglobal_ex++;
					}
					
					memos[totalmemos] = (MM *)malloc(sizeof(MM));
					if (arrai && !rep)
						memos[totalmemos]->numero = -(5000 + memoplin + maxpila*2 + nm_ex + 1);
					else
						memos[totalmemos]->numero = 5000 + memoplin + maxpila*2 + nm_ex;
					
					memos[totalmemos]->nombre = strmalloc(strlen(nombre)+1);
					strcpy(memos[totalmemos]->nombre,nombre);
					totalmemos++;
					nm_ex++;
					if (arrai && !rep)
					{
						memos[totalmemos] = (MM *)malloc(sizeof(MM));
						memos[totalmemos]->nombre = strmalloc(strlen(_nom)+2);
						sprintf(memos[totalmemos]->nombre,"I%s",_nom);
						memos[totalmemos]->numero = 5000 + memoplin + maxpila*2 + nm_ex - 1;
						totalmemos++;
					}
				}
    }
	return(in);
   }
   
   for (k = 0;k < totalmemos;k++) {
	   if (strcmp(nombre,memos[k]->nombre) == 0) {
		   un_error("Variable DUPLICADA",nombre);
		   break;
       }
   }
   
   for (;rep >= 0;rep--) {
	   
	   if (totalmemos >= MAX_MEMOS || nmemo >= MAX_MEMOS) {
		   un_error("EXCESO DE VARIABLES","");
		   in--;
		   break;
	   }
	   
	   
	   memos[totalmemos] = (MM *)malloc(sizeof(MM));
	   
	   memo[nmemo] = (T_MEMORIA *)malloc(sizeof(T_MEMORIA));
	   if ((!arrai || rep) && valor[0] == 'A') {
		   memo[nmemo]->m_tipo = CHARTIPO;
		   memo[nmemo]->mv.valc = strmalloc(strlen(valor+1)+1);
		   strcpy(memo[nmemo]->mv.valc,valor+1);
	   }
	   else
		   if ((!arrai || rep) && valor[0] == '@') {
			   memo[nmemo]->m_tipo = FECHATIPO;
			   ll = strlen(valor+1)+1;if (ll < 11) {ll = 11;strcpy(valor+1,"00.00.0000");}
			   memo[nmemo]->mv.valc = strmalloc(ll);
			   strcpy(memo[nmemo]->mv.valc,valor+1);
			   if (bienfecha(valor+1) < 0) {
				   un_error("Valor ilegal para una FECHA",valor+1);
			   }
		   }
		   else {
			   memo[nmemo]->m_tipo = DOUBLETIPO;
			   memo[nmemo]->mv.vald = rwatof(valor);
		   }
		   
		   mem = nmemo;
		   nmemo++;
		   
		   if (arrai && !rep) {
			   memos[totalmemos]->numero = (-(mem+1));
			   memo[mem]->mv.vald = (double)(mem - arrai);
		   }
		   else
			   memos[totalmemos]->numero = mem;
		   if (arrai && rep) {
			   memos[totalmemos]->nombre = strmalloc(strlen(nombre)+6);
			   sprintf(memos[totalmemos]->nombre,"%s%d",nombre,arrai - rep + 1);
		   }
		   else {
			   memos[totalmemos]->nombre = strmalloc(strlen(nombre)+1);
			   strcpy(memos[totalmemos]->nombre,nombre);
		   }
		   
		   if (arrai && !rep) {
			   totalmemos++;
			   memos[totalmemos] = (MM *)malloc(sizeof(MM));
			   memos[totalmemos]->nombre = strmalloc(strlen(nombre)+2);
			   sprintf(memos[totalmemos]->nombre,"I%s",nombre);
			   memos[totalmemos]->numero = mem;
		   }
		   totalmemos++;
		   
   }
   
   return(in);
}

int pon_fichero(fichero,numero,todo,_ini_par)
char *fichero;
int numero;
int todo;
int (*_ini_par)();
{
	int ff,j,i,irel,menor,fli,kk,fex = 0;
	char tmp[81];
	char path[81];
	char ficher[81];
	FI_DEF *ifi;
    int lll;
	
	if (todo > 1 || todo < 0) todo = 1;
	
	if (totalfichs >= 150 || nfi >= MAXDEF) {
		un_error("EXCESO DE FICHEROS","");
		return(-1);
	}
	
	i = strlen(fichero)  - 1;
	if (i > 0 && fichero[i] == '@')
	{
		/* referncia a fichero */
		fichero[i] = 0;
		j = 0;
		if (fichero[0] == '&') j++;
		if (!nombre_bueno(fichero+j,"DECLARACION DE REFERENCIA A FICHERO",fichero))
		{
			fichero[i] = '@';
			return(-1);
		}
		fichero[i] = '@';
		/* poner una variable externa especifica para la referencia */
		/*
		if (fichero[0] == '&')
		{
		ff = busca_ex(fichero+1);
		if (ff == -1)
		{
		
		  ff = pon_varia(fichero,"-1",0);
		  if (ff >= 0)
		  ff = memos[ff]->numero;
		  }
		  else
		  {
		  ff += (5000 + memoplin + maxpila*2);
		  }
		  }
		  else
		  {
		*/
        if (!strcmp(fichero,"Fichero@"))
        {
            i = -1;
            ff = 5011;
        }
        else
		for (i = 0;i < totalmemos;i++) 
		{
			if (strcmp(fichero+j,memos[i]->nombre) == 0)
			{
				ff = memos[i]->numero;
				break;
			}
		}
		if (i >= totalmemos)
		{
			ff = pon_varia(fichero,"-1",0);
			if (ff >= 0)
				ff = memos[ff]->numero;
		}
		/*} */
		if (ff < 0)
		{
			return(-1);
		}
		ff += 1000; /* mil defs ... */   
		fichero += j;
		goto salto_a_fichero;
	}
	
	if (!nombre_bueno(quita_path(fichero),"DECLARACION DE FICHERO",""))
		return(-1);
	
	en_ini_par++; /* anula efecto de localidad en compilacion */
	ff = ind_rel_x(fichero);
	if (ff < 0) {
		if (minimun && nfi >= 4)
		{
			aviso(0,"EXCESO DE FICHEROS");
			en_ini_par--; /* anula efecto de localidad en compilacion */
			return(0);
		}
		
		if (fichero == quita_path(fichero)) 
        {
			if (notacion_dir_aplicacion != quita_path(notacion_dir_aplicacion) && notacion_dir_aplicacion[0] == ':' && notacion_dir_aplicacion[1] != '/') 
            {
				kk = *(quita_path(notacion_dir_aplicacion));
				*(quita_path(notacion_dir_aplicacion)) = 0;
				strcatd(fichero,notacion_dir_aplicacion);
				*(quita_path(notacion_dir_aplicacion)) = kk;
			}
		}
		ifi = (FI_DEF *)malloc(sizeof(FI_DEF));	  
		memset(ifi,0,sizeof(FI_DEF));
		ventana_defs[nfi] = 1;
		if (lee__def(fichero,ifi,1,2) < 0) {
			ventana_defs[nfi] = 0;
			if (lee__def(fichero,ifi,1,1) < 0) {
				free((char *)ifi);
				sprintf(tmp,"Compilando: Fichero '%s' inaccesible o inexistente"
					,fichero);
				aviso(0,tmp);
				en_ini_par--; /* anula efecto de localidad en compilacion */
				return(0);
			}
			else {
				pon_v_nombre(nfi,fichero,1);
			}
		}
		else {
			pon_v_nombre(nfi,fichero,2);
		}
		
		mpfi[nfi] = ifi;
		comprueba_def(fichero);
		
		mpfirel[nfi] = (RELAS *)malloc(sizeof(RELAS));
		mflagfi[nfi] = RCERRADO;
		mpfirel[nfi]->lineas = -1;
		mpfirel[nfi]->clave = -1;
		mpfirel[nfi]->nrel    = 0;
		mpfirel[nfi]->nrelact = 0;
		mpfirel[nfi]->nreldef = 0;
		mpfirel[nfi]->nrelpin = 0;
		ff =  nfi;
		fini_par[nfi] = -1;
		irel = nfi;
		nfi++;
		if (nfi == 48) nfi++;
		if (nombre_defs[irel] != quita_path(nombre_defs[irel]) && nombre_defs[irel][0] == ':' && nombre_defs[irel][1] != '/') {
			kk = *(quita_path(nombre_defs[irel]));
			*(quita_path(nombre_defs[irel])) = 0;
			strcpy(path,nombre_defs[irel]);
			*(quita_path(nombre_defs[irel])) = kk;
		}
		else
			path[0] = 0;
		if (todo) {
			j = primero;
			primero = nfi - 1;     
			if (primero == 48) primero--;
			
			/*ini_par();*/
            if (_ini_par)
            {
                (*_ini_par)();
            }
			
			primero = j;
		}
		else {  /* SOLO LINEAS */
			libera_extras(vg_mpfi(irel));
			for (i=0;i<vg_mpfi(irel)->f_nrelaci;i++) {
				menor = 0;
				for (j=0;j<(MAXCAMPOSPORCLAVE+1);j++) {
					if (vg_mpfi(irel)->f_relaci[i]->r_campo[j] == 0) continue;
					if (menor > vg_mpfi(irel)->f_relaci[i]->r_campo[j])
						menor = vg_mpfi(irel)->f_relaci[i]->r_campo[j];
				}
				if (menor < 0) {
					mpfirel[irel]->lineas  = 0;
					strcpy(ficher,vg_mpfi(irel)->f_relaci[i]->r_fichero);
					fli = ind_def_rel(ficher);
					if (fli < 0) {
						if (ficher == quita_path(ficher))
							strcatd(ficher,path);
						ifi = (FI_DEF *)malloc(sizeof(FI_DEF));
						memset(ifi,0,sizeof(FI_DEF));
						ventana_defs[nfi] = 1;
						if (lee__def(ficher,ifi,1,2) < 0) {
							ventana_defs[nfi] = 0;
							if (lee__def(ficher,ifi,1,1) < 0) {
								free((char *)ifi);
								continue;
							}
							else {
								pon_v_nombre(nfi,ficher,1);
							}
						}
						else {
							pon_v_nombre(nfi,ficher,2);
						}
						fli = nfi;
						mpfi[nfi] = ifi;
					}
					mpfirel[irel]->camrela[mpfirel[irel]->nrel] = -1;
					mpfirel[irel]->numrela[mpfirel[irel]->nrel] = i;
					mpfirel[irel]->camminu[mpfirel[irel]->nrel] = (menor - 1);
					mpfirel[irel]->relacis[mpfirel[irel]->nrel] = fli;
					mpfirel[irel]->nrel++;;
					
					if (fli == nfi) {
						comprueba_def(ficher);
						mpfirel[fli] = (RELAS *)malloc(sizeof(RELAS));
						mflagfi[fli] = RCERRADO;
						mpfirel[fli]->nrel    = 0;
						mpfirel[fli]->nrelact = 0;
						mpfirel[fli]->nreldef = 0;
						mpfirel[fli]->nrelpin = 0;
						mpfirel[fli]->clave   = -1;
						fini_par[fli] = -1;
						libera_extras(vg_mpfi(fli));
						nfi++;
						if (nfi == 48) nfi++;
					}
					mpfirel[fli]->lineas  = (menor - 1)*160 - 22*6 - 5;
					break;
				}
			}
		}
  }
  
  en_ini_par--; 
  
  /*
  if (todo && fini_par[ff] == -1) {
	  un_aviso("DEF DECLARADO MANTE YA CARGADO SIN MANTE",fichero);
  }
  */
  
  
salto_a_fichero:
  
  for (i = 0;i < nindice_def_local;i++)
  {
	  if (indice_def_local[i] == ff)
		  break;
  }
  if (i >= nindice_def_local)
  {
	  if (nindice_def_local >= 48)
      {
		  aviso(0,"EXCESO DE FICHEROS EN UN PRC (Limite 48)");       
      }
	  else
      {
		  indice_def_local[nindice_def_local] = ff;
		  i = nindice_def_local; /* ! */
		  nindice_def_local++;
      }
  }
  
  
  if (strlen(fichero) == 0) return(0);
  fichs[totalfichs] = (FF *)malloc(sizeof(FF));
  if (*fichero == ':')
  {
      lll = strlen(fichero);
  }
  else
  {
      lll = strlen(quita_path(fichero));
  }
  fichs[totalfichs]->fichero = strmalloc(lll+1);
  if (*fichero == ':')
  {
      strcpy(fichs[totalfichs]->fichero,fichero);
  }
  else
  {
      strcpy(fichs[totalfichs]->fichero,quita_path(fichero));
  }
  fichs[totalfichs]->numero = numero;
  fichs[totalfichs]->indice = i /*ff */;
  if (ff >= 1000)
  {
	  todo += (ff * 2);
  }
  fichs[totalfichs]->fini = todo;
  totalfichs++;
  return(0);
}


int pon_varsistem()
{
	
	memoplin = 13;
#ifdef COMENTARIO
	pon_varia("Campo","0",0);
	pon_varia("Contenido","A",0);
	pon_varia("Control","A",0);
	pon_varia("FEntrada","A",0);
	pon_varia("FSalida","A",0);
	pon_varia("E_sup","A10",0);
	pon_varia("E_inf","A",0);
	pon_varia("Pos","-1",0);
	pon_varia("Usuario","A",0);
	pon_varia("Impresora","A",0);
	pon_varia("PARAMETRO","A",0);
	
	/* Pila para operaciones complejas numericas */
	while(nmemo < (memoplin + maxpila)) {
		memo[nmemo] = (T_MEMORIA *)malloc(sizeof(T_MEMORIA));
		memo[nmemo]->m_tipo = DOUBLETIPO;
		memo[nmemo]->mv.vald = 0.0;
		nmemo++;
	}
	/* Pila para operaciones complejas alfanumericas */
	while(nmemo < (memoplin + maxpila*2)) {
		memo[nmemo] = (T_MEMORIA *)malloc(sizeof(T_MEMORIA));
		memo[nmemo]->m_tipo = CHARTIPO;
		memo[nmemo]->mv.valc = strmalloc(1);
		memo[nmemo]->mv.valc[0] = '\0';
		nmemo++;
	}
#endif
	
}

int que_memo_tipo(i)
int i;
{
	if (i < 5000)
	{
		return(memo[i]->m_tipo);
	}
	
	if (i >= (5000 + memoplin + maxpila*2))
	{
		i -= (5000 + memoplin + maxpila*2);
		i = busca_ex(m_ex[ i ]->nombre);
		if (i == -1)
		{
			un_error("ERROR INTERNO DE GLOBALIZACION DE EXTERNAS 1","");
			return(DOUBLETIPO);
		}
		return(global_ex[ i ]->m_tipo);
	}
	
	i-=5000;
	
	if (i >= memoplin)
	{
		i -= memoplin;
		if (i < maxpila)
			return(DOUBLETIPO);
		return(CHARTIPO);
	}
	
	switch(i)
	{
	case 0:
	case 7:
    case 11:
		return(DOUBLETIPO);
	default:
		return(CHARTIPO);
	}
}

fin_prog()
{
	short i;

    
    if (nen_pr)
    {
      memset(en_pr,0,sizeof(struct enlaza_prc)*nen_pr);
    }
    
	nen_pr = 0;
	
	for (i=0;i<nglobal_ex;i++) {
		free(global_ex[i]->nombre);
		free((char *)global_ex[i]);
	}
	nglobal_ex = 0;
	
	for (i = 0;i < totalprogr;i++) {
		free(progr[i]->nombre);
		free((char *)progr[i]);
	}
	totalprogr = 0;
}




/* sustituciones */

int rw_error(int e)
{
char tmp[256];
sprintf(tmp,"Error interno %d",e);
return(un_error(tmp,""));
}


int v_aviso(int p,char *m)
{
return(un_aviso(m,"!"));
}


int  busca_ex(nom)
char *nom;
{
	int i;
	
	for (i = 0;i < nglobal_ex;i++) {
		if (strcmp(nom,global_ex[i]->nombre) == 0)
			return(i);
	}
	return(-1);
}
