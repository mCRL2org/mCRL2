// Author(s): Jan Friso Groote
//            Wieger Wesselink 2015 -
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
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
#include "mcrl2/process/detail/match_action_parameters.h"
#include "mcrl2/process/detail/process_context.h"
#include "mcrl2/process/normalize_sorts.h"

namespace mcrl2::process
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
  std::set<data::sort_expression_list> parameter_list = action_context.matching_action_sorts(name, parameters);
  std::pair<data::data_expression_list, data::sort_expression_list> p = 
                process::detail::match_action_parameters(parameters, parameter_list, variable_context, name, msg, typechecker);
  return action(action_label(name, p.second), p.first);
}

// returns the difference of the 2 type list lists
inline
std::set<data::sort_expression_list> sorts_list_difference(const std::set<data::sort_expression_list>& sorts1, const std::set<data::sort_expression_list>& sorts2)
{
  std::set<data::sort_expression_list> result;
  std::set_difference(sorts1.begin(),sorts1.end(), sorts2.begin(),sorts2.end(),std::inserter(result, result.begin()));
  return result;
}

// print a sorts_list, i.e. a list of lists of sort expressions.
inline
std::string pp(const std::set<data::sort_expression_list>& ll)
{
  std::string result;
  bool empty1=true;
  for(const data::sort_expression_list& l: ll)
  {
    bool empty2=true;
    for(const data::sort_expression& s: l)
    {
      result=result+(empty2?"":"#")+pp(s);
      empty2=false;
    }
    result=result+(empty1?"":", ");
    empty2=false;
  }
  return result;
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

struct typecheck_builder: public process_expression_builder<typecheck_builder>
{
  using super = process_expression_builder<typecheck_builder>;
  using super::apply;

  data::data_type_checker& m_data_type_checker;
  data::detail::variable_context m_variable_context;
  const detail::process_context& m_process_context;
  const detail::action_context& m_action_context;
  const process_identifier* m_current_equation;

  typecheck_builder(data::data_type_checker& data_typechecker,
                    const data::detail::variable_context& variable_context,
                    const detail::process_context& process_context,
                    const detail::action_context& action_context,
                    const process_identifier* current_equation = nullptr
                   )
    : m_data_type_checker(data_typechecker),
      m_variable_context(variable_context),
      m_process_context(process_context),
      m_action_context(action_context),
      m_current_equation(current_equation)
  {}

  std::set<data::sort_expression_list> action_sorts(const core::identifier_string& name)
  {
    return m_action_context.matching_action_sorts(name);
  }

  void check_action_declared(const core::identifier_string& a, const process_expression& x)
  {
    if (!m_action_context.is_declared(a))
    {
      throw mcrl2::runtime_error("Undefined action " + core::pp(a) + " (typechecking " + core::pp(x) + ").");
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
        mCRL2log(log::warning) << "Used action " << a << " twice (typechecking " << x << ").`" << std::endl;
      }
    }
  }

  template <typename Container>
  void check_not_empty(const Container& c, const std::string& msg, const process_expression& x)
  {
    if (c.empty())
    {
      mCRL2log(log::warning) << msg << " (typechecking " << x << ")." << std::endl;
    }
  }

  template <typename T>
  void check_not_equal(const T& first, const T& second, const std::string& msg, const process_expression& x)
  {
    if (first == second)
    {
      mCRL2log(log::warning) << msg << " " << first << "(typechecking " << x << ")." << std::endl;
    }
  }

  static bool has_empty_intersection(const std::set<data::sort_expression_list>& s1, const std::set<data::sort_expression_list>& s2)
  {
    std::set<data::sort_expression_list> v1(s1.begin(), s1.end());
    std::set<data::sort_expression_list> v2(s2.begin(), s2.end());
    return utilities::detail::has_empty_intersection(v1, v2);
  }

  void check_rename_common_type(const core::identifier_string& a, const core::identifier_string& b, const process_expression& x)
  {
    if (has_empty_intersection(action_sorts(a), action_sorts(b)))
    {
      throw mcrl2::runtime_error("Renaming action " + core::pp(a) + " into action " + core::pp(b) + ": these two have no common type (typechecking " + process::pp(x) + ").");
    }
  }

  void check_duplicates_in_assignments(const data::untyped_identifier_assignment_list& assignments)
  {
    std::map<core::identifier_string, data::data_expression> result;
    for (const data::untyped_identifier_assignment& a: assignments)
    {
      auto i = result.find(a.lhs());
      if (i != result.end()) // An data::assignment of the shape x := t already exists, this is not OK.
      {
        throw mcrl2::runtime_error("Double data::assignment to data::variable " + core::pp(a.lhs()) + " (detected assigned values are " + data::pp(i->second) + " and " + core::pp(a.rhs()) + ").");
      }
      result[a.lhs()]=a.rhs();
    }
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
    std::set<data::sort_expression_list> parameter_list = m_process_context.matching_process_sorts(name, parameters);
    std::pair<data::data_expression_list, data::sort_expression_list> p = 
            process::detail::match_action_parameters(parameters, parameter_list, m_variable_context, name, msg, m_data_type_checker);
    return m_process_context.make_process_instance(name, p.second, p.first);
  }

  std::string print_untyped_process_assignment(const untyped_process_assignment& x) const
  {
    if (x.assignments().empty())
    {
      return core::pp(x.name()) + "()";
    }
    else
    {
      return core::pp(x.name()) + core::detail::print_arguments(x.assignments());
    }
  }

  // Checks if in the equation P(..) = Q(assignments) all formal parameters of Q have been assigned a value,
  // either directly via an assignment in assignments, or indirectly via a formal parameter of P.
  void check_assignments(const process_identifier& P, const process_identifier& Q, const std::vector<data::assignment>& assignments, const untyped_process_assignment& x) const
  {
    std::set<data::variable> Q_variables;
    for (const data::assignment& a: assignments)
    {
      Q_variables.insert(a.lhs());
    }
    for (const data::variable& v: P.variables())
    {
      Q_variables.insert(v);
    }
    for (const data::variable& v: Q.variables())
    {
      if (Q_variables.find(v) == Q_variables.end())
      {
        throw mcrl2::runtime_error("Process parameter " + core::pp(v.name()) + " has not been assigned a value in the process call with short-hand assignments " + print_untyped_process_assignment(x) + ".");
      }
    }
  }

  template <class T>
  void apply(T& result, const untyped_process_assignment& x)
  {
    mCRL2log(log::debug) << "typechecking a process call with short-hand assignments " << x << "" << std::endl;
    if (!is_process_name(x.name()))
    {
      throw mcrl2::runtime_error("Could not find a matching declaration for action or process expression " + print_untyped_process_assignment(x) + ".");
    }

    process_identifier P = m_process_context.match_untyped_process_instance_assignment(x);
    const data::variable_list& formal_parameters = P.variables();

    // This checks for duplicate left hand sides.
    check_duplicates_in_assignments(x.assignments());

    std::map<core::identifier_string, data::variable> formal_parameter_map;
    for (const data::variable& v: formal_parameters)
    {
      formal_parameter_map[v.name()] = v;
    }

    // Typecheck the right hand sides of the assignments
    std::vector<data::assignment> assignments;
    for (const data::untyped_identifier_assignment& a: x.assignments())
    {
      try
      {
        data::variable v = formal_parameter_map[a.lhs()];
        formal_parameter_map.erase(v.name());
        data::data_expression e = m_data_type_checker.typecheck_data_expression(a.rhs(), v.sort(), m_variable_context);
        assignments.emplace_back(v, e);
      }
      catch (const mcrl2::runtime_error& e)
      {
        throw mcrl2::runtime_error(std::string(e.what()) + "\nType error occurred while typechecking the process call with short-hand assignments " + process::pp(x));
      }
    }

    // Check if all formal parameters have been assigned a value
    if (m_current_equation)
    {
      check_assignments(*m_current_equation, P, assignments, x);
    }

    // Check that for any non explicitly stated assignment x:=x, the rhs is well defined.
    for (const auto& [name, var]: formal_parameter_map)
    {
      try
      {
        m_data_type_checker.typecheck_data_expression(var, var.sort(), m_variable_context);
      }
      catch (const mcrl2::runtime_error& e)
      {
        throw mcrl2::runtime_error(std::string(e.what()) + "\nIn implicitly stated assignment " + pp(var) + "=" + pp(var) + 
                                   " in process " + pp(P) + "(" + data::pp(assignments) + ")\n");
      }
    }
    // Take care that the assignments occur in the same order as the variables in the process identifier.
    std::map<data::variable, data::data_expression> assignment_map;
    for(const data::assignment& a: assignments)
    {
      assignment_map[a.lhs()]=a.rhs();
    }

    std::vector<data::assignment> sorted_assignments;
    for(const data::variable& v: formal_parameters)
    {
      const  std::map<data::variable, data::data_expression>::const_iterator i=assignment_map.find(v);
      if (i!=assignment_map.end())
      {
        assert(v.sort()==i->second.sort());
        sorted_assignments.emplace_back(v,i->second);
      }
    }

    make_process_instance_assignment(result, P, data::assignment_list(sorted_assignments.begin(), sorted_assignments.end()));
  }

  template <class T>
  void apply(T& result, const data::untyped_data_parameter& x)
  {
    if (is_action_name(x.name()))
    {
      result = typecheck_action(x.name(), x.arguments());
    }
    else if (is_process_name(x.name()))
    {
      result = typecheck_process_instance(x.name(), x.arguments());
    }
    else
    {
      throw mcrl2::runtime_error("Could not find a matching declaration for action or process expression " + 
                                 core::pp(x.name()) + core::detail::print_arguments(x.arguments()) + ".");
    }
  }

  // This case is necessary for actionrename
  template <class T>
  void apply(T& result, const process::action& x)
  {
    result = typecheck_action(x.label().name(), x.arguments());
  }

  template <class T>
  void apply(T& result, const process::hide& x)
  {
    check_not_empty(x.hide_set(), "Hiding empty set of actions", x);
    check_actions_declared(x.hide_set(), x);
    process::make_hide(result, 
                       x.hide_set(), 
                       [&](process_expression& result){ (*this).apply(result, x.operand()); } );
  }

  template <class T>
  void apply(T& result, const process::block& x)
  {
    check_not_empty(x.block_set(), "Blocking empty set of actions", x);
    check_actions_declared(x.block_set(), x);
    make_block(result, x.block_set(), [&]( process_expression& result){ (*this).apply(result, x.operand()); });
  }

  template <class T>
  void apply(T& result, const process::rename& x)
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
        throw mcrl2::runtime_error("Renaming action " + core::pp(r.source()) + " twice (typechecking " + process::pp(x) + ").");
      }
    }
    make_rename(result, x.rename_set(), [&](process_expression& r){ (*this).apply(r, x.operand()); } );
  }

  //comm: like renaming multiactions (with the same parameters) to action/tau
  template <class T>
  void apply(T& result, const process::comm& x)
  {
    check_not_empty(x.comm_set(), "Synchronizing empty set of (multi)actions", x);

    std::multiset<core::identifier_string> left_hand_side_actions;
    for (const communication_expression& c: x.comm_set())
    {
      const core::identifier_string_list& cnames = c.action_name().names();
      assert(!cnames.empty());

      if (cnames.size() == 1)
      {
        throw mcrl2::runtime_error("Using synchronization as renaming/hiding of action " + core::pp(cnames.front()) + " into " + core::pp(c.name()) + " (typechecking " + process::pp(x) + ").");
      }

      //Actions must be declared
      if (!m_action_context.is_declared(c.name()))
      {
        throw mcrl2::runtime_error("Synchronizing to an undefined action " + core::pp(c.name()) + " (typechecking " + process::pp(x) + ").");
      }

      //Below we check the first part of requirement 7 on page 279 of Groote/Mousave, The MIT Press, 2014.
      std::set<data::sort_expression_list> c_sorts;
      bool c_sorts_defined=false;
      for (const core::identifier_string& a: cnames)
      {
        if (!m_action_context.is_declared(a))
        {
          throw mcrl2::runtime_error("Synchronizing an undefined action " + core::pp(a) + " in (multi)action " + core::pp(cnames) + " (typechecking " + process::pp(x) + ").");
        }
        if (c_sorts_defined)
        {
          // c_sorts = sorts_list_union(c_sorts, action_sorts(a));
          std::set<data::sort_expression_list> s=action_sorts(a);
          c_sorts.insert(s.begin(), s.end());
        }
        else
        {
          c_sorts=action_sorts(a);
          c_sorts_defined=true;
        }
        if (c_sorts.empty())
        {
          throw mcrl2::runtime_error("Synchronizing action " + core::pp(a) + " from (multi)action " + process::pp(c) +
                            " into action " + core::pp(c.name()) + ": these have no common type (typechecking " + process::pp(x) + ").");
        }
      }
      //Check that each sort occurring in an action at the lhs is also a sort of the action at the right.
      const std::set<data::sort_expression_list> target_sorts = action_sorts(c.name());
      const std::set<data::sort_expression_list> difference_list=sorts_list_difference(c_sorts,target_sorts);
      if (difference_list.size()>0)
      {
        throw mcrl2::runtime_error("In the communication clause " + process::pp(c) + 
                  " the action at the right does not have type" + 
                  (difference_list.size()==1?" " + pp(difference_list) + " which is a type of some action at the left."
                                            :"s " + pp(difference_list) + " which are types of actions at the left.")); 

      }
      //Check that each sort occurring in an action at the lhs is also a sort of each action at the left, 
      //in conformance with Groote/Mousavi, page 279, item 7.
      for (const core::identifier_string& a: cnames)
      {
        const std::set<data::sort_expression_list> target_sorts = action_sorts(a);
        const std::set<data::sort_expression_list> difference_list=sorts_list_difference(c_sorts,target_sorts);
        if (difference_list.size()>0)
        {
          throw mcrl2::runtime_error("In the communication clause " + process::pp(c) + 
                    " the action " + pp(a) + " in the lhs does not have type" + 
                    (difference_list.size()==1?" " + pp(difference_list) + " which is a type of some action at the left."
                                              :"s " + pp(difference_list) + " which are types of actions at the left.")); 

        }
      }

      //the multiactions in the lhss of comm should not intersect.
      //but there can be multiple of the same actions in cnames. 
      std::set < core::identifier_string > this_left_hand_sides;
      for (const core::identifier_string& a: cnames)
      {
        if (this_left_hand_sides.count(a)==0 && left_hand_side_actions.find(a) != left_hand_side_actions.end())
        {
          throw mcrl2::runtime_error("Synchronizing action " + core::pp(a) + " in different ways (typechecking " + process::pp(x) + ").");
        }
        else
        {
          left_hand_side_actions.insert(a);
          this_left_hand_sides.insert(a);
        }
      }
    }

    //the multiactions in the rhs of comm should not intersect with an action in the a lhs of
    //all other rules. So, {a|a->a} is allowed, but {a|b->c, c|a->d} is not. 
    for (const communication_expression& c: x.comm_set())
    {
      // Count how many actions in the rhs occur at the left
      std::size_t number_of_rhs_in_lhs=0;
      for (const core::identifier_string& lhs_action: c.action_name().names())
      { 
        if (lhs_action==c.name())
        {
          number_of_rhs_in_lhs++;
        }
      }
      assert(left_hand_side_actions.count(c.name())>=number_of_rhs_in_lhs);
      if (left_hand_side_actions.count(c.name())-number_of_rhs_in_lhs>0) // There are more actions x.name() in the lhss than just in this lhs.
      {
        throw mcrl2::runtime_error("Action " + core::pp(c.name()) + 
                     " occurs at the left and the right in a communication (typechecking " + process::pp(x) + ").");
      }
    }
    make_comm(result, x.comm_set(), [&](process_expression& r){ (*this).apply(r, x.operand()); } );
  }

  template <class T>
  void apply(T& result, const process::allow& x)
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
          throw mcrl2::runtime_error("Allowing an undefined action " + core::pp(a) + " in (multi)action " + core::pp(A.names()) + " (typechecking " + process::pp(x) + ").");
        }
      }
      if (multi_actions_contains(A.names(), MActs))
      {
        mCRL2log(log::warning) << "allowing (multi)action " << A.names() << " twice (typechecking " << x << ")" << std::endl;
      }
      else
      {
        MActs.push_front(mcrl2::process::action_name_multiset(A.names()));
      }
    }
    make_allow(result, x.allow_set(), [&](process_expression& r){ (*this).apply(r, x.operand()); } );
  }

  template <class T>
  void apply(T& result, const process::at& x)
  {
    data::data_expression new_time = m_data_type_checker.typecheck_data_expression(x.time_stamp(), data::sort_real::real_(), m_variable_context);
    make_at(result, [&](process_expression& r){ (*this).apply(r, x.operand()); }, new_time);
  }

  template <class T>
  void apply(T& result, const process::if_then& x)
  {
    data::data_expression condition = m_data_type_checker.typecheck_data_expression(x.condition(), data::sort_bool::bool_(), m_variable_context);
    make_if_then(result, condition, [&](process_expression& r){ (*this).apply(r, x.then_case()); } );
  }

  template <class T>
  void apply(T& result, const process::if_then_else& x)
  {
    data::data_expression condition = m_data_type_checker.typecheck_data_expression(x.condition(), data::sort_bool::bool_(), m_variable_context);
    make_if_then_else(result, 
                      condition, 
                      [&](process_expression& r){ (*this).apply(r, x.then_case()); }, 
                      [&](process_expression& r){ (*this).apply(r, x.else_case()); } );
  }

  template <class T>
  void apply(T& result, const process::sum& x)
  {
    try
    {
      auto m_variable_context_copy = m_variable_context;
      m_variable_context.add_context_variables(x.variables(), m_data_type_checker);
      process_expression operand;
      (*this).apply(operand, x.operand());
      m_variable_context = m_variable_context_copy;
      make_sum(result, x.variables(), operand);
    }
    catch (mcrl2::runtime_error& e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + "\nwhile typechecking " + process::pp(x) + ".");
    }
  }

  template <class T>
  void apply(T& result, const process::stochastic_operator& x)
  {
    try
    {
      auto m_variable_context_copy = m_variable_context;
      m_variable_context.add_context_variables(x.variables(), m_data_type_checker);
      data::data_expression distribution = m_data_type_checker.typecheck_data_expression(x.distribution(), data::sort_real::real_(), m_variable_context);
      process_expression operand;
      (*this).apply(operand, x.operand());
      m_variable_context = m_variable_context_copy;
      make_stochastic_operator(result, x.variables(), distribution, operand);
    }
    catch (mcrl2::runtime_error& e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + "\nwhile typechecking " + process::pp(x) + ".");
    }
  }
};

