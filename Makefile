Default:
	you need to choose either train(model) or run(model)

train:
	g++ train.cpp genetic.cpp tetris_engine.cpp -o main

run:
	g++ tetris_engine.cpp genetic.cpp -o main
