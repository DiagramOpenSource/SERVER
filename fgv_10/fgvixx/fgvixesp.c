

#define RWIXXEXTRAMAIN
#define RWIXXMAIN

#include "fgvixx.h"

#include <time.h>
#include <sys/timeb.h>

extern int no_read_error;


double maximustremendus = 1000000000000000000000000000000000000000000000000000000.0*
                          1000000000000000000000000000000000000000000000000000000.0*
						  1000000000000000000000000000000000000000000000000000000.0*
						  1000000000000000000000000000000000000000000000000000000.0;

static char buf_borra[10000];

/* el respaldo solo funciona a nivel de dat a dbf, hasta que se estandarice el dbf */
static int *fd_respaldo = NULL;
static int max_fd_respaldo = 256;
extern int modo_respaldo; /* modo dbf */
extern char *dir_respaldo;
extern char *lista_respaldo;
static int ejecutando_respaldo = 0;
static char **mi_lista_respaldo;


extern char *rw_usuario;

extern char *rw_modulo;

extern int ixx_control_diu;

extern int no_report_error;

static unsigned long contador_global_diu = 0L;

extern char *quita_path();

#ifdef FGVUNIX
/*
static int lock_control[IXXMAXFILES*2+50] = {0,};
static int lock_control_flag = 0;

int unix_eopen(char *name,int mode)
{
    int fd;
    
    if (!lock_control_flag)
    {
        lock_control_flag = 1;
        memset(lock_control,0,sizeof(int)*IXXMAXFILES*2+5);
    }
    
    fd = open(name,mode);
    if (fd >= 0)
    {
        lseek(fd,(off_t)LCKFIZONAINI,SEEK_SET);
        if (fgv_lockf(fd,LK_NBLCK,LCKFIZONAALL) < 0)
        {
            close(fd);
            errno = EACCES;
            return(-1);
        }
        lock_control[fd] = 2;
        lseek(fd,(off_t)0L,SEEK_SET);
    }
    
    return(fd);
}
    
int unix_sopen(char *name,int mode)
{
    int fd;
    
    if (!lock_control_flag)
    {
        lock_control_flag = 1;
        memset(lock_control,0,sizeof(int)*IXXMAXFILES*2+5);
    }
    
    fd = open(name,mode);
    if (fd >= 0)
    {
        lseek(fd,(off_t)(LCKFIZONAINI+(getpid() & 0xffff)),SEEK_SET);
        
        if (fgv_lockf(fd,LK_NBLCK,1L) < 0)
        {
            close(fd);
            errno = EACCES;
            return(-1);
        }
        lseek(fd,(off_t)0L,SEEK_SET);
        lock_control[fd] = 1;
    }
    return(fd);
}
        
int unix_close(int fd)
{
    if (fd >= 0)
    {
        if (lock_control[fd] == 2)
        {
            lseek(fd,(off_t)LCKFIZONAINI,SEEK_SET);
            fgv_lockf(fd,LK_UNLCK,LCKFIZONAALL);
        }
        else
        {
            if (lock_control[fd] == 1)
            {
                lseek(fd,(off_t)LCKFIZONAINI+(getpid() & 0xffff),SEEK_SET);
                fgv_lockf(fd,LK_UNLCK,1L);
            }
        }
        lock_control[fd] = 0;
        return(close(fd));
    } 
    return(-1);
}
  
*/          
#endif
            
            

static unsigned char *dbf_buf1 = NULL;
static int dbf_buf1_len = 0;
static struct dbf_desc *dbfs[IXXMAXFILES] = {NULL,NULL,};

/* DBF */
static int no_crear_otro = 0;
static int no_crear_cabecera = 0;
                
int stchar( Uchar *from,Uchar *to,int len )
{
    register Uchar *p,*q;
    register int a;
    
    for( a = len,p = from,q = to;a && *p;a--,p++,q++ )
    { *q = *p; }
    for( ;a;a--,q++ ) *q = ' ';
    return( 0 );
}
                
int ldchar( Uchar *from,int len,Uchar *to )
{
    register Uchar *p;
    memcpy( to,from,len );
    for( p = to + len - 1; p >= to;p-- )
    { if( *p != ' ' )
    { p++; *p = 0; return( 0 ); }
    }
    *(++p) = 0;
    return( 0 );
}
    
#ifdef ESMOTOROLA
int ldint( Uchar *s )                 /* create an int from an MIF */
{
    return (int)*((short int *)s);
}
                
int stint( short int i,Uchar *p )                   /* create 2 byte MIF from an int */
{
	*((short int *)p) = i;
    return( 0 );
}
                
long ldlong( Uchar *s )                       /* create a long from an MIF */                    
{
	return (long)*((long *)s);
}
                                
int stlong( long l,Uchar *p )              /* make MIF from a long */
{
	*((long *)p) = l;
	return 0;
}

int stfloat(float fl,Uchar *p)
{
	register Uchar *q;

    q = ( Uchar *)&fl;
    *p = q[3];
    p[1] = q[2];
    p[2] = q[1];
    p[3] = q[0];
    return( 0 );
}

float ldfloat( Uchar *s )
{
    union { float r;
    Uchar p[8];
    } x;
    x.r = 0.0;
    
	x.p[0] = s[3];
	x.p[1] = s[2];
	x.p[2] = s[1];
	x.p[3] = *s;
    return x.r;
}

double lddbl( Uchar *s )
{	
	
    union { double r;
    Uchar p[8];
    } x;
    x.r = 0.0;
    
    *x.p = s[7];
    x.p[1] = s[6];
    x.p[2] = s[5];
    x.p[3] = s[4];
	x.p[4] = s[3];
	x.p[5] = s[2];
	x.p[6] = s[1];
	x.p[7] = *s;
	if (x.r > maximustremendus)
	{
		x.r = maximustremendus;
	}
    return x.r;
}

int stdbl( double f,Uchar *p )
{
	
    register Uchar *q;
    
    q = ( Uchar *)&f;
    *p = q[7];
    p[1] = q[6];
    p[2] = q[5];
    p[3] = q[4];
	p[4] = q[3];
	p[5] = q[2];
	p[6] = q[1];    
    p[7] = *q;

    return( 0 );
}

#else            
int ldint( Uchar *s )                 /* create an int from an MIF */
{
    union { short int r;
    Uchar p[2];
    } x;
    
    x.r = 0;
    x.p[1] = *s; *x.p = s[1];
    return (int)x.r;
}
                
int stint( short int i,Uchar *p )                   /* create 2 byte MIF from an int */
{
    Uchar *q;
    
    q = ( Uchar *)&i;
    *p = q[1]; p[1] = *q;
    return( 0 );
}
                
long ldlong( Uchar *s )                       /* create a long from an MIF */                    
{
    union { long r;
    Uchar p[4];
    } x;
    x.r = 0;
    
    *x.p = s[3];
    x.p[1] = s[2];
    x.p[2] = s[1];
    x.p[3] = *s;
    return x.r;
}
                                
int stlong( long l,Uchar *p )              /* make MIF from a long */
{
    register Uchar *q;
    
    q = ( Uchar *)&l;
    *p = q[3];
    p[1] = q[2];
    p[2] = q[1];
    p[3] = *q;
    return( 0 );
}

int stfloat(float fl,Uchar *p)
{
    *((float *)p) = fl;
    return( 0 );
}

float ldfloat( Uchar *p )
{
    return(*((float *)p));
}

double lddbl( Uchar *p )
{		
	if (*((double *)p) > maximustremendus)
	{
		return maximustremendus;
	}
    return(*((double *)p));
}

int stdbl( double f,Uchar *p )
{
    *((double *)p) = f;
    return( 0 );
}

#endif

short ixxdat_graba_ddx(char **descamp,char *nombre,char *nom,char *pathdef)  /* funciones para el fichero de datos .dat compatible C-Isam */
{
    int file;
    int i;
    char c;
    short ntamreg = 0;    
	char tmp[1024];
	
    file = ixx_crea_nuevo(nombre);
    if (file >= 0)
    {
        sprintf(tmp,"S [%s][%s]\n",nom,pathdef);
		ixx_graba(file,tmp,strlen(tmp));
        i = 0;
        ntamreg = 0;
        while(descamp[i])
        {
            sprintf(tmp,"%s\r\n",descamp[i]);
			ixx_graba(file,tmp,strlen(tmp));
            strcpy(nombre,descamp[i]);
            nombre[7] = 0;
            ntamreg += atoi(nombre+2);
            i++;
        }
        ixx_cierra(file);
    }
    return(ntamreg);
}
                                                    
                                                    
int ixx_dat_pasa_a_campo(Uchar *p2,Uchar *p1,int ntipos,int nlens,int tipos,int lens,int flag)
{
    char buf[40];
    int n;
    double d;
    if (ntipos == IXXCALFA)
    {
        switch(tipos)
        {
        case IXXCSHORT:
            if (flag > -1)
                sprintf(buf,"%6d",*(short *)p1);
            else
                sprintf(buf,"%6d",ldint(p1));
            n = 0;
            if (nlens < 6)
                n = 6 - nlens;
            memcpy(p2,buf+n,6-n);
            break;
        case IXXCLONG:
            if (flag > -1)
                sprintf(buf,"%12ld",*(long *)p1);
            else
                sprintf(buf,"%12ld",ldlong(p1));
            n = 0;
            if (nlens < 12)
            {
                n = 12 - nlens;
            }
            memcpy(p2,buf+n,12-n);
            break;
        case IXXCFLOAT:
            sprintf(buf,"%%%df",nlens);
            sprintf(p2,buf,ldfloat(p1));
            break;
        case IXXCDOUBLE:
            sprintf(buf,"%%%dlf",nlens);
            sprintf(p2,buf,lddbl(p1));
            break;
        default:
            if (lens > nlens)
            {
                memcpy(p2,p1,nlens);
            }
            else
            {
                memcpy(p2,p1,lens);
            }
            break;
        }
    }
    else
    {
    if (tipos == IXXCALFA)
        d = atof(p1);
    else
        if (tipos == IXXCSHORT)
        {
            if (flag > -1)
            {
                d = (double)*((short *)p1);
            }
            else                                                
            {
                d = (double)ldint(p1);
            }
        }
        else
        if (tipos == IXXCLONG)
        {
            if (flag > -1)
            {
                d = (double)*((long *)p1);
            }
            else
            {
                d = (double)ldlong(p1);
            }
        }
        else
        if (tipos == IXXCFLOAT)
            d = (double)ldfloat(p1);
        else
            d = lddbl(p1);
                
        switch(ntipos)
        {
        case IXXCSHORT:
            stint((short)d,p2);
            break;
        case IXXCLONG:
            stlong((long)d,p2);
            break;
        case IXXCFLOAT:
            stfloat((float)d,p2);
            break;
        case IXXCDOUBLE:
            stdbl(d,p2);
            break;
        default:
            if (ixxmen_aviso)
            {
                (*ixxmen_aviso)(__Texto[159]);
            }
            break;
        }
    }
return(0);
}


extern int con_btrieve;

static void free_dbf(int i)
{
    int j;
    if (!dbfs[i]) return;
    if (dbfs[i]->campos) free(dbfs[i]->campos);
    free(dbfs[i]);
    dbfs[i] = NULL;
    for (j = 0;j < IXXMAXFILES;j++)
    {
        if (dbfs[j]) break;
    }
    if (j >= IXXMAXFILES && dbf_buf1)
    {
        free(dbf_buf1);
        dbf_buf1 = NULL;
        dbf_buf1_len = 0;
    }
}
    
    
    
