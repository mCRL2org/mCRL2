-include build/config.mk

BJAM   = bin/bjam
CONFIG = build/config.mk

# Creates an application bundle on Mac OS X
.PHONY: all bjam install clean distclean distribution

all: $(CONFIG) $(BJAM)
	@$(BOOST_BUILD)
	@$(MAKE) -C src/doc

install:  $(CONFIG) $(BJAM)
	@$(BOOST_BUILD) --install
	@$(MAKE) -C src/doc install

clean:
	@$(MAKE) -C src/doc clean
	$(RM) -rf bin/*
	$(RM) -r autom4te.cache config.log *.o *~ core core.*

distclean:
	@${MAKE} -C src/doc distclean
	$(RM) -r autom4te.cache *.o *.app *~ core core.*
	$(RM) -r config.log config.status build/config.mk build/config.jam src/setup.h
	$(RM) -rf bin

revision: $(BJAM)
	@$(BOOST_BUILD) mcrl2_revision

include build/bjam.mk

build/config.mk:
	$(error Please run configure first.)
