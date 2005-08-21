all: lpeinfo

lpeinfo.o: lpeinfo.cpp Makefile
	g++ -c -ftemplate-depth-256   -Wno-long-double -no-cpp-precomp -g -O0 \
        -Wall -I/Users/jfg/mcrl2/src  -I/Users/jfg/boost_1_33_0 \
        -I/Users/jfg/mcrl2/src/aterm -o lpeinfo.o  lpeinfo.cpp

lpeinfo: lpeinfo.o
	g++ -o lpeinfo lpeinfo.o -L/usr/local/lib -lboost_program_options 