static int dbf_crea_cabecera(char *nom,char *nombre,char **descamp,char *defectos,short *longi,IXXFILE *ixxfic,int *ojo,FgvStdTabla *dtabla,char *pathdef)
{
    int fd,xfd;
    char tmp[512];
    struct dbf_desc dbf;
    struct dbf_campo dbc;
    char c;
    int len,dlen;
    char *tmp_buf;
    long pos,npos,reg;
    int i;
        
    fd = ixx_crea_nuevo(nombre);
    if (fd >= 0)
    {
        memset(&(dbf.cabeza),0,sizeof(dbf.cabeza));
        if (default_dbf_id  > 0)
            dbf.cabeza.id = 0x3;
        else
            dbf.cabeza.id = IXXDBFID;  
        dbf.cabeza.code_page = 0x01;
        dbf.cabeza.any_update = _any_update;
        dbf.cabeza.mes_update = _mes_update;
        dbf.cabeza.dia_update = _dia_update;
        ixx_lseek(fd,0L,SEEK_SET);
        len = 0;
        if (ixx_graba(fd,&(dbf.cabeza),sizeof(dbf.cabeza)) != sizeof(dbf.cabeza)) eret(IXXEWRITE,errno,-1);
        dbf.ncampos = dtabla->ncampos;
        /* dbf.campos = (struct dbf_campo *)malloc(sizeof(struct dbf_campo)*dbf.ncampos); */
        for (i = 0;i < dtabla->ncampos;i++)
        {		  
            memset(&dbc,0,sizeof(dbc));	  
            strcpy(dbc.nombre,dtabla->Campo[i].ident);
            if (dbf.cabeza.id == IXXDBFID)
            {
                dbc.tipo = dtabla->Campo[i].tipo;
                dbc.len = dtabla->Campo[i].len;
                dbc.decimales = dtabla->Campo[i].decimales; 
                /* falta ... datos de FgvStdTabla */
            }
            else
            {
                /* Char/Num/Logical/Date/Memo */			 		    
                switch (dtabla->Campo[i].tipo)		     
                {
                case AGIALFA:
                    dbc.tipo = 'C';
                    dbc.flags = 0x04;
                    dbc.decimales = 0;
                    dbc.len = dtabla->Campo[i].len;
                    break;
                case AGISHORT:
                /*
                if (default_dbf_id == 1)
                {
                    */
                    dbc.tipo = 'N';
                    dbc.decimales = 0;
                    dbc.len = dtabla->Campo[i].longitud;
                    /*
                    }
                    else
                    {
                    dbc.tipo = 'I';
                    dbc.decimales = 0;
                    dbc.len = 4;
                    }           
                    */
                    break;
                        case AGILONG: /* !! */
                            dbc.tipo = 'I';
                            dbc.decimales = 0;
                            dbc.len = 4;
                            break;
                        case AGIFLOAT:
                            if (default_dbf_id == 1)
                            {
                                dbc.tipo = 'N';
                                dbc.decimales = dtabla->Campo[i].decimales; 
                                dbc.len = dtabla->Campo[i].longitud;
                            }
                            else
                            {
                                dbc.tipo = 'F';
                                dbc.decimales = dtabla->Campo[i].decimales; 
                                dbc.len = dtabla->Campo[i].len; /* ? */
                            }
                            break;
                        case AGIDOUBLE:
                            if (default_dbf_id == 1)
                            {
                                dbc.tipo = 'N';
                                dbc.decimales = dtabla->Campo[i].decimales; 
                                if (dbc.decimales == 7)
                                    dbc.len = dtabla->Campo[i].longitud + 7;
                                else
                                    dbc.len = dtabla->Campo[i].longitud;
                            }
                            else
                            {
                                dbc.tipo = 'B';
                                dbc.decimales = dtabla->Campo[i].decimales; 
                                dbc.len = dtabla->Campo[i].len; /* ? */
                            }
                            break;
                        case AGIFECHA:
                            dbc.tipo = 'D';
                            dbc.decimales = 0; 
                            dbc.len = dtabla->Campo[i].len; /* siempre 8 !! */
                            break;
                        default:			     
                            dbc.tipo = 'C';
                            dbc.flags = 0x04;
                            dbc.decimales = 0;
                            dbc.len = dtabla->Campo[i].len;
                        }		      
                    }
                    dbc.data_address = len;		  
                    len += dbc.len;
                    if (ixx_graba(fd,&dbc,sizeof(dbc)) != sizeof(dbc)) 
                    {
                        /*free(dbf.campos);*/
                        eret(IXXEWRITE,errno,-1);
                    }
                    /*memcpy(dbf.campos+i,&dbc,sizeof(dbc));*/
                }
                c = 13;
                if (ixx_graba(fd,&c,1) != 1) {/*free(dbf.campos);*/eret(IXXEWRITE,errno,-1);}
                memset(tmp,0,263);
                if (ixx_graba(fd,tmp,263) != 263) {/*free(dbf.campos);*/eret(IXXEWRITE,errno,-1);}
                
                dbf.cabeza.len_header = ixx_lseek(fd,0L,SEEK_CUR);
                len++;
                dbf.cabeza.len_record = len;
                ixx_lseek(fd,0L,SEEK_SET);
                if (ixx_graba(fd,&(dbf.cabeza),sizeof(dbf.cabeza)) != sizeof(dbf.cabeza)) {/*free(dbf.campos);*/eret(IXXEWRITE,errno,-1);}
                ixx_nuevo_cierra(fd);
                if (!ixxfic) return(0);
                /* comprobar si hay un dat anterior !! */
                if (no_crear_otro)
                {
                    fd = -1;
                }
                else
                {
                    sprintf(nombre,"%s.dat",nom);
                    fd = ixx_abre_dat(nombre);
                }
                sprintf(nombre,"%s.dbf",nom);
                if (fd >= 0 || con_btrieve)
                {          
                    ixx_dat_cierra(fd);
                    no_crear_otro = 1;
                    fd = dat_abre(nom,descamp,defectos,longi,ixxfic,ojo,dtabla,pathdef);
                    no_crear_otro = 0;
                    if (fd >= 0)
                    {			              
                        dlen = dtabla->reclen;
                        dat_tamfichero(fd,0L,dlen,&pos);             
                        dlen++;
                        dbf.cabeza.records = pos;
                        
                        pos = ixx_lseek(fd,0L,SEEK_END); 
                        if (ixxdat_aviso)
                        {
                            reg = pos / dlen;
                            (*ixxdat_aviso)(0,reg,nom);
                        }
                        reg = 0L;
                        /* realizar conversion */
                        if (dlen < dbf.cabeza.len_header)
                            tmp_buf = malloc(dbf.cabeza.len_header);
                        else
                            tmp_buf = malloc(dlen+1);    
                        
                        if (dbf.cabeza.id != IXXDBFID)
                        {     
                            if (ixx_lseek(fd,0L,SEEK_SET) == -1L)
                            {
                                free(tmp_buf);
                                /*free(dbf.campos);*/
                                dat_cierra(fd);			     
                                ixx_unlink_(nombre);
                                eret(IXXELSEEK,errno,-1); 
                            }
                            no_crear_cabecera  = 1;
                            xfd = dbf_abre(nom,descamp,defectos,longi,ixxfic,ojo,dtabla,pathdef);
                            no_crear_cabecera  = 0;
                            if (xfd < 0)
                            {
                                free(tmp_buf);
                                /*free(dbf.campos);*/
                                dat_cierra(fd);
                                ixx_unlink_(nombre);
                                return(-1);
                            }
                            reg = 1L;
                            while(ixx_lee(fd,tmp_buf+1,dlen) == dlen)
                            {
                                if (dbf_graba(xfd,reg,tmp_buf+1,dlen,0) < 0)
                                {
                                    dbf_cierra(xfd);
                                    dat_cierra(fd);
                                    ixx_unlink_(nombre);
                                    free(tmp_buf);
                                    ret(-1);
                                }
                                if (!tmp_buf[dlen])
                                    dbf_borra(xfd,reg,dlen);
                                if (ixxdat_aviso && !(reg%25))
                                {				   
                                    (*ixxdat_aviso)(1,reg,nom);
                                }
                                reg++;
                            }
                            dbf_cierra(xfd);
                            dat_cierra(fd);
                            free(tmp_buf);			    
                            sprintf(tmp,"%s.dat",nom);
                            ixx_unlink_(tmp);
                        }
                        else
                        {
                            /* usando dbf interno ... aprovechamos pase anterior */
                            while(pos > 0)
                            {
                                pos -= dlen;
                                if (pos < 0) {dlen += (int)pos;pos = 0L;}
                                if (ixx_lseek(fd,pos,SEEK_SET) == -1L)
                                {
                                    free(tmp_buf);
                                    /*free(dbf.campos);*/
                                    dat_cierra(fd);			     
                                    ixx_unlink_(nombre);
                                    eret(IXXELSEEK,errno,-1); 
                                }
                                if (ixx_lee(fd,tmp_buf+1,dlen) != dlen)
                                {
                                    free(tmp_buf);
                                    /*free(dbf.campos);*/
                                    dat_cierra(fd);			     
                                    ixx_unlink_(nombre);
                                    eret(IXXEREAD,errno,-1); 
                                }
                                reg++;
                                if (tmp_buf[dlen])				
                                    tmp_buf[0] = ' ';
                                else
                                    tmp_buf[0] = '*';								
                                npos = pos + dbf.cabeza.len_header;
                                if (ixx_lseek(fd,npos,SEEK_SET) == -1L) 
                                {
                                    free(tmp_buf);
                                    /*free(dbf.campos);*/
                                    dat_cierra(fd);			     
                                    ixx_unlink_(nombre);
                                    eret(IXXELSEEK,errno,-1); 
                                }
                                if (ixx_graba(fd,tmp_buf,len) != len) /* len == dlen */
                                {
                                    free(tmp_buf);
                                    /*free(dbf.campos);*/
                                    dat_cierra(fd);			     
                                    ixx_unlink_(nombre);
                                    eret(IXXEWRITE,errno,-1); 
                                }
                                
                                if (ixxdat_aviso && !(reg%25))
                                {				   
                                    (*ixxdat_aviso)(1,reg,nom);
                                }
                            }
                            xfd = ixx_abre_dat(nombre);
                            if (xfd < 0)
                            {
                                free(tmp_buf);
                                /*free(dbf.campos);*/
                                dat_cierra(fd);
                                ixx_unlink_(nombre);
                                return(-1);
                            }                
                            if (ixx_lee(xfd,tmp_buf,dbf.cabeza.len_header) != dbf.cabeza.len_header)
                            {
                                ixx_dat_cierra(xfd);
                                free(tmp_buf);
                                /*free(dbf.campos);*/
                                dat_cierra(fd);			     
                                ixx_unlink_(nombre);
                                eret(IXXEREAD,errno,-1); 
                            }
                            ixx_dat_cierra(xfd);
                            if (ixx_lseek(fd,0L,SEEK_SET) == -1L) 
                            {				
                                free(tmp_buf);
                                /*free(dbf.campos);*/
                                dat_cierra(fd);			     
                                ixx_unlink_(nombre);
                                eret(IXXELSEEK,errno,-1); 
                            }
                            memcpy(tmp_buf,&dbf,sizeof(dbf));
                            if (ixx_graba(fd,tmp_buf,dbf.cabeza.len_header) != dbf.cabeza.len_header)
                            {				
                                free(tmp_buf);
                                /*free(dbf.campos);*/
                                dat_cierra(fd);			     
                                ixx_unlink_(nombre);
                                eret(IXXEREAD,errno,-1); 
                            }			 
                            free(tmp_buf);
                            dat_cierra(fd);
                            ixx_unlink_(nombre);
                            sprintf(tmp,"%s.dat",nom);
                            if (ixx_rename_(tmp,nombre))
                            {
                                /*free(dbf.campos);*/
                                return(-1);
                            }
                        }
                        
                        if (ixxdat_aviso)
                        {
                            (*ixxdat_aviso)(2,0L,nom);
                        }             
                        
                        sprintf(tmp,"%s.ddx",nom);
                        unlink(tmp);
          }
        }	   
        /*free(dbf.campos);*/
       }
       return(0);
}

static int cambia_a_mi_tipo(int id,int tipo,int len,int decimales)
{
   int mi_tipo = 0;
   switch (tipo)		     
   { 
   case 'C':
   case 'L':
        mi_tipo = AGIALFA;
        break;
   case 'I' :
        if (len == 2)
            mi_tipo = AGISHORT;
        else
            mi_tipo = AGILONG;
        break;
   case 'F':
        if (id == IXXDBFID)
            mi_tipo = AGIFLOAT; 
        else   
            mi_tipo = AGIDOUBLE; /* nosotros no utilizamos el float aunque se tiene presente en muchas rutinas */
        break;
   case 'B':
        mi_tipo = AGIDOUBLE;
        break;
   case 'N':
        if (decimales)
        {
            mi_tipo = AGIDOUBLE;
        }        
        else
        {
            if (len < 5)
                mi_tipo = AGISHORT;
            else
                mi_tipo = AGIDOUBLE;
        }
        break;
   case 'D':
            mi_tipo = AGIFECHA;
            break;
   default:
            mi_tipo = AGIALFA;
            break;
    }
    return(mi_tipo);
}
    
    
int ajuste_a_dbf(int ixx_fd,unsigned char *dat_reg)
{
    struct dbf_desc *dbf;
    char tmp_d[1024];
    char tmp[256];
    int i;
    
    dbf = dbfs[ixxfichs[ixx_fd]->fd_dat];
    for (i = 0;i < dbf->ncampos;i++)
    {
        if (dbf->campos[i].tipo == 'N')
        {
            switch(dbf->dbf_tabla->Campo[i].tipo)
            {
            case AGISHORT:
                sprintf(tmp,"%%%dd",dbf->campos[i].len);
                sprintf(tmp_d,tmp,ldint( dat_reg ));
                tmp_d[dbf->campos[i].len] = 0;
                stint(atoi(tmp_d),dat_reg);
                break;
            case AGIDOUBLE:
                sprintf(tmp,"%%%d.%dlf",dbf->campos[i].len,dbf->campos[i].decimales);
                sprintf(tmp_d,tmp,lddbl( dat_reg ));
                tmp_d[dbf->campos[i].len] = 0;
                stdbl(atof(tmp_d),dat_reg);			
                break;
            case AGIFLOAT:
                sprintf(tmp,"%%%d.%dlf",dbf->campos[i].len,dbf->campos[i].decimales);
                sprintf(tmp_d,tmp,ldfloat( dat_reg ));
                tmp_d[dbf->campos[i].len] = 0;
                stfloat(atof(tmp_d),dat_reg);
                break;
            default:
                break;
            }
        }
        dat_reg += dbf->dbf_tabla->Campo[i].len;
    }
    return(0);
}
        
        
static void dbf_a_buf_conversion(struct dbf_desc *dbf,int i/*campo*/,unsigned char *dbf_buf,int offset1,unsigned char *buf,int offset2)
{
    char tmp[257];
    
    switch(dbf->campos[i].tipo)
    {
    case 'F':
    case 'B':
    case 'C':
    case 'L':
        memcpy(buf+offset2,dbf_buf+offset1,dbf->campos[i].len);
        break;
    case 'D':
        if (dbf->dbf_tabla->Campo[i].len == dbf->campos[i].len)
            memcpy(buf+offset2,dbf_buf+offset1,dbf->campos[i].len);
        else
        {
            memcpy(buf+offset2+2,dbf_buf+offset1,6); /* !! a pelo !! */
            if (dbf_buf[offset1] > '5')
                memcpy(buf+offset2,"19",2);
            else
                memcpy(buf+offset2,"20",2);
        }
        break;
    case 'N':         
        memcpy(tmp,dbf_buf+offset1,dbf->campos[i].len);
        tmp[dbf->campos[i].len] = 0;
        switch(dbf->dbf_tabla->Campo[i].tipo)
        {
        case AGISHORT:                   
            stint(atoi(tmp),buf+offset2);
            break;
        case AGILONG:
            stlong(atol(tmp),buf+offset2);
            break;
        case AGIDOUBLE:
            stdbl(atof(tmp),buf+offset2);
            break;
        case AGIFLOAT:
            stfloat(atof(tmp),buf+offset2);
            break;
        }
        break;
    case 'I':
        switch(dbf->dbf_tabla->Campo[i].tipo)
        {
        case AGISHORT:                
            if (dbf->campos[i].len == 2)
            {
                stint(*((short *)(dbf_buf+offset1)),buf+offset2);                     
            }
            else
            {
                stint((int)*((long *)(dbf_buf+offset1)),buf+offset2);                      
            }
            break;
        case AGILONG:
            stlong(*((long *)(dbf_buf+offset1)),buf+offset2);                    
            break;
        case AGIDOUBLE:                
            stdbl((double)*((long *)(dbf_buf+offset1)),buf+offset2 );                  
            break;
        case AGIFLOAT:                
            stfloat((float)*((long *)(dbf_buf+offset1)),buf+offset2 );
            break;
        }
        break;              
    }
}
        
        
        
static void buf_a_dbf_conversion(struct dbf_desc *dbf,int i/* campo */,unsigned char *dbf_buf,int offset1,unsigned char *buf,int offset2)
{
    char tmp[250];
    
    switch(dbf->campos[i].tipo)
    {
    case 'F':
    case 'B':
    case 'C':
    case 'L':
        memcpy(dbf_buf+offset1,buf+offset2,dbf->campos[i].len);
        break;
    case 'D':
        if (dbf->dbf_tabla->Campo[i].len == dbf->campos[i].len)
            memcpy(dbf_buf+offset1,buf+offset2,dbf->campos[i].len);
        else
            memcpy(dbf_buf+offset1,buf+offset2+2,6); /* !! a pelo !! */
        break;
    case 'N':          
        switch(dbf->dbf_tabla->Campo[i].tipo)
        {
        case AGISHORT:
            sprintf(tmp,"%%%dd",dbf->campos[i].len);
            sprintf(dbf_buf+offset1,tmp,ldint( buf+offset2 ));
            break;
        case AGILONG:
            sprintf(tmp,"%%%dld",dbf->campos[i].len);
            sprintf(dbf_buf+offset1,tmp,ldlong( buf+offset2 ));                
            break;
        case AGIDOUBLE:
            sprintf(tmp,"%%%d.%dlf",dbf->campos[i].len,dbf->campos[i].decimales);
            sprintf(dbf_buf+offset1,tmp,lddbl( buf+offset2 ));                         
            break;
        case AGIFLOAT:
            sprintf(tmp,"%%%d.%df",dbf->campos[i].len,dbf->campos[i].decimales);
            sprintf(dbf_buf+offset1,tmp,ldfloat( buf+offset2 ));                                
            break;
        }
        break;
    case 'I':
        switch(dbf->dbf_tabla->Campo[i].tipo)
        {
        case AGISHORT:                
            if (dbf->campos[i].len == 2)
            {
                *((short *)(dbf_buf+offset1)) = (short)ldint( buf+offset2 );
            }
            else
            {
                *((long *)(dbf_buf+offset1)) = (long)ldint( buf+offset2 );
            }
            break;
        case AGILONG:
            *((long *)(dbf_buf+offset1)) = (long)ldlong( buf+offset2 );
            break;
        case AGIDOUBLE:                
            *((long *)(dbf_buf+offset1)) = (long)lddbl( buf+offset2 );                  
            break;
        case AGIFLOAT:                
            *((long *)(dbf_buf+offset1)) = (long)ldfloat( buf+offset2 );
            break;
        }
        break;              
    }
}

