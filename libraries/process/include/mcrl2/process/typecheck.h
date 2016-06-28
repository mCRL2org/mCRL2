// Author(s): Jan Friso Groote, Wieger Wesselink (2015)
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/typecheck.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_TYPECHECK_H
#define MCRL2_PROCESS_TYPECHECK_H

#include <algorithm>
#include <iostream>
#include "mcrl2/data/typecheck.h"
#include "mcrl2/process/builder.h"
#include "mcrl2/process/normalize_sorts.h"
#include "mcrl2/process/process_specification.h"
#include "mcrl2/process/detail/action_context.h"
#include "mcrl2/process/detail/match_action_parameters.h"
#include "mcrl2/process/detail/process_context.h"
#include "mcrl2/utilities/detail/container_utility.h"

namespace mcrl2
{

namespace process
{

inline
action typecheck_action(const core::identifier_string& name,
                        const data::data_expression_list& parameters,
                        data::data_type_checker& typechecker,
                        const data::detail::variable_context& variable_context,
                        const detail::action_context& action_context
                       )
{
  std::string msg = "action";
  data::sorts_list parameter_list = action_context.matching_action_sorts(name, parameters);
  auto p = process::detail::match_action_parameters(parameters, parameter_list, variable_context, name, msg, typechecker);
  return action(action_label(name, p.second), p.first);
}

// returns the intersection of the 2 type list lists
inline
data::sorts_list sorts_list_intersection(const data::sorts_list& sorts1, const data::sorts_list& sorts2)
{
  data::sorts_list result;
  for (const data::sort_expression_list& s: sorts2)
  {
    if (std::find(sorts1.begin(), sorts1.end(), s) != sorts1.end())
    {
      result.push_front(s);
    }
  }
  return atermpp::reverse(result);
}

inline
std::ostream& operator<<(std::ostream& out, const data::sorts_list& x)
{
  out << "[";
  for (auto i = x.begin(); i != x.end(); ++i)
  {
    if (i != x.begin())
    {
      out << ", ";
    }
    out << *i;
  }
  return out;
}

/// \brief
inline
core::identifier_string_list list_difference(const core::identifier_string_list& l, const core::identifier_string_list& m)
{
  core::identifier_string_list n;
  for (const core::identifier_string& elem: l)
  {
    if (std::find(m.begin(), m.end(), elem) == m.end())
    {
      n.push_front(elem);
    }
  }
  return atermpp::reverse(n);
}

namespace detail
{

bool equal_multi_actions(core::identifier_string_list a1, core::identifier_string_list a2);

// returns true if a is in A
inline
bool multi_actions_contains(const core::identifier_string_list& a, const action_name_multiset_list& A)
{
  for (const action_name_multiset& i: A)
  {
    if (equal_multi_actions(a, i.names()))
    {
      return true;
    }
  }
  return false;
}

// returns true if the two multiactions are equal.
inline
bool equal_multi_actions(core::identifier_string_list a1, core::identifier_string_list a2)
{
  if (a1.size() != a2.size())
  {
    return false;
  }
  if (a1.empty())
  {
    return true;
  }
  core::identifier_string Act1 = a1.front();
  a1 = a1.tail();

  //remove Act1 once from a2. if not there -- return ATfalse.
  core::identifier_string_list NewMAct2;
  for (; !a2.empty(); a2 = a2.tail())
  {
    core::identifier_string Act2 = a2.front();
    if (Act1 == Act2)
    {
      a2 = atermpp::reverse(NewMAct2) + a2.tail();
      return equal_multi_actions(a1, a2);
    }
    else
    {
      NewMAct2.push_front(Act2);
    }
  }
  return false;
}

inline
std::ostream& operator<<(std::ostream& out, const std::pair<core::identifier_string, data::sort_expression_list>& x)
{
  return out << "(" << x.first << ", " << x.second << ")";
}

struct typecheck_builder: public process_expression_builder<typecheck_builder>
{
  typedef process_expression_builder<typecheck_builder> super;
  using super::apply;

  data::data_type_checker& m_data_type_checker;
  data::detail::variable_context m_variable_context;
  const detail::process_context& m_process_context;
  const detail::action_context& m_action_context;

  typecheck_builder(data::data_type_checker& data_typechecker,
                    const data::detail::variable_context& variable_context,
                    const detail::process_context& process_context,
                    const detail::action_context& action_context
                   )
    : m_data_type_checker(data_typechecker),
      m_variable_context(variable_context),
      m_process_context(process_context),
      m_action_context(action_context)
  {}

