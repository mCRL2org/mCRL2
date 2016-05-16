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

#include "mcrl2/process/typecheck.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/action_rename.h"
#include "mcrl2/process/untyped_multi_action.h"

namespace mcrl2
{

namespace lps
{

class multi_action_type_checker
{
  protected:
    data::data_type_checker m_data_type_checker;
    process::detail::action_context m_action_context;
    data::detail::variable_context m_variable_context;

  public:
    template <typename VariableContainer, typename ActionLabelContainer>
    multi_action_type_checker(const data::data_specification& dataspec,
                              const VariableContainer& variables,
                              const ActionLabelContainer& action_labels
                             )
      : m_data_type_checker(dataspec)
    {
      m_action_context.add_context_action_labels(action_labels, m_data_type_checker);
      m_variable_context.add_context_variables(variables, m_data_type_checker);
    }

    /// \brief Default constructor
    multi_action_type_checker(const data::data_specification& dataspec = data::data_specification())
      : m_data_type_checker(dataspec)
    {}

    /** \brief     Type check a multi action.
     *  Throws a mcrl2::runtime_error exception if the expression is not well typed.
     *  \param[in] x A multi action that has not been type checked.
     *  \return    a typed multi action.
     **/
    multi_action operator()(const process::untyped_multi_action& x)
    {
      std::vector<process::action> actions;
      try
      {
        for (const data::untyped_data_parameter& a: x.actions())
        {
          actions.push_back(process::typecheck_action(a.name(), a.arguments(), m_data_type_checker, m_variable_context, m_action_context));
        }
      }
      catch (mcrl2::runtime_error& e)
      {
        throw mcrl2::runtime_error(std::string(e.what()) + "\ntype checking of multiaction failed (" + process::pp(x) + ")");
      }
      return multi_action(process::action_list(actions.begin(), actions.end()));
    }
};

class action_rename_type_checker:public data::data_type_checker
{
  protected:
    std::map<core::identifier_string,atermpp::term_list<data::sort_expression_list> > actions;   //name -> Set(List(sort expression)) because of action polymorphism

  public:
    /** \brief     make an action type checker.
    *  Throws a mcrl2::runtime_error exception if the data_specification is not well typed.
    *  \param[in] data_spec A data specification that does not need to have been type checked.
    *  \param[in] action_decls A list of action declarations
    *  \return    a data expression where all untyped identifiers have been replace by typed ones.
    **/
    action_rename_type_checker(const data::data_specification& data_spec, const process::action_label_list& action_decls);

    /** \brief     Type check a action_rename_specification;
    *  Throws a mcrl2::runtime_error exception if the expression is not well typed.
    *  \param[in] ars An action rename specification that has not been type checked.
    *  \return    a action rename specification where all untyped identifiers have been replace by typed ones.
    **/
    action_rename_specification operator()(const action_rename_specification& ars);

  protected:
    void ReadInActs(const process::action_label_list& Acts);
    process::action TraverseAct(const std::map<core::identifier_string,data::sort_expression>& Vars, const data::untyped_data_parameter& ma);
    process::action RewrAct(const std::map<core::identifier_string,data::sort_expression>& Vars, const data::untyped_data_parameter& ma);
};


/** \brief     Type check a multi action
 *  Throws an exception if something went wrong.
 *  \param[in] mult_act A multi action that has not been type checked.
 *  \param[in] data_spec A data specification to use as context.
 *  \param[in] action_decls A list of action declarations to use as context.
 *  \post      mult_action is type checked and sorts have been added when necessary.
 **/
inline
multi_action type_check_multi_action(process::untyped_multi_action& mult_act,
                                     const data::data_specification& data_spec,
                                     const process::action_label_list& action_decls
                                    )
{
  multi_action_type_checker typechecker(data_spec, data::variable_list(), action_decls);
  return typechecker(mult_act);
}

/// \brief Type checks an action rename specification.
/// \param ar_spec An action rename specifition.
/// \param spec A linear process specification, used for the datatypes and action declarations.
/// \return A type checked rename specification.

inline
action_rename_specification type_check_action_rename_specification(const action_rename_specification& ar_spec, const lps::specification& spec)
{
  lps::action_rename_type_checker type_checker(spec.data(),spec.action_labels());
  return type_checker(ar_spec);
}


} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_TYPECHECK_H
