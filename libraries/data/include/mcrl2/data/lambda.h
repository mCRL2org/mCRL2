// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/lambda.h
/// \brief The class lambda.

#ifndef MCRL2_DATA_LAMBDA_H
#define MCRL2_DATA_LAMBDA_H

#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/data/abstraction.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/data/application.h"

namespace mcrl2 {

  namespace data {

    /// \brief function symbol.
    ///
    class lambda: public abstraction
    {
      public:

        /// Constructor.
        ///
        /// \param[in] d A data expression.
        /// \pre d is a lambda abstraction.
        lambda(const data_expression& d)
          : abstraction(d)
        {
          assert(d.is_abstraction());
          assert(static_cast<abstraction>(d).binding_operator() == abstraction::lambda());
        }

        /// Constructor.
        ///
        /// \param[in] variable A nonempty list of binding variables.
        /// \param[in] body The body of the lambda abstraction.
        /// \pre variables is not empty.
        lambda(const variable& variable,
               const data_expression& body)
          : abstraction(abstraction::lambda(), convert< variable_list >(make_list(variable)), body)
        {
        }

        /// Constructor.
        ///
        /// \param[in] variables A nonempty list of binding variables (objects of type variable).
        /// \param[in] body The body of the lambda abstraction.
        /// \pre variables is not empty.
        template < typename Container >
        lambda(const Container& variables,
               const data_expression& body,
               typename detail::enable_if_container< Container, variable >::type* = 0)
          : abstraction(abstraction::lambda(), variables, body)
        {
          assert(!variables.empty());
        }

        /// \brief Returns the application of this lambda abstraction to an argument.
        /// \pre this->sort() is a function sort.
        /// \param[in] e The data expression to which the function symbol is applied
        application operator()(const data_expression& e) const
        {
          assert(this->sort().is_function_sort());
          return application(*this, e);
        }

        /*  Should be enabled when the implementation in data_expression is
         * removed
        /// \overload
        inline
        sort_expression sort() const
        {
          return function_sort(sorts_of_data_expressions(boost::make_iterator_range(m_variables.begin(), m_variables.end())), body().sort());
        }
        */

    }; // class lambda

    /// \brief list of lambdas
    ///
    typedef atermpp::term_list<lambda> lambda_list;

  } // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_LAMBDA_H

