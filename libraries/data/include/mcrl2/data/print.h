// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/utility.h
/// \brief Provides utilities for working with lists.

#ifndef MCRL2_DATA_PRINT_H
#define MCRL2_DATA_PRINT_H

#include <iostream>
#include <sstream>
#include <list>
#include <iterator>

#include "boost/utility/enable_if.hpp"

#include "mcrl2/core/print.h"
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/list.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/atermpp/container_utility.h"
#include "mcrl2/data/find.h"

namespace atermpp {
  namespace detail {

    // This is here to make the std::list container work with the pp container ovarload.
    template < typename T >
    struct is_container_impl< std::list< T > > {
      typedef boost::true_type type;
    };

  } // namespace detail
} // namespace atermpp

namespace mcrl2 {

  namespace data {

    /// \brief Pretty prints a data specification
    /// \param[in] specification a data specification
    inline std::string pp(data_specification const& specification)
    {
      return core::pp(detail::data_specification_to_aterm_data_spec(specification));
    }

    /// \brief Pretty prints the contents of a container
    /// \param[in] c a container with data or sort expressions
    template < typename Container >
    inline std::string pp(Container const& c, typename atermpp::detail::enable_if_container< Container >::type* = 0)
    {
      std::string result;

      if (c.begin() != c.end())
      {
        result.append(mcrl2::core::pp(*c.begin()));

        for (typename Container::const_iterator i = ++(c.begin()); i != c.end(); ++i)
        {
          result.append(", ").append(mcrl2::core::pp(*i));
        }
      }

      return result;
    }

    /// \brief Pretty prints a data and sort expressions
    /// \param[in] c A data or sort expression
    inline std::string pp(atermpp::aterm_appl const& c)
    {
      return core::pp(c);
    }

    /// \brief Pretty prints a data and sort expressions
    /// \param[in] c A data or sort expression.
    template < typename Expression >
    inline std::string pp(atermpp::term_list< Expression > const& c)
    {
      return core::pp(c);
    }

    namespace detail {
      // Assumes that the name can be sued to identify a list operation
      // This same assumption was made for the previous print implementation. We
      // cannot do without because we need to recognise function symbols in
      // order to choose how to print them (infix or prefix). The assumption
      // holds only because of another assumption, namely that only identifiers
      // of system-defined sorts may start with an `@' symbol.
      //
      // Additional assumptions:
      //  - system defined types as sort expressions are recognisable outside
      //    specification context; meaning that in a context where A =
      //    List(Bool), then sort A will not be recognised as list sort.

      // Type that is used internally to signal that an expression is an
      // expression built with the constructors/mappings of the list sort
      class list_expression : public data_expression {

        private:

          static atermpp::set< core::identifier_string > initialise_list_symbols()
          {
            atermpp::set< core::identifier_string > result;

            data::find_identifiers(sort_list::list_generate_constructors_code(sort_bool::bool_()), std::inserter(result, result.end()));
            data::find_identifiers(sort_list::list_generate_functions_code(sort_bool::bool_()), std::inserter(result, result.end()));

            return result;
          }

        public:

          static bool is_sort_of_list_operation(container_sort const& expression)
          {
            return expression.container_name() == list_container();
          }

          static bool is_sort_of_list_operation(function_sort const& expression)
          {
            if (!is_sort_of_list_operation(expression.codomain()))
            {
              for (boost::iterator_range< sort_expression_list::iterator > r = expression.domain(); !r.empty(); r.advance_begin(1))
              {
                if (is_sort_of_list_operation(r.front()))
                {
                  return true;
                }
              }

              return false;
            }

            return true;
          }

          static bool is_sort_of_list_operation(sort_expression const& expression)
          {
            if (expression.is_container_sort())
            {
              return is_sort_of_list_operation(container_sort(expression));
            }
            else if (expression.is_function_sort())
            {
              return is_sort_of_list_operation(function_sort(expression));
            }

            return false;
          }

          static bool is_list_expression(function_symbol const& expression)
          {
            static atermpp::set< core::identifier_string > symbols = initialise_list_symbols();

            return symbols.find(expression.name()) != symbols.end() && is_sort_of_list_operation(expression.sort());
          }

          static bool is_list_expression(application const& expression)
          {
            return is_list_expression(expression.head());
          }

          static bool is_list_expression(data_expression const& expression)
          {
            if (expression.is_function_symbol()) {
              return is_list_expression(function_symbol(expression));
            }
            else if (expression.is_application()) {
              return is_list_expression(application(expression));
            }

            return false;
          }

