// Author(s): Wieger Wesselink
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
#include "mcrl2/data/data.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/data/detail/data_functional.h"
#include "mcrl2/lps/action_label.h"

namespace lps {

using atermpp::aterm_appl;
using atermpp::term_list;

/// \brief Represents an action.
///
// <Action>       ::= Action(<ActId>, <DataExpr>*)
class action: public aterm_appl
{
  protected:
    action_label m_label;
    data_expression_list m_arguments;

  public:
    /// Constructor.
    ///
    action()
      : aterm_appl(detail::constructAction())
    {}

    /// Constructor.
    ///
    action(aterm_appl t)
     : aterm_appl(t)
    {
      assert(detail::check_rule_Action(m_term));
      aterm_appl::iterator i = t.begin();
      m_label = action_label(*i++);
      m_arguments = data_expression_list(*i);
    }

    /// Constructor.
    ///
    action(const action_label& label, const data_expression_list& arguments)
     : aterm_appl(gsMakeAction(label, arguments)),
       m_label(label),
       m_arguments(arguments)
    {}

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

/// \brief singly linked list of actions
typedef term_list<action> action_list;

/// Returns true if the term t is an action
inline
bool is_action(aterm_appl t)
{
  return gsIsAction(t);
}

/// Returns true if the actions a and b have the same label, and the sorts of the
/// arguments of a and b are equal.
inline
bool equal_signatures(const action& a, const action& b)
{
  if (a.label() != b.label())
    return false;

  const data_expression_list& a_args = a.arguments();
  const data_expression_list& b_args = b.arguments();
  
  if (a_args.size() != b_args.size())
    return false;

  return std::equal(a_args.begin(), a_args.end(), b_args.begin(), detail::equal_data_expression_sort());
}

} // namespace lps

/// \cond INTERNAL_DOCS
namespace atermpp
{
using lps::action;

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
/// \endcond

#endif // MCRL2_LPS_ACTION_H
