// Author(s): Wieger Wesselink
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

  sort_expression UnwindType(const sort_expression& x)
  {
    return normalize_sorts(x, get_sort_specification());
  }

  sort_expression_list UnwindType(const sort_expression_list& x)
  {
    return data::detail::transform_aterm_list([&](const sort_expression& y) { return normalize_sorts(y, get_sort_specification()); }, x);
  }

  variable_list UnwindType(const variable_list& x)
  {
    return data::detail::transform_aterm_list([&](const variable& y)
      {
        data_expression z = normalize_sorts(y, get_sort_specification());
        return atermpp::down_cast<variable>(z);
      },
      x);
  }

  sort_expression UpCastNumericType(
                        sort_expression NeededType,
                        sort_expression Type,
                        data_expression &Par,
                        const std::map<core::identifier_string,sort_expression> &DeclaredVars,
                        const std::map<core::identifier_string,sort_expression> &AllowedVars,
                        std::map<core::identifier_string,sort_expression> &FreeVars,
                        const bool strictly_ambiguous,
                        bool warn_upcasting = false,
                        const bool print_cast_error = false)
  {
    return data::data_type_checker::UpCastNumericType(NeededType, Type, Par, DeclaredVars, AllowedVars, FreeVars, strictly_ambiguous, warn_upcasting, print_cast_error);
  }

  bool InTypesL(sort_expression_list Type, sorts_list sorts)
  {
    return data_type_checker::InTypesL(Type, sorts);
  }

  bool IsTypeAllowedL(const data::sort_expression_list &TypeList, const data::sort_expression_list PosTypeList)
  {
    return data_type_checker::IsTypeAllowedL(TypeList, PosTypeList);
  }

  sort_expression_list InsertType(const sort_expression_list TypeList, const sort_expression Type)
  {
    return data_type_checker::InsertType(TypeList, Type);
  }

  sort_expression ExpandNumTypesDown(sort_expression Type)
  {
    return data_type_checker::ExpandNumTypesDown(Type);
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
    return x1;
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
  for (const sort_expression_list& s: sorts2)
  {
    // if (InTypesL(s, sorts1))
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
bool IsNotInferredL(data::sort_expression_list TypeList)
{
  for (; !TypeList.empty(); TypeList=TypeList.tail())
  {
    data::sort_expression Type = TypeList.front();
    if (data::is_untyped_sort(Type) || data::is_untyped_possible_sorts(Type))
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

bool MActEq(core::identifier_string_list MAct1, core::identifier_string_list MAct2);

inline
bool MActIn(core::identifier_string_list MAct, action_name_multiset_list MActs)
{
  //returns true if MAct is in MActs
  for (auto i = MActs.begin(); i != MActs.end(); ++i)
  {
    if (MActEq(MAct, i->names()))
    {
      return true;
    }
  }
  return false;
}

inline
bool MActEq(core::identifier_string_list MAct1, core::identifier_string_list MAct2)
{
  //returns true if the two multiactions are equal.
  if (MAct1.size() != MAct2.size())
  {
    return false;
  }
  if (MAct1.empty())
  {
    return true;
  }
  core::identifier_string Act1 = MAct1.front();
  MAct1 = MAct1.tail();

  //remove Act1 once from MAct2. if not there -- return ATfalse.
  core::identifier_string_list NewMAct2;
  for (; !MAct2.empty(); MAct2=MAct2.tail())
  {
    core::identifier_string Act2=MAct2.front();
    if (Act1 == Act2)
    {
      MAct2 = atermpp::reverse(NewMAct2) + MAct2.tail();
      return MActEq(MAct1, MAct2);
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

  std::map<core::identifier_string, data::data_expression> assignment_map(const data::untyped_identifier_assignment_list& assignments)
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

  // Filter the parameters to contain only the equation sorts with parameters in this process call with assignments
  sorts_list filter_parameters(sorts_list parameter_lists, const std::map<core::identifier_string, data::data_expression>& assignments, const untyped_process_assignment& x)
  {
    sorts_list result;
    assert(!parameter_lists.empty());
    core::identifier_string culprit; // Variable used for more intelligible error messages.
    for (const data::sort_expression_list& parameters: parameter_lists)
    {
      // get the formal parameter names
      data::variable_list formal_parameters = utilities::detail::map_element(m_process_parameters, std::make_pair(x.name(), m_data_typechecker.UnwindType(parameters)));

      // we only need the names of the parameters, not the types
      core::identifier_string_list formal_parameter_names;
      for (const data::variable& v: formal_parameters)
      {
        formal_parameter_names.push_front(v.name());
      }

      core::identifier_string_list left_hand_sides;
      for (auto i = assignments.begin(); i != assignments.end(); ++i)
      {
        left_hand_sides.push_front(i->first);
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
    return result;
  }

  data::sort_expression_list GetNotInferredList(const sorts_list &sorts)
  {
    //we get: List of Lists of SortExpressions
    //Outer list: possible parameter types 0..nPosParsVectors-1
    //inner lists: parameter types vectors 0..nFormPars-1

    //we constuct 1 vector (list) of sort expressions (NotInferred if ambiguous)
    //0..nFormPars-1

    data::sort_expression_list Result;
    size_t nFormPars=(sorts.front()).size();
    std::vector<data::sort_expression_list> Pars(nFormPars);
    for (size_t i=0; i<nFormPars; i++)
    {
      Pars[i]=data::sort_expression_list();
    }

    for (data::sort_expression_list TypeList: sorts)
    {
      for (size_t i=0; i<nFormPars; TypeList = TypeList.tail(),i++)
      {
        Pars[i] = m_data_typechecker.InsertType(Pars[i],TypeList.front());
      }
    }

    for (size_t i=nFormPars; i>0; i--)
    {
      data::sort_expression Sort;
      if (Pars[i-1].size()==1)
      {
        Sort=Pars[i-1].front();
      }
      else
      {
        Sort = data::untyped_possible_sorts(data::sort_expression_list(atermpp::reverse(Pars[i-1])));
      }
      Result.push_front(Sort);
    }
    return Result;
  }

  std::pair<bool,data::sort_expression_list> AdjustNotInferredList(const data::sort_expression_list& PosTypeList, const sorts_list& sorts)
  {
    // PosTypeList -- List of Sortexpressions (possibly NotInferred(List Sortexpr))
    // sorts -- List of (Lists of sorts)
    // returns: PosTypeList, adjusted to the elements of sorts
    // NULL if cannot be ajusted.

    //if PosTypeList has only normal types -- check if it is in sorts,
    //if so return PosTypeList, otherwise return false.
    if (!IsNotInferredL(PosTypeList))
    {
      if (m_data_typechecker.InTypesL(PosTypeList,sorts))
      {
        return std::make_pair(true,PosTypeList);
      }
      else
      {
        return std::make_pair(false, data::sort_expression_list());
      }
    }

    //Filter sorts to contain only compatible with TypeList lists of parameters.
    sorts_list Newsorts;
    for (auto i=sorts.begin(); i!=sorts.end(); ++i)
    {
      data::sort_expression_list TypeList= *i;
      if (m_data_typechecker.IsTypeAllowedL(TypeList,PosTypeList))
      {
        Newsorts.push_front(TypeList);
      }
    }
    if (Newsorts.empty())
    {
      return std::make_pair(false, data::sort_expression_list());
    }
    if (Newsorts.size()==1)
    {
      return std::make_pair(true, Newsorts.front());
    }

    // otherwise return not inferred.
    return std::make_pair(true, GetNotInferredList(atermpp::reverse(Newsorts)));
  }

  process_expression MakeActionOrProc(bool is_action,
                                      const core::identifier_string& name,
                                      const data::sort_expression_list& formal_parameters,
                                      const data::data_expression_list FactParList
                                     )
  {
    if (is_action)
    {
      return action(action_label(name, formal_parameters), FactParList);
    }
    else
    {
      auto i = m_process_parameters.find(std::make_pair(name, m_data_typechecker.UnwindType(formal_parameters)));
      assert(i != m_process_parameters.end());
      const data::variable_list& FormalVars = i->second;
      return process_instance(process_identifier(name, FormalVars), FactParList);
    }
  }

  process_expression RewrActProc(const std::map<core::identifier_string,data::sort_expression>& Vars,
                                 const core::identifier_string& name,
                                 const data::data_expression_list& pars
                                )
  {
    process_expression Result;
    sorts_list ParList;

    bool action=false;

    auto j = m_actions.find(name);
    if (j != m_actions.end())
    {
      ParList = j->second;
      action = true;
    }
    else
    {
      auto j = m_equation_sorts.find(name);
      if ( j !=  m_equation_sorts.end())
      {
        ParList = j->second;
        action = false;
      }
      else
      {
        throw mcrl2::runtime_error("action or process " + core::pp(name) + " not declared");
      }
    }
    assert(!ParList.empty());

    size_t nFactPars = pars.size();
    const std::string msg = action? "action" : "process";

    //filter the list of lists ParList to keep only the lists of lenth nFactPars
    {
      atermpp::term_list <data::sort_expression_list> NewParList;
      for (; !ParList.empty(); ParList=ParList.tail())
      {
        data::sort_expression_list Par=ParList.front();
        if (Par.size()==nFactPars)
        {
          NewParList.push_front(Par);
        }
      }
      ParList=atermpp::reverse(NewParList);
    }

    if (ParList.empty())
    {
      throw mcrl2::runtime_error("no " + msg + " " + core::pp(name)
                      + " with " + atermpp::to_string(nFactPars) + " parameter" + ((nFactPars != 1)?"s":"")
                      + " is declared (while typechecking " + core::pp(name) + "(" + data::pp(pars) + "))");
    }

    if (ParList.size()==1)
    {
      Result = MakeActionOrProc(action, name, ParList.front(), pars);
    }
    else
    {
      // we need typechecking to find the correct type of the action.
      // make the list of possible types for the parameters
      Result = MakeActionOrProc(action, name, GetNotInferredList(ParList), pars);
    }

    //process the arguments

    //possible types for the arguments of the action. (not inferred if ambiguous action).
    data::sort_expression_list PosTypeList=is_action(Result)?
                  atermpp::down_cast<const process::action>(Result).label().sorts():
                  get_sorts(atermpp::down_cast<const process_instance>(Result).identifier().variables());
    data::data_expression_list NewPars;
    data::sort_expression_list NewPosTypeList;
    for (data::data_expression_list Pars=pars; !Pars.empty(); Pars=Pars.tail(),PosTypeList=PosTypeList.tail())
    {
      data::data_expression Par=Pars.front();
      data::sort_expression PosType=PosTypeList.front();

      data::sort_expression NewPosType;
      try
      {
        Par = m_data_typechecker(Par, PosType, Vars);
        NewPosType = Par.sort();
      }
      catch (mcrl2::runtime_error &e)
      {
        throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot typecheck " + data::pp(Par) + " as type " + data::pp(m_data_typechecker.ExpandNumTypesDown(PosType)) + " (while typechecking " + core::pp(name) + "(" + data::pp(pars) + "))");
      }
      NewPars.push_front(Par);
      NewPosTypeList.push_front(NewPosType);
    }
    NewPars=atermpp::reverse(NewPars);
    NewPosTypeList=atermpp::reverse(NewPosTypeList);

    std::pair<bool,data::sort_expression_list>p=AdjustNotInferredList(NewPosTypeList,ParList);
    PosTypeList=p.second;

    if (!p.first)
    {
      PosTypeList=is_action(Result)?
                       atermpp::down_cast<const process::action>(Result).label().sorts():
                       get_sorts(atermpp::down_cast<const process_instance>(Result).identifier().variables());
      data::data_expression_list Pars=NewPars;
      NewPars=data::data_expression_list();
      data::sort_expression_list CastedPosTypeList;
      for (; !Pars.empty(); Pars=Pars.tail(),PosTypeList=PosTypeList.tail(),NewPosTypeList=NewPosTypeList.tail())
      {
        data::data_expression Par=Pars.front();
        data::sort_expression PosType=PosTypeList.front();
        data::sort_expression NewPosType=NewPosTypeList.front();

        data::sort_expression CastedNewPosType;
        try
        {
          std::map<core::identifier_string,data::sort_expression> dummy_table;
          CastedNewPosType=m_data_typechecker.UpCastNumericType(PosType,NewPosType,Par,Vars,Vars,dummy_table,false);
        }
        catch (mcrl2::runtime_error &e)
        {
          throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot cast " + data::pp(NewPosType) + " to " + data::pp(PosType) + "(while typechecking " + data::pp(Par) + " in " +
                     core::pp(name) + "(" + data::pp(pars) + ")");
        }

        NewPars.push_front(Par);
        CastedPosTypeList.push_front(CastedNewPosType);
      }
      NewPars=atermpp::reverse(NewPars);
      NewPosTypeList=atermpp::reverse(CastedPosTypeList);

      std::pair<bool,data::sort_expression_list>p=AdjustNotInferredList(NewPosTypeList,ParList);
      PosTypeList=p.second;

      if (!p.first)
      {
        throw mcrl2::runtime_error("no " + msg + " " + core::pp(name) + "with type " + data::pp(NewPosTypeList) + " is declared (while typechecking " +
                core::pp(name) + "(" + data::pp(pars) + "))");
      }
    }

    if (IsNotInferredL(PosTypeList))
    {
      throw mcrl2::runtime_error("ambiguous " + msg + " " + core::pp(name));
    }
    return MakeActionOrProc(action, name, PosTypeList, NewPars);
  }

  process_expression apply(const untyped_process_assignment& x)
  {
    mCRL2log(log::debug) << "typechecking a process call with short-hand assignments " << x << "" << std::endl;

    auto j = m_equation_sorts.find(x.name());
    if (j == m_equation_sorts.end())
    {
      throw mcrl2::runtime_error("process " + core::pp(x.name()) + " not declared");
    }
    sorts_list ParList = j->second;

    std::map<core::identifier_string, data::data_expression> As = assignment_map(x.assignments());

    ParList = filter_parameters(ParList, As, x);

    // get the formal parameter names
    data::data_expression_list ActualPars;
    const data::variable_list& FormalPars = utilities::detail::map_element(m_process_parameters, std::make_pair(x.name(), m_data_typechecker.UnwindType(ParList.front())));
    {
      // we only need the names of the parameters, not the types
      for (const data::variable& par: FormalPars)
      {
        const core::identifier_string& FormalParName=par.name();
        data::data_expression ActualPar;
        auto i = As.find(FormalParName);
        if (i == As.end())
        {
          ActualPar = data::untyped_identifier(FormalParName);
        }
        else
        {
          ActualPar = i->second;
        }
        ActualPars.push_front(ActualPar);
      }
      ActualPars = atermpp::reverse(ActualPars);
    }

    process_expression TypeCheckedProcTerm;
    try
    {
      TypeCheckedProcTerm = RewrActProc(m_variables, x.name(), ActualPars);
    }
    catch (mcrl2::runtime_error &e)
    {
      throw mcrl2::runtime_error(std::string(e.what()) + "\ntype error occurred while typechecking the process call with short-hand assignments " + process::pp(x));
    }

    //atermpp::reverse the assignments
    As.clear();
    std::map <core::identifier_string,data::assignment> As_new;
    data::variable_list m = FormalPars;
    data::data_expression_list l = atermpp::down_cast<const process_instance>(TypeCheckedProcTerm).actual_parameters();
    for ( ; !l.empty(); l=l.tail(),m=m.tail())
    {
      const data::data_expression act_par = l.front();
      const data::variable form_par = m.front();
      As_new[form_par.name()] = data::assignment(form_par, act_par);
    }

    data::assignment_list TypedAssignments;
    for (const data::untyped_identifier_assignment& a: x.assignments())
    {
      auto i = As_new.find(a.lhs());
      if (i == As_new.end())
      {
        continue;
      }
      TypedAssignments.push_front(i->second);
    }
    TypedAssignments = atermpp::reverse(TypedAssignments);

    return process_instance_assignment(atermpp::down_cast<const process_instance>(TypeCheckedProcTerm).identifier(), TypedAssignments);
  }

  process_expression apply(const untyped_parameter_identifier& x)
  {
    process_expression result = RewrActProc(m_variables, x.name(), x.arguments());
    return result;
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
      if (MActIn(A.names(), MActs))
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
    data::data_expression_typechecker m_data_type_checker;
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
        // m_data_type_checker.check_sort_is_declared(v.sort());
        m_data_type_checker.check_sort_is_declared(v.sort());

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

        m_data_type_checker.check_sort_list_is_declared(ActType);

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

          if (m_data_type_checker.InTypesL(ActType, sorts))
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
        m_data_type_checker.check_sort_list_is_declared(ProcType);

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
          if (m_data_type_checker.InTypesL(ProcType, sorts))
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
        if (!m_data_type_checker.VarsUnique(id.variables()))
        {
          throw mcrl2::runtime_error("the formal variables in process " + process::pp(id) + " are not unique");
        }

        std::pair<core::identifier_string, data::sort_expression_list> p(id.name(), m_data_type_checker.UnwindType(get_sorts(id.variables())));
        m_process_parameters[p] = m_data_type_checker.UnwindType(id.variables());
      }
      std::pair<core::identifier_string, data::sort_expression_list> p(core::identifier_string("init"), data::sort_expression_list());
      m_process_parameters[p] = data::variable_list();
    }

  public:
//    process_type_checker(const data::data_specification& dataspec, const std::vector<action_label>& action_labels, const std::vector<data::variable>& global_variables, const std::vector<process_identifier>& process_identifiers)
//      : m_data_type_checker(dataspec)
//    {
//      add_action_labels(action_labels);
//      add_global_variables(global_variables);
//      add_process_identifiers(process_identifiers);
//    }

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
      return typecheck_process_expression(m_global_variables, x);
    }

    /// \brief Typecheck the process specification procspec
    void operator()(process_specification& procspec)
    {
      mCRL2log(log::verbose) << "type checking process specification..." << std::endl;

      // reset the context
      m_data_type_checker = data::data_expression_typechecker(procspec.data());

      normalize_sorts(procspec, m_data_type_checker.typechecked_data_specification());

      m_actions.clear();
      m_global_variables.clear();
      m_equation_sorts.clear();
      add_action_labels(procspec.action_labels());
      add_global_variables(procspec.global_variables());
      add_process_identifiers(equation_identifiers(procspec.equations()));


      // typecheck the equations
      for (process_equation& eqn: procspec.equations())
      {
        eqn = process_equation(eqn.identifier(), eqn.formal_parameters(), typecheck_process_expression(declared_variables(m_data_type_checker.UnwindType(eqn.identifier().variables())), eqn.expression()));
      }

      // typecheck the initial state
      procspec.init() = typecheck_process_expression(m_global_variables, procspec.init());

      // typecheck the data specification
      procspec.data() = m_data_type_checker.typechecked_data_specification();

      mCRL2log(log::debug) << "type checking process specification finished" << std::endl;
    }

  protected:
    process_expression typecheck_process_expression(const std::map<core::identifier_string, data::sort_expression>& variables, const process_expression& x)
    {
      return detail::make_typecheck_builder(m_data_type_checker, variables, m_equation_sorts, m_actions, m_process_parameters).apply(x);
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
