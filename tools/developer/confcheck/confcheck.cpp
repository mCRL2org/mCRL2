// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file confcheck.cpp

#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/lps/confluence.h"
#include "mcrl2/lps/io.h"
#include "mcrl2/lps/is_stochastic.h"
#include "mcrl2/utilities/input_output_tool.h"

using namespace mcrl2;
using data::tools::rewriter_tool;
using utilities::tools::input_output_tool;

class confcheck_tool: public rewriter_tool<input_output_tool>
{
  using super = rewriter_tool<input_output_tool>;

protected:
  /// \brief Confluence types for which the tool should check.
  std::string m_conditions;
  bool m_use_smt_solver = false;

public:
  confcheck_tool()
      : super("confcheck",
            "Wieger Wesselink",
            "checks confluence on an LPS",
            "Transforms the LPS in INFILE and writes the result to OUTFILE."
            " If OUTFILE is not present, stdout is used. If INFILE is not present, stdin is used."
            " Confluent tau actions are replaced by ctau.")
  {}

  void add_options(utilities::interface_description& desc) override
  {
    super::add_options(desc);
    desc.add_option("conditions",
        utilities::make_optional_argument("CONDITIONS", "C"),
        "use the confluence conditions specified by individual characters ("
        "Q: commutative confluence; "
        "C: square confluence; "
        "T: triangular confluence; "
        "Z: trivial confluence)",
        'x');
    desc.add_option("use-smt-solver", "Use the SMT solver Z3 (must be in the path)", 's');
    }

    void parse_options(const utilities::command_line_parser& parser) override
    {
      super::parse_options(parser);
      if (parser.has_option("conditions"))
      {
        m_conditions = parser.option_argument_as<std::string>("conditions");
      }
      m_use_smt_solver = parser.has_option("use-smt-solver");
    }

    bool run() override
    {
      lps::stochastic_specification lpsspec;
      lps::load_lps(lpsspec, input_filename());
      lps::confluence_checker checker;
      checker.run(lpsspec, m_conditions[0], m_use_smt_solver);
      lps::save_lps(lpsspec, output_filename());
      return true;
    }
};

int main(int argc, char* argv[])
{
  return confcheck_tool().execute(argc, argv);
}
