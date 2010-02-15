// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/detail/optimized_logic_operators.h
/// \brief Generic implementation of optimized logic operators.

#ifndef MCRL2_CORE_DETAIL_OPTIMIZED_LOGIC_OPERATORS_H
#define MCRL2_CORE_DETAIL_OPTIMIZED_LOGIC_OPERATORS_H

namespace mcrl2 {

namespace core {

namespace detail {

  /// \brief Make a negation
  /// \param arg A term
  /// \param not_ The operation not
  /// \param true_ The value true
  /// \param is_true Function that tests for the value true
  /// \param false_ The value false
  /// \param is_false Function that tests for the value false
  /// \return The value <tt>!arg</tt>
  template <typename T1, typename T2, typename UnaryFunction, typename UnaryPredicate>
  inline
  T1 optimized_not(T1 arg, UnaryFunction not_, T2 true_, UnaryPredicate is_true, T2 false_, UnaryPredicate is_false)
  {
    if(is_true(arg))
      return false_;
    else if(is_false(arg))
      return true_;
    else
      return not_(arg);
  }

  /// \brief Make a conjunction
  /// \param left A term
  /// \param right A term
  /// \param and_ The operation and
  /// \param true_ The value true
  /// \param is_true Function that tests for the value true
  /// \param false_ The value false
  /// \param is_false Function that tests for the value false
  /// \return The value <tt>left && right</tt>
  template <typename T1, typename T2, typename UnaryPredicate, typename BinaryFunction>
  inline
  T1 optimized_and(T1 left, T1 right, BinaryFunction and_, T2 true_, UnaryPredicate is_true, T2 false_, UnaryPredicate is_false)
  {
    if(is_true(left))
      return right;
    else if(is_false(left))
      return false_;
    else if(is_true(right))
      return left;
    else if(is_false(right))
      return false_;
    else if (left == right)
      return left;
    else
      return and_(left, right);
  }

  /// \brief Make a disjunction
  /// \param left A term
  /// \param right A term
  /// \param or_ The operation or
  /// \param true_ The value true
  /// \param is_true Function that tests for the value true
  /// \param false_ The value false
  /// \param is_false Function that tests for the value false
  /// \return The value <tt>left || right</tt>
  template <typename T1, typename T2, typename UnaryPredicate, typename BinaryFunction>
  inline
  T1 optimized_or(T1 left, T1 right, BinaryFunction or_, T2 true_, UnaryPredicate is_true, T2 false_, UnaryPredicate is_false)
  {
    if(is_true(left))
      return true_;
    else if(is_false(left))
      return right;
    else if(is_true(right))
      return true_;
    else if(is_false(right))
      return left;
    else if (left == right)
      return left;
    else
      return or_(left, right);
  }

  /// \brief Make an implication
  /// \param left A term
  /// \param right A term
  /// \param imp The implication operator
  /// \param not_ The operation not
  /// \param true_ The value true
  /// \param is_true Function that tests for the value true
  /// \param false_ The value false
  /// \param is_false Function that tests for the value false
  /// \return The value <tt>left => right</tt>
  template <typename T1, typename T2, typename UnaryPredicate, typename UnaryFunction, typename BinaryFunction>
  inline
  T1 optimized_imp(T1 left, T1 right, BinaryFunction imp, UnaryFunction not_, T2 true_, UnaryPredicate is_true, T2 false_, UnaryPredicate is_false)
  {
    if(is_true(left))
      return right;
    else if(is_false(left))
      return true_;
    else if(is_true(right))
      return true_;
    else if(is_false(right))
      return not_(left);
    else if (left == right)
      return true_;
    else
      return imp(left, right);
  }

  /// \brief Make a universal quantification
  /// \param v A sequence of variables
  /// \param arg A term
  /// \param forall The universal quantification operator
  /// \param true_ The value true
  /// \param is_true Function that tests for the value true
  /// \param false_ The value false
  /// \param is_false Function that tests for the value false
  /// \return The universal quantification <tt>forall v.arg</tt>
  template <typename T1, typename T2, typename VariableSequence, typename UnaryPredicate, typename Forall>
  inline
  T1 optimized_forall(VariableSequence v, T1 arg, Forall forall, T2 true_, UnaryPredicate is_true, T2 false_, UnaryPredicate is_false)
  {
    if(is_true(arg))
      return true_;
    else if(is_false(arg))
      return false_;
    else
      return forall(v, arg);
  }

  /// \brief Make an existential quantification
  /// \param v A sequence of variables
  /// \param arg A term
  /// \param exists The existential quantification operator
  /// \param true_ The value true
  /// \param is_true Function that tests for the value true
  /// \param false_ The value false
  /// \param is_false Function that tests for the value false
  /// \return The existential quantification <tt>exists v.arg</tt>
  template <typename T1, typename T2, typename VariableSequence, typename UnaryPredicate, typename Exists>
  inline
  T1 optimized_exists(VariableSequence v, T1 arg, Exists exists, T2 true_, UnaryPredicate is_true, T2 false_, UnaryPredicate is_false)
  {
    if(is_true(arg))
      return true_;
    else if(is_false(arg))
      return false_;
    else
      return exists(v, arg);
  }

} // namespace detail

} // namespace core

} // namespace mcrl2

#endif // MCRL2_CORE_DETAIL_OPTIMIZED_LOGIC_OPERATORS_H
