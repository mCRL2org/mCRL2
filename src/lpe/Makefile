all: lpeconstelm

lpeconstelm.o: lpeconstelm.cpp Makefile
	g++ -c -ftemplate-depth-256 -g -O0 -Wall -I/home/fstapper/mcrl/trunk/src -I/home/fstapper/mcrl/trunk/src/aterm -o lpeconstelm.o  lpeconstelm.cpp

lpeconstelm: lpeconstelm.o
	g++ -o lpeconstelm lpeconstelm.o /home/fstapper/mcrl/trunk/src/gsfunc.o -L/home/fstapper/mcrl/trunk/src /home/fstapper/mcrl/trunk/src/libgsrewrite.o /home/fstapper/mcrl/trunk/src/gsrewr_inner.o /home/fstapper/mcrl/trunk/src/gsrewr_inner2.o /home/fstapper/mcrl/trunk/src/gsrewr_inner3.o /home/fstapper/mcrl/trunk/src/gsrewr_innerc.o -L/home/fstapper/mcrl/trunk/src/aterm -lboost_program_options -lATerm -lm /home/fstapper/mcrl/trunk/src/gsrewr_jitty.o /home/fstapper/mcrl/trunk/src/gssubstitute.o -ldl /home/fstapper/mcrl/trunk/src/gslowlevel.o -lgsrewrite -lgsparse
