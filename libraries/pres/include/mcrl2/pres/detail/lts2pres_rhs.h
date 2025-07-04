// Author(s): Jan Friso Groote. Based on lts2pres_rhs.h by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pres/detail/lts2pres_rhs.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_LTS2PBES_RHS_H
#define MCRL2_PBES_DETAIL_LTS2PBES_RHS_H

#include "mcrl2/pres/replace.h"
#include "mcrl2/pres/detail/lps2pres_par.h"
#include "mcrl2/pres/detail/lps2pres_sat.h"
#include "mcrl2/pres/detail/lts2pres_lts.h"
#include "mcrl2/utilities/progress_meter.h"

namespace mcrl2::pres_system::detail
{

using lts2pres_state_type = lts::probabilistic_lts_lts_t::states_size_type;

inline
core::identifier_string make_identifier(const core::identifier_string& name, lts2pres_state_type s)
{
  return core::identifier_string(std::string(name) + "'" + std::to_string(s));
}

struct lts2pres_parameters
{
  const state_formulas::state_formula& phi0; // the original formula
  const lts::probabilistic_lts_lts_t& lts0;
  const lts2pres_lts& lts1;
  data::set_identifier_generator& id_generator;
  utilities::progress_meter& pm;

  lts2pres_parameters(const state_formulas::state_formula& phi0_,
                      const lts::probabilistic_lts_lts_t& lts0_,
                      const lts2pres_lts& lts1_,
                      data::set_identifier_generator& id_generator_,
                      utilities::progress_meter& pm_
                     )
    : phi0(phi0_), lts0(lts0_), lts1(lts1_), id_generator(id_generator_), pm(pm_)
  {}

  template <typename TermTraits>
  pres_expression rhs_may_must(bool is_must,
                               const pres_expression& left,  // Sat(a(x), alpha)
                               const pres_expression& right, // RHS(phi, t)
                               std::size_t, /* i */
                               const lps::multi_action& /* a */,
                               lts2pres_state_type /* s */,
                               lts2pres_state_type /* t */,
                               TermTraits
                              )
  {
    using tr = TermTraits;
    if (is_must)
    {
      return tr::imp(left, right);
    }
    else
    {
      return tr::and_(left, right);
    }
  }
}; 

template <typename TermTraits, typename Parameters>
pres_expression RHS(const state_formulas::state_formula& x,
                    lts2pres_state_type s,
                    Parameters& parameters,
                    TermTraits tr);

template <typename Derived, typename TermTraits, typename Parameters>
struct rhs_lts2pres_traverser: public state_formulas::state_formula_traverser<Derived>
{
  using super = state_formulas::state_formula_traverser<Derived>;
  using tr = TermTraits;

  using super::enter;
  using super::leave;
  using super::apply;

  lts2pres_state_type s;
  Parameters& parameters;
  std::vector<pres_expression> result_stack;

  rhs_lts2pres_traverser(lts2pres_state_type s_,
                         Parameters& parameters_,
                         TermTraits
                        )
    : s(s_), parameters(parameters_)
  {}

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  void push(const pres_expression& x)
  {
    result_stack.push_back(x);
  }

  pres_expression& top()
  {
    return result_stack.back();
  }

  const pres_expression& top() const
  {
    return result_stack.back();
  }

  pres_expression pop()
  {
    pres_expression result = top();
    result_stack.pop_back();
    return result;
  }

  void leave(const data::data_expression& x)
  {
    push(x);
  }

  void leave(const state_formulas::true_&)
  {
    push(true_());
  }

  void leave(const state_formulas::false_&)
  {
    push(false_());
  }

  void apply(const state_formulas::minus&)
  {
    throw mcrl2::runtime_error("rhs_lts2pres_traverser: minus is not supported!");
  }

  void leave(const state_formulas::and_&)
  {
    pres_expression right = pop();
    pres_expression left = pop();
    push(tr::and_(left, right));
  }

  void leave(const state_formulas::or_&)
  {
    pres_expression right = pop();
    pres_expression left = pop();
    push(tr::or_(left, right));
  }

  void leave(const state_formulas::plus&)
  {
    pres_expression right = pop();
    pres_expression left = pop();
    push(tr::plus(left, right));
  }

  void apply(const state_formulas::imp&)
  {
    throw mcrl2::runtime_error("rhs_lts2pres_traverser: implication is not supported!");
  }

  void apply(const state_formulas::forall& )
  {
    throw mcrl2::runtime_error("rhs_lts2pres_traverser: forall is not allowed!");
  }

  void apply(const state_formulas::exists& )
  {
    throw mcrl2::runtime_error("rhs_lts2pres_traverser: exists is not allowed!");
  }

  void apply(const state_formulas::infimum& x)
  {
    derived().apply(x.body());
    top() = infimum(x.variables(), top());
  }

  void apply(const state_formulas::supremum& x)
  {
    derived().apply(x.body());
    top() = supremum(x.variables(), top());
  }

  void apply(const state_formulas::sum& x)
  {
    derived().apply(x.body());
    top() = sum(x.variables(), top());
  }

