Default:

run:
	g++ src/run.cpp src/Tetris.cpp src/genetic.cpp -o bin/run
	
train-mpi:
	mpiCC src/train.cpp src/Tetris.cpp src/genetic.cpp -o bin/train-mpi

train-mpi-omp:
	mpiCC src/train.cpp src/Tetris.cpp src/genetic.cpp -o bin/train-mpi -fopenmp
