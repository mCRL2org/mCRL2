// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsrewr.cpp
/// \brief Tool for rewriting a linear process specification.

#include "mcrl2/lps/tools.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/rewriter_tool.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2;
using namespace mcrl2::utilities;
using mcrl2::utilities::tools::input_output_tool;
using mcrl2::utilities::tools::rewriter_tool;

class lps_rewriter_tool : public rewriter_tool< input_output_tool >
{
  protected:
    typedef rewriter_tool< input_output_tool > super;

    bool          m_benchmark;
    unsigned long m_bench_times;

    void add_options(utilities::interface_description& desc)
    {
      super::add_options(desc);
      desc.add_hidden_option("benchmark", utilities::make_mandatory_argument("NUM"),
                             "rewrite data expressions NUM times; do not save output", 'b');
    }

    /// Parse the non-default options.
    void parse_options(const utilities::command_line_parser& parser)
    {
      super::parse_options(parser);
      m_benchmark = (parser.options.count("benchmark")>0);
      if (m_benchmark)
      {
        m_bench_times = parser.option_argument_as< unsigned long >("benchmark");
      }
    }

  public:
    lps_rewriter_tool()
      : super(
        "lpsrewr",
        "Wieger Wesselink and Muck van Weerdenburg",
        "rewrite data expressions in an LPS",
        "Rewrite data expressions of the LPS in INFILE and save the result to OUTFILE."
        "If OUTFILE is not present, standard output is used. If INFILE is not present,"
        "standard input is used"
      ),
      m_benchmark(false),
      m_bench_times(1)
    {}

    bool run()
    {
      lps::lpsrewr(input_filename(),
                   output_filename(),
                   rewrite_strategy(),
                   m_benchmark,
                   m_bench_times
                 );
      return true;
    }

};

int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return lps_rewriter_tool().execute(argc, argv);
}

