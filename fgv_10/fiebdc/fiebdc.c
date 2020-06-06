#if defined(FGVDOS) || defined(FGVUNIX) || defined(FGVBSD)
#include <fgvsys.h>
#endif

#ifndef YA_FGVSYS

#define rw_open_        open
#define rw_close_       close
#ifdef LARGESEEK
#define rw_lseek_(a,b,c) ((long)lseek(a,((off_t)b),c))
#else
#define rw_lseek_       lseek
#endif
#define rw_read_        read
#define rw_write_       write

#endif

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifndef O_BINARY  
#define O_BINARY 0
#define SEEK_SET 0
#define SEEK_CUR 1
#ifndef LARGESEEK
extern long rw_lseek_();
#endif
extern unsigned int rw_read_();
#endif

#include <time.h>
#include <sys/timeb.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <errno.h>
#include <math.h>


#define BC3MAXDESGLOSE 512
#define MAXBC3VAR 26
/*****************************************************************************/

int resuelve_parametrico_bc3(char *pcodigo,char *buffer,char **_texto,char **_resumen,char **_desglose,double *_cantidad,double *_precios,double *_precio,char **_error);
int fin_bc3();
int carga_el_bc3(char *path);
int lista_de_parametros_bc3(char *pcodigo,char **parametros[]);
int pon_provincia_bc3(int provincia);

/*****************************************************************************/

static void __aviso(int pos,char *texto)
{
#ifdef YA_FGVSYS
	extern int aviso();
	aviso(pos,texto);
#else
	printf("%s\n",texto);
#endif
}

static double redondea(num,dec)
double num;
int dec;
{
   double a = 1.0;
   double d;
   
   while (dec-- > 0) a *= 10.0;    /* 10 elevado a dec */
   num *= a;
   num += .5;
   d = floor(num);       /* para esta funcion necesitamos la libreria matem.*/
   return(d / a);
}

static void quitab(s)
char *s;
{
   register char *r;
   
   for (r=s;(*r);r++);
   while (r > s) {
      r--;
      if (*r != ' ' && *r != 9) 
      {
         *(r+1) = 0;
         break;
      }
      if (r == s)
         *r = 0;
   }
   
}

static void quitabp(s)
char *s;
{
   register char *c;
   
   for (c=s;(*c == ' ' || *c == 9);c++);
   while ((*s++=*c++) != '\0');
}

static void quitan(s)
char *s;
{
 		 register char *r;
         
         for (r=s;(*r);r++);
         while (r > s) {
            r--;
            if (*r != '\n') {
               *(r+1) = 0;
               break;
            }
            if (r == s)
               *r = 0;
         }      
         
         for (r=s;(*r);r++);
         while (r > s) {
            r--;
            if (*r != '\r') {
               *(r+1) = 0;
               break;
            }
            if (r == s)
               *r = 0;
         }     
         
/*
         for (r=s;(*r);r++);
         while (r > s) {
            r--;
            if (*r != '\n') {
               *(r+1) = 0;
               break;
            }
            if (r == s)
               *r = 0;
         }
*/       
}

static int provincia_bc3 = 0;
static char *_todo_el_bc3 = NULL;
static char **_lista_parametricos_bc3 = NULL;
static int _nlista_parametricos_bc3;
static char **_lista_articulos_bc3 = NULL;
static int _nlista_articulos_bc3;
static char *_Sustituciones_bc3[] =
{
	"RESUMEN",
	"R",
	"TEXTO",
	"T",
	"PLIEGO",
	"P",
	"CLAVES",
	"K",
	"COMERCIAL",
	"F",
	NULL
};


static double evalua_expresion_num_bc3(char *ptmp);

typedef struct _variable_tag
	{		    
		int ndimalfa;
		int *dimalfa;
		int localmemo;
		char *alfa;
		int ndimnum;
		int *dimnum;
		double *num;
	} _mi_variable;
static _mi_variable _variable[MAXBC3VAR];	


int fin_bc3()
{
	if (_lista_parametricos_bc3)
	{
		free((char *)_lista_parametricos_bc3);
		_lista_parametricos_bc3 = NULL;
	}
	if (_lista_articulos_bc3)
	{
		free((char *)_lista_articulos_bc3);
		_lista_articulos_bc3 = NULL;
	}
	if (_todo_el_bc3)
	{
		free(_todo_el_bc3);
		_todo_el_bc3 = NULL;
		return 0;
	}
	return -1;
}


static char *busca_en_lista_(char **lista,int nlista,char *abuscar,int l)
{	
	int medio = nlista / 2;
	int r1;
	
	if (!(r1 = memcmp(abuscar,lista[0],l)))
		return lista[0];
	if (r1 < 0)
	{
		return NULL;
	}
	if (!(r1 = memcmp(abuscar,lista[nlista-1],l)))
		return lista[nlista-1];
	if (r1 > 0)
	{
		return NULL;
	}
	if (!(r1 = memcmp(abuscar,lista[medio],l)))
		return lista[medio];

	if (r1 < 0)
	{
		return busca_en_lista_(lista+1,medio-1,abuscar,l);
	}
	return busca_en_lista_(lista+1+medio,nlista-medio-2,abuscar,l);
}

int carga_el_bc3(char *path)
{
	int f;
	long pos;
	fin_bc3();
	f = rw_open_(path,O_RDONLY | O_BINARY);
	if (f < 0) return -1;
    pos = rw_lseek_(f,0L,SEEK_END);
	_todo_el_bc3 = malloc(pos+1);    
	rw_lseek_(f,0L,SEEK_SET);
	if (rw_read_(f,_todo_el_bc3,pos) != pos)
	{
		rw_close_(f);
		fin_bc3();
		return -2;
	}
	_todo_el_bc3[pos] = 0;
	rw_close_(f);    


	{
		char *p1 = _todo_el_bc3;
		char *p2;
		char tmp[256];
		int ntotal = 0;
		strcpy(tmp,"~P|");
		do
		{
			p2 = strstr(p1,tmp);
			if (p2)
			{
				ntotal++;
				p1 = p2+3;
			}
			else
				break;
		} while(-1);		

		_nlista_parametricos_bc3 = ntotal;
		_lista_parametricos_bc3 = (char **)malloc(sizeof(char *)*ntotal);
		memset(_lista_parametricos_bc3,0,sizeof(char *)*ntotal);

		ntotal = 0;
		p1 = _todo_el_bc3;
		do
		{
			p2 = strstr(p1,tmp);
			if (p2)
			{
				_lista_parametricos_bc3[ntotal] = p2;
				ntotal++;
				p1 = p2+3;
			}
			else
				break;
		} while(-1);
	}

	{
		char *p1 = _todo_el_bc3;
		char *p2;
		char tmp[256];
		int ntotal = 0;
		strcpy(tmp,"~C|");
		do
		{
			p2 = strstr(p1,tmp);
			if (p2)
			{
				ntotal++;
				p1 = p2+3;
			}
			else
				break;
		} while(-1);		

		_nlista_articulos_bc3 = ntotal;
		_lista_articulos_bc3 = (char **)malloc(sizeof(char *)*ntotal);
		memset(_lista_articulos_bc3,0,sizeof(char *)*ntotal);
		ntotal = 0;
		p1 = _todo_el_bc3;
		do
		{
			p2 = strstr(p1,tmp);
			if (p2)
			{
				_lista_articulos_bc3[ntotal] = p2;
				ntotal++;
				p1 = p2+3;
			}
			else
				break;
		} while(-1);
	}

	return 0;
}


static int busca_en_articulo_bc3(char *codigo,char **_buffer,double *precio)
{
	char tmp[256];
	char *p;
	int i;

	if (!_todo_el_bc3) return -1;

	sprintf(tmp,"~C|%s|",codigo);

	if ((p = busca_en_lista_(_lista_articulos_bc3,_nlista_articulos_bc3,tmp,strlen(tmp))))
	{
		if (precio)
		{
			char *p1 = p + 1;
			int pipes = 0;
			while (*p1 && *p1 != '~')
			{
				if (*p1 == '|')
					pipes++;
				if (pipes == 4)
				{
					p1++;
					*precio = rwatof(p1);
					for (i = 0;i < provincia_bc3;i++)
					{
						while (*p1 && *p1 != '|' && *p1 != '~' && *p1 != '\\') p1++;
						if (*p1 != '\\') break;
						p1++;
						*precio = rwatof(p1);
					}
					break;
				}
				else
				    p1++;
			}
		}
		if (_buffer)
		{
			char *p1 = p+1;
			while(*p1 && *p1 != '~') p1++;
			*_buffer = malloc(1+(int)(p1-p));
			memcpy(*_buffer,p,(int)(p1-p));
			(*_buffer)[(int)(p1-p)] = 0;
		}

		return 0;
	}
	return -1;
}

