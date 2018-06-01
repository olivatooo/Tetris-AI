#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <omp.h>
#include <time.h>
#include "tetris_engine.hpp"
#include "genetic.hpp" //we are going to contain neural network as well as other genetic
                     //algorithm util subroutines
using namespace std;

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
			//add scoring guidelines here
			update_tetris(); //continue searching for more
			return ;
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
	for (int i = 0; i < WIDTH+2; i++)
		printf("%c ", '#');
	printf("\n");
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH+2; j++) {
			if (j == 0 || j == WIDTH + 1 ||
					board[i][j-1] != 0)
				printf("%c ", '#');
			else
				printf("%c ", '.');
		}
		printf("\n");
	}
	for (int i = 0; i < WIDTH+2; i++)
		printf("%c ", '#');
	printf("\n\n");
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

char coil_whine(int n) //fuck c++ delays write a coil whiner
{
	for (int i = 0; i < n*n; i++) {
		int b = 6;
		int c = 9;
		int a = b + c;
		b += a + c;
		c += a + b;
		a /= 2;
	}
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

void choose_board() //TDO this this section of code right now
{
	int **original = board;  //original board to copy off of
	int **curr_best = board; //the best board so far
	int min_penalty = 2140000000;
	//lets first do left and right translation
	for (int i = -WIDTH; i <= WIDTH; i++) {
		//lets now do rotations
		for (int j = 0; j <= 4; j++) {
			int **new_board = deep_copyover(original);
			for (int k = 0; k <= abs(i); k++) {
				if (i < 0)
					translate_left();
				else if (i > 0)
					translate_right();
			}
			for (int k = 0; k < j; k++)
				rotate();
			slam();

			if (get_penalty(trained_nn, new_board) < min_penalty) {
				min_penalty = get_penalty(trained_nn, new_board);
				if (curr_best != original)
					free_2darr(curr_best); //curr best is gone
				curr_best = new_board;
			} else { //curr_best stays
				free_2darr(new_board);
			}
		}
	}
	if (curr_best != original)
		free_2darr(original);
	board = curr_best;
} //choose the board with the least penalty and set that as global

int main()
{
	srand(time(NULL));
	next_type = rand() % 7 + 1;
	board = make_2darr(HEIGHT, WIDTH);

	generate();
	while (!end_game_checker()) {
		bool spawn = update_board();
		coil_whine(5000);
		if (spawn && check_board()) {
			freeze();
			update_tetris();
			generate();
		}
		printb(board);
	}
	free_2darr(board);
	return 0;
}

int main()
{
	int disp[HEIGHT][WIDTH] = {
	{0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,0,0,0}};

	int **focus = (int**)malloc(sizeof(int*));
	for (int i = 0; i < HEIGHT; i++) {
		focus[i] = (int*)malloc(sizeof(int));
		for (int j = 0; j < WIDTH; j++)
			focus[i][j] = disp[i][j];
	}
	printf("%d\n", holes(focus));
	return 0;
}

