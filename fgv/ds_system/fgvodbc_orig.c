
/********************************************************************************/



#include <fgvsys.h>

#ifdef FGVDOS

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

#include <sql.h>
#include <sqlext.h>

#include <rtermin.h>


#define _NUEVODEF
#include <rdef.h>      /* ya incluye el isam.h */


extern char *odbc_con_s;

static int se_soporta_bloqueo_cursor = 0;
/* handles para la conexion */
static SQLHANDLE m_h_env = NULL;
static SQLHANDLE m_h_dbc = NULL;
/* en principio usaremos un solo statement handle que reusaremos continuamente por hacer caso
de la advertencia de la limitacion de los mismos a nivel del driver, seguramente habra
que utilizar mas adelante uno por tabla para mejorar la velocidad */
static SQLHANDLE m_h_stm = NULL; /* handle general para operaciones especiales */

static SQLHANDLE m_h_des = NULL;

#ifndef IXXMAXFILES
#define IXXMAXFILES 100
#endif

typedef struct fgv_conexion 
{
   char tabla[256];
   long RegCurr;  
   int KeyCurr;
   int locked;
   //int cursor;
   //long poscursor;
   SQLHANDLE h_stm;
   SQLHANDLE lock_h_stm; // Lock handle!!!
   char *buf;
   SDWORD xx[256];  
   SDWORD cbx;
   FI_DEF *pfi; // pfi asignado ... solo para lee_secuencial etc de momento ...
} FGVCONEXION;

static FGVCONEXION *FgvConexion[IXXMAXFILES] = {NULL,};


static char *mi_sqltipo(int mitipo,int milen)
{
   static char eltipo[1024];
   switch(mitipo)
   {
   case INTTIPO:                          
      strcpy(eltipo,"SMALLINT");
      break;
   case LONGTIPO:
      strcpy(eltipo,"INTEGER");
      break;
   case FLOATTIPO:
      strcpy(eltipo,"REAL");
      break;
   case DOUBLETIPO:
      strcpy(eltipo,"DOUBLE");// PRECISION
      //strcpy(eltipo,"REAL");
      break;
      /*
      case FECHATIPO:
      strcpy(eltipo,"DATE");                          
      break;
      */
   default:                          
      sprintf(eltipo,"CHAR(%d)",milen);
      break; 
   }
   return(eltipo);
}

static int mi_tipo_csql(int tipo)
{
   int mi_tipo;
   switch(tipo)
   {
   case SQL_CHAR:
      mi_tipo = CHARTIPO;
      break;
   case SQL_SMALLINT:
      mi_tipo = INTTIPO;
      break;
   case SQL_INTEGER:
      mi_tipo = LONGTIPO;
      break;
   case SQL_REAL:
      mi_tipo = FLOATTIPO;
      break;
   case SQL_DOUBLE:
      mi_tipo = DOUBLETIPO;
      break;
      /*
      case SQL_DATE:
      mi_tipo = FECHATIPO;
      */
      break;
   default:
      mi_tipo = -1;
   }
   return(mi_tipo);
}

static int su_tipo_csql(int tipo)
{
   int mi_tipo;
   switch(tipo)
   {
   case INTTIPO:
      mi_tipo = SQL_SMALLINT;
      break;
   case LONGTIPO:
      mi_tipo = SQL_INTEGER;
      break;
   case FLOATTIPO:
      mi_tipo = SQL_REAL;
      break;
   case DOUBLETIPO:
      mi_tipo = SQL_DOUBLE;
      break;
      /*
      case FECHATIPO:
      mi_tipo = SQL_DATE;
      break;
      */
   default:
      mi_tipo = SQL_CHAR;
   }
   return(mi_tipo);
}


static int su_tipo_C_sql(int tipo)
{
   int mi_tipo;
   switch(tipo)
   {
   case INTTIPO:
      mi_tipo = SQL_C_SSHORT;
      break;
   case LONGTIPO:
      mi_tipo = SQL_C_SLONG;
      break;
   case FLOATTIPO:
      mi_tipo = SQL_C_FLOAT;
      break;
   case DOUBLETIPO:
      mi_tipo = SQL_C_DOUBLE;
      break;
      /*
      case FECHATIPO:
      mi_tipo = SQL_C_DATE;
      break;
      */
   default:
      mi_tipo = SQL_C_CHAR;
   }
   return(mi_tipo);
}


static int libera_driver()
{
   
   if (m_h_stm)
   {
      SQLFreeHandle(SQL_HANDLE_STMT,m_h_stm);  
      m_h_stm = NULL;
   }
   if (m_h_dbc)
   {
      SQLDisconnect(m_h_dbc);
      SQLFreeHandle(SQL_HANDLE_DBC,m_h_dbc);  
      m_h_dbc = NULL;
   }
   if (m_h_env)
   {
      SQLFreeHandle(SQL_HANDLE_DBC,m_h_env);
      m_h_env = NULL;
   }
   
   return(0);
}

static int mira_sql_error(SQLHANDLE stm)
{
   char buffer[3000];
   long recnum;
   short s;
   SQLRETURN e;
   e = SQLGetDiagRec(SQL_HANDLE_STMT,stm,1,buffer,&recnum,buffer+10,2000,&s);
   return(0);
}

static SQLHANDLE da_stm()
{
   SQLRETURN r;
   SQLHANDLE tmp_h_stm = NULL;
   
   
   r = SQLAllocHandle(SQL_HANDLE_STMT,m_h_dbc,&tmp_h_stm);
   if (r != SQL_SUCCESS && r != SQL_SUCCESS_WITH_INFO) 
   {   
      return(NULL);
   }
   
   r = SQLSetStmtAttr(tmp_h_stm, SQL_ATTR_CURSOR_TYPE, SQL_CURSOR_DYNAMIC , 0);
   /*
   if (r != SQL_SUCCESS && r != SQL_SUCCESS_WITH_INFO) 
   {
   mira_sql_error(m_h_stm);
   }
   */
   r = SQLSetStmtAttr(tmp_h_stm, SQL_ATTR_CURSOR_SCROLLABLE, SQL_SCROLLABLE , 0);
   /*
   if (r != SQL_SUCCESS && r != SQL_SUCCESS_WITH_INFO) 
   {
   mira_sql_error(m_h_stm);
   }
   */
   
   r = SQLSetStmtAttr(tmp_h_stm, SQL_ATTR_USE_BOOKMARKS, SQL_UB_VARIABLE , 0);
   
   
   
   r = SQLSetStmtAttr(tmp_h_stm, SQL_ATTR_CONCURRENCY, SQL_CONCUR_LOCK , 0);
   /*
   if (r != SQL_SUCCESS && r != SQL_SUCCESS_WITH_INFO) 
   {
   mira_sql_error(m_h_stm);
   libera_driver();   
   return(0);
   }
   */
   return(tmp_h_stm);
}

static int conecta_al_driver()
{
   SQLRETURN r;
   unsigned char con_s[1024];
   unsigned char buffer[5000];
   SQLSMALLINT pp;
   
   strcpy(con_s,"DSN=");
   if (!odbc_con_s)
      strcat(con_s,"diagram");
   else
      strcat(con_s,odbc_con_s);
   
   r = SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE,&m_h_env);
   
   if (r != SQL_SUCCESS && r != SQL_SUCCESS_WITH_INFO) 
   {
      aviso(0,"ODBC No se alojan enviroment handles");
      return(0);
   }
   
   r = SQLSetEnvAttr(m_h_env, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0); 
   
   if (r != SQL_SUCCESS && r != SQL_SUCCESS_WITH_INFO) 
   {
      aviso(0,"ODBC Version incorrecta de driver odbc, minimo version 3");
      libera_driver();   
      return(0);
   }
   
   r = SQLAllocHandle(SQL_HANDLE_DBC,m_h_env,&m_h_dbc);
   
   if (r != SQL_SUCCESS && r != SQL_SUCCESS_WITH_INFO) 
   {
      aviso(0,"ODBC No se alojan conection handles");
      libera_driver();   
      return(0);
   }
   
   r = SQLSetConnectAttr(m_h_dbc,(void*)SQL_ATTR_ODBC_CURSORS, SQL_CUR_USE_ODBC,0);
   if (r != SQL_SUCCESS && r != SQL_SUCCESS_WITH_INFO) 
   {
      aviso(0,"ODBC Driver odbc sin cursores");
      libera_driver();   
      return(0);
   }
   
   r = SQLDriverConnect(m_h_dbc,NULL,con_s,strlen((char *)con_s),buffer,5000,&pp,SQL_DRIVER_COMPLETE /* SQL_DRIVER_PROMPT*/);
   if (r != SQL_SUCCESS && r != SQL_SUCCESS_WITH_INFO) 
   {
      aviso(0,"ODBC No se conecta a la base de datos");
      libera_driver();   
      return(0);
   }
   
   r = SQLAllocHandle(SQL_HANDLE_STMT,m_h_dbc,&m_h_stm);
   if (r != SQL_SUCCESS && r != SQL_SUCCESS_WITH_INFO) 
   {
      aviso(0,"ODBC No se alojan statement handles");
      libera_driver();   
      return(0);
   }
   
   
   /*
   SQLGetInfo(m_h_stm,SQL_SCROLL_CONCURRENCY,
   SQLHDBC                ConnectionHandle,
   SQLUSMALLINT    InfoType,
   SQLPOINTER        InfoValuePtr,
   SQLSMALLINT        BufferLength,
   SQLSMALLINT *    StringLengthPtr);
   
     SQLGetInfo with the SQL_SCROLL_CONCURRENCY option.
   */
   r = SQLSetStmtAttr(m_h_stm, SQL_ATTR_CURSOR_TYPE, SQL_CURSOR_DYNAMIC , 0);
   if (r != SQL_SUCCESS && r != SQL_SUCCESS_WITH_INFO) 
   {
      aviso(0,"ODBC No se soportan cursores dinamicos");
      mira_sql_error(m_h_stm);
      libera_driver();   
      return(0);
   }
   
   r = SQLSetStmtAttr(m_h_stm, SQL_ATTR_CURSOR_SCROLLABLE, SQL_SCROLLABLE , 0);
   /*
   if (r != SQL_SUCCESS && r != SQL_SUCCESS_WITH_INFO) 
   {
   mira_sql_error(m_h_stm);
   libera_driver();   
   return(0);
   }
   */
   
   r = SQLSetStmtAttr(m_h_stm, SQL_ATTR_USE_BOOKMARKS, SQL_UB_VARIABLE , 0);
   
   r = SQLSetStmtAttr(m_h_stm, SQL_ATTR_CONCURRENCY, SQL_CONCUR_LOCK , 0);
   if (r == SQL_SUCCESS || r == SQL_SUCCESS_WITH_INFO) 
      se_soporta_bloqueo_cursor = 1;
   else
   {
      se_soporta_bloqueo_cursor = 0;
      /*aviso(0,"ODBC ATENCION: EL DRIVER ODBC NO SOPORTA BLOQUEOS!!!!!");*/
   }
   /*
   if (r != SQL_SUCCESS && r != SQL_SUCCESS_WITH_INFO) 
   {
   mira_sql_error(m_h_stm);
   libera_driver();   
   return(0);
   }
   */
   
   return(1);
}

