#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <fstream>
#include "tetris_engine.hpp"
#include "genetic.hpp"
#include "train.hpp"
using namespace std;

int placements, lines_completed;
short gen, id, rank;
const int PLACE_LIM         = 10000,
		  TOTAL_GENERATIONS = 10;

void reset_game()
{
	rotate_cnt = 0;
	left_cnt   = 0;
	right_cnt  = 0;
	score      = 0;
	lines_completed = 0;
	placements      = 0;
	x         = -1;
	y         = -1;
	next_type = -1;
	otype     = -1;
	type      = -1;
}

void adjust_fitness()
{
	population[id].fitness = placements + lines_completed;
}

void place_pieces(organism indiv)
{
	reset_game();
	srand(time(NULL));
	next_type = rand() % 7 + 1;
	board = make_2darr(HEIGHT, WIDTH); //creating our game board
	generate();
	while (!end_game_checker() && placements <= PLACE_LIM) {
		bool spawn = update_board();
		if (spawn && check_board()) {
			freeze();
			lines_completed += update_tetris();
			generate();
			reset_move_var();
			placements++;
		}
		if (y == DECISION_THRESHOLD)
			choose_moves(indiv);
		do_move();
		//printb(board); <--- enable this to see the board every single time
	}
	free_2darr(board); //free out game board for next use
	//here we are going to assign the rank as well as the fitness
}

void print_train_info()
{   /*
	 * print:
	 * - gen
	 * - individual within gen
	 * - genes
	 * - rank #
	 *
	 * we can also allow the print_board method to print
	 */
	std::cout << "gener: " << gen << std::endl;
	std::cout << "idivi: " << id << std::endl;
	std::cout << "genes: "    <<
			population[id].a << " " <<
			population[id].b << " " <<
			population[id].c << " " <<
			population[id].d << std::endl;
	std::cout << "fitns: " << population[id].fitness << std::endl;
	std::cout << std::endl;
}

void sort_population()
{
	for (int i = 0; i < INIT_POPULATION; i++) {
		for (int j = INIT_POPULATION - 1; j > i; j--) {
			if (population[j].fitness > population[i].fitness) {
				organism tmp  = population[i];
				population[i] = population[j];
				population[j] = tmp;
			}
		}
	}
	best_in_gen = population[0];
}

void reproduce()
{
	for (int i = 0; i < INIT_POPULATION/2; i++) {
		int par1 = 0, par2 = 0;
		while (par1 == par2) {
			par1 = rand() % (INIT_POPULATION/2);
			par2 = rand() % (INIT_POPULATION/2);
		}
		organism child1, child2;
		crossover(&population[par1], &population[par2],
				&child1, &child2);
		mutate(&child1);
		mutate(&child2);
		child1.fitness = 0;
		child2.fitness = 0;
		population[par1] = child1;
		population[par2] = child2;
	}

	for (int i = INIT_POPULATION/2; i < INIT_POPULATION; i++) {
		population[i] = (organism) {RAND, RAND, RAND, RAND, 0};
	}
}

int main()
{
	init_population();
	std::cout << "OUR INIT POPULATION:" << std::endl;
	print_sample_population();
	std::cout << "..." << std::endl;
	for (int i = 0; i < TOTAL_GENERATIONS; i++) {
		for (int j = 0; j < INIT_POPULATION; j++) {
			id  = j;
			gen = i;
			place_pieces(population[id]);
			//do cleanup after placing the pieces for this organism
			adjust_fitness();
			print_train_info();
		}
		//sort by fitness and reproduce here. introduce new genes
		std::cout << "GENERATION: " << i << std::endl;
		sort_population();
		print_sample_population();

		reproduce();
	}
	//write params to file for future use ====>
	organism output = population[0];
	ofstream param_file; param_file.open("parameters", ios::trunc);
	param_file << output.a << " " <<
				  output.b << " " <<
				  output.c << " " << output.d << "\n";
	param_file.close();
	//    <====
	print_sample_population();
	//cleanup
	free(population);
	return 0; //TODO write a python script after to show convergence
}

