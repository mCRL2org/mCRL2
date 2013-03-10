// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/typecheck.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_TYPECHECK_H
#define MCRL2_LPS_TYPECHECK_H

#include "mcrl2/core/detail/struct_core.h"  // gsMakeMultAct
#include "mcrl2/data/typecheck.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/action_rename.h"

namespace mcrl2
{

namespace lps
{


class action_type_checker:public data::data_type_checker
{
  protected:
    std::map<core::identifier_string,atermpp::term_list<data::sort_expression_list> > actions;   //name -> Set(List(sort expression)) because of action polymorphism

  public:
    /** \brief     make a data type checker.
    *  Throws a mcrl2::runtime_error exception if the data_specification is not well typed.
    *  \param[in] d A data specification that does not need to have been type checked.
    *  \return    a data expression where all untyped identifiers have been replace by typed ones.
    **/
    action_type_checker(const data::data_specification &data_spec, const action_label_list& action_decls);

    /** \brief     Type check a multi action.
    *  Throws a mcrl2::runtime_error exception if the expression is not well typed.
    *  \param[in] d A multi action that has not been type checked.
    *  \return    a multi action where all untyped identifiers have been replace by typed ones.
    **/
    multi_action operator()(const multi_action &ma);

    /** \brief     Type check a action_rename_specification;
    *  Throws a mcrl2::runtime_error exception if the expression is not well typed.
    *  \param[in] ars An action rename specification that has not been type checked.
    *  \return    a action rename specification where all untyped identifiers have been replace by typed ones.
    **/
    action_rename_specification operator()(const action_rename_specification &ars);

  protected:
    void ReadInActs(const lps::action_label_list &Acts);
    action TraverseAct(const std::map<core::identifier_string,data::sort_expression> &Vars, const atermpp::aterm_appl &ma);
    action RewrAct(const std::map<core::identifier_string,data::sort_expression> &Vars, const atermpp::aterm_appl &ma);
};


/** \brief     Type check a multi action
 *  Throws an exception if something went wrong.
 *  \param[in] mult_act A multi action that has not been type checked.
 *  \post      mult_action is type checked and sorts have been added when necessary.
 **/
inline
void type_check(
  multi_action& mult_act,
  const data::data_specification& data_spec,
  const action_label_list& action_decls)
{
  action_type_checker type_checker(data_spec,action_decls);
  try
  {
    mult_act=type_checker(mult_act);
  }
  catch (mcrl2::runtime_error &e)
  {
    throw mcrl2::runtime_error(std::string(e.what()) + "\ncould not type check multi action " + pp(mult_act));
  }
}


/** \brief     Type check a multi action
 *  Throws an exception if something went wrong.
 *  \param[in] mult_act A multi action that has not been type checked.
 *  \post      mult_action is type checked and sorts have been added when necessary.
 **/
inline
void type_check(
  std::vector<multi_action>& mult_actions,
  const data::data_specification& data_spec,
  const action_label_list& action_decls)
{
  action_type_checker type_checker(data_spec,action_decls);
  
  for (std::vector<multi_action>::iterator i=mult_actions.begin(); i!=mult_actions.end(); ++i)
  {
    *i=type_checker(*i); // Change the elements in the vector.
  }
}

/// \brief Type checks an action rename specification.
/// \param ar_spec An action rename specifition.
/// \param spec A linear process specification, used for the datatypes and action declarations.
/// \return A type checked rename specification.

inline
action_rename_specification type_check_action_rename_specification(const action_rename_specification &ar_spec, const lps::specification &spec)
{
  lps::action_type_checker type_checker(spec.data(),spec.action_labels());
  return type_checker(ar_spec);
}


} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_TYPECHECK_H
