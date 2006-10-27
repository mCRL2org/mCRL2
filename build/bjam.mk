# Makefile for bjam bootstrap (include and use only from the top Makefile)
#
# Nobody is supposed to change anything to any part of the boost sources
#
# The set of files in this directory was compiled by running:
#
#   bcp --scan $(find ./ ../boost/tools/regression ../boost/boost/asio -name '*.cpp' -or -name '*.h' -or -name '*.hpp') \
#       --boost=<path-to-boost-sources> /tmp/boost_subset
#
# from the trunk/src directory.
#
# Then create boost/include and move the boost/boost directory to /boost/include/boost.

BJAM_BUILD    = ./build.sh

# Build boost build tool
ifeq ($(HOST_OS),WINDOWS)
 BJAM_BUILD   = build.bat runtime-link=static link=static
endif

bjam: $(BJAM)

$(BJAM):
	@mkdir -p bin
	@cd boost/tools/jam; $(BJAM_BUILD)
	@rm -rf boost/tools/jam/bootstrap

