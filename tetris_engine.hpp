#ifndef tetris_engine
#define tetris_engine

const int HEIGHT = 22;
const int WIDTH  = 10;

int x, y;
int type, next_type, otype;
int score = 0, delay = 50;
int **board; //this is the current board

#endif