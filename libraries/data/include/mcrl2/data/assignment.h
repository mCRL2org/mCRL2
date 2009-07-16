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

#include "boost/iterator/transform_iterator.hpp"

#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/core/substitution_function.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/data/print.h"

namespace mcrl2 {

  namespace data {

    // forward declaration for application operation of assignment
    template <typename Container, typename Substitution >
    Container replace_free_variables(Container const& container, Substitution replace_function);

    /// \brief assignment.
    ///
    /// An example of an assignment is x := e, where x is a variable and e an
    /// expression.
    class assignment: public atermpp::aterm_appl, public core::substitution_function<variable, data_expression>
    {
      public:

        /// \brief Default constructor. Note that this does not entail a valid
        ///        assignment.
        ///
        assignment()
          : atermpp::aterm_appl(core::detail::constructDataVarIdInit())
        {}

        /// \brief Constructor for assignment from a term.
        ///
        /// \param[in] a A term adhering to the internal format.
        /// \pre a is an expression adhering to the format of an assignment.
        assignment(const atermpp::aterm_appl& a)
          : atermpp::aterm_appl(a)
        {
//          assert(is_assignment(a));
          assert(core::detail::gsIsDataVarIdInit(a));
        }

        /// \brief Constructor
        ///
        /// \param[in] lhs The left hand side of the assignment.
        /// \param[in] rhs The right hand side of the assignment.
        assignment(const variable& lhs, const data_expression& rhs)
          : atermpp::aterm_appl(core::detail::gsMakeDataVarIdInit(lhs, rhs))
        {}

        /// \brief Returns the left hand side of the assignment, i.e. the
        ///        variable that gets assigned a value.
        variable lhs() const
        {
          return variable(atermpp::arg1(*this));
        }

        /// \brief Returns the right hand side of the assignment, i.e. the
        ///        value that is assigned.
        data_expression rhs() const
        {
          return atermpp::arg2(*this);
        }

        /// \brief Applies the assignment to a variable
        /// \param[in] x A variable
        /// \return The value <tt>x[lhs() := rhs()]</tt>.
        data_expression operator()(const variable& x) const
        {
          return x == lhs() ? rhs() : data_expression(x);
        }

        /// \brief Applies the assignment to a term
        /// \param[in] x A term
        /// \return The value <tt>x[lhs() := rhs()]</tt>.
        template < typename Expression >
        data_expression operator()(const Expression& x) const
        {
          return data::replace_free_variables< Expression, assignment const& >(x, *this);
        }

    }; // class assignment

    /// \brief list of assignments
    typedef atermpp::term_list<assignment> assignment_list;
    /// \brief vector of assignments
    typedef atermpp::vector<assignment>    assignment_vector;

/*
    /// \brief recogniser for an assignment
    /// \param[in] a a term
    /// \return true iff a is an assignment
    bool is_assignment(const atermpp::aterm_appl& a)
    {
      return core::detail::gsIsDataVarIdInit(a);
    }
*/

    /// \brief Selects the right-hand side of an assignment
    struct left_hand_side : public std::unary_function< const assignment, variable >
    {
      variable operator()(assignment const& a) const {
        return a.lhs();
      }
    };

    /// \brief Selects the right-hand side of an assignment
    struct right_hand_side : public std::unary_function< const assignment, data_expression >
    {
      data_expression operator()(assignment const& a) const {
        return a.rhs();
      }
    };

    /// \brief Returns the corresponding sequence of left-hand sides for a given sequence assignment (lazy)
    /// \param[in] assignments the sequence of unmutable assignments
    /// \return a sequence r such that assignments.i.lhs() = r.i.lhs() for all i
    template < typename Container >
    inline 
    boost::iterator_range< boost::transform_iterator< left_hand_side, typename Container::const_iterator > >
    make_assignment_left_hand_side_range(Container const& assignments, typename detail::enable_if_container< Container, assignment >::type* = 0)
    {
      return boost::iterator_range< boost::transform_iterator< left_hand_side, typename Container::const_iterator > >(assignments);
    }

    /// \brief Returns the corresponding sequence of left-hand sides for a given sequence assignment (lazy)
    /// \param[in] assignments the sequence of unmutable assignments
    /// \return a sequence r such that assignments.i.lhs() = r.i.lhs() for all i
    template < typename Container >
    boost::iterator_range< boost::transform_iterator< right_hand_side, typename Container::const_iterator > >
    make_assignment_right_hand_side_range(Container const& assignments, typename detail::enable_if_container< Container, assignment >::type* = 0)
    {
      return boost::iterator_range< boost::transform_iterator< right_hand_side, typename Container::const_iterator > >(assignments);
    }

    /// \brief Constructs a range of assignments from sequences of variables and expressions by pair-wise combination
    template < typename VariableSequence, typename ExpressionSequence >
    boost::iterator_range< detail::combine_iterator< detail::construct< assignment >&, typename VariableSequence::const_iterator, typename ExpressionSequence::const_iterator > >
    make_assignment_range(VariableSequence const& variables, ExpressionSequence const& expressions,
                                                  typename detail::enable_if_container< VariableSequence, variable >::type* = 0,
                                                  typename detail::enable_if_container< ExpressionSequence, data_expression >::type* = 0)
    {
      assert(boost::distance(variables) == boost::distance(expressions));

      return boost::make_iterator_range(
        detail::make_combine_iterator(detail::construct< assignment >(), variables.begin(), expressions.begin()),
        detail::make_combine_iterator(detail::construct< assignment >(), variables.end(), expressions.end()));
    }

    /// \brief Constructs an assignment_list by pairwise combining a variable and expression
    /// \param lhs A sequence of data variables
    /// \param rhs A sequence of data expressions
    /// \return The corresponding assignment list.
    template < typename VariableSequence, typename ExpressionSequence >
    assignment_vector make_assignment_vector(VariableSequence const& variables, ExpressionSequence const& expressions)
    {
      return convert< assignment_vector >(make_assignment_vector(variables, expressions));
    }

    /// \brief Converts an iterator range to data_expression_list
    /// \param r A range of assignments.
    /// \note This function uses implementation details of the iterator type
    /// and hence is sometimes efficient than copying all elements of the list.
    template < typename VariableSequence, typename ExpressionSequence >
    assignment_list make_assignment_list(VariableSequence const& variables, ExpressionSequence const& expressions)
    {
      return convert< assignment_list >(make_assignment_range(variables, expressions));
    }

    /// \brief Converts an iterator range to data_expression_list
    /// \param r A range of assignments.
    /// \note This function uses implementation details of the iterator type
    /// and hence is sometimes efficient than copying all elements of the list.
    template < typename Container >
    inline assignment_list make_assignment_list(Container const& container, typename detail::enable_if_container< Container, assignment >::type* = 0) {
      return convert< assignment_list >(container);
    }

    /// \brief Converts an iterator range to data_expression_list
    /// \param r A range of assignments.
    /// \note This function uses implementation details of the iterator type
    /// and hence is sometimes efficient than copying all elements of the list.
    template < typename Container >
    inline assignment_list make_assignment_vector(Container const& container, typename detail::enable_if_container< Container, assignment >::type* = 0) {
      return convert< assignment_vector >(container);
    }

  } // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_ASSIGNMENT_H