  data::sorts_list action_sorts(const core::identifier_string& name)
  {
    return m_action_context.matching_action_sorts(name);
  }

  void check_action_declared(const core::identifier_string& a, const process_expression& x)
  {
    if (!m_action_context.is_declared(a))
    {
      throw mcrl2::runtime_error("Undefined action " + core::pp(a) + " (typechecking " + core::pp(x) + ")");
    }
  }

  void check_actions_declared(const core::identifier_string_list& act_list, const process_expression& x)
  {
    std::set<core::identifier_string> actions;
    for (const core::identifier_string& a: act_list)
    {
      check_action_declared(a, x);
      if (!actions.insert(a).second)  // The action was already in the set.
      {
        mCRL2log(log::warning) << "Used action " << a << " twice (typechecking " << x << ")" << std::endl;
      }
    }
  }

  template <typename Container>
  void check_not_empty(const Container& c, const std::string& msg, const process_expression& x)
  {
    if (c.empty())
    {
      mCRL2log(log::warning) << msg << " (typechecking " << x << ")" << std::endl;
    }
  }

  template <typename T>
  void check_not_equal(const T& first, const T& second, const std::string& msg, const process_expression& x)
  {
    if (first == second)
    {
      mCRL2log(log::warning) << msg << " " << first << "(typechecking " << x << ")" << std::endl;
    }
  }

  bool has_empty_intersection(const data::sorts_list& s1, const data::sorts_list& s2)
  {
    std::set<data::sort_expression_list> v1(s1.begin(), s1.end());
    std::set<data::sort_expression_list> v2(s2.begin(), s2.end());
    return utilities::detail::has_empty_intersection(v1, v2);
  }

  void check_rename_common_type(const core::identifier_string& a, const core::identifier_string& b, const process_expression& x)
  {
    if (has_empty_intersection(action_sorts(a), action_sorts(b)))
    {
      throw mcrl2::runtime_error("renaming action " + core::pp(a) + " into action " + core::pp(b) + ": these two have no common type (typechecking " + process::pp(x) + ")");
    }
  }

  std::map<core::identifier_string, data::data_expression> make_assignment_map(const data::untyped_identifier_assignment_list& assignments)
  {
    std::map<core::identifier_string, data::data_expression> result;
    for (const data::untyped_identifier_assignment& a: assignments)
    {
      auto i = result.find(a.lhs());
      if (i != result.end()) // An data::assignment of the shape x:=t already exists, this is not OK.
      {
        throw mcrl2::runtime_error("Double data::assignment to data::variable " + core::pp(a.lhs()) + " (detected assigned values are " + data::pp(i->second) + " and " + core::pp(a.rhs()) + ")");
      }
      result[a.lhs()]=a.rhs();
    }
    return result;
  }

  action make_action(const core::identifier_string& name, const data::sort_expression_list& formal_parameters, const data::data_expression_list& actual_parameters)
  {
    return action(action_label(name, formal_parameters), actual_parameters);
  }

  bool is_action_name(const core::identifier_string& name)
  {
    return m_action_context.is_declared(name);
  }

  bool is_process_name(const core::identifier_string& name)
  {
    return m_process_context.is_declared(name);
  }

  action typecheck_action(const core::identifier_string& name, const data::data_expression_list& parameters)
  {
    return process::typecheck_action(name, parameters, m_data_type_checker, m_variable_context, m_action_context);
  }

  process_instance typecheck_process_instance(const core::identifier_string& name, const data::data_expression_list& parameters)
  {
    std::string msg = "process";
    data::sorts_list parameter_list = m_process_context.matching_process_sorts(name, parameters);
    auto p = process::detail::match_action_parameters(parameters, parameter_list, m_variable_context, name, msg, m_data_type_checker);
    return m_process_context.make_process_instance(name, p.second, p.first);
  }

