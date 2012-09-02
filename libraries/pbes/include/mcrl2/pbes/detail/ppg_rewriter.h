// Author(s): Gijs Kant
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/ppg_traverser.h
/// \brief Traverser class for Parameterised Parity Games (PPG), PBES expressions of the form:
/// PPG :== /\_{i: I} f_i && /\_{j: J} forall v: D_j . ( g_j => X_j(e_j) )
///       | \/_{i: I} f_i || \/_{j: J} exists v: D_j . ( g_j && X_j(e_j) ).
#ifndef MCRL2_PBES_DETAIL_PPG_REWRITER_H
#define MCRL2_PBES_DETAIL_PPG_REWRITER_H

#include <stack>
#include "mcrl2/atermpp/stack.h"
#include "mcrl2/pbes/traverser.h"
#include "mcrl2/pbes/pbes_functions.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

struct fresh_variable_name_generator
{
  /// \brief The set of variable names already in use.
  std::set<std::string> variable_names;
  /// \brief A map from variable name prefix to the last suffix that has been added.
  std::map<std::string,int> variable_name_suffix;

  template <typename Container>
  fresh_variable_name_generator(const Container& equations)
  {
    for (typename Container::const_iterator eqn = equations.begin(); eqn != equations.end(); ++eqn) {
      propositional_variable var = (*eqn).variable();
      variable_names.insert(std::string(var.name()));
    }
  }

  /// \brief Generates a fresh variable name, based on s (extending s).
  /// \param s A string.
  /// \return a fresh variable name.
  std::string generate_name(const std::string& s) {
    //std::clog << "fresh_variable_name: ";
    std::string base; std::string name;
    name = base = s;
    int suffix = 1;
    while (variable_names.find(name)!=variable_names.end()) {
      if (variable_name_suffix.find(base) != variable_name_suffix.end()) {
        suffix = variable_name_suffix[base] + 1;
      }
      variable_name_suffix[base] = suffix;
      std::stringstream ss;
      ss << base << "_" << suffix;
      ss >> name;
    }
    variable_names.insert(name);
    //std::clog << name << std::endl;
    return name;
  }
};

/// \cond INTERNAL_DOCS
/// \brief Visitor for checking if a pbes object is a PPG.
struct ppg_rewriter: public pbes_expression_traverser<ppg_rewriter>
{
  typedef pbes_expression_traverser<ppg_rewriter> super;
  using super::enter;
  using super::leave;
  using super::operator();

#if BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif

  enum expression_mode {
    CONJUNCTIVE, UNIVERSAL,
    DISJUNCTIVE, EXISTENTIAL,
    UNDETERMINED
  };

  bool result;
  atermpp::vector<pbes_equation> equations;
  std::stack<expression_mode> mode_stack;
  std::stack<fixpoint_symbol> symbol_stack;
  std::stack<propositional_variable> variable_stack;
  std::stack<data::variable_list> quantifier_variable_stack;
  atermpp::stack<pbes_expression> expression_stack;
  fresh_variable_name_generator name_generator;

  template <typename Container>
  ppg_rewriter(const Container& equations)
    : result(true),
      name_generator(equations)
  {}

  void enter(const not_&)
  {
    throw std::runtime_error("operation not should not occur");
  }

  void enter(const imp& /*x*/)
  {
    throw std::runtime_error("operation imp should not occur");
  }

  void enter(const data::data_expression& x)
  {
    expression_stack.push(x);
  }

  void enter(const true_& x)
  {
    expression_stack.push(x);
  }

  void enter(const false_& x)
  {
    expression_stack.push(x);
  }

  void enter(const propositional_variable_instantiation& x)
  {
    expression_stack.push(x);
  }

  propositional_variable_instantiation split_here(const pbes_expression& x)
  {
    fixpoint_symbol symbol = symbol_stack.top();
    propositional_variable variable = variable_stack.top();
    core::identifier_string fresh_varname = core::identifier_string(name_generator.generate_name(variable.name()));
    data::variable_list variable_parameters = variable.parameters() + quantifier_variable_stack.top();
    // Create fresh propositional variable.
    propositional_variable fresh_var =
        propositional_variable(fresh_varname, variable_parameters);
    pbes_equation new_eqn = pbes_equation(symbol, fresh_var, x);
    (*this)(new_eqn);
    propositional_variable_instantiation fresh_var_instantiation =
        propositional_variable_instantiation(fresh_varname, variable_parameters);
    return fresh_var_instantiation;
  }

