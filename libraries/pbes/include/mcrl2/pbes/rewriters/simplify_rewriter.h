// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/rewriters/simplify_rewriter.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_REWRITERS_SIMPLIFY_REWRITER_H
#define MCRL2_PBES_REWRITERS_SIMPLIFY_REWRITER_H

#include "mcrl2/data/detail/enumerator_identifier_generator.h"
#include "mcrl2/data/replace_capture_avoiding_with_an_identifier_generator.h"
#include "mcrl2/pbes/rewriters/data_rewriter.h"

namespace mcrl2::pbes_system {

namespace detail {

template <template <class> class Builder, class Derived>
struct add_simplify: public Builder<Derived>
{
  using super = Builder<Derived>;
  using super::apply;

  template <class T>
  void apply(T& result, const not_& x)
  {
    assert(&result!=&x);  // Result is used as temporary store and cannot match x. 
    apply(result, x.operand());
    if (is_false(result))
    {
      result = true_();
      return;
    }
    if (is_true(result))
    {
      result = false_();
      return;
    }
    if (is_not(result))
    {
      result = atermpp::down_cast<not_>(result).operand();
      return;
    }
    make_not_(result, result);
  }

  template <class T>
  void apply(T& result, const and_& x)
  {
    assert(&result!=&x);  // Result is used as temporary store and cannot match x. 
    apply(result, x.left());
    if (is_false(result))
    {
      result = false_();
      return;
    }
    if (is_true(result))
    {
      apply(result, x.right());
      return;
    }
    pbes_expression right;
    apply(right, x.right());
    if (is_false(right))
    {
      result = false_();
      return;
    }
    if (is_true(right) || result==right)
    {
      return;
    }
    if (data::is_data_expression(result) && data::is_data_expression(right)) 
    {
      data::sort_bool::make_and_(atermpp::assign_cast<data::data_expression>(result), 
                                     atermpp::down_cast<data::data_expression>(result), 
                                     atermpp::down_cast<data::data_expression>(right));
      return;
    }
    make_and_(result, result, right);
  }

  template <class T>
  void apply(T& result, const or_& x)
  {
    assert(&result!=&x);  // Result is used as temporary store and cannot match x. 
    apply(result, x.left());
    if (is_true(result))
    {
      result = true_();
      return;
    }
    if (is_false(result))
    {
      apply(result, x.right());
      return;
    }
    pbes_expression right;
    apply(right, x.right());
    if (is_true(right))
    {
      result = true_();
      return;
    }
    if (is_false(right) || result==right)
    {
      return;
    }
    if (data::is_data_expression(result) && data::is_data_expression(right)) 
    {
      data::sort_bool::make_or_(atermpp::assign_cast<data::data_expression>(result), 
                                    atermpp::down_cast<data::data_expression>(result), 
                                    atermpp::down_cast<data::data_expression>(right));
      return;
    }
    make_or_(result, result, right);
  }

  template <class T>
  void apply(T& result, const imp& x)
  {
    assert(&result!=&x);  // Result is used as temporary store and cannot match x. 
    if (is_false(x.left()))  // This test is cheap. 
    {
      result = true_();
      return;
    }
    apply(result, x.right());
    if (is_true(result))
    { 
      result = true_();
      return;
    }
    if (is_false(result))
    { 
      apply(result, x.left());
      if (is_not(result))
      { 
        result = atermpp::down_cast<not_>(result).operand();
        return;
      }
      if (is_true(result))
      {
        result = false_();
        return;
      }
      if (is_false(result))
      {
        result = true_();
        return;
      }
      make_not_(result, result);
      return;
    }
    pbes_expression left;
    apply(left, x.left());
    if (is_true(left))
    { 
      return;
    }
    if (is_false(left) || result==left)
    { 
      result = true_();
      return;
    }
    make_imp(result, left, result);
  }

  template <class T>
  void apply(T& result, const forall& x)
  {
    apply(result, x.body());
    data::optimized_forall(result, x.variables(), result, true);
  }

  template <class T>
  void apply(T& result, const exists& x)
  {
    apply(result, x.body());
    data::optimized_exists(result, x.variables(), result, true);
  }
};

template <typename Derived>
struct simplify_builder: public add_simplify<pbes_system::pbes_expression_builder, Derived>
{ };

template <typename Derived, typename DataRewriter, typename Substitution>
struct simplify_data_rewriter_builder : public add_data_rewriter<pbes_system::detail::simplify_builder, Derived, DataRewriter, Substitution>
{ 
  using super = add_data_rewriter<pbes_system::detail::simplify_builder, Derived, DataRewriter, Substitution>;
  using super::enter;
  using super::leave;
  using super::apply;

