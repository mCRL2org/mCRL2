// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "cleave.h"

#include "mcrl2/data/parse.h"
#include "mcrl2/lps/io.h"
#include "mcrl2/utilities/input_output_output_tool.h"

#include "lpscleave_utility.h"

namespace mcrl2
{

using log::log_level_t;
using lps::stochastic_specification;
using mcrl2::utilities::tools::input_output_output_tool;

class lpscleave_tool : public input_output_output_tool
{
  typedef input_output_output_tool super;

public:
  lpscleave_tool() : super(
      "lpscleave",
      "Maurice Laveaux",
      "Cleaves LPSs",
      "Decomposes the data parameters of the linear process specification (LPS) "
      "in INFILE and write the result of the left component to OUTFILE1 and the right"
      "component to OUTFILE2. If INFILE is not present, stdin is used.")
  {}

  bool run() override
  {
    stochastic_specification spec;

    if (input_filename().empty())
    {
      load_lps(spec, std::cin);
    }
    else
    {
      load_lps(spec, input_filename());
    }

    if (m_parameters.empty())
    {
      // Print the parameters and exit
      mCRL2log(log::info) << "Process parameters: ";
      print_names(log::info, spec.process().process_parameters());
      mCRL2log(log::info) << "\n";


      mCRL2log(log::info) << "Number of summands: " << spec.process().summand_count() << "\n";
    }
    else
    {
      // Here, we should decide on a good cleaving, but for now the parameters are given by the user.
      auto parameters = project_parameters(spec.process().process_parameters(), m_parameters);

      // Cleave the process, requires the indices to be sorted.
      m_indices.sort();
      m_indices.unique();

      // Compute the projection V.
      auto duplicated = project_parameters(spec.process().process_parameters(), m_duplicated);
      auto left_parameters = parameters + duplicated;

      // Take the complement of the parameters for the projection W.
      parameters = get_other_parameters(spec.process(), parameters);
      auto right_parameters = parameters;

      // Ensure that parameters are unique.
      for (const data::variable& param : duplicated)
      {
        if (std::find(right_parameters.begin(), right_parameters.end(), param) == right_parameters.end())
        {
          right_parameters.push_front(param);
        }
      }

      mCRL2log(log::verbose) << "Left parameters: ";
      print_names(log::verbose, left_parameters);
      mCRL2log(log::verbose) << "\nRight parameters: ";
      print_names(log::verbose, right_parameters);
      mCRL2log(log::verbose) << "\n";

      // Load the invariant.
      data::data_expression invariant;

      if (!m_invariant_filename.empty())
      {
        std::ifstream instream(m_invariant_filename.c_str());

        if (!instream.is_open())
        {
          throw mcrl2::runtime_error("cannot open input file '" + m_invariant_filename + "'");
        }

        mCRL2log(log::verbose) << "parsing input file '" <<  m_invariant_filename << "'..." << std::endl;
        invariant = data::parse_data_expression(instream, spec.process().process_parameters(), spec.data());
      }

      // The resulting LPSs
      stochastic_specification left_spec, right_spec;
      std::tie(left_spec, right_spec) = cleave(spec, left_parameters, right_parameters, m_indices, invariant, m_split_condition, m_split_action, m_merge_heuristic);

      // Save the resulting components.
      lps::save_lps(left_spec, output_filename1());
      lps::save_lps(right_spec, output_filename2());
    }

    return true;
  }

protected:
  void add_options(utilities::interface_description& desc) override
  {
    super::add_options(desc);

    desc.add_option("parameters", utilities::make_mandatory_argument("PARAMS"), "A comma separated list of PARAMS that are used for the left process of the cleave.", 'p');
    desc.add_option("shared", utilities::make_mandatory_argument("PARAMS"), "A comma separated list of shared PARAMS that occur in both processes of the cleave.", 's');
    desc.add_option("summands", utilities::make_mandatory_argument("INDICES"), "A comma separated list of INDICES of summands where the left process generates the action.", 'l');
    desc.add_option("split-condition", "Enable heuristics to split the condition expression of each summand.", 'c');
    desc.add_option("split-action", "Enable heuristics to split the action expression of each summand, where the indices in INDICES are used as a fallback (if no optimal choice is available).", 'a');
    desc.add_option("merge-heuristic", "Enable heuristics to merge synchronization indices of summands.", 'm');
    desc.add_option("invariant", utilities::make_mandatory_argument("FILE"), "A file which contains a data expression which is used as an invariant to strengthened the conditions.", 'i');
  }

  void parse_options(const utilities::command_line_parser& parser) override
  {
    super::parse_options(parser);

    if (parser.options.count("parameters"))
    {
      m_parameters = split_actions(parser.option_argument("parameters"));
    }

    if (parser.options.count("shared"))
    {
      m_duplicated = split_actions(parser.option_argument("shared"));
    }

    if (parser.options.count("summands"))
    {
      std::list<std::string> indices = split_actions(parser.option_argument("summands"));
      for (auto& string : indices)
      {
        m_indices.emplace_back(std::stoul(string));
      }
    }

    if (parser.options.count("invariant"))
    {
      m_invariant_filename = parser.option_argument_as< std::string >("invariant");
    }

    m_split_condition = parser.options.count("split-condition") > 0;
    m_split_action = parser.options.count("split-action") > 0;
    m_merge_heuristic = parser.options.count("merge-heuristic") > 0;
  }

private:
  std::list<std::string> m_parameters;
  std::list<std::string> m_duplicated;
  std::list<std::size_t> m_indices;
  std::string m_invariant_filename;
  bool m_split_condition;
  bool m_split_action;
  bool m_merge_heuristic;
};

} // namespace mcrl2

int main(int argc, char** argv)
{
  return mcrl2::lpscleave_tool().execute(argc, argv);
}
