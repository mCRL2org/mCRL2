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
aset alphabet_nabla(const process_expression& x, const aset& A);
aset alphabet_sub(const process_expression& x, const aset& A);
aset alphabet_super(const process_expression& x, const aset& A);

template <typename Derived>
struct alphabet_nabla_traverser: public process_expression_traverser<Derived>
{
  typedef process_expression_traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();

#if BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif

  const aset& A;
  std::vector<aset> result_stack;

  // N.B. the empty aset represents the set of all multi action names
  bool is_all(const aset& A) const
  {
    return A.empty();
  }

  // Constructor
  alphabet_nabla_traverser(const aset& A_)
    : A(A_)
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

  aset make_aset(const multi_action_name& a) const
  {
    aset s;
    s.insert(a);
    return s;
  }

  // N.B. tau is represented by the empty set!
  multi_action_name tau() const
  {
    return multi_action_name();
  }

  // joins the top two elements of the stack
  void join()
  {
    aset right = pop();
    aset left = pop();
    push(set_union(left, right));
  }

  // a(e1, ..., en)
  void leave(const lps::action& x)
  {
    multi_action_name a = name(x);
    const aset& A = top();
    if (A.find(a) == A.end())
    {
      push(aset());
    }
    else
    {
      push(make_aset(a));
    }
  }

  // P(e1, ..., en)
  void leave(const process::process_instance& x)
  {
    // TODO: not implemented yet
    throw std::runtime_error("not implemented yet");
  }

  // P(d1 = e1, ..., dn = en)
  void leave(const process::process_instance_assignment& x)
  {
    // TODO: not implemented yet
    throw std::runtime_error("not implemented yet");
  }

  // delta
  void leave(const process::delta& x)
  {
    push(aset());
  }

  // tau
  void leave(const process::tau& x)
  {
    const aset& A = top();
    multi_action_name tau;
    if (A.find(tau) == A.end())
    {
      push(aset());
    }
    else
    {
      push(make_aset(tau));
    }
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

  // p1 || p2
  void operator()(const process::merge& x) // use operator() here, since we don't want to go into the default recursion
  {
    aset A1 = alphabet_sub(x.left(), A);
    aset AA1 = left_arrow(A, A1);
    aset AAA1 = set_union(A, AA1);
    aset A2 = alphabet_nabla(x.right(), AAA1);
    aset A1xA2 = times(A1, A2);
    aset A2A1xA2 = set_union(A2, A1xA2);
    aset A1A2A1xA2 = set_union(A1, A2A1xA2);
    aset AA1A2A1xA2 = set_intersection(A, A1A2A1xA2);
    push(AA1A2A1xA2);
  }

  // p1 ||_ p2
  void operator()(const process::left_merge& x)
  {
    aset A1 = alphabet_sub(x.left(), A);
    aset AA1 = left_arrow(A, A1);
    aset AAA1 = set_union(A, AA1);
    aset A2 = alphabet_nabla(x.right(), AAA1);
    aset A1xA2 = times(A1, A2);
    aset A2A1xA2 = set_union(A2, A1xA2);
    aset A1A2A1xA2 = set_union(A1, A2A1xA2);
    aset AA1A2A1xA2 = set_intersection(A, A1A2A1xA2);
    push(AA1A2A1xA2);
  }

  // p1 | p2
  void operator()(const process::sync& x)
  {
    aset A1 = alphabet_sub(x.left(), A);
    aset AA1 = left_arrow(A, A1);
    aset AAA1 = set_union(A, AA1);
    aset A2 = alphabet_nabla(x.right(), AAA1);
    aset A1xA2 = times(A1, A2);
    aset AA1xA2 = set_intersection(A, A1xA2);
    push(AA1xA2);
  }

  // block(B, p)
  void leave(const process::block& x)
  {
    //if (is_all(A))
    //{
    //  core::identifier_string_list b = x.block_set();
    //  multi_action_name B(b.begin(), b.end());
    //  A = B;
    //}
    // TODO: not implemented yet
    throw std::runtime_error("not implemented yet");
  }

  // hide(I, p)
  void leave(const process::hide& x)
  {
    // TODO: not implemented yet
    throw std::runtime_error("not implemented yet");
  }

  // rename(R, p)
  void leave(const process::rename& x)
  {
    // TODO: not implemented yet
    throw std::runtime_error("not implemented yet");
  }

  // comm(C, p)
  void operator()(const process::comm& x)
  {
    communication_expression_list C = x.comm_set();
    aset CinverseA = apply_communication_inverse(C, A);
    aset ACinverseA = set_union(A, CinverseA);
    aset Aprime = alphabet_nabla(x, ACinverseA);
    aset CAprime = apply_communication(C, Aprime);
    aset ACAprime = set_intersection(A, CAprime);
    push(ACAprime);
  }

  // allow(A, p)
  void operator()(const process::allow& x)
  {
    aset V = make_aset(x.allow_set());
    multi_action_name tau;
    V.insert(tau);
    aset AV = set_intersection(A, V);
    push(alphabet_nabla(x, AV));
  }
};

//////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename Derived>
struct alphabet_sub_traverser: public alphabet_nabla_traverser<Derived>
{
  typedef alphabet_nabla_traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();
  using super::name;
  using super::top;
  using super::push;
  using super::make_aset;
  using super::join;
  using super::A;

#if BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif

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
    aset A1 = alphabet_sub(x.left(), A);
    aset A2 = alphabet_sub(x.right(), A);
    aset A1xA2 = times(A1, A2);
    aset A2A1xA2 = set_union(A2, A1xA2);
    aset A1A2A1xA2 = set_union(A1, A2A1xA2);
    aset AA1A2A1xA2 = subset_intersection(A, A1A2A1xA2);
    push(AA1A2A1xA2);
  }

