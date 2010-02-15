// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/traverser.h
/// \brief Contains a function for replacing data variables in a term.

#ifndef MCRL2_DATA_DETAIL_TRAVERSER_H
#define MCRL2_DATA_DETAIL_TRAVERSER_H

#include <algorithm>

#include "boost/utility/enable_if.hpp"
#include "boost/type_traits/is_base_of.hpp"
#include "mcrl2/data/assignment.h"
#include "mcrl2/data/abstraction.h"
#include "mcrl2/data/application.h"
#include "mcrl2/data/where_clause.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/data_equation.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/data/container_sort.h"
#include "mcrl2/data/structured_sort.h"
#include "mcrl2/data/detail/container_utility.h"
#include "mcrl2/data/lambda.h"
#include "mcrl2/data/exists.h"
#include "mcrl2/data/forall.h"

namespace mcrl2 {

  namespace data {

    class data_expression_with_variables;

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
      class traverser
      {
        protected:

          template < typename Abstraction >
          void visit(Abstraction const& a)
          {
            static_cast< Derived& >(*this).enter(static_cast< data_expression const& >(a));
            static_cast< Derived& >(*this).enter(static_cast< abstraction const& >(a));
            static_cast< Derived& >(*this).enter(a);

            (*this)(a.variables());
            static_cast< Derived& >(*this)(a.body());

            static_cast< Derived& >(*this).leave(a);
            static_cast< Derived& >(*this).leave(static_cast< abstraction const& >(a));
            static_cast< Derived& >(*this).leave(static_cast< data_expression const& >(a));
          }

        public:

          template < typename DataExpression >
          void enter(DataExpression const&)
          {}
          template < typename DataExpression >
          void leave(DataExpression const&)
          {}

          void operator()(core::identifier_string const& e)
          {
            static_cast< Derived& >(*this).enter(static_cast< core::identifier_string const& >(e));
            static_cast< Derived& >(*this).leave(static_cast< core::identifier_string const& >(e));
          }

          void operator()(function_symbol const& e)
          {
            static_cast< Derived& >(*this).enter(static_cast< data_expression const& >(e));
            static_cast< Derived& >(*this).enter(e);
            static_cast< Derived& >(*this)(e.name());
            static_cast< Derived& >(*this).leave(e);
            static_cast< Derived& >(*this).leave(static_cast< data_expression const& >(e));
          }

          void operator()(variable const& e)
          {
            static_cast< Derived& >(*this).enter(static_cast< data_expression const& >(e));
            static_cast< Derived& >(*this).enter(e);
            static_cast< Derived& >(*this)(e.name());
            static_cast< Derived& >(*this).leave(e);
            static_cast< Derived& >(*this).leave(static_cast< data_expression const& >(e));
          }

          void operator()(lambda const& e)
          {
            visit(e);
          }

          void operator()(forall const& e)
          {
            visit(e);
          }

          void operator()(exists const& e)
          {
            visit(e);
          }

          void operator()(abstraction const& e)
          {
            if (e.is_lambda())
            {
              static_cast< Derived& >(*this)(lambda(e));
            }
            else if (e.is_exists())
            {
              static_cast< Derived& >(*this)(exists(e));
            }
            else if (e.is_forall())
            {
              static_cast< Derived& >(*this)(forall(e));
            }
          }

          void operator()(application const& e)
          {
            static_cast< Derived& >(*this).enter(static_cast< data_expression const& >(e));
            static_cast< Derived& >(*this).enter(e);

            static_cast< Derived& >(*this)(e.head());
            (*this)(e.arguments());

            static_cast< Derived& >(*this).leave(e);
            static_cast< Derived& >(*this).leave(static_cast< data_expression const& >(e));
          }

          void operator()(where_clause const& e)
          {
            static_cast< Derived& >(*this).enter(static_cast< data_expression const& >(e));
            static_cast< Derived& >(*this).enter(e);

            (*this)(e.declarations());
            static_cast< Derived& >(*this)(e.body());

            static_cast< Derived& >(*this).leave(e);
            static_cast< Derived& >(*this).leave(static_cast< data_expression const& >(e));
          }

          // Default, no traversal of sort expressions
          void operator()(data_expression_with_variables const& e)
          {
            static_cast< Derived& >(*this)(static_cast< data_expression const& >(e));
          }

          // Default, no traversal of sort expressions
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
            static_cast< Derived& >(*this).enter(a);

            static_cast< Derived& >(*this)(a.lhs());
            static_cast< Derived& >(*this)(a.rhs());

            static_cast< Derived& >(*this).leave(a);
          }

          void operator()(data_equation const& e)
          {
            static_cast< Derived& >(*this).enter(e);

            static_cast< Derived& >(*this)(e.variables());
            static_cast< Derived& >(*this)(e.condition());
            static_cast< Derived& >(*this)(e.lhs());
            static_cast< Derived& >(*this)(e.rhs());

            static_cast< Derived& >(*this).leave(e);
          }

