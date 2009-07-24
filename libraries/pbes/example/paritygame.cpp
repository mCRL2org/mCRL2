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
#include <boost/program_options.hpp>
#include "mcrl2/core/messaging.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/parity_game_generator.h"
#include "mcrl2/pbes/detail/parity_game_output.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2;
using namespace mcrl2::pbes_system;
namespace po = boost::program_options;

// Example usage of the parity_game_generator class.
void run1(const pbes<>& p, bool min_parity_game)
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
void run2(const pbes<>& p, bool min_parity_game, std::string outfile)
{
  pbes_system::detail::parity_game_output pgg(p, min_parity_game);
  pgg.run();
  std::string text = pgg.pgsolver_graph();
  std::ofstream to(outfile.c_str());
  to << text;
}

int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  std::string infile;            // location of pbes
  std::string outfile;           // location of result

  try {
    //--- paritygame options ---------
    boost::program_options::options_description paritygame_options(
      "Usage: paritygame [OPTION]... INFILE OUTFILE\n"
      "\n"
      "Reads a file containing a pbes, and generates a parity game.\n"
      "By default a min-parity game is generated.\n"
      "\n"
      "Options"
    );
    paritygame_options.add_options()
      ("help,h", "display this help")
      ("verbose,v", "display short intermediate messages")
      ("debug,d", "display detailed intermediate messages")
      ("max-parity-game,m", "generate max-parity game instead of min-parity game")
      ;

    //--- hidden options ---------
    po::options_description hidden_options;
    hidden_options.add_options()
      ("input-file", po::value<std::string>(&infile), "input file")
      ("output-file", po::value<std::string>(&outfile), "output file")
    ;

    //--- positional options ---------
    po::positional_options_description positional_options;
    positional_options.add("input-file", 1);
    positional_options.add("output-file", 1);

    //--- command line options ---------
    po::options_description cmdline_options;
    cmdline_options.add(paritygame_options).add(hidden_options);

    po::variables_map var_map;
    po::store(po::command_line_parser(argc, argv).
        options(cmdline_options).positional(positional_options).run(), var_map);
    po::notify(var_map);

    if (var_map.count("help")) {
      std::cout << paritygame_options << "\n";
      return 1;
    }
    if (var_map.count("debug")) {
      mcrl2::core::gsSetDebugMsg();
    }
    if (var_map.count("verbose")) {
      mcrl2::core::gsSetVerboseMsg();
    }

    if (mcrl2::core::gsVerbose)
    {
      std::cout << "paritygame parameters:" << std::endl;
      std::cout << "  input file:         " << infile << std::endl;
      std::cout << "  output file:        " << outfile << std::endl;
    }

    pbes_system::pbes<> p;
    p.load(infile);

    if (var_map.count("max-parity-game"))
    {
      run2(p, false, outfile);
    }
    else
    {
      run1(p, true);
      run2(p, true, outfile);
    }
  }
  catch(std::exception& e) {
    std::cerr << "error: " << e.what() << "\n";
  }

  return 0;
}
