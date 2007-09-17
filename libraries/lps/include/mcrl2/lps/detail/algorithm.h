// Author(s): Wieger Wesselink
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
#include "atermpp/aterm.h"
#include "atermpp/aterm_list.h"
#include "atermpp/algorithm.h"
#include "mcrl2/data/data.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/data/set_identifier_generator.h"
#include "mcrl2/lps/linear_process.h"

namespace lps {
  
namespace detail {

using atermpp::aterm_appl;
using atermpp::aterm_list;

/// Adds a time parameter t to s if needed and returns the result. The time t
/// is chosen such that it doesn't appear in context.
struct make_timed_lps_summand
{
  fresh_variable_generator& m_generator;

  make_timed_lps_summand(fresh_variable_generator& generator)
    : m_generator(generator)
  {}

  summand operator()(summand summand_) const
  {
    if (!summand_.has_time())
    {
      data_variable v = m_generator();
      summand_ = set_time(summand_, data_expression(v));
      summand_ = set_summation_variables(summand_, summand_.summation_variables() + v);
    }
    return summand_;
  }
};

/// Adds time parameters to the lps if needed and returns the result. The times
/// are chosen such that they don't appear in context.
inline
linear_process make_timed_lps(linear_process lps, aterm context)
{
  fresh_variable_generator generator(context);
  summand_list new_summands = apply(lps.summands(), make_timed_lps_summand(generator));
  return set_summands(lps, new_summands);
}

/// Function object that can be used by the partial_replace algorithm
/// to replace data variables in an arbitrary term.
template <typename SrcList, typename DestList>
struct data_variable_replacer
{
  const SrcList& src_;
  const DestList& dest_;
  
  data_variable_replacer(const SrcList& src, const DestList& dest)
    : src_(src), dest_(dest)
  {
    assert(src_.size() == dest_.size());
  }
  
  std::pair<aterm_appl, bool> operator()(aterm_appl t) const
  {
    if (!is_data_variable(t))
    {
      return std::pair<aterm_appl, bool>(t, true); // continue the recursion
    }
    typename SrcList::const_iterator i = src_.begin();
    typename DestList::const_iterator j = dest_.begin();
    for (; i != src_.end(); ++i, ++j)
    {
      if (t == *i)
      {
        return std::pair<aterm_appl, bool>(*j, false); // don't continue the recursion
      }
    }
    return std::pair<aterm_appl, bool>(t, false); // don't continue the recursion
  }
};

/// Utility function for creating a data_variable_replacer.
template <typename T1, typename T2>
data_variable_replacer<T1, T2> make_data_variable_replacer(const T1& t1, const T2& t2)
{
  return data_variable_replacer<T1, T2>(t1, t2);
}

/// Function object that can be used by the partial_replace algorithm
/// to replace the names of data variables in an arbitrary term.
template <typename SrcList, typename DestList>
struct data_variable_name_replacer
{
  const SrcList& src_;
  const DestList& dest_;
  
  data_variable_name_replacer(const SrcList& src, const DestList& dest)
    : src_(src), dest_(dest)
  {
    assert(src_.size() == dest_.size());
  }
  
  std::pair<aterm_appl, bool> operator()(aterm_appl t) const
  {
    if (!is_data_variable(t))
    {
      return std::pair<aterm_appl, bool>(t, true); // continue the recursion
    }
    data_variable v(t);
    typename SrcList::const_iterator i = src_.begin();
    typename DestList::const_iterator j = dest_.begin();
    for (; i != src_.end(); ++i, ++j)
    {
      if (v.name() == *i)
      {
        return std::pair<aterm_appl, bool>(data_variable(*j, v.sort()), false); // don't continue the recursion
      }
    }
    return std::pair<aterm_appl, bool>(t, false); // don't continue the recursion
  }
};

/// Utility function for creating a data_variable_name_replacer.
template <typename T1, typename T2>
data_variable_name_replacer<T1, T2> make_data_variable_name_replacer(const T1& t1, const T2& t2)
{
  return data_variable_name_replacer<T1, T2>(t1, t2);
}

} // namespace detail

} // namespace lps

#endif // MCRL2_LPS_DETAIL_ALGORITHM_H
