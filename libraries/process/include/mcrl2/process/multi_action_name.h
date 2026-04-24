// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/multi_action_name.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_MULTI_ACTION_NAME_H
#define MCRL2_PROCESS_MULTI_ACTION_NAME_H

#include <boost/container/flat_set.hpp>
#include <iterator>

#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/process/action_label.h"
#include "mcrl2/process/action_name_multiset.h"
#include "mcrl2/utilities/detail/container_utility.h"

namespace mcrl2::process
{

/// \brief Represents the name of a multi action
/// Uses boost::flat_multiset for improved cache locality and memory efficiency
struct multi_action_name: public boost::container::flat_multiset<core::identifier_string, process::action_name_compare>
{
  using super = boost::container::flat_multiset<core::identifier_string, process::action_name_compare>;

  multi_action_name() = default;

  template <typename InputIterator>
  multi_action_name(InputIterator first, InputIterator last)
    : super(first, last)
  {}

  template<typename InputIterator>
  multi_action_name(boost::container::ordered_range_t tag, InputIterator first, InputIterator last)
    : super(tag, first, last)
  {}
};

/// \brief Represents a set of multi action names
using multi_action_name_set = std::set<multi_action_name>;

//-----------------------------------------------------//
// multi_action_name operations
//-----------------------------------------------------//

// Returns true if the multiset y is contained in x
inline bool includes(const multi_action_name& x, const multi_action_name& y)
{
  return std::includes(x.begin(), x.end(), y.begin(), y.end(), multi_action_name::key_compare());
}

inline bool contains(const multi_action_name& alpha, const core::identifier_string& a)
{
  return alpha.find(a) != alpha.end();
}

inline bool has_empty_intersection(const multi_action_name& x, const multi_action_name& y)
{
  return utilities::detail::has_empty_intersection(x,y);
}

// Returns alpha \ beta
inline multi_action_name multiset_difference(const multi_action_name& alpha, const multi_action_name& beta)
{
  multi_action_name result;
  std::set_difference(alpha.begin(),
    alpha.end(),
    beta.begin(),
    beta.end(),
    std::inserter(result, result.end()),
    multi_action_name::key_compare());
  return result;
}

inline multi_action_name multiset_union(multi_action_name alpha, multi_action_name beta)
{
  if(alpha.size() > beta.size())
  {
    alpha.merge(std::move(beta));
    return alpha;
  }
  else
  {
    beta.merge(std::move(alpha));
    return beta;
  }
}

/// \brief Pretty print function for a multi action name
inline
std::string pp(const multi_action_name& x)
{
  std::ostringstream out;
  if (x.empty())
  {
    out << "tau";
  }
  else
  {
    for (auto i = x.begin(); i != x.end(); ++i)
    {
      if (i != x.begin())
      {
        out << " | ";
      }
      out << *i;
    }
  }
  return out.str();
}

//-----------------------------------------------------//
// multi_action_name_set operations
//-----------------------------------------------------//

inline multi_action_name_set make_name_set(const action_name_multiset_list& v)
{
  multi_action_name_set result;
  for (const action_name_multiset& i: v)
  {
    const core::identifier_string_list& names = i.names();
    result.insert(multi_action_name(names.begin(), names.end()));
  }
  return result;
}

inline action_name_multiset_list make_multiset_list(const multi_action_name_set& A)
{
  return action_name_multiset_list(
    A.begin(),
    A.end(),
    [](const multi_action_name& alpha)
    {
      return action_name_multiset(core::identifier_string_list(alpha.begin(), alpha.end()));
    });
}

inline bool contains(const multi_action_name_set& A, const multi_action_name& a)
{
  // note that A implicitly contains the empty multi_action_name
  return a.empty() || A.find(a) != A.end();
}

inline bool subset_includes(const multi_action_name_set& A, const multi_action_name& x)
{
  return std::any_of(A.begin(), A.end(), [&x](const multi_action_name& a){ return includes(a, x); });
}

inline
  // Returns true if A contains an x such that includes(x, y)
  bool includes(const multi_action_name_set& A, const multi_action_name& y)
{
  return (A.empty() && y.empty()) || subset_includes(A, y);
}

inline multi_action_name_set set_difference(const multi_action_name_set& A1, const multi_action_name_set& A2)
{
  multi_action_name_set result;
  std::set_difference(A1.begin(), A1.end(), A2.begin(), A2.end(), std::inserter(result, result.end()));
  return result;
}

inline multi_action_name_set set_union(const multi_action_name_set& A1, const multi_action_name_set& A2)
{
  multi_action_name_set result;
  std::set_union(A1.begin(), A1.end(), A2.begin(), A2.end(), std::inserter(result, result.end()));
  return result;
}

inline multi_action_name_set set_intersection(const multi_action_name_set& A1, const multi_action_name_set& A2)
{
  multi_action_name_set result;
  std::set_intersection(A1.begin(), A1.end(), A2.begin(), A2.end(), std::inserter(result, result.end()));
  return result;
}

// Removes elements of A that are a subset of another element.
inline multi_action_name_set remove_subsets(const multi_action_name_set& A)
{
  multi_action_name_set result;
  for (const multi_action_name& alpha: A)
  {
    if (!includes(result, alpha))
    {
      result.insert(alpha);
    }
  }
  return result;
}

inline
std::set<core::identifier_string> action_names(const multi_action_name_set& A)
{
  std::set<core::identifier_string> result;
  for (const multi_action_name& alpha: A)
  {
    result.insert(alpha.begin(), alpha.end());
  }
  return result;
}

inline
std::ostream& operator<<(std::ostream& out, const multi_action_name& alpha)
{
  return out << pp(alpha);
}

/// \brief Pretty print function for a set of multi action names
inline
std::string pp(const multi_action_name_set& A)
{
  std::ostringstream out;
  out << "{";
  for (auto i = A.begin(); i != A.end(); ++i)
  {
    if (i != A.begin())
    {
      out << ", ";
    }
    out << pp(*i);
  }
  out << "}";
  return out.str();
}

inline
std::ostream& operator<<(std::ostream& out, const multi_action_name_set& A)
{
  return out << pp(A);
}

} // namespace mcrl2::process

#endif // MCRL2_PROCESS_MULTI_ACTION_NAME_H
