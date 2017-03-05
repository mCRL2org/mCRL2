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

#include "mcrl2/lps/stochastic_specification.h"
#include "mcrl2/lps/action_rename.h"
#include "mcrl2/process/typecheck.h"
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

class action_rename_type_checker
{
  protected:
    data::data_type_checker m_data_type_checker;
    process::detail::action_context m_action_context;

    action_rename_rule typecheck_action_rename_rule(const action_rename_rule& x, const process::action_label_list& action_labels)
    {
      const data::data_specification& dataspec = m_data_type_checker.typechecked_data_specification();
      data::detail::variable_context variable_context;
      variable_context.add_context_variables(x.variables(), m_data_type_checker);
      data::data_expression condition = m_data_type_checker.typecheck_data_expression(x.condition(), data::sort_bool::bool_(), variable_context);
      process::action lhs = process::typecheck_action(x.lhs().label().name(), x.lhs().arguments(), m_data_type_checker, variable_context, m_action_context);
      process::process_expression rhs = process::type_check_process_expression(x.rhs(), x.variables(), dataspec, action_labels, process::process_identifier_list());
      return action_rename_rule(x.variables(), condition, lhs, rhs);
    }

  public:
    /** \brief Default constructor for an action rename type checker.
    **/
    action_rename_type_checker()
      : m_data_type_checker(data::data_specification())
    {}

    /** \brief    Type check an action_rename_specification.
    *  \param[in] arspec An action rename specification that has not been type checked.
    *  \param[in] lpsspec A linear specification with data, action and global variable declarations.
    *  \return    a action rename specification where all untyped identifiers have been replace by typed ones.
    **/
    action_rename_specification operator()(const action_rename_specification& arspec, const stochastic_specification& lpsspec)
    {
      mCRL2log(log::verbose) << "type checking action rename specification..." << std::endl;
      m_data_type_checker = data::data_type_checker(lpsspec.data() + arspec.data());
      action_rename_specification result = arspec;
      result.data() = m_data_type_checker.typechecked_data_specification();
      lps::detail::normalize_sorts(result);
      m_action_context.clear();
      process::action_label_list action_labels = lpsspec.action_labels() + arspec.action_labels();
      m_action_context.add_context_action_labels(action_labels, m_data_type_checker);
      for (action_rename_rule& rule: result.rules())
      {
        rule = typecheck_action_rename_rule(rule, action_labels);
      }
      mCRL2log(log::debug) << "type checking action rename specification finished" << std::endl;
      return result;
    }
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
/// \param arspec An action rename specifition.
/// \param lpsspec A linear process specification, used for the datatypes and action declarations.
/// \return A type checked rename specification.

inline
action_rename_specification type_check_action_rename_specification(
                                const action_rename_specification& arspec,
                                const lps::stochastic_specification& lpsspec)
{
  lps::action_rename_type_checker typechecker;
  return typechecker(arspec, lpsspec);
}


} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_TYPECHECK_H
