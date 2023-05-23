// Author(s): Jan Friso Groote. Based on pbes/rewriters/simplify_data_rewriter by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pres/rewriters/simplify_rewriter.h
/// \brief add your file description here.

#ifndef MCRL2_PRES_REWRITERS_SIMPLIFY_REWRITER_H
#define MCRL2_PRES_REWRITERS_SIMPLIFY_REWRITER_H

#include "mcrl2/data/standard.h"
#include "mcrl2/pres/rewriters/data_rewriter.h"

namespace mcrl2 {

namespace pres_system {

namespace detail {

template <template <class> class Builder, class Derived>
struct add_simplify: public Builder<Derived>
{
  typedef Builder<Derived> super;
  using super::apply;

  template <class T>
  void apply(T& result, const minus& x)
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
    if (is_minus(result))
    {
      result = atermpp::down_cast<minus>(result).operand();
      return;
    }
    make_minus(result, result);
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
    pres_expression right;
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
      right = data::sort_real::minimum(atermpp::down_cast<data::data_expression>(result),
                                       atermpp::down_cast<data::data_expression>(right));
      apply(result, right);
      return;
    }
    make_and_(result,result, right);
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
    pres_expression right;
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
      right = data::sort_real::maximum(atermpp::down_cast<data::data_expression>(result),
                                       atermpp::down_cast<data::data_expression>(right));
      apply(result, right);
      return;
    }
    make_or_(result,result, right);
  }

  template <class T>
  void apply(T& result, const plus& x)
  {
    assert(&result!=&x);  // Result is used as temporary store and cannot match x. 
    apply(result, x.left());
    if (is_true(result))
    {
      result = true_();
      return;
    }
    if (data::sort_real::is_zero(result))
    {
      apply(result, x.right());
      return;
    }
    pres_expression right;
    apply(right, x.right());
    if (is_true(right))
    {
      result = true_();
      return;
    }
    if (is_false(right))
    {
      result=right;
      return;
    }
    if (is_false(result) || data::sort_real::is_zero(right))
    {
      return;
    }
    if (data::is_data_expression(result) && data::is_data_expression(right))
    {
      right = data::sort_real::plus(atermpp::down_cast<data::data_expression>(result), 
                                    atermpp::down_cast<data::data_expression>(right));
      apply(result, right);
      return;
    }
    make_plus(result, result, right);
    
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
      if (is_minus(result))
      { 
        result = atermpp::down_cast<minus>(result).operand();
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
      make_minus(result, result);
      return;
    }
    pres_expression left;
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
  void apply(T& result, const minall& x)
  {
    apply(result, x.body());
    make_minall(result, x.variables(), result);
  }

  template <class T>
  void apply(T& result, const maxall& x)
  {
    apply(result, x.body());
    make_maxall(result, x.variables(), result);
  }

  template <class T>
  void apply(T& result, const const_multiply& x)
  {
    apply(result, x.left());
    if (data::sort_real::is_zero(result))
    {
      return;
    }
    if (data::sort_real::is_one(result))
    {
      apply(result, x.right());
      return;
    }
    pres_expression result_right;
    apply(result_right, x.right());
    make_const_multiply(result, result, result_right);
  }

  template <class T>
  void apply(T& result, const const_multiply_alt& x)
  {
    apply(result, x.right());
    if (data::sort_real::is_zero(result))
    {
      return;
    }
    if (data::sort_real::is_one(result))
    {
      apply(result, x.left());
      return;
    }
    pres_expression result_left;
    apply(result_left, x.left());
    make_const_multiply(result, result, result_left);
  }

  template <class T>
  void apply(T& result, const sum& x)
  {
    apply(result, x.body());
    make_sum(result, x.variables(), result);
  }

  template <class T>
  void apply(T& result, const condeq& x)
  {
    apply(result, x.arg1());  
    if (is_true(result))
    {
      apply(result, x.arg3());
      return;
    }
    if (is_false(result)) 
    {
      apply(result, and_(x.arg2(), x.arg3()));
      return;
    }
    if (data::is_application(result))
    {
      const data::application& d=atermpp::down_cast<data::application>(result);
      if (d.sort()==data::sort_real::real_())
      {
        apply(result, data::less(d, data::sort_real::real_zero()));   // NOTE: arg1() is sometimes ewritten twice.
     
        if (is_true(result))
        {
          apply(result, x.arg3());
          return;
        }
        if (is_false(result))
        {
          apply(result, and_(x.arg2(), x.arg3()));
          return;
        }
      }
    }

    pres_expression result1;
    pres_expression result2;
    apply(result1, x.arg2());
    apply(result2, x.arg3());
    make_condeq(result, result, result1, result2);
  }

  template <class T>
  void apply(T& result, const condsm& x)
  {

    apply(result, x.arg1());   
    if (is_true(result))
    {
      apply(result, or_(x.arg2(), x.arg3()));
      return;
    }
    if (is_false(result))
    {
      apply(result, x.arg2());
      return;
    }
    if (data::is_application(result))
    { 
      const data::application& d=atermpp::down_cast<data::application>(result);
      if (d.sort()==data::sort_real::real_())
      { 
        apply(result, less(data::sort_real::real_zero(), d));   // NOTE: arg1() is sometimes rewritten twice.
        
        if (is_true(result))
        { 
          apply(result, x.arg2());
          return;
        }
        if (is_false(result))
        { 
          apply(result, or_(x.arg2(), x.arg3()));
          return;
        }
      }
    }


    if (is_true(result))
    {
      apply(result, or_(x.arg2(), x.arg3()));
      return;
    }
    if (is_false(result)) 
    {
      apply(result, x.arg2());
      return;
    }
    pres_expression result1;
    pres_expression result2;
    apply(result, x.arg1());
    apply(result1, x.arg2());
    apply(result2, x.arg3());
    make_condsm(result, result, result1, result2);
  }

  template <class T>
  void apply(T& result, const eqinf& x)
  {
    apply(result, x.operand());
    if (is_true(result))
    {
      result = true_();
      return;
    }
    if (is_false(result))
    {
      result = true_();
      return;
    }
    if (is_eqinf(result))
    {
      return;
    }
    if (is_eqninf(result))
    {
      return;
    }
    if (data::is_application(result) && atermpp::down_cast<data::application>(result).sort()==data::sort_real::real_())
    {
      result = false_();
      return;
    }
    make_eqinf(result, result);
  }

  template <class T>
  void apply(T& result, const eqninf& x)
  {
    apply(result, x.operand());
    if (is_true(result))
    {
      result = true_();
      return;
    }
    if (is_false(result))
    {
      result = true_();
      return;
    }
    if (is_eqinf(result))
    {
      return;
    }
    if (is_eqninf(result))
    {
      return;
    }
    if (data::is_application(result) && atermpp::down_cast<data::application>(result).sort()==data::sort_real::real_())
    {
      result = true_();
      return;
    }
    make_eqinf(result, result);
  }

};

template <typename Derived>
struct simplify_builder: public add_simplify<pres_system::pres_expression_builder, Derived>
{ };

template <typename Derived, typename DataRewriter, typename SubstitutionFunction>
struct simplify_data_rewriter_builder : public add_data_rewriter < pres_system::detail::simplify_builder, Derived, DataRewriter, SubstitutionFunction >
{
  typedef add_data_rewriter < pres_system::detail::simplify_builder, Derived, DataRewriter, SubstitutionFunction > super;
  simplify_data_rewriter_builder(const DataRewriter& R, SubstitutionFunction& sigma)
    : super(R, sigma)
  {}
};

} // namespace detail

