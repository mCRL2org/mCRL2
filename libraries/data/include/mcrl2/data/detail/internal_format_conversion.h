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
#include "mcrl2/data/detail/manipulator.h"

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
          { // std::cerr << "Data equation:: " << e << "\n";
            return data_equation((*this)(e.variables()), (*this)(e.condition()), // JK 15/10/2009 removed is_nil check
                       (*this)(e.lhs()), (*this)(e.rhs()));
          }

          data_expression operator()(function_symbol const& expression)
          {
            std::string name(expression.name());

            // expression may represent a number, if so, replace by its internal representation
            if (expression.sort().is_system_defined() && (name.find_first_not_of("-/0123456789") == std::string::npos)) // crude but efficient
            {
              return number(expression.sort(), name);
            }

            return function_symbol(expression.name(), m_data_specification.normalise_sorts(expression.sort()));
          }

          /// Translates contained numeric expressions to their internal representations
          /// Eliminates set/bag comprehension and list enumeration
          data_expression operator()(abstraction const& expression)
          {
            using namespace sort_set;
            using namespace sort_bag;

            variable_list bound_variables = convert< variable_list >((*this)(expression.variables()));

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
            if (expression.head().is_function_symbol()) {
              function_symbol head(expression.head());

              if (head.name() == "@ListEnum")
              { // convert to snoc list
                sort_expression element_sort(m_data_specification.normalise_sorts(*function_sort(head.sort()).domain().begin()));

                return m_data_specification.normalise_sorts(sort_list::list(element_sort, (*this)(expression.arguments())));
              }
              else if (head.name() == "@SetEnum")
              { // convert to finite set
                sort_expression element_sort((*this)(*function_sort(head.sort()).domain().begin()));

                return m_data_specification.normalise_sorts(sort_set::setfset(element_sort, sort_fset::fset(element_sort, (*this)(expression.arguments()))));
              }
              else if (head.name() == "@BagEnum")
              { // convert to finite bag
                sort_expression element_sort((*this)(*function_sort(head.sort()).domain().begin()));

                return m_data_specification.normalise_sorts(sort_bag::bagfbag(element_sort, sort_fbag::fbag(element_sort, (*this)(expression.arguments()))));
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

            return static_cast< super& >(*this)(e);
          }

          /// Translates the numeric expressions to their internal representations
          void operator()(data_specification& specification)
          {
            std::set< data_equation > to_remove;
            std::set< data_equation > to_insert;

            for (data_specification::equations_const_range r(specification.equations()); !r.empty(); r.advance_begin(1))
            {
              data_equation converted_equation((*this)(r.front()));

              if (r.front() != converted_equation)
              { // assumes that the range is not invalidated by inserts and remove operations
                // this means that equations cannot be inserted and removed within this loop!
                to_remove.insert(r.front());
                to_insert.insert(converted_equation);
              }
            }

            std::for_each(to_remove.begin(), to_remove.end(), boost::bind(&data_specification::remove_equation, &specification, _1));
            std::for_each(to_insert.begin(), to_insert.end(), boost::bind(&data_specification::add_equation, &specification, _1));
          }

          // assume the term represents a (linear) process or pbes
          internal_format_conversion_helper(data_specification const& specification)
            : m_data_specification(specification)
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

        atermpp::vector< atermpp::aterm_appl > arguments;

        arguments.push_back(*term.begin());

        for (atermpp::aterm_appl::iterator i = ++term.begin(); i != term.end(); ++i)
        {
          arguments.push_back(atermpp::replace(*i, converter));
        }

        return atermpp::aterm_appl(term.function(), arguments.begin(), arguments.end());
      }

    } // namespace detail
  } // namespace data
} // namespace mcrl2
#endif