static int busca_en_parametrico_bc3(char *codigo,char **_buffer)
{
	char tmp[256];
	char *p;

	if (!_todo_el_bc3) return -1;

	sprintf(tmp,"~P|%s",codigo);
	tmp[9] = '$';
	tmp[10] = '|';
	tmp[11] = 0;
	if ((p = busca_en_lista_(_lista_parametricos_bc3,_nlista_parametricos_bc3,tmp,strlen(tmp))))
	{
		char *p1 = p+1;
		while(*p1 && *p1 != '~') p1++;
		*_buffer = malloc(1+(int)(p1-p));
		memcpy(*_buffer,p,(int)(p1-p));
		(*_buffer)[(int)(p1-p)] = 0;
		return 0;
	}
	return -1;
}


static char **linealiza_parametrico_bc3(char *buffer)
{
	char **lineas;
	char *ptmp1;
	int i;	

	/* 'enlinear' */
	ptmp1 = buffer;
	i = 2;
	while(*ptmp1)
	{
		if (*ptmp1 == 10)
			i++;
		ptmp1++;
	}
	lineas = (char **)malloc(sizeof(char *)*i);
	i = 0;
	lineas[i] = buffer;
	ptmp1 = buffer;
	while(*ptmp1)
	{
		if (*ptmp1 == 10)
		{
			i++;
			lineas[i] = ptmp1 + 1;
			*ptmp1 = 0;
		}
		ptmp1++;
	}
	lineas[i+1] = NULL;

	return lineas;    
}

static int limpia_parametrico_bc3(char **lineas)
{
	int i;
	char *ptmp1,*ptmp2;

    /* limpiar la descripcion parametrica */
	for (i = 0;lineas[i];i++)
	{
		if (!lineas[i][0]) continue;

		quitan(lineas[i]);
		quitabp(lineas[i]);
		ptmp1 = lineas[i];
		if (*ptmp1 != '\\')
		{
			while(*ptmp1 && *ptmp1 != '#') ptmp1++;
			*ptmp1 = 0;
		}
		quitab(lineas[i]);
		ptmp1 = lineas[i];
		while(*ptmp1)
		{
			if (*ptmp1 == 9) *ptmp1 = ' ';
			ptmp1++;
		}
		ptmp1 = lineas[i];
		while(*ptmp1)
		{
			if (*ptmp1 == '\\')
			{
				ptmp2 = ptmp1-1;
				while(ptmp2 >= lineas[i] && *ptmp2 == ' ') ptmp2--;
				ptmp2++;
				if (ptmp2 < ptmp1)
				{
					*ptmp1 = ' ';
					ptmp1 = ptmp2;
					*ptmp1 =  '\\';
				}				
				quitabp(ptmp1+1);
			}	
			ptmp1++;
		}
		if (!lineas[i][0])
		{
			lineas[i] = "";
		}
	}

	{
		int juntar;
		int siguiente;
		int dentro1;
		int dentro2;
		int j;
		for (i = 0;lineas[i];i++)
		{
			if (!lineas[i][0])
			{
				lineas[i] = "";
				continue;
			}
			do
			{
				juntar = 0;
				siguiente = 1;
				while(lineas[i+siguiente] && !lineas[i+siguiente][0]) siguiente++;
				ptmp1 = lineas[i];		
				if (*ptmp1 && lineas[i+siguiente])
				{
					ptmp1 = lineas[i];
					dentro1 = 0;
					while(*ptmp1)
					{
						if (*ptmp1 == '"')
						{
							dentro1 = !dentro1;
						}
						ptmp1++;
					}
					ptmp1 = lineas[i];
					if (dentro1)
					{
						juntar = 1;
					}
					else
					{
						j = strlen(ptmp1)-1;			
						if (ptmp1[0] == '\\')
						{
							if (ptmp1[j] != '\\' && ptmp1[j] != '|')
							{
								juntar = 1;
							}
						}
						else
						{
							if (ptmp1[j] == ',' || ptmp1[j] == '+' || ptmp1[j] == '-' || ptmp1[j] == '*' ||
								ptmp1[j] == '/' || ptmp1[j] == '^' || ptmp1[j] == '@' || ptmp1[j] == '&' ||
								ptmp1[j] == '<' || ptmp1[j] == '>' || ptmp1[j] == '=' || ptmp1[j] == '!')
							{
								juntar = 1;
							}
						}
					}
					if (juntar)
					{
						ptmp2 = ptmp1+j+1;
						while (ptmp2 < lineas[i+siguiente])
						{
							*ptmp2 = ' ';
							ptmp2++;
						}
						quitabp(ptmp1+j+1);
						/*ptmp2 = lineas[i+siguiente];
						while (*ptmp2) ptmp2++;
						lineas[i+siguiente] = ptmp2;*/
						lineas[i+siguiente] = "";
					}
				}
			} while(juntar);

			ptmp1 = lineas[i];
			if (*ptmp1 != '\\')
			{
				dentro1 = 0;
				dentro2 = 0;
				while(*ptmp1)
				{
					if ((dentro1 || !dentro2) && *ptmp1 == ' ')
					{
						quitabp(ptmp1);
						if (!*ptmp1) break;
					}
					if (*ptmp1 == '\\')
					{
						dentro1 = !dentro1;
					}
					if (*ptmp1 == '"')
					{
						dentro2 = !dentro2;
					}
					ptmp1++;
				}
			}
		}
	}

	{
		int j;
		for (i = 0;lineas[i];i++)
		{
			if (!lineas[i][0])
			{
				for (j = i;lineas[j];j++)
				{
					lineas[j] = lineas[j+1];
				}
			}
		}
	}
	return 0;
}

static int saca_un_parametro_bc3(char **_ptmp1,char **tmp)
{
	char *ptmp1 = *_ptmp1;
	char *ptmp2 = ptmp1+1;	

	while(*ptmp2 && *ptmp2 != '\\') ptmp2++;
	if (*ptmp2 != '\\') return 0;
	if (!*tmp)
	{
		*tmp = malloc((int)(ptmp2-ptmp1)+1);
	}
	memcpy(*tmp,ptmp1+1,(int)(ptmp2-ptmp1)-1);
	(*tmp)[(int)(ptmp2-ptmp1-1)] = 0;
	*_ptmp1 = ptmp2;
	return 1;
}

static int _cuenta_parametros(char *ptmp1)
{
	int numero = 0;	
	if (*ptmp1 != '\\') return 0;
	ptmp1++;
	do
	{		
		while(*ptmp1 && *ptmp1 != '\\') ptmp1++;
		if (*ptmp1 == '\\')
		{
			numero++;
			ptmp1++;
		}
		else
			break;
	} while(-1);
	return numero;
}

static int free_varalfa(int i)
{
	char **px;
	if (_variable[i].dimalfa)
	{
		free((void *)_variable[i].dimalfa);
		_variable[i].dimalfa = NULL;
	}
	if (_variable[i].localmemo)
	{				
		if (_variable[i].alfa)
		{
			if (_variable[i].ndimalfa)
			{
				px = (char **)_variable[i].alfa;
				while(*px)
				{
					free(*px);
					px++;
				}
			}
			free(_variable[i].alfa);
			_variable[i].alfa = NULL;
		}
	}
	return 0;
}


static int free_varnum(int i)
{
	if (_variable[i].dimnum)
	{
		free((void *)_variable[i].dimnum);
		_variable[i].dimnum = NULL;
	}
	if (_variable[i].num)
	{
		free((void *)_variable[i].num);
		_variable[i].num = NULL;
	}
	return 0;
}


static double *rellena_numarrai_bc3(int ndim,int *pdim,char *ptmp)
{
    double *r;
	int total = 1;	
	int i;	
	char *restorecoma = NULL;

	for (i = 0;i < ndim;i++)
	{
		total *= pdim[i];
	}
	r = (double *)malloc(sizeof(double)*total);
	memset((void *)r,0,sizeof(double)*total);

    i = 0;	
	while (i < total && *ptmp)
	{		
		if (*ptmp >= 'a' && *ptmp <= 'z')
		{
			r[i] = (double)((*ptmp-'a')+1);
		}
		else
		{
			r[i] = rwatof(ptmp);
		}
		if (restorecoma)
		{
			*restorecoma = ',';
			restorecoma = NULL;
		}
		while(*ptmp && *ptmp != ',') ptmp++;
		if (*ptmp == ',')
		{
			*ptmp = 0;
			restorecoma = ptmp;
			i++;
			ptmp++;
		}
	}
	if (restorecoma)
	{
		*restorecoma = ',';
	}
	return r;
}