          list_expression(atermpp::aterm_appl const& a) : data_expression(a)
          {
            assert(is_list_expression(data_expression(a)));
          }

          bool is_nil() const
          {
            return sort_list::is_nil_function_symbol(*this);
          }
      };

      template < typename Expression >
      void pretty_print(std::ostream& o, Expression const& expression, typename atermpp::detail::disable_if_container< Expression >::type* = 0)
      {
        if (list_expression::is_list_expression(expression))
        {
           o << list_expression(expression);
        }
        else
        {
           o << pp(expression);
        }
      }
     
      // Temprary measure for testing purposes; print entry point for recursive printing
      template < typename Expression >
      std::string pretty_print(Expression const& expression, typename atermpp::detail::disable_if_container< Expression >::type* = 0)
      {
        std::ostringstream s;

        pretty_print< Expression >(s, expression);

        return s.str();
      }

      template < typename Container >
      void pretty_print(std::ostream& o, Container const& container, typename atermpp::detail::enable_if_container< Container >::type* = 0)
      {
        if (container.begin() != container.end())
        {
          pretty_print(o, *container.begin());

          for (typename Container::const_iterator i = ++container.begin(); i != container.end(); ++ i)
          {
            pretty_print(o << ", ", *i);
          }
        }
      }

      template < typename Container >
      std::string pretty_print(Container const& container, typename atermpp::detail::enable_if_container< Container >::type* = 0)
      {
        std::ostringstream s;

        pretty_print(s, container);

        return s.str();
      }

      inline std::ostream& operator<<(std::ostream& o, list_expression const& expression)
      {
        using namespace sort_list;

        if (expression.is_nil())
        {
          o << "[]";
        }
        else if (is_in_application(expression))
        {
          o << pretty_print(first_argument(expression)) << " in " << list_expression(last_argument(expression));
        }
        else if (is_count_application(expression))
        {
          o << "#" << list_expression(first_argument(expression));
        }
        else if (is_concat_application(expression))
        {
          o << list_expression(first_argument(expression)) << " ++ " << list_expression(last_argument(expression));
        }
        else if (is_element_at_application(expression))
        {
          o << list_expression(last_argument(expression)) << "." << pretty_print(first_argument(expression));
        }
        else if (is_head_application(expression))
        {
          o << "head(" << list_expression(first_argument(expression)) << ")";
        }
        else if (is_tail_application(expression))
        {
          o << "tail(" << list_expression(first_argument(expression)) << ")";
        }
        else if (is_rhead_application(expression))
        {
          o << "rhead(" << list_expression(first_argument(expression)) << ")";
        }
        else if (is_rtail_application(expression))
        {
          o << "rtail(" << list_expression(first_argument(expression)) << ")";
        }
        else if (is_cons_application(expression) || is_snoc_application(expression))
        { // print as list enumeration, or concatenation of list enumerations
          std::list< data_expression > elements;

          std::list< data_expression >::iterator gap = elements.begin();

          data_expression current = expression;

          while (true)
          {
            if (is_nil_function_symbol(current))
            {
              break;
            }
            else if (is_cons_application(current))
            {
              elements.insert(gap, application(current).left());

              current = application(current).right();
            }
            else if (is_snoc_application(current))
            {
              gap = elements.insert(gap, application(current).left());

              current = application(current).right();
            }
            else {
              gap = elements.insert(gap, current);

              break;
            }
          }

          if (gap != elements.end() && gap->sort() == expression.sort()) { // last expression is not an element but a list
            if (elements.begin() != gap) {
              if (std::distance(elements.begin(), gap) == 1)
              {
                o << pretty_print(*elements.begin()) << " |> ";
              }
              else
              {
                o << "[" << pretty_print(boost::make_iterator_range(elements.begin(), gap)) << "] ++ ";
              }
            }

            o << list_expression(*gap++);

            if (gap != elements.end()) {
              if (std::distance(gap, elements.end()) == 1)
              {
                o << " <| " << pretty_print(*gap);
              }
              else
              {
                o << " ++ [" << pretty_print(boost::make_iterator_range(gap, elements.end())) << "]";
              }
            }
          }
          else {
            o << "[" << pretty_print(elements) << "]";
          }
        }
        else { // variable or other unknown head symbol
          o << expression;
        }

        return o;
      }
    }

  } // namespace data

} // namespace mcrl2

#endif

