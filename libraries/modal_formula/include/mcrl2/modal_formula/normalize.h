// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/normalize.h
/// \brief Add your file description here.

#ifndef MCRL2_MODAL_FORMULA_NORMALIZE_H
#define MCRL2_MODAL_FORMULA_NORMALIZE_H

#include "mcrl2/modal_formula/negate_variables.h"
#include "mcrl2/modal_formula/traverser.h"

namespace mcrl2
{

namespace state_formulas
{

/// \cond INTERNAL_DOCS
// \brief Visitor for checking if a state formula is normalized.
struct is_normalized_traverser: public state_formula_traverser<is_normalized_traverser>
{
  typedef state_formula_traverser<is_normalized_traverser> super;
  using super::enter;
  using super::leave;
  using super::apply;

  bool result;

  is_normalized_traverser()
    : result(true)
  {}

  /// \brief Visit not node
  void enter(const not_& /* x */)
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

template <typename T>
void normalize(T& x, bool negated = false, typename std::enable_if< !std::is_base_of< atermpp::aterm, T >::value>::type* = 0);

template <typename T>
T normalize(const T& x, bool negated = false, typename std::enable_if< std::is_base_of< atermpp::aterm, T >::value>::type* = nullptr);

// \brief Visitor for normalizing a state formula.
struct normalize_builder: public state_formula_builder<normalize_builder>
{
  typedef state_formula_builder<normalize_builder> super;
  using super::enter;
  using super::leave;
  using super::update;
  using super::apply;

  bool negated;

  normalize_builder(bool negated_)
    : negated(negated_)
  {}

  template <class T>
  void apply(T& result, const data::data_expression& x)
  {
    result = negated ? data::sort_bool::not_(x) : x;
  }

  template <class T>
  void apply(T& result, const true_& /*x*/)
  {
    if (negated)
    {
      result = false_();
    }
    else
    {
      result = true_();
    }
  }

  template <class T>
  void apply(T& result, const false_& /*x*/)
  {
    if (negated)
    {
      result = true_();
    }
    else
    {
      result = false_();
    }
  }

  template <class T>
  void apply(T& result, const not_& x)
  {
    result = normalize(x.operand(), !negated);
  }

  template <class T>
  void apply(T& result, const and_& x)
  {
    state_formula left = normalize(x.left(), negated);
    state_formula right = normalize(x.right(), negated);
    if (negated)
    {
      result = or_(left, right);
    }
    else
    {
      result = and_(left, right);
    }
  }

  template <class T>
  void apply(T& result, const or_& x)
  {
    state_formula left = normalize(x.left(), negated);
    state_formula right = normalize(x.right(), negated);
    if (negated)
    {
      result = and_(left, right);
    }
    else
    {
      result = or_(left, right);
    }
  }

  template <class T>
  void apply(T& result, const imp& x)
  {
    state_formula y = or_(not_(x.left()), x.right());
    result = normalize(y, negated);
  }

  template <class T>
  void apply(T& result, const forall& x)
  {
    if (negated)
    {
      result = exists(x.variables(), normalize(x.body(), true));
    }
    else
    {
      result = forall(x.variables(), normalize(x.body(), false));
    }
  }

  template <class T>
  void apply(T& result, const exists& x)
  {
    if (negated)
    {
      result = forall(x.variables(), normalize(x.body(), true));
    }
    else
    {
      result = exists(x.variables(), normalize(x.body(), false));
    }
  }

  template <class T>
  void apply(T& result, const variable& x)
  {
    if (negated)
    {
      throw mcrl2::runtime_error(std::string("normalize error: illegal argument ") + pp(x));
    }
    result = x;
  }

  template <class T>
  void apply(T& result, const must& x)
  {
    if (negated)
    {
      result = may(x.formula(), normalize(x.operand(), negated));
    }
    else
    {
      result = must(x.formula(), normalize(x.operand(), negated));
    }
  }

  template <class T>
  void apply(T& result, const may& x)
  {
    if (negated)
    {
      result = must(x.formula(), normalize(x.operand(), negated));
    }
    else
    {
      result = may(x.formula(), normalize(x.operand(), negated));
    }
  }

  template <class T>
  void apply(T& result, const mu& x)
  {
    if (negated)
    {
      result = nu(x.name(), x.assignments(), normalize(negate_variables(x.name(), x.operand()), true));
    }
    else
    {
      result = mu(x.name(), x.assignments(), normalize(x.operand(), false));
    }
  }

  template <class T>
  void apply(T& result, const nu& x)
  {
    if (negated)
    {
      result = mu(x.name(), x.assignments(), normalize(negate_variables(x.name(), x.operand()), true));
    }
    else
    {
      result = nu(x.name(), x.assignments(), normalize(x.operand(), false));
    }
  }

  template <class T>
  void apply(T& result, const delay& x)
  {
    result = x;
  }

  template <class T>
  void apply(T& result, const delay_timed& x)
  {
    result = x;
  }

  template <class T>
  void apply(T& result, const yaled& x)
  {
    result = x;
  }

  template <class T>
  void apply(T& result,  const yaled_timed& x)
  {
    result = x;
  }
};
/// \endcond

/// \brief Checks if a state formula is normalized.
/// \param x A PBES expression.
/// \return True if the state formula is normalized.
template <typename T>
bool is_normalized(const T& x)
{
  is_normalized_traverser f;
  f.apply(x);
  return f.result;
}

/// \brief The function normalize brings (embedded) state formulas into positive normal form,
/// i.e. a formula without any occurrences of ! or =>.
/// \param x an object containing state formulas.
/// \param negated Indication whether the formula must be interpreted as being negated.
template <typename T>
void normalize(T& x, bool negated, typename std::enable_if< !std::is_base_of< atermpp::aterm, T >::value>::type*)
{
  normalize_builder f(negated);
  f.update(x);
}

/// \brief The function normalize brings (embedded) state formulas into positive normal form,
/// i.e. a formula without any occurrences of ! or =>.
/// \param x an object containing state formulas
/// \param negated Indication whether the formula must be interpreted as being negated.
template <typename T>
T normalize(const T& x, bool negated, typename std::enable_if< std::is_base_of< atermpp::aterm, T >::value>::type*)
{
  T result;
  normalize_builder f(negated);
  f.apply(result, x);
  return result;
}

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_NORMALIZE_H
