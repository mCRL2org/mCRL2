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

  std::map<core::identifier_string, data::data_expression> make_assignment_map(const data::assignment_list& assignments)
  {
    std::map<core::identifier_string, data::data_expression> result;
    for (auto const& a: assignments)
    {
      auto i = result.find(a.lhs().name());
      if (i != result.end()) // An assignment of the shape x := t already exists, this is not OK.
      {
        throw mcrl2::runtime_error("Double data::assignment to data::variable " + core::pp(a.lhs()) + " (detected assigned values are " + data::pp(i->second) + " and " + core::pp(a.rhs()) + ")");
      }
      result[a.lhs().name()] = a.rhs();
    }
    return result;
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
    std::map<core::identifier_string, data::data_expression> assignments = make_assignment_map(x.assignments());

    // typecheck the assignments
    data::assignment_list new_assignments;
    for (const data::assignment& a: x.assignments())
    {
      check_sort_declared(a.lhs().sort(), x);
      data::sort_expression expected_sort = m_data_typechecker.ExpandNumTypesDown(a.lhs().sort());
      data::data_expression rhs = m_data_typechecker(a.rhs(), expected_sort, m_variables);
      new_assignments.push_front(data::assignment(a.lhs(), rhs));
    }
    new_assignments = atermpp::reverse(new_assignments);

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

class state_formula_type_checker : lps::action_type_checker
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
      : lps::action_type_checker(dataspec, process::action_label_list(action_labels.begin(), action_labels.end())),
        m_data_typechecker(dataspec)
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
    state_formula operator()(const state_formula& formula, bool check_monotonicity)
    {
      mCRL2log(log::verbose) << "type checking state formula..." << std::endl;

      std::map<core::identifier_string, data::sort_expression> variables;
      std::map<core::identifier_string, data::sort_expression_list> state_variables;
      state_formula result=TraverseStateFrm(variables, state_variables, formula);
      if (check_monotonicity && !is_monotonous(result))
      {
        throw mcrl2::runtime_error("state formula is not monotonic: " + state_formulas::pp(result));
      }

      // state_formula result1 = detail::make_typecheck_builder(m_data_typechecker, m_variables, m_actions, m_state_variables).apply(state_formulas::normalize_sorts(formula, m_data_typechecker.typechecked_data_specification()));
      // if (result != result1)
      // {
      //   std::cout << "result  == " << result << std::endl;
      //   std::cout << "result1 == " << result1 << std::endl;
      //   assert(false);
      // }
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

    state_formula TraverseStateFrm(const std::map<core::identifier_string, data::sort_expression>& Vars, const std::map<core::identifier_string, data::sort_expression_list>& m_state_variables, const state_formula& StateFrm)
    {
      mCRL2log(log::debug) << "TraverseStateFrm: " + pp(StateFrm) + "" << std::endl;

      if (state_formulas::is_true(StateFrm) || state_formulas::is_false(StateFrm) || state_formulas::is_delay(StateFrm) || state_formulas::is_yaled(StateFrm))
      {
        return StateFrm;
      }

      if (state_formulas::is_not(StateFrm))
      {
        const  not_& t = atermpp::down_cast<const not_>(StateFrm);
        return not_(TraverseStateFrm(Vars,m_state_variables,t.operand()));
      }

      if (state_formulas::is_and(StateFrm))
      {
        const and_& t=atermpp::down_cast<const and_>(StateFrm);
        return and_(TraverseStateFrm(Vars,m_state_variables,t.left()),TraverseStateFrm(Vars,m_state_variables,t.right()));
      }

      if (state_formulas::is_or(StateFrm))
      {
        const or_& t=atermpp::down_cast<const or_>(StateFrm);
        return or_(TraverseStateFrm(Vars,m_state_variables,t.left()),TraverseStateFrm(Vars,m_state_variables,t.right()));
      }

      if (state_formulas::is_imp(StateFrm))
      {
        const imp& t=atermpp::down_cast<const imp>(StateFrm);
        return imp(TraverseStateFrm(Vars,m_state_variables,t.left()),TraverseStateFrm(Vars,m_state_variables,t.right()));
      }

      if (state_formulas::is_forall(StateFrm))
      {
        const forall& t=atermpp::down_cast<const forall>(StateFrm);
        std::map<core::identifier_string,data::sort_expression> CopyVars(Vars);

        std::map<core::identifier_string,data::sort_expression> NewVars;
        AddVars2Table(CopyVars,t.variables());
        NewVars=CopyVars;

        return forall(t.variables(),TraverseStateFrm(NewVars,m_state_variables,t.body()));
      }

      if (state_formulas::is_exists(StateFrm))
      {
        const exists& t=atermpp::down_cast<const exists>(StateFrm);
        std::map<core::identifier_string,data::sort_expression> CopyVars(Vars);

        std::map<core::identifier_string,data::sort_expression> NewVars;
        AddVars2Table(CopyVars,t.variables());
        NewVars=CopyVars;

        return exists(t.variables(),TraverseStateFrm(NewVars,m_state_variables,t.body()));
      }

      if (is_may(StateFrm))
      {
        const may& f=atermpp::down_cast<const may>(StateFrm);
        return may(TraverseRegFrm(Vars,f.formula()),TraverseStateFrm(Vars,m_state_variables,f.operand()));
      }

      if (is_must(StateFrm))
      {
        const must& f=atermpp::down_cast<const must>(StateFrm);
        return must(TraverseRegFrm(Vars,f.formula()),TraverseStateFrm(Vars,m_state_variables,f.operand()));
      }

      if (state_formulas::is_delay_timed(StateFrm))
      {
        const delay_timed& f=atermpp::down_cast<const delay_timed>(StateFrm);
        data::data_expression Time=f.time_stamp();
        data::sort_expression NewType=TraverseVarConsTypeD(Vars,Vars,Time,ExpandNumTypesDown(data::sort_real::real_()));

        data::sort_expression temp;
        if (!TypeMatchA(data::sort_real::real_(),NewType,temp))
        {
          //upcasting
          data::sort_expression CastedNewType;
          try
          {
            std::map<core::identifier_string,data::sort_expression> dummy_table;
            CastedNewType=UpCastNumericType(data::sort_real::real_(),NewType,Time,Vars,Vars,dummy_table,false);
          }
          catch (mcrl2::runtime_error &e)
          {
            throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot (up)cast time value " + pp(Time) + " to type Real (typechecking state formula " + pp(StateFrm) + ")");
          }
        }
        return delay_timed(Time);
      }

      if (state_formulas::is_yaled_timed(StateFrm))
      {
        const yaled_timed& f=atermpp::down_cast<const yaled_timed>(StateFrm);
        data::data_expression Time=f.time_stamp();
        data::sort_expression NewType=TraverseVarConsTypeD(Vars,Vars,Time,ExpandNumTypesDown(data::sort_real::real_()));

        data::sort_expression temp;
        if (!TypeMatchA(data::sort_real::real_(),NewType,temp))
        {
          //upcasting
          data::sort_expression CastedNewType;
          try
          {
            std::map<core::identifier_string,data::sort_expression> dummy_table;
            CastedNewType=UpCastNumericType(data::sort_real::real_(),NewType,Time,Vars,Vars,dummy_table,false);
          }
          catch (mcrl2::runtime_error &e)
          {
            throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot (up)cast time value " + pp(Time) + " to type Real (typechecking state formula " + pp(StateFrm) + ")");
          }
        }
        return yaled_timed(Time);
      }

      if (state_formulas::is_variable(StateFrm))
      {
        state_formulas::variable v=atermpp::down_cast<const state_formulas::variable>(StateFrm);
        core::identifier_string StateVarName=v.name();
        auto i=m_state_variables.find(StateVarName);
        if (i==m_state_variables.end())
        {
          throw mcrl2::runtime_error("undefined state variable " + to_string(StateVarName) + " (typechecking state formula " + pp(StateFrm) + ")");
        }
        data::sort_expression_list TypeList=i->second;

        const data::data_expression_list Pars=v.arguments();
        if (TypeList.size()!=Pars.size())
        {
          throw mcrl2::runtime_error("incorrect number of parameters for state variable " + core::pp(StateVarName) + " (typechecking state formula " + pp(StateFrm) + ")");
        }

        data::data_expression_list r;

        for (data::data_expression Par: Pars)
        {
          data::sort_expression ParType=TypeList.front();
          data::sort_expression NewParType;
          try
          {
            NewParType=TraverseVarConsTypeD(Vars,Vars,Par,ExpandNumTypesDown(ParType));
          }
          catch (mcrl2::runtime_error &e)
          {
            throw mcrl2::runtime_error(std::string(e.what()) + "\ntypechecking " + pp(StateFrm));
          }

          data::sort_expression temp;
          if (!TypeMatchA(ParType,NewParType,temp))
          {
            //upcasting
            try
            {
              std::map<core::identifier_string,data::sort_expression> dummy_table;
              NewParType=UpCastNumericType(ParType,NewParType,Par,Vars,Vars,dummy_table,false);
            }
            catch (mcrl2::runtime_error &e)
            {
              throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot (up)cast " + pp(Par) + " to type " + pp(ParType) + " (typechecking state formula " + pp(StateFrm) + ")");
            }
          }

          r.push_front(Par);
        }

        return state_formulas::variable(StateVarName, atermpp::reverse(r));

      }

      if (state_formulas::is_nu(StateFrm))
      {
        const nu& f=atermpp::down_cast<const nu>(StateFrm);
        std::map<core::identifier_string, data::sort_expression_list> CopyStateVars(m_state_variables);

        // Make the new state variable:
        std::map<core::identifier_string,data::sort_expression> FormPars;
        data::assignment_list r;
        data::sort_expression_list t;
        for (const data::assignment& o: f.assignments())
        {
          core::identifier_string VarName=o.lhs().name();
          if (FormPars.count(VarName)>0)
          {
            throw mcrl2::runtime_error("non-unique formal parameter " + core::pp(VarName) + " (typechecking " + pp(StateFrm) + ")");
          }

          data::sort_expression VarType=o.lhs().sort();
          try
          {
            check_sort_is_declared(VarType);
          }
          catch (mcrl2::runtime_error &e)
          {
            throw mcrl2::runtime_error(std::string(e.what()) + "\ntype error occurred while typechecking " + pp(StateFrm));
          }

          FormPars[VarName]=VarType;

          data::data_expression VarInit=o.rhs();
          data::sort_expression VarInitType;
          try
          {
            VarInitType=TraverseVarConsTypeD(Vars,Vars,VarInit,ExpandNumTypesDown(VarType));
          }
          catch (mcrl2::runtime_error &e)
          {
            throw mcrl2::runtime_error(std::string(e.what()) + "\ntypechecking " + pp(StateFrm) + ".");
          }

          data::sort_expression temp;
          if (!TypeMatchA(VarType,VarInitType,temp))
          {
            //upcasting
            try
            {
              std::map<core::identifier_string,data::sort_expression> dummy_table;
              VarInitType=UpCastNumericType(VarType,VarInitType,VarInit,Vars,Vars,dummy_table,false);
            }
            catch (mcrl2::runtime_error &e)
            {
              throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot (up)cast " + pp(VarInit) + " to type " + pp(VarType) + " (typechecking state formula " + pp(StateFrm));
            }
          }

          r.push_front(data::assignment(o.lhs(),VarInit));
          t.push_front(VarType);
        }

        std::map<core::identifier_string,data::sort_expression> CopyVars(Vars);
        CopyVars.insert(FormPars.begin(),FormPars.end());


        CopyStateVars[f.name()]=atermpp::reverse(t);

        try
        {
          return nu(f.name(),atermpp::reverse(r),TraverseStateFrm(CopyVars,CopyStateVars,f.operand()));
        }
        catch (mcrl2::runtime_error &e)
        {
          throw mcrl2::runtime_error(std::string(e.what()) + "\nwhile typechecking " + to_string(f));
        }
      }

      if (state_formulas::is_mu(StateFrm))
      {
        const mu& f=atermpp::down_cast<const mu>(StateFrm);
        std::map<core::identifier_string, data::sort_expression_list> CopyStateVars(m_state_variables);

        // Make the new state variable:
        std::map<core::identifier_string, data::sort_expression> FormPars;
        data::assignment_list r;
        data::sort_expression_list t;
        for (const data::assignment& o: f.assignments())
        {
          core::identifier_string VarName=o.lhs().name();
          if (FormPars.count(VarName)>0)
          {
            throw mcrl2::runtime_error("non-unique formal parameter " + core::pp(VarName) + " (typechecking " + pp(StateFrm) + ")");
          }

          data::sort_expression VarType=o.lhs().sort();
          try
          {
            check_sort_is_declared(VarType);
          }
          catch (mcrl2::runtime_error &e)
          {
            throw mcrl2::runtime_error(std::string(e.what()) + "\ntype error occurred while typechecking " + pp(StateFrm));
          }

          FormPars[VarName]=VarType;

          data::data_expression VarInit=o.rhs();
          data::sort_expression VarInitType;
          try
          {
            VarInitType=TraverseVarConsTypeD(Vars,Vars,VarInit,ExpandNumTypesDown(VarType));
          }
          catch (mcrl2::runtime_error &e)
          {
            throw mcrl2::runtime_error(std::string(e.what()) + "\ntypechecking " + pp(StateFrm) + ".");
          }

          data::sort_expression temp;
          if (!TypeMatchA(VarType,VarInitType,temp))
          {
            //upcasting
            try
            {
              std::map<core::identifier_string,data::sort_expression> dummy_table;
              VarInitType=UpCastNumericType(VarType,VarInitType,VarInit,Vars,Vars,dummy_table,false);
            }
            catch (mcrl2::runtime_error &e)
            {
              throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot (up)cast " + pp(VarInit) + " to type " + pp(VarType) + " (typechecking state formula " + pp(StateFrm));
            }
          }

          r.push_front(data::assignment(o.lhs(),VarInit));
          t.push_front(VarType);
        }

        std::map<core::identifier_string,data::sort_expression> CopyVars(Vars);
        CopyVars.insert(FormPars.begin(),FormPars.end());
        CopyStateVars[f.name()]=atermpp::reverse(t);

        try
        {
          return mu(f.name(),atermpp::reverse(r),TraverseStateFrm(CopyVars,CopyStateVars,f.operand()));
        }
        catch (mcrl2::runtime_error &e)
        {
          throw mcrl2::runtime_error(std::string(e.what()) + "\nwhile typechecking " + to_string(f));
        }
      }

      if (data::is_data_expression(StateFrm))
      {
        data::data_expression d(StateFrm);
        data::sort_expression Type=TraverseVarConsTypeD(Vars, Vars, d, data::sort_bool::bool_());
        return d;
      }

      throw mcrl2::runtime_error("Internal error. The state formula " + pp(StateFrm) + " fails to match any known form in typechecking case analysis");
    }

    regular_formulas::regular_formula TraverseRegFrm(const std::map<core::identifier_string, data::sort_expression>& Vars, const regular_formulas::regular_formula& RegFrm)
    {
      using namespace regular_formulas;
      mCRL2log(log::debug) << "TraverseRegFrm: " << pp(RegFrm) << "" << std::endl;
      if (regular_formulas::is_nil(RegFrm))
      {
        return RegFrm;
      }

      if (is_seq(RegFrm))
      {
        const seq t=atermpp::down_cast<const seq>(RegFrm);
        return seq(TraverseRegFrm(Vars,t.left()),TraverseRegFrm(Vars,t.right()));
      }

      if (is_alt(RegFrm))
      {
        const alt t=atermpp::down_cast<const alt>(RegFrm);
        return alt(TraverseRegFrm(Vars,t.left()),TraverseRegFrm(Vars,t.right()));
      }

      if (is_trans(RegFrm))
      {
        const trans t=atermpp::down_cast<const trans>(RegFrm);
        return trans(TraverseRegFrm(Vars,t.operand()));
      }

      if (is_trans_or_nil(RegFrm))
      {
        const trans t=atermpp::down_cast<const trans>(RegFrm);
        return trans_or_nil(TraverseRegFrm(Vars,t.operand()));
      }

      if (action_formulas::is_action_formula(RegFrm))
      {
        action_formulas::action_formula a = TraverseActFrm(Vars, atermpp::down_cast<const action_formulas::action_formula>(RegFrm));
        return regular_formula(a);
      }

      throw mcrl2::runtime_error("Internal error. The regularformula " + pp(RegFrm) + " fails to match any known form in typechecking case analysis");
    }

    action_formulas::action_formula TraverseActFrm(const std::map<core::identifier_string,data::sort_expression>& Vars, const action_formulas::action_formula& ActFrm)
    {
      using namespace action_formulas;
      mCRL2log(log::debug) << "TraverseActFrm: " << pp(ActFrm) << std::endl;

      if (action_formulas::is_true(ActFrm) || action_formulas::is_false(ActFrm))
      {
        return ActFrm;
      }

      if (action_formulas::is_not(ActFrm))
      {
        const action_formulas::not_& f = atermpp::down_cast<action_formulas::not_>(ActFrm);
        return action_formulas::not_(TraverseActFrm(Vars,f.operand()));
      }

      if (action_formulas::is_and(ActFrm))
      {
        const action_formulas::and_& t=atermpp::down_cast<action_formulas::and_>(ActFrm);
        return action_formulas::and_(TraverseActFrm(Vars,t.left()),TraverseActFrm(Vars,t.right()));
      }

      if (action_formulas::is_or(ActFrm))
      {
        const action_formulas::or_& t=atermpp::down_cast<action_formulas::or_>(ActFrm);
        return action_formulas::or_(TraverseActFrm(Vars,t.left()),TraverseActFrm(Vars,t.right()));
      }

      if (action_formulas::is_imp(ActFrm))
      {
        const action_formulas::imp& t=atermpp::down_cast<action_formulas::imp>(ActFrm);
        return action_formulas::imp(TraverseActFrm(Vars,t.left()),TraverseActFrm(Vars,t.right()));
      }

      if (action_formulas::is_forall(ActFrm))
      {
        const action_formulas::forall& t=atermpp::down_cast<action_formulas::forall>(ActFrm);
        std::map<core::identifier_string,data::sort_expression> CopyVars(Vars);

        const data::variable_list& VarList=t.variables();
        std::map<core::identifier_string,data::sort_expression> NewVars;
        AddVars2Table(CopyVars,VarList);
        NewVars=CopyVars;

        return action_formulas::forall(VarList, TraverseActFrm(NewVars,t.body()));
      }

      if (action_formulas::is_exists(ActFrm))
      {
        const action_formulas::exists& t=atermpp::down_cast<action_formulas::exists>(ActFrm);
        std::map<core::identifier_string,data::sort_expression> CopyVars(Vars);

        const data::variable_list& VarList=t.variables();
        std::map<core::identifier_string,data::sort_expression> NewVars;
        AddVars2Table(CopyVars,VarList);
        NewVars=CopyVars;

        return action_formulas::exists(VarList, TraverseActFrm(NewVars,t.body()));
      }

      if (action_formulas::is_at(ActFrm))
      {
        const action_formulas::at& t=atermpp::down_cast<action_formulas::at>(ActFrm);
        action_formula NewArg1=TraverseActFrm(Vars,t.operand());

        data::data_expression Time=t.time_stamp();
        data::sort_expression NewType=TraverseVarConsTypeD(Vars,Vars,Time,ExpandNumTypesDown(data::sort_real::real_()));

        data::sort_expression temp;
        if (!TypeMatchA(data::sort_real::real_(),NewType,temp))
        {
          //upcasting
          data::sort_expression CastedNewType;
          try
          {
            std::map<core::identifier_string,data::sort_expression> dummy_table;
            CastedNewType=UpCastNumericType(data::sort_real::real_(),NewType,Time,Vars,Vars,dummy_table,false);
          }
          catch (mcrl2::runtime_error &e)
          {
            throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot (up)cast time value " + pp(Time) + " to type Real (typechecking action formula " + pp(ActFrm) + ")");
          }
        }
        return action_formulas::at(NewArg1,Time);
      }

      if (action_formulas::is_untyped_multi_action(ActFrm))
      {
        const action_formulas::untyped_multi_action ma(ActFrm);
        process::action_list r;
        for (auto l=ma.arguments().begin(); l!=ma.arguments().end(); ++l)
        {
          process::untyped_action o= *l;
          r.push_front(RewrAct(Vars,o));
        }
        return action_formulas::multi_action(atermpp::reverse(r));
      }

      if (data::is_data_expression(ActFrm))
      {
        data::data_expression d(ActFrm);
        data::sort_expression Type=TraverseVarConsTypeD(Vars, Vars, d, data::sort_bool::bool_());
        return d;
      }

      throw mcrl2::runtime_error("Internal error. The action formula " + pp(ActFrm) + " fails to match any known form in typechecking case analysis");
    }
};

/** \brief     Type check a state formula.
 *  Throws an exception if something went wrong.
 *  \param[in] formula A state formula that has not been type checked.
 *  \post      formula is type checked.
 **/
inline
void type_check(state_formula& formula, const lps::specification& lps_spec, bool check_monotonicity = true)
{
  try
  {
    state_formula_type_checker type_checker(lps_spec.data(), lps_spec.action_labels());
    formula=type_checker(formula,check_monotonicity);
  }
  catch (mcrl2::runtime_error &e)
  {
    throw mcrl2::runtime_error(std::string(e.what()) + "\ncould not type check modal formula " +
                             mcrl2::utilities::to_string(formula));
  }
}

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_TYPECHECK_H
