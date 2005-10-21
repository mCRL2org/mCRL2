CXXFLAGS                       = -std=c++98 -O3 -g -fPIC -Wall
LDFLAGS                        = -lxml2
CPPFLAGS                       = -I/usr/include/libxml2 -DPARSER_SCHEMA_VALIDATION -DTOOL_RUN_SUCCESSFUL -DDISABLE_TOOLBAR

WX_OBJECTS                     = studio.o studio_overview.o new_model_dialog.o
WX_DEFINES                     = -D__WXDEBUG__ -D__WXGTK__ -D_FILE_OFFSET_BITS=64 -D_LARGE_FILES -D_LARGEFILE_SOURCE=1 -DNO_GCC_PRAGMA
WX_INCLUDES                    = -I/usr/lib/wx/include/gtk2-unicode-debug-2.6 -I/usr/include/wx-2.6 
WX_LIBRARIES                   = -lwx_gtk2ud_xrc-2.6 -lwx_gtk2ud_html-2.6 -lwx_gtk2ud_adv-2.6 -lwx_gtk2ud_core-2.6 -lwx_baseud_xml-2.6 \
                                 -lwx_baseud_net-2.6 -lwx_baseud-2.6

STUDIO_OBJECTS                 = studio.o studio_overview.o new_model_dialog.o project_manager.o
PROJECT_MANAGER_TESTER_OBJECTS = project_manager_tester.o project_manager.o
TOOL_MANAGER_TESTER_OBJECTS    = tool_manager_tester.o tool_manager.o
OBJECTS                        = $(STUDIO_OBJECTS) $(PROJECT_MANAGER_TESTER_OBJECTS) $(TOOL_MANAGER_TESTER_OBJECTS)

TEST_TARGETS                   = project_manager_tester tool_manager_tester
TARGETS                        = studio

.PHONY: all clean test

all: $(TARGETS)

test: all $(TEST_TARGETS)

$(WX_OBJECTS): CPPFLAGS += $(WX_DEFINES) $(WX_INCLUDES)

tool_manager_tester: $(TOOL_MANAGER_TESTER_OBJECTS)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) $(WX_LIBRARIES) $(TOOL_MANAGER_TESTER_OBJECTS) -o $@

project_manager_tester: $(PROJECT_MANAGER_TESTER_OBJECTS)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) $(WX_LIBRARIES) $(PROJECT_MANAGER_TESTER_OBJECTS) -o $@

studio: $(STUDIO_OBJECTS) project_manager.h studio_overview.h
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) $(WX_LIBRARIES) $(STUDIO_OBJECTS) -o $@

%.o: %.c
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $*

$(OBJECTS) $(TARGETS): Makefile

clean:
	$(RM) -f *.o $(TARGETS) $(TEST_TARGETS) core
