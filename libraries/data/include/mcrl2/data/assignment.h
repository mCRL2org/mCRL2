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
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/core/detail/soundness_checks.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/identifier.h"
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
      : atermpp::aterm_appl(core::detail::constructWhrDecl())
    {}

    /// \brief Constructor.
    /// \param term A term
    assignment_expression(const atermpp::aterm& term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_rule_WhrDecl(*this));
    }
};

/// \brief list of assignment_expressions
typedef atermpp::term_list<assignment_expression> assignment_expression_list;

/// \brief vector of assignment_expressions
typedef std::vector<assignment_expression>    assignment_expression_vector;


/// \brief Assignment of a data expression to a variable
class assignment: public assignment_expression
{
  public:
    /// \brief Default constructor.
    assignment()
      : assignment_expression(core::detail::constructDataVarIdInit())
    {}

    /// \brief Constructor.
    /// \param term A term
    assignment(const atermpp::aterm& term)
      : assignment_expression(term)
    {
      assert(core::detail::check_term_DataVarIdInit(*this));
    }

    /// \brief Constructor.
    assignment(const variable& lhs, const data_expression& rhs)
      : assignment_expression(core::detail::gsMakeDataVarIdInit(lhs, rhs))
    {}

    const variable& lhs() const
    {
      return atermpp::aterm_cast<const variable>(atermpp::arg1(*this));
    }

    const data_expression& rhs() const
    {
      return atermpp::aterm_cast<const data_expression>(atermpp::arg2(*this));
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
    /// \param[in] x A term
    /// \return The value <tt>x[lhs() := rhs()]</tt>.
    template < typename Expression >
    data_expression operator()(const Expression& /*x*/) const
    {
      throw std::runtime_error("data::assignment::operator(const Expression&) is a deprecated interface!");
      return data_expression();
    }
//--- end user section assignment ---//
};

/// \brief list of assignments
typedef atermpp::term_list<assignment> assignment_list;

/// \brief vector of assignments
typedef std::vector<assignment>    assignment_vector;


/// \brief Test for a assignment expression
/// \param t A term
/// \return True if it is a assignment expression
inline
bool is_assignment(const atermpp::aterm_appl& x)
{
  return core::detail::gsIsDataVarIdInit(x);
}


/// \brief Assignment of a data expression to a string
class identifier_assignment: public assignment_expression
{
  public:
    /// \brief Default constructor.
    identifier_assignment()
      : assignment_expression(core::detail::constructIdInit())
    {}

    /// \brief Constructor.
    /// \param term A term
    identifier_assignment(const atermpp::aterm& term)
      : assignment_expression(term)
    {
      assert(core::detail::check_term_IdInit(*this));
    }

    /// \brief Constructor.
    identifier_assignment(const core::identifier_string& lhs, const data_expression& rhs)
      : assignment_expression(core::detail::gsMakeIdInit(lhs, rhs))
    {}

    /// \brief Constructor.
    identifier_assignment(const std::string& lhs, const data_expression& rhs)
      : assignment_expression(core::detail::gsMakeIdInit(core::identifier_string(lhs), rhs))
    {}

    const core::identifier_string& lhs() const
    {
      return atermpp::aterm_cast<const core::identifier_string>(atermpp::arg1(*this));
    }

    const data_expression& rhs() const
    {
      return atermpp::aterm_cast<const data_expression>(atermpp::arg2(*this));
    }
//--- start user section identifier_assignment ---//
    /// \brief Applies the assignment to a variable
    /// \param[in] x An identifier string
    /// \return The value <tt>x[lhs() := rhs()]</tt>.
    data_expression operator()(const identifier& x) const
    {
      return x == lhs() ? rhs() : data_expression(x);
    }

    /// \brief Applies the assignment to a term
    /// \param[in] x A term
    /// \return The value <tt>x[lhs() := rhs()]</tt>.
    template < typename Expression >
    data_expression operator()(const Expression& /*x*/) const
    {
      throw std::runtime_error("data::identifier_assignment::operator(const Expression&) is a deprecated interface!");
      return data_expression();
    }
//--- end user section identifier_assignment ---//
};

/// \brief list of identifier_assignments
typedef atermpp::term_list<identifier_assignment> identifier_assignment_list;

/// \brief vector of identifier_assignments
typedef std::vector<identifier_assignment>    identifier_assignment_vector;


/// \brief Test for a identifier_assignment expression
/// \param t A term
/// \return True if it is a identifier_assignment expression
inline
bool is_identifier_assignment(const atermpp::aterm_appl& x)
{
  return core::detail::gsIsIdInit(x);
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
/// \param lhs A sequence of data variables
/// \param rhs A sequence of data expressions
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
/// \param r A range of assignments.
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

/// \brief Returns true if the term x is an assignment expression
/// \param x An expression
/// \return True if the term t is an assignment expression
inline
bool is_assignment_expression(const atermpp::aterm_appl& x)
{
  // TODO: this code should be generated
  return
    core::detail::gsIsDataVarIdInit(x) ||
    core::detail::gsIsIdInit(x)
    ;
}

// template function overloads
std::string pp(const assignment& x);
std::string pp(const assignment_list& x);
std::string pp(const assignment_vector& x);

} // namespace data

} // namespace mcrl2

namespace std {
//--- start generated swap functions ---//
template <>
inline void swap(mcrl2::data::assignment_expression& t1, mcrl2::data::assignment_expression& t2)
{
  t1.swap(t2);
}

template <>
inline void swap(mcrl2::data::assignment& t1, mcrl2::data::assignment& t2)
{
  t1.swap(t2);
}

template <>
inline void swap(mcrl2::data::identifier_assignment& t1, mcrl2::data::identifier_assignment& t2)
{
  t1.swap(t2);
}
//--- end generated swap functions ---//
} // namespace std

#endif // MCRL2_DATA_ASSIGNMENT_H

