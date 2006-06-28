-include build/config.mk

# Creates an application bundle on Mac OS X
.PHONY: all install clean distclean reposclean distribution

all: $(BJAM)
	@$(BOOST_BUILD)

install: $(BJAM)
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
	$(RM) -rf bin boost/tools/jam/bin boost/tools/jam/bootstrap

reposclean: distclean
	$(RM) src/mcrl2_revision.h

$(BJAM):
	@$(MAKE) -C boost bjam

revision: $(BJAM)
	@$(BOOST_BUILD) mcrl2_revision

ifeq (,$(BJAM) $(findstring $(MAKECMDGOALS),clean distclean revision))
	$(error Please configure the source tree first.)
endif

