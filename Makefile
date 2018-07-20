Default:

train:
	g++ src/train.cpp src/tetris_engine.c src/genetic.c -o bin/train

run:
	g++ src/run.cpp src/tetris_engine.c src/genetic.c -o bin/run

tune:
	g++ src/tune_ai.cpp src/genetic.c src/tetris_engine.c src/train.cpp -o bin/tune