static int ajusta_sqltipo_(int tipo,char *orig,char *buf,int modo,int longi)
{
   short i;
   long l;
   float f;
   double d;
   
   switch(tipo)
   {
   case SQL_SMALLINT:
      if (modo)
      {
         i = ldint(buf);
         *((short *)orig) = i;
      }
      else
      {
         i = *((short *)orig);
         stint(i,buf);
      }
      break;
   case SQL_INTEGER:
      if (modo)
      {
         l = ldlong(buf);
         *((long *)orig) = l;
      }
      else
      {
         l = *((long *)orig);
         stint(l,buf);
      }
      break;
   case SQL_REAL:
      if (modo)
      {
         f = ldfloat(buf);
         *((float *)orig) = f;
      }
      else
      {
         f = *((float *)orig);
         stfloat(f,buf);
      }
      break;
   case SQL_DOUBLE:
      if (modo)
      {
         d = lddbl(buf);
         *((double *)orig) = d;
      }
      else
      {
         d = *((double *)orig);
         stdbl(d,buf);
      }
      break;
      /*
      case SQL_DATE:
      mi_tipo = FECHATIPO;
      break;
      */
   default:
      if (modo)
         memcpy(orig,buf,longi);
      else
         memcpy(buf,orig,longi);
      break;
   }
   return(0);
}

static int filtra_modos_secuenciales(int modo)
{
   
   iserrno = EBADFILE;
   switch(modo)
   {
   case ISEQUAL:
   case ISGREAT:
   case ISGTEQ:
   case ISLOW:
   case ISLOWEQ:
   case ISFIRST:
   case ISLAST:
      iserrno = ENOREC;
      break;  
   case ISCURR:
      iserrno = ENOCURR;
      break;
   case ISNEXT:
   case ISPREV:
      iserrno = EENDFILE;
      break;
   }
}

static char *mi_fichero(char *f)
{
   static char _mi_fichero[256];
   int i;
   
   strcpy(_mi_fichero,f);
   for (i = 0;_mi_fichero[i];i++)
   {
      if (_mi_fichero[i] == '/' || _mi_fichero[i] == ':' || _mi_fichero[i] == '\\') 
         _mi_fichero[i] = '_';
   }
   return(_mi_fichero);
}

static int fd_correcto(int fd)
{
   if (fd >= IXXMAXFILES) 
      fd -= IXXMAXFILES;
   else
      fd = -1;
   return(fd);
}

static char *_valor_clave(FI_DEF *pfi,char *modo,long reg,int trozo)
{
   static char buffer[5000];
   int fd = fd_correcto(pfi->f_fd);
   int k = FgvConexion[fd]->KeyCurr;
   int i;
   
   buffer[0] = 0;
   
   if (k == pfi->f_nclaves)
   {
      //sprintf(buffer," WHERE NUMERO_DE_CONTROL%s%ld",modo,reg);
      sprintf(buffer," WHERE NUMERO_DE_CONTROL%s?",modo);
   }
   else
      for (i = 0;i < pfi->f_clave[k].k_ncampos;i++)
      {
         if (trozo && i >= trozo) break;
         if (!i)
         {
            strcat(buffer," WHERE ");
         }
         else
         {
            strcat(buffer," AND ");
         }
         sprintf(buffer+strlen(buffer),"K%03d%s? ",pfi->f_clave[k].k_campo[i]-1,modo);
         /*
         sprintf(buffer+strlen(buffer),"K%03d%s",pfi->f_clave[k].k_campo[i]-1,modo);
         if (pfi->f_campos[ ( pfi->f_clave[k].k_campo[i]-1 ) ].c_tipo == CHARTIPO || pfi->f_campos[ ( pfi->f_clave[k].k_campo[i]-1 ) ].c_tipo == FECHATIPO)
         {
         strcat(buffer,"'");
         ld_campo(pfi->f_clave[k].k_campo[i]-1,pfi,buffer+strlen(buffer));
         strcat(buffer,"'");
         }
         else
         ld_campo(pfi->f_clave[k].k_campo[i]-1,pfi,buffer+strlen(buffer));
         */
      }
      
      return(buffer);
}

static char *_orden_clave(FI_DEF *pfi,int trozo)
{
   static char orden_clave[1024];
   int fd = fd_correcto(pfi->f_fd);
   int k = FgvConexion[fd]->KeyCurr;
   int i;
   int t;
   
   orden_clave[0] = 0;
   if (k == pfi->f_nclaves)
   {
      strcpy(orden_clave," ORDER BY 1 ");
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
      }
      strcat(orden_clave," ");
   }
   return(orden_clave);
}

/*
// no es necesario cerrar el cursor activo a menos que sea necesario para un requery
// al cerrar el fichero ya se libera
static int cierra_cursor(int fd)
{
SQLCloseCursor(FgvConexion[fd]->h_stm);
FgvConexion[fd]->cursor = 0;
return(0);
}
*/
static int suma_char(int tipo)
{
   if (tipo == CHARTIPO || tipo == FECHATIPO)
      return(1);
   return(0);
}

// asigna el registro y todos los campos de la tabla al buffer intermedio para lectura
static int bind_buffer(FI_DEF *pfi,SQLHANDLE stm)
{
   int fd = fd_correcto(pfi->f_fd);
   SQLRETURN r;
   int i;
   
   r = SQLBindCol(stm, 1, SQL_C_SLONG, &(FgvConexion[fd]->RegCurr), sizeof(FgvConexion[fd]->RegCurr), &(FgvConexion[fd]->cbx));
   for (i = 0;i < pfi->f_ncampos;i++)
   {
      r = SQLBindCol(stm, i+2,su_tipo_C_sql(pfi->f_campos[i].c_tipo),FgvConexion[fd]->buf+pfi->f_campos[i].c_offset,pfi->f_campos[i].c_long+suma_char(pfi->f_campos[i].c_tipo),&(FgvConexion[fd]->xx[i]));
   }
   return(0);
}

// asume que como parametros son el registro + todos los campos de la tabla
// tipico para grabar
static int bind_param(FI_DEF *pfi,SQLHANDLE stm,int modo)
{
   int fd = fd_correcto(pfi->f_fd);
   
   SQLRETURN r;
   int i,j,despla = 1;
   
   if (!modo)
   {
      r = SQLBindParameter(stm, 1, SQL_PARAM_INPUT ,SQL_C_SLONG, SQL_INTEGER ,14,0,
         &(FgvConexion[fd]->RegCurr), sizeof(FgvConexion[fd]->RegCurr), &(FgvConexion[fd]->cbx));
      despla = 2;
   }
   for (i = 0;i < pfi->f_ncampos;i++)
   {
      if (pfi->f_campos[i].c_tipo == CHARTIPO || pfi->f_campos[i].c_tipo == FECHATIPO)
      {
         j = pfi->f_campos[i].c_long;
      }
      else
      {
         j = 14;
      }
      FgvConexion[fd]->xx[i] = pfi->f_campos[i].c_long;
      r = SQLBindParameter(stm, i+despla,SQL_PARAM_INPUT ,su_tipo_C_sql(pfi->f_campos[i].c_tipo),su_tipo_csql(pfi->f_campos[i].c_tipo),j,0,FgvConexion[fd]->buf+pfi->f_campos[i].c_offset,
         pfi->f_campos[i].c_long+suma_char(pfi->f_campos[i].c_tipo),&(FgvConexion[fd]->xx[i]));
   }
   if (modo)
   {
      r = SQLBindParameter(stm, pfi->f_ncampos+1, SQL_PARAM_INPUT ,SQL_C_SLONG, SQL_INTEGER ,14,0,
         &(FgvConexion[fd]->RegCurr), sizeof(FgvConexion[fd]->RegCurr), &(FgvConexion[fd]->cbx));
   }
   return(0);
}

