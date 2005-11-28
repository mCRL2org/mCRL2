-include ../config

CXXFLAGS                       = -std=c++98 -O3 -Winline --param max-inline-insns-single=8000 \
				 --param large-function-growth=5000 --param inline-unit-growth=5000

#`-std=c++98' is to strict for the default mingw gcc
ifeq ($(HOST_OS),MINGW)
 CXXFLAGS                      = -O3
endif


ifeq ($(findstring -DNDEBUG,$(CPPFLAGS)),)
 CXXFLAGS                     += -g -Wall
endif

LDFLAGS                        = $(LIBXML2_LDFLAGS)

LIBXML2_OBJECTS                = xml_text_reader.o project_manager.o tool_manager.o tool.o specification.o \
				 project_manager_tester tool_manager_tester

BOOST_OBJECTS                  = gui_project_overview.o tool_manager.o project_manager.o gui_specification_properties.o \
				 settings_manager.o tool_executor.o
BOOST_CPPFLAGS                 = -I$(BOOST_PREFIX)/include
BOOST_LDFLAGS = -L$(BOOST_PREFIX)/lib

ifdef BUILD_BOOST
 ifneq ($(HOST_OS),MACOSX)
  BOOST_LDFLAGS := -Wl,-rpath=$(libdir) $(BOOST_LDFLAGS)
 endif
endif

WX_OBJECTS                     = studio.o gui_project_overview.o gui_new_specification.o gui_specification_properties.o \
				 gui_resources.o tool_executor.o logger.o process.o
WX_LIBRARIES                  := $(shell $(WX_CONFIG) --libs core base adv net)

PROJECT_MANAGER_OBJECTS        = project_manager.o specification.o xml_text_reader.o settings_manager.o
TOOL_MANAGER_OBJECTS           = tool_manager.o tool.o xml_text_reader.o settings_manager.o
GUI_OBJECTS                    = studio.o gui_project_overview.o gui_new_specification.o gui_specification_properties.o \
				 gui_resources.o $(sort $(PROJECT_MANAGER_OBJECTS) $(TOOL_MANAGER_OBJECTS)) \
				 tool_executor.o process.o logger.o
OBJECTS                        = $(sort $(GUI_OBJECTS) $(PROJECT_MANAGER_OBJECTS) $(TOOL_MANAGER_OBJECTS))

TEST_TARGETS                   = project_manager_tester tool_manager_tester
TARGETS                        = studio

.PHONY: all install clean distclean test strip

all: $(TARGETS)
	@$(RM) revision dependencies

ifeq ($(findstring $(MAKECMDGOALS),clean distclean),)
# Add revision number
revision: ../utility/get_revision
	@echo "CPPFLAGS += -DREVISION=$$(../utility/get_revision)" > revision

../utility/get_revision: ../utility/get_revision.cpp
	$(CXX) -I$(BOOST_PREFIX)/include $(BOOST_LDFLAGS) -lboost_filesystem $< -o $@ 

-include revision

# Include dependencies 
%.o: dependencies

dependencies: $(patsubst %.o,%.cpp,$(OBJECTS))
	$(CXX) -MM $(CPPFLAGS) $(BOOST_CPPFLAGS) $(LIBXML2_CPPFLAGS) $(WX_CPPFLAGS) $^ > $@

-include dependencies

endif

test: all $(TEST_TARGETS)

$(LIBXML2_OBJECTS):         CPPFLAGS += $(LIBXML2_CPPFLAGS)
$(BOOST_OBJECTS):           CPPFLAGS += $(BOOST_CPPFLAGS)
$(WX_OBJECTS):              CPPFLAGS += $(WX_CPPFLAGS)
gui_project_overview.o:     CPPFLAGS += -DDISABLE_TOOLBAR
settings_manager.o:         CPPFLAGS += -DSCHEMA_DATA=\"$(datadir)\" -DTOOL_DATA=\"$(datadir)\"
utility/relocator.o:        CPPFLAGS += $(BOOST_CPPFLAGS)

# XML schema validation can be triggered by adding to tool_manager project_manager
#$(PROJECT_MANAGER_OBJECTS): CPPFLAGS += -DPARSER_SCHEMA_VALIDATION
#$(TOOL_MANAGER_OBJECTS):    CPPFLAGS += -DPARSER_SCHEMA_VALIDATION

tool_manager_tester: tool_manager_tester.cpp $(TOOL_MANAGER_OBJECTS)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(TOOL_MANAGER_OBJECTS) $(LDFLAGS) $(WX_LIBRARIES) -o $@ $<

project_manager_tester: project_manager_tester.cpp $(PROJECT_MANAGER_OBJECTS) 
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(PROJECT_MANAGER_OBJECTS) $(LDFLAGS) $(WX_LIBRARIES) -o $@ $<

studio: $(GUI_OBJECTS)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(GUI_OBJECTS) $(LDFLAGS) $(WX_LIBRARIES) $(BOOST_LDFLAGS) -lboost_filesystem -o $@

utility/relocator: utility/relocator.o settings_manager.o $(TOOL_MANAGER_OBJECTS)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) $(BOOST_LDFLAGS) -lboost_filesystem $^ -o $@

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $< -o $@

$(OBJECTS) $(TARGETS): Makefile

strip: $(TARGETS)
	$(STRIP) --strip-unneeded --strip-debug $^

install: all utility/relocator
	$(INSTALL) -d $(bindir)
	$(INSTALL) studio $(bindir)
	$(INSTALL) -d $(datadir)
	$(INSTALL) -m 0644 schemas/tool_catalog.xsd $(datadir)
	$(GZIP) $(datadir)/tool_catalog.xsd
	$(INSTALL) -m 0644 schemas/project.xsd $(datadir)
	$(GZIP) $(datadir)/project.xsd
	$(INSTALL) -m 0644 examples/tool_catalog.xml $(datadir)
	utility/relocator $(bindir) $(datadir)/tool_catalog.xml
ifeq ($(HOST_OS),MACOSX)
	/Developer/Tools/Rez -t APPL -o studio ../sample.r
	/Developer/Tools/SetFile -a C studio
	mkdir -p ../../studio.app/Contents
	mkdir -p ../../studio.app/Contents/MacOS
	mkdir -p ../../studio.app/Contents/Resources
	cp studio.plist ../../studio.app/Contents/Info.plist
	echo -n "APPL????" > ../../studio.app/Contents/PkgInfo
	cp studio $(bindir)/studio
	ln -f $(bindir)/studio ../../studio.app/Contents/MacOS/studio
	cp -f ../xsim.icns ../../studio.app/Contents/Resources/studio.icns
endif

clean:
	$(RM) *.o $(TARGETS) $(TEST_TARGETS) utility/*.o utility/relocator core revision dependencies

distclean: clean
