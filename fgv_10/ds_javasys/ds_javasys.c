
#include <jni.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <enlace_DsSysJava.h>
#include <fgvsys.h>
#include <fgvmnexp.h>

int sacalog = 1;
/* datos basico */
typedef void (*_despachador_)(int,int,void *);
extern _despachador_ DespachadorExterno;

int envia_subejecuta_a_server();
int entra_util();
//
extern int SinColaMensajes;


extern long server_nume_serie;
extern long nume_serie;
extern int pass_cambio_autorizado;

extern int server_ver1;
extern int server_ver2;
extern unsigned char server_verletra;
extern int server_internal_build;

extern int rtme_version1;
extern int rtme_version2;
extern char rtme_versionletra;
extern int numero_build;

extern int en_conexion_terminal;

extern int noboton_usuarios;

extern int server_internal_build;
extern int server_fgvid;
extern int fgv_internal_build;

extern int sin_signals;

extern int RetroDespachador(void *puntero);

#define MAXPATH 300

extern char dirbin[MAXPATH];
extern char dirdef[MAXPATH];
extern char dirpan[MAXPATH];
extern char dirfic[MAXPATH];
extern char nomfdir[MAXPATH];
extern char dirficb[MAXPATH];
extern char dirbass[MAXPATH];
extern char dirbase[MAXPATH];


extern char server_dirbase1[300];

extern int user_who;
extern int subnumero_de_usuario;


char *i_nombre();
int i_leeimpre(char *f);
char *rw_directorio(char *p);

/* datos locales java */
static JNIEnv *ds_env = NULL;
static jobject ds_obj;

static jclass String_Class = NULL; 

static int p;

static jobject nododefecto;
static int  valordefecto = -1;

static char local_buffer[2048];

/*JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *jvm, void *reserved);*/

#define ENTRAJAVA JNIEnv *a_env = ds_env;jobject a_obj = ds_obj;ds_env = env;ds_obj = obj;
#define SALJAVA ds_env = a_env;ds_obj = a_obj;


static void oem_a_ansi(unsigned char *c)
{
   while(*c)
   {
	   switch (*c)
	   {
	   case     130 :*c=233;break;
	   case     129 :*c=252;break;
	   case     160 :*c=225;break;
	   case     161 :*c=237;break;
	   case     162 :*c=243;break;
	   case     163 :*c=250;break;
	   case     164 :*c=241;break;
	   case     165 :*c=209;break;
	   case     166 :*c=170;break;
	   case     167 :*c=186;break;
	   case     168 :*c=191;break;
	   case     239 :*c=146;break;
	   case     241 :*c=177;break;
	   case     246 :*c=247;break;
	   case     128 :*c=199;break;
	   case     131 :*c=226;break;
	   case     132 :*c=228;break;
	   case     133 :*c=224;break;
	   case     134 :*c=229;break;
	   case     135 :*c=231;break;
	   case     136 :*c=234;break;
	   case     137 :*c=235;break;
	   case     138 :*c=232;break;
	   case     139 :*c=239;break;
	   case     140 :*c=238;break;
	   case     141 :*c=236;break;
	   case     142 :*c=196;break;
	   case     143 :*c=197;break;
	   case     144 :*c=201;break;
	   case     145 :*c=230;break;
	   case     146 :*c=198;break;
	   case     147 :*c=244;break;
	   case     148 :*c=246;break;
	   case     149 :*c=242;break;
	   case     150 :*c=251;break;
	   case     151 :*c=249;break;
	   case     152 :*c=255;break;
	   case     153 :*c=214;break;
	   case     154 :*c=220;break;
	   case     155 :*c=248;break;
	   case     156 :*c=163;break;
	   case     157 :*c=216;break;
	   case     158 :*c=215;break;
	   case     159 :*c=131;break;
	   case     169 :*c=174;break;
	   case     170 :*c=172;break;
	   case     171 :*c=189;break;
	   case     172 :*c=188;break;
	   case     173 :*c=161;break;
	   case     174 :*c=171;break;
	   case     175 :*c=187;break;
	   case     176 :*c=128;break;
	   case     177 :*c=20; break;
	   case     178 :*c=21; break;
	   case     179 :*c=124;break;
	   case     180 :*c=22; break;
	   case     181 :*c=193;break;
	   case     182 :*c=194;break;
	   case     183 :*c=192;break;
	   case     184 :*c=169;break;
	   case     185 :*c=23; break;
	   case     186 :*c=24; break;
	   case     187 :*c=25; break;
	   case     188 :*c=26; break;
	   case     189 :*c=162;break;
	   case     190 :*c=165;break;
	   case     198 :*c=227;break;
	   case     199 :*c=195;break;
	   case     196 :*c=151;break;
	   /*case     196 :*c='-';break;*/
	   case     207 :*c=156;break;
	   case     210 :*c=202;break;
	   case     211 :*c=203;break;
	   case     212 :*c=200;break;
	   case     214 :*c=205;break;
	   case     224 :*c=211;break;
	   case     225 :*c=223;break;
	   case     227 :*c=210;break;
	   case     230 :*c=181;break;
	   case     233 :*c=218;break;
	   case     235 :*c=217;break;  
	   }
           c++;
   }
   return;
}



/*
    private native int fPreInicializa(int argc,String argv[]);
    private native int fInicializa(int argc,String argv[]);
    private native int fFinaliza();
    private native int fLogin(String usuario,String password,String cambio);
    private native int fOpcion();
    private native int fLogout();
    private native int fDevuelveVentana();
    private native int fNuevaVentana();
    private native String fPreparaEjecucion(String exe,String tmp,String user,String pass,int opcion);
    private native int fAsignaVentana();
    private native int fDespachadorInterno(int funcion,StringBuffer puntero);
*/


static void
JNU_ThrowByName(JNIEnv *env, const char *name, const char *msg)
{
    jclass cls = (*env)->FindClass(env, name);
    /* if cls is NULL, an exception has already been thrown */
    if (cls != NULL) {
        (*env)->ThrowNew(env, cls, msg);
    }
    /* free the local ref */
    (*env)->DeleteLocalRef(env, cls);
}

static void InicializaGlobales(JNIEnv *env)
{   
    sin_signals = 1;

    if (String_Class == NULL) 
    {
       jclass fooClass = (*env)->FindClass(env, "java/lang/String");
       if (fooClass == NULL) 
       {
	    JNU_ThrowByName(env,
				"java/lang/IllegalArgumentException",
				"Problema al localizar java/lang/String");
            return; /* error */
       }
       String_Class = (*env)->NewGlobalRef(env, fooClass);
    }
}

