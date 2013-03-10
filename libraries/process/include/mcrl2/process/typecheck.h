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

#include "mcrl2/lps/typecheck.h"
#include "mcrl2/process/process_specification.h"

namespace mcrl2
{

namespace process
{


class process_type_checker:public data::data_type_checker
{
  protected:
    std::map<core::identifier_string,atermpp::term_list<data::sort_expression_list> > actions;   //name -> Set(List(sort expression)) because of action polymorphism
    std::map<core::identifier_string,atermpp::term_list<data::sort_expression_list> > processes; //name -> Set(List(sort expression)) because of process polymorphism
    std::map<core::identifier_string,data::sort_expression> glob_vars;                  //name -> Type: global variables (for proc, pbes and init)

    process_equation_list equations;
    std::map <process_identifier,data::variable_list> proc_pars; // process_identifier -> variable_list
    std::map <process_identifier,process_expression> proc_bodies;  // process_identifier -> rhs
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
    void ReadInActs(const lps::action_label_list &Acts);
    void ReadInProcsAndInit(const std::vector<process_equation>& Procs, const process_expression &Init);
    const process_identifier initial_process(void)
    {
      static process_identifier init_process(core::identifier_string("init"),data::sort_expression_list());
      return init_process;
    }
    void TransformActProcVarConst(void);
    process_expression TraverseActProcVarConstP(
          const std::map<core::identifier_string,data::sort_expression> &Vars, 
          const process_expression &ProcTerm);
    process_expression RewrActProc(const std::map<core::identifier_string,data::sort_expression> &Vars, process_expression ProcTerm);
    data::sort_expression_list InsertType(const data::sort_expression_list TypeList, const data::sort_expression Type);
    std::pair<bool,data::sort_expression_list> AdjustNotInferredList(
                                  const data::sort_expression_list &PosTypeList, 
                                  const atermpp::term_list<data::sort_expression_list> &TypeListList);
    bool IsTypeAllowedA(const data::sort_expression &Type, const data::sort_expression &PosType);
    bool IsTypeAllowedL(const data::sort_expression_list &TypeList, const data::sort_expression_list PosTypeList);
    data::sort_expression_list GetNotInferredList(const atermpp::term_list<data::sort_expression_list> &TypeListList);
    atermpp::term_list<data::sort_expression_list> TypeListsIntersect(
                                  const atermpp::term_list<data::sort_expression_list> &TypeListList1, 
                                  const atermpp::term_list<data::sort_expression_list> &TypeListList2);
    process_equation_list WriteProcs(const process_equation_vector &oldprocs);

};


/** \brief     Type check a process expression.
 *  Throws an exception if something went wrong.
 *  \param[in] proc_expr A process expression that has not been type checked.
 *  \post      proc_expr is type checked.
 **/
inline
void type_check(process_expression& proc_expr, const process_specification& proc_spec)
{
  try 
  {
    process_type_checker type_checker(proc_spec);
    proc_expr=type_checker(proc_expr);
  }
  catch (mcrl2::runtime_error &e)
  {
    throw mcrl2::runtime_error(std::string(e.what()) + "\ncould not type check " + pp(proc_expr));
  }
}

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
