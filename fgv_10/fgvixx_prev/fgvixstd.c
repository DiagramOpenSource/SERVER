

#ifndef AGIBTRIEVE
#ifndef AGICISAM

#define RWIXXMAIN

#include "fgvixx.h"

/****************************************************************************/
/* Cisam error code traslation */
/****************************************************************************/

#define EDUPL          100             /* duplicate record */
#define ENOTOPEN       101             /* file not open */
#define EBADARG        102             /* invalid argument */
#define EBADKEY        103             /* invalid key description */
#define ETOOMANY       104             /* out of file descriptors */
#define EBADFILE       105             /* invalid isam file format */
#define ENOTEXCL       106             /* exclusive lock required */
#define ELOCKED        107             /* record claimed by another */
#define EKEXISTS       108             /* key already exists */
#define EPRIMKEY       109             /* primary key may not be used */
#define EENDFILE       110             /* beginning or end of file reached */
#define ENOREC         111             /* no match was found */
#define ENOCURR        112             /* there is no "current" ESTABLISHED */
#define EFLOCKED       113             /* entire file locked by another */
#define EFNAME         114             /* file name too long */
#define ENOLOK         115             /* cannot create lock file */
#define EBADMEM        116             /* memory allocation request failed */
#define EBADCOLL       117             /* bad custom collating */
#define EVIRTUAL       150             /* unable to reopen virtual file */

static int Classic_Cisam_Error[] =
{
      EBADKEY,
      ENOTOPEN,
      EEXIST,
      ENOENT,
      ETOOMANY,
      EBADMEM,
      ENOENT,  /* IXXEMALFICH de momento un fichero "malo" == es igual a que no existe */
      EBADFILE,
      EBADFILE,
      EBADARG,
      ENOTOPEN,
      EBADARG,
      EBADFILE,
      EDUPL,
      EBADFILE,
      EBADFILE,
      EBADFILE,
      ENOCURR,
      ENOREC,
      ENOENT, /* IXXETOCADO de momento un fichero "tocado" == es igual a que no existe */
      ENOREC,
      EENDFILE,
      EENDFILE,
      EFLOCKED,
      EENDFILE,
      EBADCOLL,
      ENOLOK,
      ELOCKED,
      EFLOCKED,
      0,
      0,
      0,
      0,
      0,
      0,
};


#define IXX_MAXIMO_BUSQUEDA 100

extern int IXXMAXBUF;
/****************************************************************************/

int no_read_error = 0;
int ixx_e_fich = -1;
int ixx_mantener_el_buffer = -1;

int no_report_error = 0;

extern int tuned_blck;

static int  LCLAVE_POINTERS = 10; /* adaptacion para incluir la informacion de totalizador de clave, */
static int  LCLAVE_TOTAL = 5;     /* y preparar el camino para soporte de ficheros grandes (64 bits)*/
static char VERSION_INDEX[10] = "0004   ";

struct ixxbuffer
{
   long *posclave;
   Uchar *posclaflag;
   long posibloques;
   short tamblock;
   long pos_curr;
   long *pos_buf;
   Uchar **buf;
   Uchar *flag;
   short tope_buf;
   short nclaves;
} *ixxBuffers[IXXMAXFILES*2+5] = {NULL,};


long IxxNumeroPosicion = 0L;

int ixx_buf_fin(int ixx_fd);

static int ixx_buf_ini(int ixx_fd)
{
   int i;
   int tamblock = ixxfichs[ixx_fd]->tamblock;
   int fd = ixxfichs[ixx_fd]->fd;
   short nclaves = ixxfichs[ixx_fd]->nclaves;
   
   if (IXXMAXBUF <= 0) return(0);
   
   if (fd < 0)
   {
      eret(IXXECORRUP,116,-1);
   }
   
   if (ixxBuffers[fd])
   {
      ixx_buf_fin(ixx_fd);
   }
   
   ixxBuffers[fd] = malloc(sizeof(struct ixxbuffer));
   memset(ixxBuffers[fd],0,sizeof(struct ixxbuffer));
   
   ixxBuffers[fd]->pos_buf = (long *)malloc(sizeof(long) * IXXMAXBUF);
   ixxBuffers[fd]->buf     = (char **)malloc(sizeof(char *) * IXXMAXBUF);
   ixxBuffers[fd]->flag    = malloc(IXXMAXBUF);
   memset(ixxBuffers[fd]->pos_buf,0,sizeof(long) * IXXMAXBUF);
   memset(ixxBuffers[fd]->flag,0,IXXMAXBUF);
   
   ixxBuffers[fd]->tamblock = tamblock;
   for (i = 0;i < IXXMAXBUF;i++)
   {
      ixxBuffers[fd]->buf[i] = malloc(tamblock);
   }
   ixxBuffers[fd]->pos_curr = ixx_lseek(fd,0L,SEEK_CUR);
   ixxBuffers[fd]->tope_buf = 0;
   ixxBuffers[fd]->posibloques = (long)((long)tamblock * (long)(nclaves + 1));
   ixxBuffers[fd]->posclave = malloc(nclaves * sizeof(long *));
   ixxBuffers[fd]->posclaflag = malloc(nclaves);
   memset(ixxBuffers[fd]->posclave,0,nclaves * sizeof(long *));
   memset(ixxBuffers[fd]->posclaflag,0,nclaves);
   ixxBuffers[fd]->nclaves = nclaves;
   ret( 0 );
}


static int ixx_buf_fin(int ixx_fd)
{
   int i;
   int tamblock = ixxfichs[ixx_fd]->tamblock;
   int fd = ixxfichs[ixx_fd]->fd;
   struct ixxbuffer *ixxBuffer;
   
   if (IXXMAXBUF <= 0) return(0);
   
   if (fd < 0 && !(ixxfichs[ixx_fd]->pseudo_cierra))
   {
      eret(IXXECORRUP,118,-1);
   }
   
   if (ixxfichs[ixx_fd]->pseudo_cierra)
   {
      ixxBuffer = ixxfichs[ixx_fd]->pseudo_guarda;
      ixxfichs[ixx_fd]->pseudo_guarda = NULL;
   }
   else
      ixxBuffer = ixxBuffers[fd];
   if (!ixxBuffer)
   {
      ret( 0 );
   }
   
   for (i = 0;i < IXXMAXBUF;i++)
   {
      if (i < ixxBuffer->tope_buf && ixxBuffer->flag[i])
      {
         if (ixx_mantener_el_buffer != fd)
         {
            (*ixxmen_aviso)("No se ha actualizado  !!!!!!!");
         }
         if (fd < 0)
         {
            (*ixxmen_aviso)("(XX) No se ha actualizado  !!!!!!!");
         }
         ixx_lseek(fd,ixxBuffer->pos_buf[i],SEEK_SET);
         ixx_graba(fd,ixxBuffer->buf[i],tamblock);
      }
      free(ixxBuffer->buf[i]);
   }
   
   free(ixxBuffer->pos_buf);
   free(ixxBuffer->buf);
   free(ixxBuffer->flag);
   free(ixxBuffer->posclave);
   free(ixxBuffer->posclaflag);
   free(ixxBuffer);
   
   if (!(ixxfichs[ixx_fd]->pseudo_cierra))
      ixxBuffers[fd] = NULL;
   if (ixx_mantener_el_buffer == fd) ixx_mantener_el_buffer = -1;
   ret( 0 );
}

static long ixx_buf_lseek(int fd,long pos,int modo)
{
   int tamblock;
   if (ixxBuffers[fd])
   {
      tamblock = ixxBuffers[fd]->tamblock;
      if (modo == SEEK_END)
      {
         ixxBuffers[fd]->pos_curr = ixx_lseek(fd,pos,modo);
      }
      else
         if (modo == SEEK_CUR)
         {
            ixxBuffers[fd]->pos_curr += pos;
         }
         else
            if (modo == SEEK_SET)
            {
               ixxBuffers[fd]->pos_curr = pos;
            }
            if (modo != SEEK_END && ixxBuffers[fd]->pos_curr < tamblock)
               ixx_lseek(fd,pos,modo);
            return(ixxBuffers[fd]->pos_curr);
   }
   return(ixx_lseek(fd,pos,modo));
}


static int ixx_buf_lee(int fd,Uchar *buf,int len)
{
   int i,rr;
   int tamblock;
   Uchar *tmpbuf;
   long tmplong;
   Uchar ff;
   struct ixxbuffer *ixxBuffer;
   
   if (ixxBuffers[fd])
   {
      ixxBuffer = ixxBuffers[fd];
      tamblock = ixxBuffer->tamblock;
      if (ixxBuffer->pos_curr < ixxBuffer->posibloques)
      {
         if (len == sizeof(long) && ixxBuffer->pos_curr > (long)tamblock && (ixxBuffer->pos_curr%tamblock) == 9L)
         {
            tmplong = (ixxBuffer->pos_curr-9)/tamblock;
            i = (int)tmplong - 1;
            if (i >= 0 && i < ixxBuffer->nclaves)
            {
               if (!ixxBuffer->posclaflag[i])
               {
                  ixx_lseek(fd,ixxBuffer->pos_curr,SEEK_SET);
                  rr = ixx_lee(fd,buf,len);
                  if (rr != len)
                     return(rr);
                  memcpy(&(ixxBuffer->posclave[i]),buf,len);
                  ixxBuffer->posclaflag[i] = 1;
                  return(len);
               }
               else
               {
                  memcpy(buf,&(ixxBuffer->posclave[i]),len);
                  return(len);
               }
            }
         }
         ixx_lseek(fd,ixxBuffer->pos_curr,SEEK_SET);
         return(ixx_lee(fd,buf,len));
      }
      for (i = 0;i < ixxBuffer->tope_buf;i++)
      {
         if (
            ixxBuffer->pos_curr >= ixxBuffer->pos_buf[i] &&
            (ixxBuffer->pos_curr+len) <= (ixxBuffer->pos_buf[i]+tamblock)
            )
         {
            memcpy(buf,ixxBuffer->buf[i]+(int)(ixxBuffer->pos_curr - ixxBuffer->pos_buf[i]),len);
            
            tmplong = ixxBuffer->pos_buf[i];
            ff = ixxBuffer->flag[i];
            for (;i > 0;i--)
            {
               ixxBuffer->flag[i] = ixxBuffer->flag[i-1];
               tmpbuf = ixxBuffer->buf[i];
               ixxBuffer->buf[i] = ixxBuffer->buf[i-1];
               ixxBuffer->buf[i-1] = tmpbuf;
               ixxBuffer->pos_buf[i] = ixxBuffer->pos_buf[i-1];
            }
            ixxBuffer->pos_buf[0] = tmplong;
            ixxBuffer->flag[0] = ff;
            
            return(len);
         }
      }
      /* nuevo */
      ixx_lseek(fd,ixxBuffer->pos_curr,SEEK_SET);
      if (len == tamblock)
      {
         rr = ixx_lee(fd,buf,len);
         if (rr != len)
         {
            return(rr);
         }
         if (ixxBuffer->tope_buf == IXXMAXBUF)
         {
            ixxBuffer->tope_buf--;
            rr = ixxBuffer->tope_buf;
            if (ixxBuffer->flag[rr])
            {
               ixx_lseek(fd,ixxBuffer->pos_buf[rr],SEEK_SET);
               ixx_graba(fd,ixxBuffer->buf[rr],tamblock);
            }
         }
         for (i = ixxBuffer->tope_buf;i > 0;i--)
         {
            ixxBuffer->flag[i] = ixxBuffer->flag[i-1];
            tmpbuf = ixxBuffer->buf[i];
            ixxBuffer->buf[i] = ixxBuffer->buf[i-1];
            ixxBuffer->buf[i-1] = tmpbuf;
            ixxBuffer->pos_buf[i] = ixxBuffer->pos_buf[i-1];
         }
         ixxBuffer->tope_buf++;
         memcpy(ixxBuffer->buf[0],buf,len);
         ixxBuffer->flag[0] = 0;
         ixxBuffer->pos_buf[0] = ixxBuffer->pos_curr;
         return(len);
      }
   }
   return(ixx_lee(fd,buf,len));
}


static int ixx_buf_graba(int fd,Uchar *buf,int len)
{
   int i,rr;
   int tamblock;
   Uchar *tmpbuf;
   long tmplong;
   struct ixxbuffer *ixxBuffer;
   
   if (ixxBuffers[fd])
			{
      ixxBuffer = ixxBuffers[fd];
      tamblock = ixxBuffer->tamblock;
      if (ixxBuffer->pos_curr < ixxBuffer->posibloques)
      {
         if (len == sizeof(long) && ixxBuffer->pos_curr > (long)tamblock && (ixxBuffer->pos_curr%tamblock) == 9L)
         {
            tmplong = (ixxBuffer->pos_curr-9)/tamblock;
            i = (int)tmplong - 1;
            if (i >= 0 && i < ixxBuffer->nclaves)
            {
               memcpy(&(ixxBuffer->posclave[i]),buf,len);
               ixxBuffer->posclaflag[i] = 2;
               return(len);
            }
         }
         ixx_lseek(fd,ixxBuffer->pos_curr,SEEK_SET);
         return(ixx_graba(fd,buf,len));
      }
      for (i = 0;i < ixxBuffer->tope_buf;i++)
      {
         if (
            ixxBuffer->pos_curr >= ixxBuffer->pos_buf[i] &&
            (ixxBuffer->pos_curr+len) <= (ixxBuffer->pos_buf[i]+tamblock)
            )
         {
            memcpy(ixxBuffer->buf[i]+(int)(ixxBuffer->pos_curr - ixxBuffer->pos_buf[i]),buf,len);
            
            tmplong = ixxBuffer->pos_buf[i];
            for (;i > 0;i--)
            {
               ixxBuffer->flag[i] = ixxBuffer->flag[i-1];
               tmpbuf = ixxBuffer->buf[i];
               ixxBuffer->buf[i] = ixxBuffer->buf[i-1];
               ixxBuffer->buf[i-1] = tmpbuf;
               ixxBuffer->pos_buf[i] = ixxBuffer->pos_buf[i-1];
            }
            ixxBuffer->pos_buf[0] = tmplong;
            ixxBuffer->flag[0] = 1;
            return(len);
         }
      }
      /* nuevo */
      if (len == tamblock)
      {
         if (ixxBuffer->tope_buf == IXXMAXBUF)
         {
            ixxBuffer->tope_buf--;
            rr = ixxBuffer->tope_buf;
            if (ixxBuffer->flag[rr])
            {
               ixx_lseek(fd,ixxBuffer->pos_buf[rr],SEEK_SET);
               ixx_graba(fd,ixxBuffer->buf[rr],tamblock);
            }
         }
         for (i = ixxBuffer->tope_buf;i > 0;i--)
         {
            ixxBuffer->flag[i] = ixxBuffer->flag[i-1];
            tmpbuf = ixxBuffer->buf[i];
            ixxBuffer->buf[i] = ixxBuffer->buf[i-1];
            ixxBuffer->buf[i-1] = tmpbuf;
            ixxBuffer->pos_buf[i] = ixxBuffer->pos_buf[i-1];
         }
         ixxBuffer->tope_buf++;
         memcpy(ixxBuffer->buf[0],buf,len);
         ixxBuffer->flag[0] = 1;
         ixxBuffer->pos_buf[0] = ixxBuffer->pos_curr;
         return(len);
      }
      ixx_lseek(fd,ixxBuffer->pos_curr,SEEK_SET);
			}
   return(ixx_graba(fd,buf,len));
}

static int ixx_buf_flush(int fd)
{
   int i;
   int tamblock;
   long tmpos;
   
   if (ixx_mantener_el_buffer == fd) ret( 0 );
   
   if (ixxBuffers[fd])
			{
      tamblock = ixxBuffers[fd]->tamblock;
      for (i = 0;i < ixxBuffers[fd]->nclaves;i++)
      {
         if (ixxBuffers[fd]->posclaflag[i] == 2)
         {
            tmpos = (long)(i+1) * tamblock;
            tmpos += 9L;
            ixx_lseek(fd,tmpos,SEEK_SET);
            ixx_graba(fd,&(ixxBuffers[fd]->posclave[i]),sizeof(long));
            ixxBuffers[fd]->posclaflag[i] = 1;
         }
      }
      for (i = 0;i < ixxBuffers[fd]->tope_buf;i++)
      {
         if (ixxBuffers[fd]->flag[i])
         {
            ixx_lseek(fd,ixxBuffers[fd]->pos_buf[i],SEEK_SET);
            ixx_graba(fd,ixxBuffers[fd]->buf[i],tamblock);
            ixxBuffers[fd]->flag[i] = 0;
         }
      }
			}
   ret( 0 );
}

static int ixx_buf_anula(int fd)
{
   int i;
   
   if (ixx_mantener_el_buffer == fd) ret( 0 );
   
   if (ixxBuffers[fd])
			{
      for (i = 0;i < ixxBuffers[fd]->tope_buf;i++)
      {
         if (ixxBuffers[fd]->flag[i])
         {
            (*ixxmen_aviso)("No se ha actualizado!!!");
         }
      }
      ixxBuffers[fd]->tope_buf = 0;
      memset(ixxBuffers[fd]->posclave,0,ixxBuffers[fd]->nclaves * sizeof(long *));
      memset(ixxBuffers[fd]->posclaflag,0,ixxBuffers[fd]->nclaves);
			}
   ret( 0 );
}


int IXXerror(s,e,fd)
int s,e,fd;
{
   char tmp[256];
   
   if (no_report_error) return(0);
   
   if (fd > -1)
      ixx_cierra(fd);
   
   
   if (ixx_incidencia && s != IXXENOEN && s != IXXENOREC && s != IXXENOKEY && s != IXXEPRIME &&
      s != IXXEFINAL && s != IXXELOCKED)
			{
      sprintf(tmp,"CI[%d][%d]FICHERO ",s,e);
      if (ixx_e_fich  >= 0 && ixxfichs[ixx_e_fich] && ixxfichs[ixx_e_fich]->nombreixx && ixxquita_path)
      {
         strcat(tmp,(*ixxquita_path)(ixxfichs[ixx_e_fich]->nombreixx));
      }
      (*ixx_incidencia)(0,tmp);
			}
   
   if (s == IXXECORRUP && !no_corrup_error)
			{
      sprintf(tmp,"Aviso de Control de Averia en el Indice [%d]",e);
      if (ixxmen_aviso)
      {
         (*ixxmen_aviso)(tmp);
      }
			}
   else
      if (s == IXXEREAD && !no_read_error)
      {
         sprintf(tmp,"Error en lectura de fichero, posible disco lleno o corrupto [%d] ",e);
         if (ixxmen_aviso)
         {
            (*ixxmen_aviso)(tmp);
         }
      }
      else
         if (s == IXXEWRITE)
         {
            sprintf(tmp,"ATENCION: ¡¡¡¡FATAL!!!! UNIDAD O PARTICION LLENA [%d]",e);
            if (ixxmen_aviso)
            {
               (*ixxmen_aviso)(tmp);
            }
         }
         else
            if (s == IXXELSEEK)
            {
               sprintf(tmp,"Error en posicionamiento de fichero, posible disco corrupto [%d]",e);
               if (ixxmen_aviso)
               {
                  (*ixxmen_aviso)(tmp);
               }
            }
            
            ixxstatus = s; /* codigo de error */
            ixxerrno = e;  /* codigo de error extendido */
            /* conversion a iserrno "CLASSIC" */
            if (s == IXXENOEN && e == EMFILE)
               iserrno = EMFILE;
            else
               if (s >= 5001)
                  iserrno = Classic_Cisam_Error[s - 5001];
               
               return(-1);
}


int ixx_begin_transaction()
{
			
			
			
}

void AjustaPunterosSegunDatos(int modo)
{
			
			if (modo >= 0 && modo < IXXMAXFILES && ixxfichs[modo])
         {
            switch (ixxfichs[modo]->tipo_datos)
            {
            case 0:
               ixx_datos_dbf = 0;
               default_dbf_id = 0;	  
               break;
            case 1:
               ixx_datos_dbf = 1;
               default_dbf_id = 0;	  
               break;
            case 2:
               ixx_datos_dbf = 1;
               default_dbf_id = 1;	  
               break;
            case 3:
               ixx_datos_dbf = 1;
               default_dbf_id = 2;
               break;
            }    
         }
         

         if (ixx_datos_dbf == 1)
         {
            ixxdat_abre = dbf_abre;
            ixxdat_cierra = dbf_cierra;
            ixxdat_lee = dbf_lee;
            ixxdat_graba = dbf_graba;
            ixxdat_borra = dbf_borra;
            ixxdat_tamfichero = dbf_tamfichero;     
         }
         else
         {
            ixxdat_abre = dat_abre;
            ixxdat_cierra = dat_cierra;
            ixxdat_lee = dat_lee;
            ixxdat_graba = dat_graba;
            ixxdat_borra = dat_borra;
            ixxdat_tamfichero = dat_tamfichero;     
         }

         if (!ixx_haydiu && ixx_ciudiu)
         {
            carga_lista_diu();
         }

         if (ixx_haydiu)
         {
            ixxdiu_abre   = ixxdat_abre;
            ixxdiu_cierra = ixxdat_cierra;
            ixxdiu_graba  = ixxdat_graba;
            ixxdiu_borra  = ixxdat_borra;

            ixxdat_abre   = diu_abre;
            ixxdat_cierra = diu_cierra;            
            ixxdat_graba  = diu_graba;
            ixxdat_borra  = diu_borra;
         }

}


