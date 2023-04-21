// Author(s): Jan Friso Groote. Based on pbes/normalize.h by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pres/normalize.h
/// \brief Normalization of pres expressions.

#ifndef MCRL2_PRES_NORMALIZE_H
#define MCRL2_PRES_NORMALIZE_H

#include "mcrl2/data/consistency.h"
#include "mcrl2/pres/builder.h"
#include "mcrl2/pres/traverser.h"

namespace mcrl2
{

namespace pres_system
{

/// \cond INTERNAL_DOCS
// \brief Visitor for checking if a pres expression is normalized.
struct is_normalized_traverser: public pres_expression_traverser<is_normalized_traverser>
{
  typedef pres_expression_traverser<is_normalized_traverser> super;
  using super::enter;
  using super::leave;
  using super::apply;

  bool result;

  is_normalized_traverser()
    : result(true)
  {}

  /// \brief Visit not node
  void enter(const minus& /* x */)
  {
    result = false;
  }

  /// \brief Visit imp node
  void enter(const imp& /* x */)
  {
    result = false;
  }
};
/// \endcond

/// \cond INTERNAL_DOCS
// \brief Visitor for checking if a pres expression is normalized.
struct normalize_builder: public pres_expression_builder<normalize_builder>
{
  typedef pres_expression_builder<normalize_builder> super;
  using super::apply;

  bool negated;

  normalize_builder()
    : negated(false)
  {}

  template <class T>
  void apply(T& result, const data::data_expression& x)
  {
    result = negated ? data::not_(x) : x;
  }

  template <class T>
  void apply(T& result, const minus& x)
  {
    negated = !negated;
    super::apply(result, x.operand());
    negated = !negated;
  }

  template <class T>
  void apply(T& result, const and_& x)
  {
    pres_expression left;
    super::apply(left, x.left());
    pres_expression right;
    super::apply(right, x.right());
    if (negated)
    {
      make_or_(result, left, right);
    }
    else
    {
      make_and_(result, left, right);
    }
  }

  template <class T>
  void apply(T& result, const or_& x)
  {
    pres_expression left;
    super::apply(left, x.left());
    pres_expression right;
    super::apply(right, x.right());
    if (negated)
    {
      make_and_(result, left, right);
    }
    else
    {
      make_or_(result, left, right);
    }
  }

  template <class T>
  void apply(T& result, const imp& x)
  {
    negated = !negated;
    pres_expression left;
    super::apply(left, x.left());
    negated = !negated;
    pres_expression right;
    super::apply(right, x.right());
    if (negated)
    {
      make_and_(result, left, right);
    }
    else
    {
      make_or_(result, left, right);
    }
  }

  template <class T>
  void apply(T& result, const minall& x)
  {
    pres_expression body;
    super::apply(body, x.body());
    result = negated ? make_maxall(x.variables(), body) : make_minall(x.variables(), body);
  }

  template <class T>
  void apply(T& result, const maxall& x)
  {
    pres_expression body;
    super::apply(body, x.body());
    result = negated ? make_minall(x.variables(), body) : make_maxall(x.variables(), body);
  }

  template <class T>
  void apply(T& result, const propositional_variable_instantiation& x)
  {
    if (negated)
    {
      throw mcrl2::runtime_error(std::string("normalize error: illegal argument ") + pp(x));
    }
    result = x;
  }
};
/// \endcond

/// \brief Checks if a pres expression is normalized
/// \param x A PRES expression
/// \return True if the pres expression is normalized
template <typename T>
bool is_normalized(const T& x)
{
  is_normalized_traverser f;
  f.apply(x);
  return f.result;
}

/// \brief The function normalize brings (embedded) pres expressions into positive normal form,
/// i.e. a formula without any occurrences of ! or =>.
/// \param x an object containing pres expressions
template <typename T>
void normalize(T& x,
               typename std::enable_if< !std::is_base_of< atermpp::aterm, T >::value>::type* = nullptr
              )
{
  normalize_builder f;
  f.update(x);
}

/// \brief The function normalize brings (embedded) pres expressions into positive normal form,
/// i.e. a formula without any occurrences of ! or =>.
/// \param x an object containing pres expressions
template <typename T>
T normalize(const T& x,
            typename std::enable_if< std::is_base_of< atermpp::aterm, T >::value>::type* = nullptr
           )
{
  T result;
  normalize_builder f;
  f.apply(result, x);
  return result;
}

} // namespace pres_system

} // namespace mcrl2

#endif // MCRL2_PRES_NORMALIZE_H
