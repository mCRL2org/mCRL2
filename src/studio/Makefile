-include ../config

CXXFLAGS                       = -std=c++98 -O3 -Winline --param max-inline-insns-single=8000 \
				 --param large-function-growth=5000 --param inline-unit-growth=5000

#`-std=c++98' is to strict for the default mingw gcc
ifeq ($(HOST_OS),MINGW)
 CXXFLAGS                      = -O3
endif

ifeq ($(findstring -DNDEBUG,$(CPPFLAGS)),)
 CXXFLAGS                     += -g -Wall
 PROJECT_MANAGER_CPPFLAGS      = #-DPARSER_SCHEMA_VALIDATION 
else
 PROJECT_MANAGER_CPPFLAGS      =
endif

LDFLAGS                        = $(LIBXML2_LDFLAGS)

# Other flags
GUI_CPPFLAGS                   = -DDISABLE_TOOLBAR

LIBXML2_OBJECTS                = xml_text_reader.o project_manager.o tool_manager.o tool.o specification.o \
				 project_manager_tester tool_manager_tester

BOOST_OBJECTS                  = studio_overview.o tool_manager.o project_manager.o specification_properties.o settings_manager.o
BOOST_CPPFLAGS                 = -I$(BOOST_PREFIX)/include
BOOST_LIBRARIES                = -L$(BOOST_PREFIX)/lib -lboost_filesystem
WX_OBJECTS                     = studio.o studio_overview.o new_specification.o specification_properties.o \
				 resources.o tool_executor.o
WX_LIBRARIES                   = $(WX_LIBS)

PROJECT_MANAGER_OBJECTS        = project_manager.o specification.o xml_text_reader.o settings_manager.o
TOOL_MANAGER_OBJECTS           = tool_manager.o tool.o xml_text_reader.o settings_manager.o
GUI_OBJECTS                    = studio.o studio_overview.o new_specification.o specification_properties.o \
				 resources.o $(sort $(PROJECT_MANAGER_OBJECTS) $(TOOL_MANAGER_OBJECTS)) \
				 tool_executor.o
OBJECTS                        = $(sort $(GUI_OBJECTS) $(PROJECT_MANAGER_OBJECTS) $(TOOL_MANAGER_OBJECTS))

TEST_TARGETS                   = project_manager_tester tool_manager_tester
TARGETS                        = studio

.PHONY: all install clean distclean test

all: $(TARGETS)
	@$(RM) revision

# Add revision number
%.o: revision

revision: utility/get_revision
	@echo "CPPFLAGS += -DREVISION=$$(utility/get_revision)" > revision

utility/get_revision: utility/get_revision.cpp
	$(CXX) -I$(BOOST_PREFIX)/include -L$(BOOST_PREFIX)/lib -lboost_filesystem $< -o $@ 

-include revision

test: all $(TEST_TARGETS)

$(LIBXML2_OBJECTS):         CPPFLAGS += $(LIBXML2_CPPFLAGS)
$(BOOST_OBJECTS):           CPPFLAGS += $(BOOST_CPPFLAGS)
$(WX_OBJECTS):              CPPFLAGS += $(WX_CPPFLAGS)
$(PROJECT_MANAGER_OBJECTS): CPPFLAGS += $(PROJECT_MANAGER_CPPFLAGS)
$(GUI_OBJECTS):             CPPFLAGS += $(GUI_CPPFLAGS)
settings_manager.o:         CPPFLAGS += -DSCHEMA_DATA=\"$(datadir)\" -DTOOL_DATA=\"$(datadir)\"
utility/relocator.o:        CPPFLAGS += $(BOOST_CPPFLAGS)

tool_manager_tester: tool_manager_tester.cpp $(TOOL_MANAGER_OBJECTS)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) $(TOOL_MANAGER_OBJECTS) $(WX_LIBRARIES) -o $@ $<

project_manager_tester: project_manager_tester.cpp $(PROJECT_MANAGER_OBJECTS) 
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) $(PROJECT_MANAGER_OBJECTS) $(WX_LIBRARIES) -o $@ $<

studio: $(GUI_OBJECTS)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) $(GUI_OBJECTS) $(WX_LIBRARIES) $(BOOST_LIBRARIES) -o $@

utility/relocator: utility/relocator.o settings_manager.o $(TOOL_MANAGER_OBJECTS)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) $(BOOST_LIBRARIES) $^ -o $@

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $< -o $@

$(OBJECTS) $(TARGETS): Makefile

install: all utility/relocator
	$(INSTALL) -d $(bindir)
	$(INSTALL) studio $(bindir)
	$(INSTALL) -d $(datadir)
	$(INSTALL) -m 0644 schemas/tool_catalog.xsd $(datadir)
	$(GZIP) $(datadir)/tool_catalog.xsd
	$(INSTALL) -m 0644 schemas/studio_project.xsd $(datadir)
	$(GZIP) $(datadir)/studio_project.xsd
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
	$(RM) *.o $(TARGETS) $(TEST_TARGETS) utility/*.o utility/relocator utility/get_revision core revision

distclean: clean
