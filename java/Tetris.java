import java.awt.*;
import java.util.*;
import java.io.*;
import java.awt.event.*;
import javax.swing.*;

public class Tetris {
        /*
         * ====================== DOCUMENTATION =========================
         *  game mechanics:
         * - let  > 0 represent active blocks
         * - let  < 0 represent dead blocks
         * - let  0   represent empty
         *
         * - after a tetris occurs just move all the dead blocks
         *   for n amount according to how many lines were cleared
         *   pieces with area blocking should be ignored
         *
         * pieces:
         * 1 L
         * 2 J (mirror L)
         * 3 Z
         * 4 Y (mirror Z)
         * 5 O
         * 6 I
         * 7 T
         *
         * visual representation:
         *
         * [ ]
         * [x]
         * [ ][ ]
         *
         *
         *    [ ]
         *    [x]
         * [ ][ ]
         *
         *
         * [ ][X]
         *    [ ][ ]
         *
         *
         *    [X][ ]
         * [ ][ ]
         *
         *
         * [X][ ]
         * [ ][ ]
         *
         *
         * [ ][X][ ][ ]
         *
         *
         * [ ][X][ ]
         *    [ ]
         *
         * rotation notes:
         * - we need to set up a rotation grid of at least 5x5
         *
         * tetris notes:
         * - just remove the dead stuff
         *
         * HUD lookup
         *
         * [ ][ ][ ][ ][ ] 5x5 cell
         * [ ][ ][ ][ ][ ]
         * [ ][ ][ ][ ][ ]
         * [ ][ ][ ][ ][ ]
         * [ ][ ][ ][ ][ ]
         */

        public static String HIGHSCORE_DAT = "highscores.dat";
        public static final int HEIGHT = 22, //top two are not playing area
                                WIDTH  = 10,
                                HIGH_SCORE_BOARD_SIZE = 7;

        public static int x, y, type, nextType, level = 0, score = 0, delay = 500; //same as speed;
        public static int[][] board;
        public static int[] highscores;

        public static void generate() {
                type = nextType;
                nextType = (int)(Math.random() * 7 + 1);

                x    = WIDTH / 2;
                switch (type) {
                case 1:
                        y = 1;
                        x--;
                        board[y][x]     = 1;
                        board[y-1][x]   = 1;
                        board[y+1][x]   = 1;
                        board[y+1][x+1] = 1;
                        break;
                case 2:
                        y = 1;
                        board[y][x]   = 2;
                        board[y+1][x] = 2;
                        board[y-1][x] = 2;
                        board[y+1][x-1]   = 2;
                        break;
                case 3:
                        y = 0;
                        board[y][x]     = 3;
                        board[y][x-1]   = 3;
                        board[y+1][x+1] = 3;
                        board[y+1][x]   = 3;
                        break;
                case 4:
                        y = 0;
                        board[y][x]     = 4;
                        board[y][x+1]   = 4;
                        board[y+1][x]   = 4;
                        board[y+1][x-1] = 4;
                        break;
                case 5:
                        x--;
                        y = 0;
                        board[y][x]     = 5;
                        board[y][x+1]   = 5;
                        board[y+1][x]   = 5;
                        board[y+1][x+1] = 5;
                        break;
                case 6:
                        y = 0;
                        x--;
                        board[y][x]   = 6;
                        board[y][x-1] = 6;
                        board[y][x+1] = 6;
                        board[y][x+2] = 6;
                        break;
                case 7:
                        y = 0;
                        board[y][x]   = 7;
                        board[y][x+1] = 7;
                        board[y][x-1] = 7;
                        board[y+1][x] = 7;
                        break;
                }
        }

        /*
         * for the translation subroutines do the same thing we did in the update board
         * where we first checked then moved the blocks
         */
        public static void translateRight() {
                boolean ret = false;
                for (int i = 0; i < HEIGHT; i++) {
                        for (int j = WIDTH - 1; j >= 0; j--) {
                                if (board[i][j] > 0 && (j == WIDTH - 1 || board[i][j+1] < 0)) {
                                //if a single coordinate shoes with it is at a wall or dead block
                                        ret = true;
                                }
                        }
                }
                if (ret)
                        return;
                for (int i = 0; i < HEIGHT; i++) {
                        for (int j = WIDTH - 1; j >= 0; j--) {
                                if (board[i][j] > 0 && j < WIDTH - 1 && board[i][j+1] == 0) {
                                        board[i][j+1] = board[i][j];
                                        board[i][j] = 0;
                                }
                        }
                }
                x++;
        }

