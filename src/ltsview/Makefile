-include ../../config

WX_LIBS		:= `$(WX_CONFIG) --libs core base gl`
WX_CXXFLAGS	:= `$(WX_CONFIG) --cxxflags`

ICON_OBJECTS	 = mainframe.o glcanvas.o
WX_OBJECTS	 = ltsviewapp.o mainframe.o glcanvas.o wxthings/spinctld.o markstateruledialog.o
AT_OBJECTS	 = ltsviewapp.o action.o fsmparser.o lts.o state.o
CPP_OBJECTS	 = fileloader.o visualizer.o lts.o state.o transition.o action.o \
		   cluster.o fsmparser.o utils.o glutils.o

OBJECTS		 = $(WX_OBJECTS) $(CPP_OBJECTS)
TARGETS		 = ltsview

.PHONY: all clean
.PRECIOUS: fsmparser.cpp fsmlexer.c fsmlexer.h

all : $(TARGETS)

SOURCE_ROOT = ../../

include $(SOURCE_ROOT)utility/revision.mk

ltsview : $(OBJECTS)
	$(CXX) $(GLUT_LDFLAGS) $(WX_LIBS) $(LDFLAGS) $(OBJECTS) -lATerm -o $@

$(WX_OBJECTS) : %.o: %.cpp
	$(CXX) -c $(WX_CXXFLAGS) $(ATERM_CPPFLAGS) $< -o $@

$(CPP_OBJECTS) : %.o: %.cpp
	$(CXX) -c $< $(ATERM_CPPFLAGS) -o $@

%.cpp : %.h

fsmparser.cpp : fsmparser.y fsmlexer.c
	$(BISON) --defines=fsmlexer.h -o fsmparser.cpp -p fsm $<
	$(CC) -c fsmlexer.c -o fsmlexer.o

fsmlexer.c : fsmlexer.l
	$(LEX) -o$@ -Pfsm $<

$(ICON_OBJECTS) : icons.h icons/*.xpm

$(OBJECTS) $(TARGETS) : Makefile

clean:
	-rm -f *.o $(TARGETS) fsmparser.cpp fsmlexer.c fsmlexer.h core core.*

distclean: clean

install:
	$(INSTALL) -d $(bindir)
	$(INSTALL) ltsview $(bindir)
