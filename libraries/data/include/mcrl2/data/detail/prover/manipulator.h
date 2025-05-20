// Author(s): Luc Engelen
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/prover/manipulator.h
/// \brief Interface to classes Manipulator.

#ifndef MCRL2_DATA_DETAIL_PROVER_MANIPULATOR_H
#define MCRL2_DATA_DETAIL_PROVER_MANIPULATOR_H

#include "mcrl2/data/detail/prover/info.h"

namespace mcrl2
{
namespace data
{
namespace detail
{

/// \brief Base class for classes that provide functionality to modify or create terms.

class Manipulator
{
  protected:
    /// \brief A class that provides information on the structure of expressions in one of the
    /// \brief internal formats of the rewriter.
    const Info& f_info;

    /// \brief A table used by the method Manipulator::orient.
    /// The method Manipulator::orient stores resulting terms in this
    /// table. If a term is encountered that has already been processed, it is
    /// not processed again, but retreived from this table.
    std::unordered_map < data_expression, data_expression> f_orient;

    /// \brief Replaces all occurences of \c a_guard in \c a_formula by \c true. Additionally, if the variable
    /// \brief on the righthand side of the guard is encountered in \c a_formula, it is replaced by the variable
    /// \brief on the lefthand side.
    data_expression set_true_auxiliary(
                const data_expression& a_formula,
                const data_expression& a_guard,
                std::unordered_map < data_expression, data_expression >& f_set_true) const
    {
      if (is_machine_number(a_formula))
      {
        return a_formula;
      }

      if (is_function_symbol(a_formula))
      {
        return a_formula;
      }

      if (is_abstraction(a_formula))
      {
        const abstraction& t=atermpp::down_cast<abstraction>(a_formula);
        return abstraction(t.binding_operator(), t.variables(), set_true_auxiliary(t.body(), a_guard, f_set_true));
      }

      if (a_formula == a_guard)
      {
        return sort_bool::true_();
      }

      if (is_equal_to_application(a_guard))
      {
        const application& a = atermpp::down_cast<application>(a_guard);
        if (a[1]==a_formula)
        {
          // We can be sure that a[1] does not occur in a[0], due to the ordering on terms
          // Therefore, there are no never-ending substitutions occuring
          return a[0];
        }
      }

      if (is_variable(a_formula))
      {
        return a_formula;
      }

      std::unordered_map < data_expression, data_expression >::const_iterator i=f_set_true.find(a_formula);
      if (i!=f_set_true.end())
      {
        return i->second;
      }

      const application& t=atermpp::down_cast<application>(a_formula);
      data_expression v_result = application(t.head(),
                                             t.begin(),
                                             t.end(),
                                             [&a_guard, &f_set_true, this](const data_expression& d){ return set_true_auxiliary(d, a_guard,f_set_true);});

      f_set_true[a_formula]=v_result;

      return v_result;
    }

    /// \brief Replaces all occurences of \c a_guard in \c a_formula by \c false.
    data_expression set_false_auxiliary(
              const data_expression& a_formula,
              const data_expression& a_guard,
              std::unordered_map < data_expression, data_expression >& f_set_false) const
    {
      if (is_machine_number(a_formula))
      {
        return a_formula;
      }

      if (is_function_symbol(a_formula))
      {
        return a_formula;
      }

      if (is_abstraction(a_formula))
      {
        const abstraction& t=atermpp::down_cast<abstraction>(a_formula);
        return abstraction(t.binding_operator(), t.variables(), set_false_auxiliary(t.body(), a_guard, f_set_false));
      }

      if (a_formula == a_guard)
      {
        return sort_bool::false_();
      }

      if (is_variable(a_formula))
      {
        return a_formula;
      }

      std::unordered_map < data_expression, data_expression >::const_iterator i=f_set_false.find(a_formula);
      if (i!=f_set_false.end())
      {
        return i->second;
      }

      const application t(a_formula);
      data_expression v_result = application(t.head(),
                                             t.begin(),
                                             t.end(),
                                             [&a_guard, &f_set_false, this](const data_expression& d){ return set_false_auxiliary(d, a_guard,f_set_false);});
      f_set_false[a_formula]=v_result;
      return v_result;
    }

  public:
    /// \brief Constructor initializing the rewriter and the field \c f_info.
    Manipulator(const Info& a_info):
      f_info(a_info)
    {}

    /// \brief Returns an expression in the internal format of the rewriter with the jitty strategy.
    /// \brief The main operator of this expression is an \c if \c then \c else function. Its guard is \c a_expr,
    /// \brief the true-branch is \c a_high and the false-branch is \c a_low. If \c a_high equals \c a_low, the
    /// \brief method returns \c a_high instead.
    static data_expression make_reduced_if_then_else(const data_expression& a_expr,
                                                  const data_expression& a_high,
                                                  const data_expression& a_low)
    {
      return a_high == a_low ? a_high : if_(a_expr, a_high, a_low);
    }

    /// \brief Orients the term \c a_term such that all equations of the form t1 == t2 are
    /// \brief replaced by t2 == t1 if t1 > t2.
    data_expression orient(const data_expression& a_term)
    {
      if (is_variable(a_term) || is_function_symbol(a_term) || is_where_clause(a_term) || is_machine_number(a_term))
      {
        return a_term;
      }

      if (is_abstraction(a_term))
      {
        const abstraction a(atermpp::down_cast<abstraction>(a_term));
        return abstraction(a.binding_operator(), a.variables(), orient(a.body()));
      }

      std::unordered_map < data_expression, data_expression> :: const_iterator it=f_orient.find(a_term);
      if (it!=f_orient.end())   // found
      {
        return it->second;
      }

      const application& a = atermpp::down_cast<application>(a_term);
      application v_result(a.head(), a.begin(), a.end(), [this](const data_expression& d){return orient(d); });

      if (is_equal_to_application(v_result))
      {
        const data_expression& v_term1(v_result[0]);
        const data_expression& v_term2(v_result[1]);
        if (f_info.compare_term(v_term1, v_term2) == compare_result_bigger)
        {
          v_result = application(v_result.head(), v_term2, v_term1);
        }
      }
      f_orient[a_term]=v_result;

      return v_result;
    }

    /// \brief Initializes the table Manipulator::f_set_true and calls
    /// \brief f_set_true_auxiliary.
    data_expression set_true(
                 const data_expression& a_formula,
                 const data_expression& a_guard) const
    {
      std::unordered_map < data_expression, data_expression > f_set_true;
      return set_true_auxiliary(a_formula, a_guard, f_set_true);
    }

    /// \brief Initializes the table Manipulator::f_set_false and calls the method
    /// \brief AM_Jitty::f_set_false_auxiliary.
    data_expression set_false(
                 const data_expression& a_formula,
                 const data_expression& a_guard) const
    {
      std::unordered_map < data_expression, data_expression > f_set_false;
      return set_false_auxiliary(a_formula, a_guard,f_set_false);
    }
};

} // namespace detail
} // namespace data
} // namespace mcrl2

#endif
