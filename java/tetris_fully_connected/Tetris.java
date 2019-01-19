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

        public static final String HIGHSCORE_DAT = "highscores.dat";
        public static final int HEIGHT = 22; //top two are not playing area
        public static final int WIDTH  = 10;
        public static final int HIGH_SCORE_BOARD_SIZE = 7;

        public static int x, y, type, nextType, level = 0, score = 0, delay = 0; //same as speed;
        public static int[][] board;
        public static int[] highscores;
        public static int otype; //orientation + type

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
                        otype = 0;
                        break;
                case 2:
                        y = 1;
                        board[y][x]   = 2;
                        board[y+1][x] = 2;
                        board[y-1][x] = 2;
                        board[y+1][x-1] = 2;
                        otype = 4;
                        break;
                case 3:
                        y = 0;
                        board[y][x]     = 3;
                        board[y][x-1]   = 3;
                        board[y+1][x+1] = 3;
                        board[y+1][x]   = 3;
                        otype = 8;
                        break;
                case 4:
                        y = 0;
                        board[y][x]     = 4;
                        board[y][x+1]   = 4;
                        board[y+1][x]   = 4;
                        board[y+1][x-1] = 4;
                        otype = 10;
                        break;
                case 5:
                        x--;
                        y = 0;
                        board[y][x]     = 5;
                        board[y][x+1]   = 5;
                        board[y+1][x]   = 5;
                        board[y+1][x+1] = 5;
                        otype = 12;
                        break;
                case 6:
                        y = 0;
                        x--;
                        board[y][x]   = 6;
                        board[y][x-1] = 6;
                        board[y][x+1] = 6;
                        board[y][x+2] = 6;
                        otype = 13;
                        break;
                case 7:
                        y = 0;
                        board[y][x]   = 7;
                        board[y][x+1] = 7;
                        board[y][x-1] = 7;
                        board[y+1][x] = 7;
                        otype = 15;
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
                                                otype = 0;
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
                                                otype = 1;
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
                                                otype = 2;
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
                                                otype = 3;
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
                                                otype = 4;
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
                                                otype = 5;
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
                                                otype = 6;
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
                                                otype = 7;
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
                                                otype = 8;
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
                                                otype = 9;
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
                                                otype = 10;
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
                                                otype = 11;
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
                                                otype = 13;
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
                                                otype = 14;
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
                                                otype = 15;
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
                                                otype = 16;
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
                                                otype = 17;
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
                                                otype = 18;
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
                                for (int k = i-1; k >= 0; k--) {
                                        for (int m = 0; m < WIDTH; m++)
                                                board[k+1][m] = board[k][m];
                                }
                                score += 100;
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
}
