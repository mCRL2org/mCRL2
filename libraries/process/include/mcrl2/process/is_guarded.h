// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
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

namespace mcrl2::process
{

namespace detail {

bool is_guarded(const process_expression& x, const std::vector<process_equation>& equations, std::set<process_identifier>& W);

struct is_guarded_traverser: public process_expression_traverser<is_guarded_traverser>
{
  using super = process_expression_traverser<is_guarded_traverser>;
  using super::enter;
  using super::leave;
  using super::apply;

  const std::vector<process_equation>& equations;
  std::set<process_identifier>& W;
  bool result = true;

  is_guarded_traverser(const std::vector<process_equation>& equations_, std::set<process_identifier>& W_)
    : equations(equations_), W(W_)
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
  void apply(const process::seq& x)
  {
    apply(x.left()); // only p needs to be guarded
  }

  // p << q
  void operator()(const process::bounded_init& x)
  {
    apply(x.left()); // only p needs to be guarded
  }
};

inline
bool is_guarded(const process_expression& x, const std::vector<process_equation>& equations, std::set<process_identifier>& W)
{
  detail::is_guarded_traverser f(equations, W);
  f.apply(x);
  return f.result;
}

} // namespace detail

/// \brief Checks if a process expression is guarded.
inline
bool is_guarded(const process_expression& x, const std::vector<process_equation>& equations)
{
  std::set<process_identifier> W;
  return detail::is_guarded(x, equations, W);
}

} // namespace mcrl2::process

#endif // MCRL2_PROCESS_IS_GUARDED_H
