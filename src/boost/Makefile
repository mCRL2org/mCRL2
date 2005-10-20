# Seperate Makefile for recursive make.
#
# Nobody is supposed to change anything to any part of the boost sources
#
# The set of files in this directory was compiled by running:
#
#   bcp --scan atermpp/*.h lpe/*.cpp --boost=<path-to-boost-source> ./boost
#
# from the trunk/src directory.
#
# Then create boost/include and move the boost/boost directory to /boost/include/boost.

-include ../config

# Build boost build tool
ifeq ($(HOST_OS),WINDOWS)
 bjam_builder=build.bat
else
 # Assume a platform on which a decent shell is available
 bjam_builder=./build.sh
endif

.PHONY: all bjam clean

ifeq ($(HOST_OS),MINGW)

all: bjam
	tools/build/jam_src/bin/bjam.exe -sTOOLS=mingw --prefix=. install --layout=system

bjam:
	echo "Please manually close command window, when build has completed!"
	cd tools/build/jam_src; \
	start //WAIT mbuild.bat mingw;
else

all: bjam
	tools/build/jam_src/bin/bjam --prefix=. install --layout=system
bjam:
	cd tools/build/jam_src; \
	$(bjam_builder)
endif

clean:
	@-$(RM) -rf bin lib tools/build/jam_src/bin tools/build/jam_src/bootstrap

