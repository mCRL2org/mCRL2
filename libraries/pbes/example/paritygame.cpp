// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file paritygame.cpp
/// \brief Add your file description here.

#define MCRL2_ENUMERATE_QUANTIFIERS_REWRITER_DEBUG

#include <iterator>
#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>
#include <vector>
#include <boost/algorithm/string/join.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>
#include "mcrl2/core/messaging.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/parity_game_generator.h"

using namespace mcrl2::pbes_system;
namespace po = boost::program_options;

/// A class that generates python code for a parity game, such
/// that it can be solved with a parity game solver written in
/// python.
class python_parity_game_generator: public parity_game_generator
{
  protected:
    /// Returns the quoted name of the vertex, for example "X1"
    std::string vertex(unsigned int i) const
    {
      return "\"X" + boost::lexical_cast<std::string>(i+1) + "\"";
    }

    /// Returns a tuple representing an edge, for example ("X1", "X2")
    std::string edge(std::pair<unsigned int, unsigned int> e) const
    {
      return "(" + vertex(e.first) + ", " + vertex(e.second) + ")";
    }

    /// Returns a string representing a priority, for example "X1":0
    std::string priority(std::pair<unsigned int, unsigned int> p) const
    {
      return vertex(p.first) + ":" + boost::lexical_cast<std::string>(p.second);
    }

    template <typename Container, typename Function>
    std::vector<std::string> apply(const Container& c, Function f) const
    {
      std::vector<std::string> result;
      for (typename Container::const_iterator i = c.begin(); i != c.end(); ++i)
      {
        result.push_back(f(*i));
      }
      return result;
    }

    /// Wraps the elements in a set.
    std::string python_set(const std::vector<std::string>& elements) const
    {
      return "set([" + boost::algorithm::join(elements, ", ") +  "])";
    }

    template <class Iter, class T>
    void iota(Iter first, Iter last, T value) const
    {
      while (first != last)
      {
        *first++ = value++;
      }
    }

  public:
    python_parity_game_generator(pbes<>& p, bool true_false_dependencies)
      : parity_game_generator(p, true_false_dependencies)
    {}
    
    /// Generate python code for the python parity game solver.
    // Example:
    // set(["X1", "X2", "X3", "X4", "X5"])
    // set([("X1", "X2"), ("X2", "X1"), ("X2", "X3"), ("X3", "X4"), ("X3", "X5"), ("X4", "X1"), ("X5", "X3"), ("X5", "X1")])
    // {"X1":0, "X2":1, "X3":2, "X4":1, "X5":3}
    // set(["X3", "X4"])
    // set(["X1", "X2", "X5"])
    //
    // Line 1: set of vertices                   
    // Line 2: set of edges                      
    // Line 3: dictionary of priorities
    // Line 4: set of vertices for player even  
    // Line 5: set of vertices for player oneven
    std::string run()
    {
      std::set<unsigned int> V;
      std::set<std::pair<unsigned int, unsigned int> > E;
      std::map<unsigned int, unsigned int> priorities;
      std::set<unsigned int> even_vertices;
      std::set<unsigned int> odd_vertices;
      
      std::set<unsigned int> todo = get_initial_values();
      std::set<unsigned int> done;
      while (!todo.empty())
      {
        // handle vertex i
        unsigned int i = *todo.begin();
        todo.erase(i);
        done.insert(i);
        V.insert(i);    
        unsigned int p = get_priority(i);
        priorities[i] = p;
        std::set<unsigned int> dep_i = get_dependencies(i);

        switch (get_operation(i)) {
           case PGAME_AND: even_vertices.insert(i); break;
           case PGAME_OR:  odd_vertices.insert(i); break;
           default: assert(false);
        }
       
        for (std::set<unsigned int>::iterator j = dep_i.begin(); j != dep_i.end(); ++j)
        {
          // handle edge (i, *j)
          E.insert(std::make_pair(i, *j));
          if (done.find(*j) == done.end())
          {
            todo.insert(*j);
          }
        }
      }

      std::string result;
      result = result + python_set(apply(V, boost::bind(&python_parity_game_generator::vertex, *this, _1))) + "\n";
      result = result + python_set(apply(E, boost::bind(&python_parity_game_generator::edge, *this, _1))) + "\n";
      result = result + "{" + boost::algorithm::join(apply(priorities, boost::bind(&python_parity_game_generator::priority, *this, _1)), ", ") + "}\n";
      result = result + python_set(apply(even_vertices, boost::bind(&python_parity_game_generator::vertex, *this, _1))) + "\n";
      result = result + python_set(apply(odd_vertices, boost::bind(&python_parity_game_generator::vertex, *this, _1)));
      return result;
    }
};

int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  std::string infile;            // location of pbes
  bool true_false_dependencies;

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
      ("true-false-dependencies,t", po::value<bool>(&true_false_dependencies)->default_value(false), "generate dependencies for true and false")
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
    python_parity_game_generator pgg(p, true_false_dependencies);
    std::string text = pgg.run();
    std::ofstream to("paritygame.pg");
    to << text;
/*
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
*/    
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
