#Redefine default definitions using autoconf
CC              = gcc
CFLAGS          = -g -O2 -std=c99 -Wall -pedantic
WX_CONFIG 	= wx-config
CPP             = gcc -E
CPPFLAGS        = -I.. -I../aterm -I../svc `$(WX_CONFIG) --cxxflags`
CXX             = g++
CXXFLAGS        = -g -Wall
LDFLAGS         = -Wl,--as-needed -L. $(shell $(WX_CONFIG) --libs)

#mCRL2 studio definitions
STUDIO_NAME     = mcrl2studio
STUDIO_OBJECTS  = mcrl2studio.o proj.o anal.o project.o general.o object.o tagdialog.o analysis.o types.o simtool.o lintool.o common.o ../xsimbase.o ../xsimmain.o ../xsimtrace.o ../libgsnextstate.o ../libgsprover.o ../gsparser.o ../gslexer.o ../gstypecheck.o ../gsdataimpl.o ../lin_alt.o ../gslowlevel.o ../gsfunc.o ../libprint_cxx.o
STUDIO_LDFLAGS  = -L.. -L../aterm -lATerm -lm -lgsrewrite

#Make rules
.PHONY: all clean

all : $(STUDIO_NAME)

$(STUDIO_NAME) : $(STUDIO_OBJECTS) $(STUDIOBASE_NAME)
	$(CXX) $(CXX_FLAGS) -o $@ $(STUDIO_OBJECTS) $(LDFLAGS) $(STUDIO_LDFLAGS)

%.d : %.c
	@echo generating dependencies for $<
	@$(CC) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	$(RM) $@.$$$$

%.dpp : %.cpp
	@echo generating dependencies for $<
	@$(CXX) -MM $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	$(RM) $@.$$$$

ifeq (,$(findstring $(MAKECMDGOALS),clean))
-include $(foreach s,$(STUDIO_OBJECTS),$(shell if test -f $(basename $s).c; then echo -n $(basename $s).d; fi))
-include $(foreach s,$(STUDIO_OBJECTS),$(shell if test -f $(basename $s).cpp; then echo -n $(basename $s).dpp; fi))
#-include $(patsubst %.c,%.d,$(wildcard *.c))
#-include $(patsubst %.cpp,%.dpp,$(wildcard *.cpp))
endif

clean:
	rm -f mcrl2studio *.o *.d *.dpp
