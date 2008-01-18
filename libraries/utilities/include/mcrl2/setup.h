#ifndef SETTINGS_H
#define SETTINGS_H

// Locations
#define PLUGIN_DIRECTORY "/tmp/mcrl2/lib/mcrl2/plugins"
#define DATA_DIRECTORY   "/tmp/mcrl2/share/mcrl2"
#define TOOL_DIRECTORY   "/tmp/mcrl2/bin"

// Build options
#define CC               "gcc"
#define CXX              "g++"
#define CFLAGS           "-Wno-unused -O3 -rdynamic -std=c99"
#define CXXFLAGS         "-Wno-unused -O3 -rdynamic -std=c++98"
#define CPPFLAGS         ""
#define LDFLAGS          ""

// Shared object compilation
#define SCFLAGS          "-fPIC"
#define SCXXFLAGS        "-fPIC"

// Linking to shared library
#define SLDFLAGS         "-shared"

// Preprocessor and linker options for the ATerm library
#define ATERM_CPPFLAGS   "-I\"/tmp/mcrl2/include/mcrl2/aterm\""
#define ATERM_LDFLAGS    "-L\"/tmp/mcrl2/lib/mcrl2\" -lATerm"

//#define MCRL2_BCG

#endif
