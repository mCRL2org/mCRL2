// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/typecheck.h
/// \brief add your file description here.

#ifndef MCRL2_MODAL_FORMULA_TYPECHECK_H
#define MCRL2_MODAL_FORMULA_TYPECHECK_H

#include "mcrl2/data/detail/data_typechecker.h"
#include "mcrl2/lps/typecheck.h"
#include "mcrl2/modal_formula/builder.h"
#include "mcrl2/modal_formula/is_monotonous.h"
#include "mcrl2/modal_formula/normalize_sorts.h"
#include "mcrl2/modal_formula/state_formula.h"
#include "mcrl2/utilities/text_utility.h"

namespace mcrl2
{

namespace action_formulas
{

typedef atermpp::term_list<data::sort_expression_list> sorts_list;

namespace detail
{

struct typecheck_builder: public action_formula_builder<typecheck_builder>
{
  typedef action_formula_builder<typecheck_builder> super;
  using super::apply;

  data::detail::data_typechecker& m_data_typechecker;
  std::map<core::identifier_string, data::sort_expression> m_variables;
  const std::multimap<core::identifier_string, process::action_label>& m_actions;

  typecheck_builder(data::detail::data_typechecker& data_typechecker,
                    const std::map<core::identifier_string, data::sort_expression>& variables,
                    const std::multimap<core::identifier_string, process::action_label>& actions
                   )
    : m_data_typechecker(data_typechecker),
      m_variables(variables),
      m_actions(actions)
  {}

  // TODO: reuse this code from process typechecker
  sorts_list matching_action_sorts(const core::identifier_string& name, const data::data_expression_list& parameters)
  {
    sorts_list result;
    auto range = m_actions.equal_range(name);
    for (auto k = range.first; k != range.second; ++k)
    {
      const process::action_label& a = k->second;
      if (a.sorts().size() == parameters.size())
      {
        result.push_front(a.sorts());
      }
    }
    return atermpp::reverse(result);
  }

  // TODO: reuse this code from process typechecker
  process::action typecheck_action(const core::identifier_string& name, const data::data_expression_list& parameters)
  {
    std::string msg = "action";
    sorts_list parameter_list = matching_action_sorts(name, parameters);
    if (parameter_list.empty())
    {
      throw mcrl2::runtime_error("no " + msg + " " + core::pp(name)
                      + " with " + atermpp::to_string(parameters.size()) + " parameter" + ((parameters.size() != 1)?"s":"")
                      + " is declared (while typechecking " + core::pp(name) + "(" + data::pp(parameters) + "))");
    }
    process::action result = process::action(process::action_label(name, m_data_typechecker.GetNotInferredList(parameter_list)), parameters);
    auto p = m_data_typechecker.match_parameters(parameters, result.label().sorts(), parameter_list, m_variables, name, msg);
    return process::action(process::action_label(name, p.second), p.first);
  }

  action_formula apply(const data::data_expression& x)
  {
    return m_data_typechecker(x, data::sort_bool::bool_(), m_variables);
  }

  action_formula apply(const action_formulas::at& x)
  {
    data::data_expression new_time = m_data_typechecker(x.time_stamp(), data::sort_real::real_(), m_variables);
    return at((*this).apply(x.operand()), new_time);
  }

  action_formula apply(const action_formulas::untyped_multi_action& x)
  {
    process::action_list new_arguments;
    for (const process::untyped_action& a: x.arguments())
    {
      new_arguments.push_front(typecheck_action(a.name(), a.arguments()));
    }
    new_arguments = atermpp::reverse(new_arguments);
    return action_formulas::multi_action(new_arguments);
  }

  action_formula apply(const action_formulas::forall& x)
  {
    try
    {
      auto m_variables_copy = m_variables;
      for (const data::variable& v: x.variables())
      {
        m_variables[v.name()] = v.sort();
      }
      action_formula body = (*this).apply(x.body());
      m_variables = m_variables_copy;
      return forall(x.variables(), body);
    }
    catch (mcrl2::runtime_error& e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + "\nwhile typechecking " + action_formulas::pp(x));
    }
  }

