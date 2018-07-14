#ifndef genetic
#define genetic

#define RAND (((double) rand() / RAND_MAX) * 2 - 1) //between -1 and 1

/*
 * so the way we set up the genetic algorithm is by optimizing a penalty calculater,
 * The way that the penalty calculator is going to work is by analyzing the different
 * possible board configurations and choosing the one that has the least penalty as the
 * global. The global double pointer will then be set to the board allocated on heap.
 * Therefore, the additional boards should be deleted asap.
 *
 * penalty = a * (aggregate height) + b * (complete lines) + c * (holes) + d * (bumpiness)
 *
 * Here is the description of the following vocab
 *
 * - aggregate height is the height of each botton to top of the board
 * - complete lines is the number of complete lines
 * - holes are any empty space with block above it
 * - bumpiness is the absolute difference from each of the adjacent columns
 */

typedef struct organism organism; //make my life easier
struct organism {                 //this is a neural network
        double a, b, c, d, fitness; //a,b,c,d are genes
};

extern organism *population, trained_nn, best_in_gen;
extern const int INIT_POPULATION, SAMPLE_SIZE;
extern const float MUTATION_RATE;

//some functions for genetic alg
void init_population();
void print_sample_population();
void crossover(organism *a, organism *b,
		organism *child1, organism *child2);
void mutate(organism *child);

int aggregate_height(int **focus);
int complete_lines(int **focus);
int holes(int **focus);
int bumpiness(int **focus);
double get_penalty(struct organism nn, int **focus); //feed forward neural network

#endif

