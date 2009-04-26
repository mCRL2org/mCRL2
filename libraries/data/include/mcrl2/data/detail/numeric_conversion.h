// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/numeric_conversion.h
/// \brief Conversion functions for string representation of numbers to the internal representation

#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/where_clause.h"
#include "mcrl2/data/abstraction.h"
#include "mcrl2/data/application.h"
#include "mcrl2/data/assignment.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/data_equation.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/core/detail/struct.h" // for gsIsDataExprNumber

namespace mcrl2 {

  namespace data {

    namespace detail {

      data_expression numeric_conversion(data_expression const& expression);

      /// Translates contained numeric expressions to their internal representations
      inline
      where_clause numeric_conversion(where_clause const& expression)
      {
        std::vector< data_expression > declarations;

        for (where_clause::declarations_const_range r(expression.declarations()); !r.empty(); r.advance_begin(1))
        {
          declarations.push_back(assignment(
                  numeric_conversion(r.front().lhs()),
                  numeric_conversion(r.front().rhs())));
        }

        return where_clause(numeric_conversion(expression.body()), boost::make_iterator_range(declarations));
      }

      /// Translates contained numeric expressions to their internal representations
      inline
      abstraction numeric_conversion(abstraction const& expression)
      {
        return abstraction(expression.binding_operator(), expression.variables(), numeric_conversion(expression.body()));
      }

      inline
      application numeric_conversion(application const& expression)
      {
        std::vector< data_expression > arguments;

        for (application::arguments_const_range r(expression.arguments()); !r.empty(); r.advance_begin(1))
        {
          arguments.push_back(numeric_conversion(r.front()));
        }

        return application(numeric_conversion(expression.head()), boost::make_iterator_range(arguments));
      }

      /// Translates the numeric expressions to their internal representations
      inline
      data_equation numeric_conversion(data_equation const& equation)
      {
        return data_equation(equation.variables(),
          numeric_conversion(equation.condition()),
          numeric_conversion(equation.lhs()),
          numeric_conversion(equation.rhs()));
      }

      /// Translates the numeric expressions to their internal representations
      ///
      /// At some point this code should either be reimplemented as a generated
      /// visitor, or removed when the parser/type-checker takes care of it.
      inline
      data_expression numeric_conversion(data_expression const& expression)
      {
        if (mcrl2::core::detail::gsIsDataExprNumber(expression))
        { //part is a number; replace by its internal representation
          std::string number(atermpp::aterm_appl(expression(0)).function().name());
          if (expression.sort() == sort_pos::pos())
          {
            return sort_pos::pos(number);
          }
          if (expression.sort() == sort_nat::nat())
          {
            return sort_nat::nat(number);
          }
          if (expression.sort() == sort_int_::int_())
          {
            return sort_int_::int_(number);
          }
          if (expression.sort() == sort_real_::real_())
          {
            return sort_real_::real_(number);
          }
        }
        else if (expression.is_application())
        {
          numeric_conversion(application(expression));
        }
        else if (expression.is_where_clause())
        {
          numeric_conversion(where_clause(expression));
        }
        else if (expression.is_abstraction())
        {
          numeric_conversion(abstraction(expression));
        }

        return expression;
      }


      /// Translates the numeric expressions to their internal representations
      inline
      void numeric_conversion(data_specification& specification)
      {
        std::set< data_equation > to_remove;

        for (data_specification::equations_const_range r(specification.equations()); !r.empty(); r.advance_begin(1))
        {
          data_equation converted_equation(numeric_conversion(r.front()));

          if (r.front() != converted_equation)
          { // assumes that the range is not invalidated by inserts and remove operations
            to_remove.insert(converted_equation);

            specification.add_equation(converted_equation);
          }
        }

        specification.remove_equations(boost::make_iterator_range(to_remove));
      }

      inline
      data_specification numeric_conversion(data_specification const& specification)
      {
        data_specification copy(specification);

        numeric_conversion(copy);

        return copy;
      }
    } // namespace detail
  } // namespace data
} // namespace mcrl2