int ixx_abre_indice(nombre,modo,descamp,defectos,lfich,dtabla)
char *nombre;
int modo;
char **descamp;
char *defectos;
int lfich;
FgvStdTabla *dtabla;
{
    	IXXCABEZA header;
         char nombreixx[81];
         char nombredat[81];
         short i;
         int l;
         int ixx_fd;
         int fd;
         short tamblock;
         long pos;
         int ojo_norec = 0;
         unsigned short autentificacion;
         int kkk;
         
         
         AjustaPunterosSegunDatos(-1);
         
         
         if (ixx_inicial == 1)
         {
#ifdef CONTROLNLM
            pprint(2501,"*ABRE01*");
            sleep(1);
#endif
            ixx_inicial = 0;
            ixx_ojo_recursion = 0;    
            
            for (ixx_fd = 0;ixx_fd < IXXMAXFILES;ixx_fd++)
            {
               ixxfichs[ixx_fd] = NULL;
            }
            memset(_Posblock,0,sizeof(short)*IXXMAXLEVEL);
            for (ixx_fd = 0;ixx_fd < (IXXMAXFILES*2+5);ixx_fd++)
            {
               ixxBuffers[ixx_fd] = NULL;
            }
#ifdef CONTROLNLM
            pprint(2501,"        ");
            sleep(1);
#endif
         }
         
         
         ixx_e_fich = -1;
         
         if (ixx_ojo_recursion)
         {
            eret(IXXERECURS,0,-1);
         }
         ixx_ojo_recursion = 1;
         
         isrecnum = 0L;
         
         i = -1;
         l = -1;
         
         for (ixx_fd = 0;ixx_fd < IXXMAXFILES;ixx_fd++)
         {
            if (!ixxfichs[ixx_fd])
               i = ixx_fd;
            else
            {
               if (ixxfichs[ixx_fd]->pseudo_cierra)
               {
                  if (ixxfichs[ixx_fd]->nombredat && !strcmp(nombre,ixxfichs[ixx_fd]->nombredat) && ixxfichs[ixx_fd]->nombreixx)
                  {
#ifdef CONTROLNLM
                     pprint(2501,"*ABRE02*");
                     sleep(1);
#endif
                     ixxfichs[ixx_fd]->pseudo_cierra = 0;
                     if (modo == IXXEXCLUYE)
                        fd = ixx_abre_exclu(ixxfichs[ixx_fd]->nombreixx);
                     else
                        fd = ixx_abre_share(ixxfichs[ixx_fd]->nombreixx);
                     if (fd < 0)
                     {
                        if ((errno == EACCES || errno == EAGAIN || errno == 13 || errno == 35))
                        {
                           ixx_ojo_recursion = 0;
                           eret(IXXEFLOCK,errno,-1);
                        }
                        else
                        {
                           ixx_fuera_cache(ixxfichs[ixx_fd]->nombredat);
                           ixx_ojo_recursion = 0;
                           eret(IXXENOEN,errno,-1);
                        }
                     }
                     /*******/
                     
                     if (ixx_lseek(fd,OIDENTI,SEEK_SET)== -1L)
                     {
                        ixx_ojo_recursion = 0;
                        eret(IXXELSEEK,errno,fd);
                     }
                     if (ixx_lee(fd,&autentificacion,sizeof(short)) != sizeof(short))
                     {
                        ixx_ojo_recursion = 0;
                        eret(IXXEREAD,errno,fd);
                     }
                     if (autentificacion != ixxfichs[ixx_fd]->cabecera.autentificacion)
                     {
                        ixx_cierra(fd);
                        l = ixx_fd;
                        i = -1;
#ifdef CONTROLNLM
                        pprint(2501,"        ");
                        sleep(1);
#endif
                        break;
                     }
                     /*******/
                     ixxfichs[ixx_fd]->fd = fd;
                     ixxBuffers[fd] = ixxfichs[ixx_fd]->pseudo_guarda;
                     if (ixxBuffers[fd])
                     {
                        ixxBuffers[fd]->pos_curr = 0L;
                     }
                     ixxfichs[ixx_fd]->pseudo_guarda = NULL;
                     ixxfichs[ixx_fd]->openmode = modo;
                     ixxfichs[ixx_fd]->fd_dat = (*ixxdat_abre)(ixxfichs[ixx_fd]->nombredat,NULL,NULL,NULL,NULL,NULL,&(ixxfichs[ixx_fd]->dtabla));
                     ixx_ojo_recursion = 0;
                     _e_log_open++;
#ifdef CONTROLNLM
                     pprint(2501,"        ");
                     sleep(1);
#endif
                     return(ixx_fd);
                  }
                  l = ixx_fd;
               }
            }
         }
         
         
         
         if (i == -1 && l == -1)
         {
            ixx_ojo_recursion = 0;
            eret(IXXEMUCHOS,0,-1);
         }
         
         if (i == -1)
         {
            ixx_cierra_indice(l);
            i = l;
         }
         
         ixx_fd = i;
         
         if (modo != IXXCOMPARTE && modo != IXXSOLOLEE && modo != IXXEXCLUYE)
         {
            ixx_ojo_recursion = 0;
            eret(IXXENOSUPORT,0,-1);
         }
         
         
#ifdef CONTROLNLM
         pprint(2501,"*ABRE03*");
         sleep(1);
#endif
         sprintf(nombreixx,"%s.ixx",nombre);
         if (modo == IXXEXCLUYE)
            fd = ixx_abre_exclu(nombreixx);
         else
            fd = ixx_abre_share(nombreixx);
         
#ifdef CONTROLNLM
         pprint(2501,"        ");
         sleep(1);
#endif
         
         if (fd < 0)
         {
            if ((errno == EACCES || errno == EAGAIN || errno == 13 || errno == 35))
            {
               ixx_ojo_recursion = 0;
               eret(IXXEFLOCK,errno,-1);
            }
            else
            {
               ixx_ojo_recursion = 0;
               eret(IXXENOEN,errno,-1);
            }
         }
         
#ifdef CONTROLNLM
         pprint(2501,"*ABRE04*");
         sleep(1);
#endif
         
         if (ixx_bloquea_escritura(fd,modo))
         {
#ifdef CONTROLNLM
            pprint(2501,"!ABRE04!");
            sleep(1);
#endif
            
            ixx_cierra(fd);
            ixx_ojo_recursion = 0;
            ret( -1 );
         }
#ifdef CONTROLNLM
         pprint(2501,"        ");
         sleep(1);
#endif
         
#ifdef CONTROLNLM
         pprint(2501,"*ABRE05*");
         sleep(1);
#endif
         
         if (ixx_lseek(fd,0L,SEEK_SET)== -1L)
         {
            ixx_desbloquea_escritura(fd,modo);
            ixx_ojo_recursion = 0;
            eret(IXXELSEEK,errno,fd);
         }
         
         if ((l = ixx_lee(fd,&header,sizeof(header))) != sizeof(header))
         {
         /*  if (!l)
            {*/
            ixx_desbloquea_escritura(fd,modo);
            ixx_cierra(fd);
            ixx_ojo_recursion = 0;
            unlink(nombreixx);
            eret(IXXENOEN,ENOENT,-1);
            /*          }
            else
            {
            ixx_desbloquea_escritura(fd,modo);
            ixx_ojo_recursion = 0;
            eret(IXXEMALFICH,0,fd);
         } */
         }
#ifdef CONTROLNLM
         pprint(2501,"        ");
         sleep(1);
#endif
         
         
         
         
         if (memcmp(header.file_id,"ixxfgv",6))
         {
            ixx_desbloquea_escritura(fd,modo);
            ixx_ojo_recursion = 0;
            eret(IXXEMALFICH,1,fd);
         }
         
         if (memcmp(header.file_ver,VERSION_INDEX,4))
         {
            ixx_desbloquea_escritura(fd,modo);
            ixx_ojo_recursion = 0;
            eret(IXXEMALFICH,2,fd);
         }
         
         if (header.file_ver[4] != IXXTIPO_PROCESADOR)
         {
            ixx_desbloquea_escritura(fd,modo);
            ixx_ojo_recursion = 0;
            eret(IXXEMALFICH,3,fd);
         }
         
         ixxfichs[ixx_fd] = malloc(sizeof(IXXFILE));
         if (!ixxfichs[ixx_fd])
         {
            ixx_desbloquea_escritura(fd,modo);
            ixx_ojo_recursion = 0;
            eret(IXXENOMEM,errno,fd);
         }
         
         ixxfich = ixxfichs[ixx_fd];
         
         /* cargar cabecera */
         ixxfich->fd = fd;
         ixxfich->clave_activa = 0;
         ixxfich->trozo_activa = 0;
         ixxfich->nclaves = header.nkeys;
         ixxfich->claves = malloc(sizeof(IXXCLAVE) * ixxfich->nclaves);
         memset(ixxfich->claves,0,sizeof(IXXCLAVE) * ixxfich->nclaves);
         tamblock = header.tamblock;
         ixxfich->tamblock = tamblock;
         ixxfich->tamreg=header.tamreg;
         ixxfich->reg_cur=0L;
         ixxfich->blo_cur=0L;
         ixxfich->pos_cur=0;
         ixxfich->openmode = modo;
         ixxfich->numero_bloqueos = 0;
         ixxfich->nombreixx = NULL;
         ixxfich->nombredat = NULL;
         ixxfich->contenido = NULL;
         ixxfich->contenidos[0] = 0;
         ixxfich->ocupacion_total = 0L;
         ixxfich->control_numblocks = header.nblocks;
         ixxfich->pseudo_cierra = 1;
         ixxfich->pseudo_guarda = NULL;
         ixxfich->dtabla = *dtabla;
         ixxfich->tipo_datos = ixx_datos_dbf + default_dbf_id;
         ixxfich->flags_operacion = 0;
         dtabla->Campo = NULL; /* apropiacion de la memoria (!!) */
         
         memcpy(&(ixxfich->cabecera),&header,sizeof(IXXCABEZA));
         ixxfich->maximo_registros = LCKDZONAALL;
         /* calculo del maximo de bloques */
         ixxfich->maximo_bloques = IXXBLOCKEND / tamblock;
         
         if (!ixxfich->claves)
         {
            ixx_desbloquea_escritura(fd,modo);
            ixx_ojo_recursion = 0;
            eret(IXXENOMEM,errno,fd);
         }
         
         if (ixxtamblock < tamblock)
         {
            if (ixxtamblock)
            {
               free(ixxbloque);
               free(ixxbloque2);
               free(ixxbloque3);
               free(ixxbloque4);
               free(ixxbloque5);
            }
            ixxbloque = NULL;
         }
         if (!ixxbloque)
         {
            ixxbloque = malloc(tamblock+tamblock/6);
            ixxbloque2 = malloc(tamblock+tamblock/6);
            ixxbloque3 = malloc(tamblock+tamblock/6);
            ixxbloque4 = malloc(tamblock+tamblock/6);
            ixxbloque5 = malloc(tamblock+tamblock/6);
            ixxtamblock = tamblock;
         }
         
         if (!ixxbloque || !ixxbloque2 || !ixxbloque3 || !ixxbloque4 || !ixxbloque5)
         {
            ixx_desbloquea_escritura(fd,modo);
            ixx_ojo_recursion = 0;
            eret(IXXENOMEM,errno,fd);
         }
         
         autentificacion = header.tamblock ^ header.nkeys ^ header.tamreg;
         
#ifdef CONTROLNLM
         pprint(2501,"*ABRE06*");
         sleep(1);
#endif
         
         
         for (i = 0;i < ixxfich->nclaves;i++)
         {
            pos = tamblock;pos *= (i+1);
            if (ixx_lseek(fd,pos,SEEK_SET)== -1L)
            {
               ixx_desbloquea_escritura(fd,modo);
               ixx_ojo_recursion = 0;
               eret(IXXELSEEK,errno,fd);
            }
            if (ixx_lee(fd,ixxbloque,tamblock) != tamblock)
            {
               ixx_desbloquea_escritura(fd,modo);
               ixx_ojo_recursion = 0;
               eret(IXXEREAD,errno,fd);
            }
            if (memcmp(ixxbloque,"ixxkeyd",7) || memcmp(ixxbloque+7,&i,2))
            {
               ixx_desbloquea_escritura(fd,modo);
               ixx_ojo_recursion = 0;
               eret(IXXEMALFICH,4+i,fd);
            }
            memcpy(&(ixxfich->claves[i].clave_len),ixxbloque+13,2);
            memcpy(&(ixxfich->claves[i].clave_num),ixxbloque+15,2);
            memcpy(&(ixxfich->claves[i].clave_tipo),ixxbloque+17,2);
            memcpy(&(ixxfich->claves[i].clave_ncampos),ixxbloque+19,2);
            if (ixxfich->claves[i].clave_num != ((tamblock-20) / (ixxfich->claves[i].clave_len+LCLAVE_POINTERS)))
            {
               ixx_desbloquea_escritura(fd,modo);
               ixx_ojo_recursion = 0;
               eret(IXXECORRUP,1,fd);
            }
            ixxfich->claves[i].clave_campos = malloc(sizeof(IXXCAMPO) * ixxfich->claves[i].clave_ncampos);
            if (!ixxfich->claves[i].clave_campos)
            {
               ixx_desbloquea_escritura(fd,modo);
               ixx_ojo_recursion = 0;
               eret(IXXENOMEM,errno,fd);
            }
            memcpy(ixxfich->claves[i].clave_campos,ixxbloque+21,sizeof(IXXCAMPO) * ixxfich->claves[i].clave_ncampos);
            for (kkk = 0;kkk < ((tamblock/2)-1);kkk++)
            {
               autentificacion ^= *(((unsigned short *)ixxbloque)+kkk);
            }
         }
         
         
         if (autentificacion != header.autentificacion)
         {
            header.autentificacion = autentificacion;
            if (ixx_lseek(fd,OIDENTI,SEEK_SET)== -1L)
            {
               ixx_ojo_recursion = 0;
               ixx_desbloquea_escritura(fd,modo);
               eret(IXXELSEEK,errno,fd);
            }
            if (ixx_graba(fd,&autentificacion,sizeof(short)) != sizeof(short))
            {
               ixx_ojo_recursion = 0;
               ixx_desbloquea_escritura(fd,modo);
               eret(IXXEWRITE,errno,fd);
            }
         }
         
         if (!datos_dim)
         {
            datos_dim1 = ixxfich->nclaves;
            datos_dim = (short *)malloc(sizeof(short) * datos_dim1);
            memset(datos_dim,0,sizeof(short) * datos_dim1);
            Datos = (Uchar **)malloc(sizeof(char *) * datos_dim1);
            Datos2 = (Uchar **)malloc(sizeof(char *) * datos_dim1);
         }
         if (datos_dim1 < ixxfich->nclaves)
         {
            datos_dim = (short *)realloc(datos_dim,sizeof(short) * ixxfich->nclaves);
            Datos = (Uchar **)realloc(Datos,sizeof(char *) * ixxfich->nclaves);
            Datos2 = (Uchar **)realloc(Datos2,sizeof(char *) * ixxfich->nclaves);
            memset(((char *)datos_dim)+sizeof(short) * datos_dim1,0,sizeof(short) * (ixxfich->nclaves-datos_dim1));
            datos_dim1 = ixxfich->nclaves;
         }
         
         ixxfich->DatosCurr = (Uchar **)malloc(sizeof(char *) * ixxfich->nclaves);
         ixxfich->NumeroPosicion = (long *)malloc(sizeof(long) * (ixxfich->nclaves+2));
         ixxfich->max_bloqueos = IXXMAXLOCKS;
         ixxfich->lista_bloqueos = (long *)malloc(sizeof(long)*ixxfich->max_bloqueos);
         if (!ixxfich->DatosCurr || !ixxfich->NumeroPosicion || !ixxfich->lista_bloqueos)
         {
            ixx_desbloquea_escritura(fd,modo);
            ixx_ojo_recursion = 0;
            eret(IXXENOMEM,errno,fd);
         }
         
         for (i = 0;i < ixxfich->nclaves;i++)
         {
            ixxfich->DatosCurr[i] = malloc(ixxfich->claves[i].clave_len + 10);
            if (!ixxfich->DatosCurr[i])
            {
               ixx_desbloquea_escritura(fd,modo);
               ixx_ojo_recursion = 0;
               eret(IXXENOMEM,errno,fd);
            }
            
            if (ixxfich->claves[i].clave_len > datos_dim[i])
            {
               if (datos_dim[i])
               {
                  free(Datos[i]);
                  free(Datos2[i]);
               }
               datos_dim[i] = ixxfich->claves[i].clave_len;
               Datos[i] = malloc(datos_dim[i] + 10);
               Datos2[i] = malloc(datos_dim[i] + 10);
            }
         }
         
#ifdef CONTROLNLM
         pprint(2501,"        ");
         sleep(1);
#endif
         
         /**** abrir datos ***********/

         /* printf("punto 5\n");getchar(); */
#ifdef CONTROLNLM
         pprint(2501,"*ABRE07*");
         sleep(1);
#endif
         
         ixxfich->fd_dat = (*ixxdat_abre)(nombre,descamp,defectos,&(ixxfich->tamreg),ixxfich,&ojo_norec,&(ixxfich->dtabla));



         /* printf("punto 6\n");getchar(); */
#ifdef CONTROLNLM
         pprint(2501,"        ");
         sleep(1);
#endif
         
         if (ixxfich->fd_dat < 0)
         {
            i = errno;
            ixx_desbloquea_escritura(fd,modo);
            ixx_ojo_recursion = 0;
            ixx_cierra_indice(ixx_fd);
            eret(IXXEABREDAT,i,-1);
         }
         if (ixxfich->tamreg)
         {
            if (ixxfich->tamreg != ixxfich->cabecera.tamreg)
            {
               /* se podrian comprobar las claves aqui !!!! */
               ixxfich->cabecera.tamreg = ixxfich->tamreg;

               /*autentificacion = ixxfich->cabecera.tamblock ^ ixxfich->cabecera.nkeys ^ ixxfich->cabecera.tamreg;*/

               if (ixx_lseek(fd,OTAMREG,SEEK_SET)== -1L)
               {
                  ixx_desbloquea_escritura(fd,modo);
                  ixx_ojo_recursion = 0;
                  ixx_cierra_indice(ixx_fd);
                  eret(IXXELSEEK,errno,-1);
               }
               if (ixx_graba(fd,&(ixxfich->tamreg),sizeof(short)) != sizeof(short))
               {
                  ixx_desbloquea_escritura(fd,modo);
                  ixx_ojo_recursion = 0;
                  ixx_cierra_indice(ixx_fd);
                  eret(IXXEWRITE,errno,-1);
               }
            }
            /****************************/
            
            if (len_dat_cur < ixxfich->tamreg)
            {
               if (Dat_Cur) free(Dat_Cur);
               Dat_Cur = malloc(ixxfich->tamreg + 10);
               len_dat_cur = ixxfich->tamreg;
            }
            
            /* calculo del maximo de registros permitido ajustado a la realidad */
            if (ixxdat_max_reg)
               (*ixxdat_max_reg)(ixxfich);
            
            if (ixx_lseek(fd,OOPENID,SEEK_SET)== -1L)
            {
               ixx_desbloquea_escritura(fd,modo);
               ixx_ojo_recursion = 0;
               ixx_cierra_indice(ixx_fd);
               eret(IXXELSEEK,errno,-1);
            }
            if (ixx_lee(fd,&(ixxfich->openid),sizeof(short)) != sizeof(short))
            {
               ixx_desbloquea_escritura(fd,modo);
               ixx_ojo_recursion = 0;
               ixx_cierra_indice(ixx_fd);
               eret(IXXEREAD,errno,-1);
            }
            ixxfich->openid++;
            
            /* ATENCION muchas operturas-cierre del mismo fichero pueden provocar coincidencia
            al no haber (de momento) bloqueo por apertura */
            if (ixx_lseek(fd,OOPENID,SEEK_SET)== -1L)
            {
               ixx_desbloquea_escritura(fd,modo);
               ixx_ojo_recursion = 0;
               ixx_cierra_indice(ixx_fd);
               eret(IXXELSEEK,errno,-1);
            }
            if (ixx_graba(fd,&(ixxfich->openid),sizeof(short)) != sizeof(short))
            {
               ixx_desbloquea_escritura(fd,modo);
               ixx_ojo_recursion = 0;
               ixx_cierra_indice(ixx_fd);
               eret(IXXEWRITE,errno,-1);
            }
            
            
            /* comprobar que el tamaño del fichero corresponda con lo esperado tanto de indice
            como de datos */
            
            if ((*ixxdat_tamfichero)(ixxfich->fd_dat,ixxfich->cabecera.nrecords,ixxfich->tamreg,NULL))
            {
               header.touch = 1;
               ixxtouch(fd);
            }
            
            pos = ixx_lseek(fd,0L,SEEK_END);
            if (pos < (ixxfich->cabecera.nblocks * ixxfich->tamblock))
            {
               header.touch = 1;
               ixxtouch(fd);
            }
         }
         
         
         if (ixx_desbloquea_escritura(fd,modo))
         {
            ixx_ojo_recursion = 0;
            ixx_cierra_indice(ixx_fd);
            ret(-1);
         }
         ixxfich->nombredat = malloc(strlen(nombre)+1);
         if (!ixxfich->nombredat)
         {
            ixx_ojo_recursion = 0;
            ixx_cierra_indice(ixx_fd);
            eret(IXXENOMEM,0,-1);
         }
         strcpy(ixxfich->nombredat,nombre);
         ixxfich->nombreixx = malloc(strlen(nombreixx)+1);
         if (!ixxfich->nombreixx)
         {
            ixx_ojo_recursion = 0;
            ixx_cierra_indice(ixx_fd);
            eret(IXXENOMEM,0,-1);
         }
         strcpy(ixxfich->nombreixx,nombreixx);
         ixx_ojo_recursion = 0;
         
         
         if (ixx_buf_ini(ixx_fd) < 0)
         {
            ixx_cierra_indice(ixx_fd);
            ret(-1);
         }
         
         ixxfich->pseudo_cierra = 0;
         
         /* un fichero "tocado" se ha de reconstruir */
         if (ixxfich->tamreg && descamp && !ojo_norec && header.touch)
         {
            ixx_reconstruye(ixx_fd);
            if (ixxtocado(fd))
            {
               ixxfich->pseudo_cierra = 1;
               ixx_cierra_indice(ixx_fd);
               eret(IXXETOCADO,0,-1);
            }
         }
         
         ixxfich->ocupacion_total = ixx_lseek(fd,0L,SEEK_END) + ixx_lseek(ixxfich->fd_dat,0L,SEEK_END);
         
         _e_log_open++;
         ixx_e_fich = ixx_fd;
         
         bret(ixx_fd);
}


/* Bloqueos */
int ixx_bloquea_escritura(fd,modo)
int fd,modo;
{
   int _oerr;
   char c;
   
   if (modo == IXXEXCLUYE) ret( 0 );
   
   
   while(-1)
   {
      if (ixx_lseek(fd,LCKREZONAINI,SEEK_SET) == -1L)
      {
         eret(IXXELSEEK,errno,-1);
      }
      if (ixx_lee(fd,&c,1) == 1) break;
      if (errno != EACCES && errno != EAGAIN && errno != 13 && errno != 35)
      {
         eret(IXXEREAD,errno,-1);
      }
      if (ixxind_recespera)
         (*ixxind_recespera)(0);
      if (ixxdat_micro_espera)
         (*ixxdat_micro_espera)();
   }
   if (ixxind_recespera)
      (*ixxind_recespera)(1);
   
   
   
   /* primero bloquear flag de peticion de escritura con espera */
   if (ixx_lck_lseek(fd,LCKPEZONAINI,SEEK_SET) == -1L)
   {
      eret(IXXELSEEK,errno,-1);
   }
   
   while(-1) /* se espera (de momento asi) indefinidamente */
   {
      if (!ixx_lock(fd,LK_WAIT,1L)) break;
      if (errno != EACCES && errno != EDEADLOCK && errno != EAGAIN  && errno != 13 && errno != 35)
      {
         eret(IXXENOSUPORT,errno,-1);
      }
      if (ixxdat_micro_espera)
         (*ixxdat_micro_espera)();
   }
   
   /* bloquear region de "lectores" */
   if (ixx_lck_lseek(fd,LCKIZONAINI,SEEK_SET) == -1L)
   {
      eret(IXXELSEEK,errno,-1);
   }
   while(-1) /* se espera (de momento asi) indefinidamente */
   {
      if (!ixx_lock(fd,LK_WAIT,LCKIZONAALL)) break;
      if (errno != EACCES && errno != EDEADLOCK && errno != EAGAIN  && errno != 13 && errno != 35)
      {
         _oerr = errno;
         ixx_lck_lseek(fd,LCKPEZONAINI,SEEK_SET);
         ixx_lock(fd,LK_UNLCK,1L);
         eret(IXXENOSUPORT,_oerr,-1);
      }
      if (ixxdat_micro_espera)
         (*ixxdat_micro_espera)();
   }
   
   if (ixx_lck_lseek(fd,LCKBEZONAINI,SEEK_SET) == -1L)
   {
      _oerr = errno;
      ixx_lck_lseek(fd,LCKPEZONAINI,SEEK_SET);
      ixx_lock(fd,LK_UNLCK,1L);
      ixx_lck_lseek(fd,LCKIZONAINI,SEEK_SET);
      ixx_lock(fd,LK_UNLCK,LCKIZONAALL);
      eret(IXXELSEEK,_oerr,-1);
   }
   
   if (ixx_lock(fd,LK_NBLCK,1L)) /* aqui sin espera!!! */
   {
      _oerr = errno;
      ixx_lck_lseek(fd,LCKPEZONAINI,SEEK_SET);
      ixx_lock(fd,LK_UNLCK,1L);
      ixx_lck_lseek(fd,LCKIZONAINI,SEEK_SET);
      ixx_lock(fd,LK_UNLCK,LCKIZONAALL);
      eret(IXXENOSUPORT,5000+_oerr,-1);
   }
   
   ret( 0 );
}

int ixx_bloquea_lectura(fd,openid,modo)
int fd;
unsigned short openid;
int modo;
{
   int _oerr;
   char c;
   
   if (modo == IXXEXCLUYE) ret( 0 );
   
   
   while(-1)
			{
      if (ixx_lseek(fd,LCKREZONAINI,SEEK_SET) == -1L)
      {
         eret(IXXELSEEK,errno,-1);
      }
      if (ixx_lee(fd,&c,1) == 1) break;
      if (errno != EACCES && errno != EAGAIN  && errno != 13 && errno != 35)
      {
         eret(IXXEREAD,errno,-1);
      }
      if (ixxind_recespera)
         (*ixxind_recespera)(0);
      if (ixxdat_micro_espera)
         (*ixxdat_micro_espera)();
			}
   if (ixxind_recespera)
      (*ixxind_recespera)(1);
   
   
   
   while(-1) /* se espera (de momento asi) indefinidamente */
			{
      if (ixx_lseek(fd,LCKPEZONAINI,SEEK_SET) == -1L)
      {
         eret(IXXELSEEK,errno,-1);
      }
      if (ixx_lee(fd,&c,1) == 1) break;
      if (errno != EACCES && errno != EAGAIN  && errno != 13 && errno != 35)
      {
         eret(IXXEREAD,errno,-1);
      }
      if (ixxdat_micro_espera)
         (*ixxdat_micro_espera)();
			}
   if (ixx_lck_lseek(fd,LCKIZONAINI+openid,SEEK_SET) == -1L)
			{
      eret(IXXELSEEK,errno,-1);
			}
   while(-1) /* se espera (de momento asi) indefinidamente (si dos modos de lectura coincidieran se turnarian)*/
			{
      if (!ixx_lock(fd,LK_WAIT,1L)) break;
      if (errno != EACCES && errno != EDEADLOCK && errno != EAGAIN  && errno != 13 && errno != 35)
      {
         _oerr = errno;
         ixx_lck_lseek(fd,LCKPEZONAINI,SEEK_SET);
         ixx_lock(fd,LK_UNLCK,1L);
         eret(IXXENOSUPORT,_oerr,-1);
      }
      if (ixxdat_micro_espera)
         (*ixxdat_micro_espera)();
			}
   
   ret( 0 );
}


int ixx_desbloquea_escritura(fd,modo)
int fd,modo;
{
			int _oerr = -1;
         int queerr = -1;
         
         if (modo == IXXEXCLUYE) ret( 0 );
         
         if (ixx_lck_lseek(fd,LCKBEZONAINI,SEEK_SET) == -1L)
         {
            queerr = IXXELSEEK;
            _oerr = errno;
         }
         else
            if (ixx_lock(fd,LK_UNLCK,1L))
            {
               _oerr = errno;
               queerr = IXXENOSUPORT;
            }
            
            
            if (ixx_lck_lseek(fd,LCKIZONAINI,SEEK_SET) == -1L)
            {
               if (queerr == -1)
               {
                  queerr = IXXELSEEK;
                  _oerr = errno;
               }
            }
            else
               if (ixx_lock(fd,LK_UNLCK,LCKIZONAALL))
               {
                  if (queerr == -1)
                  {
                     _oerr = errno;
                     queerr = IXXENOSUPORT;
                  }
               }
               
               if (ixx_lck_lseek(fd,LCKPEZONAINI,SEEK_SET) == -1L)
               {
                  if (queerr == -1)
                  {
                     queerr = IXXELSEEK;
                     _oerr = errno;
                  }
               }
               else
                  if (ixx_lock(fd,LK_UNLCK,1L))
                  {
                     if (queerr == -1)
                     {
                        _oerr = errno;
                        queerr = IXXENOSUPORT;
                     }
                  }
                  
                  if (queerr != -1)
                  {
                     eret(queerr,_oerr,-1);
                  }
                  ret( 0 );
}


int ixx_desbloquea_lectura(fd,openid,modo)
int fd;
unsigned short openid;
int modo;
{
   
   if (modo == IXXEXCLUYE) ret( 0 );
   
   if (ixx_lck_lseek(fd,LCKIZONAINI+openid,SEEK_SET) == -1L)
   {
      eret(IXXELSEEK,errno,-1);
   }
   if (ixx_lock(fd,LK_UNLCK,1L))
   {
      eret(IXXENOSUPORT,errno,-1);
   }
   
   ret( 0 );
}


int ixx_reg_esta_bloqueado(reg) /* 1 = por mi , 2 = por otro 0 = no lo esta */
long reg;
{
   int i;
   int fd = ixxfich->fd;
   
   if (ixxfich->openmode == IXXEXCLUYE) ret( 1 ); /* siempre por uno mismo en caso de bloqueo exclusivo */
   
   if (reg <= 0L || reg > ixxfich->cabecera.nrecords)
   {
						eret(IXXECORRUP,109,-1);
   }
   
   for (i = 0;i < ixxfich->numero_bloqueos;i++)
   {
						if (reg == ixxfich->lista_bloqueos[i])
                     ret( 1 ); /* bloqueado por mi */
   }
   reg--;
   i = 0; /* sin bloqueo */
   if (ixx_lck_lseek(fd,LCKDZONAINI+reg,SEEK_SET) == -1L)
   {
						eret(IXXELSEEK,errno,-1);
   }
   if (!ixx_lock(fd,LK_NBLCK,1L))
   {
						if (ixx_lock(fd,LK_UNLCK,1L))
                  {
                     eret(IXXENOSUPORT,errno,-1);
                  }
   }
   else
   {
						if (errno != EACCES && errno != EDEADLOCK && errno != EAGAIN && errno != 13 && errno != 35)
                  {
                     eret(IXXENOSUPORT,errno,-1);
                  }
                  i = 2; /* bloqueado por otro */
   }
   ret( i );
}

int ixx_reg_bloquea(reg,espera)
long reg;
int espera;
{
   int i;
   int fd = ixxfich->fd;
   
   if (ixxfich->openmode == IXXEXCLUYE) ret( 0 );
   
   if (reg <= 0L || reg > ixxfich->cabecera.nrecords)
   {
      eret(IXXECORRUP,110,-1);
   }
   for (i = 0;i < ixxfich->numero_bloqueos;i++)
   {
      if (reg == ixxfich->lista_bloqueos[i])
         ret( 0 ); /* bloqueado por mi */
   }
   
   /*  YA NO EXISTE TOPE DE BLOQUEOS!!!!
   if (ixxfich->numero_bloqueos >= IXXMAXLOCKS)
   {
      eret(IXXETOPELOCK,0,-1);
   }
   */
   
   if (ixx_lck_lseek(fd,LCKDZONAINI+reg-1,SEEK_SET) == -1L)
   {
      eret(IXXELSEEK,errno,-1);
   }
   if (espera)
   {
      while(-1) /* se espera (de momento asi) indefinidamente (si dos modos de lectura coincidieran se turnarian)*/
      {
         if (!ixx_lock(fd,LK_WAIT,1L)) break;
         if (errno != EACCES && errno != EDEADLOCK && errno != EAGAIN && errno != 13 && errno != 35)
									{
            eret(IXXENOSUPORT,errno,-1);
									}
         if (ixxdat_micro_espera)
            (*ixxdat_micro_espera)();
      }
   }
   else
   {
      if (ixx_lock(fd,LK_NBLCK,1L))
      {
         if ((errno == EACCES || errno == EAGAIN) || errno == EDEADLOCK  || errno == 13 || errno == 35)
									{
            eret(IXXELOCKED,0,-1);
									}
         else
									{
            eret(IXXENOSUPORT,0,-1);
									}
      }
   }
   if (ixxfich->numero_bloqueos >= (ixxfich->max_bloqueos-1))
   {
       ixxfich->max_bloqueos = ixxfich->numero_bloqueos + IXXMAXLOCKS;
       ixxfich->lista_bloqueos = (long *)realloc((char *)ixxfich->lista_bloqueos,sizeof(long)*ixxfich->max_bloqueos);
   }
   ixxfich->lista_bloqueos[ixxfich->numero_bloqueos] = reg;
   ixxfich->numero_bloqueos++;
   ret( 0 );
}

