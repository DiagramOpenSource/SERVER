

#include <stdio.h>


#define LPCTSTR char *
#define LPCSTR char *

static unsigned short n_c = 00000;
static unsigned short n_m = 00000;


char m_claveactivacion[1024];
char m_diagram[256];

char ficheroini[256];

int m_grabado = 0;
int	m_modo = 1;
char m_nombre[256] = "";
int m_orobloqueado = 0;
char m_oroclave[1024] = "";
char m_ororazon[1024] = "";
int m_orosopechoso = 0;
int		m_oroveces = 0;	
int m_tareas;
int m_puestostexto = 0;
int m_puestoswindows = 0;
int m_sistema = 0;	
int m_pastilla = 0;
char	m_fecha[256] = "";
int m_es9 = 0;
int m_naplicaciones = 0;

char m_clave_aplicaciones[1024];
char m_nom_aplicaciones[256][256];
int m_id_aplicaciones[256];
int m_ver_aplicaciones[256];
int m_pues_aplicaciones[256]; 
int m_tar_aplicaciones[256]; 
extern double atof();

unsigned long claveoro = 0;

char aplicacion[300];
int id = 0;
int ver = 0;
int pues = 0;
int tar = 0;

int m_oro = 0;

static struct parametros_iniciales
{
   char *nombre;
   void **dato;
   int tipo; /* 0 = char * inicializado a NULL 1 = int 2 = long 3 = double  */
   int linea;
} agi_init[] =
{
   {"Aplicacion",&(aplicacion[0]),4,0},
   {"Id",&id,1,0},
   {"Ver",&ver,1,0},
   {"Puestos",&pues,1,0},
   {"Tareas",&tar,1,0},
   {"Cliente",&(m_diagram[0]),4,0},
   {"Nombre",&(m_nombre[0]),4,0},
   {"Fecha",&(m_fecha[0]),4,0},
   {"Pastilla",&m_pastilla,1,0},
   {"Sistema",&m_sistema,1,0},
   {"TotalTareas",&m_tareas,1,0},
   {"Es9",&m_es9,1,0},
   {"PuestosTexto",&m_puestostexto,1,0},
   {"PuestosWindows",&m_puestoswindows,1,0},
   {"ClaveBasica",&m_oro,1,0},
   {NULL,NULL,0,0},   
    };	

char *nombreletra[] =
	{
	"Cero",
	"Uno",
	"Dos",
	"Tres",
	"Cuatro",
	"Cinco",
	"Seis",
	"Siete",
	"Ocho",
	"Nueve",
	"Alemania",
	"Barcelona",
	"Cadiz",
	"Dinamarca",
	"Espa¤a",
	"Francia",
	"Gerona",
	"Huelva",
	"Italia",
	"Jaen",
	"Kilo",
	"Lerida",
	"Madrid",
	"Navarra",
	"Oviedo",
	"Pamplona",
	"Queso",
	"Roma",
	"Salamanca",
	"Teruel",
	"Universidad",
	"Valencia",
	"Washington",
	"Xilofon",
	"Yolanda",
	"Zaragoza",
	"GUION",
	"PUNTO",
	};


static int cambia_a_letra(buf,len)
unsigned char *buf;
int len;
{
int i;

for (i = 0;i < len;i++)
  {
  if (buf[i] == 63)
    {
    buf[i] = '.';
    }
  else
  if (buf[i] == 62)
    {
    buf[i] = '-';
    }
  else
  if (buf[i] < 26)
    {
    buf[i] = 'A' + buf[i];
    }
  else
  if (buf[i] < 36)
    {
    buf[i] = 22 + buf[i];
    }
  else
    {
    buf[i] = 61 + buf[i];
	}
  }
buf[i] = 0;
return(i);
}

void quitab(s)
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

/*
* quitabp
*    quitar los blancos de principio del string
*/
void quitabp(s)
char *s;
{
   register char *c;
   
   for (c=s;(*c == ' ' || *c == 9);c++);
   while ((*s++=*c++) != '\0');
}

