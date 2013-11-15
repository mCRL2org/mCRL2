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
    InternalFormatInfo &f_info;

    /// \brief A table used by the method InternalFormatManipulator::orient.
    /// The method InternalFormatManipulator::orient stores resulting terms in this
    /// table. If a term is encountered that has already been processed, it is
    /// not processed again, but retreived from this table.
    std::map < atermpp::aterm_appl, atermpp::aterm_appl> f_orient;

    /// \brief aterm representing the \c if \c then \c else function in one of the internal formats of the rewriter.
    function_symbol f_if_then_else;

    /// \brief Replaces all occurences of \c a_guard in \c a_formula by \c true. Additionally, if the variable
    /// \brief on the righthand side of the guard is encountered in \c a_formula, it is replaced by the variable
    /// \brief on the lefthand side.
    atermpp::aterm_appl set_true_auxiliary(
                const atermpp::aterm_appl &a_formula, 
                const atermpp::aterm_appl &a_guard,
                std::map < atermpp::aterm_appl, atermpp::aterm_appl > &f_set_true)
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
        return atermpp::aterm_appl(a_guard[1]);
      }
      if (f_info.is_variable(a_formula))
      {
        return a_formula;
      }

      std::map < atermpp::aterm_appl, atermpp::aterm_appl >::const_iterator i=f_set_true.find(a_formula);
      if (i!=f_set_true.end())
      {
        return i->second;
      }

      atermpp::aterm v_function;

      atermpp::function_symbol v_symbol = a_formula.function();
      v_function = a_formula[0];
      size_t v_arity = v_symbol.arity();

      std::vector<atermpp::aterm> v_parts(v_arity);
      assert(v_arity>0);
      v_parts[0] = v_function;
      for (size_t i = 1; i < v_arity; i++)
      {
        v_parts[i] = set_true_auxiliary(atermpp::aterm_appl(a_formula[i]), a_guard,f_set_true);
      }
      atermpp::aterm_appl v_result = atermpp::aterm_appl(v_symbol, v_parts.begin(),v_parts.end());
      f_set_true[a_formula]=v_result;

      return v_result;
    }

    /// \brief Replaces all occurences of \c a_guard in \c a_formula by \c false.
    atermpp::aterm_appl set_false_auxiliary(
              const atermpp::aterm_appl &a_formula, 
              const atermpp::aterm_appl &a_guard,
              std::map < atermpp::aterm_appl, atermpp::aterm_appl > &f_set_false)
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

      std::map < atermpp::aterm_appl, atermpp::aterm_appl >::const_iterator i=f_set_false.find(a_formula);
      if (i!=f_set_false.end())
      {
        return i->second;
      }

      atermpp::aterm v_function;
      size_t v_arity;

      atermpp::function_symbol v_symbol = a_formula.function();
      v_function = a_formula[0];
      v_arity = v_symbol.arity();

      std::vector <atermpp::aterm> v_parts(v_arity);
      v_parts[0] = v_function;
      for (size_t i = 1; i < v_arity; i++)
      {
        v_parts[i] = set_false_auxiliary(atermpp::aterm_appl(a_formula[i]), a_guard,f_set_false);
      }
      atermpp::aterm_appl v_result = atermpp::aterm_appl(v_symbol, v_parts.begin(),v_parts.end());
      f_set_false[a_formula]=v_result;

      return v_result;
    }

    /// \brief Returns an expression in the internal format of the rewriter with the Jitty strategy.
    /// \brief The main operator of this expression is an \c if \c then \c else function. Its guard is \c a_expr,
    /// \brief the true-branch is \c a_high and the false-branch is \c a_low.
    atermpp::aterm_appl make_if_then_else(
               const atermpp::aterm_appl &a_expr, 
               const atermpp::aterm_appl &a_high, 
               const atermpp::aterm_appl &a_low)
    {
      return (atermpp::aterm_appl)Apply3(f_if_then_else, a_expr, a_high, a_low);
    }

  public:
    /// \brief Constructor initializing the rewriter and the field \c f_info.
    InternalFormatManipulator(boost::shared_ptr<detail::Rewriter> a_rewriter, InternalFormatInfo &a_info):
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
    atermpp::aterm_appl make_reduced_if_then_else(const atermpp::aterm_appl &a_expr, 
                                                  const atermpp::aterm_appl &a_high, 
                                                  const atermpp::aterm_appl &a_low)
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
    atermpp::aterm_appl orient(const atermpp::aterm_appl &a_term)
    {
      if (is_variable(a_term))
      {
        return a_term;
      } 

      // v_result is NULL if not found; Therefore type aterm.
      std::map < atermpp::aterm_appl, atermpp::aterm_appl> :: const_iterator it=f_orient.find(a_term); 

      if (it!=f_orient.end())   // found
      {
        return it->second;
      }


      const atermpp::function_symbol &v_symbol = a_term.function();
      const atermpp::aterm &v_function = a_term[0];
      size_t v_arity = v_symbol.arity();

      typedef boost::signals2::detail::auto_buffer<atermpp::aterm, boost::signals2::detail::store_n_objects<64> > vector_t;
      vector_t v_parts;
      v_parts.push_back(v_function);
      for (size_t i = 1; i < v_arity; i++)
      {
        v_parts.push_back(orient(atermpp::aterm_appl(a_term[i])));
      }
      atermpp::aterm_appl v_result = atermpp::aterm_appl(v_symbol, v_parts.begin(),v_parts.end());

      if (f_info.is_equality(v_result))
      {
        atermpp::aterm_appl v_term1 = atermpp::aterm_cast<atermpp::aterm_appl>(v_result[1]);
        atermpp::aterm_appl v_term2 = atermpp::aterm_cast<atermpp::aterm_appl>(v_result[2]);
        if (f_info.compare_term(v_term1, v_term2) == compare_result_bigger)
        {
          v_result = atermpp::aterm_appl(v_symbol, v_function, v_term2, v_term1);
        }
      }
      f_orient[a_term]=v_result;

      return v_result;
    }

    /// \brief Initializes the table InternalFormatManipulator::f_set_true and calls the method
    /// \brief AM_Jitty::f_set_true_auxiliary.
    atermpp::aterm_appl set_true(
                 const atermpp::aterm_appl &a_formula, 
                 const atermpp::aterm_appl &a_guard)
    {
      std::map < atermpp::aterm_appl, atermpp::aterm_appl > f_set_true;
      return set_true_auxiliary(a_formula, a_guard, f_set_true);
    }

    /// \brief Initializes the table InternalFormatManipulator::f_set_false and calls the method
    /// \brief AM_Jitty::f_set_false_auxiliary.
    atermpp::aterm_appl set_false(
                 const atermpp::aterm_appl &a_formula, 
                 const atermpp::aterm_appl &a_guard)
    {
      std::map < atermpp::aterm_appl, atermpp::aterm_appl > f_set_false;
      return set_false_auxiliary(a_formula, a_guard,f_set_false);
    }
};

}
}
}

#endif