// asume que como parametros son los campos de la clave corriente, tipico para busquedas
// ya carga los valores al buffer intermedio!
static int bind_param_clave(FI_DEF *pfi,int trozo,SQLHANDLE stm,int modo)
{
   int fd = fd_correcto(pfi->f_fd);
   SQLRETURN r;
   int i,j,n;
   int campo,tipo,longi,posi;
   int k = FgvConexion[fd]->KeyCurr;
   int t;
   
   if (k == pfi->f_nclaves)
   {
      r = SQLBindParameter(stm, 1, SQL_PARAM_INPUT ,SQL_C_SLONG, SQL_INTEGER ,14,0,
         &(FgvConexion[fd]->RegCurr), sizeof(FgvConexion[fd]->RegCurr), &(FgvConexion[fd]->cbx));
   }
   else
   {
      if (trozo) t = trozo; else t = pfi->f_clave[k].k_ncampos;
      
      if (modo != 2) // en modo == 2 los valores ya estan en el buffer intermedio
      {
         for (n = 0;n < t;n++)
         {
            campo = pfi->f_clave[k].k_campo[n]-1;
            tipo = su_tipo_csql(pfi->f_campos[campo].c_tipo);
            longi = pfi->f_campos[campo].c_long;
            posi = pfi->f_campos[campo].c_offset;     
            ajusta_sqltipo_(tipo,FgvConexion[fd_correcto(pfi->f_fd)]->buf+posi,pfi->f_buf+posi,1,longi);
         }   
      }
      
      for (n = 0;n < t;n++)
      { 
         i = pfi->f_clave[k].k_campo[n]-1;
         if (pfi->f_campos[i].c_tipo == CHARTIPO || pfi->f_campos[i].c_tipo == FECHATIPO)
         {
            j = pfi->f_campos[i].c_long;
         }
         else
         {
            j = 14;
         }
         FgvConexion[fd]->xx[i] = pfi->f_campos[i].c_long;
         r = SQLBindParameter(stm, n+1,SQL_PARAM_INPUT ,su_tipo_C_sql(pfi->f_campos[i].c_tipo),su_tipo_csql(pfi->f_campos[i].c_tipo),j,0,FgvConexion[fd]->buf+pfi->f_campos[i].c_offset,
            pfi->f_campos[i].c_long+suma_char(pfi->f_campos[i].c_tipo),&(FgvConexion[fd]->xx[i]));
      } 
   }
   return(0);
}



// hace el bind de lectura y realiza un fech simple para saber si es que hay resultados
// luego se puede usar el cursor o no. En caso de error cierra el cursor ademas de devolver 1 si
// no hay datos
static int datos_a_buffer(FI_DEF *pfi,SQLHANDLE stm)
{
   int fd = fd_correcto(pfi->f_fd);
   SDWORD cbx;
   SQLRETURN r;
   int i;
   
   bind_buffer(pfi,stm);
   r = SQLFetch(stm);
   if (r != SQL_SUCCESS && r != SQL_SUCCESS_WITH_INFO && r != SQL_NO_DATA) 
   {   
      iserrno = EBADMEM;
      SQLCloseCursor(stm);
      return(-1);
   }
   if (r == SQL_NO_DATA)
   {
      SQLCloseCursor(stm);
      return(1);
   }
   return(0);
}


static int _cambio_estructura(FI_DEF *pfi)
{
   aviso(0,"ODBC VERSION ALFA!! NO HAY CAMBIO DE VERSION AUTOMATICO!");
   iserrno = EBADFILE;
   return(-1);
}

int odbc__cierra_ind(fd)
register int fd;
{
   int i;
   
   fd = fd_correcto(fd);
   if (FgvConexion[fd])
   {
      if (FgvConexion[fd]->h_stm)
      {
         SQLFreeHandle(SQL_HANDLE_STMT,FgvConexion[fd]->h_stm);
      }
      if (FgvConexion[fd]->lock_h_stm)
      {
         SQLFreeHandle(SQL_HANDLE_STMT,FgvConexion[fd]->lock_h_stm);
      }
      if (FgvConexion[fd]->buf)
         free(FgvConexion[fd]->buf);
      free((char *)FgvConexion[fd]);
      FgvConexion[fd] = NULL;
   }
   
   
   for (i = 0;i < IXXMAXFILES;i++)
   {
      if (FgvConexion[i])
      {
         return(0);
      }
   }
   
   
   libera_driver();
   
   return(0);
}

int odbc_cierra_ind(pfi)
register FI_DEF *pfi;
{
   if (fd_correcto(pfi->f_fd) < 0 || !FgvConexion[fd_correcto(pfi->f_fd)]) return(-1);
   return(odbc__cierra_ind(pfi->f_fd));
}


int odbc_crea_ind(pfi)
register FI_DEF *pfi;
{
			int fd;
         SQLRETURN r;
         int i,kk;
         char tmp[256];
         char buffer[10000];
         
         if (pfi->f_nclaves < 1 || (!m_h_env && !conecta_al_driver()))
         {
            iserrno = EBADFILE;
            return(-1);
         }
         
         
         SQLFreeHandle(SQL_HANDLE_STMT,m_h_stm);
         m_h_stm = da_stm();
         if (!m_h_stm)
         {
            aviso(0,"ODBC ATENCION: Faltan handles!!!");
            return(-1);
         }
         
         sprintf(buffer,"CREATE TABLE %s (NUMERO_DE_CONTROL INTEGER",mi_fichero(pfi->f_fichero));
         for (i = 0;i < pfi->f_ncampos;i++)
         {   
            strcat(buffer,",");
            sprintf(tmp,"K%03d %s",i,mi_sqltipo(pfi->f_campos[i].c_tipo,pfi->f_campos[i].c_long));
            strcat(buffer,tmp);
         }
         /*
         strcat(buffer,",CONSTRAINT clave0 PRIMARY (");
         for (i = 0;i < pfi->f_clave[0].k_ncampos;i++)
         {
         if (i)
         {
         strcat(buffer,",");
         }
         sprintf(tmp,"K%03d",pfi->f_clave[0].k_campo[i]-1);
         if (pfi->f_clave[0].k_orden[i] < 0)
         strcat(tmp," DESC");
         else
         strcat(tmp," ASC"); 
         strcat(buffer,tmp);   
         }
         strcat(buffer,"))");
         */
         strcat(buffer,")");
         r = SQLExecDirect(m_h_stm,buffer,strlen(buffer));
         
         if (r != SQL_SUCCESS && r != SQL_SUCCESS_WITH_INFO)
         {
            mira_sql_error(m_h_stm);
            iserrno = EBADFILE;
            return(-1);
         }
         
         for (/*kk = 1*/kk = 0;kk < pfi->f_nclaves;kk++)
         {
            
            if (!kk)
               sprintf(buffer,"CREATE UNIQUE INDEX clave%d ON %s (",kk,mi_fichero(pfi->f_fichero));
            else
               sprintf(buffer,"CREATE INDEX clave%d ON %s (",kk,mi_fichero(pfi->f_fichero));
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
            /*if (kk)
            strcat(buffer,",NUMERO_DE_CONTROL"); // 
            */
            strcat(buffer,")");
            r = SQLExecDirect(m_h_stm,buffer,strlen(buffer));
            if (r != SQL_SUCCESS && r != SQL_SUCCESS_WITH_INFO)
            {
               mira_sql_error(m_h_stm);
               iserrno = EBADFILE;
               return(-1);
            }
         }
         sprintf(buffer,"CREATE UNIQUE INDEX clave%d ON %s (NUMERO_DE_CONTROL)",pfi->f_nclaves,mi_fichero(pfi->f_fichero));
         r = SQLExecDirect(m_h_stm,buffer,strlen(buffer));
         if (r != SQL_SUCCESS && r != SQL_SUCCESS_WITH_INFO)
         {
            mira_sql_error(m_h_stm);
            iserrno = EBADFILE;
            return(-1);
         }
         
         iserrno = 0;
         fd = odbc_abre_ind(pfi,ISEXCLLOCK);
         return(fd);
}

