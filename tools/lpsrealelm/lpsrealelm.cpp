// Author(s): Jan Friso Groote, Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsrealelm.cpp

#define TOOLNAME "lpsrealelm"
#define AUTHORS "Jan Friso Groote and Jeroen Keiren"

#define DEFAULT_MAX_ITERATIONS 10

//C++
#include <exception>
#include <cstdio>

//Aterms
#include <mcrl2/atermpp/aterm.h>
#include <mcrl2/atermpp/aterm_list.h>
#include <mcrl2/atermpp/table.h>
#include <mcrl2/atermpp/algorithm.h>

//Tool framework
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/rewriter_tool.h"

#include "realelm.h"

using namespace mcrl2::utilities;
using namespace mcrl2::core;
using namespace mcrl2::lps;
using utilities::tools::rewriter_tool;
using utilities::tools::input_output_tool;
using namespace mcrl2::log;


class lpsrealelm_tool: public rewriter_tool<input_output_tool >
{
  protected:
    typedef rewriter_tool<input_output_tool> super;

    unsigned int max_iterations;

    /// Parse the non-default options.
    void parse_options(const command_line_parser& parser)
    {
      super::parse_options(parser);

      if (parser.options.count("max")>0)
      {
        max_iterations = parser.option_argument_as< unsigned int > ("max");
      }
    }

    void add_options(interface_description& desc)
    {
      super::add_options(desc);
      desc.
      add_option("max",
                 make_mandatory_argument("NUM"),
                 "perform at most NUM iterations");
    }

  public:
    lpsrealelm_tool()
      : super(
        TOOLNAME,
        AUTHORS,
        "remove real numbers from an LPS",
        "Remove Real numbers from the linear process specification (LPS) in "
        "INFILE and write the result to OUTFILE. If INFILE is not present, stdin is used. "),
      max_iterations(DEFAULT_MAX_ITERATIONS)
    {}

    /// Runs the algorithm.
    /// Reads a specification from input_file,
    ///i applies real time abstraction to it and writes the result to output_file.
    bool run()
    {
      mCRL2log(verbose) << "Parameters of lpsrealelm:" << std::endl;
      mCRL2log(verbose) << "  input file:         " << m_input_filename << std::endl;
      mCRL2log(verbose) << "  output file:        " << m_output_filename << std::endl;
      mCRL2log(verbose) << "  data rewriter       " << m_rewrite_strategy << std::endl;
      mCRL2log(verbose) << "  max_iterations:     " << max_iterations << std::endl;

      specification lps_specification;
      lps_specification.load(m_input_filename);

      // Untime lps_specification and save the output to a binary file
      rewriter r=create_rewriter(lps_specification.data());
      specification new_spec = realelm(lps_specification, max_iterations, r);

      mCRL2log(verbose) << "Real time abstraction completed, saving to " << m_output_filename << "\n";
      new_spec.save(m_output_filename);

      return true;
    }

};

int main(int argc, char** argv)
{
  return lpsrealelm_tool().execute(argc, argv);
}
