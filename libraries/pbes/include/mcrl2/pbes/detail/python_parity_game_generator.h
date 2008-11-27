// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/python_parity_game_generator.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_PYTHON_PARITY_GAME_GENERATOR_H
#define MCRL2_PBES_DETAIL_PYTHON_PARITY_GAME_GENERATOR_H

#include <set>
#include <string>
#include <utility>
#include <vector>
#include <boost/algorithm/string/join.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/parity_game_generator.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

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

      template <typename Container>
      std::string join(const Container& c, std::string sep) const
      {
        std::ostringstream out;
        for (typename Container::const_iterator i = c.begin(); i != c.end(); ++i)
        {
          out << (i == c.begin() ? "" : sep) << *i;
        }
        return out.str();
      }

      /// Wraps the elements in a set.
      std::string python_set(const std::vector<std::string>& elements) const
      {
//        return "set([" + boost::algorithm::join(elements, ", ") +  "])"; THIS GIVES STACK OVERFLOW ERRORS!?!?
        return "set([" + join(elements, ", ") +  "])";
      }

      template <class Iter, class T>
      void iota(Iter first, Iter last, T value) const
      {
        while (first != last)
        {
          *first++ = value++;
        }
      }

      void print_set(std::string name, const std::set<unsigned int>& todo) const
      {
        std::cout << name << " = {";
        for (std::set<unsigned int>::const_iterator i = todo.begin(); i != todo.end(); ++i)
        {
          std::cout << (i == todo.begin() ? "" : ", ") << *i;
        }
        std::cout << "}" << std::endl;
      }

    public:
      python_parity_game_generator(const pbes<>& p)
        : parity_game_generator(p, true)
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
#ifdef MCRL2_PARITY_GAME_DEBUG
  print_set("todo", todo);
#endif
          // handle vertex i
          unsigned int i = *todo.begin();
          todo.erase(i);
          done.insert(i);
          V.insert(i);
          unsigned int p = get_priority(i);
          priorities[i] = p;
          std::set<unsigned int> dep_i = get_dependencies(i);
#ifdef MCRL2_PARITY_GAME_DEBUG
  print_set("  dependencies vertex " + boost::lexical_cast<std::string>(i), dep_i);
#endif
          switch (get_operation(i)) {
             case PGAME_AND: odd_vertices.insert(i); break;
             case PGAME_OR:  even_vertices.insert(i); break;
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
#ifdef MCRL2_PARITY_GAME_DEBUG
  print_set("V", V);
#endif
        }

        std::string result;
        result = result + python_set(apply(V, boost::bind(&python_parity_game_generator::vertex, *this, _1))) + "\n";
        result = result + python_set(apply(E, boost::bind(&python_parity_game_generator::edge, *this, _1))) + "\n";
        result = result + "{" + join(apply(priorities, boost::bind(&python_parity_game_generator::priority, *this, _1)), ", ") + "}\n";
        result = result + python_set(apply(even_vertices, boost::bind(&python_parity_game_generator::vertex, *this, _1))) + "\n";
        result = result + python_set(apply(odd_vertices, boost::bind(&python_parity_game_generator::vertex, *this, _1)));
		return result;
      }
  };

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_PYTHON_PARITY_GAME_GENERATOR_H