        public static void translateLeft() {
                boolean ret = false;
                for (int i = 0; i < HEIGHT; i++) {
                        for (int j = 0; j < WIDTH; j++) {
                                if (board[i][j] > 0  && (j == 0 || board[i][j-1] < 0)) {
                                        ret = true;
                                }
                        }
                }
                if (ret)
                        return;
                for (int i = 0; i < HEIGHT; i++) {
                        for (int j = 0; j < WIDTH; j++) {
                                if (board[i][j] > 0 && j > 0 && board[i][j-1] == 0) {
                                        board[i][j-1] = board[i][j];
                                        board[i][j] = 0;
                                }
                        }
                }
                x--;
        }

        public static boolean safe(int y, int x) {
                return y >= 0 && y < HEIGHT && x >= 0 && x < WIDTH && board[y][x] >= 0;
        }

        public static void clearRect(int y1, int x1, int y2, int x2) { //use this to clear for rotation
                for (int i = y1; i <= y2; i++) {
                        for (int j = x1; j <= x2; j++) {
                                if (board[i][j] > 0) //only clear active blocks
                                        board[i][j] = 0;
                        }
                }
        }

        public static void rotate() {
                try {
                        switch (type) {
                        case 1:
                                /*
                                 *
                                 * [ ]         [ ][x][ ]     [ ][ ]
                                 * [x]     ->  [ ]       ->     [x]  ->       [ ]
                                 * [ ][ ]                       [ ]     [ ][x][ ]
                                 *
                                 */
                                if (board[y][x] > 0 && board[y-1][x] > 0 &&
                                                board[y+1][x] > 0 && board[y+1][x+1] > 0) {
                                        //check safety
                                        if (safe(y, x+1) &&
                                            safe(y, x-1) &&
                                            safe(y, x) &&
                                            safe(y+1, x-1)) {
                                                clearRect(y-1, x, y+1, x+1);
                                                board[y][x+1]   = 1;
                                                board[y][x-1]   = 1;
                                                board[y][x]     = 1;
                                                board[y+1][x-1] = 1;
                                        }
                                } else if (board[y][x+1] > 0 && board[y][x-1] > 0 &&
                                                board[y][x] > 0 && board[y+1][x-1] > 0) {

                                        if (safe(y-1, x) &&
                                            safe(y,   x) &&
                                            safe(y+1, x) &&
                                            safe(y-1, x-1)) {
                                                clearRect(y, x-1, y+1, x+1);
                                                board[y-1][x]   = 1;
                                                board[y][x]     = 1;
                                                board[y+1][x]   = 1;
                                                board[y-1][x-1] = 1;
                                        }
                                } else if (board[y-1][x] > 0 && board[y][x] > 0 &&
                                                board[y+1][x] > 0 && board[y-1][x-1] > 0) {

                                        if (safe(y, x-1) &&
                                            safe(y, x+1) &&
                                            safe(y, x)   &&
                                            safe(y-1, x+1)) {
                                                clearRect(y-1, x-1, y+1, x);
                                                board[y][x-1]   = 1;
                                                board[y][x+1]   = 1;
                                                board[y][x]     = 1;
                                                board[y-1][x+1] = 1;
                                        }
                                } else {
                                        if (safe(y, x)   &&
                                            safe(y-1, x) &&
                                            safe(y+1, x) &&
                                            safe(y+1, x+1)) {
                                                clearRect(y-1, x-1, y, x+1);
                                                board[y][x]     = 1;
                                                board[y-1][x]   = 1;
                                                board[y+1][x]   = 1;
                                                board[y+1][x+1] = 1;
                                        }
                                }
                                break;
                        case 2:
                                /*
                                 *
                                 *    [ ]                     [ ][ ]       [ ][x][ ]
                                 *    [x]   ->  [ ]       ->  [x]    ->          [ ]
                                 * [ ][ ]       [ ][x][ ]     [ ]
                                 *
                                 */
                                if (board[y+1][x] > 0 && board[y][x] > 0 &&
                                                board[y-1][x] > 0 && board[y+1][x-1] > 0) {

                                        if (safe(y, x-1) &&
                                            safe(y, x+1) &&
                                            safe(y, x) &&
                                            safe(y-1, x-1)) {
                                                clearRect(y-1, x-1, y+1, x);
                                                board[y][x-1]   = 2;
                                                board[y][x+1]   = 2;
                                                board[y][x]     = 2;
                                                board[y-1][x-1] = 2;
                                        }
                                } else if (board[y][x-1] > 0 && board[y][x+1] > 0 &&
                                                board[y][x] > 0 && board[y-1][x-1] > 0) {

                                        if (safe(y-1, x+1) &&
                                            safe(y+1, x)   &&
                                            safe(y, x)     &&
                                            safe(y-1, x)) {
                                                clearRect(y-1, x-1, y, x+1);
                                                board[y-1][x+1] = 2;
                                                board[y+1][x]   = 2;
                                                board[y][x]     = 2;
                                                board[y-1][x]   = 2;
                                        }
                                } else if (board[y-1][x+1] > 0 && board[y+1][x] > 0 &&
                                                board[y][x] > 0 && board[y-1][x] > 0) {

                                        if (safe(y, x) &&
                                            safe(y, x+1) &&
                                            safe(y, x-1) &&
                                            safe(y+1, x+1)) {
                                                clearRect(y-1, x, y+1, x+1);
                                                board[y][x]     = 2;
                                                board[y][x+1]   = 2;
                                                board[y][x-1]   = 2;
                                                board[y+1][x+1] = 2;
                                        }
                                } else {
                                        if (safe(y,   x) &&
                                            safe(y+1, x) &&
                                            safe(y-1, x) &&
                                            safe(y+1, x-1)) {
                                                clearRect(y, x-1, y+1, x+1);
                                                board[y][x]     = 2;
                                                board[y+1][x]   = 2;
                                                board[y-1][x]   = 2;
                                                board[y+1][x-1] = 2;
                                        }
                                }
                                break;
                        case 3:
                                /*
                                 *
                                 * [ ][x]             [ ]
                                 *    [ ][ ]   ->  [ ][x]
                                 *                 [ ]
                                 */
                                if (board[y][x-1] > 0 && board[y][x] > 0 &&
                                                board[y+1][x] > 0 && board[y+1][x+1] > 0) {

                                        if (safe(y, x) &&
                                            safe(y-1, x) &&
                                            safe(y, x-1) &&
                                            safe(y+1, x-1)) {
                                                clearRect(y, x-1, y+1, x+1);
                                                board[y][x]     = 3;
                                                board[y-1][x]   = 3;
                                                board[y][x-1]   = 3;
                                                board[y+1][x-1] = 3;
                                        }
                                } else {
                                        if (safe(y, x) &&
                                            safe(y, x-1) &&
                                            safe(y+1, x) &&
                                            safe(y+1, x+1)) {
                                                clearRect(y-1, x-1, y+1, x);
                                                board[y][x]     = 3;
                                                board[y][x-1]   = 3;
                                                board[y+1][x]   = 3;
                                                board[y+1][x+1] = 3;
                                        }
                                }
                                break;
                        case 4:
                                /*
                                 *                  [ ]
                                 *    [x][ ]   ->   [x][ ]
                                 * [ ][ ]              [ ]
                                 *
                                 */
                                if (board[y][x] > 0 && board[y][x+1] > 0 &&
                                                board[y+1][x] > 0 && board[y+1][x-1] > 0) {

                                        if (safe(y, x) &&
                                            safe(y-1, x) &&
                                            safe(y, x+1) &&
                                            safe(y+1, x+1)) {
                                                clearRect(y, x-1, y+1, x+1);
                                                board[y][x]     = 4;
                                                board[y-1][x]   = 4;
                                                board[y][x+1]   = 4;
                                                board[y+1][x+1] = 4;
                                        }
                                } else {
                                        if (safe(y, x) &&
                                            safe(y, x+1) &&
                                            safe(y+1, x) &&
                                            safe(y+1, x-1)) {
                                                clearRect(y-1, x, y+1, x+1);
                                                board[y][x]     = 4;
                                                board[y][x+1]   = 4;
                                                board[y+1][x]   = 4;
                                                board[y+1][x-1] = 4;
                                        }
                                }
                                break;
                        case 5:
                                //five is the same regardless of rotation
                                break;
                        case 6:
                                /*
                                 *                   [ ]
                                 * [ ][x][ ][ ]  ->  [ ]
                                 *                   [x]
                                 *                   [ ]
                                 *
                                 */
                                if (board[y][x] > 0 && board[y][x-1] > 0 &&
                                                board[y][x+1] > 0 && board[y][x+2] > 0) {

                                        if (safe(y+1, x) &&
                                            safe(y-2, x) &&
                                            safe(y-1, x) &&
                                            safe(y, x)) {
                                                clearRect(y, x-1, y, x+2);
                                                board[y+1][x] = 6;
                                                board[y-2][x] = 6;
                                                board[y-1][x] = 6;
                                                board[y][x]   = 6;
                                        }
                                } else {
                                        if (safe(y, x-1) &&
                                            safe(y, x) &&
                                            safe(y, x+1) &&
                                            safe(y, x+2)) {
                                                clearRect(y-2, x, y+1, x);
                                                board[y][x-1] = 6;
                                                board[y][x]   = 6;
                                                board[y][x+1] = 6;
                                                board[y][x+2] = 6;
                                        }
                                }
                                break;
                        case 7:
                                /*
                                 *                   [ ]         [ ]         [ ]
                                 * [ ][x][ ]  ->  [ ][x]  ->  [ ][x][ ]  ->  [x][ ]
                                 *    [ ]            [ ]                     [ ]
                                 *
                                 */
                                if (board[y][x] > 0 && board[y][x+1] > 0 &&
                                                board[y][x-1] > 0 && board[y+1][x] > 0) {

                                        if (safe(y, x) &&
                                            safe(y, x-1) &&
                                            safe(y+1, x) &&
                                            safe(y-1, x)) {
                                                clearRect(y, x-1, y+1, x+1);
                                                //check if rotation is safe
                                                board[y][x]   = 7;
                                                board[y][x-1] = 7;
                                                board[y+1][x] = 7;
                                                board[y-1][x] = 7;
                                        }
                                } else if (board[y][x] > 0 && board[y+1][x] > 0 &&
                                                board[y-1][x] > 0 && board[y][x-1] > 0) {

                                        if (safe(y-1, x) &&
                                            safe(y, x+1) &&
                                            safe(y, x-1) &&
                                            safe(y, x)) {
                                                clearRect(y-1, x-1, y+1, x);
                                                board[y-1][x] = 7;
                                                board[y][x+1] = 7;
                                                board[y][x-1] = 7;
                                                board[y][x]   = 7;
                                        }
                                } else if (board[y-1][x] > 0 && board[y][x+1] > 0 &&
                                                board[y][x-1] > 0 && board[y][x] > 0) {

                                        if (safe(y, x) &&
                                            safe(y, x+1) &&
                                            safe(y-1, x) &&
                                            safe(y+1, x)) {
                                                clearRect(y-1, x-1, y, x+1);
                                                board[y][x]   = 7;
                                                board[y][x+1] = 7;board[y][x]   = 7;board[y][x]   = 7;
                                                board[y-1][x] = 7;
                                                board[y+1][x] = 7;
                                        }
                                } else {
                                        if (safe(y, x) &&
                                            safe(y+1, x) &&
                                            safe(y, x+1) &&
                                            safe(y, x-1)) {
                                                clearRect(y-1, x, y+1, x+1);
                                                board[y][x]   = 7;
                                                board[y+1][x] = 7;
                                                board[y][x+1] = 7;
                                                board[y][x-1] = 7;
                                        }
                                }
                                break;
                        }
                } catch (Exception e) {
                        //the only exception that could be thrown is array out of bounds, but thats anoyying so im just
                        //going to ignore it for now. maybe change this later after im done the genetic algorithm part
                }
        }

