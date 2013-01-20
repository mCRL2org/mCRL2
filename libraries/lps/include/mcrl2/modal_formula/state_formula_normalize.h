// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/state_formula_normalize.h
/// \brief Add your file description here.

#ifndef MCRL2_MODAL_STATE_FORMULA_NORMALIZE_H
#define MCRL2_MODAL_STATE_FORMULA_NORMALIZE_H

#include "mcrl2/modal_formula/state_formula.h"
#include "mcrl2/modal_formula/detail/state_variable_negator.h"
#include "mcrl2/modal_formula/detail/state_formula_accessors.h"
#include "mcrl2/data/bool.h"

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
  using super::operator();

#if BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif

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
void normalize(T& x, bool negated = false, typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0);

template <typename T>
T normalize(const T& x, bool negated = false, typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type* = 0);

// \brief Visitor for checking if a state formula is normalized.
struct normalize_builder: public state_formula_builder<normalize_builder>
{
  typedef state_formula_builder<normalize_builder> super;
  using super::enter;
  using super::leave;
  using super::operator();

#if BOOST_MSVC
#include "mcrl2/core/detail/builder_msvc.inc.h"
#endif

  bool negated;

  normalize_builder(bool negated_)
    : negated(negated_)
  {}

  state_formula operator()(const data::data_expression& x)
  {
    return negated ? data::sort_bool::not_(x) : x;
  }

  state_formula operator()(const true_& /*x*/)
  {
    if (negated)
    {
      return false_();
    }
    else
    {
      return true_();
    }
  }

  state_formula operator()(const false_& /*x*/)
  {
    if (negated)
    {
      return true_();
    }
    else
    {
      return false_();
    }
  }

  state_formula operator()(const not_& x)
  {
    return normalize(x.operand(), !negated);
  }

  state_formula operator()(const and_& x)
  {
    state_formula left = normalize(x.left(), negated);
    state_formula right = normalize(x.right(), negated);
    if (negated)
    {
      return or_(left, right);
    }
    else
    {
      return and_(left, right);
    }
  }

  state_formula operator()(const or_& x)
  {
    state_formula left = normalize(x.left(), negated);
    state_formula right = normalize(x.right(), negated);
    if (negated)
    {
      return and_(left, right);
    }
    else
    {
      return or_(left, right);
    }
  }

  state_formula operator()(const imp& x)
  {
    state_formula y = or_(not_(x.left()), x.right());
    return normalize(y, negated);
  }

  state_formula operator()(const forall& x)
  {
    if (negated)
    {
      return exists(x.variables(), normalize(x.body(), true));
    }
    else
    {
      return forall(x.variables(), normalize(x.body(), false));
    }
  }

  state_formula operator()(const exists& x)
  {
    if (negated)
    {
      return forall(x.variables(), normalize(x.body(), true));
    }
    else
    {
      return exists(x.variables(), normalize(x.body(), false));
    }
  }

  state_formula operator()(const variable& x)
  {
    if (negated)
    {
      throw mcrl2::runtime_error(std::string("normalize error: illegal argument ") + to_string(x));
    }
    return x;
  }

  state_formula operator()(const must& x)
  {
    return must(x.formula(), normalize(x.operand(), negated));
  }

  state_formula operator()(const may& x)
  {
    return may(x.formula(), normalize(x.operand(), negated));
  }

  state_formula operator()(const mu& x)
  {
    if (negated)
    {
      return mu(x.name(), x.assignments(), normalize(detail::negate_propositional_variable(x.name(), x.operand()), true));
    }
    else
    {
      return mu(x.name(), x.assignments(), normalize(x.operand(), false));
    }
  }

  state_formula operator()(const nu& x)
  {
    if (negated)
    {
      return nu(x.name(), x.assignments(), normalize(detail::negate_propositional_variable(x.name(), x.operand()), true));
    }
    else
    {
      return nu(x.name(), x.assignments(), normalize(x.operand(), false));
    }
  }

  state_formula operator()(const delay& x)
  {
    return x;
  }

  state_formula operator()(const delay_timed& x)
  {
    return x;
  }

  state_formula operator()(const yaled& x)
  {
    return x;
  }

  state_formula operator()(const yaled_timed& x)
  {
    return x;
  }
};
/// \endcond

/// \brief Checks if a state formula is normalized
/// \param t A PBES expression
/// \return True if the state formula is normalized
template <typename T>
bool is_normalized(const T& x)
{
  is_normalized_traverser f;
  f(x);
  return f.result;
}

/// \brief The function normalize brings (embedded) state formulas into positive normal form,
/// i.e. a formula without any occurrences of ! or =>.
/// \param x an object containing state formulas
template <typename T>
void normalize(T& x, bool negated, typename boost::disable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type*)
{
  normalize_builder f(negated);
  f(x);
}

/// \brief The function normalize brings (embedded) state formulas into positive normal form,
/// i.e. a formula without any occurrences of ! or =>.
/// \param x an object containing state formulas
template <typename T>
T normalize(const T& x, bool negated, typename boost::enable_if<typename boost::is_base_of<atermpp::aterm, T>::type>::type*)
{
  normalize_builder f(negated);
  return f(x);
}

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_STATE_FORMULA_NORMALIZE_H
