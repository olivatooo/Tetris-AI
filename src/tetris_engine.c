#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <omp.h>
#include <time.h>
#include <unistd.h>
#include "../include/tetris_engine.hpp"
#include "../include/genetic.hpp"
#include "../include/train.hpp"
/*
 * we are going to contain neural network as well as other genetic
 * algorithm util subroutines
 *
 * TODO: link algorithm with java GUI
 */

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

using namespace std;
const int HEIGHT = 28;
const int WIDTH  = 10;
const int DECISION_THRESHOLD = 4;
//at least y is equal to this value before deciding moves

int x, y;
int type, next_type, otype;
int score = 0, delay = 50, lines_completed = 0;
int **board; //this is the current board
int rotate_cnt = 0, left_cnt = 0, right_cnt = 0;
//here are the current moves that have to go underway

void generate()
{
    type = next_type;
    next_type = (int)(rand() % 7) + 1;
    x = WIDTH / 2;
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
        board[y][x]     = 2;
        board[y+1][x]   = 2;
        board[y-1][x]   = 2;
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

void translate_right()
{
    char invalid_move = 0;
    for (int i = 0; i < HEIGHT; i++) { //check if a valid move can be made
        for (int j = WIDTH-1; j >= 0; j--) {
            if (board[i][j] > 0 && (j == WIDTH - 1 || board[i][j+1] < 0))
                invalid_move = 1;
        }
    }
    if (invalid_move)
        return;
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = WIDTH-1; j >= 0; j--) {
            if (board[i][j] > 0 && j < WIDTH - 1 && board[i][j+1] == 0) {
                board[i][j+1] = board[i][j];
                board[i][j] = 0; //leave empty for next piece
            }
        }
    }
    x++; //update x value
}

void translate_left()
{
    char invalid_move = 0;
    for (int i = 0; i < HEIGHT; i++) { //check if a valid move can be made
        for (int j = 0; j < WIDTH; j++) {
            if (board[i][j] > 0 && (j == 0 || board[i][j-1] < 0))
                invalid_move = 1;
        }
    }
    if (invalid_move)
        return;
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (board[i][j] > 0 && j > 0 && board[i][j-1] == 0) {
                board[i][j-1] = board[i][j];
                board[i][j] = 0; //leave empty for next piece
            }
        }
    }
    x--; //update x value
}

char safe(int y, int x)
{
    return y >= 0 && y < HEIGHT && x >= 0 && x < WIDTH && board[y][x] >= 0;
}

void clear(int y1, int x1, int y2, int x2)
{
    for (int i = y1; i <= y2; i++) {
        for (int j = x1; j<= x2; j++) {
            if (board[i][j] > 0) //clear active blocks only
                board[i][j] = 0;
        }
    }
}

void rotate() //to keep things simple we have one type of rotate only
{
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
                clear(y-1, x, y+1, x+1);
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
                clear(y, x-1, y+1, x+1);
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
                clear(y-1, x-1, y+1, x);
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
                clear(y-1, x-1, y, x+1);
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
                clear(y-1, x-1, y+1, x);
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
                clear(y-1, x-1, y, x+1);
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
                clear(y-1, x, y+1, x+1);
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
                clear(y, x-1, y+1, x+1);
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
                clear(y, x-1, y+1, x+1);
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
                clear(y-1, x-1, y+1, x);
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
                clear(y, x-1, y+1, x+1);
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
                clear(y-1, x, y+1, x+1);
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
                clear(y, x-1, y, x+2);
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
                clear(y-2, x, y+1, x);
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
                clear(y, x-1, y+1, x+1);
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
                clear(y-1, x-1, y+1, x);
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
                clear(y-1, x-1, y, x+1);
                board[y][x]   = 7;
                board[y][x+1] = 7;board[y][x] = 7; board[y][x] = 7;
                board[y-1][x] = 7;
                board[y+1][x] = 7;
                otype = 17;
            }
        } else {
            if (safe(y, x) &&
                safe(y+1, x) &&
                safe(y, x+1) &&
                safe(y, x-1)) {
                clear(y-1, x, y+1, x+1);
                board[y][x]   = 7;
                board[y+1][x] = 7;
                board[y][x+1] = 7;
                board[y][x-1] = 7;
                otype = 18;
            }
        }
        break;
    }
}

void update_tetris()
{
    //look for completed line(s)
    for (int i = HEIGHT-1; i >= 0; i--) {
        char row_found = 1;
        for (int j = 0; j < WIDTH && row_found; j++)
            row_found = board[i][j] != 0;
        if (row_found) {
            for (int k = i-1; k >= 0; k--) {
                for (int m = 0; m < WIDTH; m++)
                    board[k+1][m] = board[k][m];
            }
            lines_completed++;
            update_tetris(); //continue searching for more
        }
    }
}

