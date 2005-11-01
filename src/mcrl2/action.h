///////////////////////////////////////////////////////////////////////////////
/// \file sort.h
/// Contains action data structures for the mcrl2 library.

#ifndef MCRL2_ACTION_H
#define MCRL2_ACTION_H

#include "atermpp/atermpp.h"
#include "mcrl2/substitute.h"
#include "mcrl2/data.h"
#include "mcrl2/list_iterator.h"

namespace mcrl2 {

using atermpp::aterm_appl;
using atermpp::term_list;

///////////////////////////////////////////////////////////////////////////////
// action
/// \brief Represents an action.
///
// Action(ActId("r1",[SortId("D")]),[DataVarId("d2",SortId("D"))])
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
      aterm_list::iterator i = aterm_appl(*this).argument_list().begin();
      m_name      = *i++;
      m_arguments = data_expression_list(*i);
    }

    action(const std::string& name, const data_expression_list& arguments)
     : aterm_wrapper(gsMakeAction(gsString2ATermAppl(name.c_str()), arguments)),
       m_name(name),
       m_arguments(arguments)
    {}

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
    /// The Substitution object must supply the method aterm operator()(aterm).
    ///
    template <typename Substitution>
    action substitute(Substitution f)
    {
      return action(f(aterm_appl(*this)));
    }     

    /// Applies a sequence of substitutions to this action and returns the result.
    ///
    template <typename SubstIter>
    action substitute(SubstIter first, SubstIter last) const
    {
      return action(mcrl2::substitute(*this, first, last));
    }
};

typedef term_list<action> action_list;

} // namespace mcrl

#endif // MCRL2_ACTION_H