int   ixx_reg_desbloquea(reg)
long reg;
{
   int i;
   int fd = ixxfich->fd;
   
   if (ixxfich->openmode == IXXEXCLUYE) ret( 0 );
   
   if (reg < 0L || reg > ixxfich->cabecera.nrecords)
   {
      eret(IXXECORRUP,111,-1);
   }
   for (i = 0;i < ixxfich->numero_bloqueos;i++)
   {
      if (!reg)
      {
									if (ixx_lck_lseek(fd,LCKDZONAINI+ixxfich->lista_bloqueos[i]-1,SEEK_SET) == -1L)
                           {
                              eret(IXXELSEEK,errno,-1);
                           }
                           if (ixx_lock(fd,LK_UNLCK,1L))
                           {
                              eret(IXXENOSUPORT,errno,-1);
                           }
      }
      else
      {
       if (reg == ixxfich->lista_bloqueos[i])
           {
              if (ixx_lck_lseek(fd,LCKDZONAINI+reg-1,SEEK_SET) == -1L)
              {
                 eret(IXXELSEEK,errno,-1);
              }
              if (ixx_lock(fd,LK_UNLCK,1L))
              {
                 eret(IXXENOSUPORT,errno,-1);
              }
           }
       ixxfich->lista_bloqueos[i] = ixxfich->lista_bloqueos[ixxfich->numero_bloqueos-1];
       ixxfich->numero_bloqueos--;
       break;
      }
   }
   if (!reg)
   {
      ixxfich->numero_bloqueos = 0;
   }
   ret( 0 );
}


int ixx_nuevo_indice(nombre,nclaves,claves,tamreg,descamp,defectos,dtabla)
char *nombre;
short nclaves;
IXXCLAVE *claves; /* para cada clave obligatorio memoria para un campo mas */
short tamreg;
char **descamp;
char *defectos;
FgvStdTabla *dtabla;
{
   IXXCABEZA header;
   char ixx_nombre[51];
   short total_len_clave = 0;
   short i,j,k;
   Uchar *bloque;
   int fd,nfd;
   long reg,greg;
   int tamblock,t;
   int max;
   long oldisrecnum = isrecnum;
   unsigned short autentificacion;
   int kkk;
   int std_bloque;
   
   AjustaPunterosSegunDatos(-1);
   
   if (ixx_ojo_recursion)
   {
      eret(IXXERECURS,0,-1);
   }
   ixx_ojo_recursion = 1;
   
   
   if (nclaves < 1)
   {
      ixx_ojo_recursion = 0;
      eret( IXXENOCREA , 9999 , -1);
   }
   if (claves[0].clave_tipo != IXXCLAVENODUP)
   {
      ixx_ojo_recursion = 0;
      eret( IXXENOCREA , 9998 , -1);
   }
   
   
   total_len_clave = 0;
   for (i = 0;i < nclaves;i++)
   {
      max = 0;
      k = claves[i].clave_ncampos;
      if (k < 1)
      {
         ixx_ojo_recursion = 0;
         eret( IXXENOCREA , 7998 , -1);
      }
      if (claves[i].clave_tipo == IXXCLAVEDUP)
      {
         /* realmente no existen las duplicadas */
         if (claves[i].clave_campos[k-1].pos_en_reg != -1)
         {
            claves[i].clave_campos[k].len = sizeof(long);
            claves[i].clave_campos[k].type = IXXCLONG;
            claves[i].clave_campos[k].extra_type = IXXCNONE;
            claves[i].clave_campos[k].pos_en_reg = -1;
            claves[i].clave_campos[k].modo = 0;
            claves[i].clave_campos[k].name[0] = 0;
            k++;
            claves[i].clave_ncampos++;
         }
      }
      for (j = 0;j < k;j++)
      {
         if (claves[i].clave_campos[j].pos_en_reg < 0 || claves[i].clave_campos[j].pos_en_reg >= tamreg)
         {
            if (claves[i].clave_tipo != IXXCLAVEDUP || j != (k-1))
												{
               ixx_ojo_recursion = 0;
               eret( IXXENOCREA , 6998 , -1);
												}
         }
         max += claves[i].clave_campos[j].len;
      }
      claves[i].clave_len = max;
      if (total_len_clave < max)
         total_len_clave = max;
   }
   
   memset(&header,0,sizeof(header));
   memcpy(header.file_id,"ixxfgv",6); /*ixxfgv*/
   memcpy(header.file_ver,VERSION_INDEX,4); /* 0004 */
   
   header.file_ver[4] = IXXTIPO_PROCESADOR;
   
   /* numero de claves por bloque segun tamaño */
   j = (total_len_clave + LCLAVE_POINTERS) * 30; /* era 20 */

   if (!memcmp(VERSION_INDEX,"0003",4))
       std_bloque = 1004;
   else
   {       
       if (!tuned_blck)
       {
           std_bloque = 1004;
       }
       else
       {
           std_bloque = tuned_blck-20;
       }       
   }

   if (j < std_bloque)
      j = std_bloque;
   else
      if (j > 8172)
      {
         if ( (8172 / (total_len_clave + LCLAVE_POINTERS)) < 7)
            j = (total_len_clave + LCLAVE_POINTERS) * 7; /* si no caben al menos 7 claves ... */
         else
            j = 8172;
      }
      
      tamblock = j + 4 /* id */ +  2 /* level */+ 4 /* prev level */ + 4 /* prev chain o total claves */ + 4 /* next chain */ +
         2 /* n keys */;
      
      /* asegurarse de que quepa la descripcion de las claves */
      for (i = 0;i < nclaves;i++)
      {
         if ( (claves[i].clave_ncampos * sizeof(IXXCAMPO) + 21) > tamblock)
         {
            tamblock = claves[i].clave_ncampos * sizeof(IXXCAMPO) + 21;
         }
      }
      
      
      if ( (tamblock%512) )
      {
         /* redondeado a 512 para que el sistema operativo vaya mejor */
         tamblock += (512 - (tamblock%512));
      }
      
      header.tamreg = tamreg;
      header.tamblock = tamblock;
      header.nkeys = nclaves;
      header.nblocks = nclaves + 1;
      
      for (i = 0;i < nclaves;i++)
      {
         j = (tamblock-20) / (claves[i].clave_len+LCLAVE_POINTERS);
         claves[i].clave_num = j;
      }
      
      strcpy(ixx_nombre,nombre);
      strcat(ixx_nombre,".ixx");
      fd = ixx_abre_share(ixx_nombre);
      if (fd > -1)
      {
         ixx_cierra(fd);
         ixx_ojo_recursion = 0;
         if ( (fd = ixx_abre_indice(nombre,IXXCOMPARTE,descamp,defectos,tamreg,dtabla)) >= 0)
         {
            ixx_cierra_indice(fd);
            eret( IXXEEXISTE , 0 , -1);
         }
         if (ixxstatus == IXXEFLOCK)
         {
            eret( IXXEEXISTE , 0 , -1);
         }
      }
      else
      {
         if (errno != ENOENT)
         {
            ixx_ojo_recursion = 0;
            if ((errno == EACCES || errno == EAGAIN  || errno == 13 || errno == 35))
												{
               eret( IXXEEXISTE , 0 , -1);
												}
            else
												{
               eret( IXXENOCREA , errno , -1);
												}
         }
      }
      ixx_ojo_recursion = 1;
      
      /* bloqueo exclusivo */
      fd = ixx_crea_nuevo(ixx_nombre);
      if (fd < 0)
      {
         ixx_ojo_recursion = 0;
         eret( IXXENOCREA , errno , -1);
      }
      ixx_nuevo_cierra(fd);
      fd = ixx_abre_exclu(ixx_nombre);
      if (fd < 0)
      {
         ixx_ojo_recursion = 0;
         eret( IXXENOCREA , errno , -1);
      }
      
      bloque = malloc(tamblock);
      if (!bloque)
      {
         ixx_ojo_recursion = 0;
         eret( IXXENOMEM , errno , fd);
      }


      memset(bloque,0,tamblock);
      memcpy(bloque,&header,sizeof(header));
      
      if (ixx_graba(fd,bloque,tamblock) != tamblock)
      {
         free(bloque);
         ixx_ojo_recursion = 0;
         eret( IXXENOCREA , errno , fd);
      }
      
      /* añadir claves */
      autentificacion = header.tamblock ^ header.nkeys ^ header.tamreg;
      for (i = 0;i < nclaves;i++)
      {
         memset(bloque,0,tamblock);
         memcpy(bloque,"ixxkeyd",7);
         memcpy(bloque+7,&i,2);
         /* bloque +9 con 4 primer bloque de indice */
         if ( (claves[i].clave_ncampos * sizeof(IXXCAMPO) + 21) > tamblock)
         {
            free(bloque);
            ixx_ojo_recursion = 0;
            eret( IXXEMAXCAMCLAVE , i , fd);
         }
         memcpy(bloque+13,&(claves[i].clave_len),2);
         memcpy(bloque+15,&(claves[i].clave_num),2);
         memcpy(bloque+17,&(claves[i].clave_tipo),2);
         memcpy(bloque+19,&(claves[i].clave_ncampos),2);
         memcpy(bloque+21,claves[i].clave_campos,claves[i].clave_ncampos * sizeof(IXXCAMPO));
         if (ixx_graba(fd,bloque,tamblock) != tamblock)
         {
            free(bloque);
            ixx_ojo_recursion = 0;
            eret( IXXENOCREA , errno , fd);
         }
         for (kkk = 0;kkk < ((tamblock/2)-1);kkk++)
         {
            autentificacion ^= *(((unsigned short *)bloque)+kkk);
         }
      }
      
      header.autentificacion = autentificacion;
      if (ixx_lseek(fd,OIDENTI,SEEK_SET)== -1L)
      {
         ixx_ojo_recursion = 0;
         eret(IXXELSEEK,errno,fd);
      }
      if (ixx_graba(fd,&autentificacion,sizeof(short)) != sizeof(short))
      {
         ixx_ojo_recursion = 0;
         eret(IXXEWRITE,errno,fd);
      }



      free(bloque);
      ixx_ojo_recursion = 0;
      ixxtouch(fd);
      ixx_cierra(fd);
      ixx_fuera_cache(nombre);
      nfd = ixx_abre_indice(nombre,IXXCOMPARTE,descamp,defectos,tamreg,dtabla);
      /************************* si hay datos reindexar **************************/
      /* cuando se crea un fichero nuevo se deja
      como estropeado para que se intente reindexar
      para recoger los posibles datos que haya */
      isrecnum = oldisrecnum;
      /***************************************************************************/
      bret(nfd);
}



int ixx_cierra_indice(ixx_fd)
int ixx_fd;
{
   short i;
   int err;
   
   AjustaPunterosSegunDatos(ixx_fd);
   
   if (ixx_ojo_recursion)
   {
      eret(IXXERECURS,0,-1);
   }
   ixx_ojo_recursion = 1;
   
   
   if (ixx_fd < 0 || ixx_fd >= IXXMAXFILES)
   {
      ixx_ojo_recursion = 0;
      eret(IXXEMALARGU,0,-1);
   }
   
   if (!ixxfichs[ixx_fd])
   {
      ixx_ojo_recursion = 0;
      eret(IXXENOOPEN,0,-1);
   }
   
   ixxfich = ixxfichs[ixx_fd];
   ixx_e_fich = ixx_fd;
   
   if (!(ixxfich->pseudo_cierra) && ixx_absoluto && ixxfich->nombredat && ixxfich->nombreixx)
   {
      err = ixx_reg_desbloquea(0L);
      if (ixxfich->fd_dat >= 0)
      {
         if ((*ixxdat_cierra)(ixxfich->fd_dat) < 0)
         {
            ixx_ojo_recursion = 0;
            eret(IXXECIERRADAT,errno,-1);
         }
      }
      if (ixxfich->fd >= 0)
      {
         ixxfichs[ixx_fd]->pseudo_guarda = ixxBuffers[ixxfich->fd];
         ixxBuffers[ixxfich->fd] = NULL;
         if (ixx_cierra(ixxfich->fd) < 0)
            err = errno;
      }
      ixxfich->fd_dat = -1;
      ixxfich->fd = -1;
      ixxfich->pseudo_cierra = 1;
      ixx_ojo_recursion = 0;
      _e_log_close++;
      return(err);
   }
   
   
   if (ixx_buf_fin(ixx_fd) < 0)
   {
      ret(-1);
   }
   
   if (!(ixxfich->pseudo_cierra))
   {
      if (ixxfich->fd_dat >= 0)
      {
         if ((*ixxdat_cierra)(ixxfich->fd_dat) < 0)
         {
            ixx_ojo_recursion = 0;
            eret(IXXECIERRADAT,errno,-1);
         }
      }
      err = ixx_reg_desbloquea(0L);
      
      /* desbloquear uso del indice */
      /* de momento lo hace el sistema*/
      /* cerrar fichero */
      if (ixx_cierra(ixxfich->fd) < 0)
         err = errno;
   }
   else
   {
      err = 0;
   }
   
   
   for (i = 0;i < ixxfich->nclaves;i++)
   {
      free( ixxfich->claves[i].clave_campos );
      if (ixxfich->DatosCurr && ixxfich->DatosCurr[i])
         free( ixxfich->DatosCurr[i] );
   }
   
   free(ixxfich->claves);
   if (ixxfich->DatosCurr)
      free(ixxfich->DatosCurr);

   if (ixxfich->NumeroPosicion)
      free(ixxfich->NumeroPosicion);

   if (ixxfich->lista_bloqueos)
       free(ixxfich->lista_bloqueos);

   if (ixxfich->nombredat)
   {
      free(ixxfich->nombredat);
   }
   if (ixxfich->nombreixx)
   {
      free(ixxfich->nombreixx);
   }
   if (ixxfich->contenido)
   {
      free(ixxfich->contenido);
   }
   if (ixxfich->dtabla.Campo)
   {
      free(ixxfich->dtabla.Campo);
   }
   free(ixxfich);
   ixxfichs[ixx_fd] = NULL;
   
   /* limpiar objetos de uso comun cuando no hay ningun fichero abierto */
   for (i = 0;i < IXXMAXFILES;i++)
   {
      if (ixxfichs[i]) break;
   }
   if (i >= IXXMAXFILES)
   {
      free(ixxbloque);
      free(ixxbloque2);
      free(ixxbloque3);
      free(ixxbloque4);
      free(ixxbloque5);
      ixxbloque = NULL;
      ixxtamblock = 0;
      if (datos_dim)
      {
         for (i = 0;i < datos_dim1;i++)
         {
            free(Datos[i]);
            free(Datos2[i]);
         }
         free(datos_dim);
         free(Datos);
         free(Datos2);
         datos_dim = NULL;
         Datos =  NULL;
         datos_dim1 = 0;
      }
      if (Dat_Cur)
      {
         free(Dat_Cur);
         Dat_Cur = NULL;
         len_dat_cur = 0;
      }
   }
   ixx_ojo_recursion = 0;
   if (err >= 0)
      _e_log_close++;
   bret(err);
}


int ixx_fuera_cache(nombre)
char *nombre;
{
   int ixx_fd;
   
   for (ixx_fd = 0;ixx_fd < IXXMAXFILES;ixx_fd++)
   {
      if (ixxfichs[ixx_fd] && ixxfichs[ixx_fd]->pseudo_cierra)
      {
         if (ixxfichs[ixx_fd]->nombredat && !strcmp(nombre,ixxfichs[ixx_fd]->nombredat))
         {
            ixx_cierra_indice(ixx_fd);
         }
      }
   }
   return(0);
}


static int _IXX_reconstruye(ixx_fd,compactar /* no usado */)
int ixx_fd,compactar;
{
   int fd,i,t,tamblock,sale_mal = 0,oldmode,preverr = ixxerrno;
   long reg,greg,pos,oldisrec = isrecnum;   
   Uchar *clave_primaria = NULL; /* posiciona siempre un corriente aunque sea mentira */
   Uchar *bloque = NULL;
   char tmp[256];
   short clave_num;
   long _i_log_open = _e_log_open;
   long _i_log_close = _e_log_close;
   long _i_log_read = _e_log_read;
   long _i_log_update = _e_log_update;
   long _i_log_write = _e_log_write;
   long _i_log_delete = _e_log_delete;
   int cambio_version = 0;
   
   AjustaPunterosSegunDatos(ixx_fd);
   
   
   ixx_e_fich = -1;
   if (ixx_fd < 0 || ixx_fd >= IXXMAXFILES || !ixxfichs[ixx_fd])
			{
      eret(IXXEMALARGU,0,-1);
			}
   ixxfich=ixxfichs[ixx_fd];
   ixx_e_fich = ixx_fd;
   fd = ixxfich->fd;
   tamblock = ixxfich->tamblock;
   oldmode = ixxfich->openmode;
   
   if (!ixxfich->tamreg)
			{
      eret(IXXEMALFICH,0,-1);
			}
   
   if (ixxfich->cabecera.file_ver[4] != IXXTIPO_PROCESADOR)
   {
       eret(IXXEMALFICH,0,-1);
   }

   if (memcmp(ixxfich->cabecera.file_ver,VERSION_INDEX,4))
   {
       eret(IXXEMALFICH,0,-1);
   }

   if (ixxfich->fd_dat < 0)
			{
      ixxfich->fd_dat = (*ixxdat_abre)(ixxfich->nombredat,NULL,NULL,NULL,NULL,NULL,&(ixxfich->dtabla));
			}
   
   if (ixxfich->fd_dat < 0)
			{
      ixx_ojo_recursion = 0;
      eret(IXXENOOPEN,0,-1);
			}
   
   if (oldmode != -4)
			{
      if (ixx_lck_lseek(fd,LCKREZONAINI,SEEK_SET) == -1L)
      {
         eret(IXXELSEEK,errno,-1);
      }
      if (ixx_lock(fd,LK_NBLCK,1L) < 0)
      {
         isrecnum = oldisrec;
         ret(0); /* alguien lo esta haciendo */
      }
      ixxfich->openmode = 0;
			}
   
   if (ixx_bloquea_escritura(fd,ixxfich->openmode))
			{
      ixx_lck_lseek(fd,LCKREZONAINI,SEEK_SET);
      ixx_lock(fd,LK_UNLCK,1L);
      ret(-1);
			}
   
   ixxtouch(fd);
   
   ixx_buf_anula(fd);
   ixx_mantener_el_buffer = fd;
   
   if (oldmode != -4) ixxfich->openmode = -4;
   
   bloque = malloc(ixxfich->tamreg+2);
   clave_primaria = malloc(ixxfich->claves[0].clave_len);
   memcpy(clave_primaria,ixxfich->DatosCurr[0],ixxfich->claves[0].clave_len);
   
   ixxfich->reg_cur=0L;
   ixxfich->blo_cur=0L;
   ixxfich->pos_cur=0;
   ixxfich->control_numblocks = 0L;
   ixxfich->cabecera.nblocks = (long)(ixxfich->nclaves+1);
   ixxfich->cabecera.nrecords = 0L;
   ixxfich->cabecera.nfreeblocks = 0L;
   ixxfich->cabecera.nfreerecords = 0L;
   ixxfich->cabecera.recordfreelist =0L;
   ixxfich->cabecera.blockfreelist =0L;
   ixxfich->cabecera.touch=1;

   memcpy(ixxfich->cabecera.file_id,"ixxfgv",6);         
   memcpy(ixxfich->cabecera.file_ver,VERSION_INDEX,4);       
   ixxfich->cabecera.file_ver[4] = IXXTIPO_PROCESADOR;

   
   if (ixx_lseek(fd,OUPDATE,SEEK_SET)== -1L)
			{
      IXXerror(IXXELSEEK,errno,-1);
      sale_mal = 1;
      goto recon_sale;
			}
   if (ixx_lee(fd,&(ixxfich->cabecera.updatecount),sizeof(long)) != sizeof(long))
			{
      IXXerror(IXXEREAD,errno,-1);
      sale_mal = 1;
      goto recon_sale;
			}
   if (ixx_lseek(fd,0L,SEEK_SET)== -1L)
			{
      IXXerror(IXXELSEEK,errno,-1);
      sale_mal = 1;
      goto recon_sale;
			}
   if (ixx_graba(fd,&(ixxfich->cabecera),sizeof(IXXCABEZA)) != sizeof(IXXCABEZA))
			{
      IXXerror(IXXEWRITE,errno,-1);
      sale_mal = 1;
      goto recon_sale;
			}   


   for (i = 0;i < ixxfich->nclaves;i++)
			{
      pos = (long)(i+1)*tamblock;
      pos += 9L;
      if (ixx_lseek(fd,pos,SEEK_SET)== -1L)
      {
         IXXerror(IXXELSEEK,errno,-1);
         sale_mal = 1;
         goto recon_sale;
      }
      pos = 0L;
      if (ixx_graba(fd,&pos,sizeof(long)) != sizeof(long))
      {
         IXXerror(IXXEWRITE,errno,-1);
         sale_mal = 1;
         goto recon_sale;
      }


     if (cambio_version)
     {
          pos = (long)(i+1)*tamblock;
          pos += 15L;
          if (ixx_lseek(fd,pos,SEEK_SET)== -1L)
          {
             IXXerror(IXXELSEEK,errno,-1);
             sale_mal = 1;
             goto recon_sale;
          }
          clave_num = (tamblock-20) / (ixxfich->claves[i].clave_len+LCLAVE_POINTERS);
          ixxfich->claves[i].clave_num = clave_num;
          if (ixx_graba(fd,&clave_num,sizeof(short)) != sizeof(short))
          {
             IXXerror(IXXEWRITE,errno,-1);
             sale_mal = 1;
             goto recon_sale;
          }
      }

   }
   
   
   i = -1;
   (*ixxdat_tamfichero)(ixxfich->fd_dat,0L,ixxfich->tamreg,&reg);
   if (reg > 0L)
			{
      _e_log_open = 0L;
      _e_log_close = 0L;
      _e_log_read = 0L;
      _e_log_update = 0L;
      _e_log_write = 0L;
      _e_log_delete = 0L;
      
      if (ixxind_aviso)
      {
         strcpy(tmp,ixxfich->nombredat);                
         sprintf(tmp+strlen(tmp)," (%d)",preverr);
         (*ixxind_aviso)(0,reg,(long)(tmp));
      }
      reg = 1L;
      greg = 1L;      
      while(i)
      {
         ixxfich->cabecera.nrecords = reg;
         no_read_error = 1;
         t = (*ixxdat_lee)(ixxfich->fd_dat,reg,bloque,ixxfich->tamreg);
         no_read_error = 0;
         if (t < 0) 
         {
            ixxfich->cabecera.nrecords--;
            break;
         }
         if (t > 0)
         {
               ixxlibera_registro(reg);
         }
         else
         {
            
            isrecnum = reg;
            if (!ixx_graba_indice(ixx_fd,bloque,IXXGSOLOI))
            {
               greg++;
            }
            else
            {
               ixxlibera_registro(reg);
               (*ixxdat_borra)(ixxfich->fd_dat,reg,ixxfich->tamreg);
            }
         }
         if (!(reg % 27) && ixxind_aviso)
         {
            (*ixxind_aviso)(1,reg,greg);
         }
         reg++;
      }
      if (!ixxfich->control_numblocks)
         ixxfich->control_numblocks = 1L;
      if (ixx_lseek(fd,0L,SEEK_SET)== -1L)
      {
         IXXerror(IXXELSEEK,errno,-1);
         sale_mal = 1;
         goto recon_sale;
      }
      if (ixx_graba(fd,&(ixxfich->cabecera),sizeof(IXXCABEZA)) != sizeof(IXXCABEZA))
      {
         IXXerror(IXXEWRITE,errno,-1);
         sale_mal = 1;
         goto recon_sale;
      }
      
      if (ixx_incidencia)
      {
         strcpy(tmp,"INDEXA FICHERO ");
         if (ixxfich->nombreixx && ixxquita_path)
         {
            strcat(tmp,(*ixxquita_path)(ixxfich->nombreixx));
         }
         sprintf(tmp+strlen(tmp)," (%d)",preverr);
         (*ixx_incidencia)(0,tmp);
      }
      _e_log_open = _i_log_open;
      _e_log_close = _i_log_close;
      _e_log_read = _i_log_read;
      _e_log_update = _i_log_update;
      _e_log_write = _i_log_write;
      _e_log_delete = _i_log_delete;
      
      
      if (ixxind_aviso)
      {
         (*ixxind_aviso)(2,greg,(long)i);
      }
			}
   
   
   ixx_mantener_el_buffer = -1;
   ixx_buf_flush(fd);
   ixx_buf_anula(fd);
   
   ixxuntouch(fd);
   
recon_sale:
   if (ixx_mantener_el_buffer >= 0)
			{
      ixx_mantener_el_buffer = -1;
      ixx_buf_flush(fd);
      ixx_buf_anula(fd);
			}
   
   if (ixx_busca_clave((short)0,clave_primaria,&pos,&reg,_Posblock,0) == 1)
   {
      ixxfich->reg_cur=reg;
   }   
   if (bloque)
      free(bloque);
   if (clave_primaria)
      free(clave_primaria);
   
   if (oldmode != -4) ixxfich->openmode = 0;
   
   /* unica cosa sin chequear por redundante */
   ixx_lck_lseek(fd,LCKREZONAINI,SEEK_SET);
   ixx_lock(fd,LK_UNLCK,1L);
   /**********************/
   
   if (ixx_desbloquea_escritura(fd,ixxfich->openmode))
			{
      ret(-1);
			}
   
   ixxfich->openmode = oldmode;
   if (sale_mal) ret(-1);
   isrecnum = oldisrec;
   

   ret(0);
}

int ixx_reconstruye(ixx_fd)
int ixx_fd;
{
    return(_IXX_reconstruye(ixx_fd,0));
}

int ixxtocado(fd)
int fd;
{
   char c;
   if (ixx_lseek(fd,OTOUCH,SEEK_SET) == -1L)
   {
      eret(IXXELSEEK,errno,-1);
   }
   if (ixx_lee(fd,&c,1) == 1)
      return((int)c);
   eret(IXXEREAD,errno,-1);
}

int ixxtouch(fd)
int fd;
{
   char c=1;
   if (ixx_lseek(fd,OTOUCH,SEEK_SET) == -1L)
   {
      eret(IXXELSEEK,errno,-1);
   }
   if (ixx_graba(fd,&c,1) != 1)
   {
      eret(IXXEWRITE,errno,-1);
   }
   return(0);
}

int ixxuntouch(fd)
int fd;
{
    char c=0;
    if (ixx_lseek(fd,OTOUCH,SEEK_SET) == -1L)
    {
        eret(IXXELSEEK,errno,-1);
    }
    if (ixx_graba(fd,&c,1) != 1)
    {
        eret(IXXEWRITE,errno,-1);
    }
    return(0);
}


/************************************************************************/
/*                       Inicio SISTEMA BUFFERS                         */
/************************************************************************/
/*
#undef ixx_lee
#undef ixx_graba
#undef ixx_lseek
#define ixx_lee         ixx_buf_lee
#define ixx_graba       ixx_buf_graba
#define ixx_lseek       ixx_buf_lseek
*/

