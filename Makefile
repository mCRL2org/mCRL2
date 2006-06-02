.PHONY: all install clean distclean distribution

-include config

all: config $(BJAM)
	@$(BOOST_BUILD)

install: $(BJAM)
	@$(MAKE) -C src/doc install
	@$(BOOST_BUILD) --install

clean:
	@${MAKE} -C src clean
	@$(RM) -rf bin
	@$(RM) -r autom4te.cache config.log *.o *~ core core.*

distclean:
	@${MAKE} -C src distclean
	@${MAKE} -C boost distclean
	@$(RM) -r autom4te.cache config.log *.o *.app *~ core core.* \
	         bin config.status config config.jam src/setup.h \
		 src/mcrl2_revision.h

$(BJAM):
	$(MAKE) -C boost bjam

ifeq (,$(findstring $(MAKECMDGOALS),clean distclean revision))

config:
	$(error Please configure the source tree first.)
endif
