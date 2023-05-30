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
    apply(result, static_cast<pres_expression>(x.left()));
    if (data::sort_real::is_zero(result))
    {
      throw mcrl2::runtime_error("Constant in const_multiply cannot be zero: " + pp(static_cast<pres_expression>(x)) +".");
      return;
    }
    if (data::sort_real::is_one(result))
    {
      apply(result, x.right());
      return;
    }
    pres_expression result_right;
    apply(result_right, x.right());
    if (is_true(result_right) || is_false(result_right) || is_eqinf(result_right) || is_eqninf(result_right))
    {
      result = result_right;
      return;
    }
    if (is_const_multiply(result_right))
    {
      optimized_const_multiply(result, 
                               data::sort_real::times(atermpp::down_cast<data::data_expression>(result), 
                                                      atermpp::down_cast<const_multiply>(result_right).left()),
                               atermpp::down_cast<const_multiply>(result_right).right());
      return;
    }
    if (is_const_multiply_alt(result_right))
    {
      optimized_const_multiply(result, 
                               data::sort_real::times(atermpp::down_cast<data::data_expression>(result), 
                                                      atermpp::down_cast<const_multiply_alt>(result_right).right()),
                               atermpp::down_cast<const_multiply_alt>(result_right).left());
      return;
    }
    if (data::is_data_expression(result_right) && atermpp::down_cast<data::data_expression>(result_right).sort()==data::sort_real::real_())
    {
      data::sort_real::make_times(atermpp::reference_cast<data::data_expression>(result), 
                                  atermpp::down_cast<data::data_expression>(result), 
                                  atermpp::down_cast<data::data_expression>(result_right));
      return;
    }
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
    /* if (data::is_data_application(result))
    {
      const data::application& d=atermpp::down_cast<data::application>(result);
      if (d.sort()==data::sort_real::real_())
      {
        pres_expression aux;
        apply(aux, static_cast<pres_expression>(data::less(d, data::sort_real::real_zero())));   // NOTE: arg1() is sometimes ewritten twice.
     
        if (is_true(aux))
        {
          apply(result, x.arg3());
          return;
        }
        if (is_false(aux))
        {
          apply(result, and_(x.arg2(), x.arg3()));
          return;
        }
      }
    } */

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
    /* if (data::is_application(result))
    { 
      const data::application& d=atermpp::down_cast<data::application>(result);
      if (d.sort()==data::sort_real::real_())
      { 
        data::data_expression aux;
        apply(aux, static_cast<pres_expression>(less(data::sort_real::real_zero(), d)));   // NOTE: arg1() is sometimes rewritten twice.
        
        if (is_true(aux))
        { 
          apply(result, x.arg2());
          return;
        }
        if (is_false(aux))
        { 
          apply(result, or_(x.arg2(), x.arg3()));
          return;
        }
      }
    } */

    pres_expression result1;
    pres_expression result2;
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
    if (is_and(result))
    {  
       const and_& resulta = atermpp::down_cast<and_>(result);
       pres_expression t1, t2;
       make_eqinf(t1, resulta.left());
       apply(t2, t1);
       make_eqinf(t1, resulta.right());
       apply(result, t1);
       optimized_and(t1, t2, result);
       result = t1;
       return;
    }
    if (is_or(result))
    {  
       const or_& resulta = atermpp::down_cast<or_>(result);
       pres_expression t1, t2;
       make_eqinf(t1, resulta.left());
       apply(t2, t1);
       make_eqinf(t1, resulta.right());
       apply(result, t1);
       optimized_or(t1, t2, result);
       result = t1;
       return;
    }
    if (is_plus(result))
    {  
       const plus& resulta = atermpp::down_cast<plus>(result);
       pres_expression t1, t2;
       make_eqinf(t1, resulta.left());
       apply(t2, t1);
       make_eqinf(t1, resulta.right());
       apply(result, t1);
       optimized_plus(t1, t2, result);
       result = t1;
       return;
    }
    if (is_const_multiply(result))
    {  
       const const_multiply& resulta = atermpp::down_cast<const_multiply>(result);
       pres_expression t1;
       make_eqinf(t1, resulta.right());
       apply(result, t1);
       return;
    }
    if (is_const_multiply_alt(result))
    {  
       const const_multiply_alt& resulta = atermpp::down_cast<const_multiply_alt>(result);
       pres_expression t1;
       make_eqinf(t1, resulta.left());
       apply(result, t1);
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
    if (is_and(result))
    {
       const and_& resulta = atermpp::down_cast<and_>(result);
       pres_expression t1, t2;
       make_eqninf(t1, resulta.left());
       apply(t2, t1);
       make_eqninf(t1, resulta.right());
       apply(result, t1);
       optimized_and(t1, t2, result);
       result = t1;
       return;
    }
    if (is_or(result))
    {
       const or_& resulta = atermpp::down_cast<or_>(result);
       pres_expression t1, t2;
       make_eqninf(t1, resulta.left());
       apply(t2, t1);
       make_eqninf(t1, resulta.right());
       apply(result, t1);
       optimized_or(t1, t2, result);
       result = t1;
       return;
    }
    if (is_plus(result))
    {
       const plus& resulta = atermpp::down_cast<plus>(result);
       pres_expression t, t1, t2, t3, t4;
       make_eqninf(t, resulta.left());
       apply(t1, t);
       make_eqinf(t, resulta.right());
       apply(t2, t);
       make_eqinf(t, resulta.left());
       apply(t3, t);
       make_eqninf(t, resulta.right());
       apply(t4, t);
       optimized_or(t, t1, t2);
       optimized_or(t1, t3, t4);
       optimized_and(result, t, t1);
       return;
    }
    if (is_const_multiply(result))
    {
       const const_multiply& resulta = atermpp::down_cast<const_multiply>(result);
       pres_expression t1;
       make_eqninf(t1, resulta.right());
       apply(result, t1);
       return;
    }
    if (is_const_multiply_alt(result))
    {
       const const_multiply_alt& resulta = atermpp::down_cast<const_multiply_alt>(result);
       pres_expression t1;
       make_eqninf(t1, resulta.left());
       apply(result, t1);
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
struct simplify_data_rewriter_builder : public mcrl2::pres_system::detail::add_data_rewriter < pres_system::detail::simplify_builder, Derived, DataRewriter, SubstitutionFunction >
{
  typedef add_data_rewriter < pres_system::detail::simplify_builder, Derived, DataRewriter, SubstitutionFunction > super;

  using super::apply;

  simplify_data_rewriter_builder(const DataRewriter& R, SubstitutionFunction& sigma)
    : super(R, sigma)
  {}

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
    if (data::is_data_expression(result))
    {
      const data::data_expression& d=atermpp::down_cast<data::data_expression>(result);
      if (d.sort()==data::sort_real::real_())
      {
        pres_expression aux;
        apply(aux, static_cast<pres_expression>(data::less(d, data::sort_real::real_zero())));   // NOTE: arg1() is sometimes ewritten twice.
     
        if (is_true(aux))
        {
          apply(result, x.arg3());
          return;
        }
        if (is_false(aux))
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
    if (data::is_data_expression(result))
    { 
      const data::data_expression& d=atermpp::down_cast<data::data_expression>(result);
      if (d.sort()==data::sort_real::real_())
      { 
        pres_expression aux;
        apply(aux, static_cast<pres_expression>(less(data::sort_real::real_zero(), d)));   // NOTE: arg1() is sometimes rewritten twice.
        
        if (is_true(aux))
        { 
          apply(result, x.arg2());
          return;
        }
        if (is_false(aux))
        { 
          apply(result, or_(x.arg2(), x.arg3()));
          return;
        }
      }
    } 
  }

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
