// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file paritygame.cpp
/// \brief Add your file description here.

#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>
#include <boost/program_options.hpp>
#include "mcrl2/core/messaging.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/parity_game_generator.h"

using namespace mcrl2::pbes_system;
namespace po = boost::program_options;

int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  std::string infile;            // location of pbes

  try {
    //--- paritygame options ---------
    boost::program_options::options_description paritygame_options(
      "Usage: paritygame [OPTION]... INFILE\n"
      "\n"
      "Reads a file containing a pbes, and generates a bes.\n"
      "\n"
      "Options"
    );
    paritygame_options.add_options()
      ("help,h", "display this help")
      ("verbose,v", "display short intermediate messages")
      ("debug,d", "display detailed intermediate messages")
      ;

    //--- hidden options ---------
    po::options_description hidden_options;
    hidden_options.add_options()
      ("input-file", po::value<std::string>(&infile), "input file")
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
    }

    pbes<> p;
    p.load(infile);
    parity_game_generator pgg(p);
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
  catch(mcrl2::runtime_error e)
  {
    std::cerr << "runtime error: " << e.what() << std::endl;
    std::exit(1);
  }
  catch(std::exception& e) {
    std::cerr << "error: " << e.what() << "\n";
    return 1;
  }
  catch(...) {
    std::cerr << "exception of unknown type!\n";
  }

  return 0;
}
