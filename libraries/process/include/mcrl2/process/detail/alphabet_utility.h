// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/detail/alphabet_utility.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_DETAIL_ALPHABET_UTILITY_H
#define MCRL2_PROCESS_DETAIL_ALPHABET_UTILITY_H

#include <algorithm>
#include <iterator>
#include <sstream>
#include "mcrl2/process/traverser.h"

namespace mcrl2 {

namespace process {

// A multi-action is a set of actions. The special multi-action 'tau' is represented by the
// empty set of actions.

// A multi-action name is a set of identifiers.
// Example: the multi-action name of a(1) | b(2) | a(1) is { a, b }

// A set of multi-action names is denoted as 'multi_action_name_set'.

// Let ? be a set of multi-actions. For example { a(1) | b(2), a(2) | c(3) }
// Let A be a set of multi-action names. For example { {a, b}, {a, c} }
// Let R be a renaming function. For example ???

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
multi_action_name name(const lps::action& x)
{
  multi_action_name result;
  result.insert(x.label().name());
  return result;
}

inline
multi_action_name name(const multi_action& x)
{
  multi_action_name result;
  lps::action_list a = x.actions();
  for (lps::action_list::iterator i = a.begin(); i != a.end(); ++i)
  {
    result.insert(i->label().name());
  }
  return result;
}

inline
multi_action_name name(const core::identifier_string& x)
{
  multi_action_name result;
  result.insert(x);
  return result;
}

inline
multi_action_name_set make_name_set(const action_name_multiset_list& v)
{
  multi_action_name_set result;
  for (action_name_multiset_list::const_iterator i = v.begin(); i != v.end(); ++i)
  {
    core::identifier_string_list names = i->names();
    result.insert(multi_action_name(names.begin(), names.end()));
  }
  return result;
}

inline
multi_action_name_set make_name_set(const core::identifier_string_list& v)
{
  multi_action_name_set result;
  for (core::identifier_string_list::const_iterator i = v.begin(); i != v.end(); ++i)
  {
    multi_action_name a = name(*i);
    result.insert(a);
  }
  return result;
}

inline
multi_action_name_set make_name_set(const multi_action_name& a)
{
  multi_action_name_set s;
  s.insert(a);
  return s;
}

inline
core::identifier_string_list make_block_set(const multi_action_name_set& A)
{
  std::vector<core::identifier_string> tmp;
  for (multi_action_name_set::const_iterator i = A.begin(); i != A.end(); ++i)
  {
    const multi_action_name& a = *i;
    assert(a.size() == 1);
    tmp.push_back(*a.begin());
  }
  return core::identifier_string_list(tmp.begin(), tmp.end());
}

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
multi_action_name_set left_arrow(const multi_action_name_set& A1, const multi_action_name_set& A2)
{
  multi_action_name_set result;
  for (multi_action_name_set::const_iterator i = A2.begin(); i != A2.end(); ++i)
  {
    const multi_action_name& beta = *i;
    for (multi_action_name_set::const_iterator j = A1.begin(); j != A1.end(); ++j)
    {
      const multi_action_name& gamma = *j;
      if (std::includes(gamma.begin(), gamma.end(), beta.begin(), beta.end()))
      {
        multi_action_name alpha = set_difference(gamma, beta);
        result.insert(alpha);
      }
    }
  }
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
multi_action_name_set apply_communication(const communication_expression_list& C, const multi_action_name_set& A)
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

inline
multi_action_name_set apply_communication_inverse(const communication_expression_list& C, const multi_action_name_set& A)
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
      if (gamma.find(a) != gamma.end())
      {
        multi_action_name beta = gamma;
        beta.erase(beta.find(a));
        beta.insert(alpha.begin(), alpha.end());
        result.insert(beta);
      }
    }
  }
  return result;
}

inline
multi_action_name_set apply_communication_bar(const communication_expression_list& C, const multi_action_name_set& A)
{
  multi_action_name_set result;
  for (multi_action_name_set::const_iterator i = A.begin(); i != A.end(); ++i)
  {
    const multi_action_name& alpha = *i;
    for (communication_expression_list::const_iterator j = C.begin(); j != C.end(); ++j)
    {
      core::identifier_string_list gamma = j->action_name().names();
      multi_action_name m(gamma.begin(), gamma.end());
      if (std::includes(m.begin(), m.end(), alpha.begin(), alpha.end()))
      {
        result.insert(alpha);
      }
    }
  }
  return result;
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
multi_action_name_set apply_block(const core::identifier_string_list& I, const multi_action_name_set& A)
{
  multi_action_name alpha(I.begin(), I.end());
  return apply_block(alpha, A);
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

inline
multi_action_name_set apply_rename(const rename_expression_list& R, const multi_action_name_set& A)
{
  multi_action_name_set result;
  for (multi_action_name_set::const_iterator i = A.begin(); i != A.end(); ++i)
  {
    result.insert(apply_rename(R, *i));
  }
  return result;
}

inline
core::identifier_string apply_rename_inverse(const rename_expression_list& R, const core::identifier_string& x)
{
  for (rename_expression_list::const_iterator i = R.begin(); i != R.end(); ++i)
  {
    if (x == i->target())
    {
      return i->source();
    }
  }
  return x;
}

inline
multi_action_name apply_rename_inverse(const rename_expression_list& R, const multi_action_name& a)
{
  multi_action_name result;
  for (multi_action_name::const_iterator i = a.begin(); i != a.end(); ++i)
  {
    result.insert(apply_rename_inverse(R, *i));
  }
  return result;
}

inline
multi_action_name_set apply_rename_inverse(const rename_expression_list& R, const multi_action_name_set& A)
{
  multi_action_name_set result;
  for (multi_action_name_set::const_iterator i = A.begin(); i != A.end(); ++i)
  {
    result.insert(apply_rename_inverse(R, *i));
  }
  return result;
}

// returns all elements of B that are subset of an element in A
inline
multi_action_name_set subset_intersection(const multi_action_name_set& A, const multi_action_name_set& B)
{
  multi_action_name_set result;
  for (multi_action_name_set::iterator i = B.begin(); i != B.end(); ++i)
  {
    const multi_action_name& alpha = *i;
    for (multi_action_name_set::iterator j = A.begin(); j != A.end(); ++j)
    {
      const multi_action_name& beta = *j;
      if (std::includes(beta.begin(), beta.end(), alpha.begin(), alpha.end()))
      {
        result.insert(alpha);
      }
    }
  }
  return result;
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_DETAIL_ALPHABET_UTILITY_H
