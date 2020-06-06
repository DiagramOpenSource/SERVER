

/********************************************************************************/


#include <fgvsys.h>


#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef O_BINARY
#include <io.h>
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


#include <stdio.h>
#include <malloc.h>
#include <rwmalloc.h>
#include <memory.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>

#ifndef WIN32
#include <sys/time.h>
#endif

#include <rtermin.h>


#define _NUEVODEF
#include <rdef.h>      /* ya incluye el isam.h */

#ifndef IXXMAXFILES
#define IXXMAXFILES 100
#endif

extern char *bbdd_file_path;

extern int bbdd_tiempo_rotate;

extern int con_respaldo_externo;
extern char *sqlsrv_con_s;
extern char *sql_externo_filtro;

extern char *escapa_sql;

char *mi_sqltipo(int mitipo,int milen)
{
   static char eltipo[1024];
   switch(mitipo)
   {
   case INTTIPO:                          
      //strcpy(eltipo,"SMALLINT");
	  strcpy(eltipo,"Smallint");
      break;
   case LONGTIPO:
      //strcpy(eltipo,"INTEGER");
	  strcpy(eltipo,"Integer");
      break;
   case FLOATTIPO:
      //strcpy(eltipo,"REAL");
	  strcpy(eltipo,"Float");	   
      break;
   case DOUBLETIPO:
      //strcpy(eltipo,"DOUBLE");// PRECISION
      //strcpy(eltipo,"REAL");
	  strcpy(eltipo,"Float");	   
      break;    
   case FECHATIPO:
      strcpy(eltipo,"DATETIME");                          
      break;      
   default:                          
      //sprintf(eltipo,"CHAR(%d)",milen);
	  //sprintf(eltipo,"Varbinary(%d)",milen);	   
	  //sprintf(eltipo,"Varchar(%d)",milen);
	  sprintf(eltipo,"char(%d)",milen);	   
      break; 
   }
   return(eltipo);
}


char *mi_fichero(char *f)
{
   static char _mi_fichero[256];
   int i;

   if (f[1] == ':') /* ojo ignorar unidad ... */
   {
	   f += 2;
   }
   
   strcpy(_mi_fichero,f);
   for (i = 0;_mi_fichero[i];i++)
   {
      if (_mi_fichero[i] == '/' || _mi_fichero[i] == ':' || _mi_fichero[i] == '\\') 
         _mi_fichero[i] = '_';
   }
   return(_mi_fichero);
}

#ifdef ESDOS

extern char *sqlsrv_con_sql;
extern char *sqlsrv_conexion;

static char buffer[5000];
static HINSTANCE libreria = NULL;
static FARPROC aucall = NULL;


typedef struct fgv_conexion 
{
   char tabla[256];   
   int KeyCurr;
   int locked;
   int handle;   
   FI_DEF *pfi;
   int limpio;
} FGVCONEXION;

static FGVCONEXION *FgvConexion[IXXMAXFILES] = {NULL,};
static long recnums[IXXMAXFILES];


static int carga_sqllink()
{
	char *cc;
    libreria = LoadLibrary("dsqlink1.dll");
    if (!libreria)
    {
       return(-1);
    }
	aucall = GetProcAddress(libreria,"dll_dll_x");
    if (aucall)
	{
	   aucall((int)10,(int)12,(double)22.0);
	}
	aucall = GetProcAddress(libreria,"DsSqlLink");
    if (!aucall)
	{
	   FreeLibrary(libreria);
	   libreria = NULL;
	}	
	if (sqlsrv_conexion)
		cc = sqlsrv_conexion;
	else if (sqlsrv_con_sql)
	{
		cc = sqlsrv_con_sql;
	}
	if (cc)
	{
		if (*cc == '"')
		{
			int i;
			strcpy(buffer,cc+1);
			i = strlen(buffer) - 1;
			if (i >= 0)
			{
				if (buffer[i] == '"')
				{
					buffer[i] = 0;
				}
			}
		}
		else
		{
			strcpy(buffer,cc);
		}
	}
	else
	{
		strcpy(buffer,"");
	}
	while(!aucall((DWORD)0,(DWORD)0,(LPCSTR)buffer,(void *)NULL));	
	return 0;
}

static int descarga_sqllink()
{
    if (!libreria)
    {
       return(-1);
    }
	aucall = NULL;
    FreeLibrary(libreria);
    libreria = NULL;
	return 0;
}



static int fd_correcto(int fd)
{
   if (fd >= IXXMAXFILES) 
      fd -= IXXMAXFILES;
   else
      fd = -1;
   return(fd);
}

static void pon_el_valor(char *buffer,int k,int i,FI_DEF *pfi,char *modo,int *pars)
{
		 if (pars)
		 {
			 sprintf(buffer+strlen(buffer),"K%03d%s? ",pfi->f_clave[k].k_campo[i]-1,modo);
			 *pars = pfi->f_clave[k].k_campo[i];
		 }
		 else
		 {         
			 sprintf(buffer+strlen(buffer),"K%03d%s",pfi->f_clave[k].k_campo[i]-1,modo);
			 if (pfi->f_campos[ ( pfi->f_clave[k].k_campo[i]-1 ) ].c_tipo == CHARTIPO || pfi->f_campos[ ( pfi->f_clave[k].k_campo[i]-1 ) ].c_tipo == FECHATIPO)
			 {
				 strcat(buffer,"'");
				 ld_campo(pfi->f_clave[k].k_campo[i]-1,pfi,buffer+strlen(buffer));
				 strcat(buffer,"'");
			 }
			 else
			     ld_campo(pfi->f_clave[k].k_campo[i]-1,pfi,buffer+strlen(buffer));
		 }         
}

static char *_valor_clave(FI_DEF *pfi,char *modo,long reg,int trozo,int *pars,int plus)
{
   static char buffer[5000];
   int fd = fd_correcto(pfi->f_fd);
   int k = FgvConexion[fd]->KeyCurr;
   int i,j;
   int tc;
   int *ppars = pars;
   char modoplus[2];
   char *_modo;
   int ncam;

   modoplus[0] = modo[0];
   modoplus[1] = 0;
   if (pars)
   {
	   pars++;
	   *ppars = 0;
   }
   
   buffer[0] = 0;

   _modo = modo;
   
   if (k == pfi->f_nclaves)
   {      
	  if (plus)
		  _modo = modoplus;
	  if (pars)
	  {
		 sprintf(buffer," WHERE NUMERO_DE_CONTROL%s?",_modo);
	     *pars = 0;
	     pars++;
	     (*ppars)++;
	  }
	  else
	  {
		 sprintf(buffer," WHERE NUMERO_DE_CONTROL%s%ld",_modo,reg);
	  }
   }
   else
   {
	  ncam = pfi->f_clave[k].k_ncampos;
	  if (trozo)
		  ncam = trozo;
	  if (!strcmp(modo,"="))
	  {
		  tc = ncam;
		  plus = 0;
	  }
	  else
	  {
		  _modo = modoplus;
		  tc = 1;
	  }
	  /*
	  if (!k)
	  {
		  if (plus)
		  {
			  _modo = modoplus;
		  }
		  else
		  {
			  _modo = modo;
		  }
		  plus = 0;
	  }
	  else*/ if (ncam == 1)
	  {
		  if (!plus)
		  {
			  _modo = modo;
		  }
		  else if (!k)
			  plus = 0;

	  }
      for (i = 0;i < tc;i++)
      {
         /*if (trozo && i >= trozo) break;*/
         if (!i)
         {
            strcat(buffer," WHERE ");
         }
         else
         {
            strcat(buffer," AND ");
         }
		 pon_el_valor(buffer,k,i,pfi,_modo,pars);
		 if (pars)
		 {
			 pars++;
			 (*ppars)++;
		 }
      }
	  
	  while(i < (ncam+plus))
	  {		  
		  strcat(buffer," OR (");
		  for (j = 0;j < i;j++)
		  {
			 if (j)
				 strcat(buffer," AND ");
			 pon_el_valor(buffer,k,j,pfi,"=",pars);
			 if (pars)
			 {
				 pars++;
				 (*ppars)++;
			 }
		  }
		  strcat(buffer," AND ");
		  if (plus && (i+1) == (ncam+plus))
		  {
			  if (pars)
			  {
				 sprintf(buffer+strlen(buffer),"NUMERO_DE_CONTROL%s?",modoplus);
				 *pars = 0;
				 pars++;
				 (*ppars)++;
			  }
			  else
			  {
				 sprintf(buffer+strlen(buffer),"NUMERO_DE_CONTROL%s%ld",modoplus,reg);
			  }
		  }
		  else
		  {
			  if (!plus && (i+1) == ncam)
			  {
				  pon_el_valor(buffer,k,i,pfi,modo,pars);
			  }
			  else
			  {
				  pon_el_valor(buffer,k,i,pfi,_modo,pars);
			  }
			  if (pars)
			  {
				  pars++;
				  (*ppars)++;
			  }
		  }
		  strcat(buffer,") ");		  
		  i++;
	  }
   }     
   return(buffer);
}

