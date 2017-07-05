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

#include "mcrl2/pbes/parity_game_generator.h"
#include "mcrl2/pbes/pbes.h"
#include <boost/algorithm/string/join.hpp>
#include <functional>
#include <set>
#include <string>
#include <utility>
#include <vector>

#ifndef MCRL2_PBES_DETAIL_PARITY_GAME_OUTPUT_H
#define MCRL2_PBES_DETAIL_PARITY_GAME_OUTPUT_H

namespace mcrl2
{

namespace pbes_system
{

namespace detail
{

/// A class that generates python code for a parity game, such
/// that it can be solved with a parity game solver written in
/// python.
class parity_game_output: public parity_game_generator
{
  protected:
    /// The vertices of the parity game graph.
    std::set<std::size_t> V;

    /// The edges of the parity game graph.
    std::set<std::pair<std::size_t, std::size_t> > E;

    /// The vertex priorities of the parity game graph.
    std::map<std::size_t, std::size_t> priorities;

    /// The even vertices of the parity game graph.
    std::set<std::size_t> even_vertices;

    /// The odd vertices of the parity game graph.
    std::set<std::size_t> odd_vertices;

    /// \brief Returns the quoted name of the vertex, for example "X1"
    /// \param i A positive integer
    /// \return The quoted name of the vertex, for example "X1"
    std::string vertex(std::size_t i) const
    {
      return "\"X" + utilities::number2string(i+1) + "\"";
    }

    /// \brief Returns a tuple representing an edge, for example ("X1", "X2")
    /// \param e An edge
    /// \return A tuple representing an edge, for example ("X1", "X2")
    std::string edge(std::pair<std::size_t, std::size_t> e) const
    {
      return "(" + vertex(e.first) + ", " + vertex(e.second) + ")";
    }

    /// \brief Returns a string representing a priority, for example "X1":0
    /// \param p A pair of integers
    /// \return A string representing a priority, for example "X1":0
    std::string priority(std::pair<std::size_t, std::size_t> p) const
    {
      return vertex(p.first) + ":" + utilities::number2string(p.second);
    }

    /// \brief Applies a function to the elements of a container
    /// \param c A container
    /// \param f A function
    /// \return The transformed container
    template <typename Container, typename Function>
    std::vector<std::string> apply(const Container& c, const Function& f) const
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
    std::string join(const Container& c, const std::string& sep) const
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
      return "set([" + join(elements, ", ") +  "])";
    }

    /// \brief Prints the todo list
    /// \param name A string
    /// \param todo A todo list
    void print_set(const std::string& name, const std::set<std::size_t>& todo) const
    {
      mCRL2log(log::verbose) << name << " = {";
      for (std::set<std::size_t>::const_iterator i = todo.begin(); i != todo.end(); ++i)
      {
        mCRL2log(log::verbose) << (i == todo.begin() ? "" : ", ") << *i;
      }
      mCRL2log(log::verbose) << "}" << std::endl;
    }

  public:
    parity_game_output(pbes& p, bool min_parity_game = true)
      : parity_game_generator(p, true, min_parity_game)
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
#ifdef _MSC_VER
      result = result + python_set(apply(V, [&](std::size_t i) { return vertex(i); })) + "\n";
      result = result + python_set(apply(E, [&](std::pair<std::size_t, std::size_t> e) { return edge(e); })) + "\n";
      result = result + "{" + join(apply(priorities, [&](std::pair<std::size_t, std::size_t> p) { return priority(p); }), ", ") + "}\n";
      result = result + python_set(apply(even_vertices, [&](std::size_t i) { return vertex(i); })) + "\n";
      result = result + python_set(apply(odd_vertices, [&](std::size_t i) { return vertex(i); }));
#else      
      // std::ref is needed to prevent copying of *this
      result = result + python_set(apply(V, std::bind(&parity_game_output::vertex, std::ref(*this), std::placeholders::_1))) + "\n";
      result = result + python_set(apply(E, std::bind(&parity_game_output::edge, std::ref(*this), std::placeholders::_1))) + "\n";
      result = result + "{" + join(apply(priorities, std::bind(&parity_game_output::priority, std::ref(*this), std::placeholders::_1)), ", ") + "}\n";
      result = result + python_set(apply(even_vertices, std::bind(&parity_game_output::vertex, std::ref(*this), std::placeholders::_1))) + "\n";
      result = result + python_set(apply(odd_vertices, std::bind(&parity_game_output::vertex, std::ref(*this), std::placeholders::_1)));
#endif      
      return result;
    }

    /// \brief Returns a representation of the parity game in Alpaga format.
    /// \return A representation of the parity game in Alpaga format.
    std::string pgsolver_graph()
    {
      std::vector<std::string> lines(V.size());
      for (std::size_t k: V)
      {
        lines[k] = std::to_string(k) + " " + utilities::number2string(priorities[k]) + " " + (odd_vertices.find(k) == odd_vertices.end() ? "0 " : "1 ");
      }
      for (const auto& i: E)
      {
        std::size_t k = i.first;
        std::size_t m = i.second;
        std::string& line = lines[k];
        line += ((line[line.size()-1] == ' ' ? "" : ", ") + utilities::number2string(m));
      }
      return join(lines, ";\n") + ";";
    }

    /// \brief Generates the parity game graph
    void run()
    {
      std::set<std::size_t> todo = get_initial_values();
      std::set<std::size_t> done;
      while (!todo.empty())
      {
        // handle vertex i
        std::size_t i = *todo.begin();
        todo.erase(i);
        done.insert(i);
        V.insert(i);
        std::size_t p = get_priority(i);
        priorities[i] = p;
        std::set<std::size_t> dep_i = get_dependencies(i);
        switch (get_operation(i))
        {
          case PGAME_AND:
            odd_vertices.insert(i);
            break;
          case PGAME_OR:
            even_vertices.insert(i);
            break;
          default:
            assert(false);
        }

        for (std::size_t j: dep_i)
        {
          // handle edge (i, *j)
          E.insert(std::make_pair(i, j));
          if (done.find(j) == done.end())
          {
            todo.insert(j);
          }
        }
      }
    }
};

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_PARITY_GAME_OUTPUT_H
