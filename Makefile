all:
	g++ -std=c++11 main.cpp -o bin/main -O2 -fopenmp

test:
	clear
	g++ -std=c++11 main.cpp -o bin/main -O2 -fopenmp
	clear
	time ./bin/main

clean:
	rm bin/*
