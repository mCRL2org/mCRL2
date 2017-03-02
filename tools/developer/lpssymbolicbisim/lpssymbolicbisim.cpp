// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpssymbolicbisim.cpp

#define TOOLNAME "lpssymbolicbisim"
#define AUTHORS "Thomas Neele"

//C++
#include <exception>
#include <cstdio>

//LPS
#include "mcrl2/lps/io.h"

//Tool framework
#include "mcrl2/utilities/input_tool.h"
#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/bool.h"

#include "symbolic_bisim.h"

using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::lps;
using mcrl2::data::tools::rewriter_tool;
using mcrl2::utilities::tools::input_tool;
using namespace mcrl2::log;


class lpssymbolicbisim_tool: public rewriter_tool<input_tool>
{
  protected:
    typedef rewriter_tool<input_tool> super;

    std::string invariant;

    /// Parse the non-default options.
    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);

      if (parser.options.count("invariant")>0)
      {
        invariant = parser.option_argument("invariant");
      }
    }

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc.
      add_option("invariant",
                 make_mandatory_argument("expr"),
                 "add invariant that makes up the only block in the initial partition",
                 'i');
    }

  public:
    lpssymbolicbisim_tool()
      : super(
        TOOLNAME,
        AUTHORS,
        "Output expressions for partition refinement",
        "Perform the first N steps of partition refinement on "
        "INFILE and output the resulting expressions. "
        "This tool is highly experimental. "),
        invariant("true")
    {}

    /// Runs the algorithm.
    /// Reads a specification from input_file,
    /// applies real time abstraction to it and writes the result to output_file.
    bool run()
    {
      mCRL2log(verbose) << "Parameters of lpssymbolicbisim:" << std::endl;
      mCRL2log(verbose) << "  input file:         " << m_input_filename << std::endl;
      mCRL2log(verbose) << "  data rewriter       " << m_rewrite_strategy << std::endl;

      stochastic_specification spec;
      load_lps(spec, input_filename());

      mcrl2::data::data_expression inv = parse_data_expression(invariant, spec.process().process_parameters(), spec.data());

      mcrl2::data::symbolic_bisim_algorithm<stochastic_specification>(spec, inv, m_rewrite_strategy).run();

      return true;
    }

};

int main(int argc, char** argv)
{
  return lpssymbolicbisim_tool().execute(argc, argv);
}
