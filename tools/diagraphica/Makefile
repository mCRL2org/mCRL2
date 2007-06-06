GXX = g++
WX_LIBS        := `wx-config --libs core base gl`
WX_CXXFLAGS    := `wx-config --cxxflags`

OBJECTS        = arcdiagram.o attribute.o bitmappanel.o\
bundle.o cluster.o colleague.o colorbutton.o colorchooser.o\
combnplot.o corrlplot.o diagram.o diagrameditor.o\
diagraph.o distrplot.o dof.o droptarget.o edge.o\
examiner.o frame.o glcanvas.o graph.o node.o\
opacitychooser.o parser.o popupframe.o settingsframe.o\
shape.o simulator.o spinctrlfloat.o tinystr.o tinyxml.o\
tinyxmlerror.o tinyxmlparser.o utils.o value.o\
visualizer.o visutils.o\
timeseries.o attrdiscr.o attrconti.o \
partitionframe.o

TARGETS        = diagraph

.PHONY: all clean

all : $(TARGETS)

diagraph : $(OBJECTS)
	$(GXX) $(OBJECTS) $(WX_LIBS) -o $@

$(OBJECTS) : %.o : %.cpp
	$(GXX) -c $(WX_CXXFLAGS) $< -o $@

%.cpp : %.h

$(OBJECTS) $(TARGETS) : Makefile

clean:
	-rm -f $(OBJECTS) $(TARGETS)
