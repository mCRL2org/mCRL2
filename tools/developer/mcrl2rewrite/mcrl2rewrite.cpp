// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/data/parse.h"
#include "mcrl2/data/rewriter_tool.h"
#include "mcrl2/utilities/input_input_tool.h"

using namespace mcrl2;
using namespace mcrl2::data::tools;
using namespace mcrl2::utilities::tools;

class mcrl2rewriter_tool: public rewriter_tool<input_input_tool>
{
  public:
    /// Constructor.
    mcrl2rewriter_tool()
      : rewriter_tool<input_input_tool>(
        "mcrl2rewrite",
        "Maurice Laveaux",
        "A small utility tool to rewrite data expressions",
        "Accepts a data specification as first argument and a file containing one data expression per line as second."
      )
    {}

    bool run() override;
};

bool mcrl2rewriter_tool::run()
{
  // Parse the given data specification.
  std::string text = utilities::read_text(input_filename1());

  try
  {
    // Load the data specification and create a suitable rewriter for it.
    data::data_specification data_specification = data::parse_data_specification(text);

    // Strip out all the system defined constructors, mappings and sorts. This doesn't seem to change the performance
    // as the unused rewrite rules are removed somewhere else.
    /*
    std::set<data::sort_expression> sorts;
    std::set<data::function_symbol> constructors;
    std::set<data::function_symbol> mappings;
    data_specification.get_system_defined_sorts_constructors_and_mappings(sorts, constructors, mappings);
    for (auto& sort : sorts)
    {
      data_specification.remove_sort(sort);
    }

    for (auto& constructor : constructors)
    {
      data_specification.remove_constructor(constructor);
    }

    for (auto& mapping : mappings)
    {
      data_specification.remove_mapping(mapping);
    }
    */
    data::rewriter rewriter = create_rewriter(data_specification);

    // Read and parse the data expressions to a vector.
    std::ifstream expressions_file(input_filename2());
    std::string string;

    std::chrono::duration<long, std::nano> parse_duration = {};
    std::vector<data::data_expression> expressions;
    while(std::getline(expressions_file, string))
    {
      // Parse the data expression from the given file.
      auto current_time = std::chrono::high_resolution_clock::now();
      expressions.push_back(data::parse_data_expression(string, data_specification));
      parse_duration += (std::chrono::high_resolution_clock::now() - current_time);
    }

    if (m_timing_enabled)
    {
      std::cerr << "parsing: " << std::chrono::duration_cast<std::chrono::milliseconds>(parse_duration).count() << " milliseconds.\n";
    }

    // Rewrite the given data expressions and measure the rewrite and print times.
    std::chrono::duration<long, std::nano> rewrite_duration = {};
    std::chrono::duration<long, std::nano> print_duration = {};
    for (auto&& expression : expressions)
    {
      // Rewrite the data expression and measure the time.
      auto current_time = std::chrono::high_resolution_clock::now();
      auto result = rewriter(expression);
      rewrite_duration += (std::chrono::high_resolution_clock::now() - current_time);

      // Print the output and measure the time.
      current_time = std::chrono::high_resolution_clock::now();
      std::cout << result << "\n";
      print_duration += (std::chrono::high_resolution_clock::now() - current_time);
    }

    // Print the timing results.
    if (m_timing_enabled)
    {
      std::cerr << "rewriting: " << std::chrono::duration_cast<std::chrono::milliseconds>(rewrite_duration).count() << " milliseconds.\n";
      std::cerr << "printing: " << std::chrono::duration_cast<std::chrono::milliseconds>(print_duration).count() << " milliseconds.\n";
    }
  }
  catch (const std::exception& ex)
  {
    mCRL2log(log::error) << ex.what();
  }

  return true;
}

int main(int argc, char* argv[])
{
  return mcrl2rewriter_tool().execute(argc, argv);
}
