#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "genetic.hpp"
#include "tetris_engine.hpp"

organism trained_nn = (organism){1, 1, 1, 1}; //we should actually read this from file
organism *population;

int aggregate_height(int **focus)
{
	int total = 0;
	for (int i = 0; i < WIDTH; i++) {
		int k = 0;
		for (int j = HEIGHT; j >= 0; j--){
			if (focus[j][i] != 0)
				k = HEIGHT - j + 1;
		}
		total += k;
	}
}

int complete_lines(int **focus) 
//this is essentailly copied over from the update tetris program
{
	for (int i = HEIGHT-1; i >= 0; i--) {
		char row_found = 1;
		for (int j = 0; j < WIDTH && row_found; j++)
			row_found = focus[i][j] != 0;
                if (!row_found)
                        continue;

		for (int k = i-1; k >= 0; k--) {
	}
	return 0;
}

int holes(int **focus)
{
	int hole[HEIGHT][WIDTH];
	for (int i = 0; i < HEIGHT; i++)
		memset(hole[i], 0, sizeof(int));
	for (int i = 1; i < HEIGHT; i++) {
		//start from one because there cannot be a hole the first line
		for (int j = 0; j < WIDTH; j++) {
			if (board[i][j] == 0 && hole[i-1][j] == 0)
				continue;
			hole[i][j] = 1;
		}
	}

	int counter = 0;
	for (int i = 0; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			if (hole[i][j] && !board[i][j])	
				counter++;
		}
	}
	return counter;
}

int bumpiness(int **focus)
{

}

float get_penalty(organism nn, int **focus) //feedforward
{
	return nn.a * aggregate_height(focus) + nn.b * complete_lines(focus) +
		nn.c * holes(focus) + nn.d * bumpiness(focus);
}

//for testing these functions here
int main()
{
	int focus[HEIGHT][WIDTH] = {
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
	{0,0,0,0,0,0,0,0,0,0},
	}
	printf("%d\n", holes(focus));
	return 0;
}
