.PHONY: all install clean distclean distribution

-include config

all: config $(BJAM)
	@$(BOOST_BUILD)
	@${MAKE} -C src $(MAKECMDGOALS)

-include utility/revision.mk

install: all
	@$(BOOST_BUILD) --install
	install -d $(datadir)/examples/academic
	cp examples/academic/*.mcrl2 $(datadir)/examples/academic
	install -d $(datadir)/examples/industrial
	cp examples/industrial/*.mcrl2 $(datadir)/examples/industrial
	install -d $(datadir)/examples/industrial/garage
	cp examples/industrial/garage/*.mcrl2 $(datadir)/examples/industrial/garage
	install -d $(datadir)/examples/generated
	cp examples/generated/*.mcrl2 $(datadir)/examples/generated
	cp examples/generated/*.pnml $(datadir)/examples/generated
	install -d $(datadir)/examples/visualisation
	cp examples/visualisation/*.fsm $(datadir)/examples/visualisation

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

ifeq (,$(findstring $(MAKECMDGOALS),clean distclean revision))

config:
	$(error Please configure the source tree first.)
endif
