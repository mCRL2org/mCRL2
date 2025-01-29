// Author(s): Jore Booy
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbesnucheck.cpp

#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/pbes/pbes_rewriter_tool.h"
#include "mcrl2/pbes/pbes_input_tool.h"
#include "mcrl2/pbes/pbes_output_tool.h"
#include "mcrl2/pbes/tools/pbesnucheck.h"

using namespace mcrl2;
using namespace mcrl2::log;
using namespace mcrl2::pbes_system;
using namespace mcrl2::core;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;
using pbes_system::tools::pbes_input_tool;
using pbes_system::tools::pbes_output_tool;
using pbes_system::tools::pbes_rewriter_tool;
using data::tools::rewriter_tool;

class pbesnucheck_tool: public pbes_input_tool<pbes_output_tool<pbes_rewriter_tool<rewriter_tool<input_output_tool>>>>
{
  protected:
    typedef pbes_input_tool<pbes_output_tool<pbes_rewriter_tool<rewriter_tool<input_output_tool>>>> super;

    pbesnucheck_options m_options;

    void parse_options(const command_line_parser& parser) override
    {
      super::parse_options(parser);
      m_options.check_global_invariant = parser.has_option("check-global-invariant");
    }

    void add_options(interface_description& desc) override
    {
      super::add_options(desc);
      desc.add_option("check-global-invariant",
                  "check in nu-formulae if the core constraint is a global invariant for each equation", 'g');
    }

  public:
    pbesnucheck_tool()
      : super(
        "pbesnucheck",
        "Jore Booy",
        "Simplify a pbes by checking if the base contraint is an invariant.",
        "Reads a file containing a PBES, and checks if the base contraint always holds to simplify it. If OUTFILE "
        "is not present, standard output is used. If INFILE is not present, standard input is used."
      )
    {}

    bool run() override
    {
      mCRL2log(verbose) << "pbesnucheck parameters:" << std::endl;
      mCRL2log(verbose) << "  input file:         " << m_input_filename << std::endl;
      mCRL2log(verbose) << "  output file:        " << m_output_filename << std::endl;

      m_options.rewrite_strategy = rewrite_strategy();
      pbesnucheck(input_filename(),
                  output_filename(),
                  pbes_input_format(),
                  pbes_output_format(),
                  m_options
                 );

      return true;
    }

};

int main(int argc, char* argv[])
{
  return pbesnucheck_tool().execute(argc, argv);
}
