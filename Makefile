.PHONY: all install clean distclean

-include config

all: config
	@${MAKE} -C src $(MAKECMDGOALS)

include utility/revision.mk

install: all

clean:
	@${MAKE} -C src $(MAKECMDGOALS)
	$(RM) -r autom4te.cache config.log *.o *~ core core.*

distclean: clean distribution
	$(RM) -r config.status config config.jam revision utility/bin utility/maximum_revision

# Uses the fact that distribution only exists in an official source distribution
distribution:
	@$(RM) src/revision src/squadt/revision src/ltsview/revision

ifeq (,$(findstring $(MAKECMDGOALS),clean distclean revision))

config:
	$(error Please configure the source tree first.)

endif
