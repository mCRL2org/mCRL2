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
#include "mcrl2/process/process_specification.h"
#include "mcrl2/utilities/detail/container_utility.h"

namespace mcrl2
{

namespace data
{

typedef atermpp::term_list<data::sort_expression_list> sorts_list;

namespace detail
{

template <typename Function, typename T>
atermpp::term_list<T> transform_aterm_list(const Function& f, const atermpp::term_list<T>& x)
{
  atermpp::term_list<T> result;
  for (T t: x)
  {
    result.push_front(f(t));
  }
  return atermpp::reverse(result);
}

} // namespace detail

struct data_expression_typechecker: protected data::data_type_checker
{
  /** \brief     make a data type checker.
   *  Throws a mcrl2::runtime_error exception if the data_specification is not well typed.
   *  \param[in] data_spec A data specification that does not need to have been type checked.
   *  \return    a data expression where all untyped identifiers have been replace by typed ones.
   **/
  data_expression_typechecker(const data_specification& dataspec)
    : data_type_checker(dataspec)
  {}

  void check_sort_list_is_declared(const sort_expression_list& x)
  {
    return sort_type_checker::check_sort_list_is_declared(x);
  }

  void check_sort_is_declared(const sort_expression& x)
  {
    return sort_type_checker::check_sort_is_declared(x);
  }

  bool VarsUnique(const variable_list& VarDecls)
  {
    return data_type_checker::VarsUnique(VarDecls);
  }

  sort_expression normalize_sorts(const sort_expression& x)
  {
    return data::normalize_sorts(x, get_sort_specification());
  }

  data_expression UpCastNumericType(const data_expression& Par, const sort_expression& NeededType, const std::map<core::identifier_string,sort_expression>& variables)
  {
    std::map<core::identifier_string,data::sort_expression> dummy_table;
    data_expression Par1 = Par;
    sort_expression s = data::data_type_checker::UpCastNumericType(NeededType, Par.sort(), Par1, variables, variables, dummy_table, false, false, false);
    assert(s == Par1.sort());
    return data::normalize_sorts(Par1, get_sort_specification());
  }

  // returns true if s1 and s2 are equal after normalization
  bool equal_sorts(const sort_expression& s1, const sort_expression& s2)
  {
    if (s1 == s2)
    {
      return true;
    }
    return normalize_sorts(s1) == normalize_sorts(s2);
  }

  // returns true if s matches with an element of sorts after normalization
  bool match_sort(const sort_expression& s, const sort_expression_list& sorts)
  {
    for (const sort_expression& s1: sorts)
    {
      if (equal_sorts(s, s1))
      {
        return true;
      }
    }
    return false;
  }

  // returns true if s is allowed by allowed_sort, meaning that allowed_sort is an untyped sort,
  // or allowed_sort is a sequence that contains a matching sort
  bool is_allowed_sort(const sort_expression& sort, const sort_expression& allowed_sort)
  {
    if (is_untyped_sort(data::sort_expression(allowed_sort)))
    {
      return true;
    }
    if (is_untyped_possible_sorts(allowed_sort))
    {
      return match_sort(sort, atermpp::down_cast<const untyped_possible_sorts>(allowed_sort).sorts());
    }

    //PosType is a normal type
    return equal_sorts(sort, allowed_sort);
  }

  // returns true if all elements of sorts are allowed by the corresponding entries of allowed_sorts
  bool is_allowed_sort_list(const sort_expression_list& sorts, const sort_expression_list& allowed_sorts)
  {
    assert(sorts.size() == allowed_sorts.size());
    auto j = allowed_sorts.begin();
    for (auto i = sorts.begin(); i != sorts.end(); ++i,++j)
    {
      if (!is_allowed_sort(*i, *j))
      {
        return false;
      }
    }
    return true;
  }

  sort_expression_list insert_type(const sort_expression_list TypeList, const sort_expression Type)
  {
    for (sort_expression_list OldTypeList = TypeList; !OldTypeList.empty(); OldTypeList = OldTypeList.tail())
    {
      if (equal_sorts(OldTypeList.front(), Type))
      {
        return TypeList;
      }
    }
    sort_expression_list result = TypeList;
    result.push_front(Type);
    return result;
  }

