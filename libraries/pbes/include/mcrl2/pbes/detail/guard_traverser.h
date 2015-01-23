// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/guard_traverser.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_GUARD_TRAVERSER_H
#define MCRL2_PBES_DETAIL_GUARD_TRAVERSER_H

#include "mcrl2/data/rewriter.h"
#include "mcrl2/pbes/traverser.h"
#include "mcrl2/pbes/pbes_functions.h"
#include "mcrl2/pbes/rewrite.h"
#include "mcrl2/pbes/rewriters/simplify_rewriter.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/optimized_boolean_operators.h"

#ifdef MCRL2_PBES_STATEGRAPH_CHECK_GUARDS
#include "mcrl2/pbes/find.h"
#endif

namespace mcrl2 {

namespace pbes_system {

namespace detail {

#ifdef MCRL2_PBES_STATEGRAPH_CHECK_GUARDS
inline
pbes_expression guard_s(const pbes_expression& x)
{
  if (is_simple_expression(x))
  {
    return x;
  }
  else
  {
    return true_();
  }
}

inline
pbes_expression guard_n(const pbes_expression& x)
{
  if (is_simple_expression(x))
  {
    return utilities::optimized_not(x);
  }
  else
  {
    return true_();
  }
}

inline
bool has_propositional_variable(const pbes_expression& x, const propositional_variable_instantiation& X)
{
  std::set<propositional_variable_instantiation> xocc = find_propositional_variable_instantiations(x);
  return xocc.find(X) != xocc.end();
}

inline
pbes_expression guard_impl(const propositional_variable_instantiation& X, const pbes_expression& x)
{
  if (data::is_data_expression(x))
  {
    return false_();
  }
  else if (is_propositional_variable_instantiation(x))
  {
    return true_();
  }
  else if (pbes_system::is_true(x))
  {
    return false_();
  }
  else if (pbes_system::is_false(x))
  {
    return false_();
  }
  else if (pbes_system::is_not(x))
  {
    pbes_expression phi = pbes_system::not_(atermpp::aterm_appl(x)).operand();
    return utilities::optimized_not(guard_impl(X, phi));
  }
  else if (pbes_system::is_and(x))
  {
    pbes_expression phi = pbes_system::and_(atermpp::aterm_appl(x)).left();
    pbes_expression psi = pbes_system::and_(atermpp::aterm_appl(x)).right();
    if (has_propositional_variable(psi, X))
    {
      return utilities::optimized_and(guard_s(phi), guard_impl(X, psi));
    }
    else
    {
      return utilities::optimized_and(guard_s(psi), guard_impl(X, phi));
    }
  }
  else if (pbes_system::is_or(x))
  {
    pbes_expression phi = pbes_system::or_(atermpp::aterm_appl(x)).left();
    pbes_expression psi = pbes_system::or_(atermpp::aterm_appl(x)).right();
    if (has_propositional_variable(psi, X))
    {
      return utilities::optimized_and(guard_n(phi), guard_impl(X, psi));
    }
    else
    {
      return utilities::optimized_and(guard_n(psi), guard_impl(X, phi));
    }
  }
  else if (pbes_system::is_imp(x))
  {
    pbes_expression phi = pbes_system::imp(atermpp::aterm_appl(x)).left();
    pbes_expression psi = pbes_system::imp(atermpp::aterm_appl(x)).right();
    return guard_impl(X, or_(not_(phi), psi));
  }
  else if (pbes_system::is_forall(x))
  {
    pbes_expression phi = pbes_system::forall(atermpp::aterm_appl(x)).body();
    return guard_impl(X, phi);
  }
  else if (pbes_system::is_exists(x))
  {
    pbes_expression phi = pbes_system::exists(atermpp::aterm_appl(x)).body();
    return guard_impl(X, phi);
  }
  throw mcrl2::runtime_error("guard_impl: unknown term " + pbes_system::pp(x));
  return pbes_expression();
}

// Direct implementation of the definition, to be used for checking results.
inline
pbes_expression guard(const propositional_variable_instantiation& X, const pbes_expression& x)
{
  std::multiset<propositional_variable_instantiation> xocc;
  find_propositional_variable_instantiations(x, std::inserter(xocc, xocc.end()));
  if (xocc.count(X) != 1)
  {
    throw mcrl2::runtime_error("guard is undefined for " + pbes_system::pp(X));
  }
  return guard_impl(X, x);
}
#endif // MCRL2_PBES_STATEGRAPH_CHECK_GUARDS

struct guard_expression
{
  std::vector<std::pair<propositional_variable_instantiation, pbes_expression> > guards;
  pbes_expression condition;

