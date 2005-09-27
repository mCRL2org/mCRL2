///////////////////////////////////////////////////////////////////////////////
/// \file sort.h
/// Contains action data structures for the mcrl2 library.

#ifndef MCRL2_ACTION_H
#define MCRL2_ACTION_H

#include "atermpp/aterm.h"
#include "mcrl2/substitute.h"
#include "mcrl2/term_list.h"
#include "mcrl2/data.h"
#include "mcrl2/list_iterator.h"

namespace mcrl2 {

using atermpp::aterm_appl;
using atermpp::aterm_list;
using atermpp::aterm_list_iterator;

///////////////////////////////////////////////////////////////////////////////
// action
/// \brief Represents an action.
///
class action: public aterm_wrapper
{
  protected:
    aterm_appl m_name;
    data_expression_list m_arguments;    // elements are of type data_expression

  public:
    action()
    {}

    action(aterm_appl t)
     : aterm_wrapper(t)
    {
      aterm_list_iterator i = term().argument_list().begin();
      m_name      = *i++;
      m_arguments = data_expression_list(*i);
    }

    /// Returns the name of the action.
    ///
    std::string name() const
    {
      return m_name.to_string();
    }

    /// Returns the sequence of arguments.
    ///
    data_expression_list arguments() const
    {
      return m_arguments;
    }

    /// Applies a substitution to this action and returns the result.
    /// The Substitution object must supply the method aterm_appl operator()(aterm_appl).
    ///
    template <typename Substitution>
    action substitute(Substitution f)
    {
      return data_expression(f(term()));
    }     

    /// Applies a sequence of substitutions to this action and returns the result.
    ///
    template <typename SubstIter>
    action substitute(SubstIter first, SubstIter last) const
    {
      return action(aterm_appl_substitute(term(), first, last));
    }
};

typedef term_list<action> action_list;

} // namespace mcrl

#endif // MCRL2_ACTION_H
