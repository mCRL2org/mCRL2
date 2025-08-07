// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/join.h
/// \brief Join and split functions for data expressions.

#ifndef MCRL2_DATA_JOIN_H
#define MCRL2_DATA_JOIN_H

#include "mcrl2/data/expression_traits.h"
#include "mcrl2/utilities/detail/join.h"

namespace mcrl2::data
{

  /// \brief Returns or applied to the sequence of data expressions [first, last)
  /// \param first Start of a sequence of data expressions
  /// \param last End of a sequence of of data expressions
  /// \return Or applied to the sequence of data expressions [first, last)
  template <typename FwdIt>
  data_expression join_or(FwdIt first, FwdIt last)
  {
    using tr = core::term_traits<data::data_expression>;
    return utilities::detail::join(first, last, tr::or_, tr::false_());
  }

  /// \brief Returns and applied to the sequence of data expressions [first, last)
  /// \param first Start of a sequence of data expressions
  /// \param last End of a sequence of of data expressions
  /// \return And applied to the sequence of data expressions [first, last)
  template <typename FwdIt>
  data_expression join_and(FwdIt first, FwdIt last)
  {
    using tr = core::term_traits<data::data_expression>;
    return utilities::detail::join(first, last, tr::and_, tr::true_());
  }

  /// \brief Splits a disjunction into a sequence of operands
  /// Given a data expression of the form p1 || p2 || .... || pn, this will yield a
  /// set of the form { p1, p2, ..., pn }, assuming that pi does not have a || as main
  /// function symbol.
  /// \param expr A data expression
  /// \return A sequence of operands
  inline
  std::set<data_expression> split_or(const data_expression& expr)
  {
    using tr = core::term_traits<data::data_expression>;
    std::set<data_expression> result;
    utilities::detail::split(expr, std::insert_iterator<std::set<data_expression> >(result, result.begin()), tr::is_or, tr::left, tr::right);
    return result;
  }

namespace detail
{
  /// \brief Splits a boolean term by rewriting it to the shape p1 || p2 || ... || pn or p1 &&...&& pn and then
  /// yield a set of the form { p1, p2, ..., pn } where pi does not have || or && anymore as main function symbol. 
  /// The second parameter indicates what the outermost symbol is.
  /// \param expr A data expression.
  /// \param outermost_symbol_is_or Indicates whether the outermost symbol is ||. If false it is &&.
  /// \return A set of data expressions of type bool.
  inline
  std::set<data_expression> split_aggressive(const data_expression& expr, bool outermost_symbol_is_or)
  {
    if ((outermost_symbol_is_or&&sort_bool::is_or_application(expr)) || (!outermost_symbol_is_or&&sort_bool::is_and_application(expr)))
    {
      const std::set<data_expression> left_set=split_aggressive(sort_bool::left(expr), outermost_symbol_is_or);
      std::set<data_expression> right_set=split_aggressive(sort_bool::right(expr), outermost_symbol_is_or);
      right_set.insert(left_set.begin(),left_set.end());
      return right_set;
    }
    if ((outermost_symbol_is_or&&sort_bool::is_and_application(expr)) || (!outermost_symbol_is_or&&sort_bool::is_or_application(expr)))
    {
      const std::set<data_expression> left_set=split_aggressive(sort_bool::left(expr), outermost_symbol_is_or);
      std::set<data_expression> right_set=split_aggressive(sort_bool::right(expr), outermost_symbol_is_or);
      std::set<data_expression> result;
      for(const data_expression& e1: left_set)
      {
        for(const data_expression& e2: right_set)
        {
          if (outermost_symbol_is_or)
          {
            result.insert(sort_bool::and_(e1,e2));
          }
          else
          {
            result.insert(sort_bool::or_(e1,e2));
          }
        }
      }
      return result;
    }
    if (sort_bool::is_not_application(expr))
    {
      std::set<data_expression> result1=split_aggressive(sort_bool::arg(expr), !outermost_symbol_is_or);
      std::set<data_expression> result2;
      for(const data_expression& e: result1)
      {
        if (sort_bool::is_not_application(e))
        {
          result2.insert(sort_bool::arg(e));
        }
        else
        {
          result2.insert(sort_bool::not_(e));
        }
      }
      return result2;
    }
    if (sort_bool::is_implies_application(expr))
    {
      return split_aggressive(sort_bool::or_(sort_bool::not_(sort_bool::left(expr)),sort_bool::right(expr)), outermost_symbol_is_or); 
    }
    // Maybe handle if on booleans, and <, <=, >= and > on booleans??
    std::set<data_expression> result;
    if (((expr==sort_bool::true_())&&!outermost_symbol_is_or) || ((expr==sort_bool::false_())&&outermost_symbol_is_or))
    {
      return result;
    }
    result.insert(expr);
    return result;
  }
} // namespace detail


  /// \brief Splits a boolean term by rewriting it to the shape p1 || p2 || ... || pn and then
  /// yield a set of the form { p1, p2, ..., pn } where pi does not have || anymore as main function symbol. 
  /// The result may be exponentially large.
  /// \param expr A data expression
  /// \return A sequence of operands
  inline
  std::set<data_expression> split_or_aggressive(const data_expression& expr)
  {
    constexpr bool outermost_symbol_is_or = true;
    return detail::split_aggressive(expr, outermost_symbol_is_or);
  }

  /// \brief Splits a boolean term by rewriting it to the shape p1 && p2 && ... && pn and then
  /// yield a set of the form { p1, p2, ..., pn } where pi does not have && anymore as main function symbol. 
  /// The result may be exponentially large. 
  /// \param expr A data expression
  /// \return A sequence of operands
  inline
  std::set<data_expression> split_and_aggressive(const data_expression& expr)
  {
    constexpr bool outermost_symbol_is_and = false;
    return detail::split_aggressive(expr, outermost_symbol_is_and);
  }


  /// \brief Splits a conjunction into a sequence of operands
  /// Given a data expression of the form p1 && p2 && .... && pn, this will yield a
  /// set of the form { p1, p2, ..., pn }, assuming that pi does not have a && as main
  /// function symbol.
  /// \param expr A data expression
  /// \return A sequence of operands
  inline
  std::set<data_expression> split_and(const data_expression& expr)
  {
    using tr = core::term_traits<data::data_expression>;
    std::set<data_expression> result;
    utilities::detail::split(expr, std::insert_iterator<std::set<data_expression> >(result, result.begin()), tr::is_and, tr::left, tr::right);
    return result;
  }

  } // namespace mcrl2::data

#endif // MCRL2_DATA_JOIN_H
