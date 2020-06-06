/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package enlace;

/**
 *
 * @author francesc
 */
public class TreeDato {
    private int punto;
    private String nombre;
    TreeDato(String nom,int dato) {
        nombre = nom;
        punto = dato;
    }
    public int getDato() {
        return punto;
    }
    @Override
    public String toString() {
            return nombre;
    }
}
