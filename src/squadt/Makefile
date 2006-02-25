# Relative path to the root of the source tree
-include ../../config

CXXFLAGS                       = -std=c++98 -O3# -Winline --param max-inline-insns-single=8000 \
				 --param large-function-growth=5000 --param inline-unit-growth=5000

#`-std=c++98' is too strict for the default mingw gcc
ifeq ($(HOST_OS),MINGW)
 CXXFLAGS                      = -O3
endif

CPPFLAGS=-Ilibraries -I../src/boost

ifeq ($(findstring -DNDEBUG,$(CPPFLAGS)),)
 CXXFLAGS                     += -g -Wall -W
endif

LDFLAGS                        = $(LIBXML2_LDFLAGS)

LIBXML2_OBJECTS                = project_manager.o tool_manager.o tool.o specification.o \
				 project_manager_tester tool_manager_tester

BOOST_OBJECTS                  = gui_project_overview.o tool_manager.o project_manager.o gui_specification_properties.o \
				 specification.o settings_manager.o

ifdef WX_CONFIG
WX_OBJECTS                     = squadt.o gui_project_overview.o gui_new_specification.o gui_specification_properties.o \
				 gui_resources.o tool_executor.o logger.o process.o
WX_LIBRARIES                  := $(filter-out -mthreads,$(shell $(WX_CONFIG) --libs core base adv net))
endif

PROJECT_MANAGER_OBJECTS        = project_manager.o exception.o settings_manager.o specification.o
TOOL_MANAGER_OBJECTS           = tool_manager.o exception.o settings_manager.o tool.o
TOOL_EXECUTOR_OBJECTS          = tool_executor.o exception.o process.o
GUI_OBJECTS                    = squadt.o gui_project_overview.o gui_new_specification.o gui_specification_properties.o \
				 $(sort $(TOOL_EXECUTOR_OBJECTS) $(PROJECT_MANAGER_OBJECTS) $(TOOL_MANAGER_OBJECTS)) \
				 gui_resources.o logger.o
OBJECTS                        = $(sort $(GUI_OBJECTS) $(PROJECT_MANAGER_OBJECTS) $(TOOL_MANAGER_OBJECTS) $(TOOL_EXECUTOR_OBJECTS))

TEST_TARGETS                   = project_manager_tester tool_manager_tester tests/messenger/connection_test
TARGETS                        = squadt

.PHONY: all install clean distclean test strip

all: $(BJAM) $(TREE_ROOT)config.jam settings.h
ifeq ($(findstring $(CPPFLAGS),NDEBUG),)
	 @cd build; $(BJAM) --v2 debug squadt
else
	 @cd build; $(BJAM) --v2 release squadt
endif

$(TREE_ROOT)config.jam:
	$(error Please configure the source tree before trying to build!)

# Rules for building the maximum_revision tool and to generate a `revision' file
ifndef SOURCE_DISTRIBUTION
include ../../utility/revision.mk
else
include ../../revision
endif

CPPFLAGS += $(REVISION)

# Include dependencies 
#ifeq ($(findstring $(MAKECMDGOALS),clean distclean),)
#%.o: dependencies

#dependencies: *.cpp *.h
#	$(CXX) -MM $(CPPFLAGS) $(BOOST_CPPFLAGS) $(LIBXML2_CPPFLAGS) $(WX_CPPFLAGS) $(patsubst %o,%cpp,$(OBJECTS)) > $@
#
#-include dependencies
#endif

test: all

$(LIBXML2_OBJECTS):  CPPFLAGS += $(BOOST_CPPFLAGS) $(WX_CPPFLAGS) $(LIBXML2_CPPFLAGS)
$(BOOST_OBJECTS):    CPPFLAGS += $(BOOST_CPPFLAGS) $(WX_CPPFLAGS) $(LIBXML2_CPPFLAGS)
$(WX_OBJECTS):       CPPFLAGS += $(BOOST_CPPFLAGS) $(WX_CPPFLAGS) $(LIBXML2_CPPFLAGS)
utility/relocator.o: CPPFLAGS += $(BOOST_CPPFLAGS)

md5.o:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -fPIC -c libraries/md5pp/source/md5.cpp -o md5.o
md5pp.o:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -fPIC -c libraries/md5pp/source/md5pp.cpp -o md5pp.o

tool_manager_tester: tool_manager_tester.cpp $(TOOL_MANAGER_OBJECTS)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(BOOST_CPPFLAGS) $(TOOL_MANAGER_OBJECTS) $(BOOST_LDFLAGS) $(LDFLAGS) md5.o md5pp.o $(BOOST_FILESYSTEM) $(WX_LIBRARIES) -o $@ $<

project_manager_tester: project_manager_tester.cpp $(PROJECT_MANAGER_OBJECTS)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(BOOST_CPPFLAGS) $(PROJECT_MANAGER_OBJECTS) $(BOOST_LDFLAGS) $(LDFLAGS) md5.o md5pp.o $(BOOST_FILESYSTEM) $(WX_LIBRARIES) -o $@ $<

squadt: $(GUI_OBJECTS) md5.o md5pp.o settings.h
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(GUI_OBJECTS) $(LDFLAGS) $(WX_LIBRARIES) md5.o md5pp.o $(BOOST_LDFLAGS) $(BOOST_FILESYSTEM) -o $@

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $< -o $@

messenger.o: messenger.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -I../boost/include $< -o $@

$(OBJECTS) $(TARGETS): Makefile $(CONFIG)

strip: $(TARGETS)
	$(STRIP) --strip-unneeded --strip-debug $^

install: all
	@cd build; $(BJAM) --v2 --install
	$(INSTALL) -d $(bindir)
	$(INSTALL) -d $(datadir)
	$(INSTALL) -d $(datadir)/images
	$(INSTALL) -d $(datadir)/schemas
	$(INSTALL) -d $(datadir)/configuration
	$(INSTALL) -m 0644 pixmaps/logo.jpg $(datadir)/images
	$(INSTALL) -m 0644 schemas/tool_catalog.xsd $(datadir)/schemas
	$(GZIP) $(datadir)/schemas/tool_catalog.xsd
	$(INSTALL) -m 0644 schemas/project.xsd $(datadir)/schemas
	$(GZIP) $(datadir)/schemas/project.xsd
	$(INSTALL) -m 0644 examples/tool_catalog $(datadir)/configuration
ifeq ($(HOST_OS),MACOSX)
	/Developer/Tools/Rez -t APPL -o $(bindir)/squadt ../sample.r
	/Developer/Tools/SetFile -a C $(bindir)/squadt
	mkdir -p ../../squadt.app/Contents
	mkdir -p ../../squadt.app/Contents/MacOS
	mkdir -p ../../squadt.app/Contents/Resources
	cp squadt.plist ../../squadt.app/Contents/Info.plist
	echo -n "APPL????" > ../../squadt.app/Contents/PkgInfo
	ln -f $(bindir)/squadt ../../squadt.app/Contents/MacOS/squadt
	cp -f ../xsim.icns ../../squadt.app/Contents/Resources/squadt.icns
endif

clean:
#	@cd build; $(BJAM) --v2 --clean-all
	$(RM) -rf *.o $(TARGETS) $(TEST_TARGETS) core dependencies bin

distclean: clean
	$(RM) settings.h examples/tool_catalog

settings.h:
	$(error Please (re)configure your source tree!)
