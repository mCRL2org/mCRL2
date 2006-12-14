CONFIG = build/config.mk

-include $(CONFIG)

# Creates an application bundle on Mac OS X
.PHONY: all bjam install clean distclean distribution

all: $(CONFIG) $(BJAM)
	$(BOOST_BUILD)
	@$(MAKE) -C src/doc

install: $(CONFIG) $(BJAM)
	$(BOOST_BUILD) --install
	@$(MAKE) -C src/doc install

clean:
	@$(MAKE) -C src/doc clean
	$(RM) -rf build/bin/*
	$(RM) -r autom4te.cache config.log *.o *~ core core.*

test: $(BJAM)
	$(BJAM) ./status --preserve-test-targets

distclean:
	@${MAKE} -C src/doc distclean
	$(RM) -r autom4te.cache *.o *.app *~ core core.*
	$(RM) -r config.log config.status build/config.mk build/config.jam src/setup.h
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

include build/make/bjam.mk

$(CONFIG):
	$(error Please run configure first)