int odbc_abre_ind(pfi,modo)
register FI_DEF *pfi;
int modo; // ni caso del modo de momento!!!
{
   int fd = 0,i;
   char buffer[10000];
   SQLRETURN r,rr;
   short columnas = 0;
   char tmp[512];
   char tmp2[512];
   short tipo;
   SQLUINTEGER lon;
   short dec;
   short nul;
   SQLHANDLE tmp_h_stm = NULL;
   short colen;
   
   for (fd = 0;fd < IXXMAXFILES;fd++)
   {
      if (!FgvConexion[fd])
         break;
   }
   
   if (fd >= IXXMAXFILES)
   {
      iserrno = ETOOMANY;
      return(-1);
   }
   
   if (!m_h_env && !conecta_al_driver())
   {
      iserrno = EBADFILE;
      return(-1);
   }
   
   /* analizar las columnas de la tabla */
   /*
   SQLRETURN SQLColumns(m_h_stm,"",0,"",0,mi_fichero(pfi->f_fichero),strlen(mi_fichero(pfi->f_fichero))
   SQLHSTMT        StatementHandle,
   SQLCHAR *        CatalogName,
   SQLSMALLINT    NameLength1,
   SQLCHAR *        SchemaName,
   SQLSMALLINT    NameLength2,
   SQLCHAR *        TableName,
   SQLSMALLINT    NameLength3,
   SQLCHAR *        ColumnName,
   SQLSMALLINT    NameLength4);
   */
   
   tmp_h_stm = da_stm();
   if (!tmp_h_stm)
   {
      iserrno = EBADMEM;
      return(-1);
   }
   
   /* equivalente a un isfirst */
   sprintf(buffer,"SELECT * FROM %s ORDER BY ",mi_fichero(pfi->f_fichero));
   for (i = 0;i < pfi->f_clave[0].k_ncampos;i++)
   {
      if (i)
         strcat(buffer,", ");
      sprintf(buffer+strlen(buffer),"%d",pfi->f_clave[0].k_campo[i]+1);
   }
   strcat(buffer," ");
   
   rr = SQLExecDirect(tmp_h_stm,buffer,strlen(buffer));
   if (rr != SQL_NO_DATA && rr != SQL_SUCCESS && rr != SQL_SUCCESS_WITH_INFO)
   {
      if (tmp_h_stm)
						{
         SQLFreeHandle(SQL_HANDLE_STMT,tmp_h_stm);
						}
      iserrno = ENOENT;
      return(-1);
   }
   
   if (rr != SQL_NO_DATA)
   {
      r = SQLNumResultCols(tmp_h_stm,&columnas);
      if (r != SQL_SUCCESS && r != SQL_SUCCESS_WITH_INFO)
						{
         if (tmp_h_stm)
         {
            SQLFreeHandle(SQL_HANDLE_STMT,tmp_h_stm);
         }
         iserrno = ENOENT;
         return(-1);
						}
      
      if (columnas != (pfi->f_ncampos+1))
						{
         if (tmp_h_stm)
         {
            SQLFreeHandle(SQL_HANDLE_STMT,tmp_h_stm);
         }
         return(_cambio_estructura(pfi));
						}
      
      r = SQLDescribeCol(tmp_h_stm,1,tmp,sizeof(tmp)-1,&colen,&tipo,&lon,&dec,&nul);   
      if (strcmp(tmp,"NUMERO_DE_CONTROL"))
						{
         if (tmp_h_stm)
         {
            SQLFreeHandle(SQL_HANDLE_STMT,tmp_h_stm);
         }
         return(_cambio_estructura(pfi));
						}
      for (i = 0;i < pfi->f_ncampos;i++)
						{   
         r = SQLDescribeCol(tmp_h_stm,(short)(i+2),tmp,sizeof(tmp)-1,&colen,&tipo,&lon,&dec,&nul);   
         sprintf(tmp2,"K%03d",i);
         if (strcmp(tmp,tmp2))
         {
            if (tmp_h_stm)
            {
               SQLFreeHandle(SQL_HANDLE_STMT,tmp_h_stm);
            }
            return(_cambio_estructura(pfi));
         }
         
         if (tipo != su_tipo_csql(pfi->f_campos[i].c_tipo))
         {
            if (tmp_h_stm)
            {
               SQLFreeHandle(SQL_HANDLE_STMT,tmp_h_stm);
            }
            return(_cambio_estructura(pfi));
         }
         tipo = mi_tipo_csql(tipo);
         if (tipo == CHARTIPO && pfi->f_campos[i].c_long != lon)
         {
            if (tmp_h_stm)
            {
               SQLFreeHandle(SQL_HANDLE_STMT,tmp_h_stm);
            }
            return(_cambio_estructura(pfi));
         }
						}
   }
   /*
   SQLRETURN SQLDescribeCol(SQLHSTMT            StatementHandle,
   SQLSMALLINT        ColumnNumber,SQLCHAR *            ColumnName,
   SQLSMALLINT        BufferLength,SQLSMALLINT *    NameLengthPtr,
   SQLSMALLINT *    DataTypePtr,SQLUINTEGER *    ColumnSizePtr,
   SQLSMALLINT *    DecimalDigitsPtr,SQLSMALLINT *    NullablePtr
   */
   FgvConexion[fd] = (FGVCONEXION *)malloc(sizeof(FGVCONEXION));
   memset((char *)FgvConexion[fd],0,sizeof(FGVCONEXION));
   strcpy(FgvConexion[fd]->tabla,mi_fichero(pfi->f_fichero));
   FgvConexion[fd]->RegCurr = 0L;
   FgvConexion[fd]->KeyCurr = 0;
   FgvConexion[fd]->buf = malloc(pfi->f_lfich+1);
   FgvConexion[fd]->h_stm = tmp_h_stm;
   
   pfi->f_fd = fd + IXXMAXFILES;
   datos_a_buffer(pfi,FgvConexion[fd]->h_stm);
   iserrno = 0;
   /*
   if (rr != SQL_NO_DATA)
   {
   r = SQLGetData(tmp_h_stm,1,SQL_C_SLONG,&(FgvConexion[fd]->RegCurr),sizeof(FgvConexion[fd]->RegCurr),NULL);
   
     if (r != SQL_SUCCESS && r != SQL_SUCCESS_WITH_INFO && r != SQL_NO_DATA) 
     {
     iserrno = EBADMEM;
     return(-1);
     }
     
       }
   */
   return(pfi->f_fd);
}


static int llena_lectura(FI_DEF *pfi,char *buffer,int modo,char *pb,int trozo,SQLHANDLE stm)
{
   SQLRETURN r = 0;
   SQLSMALLINT op;
   int por_bindear_par = 0;
   
   
   buffer[0] = 0;
   switch(modo)
   {
   case ISFIRST:
      sprintf(buffer,"SELECT * FROM %s%s",FgvConexion[fd_correcto(pfi->f_fd)]->tabla,_orden_clave(pfi,trozo));
      op = SQL_FETCH_FIRST;
      break;
   case ISLAST:
      sprintf(buffer,"SELECT * FROM %s%s",FgvConexion[fd_correcto(pfi->f_fd)]->tabla,_orden_clave(pfi,trozo));
      op = SQL_FETCH_LAST;
      break;
   case ISNEXT:
      //sprintf(buffer,"SELECT * FROM %s%s%s",FgvConexion[fd_correcto(pfi->f_fd)]->tabla,_valor_clave(pfi,">",FgvConexion[fd_correcto(pfi->f_fd)]->RegCurr,0),_orden_clave(pfi,trozo));
      /* Por cursor!! */
      r = SQLFetchScroll(stm,SQL_FETCH_NEXT,1);
      if (r != SQL_SUCCESS && r != SQL_SUCCESS_WITH_INFO)
      { /* == ISGREAT */
         sprintf(buffer,"SELECT * FROM %s%s%s",FgvConexion[fd_correcto(pfi->f_fd)]->tabla,_valor_clave(pfi,">",0L,trozo),_orden_clave(pfi,trozo));
         por_bindear_par = 2;
         op = SQL_FETCH_FIRST;           
      }       
      break;
   case ISPREV:
      //sprintf(buffer,"SELECT * FROM %s%s%s",FgvConexion[fd_correcto(pfi->f_fd)]->tabla,_valor_clave(pfi,"<",FgvConexion[fd_correcto(pfi->f_fd)]->RegCurr,0),_orden_clave(pfi,trozo));
      /* Por cursor!! */
      r = SQLFetchScroll(stm,SQL_FETCH_PRIOR,1);
      if (r != SQL_SUCCESS && r != SQL_SUCCESS_WITH_INFO)
      { /* == ISLOV */
         sprintf(buffer,"SELECT * FROM %s%s%s",FgvConexion[fd_correcto(pfi->f_fd)]->tabla,_valor_clave(pfi,"<",0L,trozo),_orden_clave(pfi,trozo));
         por_bindear_par = 2;
         op = SQL_FETCH_LAST;
      }           
      break;
   case ISCURR:
      //sprintf(buffer,"SELECT * FROM %s WHERE NUMERO_DE_CONTROL=%ld ORDER BY 1",FgvConexion[fd_correcto(pfi->f_fd)]->tabla,FgvConexion[fd_correcto(pfi->f_fd)]->RegCurr);
      //sprintf(buffer,"SELECT * FROM %s WHERE NUMERO_DE_CONTROL=? ORDER BY 1",FgvConexion[fd_correcto(pfi->f_fd)]->tabla,FgvConexion[fd_correcto(pfi->f_fd)]->RegCurr);
      /* Por cursor!! */       
      r = SQLFetchScroll(stm,SQL_FETCH_RELATIVE,0);
      if (r != SQL_SUCCESS && r != SQL_SUCCESS_WITH_INFO)
         iserrno = ENOCURR;
      break;
   case ISEQUAL:
      sprintf(buffer,"SELECT * FROM %s%s%s%s",FgvConexion[fd_correcto(pfi->f_fd)]->tabla,_valor_clave(pfi,"=",0L,trozo),_orden_clave(pfi,trozo),pb);
      por_bindear_par = 1;
      op = SQL_FETCH_FIRST;
      break;
   case ISGREAT:
      sprintf(buffer,"SELECT * FROM %s%s%s",FgvConexion[fd_correcto(pfi->f_fd)]->tabla,_valor_clave(pfi,">",0L,trozo),_orden_clave(pfi,trozo));
      por_bindear_par = 1;
      op = SQL_FETCH_FIRST;
      break;
   case ISGTEQ:
      sprintf(buffer,"SELECT * FROM %s%s%s",FgvConexion[fd_correcto(pfi->f_fd)]->tabla,_valor_clave(pfi,">=",0L,trozo),_orden_clave(pfi,trozo));
      por_bindear_par = 1;
      op = SQL_FETCH_FIRST;
      break;
   case ISLOW:
      sprintf(buffer,"SELECT * FROM %s%s%s",FgvConexion[fd_correcto(pfi->f_fd)]->tabla,_valor_clave(pfi,"<",0L,trozo),_orden_clave(pfi,trozo));
      por_bindear_par = 1;
      op = SQL_FETCH_LAST;
      break;
   case ISLOWEQ:
      sprintf(buffer,"SELECT * FROM %s%s%s",FgvConexion[fd_correcto(pfi->f_fd)]->tabla,_valor_clave(pfi,"<=",0L,trozo),_orden_clave(pfi,trozo));
      por_bindear_par = 1;
      op = SQL_FETCH_LAST;
      break;
   default:
      break;
   }
   
   if (buffer[0])
   {  
      SQLCloseCursor(stm);
      r = SQLPrepare(stm, buffer, strlen(buffer));
      if (r != SQL_SUCCESS && r != SQL_SUCCESS_WITH_INFO)
      {
         mira_sql_error(stm);
         iserrno = EBADFILE;        
         return(-1);
      }
      if (por_bindear_par)
      {
         bind_param_clave(pfi,trozo,stm,por_bindear_par);        
      }
      r = SQLExecute(stm);
      if (r != SQL_SUCCESS && r != SQL_SUCCESS_WITH_INFO)
      {
         mira_sql_error(stm);        
         iserrno = EBADFILE;        
         return(-1);
      }
      
      bind_buffer(pfi,stm);
      r = SQLFetchScroll(stm,op,1);
      if (r != SQL_SUCCESS && r != SQL_SUCCESS_WITH_INFO && r != SQL_NO_DATA)
      {
         mira_sql_error(stm);        
         iserrno = EBADFILE;        
         return(-1);     
      }
      if (r == SQL_NO_DATA)
      {
         filtra_modos_secuenciales(modo);
         SQLCloseCursor(stm);
         return(1);
      }
      if (strlen(pb) && FgvConexion[fd_correcto(pfi->f_fd)]->KeyCurr == pfi->f_nclaves)
      { //bloquear          
         if (se_soporta_bloqueo_cursor)
         {
            r = SQLSetPos(stm,1,SQL_POSITION,SQL_LOCK_EXCLUSIVE);
            if (r != SQL_SUCCESS && r != SQL_SUCCESS_WITH_INFO && r != SQL_NO_DATA)
            {
               mira_sql_error(stm);        
               iserrno = ELOCKED; // afinar cuando se pruebe con algo decente
               return(1);
            }
         }
         // el driver de access o la libreria de cursores no parece poder bloquear
         // a juzgar por los mensajes de error. Lo dejamos listo para probar posteriormemte
         // con sql server o interbase sin devolver de momento error
         // hay que devolverlo pues asi no se controla si el registro esta bloqueado o no
      }    
      r = 0;
   }
   
   if (r == SQL_SUCCESS || r == SQL_SUCCESS_WITH_INFO)
      r = 0;
   
   if (r == SQL_NO_DATA)
      return(1);
   else
      if (r)
         return(-1);
      return(0);
}