/*
* quitan
*    quitar los \n de final de string
*/
void quitan(s)
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
         
}

    static void pon_dato_en_ini(j,tmp,salva)
       int j;
    char *tmp;
	int salva;
    {
       char *punt;

       switch(agi_init[j].tipo)
       {
       case 0:
		   /*
          punt =(char *)*agi_init[j].dato;		 
			  if (punt)
				 free(punt);
				 */
          punt = malloc(strlen(tmp)+1);
          strcpy(punt,tmp);
          *agi_init[j].dato = (void *)punt;
          break;
       case 1:
          *((int *)agi_init[j].dato) = atoi(tmp);
          break;
       case 2:
          *((long *)agi_init[j].dato) = atol(tmp);
          break;
       case 3:
          *((double *)agi_init[j].dato) = atof(tmp);
          break;
	   case 4:
		  strcpy((char *)agi_init[j].dato,tmp);
		  break;
       default:
          break;
       }
       if (agi_init[j].linea == -1)
       {
          agi_init[j].linea = -2;
       }
    }

static int lee_params(char *params)
			 {
       char nom[3000];
       char comenta[3000];
       FILE *file;
       int i,j,e,k;
       int linea;
       char *punt;
       

       for (j = 0;agi_init[j].nombre;j++)
       {
          agi_init[j].linea = -1;
       }

       strcpy(nom,params);
       
       file = fopen(nom,"rt");
       
       
       if (!file) 
       {
          return(-1); 
       }
       
       
       linea = 0;
       while(fgets(nom,2990,file))
       {
          linea++;
          quitan(nom);
          quitab(nom);
          quitabp(nom);
          if (!nom[0] || nom[0] == ';')
             continue;
          for (i = 0;nom[i] && nom[i] != '=' && nom[i] != ';';i++);
          if (nom[i] != '=')
          {
			  fclose(file);
			  return -1;
          }
          nom[i] = 0;
          i++;
          comenta[0] = 0;
          for (j = i;nom[j];j++)
          {
			 if (nom[j] == '"')
			 {
				 j++;
				 while(nom[j] && nom[j] != '"') j++;
				 if (!nom[j]) break;
			 }
             if (nom[j] == ';')
             {
                strcpy(comenta,nom+j+1);
                nom[j] = 0;
                break;
             }
          }
          quitabp(nom);
          quitab(nom);
          quitabp(nom+i);
          quitab(nom+i);
          for (j = 0;agi_init[j].nombre;j++)
          {
             if (!strcmp(agi_init[j].nombre,nom))
             {
                agi_init[j].linea = linea;
                pon_dato_en_ini(j,nom+i,0);
				if (!j) {
					e = m_naplicaciones;
					m_naplicaciones++;
					strcpy(m_nom_aplicaciones[e],aplicacion);
					m_id_aplicaciones[e] = id;
					m_ver_aplicaciones[e] = ver;
					m_pues_aplicaciones[e] = pues; 
					m_tar_aplicaciones[e] = tar; 
				}
				else if (j < 5) {
					e = m_naplicaciones - 1;
					if (e < 0)
						e = 0;
					switch(j) {
					case 1:
						m_id_aplicaciones[e] = id;
						break;
					case 2:
						m_ver_aplicaciones[e] = ver;
						break;
					case 3:
						m_pues_aplicaciones[e] = pues; 
						break;
					case 4:
						m_tar_aplicaciones[e] = tar; 
						break;
					default:
						break;
					}
				}
                break;
             }
          }
       }
       fclose(file);
	   return 0;
}

