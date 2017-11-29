// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbessymbolicexplore.cpp

#define TOOLNAME "pbessymbolicexplroe"
#define AUTHORS "Thomas Neele"

//C++
#include <exception>
#include <cstdio>

//LPS
#include "mcrl2/lps/io.h"

//Tool framework
#include "mcrl2/data/bool.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/parse.h"
#include "mcrl2/data/prover_tool.h"
#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/utilities/input_tool.h"

#include "forward_exploration.h"

using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::lps;
using mcrl2::data::tools::prover_tool;
using mcrl2::data::tools::rewriter_tool;
using mcrl2::utilities::tools::input_tool;
using namespace mcrl2::log;


class pbessymbolicexplore_tool: public rewriter_tool<input_tool>
{
  protected:
    typedef rewriter_tool<input_tool> super;

  public:
    pbessymbolicexplore_tool()
      : super(
        TOOLNAME,
        AUTHORS,
        "Output expressions for partition refinement",
        "Perform the first N steps of partition refinement on "
        "INFILE and output the resulting expressions. "
        "This tool is highly experimental. ")
    {}

    /// Runs the algorithm.
    /// Reads a specification from input_file,
    /// applies real time abstraction to it and writes the result to output_file.
    bool run()
    {
      mCRL2log(verbose) << "Parameters of pbessymbolicexplore:" << std::endl;
      mCRL2log(verbose) << "  input file:         " << m_input_filename << std::endl;
      mCRL2log(verbose) << "  data rewriter       " << m_rewrite_strategy << std::endl;

      stochastic_specification spec;
      load_lps(spec, input_filename());

      mcrl2::data::forward_exploration_algorithm<stochastic_specification>(spec, m_rewrite_strategy).run();

      return true;
    }

};

int main(int argc, char** argv)
{
  return pbessymbolicexplore_tool().execute(argc, argv);
}
