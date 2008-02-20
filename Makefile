ifneq ($(filter all bjam install tools test, ${MAKECMDGOALS}),)
include build/Makefile
endif
ifeq (${MAKECMDGOALS},)
include build/Makefile
endif

.PHONY: tags clean distclean parsers mcrl2parser ltsview_fsmparser liblts_fsmparser doxy

build/Makefile:
	$(error Please run configure first)

clean:
	@$(RM) -r autom4te.cache core core.* tags build/bin/.jamdeps
	$(RM) -rf build/bin/* build/stage

distclean: clean
	@$(RM) -rf build/Makefile config.log config.status build/config.jam libraries/utilities/include/mcrl2/setup.h
	$(RM) -rf build/bin

parsers: mcrl2parser chiparser ltsview_fsmparser liblts_fsmparser
	cp /usr/include/FlexLexer.h build/workarounds/all

liblts_fsmparser:
	cd libraries/lts/source; \
	flex -Pfsm -oliblts_fsmlexer.cpp liblts_fsmlexer.ll; \
	bison -p fsm -d -o liblts_fsmparser.cpp liblts_fsmparser.yy; \
	mv liblts_fsmparser.hpp ../include/mcrl2

ltsview_fsmparser:
	cd tools/ltsview; \
	flex -+ -oltsview_fsmlexer.cpp ltsview_fsmlexer.ll; \
	bison -d -o ltsview_fsmparser.cpp ltsview_fsmparser.yy

mcrl2parser:
	cd libraries/core/source; \
	flex -Pmcrl2 -omcrl2lexer.cpp mcrl2lexer.ll; \
	bison -p mcrl2 -d -o mcrl2parser.cpp mcrl2parser.yy; \
	mv mcrl2parser.hpp ../include/mcrl2/core/detail

chiparser:
	cd tools/chi2mcrl2; \
	flex -Pchi -ochilexer.cpp chilexer.ll; \
	bison -p chi -d -o chiparser.cpp chiparser.yy;

configure: build/autoconf/configure.ac
	autoconf -o $@ -W all $<

tags:
	ctags --languages=C,C++ --recurse=yes --extra=+q --fields=+i --totals=yes .

doxy:
	@doc/doxy/generate_libref_website.sh