  void apply(const state_formulas::must& x)
  {
    const auto& lts1 = parameters.lts1;
    std::vector<pres_expression> v;
    assert(action_formulas::is_action_formula(x.formula()));
    const auto& alpha = atermpp::down_cast<const action_formulas::action_formula>(x.formula());
    const state_formulas::state_formula& phi = x.operand();

    // traverse all transitions s --a--> t
    for (const lts2pres_lts::edge& e: lts1.edges(s))
    {
      lts2pres_state_type t = e.probabilistic_state; 
      const lps::multi_action& a = lts1.action_labels()[e.label];
      pres_expression left = detail::Sat(a, alpha, parameters.id_generator, TermTraits());
      pres_expression right;
      if (parameters.lts0.probabilistic_state(t).size()<=1)
      {
        right=RHS(phi, parameters.lts0.probabilistic_state(t).get(), parameters, TermTraits());
      }
      else
      {
        bool defined=false;
        for(const  lts::probabilistic_lts_lts_t::probabilistic_state_t::state_probability_pair& p: parameters.lts0.probabilistic_state(t))
        {
          if (!defined)
          {
            right = const_multiply(p.probability(), RHS(phi, p.state(), parameters, TermTraits()));
            defined=true;
          }
          else 
          {
            right = plus(right, const_multiply(p.probability(), RHS(phi, p.state(), parameters, TermTraits())));
          }
        }
      }

      pres_expression p = parameters.rhs_may_must(true, left, right, e.index, a, s, t, TermTraits());
      v.push_back(p);
    }
    push(tr::join_and(v.begin(), v.end()));
  }

  void apply(const state_formulas::may& x)
  {
    const auto& lts1 = parameters.lts1;
    std::vector<pres_expression> v;
    assert(action_formulas::is_action_formula(x.formula()));
    const auto& alpha = atermpp::down_cast<const action_formulas::action_formula>(x.formula());
    const state_formulas::state_formula& phi = x.operand();

    // traverse all transitions s --a--> t
    for (const lts2pres_lts::edge& e: lts1.edges(s))
    {
      lts2pres_state_type t = e.probabilistic_state; 
      const lps::multi_action& a = lts1.action_labels()[e.label];
      pres_expression left = detail::Sat(a, alpha, parameters.id_generator, TermTraits());
    
      pres_expression right;
      if (parameters.lts0.probabilistic_state(t).size()<=1)
      {
        right=RHS(phi, parameters.lts0.probabilistic_state(t).get(), parameters, TermTraits());
      }
      else
      {
        bool defined=false;
        for(const  lts::probabilistic_lts_lts_t::probabilistic_state_t::state_probability_pair& p: parameters.lts0.probabilistic_state(t))
        {
          if (!defined)
          {
            right = const_multiply(p.probability(), RHS(phi, p.state(), parameters, TermTraits()));
            defined=true;
          }
          else
          {
            right = plus(right, const_multiply(p.probability(), RHS(phi, p.state(), parameters, TermTraits())));
          }
        }
      }

      pres_expression p = parameters.rhs_may_must(false, left, right, e.index, a, s, t, TermTraits());
      v.push_back(p);
    }
    push(tr::join_or(v.begin(), v.end()));
  }

  void leave(const state_formulas::yaled&)
  {
    throw mcrl2::runtime_error("rhs_lts2pres_traverser: yaled is not supported!");
  }

  void leave(const state_formulas::yaled_timed&)
  {
    throw mcrl2::runtime_error("rhs_lts2pres_traverser: yaled_timed is not supported!");
  }

  void leave(const state_formulas::delay&)
  {
    throw mcrl2::runtime_error("rhs_lts2pres_traverser: yaled is not supported!");
  }

  void leave(const state_formulas::delay_timed&)
  {
    throw mcrl2::runtime_error("rhs_lts2pres_traverser: delay_timed is not supported!");
  }

  void leave(const state_formulas::variable& x)
  {
    const core::identifier_string& X = x.name();
    core::identifier_string X_s = make_identifier(X, s);
    const data::data_expression_list& e = x.arguments();
    push(propositional_variable_instantiation(X_s, e + detail::Par(X, data::variable_list(), parameters.phi0)));
  }

  void apply(const state_formulas::nu& x)
  {
    const core::identifier_string& X = x.name();
    core::identifier_string X_s = make_identifier(X, s);
    data::data_expression_list e = detail::mu_expressions(x);
    push(propositional_variable_instantiation(X_s, e + detail::Par(X, data::variable_list(), parameters.phi0)));
  }

  void apply(const state_formulas::mu& x)
  {
    const core::identifier_string& X = x.name();
    core::identifier_string X_s = make_identifier(X, s);
    data::data_expression_list e = detail::mu_expressions(x);
    push(propositional_variable_instantiation(X_s, e + detail::Par(X, data::variable_list(), parameters.phi0)));
  }
};

template <template <class, class, class> class Traverser, typename TermTraits, typename Parameters>
struct apply_rhs_lts2pres_traverser: public Traverser<apply_rhs_lts2pres_traverser<Traverser, TermTraits, Parameters>, TermTraits, Parameters>
{
  using super = Traverser<apply_rhs_lts2pres_traverser<Traverser, TermTraits, Parameters>, TermTraits, Parameters>;
  using super::enter;
  using super::leave;
  using super::apply;

  apply_rhs_lts2pres_traverser(lts2pres_state_type s, Parameters& parameters, TermTraits tr)
    : super(s, parameters, tr)
  {}
};

template <typename TermTraits, typename Parameters>
pres_expression RHS(const state_formulas::state_formula& x,
                    lts2pres_state_type s,
                    Parameters& parameters,
                    TermTraits tr)
{
  apply_rhs_lts2pres_traverser<rhs_lts2pres_traverser, TermTraits, Parameters> f(s, parameters, tr);
  f.apply(x);
  return f.top();
}

} // namespace mcrl2::pres_system::detail

#endif // MCRL2_PBES_DETAIL_LTS2PBES_RHS_H