  action_formula apply(const action_formulas::exists& x)
  {
    try
    {
      auto m_variables_copy = m_variables;
      for (const data::variable& v: x.variables())
      {
        m_variables[v.name()] = v.sort();
      }
      action_formula body = (*this).apply(x.body());
      m_variables = m_variables_copy;
      return exists(x.variables(), body);
    }
    catch (mcrl2::runtime_error& e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + "\nwhile typechecking " + action_formulas::pp(x));
    }
  }
};

inline
typecheck_builder make_typecheck_builder(
                    data::detail::data_typechecker& data_typechecker,
                    const std::map<core::identifier_string, data::sort_expression>& variables,
                    const std::multimap<core::identifier_string, process::action_label>& actions
                   )
{
  return typecheck_builder(data_typechecker, variables, actions);
}

} // namespace detail

} // namespace action_formulas

namespace regular_formulas
{

namespace detail
{

struct typecheck_builder: public regular_formula_builder<typecheck_builder>
{
  typedef regular_formula_builder<typecheck_builder> super;
  using super::apply;

  data::detail::data_typechecker& m_data_typechecker;
  const std::map<core::identifier_string, data::sort_expression>& m_variables;
  const std::multimap<core::identifier_string, process::action_label>& m_actions;

  typecheck_builder(data::detail::data_typechecker& data_typechecker,
                    const std::map<core::identifier_string, data::sort_expression>& variables,
                    const std::multimap<core::identifier_string, process::action_label>& actions
                   )
    : m_data_typechecker(data_typechecker),
      m_variables(variables),
      m_actions(actions)
  {}

  regular_formula apply(const action_formulas::action_formula& x)
  {
    return action_formulas::detail::make_typecheck_builder(m_data_typechecker, m_variables, m_actions).apply(x);
  }
};

inline
typecheck_builder make_typecheck_builder(
                    data::detail::data_typechecker& data_typechecker,
                    const std::map<core::identifier_string, data::sort_expression>& variables,
                    const std::multimap<core::identifier_string, process::action_label>& actions
                   )
{
  return typecheck_builder(data_typechecker, variables, actions);
}

} // namespace detail

} // namespace regular_formulas

namespace state_formulas
{

namespace detail
{

struct typecheck_builder: public state_formula_builder<typecheck_builder>
{
  typedef state_formula_builder<typecheck_builder> super;
  using super::apply;

  data::detail::data_typechecker& m_data_typechecker;
  std::map<core::identifier_string, data::sort_expression> m_variables;
  const std::multimap<core::identifier_string, process::action_label>& m_actions;
  std::map<core::identifier_string, data::sort_expression_list> m_state_variables;

  typecheck_builder(data::detail::data_typechecker& data_typechecker,
                    const std::map<core::identifier_string, data::sort_expression>& variables,
                    const std::multimap<core::identifier_string, process::action_label>& actions,
                    const std::map<core::identifier_string, data::sort_expression_list>& state_variables
                   )
    : m_data_typechecker(data_typechecker),
      m_variables(variables),
      m_actions(actions),
      m_state_variables(state_variables)
  {}

  void check_sort_declared(const data::sort_expression& s, const state_formula& x)
  {
    try
    {
      m_data_typechecker.check_sort_is_declared(s);
    }
    catch (mcrl2::runtime_error& e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + "\ntype error occurred while typechecking " + state_formulas::pp(x));
    }
  }

  state_formula apply(const data::data_expression& x)
  {
    return m_data_typechecker(x, data::sort_bool::bool_(), m_variables);
  }

  state_formula apply(const state_formulas::forall& x)
  {
    try
    {
      auto m_variables_copy = m_variables;
      for (const data::variable& v: x.variables())
      {
        m_variables[v.name()] = v.sort();
      }
      state_formula body = (*this).apply(x.body());
      m_variables = m_variables_copy;
      return forall(x.variables(), body);
    }
    catch (mcrl2::runtime_error& e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + "\nwhile typechecking " + state_formulas::pp(x));
    }
  }

