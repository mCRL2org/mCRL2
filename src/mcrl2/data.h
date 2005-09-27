///////////////////////////////////////////////////////////////////////////////
/// \file sort.h
/// Contains data data structures for the mcrl2 library.

#ifndef MCRL2_DATA_H
#define MCRL2_DATA_H

#include "atermpp/aterm.h"
#include "atermpp/aterm_algorithm.h"
#include "mcrl2/aterm_wrapper.h"
#include "mcrl2/substitute.h"
#include "mcrl2/sort.h"
#include "mcrl2/term_list.h"
#include "mcrl2/list_iterator.h"
#include "mcrl2/predefined_symbols.h"
#include "libgsrewrite.h"

namespace mcrl2 {

using atermpp::aterm_appl;
using atermpp::aterm_list;
using atermpp::aterm_list_iterator;

///////////////////////////////////////////////////////////////////////////////
// data_expression
/// \brief data expression.
///
class data_expression: public aterm_wrapper
{
  public:
    data_expression()
    {}

    data_expression(aterm_appl term)
      : aterm_wrapper(term)
    {}

    /// Applies a substitution to this data_expression and returns the result.
    /// The Substitution object must supply the method aterm_appl operator()(aterm_appl).
    ///
    template <typename Substitution>
    data_expression substitute(Substitution f)
    {
      return data_expression(f(term()));
    }     

    /// Applies a sequence of substitutions to this data_expression and returns the result.
    ///
    template <typename SubstIter>
    data_expression substitute(SubstIter first, SubstIter last) const
    {
      return data_expression(aterm_appl_substitute(term(), first, last));
    }
};

/// Rewrites the data expressions x an y, and then compares if they are equal.
///
bool compare(data_expression x, data_expression y)
{
  gsRewriteInit(x.term().appl(), GS_REWR_INNER3); 
  ATerm x1 = (ATerm) gsRewriteTerm(x.term().appl());
  gsRewriteFinalise();

  gsRewriteInit(y.term().appl(), GS_REWR_INNER3); 
  ATerm y1 = (ATerm) gsRewriteTerm(y.term().appl());
  gsRewriteFinalise();
  
  return atermpp::aterm(x1) == atermpp::aterm(y1);
}

///////////////////////////////////////////////////////////////////////////////
// data_expression_list
/// \brief singly linked list of data expressions
///
typedef term_list<data_expression> data_expression_list;

///////////////////////////////////////////////////////////////////////////////
// data_variable
/// \brief data variable.
///
class data_variable: public aterm_wrapper
{
  public:
    data_variable()
    {}

    data_variable(aterm_appl t)
     : aterm_wrapper(t)
    {}

    /// Returns the name of the data_variable.
    ///
    std::string name() const
    {
      return term().argument(0).to_string();
    }

    /// Returns the sort of the data_variable.
    ///
    sort type() const
    {
      return mcrl2::sort(term().argument(1));
    }
};

///////////////////////////////////////////////////////////////////////////////
// data_variable_list
/// \brief singly linked list of data variables
///
typedef term_list<data_variable> data_variable_list;

///////////////////////////////////////////////////////////////////////////////
// data_equation
/// \brief data equation.
///
class data_equation: public aterm_wrapper
{
  protected:
    data_variable_list m_variables;
    aterm_appl m_condition;
    aterm_appl m_lhs;
    aterm_appl m_rhs;

  public:
    typedef list_iterator<data_variable> variable_iterator;

    data_equation()
    {}

    data_equation(aterm_appl t)
     : aterm_wrapper(t)
    {
      aterm_list_iterator i = term().argument_list().begin();
      m_variables = data_variable_list(*i++);
      m_condition = *i++;
      m_lhs       = *i++;
      m_rhs       = *i;
    } 

    /// Returns the sequence of variables.
    ///
    data_variable_list variables() const
    {
      return m_variables;
    }

    /// Returns the condition of the summand (must be of type bool).
    ///
    data_expression condition() const
    {
      return data_expression(m_condition);
    }

    /// Returns the left hand side of the Assignment.
    ///
    data_expression lhs() const
    {
      return data_expression(m_lhs);
    }

    /// Returns the right hand side of the Assignment.
    ///
    data_expression rhs() const
    {
      return data_expression(m_rhs);
    }

    /// Applies a substitution to this data_equation and returns the result.
    /// The Substitution object must supply the method aterm_appl operator()(aterm_appl).
    ///
    template <typename Substitution>
    data_equation substitute(Substitution f)
    {
      return data_expression(f(term()));
    }     

    /// Applies a sequence of substitutions to this data_equation and returns the result.
    ///
    template <typename SubstIter>
    data_equation substitute(SubstIter first, SubstIter last) const
    {
      return data_equation(aterm_appl_substitute(term(), first, last));
    }
};

///////////////////////////////////////////////////////////////////////////////
// data_equation_list
/// \brief singly linked list of data equations
///
typedef term_list<data_equation> data_equation_list;

///////////////////////////////////////////////////////////////////////////////
// data_assignment
/// \brief data_assignment is an assignment to a data variable.
///
// syntax: data_assignment(data_variable lhs, data_expression rhs)
class data_assignment: public aterm_wrapper
{
  protected:
    aterm_appl m_lhs;         // left hand side of the assignment
    aterm_appl m_rhs;         // right hand side of the assignment

  public:
    data_assignment(aterm_appl t)
     : aterm_wrapper(t)
    {
      aterm_list_iterator i = term().argument_list().begin();
      m_lhs = *i++;
      m_rhs = *i;
    }

    data_assignment(data_variable lhs, data_expression rhs)
     : 
       aterm_wrapper(aterm_appl(mcrl2::func_Assignment(), lhs.term(), rhs.term())),
       m_lhs(lhs.term()),
       m_rhs(rhs.term())
    {
    }

    /// Applies the assignment to t and returns the result.
    ///
    aterm_appl operator()(aterm_appl t) const
    {
      return atermpp::replace_non_recursive(t, m_lhs, m_rhs);
    }

    /// Returns the left hand side of the data_assignment.
    ///
    data_variable lhs() const
    {
      return data_variable(m_lhs);
    }

    /// Returns the right hand side of the data_assignment.
    ///
    data_expression rhs() const
    {
      return data_expression(m_rhs);
    }
};

///////////////////////////////////////////////////////////////////////////////
// data_assignment_list
/// \brief singly linked list of data assignments
///
typedef term_list<data_assignment> data_assignment_list;

} // namespace mcrl

#endif // MCRL2_DATA_H