  process_expression apply(const untyped_process_assignment& x)
  {
    mCRL2log(log::debug) << "typechecking a process call with short-hand assignments " << x << "" << std::endl;
    if (!is_process_name(x.name()))
    {
      throw mcrl2::runtime_error("Could not find a matching declaration for action or process expression " + core::pp(x.name()) + core::detail::print_arguments(x.assignments()) + ".");
    }

    data::variable_list formal_parameters = m_process_context.matching_process_parameters(x);
    std::map<core::identifier_string, data::data_expression> assignments = make_assignment_map(x.assignments());

    // create actual parameters, with untyped identifiers for the parameters that are not assigned a value
    data::data_expression_list actual_parameters;
    for (const data::variable& d: formal_parameters)
    {
      data::data_expression e;
      auto i = assignments.find(d.name());
      if (i == assignments.end())
      {
        e = data::untyped_identifier(d.name());
      }
      else
      {
        e = i->second;
      }
      actual_parameters.push_front(e);
    }
    actual_parameters = atermpp::reverse(actual_parameters);

    // typecheck the actual parameters
    process_instance px;
    try
    {
      px = typecheck_process_instance(x.name(), actual_parameters);
    }
    catch (mcrl2::runtime_error& e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + "\ntype error occurred while typechecking the process call with short-hand assignments " + process::pp(x));
    }
    const data::data_expression_list& typechecked_actual_parameters = px.actual_parameters();

    // construct typechecked assignments
    data::assignment_list typechecked_assignments;
    auto q1 = formal_parameters.begin();
    auto q2 = typechecked_actual_parameters.begin();
    for (; q1 != formal_parameters.end(); ++q1, ++q2)
    {
      if (assignments.find(q1->name()) != assignments.end())
      {
        typechecked_assignments.push_front(data::assignment(*q1, *q2));
      }
    }
    typechecked_assignments = atermpp::reverse(typechecked_assignments);
    return process_instance_assignment(px.identifier(), typechecked_assignments);
  }

  process_expression apply(const data::untyped_data_parameter& x)
  {
    if (is_action_name(x.name()))
    {
      return typecheck_action(x.name(), x.arguments());
    }
    else if (is_process_name(x.name()))
    {
      return typecheck_process_instance(x.name(), x.arguments());
    }
    else
    {
      throw mcrl2::runtime_error("Could not find a matching declaration for action or process expression " + core::pp(x.name()) + core::detail::print_arguments(x.arguments()) + ".");
    }
  }

  process_expression apply(const process::hide& x)
  {
    check_not_empty(x.hide_set(), "Hiding empty set of actions", x);
    check_actions_declared(x.hide_set(), x);
    return process::hide(x.hide_set(), (*this).apply(x.operand()));
  }

  process_expression apply(const process::block& x)
  {
    check_not_empty(x.block_set(), "Blocking empty set of actions", x);
    check_actions_declared(x.block_set(), x);
    return block(x.block_set(), (*this).apply(x.operand()));
  }

  process_expression apply(const process::rename& x)
  {
    check_not_empty(x.rename_set(), "Renaming empty set of actions", x);

    std::set<core::identifier_string> actions;
    for (const rename_expression& r: x.rename_set())
    {
      check_not_equal(r.source(), r.target(), "Renaming action into itself:", x);
      check_action_declared(r.source(), x);
      check_action_declared(r.target(), x);
      check_rename_common_type(r.source(), r.target(), x);
      if (!actions.insert(r.source()).second) // The element was already in the set.
      {
        throw mcrl2::runtime_error("renaming action " + core::pp(r.source()) + " twice (typechecking " + process::pp(x) + ")");
      }
    }
    return rename(x.rename_set(), (*this).apply(x.operand()));
  }