int en_new_code(unsigned char *_buf,unsigned short mascara,unsigned int finlen)
{
unsigned long bloque[10];
unsigned long dato;
unsigned char final[100];
unsigned char buf[100];
int nbloque,quebloque,quepunto;
int elpunto,i,j,ultimobit,len,r,q;
unsigned char t,t1;
union mibloque
  {
  unsigned long b;
  unsigned char c[4];
  } mib;

if (finlen < 1 || finlen >= 41)
  {
  return(-1);
  }

memcpy(buf,_buf,finlen);

nbloque = finlen / 5;

quebloque = 0;
elpunto = 0;
bloque[quebloque] = 0L;
for (len = 0;len < (nbloque*5);len++)
  {
  dato = buf[len];
  for (i = 0;i < elpunto;i++)
    {
	dato *= 50;
    }
  bloque[quebloque] += dato;  
  elpunto++;
  if (elpunto >= 5)
    {
	elpunto = 0;
	quebloque++;
	bloque[quebloque] = 0L;
    }
  }


elpunto = 0;
buf[elpunto] = 0;
for (i = 0;i < nbloque;i++)
  {
  mib.b = bloque[i];
  for (j = 0;j < 3;j++)
    {
	buf[elpunto] = mib.c[j] ^ finlen ^ mascara;
	elpunto++;
	buf[elpunto] = 0;
    }
  }

t1 = 0;
for (i = 0;i < (nbloque-(nbloque % 2));i++)
  {
  mib.b = bloque[i];
  if ( (i%2) )
    {
	t = (mib.c[3]) & 0xf;
	t <<= 4;
	buf[elpunto] |= t;
	elpunto++;
	buf[elpunto] = 0;
    }
  else
    {
	buf[elpunto] = (mib.c[3]) & 0xf;
    }
  t = mib.c[3] & 0x10;
  t >>= 4;
  if (i)
    t <<= i;
  t1 |= t;
  }
buf[elpunto] = t1;
ultimobit = i;
j = nbloque % 2;
if (j)
  {
  mib.b = bloque[nbloque-1];
  t1 = mib.c[3];
  quepunto = 0;
  for (i = nbloque;i < 8 && quepunto < 5;i++)
    {    
	t = t1;
    t >>= quepunto;
	t &= 1;
	t <<= (i-1);
	buf[elpunto] |= t;
	quepunto++;	
    }  
  if (quepunto < 5)
    {
    elpunto++;
	buf[elpunto] = 0;
	for (i = 0;quepunto < 5;quepunto++)
	  {
	  t = t1;
      t >>= quepunto;
	  t &= 1;
	  if (i)
	    t <<= i;
	  buf[elpunto] |= t;
	  i++;
	  }	
    }
  ultimobit = (i-1)%8;
  ultimobit++;
  }


if ((finlen%5)) 
  {
  r = finlen - (finlen%5);
  q = (finlen%5) * 8;  
  i = ultimobit;
  for (j = 0;j < q;j++,i++)
    {
	if (j && !(j%8))
	  {
	  r++;
	  }
	if (i && !(i%8))
	  {
	  elpunto++;
	  buf[elpunto] = 0;
	  }
    t = _buf[r] ^ finlen ^ mascara;;
    if ((j%8))
      {
      t = t >> (j%8);
      }
    t &= 1;
    if ((i%8))
	  {
	  t <<= (i%8);
	  }
	buf[elpunto] |= t;
    }
  ultimobit = (i-1)%8;
  ultimobit++;
  if (elpunto && !buf[elpunto])
    {
    ultimobit = 8;
    while(elpunto && !buf[elpunto]) elpunto--;
    }
  }


i = ultimobit;
t1 = mascara;
for (j = 0;j < 6;j++)
  {
  if (!(i%8))
    {
	elpunto++;
    buf[elpunto] = 0;
    }
  t = t1;
  if (j)
    {
    t >>= j;
    }
  t &= 1;
  if ((i%8))
    {
    t <<= (i%8);
    }
  buf[elpunto] |= t;
  i++;
  }
ultimobit = (i-1) % 8;
ultimobit++;

q = elpunto + 1;
t = 0;
for (j = 0;j < q;j++)
  {
  t ^= buf[j];
  }

t1 = t & 0xf;
t >>= 4;
t1 ^= (t & 0xf);
t1 |= 16;
i = ultimobit;
for (j = 0;j < 5;j++)
  {
  if (!(i%8))
    {
	elpunto++;
    buf[elpunto] = 0;
    }
  t = t1;
  if (j)
    {
    t >>= j;
    }
  t &= 1;
  if ((i%8))
    {
    t <<= (i%8);
    }
  buf[elpunto] |= t;
  i++;
  }


ultimobit = (i-1) % 8;
ultimobit++;
quepunto = 0;
final[quepunto] = 0;
elpunto *= 8;
elpunto += ultimobit;
i = 0;
for (j = 0;j < elpunto;j++)
  {
  if (j && !(j%8))
    {
    i++;
    }
  if (j && !(j%6))
    {
	quepunto++;
	final[quepunto] = 0;
    }
  t = buf[i];
  if ((j%8))
    {
    t = t >> (j%8);
    }
  t &= 1;

  if ( (j%6) )
    {
    t <<= (j%6);
    }
  final[quepunto] |= t;
  }
quepunto++;

cambia_a_letra(final,quepunto);
strcpy(_buf,final);
return(quepunto);
}