  void operator()(const pbes_system::forall& x)
  {
    this->enter(x);
    bool simple_body = is_simple_expression(x.body());
    if (simple_body)
    {
      expression_stack.push(x);
    }
    else
    {
      expression_mode mode = mode_stack.top();
      switch(mode)
      {
      case UNDETERMINED:
      case CONJUNCTIVE:
        mode = UNIVERSAL;
      case UNIVERSAL:
      {
        quantifier_variable_stack.push(quantifier_variable_stack.top() + x.variables());
        mode_stack.push(mode);
        (*this)(x.body());
        mode_stack.pop();
        pbes_expression body = expression_stack.top();
        expression_stack.pop();
        pbes_expression expr = forall(x.variables(), body);
        expression_stack.push(expr);
        quantifier_variable_stack.pop();
        break;
      }
      case DISJUNCTIVE:
      case EXISTENTIAL:
        expression_stack.push(split_here(x));
        break;
      default:
        std::clog << "mode = " << mode << std::endl;
        throw std::runtime_error("unexpected forall");
        break;
      }
    }
    this->leave(x);
  }

  void operator()(const pbes_system::exists& x)
  {
    this->enter(x);
    bool simple_body = is_simple_expression(x.body());
    if (simple_body)
    {
      expression_stack.push(x);
    }
    else
    {
      expression_mode mode = mode_stack.top();
      switch(mode)
      {
      case UNDETERMINED:
      case DISJUNCTIVE:
        mode = EXISTENTIAL;
      case EXISTENTIAL:
      {
        quantifier_variable_stack.push(quantifier_variable_stack.top() + x.variables());
        mode_stack.push(mode);
        (*this)(x.body());
        mode_stack.pop();
        pbes_expression body = expression_stack.top();
        expression_stack.pop();
        pbes_expression expr = exists(x.variables(), body);
        expression_stack.push(expr);
        quantifier_variable_stack.pop();
        break;
      }
      case CONJUNCTIVE:
      case UNIVERSAL:
        expression_stack.push(split_here(x));
        break;
      default:
        std::clog << "mode = " << mode << std::endl;
        throw std::runtime_error("unexpected exists");
        break;
      }
    }
    this->leave(x);
  }

  void operator()(const pbes_system::and_& x)
  {
    this->enter(x);
    bool is_simple = is_simple_expression(x);
    if (is_simple)
    {
      expression_stack.push(x);
    }
    else
    {
      expression_mode mode = mode_stack.top();
      switch(mode)
      {
      case UNDETERMINED:
        mode = CONJUNCTIVE;
      case CONJUNCTIVE:
      {
        mode_stack.push(mode);
        (*this)(x.left());
        (*this)(x.right());
        mode_stack.pop();
        pbes_expression r = expression_stack.top();
        expression_stack.pop();
        pbes_expression l = expression_stack.top();
        expression_stack.pop();
        pbes_expression expr = and_(l, r);
        expression_stack.push(expr);
        break;
      }
      case UNIVERSAL:
        expression_stack.push(split_here(x));
        break;
      case EXISTENTIAL:
      case DISJUNCTIVE:
      {
        atermpp::vector<pbes_expression> conjuncts = pbes_expr::split_conjuncts(x);
        bool split = false;
        size_t count = 0;
        for(atermpp::vector<pbes_expression>::iterator it = conjuncts.begin(); it != conjuncts.end(); ++it)
        {
          if (!is_simple_expression(*it))
          {
            count++;
            if (count > 1 || !is_propositional_variable_instantiation(*it))
            {
              split = true;
              break;
            }
          }
        }
        if (split)
        {
          atermpp::vector<pbes_expression> simple_conjuncts;
          atermpp::vector<pbes_expression> new_conjuncts;
          for(atermpp::vector<pbes_expression>::iterator it = conjuncts.begin(); it != conjuncts.end(); ++it)
          {
            if (is_simple_expression(*it))
            {
              simple_conjuncts.push_back(*it);
            }
            else
            {
              new_conjuncts.push_back(*it);
            }
          }
          pbes_expression new_conj = pbes_expr::join_and(new_conjuncts.begin(), new_conjuncts.end());
          pbes_expression expr = split_here(new_conj);
          if (simple_conjuncts.size() > 0)
          {
            pbes_expression simple_conj = pbes_expr::join_and(simple_conjuncts.begin(), simple_conjuncts.end());
            expr = and_(simple_conj, expr);
          }
          expression_stack.push(expr);
        }
        else
        {
          expression_stack.push(x);
        }
        break;
      }
      default:
        std::clog << "mode = " << mode << std::endl;
        throw std::runtime_error("unexpected and");
        break;
      }
    }
    this->leave(x);
  }

