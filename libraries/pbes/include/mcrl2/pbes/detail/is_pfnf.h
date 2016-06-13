// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/is_pfnf.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_IS_PFNF_H
#define MCRL2_PBES_DETAIL_IS_PFNF_H

#include "mcrl2/pbes/join.h"
#include "mcrl2/pbes/pbes_expression.h"
#include "mcrl2/pbes/traverser.h"
#include "mcrl2/pbes/pbes_functions.h"
#include "mcrl2/data/optimized_boolean_operators.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

/// \brief Splits a conjunction into a sequence of operands
/// Given a pbes expression of the form p1 && p2 && ... && pn, this will yield a
/// vector of the form [ p1, p2, ..., pn ], assuming that pi does not have a && as main
/// function symbol.
/// \param expr A PBES expression
/// \return A sequence of operands
inline
void split_and(const pbes_expression& expr, std::vector<pbes_expression>& result)
{
  using namespace accessors;
  utilities::detail::split(expr, std::back_inserter(result), is_and, left, right);
}

/// \brief Splits a conjunction into a sequence of operands
/// Given a pbes expression of the form p1 || p2 || ... || pn, this will yield a
/// vector of the form [ p1, p2, ..., pn ], assuming that pi does not have a || as main
/// function symbol.
/// \param expr A PBES expression
/// \return A sequence of operands
inline
void split_or(const pbes_expression& expr, std::vector<pbes_expression>& result)
{
  using namespace accessors;
  utilities::detail::split(expr, std::back_inserter(result), is_or, left, right);
}

inline
bool is_pfnf_simple_expression(const pbes_expression& x)
{
  return is_simple_expression(x) || data::is_data_expression(x);
}

inline
bool is_pfnf_data_expression(const pbes_expression& x)
{
  return is_true(x) || is_false(x) || data::is_data_expression(x);
}

inline
bool is_pfnf_or_expression(const pbes_expression& x)
{
  bool result;
  if (is_or(x))
  {
    result = is_pfnf_or_expression(or_(x).left()) && is_pfnf_or_expression(or_(x).right());
  }
  else
  {
    result = is_propositional_variable_instantiation(x);
  }
  return result;
}

// Determines if an expression has the format \/_{j in Ji} Xj(ej)
inline
bool is_pfnf_or(const pbes_expression& x)
{
  return is_true(x) || is_pfnf_or_expression(x);
}

// Determines if an expression has the format g_i => x with is_pfnf_or(x)
inline
bool is_pfnf_imp(const pbes_expression& x)
{
  bool result;
  if (is_pfnf_simple_expression(x))
  {
    result = true;
  }
  else if (is_imp(x))
  {
    result = is_pfnf_simple_expression(imp(x).left()) && is_pfnf_or(imp(x).right());
  }
  else
  {
    result = is_pfnf_or(x);
  }
  return result;
}

// Determines if an expression has the format /\_{i in I} x_i with is_pfnf_imp(x_i)
inline
bool is_pfnf_inner_and(const pbes_expression& x)
{
  bool result = true;
  for (const pbes_expression& term: split_and(x))
  {
    if (!is_pfnf_imp(term))
    {
      result = false;
    }
  }
  return result;
}

// Determines if an expression has the format h /\ x with is_pfnf_inner_and(x), where the 'h /\' part is optional
inline
bool is_pfnf_outer_and(const pbes_expression& x)
{
  bool result = true;

  std::vector<pbes_expression> v;
  split_and(x, v);
  std::size_t simple_count = 0;
  for (std::vector<pbes_expression>::iterator i = v.begin(); i != v.end(); ++i)
  {
    if (is_pfnf_inner_and(x))
    {
      continue;
    }
    else if (is_pfnf_simple_expression(x))
    {
      simple_count++;
      if (simple_count > 1)
      {
        result = false;
      }
    }
    else
    {
      result = false;
    }
  }
  return result;
}

// Determines if an expression is in pfnf format
inline
bool is_pfnf_expression(const pbes_expression& x)
{
  if (is_forall(x))
  {
    return is_pfnf_expression(forall(x).body());
  }
  else if (is_exists(x))
  {
    return is_pfnf_expression(exists(x).body());
  }
  else
  {
    return is_pfnf_outer_and(x);
  }
}

struct is_pfnf_traverser: public pbes_expression_traverser<is_pfnf_traverser>
{
  typedef pbes_expression_traverser<is_pfnf_traverser> super;
  using super::enter;
  using super::leave;
  using super::apply;

  bool result;

  is_pfnf_traverser()
    : result(true)
  {}

  void apply(const pbes_expression& x)
  {
    result = is_pfnf_expression(x) && result;
  }
};

template <typename T>
bool is_pfnf(const T& x)
{
  is_pfnf_traverser f;
  f.apply(x);
  return f.result;
}

inline
pbes_expression remove_quantifiers(const pbes_expression& x)
{
  pbes_expression y = x;

  // strip the quantifiers
  while (is_forall(y) || is_exists(y))
  {
    if (is_forall(y))
    {
      y = forall(y).body();
    }
    else if (is_exists(y))
    {
      y = exists(y).body();
    }
  }
  return y;
}

// returns the implications /\_{i in I} x_i
// \pre x is in PFNF format
inline
std::vector<pbes_expression> pfnf_implications(const pbes_expression& x)
{
  pbes_expression y = remove_quantifiers(x);

  std::vector<pbes_expression> result;
  split_and(y, result);

  // eliminate simple expressions from result
  result.erase(std::remove_if(result.begin(), result.end(), &is_pfnf_simple_expression), result.end());

  return result;
}

// extracts the guards h and [g_i | i in I] from a pbes expression in PFNF format
inline
void split_pfnf_expression(const pbes_expression& phi, pbes_expression& h, std::vector<pbes_expression>& g)
{
  pbes_expression x = remove_quantifiers(phi);
  std::vector<pbes_expression> v;
  split_and(x, v);
  h = true_();
  for (pbes_expression& expr: v)
  {
    if (is_pfnf_simple_expression(expr))
    {
      h = data::optimized_and(h, expr);
    }
    else
    {
      g.push_back(expr);
    }
  }
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_IS_PFNF_H
