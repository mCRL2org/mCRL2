// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/abstraction.h
/// \brief The class abstraction.

#ifndef MCRL2_DATA_ABSTRACTION_H
#define MCRL2_DATA_ABSTRACTION_H

#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/aterm_traits.h"
#include "mcrl2/atermpp/vector.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/data/detail/container_utility.h"

namespace mcrl2 {

  namespace data {

    /// \brief function symbol.
    ///
    class abstraction: public data_expression
    {
      public:

        /// \brief Iterator range over bound variables
        typedef boost::iterator_range< detail::term_list_random_iterator< variable > > variables_const_range;

      protected:

        /// \brief Transforms a string to an internally used binding operator.
        ///
        /// \param[in] s String denoting a binding operator.
        /// \pre binding_operator is one of "lambda", "forall", "exists",
        ///      "setcomprehension" or "bagcomprehension".
        /// \return The internally used binding operator which is equivalent to o.
        inline atermpp::aterm_appl string_to_binding_operator(const std::string& s) const
        {
          atermpp::aterm_appl result;

          if (s == "lambda")
          {
            result = core::detail::gsMakeLambda();
          }
          else if (s == "forall")
          {
            result = core::detail::gsMakeForall();
          }
          else if (s == "exists")
          {
            result = core::detail::gsMakeExists();
          }
          else
          {
            assert(false);
          }

          return result;
        }

        /// \brief Transforms an internally used binding operator to a string.
        ///
        /// \param[in] o The internally used binding operator.
        /// \return The string equivalent to o.
        inline
        std::string binding_operator_to_string(const atermpp::aterm_appl& o) const
        {
          std::string result;

          if (core::detail::gsIsLambda(o))
          {
            result = "lambda";
          }
          else if (core::detail::gsIsForall(o))
          {
            result = "forall";
          }
          else if (core::detail::gsIsExists(o))
          {
            result = "exists";
          }
          else
          {
            assert(false);
          }

          return result;
        }

      public:

        /// Constructor.
        ///
        abstraction()
          : data_expression(core::detail::constructBinder())
        {}

        /// Constructor.
        abstraction(const data_expression& d)
          : data_expression(d)
        {
          assert(d.is_abstraction());
        }

        /// Constructor.
        ///
        /// \param[in] binding_operator The binding operator of the abstraction.
        ///              This may be one of "lambda", "forall",
        ///              "exists", "setcomprehension", "bagcomprehension".
        /// \param[in] variables A nonempty list of binding variables (objects of type variable)
        /// \param[in] body The body of the abstraction.
        /// \pre binding_operator is one of "lambda", "forall", "exists",
        ///      "setcomprehension" or "bagcomprehension".
        /// \pre variables is not empty.
        template < typename Container >
        abstraction(const std::string& binding_operator,
                    const Container& variables,
                    const data_expression& body,
                    typename detail::enable_if_container< Container, variable >::type* = 0)
          : data_expression(core::detail::gsMakeBinder(string_to_binding_operator(binding_operator), convert< variable_list >(variables), body))
        {
          assert(!variables.empty());
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

        /// \brief Returns the binding operator of the abstraction
        inline
        std::string binding_operator() const
        {
          return binding_operator_to_string(atermpp::arg1(*this));
        }

        /// \brief Returns the variables of the abstraction
        inline
        variables_const_range variables() const
        {
          return boost::make_iterator_range(add_random_access< variable >(atermpp::list_arg2(*this)));
        }

        /// \brief Returns the body of the abstraction
        inline
        data_expression body() const
        {
          return atermpp::arg3(*this);
        }

        /// \brief Returns true iff the binding operator is "lambda"
        inline
        bool is_lambda() const
        {
          return binding_operator() == "lambda";
        }

        /// \brief Returns true iff the binding operator is "forall"
        inline
        bool is_forall() const
        {
          return binding_operator() == "forall";
        }

        /// \brief Returns true iff the binding operator is "exists"
        inline
        bool is_exists() const
        {
          return binding_operator() == "exists";
        }

    }; // class abstraction

    /// \brief list of abstractions
    typedef atermpp::term_list<abstraction> abstraction_list;

    /// \brief vector of abstractions
    typedef atermpp::vector<abstraction>    abstraction_vector;

  } // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_ABSTRACTION_H

