// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/lps2pbes_par.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_LPS2PBES_PAR_H
#define MCRL2_PBES_DETAIL_LPS2PBES_PAR_H

#include "mcrl2/modal_formula/traverser.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

data::variable_list Par(const core::identifier_string& X, const data::variable_list& l, const state_formulas::state_formula& x);

struct par_traverser: public state_formulas::state_formula_traverser<par_traverser>
{
  typedef state_formulas::state_formula_traverser<par_traverser> super;
  using super::enter;
  using super::leave;
  using super::operator();

#if BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif

  const core::identifier_string& X;
  const data::variable_list& l;
  atermpp::vector<data::variable_list> result_stack;

  par_traverser(const core::identifier_string& X_, const data::variable_list& l_)
    : X(X_), l(l_)
  {}

  void push(const data::variable_list& x)
  {
    result_stack.push_back(x);
  }

  const data::variable_list& top() const
  {
    return result_stack.back();
  }

  data::variable_list pop()
  {
    data::variable_list result = top();
    result_stack.pop_back();
    return result;
  }

  // join the two topmost elements on the stack
  void join()
  {
    data::variable_list right = pop();
    data::variable_list left = pop();
    push(left + right);
  }

  void leave(const data::data_expression& x)
  {
    push(data::variable_list());
  }

  void leave(const state_formulas::true_& x)
  {
    push(data::variable_list());
  }

  void leave(const state_formulas::false_& x)
  {
    push(data::variable_list());
  }

  void leave(const state_formulas::not_& x)
  {
    // skip
  }

  void leave(const state_formulas::and_& x)
  {
    join();
  }

  void leave(const state_formulas::or_& x)
  {
    join();
  }

  void leave(const state_formulas::imp& x)
  {
    join();
  }

  void operator()(const state_formulas::forall& x)
  {
    push(Par(X, l + x.variables(), x.body()));
  }

  void operator()(const state_formulas::exists& x)
  {
    push(Par(X, l + x.variables(), x.body()));
  }

  void leave(const state_formulas::must& x)
  {
    // skip
  }

  void leave(const state_formulas::may& x)
  {
    // skip
  }

  void leave(const state_formulas::yaled& x)
  {
    push(data::variable_list());
  }

  void leave(const state_formulas::yaled_timed& x)
  {
    push(data::variable_list());
  }

  void leave(const state_formulas::delay& x)
  {
    push(data::variable_list());
  }

  void leave(const state_formulas::delay_timed& x)
  {
    push(data::variable_list());
  }

  void leave(const state_formulas::variable& x)
  {
    push(data::variable_list());
  }

  void operator()(const state_formulas::nu& x)
  {
    if (x.name() == X)
    {
      push(l);
    }
    else
    {
      push(Par(X, l + data::left_hand_sides(x.assignments()), x.operand()));
    }
  }

  void operator()(const state_formulas::mu& x)
  {
    if (x.name() == X)
    {
      push(l);
    }
    else
    {
      push(Par(X, l + data::left_hand_sides(x.assignments()), x.operand()));
    }
  }
};

inline
data::variable_list Par(const core::identifier_string& X, const data::variable_list& l, const state_formulas::state_formula& x)
{
  par_traverser f(X, l);
  f(x);
  return f.top();
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_LPS2PBES_PAR_H
