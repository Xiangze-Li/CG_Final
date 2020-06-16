all:
	g++ -std=c++11 main.cpp -o bin/main -O2 -fopenmp

test:
	g++ -std=c++11 main.cpp -o bin/main -O2 -fopenmp
	time ./bin/main

clean:
	rm bin/*
