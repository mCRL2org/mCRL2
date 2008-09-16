// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/data_equation.h
/// \brief The class data_equation.

#ifndef MCRL2_DATA_DATA_EQUATION_H
#define MCRL2_DATA_DATA_EQUATION_H


#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/aterm_traits.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/basic_sort.h"
#include "mcrl2/data/function_symbol.h"

namespace mcrl2 {
  
  namespace data {

    /// \brief data data_equation.
    ///
    class data_equation: public atermpp::aterm_appl
    {
      protected:
        variable_list m_variables; ///< Free variables of the data equation

      public:

        /// \brief Constructor.
        ///
        data_equation()
          : atermpp::aterm_appl(core::detail::constructDataEqn())
        {}

        /// \internal
        data_equation(const atermpp::aterm_appl& a)
          : atermpp::aterm_appl(a)
        {}

        /// \brief Constructor
        ///
        /// \param[in] variables The free variables of the data_equation.
        /// \param[in] condition The condition of the data_equation.
        /// \param[in] lhs The left hand side of the data_equation.
        /// \param[in] rhs The right hand side of the data_equation.
        data_equation(const boost::iterator_range<variable_list::const_iterator>& variables,
                      const data_expression& condition,
                      const data_expression& lhs,
                      const data_expression& rhs)
          : atermpp::aterm_appl(core::detail::gsMakeDataEqn(
              atermpp::term_list<variable>(variables.begin(), variables.end()),
              condition, lhs, rhs)),
            m_variables(variables.begin(), variables.end())
        {}

        /// \brief Constructor
        ///
        /// \param[in] variables The free variables of the data_equation.
        /// \param[in] lhs The left hand side of the data_equation.
        /// \param[in] rhs The right hand side of the data_equation.
        /// \post this is the data equation representing the input, with
        //        condition true
        //
        //TODO: Fix true
        data_equation(const boost::iterator_range<variable_list::const_iterator>& variables,
                      const data_expression& lhs,
                      const data_expression& rhs)
          : atermpp::aterm_appl(core::detail::gsMakeDataEqn(
              atermpp::term_list<variable>(variables.begin(), variables.end()),
              function_symbol("true", basic_sort("Bool")), lhs, rhs)),
            m_variables(variables.begin(), variables.end())
        {}

        /// \brief Returns the variables of the data equation.
        inline
        boost::iterator_range<variable_list::const_iterator> variables() const
        {
          return boost::make_iterator_range(m_variables);
        }

        /// \brief Returns the condition of the data equation.
        inline
        data_expression condition() const
        {
          return atermpp::arg2(*this);
        }

        /// \brief Returns the left hand side of the data equation.
        inline
        data_expression lhs() const
        {
          return atermpp::arg3(*this);
        }
        
        /// \brief Returns the right hand side of the data equation.
        inline
        data_expression rhs() const
        {
          return atermpp::arg4(*this);
        }

    }; // class data_equation

    /// \brief list of data_equations
    ///
    typedef atermpp::vector<data_equation> data_equation_list;

  } // namespace data

} // namespace mcrl2

/// \cond INTERNAL_DOCS
MCRL2_ATERM_TRAITS_SPECIALIZATION(mcrl2::data::data_equation)
/// \endcond

#endif // MCRL2_DATA_DATA_EQUATION_H

