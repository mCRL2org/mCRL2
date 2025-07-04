// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file transform.cpp

#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/lps/detail/lps_command.h"
#include "mcrl2/lps/if_rewrite.h"
#include "mcrl2/lps/is_well_typed.h"
#include "mcrl2/lps/one_point_rule_rewrite.h"
#include "mcrl2/lps/rewrite.h"
#include "mcrl2/utilities/detail/io.h"
#include "mcrl2/utilities/detail/transform_tool.h"
#include "mcrl2/utilities/input_output_tool.h"

using namespace mcrl2;
using data::tools::rewriter_tool;
using utilities::detail::transform_tool;
using utilities::tools::input_output_tool;

struct one_point_rule_rewriter_command : public lps::detail::lps_command
{
  one_point_rule_rewriter_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : lps::detail::lps_command("lps-one-point-rule-rewriter", input_filename, output_filename, options)
  {}

  void execute() override
  {
    lps::detail::lps_command::execute();
    lps::one_point_rule_rewrite(lpsspec);
    lps::save_lps(lpsspec, output_filename);
  }
};

struct if_rewriter_command: public lps::detail::lps_command
{
  if_rewriter_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
      : lps::detail::lps_command("if-rewriter", input_filename, output_filename, options)
  {}

  void execute() override
  {
    lps::detail::lps_command::execute();
    lps::if_rewrite(lpsspec);
    lps::save_lps(lpsspec, output_filename);
  }
};

struct if_rewriter_with_rewriter_command: public lps::detail::lps_rewriter_command
{
  if_rewriter_with_rewriter_command(const std::string& input_filename,
                                    const std::string& output_filename,
                                    const std::vector<std::string>& options,
                                    data::rewrite_strategy strategy)
      : lps::detail::lps_rewriter_command("if-rewriter-with-rewriter",
                                          input_filename, output_filename,
                                          options, strategy)
  {}

  void execute() override
  {
    lps::detail::lps_command::execute();
    data::rewriter rewr(lpsspec.data());
    data::detail::if_rewrite_with_rewriter_builder f(rewr);
    lps::rewrite(lpsspec, [&](const data::data_expression& x)
    {
      data::data_expression result;
      f.apply(result, x);
      return result;
    });
    lps::save_lps(lpsspec, output_filename);
  }
};

struct is_well_typed_command: public lps::detail::lps_command
{
  is_well_typed_command(const std::string& input_filename, const std::string& output_filename, const std::vector<std::string>& options)
    : lps::detail::lps_command("is-well-typed", input_filename, output_filename, options)
  {}

  void execute() override
  {
    lps::detail::lps_command::execute();
    std::string result = lps::detail::is_well_typed(lpsspec) ? "true\n" : "false\n";
    utilities::detail::write_text(output_filename, result);
  }
};

class lpstransform_tool: public transform_tool<rewriter_tool<input_output_tool>>
{
  using super = transform_tool<rewriter_tool<input_output_tool>>;

public:
  lpstransform_tool()
      : super("lpstransform",
            "Wieger Wesselink",
            "applies a transformation to an LPS",
            "Transform the object in INFILE and write the result to OUTFILE. If OUTFILE "
            "is not present, stdout is used. If INFILE is not present, stdin is used.")
  {}

  void add_commands(const std::vector<std::string>& options) override
  {
    add_command(std::make_shared<one_point_rule_rewriter_command>(input_filename(), output_filename(), options));
    add_command(std::make_shared<if_rewriter_command>(input_filename(), output_filename(), options));
    add_command(std::make_shared<if_rewriter_with_rewriter_command>(input_filename(),
        output_filename(),
        options,
        rewrite_strategy()));
    add_command(std::make_shared<is_well_typed_command>(input_filename(), output_filename(), options));
    }
};

int main(int argc, char* argv[])
{
  return lpstransform_tool().execute(argc, argv);
}
