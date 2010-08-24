// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/internal_format_conversion.h
/// \brief Conversion to normalise sorts and eliminate set/bag comprehension, list enumerations and numbers represented as string
/// \details The transformation of user functions to internal format is done first by employing the transformation in 
///            translate_user_notation_to_internal_format.h. After that sort normalisation is applied, and that is the only thing
///            this module is doing.

#ifndef MCRL2_DATA_DETAIL_INTERNAL_FORMAT_CONVERSION_H__
#define MCRL2_DATA_DETAIL_INTERNAL_FORMAT_CONVERSION_H__

#include "translate_user_notation_to_internal_format.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/atermpp/aterm_list.h"

namespace mcrl2 {

  namespace data {

    namespace detail {

      /**
       * Adapts the parse tree from the format after type checking to the
       * format used internally as part of data expressions.
       **/
      class internal_format_conversion_helper : public detail::expression_manipulator< internal_format_conversion_helper >
      {
        typedef detail::expression_manipulator< internal_format_conversion_helper > super;

        private:

          data_specification const& m_data_specification;

        public:

          using super::operator();

          sort_expression operator()(sort_expression const& s)
          {
            return m_data_specification.normalise_sorts(s);
          }

          identifier operator()(identifier const& i)
          {
            return i;
          }

          variable operator()(variable const& v)
          {
            return variable(v.name(), m_data_specification.normalise_sorts(v.sort()));
          }

          data_equation operator()(data_equation const& e)
          { 
            return data_equation((*this)(e.variables()), (*this)(e.condition()), // JK 15/10/2009 removed is_nil check
                       (*this)(e.lhs()), (*this)(e.rhs()));
          }

          data_expression operator()(function_symbol const& expression)
          {
            std::string name(expression.name());

            // expression may represent a number, if so, replace by its internal representation
            /* if (is_system_defined(expression.sort()) && (name.find_first_not_of("-/0123456789") == std::string::npos)) // crude but efficient
            {
              return number(expression.sort(), name);
            } */

            return function_symbol(expression.name(), m_data_specification.normalise_sorts(expression.sort()));
          }

          where_clause operator()(where_clause const& expression)
          { 
            return where_clause((*this)(expression.body()),(*this)(expression.declarations()));
          }

          /// Translates contained numeric expressions to their internal representations
          /// Eliminates set/bag comprehension and list enumeration
          data_expression operator()(abstraction const& expression)
          {
            /* using namespace sort_set;
            using namespace sort_bag;
            */

            variable_list bound_variables = atermpp::convert< variable_list >((*this)(expression.variables()));

            return abstraction(expression.binding_operator(), bound_variables, (*this)(expression.body()));
          }

          application operator()(application const& expression)
          {
            return application((*this)(expression.head()), (*this)(expression.arguments()));
          }

          // \deprecated exists only for backwards compatibility
          atermpp::aterm_appl operator()(atermpp::aterm_appl const& e)
          {
            if (is_sort_expression(e))
            {
              return (*this)(sort_expression(e));
            }

            return static_cast< super& >(*this)(e);
          }

          // assume the term represents a (linear) process or pbes
          internal_format_conversion_helper(data_specification const& specification)
            : m_data_specification(specification)
          {
          }
      };

      /// \brief Convenience overload for converting data/sort expressions using with atermpp:: functionality
      /// \param[in] term sort or data expression
      inline
      atermpp::aterm_appl internal_format_conversion_term(
                              const atermpp::aterm_appl term, 
                              data_specification const& specification)
      { 
        const atermpp::aterm_appl term1=translate_user_notation_to_internal_format(term);
        internal_format_conversion_helper converter(specification);

        return atermpp::replace(term1, converter);
      }

      /// \brief Convenience overload for converting data/sort expressions using with atermpp:: functionality
      /// \param[in] term sort or data expression
      template <typename T>
      inline
      atermpp::term_list <T> internal_format_conversion_list(
                                 const atermpp::term_list<T> terms, 
                                 const data_specification & specification)
      { 
        atermpp::term_list<T> result;
        for(typename atermpp::term_list<T>::const_iterator i=terms.begin(); 
                   i!=terms.end(); ++i)
        { 
          result=atermpp::push_front(result,T(internal_format_conversion_term(*i,specification)));
        }

        return atermpp::reverse(result); 
      }

      /// \brief Convenience overload for use with atermpp:: functionality
      /// \param[in] term process, linear process specification or pbes
      inline
      atermpp::aterm_appl internal_format_conversion_deprecated(atermpp::aterm_appl term, data_specification const& specification)
      {
        atermpp::aterm_appl deprecated_data_specification=*term.begin(); // Do not use the data specification that
        assert(deprecated_data_specification==atermpp::aterm_appl());    // is provided as the first argument in the ATerm, but use the type
                                                                         // checked data specification instead.

        internal_format_conversion_helper converter(specification);

        atermpp::vector< atermpp::aterm_appl > arguments;

        arguments.push_back(*term.begin());

        for (atermpp::aterm_appl::iterator i = ++term.begin(); i != term.end(); ++i)
        {
          const atermpp::aterm_appl term=translate_user_notation_to_internal_format(*i);
          arguments.push_back(atermpp::replace(term, converter));
        }

        return atermpp::aterm_appl(term.function(), arguments.begin(), arguments.end());
      }

    } // namespace detail
  } // namespace data
} // namespace mcrl2
#endif

