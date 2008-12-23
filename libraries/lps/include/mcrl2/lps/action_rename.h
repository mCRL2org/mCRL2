// Author(s): Wieger Wesselink
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

#include <sstream>
#include "mcrl2/exception.h"
#include "mcrl2/core/parse.h"
#include "mcrl2/core/typecheck.h"
#include "mcrl2/core/data_implementation.h"
#include "mcrl2/lps/specification.h"

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
      action_rename_rule_rhs(atermpp::aterm_appl t)
        : atermpp::aterm_appl(t)
      {
        assert(core::detail::check_rule_ActionRenameRuleRHS(m_term));
      }

      /// \brief Returns true if the right hand side is equal to delta.
      bool is_delta() const
      {
        return core::detail::gsIsDelta(*this);
      }

      /// \brief Returns true if the right hand side is equal to tau.
      bool is_tau() const
      {
        return core::detail::gsIsTau(*this);
      }

      /// \brief Returns the action.
      /// \pre The right hand side must be an action
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
      data::data_variable_list m_variables;
      data::data_expression    m_condition;
      action                   m_lhs;
      action_rename_rule_rhs   m_rhs;

      /// \brief Initialize the action rename rule with an aterm_appl.
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
      action_rename_rule(atermpp::aterm_appl t)
        : atermpp::aterm_appl(t)
      {
        assert(core::detail::check_rule_ActionRenameRule(m_term));
        init_term(t);
      }

      /// \brief Returns the variables of the rule.
      data::data_variable_list variables() const
      {
        return m_variables;
      }

      /// \brief Returns the condition of the rule.
      data::data_expression condition() const
      {
        return m_condition;
      }

      /// \brief Returns the left hand side of the rule.
      action lhs() const
      {
        return m_lhs;
      }

      /// \brief Returns the right hand side of the rule.
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
      data::data_specification m_data;
      action_label_list        m_action_labels;
      action_rename_rule_list  m_rules;

      /// \brief Initialize the action_rename_specification with an aterm_appl.
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
      action_rename_specification(atermpp::aterm_appl t)
        : atermpp::aterm_appl(t)
      {
        assert(core::detail::check_rule_ActionRenameSpec(m_term));
        init_term(t);
      }

      /// \brief Constructor.
      action_rename_specification(
          data::data_specification  data,
          action_label_list         action_labels,
          action_rename_rule_list   rules
         )
        :
          m_data(data),
          m_action_labels(action_labels),
          m_rules(rules)
      {
        m_term = reinterpret_cast<ATerm>(
          core::detail::gsMakeActionRenameSpec(
            data,
            core::detail::gsMakeActSpec(action_labels),
            core::detail::gsMakeActionRenameRules(rules)
          )
        );
      }

      /// \brief Reads the action rename specification from file.
      /// \param[in] filename
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
      /// \param[in] filename
      /// If filename is nonempty, output is written to the file named filename.
      /// If filename is empty, output is written to stdout.
      /// \param[in] binary
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
      data::data_specification data() const
      { return m_data; }

      /// \brief Returns a sequence of action labels containing all action
      /// labels occurring in the action_rename_specification (but it can have more).
      action_label_list action_labels() const
      { return m_action_labels; }

      /// \brief Returns the action rename rules.
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
    inline
    ATermAppl parse_action_rename_specification(std::istream& from)
    {
      ATermAppl result = core::parse_action_rename_spec(from);
      if (result == NULL)
        throw runtime_error("parse error");
      return result;
    }

    inline
    ATermAppl type_check_action_rename_specification(ATermAppl ar_spec, ATermAppl spec)
    {
      ATermAppl result = core::type_check_action_rename_spec(ar_spec, spec);
      if (result == NULL)
        throw runtime_error("type check error");
      return result;
    }

    inline
    ATermAppl implement_action_rename_specification(ATermAppl ar_spec, ATermAppl& lps_spec)
    {
      ATermAppl result = core::implement_data_action_rename_spec(ar_spec, lps_spec);
      if (result == NULL)
        throw runtime_error("process data implementation error");
      return result;
    }
  } // namespace detail
/// \endcond

  /// \brief Parses an action rename specification.
  /// If the action rename specification contains data types that are not
  /// present in the data specification of \p spec they are added to it.
  /// \param text A string containing an action rename specification
  /// \param spec A linear process specification
  /// \return An action rename specification
  inline
  action_rename_specification parse_action_rename_specification(const std::string& text, lps::specification& spec)
  {
    std::istringstream in(text);
    ATermAppl lps_spec = spec;
    ATermAppl result = detail::parse_action_rename_specification(in);
    result           = detail::type_check_action_rename_specification(result, lps_spec);   
    result           = detail::implement_action_rename_specification(result, lps_spec);
    spec = lps::specification(lps_spec);
    return action_rename_specification(result);
  }

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_ACTION_RENAME_H
