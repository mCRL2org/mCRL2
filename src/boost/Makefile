# Seperate Makefile for recursive make.
#
# Nobody is supposed to change anything to any part of the boost sources
#
# The set of files in this directory was compiled by running:
#
#   bcp --scan $(find ./ -name '*.cpp' -or -name '*.h' -and -not -name 'boost' | grep --invert-match -e "boost") \
#      --boost=<path-to-boost-source> ./boost
#
# from the trunk/src directory.
#
# Then create boost/include and move the boost/boost directory to /boost/include/boost.

-include ../config

BJAM          = tools/build/jam_src/bin/bjam
BJAM_BUILD    = ./build.sh
BJAM_TOOLS    =

# Build boost build tool
ifeq ($(HOST_OS),WINDOWS)
 BJAM         = tools/build/jam_src/bin/bjam.exe
 BJAM_BUILD   = build.bat
endif

ifeq ($(HOST_OS),MINGW)
 BJAM         = tools/build/jam_src/bin/bjam.exe
 BJAM_BUILD   = start //WAIT mbuild.bat mingw;
 BJAM_TOOLS   = -sTOOLS=mingw
endif

.PHONY: all bjam install clean distclean

all: $(BJAM)
	$(BJAM) $(BJAM_TOOLS) --prefix=. install --layout=system

$(BJAM):
	cd tools/build/jam_src; \
	$(BJAM_BUILD)

install: all
	$(INSTALL) -d $(libdir)
	$(INSTALL) lib/* $(libdir)

clean:

distclean:
	@-$(RM) -rf bin lib tools/build/jam_src/bin tools/build/jam_src/bootstrap
