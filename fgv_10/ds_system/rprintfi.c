
/*---------------------------------------------------------------------------
 * Programa ..: rprintfi.c
 * Fecha .....:
 * Version ...:
 * Notas .....: Rutina para campos y pantallas de datos de un fichero
 * Contenido .: int  printcam, printusi(), printusI(), printpan(), 
 *---------------------------------------------------------------------------*/

#include <fgvsys.h>

#include <stdio.h>

#include <rtermin.h>
#define _NUEVODEF
#include <rdef.h>      /* ya incluye el isam.h */

extern int rentra_atrib;
extern int rentra_sal_atrib;
extern int rentra_cc;
extern int rentra_cf;
extern int rentra_sal_cc;
extern int rentra_sal_cf;

extern char *cambia_path_fich();

extern char *poncoma(); /* Se podria incluir aqui */

extern int (*asigna_euro)();

extern int fgv_ver_euro;
extern int modo_otro_euro;

extern int _PCcolor;
extern int _PFcolor;


int es_euro(char *nom) /* invertido */
{
	if (strlen(nom) < 4) return -1;
    return memcmp(nom,"EURO",4);
}

int decimales_variables(char *min)
{
	int i = strlen(min);
	if (i > 1 && min[i-2] == '.' && min[i-1] == '*')
		return 1;
	if (min[i-2] == 'O' && min[i-1] == '*')
	{
		while(*min && *min != '?')
			min++;
		if (*min == '?')
			min++;
		if (!es_euro(min))
		{
			return 1;
		}
	}
	return 0;
}


/* a_atri_campo : Activa un atributo especial asignado al campo */
int a_atri_campo(pfi,campo,oa,m,occ,off)
FI_DEF *pfi;
int campo,oa,m,occ,off;
{
   short f = pfi->f_campos[campo].c_fflag;
   int at = *(((char *)&(f)) + 1);
   int cc = (*(((char *)&(f))) & 0x0f);
   int ff = ((*(((char *)&(f))) & 0x70) >> 4);
   
   if (at != 'D' && at != 'N' && !es_atri(at))
      at = 'D';
   if (at == 'D') 
   {
      if (oa == ' ') 
	  {
         at = rentra_sal_atrib;
         cc = rentra_sal_cc;
         ff = rentra_sal_cf;
      }
      else
         return(0);
   }
   
   if (m) 
   {
      atributo(tolower(at));
      if (oa != ' ' || occ != off) 
	  {
         if (occ != off)
            Color(occ,off);
         atributo(oa);
      }
      return(0);
   }
   
   atributo(tolower(oa));
   if (cc != ff)
      Color(cc,ff);
   atributo(at);
   
   return(0);
}


