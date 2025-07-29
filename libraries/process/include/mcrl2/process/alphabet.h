// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/alphabet.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_ALPHABET_H
#define MCRL2_PROCESS_ALPHABET_H

#include "mcrl2/process/alphabet_operations.h"
#include "mcrl2/process/utility.h"

namespace mcrl2::process
{

namespace detail {

struct alphabet_node
{
  multi_action_name_set alphabet;

  alphabet_node() = default;

  alphabet_node(const multi_action_name_set& alphabet_)
    : alphabet(alphabet_)
  {}
};

inline
std::ostream& operator<<(std::ostream& out, const alphabet_node& x)
{
  return out << "alphabet = " << pp(x.alphabet);
}

/// \brief Traverser that computes the alphabet of process expressions
template <typename Derived, typename Node = alphabet_node>
struct alphabet_traverser: public process_expression_traverser<Derived>
{
  using super = process_expression_traverser<Derived>;
  using super::enter;
  using super::leave;
  using super::apply;

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  const std::vector<process_equation>& equations;
  std::set<process_identifier>& W;
  std::vector<Node> node_stack;

  alphabet_traverser(const std::vector<process_equation>& equations_, std::set<process_identifier>& W_)
    : equations(equations_), W(W_)
  {}

  // Push a node to node_stack
  void push(const Node& node)
  {
    mCRL2log(log::trace) << "<push> A = " << pp(node.alphabet) << std::endl;
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
    mCRL2log(log::trace) << "<pop> A = " << pp(result.alphabet) << std::endl;
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
    push(alphabet_operations::set_union(left.alphabet, right.alphabet));
  }

  // Pops two elements A1 and A2 from the stack, and pushes back union(A1, A2, A1 | A2)
  void join_merge()
  {
    Node right = pop();
    Node left = pop();
    push(alphabet_operations::merge(left.alphabet, right.alphabet));
  }

  // Pops two elements A1 and A2 from the stack, and pushes back A1 | A2
  void join_sync()
  {
    Node right = pop();
    Node left = pop();
    push(alphabet_operations::sync(left.alphabet, right.alphabet));
  }

  void leave(const process::action& x)
  {
    multi_action_name alpha;
    alpha.insert(x.label().name());
    multi_action_name_set A;
    A.insert(alpha);
    push(A);
  }

  void leave(const process::process_instance& x)
  {
    using utilities::detail::contains;
    if (!contains(W, x.identifier()))
    {
      W.insert(x.identifier());
      const process_equation& eqn = find_equation(equations, x.identifier());
      derived().apply(eqn.expression());
      W.erase(x.identifier());
    }
    else
    {
      push(multi_action_name_set());
    }
  }

  void leave(const process::process_instance_assignment& x)
  {
    using utilities::detail::contains;
    if (!contains(W, x.identifier()))
    {
      W.insert(x.identifier());
      const process_equation& eqn = find_equation(equations, x.identifier());
      derived().apply(eqn.expression());
      W.erase(x.identifier());
    }
    else
    {
      push(multi_action_name_set());
    }
  }

  void leave(const process::delta& /* x */)
  {
    push(multi_action_name_set());
  }

  void leave(const process::tau& /* x */)
  {
    multi_action_name_set A;
    A.insert(multi_action_name()); // A = { tau }
    push(A);
  }

  void leave(const process::sum& /* x */)
  {
  }

  void leave(const process::block& x)
  {
    top().alphabet = alphabet_operations::block(x.block_set(), top().alphabet);
  }

  void leave(const process::hide& x)
  {
    top().alphabet = alphabet_operations::hide(x.hide_set(), top().alphabet);
  }

  void leave(const process::rename& x)
  {
    top().alphabet = alphabet_operations::rename(x.rename_set(), top().alphabet);
  }

  void leave(const process::comm& x)
  {
    top().alphabet = alphabet_operations::comm(x.comm_set(), top().alphabet);
  }

  void leave(const process::allow& x)
  {
    top().alphabet = alphabet_operations::allow(x.allow_set(), top().alphabet);
  }

  void leave(const process::sync& /* x */)
  {
    join_sync();
  }

  void leave(const process::at& /* x */)
  {
  }

  void leave(const process::seq& /* x */)
  {
    join();
  }

  void leave(const process::if_then& /* x */)
  {
  }

  void leave(const process::if_then_else& /* x */)
  {
    join();
  }

  void leave(const process::bounded_init& /* x */)
  {
    join();
  }

  void leave(const process::merge& /* x */)
  {
    join_merge();
  }

  void leave(const process::left_merge& /* x */)
  {
    join_merge();
  }

  void leave(const process::choice& /* x */)
  {
    join();
  }
};

struct apply_alphabet_traverser: public alphabet_traverser<apply_alphabet_traverser>
{
  using super = alphabet_traverser<apply_alphabet_traverser>;
  using super::enter;
  using super::leave;
  using super::apply;
  using super::node_stack;

  apply_alphabet_traverser(const std::vector<process_equation>& equations, std::set<process_identifier>& W)
    : super(equations, W)
  {}
};

inline
alphabet_node alphabet(const process_expression& x, const std::vector<process_equation>& equations, std::set<process_identifier>& W)
{
  detail::apply_alphabet_traverser f(equations, W);
  f.apply(x);
  return f.node_stack.back();
}

} // namespace detail

inline
multi_action_name_set alphabet(const process_expression& x, const std::vector<process_equation>& equations)
{
  std::set<process_identifier> W;
  return detail::alphabet(x, equations, W).alphabet;
}

} // namespace mcrl2::process

#endif // MCRL2_PROCESS_ALPHABET_H
