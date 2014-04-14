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
#include "mcrl2/process/find.h"
#include "mcrl2/process/print.h"
#include "mcrl2/process/replace.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/sequence.h"

namespace mcrl2 {

namespace process {

inline
process_expression expand_rhs(const process::process_instance_assignment& x, const std::vector<process_equation>& equations)
{
  const process_equation& eqn = find_equation(equations, x.identifier());
  process_expression p = eqn.expression();
  data::mutable_map_substitution<> sigma;
  data::assignment_list a = x.assignments();
  for (data::assignment_list::iterator i = a.begin(); i != a.end(); ++i)
  {
    sigma[i->lhs()] = i->rhs();
  }
  std::set<data::variable> v = process::find_free_variables(x);
  process_expression result = process::replace_variables_capture_avoiding(p, sigma, v);
  return result;
}

inline
process_expression expand_rhs(const process::process_instance& x, const std::vector<process_equation>& equations)
{
  const process_equation& eqn = find_equation(equations, x.identifier());
  process_expression p = eqn.expression();
  data::mutable_map_substitution<> sigma;
  data::variable_list d = eqn.formal_parameters();
  data::data_expression_list e = x.actual_parameters();
  data::variable_list::iterator di = d.begin();
  data::data_expression_list::iterator ei = e.begin();
  for (; di != d.end(); ++di, ++ei)
  {
    sigma[*di] = *ei;
  }
  std::set<data::variable> v = process::find_free_variables(x);
  process_expression result = process::replace_variables_capture_avoiding(p, sigma, v);
  return result;
}

// Expands the assignments in x
inline
process_instance expand_assignments(const process::process_instance_assignment& x, const std::vector<process_equation>& equations)
{
  const process_equation& eqn = find_equation(equations, x.identifier());
  const data::variable_list& v = eqn.formal_parameters();
  data::assignment_sequence_substitution sigma(x.assignments());
  std::vector<data::data_expression> e;
  for (auto i = v.begin(); i != v.end(); ++i)
  {
    e.push_back(sigma(*i));
  }
  return process_instance(x.identifier(), data::data_expression_list(e.begin(), e.end()));
}

inline
multi_action_name multiset_union(const multi_action_name& alpha, const multi_action_name& beta)
{
  multi_action_name result;
  std::merge(alpha.begin(), alpha.end(), beta.begin(), beta.end(), std::inserter(result, result.end()));
  return result;
}

namespace detail {

inline
bool contains_tau(const multi_action_name_set& A)
{
  multi_action_name tau;
  return A.find(tau) != A.end();
}

inline
process_expression make_sync(const process_expression x, const process_expression& y)
{
  if (is_delta(x) || is_delta(y))
  {
    return delta();
  }
  return sync(x, y);
}

inline
process_expression make_merge(const process_expression x, const process_expression& y)
{
  if (is_delta(x) && is_delta(y))
  {
    return delta();
  }
  return merge(x, y);
}

inline
process_expression make_left_merge(const process_expression x, const process_expression& y)
{
  if (is_delta(y))
  {
    return delta();
  }
  return left_merge(x, y);
}

inline
process_expression make_allow(const multi_action_name_set& A, const process_expression& x)
{
  if (A.empty())
  {
    return delta();
  }

  // convert A to an action_name_multiset_list B
  std::vector<action_name_multiset> v;
  for (multi_action_name_set::const_iterator i = A.begin(); i != A.end(); ++i)
  {
    const multi_action_name& alpha = *i;
    if (!i->empty()) // exclude tau
    {
      v.push_back(action_name_multiset(core::identifier_string_list(alpha.begin(), alpha.end())));
    }
  }
  action_name_multiset_list B(v.begin(), v.end());

  return B.empty() ? x : allow(B, x);
}

inline
process_expression make_comm(const communication_expression_list& C, const process_expression& x)
{
  if (C.empty())
  {
    return x;
  }
  else
  {
    return comm(C, x);
  }
}

inline
process_expression make_hide(const core::identifier_string_list& I, const process_expression& x)
{
  if (I.empty())
  {
    return x;
  }
  else
  {
    return hide(I, x);
  }
}

inline
process_expression make_block(const core::identifier_string_list& B, const process_expression& x)
{
  if (B.empty())
  {
    return x;
  }
  else
  {
    return block(B, x);
  }
}

// returns true if x is a source of one of the rename rules in R
inline
bool is_source(const rename_expression_list& R, const core::identifier_string& x)
{
  for (rename_expression_list::const_iterator i = R.begin(); i != R.end(); ++i)
  {
    if (i->source() == x)
    {
      return true;
    }
  }
  return false;
}

typedef std::map<core::identifier_string, std::vector<core::identifier_string> > rename_inverse_map;

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

// Example: R = {b -> c}, then rename_inverse(R) = {b -> [], c -> [b, c]}
inline
rename_inverse_map rename_inverse(const rename_expression_list& R)
{
  rename_inverse_map Rinverse;
  for (rename_expression_list::const_iterator i = R.begin(); i != R.end(); ++i)
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
void apply_comm(const communication_expression& c, multi_action_name_set& A)
{
  core::identifier_string_list names = c.action_name().names();
  core::identifier_string a = c.name();
  multi_action_name alpha(names.begin(), names.end());
  // c == alpha -> a

  multi_action_name_set to_be_added;
  for (multi_action_name_set::const_iterator i = A.begin(); i != A.end(); ++i)
  {
    multi_action_name beta = *i;
    while (includes(beta, alpha))
    {
      for (multi_action_name::const_iterator j = alpha.begin(); j != alpha.end(); ++j)
      {
        beta.erase(beta.find(*j));
      }
      beta.insert(a);
      to_be_added.insert(beta);
    }
  }
  A.insert(to_be_added.begin(), to_be_added.end());
}

// Add inverse communication to A
inline
void apply_comm_inverse(const communication_expression& gamma, multi_action_name_set& A)
{
  std::vector<multi_action_name> to_be_added;
  core::identifier_string c = gamma.name();
  core::identifier_string_list lhs = gamma.action_name().names();

  for (multi_action_name_set::iterator i = A.begin(); i != A.end(); ++i)
  {
    const multi_action_name& alpha = *i;
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
multi_action_name multiset_difference(const multi_action_name& alpha, const multi_action_name& beta)
{
  multi_action_name result = alpha;
  for (multi_action_name::const_iterator i = beta.begin(); i != beta.end(); ++i)
  {
    multi_action_name::iterator j = result.find(*i);
    if (j != result.end())
    {
      result.erase(j);
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
multi_action_name_set left_arrow1(const multi_action_name_set& A1, const multi_action_name_set& A2)
{
  multi_action_name_set result = A1; // needed because tau is not explicitly stored
  for (multi_action_name_set::const_iterator i = A2.begin(); i != A2.end(); ++i)
  {
    const multi_action_name& beta = *i;
    for (multi_action_name_set::const_iterator j = A1.begin(); j != A1.end(); ++j)
    {
      const multi_action_name& gamma = *j;
      if (detail::includes(gamma, beta))
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
  mCRL2log(log::debug) << "<left_arrow>" << process::pp(A1) << (A1_includes_subsets ? "*" : "") << " <- " << process::pp(A2) << " = " << process::pp(result) << (A1_includes_subsets ? "*" : "") << std::endl;
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

// Removes all elements from alphabet that are not in A.
inline
multi_action_name_set set_intersection(const multi_action_name_set& alphabet, const multi_action_name_set& A, bool A_includes_subsets)
{
  multi_action_name_set result = alphabet;
  for (multi_action_name_set::iterator i = result.begin(); i != result.end(); )
  {
    bool remove = A_includes_subsets ? !detail::includes(A, *i) : A.find(*i) == A.end();
    if (remove)
    {
      result.erase(i++);
    }
    else
    {
      ++i;
    }
  }
  return result;
}

inline
communication_expression_list filter_comm_set(const communication_expression_list& C, const multi_action_name_set& alphabet)
{
  std::vector<communication_expression> result;
  for (communication_expression_list::const_iterator i = C.begin(); i != C.end(); ++i)
  {
    core::identifier_string_list lhs = i->action_name().names();
    multi_action_name alpha(lhs.begin(), lhs.end());
    if (detail::includes(alphabet, alpha))
    {
      result.push_back(*i);
    }
  }
  return communication_expression_list(result.begin(), result.end());
}

/// \brief The namespace for alphabet operations
namespace alphabet_operations {

// remove all elements alpha in A that are included in another element of A
inline
multi_action_name_set remove_subsets(const multi_action_name_set& A)
{
  multi_action_name_set result;
  for (multi_action_name_set::const_iterator i = A.begin(); i != A.end(); ++i)
  {
    if (!detail::includes(result, *i))
    {
      result.insert(*i);
    }
  }
  return result;
}

inline
multi_action_name_set merge(const multi_action_name_set& A1, const multi_action_name_set& A2)
{
  return set_union(set_union(A1, A2), concat(A1, A2));
}

inline
multi_action_name_set left_merge(const multi_action_name_set& A1, const multi_action_name_set& A2)
{
  return alphabet_operations::merge(A1, A2);
}

inline
multi_action_name_set sync(const multi_action_name_set& A1, const multi_action_name_set& A2)
{
  return concat(A1, A2);
}

inline
multi_action_name_set comm(const communication_expression_list& C, const multi_action_name_set& A, bool /* A_includes_subsets */ = false)
{
  multi_action_name_set result = A;

  // sequentially apply the communication rules to result
  for (communication_expression_list::const_iterator i = C.begin(); i != C.end(); ++i)
  {
    detail::apply_comm(*i, result);
  }

  return result;
}

inline
multi_action_name_set comm_inverse(const communication_expression_list& C, const multi_action_name_set& A, bool /* A_includes_subsets */ = false)
{
  multi_action_name_set result = A;
  for (auto i = C.begin(); i != C.end(); ++i)
  {
    detail::apply_comm_inverse(*i, result);
  }
  mCRL2log(log::debug) << "<comm_inverse>" << process::pp(C) << ": " << process::pp(A) << " -> " << process::pp(result) << std::endl;
  return result;
}

template <typename IdentifierContainer>
multi_action_name_set hide(const IdentifierContainer& I, const multi_action_name_set& A, bool /* A_includes_subsets */ = false)
{
  multi_action_name m(I.begin(), I.end());
  multi_action_name_set result;
  for (multi_action_name_set::const_iterator i = A.begin(); i != A.end(); ++i)
  {
    multi_action_name alpha = *i;
    for (typename IdentifierContainer::const_iterator j = I.begin(); j != I.end(); ++j)
    {
      alpha.erase(*j);
    }
    result.insert(alpha);
  }
  return result;
}

/// \brief Computes R(A)
inline
multi_action_name_set rename(const rename_expression_list& R, const multi_action_name_set& A, bool /* A_includes_subsets */ = false)
{
  multi_action_name_set result;
  for (multi_action_name_set::const_iterator i = A.begin(); i != A.end(); ++i)
  {
    result.insert(detail::apply_rename(R, *i));
  }
  return result;
}

/// \brief Computes R^[-1}(A)
inline
multi_action_name_set rename_inverse(const rename_expression_list& R, const multi_action_name_set& A, bool /* A_includes_subsets */ = false)
{
  detail::rename_inverse_map Rinverse = detail::rename_inverse(R);

  multi_action_name_set result;
  for (multi_action_name_set::const_iterator i = A.begin(); i != A.end(); ++i)
  {
    detail::rename_inverse(Rinverse, *i, result);
  }
  return result;
}

inline
multi_action_name_set allow(const action_name_multiset_list& V, const multi_action_name_set& A, bool A_includes_subsets = false)
{
  multi_action_name_set result;

  for (action_name_multiset_list::const_iterator i = V.begin(); i != V.end(); ++i)
  {
    core::identifier_string_list names = i->names();
    multi_action_name v(names.begin(), names.end());
    for (multi_action_name_set::const_iterator j = A.begin(); j != A.end(); ++j)
    {
      const multi_action_name& alpha = *j;
      bool keep = A_includes_subsets ? detail::includes(alpha, v) : alpha == v;
      if (keep)
      {
        result.insert(v);
      }
    }
  }
  return result;
}

inline
multi_action_name_set block(const core::identifier_string_list& B, const multi_action_name_set& A, bool A_includes_subsets = false)
{
  multi_action_name_set result;
  multi_action_name beta(B.begin(), B.end());

  if (A_includes_subsets)
  {
    for (multi_action_name_set::const_iterator i = A.begin(); i != A.end(); ++i)
    {
      multi_action_name alpha = *i;
      for (core::identifier_string_list::const_iterator j = B.begin(); j != B.end(); ++j)
      {
        alpha.erase(*j);
      }
      if (!alpha.empty())
      {
        result.insert(alpha);
      }
    }
  }
  else
  {
    for (multi_action_name_set::const_iterator i = A.begin(); i != A.end(); ++i)
    {
      if (detail::has_empty_intersection(beta.begin(), beta.end(), i->begin(), i->end()))
      {
        result.insert(*i);
      }
    }
  }
  return result;
}

} // namespace alphabet_operations

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_UTILITY_H
