
/* Autor: Francisco Gonzalez Valiente */
#include <fgvsys.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef O_BINARY
#include <io.h>
#endif

#include <stdio.h>
#include <string.h>
#include <malloc.h>

#ifndef O_BINARY  /* compatibilidad xenix - dos */

#define O_BINARY 0
#define SEEK_SET 0
#define SEEK_CUR 1
#ifndef LARGESEEK
extern long rw_lseek_();
#endif
extern unsigned int rw_read_();

#endif


#include <rwmalloc.h>
#define _NUEVODEF
#include <rdef.h>



int graba_xml_defesquema(FI_DEF *pfi,FILE *fout)
{
	int i;
	 
	fprintf(fout,"<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>\r\n");
	fprintf(fout,"<!--  XML Schema DS Def: %s [%s] %s -->\r\n",pfi->f_titulo,quita_path(pfi->f_PathDef),fechas());  
	fprintf(fout,"<xsd:schema xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">\r\n");
	fprintf(fout,"<xsd:annotation>\r\n");
	fprintf(fout,"<xsd:documentation xml:lang=\"es\">XML Schema - Tabla %s (%s) .</xsd:documentation>\r\n",quita_path(pfi->f_PathDef),pfi->f_titulo);
    fprintf(fout,"</xsd:annotation>\r\n");
	fprintf(fout,"<xsd:element name=\"def_%s\">\r\n",quita_path(pfi->f_PathDef));
	fprintf(fout,"<xsd:complexType>\r\n");
	fprintf(fout,"<xs:sequence>\r\n");

	/* ya que los campos de cada def son (desgraciadamente) unicos para el no vamos a definir un tipo
	   para ellos sino como tipos simples standard */

	for (i = 0;i < pfi->f_ncampos;i++)
	{
		fprintf(fout,"<xs:element name=\"C%03d\">\r\n",i);
		fprintf(fout,"<xsd:annotation>\r\n");
		fprintf(fout,"<xsd:documentation xml:lang=\"es\">%s</xsd:documentation>\r\n",pfi->f_campos[i].c_nombre);
		fprintf(fout,"</xsd:annotation>\r\n");
		fprintf(fout,"<xs:simpleType>\r\n");
		switch(pfi->f_campos[i].c_tipo)
		{
		case CHARTIPO:
			fprintf(fout,"<xs:restriction base=\"xs:string\"");
			fprintf(fout,"<xs:minLength value=\"0\"/>\r\n");
			fprintf(fout,"<xs:maxLength value=\"%d\"/>\r\n",pfi->f_campos[i].c_qc);
			fprintf(fout,"</xs:restriction>\r\n");
			break;
	   case FECHATIPO :
		    fprintf(fout,"<xs:restriction base=\"xs:date\"");
			fprintf(fout,"</xs:restriction>\r\n");
			break;
	   case INTTIPO :
		    fprintf(fout,"<xs:restriction base=\"xs:short\"");
			fprintf(fout,"</xs:restriction>\r\n");
		  break;
	   case LONGTIPO :
		    fprintf(fout,"<xs:restriction base=\"xs:integer\"");
			fprintf(fout,"</xs:restriction>\r\n");
		  break;
	   case DOUBLETIPO :
		    fprintf(fout,"<xs:restriction base=\"xs:double\"");
			fprintf(fout,"</xs:restriction>\r\n");
		   break;
		}		
		fprintf(fout,"</xs:simpleType>\r\n</xs:element>\r\n");
	}    

	fprintf(fout,"</xs:sequence>\r\n");
    fprintf(fout,"</xsd:complexType>\r\n");
    fprintf(fout,"</xsd:element>\r\n");
	fprintf(fout,"</xsd:schema>\r\n");
}
