// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/process_variable_strongly_connected_components.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_PROCESS_VARIABLE_STRONGLY_CONNECTED_COMPONENTS_H
#define MCRL2_PROCESS_PROCESS_VARIABLE_STRONGLY_CONNECTED_COMPONENTS_H

#include "mcrl2/process/find.h"
#include "mcrl2/process/traverser.h"
#include <algorithm>
#include <cstdlib>
#include <set>
#include <utility>
#include <vector>

namespace mcrl2 {

namespace process {

namespace detail {

// Implementation of Tarjan's strongly connected components algorithm, based on
// http://en.wikipedia.org/wiki/Tarjan%27s_strongly_connected_components_algorithm
class tarjan_scc_algorithm
{
  public:
    typedef std::pair<std::size_t, std::size_t> edge;
    typedef std::vector<std::size_t> component;

  protected:
    static inline std::size_t undefined()
    {
      return std::size_t(-1);
    }

    struct vertex
    {
      std::size_t value;
      std::size_t index;
      std::size_t lowlink;
      bool onstack;

      bool operator==(const vertex& other) const
      {
        return value == other.value;
      }

      bool operator!=(const vertex& other) const
      {
        return !(*this == other);
      }

      vertex(std::size_t value_, std::size_t index_ = undefined(), std::size_t lowlink_ = undefined())
        : value(value_), index(index_), lowlink(lowlink_), onstack(false)
      {}
    };

    std::size_t strongconnect(std::size_t v_index, component& S, std::size_t index)
    {
      vertex& v = V[v_index];
      v.index = index;
      v.lowlink = index;
      index++;
      S.push_back(v_index);
      v.onstack = true;

      for (const edge& e: E)
      {
        auto u_index = e.first;
        auto w_index = e.second;
        const vertex& u = V[u_index];
        const vertex& w = V[w_index];
        if (u != v)
        {
          continue;
        }
        if (w.index == undefined())
        {
          index = strongconnect(w_index, S, index);
          v.lowlink = (std::min)(v.lowlink, w.lowlink);
        }
        else if (w.onstack)
        {
          v.lowlink = (std::min)(v.lowlink, w.index);
        }
      }

      if (v.lowlink == v.index)
      {
        component S1;
        while (true)
        {
          std::size_t w_index = S.back();
          S.pop_back();
          vertex& w = V[w_index];
          w.onstack = false;
          S1.push_back(w_index);
          if (w == v)
          {
            result.push_back(S1);
            break;
          }
        }
      }
      return index;
    }

    std::vector<vertex> V;
    std::vector<edge> E;
    std::size_t N;
    std::vector<component> result;

  public:
    tarjan_scc_algorithm()
    {}

    // The graph contains vertices [0, ... N).
    // The edges are pairs of vertices.
    void run(const std::vector<edge>& E_, std::size_t N_)
    {
      E = E_;
      N = N_;
      V.clear();
      result.clear();

      for (std::size_t i = 0; i < N; i++)
      {
        V.push_back(vertex(i));
      }

      component S;
      for (vertex& v: V)
      {
        if (v.index == undefined())
        {
          v.index = strongconnect(v.value, S, 0);
        }
      }
    }

    const std::vector<component>& components() const
    {
      return result;
    }
};

struct find_process_identifiers_traverser: public process_expression_traverser<find_process_identifiers_traverser>
{
  typedef process_expression_traverser<find_process_identifiers_traverser> super;
  using super::enter;
  using super::leave;
  using super::apply;

  std::set<process_identifier> result;

  void apply(const process::process_instance& x)
  {
    result.insert(x.identifier());
  }

  void apply(const process::process_instance_assignment& x)
  {
    result.insert(x.identifier());
  }
};

inline
std::set<process_identifier> find_process_identifiers(const process_expression& x)
{
  find_process_identifiers_traverser f;
  f.apply(x);
  return f.result;
}

struct process_variable_scc_algorithm
{
  std::map<process_identifier, std::size_t> index;
  std::map<process_identifier, std::set<process_identifier> > dependencies;

  void add_edge(const process_identifier& source, const process_identifier& target)
  {
    dependencies[source].insert(target);
  }

  void compute_index(const std::vector<process_equation>& equations)
  {
    index.clear();
    std::size_t i = 0;
    for (const process_equation& eqn: equations)
    {
      index[eqn.identifier()] = i++;
    }
  }

  void compute_dependencies(const std::vector<process_equation>& equations)
  {
    dependencies.clear();
    for (const process_equation& eqn: equations)
    {
      const process_identifier& source = eqn.identifier();
      for (const process_identifier& target: detail::find_process_identifiers(eqn.expression()))
      {
        add_edge(source, target);
      }
    }
  }

  // only add dependencies for equations reachable from init
  void compute_dependencies(const std::vector<process_equation>& equations, const process_expression& init)
  {
    dependencies.clear();
    std::set<process_identifier> todo;
    std::set<process_identifier> done;

    todo = find_process_identifiers(init);
    while (!todo.empty())
    {
      // pick an element from todo
      process_identifier source = *todo.begin();
      todo.erase(todo.begin());
      done.insert(source);

      const process_equation& eqn = find_equation(equations, source);

      // search for new identifiers in the rhs of the equation
      for (const process_identifier& target: find_process_identifiers(eqn.expression()))
      {
        add_edge(source, target);
        if (done.find(target) == done.end())
        {
          todo.insert(target);
        }
      }
    }
  }

  std::vector<std::set<process_identifier> > run_impl(const std::vector<process_equation>& equations)
  {
    // convert dependency graph to edge list
    std::vector<std::pair<std::size_t, std::size_t> > E;
    for (const auto& p: dependencies)
    {
      std::size_t source = index[p.first];
      for (const process_identifier& id: p.second)
      {
        std::size_t target = index[id];
        E.push_back({ source, target });
      }
    }

    // compute sccs
    tarjan_scc_algorithm algorithm;
    algorithm.run(E, index.size());

    // make result
    std::vector<std::set<process_identifier> > result;
    for (const auto& component: algorithm.components())
    {
      std::set<process_identifier> S;
      for (std::size_t i: component)
      {
        S.insert(equations[i].identifier());
      }
      result.push_back(S);
    }
    return result;
  }

  // make scc for all equations
  std::vector<std::set<process_identifier> > run(const std::vector<process_equation>& equations)
  {
    compute_index(equations);
    compute_dependencies(equations);
    return run_impl(equations);
  }

  // make scc for the equations reachable from init
  std::vector<std::set<process_identifier> > run(const std::vector<process_equation>& equations, const process_expression& init)
  {
    compute_index(equations);
    compute_dependencies(equations, init);
    return run_impl(equations);
  }
};

} // namespace detail

/// \brief Computes an SCC graph of the equations
inline
std::vector<std::set<process_identifier> > process_variable_strongly_connected_components(const std::vector<process_equation>& equations)
{
  detail::process_variable_scc_algorithm algorithm;
  return algorithm.run(equations);
}

/// \brief Compute an SCC graph of the equations reachable from init
inline
std::vector<std::set<process_identifier> > process_variable_strongly_connected_components(const std::vector<process_equation>& equations, const process_expression& init)
{
  detail::process_variable_scc_algorithm algorithm;
  return algorithm.run(equations, init);
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_PROCESS_VARIABLE_STRONGLY_CONNECTED_COMPONENTS_H
