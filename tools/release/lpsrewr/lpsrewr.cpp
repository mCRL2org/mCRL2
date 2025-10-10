// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lpsrewr.cpp
/// \brief Tool for rewriting a linear process specification.

#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/lps/io.h"
#include "mcrl2/lps/lps_rewriter_tool.h"
#include "mcrl2/lps/lps_rewriter_type.h"
#include "mcrl2/lps/one_point_rule_rewrite.h"
#include "mcrl2/lps/remove.h"
#include "mcrl2/lps/rewrite.h"
#include "mcrl2/lps/rewriters/dataspec_prune_rewriter.h"
#include "mcrl2/lps/rewriters/one_point_condition_rewrite.h"
#include "mcrl2/lps/stochastic_specification.h"
#include "mcrl2/utilities/input_output_tool.h"

using namespace mcrl2;
using namespace mcrl2::lps;
using namespace mcrl2::log;
using namespace mcrl2::utilities;
using mcrl2::utilities::tools::input_output_tool;
using mcrl2::data::tools::rewriter_tool;
using lps::tools::lps_rewriter_tool;

class lps_rewriter : public lps_rewriter_tool<rewriter_tool< input_output_tool > >
{
  protected:
    using super = lps_rewriter_tool<rewriter_tool<input_output_tool>>;

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

    bool run() override
    {
      using namespace utilities;

      mCRL2log(verbose) << "lpsrewr parameters:" << std::endl;
      mCRL2log(verbose) << "  input file:         " << m_input_filename << std::endl;
      mCRL2log(verbose) << "  output file:        " << m_output_filename << std::endl;
      mCRL2log(verbose) << "  lps rewriter:       " << m_lps_rewriter_type << std::endl;

      stochastic_specification spec;
      load_lps(spec, input_filename());
      switch (rewriter_type())
      {
        case simplify:
        {
          mcrl2::data::rewriter R(spec.data(), rewrite_strategy());
          lps::rewrite(spec, R);
          break;
        }
        case quantifier_one_point:
        {
          one_point_rule_rewrite(spec);
          break;
        }
        case condition_one_point:
        {
          mcrl2::data::rewriter R(spec.data(), rewrite_strategy());
          lps::one_point_condition_rewrite(spec, R);
          break;
        }
        case prune_dataspec:
        {
          dataspec_prune_rewriter<stochastic_specification> rewr;
          spec = rewr(spec);
          break;
        }
      }
      lps::remove_trivial_summands(spec);
      lps::remove_redundant_assignments(spec);
      save_lps(spec, output_filename());
      return true;
    }

};

int main(int argc, char* argv[])
{
  return lps_rewriter().execute(argc, argv);
}
