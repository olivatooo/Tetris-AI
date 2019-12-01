//
// Created by olivato on 30/11/2019.
//

#ifndef TETRIS_AI_TETRIS_H
#define TETRIS_AI_TETRIS_H
#include "genetic.hpp"

extern const int DECISION_THRESHOLD;
extern const int HEIGHT;
extern const int WIDTH;

class Tetris {
    public:
        const int DECISION_THRESHOLD = 4;
        int x;
        int y;
        int type;
        int next_type;
        int otype;
        int score = 0;
        int delay = 50;
        int lines_completed = 0;
        int **board;
        int rotate_cnt = 0;
        int left_cnt = 0;
        int right_cnt = 0;

        void generate();
        void translate_right();
        void translate_left();
        void slam();

        char safe(int y, int x);
        void clear(int y1, int x1, int y2, int x2);
        void rotate();

        void  update_tetris();
        bool update_board();
        bool check_board();
        void freeze();
        bool end_game_checker();

        void  printb(int **board);
        int** make_2darr(int h, int w);

        char  coil_whine(int n);
        int** deep_copyover(int **original);
        void  free_2darr(int **ptr);

        void choose_moves(organism nn);
        void do_move();
        void reset_move_var();
};


#endif //TETRIS_AI_TETRIS_H