static char *__orden_clave(FI_DEF *pfi,int trozo,int k,int reves)
{
   static char orden_clave[1024];
   int i;
   int t;
   char modir[30];
   
   orden_clave[0] = 0;
   if (k == pfi->f_nclaves)
   {
	  if (reves)
	  {
		  strcpy(orden_clave," ORDER BY 1 DESC");
	  }
	  else
	  {
		  strcpy(orden_clave," ORDER BY 1 ASC");
	  }
   }
   else
   {
      strcpy(orden_clave," ORDER BY ");
      if (trozo) t = trozo; else t = pfi->f_clave[k].k_ncampos;
      for (i = 0;i < t;i++)
      {
         if (i)
            strcat(orden_clave,", ");
         sprintf(orden_clave+strlen(orden_clave),"%d",pfi->f_clave[k].k_campo[i]+1);
		 if (reves)
		 {
			 if (pfi->f_clave[k].k_orden[i] < 0)
			 {
				 strcpy(modir,"ASC");
			 }
			 else
			 {
				 strcpy(modir,"DESC");
			 }
			 strcat(orden_clave," ");
			 strcat(orden_clave,modir);
		 }
      }
	  if (k)
	  {
		  strcat(orden_clave,",1 ");
		  if (reves)
		  {
			  strcat(orden_clave," DESC ");
		  }
	  }
	  else
	  {
		  strcat(orden_clave," ");
	  }
   }
   return(orden_clave);
}

static char *_orden_clave(FI_DEF *pfi,int trozo,int reves)
{
   int fd = fd_correcto(pfi->f_fd);
   int k = FgvConexion[fd]->KeyCurr;
   return __orden_clave(pfi,trozo,k,reves);
}

static int llena_lectura(FI_DEF *pfi,char *buffer,int modo,char *pb,int trozo,int *pars)
{   
   buffer[0] = 0;
   switch(modo)
   {
   case ISFIRST:
      sprintf(buffer,"SELECT * FROM %s%s",FgvConexion[fd_correcto(pfi->f_fd)]->tabla,_orden_clave(pfi,trozo,0));     
      break;
   case ISLAST:
      sprintf(buffer,"SELECT * FROM %s%s",FgvConexion[fd_correcto(pfi->f_fd)]->tabla,_orden_clave(pfi,trozo,1));      
      break;
   case ISNEXT:
	  sprintf(buffer,"SELECT * FROM %s%s%s",FgvConexion[fd_correcto(pfi->f_fd)]->tabla,_valor_clave(pfi,">=",0L,0,pars,1),_orden_clave(pfi,0,0));
      break;
   case ISPREV:
      sprintf(buffer,"SELECT * FROM %s%s%s",FgvConexion[fd_correcto(pfi->f_fd)]->tabla,_valor_clave(pfi,"<=",0L,0,pars,1),_orden_clave(pfi,0,1));
      break;
   case ISCURR:
	  if (pb[0])
	  {
		  sprintf(buffer,"SELECT * FROM %s WHERE NUMERO_DE_CONTROL=%ld %s%s",FgvConexion[fd_correcto(pfi->f_fd)]->tabla,recnums[fd_correcto(pfi->f_fd)],_orden_clave(pfi,0,0),pb);
	  }
	  else
		  buffer[0] = 0;	  
      break;
   case ISEQUAL:
      sprintf(buffer,"SELECT * FROM %s%s%s%s",FgvConexion[fd_correcto(pfi->f_fd)]->tabla,_valor_clave(pfi,"=",0L,trozo,pars,0),_orden_clave(pfi,trozo,0),pb);
      break;
   case ISGREAT:
      sprintf(buffer,"SELECT * FROM %s%s%s",FgvConexion[fd_correcto(pfi->f_fd)]->tabla,_valor_clave(pfi,">",0L,trozo,pars,0),_orden_clave(pfi,trozo,0));
      break;
   case ISGTEQ:
      sprintf(buffer,"SELECT * FROM %s%s%s",FgvConexion[fd_correcto(pfi->f_fd)]->tabla,_valor_clave(pfi,">=",0L,trozo,pars,0),_orden_clave(pfi,trozo,0));
      break;
   case ISLOW:
      sprintf(buffer,"SELECT * FROM %s%s%s",FgvConexion[fd_correcto(pfi->f_fd)]->tabla,_valor_clave(pfi,"<",0L,trozo,pars,0),_orden_clave(pfi,trozo,1));
      break;
   case ISLOWEQ:
      sprintf(buffer,"SELECT * FROM %s%s%s",FgvConexion[fd_correcto(pfi->f_fd)]->tabla,_valor_clave(pfi,"<=",0L,trozo,pars,0),_orden_clave(pfi,trozo,1));
      break;
   default:
      break;
   }
   
   return(0);
}


static int suma_char(int tipo)
{
   if (tipo == CHARTIPO || tipo == FECHATIPO)
      return(1);
   return(0);
}


int sqlsrv__cierra_ind(_fd)
register int _fd;
{
	int fd  = fd_correcto(_fd);	
	if (fd < 0 || !FgvConexion[fd]) return -1;
    aucall((DWORD)1,(DWORD)FgvConexion[fd]->handle,(LPCSTR)buffer,(void *)NULL);
	free(FgvConexion[fd]);
	FgvConexion[fd] = NULL;
    return(0);
}

int sqlsrv_cierra_ind(pfi)
register FI_DEF *pfi;
{
    return sqlsrv__cierra_ind(pfi->f_fd);
}

static int nonono_esprueba = 0;
int sqlsrv_crea_ind(pfi)
register FI_DEF *pfi;
{
	int i,kk;
	DWORD r;
	char tmp[512];

	if (!libreria)
		carga_sqllink();
    if (!libreria)
	   return -1;


	//sqlsrv_borra_ind(pfi); // para pruebas 

	// 
    sprintf(buffer,"CREATE TABLE %s (NUMERO_DE_CONTROL Integer PRIMARY KEY",mi_fichero(pfi->f_fichero));
	

	if (!pfi->f_orig_ncampos)
		pfi->f_orig_ncampos = pfi->f_ncampos;

    for (i = 0;i < pfi->f_orig_ncampos;i++)
    {   
        strcat(buffer,",");
        sprintf(tmp,"K%03d %s",i,mi_sqltipo(pfi->f_campos[i].c_tipo,pfi->f_campos[i].c_long));
        strcat(buffer,tmp);
    }
	
	/*

	for (kk = 0;kk < pfi->f_nclaves;kk++)
	{            
		if (!kk)
		   sprintf(buffer+strlen(buffer),",CONSTRAINT clave%d UNIQUE (",kk);
		else
		   sprintf(buffer+strlen(buffer),",CONSTRAINT clave%d (",kk);	   
		for (i = 0;i < pfi->f_clave[kk].k_ncampos;i++)
		{
		   if (i)
		   {
			  strcat(buffer,",");
		   }
		   sprintf(tmp,"K%03d",pfi->f_clave[kk].k_campo[i]-1);
		   if (pfi->f_clave[kk].k_orden[i] < 0)
			  strcat(tmp," DESC");
		   else
			  strcat(tmp," ASC"); 
		   strcat(buffer,tmp);
		}  
		strcat(buffer,")");	
	}
	*/
	strcat(buffer,")");


	r = aucall((DWORD)2,(DWORD)TRUE,(LPCSTR)buffer,(void *)NULL);
	if (r)
	{
		 int kk,j;

		 sprintf(buffer,"CREATE UNIQUE INDEX NUMERO_DE_CONTROLD ON %s (NUMERO_DE_CONTROL DESC)",mi_fichero(pfi->f_fichero));
		 r = aucall((DWORD)2,(DWORD)TRUE,(LPCSTR)buffer,(void *)NULL);

         for (kk = 0;kk < pfi->f_nclaves;kk++)
         {            
            if (!kk)
               sprintf(buffer,"CREATE UNIQUE INDEX clave%d ON %s (",kk,mi_fichero(pfi->f_fichero));
            else
               sprintf(buffer,"CREATE INDEX clave%d ON %s (",kk,mi_fichero(pfi->f_fichero));
            for (i = 0;i < pfi->f_clave[kk].k_ncampos;i++)
            {
			   for (j = 0;j < i;j++)
			   {
				   if (pfi->f_clave[kk].k_campo[i] == pfi->f_clave[kk].k_campo[j])
				   {
					   break;
				   }
			   }
			   if (j < i)
				   continue;
               if (i)
               {
                  strcat(buffer,",");
               }
               sprintf(tmp,"K%03d",pfi->f_clave[kk].k_campo[i]-1);
               if (pfi->f_clave[kk].k_orden[i] < 0)
                  strcat(tmp," DESC");
               else
                  strcat(tmp," ASC"); 
               strcat(buffer,tmp);
            }
			if (kk)
			{
				strcat(buffer,",NUMERO_DE_CONTROL ASC");
			}
            strcat(buffer,")");
			r = aucall((DWORD)2,(DWORD)TRUE,(LPCSTR)buffer,(void *)NULL);
			if (!r)
			{
				r = r;
			}
		 }

         for (kk = 0;kk < pfi->f_nclaves;kk++)
         {            
            if (!kk)
               sprintf(buffer,"CREATE UNIQUE INDEX claveD%d ON %s (",kk,mi_fichero(pfi->f_fichero));
            else
               sprintf(buffer,"CREATE INDEX claveD%d ON %s (",kk,mi_fichero(pfi->f_fichero));
            for (i = 0;i < pfi->f_clave[kk].k_ncampos;i++)
            {
			   for (j = 0;j < i;j++)
			   {
				   if (pfi->f_clave[kk].k_campo[i] == pfi->f_clave[kk].k_campo[j])
				   {
					   break;
				   }
			   }
			   if (j < i)
				   continue;
               if (i)
               {
                  strcat(buffer,",");
               }
               sprintf(tmp,"K%03d",pfi->f_clave[kk].k_campo[i]-1);
               if (pfi->f_clave[kk].k_orden[i] >= 0)
                  strcat(tmp," DESC");
               else
                  strcat(tmp," ASC"); 
               strcat(buffer,tmp);
            }
			if (kk)
			{
				strcat(buffer,",NUMERO_DE_CONTROL DESC");
			}
            strcat(buffer,")");
			r = aucall((DWORD)2,(DWORD)TRUE,(LPCSTR)buffer,(void *)NULL);
			if (!r)
			{
				r = r;
			}
		 }

		if (nonono_esprueba)
			return 0;
		return(sqlsrv_abre_ind(pfi,ISINOUT + ISMANULOCK));
	}
	return -1;
}

