.PHONY: all install clean distclean

all: config
	@${MAKE} -C src $(MAKECMDGOALS)

revision:
	@${MAKE} -C src revision
	@${MAKE} -C src/squadt revision
	@${MAKE} -C src/ltsview revision

install: all

clean:
	@${MAKE} -C src $(MAKECMDGOALS)
	$(RM) -r autom4te.cache config.log *~ core core.*

distclean: clean
	@$(RM) src/revision src/squadt/revision src/ltsview/revision
	$(RM) -r config.status config utility/maximum_revision

config:
	$(error You must configure your source tree first.)