  // p1 ||_ p2
  void operator()(const process::left_merge& x)
  {
    aset A1 = alphabet_sub(x.left(), A);
    aset A2 = alphabet_sub(x.right(), A);
    aset A1xA2 = times(A1, A2);
    aset A2A1xA2 = set_union(A2, A1xA2);
    aset A1A2A1xA2 = set_union(A1, A2A1xA2);
    aset AA1A2A1xA2 = subset_intersection(A, A1A2A1xA2);
    push(AA1A2A1xA2);
  }

  // p1 | p2
  void operator()(const process::sync& x)
  {
    aset A1 = alphabet_sub(x.left(), A);
    aset A2 = alphabet_sub(x.right(), A);
    aset A1xA2 = times(A1, A2);
    aset AA1xA2 = subset_intersection(A, A1xA2);
    push(AA1xA2);
  }

  // block(B, p)
  void leave(const process::block& x)
  {
    // TODO: not implemented yet
    throw std::runtime_error("not implemented yet");
  }

  // hide(I, p)
  void leave(const process::hide& x)
  {
    // TODO: not implemented yet
    throw std::runtime_error("not implemented yet");
  }

  // rename(, p)
  void leave(const process::rename& x)
  {
    // TODO: not implemented yet
    throw std::runtime_error("not implemented yet");
  }

  // comm(A, p)
  void operator()(const process::comm& x)
  {
    communication_expression_list C = x.comm_set();
    aset CinverseA = apply_communication_inverse(C, A);
    aset ACinverseA = set_union(A, CinverseA);
    aset Aprime = alphabet_nabla(x, ACinverseA);
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
    push(alphabet_nabla(x, AV));
  }
};

inline
aset alphabet_nabla(const process_expression& x, const aset& A)
{
  return aset();
}

inline
aset alphabet_sub(const process_expression& x, const aset& A)
{
  return aset();
}

inline
aset alphabet_super(const process_expression& x, const aset& A)
{
  return aset();
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_ALPHABET_H
