# This is the default, that does not work for MacOSX (../config overrides it)
BOOST_LDFLAGS = -Wl,-rpath=../src/boost/lib

-include ../config

all: maximum_revision

maximum_revision: maximum_revision.cpp
	$(MAKE) -C ../src/boost boost_filesystem
	$(CXX) $(BOOST_LDFLAGS) -I../src/boost $< -L../src/boost/lib -lboost_filesystem -o $@

.PHONY: all

