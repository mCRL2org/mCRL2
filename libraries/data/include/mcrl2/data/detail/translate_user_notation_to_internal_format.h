// Author(s): Jeroen van der Wulp, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/translate_user_notation_to_internal_format.h
/// \brief Conversion to eliminate set/bag comprehension, list enumerations and numbers represented as string.
//         Works only on data terms.

#ifndef MCRL2_DATA_DETAIL_TRANSLATE_USER_NOTATION_TO_INTERNAL_FORMAT_H__
#define MCRL2_DATA_DETAIL_TRANSLATE_USER_NOTATION_TO_INTERNAL_FORMAT_H__

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
#include "mcrl2/data/detail/manipulator.h"

namespace mcrl2 {

  namespace data {

    namespace detail {

      /**
       * Adapts the parse tree from the format after type checking to the
       * format used internally as part of data expressions.
       **/
      class translate_user_notation_to_internal_format_helper : public detail::expression_manipulator< translate_user_notation_to_internal_format_helper >
      {
        typedef detail::expression_manipulator< translate_user_notation_to_internal_format_helper > super;

        public:

          using super::operator();

          sort_expression operator()(sort_expression const& s)
          {
            return s;
          }

          variable operator()(variable const& v)
          {
            return v;
          }

          identifier operator()(identifier const& i)
          {
            return i;
          }


          data_expression operator()(function_symbol const& expression)
          {
            std::string name(expression.name());

            // expression may represent a number, if so, replace by its internal representation
            if (is_system_defined(expression.sort()) && (name.find_first_not_of("-/0123456789") == std::string::npos)) // crude but efficient
            {
              return number(expression.sort(), name);
            }

            return function_symbol(expression.name(), expression.sort());
          }

          where_clause operator()(where_clause const& expression)
          {
            return where_clause((*this)(expression.body()),(*this)(expression.declarations()));
          }

          assignment_expression operator()(assignment_expression const &expression)
          { 
            return assignment_expression(expression.lhs(),(*this)(expression.rhs()));
          }

          /// Translates contained numeric expressions to their internal representations
          /// Eliminates set/bag comprehension and list enumeration
          data_expression operator()(abstraction const& expression)
          {
            using namespace sort_set;
            using namespace sort_bag;

            variable_list bound_variables = atermpp::convert< variable_list >((*this)(expression.variables()));

            if (atermpp::function_symbol(atermpp::arg1(expression).function()).name() == "SetComp")
            {
              sort_expression element_sort((*this)(expression.variables().begin()->sort()));
              return setconstructor(element_sort, lambda(bound_variables, (*this)(expression.body())),sort_fset::fset_empty(element_sort));
            }
            else if (atermpp::function_symbol(atermpp::arg1(expression).function()).name() == "BagComp")
            {
              sort_expression element_sort((*this)(expression.variables().begin()->sort()));

              return bagconstructor(element_sort, lambda(bound_variables, (*this)(expression.body())), sort_fbag::fbag_empty(element_sort));
            }

            return abstraction(expression.binding_operator(), bound_variables, (*this)(expression.body()));
          }

          application operator()(application const& expression)
          {
            if (is_function_symbol(expression.head())) 
            {
              function_symbol head(expression.head());

              if (head.name() == "@ListEnum")
              { // convert to snoc list
                sort_expression element_sort(*function_sort(head.sort()).domain().begin());

                return sort_list::list(element_sort, (*this)(expression.arguments()));
              }
              else if (head.name() == "@SetEnum")
              { // convert to finite set
                sort_expression element_sort((*this)(*function_sort(head.sort()).domain().begin()));

                return sort_set::setfset(element_sort, sort_fset::fset(element_sort, (*this)(expression.arguments())));
              }
              else if (head.name() == "@BagEnum")
              { // convert to finite bag
                using namespace sort_bag;
                sort_expression element_sort((*this)(*function_sort(head.sort()).domain().begin()));
                return sort_bag::bagfbag(element_sort, sort_fbag::fbag(element_sort, (*this)(expression.arguments())));
              }
            }

            return application((*this)(expression.head()), (*this)(expression.arguments()));
          }

          // \deprecated exists only for backwards compatibility
          atermpp::aterm_appl operator()(atermpp::aterm_appl const& e)
          { 
            if (is_sort_expression(e))
            {
              return (*this)(sort_expression(e));
            }
            if (is_abstraction(e))
            { 
              return (*this)(abstraction(e));
            }
            if (is_function_symbol(e))
            { 
              return (*this)(function_symbol(e));
            }
            if (is_application(e))
            { 
              return (*this)(application(e));
            }
            if (is_identifier(e))
            { 
              return (*this)(identifier(e));
            }

            return static_cast< super& >(*this)(e);
          } 

          /// Constructor
          translate_user_notation_to_internal_format_helper()
          {}
      };

      
      /// \brief Convenience overload for converting data/sort expressions using with atermpp:: functionality
      /// \param[in] term sort or data expression
      
      template <typename T>
      T translate_user_notation_to_internal_format(const T &term)
      { 
        translate_user_notation_to_internal_format_helper converter;

        return atermpp::replace(term, converter);
      }

    } // namespace detail
  } // namespace data
} // namespace mcrl2
#endif

