// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file reduce_pbes.cpp

#define NAME "reduce_pbes"
#define AUTHOR "Wieger Wesselink"

#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/utilities/input_tool.h"
#include "mcrl2/pbes/pbes_input_tool.h"
#include "mcrl2/pbes/detail/pbes_io.h"
#include "mcrl2/pbes/replace_subterm.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;
using pbes_system::tools::pbes_input_tool;
using utilities::tools::input_tool;

void generate_reduced_pbesses(const pbes& p, std::size_t depth, const std::string& input_filename)
{
  std::vector<std::size_t> counts = pbes_system::detail::position_counts(p);
  if (depth >= counts.size())
  {
    return;
  }
  std::size_t n = counts[depth];
  for (std::size_t x = 0; x < n; x++)
  {
    pbes_system::pbes_expression expr = pbes_system::find_subterm(p, x, depth);
    std::set<pbes_system::pbes_expression> replacements;

    if (pbes_system::is_not   (expr)) { replacements.insert(atermpp::down_cast<pbes_system::not_  >(expr).operand()); }
    if (pbes_system::is_forall(expr)) { replacements.insert(atermpp::down_cast<pbes_system::forall>(expr).body()); }
    if (pbes_system::is_exists(expr)) { replacements.insert(atermpp::down_cast<pbes_system::exists>(expr).body()); }
    if (pbes_system::is_and   (expr)) { replacements.insert(atermpp::down_cast<pbes_system::and_  >(expr).left()); replacements.insert(atermpp::down_cast<pbes_system::and_>(expr).right()); }
    if (pbes_system::is_or    (expr)) { replacements.insert(atermpp::down_cast<pbes_system::or_   >(expr).left()); replacements.insert(atermpp::down_cast<pbes_system::or_ >(expr).right()); }
    if (pbes_system::is_imp   (expr)) { replacements.insert(atermpp::down_cast<pbes_system::imp   >(expr).left()); replacements.insert(atermpp::down_cast<pbes_system::imp >(expr).right()); }

    replacements.insert(pbes_system::false_());
    replacements.insert(pbes_system::true_());
    if (pbes_system::is_true(expr))
    {
      replacements.erase(pbes_system::true_());
      replacements.erase(pbes_system::false_());
    }
    if (pbes_system::is_false(expr))
    {
      replacements.erase(pbes_system::true_());
      replacements.erase(pbes_system::false_());
    }

    std::size_t index = 0;
    for (const pbes_system::pbes_expression& replacement: replacements)
    {
      pbes_system::pbes q = replace_subterm(p, x, depth, replacement);
      if (!q.is_well_typed())
      {
        continue;
      }
      std::string filename = input_filename.substr(0, input_filename.size() - 5) + "_" + utilities::number2string(depth) + "_" + utilities::number2string(x) + "_" + utilities::number2string(index) + ".pbes";
      std::string text = pbes_system::pp(q);
      pbes_system::detail::save_pbes(q, filename);
      std::cout << "file = " << filename << std::endl;
      index++;
    }
  }
}

class reduce_pbes_tool: public pbes_input_tool<input_tool>
{
  protected:
    const std::size_t undefined = std::size_t(-1);
    using super = pbes_input_tool<input_tool>;
    bool m_print_max_depth;
    bool m_verbose;
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
    reduce_pbes_tool()
      : super(NAME, AUTHOR,
              "reduce PBESs",
              "Reduces the PBES in INFILE by replacing subterms at a given depth. The reduced PBESs are written to files."
             )
    {}

    bool run() override
    {
      pbes p = pbes_system::detail::load_pbes(input_filename());
      if (m_verbose)
      {
        std::cout << "Position counts = " << core::detail::print_list(detail::position_counts(p)) << std::endl;
      }
      if (m_depth != undefined)
      {
        generate_reduced_pbesses(p, m_depth, input_filename());
      }
      return true;
    }
};

int main(int argc, char* argv[])
{
  return reduce_pbes_tool().execute(argc, argv);
}
