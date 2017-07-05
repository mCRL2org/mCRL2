// Author(s): Jan Friso Groote, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/action_rename.h
/// \brief Action rename specifications.

#ifndef MCRL2_LPS_ACTION_RENAME_H
#define MCRL2_LPS_ACTION_RENAME_H

#include "mcrl2/utilities/exception.h"
#include "mcrl2/core/detail/function_symbols.h"
#include "mcrl2/core/parse.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/lps/stochastic_specification.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/set_identifier_generator.h"
#include "mcrl2/data/substitutions/mutable_map_substitution.h"
#include "mcrl2/process/normalize_sorts.h"
#include "mcrl2/process/replace.h"
#include "mcrl2/process/translate_user_notation.h"

// //Action rename rules
// <ActionRenameRules>
//                ::= ActionRenameRules(<ActionRenameRule>*)
//
// //Action rename rule
// <ActionRenameRule>
//                ::= ActionRenameRule(<DataVarId>*, <DataExprOrNil>,
//                      <ParamIdOrAction>, <ActionRenameRuleRHS>)
//
// //Right-hand side of an action rename rule
// <ActionRenameRuleRHS>
//                ::= <ParamId>                                             [- tc]
//                  | <Action>                                              [+ tc]
//                  | Delta
//                  | Tau
//
// //Action rename action_rename_specification
// <ActionRenameSpec>
//                ::= ActionRenameSpec(<DataSpec>, <ActSpec>, <ActionRenameRules>)

namespace mcrl2
{

namespace lps
{

//                ::= ActionRenameRule(<DataVarId>*, <DataExprOrNil>,
//                      <ParamIdOrAction>, <ActionRenameRuleRHS>)

/// \brief Action rename rule
class action_rename_rule
{
  protected:
    /// \brief The data variables of the rule
    data::variable_list m_variables;

    /// \brief The condition of the rule
    data::data_expression    m_condition;

    /// \brief The left hand side of the rule
    process::action         m_lhs;

    /// \brief right hand side of the rule. Can only be an action, tau or delta.
    process::process_expression   m_rhs;

    bool check_that_rhs_is_tau_delta_or_an_action() const
    {
      return is_delta(m_rhs) || is_tau(m_rhs) || process::is_action(m_rhs);
    }

  public:
    /// \brief Constructor.
    action_rename_rule()
    { }

    /// \brief Constructor.
    /// \param t A term
    action_rename_rule(const atermpp::aterm_appl& t)
    {
      assert(core::detail::check_rule_ActionRenameRule(t));
      atermpp::aterm_appl::iterator i = t.begin();
      m_variables       = data::variable_list(*i++);
      m_condition       = data::data_expression(*i++);
      m_lhs             = process::action(*i++);
      m_rhs             = process::process_expression(*i);
      assert(check_that_rhs_is_tau_delta_or_an_action());
    }

    /// \brief Constructor.
    /// \param t1 A term
    explicit action_rename_rule(const atermpp::aterm& t1)
    {
      const atermpp::aterm_appl t(t1);
      assert(core::detail::check_rule_ActionRenameRule(t));
      atermpp::aterm_appl::iterator i = t.begin();
      m_variables       = data::variable_list(*i++);
      m_condition       = data::data_expression(*i++);
      m_lhs             = process::action(*i++);
      m_rhs             = process::process_expression(*i);
      assert(check_that_rhs_is_tau_delta_or_an_action());
    }

    /// \brief Constructor.
    action_rename_rule(const data::variable_list&   variables,
                       const data::data_expression& condition,
                       const process::action&       lhs,
                       const process::process_expression& rhs)
      : m_variables(variables), m_condition(condition), m_lhs(lhs), m_rhs(rhs)
    {
      assert(check_that_rhs_is_tau_delta_or_an_action());
    }

    /// \brief Returns the variables of the rule.
    /// \return The variables of the rule.
    const data::variable_list& variables() const
    {
      return m_variables;
    }

    /// \brief Returns the variables of the rule.
    /// \return The variables of the rule.
    data::variable_list& variables()
    {
      return m_variables;
    }

    /// \brief Returns the condition of the rule.
    /// \return The condition of the rule.
    const data::data_expression& condition() const
    {
      return m_condition;
    }

    /// \brief Returns the condition of the rule.
    /// \return The condition of the rule.
    data::data_expression& condition()
    {
      return m_condition;
    }

