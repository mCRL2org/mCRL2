// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/action.h
/// \brief The class action.

#ifndef MCRL2_LPS_ACTION_H
#define MCRL2_LPS_ACTION_H

#include <cassert>
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/detail/data_functional.h"
#include "mcrl2/lps/action_label.h"

namespace mcrl2 {

namespace lps {

/// \brief Represents an action.
// <Action>       ::= Action(<ActId>, <DataExpr>*)
class action: public atermpp::aterm_appl
{
  protected:

    /// \brief The label of the action
    action_label m_label;

    /// \brief The arguments of the action
    data::data_expression_list m_arguments;

  public:
    /// \brief Constructor.
    action()
      : atermpp::aterm_appl(mcrl2::core::detail::constructAction())
    {}

    /// \brief Constructor.
    /// \param t A term
    action(atermpp::aterm_appl t)
     : atermpp::aterm_appl(t)
    {
      assert(core::detail::check_rule_Action(m_term));
      atermpp::aterm_appl::iterator i = t.begin();
      m_label = action_label(*i++);
      m_arguments = *i;
    }

    /// \brief Constructor.
    /// \param label An action label
    /// \param arguments A sequence of data expressions
    action(const action_label& label, const data::data_expression_list& arguments)
     : atermpp::aterm_appl(core::detail::gsMakeAction(label, arguments)),
       m_label(label),
       m_arguments(arguments)
    {}

    /// \brief Returns the label of the action.
    /// \return The label of the action.
    action_label label() const
    {
      return m_label;
    }

    /// \brief Returns the arguments of the action.
    /// \return The arguments of the action.
    data::data_expression_list arguments() const
    {
      return m_arguments;
    }

    /// \brief Applies a low level substitution function to this term and returns the result.
    /// \param f A
    /// The function <tt>f</tt> must supply the method <tt>aterm operator()(aterm)</tt>.
    /// This function is applied to all <tt>aterm</tt> noded appearing in this term.
    /// \deprecated
    /// \return The substitution result.
    template <typename Substitution>
    action substitute(Substitution f)
    {
      return action(f(atermpp::aterm_appl(*this)));
    }
};

/// Read-only singly linked list of actions
typedef atermpp::term_list<action> action_list;

/// \brief Returns true if the term t is an action
/// \param t A term
/// \return True if the term t is an action
inline
bool is_action(atermpp::aterm_appl t)
{
  return core::detail::gsIsAction(t);
}

/// \brief Compares the signatures of two actions
/// \param a An action
/// \param b An action
/// \return Returns true if the actions a and b have the same label, and
/// the sorts of the arguments of a and b are equal.
inline
bool equal_signatures(const action& a, const action& b)
{
  if (a.label() != b.label())
    return false;

  const data::data_expression_list& a_args = a.arguments();
  const data::data_expression_list& b_args = b.arguments();

  if (a_args.size() != b_args.size())
    return false;

  return std::equal(a_args.begin(), a_args.end(), b_args.begin(), mcrl2::data::detail::equal_data_expression_sort());
}

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_ACTION_H
