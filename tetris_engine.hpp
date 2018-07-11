#ifndef tetris_engine
#define tetris_engine

extern const int HEIGHT;
extern const int WIDTH;

extern int x, y;
extern int type, next_type, otype;
extern int score, delay;
extern int **board; //this is the current board

void generate();
void translate_right();
void translate_left();

char safe(int y, int x);
void clear(int y1, int x1, int y2, int x2);
void rotate();

bool update_board();
bool check_board();
void freeze();
bool end_game_checker();

void printb(int **board);
int** make_2darr(int h, int w);

char coil_whine(int n);
int** deep_copyover(int **original);
void free_2darr(int **ptr);

void choose_moves();
void do_move();
void reset_move_var();
#endif

