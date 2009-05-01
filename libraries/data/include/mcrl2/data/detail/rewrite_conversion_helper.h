// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/implement_helper.h
/// \brief The class rewriter.

#ifndef MCRL2_DATA_DETAIL_REWRITE_CONVERSION_HELPER_H
#define MCRL2_DATA_DETAIL_REWRITE_CONVERSION_HELPER_H

#include "boost/assert.hpp"

#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/data/substitution.h"
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/abstraction.h"
#include "mcrl2/data/lambda.h"
#include "mcrl2/data/set.h"
#include "mcrl2/data/bag.h"
#include "mcrl2/data/where_clause.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/exception.h"

namespace mcrl2 {

  namespace data {

    namespace detail {

      /// \brief Component that provides minimal `implementation' and `reconstruction'
      ///
      /// part of old data implementation that is needed by the rewriter
      /// This code should become obsolete when the rewriter can deal with
      /// abstraction.
      class rewrite_conversion_helper {

        private:

          /// \brief associated rewriter object
          mcrl2::data::detail::Rewriter&                           m_rewriter;

          /// \brief before rewriting
          mutable_substitution< data_expression, data_expression > m_implementation_context;

          /// \brief after rewriting
          mutable_substitution< data_expression, data_expression > m_reconstruction_context;

          /// \brief the known sorts
          atermpp::set< sort_expression >                          m_known_sorts;

        public:

          // For normalising sort expressions
          sort_expression implement(sort_expression const& expression)
          {
            if (m_known_sorts.find(expression) == m_known_sorts.end())
            {
              m_known_sorts.insert(expression);

              // add equations for standard functions for new sorts
              data_equation_vector equations(standard_generate_equations_code(expression));

              for (data_equation_vector::const_iterator i = equations.begin(); i != equations.end(); ++i)
              {
                m_rewriter.addRewriteRule(implement(*i));
              }
            }

            return expression;
          }

          data_equation implement(data_equation const& equation)
          {
            return data_equation(
              implement(equation.variables()),
              implement(equation.condition()),
              implement(equation.lhs()),
              implement(equation.rhs()));
          }

          data_expression implement(lambda const& expression)
          {
            using namespace mcrl2::core::detail;

            // Using local generator because existing generators do not satisfy requirements and performance expectations
            struct local_generator {

              std::string m_current;

              local_generator() : m_current("lambda@")
              {
              }

              mcrl2::data::variable operator()(mcrl2::data::sort_expression const& sort)
              {
                bool carry = true;

                for (std::string::iterator i = m_current.begin() + 5; carry && (i != m_current.end()); ++i) {
                  if (*i == '9') {
                    *i = 'a';

                    carry = false;
                  }
                  else if (*i == 'z') {
                    *i = '0';
                  }
                }

                if (carry) {
                  m_current.append("0");
                }

                return mcrl2::data::variable(m_current, sort);
              }
            } variable_generator;

            data_expression converted(m_implementation_context(expression));

            if (converted != expression)
            { // implementation with previously generated function
              atermpp::term_list< variable > bound_variables;

              for (lambda::variables_const_range r(expression.variables()); !r.empty(); r.advance_begin(1))
              {
                atermpp::push_front(bound_variables, variable(r.front().name(), implement(r.front().sort())));
              }

              if (!bound_variables.empty())
              { // function with non-empty domain
                atermpp::aterm_appl body(implement(expression.body()));
                atermpp::term_list< variable > free_variables(get_free_vars(body));

                function_sort   new_function_sort(sort_expression_list(gsGetSorts(free_variables)),
                         function_sort(sort_expression_list(gsGetSorts(bound_variables)), sort_expression(gsGetSort(body))));
                function_symbol new_function_symbol(variable_generator(new_function_sort), new_function_sort);
                application     new_function(application(new_function_symbol, free_variables));

                // lambda f : type_of(free_variables). lambda b. type_of(bound_variables) = body
                m_rewriter.addRewriteRule(
                  data_equation(free_variables + bound_variables, application(new_function, bound_variables),body));

                m_implementation_context[expression]   = new_function;
                m_reconstruction_context[new_function] = expression;

                return new_function;
              }

              return expression.body();
            }

            return converted;
          }

