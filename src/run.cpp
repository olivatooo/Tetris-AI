#include <iostream>
#include <sstream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <omp.h>
#include <time.h>
#include <unistd.h>
#include "../include/tetris_engine.hpp"
#include "../include/genetic.hpp"
#define ts(s) to_string(s)
using namespace std;

int placements = 0;
int main()
{
	srand(time(NULL));
	next_type = rand() % 7 + 1;
	board = make_2darr(HEIGHT, WIDTH);
	organism curr_org = {0, 0, 0, 0, 0, 0, 0};
	freopen("parameters", "r", stdin);
	cin >> curr_org.a >> curr_org.b >> curr_org.c >> curr_org.d
		>> curr_org.e >> curr_org.f >> curr_org.g;
	cout << "running with params "
		 << curr_org.a << " "
		 << curr_org.b << " "
		 << curr_org.c << " "
		 << curr_org.d << endl;

	generate();
	while (!end_game_checker()) {
		bool spawn = update_board();
		usleep(30000);
		if (spawn && check_board()) {
			freeze();
			update_tetris();
			generate();
			reset_move_var(); //reset the action variables (left, right, rotate)
			placements++;
		}
		if (y == DECISION_THRESHOLD)
			choose_moves(curr_org); //we are using a pre-trained model TODO change this
		do_move();
		printb(board);
		cout << "placements: " << placements << endl;
	}
	free_2darr(board);
	return 0;
}

