import java.awt.*;
import java.util.*;
import java.io.*;
import java.awt.event.*;
import javax.swing.*;

public class Gui implements KeyListener{ //gui is both a gui and a keylistener for convenience
        //first use text based tetris game to debug
        public static final int WINDOW_WIDTH  = 200*2 - 4;
        public static final int WINDOW_HEIGHT = 440 + 1; //add one more to allow last line

        public static JFrame window   = null;
        public static JPanel content  = null;
        public static Hud    hud    = null;
        public static Canvas canvas = null;

        public static Gui keyListener = null;
        public static boolean activeListening = true;


        public static void removeKeyListener() {
                activeListening = false;
        }

        public static void init() {
                if (window != null) {
                        Tetris.board = new int[Tetris.HEIGHT][Tetris.WIDTH];
                        return; //if already ran once at least
                }
                Tetris.board = new int[Tetris.HEIGHT][Tetris.WIDTH];

                window = new JFrame("Tetris AI");
                window.setSize(WINDOW_WIDTH, WINDOW_HEIGHT);
                window.setVisible(true);
                window.setResizable(false);
                window.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

                Gui keyListener = new Gui();
                window.addKeyListener(keyListener);

                content = new JPanel();
                content.setLayout(new BoxLayout(content, BoxLayout.X_AXIS));

                content.setDoubleBuffered(true);
                window.add(content);
        }

        public static synchronized void refresh() {

                content.removeAll();

                hud    = new Hud();
                canvas = new Canvas();

                content.add(hud);
                content.add(canvas);
                window.add(content);

                window.setVisible(true);
        }

        //@override
        public void keyPressed(KeyEvent e) {
                if (!activeListening)
                        return;

                switch (e.getKeyCode()) {
                case KeyEvent.VK_A:
                        Tetris.translateLeft();
                        break;
                case KeyEvent.VK_D:
                        Tetris.translateRight();
                        break;
                case KeyEvent.VK_W:
                        Tetris.rotate();
                        break;
                case KeyEvent.VK_S:
                        //prett much a clone of what happens in the main method
                        while (!Tetris.checkBoard())
                                Tetris.updateBoard();
                        /*
                        freeze(); //first freeze active
                        updateTetris(); //update to see if there is a tetris after placement
                        generate();*/
                        break;
                }
                Gui.refresh(); //refresh every timcivilization 5e a subroutine is called to improve ux
        }

        //@override
        public void keyTyped(KeyEvent e) {
                //do nothing
        }

        //@override
        public void keyReleased(KeyEvent e) {
                //do nothing
        }
}