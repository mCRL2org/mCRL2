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
#include "mcrl2/data/detail/data_typechecker.h"
#include "mcrl2/process/builder.h"
#include "mcrl2/process/normalize_sorts.h"
#include "mcrl2/process/process_specification.h"
#include "mcrl2/utilities/detail/container_utility.h"

namespace mcrl2
{

namespace process
{

typedef atermpp::term_list<data::sort_expression_list> sorts_list;

// returns the intersection of the 2 type list lists
inline
sorts_list sorts_list_intersection(const sorts_list& sorts1, const sorts_list& sorts2)
{
  sorts_list result;
  for (const data::sort_expression_list& s: sorts2)
  {
    if (std::find(sorts1.begin(), sorts1.end(), s) != sorts1.end())
    {
      result.push_front(s);
    }
  }
  return atermpp::reverse(result);
}

template <class T>
data::sort_expression_list get_sorts(const atermpp::term_list<T>& l)
{
  std::vector<data::sort_expression> v;
  for (auto i = l.begin(); i != l.end(); ++i)
  {
    v.push_back(i->sort());
  }
  return data::sort_expression_list(v.begin(),v.end());
}

inline
std::ostream& operator<<(std::ostream& out, const sorts_list& x)
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
  for (auto i = l.begin(); i != l.end(); ++i)
  {
    if (std::find(m.begin(), m.end(),*i) == m.end())
    {
      n.push_front(*i);
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
  for (auto i = A.begin(); i != A.end(); ++i)
  {
    if (equal_multi_actions(a, i->names()))
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

  data::detail::data_typechecker& m_data_typechecker;
  std::map<core::identifier_string, data::sort_expression> m_variables;
  const std::multimap<core::identifier_string, process_identifier>& m_process_identifiers;
  const std::multimap<core::identifier_string, action_label>& m_actions;

  typecheck_builder(data::detail::data_typechecker& data_typechecker,
                    const std::map<core::identifier_string, data::sort_expression>& variables,
                    const std::multimap<core::identifier_string, process_identifier>& process_identifiers,
                    const std::multimap<core::identifier_string, action_label>& actions
                   )
    : m_data_typechecker(data_typechecker),
      m_variables(variables),
      m_process_identifiers(process_identifiers),
      m_actions(actions)
  {}

  sorts_list action_sorts(const core::identifier_string& name)
  {
    auto range = m_actions.equal_range(name);
    assert(range.first != m_actions.end());
    sorts_list result;
    for (auto k = range.first; k != range.second; ++k)
    {
      const action_label& a = k->second;
      result.push_front(a.sorts());
    }
    return atermpp::reverse(result);
  }

  template <typename Container>
  data::sort_expression_list parameter_sorts(const Container& parameters)
  {
    data::sort_expression_list sorts;
    for (const data::data_expression& e: parameters)
    {
      sorts.push_front(e.sort());
    }
    return atermpp::reverse(sorts);
  }

  void check_action_declared(const core::identifier_string& a, const process_expression& x)
  {
    if (m_actions.find(a) == m_actions.end())
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

  bool has_empty_intersection(const sorts_list& s1, const sorts_list& s2)
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

  // returns true if all left hand sides of assignments appear as the name of a variable in parameters
  bool is_matching_assignment(const data::untyped_identifier_assignment_list& assignments, const data::variable_list& parameters)
  {
    for (const data::untyped_identifier_assignment& a: assignments)
    {
      if (std::find_if(parameters.begin(), parameters.end(), [&](const data::variable& v) { return a.lhs() == v.name(); }) == parameters.end())
      {
        return false;
      }
    }
    return true;
  }

  // returns the equation sorts that corresponds to the untyped process assignment x
  data::variable_list matching_process_parameters(const untyped_process_assignment& x)
  {
    auto range = m_process_identifiers.equal_range(x.name());
    if (range.first == m_process_identifiers.end())
    {
      throw mcrl2::runtime_error("process " + core::pp(x.name()) + " not declared");
    }
    std::vector<process_identifier> result;
    for (auto k = range.first; k != range.second; ++k)
    {
      const process_identifier& id = k->second;
      if (is_matching_assignment(x.assignments(), id.variables()))
      {
        result.push_back(id);
      }
    }
    if (result.empty())
    {
      throw mcrl2::runtime_error("no process " + core::pp(x.name()) + " containing all assignments in " + process::pp(x) + ".\n");
    }
    if (result.size() > 1)
    {
      throw mcrl2::runtime_error("ambiguous process " + core::pp(x.name()) + " containing all assignments in " + process::pp(x) + ".");
    }
    return result.front().variables();
  }

  action make_action(const core::identifier_string& name, const data::sort_expression_list& formal_parameters, const data::data_expression_list& actual_parameters)
  {
    return action(action_label(name, formal_parameters), actual_parameters);
  }

  process_instance make_process_instance(const core::identifier_string& name, const data::sort_expression_list& formal_parameters, const data::data_expression_list& actual_parameters)
  {
    auto range = m_process_identifiers.equal_range(name);
    assert(range.first != m_process_identifiers.end());
    for (auto k = range.first; k != range.second; ++k)
    {
      const process_identifier& id = k->second;
      if (parameter_sorts(id.variables()) == formal_parameters)
      {
        return process_instance(id, actual_parameters);
      }
    }
    throw mcrl2::runtime_error("no matching process found for " + core::pp(name) + "(" + data::pp(formal_parameters) + ")");
  }

  bool is_action_name(const core::identifier_string& name)
  {
    return m_actions.find(name) != m_actions.end();
  }

  bool is_process_name(const core::identifier_string& name)
  {
    return m_process_identifiers.find(name) != m_process_identifiers.end();
  }

  sorts_list matching_action_sorts(const core::identifier_string& name, const data::data_expression_list& parameters)
  {
    sorts_list result;
    auto range = m_actions.equal_range(name);
    for (auto k = range.first; k != range.second; ++k)
    {
      const action_label& a = k->second;
      if (a.sorts().size() == parameters.size())
      {
        result.push_front(a.sorts());
      }
    }
    return atermpp::reverse(result);
  }

  action typecheck_action(const core::identifier_string& name, const data::data_expression_list& parameters)
  {
    std::string msg = "action";
    sorts_list parameter_list = matching_action_sorts(name, parameters);
    if (parameter_list.empty())
    {
      throw mcrl2::runtime_error("no " + msg + " " + core::pp(name)
                      + " with " + atermpp::to_string(parameters.size()) + " parameter" + ((parameters.size() != 1)?"s":"")
                      + " is declared (while typechecking " + core::pp(name) + "(" + data::pp(parameters) + "))");
    }
    action result = make_action(name, m_data_typechecker.GetNotInferredList(parameter_list), parameters);
    auto p = m_data_typechecker.match_parameters(parameters, result.label().sorts(), parameter_list, m_variables, name, msg);
    return make_action(name, p.second, p.first);
  }

  sorts_list matching_process_sorts(const core::identifier_string& name, const data::data_expression_list& parameters)
  {
    sorts_list result;
    auto range = m_process_identifiers.equal_range(name);
    for (auto k = range.first; k != range.second; ++k)
    {
      const process_identifier& id = k->second;
      if (id.variables().size() == parameters.size())
      {
        result.push_front(parameter_sorts(id.variables()));
      }
    }
    return atermpp::reverse(result);
  }

  process_instance typecheck_process_instance(const core::identifier_string& name, const data::data_expression_list& parameters)
  {
    std::string msg = "process";
    sorts_list parameter_list = matching_process_sorts(name, parameters);
    if (parameter_list.empty())
    {
      throw mcrl2::runtime_error("no " + msg + " " + core::pp(name)
                      + " with " + atermpp::to_string(parameters.size()) + " parameter" + ((parameters.size() != 1)?"s":"")
                      + " is declared (while typechecking " + core::pp(name) + "(" + data::pp(parameters) + "))");
    }
    process_instance Result = make_process_instance(name, m_data_typechecker.GetNotInferredList(parameter_list), parameters);
    auto p = m_data_typechecker.match_parameters(parameters, get_sorts(Result.identifier().variables()), parameter_list, m_variables, name, msg);
    return make_process_instance(name, p.second, p.first);
  }

  process_expression apply(const untyped_process_assignment& x)
  {
    mCRL2log(log::debug) << "typechecking a process call with short-hand assignments " << x << "" << std::endl;
    if (!is_process_name(x.name()))
    {
      throw mcrl2::runtime_error("action or process " + core::pp(x.name()) + " not declared" + "\ntype error occurred while typechecking the process call with short-hand assignments " + process::pp(x));
    }

    data::variable_list formal_parameters = matching_process_parameters(x);
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

  process_expression apply(const untyped_parameter_identifier& x)
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
      throw mcrl2::runtime_error("action or process " + core::pp(x.name()) + " not declared");
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
      sorts_list c_sorts;
      if (!core::is_nil(c.name()))
      {
        auto j = m_actions.find(c.name());
        if (j == m_actions.end())
        {
          throw mcrl2::runtime_error("synchronizing to an undefined action " + core::pp(c.name()) + " (typechecking " + process::pp(x) + ")");
        }
        c_sorts = action_sorts(c.name());
      }

      for (const core::identifier_string& a: cnames)
      {
        auto j = m_actions.find(a);
        if (j == m_actions.end())
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
        if (m_actions.count(a) == 0)
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
    data::data_expression new_time = m_data_typechecker(x.time_stamp(), data::sort_real::real_(), m_variables);
    return at((*this).apply(x.operand()), new_time);
  }

  process_expression apply(const process::if_then& x)
  {
    data::data_expression condition = m_data_typechecker(x.condition(), data::sort_bool::bool_(), m_variables);
    return if_then(condition, (*this).apply(x.then_case()));
  }

  process_expression apply(const process::if_then_else& x)
  {
    data::data_expression condition = m_data_typechecker(x.condition(), data::sort_bool::bool_(), m_variables);
    return if_then_else(condition, (*this).apply(x.then_case()), (*this).apply(x.else_case()));
  }

  process_expression apply(const process::sum& x)
  {
    try
    {
      auto m_variables_copy = m_variables;
      for (const data::variable& v: x.variables())
      {
        m_variables[v.name()] = v.sort();
      }
      process_expression operand = (*this).apply(x.operand());
      m_variables = m_variables_copy;
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
      auto m_variables_copy = m_variables;
      for (const data::variable& v: x.variables())
      {
        m_variables[v.name()] = v.sort();
      }
      data::data_expression distribution = m_data_typechecker(x.distribution(), data::sort_real::real_(), m_variables);
      process_expression operand = (*this).apply(x.operand());
      m_variables = m_variables_copy;
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
                    data::detail::data_typechecker& data_typechecker,
                    const std::map<core::identifier_string, data::sort_expression>& variables,
                    const std::multimap<core::identifier_string, process_identifier>& process_identifiers,
                    const std::multimap<core::identifier_string, action_label>& actions
                   )
{
  return typecheck_builder(data_typechecker, variables, process_identifiers, actions);
}

} // namespace detail

class process_type_checker
{
  protected:
    data::detail::data_typechecker m_data_typechecker;
    std::multimap<core::identifier_string, action_label> m_actions;
    std::multimap<core::identifier_string, process_identifier> m_process_identifiers;
    std::map<core::identifier_string, data::sort_expression> m_global_variables;

    std::vector<process_identifier> equation_identifiers(const std::vector<process_equation>& equations)
    {
      std::vector<process_identifier> result;
      for (const process_equation& eqn: equations)
      {
        result.push_back(eqn.identifier());
      }
      return result;
    }

    // Returns m_global_variables with variables inserted into it
    std::map<core::identifier_string, data::sort_expression> declared_variables(const data::variable_list& variables)
    {
      std::map<core::identifier_string, data::sort_expression> result = m_global_variables;
      for (const data::variable& v: variables)
      {
        result[v.name()] = v.sort();
      }
      return result;
    }

    template <typename VariableContainer>
    void add_global_variables(const VariableContainer& global_variables)
    {
      for (const data::variable& v: global_variables)
      {
        // m_data_typechecker.check_sort_is_declared(v.sort());
        m_data_typechecker.check_sort_is_declared(v.sort());

        auto i = m_global_variables.find(v.name());
        if (i == m_global_variables.end())
        {
          m_global_variables[v.name()] = v.sort();
        }
        else
        {
          throw mcrl2::runtime_error("attempt to overload global variable " + core::pp(v.name()));
        }
      }
    }

    template <typename ActionLabelContainer>
    void add_action_labels(const ActionLabelContainer& actions)
    {
      for (const action_label& a: actions)
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

    template <typename ProcessIdentifierContainer>
    void add_process_identifiers(const ProcessIdentifierContainer& ids)
    {
      for (const process_identifier& id: ids)
      {
        const core::identifier_string& name = id.name();

        if (m_actions.find(name) != m_actions.end())
        {
          throw mcrl2::runtime_error("declaration of both process and action " + core::pp(name));
        }

        // Insert id in m_process_identifiers; N.B. Before that check if it already exists
        auto range = m_process_identifiers.equal_range(id.name());
        if (range.first != m_process_identifiers.end())
        {
          for (auto i = range.first; i != range.second; ++i)
          {
            if (i->second == id)
            {
              throw mcrl2::runtime_error("double declaration of process " + process::pp(id));
            }
          }
        }
        m_process_identifiers.insert(range.first, std::make_pair(id.name(), id));

        data::sort_expression_list ProcType = get_sorts(id.variables());
        m_data_typechecker.check_sort_list_is_declared(ProcType);

        //check that all formal parameters of the process are unique.
        if (!m_data_typechecker.VarsUnique(id.variables()))
        {
          throw mcrl2::runtime_error("the formal variables in process " + process::pp(id) + " are not unique");
        }
      }
    }

  public:
    template <typename VariableContainer, typename ActionLabelContainer, typename ProcessIdentifierContainer>
    process_type_checker(const data::data_specification& dataspec,
                         const VariableContainer& variables,
                         const ActionLabelContainer& action_labels,
                         const ProcessIdentifierContainer& process_identifiers
                        )
      : m_data_typechecker(dataspec)
    {
      add_action_labels(action_labels);
      add_global_variables(variables);
      add_process_identifiers(process_identifiers);
    }

    /// \brief Default constructor
    process_type_checker(const data::data_specification& dataspec = data::data_specification())
      : m_data_typechecker(dataspec)
    {}

    /** \brief     Type check a process expression.
     *  Throws a mcrl2::runtime_error exception if the expression is not well typed.
     *  \param[in] d A process expression that has not been type checked.
     *  \return    a process expression where all untyped identifiers have been replace by typed ones.
     **/    /// \brief Typecheck the pbes pbesspec
    process_expression operator()(const process_expression& x)
    {
      return typecheck_process_expression(m_global_variables, normalize_sorts(x, m_data_typechecker.typechecked_data_specification()));
    }

    /// \brief Typecheck the process specification procspec
    void operator()(process_specification& procspec)
    {
      mCRL2log(log::verbose) << "type checking process specification..." << std::endl;

      // reset the context
      m_data_typechecker = data::detail::data_typechecker(procspec.data());

      normalize_sorts(procspec, m_data_typechecker.typechecked_data_specification());

      m_actions.clear();
      m_global_variables.clear();
      m_process_identifiers.clear();
      add_action_labels(procspec.action_labels());
      add_global_variables(procspec.global_variables());
      add_process_identifiers(equation_identifiers(procspec.equations()));

      // typecheck the equations
      for (process_equation& eqn: procspec.equations())
      {
        eqn = process_equation(eqn.identifier(), eqn.formal_parameters(), typecheck_process_expression(declared_variables(eqn.identifier().variables()), eqn.expression()));
      }

      // typecheck the initial state
      procspec.init() = typecheck_process_expression(m_global_variables, procspec.init());

      // typecheck the data specification
      procspec.data() = m_data_typechecker.typechecked_data_specification();

      mCRL2log(log::debug) << "type checking process specification finished" << std::endl;
    }

  protected:
    process_expression typecheck_process_expression(const std::map<core::identifier_string, data::sort_expression>& variables, const process_expression& x)
    {
      return detail::make_typecheck_builder(m_data_typechecker, variables, m_process_identifiers, m_actions).apply(x);
    }
};

/** \brief     Type check a parsed mCRL2 process specification.
 *  Throws an exception if something went wrong.
 *  \param[in] proc_spec A process specification  that has not been type checked.
 *  \post      proc_spec is type checked.
 **/

inline
void type_check(process_specification& proc_spec)
{
  process_type_checker type_checker;
  type_checker(proc_spec);
}

template <typename VariableContainer, typename ActionLabelContainer, typename ProcessIdentifierContainer>
process_expression type_check(process_expression& x,
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
