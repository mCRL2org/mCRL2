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
    return data_type_checker::check_sort_is_declared(x);
  }

  const std::map<sort_expression, sort_expression>& aliases() const
  {
    return get_sort_specification().sort_alias_map();
  }

  sort_expression unwind_sort_expression(const sort_expression& x)
  {
    if (is_container_sort(x))
    {
      const container_sort& cs = atermpp::down_cast<const container_sort>(x);
      return container_sort(cs.container_name(), unwind_sort_expression(cs.element_sort()));
    }
    else if (is_function_sort(x))
    {
      const function_sort& fs = atermpp::down_cast<function_sort>(x);
      auto new_arguments = detail::transform_aterm_list([&](const sort_expression& s) { return unwind_sort_expression(s); }, fs.domain());
      return function_sort(new_arguments, unwind_sort_expression(fs.codomain()));
    }
    else if (is_basic_sort(x))
    {
      const basic_sort& bs = atermpp::down_cast<const basic_sort>(x);
      auto i = get_sort_specification().sort_alias_map().find(bs);
      if (i == get_sort_specification().sort_alias_map().end())
      {
        return x;
      }
      return unwind_sort_expression(i->second);
    }
    return x;
  }

  sort_expression_list unwind_sort_expression_list(const sort_expression_list& x)
  {
    std::vector<sort_expression> result;
    for (const sort_expression& s: x)
    {
      result.push_back(unwind_sort_expression(s));
    }
    return sort_expression_list(result.begin(), result.end());
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

  bool EqTypesA(const sort_expression& x1, const sort_expression& x2)
  {
    if (x1 == x2)
    {
      return true;
    }
    return unwind_sort_expression(x1) == unwind_sort_expression(x2);
  }

  bool EqTypesL(sort_expression_list Type1, sort_expression_list Type2)
  {
    if (Type1==Type2)
    {
      return true;
    }
    if (Type1.size()!=Type2.size())
    {
      return false;
    }
    for (; !Type1.empty(); Type1=Type1.tail(),Type2=Type2.tail())
    {
      if (!EqTypesA(Type1.front(),Type2.front()))
      {
        return false;
      }
    }
    return true;
  }

  bool InTypesL(sort_expression_list Type, sorts_list Types)
  {
    for (; !Types.empty(); Types=Types.tail())
      if (EqTypesL(Type, Types.front()))
      {
        return true;
      }
    return false;
  }

  bool IsTypeAllowedA(const data::sort_expression &Type, const data::sort_expression &PosType)
  {
    //Checks if Type is allowed by PosType
    if (data::is_untyped_sort(data::sort_expression(PosType)))
    {
      return true;
    }
    if (is_untyped_possible_sorts(PosType))
    {
      const data::untyped_possible_sorts& s=atermpp::down_cast<data::untyped_possible_sorts>(PosType);
      return InTypesA(Type,s.sorts());
    }

    //PosType is a normal type
    return EqTypesA(Type,PosType);
  }


  bool IsTypeAllowedL(const data::sort_expression_list &TypeList, const data::sort_expression_list PosTypeList)
  {
    //Checks if TypeList is allowed by PosTypeList (each respective element)
    assert(TypeList.size()==PosTypeList.size());
    auto j = PosTypeList.begin();
    for (auto i = TypeList.begin(); i != TypeList.end(); ++i, ++j)
    {
      if (!IsTypeAllowedA(*i,*j))
      {
        return false;
      }
    }
    return true;
  }

  sort_expression_list InsertType(const sort_expression_list TypeList, const sort_expression Type)
  {
    for (sort_expression_list OldTypeList=TypeList; !OldTypeList.empty(); OldTypeList=OldTypeList.tail())
    {
      if (EqTypesA(OldTypeList.front(),Type))
      {
        return TypeList;
      }
    }
    sort_expression_list result=TypeList;
    result.push_front(Type);
    return result;
  }

  sort_expression ExpandNumTypesDown(sort_expression Type)
  {
    // Expand Numeric types down
    if (data::is_untyped_sort(data::sort_expression(Type)))
    {
      return Type;
    }
    if (is_basic_sort(Type))
    {
      Type=UnwindType(Type);
    }

    bool function=false;
    sort_expression_list Args;
    if (is_function_sort(Type))
    {
      const function_sort fs=atermpp::down_cast<const function_sort>(Type);
      function=true;
      Args=fs.domain();
      Type=fs.codomain();
    }

    if (EqTypesA(sort_real::real_(),Type))
    {
      Type=untyped_possible_sorts({ sort_pos::pos(),sort_nat::nat(),sort_int::int_(),sort_real::real_() });
    }
    if (EqTypesA(sort_int::int_(),Type))
    {
      Type=untyped_possible_sorts({ sort_pos::pos(),sort_nat::nat(),sort_int::int_() } );
    }
    if (EqTypesA(sort_nat::nat(),Type))
    {
      Type=untyped_possible_sorts({ sort_pos::pos(),sort_nat::nat() } );
    }
    if (is_container_sort(Type))
    {
      const container_sort& s=atermpp::down_cast<container_sort>(Type);
      const container_type& ConsType = s.container_name();
      if (is_list_container(ConsType))
      {
        Type=container_sort(s.container_name(),ExpandNumTypesDown(s.element_sort()));
      }

      if (is_fset_container(ConsType))
      {
        Type=container_sort(s.container_name(),ExpandNumTypesDown(s.element_sort()));
      }

      if (is_fbag_container(ConsType))
      {
        Type=container_sort(s.container_name(),ExpandNumTypesDown(s.element_sort()));
      }

      if (is_set_container(ConsType))
      {
        const sort_expression shrinked_sorts=ExpandNumTypesDown(s.element_sort());
        Type=untyped_possible_sorts({
                       container_sort(s.container_name(),shrinked_sorts),
                       container_sort(set_container(),shrinked_sorts) } );
      }

      if (is_bag_container(ConsType))
      {
        const sort_expression shrinked_sorts=ExpandNumTypesDown(s.element_sort());
        Type=untyped_possible_sorts({
                       container_sort(s.container_name(),shrinked_sorts),
                       container_sort(bag_container(),shrinked_sorts) } );
      }
    }

    return (function)?function_sort(Args,Type):Type;
  }

  sorts_list TypeListsIntersect(const sorts_list& TypeListList1, const sorts_list& TypeListList2)
  {
    // returns the intersection of the 2 type list lists
    sorts_list Result;

    for (auto i=TypeListList2.begin(); i!=TypeListList2.end(); ++i)
    {
      const sort_expression_list TypeList2= *i;
      if (InTypesL(TypeList2,TypeListList1))
      {
        Result.push_front(TypeList2);
      }
    }
    return atermpp::reverse(Result);
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
    data::sort_expression Type=TypeList.front();
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

  data::sort_expression unwind_sort_expression(const data::sort_expression& x)
  {
    return m_data_typechecker.unwind_sort_expression(x);
  }

  data::sort_expression_list unwind_sort_expression_list(const data::sort_expression_list& x)
  {
    return m_data_typechecker.unwind_sort_expression_list(x);
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
    std::set<core::identifier_string> Acts;
    for (const core::identifier_string& a: act_list)
    {
      check_action_declared(a, x);
      if (!Acts.insert(a).second)  // The action was already in the set.
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
      data::variable_list formal_parameters = utilities::detail::map_element(m_process_parameters, std::make_pair(x.name(), unwind_sort_expression_list(parameters)));

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

  data::sort_expression_list GetNotInferredList(const sorts_list &TypeListList)
  {
    //we get: List of Lists of SortExpressions
    //Outer list: possible parameter types 0..nPosParsVectors-1
    //inner lists: parameter types vectors 0..nFormPars-1

    //we constuct 1 vector (list) of sort expressions (NotInferred if ambiguous)
    //0..nFormPars-1

    data::sort_expression_list Result;
    size_t nFormPars=(TypeListList.front()).size();
    std::vector<data::sort_expression_list> Pars(nFormPars);
    for (size_t i=0; i<nFormPars; i++)
    {
      Pars[i]=data::sort_expression_list();
    }

    for (sorts_list::const_iterator j=TypeListList.begin(); j!=TypeListList.end(); ++j)
    {
      data::sort_expression_list TypeList=*j;
      for (size_t i=0; i<nFormPars; TypeList=TypeList.tail(),i++)
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

  std::pair<bool,data::sort_expression_list> AdjustNotInferredList(const data::sort_expression_list& PosTypeList, const sorts_list& TypeListList)
  {
    // PosTypeList -- List of Sortexpressions (possibly NotInferred(List Sortexpr))
    // TypeListList -- List of (Lists of Types)
    // returns: PosTypeList, adjusted to the elements of TypeListList
    // NULL if cannot be ajusted.

    //if PosTypeList has only normal types -- check if it is in TypeListList,
    //if so return PosTypeList, otherwise return false.
    if (!IsNotInferredL(PosTypeList))
    {
      if (m_data_typechecker.InTypesL(PosTypeList,TypeListList))
      {
        return std::make_pair(true,PosTypeList);
      }
      else
      {
        return std::make_pair(false, data::sort_expression_list());
      }
    }

    //Filter TypeListList to contain only compatible with TypeList lists of parameters.
    sorts_list NewTypeListList;
    for (auto i=TypeListList.begin(); i!=TypeListList.end(); ++i)
    {
      data::sort_expression_list TypeList= *i;
      if (m_data_typechecker.IsTypeAllowedL(TypeList,PosTypeList))
      {
        NewTypeListList.push_front(TypeList);
      }
    }
    if (NewTypeListList.empty())
    {
      return std::make_pair(false, data::sort_expression_list());
    }
    if (NewTypeListList.size()==1)
    {
      return std::make_pair(true,NewTypeListList.front());
    }

    // otherwise return not inferred.
    return std::make_pair(true,GetNotInferredList(atermpp::reverse(NewTypeListList)));
  }

  process_expression MakeActionOrProc(bool is_action,
                                      const core::identifier_string& Name,
                                      const data::sort_expression_list& FormParList,
                                      const data::data_expression_list FactParList
                                     )
  {
    if (is_action)
    {
      return action(action_label(Name,FormParList),FactParList);
    }
    else
    {
      const data::variable_list& FormalVars = utilities::detail::map_element(m_process_parameters, std::make_pair(Name, unwind_sort_expression_list(FormParList)));
      return process_instance(process_identifier(Name,FormalVars),FactParList);
    }
  }

  process_expression RewrActProc(const std::map<core::identifier_string,data::sort_expression>& Vars,
                                 const core::identifier_string& Name,
                                 const data::data_expression_list& pars
                                )
  {
    process_expression Result;
    sorts_list ParList;

    bool action=false;

    auto j = m_actions.find(Name);
    if (j != m_actions.end())
    {
      ParList = j->second;
      action = true;
    }
    else
    {
      auto j = m_equation_sorts.find(Name);
      if ( j !=  m_equation_sorts.end())
      {
        ParList = j->second;
        action = false;
      }
      else
      {
        throw mcrl2::runtime_error("action or process " + core::pp(Name) + " not declared");
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
      throw mcrl2::runtime_error("no " + msg + " " + core::pp(Name)
                      + " with " + atermpp::to_string(nFactPars) + " parameter" + ((nFactPars != 1)?"s":"")
                      + " is declared (while typechecking " + core::pp(Name) + "(" + data::pp(pars) + "))");
    }

    if (ParList.size()==1)
    {
      Result = MakeActionOrProc(action, Name, ParList.front(), pars);
    }
    else
    {
      // we need typechecking to find the correct type of the action.
      // make the list of possible types for the parameters
      Result = MakeActionOrProc(action, Name, GetNotInferredList(ParList), pars);
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
        throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot typecheck " + data::pp(Par) + " as type " + data::pp(m_data_typechecker.ExpandNumTypesDown(PosType)) + " (while typechecking " + core::pp(Name) + "(" + data::pp(pars) + "))");
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
                     core::pp(Name) + "(" + data::pp(pars) + ")");
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
        throw mcrl2::runtime_error("no " + msg + " " + core::pp(Name) + "with type " + data::pp(NewPosTypeList) + " is declared (while typechecking " +
                core::pp(Name) + "(" + data::pp(pars) + "))");
      }
    }

    if (IsNotInferredL(PosTypeList))
    {
      throw mcrl2::runtime_error("ambiguous " + msg + " " + core::pp(Name));
    }
    return MakeActionOrProc(action, Name, PosTypeList, NewPars);
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
    const data::variable_list& FormalPars = utilities::detail::map_element(m_process_parameters, std::make_pair(x.name(), unwind_sort_expression_list(ParList.front())));
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
    // process_expression result = RewrActProc(Vars, t.name(), t.arguments());
    return x;
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
    if (!x.comm_set().empty())
    {
      core::identifier_string_list ActsFrom;

      for (const communication_expression& c: x.comm_set())
      {
        const core::identifier_string_list& cnames = c.action_name().names();
        assert(!cnames.empty());

        if (cnames.size() == 1)
        {
          throw mcrl2::runtime_error("using synchronization as renaming/hiding of action " + core::pp(cnames.front()) + " into " + core::pp(c.name()) + " (typechecking " + process::pp(x) + ")");
        }

        //Actions must be declared
        sorts_list ResTypes;
        if (!core::is_nil(c.name()))
        {
          const std::map<core::identifier_string,sorts_list >::const_iterator j=m_actions.find(c.name());
          if (j == m_actions.end())
          {
            throw mcrl2::runtime_error("synchronizing to an undefined action " + core::pp(c.name()) + " (typechecking " + process::pp(x) + ")");
          }
          ResTypes = j->second;
        }

        for (const core::identifier_string& Act: cnames)
        {
          auto j = m_actions.find(Act);
          sorts_list Types;
          if (j == m_actions.end())
          {
            throw mcrl2::runtime_error("synchronizing an undefined action " + core::pp(Act) + " in (multi)action " + core::pp(cnames) + " (typechecking " + process::pp(x) + ")");
          }
          Types = j->second;
          ResTypes = m_data_typechecker.TypeListsIntersect(ResTypes, Types);
          if (ResTypes.empty())
          {
            throw mcrl2::runtime_error("synchronizing action " + core::pp(Act) + " from (multi)action " + core::pp(cnames) +
                              " into action " + core::pp(c.name()) + ": these have no common type (typechecking " + process::pp(x) + ")");
          }
        }

        //the multiactions in the lhss of comm should not intersect.
        //make the list of unique actions
        core::identifier_string_list Acts;
        for (const core::identifier_string& Act: cnames)
        {
          if (std::find(Acts.begin(), Acts.end(), Act) == Acts.end())
          {
            Acts.push_front(Act);
          }
        }
        for (const core::identifier_string& Act: Acts)
        {
          if (std::find(ActsFrom.begin(), ActsFrom.end(), Act) != ActsFrom.end())
          {
            throw mcrl2::runtime_error("synchronizing action " + core::pp(Act) + " in different ways (typechecking " + process::pp(x) + ")");
          }
          else
          {
            ActsFrom.push_front(Act);
          }
        }
      }
    }
    return comm(x.comm_set(), (*this).apply(x.operand()));
  }

  process_expression apply(const process::allow& x)
  {
    if (x.allow_set().empty())
    {
      mCRL2log(log::warning) << "allowing empty set of (multi) actions (typechecking " << x << ")" << std::endl;
    }
    else
    {
      action_name_multiset_list MActs;
      for (const action_name_multiset& A: x.allow_set())
      {
        const core::identifier_string_list& MAct = A.names();

        //Actions must be declared
        for (const core::identifier_string& Act: MAct)
        {
          if (m_actions.count(Act)==0)
          {
            throw mcrl2::runtime_error("allowing an undefined action " + core::pp(Act) + " in (multi)action " + core::pp(MAct) + " (typechecking " + process::pp(x) + ")");
          }
        }
        if (MActIn(MAct,MActs))
        {
          mCRL2log(log::warning) << "allowing (multi)action " << MAct << " twice (typechecking " << x << ")" << std::endl;
        }
        else
        {
          MActs.push_front(MAct);
        }
      }
    }
    return allow(x.allow_set(), (*this).apply(x.operand()));
  }

  process_expression apply(const process::at& x)
  {
    // const process_expression NewProc=TraverseActProcVarConstP(Vars,t.operand());
    // data::data_expression Time=t.time_stamp();
    // const data::sort_expression NewType=TraverseVarConsTypeD(Vars,Vars,Time,ExpandNumTypesDown(data::sort_real::real_()));
    //
    // data::sort_expression temp;
    // if (!TypeMatchA(data::sort_real::real_(),NewType,temp))
    // {
    //   //upcasting
    //   data::sort_expression CastedNewType;
    //   try
    //   {
    //     std::map<core::identifier_string,data::sort_expression> dummy_table;
    //     CastedNewType=UpCastNumericType(data::sort_real::real_(),NewType,Time,Vars,Vars,dummy_table,false);
    //   }
    //   catch (mcrl2::runtime_error &e)
    //   {
    //     throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot (up)cast time value " + data::pp(Time) + " to type Real");
    //   }
    // }
    //
    // return at(NewProc,Time);
    return x;
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

//  process_expression apply(const data::data_expression& x)
//  {
//    return m_data_typechecker(x, data::sort_bool::bool_(), m_variables);
//  }

//  process_expression apply(const & x)
//  {
//    try
//    {
//    }
//    catch (mcrl2::runtime_error& e)
//    {
//      throw mcrl2::runtime_error(std::string(e.what()) + "\nwhile typechecking " + process_system::pp(x));
//    }
//  }
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

class process_type_checker:public data::data_type_checker
{
  protected:
    std::map<core::identifier_string, sorts_list> m_actions;
    std::map<core::identifier_string, sorts_list> m_equation_sorts;
    std::map<core::identifier_string, data::sort_expression> m_global_variables;

    process_equation_list equations;
    std::map<std::pair<core::identifier_string,data::sort_expression_list>,data::variable_list> m_process_parameters; // process_identifier -> variable_list
    std::map<std::pair<core::identifier_string,data::sort_expression_list>,process_expression> m_process_bodies;  // process_identifier -> rhs
    process_specification type_checked_process_spec;

  public:
    process_type_checker(const process_specification &proc_spec);

    /** \brief     Type check a process expression.
     *  Throws a mcrl2::runtime_error exception if the expression is not well typed.
     *  \param[in] d A process expression that has not been type checked.
     *  \return    a process expression where all untyped identifiers have been replace by typed ones.
     **/
    process_expression operator()(const process_expression &d);
    process_specification operator()();

  protected:
    void ReadInActs(const process::action_label_list &Acts);
    void ReadInProcsAndInit(const std::vector<process_equation>& Procs, const process_expression &Init);
    const process_identifier initial_process(void)
    {
      static process_identifier init_process(core::identifier_string("init"),data::variable_list());
      return init_process;
    }
    void TransformActProcVarConst(void);
    process_expression TraverseActProcVarConstP(
          const std::map<core::identifier_string,data::sort_expression> &Vars,
          const process_expression &ProcTerm);
    process_expression RewrActProc(
               const std::map<core::identifier_string,data::sort_expression> &Vars,
               const core::identifier_string& Name,
               const data::data_expression_list& pars);

    data::sort_expression_list InsertType(const data::sort_expression_list TypeList, const data::sort_expression Type);
    std::pair<bool,data::sort_expression_list> AdjustNotInferredList(
                                  const data::sort_expression_list &PosTypeList,
                                  const sorts_list &TypeListList);
    bool IsTypeAllowedA(const data::sort_expression &Type, const data::sort_expression &PosType);
    bool IsTypeAllowedL(const data::sort_expression_list &TypeList, const data::sort_expression_list PosTypeList);
    data::sort_expression_list GetNotInferredList(const sorts_list &TypeListList);
    sorts_list TypeListsIntersect(
                                  const sorts_list &TypeListList1,
                                  const sorts_list &TypeListList2);
    process_equation_list WriteProcs(const process_equation_vector &oldprocs);
    process_expression MakeActionOrProc(bool is_action,
                                        const core::identifier_string &Name,
                                        const data::sort_expression_list &FormParList,
                                        const data::data_expression_list FactParList);

};

/** \brief     Type check a parsed mCRL2 process specification.
 *  Throws an exception if something went wrong.
 *  \param[in] proc_spec A process specification  that has not been type checked.
 *  \post      proc_spec is type checked.
 **/

inline
void type_check(process_specification& proc_spec)
{
  process_type_checker type_checker(proc_spec);
  proc_spec=type_checker();
}

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_TYPECHECK_H
