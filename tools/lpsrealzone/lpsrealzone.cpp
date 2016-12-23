// Author(s): Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsrealzone.cpp

#define TOOLNAME "lpsrealzone"
#define AUTHORS "Thomas Neele"

//C++
#include <exception>
#include <cstdio>

//LPS
#include "mcrl2/lps/io.h"

//Tool framework
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/data/rewriter_tool.h"

#include "realzone.h"

using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::lps;
using mcrl2::data::tools::rewriter_tool;
using mcrl2::utilities::tools::input_output_tool;
using namespace mcrl2::log;


class lpsrealzone_tool: public rewriter_tool<input_output_tool>
{
  protected:
    typedef rewriter_tool<input_output_tool> super;

  public:
    lpsrealzone_tool()
      : super(
        TOOLNAME,
        AUTHORS,
        "remove real numbers from an LPS",
        "Remove Real numbers from the linear process specification (LPS) in "
        "INFILE and write the result to OUTFILE. If INFILE is not present, stdin is used. "
        "In the output, the real numbers are replaced by a difference bound matrix. "
        "Therefore, there are certain restrictions on the linear inequalities present in the input. "
        "This tool is highly experimental. ")
    {}

    /// Runs the algorithm.
    /// Reads a specification from input_file,
    /// applies real time abstraction to it and writes the result to output_file.
    bool run()
    {
      mCRL2log(verbose) << "Parameters of lpsrealzone:" << std::endl;
      mCRL2log(verbose) << "  input file:         " << m_input_filename << std::endl;
      mCRL2log(verbose) << "  output file:        " << m_output_filename << std::endl;
      mCRL2log(verbose) << "  data rewriter       " << m_rewrite_strategy << std::endl;

      stochastic_specification spec;
      load_lps(spec, input_filename());

      // Translate spec and save the output to a binary file
      mcrl2::data::realzone_algorithm<stochastic_specification>(spec, m_rewrite_strategy).run();

      mCRL2log(verbose) << "Real time abstraction completed, saving to " << m_output_filename << "\n";
      save_lps(spec, output_filename());

      return true;
    }

};

int main(int argc, char** argv)
{
  return lpsrealzone_tool().execute(argc, argv);
}
