// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file tbf2lps.cpp

#include "boost.hpp" // precompiled headers

#define NAME "tbf2lps"
#define AUTHOR "Muck van Weerdenburg"

#include <cstdio>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <aterm2.h>
#include "mcrl2/core/detail/struct.h"
#include "lpstrans.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/utilities/command_line_interface.h"
#include "mcrl2/utilities/command_line_messaging.h"
#include "mcrl2/exception.h"

using namespace mcrl2::utilities;
using namespace mcrl2::core;

struct tool_options_type {
  bool            convert_funcs;
  bool            convert_bools;
  std::string     infilename;
  std::string     outfilename;
};

bool parse_command_line(int ac, char** av, tool_options_type& options) {
  interface_description clinterface(av[0], NAME, AUTHOR, "[OPTION]... [INFILE [OUTFILE]]\n",
    "Read mCRL LPS from INFILE, convert it to a mCRL2 LPS and save the result to "
    "OUTFILE. If OUTFILE is not present, stdout is used. If INFILE is not present, "
    "stdin is used. To use stdin and save the output to a file, use '-' for INFILE.\n"
    "\n"
    "This conversion assumes that Bool is the boolean sort with "
    "constructors T and F. Additionally, the following "
    "conversions on the data specification will be applied:\n"
    "  constructors T, F: -> Bool are replaced by true and false,\n"
    "  mapping and: Bool # Bool -> Bool is replaced by ||, and\n"
    "  mapping eq: S # S -> Bool is replaced by == for each sort S");

  clinterface.
    add_option("no-conv-map",
      "do not apply the conversion of mappings and and eq", 'n');

  command_line_parser parser(clinterface, ac, av);

  if (parser.continue_execution()) {
    options.convert_bools = true;
    options.convert_funcs = options.convert_bools && parser.options.count("no-conv-map") == 0;

    if (2 < parser.arguments.size()) {
      parser.error("too many file arguments");
    }
    else {
      if (0 < parser.arguments.size()) {
        options.infilename = parser.arguments[0];
      }
      if (1 < parser.arguments.size()) {
        options.outfilename = parser.arguments[1];
      }
    }
  }

  return parser.continue_execution();
}

int main(int argc, char **argv)
{
  MCRL2_ATERM_INIT(argc, argv)

  try {
    tool_options_type options;

    if (parse_command_line(argc, argv, options)) {

      ATermAppl mcrl_spec;

      if (options.infilename.empty()) {
        gsVerboseMsg("reading mCRL LPS from stdin...\n");

        mcrl_spec = (ATermAppl) ATreadFromFile(stdin);

        if (mcrl_spec == 0) {
          throw mcrl2::runtime_error("could not read mCRL LPS from '" + options.infilename + "'");
        }
        if (!is_mCRL_spec(mcrl_spec)) {
          throw mcrl2::runtime_error("stdin does not contain an mCRL LPS");
        }
      }
      else {
        gsVerboseMsg("reading mCRL LPS from '%s'...\n", options.infilename.c_str());

        FILE *in_stream = fopen(options.infilename.c_str(), "rb");

        if (in_stream == 0) {
          throw mcrl2::runtime_error("could not open input file '" + options.infilename + "' for reading");
        }

        mcrl_spec = (ATermAppl) ATreadFromFile(in_stream);

        fclose(in_stream);

        if (mcrl_spec == 0) {
          throw mcrl2::runtime_error("could not read mCRL LPS from '" + options.infilename + "'");
        }
        if (!is_mCRL_spec(mcrl_spec)) {
          throw mcrl2::runtime_error("'" + options.infilename + "' does not contain an mCRL LPS");
        }
      }

      ATprotectAppl(&mcrl_spec);
      assert(is_mCRL_spec(mcrl_spec));

      ATermAppl spec = translate(mcrl_spec,options.convert_bools,options.convert_funcs);
      ATprotectAppl(&spec);

      if (options.outfilename.empty()) {
        gsVerboseMsg("writing mCRL2 LPS to stdout...\n");

        ATwriteToSAFFile((ATerm) spec, stdout);
      }
      else {
        gsVerboseMsg("writing mCRL2 LPS to '%s'...\n", options.outfilename.c_str());

        FILE *outstream = fopen(options.outfilename.c_str(), "wb");

        if (outstream == NULL) {
          throw mcrl2::runtime_error("cannot open output file '" + options.outfilename + "'");
        }

        ATwriteToSAFFile((ATerm) spec,outstream);

        fclose(outstream);
      }
    }
  }
  catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