  bool equal_sort_lists(const sort_expression_list& x1, const sort_expression_list& x2)
  {
    if (x1 == x2)
    {
      return true;
    }
    if (x1.size() != x2.size())
    {
      return false;
    }
    return std::equal(x1.begin(), x1.end(), x2.begin(), [&](const sort_expression& s1, const sort_expression& s2) { return equal_sorts(s1, s2); });
  }

  // returns true if l is (after unwinding) contained in sorts
  bool is_contained_in(const sort_expression_list& l, const sorts_list& sorts)
  {
    for (const sort_expression_list& m: sorts)
    {
      if (equal_sort_lists(l, m))
      {
        return true;
      }
    }
    return false;
  }

  sort_expression ExpandNumTypesDown(const sort_expression& x)
  {
    return data_type_checker::ExpandNumTypesDown(x);
  }

  /** \brief     Type check a data expression.
   *  Throws a mcrl2::runtime_error exception if the expression is not well typed.
   *  \param[in] x A data expression that has not been type checked.
   *  \param[in] expected_sort The expected sort of the data expression.
   *  \param[in] variable_constext a mapping of variable names to their types.
   *  \return the type checked data expression.
   **/
  data_expression operator()(const data_expression& x,
                             const sort_expression& expected_sort,
                             const std::map<core::identifier_string, sort_expression>& variable_context
                            )
  {
    data_expression x1 = x;
    TraverseVarConsTypeD(variable_context, variable_context, x1, expected_sort);
    return data::normalize_sorts(x1, get_sort_specification());
  }