          data_expression implement(abstraction const& expression)
          {
            using namespace mcrl2::core::detail;
            using namespace mcrl2::data::sort_set_;
            using namespace mcrl2::data::sort_bag;

            data_expression abstract_body(implement(lambda(expression.variables(), expression.body())));

            if (is_setcomprehension_application(expression))
            {
              return setcomprehension(set_(expression.variables()[0].sort()), abstract_body);
            }
            else if (is_bagcomprehension_application(expression))
            {
              return bagcomprehension(bag(expression.variables()[0].sort()), abstract_body);
            }
            else if (expression.is_exists())
            {
              return data_expression(gsMakeDataExprExists(abstract_body));
            }
            else if (expression.is_forall())
            {
              return data_expression(gsMakeDataExprForall(abstract_body));
            }

            return abstract_body;
          }

          data_expression implement(where_clause const& expression)
          { // return corresponding lambda expression
            atermpp::vector< variable >        variables;
            atermpp::vector< data_expression > arguments;

            for (where_clause::declarations_const_range r(expression.declarations()); !r.empty(); r.advance_begin(1))
            {
              variables.push_back(implement(r.front().lhs()));
              arguments.push_back(implement(r.front().rhs()));
            }

            return (variables.empty()) ? implement(expression.body()) :
                application(lambda(boost::make_iterator_range(variables), implement(expression.body())),
                          boost::make_iterator_range(arguments));
          }

          template < typename ForwardTraversalIterator >
          atermpp::term_list< data_expression > implement(boost::iterator_range< ForwardTraversalIterator > const& range)
          {
            atermpp::vector< data_expression > result;

            for (typename boost::iterator_range< ForwardTraversalIterator >::const_iterator
                                                          i(range.begin()); i != range.end(); ++i)
            {
              result.push_back(implement(*i));
            }

            return convert< atermpp::term_list< data_expression > >(result);
          }

          data_expression implement(application const& expression)
          {
            using namespace mcrl2::core::detail;

            // convert arguments
            atermpp::term_list< data_expression > arguments(implement(expression.arguments()));

            if (expression.is_function_symbol()) {
              if (expression.head() == gsMakeOpIdNameListEnum())
              { // convert to snoc list
                return sort_list::list(
                  container_sort(function_sort(expression.head().sort()).target_sort()).element_sort(),
                                                                          boost::make_iterator_range(arguments));
              }
              else if (expression.head() == gsMakeOpIdNameSetEnum())
              { // convert to finite set
                return sort_fset::fset(
                  container_sort(function_sort(expression.head().sort()).target_sort()).element_sort(),
                                                                          boost::make_iterator_range(arguments));
              }
              else if (expression.head() == gsMakeOpIdNameBagEnum())
              { // convert to finite bag
                return sort_fbag::fbag(
                  container_sort(function_sort(expression.head().sort()).target_sort()).element_sort(),
                                                                          boost::make_iterator_range(arguments));
              }
            }

            return application(implement(expression.head()), boost::make_iterator_range(arguments));
          }

          data_expression reconstruct(data_expression const& expression)
          {
            return atermpp::replace(expression, m_reconstruction_context);
          }

          data_expression implement(data_expression const& expression)
          {
            using namespace mcrl2::core::detail;

            if (expression.is_application())
            {
              return implement(application(expression));
            }
            else if (mcrl2::core::detail::gsIsDataExprNumber(expression))
            { //part is a number; replace by its internal representation (should be obsolete)
              return number(expression.sort(), atermpp::aterm_appl(expression(0)).function().name());
            }
            else if (expression.is_abstraction())
            {
              return implement(abstraction(expression));
            }
            else if (expression.is_where_clause())
            {
              return implement(where_clause(expression));
            }

            return expression;
          }

          rewrite_conversion_helper(data_specification const& specification,
                                    mcrl2::data::detail::Rewriter& rewriter) :
                   m_rewriter(rewriter),
                   m_known_sorts(convert< atermpp::set< sort_expression > >(specification.sorts()))
          {
            // Use aliases for type normalisation using substitutions
            for (data_specification::aliases_const_range r(specification.aliases()); !r.empty(); r.advance_begin(1))
            {
              m_known_sorts.insert(r.front().name());

              m_implementation_context[r.front().name()] = r.front().reference();
              m_reconstruction_context[r.front().reference()] = r.front().name();
            }

            // Add rewrite rules (needed only for lambda expressions)
            for (data_specification::equations_const_range r = specification.equations(); !r.empty(); r.advance_begin(1))
            {
    std::cerr << "RULE " << implement(r.front()) << std::endl;
              if (!m_rewriter.addRewriteRule(implement(r.front())))
              {
                throw mcrl2::runtime_error("Could not add rewrite rule!");
              }
            }
          }

          rewrite_conversion_helper(mcrl2::data::detail::Rewriter& rewriter) : m_rewriter(rewriter)
          {
          }
      };
    } // namespace detail
  } // namespace data
} // namespace mcrl2

#endif
