// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/detail/allow_set.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_DETAIL_ALLOW_SET_H
#define MCRL2_PROCESS_DETAIL_ALLOW_SET_H

#include <algorithm>
#include "mcrl2/process/utility.h"

namespace mcrl2 {

namespace process {

namespace detail {

inline
bool contains(const core::identifier_string_list A, const core::identifier_string& a)
{
  return std::find(A.begin(), A.end(), a) != A.end();
}

inline
bool contains(const action_name_set& A, const core::identifier_string& a)
{
  return A.find(a) != A.end();
}

inline
bool contains(const multi_action_name& alpha, const core::identifier_string& a)
{
  return alpha.find(a) != alpha.end();
}

inline
bool contains(const multi_action_name_set& A, const multi_action_name& a)
{
  // note that A implicitly contains the empty multi_action_name
  return a.empty() || A.find(a) != A.end();
}

// Hides (or: removes) elements in I from C
inline
action_name_set hide(const core::identifier_string_list& I, const action_name_set& J)
{
  action_name_set result;
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
multi_action_name hide(const action_name_set& I, const multi_action_name& alpha)
{
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

} // namespace detail

struct allow_set;
std::ostream& operator<<(std::ostream& out, const allow_set& x);

/// \brief Represents the set AI*. If the attribute A_includes_subsets is true, also subsets of the elements are included.
/// An invariant of the allow_set is that elements of A do not contain elements of I. This invariant will be
/// established during construction.
struct allow_set
{
  multi_action_name_set A;
  bool A_includes_subsets;
  action_name_set I;

  void establish_invariant()
  {
    multi_action_name_set A1;
    for (const multi_action_name& i : A)
    {
      A1.insert(detail::hide(I, i));
    }
    std::swap(A, A1);
    assert(check_invariant());
  }

  bool check_invariant() const
  {
    using utilities::detail::contains;
    if (I.empty())
    {
      return true;
    }
    for (const multi_action_name& alpha: A)
    {
      for (const core::identifier_string& j: alpha)
      {
        if (contains(I, j))
        {
          return false;
        }
      }
    }
    return true;
  }

  allow_set()
  {}

  allow_set(const multi_action_name_set& A_, bool A_includes_subsets_ = false, const action_name_set& I_ = action_name_set())
    : A_includes_subsets(A_includes_subsets_), I(I_)
  {
    for (const multi_action_name& i: A_)
    {
      A.insert(detail::hide(I_, i));
    }
  }

  /// \brief Returns true if the allow set contains the multi action name alpha.
  bool contains(const multi_action_name& alpha) const
  {
    multi_action_name beta = detail::hide(I, alpha);
    return beta.empty() || (A_includes_subsets ? detail::includes(A, beta) : detail::contains(A, beta));
  }

  /// \brief Returns the intersection of the allow set with alphabet.
  multi_action_name_set intersect(const multi_action_name_set& alphabet) const
  {
    multi_action_name_set result;
    for (const multi_action_name& alpha: alphabet)
    {
      if (contains(alpha))
      {
        result.insert(alpha);
      }
    }
    multi_action_name tau;
    if (alphabet.find(tau) != alphabet.end())
    {
      result.insert(tau);
    }
    return result;
  }

  bool operator==(const allow_set& other) const
  {
    return A == other.A && A_includes_subsets == other.A_includes_subsets && I == other.I;
  }

  bool operator<(const allow_set& other) const
  {
    if (A_includes_subsets != other.A_includes_subsets)
    {
      return A_includes_subsets < other.A_includes_subsets;
    }
    if (A.size() != other.A.size())
    {
      return A.size() < other.A.size();
    }
    if (I.size() != other.I.size())
    {
      return I.size() < other.I.size();
    }
    auto m = std::mismatch(A.begin(), A.end(), other.A.begin());
    if (m.first != A.end())
    {
      return *m.first < *m.second;
    }
    return I < other.I;
  }
};

inline
std::ostream& operator<<(std::ostream& out, const allow_set& x)
{
  if (!x.A.empty())
  {
    out << pp(x.A) << (x.A_includes_subsets ? "@" : "");
  }
  if (!x.I.empty())
  {
    out << "{" << core::pp(x.I) << "}*";
  }
  if (x.A.empty() && x.I.empty())
  {
    out << "{}";
  }
  return out;
}

// operations on allow_set

namespace allow_set_operations {

inline
action_name_set rename_inverse(const rename_expression_list& R, const action_name_set& I)
{
  detail::rename_inverse_map Rinverse = detail::rename_inverse(R);

  action_name_set result;
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
std::pair<multi_action_name, multi_action_name> apply_comm_inverse(const communication_expression& x, const multi_action_name& alpha1, const multi_action_name& alpha2)
{
  core::identifier_string c = x.name();
  core::identifier_string_list lhs = x.action_name().names();
  std::pair<multi_action_name, multi_action_name> result;
  result.first = alpha1;
  result.second = alpha2;
  result.first.erase(result.first.find(c));
  result.second.insert(lhs.begin(), lhs.end());
  return result;
}

inline
void comm_inverse(const communication_expression_list& C, const multi_action_name& alpha1, const multi_action_name& alpha2, multi_action_name_set& result)
{
  result.insert(multiset_union(alpha1, alpha2));
  for (auto i = C.begin(); i != C.end(); ++i)
  {
    if (detail::contains(alpha1, i->name()))
    {
      std::pair<multi_action_name, multi_action_name> beta = apply_comm_inverse(*i, alpha1, alpha2);
      comm_inverse(C, beta.first, beta.second, result);
    }
  }
}

inline
multi_action_name_set comm_inverse(const communication_expression_list& C, const multi_action_name_set& A)
{
  multi_action_name_set result;
  multi_action_name empty;
  for (const multi_action_name& i: A)
  {
    comm_inverse(C, i, empty, result);
  }
  return result;
}

// Note that the result is flattened.
inline
action_name_set comm_inverse(const communication_expression_list& C, const action_name_set& I)
{
  action_name_set result = I;
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
allow_set block(const core::identifier_string_list& B, const allow_set& x)
{
  if (x.A_includes_subsets)
  {
    return allow_set(alphabet_operations::hide(B, x.A), x.A_includes_subsets, detail::hide(B, x.I));
  }
  else
  {
    return allow_set(alphabet_operations::block(B, x.A), x.A_includes_subsets, detail::hide(B, x.I));
  }
}

inline
allow_set hide_inverse(const core::identifier_string_list& I, const allow_set& x)
{
  allow_set result = x;
  result.A = alphabet_operations::hide_inverse(I, result.A, result.A_includes_subsets);
  result.I.insert(I.begin(), I.end());
  result.establish_invariant();
  return result;
}

inline
allow_set allow(const action_name_multiset_list& V, const allow_set& x)
{
  multi_action_name_set A;
  for (const action_name_multiset& v: V)
  {
    const core::identifier_string_list& names = v.names();
    multi_action_name beta(names.begin(), names.end());
    bool add = x.A_includes_subsets ? detail::includes(x.A, beta) : detail::contains(x.A, detail::hide(x.I, beta));
    if (add)
    {
      A.insert(beta);
    }
  }
  return allow_set(A);
}

inline
allow_set rename_inverse(const rename_expression_list& R, const allow_set& x)
{
  allow_set result(alphabet_operations::rename_inverse(R, x.A, x.A_includes_subsets), x.A_includes_subsets, rename_inverse(R, x.I));
  mCRL2log(log::debug1) << "rename_inverse(" << R << ", " << x << ") = " << result << std::endl;
  return result;
}

inline
allow_set comm_inverse(const communication_expression_list& C, const allow_set& x)
{
  allow_set result(comm_inverse(C, x.A), x.A_includes_subsets, comm_inverse(C, x.I));
  mCRL2log(log::debug1) << "comm_inverse(" << C << ", " << x << ") = " << result << std::endl;
  return result;
}

inline
multi_action_name_set left_arrow2(const multi_action_name_set& A1, const action_name_set& I, const multi_action_name_set& A2)
{
  multi_action_name_set result = A1; // needed because tau is not explicitly stored
  for (const multi_action_name& i: A2)
  {
    multi_action_name beta = detail::hide(I, i);
    for (const multi_action_name& gamma: A1)
    {
      if (detail::includes(gamma, beta))
      {
        multi_action_name alpha = multiset_difference(gamma, beta);
        if (!alpha.empty())
        {
          result.insert(detail::hide(I, alpha));
        }
      }
    }
  }
  return result;
}

inline
allow_set left_arrow(const allow_set& x, const multi_action_name_set& A)
{
  allow_set result = x;
  if (!x.A_includes_subsets)
  {
    result.A = left_arrow2(x.A, x.I, A);
  }
  result.establish_invariant();
  mCRL2log(log::debug1) << "left_arrow(" << x << ", " << process::pp(A) << ") = " << result << std::endl;
  return result;
}

inline
allow_set subsets(const allow_set& x)
{
  allow_set result = x;
  result.A_includes_subsets = true;
  result.A = alphabet_operations::remove_subsets(result.A);
  result.establish_invariant();
  return result;
}

} // namespace allow_set_operations

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_DETAIL_ALLOW_SET_H