static jvalue
JNU_CallStaticMethodByName(JNIEnv *env,
                     jboolean *hasException,
                     jobject obj,
                     const char *name,
                     const char *descriptor, ...)
{
    va_list args;
    jclass clazz;
    jmethodID mid;

	jvalue result;
	if ((*env)->EnsureLocalCapacity(env, 2) == JNI_OK) {
	    clazz = (*env)->GetObjectClass(env, obj);
	    mid = (*env)->GetStaticMethodID(env, clazz, name,
		                       descriptor);
	    if (mid) {
		const char *p = descriptor;
		/* skip over argument types to find out the
		   return type */
		while (*p != ')') p++;
		/* skip ')' */
		p++;
		va_start(args, descriptor);
		switch (*p) {
		case 'V':
		     (*env)->CallStaticVoidMethodV(env, obj, mid, args);
		     break;
		case '[':
		case 'L':
		     result.l = (*env)->CallStaticObjectMethodV(
		                            env, obj, mid, args);
		     break;
		case 'Z':
		     result.z = (*env)->CallStaticBooleanMethodV(
		                            env, obj, mid, args);
		     break;
		case 'B':
		     result.b = (*env)->CallStaticByteMethodV(
		                            env, obj, mid, args);
		     break;
		case 'C':
		     result.c = (*env)->CallStaticCharMethodV(
		                            env, obj, mid, args);
		     break;
		case 'S':
		     result.s = (*env)->CallStaticShortMethodV(
		                            env, obj, mid, args);
		     break;
		case 'I':
		     result.i = (*env)->CallStaticIntMethodV(
		                            env, obj, mid, args);
		     break;
		case 'J':
		     result.j = (*env)->CallStaticLongMethodV(
		                            env, obj, mid, args);
		     break;
		case 'F':
		     result.f = (*env)->CallStaticFloatMethodV(
                                     env, obj, mid, args);
              break;
		  case 'D':
		      result.d = (*env)->CallStaticDoubleMethodV(
		                             env, obj, mid, args);
		      break;
		  default:
                      printf("illegal descriptor para el procedimiento (%s)\r\n",name);fflush(stdout);
		      (*env)->FatalError(env, "illegal descriptor");
		  }
		  va_end(args);
	      }
              else
              {
                  printf("No se encuentra el procedimiento (%s) en java\r\n",name);fflush(stdout);
              }
	      (*env)->DeleteLocalRef(env, clazz);
	  }
          else
          {
              printf("Error de 'local capacity' para el procedimiento (%s) en java\r\n",name);fflush(stdout);
          }
	  if (hasException) 
          {
	      *hasException = (*env)->ExceptionCheck(env);
          }
          else
          { 
              if ((*env)->ExceptionCheck(env))
              {
                   printf("Excepcion en (%s)\r\n",name);fflush(stdout);                
              }
          }
  return result;
}

static void Pon_Tree_Items(jobject padre,MenuStruct *menu)
{
         jobject obj;
         jbyteArray buffer;
         char *nombre,*opcion;
         jvalue rr;
         int entero;
         int i,tam,len;
         char tmp[1024];

         if (!padre)
         {
	     entero = -1;
         }
         else
             entero = 0;
	 nombre = menu->NomMenu;
	 tam = len = strlen(nombre)+1;
	 //oem_a_ansi((unsigned char *)nombre);

	 buffer = (*ds_env)->NewByteArray(ds_env, len);
	 (*ds_env)->SetByteArrayRegion(ds_env, buffer, 0, tam,(jbyte *)nombre);
	 //printf("creaNodoTree: llamando ....\r\n");fflush(stdout);
	 rr = JNU_CallStaticMethodByName(ds_env,NULL,ds_obj,"creaNodoTree","(Ljava/lang/Object;I[B)Ljava/lang/Object;",padre,entero,buffer);
	 obj = rr.l;
	 //printf("creaNodoTree: saliendo ....\r\n");fflush(stdout);
	 (*ds_env)->GetByteArrayRegion(ds_env, buffer, 0, len,(jbyte *)nombre);
	 (*ds_env)->DeleteLocalRef(ds_env, buffer);
   
	 for (i = 0;menu->Items[i].NomItem;i++)
	 {
	      if (menu->Items[i].OpItem[0] == 1 && menu->Items[i].OpItem[1] == 2 && menu->Items[i].OpItem[2] == 3 && menu->Items[i].OpItem[3] == 0)
	      {
		     Pon_Tree_Items(obj,(MenuStruct *)menu->Items[i].OpItem);
	      }
	      else 
	      {
                 memcpy((void *)&(entero),menu->Items[i].OpItem+1,sizeof(long));
		 entero++;
		 opcion = menu->Items[i].OpItem+1+sizeof(long);
		 nombre = (char *)menu->Items[i].NomItem;
		 tam = len = strlen(nombre)+3+strlen(opcion);
		 //oem_a_ansi((unsigned char *)nombre);
                 sprintf(tmp,"%s%c%s",nombre,7,opcion);
		 buffer = (*ds_env)->NewByteArray(ds_env, len);
		 (*ds_env)->SetByteArrayRegion(ds_env, buffer, 0, tam,(jbyte *)tmp);
		 //printf("creaNodoTree: llamando ....\r\n");fflush(stdout);
		 rr = JNU_CallStaticMethodByName(ds_env,NULL,ds_obj,"creaNodoTree","(Ljava/lang/Object;I[B)Ljava/lang/Object;",obj,entero,buffer);
                 if (valordefecto == entero) 
                 {
                    if (sacalog)
                    	printf("defectomenu set\r\n");
                    nododefecto = rr.l;
                 }
		 //printf("creaNodoTree: saliendo ....\r\n");fflush(stdout);
		 //(*ds_env)->GetByteArrayRegion(ds_env, buffer, 0, len,(jbyte *)tmp);
		 (*ds_env)->DeleteLocalRef(ds_env, buffer);
	      }
	 }   
}


