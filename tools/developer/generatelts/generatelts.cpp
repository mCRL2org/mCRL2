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
      desc.add_option("cached", "cache enumerator solutions");
      desc.add_option("confluence", "apply confluence reduction", 'c');
      desc.add_option("no-one-point-rule-rewrite", "do not apply the one point rule rewriter");
      desc.add_option("no-replace-constants-by-variables", "do not move constant expressions to a substitution");
      desc.add_option("no-resolve-summand-variable-name-clashes", "do not resolve summand variable name clashes");
      options.rewrite_strategy = rewrite_strategy();
    }

    void parse_options(const utilities::command_line_parser& parser) override
    {
      super::parse_options(parser);
      options.cached                                = parser.options.find("cached") != parser.options.end();
      options.confluence                            = parser.options.find("confluence") != parser.options.end();
      options.one_point_rule_rewrite                = parser.options.find("no-one-point-rule-rewrite") == parser.options.end();
      options.replace_constants_by_variables        = parser.options.find("no-replace-constants-by-variables") == parser.options.end();
      options.resolve_summand_variable_name_clashes = parser.options.find("no-resolve-summand-variable-name-clashes") == parser.options.end();
    }

    bool run() override
    {
      lps::labeled_transition_system lts;
      lps::specification lpsspec = lps::detail::load_lps(input_filename());
      lps::generate_labeled_transition_system(lpsspec, options, lts);
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
