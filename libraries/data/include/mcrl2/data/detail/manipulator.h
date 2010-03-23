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

#ifndef MCRL2_DATA_DETAIL_EXPRESSION_MANIPULATOR_H
#define MCRL2_DATA_DETAIL_EXPRESSION_MANIPULATOR_H

#include "boost/type_traits/is_convertible.hpp"
#include "boost/range/iterator_range.hpp"

#include "mcrl2/data/assignment.h"
#include "mcrl2/data/lambda.h"
#include "mcrl2/data/forall.h"
#include "mcrl2/data/exists.h"
#include "mcrl2/data/container_sort.h"
#include "mcrl2/data/where_clause.h"
#include "mcrl2/data/data_equation.h"
#include "mcrl2/data/assignment.h"
#include "mcrl2/data/detail/data_expression_with_variables.h"
#include "mcrl2/data/detail/container_utility.h"

namespace mcrl2 {

  namespace data {

    /// \cond INTERNAL_DOCS
    namespace detail {

      template < typename Derived >
      class expression_manipulator
      {
          template < typename Abstraction >
          data_expression visit(Abstraction const& a)
          {
            return abstraction(a.binding_operator(),
              static_cast< Derived& >(*this)(a.variables()),
              static_cast< Derived& >(*this)(a.body()));
          }

        public:

          data_expression const& operator()(function_symbol const& e)
          {
            return e;
          }

          data_expression const& operator()(variable const& e)
          {
            return e;
          }

          data_expression operator()(where_clause const& w)
          {
            return where_clause(static_cast< Derived& >(*this)(w.body()),
                                static_cast< Derived& >(*this)(w.declarations()));
          }

          data_expression operator()(application const& a)
          {
            return application(static_cast< Derived& >(*this)(a.head()),
                               static_cast< Derived& >(*this)(a.arguments()));
          }

          data_expression operator()(lambda const& a)
          {
            return visit(a);
          }

          data_expression operator()(exists const& a)
          {
            return visit(a);
          }

          data_expression operator()(forall const& a)
          {
            return visit(a);
          }

          data_expression operator()(abstraction const& a)
          {
            return visit(a);
          }

          data_expression operator()(data_expression_with_variables const& a)
          {
            return static_cast< Derived& >(*this)(static_cast< data_expression const& >(a));
          }

          data_expression operator()(data_expression const& e)
          {
            if (e.is_application())
            {
              return static_cast< Derived& >(*this)(application(e));
            }
            else if (e.is_variable())
            {
              return static_cast< Derived& >(*this)(variable(e));
            }
            else if (e.is_function_symbol())
            {
              return static_cast< Derived& >(*this)(function_symbol(e));
            }
            else if (e.is_abstraction())
            {
              return static_cast< Derived& >(*this)(abstraction(e));
            }
            else if (e.is_where_clause())
            {
              return static_cast< Derived& >(*this)(where_clause(e));
            }

            return e;
          }

		  assignment_expression operator()(assignment_expression const& a)
		  {
			if (is_assignment(a))
			{
			  return static_cast< Derived& >(*this)(assignment(a));
			}
			else if (is_identifier_assignment(a))
			{
			  return static_cast< Derived& >(*this)(identifier_assignment(a));
			}

			return a;
		  }

          assignment operator()(assignment const& a)
          {
            return assignment(static_cast< Derived& >(*this)(a.lhs()),
                              static_cast< Derived& >(*this)(a.rhs()));
          }

		  identifier_assignment operator()(identifier_assignment const& a)
          {
            return identifier_assignment(static_cast< Derived& >(*this)(a.lhs()),
                              			 static_cast< Derived& >(*this)(a.rhs()));
          }

          data_equation operator()(data_equation const& a)
          {
            return data_equation(
               static_cast< Derived& >(*this)(a.variables()),
               static_cast< Derived& >(*this)(a.condition()),
               static_cast< Derived& >(*this)(a.lhs()),
               static_cast< Derived& >(*this)(a.rhs()));
          }

#ifndef NO_TERM_TRAVERSAL
          // \deprecated exists only for backwards compatibility
          template < typename Expression >
          Expression operator()(Expression const& e, typename detail::disable_if_container< Expression >::type* = 0)
          {
            if (e.type() == AT_APPL)
            {
              if (is_data_expression(e)) {
                return static_cast< Derived& >(*this)(data_expression(e));
              }
              else {
                return static_cast< Derived& >(*this)(atermpp::aterm_appl(e));
              }
            }
            else if (e.type() == AT_LIST)
            {
              return atermpp::aterm_appl(reinterpret_cast< ATermAppl >(static_cast< ATerm >(apply(atermpp::aterm_list(e)))));
            }

            return e;
          }