static char *rellena_alfaarrai_bc3(int ndim,int *pdim,char *ptmp)
{
    char **r;
	int total = 1;
	int i;	
	int dentro;
	char *ptmp1;

	for (i = 0;i < ndim;i++)
	{
		total *= pdim[i];
	}	
	r = (char **)malloc(sizeof(char *)*(total+1));
	memset((void *)r,0,sizeof(char *)*(total+1));

    i = 0;	
	dentro = 0;
	ptmp1 = ptmp;
	while (i < total && *ptmp)
	{
		if (!dentro)
		{
			while(*ptmp && *ptmp != '"') ptmp++;
			if (!*ptmp)
				break;
			dentro = 1;
		}
		if (dentro)
		{
			if (*ptmp == '"')
			   ptmp++;
			ptmp1 = ptmp;
			while(*ptmp && *ptmp != '"') ptmp++;
			dentro = 0;
			r[i] = malloc((int)(ptmp-ptmp1)+1);
			memcpy(r[i],ptmp1,(int)(ptmp-ptmp1));
			r[i][(int)(ptmp-ptmp1)] = 0;
		}		
		while(*ptmp && *ptmp != ',') ptmp++;
		if (*ptmp == ',')
		{
			i++;
			ptmp++;
		}
	}
	return (char *)r;
}


static char *saca_valor_alfa_bc3(int nv,int ndim,int *pdim)
{
	int i,j;
	int offset;
	int idx = 0;
	char **p;
	int total;

	if (_variable[nv].ndimalfa != ndim)
	{
		/* assert*/
		return NULL;
	}
	if (!ndim)
	{
		return _variable[nv].alfa;
	}

	if (!_variable[nv].dimalfa)
	{
		/* assert */
		return NULL;
	}

	idx = 0;
	total = 1;
	for (i = 0;i < ndim;i++)
	{
		offset = 1;
		total *= _variable[nv].dimalfa[i];
		for (j = i+1;j < ndim;j++)
		{
			offset *= _variable[nv].dimalfa[j];
		}
		idx += (offset * pdim[i]);
	}
	p = (char **)_variable[nv].alfa;

	if (idx >= total)
	{
		return "";
	}
	return p[idx];
}

static char saca_valor_varparametro_bc3(int nv)
{
	if (_variable[nv].ndimnum && !_variable[nv].dimnum)
	{
		return 'a'+(_variable[nv].ndimnum-1);
	}
	return 0;
}

static double saca_valor_num_bc3(int nv,int ndim,int *pdim)
{
	int i,j;
	int offset,total;
	int idx = 0;

	if (!ndim && _variable[nv].ndimnum && !_variable[nv].dimnum)
	{
		return (double)(_variable[nv].ndimnum);
	}

	if (_variable[nv].ndimnum != ndim)
	{
		/* assert*/
		return 0.0;
	}

	if (!_variable[nv].num)
	{
		/* assert*/
		return 0.0;
	}

	if (!ndim)
	{
		return *(_variable[nv].num);
	}

	idx = 0;
	total = 1;
	for (i = 0;i < ndim;i++)
	{
		total *= _variable[nv].dimnum[i];
		offset = 1;
		for (j = i+1;j < ndim;j++)
		{
			offset *= _variable[nv].dimnum[j];
		}
		idx += (offset * pdim[i]);
	}
	if (idx >= total)
	{
		return 0.0;
	}
	return _variable[nv].num[idx];
}


static int que_variable_es_bc3(char **ptmp,int *_nv,int *_ndim,int **_pdim)
{
	char *ptmp1 = *ptmp;
	char *ptmp2;
	int tipo = 0;
	int nv = 0;
	int ndim = 0;
	int *pdim = NULL;
	int elfinal;
	char csal = ')';
	
	if (*ptmp1 == '%')
	{
		tipo = 1;
	}
	else if (*ptmp1 == '$')
	{
		tipo = 2;
	}
	else
		return -1;

	ptmp1++;
	
	if (*ptmp1 >= 'A' && *ptmp1 <= 'Z')
	{
		nv = *ptmp1 - 'A';
	}
	else
		return -1;

	if (*(ptmp1+1) == '(')
	{		
		ptmp1 += 2;
		ptmp2 = ptmp1;
		ndim++;
		elfinal = 0;		
		while(*ptmp2 && (*ptmp2 != ')' || elfinal))
		{
			if (*ptmp2 == '(') 
			{
				elfinal++;
			}
			if (*ptmp2 == ')') 
			{
				elfinal--;
			}
			if (*ptmp2 == ',' && !elfinal)
			{
				ndim++;
			}
			ptmp2++;
		}
		pdim = (int *)malloc(sizeof(int)*ndim);
		memset((void *)pdim,0,sizeof(int)*ndim);		
		ndim = 0;

		do 
		{
			elfinal = 0;
			ptmp2 = ptmp1;
			csal = ')';
			while(*ptmp1 && (*ptmp1 != ')' || elfinal))
			{
				if (*ptmp1 == '(') 
				{
					elfinal++;
				}
				if (*ptmp1 == ')') 
				{
					elfinal--;
				}
				if (*ptmp1 == ',' && !elfinal)
				{
					csal = ',';					
					break;
				}
				ptmp1++;
			}
			if (*ptmp1)
			{
				*ptmp1 = 0;
			}
			else
			{
				csal = 0;
			}
				

			if (*ptmp2 == '%' && *(ptmp2+1) >= 'A' &&  *(ptmp2+1) <= 'Z')
			{
				if (*(ptmp2+2) != '(')
				{
					pdim[ndim] = (int)saca_valor_num_bc3(*(ptmp2+1)-'A',0,NULL);
				}
				else
				{   /* recursion para obtener valores de matriz dentro de matriz */
					int _tipo = 0;
					int nv_ = 0;
					int ndim_ = 0;
					int *pdim_ = NULL;
					_tipo = que_variable_es_bc3(&ptmp2,&nv_,&ndim_,&pdim_);
					if (_tipo == 1)
					{
						pdim[ndim] = (int)saca_valor_num_bc3(nv_,ndim_,pdim_);
					}
					if (pdim_)
					{
						free((void *)pdim_);
					}
					if (*ptmp2 == ')') ptmp2++;
				}
			}
			else
			{
				pdim[ndim] = (int)evalua_expresion_num_bc3(ptmp2);
					/*atoi(ptmp2);*/
			}
			if (pdim[ndim] > 0)
				pdim[ndim]--;
			ndim++;

			if (csal == ',')
			{
				*ptmp1 = csal;		
				ptmp1++;
			}

		} while(csal == ',');
		*ptmp1 = csal;		
	}

	*_nv = nv;
	*_ndim = ndim;
	*_pdim = pdim;
	*ptmp = ptmp1;
	return tipo;
}

static int saca_valor_variable_bc3(char **ptmp,char **valor)
{
	int tipo = 0;
	int nv = 0;
	int ndim = 0;
	int *pdim = NULL;
	char *ptmp1;
	double d;

	tipo = que_variable_es_bc3(ptmp,&nv,&ndim,&pdim);
    if (tipo < 0)
		return -1;
	if (tipo == 1)
	{
		d = saca_valor_num_bc3(nv,ndim,pdim);
		if (d >= 1.0 && d < 27.0)
		{
			*valor = malloc(2);
			(*valor)[0] = 'a'+((int)d - 1);
			(*valor)[1] = 0;
		}
	}
	else if (tipo == 2)
	{

		ptmp1 = saca_valor_alfa_bc3(nv,ndim,pdim);
		if (ptmp1)
		{
			*valor = malloc(strlen(ptmp1)+1);
			strcpy(*valor,ptmp1);
		}
	}

	if (pdim)
	{
		free((void *)pdim);
	}

	return 0;
}

