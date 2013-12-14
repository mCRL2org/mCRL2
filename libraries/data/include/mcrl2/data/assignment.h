// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/assignment.h
/// \brief The class assignment.

#ifndef MCRL2_DATA_ASSIGNMENT_H
#define MCRL2_DATA_ASSIGNMENT_H

#include <stdexcept>
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/core/detail/default_values.h"
#include "mcrl2/core/detail/soundness_checks.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/undefined.h"
#include "mcrl2/data/untyped_identifier.h"
#include "mcrl2/data/variable.h"

namespace mcrl2
{

namespace data
{

//--- start generated classes ---//
/// \brief Assignment expression
class assignment_expression: public atermpp::aterm_appl
{
  public:
    /// \brief Default constructor.
    assignment_expression()
      : atermpp::aterm_appl(core::detail::default_values::WhrDecl)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit assignment_expression(const atermpp::aterm& term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_rule_WhrDecl(*this));
    }
};

/// \brief list of assignment_expressions
typedef atermpp::term_list<assignment_expression> assignment_expression_list;

/// \brief vector of assignment_expressions
typedef std::vector<assignment_expression>    assignment_expression_vector;

// prototypes
inline bool is_assignment(const atermpp::aterm_appl& x);
inline bool is_untyped_identifier_assignment(const atermpp::aterm_appl& x);

/// \brief Test for a assignment_expression expression
/// \param x A term
/// \return True if \a x is a assignment_expression expression
inline
bool is_assignment_expression(const atermpp::aterm_appl& x)
{
  return data::is_assignment(x) ||
         data::is_untyped_identifier_assignment(x);
}

// prototype declaration
std::string pp(const assignment_expression& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const assignment_expression& x)
{
  return out << data::pp(x);
}

/// \brief swap overload
inline void swap(assignment_expression& t1, assignment_expression& t2)
{
  t1.swap(t2);
}


/// \brief Assignment of a data expression to a variable
class assignment: public assignment_expression
{
  public:
    /// \brief Default constructor.
    assignment()
      : assignment_expression(core::detail::default_values::DataVarIdInit)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit assignment(const atermpp::aterm& term)
      : assignment_expression(term)
    {
      assert(core::detail::check_term_DataVarIdInit(*this));
    }

    /// \brief Constructor.
    assignment(const variable& lhs, const data_expression& rhs)
      : assignment_expression(atermpp::aterm_appl(core::detail::function_symbol_DataVarIdInit(), lhs, rhs))
    {}

    const variable& lhs() const
    {
      return atermpp::aterm_cast<const variable>((*this)[0]);
    }

    const data_expression& rhs() const
    {
      return atermpp::aterm_cast<const data_expression>((*this)[1]);
    }
//--- start user section assignment ---//
    /// \brief Applies the assignment to a variable
    /// \param[in] x A variable
    /// \return The value <tt>x[lhs() := rhs()]</tt>.
    const data_expression &operator()(const variable& x) const
    {
      return x == lhs() ? rhs() : atermpp::aterm_cast<const data_expression>(x);
    }

    /// \brief Applies the assignment to a term
    /// \return The value <tt>x[lhs() := rhs()]</tt>.
    template < typename Expression >
    data_expression operator()(const Expression& /*x*/) const
    {
      throw std::runtime_error("data::assignment::operator(const Expression&) is a deprecated interface!");
      return data::undefined_data_expression();
    }
//--- end user section assignment ---//
};

/// \brief list of assignments
typedef atermpp::term_list<assignment> assignment_list;

/// \brief vector of assignments
typedef std::vector<assignment>    assignment_vector;

/// \brief Test for a assignment expression
/// \param x A term
/// \return True if \a x is a assignment expression
inline
bool is_assignment(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::DataVarIdInit;
}

// prototype declaration
std::string pp(const assignment& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const assignment& x)
{
  return out << data::pp(x);
}

/// \brief swap overload
inline void swap(assignment& t1, assignment& t2)
{
  t1.swap(t2);
}


/// \brief Assignment of a data expression to a string
class untyped_identifier_assignment: public assignment_expression
{
  public:
    /// \brief Default constructor.
    untyped_identifier_assignment()
      : assignment_expression(core::detail::default_values::UntypedIdentifierAssignment)
    {}

    /// \brief Constructor.
    /// \param term A term
    explicit untyped_identifier_assignment(const atermpp::aterm& term)
      : assignment_expression(term)
    {
      assert(core::detail::check_term_UntypedIdentifierAssignment(*this));
    }

