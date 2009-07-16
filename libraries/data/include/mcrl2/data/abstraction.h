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

#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/data/detail/construction_utility.h"
#include "mcrl2/data/detail/container_utility.h"
#include "mcrl2/core/detail/constructors.h"

namespace mcrl2 {

  namespace data {

    /// \brief Expression for abstraction, i.e. an expression binding a
    ///        number of variables.
    ///
    /// An example of an abstraction is lambda x,y:Pos . f(x,y),
    /// where lambda is the binding operator, x,y are the variables,
    /// and f(x,y) is the body of the abstraction.
    ///
    class abstraction: public data_expression
    {
      protected:

        /// \brief base class for abstraction types
        struct binder_type : public atermpp::aterm_appl {
          binder_type(atermpp::aterm_appl const& e) : atermpp::aterm_appl(e)
          {}
        };

      public:

        /// \brief Iterator range over bound variables
        typedef atermpp::term_list< variable > variables_const_range;

        /// \brief Type for lambda abstracions
        struct lambda : public detail::singleton_expression< abstraction::lambda, abstraction::binder_type > {
          static atermpp::aterm_appl initialise() {
            return core::detail::gsMakeLambda();
          }
        };

        /// \brief Type for universal quantifications
        struct forall : public detail::singleton_expression< abstraction::forall, abstraction::binder_type > {
          static atermpp::aterm_appl initialise() {
            return core::detail::gsMakeForall();
          }
        };

        /// \brief Type for existential quantifications
        struct exists : public detail::singleton_expression< abstraction::exists, abstraction::binder_type > {
          static atermpp::aterm_appl initialise() {
            return core::detail::gsMakeExists();
          }
        };

      public:

        /// Default constructor for abstraction (does not entail a
        /// valid data expression.
        ///
        abstraction()
          : data_expression(core::detail::constructBinder())
        {}

        /// Construct abstraction from a data expression.
        /// \param[in] d a data expression
        /// \pre d.is_abstraction()
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
        abstraction(const abstraction::binder_type& binding_operator,
                    const Container& variables,
                    const data_expression& body,
                    typename detail::enable_if_container< Container, variable >::type* = 0)
          : data_expression(core::detail::gsMakeBinder(binding_operator, convert< variable_list >(variables), body))
        {
          assert(!variables.empty());
        }

        /// \brief Returns the binding operator of the abstraction
        inline
        abstraction::binder_type binding_operator() const
        {
          return atermpp::arg1(*this);
        }

        /// \brief Returns the variables of the abstraction
        inline
        variables_const_range variables() const
        {
          return atermpp::list_arg2(*this);
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
          return binding_operator() == abstraction::lambda();
        }

        /// \brief Returns true iff the binding operator is "forall"
        inline
        bool is_forall() const
        {
          return binding_operator() == abstraction::forall();
        }

        /// \brief Returns true iff the binding operator is "exists"
        inline
        bool is_exists() const
        {
          return binding_operator() == abstraction::exists();
        }

    }; // class abstraction

  } // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_ABSTRACTION_H

