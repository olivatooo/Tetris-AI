#ifndef train
#define train
#include "./Tetris.h"
extern int PLACE_LIM; //the number of block placements per game
extern int TOTAL_GENERATIONS;
extern int GAMES_PER_ORG;

extern int placements; //lets use this as fitness measurement
extern short rank;

void reset_game(Tetris * t);
void adjust_fitness(Tetris * t, int id);
void place_pieces(Tetris * t, organism indiv);
void print_train_info(int id, int gen);
void sort_population();

#endif