inline
typecheck_builder make_typecheck_builder(
                    data::data_type_checker& data_typechecker,
                    const data::detail::variable_context& variables,
                    const detail::process_context& process_identifiers,
                    const detail::action_context& action_context,
                    const process_identifier* current_equation = nullptr
                   )
{
  return typecheck_builder(data_typechecker, variables, process_identifiers, action_context, current_equation);
}

} // namespace detail

class process_type_checker
{
  protected:
    data::data_type_checker m_data_type_checker;
    detail::action_context m_action_context;
    detail::process_context m_process_context;
    data::detail::variable_context m_variable_context;

    static std::vector<process_identifier> equation_identifiers(const std::vector<process_equation>& equations)
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
    explicit process_type_checker(const data::data_specification& dataspec = data::data_specification())
      : m_data_type_checker(dataspec)
    {}

    /** \brief     Type check a process expression.
     *  Throws a mcrl2::runtime_error exception if the expression is not well typed.
     *  \param[in] x A process expression that has not been type checked.
     *  \return    a process expression where all untyped identifiers have been replace by typed ones.
     **/    /// \brief Typecheck the pbes pbesspec
    process_expression operator()(const process_expression& x, const process_identifier* current_equation = nullptr)
    {
      return typecheck_process_expression(m_variable_context, process::normalize_sorts(x, m_data_type_checker.typechecked_data_specification()), current_equation);
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
        eqn = process_equation(eqn.identifier(), eqn.formal_parameters(), typecheck_process_expression(variable_context, eqn.expression(), &eqn.identifier()));
      }

