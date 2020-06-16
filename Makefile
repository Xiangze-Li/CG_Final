all:
	g++ -std=c++11 main.cpp -o bin/main -O2 -fopenmp

test:
	clear
	g++ -std=c++11 main.cpp -o bin/main -O2 -fopenmp
	sleep 2s
	clear
	time ./bin/main

clean:
	rm bin/*