int formato_campo(FI_DEF *pfi,int campo,char *qo)
{
   char *buf,*usi,*min,usi_2[20];
   char qotmp[MAXDEFECTO];
   int tipo,offset,longit,i,j,kkk,l;
   double numd;
   int permi;
   int r = 0;
   int ancho;

       permi = hay_dat_permiso(pfi,campo);
	   if (!(pfi->f_campos[campo].c_mante.c_lineal& FGV_C_GRID))
	   {
		   i = pfi->f_campos[campo].c_mante.c_qpl;
		   ancho = i;
	   }
	   else
	   {
		   ancho = 250;
		   i = 250;
	   }

	   modo_otro_euro++;
       usi = recalcula_usi(pfi,campo);
	   modo_otro_euro--;

       offset = pfi->f_campos[campo].c_offset;
       longit = pfi->f_campos[campo].c_long;
       tipo = pfi->f_campos[campo].c_tipo;
       min = pfi->f_campos[campo].c_minimo;
       buf = pfi->f_buf;
   
       qo[0] = 0;
   
       switch ( tipo ) 
       {
			    case CHARTIPO :
				if (permi) {
				   ldchar(buf+offset,longit,qo);
				}
                //if (i > longit) i = longit;
                //qo[i] = 0;
                /* se supone que el usi del char es caracteres a la izquierda + espacios */
                //for (j = strlen(qo);j < i;j++)
				for (j = strlen(qo);j < longit;j++)
                   qo[j] = ' ';
                qo[j] = 0;
                break;
             case FECHATIPO :		                 
				kkk = 0;
				ldfecha(buf+offset,qotmp);
                if (!atoi(qotmp+6))
				{
                      memset(qotmp,0,10);
					  kkk = 1;
				}

				if (!kkk)
				{
				   if (i == 6)
					{
					   ldfecha6(buf+offset,qotmp);					   
					}
				   else if (i == 8)
				   {
					  ldfecha8(buf+offset,qotmp);					  
				   }
				   else
				   {
					  ldfecha(buf+offset,qotmp);					  
				   }
				}
                   if (i == 6)
                      sprintf(qo,"%-6.6s",qotmp);
                   else
                      if (i == 8)
                         sprintf(qo,"%-8.8s",qotmp);
                      else
                         sprintf(qo,usi,qotmp);
                      break;
             case INTTIPO :
                sprintf(qo,usi,ldint(buf+offset));
                if ((*min == ' ' && !atoi(qo)) || !permi)
                   memset(qo,' ',strlen(qo));
                else
				{
				   if (ancho == 1)
				   {
					    poncoma(qo,NULL);
				   }
				   else
				   {
						poncoma(qo,qo);
				   }
				}
                break;
             case LONGTIPO :
                 /* ojo desde GRID!!!*/
                if (permi && pfi->f_campos[campo].c_qc == -2 && !(pfi->f_campos[campo].c_mante.c_lineal& C_LINEAL) && !(pfi->f_campos[campo].c_mante.c_lineal& FGV_C_GRID))
                {
                   i=atoi(pfi->f_campos[campo].c_minimo);
                   strcpy(qo,pfi->f_campos[campo].c_mante.c_defecto);
                   trata_memo(cambia_path_fich(qo),
                      ldlong(buf+offset),
                      3,i,
                      i + atoi(pfi->f_campos[campo].c_maximo) - 101,
                      (pfi->f_campos[campo].c_mante.c_lineal& C_LINEAL),
                      pfi->f_campos[campo].c_mante.c_qp,
                      "",pfi->f_campos[campo].c_mante.c_qpl,NULL,pfi->f_campos[campo].c_fflag);
                   r = -1;
                }
                break;
             case DOUBLETIPO :
                numd = lddbl(buf+offset);               

				if (fgv_ver_euro)
				{
					l = 0;
					for (i = 0;min[i];i++)
					{
					   if (min[i] == '.') l++;
					   if (l && (min[i] == '?' || min[i] == '!')) {break;}
					}
					if (min[i]) 
					{
						i++;
						while(min[i] == '?') i++;
					    if (asigna_euro)
						{							 
							 (*asigna_euro)(min+i,&numd);
							 if ( (Patrib & 128) && !es_euro(min+i))
							 {								 
								 _PFcolor = 0;
								 _PCcolor = 0;
								 atributo('S');
								 Patrib |= 128;
							 }
						}
					}
				}

				/*i = strlen(min);*/


                /* SISTEMA DE DECIMALES VARIABLES */
                if (decimales_variables(min)) 
				{
                   j = len_numerico(min,pfi->f_campos[campo].c_maximo);
                   sprintf(qo,"%lf",numd);
                   quitadec(qo); /* siempre hay decimales (.0000) */
                   sprintf(usi_2,"%%%d.%dlf",j,decimales(qo));
                   sprintf(qo,usi_2,numd);
                   if (strlen(qo) > j)
                   {
                      i = decimales(qo) - (strlen(qo) - j);
                      if (i < 0) i = 0;
                      sprintf(usi_2,"%%%d.%dlf",j,i);
                      sprintf(qo,usi_2,numd);
                   }
                }
                else
                   sprintf(qo,usi,numd);
                if ((*min == ' ' && numd == 0.0) || !permi) {
                   memset(qo,' ',strlen(qo));
                }
                else
				{
				   if (ancho == 1)
				   {
					    poncoma(qo,NULL);
				   }
				   else
				   {
						poncoma(qo,qo);
				   }
                }                
                break;
             default:
                break;
        }
return r;
}


