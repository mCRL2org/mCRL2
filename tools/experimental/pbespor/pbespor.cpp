// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbespor.cpp

// #define MCRL2_PBES_CONSTELM_DEBUG
// #define MCRL2_PBES_EXPRESSION_BUILDER_DEBUG

#include <iostream>
#include <string>
#include "mcrl2/bes/pbes_rewriter_tool.h"
#include "mcrl2/bes/pbes_input_tool.h"
#include "mcrl2/bes/pbes_output_tool.h"
#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/pbes/tools/pbespor.h"
#include "mcrl2/utilities/input_output_tool.h"

using namespace mcrl2;
using namespace mcrl2::log;
using namespace mcrl2::pbes_system;
using namespace mcrl2::core;
using namespace mcrl2::utilities;
using namespace mcrl2::utilities::tools;
using bes::tools::pbes_input_tool;
using bes::tools::pbes_output_tool;
using bes::tools::pbes_rewriter_tool;
using data::tools::rewriter_tool;

class pbespor_tool: public pbes_input_tool<pbes_output_tool<pbes_rewriter_tool<rewriter_tool<input_output_tool>>>>
{
  protected:
    typedef pbes_input_tool<pbes_output_tool<pbes_rewriter_tool<rewriter_tool<input_output_tool>>>> super;

    void parse_options(const command_line_parser& parser) override
    {
      super::parse_options(parser);
    }

    void add_options(interface_description& desc) override
    {
      super::add_options(desc);
    }

  public:
    pbespor_tool()
      : super(
        "pbespor",
        "Wieger Wesselink; Thomas Neele and Tim Willemse",
        "apply partial order reduction to a PBES",
        "Reads a file containing a PBES, and applies partial order reduction to it. If OUTFILE "
        "is not present, standard output is used. If INFILE is not present, standard input is used."
      )
    {}

    bool run() override
    {
      mCRL2log(verbose) << "pbespor parameters:" << std::endl;
      mCRL2log(verbose) << "  input file:         " << m_input_filename << std::endl;
      mCRL2log(verbose) << "  output file:        " << m_output_filename << std::endl;

      pbespor(input_filename(),
              output_filename(),
              pbes_input_format(),
              pbes_output_format(),
              rewrite_strategy()
             );

      return true;
    }

};

int main(int argc, char* argv[])
{
  return pbespor_tool().execute(argc, argv);
}
