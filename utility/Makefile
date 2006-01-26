# This is the default, that does not work for MacOSX (../config overrides it)
BOOST_LDFLAGS  = -L../src/boost/lib -Wl,-rpath=../src/boost/lib
BOOST_CPPFLAGS = -I../src/boost 

-include ../config

all: maximum_revision

ifndef HOST_OS # ../config does not exist
boost_filesystem:
	$(MAKE) -C ../src/boost boost_filesystem
endif
ifdef BUILD_BOOST # local boost was configured
 BOOST_LDFLAGS  = -L../src/boost/lib

 ifeq ($(HOST_OS),LINUX)
  BOOST_LDFLAGS += -Wl,-rpath=../src/boost/lib
 endif

boost_filesystem:
	$(MAKE) -C ../src/boost boost_filesystem
endif

maximum_revision: maximum_revision.cpp boost_filesystem
	$(CXX) $(BOOST_CPPFLAGS) $(BOOST_LDFLAGS) $< -lboost_filesystem -o $@

.PHONY: all boost_filesystem

