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

#include "mcrl2/process/utility.h"
#include "mcrl2/utilities/sequence.h"

namespace mcrl2 {

namespace process {

//namespace detail {
//
//} // namespace detail

/// \brief Represents the set AI*, where the attribute includes_subsets determines whether subsets
/// of the elements are included.
struct allow_set
{
  multi_action_name_set A;
  action_name_set I;

  bool includes_subsets;

  allow_set()
  {}

  allow_set(const multi_action_name_set& A_, const action_name_set& I_ = action_name_set(), bool includes_subsets_ = false)
    : A(A_), I(I_), includes_subsets(includes_subsets_)
  {}
};

inline
std::ostream& operator<<(std::ostream& out, const allow_set& x)
{
  return out << lps::pp(x.A) << (x.includes_subsets ? "*" : "") << core::pp(x.I) << "*";
}

// operations on allow_set

namespace allow_set_operations {

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
  return A.find(a) != A.end();
}

typedef atermpp::map<core::identifier_string, std::vector<core::identifier_string> > rename_inverse_map;

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
void rename_inverse(const rename_inverse_map& Rinverse, const multi_action_name& x, multi_action_name_set& result)
{
  std::vector<std::vector<core::identifier_string> > V;

  multi_action_name alpha = x;

  // remove elements that appear in Rinverse, and put the replacements in V
  for (multi_action_name::iterator i = alpha.begin(); i != alpha.end(); )
  {
    rename_inverse_map::const_iterator j = Rinverse.find(*i);
    if (j != Rinverse.end())
    {
      alpha.erase(i++);
      V.push_back(j->second);
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

inline
multi_action_name_set rename_inverse(const rename_expression_list& R, const multi_action_name_set& A)
{
  // compute inverse of R
  rename_inverse_map Rinverse;
  for (rename_expression_list::const_iterator i = R.begin(); i != R.end(); ++i)
  {
    Rinverse[i->target()].push_back(i->source());
  }

  multi_action_name_set result;
  for (multi_action_name_set::const_iterator i = A.begin(); i != A.end(); ++i)
  {
    rename_inverse(Rinverse, *i, result);
  }
  return result;
}

inline
action_name_set rename_inverse(const rename_expression_list& R, const action_name_set& I)
{
  // compute inverse of R
  rename_inverse_map Rinverse;
  for (rename_expression_list::const_iterator i = R.begin(); i != R.end(); ++i)
  {
    Rinverse[i->target()].push_back(i->source());
  }

  action_name_set result;
  for (action_name_set::const_iterator i = I.begin(); i != I.end(); ++i)
  {
    rename_inverse_map::const_iterator j = Rinverse.find(*i);
    if (j != Rinverse.end())
    {
      result.insert(j->second.begin(), j->second.end());
    }
    else
    {
      result.insert(*i);
    }
  }
  return result;
}

multi_action_name apply_comm_inverse(const communication_expression& x, const multi_action_name& alpha)
{
  core::identifier_string c = x.name();
  core::identifier_string_list lhs = x.action_name().names();
  multi_action_name result = alpha;
  result.erase(result.find(c));
  result.insert(lhs.begin(), lhs.end());
  return result;
}

inline
void comm_inverse(const communication_expression_list& C, const multi_action_name& alpha, multi_action_name_set& result)
{
  for (communication_expression_list::const_iterator i = C.begin(); i != C.end(); ++i)
  {
    if (contains(alpha, i->name()))
    {
      multi_action_name beta = apply_comm_inverse(*i, alpha);
      comm_inverse(C, beta, result);
    }
  }
}

inline
multi_action_name_set comm_inverse(const communication_expression_list& C, const multi_action_name_set& A)
{
  multi_action_name_set result;
  for (multi_action_name_set::const_iterator i = A.begin(); i != A.end(); ++i)
  {
    comm_inverse(C, *i, result);
  }
  return result;
}

// Note that the result is flattened.
inline
action_name_set comm_inverse(const communication_expression_list& C, const action_name_set& I)
{
  action_name_set result = I;
  for (action_name_set::const_iterator i = I.begin(); i != I.end(); ++i)
  {
    for (communication_expression_list::const_iterator j = C.begin(); j != C.end(); ++j)
    {
      if (*i == j->name())
      {
        core::identifier_string_list lhs = j->action_name().names();
        result.insert(lhs.begin(), lhs.end());
      }
    }
  }
  return result;
}

// Hides (or: removes) elements in I from C
template <typename ContainerI, typename ContainerC>
ContainerC hide(const ContainerI& I, const ContainerC& C)
{
  std::vector<core::identifier_string> result;
  for (typename ContainerC::const_iterator i = C.begin(); i != C.end(); ++i)
  {
    if (contains(I, *i))
    {
      result.push_back(*i);
    }
  }
  return ContainerC(result.begin(), result.end());
}

inline
allow_set block(const core::identifier_string_list& B, const allow_set& x)
{
  if (x.includes_subsets)
  {
    return allow_set(alphabet_operations::hide(B, x.A), hide(B, x.I), x.includes_subsets);
  }
  else
  {
    return allow_set(alphabet_operations::block(B, x.A), hide(B, x.I), x.includes_subsets);
  }
}

inline
allow_set hide_inverse(const core::identifier_string_list& I, const allow_set& x)
{
  allow_set result = x;
  result.I.insert(I.begin(), I.end());
  return result;
}

inline
allow_set intersection(const multi_action_name_set& V, const allow_set& x)
{
  multi_action_name_set result;
  for (multi_action_name_set::iterator i = V.begin(); i != V.end(); ++i)
  {
    multi_action_name beta = hide(x.I, *i);
    bool add = x.includes_subsets ? detail::includes(x.A, beta) : contains(x.A, beta);
    if (add)
    {
      result.insert(*i);
    }
  }
  return allow_set(result);
}

inline
allow_set rename_inverse(const rename_expression_list& R, const allow_set& x)
{
  return allow_set(rename_inverse(R, x.A), rename_inverse(R, x.I), x.includes_subsets);
}

inline
allow_set comm_inverse(const communication_expression_list& C, const allow_set& x)
{
  return allow_set(comm_inverse(C, x.A), comm_inverse(C, x.I), x.includes_subsets);
}

inline
allow_set left_arrow(const allow_set& x, const multi_action_name_set& A)
{
  allow_set result = x;
  if (!x.includes_subsets)
  {
    result.A = left_arrow1(x.A, alphabet_operations::hide(x.I, A));
  }
  return result;
}

inline
allow_set subsets(const allow_set& x)
{
  allow_set result = x;
  result.includes_subsets = true;
  return result;
}

} // namespace allow_set_operations

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_DETAIL_ALLOW_SET_H
