#include <stdio.h>

#ifndef ESMOTOROLA
#define moto_long(a) /* */
#endif

static int pasa_a_numero(buf)
unsigned char *buf;
{
   int i;
   
   for (i =0;buf[i];i++)
   {
      if (buf[i] == '.')
         buf[i] = 63;
      else
         if (buf[i] == '-')
            buf[i] = 62;
         else
            if (buf[i] >= 'A' && buf[i] <= 'Z')
               buf[i] = buf[i] - 'A';
            else
               if (buf[i] >= '0' && buf[i] <= '9')
                  buf[i] = buf[i] - 22;
               else
                  buf[i] = buf[i] - 61;
   }
   
   return(i);
}



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



int en_new_code(_buf,passwd)
unsigned char *_buf;
char *passwd;
{
			unsigned int finlen = 0;
         unsigned short mascara;
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
         
         
         if (passwd)
         {
            strcpy(buf,passwd);
            de_new_code(buf,12,2);
            mascara = buf[0];
            finlen  = buf[1];
         }
         
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
			moto_long(&mib.b);
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
			moto_long(&mib.b);
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
			moto_long(&mib.b);
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


static int malas = 0;

int de_new_code(_buf,mascara,finlen)
unsigned char *_buf;
unsigned short mascara;
unsigned int finlen;
{
   int len = strlen(_buf);
   int p,q,r,i,j,ultimobit,nbloque,quepunto;
   unsigned char t,t1,t2,t3;
   unsigned char buf[100],buf2[100];
   unsigned long bloque[10];
   unsigned long dato1,dato2;
   union mibloque
   {
      unsigned long b;
      unsigned char c[4];
   } mib;
   
   if (finlen < 1 || finlen >= 41)
   {
      return(-1);
   }
   
   pasa_a_numero(_buf);
   
   
   q = len * 6;
   len = 0;
   r = 0;
   p = 0;
   buf[0] = 0;
   for (i = 0;i < q;i++,p++)
   {
      if (i && !(i%6))
      {
         r++;
      }
      if (p && !(p%8))
      {
         len++;
         buf[len] = 0;
      }
      
      t = _buf[r];
      if ( (i%6) )
      {
         t >>= (i%6);
      }
      t &= 1;  
      if ( (p%8) )
      {
         t <<= (p%8);
      }
      buf[len] |= t;
   }
   len++;
   
   
   p = len-1;
   r = 4;
   ultimobit = 0;
   t2 = 0;
   while(p >= 0)
   {
      t = buf[p];
      for (i = 7;i >= 0;i--)
      {
         t1 = t;
         if (i)
            t1 >>= i;
         t1 &= 1;
         if (t1 && !ultimobit)
         {
            ultimobit = i + 1;  
            if (i)
               t1 <<= i;
            buf[p] &= ~t1;
         }
         else
         {
            if (ultimobit)
            {		
               t3 = t1 << i;		
               buf[p] &= ~t3;
               if (r > 1)
               {
                  t1 <<= (r-1);
               }
               t2 |= t1;
               r--;
               if (!r)
               {
                  if (!i)
                  {
                     ultimobit = 8;
                     p--;
                  }
                  else
                     ultimobit = i;
                  break;
               }
            }
         }
      }
      if (!r)
      {
         break;
      }
      p--;
   }
   
   if (!ultimobit)
   {
      /*printf("MUY MAL 1\n\n");*/
   }
   
   len = p + 1;
   t = 0;
   for (i = 0;i < len;i++)
   {
      t ^= buf[i];
   }
   t1 = t & 0xf;
   t >>= 4;
   t1 ^= (t & 0xf);
   if (t1 != t2)
   {
      /*printf("REQUETE MAL 1 \n\n");*/
   }
   
   /* sacar la mascara */
   r = 6;
   t2 = 0;
   while(p >= 0)
   {
      t = buf[p];
      for (i = ultimobit-1;i >= 0;i--)
      {
         t1 = t;
         if ( i )
            t1 >>= i;
         t1 &= 1;	
         t3 = t1 << i;		
         buf[p] &= ~t3;
         if (r > 1)
         {
            t1 <<= (r-1);
         }
         t2 |= t1;
         r--;
         if (!r)
         {
            if (!i)
            {
               ultimobit = 8;
               p--;
            }
            else
               ultimobit = i;
            break;
         }	  
      }
      if (!r)
      {
         break;
      }
      p--;
      ultimobit = 8;
   }
   len = p + 1;
   
   if (mascara == 99) mascara = t2;
   
   memset(buf+len,0,99-len);
   nbloque = finlen / 5;
   r = 0;
   for (i = 0;i < nbloque;i++)
   {
      mib.b = 0L;
	  moto_long(&mib.b);
      for (j = 0;j < 3;j++)
      {
         mib.c[j] = buf[r] ^ finlen ^ mascara;
         r++;
      }  
	  moto_long(&mib.b);
      bloque[i] = mib.b;
   }
   
   for (i = 0;i < (nbloque-(nbloque % 2));i++)
   {
      mib.b = bloque[i];
	  moto_long(&mib.b);
      if ( (i%2) )
      {
         t = buf[r];
         t >>= 4;
         t &= 0xf;
         mib.c[3] = t;
         r++;
      }
      else
      {
         mib.c[3] = buf[r] & 0xf;	
      }
	  moto_long(&mib.b);
      bloque[i] = mib.b;
   }
   t1 = buf[r];
   for (i = 0;i < (nbloque-(nbloque % 2));i++)
   {
      mib.b = bloque[i];
	  moto_long(&mib.b);
      t = t1;
      if (i)
         t >>= i;
      t &= 1;
      t <<= 4;
      mib.c[3] |= t;
	  moto_long(&mib.b);
      bloque[i] = mib.b;
   }
   ultimobit = i;
   j = nbloque % 2;
   if (j)
   {
      mib.b = bloque[nbloque-1];
	  moto_long(&mib.b);
      t1 = mib.c[3];
      quepunto = 0;
      for (i = nbloque;i < 8 && quepunto < 5;i++)
      {
         t = buf[r];
         t >>= (i-1);
         t &= 1;
         t <<= quepunto;
         t1 |= t;
         quepunto++;	
      }    
      if (quepunto < 5)
      {
         r++;	
         for (i = 0;quepunto < 5;quepunto++)
         {
            t = buf[r];
            if (i)
               t >>= i;
            t &= 1;     
            t <<= quepunto;
            t1 |= t;
            i++;
         }	
      }
      mib.c[3] = t1;
	  moto_long(&mib.b);
      bloque[nbloque-1] = mib.b;
      ultimobit = (i-1)%8;
      ultimobit++;
   }
   
   quepunto = 0;
   buf2[quepunto] = 0;
   for (i = 0;i < nbloque;i++)
   {  
      dato1 = 1L;  
      for (j = 0;j < 5;j++)
      {  	
         dato2 = ((bloque[i] / dato1) % 50L);
         buf2[quepunto] = (unsigned char)dato2;
         dato1 *= 50L;	
         quepunto++;
         buf2[quepunto] = 0;
      }
   }
   
   if ((finlen%5)) 
   {  
      q = (finlen%5) * 8;  
      i = ultimobit;
      for (j = 0;j < q;j++,i++)
      {
         if (j && !(j%8))
         {
            buf2[quepunto] = buf2[quepunto] ^ finlen ^ mascara;
            quepunto++;
            buf2[quepunto] = 0;	  
         }
         if (i && !(i%8))
         {
            r++;
         }
         t = buf[r];
         if ((i%8))
         {
            t >>= (i%8);
         }
         t &= 1;
         if ((j%8))
         {
            t <<= (j%8);
         }
         buf2[quepunto] |= t;
      }
      ultimobit = (i-1)%8;
      ultimobit++;
      buf2[quepunto] = buf2[quepunto] ^ finlen ^ mascara;
      quepunto++;
   }
   
   if (quepunto != finlen)
   {
      /*printf("WARRING %d %d\n\n",quepunto,finlen);*/
   }
   memcpy(_buf,buf2,finlen);
   return(0);
}
