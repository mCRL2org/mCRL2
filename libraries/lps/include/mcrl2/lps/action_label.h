// Author(s): Wieger Wesselink
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
#include "mcrl2/core/detail/soundness_checks.h"

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
    sort_expression_list m_sorts;

  public:
    /// Constructor.
    ///
    action_label()
      : aterm_appl(mcrl2::core::detail::constructActId())
    {}

    /// Constructor.
    ///
    action_label(aterm_appl t)
     : aterm_appl(t)
    {
      assert(check_rule_ActId(m_term));
      aterm_appl::iterator i = t.begin();
      m_name  = *i++;
      m_sorts = *i;
    }

    /// Constructor.
    ///
    action_label(const identifier_string& name, const sort_expression_list &sorts)
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
    sort_expression_list sorts() const
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

/// \brief singly linked list of action labels
typedef term_list<action_label> action_label_list;

/// \brief Returns true if the term t is an action label
inline
bool is_action_label(aterm_appl t)
{
  return gsIsActId(t);
};

} // namespace lps

/// \cond INTERNAL_DOCS
namespace atermpp
{
using lps::action_label;

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

} // namespace atermpp
/// \endcond

#endif // MCRL2_LPS_ACTION_LABEL_H
