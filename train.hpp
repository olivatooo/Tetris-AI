#ifndef train
#define train

#include "tetris_engine.hpp"

extern const int PLACE_LIM; //the number of block placements per game
extern const int TOTAL_GENERATIONS;

extern int placements, lines_completed; //lets use this as fitness measurement
extern short gen, id, rank;

void reset_game();
void adjust_fitness();
void place_pieces();
void print_train_info();
void sort_population();

#endif

