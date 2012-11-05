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
#include <iostream>
#include <sstream>
#include <boost/logic/tribool.hpp>
#include <boost/logic/tribool_io.hpp>
#include "mcrl2/data/set_identifier_generator.h"
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
  if (A.empty())
  {
    return delta();
  }

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
};

inline
std::ostream& operator<<(std::ostream& out, const alphabet_node& x)
{
  return out << "alphabet = " << lps::pp(x.alphabet);
}

struct push_allow_node: public alphabet_node
{
  push_allow_node(const multi_action_name_set& alphabet, const process_expression& expression = process_expression(), boost::logic::tribool needs_allow = boost::logic::indeterminate)
    : alphabet_node(alphabet), m_expression(expression), m_needs_allow(needs_allow)
  {}

/*
  // computes the process expression corresponding to this node
  process_expression expression(const multi_action_name_set& A, bool A_includes_subsets) const
  {
    //std::cout << "<expression> A = " << lps::pp(A) << " alphabet = " << lps::pp(alphabet) << " m_needs_allow = " << std::boolalpha << m_needs_allow;
    multi_action_name_set alphabet1 = alphabet;
    bool needs_allow;
    if (boost::logic::indeterminate(m_needs_allow))
    {
      multi_action_name_set alphabet1 = set_intersection(alphabet1, A, A_includes_subsets);
    }
    else
    {
      needs_allow = m_needs_allow;
    }
    //std::cout << " alphabet1 = " << lps::pp(alphabet1) << " needs_allow = " << needs_allow << std::endl;
    if (needs_allow)
    {
      return make_allow(alphabet1, m_expression);
    }
    else
    {
      return m_expression;
    }
  }
*/

  std::string print(const multi_action_name_set& A, bool A_includes_subsets) const
  {
    std::ostringstream out;
    if (m_needs_allow)
    {
      out << "allow(" << lps::pp(set_intersection(alphabet, A, A_includes_subsets)) << ", " << process::pp(m_expression) << ")";
    }
    else if (!m_needs_allow)
    {
      out << process::pp(m_expression);
    }
    else
    {
      out << "allow?(" << lps::pp(set_intersection(alphabet, A, A_includes_subsets)) << ", " << process::pp(m_expression) << ")";
    }
    out << " with alphabet = " << lps::pp(alphabet);
    return out.str();
  }

  void finish(const multi_action_name_set& A, bool A_includes_subsets)
  {
    if (boost::logic::indeterminate(m_needs_allow))
    {
      multi_action_name_set alphabet1 = set_intersection(alphabet, A, A_includes_subsets);
      m_needs_allow = alphabet1.size() < alphabet.size();
      alphabet = alphabet1;
    }
    if (m_needs_allow)
    {
      m_expression = make_allow(alphabet, m_expression);
    }
    m_needs_allow = false;
  }

  process_expression m_expression;

  // if true, the alphabet of this node is included in the allow set A
  boost::logic::tribool m_needs_allow;
};

