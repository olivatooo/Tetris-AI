import java.awt.*;
import java.util.*;
import java.io.*;
import java.awt.event.*;
import javax.swing.*;

public class Canvas extends JPanel{
        private Color BG = new Color(34, 47, 62);

        public static Color[] palette = { new Color(243, 104, 224),
                                    new Color(255, 159, 67),
                                    new Color(255, 107, 107),
                                    new Color(10, 189, 237),
                                    new Color(29, 219, 161),
                                    new Color(125, 39, 215),
                                    new Color(255, 255, 255) };

        private void drawBlock(int y, int x, Graphics g) {

                g.setColor(palette[(int)(Math.abs(Tetris.board[y][x]) - 1)]);
                g.fillRect(x * (this.getWidth()  / Tetris.WIDTH),
                           y * (this.getHeight() / Tetris.HEIGHT),
                           (this.getWidth()  / Tetris.WIDTH),
                           (this.getHeight() / Tetris.HEIGHT) );

                g.setColor(BG);
                g.drawRect(x * (this.getWidth()  / Tetris.WIDTH),
                           y * (this.getHeight() / Tetris.HEIGHT),
                           (this.getWidth()  / Tetris.WIDTH),
                           (this.getHeight() / Tetris.HEIGHT) );
        }

        //@override
        public void paintComponent(Graphics g) {
                try {
                        Toolkit.getDefaultToolkit().sync();
                        g.setColor(BG);
                        g.fillRect(0, 0, this.getWidth(), this.getHeight());
                        for (int i = 0; i < Tetris.HEIGHT; i++) {
                              for (int j = 0; j < Tetris.WIDTH; j++) {
                                        if (Tetris.board[i][j] != 0)
                                               drawBlock(i, j, g);
                                }
                        }
                } catch (Exception e) {

                }
        }
}
