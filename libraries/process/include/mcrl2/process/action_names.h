// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/action_names.h
/// \brief facilities to compute an overapproximation of the set of action names
/// that can occur in a process expression.

#ifndef MCRL2_PROCESS_ACTION_NAMES_H
#define MCRL2_PROCESS_ACTION_NAMES_H

#include "mcrl2/core/identifier_string.h"
#include "mcrl2/process/alphabet_operations.h"
#include "mcrl2/process/find.h"
#include "mcrl2/process/multi_action_name.h"
#include "mcrl2/process/process_expression.h"
#include "mcrl2/process/traverser.h"
#include "mcrl2/utilities/detail/container_utility.h"
#include <algorithm>

namespace mcrl2::process
{

using action_name_set = std::set<core::identifier_string>;

namespace detail {

struct action_names_node
{
  std::set<core::identifier_string> action_names;

  action_names_node() = default;

  action_names_node(const action_name_set& action_names_)
    : action_names(action_names_)
  {}
};

inline
std::ostream& operator<<(std::ostream& out, const action_names_node& x)
{
  return out << "action_names = " << core::pp(x.action_names);
}

/// \brief Traverser that computes the action names of process expressions
template <typename Derived, typename Node = action_names_node>
struct action_names_traverser: public process_expression_traverser<Derived>
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

  action_names_traverser(const std::vector<process_equation>& equations_, std::set<process_identifier>& W_)
    : equations(equations_), W(W_)
  {}

  // Push a node to node_stack
  void push(const Node& node)
  {
    mCRL2log(log::trace) << "<push> A = " << core::pp(node.action_names) << std::endl;
    node_stack.push_back(node);
  }

  // Push A to node_stack
  void push(const action_name_set& A) { push(Node(A)); }

  // Pop the top element of node_stack and return it
  Node pop()
  {
    Node result = node_stack.back();
    mCRL2log(log::trace) << "<pop> A = " << core::pp(result.action_names) << std::endl;
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
    push(utilities::detail::set_union(left.action_names, right.action_names));
  }

  void leave(const process::action& x)
  {
    action_name_set A({ x.label().name() });
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
      push(action_name_set());
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
      push(action_name_set());
    }
  }

  void leave(const process::delta& /* x */)
  {
    push(action_name_set());
  }

  void leave(const process::tau& /* x */)
  {
    push(action_name_set());
  }

  void leave(const process::sum& /* x */)
  {
  }

  void leave(const process::block& x)
  {
    top().action_names = utilities::detail::set_difference(top().action_names, std::set<core::identifier_string>(x.block_set().begin(), x.block_set().end()));
  }

  void leave(const process::hide& x)
  {
    top().action_names = utilities::detail::set_difference(top().action_names, std::set<core::identifier_string>(x.hide_set().begin(), x.hide_set().end()));
  }

  void leave(const process::rename& x)
  {
    action_name_set renamed_action_names;
    std::transform(top().action_names.begin(),
      top().action_names.end(),
      std::inserter(renamed_action_names, renamed_action_names.end()),
      [&x](const core::identifier_string& a)
      {
        for (const rename_expression& r: x.rename_set())
        {
          if (a == r.source())
          {
            return r.target();
          }
        }
        return a;
      });
    std::swap(top().action_names, renamed_action_names);
  }

  void leave(const process::comm& x)
  {
    for (const communication_expression& c: x.comm_set())
    {
      const core::identifier_string_list& lhs_names(c.action_name().names());
      std::set<core::identifier_string> lhs_set(lhs_names.begin(), lhs_names.end());
      if (utilities::detail::set_includes(top().action_names, lhs_set))
      {
        top().action_names.insert(c.name());
      }
    }
  }

  void leave(const process::allow& x)
  {
    std::set<core::identifier_string> allow_set(x.allow_set().begin(), x.allow_set().end());
    top().action_names = utilities::detail::set_intersection(top().action_names, allow_set);
  }

  void leave(const process::sync& /* x */)
  {
    join();
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
    join();
  }

  void leave(const process::left_merge& /* x */)
  {
    join();
  }

  void leave(const process::choice& /* x */)
  {
    join();
  }
};

struct apply_action_names_traverser : public action_names_traverser<apply_action_names_traverser>
{
  using super = action_names_traverser<apply_action_names_traverser>;
  using super::enter;
  using super::leave;
  using super::apply;
  using super::node_stack;

  apply_action_names_traverser(const std::vector<process_equation>& equations, std::set<process_identifier>& W)
    : super(equations, W)
  {}
};

inline
action_names_node action_names(const process_expression& x, const std::vector<process_equation>& equations, std::set<process_identifier>& W)
{
  detail::apply_action_names_traverser f(equations, W);
  f.apply(x);
  return f.node_stack.back();
}

} // namespace detail

inline
action_name_set action_names(const process_expression& x, const std::vector<process_equation>& equations)
{
  std::set<process_identifier> W;
  return detail::action_names(x, equations, W).action_names;
}

} // namespace mcrl2::process

#endif // MCRL2_PROCESS_ALPHABET_H