int sqlsrv_abre_ind(pfi,modo)
register FI_DEF *pfi;
int modo; 
{   
   DWORD r;
   struct abre_sql
   {
	   char *out_buffer;
	   int lfich;
	   int ncampos;
	   int posiciones[300];
	   int lon[300];
	   int tipo[300];	   
	   long *isrecnum;
	   char Tabla[150];
   } abre;
   int fd;
   int i;

   if (!libreria)
  	  carga_sqllink();

   if (!libreria)
	   return -1;
   
   /*
   sqlsrv_borra_ind(pfi);
   nonono_esprueba = 1;
   sqlsrv_crea_ind(pfi);
   nonono_esprueba = 0;
   */     
	if (!pfi->f_orig_ncampos)
		pfi->f_orig_ncampos = pfi->f_ncampos;

   abre.out_buffer = pfi->f_buf;
   abre.lfich = pfi->f_lfich;
   abre.ncampos = pfi->f_orig_ncampos;
   strcpy(abre.Tabla,mi_fichero(pfi->f_fichero));
   for (i = 0;i < pfi->f_orig_ncampos;i++)
   {
	   abre.posiciones[i] = pfi->f_campos[i].c_offset;
	   abre.lon[i] = pfi->f_campos[i].c_long;
	   abre.tipo[i] = pfi->f_campos[i].c_tipo;
   }
   for (i = 0;i < IXXMAXFILES;i++)
   {
	   if (!FgvConexion[i])
	   {
		   break;
	   }
   }
   abre.isrecnum = &(recnums[i]);

   sprintf(buffer,"SELECT * from %s %s",abre.Tabla,__orden_clave(pfi,0,0,0));
   r = aucall((DWORD)3,(DWORD)&abre,(LPCSTR)buffer,(void *)NULL);
   
   fd = (int)r;
   if (fd < 0)
   {
	   iserrno = ENOENT;
	   return(-1);
   }
   else
   {
		   if (!FgvConexion[i])
		   {
			   FgvConexion[i] = (FGVCONEXION *)malloc(sizeof(FGVCONEXION));
			   memset(FgvConexion[i],0,sizeof(FGVCONEXION));
			   strcpy(FgvConexion[i]->tabla,mi_fichero(pfi->f_fichero));
			   FgvConexion[i]->KeyCurr = 0;
			   FgvConexion[i]->locked = 0;
			   FgvConexion[i]->handle = fd;			   
			   FgvConexion[i]->pfi = pfi;
			   FgvConexion[i]->limpio = 1;
			   fd = i+IXXMAXFILES;
			   pfi->f_fd = fd;
			   if (!(isrecnum = sqlsrv_tam_btrv(pfi)))
			   {
				   char tmp[512];
				   int ff;
				   int l = pfi->f_lfich;
				   char c;
				   long conta = 0L;
				   sprintf(tmp,"%s.dat",pfi->f_fichero);
				   ff = rw_open_(tmp,O_RDONLY | O_BINARY);
				   if (ff >= 0)
				   {
					  while(rw_read_(ff,pfi->f_buf,l) == l)
					  {
						  c = 0;
						  rw_read_(ff,&c,1);
						  if (c)
						  {
					  	 	 sqlsrv_graba_nuevo(pfi);
						     conta++;
							 if (!(conta%57))
							 {
								sprintf(tmp,"grabados %ld",conta);
								mensaje(0,tmp);
							 }
						  }
					  }
					  rw_close_(ff);					  
				   }
				   /*
				   if (sqlsrv_tam_btrv(pfi) != conta)
				   {
					   char tmpx[512];
					   sprintf(tmpx,"%s errores %ld != %ld",quita_path(pfi->f_fichero),sqlsrv_tam_btrv(pfi),conta);
					   aviso(0,tmpx);
				   }
				   */
			   }
			   isrecnum = sqlsrv_tam_btrv(pfi);
			   if (isrecnum)
			   {				   
				   isrecnum = recnums[i];
			   }			
		   }	   
   }   

   return fd;
}


int sqlsrv_apunta_ind(pfi,clave,modo,trozo)
register FI_DEF *pfi;
int clave,modo,trozo;
{
   int fd  = fd_correcto(pfi->f_fd);	
   int pars[300];
   if (fd < 0 || !FgvConexion[fd]) return -1;

   modo &= ~ISLOCK;
   modo &= ~ISWAIT;
   
   if (clave != -1)
   {
      if (!clave) 
      {
         FgvConexion[fd]->KeyCurr = pfi->f_nclaves;
      }
      else
      {
         clave--;
         FgvConexion[fd]->KeyCurr = clave;
      }
   }
   
   /* ojo: comprobar si no se ha de asumir el cambio de clave */
   if (!(recnums[fd]) && (modo == ISCURR || modo == ISNEXT || modo == ISPREV))
   {
      iserrno = ENOCURR;
      return(-1);
   }
   
   pars[0] = 0;
   llena_lectura(pfi,buffer,modo,"",trozo,pars);  
   isrecnum = recnums[fd];
   if (modo != ISFIRST)
   {
	   FgvConexion[fd]->limpio = 0;
   }
   else
   {
	   FgvConexion[fd]->limpio = 1;
   }
   if (!((int)aucall((DWORD)4,(DWORD)FgvConexion[fd]->handle,(LPCSTR)buffer,(void *)pars)))
   {
      if (modo == ISFIRST) //caso de cambio de clave y no hay registros 
      {
         iserrno = 0;
         return(0);
      }
	    if (modo == ISPREV)
			iserrno = EENDFILE;
		else
	    if (modo == ISNEXT)
			iserrno = EENDFILE;
		else
	    if (modo == ISCURR)
			iserrno = ENOCURR;
		else
		    iserrno = 111;
	  return -1;
   }   
   isrecnum = recnums[fd];
   iserrno = 0;
   return(0);
}

int sqlsrv_cambia_ind(pfi,clave)
register FI_DEF *pfi;
int clave;
{
   return sqlsrv_apunta_ind(pfi,clave,ISFIRST,0);   
}