          /* ENG. Substitutie moet plaatsvinden op interne data structuur. TODO.
          void operator()(data_specification const& e)
          {
            static_cast< Derived& >(*this)(e.sorts());
            static_cast< Derived& >(*this)(e.constructors());
            static_cast< Derived& >(*this)(e.mappings());
            // static_cast< Derived& >(*this)(e.aliases());
            static_cast< Derived& >(*this)(e.equations());
          } */

#ifndef NO_TERM_TRAVERSAL
          // \deprecated exists only for backwards compatibility
          template < typename Expression >
          void operator()(Expression const& e, typename detail::disable_if_container< Expression >::type* = 0)
          {
            (*this)(static_cast< atermpp::aterm const& >(e));
          }

          // \deprecated exists only for backwards compatibility
          void operator()(atermpp::aterm const& e)
          {
            if (e.type() == AT_APPL)
            {
              static_cast< Derived& >(*this)(atermpp::aterm_appl(e));
            }
            else if (e.type() == AT_LIST)
            {
              (*this)(atermpp::aterm_list(e));
            }
          }

          // \deprecated exists only for backwards compatibility
          void operator()(atermpp::aterm_appl const& e)
          {
            if (is_data_expression(e))
            {
              static_cast< Derived& >(*this)(data_expression(e));
            }
            else {
              for (atermpp::aterm_appl::const_iterator i = e.begin(); i != e.end(); ++i)
              {
                (*this)(*i);
              }
            }
          }

          // \deprecated exists only for backwards compatibility
          void operator()(atermpp::aterm_list const& e)
          {
            for (atermpp::aterm_list::const_iterator i = e.begin(); i != e.end(); ++i)
            {
              (*this)(*i);
            }
          }
#endif // NO_TERM_TRAVERSAL

          template < typename Container >
          void operator()(Container const& container, typename detail::enable_if_container< Container >::type* = 0)
          {
            for (typename Container::const_iterator i = container.begin(); i != container.end(); ++i)
            {
              static_cast< Derived& >(*this)(*i);
            }
          }
      };

      /**
       * \brief expression traverser that can be used for cases in which parts
       * of the expression should not be traversed
       *
       * Types:
       *  \arg Derived the type of a derived class, as per CRTP
       *  \arg AdaptablePredicate is a unary predicate on expressions
       *
       * Before a subterm is explored the predicate is applied to see whether
       * traversal should continue.
       *
       * \see traverser
       **/
      template < typename Derived, typename AdaptablePredicate, template < class > class Traverser = detail::traverser >
      class selective_traverser : public Traverser< Derived >
      {
          typedef Traverser< Derived > super;

        protected:

          AdaptablePredicate m_traverse_condition;

          template < typename Expression >
          void forward_call(Expression const& e, typename boost::disable_if< typename boost::is_base_of< data_expression, Expression >::type >::type* = 0,
                                                 typename boost::disable_if< typename boost::is_base_of< sort_expression, Expression >::type >::type* = 0)
          {
            static_cast< super& >(*this)(e);
          }

          void forward_call(data_expression const& e)
          {
            static_cast< super& >(*this)(e);
          }

          void forward_call(sort_expression const& e)
          {
            static_cast< super& >(*this)(e);
          }

          template < typename Expression >
          void forward_call(Expression const& e, typename boost::enable_if< typename boost::is_base_of< data_expression, Expression >::type >::type* = 0)
          {
            if (m_traverse_condition(e))
            {
              static_cast< super& >(*this)(e);
            }
          }

          template < typename Expression >
          void forward_call(Expression const& e, typename boost::enable_if< typename boost::is_base_of< sort_expression, Expression >::type >::type* = 0) {
            if (m_traverse_condition(e))
            {
              static_cast< super& >(*this)(e);
            }
          }

          AdaptablePredicate& traverse_condition()
          {
            return m_traverse_condition;
          }

        public:

          template < typename Expression >
          void operator()(Expression const& e, typename detail::disable_if_container< Expression >::type* = 0)
          {
            forward_call(e);
          }

          template < typename Container >
          void operator()(Container const& container, typename detail::enable_if_container< Container >::type* = 0)
          {
            for (typename Container::const_iterator i = container.begin(); i != container.end(); ++i)
            {
              forward_call(*i);
            }
          }

          // Default constructor (only works if SelectionPredicate is Default Constructible)
          selective_traverser()
          { }

          selective_traverser(AdaptablePredicate predicate) : m_traverse_condition(predicate)
          { }

          virtual ~selective_traverser()
          { }
      };

      template < typename Derived, typename AdaptablePredicate >
      class selective_data_traverser : public selective_traverser< Derived, AdaptablePredicate >
      {
        typedef selective_traverser< Derived, AdaptablePredicate > super;

        public:

          selective_data_traverser()
          { }

          selective_data_traverser(AdaptablePredicate predicate) : super(predicate)
          { }
      };

    } // namespace detail
    /// \endcond
} // namespace data

} // namespace mcrl2

#endif

