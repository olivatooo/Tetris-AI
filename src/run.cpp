#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <unistd.h>
#include "../include/genetic.hpp"
#include "../include/Tetris.h"

using namespace std;

int placements = 0;
int main()
{
    Tetris t;
    srand(time(nullptr));
    t.next_type = rand() % 7 + 1;
    t.board = t.make_2darr(HEIGHT, WIDTH);
    organism curr_org = {0, 0, 0, 0, 0, 0, 0};
    freopen("parameters", "r", stdin);
    cin >> curr_org.a >> curr_org.b >> curr_org.c >> curr_org.d
        >> curr_org.e >> curr_org.f >> curr_org.g;

    t.generate();
    while (!t.end_game_checker()) {
        bool spawn = t.update_board();
        usleep(50000);
        cout << "\033[2J\033[1;1H";
        if (spawn && t.check_board()) {
            t.freeze();
            t.update_tetris();
            t.generate();
            t.reset_move_var(); //reset the action variables (left, right, rotate)
            placements++;
        }
        if (t.y == DECISION_THRESHOLD)
            t.choose_moves(curr_org); //we are using a pre-trained model TODO change this
        t.do_move();
        t.printb(t.board);
        cout << "placements: " << placements << endl;
    }
    t.free_2darr(t.board);
    return 0;
}