          // \deprecated exists only for backwards compatibility
          atermpp::aterm_appl operator()(atermpp::aterm_appl const& e)
          {
            if (!e.empty())
            {
              atermpp::vector< atermpp::aterm_appl > new_arguments;

              for (atermpp::aterm_appl::const_iterator i = atermpp::aterm_appl(e).begin(); i != atermpp::aterm_appl(e).end(); ++i)
              {
                new_arguments.push_back((*this)(*i));
              }

              return atermpp::aterm_appl(e.function(), new_arguments.begin(), new_arguments.end());
            }

            return e;
          }

          // \deprecated exists only for backwards compatibility
          atermpp::aterm apply(atermpp::aterm_list const& e)
          {
            atermpp::vector< atermpp::aterm > result;

            for (atermpp::aterm_list::const_iterator i= e.begin(); i != e.end(); ++i)
            {
              result.push_back((*this)(*i));
            }

            return convert< atermpp::aterm_list >(result);
          }

          template < typename Expression, bool = boost::is_convertible< Expression, data_expression >::value >
          struct manipulation_result_type {
            typedef Expression type;
          };

          template < typename Expression >
          struct manipulation_result_type< Expression, true > {
            typedef data_expression type;
          };

          template < typename Expression >
          atermpp::term_list< typename manipulation_result_type< Expression >::type > operator()(atermpp::term_list< Expression > const& container)
          {
            typedef typename manipulation_result_type< Expression >::type result_type;

            atermpp::vector< atermpp::aterm_appl > result;

            for (typename atermpp::term_list< Expression >::const_iterator i = container.begin(); i != container.end(); ++i)
            {
              result.push_back(static_cast< Derived& >(*this)(*i));
            }

            return convert< atermpp::term_list< result_type > >(result);
          }
#endif // NO_TERM_TRAVERSAL

	  // This implementation suffers from the problem that a transformation
	  // may replace expressions by more general expression types. It is
	  // not known a-priory what the resulting expression type should be.
	  // Using type-erasure, the result type can be eliminated from the
	  // return type, but since the current use of this component is not
	  // that advanced it was chosen not to enhance the implement.
          template < typename Container >
          boost::iterator_range< transform_iterator< Derived&, typename Container::const_iterator, typename Container::value_type > >
          operator()(Container const& container, typename detail::enable_if_container< Container >::type* = 0)
          {
            return boost::make_iterator_range(
              transform_iterator< Derived&, typename Container::const_iterator, typename Container::value_type >(container.begin(), static_cast< Derived& >(*this)),
              transform_iterator< Derived&, typename Container::const_iterator, typename Container::value_type >(container.end(), static_cast< Derived& >(*this)));
          }
      };

      template < typename Derived >
      class binding_aware_expression_manipulator : public expression_manipulator< Derived > {

          typedef expression_manipulator< Derived > super;

        protected:

          std::multiset< variable > m_bound;

          template < typename Container >
          void increase_bind_count(const Container& variables, typename detail::enable_if_container< Container, variable >::type* = 0)
          {
            m_bound.insert(variables.begin(), variables.end());
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
          data_expression visit(Abstraction const& a)
          {
            increase_bind_count(a.variables());

            abstraction result(a.binding_operator(), a.variables(), static_cast< Derived& >(*this)(a.body()));

            decrease_bind_count(a.variables());

            return result;
          }

        public:

          using super::operator();

          data_expression operator()(lambda const& a)
          {
            return visit(a);
          }

          data_expression operator()(exists const& a)
          {
            return visit(a);
          }

          data_expression operator()(forall const& a)
          {
            return visit(a);
          }

          data_expression operator()(abstraction const& a)
          {
            return visit(a);
          }

          data_expression operator()(where_clause const& w)
          {
            increase_bind_count(make_assignment_left_hand_side_range(w.declarations()));

            where_clause result(static_cast< Derived& >(*this)(w.body()), make_assignment_range(
                                 make_assignment_left_hand_side_range(w.declarations()),
                                 (*this)(make_assignment_right_hand_side_range(w.declarations()))));

            decrease_bind_count(make_assignment_left_hand_side_range(w.declarations()));

            return result;
          }

          bool is_bound(variable const& v) const
          {
            return m_bound.find(v) != m_bound.end();
          }

          binding_aware_expression_manipulator()
          { }

          template < typename Container >
          binding_aware_expression_manipulator(Container const& bound_by_context,
                                    typename detail::enable_if_container< Container, variable >::type* = 0) :
                              m_bound(bound_by_context.begin(), bound_by_context.end())
          { }

          virtual ~binding_aware_expression_manipulator()
          { }
      };
    } // namespace detail
    /// \endcond
} // namespace data

} // namespace mcrl2

#endif

