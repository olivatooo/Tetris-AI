#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "tetris_engine.hpp"
#include "genetic.hpp"

int main()
{
	init_population();
	print_sample_population();

	free(population);
	return 0;
}