int ixxcompara_clave(p1,datos_clave,clave,mode)
Uchar *p1,*datos_clave;
IXXCLAVE *clave;
int mode; /* viene con 1 en caso de secundaria y sea busqueda no especifica de una duplicada */
{
   int i;
   register int n;
   register Uchar *c1,*c2;
   int tip;
   int len;
   int modo;
   int r = 0;
   
   for (i = 0;i < (clave->clave_ncampos-mode);i++)
   {
      len = clave->clave_campos[i].len;
      tip = clave->clave_campos[i].type;
      modo = clave->clave_campos[i].modo; /* 0 ascendente 1 descendente */
      switch (tip)
						{
						case IXXCALFA:  /* CHAR  TIPO */
                     if (!modo)
                     {
                        c1 = datos_clave;
                        c2 = p1;
                     }
                     else
                     {
                        c2 = datos_clave;
                        c1 = p1;
                     }
                     r = 0;
                     for (n = 0;n < len;n++,c1++,c2++)
                     {
                        if (*c1 < *c2)
                        {
                           r = -1;
                           break;
                        }
                        if (*c1 > *c2)
                        {
                           r = 1;
                           break;
                        }
                     }
                     break;
                  case IXXCSHORT:  /* SHORT TIPO */
                     r = 0;
                     if (!modo)
                     {
                        if (  *((short *)(datos_clave)) > *((short *)(p1)))
                           r = 1;
                        else
                           if (*((short *)(datos_clave)) < *((short *)(p1)))
                              r = -1;
                     }
                     else
                     {
                        if (*((short *)(datos_clave)) > *((short *)(p1)))
                           r = -1;
                        else
                           if (*((short *)(datos_clave)) < *((short *)(p1)))
                              r = 1;
                     }
                     break;
                  case IXXCLONG:  /* LONG TIPO */
                     r = 0;
                     if (!modo)
                     {
                        if (*((long *)(datos_clave)) > *((long *)(p1)))
                           r = 1;
                        else
                           if (*((long *)(datos_clave)) < *((long *)(p1)))
                              r = -1;
                     }
                     else
                     {
                        if (*((long *)(datos_clave)) > *((long *)(p1)))
                           r = -1;
                        else
                           if (*((long *)(datos_clave)) < *((long *)(p1)))
                              r = 1;
                     }
                     break;
                  case IXXCFLOAT:  /* FLOAT TIPO */
                     if (!modo)
                     {
                        if (*((float *)datos_clave) < *((float *)p1))
                           r = -1;
                        else
                           if (*((float *)datos_clave) > *((float *)p1))
                              r = 1;
                           else
                              r = 0;
                     }
                     else
                     {
                        if (*((float *)p1) < *((float *)datos_clave))
                           r = -1;
                        else
                           if (*((float *)p1) > *((float *)datos_clave))
                              r = 1;
                           else
                              r = 0;
                     }
                     break;
                  case IXXCDOUBLE:  /* DOUBLE TIPO */
                     if (!modo)
                     {
                        if (*((double *)datos_clave) < *((double *)p1))
                           r = -1;
                        else
                           if (*((double *)datos_clave) > *((double *)p1))
                              r = 1;
                           else
                              r = 0;
                     }
                     else
                     {
                        if (*((double *)p1) < *((double *)datos_clave))
                           r = -1;
                        else
                           if (*((double *)p1) > *((double *)datos_clave))
                              r = 1;
                           else
                              r = 0;
                     }
                     break;
                  default:
                     r = 0;
                     break;
         }
         if (r)
         {
            if (r < 0) r = -1;else r = 1;
            break;
         }
         p1 += len;
         datos_clave += len;
  }
  ret(r);
}


/* busca en bloque :
*/

int ixxbusca_en_bloque(num,len,laclave,datos_clave,reg,posblock,modo,nkeys,modus)
short num;
short len;
short laclave;
Uchar *datos_clave;
long *reg;
short *posblock;
int modo;
short nkeys;
int modus;
{
   Uchar *p1,*p2;
   int com;
   int nb,nbm,nk;
   IXXCLAVE *clave = &(ixxfich->claves[laclave]);
   int avance = 0;
   int retro = 0;
   long parcial;
   if (nkeys > num || nkeys <= 0)
   {
      eret(IXXECORRUP,2,-1);
   }
   
   if (modo)
   {
      if (modus == IXXGREAT)
         avance = 1;
      else
         retro = 1;
   }
   
   
   /* primero comparar la primera para bajada rapida de clave coincidente de nivel
   superior o clave inferior*/
   p1 = ixxbloque + 20;
   com = ixxcompara_clave(p1,datos_clave,clave,modo);
   switch(com)
   {
   case -1:
      *posblock = (short)(p1-ixxbloque);
      *reg = *((long *)(p1+len)); /* justo la primera */
      ret(2); /* es menor que la primera (no existe) */
   case 0:
      if (avance)
      {
         /* ajustar a la ultima igual */
         for (nb = 1;nb < nkeys;nb++) /* encontrado bug habia un nb-- (21/4/97) */
         {            
            p1 += (len+LCLAVE_POINTERS);
            com = ixxcompara_clave(p1,datos_clave,clave,modo);
            if (com)
            {
               p1 -= (len+LCLAVE_POINTERS);
               break;
            }            
         }
      }
      *posblock = (short)(p1-ixxbloque);
      *reg = *((long *)(p1+len));
      ret(1);
   case 1:
      break; /* es mayor !! */
   default:
      eret(IXXECORRUP,3,-1);
   };
   /* luego comparar clave ultima */
   p1 = ixxbloque + 20 + (nkeys-1) * (len+LCLAVE_POINTERS);
   com = ixxcompara_clave(p1,datos_clave,clave,modo);
   switch(com)
   {
   case -1: /* es menor */
      break;
   case 0:
      if (retro)
      {
         /* al ser igual ajustar puntero asegurandose la primera duplicada */
         for (nb = nkeys;nb > 1;nb--)
         {
            p1 -= (len+LCLAVE_POINTERS);
            com = ixxcompara_clave(p1,datos_clave,clave,modo);
            if (com)
            {
               p1 += (len+LCLAVE_POINTERS);
               break;
            }
         }
         if (nb == 1)
         { /* internal check */
            eret(IXXECORRUP,4,-1);
         }
      }      

      *posblock = (short)(p1-ixxbloque);
      *reg = *((long *)(p1+len));

      ret(1);
   case 1:
      *posblock = (short)(p1-ixxbloque);
      *reg = *((long *)(p1+len));

      ret(0);
   default:
      eret(IXXECORRUP,5,-1);
   };
   
   nbm = nkeys - 2;
   nb = 0;
   p1 = ixxbloque + 20 + len+LCLAVE_POINTERS;
   do
   {
      if (nbm < 12)
      {
         do {
            com = ixxcompara_clave(p1,datos_clave,clave,modo);
            switch(com)
            {
            case -1: /* es menor */
               *posblock = (short)(p1-ixxbloque-len-LCLAVE_POINTERS);
               *reg = *((long *)(p1-LCLAVE_POINTERS));

               ret(0);
            case 0:
               nk = (short)(p1-ixxbloque);
               nk -= 20;
               nk /= (len+LCLAVE_POINTERS);
               if (retro)
               {
                  for (;nk > 0;nk--)
                  {
                     p1 -= (len+LCLAVE_POINTERS);
                     com = ixxcompara_clave(p1,datos_clave,clave,modo);
                     if (com)
                     {
                        p1 += (len+LCLAVE_POINTERS);
                        break;
                     }
                  }
                  if (!nk)
                  {
                     eret(IXXECORRUP,6,-1);
                  }
               }
               else
                  if (avance)
                  {
                     for (;nk < (nkeys-1);nk++) /* bug (21/4/97) habia nk <= nkeys haciendo 2 mas aunque la comparacion lo arreglaba */
                     {
                        p1 += (len+LCLAVE_POINTERS);
                        com = ixxcompara_clave(p1,datos_clave,clave,modo);
                        if (com)
                        {
                           p1 -= (len+LCLAVE_POINTERS);
                           break;
                        }
                     }
                     if (nk >= nkeys)
                     {
                        eret(IXXECORRUP,6,-1);
                     }
                  }
                  *posblock = (short)(p1-ixxbloque);
                  *reg = *((long *)(p1+len));

                  ret(1);
            case 1: /* es mayor */
               break;
            default:
               eret(IXXECORRUP,7,-1);
            };
            p1 += (len+LCLAVE_POINTERS);
            nb++;
         } while (nb < nbm);
         *posblock = (short)(p1-ixxbloque-len-LCLAVE_POINTERS);
         *reg = *((long *)(p1-LCLAVE_POINTERS));

         ret(0);
      }
      else
      {
         p2 = p1;
         p1 += ((nbm/2) * (len+LCLAVE_POINTERS));
         com = ixxcompara_clave(p1,datos_clave,clave,modo);
         switch(com)
         {
         case -1: /* es menor */
            p1 = p2;
            nbm /= 2;
            break;
         case 0:
            nk = (short)(p1-ixxbloque);
            nk -= 20;
            nk /= (len+LCLAVE_POINTERS);
            if (retro)
            {
               for (;nk > 0;nk--)
               {
                  p1 -= (len+LCLAVE_POINTERS);
                  com = ixxcompara_clave(p1,datos_clave,clave,modo);
                  if (com)
                  {
                     p1 += (len+LCLAVE_POINTERS);
                     break;
                  }
               }
               if (!nk)
               {
                  eret(IXXECORRUP,8,-1);
               }
            }
            else
               if (avance)
               {
                  for (;nk < (nkeys-1);nk++) /* bug (21/4/97) habia nk <= nkeys haciendo 2 mas aunque la comparacion lo arreglaba */
                  {
                     p1 += (len+LCLAVE_POINTERS);
                     com = ixxcompara_clave(p1,datos_clave,clave,modo);
                     if (com)
                     {
                        p1 -= (len+LCLAVE_POINTERS);
                        break;
                     }
                  }
                  if (nk >= nkeys)
                  {
                     eret(IXXECORRUP,6,-1);
                  }
               }
               *posblock = (short)(p1-ixxbloque);
               *reg = *((long *)(p1+len));

               ret(1);
         case 1: /* es mayor */
            p1 += (len+LCLAVE_POINTERS);
            nbm -= (nbm/2);
            nbm--;
            break;
         default:
            eret(IXXECORRUP,9,-1);
         };
      }
 } while(-1);
}

/* busca_clave:
busca la clave por el arbol */

int ixx_busca_clave(laclave,datos_clave,blo,reg,posblock,modo)
short laclave;
Uchar *datos_clave;
long *blo;
long *reg;
short *posblock;
int modo;
{
   long pos;
   int fd = ixxfich->fd;
   int identica = -1;
   short level;
   short len = ixxfich->claves[laclave].clave_len;
   short num = ixxfich->claves[laclave].clave_num;
   int tamblock = ixxfich->tamblock;
   short control1 = 0; /* control de menor */
   short nkeys,expected = -1;
   int control_identica = 0; /* control de identica */
   int modus = 0;
   int chequeo_especial = 0;   
   char *p1;   

   /*if (!memcmp(VERSION_INDEX,"0003",4))*/
      chequeo_especial = -1;   

   if (modo)
   {
      modus = modo - 1;
      modo = 1;
   }
   
   if (modo && ixxfich->claves[laclave].clave_tipo == IXXCLAVENODUP)
      modo = 0;
   
   /* leer posicion del bloque raiz de la clave en cuestion */
   pos = tamblock;
   pos *= (laclave+1);
   pos += 9;
   if (ixx_lseek(fd,pos,SEEK_SET)== -1L)
   {
      eret(IXXELSEEK,errno,-1);
   }
   if (ixx_lee(fd,(unsigned char *)&pos,sizeof(long)) != sizeof(long))
   {
      eret(IXXEREAD,errno,-1);
   }
   if (!pos) ret(3); /* fichero vacio */
   /* bucle de busqueda */

   IxxNumeroPosicion = 0L;

   do {
      if (pos <= 0L || pos >= ixxfich->cabecera.nblocks)
      {
         eret(IXXECORRUP,11,-1);
      }
      
      *blo = pos;
      
      pos *= tamblock;
      if (ixx_lseek(fd,pos,SEEK_SET)== -1L)
      {
         eret(IXXELSEEK,errno,-1);
      }
      if (ixx_lee(fd,ixxbloque,ixxfich->tamblock) != ixxfich->tamblock)
      {
         eret(IXXEREAD,errno,-1);
      }
      /* establecer posicion de la clave */
      if (*((short *)ixxbloque) != IXXIDBCLAVE)
      {
         eret(IXXECORRUP,12,-1);
      }
      if (*((short *)(ixxbloque+2)) != laclave)
      {
         eret(IXXECORRUP,13,-1);
      }
      
      nkeys = *((short *)(ixxbloque+18));
      level = *((short *)(ixxbloque+4));
      if (expected == -1)
         expected = level;
      else
         if (expected != level)
         {
            eret(IXXECORRUP,83,-1);
         }
         
         if (!nkeys)
         {
            if (!level)
               ret(3); /* fichero vacio */
            eret(IXXECORRUP,14,-1);
         }
         /* busqueda en el bloque */
         identica = ixxbusca_en_bloque(num,len,laclave,datos_clave,reg,posblock+level,modo,nkeys,modus);
         if (identica < 0)
         {
            ret(-1);
         }
         if (identica == 1)  /* a partir de una identica el resto de subniveles lo ha de ser!!! */
         {
            control_identica = 1;
         }
         else
            if (control_identica)
            {
               eret(IXXECORRUP,87,-1);
            }
            
            if (identica == 2)
            {
               control1 = 1;
               /* bajar hasta el ultimo nivel para dejar posicionado el bloque ok */
            }
            else
               if (control1)
               {
               /* esto pasa si al bajar los niveles la clave en un nivel inferior
                  deja de ser anterior */
                  eret(IXXECORRUP,15,-1);
               }

               if (chequeo_especial == -1)
               {
                   if (posblock[level] == 20)
                   {
                       chequeo_especial = 1;
                   }
                   /*
                   else
                   {
                       chequeo_especial = 0;
                   }
                   */
               }
               else
               {
                   /*
                   if (chequeo_especial == 1 && posblock[level] != 20)
                   {
                       chequeo_especial = 0;
                   }
                   */
               }
               pos = *reg;
               expected--;

              if (LCLAVE_TOTAL)
              {
                  for (p1 = ixxbloque + 20;p1 < (ixxbloque+*(posblock+level));p1 += (len+LCLAVE_POINTERS))
                  {
                      IxxNumeroPosicion += *((long *)(p1 + len + LCLAVE_TOTAL));
                  }
              }

   } while(level);
   
   
   /* control de integridad de duplicadas por problema antiguo con el indice */
   if (modo && !level && /*posblock[0] == 20 && */chequeo_especial == 1)
   {
      long padre;
      long hijo;
      long posi;
      int kkks;
      int nlevel = 0;
      int nnlevel = 0;      
      int kklevel = 0;
      char *kp2;
      char *kp1;
      
      padre = *((long *)(ixxbloque+6));
      while (padre)
      {         
         nlevel++;         

         posi = padre * tamblock;
         if (ixx_lseek(fd,posi,SEEK_SET)== -1L)
         {
            eret(IXXELSEEK,errno,-1);
         }
         if (ixx_lee(fd,ixxbloque5,ixxfich->tamblock) != ixxfich->tamblock)
         {
            eret(IXXEREAD,errno,-1);
         }
         padre = *((long *)(ixxbloque5+6));

         kklevel = (int)*((short *)(ixxbloque5+4));

         if (kklevel != nlevel) 
         {
             break; /* anti enbucle ...!... */         
         }

         if (posblock[nlevel] == 20 || posblock[nlevel-1] != 20)
         {
            			   
         }
         else
         {
            kp2 = ixxbloque5+posblock[nlevel];
            kp1 = ixxbloque5+posblock[nlevel]-(len+LCLAVE_POINTERS);
            if (ixxcompara_clave(kp1,kp2,&(ixxfich->claves[laclave]),1))
            {
               nnlevel = nlevel;
               while(nlevel)
               {
                  hijo = *((long *)(kp1+len));
                  posi = hijo * tamblock;
                  if (ixx_lseek(fd,posi,SEEK_SET)== -1L)
                  {
                     eret(IXXELSEEK,errno,-1);
                  }
                  if (ixx_lee(fd,ixxbloque5,ixxfich->tamblock) != ixxfich->tamblock)
                  {
                     eret(IXXEREAD,errno,-1);
                  }
                  kkks = *((short *)(ixxbloque5+18));
                  kkks--;
                  kp1 = ixxbloque5 + 20 + ((len+LCLAVE_POINTERS)*kkks);
                  nlevel--;
                  if (!nlevel)
                  {				    														
                     /* comprobar error */
                     kp2 = ixxbloque+20;
                     if (!ixxcompara_clave(kp1,kp2,&(ixxfich->claves[laclave]),1))
                     {
                        eret(IXXECORRUP,138,-1);
                     }                      
                  }
               }
               nlevel = nnlevel;
            }
            else
            {
              /*break;*/
            }
         }
      }	
   }

   
   ret(identica);
}



int ixxinicializa_bloclave(tambloque,numclave,level,padre,ixxbloq)
int tambloque;
short numclave;
short level;
long padre;
Uchar *ixxbloq;
{
   memset(ixxbloq,0,tambloque);
   *((short *)ixxbloq) = IXXIDBCLAVE;
   *((short *)(ixxbloq+2)) = numclave;
   *((short *)(ixxbloq+4)) = level;
   *((long *)(ixxbloq+6)) = padre;
   /* prev chain o total claves = 0 next chain = 0 nkeys = 0 */
   ret(0);
}

/* free list */
long ixxnuevo_registro_datos()
{
   long reg,pos,next,blo;
   short tamblock = ixxfich->tamblock;
   short nrecs;
   int fd = ixxfich->fd;
   
   blo = ixxfich->cabecera.recordfreelist;
   if ( blo )
			{
      if (blo <= 0L || blo >= ixxfich->cabecera.nblocks)
      {
         eret(IXXECORRUP,94,-1);
      }
      pos = blo * tamblock;
      if (ixx_lseek(fd,pos,SEEK_SET)== -1L)
      {
         IXXerror(IXXELSEEK,errno,-1);
         return( 0L );
      }
      if (ixx_lee(fd,ixxbloque4,tamblock) != tamblock)
      {
         IXXerror(IXXEREAD,errno,-1);
         return( 0L );
      }
      if (*((short *)ixxbloque4) != IXXIDBFRERE || *((short *)(ixxbloque4+2)) != IXXIDBFRERE)
      {
         IXXerror(IXXECORRUP,16,-1);
         return( 0L );
      }
      next = *((long *)(ixxbloque4+14));
      nrecs = *((short *)(ixxbloque4+18));
      if (!nrecs)
      {
         IXXerror(IXXECORRUP,17,-1);
         return( 0L );
      }
      reg = *((long *)(ixxbloque4+20+(nrecs-1)*sizeof(long)));
      if (!ixxfich->cabecera.nfreerecords)
      {
         IXXerror(IXXECORRUP,18,-1);
         return( 0L );
      }
      ixxfich->cabecera.nfreerecords--;
      nrecs--;
      if (!nrecs)
      {
         ixxfich->cabecera.recordfreelist = next;
         if (ixxlibera_bloque(blo))
            ret(-1);
      }
      else
      {
         *((short *)(ixxbloque4+18)) = nrecs;
         /* de momento hacer grabar solo lo justo */
         pos = blo * tamblock + 18;
         if (ixx_lseek(fd,pos,SEEK_SET)== -1L)
         {
            IXXerror(IXXELSEEK,errno,-1);
            return( 0L );
         }
         if (ixx_graba(fd,ixxbloque4+18,sizeof(short)) != sizeof(short))
         {
            IXXerror(IXXEWRITE,errno,-1);
            return( 0L );
         }
      }
			}
   else
			{
      if (ixxfich->cabecera.nfreerecords)
      {
         IXXerror(IXXECORRUP,19,-1);
         return( 0L );
      }
      ixxfich->cabecera.nrecords++;
      reg = ixxfich->cabecera.nrecords;
			}
   return( reg );
}

int ixxlibera_registro(reg)
long reg;
{
   long pos,next,blo;
   short tamblock = ixxfich->tamblock;
   short nrecs;
   short maxi = (tamblock - 24) / sizeof(long);
   int fd = ixxfich->fd;
   
#ifdef COMENTARIO
#ifdef FGVDOS
   if (ixxfich->cabecera.nrecords == reg)
			{
      ixxfich->cabecera.nrecords--;
      if (chsize(fd,ixxfich->cabecera.nrecords*ixxfich->tamreg) == 0)
      {
         ret(0);
      }
      ixxfich->cabecera.nrecords++;
			}
#endif
#endif
   
   next = 0L;
   blo = ixxfich->cabecera.recordfreelist;
   if (blo)
			{
      if (blo <= 0L || blo >= ixxfich->cabecera.nblocks)
      {
         eret(IXXECORRUP,95,-1);
      }
      if (!ixxfich->cabecera.nfreerecords)
      {
         IXXerror(IXXECORRUP,20,-1);
         return( 0L );
      }
      pos = blo * tamblock;
      if (ixx_lseek(fd,pos,SEEK_SET)== -1L)
      {
         eret(IXXELSEEK,errno,-1);
      }
      if (ixx_lee(fd,ixxbloque4,tamblock) != tamblock)
      {
         eret(IXXEREAD,errno,-1);
      }
      if (*((short *)ixxbloque4) != IXXIDBFRERE || *((short *)(ixxbloque4+2)) != IXXIDBFRERE)
      {
         eret(IXXECORRUP,21,-1);
      }
      nrecs = *((short *)(ixxbloque4+18));
      if (nrecs < maxi)
      {
         *((long *)(ixxbloque4+20+nrecs*sizeof(long))) = reg;
         nrecs++;
         *((short *)(ixxbloque4+18)) = nrecs;
         /* pos = blo * tamblock; YA ESTA */
         if (ixx_lseek(fd,pos,SEEK_SET)== -1L)
         {
            eret(IXXELSEEK,errno,-1);
         }
         if (ixx_graba(fd,ixxbloque4,tamblock) != tamblock)
         {
            eret(IXXEWRITE,errno,-1);
         }
         ixxfich->cabecera.nfreerecords++;
         ret(0);
      }
      next = blo;
			}
   else
			{
      if (ixxfich->cabecera.nfreerecords)
      {
         IXXerror(IXXECORRUP,22,-1);
         return( 0L );
      }
			}
   blo = ixxnuevo_bloque();
   if (!blo)
      ret(-1);
   ixxfich->cabecera.recordfreelist = blo;
   memset(ixxbloque4,0,tamblock);
   *((short *)ixxbloque4) = IXXIDBFRERE;
   *((short *)(ixxbloque4+2)) = IXXIDBFRERE;
   *((long *)(ixxbloque4+14)) = next;
   *((short *)(ixxbloque4+18)) = (short)1; /* nrecs */
   *((long *)(ixxbloque4+20)) = reg;
   pos = blo * tamblock;
   if (ixx_lseek(fd,pos,SEEK_SET)== -1L)
			{
      eret(IXXELSEEK,errno,-1);
			}
   if (ixx_graba(fd,ixxbloque4,tamblock) != tamblock)
			{
      eret(IXXEWRITE,errno,-1);
			}
   ixxfich->cabecera.nfreerecords++;
   ret(0);
}



long ixxnuevo_bloque()
{
			long reg,pos,next,blo;
         short tamblock = ixxfich->tamblock;
         short nrecs;
         int fd = ixxfich->fd;
         
         blo = ixxfich->cabecera.blockfreelist;
         if ( blo )
         {
            if (blo <= 0L || blo >= ixxfich->cabecera.nblocks)
            {
               eret(IXXECORRUP,95,-1);
            }
            pos = blo * tamblock;
            if (ixx_lseek(fd,pos,SEEK_SET)== -1L)
            {
               IXXerror(IXXELSEEK,errno,-1);
               return( 0L );
            }
            if (ixx_lee(fd,ixxbloque5,tamblock) != tamblock)
            {
               IXXerror(IXXEREAD,errno,-1);
               return( 0L );
            }
            if (*((short *)ixxbloque5) != IXXIDBFREBL || *((short *)(ixxbloque5+2)) != IXXIDBFREBL)
            {
               IXXerror(IXXECORRUP,23,-1);
               return( 0L );
            }
            next = *((long *)(ixxbloque5+14));
            nrecs = *((short *)(ixxbloque5+18));
            if (!nrecs)
            {
               /* este es el que se va ha usar, como estaba usado por la freelist no era libre!!! */
               ixxfich->cabecera.blockfreelist = next;
               ret( blo );
            }
            reg = *((long *)(ixxbloque5+20+(nrecs-1)*sizeof(long)));
            nrecs--;
            if (!ixxfich->cabecera.nfreeblocks)
            {
               IXXerror(IXXECORRUP,24,-1);
               return( 0L );
            }
            ixxfich->cabecera.nfreeblocks--;
            *((short *)(ixxbloque5+18)) = nrecs;
            /* de momento hacer grabar solo lo justo */
            pos = blo * tamblock + 18;
            if (ixx_lseek(fd,pos,SEEK_SET)== -1L)
            {
               IXXerror(IXXELSEEK,errno,-1);
               return( 0L );
            }
            if (ixx_graba(fd,ixxbloque5+18,sizeof(short)) != sizeof(short))
            {
               IXXerror(IXXEWRITE,errno,-1);
               return( 0L );
            }
         }
         else
         {
            if (ixxfich->cabecera.nfreeblocks)
            {
               IXXerror(IXXECORRUP,25,-1);
               return( 0L );
            }
            if (ixxfich->cabecera.nblocks >= ixxfich->maximo_bloques)
            {
               IXXerror(IXXECORRUP,107,-1);
               return( 0L );
            }
            reg = ixxfich->cabecera.nblocks;
            ixxfich->cabecera.nblocks++;
         }
         return( reg );
}

int ixxlibera_bloque(reg)
long reg;
{
			long pos,next,blo;
         short tamblock = ixxfich->tamblock;
         short nrecs;
         short maxi = (tamblock - 24) / sizeof(long);
         int fd = ixxfich->fd;
         
#ifdef COMENTARIO
#ifdef FGVDOS
         if (reg == (ixxfich->cabecera.nblocks-1))
         {
            ixxfich->cabecera.nblocks--;
            if (chsize( fd, ixxfich->cabecera.nblocks*tamblock ) == 0)
            {
               ret(0);
            }
            ixxfich->cabecera.nblocks++;
         }
#endif
#endif
         
         next = 0L;
         blo = ixxfich->cabecera.blockfreelist;
         if (blo)
         {
            if (blo <= 0L || blo >= ixxfich->cabecera.nblocks)
            {
               eret(IXXECORRUP,96,-1);
            }
            pos = blo * tamblock;
            if (ixx_lseek(fd,pos,SEEK_SET)== -1L)
            {
               eret(IXXELSEEK,errno,-1);
            }
            if (ixx_lee(fd,ixxbloque5,tamblock) != tamblock)
            {
               eret(IXXEREAD,errno,-1);
            }
            if (*((short *)ixxbloque5) != IXXIDBFREBL || *((short *)(ixxbloque5+2)) != IXXIDBFREBL)
            {
               eret(IXXECORRUP,26,-1);
            }
            nrecs = *((short *)(ixxbloque5+18));
            if (nrecs < maxi)
            {
               *((long *)(ixxbloque5+20+nrecs*sizeof(long))) = reg;
               nrecs++;
               *((short *)(ixxbloque5+18)) = nrecs;
               /* pos = blo * tamblock; YA ESTA */
               if (ixx_lseek(fd,pos,SEEK_SET)== -1L)
               {
                  eret(IXXELSEEK,errno,-1);
               }
               if (ixx_graba(fd,ixxbloque5,tamblock) != tamblock)
               {
                  eret(IXXEWRITE,errno,-1);
               }
               ixxfich->cabecera.nfreeblocks++;
               ret(0);
            }
            next = blo;
         }
         else
         {
            if (ixxfich->cabecera.nfreeblocks)
            {
               IXXerror(IXXECORRUP,27,-1);
               return( 0L );
            }
         }
         blo = reg; /* se usa el que se libera por tanto al seguir usado no se suma en freblocks */
         ixxfich->cabecera.blockfreelist = blo;
         memset(ixxbloque5,0,tamblock);
         *((short *)ixxbloque5) = IXXIDBFREBL;
         *((short *)(ixxbloque5+2)) = IXXIDBFREBL;
         *((long *)(ixxbloque5+14)) = next;
         *((short *)(ixxbloque5+18)) = (short)0; /* nrecs */
         
         if (blo <= 0L || blo >= ixxfich->cabecera.nblocks)
         {
            eret(IXXECORRUP,97,-1);
         }
         
         pos = blo * tamblock;
         if (ixx_lseek(fd,pos,SEEK_SET)== -1L)
         {
            eret(IXXELSEEK,errno,-1);
         }
         if (ixx_graba(fd,ixxbloque5,tamblock) != tamblock)
         {
            eret(IXXEWRITE,errno,-1);
         }
         ret(0);
}

