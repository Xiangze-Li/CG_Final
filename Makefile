all: main.cpp  *.hpp
	g++ -std=c++11 main.cpp -o bin/main -O2 -fopenmp

clean:
	rm bin/*
