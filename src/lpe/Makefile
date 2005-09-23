# Fetch some configuration settings obtained by the configure script
include ../config

ifneq ($(BOOST_PREFIX),)
 BOOST_HEADERS   = -I$(BOOST_PREFIX)/include
 BOOST_LIBRARIES = -L$(BOOST_PREFIX)/lib
 ifeq ($(CC),gcc)
  BOOST_LIBRARIES += -lboost_program_options-gcc
 else
  BOOST_LIBRARIES += -lboost_program_options
 endif
endif

ATERM_HEADERS   = -I../aterm
ATERM_LIBRARIES = -L../aterm -lATerm

MCRL2_HEADERS   = -I../
MCRL2_LIBRARIES = -L../ ../gsfunc.o ../libgsrewrite.o ../gsrewr_inner.o ../gsrewr_inner2.o ../gsrewr_inner3.o ../gsrewr_innerc.o \
		  ../gsrewr_jitty.o ../gssubstitute.o ../libprint_c.o ../gslowlevel.o -lgsrewrite

CXXFLAGS       += -ftemplate-depth-256 -g -Wall

all: ../lpeconstelm ../lpeinfo

../lpeconstelm: lpeconstelm.cpp Makefile
	$(CXX) $(CXXFLAGS) $(ATERM_HEADERS) $(MCRL2_HEADERS) $(BOOST_HEADERS) $(BOOST_LIBRARIES) $(MCRL2_LIBRARIES) $(ATERM_LIBRARIES) -lm -ldl -o $@ $<

../lpeinfo: lpeinfo.cpp Makefile
	$(CXX) $(CXXFLAGS) $(ATERM_HEADERS) $(MCRL2_HEADERS) $(BOOST_HEADERS) $(BOOST_LIBRARIES) $(MCRL2_LIBRARIES) $(ATERM_LIBRARIES) -ldl -o $@ $<

clean:
	@rm -f ../lpeconstelm ../lpeinfo
 
#########################################################
# Old contents of Makefile
#########################################################
#lpeconstelm.o: lpeconstelm.cpp Makefile
#	g++ -c -ftemplate-depth-256 -g -O0 -Wall -I/home/fstapper/mcrl/trunk/src -I/home/fstapper/mcrl/trunk/src/aterm -o lpeconstelm.o  lpeconstelm.cpp

#lpeconstelm: lpeconstelm.o
#	g++ -o lpeconstelm lpeconstelm.o /home/fstapper/mcrl/trunk/src/gsfunc.o -L/home/fstapper/mcrl/trunk/src /home/fstapper/mcrl/trunk/src/libgsrewrite.o /home/fstapper/mcrl/trunk/src/gsrewr_inner.o /home/fstapper/mcrl/trunk/src/gsrewr_inner2.o /home/fstapper/mcrl/trunk/src/gsrewr_inner3.o /home/fstapper/mcrl/trunk/src/gsrewr_innerc.o -L/home/fstapper/mcrl/trunk/src/aterm -lboost_program_options -lATerm -lm /home/fstapper/mcrl/trunk/src/gsrewr_jitty.o /home/fstapper/mcrl/trunk/src/gssubstitute.o -ldl /home/fstapper/mcrl/trunk/src/gslowlevel.o -lgsrewrite -lgsparse
