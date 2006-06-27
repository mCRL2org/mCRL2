-include build/config.mk

# Creates an application bundle on Mac OS X
%.app: $(bindir)/%
	/Developer/Tools/Rez -t APPL -o $^ build/sample.r
	/Developer/Tools/SetFile -a C $^
	install -d $@/Contents/MacOS
	install src/$*/$*.plist $@/Contents/Info.plist
	install -d $@/Contents/Resources
	install src/$*/$*.icns $@/Contents/Resources/xsim.icns
	ln -f $^ $@/Contents/MacOS/$*
	cp -f src/$*/$*.icns $@/Contents/Resources/$*.icns

.PHONY: all install clean distclean reposclean distribution

all: $(BJAM)
	@$(BOOST_BUILD)

install-local: $(BJAM)
	@$(BOOST_BUILD) --install
	@$(MAKE) -C src/doc install

install: install-local install-app

ifeq ($(HOST_OS),MACOSX)
install-app: xsim.app ltsgraph.app ltsview.app squadt.app
else
install-app:
endif

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