        public static void updateTetris() {
                for (int i = HEIGHT - 1; i >= 0; i--) {
                        boolean rowFound = true;
                        for (int j = 0; j < WIDTH && rowFound; j++)
                                rowFound = board[i][j] != 0;
                        if (rowFound) {
                                score += (level+1) * 10;
                                for (int k = i-1; k >= 0; k--) {
                                        for (int m = 0; m < WIDTH; m++)
                                                board[k+1][m] = board[k][m];
                                }
                                updateTetris(); //look for more
                                return;         //end recursion stack
                        }
                }
        }

        public static boolean checkBoard() {
                for (int i = HEIGHT - 1; i >= 0; i--) { //first check if we hit a dead end
                        for (int j = 0; j < WIDTH; j++) {
                                if (board[i][j] > 0 && i == HEIGHT - 1
                                || board[i][j] > 0 && i < HEIGHT - 1 && board[i+1][j] < 0) {
                                        //check if bottom is hit or hit another block
                                        return true;
                                }
                        }
                }
                return false;
        }

        public static boolean updateBoard() {
                boolean ret = false; //wether or not we have hit a dead end
                for (int i = HEIGHT - 1; i >= 0; i--) { //first check if we hit a dead end
                        for (int j = 0; j < WIDTH; j++) {
                                if (board[i][j] > 0 && i == HEIGHT - 1
                                || board[i][j] > 0 && i < HEIGHT - 1 && board[i+1][j] < 0) {
                                        //check if bottom is hit or hit another block
                                        ret = true;
                                }
                        }
                }
                if (!ret) { //if we didnt hit a dead end, continue
                        for (int i = HEIGHT - 1; i >= 0; i--) {
                                for (int j = 0; j < WIDTH; j++) {
                                        if (board[i][j] > 0 && board[i+1][j] == 0) {
                                                board[i+1][j] = board[i][j];
                                                board[i][j]   = 0;
                                        }
                                }
                        }
                        y++; //keep track of the y coordinates
                }
                return ret;
        }

