

int pon_campo_en_el_raton(ipfi,ccc,faclin)
int ipfi;
int ccc;
int faclin;
{
   int i,kk,qp,j;
   int campo = v_mpfi(ipfi)->f_campos[ccc-1].c_mante.c_actual % 1000;
   qp = v_mpfi(ipfi)->f_campos[campo].c_mante.c_qp;
   if ((faclin/100) && (v_mpfi(ipfi)->f_campos[campo].c_mante.c_lineal & C_LINEAL))
   {
      qp += (faclin/100-1)*100;
   }
   i = v_mpfi(ipfi)->f_campos[campo].c_mante.c_qpl;
   j = long_campo(v_mpfi(ipfi),campo);
   if (v_mpfi(ipfi)->f_campos[campo].c_tipo != CHARTIPO || i < 1 || i > j)
      i = j;
   return(pon_mapa_raton(qp,qp+i-1,ccc*100+ipfi,0));
}


/*
* entra_datos
* entrada de datos de todos los campos de un fichero
* devuelve -1 si se sale con ctrl-c
* 0 en caso contrario
*/
int entra_datos_old(alta,ipfi,faclin)
int alta;
int ipfi;
int faclin;
{
		 int qj = 1;
         int campini,campfin;
         int panini,panfin;
         int numpan,numcam,ccc,pregucor = -2,i,j;
         int pintar;
         int pintada1;
         int salida=0,estoyen;
         int cerrar; /* si estaba ya en no cerrar no liberar el flag */
         int tipo;
         int altaa;
         int ultimocampo = -1;
         char *buf = NULL;
         double vca,vcf;
         int old_modo_validacion = modo_de_validacion;
         int o_modo_rel;
         int p_p_for_lin = 0;
         int resposicionar = 0;
         int estoy_en_lineas = -1;
         int tmp_qj;
         int avance_especial = 0;
         int sin_pintar = 0;
         
         if (faclin > 99)
         {
            estoy_en_lineas = campo_linea(ipfi) + 1;
         }
         
         
         _M_EXTRA_AVANCE = 0;
         
         recurre_raton(0);
         
         modo_de_validacion = ESC;
         
         if (memoplin > 2) 
         {
            vca = v_memo(5000)->numerica;
            vcf = v_memo(5011)->numerica;
         }
         
         if (faclin < 0) 
         {
             sin_pintar = 2;
             faclin = 0; 
         }
         else
         {
             sin_pintar = 1;
             pinta_datos(ipfi,0);
         }

         
         if (memoplin > 3) {
            altaa = atoi(v_memo(5003)->alfabetica);
            altaa = (altaa % 100) + alta * 100;
            sprintf(v_memo(5003)->alfabetica,"%d",altaa);
         }
         if ((cerrar = f_v(ipfi,RNOCERRAR)) == -1)
            f_s(ipfi,RNOCERRAR);
         tipo = (-alta);
         o_modo_rel = modo_de_relacion;
         modo_de_relacion = 1;
         if (actualiza_rel(ipfi,tipo,0,faclin,&buf) < 0)
         {
            tipo = alta;
            actualiza_rel(ipfi,tipo,-1,faclin,&buf);
            Ar();Bl(4);Cr();   /* borrar mensaje */
            if (cerrar == -1)
               f_n(ipfi,RNOCERRAR);
            if (buf)
            {
               rw_error(77);
            }
            modo_de_validacion = old_modo_validacion;
            modo_de_relacion = o_modo_rel;
            recurre_raton(1);
            if (faclin > 0)
               return(7);
            return(-1);
         }
         modo_de_relacion = o_modo_rel;   
         if (!sin_pintar)
            panfin = v_mpfi(ipfi)->f_npanta;   /* numero de pantallas */
         else
            panfin = 1;
         panini=0;
         
         if (faclin > 99)
         {
            panfin = panini + 1;
         }
         
         
         if (alta == MODIF) 
         { /* poner control de relaciones a ok */
            entra_rel(0,0,alta,FALSO,1,ipfi,6,0);
         }
         
         pintada1 = 1;
         numpan=panini;
         
         
         do 
         {
            if (numpan > panfin)
            {
               estoyen = 0;
               pintada1 = 0;
               pintar = -1;
               numpan=panini;
            }
            while (numpan<panfin) 
            {
               if (_M_EXTRA_AVANCE && avance_especial)
               {
                  no_pintar_ahora(1);
               }
               
               if (numpan < panini || pintada1 == 1) {
                  pintar = 0;           /* no pintar la pantalla otra vez */
                  pintada1 = 0;         /* solo la primera vez no se pinta  */
                  numpan = panini;
                  estoyen = 0;
               }
               else
                  pintar = -1;
         
               
               if (!sin_pintar)
               {
                   campini = v_mpfi(ipfi)->f_pantalla[numpan].p_campoini;
                   campfin = v_mpfi(ipfi)->f_pantalla[numpan].p_campofin;
               }
               else               
               {
                   campini = v_mpfi(ipfi)->f_pantalla[0].p_campoini;  /* campo inicial */
                   campfin = v_mpfi(ipfi)->f_pantalla[v_mpfi(ipfi)->f_npanta-1].p_campofin;  /* campo final */
               }               
               /* poner campos para raton */
               anula_mapa_raton(-1);
               if (!sin_pintar)
               {
                   for (i = campini;i <= campfin;i++)
                   {
                      pon_campo_en_el_raton(ipfi,i,faclin);
                   }
               }              
               
               if (!sin_pintar && pintar) 
               {
                  /*pinppan(ipfi,numpan);                  /* pintar pantalla */
                  /*cabecera_v(rw_version,proceso,titulo,empresa);    /* pintar cabecera */
                  prin_pan(ipfi,numpan);     /* pintar datos pantalla numero numpan */
               }               
               
               
               if (estoyen > numpan) {
                  ccc=campfin;
                  qj = 2;
               }
               else {
                  if (resposicionar)
                  {
                     ccc = p_p_for_lin;
                     resposicionar = 0;
                  }
                  else
                  {
                     ccc=campini;
                  }
                  qj = 0;
               }
               estoyen = numpan;            /* numero de pantalla en la que estamos */
               
               
               if (!_M_EXTRA_AVANCE || !avance_especial)
               {
                  no_pintar_ahora(0);
               }
               if (!sin_pintar && DespachadorExterno)
               {
                   /*
                  if (!_M_EXTRA_AVANCE)
                     (*DespachadorExterno)((int)DESPACHO_QUEPANTALLA,(int)estoyen,v_mpfi(ipfi)->f_titulo);
                     */
               }


               
               while (ccc<=campfin) 
               {
                  if (ccc < campini) 
                  {                      
                     /*numpan -= 2;*/
                     ccc  = campini;
                     break;
                  }
                  p_p_for_lin = ccc;

                  for (i = 0;i < v_mpfi(ipfi)->f_npanta;i++)
                  {
                      if (ccc >= v_mpfi(ipfi)->f_pantalla[i].p_campoini && 
                          ccc <= v_mpfi(ipfi)->f_pantalla[i].p_campofin)
                      {
                          numpan = i;
                          estoyen = i;
                          break;
                      }
                  }

                  numcam = (v_mpfi(ipfi)->f_campos[ccc-1].c_mante.c_actual % 1000) + 1;

                  if (ultimocampo >= ccc) {
                     if (alta != CONSULTA) altaa = MODIF;
                  }
                  else  {
                     ultimocampo = ccc;
                     altaa = alta;
                     if (memoplin > 2) {
                        v_memo(5000)->numerica = (double)(numcam - 1);
                        v_memo(5011)->numerica = (double)ipfi;
                     }
                  }
                  if (numcam == estoy_en_lineas &&  alta == ALTA)
                  {
                     /* nada ! */
                  }
                  else
                  {
                     if (_M_EXTRA_AVANCE)
                     {
                        _M_EXTRA_AVANCE--;
                        if (avance_especial < 0)
                        {
                           ccc++;
                           qj = SOLICITA_RETROCESO;
                        }
                        else
                           if (avance_especial > 0)
                           {
                              qj = SOLICITA_AVANCE;
                              ccc--;
                           }
                     }
                     else
                     {
                        qj = entra_campo(ipfi,numcam,numpan,altaa,faclin,qj,FALSO,0,ccc,alta);
                        if (qj == SOLICITA_INICIO)
                        {
                            for (i = campini-1;i < ccc;i++)
                            {
                                qj = (v_mpfi(ipfi)->f_campos[i].c_mante.c_actual % 1000);
                                if (qj >= 0 && qj < v_mpfi(ipfi)->f_ncampos && !v_mpfi(ipfi)->f_campos[qj].c_mante.c_modif && !v_mpfi(ipfi)->f_campos[qj].c_mante.c_visual && v_mpfi(ipfi)->f_campos[qj].c_key != 1)
                                {
                                    break;
                                }
                            }
                            if (i < ccc)
                            {
                                qj = SOLICITA_POSICION;
                                _ID_Cursor = (i+1)*1000+ipfi;
                            }
                            else
                            {
                                qj = 0;
                                continue;
                            }
                        }
                        else
                        if (qj == SOLICITA_FINAL)
                        {
                            for (i = campfin-1;i >= ccc;i++)
                            {
                                qj = (v_mpfi(ipfi)->f_campos[i].c_mante.c_actual % 1000);
                                if (qj >= 0 && qj < v_mpfi(ipfi)->f_ncampos && !v_mpfi(ipfi)->f_campos[qj].c_mante.c_modif && !v_mpfi(ipfi)->f_campos[qj].c_mante.c_visual && v_mpfi(ipfi)->f_campos[qj].c_key != 1)
                                {
                                    break;
                                }
                            }
                            if (i >= ccc)
                            {
                                qj = SOLICITA_POSICION;
                                _ID_Cursor = (i+1)*1000+ipfi;
                            }
                            else
                            {
                                qj = 0;
                                continue;
                            }
                        }
                     }
                  }
                  avance_especial = 0;
                  if (qj == SOLICITA_POSICION || qj == 4 || qj == 5 || qj == SOLICITA_AVANCE || qj == SOLICITA_RETROCESO)
                  {
                     if (qj == SOLICITA_POSICION)
                     {
                        qj = que_clave_tiene_el_raton(ipfi,&j);
                        if (qj == 1)
                        {
                           pito();
                           qj = 0;
                           _ID_Cursor = -1;
                           continue;
                        }
                        if ((_ID_Cursor%1000) != ipfi)
                        {
                           _ID_Cursor = -1;
                           continue;
                        }
                        i = (_ID_Cursor/1000);
                        qj = (v_mpfi(ipfi)->f_campos[i-1].c_mante.c_actual % 1000);
                        if (v_mpfi(ipfi)->f_campos[qj].c_mante.c_modif)
                        {
                           pito();
                           qj = 0;
                           _ID_Cursor = -1;
                           continue;
                        }
                        _ID_Cursor = -1;
                        qj = 0;
                     }
                     else
                        if (qj == 4 || qj == SOLICITA_RETROCESO)
                        {
                           i = campini - 1;
                           if (qj == SOLICITA_RETROCESO)
                              avance_especial = -1;
                        }
                        else
                           if (qj == 5 || qj == SOLICITA_AVANCE)
                           {
                              i = campfin + 1;
                              if (qj == SOLICITA_AVANCE)
                                 avance_especial = 1;
                           }
                           no_mensaje = 1;
                           if (ccc < i)
                           {
                              /* ha de saltar el actual que ya ha pasado el filtro */
                              for(ccc++;ccc < i;ccc++)
                              {
                                 if (alta == ALTA && ccc > ultimocampo)
                                 {
                                    altaa = alta;
                                    ultimocampo = ccc;
                                 }
                                 j = (v_mpfi(ipfi)->f_campos[ccc-1].c_mante.c_actual % 1000)+1;
                                 
                                 if (numcam == estoy_en_lineas &&  alta == ALTA) continue;
                                 if (memoplin > 4) 
                                 {			
                                    if (qj == SOLICITA_AVANCE)
                                       tmp_qj = 5;
                                    else
                                       if (qj == SOLICITA_RETROCESO)
                                          tmp_qj = 4;
                                       else
                                          if (qj < 0)
                                             tmp_qj = 3;
                                          else
                                             tmp_qj = qj;
                                          sprintf(v_memo(5004)->alfabetica,"%d",tmp_qj);
                                 }
                                 
                                 if (!antes_entra(numpan,j,altaa,FALSO,ipfi,faclin,0))
                                 {
                                    if (tras_entra(numpan,j,altaa,FALSO,0,ipfi,faclin) < 0)
                                    {
                                       i = ccc;
                                       qj = 0;
                                       _M_EXTRA_AVANCE = 0;		              
                                       no_pintar_ahora(0);
                                       if (!sin_pintar && DespachadorExterno)
                                       {
                                           /*
                                          (*DespachadorExterno)((int)DESPACHO_QUEPANTALLA,(int)estoyen,v_mpfi(ipfi)->f_titulo);
                                          */
                                       }
                                       
                                       break;
                                    }
                                 }
                              }
                           }
                           
                           if (ccc > i)
                           {
                              altaa = MODIF;
                              for(ccc--;ccc > i;ccc--)
                              {
                                 j = (v_mpfi(ipfi)->f_campos[ccc-1].c_mante.c_actual % 1000)+1;
                                 if (numcam == estoy_en_lineas &&  alta == ALTA) continue;
                                 if (memoplin > 4) 
                                 {			
                                    if (qj == SOLICITA_AVANCE)
                                       tmp_qj = 5;
                                    else
                                       if (qj == SOLICITA_RETROCESO)
                                          tmp_qj = 4;
                                       else
                                          if (qj < 0)
                                             tmp_qj = 2;
                                          else
                                             tmp_qj = qj;
                                          sprintf(v_memo(5004)->alfabetica,"%d",tmp_qj);
                                 }
                                 if (!antes_entra(numpan,j,altaa,FALSO,ipfi,faclin,0))
                                 {
                                    if (tras_entra(numpan,j,altaa,FALSO,0,ipfi,faclin) < 0)
                                    {
                                       i = ccc;
                                       qj = 0;
                                       _M_EXTRA_AVANCE = 0;
                                       
                                       no_pintar_ahora(0);
                                       if (!sin_pintar && DespachadorExterno)
                                       {
                                           /*
                                          (*DespachadorExterno)((int)DESPACHO_QUEPANTALLA,(int)estoyen,v_mpfi(ipfi)->f_titulo);
                                          */
                                       }
                                       
                                       break;
                                    }
                                 }
                              }
                           }
                           no_mensaje = 0;
                           if (!qj) ccc--;
                          }
                          
                          if (qj == 0 || qj == 3)         /* validar , abajo */
                             ;
                          else if (qj == 2)               /* arriba */
                             ccc -= 2;
                          else if (qj == 4 || qj == SOLICITA_RETROCESO) {             /* pgup   */
                             /*numpan -= 2;
                             break;*/
                          }
                          else if (qj == 5 || qj == SOLICITA_AVANCE)               /* pgdn   */
                             /*break*/;
                          else if (qj == 1) {             /* esc    */
                             numpan = 9999;
                             ccc = 9999;
                             break;
                          }
                          else if (qj == 7) {             /* abort  */
                             numpan = 9999;
                             ccc = 9999;
                             salida = -1;
                             break;
                          }
                          ccc++;
                        }

                        break;
                        /*
                        numpan++;
                        if (!faclin && numpan == panfin)
                        {
                           numpan--; 
                           break;
                        }
                        if (faclin > 0 && numpan < panini) break;
                        */
                }
                
                if (memoplin > 3)
                   sprintf(v_memo(5004)->alfabetica,"%d",qj);
                pregucor = ejecuta_otro(ipfi,12,O_NOR,faclin,0);
				if (pregucor >= 0)
					pregucor = 0;
                if (pregucor == -2 && !salida) /* no hay proceso!! */
                {
                   /* preguntar si-no-anular */
                   
                }
                if (pregucor == -1)
                {
                   if (faclin > 100)
                   {
                      numpan=panini;
                      pintada1 = 1;
                      resposicionar = 1;
                   }
                   else
                   {
                      numpan = 9999;
                   }
                   salida = 0;
                   qj = 1;
                   continue;
                }
                
#ifdef COMENTARIO		
                if (memoplin > 3 && !memcmp(v_memo(5002)->alfabetica+22,"si",2))
                   salida = 1; /* abandono forzado por calculo */
#endif
                
                /* ojo continue ... no poner para salir sino para repetir */
                if (salida || entra_rel(numpan,0,alta,FALSO,1,ipfi,5,faclin) >= 0)
                   break;
        } while(-1);
        
        /* limpiar marcas */
        entra_rel(numpan,0,alta,FALSO,1,ipfi,7,faclin);
        
        if (!salida && v_mpfirel(ipfi)->clave > -1 && faclin == 0)
           /* solo fichas no lineales */
           salida = cheq_clave(ipfi,v_mpfirel(ipfi)->clave,1);
        
        /* conservo el antiguo sistema de correcto aunque nunca se ejecute */
        if (!salida && (v_mpfirel(ipfi)->lineas > -1 || faclin || pregucor != -2 || !(salida = correcto()))) 
        {
           
           if (alta == ALTA)
              defecam_rel(ipfi,-1,numpan,faclin);
           if (v_mpfirel(ipfi)->lineas > -1) {   /* lineas */
              salida = trata_lineas(ipfi,alta,0,0,faclin);
              if (trata_lineas(ipfi,alta,3,0,faclin) != 0)
                 estoyen = panini + 1;
              if (apli_aborto) {salida = -1;}
           }
        }
        
        
        
        if (salida != 1 || alta == MODIF)
        {		
           o_modo_rel = modo_de_relacion;
           modo_de_relacion = 1;
           tipo = actualiza_rel(ipfi,alta,((salida != 1) ? (salida) : (-1)),faclin,&buf);
           modo_de_relacion = o_modo_rel;
           if (!salida) salida = tipo;           		
        }
        
        if (memoplin > 2) 
        {
           v_memo(5000)->numerica = vca;
           v_memo(5011)->numerica = vcf;
        }
        
        if (!sin_pintar && salida != 1 && estoyen != 0) { /* pinto la primera pantalla antes de salir */
           /*pinppan(ipfi,0);*/
           /*cabecera_v(rw_version,proceso,titulo,empresa);*/
           if (alta != ALTA || salida)
              prin_pan(ipfi,0);
        }
        
        anula_mapa_raton(-1);
        
        if (salida == 1) salida = 0;
        
        Ar();Bl(4);Cr();   /* borrar linea de mensajes */
        if (cerrar == -1)
           f_n(ipfi,RNOCERRAR);
        
        if (faclin > 0)
        {
           if (!salida)
              salida = qj;
           else
              salida = (-(qj+2));
        }
        
        if (buf) 
        {
           rw_error(78);
        }
        modo_de_validacion = old_modo_validacion;
        recurre_raton(1);        
        
        
        _M_EXTRA_AVANCE = 0;
        
        return(salida);
}
