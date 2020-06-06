/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package enlace;

import java.io.Serializable;

/**
 *
 * @author francesc
 */
public class TreeDato implements Serializable {
    private int punto;
    private String nombre;
    private String opcion = "";
    public TreeDato(String nom,int dato) {
        nombre = nom;
        punto = dato;
    }
    public int getDato() {
        return punto;
    }
    public void setOpcion(String opcion) {
        this.opcion = opcion;
    }
    public String getOpcion() {
        return opcion;
    }
    @Override
    public String toString() {
            return nombre;
    }

    public void setString(String string) {
        nombre = string;
    }
}
