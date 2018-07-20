#ifndef train
#define train
#include "tetris_engine.h"

extern const int PLACE_LIM; //the number of block placements per game
extern const int TOTAL_GENERATIONS;
extern const int GAMES_PER_ORG;

extern int placements; //lets use this as fitness measurement
extern short gen, id, rank;

void reset_game();
void adjust_fitness();
void place_pieces(organism indiv);
void print_train_info();
void sort_population();

#endif

