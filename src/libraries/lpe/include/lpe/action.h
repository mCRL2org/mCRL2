///////////////////////////////////////////////////////////////////////////////
/// \file sort.h
/// Contains action data structures for the LPE Library.

#ifndef LPE_ACTION_H
#define LPE_ACTION_H

#include <cassert>
#include "atermpp/atermpp.h"
#include "atermpp/aterm_list.h"
#include "atermpp/aterm_string.h"
#include "lpe/substitute.h"
#include "lpe/data.h"

namespace lpe {

using atermpp::aterm_appl;
using atermpp::aterm_string;
using atermpp::term_list;

///////////////////////////////////////////////////////////////////////////////
// action
/// \brief Represents an action.
///
// <Action>       ::= Action(<ActId>, <DataExpr>*)
// <ActId>        ::= ActId(<String>, <SortExpr>*)
class action: public aterm_appl_wrapper
{
  protected:
    aterm_string m_name;
    sort_list m_sorts;
    data_expression_list m_arguments;    // elements are of type data_expression

  public:
    action()
    {}

    action(aterm_appl t)
     : aterm_appl_wrapper(t)
    {
      assert(gsIsNil(t) || gsIsAction(t));
      aterm_list::iterator i = t.argument_list().begin();
      aterm_appl act_id = *i++;
      m_arguments = data_expression_list(*i);
      
      aterm_list::iterator j = act_id.argument_list().begin();
      m_name = *j++;
      m_sorts = *j;
    }

    action(const aterm_string& name, const data_expression_list& arguments)
     : aterm_appl_wrapper(gsMakeAction(gsMakeActId(name, get_sorts(arguments)), arguments)),
       m_name(name),
       m_arguments(arguments)
    {}

    /// Returns the name of the action.
    ///
    aterm_string name() const
    {
      return m_name;
    }

    sort_list sorts() const
    {
      return m_sorts;
    }

    /// Returns the sequence of arguments.
    ///
    data_expression_list arguments() const
    {
      return m_arguments;
    }

    /// Applies a substitution to this action and returns the result.
    /// The Substitution object must supply the method aterm operator()(aterm).
    ///
    template <typename Substitution>
    action substitute(Substitution f)
    {
      return action(f(aterm_appl(*this)));
    }     
};

///////////////////////////////////////////////////////////////////////////////
// action_list
/// \brief singly linked list of actions
///
typedef term_list<action> action_list;

///////////////////////////////////////////////////////////////////////////////
// timed_action
/// \brief multi action with time
///
class timed_action
{
  protected:
    action_list     m_actions;
    data_expression m_time;

  public:
    timed_action(action_list actions, data_expression time)
      : m_actions(actions), m_time(time)
    {}

    /// Returns true if time is available.
    ///
    bool has_time() const
    {
      return !m_time.is_nil();
    }

    /// Returns the sequence of actions. Returns an empty list if is_delta() holds.
    ///
    action_list actions() const
    {
      return m_actions;
    }

    /// Returns the time expression.
    ///
    data_expression time() const
    {
      return m_time;
    }
    
    /// Returns a term representing the name of the first action.
    ///
    aterm_string name() const
    {
      return front(m_actions).name();
    }
    
    /// Returns the argument of the multi action.
    data_expression_list arguments() const
    {
      return front(m_actions).arguments();
    }
};

} // namespace mcrl

#endif // LPE_ACTION_H