      // typecheck the initial state
      procspec.init() = typecheck_process_expression(m_variable_context, procspec.init());

      // typecheck the data specification
      procspec.data() = m_data_type_checker.typechecked_data_specification();
      procspec.data().translate_user_notation();
      

      mCRL2log(log::debug) << "type checking process specification finished" << std::endl;
    }

  protected:
    process_expression typecheck_process_expression(const data::detail::variable_context& variables, const process_expression& x, const process_identifier* current_equation = nullptr)
    {
      process_expression result;
      detail::make_typecheck_builder(m_data_type_checker, variables, m_process_context, m_action_context, current_equation).apply(result, x);
      return result;
    }
};

/** \brief     Type check a parsed mCRL2 process specification.
 *  Throws an exception if something went wrong.
 *  \param[in] proc_spec A process specification  that has not been type checked.
 *  \post      proc_spec is type checked.
 **/

inline
void typecheck_process_specification(process_specification& proc_spec)
{
  process_type_checker type_checker;
  type_checker(proc_spec);
}

/// \brief Typecheck a process expression
/// \param x An untyped process expression
/// \param variables A sequence of data variables
/// \param dataspec A data specification
/// \param action_labels A sequence of action labels
/// \param process_identifiers A sequence of process identifiers
/// \param current_equation A pointer to the current equation. If this pointer is set, a check will be done it
/// process instance assignments assign values to all their parameters.
template <typename VariableContainer, typename ActionLabelContainer, typename ProcessIdentifierContainer>
process_expression typecheck_process_expression(const process_expression& x,
                                                const VariableContainer& variables = VariableContainer(),
                                                const data::data_specification& dataspec = data::data_specification(),
                                                const ActionLabelContainer& action_labels = ActionLabelContainer(),
                                                const ProcessIdentifierContainer& process_identifiers = ProcessIdentifierContainer(),
                                                const process_identifier* current_equation = nullptr
                                               )
{
  process_type_checker type_checker(dataspec, variables, action_labels, process_identifiers);
  return type_checker(x, current_equation);
}

} // namespace mcrl2::process

#endif // MCRL2_PROCESS_TYPECHECK_H
