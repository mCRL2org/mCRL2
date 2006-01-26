# This is the default, that does not work for MacOSX (../config overrides it)
BOOST_LDFLAGS  = -L../src/boost/lib -Wl,-rpath=../src/boost/lib
BOOST_CPPFLAGS = -I../src/boost 

-include ../config

all: maximum_revision

maximum_revision: maximum_revision.cpp
	$(MAKE) -C ../src/boost boost_filesystem
	$(CXX) $(BOOST_CPPFLAGS) $(BOOST_LDFLAGS) $< -lboost_filesystem -o $@

.PHONY: all

