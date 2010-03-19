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
#include "mcrl2/data/binder_type.h"
#include "mcrl2/data/detail/construction_utility.h"
#include "mcrl2/data/detail/container_utility.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/core/detail/soundness_checks.h"

namespace mcrl2 {

  namespace data {

    namespace detail {

//--- start generated class ---//
/// \brief An abstraction expression.
class abstraction_base: public data_expression
{
  public:
    /// \brief Constructor.
    /// \param term A term
    abstraction_base(atermpp::aterm_appl term)
      : data_expression(term)
    {
      assert(core::detail::check_term_Binder(m_term));
    }

    /// \brief Constructor.
    abstraction_base(const binder_type& binding_operator, const variable_list& variables, const data_expression& body)
      : data_expression(core::detail::gsMakeBinder(binding_operator, variables, body))
    {}

    binder_type binding_operator() const
    {
      return atermpp::arg1(*this);
    }

    variable_list variables() const
    {
      return atermpp::list_arg2(*this);
    }

    data_expression body() const
    {
      return atermpp::arg3(*this);
    }
};
//--- end generated class ---//

    } // namespace detail

    /// \brief Expression for abstraction, i.e. an expression binding a
    ///        number of variables.
    ///
    /// An example of an abstraction is lambda x,y:Pos . f(x,y),
    /// where lambda is the binding operator, x,y are the variables,
    /// and f(x,y) is the body of the abstraction.
    ///
    class abstraction: public detail::abstraction_base
    {
      public:

        /// \brief Iterator range over bound variables
        typedef atermpp::term_list< variable > variables_const_range;

        /// Default constructor for abstraction (does not entail a
        /// valid data expression.
        ///
        abstraction()
          : detail::abstraction_base(core::detail::constructBinder())
        {}

        /// \overload
        abstraction(atermpp::aterm_appl term)
          : abstraction_base(term)
        {}

        /// Construct abstraction from a data expression.
        /// \param[in] d a data expression
        /// \pre d.is_abstraction()
        abstraction(const data_expression& d)
          : detail::abstraction_base(d)
        {
          assert(d.is_abstraction());
        }

        /// \overload
        abstraction(const binder_type& binding_operator, const variable_list& variables, const data_expression& body)
          : abstraction_base(binding_operator, variables, body)
        {}

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
        abstraction(const binder_type& binding_operator,
                    const Container& variables,
                    const data_expression& body,
                    typename detail::enable_if_container< Container, variable >::type* = 0)
          : detail::abstraction_base(binding_operator, convert< variable_list >(variables), body)
        {
          assert(!variables.empty());
        }

        /// \brief Returns the binding operator of the abstraction
        inline
        binder_type binding_operator() const
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
          return binding_operator() == lambda_binder();
        }

        /// \brief Returns true iff the binding operator is "forall"
        inline
        bool is_forall() const
        {
          return binding_operator() == forall_binder();
        }

        /// \brief Returns true iff the binding operator is "exists"
        inline
        bool is_exists() const
        {
          return binding_operator() == exists_binder();
        }

    }; // class abstraction

  } // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_ABSTRACTION_H

