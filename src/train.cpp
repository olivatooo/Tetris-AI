#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <fstream>
#include <omp.h>
#include <unistd.h>
#include "../include/tetris_engine.h"
#include "../include/genetic.h"
#include "../include/train.h"
using namespace std;

organism the_best_tetris_player = (struct organism) {0, 0, 0, 0, 0, 0, 0, 0};
int placements;
short gen, id, rank;
const int PLACE_LIM         = 1000,
		  TOTAL_GENERATIONS = 5,
		  GAMES_PER_ORG     = 50,
		  LINE_TH			= 0;

void reset_game()
{
	rotate_cnt = 0;
	left_cnt   = 0;
	right_cnt  = 0;
	score      = 0;
	placements = 0;
	x         = -1;
	y         = -1;
	next_type = -1;
	otype     = -1;
	type      = -1;
}

void adjust_fitness()
{
	population[id].fitness = lines_completed;
}

void place_pieces(organism indiv)
{
	reset_game();
	next_type = rand() % 7 + 1;
	board = make_2darr(HEIGHT, WIDTH); //creating our game board
	generate();
	while (!end_game_checker() && placements <= PLACE_LIM) {
		bool spawn = update_board();
		if (spawn && check_board()) {
			freeze();
			update_tetris();
			generate();
			reset_move_var();
			placements++;
		}
		if (y == DECISION_THRESHOLD)
			choose_moves(indiv);
		do_move();
		//printb(board); //<--- enable this to see the board every single time
	}
	free_2darr(board);
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
			population[id].d << " " <<
			population[id].e << " " <<
			population[id].f << " " <<
			population[id].g << std::endl;
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
	/*
	 * take 10% of the population at a time, take the 2 fittest infividuals from
	 * that 10% and use them to crossover a child. Once we have 33% of population
	 * that has crossed over, stop. remove and regenerate the last 33%
	 */
	int offspring = 0;
	while (offspring < INIT_POPULATION/3) {   //33%
		int tenth[INIT_POPULATION / 10]; //10%
		organism par_a = (organism) {0, 0, 0, 0, 0, 0, 0, 0};
		organism par_b = (organism) {0, 0, 0, 0, 0, 0, 0, 0};
		organism child = (organism) {0, 0, 0, 0, 0, 0, 0, 0};
		for (int i = 0; i < INIT_POPULATION / 10; i++) {
			tenth[i] = rand() % INIT_POPULATION;
			if (population[tenth[i]].fitness > par_a.fitness)
				par_a = population[tenth[i]];
			else if (population[tenth[i]].fitness > par_b.fitness)
				par_b = population[tenth[i]];
		}
		weighted_crossover(par_a, par_b, &child);
		mutate(&child);
		offspring++;
		population[offspring] = child;
	}
	for (int i = INIT_POPULATION/3; i < INIT_POPULATION; i++) {
		population[i] = (organism) {RAND, RAND, RAND, RAND,
				RAND, RAND, RAND, 0};
	}
}

int main()
{
	srand(time(NULL));
	init_population();
	std::cout << "OUR INIT POPULATION:" << std::endl;
	print_sample_population();
	std::cout << "..." << std::endl;
	for (int i = 0; i < TOTAL_GENERATIONS; i++) {
		for (int j = 0; j < INIT_POPULATION; j++) {
			id  = j;
			gen = i;
			lines_completed = 0; //important

			int score_arr[GAMES_PER_ORG];
			int score_con[GAMES_PER_ORG];
			memset(score_arr, 0, sizeof(int) * GAMES_PER_ORG);
			memset(score_con, 0, sizeof(int) * GAMES_PER_ORG);
			for (int k = 0; k < GAMES_PER_ORG; k++) {
				place_pieces(population[id]);

				score_con[k] = lines_completed;
				score_arr[k] = lines_completed;
				if (k > 0)
					score_arr[k] = score_con[k] - score_con[k-1];

				if (lines_completed <= LINE_TH)
					break;
			}

			//do cleanup after placing the pieces for this organism
			adjust_fitness();
			for (int k = 0; k < GAMES_PER_ORG; k++)
				printf("%d ", score_arr[k]);
			cout << endl;
			print_train_info();

			fflush(stdout);
		}
		//sort by fitness and reproduce here. introduce new genes
		std::cout << "GENERATION: " << i << std::endl;
		sort_population();
		print_sample_population();
		//after sort compare if we have beaten the best player
		if (population[0].fitness > the_best_tetris_player.fitness)
			the_best_tetris_player = population[0];
		reproduce();
	}
	//write params to file for future use ====>
	organism output = the_best_tetris_player;
	ofstream param_file; param_file.open("parameters", ios::trunc);
	param_file << output.a << " " <<
				  output.b << " " <<
				  output.c << " " <<
				  output.d << " " <<
				  output.e << " " <<
				  output.f << " " <<
				  output.g << " " << "\n";
	param_file.close();
	//    <====
	print_sample_population();
	//cleanup
	free(population);
	return 0; //TODO write a python script after to show convergence
}

