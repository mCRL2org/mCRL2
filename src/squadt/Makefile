-include ../../config

CXXFLAGS                       = -std=c++98 -O3# -Winline --param max-inline-insns-single=8000 \
				 --param large-function-growth=5000 --param inline-unit-growth=5000

#`-std=c++98' is to strict for the default mingw gcc
ifeq ($(HOST_OS),MINGW)
 CXXFLAGS                      = -O3
endif


ifeq ($(findstring -DNDEBUG,$(CPPFLAGS)),)
 CXXFLAGS                     += -g -Wall -W
endif

LDFLAGS                        = $(LIBXML2_LDFLAGS)

LIBXML2_OBJECTS                = xml_text_reader.o project_manager.o tool_manager.o tool.o specification.o \
				 project_manager_tester tool_manager_tester

BOOST_OBJECTS                  = gui_project_overview.o tool_manager.o project_manager.o gui_specification_properties.o \
				 settings_manager.o specification.o

WX_OBJECTS                     = squadt.o gui_project_overview.o gui_new_specification.o gui_specification_properties.o \
				 gui_resources.o tool_executor.o logger.o process.o
WX_LIBRARIES                  := $(filter-out -mthreads,$(shell $(WX_CONFIG) --libs core base adv net))

PROJECT_MANAGER_OBJECTS        = project_manager.o specification.o xml_text_reader.o settings_manager.o
TOOL_MANAGER_OBJECTS           = tool_manager.o tool.o xml_text_reader.o settings_manager.o
TOOL_EXECUTOR_OBJECTS          = tool_executor.o process.o
GUI_OBJECTS                    = squadt.o gui_project_overview.o gui_new_specification.o gui_specification_properties.o \
				 $(sort $(TOOL_EXECUTOR_OBJECTS) $(PROJECT_MANAGER_OBJECTS) $(TOOL_MANAGER_OBJECTS)) \
				 gui_resources.o logger.o
OBJECTS                        = $(sort $(GUI_OBJECTS) $(PROJECT_MANAGER_OBJECTS) $(TOOL_MANAGER_OBJECTS) $(TOOL_EXECUTOR_OBJECTS))

TEST_TARGETS                   = project_manager_tester tool_manager_tester tests/messenger/connection_test
TARGETS                        = squadt libmd5.a

.PHONY: all install clean distclean test strip

all: $(TARGETS)
	@$(RM) revision

ifeq ($(findstring $(MAKECMDGOALS),clean distclean),)
# Add revision number (when not building a source distribution)
ifndef SOURCE_DISTRIBUTION
revision: ../../utility/get_revision
	@echo "CPPFLAGS += -DREVISION=$$(../../utility/get_revision)" > revision

../../utility/get_revision: ../../utility/get_revision.cpp
	$(CXX) $(BOOST_CPPFLAGS) $< $(subst $(libdir),$(BOOST_PREFIX)/lib,$(BOOST_LDFLAGS)) $(BOOST_FILESYSTEM) -o $@ 
endif

-include revision

# Include dependencies 
%.o: dependencies

dependencies: *.cpp *.h
	$(CXX) -MM $(CPPFLAGS) $(BOOST_CPPFLAGS) $(LIBXML2_CPPFLAGS) $(WX_CPPFLAGS) $(patsubst %o,%cpp,$(OBJECTS)) > $@

-include dependencies

endif

test: all $(TEST_TARGETS)
	@tests/messenger/connection_test

$(LIBXML2_OBJECTS):         CPPFLAGS += $(BOOST_CPPFLAGS) $(WX_CPPFLAGS) $(LIBXML2_CPPFLAGS)
$(BOOST_OBJECTS):           CPPFLAGS += $(BOOST_CPPFLAGS) $(WX_CPPFLAGS) $(LIBXML2_CPPFLAGS)
$(WX_OBJECTS):              CPPFLAGS += $(BOOST_CPPFLAGS) $(WX_CPPFLAGS) $(LIBXML2_CPPFLAGS)
settings_manager.o:         CPPFLAGS += $(BOOST_CPPFLAGS) $(WX_CPPFLAGS) $(LIBXML2_CPPFLAGS) -DDATA_DIRECTORY=\"$(datadir)\"
utility/relocator.o:        CPPFLAGS += $(BOOST_CPPFLAGS)

