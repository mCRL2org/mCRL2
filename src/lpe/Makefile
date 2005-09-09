all: lpeinfo

lpeinfo.o: lpeinfo.cpp Makefile
	g++ -c -ftemplate-depth-256 -g -O0 -Wall -I/home/fstapper/mcrl/trunk/src -I/home/fstapper/mcrl/trunk/src/aterm -o lpeinfo.o  lpeinfo.cpp

lpeinfo: lpeinfo.o
	g++ -o lpeinfo lpeinfo.o -L/home/fstapper/mcrl/trunk/src/aterm/ -lboost_program_options -lATerm
