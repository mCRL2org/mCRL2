// Author(s): Jan Friso Groote. Based on lps2pbes_sat.h by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pres/detail/lps2pres_sat.h
/// \brief add your file description here.

#ifndef MCRL2_PRES_DETAIL_LPS2PRES_SAT_H
#define MCRL2_PRES_DETAIL_LPS2PRES_SAT_H

#include "mcrl2/data/detail/data_utility.h"
#include "mcrl2/data/detail/find.h"
#include "mcrl2/modal_formula/replace.h"
#include "mcrl2/pres/detail/lps2pres_utility.h"





namespace mcrl2::pres_system::detail {

template <typename TermTraits>
typename TermTraits::term_type Sat(const lps::multi_action& a,
                                   const action_formulas::action_formula& x,
                                   data::set_identifier_generator& id_generator,
                                   TermTraits tr
                                  );

template <typename Derived, typename TermTraits>
struct sat_traverser: public action_formulas::action_formula_traverser<Derived>
{
  using super = action_formulas::action_formula_traverser<Derived>;
  using tr = TermTraits;
  using expression_type = typename tr::term_type;

  using super::enter;
  using super::leave;
  using super::apply;

  const lps::multi_action& a;
  data::set_identifier_generator& id_generator;
  std::vector<expression_type> result_stack;

  sat_traverser(const lps::multi_action& a_, data::set_identifier_generator& id_generator_, TermTraits)
    : a(a_), id_generator(id_generator_)
  {}

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  void push(const expression_type& x)
  {
    result_stack.push_back(x);
  }

  const expression_type& top() const
  {
    return result_stack.back();
  }

  expression_type pop()
  {
    expression_type result = top();
    result_stack.pop_back();
    return result;
  }

  void leave(const data::data_expression& x)
  {
    push(x);
  }

  void leave(const action_formulas::multi_action& x)
  {
    push(lps::equal_multi_actions(a, lps::multi_action(x.actions())));
  }

  void leave(const action_formulas::true_&)
  {
    push(true_());
  }

  void leave(const action_formulas::false_&)
  {
    push(false_());
  }

  void apply(const action_formulas::not_& x)
  {
    push(minus(Sat(a, x.operand(), id_generator, TermTraits())));
  }

  void leave(const action_formulas::and_&)
  {
    expression_type right = pop();
    expression_type left = pop();
    push(tr::and_(left, right));
  }

  void leave(const action_formulas::or_&)
  {
    expression_type right = pop();
    expression_type left = pop();
    push(tr::or_(left, right));
  }

  void leave(const action_formulas::imp&)
  {
    expression_type right = pop();
    expression_type left = pop();
    push(tr::imp(left, right));
  }

  void apply(const action_formulas::forall& x)
  {
    data::mutable_map_substitution<> sigma_x = pres_system::detail::make_fresh_variable_substitution(x.variables(), id_generator, false);
    const action_formulas::action_formula& alpha = x.body();
    data::variable_list y = data::replace_variables(x.variables(), sigma_x);
    
    pres_expression result;
    make_infimum(result, y, Sat(a, action_formulas::replace_variables_capture_avoiding(alpha, sigma_x), id_generator, TermTraits()));
    push(result);
  }

  void apply(const action_formulas::exists& x)
  {
    data::mutable_map_substitution<> sigma_x = pres_system::detail::make_fresh_variable_substitution(x.variables(), id_generator, false);
    const action_formulas::action_formula& alpha = x.body();
    data::variable_list y = data::replace_variables(x.variables(), sigma_x);

    pres_expression result;
    make_supremum(result, y, Sat(a, action_formulas::replace_variables_capture_avoiding(alpha, sigma_x), id_generator, TermTraits()));
    push(result);
  }

  void apply(const action_formulas::at& x)
  {
    data::data_expression t = a.time();
    const action_formulas::action_formula& alpha = x.operand();
    const data::data_expression& u = x.time_stamp();
    push(tr::and_(Sat(a, alpha, id_generator, TermTraits()), data::equal_to(t, u)));
  }
};

template <template <class, class> class Traverser, typename TermTraits>
struct apply_sat_traverser: public Traverser<apply_sat_traverser<Traverser, TermTraits>, TermTraits>
{
  using super = Traverser<apply_sat_traverser<Traverser, TermTraits>, TermTraits>;
  using super::enter;
  using super::leave;
  using super::apply;
  using super::top;

  apply_sat_traverser(const lps::multi_action& a, data::set_identifier_generator& id_generator, TermTraits tr)
    : super(a, id_generator, tr)
  {}
};

template <typename TermTraits>
typename TermTraits::term_type Sat(const lps::multi_action& a,
                                   const action_formulas::action_formula& x,
                                   data::set_identifier_generator& id_generator,
                                   TermTraits tr
                                  )
{
  apply_sat_traverser<sat_traverser, TermTraits> f(a, id_generator, tr);
  f.apply(x);
  return f.top();
}

} // namespace mcrl2::pres_system::detail





#endif // MCRL2_PRES_DETAIL_LPS2PRES_SAT_H