  state_formula apply(const state_formulas::exists& x)
  {
    try
    {
      auto m_variables_copy = m_variables;
      for (const data::variable& v: x.variables())
      {
        m_variables[v.name()] = v.sort();
      }
      state_formula body = (*this).apply(x.body());
      m_variables = m_variables_copy;
      return exists(x.variables(), body);
    }
    catch (mcrl2::runtime_error& e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + "\nwhile typechecking " + state_formulas::pp(x));
    }
  }

  state_formula apply(const state_formulas::may& x)
  {
    return may(regular_formulas::detail::make_typecheck_builder(m_data_typechecker, m_variables, m_actions).apply(x.formula()), (*this).apply(x.operand()));
  }

  state_formula apply(const state_formulas::must& x)
  {
    return must(regular_formulas::detail::make_typecheck_builder(m_data_typechecker, m_variables, m_actions).apply(x.formula()), (*this).apply(x.operand()));
  }

  state_formula apply(const state_formulas::delay_timed& x)
  {
    data::data_expression new_time = m_data_typechecker(x.time_stamp(), data::sort_real::real_(), m_variables);
    return delay_timed(new_time);
  }

  state_formula apply(const state_formulas::yaled_timed& x)
  {
    data::data_expression new_time = m_data_typechecker(x.time_stamp(), data::sort_real::real_(), m_variables);
    return yaled_timed(new_time);
  }

  state_formula apply(const state_formulas::variable& x)
  {
    auto i = m_state_variables.find(x.name());
    if (i == m_state_variables.end())
    {
      throw mcrl2::runtime_error("undefined state variable " + core::pp(x.name()) + " (typechecking state formula " + state_formulas::pp(x) + ")");
    }
    const data::sort_expression_list& expected_sorts = i->second;

    if (expected_sorts.size() != x.arguments().size())
    {
      throw mcrl2::runtime_error("incorrect number of parameters for state variable " + core::pp(x.name()) + " (typechecking state formula " + state_formulas::pp(x) + ")");
    }

    data::data_expression_list new_arguments;
    auto q1 = expected_sorts.begin();
    auto q2 = x.arguments().begin();
    for (; q1 != expected_sorts.end(); ++q1, ++q2)
    {
      new_arguments.push_front(m_data_typechecker(*q2, *q1, m_variables));
    }
    return state_formulas::variable(x.name(), new_arguments);
  }

  template <typename MuNuFormula>
  state_formula apply_mu_nu(const MuNuFormula& x, bool is_mu)
  {
    for (const data::assignment& a: x.assignments())
    {
      check_sort_declared(a.lhs().sort(), x);
    }

    data::assignment_list new_assignments = m_data_typechecker.typecheck_assignments(x.assignments(), m_variables);

    // add the assignment variables to the context
    auto m_variables_copy = m_variables;
    for (const data::assignment& a: x.assignments())
    {
      m_variables[a.lhs().name()] = a.lhs().sort();
    }

    // add x to the state variable context
    auto m_state_variables_copy = m_state_variables;
    data::sort_expression_list x_lhs_sorts;
    for (const data::assignment& a: x.assignments())
    {
      x_lhs_sorts.push_front(a.lhs().sort());
    }
    x_lhs_sorts = atermpp::reverse(x_lhs_sorts);
    m_state_variables[x.name()] = x_lhs_sorts;

    // typecheck the operand
    state_formula new_operand = (*this).apply(x.operand());

    // restore the context
    m_variables = m_variables_copy;
    m_state_variables = m_state_variables_copy;

    if (is_mu)
    {
      return mu(x.name(), new_assignments, new_operand);
    }
    else
    {
      return nu(x.name(), new_assignments, new_operand);
    }
  }

  state_formula apply(const state_formulas::nu& x)
  {
    return apply_mu_nu(x, false);
  }

  state_formula apply(const state_formulas::mu& x)
  {
    return apply_mu_nu(x, true);
  }
};

inline
typecheck_builder make_typecheck_builder(
                    data::detail::data_typechecker& data_typechecker,
                    const std::map<core::identifier_string, data::sort_expression>& variables,
                    const std::multimap<core::identifier_string, process::action_label>& actions,
                    const std::map<core::identifier_string, data::sort_expression_list>& state_variables
                   )
{
  return typecheck_builder(data_typechecker, variables, actions, state_variables);
}

} // namespace detail

class state_formula_type_checker
{
  protected:
    data::detail::data_typechecker m_data_typechecker;
    std::map<core::identifier_string, data::sort_expression> m_variables;
    std::multimap<core::identifier_string, process::action_label> m_actions;
    std::map<core::identifier_string, data::sort_expression_list> m_state_variables;

