// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/alphabet_new.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_ALPHABET_NEW_H
#define MCRL2_PROCESS_ALPHABET_NEW_H

#include "mcrl2/process/alphabet_efficient.h"

namespace mcrl2 {

namespace process {

namespace detail {

/// \brief Traverser that computes the alphabet of process expressions
struct alphabet_new_traverser: public process_expression_traverser<alphabet_new_traverser>
{
  typedef process_expression_traverser<alphabet_new_traverser> super;
  using super::enter;
  using super::leave;
  using super::apply;

  const std::vector<process_equation>& equations;
  const std::map<process_identifier, multi_action_name_set>& cache;
  std::vector<multi_action_name_set> node_stack;

  alphabet_new_traverser(const std::vector<process_equation>& equations_, const std::map<process_identifier, multi_action_name_set>& variable_cache_)
    : equations(equations_), cache(variable_cache_)
  {}

  // Push A to node_stack
  void push(const multi_action_name_set& A)
  {
    node_stack.push_back(A);
  }

  // Pop the top element of node_stack and return it
  multi_action_name_set pop()
  {
    multi_action_name_set result = node_stack.back();
    node_stack.pop_back();
    return result;
  }

  // Return the top element of node_stack
  multi_action_name_set& top()
  {
    return node_stack.back();
  }

  // Return the top element of node_stack
  const multi_action_name_set& top() const
  {
    return node_stack.back();
  }

  // Pops two elements A1 and A2 from the stack, and pushes back union(A1, A2)
  void join()
  {
    auto right = pop();
    auto left = pop();
    push(alphabet_operations::set_union(left, right));
  }

  // Pops two elements A1 and A2 from the stack, and pushes back union(A1, A2, A1 | A2)
  void join_merge()
  {
    auto right = pop();
    auto left = pop();
    push(alphabet_operations::merge(left, right));
  }

  // Pops two elements A1 and A2 from the stack, and pushes back A1 | A2
  void join_sync()
  {
    auto right = pop();
    auto left = pop();
    push(alphabet_operations::sync(left, right));
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
    auto i = cache.find(x.identifier());
    if (i != cache.end())
    {
      push(i->second);
    }
    else
    {
      const process_equation& eqn = find_equation(equations, x.identifier());
      apply(eqn.expression());
    }
  }

  void leave(const process::process_instance_assignment& x)
  {
    auto i = cache.find(x.identifier());
    if (i != cache.end())
    {
      push(i->second);
    }
    else
    {
      const process_equation& eqn = find_equation(equations, x.identifier());
      apply(eqn.expression());
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
    top() = alphabet_operations::block(x.block_set(), top());
  }

  void leave(const process::hide& x)
  {
    top() = alphabet_operations::hide(x.hide_set(), top());
  }

  void leave(const process::rename& x)
  {
    top() = alphabet_operations::rename(x.rename_set(), top());
  }

  void leave(const process::comm& x)
  {
    top() = alphabet_operations::comm(x.comm_set(), top());
  }

  void leave(const process::allow& x)
  {
    top() = alphabet_operations::allow(x.allow_set(), top());
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

} // namespace detail

inline
multi_action_name_set alphabet_new(const process_expression& x, const std::vector<process_equation>& equations)
{
  // compute variable cache
  // TODO: This can be implemented more efficiently, using an SCC graph
  std::map<process_identifier, multi_action_name_set> cache;
  for (const process_equation& eqn: equations)
  {
    if (is_pcrl(eqn.expression()))
    {
      cache[eqn.identifier()] = alphabet_efficient(eqn.expression(), equations);
    }
  }

  // print cache
  mCRL2log(log::verbose) << "--- computed variable cache ---" << std::endl;
  for (const auto& i: cache)
  {
    mCRL2log(log::verbose) << i.first << " -> " << process::pp(i.second) << std::endl;
  }

  detail::alphabet_new_traverser f(equations, cache);
  f.apply(x);
  return f.node_stack.back();
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_ALPHABET_NEW_H