# XML schema validation can be triggered by adding to tool_manager project_manager
#$(PROJECT_MANAGER_OBJECTS): CPPFLAGS += -DPARSER_SCHEMA_VALIDATION
#$(TOOL_MANAGER_OBJECTS):    CPPFLAGS += -DPARSER_SCHEMA_VALIDATION

libmd5.a: libmd5/md5pp.cpp libmd5/md5pp.h
	$(CXX) $(CXXFLAGS) -fPIC -c libmd5/md5.c -o libmd5/md5.o
	$(CXX) $(CXXFLAGS) -fPIC -c libmd5/md5pp.cpp -o libmd5/md5pp.o
	$(AR) $(ARFLAGS) $@ libmd5/md5pp.o libmd5/md5.o

tool_manager_tester: tool_manager_tester.cpp $(TOOL_MANAGER_OBJECTS)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(BOOST_CPPFLAGS) $(TOOL_MANAGER_OBJECTS) $(BOOST_LDFLAGS) $(LDFLAGS) libmd5.a $(BOOST_FILESYSTEM) $(WX_LIBRARIES) -o $@ $<

project_manager_tester: project_manager_tester.cpp $(PROJECT_MANAGER_OBJECTS)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(BOOST_CPPFLAGS) $(PROJECT_MANAGER_OBJECTS) $(BOOST_LDFLAGS) $(LDFLAGS) libmd5.a $(BOOST_FILESYSTEM) $(WX_LIBRARIES) -o $@ $<

squadt: $(GUI_OBJECTS) libmd5.a
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(GUI_OBJECTS) $(LDFLAGS) $(WX_LIBRARIES) libmd5.a $(BOOST_LDFLAGS) $(BOOST_FILESYSTEM) -o $@

utility/relocator: utility/relocator.o settings_manager.o $(TOOL_MANAGER_OBJECTS)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) $(subst $(libdir),$(BOOST_PREFIX)/lib,$(BOOST_LDFLAGS)) $^ $(BOOST_FILESYSTEM) -lxml2 -o $@

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $< -o $@

tests/messenger/connection_test: messenger.o tests/messenger/connection_test.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -I../boost/include -I. -lpthread -lboost_unit_test_framework $^ -o $@

messenger.o: messenger.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -I../boost/include $< -o $@

$(OBJECTS) $(TARGETS): Makefile

strip: $(TARGETS)
	$(STRIP) --strip-unneeded --strip-debug $^

install: all utility/relocator
	$(INSTALL) -d $(bindir)
	$(INSTALL) squadt $(bindir)
	$(INSTALL) -d $(datadir)
	$(INSTALL) -d $(datadir)/images
	$(INSTALL) -d $(datadir)/schemas
	$(INSTALL) -d $(datadir)/configuration
	$(INSTALL) -m 0644 pixmaps/logo.jpg $(datadir)/images
	$(INSTALL) -m 0644 schemas/tool_catalog.xsd $(datadir)/schemas
	$(GZIP) $(datadir)/schemas/tool_catalog.xsd
	$(INSTALL) -m 0644 schemas/project.xsd $(datadir)/schemas
	$(GZIP) $(datadir)/schemas/project.xsd
	$(INSTALL) -m 0644 examples/tool_catalog.xml $(datadir)/configuration
	utility/relocator $(bindir) $(datadir)/configuration/tool_catalog.xml
ifeq ($(HOST_OS),MACOSX)
	/Developer/Tools/Rez -t APPL -o squadt ../sample.r
	/Developer/Tools/SetFile -a C squadt
	mkdir -p ../../squadt.app/Contents
	mkdir -p ../../squadt.app/Contents/MacOS
	mkdir -p ../../squadt.app/Contents/Resources
	cp squadt.plist ../../squadt.app/Contents/Info.plist
	echo -n "APPL????" > ../../squadt.app/Contents/PkgInfo
	cp squadt $(bindir)/squadt
	ln -f $(bindir)/squadt ../../squadt.app/Contents/MacOS/squadt
	cp -f ../xsim.icns ../../squadt.app/Contents/Resources/squadt.icns
endif

clean:
	$(RM) *.o $(TARGETS) $(TEST_TARGETS) utility/*.o utility/relocator core revision dependencies

distclean: clean
