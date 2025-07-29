// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file reduce_process.cpp

#define NAME "reduce_process"
#define AUTHOR "Wieger Wesselink"

#include "mcrl2/process/is_well_typed.h"
#include "mcrl2/process/parse.h"
#include "mcrl2/process/replace_subterm.h"
#include "mcrl2/utilities/detail/io.h"
#include "mcrl2/utilities/input_output_tool.h"

using namespace mcrl2;

/// \brief Loads a process specification from input_filename, or from stdin if filename equals "".
inline
process::process_specification parse_process_specification(const std::string& input_filename)
{
  process::process_specification result;
  if (input_filename.empty())
  {
    return process::parse_process_specification(std::cin);
  }
  else
  {
    std::ifstream from(input_filename, std::ifstream::in | std::ifstream::binary);
    return process::parse_process_specification(from);
  }
  return result;
}

void generate_reduced_processes(const process::process_specification& p, std::size_t depth, const std::string& input_filename)
{
  std::vector<std::size_t> counts = process::detail::position_counts(p);
  if (depth >= counts.size())
  {
    return;
  }
  std::size_t n = counts[depth];
  for (std::size_t x = 0; x < n; x++)
  {
    process::process_expression expr = process::find_subterm(p, x, depth);
    std::set<process::process_expression> replacements;

    if (process::is_sum                (expr)) { replacements.insert(atermpp::down_cast<process::sum                >(expr).operand()); }
    if (process::is_stochastic_operator(expr)) { replacements.insert(atermpp::down_cast<process::stochastic_operator>(expr).operand()); }
    if (process::is_block              (expr)) { replacements.insert(atermpp::down_cast<process::block              >(expr).operand()); }
    if (process::is_hide               (expr)) { replacements.insert(atermpp::down_cast<process::hide               >(expr).operand()); }
    if (process::is_rename             (expr)) { replacements.insert(atermpp::down_cast<process::rename             >(expr).operand()); }
    if (process::is_comm               (expr)) { replacements.insert(atermpp::down_cast<process::comm               >(expr).operand()); }
    if (process::is_allow              (expr)) { replacements.insert(atermpp::down_cast<process::allow              >(expr).operand()); }
    if (process::is_at                 (expr)) { replacements.insert(atermpp::down_cast<process::at                 >(expr).operand()); }
    if (process::is_sync               (expr)) { replacements.insert(atermpp::down_cast<process::sync               >(expr).left()); replacements.insert(atermpp::down_cast<process::sync        >(expr).right()); }
    if (process::is_seq                (expr)) { replacements.insert(atermpp::down_cast<process::seq                >(expr).left()); replacements.insert(atermpp::down_cast<process::seq         >(expr).right()); }
    if (process::is_bounded_init       (expr)) { replacements.insert(atermpp::down_cast<process::bounded_init       >(expr).left()); replacements.insert(atermpp::down_cast<process::bounded_init>(expr).right()); }
    if (process::is_choice             (expr)) { replacements.insert(atermpp::down_cast<process::choice             >(expr).left()); replacements.insert(atermpp::down_cast<process::choice      >(expr).right()); }
    if (process::is_merge              (expr)) { replacements.insert(atermpp::down_cast<process::merge              >(expr).left()); replacements.insert(atermpp::down_cast<process::merge       >(expr).right()); }
    if (process::is_left_merge         (expr)) { replacements.insert(atermpp::down_cast<process::left_merge         >(expr).left()); replacements.insert(atermpp::down_cast<process::left_merge  >(expr).right()); }

    if (process::is_delta(expr))
    {
      replacements.erase(process::delta());
    }
    else
    {
      replacements.insert(process::delta());
    }

    std::size_t index = 0;
    for (const process::process_expression& replacement: replacements)
    {
      process::process_specification q = replace_subterm(p, x, depth, replacement);
      if (!process::is_well_typed(q))
      {
        continue;
      }
      std::string filename = input_filename.substr(0, input_filename.size() - 6) + "_" + utilities::number2string(depth) + "_" + utilities::number2string(x) + "_" + utilities::number2string(index) + ".mcrl2";
      std::string text = process::pp(q);
      utilities::detail::write_text(filename, text);
      std::cout << "file = " << filename << std::endl;
      index++;
    }
  }
}

class reduce_process_tool: public utilities::tools::input_output_tool
{
  protected:
    const std::size_t undefined = std::size_t(-1);
    using super = utilities::tools::input_output_tool;
    bool m_print_max_depth = false;
    bool m_verbose = false;
    std::size_t m_depth = undefined;

    void parse_options(const utilities::command_line_parser& parser) override
    {
      super::parse_options(parser);
      m_verbose = parser.options.count("verbose") > 0;
      if (parser.options.count("depth") > 0)
      {
        m_depth = parser.option_argument_as<std::size_t>("depth");
      }
    }

    void add_options(utilities::interface_description& desc) override
    {
      super::add_options(desc);
      desc.add_option("depth", utilities::make_optional_argument<std::size_t>("DEPTH", "0"), "the depth at which the reductions are applied", 'D');
    }

  public:
    reduce_process_tool()
      : super(NAME, AUTHOR,
              "reduce process specifications",
              "Reduces the process specification in INFILE by replacing subterms at a given depth. The reduced process specifications are written to files."
             )
    {}

    bool run() override
    {
      process::process_specification procspec = parse_process_specification(input_filename());
      if (m_verbose)
      {
        std::cout << "Position counts = " << core::detail::print_list(process::detail::position_counts(procspec)) << std::endl;
      }
      if (m_depth != undefined)
      {
        generate_reduced_processes(procspec, m_depth, input_filename());
      }
      return true;
    }
};

int main(int argc, char* argv[])
{
  return reduce_process_tool().execute(argc, argv);
}
