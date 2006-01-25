.PHONY: all install clean distclean

all: config
	@${MAKE} -C src $(MAKECMDGOALS)

install: all

clean: all
	$(RM) -r autom4te.cache config.log *~ core core.* utility/get_revision

distclean: clean
	$(RM) -r config.status config

config:
	$(error You must configure your source tree first.)

