

#include <stdio.h>

void Ar()
{

}

void Cr()
{

}
int v_prints(char *f,char *s)
{
printf(f,s);
return(printf("\r\n"));
}

int pprint(int p,char *s)
{
return(printf("%s\r\n",s));
}

int pprint2(int p,char *s)
{
return(printf("%s\r\n",s));
}


int zonablan(int pi,int pf)
{
return(pi+pf); /* :-) */
}



int main(int argc,char *argv[])
{
return(linkar(argc,argv));
}