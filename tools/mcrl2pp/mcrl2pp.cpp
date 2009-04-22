// Author(s): Yaroslav Usenko
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2pp.cpp

#include "boost.hpp" // precompiled headers

#define NAME "mcrl2pp"
#define AUTHOR "Aad Mathijsen and Yaroslav S. Usenko"

#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <cassert>

#include <aterm2.h>
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/core/detail/aterm_io.h"
#include "mcrl2/core/print.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/utilities/command_line_interface.h"
#include "mcrl2/utilities/command_line_messaging.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/detail/data_specification_compatibility.h"

using namespace mcrl2::utilities;
using namespace mcrl2::core;
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

  bool parse_command_line(int ac, char** av) {
    interface_description clinterface(av[0], NAME, AUTHOR,
      "pretty print an mCRL2 specification, an LPS, or a PBES",
      "[OPTION]... [INFILE [OUTFILE]]\n",
      "Print an mCRL2 specification, an LPS, or a PBES in INFILE to OUTFILE in a human readable format. If OUTFILE "
      "is not present, stdout is used. If INFILE is not present, stdin is used.",
      "The mCRL2 specification or the LPS printed in the default format might not be a well-formed mCRL2 specification, "
      "because the proc and init sections could be preceded by declarations of free variables "
      "denoted by var.");

    clinterface.
      add_option("format", make_mandatory_argument("FORMAT"),
        "print the mCRL2 specification, the LPS, or the PBES in the specified FORMAT:\n"
        "  'internal' for a textual ATerm representation of the internal format,\n"
        "  'default' for an mCRL2 specification (default), or\n"
        "  'debug' for 'default' with the exceptions that data expressions are printed in prefix notation using identifiers from the internal format, each data equation is put in a separate data equation section, and next states of process references are printed in assignment notation", 'f');

    command_line_parser parser(clinterface, ac, av);

    if (parser.continue_execution()) {
      format = ppDefault;

      if (parser.options.count("format")) {
        std::string str_format(parser.option_argument("format"));
        if (str_format == "internal") {
          format = ppInternal;
        } else if (str_format == "debug") {
          format = ppDebug;
        } else if (str_format != "default") {
          parser.error("option -f/--format has illegal argument '" + str_format + "'");
        }
      }

      if (2 < parser.arguments.size()) {
        parser.error("too many file arguments");
      }
      if (0 < parser.arguments.size()) {
        specification_file_name = parser.arguments[0];
      }
      if (1 < parser.arguments.size()) {
        output_file_name = parser.arguments[1];
      }
    }

    return parser.continue_execution();
  }
};

static void print_specification_file_name(t_tool_options const& tool_options);

static std::string pp_format_to_string(t_pp_format pp_format);

//implementation

int main(int argc, char* argv[])
{
  MCRL2_ATERM_INIT(argc, argv)

  try {
    t_tool_options options;

    if (options.parse_command_line(argc, argv)) {
      //print specification
      print_specification_file_name(options);
    }
  }
  catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

void print_specification_file_name(t_tool_options const& tool_options)
{
  std::string str_in  = (tool_options.specification_file_name.empty())?"stdin":("'" + tool_options.specification_file_name + "'");
  std::string str_out = (tool_options.output_file_name.empty())?"stdout":("'" + tool_options.output_file_name + "'");
  ATermAppl spec = (ATermAppl) mcrl2::core::detail::load_aterm(tool_options.specification_file_name);
  if (! (    mcrl2::core::detail::gsIsLinProcSpec(spec)
          || mcrl2::core::detail::gsIsProcSpec(spec)
          || mcrl2::core::detail::gsIsPBES(spec)
          
        )
     ) {
    throw mcrl2::runtime_error(str_in + " does not contain an mCRL2 specification, an LPS, or a PBES");
  }
  std::string spec_type = "mCRL2 specification";
  if(mcrl2::core::detail::gsIsLinProcSpec(spec)) spec_type = "LPS";
  if(mcrl2::core::detail::gsIsPBES(spec)) spec_type = "PBES";

  data::data_specification data_spec(ATAgetArgument(spec, 0));
  spec = ATsetArgument(spec, atermpp::aterm((data::detail::data_specification_to_aterm_data_spec(data_spec))), 0);
  //open output file for writing or set to stdout
  FILE *output_stream    = NULL;
  if (tool_options.output_file_name.empty()) {
    output_stream = stdout;
    gsDebugMsg("output to stdout.\n");
  } else {
    output_stream = fopen(tool_options.output_file_name.c_str(), "w");
    if (output_stream == NULL) {
      std::string err_msg(strerror(errno));
      if (err_msg.length() > 0 && err_msg[err_msg.length()-1] == '\n') {
        err_msg.replace(err_msg.length()-1, 1, "");
      }
      throw mcrl2::runtime_error("could not open output file " + str_out + " for writing (" + err_msg + ")");
    }
    gsDebugMsg("output file %s is opened for writing.\n", str_out.c_str());
  }
  assert(output_stream != NULL);
  //print spec to output_stream
  gsVerboseMsg("printing %s from %s to %s in the %s format\n",
    spec_type.c_str(), str_in.c_str(), str_out.c_str(), pp_format_to_string(tool_options.format).c_str());
  //pretty print spec to output_stream
  PrintPart_C(output_stream, (ATerm) spec, tool_options.format);
  if (output_stream != stdout) {
    fclose(output_stream);
  }
}

std::string pp_format_to_string(t_pp_format pp_format)
{
  if (pp_format == ppDefault) {
    return "default";
  } else if (pp_format == ppDebug) {
    return "debug";
  } else if (pp_format == ppInternal) {
    return "internal";
  } else {
    throw mcrl2::runtime_error("unknown pretty print format");
  }
}
