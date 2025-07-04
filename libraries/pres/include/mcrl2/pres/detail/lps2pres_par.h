// Author(s): Jan Friso Groote. Based on lps2pbes_par.h by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pres/detail/lps2pres_par.h
/// \brief add your file description here.

#ifndef MCRL2_PRES_DETAIL_LPS2PRES_PAR_H
#define MCRL2_PRES_DETAIL_LPS2PRES_PAR_H

#include "mcrl2/modal_formula/traverser.h"





namespace mcrl2::pres_system::detail {

data::variable_list Par(const core::identifier_string& X, const data::variable_list& l, const state_formulas::state_formula& x);

struct par_traverser: public state_formulas::state_formula_traverser<par_traverser>
{
  using super = state_formulas::state_formula_traverser<par_traverser>;
  using super::enter;
  using super::leave;
  using super::apply;

  const core::identifier_string& X;
  const data::variable_list& l;
  std::vector<data::variable_list> result_stack;

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

  void leave(const data::data_expression&)
  {
    push(data::variable_list());
  }

  void leave(const state_formulas::true_&)
  {
    push(data::variable_list());
  }

  void leave(const state_formulas::false_&)
  {
    push(data::variable_list());
  }

  void leave(const state_formulas::not_&)
  {
    // skip
  }

  void leave(const state_formulas::and_&)
  {
    join();
  }

  void leave(const state_formulas::or_&)
  {
    join();
  }

  void leave(const state_formulas::plus&)
  {
    join();
  }

  void leave(const state_formulas::imp&)
  {
    join();
  }

  void leave(const state_formulas::const_multiply&)
  {
    // skip
  }

  void leave(const state_formulas::const_multiply_alt&)
  {
    // skip
  }

  void apply(const state_formulas::forall& x)
  {
    push(Par(X, l + x.variables(), x.body()));
  }

  void apply(const state_formulas::exists& x)
  {
    push(Par(X, l + x.variables(), x.body()));
  }

  void apply(const state_formulas::infimum& x)
  {
    push(Par(X, l + x.variables(), x.body()));
  }

  void apply(const state_formulas::supremum& x)
  {
    push(Par(X, l + x.variables(), x.body()));
  }

  void apply(const state_formulas::sum& x)
  {
    push(Par(X, l + x.variables(), x.body()));
  }

  void leave(const state_formulas::must&)
  {
    // skip
  }

  void leave(const state_formulas::may&)
  {
    // skip
  }

  void leave(const state_formulas::yaled&)
  {
    push(data::variable_list());
  }

  void leave(const state_formulas::yaled_timed&)
  {
    push(data::variable_list());
  }

  void leave(const state_formulas::delay&)
  {
    push(data::variable_list());
  }

  void leave(const state_formulas::delay_timed&)
  {
    push(data::variable_list());
  }

  void leave(const state_formulas::variable&)
  {
    push(data::variable_list());
  }

  void apply(const state_formulas::nu& x)
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

  void apply(const state_formulas::mu& x)
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
  f.apply(x);
  assert(f.result_stack.size()==1);
  return f.top();
}

} // namespace mcrl2::pres_system::detail





#endif // MCRL2_PRES_DETAIL_LPS2PRES_PAR_H