int odbc_apunta_ind(pfi,clave,modo,trozo)
register FI_DEF *pfi;
int clave,modo,trozo;
{
   int r,i,campo,tipo,longi,posi,k;
   char buffer[10000];
   SQLRETURN rr,e;
   if (fd_correcto(pfi->f_fd) < 0 || !FgvConexion[fd_correcto(pfi->f_fd)] || clave < -1 || clave > pfi->f_nclaves) return(-1);
   
   //cierra_cursor(fd_correcto(pfi->f_fd));
   
   isrecnum = FgvConexion[fd_correcto(pfi->f_fd)]->RegCurr;
   
   modo &= ~ISLOCK;
   modo &= ~ISWAIT;
   
   if (clave != -1)
   {
      if (!clave) 
      {
         FgvConexion[fd_correcto(pfi->f_fd)]->KeyCurr = pfi->f_nclaves;
      }
      else
      {
         clave--;
         FgvConexion[fd_correcto(pfi->f_fd)]->KeyCurr = clave;
      }
   }
   
   /* ojo: comprobar si no se ha de asumir el cambio de clave */
   if (!(FgvConexion[fd_correcto(pfi->f_fd)]->RegCurr) && (modo == ISCURR || modo == ISNEXT || modo == ISPREV))
   {
      iserrno = ENOCURR;
      return(-1);
   }
   
   r = llena_lectura(pfi,buffer,modo,"",trozo,FgvConexion[fd_correcto(pfi->f_fd)]->h_stm);
   
   /*
   rr = SQLExecDirect(FgvConexion[fd_correcto(pfi->f_fd)]->h_stm,buffer,strlen(buffer));
   if (rr != SQL_SUCCESS && rr != SQL_SUCCESS_WITH_INFO)
   {
   if (rr == SQL_NO_DATA)        
   {
   if (modo == ISFIRST)
   {
   iserrno = 0;           
   return(0);
   }
   filtra_modos_secuenciales(modo);
   }
   else
   iserrno = EBADFILE; 
   return(-1);
   }
   */
   /*
   e = SQLGetData(FgvConexion[fd_correcto(pfi->f_fd)]->h_stm,1,SQL_C_SLONG,&(FgvConexion[fd_correcto(pfi->f_fd)]->RegCurr),sizeof(FgvConexion[fd_correcto(pfi->f_fd)]->RegCurr),NULL);
   if (datos_a_buffer(pfi,FgvConexion[fd_correcto(pfi->f_fd)]->h_stm) == 1)
   {
   if (modo == ISFIRST)
   {
   iserrno = 0;           
   return(0);
   }
   filtra_modos_secuenciales(modo);
   return(-1);
   }
   */
   if (!r && (clave == -1 /*modo == ISGREAT || modo == ISGTEQ || modo == ISLOW || modo == ISLOWEQ*/) )
   {
      k = FgvConexion[fd_correcto(pfi->f_fd)]->KeyCurr;
      if (k < pfi->f_nclaves)
         for (i = 0;i < pfi->f_clave[k].k_ncampos;i++)
         {
            campo = pfi->f_clave[k].k_campo[k] - 1;
            tipo = su_tipo_csql(pfi->f_campos[campo].c_tipo);
            longi = pfi->f_campos[campo].c_long;
            posi = pfi->f_campos[campo].c_offset;
            /*e = SQLGetData(FgvConexion[fd_correcto(pfi->f_fd)]->h_stm,campo+2,tipo,pfi->f_buf+posi,longi,NULL);*/
            ajusta_sqltipo_(tipo,FgvConexion[fd_correcto(pfi->f_fd)]->buf+posi,pfi->f_buf+posi,0,longi);
         }
   }
   
   isrecnum = FgvConexion[fd_correcto(pfi->f_fd)]->RegCurr;
   
   if (r)
   {
      if (r == 1 && modo == ISFIRST) //caso de cambio de clave y no hay registros 
      {
         iserrno = 0;
         return(0);
      }
      return(-1);
   }
   iserrno = 0;
   return(0);
}

int odbc_cambia_ind(pfi,clave)
register FI_DEF *pfi;
int clave;
{
   if (fd_correcto(pfi->f_fd) < 0 || !FgvConexion[fd_correcto(pfi->f_fd)]) return(-1);
   return(odbc_apunta_ind(pfi,clave,ISFIRST,0));
}


int odbc_borra_actual(pfi)
register FI_DEF *pfi;
{
			SQLRETURN r;
         int i,rr;
         unsigned char buffer[1024];
         
         if (fd_correcto(pfi->f_fd) < 0 || !FgvConexion[fd_correcto(pfi->f_fd)]) return(-1);
         
         if (!(FgvConexion[fd_correcto(pfi->f_fd)]->RegCurr))
         {
            iserrno = ENOCURR;
            return(-1);
         }
         
         //cierra_cursor(fd_correcto(pfi->f_fd));
         rr = 0;
         if (!(FgvConexion[fd_correcto(pfi->f_fd)]->lock_h_stm) && se_soporta_bloqueo_cursor)
         {  // grabar sin bloqueo!!
            FgvConexion[fd_correcto(pfi->f_fd)]->lock_h_stm = da_stm();
            i = FgvConexion[fd_correcto(pfi->f_fd)]->KeyCurr;
            FgvConexion[fd_correcto(pfi->f_fd)]->KeyCurr = pfi->f_nclaves;
            rr = llena_lectura(pfi,buffer,ISEQUAL," FOR UPDATE",0,FgvConexion[fd_correcto(pfi->f_fd)]->lock_h_stm);
            FgvConexion[fd_correcto(pfi->f_fd)]->KeyCurr = i;
            if (rr)
            {
               iserrno = ENOCURR;
               return(-1);
            }
            rr = 1;
         }
         
         
         if (se_soporta_bloqueo_cursor && FgvConexion[fd_correcto(pfi->f_fd)]->lock_h_stm)
         {
            // parece que el access soporta una mierda, con lo facil que seria!!
            // el bind esta hecho de la lectura!!
            r = SQLSetPos(FgvConexion[fd_correcto(pfi->f_fd)]->lock_h_stm,1,SQL_DELETE,SQL_LOCK_NO_CHANGE);
         }
         else
         {
            SQLCloseCursor(m_h_stm);
            SQLFreeHandle(SQL_HANDLE_STMT,m_h_stm);
            m_h_stm = da_stm();
            if (!m_h_stm)
            {
               aviso(0,"ODBC ATENCION: Faltan handles!!!");
               return(-1);
            }
            
            i = FgvConexion[fd_correcto(pfi->f_fd)]->KeyCurr;
            FgvConexion[fd_correcto(pfi->f_fd)]->KeyCurr = pfi->f_nclaves;
            bind_param_clave(pfi,0,m_h_stm,1);
            sprintf(buffer,"DELETE * FROM %s WHERE NUMERO_DE_CONTROL=? ",FgvConexion[fd_correcto(pfi->f_fd)]->tabla);
            r = SQLExecDirect(m_h_stm,buffer,strlen(buffer));
            FgvConexion[fd_correcto(pfi->f_fd)]->KeyCurr = i;
         }
         if (r != SQL_SUCCESS && r != SQL_SUCCESS_WITH_INFO)
         {
            if (rr)
            {
               SQLFreeHandle(SQL_HANDLE_STMT,FgvConexion[fd_correcto(pfi->f_fd)]->lock_h_stm);
               FgvConexion[fd_correcto(pfi->f_fd)]->lock_h_stm = NULL;
            }
            iserrno = ENOCURR; 
            return(-1);
         }
         /*
         DELETE [table.*]
         
           FROM tableexpression
           
             WHERE criteria
         */
         if (rr)
         {
            SQLFreeHandle(SQL_HANDLE_STMT,FgvConexion[fd_correcto(pfi->f_fd)]->lock_h_stm);
            FgvConexion[fd_correcto(pfi->f_fd)]->lock_h_stm = NULL;
         }
         iserrno = 0;
         
         return(0);
         
}

