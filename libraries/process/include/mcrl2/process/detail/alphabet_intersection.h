// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/detail/alphabet_intersection.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_DETAIL_ALPHABET_INTERSECTION_H
#define MCRL2_PROCESS_DETAIL_ALPHABET_INTERSECTION_H

#include "mcrl2/process/alphabet.h"

namespace mcrl2::process::detail
{

template <typename Derived, typename Node = alphabet_node>
struct alphabet_intersection_traverser: public alphabet_traverser<Derived, Node>
{
  using super = alphabet_traverser<Derived, Node>;
  using super::enter;
  using super::leave;
  using super::apply;
  using super::top;

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  const multi_action_name_set& A;

  alphabet_intersection_traverser(const std::vector<process_equation>& equations, std::set<process_identifier>& W, const multi_action_name_set& A_)
    : super(equations, W), A(A_)
  {}

  void filter()
  {
    Node& node = top();
    for (multi_action_name_set::iterator i = node.alphabet.begin(); i != node.alphabet.end(); )
    {
      bool remove = !alphabet_operations::includes(A, *i);
      if (remove)
      {
        node.alphabet.erase(i++);
      }
      else
      {
        ++i;
      }
    }
  }

  void leave(const process::action& x)
  {
    super::leave(x);
    filter();
  }

  void leave(const process::block& x)
  {
    throw mcrl2::runtime_error("alphabet_intersection is undefined for expression " + process::pp(x));
  }

  void leave(const process::hide& x)
  {
    throw mcrl2::runtime_error("alphabet_intersection is undefined for expression " + process::pp(x));
  }

  void leave(const process::rename& x)
  {
    throw mcrl2::runtime_error("alphabet_intersection is undefined for expression " + process::pp(x));
  }

  void leave(const process::comm& x)
  {
    throw mcrl2::runtime_error("alphabet_intersection is undefined for expression " + process::pp(x));
  }

  void leave(const process::allow& x)
  {
    throw mcrl2::runtime_error("alphabet_intersection is undefined for expression " + process::pp(x));
  }

  void leave(const process::sync& x)
  {
    super::leave(x);
    filter();
  }

  void leave(const process::merge& x)
  {
    super::leave(x);
    filter();
  }

  void leave(const process::left_merge& x)
  {
    super::leave(x);
    filter();
  }
};

struct apply_alphabet_intersection_traverser: public alphabet_intersection_traverser<apply_alphabet_intersection_traverser>
{
  using super = alphabet_intersection_traverser<apply_alphabet_intersection_traverser>;
  using super::enter;
  using super::leave;
  using super::apply;
  using super::node_stack;

  apply_alphabet_intersection_traverser(const std::vector<process_equation>& equations, std::set<process_identifier>& W, const multi_action_name_set& A)
    : super(equations, W, A)
  {}
};

inline
alphabet_node alphabet_intersection(const process_expression& x, const std::vector<process_equation>& equations, std::set<process_identifier>& W, const multi_action_name_set& A)
{
  apply_alphabet_intersection_traverser f(equations, W, A);
  f.apply(x);
  return f.node_stack.back();
}

inline
multi_action_name_set alphabet_intersection(const process_expression& x, const std::vector<process_equation>& equations, const multi_action_name_set& A)
{
  std::set<process_identifier> W;
  return detail::alphabet_intersection(x, equations, W, A).alphabet;
}
} // namespace mcrl2::process::detail

#endif // MCRL2_PROCESS_DETAIL_ALPHABET_INTERSECTION_H