static char *evalua_expresion_sustitucion_bc3(char *ptmp)
{
	char *p;
	int len = strlen(ptmp);
	char *ptmp1,*ptmp2;
	char *valor;	
	int i;

	/* evaluar tamaño final */
	ptmp1 = ptmp;
	while(*ptmp1)
	{
		if ((*ptmp1 == '%' || *ptmp1 == '$') &&
			(*(ptmp1+1) >= 'A' && *(ptmp1+1) <= 'Z'))
		{
			valor = NULL;
			ptmp2 = ptmp1;
			saca_valor_variable_bc3(&ptmp2,&valor);
			if (valor)
			{
				len += strlen(valor);
				free(valor);
			}
			ptmp1 = ptmp2;
		}
		ptmp1++;
	}
	
	p = malloc(len+2);

	i = 0;

	/* sustituir */
	ptmp1 = ptmp;
	while(*ptmp1)
	{
		if ((*ptmp1 == '%' || *ptmp1 == '$') &&
			(*(ptmp1+1) >= 'A' && *(ptmp1+1) <= 'Z'))
		{
			valor = NULL;			
			saca_valor_variable_bc3(&ptmp1,&valor);
			if (valor)
			{
				strcpy(p+i,valor);
				i += strlen(valor);
				free(valor);
			}			
		}
		else
		{
			p[i] = *ptmp1;
			i++;
		}
		ptmp1++;
	}

	p[i] = 0;

	return p;
}

static int es_operador_bc3(int o)
{
if (o == '+' || o == '-' || o == '*' || o == '/' || o == '^') return 1;
if (o == '@' || o == '&' || o == '<' || o == '>' || o == '=' || o == '!') return 2;
return 0;
}

static int codigo_operador_bc3(char *ptmp1)
{
	switch(*ptmp1)
	{
	case '+':	
		return 0;
	case '-':
		return 1;
	case '*':
		return 2;
	case '/':
		return 3;
	case '^':
		return 4;
	case '@':
		return 5;
	case '&':
		return 6;
	case '<':
		if (*(ptmp1+1) == '=')
			return 9;
		if (*(ptmp1+1) == '>')
			return 12;
		return 7;
	case '>':
		if (*(ptmp1+1) == '=')
			return 10;
		return 8;
	case '=':
		return 11;
	case '!':
		return 13;
	default:
		break;
	}

	return -1;
}

static int nivel_operador_bc3(char *ptmp1)
{
	return codigo_operador_bc3(ptmp1);
}


static int que_funcion_bc3(char *ptmp,char *ptmp1)
{
	int l;
	char *p;
	if (*ptmp1 != '(') return 0;
    l = (int)(ptmp1 - ptmp);
	if (l < 3) return 0;	
	if (l >= 5)
	{
	   p = ptmp1 - 5;
	   if (!memcmp(p,"ROUND(",6))
	   {
		   return 12;
	   }
	   if (!memcmp(p,"ATAN2(",6))
	   {
		   return 13;
	   }
	}
	if (l >= 4)
	{
	   p = ptmp1 - 4;
	   if (!memcmp(p,"ASIN(",5))
	   {
		   return 6;
	   }
	   if (!memcmp(p,"ACOS(",5))
	   {
		   return 7;
	   }
	   if (!memcmp(p,"ATAN(",5))
	   {
		   return 8;
	   }
	   if (!memcmp(p,"SQRT(",5))
	   {
		   return 9;
	   }
	   if (!memcmp(p,"ATOF(",5))
	   {
		   return 10;
	   }
	   if (!memcmp(p,"FTOA(",5))
	   {
		   return 11;
	   }
	}
	if (l >= 3)
	{
	   p = ptmp1 - 3;
	   if (!memcmp(p,"ABS(",4))
	   {
		   return 1;
	   }
	   if (!memcmp(p,"INT(",4))
	   {
		   return 2;
	   }
	   if (!memcmp(p,"SIN(",4))
	   {
		   return 3;
	   }
	   if (!memcmp(p,"COS(",4))
	   {
		   return 4;
	   }
	   if (!memcmp(p,"TAN(",4))
	   {
		   return 5;
	   }
	}
	return 0;
}

/* saca las expresiones entre parentesis (ignora arrais) por orden de precedencia */
static char *saca_parentesis_bc3(char *ptmp,int punto)
{
	int nivel = 0;
	char *niveles[200]; /* tiene que sobrar!!! */
	char *niveles1[200]; /* tiene que sobrar!!! */
	char *ptmp1 = ptmp;
	char *p1 = NULL;
	char *p2 = NULL;
	char *ip1 = NULL;
	int maxnivel = -1;
	int ff,i;
	while(*ptmp1)
	{
		if (*ptmp1 == '"')
		{
			ptmp1++;
			while(*ptmp1 && *ptmp1 != '"') 
				ptmp1++;
		}
		if (*(unsigned char *)ptmp1 < 32)
		{
			ptmp1 += 2;
			continue;
		}
		if (*ptmp1 == '(')
		{
			if (nivel >= 200)
			{
				__aviso(0,"Demasiados parentesis anidados");
				return NULL;
			}
			ff = 0;
			if (ptmp1 == ptmp || *(ptmp1-1) == '(' || es_operador_bc3(*(ptmp1-1)) || 
				(ff = que_funcion_bc3(ptmp,ptmp1)))
			{
			   niveles[nivel] = ptmp1; /* bueno */
			   if (ff)
			   {
				   if (ff > 11)
				   {
					   niveles1[nivel] = ptmp1 - 5;
				   }
				   else
				   if (ff < 6)
				   {
					   niveles1[nivel] = ptmp1 - 3;
				   }
				   else
				   {
					   niveles1[nivel] = ptmp1 - 4;
				   }
			   }
			   else
			   {
				  niveles1[nivel] = ptmp1;
			   }
			}
			else
			{
			   niveles[nivel] = NULL; /* malo */	
			}
			nivel++;
		}
		if (*ptmp1 == ')')
		{
			nivel--;
			if (niveles[nivel])
			{
				if (nivel > maxnivel)
				{
					maxnivel = nivel;
					ip1 = niveles1[nivel];
					p1 = niveles[nivel];
					p2 = ptmp1;
				}
			}
		}
		ptmp1++;
	}
	if (maxnivel < 0 || !p1 || !p2)
		return NULL;

	p1++;
	ptmp1 = malloc( (int)(p2-p1)+1 ); 
	memcpy(ptmp1,p1,(int)(p2-p1));
	ptmp1[(int)(p2-p1)] = 0;


	ff = 0;
	for (i = 0;ptmp1[i];i++)
	{
		if (ptmp1[i] == '"')
		{
			i++;
			while(ptmp1[i] && ptmp1[i] != '"') 
				i++;
		}
		if (ptmp1[i]) break;
		if (ptmp1[i] == '(')
			ff++;
		if (ptmp1[i] == ')')
			ff--;
		if (ptmp1[i] == ',' && !ff)
		{
			ptmp1[i] = (char)((unsigned char)255);
		}
	}	

	*ip1 = 1+que_funcion_bc3(ip1,p1-1);
	if (*ip1 > 1) (*ip1)++;
	ip1++;
	*ip1 = punto;
	ip1++;
    strcpy(ip1,p2+1);

	return ptmp1;
}

static char **saca_prioridad_bc3(char *ptmp)
{
	int nivel;
	char *inip = NULL;	
	char *p1 = NULL;
	char *p2 = NULL;
	int maxnivel = -1;
	char *ptmp1;
	int punto  = 0;
	char **r;
	unsigned char *px1;
	
	r = (char **)malloc(sizeof(char *)*200); /* 200 limite de momento */
	memset(r,0,sizeof(char *)*200);
	

	do
	{
		px1 = (unsigned char *)ptmp;
		if (*px1 == (unsigned char)255)
		{
			px1 = NULL;
			ptmp++;
		}
		else
		{
			while(*px1 && *px1 != (unsigned char)255) px1++;
			if (*px1)
			{
				*px1 = 0;
			}
			else
			{
				px1 = NULL;
			}
		}
		
		ptmp1 = ptmp;
		inip = ptmp;
		p1 = NULL;
		p2 = NULL;
		maxnivel = -1;

		while(*ptmp1)
		{
			if (*ptmp1 == '"')
			{
				ptmp1++;
				while(*ptmp1 && *ptmp1 != '"') 
					ptmp1++;
			}

			if (*(unsigned char *)ptmp1 < 32)
			{
				ptmp1 += 2;
				continue;
			}
			if (es_operador_bc3(*ptmp1))
			{
				nivel = nivel_operador_bc3(ptmp1);
				if (nivel == 9 || nivel == 10 || nivel == 12)
					ptmp1++;
				ptmp1++;
				if (nivel > maxnivel)
				{
					p1 = inip;				
				}
				inip = ptmp1;
				while(*ptmp1)
				{
					if (*ptmp1 == '"')
					{
						ptmp1++;
						while(*ptmp1 && *ptmp1 != '"') 
							ptmp1++;
					}
					if (*(unsigned char *)ptmp1 < 32)
					{
						ptmp1 += 2;
						continue;
					}
					if (es_operador_bc3(*ptmp1))
					{
						break;
					}
					ptmp1++;
				}
				if (nivel > maxnivel)
				{
					p2 = ptmp1;
					maxnivel = nivel;
				}
			}
			else
				ptmp1++;
		}

		if (px1)
		{
			*px1 = (unsigned char)255;
		}

		if (maxnivel > -1 && p1 && p2)
		{			
			ptmp1 = malloc( (int)(p2-p1)+1 ); 
			memcpy(ptmp1,p1,(int)(p2-p1));
			ptmp1[(int)(p2-p1)] = 0;			
			*p1 = 2;
			p1++;
			*p1 = (punto+1);
			p1++;
			strcpy(p1,p2);

			r[punto] = ptmp1;

			punto++;
		}
	} while(maxnivel > -1);


	
	if (punto)
	{
		/*
		if (ptmp[0] != 2)
		{
			__aviso(0,"error intero ptmp[0] != 2");
		}
		if (ptmp[1] != (punto -1))
		{
			__aviso(0,"error intero ptmp[1] != (punto -1)");
		}
		*/
		/*
		punto--;
		strcpy(ptmp,r[punto]);
		free(r[punto]);
		r[punto] = ptmp;
		*/
	}
	if (strlen(ptmp))
	{
		r[punto] = malloc(strlen(ptmp)+1);
		strcpy(r[punto],ptmp);
		punto++;
	}
	return r;
}