  //comm: like renaming multiactions (with the same parameters) to action/tau
  process_expression apply(const process::comm& x)
  {
    check_not_empty(x.comm_set(), "synchronizing empty set of (multi)actions", x);

    std::set<core::identifier_string> left_hand_side_actions;
    for (const communication_expression& c: x.comm_set())
    {
      const core::identifier_string_list& cnames = c.action_name().names();
      assert(!cnames.empty());

      if (cnames.size() == 1)
      {
        throw mcrl2::runtime_error("using synchronization as renaming/hiding of action " + core::pp(cnames.front()) + " into " + core::pp(c.name()) + " (typechecking " + process::pp(x) + ")");
      }

      //Actions must be declared
      data::sorts_list c_sorts;
      if (!core::is_nil(c.name()))
      {
        if (!m_action_context.is_declared(c.name()))
        {
          throw mcrl2::runtime_error("synchronizing to an undefined action " + core::pp(c.name()) + " (typechecking " + process::pp(x) + ")");
        }
        c_sorts = action_sorts(c.name());
      }

      for (const core::identifier_string& a: cnames)
      {
        if (!m_action_context.is_declared(a))
        {
          throw mcrl2::runtime_error("synchronizing an undefined action " + core::pp(a) + " in (multi)action " + core::pp(cnames) + " (typechecking " + process::pp(x) + ")");
        }
        c_sorts = sorts_list_intersection(c_sorts, action_sorts(a));
        if (c_sorts.empty())
        {
          throw mcrl2::runtime_error("synchronizing action " + core::pp(a) + " from (multi)action " + core::pp(cnames) +
                            " into action " + core::pp(c.name()) + ": these have no common type (typechecking " + process::pp(x) + ")");
        }
      }

      //the multiactions in the lhss of comm should not intersect.
      for (const core::identifier_string& a: std::set<core::identifier_string>(cnames.begin(), cnames.end()))
      {
        if (left_hand_side_actions.find(a) != left_hand_side_actions.end())
        {
          throw mcrl2::runtime_error("synchronizing action " + core::pp(a) + " in different ways (typechecking " + process::pp(x) + ")");
        }
        else
        {
          left_hand_side_actions.insert(a);
        }
      }
    }

    return comm(x.comm_set(), (*this).apply(x.operand()));
  }

  process_expression apply(const process::allow& x)
  {
    check_not_empty(x.allow_set(), "Allowing empty set of (multi) actions", x);
    action_name_multiset_list MActs;
    for (const action_name_multiset& A: x.allow_set())
    {
      //Actions must be declared
      for (const core::identifier_string& a: A.names())
      {
        if (!m_action_context.is_declared(a))
        {
          throw mcrl2::runtime_error("allowing an undefined action " + core::pp(a) + " in (multi)action " + core::pp(A.names()) + " (typechecking " + process::pp(x) + ")");
        }
      }
      if (multi_actions_contains(A.names(), MActs))
      {
        mCRL2log(log::warning) << "allowing (multi)action " << A.names() << " twice (typechecking " << x << ")" << std::endl;
      }
      else
      {
        MActs.push_front(A.names());
      }
    }
    return allow(x.allow_set(), (*this).apply(x.operand()));
  }

  process_expression apply(const process::at& x)
  {
    data::data_expression new_time = m_data_type_checker.typecheck_data_expression(x.time_stamp(), data::sort_real::real_(), m_variable_context);
    return at((*this).apply(x.operand()), new_time);
  }

  process_expression apply(const process::if_then& x)
  {
    data::data_expression condition = m_data_type_checker.typecheck_data_expression(x.condition(), data::sort_bool::bool_(), m_variable_context);
    return if_then(condition, (*this).apply(x.then_case()));
  }

  process_expression apply(const process::if_then_else& x)
  {
    data::data_expression condition = m_data_type_checker.typecheck_data_expression(x.condition(), data::sort_bool::bool_(), m_variable_context);
    return if_then_else(condition, (*this).apply(x.then_case()), (*this).apply(x.else_case()));
  }

  process_expression apply(const process::sum& x)
  {
    try
    {
      auto m_variable_context_copy = m_variable_context;
      m_variable_context.add_context_variables(x.variables(), m_data_type_checker);
      process_expression operand = (*this).apply(x.operand());
      m_variable_context = m_variable_context_copy;
      return sum(x.variables(), operand);
    }
    catch (mcrl2::runtime_error& e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + "\nwhile typechecking " + process::pp(x));
    }
  }

  process_expression apply(const process::stochastic_operator& x)
  {
    try
    {
      auto m_variable_context_copy = m_variable_context;
      m_variable_context.add_context_variables(x.variables(), m_data_type_checker);
      data::data_expression distribution = m_data_type_checker.typecheck_data_expression(x.distribution(), data::sort_real::real_(), m_variable_context);
      process_expression operand = (*this).apply(x.operand());
      m_variable_context = m_variable_context_copy;
      return stochastic_operator(x.variables(), distribution, operand);
    }
    catch (mcrl2::runtime_error& e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + "\nwhile typechecking " + process::pp(x));
    }
  }
};

inline
typecheck_builder make_typecheck_builder(
                    data::data_type_checker& data_typechecker,
                    const data::detail::variable_context& variables,
                    const detail::process_context& process_identifiers,
                    const detail::action_context& action_context
                   )
{
  return typecheck_builder(data_typechecker, variables, process_identifiers, action_context);
}

} // namespace detail