/*
* printcam
* pinta el contenido de un campo (a partir del 1)
* lo hace sin posicionar el cursor; es decir, previamente se hace cursor(qp)
*/
printcam_x(pfi,campo,posicional)
FI_DEF *pfi;
int campo;
int posicional;
{
   char qo[MAXDEFECTO];
   
   campo--;
   
   Patrib |= 128;
   
   if ( (pfi->f_campos[campo].c_mante.c_lineal & FGV_C_GRID)  && pfi->f_idgrafico > -1 && pfi->f_campogids && pfi->f_campogids[campo] != 0xffffffff)
   {
       if (formato_campo(pfi,campo,qo) >= 0)
       {
            char tmp[400];
            long modo = 0; /* OJO !!!! */
            long tam = (sizeof(long)*3)+(sizeof(int)*2)+strlen(qo)+2;
            int id_grid = pfi->f_idgrafico;                

            memcpy(tmp,&tam,sizeof(long));
            memcpy(tmp+sizeof(long),&tam,sizeof(long));
            memcpy(tmp+(sizeof(long)*2),&modo,sizeof(long));
            memcpy(tmp+(sizeof(long)*3),&id_grid,sizeof(int));
            memcpy(tmp+(sizeof(long)*3)+sizeof(int),&campo,sizeof(int));
            strcpy(tmp+(sizeof(long)*3)+sizeof(int)*2,qo);            
            (*DespachadorExterno)((int)DESPACHO_BINARIO,(int)2,tmp);

          /* pfi->f_idgrafico == id_grid */
          /* pfi->f_campogids[campo] == columna_campo */
          /* a rellenar ... */
          /* de momento no se pintan desde aqui hasta que el control pinte UN SOLO CAMPO (para evitar recusrsiones infinitas)*/          
       }       
   }
   else
   {      
       if (!posicional || (posicional == 1 && pfi->f_campos[campo].c_mante.c_qp == Pcursor))
       {		   
           entra_cedit(pfi,campo,-1,0,pfi->f_ipfi,0,NULL,0,-1,-1,0);
       }
       else
       {
           /* auto faclin para casos no filtrados (p.ej el pinta con posicion desde procesos) */
           int px = (Pcursor%100)-(pfi->f_campos[campo].c_mante.c_qp%100);
           int py = (Pcursor/100)-(pfi->f_campos[campo].c_mante.c_qp/100);
           if (px || posicional == 2 || posicional == 3)
           {
               px = Pcursor;
           }
           else
           {
               px = py*100 + 100;
           }
		   {
			   int cwid = pfi->f_campowids[campo];
			   if (posicional == 3)
				  entra_cedit(pfi,campo,-1,128,pfi->f_ipfi,0,NULL,px,-1,-1,0);
			   else			  
				  entra_cedit(pfi,campo,-1,128,pfi->f_ipfi,0,NULL,px,-1,-1,0);
			   pfi->f_campowids[campo] = cwid;
		   }
           /*if (formato_campo(pfi,campo,qo) >= 0)
              v_print(qo);*/
       }
   }
   Patrib &= ~128;


   return(0);
}

/*
 * printusi
 * pinta el contenido de un campo (a partir del 1)
 * igual que printcam pero posicionando el cursor
 */
printusi(pfi,campo)
FI_DEF *pfi;
int campo;
{
   cursor(pfi->f_campos[campo-1].c_mante.c_qp);
   printcam_x(pfi,campo,0);
   return(0);
}


printcam_p(pfi,campo)
FI_DEF *pfi;
int campo;
{
   return printcam_x(pfi,campo,2);
}


printcam(pfi,campo)
FI_DEF *pfi;
int campo;
{
   return printcam_x(pfi,campo,1);
}