unsigned long buen_oro_(long nume_serie,int posicion,unsigned long NumeroDeSerie)
{
unsigned long filtro;
unsigned char buf[100];

sprintf(buf,"%06lu",NumeroDeSerie);
buf[6] = 0;
buf[0] ^= posicion;
buf[1] ^= nume_serie;
buf[2] ^= posicion;
buf[3] ^= nume_serie;
buf[4] ^= posicion;
buf[5] ^= nume_serie;
filtro = buf[0]*1000 + buf[1]*100 + buf[2]*10 + buf[3]*6 + buf[4]*2 + buf[5];
filtro ^= nume_serie;

return(filtro);
}


int clave_activacion(unsigned char *buf,unsigned long  nume_serie,unsigned short posicion,unsigned short sistema1,unsigned short accesos,unsigned short servidor1,unsigned short servidor2)
{
unsigned short nume_code;
unsigned short nume_mochila;
unsigned short checo;

accesos += 199;

nume_code    = n_c;
nume_mochila = n_m;

buf[3]  = sistema1;
buf[1]  = accesos % 50;
buf[2]  = (accesos / 50) % 50;
buf[0]  = (unsigned char)(long)(nume_serie%50);
buf[4]  = (unsigned char)(long)((nume_serie / 50) % 50);
buf[5]  = (unsigned char)(long)((nume_serie / (50*50)) % 50);
buf[6]  = (unsigned char)(long)(nume_serie / (50L*50L*50L));
buf[10] = posicion%50;
buf[8]  = (posicion / 50) % 50;
buf[9]  = servidor1;
buf[7]  = servidor2;
checo   = (buf[3] ^ buf[1] ^ buf[2] ^ buf[9] ^ buf[7]) % 50;
buf[11] = checo;
return(en_new_code(buf,((posicion%50) ^ (posicion/50)) % 50,12));
}

int clave_paquete(unsigned char *buf,unsigned long  nume_serie,unsigned short posicion,unsigned short paquete,unsigned short version,unsigned short puestos,unsigned short tareas)
{
unsigned short nume_code;
unsigned short nume_mochila;
unsigned short numeropa;
  
  nume_code    = n_c;
  nume_mochila = n_m;
  numeropa = puestos * 256 + tareas;
  paquete = paquete ^ nume_code ^ (short)(nume_serie & 0xffff);
  version = version ^ nume_code ^ (short)(nume_serie & 0xffff);
  buf[5] = paquete%50;
  buf[1] = (paquete / 50) % 50;
  buf[3] = (paquete / (50*50));
  buf[2] = version%50;
  buf[4] = (version / 50) % 50;
  buf[0] = (version / (50*50));
  buf[8] = numeropa%50;
  buf[7] = (numeropa / 50) % 50;
  buf[6] = (numeropa / (50*50));
  return(en_new_code(buf,((posicion%50) ^ (posicion/50))%50,9));

}


