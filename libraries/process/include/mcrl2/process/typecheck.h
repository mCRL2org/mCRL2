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

struct typecheck_builder: public process_expression_builder<typecheck_builder>
{
  typedef process_expression_builder<typecheck_builder> super;
  using super::apply;

  data_expression_typechecker& m_data_typechecker;
  std::map<core::identifier_string, data::sort_expression> m_variables;
  const std::map<core::identifier_string, data::sort_expression_list>& m_equation_sorts;

  typecheck_builder(data_expression_typechecker& data_typechecker,
                    const std::map<core::identifier_string, data::sort_expression>& variables,
                    const std::map<core::identifier_string, data::sort_expression_list>& equation_sorts
                   )
    : m_data_typechecker(data_typechecker),
      m_variables(variables),
      m_equation_sorts(equation_sorts)
  {}

  process_expression apply(const untyped_parameter_identifier& x)
  {
    // process_expression result = RewrActProc(Vars, t.name(), t.arguments());
    return x;
  }

  process_expression apply(const process::hide& x)
  {
//    const core::identifier_string_list& act_list = x.hide_set();
//    if (act_list.empty())
//    {
//      mCRL2log(log::warning) << "Hiding empty set of actions (typechecking " << x << ")" << std::endl;
//    }
//
//    std::set<core::identifier_string> Acts;
//    for (const core::identifier_string& a: act_list)
//    {
//      //Actions must be declared
//      if (m_actions.count(a) == 0)
//      {
//        throw mcrl2::runtime_error("Hiding an undefined action " + core::pp(a) + " (typechecking " + core::pp(x) + ")");
//      }
//      if (!Acts.insert(a).second)  // The action was already in the set.
//      {
//        mCRL2log(log::warning) << "Hiding action " << a << " twice (typechecking " << x << ")" << std::endl;
//      }
//    }
//    return hide(act_list, (*this)(x.operand()));
    return x;
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
                    const std::map<core::identifier_string, data::sort_expression_list>& equation_sorts
                   )
{
  return typecheck_builder(data_typechecker, variables, equation_sorts);
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
