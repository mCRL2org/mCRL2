CXXFLAGS                       = -std=c++98 -O3 -g -fPIC -Wall
LDFLAGS                        = -lxml2
CPPFLAGS                       = -I/usr/include/libxml2 -DPARSER_SCHEMA_VALIDATION -DTOOL_RUN_SUCCESSFUL -DDISABLE_TOOLBAR

WX_OBJECTS                     = studio.o studio_overview.o new_model_dialog.o
WX_CFLAGS                      = $$(wx-config --cflags)
WX_LIBRARIES                   = $$(wx-config --libs)

STUDIO_OBJECTS                 = studio.o studio_overview.o new_model_dialog.o project_manager.o
PROJECT_MANAGER_TESTER_OBJECTS = project_manager_tester.o project_manager.o
TOOL_MANAGER_TESTER_OBJECTS    = tool_manager_tester.o tool_manager.o
OBJECTS                        = $(STUDIO_OBJECTS) $(PROJECT_MANAGER_TESTER_OBJECTS) $(TOOL_MANAGER_TESTER_OBJECTS)

TEST_TARGETS                   = project_manager_tester tool_manager_tester
TARGETS                        = studio

.PHONY: all clean test

all: $(TARGETS)

test: all $(TEST_TARGETS)

$(WX_OBJECTS): CPPFLAGS += $(WX_CFLAGS)

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