int sqlsrv_borra_actual(pfi)
register FI_DEF *pfi;
{
	int fd  = fd_correcto(pfi->f_fd);	
	if (fd < 0 || !FgvConexion[fd]) return -1;
	buffer[0] = 0;
	if (!((int)aucall((DWORD)5,(DWORD)FgvConexion[fd]->handle,(LPCSTR)buffer,(void *)NULL)))
	{
		iserrno = ENOCURR;
		return -1;
	}
    return(0);        
}

int sqlsrv_borra_reg(pfi)
register FI_DEF *pfi;
{
	int fd  = fd_correcto(pfi->f_fd);	
	if (fd < 0 || !FgvConexion[fd]) return -1;	
	sprintf(buffer,"DELETE %s %s",FgvConexion[fd]->tabla,_valor_clave(pfi,"=",recnums[fd],0,NULL,0));
	if (!((int)aucall((DWORD)6,(DWORD)FgvConexion[fd]->handle,(LPCSTR)buffer,(void *)NULL)))
	{
		iserrno = 111;
		return -1;
	}
    return(0);        
}

long sqlsrv_da_registo_actual(pfi)
FI_DEF *pfi;
{
   int fd  = fd_correcto(pfi->f_fd);	
   if (fd < 0 || !FgvConexion[fd]) return -1;
   return recnums[fd];
}

void sqlsrv_pon_registro_actual(pfi,r)
FI_DEF *pfi;
long r;
{
   int fd  = fd_correcto(pfi->f_fd);	
   if (fd < 0 || !FgvConexion[fd]) return;
   recnums[fd] = r;
}


int sqlsrv_ind_clave_activa(pfi)
FI_DEF *pfi;
{
   int fd  = fd_correcto(pfi->f_fd);	
   if (fd < 0 || !FgvConexion[fd]) return;	
   return FgvConexion[fd]->KeyCurr;
}


int sqlsrv_libera_ind(pfi)
register FI_DEF *pfi;
{
	int fd  = fd_correcto(pfi->f_fd);	
	if (fd < 0 || !FgvConexion[fd]) return -1;
	buffer[0] = 0;
	if (!((int)aucall((DWORD)10,(DWORD)FgvConexion[fd]->handle,(LPCSTR)buffer,(void *)NULL)))
	{
		iserrno = 111;
		return -1;
	}    
   return(0);
}

int sqlsrv_lee_reg(pfi,modo)
register FI_DEF *pfi;
int modo; /* modos compatibles C-ISAM */
{   
   int fd  = fd_correcto(pfi->f_fd);	
   int pars[300];
   char bloqueo[100];
   if (fd < 0 || !FgvConexion[fd]) return -1;

   bloqueo[0] = 0;
   if ((modo & ISLOCK) || (modo & ISWAIT))
   {
	   strcpy(bloqueo," FOR UPDATE ");
   }
   modo &= ~ISLOCK;
   modo &= ~ISWAIT;
   pars[0] = 0;
   if (modo != ISFIRST)
   {
	   FgvConexion[fd]->limpio = 0;
   }
   else
   {
	   FgvConexion[fd]->limpio = 1;
   }
   llena_lectura(pfi,buffer,modo,bloqueo,0,pars);
   if (!((int)aucall((DWORD)11,(DWORD)FgvConexion[fd]->handle,(LPCSTR)buffer,(void *)pars)))
   {
	    if (modo == ISPREV)
			iserrno = EENDFILE;
		else
	    if (modo == ISNEXT)
			iserrno = EENDFILE;
		else
	    if (modo == ISCURR)
			iserrno = ENOCURR;
		else
		    iserrno = 111;
		return -1;
   }   
   isrecnum = recnums[fd];
   iserrno = 0;
   return(0);
}

  
int sqlsrv_graba_nuevo(pfi)
register FI_DEF *pfi;
{
	int i;
	int fd  = fd_correcto(pfi->f_fd);	
	if (fd < 0 || !FgvConexion[fd]) return -1;

	buffer[0] = 0;
	if (!((int)aucall((DWORD)12,(DWORD)FgvConexion[fd]->handle,(LPCSTR)buffer,(void *)NULL)))
	{
		iserrno = 100;
		return -1;
	}
	iserrno = 0;
	return 0;
#ifdef PRUEBASS

	/*
	    sprintf(buffer,"INSERT INTO %s (NUMERO_DE_CONTROL) VALUES (1)",FgvConexion[fd]->tabla);
		if (!((int)aucall((DWORD)12,(DWORD)fd,(LPCSTR)buffer)))
		{
			iserrno = 100;
			return -1;
		}
		return 0;
	*/

		if (!pfi->f_orig_ncampos)
			pfi->f_orig_ncampos = pfi->f_ncampos;

        sprintf(buffer,"INSERT INTO %s ( NUMERO_DE_CONTROL",FgvConexion[fd]->tabla);
        for (i = 0;i < pfi->f_orig_ncampos;i++)
        {
           strcat(buffer," ,");
           sprintf(buffer+strlen(buffer),"K%03d",i);
        }   
		isrecnum++;
        //sprintf(buffer+strlen(buffer)," ) VALUES ( %ld ",isrecnum);
        sprintf(buffer+strlen(buffer)," ) VALUES ( ? ");
        
        for (i = 0;i < pfi->f_orig_ncampos;i++)
        {
           strcat(buffer,",? ");
		   /*
           strcat(buffer,",");
           if (pfi->f_campos[i].c_tipo == CHARTIPO)
           {
			   strcat(buffer,"'");
			   ld_campo(i,pfi,buffer+strlen(buffer));
			   strcat(buffer,"'");
           }
           else if (pfi->f_campos[i].c_tipo == FECHATIPO)
		   {			   
			   sprintf(buffer+strlen(buffer),"'%8.8s'",pfi->f_buf+pfi->f_campos[i].c_offset);
		   }
		   else
               ld_campo(i,pfi,buffer+strlen(buffer));      
			*/
           
        }   
        strcat(buffer,")");   

		if (!((int)aucall((DWORD)12,(DWORD)fd,(LPCSTR)buffer,(void *)NULL)))
		{
			iserrno = 100;
			return -1;
		}

		/*
		if (!((int)aucall((DWORD)2,(DWORD)0,(LPCSTR)buffer)))
		{
			iserrno = 100;
			return -1;
		}
		*/
		

   return(0);
#endif
}


int sqlsrv_graba_actual(pfi)
register FI_DEF *pfi;
{
	int fd  = fd_correcto(pfi->f_fd);	
	if (fd < 0 || !FgvConexion[fd]) return -1;
	buffer[0] = 0;
	if (!((int)aucall((DWORD)13,(DWORD)FgvConexion[fd]->handle,(LPCSTR)buffer,(void *)NULL)))
	{
		iserrno = ENOCURR;
		return -1;
	}
    return(0);        
}


int sqlsrv_cierra_todo(como)
int como; 
{  
   descarga_sqllink();
   return(como);
}


int sqlsrv_lee_soloindice(pfi,modo)
register FI_DEF *pfi;
int modo;
{
   int fd  = fd_correcto(pfi->f_fd);	
   int pars[300];
   if (fd < 0 || !FgvConexion[fd]) return -1;
   pars[0] = 0;
   llena_lectura(pfi,buffer,modo,"",0,pars);  
   isrecnum = recnums[fd];
   if (!((int)aucall((DWORD)4,(DWORD)FgvConexion[fd]->handle,(LPCSTR)buffer,(void *)pars)))
   {
	    if (modo == ISPREV)
			iserrno = EENDFILE;
		else
	    if (modo == ISNEXT)
			iserrno = EENDFILE;
		else
	    if (modo == ISCURR)
			iserrno = ENOCURR;
		else
		    iserrno = 111;
	  return -1;
   }
   isrecnum = recnums[fd];
   iserrno = 0;
   return(0);
}


int sqlsrv_abre_directo(pfi,nom)
FI_DEF *pfi;
char *nom;
{
	int ofd = pfi->f_fd;
	int fd;
    fd = sqlsrv_abre_ind(pfi,ISINOUT + ISMANULOCK);
	if (fd >= 0)
	{
		fd = pfi->f_fd;
		sqlsrv_apunta_ind(pfi,0,ISCURR,0);
	}
	pfi->f_fd = ofd;
	fd -= IXXMAXFILES;
	return fd;
}

int sqlsrv_cierra_directo(fd)
int fd;
{	
	if (fd >= 0)
		fd += IXXMAXFILES;
	return sqlsrv__cierra_ind(fd);
}

