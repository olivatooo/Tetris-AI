Default:

train:
	g++ src/train.cpp src/Tetris.cpp src/genetic.cpp -o bin/train

train-omp:
	g++ src/train.cpp src/Tetris.cpp src/genetic.cpp -o bin/train-omp -fopenmp

test:
	g++ src/Teste.cpp src/Tetris.cpp src/genetic.cpp -o bin/teste

run:
	g++ src/run.cpp src/Tetris.cpp src/genetic.cpp -o bin/run

tune:
	g++ src/tune_ai.cpp src/genetic.cpp src/tetris_engine.c src/train.cpp -o bin/tune

train-mpi:
	mpiCC src/train.cpp src/Tetris.cpp src/genetic.cpp -o bin/train-mpi

train-mpi-omp:
	mpiCC src/train.cpp src/Tetris.cpp src/genetic.cpp -o bin/train-mpi -fopenmp