    /// \brief Constructor.
    untyped_identifier_assignment(const core::identifier_string& lhs, const data_expression& rhs)
      : assignment_expression(atermpp::aterm_appl(core::detail::function_symbol_UntypedIdentifierAssignment(), lhs, rhs))
    {}

    /// \brief Constructor.
    untyped_identifier_assignment(const std::string& lhs, const data_expression& rhs)
      : assignment_expression(atermpp::aterm_appl(core::detail::function_symbol_UntypedIdentifierAssignment(), core::identifier_string(lhs), rhs))
    {}

    const core::identifier_string& lhs() const
    {
      return atermpp::aterm_cast<const core::identifier_string>((*this)[0]);
    }

    const data_expression& rhs() const
    {
      return atermpp::aterm_cast<const data_expression>((*this)[1]);
    }
//--- start user section untyped_identifier_assignment ---//
    /// \brief Applies the assignment to a variable
    /// \param[in] x An identifier string
    /// \return The value <tt>x[lhs() := rhs()]</tt>.
    data_expression operator()(const untyped_identifier& x) const
    {
      return x == lhs() ? rhs() : data_expression(x);
    }
//--- end user section untyped_identifier_assignment ---//
};

/// \brief list of untyped_identifier_assignments
typedef atermpp::term_list<untyped_identifier_assignment> untyped_identifier_assignment_list;

/// \brief vector of untyped_identifier_assignments
typedef std::vector<untyped_identifier_assignment>    untyped_identifier_assignment_vector;

/// \brief Test for a untyped_identifier_assignment expression
/// \param x A term
/// \return True if \a x is a untyped_identifier_assignment expression
inline
bool is_untyped_identifier_assignment(const atermpp::aterm_appl& x)
{
  return x.function() == core::detail::function_symbols::UntypedIdentifierAssignment;
}

// prototype declaration
std::string pp(const untyped_identifier_assignment& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const untyped_identifier_assignment& x)
{
  return out << data::pp(x);
}

/// \brief swap overload
inline void swap(untyped_identifier_assignment& t1, untyped_identifier_assignment& t2)
{
  t1.swap(t2);
}
//--- end generated classes ---//

/// \brief Selects the right-hand side of an assignment
struct left_hand_side : public std::unary_function< const assignment, variable >
{
  variable operator()(assignment const& a) const
  {
    return a.lhs();
  }
};

/// \brief Selects the right-hand side of an assignment
struct right_hand_side : public std::unary_function< const assignment, data_expression >
{
  data_expression operator()(assignment const& a) const
  {
    return a.rhs();
  }
};

/// \brief Constructs an assignment_list by pairwise combining a variable and expression
/// \param variables A sequence of data variables
/// \param expressions A sequence of data expressions
/// \return The corresponding assignment list.
template < typename VariableSequence, typename ExpressionSequence >
assignment_vector make_assignment_vector(VariableSequence const& variables, ExpressionSequence const& expressions)
{
  std::vector<assignment> result;
  typename ExpressionSequence::const_iterator j=expressions.begin();
  for(typename VariableSequence::const_iterator i=variables.begin(); i!=variables.end(); ++i, ++j)
  {
    assert(j!=expressions.end());
    result.push_back(assignment(*i,*j));
  }
  assert(j==expressions.end());
  return result;
}

/// \brief Converts an iterator range to data_expression_list
/// \param variables A sequence of variables.
/// \param expressions A sequence of variables.
/// \pre length(variables) == length(expressions)
/// \note This function uses implementation details of the iterator type
/// and hence is sometimes efficient than copying all elements of the list.
template < typename VariableSequence, typename ExpressionSequence >
assignment_list make_assignment_list(const VariableSequence& variables, const ExpressionSequence& expressions)
{
  std::vector<assignment> result(make_assignment_vector(variables,expressions));
  return assignment_list(result.begin(),result.end());
}

/// \brief Returns the left hand sides of an assignment list
/// \param x An assignment list
inline
variable_list left_hand_sides(const assignment_list& x)
{
  std::vector<variable> result;
  for (assignment_list::const_iterator i = x.begin(); i != x.end(); ++i)
  {
    result.push_back(i->lhs());
  }
  return variable_list(result.begin(), result.end());
}

// template function overloads
std::string pp(const assignment_list& x);
std::string pp(const assignment_vector& x);

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_ASSIGNMENT_H

