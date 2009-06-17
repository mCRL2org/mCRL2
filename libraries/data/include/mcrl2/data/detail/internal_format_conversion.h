// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/internal_format_conversion.h
/// \brief Conversion to eliminate set/bag comprehension, list enumerations and numbers represented as string

#ifndef MCRL2_DATA_DETAIL_INTERNAL_FORMAT_CONVERSION_H__
#define MCRL2_DATA_DETAIL_INTERNAL_FORMAT_CONVERSION_H__

#include "boost/bind.hpp"

#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/where_clause.h"
#include "mcrl2/data/real.h"
#include "mcrl2/data/int.h"
#include "mcrl2/data/nat.h"
#include "mcrl2/data/pos.h"
#include "mcrl2/data/set.h"
#include "mcrl2/data/bag.h"
#include "mcrl2/data/lambda.h"
#include "mcrl2/data/abstraction.h"
#include "mcrl2/data/application.h"
#include "mcrl2/data/assignment.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/data_equation.h"
#include "mcrl2/data/data_specification.h"

namespace mcrl2 {

  namespace data {

    namespace detail {

      /*
       * Adapts the parse tree from the format after type checking to the
       * format used internally as part of data expressions.
       **/
      class internal_format_conversion_helper
      {

        private:

          data_specification const& m_data_specification;

        public:

          variable operator()(variable const& v)
          {
            return variable(v.name(), m_data_specification.normalise(v.sort()));
          }

          data_expression operator()(function_symbol const& expression)
          {
            std::string name(expression.name());

            // expression may represent a number, if so, replace by its internal representation
            if (expression.sort().is_system_defined() && (name.find_first_not_of("-/0123456789") == std::string::npos)) // crude but efficient
            {
              return number(expression.sort(), name);
            }

            return function_symbol(name, m_data_specification.normalise(expression.sort()));
          }

          /// Translates a range of expressions to their internal representations
          template < typename ForwardTraversalIterator >
          atermpp::vector< data_expression > operator()(boost::iterator_range< ForwardTraversalIterator > const& range)
          {
            atermpp::vector< data_expression > expressions;

            for (ForwardTraversalIterator i = range.begin(); i != range.end(); ++i)
            {
              expressions.push_back((*this)(*i));
            }

            return expressions;
          }

          /// Translates contained numeric expressions to their internal representations
          assignment operator()(assignment const& a)
          {
            return assignment((*this)(a.lhs()), (*this)(a.rhs()));
          }

          /// Translates contained numeric expressions to their internal representations
          where_clause operator()(where_clause const& expression)
          {
            return where_clause((*this)(expression.body()), (*this)(expression.declarations()));
          }

          /// Translates contained numeric expressions to their internal representations
          /// Eliminates set/bag comprehension and list enumeration
          data_expression operator()(abstraction const& expression)
          {
            using namespace sort_set_;
            using namespace sort_bag;

            variable_list bound_variables = convert< variable_list >((*this)(expression.variables()));

            if (atermpp::function_symbol(atermpp::arg1(expression).function()).name() == "SetComp")
            {
              sort_expression element_sort((*this)(expression.variables()[0].sort()));

              return setcomprehension(set_(element_sort), lambda(bound_variables, (*this)(expression.body())));
            }
            else if (atermpp::function_symbol(atermpp::arg1(expression).function()).name() == "BagComp")
            {
              sort_expression element_sort((*this)(expression.variables()[0].sort()));

              return bagcomprehension(bag(element_sort), lambda(bound_variables, (*this)(expression.body())));
            }

            return abstraction(expression.binding_operator(), bound_variables, (*this)(expression.body()));
          }