void CalculaActivacion()
{

         unsigned long nume_serie;
         unsigned short sistema;
		 unsigned short p1,p2;
		 char palabra[1024];

         if (!m_modo)
            {  
            p1 = 0;
            p2 = 0;
            }
		 else
		    {
            p1 = (unsigned short)m_puestostexto;
            p2 = (unsigned short)m_puestoswindows;
			}
		 
         nume_serie = (unsigned long)atol(m_diagram);



		 switch(m_sistema)
		    {
			case 0:
			  if (m_es9)
			     sistema = 10;
			  else
			     sistema = 24;
			  break;
			case 1:
			  sistema = 13;
			  break;
			case 2:
 			  if (m_es9)
			     sistema = 9;
			  else
			     sistema = 11;
			  break;
			case 3:
			  if (m_es9)
			     sistema = 3;
			  else
			     sistema = 7;
			  break;
			case 4:
			  if (m_es9)
			     sistema = 6;
			  else
			     sistema = 23;
			  break;
			default:
			  sistema = 0;
			  break;
			}

		 clave_activacion((unsigned char *)palabra,
		                    (unsigned long)nume_serie,
							(unsigned short)m_pastilla,
							(unsigned short)sistema,
							(unsigned short)m_tareas,
							(unsigned short)p1,
							(unsigned short)p2);
		 strcpy(m_claveactivacion,palabra);		 	 		 

		 if (m_oro) {
			claveoro = buen_oro_((long)nume_serie,(int)m_pastilla,(unsigned long)m_oro);
		 }

}

int pinta_nombre_letras(char *buf,char *buffer)
{
int i;
int valor;
int origen;

buffer[0] = 0;

for (i = 0;buf[i];i++)
  {
  if (i)
	  sprintf(buffer+strlen(buffer),",");
  origen = 0;
  if (buf[i] == '-')
      {
	  valor = 36;
      }
  else
  if (buf[i] == '.')
      {
	  valor = 37;
      }
  else  
  if (buf[i] >= '0' && buf[i] <= '9')
	  {
	  valor = buf[i] - 48;
	  }
  else
  if (buf[i] >= 'A' && buf[i] <= 'Z')
	  {
	  valor = buf[i] - 55;
	  origen = 1;
	  }
  else
  if (buf[i] >= 'a' && buf[i] <= 'z')
	  {
	  valor = buf[i] - 87;
	  origen = 1;
	  }
  else
	  continue;
  if (!origen)
	 {
	 sprintf(buffer+strlen(buffer),"%c",nombreletra[valor][0]);
	 }
  else
	 {
	 sprintf(buffer+strlen(buffer),"%c",buf[i]);
	 }
  sprintf(buffer+strlen(buffer),"%s",nombreletra[valor]+1);
  }
return(0);
}