class process_type_checker
{
  protected:
    data::data_type_checker m_data_type_checker;
    detail::action_context m_action_context;
    detail::process_context m_process_context;
    data::detail::variable_context m_variable_context;

    std::vector<process_identifier> equation_identifiers(const std::vector<process_equation>& equations)
    {
      std::vector<process_identifier> result;
      for (const process_equation& eqn: equations)
      {
        result.push_back(eqn.identifier());
      }
      return result;
    }

  public:
    template <typename VariableContainer, typename ActionLabelContainer, typename ProcessIdentifierContainer>
    process_type_checker(const data::data_specification& dataspec,
                         const VariableContainer& variables,
                         const ActionLabelContainer& action_labels,
                         const ProcessIdentifierContainer& process_identifiers
                        )
      : m_data_type_checker(dataspec)
    {
      m_action_context.add_context_action_labels(action_labels, m_data_type_checker);
      m_variable_context.add_context_variables(variables, m_data_type_checker);
      m_process_context.add_process_identifiers(process_identifiers, m_action_context, m_data_type_checker);
    }

    /// \brief Default constructor
    process_type_checker(const data::data_specification& dataspec = data::data_specification())
      : m_data_type_checker(dataspec)
    {}

    /** \brief     Type check a process expression.
     *  Throws a mcrl2::runtime_error exception if the expression is not well typed.
     *  \param[in] d A process expression that has not been type checked.
     *  \return    a process expression where all untyped identifiers have been replace by typed ones.
     **/    /// \brief Typecheck the pbes pbesspec
    process_expression operator()(const process_expression& x)
    {
      return typecheck_process_expression(m_variable_context, process::normalize_sorts(x, m_data_type_checker.typechecked_data_specification()));
    }

    /// \brief Typecheck the process specification procspec
    void operator()(process_specification& procspec)
    {
      mCRL2log(log::verbose) << "type checking process specification..." << std::endl;

      // reset the context
      m_data_type_checker = data::data_type_checker(procspec.data());

      process::normalize_sorts(procspec, m_data_type_checker.typechecked_data_specification());

      m_action_context.clear();
      m_variable_context.clear();
      m_process_context.clear();
      m_action_context.add_context_action_labels(procspec.action_labels(), m_data_type_checker);
      m_variable_context.add_context_variables(procspec.global_variables(), m_data_type_checker);
      m_process_context.add_process_identifiers(equation_identifiers(procspec.equations()), m_action_context, m_data_type_checker);

      // typecheck the equations
      for (process_equation& eqn: procspec.equations())
      {
        data::detail::variable_context variable_context = m_variable_context;
        variable_context.add_context_variables(eqn.identifier().variables(), m_data_type_checker);
        eqn = process_equation(eqn.identifier(), eqn.formal_parameters(), typecheck_process_expression(variable_context, eqn.expression()));
      }

      // typecheck the initial state
      procspec.init() = typecheck_process_expression(m_variable_context, procspec.init());

      // typecheck the data specification
      procspec.data() = m_data_type_checker.typechecked_data_specification();

      mCRL2log(log::debug) << "type checking process specification finished" << std::endl;
    }

  protected:
    process_expression typecheck_process_expression(const data::detail::variable_context& variables, const process_expression& x)
    {
      return detail::make_typecheck_builder(m_data_type_checker, variables, m_process_context, m_action_context).apply(x);
    }
};

/** \brief     Type check a parsed mCRL2 process specification.
 *  Throws an exception if something went wrong.
 *  \param[in] proc_spec A process specification  that has not been type checked.
 *  \post      proc_spec is type checked.
 **/

inline
void type_check_process_specification(process_specification& proc_spec)
{
  process_type_checker type_checker;
  type_checker(proc_spec);
}

template <typename VariableContainer, typename ActionLabelContainer, typename ProcessIdentifierContainer>
process_expression type_check_process_expression(const process_expression& x,
                                                 const VariableContainer& variables = VariableContainer(),
                                                 const data::data_specification& dataspec = data::data_specification(),
                                                 const ActionLabelContainer& action_labels = ActionLabelContainer(),
                                                 const ProcessIdentifierContainer& process_identifiers = ProcessIdentifierContainer()
                                                )
{
  process_type_checker type_checker(dataspec, variables, action_labels, process_identifiers);
  return type_checker(x);
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_TYPECHECK_H
