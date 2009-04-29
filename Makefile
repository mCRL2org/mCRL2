.PHONY: all bjam install test tags clean distclean parsers mcrl2parser chiparser liblts_fsmparser doxy

all: bjam config.status
	$(BOOST_BUILD)

install: bjam config.status
	$(BOOST_BUILD) --install

test: bjam config.status
	$(BJAM) ./status -l300 --enable-experimental --enable-deprecated --tool-tests

clean:
	@$(RM) -r autom4te.cache core core.* tags build/bin/.jamdeps
	$(RM) -rf build/bin/* build/stage

distclean: clean
	@$(RM) -rf build/Makefile config.log config.status build/config.jam libraries/utilities/include/mcrl2/setup.h
	$(RM) -rf build/bin

ifneq ($(filter all bjam install test configure, ${MAKECMDGOALS}),)
  include build/Makefile
endif
ifeq (${MAKECMDGOALS},)
  include build/Makefile
endif

parsers: mcrl2parser chiparser liblts_fsmparser liblts_dotparser
	cp /usr/include/FlexLexer.h build/workarounds

liblts_fsmparser:
	cd libraries/lts/source; \
	flex -Pfsm -oliblts_fsmlexer.cpp liblts_fsmlexer.ll; \
	bison -p fsm --defines=../include/mcrl2/liblts_fsmparser.hpp -o liblts_fsmparser.cpp liblts_fsmparser.yy; \
	sed -i 's+#include "liblts_fsmparser.hpp"+#include "mcrl2/liblts_fsmparser.hpp"+' liblts_fsmparser.cpp

liblts_dotparser:
	cd libraries/lts/source; \
	flex -Pdot -oliblts_dotlexer.cpp liblts_dotlexer.ll; \
	bison -p dot --defines=../include/mcrl2/liblts_dotparser.hpp -o liblts_dotparser.cpp liblts_dotparser.yy; \
	sed -i 's+#include "liblts_dotparser.hpp"+#include "mcrl2/liblts_dotparser.hpp"+' liblts_dotparser.cpp

mcrl2parser:
	cd libraries/core/source; \
	flex -Pmcrl2 -omcrl2lexer.cpp mcrl2lexer.ll; \
	bison -p mcrl2 --defines=../include/mcrl2/core/detail/mcrl2parser.hpp -o mcrl2parser.cpp mcrl2parser.yy; \
	sed -i 's+#include "mcrl2parser.hpp"+#include "mcrl2/core/detail/mcrl2parser.hpp"+' mcrl2parser.cpp

chiparser:
	cd tools/chi2mcrl2; \
	flex -Pchi -ochilexer.cpp chilexer.ll; \
	bison -p chi -d -o chiparser.cpp chiparser.yy;

tags:
	ctags --languages=C,C++ --recurse=yes --extra=+q --fields=+i --totals=yes .

doxy:
	@doc/doxy/generate_libref_website.sh

build/Makefile:
	$(error Please run configure first)

