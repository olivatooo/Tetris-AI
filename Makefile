Default:

train:
	g++ src/train.cpp src/tetris_engine.cpp src/genetic.cpp -o bin/train

run:
	g++ src/run.cpp src/tetris_engine.cpp src/genetic.cpp -o bin/run

tune:
	g++ src/tune_ai.cpp src/genetic.cpp src/tetris_engine.cpp src/train.cpp -o bin/tune
