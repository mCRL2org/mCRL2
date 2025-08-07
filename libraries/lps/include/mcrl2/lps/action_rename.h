// Author(s): Jan Friso Groote, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/action_rename.h
/// \brief Action rename specifications.

#ifndef MCRL2_LPS_ACTION_RENAME_H
#define MCRL2_LPS_ACTION_RENAME_H

#include <regex>

#include "mcrl2/core/parse.h"

#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/substitutions/mutable_map_substitution.h"

#include "mcrl2/lps/stochastic_specification.h"
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

namespace mcrl2::lps
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
    action_rename_rule() = default;

    /// \brief Constructor.
    /// \param t A term
    action_rename_rule(const atermpp::aterm& t)
    {
      assert(core::detail::check_rule_ActionRenameRule(t));
      atermpp::aterm::iterator i = t.begin();
      m_variables       = atermpp::down_cast<data::variable_list>(*i++);
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
    action_rename_specification() = default;

    /// \brief Constructor.
    /// \param t A term
    action_rename_specification(atermpp::aterm t)
    {
      assert(core::detail::check_rule_ActionRenameSpec(t));
      atermpp::aterm::iterator i = t.begin();
      m_data            = static_cast<data::data_specification>(*i++);
      m_action_labels   = atermpp::down_cast<process::action_label_list>((*i++)[0]);

      atermpp::aterm_list rules_list = atermpp::down_cast<atermpp::aterm_list>((*i)[0]);
      for (const atermpp::aterm& r: rules_list)
      {
        m_rules.emplace_back(r);
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
atermpp::aterm action_rename_rule_to_aterm(const action_rename_rule& rule)
{
  return atermpp::aterm(core::detail::function_symbol_ActionRenameRule(), rule.variables(), rule.condition(), rule.lhs(), rule.rhs());
}

inline
atermpp::aterm action_rename_specification_to_aterm(const action_rename_specification& spec)
{
  std::vector<atermpp::aterm> rules;
  for (const action_rename_rule& r: spec.rules())
  {
    rules.push_back(action_rename_rule_to_aterm(r));
  }
  return atermpp::aterm(core::detail::function_symbol_ActionRenameSpec(),
    data::detail::data_specification_to_aterm(spec.data()),
    atermpp::aterm(core::detail::function_symbol_ActSpec(), spec.action_labels()),
    atermpp::aterm(core::detail::function_symbol_ActionRenameRules(), atermpp::aterm_list(rules.begin(), rules.end()))
  );
}

}

namespace mcrl2::lps
{

/// \cond INTERNAL_DOCS
namespace detail
{
// Put the equalities t==u in the replacement map as u:=t.
inline void fill_replacement_map(const data::data_expression& equalities_in_conjunction, 
                                 std::map<data::data_expression, data::data_expression>& replacement_map)
{
  if (equalities_in_conjunction==data::sort_bool::true_())
  {
    return;
  }
  if (data::sort_bool::is_and_application(equalities_in_conjunction))
  {
    fill_replacement_map(data::sort_bool::left(equalities_in_conjunction),replacement_map);
    fill_replacement_map(data::sort_bool::right(equalities_in_conjunction),replacement_map);
    return;
  }
  if(is_equal_to_application(equalities_in_conjunction))
  { 
    const data::application a=atermpp::down_cast<data::application>(equalities_in_conjunction);
    if (a[1]!=a[0])
    {
      replacement_map[a[1]]=a[0];
    }
  }
}

// Replace expressions in e according to the replacement map.
// Assume that e only consists of and, not and equality applied to terms. 
inline data::data_expression replace_expressions(const data::data_expression& e, 
                                    const std::map<data::data_expression, data::data_expression>& replacement_map)
{
  if (data::sort_bool::is_and_application(e))
  {
    return data::sort_bool::and_(replace_expressions(data::sort_bool::left(e),replacement_map),
                           replace_expressions(data::sort_bool::right(e),replacement_map));
  }
  if (data::sort_bool::is_not_application(e))
  {
    return data::sort_bool::not_(replace_expressions(data::sort_bool::arg(e),replacement_map));
  }
  if (is_equal_to_application(e))
  {
    const data::application a=atermpp::down_cast<data::application>(e);
    return data::application(a.head(),
                       replace_expressions(a[0],replacement_map),
                       replace_expressions(a[1],replacement_map));
  }
  const std::map<data::data_expression, data::data_expression>::const_iterator i=replacement_map.find(e);
  if (i!=replacement_map.end()) // found;
  {
    return i->second;
  }
  return e;
}

// Substitute the equalities in equalities_in_conjunction from right to left in e. 
inline data::data_expression substitute_equalities(const data::data_expression& e, const data::data_expression& equalities_in_conjunction)
{
  std::map<data::data_expression, data::data_expression> replacement_map;
  fill_replacement_map(equalities_in_conjunction, replacement_map);
  return replace_expressions(e,replacement_map);
}

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

  for (const data::variable& v: new_vars)
  {
    mcrl2::core::identifier_string new_name = generator(std::string(v.name()));

    if (new_name != v.name())
    {
      renamings[v] = data::variable(new_name, v.sort());
    }
  }

  data::set_identifier_generator id_generator;
  for (const data::variable& v: data::substitution_variables(renamings))
  {
    id_generator.add_identifier(v.name());
  }
  rcond = data::replace_variables_capture_avoiding(rcond, renamings, id_generator);
  rleft = process::replace_variables_capture_avoiding(rleft, renamings, id_generator);
  rright = process::replace_variables_capture_avoiding(rright, renamings, id_generator);
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
/// \param  rewr A data rewriter.
/// \return The lps_old_spec where all actions have been renamed according
///         to action_rename_spec.
inline
lps::stochastic_specification action_rename(
  const action_rename_specification& action_rename_spec,
  const lps::stochastic_specification& lps_old_spec,
  const data::rewriter& rewr,
  const bool enable_rewriting)
{
  using namespace mcrl2::core;
  using namespace mcrl2::data;
  using namespace mcrl2::lps;

  const std::vector <action_rename_rule>& rename_rules = action_rename_spec.rules();
  stochastic_action_summand_vector lps_old_action_summands = lps_old_spec.process().action_summands();
  deadlock_summand_vector lps_deadlock_summands = lps_old_spec.process().deadlock_summands();
  process::action_list lps_new_actions;

  data::set_identifier_generator generator;
  generator.add_identifiers(lps::find_identifiers(lps_old_spec));
  generator.add_identifiers(data::function_and_mapping_identifiers(lps_old_spec.data()));

  //go through the rename rules of the rename file
  mCRL2log(log::debug) << "Rename rules found: " << rename_rules.size() << "\n";
  for (const action_rename_rule& r: rename_rules)
  {
    stochastic_action_summand_vector lps_new_action_summands;

    data_expression rule_condition = r.condition();
    process::action rule_old_action = r.lhs();
    process::action rule_new_action;
    process::process_expression new_element = r.rhs();
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

    for (const data_expression& rule_old_argument_i: rule_old_action.arguments())
    {
      if (!is_variable(rule_old_argument_i) &&
          (!(data::find_all_variables(rule_old_argument_i).empty())))
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
    for (const data_expression& d: rule_old_action.arguments())
    {
      if (is_variable(d))
      {
        const variable& v = atermpp::down_cast<variable>(d);
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
      std::vector < data_expression > lps_new_condition(1,lps_old_action_summand.condition());
      std::vector < process::action_list >
      lps_new_actions(1,process::action_list());
      std::vector < bool > lps_new_actions_is_delta(1,false);

      mCRL2log(log::debug) << "Actions in summand found: " << lps_old_actions.size() << "\n";
      for (const process::action& lps_old_action: lps_old_actions)
      {
        if (equal_signatures(lps_old_action, rule_old_action))
        {
          mCRL2log(log::debug) << "Renaming action " << rule_old_action << "\n";

          //rename all previously used variables.
          data_expression renamed_rule_condition=rule_condition;
          process::action renamed_rule_old_action=rule_old_action;
          process::action renamed_rule_new_action=rule_new_action;
          detail::rename_renamerule_variables(renamed_rule_condition, renamed_rule_old_action, renamed_rule_new_action, generator);

          //go through the arguments of the action.
          data_expression_list::iterator lps_old_argument_i = lps_old_action.arguments().begin();
          data_expression new_equalities_condition=sort_bool::true_();
          for (const data_expression& rule_old_argument_i: renamed_rule_old_action.arguments())
          {
            if (is_variable(rule_old_argument_i))
            {
              new_equalities_condition=lazy::and_(new_equalities_condition,
                                                  data::equal_to(rule_old_argument_i, *lps_old_argument_i));
            }
            else
            {
              assert((data::find_all_variables(rule_old_argument_i).empty())); // the argument must be closed, which is checked above.
              renamed_rule_condition=
                  lazy::and_(renamed_rule_condition,
                           data::equal_to(rule_old_argument_i, *lps_old_argument_i));
              if (enable_rewriting)
              {
                renamed_rule_condition
                    = rewr(renamed_rule_condition); // Make sure that renamed_rule_condition is as simple as possible.
              }
            }
            lps_old_argument_i++;
          }

          /* insert the new equality condition in all the newly generated summands */
          for (data_expression& d: lps_new_condition)
          {
            d=lazy::and_(d,new_equalities_condition);
          }

          /* insert the new sum variables in all the newly generated summands */
          std::set<variable> new_vars = find_all_variables(renamed_rule_old_action);
          for (const variable& sdvi: new_vars)
          {
            for (variable_list& l: lps_new_sum_vars)
            {
              l.push_front(sdvi);
            }
          }

          if (enable_rewriting)
          {
            renamed_rule_condition
                = rewr(renamed_rule_condition); // Make sure that renamed_rule_condition is as simple as possible.
          }
          if (renamed_rule_condition==sort_bool::true_())
          {
            if (to_delta)
            {
              std::vector < bool >::iterator i_is_delta=lps_new_actions_is_delta.begin();
              for (process::action_list& l: lps_new_actions)
              {
                l=process::action_list(); // the action becomes delta
                *i_is_delta=true;
                ++i_is_delta;
              }
            }
            else if (!to_tau)
            {
              std::vector < bool >::iterator i_is_delta=lps_new_actions_is_delta.begin();
              for (process::action_list& l: lps_new_actions)
              {
                if (!*i_is_delta) // the action is not delta
                {
                  l.push_front(renamed_rule_new_action);
                }
                ++i_is_delta;
              }
            }
          }
          else if (renamed_rule_condition==sort_bool::false_())
          {
            std::vector < bool >::iterator i_is_delta=lps_new_actions_is_delta.begin();
            for (process::action_list& l: lps_new_actions)
            {
              if (!*i_is_delta) // The action does not equal delta.
              {
                l.push_front(lps_old_action);
              }
              ++i_is_delta;
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
              for (process::action_list& l: lps_new_actions)
              {
                if (to_delta)
                {
                  l=process::action_list();
                  *i_is_delta=true;
                }
                else
                {
                  l.push_front(renamed_rule_new_action);
                  *i_is_delta=false;
                }
                ++i_is_delta;
              }
            }

            for (process::action_list& l: lps_new_actions_temp)
            {
              lps_new_actions_is_delta.push_back(false); // A non renamed action is not delta;
              l.push_front(lps_old_action);
            }

            lps_new_actions.insert(lps_new_actions.end(),
                                   lps_new_actions_temp.begin(),
                                   lps_new_actions_temp.end());
            assert(lps_new_actions_is_delta.size()==lps_new_actions.size());

            /* lps_new_condition_temp will contain the conditions in conjunction with
               the negated new_condition. It will be concatenated to lps_new_condition,
               in which the terms will be conjoined with the non-negated new_condition */

            std::vector < data_expression > lps_new_condition_temp(lps_new_condition);

            for (data_expression& d: lps_new_condition)
            {
              // substitute the equalities in d in renamed_rule_condition. 
              d=lazy::and_(renamed_rule_condition,detail::substitute_equalities(d,renamed_rule_condition));
            }

            for (const data_expression& d: lps_new_condition_temp)
            {

              lps_new_condition.push_back(lazy::and_(d,sort_bool::not_(renamed_rule_condition)));
            }

            // Replace lps_new_sum_vars with two consecutive copies of itself. 
            // The clumsily looking method below is required, to avoid problems with vector reallocation.
            std::size_t size=lps_new_sum_vars.size();
            lps_new_sum_vars.reserve(2*size);
            for(std::size_t i=0; i<size; ++i)
            {
              lps_new_sum_vars.push_back(lps_new_sum_vars[i]);
            }
          }
        }//end if(equal_signatures(...))
        else
        {
          for (process::action_list& l: lps_new_actions)
          {
            l.push_front(lps_old_action);
          }
        }
        mCRL2log(log::debug) << "Action done\n";

      } //end of action list iterator

      /* Add the summands to lps_new_action_summands or to the deadlock summands*/

      std::vector < process::action_list > :: iterator i_act=lps_new_actions.begin();
      std::vector < bool > :: iterator i_act_is_delta=lps_new_actions_is_delta.begin();
      std::vector < variable_list > :: iterator i_sumvars=lps_new_sum_vars.begin();
      for (const data_expression& cond: lps_new_condition)
      {
        //create a summand for the new lps
        if (*i_act_is_delta)
        {
          // Create a deadlock summand.
          const deadlock_summand d(*i_sumvars,
                                   cond,
                                   deadlock(lps_old_action_summand.multi_action().time()));
          lps_deadlock_summands.push_back(d);
        }
        else
        {
          // create an action summand.
          stochastic_action_summand lps_new_summand(*i_sumvars,
                                                    cond,
                                                    multi_action(reverse(*i_act), lps_old_action_summand.multi_action().time()),
                                                    lps_old_action_summand.assignments(),
                                                    lps_old_action_summand.distribution());
          lps_new_action_summands.push_back(lps_new_summand);
        }
        i_act++;
        i_sumvars++;
        ++i_act_is_delta;
      }
    } // end of summand list iterator
    lps_old_action_summands = lps_new_action_summands;
  } //end of rename rule iterator

  mCRL2log(log::debug) << "Simplifying the result...\n";

  stochastic_linear_process new_process(lps_old_spec.process().process_parameters(),
                                        lps_deadlock_summands,
                                        lps_old_action_summands);

  // add action_rename_spec.action_labels to action_rename_spec.action_labels without adding duplicates.
  process::action_label_list all=action_rename_spec.action_labels();
  for (const process::action_label& a: lps_old_spec.action_labels())
  {
    if (std::find(action_rename_spec.action_labels().begin(),
                  action_rename_spec.action_labels().end(),a)==action_rename_spec.action_labels().end())
    {
      // Not found;
      all.push_front(a);
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

namespace detail
{

inline
process::action_label rename_action_label(const process::action_label& act, const std::regex& matching_regex, const std::string& replacing_fmt)
{
  return process::action_label(std::regex_replace(std::string(act.name()), matching_regex, replacing_fmt), act.sorts());
}

} // namespace detail

/**
 * \brief Rename actions in given specification based on a regular expression and
 * a string that specifies how the replacement should be formatted.
 */
inline
stochastic_specification action_rename(
  const std::regex& matching_regex,
  const std::string& replacing_fmt,
  const stochastic_specification& lps_old_spec)
{
  // Use a set to store the new action labels to avoid duplicates
  std::set<process::action_label> new_actions_set;
  process::action_label_list new_actions;
  for(const process::action_label& act: lps_old_spec.action_labels())
  {
    process::action_label new_action_label(detail::rename_action_label(act, matching_regex, replacing_fmt));
    if (std::string(new_action_label.name()).empty())
    {
      throw mcrl2::runtime_error("After renaming the action " + std::string(act.name()) + " becomes empty, which is not allowed.");
    }
    if(std::string(new_action_label.name()) != "delta" && std::string(new_action_label.name()) != "tau" &&
        new_actions_set.find(new_action_label) == new_actions_set.end())
    {
      // The list of actions should not contain delta and tau actions and also no duplicates.
      new_actions_set.insert(new_action_label);
      new_actions.push_front(new_action_label);
    }
  }
  new_actions = reverse(new_actions);

  // The list of new actions summands is initially empty
  std::vector<stochastic_action_summand> new_action_summands;
  // The list of new deadlock summands is initialised to the existing list, we will only add new deadlock summands
  std::vector<deadlock_summand> new_deadlock_summands(lps_old_spec.process().deadlock_summands());
  for(const stochastic_action_summand& as: lps_old_spec.process().action_summands())
  {
    process::action_list new_action_list;
    bool becomes_deadlock_summand = false;
    for(const process::action& act: as.multi_action().actions())
    {
      process::action_label new_action_label(detail::rename_action_label(act.label(), matching_regex, replacing_fmt));
      if(std::string(new_action_label.name()) == "delta")
      {
        // delta is the absorbing element for multi action concatenation
        // Therefore, this summand now becomes a deadlock summand
        becomes_deadlock_summand = true;
        break;
      }
      // tau is the identity for multi action concatenation
      // therefore, we should not add it to a multi action
      if(std::string(new_action_label.name()) != "tau")
      {
        new_action_list.push_front(process::action(new_action_label, act.arguments()));
      }
    }

    if(!becomes_deadlock_summand)
    {
      new_action_list = reverse(new_action_list);
      multi_action new_multi_action(new_action_list, as.multi_action().time());

      // Copy most of the old information, only the multi action has changed
      stochastic_action_summand new_summand(as.summation_variables(), as.condition(), new_multi_action, as.assignments(), as.distribution());
      new_action_summands.push_back(new_summand);
    }
    else
    {
      // Add a new deadlock summand, copying most of the information for the old action summand
      new_deadlock_summands.emplace_back(as.summation_variables(), as.condition(), deadlock(as.multi_action().time()));
    }
  }

  stochastic_linear_process new_process(lps_old_spec.process().process_parameters(),
                                        new_deadlock_summands,
                                        new_action_summands);
  stochastic_specification lps_new_spec(lps_old_spec.data(),
                                        new_actions,
                                        lps_old_spec.global_variables(),
                                        new_process,
                                        lps_old_spec.initial_process());
  return lps_new_spec;
}

} // namespace mcrl2::lps

namespace std
{
/// \brief Output an action_rename_rule to ostream. 
/// \param out An output stream
/// \return The output stream
// Currently, the variables are not printed. The shape is also not parseable. This may be mended. 
inline
std::ostream& operator<<(std::ostream& out, const mcrl2::lps::action_rename_rule& r)
{
  return out << "(" << r.condition() << ") -> " << r.lhs() << " => " << r.rhs();
}

/// \brief Output a action_rename_rule to ostream. 
/// \param out An output stream
/// \return The output stream
// Currently, the data declaration and the action declaration are not printed. 
inline
std::ostream& operator<<(std::ostream& out, const mcrl2::lps::action_rename_specification& s)
{
  for(const mcrl2::lps::action_rename_rule& r: s.rules())
  {
    out << r << "\n";
  }
  return out;
}


}  // end namespace std


#endif // MCRL2_LPS_ACTION_RENAME_H
