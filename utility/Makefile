# This is the default, that does not work for MacOSX (../config overrides it)
BOOST_LDFLAGS  = -L../src/boost/lib -Wl,-rpath=../src/boost/lib
BOOST_CPPFLAGS = -I../src/boost 

-include ../config

all: maximum_revision

maximum_revision: maximum_revision.cpp boost_filesystem
	$(CXX) $(BOOST_CPPFLAGS) $(BOOST_LDFLAGS) $< -lboost_filesystem -o $@

ifndef HOST_OS
boost_filesystem:
	$(MAKE) -C ../src/boost boost_filesystem
endif
ifdef BUILD_BOOST
boost_filesystem:
	$(MAKE) -C ../src/boost boost_filesystem
endif

.PHONY: all boost_filesystem