int sqlsrv_lee_directo(fd,pos,pfi,buf)
int fd;
long pos;
FI_DEF *pfi;
char *buf;
{
	int r = -1;
	int ofd = pfi->f_fd;
	int hfd;
	if (buf != pfi->f_buf)
	{
		/* solo para este caso!!! */
		iserrno = 111;
		return -1;
	}
	if (fd >= 0)
		fd += IXXMAXFILES;
	pfi->f_fd = fd;
	hfd = fd_correcto(fd);
	if (hfd >= 0 && FgvConexion[hfd])
	{
		recnums[hfd] = pos;   
		r = sqlsrv_lee_reg(pfi,ISEQUAL);    
	}
	pfi->f_fd = ofd;
	return r;
}

int sqlsrv_estalimpio(int _fd)
{
   int fd  = fd_correcto(_fd);	   
   if (fd < 0 || !FgvConexion[fd]) return -1;
   return FgvConexion[fd]->limpio;
}

int sqlsrv_lee_por_posicion(int _fd,long posicion,unsigned char *dat_reg,int modo,int flags)
{
   int fd  = fd_correcto(_fd);	   
   if (fd < 0 || !FgvConexion[fd]) return -1;
   if (!((int)aucall((DWORD)29,(DWORD)FgvConexion[fd]->handle,(LPCSTR)dat_reg,(void *)posicion)))
   {
	  iserrno = 111;
	  return -1;
   }
   isrecnum = recnums[fd];
   iserrno = 0;
   return 0;
}

int sqlsrv_lee_secuencia(fd,po,md,tamr,buf)
int fd;
long *po;
int *md;
int tamr;
char *buf;
{   
   iserrno = 111; /* no es adecuado este modo de lectura .... no se debe usar */
   return -1;
}


/* !!! */
long sqlsrv_tam_btrv(pfi)
FI_DEF *pfi;
{	
	int fd = fd_correcto(pfi->f_fd);
	return (long)aucall((DWORD)20,(DWORD)FgvConexion[fd]->handle,(LPCSTR)FgvConexion[fd]->tabla,(void *)NULL);
}

sqlsrv_flush_ind(pfi)
register FI_DEF *pfi;
{
   return(0);
}

/*
* lock_ind
* bloquea todo el indexado
* el fichero debe abrirse con bloqueo manual
* se libera el bloqueo con unlock_ind
* devuelve 0 si va bien, -1 si hay error
*/
sqlsrv_lock_ind(pfi)
register FI_DEF *pfi;
{
   return(0);
}

/*
* unlock_ind
* desbloquea todo el indexado
* devuelve 0 si va bien, -1 si hay error
*/
sqlsrv_unlock_ind(pfi)
register FI_DEF *pfi;
{
   return(0);
}


int sqlsrv_borra_ind(pfi)
register FI_DEF *pfi;
{
	if (!libreria)
		carga_sqllink();
    if (!libreria)
	   return -1;


    sprintf(buffer,"DROP TABLE %s",mi_fichero(pfi->f_fichero));	
	aucall((DWORD)2,(DWORD)FALSE,(LPCSTR)buffer,(void *)NULL);
   return(0);
}


int sqlsrv_empieza_trans()
{
   /* no soportado de momento */
	return 0;
}

int sqlsrv_anula_trans()
{
   /* no soportado de momento */
	return 0;
}

int sqlsrv_acaba_trans()
{
   /* no soportado de momento */
	return 0;
}

int sql_resultado_en_bloque(FI_DEF *pfi,int prog,char *tabla,char *where,char *order)
{
	char *buf = malloc(1024 + strlen(where) + strlen(order));

	sprintf(buf,"SELECT NUMERO_DE_CONTROL FROM %s %s %s",mi_fichero(pfi->f_fichero),where,order);

	aucall((DWORD)28,(DWORD)0,(LPCSTR)buf,(void *)tabla);

	free(buf);
	return 0;
}

#else /* ESDOS */


int sqlsrv__cierra_ind(fd)
register int fd;
{	

    return(0);
}

int sqlsrv_cierra_ind(pfi)
register FI_DEF *pfi;
{
  return 0;
}

int sqlsrv_crea_ind(pfi)
register FI_DEF *pfi;
{
  return 0;
}

int sqlsrv_abre_ind(pfi,modo)
register FI_DEF *pfi;
int modo; 
{   
   return(pfi->f_fd);
}


int sqlsrv_apunta_ind(pfi,clave,modo,trozo)
register FI_DEF *pfi;
int clave,modo,trozo;
{
   return(0);
}

int sqlsrv_cambia_ind(pfi,clave)
register FI_DEF *pfi;
int clave;
{
   return 0;
}


int sqlsrv_borra_actual(pfi)
register FI_DEF *pfi;
{
   return(0);
         
}

int sqlsrv_borra_reg(pfi)
register FI_DEF *pfi;
{
   iserrno = 0;
   return(0);
}

long sqlsrv_da_registo_actual(pfi)
FI_DEF *pfi;
{
	return 0;
}

void sqlsrv_pon_registro_actual(pfi,r)
FI_DEF *pfi;
long r;
{
}


int sqlsrv_ind_clave_activa(pfi)
FI_DEF *pfi;
{
	return 0;
}


int sqlsrv_libera_ind(pfi)
register FI_DEF *pfi;
{
   return(0);
}

int sqlsrv_lee_reg(pfi,modo)
register FI_DEF *pfi;
int modo; /* modos compatibles C-ISAM */
{
   return(0);
}


  
int sqlsrv_graba_nuevo(pfi)
register FI_DEF *pfi;
{

   return(0);
}


int sqlsrv_graba_actual(pfi)
register FI_DEF *pfi;
{
   return(0);
}


int sqlsrv_cierra_todo(como)
int como; 
{  
   return(como);
}



int sqlsrv_lee_soloindice(pfi,modo)
register FI_DEF *pfi;
int modo;
{
	return 0;
}



int sqlsrv_abre_directo(pfi,nom)
FI_DEF *pfi;
char *nom;
{
	return 0;
}

int sqlsrv_cierra_directo(fd)
int fd;
{
	return 0;
}

int sqlsrv_lee_directo(fd,pos,pfi,buf)
int fd;
long pos;
FI_DEF *pfi;
char *buf;
{
   return(0);   
}

int sqlsrv_lee_secuencia(fd,po,md,tamr,buf)
int fd;
long *po;
int *md;
int tamr;
char *buf;
{
   
   return(0);
}


/* !!! */
long sqlsrv_tam_btrv(pfi)
FI_DEF *pfi;
{
	return 0;
}

sqlsrv_flush_ind(pfi)
register FI_DEF *pfi;
{
   return(0);
}

/*
* lock_ind
* bloquea todo el indexado
* el fichero debe abrirse con bloqueo manual
* se libera el bloqueo con unlock_ind
* devuelve 0 si va bien, -1 si hay error
*/
sqlsrv_lock_ind(pfi)
register FI_DEF *pfi;
{
   return(0);
}

/*
* unlock_ind
* desbloquea todo el indexado
* devuelve 0 si va bien, -1 si hay error
*/
sqlsrv_unlock_ind(pfi)
register FI_DEF *pfi;
{
   return(0);
}


sqlsrv_borra_ind(pfi)
register FI_DEF *pfi;
{
   return(0);
}


int sqlsrv_empieza_trans()
{
   /* no soportado de momento */
	return 0;
}

int sqlsrv_anula_trans()
{
   /* no soportado de momento */
	return 0;
}

int sqlsrv_acaba_trans()
{
   /* no soportado de momento */
	return 0;
}

#endif /* ESDOS */

static int abre_count = 0;
static int reenviadefs = 0;

static char *respaldo_buffer = NULL;
static int  respaldo_tam = 0;

char **sql_lista_filtro = NULL;

static int filtro_fds[IXXMAXFILES];
static char reenvia_fds[IXXMAXFILES];

int respaldo_carga_filtro()
{
	memset(filtro_fds,0xff,sizeof(filtro_fds));
	memset(reenvia_fds,0xff,sizeof(reenvia_fds));
	return carga_filtro_x(sql_externo_filtro,&sql_lista_filtro);
}
		
int respaldo_pasa_filtro(char *nom,int fd)
{
    int ok = 0;
	int i,j;
    /* comprobar si es adecuado para incluir */ 
	if (!sql_externo_filtro)
	{
		ok = 1;
	}
	else
	{
		for (i = 0;sql_lista_filtro && sql_lista_filtro[i];i++)
		{
			if (!strcmp(sql_lista_filtro[i],"*"))
			{
				ok = 1;
				break;
			}
			else
			if (strstr(nom,sql_lista_filtro[i]))
			{
				ok = 1;
				break;
			}
		}
	}
	if (ok)
	{
		for (i = 0;i < IXXMAXFILES;i++)
		{
			if (filtro_fds[i] == -1)
			{
				filtro_fds[i] = fd;
				reenvia_fds[i] = 0xff;
				ok = i + 1;
				break;
			}
		}
	}
	else
	{
		for (i = 0;i < IXXMAXFILES;i++)
		{
			if (filtro_fds[i] == fd)
			{
				for (j = i;j < (IXXMAXFILES-1);j++)
				{
					filtro_fds[j] = filtro_fds[j+1];
					reenvia_fds[j] = reenvia_fds[j+1];
				}
				filtro_fds[IXXMAXFILES-1] = -1;
				reenvia_fds[IXXMAXFILES-1] = 0xff;
				break;
			}
		}
	}
	return ok;
}