int odbc_borra_reg(pfi)
register FI_DEF *pfi;
{
   SQLRETURN r;
   unsigned char buffer[5000];
   
   if (fd_correcto(pfi->f_fd) < 0 || !FgvConexion[fd_correcto(pfi->f_fd)]) return(-1);
   
   //cierra_cursor(pfi->f_fd);
   
   
   isrecnum = FgvConexion[fd_correcto(pfi->f_fd)]->RegCurr;
   SQLCloseCursor(m_h_stm);
   SQLFreeHandle(SQL_HANDLE_STMT,m_h_stm);
   m_h_stm = da_stm();
   if (!m_h_stm)
   {
      aviso(0,"ODBC ATENCION: Faltan handles!!!");
      return(-1);
   }
   
   bind_param_clave(pfi,0,m_h_stm,1);
   sprintf(buffer,"DELETE * FROM %s%s",FgvConexion[fd_correcto(pfi->f_fd)]->tabla,_valor_clave(pfi,"=",0L,0));
   r = SQLExecDirect(m_h_stm,buffer,strlen(buffer));
   if (r != SQL_SUCCESS && r != SQL_SUCCESS_WITH_INFO)
   {
      iserrno = ENOREC; 
      return(-1);
   }
   iserrno = 0;
   return(0);
}

long odbc_da_registo_actual(pfi)
FI_DEF *pfi;
{
   if (fd_correcto(pfi->f_fd) < 0 || !FgvConexion[fd_correcto(pfi->f_fd)]) return(0L);
   if (!FgvConexion[fd_correcto(pfi->f_fd)]) return(-1);
   return(FgvConexion[fd_correcto(pfi->f_fd)]->RegCurr);
}

void odbc_pon_registro_actual(pfi,r)
FI_DEF *pfi;
long r;
{
   if (fd_correcto(pfi->f_fd) < 0 || !FgvConexion[fd_correcto(pfi->f_fd)]) return;
   FgvConexion[fd_correcto(pfi->f_fd)]->RegCurr = r;
}


int odbc_ind_clave_activa(pfi)
FI_DEF *pfi;
{
   if (fd_correcto(pfi->f_fd) < 0 || !FgvConexion[fd_correcto(pfi->f_fd)]) return(-1);
   return(FgvConexion[fd_correcto(pfi->f_fd)]->KeyCurr+1);
}


int odbc_libera_ind(pfi)
register FI_DEF *pfi;
{
   char buffer[1024];
   SQLRETURN r;
   
   if (fd_correcto(pfi->f_fd) < 0 || !FgvConexion[fd_correcto(pfi->f_fd)]) return(-1);
   //cierra_cursor(fd_correcto(pfi->f_fd)); /* se supone que libera */
   // usar un handle diferente para los bloqueos!!!! */
   
   if (FgvConexion[fd_correcto(pfi->f_fd)]->lock_h_stm)
      SQLFreeHandle(SQL_HANDLE_STMT,FgvConexion[fd_correcto(pfi->f_fd)]->lock_h_stm);
   FgvConexion[fd_correcto(pfi->f_fd)]->lock_h_stm = NULL;
   FgvConexion[fd_correcto(pfi->f_fd)]->locked = 0;
   /*
   FgvConexion[fd_correcto(pfi->f_fd)]->locked = 0;
   sprintf(buffer,"UPDATE INTO %s",FgvConexion[fd_correcto(pfi->f_fd)]->tabla);
   r = SQLExecDirect(FgvConexion[fd_correcto(pfi->f_fd)]->h_stm,buffer,strlen(buffer));
   if (r != SQL_SUCCESS && r != SQL_SUCCESS_WITH_INFO)
   {
   iserrno = EBADFILE;
   return(-1);
   }
   */
   iserrno = 0;
   return(0);
}

int odbc_lee_reg(pfi,modo)
register FI_DEF *pfi;
int modo; /* modos compatibles C-ISAM */
{
   SQLRETURN r,e;
   unsigned char buffer[5000];
   SQLSMALLINT pp;
   int rr,i,campo,tipo,longi,posi;
   int bloqueo = 0;
   
   
   if (fd_correcto(pfi->f_fd) < 0 || !FgvConexion[fd_correcto(pfi->f_fd)]) return(-1);
   //cierra_cursor(fd_correcto(pfi->f_fd));
   
   isrecnum = FgvConexion[fd_correcto(pfi->f_fd)]->RegCurr;
   if ((modo & ISLOCK))
   {
      bloqueo = 1;
   }
   if ((modo & ISWAIT))
   {
      bloqueo = 2; 
   }
   
   /*
   if (bloqueo)
   {
   pb = " FOR UPDATE"; // usar el fetch_scroll
   }
   else
   {
   pb = "";
   }
   */
   
   modo &= ~ISLOCK;
   modo &= ~ISWAIT;
   /* ojo al ISWAIT */
   
   if (!(FgvConexion[fd_correcto(pfi->f_fd)]->RegCurr) && (modo == ISCURR || modo == ISNEXT || modo == ISPREV))
   {
      iserrno = ENOCURR;
      return(-1);
   }
   
   rr = llena_lectura(pfi,buffer,modo,"",0,FgvConexion[fd_correcto(pfi->f_fd)]->h_stm);
   
   /* 
   if (!(FgvConexion[fd_correcto(pfi->f_fd)]->KeyCurr) || FgvConexion[fd_correcto(pfi->f_fd)]->KeyCurr == pfi->f_nclaves)
   {
   if (modo == ISNEXT) modo = ISGREAT;
   else
   if (modo == ISPREV) modo = ISLOW;
   }
   */
   
   /*
   if (bloqueo)
   FgvConexion[fd_correcto(pfi->f_fd)]->locked = 1;
   rr = SQLExecDirect(FgvConexion[fd_correcto(pfi->f_fd)]->h_stm,buffer,strlen(buffer));
   if (rr != SQL_SUCCESS && rr != SQL_SUCCESS_WITH_INFO)
   {
   if (rr == SQL_NO_DATA)
   {
   filtra_modos_secuenciales(modo);        
   }
   else
   iserrno = EBADFILE; 
   return(-1);
   }
   */
   /*e = SQLGetData(FgvConexion[fd_correcto(pfi->f_fd)]->h_stm,1,SQL_C_SLONG,&(FgvConexion[fd_correcto(pfi->f_fd)]->RegCurr),sizeof(FgvConexion[fd_correcto(pfi->f_fd)]->RegCurr),NULL);*/
   
   /*
   if (datos_a_buffer(pfi,FgvConexion[fd_correcto(pfi->f_fd)]->h_stm) == 1)
   {
   filtra_modos_secuenciales(modo);  
   return(-1);
   }
   */
   if (!rr)
   {
      if (bloqueo)
      {
         if (!FgvConexion[fd_correcto(pfi->f_fd)]->lock_h_stm)
         {
												FgvConexion[fd_correcto(pfi->f_fd)]->lock_h_stm = da_stm();
         }
         else
         {
												SQLCloseCursor(FgvConexion[fd_correcto(pfi->f_fd)]->lock_h_stm);          
         }
         if (!FgvConexion[fd_correcto(pfi->f_fd)]->lock_h_stm)
         { 
												iserrno = ELOCKED; // no sepuede intentar el bloqueo
                                    rr = -1;
         }
         else
         {
												i = FgvConexion[fd_correcto(pfi->f_fd)]->KeyCurr;
                                    FgvConexion[fd_correcto(pfi->f_fd)]->KeyCurr = pfi->f_nclaves;
                                    rr = llena_lectura(pfi,buffer,ISEQUAL," FOR UPDATE",0,FgvConexion[fd_correcto(pfi->f_fd)]->lock_h_stm);
                                    FgvConexion[fd_correcto(pfi->f_fd)]->KeyCurr = i;
                                    if (!rr)
                                       FgvConexion[fd_correcto(pfi->f_fd)]->locked++;
         }
      }
   }
   if (!rr)
   {
      
      for (campo = 0;campo < pfi->f_ncampos;campo++)
      {
         tipo = su_tipo_csql(pfi->f_campos[campo].c_tipo);
         longi = pfi->f_campos[campo].c_long;
         posi = pfi->f_campos[campo].c_offset;
         /*e = SQLGetData(FgvConexion[fd_correcto(pfi->f_fd)]->h_stm,campo+2,tipo,pfi->f_buf+posi,longi,NULL);*/     
         ajusta_sqltipo_(tipo,FgvConexion[fd_correcto(pfi->f_fd)]->buf+posi,pfi->f_buf+posi,0,longi);
      }
   }
   isrecnum = FgvConexion[fd_correcto(pfi->f_fd)]->RegCurr;
   if (rr)
      return(-1);
   iserrno = 0;
   return(0);
}