static int dbf_covierte_de_alfa(unsigned char *p1,struct dbf_campo *campo1,unsigned char *p2,struct dbf_campo *campo2)
{
    int len1 = campo1->len,len2 = campo2->len;
    int maxlen = max(len1,len2);
    int minlen = min(len1,len2);
    double numero;
    char tmp[1024];
    
    switch(campo2->tipo)
    {
    case AGIALFA:
    case 'C':          
    case 'L': /* !! */
        memcpy(p2,p1,minlen);
        break;
    case 'D':
    case AGIFECHA:
        if (len2 < len1)
            memcpy(p2,p1+(len1-len2),minlen);
        else
        {
            if (len2 == 8 && len1 == 6)
            {
                if (*p1 > '5')
                    memcpy(p2,"19",2);
                else
                    memcpy(p2,"20",2);
                memcpy(p2+2,p1,6); 
            }
            else
                memcpy(p2,p1,minlen);
        }
        break; 
    case 'N':
        memcpy(tmp,p1,len1);tmp[len1] = 0;
        numero = atof(tmp);
        sprintf(tmp,"%%%d.%dlf",campo2->len,campo2->decimales);
        sprintf(p2,tmp,numero);        
        break;
    case 'B':
    case AGIDOUBLE:
        memcpy(tmp,p1,len1);tmp[len1] = 0;
        *(double *)p2 = atof(tmp);        
        break;
    case 'F':
    case AGIFLOAT:
        memcpy(tmp,p1,len1);tmp[len1] = 0;
        *(float *)p2 = atof(tmp);        
        break;
    case AGISHORT:
        memcpy(tmp,p1,len1);tmp[len1] = 0;
        stint(atoi(tmp),p2);
        break;
    case AGILONG:
        memcpy(tmp,p1,len1);tmp[len1] = 0;
        stlong(atol(tmp),p2);
        break;
    case 'I':
        memcpy(tmp,p1,len1);tmp[len1] = 0;
        *(long *)p2 = atol(tmp);
        break;
    }
    return(0);
}
                
static int dbf_covierte_de_numero(unsigned char *p1,struct dbf_campo *campo1,unsigned char *p2,struct dbf_campo *campo2)
{
    int len1 = campo1->len,len2 = campo2->len;
    int maxlen = max(len1,len2);
    int minlen = min(len1,len2);
    double numero;
    char tmp[1024];
    
    
    switch(campo2->tipo)
    {
    case AGIALFA:
    case 'C':     
    case 'D':
    case 'L': /* !! */
    case AGIFECHA:
        while(len1 && *p1 == ' ') {p1++;len1--;}
        minlen = min(len1,len2);
        if (campo2->tipo == AGIFECHA || campo2->tipo == 'D')
        {
            if (len2 < len1)
                memcpy(p2,p1+(len1-len2),minlen);
            else
            {
                if (len2 == 8 && len1 == 6)
                {
                    if (*p1 > '5')
                        memcpy(p2,"19",2);
                    else
                        memcpy(p2,"20",2);
                    memcpy(p2+2,p1,6); 
                }
                else
                    memcpy(p2,p1,minlen);
            }
        }
        else
            memcpy(p2,p1,minlen);        
        break;
    case 'N':
        memcpy(tmp,p1,len1);tmp[len1] = 0;
        numero = atof(tmp);
        sprintf(tmp,"%%%d.%dlf",campo2->len,campo2->decimales);
        sprintf(p2,tmp,numero);        
        break;
    case 'B':
    case AGIDOUBLE:
        memcpy(tmp,p1,len1);tmp[len1] = 0;
        *(double *)p2 = atof(tmp);        
        break;
    case 'F':
    case AGIFLOAT:
        memcpy(tmp,p1,len1);tmp[len1] = 0;
        *(float *)p2 = atof(tmp);        
        break;
    case AGISHORT:
        memcpy(tmp,p1,len1);tmp[len1] = 0;
        stint(atoi(tmp),p2);
        break;
    case AGILONG:
        memcpy(tmp,p1,len1);tmp[len1] = 0;
        stlong(atol(tmp),p2);
        break;
    case 'I':
        memcpy(tmp,p1,len1);tmp[len1] = 0;
        *(long *)p2 = atol(tmp);
        break;
    }
    return(0);
}


static int dbf_covierte_de_short(unsigned char *p1,struct dbf_campo *campo1,unsigned char *p2,struct dbf_campo *campo2)
{
    int len1 = campo1->len,len2 = campo2->len;
    int maxlen = max(len1,len2);
    int minlen = min(len1,len2);
    short numero = *(short *)p1;
    char tmp[1024];
    char tmp2[1024];
    
    switch(campo2->tipo)
    {
    case AGIALFA:
    case 'C':     
    case 'D':
    case 'L': /* !! */
    case AGIFECHA:
        sprintf(tmp,"%%%d",campo1->len);
        sprintf(tmp2,tmp,numero);
        len1 = strlen(tmp2);
        p1 = tmp2;
        while(len1 && *p1 == ' ') {p1++;len1--;}
        minlen = min(len1,len2);
        if (campo2->tipo == AGIFECHA || campo2->tipo == 'D')
        {
            if (len2 < len1)
                memcpy(p2,p1+(len1-len2),minlen);
            else
            {
                if (len2 == 8 && len1 == 6)
                {
                    if (*p1 > '5')
                        memcpy(p2,"19",2);
                    else
                        memcpy(p2,"20",2);
                    memcpy(p2+2,p1,6); 
                }
                else
                    memcpy(p2,p1,minlen);
            }
        }
        else
            memcpy(p2,p1,minlen);        
        break;
    case 'N':
        sprintf(tmp,"%%%d",campo2->len);
        sprintf(p2,tmp,numero);        
        break;
    case 'B':
    case AGIDOUBLE:
        *(double *)p2 = (double)numero;        
        break;
    case 'F':
    case AGIFLOAT:        
        *(float *)p2 = (float)numero;        
        break;
    case AGISHORT:        
        stint(numero,p2);
        break;
    case AGILONG:        
        stlong((long)numero,p2);
        break;
    case 'I':        
        *(long *)p2 = (long)numero;
        break;
    }
    return(0);
}
                    
                    
static int dbf_covierte_de_double(unsigned char *p1,struct dbf_campo *campo1,unsigned char *p2,struct dbf_campo *campo2)
{
    int len1 = campo1->len,len2 = campo2->len;
    int maxlen = max(len1,len2);
    int minlen = min(len1,len2);
    double numero = *(double *)p1;
    char tmp[1024];
    char tmp2[1024];
    
    switch(campo2->tipo)
    {
    case AGIALFA:
    case 'C':     
    case 'D':
    case 'L': /* !! */
    case AGIFECHA:
        sprintf(tmp,"%%%d.%dlf",campo1->len,campo1->decimales);
        sprintf(tmp2,tmp,numero);
        len1 = strlen(tmp2);
        p1 = tmp2;
        while(len1 && *p1 == ' ') {p1++;len1--;}
        minlen = min(len1,len2);
        if (campo2->tipo == AGIFECHA || campo2->tipo == 'D')
        {
            if (len2 < len1)
                memcpy(p2,p1+(len1-len2),minlen);
            else
            {
                if (len2 == 8 && len1 == 6)
                {
                    if (*p1 > '5')
                        memcpy(p2,"19",2);
                    else
                        memcpy(p2,"20",2);
                    memcpy(p2+2,p1,6); 
                }
                else
                    memcpy(p2,p1,minlen);
            }
        }
        else
            memcpy(p2,p1,minlen);        
        break;
    case 'N':
        sprintf(tmp,"%%%d.%dlf",campo2->len,campo2->decimales);
        sprintf(p2,tmp,numero);        
        break;
    case 'B':
    case AGIDOUBLE:
        *(double *)p2 = numero;        
        break;
    case 'F':
    case AGIFLOAT:        
        *(float *)p2 = (float)numero;        
        break;
    case AGISHORT:        
        stint((short)numero,p2);
        break;
    case AGILONG:        
        stlong((long)numero,p2);
        break;
    case 'I':        
        *(long *)p2 = (long)numero;
        break;
    }
    return(0);
}
                        
                        
int ixx_dbf_pasa_campo(unsigned char *p1,struct dbf_campo *campo1,unsigned char *p2,struct dbf_campo *campo2)
{
    double otro;
	short sotro;
    switch(campo1->tipo)
    {
    case AGIALFA:
    case 'C':
    case AGIFECHA: /* no hay conversion de formato fecha ya que no se ha hecho hasta ahora y falta determinar problemas */
    case 'D':
    case 'L': /* !! */
        dbf_covierte_de_alfa(p1,campo1,p2,campo2);
        break;     
    case 'N':
        dbf_covierte_de_numero(p1,campo1,p2,campo2);
        break;     
    case 'B':
    case AGIDOUBLE:
        dbf_covierte_de_double(p1,campo1,p2,campo2);
        break;     
    case 'F':
    case AGIFLOAT:
        otro = (double)*((float *)p1);
        dbf_covierte_de_double((unsigned char *)&otro,campo1,p2,campo2);
        break;
    case AGISHORT:
        sotro = ldint(p1);
        dbf_covierte_de_short((unsigned char *)&sotro,campo1,p2,campo2);
        break;
    case AGILONG:
        otro = (double)ldlong(p1);
        dbf_covierte_de_double((unsigned char *)&otro,campo1,p2,campo2);
        break;
    case 'I':
        otro = (double)*((long *)p1);
        dbf_covierte_de_double((unsigned char *)&otro,campo1,p2,campo2);
        break;
    }
    return(0);
}
                            
