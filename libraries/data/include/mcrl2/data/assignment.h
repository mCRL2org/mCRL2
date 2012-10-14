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
#include "boost/iterator/transform_iterator.hpp"

#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/combine_iterator.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/core/detail/soundness_checks.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/identifier.h"
#include "mcrl2/data/variable.h"

namespace mcrl2
{

namespace data
{

class assignment_expression: public atermpp::aterm_appl,
  public std::unary_function<variable, data_expression>
{
  public:
    typedef variable variable_type;
    typedef data_expression expression_type;

    assignment_expression()
      : atermpp::aterm_appl(core::detail::constructWhrDecl())
    {}

    assignment_expression(const aterm &term)
      : atermpp::aterm_appl(term)
    {
      assert(core::detail::check_rule_WhrDecl(atermpp::aterm_appl(term)));
    }

    assignment_expression(const variable v, const data_expression d)
      : atermpp::aterm_appl(core::detail::gsMakeDataVarIdInit(v,d))
    {}

    variable lhs() const
    {
      return atermpp::arg1(*this);
    }

    data_expression rhs() const
    {
      return data_expression(atermpp::arg2(*this));
    }
};

/// \brief list of assignment expressions
typedef atermpp::term_list<assignment_expression> assignment_expression_list;
/// \brief vector of assignment expressions
typedef std::vector<assignment_expression>    assignment_expression_vector;

//--- start generated classes ---//

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
    assignment(const aterm& term)
      : assignment_expression(atermpp::aterm_appl(term))
    {
      assert(core::detail::check_term_DataVarIdInit(m_term));
    }

    /// \brief Constructor.
    assignment(const variable& lhs, const data_expression& rhs)
      : assignment_expression(core::detail::gsMakeDataVarIdInit(lhs, rhs))
    {}

    const variable &lhs() const
    {
      return atermpp::aterm_cast<const variable>(atermpp::arg1(*this));
    }

    const data_expression &rhs() const
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
bool is_assignment(const assignment_expression& t)
{
  return core::detail::gsIsDataVarIdInit(t);
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
    identifier_assignment(const atermpp::aterm_appl& term)
      : assignment_expression(term)
    {
      assert(core::detail::check_term_IdInit(m_term));
    }

    /// \brief Constructor.
    identifier_assignment(const core::identifier_string& lhs, const data_expression& rhs)
      : assignment_expression(core::detail::gsMakeIdInit(lhs, rhs))
    {}

    /// \brief Constructor.
    identifier_assignment(const std::string& lhs, const data_expression& rhs)
      : assignment_expression(core::detail::gsMakeIdInit(core::identifier_string(lhs), rhs))
    {}

    core::identifier_string lhs() const
    {
      return atermpp::aterm_cast<core::identifier_string>(atermpp::arg1(*this));
    }

    data_expression rhs() const
    {
      return data_expression(atermpp::arg2(*this));
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
bool is_identifier_assignment(const assignment_expression& t)
{
  return core::detail::gsIsIdInit(t);
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

/// \brief Returns the corresponding sequence of left-hand sides for a given sequence assignment (lazy)
/// \param[in] assignments the sequence of unmutable assignments
/// \return a sequence r such that assignments.i.lhs() = r.i.lhs() for all i
template < typename Container >
inline
boost::iterator_range< boost::transform_iterator< left_hand_side, typename Container::const_iterator > >
make_assignment_left_hand_side_range(Container const& assignments, typename atermpp::detail::enable_if_container< Container, assignment >::type* = 0)
{
  return boost::iterator_range< boost::transform_iterator< left_hand_side, typename Container::const_iterator > >(assignments);
}

/// \brief Returns the corresponding sequence of left-hand sides for a given sequence assignment (lazy)
/// \param[in] assignments the sequence of unmutable assignments
/// \return a sequence r such that assignments.i.lhs() = r.i.lhs() for all i
template < typename Container >
boost::iterator_range< boost::transform_iterator< right_hand_side, typename Container::const_iterator > >
make_assignment_right_hand_side_range(Container const& assignments, typename atermpp::detail::enable_if_container< Container, assignment >::type* = 0)
{
  return boost::iterator_range< boost::transform_iterator< right_hand_side, typename Container::const_iterator > >(assignments);
}

/// \brief Constructs a range of assignments from sequences of variables and expressions by pair-wise combination
template < typename VariableSequence, typename ExpressionSequence >
boost::iterator_range< atermpp::detail::combine_iterator< atermpp::detail::construct< assignment >&, typename VariableSequence::const_iterator, typename ExpressionSequence::const_iterator > >
make_assignment_range(VariableSequence const& variables, ExpressionSequence const& expressions,
                      typename atermpp::detail::enable_if_container< VariableSequence, variable >::type* = 0,
                      typename atermpp::detail::enable_if_container< ExpressionSequence, data_expression >::type* = 0)
{
  assert(boost::distance(variables) == boost::distance(expressions));

  return boost::make_iterator_range(
           atermpp::detail::make_combine_iterator(atermpp::detail::construct< assignment >(), variables.begin(), expressions.begin()),
           atermpp::detail::make_combine_iterator(atermpp::detail::construct< assignment >(), variables.end(), expressions.end()));
}

/// \brief Constructs an assignment_list by pairwise combining a variable and expression
/// \param lhs A sequence of data variables
/// \param rhs A sequence of data expressions
/// \return The corresponding assignment list.
template < typename VariableSequence, typename ExpressionSequence >
assignment_vector make_assignment_vector(VariableSequence const& variables, ExpressionSequence const& expressions)
{
  return atermpp::convert< assignment_vector >(make_assignment_vector(variables, expressions));
}

/// \brief Converts an iterator range to data_expression_list
/// \param r A range of assignments.
/// \note This function uses implementation details of the iterator type
/// and hence is sometimes efficient than copying all elements of the list.
template < typename VariableSequence, typename ExpressionSequence >
assignment_list make_assignment_list(VariableSequence const& variables, ExpressionSequence const& expressions)
{
  return atermpp::convert< assignment_list >(make_assignment_range(variables, expressions));
}

/// \brief Converts an iterator range to data_expression_list
/// \param r A range of assignments.
/// \note This function uses implementation details of the iterator type
/// and hence is sometimes efficient than copying all elements of the list.
template < typename Container >
inline assignment_list make_assignment_list(Container const& container, typename atermpp::detail::enable_if_container< Container, assignment >::type* = 0)
{
  return atermpp::convert< assignment_list >(container);
}

/// \brief Converts an iterator range to data_expression_list
/// \param r A range of assignments.
/// \note This function uses implementation details of the iterator type
/// and hence is sometimes efficient than copying all elements of the list.
template < typename Container >
inline assignment_list make_assignment_vector(Container const& container, typename atermpp::detail::enable_if_container< Container, assignment >::type* = 0)
{
  return atermpp::convert< assignment_vector >(container);
}

////--- start generated is-functions ---//
//
//    /// \brief Test for a assignment expression
//    /// \param t A term
//    /// \return True if it is a assignment expression
//    inline
//    bool is_assignment(const assignment_expression& t)
//    {
//      return core::detail::gsIsDataVarIdInit(t);
//    }
//
//    /// \brief Test for a identifier_assignment expression
//    /// \param t A term
//    /// \return True if it is a identifier_assignment expression
//    inline
//    bool is_identifier_assignment(const assignment_expression& t)
//    {
//      return core::detail::gsIsIdInit(t);
//    }
////--- end generated is-functions ---//

// template function overloads
std::string pp(const assignment& x);
std::string pp(const assignment_list& x);
std::string pp(const assignment_vector& x);

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_ASSIGNMENT_H

