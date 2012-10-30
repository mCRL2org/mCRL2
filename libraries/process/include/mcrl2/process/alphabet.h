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
#include "mcrl2/data/set_identifier_generator.h"
#include "mcrl2/process/detail/alphabet_utility.h"
#include "mcrl2/process/find.h"
#include "mcrl2/process/builder.h"
#include "mcrl2/process/traverser.h"
#include "mcrl2/process/utility.h"
#include "mcrl2/utilities/logger.h"

namespace mcrl2 {

namespace process {

namespace detail {

inline
bool contains_tau(const multi_action_name_set& A)
{
  multi_action_name tau;
  return A.find(tau) != A.end();
}

inline
process_expression make_allow(const multi_action_name_set& A, const process_expression& x)
{
  assert(!contains_tau(A));

  // convert A to an action_name_multiset_list B
  atermpp::vector<action_name_multiset> v;
  for (multi_action_name_set::const_iterator i = A.begin(); i != A.end(); ++i)
  {
    const multi_action_name& alpha = *i;
    v.push_back(action_name_multiset(core::identifier_string_list(alpha.begin(), alpha.end())));
  }
  action_name_multiset_list B(v.begin(), v.end());
  return allow(B, x);
}

struct alphabet_node
{
  multi_action_name_set alphabet;

  alphabet_node(const multi_action_name_set& alphabet_)
    : alphabet(alphabet_)
  {}

  std::string print() const
  {
    std::ostringstream out;
    out << "alphabet = " << lps::pp(alphabet);
    return out.str();
  }
};

struct push_allow_node: public alphabet_node
{
  push_allow_node(const multi_action_name_set& alphabet, const process_expression& expression = process_expression(), bool true_intersection = false)
    : alphabet_node(alphabet), m_expression(expression), m_true_intersection(true_intersection)
  {}

  process_expression expression() const
  {
    if (m_true_intersection)
    {
      return make_allow(alphabet, m_expression);
    }
    else
    {
      return m_expression;
    }
  }

  std::string print() const
  {
    std::ostringstream out;
    out << "alphabet = " << lps::pp(alphabet) << " expression = " << process::pp(m_expression) << " needs allow = " << std::boolalpha << m_true_intersection << std::endl;
    return out.str();
  }

  process_expression m_expression;

  // if true, then there were elements removed from the alphabet using the intersection with the bound set A
  bool m_true_intersection;
};

push_allow_node push_allow(const process_expression& x, const multi_action_name_set& A, bool A_includes_subsets, const atermpp::vector<process_equation>& equations);

/// \brief Traverser that computes the alphabet of process expressions
template <typename Derived, typename Node = alphabet_node>
struct alphabet_traverser: public process_expression_traverser<Derived>
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

  const atermpp::vector<process_equation>& equations;
  std::set<process_identifier>& W;
  atermpp::vector<Node> node_stack;

  alphabet_traverser(const atermpp::vector<process_equation>& equations_, std::set<process_identifier>& W_)
    : equations(equations_), W(W_)
  {}

  // Push A to node_stack
  void push(const multi_action_name_set& A)
  {
    mCRL2log(log::debug) << "<push> A = " << lps::pp(A) << std::endl;
    node_stack.push_back(Node(A));
  }

  // Push a node to node_stack
  void push(const Node& node)
  {
    mCRL2log(log::debug) << "<push> A = " << lps::pp(node.alphabet) << std::endl;
    node_stack.push_back(node);
  }

  // Pop the top element of node_stack and return it
  Node pop()
  {
    Node result = node_stack.back();
    mCRL2log(log::debug) << "<pop> A = " << lps::pp(result.alphabet) << std::endl;
    node_stack.pop_back();
    return result;
  }

  // Return the top element of node_stack
  Node& top()
  {
    return node_stack.back();
  }

  // Return the top element of node_stack
  const Node& top() const
  {
    return node_stack.back();
  }

  // Pops two elements A1 and A2 from the stack, and pushes back union(A1, A2)
  void join()
  {
    Node right = pop();
    Node left = pop();
    push(set_union(left.alphabet, right.alphabet));
  }

  // Pops two elements A1 and A2 from the stack, and pushes back union(A1, A2, A1 | A2)
  void join_merge()
  {
    Node right = pop();
    Node left = pop();
    push(set_union(set_union(left.alphabet, right.alphabet), concat(left.alphabet, right.alphabet)));
  }

  // Pops two elements A1 and A2 from the stack, and pushes back A1 | A2
  void join_sync()
  {
    Node right = pop();
    Node left = pop();
    push(concat(left.alphabet, right.alphabet));
  }

