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

#include "mcrl2/exception.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/core/parse.h"
#include "mcrl2/core/typecheck.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/substitution.h"
#include "mcrl2/data/postfix_identifier_generator.h"
#include "mcrl2/data/detail/internal_format_conversion.h"
#include "mcrl2/lps/detail/algorithm.h"

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

namespace mcrl2 {

namespace lps {

  /// \brief Right hand side of an action rename rule
  class action_rename_rule_rhs: public atermpp::aterm_appl
  {
    public:
      /// \brief Constructor.
      action_rename_rule_rhs()
        : atermpp::aterm_appl(core::detail::constructActionRenameRuleRHS())
      { }

      /// \brief Constructor.
      /// \param t A term
      action_rename_rule_rhs(atermpp::aterm_appl t)
        : atermpp::aterm_appl(t)
      {
        assert(core::detail::check_rule_ActionRenameRuleRHS(m_term));
      }

      /// \brief Returns true if the right hand side is equal to delta.
      /// \return True if the right hand side is equal to delta.
      bool is_delta() const
      {
        return core::detail::gsIsDelta(*this);
      }

      /// \brief Returns true if the right hand side is equal to tau.
      /// \return True if the right hand side is equal to tau.
      bool is_tau() const
      {
        return core::detail::gsIsTau(*this);
      }

      /// \brief Returns the action.
      /// \pre The right hand side must be an action
      /// \return The action.
      action act() const
      {
        return *this;
      }
  };

//                ::= ActionRenameRule(<DataVarId>*, <DataExprOrNil>,
//                      <ParamIdOrAction>, <ActionRenameRuleRHS>)

  /// \brief Action rename rule
  class action_rename_rule: public atermpp::aterm_appl
  {
    protected:
      /// \brief The data variables of the rule
      data::variable_list m_variables;

      /// \brief The condition of the rule
      data::data_expression    m_condition;

      /// \brief The left hand side of the rule
      action                   m_lhs;

      /// \brief right hand side of the rule
      action_rename_rule_rhs   m_rhs;

      /// \brief Initialize the action rename rule with an aterm_appl.
      /// \param t A term
      void init_term(atermpp::aterm_appl t)
      {
        m_term = atermpp::aterm_traits<atermpp::aterm_appl>::term(t);
        atermpp::aterm_appl::iterator i = t.begin();
        m_variables       = atermpp::aterm_list(*i++);
        m_condition       = atermpp::aterm_appl(*i++);
        m_lhs             = atermpp::aterm_appl(*i++);
        m_rhs             = atermpp::aterm_appl(*i);
      }

    public:
      /// \brief Constructor.
      action_rename_rule()
        : atermpp::aterm_appl(core::detail::constructActionRenameRule())
      { }

      /// \brief Constructor.
      /// \param t A term
      action_rename_rule(atermpp::aterm_appl t)
        : atermpp::aterm_appl(t)
      {
        assert(core::detail::check_rule_ActionRenameRule(m_term));
        init_term(t);
      }

      /// \brief Returns the variables of the rule.
      /// \return The variables of the rule.
      data::variable_list variables() const
      {
        return m_variables;
      }

      /// \brief Returns the condition of the rule.
      /// \return The condition of the rule.
      data::data_expression condition() const
      {
        return m_condition;
      }

      /// \brief Returns the left hand side of the rule.
      /// \return The left hand side of the rule.
      action lhs() const
      {
        return m_lhs;
      }

      /// \brief Returns the right hand side of the rule.
      /// \return The right hand side of the rule.
      action_rename_rule_rhs rhs() const
      {
        return m_rhs;
      }
  };

  /// \brief Read-only singly linked list of action rename rules
  typedef atermpp::term_list<action_rename_rule> action_rename_rule_list;

  /// \brief Action rename specification
  class action_rename_specification: public atermpp::aterm_appl
  {
    protected:

      /// \brief The data specification of the action rename specification
      data::data_specification m_data;

      /// \brief The action labels of the action rename specification
      action_label_list        m_action_labels;

