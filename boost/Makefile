# Seperate Makefile for recursive make.
#
# Nobody is supposed to change anything to any part of the boost sources
#
# The set of files in this directory was compiled by running:
#
#   bcp --scan $(find ./ ../boost/boost/asio -name '*.cpp' -or -name '*.h' -or -name '*.hpp') \
#      --boost=<path-to-boost-sources> /tmp/boost_subset
#
# from the trunk/src directory.
#
# Then create boost/include and move the boost/boost directory to /boost/include/boost.

TREE_ROOT = ../

-include $(TREE_ROOT)config

BJAM           = tools/jam/bin/bjam
BJAM_BUILD     = ./build.sh
BJAM_ARGUMENTS = variant=release threading=single optimization=speed $(LINK_METHOD) \
		 --prefix=. --layout=system --stagedir=lib stage
LINK_METHOD    = runtime-link=shared

# Build boost build tool
ifeq ($(HOST_OS),WINDOWS)
 BJAM         = tools/jam/bin/bjam.exe
 BJAM_BUILD   = build.bat
endif
ifeq ($(HOST_OS),CYGWIN)
 LINK_METHOD  = runtime-link=static link=static
endif
ifeq ($(HOST_OS),MINGW)
 BJAM         = tools/jam/bin/bjam.exe
 BJAM_BUILD   = ./build.sh mingw
 LINK_METHOD  = runtime-link=static link=static
endif

.PHONY: all bjam install clean distclean

all: build_completed

build_completed: $(BJAM)
	touch build_completed

bjam: $(BJAM)

boost_filesystem: $(BJAM)
	$(BJAM) $(BJAM_ARGUMENTS) --with-filesystem

boost_program_options: $(BJAM)
	$(BJAM) $(BJAM_ARGUMENTS) --with-program_options

$(BJAM):
	@cd tools/jam; $(BJAM_BUILD)

install: all

clean:

distclean:
	@-$(RM) -rf bin bin.v2 lib build_completed tools/jam/bin tools/jam/bootstrap