void DespachadorLocal(int f,int entero,void *puntero)
{
    jbyteArray buffer;
    long tam = 0;
    long len = 0;
    long i2;

    if (sacalog) {
	    printf("DespachadorLocal(%d,%d,%x)\r\n",f,entero,(unsigned int)puntero);fflush(stdout);
    }
    
    if (!ds_env)
    {
        /*
        JNU_ThrowByName(env,
			"java/lang/IllegalArgumentException",
			"ds_env no definido!");
        */         
        printf("DespachadorLocal: No ds_env!\r\n");fflush(stdout);
        return;
    }

    if ((*ds_env)->ExceptionCheck(ds_env)) 
    {
	printf("Excepcion en DESPACHADOR_LOCAL(%d,%d)-PREVIA->limpiando\r\n",f,entero);fflush(stdout);                
        (*ds_env)->ExceptionClear(ds_env);
    } 

    if ((*ds_env)->PushLocalFrame(ds_env, 10) < 0) 
    {
        printf("Sin memoria en DESPACHADOR_LOCAL(%d,%d)\r\n",f,entero);fflush(stdout);                
    }

    if (f == DESPACHO_OMENU) 
    {
         MenuStruct *menu; 
         jobject obj = NULL;

         if (!puntero) 
         {
            (*ds_env)->PopLocalFrame(ds_env, NULL);
            return;
         }

	 nododefecto = NULL;
 	 valordefecto = entero;
         menu = (MenuStruct *)puntero;         
         if (sacalog) {
            printf("RellenadoMenu(%s) defecto:%d\r\n",menu->NomMenu,valordefecto);fflush(stdout);
         }
         Pon_Tree_Items(NULL,menu);         
	 buffer = (*ds_env)->NewByteArray(ds_env, sizeof(entero));
         if (buffer)
	    (*ds_env)->SetByteArrayRegion(ds_env, buffer, 0, sizeof(entero),(jbyte *)&entero);
         if (sacalog) {
            printf("creaNodoTree\r\n");fflush(stdout);
         }
         entero = -2;
         JNU_CallStaticMethodByName(ds_env,NULL,ds_obj,"creaNodoTree","(Ljava/lang/Object;I[B)Ljava/lang/Object;",nododefecto,entero,buffer);
         (*ds_env)->DeleteLocalRef(ds_env, buffer);
         if (sacalog) {
            printf("Sale RellenadoMenu\r\n");fflush(stdout);
         }
         (*ds_env)->PopLocalFrame(ds_env, NULL);
	 return;
    }


    if (puntero) 
    {
        if (f == DESPACHO_BINARIO)
        {
            memcpy(&tam,puntero,sizeof(long));
            memcpy(&len,((char *)puntero)+sizeof(long),sizeof(long)); /* longitud esperada de vuelta */            
  	    if (sacalog) {
	        printf("Despacho binario: tam = %ld  len = %ld\r\n",tam,len);fflush(stdout);
            }
            if (tam > len)
               len = tam;
        }
        else if (f == DESPACHO_PROCESAMENSAJES || f == DESPACHO_DATA_PROCESAMENSAJES)
        {
            tam = sizeof(int) * 10;
            if (f == DESPACHO_DATA_PROCESAMENSAJES)
	    {
		tam += *((int *)puntero);
	    }
            len = tam;
        }
	else if (f == DESPACHO_CCONTROL || f == DESPACHO_CEDIT || f == DESPACHO_DIF_CCONTROL ||
              f == DESPACHO_GETDATACONTROL || f == DESPACHO_MAINCOMMAND || f == DESPACHO_DIF_CEDIT ||
              f == DESPACHO_COMMANDCONTROL || f == DESPACHO_EN_LINEAS ||
              f == DESPACHO_BOTONSALIDA)
        {
           int *px1;
           char *pp = (char *)puntero;
           int n = entero;

		if (f == DESPACHO_CEDIT || f == DESPACHO_DIF_CEDIT)
		{
		  n &= 0xffff0000;
		  n >>= 16;
		  if (n > 0)
			 n--;
		}
	        if (((f == DESPACHO_CCONTROL || f == DESPACHO_DIF_CCONTROL) && entero == -1) || (f == DESPACHO_MAINCOMMAND && entero == ID_DLGINDICES))
	        {
		   /*entero = 0;*/
	           n = 0;
	        }

		tam = 0;
		do
		{
		 px1 = (int *)pp;
		 tam += px1[0];
		 pp += px1[0];
		 if ((f != DESPACHO_CCONTROL && f != DESPACHO_CEDIT && f != DESPACHO_DIF_CCONTROL && f != DESPACHO_DIF_CEDIT) || !n) 
		     break;
		 n--;
		} while(-1);
                len = tam;
        }
        else if (f == DESPACHO_PANTALLAS)
        {
            tam = len = 512;
        }
        else if (f == DESPACHO_MI_CONTROL)
        {
            tam = len = 1024;
        }
        else if (f == DESPACHO_USUARIOS)
        {
            tam = (strlen((char *)puntero)+1);
            len = -1;
        }
        else if (f == DESPACHO_FUNCIONES_IMPRESION && (entero == 1 || entero == 2 || entero == 5 || entero == 6 || entero == 7|| entero == 8 || entero == 998)) {
	    tam = len = 1024;
            if (entero == 8 || entero == 998) {
                    if (sacalog) {
	               printf("impresión excel:%s %s\r\n",(char*)puntero,((char*)puntero)+31);
                       fflush(stdout);
                    }
            }
	}
	else if (f == DESPACHO_MULTIPLE && (entero == 12 || entero == 13))
	{  
	    tam = len = *((int *)puntero);
	}
	else if (f == DESPACHO_MULTIPLE && (entero == 668))
	{  
	    tam = len = 4;
	}
	else
	{
	   if (f == DESPACHO_PROGRESO)
	   {
		   tam = len = sizeof(int);
		   len += sizeof(long);
	   }
	   else
	   {
		   tam = (strlen((char *)puntero)+1);
		   len = -1;
	   }
	}        
    }

    if (puntero)
    {
            if (len == 0)
                buffer = (*ds_env)->NewByteArray(ds_env, 1);
            else if (len == -1)
            {
                if (tam > 2048)
                   buffer = (*ds_env)->NewByteArray(ds_env, tam);
                else
                   buffer = (*ds_env)->NewByteArray(ds_env, 2048);
            }
            else
	        buffer = (*ds_env)->NewByteArray(ds_env, len);
	    if ((*ds_env)->ExceptionCheck(ds_env)) 
	    {
		printf("Excepcion en DESPACHADOR_LOCAL(%d,%d)-0\r\n",f,entero);fflush(stdout);                
	    } 
    }
    else 
        buffer = NULL;
    if (buffer)
    {
        (*ds_env)->SetByteArrayRegion(ds_env, buffer, 0, tam,(jbyte *)puntero);
        if ((*ds_env)->ExceptionCheck(ds_env))
        {
           printf("Excepcion en DESPACHADOR_LOCAL(%d,%d)-1\r\n",f,entero);fflush(stdout);                
        }
    }    
    if (sacalog) {
       printf("DespachadorLocal: llamando .... tam=%ld  len=%ld\r\n",tam,len);fflush(stdout);
    }
    JNU_CallStaticMethodByName(ds_env,NULL,ds_obj,"despachadorExterno", "(II[B)V",f,entero,buffer);
    if (sacalog) {
       printf("DespachadorLocal: saliendo ....\r\n");fflush(stdout);
    }
    if (buffer)
    {
        if (len == -1) {
           if (tam > 2048)
              len = tam;
           else {
               (*ds_env)->GetByteArrayRegion(ds_env, buffer, 0, tam,(jbyte *)local_buffer);               
               local_buffer[tam] = 0;
               if (strlen(local_buffer) == tam)
                    (*ds_env)->GetByteArrayRegion(ds_env, buffer, 0, 2048,(jbyte *)local_buffer);
 	       if ((*ds_env)->ExceptionCheck(ds_env))
               {
		    printf("Excepcion en DESPACHADOR_LOCAL(%d,%d)-2.\r\n",f,entero);fflush(stdout);                
	       }
               else
                    strcpy((char *)puntero,local_buffer);
           }
        }
        if (len > 0)
        {
           (*ds_env)->GetByteArrayRegion(ds_env, buffer, 0, len,(jbyte *)puntero);
 	   if ((*ds_env)->ExceptionCheck(ds_env))
	   {
	       printf("Excepcion en DESPACHADOR_LOCAL(%d,%d)-2\r\n",f,entero);fflush(stdout);                
	   }
        }
    	(*ds_env)->DeleteLocalRef(ds_env, buffer);
        if ((*ds_env)->ExceptionCheck(ds_env)) 
	{
	   printf("Excepcion en DESPACHADOR_LOCAL(%d,%d)-3\r\n",f,entero);fflush(stdout);                
	}
    }

    (*ds_env)->PopLocalFrame(ds_env, NULL);

    if (sacalog) {
        printf("DespachadorLocal: OK\r\n");fflush(stdout);
    }
}


