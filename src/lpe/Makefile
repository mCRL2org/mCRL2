#
# This makefile now only functions for the convenience of the developers
# that are involved with the source under /trunk/src/lpe.
#
# Precondition :
#
# All targets the software in this directory depends on must have been
# made previously.
#


# Fetch some configuration settings obtained by the configure script
#
include ../config

ifeq ($(USE_BOOST),1)
 ifneq ($(BOOST_PREFIX),)
  ifeq ($(BOOST_PREFIX),boost)
   BOOST_HEADERS   = -I../$(BOOST_PREFIX)/include
   BOOST_LIBRARIES = ../boost/lib/libboost_program_options.a
  else
   BOOST_HEADERS   = -I$(BOOST_PREFIX)/include
   BOOST_LIBRARIES = -L$(BOOST_PREFIX)/lib -lboost_program_options
  endif
 endif
endif

ATERM_HEADERS   = -I../aterm
ATERM_LIBRARIES = -L../aterm -lATerm

MCRL2_HEADERS   = -I../
MCRL2_LIBRARIES = -L../ ../gsfunc.o ../libgsrewrite.o ../gsrewr_inner.o \
		  ../gsrewr_inner2.o ../gsrewr_inner3.o ../gsrewr_innerc.o \
		  ../gsrewr_jitty.o ../gssubstitute.o ../libprint_c.o \
		  ../gslowlevel.o -lgsrewrite

CXXFLAGS       += -ftemplate-depth-256 -g -Wall

all: ../lpeconstelm ../lpeinfo

../lpeconstelm: lpeconstelm.cpp Makefile
	$(CXX) $(CXXFLAGS) $(ATERM_HEADERS) $(MCRL2_HEADERS) $(BOOST_HEADERS) $(MCRL2_LIBRARIES) $(ATERM_LIBRARIES) -lm -ldl -o $@ $< $(BOOST_LIBRARIES)

../lpeinfo: lpeinfo.cpp Makefile
	$(CXX) $(CXXFLAGS) $(ATERM_HEADERS) $(MCRL2_HEADERS) $(BOOST_HEADERS) $(MCRL2_LIBRARIES) $(ATERM_LIBRARIES) -ldl -o $@ $< $(BOOST_LIBRARIES)

clean:
	@rm -f ../lpeconstelm ../lpeinfo