static int saca_operandos_bc3(char *ptmp,char **_op1,char **_op2)
{	
	int operacion = -1;
	int l1,l2;
	char *p1 = ptmp;

	while (*p1)
	{
	   if (*((unsigned char *)p1) == 255)
	   {
		   *p1 = 0; /* eliminacion de parametros !!!! */
	   }
	   p1++;		
	}

	p1 = ptmp;

	while (*ptmp)
	{
		if (*ptmp == '"')
		{
			ptmp++;
			while(*ptmp && *ptmp != '"') 
				ptmp++;
		}
		if (*(unsigned char *)ptmp < 32)
		{
			ptmp += 2;
			continue;
		}
		if (es_operador_bc3(*ptmp))
		{
			operacion = codigo_operador_bc3(ptmp);
			l1 = (int)(ptmp - p1);
			if (!*_op1)
			{
				*_op1 = malloc(l1+1);
			}
			memcpy(*_op1,p1,l1);
			(*_op1)[l1] = 0;
			if (operacion == 9 || operacion == 10 || operacion == 12)
				ptmp++;
			ptmp++;			
			l2 = strlen(ptmp);
			if (!*_op2)
			{
				*_op2 = malloc(l2+1);
			}
			strcpy(*_op2,ptmp);

			return operacion;
		}
		ptmp++;
	}
	return -1;

}

static int es_booleana_bc3(char *ptmp)
{
	while (*ptmp)
	{
		if (*ptmp == '"')
		{
			ptmp++;
			while(*ptmp && *ptmp != '"') 
				ptmp++;
		}
		if (*(unsigned char *)ptmp < 32)
		{
			ptmp += 2;
			continue;
		}
		if (es_operador_bc3(*ptmp) == 2)
		{
			return 1;
		}
		ptmp++;
	}
	return 0;
}

static double funcion_numerica_bc3(int codigo,double d1)
{
	double r = d1;

	switch(codigo)
	{
	case 1:
		r = fabs(d1);
		break;
	case 2:
		r = floor(d1);
		break;
	case 3:
		r = sin(d1);
		break;
	case 4:
		r = cos(d1);
		break;
	case 5:
		r = tan(d1);
		break;
	case 6:
		r = asin(d1);
		break;
	case 7:
		r = acos(d1);
		break;
	case 8:
		r = atan(d1);
		break;
	case 9:
		r = sqrt(d1);
		break;
	case 10:
		r = d1;
		break;
	case 11:
		r = d1;
		break;
    case 12:
		r = redondea(d1,0);
		break;
	case 13:
		r = atan2(d1,0);
		break;
	}

	return r;
}

static char *funcion_alfanumerica_bc3(int codigo,double d1)
{
	char *r = malloc(200);
	sprintf(r,"%lf",d1);
	return r;
}



static double saca_valor_numerico_bc3(char *ptmp,void *_resultados,void *_parciales,int modovalor)
{
	int _tipo = 0;
	int nv_ = 0;
	int ndim_ = 0;
	int *pdim_ = NULL;	
	char **charvalor1;
	double *numvalor1;
	char **charvalor2;
	double *numvalor2;
	double d1 = 0.0;
	char *tmp;

	if (!modovalor)
	{
		charvalor1 = (char **)_resultados;
		charvalor2 = (char **)_parciales;
	}
	else
	{
		numvalor1 = (double *)_resultados;
		numvalor2 = (double *)_parciales;
	}
	
	if (ptmp[0] >= 'a' && ptmp[0] <= 'z')
	{
		d1 = (double)(ptmp[0]-'a')+1;
	}
	else if (ptmp[0] != 2 && (unsigned char)ptmp[0] < 32)
	{
		if (!modovalor)
		{
			tmp = charvalor1[ptmp[1]-1];
			if (*tmp >= 'a' && *tmp <= 'z')
			{
				d1 = (double)(tmp[0]-'a')+1;
			}
			else
			{
				if (*tmp == '"')
					tmp++;
				d1 = rwatof(tmp);
			}
		}
		else
		{
			d1 = numvalor1[ptmp[1]-1];
		}
		d1 = funcion_numerica_bc3(ptmp[0]-2,d1);
	}
	else if (ptmp[0] == 2)
	{
		if (!modovalor)
		{
			tmp = charvalor2[ptmp[1]-1];
			if (*tmp >= 'a' && *tmp <= 'z')
			{
				d1 = (double)(tmp[0]-'a')+1;
			}
			else
			{
				d1 = rwatof(tmp);
			}
		}
		else
		{
			d1 = numvalor2[ptmp[1]-1];
		}
	}
	else
	{		
		_tipo = que_variable_es_bc3(&ptmp,&nv_,&ndim_,&pdim_);
		if (_tipo == 2)
		{
			d1 = rwatof(saca_valor_alfa_bc3(nv_,ndim_,pdim_));
		}
		else if (_tipo == 1)
		{
			d1 = saca_valor_num_bc3(nv_,ndim_,pdim_);
		}
		else if (_tipo < 0)
		{
			d1 = rwatof(ptmp);
		}
	}

	if (pdim_)
	{
		free((void *)pdim_);
	}

	return d1;
}

static double resultado_operacion_numerica_bc3(char *ptmp,void *_resultados,void *_parciales)
{
	int operacion;
	char _op1[512]; /* en numericos no tiene sentido mas */
	char _op2[512];
	char *op1 = _op1;
	char *op2 = _op2;
	double d1,d2;
	double r = 0.0;

	operacion = saca_operandos_bc3(ptmp,&op1,&op2);
	if (operacion == -1)
	{
		r = saca_valor_numerico_bc3(ptmp,_resultados,_parciales,1);
		return r;
	}	

	if (operacion > 4)
	{
		__aviso(0,"error interno operacion < 5");
	}

	d1 = saca_valor_numerico_bc3(op1,_resultados,_parciales,1);	

	d2 = saca_valor_numerico_bc3(op2,_resultados,_parciales,1);

	switch(operacion)
	{
	case 0:		
		r = d1 + d2;
		break;
	case 1:
		r = d1 - d2;
		break;
	case 2:
		r = d1 * d2;
		break;
	case 3:
		if (d2)
			r = d1 / d2;
		else
			r = 0.0;
		break;
	case 4:	
		r = pow(d1,d2);
		break;
	}

	return r;
}