void GrabaIni()
{
    FILE *f;
	char ini[1024];
	char s[2048];
	char tmp[1024];
	int i,total;


    if (!m_claveactivacion[0]) 
      {
	  return;
	  }

    if (!m_pastilla)
	  {
	  return;
	  }

    sprintf(ini,"%s.ini",(LPCTSTR)ficheroini);


    f = fopen(ini,"wt");
	if (!f)
	   {
	   fprintf("No puedo grabar %s",ini);
	   return;
	   }
	if (claveoro) {
		fprintf(f,";ORO:%ld\n",claveoro);
	}
	fprintf(f,";FECHA:%s\n",m_fecha);
    fprintf(f,";CLIENTE:%s\n",m_nombre);
    fprintf(f,";SERIE:%s-%d\n",m_diagram,m_pastilla);
	fprintf(f,";TAREAS:%d\n",m_tareas);
	fprintf(f,";PUESTOS T-W:%d-%d\n",m_puestostexto,m_puestoswindows);
	if (m_sistema == 3 || m_sistema == 4)
	{
		fprintf(f,";SISTEMA BSD\n");	
	}
	else
	{
		fprintf(f,";SISTEMA WINDOWS\n");	
	}	
	//fprintf(f,";VECES:%d\n",m_oroveces);
	//fprintf(f,";GRABADO:%d\n",m_grabado);
	//fprintf(f,";SOSPECHOSO:%d\n",m_orosopechoso);
	//fprintf(f,";BLOQUEADO:%d\n",m_orobloqueado);
	//fprintf(f,";CLAVEORO:%s\n",m_oroclave);
	//fprintf(f,";RAZON:%s\n",m_ororazon);
    fprintf(f,"Activacion=%s\n",m_claveactivacion);
	pinta_nombre_letras((char *)(LPCSTR)m_claveactivacion,s);
	fprintf(f,";%s\n",s);
	total = m_naplicaciones;
	for (i = 0;i < total;i++)
	   {
       clave_paquete((unsigned char *)m_clave_aplicaciones,atol(m_diagram),(unsigned short)m_pastilla,
	                                 (unsigned short)m_id_aplicaciones[i],
									 (unsigned short)m_ver_aplicaciones[i] ,
									 (unsigned short)m_pues_aplicaciones[i] ,
									 (unsigned short)m_tar_aplicaciones[i] );

	   if (m_ver_aplicaciones[i] == 19999)
	      {
		  if (m_id_aplicaciones[i] < 2)
		     fprintf(f,"PaqDesde=%s\n",m_clave_aplicaciones);
		  else
		     fprintf(f,"PaqHasta=%s\n",m_clave_aplicaciones);
		  }
	   else
          fprintf(f,"InsPaq%02d=%s ; %s\n",i+1,m_clave_aplicaciones,m_nom_aplicaciones[i]);
	   pinta_nombre_letras((char *)(LPCSTR)m_clave_aplicaciones,s);
	   fprintf(f,";%s\n",s);
	   }

    fclose(f);
}