        public static void freeze() {
                //turn the active blocks inactive, switch the integer value
                for (int i = 0; i < HEIGHT; i++) {
                        for (int j = 0; j < WIDTH; j++) {
                                if (board[i][j] > 0) //freeze
                                       board[i][j] = board[i][j] * -1;
                        }
                }
        }

        public static boolean endGameChecker() {
                //run this to check if the game is ended
                boolean found = false;
                for (int i = 0; i < WIDTH && !found; i++)
                        if (board[2][i] < 0)
                                found = true;
                return found;
        }

        public static void printBoard() {
                for (int i = 0; i < HEIGHT; i++) {
                        for (int j = 0; j < WIDTH; j++)
                                System.out.print(board[i][j] + " ");
                        System.out.println();
                }
        }

        //first use text based tetris game to debug
        public static class Gui implements KeyListener{ //gui is both a gui and a keylistener for convenience
                public static final int WINDOW_WIDTH  = 200*2 - 4;
                public static final int WINDOW_HEIGHT = 400 + 1; //add one more to allow last line

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
                        Tetris.board = new int[Tetris.HEIGHT][Tetris.WIDTH];

                        window = new JFrame("Tetris");
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
                                translateLeft();
                                break;
                        case KeyEvent.VK_D:
                                translateRight();
                                break;
                        case KeyEvent.VK_W:
                                rotate();
                                break;
                        case KeyEvent.VK_S:
                                //prett much a clone of what happens in the main method
                                while (!checkBoard())
                                        updateBoard();
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