static int respaldo_pasa_fd(int fd)
{
	int i;
	for (i = 0;i < IXXMAXFILES;i++)
	{
		if (filtro_fds[i] == fd)
		{
			return 1+i;
		}
		if (filtro_fds[i] == -1)
			break;
	}
	return 0;
}

char *da_memoria_respaldo(int memoria)
{
  if (!respaldo_buffer)
  {
      respaldo_buffer = malloc(memoria);
      respaldo_tam = memoria;
  }
  else
  {
      if (respaldo_tam < memoria)
      {
          free(respaldo_buffer);
          respaldo_buffer = malloc(memoria);
          respaldo_tam = memoria;
      }
  }
  return respaldo_buffer;
}

static char *empaqueta_def(FI_DEF *fi)
{
int memoria;
int i;
char *p,*buf;



	if (!fi->f_orig_ncampos)
		fi->f_orig_ncampos = fi->f_ncampos;

  memoria = sizeof(FI_DEF_LEE) + sizeof(CAMPO_DEF) * fi->f_orig_ncampos + sizeof(KEY_DEF) * fi->f_nclaves;


  for (i=0;i<fi->f_nclaves;i++) 
  {
      memoria += (fi->f_clave[i].k_ncampos * 2 * sizeof(short));          
  }
  for (i = 0;i < fi->f_orig_ncampos;i++)
  { 
      memoria += (strlen(fi->f_campos[i].c_lista)+1);
  }
  
  memoria += (sizeof(short) + sizeof(int)*2);
  
  buf = da_memoria_respaldo(memoria);

  p = buf;
  *((int *)p) = memoria;
  p += sizeof(int);
  *((short *)p) = FGVGEN_ID_MAS;
  p += sizeof(short);

  *((FI_DEF_LEE *)p) = *((FI_DEF_LEE *)fi);
  p += sizeof(FI_DEF_LEE);

  memcpy(p,fi->f_campos,sizeof(CAMPO_DEF) * fi->f_orig_ncampos);
  p += (sizeof(CAMPO_DEF) * fi->f_orig_ncampos);

  memcpy(p,fi->f_clave,sizeof(KEY_DEF) * fi->f_nclaves);
  p += (sizeof(KEY_DEF) * fi->f_nclaves);

  for (i=0;i<fi->f_nclaves;i++) 
  {
      memcpy(p,fi->f_clave[i].k_campo,sizeof(short) * fi->f_clave[i].k_ncampos);
      p += (sizeof(short) * fi->f_clave[i].k_ncampos);
      memcpy(p,fi->f_clave[i].k_orden,sizeof(short) * fi->f_clave[i].k_ncampos);
      p += (sizeof(short) * fi->f_clave[i].k_ncampos);
  }

  for (i = 0;i < fi->f_orig_ncampos;i++)
  {
      strcpy(p,fi->f_campos[i].c_lista);
      p += (strlen(fi->f_campos[i].c_lista)+1);
  }

  return buf;
}

int respaldo_cierra_ind(int fd)
{   
    char buffer[512];
	int filtro,j;

	if ((filtro = respaldo_pasa_fd(fd)))
	{
		for (j = filtro-1;j < (IXXMAXFILES-1);j++)
		{
			filtro_fds[j] = filtro_fds[j+1];
			reenvia_fds[j] = reenvia_fds[j+1];
		}
		filtro_fds[IXXMAXFILES-1] = -1;		
		reenvia_fds[IXXMAXFILES-1] = 0xff;

		if (!esta_activo_el_respaldo_remoto())
		{
			activa_respaldo_remoto(sqlsrv_con_s);
		}
		*((int *)buffer) = sizeof(int)*2+sizeof(short);
		*((short *)(buffer+sizeof(int))) = 1;
		*((int *)(buffer+sizeof(int)+sizeof(short))) = fd;
		envia_respaldo_remoto(buffer);
		
		abre_count--;
		if (abre_count < 0)
			abre_count = 0;
		if (!abre_count)
		{
			desactiva_respaldo_remoto();
			if (respaldo_buffer)
			{
				free(respaldo_buffer);
				respaldo_buffer = NULL;
				respaldo_tam = 0;
			}
		}		
	}
    return 0;
}

int respaldo_borra_ind(FI_DEF *pfi)
{   
    char buffer[512];
	int desactivar = 0;
	int i,j;

	if (sql_externo_filtro)
	{		

		if (!sql_lista_filtro)
		{
			respaldo_carga_filtro();
		}
		if (!respaldo_pasa_filtro(pfi->f_fichero,-1))
		{
			return 0;
		}
	}
	for (i = 0;i < IXXMAXFILES;i++)
	{
		if (filtro_fds[i] == pfi->f_fd)
		{
			for (j = i;j < (IXXMAXFILES-1);j++)
			{
				filtro_fds[j] = filtro_fds[j+1];
				reenvia_fds[j] = reenvia_fds[j+1];
			}
			filtro_fds[IXXMAXFILES-1] = -1;
			reenvia_fds[IXXMAXFILES-1] = 0xff;
			break;
		}
	}

    if (!esta_activo_el_respaldo_remoto())
    {
        activa_respaldo_remoto(sqlsrv_con_s);
		desactivar = 1;
    }

    *((int *)buffer) = sizeof(int)+sizeof(short)+60;
    *((short *)(buffer+sizeof(int))) = 2;
    strcpy(buffer+sizeof(int)+sizeof(short),pfi->f_fichero);
    envia_respaldo_remoto(buffer);

    if (desactivar)
    {
        desactiva_respaldo_remoto();
    }
    return 0;
}

int respaldo_abre_ind(FI_DEF *pfi)
{ 
    char *p;
	int filtro;
	
	if (sql_externo_filtro)
	{
		if (!sql_lista_filtro)
		{
			respaldo_carga_filtro();
		}
	}
	if (!(filtro = respaldo_pasa_filtro(pfi->f_fichero,pfi->f_fd)))
	{
		return 0;
	}
	filtro--;

    if (!esta_activo_el_respaldo_remoto())
    {        
        activa_respaldo_remoto(sqlsrv_con_s);
    }

	p = empaqueta_def(pfi);

    abre_count++;
	do
	{
		if (envia_respaldo_remoto(p) < 0)
		{
			activa_respaldo_remoto(sqlsrv_con_s);
			if (envia_respaldo_remoto(p) < 0)
			{
				if (con_respaldo_externo != 2)
				{
					break;
				}
				sleep(1);
			}
			else
			{
				reenvia_fds[filtro] = 0;
				break;
			}			
		}
		else
		{
			reenvia_fds[filtro] = 0;
			break;
		}
	} while(-1);

    return 0;
}

int respaldo_graba_nuevo(FI_DEF *pfi)
{
	int filtro;
	int memoria;
	char *buffer;
	char *p;

	if ((filtro = respaldo_pasa_fd(pfi->f_fd)))
	{
		filtro--;
		if (!esta_activo_el_respaldo_remoto())
		{
			activa_respaldo_remoto(sqlsrv_con_s);
		}
		do
		{		
			if (reenvia_fds[filtro])
			{
				p = empaqueta_def(pfi);				
				if (envia_respaldo_remoto(p) < 0)
				{
					if (con_respaldo_externo != 2)
					{
						break;
					}
					activa_respaldo_remoto(sqlsrv_con_s);
					sleep(1);
					continue;
				}				
				reenvia_fds[filtro] = 0;
			}
		    memoria = sizeof(int)*3+sizeof(short)+pfi->f_lfich;
		    buffer = da_memoria_respaldo(memoria);

			*((int *)buffer) = memoria;
			*((short *)(buffer+sizeof(int))) = 3;
			*((int *)(buffer+sizeof(int)+sizeof(short))) = pfi->f_fd;
			*((int *)(buffer+sizeof(int)*2+sizeof(short))) = isrecnum;
			memcpy(buffer+sizeof(int)*3+sizeof(short),pfi->f_buf,pfi->f_lfich);

			if (envia_respaldo_remoto(buffer) < 0)
			{				
				activa_respaldo_remoto(sqlsrv_con_s);
				memset(reenvia_fds,0xff,sizeof(filtro_fds));
			}
			else
				break;
		} while(-1);
	}
    
    return 0;
}

