// Copyright (c) 2007 Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

/// \file mcrl2/lps/action.h
/// \brief Add your file description here.

#ifndef MCRL2_LPS_ACTION_H
#define MCRL2_LPS_ACTION_H

#include <cassert>
#include "atermpp/atermpp.h"
#include "mcrl2/data/identifier_string.h"
#include "mcrl2/data/data.h"
#include "mcrl2/data/pretty_print.h"
#include "mcrl2/lps/detail/soundness_checks.h"

namespace lps {

using atermpp::aterm_appl;
using atermpp::term_list;

///////////////////////////////////////////////////////////////////////////////
// action_label
/// \brief Represents a label of an action.
///
// <ActId>        ::= ActId(<String>, <SortExpr>*)
class action_label: public aterm_appl
{
  protected:
    identifier_string m_name;
    sort_list m_sorts;

  public:
    action_label()
      : aterm_appl(detail::constructActId())
    {}

    action_label(aterm_appl t)
     : aterm_appl(t)
    {
      assert(detail::check_rule_ActId(m_term));
      aterm_appl::iterator i = t.begin();
      m_name  = *i++;
      m_sorts = *i;
    }

    action_label(const identifier_string& name, const sort_list &sorts)
     : aterm_appl(gsMakeActId(name, sorts)),
       m_name(name),
       m_sorts(sorts)
    {}

    /// Returns the name of the action label.
    ///
    identifier_string name() const
    {
      return m_name;
    }

    /// Returns the sorts of the action label
    ///
    sort_list sorts() const
    {
      return m_sorts;
    }

    /// Applies a substitution to this action label and returns the result
    /// The Substitution object must supply the method aterm operator()(aterm).
    ///
    template <typename Substitution>
    action_label substitute(Substitution f)
    {
      return action_label(f(*this));
    }     
};

///////////////////////////////////////////////////////////////////////////////
// action_label_list
/// \brief singly linked list of action labels
typedef term_list<action_label> action_label_list;

/// \brief Returns true if the term t is an action label
inline
bool is_action_label(aterm_appl t)
{
  return gsIsActId(t);
};

///////////////////////////////////////////////////////////////////////////////
// action
/// \brief Represents an action
///
// <Action>       ::= Action(<ActId>, <DataExpr>*)
class action: public aterm_appl
{
  protected:
    action_label m_label;
    data_expression_list m_arguments;

  public:
    action()
      : aterm_appl(detail::constructAction())
    {}

    action(aterm_appl t)
     : aterm_appl(t)
    {
      assert(detail::check_rule_Action(m_term));
      aterm_appl::iterator i = t.begin();
      m_label = action_label(*i++);
      m_arguments = data_expression_list(*i);
    }

    action(const action_label& label, const data_expression_list& arguments)
     : aterm_appl(gsMakeAction(label, arguments)),
       m_label(label),
       m_arguments(arguments)
    {}

    /// DEPRECATED (This constructor will disappear).
    action(const identifier_string& name, const data_expression_list& arguments)
     : aterm_appl(gsMakeAction(gsMakeActId(name, apply(arguments, gsGetSort)), arguments)),
       m_arguments(arguments)
    {
      m_label = action_label(*begin());
    }

    /// Returns the label of the action.
    ///
    action_label label() const
    {
      return m_label;
    }

    /// Returns the arguments of the action.
    ///
    data_expression_list arguments() const
    {
      return m_arguments;
    }

    /// Applies a substitution to this action and returns the result.
    /// The Substitution object must supply the method aterm operator()(aterm).
    template <typename Substitution>
    action substitute(Substitution f)
    {
      return action(f(aterm_appl(*this)));
    }     
};

///////////////////////////////////////////////////////////////////////////////
// action_list
/// \brief singly linked list of actions
typedef term_list<action> action_list;

/// Returns true if the term t is an action
inline
bool is_action(aterm_appl t)
{
  return gsIsAction(t);
}

} // namespace lps

/// INTERNAL ONLY
namespace atermpp
{
using lps::action_label;
using lps::action;

template<>
struct aterm_traits<action_label>
{
  typedef ATermAppl aterm_type;
  static void protect(action_label t)   { t.protect(); }
  static void unprotect(action_label t) { t.unprotect(); }
  static void mark(action_label t)      { t.mark(); }
  static ATerm term(action_label t)     { return t.term(); }
  static ATerm* ptr(action_label& t)    { return &t.term(); }
};

template<>
struct aterm_traits<action>
{
  typedef ATermAppl aterm_type;
  static void protect(action t)   { t.protect(); }
  static void unprotect(action t) { t.unprotect(); }
  static void mark(action t)      { t.mark(); }
  static ATerm term(action t)     { return t.term(); }
  static ATerm* ptr(action& t)    { return &t.term(); }
};

} // namespace atermpp

#endif // MCRL2_LPS_ACTION_H