        public static class Hud extends JPanel{
                private Color BG     = new Color(44, 57, 72);
                private Color TEXT   = new Color(87, 101, 116);

                private final int MARGIN = 58;

                public void paintComponent(Graphics g) {
                        Toolkit.getDefaultToolkit().sync();
                        g.setColor(BG);
                        g.fillRect(0, 0, this.getWidth(), this.getHeight());

                        //title
                        g.setFont(new Font("Monospaced", Font.BOLD, 24));
                        g.setColor(TEXT);
                        g.drawString("TETRIS", this.getWidth()/2 - 45, 29);

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
                                                g.setColor(Canvas.palette[nextType-1]);
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
                        g.drawString("SCORE: " + score, this.getWidth() / 2 - 89, 165);
                        g.drawString("LEVEL: " + level, this.getWidth() / 2 - 89, 195);
                        g.drawString("HIGHSCORES:", this.getWidth() / 2 - 89, 225);


                        g.setFont(new Font("Monospaced", Font.BOLD, 14));
                        for (int i = 0; i < HIGH_SCORE_BOARD_SIZE-2; i++) {
                                g.drawString((i+1) + ".) " + highscores[i] + "",
                                        this.getWidth() / 2 - 89, 255 + i * 25);
                        }
                        //highscores from highscore.dat
                }
        }

