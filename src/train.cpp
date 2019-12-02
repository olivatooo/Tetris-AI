#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <fstream>
#include <omp.h>
#include <mpi.h>
#include "../include/genetic.hpp"
#include "../include/train.hpp"
#include <unistd.h>

using namespace std;

organism the_best_tetris_player = (struct organism) {0, 0, 0, 0, 0, 0, 0, 0};

int PLACE_LIM = 100000;
int TOTAL_GENERATIONS = 10;
int GAMES_PER_ORG = 10;
int LINE_TH = 0;

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
    #pragma omp parallel for
    for(int offspring = 1; offspring < INIT_POPULATION/3 ; offspring++){   //33%
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
        population[offspring] = child;
    }
    int value = INIT_POPULATION/3;

    #pragma omp parallel for
    for (int i = value; i < INIT_POPULATION; i++) {
        population[i] = (organism) {RAND, RAND, RAND, RAND,RAND, RAND, RAND, 0};
    }
}

int main(int argc, char* argv[])
{
    int rank, size;
    if (argc == 1)
    {
        cout << "Usage:" << argv[0] << " num_generations number_of_games initial_population mutation_rate" << endl;
        cout << "Default: " << argv[0] << " " << TOTAL_GENERATIONS << " " << GAMES_PER_ORG << " " << INIT_POPULATION << " " << MUTATION_RATE << endl;
        exit(0);
    }
    if(argc > 1)
    {
        TOTAL_GENERATIONS = atoi(argv[1]);
    }
    if(argc > 2)
    {
        GAMES_PER_ORG = atoi(argv[2]);
    }
    if(argc > 3)
    {
        INIT_POPULATION = atoi(argv[3]);
    }
    if(argc > 4)
    {
        MUTATION_RATE = atof(argv[4]);
    }
    MPI_Init(&argc, &argv);
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );
    if (rank == 0) {
        cout << "Executing: " << argv[0] << " " << TOTAL_GENERATIONS << " " << GAMES_PER_ORG << " " << INIT_POPULATION << " " << MUTATION_RATE << endl;
        cout << "Number of threads: " << omp_get_num_threads() << endl;
        cout << "Size: " << size << endl;
        usleep(50000);
    }
    srand(time(nullptr));
    if (rank == 0) {
        init_population();
        std::cout << "OUR INIT POPULATION:" << std::endl;
        print_sample_population();
        std::cout << "..." << std::endl;
    }

    MPI_Datatype dt_organism;
    MPI_Datatype types[8] = {
        MPI_FLOAT,
        MPI_FLOAT,
        MPI_FLOAT,
        MPI_FLOAT,
        MPI_FLOAT,
        MPI_FLOAT,
        MPI_FLOAT,
        MPI_INT
    };
    MPI_Aint displacements[8]  = {
        offsetof(organism, a),
        offsetof(organism, b),
        offsetof(organism, c),
        offsetof(organism, d),
        offsetof(organism, e),
        offsetof(organism, f),
        offsetof(organism, g),
        offsetof(organism, fitness)
    };
    int block_lenth[8] = {1,1,1,1,1,1,1,1};
    MPI_Type_create_struct(8, block_lenth, displacements, types, &dt_organism);
    MPI_Type_commit(&dt_organism);
    
    for (int generation = 0; generation < TOTAL_GENERATIONS; generation++) {

        organism *divided_individuals = (organism*)malloc((INIT_POPULATION/size)*sizeof(organism));

        MPI_Scatter(population, INIT_POPULATION/size,
            dt_organism, divided_individuals, INIT_POPULATION/size,
            dt_organism, 0, MPI_COMM_WORLD);

        #pragma omp parallel for schedule(dynamic)
        for (int individual = 0; individual < INIT_POPULATION/size; individual++) {
            Tetris t;
            int score_arr[GAMES_PER_ORG];
            int score_con[GAMES_PER_ORG];
            memset(score_arr, 0, sizeof(int) * GAMES_PER_ORG);
            memset(score_con, 0, sizeof(int) * GAMES_PER_ORG);
            for (int k = 0; k < GAMES_PER_ORG; k++) {
                place_pieces(&t, divided_individuals[individual]);
                score_con[k] = t.lines_completed;
                score_arr[k] = t.lines_completed;
                if (k > 0)
                    score_arr[k] = score_con[k] - score_con[k-1];

                if (t.lines_completed <= LINE_TH)
                    break;
            }
            //do cleanup after placing the pieces for this organism
            divided_individuals[individual].fitness = t.lines_completed;
            // if (individual % (INIT_POPULATION/5) == 0)
            //     print_train_info(individual, generation);
            // fflush(stdout);
        }

        MPI_Gather(divided_individuals, INIT_POPULATION/size,
            dt_organism, population, INIT_POPULATION/size,
            dt_organism, 0, MPI_COMM_WORLD);

        free(divided_individuals);

        if (rank == 0) {
            //sort by fitness and reproduce here. introduce new genes
            std::cout << std::endl << "GENERATION: " << generation << std::endl;
            sort_population();
            print_sample_population();
            //after sort compare if we have beaten the best player
            if (population[0].fitness > the_best_tetris_player.fitness)
                the_best_tetris_player = population[0];
            reproduce();
            

            //write params to file for future use ====>
            organism output = the_best_tetris_player;
            ofstream param_file;
            param_file.open("parameters", ios::trunc);
            param_file << output.a << " " <<
                    output.b << " " <<
                    output.c << " " <<
                    output.d << " " <<
                    output.e << " " <<
                    output.f << " " <<
                    output.g << " " << "\n";
            param_file.close();
        }
    }
    //    <====

    
    //cleanup
    if (rank == 0) {
        // print_sample_population(); 
        free(population);
    }
    MPI_Finalize();
    return 0;
}

