// Author(s): Luc Engelen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/prover/manipulator.h
/// \brief Interface to classes InternalFormatManipulator, AM_Jitty and AM_Inner

#ifndef MANIPULATOR_H
#define MANIPULATOR_H

#include <boost/signals2/detail/auto_buffer.hpp>
#include "mcrl2/data/detail/prover/info.h"

namespace mcrl2
{
namespace data
{
namespace detail
{

/// \brief Base class for classes that provide functionality to modify or create
/// \brief terms in one of the internal formats of the rewriter.

class InternalFormatManipulator
{
  protected:
    /// \brief The rewriter used to translate formulas to one of the internal formats of the rewriter.
    boost::shared_ptr<detail::Rewriter> f_rewriter;

    /// \brief A class that provides information on the structure of expressions in one of the
    /// \brief internal formats of the rewriter.
    InternalFormatInfo& f_info;

    /// \brief A table used by the method InternalFormatManipulator::orient.
    /// The method InternalFormatManipulator::orient stores resulting terms in this
    /// table. If a term is encountered that has already been processed, it is
    /// not processed again, but retreived from this table.
    std::map < data_expression, data_expression> f_orient;

    /// \brief aterm representing the \c if \c then \c else function in one of the internal formats of the rewriter.
    function_symbol f_if_then_else;

    /// \brief Replaces all occurences of \c a_guard in \c a_formula by \c true. Additionally, if the variable
    /// \brief on the righthand side of the guard is encountered in \c a_formula, it is replaced by the variable
    /// \brief on the lefthand side.
    data_expression set_true_auxiliary(
                const data_expression& a_formula, 
                const data_expression& a_guard,
                std::map < data_expression, data_expression >& f_set_true)
    {
      if (a_formula == f_rewriter->internal_true || a_formula == f_rewriter->internal_false)
      {
        return a_formula;
      }
      if (a_formula == a_guard)
      {
        return f_rewriter->internal_true;
      }

      bool v_is_equality = f_info.is_equality(a_guard);
      if (v_is_equality && a_guard[2] == a_formula)
      {
        return data_expression(a_guard[1]);
      }
      if (f_info.is_variable(a_formula))
      {
        return a_formula;
      }

      if (is_function_symbol(a_formula))
      {
        return a_formula;
      }

      std::map < data_expression, data_expression >::const_iterator i=f_set_true.find(a_formula);
      if (i!=f_set_true.end())
      {
        return i->second;
      }

      const application t(a_formula);
      data_expression_vector v_parts;
      for (application::const_iterator i = t.begin(); i!=t.end(); ++i)
      {
        v_parts.push_back(set_true_auxiliary(*i, a_guard,f_set_true));
      }
      data_expression v_result = application(set_true_auxiliary(t.head(), a_guard,f_set_true), v_parts);
      f_set_true[a_formula]=v_result;

      return v_result;
    }

    /// \brief Replaces all occurences of \c a_guard in \c a_formula by \c false.
    data_expression set_false_auxiliary(
              const data_expression& a_formula, 
              const data_expression& a_guard,
              std::map < data_expression, data_expression >& f_set_false)
    {
      if (a_formula == f_rewriter->internal_true || a_formula == f_rewriter->internal_false)
      {
        return a_formula;
      }
      if (a_formula == a_guard)
      {
        return f_rewriter->internal_false;
      }
      if (f_info.is_variable(a_formula))
      {
        return a_formula;
      }

      if (is_function_symbol(a_formula))
      {
        return a_formula;
      }

      std::map < data_expression, data_expression >::const_iterator i=f_set_false.find(a_formula);
      if (i!=f_set_false.end())
      {
        return i->second;
      }

      const application t(a_formula);
      data_expression_vector v_parts;
      for (application::const_iterator i = t.begin(); i!=t.end(); ++i)
      {
        v_parts.push_back(set_false_auxiliary(*i, a_guard,f_set_false));
      }
      data_expression v_result = application(set_false_auxiliary(t.head(), a_guard,f_set_false), v_parts);
      f_set_false[a_formula]=v_result;

      return v_result;
    }

