
#include <memory.h>
#include <stdio.h>


#define moto_short(a) /* */
unsigned short local_term[52];
unsigned short local_term_[52];

unsigned short _Posicion() {return(777);}

int nume_code()
{
return( 00000 );
}


int nume_mochila()
{
return( 00000 );
}

int decripto(unsigned char *buf)
{
short i,n;
unsigned short nn,mm,hh;
#ifdef ESMOTOROLA
unsigned short t_t;
unsigned short kk[256],ll[256];
#else
unsigned short *kk,*ll;
#endif


#ifdef ESMOTOROLA
memcpy(kk,buf,512);
#else   
kk = (unsigned short *)buf;
#endif

nn = nume_mochila(); /* decripto2 */

for(i = 0;i < 256;i++)
  {
#ifdef ESMOTOROLA 
  for (n = 0;n < ((nn & 0xff) & 0xf);n++)
     {
     moto_short(&(kk[i]));
     hh = kk[i] & 0x8000;
     hh >>= 15;
     kk[i] <<= 1;
     kk[i] |= hh;
     moto_short(&(kk[i]));     
     }
#else
  for (n = 0;n < ((nn & 0xff) & 0xf);n++)
     {
     hh = kk[i] & 0x8000;
     hh >>= 15;
     kk[i] <<= 1;
     kk[i] |= hh;
     }
#endif            
  }
#ifdef ESMOTOROLA
memcpy(buf,kk,512);
memcpy(ll,buf+1,508);
#else
ll = (unsigned short *)( ((unsigned char *)buf+1) );
#endif

for (n = 0;n < 254;)
  {
#ifdef ESMOTOROLA
  for(i = 0;i < (nn & 0xff) && n < 254;i++)
    {
    moto_short(&(ll[n]));
    ll[n] ^= ((((nn & 0xff00) >> 8) & 0xf) + i);
    moto_short(&(ll[n]));
    n++;
    }
#else   
  for(i = 0;i < (nn & 0xff) && n < 254;i++)
    {
    ll[n] ^= (((nn & 0xff00) >> 8) & 0xf) + i;
    n++;
    }
#endif    
  }

#ifdef ESMOTOROLA
memcpy(buf+1,ll,508);
memcpy(kk,buf,512);
#endif

for(i = 0;i < 256;i++)
  {
  moto_short(&(kk[i]));         
  kk[i] ^= nn;
  moto_short(&(kk[i]));    
  }

  /* decripto */
nn = nume_code();
/* moto_short(&nn); */

if (!nn) {
	return(-1);
	}

moto_short(&(kk[255]));
mm = kk[255] ^ nn;
moto_short(&(kk[255]));

for(i = 0;i < 100;i++)
  {
#ifdef ESMOTOROLA       
     for (n = 0;n < ((mm & 0xff) & 0xf);n++)
       {
       moto_short(&(kk[i]));
       hh = kk[i] & 0x8000;
       hh >>= 15;
       kk[i] <<= 1;
       kk[i] |= hh;
       moto_short(&(kk[i]));
       }       
#else
     for (n = 0;n < ((mm & 0xff) & 0xf);n++)
       {
       hh = kk[i] & 0x8000;
       hh >>= 15;
       kk[i] <<= 1;
       kk[i] |= hh;
       }
#endif       
  }
for(i = 100;i < 155;i++)
  {
#ifdef ESMOTOROLA       
     for (n = 0;n < (((mm & 0xff00) >> 8) & 0xf);n++)
       {
       moto_short(&(kk[i]));
       hh = kk[i] & 0x8000;
       hh >>= 15;
       kk[i] <<= 1;
       kk[i] |= hh;
       moto_short(&(kk[i]));
       }
#else
     for (n = 0;n < (((mm & 0xff00) >> 8) & 0xf);n++)
       {
       hh = kk[i] & 0x8000;
       hh >>= 15;
       kk[i] <<= 1;
       kk[i] |= hh;
       }
#endif       
  }
mm = 0;
for(i = 0;i < 255;i++)
  {
  moto_short(&(kk[i]));
  mm ^= kk[i];
  moto_short(&(kk[i]));
  }
/* printf("mm=%X\n",mm); */
mm ^= nn;
moto_short(&(kk[255]));
if (mm != kk[255])
{
/*    printf("PARITY ERROR!!\n"); */
}
moto_short(&(kk[255]));
for(i = 0;i < 255;i++)
  {
#ifdef ESMOTOROLA
  for (n = 0;n < ((nn & 0xff) & 0xf);n++)
     {
     moto_short(&(kk[i]));                              
     hh = kk[i] & 0x8000;
     hh >>= 15;
     kk[i] <<= 1;
     kk[i] |= hh;
     moto_short(&(kk[i]));                                   
     }
#else   
  for (n = 0;n < ((nn & 0xff) & 0xf);n++)
     {
     hh = kk[i] & 0x8000;
     hh >>= 15;
     kk[i] <<= 1;
     kk[i] |= hh;
     }
#endif       
  }
#ifdef ESMOTOROLA
memcpy(buf,kk,512);
memcpy(ll,buf+1,508);
#else  
ll = (unsigned short *)( ((unsigned char *)buf+1) );
#endif
for (n = 0;n < 254;)
  {
#ifdef ESMOTOROLA
  for(i = 0;i < (nn & 0xff) && n < 254;i++)
    {
    moto_short(&(ll[n]));
    ll[n] ^= (((nn & 0xff00) >> 8) & 0xf) + i;
    moto_short(&(ll[n]));    
    n++;
    }
#else   
  for(i = 0;i < (nn & 0xff) && n < 254;i++)
    {
    ll[n] ^= (((nn & 0xff00) >> 8) & 0xf) + i;
    n++;
    }
#endif    
  }
  
#ifdef ESMOTOROLA
memcpy(buf+1,ll,508);
memcpy(kk,buf,512);
#endif
 
for(i = 0;i < 255;i++)
  {
  moto_short(&(kk[i]));         
  kk[i] ^= nn;
  moto_short(&(kk[i]));
  }

#ifdef ESMOTOROLA
memcpy(buf,kk,512);
#endif

return(0);
}


