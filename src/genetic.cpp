#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include "../include/genetic.hpp"
#include "../include/Tetris.h"

using namespace std;
int INIT_POPULATION = 512; //one part reproduce, one part introduced
int SAMPLE_SIZE = 10;
double MUTATION_RATE = 0.02f;

/*
 * the above organism is a pre-trained value we are using as reference
 * this is actually an example. we should keep the best organisms in a list
 * more importantly pay close attention to the variables below
 */

organism *population;  //don't forget to free the population that got elim

void init_population()
{
    //lets seed the rand()
    srand(time(NULL));
    population = (organism*) malloc(sizeof(organism) * INIT_POPULATION);
    for (int i = 0; i < INIT_POPULATION; i++)
        population[i] = (organism) {RAND, RAND, RAND, RAND,
                RAND, RAND, RAND, 0};
}

void print_sample_population()
{
    for (int i = 0; i < SAMPLE_SIZE; i++) {
        printf("individual %d: %f, %f, %f, %f, %f, %f, %f, %d\n", i+1, population[i].a,
                population[i].b, population[i].c,
                population[i].d, population[i].e,
                population[i].f, population[i].g, population[i].fitness);
    }
}

void weighted_crossover(const organism p1, const organism p2, organism *child)
{
    double p1w = (double) p1.fitness / (double) (p1.fitness + p2.fitness);
    double p2w = (double) p2.fitness / (double) (p1.fitness + p2.fitness);

    child->a = p1w * p1.a + p2w * p2.a;
    child->b = p1w * p1.b + p2w * p2.b;
    child->c = p1w * p1.c + p2w * p2.c;
    child->d = p1w * p1.d + p2w * p2.d;

    child->e = p1w * p1.e + p2w * p2.e;
    child->f = p1w * p1.f + p2w * p2.f;
    child->g = p1w * p1.g + p2w * p2.g;
}

void mutate(organism *child)
{
    int MUTATION_WEIGHT = 1;
    if(RAND_0 <= MUTATION_RATE) {
        child->a += (RAND * MUTATION_WEIGHT) * child->a;
        child->b += (RAND * MUTATION_WEIGHT) * child->b;
        child->c += (RAND * MUTATION_WEIGHT) * child->c;
        child->d += (RAND * MUTATION_WEIGHT) * child->d;
        child->e += (RAND * MUTATION_WEIGHT) * child->e;
        child->f += (RAND * MUTATION_WEIGHT) * child->f;
        child->g += (RAND * MUTATION_WEIGHT) * child->g;
    }
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
            if (focus[i][j] != 0)
                continue;
            for (int k = i; k >= 0; k--) {
                if (focus[k][j] != 0) {
                    cntr++;
                    break;
                }
            }
        }
    }
    return cntr;
}

int bumpiness(int **focus)
{
    int height_memo[WIDTH];
    memset(height_memo, 0, sizeof(int) * WIDTH);
    for (int j = 0; j < WIDTH; j++) {
        for (int i = 0; i < HEIGHT; i++) {
            if (focus[i][j] != 0) {
                height_memo[j] = HEIGHT - i;
                break;
            }
        }
    }
    int bumpiness = 0;
    for (int i = 1; i < WIDTH; i++)
        bumpiness += abs(height_memo[i] - height_memo[i-1]);
    return bumpiness;
}

int right_wall(int **focus)
{
    int cntr = 0;
    for (int i = 0; i < HEIGHT; i++)
        cntr += (int) (focus[i][WIDTH-1] == 1 ? 1 : 0);
    return cntr;
}

int left_wall(int **focus)
{
    int cntr = 0;
    for (int i = 0; i < HEIGHT; i++)
        cntr += (int) (focus[i][0] == 1 ? 1 : 0);
    return cntr;
}

int max_height(int **focus)
{
    int ret = 0;
    for (int i = 0; i < WIDTH; i++) {
        int k = 0;
        for (int j = HEIGHT-1; j >= 0; j--){
            if (focus[j][i])
                k = HEIGHT - j;
        }
        if (k > ret)
            ret = k;
    }
    return ret;
}

double get_penalty(organism nn, int **focus)
{
    return nn.b  * complete_lines(focus)+ nn.a * aggregate_height(focus) +
            nn.c * holes(focus)         + nn.d * bumpiness(focus)        +
            nn.e * max_height(focus)   + nn.f * left_wall(focus)        +
            nn.g * right_wall(focus);
}

