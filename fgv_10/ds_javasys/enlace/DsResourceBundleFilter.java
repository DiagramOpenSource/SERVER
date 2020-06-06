/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package enlace;

import bibliothek.gui.dock.support.util.DsResourceStringFilter;
import bibliothek.gui.dock.support.util.Resources;
import java.util.ResourceBundle;

/**
 *
 * @author francesc
 */
public class DsResourceBundleFilter implements DsResourceStringFilter {
    @Override
    public ResourceBundle getBundle() {
        return Resources.i_bundle.getBundle();
    }

    @Override
    public String getString(String s) {
        return DsSysJava.Idiomaton(Resources.i_bundle.getString(s));
    }
}
