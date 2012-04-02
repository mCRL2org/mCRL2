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
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/trace/trace.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/mcrl2_gui_tool.h"
#include "mcrl2/exception.h"
#include "mcrl2/lps/multi_action.h"
#include "mcrl2/data/data_expression.h"

using namespace std;
using namespace mcrl2;
using namespace mcrl2::log;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;
using namespace mcrl2::core;
using namespace mcrl2::trace;

enum output_type { otPlain, otMcrl2, otDot, otAut, /*otSvc,*/ otNone, otStates };

static inline
std::string print_output_type(const output_type t)
{
  switch(t)
  {
    case otPlain: return "plain";
    case otMcrl2: return "mcrl2";
    case otDot: return "dot";
    case otAut: return "aut";
    case otNone: return "none";
    case otStates: return "states";
  }
  throw mcrl2::runtime_error("unknown trace format");
}

static inline
std::string description(const output_type t)
{
  switch(t)
  {
    case otPlain: return "plain text";
    case otMcrl2: return "the mCRL2 format";
    case otDot: return "the GraphViz format";
    case otAut: return "the Aldebaran format";
    case otNone: return "no output";
    case otStates: return "plain text with state vectors";
  }
  throw mcrl2::runtime_error("unknown trace format");
}

static inline
output_type parse_output_type(const std::string& s)
{
  if(s == "plain") return otPlain;
  else if(s == "mcrl2") return otMcrl2;
  else if(s == "dot") return otDot;
  else if(s == "aut") return otAut;
  else if(s == "none") return otNone;
  else if(s == "states") return otStates;
  else throw mcrl2::runtime_error("unknown trace format " + s);
}

static inline
std::istream& operator>>(std::istream& is, output_type& t)
{
  try
  {
    std::string s;
    is >> s;
    t = parse_output_type(s);
  }
  catch(mcrl2::runtime_error&)
  {
    is.setstate(std::ios_base::failbit);
  }
  return is;
}

static inline
std::ostream& operator<<(std::ostream& os, const output_type t)
{
  os << print_output_type(t);
  return os;
}

static void print_state(ostream& os, const mcrl2::lps::state &s)
{
  size_t arity = s.size();

  os << "(";
  for (size_t i=0; i<arity; i++)
  {
    if (i > 0)
    {
      os << ",";
    }
    os << mcrl2::data::pp(s[i]);
  }
  os << ")";
}

static void trace2dot(ostream& os, Trace& trace, const std::string& name)
{
  os << "digraph \"" << name << "\" {" << endl;
  os << "center = TRUE;" << endl;
  os << "mclimit = 10.0;" << endl;
  os << "nodesep = 0.05;" << endl;

  trace.resetPosition();

  os << 0 << " [label=\"";
  if (trace.current_state_exists())
  {
    print_state(os,trace.currentState());
  }
  os << "\",peripheries=2];" << endl;

  for(size_t i=0; i<trace.number_of_actions(); ++i, trace.increasePosition())
  {
    os << i+1 << " [label=\"";
    trace.increasePosition();
    if (trace.current_state_exists())
    {
      print_state(os,trace.currentState());
    }
    trace.decreasePosition();
    os << "\"];" << endl;
    os << i << " -> " << i+1 << " [label=\"";
    os << mcrl2::lps::pp(trace.currentAction());
    os << "\"];" << endl;
  }
  os << "}" << endl;
}

static void trace2statevector(ostream& os, Trace& trace)
{
  trace.resetPosition();

  for(size_t i=0; i<trace.number_of_actions(); ++i, trace.increasePosition())
  {
    if (trace.current_state_exists())
    {
      print_state(os,trace.currentState());
    }
    os << " -";
    os << mcrl2::lps::pp(trace.currentAction());
    os << "-> " << std::endl;
  }
  if (trace.current_state_exists())
  {
    print_state(os, trace.currentState());
  }
  os << std::endl;
}

static void trace2aut(ostream& os, Trace& trace)
{
  os << "des (0," << trace.number_of_actions() << "," << trace.number_of_actions()+1 << ")" << endl;
  trace.resetPosition();

  for(size_t i=0; i<trace.number_of_actions(); ++i, trace.increasePosition())
  {
    os << "(" << i << ",\"";
    os << mcrl2::lps::pp(trace.currentAction());
    os << "\"," << i+1 << ")" << endl;
  }
}

inline void save_trace(Trace& trace, output_type outtype, std::ostream& out, const std::string& name)
{
  mCRL2log(verbose) << "writing result in " << description(outtype) << "..." << std::endl;
  switch (outtype)
  {
    case otPlain:
      trace.save(out,tfPlain);
      break;
    case otMcrl2:
      trace.save(out,tfMcrl2);
      break;
    case otAut:
      trace2aut(out,trace);
      break;
    case otStates:
      trace2statevector(out,trace);
      break;
    case otDot:
      trace2dot(out, trace, name);
    default:
      break;
  }
}

class tracepp_tool: public input_output_tool
{
  protected:
    typedef input_output_tool super;

  public:
    tracepp_tool()
      : super(NAME, AUTHOR,
              "convert and pretty print traces",
              "Convert the trace in INFILE and save it in another format to OUTFILE. If OUTFILE"
              "is not present, stdout is used. If INFILE is not present, stdin is used.\n"
              "\n"
              "Input should be either in plain format, which means a text file with one action on each line, "
              "or the mCRL2 trace format (as generated by lps2lts, for example).\n"
             ),
      format_for_output(otPlain)
    {}

    bool run()
    {
      Trace trace;

      if (input_filename().empty())
      {
        mCRL2log(verbose) << "reading input from stdin..." << std::endl;

        trace.load(std::cin);
      }
      else
      {
        mCRL2log(verbose) << "reading input from '" <<  input_filename() << "'..." << std::endl;

        std::ifstream in(input_filename().c_str(), std::ios_base::binary|std::ios_base::in);

        if (in.good())
        {
          trace.load(in);
        }
        else
        {
          throw mcrl2::runtime_error("could not open input file '" +
                                     input_filename() + "' for reading");
        }
      }


      if (output_filename().empty())
      {
        mCRL2log(verbose) << "writing result to stdout..." << std::endl;
        save_trace(trace, format_for_output, std::cout,"stdin");
      }
      else
      {
        mCRL2log(verbose) << "writing result to '" <<  output_filename() << "'..." << std::endl;

        std::ofstream out(output_filename().c_str(), std::ios_base::binary|std::ios_base::out|std::ios_base::trunc);

        if (out.good())
        {
          save_trace(trace, format_for_output, out, input_filename().substr(input_filename().find_last_of('.')));
        }
        else
        {
          throw mcrl2::runtime_error("could not open output file '" +
                                     output_filename() +  "' for writing");
        }
      }
      return true;
    }

  protected:
    output_type     format_for_output;

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc.add_option("format", make_enum_argument<output_type>("FORMAT")
                      .add_value(otPlain, true)
                      .add_value(otStates)
                      .add_value(otMcrl2)
                      .add_value(otAut)
                      .add_value(otDot),
                      "print the trace in the specified FORMAT:", 'f');
    }

    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);
      parser.option_argument_as<output_type>("format");
    }
};

class tracepp_gui_tool: public mcrl2_gui_tool<tracepp_tool>
{
  public:
    tracepp_gui_tool()
    {
      std::vector<std::string> values;
      values.push_back("plain");
      values.push_back("states");
      values.push_back("mcrl2");
      values.push_back("aut");
      values.push_back("dot");
      m_gui_options["format"] = create_radiobox_widget(values);
    }
};


int main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)
  return tracepp_gui_tool().execute(argc, argv);
}
