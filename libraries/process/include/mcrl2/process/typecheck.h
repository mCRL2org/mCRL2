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

#include "mcrl2/data/typecheck.h"
#include "mcrl2/process/builder.h"
#include "mcrl2/process/process_specification.h"

namespace mcrl2
{

namespace process
{

typedef atermpp::term_list<data::sort_expression_list> sorts_list;

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

namespace detail
{

struct data_expression_typechecker: protected data::data_type_checker
{
  /** \brief     make a data type checker.
   *  Throws a mcrl2::runtime_error exception if the data_specification is not well typed.
   *  \param[in] data_spec A data specification that does not need to have been type checked.
   *  \return    a data expression where all untyped identifiers have been replace by typed ones.
   **/
  data_expression_typechecker(const data::data_specification& dataspec)
    : data::data_type_checker(dataspec)
  {}

  void check_sort_list_is_declared(const data::sort_expression_list& x)
  {
    return sort_type_checker::check_sort_list_is_declared(x);
  }

  void check_sort_is_declared(const data::sort_expression& x)
  {
    return data::data_type_checker::check_sort_is_declared(x);
  }

  sorts_list TypeListsIntersect(const sorts_list& TypeListList1, const sorts_list& TypeListList2)
  {
    // returns the intersection of the 2 type list lists
    sorts_list Result;

    for (auto i=TypeListList2.begin(); i!=TypeListList2.end(); ++i)
    {
      const data::sort_expression_list TypeList2= *i;
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
  data::data_expression operator()(const data::data_expression& x,
                                   const data::sort_expression& expected_sort,
                                   const std::map<core::identifier_string, data::sort_expression>& variable_context
                                  )
  {
    data::data_expression x1 = x;
    TraverseVarConsTypeD(variable_context, variable_context, x1, expected_sort);
    return x1;
  }

  data::data_specification typechecked_data_specification()
  {
    return type_checked_data_spec;
  }
};

bool MActEq(core::identifier_string_list MAct1, core::identifier_string_list MAct2);

inline
bool MActIn(core::identifier_string_list MAct, action_name_multiset_list MActs)
{
  //returns true if MAct is in MActs
  for (action_name_multiset_list::const_iterator i=MActs.begin(); i!=MActs.end(); ++i)
    if (MActEq(MAct,i->names()))
    {
      return true;
    }

  return false;
}

inline
bool MActEq(core::identifier_string_list MAct1, core::identifier_string_list MAct2)
{
  //returns true if the two multiactions are equal.
  if (MAct1.size()!=MAct2.size())
  {
    return false;
  }
  if (MAct1.empty())
  {
    return true;
  }
  core::identifier_string Act1=MAct1.front();
  MAct1=MAct1.tail();

  //remove Act1 once from MAct2. if not there -- return ATfalse.
  core::identifier_string_list NewMAct2;
  for (; !MAct2.empty(); MAct2=MAct2.tail())
  {
    core::identifier_string Act2=MAct2.front();
    if (Act1==Act2)
    {
      MAct2=atermpp::reverse(NewMAct2)+MAct2.tail();
      return MActEq(MAct1,MAct2);
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
  typedef process_expression_builder<typecheck_builder> super;
  using super::apply;

  data_expression_typechecker& m_data_typechecker;
  std::map<core::identifier_string, data::sort_expression> m_variables;
  const std::map<core::identifier_string, data::sort_expression_list>& m_equation_sorts;
  const std::map<core::identifier_string, sorts_list>& m_actions;

  typecheck_builder(data_expression_typechecker& data_typechecker,
                    const std::map<core::identifier_string, data::sort_expression>& variables,
                    const std::map<core::identifier_string, data::sort_expression_list>& equation_sorts,
                    const std::map<core::identifier_string, sorts_list>& actions
                   )
    : m_data_typechecker(data_typechecker),
      m_variables(variables),
      m_equation_sorts(equation_sorts),
      m_actions(actions)
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
                    data_expression_typechecker& data_typechecker,
                    const std::map<core::identifier_string, data::sort_expression>& variables,
                    const std::map<core::identifier_string, data::sort_expression_list>& equation_sorts,
                    const std::map<core::identifier_string, sorts_list>& actions
                   )
{
  return typecheck_builder(data_typechecker, variables, equation_sorts, actions);
}

} // namespace detail

class process_type_checker:public data::data_type_checker
{
  protected:
    std::map<core::identifier_string, sorts_list> m_actions;
    std::map<core::identifier_string, sorts_list> m_equation_sorts;
    std::map<core::identifier_string, data::sort_expression> m_global_variables;

    process_equation_list equations;
    std::map <std::pair<core::identifier_string,data::sort_expression_list>,data::variable_list> proc_pars; // process_identifier -> variable_list
    std::map <std::pair<core::identifier_string,data::sort_expression_list>,process_expression> proc_bodies;  // process_identifier -> rhs
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