/*
SQL_ADD
SQL_UPDATE_BY_BOOKMARK
SQL_DELETE_BY_BOOKMARK
SQL_FETCH_BY_BOOKMARK
*/

//  bind_buffer(pfi,m_h_stm);
//r = SQLFetch(m_h_stm);
/*Scroll(m_h_stm,SQL_FETCH_LAST,0);*/
/*
FgvConexion[fd_correcto(pfi->f_fd)]->RegCurr = recnum;
for (campo = 0;campo < pfi->f_ncampos;campo++)
{
tipo = su_tipo_csql(pfi->f_campos[campo].c_tipo);
longi = pfi->f_campos[campo].c_long;
posi = pfi->f_campos[campo].c_offset;     
ajusta_sqltipo_(tipo,FgvConexion[fd_correcto(pfi->f_fd)]->buf+posi,pfi->f_buf+posi,1,longi);
}
*/
//   r = SQLBulkOperations(m_h_stm,SQL_ADD);
/*
if (r == SQL_ERROR)
{
e = SQLGetDiagRec(SQL_HANDLE_STMT,m_h_stm,1,buffer,&recnum,buffer+10,2000,&s);
}

  SQLCloseCursor(m_h_stm);
  */
  
  int odbc_graba_nuevo(pfi)
     register FI_DEF *pfi;
  {
     SQLRETURN r,rr,e;
     unsigned char buffer[5000];
     int i,campo,tipo,longi,posi;
     short s;
     long recnum;
     int repite;
     
     /* problemas previstos: la competencia por el contador al grabar un registro nuevo por no usar ningun bloqueo */
     if (fd_correcto(pfi->f_fd) < 0 || !FgvConexion[fd_correcto(pfi->f_fd)]) return(-1);
     
     
     isrecnum = FgvConexion[fd_correcto(pfi->f_fd)]->RegCurr;   
     
     for (repite = 0;repite < 10;repite++)
     { /* esto es una manera simplona de solucionar el tema de la competencia por el numero de
        registro sin bloquear nada */
        SQLCloseCursor(m_h_stm);
        SQLFreeHandle(SQL_HANDLE_STMT,m_h_stm); // parece que si no se cierra se corrompe el asunto
        m_h_stm = da_stm();
        if (!m_h_stm)
        {
           aviso(0,"ODBC ATENCION: Faltan handles!!!");
           return(-1);
        }
        sprintf(buffer,"SELECT * FROM %s ORDER BY 1",FgvConexion[fd_correcto(pfi->f_fd)]->tabla);
        r = SQLExecDirect(m_h_stm,buffer,strlen(buffer));
        if (r != SQL_SUCCESS && r != SQL_SUCCESS_WITH_INFO)
        {
           mira_sql_error(m_h_stm);
           iserrno = EKEXISTS;
           FgvConexion[fd_correcto(pfi->f_fd)]->RegCurr = isrecnum;
           return(-1);
        }   
        bind_buffer(pfi,m_h_stm); /* con solo el registro seria suficiente */
        r = SQLFetchScroll(m_h_stm,SQL_FETCH_LAST,1);
        if (r == SQL_NO_DATA)
        {
           FgvConexion[fd_correcto(pfi->f_fd)]->RegCurr = 0L;
        }
        else
        {     
           if (r != SQL_SUCCESS && r != SQL_SUCCESS_WITH_INFO)
           {
              mira_sql_error(m_h_stm);
              SQLCloseCursor(m_h_stm);
              iserrno = EKEXISTS;
              FgvConexion[fd_correcto(pfi->f_fd)]->RegCurr = isrecnum;  
              return(-1);
           }
        }
        SQLCloseCursor(m_h_stm);
        recnum = FgvConexion[fd_correcto(pfi->f_fd)]->RegCurr;
        FgvConexion[fd_correcto(pfi->f_fd)]->RegCurr++;
        
        
        sprintf(buffer,"INSERT INTO %s ( NUMERO_DE_CONTROL",FgvConexion[fd_correcto(pfi->f_fd)]->tabla);
        for (i = 0;i < pfi->f_ncampos;i++)
        {
           strcat(buffer," ,");
           sprintf(buffer+strlen(buffer),"K%03d",i);
        }   
        //sprintf(buffer+strlen(buffer)," ) VALUES ( %ld ",recnum+12);
        sprintf(buffer+strlen(buffer)," ) VALUES ( ? ");
        
        for (i = 0;i < pfi->f_ncampos;i++)
        {
           strcat(buffer,",? ");
           /*
           if (pfi->f_campos[i].c_tipo == CHARTIPO || pfi->f_campos[i].c_tipo == FECHATIPO)
           {
           strcat(buffer,"'");
           ld_campo(i,pfi,buffer+strlen(buffer));
           strcat(buffer,"'");
           }
           else
           ld_campo(i,pfi,buffer+strlen(buffer));      
           */
        }   
        strcat(buffer,")");   
        
        //   rr = SQLExecDirect(m_h_stm,buffer,strlen(buffer));
        r = SQLPrepare(m_h_stm, buffer, strlen(buffer));
        if (r != SQL_SUCCESS && r != SQL_SUCCESS_WITH_INFO)
        {
           mira_sql_error(m_h_stm);
           iserrno = EBADFILE;
           FgvConexion[fd_correcto(pfi->f_fd)]->RegCurr = isrecnum;
           return(-1);
        }
        
        bind_param(pfi,m_h_stm,0);
        for (campo = 0;campo < pfi->f_ncampos;campo++)
        {
           tipo = su_tipo_csql(pfi->f_campos[campo].c_tipo);
           longi = pfi->f_campos[campo].c_long;
           posi = pfi->f_campos[campo].c_offset;     
           ajusta_sqltipo_(tipo,FgvConexion[fd_correcto(pfi->f_fd)]->buf+posi,pfi->f_buf+posi,1,longi);
        }
        
        r = SQLExecute(m_h_stm);
        if (r == SQL_SUCCESS || r == SQL_SUCCESS_WITH_INFO)
        {
           break;
        }
        /* a alguien le ira tocando ... */
     }
     
     if (r != SQL_SUCCESS && r != SQL_SUCCESS_WITH_INFO)
     {
        mira_sql_error(m_h_stm);
        iserrno = EKEXISTS;
        FgvConexion[fd_correcto(pfi->f_fd)]->RegCurr = isrecnum;
        return(-1);
     }
     
     
     FgvConexion[fd_correcto(pfi->f_fd)]->RegCurr = isrecnum;
     iserrno = 0;
     return(0);
}



int odbc_graba_actual(pfi)
register FI_DEF *pfi;
{
   unsigned char buffer[5000];
   SQLRETURN r;
   int i,campo,tipo,longi,posi,rr;
   
   if (fd_correcto(pfi->f_fd) < 0 || !FgvConexion[fd_correcto(pfi->f_fd)]) return(-1);
   
   isrecnum = FgvConexion[fd_correcto(pfi->f_fd)]->RegCurr;
   
   if (!FgvConexion[fd_correcto(pfi->f_fd)]->RegCurr)
   {
      iserrno = ENOCURR;
      return(-1);
   }
   
   /* probar con el put data, si va, solucionar el insert con este */
   
   /*
   SQLRETURN SQLSetPos(
   SQLHSTMT            StatementHandle,
   SQLUSMALLINT    RowNumber,
   SQLUSMALLINT    Operation,
   SQLUSMALLINT    LockType);
   SQL_LOCK_EXCLUSIVE The driver or data source locks the row exclusively. A statement on a different connection or in a different application cannot be used to acquire any locks on the row. 
   SQL_LOCK_UNLOCK The driver or data source unlocks the row. 
   
   */
   rr = 0;
   if (!(FgvConexion[fd_correcto(pfi->f_fd)]->lock_h_stm) && se_soporta_bloqueo_cursor)
   {  // grabar sin bloqueo!!
      FgvConexion[fd_correcto(pfi->f_fd)]->lock_h_stm = da_stm();
      i = FgvConexion[fd_correcto(pfi->f_fd)]->KeyCurr;
      FgvConexion[fd_correcto(pfi->f_fd)]->KeyCurr = pfi->f_nclaves;
      rr = llena_lectura(pfi,buffer,ISEQUAL," FOR UPDATE",0,FgvConexion[fd_correcto(pfi->f_fd)]->lock_h_stm);
      FgvConexion[fd_correcto(pfi->f_fd)]->KeyCurr = i;
      if (rr)
      {
         iserrno = ENOCURR;
         return(-1);
      }
      rr = 1;
   }
   
   
   for (campo = 0;campo < pfi->f_ncampos;campo++)
   {
      tipo = su_tipo_csql(pfi->f_campos[campo].c_tipo);
      longi = pfi->f_campos[campo].c_long;
      posi = pfi->f_campos[campo].c_offset;     
      ajusta_sqltipo_(tipo,FgvConexion[fd_correcto(pfi->f_fd)]->buf+posi,pfi->f_buf+posi,1,longi);
   }
   
   
   if (se_soporta_bloqueo_cursor && FgvConexion[fd_correcto(pfi->f_fd)]->lock_h_stm)
   {
      // parece que el access soporta una mierda, con lo facil que seria!!
      // el bind esta hecho de la lectura!!
      r = SQLSetPos(FgvConexion[fd_correcto(pfi->f_fd)]->lock_h_stm,1,SQL_UPDATE,SQL_LOCK_NO_CHANGE);
   }
   else
   {
      SQLCloseCursor(m_h_stm);
      SQLFreeHandle(SQL_HANDLE_STMT,m_h_stm);
      m_h_stm = da_stm();
      if (!m_h_stm)
      {
         aviso(0,"ODBC ATENCION: Faltan handles!!!");
         return(-1);
      }
      
      sprintf(buffer,"UPDATE %s SET ",FgvConexion[fd_correcto(pfi->f_fd)]->tabla);
      for (i = 0;i < pfi->f_ncampos;i++)
      {
         if (i)
            strcat(buffer,", ");
         sprintf(buffer+strlen(buffer),"K%03d=?",i);
      }   
      strcat(buffer," WHERE NUMERO_DE_CONTROL=? ");     
      r = SQLPrepare(m_h_stm, buffer, strlen(buffer));    
      if (r == SQL_SUCCESS || r == SQL_SUCCESS_WITH_INFO)
      {
         bind_param(pfi,m_h_stm,1);
         r = SQLExecute(m_h_stm);
      }
   }   
   
   if (r != SQL_SUCCESS && r != SQL_SUCCESS_WITH_INFO)
   {
      mira_sql_error(FgvConexion[fd_correcto(pfi->f_fd)]->lock_h_stm);
      iserrno = ENOCURR;
      if (rr)
      {
         SQLFreeHandle(SQL_HANDLE_STMT,FgvConexion[fd_correcto(pfi->f_fd)]->lock_h_stm);
         FgvConexion[fd_correcto(pfi->f_fd)]->lock_h_stm = NULL;
      }
      return(-1);
   }
   
   if (rr)
   {
      SQLFreeHandle(SQL_HANDLE_STMT,FgvConexion[fd_correcto(pfi->f_fd)]->lock_h_stm);
      FgvConexion[fd_correcto(pfi->f_fd)]->lock_h_stm = NULL;
   }
   isrecnum = FgvConexion[fd_correcto(pfi->f_fd)]->RegCurr;
   iserrno = 0;
   return(0);
}