        public static class Canvas extends JPanel{
                private Color BG = new Color(34, 47, 62);

                public static Color[] palette = { new Color(243, 104, 224),
                                            new Color(255, 159, 67),
                                            new Color(255, 107, 107),
                                            new Color(10, 189, 237),
                                            new Color(29, 219, 161),
                                            new Color(125, 39, 215),
                                            new Color(255, 255, 255) };

                private void drawBlock(int y, int x, Graphics g) {

                        g.setColor(palette[Math.abs(Tetris.board[y][x]) - 1]);
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
                        Toolkit.getDefaultToolkit().sync();
                        g.setColor(BG);
                        g.fillRect(0, 0, this.getWidth(), this.getHeight());
                        for (int i = 0; i < Tetris.HEIGHT; i++) {
                                for (int j = 0; j < Tetris.WIDTH; j++) {
                                        if (Tetris.board[i][j] != 0)
                                               drawBlock(i, j, g);
                                }
                        }
                }
        }

        public static void delay(int d) {
                try {
                        Thread.sleep(d);
                } catch (Exception e) {
                        //do nothing for now
                }
        }

        public static void importHighScores() {
                try {
                        Scanner sc = new Scanner(new File(HIGHSCORE_DAT));
                        highscores = new int[HIGH_SCORE_BOARD_SIZE];
                        for (int i = 0; i < highscores.length; i++)
                                highscores[i] = sc.nextInt();
                } catch (Exception e) {
                        System.out.println("something went wrong on importing");
                }
        }

        public static void exportHighScores() {
                for (int i = 0; i < highscores.length; i++) {
                        if (score >= highscores[i]) {
                                for (int j = highscores.length - 1; j > i; j--)
                                        highscores[j] = highscores[j-1];
                                highscores[i] = score;
                                break;
                        }
                }
                try {
                        FileWriter fw = new FileWriter(new File(HIGHSCORE_DAT));
                        BufferedWriter bw = new BufferedWriter(fw);
                        for (int i = 0; i < highscores.length; i++)
                                bw.write(highscores[i]+" ");

                        bw.close();
                } catch (Exception e) {
                        System.out.println("something went wrong on export");
                }
        }

        public static void main(String[] args) {
                importHighScores();

                Gui.init();
                Gui.refresh();

                nextType = (int)(Math.random() * 7 + 1);
                generate();

                long startTime = System.currentTimeMillis();

                while (!endGameChecker()) {
                        //printBoard();
                        //System.out.println();
                        //game mechanics printing
                        boolean spawn = updateBoard();
                        delay(delay); //give player a chance to move
                        if (spawn && checkBoard()) { //checkboard because you can move out of a hit == placement
                                freeze(); //first freeze active
                                updateTetris(); //update to see if there is a tetris after placement
                                generate();
                        }

                        //let's set up a timer that speeds up the game of tetris as it goes along
                        long elapsedTime = System.currentTimeMillis() - startTime;

                        //use this for human mode
                        if (elapsedTime >  10000) {
                                level = 1;
                                delay = 450;
                        }
                        if (elapsedTime >  20000) {
                                level = 2;
                                delay = 400;
                        }
                        if (elapsedTime >  40000) {
                                level = 3;
                                delay = 350;
                        }
                        if (elapsedTime >  60000) {
                                level = 4;
                                delay = 300;
                        }
                        if (elapsedTime >  90000) {
                                level = 5;
                                delay = 250;
                        }
                        if (elapsedTime >  120000) {
                                level = 6;
                                delay = 200;
                        }
                        if (elapsedTime >  160000) {
                                level = 7;
                                delay = 150;
                        }
                        if (elapsedTime >  200000) {
                                level = 8;
                                delay = 115;
                        }
                        if (elapsedTime >  250000) {
                                level = 9;
                                delay = 100;
                        }
                        if (elapsedTime >  320000) {
                                level = 10;
                                delay = 85;
                        }
                        if (elapsedTime >  390000) {
                                level = 11;
                                delay = 70;
                        }
                        if (elapsedTime >  480000) {
                                level = 12;
                                delay = 55;
                        }

                        Gui.refresh();
                }

                exportHighScores();
                Gui.removeKeyListener(); //end game, instead of disposing window
                Gui.window.removeAll();
                Gui.window.setVisible(false);
                Gui.window.dispose();
        }
}
