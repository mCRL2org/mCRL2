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
// <ActId>        ::= ActId(<String>, <SortExpr>*)
class action_label: public atermpp::aterm_appl
{
  protected:
    /// \brief The name of the label
    core::identifier_string m_name;

    /// \brief The sorts of the label
    data::sort_expression_list m_sorts;

  public:
    /// \brief Constructor.
    action_label()
      : atermpp::aterm_appl(mcrl2::core::detail::constructActId())
    {}

    /// \brief Constructor.
    /// \param t A term
    action_label(atermpp::aterm_appl t)
     : atermpp::aterm_appl(t)
    {
      assert(core::detail::check_rule_ActId(m_term));
      atermpp::aterm_appl::iterator i = t.begin();
      m_name  = *i++;
      m_sorts = *i;
    }

    /// \brief Constructor.
    /// \param name A
    /// \param sorts A sequence of sort expressions
    action_label(const core::identifier_string& name, const data::sort_expression_list &sorts)
     : atermpp::aterm_appl(core::detail::gsMakeActId(name, sorts)),
       m_name(name),
       m_sorts(sorts)
    {}

    /// \brief Returns the name of the action label.
    /// \return The name of the action label.
    core::identifier_string name() const
    {
      return m_name;
    }

    /// \brief Returns the sorts of the action label
    /// \return The sorts of the action label
    data::sort_expression_list sorts() const
    {
      return m_sorts;
    }

    /// \brief Applies a low level substitution function to this term and returns the result.
    /// \param f A
    /// The function <tt>f</tt> must supply the method <tt>aterm operator()(aterm)</tt>.
    /// This function is applied to all <tt>aterm</tt> noded appearing in this term.
    /// \deprecated
    /// \return The substitution result.
    template <typename Substitution>
    action_label substitute(Substitution f)
    {
      return action_label(f(*this));
    }
};

/// \brief Read-only singly linked list of action labels
typedef atermpp::term_list<action_label> action_label_list;

/// \brief Returns true if the term t is an action label
/// \param t A term
/// \return True if the term t is an action label
inline
bool is_action_label(atermpp::aterm_appl t)
{
  return core::detail::gsIsActId(t);
}

} // namespace lps

} // namespace mcrl2

/// \cond INTERNAL_DOCS
MCRL2_ATERM_TRAITS_SPECIALIZATION(mcrl2::lps::action_label)
/// \endcond

#endif // MCRL2_LPS_ACTION_LABEL_H
