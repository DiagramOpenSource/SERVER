/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package enlace;

/**
 *
 * @author francesc
 */
import java.awt.*;
import javax.swing.*;
import java.awt.event.*;

public class PidePassword {
    /*
     * Prompts the user for input a password with icon in a blocking dialog where the initial value, dialog title canbe specified.
     * User can input password which is required for further processing, usually by means of a <code>JTextField</code>.
     * <code>initalValue</code> is the initial value that can be used in cased of Saved Password
     * @return user's input password, or <code>null</code> meaning the user canceled the input
     */
    public static int showPasswordDialog(Component owner,JPasswordField pwd,String message,String initialValue,String title) {
        final JPasswordField pField = pwd;
        GridBagLayout gridbag = new GridBagLayout();
        GridBagConstraints c = new GridBagConstraints();
        final JPanel pPanel;

        c.insets.top = 4;
        c.insets.bottom = 4;
        pField.setText(initialValue);
        pPanel = new JPanel(gridbag);
        pPanel.setBorder(BorderFactory.createEmptyBorder(10, 20, 5, 20));
        c.anchor = GridBagConstraints.WEST;
        //pPanel.add(new JLabel(message),c);
        pField.addFocusListener(new FocusListener() {
        	public void focusLost(FocusEvent fe) {
        		pField.setBackground(Color.white);
        	}
        	public void focusGained(FocusEvent fe) {
        		pField.setBackground(Color.yellow);
        	}
        });

        c.gridy=1;
        pPanel.add(pField,c);
        pPanel.requestFocusInWindow(); //DOES NOT WORKS
        JButton okButton = new JButton(DsSysJava.Idiomaton("OK")) {
			boolean firstTimeFocuss=true;
			@Override
			public void requestFocus() {
		   		if(firstTimeFocuss) {
		   			pPanel.requestFocusInWindow();
		   			firstTimeFocuss=false;
		   		}
		   		super.requestFocus();
		   	}
		};
       Object[] possibleValues = {okButton, DsSysJava.Idiomaton("Cancelar")};
        JOptionPane pane=new JOptionPane(pPanel, JOptionPane.QUESTION_MESSAGE);
        pane.setOptions(possibleValues);

        final JDialog jd=pane.createDialog(owner,title);
		pField.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent ae)	 {
				jd.setVisible(false)	;
			}
		});
		okButton.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent ae) {
				jd.setVisible(false);
			}
		});
		jd.setVisible(true);
       Object selectedValue = pane.getValue();
       /*if(selectedValue!=null)
       		System.out.println (selectedValue+"="+selectedValue.getClass());*/
        jd.dispose();
        //int result = JOptionPane.showConfirmDialog(owner, pPanel,title,JOptionPane.OK_CANCEL_OPTION);
        if (selectedValue!=null && selectedValue.equals("uninitializedValue"))
            return 0;
        return -1;
    }
}