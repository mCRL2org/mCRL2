all: lpeconstelm

lpeconstelm.o: lpeconstelm.cpp Makefile
	g++ -c -ftemplate-depth-256 -g -O0 -Wall -I/home/fstapper/mcrl/trunk/src -I/home/fstapper/mcrl/trunk/src/aterm -o lpeconstelm.o  lpeconstelm.cpp

lpeconstelm: lpeconstelm.o
	g++ -o lpeconstelm lpeconstelm.o -L/home/fstapper/mcrl/trunk/src/aterm/ -lboost_program_options -lATerm