  using substitution_administration_type = data::detail::substitution_updater_with_an_identifier_generator<Substitution,
                                                                  data::enumerator_identifier_generator>;
  substitution_administration_type substitution_administration;

  simplify_data_rewriter_builder(const DataRewriter& R, Substitution& sigma)
    : super(R, sigma),
      substitution_administration(sigma, const_cast<data::enumerator_identifier_generator&>(R.identifier_generator()))
  {}

  Substitution& substitution()
  {
    substitution_administration.substitution();
  }

  template <class T>
  void apply(T& result, const forall& x)
  {
    const data::variable_list vl = substitution_administration.push(x.variables());
    super::apply(result, x.body());
    data::optimized_forall(result, vl, result, true);
    substitution_administration.pop(vl);
  }

  template <class T>
  void apply(T& result, const exists& x)
  {
    const data::variable_list vl = substitution_administration.push(x.variables());
    super::apply(result, x.body());
    data::optimized_exists(result, vl, result, true);
    substitution_administration.pop(vl);
  }
};

template <template <class, class, class> class Builder, class DataRewriter, class MutableSubstitution>
struct apply_data_rewrite_builder: public Builder<apply_data_rewrite_builder<Builder, DataRewriter, MutableSubstitution>, DataRewriter, MutableSubstitution>
{
  using super = Builder<apply_data_rewrite_builder<Builder, DataRewriter, MutableSubstitution>, DataRewriter, MutableSubstitution>;
  using super::enter;
  using super::leave;

  apply_data_rewrite_builder(const DataRewriter& R, MutableSubstitution& sigma, data::enumerator_identifier_generator& id_generator)
    : super(R, sigma, id_generator)
  {}
};

template <template <class, class, class> class Builder, class DataRewriter, class MutableSubstitution>
apply_data_rewrite_builder<Builder, DataRewriter, MutableSubstitution>
make_apply_data_rewrite_builder(const DataRewriter& R, MutableSubstitution& sigma, data::enumerator_identifier_generator& id_generator)
{
  return apply_data_rewrite_builder<Builder, DataRewriter, MutableSubstitution>(R, sigma, id_generator);
}

} // namespace detail

/// \brief A rewriter that simplifies boolean expressions in a term.
struct simplify_rewriter
{
  using term_type = pbes_expression;
  using variable_type = data::variable;

  pbes_expression operator()(const pbes_expression& x) const
  {
    pbes_expression result;
    core::make_apply_builder<detail::simplify_builder>().apply(result, x);
    return result;
  }

  void operator()(pbes_expression& result, const pbes_expression& x) const
  {
    core::make_apply_builder<detail::simplify_builder>().apply(result, x);
  }
};

/// \brief A rewriter that simplifies boolean expressions in a term, and rewrites data expressions.
template <typename DataRewriter>
struct simplify_data_rewriter
{
  using term_type = pbes_expression;
  using variable_type = data::variable;

  const DataRewriter& R;

  explicit simplify_data_rewriter(const DataRewriter& R_)
    : R(R_)
  {}

  pbes_expression operator()(const pbes_expression& x) const
  {
    data::no_substitution sigma;
    pbes_expression result;
    detail::make_apply_rewriter_builder<pbes_system::detail::simplify_data_rewriter_builder>(R, sigma).apply(result,x);
    return result;
  }

  template <typename Substitution>
  pbes_expression operator()(const pbes_expression& x, Substitution& sigma) const
  {
    pbes_expression result;
    detail::make_apply_rewriter_builder<pbes_system::detail::simplify_data_rewriter_builder>(R, sigma).apply(result, x);
    return result;
  }

  template <typename Substitution>
  void operator()(pbes_expression& result, const pbes_expression& x, Substitution& sigma) const
  {
    detail::make_apply_rewriter_builder<pbes_system::detail::simplify_data_rewriter_builder>(R, sigma).apply(result, x);
  }
};

} // namespace mcrl2::pbes_system



#endif // MCRL2_PBES_REWRITERS_SIMPLIFY_REWRITER_H
