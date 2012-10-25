// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/utility.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_UTILITY_H
#define MCRL2_PROCESS_UTILITY_H

#include "mcrl2/process/process_expression.h"

namespace mcrl2 {

namespace process {

namespace detail {

// checks if the sorted ranges [first1, ..., last1) and [first2, ..., last2) have an empty intersection
template <typename InputIterator1, typename InputIterator2>
bool has_empty_intersection(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2)
{
  while (first1 != last1 && first2 != last2)
  {
    if (*first1 < *first2)
    {
      ++first1;
    }
    else if (*first2 < *first1)
    {
      ++first2;
    }
    else
    {
      return false;
    }
  }
  return true;
}

inline
multi_action_name_set apply_block(const multi_action_name& alpha, const multi_action_name_set& A)
{
  multi_action_name_set result;
  for (multi_action_name_set::const_iterator i = A.begin(); i != A.end(); ++i)
  {
    if (has_empty_intersection(alpha.begin(), alpha.end(), i->begin(), i->end()))
    {
      result.insert(*i);
    }
  }
  return result;
}

inline
multi_action_name_set apply_block(const multi_action_name_set& B, const multi_action_name_set& A)
{
  multi_action_name alpha;
  for (multi_action_name_set::const_iterator i = B.begin(); i != B.end(); ++i)
  {
    alpha.insert(i->begin(), i->end());
  }
  return apply_block(alpha, A);
}

inline
core::identifier_string apply_rename(const rename_expression_list& R, const core::identifier_string& x)
{
  for (rename_expression_list::const_iterator i = R.begin(); i != R.end(); ++i)
  {
    if (x == i->source())
    {
      return i->target();
    }
  }
  return x;
}

inline
multi_action_name apply_rename(const rename_expression_list& R, const multi_action_name& a)
{
  multi_action_name result;
  for (multi_action_name::const_iterator i = a.begin(); i != a.end(); ++i)
  {
    result.insert(apply_rename(R, *i));
  }
  return result;
}

} // namespace detail

inline
multi_action_name set_union(const multi_action_name& alpha, const multi_action_name& beta)
{
  multi_action_name result;
  std::set_union(alpha.begin(), alpha.end(), beta.begin(), beta.end(), std::inserter(result, result.end()));
  return result;
}

inline
multi_action_name set_difference(const multi_action_name& alpha, const multi_action_name& beta)
{
  multi_action_name result;
  std::set_difference(alpha.begin(), alpha.end(), beta.begin(), beta.end(), std::inserter(result, result.end()));
  return result;
}

inline
multi_action_name_set set_union(const multi_action_name_set& A1, const multi_action_name_set& A2)
{
  multi_action_name_set result;
  std::set_union(A1.begin(), A1.end(), A2.begin(), A2.end(), std::inserter(result, result.end()));
  return result;
}

inline
multi_action_name_set set_intersection(const multi_action_name_set& A1, const multi_action_name_set& A2)
{
  multi_action_name_set result;
  std::set_intersection(A1.begin(), A1.end(), A2.begin(), A2.end(), std::inserter(result, result.end()));
  return result;
}

inline
multi_action_name_set set_difference(const multi_action_name_set& A1, const multi_action_name_set& A2)
{
  multi_action_name_set result;
  std::set_difference(A1.begin(), A1.end(), A2.begin(), A2.end(), std::inserter(result, result.end()));
  return result;
}

inline
multi_action_name_set times(const multi_action_name_set& A1, const multi_action_name_set& A2)
{
  multi_action_name_set result;
  for (multi_action_name_set::const_iterator i = A1.begin(); i != A1.end(); ++i)
  {
    const multi_action_name& alpha = *i;
    for (multi_action_name_set::const_iterator j = A2.begin(); j != A2.end(); ++j)
    {
      const multi_action_name& beta = *j;
      multi_action_name gamma = set_union(alpha, beta);
      result.insert(gamma);
    }
  }
  return result;
}

inline
multi_action_name_set apply_block(const core::identifier_string_list& B, const multi_action_name_set& A)
{
  multi_action_name alpha(B.begin(), B.end());
  return detail::apply_block(alpha, A);
}

inline
multi_action_name_set apply_hide(const core::identifier_string_list& I, const multi_action_name_set& A)
{
  multi_action_name m(I.begin(), I.end());
  multi_action_name_set result;
  for (multi_action_name_set::const_iterator i = A.begin(); i != A.end(); ++i)
  {
    result.insert(set_difference(*i, m));
  }
  return result;
}

inline
multi_action_name_set apply_rename(const rename_expression_list& R, const multi_action_name_set& A)
{
  multi_action_name_set result;
  for (multi_action_name_set::const_iterator i = A.begin(); i != A.end(); ++i)
  {
    result.insert(detail::apply_rename(R, *i));
  }
  return result;
}

inline
multi_action_name_set apply_allow(const action_name_multiset_list& V, const multi_action_name_set& A)
{
  // compute V1 such that V1 = union(V, { tau })
  multi_action_name_set V1;
  for (action_name_multiset_list::const_iterator i = V.begin(); i != V.end(); ++i)
  {
    core::identifier_string_list names = i->names();
    multi_action_name alpha(names.begin(), names.end());
    V1.insert(alpha);
  }
  V1.insert(multi_action_name());

  return set_intersection(A, V1);
}

inline
multi_action_name_set apply_comm(const communication_expression_list& C, const multi_action_name_set& A)
{
  multi_action_name_set result;
  for (communication_expression_list::const_iterator i = C.begin(); i != C.end(); ++i)
  {
    core::identifier_string_list names = i->action_name().names();
    core::identifier_string a = i->name();
    multi_action_name alpha(names.begin(), names.end());
    // *i == alpha -> a

    for (multi_action_name_set::const_iterator j = A.begin(); j != A.end(); ++j)
    {
      const multi_action_name& gamma = *j;
      if (std::includes(gamma.begin(), gamma.end(), alpha.begin(), alpha.end()))
      {
        multi_action_name beta = set_difference(gamma, alpha);
        beta.insert(a);
        result.insert(beta);
      }
    }
  }
  return result;
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_UTILITY_H
