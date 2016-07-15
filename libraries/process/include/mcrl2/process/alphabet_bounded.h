// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/alphabet_bounded.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_ALPHABET_BOUNDED_H
#define MCRL2_PROCESS_ALPHABET_BOUNDED_H

#include "mcrl2/process/alphabet_efficient.h"
#include "mcrl2/process/allow_set.h"

namespace mcrl2 {

namespace process {

namespace detail {

multi_action_name_set alphabet_bounded(const process_expression& x,
                                       const allow_set& A,
                                       const std::vector<process_equation>& equations,
                                       const std::map<process_identifier, multi_action_name_set>& cache
                                      );

/// \brief Traverser that computes the alphabet of process expressions
struct alphabet_bounded_traverser: public process_expression_traverser<alphabet_bounded_traverser>
{
  typedef process_expression_traverser<alphabet_bounded_traverser> super;
  using super::enter;
  using super::leave;
  using super::apply;

  const allow_set& A;
  const std::vector<process_equation>& equations;
  const std::map<process_identifier, multi_action_name_set>& cache;
  std::vector<allow_set> node_stack;

  alphabet_bounded_traverser(const allow_set& A_,
                             const std::vector<process_equation>& equations_,
                             const std::map<process_identifier, multi_action_name_set>& variable_cache_
                            )
    : A(A_), equations(equations_), cache(variable_cache_)
  {}

  // Push A to node_stack
  void push(const multi_action_name_set& A1)
  {
    push(allow_set(A1));
  }

  // Push A to node_stack
  void push(const allow_set& A1)
  {
    node_stack.push_back(A1);
  }

  // Pop the top element of node_stack and return it
  allow_set pop()
  {
    allow_set result = node_stack.back();
    node_stack.pop_back();
    return result;
  }

  // Return the top element of node_stack
  allow_set& top()
  {
    return node_stack.back();
  }

  // Return the top element of node_stack
  const allow_set& top() const
  {
    return node_stack.back();
  }

  // Pops two elements A1 and A2 from the stack, and pushes back union(A1, A2)
  void join()
  {
    auto right = pop();
    auto left = pop();
    push(alphabet_operations::set_union(left.A, right.A));
  }

  void leave(const process::action& x)
  {
    multi_action_name alpha;
    alpha.insert(x.label().name());
    multi_action_name_set A;
    A.insert(alpha);
    push(A);
  }

  void apply(const process::process_instance& x)
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

  void apply(const process::process_instance_assignment& x)
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

  void apply(const process::block& x)
  {
    const core::identifier_string_list& B = x.block_set();
    allow_set A1 = alphabet_operations::block(B, A);
    push(A1);
  }

  void apply(const process::hide& x)
  {
    const core::identifier_string_list& I = x.hide_set();
    allow_set A1 = alphabet_operations::hide_inverse(I, A);
    multi_action_name_set A2 = alphabet_bounded(x.operand(), A1, equations, cache);
    A2 = alphabet_operations::hide(I, A2);
    push(A2);
  }

  void apply(const process::rename& x)
  {
    const rename_expression_list& R = x.rename_set();
    allow_set A1 = alphabet_operations::rename_inverse(R, A);
    multi_action_name_set A2 = alphabet_bounded(x.operand(), A1, equations, cache);
    A2 = alphabet_operations::rename(R, A2);
    push(A2);
  }

  void apply(const process::comm& x)
  {
    const communication_expression_list& C = x.comm_set();
    allow_set A1 = alphabet_operations::comm_inverse(C, A);
    multi_action_name_set A2 = alphabet_bounded(x.operand(), A1, equations, cache);
    communication_expression_list C1 = alphabet_operations::filter_comm_set(x.comm_set(), A2);
    A2 = alphabet_operations::comm(C1, A2);
    push(A2);
  }

  void apply(const process::allow& x)
  {
    const action_name_multiset_list& V = x.allow_set();
    allow_set A1 = alphabet_operations::allow(V, A);
    multi_action_name_set A2 = alphabet_bounded(x.operand(), A1, equations, cache);
    push(A2);
  }

  void apply(const process::merge& x)
  {
    allow_set A_sub = alphabet_operations::subsets(A);
    multi_action_name_set Aleft = alphabet_bounded(x.left(), A_sub, equations, cache);
    allow_set A_arrow = alphabet_operations::left_arrow(A, Aleft);
    multi_action_name_set Aright = alphabet_bounded(x.right(), A_arrow, equations, cache);
    push(alphabet_operations::bounded_merge(Aleft, Aright, A).first);
  }

  void apply(const process::left_merge& x)
  {
    allow_set A_sub = alphabet_operations::subsets(A);
    multi_action_name_set Aleft = alphabet_bounded(x.left(), A_sub, equations, cache);
    allow_set A_arrow = alphabet_operations::left_arrow(A, Aleft);
    multi_action_name_set Aright = alphabet_bounded(x.right(), A_arrow, equations, cache);
    push(alphabet_operations::bounded_left_merge(Aleft, Aright, A).first);
  }

  void apply(const process::sync& x)
  {
    allow_set A_sub = alphabet_operations::subsets(A);
    multi_action_name_set Aleft = alphabet_bounded(x.left(), A_sub, equations, cache);
    allow_set A_arrow = alphabet_operations::left_arrow(A, Aleft);
    multi_action_name_set Aright = alphabet_bounded(x.right(), A_arrow, equations, cache);
    push(alphabet_operations::bounded_sync(Aleft, Aright, A).first);
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

  void leave(const process::choice& /* x */)
  {
    join();
  }
};

inline
multi_action_name_set alphabet_bounded(const process_expression& x,
                                       const allow_set& A,
                                       const std::vector<process_equation>& equations,
                                       const std::map<process_identifier, multi_action_name_set>& cache
                                      )
{
  alphabet_bounded_traverser f(A, equations, cache);
  f.apply(x);
  return f.node_stack.back().A;
}

} // namespace detail

inline
multi_action_name_set alphabet_bounded(const process_expression& x, const multi_action_name_set& A, const std::vector<process_equation>& equations)
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
    mCRL2log(log::verbose) << i.first << " -> " << i.second << std::endl;
  }

  return detail::alphabet_bounded(x, allow_set(A), equations, cache);
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_ALPHABET_BOUNDED_H
