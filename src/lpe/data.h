///////////////////////////////////////////////////////////////////////////////
/// \file sort.h
/// Contains data data structures for the LPE Library.

#ifndef LPE_DATA_H
#define LPE_DATA_H

#include <iostream> // for debugging

#include <string>
#include "atermpp/atermpp.h"
#include "lpe/aterm_wrapper.h"
#include "lpe/substitute.h"
#include "lpe/sort.h"
#include "lpe/predefined_symbols.h"
#include "lpe/detail/string_utility.h"
#include "libstruct.h"

namespace lpe {

using atermpp::aterm_appl;
using atermpp::aterm_list;
using atermpp::term_list;
using atermpp::aterm;

///////////////////////////////////////////////////////////////////////////////
// data_expression
/// \brief data expression.
///
class data_expression: public aterm_wrapper
{
  public:
    data_expression()
    {}

    data_expression(ATermAppl term)
      : aterm_wrapper(aterm_appl(term))
    {}

    data_expression(aterm_appl term)
      : aterm_wrapper(term)
    {}

    /// Returns true if the data expression equals 'nil' (meaning it has no
    /// sensible value).
    ///
    bool is_nil() const
    {
      return *this == gsMakeNil();
    }     

    /// Returns true if the data expression equals 'true'.
    /// Note that the term will not be rewritten first.
    ///
    bool is_true() const
    {
      assert(!is_nil());
      return *this == gsMakeDataExprTrue();
    }     

    /// Returns true if the data expression equals 'false'.
    /// Note that the term will not be rewritten first.
    ///
    bool is_false() const
    {
      assert(!is_nil());
      return *this == gsMakeDataExprFalse();
    }     

    /// Applies a substitution to this data_expression and returns the result.
    /// The Substitution object must supply the method aterm operator()(aterm).
    ///
    template <typename Substitution>
    data_expression substitute(Substitution f) const
    {
      return data_expression(f(aterm_appl(*this)));
    }     
};

///////////////////////////////////////////////////////////////////////////////
// data_expression_list
/// \brief singly linked list of data expressions
///
typedef term_list<data_expression> data_expression_list;

///////////////////////////////////////////////////////////////////////////////
// data_variable
/// \brief data variable.
///
// DataVarId(<String>, <SortExpr>)
class data_variable: public aterm_wrapper
{
  public:
    data_variable()
    {}

    data_variable(aterm_appl t)
     : aterm_wrapper(t)
    {}

    data_variable(const std::string& name, const sort& s)
     : aterm_wrapper(gsMakeDataVarId(gsString2ATermAppl(name.c_str()), s))
    {}

    data_expression to_expr() const
    {
      return data_expression(aterm_appl(*this));
    }

    /// Returns the name of the data_variable.
    ///
    std::string name() const
    {
      return unquote(aterm_appl(*this).argument(0).to_string());
    }

    /// Returns the sort of the data_variable.
    ///
    sort type() const
    {
      return lpe::sort(aterm_appl(*this).argument(1));
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
    data_expression m_condition;
    data_expression m_lhs;
    data_expression m_rhs;

  public:
    typedef data_variable_list::iterator variable_iterator;

    data_equation()
    {}

    data_equation(aterm_appl t)
     : aterm_wrapper(t)
    {
      aterm_list::iterator i = t.argument_list().begin();
      m_variables = data_variable_list(*i++);
      m_condition = data_expression(*i++);
      m_lhs       = data_expression(*i++);
      m_rhs       = data_expression(*i);
    } 

    data_equation(data_variable_list variables,
                  data_expression    condition,
                  data_expression    lhs,
                  data_expression    rhs
                 )
     : aterm_wrapper(gsMakeDataEqn(variables, condition, lhs, rhs)),
       m_variables(variables),
       m_condition(condition),
       m_lhs(lhs),
       m_rhs(rhs)     
    {}

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
      return m_condition;
    }

    /// Returns the left hand side of the Assignment.
    ///
    data_expression lhs() const
    {
      return m_lhs;
    }

    /// Returns the right hand side of the Assignment.
    ///
    data_expression rhs() const
    {
      return m_rhs;
    }

    /// Applies a substitution to this data_equation and returns the result.
    /// The Substitution object must supply the method aterm operator()(aterm).
    ///
    template <typename Substitution>
    data_equation substitute(Substitution f) const
    {
      return data_equation(f(aterm_appl(*this)));
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
    data_variable   m_lhs;         // left hand side of the assignment
    data_expression m_rhs;         // right hand side of the assignment

  public:
    data_assignment(aterm_appl t)
     : aterm_wrapper(t)
    {
      aterm_list::iterator i = t.argument_list().begin();
      m_lhs = data_variable(*i++);
      m_rhs = data_expression(*i);
    }

    data_assignment(data_variable lhs, data_expression rhs)
     : 
       aterm_wrapper(gsMakeAssignment(lhs, rhs)),
       m_lhs(lhs),
       m_rhs(rhs)
    {
    }

    /// Applies the assignment to t and returns the result.
    ///
    aterm operator()(aterm t) const
    {
      return atermpp::replace(aterm_appl(m_lhs), aterm_appl(m_rhs), t);
    }

    /// Returns the left hand side of the data_assignment.
    ///
    data_variable lhs() const
    {
      return m_lhs;
    }

    /// Returns the right hand side of the data_assignment.
    ///
    data_expression rhs() const
    {
      return m_rhs;
    }
};

///////////////////////////////////////////////////////////////////////////////
// data_assignment_list
/// \brief singly linked list of data assignments
///
typedef term_list<data_assignment> data_assignment_list;

///////////////////////////////////////////////////////////////////////////////
// assignment_list_substitution
/// Utility class for applying a sequence of data assignments.
///
struct assignment_list_substitution
{
  const data_assignment_list& m_assignments;
  
  assignment_list_substitution(const data_assignment_list& assignments)
    : m_assignments(assignments)
  {}
  
  aterm operator()(aterm t) const
  {
    for (data_assignment_list::iterator i = m_assignments.begin(); i != m_assignments.end(); ++i)
    {
//std::cout << "i = " << i->pp() << std::endl;
//std::cout << "t0 = " << t.to_string() << std::endl;
      t = (*i)(t);
//std::cout << "t1 = " << t.to_string() << std::endl;
    }
    return t;
  }
};

} // namespace mcrl

#endif // LPE_DATA_H