    /// \brief Returns the left hand side of the rule.
    /// \return The left hand side of the rule.
    const process::action& lhs() const // Type should be action, but as it can be a ParamId(identifier_string,data_expression_list),
                                    // which is not accepted by the typechecker, this is a temporary fix, until this option
                                    // is added to an action.
    {
      return m_lhs;
    }

    /// \brief Returns the right hand side of the rule.
    /// \return The right hand side of the rule.
    const process::process_expression& rhs() const
    {
      assert(check_that_rhs_is_tau_delta_or_an_action());
      return m_rhs;
    }
};

/// \brief Read-only singly linked list of action rename rules
// typedef atermpp::term_list<action_rename_rule> action_rename_rule_list;

/// \brief Action rename specification
class action_rename_specification
{
  protected:

    /// \brief The data specification of the action rename specification
    data::data_specification m_data;

    /// \brief The action labels of the action rename specification
    process::action_label_list        m_action_labels;

    /// \brief The action rename rules of the action rename specification
    std::vector <action_rename_rule>  m_rules;

  public:
    /// \brief Constructor.
    action_rename_specification()
    { }

    /// \brief Constructor.
    /// \param t A term
    action_rename_specification(atermpp::aterm_appl t)
    {
      assert(core::detail::check_rule_ActionRenameSpec(t));
      atermpp::aterm_appl::iterator i = t.begin();
      m_data            = atermpp::aterm_appl(*i++);
      m_action_labels   = process::action_label_list(atermpp::aterm_appl(*i++)[0]);

      atermpp::aterm_list rules_list = static_cast<atermpp::aterm_list>(atermpp::aterm_appl(*i)[0]);
      for (atermpp::aterm_list::const_iterator i=rules_list.begin(); i!=rules_list.end(); ++i)
      {
        m_rules.push_back(action_rename_rule(*i));
      }
    }

    /// \brief Constructor.
    /// \param data A data specification
    /// \param action_labels A sequence of action labels
    /// \param rules A sequence of action rename rules
    action_rename_specification(
      const data::data_specification& data,
      const process::action_label_list& action_labels,
      const std::vector <action_rename_rule>& rules)
      :
      m_data(data),
      m_action_labels(action_labels),
      m_rules(rules)
    { }

    /// \brief Returns the data action_rename_specification.
    /// \return The data action_rename_specification.
    const data::data_specification& data() const
    {
      return m_data;
    }

    /// \brief Returns the data specification.
    data::data_specification& data()
    {
      return m_data;
    }

    /// \brief Returns the sequence of action labels
    /// \return A sequence of action labels containing all action
    /// labels occurring in the action_rename_specification (but it can have more).
    const process::action_label_list& action_labels() const
    {
      return m_action_labels;
    }

    /// \brief Returns the sequence of action labels
    process::action_label_list& action_labels()
    {
      return m_action_labels;
    }

    /// \brief Returns the action rename rules.
    /// \return The action rename rules.
    const std::vector<action_rename_rule>& rules() const
    {
      return m_rules;
    }

    /// \brief Returns the action rename rules.
    std::vector<action_rename_rule>& rules()
    {
      return m_rules;
    }

    /// \brief Indicates whether the action_rename_specification is well typed.
    /// \return Always returns true.
    bool is_well_typed() const
    {
      return true;
    }
};

inline
atermpp::aterm_appl action_rename_rule_to_aterm(const action_rename_rule& rule)
{
  return atermpp::aterm_appl(core::detail::function_symbol_ActionRenameRule(), rule.variables(), rule.condition(), rule.lhs(), rule.rhs());
}

inline
atermpp::aterm_appl action_rename_specification_to_aterm(const action_rename_specification& spec)
{
  std::vector<atermpp::aterm_appl> rules;
  for (std::vector<action_rename_rule>::const_iterator i = spec.rules().begin(); i != spec.rules().end(); ++i)
  {
    rules.push_back(action_rename_rule_to_aterm(*i));
  }
  return atermpp::aterm_appl(core::detail::function_symbol_ActionRenameSpec(),
    data::detail::data_specification_to_aterm_data_spec(spec.data()),
    atermpp::aterm_appl(core::detail::function_symbol_ActSpec(), spec.action_labels()),
    atermpp::aterm_appl(core::detail::function_symbol_ActionRenameRules(), atermpp::aterm_list(rules.begin(), rules.end()))
  );
}

}
}