    /// \brief Returns an expression in the internal format of the rewriter with the Jitty strategy.
    /// \brief The main operator of this expression is an \c if \c then \c else function. Its guard is \c a_expr,
    /// \brief the true-branch is \c a_high and the false-branch is \c a_low.
    data_expression make_if_then_else(
               const data_expression& a_expr, 
               const data_expression& a_high, 
               const data_expression& a_low)
    {
      return (data_expression)Apply3(f_if_then_else, a_expr, a_high, a_low);
    }

  public:
    /// \brief Constructor initializing the rewriter and the field \c f_info.
    InternalFormatManipulator(boost::shared_ptr<detail::Rewriter> a_rewriter, InternalFormatInfo& a_info):
      f_info(a_info)
    {
      f_rewriter = a_rewriter;
      f_if_then_else = static_cast<function_symbol>(a_rewriter->toRewriteFormat(if_(sort_bool::bool_())));
    }

    /// \brief Destructor with no particular functionality.
    ~InternalFormatManipulator()
    {
    }

    /// \brief Returns an expression in the internal format of the rewriter with the jitty strategy.
    /// \brief The main operator of this expression is an \c if \c then \c else function. Its guard is \c a_expr,
    /// \brief the true-branch is \c a_high and the false-branch is \c a_low. If \c a_high equals \c a_low, the
    /// \brief method returns \c a_high instead.
    data_expression make_reduced_if_then_else(const data_expression& a_expr, 
                                                  const data_expression& a_high, 
                                                  const data_expression& a_low)
    {
      if (a_high == a_low)
      {
        return a_high;
      }
      else
      {
        return make_if_then_else(a_expr, a_high, a_low);
      }
    }

    /// \brief Orients the term \c a_term such that all equations of the form t1 == t2 are
    /// \brief replaced by t2 == t1 if t1 > t2.
    data_expression orient(const data_expression& a_term)
    {
      if (is_variable(a_term))
      {
        return a_term;
      } 

      if (is_function_symbol(a_term))
      {
        return a_term;
      } 

      // v_result is NULL if not found; Therefore type aterm.
      std::map < data_expression, data_expression> :: const_iterator it=f_orient.find(a_term); 

      if (it!=f_orient.end())   // found
      {
        return it->second;
      }


      const atermpp::function_symbol& v_symbol = a_term.function();
      const data::function_symbol& v_function = atermpp::aterm_cast<data::function_symbol>(a_term[0]);
      size_t v_arity = v_symbol.arity();

      data_expression_vector v_parts;
      for (size_t i = 1; i < v_arity; i++)
      {
        v_parts.push_back(orient(data_expression(a_term[i])));
      }
      data_expression v_result = application(v_function, v_parts);

      if (f_info.is_equality(v_result))
      {
        data_expression v_term1 = atermpp::aterm_cast<data_expression>(v_result[1]);
        data_expression v_term2 = atermpp::aterm_cast<data_expression>(v_result[2]);
        if (f_info.compare_term(v_term1, v_term2) == compare_result_bigger)
        {
          v_result = application(data_expression(v_function), v_term2, v_term1);
        }
      }
      f_orient[a_term]=v_result;

      return v_result;
    }

    /// \brief Initializes the table InternalFormatManipulator::f_set_true and calls the method
    /// \brief AM_Jitty::f_set_true_auxiliary.
    data_expression set_true(
                 const data_expression& a_formula, 
                 const data_expression& a_guard)
    {
      std::map < data_expression, data_expression > f_set_true;
      return set_true_auxiliary(a_formula, a_guard, f_set_true);
    }

    /// \brief Initializes the table InternalFormatManipulator::f_set_false and calls the method
    /// \brief AM_Jitty::f_set_false_auxiliary.
    data_expression set_false(
                 const data_expression& a_formula, 
                 const data_expression& a_guard)
    {
      std::map < data_expression, data_expression > f_set_false;
      return set_false_auxiliary(a_formula, a_guard,f_set_false);
    }
}; 

}
}
}

#endif