  data_specification typechecked_data_specification()
  {
    return type_checked_data_spec;
  }
};

} // namespace data

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
bool contains_untyped_sorts(const data::sort_expression_list& sorts)
{
  for (const data::sort_expression& s: sorts)
  {
    if (data::is_untyped_sort(s) || data::is_untyped_possible_sorts(s))
    {
      return true;
    }
  }
  return false;
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

  data::data_expression_typechecker& m_data_typechecker;
  std::map<core::identifier_string, data::sort_expression> m_variables;
  const std::map<core::identifier_string, sorts_list>& m_equation_sorts;
  const std::map<core::identifier_string, sorts_list>& m_actions;
  const std::map<std::pair<core::identifier_string, data::sort_expression_list>, data::variable_list>& m_process_parameters;

  typecheck_builder(data::data_expression_typechecker& data_typechecker,
                    const std::map<core::identifier_string, data::sort_expression>& variables,
                    const std::map<core::identifier_string, data::sorts_list>& equation_sorts,
                    const std::map<core::identifier_string, sorts_list>& actions,
                    const std::map<std::pair<core::identifier_string,data::sort_expression_list>,data::variable_list>& process_parameters
                   )
    : m_data_typechecker(data_typechecker),
      m_variables(variables),
      m_equation_sorts(equation_sorts),
      m_actions(actions),
      m_process_parameters(process_parameters)
  {}

  const sorts_list& action_sorts(const core::identifier_string& x)
  {
    auto i = m_actions.find(x);
    assert(i != m_actions.end());
    return i->second;
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

  // returns the equation sorts that corresponds to the untyped process assignment x
  data::variable_list matching_process_parameters(const untyped_process_assignment& x)
  {
    auto j = m_equation_sorts.find(x.name());
    if (j == m_equation_sorts.end())
    {
      throw mcrl2::runtime_error("process " + core::pp(x.name()) + " not declared");
    }
    sorts_list parameter_lists = j->second;
    assert(!parameter_lists.empty());

    sorts_list result;
    core::identifier_string culprit; // Variable used for more intelligible error messages.
    for (const data::sort_expression_list& parameters: parameter_lists)
    {
      // get the formal parameter names
      data::variable_list formal_parameters = utilities::detail::map_element(m_process_parameters, std::make_pair(x.name(), parameters));

      // we only need the names of the parameters, not the types
      core::identifier_string_list formal_parameter_names;
      for (const data::variable& v: formal_parameters)
      {
        formal_parameter_names.push_front(v.name());
      }

      core::identifier_string_list left_hand_sides;
      for (const data::untyped_identifier_assignment& a: x.assignments())
      {
        left_hand_sides.push_front(a.lhs());
      }
      core::identifier_string_list l = list_difference(left_hand_sides, formal_parameter_names);
      if (l.empty())
      {
        result.push_front(parameters);
      }
      else
      {
        culprit = l.front();
      }
    }
    result = atermpp::reverse(result);

    if (parameter_lists.empty())
    {
      throw mcrl2::runtime_error("no process " + core::pp(x.name()) + " containing all assignments in " + process::pp(x) + ".\n" + "Problematic data::variable is " + core::pp(culprit) + ".");
    }
    if (!parameter_lists.tail().empty())
    {
      throw mcrl2::runtime_error("ambiguous process " + core::pp(x.name()) + " containing all assignments in " + process::pp(x) + ".");
    }
    assert(result.size() == 1);
    const data::variable_list& formal_parameters = utilities::detail::map_element(m_process_parameters, std::make_pair(x.name(), result.front()));
    return formal_parameters;
  }

  //we get: List of Lists of SortExpressions
  //Outer list: possible parameter types 0..nPosParsVectors-1
  //inner lists: parameter types vectors 0..n-1
  //we constuct 1 vector (list) of sort expressions (NotInferred if ambiguous)
  //0..n-1
  data::sort_expression_list GetNotInferredList(const sorts_list& sorts)
  {
    if (sorts.size() == 1)
    {
      return sorts.front();
    }
    data::sort_expression_list result;
    size_t n = sorts.front().size();
    std::vector<data::sort_expression_list> parameter_lists(n, data::sort_expression_list());

    for (data::sort_expression_list s: sorts)
    {
      for (size_t i = 0; i < n; s = s.tail(), i++)
      {
        parameter_lists[i] = m_data_typechecker.insert_type(parameter_lists[i], s.front());
      }
    }

    for (size_t i = n; i > 0; i--)
    {
      data::sort_expression sort;
      if (parameter_lists[i - 1].size() == 1)
      {
        sort = parameter_lists[i - 1].front();
      }
      else
      {
        sort = data::untyped_possible_sorts(data::sort_expression_list(atermpp::reverse(parameter_lists[i - 1])));
      }
      result.push_front(sort);
    }
    return result;
  }

  std::pair<bool, data::sort_expression_list> AdjustNotInferredList(const data::sort_expression_list& possible_sorts, const sorts_list& sorts)
  {
    // possible_sorts -- List of Sortexpressions (possibly NotInferred(List Sortexpr))
    // sorts -- List of (Lists of sorts)
    // returns: possible_sorts, adjusted to the elements of sorts
    // NULL if cannot be ajusted.

    //if possible_sorts has only normal types -- check if it is in sorts,
    //if so return possible_sorts, otherwise return false.
    if (!contains_untyped_sorts(possible_sorts))
    {
      if (m_data_typechecker.is_contained_in(possible_sorts,sorts))
      {
        return std::make_pair(true, possible_sorts);
      }
      else
      {
        return std::make_pair(false, data::sort_expression_list());
      }
    }

    //Filter sorts to contain only compatible with TypeList lists of parameters.
    sorts_list new_sorts;
    for (const data::sort_expression_list& s: sorts)
    {
      if (m_data_typechecker.is_allowed_sort_list(s, possible_sorts))
      {
        new_sorts.push_front(s);
      }
    }
    if (new_sorts.empty())
    {
      return std::make_pair(false, data::sort_expression_list());
    }
    if (new_sorts.size() == 1)
    {
      return std::make_pair(true, new_sorts.front());
    }

    // otherwise return not inferred.
    return std::make_pair(true, GetNotInferredList(atermpp::reverse(new_sorts)));
  }

  sorts_list filter_parameters_on_size(sorts_list parameter_list, const data::data_expression_list& parameters, const core::identifier_string& name, const std::string& msg)
  {
    sorts_list result;
    for (const data::sort_expression_list& sorts: parameter_list)
    {
      if (sorts.size() == parameters.size())
      {
        result.push_front(sorts);
      }
    }
    result = atermpp::reverse(result);

    if (result.empty())
    {
      throw mcrl2::runtime_error("no " + msg + " " + core::pp(name)
                      + " with " + atermpp::to_string(parameters.size()) + " parameter" + ((parameters.size() != 1)?"s":"")
                      + " is declared (while typechecking " + core::pp(name) + "(" + data::pp(parameters) + "))");
    }
    return result;
  }

  action make_action(const core::identifier_string& name, const data::sort_expression_list& formal_parameters, const data::data_expression_list& actual_parameters)
  {
    return action(action_label(name, formal_parameters), actual_parameters);
  }

  process_instance make_process_instance(const core::identifier_string& name, const data::sort_expression_list& formal_parameters, const data::data_expression_list& actual_parameters)
  {
    auto i = m_process_parameters.find(std::make_pair(name, formal_parameters));
    assert(i != m_process_parameters.end());
    return process_instance(process_identifier(name, i->second), actual_parameters);
  }

  bool is_action_name(const core::identifier_string& name)
  {
    return m_actions.find(name) != m_actions.end();
  }

  bool is_process_name(const core::identifier_string& name)
  {
    return m_equation_sorts.find(name) != m_equation_sorts.end();
  }

  action typecheck_action(const core::identifier_string& name, const data::data_expression_list& parameters)
  {
    auto j = m_actions.find(name);
    assert(j != m_actions.end());
    sorts_list parameter_list = j->second;
    assert(!parameter_list.empty());
    std::string msg = "action";
    parameter_list = filter_parameters_on_size(parameter_list, parameters, name, msg);
    if (parameter_list.empty())
    {
      throw mcrl2::runtime_error("no " + msg + " " + core::pp(name)
                      + " with " + atermpp::to_string(parameters.size()) + " parameter" + ((parameters.size() != 1)?"s":"")
                      + " is declared (while typechecking " + core::pp(name) + "(" + data::pp(parameters) + "))");
    }
    action Result = make_action(name, GetNotInferredList(parameter_list), parameters);
    auto p = match_parameters(parameters, Result.label().sorts(), parameter_list, m_variables, name, msg);
    return make_action(name, p.second, p.first);
  }

  process_instance typecheck_process_instance(const core::identifier_string& name, const data::data_expression_list& parameters)
  {
    sorts_list parameter_list;
    auto j = m_equation_sorts.find(name);
    assert(j !=  m_equation_sorts.end());
    parameter_list = j->second;
    assert(!parameter_list.empty());
    std::string msg = "process";
    parameter_list = filter_parameters_on_size(parameter_list, parameters, name, msg);
    if (parameter_list.empty())
    {
      throw mcrl2::runtime_error("no " + msg + " " + core::pp(name)
                      + " with " + atermpp::to_string(parameters.size()) + " parameter" + ((parameters.size() != 1)?"s":"")
                      + " is declared (while typechecking " + core::pp(name) + "(" + data::pp(parameters) + "))");
    }
    process_instance Result = make_process_instance(name, GetNotInferredList(parameter_list), parameters);
    auto p = match_parameters(parameters, get_sorts(Result.identifier().variables()), parameter_list, m_variables, name, msg);
    return make_process_instance(name, p.second, p.first);
  }

  data::data_expression typecheck_data_expression(const data::data_expression& d, const data::sort_expression& expected_sort, const std::map<core::identifier_string, data::sort_expression>& variables, const core::identifier_string& name, const data::data_expression_list& parameters)
  {
    try
    {
      return m_data_typechecker(d, expected_sort, variables);
    }
    catch (mcrl2::runtime_error& e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot typecheck " + data::pp(d) + " as type " + data::pp(m_data_typechecker.ExpandNumTypesDown(expected_sort)) + " (while typechecking " + core::pp(name) + "(" + data::pp(parameters) + "))");
    }
  }

  data::data_expression upcast_numeric_type(const data::data_expression& d, const data::sort_expression& expected_sort, const std::map<core::identifier_string, data::sort_expression>& variables, const core::identifier_string& name, const data::data_expression_list& parameters)
  {
    try
    {
      return m_data_typechecker.UpCastNumericType(d, expected_sort, variables);
    }
    catch (mcrl2::runtime_error& e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot typecheck " + data::pp(d) + " as type " + data::pp(m_data_typechecker.ExpandNumTypesDown(expected_sort)) + " (while typechecking " + core::pp(name) + "(" + data::pp(parameters) + "))");
    }
  }

  std::pair<data::data_expression_list, data::sort_expression_list> match_parameters(const data::data_expression_list& parameters,
                                                                                     const data::sort_expression_list& expected_sorts,
                                                                                     const sorts_list& parameter_list,
                                                                                     const std::map<core::identifier_string, data::sort_expression>& variables,
                                                                                     const core::identifier_string& name,
                                                                                     const std::string& msg
                                                                                    )
  {
    data::sort_expression_list possible_sorts = expected_sorts;
    data::data_expression_vector new_parameters(parameters.begin(), parameters.end());
    auto p1 = new_parameters.begin();
    auto p2 = possible_sorts.begin();
    for (; p1 != new_parameters.end(); ++p1, ++p2)
    {
      data::data_expression& e = *p1;
      const data::sort_expression& expected_sort = *p2;
      e = typecheck_data_expression(e, expected_sort, variables, name, parameters);
    }

    std::pair<bool, data::sort_expression_list> p = AdjustNotInferredList(parameter_sorts(new_parameters), parameter_list);
    possible_sorts = p.second;

    if (!p.first)
    {
      possible_sorts = expected_sorts;
      auto q1 = new_parameters.begin();
      auto q2 = possible_sorts.begin();
      for (; q1 != new_parameters.end(); ++q1, ++q2)
      {
        data::data_expression& e = *q1;
        data::sort_expression expected_sort = *q2;
        e = upcast_numeric_type(e, expected_sort, variables, name, parameters);
      }

      std::pair<bool, data::sort_expression_list> p = AdjustNotInferredList(parameter_sorts(new_parameters), parameter_list);
      possible_sorts = p.second;

      if (!p.first)
      {
        throw mcrl2::runtime_error("no " + msg + " " + core::pp(name) + "with type " + data::pp(parameter_sorts(new_parameters)) + " is declared (while typechecking " + core::pp(name) + "(" + data::pp(parameters) + "))");
      }
    }
    if (contains_untyped_sorts(possible_sorts))
    {
      throw mcrl2::runtime_error("ambiguous " + msg + " " + core::pp(name));
    }
    return std::make_pair(data::data_expression_list(new_parameters.begin(), new_parameters.end()), possible_sorts);
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
        c_sorts = j->second;
      }

      for (const core::identifier_string& a: cnames)
      {
        auto j = m_actions.find(a);
        if (j == m_actions.end())
        {
          throw mcrl2::runtime_error("synchronizing an undefined action " + core::pp(a) + " in (multi)action " + core::pp(cnames) + " (typechecking " + process::pp(x) + ")");
        }
        c_sorts = sorts_list_intersection(c_sorts, j->second);
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
                    data::data_expression_typechecker& data_typechecker,
                    const std::map<core::identifier_string, data::sort_expression>& variables,
                    const std::map<core::identifier_string, data::sorts_list>& equation_sorts,
                    const std::map<core::identifier_string, sorts_list>& actions,
                    const std::map<std::pair<core::identifier_string,data::sort_expression_list>,data::variable_list>& process_parameters
                   )
{
  return typecheck_builder(data_typechecker, variables, equation_sorts, actions, process_parameters);
}

} // namespace detail

class process_type_checker
{
  protected:
    data::data_expression_typechecker m_data_typechecker;
    std::map<core::identifier_string, sorts_list> m_actions;
    std::map<core::identifier_string, sorts_list> m_equation_sorts;
    std::map<core::identifier_string, data::sort_expression> m_global_variables;
    std::map<std::pair<core::identifier_string, data::sort_expression_list>, data::variable_list> m_process_parameters;

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

    void add_action_labels(const process::action_label_list& actions)
    {
      for (const action_label& a: actions)
      {
        core::identifier_string ActName = a.name();
        data::sort_expression_list ActType = a.sorts();

        m_data_typechecker.check_sort_list_is_declared(ActType);

        auto j = m_actions.find(ActName);
        sorts_list sorts;
        if (j==m_actions.end())
        {
          sorts = { ActType };
        }
        else
        {
          sorts = j->second;
          // the table actions contains a list of types for each
          // action name. We need to check if there is already such a type
          // in the list. If so -- error, otherwise -- add

          if (m_data_typechecker.is_contained_in(ActType, sorts))
          {
            throw mcrl2::runtime_error("double declaration of action " + core::pp(ActName));
          }
          else
          {
            sorts = sorts + sorts_list({ ActType });
          }
        }
        m_actions[ActName] = sorts;
      }
    }

    void add_process_identifiers(const std::vector<process_identifier>& ids)
    {
      for (const process_identifier& id: ids)
      {
        const core::identifier_string& name = id.name();

        if (m_actions.count(name) > 0)
        {
          throw mcrl2::runtime_error("declaration of both process and action " + std::string(name));
        }

        data::sort_expression_list ProcType = get_sorts(id.variables());
        m_data_typechecker.check_sort_list_is_declared(ProcType);

        auto j = m_equation_sorts.find(name);
        sorts_list sorts;
        if (j == m_equation_sorts.end())
        {
          sorts = sorts_list({ ProcType });
        }
        else
        {
          sorts = j->second;
          // the table m_equation_sorts contains a list of types for each
          // process name. We need to check if there is already such a type
          // in the list. If so -- error, otherwise -- add
          if (m_data_typechecker.is_contained_in(ProcType, sorts))
          {
            throw mcrl2::runtime_error("double declaration of process " + std::string(name));
          }
          else
          {
            sorts = sorts + sorts_list({ ProcType });
          }
        }
        m_equation_sorts[name] = sorts;

        //check that all formal parameters of the process are unique.
        if (!m_data_typechecker.VarsUnique(id.variables()))
        {
          throw mcrl2::runtime_error("the formal variables in process " + process::pp(id) + " are not unique");
        }

        std::pair<core::identifier_string, data::sort_expression_list> p(id.name(), get_sorts(id.variables()));
        m_process_parameters[p] = id.variables();
      }
      std::pair<core::identifier_string, data::sort_expression_list> p(core::identifier_string("init"), data::sort_expression_list());
      m_process_parameters[p] = data::variable_list();
    }

  public:
//    process_type_checker(const data::data_specification& dataspec, const std::vector<action_label>& action_labels, const std::vector<data::variable>& global_variables, const std::vector<process_identifier>& process_identifiers)
//      : m_data_typechecker(dataspec)
//    {
//      add_action_labels(action_labels);
//      add_global_variables(global_variables);
//      add_process_identifiers(process_identifiers);
//    }

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
      return typecheck_process_expression(m_global_variables, x);
    }

    /// \brief Typecheck the process specification procspec
    void operator()(process_specification& procspec)
    {
      mCRL2log(log::verbose) << "type checking process specification..." << std::endl;

      // reset the context
      m_data_typechecker = data::data_expression_typechecker(procspec.data());

      normalize_sorts(procspec, m_data_typechecker.typechecked_data_specification());

      m_actions.clear();
      m_global_variables.clear();
      m_equation_sorts.clear();
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
      return detail::make_typecheck_builder(m_data_typechecker, variables, m_equation_sorts, m_actions, m_process_parameters).apply(x);
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

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_TYPECHECK_H
