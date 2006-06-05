-include config

# Creates an application bundle on Mac OS X
%.app: src/%
	/Developer/Tools/Rez -t APPL -o $^ sample.r
	/Developer/Tools/SetFile -a C $^
	install -d $@/Contents/MacOS
	install Info.plist $@/Contents/Info.plist
	install -d $@/Contents/Resources
	install xsim.icns $@/Contents/Resources/xsim.icns
	ln -f $(bindir)/$^ $@/Contents/MacOS/xsim

.PHONY: all install clean distclean distribution

all: config $(BJAM)
	@$(BOOST_BUILD)

install: $(BJAM)
	@$(BOOST_BUILD) --install
	@$(MAKE) -C src/doc install

ifeq ($(HOST_OS),MACOSX)
install: xsim.app ltsgraph.app ltsview.app squadt.app
endif

clean:
	@$(MAKE) -C src/doc clean
	@$(RM) -rf bin
	@$(RM) -r autom4te.cache config.log *.o *~ core core.*

distclean:
	@${MAKE} -C src distclean
	@$(MAKE) -C src/doc distclean
	@$(RM) -r autom4te.cache config.log *.o *.app *~ core core.* \
	         bin config.status config config.jam src/setup.h \
		 src/mcrl2_revision.h boost/tools/jam/bin boost/tools/jam/bootstrap

$(BJAM):
	@$(MAKE) -C boost bjam

revision: $(BJAM)
	@$(BOOST_BUILD) mcrl2_revision

ifeq (,$(findstring $(MAKECMDGOALS),clean distclean revision))

config:
	$(error Please configure the source tree first.)
endif