JNIEXPORT jint JNICALL Java_enlace_DsSysJava_fPreInicializa
  (JNIEnv *env, jobject obj, jint argc, jobjectArray argv)
{
   jobject localref;
   int narg,i;
   char *arg[200];
   const jbyte *str;
   jstring newstr;
   ENTRAJAVA

   freopen("/tmp/test_stdout_2.txt","w",stdout);
   freopen("/tmp/test_stderr_2.txt","w",stderr);
  
   printf("->PreInicializa!\r\n");fflush(stdout);

   InicializaGlobales(env);

   DespachadorExterno = DespachadorLocal; 
   narg = argc;
   if ((*env)->GetArrayLength(env,argv) > narg)
   {
	    JNU_ThrowByName(env,
		        "java/lang/IllegalArgumentException",
		        "argv[] no puede alojar el numero de argumentos indicado en argc");
	    return narg;
   }

   for (i = 0; i < narg; i++)
   {
        arg[i] = NULL;	
	localref = (*env)->GetObjectArrayElement(env,argv, i);
        if (localref)
        {
            if ((*env)->IsInstanceOf(env,localref, String_Class)) 
            {
                str = (*env)->GetStringUTFChars(env, localref, NULL);
                arg[i] = malloc(strlen(str)+100);
                strcpy(arg[i],str);                
                (*env)->ReleaseStringUTFChars(env, localref, str);
            }
            else
            {
		JNU_ThrowByName(env,
		     "java/lang/IllegalArgumentException",
		     "argv[] solo puede contener strings");
                return narg;
            }
            (*env)->DeleteLocalRef(env,localref);
        }
   }    
   arg[i] = NULL;
   pre_inicializa_systema(&narg,arg);
   for (i = 0; i < narg; i++) 
   {
        if (arg[i])
        {
        	newstr = (*env)->NewStringUTF(env, arg[i]);
        	(*env)->SetObjectArrayElement(env, argv, i, newstr);
        	(*env)->DeleteLocalRef(env, newstr);
        	free(arg[i]);
        }
   }

   SALJAVA
   return narg;
}


JNIEXPORT jint JNICALL Java_enlace_DsSysJava_fInicializa
  (JNIEnv *env, jobject obj, jint argc, jobjectArray argv)
{
   char *arg[200];
   int r,i;
   jobject localref;
   const jbyte *str;
   ENTRAJAVA

   printf("Inicializa!\r\n");fflush(stdout);   

   for (i = 0; i < argc; i++) 
   {
        arg[i] = NULL;	
	localref = (*env)->GetObjectArrayElement(env,argv, i);
        if (localref)
        {
            if ((*env)->IsInstanceOf(env,localref, String_Class)) 
            {
                str = (*env)->GetStringUTFChars(env, localref, NULL);
                arg[i] = malloc(strlen(str)+100);
                strcpy(arg[i],str);                
                (*env)->ReleaseStringUTFChars(env, localref, str);
            }
            else
            {
		JNU_ThrowByName(env,
		     "java/lang/IllegalArgumentException",
		     "argv[] solo puede contener strings");
                return r;
            }
            (*env)->DeleteLocalRef(env,localref);
        }
   }    
   arg[i] = NULL;
   r = 1;
   if ((p = inicializa_systema(argc,arg)) < 0)
   {
      if (p == -10)
      {         
         r = p;
      }
      r = 0;
   }   
   for (i = 0;i < argc;i++)
   {
      if (arg[i])
         free(arg[i]);
   }   

   SALJAVA
   printf("Inicializado! (en_conexion_terminal=%d)\r\n",en_conexion_terminal);fflush(stdout);   
   return r;
}

JNIEXPORT jint JNICALL Java_enlace_DsSysJava_fFinaliza
  (JNIEnv *env, jobject obj)
{
   ENTRAJAVA
   printf("Finaliza!\r\n");fflush(stdout);   
   finaliza_systema();
   DespachadorExterno = NULL;
   SALJAVA
   return 1;
}

