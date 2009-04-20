// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/new_data/data_equation.h
/// \brief The class data_equation.

#ifndef MCRL2_NEW_DATA_DATA_EQUATION_H
#define MCRL2_NEW_DATA_DATA_EQUATION_H

#include "boost/range/iterator_range.hpp"

#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/aterm_traits.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/new_data/variable.h"
#include "mcrl2/new_data/data_expression.h"
#include "mcrl2/new_data/basic_sort.h"
#include "mcrl2/new_data/function_symbol.h"
#include "mcrl2/new_data/detail/container_utility.h"

namespace mcrl2 {

  namespace new_data {

    /// \brief new_data data_equation.
    ///
    class data_equation: public atermpp::aterm_appl
    {
      public:

        /// \brief iterator range over a list of variables
        typedef boost::iterator_range< detail::term_list_random_iterator< variable > > variables_const;

        /// \brief iterator range over a constant list of variables
        typedef boost::iterator_range< detail::term_list_random_iterator< variable > > variables_const_range;

      public:

        /// \brief Constructor.
        ///
        data_equation()
          : atermpp::aterm_appl(core::detail::constructDataEqn())
        {}

        /// \internal
        /// \brief Constructor
        ///
        /// \param[in] a An aterm adhering to the internal format.
        data_equation(const atermpp::aterm_appl& a)
          : atermpp::aterm_appl(a)
        { }

        /// \brief Constructor
        ///
        /// \param[in] variables The free variables of the data_equation.
        /// \param[in] condition The condition of the data_equation.
        /// \param[in] lhs The left hand side of the data_equation.
        /// \param[in] rhs The right hand side of the data_equation.
        data_equation(const variable_list& variables,
                      const data_expression& condition,
                      const data_expression& lhs,
                      const data_expression& rhs)
          : atermpp::aterm_appl(core::detail::gsMakeDataEqn(
                  variables, condition, lhs, rhs))
        {}

        /// \brief Constructor
        /// \overload for variable list as vector
        data_equation(const variable_vector& variables,
                      const data_expression& condition,
                      const data_expression& lhs,
                      const data_expression& rhs)
          : atermpp::aterm_appl(core::detail::gsMakeDataEqn(
                  make_variable_list(variables), condition, lhs, rhs))
        {}

        /// \brief Constructor
        ///
        /// \overload for variable list as iterator range
        template < typename ForwardTraversalIterator >
        data_equation(const typename boost::iterator_range< ForwardTraversalIterator >& variables,
                      const data_expression& condition,
                      const data_expression& lhs,
                      const data_expression& rhs)
          : atermpp::aterm_appl(core::detail::gsMakeDataEqn(
                  make_variable_list(variables), condition, lhs, rhs))
        {}

        /// \brief Constructor
        ///
        /// \param[in] variables The free variables of the data_equation.
        /// \param[in] lhs The left hand side of the data_equation.
        /// \param[in] rhs The right hand side of the data_equation.
        /// \post this is the new_data equation representing the input, with
        ///       condition true
        template < typename ForwardTraversalIterator >
        data_equation(const typename boost::iterator_range< ForwardTraversalIterator >& variables,
                      const data_expression& lhs,
                      const data_expression& rhs)
          : atermpp::aterm_appl(core::detail::gsMakeDataEqn(
                  variable_list(variables.begin(), variables.end()), core::detail::gsMakeNil(), lhs, rhs))
        {}

        /// \brief Constructor
        ///
        /// \param[in] variables The free variables of the data_equation.
        /// \param[in] lhs The left hand side of the data_equation.
        /// \param[in] rhs The right hand side of the data_equation.
        /// \post this is the new_data equation representing the input, with
        ///       condition true
        data_equation(const variable_list& variables,
                      const data_expression& lhs,
                      const data_expression& rhs)
          : atermpp::aterm_appl(core::detail::gsMakeDataEqn(
                  variables, core::detail::gsMakeNil(), lhs, rhs))
        {}

        /// \brief Constructor
        ///
        /// \param[in] variables The free variables of the data_equation.
        /// \param[in] lhs The left hand side of the data_equation.
        /// \param[in] rhs The right hand side of the data_equation.
        /// \post this is the new_data equation representing the input, with
        ///       condition true
        data_equation(const variable_vector& variables,
                      const data_expression& lhs,
                      const data_expression& rhs)
          : atermpp::aterm_appl(core::detail::gsMakeDataEqn(
                  variable_list(variables.begin(), variables.end()), core::detail::gsMakeNil(), lhs, rhs))
        {}

        /// \brief Constructor
        ///
        /// \param[in] condition The condition of the new_data equation.
        /// \param[in] lhs The left hand side of the new_data equation.
        /// \param[in] rhs The right hand side of the new_data equation.
        /// \post this is the new_data equations representing the input, without
        ///       variables, and condition true
        data_equation(const data_expression& condition,
                      const data_expression& lhs,
                      const data_expression& rhs)
          : atermpp::aterm_appl(core::detail::gsMakeDataEqn(
                  variable_list(), condition, lhs, rhs))
        {}

        /// \brief Constructor
        ///
        /// \param[in] lhs The left hand side of the new_data equation.
        /// \param[in] rhs The right hand side of the new_data equation.
        /// \post this is the new_data equations representing the input, without
        ///       variables, and condition true
        data_equation(const data_expression& lhs,
                      const data_expression& rhs)
          : atermpp::aterm_appl(core::detail::gsMakeDataEqn(
                  variable_list(), core::detail::gsMakeNil(), lhs, rhs))
        {}

        /// \brief Returns the variables of the new_data equation.
        inline
        variables_const_range variables() const
        {
          return boost::make_iterator_range(add_random_access< variable >(atermpp::list_arg1(appl())));
        }

        /// \brief Returns the condition of the new_data equation.
        inline
        data_expression condition() const
        {
          return atermpp::arg2(*this);
        }

        /// \brief Returns the left hand side of the new_data equation.
        inline
        data_expression lhs() const
        {
          return atermpp::arg3(*this);
        }

        /// \brief Returns the right hand side of the new_data equation.
        inline
        data_expression rhs() const
        {
          return atermpp::arg4(*this);
        }

    }; // class data_equation

    /// \brief list of data_equations
    typedef atermpp::term_list< data_equation >    data_equation_list;

    /// \brief list of data_equations
    typedef atermpp::vector< data_equation >       data_equation_vector;

  } // namespace new_data

} // namespace mcrl2

#endif // MCRL2_NEW_DATA_DATA_EQUATION_H