static long ixx_suma_total_bloque(char *elbloque,short len)
{
short nkeys = *((short *)(elbloque+18));
short i;
long total = 0L;
char *p1;

    if (!(LCLAVE_TOTAL)) return 0;

    if (! *((short *)(elbloque+4)) ) return (long)nkeys;

    p1 = elbloque+20+len+LCLAVE_TOTAL;
    for (i = 0;i < nkeys;i++)
    {
        total += *((long *)p1);
        p1 += (len + LCLAVE_POINTERS);
    }

    return total;
}


int ixxinserta_clave(punto,laclave,datos,blo,posblock,reg,duplicadas)
int punto;
short laclave;
Uchar *datos;
long blo;
short *posblock;
long *reg;
int duplicadas;
{
   short level,clevel;
   long padre = -1L;
   short nkeys = *((short *)(ixxbloque+18));
   Uchar *p1,*p2,*p3;
   short len = ixxfich->claves[laclave].clave_len;
   short max = ixxfich->claves[laclave].clave_num;
   int nuevopadre;
   short k,expected = 1;
   int fd = ixxfich->fd;
   long pos,nblo = 0L;
   int tamblock = ixxfich->tamblock;
   short k3;
   Uchar *kp1,*kp2;
   int fuerza_partir = 0;
   int al_final = 0;
   long total_b1 = -1L,total_b2 = -1L;
   int flag_1 = 0;
   /* verificar que el bloque corriente esta posicionado correctamente */
   
   if (*((short *)ixxbloque) != IXXIDBCLAVE)
   {
      eret(IXXECORRUP,28,-1);
   }
   level = *((short *)(ixxbloque+4));
   if (level)
   {
      eret(IXXECORRUP,29,-1);
   }
   
   if (*((short *)(ixxbloque+2)) != laclave)
   {
      eret(IXXECORRUP,30,-1);
   }
   
   p1 = ixxbloque+*posblock;
   if (punto != 2)
   {
      p1 += (len+LCLAVE_POINTERS);
   }
   else
   {
      /* caso especial de ser la primerisima clave */
      padre = *((long *)(ixxbloque+6));
      
      /* actualizar la primera clave en niveles superiores */
      /* ojo no implementado sistema safe-mode */
      
      while(padre)
		{
         pos = padre;
         if (pos <= 0L || pos >= ixxfich->cabecera.nblocks)
         {
            eret(IXXECORRUP,31,-1);
         }
         pos *= tamblock;
         if (ixx_lseek(fd,pos,SEEK_SET)== -1L)
         {
            eret(IXXELSEEK,errno,-1);
         }
         if (ixx_lee(fd,ixxbloque2,ixxfich->tamblock) != ixxfich->tamblock)
         {
            eret(IXXEREAD,errno,-1);
         }
         
         if (*((short *)ixxbloque2) != IXXIDBCLAVE)
         {
            eret(IXXECORRUP,84,-1);
         }
         if (*((short *)(ixxbloque2+2)) != laclave)
         {
            eret(IXXECORRUP,85,-1);
         }
         if (expected  != *((short *)(ixxbloque2+4)))
         {
            eret(IXXECORRUP,86,-1);
         }
         
         /* comprobar duplicadas de encabezamiento */
         if (duplicadas == IXXCLAVEDUP && expected == (fuerza_partir+1))
         {
                kp2 = ixxbloque2+20+len+LCLAVE_POINTERS;
                kp1 = ixxbloque2+20;
                if (!ixxcompara_clave(kp1,kp2,&(ixxfich->claves[laclave]),1))
                {
                   fuerza_partir++;               
                }
         }
         
         memcpy(ixxbloque2+20,datos,len);
         
         if (ixx_lseek(fd,pos,SEEK_SET)== -1L)
         {
            eret(IXXELSEEK,errno,-1);
         }
         
         if (ixx_graba(fd,ixxbloque2,tamblock) != tamblock)
         {
            eret( IXXEWRITE , errno , fd);
         }
         padre = *((long *)(ixxbloque2+6));
         expected++;
         }
   }

   nuevopadre = 0;
   
   do {
      /* siempre en ixxbloque hay sitio para una clave mas */
      level = *((short *)(ixxbloque+4));
      clevel = level;
      p2 = ixxbloque+ 19 +(nkeys+1)*(len+LCLAVE_POINTERS); /* 19 por que p2 apunta al ultimo */
      p3 = p2 - (len+LCLAVE_POINTERS);
      if (p3<p1 && duplicadas == IXXCLAVEDUP)
      {
         al_final = 1;
      }
      else
      {
         al_final = 0;
      }
      while(p3>=p1)
      {
         *p2 = *p3;
         p2--;
         p3--;
      }
      memcpy(p1,datos,len);
      if (!level)
      {
         if (!*reg)
         {
            *reg = ixxnuevo_registro_datos();
            if (!*reg)
            {
               ret(-1);
            }
         }
         *((long *)(p1+len)) = *reg;

         if (LCLAVE_TOTAL)
            *((long *)(p1+len+LCLAVE_TOTAL)) = 1; /* en nivel 0 cada clave solo se tiene a si misma */
      }
      else
      {
         if (LCLAVE_TOTAL)
         {
             if (total_b2 == -1L)
             {
                 eret(IXXECORRUP,140,-1);
             }         
             *((long *)(p1+len+LCLAVE_TOTAL)) = total_b2; /* sumar claves del nivel apuntado!!! */
         }

         *((long *)(p1+len)) = nblo;
      }

      *(p1+len+4) = 0; /* longitud extendida */      

      nkeys++;

      if (al_final) /* se podria poner codigo para aprovechar esta lectura y no releer */
      {
          al_final = 0; /* comprobar caso de duplicada al final de un bloque con continuacion al 
                           siguiente */
          padre = *((long *)(ixxbloque+6));
          if (padre)
          {
             int nk1,nk2;
             pos = padre;
             pos *= tamblock;
             if (ixx_lseek(fd,pos,SEEK_SET)== -1L)
             {
                eret(IXXELSEEK,errno,-1);
             }
             if (ixx_lee(fd,ixxbloque5,ixxfich->tamblock) != ixxfich->tamblock)
             {
                eret(IXXEREAD,errno,-1);
             }

             nk1 = *(posblock+level+1) + (len+LCLAVE_POINTERS);
             nk2 = 20 + *((short *)(ixxbloque5+18)) * (len+LCLAVE_POINTERS);
             if (nk1 < nk2)
             {
                kp2 = ixxbloque5+nk1;
                kp1 = datos;
                if (!ixxcompara_clave(kp1,kp2,&(ixxfich->claves[laclave]),1))
                {
                    al_final = 1;
                }
             }
          }
      }

      if (!fuerza_partir && nkeys <= max && !al_final)
      {
         *((short *)(ixxbloque+18)) = nkeys;
         
         /* de momento grabacion con posible daño por corte */
         pos = blo;
         if (pos <= 0L || pos >= ixxfich->cabecera.nblocks)
         {
            eret(IXXECORRUP,32,-1);
         }
         pos *= tamblock;
         if (ixx_lseek(fd,pos,SEEK_SET)== -1L)
         {
            eret(IXXELSEEK,errno,-1);
         }
         
         /* poner back-up */
         
         if (ixx_graba(fd,ixxbloque,tamblock) != tamblock)
         {
            eret( IXXEWRITE , errno , -1);
         }         

         padre = *((long *)(ixxbloque+6));
         if (padre && nuevopadre)
         {
             eret(IXXECORRUP,142,-1);
         }


         if (LCLAVE_TOTAL)
         {
             total_b2 = ixx_suma_total_bloque(ixxbloque,len);
             flag_1 = 1;

             while(padre)
             {
                 pos = padre;
                 if (pos <= 0L || pos >= ixxfich->cabecera.nblocks)
                 {
                    eret(IXXECORRUP,143,-1);
                 }
                 pos *= tamblock;

                 level++;

                 pos += (*(posblock+level)+len+LCLAVE_TOTAL);

                 if (ixx_lseek(fd,pos,SEEK_SET)== -1L)
                 {
                    eret(IXXELSEEK,errno,-1);
                 }
                 if (flag_1)
                 {
                     if (ixx_graba(fd,(char *)&total_b2,sizeof(long)) != sizeof(long))
                     {
                        eret( IXXEWRITE , errno , -1);
                     }
                     flag_1 = 0;
                 }
                 else
                 {                     
                     if (ixx_lee(fd,(char *)&total_b2,sizeof(long)) != sizeof(long))
                     {
                        eret(IXXEREAD,errno,-1);
                     }
                     if (ixx_lseek(fd,pos,SEEK_SET)== -1L)
                     {
                        eret(IXXELSEEK,errno,-1);
                     }
                     total_b2++;
                     if (ixx_graba(fd,(char *)&total_b2,sizeof(long)) != sizeof(long))
                     {
                        eret( IXXEWRITE , errno , -1);
                     }
                 }

                 pos = padre;
                 pos *= tamblock;
                 pos += 6;             
                 if (ixx_lseek(fd,pos,SEEK_SET)== -1L)
                 {
                    eret(IXXELSEEK,errno,-1);
                 }
                 if (ixx_lee(fd,(char *)&padre,sizeof(long)) != sizeof(long))
                 {
                    eret(IXXEREAD,errno,-1);
                 }                
             }

             /*
             if (total_b2 != (ixxfich->cabecera.nrecords-ixxfich->cabecera.nfreerecords))
             {
                 eret(IXXECORRUP,144,-1);
             }
             */
         }
         

         ret(0);
						}
      else
						{
         if (nuevopadre)
         {
             eret(IXXECORRUP,141,-1);
         }

         padre = *((long *)(ixxbloque+6));
         /* pero si esta es en exceso se parte el bloque */
         if (!padre)
         {
            
            /* new level */
            
            padre = ixxnuevo_bloque();
            if (!padre)
            {
               ret(-1);
            }
            
            *((long *)(ixxbloque+6)) = padre;            
            
            pos = tamblock;
            pos *= (laclave+1);
            pos += 9;
            if (ixx_lseek(fd,pos,SEEK_SET)== -1L)
            {
               eret(IXXELSEEK,errno,-1);
            }
            if (ixx_graba(fd,(unsigned char *)&padre,sizeof(long)) != sizeof(long))
            {
               eret(IXXEWRITE,errno,-1);
            }
            level++;
            if (level >= IXXMAXLEVEL)
            {
               eret(IXXECORRUP,33,-1);
            }
            
            *(posblock+level) = 20;
            
            ixxinicializa_bloclave(tamblock,laclave,level,0L,ixxbloque3);
            *((short *)(ixxbloque3+18)) = 1;
            memcpy(ixxbloque3+20,ixxbloque+20,len);
            memcpy(ixxbloque3+20+len,&blo,4);
            nuevopadre = 1;
         }
         else
         {
            nuevopadre = 0;
         }
         
         memcpy(ixxbloque2,ixxbloque,18);
         if (al_final)
         {
            k = 1;
            nkeys--;
         }
         else if (fuerza_partir)
         {
            fuerza_partir--;
            k = nkeys-1;
            nkeys = 1;
         }
         else
         {
            k = nkeys/2;
            nkeys -= k;
            if (duplicadas == IXXCLAVEDUP)
            {
               kp2 = ixxbloque+20+nkeys*(len+LCLAVE_POINTERS);
               kp1 = ixxbloque+20;
               if (ixxcompara_clave(kp1,kp2,&(ixxfich->claves[laclave]),1))
               {
                  kp2 = ixxbloque+20+nkeys*(len+LCLAVE_POINTERS);
                  kp1 = kp2 - (len+LCLAVE_POINTERS);
                  for (k3 = nkeys;k3 > 0;k3--)
                  {
                     if (ixxcompara_clave(kp1,kp2,&(ixxfich->claves[laclave]),1))
                     {
                        break;
                     }
                     kp1 -= (len+LCLAVE_POINTERS);
                     kp2 -= (len+LCLAVE_POINTERS);
                  }
                  if (k3 < nkeys && k3 > 0)
                  {
                     k += (nkeys-k3);
                     nkeys = k3;
                  }
               }
            }
         }
         *((short *)(ixxbloque+18)) = nkeys;
         *((short *)(ixxbloque2+18)) = k;
         memcpy(ixxbloque2+20,ixxbloque+20+nkeys*(len+LCLAVE_POINTERS),k*(len+LCLAVE_POINTERS));
         
         /* poner back-up */
         
         pos = blo;
         if (pos <= 0L || pos >= ixxfich->cabecera.nblocks)
         {
            eret(IXXECORRUP,34,-1);
         }
         pos *= tamblock;
         if (ixx_lseek(fd,pos,SEEK_SET)== -1L)
         {
            eret(IXXELSEEK,errno,-1);
         }
         
         /* poner back-up */

         if (LCLAVE_TOTAL)
            total_b1 = ixx_suma_total_bloque(ixxbloque,len);
         
         if (ixx_graba(fd,ixxbloque,tamblock) != tamblock)
         {
            eret( IXXEWRITE , errno , -1);
         }
         
         nblo = ixxnuevo_bloque();
         if (!nblo)
         {
            ret(-1);
         }
         pos = nblo;
         
         if (pos <= 0L || pos >= ixxfich->cabecera.nblocks)
         {
            eret(IXXECORRUP,35,-1);
         }
         pos *= tamblock;
         if (ixx_lseek(fd,pos,SEEK_SET)== -1L)
         {
            eret(IXXELSEEK,errno,-1);
         }
         
         if (LCLAVE_TOTAL)
            total_b2 = ixx_suma_total_bloque(ixxbloque2,len);

         if (ixx_graba(fd,ixxbloque2,tamblock) != tamblock)
         {
            eret( IXXEWRITE , errno , -1);
         }
         
         
         if (clevel)
         {
            /* actualizar el padre en la nueva rama */
            nkeys = *((short *)(ixxbloque2+18));
            p2 = ixxbloque2+20+len;
            while(nkeys)
            {
               pos = *((long *)(p2));
               
               if (pos <= 0L || pos >= ixxfich->cabecera.nblocks)
               {
                  eret(IXXECORRUP,10,-1);
               }
               pos *= tamblock;
               
               /* ojo posicionamiento directo!!! */
               if (ixx_lseek(fd,pos+6L,SEEK_SET)== -1L)
               {
                  eret(IXXELSEEK,errno,-1);
               }
               /* y grabacion de solo lo justo !!!*/
               if (ixx_graba(fd,(unsigned char *)&nblo,4) != 4)
               {
                  eret( IXXEWRITE , errno , -1);
               }
               
               p2 += (len+LCLAVE_POINTERS);
               nkeys--;
            }
         }
         
         
         
         if (!nuevopadre)
         {
            pos = padre;
            if (pos <= 0L || pos >= ixxfich->cabecera.nblocks)
            {
               eret(IXXECORRUP,36,-1);
            }
            pos *= tamblock;
            if (ixx_lseek(fd,pos,SEEK_SET)== -1L)
            {
               eret(IXXELSEEK,errno,-1);
            }
            if (ixx_lee(fd,ixxbloque,tamblock) != tamblock)
            {
               eret( IXXEREAD , errno , -1);
            }
            level = *((short *)(ixxbloque+4));
            if (LCLAVE_TOTAL)
               *((long *)(ixxbloque+*(posblock+level)+len+LCLAVE_TOTAL)) = total_b1;
         }
         else
         {
            if (LCLAVE_TOTAL)
               *((long *)(ixxbloque3+20+len+LCLAVE_TOTAL)) = total_b1;
            memcpy(ixxbloque,ixxbloque3,tamblock);            
         }
         blo = padre;
         level = *((short *)(ixxbloque+4));
         p1 = ixxbloque+*(posblock+level)+len+LCLAVE_POINTERS;
         nkeys = *((short *)(ixxbloque+18));
         datos = ixxbloque2+20;
         if (*((short *)ixxbloque) != IXXIDBCLAVE)
         {
            eret(IXXECORRUP,37,-1);
         }
         if (*((short *)(ixxbloque+2)) != laclave)
         {
            eret(IXXECORRUP,38,-1);
         }
         }
        } while(padre);
        eret(IXXECORRUP,39,-1);
}


int ixx_graba_indice(ixx_fd,dat_reg,modo)
int ixx_fd;
Uchar *dat_reg;
int modo;
{
   int fd;
   short laclave;
   long blo,pos;
   long reg,rtmp = 0L,rwupdate;
   int t,r;
   int tamblock;
   int sale_mal = 0;
   int cu;
   int solo_indice = 0;
   int se_ha_hecho_algo = 0;
   long dat_records;
   
   AjustaPunterosSegunDatos(ixx_fd);
   
   ixx_e_fich = -1;
   if (ixx_ojo_recursion)
   {
      eret(IXXERECURS,0,-1);
   }
   ixx_ojo_recursion = 1;
   
   if (ixx_fd < 0 || ixx_fd >= IXXMAXFILES)
   {
      ixx_ojo_recursion = 0;
      eret(IXXEMALARGU,0,-1);
   }
   
   if (modo != IXXGNUEVO && modo != IXXGACTUA && modo != IXXGSOLOI)
   {
      ixx_ojo_recursion = 0;
      eret(IXXEMALARGU,1,-1);
   }
   
   if (!ixxfichs[ixx_fd])
   {
      ixx_ojo_recursion = 0;
      eret(IXXENOOPEN,0,-1);
   }
   
   ixxfich = ixxfichs[ixx_fd];
   ixx_e_fich = ixx_fd;
   fd = ixxfich->fd;
   tamblock = ixxfich->tamblock;
   dat_records = ixxfich->cabecera.nrecords;
 
   if ((ixxfich->flags_operacion & 1))
   {
      ixx_ojo_recursion = 0;
      eret(IXXEMULTIPLE,0,-1);
   }
   

   if (!ixxfich->tamreg)
   {
      ixx_ojo_recursion = 0;
      eret(IXXEMALFICH,0,-1);
   }
   
   
   if (ixxfich->openmode == IXXSOLOLEE)
   { /* solo input!!! */
      ixx_ojo_recursion = 0;
      eret(IXXENOSUPORT,0,-1);
   }
   
   if (ixxfich->fd_dat < 0)
   {
      ixxfich->fd_dat = (*ixxdat_abre)(ixxfich->nombredat,NULL,NULL,NULL,NULL,NULL,&(ixxfich->dtabla));
   }
   
   if (ixxfich->fd_dat < 0)
   {
      ixx_ojo_recursion = 0;
      eret(IXXENOOPEN,0,-1);
   }
   
   if (modo == IXXGSOLOI)
   {
      modo = IXXGNUEVO;
      solo_indice = 1;
   }
   
   if (modo == IXXGNUEVO &&
      (ixxfich->cabecera.nrecords - ixxfich->cabecera.nfreerecords) >= ixxfich->maximo_registros
      )
   {
      ixx_ojo_recursion = 0;
      eret(IXXENOMASRECORD,0,-1);
   }
   
   /* bloquear indice para grabacion */
   /******************************************/
   if (ixx_bloquea_escritura(fd,ixxfich->openmode))
   {
      ixx_ojo_recursion = 0;
      ret(-1);
   }
   
   
   /***** Caso DBF standar *****/
   if (ixx_datos_dbf == 1 && default_dbf_id)
   {
   ajuste_a_dbf(ixx_fd,dat_reg); /* hay que ajustar las comas flotantes a como van a quedar 
   al covertirlas a ascii */
   }
   /****************************/
   
   
   if ((!solo_indice || ixxfich->openmode != IXXEXCLUYE) && ixxtocado(fd))
   {
      ixx_ojo_recursion = 0;
      ixx_desbloquea_escritura(fd,ixxfich->openmode);
      ixx_reconstruye(ixx_fd);
      if (ixx_bloquea_escritura(fd,ixxfich->openmode))
      {
         ret(-1);
      }
      if (ixxtocado(fd))
      {
         eret(IXXETOCADO,0,-1);
      }
      ixx_ojo_recursion = 1;
   }
   
   if (ixxtouch(fd))
   {
      ixx_desbloquea_escritura(fd,ixxfich->openmode);
      ixx_ojo_recursion = 0;
      ret(-1);
   }
   
   if (!solo_indice || ixxfich->openmode != IXXEXCLUYE)
   {
      if (ixx_lseek(fd,OUPDATE,SEEK_SET)== -1L)
      {
         IXXerror(IXXELSEEK,errno,-1);
         sale_mal = 1;
         goto graba_sale;
      }
      if (ixx_lee(fd,(unsigned char *)&rwupdate,sizeof(long)) != sizeof(long))
      {
         IXXerror(IXXEREAD,errno,-1);
         sale_mal = 1;
         goto graba_sale;
      }
   }
   else
      rwupdate = ixxfich->cabecera.updatecount;
   
   if (rwupdate != ixxfich->cabecera.updatecount)
   {
      
      ixx_buf_anula(fd);
      
      ixxfich->blo_cur = 0L;
      ixxfich->pos_cur = 0;
      /* actualizar datos cabecera */
      if (ixx_lseek(fd,0L,SEEK_SET)== -1L)
      {
         IXXerror(IXXELSEEK,errno,-1);
         sale_mal = 1;
         goto graba_sale;
      }
      
      if (ixx_lee(fd,(unsigned char *)&(ixxfich->cabecera),sizeof(IXXCABEZA)) != sizeof(IXXCABEZA))
      {
         IXXerror(IXXEREAD,errno,-1);
         sale_mal = 1;
         goto graba_sale;
      }

      if (memcmp(ixxfich->cabecera.file_ver,VERSION_INDEX,4))
      {            
         IXXerror(IXXEMALFICH,2,-1);
         sale_mal = 1;
         goto graba_sale;
      }
      
      if (ixxfich->tamreg != ixxfich->cabecera.tamreg)
      {
         IXXerror(IXXECORRUP,40,-1);
         sale_mal = 1;
         goto graba_sale;
      }
      
      if (ixxfich->tamblock != ixxfich->cabecera.tamblock)
      {
         IXXerror(IXXECORRUP,41,-1);
         sale_mal = 1;
         goto graba_sale;
      }
      
      if (ixxfich->nclaves != ixxfich->cabecera.nkeys)
      {
         IXXerror(IXXECORRUP,42,-1);
         sale_mal = 1;
         goto graba_sale;
      }
   }
   else
      ixxfich->cabecera.touch = 1;
   
   if (modo == IXXGACTUA)
   {
      
      if (!ixxfich->reg_cur)
      {
         IXXerror(IXXENOCURR,0,-1);
         sale_mal = 1;
         goto graba_sale;
      }
      /* es obligatorio que el registro este bloqueado */
      r = ixx_reg_esta_bloqueado(ixxfich->reg_cur);
      if (r < 0 || r == 2)
      {
         sale_mal = 1;
         goto graba_sale;
      }
      /*
      if (r != 1)
      { */
      /* por ahora pase, siempre y cuando el registro se conserve */
      /*  IXXerror(IXXENOLOCK,0,-1); */
      /*      }
      */
      
      /* leer registro corriente */
      reg = ixxfich->reg_cur;
      rtmp = reg; /* importante */
      cu = (*ixxdat_lee)(ixxfich->fd_dat,ixxfich->reg_cur,Dat_Cur,ixxfich->tamreg);
      if (cu < 0)
      {
         sale_mal = 1;
         goto graba_sale;
      }
      if (cu) /* para poder regrabar debe estar bloqueado y entonces no pasa esto */
      {
         if (r == 1)
            IXXerror(IXXECORRUP,43,-1);
         else
            IXXerror(IXXENOLOCK,0,-1);
         sale_mal = 1;
         goto graba_sale;
      }
      
      (*ixxdat_clave)(ixxfich,Dat_Cur,Datos2,-1,reg,0,0,0);
      /* esto se deberia eliminar y obligar siempre a que este bloqueado */
      if (r != 1)
      {
         for (laclave = 0;laclave < ixxfich->nclaves;laclave++)
         {
            if (memcmp(ixxfich->DatosCurr[laclave],Datos2[laclave],ixxfich->claves[laclave].clave_len))
            {
               IXXerror(IXXENOLOCK,0,-1); 
               sale_mal = 1;
               goto graba_sale;
            }
         }
      }
      (*ixxdat_clave)(ixxfich,dat_reg,Datos,-1,reg,0,0,0);
      /* asegurarse de que las claves son buenas !! */
      for (laclave = 0;laclave < ixxfich->nclaves;laclave++)
      {
         if (!memcmp(Datos[laclave],Datos2[laclave],ixxfich->claves[laclave].clave_len))
            continue;
         if (ixxfich->claves[laclave].clave_tipo == IXXCLAVENODUP)
         {
            t = ixx_busca_clave(laclave,Datos[laclave],&blo,&rtmp,_Posblock,0);
            if (t < 0)
            {
               sale_mal = 1;
               goto graba_sale;
            }
            if (t == 1)
            {
               IXXerror(IXXEKEXIST,laclave,-1);
               sale_mal = 1;
               goto graba_sale;
            }
         }
      }
   }
   else
   {
      if (solo_indice)
         reg = isrecnum;
      else
         reg = 0L;
      se_ha_hecho_algo = 1; /* fijo */
   }
   
   
   for (laclave = 0;laclave < ixxfich->nclaves;laclave++)
   {
      if (modo == IXXGACTUA)
      {
      /* comprobar que la clave ha cambiado *
         /* si ha cambiado se elimina la antigua */
         /* sino continue */
         if (!memcmp(Datos[laclave],Datos2[laclave],ixxfich->claves[laclave].clave_len))
            continue;
         /* eliminar la clave */
         
         se_ha_hecho_algo = 1;
         
         if (ixxborra_clave(laclave,&rtmp) < 0)
         {
            if (ixxstatus == IXXENOKEY)
            { /* esto indica indice descuadrado */
               IXXerror(IXXECORRUP,93,-1);
            }
            sale_mal = 1;
            goto graba_sale;
         }
         
         if (rtmp != reg)
         {
            IXXerror(IXXECORRUP,/* 44 */1000+laclave,-1);
            sale_mal = 1;
            goto graba_sale;
         }
      }
      else
         (*ixxdat_clave)(ixxfich,dat_reg,Datos,laclave,reg,0,0,0);
      /* buscar clave */
      /* t = 0 no esta pero hay menor, t = 1 identica, 2 = menor que cualquiera,3 = no hay */
      /* recoger datos de la clave */
      
      if ((t = ixx_busca_clave(laclave,Datos[laclave],&blo,&rtmp,_Posblock,0)) < 0)
      {
         sale_mal = 1;
         goto graba_sale;
      }
      
      /*******/
      se_ha_hecho_algo = 1; /* asegurarse */
      
      if (t == 1)
      {
         if (ixxfich->claves[laclave].clave_tipo == IXXCLAVENODUP)
         {
            if (modo == IXXGACTUA)
            {
               IXXerror(IXXECORRUP,/* 45*/2000+laclave,-1);
               sale_mal = 1;
               goto graba_sale;
            }
            /* deshacer el entuerto !!!!! */
            for(t = laclave - 1;t >= 0;t--)
            {
               (*ixxdat_clave)(ixxfich,dat_reg,Datos2,(short)t,reg,0,0,0);
               if (ixxborra_clave((short)t,&rtmp) < 0)
               {
                  IXXerror(IXXECORRUP,46,-1);
                  sale_mal = 1;
                  goto graba_sale;
               }
            }
            if (laclave)
            {
               ixxlibera_registro(reg);
            }
            IXXerror(IXXEKEXIST,laclave,-1);
            sale_mal = 1;
            goto graba_sale;
         }
         else
         {
            IXXerror(IXXECORRUP,4700+laclave,-1);
            sale_mal = 1;
            goto graba_sale;
         }
      }
      if (t == 3)
      {
         /* clave vacia */
         pos = tamblock;
         pos *= (laclave+1);
         pos += 9;
         if (ixx_lseek(fd,pos,SEEK_SET)== -1L)
         {
            IXXerror(IXXELSEEK,errno,-1);
            sale_mal = 1;
            goto graba_sale;
         }
         if (ixx_lee(fd,(unsigned char *)&blo,sizeof(long)) != sizeof(long))
         {
            IXXerror(IXXEREAD,errno,-1);
            sale_mal = 1;
            goto graba_sale;
         }
         if (!blo)
         {
            ixxinicializa_bloclave(tamblock,laclave,(short)0,0L,ixxbloque);
            blo = ixxnuevo_bloque();
            if (!blo)
            {
               sale_mal = 1;
               goto graba_sale;
            }
            if (ixx_lseek(fd,pos,SEEK_SET)== -1L)
            {
               IXXerror(IXXELSEEK,errno,-1);
               sale_mal = 1;
               goto graba_sale;
            }
            if (ixx_graba(fd,(unsigned char *)&blo,sizeof(long)) != sizeof(long))
            {
               IXXerror(IXXEWRITE,errno,-1);
               sale_mal = 1;
               goto graba_sale;
            }
         }
         *_Posblock = 20;
         t = 2; /* es la primera */
      }
      
      if (ixxinserta_clave(t,laclave,Datos[laclave],blo,_Posblock,&reg,ixxfich->claves[laclave].clave_tipo) < 0)
      {
         sale_mal = 1;
         goto graba_sale;
      }
      
      /*******/
  }
  
  
  /* actualizar datos cabecera */
graba_sale:
  
  ixx_buf_flush(fd);
  
  if (se_ha_hecho_algo)
  {
     
  /* sea como fuere reposicionar el corriente pues las posiciones de nivel guardadas
     pueden no ser correctas */
     ixxfich->blo_cur = 0L;
     ixxfich->pos_cur = 0;
     
     /* actualizar datos de la clave del corriente logico */
     if (modo == IXXGACTUA && !sale_mal)
     {
        (*ixxdat_clave)(ixxfich,dat_reg,ixxfich->DatosCurr,-1,reg,0,0,0);
     }
     
     ixxfich->cabecera.updatecount++;
     
     if (!solo_indice  || ixxfich->openmode != IXXEXCLUYE)
     {
        /* aunque este mal la cosa sepone como ha quedado */
        if (ixx_lseek(fd,0L,SEEK_SET)== -1L)
        {
           IXXerror(IXXELSEEK,errno,-1);
           sale_mal = 1;
           goto graba_salextra;
        }
        if (ixx_graba(fd,(unsigned char *)&(ixxfich->cabecera),sizeof(IXXCABEZA)) != sizeof(IXXCABEZA))
        {
           IXXerror(IXXEWRITE,errno,-1);
           sale_mal = 1;
           goto graba_salextra;
        }
     }
  }
  
graba_salextra:
  
  
  if (sale_mal &&
     (ixxstatus == IXXECORRUP || ixxstatus == IXXEREAD ||
     ixxstatus == IXXEWRITE  || ixxstatus == IXXELSEEK ||
     ixxstatus == IXXENOSUPORT || ixxstatus == IXXETOCADO)
     )
  {
     /* deja el archivo como "jodido" */
     ixx_desbloquea_escritura(fd,ixxfich->openmode);
     ixx_ojo_recursion = 0;
     ret(-1);
  }
  
  
  if (!sale_mal && !solo_indice)
  {
     if ((*ixxdat_graba)(ixxfich->fd_dat,reg,dat_reg,ixxfich->tamreg) < 0)
			  {
        sale_mal = 1;
			  }
     else
			  {
        /* sistema asegurate */
        
        if (!solo_indice && dat_records < ixxfich->cabecera.nrecords)
        {
           if ((*ixxdat_cierra)(ixxfich->fd_dat) >= 0)
           {
              ixxfich->fd_dat = (*ixxdat_abre)(ixxfich->nombredat,NULL,NULL,NULL,NULL,NULL,&(ixxfich->dtabla));
              if (ixxfich->fd_dat < 0)
              {
                 IXXerror(IXXECORRUP,112,-1);
                 sale_mal = 1;
              }
           }
        }
        
			  }
  }
  
  /* esta funcion es la unica no chequeada pues si no consigue quitar la marca
  el indexado se dara por estropeado de todos modos */
  if (!solo_indice || ixxfich->openmode != IXXEXCLUYE)
     ixxuntouch(fd);
  
  /* desbloquear */
  
  if (ixx_desbloquea_escritura(fd,ixxfich->openmode))
  {
     sale_mal = 1;
  }
  else
  {
     if (ixxfich->openmode == IXXCOMPARTE && !ixxfich->numero_bloqueos &&
        ixxfich->control_numblocks < ixxfich->cabecera.nblocks)
			  {
        if (ixx_cierra(fd) >= 0)
        {
           ixxfich->fd = ixx_abre_share(ixxfich->nombreixx);
           if (ixxfich->fd < 0)
           {
              IXXerror(IXXECORRUP,114,-1);
              sale_mal = 1;
           }
        }
        ixxfich->control_numblocks = ixxfich->cabecera.nblocks;
        /* OJO fd ya no es valido */
			  }
  }
  ixx_ojo_recursion = 0;
  if (sale_mal)
     ret(-1);
  else
     isrecnum = reg;
  if (modo == IXXGNUEVO)
     _e_log_write++;
  else
     _e_log_update++;
  bret(0);
}


