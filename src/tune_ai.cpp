#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../include/genetic.hpp"
#include "../include/tetris_engine.hpp"
#include "../include/train.hpp"
using namespace std;
//our current champion genes: 0.520401 0.066064 0.358904 0.217230 -0.010198 -0.077927 -0.196434
const double incre1 = 0.2; //we can play around with different tuning increments
const double incre2 = 0.1;
const double incre3 = 0.05;
const double incre4 = 0.01;

void tuning_test(organism *test) {
	lines_completed = 0;
	for (int i = 0; i < GAMES_PER_ORG; i++)
		place_pieces(*test);
	test->fitness = lines_completed; //adjust fitness
}

int main()
{
	organism set;
	freopen("parameters", "r", stdin);
	std::cin >> set.a >> set.b >> set.c >> set.d
		>> set.e >> set.f >> set.g;
	//from 0 to 1111111, we can get all combinations using binary, [0,127] in decimal
	/*
	for (int i = 0; i < 64; i++) {
		cout << "config: ";
		cout << (int) (((i >> 5) & 1) ? 1 : 0);
		cout << (int) (((i >> 4) & 1) ? 1 : 0);
		cout << (int) (((i >> 3) & 1) ? 1 : 0);
		cout << (int) (((i >> 2) & 1) ? 1 : 0);
		cout << (int) (((i >> 1) & 1) ? 1 : 0);
		cout << (int) (((i >> 0) & 1) ? 1 : 0);
		cout << endl;
		organism test = set;
		double genes[6]; //we have 7 genes in total but a 6 pont sphere is being optimized
		genes[0] = test.a;
		genes[1] = test.b;
		genes[2] = test.c;
		genes[3] = test.d;
		genes[4] = test.e;
		genes[5] = test.f;

		for (int whbt = 0; whbt < 6; whbt++)
			genes[whbt] = ((i >> whbt) & 1) ? genes[whbt]+incre2 : genes[whbt];
		test.a = genes[0];
		test.b = genes[1];
		test.c = genes[2];
		test.d = genes[3];
		test.e = genes[4];
		test.f = genes[5];
		tuning_test(&test);
		cout << "fitness: " << test.fitness << endl;
		if (test.fitness > set.fitness)
			set = test;
		cout << endl;
	}

	std::cout << "genes: "    <<
			set.a << " " <<
			set.b << " " <<
			set.c << " " <<
			set.d << " " <<
			set.e << " " <<
			set.f << " " <<
			set.g << std::endl;
	std::cout << "fitns: " << set.fitness << std::endl;*/

	//let alpha represent positive adjustment, and beta negative
	for (int alpha = 0; alpha < 8; alpha++) {
		for (int beta = 0; beta < 8; beta++) {
			organism test = set;
			test.fitness = 0;
			double genes[3];
			genes[0] = test.a;
			genes[1] = test.b;
			genes[2] = test.c;
			for (int whbt = 2; whbt >= 0; whbt--) {
				if (((alpha >> whbt) & 1) && !((beta >> whbt) & 1))
					genes[whbt] += incre2;
				else if (!((alpha >> whbt) & 1) && ((beta >> whbt) & 1))
					genes[whbt] -= incre2;
			}
			test.a = genes[0];
			test.b = genes[1];
			test.c = genes[2];
			cout << "config: ";
			printf("%f %f %f\n", genes[0], genes[1], genes[2]);
			tuning_test(&test);
			if (test.fitness > set.fitness)
				set = test;
			cout << "fitness: " << test.fitness << endl;
		}
	}
	return 0;
}

