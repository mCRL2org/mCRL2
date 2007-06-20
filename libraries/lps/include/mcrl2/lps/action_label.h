// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/action_label.h
/// \brief Add your file description here.

#ifndef MCRL2_LPS_ACTION_LABEL_H
#define MCRL2_LPS_ACTION_LABEL_H

#include <cassert>
#include "atermpp/atermpp.h"
#include "mcrl2/basic/identifier_string.h"
#include "mcrl2/data/sort.h"
#include "mcrl2/basic/detail/soundness_checks.h"

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

} // namespace lps

/// INTERNAL ONLY
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

#endif // MCRL2_LPS_ACTION_LABEL_H
