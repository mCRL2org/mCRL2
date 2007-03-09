ifneq ($(filter all bjam install tools test man web, ${MAKECMDGOALS}),)
include build/Makefile
endif
ifeq (${MAKECMDGOALS},)
include build/Makefile
endif

.PHONY: clean distclean parsers

build/Makefile:
	$(error Please run configure first)

clean:
	@$(RM) -r autom4te.cache core core.*
	$(RM) -rf build/bin/*
	$(RM) -rf build/man
	$(RM) -rf build/web

distclean: clean
	$(RM) -r config.log config.status build/config.jam src/setup.h
	$(RM) -rf build/bin

parsers:
	cd src/libraries/lts/source; \
	flex -Pfsm -ofsmlexer.cpp fsmlexer.ll; \
	bison -p fsm -d -o fsmparser.cpp fsmparser.yy; \
	mv fsmparser.hpp ../include
	cd src/ltsview; \
	flex -Pfsm -ofsmlexer.cpp fsmlexer.ll; \
	bison -p fsm -d -o fsmparser.cpp fsmparser.yy
	cd src/libraries/parser/source; \
	flex -Pmcrl2 -omcrl2lexer.cpp mcrl2lexer.ll; \
	bison -p mcrl2 -d -o mcrl2parser.cpp mcrl2parser.yy; \
	mv mcrl2parser.hpp ../include
	cp /usr/include/FlexLexer.h build/workarounds

configure: build/autoconf/configure.ac
	autoconf -o $@ -W all $<
