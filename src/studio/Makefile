#Redefine default definitions using autoconf
CC              = gcc
CFLAGS          = -g -O2 -std=c99 -Wall -pedantic -I..
LDFLAGS         = -Wl,--as-needed -L. $(shell $(WX_CONFIG) --libs)
WX_CONFIG 	= wx-config
CXX		= $(shell $(WX_CONFIG) --cxx)
CXXFLAGS       =  -g -I.. `$(WX_CONFIG) --cxxflags`

#GenSpect GUI definitions
STUDIO_NAME     = mcrl2studio
STUDIO_OBJECTS  = mcrl2studio.o proj.o anal.o project.o general.o object.o tagdialog.o analysis.o types.o ../xsimbase.o ../xsimmain.o ../xsimtrace.o ../libgsnextstate.o ../libgsprover.o simtool.o ../gslinearise2.o lintool.o common.o ../libprint.o
STUDIO_LDFLAGS  = -L.. -lgsparse -lATerm -lm -lgsrewrite

#Make rules
.PHONY: all

all : $(STUDIO_NAME)

$(STUDIO_NAME) : $(STUDIO_OBJECTS) ../libgsparse.a $(STUDIOBASE_NAME)
	$(CXX) $(CXX_FLAGS) -o $@ $(STUDIO_OBJECTS) $(LDFLAGS) $(STUDIO_LDFLAGS)

%.dpp : %.cpp
	@echo generating dependencies for $<
	@$(CXX) -MM $(CXXFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	$(RM) $@.$$$$

-include $(patsubst %.c,%.d,$(wildcard *.c))
-include $(patsubst %.cpp,%.dpp,$(wildcard *.cpp))

clean:
	rm -f *.o mcrl2studio
