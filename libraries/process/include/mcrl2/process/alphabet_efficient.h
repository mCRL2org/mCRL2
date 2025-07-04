// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/alphabet_efficient.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_ALPHABET_EFFICIENT_H
#define MCRL2_PROCESS_ALPHABET_EFFICIENT_H

#include "mcrl2/process/alphabet.h"

namespace mcrl2::process
{

namespace detail {

/// \brief Traverser that computes the alphabet of process expressions. This version should be more efficient than the default one.
template <typename Derived, typename Node = alphabet_node>
struct alphabet_efficient_traverser: public alphabet_traverser<Derived>
{
  using super = alphabet_traverser<Derived>;
  using super::enter;
  using super::leave;
  using super::apply;
  using super::W;
  using super::push;

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  alphabet_efficient_traverser(const std::vector<process_equation>& equations, std::set<process_identifier>& W)
    : super(equations, W)
  {}

  // N.B. In this version no elements from W are removed. This should be fine except for parallel expressions.
  void leave(const process::process_instance& x)
  {
    using utilities::detail::contains;
    if (!contains(W, x.identifier()))
    {
      W.insert(x.identifier());
      const process_equation& eqn = find_equation(super::equations, x.identifier());
      derived().apply(eqn.expression());
      // W.erase(x.identifier());
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
      const process_equation& eqn = find_equation(super::equations, x.identifier());
      derived().apply(eqn.expression());
      // W.erase(x.identifier());
    }
    else
    {
      push(multi_action_name_set());
    }
  }

  // For parallel expressions, the value of W is reset after finishing the left operand.
  template <typename T>
  void apply_parallel_operator(const T& x, const process_expression& left, const process_expression& right)
  {
    derived().enter(x);
    auto Wcopy = W;
    derived().apply(left);
    W = Wcopy;
    derived().apply(right);
    derived().leave(x);
  }

  void apply(const process::merge& x)
  {
    apply_parallel_operator(x, x.left(), x.right());
  }

  void apply(const process::left_merge& x)
  {
    apply_parallel_operator(x, x.left(), x.right());
  }

  void apply(const process::sync& x)
  {
    apply_parallel_operator(x, x.left(), x.right());
  }
};

struct apply_alphabet_efficient_traverser: public alphabet_efficient_traverser<apply_alphabet_efficient_traverser>
{
  using super = alphabet_efficient_traverser<apply_alphabet_efficient_traverser>;
  using super::enter;
  using super::leave;
  using super::apply;
  using super::node_stack;

  apply_alphabet_efficient_traverser(const std::vector<process_equation>& equations, std::set<process_identifier>& W)
    : super(equations, W)
  {}
};

inline
alphabet_node alphabet_efficient(const process_expression& x, const std::vector<process_equation>& equations, std::set<process_identifier>& W)
{
  detail::apply_alphabet_efficient_traverser f(equations, W);
  f.apply(x);
  return f.node_stack.back();
}

} // namespace detail

inline
multi_action_name_set alphabet_efficient(const process_expression& x, const std::vector<process_equation>& equations)
{
  std::set<process_identifier> W;
  return detail::alphabet_efficient(x, equations, W).alphabet;
}

} // namespace mcrl2::process

#endif // MCRL2_PROCESS_ALPHABET_EFFICIENT_H
