// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "combine.h"
#include "lts_combine.h"

#include "mcrl2/utilities/input_input_output_tool.h"

namespace mcrl2
{

using mcrl2::utilities::tools::input_input_output_tool;

class lpscombine_tool : public input_input_output_tool
{
  using super = input_input_output_tool;

public:
  lpscombine_tool() : super(
      "lpscombine",
      "Maurice Laveaux",
      "Combines LPSs created by lpscleave",
      "Combines two linear process specifications (LPS) obtained from lpscleave "
      "in INFILE1 and INFILE2 and writes the resulting mCRL2 specification to OUTFILE1. "
      "If INFILE2 is not present, stdin is used.")
  {}

  bool run() override
  {
    if (m_lts_mode)
    {
      lts::lts_lts_t left_lts;
      left_lts.load(input_filename1());

      lts::lts_lts_t right_lts;
      right_lts.load(input_filename2());

      if (output_filename().empty())
      {
        combine_lts(left_lts, right_lts, m_action_prefix, m_introduce_tags, std::cout);
      }
      else
      {
        std::ofstream file(output_filename(), std::ios_base::binary);
        combine_lts(left_lts, right_lts, m_action_prefix, m_introduce_tags, file);
      }
    }
    else
    {
      // Load the linear processes and combine them to an mcrl2 specification.
      lps::stochastic_specification left_spec;
      load_lps(left_spec, input_filename1());

      lps::stochastic_specification right_spec;

      if (input_filename2().empty())
      {
        load_lps(right_spec, std::cin);
      }
      else
      {
        load_lps(right_spec, input_filename2());
      }

      if (output_filename().empty())
      {
        combine_specification(left_spec, right_spec, m_action_prefix, std::cout);
      }
      else
      {
        std::ofstream file(output_filename());
        combine_specification(left_spec, right_spec, m_action_prefix, file);
      }
    }

    return true;
  }

protected:
  void add_options(utilities::interface_description& desc) override
  {
    super::add_options(desc);

    desc.add_option("prefix", utilities::make_mandatory_argument("PREFIX"), "Add a prefix to the synchronisation action labels to ensure that do not already occur in the specification", 'f');
    desc.add_option("lts", "Combine two labelled transition systems based on the composition semantics", 'l');
    desc.add_option("introduce-tags", "Introduce tag actions for every multi-action without synchronisation actions.", 't');
  }

  void parse_options(const utilities::command_line_parser& parser) override
  {
    super::parse_options(parser);

    if (parser.options.count("prefix"))
    {
      m_action_prefix = parser.option_argument_as< std::string >("prefix");
    }

    m_lts_mode = parser.options.count("lts") > 0;
    m_introduce_tags = parser.options.count("introduce-tags") > 0;

    if (m_introduce_tags && !m_lts_mode)
    {
        parser.error("The --introduce-tags option only works with --lts.");
    }
  }

private:
  std::string m_action_prefix;
  bool m_lts_mode = false;
  bool m_introduce_tags = false;
};

} // namespace mcrl2

int main(int argc, char** argv)
{
  return mcrl2::lpscombine_tool().execute(argc, argv);
}
