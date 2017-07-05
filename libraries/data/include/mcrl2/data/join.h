// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
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

namespace mcrl2
{

namespace data
{

  /// \brief Returns or applied to the sequence of data expressions [first, last)
  /// \param first Start of a sequence of data expressions
  /// \param last End of a sequence of of data expressions
  /// \return Or applied to the sequence of data expressions [first, last)
  template <typename FwdIt>
  data_expression join_or(FwdIt first, FwdIt last)
  {
    typedef core::term_traits<data::data_expression> tr;
    return utilities::detail::join(first, last, tr::or_, tr::false_());
  }

  /// \brief Returns and applied to the sequence of data expressions [first, last)
  /// \param first Start of a sequence of data expressions
  /// \param last End of a sequence of of data expressions
  /// \return And applied to the sequence of data expressions [first, last)
  template <typename FwdIt>
  data_expression join_and(FwdIt first, FwdIt last)
  {
    typedef core::term_traits<data::data_expression> tr;
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
    typedef core::term_traits<data::data_expression> tr;
    std::set<data_expression> result;
    utilities::detail::split(expr, std::insert_iterator<std::set<data_expression> >(result, result.begin()), tr::is_or, tr::left, tr::right);
    return result;
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
    typedef core::term_traits<data::data_expression> tr;
    std::set<data_expression> result;
    utilities::detail::split(expr, std::insert_iterator<std::set<data_expression> >(result, result.begin()), tr::is_and, tr::left, tr::right);
    return result;
  }

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_JOIN_H
