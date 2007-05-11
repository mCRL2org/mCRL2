ifneq ($(filter all bjam install tools test man web, ${MAKECMDGOALS}),)
include build/Makefile
endif
ifeq (${MAKECMDGOALS},)
include build/Makefile
endif

.PHONY: tags clean distclean parsers mcrl2parser ltsview-fsmparser liblts-fsmparser

build/Makefile:
	$(error Please run configure first)

clean:
	@$(RM) -r autom4te.cache core core.* tags
	$(RM) -rf build/bin/*
	$(RM) -rf build/man
	$(RM) -rf build/web

distclean: clean
	@$(RM) -r build/Makefile config.log config.status build/config.jam src/setup.h
	$(RM) -rf build/bin

parsers: mcrl2parser ltsview-fsmparser liblts-fsmparser

liblts-fsmparser:
	cd src/libraries/lts/source; \
	flex -Pfsm -ofsmlexer.cpp fsmlexer.ll; \
	bison -p fsm -d -o fsmparser.cpp fsmparser.yy; \
	mv fsmparser.hpp ../include

ltsview-fsmparser:
	cd src/ltsview; \
	flex -Pfsm -ofsmlexer.cpp fsmlexer.ll; \
	bison -p fsm -d -o fsmparser.cpp fsmparser.yy

mcrl2parser:
	cd src/libraries/parser/source; \
	flex -Pmcrl2 -omcrl2lexer.cpp mcrl2lexer.ll; \
	bison -p mcrl2 -d -o mcrl2parser.cpp mcrl2parser.yy; \
	mv mcrl2parser.hpp ../include
	cp /usr/include/FlexLexer.h build/workarounds/all

configure: build/autoconf/configure.ac
	autoconf -o $@ -W all $<

tags:
	ctags --languages=C,C++ --recurse=yes --extra=+q --fields=+i --totals=yes .
