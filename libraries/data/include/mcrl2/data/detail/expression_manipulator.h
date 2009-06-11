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

#include "mcrl2/data/detail/data_expression_with_variables.h"
#include "mcrl2/data/detail/container_utility.h"

namespace mcrl2 {

  namespace data {

    /// \cond INTERNAL_DOCS
    namespace detail {

      template < typename Derived >
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

        data_expression operator()(data_expression_with_variables const& e)
        {
          return (*this)(static_cast< data_expression const& >(e));
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
          return assignment(a.lhs(), (*this)(a.rhs()));
        }

        // \deprecated exists only for backwards compatibility
        template < typename Expression >
        Expression operator()(Expression const& e)
        {
          if (is_data_expression(e)) {
            return (*this)(data_expression(e));
          }
          else if (e.type() == AT_APPL)
          {
            return apply(atermpp::aterm_appl(e));
          }
          else if (e.type() == AT_LIST)
          {
            return atermpp::aterm_appl(reinterpret_cast< ATermAppl >(static_cast< ATerm >(apply_list(e))));
          }

          return e;
        }

        // \deprecated exists only for backwards compatibility
        atermpp::aterm_appl apply(atermpp::aterm_appl e)
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
        atermpp::aterm apply_list(atermpp::aterm e)
        {
          return reinterpret_cast< ATerm >(static_cast< ATermList >((*this)(atermpp::aterm_list(e))));
        }

        template < typename Expression >
        atermpp::term_list< Expression > operator()(atermpp::term_list< Expression > const& container)
        {
          atermpp::vector< Expression > result;

          for (typename atermpp::term_list< Expression >::const_iterator i = container.begin(); i != container.end(); ++i)
          {
            result.push_back((*this)(*i));
          }

          return convert< atermpp::term_list< Expression > >(result);
        }

        template < typename Container >
        Container operator()(Container const& container, typename detail::enable_if_container< Container >::type* = 0)
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

