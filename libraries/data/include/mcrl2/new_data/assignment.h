// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/new_data/assignment.h
/// \brief The class assignment.

#ifndef MCRL2_DATA_ASSIGNMENT_H
#define MCRL2_DATA_ASSIGNMENT_H

#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/aterm_traits.h"
#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/core/print.h"
#include "mcrl2/core/substitution_function.h"
#include "mcrl2/new_data/data_expression.h"
#include "mcrl2/new_data/variable.h"

namespace mcrl2 {

  namespace new_data {

    /// \brief new_data assignment.
    ///
    class assignment: public atermpp::aterm_appl, public core::substitution_function<variable, data_expression>
    {
      public:

        /// \brief Constructor.
        ///
        assignment()
          : atermpp::aterm_appl(core::detail::constructDataVarIdInit())
        {}

        /// \brief Constructor.
        ///
        /// \param[in] a A term adhering to the internal format.
        assignment(const atermpp::aterm_appl& a)
          : atermpp::aterm_appl(a)
        {}

        /// \brief Constructor
        ///
        /// \param[in] lhs The left hand side of the assignment.
        /// \param[in] rhs The right hand side of the assignment.
        assignment(const variable& lhs, const data_expression& rhs)
          : atermpp::aterm_appl(core::detail::gsMakeDataVarIdInit(lhs, rhs))
        {}

        /// \brief Returns the name of the assignment.
        inline
        variable lhs() const
        {
          return variable(atermpp::arg1(*this));
        }

        /// \brief Returns the right hand side of the assignment
        inline
        data_expression rhs() const
        {
          return atermpp::arg2(*this);
        }

        /// \brief Returns true if the sorts of the left and right hand side are equal.
        /// \return True if the assignement is well typed.
        bool is_well_typed() const
        {
          bool result = lhs().sort() == rhs().sort();
          if (!result)
          {
            std::clog << "data_assignment::is_well_typed() failed: the left and right hand sides "
               << mcrl2::core::pp(lhs()) << " and " << mcrl2::core::pp(rhs()) << " have different sorts." << std::endl;
            return false;
          }
          return true;
        }

        /// \brief Applies the substitution to a term
        /// \param x A term
        /// \return The value <tt>x[lhs() := rhs()]</tt>.
        template <typename Term>
        data_expression operator()(const Term& x) const
        {
          return atermpp::replace(x, lhs(), rhs());
        }

    }; // class assignment

    /// \brief list of assignments
    typedef atermpp::term_list<assignment> assignment_list;
    /// \brief vector of assignments
    typedef atermpp::vector<assignment>    assignment_vector;


    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Constructs an assignment_list by pairwise combining a variable and expression
    /// \param lhs A sequence of data variables
    /// \param rhs A sequence of data expressions
    /// \return The corresponding assignment list.
    inline assignment_vector make_assignment_vector(variable_list const& lhs, data_expression_list const& rhs)
    {
      assert(lhs.size() == rhs.size());
      assignment_vector result;
      variable_list::const_iterator i = lhs.begin();
      data_expression_list::const_iterator j = rhs.begin();
      for ( ; i != lhs.end(); ++i, ++j)
      {
        result.push_back(assignment(*i, *j));
      }
      return result;
    }

    ///////////////////////////////////////////////////////////////////////////////
    /// \brief Constructs an assignment_list by pairwise combining a variable and expression
    /// \param lhs A sequence of data variables
    /// \param rhs A sequence of data expressions
    /// \return The corresponding assignment list.
    inline assignment_list make_assignment_list(variable_list const& lhs, data_expression_list const& rhs)
    {
      return convert< assignment_list >(make_assignment_vector(lhs, rhs));
    }

/// \cond INTERNAL_DOCS
namespace detail {

  /// \brief Function object that applies a substitution to the right hand side of an assignment
  template <typename UnaryFunction>
  struct assignment_substitute_rhs
  {
    const UnaryFunction& f;

    assignment_substitute_rhs(const UnaryFunction& f_)
      : f(f_)
    {}

    assignment operator()(const assignment& a) const
    {
      return assignment(a.lhs(), f(a.rhs()));
    }
  };

} // namespace detail
/// \endcond

    /// \brief Applies a substitution function to data expressions appearing in the right hand
    /// sides of assignments.
    /// \param l                         A sequence of assignments
    /// \param f A function that models the concept UnaryFunction with dependent
    /// types argument_type and result_type equal to data_expression.
    /// \return The substitution result.
    template <typename UnaryFunction>
    assignment_list replace_data_expressions(const assignment_list& l, UnaryFunction f)
    {
      return atermpp::apply(l, detail::assignment_substitute_rhs<UnaryFunction>(f));
    }

    /// \brief Converts an iterator range to data_expression_list
    /// \param r A range of assignments.
    /// \note This function uses implementation details of the iterator type
    /// and hence is sometimes efficient than copying all elements of the list.
    template < typename ForwardTraversalIterator >
    inline assignment_list make_assignment_list(boost::iterator_range< ForwardTraversalIterator > const& r) {
      return convert< assignment_list >(r);
    }

    /// \brief Converts a vector to a variable_list 
    /// \param r A range of assignments.
    template < typename Expression >
    inline assignment_list make_assignment_list(atermpp::vector< Expression >const& r) {
      return convert< assignment_list >(r);
    }

    /// \brief Converts an iterator range to variable_list
    /// \param r A range of assignments.
    template < typename ForwardTraversalIterator >
    inline assignment_vector make_assignment_vector(boost::iterator_range< ForwardTraversalIterator > const& r) {
      return convert< assignment_vector >(r);
    }

    /// \brief Converts an iterator range to variable_list
    /// \param r A range of assignments.
    template < typename Expression >
    inline assignment_vector make_assignment_vector(atermpp::term_list< Expression > const& r) {
      return convert< assignment_vector >(boost::make_iterator_range(r));
    }

  } // namespace new_data

} // namespace mcrl2

#endif // MCRL2_DATA_ASSIGNMENT_H

