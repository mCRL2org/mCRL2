// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file transform.cpp

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/lps/generate_lts.h"
#include "mcrl2/lps/detail/lps_io.h"
#include "mcrl2/utilities/detail/io.h"
#include "mcrl2/utilities/detail/transform_tool.h"
#include "mcrl2/utilities/input_output_tool.h"

using namespace mcrl2;
using data::tools::rewriter_tool;
using utilities::detail::transform_tool;
using utilities::tools::input_output_tool;

class generatelts_tool: public rewriter_tool<input_output_tool>
{
  typedef rewriter_tool<input_output_tool> super;

  lps::generate_lts_options options;

  public:
    generatelts_tool()
      : super("generatelts",
              "Wieger Wesselink",
              "generates an LTS from an LPS",
              "Transforms the LPS in INFILE and writes a corresponding LTS in .aut format "
              " to OUTFILE. If OUTFILE is not present, stdout is used. If INFILE is not "
              " present, stdin is used."
             )
    {}

    void add_options(utilities::interface_description& desc) override
    {
      super::add_options(desc);
      desc.add_option("store-states-as-trees", "store states as trees instead of lists", 's');
      desc.add_option("one-point-rule-rewrite", "apply the one point rule rewriter");
      desc.add_option("replace-constants-by-variables", "move constant expressions to a substitution");
      desc.add_option("resolve-summand-variable-name-clashes", "resolve summand variable name clashes");
      options.rewrite_strategy = rewrite_strategy();
    }

    void parse_options(const utilities::command_line_parser& parser) override
    {
      super::parse_options(parser);
      options.store_states_as_trees                 = parser.options.find("store-states-as-trees") != parser.options.end();
      options.one_point_rule_rewrite                = parser.options.find("one-point-rule-rewrite") != parser.options.end();
      options.replace_constants_by_variables        = parser.options.find("replace-constants-by-variables") != parser.options.end();
      options.resolve_summand_variable_name_clashes = parser.options.find("resolve-summand-variable-name-clashes") != parser.options.end();
    }

    bool run() override
    {
      lps::labeled_transition_system lts;
      lps::specification lpsspec = lps::detail::load_lps(input_filename());

      if (options.store_states_as_trees)
      {
        lps::generate_labeled_transition_system<lps::state>(lpsspec, options, lts);
      }
      else
      {
        lps::generate_labeled_transition_system<data::data_expression_list>(lpsspec, options, lts);
      }

      std::ostringstream out;
      out << lts;
      utilities::detail::write_text(output_filename(), out.str());
      return true;
    }
};

int main(int argc, char* argv[])
{
  return generatelts_tool().execute(argc, argv);
}
