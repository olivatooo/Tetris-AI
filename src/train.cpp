#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <omp.h>
#include "../include/genetic.hpp"
#include "../include/train.hpp"

using namespace std;

organism the_best_tetris_player = (struct organism) {0, 0, 0, 0, 0, 0, 0, 0};

const int PLACE_LIM         = 1000,
          TOTAL_GENERATIONS = 5,
          GAMES_PER_ORG     = 5,
          LINE_TH           = 0;

void reset_game(Tetris * t)
{
    t->rotate_cnt = 0;
    t->left_cnt   = 0;
    t->right_cnt  = 0;
    t->x         = -1;
    t->y         = -1;
    t->next_type = -1;
    t->type      = -1;
}

void adjust_fitness(Tetris * t, int id)
{
    population[id].fitness = t->lines_completed;
}

void place_pieces(Tetris * t, organism indiv)
{
    int placements = 0;
    reset_game(t);
    t->next_type = rand() % 7 + 1;
    t->board = t->make_2darr(HEIGHT, WIDTH); //creating our game board
    t->generate();
    while (!t->end_game_checker() && placements <= PLACE_LIM) {
        bool spawn = t->update_board();
        if (spawn && t->check_board()) {
            t->freeze();
            t->update_tetris();
            t->generate();
            t->reset_move_var();
            placements++;
        }
        if (t->y == DECISION_THRESHOLD)
            t->choose_moves(indiv);
        t->do_move();
    }
    t->free_2darr(t->board);
}

void print_train_info(int id, int gen)
{   /*
     * print:
     * - gen
     * - individual within gen
     * - genes
     * - rank #
     *
     * we can also allow the print_board method to print
     */
    std::cout << std::endl << "Generation: " << gen << std::endl;
    std::cout << "ID: " << id << std::endl;
    std::cout << "Genes: "    <<
            population[id].a << " " <<
            population[id].b << " " <<
            population[id].c << " " <<
            population[id].d << " " <<
            population[id].e << " " <<
            population[id].f << " " <<
            population[id].g << std::endl;
    std::cout << "fitness: " << population[id].fitness << std::endl;
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
    srand(time(nullptr));
    init_population();
    std::cout << "OUR INIT POPULATION:" << std::endl;
    print_sample_population();
    std::cout << "..." << std::endl;
    for (int i = 0; i < TOTAL_GENERATIONS; i++) {
        #pragma omp parallel for private(i)
        for (int j = 0; j < INIT_POPULATION; j++) {
            int tid = omp_get_thread_num();
            cout << "Processor (" << tid << ") running";
            int id  = j;
            int gen = i;
            Tetris t;
            int score_arr[GAMES_PER_ORG];
            int score_con[GAMES_PER_ORG];
            memset(score_arr, 0, sizeof(int) * GAMES_PER_ORG);
            memset(score_con, 0, sizeof(int) * GAMES_PER_ORG);
            for (int k = 0; k < GAMES_PER_ORG; k++) {
                place_pieces(&t, population[id]);
                score_con[k] = t.lines_completed;
                score_arr[k] = t.lines_completed;
                if (k > 0)
                    score_arr[k] = score_con[k] - score_con[k-1];

                if (t.lines_completed <= LINE_TH)
                    break;
            }
            //do cleanup after placing the pieces for this organism
            adjust_fitness(&t, id);
            print_train_info(id, gen);
            fflush(stdout);
        }
        //sort by fitness and reproduce here. introduce new genes
        std::cout << std::endl << "GENERATION: " << i << std::endl;
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
    return 0;
}