  bool is_simple() const
  {
    return guards.empty();
  }

  void add_guard(const pbes_expression& guard)
  {
    if (is_simple())
    {
      condition = utilities::optimized_and(guard, condition);
    }
    else
    {
      for (std::vector<std::pair<propositional_variable_instantiation, pbes_expression> >::iterator i = guards.begin(); i != guards.end(); ++i)
      {
        i->second = utilities::optimized_and(guard, i->second);
      }
    }
  }

  void negate()
  {
    if (is_simple())
    {
      condition = utilities::optimized_not(condition);
    }
    else
    {
      for (auto i = guards.begin(); i != guards.end(); ++i)
      {
        i->second = utilities::optimized_not(i->second);
      }
    }
  }

  guard_expression(const pbes_expression& condition_ = pbes_system::true_())
    : condition(condition_)
  {}

#ifdef MCRL2_PBES_STATEGRAPH_CHECK_GUARDS
  // Check if the guards were correctly computed with respect to x.
  template <typename PbesRewriter>
  bool check_guards(const pbes_expression& x, PbesRewriter R) const
  {
    mCRL2log(log::debug, "stategraph") << "check_guards: x = " << pbes_system::pp(x) << std::endl;
    bool result = true;
    for (auto i = guards.begin(); i != guards.end(); ++i)
    {
      try
      {
        const propositional_variable_instantiation& X = i->first;
        const pbes_expression& g1 = i->second;
        pbes_expression g2 = guard(X, x);
        if (pbes_rewrite(g1, R) != pbes_rewrite(g2, R))
        {
          result = false;
mCRL2log(log::debug, "stategraph") << " g1 = " << g1 << " g2 = " << g2 << std::endl;
          mCRL2log(log::debug, "stategraph") << "guard error: X = " << X << " g1 = " << pbes_rewrite(g1, R) << " g2 = " << pbes_rewrite(g2, R) << std::endl;
        }
      }
      catch (mcrl2::runtime_error&)
      {
        // do not check multiple instances of predicate variables
      }
    }
    return result;
  }
#else
  // Check if the guards were correctly computed with respect to x.
  template <typename PbesRewriter>
  bool check_guards(const pbes_expression& /* x */, PbesRewriter /* R */) const
  {
    return true;
  }
#endif
};

inline
std::ostream& operator<<(std::ostream& out, const guard_expression& x)
{
  if (x.is_simple())
  {
    out << "condition = " << pbes_system::pp(x.condition) << std::endl;
  }
  else
  {
    for (std::vector<std::pair<propositional_variable_instantiation, pbes_expression> >::const_iterator i = x.guards.begin(); i != x.guards.end(); ++i)
    {
      out << pbes_system::pp(i->first) << " guard = " << pbes_system::pp(i->second) << std::endl;
    }
  }
  return out;
}

/// \brief Computes a multimap of propositional variable instantiations and the corresponding guards from a PBES expression
struct guard_traverser: public pbes_expression_traverser<guard_traverser>
{
  typedef pbes_expression_traverser<guard_traverser> super;
  using super::enter;
  using super::leave;
  using super::apply;

  simplify_data_rewriter<data::rewriter> R;
  std::vector<guard_expression> expression_stack;