      /// \brief The action rename rules of the action rename specification
      action_rename_rule_list  m_rules;

      /// \brief Initialize the action_rename_specification with an aterm_appl.
      /// \param t A term
      void init_term(atermpp::aterm_appl t)
      {
        m_term = atermpp::aterm_traits<atermpp::aterm_appl>::term(t);
        atermpp::aterm_appl::iterator i = t.begin();
        m_data            = atermpp::aterm_appl(*i++);
        m_action_labels   = atermpp::aterm_appl(*i++)(0);
        m_rules           = atermpp::aterm_appl(*i)(0);
      }

    public:
      /// \brief Constructor.
      action_rename_specification()
        : atermpp::aterm_appl(core::detail::constructActionRenameSpec())
      { }

      /// \brief Constructor.
      /// \param t A term
      action_rename_specification(atermpp::aterm_appl t)
        : atermpp::aterm_appl(t)
      {
        assert(core::detail::check_rule_ActionRenameSpec(m_term));
        init_term(t);
      }

      /// \brief Constructor.
      /// \param data A data specification
      /// \param action_labels A sequence of action labels
      /// \param rules A sequence of action rename rules
      action_rename_specification(data::data_specification const& data, action_label_list action_labels, action_rename_rule_list rules)
        :
          m_data(data),
          m_action_labels(action_labels),
          m_rules(rules)
      {
          m_term = reinterpret_cast<ATerm>(
          core::detail::gsMakeActionRenameSpec(
            data::detail::data_specification_to_aterm_data_spec(data, true),
            core::detail::gsMakeActSpec(action_labels),
            core::detail::gsMakeActionRenameRules(rules)
          )
        );
      }

      /// \brief Reads the action rename specification from file.
      /// \param filename A string
      /// If filename is nonempty, input is read from the file named filename.
      /// If filename is empty, input is read from stdin.
      void load(const std::string& filename)
      {
        atermpp::aterm t = core::detail::load_aterm(filename);
        if (!t || t.type() != AT_APPL || !core::detail::gsIsActionRenameSpec(atermpp::aterm_appl(t)))
        {
          throw runtime_error(((filename.empty())?"stdin":("'" + filename + "'")) + " does not contain an action rename specification");
        }
        init_term(atermpp::aterm_appl(t));
        if (!is_well_typed())
        {
          throw runtime_error("action rename specification is not well typed (action_rename_specification::load())");
        }
      }

      /// \brief Writes the action rename specification to file.
      /// \param filename A string
      /// If filename is nonempty, output is written to the file named filename.
      /// If filename is empty, output is written to stdout.
      /// \param binary
      /// If binary is true the linear process is saved in compressed binary format.
      /// Otherwise an ascii representation is saved. In general the binary format is
      /// much more compact than the ascii representation.
      void save(const std::string& filename, bool binary = true)
      {
        if (!is_well_typed())
        {
          throw runtime_error("action rename specification is not well typed (action_rename_specification::save())");
        }
        core::detail::save_aterm(m_term, filename, binary);
      }

      /// \brief Returns the data action_rename_specification.
      /// \return The data action_rename_specification.
      data::data_specification data() const
      { return m_data; }

      /// \brief Returns the sequence of action labels
      /// \return A sequence of action labels containing all action
      /// labels occurring in the action_rename_specification (but it can have more).
      action_label_list action_labels() const
      { return m_action_labels; }

      /// \brief Returns the action rename rules.
      /// \return The action rename rules.
      action_rename_rule_list rules() const
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

/// \cond INTERNAL_DOCS
  namespace detail {
    /// \brief Reads an action rename specification from a stream
    /// \param from An input stream
    /// \return A term in an undocumented format
    inline
    ATermAppl parse_action_rename_specification(std::istream& from)
    {
      ATermAppl result = core::parse_action_rename_spec(from);
      if (result == NULL)
         throw runtime_error("parse error");
      return result;
    }