static char *saca_valor_alfanumerico_bc3(char *ptmp,void *_resultados,void *_parciales,int modovalor)
{
	int _tipo = 0;
	int nv_ = 0;
	int ndim_ = 0;
	int *pdim_ = NULL;	
	char **charvalor1;
	double *numvalor1;
	char **charvalor2;
	double *numvalor2;
	double d1 = 0.0;
	char *p1 = NULL;
	char *tmp = NULL;

	if (!modovalor)
	{
		charvalor1 = (char **)_resultados;
		charvalor2 = (char **)_parciales;
	}
	else
	{
		numvalor1 = (double *)_resultados;
		numvalor2 = (double *)_parciales;
	}
	
	if (ptmp[0] != 2 && (unsigned char)ptmp[0] < 32)
	{
		if (!modovalor)
		{
			p1 = charvalor1[ptmp[1]-1];
			/*p1 = funcion_alfanumerica_bc3(ptmp[0]-2,rwatof(p1));*/
		}
		else
		{
			d1 = numvalor1[ptmp[1]-1];
			/*p1 = tmp = funcion_alfanumerica_bc3(ptmp[0]-2,d1);*/
		}		
	}
	else if (ptmp[0] == 2)
	{
		if (!modovalor)
		{
			p1 = charvalor2[ptmp[1]-1];
		}
		else
		{
			d1 = numvalor2[ptmp[1]-1];
		}
	}
	else
	{		
		_tipo = que_variable_es_bc3(&ptmp,&nv_,&ndim_,&pdim_);
		if (_tipo == 2)
		{
			p1 = saca_valor_alfa_bc3(nv_,ndim_,pdim_);
		}
		else if (_tipo == 1)
		{
			d1 = saca_valor_num_bc3(nv_,ndim_,pdim_);
		}
		else if (_tipo < 0)
		{
			p1 = ptmp;
		}
	}

	if (pdim_)
	{
		free((void *)pdim_);
	}
	if (!tmp)
	{
		if (p1)
		{
			tmp = malloc(strlen(p1)+1);
			strcpy(tmp,p1);
		}
		else
		{
			tmp = funcion_alfanumerica_bc3(11,d1);
		}
	}
	return tmp;
}

static void descomilla_alfa(char *ptmp)
{
	int l;
	if (*ptmp == '"')
	{
		strcpy(ptmp,ptmp+1);
		l = strlen(ptmp);
		l--;
		if (l>=0)
		{
			if (ptmp[l] == '"')
			{
				ptmp[l] = 0;
			}
		}
	}
}


static int resultado_operacion_booleana_bc3(char *ptmp,void *_resultados,void *_parciales,int modovalor)
{	
	int operacion;
	char _op1[512]; /* en booleanos no tiene sentido mas */
	char _op2[512];
	char *op1 = _op1;
	char *op2 = _op2;
	double d1,d2;
	char *ra1=NULL,*ra2=NULL;
	int r = 0;
	int esalfa = 0;

	operacion = saca_operandos_bc3(ptmp,&op1,&op2);
	if (operacion == -1)
	{
		r =(int)saca_valor_numerico_bc3(ptmp,_resultados,_parciales,modovalor);
		return r;
	}

	if (operacion < 5)
	{
		__aviso(0,"error interno operacion < 5");
	}

	if (operacion != 13)
	{
		if (operacion != 5 && operacion != 6 && (op1[0] == '$' || op1[0] == '"') && (op2[0] == '$' || op2[0] == '"'))
		{
			esalfa = 1;
		}
		if (esalfa)
		{
			ra1 = saca_valor_alfanumerico_bc3(op1,_resultados,_parciales,modovalor);
			if (!ra1)
			{
				ra1 = malloc(1);
				ra1[0] = 0;
			}
			else
				descomilla_alfa(ra1);
		}
		else
		{
			d1 = saca_valor_numerico_bc3(op1,_resultados,_parciales,modovalor);
		}
	}

	if (esalfa)
	{
		ra2 = saca_valor_alfanumerico_bc3(op2,_resultados,_parciales,modovalor);
		if (!ra2)
		{
			ra2 = malloc(1);
			ra2[0] = 0;
		}
		else
			descomilla_alfa(ra2);
		switch(operacion)
		{
		case 7:
			if (strcmp(ra1,ra2) < 0)
				r = 1;
			break;
		case 8:
			if (strcmp(ra1,ra2) > 0)
				r = 1;
			break;
		case 9:
			if (strcmp(ra1,ra2) <= 0)
				r = 1;
			break;
		case 10:
			if (strcmp(ra1,ra2) >= 0)
				r = 1;
			break;
		case 11:
			if (!strcmp(ra1,ra2))
				r = 1;
			break;
		case 12:
			if (strcmp(ra1,ra2))
				r = 1;
			break;
		}
	}
	else
	{
		d2 = saca_valor_numerico_bc3(op2,_resultados,_parciales,modovalor);
		switch(operacion)
		{
		case 5:
			if (d1 || d2)
				r = 1;
			break;
		case 6:
			if (d1 && d2)
				r = 1;
			break;
		case 7:
			if (d1 < d2)
				r = 1;
			break;
		case 8:
			if (d1 > d2)
				r = 1;
			break;
		case 9:
			if (d1 <= d2)
				r = 1;
			break;
		case 10:
			if (d1 >= d2)
				r = 1;
			break;
		case 11:
			if (d1 == d2)
				r = 1;
			break;
		case 12:
			if (d1 != d2)
				r = 1;
			break;
		case 13:
			if (!d2)
				r = 1;
			break;
		}
	}

	if (ra1)
		free(ra1);
	if (ra2)
		free(ra2);

	return r; /* 0 o 1 */
}

static char **lista_parentesis_bc3(char *ptmp)
{
    char *tmp,*ptmp1;
	char **r;
    int i;

	r = (char **)malloc(sizeof(char *)*200); /* 200 limite de momento */
	memset(r,0,sizeof(char *)*200);
    
	tmp = malloc(strlen(ptmp)+1);
	strcpy(tmp,ptmp);

	/* primero sacamos los parentesis */
	i = 0;
    do
	{
	   ptmp1 = saca_parentesis_bc3(tmp,i+1);
	   if (ptmp1)
	   {
		   r[i] = ptmp1;
	   }
	   else
	   {
		   r[i] = tmp;
	   }
	   i++;
	} while(ptmp1);
	r[i] = NULL;


	return r;
}

static double evalua_expresion_num_bc3(char *ptmp)
{
	double r;	
    char **listaparentesis = lista_parentesis_bc3(ptmp);
	char **listaorden = NULL;
	double resultados[200];
	double parciales[200];		
	int i,j;
	int asignado;
	int ulti = 0;

	for (i = 0;i < 200;i++)
	{
	    resultados[i] = 0.0;
		parciales[i] = 0.0;
	}

	for (i = 0;listaparentesis[i];i++)
	{		
		listaorden = saca_prioridad_bc3(listaparentesis[i]);
		asignado = 0;
		for (j = 0;listaorden[j];j++)
		{
			if (es_booleana_bc3(listaorden[j]))
			{
				parciales[j] = (double)resultado_operacion_booleana_bc3(listaorden[j],resultados,parciales,1);
			}
			else
			{
				parciales[j] = resultado_operacion_numerica_bc3(listaorden[j],resultados,parciales);
			}
			if (!listaorden[j+1])
			{
				resultados[i] = parciales[j];				
				asignado = 1;
			}
			free(listaorden[j]);
		}
		free(listaorden);
		if (!asignado)
		{
			resultados[i] = saca_valor_numerico_bc3(ptmp,resultados,parciales,1);
		}		
		free(listaparentesis[i]);		
		ulti = i;
	}
	free(listaparentesis);

	r = resultados[ulti];

    return r;
}


static char *resultado_operacion_alfa_bc3(char *ptmp,void *_resultados,void *_parciales)
{
	int operacion;
	char *op1 = NULL;
	char *op2 = NULL;	
	char *r1;
	char *r2;
	char *r = NULL;
	int i,j;

	operacion = saca_operandos_bc3(ptmp,&op1,&op2);	
	if (operacion == -1)	
	{
		r = saca_valor_alfanumerico_bc3(ptmp,_resultados,_parciales,0);
		return r;
	}

	if (operacion != 0 && operacion != 2)
	{
		__aviso(0,"error interno operacion != 0 && operacion != 2");
	}

	r1 = saca_valor_alfanumerico_bc3(op1,_resultados,_parciales,0);
	if (op1)
	{
		free(op1);
	}
	r2 = saca_valor_alfanumerico_bc3(op2,_resultados,_parciales,0);
	if (op2)
	{
		free(op2);
	}

	if (!operacion)
	{
		r = malloc(strlen(r1)+strlen(r2)+3);
		r[0] = '"';
		r[1] = 0;
		if (r1[0] == '"')
			i = 1;
		else
			i = 0;
		if (r1[0])
		   strcpy(r+1,r1+1);
		j = strlen(r) - 1;
		if (!j || r[j] != '"')
			j++;
		if (r2[0] == '"')
			i = 1;
		else
			i = 0;
		strcpy(r+j,r2+i);
		j = strlen(r) - 1;
		if (r[j] != '"')
		{
			j++;
			r[j] = '"';
			r[j+1] = 0;
		}
		if (r[0] == '"' && !r[1])
		{
			r[0] = 0;
		}
	}
	else
	{
		if (r2[0] == '1')
		{
			r = malloc(strlen(r1)+1);
			strcpy(r,r1);
		}
		else
		{
			r = malloc(2);
			r[0] = 0;
		}	
	}

	if (r1)
		free(r1);
	if (r2)
		free(r2);
   return r;
}