JNIEXPORT jint JNICALL Java_enlace_DsSysJava_fLogin
  (JNIEnv *env, jobject obj, jstring usuario, jstring password, jstring cambio)
{
   int r = 0;
   const jbyte *usu;
   const jbyte *pass;
   const jbyte *cam;
   char user[30];
   char pasw[30];
   char camb[30];
   ENTRAJAVA
   
   printf("Login! (info int:%d long:%d)\r\n",sizeof(int),sizeof(long));fflush(stdout);   

   usu = (*env)->GetStringUTFChars(env, usuario, NULL);
   pass = (*env)->GetStringUTFChars(env, password, NULL);
   cam = (*env)->GetStringUTFChars(env, cambio, NULL);
   printf("login(p=%d):[%s],[%s],[%s]\r\n",p,usu,pass,cam);fflush(stdout);
   strcpy(user,usu);
   strcpy(pasw,pass);
   strcpy(camb,cam);
   r = login_usuario(p,user,pasw,camb);
   printf("sale login:[%s],[%s]->%d\r\n",usu,pass,r);fflush(stdout);
   (*env)->ReleaseStringUTFChars(env, usuario, usu);
   (*env)->ReleaseStringUTFChars(env, password, pass);
   (*env)->ReleaseStringUTFChars(env, cambio, cam);

   SALJAVA
   return r;
}

JNIEXPORT jint JNICALL Java_enlace_DsSysJava_fOpcion
  (JNIEnv *env, jobject obj)
{
   ENTRAJAVA
   printf("Opcion!\r\n");fflush(stdout);   
   if (opcion_usuario(p))
   {
      //SinColaMensajes = 0;
      printf("Opcion=1\r\n");fflush(stdout);   
      SALJAVA
      return 1;
   }
   //SinColaMensajes = 0;
   printf("Opcion=0\r\n");fflush(stdout);   
   SALJAVA
   return 0;
}

JNIEXPORT jint JNICALL Java_enlace_DsSysJava_fLogout
  (JNIEnv *env, jobject obj)
{
   ENTRAJAVA
   printf("Logout!\r\n");fflush(stdout);   
   if (logout_usuario())
   {
      SALJAVA
      return 1;
   }     
   SALJAVA
   return 0;
}

JNIEXPORT jint JNICALL Java_enlace_DsSysJava_fDevuelveVentana
  (JNIEnv *env, jobject obj)
{
   ENTRAJAVA

   /* ignorable ... */

   SALJAVA
   return 0;
}

JNIEXPORT jint JNICALL Java_enlace_DsSysJava_fNuevaVentana
  (JNIEnv *env, jobject obj)
{
   ENTRAJAVA
 
   /* ignorable ... */

   SALJAVA
   return 0;
}

JNIEXPORT jstring JNICALL Java_enlace_DsSysJava_fPreparaEjecucion
  (JNIEnv *env, jobject obj, jstring exe, jstring tmp, jstring user, jstring pass, jint opcion)
{
   const jbyte *_exe;
   const jbyte *_user;
   const jbyte *_pass;
   const jbyte *_tmp;
   char tmpx[1024];
   char exex[1024];
   ENTRAJAVA

   printf("PreparaEjecucion!\r\n");fflush(stdout);   

   _exe = (*env)->GetStringUTFChars(env, exe, NULL);
   _user = (*env)->GetStringUTFChars(env, user, NULL);
   _pass = (*env)->GetStringUTFChars(env, pass, NULL);
   _tmp = (*env)->GetStringUTFChars(env, tmp, NULL);
   strcpy(tmpx,_tmp);
   strcpy(exex,_exe);

   montapar_x(exex,tmpx,_user,_pass,opcion);

   (*env)->ReleaseStringUTFChars(env, exe, _exe);  
   (*env)->ReleaseStringUTFChars(env, user, _user);  
   (*env)->ReleaseStringUTFChars(env, pass, _pass);  
   (*env)->ReleaseStringUTFChars(env, tmp, _tmp);  

   printf("Sal PreparaEjecucion\r\n");fflush(stdout);   

   SALJAVA
   return (*env)->NewStringUTF(env, tmpx);
} 

JNIEXPORT jint JNICALL Java_enlace_DsSysJava_fAsignaVentana
  (JNIEnv *env, jobject obj)
{
   ENTRAJAVA
 
   /* ignorable ... */

   SALJAVA
   return 0;
}