  void leave(const lps::action& x)
  {
    multi_action_name alpha;
    alpha.insert(x.label().name());
    multi_action_name_set A;
    A.insert(alpha);
    push(A);
  }

  void leave(const process::process_instance& x)
  {
    if (W.find(x.identifier()) == W.end())
    {
      W.insert(x.identifier());
      const process_equation& eqn = find_equation(equations, x.identifier());
      derived()(eqn.expression());
      W.erase(x.identifier());
    }
    else
    {
      push(multi_action_name_set());
    }
  }

  void leave(const process::process_instance_assignment& x)
  {
    if (W.find(x.identifier()) == W.end())
    {
      W.insert(x.identifier());
      const process_equation& eqn = find_equation(equations, x.identifier());
      derived()(eqn.expression());
      W.erase(x.identifier());
    }
    else
    {
      push(multi_action_name_set());
    }
  }

  void leave(const process::delta& x)
  {
    push(multi_action_name_set());
  }

  void leave(const process::tau& x)
  {
    multi_action_name_set A;
    A.insert(multi_action_name()); // A = { tau }
    push(A);
  }

  void leave(const process::sum& x)
  {
  }

  void leave(const process::block& x)
  {
    top() = process::apply_block(x.block_set(), top().alphabet);
  }

  void leave(const process::hide& x)
  {
    top() = process::apply_hide(x.hide_set(), top().alphabet);
  }

  void leave(const process::rename& x)
  {
    top() = process::apply_rename(x.rename_set(), top().alphabet);
  }

  void leave(const process::comm& x)
  {
    top() = process::apply_comm(x.comm_set(), top().alphabet);
  }

  void leave(const process::allow& x)
  {
    top() = process::apply_allow(x.allow_set(), top().alphabet);
  }

  void leave(const process::sync& x)
  {
    join_sync();
  }

  void leave(const process::at& x)
  {
  }

  void leave(const process::seq& x)
  {
    join();
  }

  void leave(const process::if_then& x)
  {
  }

  void leave(const process::if_then_else& x)
  {
    join();
  }

  void leave(const process::bounded_init& x)
  {
    join();
  }

  void leave(const process::merge& x)
  {
    join_merge();
  }

  void leave(const process::left_merge& x)
  {
    join_merge();
  }

  void leave(const process::choice& x)
  {
    join();
  }
};

struct apply_alphabet_traverser: public alphabet_traverser<apply_alphabet_traverser>
{
  typedef alphabet_traverser<apply_alphabet_traverser> super;
  using super::enter;
  using super::leave;
  using super::operator();
  using super::node_stack;

#if BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif

  apply_alphabet_traverser(const atermpp::vector<process_equation>& equations, std::set<process_identifier>& W)
    : alphabet_traverser(equations, W)
  {}
};

inline
alphabet_node alphabet(const process_expression& x, const atermpp::vector<process_equation>& equations, std::set<process_identifier>& W)
{
  apply_alphabet_traverser f(equations, W);
  f(x);
  return f.node_stack.back();
}

/*
  // remove each element x from node.alphabet for which no y in A exists such that x is included in A
  // TODO: the efficiency of this operation can probably be improved
  void filter_alphabet(Node& node, bool A_includes_subsets = false)
  {
    for (multi_action_name_set::iterator i = node.alphabet.begin(); i != node.alphabet.end(); )
    {
      bool remove = !includes(A, *i);
      if (exact)
      {
        remove = remove || A.find(*i) == A.end();
      }
      if (remove)
      {
        node.alphabet.erase(i++);
        node.m_true_intersection = true;
      }
      else
      {
        ++i;
      }
    }
  }
*/

template <typename Derived, typename Node = push_allow_node>
struct push_allow_traverser: public alphabet_traverser<Derived, Node>
{
  typedef alphabet_traverser<Derived, Node> super;
  using super::enter;
  using super::leave;
  using super::operator();
  using super::W;
  using super::push;
  using super::pop;
  using super::top;
  using super::node_stack;
  using super::equations;

#if BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif

  // the parameter A
  const multi_action_name_set& A;

  // if true, interpret A as 'A_subset'
  bool A_includes_subsets;

  push_allow_traverser(const atermpp::vector<process_equation>& equations, std::set<process_identifier>& W, const multi_action_name_set& A_, bool A_includes_subsets_)
    : super(equations, W), A(A_), A_includes_subsets(A_includes_subsets_)
  {}

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  void leave(const lps::action& x)
  {
    super::leave(x);
    top().m_expression = x;
  }

