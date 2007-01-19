///////////////////////////////////////////////////////////////////////////////
/// \file data.h
/// Contains data data structures for the LPE Library.

#ifndef LPE_DATA_H
#define LPE_DATA_H

#include <iostream> // for debugging

#include <string>
#include <cassert>
#include "atermpp/atermpp.h"
#include "atermpp/algorithm.h"
#include "atermpp/aterm_access.h"
#include "atermpp/aterm_string.h"
#include "atermpp/utility.h"
#include "lpe/sort.h"
#include "lpe/pretty_print.h"
#include "lpe/detail/string_utility.h"
#include "lpe/soundness_checks.h"
#include "libstruct.h"

namespace lpe {

using atermpp::aterm_appl;
using atermpp::aterm_list;
using atermpp::aterm_string;
using atermpp::term_list;
using atermpp::aterm;
using atermpp::arg1;
using atermpp::arg2;
using atermpp::arg3;

// prototypes
class data_expression;
class data_variable;

///////////////////////////////////////////////////////////////////////////////
// data_expression_list
/// \brief singly linked list of data expressions
///
typedef term_list<data_expression> data_expression_list;

///////////////////////////////////////////////////////////////////////////////
// data_expression
/// \brief data expression.
/// Represents a data expression or nil(!!!).
class data_expression: public aterm_appl
{
  public:
    data_expression()
    {}

    data_expression(aterm_appl term)
      : aterm_appl(term)
    {
      assert(check_rule_DataExprOrNil(m_term));
    }

    data_expression(ATermAppl term)
      : aterm_appl(term)
    {
      assert(check_rule_DataExprOrNil(m_term));
    }

    /// Returns the sort of the data expression.
    ///
    lpe::sort sort() const
    {
      ATermAppl result = gsGetSort(*this);
      assert(!gsIsUnknown(result));
      return lpe::sort(result);
    }     

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

    /// Returns head of the data expression.
    data_expression head() const
    {
      return gsGetDataExprHead(*this);
    }

    /// Returns arguments of the data expression.
    data_expression_list arguments() const
    {
      return gsGetDataExprArgs(*this);
    }  
  
    /// Applies a substitution to this data_expression and returns the result.
    /// The Substitution object must supply the method aterm operator()(aterm).
    ///
    template <typename Substitution>
    data_expression substitute(Substitution f) const
    {
      return data_expression(f(aterm(*this)));
    }     
};

///////////////////////////////////////////////////////////////////////////////
// data_expression_list
/// \brief singly linked list of data expressions
///
typedef term_list<data_expression> data_expression_list;

inline
bool is_data_expression(aterm_appl t)
{
  return gsIsDataExpr(t);
}

///////////////////////////////////////////////////////////////////////////////
// data_variable
/// \brief data variable.
///
// DataVarId(<String>, <SortExpr>)
class data_variable: public data_expression
{
  public:
    data_variable()
    {}

    data_variable(aterm_appl t)
     : data_expression(t)
    {
      assert(check_rule_DataVarId(m_term));
    }

    /// Very incomplete implementation for initialization using strings like "d:D".
    data_variable(const std::string& s)
    {
      std::string::size_type idx = s.find(':');
      assert (idx != std::string::npos);
      std::string name = s.substr(0, idx);
      std::string type = s.substr(idx+1);
      m_term = reinterpret_cast<ATerm>(gsMakeDataVarId(gsString2ATermAppl(name.c_str()), lpe::sort(type)));
    }

    data_variable(const std::string& name, const lpe::sort& s)
     : data_expression(gsMakeDataVarId(gsString2ATermAppl(name.c_str()), s))
    {}

    /// Returns the name of the data_variable.
    ///
    aterm_string name() const
    {
      return arg1(*this);
    }

    /// Returns the name of the data_variable.
    /// DEPRECATED!
    ///
    std::string unquoted_name() const
    {
      return atermpp::unquote(name());
    }

    /// Returns the sort of the data_variable.
    ///
    lpe::sort sort() const
    {
      return gsGetSort(*this);
    }
  };
                                                            
///////////////////////////////////////////////////////////////////////////////
// data_variable_list
/// \brief singly linked list of data variables
///
typedef term_list<data_variable> data_variable_list;

inline
bool is_data_variable(aterm_appl t)
{
  return gsIsDataVarId(t);
}

///////////////////////////////////////////////////////////////////////////////
// data_application
/// \brief data application.
///
// DataAppl(<DataExpr>, <DataExpr>)
class data_application: public data_expression
{
  public:
    data_application()
    {}

