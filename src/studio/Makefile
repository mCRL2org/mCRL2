-include ../config

CXXFLAGS                       = -std=c++98 -O3 -fPIC

ifeq ($(findstring -DNDEBUG,$(CPPFLAGS)),)
 CXXFLAGS                     += -g -Wall
 CPPFLAGS                      =
else
 CPPFLAGS                      = -DNDEBUG
endif

CPPFLAGS                      += -I/usr/include/libxml2
LDFLAGS                        = -lxml2

STUDIO_CPPFLAGS                = -DDISABLE_TOOLBAR

ifeq ($(findstring enable-debug,$(MAKECMDGOALS)),)
 PROJECT_MANAGER_CPPFLAGS      = -DTOOL_RUN_SUCCESSFUL
else
 PROJECT_MANAGER_CPPFLAGS      = -DPARSER_SCHEMA_VALIDATION -DTOOL_RUN_SUCCESSFUL
endif

WX_OBJECTS                     = studio.o studio_overview.o new_model_dialog.o
WX_LIBRARIES                   = $(WX_LIBS)

STUDIO_OBJECTS                 = studio.o studio_overview.o new_model_dialog.o project_manager.o
PROJECT_MANAGER_TESTER_OBJECTS = project_manager_tester.o project_manager.o
TOOL_MANAGER_TESTER_OBJECTS    = tool_manager_tester.o tool_manager.o
OBJECTS                        = $(STUDIO_OBJECTS) $(PROJECT_MANAGER_TESTER_OBJECTS) $(TOOL_MANAGER_TESTER_OBJECTS)

TEST_TARGETS                   = project_manager_tester tool_manager_tester
TARGETS                        = studio

.PHONY: all clean test enable-debug

all: $(TARGETS)

test: all $(TEST_TARGETS)

$(WX_OBJECTS):     CPPFLAGS += $(WX_CPPFLAGS)
project_manager.o: CPPFLAGS += $(PROJECT_MANAGER_CPPFLAGS)
studio.o:          CPPFLAGS += $(STUDIO_FLAGS)

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