int respaldo_graba_actual(FI_DEF *pfi)
{
	int filtro;
	int memoria;
	char *buffer;
	char *p;

	if ((filtro = respaldo_pasa_fd(pfi->f_fd)))
	{		
		filtro--;
		if (!esta_activo_el_respaldo_remoto())
		{
			activa_respaldo_remoto(sqlsrv_con_s);
		}
		do
		{		
			if (reenvia_fds[filtro])
			{
				p = empaqueta_def(pfi);				
				if (envia_respaldo_remoto(p) < 0)
				{
					if (con_respaldo_externo != 2)
					{
						break;
					}
					activa_respaldo_remoto(sqlsrv_con_s);
					sleep(1);
					continue;
				}				
				reenvia_fds[filtro] = 0;
			}
		    memoria = sizeof(int)*3+sizeof(short)+pfi->f_lfich;
		    buffer = da_memoria_respaldo(memoria);

			*((int *)buffer) = memoria;
			*((short *)(buffer+sizeof(int))) = 4;
			*((int *)(buffer+sizeof(int)+sizeof(short))) = pfi->f_fd;
			*((int *)(buffer+sizeof(int)*2+sizeof(short))) = isrecnum;
			memcpy(buffer+sizeof(int)*3+sizeof(short),pfi->f_buf,pfi->f_lfich);

			if (envia_respaldo_remoto(buffer) < 0)
			{				
				activa_respaldo_remoto(sqlsrv_con_s);
				memset(reenvia_fds,0xff,sizeof(filtro_fds));
			}
			else
				break;
		} while(-1);
	}
    
    return 0;
}

int respaldo_borra_actual(FI_DEF *pfi)
{
	int filtro;
	int memoria;
	char *buffer;
	char *p;

	if ((filtro = respaldo_pasa_fd(pfi->f_fd)))
	{		
		filtro--;
		if (!esta_activo_el_respaldo_remoto())
		{
			activa_respaldo_remoto(sqlsrv_con_s);
		}
		do
		{		
			if (reenvia_fds[filtro])
			{
				p = empaqueta_def(pfi);				
				if (envia_respaldo_remoto(p) < 0)
				{
					if (con_respaldo_externo != 2)
					{
						break;
					}
					activa_respaldo_remoto(sqlsrv_con_s);
					sleep(1);
					continue;
				}				
				reenvia_fds[filtro] = 0;
			}
		    memoria = sizeof(int)*3+sizeof(short)+pfi->f_lfich;
		    buffer = da_memoria_respaldo(memoria);

			*((int *)buffer) = memoria;
			*((short *)(buffer+sizeof(int))) = 5;
			*((int *)(buffer+sizeof(int)+sizeof(short))) = pfi->f_fd;
			*((int *)(buffer+sizeof(int)*2+sizeof(short))) = isrecnum;
			memcpy(buffer+sizeof(int)*3+sizeof(short),pfi->f_buf,pfi->f_lfich);

			if (envia_respaldo_remoto(buffer) < 0)
			{				
				activa_respaldo_remoto(sqlsrv_con_s);
				memset(reenvia_fds,0xff,sizeof(filtro_fds));
			}
			else
				break;
		} while(-1);
	}
    
    return 0;
}

int respaldo_borra_reg(FI_DEF *pfi)
{
	int filtro;
	int memoria;
	char *buffer;
	char *p;

	if ((filtro = respaldo_pasa_fd(pfi->f_fd)))
	{		
		filtro--;
		if (!esta_activo_el_respaldo_remoto())
		{
			activa_respaldo_remoto(sqlsrv_con_s);
		}
		do
		{		
			if (reenvia_fds[filtro])
			{
				p = empaqueta_def(pfi);				
				if (envia_respaldo_remoto(p) < 0)
				{
					if (con_respaldo_externo != 2)
					{
						break;
					}
					activa_respaldo_remoto(sqlsrv_con_s);
					sleep(1);
					continue;
				}				
				reenvia_fds[filtro] = 0;
			}

		    memoria = sizeof(int)*3+sizeof(short)+pfi->f_lfich;
			buffer = da_memoria_respaldo(memoria);

			*((int *)buffer) = memoria;
			*((short *)(buffer+sizeof(int))) = 6;
			*((int *)(buffer+sizeof(int)+sizeof(short))) = pfi->f_fd;
			*((int *)(buffer+sizeof(int)*2+sizeof(short))) = isrecnum;
			memcpy(buffer+sizeof(int)*3+sizeof(short),pfi->f_buf,pfi->f_lfich);

			if (envia_respaldo_remoto(buffer) < 0)
			{				
				activa_respaldo_remoto(sqlsrv_con_s);
				memset(reenvia_fds,0xff,sizeof(filtro_fds));
			}
			else
				break;
		} while(-1);
	}

    return 0;
}


FILE *sql_txt_open_output()
{
	FILE *f;
	char buffer[1024];

	if (!bbdd_tiempo_rotate)
		return NULL;

	if (bbdd_file_path)
		sprintf(buffer,"%s/%08d.%08d.sql",bbdd_file_path,time(NULL)/bbdd_tiempo_rotate,getpid());
	else
		sprintf(buffer,"%s%08d.%08d.sql",rw_directorio("sql_txt/"),time(NULL)/bbdd_tiempo_rotate,getpid());
	crea_dir(buffer);
	f = fopen(buffer,"a");
	return f;
}

unsigned long sql_txt_id()
{
#ifndef WIN32
	struct timespec tp;
    clock_gettime(CLOCK_REALTIME, &tp);
	return tp.tv_sec+(tp.tv_nsec/1000000);
#else
    return GetTickCount();
#endif
}

int sql_txt_respaldo_crea(FI_DEF *pfi)
{
	if (sql_externo_filtro)
	{		

		if (!sql_lista_filtro)
		{
			respaldo_carga_filtro();
		}
		if (!respaldo_pasa_filtro(pfi->f_fichero,-1))
		{
			return 0;
		}
	}

	if (!pfi->f_orig_ncampos)
		pfi->f_orig_ncampos = pfi->f_ncampos;

	{
		FILE *f = sql_txt_open_output();
		if (f)
		{
			int i,kk,j;

			fprintf(f,"%ld.%ld CREATE TABLE %s (",time(NULL),sql_txt_id(),mi_fichero(pfi->f_fichero));
			for (i = 0;i < pfi->f_orig_ncampos;i++)
			{   
				if (i)
				   fprintf(f,",");
				fprintf(f,"C%03d %s",i,mi_sqltipo(pfi->f_campos[i].c_tipo,pfi->f_campos[i].c_long));			
			}	
			fprintf(f,")\r\n");

            fprintf(f,"%ld.%ld CREATE UNIQUE INDEX clave ON %s (",time(NULL),sql_txt_id(),mi_fichero(pfi->f_fichero));

			kk = 0;
            for (i = 0;i < pfi->f_clave[kk].k_ncampos;i++)
            {
			   for (j = 0;j < i;j++)
			   {
				   if (pfi->f_clave[kk].k_campo[i] == pfi->f_clave[kk].k_campo[j])
				   {
					   break;
				   }
			   }
			   if (j < i)
				   continue;
               if (i)
               {
                  fprintf(f,",");
               }
               fprintf(f,"C%03d",pfi->f_clave[kk].k_campo[i]-1);
               if (pfi->f_clave[kk].k_orden[i] < 0)
                  fprintf(f," DESC");
               else
                  fprintf(f," ASC");                
            }
            fprintf(f,")\r\n");

			fclose(f);
		}
	}
	return 0;
}

static int __esescape(char car)
{
	int i;
	if (car == '\\' || car == '\'')
		return 1;
	if (escapa_sql)
	{
		for (i = 0;escapa_sql[i];i++)
		{
			if (escapa_sql[i] == car)
				return 1;
		}
	}
	return 0;
}

static int escapa_buffer(unsigned char *buffer)
{
   int j;
   for (j = 0;buffer[j];j++)
   {	   
	   if (__esescape(buffer[j]))
	   {
		   strcatd(buffer+j,"\\");
		   j++;
	   }
	   else if (buffer[j] < 32)
	   {
		   buffer[j] = 32;
	   }
	   else
		   oem_a_ansi(buffer+j);
   }
   return j;
}

