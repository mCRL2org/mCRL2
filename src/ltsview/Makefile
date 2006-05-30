# Relative path to the root of the source tree
-include ../../config

WX_LIBS		:= `$(WX_CONFIG) --libs core base gl`
WX_CXXFLAGS	:= `$(WX_CONFIG) --cxxflags`
CPPFLAGS        += -I..

ICON_OBJECTS	 = mainframe.o glcanvas.o
WX_OBJECTS	 = ltsviewapp.o mainframe.o glcanvas.o spinctrlfloat.o \
		   markstateruledialog.o colorbutton.o
AT_OBJECTS	 = ltsviewapp.o action.o fsmparser.o lts.o state.o
CPP_OBJECTS	 = fileloader.o visualizer.o lts.o state.o transition.o \
		   cluster.o fsmparser.o utils.o glutils.o ../liblowlevel.o

OBJECTS		 = $(WX_OBJECTS) $(CPP_OBJECTS)
TARGETS		 = ltsview

.PHONY: all clean
.PRECIOUS: fsmparser.cpp fsmlexer.cpp fsmlexer.h

all : $(TARGETS)

-include ../../utility/revision.mk

ltsview : $(OBJECTS)
	$(MAKE) -C .. aterm/libATerm.a
	$(CXX) $(LDFLAGS) $(OBJECTS) $(GLUT_LDFLAGS) $(ATERM_LDFLAGS) $(WX_LIBS) -lATerm -o $@

$(WX_OBJECTS) : %.o: %.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $(WX_CXXFLAGS) $(ATERM_CPPFLAGS) $< -o $@

$(CPP_OBJECTS) : %.o: %.cpp
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $(ATERM_CPPFLAGS) $< -o $@

%.cpp : %.h

fsmparser.cpp : fsmparser.yy fsmlexer.cpp
	$(BISON) --defines=fsmlexer.h -o fsmparser.cpp -p fsm $<

fsmlexer.cpp : fsmlexer.ll
	$(FLEX) -o$@ -Pfsm $<

$(ICON_OBJECTS) : icons/*.xpm

$(OBJECTS) $(TARGETS) : Makefile $(CONFIG)

clean:
	-rm -f *.o $(TARGETS) fsmparser.cpp fsmlexer.cpp fsmlexer.h core core.*

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