/*
BOOL CMasterdsDlg::CargaIni(LPCSTR ini)
{
FILE *f;


f = ::fopen(ini,"rt");

if (!f) return FALSE;


m_fecha = CTime(::time(NULL)).Format( "%d/%m/%Y" );
m_oroveces = 0;
m_grabado = FALSE;
m_orosopechoso= FALSE;
m_orobloqueado = FALSE;
m_oro = "";
m_oroclave = "";
m_ororazon = "";
UpdateData(FALSE);
CString r;
while(::fgets(r.GetBufferSetLength(1024),1024,f))
   {
   r.ReleaseBuffer(-1);
   r.TrimRight('\n');
   r.TrimRight('\r');
   r.TrimRight('\n');   
   r.TrimRight();
   int pcoma = r.Find(';');
   // buscar datos cliente y fecha
   int item = r.Find("CLIENTE");
   if (item > -1)
 	     {
         m_nombre = r.Mid(item+8);		 
		 }        
   item = r.Find("FECHA");
   if (item > -1)
 	     {
         m_fecha = r.Mid(item+6);		 
		 }
	item = r.Find(";VECES:");
	if (item > -1)
	   {
	   m_oroveces = atoi(r.Mid(item+7));
	   }
	item = r.Find(";GRABADO:");
	if (item > -1)
	   {
	   m_grabado = (BOOL)atoi(r.Mid(item+9));
	   }
	item = r.Find(";SOSPECHOSO:");
	if (item > -1)
	   {
	   m_orosopechoso = (BOOL)atoi(r.Mid(item+12));
	   }
	item = r.Find(";BLOQUEADO:");
	if (item > -1)
	   {
	   m_orobloqueado = (BOOL)atoi(r.Mid(item+11));
	   }
	item = r.Find(";ORO:");
	if (item > -1)
	   {
	   m_oro = r.Mid(item+5);
	   }
	item = r.Find(";CLAVEORO:");
	if (item > -1)
	   {
	   m_oroclave = r.Mid(item+10);
	   }
	item = r.Find(";RAZON:");
	if (item > -1)
	   {
	   m_ororazon = r.Mid(item+7);
	   } 

   int igual = r.Find('=');
   if (pcoma == -1)
      {
	  pcoma = r.Find('\'');
	  }
   if (igual > -1 && (pcoma == -1 || igual < pcoma))
      {
	  if (pcoma == -1) 
	     pcoma = r.GetLength() - 1;
	  pcoma -= igual;
	  CString palabra = r.Mid(0,igual);
	  palabra.TrimLeft();
	  palabra.TrimRight();
	  if (!palabra.Compare("Activacion"))
	     {
		 palabra = r.Mid(igual+1,pcoma);
	     palabra.TrimLeft();
	     palabra.TrimRight();
	     int x = palabra.Find(' ');
	     if (x > -1)
	       {
		   palabra.SetAt(x,0);
		   }	  
		 m_claveactivacion = palabra;
         unsigned long nume_serie;
         unsigned short sistema;
		 ::datos_activacion((unsigned char *)palabra.GetBufferSetLength(1024),
		                    (unsigned long *)&nume_serie,
							(unsigned short *)&m_pastilla,
							(unsigned short *)&sistema,
							(unsigned short *)&m_tareas,
							(unsigned short *)&m_puestostexto,
							(unsigned short *)&m_puestoswindows);
		 palabra.ReleaseBuffer(-1);
		 
		 m_diagram.Format("%06ld",nume_serie);
         m_es9 = FALSE;
		 switch(sistema)
		    {
			case 10:
			  m_es9 = TRUE;
			case 24:			
			  m_sistema = 0;
			  break;
			case 13:
			  m_sistema = 1;
			  break;
			case 9:
			  m_es9 = TRUE;
			case 11:			  
			  m_sistema = 2;
			  break;
			case 3:
			  m_es9 = TRUE;
			case 7:
			  m_sistema = 3;
			  break;
			case 6:
			  m_es9 = TRUE;
			case 23:
			  m_sistema = 4;
			  break;
			default:
			  m_sistema = -1;
			  break;
			}
		 if (m_puestostexto || m_puestoswindows)		    
			m_modo = 1;
		 else
		    m_modo = 0;
		 UpdateData(FALSE);
		 }
	  else
	  if (  (palabra.Compare("InsPaq00") > 0 && palabra.Compare("InsPaq99") < 0) 
	        || !palabra.Compare("PaqDesde")
			|| !palabra.Compare("PaqHasta")
		 )
	     {
		 CString tmp,tmp2;
		 palabra = r.Mid(igual+1,pcoma);
	     palabra.TrimLeft();
	     palabra.TrimRight();
	     int x = palabra.Find(' ');
	     if (x > -1)
	       {
		   palabra.SetAt(x,0);
		   }	  
         unsigned long nume_serie = (unsigned long)atol(m_diagram);	     
		 unsigned short paquete;
		 unsigned short version;
		 unsigned short puestos;
		 unsigned short tareas;
		 tmp2 = palabra;
		 ::datos_paquete((unsigned char *)palabra.GetBufferSetLength(1024),
		                 (unsigned long)nume_serie,						  
						  (unsigned short *)&paquete,
						  (unsigned short *)&version,
						  (unsigned short *)&puestos,
						  (unsigned short *)&tareas);
		 palabra.ReleaseBuffer(-1);
         int i,j = -1;
		 for (i = 0;defecto[i].codigo[0];i++)
		    {
			if (defecto[i].numero == (int)paquete && defecto[i].version == (int)version)
			   j = i;
			else
			if (j == -1 && defecto[i].numero == (int)paquete)
			   j = i;
			}
		 if (j == -1) j = i;
		 tmp.Format("%s\t%d\t%d\t%d\t%d\t%s",defecto[j].codigo,paquete,version,puestos,tareas,tmp2);
         m_aplicaciones.AddString(tmp);		 
		 UpdateData(FALSE);
		 }
	  }
   }
r.ReleaseBuffer(-1);
::fclose(f);

return TRUE;
}
*/


int main(int argc, char *argv[]) {
	if (argc != 3) {
		printf("Uso: masterdsu fichero_parametros  fichero_ini\r\n");
		return 1;
	}
	strcpy(ficheroini,argv[2]);
	lee_params(argv[1]);
	 if (m_puestostexto || m_puestoswindows)		    
		m_modo = 1;
	 else
		m_modo = 0;
	CalculaActivacion();
	GrabaIni();

	return 0;
}


