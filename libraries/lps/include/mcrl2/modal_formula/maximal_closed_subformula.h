// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/maximal_closed_subformula.h
/// \brief add your file description here.

#ifndef MCRL2_MODAL_FORMULA_MAXIMAL_CLOSED_SUBFORMULA_H
#define MCRL2_MODAL_FORMULA_MAXIMAL_CLOSED_SUBFORMULA_H

#include <iostream>
#include "mcrl2/data/find.h"
#include "mcrl2/modal_formula/traverser.h"

namespace mcrl2 {

namespace state_formulas {

namespace detail {

inline
std::size_t child_count(const state_formula& x)
{
  if (data::is_data_expression(x))            { return 0; }
  else if (state_formulas::is_true(x))        { return 0; }
  else if (state_formulas::is_false(x))       { return 0; }
  else if (state_formulas::is_not(x))         { return 1; }
  else if (state_formulas::is_and(x))         { return 2; }
  else if (state_formulas::is_or(x))          { return 2; }
  else if (state_formulas::is_imp(x))         { return 2; }
  else if (state_formulas::is_forall(x))      { return 1; }
  else if (state_formulas::is_exists(x))      { return 1; }
  else if (state_formulas::is_must(x))        { return 1; }
  else if (state_formulas::is_may(x))         { return 1; }
  else if (state_formulas::is_yaled(x))       { return 0; }
  else if (state_formulas::is_yaled_timed(x)) { return 0; }
  else if (state_formulas::is_delay(x))       { return 0; }
  else if (state_formulas::is_delay_timed(x)) { return 0; }
  else if (state_formulas::is_variable(x))    { return 0; }
  else if (state_formulas::is_nu(x))          { return 1; }
  else if (state_formulas::is_mu(x))          { return 1; }
  throw mcrl2::runtime_error("child_count: unknown argument " + x.to_string());
  return 0;
}

template <template <class> class Traverser, class Node, class Derived>
struct bottom_up_traverser: public Traverser<Derived>
{
  typedef Traverser<Derived> super;
  using super::enter;   
#ifndef BOOST_MSVC  
  using super::leave;
#endif
  using super::operator();

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  // Maintain a stack with nodes, used to store intermediate results
  atermpp::vector<Node> node_stack;
  typedef typename atermpp::vector<Node>::const_iterator node_iterator;

  // Push a node to node_stack
  void push(const Node& node)
  {
    node_stack.push_back(node);
  }

  // Pop the top element of node_stack and return it
  Node pop()
  {
    Node result = node_stack.back();
    node_stack.pop_back();
    return result;
  }

  // Return the top element of node_stack
  Node& top()
  {
    return node_stack.back();
  }

  // Return the top element of node_stack
  const Node& top() const
  {
    return node_stack.back();
  }

  template <typename T>
  void join(const T& x, node_iterator first, node_iterator last, Node& result)
  {
  }

  // Override the leave function, such that it combines the results of child nodes
  template <typename T>
  void leave(const T& x)
  {
    Node result;
    std::size_t n = child_count(x);
    derived().join(x, node_stack.end() - n, node_stack.end(), result);
    node_stack.erase(node_stack.end() - n, node_stack.end());
    push(result);
  }

  // This leave function needs to be disabled.
  // TODO: It seems more logical that the call to this leave function is not generated in
  // the traverser, to avoid that the same term is visited twice.
  void leave(const state_formulas::state_formula&)
  {
  }
};

struct free_variables_node
{
  std::set<data::variable> variables;

  free_variables_node(const std::set<data::variable>& variables_ = std::set<data::variable>())
    : variables(variables_)
  { }
};

struct maximal_closed_subformula_node: public free_variables_node
{
  std::set<state_formulas::state_formula> formulas;

  maximal_closed_subformula_node(const std::set<data::variable>& variables = std::set<data::variable>(),
                                 const std::set<state_formulas::state_formula>& formulas_ = std::set<state_formulas::state_formula>()
                                )
    : free_variables_node(variables),
      formulas(formulas_)
  { }
};

std::ostream& operator<<(std::ostream& out, const maximal_closed_subformula_node& node)
{
  out << "<variables>";
  for (std::set<data::variable>::const_iterator i = node.variables.begin(); i != node.variables.end(); ++i)
  {
    out << data::pp(*i) << " ";
  }
  out << "\n<formulas>";
  for (std::set<state_formulas::state_formula>::const_iterator i = node.formulas.begin(); i != node.formulas.end(); ++i)
  {
    out << state_formulas::pp(*i) << " ";
  }
  return out;
}

template <typename Derived>
struct maximal_closed_subformula_traverser: public bottom_up_traverser<state_formulas::state_formula_traverser, maximal_closed_subformula_node, Derived>
{
  typedef bottom_up_traverser<state_formulas::state_formula_traverser, maximal_closed_subformula_node, Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();
  using super::push;
  using super::pop;
  using super::top;
  using super::node_stack;

  typedef typename super::node_iterator node_iterator;

#if BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  template <typename T>
  void update_free_variables(const T& x, maximal_closed_subformula_node& result)
  { }

  void update_free_variables(const data::data_expression& x, maximal_closed_subformula_node& result)
  {
    data::find_free_variables(x, std::inserter(result.variables, result.variables.end()));
  }

  void update_free_variables(const state_formulas::forall& x, maximal_closed_subformula_node& result)
  {
    data::variable_list v = x.variables();
    for (data::variable_list::const_iterator i = v.begin(); i != v.end(); ++i)
    {
      result.variables.erase(*i);
    }
  }

  void update_free_variables(const state_formulas::exists& x, maximal_closed_subformula_node& result)
  {
    data::variable_list v = x.variables();
    for (data::variable_list::const_iterator i = v.begin(); i != v.end(); ++i)
    {
      result.variables.erase(*i);
    }
  }

  void update_free_variables(const state_formulas::variable& x, maximal_closed_subformula_node& result)
  {
    data::data_expression_list v = x.arguments();
    for (data::data_expression_list::const_iterator i = v.begin(); i != v.end(); ++i)
    {
      data::find_free_variables(*i, std::inserter(result.variables, result.variables.end()));
    }
  }

  template <typename T>
  void update_maximal_closed_formulas(const T& x, maximal_closed_subformula_node& result)
  {
    if (result.variables.empty())
    {
      result.formulas.clear();
      result.formulas.insert(x);
    }
  }

  template <typename T>
  void join(const T& x, node_iterator first, node_iterator last, maximal_closed_subformula_node& result)
  {
    for (node_iterator i = first; i != last; ++i)
    {
      result.variables.insert(i->variables.begin(), i->variables.end());
    }
    update_free_variables(x, result);
    if (result.variables.empty())
    {
      result.formulas.insert(x);
    }
    else
    {
      for (node_iterator i = first; i != last; ++i)
      {
        result.formulas.insert(i->formulas.begin(), i->formulas.end());
      }
    }
  }
};

struct apply_maximal_closed_subformula_traverser: public maximal_closed_subformula_traverser<apply_maximal_closed_subformula_traverser>
{
  typedef maximal_closed_subformula_traverser<apply_maximal_closed_subformula_traverser> super;
  using super::enter;
  using super::leave;
  using super::operator();
  using super::top;
};

}; // namespace detail

inline
std::set<state_formulas::state_formula> maximal_closed_subformulas(const state_formula& x)
{
  detail::apply_maximal_closed_subformula_traverser f;
  f(x);
  return f.top().formulas;
}

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_MAXIMAL_CLOSED_SUBFORMULA_H