int ixxborra_clave(laclave,reg)
short laclave;
long *reg;
{
   short *posblock = _Posblock;
   int punto;
   long blo,pos,padre,oreg = *reg;
   Uchar *p1,*p2,*p3;
   int fd = ixxfich->fd,i;
   short level,nkeys;
   short tamblock = ixxfich->tamblock;
   int len = ixxfich->claves[laclave].clave_len;
   long total_b1 = -1L;
   int flag_1 = 0;
   int avance_20 = 0;
   
   punto = ixx_busca_clave(laclave,Datos2[laclave],&blo,reg,posblock,0);
   if (punto < 0)
      ret( -1 );
   if (punto != 1)
   {
      eret(IXXENOKEY,-1,0);
   }
   
   /* verificar bloqueo en caso de primaria y reg original = 0!!! */
   if (!laclave && !oreg)
   {
      i = ixx_reg_esta_bloqueado(*reg);
      if (i < 0)
      {
         ret( -1 );
      }
      if (i == 2)
      {
         eret(IXXELOCKED,-1,0);
      }
   }
   
   level = 0;
   do
   {
      if (*((short *)ixxbloque) != IXXIDBCLAVE)
      {
         eret(IXXECORRUP,48,-1);
      }
      if (*((short *)(ixxbloque+2)) != laclave)
      {
         eret(IXXECORRUP,49,-1);
      }
      if (level != *((short *)(ixxbloque+4)))
      {
         eret(IXXECORRUP,50,-1);
      }
      padre = *((long *)(ixxbloque+6));
      nkeys = *((short *)(ixxbloque+18));
      p1 = ixxbloque + *posblock;
      
      /* verificacion especial ralentiza en muy poco y previene desastres */
      /* antes de borrar la clave esta debe coincidir lo que se va ha borrar
      con lo que se pretende borrar */
      if (memcmp(p1,Datos2[laclave],len) != 0)
      {
         eret(IXXECORRUP,88,-1);
      }
      
      p2 = p1 + len + LCLAVE_POINTERS;
      p3 = ixxbloque + 20 + nkeys * (len+LCLAVE_POINTERS);
      while(p2 < p3)
      {
         *p1 = *p2;
         p1++;
         p2++;
      }
      nkeys--;
      *((short *)(ixxbloque+18)) = nkeys;
      if (nkeys || !padre)
      {
         pos = blo;
         if (!nkeys && !padre)
         {
            *((short *)(ixxbloque+4)) = (short)0;
         }
         if (pos <= 0L || pos >= ixxfich->cabecera.nblocks)
         {
            eret(IXXECORRUP,51,-1);
         }
         pos *= tamblock;
         if (ixx_lseek(fd,pos,SEEK_SET)== -1L)
         {
            eret(IXXELSEEK,errno,-1);
         }
         if (ixx_graba(fd,ixxbloque,tamblock) != tamblock)
         {
            eret( IXXEWRITE , errno , -1);
         }
         if (LCLAVE_TOTAL)
         {
             total_b1 = ixx_suma_total_bloque(ixxbloque,len);
             flag_1 = 1;
         }

         /* caso de la primera clave actualizar niveles superiores */
         avance_20 = 1;
         while(padre && (*posblock == 20 || LCLAVE_TOTAL))
         {
            pos = padre;
            if (pos <= 0L || pos >= ixxfich->cabecera.nblocks)
            {
               eret(IXXECORRUP,91,-1);
            }
            pos *= tamblock;

            if (*posblock == 20 && avance_20)
            {

                if (ixx_lseek(fd,pos,SEEK_SET)== -1L)
                {
                   eret(IXXELSEEK,errno,-1);
                }
                if (ixx_lee(fd,ixxbloque2,tamblock) != tamblock)
                {
                   eret( IXXEREAD , errno , -1);
                }
                if (*((short *)ixxbloque2) != IXXIDBCLAVE)
                {
                   eret(IXXECORRUP,89,-1);
                }
                if (*((short *)(ixxbloque2+2)) != laclave)
                {
                   eret(IXXECORRUP,90,-1);
                }
                level++;            
                if (level != *((short *)(ixxbloque2+4)))
                {
                   eret(IXXECORRUP,52,-1);
                }
                posblock++;
                if (LCLAVE_TOTAL)
                {
                    *((long *)(ixxbloque2+*posblock+len+LCLAVE_TOTAL)) = total_b1;
                    total_b1 = ixx_suma_total_bloque(ixxbloque2,len);
                    flag_1 = 1;
                }

                memcpy(ixxbloque2+*posblock,ixxbloque+20,len);

                if (ixx_lseek(fd,pos,SEEK_SET)== -1L)
                {
                   eret(IXXELSEEK,errno,-1);
                }
                if (ixx_graba(fd,ixxbloque2,tamblock) != tamblock)
                {
                   eret( IXXEWRITE , errno , -1);
                }
                padre = *((long *)(ixxbloque2+6));

            }
            else
            {
                avance_20 = 0;
                posblock++;
                pos += ((*posblock)+len+LCLAVE_TOTAL);
                if (ixx_lseek(fd,pos,SEEK_SET)== -1L)
                {
                   eret(IXXELSEEK,errno,-1);
                }
                if (flag_1)
                {
                    if (ixx_graba(fd,(char *)&total_b1,sizeof(long)) != sizeof(long))
                    {
                       eret( IXXEWRITE , errno , -1);
                    }
                    flag_1 = 0;
                }
                else
                {
                    if (ixx_lee(fd,(char *)&total_b1,sizeof(long)) != sizeof(long))
                    {
                       eret( IXXEREAD , errno , -1);
                    }
                    total_b1--;
                    if (ixx_lseek(fd,pos,SEEK_SET)== -1L)
                    {
                       eret(IXXELSEEK,errno,-1);
                    }
                    if (ixx_graba(fd,(char *)&total_b1,sizeof(long)) != sizeof(long))
                    {
                       eret( IXXEWRITE , errno , -1);
                    }
                }
                pos = padre;
                pos *= tamblock;
                pos += 6;
                level++;
                if (ixx_lseek(fd,pos,SEEK_SET)== -1L)
                {
                   eret(IXXELSEEK,errno,-1);
                }
                if (ixx_lee(fd,(char *)&padre,sizeof(long)) != sizeof(long))
                {
                   eret( IXXEREAD , errno , -1);
                }
            }
         }

         /*
         if (LCLAVE_TOTAL)
         {
             if (total_b1 != (ixxfich->cabecera.nrecords-ixxfich->cabecera.nfreerecords))
             {
                 eret(IXXECORRUP,144,-1);
             }
         }
         */

         ret(0);
      }
      if (ixxlibera_bloque(blo))
      {
         ret(-1);
      }
      posblock++;
      level++;
      blo = padre;
      pos = blo;
      if (pos <= 0L || pos >= ixxfich->cabecera.nblocks)
      {
         eret(IXXECORRUP,53,-1);
      }
      pos *= tamblock;
      if (ixx_lseek(fd,pos,SEEK_SET)== -1L)
      {
         eret(IXXELSEEK,errno,-1);
      }
      if (ixx_lee(fd,ixxbloque,tamblock) != tamblock)
      {
         eret( IXXEREAD , errno , -1);
      }
        } while(padre);
        eret(IXXECORRUP,54,-1);
}

int ixx_borra_indice(ixx_fd,dat_reg,modo)
int ixx_fd;
Uchar *dat_reg;
int modo;
{
   int fd;
   short laclave;
   long reg,rtmp,rwupdate;
   Uchar *data;
   int sale_mal = 0;
   int cu,t,r;
   
   AjustaPunterosSegunDatos(ixx_fd);
   
   if (ixx_ojo_recursion)
   {
      eret(IXXERECURS,0,-1);
   }
   ixx_ojo_recursion = 1;
   
   ixx_e_fich = -1;
   if (ixx_fd < 0 || ixx_fd >= IXXMAXFILES)
   {
      ixx_ojo_recursion = 0;
      eret(IXXEMALARGU,0,-1);
   }
   
   if (modo != IXXBCLAVE && modo != IXXBACTUA)
   {
      ixx_ojo_recursion = 0;
      eret(IXXEMALARGU,1,-1);
   }
   
   if (!ixxfichs[ixx_fd])
   {
      ixx_ojo_recursion = 0;
      eret(IXXENOOPEN,0,-1);
   }
   
   ixxfich = ixxfichs[ixx_fd];
   fd = ixxfich->fd;
   ixx_e_fich = ixx_fd;
   
   if (!ixxfich->tamreg)
   {
      ixx_ojo_recursion = 0;
      eret(IXXEMALFICH,0,-1);
   }

   if ((ixxfich->flags_operacion & 1))
   {
      ixx_ojo_recursion = 0;
      eret(IXXEMULTIPLE,0,-1);
   }
   
   if (ixxfich->openmode == IXXSOLOLEE)
   { /* solo input!!! */
      ixx_ojo_recursion = 0;
      eret(IXXENOSUPORT,0,-1);
   }
   
   if (ixxfich->fd_dat < 0)
   {
      ixxfich->fd_dat = (*ixxdat_abre)(ixxfich->nombredat,NULL,NULL,NULL,NULL,NULL,&(ixxfich->dtabla));
   }
   
   if (ixxfich->fd_dat < 0)
   {
      ixx_ojo_recursion = 0;
      eret(IXXENOOPEN,0,-1);
   }
   
   /* bloquear indice para grabacion */
   /******************************************/
   if (ixx_bloquea_escritura(fd,ixxfich->openmode))
   {
      ixx_ojo_recursion = 0;
      ret(-1);
   }
   
   
   if (ixxtocado(fd))
   {
      ixx_ojo_recursion = 0;
      ixx_desbloquea_escritura(fd,ixxfich->openmode);
      ixx_reconstruye(ixx_fd);
      if (ixx_bloquea_escritura(fd,ixxfich->openmode))
      {
         ret(-1);
      }
      if (ixxtocado(fd))
      {
         eret(IXXETOCADO,0,-1);
      }
      ixx_ojo_recursion = 1;
   }
   
   if (ixxtouch(fd))
   {
      ixx_desbloquea_escritura(fd,ixxfich->openmode);
      ixx_ojo_recursion = 0;
      ret(-1);
   }
   
   /* actualizar datos cabecera */
   
   if (ixx_lseek(fd,OUPDATE,SEEK_SET)== -1L)
   {
      IXXerror(IXXELSEEK,errno,-1);
      sale_mal = 1;
      goto borra_sale;
   }
   if (ixx_lee(fd,(unsigned char *)&rwupdate,sizeof(long)) != sizeof(long))
   {
      IXXerror(IXXEREAD,errno,-1);
      sale_mal = 1;
      goto borra_sale;
   }
   
   if (rwupdate != ixxfich->cabecera.updatecount)
   {
      
      ixx_buf_anula(fd);
      
      ixxfich->blo_cur = 0L;
      ixxfich->pos_cur = 0;
      if (ixx_lseek(fd,0L,SEEK_SET)== -1L)
      {
         IXXerror(IXXELSEEK,errno,-1);
         sale_mal = 1;
         goto borra_sale;
      }
      if (ixx_lee(fd,(unsigned char *)&(ixxfich->cabecera),sizeof(IXXCABEZA)) != sizeof(IXXCABEZA))
      {
         IXXerror(IXXEREAD,errno,-1);
         sale_mal = 1;
         goto borra_sale;
      }

      if (memcmp(ixxfich->cabecera.file_ver,VERSION_INDEX,4))
      {            
         IXXerror(IXXEMALFICH,2,-1);
         sale_mal = 1;
         goto borra_sale;
      }
      
      if (ixxfich->tamreg != ixxfich->cabecera.tamreg)
      {
         IXXerror(IXXECORRUP,55,-1);
         sale_mal = 1;
         goto borra_sale;
      }
      
      if (ixxfich->tamblock != ixxfich->cabecera.tamblock)
      {
         IXXerror(IXXECORRUP,56,-1);
         sale_mal = 1;
         goto borra_sale;
      }
      
      if (ixxfich->nclaves != ixxfich->cabecera.nkeys)
      {
         IXXerror(IXXECORRUP,57,-1);
         sale_mal = 1;
         goto borra_sale;
      }
   }
   else
      ixxfich->cabecera.touch = 1;
   
   
   if (modo == IXXBACTUA)
   {
      /* leer registro corriente */
      
      if (!ixxfich->reg_cur)
      {
         IXXerror(IXXENOCURR,0,-1);
         sale_mal = 1;
         goto borra_sale;
      }
      
      /* es obligatorio que el registro este bloqueado */
      r = ixx_reg_esta_bloqueado(ixxfich->reg_cur);
      if (r < 0 || r == 2)
      {
         sale_mal = 1;
         goto borra_sale;
      }
      
      cu = (*ixxdat_lee)(ixxfich->fd_dat,ixxfich->reg_cur,Dat_Cur,ixxfich->tamreg);
      if (cu < 0)
      {
         sale_mal = 1;
         goto borra_sale;
      }
      if (cu)
      {
         if (r != 1)
         {
            IXXerror(IXXENOLOCK,0,-1);
         }
         else
         {
            IXXerror(IXXECORRUP,58,-1);
         }
         sale_mal = 1;
         goto borra_sale;
      }
      reg = ixxfich->reg_cur;
      data = Dat_Cur;
      
      if (r != 1)
      {
         (*ixxdat_clave)(ixxfich,data,Datos2,-1,reg,0,0,0);
         for (laclave = 0;laclave < ixxfich->nclaves;laclave++)
         {
            if (memcmp(ixxfich->DatosCurr[laclave],Datos2[laclave],ixxfich->claves[laclave].clave_len))
            {
               IXXerror(IXXENOLOCK,0,-1);
               sale_mal = 1;
               goto borra_sale;
            }
         }
      }
      
   }
   else
   {
   /*
   if (ixxfich->clave_activa)
   {
      */
      if (ixxfich->clave_activa == ixxfich->nclaves)
      {
         cu = (*ixxdat_lee)(ixxfich->fd_dat,isrecnum,Dat_Cur,ixxfich->tamreg);
         if (cu < 0)
         {
            sale_mal = 1;
            goto borra_sale;
         }
         if (cu)
         {
            IXXerror(IXXENOKEY,0,-1);
            sale_mal = 1;
            goto borra_sale;
         }
         data = Dat_Cur;
         reg = isrecnum;
      }
      else
      {
         (*ixxdat_clave)(ixxfich,dat_reg,Datos,ixxfich->clave_activa,0L,0,0,0);
         t = ixx_busca_clave(ixxfich->clave_activa,Datos[ixxfich->clave_activa],&rtmp,&reg,_Posblock,1+IXXEQUAL);
         if (t < 0)
         {
            sale_mal = 1;
            goto borra_sale;
         }
         if (t != 1)
         {
            IXXerror(IXXENOKEY,0,-1);
            sale_mal = 1;
            goto borra_sale;
         }
         cu = (*ixxdat_lee)(ixxfich->fd_dat,reg,Dat_Cur,ixxfich->tamreg);
         if (cu < 0)
         {
            sale_mal = 1;
            goto borra_sale;
         }
         if (cu)
         {
            IXXerror(IXXECORRUP,59,-1);
            sale_mal = 1;
            goto borra_sale;
         }
         data = Dat_Cur;
      }
      /*
      }
      else
      {
      reg = 0L; 
      data = dat_reg;
      }
      */
   }
   
   
   for (laclave = 0;laclave < ixxfich->nclaves;laclave++)
   {
      /* comprobar variacion de reg entre claves !!!! */
      (*ixxdat_clave)(ixxfich,data,Datos2,laclave,reg,0,0,0);
      
      if (ixxborra_clave(laclave,&rtmp) < 0)
      {
         if ((laclave || modo == IXXBACTUA) && ixxstatus == IXXENOKEY)
         {
            IXXerror(IXXECORRUP,92,-1);
         }
         sale_mal = 1;
         goto borra_sale;
      }
      
      if ((modo == IXXBACTUA || laclave) && rtmp != reg)
      {
         IXXerror(IXXECORRUP,60,-1);
         sale_mal = 1;
         goto borra_sale;
      }
      
      if (modo == IXXBCLAVE && !laclave && !reg)
      {
         reg = rtmp;
         /* resto de informacion segun datos grabados no de lo que me ponga!!! */
         if ((*ixxdat_lee)(ixxfich->fd_dat,reg,Dat_Cur,ixxfich->tamreg))
         {
            IXXerror(IXXECORRUP,61,-1);
            sale_mal = 1;
            goto borra_sale;
         }
         data = Dat_Cur;
      }
   }
   
   if (ixxlibera_registro(reg))
   {
      sale_mal = 1;
      goto borra_sale;
   }
   
borra_sale:
   
   ixx_buf_flush(fd);
   
   /*if (reg == ixxfich->reg_cur)
			{ */
   /* hacerlo siempre pues pueden haber cambiado los niveles anterires!!! */
   /* se asegura de que lo rebusque otra vez!!! */
   ixxfich->blo_cur = 0L;
   ixxfich->pos_cur = 0;
   /*        } */
   
   /* actualizar datos cabecera */
   ixxfich->cabecera.updatecount++;
   
   if (ixx_lseek(fd,0L,SEEK_SET)== -1L)
			{
      IXXerror(IXXELSEEK,errno,-1);
      sale_mal = 1;
      goto borra_salextra;
			}
   if (ixx_graba(fd,(unsigned char *)&(ixxfich->cabecera),sizeof(IXXCABEZA)) != sizeof(IXXCABEZA))
			{
      IXXerror(IXXEWRITE,errno,-1);
      sale_mal = 1;
      goto borra_salextra;
			}
   
   if (!sale_mal)
			{
      if ((*ixxdat_borra)(ixxfich->fd_dat,reg,ixxfich->tamreg) < 0)
      {
         sale_mal = 1;
         goto borra_salextra;
      }
			}
   
borra_salextra:
         if (sale_mal &&
            (ixxstatus == IXXECORRUP || ixxstatus == IXXEREAD ||
            ixxstatus == IXXEWRITE  || ixxstatus == IXXELSEEK ||
            ixxstatus == IXXENOSUPORT || ixxstatus == IXXETOCADO)
            )
         {
            /* deja el archivo como "jodido" */
            ixx_desbloquea_escritura(fd,ixxfich->openmode);
            ixx_ojo_recursion = 0;
            ret(-1);
         }
         
         ixxuntouch(fd);
         /* desbloquear */
         if (ixx_desbloquea_escritura(fd,ixxfich->openmode))
            sale_mal = 1;
         else
         {
            if (ixxfich->openmode == IXXCOMPARTE && !ixxfich->numero_bloqueos &&
               ixxfich->control_numblocks < ixxfich->cabecera.nblocks)
            {
               if (ixx_cierra(fd) >= 0)
               {
                  ixxfich->fd = ixx_abre_share(ixxfich->nombreixx);
                  if (ixxfich->fd < 0)
                  {
                     IXXerror(IXXECORRUP,113,-1);
                     sale_mal = 1;
                  }
               }
               ixxfich->control_numblocks = ixxfich->cabecera.nblocks;
               /* OJO fd ya no es valido */
            }
         }
         ixx_ojo_recursion = 0;
         if (sale_mal) ret(-1);
         _e_log_delete++;
         bret(0);
}

static int ixxbusca_contenido(Uchar *dato_clave,IXXCLAVE *clave)
{
   short len;
   char tip;
   int i,j,k;
   int n,on;
   int lenc,lo;
   Uchar *p1 = dato_clave,*p2 = ixxfich->contenido;
   
   n = 0;
   
   if (!ixxfich->contenidos[n]) ret(0);
   for (i = 0;i < clave->clave_ncampos;i++)
   {
      len = clave->clave_campos[i].len;
      tip = clave->clave_campos[i].type;
      if (tip == IXXCALFA)
      {
         lenc = ixxfich->contenidos[n];
         if (lenc <= len)
         {
            on = n;
            for (j = 0;j <= (len-lenc);j++)
            {
               if (*p2 == *(p1+j))
               {
                  if (!memcmp(p2+1,p1+j+1,lenc-1))
                  {
                     n++;
                     if (!ixxfich->contenidos[n])
                     {
                        ret(0);
                     }
                     p2 += lenc;
                     break;
                  }
               }
            }
            if (on != n)
            {
               i = -1;
               p1 = dato_clave;
            }
         }
      }
      if (i != -1)
         p1 += len;
   }
   ret(1);
}

int ixx_pon_contenido(ixx_fd,con,len)
int ixx_fd;
Uchar *con;
int len;
{
   int n;
   int nlen;
   
   AjustaPunterosSegunDatos(ixx_fd);
   
   ixx_e_fich = -1;
   if (ixx_fd < 0 || ixx_fd >= IXXMAXFILES || !len)
   {
      eret(IXXEMALARGU,0,-1);
   }
   if (!ixxfichs[ixx_fd])
   {
      eret(IXXENOOPEN,0,-1);
   }
   ixxfich = ixxfichs[ixx_fd];
   ixx_e_fich = ixx_fd;
   n = 0;
   nlen =0;
   if (!ixxfich->contenido)
   {
      ixxfich->contenido = malloc(len);
   }
   else
   {
      while(ixxfich->contenidos[n]) {nlen += ixxfich->contenidos[n];n++;}
      if ((n+1) >= IXXMAXCONTENIDO)
      {
         eret(IXXENOSUPORT,0,-1);
      }
      ixxfich->contenido = realloc(ixxfich->contenido,nlen+len);
   }
   memcpy(ixxfich->contenido+nlen,con,len);
   ixxfich->contenidos[n] = len;
   ixxfich->contenidos[n+1] = 0;
   ret(0);
}

