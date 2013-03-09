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
#include "mcrl2/core/typecheck.h"
#include "mcrl2/data/typecheck.h"
#include "mcrl2/lps/specification.h"

namespace mcrl2
{

namespace lps
{


class multi_action_type_checker:public data::data_type_checker
{
  protected:
    std::map<core::identifier_string,atermpp::term_list<data::sort_expression_list> > actions;   //name -> Set(List(sort expression)) because of action polymorphism

  public:
    /** \brief     make a data type checker.
    *  Throws a mcrl2::runtime_error exception if the data_specification is not well typed.
    *  \param[in] d A data specification that does not need to have been type checked.
    *  \return    a data expression where all untyped identifiers have been replace by typed ones.
    **/
    multi_action_type_checker(const data::data_specification &data_spec, const action_label_list& action_decls);

    /** \brief     Type check a multi action.
    *  Throws a mcrl2::runtime_error exception if the expression is not well typed.
    *  \param[in] d A multi action that has not been type checked.
    *  \return    a multi action where all untyped identifiers have been replace by typed ones.
    **/
    multi_action operator()(const multi_action &ma);

  protected:
    void ReadInActs(const lps::action_label_list &Acts);
    action TraverseAct(const action &ma);
    action RewrAct(const std::map<core::identifier_string,data::sort_expression> &Vars, const action &ma);
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
  multi_action_type_checker type_checker(data_spec,action_decls);
  try
  {
    mult_act=type_checker(mult_act);
  }
  catch (mcrl2::runtime_error &e)
  {
    throw mcrl2::runtime_error(std::string(e.what()) + "\ncould not type check multi action " + pp(mult_act));
  }
/*   // TODO: replace all this nonsense code by a proper type check implementation
  atermpp::aterm_appl t = core::type_check_mult_act(
                  core::detail::gsMakeMultAct(mult_act.actions()),
                  data::detail::data_specification_to_aterm_data_spec(data_spec),
                  action_decls);
  if (t==atermpp::aterm_appl())
  {
    throw mcrl2::runtime_error("could not type check multi action " + pp(lps::detail::multi_action_to_aterm(mult_act)));
  }
  mult_act = multi_action(t); */
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
  multi_action_type_checker type_checker(data_spec,action_decls);
  
  for (std::vector<multi_action>::iterator i=mult_actions.begin(); i!=mult_actions.end(); ++i)
  {
    *i=type_checker(*i); // Change the elements in the vector.
  }
  
/*   // TODO: replace all this nonsense code by a proper type check implementation
  // Bleh; do conversions...
  atermpp::aterm_list l;
  for (std::vector<multi_action>::const_iterator i=mult_actions.begin(); // Using a const_reverse_iterator does not compile on mac.
       i!=mult_actions.end(); ++i)
  {
    l.push_front(i->actions());
  }
  l=core::type_check_mult_actions(
      reverse(l),
      data::detail::data_specification_to_aterm_data_spec(data_spec),
      action_decls);
  // And convert back...
  mult_actions.clear();
  for (atermpp::aterm_list::const_iterator i=l.begin(); i!=l.end(); ++i)
  {
    mult_actions.push_back(multi_action((action_list)(*i)));
  } */
}

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_TYPECHECK_H
