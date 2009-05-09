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
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/abstraction.h"
#include "mcrl2/data/lambda.h"
#include "mcrl2/data/set.h"
#include "mcrl2/data/bag.h"
#include "mcrl2/data/where_clause.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/map_substitution_adapter.h"
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

          /// \brief the known sorts (pointer type to allow assignment)
          data_specification const*                                m_data_specification;

          /// \brief associated rewriter object (pointer type to allow assignment)
          mcrl2::data::detail::Rewriter*                           m_rewriter;

          /// \brief before rewriting
          atermpp::map< data_expression, data_expression >         m_implementation_context;

          /// \brief after rewriting
          atermpp::map< data_expression, data_expression >         m_reconstruction_context;

        public:

          // For normalising sort expressions
          sort_expression implement(sort_expression const& expression)
          {
            return m_data_specification->normalise(expression);
          }

          function_symbol implement(function_symbol const& f)
          {
            return function_symbol(f.name(), implement(f.sort()));
          }

          variable implement(variable const& v)
          {
            return variable(v.name(), implement(v.sort()));
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

              function_symbol operator()(mcrl2::data::sort_expression const& sort)
              {
                bool carry = true;

                for (std::string::iterator i = m_current.begin() + 7; carry && (i != m_current.end()); ++i) {
                  if (*i == '9') {
                    *i = 'a';

                    carry = false;
                  }
                  else if (*i == 'z') {
                    *i = '0';
                  }
                  else {
                    ++(*i);

                    carry = false;
                  }
                }

                if (carry) {
                  m_current.append("0");
                }

                return mcrl2::data::function_symbol(m_current, sort);
              }
            } symbol_generator;

            atermpp::map< data_expression, data_expression >::const_iterator i = m_implementation_context.find(expression);

            if (i == m_implementation_context.end())
            { // implementation with previously generated function
              atermpp::term_list< variable > bound_variables = implement(expression.variables());

              if (!bound_variables.empty())
              { // function with non-empty domain
                atermpp::aterm_appl body(implement(expression.body()));
                atermpp::term_list< variable > free_variables(implement(
                                                 boost::make_iterator_range(find_all_free_variables(expression))));

                function_sort   new_function_sort(sort_expression_list(gsGetSorts(bound_variables)), sort_expression(gsGetSort(body)));

                data_expression new_function(symbol_generator((free_variables.empty()) ? new_function_sort :
                                      function_sort(sort_expression_list(gsGetSorts(free_variables)), new_function_sort)));

                // lambda f : type_of(free_variables). lambda b. type_of(bound_variables) = body
                if (free_variables.empty())
                {
                  m_rewriter->addRewriteRule(data_equation(bound_variables, application(new_function, bound_variables), body));
                }
                else
                {
                  new_function = application(new_function, free_variables);

                  m_rewriter->addRewriteRule(data_equation(free_variables + bound_variables, application(new_function, bound_variables), body));
                }

                m_implementation_context[expression]   = new_function;
                m_reconstruction_context[new_function] = expression;

                return new_function;
              }

              return implement(expression.body());
            }

            return i->second;
          }

          data_expression implement(abstraction const& expression)
          {
            using namespace mcrl2::core::detail;
            using namespace mcrl2::data::sort_set_;
            using namespace mcrl2::data::sort_bag;

            data_expression abstract_body(implement(lambda(implement(expression.variables()), implement(expression.body()))));

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
              variables.push_back(r.front().lhs());
              arguments.push_back(implement(r.front().rhs()));
            }

            return (variables.empty()) ? implement(expression.body()) :
                application(implement(lambda(boost::make_iterator_range(variables), expression.body())),
                          boost::make_iterator_range(arguments));
          }

          template < typename ForwardTraversalIterator >
          atermpp::term_list< typename ForwardTraversalIterator::value_type > implement(boost::iterator_range< ForwardTraversalIterator > const& range)
          {
            atermpp::vector< typename ForwardTraversalIterator::value_type > result;

            for (typename boost::iterator_range< ForwardTraversalIterator >::const_iterator
                                                          i(range.begin()); i != range.end(); ++i)
            {
              result.push_back(implement(*i));
            }

            return convert< atermpp::term_list< typename ForwardTraversalIterator::value_type > >(result);
          }

          data_expression implement(application const& expression)
          {
            using namespace mcrl2::core::detail;

            // convert arguments
            atermpp::term_list< data_expression > arguments(implement(expression.arguments()));

            return application(implement(expression.head()), boost::make_iterator_range(arguments));
          }

          application reconstruct(application const& expression)
          {
            atermpp::vector< data_expression > arguments;

            for (application::arguments_const_range r(expression.arguments()); !r.empty(); r.advance_begin(1))
            {
              arguments.push_back(reconstruct(r.front()));
            }

            return application(reconstruct(expression.head()), boost::make_iterator_range(arguments));
          }

          data_expression reconstruct(data_expression const& expression)
          {
            if (expression.is_function_symbol())
            {
              atermpp::map< data_expression, data_expression >::const_iterator i(m_reconstruction_context.find(expression));

              if (i != m_reconstruction_context.end())
              {
                return i->second;
              }
            }
            else if (expression.is_application())
            {
              return reconstruct(application(expression));
            }

            return expression;
          }

          data_expression implement(data_expression const& expression)
          {
            using namespace mcrl2::core::detail;

            if (expression.is_application())
            {
              return implement(application(expression));
            }
            else if (expression.is_variable())
            {
              return implement(variable(expression));
            }
            else if (expression.is_function_symbol())
            {
              return implement(function_symbol(expression));
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
                                    detail::Rewriter& rewriter) :
                   m_data_specification(&specification),
                   m_rewriter(&rewriter)
          {
            // Add rewrite rules (needed only for lambda expressions)
            for (data_specification::equations_const_range r = specification.equations(); !r.empty(); r.advance_begin(1))
            {
              if (!m_rewriter->addRewriteRule(implement(r.front())))
              {
                throw mcrl2::runtime_error("Could not add rewrite rule!");
              }
            }
          }
      };
    } // namespace detail
  } // namespace data
} // namespace mcrl2

#endif
