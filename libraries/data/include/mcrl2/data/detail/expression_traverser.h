// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/replace.h
/// \brief Contains a function for replacing data variables in a term.

#ifndef MCRL2_DATA_DETAIL_EXPRESSION_TRAVERSER_H
#define MCRL2_DATA_DETAIL_EXPRESSION_TRAVERSER_H

#include <algorithm>

#include "boost/ref.hpp"
#include "mcrl2/data/assignment.h"
#include "mcrl2/data/abstraction.h"
#include "mcrl2/data/application.h"
#include "mcrl2/data/where_clause.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/data_equation.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/data/detail/container_utility.h"

namespace mcrl2 {

  namespace data {

    /// \cond INTERNAL_DOCS
    namespace detail {

      /**
       * \brief expression traverser that visits all sub expressions
       *
       * Types:
       *  \arg Derived the type of a derived class, as per CRTP
       *
       * Note that the type of expressions is *not* necessarily limited to data
       * expression. The point is that expression types that extend data
       * expressions can also be used transparently.
       **/
      template < typename Derived >
      struct expression_traverser
      {
        void operator()(function_symbol const& e)
        { }

        void operator()(variable const& e)
        { }

        void operator()(abstraction const& e)
        {
          (*this)(e.variables());
          (*this)(e.body());
        }

        void operator()(application const& e)
        {
          (*this)(e.head());
          (*this)(e.arguments());
        }

        void operator()(where_clause const& e)
        {
          (*this)(e.declarations());
          (*this)(e.body());
        }

        void operator()(data_expression const& e)
        {
          if (e.is_application())
          {
            static_cast< Derived& >(*this)(application(e));
          }
          else if (e.is_where_clause())
          {
            static_cast< Derived& >(*this)(where_clause(e));
          }
          else if (e.is_abstraction())
          {
            static_cast< Derived& >(*this)(abstraction(e));
          }
          else if (e.is_variable())
          {
            static_cast< Derived& >(*this)(variable(e));
          }
          else if (e.is_function_symbol())
          {
            static_cast< Derived& >(*this)(function_symbol(e));
          }
        }

        void operator()(assignment const& a)
        {
          (*this)(a.lhs());
          (*this)(a.rhs());
        }

        // \deprecated exists only for backwards compatibility
        template < typename Expression >
        void operator()(Expression const& e)
        {
          if (is_data_expression(e))
          {
            (*this)(data_expression(e));
          }
          else if (e.type() == AT_APPL)
          {
            for (atermpp::aterm_appl::const_iterator i = atermpp::aterm_appl(e).begin(); i != atermpp::aterm_appl(e).end(); ++i)
            {
              (*this)(*i);
            }
          }
          else if (e.type() == AT_LIST)
          {
            apply_list(e);
          }
        }

        // \deprecated exists only for backwards compatibility
        void apply_list(atermpp::aterm e)
        {
          (*this)(atermpp::aterm_list(e));
        }

        template < typename Container >
        void operator()(Container const& container, typename detail::enable_if_container< Container >::type* = 0)
        {
          std::for_each(container.begin(), container.end(), boost::ref(*this));
        }
      };

      /**
       * \brief expression traverser that can be used for cases in which parts
       * of the expression should not be traversed
       *
       * Types:
       *  \arg Derived the type of a derived class, as per CRTP
       *  \arg SelectionPredicate unary predicate that takes an expression
       *
       * \see expression_traverser
       **/
      template < typename Derived, typename SelectionPredicate >
      class selective_expression_traverser : protected expression_traverser< selective_expression_traverser< Derived, SelectionPredicate > >
      {
        typedef expression_traverser< selective_expression_traverser< Derived, SelectionPredicate > > super;

        protected:

          SelectionPredicate m_predicate;

        public:

          template < typename Expression >
          void operator()(Expression const& e)
          {
            if (m_predicate(e))
            {
              super::operator()(e);
            }
          }

          template < typename Container >
          void operator()(Container const& container, typename detail::enable_if_container< Container >::type* = 0)
          {
            for (typename Container::const_iterator i = container.begin(); i != container.end(); ++i)
            {
              if (m_predicate(*i))
              {
                (*this)(*i);
              }
            }
          }

          // Default constructor (only works if SelectionPredicate is Default Constructible)
          selective_expression_traverser()
          { }

          selective_expression_traverser(SelectionPredicate predicate) : m_predicate(predicate)
          { }

          virtual ~selective_expression_traverser()
          { }
      };
    } // namespace detail
    /// \endcond
} // namespace data

} // namespace mcrl2

#endif

