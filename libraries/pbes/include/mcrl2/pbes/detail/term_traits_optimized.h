// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/term_traits_optimized.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_TERM_TRAITS_OPTIMIZED_H
#define MCRL2_PBES_DETAIL_TERM_TRAITS_OPTIMIZED_H

#include "mcrl2/pbes/pbes_expression.h"



namespace mcrl2::core {

/// \brief Contains type information for terms.
template <typename T>
struct term_traits_optimized
{
};

/// \brief Contains type information for pbes expressions.
template <>
struct term_traits_optimized<pbes_system::pbes_expression>: public core::term_traits<pbes_system::pbes_expression>
{
  using super = core::term_traits<pbes_system::pbes_expression>;

  static inline
  term_type not_(const term_type& x)
  {
    term_type result;
    data::optimized_not(result, x);
    return result;
  } 

  static inline
  void make_not_(term_type& result, const term_type& x)
  {
    data::optimized_not(result, x);
  }

  static inline
  term_type and_(const term_type& x, const term_type& y)
  {
    term_type result;
    data::optimized_and(result, x, y);
    return result;
  }

  static inline
  void make_and_(term_type& result, const term_type& x, const term_type& y)
  {
    data::optimized_and(result, x, y);
  }

  static inline
  term_type or_(const term_type& x, const term_type& y)
  {
    term_type result;
    data::optimized_or(result, x, y);
    return result;
  } 

  static inline
  void make_or_(term_type& result, const term_type& x, const term_type& y)
  {
    data::optimized_or(result, x, y);
  }

  static inline
  term_type imp(const term_type& x, const term_type& y)
  {
    term_type result;
    data::optimized_imp(result, x, y);
    return result;
  } 

  static inline
  void make_imp(term_type& result, const term_type& x, const term_type& y)
  {
    data::optimized_imp(result, x, y);
  }

  static inline
  void make_forall(term_type& result, const variable_sequence_type& d, const term_type& x)
  {
    data::optimized_forall_no_empty_domain(result, d, x);
  }

  static inline
  void make_exists(term_type& result, const variable_sequence_type& d, const term_type& x)
  {
    data::optimized_exists_no_empty_domain(result, d, x);
  }

  template <typename FwdIt>
  static inline
  term_type join_or(FwdIt first, FwdIt last)
  {
    return utilities::detail::join(first, last, or_, false_());
  }

  template <typename FwdIt>
  static inline
  term_type join_and(FwdIt first, FwdIt last)
  {
    return utilities::detail::join(first, last, and_, true_());
  }
};

} // namespace mcrl2::core



#endif // MCRL2_PBES_DETAIL_TERM_TRAITS_OPTIMIZED_H
