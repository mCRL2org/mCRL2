// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/parity_game_output.h
/// \brief add your file description here.

#include <set>
#include <string>
#include <utility>
#include <vector>
#include <boost/algorithm/string/join.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/parity_game_generator.h"

#ifndef MCRL2_PBES_DETAIL_PARITY_GAME_OUTPUT_H
#define MCRL2_PBES_DETAIL_PARITY_GAME_OUTPUT_H

namespace mcrl2 {

namespace pbes_system {

namespace detail {

  /// A class that generates python code for a parity game, such
  /// that it can be solved with a parity game solver written in
  /// python.
  class parity_game_output: public parity_game_generator
  {
    protected:
      /// The vertices of the parity game graph.
      std::set<unsigned int> V;

      /// The edges of the parity game graph.
      std::set<std::pair<unsigned int, unsigned int> > E;

      /// The vertex priorities of the parity game graph.
      std::map<unsigned int, unsigned int> priorities;

      /// The even vertices of the parity game graph.
      std::set<unsigned int> even_vertices;

      /// The odd vertices of the parity game graph.
      std::set<unsigned int> odd_vertices;

      /// \brief Returns the quoted name of the vertex, for example "X1"
      /// \param i A positive integer
      /// \return The quoted name of the vertex, for example "X1"
      std::string vertex(unsigned int i) const
      {
        return "\"X" + boost::lexical_cast<std::string>(i+1) + "\"";
      }

      /// \brief Returns a tuple representing an edge, for example ("X1", "X2")
      /// \param e An edge
      /// \return A tuple representing an edge, for example ("X1", "X2")
      std::string edge(std::pair<unsigned int, unsigned int> e) const
      {
        return "(" + vertex(e.first) + ", " + vertex(e.second) + ")";
      }

      /// \brief Returns a string representing a priority, for example "X1":0
      /// \param p A pair of integers
      /// \return A string representing a priority, for example "X1":0
      std::string priority(std::pair<unsigned int, unsigned int> p) const
      {
        return vertex(p.first) + ":" + boost::lexical_cast<std::string>(p.second);
      }

      /// \brief Applies a function to the elements of a container
      /// \param c A container
      /// \param f A function
      /// \return The transformed container
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

      /// \brief Prints the elements of a container with a separator between them
      /// \param c A container
      /// \param sep A string
      /// \return The joined elements
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

      /// \brief Wraps a sequence of strings in a Python set.
      /// \param elements A vector of strings
      /// \return The Python set representation
      std::string python_set(const std::vector<std::string>& elements) const
      {
//        return "set([" + boost::algorithm::join(elements, ", ") +  "])"; THIS GIVES STACK OVERFLOW ERRORS!?!?
        return "set([" + join(elements, ", ") +  "])";
      }

      /// \brief Prints the todo list
      /// \param name A string
      /// \param todo A todo list
      void print_set(std::string name, const std::set<unsigned int>& todo) const
      {
        std::cerr << name << " = {";
        for (std::set<unsigned int>::const_iterator i = todo.begin(); i != todo.end(); ++i)
        {
          std::cerr << (i == todo.begin() ? "" : ", ") << *i;
        }
        std::cerr << "}" << std::endl;
      }

    public:
      parity_game_output(const pbes<>& p)
        : parity_game_generator(p, true)
      {}

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
      /// \brief Returns a representation of the parity game in python format.
      /// This code is used as input for a python parity game solver.
      /// \return A representation of the parity game in python format.
      std::string python_graph()
      {
        std::string result;
        result = result + python_set(apply(V, boost::bind(&parity_game_output::vertex, *this, _1))) + "\n";
        result = result + python_set(apply(E, boost::bind(&parity_game_output::edge, *this, _1))) + "\n";
        result = result + "{" + join(apply(priorities, boost::bind(&parity_game_output::priority, *this, _1)), ", ") + "}\n";
        result = result + python_set(apply(even_vertices, boost::bind(&parity_game_output::vertex, *this, _1))) + "\n";
        result = result + python_set(apply(odd_vertices, boost::bind(&parity_game_output::vertex, *this, _1)));
        return result;
      }

      /// \brief Returns a representation of the parity game in Alpaga format.
      /// \return A representation of the parity game in Alpaga format.
      std::string pgsolver_graph()
      {
        std::vector<std::string> lines(V.size());
        for (std::set<unsigned int>::const_iterator i = V.begin(); i != V.end(); ++i)
        {
          unsigned int k = *i;
          lines[k] = boost::lexical_cast<std::string>(k) + " " + boost::lexical_cast<std::string>(priorities[k]) + " " + (odd_vertices.find(*i) == odd_vertices.end() ? "0 " : "1 ");
        }
        for (std::set<std::pair<unsigned int, unsigned int> >::const_iterator i = E.begin(); i != E.end(); ++i)
        {
          unsigned int k = i->first;
          unsigned int m = i->second;
          std::string& line = lines[k];
          line += ((line[line.size()-1] == ' ' ? "" : ", ") + boost::lexical_cast<std::string>(m));
        }
        return join(lines, ";\n") + ";";
      }

      /// \brief Generates the parity game graph
      void run()
      {
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
        }
      }
  };

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_PARITY_GAME_OUTPUT_H
