BJAM = ../src/boost/tools/build/jam_src/bin/bjam

all: $(BJAM) maximum_revision

maximum_revision: maximum_revision.cpp
	$(MAKE) -C ../src/boost boost_filesystem
	$(CXX) -I../src/boost/boost $< -L../src/boost/lib -lboost_filesystem -o $@

.PHONY: all bjam

