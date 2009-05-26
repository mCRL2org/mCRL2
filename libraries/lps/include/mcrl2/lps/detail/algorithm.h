// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file algorithm.h
/// \brief Add your file description here.

#ifndef MCRL2_LPS_DETAIL_ALGORITHM_H
#define MCRL2_LPS_DETAIL_ALGORITHM_H

#include <string>
#include <iterator>
#include <vector>
#include <set>
#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/data/data.h"
#include "mcrl2/data/real.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/data/set_identifier_generator.h"
#include "mcrl2/lps/linear_process.h"

namespace mcrl2 {

namespace lps {

namespace detail {

/// \brief Adds a time parameter t to s if needed and returns the result. The time t
/// is chosen such that it doesn't appear in context.
template <typename IdentifierGenerator>
struct make_timed_lps_summand
{
  IdentifierGenerator& m_generator;

  make_timed_lps_summand(IdentifierGenerator& generator)
    : m_generator(generator)
  {}

  /// \brief Function call operator
  /// \param summand_ A linear process summand
  /// \return The result of the function
  summand operator()(summand summand_) const
  {
    if (!summand_.has_time())
    {
      data::variable v(m_generator("T"), data::sort_real_::real_());
      summand_ = set_time(summand_, data::data_expression(v));
      data::variable_list V(summand_.summation_variables());
      V = push_front(V, v);
      summand_ = set_summation_variables(summand_, V);
    }
    return summand_;
  }
};

/// \brief Adds time parameters to the lps if needed and returns the result.
/// The times are chosen such that they don't appear in context.
/// \param lps A linear process
/// \param context A term
/// \return A timed linear process
inline
linear_process make_timed_lps(const linear_process& lps, atermpp::aterm context)
{
  data::set_identifier_generator generator(context);
  summand_list new_summands = atermpp::apply(lps.summands(), make_timed_lps_summand<data::set_identifier_generator>(generator));
  linear_process result = lps;
  result.set_summands(new_summands);
  return result;
}

/// \brief Function object that can be used by the partial_replace algorithm
/// to replace data variables in an arbitrary term.
template <typename SrcList, typename DestList>
struct variable_replacer
{
  const SrcList& src_;
  const DestList& dest_;

  variable_replacer(const SrcList& src, const DestList& dest)
    : src_(src), dest_(dest)
  {
    assert(src_.size() == dest_.size());
  }

  /// \brief Function call operator
  /// \param t A term
  /// \return The function result
  std::pair<atermpp::aterm_appl, bool> operator()(atermpp::aterm_appl t) const
  {
    if (!data::data_expression(t).is_variable())
    {
      return std::pair<atermpp::aterm_appl, bool>(t, true); // continue the recursion
    }
    typename SrcList::const_iterator i = src_.begin();
    typename DestList::const_iterator j = dest_.begin();
    for (; i != src_.end(); ++i, ++j)
    {
      if (t == *i)
      {
        return std::pair<atermpp::aterm_appl, bool>(*j, false); // don't continue the recursion
      }
    }
    return std::pair<atermpp::aterm_appl, bool>(t, false); // don't continue the recursion
  }
};

/// \brief Utility function for creating a variable_replacer.
/// \param t1 A term
/// \param t2 A term
/// \return A variable_replacer
template <typename T1, typename T2>
variable_replacer<T1, T2> make_variable_replacer(const T1& t1, const T2& t2)
{
  return variable_replacer<T1, T2>(t1, t2);
}

/// \brief Function object that can be used by the partial_replace algorithm
/// to replace the names of data variables in an arbitrary term.
template <typename SrcList, typename DestList>
struct variable_name_replacer
{
  const SrcList& src_;
  const DestList& dest_;

  variable_name_replacer(const SrcList& src, const DestList& dest)
    : src_(src), dest_(dest)
  {
    assert(src_.size() == dest_.size());
  }

  /// \brief Function call operator
  /// \param t A term
  /// \return The function result
  std::pair<atermpp::aterm_appl, bool> operator()(atermpp::aterm_appl t) const
  {
    if (!data::data_expression(t).is_variable())
    {
      return std::pair<atermpp::aterm_appl, bool>(t, true); // continue the recursion
    }
    data::variable v(t);
    typename SrcList::const_iterator i = src_.begin();
    typename DestList::const_iterator j = dest_.begin();
    for (; i != src_.end(); ++i, ++j)
    {
      if (v.name() == *i)
      {
        return std::pair<atermpp::aterm_appl, bool>(data::variable(*j, v.sort()), false); // don't continue the recursion
      }
    }
    return std::pair<atermpp::aterm_appl, bool>(t, false); // don't continue the recursion
  }
};

/// \brief Utility function for creating a variable_name_replacer.
/// \param t1 A term
/// \param t2 A term
/// \return A variable_replacer
template <typename T1, typename T2>
variable_name_replacer<T1, T2> make_variable_name_replacer(const T1& t1, const T2& t2)
{
  return variable_name_replacer<T1, T2>(t1, t2);
}

} // namespace detail

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_DETAIL_ALGORITHM_H
