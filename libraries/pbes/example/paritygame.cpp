// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file paritygame.cpp
/// \brief Example program for the parity_game_generator class.

#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>
#include "mcrl2/core/messaging.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/parity_game_generator.h"
#include "mcrl2/pbes/detail/parity_game_output.h"
#include "mcrl2/atermpp/aterm_init.h"
#include "mcrl2/utilities/input_output_tool.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;

// Example usage of the parity_game_generator class.
void run1(pbes<>& p, bool min_parity_game)
{
  parity_game_generator pgg(p, min_parity_game);
  std::set<unsigned int> todo = pgg.get_initial_values();
  std::set<unsigned int> done;
  while (!todo.empty())
  {
    unsigned int i = *todo.begin();
    todo.erase(i);
    done.insert(i);

    parity_game_generator::operation_type t = pgg.get_operation(i);
    unsigned int p = pgg.get_priority(i);
    std::set<unsigned int> v = pgg.get_dependencies(i);
    std::cout << "adding equation " << i << ", dependencies = [";
    for (std::set<unsigned int>::iterator j = v.begin(); j != v.end(); ++j)
    {
      if (done.find(*j) == done.end())
      {
        todo.insert(*j);
      }
      std::cout << (j == v.begin() ? "" : ", ") << *j;
    }
    std::cout << "], priority = " << p << " type = " << (t == parity_game_generator::PGAME_AND ? "AND" : "OR") << std::endl;
  }
  pgg.print_variable_mapping();
}

// Create a parity game graph, and write it to outfile. The graph
// is in pgsolver format, see http://www.tcs.ifi.lmu.de/~mlange/pgsolver/index.html.
void run2(pbes<>& p, bool min_parity_game, std::string outfile)
{
  pbes_system::detail::parity_game_output pgg(p, min_parity_game);
  pgg.run();
  std::string text = pgg.pgsolver_graph();
  std::ofstream to(outfile.c_str());
  to << text;
}

using namespace mcrl2::utilities;

/// \brief Simple input/output tool that transforms a pbes to a parity game
class paritygame_tool: public mcrl2::utilities::tools::input_output_tool
{

  protected:

    typedef tools::input_output_tool super;

    bool m_max_pg;

    void add_options(mcrl2::utilities::interface_description& desc)
    {
      super::add_options(desc);
      desc.add_option("max_parity_game", "generate max parity game instead of min parity game", 'm');
    }

    void parse_options(const mcrl2::utilities::command_line_parser& parser)
    {
      super::parse_options(parser);
      m_max_pg = parser.options.count("max_parity_game") > 0;
    }

  public:
     paritygame_tool()
      : super(
          "paritygame",
          "Wieger Wesselink",
          "Reads a file containing a pbes, and generates a parity game.",
          "Convert the PBES in INFILE to parity game and write the result to OUTFILE. If INFILE is not "
          "present, stdin is used. If OUTFILE is not present, stdout is used. By default a min-parity game is generated."),
        m_max_pg(false)
    {}

    bool run()
    {
      if (mcrl2::core::gsVerbose)
      {
        std::cout << "paritygame parameters:" << std::endl;
        std::cout << "  input file:         " << m_input_filename << std::endl;
        std::cout << "  output file:        " << m_output_filename << std::endl;
      }

      pbes_system::pbes<> p;
      p.load(m_input_filename);

      if(!m_max_pg)
      {
        run1(p, true);
      }
      run2(p, m_max_pg, m_output_filename);
    }
};

int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  return paritygame_tool().execute(argc, argv);
}
