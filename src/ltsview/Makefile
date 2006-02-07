# Relative path to the root of the source tree
TREE_ROOT = ../../

-include $(TREE_ROOT)config

WX_LIBS		:= `$(WX_CONFIG) --libs core base gl`
WX_CXXFLAGS	:= `$(WX_CONFIG) --cxxflags`
ATERM_CPPFLAGS	+= -I$(TREE_ROOT)src/aterm

ICON_OBJECTS	 = mainframe.o glcanvas.o
WX_OBJECTS	 = ltsviewapp.o mainframe.o glcanvas.o spinctrlfloat.o markstateruledialog.o
AT_OBJECTS	 = ltsviewapp.o action.o fsmparser.o lts.o state.o
CPP_OBJECTS	 = fileloader.o visualizer.o lts.o state.o transition.o \
		   cluster.o fsmparser.o utils.o glutils.o

OBJECTS		 = $(WX_OBJECTS) $(CPP_OBJECTS) ../liblowlevel.o
TARGETS		 = ltsview

.PHONY: all clean
.PRECIOUS: fsmparser.cpp fsmlexer.c fsmlexer.h

all : $(TARGETS)

ifndef SOURCE_DISTRIBUTION
include $(TREE_ROOT)utility/revision.mk
else
include revision
endif

CPPFLAGS += $(REVISION)

ltsview : $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) $(GLUT_LDFLAGS) $(WX_LIBS) -lATerm -o $@

$(WX_OBJECTS) : %.o: %.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $(WX_CXXFLAGS) $(ATERM_CPPFLAGS) $< -o $@

$(CPP_OBJECTS) : %.o: %.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $(ATERM_CPPFLAGS) $< -o $@

%.cpp : %.h

fsmparser.cpp : fsmparser.y fsmlexer.c
	$(BISON) --defines=fsmlexer.h -o fsmparser.cpp -p fsm $<
	$(CC) -c fsmlexer.c -o fsmlexer.o

fsmlexer.c : fsmlexer.l
	$(LEX) -o$@ -Pfsm $<

$(ICON_OBJECTS) : icons/*.xpm

$(OBJECTS) $(TARGETS) : Makefile $(CONFIG)

clean:
	-rm -f *.o $(TARGETS) fsmparser.cpp fsmlexer.c fsmlexer.h core core.*

distclean: clean

install: all
	$(INSTALL) -d $(bindir)
	$(INSTALL) ltsview $(bindir)
ifeq ($(HOST_OS),MACOSX)
	/Developer/Tools/Rez -t APPL -o ltsview ../sample.r
	/Developer/Tools/SetFile -a C ltsview
	mkdir -p ../../ltsview.app/Contents
	mkdir -p ../../ltsview.app/Contents/MacOS
	mkdir -p ../../ltsview.app/Contents/Resources
	cp ltsview.plist ../../ltsview.app/Contents/Info.plist
	echo -n "APPL????" > ../../ltsview.app/Contents/PkgInfo
	cp ltsview $(bindir)/ltsview
	ln -f $(bindir)/ltsview ../../ltsview.app/Contents/MacOS/ltsview
	cp -f ltsview.icns ../../ltsview.app/Contents/Resources/ltsview.icns
endif

