.PHONY: all install clean distclean distribution

-include config

all: config $(BJAM)
	@$(BOOST_BUILD)

install: $(BJAM)
	@$(BOOST_BUILD) --install
	@$(MAKE) -C src/doc install

clean:
	@${MAKE} -C src clean
	@$(MAKE) -C src/doc clean
	@$(RM) -rf bin
	@$(RM) -r autom4te.cache config.log *.o *~ core core.*

distclean:
	@${MAKE} -C src distclean
	@$(MAKE) -C src/doc distclean
	@$(RM) -r autom4te.cache config.log *.o *.app *~ core core.* \
	         bin config.status config config.jam src/setup.h \
		 src/mcrl2_revision.h

$(BJAM):
	@$(MAKE) -C boost bjam

revision: $(BJAM)
	@$(BOOST_BUILD) mcrl2_revision

ifeq (,$(findstring $(MAKECMDGOALS),clean distclean revision))

config:
	$(error Please configure the source tree first.)
endif
