#ifndef genetic
#define genetic

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
        float a, b, c, d;
};

extern organism *population, trained_nn;

extern int aggregate_height(int **focus);
extern int complete_lines(int **focus);
extern int holes(int **focus);
extern int bumpiness(int **focus);
extern float get_penalty(struct organism nn, int **focus); //feed forward neural network

#endif
