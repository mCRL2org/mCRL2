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

// Returns true if the multiset y is contained in x
inline
bool includes(const multi_action_name& x, const multi_action_name& y)
{
  return std::includes(x.begin(), x.end(), y.begin(), y.end());
}

inline
// Returns true if A contains an x such that includes(x, y)
bool includes(const multi_action_name_set& A, const multi_action_name& y)
{
  for (multi_action_name_set::const_iterator i = A.begin(); i != A.end(); ++i)
  {
    if (includes(*i, y))
    {
      return true;
    }
  }
  return false;
}

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
  return detail::apply_block(alpha, A);
}

// TODO: increase the efficiency of this implementation
inline
multi_action_name apply_comm(const std::map<multi_action_name, core::identifier_string>& C, const multi_action_name& alpha)
{
  multi_action_name result = alpha;
  for (std::map<multi_action_name, core::identifier_string>::const_iterator i = C.begin(); i != C.end(); ++i)
  {
    const multi_action_name& beta = i->first;
    while (includes(result, beta))
    {
      for (multi_action_name::const_iterator j = beta.begin(); j != beta.end(); ++j)
      {
        result.erase(result.find(*j));
      }
      result.insert(i->second);
    }
  }
  return result;
}

inline
void apply_comm_inverse(const atermpp::vector<core::identifier_string>& alpha1, atermpp::vector<core::identifier_string>& alpha2, std::size_t i, const std::map<core::identifier_string, core::identifier_string_list>& Cinverse, multi_action_name_set& result)
{
  if (i >= alpha1.size())
  {
    result.insert(multi_action_name(alpha2.begin(), alpha2.end()));
  }
  else
  {
    core::identifier_string c = alpha1[i];
    alpha2.push_back(c);
    apply_comm_inverse(alpha1, alpha2, i + 1, Cinverse, result);
    alpha2.erase(--alpha2.end());
    std::map<core::identifier_string, core::identifier_string_list>::const_iterator j = Cinverse.find(alpha1[i]);
    assert (j != Cinverse.end());
    alpha2.insert(alpha2.end(), j->second.begin(), j->second.end());
    apply_comm_inverse(alpha1, alpha2, i + 1, Cinverse, result);
  }
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
    result.insert(detail::apply_rename_inverse(R, *i));
  }
  return result;
}

} // namespace detail

inline
multi_action_name multiset_union(const multi_action_name& alpha, const multi_action_name& beta)
{
  multi_action_name result;
  std::merge(alpha.begin(), alpha.end(), beta.begin(), beta.end(), std::inserter(result, result.end()));
  return result;
}

inline
multi_action_name multiset_difference(const multi_action_name& alpha, const multi_action_name& beta)
{
  multi_action_name result = alpha;
  for (multi_action_name::const_iterator i = beta.begin(); i != beta.end(); ++i)
  {
    result.erase(*i);
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
multi_action_name_set concat(const multi_action_name_set& A1, const multi_action_name_set& A2)
{
  multi_action_name_set result;
  for (multi_action_name_set::const_iterator i = A1.begin(); i != A1.end(); ++i)
  {
    for (multi_action_name_set::const_iterator j = A2.begin(); j != A2.end(); ++j)
    {
      result.insert(multiset_union(*i, *j));
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
        multi_action_name alpha = multiset_difference(gamma, beta);
        result.insert(alpha);
      }
    }
  }
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
multi_action_name_set apply_block(const core::identifier_string_list& B, const multi_action_name_set& A)
{
  multi_action_name alpha(B.begin(), B.end());
  return detail::apply_block(alpha, A);
}

inline
multi_action_name_set apply_comm(const communication_expression_list& C, const multi_action_name_set& A)
{
  multi_action_name_set result;

  // convert C to C1
  std::map<multi_action_name, core::identifier_string> C1;
  for (communication_expression_list::const_iterator i = C.begin(); i != C.end(); ++i)
  {
    core::identifier_string_list names = i->action_name().names();
    core::identifier_string a = i->name();
    multi_action_name alpha(names.begin(), names.end());
    // *i == alpha -> a
    C1[alpha] = a;
  }

  // apply C1 to the elements of A
  for (multi_action_name_set::const_iterator j = A.begin(); j != A.end(); ++j)
  {
    result.insert(detail::apply_comm(C1, *j));
  }
  return result;
}

inline
multi_action_name_set apply_comm_inverse(const communication_expression_list& C, const multi_action_name_set& A)
{
  std::map<core::identifier_string, core::identifier_string_list> Cinverse;
  for (communication_expression_list::const_iterator i = C.begin(); i != C.end(); ++i)
  {
    Cinverse[i->name()] = i->action_name().names();
  }
  multi_action_name_set result;
  for (multi_action_name_set::const_iterator j = A.begin(); j != A.end(); ++j)
  {
    const multi_action_name& alpha = *j;
    atermpp::vector<core::identifier_string> alpha1; // elements of alpha that are present in Cinverse
    atermpp::vector<core::identifier_string> alpha2; // elements of alpha that are not present in Cinverse
    for (multi_action_name::const_iterator k = alpha.begin(); k != alpha.end(); ++k)
    {
      if (Cinverse.find(*k) == Cinverse.end())
      {
        alpha2.push_back(*k);
      }
      else
      {
        alpha1.push_back(*k);
      }
    }
    detail::apply_comm_inverse(alpha1, alpha2, 0, Cinverse, result);
  }
  return result;
}

inline
multi_action_name_set apply_hide(const core::identifier_string_list& I, const multi_action_name_set& A)
{
  multi_action_name m(I.begin(), I.end());
  multi_action_name_set result;
  for (multi_action_name_set::const_iterator i = A.begin(); i != A.end(); ++i)
  {
    result.insert(multiset_difference(*i, m));
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
multi_action_name_set apply_rename_inverse(const rename_expression_list& R, const multi_action_name_set& A)
{
  multi_action_name_set result;
  for (multi_action_name_set::const_iterator i = A.begin(); i != A.end(); ++i)
  {
    result.insert(detail::apply_rename_inverse(R, *i));
  }
  return result;
}

// Returns true if elements were removed from alphabet
inline
bool filter_alphabet(multi_action_name_set& alphabet, const multi_action_name_set& A, bool A_includes_subsets = false)
{
  bool result = false;
  for (multi_action_name_set::iterator i = alphabet.begin(); i != alphabet.end(); )
  {
    bool remove = A_includes_subsets ? !detail::includes(A, *i) : A.find(*i) == A.end();
    if (remove)
    {
      alphabet.erase(i++);
      result = true;
    }
    else
    {
      ++i;
    }
  }
  return result;
}

inline
multi_action_name_set merge_union(const multi_action_name_set& A1, const multi_action_name_set& A2)
{
  return set_union(set_union(A1, A2), concat(A1, A2));
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_UTILITY_H
