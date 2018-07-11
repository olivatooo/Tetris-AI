#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctime>
#include "genetic.hpp"
#include "tetris_engine.hpp"

const int INIT_POPULATION = 5000,
	  	  SAMPLE_SIZE 	  = 10;

const float MUTATION_RATE = ;
//sample size is how much we should be printing
organism trained_nn = (organism) {0.510066, -0.760666, 0.35663, 0.19448, 69};
/*
 * the above organism is a pre-trained value we are using as reference
 * we should actually read this from file, this is actually a pre-trained value rn,
 * this is actually an example. we should keep the best organisms in
 * an array of vecs. don't forget 69 is just an arbitrary value I placed for fitness
 */
organism *population; //don't forget to free the population that got elim
organism  best_in_gen; //keep track of fittest individual in each gen and write it out to a file after

void init_population()
{
	//lets seed the rand()
	srand(time(NULL));
	population = (organism*) malloc(sizeof(organism) * INIT_POPULATION);
	for (int i = 0; i < INIT_POPULATION; i++)
		population[i] = (organism) {RAND, RAND, RAND, RAND, 0};
}

void print_sample_population()
{
	for (int i = 0; i < SAMPLE_SIZE; i++) {
		printf("individual %d: %f, %f, %f, %f\n", i+1, population[i].a,
			population[i].b, population[i].c, population[i].d);
	}
}

void crossover(organism a, organism b,
		organism *child1, organism *child2) //aka scramble child genes
{
	child1.a = a.a; // <----+
	child1.b = a.b; //      |
	child1.c = b.c; //      |
	child1.d = b.d; // <---+|
					//     ||
	child2.a = b.a; // <---+|
	child2.b = b.b; //      |
	child2.c = a.c; //      |
	child2.d = a.d; // <----+
}

void mutate(organism *child)
{
	
}

//===== here begins the penalty calculations for the individual

int aggregate_height(int **focus)
{
	int total = 0;
	for (int i = 0; i < WIDTH; i++) {
		int k = 0;
		for (int j = HEIGHT-1; j >= 0; j--){
			if (focus[j][i])
				k = HEIGHT - j;
		}
		total += k;
	}
	return total;
}

int complete_lines(int **focus)
{
	//look for completed line(s)
	for (int i = HEIGHT-1; i >= 0; i--) {
		char row_found = 1;
		for (int j = 0; j < WIDTH && row_found; j++)
			row_found = focus[i][j] != 0;
		if (row_found) {
			for (int k = i-1; k >= 0; k--) {
				for (int m = 0; m < WIDTH; m++)
					focus[k+1][m] = focus[k][m];
			}
			return complete_lines(focus) + 1;
		}
	}
	return 0;
}

int holes(int **focus)
{
	int cntr = 0;
	for (int i = 1; i < HEIGHT; i++) {
		for (int j = 0; j < WIDTH; j++) {
			if (!focus[i][j]) {
				for (int k = i; k >= 0; k--) {
					if (focus[k][j]) {
						cntr++;
						break;
					}
				}
			}
		}
	}
	return cntr;
}

int bumpiness(int **focus)
{
	int height[WIDTH];
	for (int i = 0; i < WIDTH; i++) {
		int cntr = 0;
		for (int j = HEIGHT-1; j >= 0; j--) {
			if (focus[j][i])
				cntr = HEIGHT - j;
		}
		height[i] = cntr;
	}

	int bumpiness = 0;
	for (int i = 1; i < WIDTH; i++)
		bumpiness += abs(height[i] - height[i-1]);
	return bumpiness;
}

double get_penalty(organism nn, int **focus) //feedforward
{
	return nn.a * aggregate_height(focus) + nn.b * complete_lines(focus) +
		nn.c * holes(focus) + nn.d * bumpiness(focus);
}

