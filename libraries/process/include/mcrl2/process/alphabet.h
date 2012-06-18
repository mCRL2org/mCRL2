// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/alphabet.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_ALPHABET_H
#define MCRL2_PROCESS_ALPHABET_H

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

// A set of multi-action names is denoted as 'aset'.

// Let ? be a set of multi-actions. For example { a(1) | b(2), a(2) | c(3) }
// Let A be a set of multi-action names. For example { {a, b}, {a, c} }
// Let R be a renaming function. For example ???

typedef atermpp::multiset<core::identifier_string> multi_action_name;
typedef atermpp::set<multi_action_name> aset;

inline
std::string pp(const multi_action_name& x)
{
  std::ostringstream out;
  for (multi_action_name::const_iterator i = x.begin(); i != x.end(); ++i)
  {
    if (i != x.begin())
    {
      out << " | ";
    }
    out << core::pp(*i);
  }
  return out.str();
}

inline
std::string pp(const aset& A)
{
  std::ostringstream out;
  out << "{";
  for (aset::const_iterator i = A.begin(); i != A.end(); ++i)
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
multi_action_name name(const lps::multi_action& x)
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
aset make_aset(const action_name_multiset_list& v)
{
  aset result;
  for (action_name_multiset_list::const_iterator i = v.begin(); i != v.end(); ++i)
  {
    core::identifier_string_list names = i->names();
    result.insert(multi_action_name(names.begin(), names.end()));
  }
  return result;
}

inline
aset make_aset(const core::identifier_string_list& v)
{
  aset result;
  for (core::identifier_string_list::const_iterator i = v.begin(); i != v.end(); ++i)
  {
    multi_action_name a = name(*i);
    result.insert(a);
  }
  return result;
}

inline
aset make_aset(const multi_action_name& a)
{
  aset s;
  s.insert(a);
  return s;
}

inline
core::identifier_string_list make_block_set(const aset& A)
{
  std::vector<core::identifier_string> tmp;
  for (aset::const_iterator i = A.begin(); i != A.end(); ++i)
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
aset times(const aset& A1, const aset& A2)
{
  aset result;
  for (aset::const_iterator i = A1.begin(); i != A1.end(); ++i)
  {
    const multi_action_name& alpha = *i;
    for (aset::const_iterator j = A2.begin(); j != A2.end(); ++j)
    {
      const multi_action_name& beta = *j;
      multi_action_name gamma = set_union(alpha, beta);
      result.insert(gamma);
    }
  }
  return result;
}

inline
aset left_arrow(const aset& A1, const aset& A2)
{
  aset result;
  for (aset::const_iterator i = A2.begin(); i != A2.end(); ++i)
  {
    const multi_action_name& beta = *i;
    for (aset::const_iterator j = A1.begin(); j != A1.end(); ++j)
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
aset set_union(const aset& A1, const aset& A2)
{
  aset result;
  std::set_union(A1.begin(), A1.end(), A2.begin(), A2.end(), std::inserter(result, result.end()));
  return result;
}

inline
aset set_intersection(const aset& A1, const aset& A2)
{
  aset result;
  std::set_intersection(A1.begin(), A1.end(), A2.begin(), A2.end(), std::inserter(result, result.end()));
  return result;
}

inline
aset apply_communication(const communication_expression_list& C, const aset& A)
{
  aset result;
  for (communication_expression_list::const_iterator i = C.begin(); i != C.end(); ++i)
  {
    core::identifier_string_list names = i->action_name().names();
    core::identifier_string a = i->name();
    multi_action_name alpha(names.begin(), names.end());
    // *i == alpha -> a

    for (aset::const_iterator j = A.begin(); j != A.end(); ++j)
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
aset apply_communication_inverse(const communication_expression_list& C, const aset& A)
{
  aset result;
  for (communication_expression_list::const_iterator i = C.begin(); i != C.end(); ++i)
  {
    core::identifier_string_list names = i->action_name().names();
    core::identifier_string a = i->name();
    multi_action_name alpha(names.begin(), names.end());
    // *i == alpha -> a

    for (aset::const_iterator j = A.begin(); j != A.end(); ++j)
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
aset apply_block(const core::identifier_string_list& B, const aset& A)
{
  std::set<core::identifier_string> S(B.begin(), B.end());
  aset result;
  for (aset::const_iterator i = A.begin(); i != A.end(); ++i)
  {
    if (has_empty_intersection(S.begin(), S.end(), i->begin(), i->end()))
    {
      result.insert(*i);
    }
  }
  return result;
}

inline
aset apply_hide(const core::identifier_string_list& I, const aset& A)
{
  multi_action_name m(I.begin(), I.end());
  aset result;
  for (aset::const_iterator i = A.begin(); i != A.end(); ++i)
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
aset apply_rename(const rename_expression_list& R, const aset& A)
{
  aset result;
  for (aset::const_iterator i = A.begin(); i != A.end(); ++i)
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
aset apply_rename_inverse(const rename_expression_list& R, const aset& A)
{
  aset result;
  for (aset::const_iterator i = A.begin(); i != A.end(); ++i)
  {
    result.insert(apply_rename_inverse(R, *i));
  }
  return result;
}

// returns all elements of B that are subset of an element in A
inline
aset subset_intersection(const aset& A, const aset& B)
{
  aset result;
  for (aset::iterator i = B.begin(); i != B.end(); ++i)
  {
    const multi_action_name& alpha = *i;
    for (aset::iterator j = A.begin(); j != A.end(); ++j)
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

// prototype declarations
aset alphabet_allow(const process_expression& x, const aset& A, bool A_is_all, const atermpp::map<process_identifier, process_expression>& process_bodies);
aset alphabet_sub(const process_expression& x, const aset& A, const atermpp::map<process_identifier, process_expression>& process_bodies);
aset alphabet_block(const process_expression& x, const aset& A, const atermpp::map<process_identifier, process_expression>& process_bodies);

template <typename Derived>
struct default_alphabet_traverser: public process_expression_traverser<Derived>
{
  typedef process_expression_traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();

#if BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  const aset& A;

  // maps processes to their corresponding bodies
  const atermpp::map<process_identifier, process_expression>& process_bodies;

  std::vector<aset> result_stack;

  const process_expression& process_body(const process_identifier& id) const
  {
    atermpp::map<process_identifier, process_expression>::const_iterator i = process_bodies.find(id);
    assert(i != process_bodies.end());
    return i->second;
  }

  // Constructor
  default_alphabet_traverser(const aset& A_, const atermpp::map<process_identifier, process_expression>& process_bodies_)
    : A(A_), process_bodies(process_bodies_)
  {}

  // Push x to result_stack
  void push(const aset& x)
  {
    result_stack.push_back(x);
  }

  // Pop the last element of result_stack and return it
  aset pop()
  {
    aset result = result_stack.back();
    result_stack.pop_back();
    return result;
  }

  // Return the top element of result_stack
  const aset& top() const
  {
    return result_stack.back();
  }

  // joins the top two elements of the stack
  void join()
  {
    aset right = pop();
    aset left = pop();
    push(set_union(left, right));
  }

  // P(e1, ..., en)
  void operator()(const process::process_instance& x)
  {
    derived()(process_body(x.identifier()));
  }

  // P(d1 = e1, ..., dn = en)
  void operator()(const process::process_instance_assignment& x)
  {
    derived()(process_body(x.identifier()));
  }

  // delta
  void leave(const process::delta& x)
  {
    push(aset());
  }

  // p1 + p2
  void leave(const process::choice& x)
  {
    join();
  }

  // p1 . p2
  void leave(const process::seq& x)
  {
    join();
  }

  // c -> p
  void leave(const process::if_then& x)
  {
    // skip
  }

  // c -> p1 <> p2
  void leave(const process::if_then_else& x)
  {
    join();
  }

  // sum d:D . p
  void leave(const process::sum& x)
  {
    // skip
  }

  // p1 ||_ p2
  void operator()(const process::left_merge& x)
  {
    // implement it using p1 || p2
    merge y(x.left(), x.right());
    derived()(y);
  }

  // p @ t
  void operator()(const process::at& x)
  {
    // skip
  }
};

template <typename Derived>
struct alphabet_allow_traverser: public default_alphabet_traverser<Derived>
{
  typedef default_alphabet_traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();
  using super::A;
  using super::push;
  using super::pop;
  using super::top;
  using super::join;
  using super::process_bodies;

#if BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  // if true, A represents the set of all multi action names
  bool A_is_all;

  // Constructor
  alphabet_allow_traverser(const aset& A_, bool A_is_all_, const atermpp::map<process_identifier, process_expression>& process_bodies_)
    : super(A_, process_bodies_), A_is_all(A_is_all_)
  {}

  // pushes intersect(A, {a})
  void push_A_intersection(const multi_action_name& a)
  {
    if (A_is_all)
    {
      push(make_aset(a));
    }
    else
    {
      if (A.find(a) == A.end())
      {
        push(aset());
      }
      else
      {
        push(make_aset(a));
      }
    }
  }

  // a(e1, ..., en)
  void leave(const lps::action& x)
  {
    multi_action_name a = name(x);
    push_A_intersection(a);
  }

  // tau
  void leave(const process::tau& x)
  {
    multi_action_name tau;
    push_A_intersection(tau);
  }

  // p1 || p2
  void operator()(const process::merge& x) // use operator() here, since we don't want to go into the default recursion
  {
    if (A_is_all)
    {
      aset A1 = alphabet_allow(x.left(), A, true, process_bodies);
      aset A2 = alphabet_allow(x.right(), A, true, process_bodies);
      aset A1xA2 = times(A1, A2);
      push(A1xA2);
    }
    else
    {
      aset A1 = alphabet_sub(x.left(), A, process_bodies);
      aset AA1 = left_arrow(A, A1);
      aset AAA1 = set_union(A, AA1);
      aset A2 = alphabet_allow(x.right(), AAA1, false, process_bodies);
      aset A1xA2 = times(A1, A2);
      aset A2A1xA2 = set_union(A2, A1xA2);
      aset A1A2A1xA2 = set_union(A1, A2A1xA2);
      aset AA1A2A1xA2 = set_intersection(A, A1A2A1xA2);
      push(AA1A2A1xA2);
    }
  }

  // p1 | p2
  void operator()(const process::sync& x)
  {
    if (A_is_all)
    {
      aset A1 = alphabet_allow(x.left(), A, true, process_bodies);
      aset A2 = alphabet_allow(x.right(), A, true, process_bodies);
      aset AA1 = left_arrow(A, A1);
      aset AAA1 = set_union(A, AA1);
      aset A1xA2 = times(A1, A2);
      aset AA1xA2 = set_intersection(A, A1xA2);
      push(AA1xA2);
    }
    else
    {
      aset A1 = alphabet_sub(x.left(), A, process_bodies);
      aset AA1 = left_arrow(A, A1);
      aset AAA1 = set_union(A, AA1);
      aset A2 = alphabet_allow(x.right(), AAA1, false, process_bodies);
      aset A1xA2 = times(A1, A2);
      aset AA1xA2 = set_intersection(A, A1xA2);
      push(AA1xA2);
    }
  }

  // block(B, p)
  void operator()(const process::block& x)
  {
    core::identifier_string_list B = x.block_set();
    if (A_is_all)
    {
      push(alphabet_block(x.operand(), make_aset(B), process_bodies));
    }
    else
    {
      push(apply_block(B, alphabet_allow(x.operand(), A, false, process_bodies)));
    }
  }

  // hide(I, p)
  void operator()(const process::hide& x)
  {
    aset A1 = alphabet_allow(x.operand(), A, true, process_bodies);
    aset A2 = apply_hide(x.hide_set(), A1);
    if (A_is_all)
    {
      aset AA2 = set_intersection(A, A2);
      push(AA2);
    }
    else
    {
      push(A2);
    }
  }

  // rename(R, p)
  void operator()(const process::rename& x)
  {
    rename_expression_list R = x.rename_set();
    if (A_is_all)
    {
      aset A1 = alphabet_allow(x.operand(), A, true, process_bodies);
      aset RA1 = apply_rename(R, A1);
      push(RA1);
    }
    else
    {
      aset RinverseA = apply_rename_inverse(R, A);
      aset A1 = alphabet_allow(x.operand(), RinverseA, false, process_bodies);
      aset RA1 = apply_rename(R, A1);
      push(RA1);
    }
  }

  // comm(C, p)
  void operator()(const process::comm& x)
  {
    communication_expression_list C = x.comm_set();
    if (A_is_all)
    {
      aset Aprime = alphabet_allow(x, A, true, process_bodies);
      aset CAprime = apply_communication(C, Aprime);
      push(CAprime);
    }
    else
    {
      aset CinverseA = apply_communication_inverse(C, A);
      aset ACinverseA = set_union(A, CinverseA);
      aset Aprime = alphabet_allow(x, ACinverseA, false, process_bodies);
      aset CAprime = apply_communication(C, Aprime);
      aset ACAprime = set_intersection(A, CAprime);
      push(ACAprime);
    }
  }

  // allow(A, p)
  void operator()(const process::allow& x)
  {
    aset V = make_aset(x.allow_set());
    multi_action_name tau;
    V.insert(tau);
    if (A_is_all)
    {
      push(alphabet_allow(x, V, true, process_bodies));
    }
    else
    {
      aset AV = set_intersection(A, V);
      push(alphabet_allow(x, AV, false, process_bodies));
    }
  }

//  void operator()(const process::process_expression& x)
//  {
//    std::cout << "<visit>" << process::pp(x) << std::endl;
//    super::operator()(x);
//    std::cout << "<top>" << pp(result_stack.back()) << std::endl;
//  }
};

//////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename Derived>
struct alphabet_sub_traverser: public default_alphabet_traverser<Derived>
{
  typedef default_alphabet_traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();
  using super::top;
  using super::push;
  using super::join;
  using super::A;
  using super::process_bodies;

#if BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif

  // Constructor
  alphabet_sub_traverser(const aset& A, const atermpp::map<process_identifier, process_expression>& process_bodies)
    : super(A, process_bodies)
  {}

  // a(e1, ..., en)
  void leave(const lps::action& x)
  {
    aset B = make_aset(name(x));
    push(subset_intersection(A, B));
  }

  // tau
  void leave(const process::tau& x)
  {
    multi_action_name tau;
    aset B = make_aset(tau);
    push(subset_intersection(A, B));
  }

  // p1 || p2
  void operator()(const process::merge& x)
  {
    aset A1 = alphabet_sub(x.left(), A, process_bodies);
    aset A2 = alphabet_sub(x.right(), A, process_bodies);
    aset A1xA2 = times(A1, A2);
    aset A2A1xA2 = set_union(A2, A1xA2);
    aset A1A2A1xA2 = set_union(A1, A2A1xA2);
    aset AA1A2A1xA2 = subset_intersection(A, A1A2A1xA2);
    push(AA1A2A1xA2);
  }

  // p1 | p2
  void operator()(const process::sync& x)
  {
    aset A1 = alphabet_sub(x.left(), A, process_bodies);
    aset A2 = alphabet_sub(x.right(), A, process_bodies);
    aset A1xA2 = times(A1, A2);
    aset AA1xA2 = subset_intersection(A, A1xA2);
    push(AA1xA2);
  }

  // block(B, p)
  void operator()(const process::block& x)
  {
    core::identifier_string_list B = x.block_set();
    aset A1 = apply_block(B, A);
    aset A2 = alphabet_block(x.operand(), A1, process_bodies);
    push(A2);
  }

  // hide(I, p)
  void operator()(const process::hide& x)
  {
    aset A1 = alphabet_allow(x.operand(), A, true, process_bodies);
    aset A2 = apply_hide(x.hide_set(), A1);
    push(A2);
  }

  // comm(A, p)
  void operator()(const process::comm& x)
  {
    communication_expression_list C = x.comm_set();
    aset CinverseA = apply_communication_inverse(C, A);
    aset ACinverseA = set_union(A, CinverseA);
    aset Aprime = alphabet_allow(x, ACinverseA, false, process_bodies);
    aset CAprime = apply_communication(C, Aprime);
    aset ACAprime = subset_intersection(A, CAprime);
    push(ACAprime);
  }

  // allow(A, p)
  void operator()(const process::allow& x)
  {
    aset V = make_aset(x.allow_set());
    multi_action_name tau;
    V.insert(tau);
    aset AV = subset_intersection(A, V);
    push(alphabet_allow(x, AV, false, process_bodies));
  }
};

template <typename Derived>
struct alphabet_block_traverser: public default_alphabet_traverser<Derived>
{
  typedef default_alphabet_traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();
  using super::top;
  using super::push;
  using super::join;
  using super::A;
  using super::process_bodies;

#if BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif

  // Constructor
  alphabet_block_traverser(const aset& A, const atermpp::map<process_identifier, process_expression>& process_bodies)
    : super(A, process_bodies)
  {}

  // tau
  void leave(const process::tau& x)
  {
    multi_action_name tau;
    push(make_aset(tau));
  }

  // p1 || p2
  void operator()(const process::merge& x)
  {
    aset A1 = alphabet_block(x.left(), A, process_bodies);
    aset A2 = alphabet_block(x.right(), A, process_bodies);
    aset A1xA2 = times(A1, A2);
    aset A2A1xA2 = set_union(A2, A1xA2);
    aset A1A2A1xA2 = set_union(A1, A2A1xA2);
    push(A1A2A1xA2);
  }

  // p1 | p2
  void operator()(const process::sync& x)
  {
    aset A1 = alphabet_sub(x.left(), A, process_bodies);
    aset A2 = alphabet_block(x.right(), A, process_bodies);
    aset A1xA2 = times(A1, A2);
    push(A1xA2);
  }

  // block(B, p)
  void operator()(const process::block& x)
  {
    core::identifier_string_list B = x.block_set();
    push(set_intersection(make_aset(B), A));
  }

  // hide(I, p)
  void leave(const process::hide& x)
  {
    aset A1 = alphabet_allow(x.operand(), A, true, process_bodies);
    aset A2 = apply_hide(x.hide_set(), A1);
    push(A2);
  }

  // comm(A, p)
  void operator()(const process::comm& x)
  {
    communication_expression_list C = x.comm_set();
    aset A1 = apply_communication_inverse(C, A);
    aset Aprime = alphabet_allow(x, A1, false, process_bodies);
    aset CAprime = apply_communication(C, Aprime);
    core::identifier_string_list B = make_block_set(A);
    aset BCAprime = apply_block(B, CAprime);
    push(BCAprime);
  }

  // allow(A, p)
  void operator()(const process::allow& x)
  {
    aset V = make_aset(x.allow_set());
    multi_action_name tau;
    core::identifier_string_list B = make_block_set(A);
    aset dBV = apply_block(B, V);
    dBV.insert(tau);
    push(alphabet_allow(x, dBV, false, process_bodies));
  }
};

template <template <class> class Traverser>
struct apply_alphabet_traverser_all: public Traverser<apply_alphabet_traverser_all<Traverser> >
{
  typedef Traverser<apply_alphabet_traverser_all<Traverser> > super;
  using super::enter;
  using super::leave;
  using super::operator();

  apply_alphabet_traverser_all(const aset& A, bool A_is_all, const atermpp::map<process_identifier, process_expression>& process_bodies)
    : super(A, A_is_all, process_bodies)
  {}

#ifdef BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif
};

template <template <class> class Traverser>
struct apply_alphabet_traverser: public Traverser<apply_alphabet_traverser<Traverser> >
{
  typedef Traverser<apply_alphabet_traverser<Traverser> > super;
  using super::enter;
  using super::leave;
  using super::operator();

  apply_alphabet_traverser(const aset& A, const atermpp::map<process_identifier, process_expression>& process_bodies)
    : super(A, process_bodies)
  {}

#ifdef BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif
};

inline
aset alphabet_allow(const process_expression& x, const aset& A, bool A_is_all, const atermpp::map<process_identifier, process_expression>& process_bodies)
{
  apply_alphabet_traverser_all<alphabet_allow_traverser> f(A, A_is_all, process_bodies);
  f(x);
  return f.result_stack.back();
}

inline
aset alphabet_allow(const process_expression& x, const aset& A, bool A_is_all, const process_specification& procspec)
{
  atermpp::map<process_identifier, process_expression> process_bodies;
  const atermpp::vector<process_equation>& equations = procspec.equations();
  for (atermpp::vector<process_equation>::const_iterator i = equations.begin(); i != equations.end(); ++i)
  {
    process_bodies[i->identifier()] = i->expression();
  }
  return alphabet_allow(x, A, A_is_all, process_bodies);
}

inline
aset alphabet_sub(const process_expression& x, const aset& A, const atermpp::map<process_identifier, process_expression>& process_bodies)
{
  apply_alphabet_traverser<alphabet_sub_traverser> f(A, process_bodies);
  f(x);
  return f.result_stack.back();
}

inline
aset alphabet_sub(const process_expression& x, const aset& A, const process_specification& procspec)
{
  atermpp::map<process_identifier, process_expression> process_bodies;
  const atermpp::vector<process_equation>& equations = procspec.equations();
  for (atermpp::vector<process_equation>::const_iterator i = equations.begin(); i != equations.end(); ++i)
  {
    process_bodies[i->identifier()] = i->expression();
  }
  return alphabet_sub(x, A, process_bodies);
}

inline
aset alphabet_block(const process_expression& x, const aset& A, const atermpp::map<process_identifier, process_expression>& process_bodies)
{
  apply_alphabet_traverser<alphabet_block_traverser> f(A, process_bodies);
  f(x);
  return f.result_stack.back();
}

inline
aset alphabet_block(const process_expression& x, const aset& A, const process_specification& procspec)
{
  atermpp::map<process_identifier, process_expression> process_bodies;
  const atermpp::vector<process_equation>& equations = procspec.equations();
  for (atermpp::vector<process_equation>::const_iterator i = equations.begin(); i != equations.end(); ++i)
  {
    process_bodies[i->identifier()] = i->expression();
  }
  return alphabet_block(x, A, process_bodies);
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_ALPHABET_H