int ixx_fin_contenido(ixx_fd)
int ixx_fd;
{
   AjustaPunterosSegunDatos(ixx_fd);
   
   ixx_e_fich = -1;
   if (ixx_fd < 0 || ixx_fd >= IXXMAXFILES)
			{
      ixx_ojo_recursion = 0;
      eret(IXXEMALARGU,0,-1);
			}
   if (!ixxfichs[ixx_fd])
			{
      ixx_ojo_recursion = 0;
      eret(IXXENOOPEN,0,-1);
			}
   ixxfich = ixxfichs[ixx_fd];
   ixx_e_fich = ixx_fd;
   if (ixxfich->contenido)
			{
      free(ixxfich->contenido);
      ixxfich->contenido = NULL;
			}
   ixxfich->contenidos[0] = 0;
   ret(0);
}

int ixx_es_el_curr(ixx_fd,dat_reg)
int ixx_fd;
Uchar *dat_reg; 
{
			int fd;
         int sale_mal = 0;
         int laclave = 0;
         int salida = 0;
         IXXCLAVE *clave;
         
         if (ixx_ojo_recursion)
         {
            eret(IXXERECURS,0,-1);
         }
         ixx_ojo_recursion = 1;
         ixx_e_fich = -1;
         if (ixx_fd < 0 || ixx_fd >= IXXMAXFILES)
         {
            ixx_ojo_recursion = 0;
            eret(IXXEMALARGU,0,-1);
         }
         
         if (!ixxfichs[ixx_fd])
         {
            ixx_ojo_recursion = 0;
            eret(IXXENOOPEN,0,-1);
         }
         
         ixx_e_fich = ixx_fd;
         ixxfich = ixxfichs[ixx_fd];
         fd = ixxfich->fd;
         
         if (ixxfich->fd_dat < 0)
         {
            ixxfich->fd_dat = (*ixxdat_abre)(ixxfich->nombredat,NULL,NULL,NULL,NULL,NULL,&(ixxfich->dtabla));
         }
#ifdef CONTROLNLM
         pprint(2501,"        ");
         sleep(1);
#endif
         
         if (ixxfich->fd_dat < 0)
         {
            ixx_ojo_recursion = 0;
            eret(IXXENOOPEN,0,-1);
         }
         
         if (!ixxfich->tamreg)
         {
            ixx_ojo_recursion = 0;
            eret(IXXEMALFICH,0,-1);
         }
         
         
         if (!ixxfich->reg_cur)
         {
            ixx_ojo_recursion = 0;
            eret(IXXENOCURR,0,-1);
         }
         
         
         /* bloquear uso de lectura */
         if (!(ixxfich->flags_operacion & 1))
         {         
             if (ixx_bloquea_lectura(fd,ixxfich->openid,ixxfich->openmode))
             {
                ixx_ojo_recursion = 0;
                ret(-1);
             }
         }
         if (ixxtocado(fd))
         {
            ixx_ojo_recursion = 0;
            ixx_desbloquea_lectura(fd,ixxfich->openid,ixxfich->openmode);
            ixx_reconstruye(ixx_fd);
            if (ixx_bloquea_lectura(fd,ixxfich->openid,ixxfich->openmode))
            {
               ret(-1);
            }
            if (ixxtocado(fd))
            {
               eret(IXXETOCADO,0,-1);
            }
            ixx_ojo_recursion = 1;
         }
         
         
         if ((*ixxdat_lee)(ixxfich->fd_dat,ixxfich->reg_cur,Dat_Cur,ixxfich->tamreg) < 0)
         {
            sale_mal = 1;
         }
         else
         {   
            clave = &(ixxfich->claves[laclave]);
            (*ixxdat_clave)(ixxfich,Dat_Cur,Datos,(short)laclave,ixxfich->reg_cur,0,0,0);
            (*ixxdat_clave)(ixxfich,dat_reg,Datos2,(short)laclave,ixxfich->reg_cur,0,0,0);
            if (!ixxcompara_clave(Datos[laclave],Datos2[laclave],clave,0))
            {
               salida = 1;
            }
         }
         
         if (!(ixxfich->flags_operacion & 1))
         {
             if (ixx_desbloquea_lectura(fd,ixxfich->openid,ixxfich->openmode))
             {
                sale_mal = 1;
             }
         }
         
         if (sale_mal)
         {
#ifdef CONTROLNLM
            pprint(2501,"*LEEMAL*");
            sleep(1);
            pprint(2501,"        ");
#endif
            ixx_ojo_recursion = 0;
            isrecnum = 0L;
            ret(-1);
         }
         
         isrecnum = ixxfich->reg_cur;
         ixx_ojo_recursion = 0;
         bret(salida);
}

int ixx_lee_indice(ixx_fd,dat_reg,modo,queclave,leer,parte,flags)
int ixx_fd;
Uchar *dat_reg; /* si es NULL solo posiciona el indexado */
int modo;
int queclave;
int leer;
int parte;
int flags;
{
   int fd;
   int mode,omode;
   long rwupdate;
   short laclave;
   long pos,blo,hijo,padre;
   short tamblock;
   short level,nkeys;
   short len;
   short posi,levelcon,posiori,posiavance;
   int t,sale_mal = 0,ojo = 0,bloqueo;
   Uchar *data;
   int con_error = 0;
   int si_clave_dat = 0,cambio_clave = 0,repitio = 0,salta_contenido = 0;
   int cuantos_saltos = 0;
   IXXCLAVE *clave;
   long old_rec_cur;
   short local_Pos[IXXMAXLEVEL];
   int old_queclave = queclave;
   
   AjustaPunterosSegunDatos(ixx_fd);
   
   if (ixx_ojo_recursion)
   {
      eret(IXXERECURS,0,-1);
   }
   ixx_ojo_recursion = 1;
   
   ixx_e_fich = -1;
   if (ixx_fd < 0 || ixx_fd >= IXXMAXFILES)
   {
      ixx_ojo_recursion = 0;
      eret(IXXEMALARGU,0,-1);
   }
   
   mode = (modo & (~IXXLOCK)) & (~IXXWAIT);
   omode = mode;
   
   if ( (modo & IXXLOCK) )
      bloqueo = 1; /* bloquear , si no se puede abandonar */
   else
      if ( (modo & IXXWAIT) )
         bloqueo = 2; /* bloquear , si no se puede esperar */
      else
         bloqueo = 0; /* sin bloqueo */
      
      
      if (mode < IXXFIRST || mode > IXXLOWEQ)
      {
         ixx_ojo_recursion = 0;
         eret(IXXEMALARGU,1,-1);
      }
      
      if (!ixxfichs[ixx_fd])
      {
         ixx_ojo_recursion = 0;
         eret(IXXENOOPEN,0,-1);
      }
      
      ixx_e_fich = ixx_fd;
      ixxfich = ixxfichs[ixx_fd];
      fd = ixxfich->fd;
      old_rec_cur = ixxfich->reg_cur;
      
#ifdef CONTROLNLM
      pprint(2501,"*LEE 01*");
      sleep(1);
#endif
      
      if (ixxfich->fd_dat < 0)
      {
         ixxfich->fd_dat = (*ixxdat_abre)(ixxfich->nombredat,NULL,NULL,NULL,NULL,NULL,&(ixxfich->dtabla));
      }
#ifdef CONTROLNLM
      pprint(2501,"        ");
      sleep(1);
#endif
      
      if (ixxfich->fd_dat < 0)
      {
         ixx_ojo_recursion = 0;
         eret(IXXENOOPEN,0,-1);
      }
      
      if (!ixxfich->tamreg)
      {
         ixx_ojo_recursion = 0;
         eret(IXXEMALFICH,0,-1);
      }
      
      if (queclave < -1 || queclave > ixxfich->nclaves)
      {
         ixx_ojo_recursion = 0;
         eret(IXXEMALARGU,1,-1);
      }
      
      if (queclave == ixxfich->nclaves && mode != IXXCURR && mode != IXXEQUAL)
      {
         ixx_ojo_recursion = 0;
         eret(IXXEMALARGU,1,-1);
      }
      
      if (queclave == -1 &&  ixxfich->clave_activa == ixxfich->nclaves && mode != IXXCURR && mode != IXXEQUAL)
      {
         ixx_ojo_recursion = 0;
         eret(IXXEMALARGU,1,-1);
      }
      
repeticion_extrana:
      /* bloquear uso de lectura */
      if (!(ixxfich->flags_operacion & 1))
      {
          if (ixx_bloquea_lectura(fd,ixxfich->openid,ixxfich->openmode))
          {
             ixx_ojo_recursion = 0;
             ret(-1);
          }
      }
      
      if (ixxtocado(fd))
      {
         ixx_ojo_recursion = 0;
         ixx_desbloquea_lectura(fd,ixxfich->openid,ixxfich->openmode);
         ixx_reconstruye(ixx_fd);
         if (ixx_bloquea_lectura(fd,ixxfich->openid,ixxfich->openmode))
         {
            ret(-1);
         }
         if (ixxtocado(fd))
         {
            eret(IXXETOCADO,0,-1);
         }
         ixx_ojo_recursion = 1;
      }
      
#ifdef CONTROLNLM
      pprint(2501,"*LEE 02*");
      printf("%d,%d<-",sizeof(long),sizeof(rwupdate));
      sleep(1);
#endif
      
      if (ixx_lseek(fd,OUPDATE,SEEK_SET)== -1L)
      {
         IXXerror(IXXELSEEK,errno,-1);
         sale_mal = 1;
         goto lee_sale;
      }
      if (ixx_lee(fd,(unsigned char *)&rwupdate,sizeof(long)) != sizeof(long))
      {
         IXXerror(IXXEREAD,errno,-1);
         sale_mal = 1;
         goto lee_sale;
      }
#ifdef CONTROLNLM
      pprint(2501,"        ");
      sleep(1);
#endif
      
      
      if (rwupdate != ixxfich->cabecera.updatecount)
      {
#ifdef CONTROLNLM
         pprint(2501,"*LEE003*");
         sleep(1);
#endif
         
         ixx_buf_anula(fd);
         
         /* actualizar datos cabecera */
         ixxfich->blo_cur = 0L;
         ixxfich->pos_cur = 0;
         if (ixx_lseek(fd,0L,SEEK_SET)== -1L)
         {
            IXXerror(IXXELSEEK,errno,-1);
            sale_mal = 1;
            goto lee_sale;
         }         

         if (ixx_lee(fd,(unsigned char *)&(ixxfich->cabecera),sizeof(IXXCABEZA)) != sizeof(IXXCABEZA))
         {
            IXXerror(IXXEREAD,errno,-1);
            sale_mal = 1;
            goto lee_sale;
         }

         if (memcmp(ixxfich->cabecera.file_ver,VERSION_INDEX,4))
         {            
            IXXerror(IXXEMALFICH,2,-1);
            sale_mal = 1;
            goto lee_sale;
         }

         if (ixxfich->tamreg != ixxfich->cabecera.tamreg)
         {
            IXXerror(IXXECORRUP,62,-1);
            sale_mal = 1;
            goto lee_sale;
         }
         if (ixxfich->tamblock != ixxfich->cabecera.tamblock)
         {
            IXXerror(IXXECORRUP,63,-1);
            sale_mal = 1;
            goto lee_sale;
         }
         if (ixxfich->nclaves != ixxfich->cabecera.nkeys)
         {
            IXXerror(IXXECORRUP,64,-1);
            sale_mal = 1;
            goto lee_sale;
         }
#ifdef CONTROLNLM
         pprint(2501,"        ");
         sleep(1);
#endif
         
      }
      
#ifdef CONTROLNLM
      pprint(2501,"*LEE 03*");
      sleep(1);
#endif
      
      /* ajustar clave corriente */
      if (queclave == -1)
      {
         if (leer && !parte)
            ixxfich->trozo_activa = 0;
         queclave = ixxfich->clave_activa;
         if (!parte)
            parte = ixxfich->trozo_activa;
      }
      else
      {
         if (queclave != ixxfich->nclaves && ixxfich->clave_activa != ixxfich->nclaves)
         {
            if (ixxfich->contenido)
            {
               free(ixxfich->contenido);
            }
            ixxfich->contenido = NULL;
            ixxfich->contenidos[0] = 0;
         }
         ixxfich->trozo_activa = parte;
         cambio_clave = 1;
      }
      
      if (queclave != ixxfich->clave_activa)
      {
         ixxfich->clave_activa = queclave;
         ixxfich->blo_cur = 0L;
         ixxfich->pos_cur = 0;
      }
      
      if (queclave == ixxfich->nclaves)
      {
		 if (!cambio_clave && (isrecnum < 1L || isrecnum > ixxfich->cabecera.nrecords))
		 {
			 laclave = ixxfich->clave_activa;
			 tamblock = ixxfich->tamblock;
			 len = 0;
		     if (leer) data = dat_reg;else data = Dat_Cur;

			 IXXerror(IXXENOKEY,0,-1);
			 sale_mal = 1;
			 goto lee_sale;
		 }

         if (mode == IXXEQUAL)
            ixxfich->reg_cur = isrecnum;
         ixxfich->blo_cur = 0L;
         ixxfich->pos_cur = 0;
         mode = IXXCURR;
         ojo = 1;
      }
      
      laclave = ixxfich->clave_activa;
      tamblock = ixxfich->tamblock;
      if (laclave < ixxfich->nclaves)
         len = ixxfich->claves[laclave].clave_len;
      else
         len = 0;
      if (leer) data = dat_reg;else data = Dat_Cur;
      
#ifdef CONTROLNLM
      pprint(2501,"*LEE 04*");
      sleep(1);
#endif
      
      
      if (mode != IXXNEXT && mode != IXXPREV && mode != IXXCURR && mode != IXXFIRST && mode != IXXLAST)
      {
         old_rec_cur = 0L;
         ixxfich->blo_cur = 0L;
         ixxfich->reg_cur = 0L;
         ixxfich->pos_cur = 0;
         pos = 0L;
         
         if (ixxfich->contenido)
         {
            free(ixxfich->contenido);
            ixxfich->contenido = NULL;
            ixxfich->contenidos[0] = 0;
         }
         
         if (!leer) si_clave_dat = 1;
         (*ixxdat_clave)(ixxfich,dat_reg,Datos,laclave,pos,0,parte,mode);
         t = ixx_busca_clave(laclave,Datos[laclave],&blo,&pos,_Posblock,1+mode);
         if (t < 0)
         {
            sale_mal = 1;
            goto lee_sale;
         }
         else
            if (t == 3)
            {
               IXXerror(IXXENOKEY,0,-1);
               sale_mal = 1;
               goto lee_sale;
            }
            else
               if (t == 2) /* es la mas pequeña */
               {
                  if (mode == IXXGTEQ || mode == IXXGREAT)
                  {
                     mode = IXXFIRST;
                  }
                  else
                  {
                     IXXerror(IXXENOKEY,0,-1);
                     sale_mal = 1;
                     goto lee_sale;
                  }
               }
               else
                  if (t == 0) /* se ha situado en la anterior */
                  {
                     if (mode == IXXLOW || mode == IXXLOWEQ)
                     {
                        /* atencion bloqueo */
                        if (bloqueo)
                        {
                           if (ixx_reg_bloquea(pos,bloqueo-1))
                           {
                              sale_mal = 1;
                              goto lee_sale;
                           }
                        }
                        if ((*ixxdat_lee)(ixxfich->fd_dat,pos,data,ixxfich->tamreg))
                        {
                           IXXerror(IXXECORRUP,65,-1);
                           sale_mal = 1;
                           goto lee_sale;
                        }
                        ixxfich->blo_cur = blo;
                        ixxfich->reg_cur = pos;
                        ixxfich->pos_cur = _Posblock[0];
                        (*ixxdat_clave)(ixxfich,data,ixxfich->DatosCurr,-1,pos,0,0,0);
                        memcpy(ixxfich->PosCurr,_Posblock,sizeof(short)*IXXMAXLEVEL);                        
                        goto lee_sale;
                     }
                     else
                        if (mode == IXXEQUAL)
                        {
                           IXXerror(IXXENOKEY,0,-1);
                           sale_mal = 1;
                           goto lee_sale;
                        }
                        mode = IXXNEXT;
                        ixxfich->blo_cur = blo;
                        ixxfich->reg_cur = pos;
                        ixxfich->pos_cur = _Posblock[0];
                        memcpy(ixxfich->PosCurr,_Posblock,sizeof(short)*IXXMAXLEVEL);
                        ixxfich->NumeroPosicion[laclave] = IxxNumeroPosicion;
                        ojo = 1;
                  }
                  else
                     if (t == 1) /* exacta */
                     {
                        if (mode == IXXEQUAL || mode == IXXGTEQ || mode == IXXLOWEQ)
                        {
                           /* atencion bloqueo */
                           if (bloqueo)
                           {
                              if (ixx_reg_bloquea(pos,bloqueo-1))
                              {
                                 sale_mal = 1;
                                 goto lee_sale;
                              }
                           }
                           if ((*ixxdat_lee)(ixxfich->fd_dat,pos,data,ixxfich->tamreg))
                           {
                              IXXerror(IXXECORRUP,66,-1);
                              sale_mal = 1;
                              goto lee_sale;
                           }
                           ixxfich->blo_cur = blo;
                           ixxfich->reg_cur = pos;
                           ixxfich->pos_cur = _Posblock[0];
                           (*ixxdat_clave)(ixxfich,data,ixxfich->DatosCurr,-1,pos,0,0,0);
                           memcpy(ixxfich->PosCurr,_Posblock,sizeof(short)*IXXMAXLEVEL);
                           goto lee_sale;
                        }
                        if (mode == IXXLOW)
                           mode = IXXPREV;
                        else
                           if (mode == IXXGREAT)
                              mode = IXXNEXT;
                           else
                           {
                              IXXerror(IXXECORRUP,67,-1);
                              sale_mal = 1;
                              goto lee_sale;
                           }
                           ixxfich->blo_cur = blo;
                           ixxfich->reg_cur = pos;
                           ixxfich->pos_cur = _Posblock[0];
                           memcpy(ixxfich->PosCurr,_Posblock,sizeof(short)*IXXMAXLEVEL);
                           ixxfich->NumeroPosicion[laclave] = IxxNumeroPosicion;
                           ojo = 1;
                     }
        }
        
        /* no debe haber else por las conversiones de mode */
        
        if (mode == IXXNEXT || mode == IXXPREV || mode == IXXCURR)
        {           
           if (!ixxfich->reg_cur)
           {
              IXXerror(IXXENOCURR,0,-1);
              sale_mal = 1;
              goto lee_sale;
           }
           /* reposicionar si es necesario */
           if (!ixxfich->blo_cur && laclave < ixxfich->nclaves)
           {
              if (ojo)
              {
                 IXXerror(IXXECORRUP,68,-1);
                 sale_mal = 1;
                 goto lee_sale;
              }
              t = ixx_busca_clave(laclave,ixxfich->DatosCurr[laclave],&blo,&pos,_Posblock,0);
              if (t < 0)
              {
                 sale_mal = 1;
                 goto lee_sale;
              }
              if (t == 1)
              {
                 if (pos != ixxfich->reg_cur)
                 {
                    IXXerror(IXXECORRUP,69,-1);
                    sale_mal = 1;
                    goto lee_sale;
                 }
                 ixxfich->blo_cur = blo;
                 ixxfich->pos_cur = _Posblock[0];
                 memcpy(ixxfich->PosCurr,_Posblock,sizeof(short)*IXXMAXLEVEL);                 
              }
              else
              {
                 if (t == 3)
                 {
                    ixxfich->blo_cur = 0L;
                    ixxfich->reg_cur = 0L;
                    ixxfich->pos_cur = 0;
                    IXXerror(IXXENOREC,0,-1);
                    sale_mal = 1;
                    goto lee_sale;
                 }
                 if (t == 2)
                 {
                    if (mode == IXXPREV) con_error = 1;
                    mode = IXXFIRST;
                    ojo = 1;
                 }
                 if (t == 0)
                 {
                    ixxfich->reg_cur = pos;
                    ixxfich->blo_cur = blo;
                    ixxfich->pos_cur = _Posblock[0];
                    memcpy(ixxfich->PosCurr,_Posblock,sizeof(short)*IXXMAXLEVEL);
                    ojo = 1;
                    if (mode == IXXCURR) mode = IXXNEXT;
                    if (mode == IXXPREV) mode = IXXCURR;
                 }
              }
           }
           else
           {
               IxxNumeroPosicion = ixxfich->NumeroPosicion[laclave];
           }
           
           if (mode != IXXFIRST && laclave < ixxfich->nclaves && ixxfich->PosCurr[0] != ixxfich->pos_cur)
           {
              IXXerror(IXXECORRUP,70,-1);
              sale_mal = 1;
              goto lee_sale;
           }
           
caso_especial_no_estructurado:
           
           hijo = -1L;
           padre = -1L;           
           
           if (mode == IXXNEXT || mode == IXXPREV)
           {             

              if (!leer) si_clave_dat = 1;
              
              clave = &(ixxfich->claves[laclave]);
              
              pos = ixxfich->blo_cur;
              levelcon = 0;
              posiori = ixxfich->pos_cur;
              memcpy(local_Pos,ixxfich->PosCurr,sizeof(short)*IXXMAXLEVEL);
              
              posiavance = 1;
              do {
                 if (!repitio)
                 {
                    if (pos <= 0L || pos >= ixxfich->cabecera.nblocks)
                    {
                       IXXerror(IXXECORRUP,71,-1);
                       sale_mal = 1;
                       goto lee_sale;
                    }
                    blo = pos;
                    pos *= tamblock;
                    if (ixx_lseek(fd,pos,SEEK_SET)== -1L)
                    {
                       IXXerror(IXXELSEEK,errno,-1);
                       sale_mal = 1;
                       goto lee_sale;
                    }
                    if (ixx_lee(fd,ixxbloque,ixxfich->tamblock) != ixxfich->tamblock)
                    {
                       IXXerror(IXXEREAD,errno,-1);
                       sale_mal = 1;
                       goto lee_sale;
                    }
                    if (*((short *)ixxbloque) != IXXIDBCLAVE)
                    {
                       IXXerror(IXXECORRUP,72,-1);
                       sale_mal = 1;
                       goto lee_sale;
                    }
                    if (*((short *)(ixxbloque+2)) != laclave)
                    {
                       IXXerror(IXXECORRUP,73,-1);
                       sale_mal = 1;
                       goto lee_sale;
                    }
                 }
                 else
                    repitio = 0;
                 level = *((short *)(ixxbloque+4));
                 if (level != levelcon)
                 {
                    IXXerror(IXXECORRUP,74,-1);
                    sale_mal = 1;
                    goto lee_sale;
                 }
                 nkeys = *((short *)(ixxbloque+18));
                 if (posiori == 9999)
                 {
                    posiori = (20+(nkeys-1)*(len+LCLAVE_POINTERS));
                 }
                 if ( nkeys < 1 || posiori < 20 || posiori > (20+(nkeys-1)*(len+LCLAVE_POINTERS)) )
                 {
                    IXXerror(IXXECORRUP,75,-1);
                    sale_mal = 1;
                    goto lee_sale;
                 }
                 
                 if (posiavance &&
                    (
                    (posiori == (20+(nkeys-1)*(len+LCLAVE_POINTERS)) && mode == IXXNEXT) ||
                    (posiori == 20 && mode == IXXPREV)
                    )
                    )
                 {
                    /* comprobar que el bloque este bien relacionado cuando se esta subiendo */
                    if (hijo != -1L)
                    {
                       if (hijo != *((long *)(ixxbloque+posiori+len)))
                       {
                          IXXerror(IXXECORRUP,102,-1);
                          sale_mal = 1;
                          goto lee_sale;
                       }
                       if (memcmp(ixxbloque2+20,ixxbloque+posiori,len))
                       {
                          IXXerror(IXXECORRUP,101,-1);
                          sale_mal = 1;
                          goto lee_sale;
                       }
                    }
                    memcpy(ixxbloque2+20,ixxbloque+20,len);
                    hijo = blo;
                    
                    pos = *((long *)(ixxbloque+6)); /* el padre */
                    if (!pos)
                    {
                       if (mode == IXXPREV)
                       {
                          IXXerror(IXXEPRIME,1,-1);
                       }
                       else
                       {
                          IXXerror(IXXEFINAL,1,-1);
                       }
                       if (ojo)
                       {
                          t = (*ixxdat_lee)(ixxfich->fd_dat,ixxfich->reg_cur,Dat_Cur,ixxfich->tamreg);
                          if (t)
                          {
                             if (t > 0) IXXerror(IXXECORRUP,76,-1);
                             sale_mal = 1;
                             goto lee_sale;
                          }
                          (*ixxdat_clave)(ixxfich,Dat_Cur,ixxfich->DatosCurr,-1,ixxfich->reg_cur,0,0,0);
                       }
                       sale_mal = 1;
                       goto lee_sale;
                    }
                    levelcon++;                    
                    posiori = local_Pos[levelcon];
                 }
                 else
                 {
                    if (posiavance)
                    {
                       if (mode == IXXNEXT)
                       {
                          memcpy(ixxbloque3+20,ixxbloque+posiori,len);
                          posiori += (len+LCLAVE_POINTERS);
                          if (salta_contenido && ixxfich->contenido)
                          { /* saltar duplicados */
                              while(posiori < (20+(nkeys-1)*(len+LCLAVE_POINTERS)) && !ixxcompara_clave(ixxbloque3+20,ixxbloque+posiori,clave,1))
                              {
                                  posiori += (len+LCLAVE_POINTERS);
                              }
                          }
                          salta_contenido = 0;
                          if (ixxcompara_clave(ixxbloque3+20,ixxbloque+posiori,clave,0) != 1)
                          {
                             IXXerror(IXXECORRUP,105,-1);
                             sale_mal = 1;
                             goto lee_sale;
                          }
                          if (!level)
                              IxxNumeroPosicion++;
                       }
                       else
                       {
                          memcpy(ixxbloque3+20,ixxbloque+posiori,len);
                          posiori -= (len+LCLAVE_POINTERS);
                          if (salta_contenido && ixxfich->contenido)
                          { /* saltar duplicados */
                              while(posiori > 20 && !ixxcompara_clave(ixxbloque3+20,ixxbloque+posiori,clave,1))
                              {
                                  posiori -= (len+LCLAVE_POINTERS);
                              }
                          }
                          salta_contenido = 0;
                          if (ixxcompara_clave(ixxbloque3+20,ixxbloque+posiori,clave,0) != -1)
                          {
                             IXXerror(IXXECORRUP,106,-1);
                             sale_mal = 1;
                             goto lee_sale;
                          }
                          if (!level)
                              IxxNumeroPosicion--;
                       }
                    }
                    
                    
                    local_Pos[levelcon] = posiori;
                    
                    /* comprobar que el bloque este bien relacionado cuando se esta bajando */
                    if (padre != -1L)
                    {
                       if (padre != *((long *)(ixxbloque+6)))
                       {
                          IXXerror(IXXECORRUP,104,-1);
                          sale_mal = 1;
                          goto lee_sale;
                       }
                       if (memcmp(ixxbloque2+20,ixxbloque+20,len))
                       {
                          IXXerror(IXXECORRUP,103,-1);
                          sale_mal = 1;
                          goto lee_sale;
                       }
                    }
                    
                    if (levelcon)
                    {
                       pos = *((long *)(ixxbloque+posiori+len)); /* el hijo */
                       
                       padre = blo;
                       memcpy(ixxbloque2+20,ixxbloque+posiori,len);
                       
                       levelcon--;
                       posiavance = 0;
                       if (mode == IXXNEXT)
                          posiori = 20;   /* siempre la primera */
                       else
                          posiori = 9999; /* siempre la ultima */
					   if (!levelcon)
					   {
						   if (mode == IXXNEXT)
							   IxxNumeroPosicion++;
						   else
							   IxxNumeroPosicion--;
					   }
                       continue;
                    }
                 }
                 if (!levelcon)
                 {
                    /* primero comprobar filtro antes del contenido */
                    if (!ixx_comprueba_filtro(clave,ixxbloque+posiori))
                    {
                       hijo = -1L;
                       padre = -1L;
                       posiavance = 1;
                       pos = blo;
                       pos *= tamblock;
                       repitio = 1;
                       continue;                               
                    }                           
                    
                    if (ixxfich->contenido)
                    {
                       if (ixxbusca_contenido(ixxbloque+posiori,clave))
                       {
                          if (cuantos_saltos >= IXX_MAXIMO_BUSQUEDA)
                          {
                              cuantos_saltos = 99999;
                              break;
                          }
                          hijo = -1L;
                          padre = -1L;
                          posiavance = 1;
                          pos = blo;
                          pos *= tamblock;
                          repitio = 1;
                          salta_contenido = 1;
                          cuantos_saltos++;
                          continue;
                       }
                    }
                    break;
                 }
                  } while(-1);
                  
                  
                  pos = *((long *)(ixxbloque+posiori+len));
                  ixxfich->pos_cur = posiori;
                  ixxfich->reg_cur = pos;
                  ixxfich->blo_cur = blo;
                  memcpy(ixxfich->PosCurr,local_Pos,sizeof(short)*IXXMAXLEVEL);
                  if (ixxfich->PosCurr[levelcon] != ixxfich->pos_cur)
                  {
                     IXXerror(IXXECORRUP,77,-1);
                     sale_mal = 1;
                     goto lee_sale;
                  }
                  mode = IXXCURR;
                  /* el bloqueo si es necesario lo hace en IXXCURR */
                  ojo = 1;
                }
                
                if (mode == IXXCURR)
                {
                   /* ojo hay que reposicionar en caso de bloqueado*/
                   if (bloqueo)
                   {
                      if (ixx_reg_bloquea(ixxfich->reg_cur,bloqueo-1))
                      {
                         if (ojo)
                         {
                            ixxfich->reg_cur = old_rec_cur;
                            ixxfich->blo_cur = 0L;
                            ixxfich->pos_cur = 0;
                         }
                         sale_mal = 1;
                         goto lee_sale;
                      }
                   }
                   
                   if (leer || ojo)
                   {
                      t = (*ixxdat_lee)(ixxfich->fd_dat,ixxfich->reg_cur,data,ixxfich->tamreg);
                      if (t)
                      {
                         if (t > 0)
                         {
                            IXXerror(IXXENOCURR,0,-1);
                            ixxfich->blo_cur = 0L;
                            ixxfich->reg_cur = 0L;
                            ixxfich->pos_cur = 0;
                         }
                         sale_mal = 1;
                         goto lee_sale;
                      }
                   }
                   if (ojo)
                   {
                      (*ixxdat_clave)(ixxfich,data,ixxfich->DatosCurr,-1,ixxfich->reg_cur,0,0,0);
                   }
                   goto lee_sale;
                }
        }
        
        if (mode == IXXFIRST || mode == IXXLAST)
        {
           ixxfich->blo_cur = 0L;
           ixxfich->reg_cur = 0L;
           ixxfich->pos_cur = 0;
           
           if (!leer) si_clave_dat = 1;
           
           pos = tamblock;
           pos *= (laclave+1);
           pos += 9;
           if (ixx_lseek(fd,pos,SEEK_SET)== -1L)
           {
              IXXerror(IXXELSEEK,errno,-1);
              sale_mal = 1;
              goto lee_sale;
           }
           if (ixx_lee(fd,(unsigned char *)&pos,sizeof(long)) != sizeof(long))
           {
              IXXerror(IXXEREAD,errno,-1);
              sale_mal = 1;
              goto lee_sale;
           }
           if (!pos)
           {
              if (!cambio_clave || leer || con_error)
              {
                 IXXerror(IXXENOREC,errno,-1);
                 sale_mal = 1;
              }
              con_error = 1;
              goto lee_sale;
           }
           do {
              if (pos <= 0L || pos >= ixxfich->cabecera.nblocks)
              {
                 IXXerror(IXXECORRUP,78,-1);
                 sale_mal = 1;
                 goto lee_sale;
              }
              blo = pos;
              pos *= tamblock;
              if (ixx_lseek(fd,pos,SEEK_SET)== -1L)
              {
                 IXXerror(IXXELSEEK,errno,-1);
                 sale_mal = 1;
                 goto lee_sale;
              }
              if (ixx_lee(fd,ixxbloque,ixxfich->tamblock) != ixxfich->tamblock)
              {
                 IXXerror(IXXEREAD,errno,-1);
                 sale_mal = 1;
                 goto lee_sale;
              }
              if (*((short *)ixxbloque) != IXXIDBCLAVE)
              {
                 IXXerror(IXXECORRUP,79,-1);
                 sale_mal = 1;
                 goto lee_sale;
              }
              if (*((short *)(ixxbloque+2)) != laclave)
              {
                 IXXerror(IXXECORRUP,80,-1);
                 sale_mal = 1;
                 goto lee_sale;
              }
              level = *((short *)(ixxbloque+4));
              nkeys = *((short *)(ixxbloque+18));
              if (!nkeys)
              {
                 if (!level)
                 {
                    if (!cambio_clave || leer || con_error)
                    {
                       IXXerror(IXXENOREC,errno,-1);
                       sale_mal = 1;
                    }
                    con_error = 1;
                    goto lee_sale;
                 }
                 else
                 {
                    IXXerror(IXXECORRUP,81,-1);
                    sale_mal = 1;
                    goto lee_sale;
                 }
              }
              if (mode == IXXFIRST)
              {
                 pos = *((long *)(ixxbloque+20+len)); /* la primera */
                 posi = 20;
              }
              else
              {
                 pos = *((long *)(ixxbloque+(20-LCLAVE_POINTERS) + nkeys*(len+LCLAVE_POINTERS) )); /* la ultima */
                 posi = 20 + (nkeys-1) * (len+LCLAVE_POINTERS);
              }
              _Posblock[level] = posi;
           } while(level);


           if (mode == IXXFIRST)
           {
               IxxNumeroPosicion = 0L;
           }
           else
           {
               IxxNumeroPosicion = ixxfich->cabecera.nrecords-ixxfich->cabecera.nfreerecords-1;
           }
           
           /* atencion poner bloqueo de registro o controlar el mismo */
           if (bloqueo && !con_error)
           {
              if (ixx_reg_bloquea(pos,bloqueo-1))
              {
                 if (mode != omode && ojo)
                 {
                    ixxfich->reg_cur = old_rec_cur;
                 }
                 sale_mal = 1;
                 goto lee_sale;
              }
           }
           
           if ((*ixxdat_lee)(ixxfich->fd_dat,pos,data,ixxfich->tamreg))
           {
              IXXerror(IXXECORRUP,82,-1);
              sale_mal = 1;
              goto lee_sale;
           }
           ixxfich->blo_cur = blo;
           ixxfich->reg_cur = pos;
           ixxfich->pos_cur = posi;
           (*ixxdat_clave)(ixxfich,data,ixxfich->DatosCurr,-1,pos,0,0,0);
           memcpy(ixxfich->PosCurr,_Posblock,sizeof(short)*IXXMAXLEVEL);
           if (con_error)
           {
              if (mode == IXXFIRST)
                 IXXerror(IXXEPRIME,0,-1);
              else
                 IXXerror(IXXEFINAL,0,-1);
              sale_mal = 1;
           }
           else
           {
              /* primero comprobar filtro antes del contenido */
              if (!ixx_comprueba_filtro(&(ixxfich->claves[laclave]),ixxbloque+posi))
              {
                 if (mode == IXXFIRST)
                    mode = IXXNEXT;
                 else
                    mode = IXXPREV;
                 goto caso_especial_no_estructurado;
              }                           
              
              
              if (ixxfich->contenido)
              {
                 clave = &(ixxfich->claves[laclave]);
                 if (ixxbusca_contenido(ixxbloque+posi,clave))
                 {
                      if (cuantos_saltos >= IXX_MAXIMO_BUSQUEDA)
                      {
                          cuantos_saltos = 99999;
                          goto lee_sale;
                      }

                    if (mode == IXXFIRST)
                       mode = IXXNEXT;
                    else
                       mode = IXXPREV;
                    salta_contenido = 1;
                    cuantos_saltos++;
                    goto caso_especial_no_estructurado;
                 }
              }
           }
           goto lee_sale;
        }
        
        
lee_sale:
        
        ixxfich->NumeroPosicion[laclave] = IxxNumeroPosicion;

        /* comprobacion de la integridad del bloqueo */
        if (ixx_lseek(fd,OUPDATE,SEEK_SET)== -1L)
        {
           IXXerror(IXXELSEEK,errno,-1);
           sale_mal = 1;
           goto lee_salextra;
        }
        if (ixx_lee(fd,(unsigned char *)&rwupdate,sizeof(long)) != sizeof(long))
        {
           IXXerror(IXXEREAD,errno,-1);
           sale_mal = 1;
           goto lee_salextra;
        }
        
        if (rwupdate != ixxfich->cabecera.updatecount)
        {
           IXXerror(IXXECORRUP,108,-1);
           sale_mal = 1;
           goto lee_salextra;
        }
        
lee_salextra:
        /* desbloquear uso de lectura */
        if (!(ixxfich->flags_operacion & 1))
        {
            if (ixx_desbloquea_lectura(fd,ixxfich->openid,ixxfich->openmode))
            {
               sale_mal = 1;
            }
        }
        
        if (sale_mal)
        {
#ifdef CONTROLNLM
           pprint(2501,"*LEEMAL*");
           sleep(1);
           pprint(2501,"        ");
#endif
           ixx_ojo_recursion = 0;
           isrecnum = 0L;
           ret(-1);
        }
#ifdef CONTROLNLM
        pprint(2501,"        ");
        sleep(1);
#endif
        if (cuantos_saltos == 99999 && !(flags & 1))
        {
            cuantos_saltos = 0;
            queclave = old_queclave;
            if (mode == IXXCURR)
            {
                if (omode == IXXFIRST)
                    mode = IXXNEXT;
                else
                if (omode == IXXLAST)
                    mode = IXXPREV;
                else
                    mode = omode;
            }
            goto repeticion_extrana;
        }
        isrecnum = ixxfich->reg_cur;
        if (!cambio_clave && dat_reg && si_clave_dat && !con_error && queclave < ixxfich->nclaves)
        {
           if ((*ixxdat_lee)(ixxfich->fd_dat,ixxfich->reg_cur,Dat_Cur,ixxfich->tamreg) >= 0)
           {
              (*ixxdat_clave)(ixxfich,dat_reg,&Dat_Cur,ixxfich->clave_activa,pos,2,0,0);
              /* (*ixxdat_clave)(ixxfich,Dat_Cur,ixxfich->DatosCurr,ixxfich->clave_activa,pos,0,0,0); */
           }
           /* (*ixxdat_clave)(ixxfich,dat_reg,ixxfich->DatosCurr,ixxfich->clave_activa,pos,2,0,0); */
           /* por el asunto minusculas-mayusculas */	      
        }

        ixx_ojo_recursion = 0;
        if (leer)
           _e_log_read++;

        if (cuantos_saltos == 99999)
        {
           bret(1);
        }
        else
        {
           bret(0);
        }
}