static char *evalua_expresion_alfa_bc3(char *ptmp)
{
	char *r = NULL;	
    char **listaparentesis = lista_parentesis_bc3(ptmp);
	char **listaorden = NULL;
	char *resultados[200];
	char *parciales[200];	
	int i,j;

	for (i = 0;i < 200;i++)
	{
	    resultados[i] = NULL;
		parciales[i] = NULL;
	}

	for (i = 0;listaparentesis[i];i++)
	{		
		listaorden = saca_prioridad_bc3(listaparentesis[i]);
		for (j = 0;listaorden[j];j++)
		{
			if (es_booleana_bc3(listaorden[j]))
			{
				parciales[j] = malloc(2);
				parciales[j][0] = 48 + resultado_operacion_booleana_bc3(listaorden[j],resultados,parciales,0);
				parciales[j][1] = 0;
			}
			else
			{
				parciales[j] = resultado_operacion_alfa_bc3(listaorden[j],resultados,parciales);
			}
			if (!listaorden[j+1])
			{
				resultados[i] = parciales[j];
				parciales[j] = NULL;
			}
			free(listaorden[j]);
		}
		free(listaorden);
		for (j = 0;parciales[j];j++)
		{
			free(parciales[j]);
		}
		if (!resultados[i])
		{
			resultados[i] = listaparentesis[i];
		}
		else
		{
			free(listaparentesis[i]);
		}
	}
	free(listaparentesis);

	for (j = 0;resultados[j];j++)
	{
		if (!resultados[j+1])
		{
			r = resultados[j];
		}
		else
		{
			free(resultados[j]);
		}
	}

	if (r)
	{
		if (*r == '"')
		{
			strcpy(r,r+1);
			j = strlen(r)-1;
			if (j >= 0)
			{
				if (r[j] == '"')
					r[j] = 0;
			}
		}
	}

	return r;
}


static int incluye_desglose_bc3(int ndesglose,char **_desglose,double *_cantidad,double *precios,char *codigo,double cantidad,int precio_asignado)
{
    char *p = NULL;
	double precio = 0.0;
	if (_desglose)
	{
	   _desglose[ndesglose] = codigo;
	}
	_cantidad[ndesglose] = cantidad;
	precios[ndesglose] = 0.0;
	if (!precio_asignado)
	{		
		busca_en_parametrico_bc3(codigo,&p);
		if (p)
		{
			_mi_variable ___variable[MAXBC3VAR];
			memcpy(___variable,_variable,sizeof(_mi_variable)*MAXBC3VAR);
			resuelve_parametrico_bc3(codigo,p,NULL,NULL,NULL,NULL,NULL,&precio,NULL);
			memcpy(_variable,___variable,sizeof(_mi_variable)*MAXBC3VAR);
			precios[ndesglose] = precio;
			free(p);
		}
		else
		{
			busca_en_articulo_bc3(codigo,NULL,&precio);
			precios[ndesglose] = precio;
		}
	}
	ndesglose++;
	return ndesglose;
}

static int verifica_datos_parametrico_bc3(char **_buffer,char *pcodigo,char *codigo)
{
	    char *buffer = *_buffer;
		char *ptmp1;
		/* Comprobar validez del registro (debe incluir el codigo) */
		if (*buffer != '~' || *(buffer+1) != 'P') return -1;
		buffer += 2;
		while(*buffer && *buffer == ' ') buffer++;
		if (*buffer != '|') return -2;
		buffer++;
		while(*buffer && *buffer == ' ') buffer++;
		ptmp1 = buffer;	
		while(*buffer && *buffer != ' ' && *buffer != '|') buffer++;
		memcpy(codigo,ptmp1,(int)(buffer-ptmp1));
		codigo[(int)(buffer-ptmp1)] = 0;
		while(*buffer && *buffer == ' ') buffer++;
		if (*buffer != '|') return -3;
		buffer++;
		if (codigo[6] != '$') return -4;
		if (memcmp(codigo,pcodigo,6)) return -5;

		*_buffer = buffer;

		return 0;
}

