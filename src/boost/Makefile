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

TREE_ROOT = ../../

-include $(TREE_ROOT)config

BJAM           = tools/build/jam_src/bin/bjam
BJAM_BUILD     = ./build.sh
BJAM_ARGUMENTS = "-sBUILD=release <threading>single <optimization>speed <runtime-link>dynamic"

# Build boost build tool
ifeq ($(HOST_OS),WINDOWS)
 BJAM         = tools/build/jam_src/bin/bjam.exe
 BJAM_BUILD   = build.bat
endif
ifeq ($(HOST_OS),CYGWIN)
 BJAM_ARGUMENTS = "-sBUILD=release <threading>single <optimization>speed <runtime-link>static"
endif
ifeq ($(HOST_OS),MINGW)
 BJAM           = tools/build/jam_src/bin/bjam.exe
 BJAM_BUILD     = start //WAIT mbuild.bat mingw;
 BJAM_ARGUMENTS = "-sTOOLS=mingw" "-sBUILD=release <threading>single <optimization>speed <runtime-link>static"
endif

.PHONY: all bjam install clean distclean

all: bjam build_completed

build_completed:
	$(BJAM) $(BJAM_ARGUMENTS) --prefix=. --layout=system install
	touch build_completed

bjam: $(BJAM)

boost_filesystem: bjam
	$(BJAM) $(BJAM_ARGUMENTS) --prefix=. --layout=system --with-filesystem install

$(BJAM):
	cd tools/build/jam_src; \
	$(BJAM_BUILD)

install: all
	$(INSTALL) -d $(libdir)
	$(INSTALL) lib/* $(libdir)

clean:

distclean:
	@-$(RM) -rf bin lib stage build_completed tools/build/jam_src/bin tools/build/jam_src/bootstrap