JNIEXPORT jint JNICALL Java_enlace_DsSysJava_fDespachadorInterno
  (JNIEnv *env, jobject obj, jint funcion, jbyteArray buffer)
{
   char *puntero = NULL;
   jsize len = 0;
   int r = 0;

   ENTRAJAVA//

   if (sacalog) {
       printf("DespachadorInterno!\r\n");fflush(stdout);   
   }

   if (buffer) 
   {
       len = (*env)->GetArrayLength(env, buffer);
       if (len > 0)
       {
           if (sacalog) {
               printf("puntero len = %d!\r\n",len);fflush(stdout);   
           }
           puntero = malloc(len+1);
	   (*env)->GetByteArrayRegion(env, buffer, 0, len,(jbyte *)puntero);
       }
   }

   switch (funcion)
   {
   case 0: // recoger version       
      r = ( ((rtme_version1*100+rtme_version2)*256+rtme_versionletra) | (numero_build << 24) );
      break;
   case 1: // recoger funciones programadas

      // array de string ok
      //*((char ***)puntero) = (char **)desc_funs;


      r = (20);
      break;
   case 2:
      /* No necesario en X puro
      {
         int pi;
         RECT r;
         char *punt = (char *)puntero;
         memcpy(&pi,punt,sizeof(int));		 
         CuadroTab(pi,&r);
         memcpy(punt,&r,sizeof(RECT));
      }
      */
      break;
   case 3:
      /* No necesario en X puro  
      Coge_StdPan(puntero);
      */
      break;
   case 4:
      /* No necesario en X puro  
      RePon_StdPan(puntero);
      */
      break;
   case 5:
      /* compatible ... ojo que es reentrante a java por despachoexterno */
      agi_cola_timer();
      break;
   case 6:
      /* No necesario en X puro  
      stdpan_es_invisible();
      */
      break;
   case 7:
      /* No necesario en X puro  
      {
         int pi,pf,flag1,flag2;
         pi = ((RECT *)puntero)->left;
         pf = ((RECT *)puntero)->top;
         flag1 = ((RECT *)puntero)->right;
         flag2 = ((RECT *)puntero)->bottom;
         TxtARect(pi,pf,(RECT *)puntero);
         if (!flag1 && !flag2 && ModoProfundo)
         {
             ((RECT *)puntero)->top += 2;
             ((RECT *)puntero)->bottom -= 2;
         }
      }
      */
      break;
   case 8:
      /* No necesario en X puro ... no es usado  
      {
         short sqj;
         unsigned int qj;
         short tecla;
         HACCEL _agi_acelerador = agi_acelerador;
         memcpy((void*)&qj,(void*)puntero,sizeof(int));
         agi_acelerador = NULL;
         tecla = (short)leecar();
         sqj = (short)que_qjes(tecla,qj);
         qj = ((((unsigned int)tecla) << 16) & 0xffff0000);         
         qj |= (((unsigned int)sqj) & 0xffff);
         memcpy((void*)puntero,(void*)&qj,sizeof(int));
         agi_acelerador = _agi_acelerador;
      }
      */
      break;
   case 9:
      r = (en_conexion_terminal);
      break;
   case 10:
      // array de string ok, ignorable en version inicial solo se usa para la "transmision interna"
      /*
      {
         char **_dir = lee_direct(*((char **)puntero),3);
         *((char **)puntero) = (char *)_dir;
      }
      */
      break;
   case 11:
      // array de string ok, ignorable en version inicial solo se usa para la "transmision interna"      
      //libera_dir((char **)puntero);
      break;
   case 12:

      // array de string ok, ignorable en version inicial solo se usa para la "transmision interna"      
      return (rw_envia_fichero((char *)puntero,777));
      break;
    case 13: /* trasmision binaria inversa */
		     /* formato = int(funcion) long(tamano-in) long(tamano-out) long(modo)*/          
          if (sacalog) {
             printf("Entra RetroDespachador!\r\n");fflush(stdout);
          }
          if (RetroDespachador(puntero) < 0)
	  {
		  r = -1;
          } 
          if (sacalog) {
             printf("Sale RetroDespachador=%d!\r\n",r);fflush(stdout);
          }
	  break;
    case 14:
      strcpy((char *)puntero,dirbass);
      break;
    case 15:
      break;
	case 16:
          // adaptar a java string
	  if (en_conexion_terminal)
	  {
		  llama_fun_int(envia_subejecuta_a_server,(char *)puntero);
	  }
	  else
	  {
		  llama_fun_int(entra_util,(char *)puntero);
	  }
	  break;
	case 17:
	{
             // adaptar a java string
	    //pon_ayuda_de_campo(puntero);
	}
	  break;
	case 18:
	{
#ifdef PARAMASADELANTE
		/* enviar y borrar teclas de funcion programadas */
		char **pp = (char **)puntero;		
		char *p;
		char **p1;
		int i;
		*((int *)*pp) = 0/*FUNCIONES*/;
		pp++;
		p1 = NULL;/*(char **)malloc(sizeof(char *)*FUNCIONES);*/
		*pp = (char *)p1;

		for (i = 0;i < 0/*FUNCIONES*/;i++)
		{
			p = NULL;
			p = tecla_funcion(i,p,1);
			if (p && *p != '@' && *p != '#')
			{
				p1[i] = p;
				tecla_funcion(i,NULL,99);
			}
			else
				p1[i] = NULL;
		}
#endif
	}
	break;
	case 19:
                /*
		if (puntero)
			free(puntero);
                 */
		break;
	case 20:	
	  if (en_conexion_terminal)
	  {
		  memcpy(puntero,&server_nume_serie,sizeof(long));
		  memcpy((void *)(((char *)puntero)+sizeof(long)),&pass_cambio_autorizado,sizeof(long));
		  r = ( ((server_ver1*100+server_ver2)*256+server_verletra) | (server_internal_build << 24) );
	  }
	  else
	  {
		  memcpy(puntero,&nume_serie,sizeof(long));
		  memcpy((void *)(((char *)puntero)+sizeof(long)),&pass_cambio_autorizado,sizeof(long));		  
		  r = ( ((rtme_version1*100+rtme_version2)*256+rtme_versionletra) | (numero_build << 24) );
	  }	  
	break;
   case 21:      
      r = (noboton_usuarios);
      break;
	case 22:
		/* datos ini */
		{
                    // incluir en java array de strings
                    char clave[256];
		    strcpy(clave,(char *)puntero);                       
		    r = dame_valor_ini(clave,(char *)puntero);                    
		    printf("Datosini=%d clave=%s puntero=%s\r\n",r,clave,puntero);fflush(stdout);                    
		}
	   break;
	case 23:
	{
                // incluir en java array de strings, en java habra que consultar cada vez
                /*
		datoscomunes.nume_serie = &nume_serie;		
		datoscomunes.server_nume_serie = &server_nume_serie;
		datoscomunes.nume_serie_real = &nume_serie_real;
		datoscomunes.local_term = &(local_term[0]);
		datoscomunes.local_term_ = &(local_term_[0]);
		datoscomunes.local_acce_ = &(local_acce_[0]);
		datoscomunes.en_conexion_terminal = &en_conexion_terminal;		
		memset(datoscomunes.reservado,0,sizeof(datoscomunes.reservado));
                SALJAVA
		return (int)&datoscomunes;
                */
	}
	break;
	case 24: /* ! */
	  break;
	case 25:
	{  // incluir en java array de strings
	  if (!en_conexion_terminal)
	  {      
		  //memcpy(puntero,&fgv_internal_build,sizeof(int));

		  r =  2147;
	  }
	  else
	  {
		  //memcpy(puntero,&server_internal_build,sizeof(int));

		  r =  server_fgvid;
	  }	
	}
	break;
	case 26:
	{
#ifdef PARAMASADELANTE
	  char tmp[512],destino[128];		 
	  unsigned long tam;

	  tam = 3;
		/* instalacion python */	  
	  if (en_conexion_terminal)
	  {		 
		 strcpy(tmp,rw_directorio("agitool/python.tgz"));         
		 unlink(tmp);
		 tmp[0] = 3;
		 strcpy(tmp+1,"agitool/python.tgz");         
		 envia_control_a_server(tmp,strlen(tmp)+1,170);
		 modo_terminal();
	  }
	  /*
	  (*DespachadorExterno)((int)DESPACHO_PROGRESO,0,(void *)&tam);
	  tam = 1;
	  (*DespachadorExterno)((int)DESPACHO_PROGRESO,1,(void *)&tam);
      agi_cola_timer();
	  */
	  tmp[0] = 7;
	  strcpy(tmp+1,rw_directorio("agitool/python.tgz"));
	  strcpy(destino,rw_directorio(""));
	  agi_detar(tmp,destino);
	  /*unlink(rw_directorio("python/python.tgz"));*/
	  /*(*DespachadorExterno)((int)DESPACHO_PROGRESO,2,NULL);*/
#endif

	  r =  0;
	}
	break;
	case 27:
	{
                // array de string ok
                
		char *pdbs;
		if (en_conexion_terminal)		  		
			pdbs = server_dirbase1;
		else
			pdbs = dirbass;		
		sprintf((char *)puntero,"%s %03d%03d",pdbs,user_who,subnumero_de_usuario);
                

		r =  0;
	}
	break;
        case 28:
          envia_debug();
          printf("envia_debug\r\n");fflush(stdout);
          break;
	case 29:
	{
		/* siguiente libre */
	}
	break;
    case 77:
        if (puntero)
        {
          extern char *dsproxy;
          dsproxy = malloc(strlen((char *)puntero)+1);
          strcpy(dsproxy,(char *)puntero);
        }
        break;
    case 99:
      ptec(807);
      ptec(807);
      ptec(807);
      ptec(807);
      ptec(807);
      ptec(807);
      ptec(807);
      ptec(807);
      ptec(807);
      ptec(807);
      ptec(807);
      ptec(807);
      ptec(807);
      ptec(807);
      ptec(807);
      ptec(807);
      ptec(807);
      break;
	case 199:

                {
                  int r = 0; // comprobar si recoger desde parametro
                  leemensaje_oob_1(&r);
                  r =  r;
                }      		
	case 299:
          // correcto tal cual
		/* ping */
	  if (en_conexion_terminal)
	  {
		 envia_pingping();
	  }
	  break;

		break;
	case 500:          
                // array de string ok ... con adaptacion
                //SALJAVA
		//return ListaTareas(puntero);
	case 515:
      {
        // array de string ok
        
        char tmp[256];
        int *p1 = (int *)puntero;
        int r;
        strcpy(tmp,rw_directorio("dev/imp/"));
        strcat(tmp,(char *)(p1+1));        
        r = i_leeimpre(tmp);
        *p1 = r;
        if (r >= 0)
        {
            strcpy((char *)(p1+1),i_nombre());
        }
        
      }
	  break;
   case 600:
          /* ignorable de momento */
          //SALJAVA
	  //return (int)AccesoPython;
	  //break;
   default:
      break;
   }
   
   if (puntero) 
   {
       if (sacalog) {
          printf("free puntero len = %d!\r\n",len);fflush(stdout);   
       }
       (*env)->SetByteArrayRegion(env, buffer, 0, len,(jbyte *)puntero);      
       free(puntero);
       puntero = NULL;
   }
  
  
   if (sacalog) {
      printf("Salida DespachadorInterno(%d)!\r\n",r);fflush(stdout);      
   }
   SALJAVA
   return r;
}

