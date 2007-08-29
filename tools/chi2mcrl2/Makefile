main: main.o lexer.o parser.o
	gcc -o main -Wall main.c lexer.o parser.c -lm

parser.o: parser.c

parser.c: parser.y
	bison -d -o parser.c parser.y
#	test -e parser.tab.c && mv parser.tab.c parser.c
#	test -e parser.tab.h && mv parser.tab.h parser.h
parser.h: parser.c

lexer.o: lexer.c

lexer.c: lexer.l parser.h
	flex -olexer.c lexer.l
#	test -e lex.yy.c && mv lex.yy.c lexer.c

clean:
	rm -f main *.o chiparser.hpp chiparser.cpp chilexer.cpp chilexer.hpp chiparser.output 

test: main
	./main

