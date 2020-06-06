package enlace;

import bibliothek.gui.dock.support.util.Resources;
import java.awt.Color;
import java.awt.Component;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.UnsupportedEncodingException;
import java.util.Vector;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JPasswordField;
import javax.swing.tree.DefaultMutableTreeNode;

import com.google.api.translate.Language;
import com.google.api.translate.Translate;
import java.io.InputStream;
import java.util.Locale;
import java.util.TreeMap;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class DsSysJava {
    //static Hashtable idiomatontable = new Hashtable(10000,(float)0.75);
    static TreeMap idiomatontable[] = new TreeMap[3];
    //static Vector idiomatonlang = new Vector(10000,10000);
    static int idiomatonindex[] = new int[3];
    static String m_path = "";
    private static String m_idioma = "ENG";
    private static boolean traducir = false;
    private static boolean usargoogle = false;
    private static boolean rellenartabla = false;
    private static byte lineabuffer[] = new byte[2048];
    private static Language m_lenguagoogle = Language.ENGLISH;

  /**
   * Tokenizes a String by pattern , splitting it but skipping quotes
   *
   * @param s
   *            String ti be splitted
   * @param pattern
   *            pattern to split string.
   */
    public static String[] split(String s, String pattern) {
        int i = 0;
        int quote = 0;
        Vector v = new Vector();
        do {
            quote = s.indexOf('"');
            i = s.indexOf(pattern);
            if (quote > -1 && quote < i) {
                quote = s.indexOf('"',quote+1);
                if (quote > -1) {
                    i = s.indexOf(pattern,quote+1);
                }
            }
            if (i > -1) {
                v.add(s.substring(0,i));
                s = s.substring(i+1);
            }
            else {
                v.add(s);
                break;
            }
        } while (true);
        return (String[])v.toArray(new String[v.size()]);
    }

    public static String leelinea(InputStream br, boolean trim) {
        String strLine = null;
        int b = -1;
        int i = 0;
        try {
            while(i < lineabuffer.length && (b = br.read()) != -1) {
                if (b == 13)
                    continue;
                if (b == 10)
                    break;
                lineabuffer[i++] = (byte)b;
            }
        } catch (Exception ex) {
            return null;
        }
        if (b == -1 && i == 0)
            return null;
        if (i == 0)
            return "";
        try {
            strLine = new String(lineabuffer, 0, i, "windows-1252");
        } catch (Exception ex) {
            return null;
        }
        if (trim == true) {
            strLine = strLine.trim();
        }
        return strLine;
    }

    @SuppressWarnings("empty-statement")
    private static void CargaIdiomaton(int prioridad,String nombre) {
        FileInputStream fstream = null;
        try {
            idiomatontable[prioridad].clear();
            idiomatontable[prioridad] = null;
        } catch(Exception e) {
            e.printStackTrace();
        }
        idiomatontable[prioridad] = new TreeMap();
        idiomatonindex[prioridad] = 0;
        TreeMap relacion = new TreeMap();
        try {            
            String idioma = m_idioma+":";
            String linea = null;
            fstream = new FileInputStream(m_path + ".dsj"+File.separator+nombre+".dslang");
            int i,ini;
            while((linea = leelinea(fstream,false)) != null) {
                if (linea.trim().length() < 2)
                    continue;
                ini = linea.indexOf(':',0);
                if (ini > -1) {
                    char c;
                    int idx = 0;
                    int potencia = 1;
                    ini++;
                    i = linea.indexOf(':',ini);
                    String dato = linea.substring(i+1);
                    while(i > ini) {
                        i--;
                        idx += ((linea.charAt(i) - '0') * potencia);
                        potencia *= 10;
                    }
                    if (idx > 0 && ini > 0) {
                        Integer Idx = new Integer(idx);
                        if (ini == 1) {
                            if (idx > idiomatonindex[prioridad])
                                idiomatonindex[prioridad] = idx;
                            try {
                                idiomatontable[prioridad].put(dato, Idx);
                                relacion.put(Idx,dato);
                            } catch(Exception e) {
                                e.printStackTrace();
                            }
                        }
                        else {
                            if (linea.startsWith(idioma) && !dato.isEmpty()) {
                                 try {
                                    Object o = relacion.get(Idx);
                                    idiomatontable[prioridad].put(o, dato);
                                  } catch (Exception ex) {
                                      ex.printStackTrace();
                                  }
                                /*
                                 try {
                                      for(i = idiomatonlang.size();i <= idx;i++)
                                          idiomatonlang.add("");
                                      idiomatonlang.set(idx, dato);
                                  } catch (Exception ex) {
                                      ex.printStackTrace();
                                  }
                                 * */
                            }
                        }
                    }
                }
            }
            if (fstream != null) {
                fstream.close();
                fstream = null;
            }
            if (prioridad == 0)
                traducir = true;
        } catch (Exception ex) {
            fstream = null;
            if (prioridad == 0) {
                if (rellenartabla)
                    traducir = true;
            }
        }
        try {
            relacion.clear();
            relacion = null;
        } catch(Exception e) {
            e.printStackTrace();
        }
    }

    public static void setPathIdioma(String path, String idioma,boolean google,boolean rellenar) {
        if (!path.endsWith(File.separator))
            path = path + File.separator;
        DsSysJava.m_path = path;
        DsSysJava.m_idioma = idioma;
        usargoogle = google;
        rellenartabla = rellenar;
        idiomatontable[0] = new TreeMap();
        idiomatontable[1] = new TreeMap();
        idiomatontable[2] = new TreeMap();
        idiomatonindex[0] = 0;
        idiomatonindex[1] = 0;
        idiomatonindex[2] = 0;
        try {
            idioma = idioma.toLowerCase();
            for (Locale l:Locale.getAvailableLocales()) {
                if(idioma.equals(l.getISO3Language())) {
                    m_view.setPais(l.getLanguage().toLowerCase(),false);
                    if (usargoogle) {
                        //Locale salva = (Locale)Locale.getDefault().clone();
                        //Locale.setDefault(new Locale("en_US"));
                        m_lenguagoogle = Language.fromString(l.getLanguage().toLowerCase());//Language.valueOf(l.getDisplayLanguage().toUpperCase());
                        //Locale.setDefault(salva);
                    }
                    break;
                }
            }
        } catch(Exception e) {
            e.printStackTrace();
        }        
        CargaIdiomaton(0,"definition");
        if (traducir) {
            Resources.bundle =  new DsResourceBundleFilter();
            try {
                Translate.setHttpReferrer("http://www.diagram.es");
            }
            catch (Exception e) {
                e.printStackTrace();
                usargoogle = false;
            }
        }
    }

    private static String setAlternateTraduction(String codigo, int prioridad) {
        if (traducir) {
            CargaIdiomaton(prioridad,codigo);
        }
        return codigo;
    }

    public static String Idiomaton(String s) {
        try {
            if (traducir) {
                String sc = s.replaceAll("[^a-zA-Z]","");
                if (sc.isEmpty())
                    return s;                
                String escapes[] = (" "+s).split("\\'\\'");
                String se = escapes[0].substring(1);
                for (int i = 1;i < escapes.length;i++) {
                    if ((i%2) == 0)
                        se = se + escapes[i];
                    else
                        se = se + "[*]";
                }
                if (se.equals("[*]"))
                    return s;
                
                int comodin = 0;
                String sout;
                boolean alfichero = true;
                Matcher mmn = Pattern.compile("[0-9]+").matcher(se);
                se = mmn.replaceAll("[#]");

                Object obj = null;
                for(int prioridad = 2;prioridad >= 0;prioridad--) {
                    if (idiomatontable[prioridad].size() == 0)
                        continue;
                    obj = idiomatontable[prioridad].get(se);
                    if (obj == null) { // probemos un comodin
                        for (int nn = se.length()-1;nn > 1;nn--) {
                            if (se.charAt(nn) == ' ') {
                                obj = idiomatontable[prioridad].get(se.substring(0,nn) + "[*]");
                                if (obj != null) {
                                    comodin = nn;
                                    break;
                                }
                                while(nn > 1 && se.charAt(nn) == ' ') nn--;
                            }
                        }
                        if (obj == null && se.charAt(se.length()-1) != ' ') {
                            obj = idiomatontable[prioridad].get(se+"[*]");
                            if (obj != null) {
                                comodin = se.length();
                            }
                        }
                    }
                    if (obj != null && obj instanceof String) {
                        break;
                    }
                }

                int idx = 0;
                if (obj == null) {
                    idiomatonindex[0]++;
                    idx = idiomatonindex[0];
                    idiomatontable[0].put(se, new Integer(idiomatonindex[0]));
                    if (rellenartabla) {
                        try {
                            sout = String.format(":%d:%s\r\n",idiomatonindex[0],se);
                            FileOutputStream fos = new FileOutputStream(m_path + ".dsj"+File.separator+"definition.dslang", true);
                            fos.write(sout.getBytes("windows-1252")); //"windows-1252"
                            fos.close();                            
                        } catch (Exception ex) {
                            Logger.getLogger(DsSysJava.class.getName()).log(Level.SEVERE, null, ex);
                            alfichero = false;
                        }
                    }
                }
                else {
                    if (obj instanceof String)
                        idx = -1;
                    else if (obj instanceof Integer)
                        idx = ((Integer)obj).intValue();
                }
                if (idx != 0) {
                     String translatedText = null;
                     if (idx == -1) {
                         translatedText = (String)obj;
                     }
                     /*
                     try {
                        translatedText = (String)idiomatonlang.get(idx);
                     } catch (Exception ex) {
                        translatedText = null;
                     }
                     */
                     if (translatedText == null/* || translatedText.trim().isEmpty()*/) {
                         String totranslate = se;
                         if (comodin > 0) {
                             try {
                                totranslate = se.substring(comodin);
                             } catch(Exception e) {
                             }
                         }

                         if (usargoogle) {                             
                             try {
                                m_view.showBusyAnimation();
                              } catch (Exception ex) {
                                  ex.printStackTrace();
                              }
                             try {
                                  translatedText = Translate.execute(totranslate,Language.SPANISH, m_lenguagoogle);
                              } catch (Exception ex) {
                                  ex.printStackTrace();
                              }
                             try {
                                  m_view.stopBusyAnimation();
                              } catch (Exception ex) {
                                  ex.printStackTrace();
                              }
                         }
                         if (comodin > 0)
                             totranslate += "[*]";
                         if (translatedText == null || translatedText.trim().isEmpty()) {
                             translatedText = se;
                             comodin = 0;
                         }
                         else {
                             if (rellenartabla && alfichero && idx > 0) {
                                try {
                                    sout = String.format("%s:%d:%s\r\n",m_idioma,idx,translatedText);
                                    FileOutputStream fos = new FileOutputStream(m_path + ".dsj"+File.separator+"definition.dslang", true);
                                    fos.write(sout.getBytes("windows-1252"));
                                    fos.close();
                                } catch (Exception ex) {
                                    Logger.getLogger(DsSysJava.class.getName()).log(Level.SEVERE, null, ex);
                                }
                             }
                         }
                         try {
                             idiomatontable[0].put(totranslate,translatedText);
                             
                           /*
                              for(int i = idiomatonlang.size();i <= idx;i++)
                                  idiomatonlang.add("");
                              idiomatonlang.set(idx, translatedText);
                           */
                          } catch (Exception ex) {
                              ex.printStackTrace();
                          }
                     }
                     if (comodin > 0) {
                         try {
                              translatedText = translatedText + se.substring(comodin);
                         } catch(Exception e) {
                         }
                     }
                     mmn.reset();
                     try {
                         while (mmn.find()) {
                            translatedText = translatedText.replaceFirst("\\[\\#\\]",mmn.group());
                         }
                     } catch(Exception e) {
                     }
                     try {
                         for(int i = 1;i < escapes.length;i+=2) {
                            translatedText = translatedText.replaceFirst("\\[\\*\\]","''"+escapes[i]+"''");
                         }
                     } catch(Exception e) {
                     }                     
                     s = translatedText;
                }
            }
        }
        catch (Exception e) {
            e.printStackTrace();
        }
        return s;
    }


    /* funciones del basico nativo */
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
    private native int fDespachadorInterno(int funcion,byte[] buffer);    
    private native int fenhebra(int fun,int arg);


    /* utiles en c ... */
    /*
     * Mas eficaz en puro java que el coste de la llamada
    public native int memcpyInt(byte[] buffer,int arg);
     * DsSysJava.memSet(buffer,0,4,4,buffer.length);
     */
    public native void memset(byte[] buffer,int valor,int tam,int ini,int fin);

    public static final int SIZEINT = 4;
    public static final int SIZELONG = 4;
    public static final int SIZESHORT = 2;
    
    public static final int SIZEgrid_filtro = 916;

    
    /* Instancia unica, sin accesor ... para pruebas :) */
    public static DsSysJava dssys = null;

    /* constantes del basico nativo */
    public static final int IDOK = 1;
    public static final int IDCANCEL = 2;
    public static final int IDABORT = 3;
    public static final int IDRETRY = 4;
    public static final int IDIGNORE = 5;
    public static final int IDYES = 6;
    public static final int IDNO = 7;
    public static final int IDCLOSE = 8;
    public static final int IDHELP = 9;

    public static final int MAXMENUA=15;
    /* a partir de 1000 los codigos de teclado son para funciones especiales */
    public static final int MID_XNOTECLAS=1000;
    public static final int MID_MENUSG   =1000;
    public static final int MID_DISENO   =1100;
    public static final int MID_XPRIMERA =8000;
    public static final int MID_XOPCION1=18001;
    public static final int MID_XARXA   =28567;

    /* ObtieneMenu */
    public static final int DESPACHO_OMENU				=4001;
    /* EnMenu */
    public static final int DESPACHO_EMENU				=4002;
    /* Pantallas (pestañas) */
    public static final int DESPACHO_PANTALLAS           =  4003;
    /* Mueve pantalla */
    public static final int DESPACHO_TITULO               = 4004;
    /* comando del menu del terminal o interno */
    public static final int DESPACHO_MAINCOMMAND           =4005;
    /* actualiza los 'accesos directos' */
    public static final int NUEVA_TECLA_PROGRAMADA         =4006;
    /* indica que aparezcan las barras de scroll*/
    public static final int DESPACHO_EN_LINEAS             =4007;
    /* crear control en el cliente*/
    public static final int DESPACHO_MI_CONTROL            =4008;
    public static final int DESPACHO_FIN_CONTROL           =4009;
    public static final int DESPACHO_PRE_CONTROL           =4010;
    public static final int DESPACHO_SOBRE_PANTALLA        =4011;
    public static final int DESPACHO_REPON_PANTALLA        =4012;
    public static final int DESPACHO_MENSAJE_CABECERA      =4013;
    public static final int DESPACHO_FUNCIONES_IMPRESION   =4014;
    public static final int DESPACHO_BOTONSALIDA           =4015;
    public static final int DESPACHO_COMMANDCONTROL        =4016;
    public static final int DESPACHO_TRASMISION            =4017;
    public static final int DESPACHO_PROGRESO              =4018;
    public static final int DESPACHO_USUARIOS			=4019;
    public static final int DESPACHO_INICIO_TRAZA		=4020;
    public static final int DESPACHO_FIN_TRAZA			=4021;
    public static final int DESPACHO_PUNTO_TRAZA		=4022;
    public static final int DESPACHO_BINARIO    		=4023;
    public static final int DESPACHO_PROCESAMENSAJES     =  4024;
    public static final int DESPACHO_CCONTROL             = 4025;
    public static final int DESPACHO_CEDIT                 =4026;
    public static final int DESPACHO_GETDATACONTROL        =4027;
    public static final int DESPACHO_FOCOCONTROL           =4028;
    public static final int DESPACHO_DESTRUYECONTROL       =4029;
    public static final int DESPACHO_REFRESCACONTROL       =4030;
    public static final int DESPACHO_FOCOWINDOW            =4031;
    public static final int DESPACHO_TRAEFICHERO           =4032;
    public static final int DESPACHO_ENLACEWORD            =4033;
    public static final int DESPACHO_LISTADO               =4034;
    public static final int DESPACHO_BROWSE                =4035;
    public static final int DESPACHO_ESTADOCONTROL         =4036;
    public static final int DESPACHO_EURO                  =4037;
    public static final int DESPACHO_SUPERUSUARIO          =4038;
    public static final int DESPACHO_MENUITEM              =4039;
    public static final int DESPACHO_CONSULTA_REMOTA	=4040;
    public static final int DESPACHO_MULTIPLE             = 4041;
    public static final int DESPACHO_DIF_CCONTROL         = 4042;
    public static final int DESPACHO_DIF_CEDIT             =4043;
    public static final int DESPACHO_DIF_FOCOCONTROL       =4044;
    public static final int DESPACHO_DATA_PROCESAMENSAJES=4045;
    public static final int DESPACHO_DATA_INFO			=4046;
    public static final int DESPACHO9_OMENU              =   4501;
    public static final int DESPACHO9_EMENU               =  4502;
    public static final int DESPACHO9_PANTALLAS           =  4503;
    public static final int DESPACHO9_TITULO              =  4504;
    public static final int DESPACHO9_QUEPANTALLA         =  4505;
    public static final int NUEVA9_TECLA_PROGRAMADA       =  4506;
    public static final int DESPACHO9_EN_LINEAS           =  4507;
    public static final int DESPACHO9_MI_CONTROL          =  4508;
    public static final int DESPACHO9_FIN_CONTROL         =  4509;
    public static final int DESPACHO9_PRE_CONTROL         =  4510;
    public static final int DESPACHO9_SOBRE_PANTALLA      =  4511;
    public static final int DESPACHO9_REPON_PANTALLA      =  4512;
    public static final int DESPACHO9_MENSAJE_CABECERA     = 4513;
    public static final int DESPACHO9_FUNCIONES_IMPRESION  = 4514;
    public static final int DESPACHO9_HAZ_VISIBLE          = 4515;
    public static final int DESPACHO9_EDITA                = 4516;
    public static final int DESPACHO9_TRASMISION           = 4517;
    public static final int DESPACHO9_PROGRESO             = 4518;
    public static final int DESPACHO9_USUARIOS             = 4519;
    public static final int DESPACHO9_INICIO_TRAZA         = 4520;
    public static final int DESPACHO9_FIN_TRAZA            = 4521;
    public static final int DESPACHO9_PUNTO_TRAZA          = 4522;
    public static final int DESPACHO9_BINARIO              = 4523;
    public static final int DESPACHO9_PRETRANSLATE         = 4524;
    public static final int DESPACHO9_TRAEFICHERO          = 4532;
    public static final int DESPACHO9_ENLACEWORD           = 4533;
    public static final int DESPACHO9_BROWSE               = 4535;
    public static final int DESPACHO9_ESTADOCONTROL        = 4536;
    public static final int DESPACHO9_EURO                 = 4537;
    public static final int DESPACHO9_SUPERUSUARIO         = 4538;

    public static final int ID_RECORD_UPDATE             =  32828;
    public static final int ID_RECORD_NEW                =  32829;
    public static final int ID_RECORD_DELETE             =  32830;
    public static final int ID_RECORD_FIRST              =  0xE900;
    public static final int ID_RECORD_LAST               =  0xE901;
    public static final int ID_RECORD_NEXT               =  0xE902;
    public static final int ID_RECORD_PREV               =  0xE903;

    public static final int ID_SIONO                     =  32831;
    public static final int ID_SIONOCANCELAR             =  32832;
    public static final int ID_MESSAGEBOX                =  32833;
    public static final int ID_PREGUCLAVE                =  32834;
    public static final int ID_ERRORFICHERO              =  32835;
    public static final int ID_DLGKACTIVA                =  32836;
    public static final int ID_DLGINDICES                =  32837;
    public static final int ID_PTECS                     =  32845;
    public static final int ID_TESTCRYSTAL              =  32929;

    public static final int ID_CUSTOM_ESTADO             =  32861;
    public static final int ID_COMMANDCONTROL            =  32862;

    public static final int ID_TICSCON                   =  32896;
    public static final int ID_AVISOGLOBO                =  32897;

    public static final int ID_PESTANA                   =  32913;
    
    public static final int DISPARO_CONTROL               = 63200;
    /* .................................... 632XX, XX=subnumero del control 633XX siguiente control*/
    public static final int MAX_DISPARO_CONTROL           = 63600;
    public static final int PREDISPARO_CONTROL            = 63600;
    /* .................................... 632XX, XX=subnumero del control 633XX siguiente control*/
    public static final int PREMAX_DISPARO_CONTROL        = 64000;
    public static final int BUSCA_PTECS                   = 64000;
    public static final int PIDO_TRASMISION               = 64001;
    public static final int PIDO_USUARIOS                 = 64002;
    public static final int PIDO_EMPRESA                  = 64003;
    public static final int TECLA_PING                    = 64004;
    public static final int TECLA_COMBO                  =  64005;
    public static final int TECLA_SUPERUSUARIO           =  64006;
    public static final int PIDO_VEREURO                 =  64007;
    public static final int PIDO_SUPERBLOQUEO            =  64008;
    public static final int PIDO_SUPERDESBLOQUEO         =  64009;
    public static final int PIDO_SUPERCAMBIO             =  64010;
    public static final int PIDO_SUPERTODO               =  64011;
    public static final int PIDO_SUPERDESTODO            =  64012;
    public static final int TECLA_CHECK                  =  64013;
    public static final int TECLA_DOBLECLICK             =  64014;
    public static final int ID_BUSCON					=64015;
    public static final int ID_ACTIVAR_ERP				=64016;
    public static final int ID_ACTIVAR_EVT              =   64017;
    public static final int TECLA_HINT					=64018;
    public static final int ID_ACTIVAR_EVTPBX            =  64019;
    public static final int ID_ACTIVAR_EVTCRM             = 64020;

    public static final int ID_SIGUIENTEPANE              =  32846;
    public static final int ID_ANTERIORPANE               =  32847;


    /* Gestion de teclas */

    /* internas de la clase */
    private static boolean m_BasicoCargado = false;

    /* de uso en el cliente java */
    private static String m_dll;
    private static Component m_frame = null;
    /* a modificar segun donde se incluya ... de momento no es generica */
    private static DsSysView m_view = null;

    private static JPanel m_dlgview = null;

    /* metodos */
    
    public DsSysJava() {
        cargaBasico();        
    }

    public static void setFrame(Component frame) {
        DsSysJava.m_frame = frame;
    }

    public static Component getFrame() {
        return m_frame;
    }

    public static void setView(DsSysView m_view) {
        DsSysJava.m_view = m_view;
    }

    public static DsSysView getView() {
        return m_view;
    }

    public static void setDlgView(JPanel dlgview) {
        DsSysJava.m_dlgview = dlgview;
    }

    public static JPanel getDlgView() {
        return m_dlgview;
    }

    public static void setDll(String dll) {
        DsSysJava.m_dll = dll;
    }

/*
    public static String getPath() {
        return DsSysJava.m_path;
    }
*/
    static public Object creaNodoTree(Object padre,int parametro,byte[] buffer) {
        if (parametro == -2)
        {
            m_view.refrescaTree(padre,buffer);
            return null;
        }
        boolean root = false;
        if (parametro == -1) {
            root = true;
            parametro = 0;
        }
        String s = getCString(buffer,0,false,-1, false);
        String s2 = "";
        int i = s.lastIndexOf(7);
        if (i >= 0) {
            s2 = s.substring(i+1);
            s = s.substring(0,i);
        }
        TreeDato treedato = new TreeDato(Idiomaton(s),parametro);
        treedato.setOpcion(s2);
        DefaultMutableTreeNode nodo = new DefaultMutableTreeNode(treedato);

        if (root == true)
        {            
            return m_view.setLeftTree(nodo);
        }
        ((DefaultMutableTreeNode)padre).add(nodo);
        return nodo;
    }


    static public void despachadorExterno(int codigo,int parametro,byte[] buffer) {
        try  {
            switch (codigo) {
                case DESPACHO_DATA_PROCESAMENSAJES:
                case DESPACHO_PROCESAMENSAJES:
                    m_view.procesaMensajes(parametro,buffer,(codigo == DESPACHO_DATA_PROCESAMENSAJES));
                    //JOptionPane.showMessageDialog(m_frame, "DESPACHO_PROCESAMENSAJES " + parametro,"NO IMPLEMENTADO",JOptionPane.ERROR_MESSAGE);
                    break;
                case DESPACHO_BINARIO:
                    m_view.ControlBinario(parametro,buffer);
                    break;
                case DESPACHO_MI_CONTROL:
                    m_view.ControlUsuario(parametro,buffer);
                    break;
               case DESPACHO_DIF_CCONTROL:
               case DESPACHO_CCONTROL:
                    m_view.InsertaControl(parametro,buffer);
                    break;
                case DESPACHO_DIF_CEDIT:
                case DESPACHO_CEDIT:
                    m_view.InsertaEdit(parametro,buffer);
                    break;
                case DESPACHO_DESTRUYECONTROL:
                case DESPACHO_FIN_CONTROL:
                    m_view.DestruyeControl(parametro,buffer);
                    break;
                case DESPACHO_ESTADOCONTROL:
                    m_view.EstadoControl(parametro,buffer);
                    break;
                case DESPACHO_FOCOWINDOW:
                case DESPACHO_FOCOCONTROL:
                case DESPACHO_DIF_FOCOCONTROL:
                    m_view.FocoControl(parametro,buffer);
                    break;
                case DESPACHO_PRE_CONTROL:
                    // no aplicable al X
                    break;
                case DESPACHO_GETDATACONTROL:
                    m_view.GetDataControl(parametro, buffer,false);
                    break;
                case DESPACHO_REFRESCACONTROL:
                    m_view.RefrescaControl(parametro);
                    break;
                case DESPACHO_MAINCOMMAND:
                    m_view.MainCommand(parametro, buffer);
                    break;
                case DESPACHO_SUPERUSUARIO:
                    switch(parametro) {
                        case 4:
                        {
                            String c;
                            JPasswordField pwd = new JPasswordField(10);
                            int action = PidePassword.showPasswordDialog(m_frame, pwd,"","",DsSysJava.Idiomaton("Entre la clave"));
                            //int action = JOptionPane.showConfirmDialog(m_frame, pwd,"Entre la clave",JOptionPane.OK_CANCEL_OPTION);
                            if (action >= 0) {
                                c = String.format("%-10.10s", new String(pwd.getPassword()));
                                DsSysJava.putCString(buffer, 0, c, 11);
                            }
                            else {
                                buffer[0] = (char)1;
                                buffer[1] = (char)0;
                            }
                        }
                            break;
                    }
                    break;
                case DESPACHO_TITULO:
                    m_view.setTitle(getCString(buffer,0));
                    break;
                case DESPACHO_MENSAJE_CABECERA:
                    if (buffer == null || buffer.length == 0)
                    {
                       switch(parametro) {
                        case 0:
                            m_view.setMensa("");
                            break;
                        case 1:
                            m_view.setStatusMensa("");
                            break;
                        case 2:
                            m_view.setStatusRelacion("");
                            break;
                        case 3:
                            m_view.setStatusUsuario("");
                            break;
                        }
                    }
                    else  switch(parametro) {
                        case 0:
                            m_view.setMensa(getCString(buffer,0));
                            break;
                        case 1:
                            m_view.setStatusMensa(getCString(buffer,0));
                            break;
                        case 2:
                            m_view.setStatusRelacion(getCString(buffer,0));
                            break;
                        case 3:
                            m_view.setStatusUsuario(getCString(buffer,0,false));
                            break;
                    }
                    break;
                case DESPACHO_OMENU:
                    // rellenar el menu ... realmente no se hace aqui, esto no se debe llamar nunca
                    break;
                case DESPACHO_EMENU:
                    /* si estamos en menu o no, varios modos segun pantalla 9, no aplicables */
                    m_view.enMenu(parametro);
                    break;
                case DESPACHO_PANTALLAS:
                    parametro %= 10000;
                    m_view.controlDePantallas(parametro,buffer);
                    break;
               case DESPACHO_DATA_INFO:
                   switch(parametro)
                   {
                   case 0:
                       m_view.setNomApli(setAlternateTraduction(getCString(buffer,0,false),1));
                       break;
                   case 1:
                       m_view.setNomTab(setAlternateTraduction(getCString(buffer,0,false),2));
                       break;
                   case 10:
                       m_view.setNueve();
                       break;
                   case 11:
                       m_view.clearNueve();
                       break;
                   default:
                       break;
                   }
                   break;
                case DESPACHO_EURO:
                   break;
                case DESPACHO_PROGRESO:
                   m_view.Progreso(parametro,buffer);
                   break;
                default:
                   m_view.DespachoExterno(codigo,parametro,buffer);
                   break;
            }
        } catch(Exception e) {
            e.printStackTrace();            
        }
        return;
    }

    public static int llamaDespachadorInterno(int funcion,byte[] buffer) {
        return dssys.fDespachadorInterno(funcion,buffer);        
    }

    public static int llamafPreInicializa(int argc,String argv[]) {
        if (m_BasicoCargado == false) {
            dssys = new DsSysJava();
        }
        return dssys.fPreInicializa(argc,argv);
    }

    public static int llamafInicializa(int argc,String argv[]) {
        return dssys.fInicializa(argc,argv);
    }

    public static int llamafLogin(String usuario,String password,String cambio) {
        return dssys.fLogin(usuario, password, cambio);
    }

    public static int llamafOpcion() {
        return dssys.fOpcion();
    }

    public static String PreparaEjecucion(String exe,String tmp,String user,String pass,int opcion) {
        return dssys.fPreparaEjecucion(exe,tmp,user,pass,opcion);
    }

    public static int llamafenhebra(int fun,int arg) {
        return dssys.fenhebra(fun,arg);
    }

    private void cargaBasico() {        
        if (m_BasicoCargado == false) {           
           try {
                System.load(m_dll); //m_dirso + "diagramx.lx.so"
           } catch(Exception e) {
               e.printStackTrace();
               JOptionPane.showMessageDialog(m_frame,DsSysJava.Idiomaton("No se puede cargar el complemento diagramx. ")+e.toString());
               return;   
           }
           m_BasicoCargado = true;
        }
    }

    /* utilidades para mayormente para tratar con el basico */
    public static int unsignedByteToInt(byte b) {
        return (int) b & 0xFF;
        }

    public static int atoi(byte buffer[],int pos,int len) {
        int r = 0;
        int potencia = 1;
        while(len > 0) {
            len--;
            r += (((int)buffer[pos+len] - 48) * potencia);
            potencia *= 10;
        }
        return r;
     }
    
    public static short getShort(byte buffer[],int pos) {
        short ch1 = (short) (buffer[pos++] & 0xFF);
        short ch2 = (short) (buffer[pos] & 0xFF);
        ch2 <<= 8;
        ch1 += ch2;
        return ch1;
     }

    public static void putShort(byte buffer[],int pos,int arg) {
        buffer[pos++] = (byte)((arg >>>  0) & 0xFF);
        buffer[pos] = (byte)((arg >>>  8) & 0xFF);
     }

    public static void putInt(byte buffer[],int pos,int arg) {
        buffer[pos++] = (byte)((arg >>>  0) & 0xFF);
        buffer[pos++] = (byte)((arg >>>  8) & 0xFF);
        buffer[pos++] = (byte)((arg >>> 16) & 0xFF);
        buffer[pos] = (byte)((arg >>> 24) & 0xFF);        
     }
    
    public static int getInt(byte buffer[],int pos) {
        int ch1 = buffer[pos++] & 0xFF;
        int ch2 = buffer[pos++] & 0xFF;
        int ch3 = buffer[pos++] & 0xFF;
        int ch4 = buffer[pos] & 0xFF;
        return ((ch4 << 24) + (ch3 << 16) + (ch2 << 8) + (ch1 << 0));
     }

    public static int [] getIntArray(byte buffer[],int pos,int tam) {
        int r[] = new int[tam];
        for (int i = 0;i < tam;i++) {
            if ((pos+4) > buffer.length) {
                r[i] = 0;
            }
            else
                r[i] = getInt(buffer,pos);
            pos += SIZEINT;
        }
        return r;
    }

    public static String quita_path(String path) {
        int n1 = path.lastIndexOf("/");
        int n2 = path.lastIndexOf("\\");
        if (n2 > n1)
            return path.substring(n2+1);
        else
            return path.substring(n1+1);
    }


    public static String getCString(byte[] buffer, int pos,boolean traducir) {
        return getCString(buffer,pos,true,-1,traducir);
    }

    public static String getCString(byte[] buffer, int pos, int max,boolean traducir) {
        return getCString(buffer,pos,true,max,traducir);
    }
    
    public static String getCString(byte[] buffer, int pos) {
        return getCString(buffer,pos,true,-1,true);
    }

    public static String getCString(byte[] buffer, int pos, int max) {
        return getCString(buffer,pos,true,max,true);
    }

    public static String getCString(byte[] buffer, int pos, boolean oem, int max) {
        return getCString(buffer, pos, oem, max, true);
    }

    public static String getCString(byte[] buffer, int pos, boolean oem, int max, boolean traducir) {
        String s;
        //byte[] tmp;
        if (max == -1)
            max = buffer.length;
        else
            max += pos;
        int i = pos;
        while(i < buffer.length && i < max && buffer[i] != 0)
            i++;
        int len = i-pos;
        if (len == 0)
            return new String("");
        /*
        tmp = new byte[i-pos];
        for (i = 0;i < len;i++) {
                tmp[i] = buffer[pos+i];
        }
        */
        try {
            //s = new String(buffer,pos,i-pos);            
            if (oem == true) {
                int cc;
                byte tmp[] = new byte[len];
                for (i = 0;i < len;i++) {
                   cc = (0x000000FF & ((int)buffer[pos+i]));
                   switch (cc)
                   {
                   case     130 :tmp[i]=(byte)233;break;
                   case     129 :tmp[i]=(byte)252;break;
                   case     160 :tmp[i]=(byte)225;break;
                   case     161 :tmp[i]=(byte)237;break;
                   case     162 :tmp[i]=(byte)243;break;
                   case     253 :tmp[i]=(byte)243;break;
                   case     163 :tmp[i]=(byte)250;break;
                   case     164 :tmp[i]=(byte)241;break;
                   case     165 :tmp[i]=(byte)209;break;
                   case     166 :tmp[i]=(byte)170;break;
                   case     167 :tmp[i]=(byte)186;break;
                   case     168 :tmp[i]=(byte)191;break;
                   case     239 :tmp[i]=(byte)146;break;
                   case     241 :tmp[i]=(byte)177;break;
                   case     246 :tmp[i]=(byte)247;break;
                   case     128 :tmp[i]=(byte)199;break;
                   case     131 :tmp[i]=(byte)226;break;
                   case     132 :tmp[i]=(byte)228;break;
                   case     133 :tmp[i]=(byte)224;break;
                   case     134 :tmp[i]=(byte)229;break;
                   case     135 :tmp[i]=(byte)231;break;
                   case     136 :tmp[i]=(byte)234;break;
                   case     137 :tmp[i]=(byte)235;break;
                   case     138 :tmp[i]=(byte)232;break;
                   case     139 :tmp[i]=(byte)239;break;
                   case     140 :tmp[i]=(byte)238;break;
                   case     141 :tmp[i]=(byte)236;break;
                   case     142 :tmp[i]=(byte)196;break;
                   case     143 :tmp[i]=(byte)197;break;
                   case     144 :tmp[i]=(byte)201;break;
                   case     145 :tmp[i]=(byte)230;break;
                   case     146 :tmp[i]=(byte)198;break;
                   case     147 :tmp[i]=(byte)244;break;
                   case     148 :tmp[i]=(byte)246;break;
                   case     149 :tmp[i]=(byte)242;break;
                   case     150 :tmp[i]=(byte)251;break;
                   case     151 :tmp[i]=(byte)249;break;
                   case     152 :tmp[i]=(byte)255;break;
                   case     153 :tmp[i]=(byte)214;break;
                   case     154 :tmp[i]=(byte)220;break;
                   case     155 :tmp[i]=(byte)248;break;
                   case     156 :tmp[i]=(byte)163;break;
                   case     157 :tmp[i]=(byte)216;break;
                   case     158 :tmp[i]=(byte)215;break;
                   case     159 :tmp[i]=(byte)131;break;
                   case     169 :tmp[i]=(byte)174;break;
                   case     170 :tmp[i]=(byte)172;break;
                   case     171 :tmp[i]=(byte)189;break;
                   case     172 :tmp[i]=(byte)188;break;
                   case     173 :tmp[i]=(byte)161;break;
                   case     174 :tmp[i]=(byte)171;break;
                   case     175 :tmp[i]=(byte)187;break;
                   case     176 :tmp[i]=(byte)128;break;
                   case     177 :tmp[i]=(byte)20; break;
                   case     178 :tmp[i]=(byte)21; break;
                   case     179 :tmp[i]=(byte)124;break;
                   case     180 :tmp[i]=(byte)22; break;
                   case     181 :tmp[i]=(byte)193;break;
                   case     182 :tmp[i]=(byte)194;break;
                   case     183 :tmp[i]=(byte)192;break;
                   case     184 :tmp[i]=(byte)169;break;
                   case     185 :tmp[i]=(byte)23; break;
                   case     186 :tmp[i]=(byte)24; break;
                   case     187 :tmp[i]=(byte)25; break;
                   case     188 :tmp[i]=(byte)26; break;
                   case     189 :tmp[i]=(byte)162;break;
                   case     190 :tmp[i]=(byte)165;break;
                   case     198 :tmp[i]=(byte)227;break;
                   case     199 :tmp[i]=(byte)195;break;
                   case     196 :tmp[i]=(byte)151;break;
                   case     207 :tmp[i]=(byte)156;break;
                   case     210 :tmp[i]=(byte)202;break;
                   case     211 :tmp[i]=(byte)203;break;
                   case     212 :tmp[i]=(byte)200;break;
                   case     214 :tmp[i]=(byte)205;break;
                   case     224 :tmp[i]=(byte)211;break;
                   case     225 :tmp[i]=(byte)223;break;
                   case     227 :tmp[i]=(byte)210;break;
                   case     230 :tmp[i]=(byte)181;break;
                   case     233 :tmp[i]=(byte)218;break;
                   case     235 :tmp[i]=(byte)217;break;
                   default: tmp[i]=buffer[pos+i];break;
                   }
                }
                //s = new String(tmp, 0, len,"ISO8859-1");
                s = new String(tmp, 0, len,"windows-1252");                
                //s = new String(buffer, pos, len,"Cp437");
            }
            else
            //s = new String(tmp, "ISO8859-1");
                s = new String(buffer, pos, len,"windows-1252");
            if (!traducir)
                return s;
            return Idiomaton(s);
        } catch (UnsupportedEncodingException ex) {
            ex.printStackTrace();
            Logger.getLogger(DsSysJava.class.getName()).log(Level.SEVERE, null, ex);
            s = new String(buffer, pos, len);
            if (!traducir)
                return s;
            return Idiomaton(s);
        }        
    }

    public static int putCStringD(byte[] buffer, int pos,String s,int max) {
        try {
            int i;
            byte tmp[] = s.getBytes();
            for (i = 0; i < max && i < tmp.length && pos < (buffer.length - 1); i++) {
                buffer[pos] = tmp[i];
                pos++;
            }
            buffer[pos] = 0;
            pos++;
        } catch (Exception ex) {
            ex.printStackTrace();
            Logger.getLogger(DsSysJava.class.getName()).log(Level.SEVERE, null, ex);
        }
        return pos;
    }

    public static int putCString(byte[] buffer, int pos,String s,int max,boolean oem) {
        try {
            int i;
            byte[] tmp;
            if (oem == true) {
                tmp = s.getBytes("Cp437");
            } else {
                tmp = s.getBytes("ISO8859-1");
            }
            for (i = 0; i < max && i < tmp.length && pos < (buffer.length - 1); i++) {
                if (oem == true)
                    buffer[pos] = tmp[i];
                else {
                    int cc;
                   cc = (0x000000FF & ((int)tmp[i]));
                   switch (cc) {
                   /* sin conversion 'atras'*/
                   case     193:buffer[pos] = (byte)'A';break;
                   case		201:buffer[pos] = (byte)'E';break;
                   case		205:buffer[pos] = (byte)'I';break;
                   case		211:buffer[pos] = (byte)'O';break;
                   case		218:buffer[pos] = (byte)'U';break;
                   case		192:buffer[pos] = (byte)'A';break;
                   case		200:buffer[pos] = (byte)'E';break;
                   case		204:buffer[pos] = (byte)'I';break;
                   case		210:buffer[pos] = (byte)'O';break;
                   case		217:buffer[pos] = (byte)'U';break;
                   case	    180:buffer[pos] = (byte) 39;break; /* apostrofo */
                   /* reversibles (mostly) */
                   case     233:buffer[pos] = (byte)130;break;
                   case     252:buffer[pos] = (byte)129;break;
                   case     225:buffer[pos] = (byte)160;break;
                   case     237:buffer[pos] = (byte)161;break;
                   case     243:buffer[pos] = (byte)162;break;
                   case     250:buffer[pos] = (byte)163;break;
                   case     241:buffer[pos] = (byte)164;break;
                   case     209:buffer[pos] = (byte)165;break;
                   case     170:buffer[pos] = (byte)166;break;
                   case     186:buffer[pos] = (byte)167;break;
                   case     191:buffer[pos] = (byte)168;break;
                   case     146:buffer[pos] = (byte)239;break;
                   case     177:buffer[pos] = (byte)241;break;
                   case     247:buffer[pos] = (byte)246;break;
                   case     199:buffer[pos] = (byte)128;break;
                   case     226:buffer[pos] = (byte)131;break;
                   case     228:buffer[pos] = (byte)132;break;
                   case     224:buffer[pos] = (byte)133;break;
                   case     229:buffer[pos] = (byte)134;break;
                   case     231:buffer[pos] = (byte)135;break;
                   case     234:buffer[pos] = (byte)136;break;
                   case     235:buffer[pos] = (byte)137;break;
                   case     232:buffer[pos] = (byte)138;break;
                   case     239:buffer[pos] = (byte)139;break;
                   case     238:buffer[pos] = (byte)140;break;
                   case     236:buffer[pos] = (byte)141;break;
                   case     196:buffer[pos] = (byte)142;break;
                   case     197:buffer[pos] = (byte)143;break;
                   /*case     201:buffer[pos] = (byte)144;break;*/
                   case     230:buffer[pos] = (byte)145;break;
                   case     198:buffer[pos] = (byte)146;break;
                   case     244:buffer[pos] = (byte)147;break;
                   case     245:buffer[pos] = (byte)148;break;
                   case     242:buffer[pos] = (byte)149;break;
                   case     251:buffer[pos] = (byte)150;break;
                   case     249:buffer[pos] = (byte)151;break;
                   case     255:buffer[pos] = (byte)152;break;
                   case     214:buffer[pos] = (byte)153;break;
                   case     220:buffer[pos] = (byte)154;break;
                   case     248:buffer[pos] = (byte)155;break;
                   case     163:buffer[pos] = (byte)156;break;
                   case     216:buffer[pos] = (byte)157;break;
                   case     215:buffer[pos] = (byte)158;break;
                   case     131:buffer[pos] = (byte)159;break;
                   case     174:buffer[pos] = (byte)169;break;
                   case     172:buffer[pos] = (byte)170;break;
                   case     189:buffer[pos] = (byte)171;break;
                   case     188:buffer[pos] = (byte)172;break;
                   case     161:buffer[pos] = (byte)173;break;
                   case     171:buffer[pos] = (byte)174;break;
                   case     187:buffer[pos] = (byte)175;break;
                   case     128:buffer[pos] = (byte)176;break;
                   case     20: buffer[pos] = (byte)177;break;
                   case     21: buffer[pos] = (byte)178;break;
                   /*case     124:buffer[pos] = (byte)179;break;*/
                   case     22: buffer[pos] = (byte)180;break;
                   /*case     193:buffer[pos] = (byte)181;break;*/
                   case     194:buffer[pos] = (byte)182;break;
                   /*case     192:buffer[pos] = (byte)183;break;*/
                   case     169:buffer[pos] = (byte)184;break;
                   case     23: buffer[pos] = (byte)185;break;
                   case     24: buffer[pos] = (byte)186;break;
                   case     25: buffer[pos] = (byte)187;break;
                   case     26: buffer[pos] = (byte)188;break;
                   case     162:buffer[pos] = (byte)189;break;
                   case     165:buffer[pos] = (byte)190;break;
                   case     227:buffer[pos] = (byte)198;break;
                   case     195:buffer[pos] = (byte)199;break;

                   case     156:buffer[pos] = (byte)207;break;
                   case     202:buffer[pos] = (byte)210;break;
                   case     203:buffer[pos] = (byte)211;break;
                   /*case     200:buffer[pos] = (byte)212;break;*/
                   /*case     205:buffer[pos] = (byte)214;break;*/
                   /*case     211:buffer[pos] = (byte)224;break;*/
                   case     223:buffer[pos] = (byte)225;break;
                   /*case     210:buffer[pos] = (byte)227;break;*/
                   case     181:buffer[pos] = (byte)230;break;
                   /*case     218:buffer[pos] = (byte)233;break;*/
                   /*case     217:buffer[pos] = (byte)235;break;*/
                       default:
                           buffer[pos] = tmp[i];
                   }
                }
                pos++;
            }
            buffer[pos] = 0;
            pos++;
        } catch (UnsupportedEncodingException ex) {
            ex.printStackTrace();
            Logger.getLogger(DsSysJava.class.getName()).log(Level.SEVERE, null, ex);
        }
        return pos;
    }

    public static int putCString(byte[] buffer, int pos,String s,int max) {
        return putCString(buffer,pos,s,max,true);
    }

    public static void memSet(byte[] buffer,int valor,int tam,int ini,int fin) {
         dssys.memset(buffer,valor,tam,ini,fin);
    }

    public static int CambiaOptionPane(int retConfirmDialog) {
        switch(retConfirmDialog) {
            case JOptionPane.OK_OPTION:
                return IDOK;
            case JOptionPane.CANCEL_OPTION:
                return IDCANCEL;
            //case JOptionPane.YES_OPTION:
              //  return IDYES;
            case JOptionPane.NO_OPTION:
                return IDNO;
            default:
                break;
        }
        return retConfirmDialog;
    }

    public static Object resizeArray (Object oldArray, int newSize) {
       int oldSize = java.lang.reflect.Array.getLength(oldArray);
       Class elementType = oldArray.getClass().getComponentType();
       Object newArray = java.lang.reflect.Array.newInstance(
             elementType,newSize);
       int preserveLength = Math.min(oldSize,newSize);
       if (preserveLength > 0)
          System.arraycopy (oldArray,0,newArray,0,preserveLength);
       return newArray; }

    public static Color ColorRGB(int rgb) {
        int r = 0x000000FF & (int)(byte)((rgb >>>  0) & 0xFF);
        int g = 0x000000FF & (int)(byte)((rgb >>>  8) & 0xFF);
        int b = 0x000000FF & (int)(byte)((rgb >>>  16) & 0xFF);
        return new Color(r,g,b);
    }


    public static String trimLeft(String s) {
        return s.replaceAll("^\\s+", "");
    }

    public static String trimRight(String s) {
        return s.replaceAll("\\s+$", "");
    }

    public static boolean isnumber(String s) {
        s = s.trim();
        for (int i = 0;i < s.length();i++) {
            if (s.charAt(i) < '0' || s.charAt(i) > '9') {
                if (s.charAt(i) != ',' && s.charAt(i) != '.')
                    return false;
            }
        }
        return true;
    }
}