int dbf_abre(char *nom,char **descamp,char *defectos,short *longi,IXXFILE *ixxfic,int *ojo,FgvStdTabla *dtabla,char *pathdef)
{
    int fd,xfd;
    struct dbf_cabecera dbf;
    struct dbf_campo dbc;
    char nombre[256];
    char tmp[256];
    int ncamp = 0,i;
    char *tmp_buf1;
    char *tmp_buf2;
    int len1,len2,lenf;
    long pos,npos,cuenta,malas;
    int sobran;
    int faltan;
    int difiere;
    int j,r,k,fd_tmx,n,h;
    int *asignacion = NULL;
    long reg;
    char *p1,*p2;
    char *defectos2 = NULL;
    unsigned char mi_tipo;
    unsigned char mi_tipo2;
    time_t a; 
    struct tm *lc;
    static int recursion = 0;
    
    sprintf(nombre,"%s.dbf",nom);
    fd = ixx_abre_dat(nombre);
    if (fd < 0 && errno == ENOENT && descamp)
    {
        if (no_crear_cabecera || dbf_crea_cabecera(nom,nombre,descamp,defectos,longi,ixxfic,ojo,dtabla,pathdef) < 0)
        {
            return(-1);
        }
        fd = ixx_abre_dat(nombre);  
    }
    if (fd >= 0)
    {
        ixx_lseek(fd,0L,SEEK_SET);
        if (ixx_lee(fd,&dbf,sizeof(dbf)) != sizeof(dbf) || (dbf.id != IXXDBFID && dbf.id != 0x3 && dbf.id != 0x30 /* && dbf.id != 0x83 */))
        {
            ixx_dat_cierra(fd);
            sprintf(tmp,"%s.db!",nom);
            if (ixx_rename_(nombre,tmp))
            {
                return(-1);
            }
            return(dbf_abre(nom,descamp,defectos,longi,ixxfic,ojo,dtabla,pathdef));
        }
		/* auto correccion!! */
		if (dbf.code_page != 0x01)
		{
			dbf.code_page = 0x01;
			ixx_lseek(fd,0L,SEEK_SET);
			ixx_graba(fd,&dbf,sizeof(dbf));
		}
        for (i = 0;i < IXXMAXFILES && dbfs[i];i++);
        if (i >= IXXMAXFILES)
        {
            ixx_dat_cierra(fd);
            return(-1);
        }
        dbfs[i] = (struct dbf_desc *)malloc(sizeof(struct dbf_desc));
        dbfs[i]->cabeza = dbf;
        dbfs[i]->tocado = 0;
        dbfs[i]->fd = fd;  
        dbfs[i]->ncampos = (dbf.len_header - 264 - sizeof(dbf)) / sizeof(dbc);
        dbfs[i]->campos = (struct dbf_campo *)malloc(sizeof(struct dbf_campo) * dbfs[i]->ncampos);  
        dbfs[i]->dbf_tabla = dtabla;
        
        if (dbfs[i]->ncampos < dtabla->ncampos)
            sobran = dtabla->ncampos;
        else
            sobran = dbfs[i]->ncampos;
        asignacion = (int *)malloc(sizeof(int) * (sobran+1));
        sobran = 0;
        faltan = 0;
        difiere = 0;
        
        
        for (ncamp = 0;ncamp < dbfs[i]->ncampos;ncamp++)
        {
            if (ixx_lee(fd,&dbc,sizeof(dbc)) != sizeof(dbc))
            {
                if (asignacion)
                    free(asignacion);
                free_dbf(i);
                ixx_dat_cierra(fd);
                sprintf(tmp,"%s.db!",nom);
                if (ixx_rename_(nombre,tmp))
                {
                    return(-1);
                }        
            return(dbf_abre(nom,descamp,defectos,longi,ixxfic,ojo,dtabla,pathdef));
            }
            /* comprobar campo */
            dbfs[i]->campos[ncamp] = dbc; 
    
            mi_tipo = cambia_a_mi_tipo(dbf.id,dbc.tipo,dbc.len,dbc.decimales);	 
    
            if (dbf.id != IXXDBFID && mi_tipo == AGIDOUBLE && dtabla->Campo[ncamp].tipo == AGISHORT)
            {
                mi_tipo = AGISHORT; /* no puedo hacer otra cosa !!! */
            } 
    
            if (!sobran)
            {		
                if (ncamp >= dtabla->ncampos) 
                {
                    sobran = ncamp + 1;
                    asignacion[ncamp] = -1;
                }
                else
                {		 
                    if (strcmp(dbc.nombre,dtabla->Campo[ncamp].ident))
                    {
                        asignacion[ncamp] = -3;
                        difiere = 1;
                    }
                    else
                        /* a revisar a efecto del tema de longitud para numericos! */
                        if ( mi_tipo != dtabla->Campo[ncamp].tipo || (dbf.id == IXXDBFID && dbc.len != dtabla->Campo[ncamp].len) ||
                            (dbf.id != IXXDBFID && mi_tipo == AGIALFA && dbc.len != dtabla->Campo[ncamp].len) ||
                            (dbc.tipo == 'N' && dbc.len < dtabla->Campo[ncamp].longitud) )
                        {
                            asignacion[ncamp] = ncamp;
                            difiere = 1;
                        }
                        else
                            asignacion[ncamp] = ncamp;		 
                }
            }	 
        }
        if (sobran)
        {
            sobran = ncamp - sobran + 1;
        }

        /* recoger fecha */
        a = time(NULL);
        if (a)
        {
            lc = localtime(&a);
            if (lc)
            {
                _any_update  = lc->tm_year;
                _mes_update  = (lc->tm_mon)+1;
                _dia_update  = lc->tm_mday;
            }
        }


        while (!sobran && ncamp < dtabla->ncampos)
        {
            asignacion[ncamp] = -1;
            faltan++;
            ncamp++;
        }
        /* verificacion de cambio de estructura!! */
        if (difiere)
        {
            for (j = 0;j < ncamp;j++)
            {
                if (asignacion[j] == -3)
                {
                    for (k = 0;k < dtabla->ncampos;k++)
                    {
                        if (!strcmp(dtabla->Campo[k].ident,dbfs[i]->campos[j].nombre))
                        {
                            asignacion[j] = k;
                            break;
                        }
                    }
                }
            }
        }

        /* *** */
        if (!dbf_buf1)
        {
            dbf_buf1_len = dbf.len_record+1;
            dbf_buf1 = malloc(dbf.len_record+100); /* el extra es para evitar algunos problemas con el sprintf */
            /* al convertir comas flotantes anomalas a string */
        }
        else
        if (dbf_buf1_len < (dbf.len_record+1))
        {      
            dbf_buf1 = realloc((void *)dbf_buf1,dbf.len_record+100);     
            dbf_buf1_len = dbf.len_record+1;
        }
        /* *** */
    
    
        if (difiere || faltan || sobran)
        {
            /* cambio de version */
            if (faltan && sobran)
            {
                free_dbf(i);
                if (asignacion)
                    free(asignacion);
                eret(IXXECORRUP,136,-1); 
            }
            ixx_dat_cierra(fd);     
            if (recursion)
            {
                free_dbf(i);
                if (asignacion)
                    free(asignacion);
                eret(IXXECORRUP,135,-1); 
            }

			if (ixx_lck_lseek(ixxfic->fd,LCKREZONAINI,SEEK_SET) == -1L)
			{
				eret(IXXELSEEK,errno,-1);
			}
			if (ixx_lock(ixxfic->fd,LK_NBLCK,1L) < 0)
			{
                if ((errno == EACCES || errno == EAGAIN || errno == 13 || errno == 35))
                {
					char c;
					dbf_cierra(i);
				    while(-1)
					{
					  if (ixx_lseek(ixxfic->fd,LCKREZONAINI,SEEK_SET) == -1L)
					  {
						 eret(IXXELSEEK,errno,-1);
					  }
					  if (ixx_lee(ixxfic->fd,&c,1) == 1) break;
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
					sleep(5);
					return(dbf_abre(nom,descamp,defectos,longi,ixxfic,ojo,dtabla,pathdef));
				}
				else
					ret( -1 );
			}

            sprintf(tmp,"%s.tdy",nom);
            ixx_rename_(nombre,tmp);
            fd = ixx_abre_dat(tmp); 	 
            dbfs[i]->fd = fd;
            if (fd < 0)
            {
                free_dbf(i);
                if (asignacion)
                    free(asignacion);
                eret(IXXECORRUP,134,-1); 		
            }
            else
            {
                if (dbf_crea_cabecera(nom,nombre,descamp,defectos,longi,ixxfic,ojo,dtabla,pathdef) >= 0)
                {
                    recursion = 1;
                    fd_tmx = dbf_abre(nom,descamp,defectos,longi,ixxfic,ojo,dtabla,pathdef);
                    recursion = 0;
                    if (fd_tmx < 0)
                    {
                        free_dbf(i);
                        if (asignacion)
                            free(asignacion);
                        return(-1);
                    }
                }
                else
                {
                    free_dbf(i);
                    if (asignacion)
                        free(asignacion);
                    return(-1);
                }
                /**************************************/				
                no_read_error++;		
                len1 = dbfs[i]->cabeza.len_record - 1;
                len2 = dbfs[fd_tmx]->cabeza.len_record - 1;				
                tmp_buf1 = malloc(len1+1);
                tmp_buf2 = malloc(len2+1);		
                if (!difiere)
                {
                    lenf =  0;
                    for (h = 0;h < (ncamp-sobran);h++)
                    {
                        lenf += dbfs[i]->campos[h].len;
                    }
                }
                reg = (ixx_lseek(dbfs[i]->fd,0L,SEEK_END) - dbfs[i]->cabeza.len_header) / dbfs[i]->cabeza.len_record;
                if (ixxdat_aviso)
                {
                    (*ixxdat_aviso)(0,reg,nom);
                }
                reg = 1L;
                malas = 0L;
            
            
                if (dbfs[fd_tmx]->cabeza.id == IXXDBFID)
                {
                    defectos2 = defectos;
                }
                else
                {
                    defectos2 = malloc(len2+1);
                    p1 = defectos;
                    p2 = defectos2;
                    for (j = 0;j < ncamp;j++)
                    {
                        buf_a_dbf_conversion(dbfs[fd_tmx],j,p2,0,p1,0);              
                        p1 += dtabla->Campo[j].len;
                        p2 += dbfs[fd_tmx]->campos[j].len;
                    }
                }
            
                while ((r = dbf_lee(i,reg,tmp_buf1,0 /* forzar lectura directa */)) >= 0)
                {
                    memcpy(tmp_buf2,defectos2,len2);
                    if (difiere)
                    {
                        p1 = tmp_buf1;
                        for (h = 0;h < (ncamp-sobran);h++)
                        {
                            k = asignacion[h];
                            if (k >= 0)
                            {
                                n = 0;
                                for(j = 0;j < k;j++) n += dbfs[fd_tmx]->campos[j].len; /* prescindimos de los offset precalculados */
                                p2 = tmp_buf2 + n;
                                if (dbfs[i]->campos[h].tipo == dbfs[fd_tmx]->campos[k].tipo && dbfs[i]->campos[h].len == dbfs[fd_tmx]->campos[k].len)
                                {
                                    memcpy(p2,p1,dbfs[fd_tmx]->campos[k].len);
                                }
                                else
                                {
                                    if (dbfs[i]->campos[h].tipo == dbfs[fd_tmx]->campos[k].tipo && (dbfs[i]->campos[h].tipo == 'C' || dbfs[i]->campos[h].tipo == AGIALFA))
                                    {
                                        if (dbfs[i]->campos[h].len < dbfs[fd_tmx]->campos[k].len)
                                            n = dbfs[i]->campos[h].len;
                                        else
                                            n = dbfs[fd_tmx]->campos[k].len;
                                        memcpy(p2,p1,n);
                                    }
                                    else
                                    {
                                        ixx_dbf_pasa_campo(p1,&(dbfs[i]->campos[h]),p2,&(dbfs[fd_tmx]->campos[k]));
                                        /*
                                        mi_tipo = cambia_a_mi_tipo(dbfs[fd_tmx]->campos[k].tipo,dbfs[fd_tmx]->campos[k].len);
                                        mi_tipo2 = cambia_a_mi_tipo(dbfs[i]->campos[h].tipo,dbfs[i]->campos[h].len);
                                        ixx_dat_pasa_a_campo(p2,p1,mi_tipo-48,dbfs[fd_tmx]->campos[k].len,mi_tipo2-48,dbfs[i]->campos[h].len,-1);
                                        */
                                    }
                                }
                            }
                            p1 += dbfs[i]->campos[h].len;
                        }
                    }
                    else
                    {
                        memcpy(tmp_buf2,tmp_buf1,lenf);
                    }
                
                    if (dbf_graba(fd_tmx,reg,tmp_buf2,0,0) < 0)
                    {
                        malas++;
                    }
                    if (r)
                        dbf_borra(fd_tmx,reg,len2);
                    reg++;
                    if (!(reg%54) && ixxdat_aviso)
                    {
                        (*ixxdat_aviso)(1,reg,nom);
                    }                                                
                }

                if (defectos2 != defectos)
				{
					if (defectos2)
						free(defectos2);
					defectos2 = NULL;
				}
                no_read_error--;
                dbf_cierra(i);
                if (!malas)
                {
                    sprintf(tmp,"%s.tdy",nom);
                    ixx_unlink_(tmp);
                }
                free(tmp_buf1);
                free(tmp_buf2);
                i = fd_tmx;
                dbf = dbfs[fd_tmx]->cabeza;
                if (ixxdat_aviso)
                {
                    (*ixxdat_aviso)(2,reg,nom);
                }
                if (malas)
                {
                    sprintf(tmp,"%s.dbf",nom);
                    ixx_unlink_(tmp);
                    sprintf(tmp,"%s.tdy",nom);
                    ixx_rename_(tmp,nombre);
                    dbf_cierra(i);
                    if (asignacion)
                        free(asignacion);
                    eret(IXXECORRUP,137,-1);
                }
            }
			ixx_lck_lseek(ixxfic->fd,LCKREZONAINI,SEEK_SET);
			ixx_lock(ixxfic->fd,LK_UNLCK,1L);
        }
    
        if (asignacion)
            free(asignacion);
    
        if (longi)
        {
            if (dbf.id == IXXDBFID)
                *longi = dbf.len_record-1;
            else
                *longi = dtabla->reclen;
        }
        fd = i;
    }  
    return(fd);
}


int dbf_lee(int fd,long reg,Uchar *buf,int len)
{
    char c;
    int i;
    long pos;
    struct dbf_desc *dbf;
    unsigned char *local_buf;
    int local_len;
    int offset1;
    int offset2;
    
    if (!dbfs[fd])
    {
        eret(IXXECORRUP,120,-1); 
    }
    dbf = dbfs[fd];
    fd = dbf->fd;
    /*
    if (reg <= 0L || reg > ixxfich->cabecera.nrecords)
    {
    eret(IXXECORRUP,121,-1);
    }
    */
    /*
    if (no_read_error)
    {
    dbf->cabeza.records = reg;
    if (ixx_lseek(fd,0L,SEEK_SET) == -1L) eret(IXXELSEEK,errno,-1); 
    if (ixx_graba(fd,&(dbf->cabeza),sizeof(dbf->cabeza)) != sizeof(dbf->cabeza)) eret(IXXEWRITE,errno,-1);
    }
    else
    if (reg <= 0L || reg > dbf->cabeza.records)
    {
    eret(IXXECORRUP,122,-1);
    }
    */

    if (len && dbf->cabeza.id == IXXDBFID && len != (dbf->cabeza.len_record-1))
    {
        eret(IXXECORRUP,123,-1); 
    }

    if (dbf->cabeza.id != IXXDBFID && len)
    {
        local_buf = dbf_buf1;
        local_len = dbf->cabeza.len_record-1;
    }
    else
    {
        local_buf = buf;
        if (len)
            local_len = len;
	    else
            local_len = dbf->cabeza.len_record-1;  
    }


    pos = ((reg - 1L) * (local_len+1))  + dbf->cabeza.len_header;
    if (ixx_lseek(fd,pos,SEEK_SET) == -1L) eret(IXXELSEEK,errno,-1);
    if (ixx_lee(fd,&c,1) != 1) eret(IXXEREAD,errno,-1);
    if (c != '*')
    {
        if (ixx_lee(fd,local_buf,local_len) != local_len) eret(IXXEREAD,errno,-1);
        if (dbf->cabeza.id != IXXDBFID && len)
        {
            /* conversion */
            offset1 = 0;
            offset2 = 0;
            for (i = 0;i < dbf->ncampos;i++)
            {
                dbf_a_buf_conversion(dbf,i,dbf_buf1,offset1,buf,offset2);
                offset1 += dbf->campos[i].len;
                offset2 += dbf->dbf_tabla->Campo[i].len;
            }
        }
        ret( 0 );
    }
    ret(1);
}
    
    
    
int dbf_graba(int fd,long reg,Uchar *buf,int len,int flag)
{
    long pos;
    struct dbf_desc *dbf;
    char c = ' ';
    unsigned int offset1;
    unsigned int offset2;
    unsigned char *local_buf;
    int local_len;
    int e,i;
    

	
    if (!dbfs[fd])
    {
        eret(IXXECORRUP,124,-1);
    }
    dbf = dbfs[fd];
    fd = dbf->fd;
    
    
    
    if (len && dbf->cabeza.id == IXXDBFID && len != (dbf->cabeza.len_record-1))
    {
        eret(IXXECORRUP,127,-1);
    }
    
    /*
    if (reg <= 0L || reg > ixxfich->cabecera.nrecords)
    {
    eret(IXXECORRUP,125,-1);
    }
    */
    /*
    if (reg <= 0L || reg > (dbf->cabeza.records+1))
    {
    eret(IXXECORRUP,126,-1);
    }
*/
/*
#ifdef BASURA
case IXXCALFA:
dbc.tipo = 'C';
dbc.flags = 0x04;
dbc.decimales = 0;
break;
case IXXCSHORT :
dbc.tipo = 'I';
dbc.decimales = 0;
break;
case IXXCLONG :
dbc.tipo = 'I';
dbc.decimales = 0;
break;
case IXXCFLOAT :
dbc.tipo = 'F';
dbc.decimales = 0; // de prueba 
break;
case IXXCDOUBLE :
dbc.tipo = 'B';
dbc.decimales = 0;
break;
#endif
*/
    if (dbf->cabeza.id != IXXDBFID && len)
    {
        local_buf = dbf_buf1;
        local_len = dbf->cabeza.len_record-1;
        /* conversion */
        offset1 = 0;
        offset2 = 0;
        for (i = 0;i < dbf->ncampos;i++)
        {
            buf_a_dbf_conversion(dbf,i,dbf_buf1,offset1,buf,offset2);
            offset1 += dbf->campos[i].len;
            offset2 += dbf->dbf_tabla->Campo[i].len;
        }
    }
    else
    {
        local_buf = buf;
        if (len)
            local_len = len;
        else
            local_len = dbf->cabeza.len_record-1;  
    }
                    
    pos = ((reg - 1L) * (local_len+1))  + dbf->cabeza.len_header;
    
    if (ixx_lseek(fd,pos,SEEK_SET) == -1L) eret(IXXELSEEK,errno,-1);
    if (ixx_graba(fd,&c,1) != 1) eret(IXXEWRITE,errno,-1);
    
    
    if (ixx_graba(fd,local_buf,local_len) != local_len)
        e = 1;
    else
    {
        e = 0;
        dbf->tocado = 1;
        dbf->cabeza.any_update = _any_update;
        dbf->cabeza.mes_update = _mes_update;
        dbf->cabeza.dia_update = _dia_update;
        if (reg > dbf->cabeza.records) 
        {
            dbf->cabeza.records = reg;     
            if (dbf->cabeza.id != IXXDBFID && len)
            {
                if (ixx_lseek(fd,0L,SEEK_SET) == -1L) eret(IXXELSEEK,errno,-1); 
                if (ixx_graba(fd,&(dbf->cabeza),sizeof(dbf->cabeza)) != sizeof(dbf->cabeza)) eret(IXXEWRITE,errno,-1);
            }
        }
    }
                    
    if (e) eret(IXXEWRITE,errno,-1);
    ret(0);
}

int dbf_borra(fd,reg,len)
int fd;
long reg;
int len;
{
    long pos;
    struct dbf_desc *dbf;
    char c = '*';
    int local_len;
	int tlen,glen;
    
    if (!dbfs[fd])
    {
        eret(IXXECORRUP,128,-1);
    }
    dbf = dbfs[fd];
    fd = dbf->fd;
    
    if (dbf->cabeza.id == IXXDBFID && len != (dbf->cabeza.len_record-1))
    {
        eret(IXXECORRUP,131,-1); 
    }
    /*
    if (reg <= 0L || reg > ixxfich->cabecera.nrecords)
    {
    eret(IXXECORRUP,129,-1);
    }
    */
    /*
    if (reg <= 0L || reg > dbf->cabeza.records)
    {
    eret(IXXECORRUP,130,-1);
    }
*/

if (dbf->cabeza.id != IXXDBFID || !len)
{
    local_len = dbf->cabeza.len_record-1;
}
else
{
    local_len = len;      
}

pos = ((reg - 1L) * (local_len+1))  + dbf->cabeza.len_header;

if (ixx_lseek(fd,pos,SEEK_SET) == -1L) eret(IXXELSEEK,errno,-1);
if (ixx_graba(fd,&c,1) != 1) eret(IXXEWRITE,errno,-1);

glen = local_len;
while(glen > 0)
{
	if (glen > 10000)
	{
		tlen = 10000;
	}
	else
	{
		tlen = glen;
	}
	memset(buf_borra,0,tlen);		
	if (ixx_graba(fd,buf_borra,tlen) != tlen) eret(IXXEWRITE,errno,-1);
	glen -= tlen;
}

dbf->tocado = 1;
dbf->cabeza.any_update = _any_update;
dbf->cabeza.mes_update = _mes_update;
dbf->cabeza.dia_update = _dia_update;

ret(0);
}
    
    
int dbf_cierra(int fd)
{
    int i;
    struct dbf_desc *dbf;
    if (!dbfs[fd])
    {
        eret(IXXECORRUP,132,-1); 
    }
    dbf = dbfs[fd];
    i = dbf->fd;
    
    if (dbf->tocado)
    {
        ixx_lseek(i,0L,SEEK_SET); 
        ixx_graba(i,&(dbf->cabeza),sizeof(dbf->cabeza));   
    }
    
    free_dbf(fd);
    ret(ixx_dat_cierra(i));
}
        
        
        
int dbf_tamfichero(int fd,long total,int tamreg,long *reales)
{
    long pos;
    struct dbf_desc *dbf;
    if (!dbfs[fd])
    {
        eret(IXXECORRUP,133,-1);
    }
    dbf = dbfs[fd];
    fd = dbf->fd;
    
    pos = ixx_lseek(fd,0L,SEEK_END);
    if (pos == -1L) ret(-1);
    pos -= dbf->cabeza.len_header;
    pos /= (dbf->cabeza.len_record);
    
    if (reales)
    {
        *reales = pos;
    }
    
    if (total < pos)
        ret(1);
    if (total > pos)
        ret(-1);
    
    ret(0);
}

static char ddx_buffer[2200];
static int nddx_buffer = 0;
static char *pddx_buffer;
static char *lectura_ddx(char *res,int maxl,int fd_file)
{	
	char *_res = res;
	int leidos = 0;
	if (!nddx_buffer)
	{
		nddx_buffer = ixx_lee(fd_file,ddx_buffer,2048);
		pddx_buffer = ddx_buffer;
	}
	while(nddx_buffer > 0 && leidos < maxl)
	{
		if (*pddx_buffer == 4 || *pddx_buffer == 26)
		{
			nddx_buffer = 0;
			pddx_buffer = ddx_buffer;
			break;
		}
		*res = *pddx_buffer;
		leidos++;		
		if (*pddx_buffer == 10)
		{
			res++;
			pddx_buffer++;
			nddx_buffer--;
			break;
		}
		res++;
		pddx_buffer++;
		nddx_buffer--;
	}
	if (leidos)
	{
		*res = 0;
		if (nddx_buffer)
		{
			memcpy(ddx_buffer,pddx_buffer,nddx_buffer);
			pddx_buffer = ddx_buffer;
			nddx_buffer = nddx_buffer + ixx_lee(fd_file,ddx_buffer+nddx_buffer,2048-nddx_buffer);
		}
		return _res; 
	}
	return NULL;
}
int cierra_lectura_ddx(int fd_file)
{
	nddx_buffer = 0;
	pddx_buffer = ddx_buffer;
	return ixx_cierra(fd_file);
}

/* esta rutina solo se ejecuta con un bloqueo de escritura */
int dat_abre(char *nom,char **descamp,char *defectos,short *longi,IXXFILE *ixxfic,int *ojo,FgvStdTabla *dtabla,char *pathdef)
{
    int fd,i,ncamp,nc,creado = 0,nfd,ok,k,j,n,haypase = 0,r;
    short tamreg,ntamreg,posi;
    int convierte = 0,discrepa = 0,len;
    char nombre[256],c;
    long reg;
    FILE *file;
	int fd_file;
    char *tmpreg,*ntmpreg;
    Uchar *p1,*p2;
    short *ps,tot,nps,cam;
    short *pase = NULL;
    /* de momento dado el agisys esto esta asi */
    short tipos[257];
    short lens[257];
    short ntipos[257];
    short nlens[257];
    int fdbtr = -1;	
#ifdef AGIEXTEND
    char p_block[512+128];
#endif
    
    sprintf(nombre,"%s.dat",nom);
    fd = ixx_abre_dat(nombre);
    if (fd < 0 && errno == ENOENT)
    {
        sprintf(nombre,"%s.tmx",nom);
        fd = ixx_abre_dat(nombre);
        if (fd < 0)
        {
#ifdef AGIEXTEND
            if (con_btrieve)
            {
                sprintf(nombre,"%s.dbt",nom);
                i = 0;
                filtro(nombre,0);
                fdbtr = BTRV_(0, p_block,"",&i,nombre,-2);
                if (fdbtr) fdbtr = -1;
            }
#endif            
            if (fdbtr == -1 && !no_crear_otro)
            {
                sprintf(nombre,"%s.dbf",nom);
                fd = ixx_abre_dat(nombre);			 
                if (fd >= 0)
                {
                    ixx_dat_cierra(fd);
                    /* ojo hay un DBF */
                    no_crear_otro = 1;
                    fd = dbf_abre(nom,descamp,defectos,longi,ixxfic,ojo,dtabla,pathdef);
                    no_crear_otro = 0;					
                }
            }		  
            if (fd >= 0 && fdbtr == -1 && !no_crear_otro)
            {			  
                i = fd;
                sprintf(nombre,"%s.dat",nom);
                fd = ixx_crea_nuevo(nombre);
                if (fd >= 0)
                {					
                    len = dtabla->reclen + 1;
                    if (ixxdat_aviso)
                    {	   			    
                        dbf_tamfichero(i,0L,len-1,&reg);
                        (*ixxdat_aviso)(0,reg,nom);
                    }			 				 
                    tmpreg = malloc(len);				 
                    reg = 1L;
                    no_read_error++;
                    while ((r = dbf_lee(i,reg,tmpreg,len-1)) >= 0)
                    {
                        if (ixxdat_aviso && !(reg%25))
                        {				   
                            (*ixxdat_aviso)(1,reg,nom);
                        }
                        ixx_graba(fd,tmpreg,len-1);
                        if (!r) c = 10;else c = 0;
                        ixx_graba(fd,&c,1);
                        reg++;
                    }
                    no_read_error--;
                    if (ixxdat_aviso)
                    {
                        (*ixxdat_aviso)(2,0L,nom);
                    }
                    free(tmpreg);				 			
                    ixx_nuevo_cierra(fd);
                    creado = 1;
                }
                dbf_cierra(i);
                if (creado)
                {
                    sprintf(nombre,"%s.dbf",nom);
                    ixx_unlink_(nombre);
                    sprintf(nombre,"%s.dat",nom);
                }			  
            }
            else
            {              
                sprintf(nombre,"%s.dat",nom);
                fd = ixx_crea_nuevo(nombre);
                if (fd >= 0)
                {
                    ixx_nuevo_cierra(fd);
                    if (fdbtr == -1)
                        creado = 1;
                }
            }
            fd = ixx_abre_dat(nombre);
        }
        else
        {
            ixx_dat_cierra(fd);
            sprintf(nombre,"%s.dat",nom);
            sprintf(nombre+128,"%s.tmx",nom);
            ixx_rename_(nombre+128,nombre);
            fd = ixx_abre_dat(nombre);
        }
        if (fdbtr == -1)
        {
            sprintf(nombre,"%s.ddx",nom);
            unlink(nombre);
        }
    }                    
	if (fd >= 0 && descamp)
	{
		/*
		sprintf(nombre,"%s.tmx",nom);
		ixx_unlink_(nombre);
		*/
		
		sprintf(nombre,"%s.ddx",nom);
		if (creado)
			fd_file = -1;
		else
			fd_file = ixx_abre_dat(nombre);
		if (fd_file < 0)
		{
			ntamreg = ixxdat_graba_ddx(descamp,nombre,nom,pathdef);
			if (ntamreg && longi)
				*longi = ntamreg;
			if (fdbtr > -1)
			{
				sprintf(nombre,"%s.ddx",nom);
				fd_file = ixx_abre_dat(nombre);
			}
		}
		if (fd_file > -1)
		{
			/* falta controlar descripcion !!! */
			ncamp = 0;while(descamp[ncamp]) ncamp++;
			
			nombre[0] = 'N';
			lectura_ddx(nombre,250,fd_file);
			if (nombre[0] != 'N')
				convierte = 1;
			tamreg = 0;
			ntamreg = 0;
			nc = 0;
			while(lectura_ddx(nombre,250,fd_file) && nc < 256)
			{
				if (nc >= ncamp)
				{
					discrepa = 2;
					break;
				}
				nombre[1] = 0;
				nombre[7] = 0;
				tipos[nc] = atoi(nombre);
				lens[nc] = atoi(nombre+2);
				strcpy(nombre,descamp[nc]);
				nombre[1] = 0;
				nombre[7] = 0;
				ntipos[nc] = atoi(nombre);
				nlens[nc] = atoi(nombre+2);
				if (discrepa < 2)
				{
					if (tipos[nc] != ntipos[nc])
					{
						if (tipos[nc] != IXXCSHORT && tipos[nc] != IXXCDOUBLE)
						{
							discrepa = 2;
						}
						else if (ntipos[nc] != IXXCSHORT && ntipos[nc] != IXXCDOUBLE)
							{
								discrepa = 2;
							}
							else if (nlens[nc] != sizeof(short) && nlens[nc] != sizeof(double))
								{
									discrepa = 2;
								}
								else if (lens[nc] != sizeof(short) && lens[nc] != sizeof(double))
									{
										discrepa = 2;
									}
									else
										discrepa = 1;
					}
					else
					{
						if (lens[nc] != nlens[nc])
						{
							if (tipos[nc] != IXXCALFA || lens[nc] > nlens[nc])
							{
								discrepa = 2;
							}
							else
							{
								discrepa = 1;
							}
						}
					}
				}
				tamreg += lens[nc];
				ntamreg += nlens[nc];
				nc++;
			}
			if (!discrepa && nc < ncamp)			
				discrepa = 1;
			
			for(i = nc;i < ncamp;i++)
			{
				strcpy(nombre,descamp[i]);
				nombre[1] = 0;
				nombre[7] = 0;
				ntipos[i] = atoi(nombre);
				nlens[i] = atoi(nombre+2);
				ntamreg += nlens[i];
			}
			if (!discrepa && fdbtr > -1)
				discrepa = 1;
			cierra_lectura_ddx(fd_file);
			
			if (discrepa)
			{
				if (discrepa == 1 && nc < ncamp)			
				{				
					char tmp[1024];
					extern char *mi_fichero(char *f);
					extern char *mi_sqltipo(int mitipo,int milen);
					
					for(i = nc;i < ncamp;i++)
					{
						sprintf(tmp,"ALTER TABLE %s ADD COLUMN C%03d %s\r\n",mi_fichero(nom),i,mi_sqltipo(ntipos[i],nlens[i]));
						sql_txt_respaldo_sentencia(tmp);
					}										
				}

				sprintf(nombre,"%s.dps",nom);
				file = fopen(nombre,"r");
				if (file)
				{
					pase = malloc(ncamp*sizeof(short));
					ok = 0;
					while(fgets(nombre,250,file))
					{
						if (ok >= ncamp)
						{
							if (ixxmen_aviso)
							{
								sprintf(nombre,"Mal fichero de pase %s.dps",nom);
								(*ixxmen_aviso)(nombre);
							}
							ok = -1;
							break;
						}
						pase[ok] = atoi(nombre);
						if (pase[ok] != -1 && (pase[ok] < 0 || pase[ok] >= nc))
						{
							if (ixxmen_aviso)
							{
								sprintf(nombre,"Mal fichero de pase %s.dps Campo[%d] -[%d]",nom,ok,pase[ok]);
								(*ixxmen_aviso)(nombre);
							}
							ok = -1;
							break;
						}
						ok++;
					}
					if (ok != -1)
					{
						for (;ok < ncamp;ok++)
							pase[ok] = -1;
						discrepa = 1;
						haypase = 1;
					}
					else
						discrepa = 2;
					fclose(file);
				}
				if (longi)
				{
					if (ntamreg != (*longi))
					{
						if (discrepa == 1)
						{
							*longi = ntamreg;
						}
						else
						{
							discrepa = 2;
						}
					}
				}
			}
			else
			{
				if (longi)
				{
					if (ntamreg != (*longi))
					{
						if (discrepa == 1)
						{
							*longi = ntamreg;
						}
						else
						{
							discrepa = 2;
						}
					}
				}
			}
			/* comprobar claves como pertenecientes a campos validos */
			if (discrepa)
			{
				if (!ixxfic)
				{
					ixx_dat_cierra(fd);
					ret( -1 );
				}
				ps = (short *)&(descamp[ncamp+1]);
				tot = *ps;
				nps = 0;
				for (i = 0;i < ixxfic->nclaves;i++)
				{
					ok = ixxfic->claves[i].clave_ncampos;
					if (ixxfic->claves[i].clave_tipo == IXXCLAVEDUP)
						ok--;
					for (j = 0;j < ok;j++)
					{
						if (nps >= tot)
						{
							j = 0;
							*ojo = 1;
							break;
						}
						ps++;
						cam = *ps;
						posi = 0;
						for (k = 0;k < cam;k++)
						{
							posi += nlens[k];
						}
						if (ixxfic->claves[i].clave_campos[j].len != nlens[k] ||
							ixxfic->claves[i].clave_campos[j].type != ntipos[k] ||
							ixxfic->claves[i].clave_campos[j].pos_en_reg != posi)
						{
							*ojo = 1;
							break;
						}
						nps++;
					}
					if (j < ixxfic->claves[i].clave_ncampos)
						break;
				}
			} 
							
			if (discrepa && !ixx_lseek(fd,0L,SEEK_END) && fdbtr == -1)
			{
				/* si esta vacio el fichero pasar de todo */
				sprintf(nombre,"%s.ddx",nom);
				ntamreg = ixxdat_graba_ddx(descamp,nombre,nom,pathdef);
				if (ntamreg && longi && discrepa != 3)
					*longi = ntamreg;
				discrepa = 0;
			}
			if (discrepa == 1)
			{
				if (ixx_lck_lseek(ixxfic->fd,LCKREZONAINI,SEEK_SET) == -1L)
				{
					eret(IXXELSEEK,errno,-1);
				}
				if (ixx_lock(ixxfic->fd,LK_NBLCK,1L) < 0)
				{
                    if ((errno == EACCES || errno == EAGAIN || errno == 13 || errno == 35))
                    {
					   char c;
					   ixx_dat_cierra(fd);
					   while(-1)
					   {
						  if (ixx_lseek(ixxfic->fd,LCKREZONAINI,SEEK_SET) == -1L)
						  {
							 eret(IXXELSEEK,errno,-1);
						  }
						  if (ixx_lee(ixxfic->fd,&c,1) == 1) break;
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
					   sleep(5);
					   return dat_abre(nom,descamp,defectos,longi,ixxfic,ojo,dtabla,pathdef);
					}
					else
						ret( -1 );
				}
			}
			if (discrepa == 1)
			{
				sprintf(nombre,"%s.tmx",nom);
				nfd = ixx_crea_nuevo(nombre);
				if (nfd >= 0)
				{
					ixx_nuevo_cierra(nfd);
					nfd = ixx_abre_dat(nombre);
					if (nfd >= 0)
					{
						tmpreg = malloc(tamreg+1);
						ntmpreg = malloc(ntamreg+1);
						if (!tmpreg || !ntmpreg) 
						{
							ixx_lck_lseek(ixxfic->fd,LCKREZONAINI,SEEK_SET);
							ixx_lock(ixxfic->fd,LK_UNLCK,1L);
							ret(-1);
						}
#ifdef AGIEXTEND
						if (fdbtr > -1)
						{
							i = 512;
							BTRV_(15,p_block,p_block+128,&i,nombre,0);
							reg = (*(long *)(p_block+128+6));
							if ( (*(short *)(p_block+128)) != tamreg)
							{
								sprintf(nombre,__Texto[516],*(short *)(p_block+128),tamreg);
								if (ixxmen_aviso)
								{
									(*ixxmen_aviso)(nombre);
								}
							}
						}
						else
#endif
							reg = ixx_lseek(fd,0L,SEEK_END)/(tamreg+1);
				
						if (ixxdat_aviso)
						{
							(*ixxdat_aviso)(0,reg,nom);
						}
						reg = 0L;
						if (fdbtr > -1)
						{
							ixx_dat_cierra(fd);
							fd = -1;
							sprintf(nombre,"%s.dat",nom);
							ixx_unlink_(nombre);
						}
						else
						{
							ixx_lseek(fd,0L,SEEK_SET);
						}
						ixx_lseek(nfd,0L,SEEK_SET);
						ok = 0;
						while(-1)
						{
		#ifdef AGIEXTEND
							if (fdbtr > -1)
							{
								if (BTRV_(24,p_block,tmpreg,&tamreg,"",0)) break;
								tmpreg[tamreg] = 10;
							}
							else
							{
		#endif
								if (ixx_lee(fd,tmpreg,tamreg+1) != (tamreg+1))
								{
									break;
								}
		#ifdef AGIEXTEND
							}
		#endif
							reg++;
							ok = 1;
							p1 = tmpreg;
							p2 = ntmpreg;
							memcpy(ntmpreg,defectos,ntamreg);
							ntmpreg[ntamreg] = tmpreg[tamreg];
							if (tmpreg[tamreg])
							{
								if (pase)
								{
									for (i = 0;i < ncamp;i++)
									{
										k = pase[i];
										if (k != -1)
										{
											n = 0;
											for(j = 0;j < k;j++) n += lens[j];
											p1 = tmpreg + n;
		#ifdef AGIEXTEND
											if (fdbtr > -1)
												ixx_dat_pasa_a_campo(p2,p1,ntipos[i],nlens[i],tipos[k],lens[k],fdbtr);
											else
		#endif
											if (ntipos[i] == tipos[k] && nlens[i] == lens[k])
											{
												memcpy(p2,p1,lens[k]);
											}
											else
											{
												if (ntipos[i] == tipos[k])
												{
													if (lens[k] < nlens[i])
														n = lens[k];
													else
														n = nlens[i];
													memcpy(p2,p1,n);
												}
												else
												{
													ixx_dat_pasa_a_campo(p2,p1,ntipos[i],nlens[i],tipos[k],lens[k],fdbtr);
												}
											}
										}
										
										p2 += nlens[i];
									}
								}
								else
								{
									for (i = 0;i < nc;i++)
									{
		#ifdef AGIEXTEND
										if (fdbtr > -1)
										{
											ixx_dat_pasa_a_campo(p2,p1,ntipos[i],nlens[i],tipos[i],lens[i],fdbtr);
										}
										else
		#endif
										if (tipos[i] != ntipos[i])
										{
											if (ntipos[i] == IXXCDOUBLE)
											{
												stdbl((double)ldint(p1),p2);
											}
											else
											{
												stint((short)lddbl( p1 ),p2);
											}
										}
										else
										{
											memcpy(p2,p1,lens[i]);
										}
										
										p1 += lens[i];
										p2 += nlens[i];
									}
								}
							}
							if (ixx_graba(nfd,ntmpreg,ntamreg+1) != (ntamreg+1))
							{
								IXXerror(IXXEWRITE,errno,-1);
								ok = 0;
								break;
							}
							if (!(reg%54) && ixxdat_aviso)
							{
								(*ixxdat_aviso)(1,reg,nom);
							}
						}
						if (ixxdat_aviso)
						{
							(*ixxdat_aviso)(2,reg,nom);
						}
						ixx_dat_cierra(nfd);
						free(ntmpreg);
						free(tmpreg);
						if (ok)
						{
							sprintf(nombre+128,"%s.dat",nom);
							sprintf(nombre,"%s.tmx",nom);
							if (fd > -1)
								ixx_dat_cierra(fd);
							if (fdbtr > -1 || ixx_unlink_(nombre+128) >= 0)
							{
								if (ixx_rename_(nombre,nombre+128) < 0)
								{
									ixx_lck_lseek(ixxfic->fd,LCKREZONAINI,SEEK_SET);
									ixx_lock(ixxfic->fd,LK_UNLCK,1L);
									ret(-1);
								}
								sprintf(nombre,"%s.ddx",nom);
								if (ixxdat_graba_ddx(descamp,nombre,nom,pathdef) != ntamreg)
								{
									sprintf(nombre,__Texto[160],ntamreg);
									if (ixxmen_aviso)
									{
										(*ixxmen_aviso)(nombre);
									}
								}
								if (longi)
									*longi = ntamreg;
								sprintf(nombre,"%s.dps",nom);
								unlink(nombre);
								discrepa = 0;
								}
								else
								{
									sprintf(nombre,"%s.tmx",nom);
									ixx_unlink_(nombre);
									discrepa = 0;
									if (longi && fdbtr > -1)
										*longi = ntamreg;
								}
								sprintf(nombre,"%s.dat",nom);
								fd = ixx_abre_dat(nombre);
							}
																										 }
								}
								ixx_lck_lseek(ixxfic->fd,LCKREZONAINI,SEEK_SET);
								ixx_lock(ixxfic->fd,LK_UNLCK,1L);
						}
						if (discrepa == 2) /* esta jodida la cosa */
						{
							if (longi)
								*longi = 0;
						}
					}
					if (pase)
						free(pase);
#ifdef AGIEXTEND
					if (fdbtr > -1)
					{
						i = 0;
						BTRV_(1,p_block,"",&i,"",0);
						sprintf(nombre,"%s.dbt",nom);
						ixx_unlink_(nombre);
						if (fd == -1)
						{
							sprintf(nombre,"%s.dat",nom);
							fd = ixx_crea_nuevo(nombre);
							if (fd >= 0)
							{
								ixx_nuevo_cierra(fd);
							}
							fd = ixx_abre_dat(nombre);
						}
						fdbtr = -1;
					}
#endif
				}        

				{    
				if (!ejecutando_respaldo && modo_respaldo > -1 && dir_respaldo && fd >= 0)
				{   
					int xfd;
					char otro_nom[128];
					char tmp_nom[128];
					char *_p1;
					int ojete = 0;
					int ant_default_dbf_id = default_dbf_id;
					int hacerla = 0;
					ejecutando_respaldo = 1;

					if (!fd_respaldo)
					{
						int kki;
						fd_respaldo = (int *)malloc(max_fd_respaldo * sizeof(int));
						for (kki = 0;kki < max_fd_respaldo;kki++)
							fd_respaldo[kki] = -1;
					}
					if (fd >= max_fd_respaldo)
					{
						int kki = max_fd_respaldo;
						max_fd_respaldo = fd + 256; /* guau! */
						fd_respaldo = (int *)realloc((char *)fd_respaldo,max_fd_respaldo * sizeof(int));
						for (;kki < max_fd_respaldo;kki++)
							fd_respaldo[kki] = -1;
					}
        
					if (lista_respaldo && !mi_lista_respaldo)
					{
						FILE *f = fopen(lista_respaldo,"r");
						if (f)
						{
							int lineas = 0,kk;
							while(fgets(tmp_nom,120,f))
							{
								lineas++;
							}
							rewind(f);
							mi_lista_respaldo = (char **)malloc(sizeof(char *)*(lineas+1));
							kk = 0;
							while(kk < lineas && fgets(tmp_nom,120,f))
							{
								quitan(tmp_nom);
								quitab(tmp_nom);
								mi_lista_respaldo[kk] = malloc(strlen(tmp_nom)+1);
								strcpy(mi_lista_respaldo[kk],tmp_nom);
								kk++;
							}		          
							mi_lista_respaldo[kk] = NULL;
							fclose(f);
						}
					}        
					if (mi_lista_respaldo)
					{
						int kk;
						for (kk = 0;mi_lista_respaldo[kk];kk++)
						{
							if (!strcmp(quita_path(mi_lista_respaldo[kk]),quita_path(nom)))
							{
								hacerla = 1;
								break;
							}
						}
					}
					else
						hacerla = 1;        
        
					if (!hacerla)
					{
						fd_respaldo[fd] = -1;
					}
					else
					{
						strcpy(otro_nom,dir_respaldo);
						for (_p1 = nom;*_p1 && *_p1 != '/' && *_p1 != '\\';_p1++);
						if (*_p1)
						{
							strcat(otro_nom,_p1);
						}
						else
						{
							strcat(otro_nom,"/");
							strcat(otro_nom,quita_path(nom));
						}
						crea_dir(otro_nom);   
						default_dbf_id = modo_respaldo;
						no_report_error = 1;
						no_crear_cabecera = 1;
						xfd = dbf_abre(otro_nom,descamp,defectos,longi,ixxfic,&ojete,dtabla,pathdef);
						no_crear_cabecera = 0;
						if (xfd < 0)
						{
							strcpy(nombre,nom);
							strcat(nombre,".dat");
							strcpy(tmp_nom,otro_nom);
							strcat(tmp_nom,".dat");
							/* copiar */
							copiarfich(nombre,tmp_nom,0);	  
							xfd = dbf_abre(otro_nom,descamp,defectos,longi,ixxfic,&ojete,dtabla,pathdef);	  	  
						}
						no_report_error = 0;
						fd_respaldo[fd] = xfd;
						/*dbf_cierra(xfd);*/
					}
					default_dbf_id = ant_default_dbf_id;
					ejecutando_respaldo = 0;
		}    
	}

return(fd);
}

int ixx_borra_respaldo(char *nom)
{
    if (!ejecutando_respaldo && modo_respaldo > -1 && dir_respaldo)
    {   
        char otro_nom[128];
        char *_p1;
        strcpy(otro_nom,dir_respaldo);
        for (_p1 = nom;*_p1 && *_p1 != '/' && *_p1 != '\\';_p1++);
        if (*_p1)
        {
            strcat(otro_nom,_p1);
        }
        else
        {
            strcat(otro_nom,"/");
            strcat(otro_nom,quita_path(nom));
        }
        strcat(otro_nom,".dbf");
        ixx_unlink_(otro_nom);
    }
    return(0);
}

void ixx_libera_respaldo()
{
    if (mi_lista_respaldo)
    {
        int kk;
        for (kk = 0;mi_lista_respaldo[kk];kk++)
        {
            free(mi_lista_respaldo[kk]);
        }
        free(mi_lista_respaldo);
        mi_lista_respaldo = NULL;
    }
    if (fd_respaldo)
    {
        free((char *)fd_respaldo);
        fd_respaldo = NULL;
    }
}    
    
    
int dat_lee(fd,reg,buf,len)
int fd;
long reg;
Uchar *buf;
int len;
{
    char c;
    long pos = (reg-1)*(len+1);
	
    if (reg <= 0L /*|| reg > ixxfich->cabecera.nrecords*/) 
        eret(IXXECORRUP,100,-1);
	
    
    if (ixx_lseek(fd,pos,SEEK_SET) == -1L) eret(IXXELSEEK,errno,-1);
    if (ixx_lee(fd,buf,len) != len) eret(IXXEREAD,errno,-1);
    if (ixx_lee(fd,&c,1) != 1) eret(IXXEREAD,errno,-1);
    if (c) ret(0);
    ret(1);
}
    
int dat_graba(int fd,long reg,Uchar *buf,int len,int flag)
{
    long pos = (reg-1)*(len+1);
    char c = 10;
	
    if (reg <= 0L /*|| reg > ixxfich->cabecera.nrecords*/)
        eret(IXXECORRUP,98,-1);
    
    if (ixx_lseek(fd,pos,SEEK_SET) == -1L) eret(IXXELSEEK,errno,-1);
    if (ixx_graba(fd,buf,len) != len) eret(IXXEWRITE,errno,-1);
    if (ixx_graba(fd,&c,1) != 1) eret(IXXEWRITE,errno,-1);
    
    
    {
        
        if (!ejecutando_respaldo && modo_respaldo > -1 && dir_respaldo)
        {   
            int xfd;
            int ant_default_dbf_id = default_dbf_id;
            ejecutando_respaldo = 1;
            no_report_error = 1;
            xfd = fd_respaldo[fd];   
            if (xfd >= 0)
            {
                default_dbf_id = modo_respaldo;   
                dbf_graba(xfd,reg,buf,len,0);   
            }
            no_report_error = 0;
            default_dbf_id = ant_default_dbf_id;
            ejecutando_respaldo = 0;
        }
                
    }
            
            
    ret(0);
}

int dat_borra(int fd,long reg,int len)
{		
	long pos = (reg-1)*(len+1);
	int glen,tlen;
    /*long pos = (reg-1)*(len+1)+len;*/
    char c = 0;	
    if (reg <= 0L /*|| reg > ixxfich->cabecera.nrecords*/) 
        eret(IXXECORRUP,99,-1);

    
    if (ixx_lseek(fd,pos,SEEK_SET) == -1L) eret(IXXELSEEK,errno,-1);

	glen = len;
	while(glen > 0)
	{
		if (glen > 10000)
		{
			tlen = 10000;
		}
		else
		{
			tlen = glen;
		}
		memset(buf_borra,0,tlen);
		if (ixx_graba(fd,buf_borra,tlen) != tlen) eret(IXXEWRITE,errno,-1);
		glen -= tlen;
	}
    if (ixx_graba(fd,&c,1) != 1) eret(IXXEWRITE,errno,-1);
    
    {
        
        if (!ejecutando_respaldo && modo_respaldo > -1 && dir_respaldo)
        {   
            int xfd;
            int ant_default_dbf_id = default_dbf_id;
            ejecutando_respaldo = 1;
            no_report_error = 1;
            xfd = fd_respaldo[fd];   
            if (xfd >= 0)
            {
                default_dbf_id = modo_respaldo;   
                dbf_borra(xfd,reg,len);
            }
            no_report_error = 0;
            default_dbf_id = ant_default_dbf_id;
            ejecutando_respaldo = 0;
        }
                    
    }
                
    ret(0);
}


int dat_cierra(int fd)
{
    int r = ixx_dat_cierra(fd);
    
    {
        
        if (!ejecutando_respaldo && modo_respaldo > -1 && dir_respaldo)
        {   
            int xfd;
            int ant_default_dbf_id = default_dbf_id;
            ejecutando_respaldo = 1;
            no_report_error = 1;
            xfd = fd_respaldo[fd];   
            if (xfd >= 0)
            {
                default_dbf_id = modo_respaldo;   
                dbf_cierra(xfd);
            }
            fd_respaldo[fd] = -1;  
            default_dbf_id = ant_default_dbf_id;
            no_report_error = 0;
            ejecutando_respaldo = 0;
        }
                        
    }
                    
    return(r);
}
                
                
int dat_clave(ixxfile,recbuf,clabuf,laclave,reg,modo,parte,mode)
IXXFILE *ixxfile;
Uchar *recbuf;
Uchar **clabuf;
short laclave;
long reg;
int modo;
int parte; /* permite ignorar algunos segmentos de cara a saltar bloques de claves */
int mode;
{
    int nkeys;
    int k,nc,c,t,ext,i;
    int pos,len,grado;
    Uchar *cbuf = NULL;
    short in;
    long lo;
    double d;
    float f;
    char tmp_d[512];
	int sal = 0;
    
    if (laclave == -1)
    {
        nkeys = ixxfile->nclaves;
        k = 0;
    }
    else
    {
        k = laclave;
        nkeys = k + 1;
    }
    
    if (parte)
    {
        if (mode != IXXGREAT && mode != IXXGTEQ && mode != IXXLOW && mode != IXXLOWEQ)
            parte = 0;
        else
            if (mode == IXXGREAT || mode == IXXLOWEQ)
                grado = 1;
            else
                grado = 0;
    }
    
    for (;k < nkeys;k++)
    {
        nc = ixxfile->claves[k].clave_ncampos;
        if (clabuf)
        {
            if (modo < 2)
                cbuf = clabuf[k];
            else
                cbuf = *clabuf;
        }
        for (c = 0;c < nc;c++)
        {
            pos = ixxfile->claves[k].clave_campos[c].pos_en_reg;
            len = ixxfile->claves[k].clave_campos[c].len;
            t = ixxfile->claves[k].clave_campos[c].type;
            ext = ixxfile->claves[k].clave_campos[c].extra_type;
            if (!modo)
            {
                if (pos == -1)
                {
                    *((long *)cbuf) = reg; /* totalmente "machine dependant" */
                }
                else
                {
                    if (parte && c >= parte)
                    {
                        switch (t)
                        {
                        case IXXCALFA :
                            if (grado)
                                memset(cbuf,0xff,len);
                            else
                                memset(cbuf,0x0,len);
                            break;
                        case IXXCSHORT :
                            if (grado)
                                in = (short)0x7FFF;
                            else
                                in = ((short)0x8000);
                            memcpy(cbuf,(char *)&in,len);
                            break;
                        case IXXCLONG :
                            if (grado)
                                lo = (long)0x7FFFFFFFL;
                            else
                                lo = ((long)0x80000000L);
                            memcpy(cbuf,(char *)&lo,len);
                            break;
                        case IXXCDOUBLE :
                            if (grado)
                                d = MAXDOUBLE;
                            else
								d = MINDOUBLE;
                            memcpy(cbuf,(char *)&d,len);
                            break;
                        case IXXCFLOAT :
                            if (grado)
                                f = MAXFLOAT;
                            else
                                f = MINFLOAT;
                            memcpy(cbuf,(char *)&f,len);
                            break;
						default:
                            memcpy(cbuf,recbuf+pos,len);
                            break;
                                    }
                                        }
                                        else
                                        {
                                            switch (t)
                                            {
                                            case IXXCSHORT :
                                                *((short *)cbuf) = (short)ldint( recbuf+pos );
                                                break;
                                            case IXXCLONG :
                                                *((long *)cbuf) = ldlong( recbuf+pos );
                                                break;
                                            case IXXCDOUBLE :
                                                *((double *)cbuf) = lddbl( recbuf+pos );
												if (*((double *)cbuf) >= maximustremendus)
												{
													sal = -1;
												}
												break;
                                            case IXXCFLOAT :
                                                *((float *)cbuf) = ldfloat( recbuf+pos );
                                                break;
                                            case IXXCALFA :
                                            default:
                                                memcpy(cbuf,recbuf+pos,len);
                                                if ( (ext & IXXCNOCASE) )
                                                {
                                                    for (i = 0;i < len;i++)
                                                    {
                                                        if (cbuf[i] >= 'a' && cbuf[i] <= 'z')
                                                            cbuf[i] -= 32; 
														else if (cbuf[i] == 164)
															cbuf[i] = 165;
                                                    }
                                                }
                                                break;
                                            }
                                        }
                                    }
                                }
                                else
                                {
                                    if (modo == 2)
                                    {
                                        if (pos > -1)
                                           memcpy(recbuf+pos,cbuf+pos,len);
                                    }
                                    else
                                        if (pos != -1)
                                        {
                                            switch (t)
                                            {
                                            case IXXCSHORT :
                                                stint( *((short *)cbuf) ,recbuf+pos);
                                                break;
                                            case IXXCLONG :
                                                stlong( *((long *)cbuf) ,recbuf+pos);
                                                break;
                                            case IXXCDOUBLE :
                                                stdbl( *((double *)cbuf) ,recbuf+pos);
                                                break;
                                            case IXXCFLOAT :
                                                stfloat( *((float *)cbuf) ,recbuf+pos);
                                                break;
                                            case IXXCALFA :
                                            default:
                                                memcpy(recbuf+pos,cbuf,len);
                                                break;
                                            }
                                        }
                                }
                                if (modo < 2)
                                    cbuf += len;
                }
        }
        ret( sal );
}

int maxi_regi(ixxfile)
IXXFILE *ixxfile;
{
    long maximus = IXXFILEEND / (ixxfile->tamreg+1);
    if (maximus > LCKDZONAALL)
        maximus = LCKDZONAALL;
    ixxfile->maximo_registros = maximus;
    ret( 0 );
}

int dat_tamfichero(fd,total,tamreg,reales)
int fd;
long total;
int tamreg;
long *reales;
{
    long pos;
    
    pos = ixx_lseek(fd,0L,SEEK_END);
    if (pos == -1L) ret(-1);
    pos /= (tamreg+1);
    
    if (reales)
    {
        *reales = pos;
    }
    
    if (total < pos)
        ret(1);
    if (total > pos)
        ret(-1);
    
    ret(0);
}
        
        
int ixx_comprueba_filtro(IXXCLAVE *laclave,Uchar *clave)
{
    int c,r,t;
    int res = 1;
    int mode = 0;
    struct ixx_tabla_de_filtro *fl = laclave->filtro,*fltmp;
    
    if (laclave->clave_tipo != IXXCLAVENODUP)
        mode = 1;
    
    while (fl)
    {
        c = (int)fl->condicion;
        if ( (c/10) == 1)
        {
            fltmp = (struct ixx_tabla_de_filtro *)fl->referencia;
            r = fltmp->resultado;
        }
        else
        {
            if ( (c/10) == 2)
            {
                /* !!! */ 
            }
            t = ixxcompara_clave(fl->referencia,clave,laclave,mode);
            r = 1;
            switch(c%10)
            {
            case 0:
                if (t)
                    r = 0;
                break;
            case 1:
                if (!t)
                    r = 0;
                break;
            case 2:
                if (t >= 0)
                    r = 0;
                break;
            case 3:
                if (t <= 0)
                    r = 0;
                break;
            case 4:
                if (t > 0)
                    r = 0;
                break;
            case 5:
                if (t < 0)
                    r = 0;
                break;
                        }
                    }
                    fl->resultado = r;
                    if (fl->modo == 0)
                    {
                        if (!r)
                        {
                            res = 0;
                            break;
                        }
                        res = r;
                    }
                    else
                        if (fl->modo == 1)
                        {
                            if (res || r)
                            {
                                res = 1;
                                break;
                            }
                            res = r;
                        }
                        
                        fl = fl->siguiente;
                }
    return(res);
}
            

#ifdef WATCOMNLM
int locking(int fd,int mode,long region)
{
    long pos = ixx_lck_lseek(fd,0L,SEEK_CUR);
    int r;
    
    if (mode == LK_UNLCK)
    {
        return(unlock(fd,pos,region));
    }
    
    if (mode != LK_WAIT)
    {
        if (read(fd,&r,1) < 0)
        {
            ixx_lck_lseek(fd,pos,SEEK_SET);
            errno = EACCES;
            return(-1);
        }
        ixx_lck_lseek(fd,pos,SEEK_SET);
    }
    
    r = lock(fd,pos,region);
    if (r < 0)
        errno = EACCES;
    return(r);
}
#endif



/******** Podria ir en su fichero */
/* sistema de control de update */
static int  *fd_diu = NULL;
static long  *modo_diu = NULL;
static int  max_fd_diu = 256;

static int  fd_ciu = -1;

static char **inclusiones_diu = NULL;

static long control_diu = 0L;

/*
#define ixx_abre_share(a)    sopen(a,O_RDWR | O_BINARY,SH_DENYNO)
#define ixx_abre_dat(a)      sopen(a,O_RDWR | O_BINARY,SH_DENYNO)
#define ixx_abre_exclu(a)    sopen(a,O_RDWR | O_BINARY,SH_DENYRW)
#define ixx_lee              read
#define ixx_graba            write
#define ixx_lseek            lseek
#define ixx_cierra           close
#define ixx_dat_cierra       close

#define ixx_lck_lseek        lseek
#define ixx_lock             locking

#define FGV_ID_CIU    0x5DE861C7
#define FGV_ID_DIU    0x5DE861C8
*/

int carga_lista_diu()
{
    if (!inclusiones_diu)
    {
        if (!lista_diu)
        {
            inclusiones_diu = (char **)malloc(sizeof(char *) * 2);
            inclusiones_diu[0] = malloc(10);
            strcpy(inclusiones_diu[0],"*");
            inclusiones_diu[1] = NULL;
        }
        else
        {
            FILE *f = fopen(lista_diu,"r");
            if (f)
            {
                char tmp_nom[128];
                int lineas = 0,kk;
                while(fgets(tmp_nom,120,f))
                {
                    quitan(tmp_nom);
                    quitab(tmp_nom);
					if (tmp_nom[0])
					{
						lineas++;
					}
                }
                rewind(f);
                inclusiones_diu = (char **)malloc(sizeof(char *)*(lineas+1));
                kk = 0;
                while(kk < lineas && fgets(tmp_nom,120,f))
                {
                    quitan(tmp_nom);
                    quitab(tmp_nom);
					if (tmp_nom[0])
					{
						inclusiones_diu[kk] = malloc(strlen(tmp_nom)+1);
						strcpy(inclusiones_diu[kk],tmp_nom);
						kk++;
					}
                }
                inclusiones_diu[kk] = NULL;
                fclose(f);
            }
        }
    }

    ixx_haydiu = 1;
    return(1);
}

static int crea_ciu()
{
    char tmp[256];
    strcpy(tmp,ixx_ciudiu);
    strcat(tmp,".ciu");
    fd_ciu = ixx_crea_nuevo(tmp);
    if (fd_ciu >= 0)
    {
        long id = FGV_ID_CIU;
        control_diu = 0L;
        ixx_lseek(fd_ciu,0L,SEEK_SET);
        ixx_graba(fd_ciu,&id,sizeof(id));
        ixx_graba(fd_ciu,&control_diu,sizeof(control_diu));
        id = time(NULL);
        ixx_graba(fd_ciu,&id,sizeof(id));
        id = 0L;
        ixx_graba(fd_ciu,&id,sizeof(id));

        ixx_graba(fd_ciu,&id,sizeof(id)); /* contador */
        ixx_graba(fd_ciu,&id,sizeof(id)); /* reservados */
        ixx_graba(fd_ciu,&id,sizeof(id));
        ixx_graba(fd_ciu,&id,sizeof(id));
        contador_global_diu = 0L;
        ixx_cierra(fd_ciu);
    }
    return(0);
}


static int crea_diu(int lfd)
{
    long id = FGV_ID_DIU;
    ixx_lseek(lfd,0L,SEEK_SET);
    ixx_graba(lfd,&id,sizeof(id));
    ixx_graba(lfd,&control_diu,sizeof(control_diu));
    id = time(NULL);
    ixx_graba(lfd,&id,sizeof(id));
    id = ixx_control_diu;
    ixx_graba(lfd,&id,sizeof(id));
}

static int lee_modo_diu(int fd)
{
	int lfd = fd_diu[(fd&~0x40000000)];
	ixx_lseek(lfd,(long)sizeof(long)*3,SEEK_SET);
	ixx_lee(lfd,&(modo_diu[(fd&~0x40000000)]),sizeof(long));	
	ixx_lseek(lfd,0L,SEEK_END);
}

static int update_diu(int modo)
{ 
    if (ixx_control_diu) /* en este caso no queda mas remedio que bloquear */
    {

        if (modo) return(0); /* no lo vamos a mantener bloqueado!!! */

        while(-1) /* se espera (de momento asi) indefinidamente */
        {
            ixx_lseek(fd_ciu,(long)sizeof(long),SEEK_SET);
            if (!ixx_lock(fd_ciu,LK_WAIT,1)) break;
            if (errno != EACCES && errno != EDEADLOCK && errno != EAGAIN  && errno != 13 && errno != 35)
            {
                break; /* glups!! */
            }
            if (ixxdat_micro_espera)
               (*ixxdat_micro_espera)();
        }
     
        ixx_lseek(fd_ciu,(long)(sizeof(long)*4),SEEK_SET);
        ixx_lee(fd_ciu,&contador_global_diu,sizeof(contador_global_diu));
        contador_global_diu++;
        ixx_lseek(fd_ciu,(long)(sizeof(long)*4),SEEK_SET);
        ixx_graba(fd_ciu,&contador_global_diu,sizeof(contador_global_diu));

        ixx_lseek(fd_ciu,(long)sizeof(long),SEEK_SET); 
        ixx_lock(fd_ciu,LK_UNLCK,1);
    }
    else
    {

        if (modo) return(0); /* de momento se protege la grabacion pero no la extraccion:
                                el extractor debe bloquear la zona del 'control_diu' y esperar
                                un tiempo para que sea seguro actulizar. Esto se podria hacer sin espera
                                (que evitaria el tiempo de espera extra de los puestos) usando un bloqueo
                                de lectura al estilo del indexado.
                                De momento esto es suficiente, basta esperar un par de segundos o en todo caso 
                                recoger los tamaños de los ficheros y comprobar si 'han crecido' en cuyo caso en vez
                                del 'trunc' crear el fichero incluyendo los nuevos registros.
                             */

        /* para actualizar el ciu si que hay que bloquear */
        do
        {
            if (ixx_lseek(fd_ciu,(long)sizeof(long),SEEK_SET) != sizeof(long))
			{
				break;
			}
            if (ixx_lee(fd_ciu,(char *)&control_diu,sizeof(long)) != sizeof(long))
            {
                sleep(1);
                continue;
            }
            break;
        } while(-1);

    }

    return(0);    
}

int es_fichero_para_diu(char *nom)
{
	int i,ok = 0;

	if (!ixx_haydiu)
		return 0;

        /* comprobar si es adecuado para incluir */        
        for (i = 0;inclusiones_diu && inclusiones_diu[i];i++)
        {
			if (*(inclusiones_diu[i]) == '-')
			{
				if (strstr(nom,inclusiones_diu[i]+1))
				{
					ok = 0;
					break;
				}
				continue;
			}
            if (!strcmp(inclusiones_diu[i],"*"))
            {
                ok = 1;
                break;
            }
            else
            if (strstr(nom,inclusiones_diu[i]))
            {
                ok = 1;
                break;
            }
        }
        /*****************************************/
        if (ok)
        {
            if (fd_ciu < 0)
            {
                char tmp[256];

                strcpy(tmp,ixx_ciudiu);
                strcat(tmp,".ciu");

                fd_ciu = ixx_abre_dat(tmp);
                if (fd_ciu < 0)
                {
                    crea_ciu();
                    fd_ciu = ixx_abre_dat(tmp);
                }
            }
            if (fd_ciu < 0)
            {
                ok = 0;
                ixx_haydiu = 0; /* desactivar !!! */
            }
        }
		return ok;
}

int acceso_a_ciu(int fd,char *nom)
{
	int i;
	int ok = 0;
	long tiempo;
	if (ixx_haydiu)
	{
        if (!fd_diu)
        {            
            fd_diu = (int *)malloc(sizeof(int)*max_fd_diu);
			modo_diu = (int *)malloc(sizeof(long)*max_fd_diu);
            for (i = 0;i < max_fd_diu;i++)
			{
                fd_diu[i] = -1;
				modo_diu[i] = 0;
			}
        }
        if ( fd >= 0 && (fd&~0x40000000) >= max_fd_diu)
        {
            i = max_fd_diu;
            max_fd_diu = (fd&~0x40000000) + 256; /* !! */
            fd_diu = (int *)realloc((char *)fd_diu,sizeof(int)*max_fd_diu);
			modo_diu = (int *)realloc((char *)modo_diu,sizeof(long)*max_fd_diu);
            for(;i < max_fd_diu;i++)
			{
                fd_diu[i] = -1;
				modo_diu[i] = 0;
			}
        }
		if (fd_ciu < 0 || fd < 0 || fd_diu[(fd&~0x40000000)] == -1)
		{
			if (es_fichero_para_diu(nom))
			{
				ok = 1;
			}
			else
			{
				if (fd >= 0)
				{
					fd_diu[(fd&~0x40000000)] = -2;
				}
			}
		}
		else if (fd >= 0 && fd_diu[(fd&~0x40000000)] >= 0)
		{
			ok = 1;
		}
		if (ok)
		{
			ok = 0;
			do 
			{
				ixx_lseek(fd_ciu,(long)sizeof(long)*2,SEEK_SET);
				if (ixx_lee(fd_ciu,(char *)&tiempo,sizeof(long)) != sizeof(long))
				{
			        if (ixxind_recespera)
					 (*ixxind_recespera)(100);
					ok = 1;
					sleep(1);
					continue;
				}
				break;
			} while(-1);
	        if (ok && ixxind_recespera)
 			   (*ixxind_recespera)(101);
		}
	}
	return 0;

}

static int update_ciu(char *el_diu,char *el_def,long regs)
{
long total = 0L;
long i;
char tmp_p[52];
char tmp_d[52];
long reg_s;
long flag;
int hueco = -1;

/* */



    do 
    {
        ixx_lseek(fd_ciu,(long)sizeof(long),SEEK_SET);
        if (ixx_lee(fd_ciu,(char *)&control_diu,sizeof(long)) != sizeof(long))
        {
            sleep(1);
            continue;
        }
        break;
    } while(-1);

    
    ixx_lseek(fd_ciu,(long)(sizeof(long)*3),SEEK_SET);
    ixx_lee(fd_ciu,(char *)&total,sizeof(long));
    ixx_lseek(fd_ciu,(long)(sizeof(long)*4),SEEK_CUR);
    for (i = 0;i < total;i++)
    {
       ixx_lee(fd_ciu,tmp_p,52);
       ixx_lee(fd_ciu,tmp_d,52);
       ixx_lee(fd_ciu,(char *)&reg_s,sizeof(long));
       ixx_lee(fd_ciu,(char *)&flag,sizeof(long));	   
       if (!strcmp(tmp_p,el_diu))
       {
           ixx_lseek(fd_ciu,(long)-((long)((sizeof(long)*2)+52)),SEEK_CUR);
           if (el_def)
           {
                memset(tmp_d,0,52);
                strcpy(tmp_d,el_def);
           }
           ixx_graba(fd_ciu,tmp_d,52);
           if (regs && regs != reg_s)
           {               
               ixx_graba(fd_ciu,(char *)&regs,sizeof(long));
           }
           else
           {
               ixx_graba(fd_ciu,(char *)&reg_s,sizeof(long));
           }
           flag = 1 + ixx_datos_dbf*10 + default_dbf_id*100;
           ixx_graba(fd_ciu,(char *)&flag,sizeof(long));
           return(0);
       }
	   else
	   {
		   if (!tmp_p[0])
		   {
			   hueco = i;
		   }
	   }
    }

	/* se espera (de momento asi) indefinidamente */	
    while(-1) 
    {
        ixx_lseek(fd_ciu,(long)sizeof(long),SEEK_SET);
        if (!ixx_lock(fd_ciu,LK_WAIT,1)) break;
        if (errno != EACCES && errno != EDEADLOCK && errno != EAGAIN  && errno != 13 && errno != 35)
        {
            break; /* glups!! */
        }
        if (ixxdat_micro_espera)
           (*ixxdat_micro_espera)();
    }

	if (hueco < 0)
	{
		ixx_lseek(fd_ciu,(long)(sizeof(long)*3),SEEK_SET);
		ixx_lee(fd_ciu,(char *)&total,sizeof(long));
		ixx_lseek(fd_ciu,(long)( (sizeof(long)*8) + (112 * total) ),SEEK_SET);
	}
	else
	{
		ixx_lseek(fd_ciu,(long)( (sizeof(long)*8) + (112 * hueco) ),SEEK_SET);
	}

    memset(tmp_p,0,52);
    strcpy(tmp_p,el_diu);
    ixx_graba(fd_ciu,tmp_p,52);
    memset(tmp_d,0,52);
    if (el_def)
       {                
       strcpy(tmp_d,el_def);
       }
    ixx_graba(fd_ciu,tmp_d,52);
    ixx_graba(fd_ciu,(char *)&regs,sizeof(long));
    flag = 1 + ixx_datos_dbf*10 + default_dbf_id*100;
    ixx_graba(fd_ciu,(char *)&flag,sizeof(long));

	if (hueco < 0)
	{
		total++;
		ixx_lseek(fd_ciu,(long)(sizeof(long)*3),SEEK_SET);
		ixx_graba(fd_ciu,(char *)&total,sizeof(long));
	}

    ixx_lseek(fd_ciu,(long)sizeof(long),SEEK_SET);
    ixx_lock(fd_ciu,LK_UNLCK,1);

    return(0);
}

int diu_abre(char *nom,char **descamp,char *defectos,short *longi,IXXFILE *ixxfic,int *ojo,FgvStdTabla *dtabla,char *pathdef)
{
    int i;
    int fd = (*ixxdiu_abre)(nom,descamp,defectos,longi,ixxfic,ojo,dtabla,pathdef);
    

    if (fd >= 0 && ixx_haydiu)
    {
        int ok = es_fichero_para_diu(nom);
        if (!fd_diu)
        {            
            fd_diu = (int *)malloc(sizeof(int)*max_fd_diu);
			modo_diu = (int *)malloc(sizeof(long)*max_fd_diu);
            for (i = 0;i < max_fd_diu;i++)
			{
                fd_diu[i] = -1;
				modo_diu[i] = 0;
			}
        }

        if ( (fd&~0x40000000) >= max_fd_diu)
        {
            i = max_fd_diu;
            max_fd_diu = (fd&~0x40000000) + 256; /* !! */
            fd_diu = (int *)realloc((char *)fd_diu,sizeof(int)*max_fd_diu);
			modo_diu = (int *)realloc((char *)modo_diu,sizeof(long)*max_fd_diu);
            for(;i < max_fd_diu;i++)
			{
                fd_diu[i] = -1;
				modo_diu[i] = 0;
			}
        }


        if (ok)
        {
            char tmp[256];
            /* abrir local diu */
            if (fd_diu[(fd&~0x40000000)] >= 0)
            {
                /* error o que? */
                ixx_cierra(fd_diu[(fd&~0x40000000)]);
            }
            sprintf(tmp,"%s.diu",nom);

            update_diu(0);
            fd_diu[(fd&~0x40000000)] = ixx_abre_dat(tmp);

            if (fd_diu[(fd&~0x40000000)] >= 0)
            {
                if (!ixx_lseek(fd_diu[(fd&~0x40000000)],0L,SEEK_END))
                {
                   ixx_cierra(fd_diu[(fd&~0x40000000)]);
                   ixx_unlink_(tmp);
                   fd_diu[(fd&~0x40000000)] = -1;
                }
            }
            else
            {
                if (dtabla && !strlen(dtabla->PathDef))
                {
                    ok = 0;
                    fd_diu[(fd&~0x40000000)] = -1;
                    update_diu(1); /* salir !!!!! */
                    return(fd);
                }
            }
            

            if (fd_diu[(fd&~0x40000000)] < 0)
            {                
                int lfd;
                lfd = ixx_crea_nuevo(tmp);
                if (lfd >= 0)
                {
                    crea_diu(lfd);
                    ixx_cierra(lfd);
                }
                fd_diu[(fd&~0x40000000)] = ixx_abre_dat(tmp);
            }

            if (fd_diu[(fd&~0x40000000)] >= 0)
            {
                long regs = 0L;
                char *el_def = NULL;
                if (dtabla)
                {
                    el_def = dtabla->PathDef;
                    regs = (long)(dtabla->reclen);
                }
				lee_modo_diu(fd);				
                update_ciu(nom,el_def,regs);
            }
            update_diu(1);

        }
        else
            fd_diu[(fd&~0x40000000)] = -1;
    }
    return(fd);
}

int diu_graba(int fd,long reg,Uchar *buf,int len,int flag)
{	
    int r = (*ixxdiu_graba)(fd,reg,buf,len);
    if (r >= 0 && ixx_haydiu && fd_diu && fd_diu[(fd&~0x40000000)] >= 0)
    {
		int m_diu = modo_diu[(fd&~0x40000000)];
        int _ant_errno = errno;
        char c = (char)flag;
        update_diu(0);
        if (!ixx_lseek(fd_diu[(fd&~0x40000000)],0L,SEEK_END))
        {
            crea_diu(fd_diu[(fd&~0x40000000)]);
			lee_modo_diu(fd);
        }
        ixx_graba(fd_diu[(fd&~0x40000000)],&c,1);
        ixx_graba(fd_diu[(fd&~0x40000000)],&reg,sizeof(reg));
        if (m_diu)
        {
            long hora = time(NULL);
            char usuario[20];
            ixx_graba(fd_diu[(fd&~0x40000000)],buf,len);
            ixx_graba(fd_diu[(fd&~0x40000000)],&hora,sizeof(long));
            memset(usuario,0,20);
            if (rw_usuario)
               strcpy(usuario,rw_usuario);
            ixx_graba(fd_diu[(fd&~0x40000000)],usuario,10);
            ixx_graba(fd_diu[(fd&~0x40000000)],&contador_global_diu,sizeof(long));
			if (m_diu > 1)
			{
				if (rw_modulo)
				{
					ixx_graba(fd_diu[(fd&~0x40000000)],rw_modulo,8);
				}
				else
				{
					ixx_graba(fd_diu[(fd&~0x40000000)],"xxxxxxxx",8);
				}
			}
        }
        update_diu(1);
        errno = _ant_errno;
    }
    return(r);
}

int diu_borra(int fd,long reg,int len)
{
    int r;

    char *buf = NULL;
	int m_diu;
    if (ixx_haydiu && fd_diu && fd_diu[(fd&~0x40000000)] >= 0)
    {
		m_diu = modo_diu[(fd&~0x40000000)];
		if (m_diu)
		{
			buf = malloc(len+1);
			memset(buf,0,len+1);
			(*ixxdat_lee)(fd,reg,buf,len);        
		}
    }

    r = (*ixxdiu_borra)(fd,reg,len);
    if (r >= 0 && ixx_haydiu && fd_diu && fd_diu[(fd&~0x40000000)] >= 0)
    {
        int _ant_errno = errno;
        char c = 1;
		m_diu = modo_diu[(fd&~0x40000000)];
        update_diu(0);
        if (!ixx_lseek(fd_diu[(fd&~0x40000000)],0L,SEEK_END))
        {
            crea_diu(fd_diu[(fd&~0x40000000)]);
			lee_modo_diu(fd);
        }
        ixx_graba(fd_diu[(fd&~0x40000000)],&c,1);
        ixx_graba(fd_diu[(fd&~0x40000000)],&reg,sizeof(reg));
        if (m_diu)
        {
            long hora = time(NULL);
            char usuario[20];
            ixx_graba(fd_diu[(fd&~0x40000000)],buf,len);
            ixx_graba(fd_diu[(fd&~0x40000000)],&hora,sizeof(long));
            memset(usuario,0,20);
            if (rw_usuario)
               strcpy(usuario,rw_usuario);
            ixx_graba(fd_diu[(fd&~0x40000000)],usuario,10);
            ixx_graba(fd_diu[(fd&~0x40000000)],&contador_global_diu,sizeof(long));
			if (m_diu > 1)
			{
				if (rw_modulo)
				{
					ixx_graba(fd_diu[(fd&~0x40000000)],rw_modulo,8);
				}
				else
				{
					ixx_graba(fd_diu[(fd&~0x40000000)],"xxxxxxxx",8);
				}
			}
        }
        update_diu(1);
        errno = _ant_errno;
    }
    if (buf) free(buf);
    return(r);
}

int diu_cierra(int fd)
{
    int r = (*ixxdiu_cierra)(fd);
    if (ixx_haydiu && fd_diu && fd_diu[(fd&~0x40000000)] >= 0)
    {
        int _ant_errno = errno;
        ixx_cierra(fd_diu[(fd&~0x40000000)]);
        fd_diu[(fd&~0x40000000)] = -1;
        errno = _ant_errno;
    }    
    return(r);
}

void  ixx_borra_diu(char *fichero)
{
    char tmp[256];
    if (ixx_haydiu)
    {
        int lfd;
        long reg = 0L;
        char c = 1;

        sprintf(tmp,"%s.diu",fichero);

        lfd = ixx_abre_dat(tmp);

        if (lfd >= 0)
        {
            update_diu(0);
            if (ixx_lseek(lfd,0L,SEEK_END))
            {
               ixx_graba(lfd,&c,1);
               ixx_graba(lfd,&reg,sizeof(reg));
            }
            update_diu(1);
            ixx_cierra(lfd);
        }        
    }
}

void  ixx_libera_diu()
{
    int i;

    if (fd_ciu >= 0)
    {
        ixx_cierra(fd_ciu);
        fd_ciu = -1;
    }

    if (inclusiones_diu)
    {
        for(i = 0;inclusiones_diu[i];i++)
            free(inclusiones_diu[i]);
        free((char *)inclusiones_diu);
        inclusiones_diu = NULL;
    }

    if (fd_diu)
    {
        int i;
        for(i = 0;i < max_fd_diu;i++)
        {
            if (fd_diu[i] >= 0)
                ixx_cierra(fd_diu[i]);
        }
        free((char *)fd_diu);
		free((char *)modo_diu);
        fd_diu = NULL;
		modo_diu = NULL;
    }
}


