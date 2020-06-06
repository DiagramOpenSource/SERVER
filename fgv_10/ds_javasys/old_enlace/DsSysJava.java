package enlace;

import java.io.UnsupportedEncodingException;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.tree.DefaultMutableTreeNode;


public class DsSysJava {

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
    
    /* Instancia unica, sin accesor ... para pruebas :) */
    public static DsSysJava dssys = null;


    /* constantes del basico nativo */
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

    public static final int ID_CUSTOM_ESTADO             =  32861;
    public static final int ID_COMMANDCONTROL            =  32862;

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
    private static String m_dirso;
    private static JFrame m_frame = null;
    /* a modificar segun donde se incluya ... de momento no es generica */
    private static DsSysView m_view = null;

    private static JPanel m_dlgview = null;

    /* metodos */
    
    public DsSysJava() {
        cargaBasico();        
    }

    public static void setFrame(JFrame frame) {
        DsSysJava.m_frame = frame;
    }

    public static JFrame getFrame() {
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

    public static void setDir(String dir) {
        DsSysJava.m_dirso = dir;
    }

    static public Object creaNodoTree(Object padre,int parametro,byte[] buffer) {
        if (parametro == -2)
        {
            m_view.refrescaTree();
            return null;
        }
        boolean root = false;
        if (parametro == -1) {
            root = true;
            parametro = 0;
        }
        DefaultMutableTreeNode nodo = new DefaultMutableTreeNode(new TreeDato(getCString(buffer,0,false),parametro));
        if (root == true)
        {
            m_view.setLeftTree(nodo);
            return nodo;
        }
        ((DefaultMutableTreeNode)padre).add(nodo);
        return nodo;
    }


    static public void despachadorExterno(int codigo,int parametro,byte[] buffer) {

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
                switch(parametro) {
                    case ID_MESSAGEBOX:                        
                        JOptionPane.showMessageDialog(m_frame, getCString(buffer,8) ,"ATENCION",JOptionPane.WARNING_MESSAGE);
                        break;
                    default:
                        JOptionPane.showMessageDialog(m_frame, "MAINCOMMAND " + parametro,"NO IMPLEMENTADO",JOptionPane.ERROR_MESSAGE);
                        break;
                }
                break;
            case DESPACHO_TITULO:
                m_frame.setTitle(getCString(buffer,0));
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
                        m_view.setStatusUsuario(getCString(buffer,0));
                        break;
                }
                break;
            case DESPACHO_OMENU:
                // rellenar el menu ... realmente no se hace aqui, esto no se debe llamar nunca
                break;
            case DESPACHO_EMENU:
                /* si estamos en menu o no, varios modos segun pantalla 9, no aplicables */
                
                break;
            case DESPACHO_PANTALLAS:
                parametro %= 10000;
                m_view.controlDePantallas(parametro,buffer);
                break;
           case DESPACHO_DATA_INFO:
               switch(parametro)
               {
               case 0:
                   m_view.setNomApli(getCString(buffer,0));
                   break;
               case 1:
                   m_view.setNomTab(getCString(buffer,0));
                   break;
               default:
                   break;
               }
               break;
            case DESPACHO_EURO:
               break;
            case DESPACHO_LISTADO:
               break;
            case DESPACHO_PROGRESO:
               m_view.Progreso(parametro,buffer);
               break;
            default:
                JOptionPane.showMessageDialog(m_frame,"DESPACHOEXTERNO "+codigo+" parametro: "+parametro,"NO IMPLEMENTADO",JOptionPane.ERROR_MESSAGE);
                break;
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


    private void cargaBasico() {
        if (m_BasicoCargado == false) {
           System.load(m_dirso + "diagramx.lx.so");
           m_BasicoCargado = true;
        }
    }

    /* utilidades para mayormente para tratar con el basico */
    public static int unsignedByteToInt(byte b) {
        return (int) b & 0xFF;
        }

    public static short getShort(byte buffer[],int pos) {
        short ch1 = (short) (buffer[pos++] & 0xFF);
        short ch2 = (short) (buffer[pos] & 0xFF);
        ch2 <<= 8;
        ch1 += ch2;
        return ch1;
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
            r[i] = getInt(buffer,pos);
            pos += SIZEINT;
        }
        return r;
    }

            /*
            if (oem == true) {
               buffer[pos+i] = (byte)164;
               switch ((int)buffer[pos+i])
               {
               case     130 :tmp[i]=(byte)233;break;
               case     129 :tmp[i]=(byte)252;break;
               case     160 :tmp[i]=(byte)225;break;
               case     161 :tmp[i]=(byte)237;break;
               case     162 :tmp[i]=(byte)243;break;
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
            else*/

    public static String getCString(byte[] buffer, int pos, boolean oem) {
        String s;
        //byte[] tmp;
        int i = pos;
        while(i < buffer.length && buffer[i] != 0)
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
            if (oem == true)
                //s = new String(tmp, "Cp437");
                s = new String(buffer, pos, len,"Cp437");
            else
            //s = new String(tmp, "ISO8859-1");
                s = new String(buffer, pos, len,"ISO8859-1");
            return s;
        } catch (UnsupportedEncodingException ex) {
            Logger.getLogger(DsSysJava.class.getName()).log(Level.SEVERE, null, ex);
            s = new String(buffer, pos, len);
            return s;
        }        
    }

    public static String getCString(byte[] buffer, int pos) {
        return getCString(buffer,pos,true);
    }

    public static void putCString(byte[] buffer, int pos,String s,int max,boolean oem) {
        try {
            int i;
            byte[] tmp;
            if (oem == true) {
                tmp = s.getBytes("Cp437");
            } else {
                tmp = s.getBytes("ISO8859-1");
            }
            for (i = 0; i < max && i < tmp.length && pos < (buffer.length - 1); i++) {
                buffer[pos] = tmp[i];
                pos++;
            }
            buffer[pos] = 0;
        } catch (UnsupportedEncodingException ex) {
            Logger.getLogger(DsSysJava.class.getName()).log(Level.SEVERE, null, ex);
        }
    }

    public static void putCString(byte[] buffer, int pos,String s,int max) {
        putCString(buffer,pos,s,max,true);
    }

    public static void memSet(byte[] buffer,int valor,int tam,int ini,int fin) {
         dssys.memset(buffer,valor,tam,ini,fin);
    }
}

