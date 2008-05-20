// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING).
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

  template <typename T, typename UnaryFunction, typename UnaryPredicate>
  T optimized_not(T arg, UnaryFunction not_, T true_, UnaryPredicate is_true, T false_, UnaryPredicate is_false)
  {
    if(is_true(arg))
      return false_;
    else if(is_false(arg))
      return true_;
    else
      return not_(arg);
  }

  template <typename T, typename UnaryPredicate, typename BinaryFunction>
  T optimized_and(T left, T right, BinaryFunction and_, T true_, UnaryPredicate is_true, T false_, UnaryPredicate is_false)
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

  template <typename T, typename UnaryPredicate, typename BinaryFunction>
  T optimized_or(T left, T right, BinaryFunction or_, T true_, UnaryPredicate is_true, T false_, UnaryPredicate is_false)
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

  template <typename T, typename UnaryPredicate, typename UnaryFunction, typename BinaryFunction>
  T optimized_imp(T left, T right, BinaryFunction imp, UnaryFunction not_, T true_, UnaryPredicate is_true, T false_, UnaryPredicate is_false)
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

} // namespace detail

} // namespace core

} // namespace mcrl2

#endif // MCRL2_CORE_DETAIL_OPTIMIZED_LOGIC_OPERATORS_H
