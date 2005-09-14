all: constelim

constelim.o: constelim.cpp Makefile
	g++ -c -ftemplate-depth-256 -g -O0 -Wall -I/home/fstapper/mcrl/trunk/src -I/home/fstapper/mcrl/trunk/src/aterm -o constelim.o  constelim.cpp

constelim: constelim.o
	g++ -o constelim constelim.o -L/home/fstapper/mcrl/trunk/src/aterm/ -lboost_program_options -lATerm
