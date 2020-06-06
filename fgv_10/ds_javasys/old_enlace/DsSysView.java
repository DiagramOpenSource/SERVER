/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package enlace;

import javax.swing.tree.DefaultMutableTreeNode;

/**
 *
 * @author francesc
 */
public interface DsSysView {
        public void Progreso(int opcion, byte[] buffer);
        public void setLeftTree(DefaultMutableTreeNode nodo);
        public void refrescaTree();
        public void setMensa(String s);
        public void setStatusMensa(String s);
        public void setStatusRelacion(String s);
        public void setStatusUsuario(String s);        
        public void setNomApli(String s);
        public void setNomTab(String s);
        public void procesaMensajes(int parametro,byte[] buffer,boolean condata);
        public void controlDePantallas(int parametro,byte[] buffer);
        public void ControlUsuario(int parametro,byte[] buffer);
        public void InsertaControl(int parametro,byte[] buffer);
        public void InsertaEdit(int parametro,byte[] buffer);
        public void DestruyeControl(int parametro,byte[] buffer);
        public void EstadoControl(int parametro,byte[] buffer);
        public void FocoControl(int parametro, byte[] buffer);
        public void GetDataControl(int parametro, byte[] buffer,boolean inlinedata);
        public void RefrescaControl(int parametro);
        public void ControlBinario(int parametro, byte[] buffer);
        public void ponTecla(int tecla,Object idtecla);
}
