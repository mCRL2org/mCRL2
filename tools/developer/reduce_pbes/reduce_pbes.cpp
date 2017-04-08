// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file reduce_pbes.cpp

#define NAME "reduce_pbes"
#define AUTHOR "Wieger Wesselink"

#include <iostream>
#include <fstream>
#include <string>
#include <type_traits>

#include "mcrl2/bes/pbes_input_tool.h"
#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/pbes/replace_subterm.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/input_tool.h"
#include "mcrl2/utilities/text_utility.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;
using bes::tools::pbes_input_tool;
using utilities::tools::input_tool;

void generate_reduced_pbesses(const pbes& p, std::size_t depth, const std::string& input_filename)
{
  std::vector<std::size_t> counts = detail::position_counts(p);
  if (depth >= counts.size())
  {
    return;
  }
  std::size_t n = counts[depth];
  for (std::size_t x = 0; x < n; x++)
  {
    pbes q1 = replace_subterm(p, x, depth, pbes_system::true_());
    pbes q2 = replace_subterm(p, x, depth, pbes_system::false_());
    if (!(p == q1) && !(p == q2))
    {
      std::string filename1 = input_filename.substr(0, input_filename.size() - 5) + "_" + utilities::number2string(depth) + "_" + std::to_string(x) + "t.pbes";
      std::cout << "file = " << filename1 << std::endl;
      save_pbes(q1, filename1, pbes_system::pbes_format_internal());
      std::string filename2 = input_filename.substr(0, input_filename.size() - 5) + "_" + utilities::number2string(depth) + "_" + std::to_string(x) + "f.pbes";
      std::cout << "file = " << filename2 << std::endl;
      save_pbes(q2, filename2, pbes_system::pbes_format_internal());
    }
  }
}

class reduce_pbes_tool: public pbes_input_tool<input_tool>
{
  protected:
    const std::size_t undefined = std::size_t(-1);
    typedef pbes_input_tool<input_tool> super;
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
    reduce_pbes_tool()
      : super(NAME, AUTHOR,
              "reduce PBESs",
              "Reduces the PBES in INFILE by replacing subterms at a given depth. The reduced PBESs are written to files."
             )
    {}

    bool run()
    {
      pbes p;
      load_pbes(p, input_filename(), pbes_input_format());
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
