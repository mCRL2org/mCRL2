// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/alphabet_operations.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_ALPHABET_OPERATIONS_H
#define MCRL2_PROCESS_ALPHABET_OPERATIONS_H

#include "mcrl2/process/communication_expression.h"
#include "mcrl2/process/multi_action_name.h"
#include "mcrl2/process/rename_expression.h"
#include "mcrl2/utilities/sequence.h"
#include "mcrl2/utilities/detail/container_utility.h"

namespace mcrl2 {

namespace process {

namespace alphabet_operations {

//-----------------------------------------------------//
// multi_action_name operations
//-----------------------------------------------------//

// Returns true if the multiset y is contained in x
inline
bool includes(const multi_action_name& x, const multi_action_name& y)
{
  return std::includes(x.begin(), x.end(), y.begin(), y.end());
}

inline
bool contains(const multi_action_name& alpha, const core::identifier_string& a)
{
  return alpha.find(a) != alpha.end();
}

// Returns alpha \ beta
inline
multi_action_name multiset_difference(const multi_action_name& alpha, const multi_action_name& beta)
{
  multi_action_name result = alpha;
  for (const core::identifier_string& i: beta)
  {
    multi_action_name::iterator j = result.find(i);
    if (j != result.end())
    {
      result.erase(j);
    }
  }
  return result;
}

inline
multi_action_name multiset_union(const multi_action_name& alpha, const multi_action_name& beta)
{
  multi_action_name result;
  std::merge(alpha.begin(), alpha.end(), beta.begin(), beta.end(), std::inserter(result, result.end()));
  return result;
}

//-----------------------------------------------------//
// multi_action_name_set operations
//-----------------------------------------------------//

inline
multi_action_name_set make_name_set(const action_name_multiset_list& v)
{
  multi_action_name_set result;
  for (const action_name_multiset& i: v)
  {
    const core::identifier_string_list& names = i.names();
    result.insert(multi_action_name(names.begin(), names.end()));
  }
  return result;
}

inline
bool contains(const multi_action_name_set& A, const multi_action_name& a)
{
  // note that A implicitly contains the empty multi_action_name
  return a.empty() || A.find(a) != A.end();
}

inline
// Returns true if A contains an x such that includes(x, y)
bool includes(const multi_action_name_set& A, const multi_action_name& y)
{
  for (const multi_action_name& alpha: A)
  {
    if (includes(alpha, y))
    {
      return true;
    }
  }
  return false;
}

inline
bool subset_includes(const multi_action_name_set& A, const multi_action_name& x)
{
  for (const multi_action_name& a: A)
  {
    if (includes(a, x))
    {
      return true;
    }
  }
  return false;
}

inline
multi_action_name_set set_difference(const multi_action_name_set& A1, const multi_action_name_set& A2)
{
  multi_action_name_set result;
  std::set_difference(A1.begin(), A1.end(), A2.begin(), A2.end(), std::inserter(result, result.end()));
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
multi_action_name_set remove_subsets(const multi_action_name_set& A)
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

//-----------------------------------------------------//
// block operations
//-----------------------------------------------------//

inline
multi_action_name_set block(const core::identifier_string_list& B, const multi_action_name_set& A, bool A_includes_subsets = false)
{
  multi_action_name_set result;
  multi_action_name beta(B.begin(), B.end());

  if (A_includes_subsets)
  {
    for (multi_action_name alpha: A)
    {
      for (const core::identifier_string& b: B)
      {
        alpha.erase(b);
      }
      if (!alpha.empty())
      {
        result.insert(alpha);
      }
    }
  }
  else
  {
    for (const multi_action_name& alpha: A)
    {
      if (utilities::detail::has_empty_intersection(beta.begin(), beta.end(), alpha.begin(), alpha.end()))
      {
        result.insert(alpha);
      }
    }
  }
  return result;
}

//-----------------------------------------------------//
// hide operations
//-----------------------------------------------------//

// Hides elements in I from alpha
inline
multi_action_name hide(const multi_action_name& alpha, const std::set<core::identifier_string>& I)
{
  using utilities::detail::contains;
  multi_action_name result;
  for (const core::identifier_string& a: alpha)
  {
    if (!contains(I, a))
    {
      result.insert(a);
    }
  }
  return result;
}

// Hides (or: removes) elements in I from C
inline
std::set<core::identifier_string> hide(const core::identifier_string_list& I, const std::set<core::identifier_string>& J)
{
  using utilities::detail::contains;
  std::set<core::identifier_string> result;
  for (const core::identifier_string& j: J)
  {
    if (!contains(I, j))
    {
      result.insert(j);
    }
  }
  return result;
}

inline
multi_action_name hide(const std::set<core::identifier_string>& I, const multi_action_name& alpha)
{
  using utilities::detail::contains;
  multi_action_name result;
  for (const core::identifier_string& i: alpha)
  {
    if (!contains(I, i))
    {
      result.insert(i);
    }
  }
  return result;
}

template <typename IdentifierContainer>
multi_action_name_set hide(const IdentifierContainer& I, const multi_action_name_set& A, bool /* A_includes_subsets */ = false)
{
  multi_action_name m(I.begin(), I.end());
  multi_action_name_set result;
  for (multi_action_name alpha: A)
  {
    for (const core::identifier_string& i: I)
    {
      alpha.erase(i);
    }
    result.insert(alpha);
  }
  return result;
}

inline
multi_action_name_set hide_inverse(const core::identifier_string_list& I, const multi_action_name_set& A, bool A_includes_subsets = false)
{
  return block(I, A, A_includes_subsets);
}

//-----------------------------------------------------//
// merge/left_merge/sync operations
//-----------------------------------------------------//

inline
multi_action_name_set concat(const multi_action_name_set& A1, const multi_action_name_set& A2)
{
  multi_action_name_set result;
  for (const multi_action_name& i: A1)
  {
    for (const multi_action_name& j: A2)
    {
       result.insert(multiset_union(i, j));
    }
  }
  return result;
}

// Returns true if alpha in concat(A1, A2).
// TODO: Make the implementation more inefficient!
inline
bool concat_includes(const multi_action_name_set& A1, const multi_action_name_set& A2, const multi_action_name& alpha)
{
  for (const multi_action_name& alpha1: A1)
  {
    for (const multi_action_name& alpha2: A2)
    {
      if (alpha == multiset_union(alpha1, alpha2))
      {
        return true;
      }
    }
  }
  return false;
}

// Returns the intersection of concat(A1, A2) and subsets(A)
inline
multi_action_name_set bounded_concat(const multi_action_name_set& A1, const multi_action_name_set& A2, const multi_action_name_set& A)
{
  multi_action_name_set result;
  for (const multi_action_name& i: A1)
  {
    for (const multi_action_name& j: A2)
    {
      multi_action_name alpha = multiset_union(i, j);
      if (subset_includes(A, alpha))
      {
        result.insert(alpha);
      }
    }
  }
  return result;
}

inline
multi_action_name_set left_arrow1(const multi_action_name_set& A1, const multi_action_name_set& A2)
{
  multi_action_name_set result = A1; // needed because tau is not explicitly stored
  for (const multi_action_name& beta: A2)
  {
    for (const multi_action_name& gamma: A1)
    {
      if (includes(gamma, beta))
      {
        multi_action_name alpha = multiset_difference(gamma, beta);
        if (!alpha.empty())
        {
          result.insert(alpha);
        }
      }
    }
  }
  return result;
}

inline
multi_action_name_set left_arrow(const multi_action_name_set& A1, bool A1_includes_subsets, const multi_action_name_set& A2)
{
  multi_action_name_set result;
  if (A1_includes_subsets)
  {
    result = A1;
  }
  else
  {
    result = set_union(A1, left_arrow1(A1, A2));
  }
  return result;
}

// returns left_arrow(A I*, A2)
inline
multi_action_name_set left_arrow2(const multi_action_name_set& A, const std::set<core::identifier_string>& I, const multi_action_name_set& A2)
{
  multi_action_name_set result = A; // needed because tau is not explicitly stored
  for (const multi_action_name& alpha2: A2)
  {
    multi_action_name beta = hide(I, alpha2);
    for (const multi_action_name& alpha: A)
    {
      if (includes(alpha, beta))
      {
        multi_action_name gamma = multiset_difference(alpha, beta);
        if (!gamma.empty())
        {
          result.insert(hide(I, gamma));
        }
      }
    }
  }
  return result;
}

inline
multi_action_name_set merge(const multi_action_name_set& A1, const multi_action_name_set& A2)
{
  return set_union(set_union(A1, A2), concat(A1, A2));
}

// Returns the intersection of merge(A1, A2) and subsets(A)
inline
multi_action_name_set bounded_merge(const multi_action_name_set& A1, const multi_action_name_set& A2, const multi_action_name_set& A)
{
  return set_union(set_union(A1, A2), bounded_concat(A1, A2, A));
}

inline
multi_action_name_set left_merge(const multi_action_name_set& A1, const multi_action_name_set& A2)
{
  return merge(A1, A2);
}

inline
multi_action_name_set sync(const multi_action_name_set& A1, const multi_action_name_set& A2)
{
  return concat(A1, A2);
}

// Returns the intersection of sync(A1, A2) and subsets(A)
inline
multi_action_name_set bounded_sync(const multi_action_name_set& A1, const multi_action_name_set& A2, const multi_action_name_set& A)
{
  return bounded_concat(A1, A2, A);
}

//-----------------------------------------------------//
// comm operations
//-----------------------------------------------------//

inline
std::pair<multi_action_name, multi_action_name> apply_comm_inverse(const communication_expression& x, const multi_action_name& alpha1, const multi_action_name& alpha2)
{
  const core::identifier_string& c = x.name();
  core::identifier_string_list lhs = x.action_name().names();
  std::pair<multi_action_name, multi_action_name> result;
  result.first = alpha1;
  result.second = alpha2;
  result.first.erase(result.first.find(c));
  result.second.insert(lhs.begin(), lhs.end());
  return result;
}

// Add inverse communication to A
inline
void apply_comm_inverse(const communication_expression& gamma, multi_action_name_set& A)
{
  std::vector<multi_action_name> to_be_added;
  const core::identifier_string& c = gamma.name();
  core::identifier_string_list lhs = gamma.action_name().names();

  for (const multi_action_name& alpha: A)
  {
    std::size_t n = alpha.count(c);
    if (n > 0)
    {
      multi_action_name beta = alpha;
      for (std::size_t k = 0; k < n; k++)
      {
        beta.erase(beta.find(c));
        beta.insert(lhs.begin(), lhs.end());
        to_be_added.push_back(beta);
      }
    }
  }
  A.insert(to_be_added.begin(), to_be_added.end());
}

inline
void comm_inverse(const communication_expression_list& C, const multi_action_name& alpha1, const multi_action_name& alpha2, multi_action_name_set& result)
{
  using utilities::detail::contains;
  result.insert(alphabet_operations::multiset_union(alpha1, alpha2));
  for (const communication_expression& c: C)
  {
    if (contains(alpha1, c.name()))
    {
      std::pair<multi_action_name, multi_action_name> beta = apply_comm_inverse(c, alpha1, alpha2);
      comm_inverse(C, beta.first, beta.second, result);
    }
  }
}

inline
multi_action_name_set comm_inverse1(const communication_expression_list& C, const multi_action_name_set& A)
{
  multi_action_name_set result;
  multi_action_name empty;
  for (const multi_action_name& alpha: A)
  {
    comm_inverse(C, alpha, empty, result);
  }
  return result;
}

inline
multi_action_name_set comm_inverse(const communication_expression_list& C, const multi_action_name_set& A, bool /* A_includes_subsets */ = false)
{
  multi_action_name_set result = A;
  for (const communication_expression& c: C)
  {
    apply_comm_inverse(c, result);
  }
  return result;
}

// Note that the result is flattened.
inline
std::set<core::identifier_string> comm_inverse(const communication_expression_list& C, const std::set<core::identifier_string>& I)
{
  std::set<core::identifier_string> result = I;
  for (const core::identifier_string& i: I)
  {
    for (const communication_expression& j: C)
    {
      if (i == j.name())
      {
        core::identifier_string_list lhs = j.action_name().names();
        result.insert(lhs.begin(), lhs.end());
      }
    }
  }
  return result;
}

inline
void apply_comm(const communication_expression& c, multi_action_name_set& A)
{
  core::identifier_string_list names = c.action_name().names();
  const core::identifier_string& a = c.name();
  multi_action_name alpha(names.begin(), names.end());
  // c == alpha -> a

  multi_action_name_set to_be_added;
  for (multi_action_name beta: A)
  {
    while (includes(beta, alpha))
    {
      for (const core::identifier_string& a: alpha)
      {
        beta.erase(beta.find(a));
      }
      beta.insert(a);
      to_be_added.insert(beta);
    }
  }
  A.insert(to_be_added.begin(), to_be_added.end());
}

inline
multi_action_name_set comm(const communication_expression_list& C, const multi_action_name_set& A, bool /* A_includes_subsets */ = false)
{
  multi_action_name_set result = A;

  // sequentially apply the communication rules to result
  for (const communication_expression& c: C)
  {
    apply_comm(c, result);
  }

  return result;
}

inline
communication_expression_list filter_comm_set(const communication_expression_list& C, const multi_action_name_set& alphabet)
{
  std::vector<communication_expression> result;
  for (const communication_expression& c: C)
  {
    core::identifier_string_list lhs = c.action_name().names();
    multi_action_name alpha(lhs.begin(), lhs.end());
    if (includes(alphabet, alpha))
    {
      result.push_back(c);
    }
  }
  return communication_expression_list(result.begin(), result.end());
}

//-----------------------------------------------------//
// rename operations
//-----------------------------------------------------//

// returns true if x is a source of one of the rename rules in R
inline
bool is_source(const rename_expression_list& R, const core::identifier_string& x)
{
  for (const rename_expression& r: R)
  {
    if (r.source() == x)
    {
      return true;
    }
  }
  return false;
}

inline
core::identifier_string apply_rename(const rename_expression_list& R, const core::identifier_string& x)
{
  for (const rename_expression& r: R)
  {
    if (x == r.source())
    {
      return r.target();
    }
  }
  return x;
}

inline
multi_action_name apply_rename(const rename_expression_list& R, const multi_action_name& a)
{
  multi_action_name result;
  for (const core::identifier_string& i: a)
  {
    result.insert(apply_rename(R, i));
  }
  return result;
}

/// \brief Computes R(A)
inline
multi_action_name_set rename(const rename_expression_list& R, const multi_action_name_set& A, bool /* A_includes_subsets */ = false)
{
  multi_action_name_set result;
  for (const multi_action_name& alpha: A)
  {
    result.insert(apply_rename(R, alpha));
  }
  return result;
}

typedef std::map<core::identifier_string, std::vector<core::identifier_string> > rename_inverse_map;

// Example: R = {b -> c}, then rename_inverse(R) = {b -> [], c -> [b, c]}
inline
rename_inverse_map rename_inverse(const rename_expression_list& R)
{
  rename_inverse_map Rinverse;
  for (auto i = R.begin(); i != R.end(); ++i)
  {
    Rinverse[i->target()].push_back(i->source());
    if (!is_source(R, i->target()))
    {
      Rinverse[i->target()].push_back(i->target());
    }
    Rinverse[i->source()]; // this is to make sure that i->source() is in the map
  }
  return Rinverse;
}

inline
std::string print_rename_inverse_map(const rename_inverse_map& m)
{
  std::ostringstream out;
  out << "{ ";
  for (auto i = m.begin(); i != m.end(); ++i)
  {
    if (i != m.begin())
    {
      out << ", ";
    }
    out << core::pp(i->first) << " -> [";
    const std::vector<core::identifier_string>& v = i->second;
    for (auto j = v.begin(); j != v.end(); ++j)
    {
      if (j != v.begin())
      {
        out << ", ";
      }
      out << *j;
    }
    out << "]";
  }
  out << " }";
  return out.str();
}

struct rename_inverse_apply
{
  const multi_action_name& alpha;
  const std::vector<core::identifier_string>& beta;
  multi_action_name_set& A;

  rename_inverse_apply(const multi_action_name& alpha_, const std::vector<core::identifier_string>& beta_, multi_action_name_set& A_)
    : alpha(alpha_), beta(beta_), A(A_)
  {}

  void operator()()
  {
    multi_action_name gamma = alpha;
    gamma.insert(beta.begin(), beta.end());
    A.insert(gamma);
  }
};

inline
std::set<core::identifier_string> rename_inverse(const rename_expression_list& R, const std::set<core::identifier_string>& I)
{
  alphabet_operations::rename_inverse_map Rinverse = alphabet_operations::rename_inverse(R);

  std::set<core::identifier_string> result;
  for (const core::identifier_string& i: I)
  {
    auto j = Rinverse.find(i);
    if (j != Rinverse.end())
    {
      result.insert(j->second.begin(), j->second.end());
    }
    else
    {
      result.insert(i);
    }
  }
  return result;
}

inline
void rename_inverse(const rename_inverse_map& Rinverse, const multi_action_name& x, bool x_includes_subsets, multi_action_name_set& result)
{
  std::vector<std::vector<core::identifier_string> > V;

  multi_action_name alpha = x;

  // remove elements that appear in Rinverse, and put the replacements in V
  for (auto i = alpha.begin(); i != alpha.end(); )
  {
    auto j = Rinverse.find(*i);
    if (j != Rinverse.end())
    {
      alpha.erase(i++);
      if (!j->second.empty() || !x_includes_subsets)
      {
        V.push_back(j->second);
      }
    }
    else
    {
      ++i;
    }
  }

  // v will hold a replacement
  std::vector<core::identifier_string> v(V.size());

  // generate all permutations of the replacements in V, and put them in result
  utilities::foreach_sequence(V, v.begin(), rename_inverse_apply(alpha, v, result));
}

/// \brief Computes R^[-1}(A)
inline
multi_action_name_set rename_inverse(const rename_expression_list& R, const multi_action_name_set& A, bool A_includes_subsets = false)
{
  rename_inverse_map Rinverse = rename_inverse(R);

  multi_action_name_set result;
  for (const multi_action_name& alpha: A)
  {
    rename_inverse(Rinverse, alpha, A_includes_subsets, result);
  }
  return result;
}

//-----------------------------------------------------//
// allow operations
//-----------------------------------------------------//

inline
multi_action_name_set allow(const action_name_multiset_list& V, const multi_action_name_set& A, bool A_includes_subsets = false)
{
  multi_action_name_set result;

  for (const action_name_multiset& s: V)
  {
    const core::identifier_string_list& names = s.names();
    multi_action_name v(names.begin(), names.end());
    for (const multi_action_name& alpha: A)
    {
      bool keep = A_includes_subsets ? includes(alpha, v) : alpha == v;
      if (keep)
      {
        result.insert(v);
      }
    }
  }
  return result;
}

} // namespace alphabet_operations

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_ALPHABET_OPERATIONS_H