bool check_board()
{
    for (int i = HEIGHT-1; i >= 0; i--) {
        for (int j = 0; j < WIDTH; j++) {
            if (board[i][j] > 0 && i == HEIGHT-1 ||
                board[i][j] > 0 && i < HEIGHT-1 && board[i+1][j] < 0)
            return true;
        }
    }
    return false;
}

bool update_board()
{
    bool ret = false;
    if (check_board()) //if we hit dead end
        return true;
    for (int i = HEIGHT-1; i >= 0; i--) {
        for (int j = 0; j < WIDTH; j++) {
            if (board[i][j] > 0 && board[i+1][j] == 0) {
                board[i+1][j] = board[i][j];
                board[i][j] = 0;
            }
        }
    }

    y++; //update y coor
    return false; //no dead end was encountered
}

void freeze()
{
    //turn all the active blocks into inactive ones
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (board[i][j] > 0)
                board[i][j] *= -1;
        }
    }
}

bool end_game_checker()
{
    for (int i = 0; i < WIDTH; i++) {
        if (board[2][i] < 0)
            return true;
    }
    return false;
}

void printb(int **board)
{
    printf("+ ");
    for (int i = 0; i < WIDTH; i++)
        printf("%c ", '-');
    printf("+");
    printf("\n");
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH+2; j++) {
            if (j == 0 || j == WIDTH + 1)
                printf("%c ", '|');
            else if (board[i][j-1])
                printf("%c ", '#');
            else
                printf("%c ", ' ');
        }
        printf("\n");
    }
    printf("+ ");
    for (int i = 0; i < WIDTH; i++)
        printf("%c ", '-');
    printf("+ "); printf(" lines: %d", lines_completed);
    printf("\n\n\n\n");
}

int** make_2darr(int h, int w)
{
    int **ret = (int**)malloc(sizeof(int*) * h);
    for (int i = 0; i < h; i++) {
        ret[i] = (int*)malloc(sizeof(int) * w);
        memset(ret[i], 0, sizeof(int) * w);
    }
    return ret;
}

int** deep_copyover(int **original)
{
    int **ret = (int**)malloc(sizeof(int*) * HEIGHT);
    for (int i = 0; i < HEIGHT; i++)
        ret[i] = (int*)malloc(sizeof(int) * WIDTH);
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++)
            ret[i][j] = original[i][j];
    }
    return ret;
} //return location to a copy of board in heap memory

void free_2darr(int **ptr)
{
    for (int i = 0; i < HEIGHT; i++)
        free(ptr[i]);
    free(ptr);
}

void slam() //let block reach the bottom
{
    while (!check_board())
            update_board();
}
//this is going to be our minimax algorithm
void choose_moves(organism nn)
{
    int **curr_board = board; //this is going to be the original
    int curr_y = y;
    int curr_x = x;
    double least_penalty = 0xFFFFF; //set pen to curr
    char preview = 1;
    //set everything to no moves as best
    for (int i = 0; i < 4; i++) {
        //choose  how many rotations from current state yield least penalty
        for (int j = 0; j <= WIDTH; j++) {
            //choose how many left shifts from current state yield least
            board = deep_copyover(curr_board); //set board to new
            y = curr_y;
            x = curr_x;

            for (int k = 0; k < i; k++)
                rotate();
            for (int k = 0; k < j; k++)
                translate_left();
            slam();

            double stackpen = get_penalty(nn, board); //here we choose what nn to use for our penalty calculation
            //penalty in this stack
            if (stackpen < least_penalty) {
                least_penalty = stackpen;
                left_cnt   = j;
                right_cnt  = 0;
                rotate_cnt = i;
            }
            free_2darr(board); //free this board
        }

        for (int j = 0; j <= WIDTH; j++) {
            //choose how many right shifts from current state yield least
            board = deep_copyover(curr_board); //set board to new
            y = curr_y;
            x = curr_x;

            for (int k = 0; k < i; k++)
                rotate();
            for (int k = 0; k < j; k++)
                translate_right();
            slam();

            double stackpen = get_penalty(nn, board);
            //penalty in this stack
            if (stackpen < least_penalty) {
                least_penalty = stackpen;
                right_cnt  = j;
                left_cnt   = 0;
                rotate_cnt = i;
            }
            free_2darr(board); //free this board
        }

    }
    y     = curr_y;
    x     = curr_x;
    board = curr_board; //set back to old board
}

void do_move()
{
    //printf("%d %d %d\n", right_cnt, left_cnt, rotate_cnt);
    //fflush(stdout);
    if (rotate_cnt > 0) {
        rotate();
        rotate_cnt--;
    } else if (right_cnt > 0) {
        translate_right();
        right_cnt--;
    } else if (left_cnt > 0) {
        translate_left();
        left_cnt--;
    }
}

void reset_move_var()
{
    left_cnt   = 0;
    right_cnt  = 0;
    rotate_cnt = 0;
}