namespace mcrl2
{

namespace lps
{

/// \cond INTERNAL_DOCS
namespace detail
{
/// \brief Renames variables
/// \param rcond A data expression
/// \param rleft An action
/// \param rright An action
/// \param generator A generator for fresh identifiers
template <typename IdentifierGenerator>
void rename_renamerule_variables(data::data_expression& rcond, process::action& rleft, process::action& rright, IdentifierGenerator& generator)
{
  data::mutable_map_substitution<> renamings;

  std::set< data::variable > new_vars = data::find_all_variables(rleft.arguments());

  for (std::set< data::variable >::const_iterator i = new_vars.begin(); i != new_vars.end(); ++i)
  {
    mcrl2::core::identifier_string new_name = generator(std::string(i->name()));

    if (new_name != i->name())
    {
      renamings[*i] = data::variable(new_name, i->sort());
    }
  }

  std::set<data::variable> renamings_variables = data::substitution_variables(renamings);
  rcond = data::replace_variables_capture_avoiding(rcond, renamings, renamings_variables);
  rleft = process::replace_variables_capture_avoiding(rleft, renamings, renamings_variables);
  rright = process::replace_variables_capture_avoiding(rright, renamings, renamings_variables);
}

/* ------------------------------------------ Normalise sorts ------------------------------------------ */

inline
void normalize_sorts(action_rename_specification& arspec)
{
  arspec.action_labels()=process::normalize_sorts(arspec.action_labels(), arspec.data());

  for (action_rename_rule& rule: arspec.rules())
  {
    rule = action_rename_rule(data::normalize_sorts(rule.variables(), arspec.data()),
                              data::normalize_sorts(rule.condition(), arspec.data()),
                              process::normalize_sorts(rule.lhs(), arspec.data()),
                              process::normalize_sorts(rule.rhs(), arspec.data()));
  }
}


/* ------------------------------------------ Translate user notation ------------------------------------------ */

inline
void translate_user_notation(action_rename_specification& arspec)
{
  for (action_rename_rule& rule: arspec.rules())
  {
    rule = action_rename_rule(rule.variables(),
                              data::translate_user_notation(rule.condition()),
                              process::translate_user_notation(rule.lhs()),
                              process::translate_user_notation(rule.rhs()));
  }
}

} // namespace detail
/// \endcond

/// \brief  Rename the actions in a linear specification using a given action_rename_spec
/// \details The actions in a linear specification are renamed according to a given
///         action rename specification.
///         Note that the rules are applied in the order they appear in the specification.
///         This yield quite elaborate conditions in the resulting lps, as a latter rule
///         can only be applied if an earlier rule is not applicable. Note also that
///         there is always a default summand, where the action is not renamed. Using
///         sum elimination and rewriting a substantial reduction of the conditions that
///         are generated can be obtained, often allowing many summands to be removed.
/// \param  action_rename_spec The action_rename_specification to be used.
/// \param  lps_old_spec The input linear specification.
/// \return The lps_old_spec where all actions have been renamed according
///         to action_rename_spec.
inline
lps::stochastic_specification action_rename(
  const action_rename_specification& action_rename_spec,
  const lps::stochastic_specification& lps_old_spec)
{
  using namespace mcrl2::core;
  using namespace mcrl2::data;
  using namespace mcrl2::lps;
  using namespace std;

  const std::vector <action_rename_rule>& rename_rules = action_rename_spec.rules();
  stochastic_action_summand_vector lps_old_action_summands = lps_old_spec.process().action_summands();
  deadlock_summand_vector lps_deadlock_summands = lps_old_spec.process().deadlock_summands();
  process::action_list lps_new_actions;

  data::set_identifier_generator generator;
  generator.add_identifiers(lps::find_identifiers(lps_old_spec));
  generator.add_identifiers(data::function_and_mapping_identifiers(lps_old_spec.data()));

  //go through the rename rules of the rename file
  mCRL2log(log::debug) << "Rename rules found: " << rename_rules.size() << "\n";
  for (std::vector <action_rename_rule>::const_iterator i = rename_rules.begin(); i != rename_rules.end(); ++i)
  {
    stochastic_action_summand_vector lps_new_action_summands;

    data_expression rule_condition = i->condition();
    process::action rule_old_action = i->lhs();
    process::action rule_new_action;
    process::process_expression new_element = i->rhs();
    if (!is_tau(new_element) && !is_delta(new_element))
    {
      rule_new_action =  atermpp::down_cast<process::action>(new_element);
    }

    const bool to_tau = is_tau(new_element);
    const bool to_delta = is_delta(new_element);

    // Check here that the arguments of the rule_old_action only consist
    // of uniquely occurring variables or closed terms. Furthermore, check that the variables
    // in rule_new_action and in rule_condition are a subset of those in
    // rule_old_action. This check ought to be done in the static checking
    // part of the renaming rules, but as yet it has nog been done. Ultimately
    // this check should be moved there.

    // first check that the arguments of rule_old_action are variables or closed
    // terms.

    for (data_expression_list::iterator
         rule_old_argument_i = rule_old_action.arguments().begin();
         rule_old_argument_i != rule_old_action.arguments().end();
         rule_old_argument_i++)
    {
      if (!is_variable(*rule_old_argument_i) &&
          (!(data::find_all_variables(*rule_old_argument_i).empty())))
      {
        throw mcrl2::runtime_error("The arguments of the lhs " + process::pp(rule_old_action) +
                                   " are not variables or closed expressions");
      }
    }

    // Check whether the variables in rhs are included in the lefthandside.
    std::set < variable > variables_in_old_rule = process::find_free_variables(rule_old_action);
    std::set < variable > variables_in_new_rule = process::find_free_variables(rule_new_action);

    if (!includes(variables_in_old_rule.begin(),variables_in_old_rule.end(),
                  variables_in_new_rule.begin(),variables_in_new_rule.end()))
    {
      throw mcrl2::runtime_error("There are variables occurring in rhs " + process::pp(rule_new_action) +
                                 " of a rename rule not occurring in lhs " + process::pp(rule_old_action));
    }

    // Check whether the variables in condition are included in the lefthandside.
    std::set < variable > variables_in_condition = data::find_free_variables(rule_condition);
    if (!includes(variables_in_old_rule.begin(),variables_in_old_rule.end(),
                  variables_in_condition.begin(),variables_in_condition.end()))
    {
      throw mcrl2::runtime_error("There are variables occurring in the condition " + data::pp(rule_condition) +
                                 " of a rename rule not occurring in lhs " + process::pp(rule_old_action));
    }

    // check for double occurrences of variables in the lhs. Note that variables_in_old_rule
    // is empty at the end.
    for (data_expression_list::iterator i=rule_old_action.arguments().begin() ;
         i!=rule_old_action.arguments().end() ; i++)
    {
      if (is_variable(*i))
      {
        const variable& v = atermpp::down_cast<variable>(*i);
        if (variables_in_old_rule.find(v)==variables_in_old_rule.end())
        {
          throw mcrl2::runtime_error("Variable " + data::pp(v) + " occurs more than once in lhs " +
                                     process::pp(rule_old_action) + " of an action rename rule");
        }
        else
        {
          variables_in_old_rule.erase(v);
        }
      }
    }
    assert(variables_in_old_rule.empty());


    //go through the summands of the old lps
    mCRL2log(log::debug) << "Action summands found: " << lps_old_action_summands.size() << "\n";
    for (const stochastic_action_summand& lps_old_action_summand: lps_old_action_summands)
    {
      process::action_list lps_old_actions = lps_old_action_summand.multi_action().actions();

      /* For each individual action in the multi-action, for which the
         rename rule applies, two new summands must be made, namely one
         where the rule does not match with the parameters of the action,
         and one where it actually does. This means that for a multiaction
         with k summands 2^k new summands can result. */

      std::vector < variable_list >
      lps_new_sum_vars(1,lps_old_action_summand.summation_variables());
      std::vector < data_expression >
      lps_new_condition(1,lps_old_action_summand.condition());
      std::vector < process::action_list >
      lps_new_actions(1,process::action_list());
      std::vector < bool > lps_new_actions_is_delta(1,false);

      mCRL2log(log::debug) << "Actions in summand found: " << lps_old_actions.size() << "\n";
      for (process::action_list::iterator loai = lps_old_actions.begin();
           loai != lps_old_actions.end(); loai++)
      {
        process::action lps_old_action = *loai;

        if (equal_signatures(lps_old_action, rule_old_action))
        {
          mCRL2log(log::debug) << "Renaming action " << rule_old_action << "\n";

          //rename all previously used variables
          data_expression renamed_rule_condition=rule_condition;
          process::action renamed_rule_old_action=rule_old_action;
          process::action renamed_rule_new_action=rule_new_action;
          detail::rename_renamerule_variables(renamed_rule_condition, renamed_rule_old_action, renamed_rule_new_action, generator);

          //go through the arguments of the action
          data_expression_list::iterator
          lps_old_argument_i = lps_old_action.arguments().begin();
          data_expression new_equalities_condition=sort_bool::true_();
          for (data_expression_list::iterator
               rule_old_argument_i = renamed_rule_old_action.arguments().begin();
               rule_old_argument_i != renamed_rule_old_action.arguments().end();
               rule_old_argument_i++)
          {
            if (is_variable(*rule_old_argument_i))
            {
              new_equalities_condition=lazy::and_(new_equalities_condition,
                                                  data::equal_to(*rule_old_argument_i, *lps_old_argument_i));
            }
            else
            {
              assert((data::find_all_variables(*rule_old_argument_i).empty())); // the argument must be closed,
              // which is checked above.
              renamed_rule_condition=
                lazy::and_(renamed_rule_condition,
                           data::equal_to(*rule_old_argument_i, *lps_old_argument_i));
            }
            lps_old_argument_i++;
          }

          /* insert the new equality condition in all the newly generated summands */
          for (std::vector < data_expression > :: iterator i=lps_new_condition.begin() ;
               i!=lps_new_condition.end() ; i++)
          {
            *i=lazy::and_(*i,new_equalities_condition);
          }

          /* insert the new sum variables in all the newly generated summands */
          std::set<variable> new_vars = find_all_variables(renamed_rule_old_action);
          for (std::set<variable>::iterator sdvi = new_vars.begin();
               sdvi != new_vars.end(); sdvi++)
          {
            for (std::vector < variable_list > :: iterator i=lps_new_sum_vars.begin() ;
                 i!=lps_new_sum_vars.end() ; i++)
            {
              i->push_front(*sdvi);
            }
          }

          if (renamed_rule_condition==sort_bool::true_())
          {
            if (to_delta)
            {
              std::vector < bool >::iterator i_is_delta=lps_new_actions_is_delta.begin();
              for (std::vector < process::action_list > :: iterator
                   i=lps_new_actions.begin() ;
                   i!=lps_new_actions.end() ; ++i,++i_is_delta)
              {
                *i=process::action_list(); // the action becomes delta
                *i_is_delta=true;
              }
            }
            else if (!to_tau)
            {
              std::vector < bool >::iterator i_is_delta=lps_new_actions_is_delta.begin();
              for (std::vector < process::action_list > :: iterator i=lps_new_actions.begin() ;
                   i!=lps_new_actions.end() ; ++i,++i_is_delta)
              {
                if (!*i_is_delta) // the action is not delta
                {
                  i->push_front(renamed_rule_new_action);
                }
              }
            }
          }
          else if (renamed_rule_condition==sort_bool::false_())
          {
            std::vector < bool >::iterator i_is_delta=lps_new_actions_is_delta.begin();
            for (std::vector < process::action_list > :: iterator i=lps_new_actions.begin() ;
                 i!=lps_new_actions.end() ; ++i,++i_is_delta)
            {
              if (!*i_is_delta) // The action does not equal delta.
              {
                i->push_front(lps_old_action);
              }
            }

          }
          else
          {
            /* Duplicate summands, one where the renaming is applied, and one where it is not
               applied. */

            std::vector < process::action_list > lps_new_actions_temp(lps_new_actions);

            if (!to_tau) // if the new element is tau, we do not insert it in the multi-action.
            {
              std::vector < bool >::iterator i_is_delta=lps_new_actions_is_delta.begin();
              for (std::vector < process::action_list > :: iterator
                   i=lps_new_actions.begin() ;
                   i!=lps_new_actions.end() ; ++i,++i_is_delta)
              {
                if (to_delta)
                {
                  *i=process::action_list();
                  *i_is_delta=true;
                }
                else
                {
                  i->push_front(renamed_rule_new_action);
                  *i_is_delta=false;
                }
              }
            }

            for (std::vector < process::action_list > :: iterator
                 i=lps_new_actions_temp.begin() ;
                 i!=lps_new_actions_temp.end() ; ++i)
            {
              lps_new_actions_is_delta.push_back(false); // An non renamed action is not delta;
              i->push_front(lps_old_action);
            }

            lps_new_actions.insert(lps_new_actions.end(),
                                   lps_new_actions_temp.begin(),
                                   lps_new_actions_temp.end());
            assert(lps_new_actions_is_delta.size()==lps_new_actions.size());

            /* lps_new_condition_temp will contain the conditions in conjunction with
               the negated new_condition. It will be concatenated to lps_new_condition,
               in which the terms will be conjoined with the non-negated new_condition */

            std::vector < data_expression > lps_new_condition_temp(lps_new_condition);

            for (std::vector < data_expression > :: iterator i=lps_new_condition.begin() ;
                 i!=lps_new_condition.end() ; i++)
            {
              *i=lazy::and_(*i,renamed_rule_condition);
            }

            for (std::vector < data_expression > :: iterator i=lps_new_condition_temp.begin() ;
                 i!=lps_new_condition_temp.end() ; i++)
            {
              *i=lazy::and_(*i,sort_bool::not_(renamed_rule_condition));
            }

            lps_new_condition.insert(lps_new_condition.end(),
                                     lps_new_condition_temp.begin(),
                                     lps_new_condition_temp.end());

            std::vector < variable_list > lps_new_sum_vars_temp(lps_new_sum_vars);
            lps_new_sum_vars.insert(lps_new_sum_vars.end(),
                                    lps_new_sum_vars_temp.begin(),
                                    lps_new_sum_vars_temp.end());
          }
        }//end if(equal_signatures(...))
        else
        {
          for (std::vector < process::action_list > :: iterator i=lps_new_actions.begin() ;
               i!=lps_new_actions.end() ; ++i)
          {
            i->push_front(lps_old_action);
          }
        }
        mCRL2log(log::debug) << "Action done\n";

      } //end of action list iterator

      /* Add the summands to lps_new_action_summands or to the deadlock summands*/

      std::vector < process::action_list > :: iterator i_act=lps_new_actions.begin();
      std::vector < bool > :: iterator i_act_is_delta=lps_new_actions_is_delta.begin();
      std::vector < variable_list > :: iterator i_sumvars=lps_new_sum_vars.begin();
      for (std::vector < data_expression > :: iterator i_cond=lps_new_condition.begin() ;
           i_cond!=lps_new_condition.end() ; ++i_cond, ++i_act_is_delta)
      {
        //create a summand for the new lps
        if (*i_act_is_delta)
        {
          // Create a deadlock summand.
          const deadlock_summand d(*i_sumvars,
                                   *i_cond,
                                   deadlock(lps_old_action_summand.multi_action().time()));
          lps_deadlock_summands.push_back(d);
        }
        else
        {
          // create an action summand.
          stochastic_action_summand lps_new_summand(*i_sumvars,
                                                    *i_cond,
                                                    multi_action(reverse(*i_act), lps_old_action_summand.multi_action().time()),
                                                    lps_old_action_summand.assignments(),
                                                    lps_old_action_summand.distribution());
          lps_new_action_summands.push_back(lps_new_summand);
        }
        i_act++;
        i_sumvars++;
      }
    } // end of summand list iterator
    lps_old_action_summands = lps_new_action_summands;
  } //end of rename rule iterator

  mCRL2log(log::debug) << "Simplifying the result...\n";

  stochastic_linear_process new_process(lps_old_spec.process().process_parameters(),
                                        lps_deadlock_summands,
                                        lps_old_action_summands);

  // add action_rename_spec.action_labels to action_rename_spec.action_labels without adding duplates.
  process::action_label_list all=action_rename_spec.action_labels();
  for (auto i=lps_old_spec.action_labels().begin();
       i!=lps_old_spec.action_labels().end(); ++i)
  {
    if (find(action_rename_spec.action_labels().begin(),
             action_rename_spec.action_labels().end(),*i)==action_rename_spec.action_labels().end())
    {
      // Not found;
      all.push_front(*i);
    }
  }
  stochastic_specification lps_new_spec(action_rename_spec.data(), // This contains the data of the lps and the rename file.
                                        all,
                                        lps_old_spec.global_variables(),
                                        new_process,
                                        lps_old_spec.initial_process());

  mCRL2log(log::debug) << "New lps complete\n";
  return lps_new_spec;
} //end of rename(...)

} // namespace lps

} // namespace mcrl2


#endif // MCRL2_LPS_ACTION_RENAME_H
