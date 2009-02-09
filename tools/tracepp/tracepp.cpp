// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file tracepp.cpp

#include "boost.hpp" // precompiled headers

#define NAME "tracepp"
#define AUTHOR "Muck van Weerdenburg"

#include <iostream>
#include <fstream>
#include <cassert>
#include <aterm2.h>
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/core/print.h"
#include "mcrl2/trace.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/utilities/command_line_interface.h"
#include "mcrl2/utilities/command_line_messaging.h"
#include "mcrl2/exception.h"

using namespace std;
using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::trace;

enum output_type { otPlain, otMcrl2, otDot, otAut, /*otSvc,*/ otNone };

static void print_state(ostream &os, ATermAppl state)
{
  int arity = ATgetArity(ATgetAFun(state));

  os << "(";
  for (int i=0; i<arity; i++)
  {
    if ( i > 0 )
    {
      os << ",";
    }
    PrintPart_CXX(os,ATgetArgument(state,i),ppDefault);
  }
  os << ")";
}

static void trace2dot(ostream &os, Trace &trace, char const* name)
{
  os << "digraph \"" << name << "\" {" << endl;
  os << "center = TRUE;" << endl;
  os << "mclimit = 10.0;" << endl;
  os << "nodesep = 0.05;" << endl;
  ATermAppl act;
  int i = 0;
  os << i << " [label=\"";
  if ( trace.currentState() != NULL )
  {
    print_state(os,trace.currentState());
  }
  os << "\",peripheries=2];" << endl;
  while ( (act = trace.nextAction()) != NULL )
  {
    os << i+1 << " [label=\"";
    if ( trace.currentState() != NULL )
    {
      print_state(os,trace.currentState());
    }
    os << "\"];" << endl;
    os << i << " -> " << i+1 << " [label=\"";
    if ( mcrl2::core::detail::gsIsMultAct(act) )
    {
      PrintPart_CXX(os,(ATerm) act,ppDefault);
    } else {
      // needed because trace library cannot parse strings
      os << ATgetName(ATgetAFun(act));
    }
    os << "\"];" << endl;
    i++;
  }
  os << "}" << endl;
}

static void trace2aut(ostream &os, Trace &trace)
{
  os << "des (0," << trace.getLength() << "," << trace.getLength()+1 << ")" << endl;
  ATermAppl act;
  int i = 0;
  while ( (act = trace.nextAction()) != NULL )
  {
    os << "(" << i << ",\"";
    if ( mcrl2::core::detail::gsIsMultAct(act) )
    {
      PrintPart_CXX(os,(ATerm) act,ppDefault);
    } else {
      // needed because trace library cannot parse strings
      os << ATgetName(ATgetAFun(act));
    }
    i++;
    os << "\"," << i << ")" << endl;
  }
}

/*static void trace2svc(ostream &os, Trace &trace)
{
  // SVC library does not accept ostreams
}*/

struct t_tool_options {
  std::string     name_for_input;
  std::string     name_for_output;
  output_type     format_for_output;
};

inline void save_trace(Trace& trace, output_type outtype, std::ostream& out) {
  switch ( outtype )
  {
    case otPlain:
      gsVerboseMsg("writing result in plain text...\n");
      trace.save(out,tfPlain);
      break;
    case otMcrl2:
      gsVerboseMsg("writing result in mCRL2 trace format...\n");
      trace.save(out,tfMcrl2);
      break;
    case otAut:
      gsVerboseMsg("writing result in aut format...\n");
      trace2aut(out,trace);
      break;
/*      gsVerboseMsg("writing result in svc format...\n");
      case otSvc:
      trace2svc(*OutStream,trace);
      break;*/
    default:
      break;
  }
}

void process(t_tool_options const& tool_options) {
  Trace trace;

  if (tool_options.name_for_input.empty()) {
    gsVerboseMsg("reading input from stdin...\n");

    trace.load(std::cin);
  }
  else {
    gsVerboseMsg("reading input from '%s'...\n", tool_options.name_for_input.c_str());

    std::ifstream in(tool_options.name_for_input.c_str(), std::ios_base::binary|std::ios_base::in);

    if (in.good()) {
      trace.load(in);
    }
    else {
      throw mcrl2::runtime_error("could not open input file '" +
          tool_options.name_for_input + "' for reading");
    }
  }

  if (tool_options.name_for_output.empty()) {
    gsVerboseMsg("writing result to stdout...\n");

    if (tool_options.format_for_output == otDot) {
      gsVerboseMsg("writing result in dot format...\n");

      trace2dot(std::cout,trace,"stdin");
    }
    else {
      save_trace(trace, tool_options.format_for_output, std::cout);
    }
  }
  else {
    gsVerboseMsg("writing result to '%s'...\n", tool_options.name_for_output.c_str());

    std::ofstream out(tool_options.name_for_output.c_str(), std::ios_base::binary|std::ios_base::out|std::ios_base::trunc);

    if (out.good()) {
      if (tool_options.format_for_output == otDot) {
        gsVerboseMsg("writing result in dot format...\n");

        trace2dot(out,trace,
            tool_options.name_for_input.substr(tool_options.name_for_input.find_last_of('.')).c_str());
      }
      else {
        save_trace(trace, tool_options.format_for_output, out);
      }
    }
    else {
      throw mcrl2::runtime_error("could not open output file '" +
                  tool_options.name_for_output +  "' for writing");
    }
  }
}

bool parse_command_line(int ac, char** av, t_tool_options& tool_options) {
  interface_description clinterface(av[0], NAME, AUTHOR, "[OPTION]... [[INFILE] OUTFILE]\n",
    "Convert the trace in INFILE and save it in another format to OUTFILE. If OUTFILE"
    "is not present, stdout is used. If INFILE is not present, stdin is used.\n"
    "\n"
    "Input should be either in plain format, which means a text file with one action on each line, "
    "or the mCRL2 trace format (as generated by lps2lts, for example).\n");

  clinterface.add_option("format", make_mandatory_argument("FORMAT"),
    "print the trace in the specified FORMAT:\n"
    "  'plain' for plain text (default),\n"
    "  'mcrl2' for the mCRL2 format,\n"
    "  'aut' for the Aldebaran format, or\n"
    "  'dot' for the GraphViz format"
    , 'f');

  command_line_parser parser(clinterface, ac, av);

  if (parser.continue_execution()) {
    tool_options.format_for_output = otPlain;

    if (parser.options.count("format")) {
      std::string eq_name(parser.option_argument("format"));
      if (eq_name == "plain") {
        tool_options.format_for_output = otPlain;
      } else if (eq_name == "mcrl2") {
        tool_options.format_for_output = otMcrl2;
      } else if (eq_name == "dot") {
        tool_options.format_for_output = otDot;
      } else if (eq_name == "aut") {
        tool_options.format_for_output = otAut;
      } else {
        parser.error("option -f/--format has illegal argument '" + eq_name + "'");
      }
    }

    if (2 < parser.arguments.size()) {
      parser.error("too many file arguments");
    }
    if (0 < parser.arguments.size()) {
      tool_options.name_for_input = parser.arguments[0];
    }
    if (1 < parser.arguments.size()) {
      tool_options.name_for_output = parser.arguments[1];
    }
  }

  return parser.continue_execution();
}

int main(int argc, char **argv)
{
  MCRL2_ATERM_INIT(argc, argv)

  try {
    t_tool_options options;

    if (parse_command_line(argc, argv, options)) {
       process(options);
    }
  }
  catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
