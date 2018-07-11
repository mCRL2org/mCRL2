// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsrewr.cpp
/// \brief Tool for rewriting a linear process specification.

#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/lps/lps_rewriter_tool.h"
#include "mcrl2/lps/tools.h"

using namespace mcrl2;
using namespace mcrl2::log;
using namespace mcrl2::utilities;
using mcrl2::utilities::tools::input_output_tool;
using mcrl2::data::tools::rewriter_tool;
using lps::tools::lps_rewriter_tool;

class lps_rewriter : public lps_rewriter_tool<rewriter_tool< input_output_tool > >
{
  protected:
    typedef lps_rewriter_tool<rewriter_tool< input_output_tool > > super;

    void add_options(utilities::interface_description& desc)
    {
      super::add_options(desc);
    }

    /// Parse the non-default options.
    void parse_options(const utilities::command_line_parser& parser)
    {
      super::parse_options(parser);
    }

  public:
    lps_rewriter()
      : super(
        "lpsrewr",
        "Wieger Wesselink and Muck van Weerdenburg",
        "rewrite data expressions in an LPS",
        "Rewrite data expressions of the LPS in INFILE and save the result to OUTFILE."
        "If OUTFILE is not present, standard output is used. If INFILE is not present,"
        "standard input is used"
      )
    {}

    bool run()
    {
      using namespace utilities;

      mCRL2log(verbose) << "lpsrewr parameters:" << std::endl;
      mCRL2log(verbose) << "  input file:         " << m_input_filename << std::endl;
      mCRL2log(verbose) << "  output file:        " << m_output_filename << std::endl;
      mCRL2log(verbose) << "  lps rewriter:       " << m_lps_rewriter_type << std::endl;


      lps::lpsrewr(input_filename(),
                   output_filename(),
                   rewrite_strategy(),
                   rewriter_type()
                 );
      return true;
    }

};

int main(int argc, char* argv[])
{
  return lps_rewriter().execute(argc, argv);
}

