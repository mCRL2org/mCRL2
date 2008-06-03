// Author(s): Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpspp.cpp

#define NAME "lpspp"
#define AUTHOR "Aad Mathijssen"

#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <cassert>

#include <aterm2.h>
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/core/print.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/utilities/aterm_ext.h"
#include "mcrl2/utilities/command_line_interface.h" // after messaging.h, rewrite.h and bdd_path_eliminator.h

using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2;

//local declarations

int main(int argc, char *argv[]);
//main function where:
//  argc represents the number of arguments
//  argv represents the arguments

struct t_tool_options {
  std::string  specification_file_name;
  std::string  output_file_name;
  t_pp_format  format;
};

static bool PrintSpecificationFileName(t_tool_options const& tool_options);

static void PrintPPFormat(FILE *stream, t_pp_format pp_format);
/*Pre: stream points to a stream to which can be written
 *Ret: a string representation of pp_format is written to stream
 */

//implementation

t_tool_options parse_command_line(int ac, char** av) {
  interface_description clinterface(av[0], NAME, AUTHOR, "[OPTION]... [INFILE [OUTFILE]]\n",
    "Print the mCRL2 LPS in INFILE to OUTFILE in a human readable format. If OUTFILE "
    "is not present, stdout is used. If INFILE is not present, stdin is used.",
    "The LPS printed in the default format might not be a well-formed mCRL2 specification,"
    "because the proc and init sections could be preceded by declarations of free variables"
    "denoted by var");

  clinterface.
    add_option("format", make_mandatory_argument("FORMAT"),
      "print the LPS in the specified FORMAT:\n"
      "  'internal' for a textual ATerm representation of the internal format,\n"
      "  'default' for an mCRL2 specification (default), or\n"
      "  'debug' for 'default' with the exceptions that data expressions are printed in prefix notation using identifiers from the internal format, each data equation is put in a separate data equation section, and next states of process references are printed in assignment notation", 'f');

  command_line_parser parser(clinterface, ac, av);

  t_tool_options tool_options;

  tool_options.format = ppDefault;

  if (parser.options.count("format")) {
    char const* format(parser.option_argument("format").c_str());

    if (std::strncmp(format, "internal", 9) == 0) {
      tool_options.format = ppInternal;
    }
    else if (std::strncmp(format, "debug", 6) == 0) {
      tool_options.format = ppDebug;
    }
    else if (std::strncmp(format, "default", 8) != 0) {
      parser.error("option -f has illegal argument '" + std::string(format) + "'");
    }
  } 

  if (2 < parser.arguments.size()) {
    parser.error("too many file arguments");
  }
  else {
    if (0 < parser.arguments.size()) {
      tool_options.specification_file_name = parser.arguments[0];
    }
    if (1 < parser.arguments.size()) {
      tool_options.output_file_name = parser.arguments[1];
    }
  }

  return tool_options;
}

int main(int argc, char* argv[])
{
  MCRL2_ATERM_INIT(argc, argv)

  try {
    //print specification  
    return PrintSpecificationFileName(parse_command_line(argc, argv)) == false;
  }
  catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }

  return EXIT_FAILURE;
}

bool PrintSpecificationFileName(t_tool_options const& tool_options)
{
  FILE *SpecStream      = NULL;
  FILE *OutputStream    = NULL;
  //open SpecFileName for reading
  if (tool_options.specification_file_name.empty()) {
    SpecStream = stdin;
    gsDebugMsg("input from stdin.\n");
  } else {
    SpecStream = fopen(tool_options.specification_file_name.c_str(), "rb");
  }
  if (SpecStream == NULL) {
    gsErrorMsg("could not open input file '%s' for reading ",
      tool_options.specification_file_name.c_str());
    perror(NULL);
    return false;
  }
  assert(SpecStream != NULL);
  //read specification from SpecStream
  if ( SpecStream != stdin )
    gsDebugMsg("input file '%s' is opened for reading.\n",
                                 tool_options.specification_file_name.c_str());
  ATermAppl Spec = (ATermAppl) ATreadFromFile(SpecStream);
  if (Spec == NULL) {
    if (SpecStream == stdin) {
      gsErrorMsg("could not read LPS from stdin\n");
    } else {
      gsErrorMsg("could not read LPS from '%s'\n",
                 tool_options.specification_file_name.c_str());
      fclose(SpecStream);
    }
    return false;
  }
  assert(Spec != NULL);
  if (!gsIsSpecV1(Spec)) {
    if (SpecStream == stdin) {
      gsErrorMsg("stdin does not contain an LPS\n");
    } else {
      gsErrorMsg("'%s' does not contain an LPS\n",
                 tool_options.specification_file_name.c_str());
      fclose(SpecStream);
    }
    return false;
  }
  assert(gsIsSpecV1(Spec));
  //open output file for writing or set to stdout
  if (tool_options.output_file_name.empty()) {
    OutputStream = stdout;
    gsDebugMsg("output to stdout.\n");
  } else {  
    OutputStream = fopen(tool_options.output_file_name.c_str(), "w");
    if (OutputStream == NULL) {
      gsErrorMsg("could not open output file '%s' for writing ",
        tool_options.output_file_name.c_str());
      perror(NULL);
      if (SpecStream != stdin) {
        fclose(SpecStream);
      }
      return false;
    }
    gsDebugMsg("output file '%s' is opened for writing.\n", tool_options.output_file_name.c_str());
  }
  assert(OutputStream != NULL);
  //print Spec to OutputStream
  if (gsVerbose) {
    fprintf(stderr, "printing LPS from ");
    if (SpecStream == stdin) {
      fprintf(stderr, "stdin");
    } else {
      fprintf(stderr, "'%s'", tool_options.specification_file_name.c_str());
    }
    fprintf(stderr, " to ");
    if (OutputStream == stdout) {
      fprintf(stderr, "stdout");
    } else {
      fprintf(stderr, "'%s'", tool_options.output_file_name.c_str());
    }
    fprintf(stderr, " in the ");
    PrintPPFormat(stderr, tool_options.format);
    fprintf(stderr, " format\n");
  }
  //pretty print Spec to OutputStream
  PrintPart_C(OutputStream, (ATerm) Spec, tool_options.format);
  if (SpecStream != stdin) {
    fclose(SpecStream);
  }
  if (OutputStream != stdout) {
    fclose(OutputStream);
  }
  return true;
}

void PrintPPFormat(FILE *stream, t_pp_format pp_format)
{
  if (pp_format == ppDefault) {
    fprintf(stream, "default");
  } else if (pp_format == ppDebug) {
    fprintf(stream, "debug");
  } else if (pp_format == ppInternal) {
    fprintf(stream, "internal");
  }
}