static int __compon_fecha(FILE *f,char *buffer)
{
	int esnula;
	char valor[32];
	int entero;
	 
	esnula = 0;

	memcpy(valor,buffer,4);valor[4] = 0;entero = atoi(valor);
	if (entero <= 1900 || entero >= 3000)
	    esnula = 1;	  

	memcpy(valor,buffer+4,2);valor[2] = 0;entero = atoi(valor);
	if (entero < 1 || entero > 12)
		esnula = 1;

	memcpy(valor,buffer+6,2);valor[2] = 0;entero = atoi(valor);
	if (entero < 1 || entero > 31)
		esnula = 1;

	if (esnula)
	    fprintf(f,"NULL");
	else
	{
	    fprintf(f,"'%-4.4s-",buffer);						 
	    fprintf(f,"%-2.2s-",buffer+4);
	    fprintf(f,"%-2.2s ",buffer+6);
	    fprintf(f,"00:00:00'");
	}
	return 0;
}

int sql_txt_respaldo_nuevo(FI_DEF *pfi)
{
	if (sql_externo_filtro)
	{
		if (!sql_lista_filtro)
		{
			respaldo_carga_filtro();
		}
		if (!respaldo_pasa_filtro(pfi->f_fichero,-1))
		{
			return 0;
		}
	}

	if (!pfi->f_orig_ncampos)
		pfi->f_orig_ncampos = pfi->f_ncampos;

	{
		FILE *f = sql_txt_open_output();
		if (f)
		{
			int i;
			int j;
			char buffer[1024];

			fprintf(f,"%ld.%ld INSERT INTO %s (",time(NULL),sql_txt_id(),mi_fichero(pfi->f_fichero));
			for (i = 0;i < pfi->f_orig_ncampos;i++)
			{
			   if (i)
				 fprintf(f,",");
			   fprintf(f,"C%03d",i);
			}        
			fprintf(f," ) VALUES ( ");
			for (i = 0;i < pfi->f_orig_ncampos;i++)
			{
			   if (i)
				  fprintf(f,",");
			   if (pfi->f_campos[i].c_tipo == CHARTIPO)
			   {				   
				   ld_campo(i,pfi,buffer);
				   j = escapa_buffer(buffer);
				   fprintf(f,"'%s'",buffer);
			   }
			   else if (pfi->f_campos[i].c_tipo == FECHATIPO)
			   {
				   char *pbuf;
				   pbuf = pfi->f_buf+pfi->f_campos[i].c_offset;		 
				   memcpy(buffer,pbuf,8);
				   buffer[8] = 0;
				   j = escapa_buffer(buffer);
				   __compon_fecha(f,buffer);
			   }
			   else
			   {
				   ld_campo(i,pfi,buffer);
				   j = escapa_buffer(buffer);
				   fprintf(f,"%s",buffer);
			   }
			}   
			fprintf(f," )\r\n");
			fclose(f);
		}
	}
	return 0;
}

int sql_txt_respaldo_actualiza(FI_DEF *pfi)
{
	if (sql_externo_filtro)
	{		

		if (!sql_lista_filtro)
		{
			respaldo_carga_filtro();
		}
		if (!respaldo_pasa_filtro(pfi->f_fichero,-1))
		{
			return 0;
		}
	}
	if (!pfi->f_orig_ncampos)
		pfi->f_orig_ncampos = pfi->f_ncampos;

	{
		FILE *f = sql_txt_open_output();
		if (f)
		{
			int i,k;
			int j;
			char buffer[512];

			fprintf(f,"%ld.%ld UPDATE %s SET ",time(NULL),sql_txt_id(),mi_fichero(pfi->f_fichero));
			for (i = 0;i < pfi->f_orig_ncampos;i++)
			{
			   if (i)
				  fprintf(f,",");
			   fprintf(f,"C%03d=",i);
			   if (pfi->f_campos[i].c_tipo == CHARTIPO)
			   {				   
				   fprintf(f,"'");
				   ld_campo(i,pfi,buffer);
				   j = escapa_buffer(buffer);
				   fprintf(f,"%s'",buffer);
			   }
			   else if (pfi->f_campos[i].c_tipo == FECHATIPO)
			   {			   
				   char *pbuf;
				   pbuf = pfi->f_buf+pfi->f_campos[i].c_offset;
				   memcpy(buffer,pbuf,8);
				   buffer[8] = 0;
				   j = escapa_buffer(buffer);
				   __compon_fecha(f,buffer);
				   /*fprintf(f,"'%8.8s'",pfi->f_buf+pfi->f_campos[i].c_offset);*/
			   }
			   else
			   {
				   ld_campo(i,pfi,buffer); 
				   j = escapa_buffer(buffer);
				   fprintf(f,"%s",buffer);
			   }
			}   

			fprintf(f," WHERE ");

			for (k = 0;k < pfi->f_clave[0].k_ncampos;k++)
			{
			   if (k)		   
				  fprintf(f," AND ");
			   
			   i = pfi->f_clave[0].k_campo[k]-1;

			   fprintf(f,"C%03d=",i);
			   if (pfi->f_campos[i].c_tipo == CHARTIPO)
			   {
				   int j;
				   fprintf(f,"'");
				   ld_campo(i,pfi,buffer);
				   j = escapa_buffer(buffer);
				   fprintf(f,"%s'",buffer);
			   }
			   else if (pfi->f_campos[i].c_tipo == FECHATIPO)
			   {
				   char *pbuf;
				   pbuf = pfi->f_buf+pfi->f_campos[i].c_offset;
				   memcpy(buffer,pbuf,8);
				   buffer[8] = 0;
				   j = escapa_buffer(buffer);
				   __compon_fecha(f,buffer);	   
				   /*fprintf(f,"'%8.8s'",pfi->f_buf+pfi->f_campos[i].c_offset);*/
			   }
			   else
			   {
				   ld_campo(i,pfi,buffer); 
				   fprintf(f,"%s",buffer);
			   }		   
			}  

			fprintf(f,"\r\n");

			fclose(f);
		}
	}
	return 0;
}

int sql_txt_respaldo_borra(FI_DEF *pfi)
{
	if (sql_externo_filtro)
	{		

		if (!sql_lista_filtro)
		{
			respaldo_carga_filtro();
		}
		if (!respaldo_pasa_filtro(pfi->f_fichero,-1))
		{
			return 0;
		}
	}

	{
		FILE *f = sql_txt_open_output();
		if (f)
		{
			char buffer[512];
			int i,k;

			fprintf(f,"%ld.%ld DELETE FROM %s WHERE ",time(NULL),sql_txt_id(),mi_fichero(pfi->f_fichero));

			for (k = 0;k < pfi->f_clave[0].k_ncampos;k++)
			{
			   if (k)		   
				  fprintf(f," AND ");
			   
			   i = pfi->f_clave[0].k_campo[k]-1;

			   fprintf(f,"C%03d=",i);
			   if (pfi->f_campos[i].c_tipo == CHARTIPO)
			   {
				   int j;
				   fprintf(f,"'");
				   ld_campo(i,pfi,buffer);
				   j = escapa_buffer(buffer);
				   fprintf(f,"%s'",buffer);
			   }
			   else if (pfi->f_campos[i].c_tipo == FECHATIPO)
			   {			   
				   char *pbuf;
				   int j;
				   pbuf = pfi->f_buf+pfi->f_campos[i].c_offset;
				   memcpy(buffer,pbuf,8);
				   buffer[8] = 0;
				   j = escapa_buffer(buffer);
				   __compon_fecha(f,buffer);	   
				   /*fprintf(f,"'%8.8s'",pfi->f_buf+pfi->f_campos[i].c_offset);*/
			   }
			   else
			   {
				   ld_campo(i,pfi,buffer); 
				   fprintf(f,"%s",buffer);
			   }		   
			}  

			fprintf(f,"\r\n");
			fclose(f);
		}
	}
	return 0;
}

int sql_txt_respaldo_drop(FI_DEF *pfi)
{
	if (sql_externo_filtro)
	{		

		if (!sql_lista_filtro)
		{
			respaldo_carga_filtro();
		}
		if (!respaldo_pasa_filtro(pfi->f_fichero,-1))
		{
			return 0;
		}
	}

	{
		FILE *f = sql_txt_open_output();
		if (f)
		{
			fprintf(f,"%ld.%ld DROP TABLE %s\r\n",time(NULL),sql_txt_id(),mi_fichero(pfi->f_fichero));
			fclose(f);
		}
	}
	return 0;
}

int sql_txt_respaldo_sentencia(char *p)
{
	{
		FILE *f = sql_txt_open_output();
		if (f)
		{
			fprintf(f,"%ld.%ld %s",time(NULL),sql_txt_id(),p);
			fclose(f);
		}
	}
	return 0;
}