  void leave(const process::process_instance& x)
  {
    super::leave(x);
    top().m_expression = x;
  }

  void leave(const process::process_instance_assignment& x)
  {
    super::leave(x);
    top().m_expression = x;
  }

  void leave(const process::delta& x)
  {
    super::leave(x);
    top().m_expression = x;
  }

  void leave(const process::tau& x)
  {
    super::leave(x);
    top().m_expression = x;
  }

  void leave(const process::sum& x)
  {
    super::leave(x);
    top().m_expression = x;
  }

  void leave(const process::block& x)
  {
    super::leave(x);
    top().m_expression = x;
  }

  void leave(const process::hide& x)
  {
    super::leave(x);
    top().m_expression = x;
  }

  void leave(const process::rename& x)
  {
    super::leave(x);
    top().m_expression = x;
  }

  void leave(const process::comm& x)
  {
    super::leave(x);
    top().m_expression = x;
  }

  void leave(const process::allow& x)
  {
    super::leave(x);
    top().m_expression = x;
  }

  void leave(const process::sync& x)
  {
    super::leave(x);
    top().m_expression = x;
  }

  void leave(const process::at& x)
  {
    super::leave(x);
    top().m_expression = x;
  }

  void leave(const process::seq& x)
  {
    super::leave(x);
    top().m_expression = x;
  }

  void leave(const process::if_then& x)
  {
    super::leave(x);
    top().m_expression = x;
  }

  void leave(const process::if_then_else& x)
  {
    super::leave(x);
    top().m_expression = x;
  }

  void leave(const process::bounded_init& x)
  {
    super::leave(x);
    top().m_expression = x;
  }

  void operator()(const process::merge& x)
  {
    push_allow_node left = push_allow(x.left(), A, true, equations);
    push_allow_node right = push_allow(x.right(), set_union(A, left_arrow(A, left.alphabet)), false, equations);
    push(push_allow_node(set_union(left.alphabet, right.alphabet), merge(left.expression(), right.expression()), false));
  }

  void leave(const process::left_merge& x)
  {
    super::leave(x);
    top().m_expression = x;
  }

  void leave(const process::choice& x)
  {
    super::leave(x);
    top().m_expression = x;
  }
};

template <template <class, class> class Traverser, typename Node = push_allow_node>
struct apply_push_allow_traverser: public Traverser<apply_push_allow_traverser<Traverser, Node>, Node>
{
  typedef Traverser<apply_push_allow_traverser<Traverser, Node>, Node> super;
  using super::enter;
  using super::leave;
  using super::operator();

  apply_push_allow_traverser(const atermpp::vector<process_equation>& equations, std::set<process_identifier>& W, const multi_action_name_set& A, bool A_includes_subsets)
    : super(equations, W, A, A_includes_subsets)
  {}

#ifdef BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif
};

inline
push_allow_node push_allow(const process_expression& x, const multi_action_name_set& A, bool A_includes_subsets, const atermpp::vector<process_equation>& equations)
{
  std::set<process_identifier> W;
  apply_push_allow_traverser<push_allow_traverser> f(equations, W, A, A_includes_subsets);
  f(x);
  mCRL2log(log::debug) << "<push_allow result>" << f.node_stack.back().print() << std::endl;
  return f.node_stack.back();
}

struct alphabet_reduce_builder: public process_expression_builder<alphabet_reduce_builder>
{
  typedef process_expression_builder<alphabet_reduce_builder> super;
  using super::enter;
  using super::leave;
  using super::operator();

#if BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif

  const atermpp::vector<process_equation>& equations;

  alphabet_reduce_builder(const atermpp::vector<process_equation>& equations_)
    : equations(equations_)
  {}

  process_expression operator()(const process::allow& x)
  {
    multi_action_name_set A = make_name_set(x.allow_set());
    return push_allow(x.operand(), A, false, equations).expression();
  }
};

inline
process_expression alphabet_reduce(const process_expression& x, const atermpp::vector<process_equation>& equations)
{
  alphabet_reduce_builder f(equations);
  return f(x);
}

} // detail

inline
multi_action_name_set alphabet(const process_expression& x, const atermpp::vector<process_equation>& equations)
{
  std::set<process_identifier> W;
  return detail::alphabet(x, equations, W).alphabet;
}

inline
void alphabet_reduce(process_specification& procspec)
{
  procspec.init() = detail::alphabet_reduce(procspec.init(), procspec.equations());
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_ALPHABET_H