    /// \brief Type checks an action rename specification
    /// \param ar_spec A term
    /// \param spec A term
    /// \return A term in an undocumented format
    inline
    ATermAppl type_check_action_rename_specification(ATermAppl ar_spec, ATermAppl spec)
    {
      ATermAppl result = core::type_check_action_rename_spec(ar_spec, spec);
      if (result == NULL)
        throw runtime_error("type check error");
      return result;
    }

    /// \brief Renames variables
    /// \param rcond A data expression
    /// \param rleft An action
    /// \param rright An action
    /// \param generator A generator for fresh identifiers
    template <typename IdentifierGenerator>
    void rename_renamerule_variables(data::data_expression& rcond, lps::action& rleft, lps::action& rright, IdentifierGenerator& generator)
    {
      atermpp::map< data::variable, data::variable > renamings;

      std::set< data::variable > new_vars = data::find_variables(rleft.arguments());

      for (std::set< data::variable >::const_iterator i = new_vars.begin(); i != new_vars.end(); ++i)
      {
        mcrl2::core::identifier_string new_name = generator(i->name());

        if (new_name != i->name())
        {
          renamings[*i] = data::variable(new_name, i->sort());
        }
      }

      lps::substitute(rcond, mcrl2::data::make_map_substitution_adapter(renamings));
      lps::substitute(rleft, mcrl2::data::make_map_substitution_adapter(renamings));
      lps::substitute(rright, mcrl2::data::make_map_substitution_adapter(renamings));
    }

  } // namespace detail
  /// \endcond