int ixx_libera(ixx_fd,reg)      
int ixx_fd;
long reg;
{
   AjustaPunterosSegunDatos(ixx_fd);
   
   if (ixx_ojo_recursion)
   {
      eret(IXXERECURS,0,-1);
   }
   ixx_ojo_recursion = 1;
   
   if (ixx_fd < 0 || ixx_fd >= IXXMAXFILES)
   {
      ixx_ojo_recursion = 0;
      eret(IXXEMALARGU,0,-1);
   }
   
   ixx_e_fich = ixx_fd;
   if (!ixxfichs[ixx_fd])
   {
      ixx_ojo_recursion = 0;
      eret(IXXENOOPEN,0,-1);
   }
   ixxfich = ixxfichs[ixx_fd];
   if (ixx_reg_desbloquea(reg))
   {
      ixx_ojo_recursion = 0;
      ret(-1);
   }
   
   if (ixxfich->openmode == IXXCOMPARTE && !ixxfich->numero_bloqueos &&
      ixxfich->control_numblocks < ixxfich->cabecera.nblocks)
   {
      if (ixx_cierra(ixxfich->fd) >= 0)
      {
         ixxfich->fd = ixx_abre_share(ixxfich->nombreixx);
         if (ixxfich->fd < 0)
         {
            IXXerror(IXXECORRUP,115,-1);
            ret(-1);
         }
      }
      ixxfich->control_numblocks = ixxfich->cabecera.nblocks;
   }
   ixx_ojo_recursion = 0;
   bret( 0 );
}


int ixx_anula_un_dat()
{
   int i;
   
   for (i = 0;i < IXXMAXFILES;i++)
			{
      if (ixxfichs[i])
      {
         ixxfich=ixxfichs[i];
         AjustaPunterosSegunDatos(i);
         if (ixxfich->fd_dat >= 0 && ixxfich->fd_dat < 20)
         {						
            (*ixxdat_cierra)(ixxfich->fd_dat);
            ixxfich->fd_dat = -1;
            return(i);
         }
      }
			}
   return(-1);
}

int ixx_repon_un_dat(i)
int i;
{
   
   if (i >= 0 && i < IXXMAXFILES && ixxfichs[i])
			{
      AjustaPunterosSegunDatos(i);
      ixxfich = ixxfichs[i];
      if (ixxfich->fd_dat < 0 && ixxfich->nombredat)
      {
         ixxfich->fd_dat = (*ixxdat_abre)(ixxfich->nombredat,NULL,NULL,NULL,NULL,NULL,&(ixxfich->dtabla));
      }
			}
   return(0);
}

int ixx_ocupacion_total(i)
int i;
{
			
			if (i >= 0 && i < IXXMAXFILES && ixxfichs[i])
         {
            AjustaPunterosSegunDatos(i);
            ixxfich = ixxfichs[i];
            ixxfich->ocupacion_total = ixx_lseek(ixxfich->fd,0L,SEEK_END) + ixx_lseek(ixxfich->fd_dat,0L,SEEK_END);
         }
         return(0);
}

int ixx_limpia_pseudos(modo)
int modo;
{
   int i;
   int orig = ixx_absoluto;
   
   ixx_absoluto = 0;
   for (i = 0;i < IXXMAXFILES;i++)
   {
      if (ixxfichs[i])
						{
         if (!modo || ixxfichs[i]->pseudo_cierra)
            ixx_cierra_indice(i);
						}
   }
   ixx_absoluto = orig;
   return(0);
}


int ixx_tam_fichero(ixx_fd,tam)
int ixx_fd;
long *tam;
{
   
   AjustaPunterosSegunDatos(ixx_fd);
   
   if (ixx_fd < 0 || ixx_fd >= IXXMAXFILES || !ixxfichs[ixx_fd])
   {
						*tam = 0L;
                  return(0);
   }
   *tam = ixxfichs[ixx_fd]->cabecera.nrecords-ixxfichs[ixx_fd]->cabecera.nfreerecords;
   return(0);
}


int ixx_inicio_lectura_multiple(int ixx_fd)
{
int fd;
long rwupdate;
int r = 0;

  if (ixx_fd < 0 || ixx_fd >= IXXMAXFILES)
  {      
      eret(IXXEMALARGU,0,-1);  
  }


  AjustaPunterosSegunDatos(ixx_fd);


  if (!ixxfichs[ixx_fd])
  {
     eret(IXXENOOPEN,0,-1);
  }

  ixx_e_fich = ixx_fd;
  ixxfich = ixxfichs[ixx_fd];
  fd = ixxfich->fd;

  if (ixx_bloquea_lectura(fd,ixxfich->openid,ixxfich->openmode))
  {     
     ret(-1);
  }
  

  if (ixxtocado(fd))
  {     
     ixx_desbloquea_lectura(fd,ixxfich->openid,ixxfich->openmode);
     ixx_reconstruye(ixx_fd);
     if (ixx_bloquea_lectura(fd,ixxfich->openid,ixxfich->openmode))
     {
        ret(-1);
     }
     if (ixxtocado(fd))
     {
        eret(IXXETOCADO,0,-1);
     }     
  }      

  if (ixx_lseek(fd,OUPDATE,SEEK_SET)== -1L)
  {
     ixx_desbloquea_lectura(fd,ixxfich->openid,ixxfich->openmode);
     eret(IXXELSEEK,errno,-1);
  }
  if (ixx_lee(fd,(unsigned char *)&rwupdate,sizeof(long)) != sizeof(long))
  {
     ixx_desbloquea_lectura(fd,ixxfich->openid,ixxfich->openmode);
     eret(IXXEREAD,errno,-1);
  }

  if (rwupdate != ixxfich->cabecera.updatecount)
  {
         ixx_buf_anula(fd);
         
         /* actualizar datos cabecera */
         ixxfich->blo_cur = 0L;
         ixxfich->pos_cur = 0;
         if (ixx_lseek(fd,0L,SEEK_SET)== -1L)
         {
            eret(IXXELSEEK,errno,-1);
         }         

         if (ixx_lee(fd,(unsigned char *)&(ixxfich->cabecera),sizeof(IXXCABEZA)) != sizeof(IXXCABEZA))
         {
            ixx_desbloquea_lectura(fd,ixxfich->openid,ixxfich->openmode);
            eret(IXXEREAD,errno,-1);
         }

         if (memcmp(ixxfich->cabecera.file_ver,VERSION_INDEX,4))
         {
            ixx_desbloquea_lectura(fd,ixxfich->openid,ixxfich->openmode);
            eret(IXXEMALFICH,2,-1);
         }

         if (ixxfich->tamreg != ixxfich->cabecera.tamreg)
         {
             ixx_desbloquea_lectura(fd,ixxfich->openid,ixxfich->openmode);
            eret(IXXECORRUP,62,-1);
         }
         if (ixxfich->tamblock != ixxfich->cabecera.tamblock)
         {
             ixx_desbloquea_lectura(fd,ixxfich->openid,ixxfich->openmode);
            eret(IXXECORRUP,63,-1);
         }
         if (ixxfich->nclaves != ixxfich->cabecera.nkeys)
         {
             ixx_desbloquea_lectura(fd,ixxfich->openid,ixxfich->openmode);
            eret(IXXECORRUP,64,-1);
         }

      r = 1;
  }

  ixxfich->flags_operacion |= 1;

  bret(r);
}



int ixx_fin_lectura_multiple(int ixx_fd)
{
int fd;

  if (ixx_fd < 0 || ixx_fd >= IXXMAXFILES)
  {      
      eret(IXXEMALARGU,0,-1);  
  }


  AjustaPunterosSegunDatos(ixx_fd);


  if (!ixxfichs[ixx_fd])
  {
     eret(IXXENOOPEN,0,-1);
  }

  ixx_e_fich = ixx_fd;
  ixxfich = ixxfichs[ixx_fd];
  fd = ixxfich->fd;

  if (ixx_desbloquea_lectura(fd,ixxfich->openid,ixxfich->openmode))
  {     
     ret(-1);
  }
  

  ixxfich->flags_operacion &= ~1;

  bret(0);
}




int ixx_lee_por_posicion(int ixx_fd,long posicion,Uchar *dat_reg,int modo,int flags)
{
int fd,laclave,tamblock;
long old_rec_cur;
long rwupdate;
long pos;
long blo;
long punto;
char *p1;
short len;
int level,nkeys,i;
int sale_mal = 0;
int bloqueo = 0;
long total_b1 = -1L;


  if (ixx_fd < 0 || ixx_fd >= IXXMAXFILES)
  {      
      eret(IXXEMALARGU,0,-1);  
  }


  AjustaPunterosSegunDatos(ixx_fd);


  if (!ixxfichs[ixx_fd])
  {
     eret(IXXENOOPEN,0,-1);
  }


  /* mode = (modo & (~IXXLOCK)) & (~IXXWAIT); */

  /* bloquear uso de lectura */
  ixx_e_fich = ixx_fd;
  ixxfich = ixxfichs[ixx_fd];
  fd = ixxfich->fd;
  old_rec_cur = ixxfich->reg_cur;

    if ( (modo & IXXLOCK) )
      bloqueo = 1; /* bloquear , si no se puede abandonar */
    else
    if ( (modo & IXXWAIT) )
         bloqueo = 2; /* bloquear , si no se puede esperar */
    else
         bloqueo = 0; /* sin bloqueo */


  if (ixxfich->fd_dat < 0)
  {
     ixxfich->fd_dat = (*ixxdat_abre)(ixxfich->nombredat,NULL,NULL,NULL,NULL,NULL,&(ixxfich->dtabla));
  }
  if (ixxfich->fd_dat < 0)
  {
     eret(IXXENOOPEN,0,-1);
  }

  if (ixxfich->clave_activa == ixxfich->nclaves || !LCLAVE_TOTAL)
  {
      eret(IXXEMALARGU,0,-1);
  }

  if (!(ixxfich->flags_operacion & 1))
  {
      if (ixx_bloquea_lectura(fd,ixxfich->openid,ixxfich->openmode))
      {     
         ret(-1);
      }
  }
  
  if (ixxtocado(fd))
  {
     ixx_ojo_recursion = 0;
     ixx_desbloquea_lectura(fd,ixxfich->openid,ixxfich->openmode);
     ixx_reconstruye(ixx_fd);
     if (ixx_bloquea_lectura(fd,ixxfich->openid,ixxfich->openmode))
     {
        ret(-1);
     }
     if (ixxtocado(fd))
     {
        eret(IXXETOCADO,0,-1);
     }     
  }      

  if (ixx_lseek(fd,OUPDATE,SEEK_SET)== -1L)
  {
     IXXerror(IXXELSEEK,errno,-1);
     sale_mal = 1;
     goto posi_sale;
  }
  if (ixx_lee(fd,(unsigned char *)&rwupdate,sizeof(long)) != sizeof(long))
  {
     IXXerror(IXXEREAD,errno,-1);
     sale_mal = 1;
     goto posi_sale;
  }



  if (rwupdate != ixxfich->cabecera.updatecount)
  {
     IXXerror(IXXECAMBIADO,errno,-1);
     sale_mal = 1;
     goto posi_sale;
  }
  
 

  if (posicion < 0L || posicion >= (ixxfich->cabecera.nrecords-ixxfich->cabecera.nfreerecords))
  {      
     IXXerror(IXXENOEN,errno,-1);
     sale_mal = 1;
     goto posi_sale;     
  }

  tamblock = ixxfich->tamblock;

  laclave = ixxfich->clave_activa;  
  len = ixxfich->claves[laclave].clave_len;

           
  pos = tamblock;
  pos *= (laclave+1);
  pos += 9;



    if (ixx_lseek(fd,pos,SEEK_SET)== -1L)
    {
      IXXerror(IXXELSEEK,errno,-1);
      sale_mal = 1;
      goto posi_sale;
    }
    if (ixx_lee(fd,(unsigned char *)&pos,sizeof(long)) != sizeof(long))
    {
      IXXerror(IXXEREAD,errno,-1);
      sale_mal = 1;
      goto posi_sale;
    }
    if (!pos)
    {      
      goto posi_sale;
    }




    punto = 0L;
    do
    {
        blo = pos;
        pos *= tamblock;
        if (ixx_lseek(fd,pos,SEEK_SET)== -1L)
        {
          IXXerror(IXXELSEEK,errno,-1);
          sale_mal = 1;
          goto posi_sale;
        }

        if (ixx_lee(fd,ixxbloque,tamblock) != tamblock)
        {
           IXXerror(IXXEREAD,errno,-1);
           sale_mal = 1;
           goto posi_sale;
        }


        if (total_b1 == -1L)
        {
          total_b1 = ixx_suma_total_bloque(ixxbloque,len);
          if (total_b1 != (ixxfich->cabecera.nrecords-ixxfich->cabecera.nfreerecords))
          {
                 eret(IXXECORRUP,144,-1);
          }
        }


          if (*((short *)ixxbloque) != IXXIDBCLAVE)
          {
             IXXerror(IXXECORRUP,145,-1);
             sale_mal = 1;
             goto posi_sale;
          }
          if (*((short *)(ixxbloque+2)) != laclave)
          {
             IXXerror(IXXECORRUP,146,-1);
             sale_mal = 1;
             goto posi_sale;
          }

        level = *((short *)(ixxbloque+4));
        nkeys = *((short *)(ixxbloque+18));

        if (!level)
        {
            if (posicion < punto || posicion >= (punto+nkeys))
            {
                 IXXerror(IXXECORRUP,147,-1);
                 sale_mal = 1;
                 goto posi_sale;
            }
            _Posblock[level] = (short)(20 + ((posicion-punto)*(len+LCLAVE_POINTERS)));

            pos = *((long *)(ixxbloque+_Posblock[level]+len));

            if (bloqueo)
            {
               if (ixx_reg_bloquea(pos,bloqueo-1))
               {
                  sale_mal = 1;
                  goto posi_sale;
               }
            }

            if ((*ixxdat_lee)(ixxfich->fd_dat,pos,dat_reg,ixxfich->tamreg))
            {
              IXXerror(IXXECORRUP,148,-1);
              sale_mal = 1;
              goto posi_sale;
            }

            ixxfich->blo_cur = blo;
            ixxfich->reg_cur = pos;
            ixxfich->pos_cur = _Posblock[level];
            (*ixxdat_clave)(ixxfich,dat_reg,ixxfich->DatosCurr,-1,pos,0,0,0);
            memcpy(ixxfich->PosCurr,_Posblock,sizeof(short)*IXXMAXLEVEL);
            break;
        }

        p1 = ixxbloque+20+len+LCLAVE_TOTAL;
        for (i = 0;i < nkeys;i++)
        {
            pos = *((long *)p1);
            if (posicion < (punto + pos))
            {
               _Posblock[level] = (short)(20 + (i*(len+LCLAVE_POINTERS)));
               pos = *((long *)(ixxbloque+_Posblock[level]+len));
               break;
            }
            punto += pos;
            p1 += (len + LCLAVE_POINTERS);
        }
        if (i >= nkeys)
        {
              IXXerror(IXXECORRUP,149,-1); /* ultimocorrup */
              sale_mal = 1;
              goto posi_sale;
        }
    } while(level);
   

          /* desbloquear uso de lectura */
posi_sale:

  if (!(ixxfich->flags_operacion & 1))
  {
    ixx_desbloquea_lectura(fd,ixxfich->openid,ixxfich->openmode);
  }
if (sale_mal)
  ret( -1 );

  isrecnum = ixxfich->reg_cur;
  ixxfich->NumeroPosicion[laclave] = posicion;

bret( 0 );
}


int ixx_a_version_3_()
{
LCLAVE_POINTERS = 5;
LCLAVE_TOTAL = 0;
strcpy(VERSION_INDEX,"0003   ");
return(0);
}


#endif
#endif