/*
JNIEXPORT jint JNICALL Java_enlace_DsSysJava_memcpyInt
  (JNIEnv *env, jobject obj, jobject buffer, jint arg)
{
    (*env)->SetByteArrayRegion(env, buffer, 0, sizeof(jint),(jbyte *)&arg);
    return sizeof(jint);
}
*/
JNIEXPORT void JNICALL Java_enlace_DsSysJava_memset
  (JNIEnv *env, jobject obj, jobject buffer, jint valor,jint tam,jint ini,jint fin)
{
    int i,ifin;
    ifin = fin*tam;
    for (i = ini*tam;i < ifin;i += tam)
    {
    	(*env)->SetByteArrayRegion(env, buffer, i, tam,(jbyte *)&valor);
    }
}

static JNIEnv *dsh_env = NULL;
static jobject dsh_obj;

//#define ENTRAJAVA JNIEnv *a_env = ds_env;jobject a_obj = ds_obj;ds_env = env;ds_obj = obj;
//#define SALJAVA ds_env = a_env;ds_obj = a_obj;

JNIEXPORT jint JNICALL Java_enlace_DsSysJava_fenhebra
  (JNIEnv *env, jobject obj, jint fun, jint arg)
{
JNIEnv *a_env = dsh_env;jobject a_obj = dsh_obj;dsh_env = env;dsh_obj = obj;
EstoyVivo(arg);
dsh_env = a_env;dsh_obj = a_obj;
return 0;
}