int odbc_cierra_todo(como)
int como; 
{
   int i;
   
   for (i = 0;i < IXXMAXFILES;i++)
   {
      if (FgvConexion[i])
      {
         odbc__cierra_ind(cierra_join_(i+IXXMAXFILES));
      }
   }
   
   libera_driver();
   
   return(como);
}



odbc_lee_soloindice(pfi,modo)
register FI_DEF *pfi;
int modo;
{
   int r;
   if (fd_correcto(pfi->f_fd) < 0 || !FgvConexion[fd_correcto(pfi->f_fd)]) return(-1);
   
   isrecnum = FgvConexion[fd_correcto(pfi->f_fd)]->RegCurr;
   
   r = odbc_apunta_ind(pfi,-1,modo,0);
   
   isrecnum = FgvConexion[fd_correcto(pfi->f_fd)]->RegCurr;
   return(r);
}



int odbc_abre_directo(pfi,nom)
FI_DEF *pfi;
char *nom;
{
			int fd,o_fd = pfi->f_fd;
         
         pfi->f_fd = -1;
         fd = odbc_abre_ind(pfi);
         pfi->f_fd = o_fd;
         if (fd >= 0)
         {
            FgvConexion[fd_correcto(fd)]->pfi = pfi;
         }
         return(fd);
}

int odbc_cierra_directo(fd)
int fd;
{
   odbc__cierra_ind(fd); 
   return(0);
}

int odbc_lee_directo(fd,pos,pfi,buf)
int fd;
long pos;
FI_DEF *pfi;
char *buf;
{
   char buffer[2048];
   SQLRETURN rr;
   int o_fd = pfi->f_fd;
   int r,i,campo,tipo,longi,posi,k;
   
   if (pos < 1L) return(-1);
   
   if (fd_correcto(fd) < 0 || !FgvConexion[fd_correcto(fd)]) return(-1);
   
   pfi->f_fd = fd;
   sprintf(buffer,"SELECT * TOP 1 ORDER BY 1 WHERE NUMERO_DE_CONTROL=%ld",pfi->f_nclaves,pos);
   rr = SQLExecDirect(FgvConexion[fd_correcto(fd)]->h_stm,buffer,strlen(buffer));
   if (rr != SQL_SUCCESS && rr != SQL_SUCCESS_WITH_INFO)
   {
						mira_sql_error(FgvConexion[fd_correcto(fd)]->h_stm);
                  pfi->f_fd = o_fd;
                  return(-1);
   }
   
   bind_buffer(pfi,FgvConexion[fd_correcto(fd)]->h_stm);
   if (datos_a_buffer(pfi,FgvConexion[fd_correcto(fd)]->h_stm))
   {
						pfi->f_fd = o_fd;
                  return(-1);
   }
   
   
   for (campo = 0;campo < pfi->f_ncampos;campo++)
   {
						tipo = su_tipo_csql(pfi->f_campos[campo].c_tipo);
                  longi = pfi->f_campos[campo].c_long;
                  posi = pfi->f_campos[campo].c_offset;     
                  ajusta_sqltipo_(tipo,FgvConexion[fd_correcto(pfi->f_fd)]->buf+posi,buf+posi,0,longi);
   }
   
   pfi->f_fd = o_fd;   
   return(0);   
}

int odbc_lee_secuencia(fd,po,md,tamr,buf)
int fd;
long *po;
int *md;
int tamr;
char *buf;
{
   char c;
   SQLRETURN rr;
   char buffer[2048];
   int o_fd;
   int i,campo,tipo,longi,posi,k;
   FI_DEF *pfi;
   
   
   if (fd_correcto(fd) < 0 || !FgvConexion[fd_correcto(fd)]) return(-1);
   
   pfi = FgvConexion[fd_correcto(fd)]->pfi;
   if (!pfi) return(-1); // debe estar abierto con abre_directo!!
   o_fd = pfi->f_fd;
   
   if (!*md)
   {
      *md = 1;
      *po = 0L;        
   }
   
   (*po)++;
   pfi->f_fd = fd;
   sprintf(buffer,"SELECT * TOP 1 ORDER BY 1 WHERE NUMERO_DE_CONTROL>=%ld",pfi->f_nclaves,*po);
   rr = SQLExecDirect(FgvConexion[fd_correcto(fd)]->h_stm,buffer,strlen(buffer));
   if (rr != SQL_SUCCESS && rr != SQL_SUCCESS_WITH_INFO)
   {
      mira_sql_error(FgvConexion[fd_correcto(fd)]->h_stm);
      pfi->f_fd = o_fd;
      return(-1);
   }
   
   bind_buffer(pfi,FgvConexion[fd_correcto(fd)]->h_stm);
   if (datos_a_buffer(pfi,FgvConexion[fd_correcto(fd)]->h_stm))
   {
      pfi->f_fd = o_fd;
      return(-1);
   }     
   
   for (campo = 0;campo < pfi->f_ncampos;campo++)
   {
      tipo = su_tipo_csql(pfi->f_campos[campo].c_tipo);
      longi = pfi->f_campos[campo].c_long;
      posi = pfi->f_campos[campo].c_offset;     
      ajusta_sqltipo_(tipo,FgvConexion[fd_correcto(pfi->f_fd)]->buf+posi,buf+posi,0,longi);
   }
   
   *po = FgvConexion[fd_correcto(pfi->f_fd)]->RegCurr;
   pfi->f_fd = o_fd;      
   
   return(0);
}


/* !!! */
long odbc_tam_btrv(pfi)
FI_DEF *pfi;
{
   long tam;
   char buffer[2048];
   SQLRETURN r;
   if (fd_correcto(pfi->f_fd) < 0 || !FgvConexion[fd_correcto(pfi->f_fd)]) return(0L);
   
   SQLCloseCursor(m_h_stm);
   sprintf(buffer,"SELECT * FROM %s",FgvConexion[fd_correcto(pfi->f_fd)]->tabla);
   r = SQLExecDirect(m_h_stm,buffer,strlen(buffer));
   if (r != SQL_SUCCESS && r != SQL_SUCCESS_WITH_INFO)
   {	
      return(0L);
   }
   SQLRowCount(m_h_stm,&tam); // si va bien si no ... que le vamos a hacer!
   return(tam);
}

odbc_flush_ind(pfi)
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
odbc_lock_ind(pfi)
register FI_DEF *pfi;
{
   return(0);
}

/*
* unlock_ind
* desbloquea todo el indexado
* devuelve 0 si va bien, -1 si hay error
*/
odbc_unlock_ind(pfi)
register FI_DEF *pfi;
{
   return(0);
}


odbc_borra_ind(pfi)
register FI_DEF *pfi;
{
   char tmp[256];
   SQLRETURN r;
   int i,kk;
   char buffer[10000];
   
   
   
   if (!m_h_env && !conecta_al_driver())
   {
      iserrno = ENOTOPEN;
      return(-1);
   }
   
   if (m_h_stm)
   {
      SQLCloseCursor(m_h_stm);
      SQLFreeHandle(SQL_HANDLE_STMT,m_h_stm);
   }
   m_h_stm = da_stm();
   if (!m_h_stm)
   {
      aviso(0,"ODBC ATENCION: Faltan handles!!!");
      return(-1);
   }
   sprintf(buffer,"DROP TABLE %s",mi_fichero(pfi->f_fichero));
   r = SQLExecDirect(m_h_stm,buffer,strlen(buffer));
   if (r != SQL_SUCCESS && r != SQL_SUCCESS_WITH_INFO)
   {
      iserrno = ENOENT;
      return(-1);
   }
   
   
   return(0);
}


int odbc_empieza_trans()
{
   /* no soportado de momento */
}

int odbc_anula_trans()
{
   /* no soportado de momento */
}

int odbc_acaba_trans()
{
   /* no soportado de momento */
}

#endif