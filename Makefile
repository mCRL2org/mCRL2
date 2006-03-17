.PHONY: all install clean distclean distribution

-include config

all: config
	@${MAKE} -C src $(MAKECMDGOALS)

include utility/revision.mk

install: all
	install -d $(datadir)/academic
	cp examples/academic/*.mcrl2 $(datadir)/academic
	install -d $(datadir)/industrial
	cp examples/industrial/*.mcrl2 $(datadir)/industrial
	install -d $(datadir)/industrial/garage
	cp examples/industrial/garage/*.mcrl2 $(datadir)/industrial/garage
	install -d $(datadir)/generated
	cp examples/generated/*.mcrl2 $(datadir)/generated
	cp examples/generated/*.pnml $(datadir)/generated
	install -d $(datadir)/visualisation
	cp examples/visualisation/*.fsm $(datadir)/visualisation

clean:
	@${MAKE} -C src $(MAKECMDGOALS)
	$(RM) -r autom4te.cache config.log *.o *~ core core.*

distclean: clean distribution
	$(RM) -r bin config.status config config.jam src/setup.h src/mcrl2_revision.h

ifeq (,$(findstring $(MAKECMDGOALS),clean distclean revision))

config:
	$(error Please configure the source tree first.)

endif
