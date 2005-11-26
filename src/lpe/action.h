///////////////////////////////////////////////////////////////////////////////
/// \file sort.h
/// Contains action data structures for the LPE Library.

#ifndef LPE_ACTION_H
#define LPE_ACTION_H

#include "atermpp/atermpp.h"
#include "lpe/substitute.h"
#include "lpe/data.h"

namespace lpe {

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
      return unquote(aterm_appl(ATAgetArgument(m_name,0)).to_string());
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

typedef term_list<action> action_list;

} // namespace mcrl

#endif // LPE_ACTION_H