int resuelve_parametrico_bc3(char *pcodigo,char *buffer,char **_texto,char **_resumen,char **_desglose,double *_cantidad,double *_precios,double *_precio,char **_error)
{
	char codigo[256];
	char tmp[256];
	char *ptmp1,*ptmp2,*_buffer = NULL;
	char **lineas = NULL;
	int salida = 0;
	int i;	
	int numero_parametros = 0;
	int parametros_pasados = 0;
	int precio_asignado = 0;
	int ndesglose = 0;	
	double precios[BC3MAXDESGLOSE];
	double lcantidad[BC3MAXDESGLOSE];
	/* se usan como maximo 4 parametros ... */
	char **parametros[4];

	if (_precio)
	{
		*_precio = 0.0;
	}

	if (buffer)
	{
		i = verifica_datos_parametrico_bc3(&buffer,pcodigo,codigo);
		if (i < 0) 
			return i;
	}	
	
	parametros_pasados = strlen(pcodigo+6);
	if (parametros_pasados > 4)
	{
		return -6;
	}

	if (!_cantidad)
	{
		_cantidad = lcantidad;
	}
	if (!_precios)
	{
		_precios = precios;
	}
	if (!buffer)
	{
        busca_en_parametrico_bc3(pcodigo,&_buffer);
		if (!_buffer) return -999;		
		buffer = _buffer;
		i = verifica_datos_parametrico_bc3(&buffer,pcodigo,codigo);
		if (i < 0) 
		{			
			free(_buffer);
		    return i;
		}
	}

	/* inicializar variables, teniendo en cuenta los parametros */
	for (i = 0;i < parametros_pasados;i++)
	{
		_variable[i].ndimalfa = 0;
		_variable[i].dimalfa = NULL;
		_variable[i].localmemo = 0;
		_variable[i].alfa = NULL;		
		_variable[i].ndimnum = pcodigo[6+i]-'a'+1;
		_variable[i].dimnum = NULL;
		_variable[i].num = NULL;
	}

	for (;i < MAXBC3VAR;i++)
	{		
		_variable[i].ndimalfa = 0;
		_variable[i].dimalfa = NULL;
		_variable[i].localmemo = 0;
		_variable[i].alfa = NULL;
		_variable[i].ndimnum = 0;
		_variable[i].dimnum = NULL;
		_variable[i].num = NULL;
	}


	/* crear la referencia de cada linea */
    lineas = linealiza_parametrico_bc3(buffer);
	/* limpiar segun normas fiebdc */
	limpia_parametrico_bc3(lineas);

	/* analizar el parametrico */
	{
		int dentro;
		int j,k;
		for (i = 0;lineas[i];i++)
		{
			ptmp1 = lineas[i];
			if (*ptmp1 == '\\')
			{				
				ptmp2 = tmp;
				if (saca_un_parametro_bc3(&ptmp1,&ptmp2))
				{
					if (!strcmp(tmp,"TEXTO") || !strcmp(tmp,"T"))
					{	
						if (_texto)
						{
							*_texto = NULL;
							ptmp2 = NULL;														
							saca_un_parametro_bc3(&ptmp1,&ptmp2);							
							if (ptmp2)
							{
								*_texto = evalua_expresion_sustitucion_bc3(ptmp2);
								free(ptmp2);
								ptmp2 = NULL;
							}
							
						}

					} else if (!strcmp(tmp,"RESUMEN") || !strcmp(tmp,"R"))
					{
						if (_resumen)
						{
							*_resumen = NULL;
							ptmp2 = NULL;							
							saca_un_parametro_bc3(&ptmp1,&ptmp2);
							if (ptmp2)
							{
								*_resumen = evalua_expresion_sustitucion_bc3(ptmp2);
								free(ptmp2);
								ptmp2 = NULL;
							}							
						}
					} else
					{
						for (j = 0;_Sustituciones_bc3[j];j++)
						{
							if (!strcmp(_Sustituciones_bc3[j],tmp))
							{
								break;
							}
						}
						if (!_Sustituciones_bc3[j])
						{
							if (numero_parametros < parametros_pasados)
							{
								/* es un rotulo de parametros */
								free_varalfa(numero_parametros);
								j = _cuenta_parametros(ptmp1);
								parametros[numero_parametros] = (char **)malloc(sizeof(char *)*(j+2));
								parametros[numero_parametros][0] = malloc(strlen(tmp)+1);
								strcpy(parametros[numero_parametros][0],tmp);
								for (k = 1;k <= j;k++)
								{								
									parametros[numero_parametros][k] = NULL;
									if (!saca_un_parametro_bc3(&ptmp1,&(parametros[numero_parametros][k])))
									{								
										numero_parametros++;
										salida = -7;
										goto salida;										
									}
								}								
								parametros[numero_parametros][k] = NULL;

								k = _variable[numero_parametros].ndimnum;
								_variable[numero_parametros].ndimalfa = 0;
								_variable[numero_parametros].dimalfa = NULL;								
								_variable[numero_parametros].localmemo = 0;
								if (k > 0 && k <= j)
								{
									_variable[numero_parametros].alfa = parametros[numero_parametros][k];
								}
								else
								{
									_variable[numero_parametros].alfa = NULL;
								}
								numero_parametros++;
								if (('a'+j) <= pcodigo[5+numero_parametros])
								{
									salida = -8;
									goto salida;
								}
							}
							else
							{
								salida = -9;
								goto salida;
							}
						}
					}
				}				
			}
			else
			{
				dentro = 0;
				ptmp2 = ptmp1;
				while(*ptmp2)
				{
					if (*ptmp2 == '"')
						dentro = !dentro;
					else if (!dentro)
					{
						if (*ptmp2 == ':' && *(ptmp2+1) == ':')
						{
							/* PRECIO! */
						   if (_precio)
						   {
							   *_precio = evalua_expresion_num_bc3(ptmp2+2);
						   }
						   precio_asignado = 1;
						   break;
						}
						else if (*ptmp2 == ':')
						{
							char *codigo = NULL;
							double cantidad = 0.0;
							*ptmp2 = 0;							
							/* desglose */
							codigo = evalua_expresion_sustitucion_bc3(ptmp1);
							cantidad = evalua_expresion_num_bc3(ptmp2+1);
							if (_desglose)
							{
								_desglose[ndesglose] = NULL;
							}
							if (cantidad && codigo)
							{
								ndesglose = incluye_desglose_bc3(ndesglose,_desglose,_cantidad,_precios,codigo,cantidad,precio_asignado);
							}
							else
							{
								if (codigo)
									free(codigo);
							}
							*ptmp2 = ':';
						    break;
						}
						else if (*ptmp2 == '=')
						{
							/* declaracion de variable */
							{
								int nv = *(ptmp1+1)-'A';
								int ndim = 0;
								int *pdim = NULL;
								if (*(ptmp1+2) == '(')
								{
									ndim++;
									ptmp2 = ptmp1+3;									
									while(*ptmp2 && *ptmp2 != ')')
									{
										if (*ptmp2 == ',')
										   ndim++;
										ptmp2++;
									}
									pdim = (int *)malloc(sizeof(int)*ndim);
									memset((void *)pdim,0,sizeof(int)*ndim);
									ndim = 0;
									ptmp2 = ptmp1+3;
									pdim[ndim] = atoi(ptmp2);
									ndim++;
									while(*ptmp2 && *ptmp2 != ')')
									{
										if (*ptmp2 == ',')
										{										   
										   pdim[ndim] = atoi(ptmp2+1);
										   ndim++;
										}
										ptmp2++;
									}
									if (*ptmp2 == ')')
										ptmp2++;									
								}
								if (*ptmp2 != '=')
								{
									__aviso(0,"Error interno en proceso de evaluacion");
								}
								if (*ptmp1 == '%')
								{
									free_varnum(nv);
									_variable[nv].ndimnum = ndim;
									_variable[nv].dimnum = pdim;
									_variable[nv].num = NULL;
									if (ndim)
									{
										_variable[nv].num = rellena_numarrai_bc3(ndim,pdim,ptmp2+1);
									}
									else
									{
										_variable[nv].num = malloc(sizeof(double));
										*(_variable[nv].num) = evalua_expresion_num_bc3(ptmp2+1);
										/* condicion de error */
										if (nv == 4 && *(_variable[nv].num))
										{
											if (_error)
											{
												if (_variable[nv].alfa && !_variable[nv].dimalfa)
												{
													*_error = malloc(strlen(_variable[nv].alfa)+1);
													strcpy(*_error,_variable[nv].alfa);
												}
												else
												{
													*_error = malloc(100);
													strcpy(*_error,"::Condicion de error evaluada::");
												}
											}
											salida = -10;
											goto salida;
										}
									}
								}
								else if (*ptmp1 == '$')
								{
									free_varalfa(nv);
									_variable[nv].ndimalfa = ndim;
									_variable[nv].dimalfa = pdim;
									_variable[nv].localmemo = 1;
									_variable[nv].alfa = NULL;
									if (ndim)
									{
										_variable[nv].alfa = rellena_alfaarrai_bc3(ndim,pdim,ptmp2+1);
									}
									else
									{
										_variable[nv].alfa = evalua_expresion_alfa_bc3(ptmp2+1);
									}
								}
								else
								{
									salida = -11;
									goto salida;
								}
							}
							break;
						}
						else if (*ptmp2 == '%' && *(ptmp2+1) == ':')
						{
							/* medios auxiliares tanto por uno */
							break;
						}
						else if (*ptmp2 == '%' && *(ptmp2+1) == '%' && *(ptmp2+2) == ':')
						{
							/* medios auxiliares % */
							break;
						}
					}
					ptmp2++;
				}
			}
		}
	}

salida:
	/* clean-up */
	if (_buffer)
	{
		free(_buffer);
	}
	{
		int k;		

		for (i = 0;i < MAXBC3VAR;i++)
		{	
			free_varalfa(i);
			free_varnum(i);			 
		}


		for (k = 0;k <numero_parametros;k++)
		{
			if (parametros[k])
			{
				i = 0;
				while(parametros[k][i])
				{
					free(parametros[k][i]);
					i++;
				}
				free(parametros[k]);
			}
		}
		if (lineas)
		{
			free((void *)lineas);
		}	
	}
	if (salida >= 0 && !precio_asignado && ndesglose && _precio)
	{
		*_precio = 0.0;
		for (i = 0;i < ndesglose;i++)
		{
			*_precio += (_cantidad[i] * _precios[i]);
		}
	}
	if (salida >= 0)
	{
		salida = ndesglose;
	}
	return salida;
}


int lista_de_parametros_bc3(char *pcodigo,char **parametros[])
{
	char *_buffer = NULL;
	char *buffer = NULL;
	char codigo[256];
	int i;
	char **lineas = NULL;
	char *ptmp1,*ptmp2;
	char tmp[256];
	int numero_parametros = 0;

    busca_en_parametrico_bc3(pcodigo,&_buffer);
	if (!_buffer) return -999;		
	buffer = _buffer;
	i = verifica_datos_parametrico_bc3(&buffer,pcodigo,codigo);
	if (i < 0) 
	{			
		free(_buffer);
		return i;
	}
    lineas = linealiza_parametrico_bc3(buffer);	
	limpia_parametrico_bc3(lineas);


	/* analizar el parametrico */
	{		
		int j,k;
		for (i = 0;lineas[i];i++)
		{
			ptmp1 = lineas[i];
			if (*ptmp1 == '\\')
			{				
				ptmp2 = tmp;
				if (saca_un_parametro_bc3(&ptmp1,&ptmp2))
				{					
					for (j = 0;_Sustituciones_bc3[j];j++)
					{
						if (!strcmp(_Sustituciones_bc3[j],tmp))
						{
							break;
						}
					}
					if (!_Sustituciones_bc3[j])
					{						
						{
							/* es un rotulo de parametros */							
							j = _cuenta_parametros(ptmp1);
							parametros[numero_parametros] = (char **)malloc(sizeof(char *)*(j+2));
							parametros[numero_parametros][0] = malloc(strlen(tmp)+1);
							strcpy(parametros[numero_parametros][0],tmp);
							for (k = 1;k <= j;k++)
							{								
								parametros[numero_parametros][k] = NULL;
								if (!saca_un_parametro_bc3(&ptmp1,&(parametros[numero_parametros][k])))
								{								
									numero_parametros++;
									break;
								}
							}								
							parametros[numero_parametros][k] = NULL;
							numero_parametros++;
						}
					}
				}
			}
		}
	}

	if (lineas)
	{
	    free((void *)lineas);
	}	
	if (_buffer)
	{
		free(_buffer);
	}
	return numero_parametros;
}

int pon_provincia_bc3(int provincia)
{
	provincia_bc3 = provincia;
	return provincia_bc3;
}
