# Fetch some configuration settings obtained by the configure script
include ../config

ifeq ($(USE_BOOST),1)
 ifneq ($(BOOST_PREFIX),)
  BOOST_HEADERS   = -I$(BOOST_PREFIX)/include
  BOOST_LIBRARIES = -L$(BOOST_PREFIX)/lib
 endif

 ifeq ($(CC),gcc)
  BOOST_LIBRARIES = -lboost_program_options
 else
  BOOST_LIBRARIES = -lboost_program_options
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
