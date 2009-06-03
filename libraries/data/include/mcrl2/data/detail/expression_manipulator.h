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

#include "mcrl2/data/detail/container_utility.h"

namespace mcrl2 {

  namespace data {

    /// \cond INTERNAL_DOCS
    namespace detail {

      template < typename Derived, typename Result >
      struct expression_manipulator
      {
        data_expression const& operator()(function_symbol const& e)
        {
          return e;
        }

        data_expression const& operator()(variable const& e)
        {
          return e;
        }

        data_expression const& operator()(abstraction const& e)
        {
          return e;
        }

        data_expression const& operator()(application const& e)
        {
          return e;
        }

        data_expression const& operator()(where_clause const& e)
        {
          return e;
        }

        data_expression operator()(data_expression const& e)
        {
          if (e.is_application())
          {
            return static_cast< Derived& >(*this)(application(e));
          }
          else if (e.is_where_clause())
          {
            return static_cast< Derived& >(*this)(where_clause(e));
          }
          else if (e.is_abstraction())
          {
            return static_cast< Derived& >(*this)(abstraction(e));
          }
          else if (e.is_variable())
          {
            return static_cast< Derived& >(*this)(variable(e));
          }
          else if (e.is_function_symbol())
          {
            return static_cast< Derived& >(*this)(function_symbol(e));
          }

          return e;
        }

        assignment operator()(assignment const& a)
        {
          return assignment((*this)(a.lhs()), (*this)(a.rhs()));
        }

        // \deprecated exists only for backwards compatibility
        template < typename Expression >
        Expression operator()(Expression const& e)
        {
          if (is_data_expression(e)) {
            return (*this)(data_expression(e));
          }

          atermpp::vector< atermpp::aterm_appl > new_arguments;

          for (atermpp::aterm_appl::const_iterator i = e.begin(); i != e.end(); ++i)
          {
            new_arguments.push_back((*this)(e));
          }

          return atermpp::aterm_appl(e.function(), new_arguments.begin(), new_arguments.end());
        }

        template < typename Expression >
        atermpp::term_list< Result > operator()(atermpp::term_list< Expression > const& container)
        {
          atermpp::term_list< Result > result;

          for (typename atermpp::term_list< Expression >::const_iterator i = container.begin(); i != container.end(); ++i)
          {
            atermpp::push_front(result, (*this)(*i));
          }

          return atermpp::reverse(result);
        }

        template < typename Container >
        result_container< Container, Result > operator()(Container const& container, typename detail::enable_if_container< Container >::type* = 0)
        {
          Container result;

          for (typename Container::const_iterator i = container.begin(); i != container.end(); ++i)
          {
            result.insert(result.end(), (*this)(*i));
          }

          return result;
        }
      };
    } // namespace detail
    /// \endcond
} // namespace data

} // namespace mcrl2

#endif

