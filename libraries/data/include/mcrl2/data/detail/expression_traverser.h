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
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/detail/container_utility.h"
#include "mcrl2/data/lambda.h"
#include "mcrl2/data/exists.h"
#include "mcrl2/data/forall.h"

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
      class expression_traverser
      {
        protected:

          template < typename Abstraction >
          void visit(Abstraction const& a)
          {
            static_cast< Derived& >(*this).enter(static_cast< data_expression const& >(a));
            static_cast< Derived& >(*this).enter(static_cast< abstraction const& >(a));
            static_cast< Derived& >(*this).enter(a);

            static_cast< Derived& >(*this)(a.variables());
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

          void operator()(function_symbol const& e)
          {
            static_cast< Derived& >(*this).enter(static_cast< data_expression const& >(e));
            static_cast< Derived& >(*this).enter(e);
            static_cast< Derived& >(*this).leave(e);
            static_cast< Derived& >(*this).leave(static_cast< data_expression const& >(e));
          }

          void operator()(variable const& e)
          {
            static_cast< Derived& >(*this).enter(static_cast< data_expression const& >(e));
            static_cast< Derived& >(*this).enter(e);
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
            static_cast< Derived& >(*this)(e.arguments());

            static_cast< Derived& >(*this).leave(e);
            static_cast< Derived& >(*this).leave(static_cast< data_expression const& >(e));
          }

          void operator()(where_clause const& e)
          {
            static_cast< Derived& >(*this).enter(static_cast< data_expression const& >(e));
            static_cast< Derived& >(*this).enter(e);

            static_cast< Derived& >(*this)(e.declarations());
            static_cast< Derived& >(*this)(e.body());

            static_cast< Derived& >(*this).leave(e);
            static_cast< Derived& >(*this).leave(static_cast< data_expression const& >(e));
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

          void operator()(data_specification const& e)
          {
            static_cast< Derived& >(*this)(e.sorts());
            static_cast< Derived& >(*this)(e.constructors());
            static_cast< Derived& >(*this)(e.mappings());
            static_cast< Derived& >(*this)(e.aliases());
            static_cast< Derived& >(*this)(e.equations());
          }

          // \deprecated exists only for backwards compatibility
          template < typename Expression >
          void operator()(Expression const& e, typename detail::disable_if_container< Expression >::type* = 0)
          {
            if (is_data_expression(e))
            {
              static_cast< Derived& >(*this)(data_expression(e));
            }
            else {
              (*this)(static_cast< atermpp::aterm const& >(e));
            }
          }

          // \deprecated exists only for backwards compatibility
          void operator()(atermpp::aterm const& e)
          {
            if (e.type() == AT_APPL)
            {
              for (atermpp::aterm_appl::const_iterator i = atermpp::aterm_appl(e).begin(); i != atermpp::aterm_appl(e).end(); ++i)
              {
                static_cast< Derived& >(*this)(*i);
              }
            }
            else if (e.type() == AT_LIST)
            {
              static_cast< Derived& >(*this)(atermpp::aterm_list(e));
            }
          }

          template < typename Container >
          void operator()(Container const& container, typename detail::enable_if_container< Container >::type* = 0)
          {
            std::for_each(container.begin(), container.end(), static_cast< Derived& >(*this));
          }
      };

      template < typename Derived >
      class sort_expression_traverser : public expression_traverser< Derived >
      {
        public:
          typedef expression_traverser< Derived > super;

          template < typename Expression >
          void enter(Expression const&)
          {}
          template < typename Expression >
          void leave(Expression const&)
          {}

          void operator()(basic_sort const& e)
          {
            static_cast< Derived& >(*this).enter(static_cast< sort_expression const& >(e));
            static_cast< Derived& >(*this).enter(e);
            static_cast< Derived& >(*this).leave(e);
            static_cast< Derived& >(*this).leave(static_cast< sort_expression const& >(e));
          }

          void operator()(function_sort const& e)
          { 
            static_cast< Derived& >(*this).enter(static_cast< sort_expression const& >(e));
            static_cast< Derived& >(*this).enter(e);

            static_cast< Derived& >(*this)(e.domain());
            static_cast< Derived& >(*this)(e.codomain());

            static_cast< Derived& >(*this).leave(e);
            static_cast< Derived& >(*this).leave(static_cast< sort_expression const& >(e));
          }

          void operator()(container_sort const& e)
          { 
            static_cast< Derived& >(*this).enter(static_cast< sort_expression const& >(e));
            static_cast< Derived& >(*this).enter(e);

            static_cast< Derived& >(*this)(e.element_sort());

            static_cast< Derived& >(*this).leave(static_cast< sort_expression const& >(e));
            static_cast< Derived& >(*this).leave(e);
          }

          void operator()(structured_sort const& e)
          {
            static_cast< Derived& >(*this).enter(static_cast< sort_expression const& >(e));
            static_cast< Derived& >(*this).enter(e);

            for (structured_sort::constructors_const_range r(e.struct_constructors()); !r.empty(); r.advance_begin(1))
            {
              for (structured_sort_constructor::arguments_const_range j(r.front().arguments()); !j.empty(); j.advance_begin(1))
              {
                static_cast< Derived& >(*this)(j.front().sort());
              }
            }

            static_cast< Derived& >(*this).leave(e);
            static_cast< Derived& >(*this).leave(static_cast< sort_expression const& >(e));
          }

          void operator()(sort_expression const& e)
          {
            if (e.is_basic_sort())
            {
              static_cast< Derived& >(*this)(basic_sort(e));
            }
            else if (e.is_container_sort())
            {
              static_cast< Derived& >(*this)(container_sort(e));
            }
            else if (e.is_structured_sort())
            {
              static_cast< Derived& >(*this)(structured_sort(e));
            }
            else if (e.is_function_sort())
            {
              static_cast< Derived& >(*this)(function_sort(e));
            }
            else if (e.is_alias())
            {
              static_cast< Derived& >(*this)(alias(e));
            }
          }

          void operator()(alias const& e)
          {
            static_cast< Derived& >(*this).enter(static_cast< sort_expression const& >(e));
            static_cast< Derived& >(*this).enter(e);

            static_cast< Derived& >(*this)(e.name());
            static_cast< Derived& >(*this)(e.reference());

            static_cast< Derived& >(*this).leave(e);
            static_cast< Derived& >(*this).leave(static_cast< sort_expression const& >(e));
          }

          void operator()(assignment const& a)
          {
            static_cast< super& >(*this)(a);
          }

          void operator()(data_equation const& e)
          {
            static_cast< super& >(*this)(e);
          }

          void operator()(data_specification const& e)
          {
            static_cast< Derived& >(*this)(e.sorts());
            static_cast< Derived& >(*this)(e.aliases());
            static_cast< super& >(*this)(e);
          }

          // \deprecated
          void operator()(atermpp::aterm_appl const& e)
          {
            static_cast< super& >(*this)(e);
          }

          // \deprecated
          void operator()(atermpp::aterm const& e)
          {
            static_cast< super& >(*this)(e);
          }

          template < typename Expression >
          void operator()(Expression const& e, typename detail::disable_if_container< Expression >::type* = 0)
          {
            if (!is_nil(e)) { // REMOVE ME (condition)
              static_cast< Derived& >(*this)(e.sort());

              static_cast< super& >(*this)(e);
            }
          }

          template < typename Container >
          void operator()(Container const& container, typename detail::enable_if_container< Container >::type* = 0)
          {
            std::for_each(container.begin(), container.end(), static_cast< Derived& >(*this));
          }
      };

      template < typename Derived >
      class binding_aware_expression_traverser : public expression_traverser< Derived > {

        typedef expression_traverser< Derived > super;

        protected:

          std::multiset< variable > m_bound;

          template < typename Container >
          void increase_bind_count(const Container& variables, typename detail::enable_if_container< Container, variable >::type* = 0)
          {
            for (typename Container::const_iterator i = variables.begin(); i != variables.end(); ++i)
            {
              m_bound.insert(*i);
            }
          }

          template < typename Container >
          void decrease_bind_count(const Container& variables, typename detail::enable_if_container< Container, variable >::type* = 0)
          {
            for (typename Container::const_iterator i = variables.begin(); i != variables.end(); ++i)
            {
              m_bound.erase(m_bound.find(*i));
            }
          }

          template < typename Abstraction >
          void visit(Abstraction const& a) {
            increase_bind_count(a.variables());

            super::visit(a);

            decrease_bind_count(a.variables());
          }

        public:

          using super::operator();

          void operator()(where_clause const& w)
          {
            increase_bind_count(make_assignment_left_hand_side_range(w.declarations()));

            static_cast< Derived& >(*this).enter(static_cast< data_expression const& >(w));
            static_cast< Derived& >(*this).enter(w);
            static_cast< Derived& >(*this)(make_assignment_left_hand_side_range(w.declarations()));
            static_cast< Derived& >(*this)(w.body());
            static_cast< Derived& >(*this).leave(static_cast< data_expression const& >(w));
            static_cast< Derived& >(*this).leave(w);

            decrease_bind_count(make_assignment_left_hand_side_range(w.declarations()));
          }

          void operator()(lambda const& a)
          {
            visit(a);
          }

          void operator()(exists const& a)
          {
            visit(a);
          }

          void operator()(forall const& a)
          {
            visit(a);
          }

          bool is_bound(variable const& v) const
          {
            return m_bound.find(v) != m_bound.end();
          }

          binding_aware_expression_traverser()
          { }

          template < typename Container >
          binding_aware_expression_traverser(Container const& bound_by_context,
                                    typename detail::enable_if_container< Container, variable >::type* = 0) :
                              m_bound(bound_by_context.begin(), bound_by_context.end())
          { }

          virtual ~binding_aware_expression_traverser()
          { }
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
       * \see expression_traverser
       **/
      template < typename Derived, typename AdaptablePredicate, template < class > class Traverser = detail::expression_traverser >
      class selective_expression_traverser : public Traverser< Derived >
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
          selective_expression_traverser()
          { }

          selective_expression_traverser(AdaptablePredicate predicate) : m_traverse_condition(predicate)
          { }

          virtual ~selective_expression_traverser()
          { }
      };

      template < typename Derived, typename AdaptablePredicate >
      class selective_data_expression_traverser : public selective_expression_traverser< Derived, AdaptablePredicate >
      {
        typedef selective_expression_traverser< Derived, AdaptablePredicate > super;

        public:

          using super::operator();
          using super::enter;
          using super::leave;

          selective_data_expression_traverser()
          { }

          selective_data_expression_traverser(AdaptablePredicate predicate) : super(predicate)
          { }
      };

      template < typename Derived, typename AdaptablePredicate >
      class selective_sort_expression_traverser : public selective_expression_traverser< Derived, AdaptablePredicate, detail::sort_expression_traverser >
      {
        typedef selective_expression_traverser< Derived, AdaptablePredicate, detail::sort_expression_traverser > super;

        public:

          using super::operator();
          using super::enter;
          using super::leave;

          selective_sort_expression_traverser()
          { }

          selective_sort_expression_traverser(AdaptablePredicate predicate) : super(predicate)
          { }
      };

      template < typename Derived, typename AdaptablePredicate >
      class selective_binding_aware_expression_traverser : public selective_expression_traverser< Derived, AdaptablePredicate, detail::binding_aware_expression_traverser >
      {
        typedef selective_expression_traverser< Derived, AdaptablePredicate, detail::binding_aware_expression_traverser > super;

        public:

          using super::operator();
          using super::enter;
          using super::leave;

          selective_binding_aware_expression_traverser()
          { }

          selective_binding_aware_expression_traverser(AdaptablePredicate predicate) : super(predicate)
          { }
      };

    } // namespace detail
    /// \endcond
} // namespace data

} // namespace mcrl2

#endif

