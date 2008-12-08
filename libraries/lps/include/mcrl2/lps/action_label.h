// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/action_label.h
/// \brief The class action_label.

#ifndef MCRL2_LPS_ACTION_LABEL_H
#define MCRL2_LPS_ACTION_LABEL_H

#include <cassert>
#include "mcrl2/atermpp/atermpp.h"
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/core/detail/soundness_checks.h"

namespace mcrl2 {

namespace lps {

///////////////////////////////////////////////////////////////////////////////
// action_label
/// \brief Represents a label of an action.
///
// <ActId>        ::= ActId(<String>, <SortExpr>*)
class action_label: public atermpp::aterm_appl
{
  protected:
    core::identifier_string m_name;
    data::sort_expression_list m_sorts;

  public:
    /// Constructor.
    ///
    action_label()
      : atermpp::aterm_appl(mcrl2::core::detail::constructActId())
    {}

    /// Constructor.
    ///
    action_label(atermpp::aterm_appl t)
     : atermpp::aterm_appl(t)
    {
      assert(core::detail::check_rule_ActId(m_term));
      atermpp::aterm_appl::iterator i = t.begin();
      m_name  = *i++;
      m_sorts = *i;
    }

    /// Constructor.
    ///
    action_label(const core::identifier_string& name, const data::sort_expression_list &sorts)
     : atermpp::aterm_appl(core::detail::gsMakeActId(name, sorts)),
       m_name(name),
       m_sorts(sorts)
    {}

    /// Returns the name of the action label.
    ///
    core::identifier_string name() const
    {
      return m_name;
    }

    /// Returns the sorts of the action label
    ///
    data::sort_expression_list sorts() const
    {
      return m_sorts;
    }

    /// Applies a substitution to this action label and returns the result
    /// The Substitution object must supply the method atermpp::aterm operator()(atermpp::aterm).
    ///
    template <typename Substitution>
    action_label substitute(Substitution f)
    {
      return action_label(f(*this));
    }     
};

/// \brief Read-only singly linked list of action labels
typedef atermpp::term_list<action_label> action_label_list;

/// \brief Returns true if the term t is an action label
inline
bool is_action_label(atermpp::aterm_appl t)
{
  return core::detail::gsIsActId(t);
};

} // namespace lps

} // namespace mcrl2

/// \cond INTERNAL_DOCS
MCRL2_ATERM_TRAITS_SPECIALIZATION(mcrl2::lps::action_label)
/// \endcond

#endif // MCRL2_LPS_ACTION_LABEL_H