  public:
    /** \brief     Type check a state formula.
     *  Throws a mcrl2::runtime_error exception if the expression is not well typed.
     *  \param[in] d A state formula that has not been type checked.
     *  \param[in] check_monotonicity Check whether the formula is monotonic, in the sense that no fixed point
     *             variable occurs in the scope of an odd number of negations.
     *  \return    a state formula where all untyped identifiers have been replace by typed ones.
     **/
    template <typename ActionLabelContainer = std::vector<state_formulas::variable>, typename VariableContainer = std::vector<data::variable>, typename StateVariableContainer = std::vector<state_formulas::variable> >
    state_formula_type_checker(const data::data_specification& dataspec,
                               const ActionLabelContainer& action_labels = ActionLabelContainer(),
                               const VariableContainer& variables = VariableContainer(),
                               const StateVariableContainer& state_variables = StateVariableContainer()
                              )
      : m_data_typechecker(dataspec)
    {
      add_global_variables(variables);
      add_action_labels(action_labels);
      add_state_variables(state_variables);
    }

    //check correctness of the state formula in state_formula using
    //the process specification or LPS in spec as follows:
    //1) determine the types of actions according to the definitions
    //   in spec
    //2) determine the types of data expressions according to the
    //   definitions in spec
    //3) check for name conflicts of data variable declarations in
    //   forall, exists, mu and nu quantifiers
    //4) check for monotonicity of fixpoint variables
    state_formula operator()(const state_formula& x, bool check_monotonicity)
    {
      mCRL2log(log::verbose) << "type checking state formula..." << std::endl;

      state_formula result = detail::make_typecheck_builder(m_data_typechecker, m_variables, m_actions, m_state_variables).apply(state_formulas::normalize_sorts(x, m_data_typechecker.typechecked_data_specification()));
      if (check_monotonicity && !is_monotonous(result))
      {
        throw mcrl2::runtime_error("state formula is not monotonic: " + state_formulas::pp(result));
      }
      return result;
    }

  protected:
    // TODO: reuse this code
    template <typename VariableContainer>
    void add_global_variables(const VariableContainer& global_variables)
    {
      for (const data::variable& v: global_variables)
      {
        m_data_typechecker.check_sort_is_declared(v.sort());
        auto i = m_variables.find(v.name());
        if (i == m_variables.end())
        {
          m_variables[v.name()] = v.sort();
        }
        else
        {
          throw mcrl2::runtime_error("attempt to overload global variable " + core::pp(v.name()));
        }
      }
    }

    // TODO: reuse this code
    template <typename ActionLabelContainer>
    void add_action_labels(const ActionLabelContainer& actions)
    {
      for (const process::action_label& a: actions)
      {
        core::identifier_string name = a.name();
        m_data_typechecker.check_sort_list_is_declared(a.sorts());

        // Insert a in m_actions; N.B. Before that check if it already exists
        auto range = m_actions.equal_range(a.name());
        if (range.first != m_actions.end())
        {
          for (auto i = range.first; i != range.second; ++i)
          {
            if (i->second == a)
            {
              throw mcrl2::runtime_error("double declaration of action " + process::pp(a));
            }
          }
        }
        m_actions.insert(range.first, std::make_pair(a.name(), a));
      }
    }

    template <typename StateVariableContainer>
    void add_state_variables(const StateVariableContainer& state_variables)
    {
      for (const state_formulas::variable& v: state_variables)
      {
        auto i = m_state_variables.find(v.name());
        if (i == m_state_variables.end())
        {
          data::sort_expression_list sorts = data::detail::parameter_sorts(v.arguments());
          m_data_typechecker.check_sort_list_is_declared(sorts);
          m_state_variables[v.name()] = sorts;
        }
        else
        {
          throw mcrl2::runtime_error("attempt to overload state variable " + core::pp(v.name()));
        }
      }
    }
};

/** \brief     Type check a state formula.
 *  Throws an exception if something went wrong.
 *  \param[in] formula A state formula that has not been type checked.
 *  \post      formula is type checked.
 **/
inline
state_formula type_check(const state_formula& x, const lps::specification& lpsspec, bool check_monotonicity = true)
{
  try
  {
    state_formula_type_checker type_checker(lpsspec.data(), lpsspec.action_labels());
    return type_checker(x, check_monotonicity);
  }
  catch (mcrl2::runtime_error& e)
  {
    throw mcrl2::runtime_error(std::string(e.what()) + "\ncould not type check modal formula " + state_formulas::pp(x));
  }
}

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_TYPECHECK_H