void DespachadorLocal_hebra(int f,int entero,void *puntero)
{
    jbyteArray buffer;
    long tam = 0;
    long len = 0;
    long i2;

    if (sacalog > 1) {
	    printf("DespachadorLocal_hebra(%d,%d,%x)\r\n",f,entero,(unsigned int)puntero);fflush(stdout);
    }
    
    if (!dsh_env)
    {
        /*
        JNU_ThrowByName(env,
			"java/lang/IllegalArgumentException",
			"ds_env no definido!");
        */         
        printf("DespachadorLocal: No dsh_env!\r\n");fflush(stdout);
        return;
    }

    if ((*dsh_env)->ExceptionCheck(dsh_env)) 
    {
	printf("Excepcion en DESPACHADOR_LOCAL_hebra(%d,%d)-PREVIA->limpiando\r\n",f,entero);fflush(stdout);                
        (*dsh_env)->ExceptionClear(dsh_env);
    } 

    if ((*dsh_env)->PushLocalFrame(dsh_env, 10) < 0) 
    {
        printf("Sin memoria en DESPACHADOR_LOCAL_hebra(%d,%d)\r\n",f,entero);fflush(stdout);                
    }

    if (puntero) 
    {
	if (f == DESPACHO_CCONTROL || f == DESPACHO_CEDIT || f == DESPACHO_DIF_CCONTROL ||
              f == DESPACHO_GETDATACONTROL || f == DESPACHO_MAINCOMMAND || f == DESPACHO_DIF_CEDIT ||
              f == DESPACHO_COMMANDCONTROL || f == DESPACHO_EN_LINEAS ||
              f == DESPACHO_BOTONSALIDA)
        {
           int *px1;
           char *pp = (char *)puntero;
           int n = entero;

		if (f == DESPACHO_CEDIT || f == DESPACHO_DIF_CEDIT)
		{
		  n &= 0xffff0000;
		  n >>= 16;
		  if (n > 0)
			 n--;
		}
	        if (((f == DESPACHO_CCONTROL || f == DESPACHO_DIF_CCONTROL) && entero == -1) || (f == DESPACHO_MAINCOMMAND && entero == ID_DLGINDICES))
	        {
		   /*entero = 0;*/
	           n = 0;
	        }

		tam = 0;
		do
		{
		 px1 = (int *)pp;
		 tam += px1[0];
		 pp += px1[0];
		 if ((f != DESPACHO_CCONTROL && f != DESPACHO_CEDIT && f != DESPACHO_DIF_CCONTROL && f != DESPACHO_DIF_CEDIT) || !n) 
		     break;
		 n--;
		} while(-1);
                len = tam;
        }
	else if (f == DESPACHO_MULTIPLE && (entero == 12 || entero == 13))
	{  
	    tam = len = *((int *)puntero);
	}
	else if (f == DESPACHO_MULTIPLE && (entero == 668))
	{  
	    tam = len = 4;
	}
	else
	{
	   if (f == DESPACHO_PROGRESO)
	   {
		   tam = len = sizeof(int);
		   len += sizeof(long);
	   }
	   else
	   {
		   tam = (strlen((char *)puntero)+1);
		   len = -1;
	   }
	}        
    }

    if (puntero)
    {
            if (len == 0)
                buffer = (*dsh_env)->NewByteArray(dsh_env, 1);
            else if (len == -1)
            {
                if (tam > 2048)
                   buffer = (*dsh_env)->NewByteArray(dsh_env, tam);
                else
                   buffer = (*dsh_env)->NewByteArray(dsh_env, 2048);
            }
            else
	        buffer = (*dsh_env)->NewByteArray(dsh_env, len);
	    if ((*dsh_env)->ExceptionCheck(dsh_env)) 
	    {
		printf("Excepcion en DESPACHADOR_LOCAL_hebra(%d,%d)-0\r\n",f,entero);fflush(stdout);                
	    } 
    }
    else 
        buffer = NULL;
    if (buffer)
    {
        (*dsh_env)->SetByteArrayRegion(dsh_env, buffer, 0, tam,(jbyte *)puntero);
        if ((*dsh_env)->ExceptionCheck(dsh_env))
        {
           printf("Excepcion en DESPACHADOR_LOCAL_hebra(%d,%d)-1\r\n",f,entero);fflush(stdout);                
        }
    }    
    if (sacalog > 1) {
       printf("DespachadorLocal_hebra: llamando .... tam=%ld  len=%ld\r\n",tam,len);fflush(stdout);
    }
    JNU_CallStaticMethodByName(dsh_env,NULL,dsh_obj,"despachadorExterno", "(II[B)V",f,entero,buffer);
    if (sacalog > 1) {
       printf("DespachadorLocal_hebra: saliendo ....\r\n");fflush(stdout);
    }
    if (buffer)
    {
        if (len == -1) {
           if (tam > 2048)
              len = tam;
           else {
               (*dsh_env)->GetByteArrayRegion(dsh_env, buffer, 0, tam,(jbyte *)local_buffer);               
               local_buffer[tam] = 0;
               if (strlen(local_buffer) == tam)
                    (*dsh_env)->GetByteArrayRegion(dsh_env, buffer, 0, 2048,(jbyte *)local_buffer);
 	       if ((*dsh_env)->ExceptionCheck(dsh_env))
               {
		    printf("Excepcion en DESPACHADOR_LOCAL_hebra(%d,%d)-2.\r\n",f,entero);fflush(stdout);                
	       }
               else
                    strcpy((char *)puntero,local_buffer);
           }
        }
        if (len > 0)
        {
           (*dsh_env)->GetByteArrayRegion(dsh_env, buffer, 0, len,(jbyte *)puntero);
 	   if ((*dsh_env)->ExceptionCheck(dsh_env))
	   {
	       printf("Excepcion en DESPACHADOR_LOCAL_hebra(%d,%d)-2\r\n",f,entero);fflush(stdout);                
	   }
        }
    	(*dsh_env)->DeleteLocalRef(dsh_env, buffer);
        if ((*dsh_env)->ExceptionCheck(dsh_env)) 
	{
	   printf("Excepcion en DESPACHADOR_LOCAL_hebra(%d,%d)-3\r\n",f,entero);fflush(stdout);                
	}
    }

    (*dsh_env)->PopLocalFrame(dsh_env, NULL);

    if (sacalog > 1) {
        printf("DespachadorLocal_hebra: OK\r\n");fflush(stdout);
    }
}

/*
JNIEXPORT void JNICALL 
    Java_SysInfo_getenv__Ljava_lang_String_2Ljava_lang_StringBuffer_2 
        (JNIEnv *env, jclass clazz, jstring EVName, jobject EVValue)
{
    // Check Java String object argument (environment variable name) to see 
    // if null reference passed.
    if (EVName == 0)
            return;
    // Check Java StringBuffer object argument (environment variable value) 
    // to see if null reference passed.
    if (EVValue == 0)
            return;
    // Get ASCII representation of environment variable name.
    const char *_EVName = env->GetStringUTFChars (EVName, 0);
    // Get environment variable value.
    const char *_EVValue = getenv (_EVName);
    // Release memory used to hold ASCII representation.
        env->ReleaseStringUTFChars (EVName, _EVName);
    // Return if there is no environment variable value.
    if (_EVValue == 0)
            return;
    // Obtain the Java StringBuffer class handle that corresponds to the 
    // Java StringBuffer object handle.
    clazz = env->GetObjectClass (EVValue);
    // Obtain the method ID for the StringBuffer append method which takes 
    // a StringBuffer object reference argument and returns a String object 
    // reference.
    jmethodID mid = 
        env->GetMethodID (clazz, 
                          "append", 
                                  "(Ljava/lang/String;)Ljava/lang/StringBuffer;");
    // If this method does not exist then return.
    if (mid == 0)
            return;
    // Create a new Java String object and populate it with the environment
    // variable value.  Obtain a handle to this object.
    jstring _jstring = env->NewStringUTF ((const char *) _EVValue);
    // Call the StringBuffer object's append method passing the handle to 
    // the new Java String object.
    env->CallObjectMethod (EVValue, mid, _jstring);
}
*/