  void operator()(const pbes_system::or_& x)
  {
    this->enter(x);
    bool is_simple = is_simple_expression(x);
    if (is_simple)
    {
      expression_stack.push(x);
    }
    else
    {
      expression_mode mode = mode_stack.top();
      switch(mode)
      {
      case UNDETERMINED:
        mode = DISJUNCTIVE;
      case DISJUNCTIVE:
      {
        mode_stack.push(mode);
        (*this)(x.left());
        (*this)(x.right());
        mode_stack.pop();
        pbes_expression r = expression_stack.top();
        expression_stack.pop();
        pbes_expression l = expression_stack.top();
        expression_stack.pop();
        pbes_expression expr = or_(l, r);
        expression_stack.push(expr);
        break;
      }
      case EXISTENTIAL:
        expression_stack.push(split_here(x));
        break;
      case UNIVERSAL:
      case CONJUNCTIVE:
      {
        atermpp::vector<pbes_expression> disjuncts = pbes_expr::split_disjuncts(x);
        bool split = false;
        size_t count = 0;
        for(atermpp::vector<pbes_expression>::iterator it = disjuncts.begin(); it != disjuncts.end(); ++it)
        {
          if (!is_simple_expression(*it))
          {
            count++;
            if (count > 1 || !is_propositional_variable_instantiation(*it))
            {
              split = true;
              break;
            }
          }
        }
        if (split)
        {
          atermpp::vector<pbes_expression> simple_disjuncts;
          atermpp::vector<pbes_expression> new_disjuncts;
          for(atermpp::vector<pbes_expression>::iterator it = disjuncts.begin(); it != disjuncts.end(); ++it)
          {
            if (is_simple_expression(*it))
            {
              simple_disjuncts.push_back(*it);
            }
            else
            {
              new_disjuncts.push_back(*it);
            }
          }
          pbes_expression new_disj = pbes_expr::join_or(new_disjuncts.begin(), new_disjuncts.end());
          pbes_expression expr = split_here(new_disj);
          if (simple_disjuncts.size() > 0)
          {
            pbes_expression simple_disj = pbes_expr::join_or(simple_disjuncts.begin(), simple_disjuncts.end());
            expr = or_(simple_disj, expr);
          }
          expression_stack.push(expr);

        }
        else
        {
          expression_stack.push(x);
        }
        break;
      }
      default:
        std::clog << "mode = " << mode << std::endl;
        throw std::runtime_error("unexpected or");
        break;
      }
    }
    this->leave(x);
  }

  void enter(const pbes_equation& x)
  {
    symbol_stack.push(x.symbol());
    variable_stack.push(x.variable());
    data::variable_list l;
    quantifier_variable_stack.push(l);
    mode_stack.push(UNDETERMINED);
  }

  void leave(const pbes_equation& x)
  {
    fixpoint_symbol symbol = symbol_stack.top();
    symbol_stack.pop();
    assert(symbol==x.symbol());
    propositional_variable variable = variable_stack.top();
    variable_stack.pop();
    assert(variable==x.variable());
    mode_stack.pop();
    pbes_expression expr = expression_stack.top();
    pbes_equation e(symbol, variable, expr);
    equations.push_back(e);
    expression_stack.pop();
  }

};
/// \endcond

/// \brief Rewrites a PBES to a PPG.
/// \param x a PBES
/// \return a PPG.
template <typename Container>
pbes<Container> to_ppg(const pbes<Container>& x)
{
  ppg_rewriter f(x.equations());
  f(x);
  pbes<Container> result(
      x.data(),
      f.equations,
      x.initial_state());
  return result;
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_PPG_REWRITER_H