    data_application(aterm_appl t)
     : data_expression(t)
    {
      assert(check_term_DataAppl(m_term));
    }

    data_application(data_expression expr, data_expression arg)
     : data_expression(gsMakeDataAppl(expr, arg))
    {}
  };
                                                            
///////////////////////////////////////////////////////////////////////////////
// data_application_list
/// \brief singly linked list of data applications
///
typedef term_list<data_application> data_application_list;

inline
bool is_data_application(aterm_appl t)
{
  return gsIsDataAppl(t);
}

///////////////////////////////////////////////////////////////////////////////
// data_operation
/// \brief operation on data.
///
class data_operation: public data_expression
{
  public:
    data_operation()
    {}

    data_operation(aterm_appl t)
     : data_expression(t)
    {
      assert(check_rule_OpId(m_term));
    }

    data_operation(aterm_string name, lpe::sort s)
     : data_expression(gsMakeOpId(name, s))
    {}
  };
                                                            
///////////////////////////////////////////////////////////////////////////////
// data_operation_list
/// \brief singly linked list of data operations
///
typedef term_list<data_operation> data_operation_list;

inline
bool is_data_operation(aterm_appl t)
{
  return gsIsOpId(t);
}

///////////////////////////////////////////////////////////////////////////////
// data_variable_init
/// \brief data variable with an initial value.
///
// DataVarIdInit(<String>, <SortExpr>, <DataExpr>)
class data_variable_init: public aterm_appl
{
  public:
    data_variable_init()
    {}

    data_variable_init(aterm_appl t)
     : aterm_appl(t)
    {
      assert(check_rule_DataVarIdInit(m_term));
    }
    
    data_variable to_variable() const
    {
      iterator i = begin();
      aterm_appl x = *i++;
      aterm_appl y = *i;
      return data_variable(gsMakeDataVarId(x, y));
    }

    data_expression to_expression() const
    {
      iterator i = begin();
      aterm_appl x = *i++;
      aterm_appl y = *i++;
      aterm_appl z = *i;
      return data_expression(z);
    }
};

inline
bool is_data_variable_init(aterm_appl t)
{
  return gsIsDataVarIdInit(t);
}

///////////////////////////////////////////////////////////////////////////////
// data_variable_init_list
/// \brief singly linked list of initialized data variables
///
typedef term_list<data_variable_init> data_variable_init_list;

///////////////////////////////////////////////////////////////////////////////
// data_equation
/// \brief data equation.
///
//<DataEqn>      ::= DataEqn(<DataVarId>*, <DataExprOrNil>,
//                     <DataExpr>, <DataExpr>)
class data_equation: public aterm_appl
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
     : aterm_appl(t)
    {
      assert(check_rule_DataEqn(m_term));
      aterm_appl::iterator i = t.begin();
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
     : aterm_appl(gsMakeDataEqn(variables, condition, lhs, rhs)),
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
      return data_equation(f(aterm(*this)));
    }     
};

///////////////////////////////////////////////////////////////////////////////
// data_equation_list
/// \brief singly linked list of data equations
///
typedef term_list<data_equation> data_equation_list;

inline
bool is_data_equation(aterm_appl t)
{
  return gsIsDataEqn(t);
}

///////////////////////////////////////////////////////////////////////////////
// data_assignment
/// \brief data_assignment is an assignment to a data variable.
///
// syntax: data_assignment(data_variable lhs, data_expression rhs)
class data_assignment: public aterm_appl
{
  protected:
    data_variable   m_lhs;         // left hand side of the assignment
    data_expression m_rhs;         // right hand side of the assignment

  public:
    /// Returns true if the types of the left and right hand side are equal.
    bool is_well_typed() const
    {
      return gsGetSort(m_lhs) == gsGetSort(m_rhs);
    }

    data_assignment(aterm_appl t)
     : aterm_appl(t)
    {
      assert(check_rule_Assignment(m_term));
      aterm_appl::iterator i = t.begin();
      m_lhs = data_variable(*i++);
      m_rhs = data_expression(*i);
      assert(is_well_typed());
    }

