CONFIG = build/config.mk

-include $(CONFIG)

# Creates an application bundle on Mac OS X
.PHONY: all bjam install clean distclean distribution

all: $(CONFIG) $(BJAM) tools man

install: $(CONFIG) $(BJAM)
	$(BOOST_BUILD) --install
	@install -d $(mandir)
	@cp -rf ../../build/man/* $(mandir)

tools:
	$(BOOST_BUILD)

clean:
	$(RM) -r autom4te.cache config.log core core.*
	$(RM) -rf build/bin/*
	$(RM) -rf build/man
	$(RM) -rf build/web

test: $(BJAM)
	$(BJAM) ./status --preserve-test-targets

distclean: clean
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

$(MAN_MANIFEST): src/doc/*.xml
	@cd ./src/doc; $(XSLTPROC) --stringparam man.output.subdirs.enabled 1 \
	             --stringparam man.output.manifest.enabled 1 \
	             --stringparam man.output.manifest.filename $(MAN_MANIFEST) \
	             --stringparam man.output.in.separate.dir 1 \
	             --stringparam man.output.base.dir ../../build/man/ \
		     http://docbook.sourceforge.net/release/xsl/current/manpages/docbook.xsl index.xml
man: $(MAN_MANIFEST)

web: 
	@cd ./src/doc; $(XSLTPROC) --stringparam base.dir ../../build/web/ \
	             --stringparam html.stylesheet man.css \
		     http://docbook.sourceforge.net/release/xsl/current/html/chunk.xsl index.xml; \
	 install -m 0644 man.css ../../build/web/

configure: build/autoconf/configure.ac
	autoconf -o $@ -W all $<

include build/make/bjam.mk

$(CONFIG):
	$(error Please run configure first)