  /// \brief Parses an action rename specification.
  /// Parses an acion rename specification.
  /// If the action rename specification contains data types that are not
  /// present in the data specification of \p spec they are added to it.
  /// \param in An input stream
  /// \param spec A linear process specification
  /// \return An action rename specification
  inline
  action_rename_specification parse_action_rename_specification(
                                 std::istream& in,
                                 lps::specification const& spec)
  {
    //std::istringstream in(text);
    ATermAppl result = detail::parse_action_rename_specification(in);
    lps::specification copy_specification(spec);
    /* copy_specification.data() = mcrl2::data::remove_all_system_defined(spec.data()); */
    ATermAppl lps_spec = specification_to_aterm(copy_specification);
    result           = detail::type_check_action_rename_specification(result, lps_spec);
    result           = data::detail::internal_format_conversion(spec.data(), result);
    return action_rename_specification(result);
  }


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
lps::specification action_rename(
            const action_rename_specification &action_rename_spec,
            const lps::specification &lps_old_spec)
{
  using namespace mcrl2::core;
  using namespace mcrl2::data;
  using namespace mcrl2::lps;
  using namespace std;

  action_rename_rule_list rename_rules = action_rename_spec.rules();
  summand_list lps_old_summands = lps_old_spec.process().summands();
  summand_list lps_summands; //for changes in lps_old_summands
  action_list lps_new_actions;

  data::postfix_identifier_generator generator("");
  generator.add_to_context(specification_to_aterm(lps_old_spec));

  bool to_tau=false;
  bool to_delta=false;

  //go through the rename rules of the rename file
  if (gsDebug)
  { std::cerr << "Rename rules found: " << rename_rules.size() << "\n";
  }
  for(action_rename_rule_list::iterator i = rename_rules.begin(); i != rename_rules.end(); ++i)
  {
    summand_list lps_new_summands;

    data_expression rule_condition = i->condition();
    action rule_old_action =  i->lhs();
    action rule_new_action;
    action_rename_rule_rhs new_element = i->rhs();

    if(is_action(new_element))
    {
      rule_new_action =  action(new_element);
      to_tau = false;
      to_delta = false;
    }
    else
    {
      rule_new_action = action();
      if(mcrl2::core::detail::gsIsTau(new_element)){ to_tau = true; to_delta = false;}
      else if (mcrl2::core::detail::gsIsDelta(new_element)){ to_tau = false; to_delta = true;}
    }

    // Check here that the arguments of the rule_old_action only consist
    // of uniquely occurring variables or closed terms. Furthermore, check that the variables
    // in rule_new_action and in rule_condition are a subset of those in
    // rule_old_action. This check ought to be done in the static checking
    // part of the renaming rules, but as yet it has nog been done. Ultimately
    // this check should be moved there.

    // first check that the arguments of rule_old_action are variables or closed
    // terms.

    for(data_expression_list::iterator
                       rule_old_argument_i = rule_old_action.arguments().begin();
                       rule_old_argument_i != rule_old_action.arguments().end();
                       rule_old_argument_i++)
    { if ((!rule_old_argument_i->is_variable()) &&
          (!(data::find_variables(*rule_old_argument_i).empty())))
      { throw mcrl2::runtime_error("The arguments of the lhs " + core::pp(rule_old_action) +
                          " are not variables or closed expressions");
      }
    }

    // Check whether the variables in rhs are included in the lefthandside.
    std::set < variable > variables_in_old_rule = lps::find_free_variables(rule_old_action);
    std::set < variable > variables_in_new_rule = lps::find_free_variables(rule_new_action);

    if (!includes(variables_in_old_rule.begin(),variables_in_old_rule.end(),
                  variables_in_new_rule.begin(),variables_in_new_rule.end()))
    { throw mcrl2::runtime_error("There are variables occurring in rhs " + core::pp(rule_new_action) +
                   " of a rename rule not occurring in lhs " + core::pp(rule_old_action));
    }

    // Check whether the variables in condition are included in the lefthandside.
    std::set < variable > variables_in_condition = data::find_free_variables(rule_condition);
    if (!includes(variables_in_old_rule.begin(),variables_in_old_rule.end(),
                  variables_in_condition.begin(),variables_in_condition.end()))
    { throw mcrl2::runtime_error("There are variables occurring in the condition " + core::pp(rule_condition) +
                   " of a rename rule not occurring in lhs " + core::pp(rule_old_action));
    }

    // check for double occurrences of variables in the lhs. Note that variables_in_old_rule
    // is empty at the end.
    for(data_expression_list::iterator i=rule_old_action.arguments().begin() ;
                     i!=rule_old_action.arguments().end() ; i++)
    { if (i->is_variable())
      { if (variables_in_old_rule.find(*i)==variables_in_old_rule.end())
        { throw mcrl2::runtime_error("Variable " + core::pp(*i) + " occurs more than once in lhs " +
                       core::pp(rule_old_action) + " of an action rename rule");
        }
        else
        { variables_in_old_rule.erase(*i);
        }
      }
    }
    assert(variables_in_old_rule.empty());


    lps_summands = summand_list();
    //go through the summands of the old lps
    if (gsDebug)
    { std::cerr << "Summands found: " << lps_old_summands.size() << "\n";
    }
    for(summand_list::iterator losi = lps_old_summands.begin();
                                    losi != lps_old_summands.end(); ++losi)
    {
      summand lps_old_summand = *losi;
      action_list lps_old_actions = lps_old_summand.actions();

      /* For each individual action in the multi-action, for which the
         rename rule applies, two new summands must be made, namely one
         where the rule does not match with the parameters of the action,
         and one where it actually does. This means that for a multiaction
         with k summands 2^k new summands can result. */

      atermpp::vector < variable_list >
                           lps_new_sum_vars(1,lps_old_summand.summation_variables());
      atermpp::vector < data_expression >
                         lps_new_condition(1,lps_old_summand.condition());
      atermpp::vector < std::pair <bool, action_list > >
                           lps_new_actions(1,std::make_pair(lps_old_summand.is_delta(),action_list()));

      if (gsDebug)
      { std::cerr << "Actions in summand found: " << lps_old_actions.size() << "\n";
      }
      for(action_list::iterator loai = lps_old_actions.begin();
                loai != lps_old_actions.end(); loai++)
      {
        action lps_old_action = *loai;
        //std::cerr << "Considering " << lps_old_action << "\nand " << rule_old_action << "\n";
        if (equal_signatures(lps_old_action, rule_old_action))
        {
          if (gsDebug)
          { std::cerr << "Renaming action " << core::pp(rule_old_action) << "\n";
          }

          //rename all previously used variables
          data_expression renamed_rule_condition=rule_condition;
          action renamed_rule_old_action=rule_old_action;
          action renamed_rule_new_action=rule_new_action;
          detail::rename_renamerule_variables(renamed_rule_condition, renamed_rule_old_action, renamed_rule_new_action, generator);

          /*
          if (is_nil(renamed_rule_condition))
          { renamed_rule_condition=sort_bool::true_();
          }
          */ // JK 15/10/2009 condition is always a data expression

          //go through the arguments of the action
          data_expression_list::iterator
                    lps_old_argument_i = lps_old_action.arguments().begin();
          data_expression new_equalities_condition=sort_bool::true_();
          for(data_expression_list::iterator
                       rule_old_argument_i = renamed_rule_old_action.arguments().begin();
                       rule_old_argument_i != renamed_rule_old_action.arguments().end();
                       rule_old_argument_i++)
          { if (rule_old_argument_i->is_variable())
            {
              new_equalities_condition=lazy::and_(new_equalities_condition,
                               data::equal_to(*rule_old_argument_i, *lps_old_argument_i));
            }
            else
            { assert((data::find_variables(*rule_old_argument_i).empty())); // the argument must be closed,
                                                                               // which is checked above.
              renamed_rule_condition=
                        lazy::and_(renamed_rule_condition,
                             data::equal_to(*rule_old_argument_i, *lps_old_argument_i));
            }
            lps_old_argument_i++;
          }

          /* insert the new equality condition in all the newly generated summands */
          for (atermpp::vector < data_expression > :: iterator i=lps_new_condition.begin() ;
                       i!=lps_new_condition.end() ; i++ )
          { *i=lazy::and_(*i,new_equalities_condition);
          }

          /* insert the new sum variables in all the newly generated summands */
          std::set<variable> new_vars = find_variables(renamed_rule_old_action);
          for(std::set<variable>::iterator sdvi = new_vars.begin();
                         sdvi != new_vars.end(); sdvi++)
          {
            for ( atermpp::vector < variable_list > :: iterator i=lps_new_sum_vars.begin() ;
                        i!=lps_new_sum_vars.end() ; i++ )
            { *i = push_front(*i, *sdvi);
            }
          }

          if (renamed_rule_condition==sort_bool::true_())
          {
            if (to_delta)
            { for(atermpp::vector < std::pair <bool, action_list > > :: iterator
                      i=lps_new_actions.begin() ;
                      i!=lps_new_actions.end() ; i++ )
              { *i=std::make_pair(true,action_list()); /* the action becomes delta */
              }
            }
            else if (!to_tau)
            { for(atermpp::vector < std::pair <bool, action_list > > :: iterator i=lps_new_actions.begin() ;
                        i!=lps_new_actions.end() ; i++ )
              { if (!((*i).first)) // the action is not delta
                { *i=std::make_pair(false,push_front((*i).second,renamed_rule_new_action));
                }
              }
            }
          }
          else if (renamed_rule_condition==sort_bool::false_())
          {
            for(atermpp::vector < std::pair <bool, action_list > > :: iterator i=lps_new_actions.begin() ;
                        i!=lps_new_actions.end() ; i++ )
            { if (!((*i).first)) // The action does not equal delta.
              { *i=std::make_pair(false,push_front((*i).second,lps_old_action));
              }
            }

          }
          else
          { /* Duplicate summands, one where the renaming is applied, and one where it is not
               applied. */

            atermpp::vector < std::pair <bool, action_list > > lps_new_actions_temp(lps_new_actions);

            if (!to_tau) // if the new element is tau, we do not insert it in the multi-action.
            { for(atermpp::vector < std::pair <bool, action_list > > :: iterator
                        i=lps_new_actions.begin() ;
                        i!=lps_new_actions.end() ; i++ )
              { if (to_delta)
                { *i=std::make_pair(true,action_list());
                }
                else
                { *i=std::make_pair(false,push_front(i->second,renamed_rule_new_action));
                }
              }
            }

            for(atermpp::vector < std::pair <bool, action_list > > :: iterator
                        i=lps_new_actions_temp.begin() ;
                        i!=lps_new_actions_temp.end() ; i++ )
            { if (!(i->first)) // The element is not equal to delta
              { *i=std::make_pair(false,push_front(i->second,lps_old_action));
              }
            }

            lps_new_actions.insert(lps_new_actions.end(),
                                   lps_new_actions_temp.begin(),
                                   lps_new_actions_temp.end());


            /* lps_new_condition_temp will contain the conditions in conjunction with
               the negated new_condition. It will be concatenated to lps_new_condition,
               in which the terms will be conjoined with the non-negated new_condition */

            atermpp::vector < data_expression > lps_new_condition_temp(lps_new_condition);

            for (atermpp::vector < data_expression > :: iterator i=lps_new_condition.begin() ;
                         i!=lps_new_condition.end() ; i++ )
            { *i=lazy::and_(*i,renamed_rule_condition);
            }

            for (atermpp::vector < data_expression > :: iterator i=lps_new_condition_temp.begin() ;
                         i!=lps_new_condition_temp.end() ; i++ )
            { *i=lazy::and_(*i,sort_bool::not_(renamed_rule_condition));
            }

            lps_new_condition.insert(lps_new_condition.end(),
                                     lps_new_condition_temp.begin(),
                                     lps_new_condition_temp.end());

            atermpp::vector < variable_list > lps_new_sum_vars_temp(lps_new_sum_vars);
            lps_new_sum_vars.insert(lps_new_sum_vars.end(),
                                    lps_new_sum_vars_temp.begin(),
                                    lps_new_sum_vars_temp.end());
          }

        }//end if(equal_signatures(...))
        else
        { for ( atermpp::vector < std::pair <bool, action_list > > :: iterator i=lps_new_actions.begin() ;
                i!=lps_new_actions.end() ; i++ )
          { *i = std::make_pair((*i).first,push_front((*i).second, lps_old_action));
          }
        }
        if (gsDebug)
        { std::cerr << "Action done\n";
        }

      } //end of action list iterator

      /* Add the summands to lps_new_summands */

      atermpp::vector < std::pair <bool, action_list > > :: iterator i_act=lps_new_actions.begin();
      atermpp::vector < variable_list > :: iterator i_sumvars=lps_new_sum_vars.begin();
      for( atermpp::vector < data_expression > :: iterator i_cond=lps_new_condition.begin() ;
           i_cond!=lps_new_condition.end() ; i_cond++)
      { //create a summand for the new lps
        summand lps_new_summand = summand( *i_sumvars,
                                           *i_cond,
                                           (*i_act).first,
                                           reverse((*i_act).second),
                                           lps_old_summand.time(),
                                           lps_old_summand.assignments());
        lps_new_summands = push_front(lps_new_summands, lps_new_summand);
        i_act++;
        i_sumvars++;
      }
    } // end of summand list iterator
    lps_old_summands = lps_new_summands;
  } //end of rename rule iterator

  if (gsDebug)
  { std::cerr << "Simplifying the result...\n";
  }

  linear_process new_process = lps_old_spec.process();
  new_process.set_summands(lps_old_summands); // These are the renamed sumands.

  // add action_rename_spec.action_labels to action_rename_spec.action_labels without adding duplates.
  action_label_list all=action_rename_spec.action_labels();
  for(action_label_list::const_iterator i=lps_old_spec.action_labels().begin();
           i!=lps_old_spec.action_labels().end(); ++i)
  { if (find(action_rename_spec.action_labels().begin(),
             action_rename_spec.action_labels().end(),*i)==action_rename_spec.action_labels().end())
    { // Not found;
      all=push_front(all,*i);
    }
  }
  specification lps_new_spec = specification(
                                          lps_old_spec.data(),
                                          all,
                                          lps_old_spec.global_variables(),
                                          new_process,
                                          lps_old_spec.initial_process());

  if (gsDebug)
  { std::cerr << "New lps complete\n";
  }
  return lps_new_spec;
} //end of rename(...)

} // namespace lps

} // namespace mcrl2


#endif // MCRL2_LPS_ACTION_RENAME_H
