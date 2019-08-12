// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/lps/io.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/input_output_tool.h"

#include "dependency_cleave.h"
#include "lpscleave_utility.h"

#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <vector>

namespace mcrl2
{

using log::log_level_t;
using utilities::tools::input_output_tool;
using lps::stochastic_specification;

class lpscleave_tool : public input_output_tool
{
    typedef input_output_tool super;

  public:
    lpscleave_tool() : super(
        "lpscleave",
        "Maurice Laveaux",
        "Cleaves LPSs",
        "")
    {}

    bool run() override
    {
      stochastic_specification spec;

      if (input_filename() == "-")
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
        print_names("Parameters", spec.process().process_parameters());
        mCRL2log(log::info) << "Number of summands: " << spec.process().summand_count() << "\n";

      }
      else
      {
        // Here, we should decide on a good cleaving.

        // For now, the parameters are given by the user.
        auto parameters = project_parameters(spec.process().process_parameters(), m_parameters);

        print_elements("Local indices", compute_indices(spec, parameters));

        m_indices.sort();
        m_indices.unique();

        if (m_right_process)
        {
          // Take the complement of the indices.
          m_indices = get_other_indices(spec.process(), m_indices);

          // Take the complement of the parameters
          parameters = get_other_parameters(spec.process(), parameters);
        }

        // Cleave the process, requires the indices to be sorted.
        stochastic_specification left_cleave = dependency_cleave(spec, parameters, m_indices, m_right_process);

        // Save the resulting left-cleave.
        if (output_filename().empty())
        {
          left_cleave.save(std::cout, true);
        }
        else
        {
          std::ofstream file(output_filename(), std::ios::binary);
          left_cleave.save(file, true);
        }
      }

      return true;
    }

  protected:
    void add_options(utilities::interface_description& desc) override
    {
      super::add_options(desc);

      desc.add_option("parameters", utilities::make_mandatory_argument("PARAMS"), "A comma separated list of PARAMS that are used for the left process of the cleave.", 'p');
      desc.add_option("summands", utilities::make_mandatory_argument("INDICES"), "A comma separated list of INDICES of summands where the left process generates the action.", 's');
      desc.add_option("right", "The output is the right component of the cleave and the left component otherwise", 'r');
    }

    void parse_options(const utilities::command_line_parser& parser) override
    {
      super::parse_options(parser);

      if (parser.options.count("parameters"))
      {
        m_parameters = split_actions(parser.option_argument("parameters"));
      }

      if (parser.options.count("summands"))
      {
        std::list<std::string> indices = split_actions(parser.option_argument("summands"));
        for (auto& string : indices)
        {
          m_indices.emplace_back(std::stoul(string));
        }
      }

      if (parser.options.count("right"))
      {
        m_right_process = true;
      }
    }

  private:

    std::list<std::string> m_parameters;
    std::list<std::size_t> m_indices;
    bool m_right_process = false;
};

} // namespace mcrl2

int main(int argc, char** argv)
{
  return mcrl2::lpscleave_tool().execute(argc, argv);
}
