// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/eliminate_unused_equations.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_ELIMINATE_UNUSED_EQUATIONS_H
#define MCRL2_PROCESS_ELIMINATE_UNUSED_EQUATIONS_H

#include <algorithm>
#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/process/traverser.h"
#include "mcrl2/utilities/detail/container_utility.h"
#include "mcrl2/utilities/detail/join.h"

namespace mcrl2 {

namespace process {

namespace detail {

/// Computes dependencies of a process expression
struct process_variable_dependency_traverser: public process_expression_traverser<process_variable_dependency_traverser>
{
  typedef process_expression_traverser<process_variable_dependency_traverser> super;
  using super::enter;
  using super::leave;
  using super::apply;

  std::set<process_identifier> dependencies;

  void apply(const process::process_instance& x)
  {
    dependencies.insert(x.identifier());
  }

  void apply(const process::process_instance_assignment& x)
  {
    dependencies.insert(x.identifier());
  }
};

/// Creates a dependency graph of process variables
struct process_variable_dependency_graph_traverser: public process_expression_traverser<process_variable_dependency_graph_traverser>
{
  typedef process_expression_traverser<process_variable_dependency_graph_traverser> super;
  using super::enter;
  using super::leave;
  using super::apply;

  std::map<process_identifier, std::set<process_identifier> > dependencies;
  process_identifier current_equation_identifier;

  void enter(const process_equation& x)
  {
    current_equation_identifier = x.identifier();
    dependencies[x.identifier()]; // initialize to empty set
  }

  void apply(const process::process_instance& x)
  {
    dependencies[current_equation_identifier].insert(x.identifier());
  }

  void apply(const process::process_instance_assignment& x)
  {
    dependencies[current_equation_identifier].insert(x.identifier());
  }
};

} // namespace detail

// Removes unused equations
struct eliminate_unused_equations_algorithm
{
  std::vector<process_equation>& equations;
  process_expression init;

  eliminate_unused_equations_algorithm(std::vector<process_equation>& equations_, const process_expression& init_)
    : equations(equations_), init(init_)
  {}

  void run()
  {
    using utilities::detail::contains;

    // compute dependencies
    detail::process_variable_dependency_graph_traverser f;
    f.apply(equations);

    // compute dependencies of
    detail::process_variable_dependency_traverser g;
    g.apply(init);

    // make an index of the equations
    std::map<process_identifier, process_equation*> equation_index;
    for (process_equation& eqn: equations)
    {
      equation_index[eqn.identifier()] = &eqn;
    }

    // make new list of equations
    std::vector<process_equation> new_equations;

    std::set<process_identifier> todo = g.dependencies;
    std::set<process_identifier> done;
    while (!todo.empty())
    {
      process_identifier P = *todo.begin();
      todo.erase(todo.begin());
      done.insert(P);
      new_equations.push_back(*equation_index[P]);

      for (const process_identifier& Q: f.dependencies[P])
      {
        if (!contains(done, Q))
        {
          todo.insert(Q);
        }
      }
    }
    mCRL2log(log::verbose) << "done = " << core::detail::print_set(done) << std::endl;

    // print the removed equations
    std::vector<process_identifier> removed_equations;
    for (process_equation& eqn: equations)
    {
      if (!contains(done, eqn.identifier()))
      {
        removed_equations.push_back(eqn.identifier());
      }
    }
    mCRL2log(log::verbose) << "removed unused equations: " << core::detail::print_list(removed_equations) << std::endl;

    equations = new_equations;
  }
};

inline
void eliminate_unused_equations(std::vector<process_equation>& equations, const process_expression& init)
{
  eliminate_unused_equations_algorithm algorithm(equations, init);
  algorithm.run();
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_ELIMINATE_UNUSED_EQUATIONS_H