inline
std::ostream& operator<<(std::ostream& out, const push_allow_node& x)
{
  return out << "alphabet = " << lps::pp(x.alphabet) << " expression = " << process::pp(x.m_expression) << " needs_allow = " << std::boolalpha << x.m_needs_allow << std::endl;
}

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

  // Push a node to node_stack
  void push(const Node& node)
  {
    mCRL2log(log::debug1) << "<push> A = " << lps::pp(node.alphabet) << std::endl;
    node_stack.push_back(node);
  }

  // Push A to node_stack
  void push(const multi_action_name_set& A)
  {
    push(Node(A));
  }

  // Pop the top element of node_stack and return it
  Node pop()
  {
    Node result = node_stack.back();
    mCRL2log(log::debug1) << "<pop> A = " << lps::pp(result.alphabet) << std::endl;
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
    push(merge_union(left.alphabet, right.alphabet));
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
    top().alphabet = process::apply_block(x.block_set(), top().alphabet);
  }

  void leave(const process::hide& x)
  {
    top().alphabet = process::apply_hide(x.hide_set(), top().alphabet);
  }

  void leave(const process::rename& x)
  {
    top().alphabet = process::apply_rename(x.rename_set(), top().alphabet);
  }

  void leave(const process::comm& x)
  {
    top().alphabet = process::apply_comm(x.comm_set(), top().alphabet);
  }

  void leave(const process::allow& x)
  {
    top().alphabet = process::apply_allow(x.allow_set(), top().alphabet);
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

  void log(const process_expression& x)
  {
    mCRL2log(log::debug) << "allow(" << lps::pp(A) << (A_includes_subsets ? "*" : "") << ", " << process::pp(x) << ") -> " << top().print(A, A_includes_subsets) << std::endl;
  }

  void leave_pcrl(const process_expression& x)
  {
    top().m_expression = x;
    top().m_needs_allow = boost::logic::indeterminate;
    log(x);
  }

  void leave(const lps::action& x)
  {
    super::leave(x);
    leave_pcrl(x);
  }

  void leave(const process::process_instance& x)
  {
    super::leave(x);
    leave_pcrl(x);
  }

  void leave(const process::process_instance_assignment& x)
  {
    super::leave(x);
    leave_pcrl(x);
  }

  void leave(const process::delta& x)
  {
    super::leave(x);
    leave_pcrl(x);
  }

  void leave(const process::tau& x)
  {
    super::leave(x);
    leave_pcrl(x);
  }

  void leave(const process::sum& x)
  {
    super::leave(x);
    leave_pcrl(x);
  }

  void leave(const process::block& x)
  {
    core::identifier_string_list B = x.block_set();
    multi_action_name_set A1 = process::apply_block(B, A);
    push_allow_node node = push_allow(x.operand(), A1, false, equations);
    node.finish(A1, false);
    push(node);
    log(x);
  }

  void leave(const process::hide& x)
  {
    super::leave(x);
    top().m_expression = x;
    log(x);
  }

  void leave(const process::rename& x)
  {
    rename_expression_list R = x.rename_set();
    multi_action_name_set A1 = process::apply_rename_inverse(R, A);
    push_allow_node node = push_allow(x.operand(), A1, false, equations);
    node.m_expression = rename(R, node.m_expression);
    node.finish(A1, false);
    push(node);
    log(x);
  }

  void leave(const process::comm& x)
  {
    communication_expression_list C = x.comm_set();
    multi_action_name_set A1 = set_union(A, process::apply_comm_inverse(C, A));
    push_allow_node node = push_allow(x.operand(), A1, false, equations);
    node.finish(A1, false);
    communication_expression_list C1 = filter_comm_set(x.comm_set(), node.alphabet);
    push(push_allow_node(process::apply_comm(C1, node.alphabet), comm(C1, node.m_expression), boost::logic::indeterminate));
    log(x);
  }

  void leave(const process::allow& x)
  {
    action_name_multiset_list V = x.allow_set();
    multi_action_name_set A1 = process::apply_allow(V, A);
    push_allow_node node = push_allow(x.operand(), A1, false, equations);
    node.finish(A1, false);
    push(node);
    log(x);
  }

  void leave(const process::at& x)
  {
    super::leave(x);
    leave_pcrl(x);
  }

  void leave(const process::seq& x)
  {
    super::leave(x);
    leave_pcrl(x);
  }

  void leave(const process::if_then& x)
  {
    super::leave(x);
    leave_pcrl(x);
  }

  void leave(const process::if_then_else& x)
  {
    leave_pcrl(x);
  }

  void leave(const process::bounded_init& x)
  {
    super::leave(x);
    leave_pcrl(x);
  }

  void operator()(const process::merge& x)
  {
    push_allow_node left = push_allow(x.left(), A, true, equations);
    left.finish(A, true);
    multi_action_name_set A1 = set_union(A, left_arrow(A, left.alphabet));
    push_allow_node right = push_allow(x.right(), A1, false, equations);
    right.finish(A1, false);
    push(push_allow_node(merge_union(left.alphabet, right.alphabet), merge(left.m_expression, right.m_expression), boost::logic::indeterminate));
    log(x);
  }

  void leave(const process::left_merge& x)
  {
    push_allow_node left = push_allow(x.left(), A, true, equations);
    left.finish(A, true);
    multi_action_name_set A1 = set_union(A, left_arrow(A, left.alphabet));
    push_allow_node right = push_allow(x.right(), A1, false, equations);
    right.finish(A1, false);
    push(push_allow_node(merge_union(left.alphabet, right.alphabet), left_merge(left.m_expression, right.m_expression), boost::logic::indeterminate));
    log(x);
  }

  void leave(const process::sync& x)
  {
    push_allow_node left = push_allow(x.left(), A, true, equations);
    left.finish(A, true);
    multi_action_name_set A1 = set_union(A, left_arrow(A, left.alphabet));
    push_allow_node right = push_allow(x.right(), A1, false, equations);
    right.finish(A1, false);
    push(push_allow_node(concat(left.alphabet, right.alphabet), sync(left.m_expression, right.m_expression), boost::logic::indeterminate));
    log(x);
  }

  void leave(const process::choice& x)
  {
    super::leave(x);
    leave_pcrl(x);
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
  f.node_stack.back().finish(A, A_includes_subsets);
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
    push_allow_node node = push_allow(x.operand(), A, false, equations);
    node.finish(A, false);
    return node.m_expression;
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
