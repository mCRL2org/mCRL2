-include ../config

CXXFLAGS                       = -std=c++98 -O3 -fPIC

ifeq ($(findstring -DNDEBUG,$(CPPFLAGS)),)
 CXXFLAGS                     += -g -Wall
 CPPFLAGS                      =
 PROJECT_MANAGER_CPPFLAGS      = -DTOOL_RUN_SUCCESSFUL
else
 CPPFLAGS                      = -DNDEBUG
 PROJECT_MANAGER_CPPFLAGS      = -DPARSER_SCHEMA_VALIDATION -DTOOL_RUN_SUCCESSFUL
endif

CPPFLAGS                      += -I/usr/include/libxml2
LDFLAGS                        = -lxml2

GUI_CPPFLAGS                   = -DDISABLE_TOOLBAR

WX_OBJECTS                     = studio.o studio_overview.o new_specification.o specification_properties.o resources.o
WX_LIBRARIES                   = $(WX_LIBS)

GUI_OBJECTS                    = studio.o studio_overview.o new_specification.o specification_properties.o resources.o
PROJECT_MANAGER_OBJECTS        = project_manager.o
TOOL_MANAGER_OBJECTS           = tool_manager.o
OBJECTS                        = $(GUI_OBJECTS) $(PROJECT_MANAGER_OBJECTS) $(TOOL_MANAGER_OBJECTS)

TEST_TARGETS                   = project_manager_tester tool_manager_tester
TARGETS                        = studio

.PHONY: all clean test enable-debug

all: $(TARGETS)

test: all $(TEST_TARGETS)

$(WX_OBJECTS):              CPPFLAGS += $(WX_CPPFLAGS)
$(PROJECT_MANAGER_OBJECTS): CPPFLAGS += $(PROJECT_MANAGER_CPPFLAGS)
$(GUI_OBJECTS):             CPPFLAGS += $(GUI_CPPFLAGS)

tool_manager_tester: tool_manager_tester.cpp $(TOOL_MANAGER_OBJECTS)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) $(WX_LIBRARIES) $(TOOL_MANAGER_OBJECTS) -o $@

project_manager_tester: project_manager_tester.cpp $(PROJECT_MANAGER_OBJECTS) 
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) $(WX_LIBRARIES) $(PROJECT_MANAGER_OBJECTS) -o $@

studio: $(GUI_OBJECTS) $(PROJECT_MANAGER_OBJECTS)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) $(WX_LIBRARIES) $(GUI_OBJECTS) $(PROJECT_MANAGER_OBJECTS) -o $@

%.o: %.c
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $<

$(OBJECTS) $(TARGETS): Makefile

install:
	$(INSTALL) -d $(bindir)
	$(INSTALL) studio $(bindir)

clean:
	$(RM) -f *.o $(TARGETS) $(TEST_TARGETS) core