static int cripto2(buf)
char *buf;
{
short i,n;
unsigned short nn = nume_mochila(),hh;
#ifdef ESMOTOROLA
unsigned short t_t;
unsigned short kk[256],ll[256];
#else
unsigned short *kk,*ll;
#endif

#ifdef ESMOTOROLA
memcpy(kk,buf,512);
#else   
kk = (unsigned short *)buf;
#endif

for(i = 0;i < 256;i++)
  {
  moto_short(&(kk[i]));
  kk[i] ^= nn;
  moto_short(&(kk[i]));  
  }

#ifdef ESMOTOROLA
memcpy(buf,kk,512);
memcpy(ll,buf+1,508);
#else
ll = (unsigned short *)( ((unsigned char *)buf+1) );
#endif

for (n = 0;n < 254;)
  {
  for(i = 0;i < (nn & 0xff) && n < 254;i++)
    {
    moto_short(&(ll[n]));    	
    ll[n] ^= (((nn & 0xff00) >> 8) & 0xf) + i;
    moto_short(&(ll[n]));    
    n++;
    }
  }
  
#ifdef ESMOTOROLA
memcpy(buf+1,ll,508);
memcpy(kk,buf,512);
#endif 
 
for(i = 0;i < 256;i++)
  {
  for (n = 0;n < ((nn & 0xff) & 0xf);n++)
     {
     moto_short(&(kk[i]));     	
     hh = kk[i] & 0x1;
     hh <<= 15;
     kk[i] >>= 1;
     kk[i] |= hh;
     moto_short(&(kk[i]));     
     }
  }
  
#ifdef ESMOTOROLA
memcpy(buf,kk,512);
#endif

return(0);
}

int cripto(char *buf,int k)
{
short i,n;
unsigned short nn = nume_code(),mm,hh;
#ifdef ESMOTOROLA
unsigned short t_t;
unsigned short kk[256],ll[256];
#else
unsigned short *kk,*ll;
#endif

if (!nn) return(-1);

if (k < 510)
  {
  buf[k] = 4;
  k++;
  memcpy(buf+k,buf,510-k); /* rellenar bloque */
  }

#ifdef ESMOTOROLA
memcpy(kk,buf,512);
#else   
kk = (unsigned short *)buf;
#endif

for(i = 0;i < 255;i++)
  {
  moto_short(&(kk[i]));
  kk[i] ^= nn;
  moto_short(&(kk[i]));  
  }

#ifdef ESMOTOROLA
memcpy(buf,kk,512);
memcpy(ll,buf+1,508);
#else
ll = (unsigned short *)( ((unsigned char *)buf+1) );
#endif  

for (n = 0;n < 254;)
  {
  for(i = 0;i < (nn & 0xff) && n < 254;i++)
    {
    moto_short(&(ll[n]));
    ll[n] ^= (((nn & 0xff00) >> 8) & 0xf) + i;
    moto_short(&(ll[n]));    
    n++;
    }
  }

#ifdef ESMOTOROLA
memcpy(buf+1,ll,508);
memcpy(kk,buf,512);
#endif
  
for(i = 0;i < 255;i++)
  {
  for (n = 0;n < ((nn & 0xff) & 0xf);n++)
     {
     moto_short(&(kk[i]));     	
     hh = kk[i] & 0x1;
     hh <<= 15;
     kk[i] >>= 1;
     kk[i] |= hh;
     moto_short(&(kk[i]));     
     }
  }
mm = 0;
for(i = 0;i < 255;i++)
  {
  moto_short(&(kk[i]));     	  	
  mm ^= kk[i];
  moto_short(&(kk[i]));     	  
  }
for(i = 0;i < 100;i++)
  {
     for (n = 0;n < ((mm & 0xff) & 0xf);n++)
       {
       moto_short(&(kk[i]));     	       	
       hh = kk[i] & 0x1;
       hh <<= 15;
       kk[i] >>= 1;
       kk[i] |= hh;
       moto_short(&(kk[i]));     	       
       }
  }
for(i = 100;i < 155;i++)
  {
     for (n = 0;n < (((mm & 0xff00) >> 8) & 0xf);n++)
       {
       moto_short(&(kk[i]));     	       	
       hh = kk[i] & 0x1;
       hh <<= 15;
       kk[i] >>= 1;
       kk[i] |= hh;
       moto_short(&(kk[i]));
       }
  }
moto_short(&(kk[255]));     	  
kk[255] = mm ^ nn;
moto_short(&(kk[255]));

#ifdef ESMOTOROLA
memcpy(buf,kk,512);
#endif

cripto2(buf);
return(0);
}