    data_assignment(data_variable lhs, data_expression rhs)
     : 
       aterm_appl(gsMakeAssignment(lhs, rhs)),
       m_lhs(lhs),
       m_rhs(rhs)
    {
    }

    /// Applies the assignment to t and returns the result.
    ///
    aterm operator()(aterm t) const
    {
      return atermpp::replace(t, aterm(m_lhs), aterm(m_rhs));
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

inline
bool is_data_assignment(aterm_appl t)
{
  return gsIsAssignment(t);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief Returns the right hand sides of the assignments.
inline
data_expression_list data_assignment_expressions(data_assignment_list l)
{
  data_expression_list result;
  for (data_assignment_list::iterator i = l.begin(); i != l.end(); ++i)
  {
    result = push_front(result, i->rhs());
  }
  return atermpp::reverse(result);
}

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
      t = (*i)(t);
    }
    return t;
  }
  private:
    assignment_list_substitution& operator=(const assignment_list_substitution&)
    {
      return *this;
    }
};

} // namespace lpe

namespace atermpp
{
using lpe::data_expression;
using lpe::data_variable;
using lpe::data_application;
using lpe::data_operation;
using lpe::data_assignment;
using lpe::data_variable_init;
using lpe::data_equation;

template<>
struct aterm_traits<data_expression>
{
  typedef ATermAppl aterm_type;
  static void protect(lpe::data_expression t)   { t.protect(); }
  static void unprotect(lpe::data_expression t) { t.unprotect(); }
  static void mark(lpe::data_expression t)      { t.mark(); }
  static ATerm term(lpe::data_expression t)     { return t.term(); }
  static ATerm* ptr(lpe::data_expression& t)    { return &t.term(); }
};

template<>
struct aterm_traits<data_variable>
{
  typedef ATermAppl aterm_type;
  static void protect(data_variable t)   { t.protect(); }
  static void unprotect(data_variable t) { t.unprotect(); }
  static void mark(data_variable t)      { t.mark(); }
  static ATerm term(data_variable t)     { return t.term(); }
  static ATerm* ptr(data_variable& t)    { return &t.term(); }
};

template<>
struct aterm_traits<data_application>
{
  typedef ATermAppl aterm_type;
  static void protect(data_application t)   { t.protect(); }
  static void unprotect(data_application t) { t.unprotect(); }
  static void mark(data_application t)      { t.mark(); }
  static ATerm term(data_application t)     { return t.term(); }
  static ATerm* ptr(data_application& t)    { return &t.term(); }
};

template<>
struct aterm_traits<data_operation>
{
  typedef ATermAppl aterm_type;
  static void protect(data_operation t)   { t.protect(); }
  static void unprotect(data_operation t) { t.unprotect(); }
  static void mark(data_operation t)      { t.mark(); }
  static ATerm term(data_operation t)     { return t.term(); }
  static ATerm* ptr(data_operation& t)    { return &t.term(); }
};

template<>
struct aterm_traits<data_assignment>
{
  typedef ATermAppl aterm_type;
  static void protect(data_assignment t)   { t.protect(); }
  static void unprotect(data_assignment t) { t.unprotect(); }
  static void mark(data_assignment t)      { t.mark(); }
  static ATerm term(data_assignment t)     { return t.term(); }
  static ATerm* ptr(data_assignment& t)    { return &t.term(); }
};

template<>
struct aterm_traits<data_variable_init>
{
  typedef ATermAppl aterm_type;
  static void protect(lpe::data_variable_init t)   { t.protect(); }
  static void unprotect(lpe::data_variable_init t) { t.unprotect(); }
  static void mark(lpe::data_variable_init t)      { t.mark(); }
  static ATerm term(lpe::data_variable_init t)     { return t.term(); }
  static ATerm* ptr(lpe::data_variable_init& t)    { return &t.term(); }
};

template<>
struct aterm_traits<data_equation>
{
  typedef ATermAppl aterm_type;
  static void protect(data_equation t)   { t.protect(); }
  static void unprotect(data_equation t) { t.unprotect(); }
  static void mark(data_equation t)      { t.mark(); }
  static ATerm term(data_equation t)     { return t.term(); }
  static ATerm* ptr(data_equation& t)    { return &t.term(); }
};

} // namespace atermpp

#endif // LPE_DATA_H
