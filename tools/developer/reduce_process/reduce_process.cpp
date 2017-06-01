// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file reduce_process.cpp

#define NAME "reduce_process"
#define AUTHOR "Wieger Wesselink"

#include <iostream>
#include <fstream>

#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/process/parse.h"
#include "mcrl2/process/replace_subterm.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/input_output_tool.h"
#include "mcrl2/utilities/text_utility.h"

using namespace mcrl2;

/// \brief Saves text to the file filename, or to stdout if filename equals "-".
inline
void write_text(const std::string& filename, const std::string& text)
{
  if (filename.empty())
  {
    std::cout << text;
  }
  else
  {
    std::ofstream out(filename);
    out << text;
  }
}

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
    process::process_expression expr = find_subterm(p, x, depth);
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
      std::string filename = input_filename.substr(0, input_filename.size() - 6) + "_" + utilities::number2string(depth) + "_" + utilities::number2string(x) + "_" + utilities::number2string(index) + ".mcrl2";
      std::string text = process::pp(q);
      write_text(filename, text);
      std::cout << "file = " << filename << std::endl;
      index++;
    }

    // process::process_specification q1 = replace_subterm(p, x, depth, process::delta());
    // process::process_specification q2 = replace_subterm(p, x, depth, process::tau());
    // if (!(p == q1) && !(p == q2))
    // {
    //   std::string filename1 = input_filename.substr(0, input_filename.size() - 6) + "_" + utilities::number2string(depth) + "_" + std::to_string(x) + "t.mcrl2";
    //   std::cout << "file = " << filename1 << std::endl;
    //   write_text(filename1, process::pp(q1));
    //   std::string filename2 = input_filename.substr(0, input_filename.size() - 6) + "_" + utilities::number2string(depth) + "_" + std::to_string(x) + "f.mcrl2";
    //   std::cout << "file = " << filename2 << std::endl;
    //   write_text(filename2, process::pp(q2));
    // }
  }
}

class reduce_process_tool: public utilities::tools::input_output_tool
{
  protected:
    const std::size_t undefined = std::size_t(-1);
    typedef utilities::tools::input_output_tool super;
    bool m_print_max_depth;
    bool m_verbose;
    std::size_t m_depth = undefined;

    void parse_options(const utilities::command_line_parser& parser)
    {
      super::parse_options(parser);
      m_verbose = parser.options.count("verbose") > 0;
      if (parser.options.count("depth") > 0)
      {
        m_depth = parser.option_argument_as<std::size_t>("depth");
      }
    }

    void add_options(utilities::interface_description& desc)
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

    bool run()
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
