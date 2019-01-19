import java.awt.*;
import java.util.*;
import java.io.*;
import java.awt.event.*;
import javax.swing.*;

public class Hud extends JPanel{
        private Color BG     = new Color(44, 57, 72);
        private Color TEXT   = new Color(87, 101, 116);

        private final int MARGIN = 58;
        public static int dispGen = 0;


        public void paintComponent(Graphics g) {
                Toolkit.getDefaultToolkit().sync();
                g.setColor(BG);
                g.fillRect(0, 0, this.getWidth(), this.getHeight());

                //title
                g.setFont(new Font("Monospaced", Font.BOLD, 24));
                g.setColor(TEXT);
                g.drawString("TETRIS AI", this.getWidth()/2 - 63, 29);

                //next piece
                g.setFont(new Font("Monospaced", Font.BOLD, 18));
                g.drawString("NEXT PIECE", this.getWidth()/2 - 90, 57);


                int[][] grid = new int[5][5];
                int x = 2; //5x5 grid
                switch(Tetris.nextType) {
                case 1:
                        grid[1][2] = 1;
                        grid[2][2] = 1;
                        grid[3][2] = 1;
                        grid[3][3] = 1;
                        break;
                case 2:
                        grid[1][2] = 2;
                        grid[2][2] = 2;
                        grid[3][2] = 2;
                        grid[3][1] = 2;
                        break;
                case 3:
                        grid[2][2] = 3;
                        grid[3][2] = 3;
                        grid[2][1] = 3;
                        grid[3][3] = 3;
                        break;
                case 4:
                        grid[2][2] = 4;
                        grid[3][2] = 4;
                        grid[2][3] = 4;
                        grid[3][1] = 4;
                        break;
                case 5:
                        grid[2][2] = 5;
                        grid[3][2] = 5;
                        grid[2][1] = 5;
                        grid[3][1] = 5;
                        break;
                case 6:
                        grid[1][2] = 6;
                        grid[2][2] = 6;
                        grid[3][2] = 6;
                        grid[4][2] = 6;
                        break;
                case 7:
                        grid[2][1] = 7;
                        grid[2][2] = 7;
                        grid[2][3] = 7;
                        grid[3][2] = 7;
                        break;
                }

                //drawing piece
                for (int i = 0; i < grid.length; i++) {
                        for (int j = 0; j < grid[0].length; j++) {
                                int blockSize = (this.getWidth() - MARGIN * 2) / 5;
                                if (grid[i][j] > 0) {
                                        g.setColor(Canvas.palette[Tetris.nextType-1]);
                                        g.fillRect(MARGIN + blockSize * j, MARGIN + blockSize * i,
                                                blockSize, blockSize);

                                        g.setColor(BG);
                                        g.drawRect(MARGIN + blockSize * j, MARGIN + blockSize * i,
                                                blockSize, blockSize);
                                }
                        }
                }

                //score and level

                g.setColor(TEXT);
                g.drawString("SCORE: " + Tetris.score, this.getWidth() / 2 - 89, 165);
                g.drawString("LEVEL: " + Tetris.level, this.getWidth() / 2 - 89, 195);
                g.drawString("HIGHSCORES:", this.getWidth() / 2 - 89, 225);
                g.drawString("GENERATION:" + dispGen, this.getWidth() / 2 - 89, 395);


                g.setFont(new Font("Monospaced", Font.BOLD, 14));
                for (int i = 0; i < Tetris.HIGH_SCORE_BOARD_SIZE-2; i++) {
                        g.drawString((i+1) + ".) " + Tetris.highscores[i] + "",
                                this.getWidth() / 2 - 89, 255 + i * 25);
                }
                //highscores from highscore.dat
        }
}