/// \brief A rewriter that simplifies boolean expressions in a term.
struct simplify_rewriter
{
  typedef pres_expression term_type;
  typedef data::variable variable_type;

  pres_expression operator()(const pres_expression& x) const
  {
    pres_expression result;
    core::make_apply_builder<detail::simplify_builder>().apply(result, x);
    return result;
  }

  void operator()(pres_expression& result, const pres_expression& x) const
  {
    core::make_apply_builder<detail::simplify_builder>().apply(result, x);
  }
};

/// \brief A rewriter that simplifies boolean expressions in a term, and rewrites data expressions.
template <typename DataRewriter>
struct simplify_data_rewriter
{
  typedef pres_expression term_type;
  typedef data::variable variable_type;

  const DataRewriter& R;

  explicit simplify_data_rewriter(const DataRewriter& R_)
    : R(R_)
  {}

  pres_expression operator()(const pres_expression& x) const
  {
    data::no_substitution sigma;
    pres_expression result;
    detail::make_apply_rewriter_builder<pres_system::detail::simplify_data_rewriter_builder>(R, sigma).apply(result,x);
    return result;
  }

  template <typename SubstitutionFunction>
  pres_expression operator()(const pres_expression& x, SubstitutionFunction& sigma) const
  {
    pres_expression result;
    detail::make_apply_rewriter_builder<pres_system::detail::simplify_data_rewriter_builder>(R, sigma).apply(result, x);
    return result;
  }

  template <typename SubstitutionFunction>
  void operator()(pres_expression& result, const pres_expression& x, SubstitutionFunction& sigma) const
  {
    detail::make_apply_rewriter_builder<pres_system::detail::simplify_data_rewriter_builder>(R, sigma).apply(result, x);
  }
};

} // namespace pres_system

} // namespace mcrl2

#endif // MCRL2_PRES_REWRITERS_SIMPLIFY_REWRITER_H
