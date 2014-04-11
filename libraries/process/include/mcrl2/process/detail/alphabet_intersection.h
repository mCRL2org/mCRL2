// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
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

namespace mcrl2 {

namespace process {

namespace detail {

template <typename Derived, typename Node = alphabet_node>
struct alphabet_intersection_traverser: public alphabet_traverser<Derived, Node>
{
  typedef alphabet_traverser<Derived, Node> super;
  using super::enter;
  using super::leave;
  using super::operator();
  using super::top;

#if BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif

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
      bool remove = !includes(A, *i);
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
  typedef alphabet_intersection_traverser<apply_alphabet_intersection_traverser> super;
  using super::enter;
  using super::leave;
  using super::operator();
  using super::node_stack;

#if BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif

  apply_alphabet_intersection_traverser(const std::vector<process_equation>& equations, std::set<process_identifier>& W, const multi_action_name_set& A)
    : super(equations, W, A)
  {}
};

inline
alphabet_node alphabet_intersection(const process_expression& x, const std::vector<process_equation>& equations, std::set<process_identifier>& W, const multi_action_name_set& A)
{
  apply_alphabet_intersection_traverser f(equations, W, A);
  f(x);
  return f.node_stack.back();
}

inline
multi_action_name_set alphabet_intersection(const process_expression& x, const std::vector<process_equation>& equations, const multi_action_name_set& A)
{
  std::set<process_identifier> W;
  return detail::alphabet_intersection(x, equations, W, A).alphabet;
}
} // namespace detail

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_DETAIL_ALPHABET_INTERSECTION_H
