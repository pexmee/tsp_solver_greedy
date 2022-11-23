greedy = src/greedy.cpp
include = headers

all:
	g++ -I=include -Ofast $(greedy) -o bin/greedy.bin

clean:
	rm a.out