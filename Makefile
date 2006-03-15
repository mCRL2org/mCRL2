.PHONY: all install clean distclean distribution

-include config

all: config
	@${MAKE} -C src $(MAKECMDGOALS)

include utility/revision.mk

install: all

clean:
	@${MAKE} -C src $(MAKECMDGOALS)
	$(RM) -r autom4te.cache config.log *.o *~ core core.*

distclean: clean distribution
	$(RM) -r bin config.status config config.jam src/setup.h src/mcrl2_revision.h

ifeq (,$(findstring $(MAKECMDGOALS),clean distclean revision))

config:
	$(error Please configure the source tree first.)

endif
