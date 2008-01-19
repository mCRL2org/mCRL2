#ifndef SETTINGS_H
#define SETTINGS_H

// Locations
#define PLUGIN_DIRECTORY "C:/mcrl2/lib/mcrl2/plugins"
#define DATA_DIRECTORY   "C:/mcrl2/share/mcrl2"
#define TOOL_DIRECTORY   "C:/mcrl2/bin"

// Build options
#define CC               "gcc"
#define CXX              "g++"
#define CFLAGS           "-Wno-unused -O3 -std=c99"
#define CXXFLAGS         "-Wno-unused -O3 -std=c++98"
#define CPPFLAGS         "-I/usr/local/include/cvc3"
#define LDFLAGS          "-bundle -undefined dynamic_lookup"

// Shared object compilation
#define SCFLAGS          "-fPIC"
#define SCXXFLAGS        "-fPIC"

// Linking to shared library
#define SLDFLAGS         ""

// Preprocessor and linker options for the ATerm library
#define ATERM_CPPFLAGS   "-m32 -I\"c:/mcrl2/include/mcrl2/aterm\""
#define ATERM_LDFLAGS    "-L\"c:/mcrl2/lib/mcrl2\" -lATerm"

//#define MCRL2_BCG

#endif
