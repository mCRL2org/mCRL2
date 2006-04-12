# Relative path to the root of the source tree
-include ../../config

CXXFLAGS                       = -std=c++98 -O3# -Winline --param max-inline-insns-single=8000 \
				 --param large-function-growth=5000 --param inline-unit-growth=5000

#`-std=c++98' is too strict for the default mingw gcc
ifeq ($(HOST_OS),MINGW)
 CXXFLAGS                      = -O3
endif

CPPFLAGS=-Ilibraries -I../boost

ifeq ($(findstring -DNDEBUG,$(CPPFLAGS)),)
 CXXFLAGS                     += -g -Wall -W
endif

ifdef WX_CONFIG
WX_OBJECTS                     = squadt.o gui_project_overview.o gui_new_specification.o gui_resources.o
endif

PROJECT_MANAGER_OBJECTS        = project_manager.o exception.o settings_manager.o processor.o
TOOL_MANAGER_OBJECTS           = tool_manager.o exception.o settings_manager.o tool.o
TOOL_EXECUTOR_OBJECTS          = executor.o exception.o process.o
GUI_OBJECTS                    = squadt.o gui_project_overview.o gui_new_specification.o gui_resources.o
				 $(sort $(TOOL_EXECUTOR_OBJECTS) $(PROJECT_MANAGER_OBJECTS) $(TOOL_MANAGER_OBJECTS)) \
OBJECTS                        = $(sort $(GUI_OBJECTS) $(PROJECT_MANAGER_OBJECTS) $(TOOL_MANAGER_OBJECTS) $(TOOL_EXECUTOR_OBJECTS))
TARGETS                        = squadt

.PHONY: all install clean distclean strip

all: $(BJAM) $(TREE_ROOT)config.jam
	 @cd build; $(BOOST_BUILD) debug squadt

ifeq ($(findstring $(MAKECMDGOALS),clean distclean),)
../../config.jam ../../config ../setup.h:
	$(error Please (re)configure your source tree!)
endif

# Rules for building the maximum_revision tool and to generate a `revision' file
-include ../../utility/revision.mk

$(WX_OBJECTS): CPPFLAGS += $(BOOST_CPPFLAGS) $(WX_CPPFLAGS) $(LIBXML2_CPPFLAGS)

squadt: $(GUI_OBJECTS) md5.o md5pp.o
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(GUI_OBJECTS) $(LIBXML2_CPPFLAGS) $(WX_LDFLAGS) md5.o md5pp.o $(BOOST_LDFLAGS) $(BOOST_FILESYSTEM) -o $@

md5.o md5pp.o:
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -fPIC -c libraries/md5pp/source/$(patsubst %.o,%.cpp,$@) -o $@

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -I.. $(BOOST_CPPFLAGS) $(LIBXML2_CPPFLAGS) $< -o $@

%.o: Makefile ../../config

strip: $(TARGETS)
	$(STRIP) --strip-unneeded --strip-debug $^

install: all
	@cd build; $(BJAM) install
	$(INSTALL) -d $(bindir)
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
	cp -f squadt.icns ../../squadt.app/Contents/Resources/squadt.icns
endif

clean:
	@if test -e $(BJAM); then \
	  cd build; $(BJAM) --clean-all; \
	fi
	$(RM) -rf *.o $(TARGETS) core dependencies

distclean: clean
	$(RM) examples/tool_catalog
