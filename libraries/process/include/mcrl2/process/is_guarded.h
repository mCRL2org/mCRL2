// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/is_guarded.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_IS_GUARDED_H
#define MCRL2_PROCESS_IS_GUARDED_H

#include "mcrl2/process/find.h"
#include "mcrl2/process/traverser.h"
#include "mcrl2/utilities/detail/container_utility.h"

namespace mcrl2 {

namespace process {

namespace detail {

bool is_guarded(const process_expression& x, const std::vector<process_equation>& equations, std::set<process_identifier>& W);

struct is_guarded_traverser: public process_expression_traverser<is_guarded_traverser>
{
  typedef process_expression_traverser<is_guarded_traverser> super;
  using super::enter;
  using super::leave;
  using super::operator();

#if BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif

  const std::vector<process_equation>& equations;
  std::set<process_identifier>& W;
  bool result;

  is_guarded_traverser(const std::vector<process_equation>& equations_, std::set<process_identifier>& W_)
    : equations(equations_), W(W_), result(true)
  {}

  // P(e1, ..., en)
  void enter(const process::process_instance& x)
  {
    using utilities::detail::contains;
    if (!contains(W, x.identifier()))
    {
      W.insert(x.identifier());
      const process_equation& eqn = find_equation(equations, x.identifier());
      result = result && is_guarded(eqn.expression(), equations, W);
    }
    else
    {
      result = false;
    }
  }

  // P(d1 = e1, ..., dn = en)
  void enter(const process::process_instance_assignment& x)
  {
    using utilities::detail::contains;
    if (!contains(W, x.identifier()))
    {
      W.insert(x.identifier());
      const process_equation& eqn = find_equation(equations, x.identifier());
      result = result && is_guarded(eqn.expression(), equations, W);
    }
    else
    {
      result = false;
    }
  }

  // p . q
  void operator()(const process::seq& x)
  {
    (*this)(x.left()); // only p needs to be guarded
  }

  // p << q
  void operator()(const process::bounded_init& x)
  {
    (*this)(x.left()); // only p needs to be guarded
  }
};

inline
bool is_guarded(const process_expression& x, const std::vector<process_equation>& equations, std::set<process_identifier>& W)
{
  detail::is_guarded_traverser f(equations, W);
  f(x);
  return f.result;
}

} // detail

inline
bool is_guarded(const process_expression& x, const std::vector<process_equation>& equations)
{
  std::set<process_identifier> W;
  return detail::is_guarded(x, equations, W);
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_IS_GUARDED_H
