///////////////////////////////////////////////////////////////////////////////
/// \file action.h
/// Contains action data structures for the LPE Library.

#ifndef LPE_ACTION_H
#define LPE_ACTION_H

#include <cassert>
#include "atermpp/atermpp.h"
#include "atermpp/aterm_list.h"
#include "atermpp/aterm_string.h"
#include "lpe/data.h"
#include "lpe/pretty_print.h"

namespace lpe {

using atermpp::aterm_appl;
using atermpp::aterm_string;
using atermpp::term_list;

///////////////////////////////////////////////////////////////////////////////
// action_label
/// \brief Represents a label of an action.
///
// <ActId>        ::= ActId(<String>, <SortExpr>*)
class action_label: public aterm_appl
{
  protected:
    aterm_string m_name;
    sort_list m_sorts;

  public:
    action_label()
    {}

    action_label(aterm_appl t)
     : aterm_appl(t)
    {
      assert(gsIsActId(t));
      aterm_appl::iterator i = t.begin();
      m_name  = *i++;
      m_sorts = *i;
    }

    /// Returns the name of the action label.
    ///
    aterm_string name() const
    {
      return m_name;
    }

    /// Returns the sorts of the action label.
    ///
    sort_list sorts() const
    {
      return m_sorts;
    }

    /// Applies a substitution to this action label and returns the result.
    /// The Substitution object must supply the method aterm operator()(aterm).
    ///
    template <typename Substitution>
    action_label substitute(Substitution f)
    {
      return action_label(f(*this));
    }     

    /// Returns a pretty print representation of the term.
    ///                                                   
    std::string pp() const                                
    {                                                     
      return pretty_print(term());                        
    }
};

///////////////////////////////////////////////////////////////////////////////
// action_label_list
/// \brief singly linked list of actions
///
typedef term_list<action_label> action_label_list;

inline
bool is_action_label(aterm_appl t)
{
  return gsIsActId(t);
};

///////////////////////////////////////////////////////////////////////////////
// action
/// \brief Represents an action.
///
// <Action>       ::= Action(<ActId>, <DataExpr>*)
// <ActId>        ::= ActId(<String>, <SortExpr>*)
class action: public aterm_appl
{
  protected:
    aterm_string m_name;
    sort_list m_sorts;
    data_expression_list m_arguments;    // elements are of type data_expression

  public:
    action()
    {}

    action(aterm_appl t)
     : aterm_appl(t)
    {
      assert(gsIsAction(t));
      aterm_appl::iterator i = t.begin();
      aterm_appl act_id = *i++;
      m_arguments = data_expression_list(*i);
      
      aterm_appl::iterator j = act_id.begin();
      m_name = *j++;
      m_sorts = *j;
    }

    action(const aterm_string& name, const data_expression_list& arguments)
     : aterm_appl(gsMakeAction(gsMakeActId(name, apply(arguments, gsGetSort)), arguments)),
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

    /// Returns a pretty print representation of the term.
    ///                                                   
    std::string pp() const                                
    {                                                     
      return pretty_print(term());                        
    }
};

///////////////////////////////////////////////////////////////////////////////
// action_list
/// \brief singly linked list of actions
///
typedef term_list<action> action_list;

inline
bool is_action(aterm_appl t)
{
  return gsIsAction(t);
}

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

} // namespace lpe

namespace atermpp
{
using lpe::action_label;
using lpe::action;

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

#endif // LPE_ACTION_H