  guard_traverser(const data::rewriter& r)
    : R(r)
  {}

  void push(const guard_expression& x)
  {
    mCRL2log(log::debug2) << "<push>" << "\n" << x << std::endl;
    expression_stack.push_back(x);
  }

  guard_expression& top()
  {
    return expression_stack.back();
  }

  const guard_expression& top() const
  {
    return expression_stack.back();
  }

  guard_expression pop()
  {
    guard_expression result = top();
    expression_stack.pop_back();
    return result;
  }

  void leave(const data::data_expression& x)
  {
    push(guard_expression(x));
    assert(top().check_guards(x, R));
  }

  void leave(const pbes_system::propositional_variable_instantiation& x)
  {
    guard_expression node;
    node.guards.push_back(std::make_pair(x, pbes_system::true_()));
    push(node);
    assert(top().check_guards(x, R));
  }

  void leave(const pbes_system::not_& x)
  {
    top().negate();
    assert(top().check_guards(x, R));
  }

  void leave(const pbes_system::and_& x)
  {
    guard_expression right = pop();
    guard_expression left = pop();
    pbes_expression new_condition = utilities::optimized_and(left.condition, right.condition);
    if (left.is_simple() && right.is_simple())
    {
      left.condition = new_condition;
      push(left);
    }
    else if (left.is_simple())
    {
      right.add_guard(left.condition);
      right.condition = new_condition;
      push(right);
    }
    else if (right.is_simple())
    {
      left.add_guard(right.condition);
      left.condition = new_condition;
      push(left);
    }
    else
    {
      left.guards.insert(left.guards.end(), right.guards.begin(), right.guards.end());
      left.condition = new_condition;
      push(left);
    }
    assert(top().check_guards(x, R));
  }

  void leave(const pbes_system::or_& x)
  {
    guard_expression right = pop();
    guard_expression left = pop();
    pbes_expression new_condition = utilities::optimized_or(left.condition, right.condition);
    if (left.is_simple() && right.is_simple())
    {
      left.condition = new_condition;
      push(left);
    }
    else if (left.is_simple())
    {
      right.add_guard(utilities::optimized_not(left.condition));
      right.condition = new_condition;
      push(right);
    }
    else if (right.is_simple())
    {
      left.add_guard(utilities::optimized_not(right.condition));
      left.condition = new_condition;
      push(left);
    }
    else
    {
      left.guards.insert(left.guards.end(), right.guards.begin(), right.guards.end());
      left.condition = new_condition;
      push(left);
    }
    assert(top().check_guards(x, R));
  }

  void leave(const pbes_system::imp& x)
  {
    guard_expression right = pop();
    guard_expression left = pop();
    left.negate();
    pbes_expression new_condition = utilities::optimized_or(left.condition, right.condition);
    if (left.is_simple() && right.is_simple())
    {
      left.condition = new_condition;
      push(left);
    }
    else if (left.is_simple())
    {
      right.add_guard(utilities::optimized_not(left.condition));
      right.condition = new_condition;
      push(right);
    }
    else if (right.is_simple())
    {
      left.add_guard(utilities::optimized_not(right.condition));
      left.condition = new_condition;
      push(left);
    }
    else
    {
      left.guards.insert(left.guards.end(), right.guards.begin(), right.guards.end());
      left.condition = new_condition;
      push(left);
    }
    assert(top().check_guards(x, R));
  }

  void leave(const pbes_system::forall& x)
  {
    // If x is a simple expression, quantifiers need to be handled differently!
    if (top().is_simple())
    {
      top().condition = x;
    }
    assert(top().check_guards(x, R));
  }

  void leave(const pbes_system::exists& x)
  {
    // If x is a simple expression, quantifiers need to be handled differently!
    if (top().is_simple())
    {
      top().condition = x;
    }
    assert(top().check_guards(x, R));
  }
};

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_GUARD_TRAVERSER_H