          application operator()(application const& expression)
          {
            atermpp::vector< data_expression > arguments((*this)(expression.arguments()));

            if (expression.head().is_function_symbol()) {
              function_symbol head(expression.head());

              if (head.name() == "@ListEnum")
              { // convert to snoc list
                sort_expression element_sort((*this)(function_sort(head.sort()).domain()[0]));

                return sort_list::list(element_sort, arguments);
              }
              else if (head.name() == "@SetEnum")
              { // convert to finite set
                sort_expression element_sort((*this)(function_sort(head.sort()).domain()[0]));

                return sort_set_::setfset(element_sort, sort_fset::fset(element_sort, arguments));
              }
              else if (head.name() == "@BagEnum")
              { // convert to finite bag
                sort_expression element_sort((*this)(function_sort(head.sort()).domain()[0]));

                return sort_bag::bagfbag(element_sort, sort_fbag::fbag(element_sort, arguments));
              }
            }

            return application((*this)(expression.head()), arguments);
          }

          /// Translates the numeric expressions to their internal representations
          data_equation operator()(data_equation const& equation)
          {
            return data_equation(
              (*this)(equation.variables()),
              (*this)(equation.condition()),
              (*this)(equation.lhs()),
              (*this)(equation.rhs()));
          }

          /// Translates the numeric expressions to their internal representations
          ///
          /// At some point this code should either be reimplemented as a generated
          /// visitor, or removed when the parser/type-checker takes care of it.
          data_expression operator()(data_expression const& expression)
          {
            if (expression.is_application())
            {
              return (*this)(application(expression));
            }
            else if (expression.is_function_symbol())
            {
              return (*this)(function_symbol(expression));
            }
            else if (expression.is_variable())
            {
              return (*this)(variable(expression));
            }
            else if (expression.is_where_clause())
            {
              return (*this)(where_clause(expression));
            }
            else if (expression.is_abstraction())
            {
              return (*this)(abstraction(expression));
            }

            return expression;
          }

          /// Translates the numeric expressions to their internal representations
          void operator()(data_specification& specification)
          {
            std::set< data_equation > to_remove;

            for (data_specification::equations_const_range r(specification.equations()); !r.empty(); r.advance_begin(1))
            {
              data_equation converted_equation((*this)(r.front()));

              if (r.front() != converted_equation)
              { // assumes that the range is not invalidated by inserts and remove operations
                to_remove.insert(converted_equation);

                specification.add_equation(converted_equation);
              }
            }

            specification.remove_equations(boost::make_iterator_range(to_remove));
          }

          /// \brief Convenience overload for use with atermpp:: functionality
          atermpp::aterm_appl operator()(atermpp::aterm_appl const& term)
          {
            if (is_data_expression(term))
            {
              return (*this)(data_expression(term));
            }
            else if (is_sort_expression(term))
            {
              return m_data_specification.normalise(sort_expression(term));
            }

            return term;
          }

          // assume the term represents a (linear) process or pbes
          internal_format_conversion_helper(data_specification const& specification) :
                                                         m_data_specification(specification)
          {
          }
      };

      inline
      void internal_format_conversion(data_specification& specification)
      {
        internal_format_conversion_helper converter(specification);

        converter(specification);
      }

      inline
      data_specification internal_format_conversion(data_specification const& specification)
      {
        data_specification                copy(specification);
        internal_format_conversion_helper converter(specification);

        converter(copy);

        return copy;
      }

      /// \brief Convenience overload for converting data/sort expressions using with atermpp:: functionality
      /// \param[in] term sort or data expression
      inline
      atermpp::aterm_appl internal_format_conversion(data_specification const& specification, atermpp::aterm_appl term)
      {
        internal_format_conversion_helper converter(specification);

        return atermpp::replace(term, converter);
      }

      /// \brief Convenience overload for use with atermpp:: functionality
      /// \param[in] term process, linear process specification or pbes
      inline
      atermpp::aterm_appl internal_format_conversion(atermpp::aterm_appl term)
      {
        data_specification                specification(*term.begin());
        internal_format_conversion_helper converter(specification);

        return atermpp::replace(term, converter);
      }

    } // namespace detail
  } // namespace data
} // namespace mcrl2
#endif

