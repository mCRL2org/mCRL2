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
#include "mcrl2/atermpp/filter_iterator.h"
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/abstraction.h"
#include "mcrl2/data/lambda.h"
#include "mcrl2/data/set.h"
#include "mcrl2/data/bag.h"
#include "mcrl2/data/exists.h"
#include "mcrl2/data/forall.h"
#include "mcrl2/data/where_clause.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/data/identifier_generator.h"
#include "mcrl2/data/substitutions.h"
#include "mcrl2/exception.h"

namespace mcrl2
{

namespace data
{

namespace detail
{

/// \brief Component that provides minimal `implementation' and `reconstruction'
///
/// part of old data implementation that is needed by the rewriter
/// This code should become obsolete when the rewriter can deal with
/// abstraction.
template < class Rewriter >
class rewrite_conversion_helper
{

  private:

    /// \brief the known sorts (pointer type to allow assignment)
    // data_specification const*                                m_data_specification;

    /// \brief associated rewriter object (pointer type to allow assignment)
    Rewriter*                                                m_rewriter;

    /// \brief before rewriting
    atermpp::map< data_expression, data_expression >         m_implementation_context;

    /// \brief after rewriting
    atermpp::map< data_expression, data_expression >         m_reconstruction_context;

  protected:

    /* template < typename Sequence >
    void initialise(Sequence const& s)
    {
      // Add rewrite rules (needed only for lambda expressions)
      for (typename Sequence::const_iterator i = s.begin(); i != s.end(); ++i)
      {
        if (!m_rewriter->addRewriteRule(implement(*i)))
        {
          throw mcrl2::runtime_error("Could not add rewrite rule!");
        }
      }
    } */

    // Implementor has a pointer to a rewrite conversion helper.
    // This allows the implementor object to remain constant, wheras the
    // rewrite_conversion_helper can be changed.
    struct implementor
    {
      rewrite_conversion_helper* m_owner;

      implementor(rewrite_conversion_helper* owner) : m_owner(owner)
      {}

      template < typename Expression >
      data_expression operator()(Expression const& expression) const
      {
        return m_owner->implement(expression);
      }
    };

    struct reconstructor
    {
      rewrite_conversion_helper* m_owner;

      reconstructor(rewrite_conversion_helper* owner) : m_owner(owner)
      {}

      template < typename Expression >
      data_expression operator()(Expression const& expression) const
      {
        return m_owner->reconstruct(expression);
      }
    };

  public:

    // For normalising sort expressions
    sort_expression implement(const sort_expression& s)
    {
      return s;
    }

    function_symbol implement(const function_symbol& f)
    {
      return f;
    }

    variable implement(const variable& v)
    {
      return v;
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

      static number_postfix_generator symbol_generator("lambda@");
      const atermpp::map< data_expression, data_expression >::const_iterator i = m_implementation_context.find(expression);
      if (i == m_implementation_context.end())
      {
        const atermpp::term_list< variable > bound_variables = atermpp::convert< atermpp::term_list< variable > >(implement(expression.variables()));

        if (!bound_variables.empty())
        {
          // We encounter an expression of the form
          //     lambda x1...xn.body(y1,...,ym,x1,...,xn) where xi are the bound variables, and yj are free variables in body.
          // This expression is replaced by a new function symbol lambda@ and a new rewrite rule as follows:
          //
          //     lambda x1...xn.body(y1,...,ym,x1,...,xn) --> lambda@(y1,...,yn)
          //
          // The new rewrite rule is
          //
          //     lamba@(y1,...,ym)(x1,...xn)=body(y1,...,ym,x1,...xn).
          //
          // When an expression must be translated back, an occurrence of lamba@ is translated into
          //
          //     lambda@ --> lambda y1...ym.lambda.x1...xn.body(y1,...,ym,x1,...xn)
          //
          // Below the variables x1...xn are the bound_variables and y1...ym are the free_variables.

          const data_expression body(implement(expression.body()));
          const atermpp::term_list< variable > free_variables = atermpp::convert< atermpp::term_list< variable > >
              (implement(find_free_variables_with_bound(expression, bound_variables)));

          function_sort lambdaAt_function_sort(make_sort_range(bound_variables), sort_expression(body.sort()));
          if (!free_variables.empty())
          {
            lambdaAt_function_sort=function_sort(make_sort_range(free_variables), lambdaAt_function_sort);
          }

          const data_expression lambdaAt_function(function_symbol(symbol_generator(), lambdaAt_function_sort));

          m_rewriter->addRewriteRule(data_equation(free_variables + bound_variables,
                                     (free_variables.empty()?
                                      application(lambdaAt_function, bound_variables):
                                      application(application(lambdaAt_function, free_variables),bound_variables)), body));
          m_rewriter->data_equation_selector.add_function_symbols(lambdaAt_function);

          m_reconstruction_context[lambdaAt_function] =
            (free_variables.empty()?
             lambda(bound_variables,body):
             lambda(free_variables,lambda(bound_variables,body)));
          assert(lambdaAt_function.sort()==m_reconstruction_context[lambdaAt_function].sort());

          const data_expression result(free_variables.empty() ? lambdaAt_function : application(lambdaAt_function,free_variables));
          m_implementation_context[expression]   = result;
          assert(expression.sort()==result.sort());
          return result;
        }

        return implement(expression.body());
      }

      // implementation with previously generated function.
      return i->second;
    }

    data_expression implement(abstraction const& expression)
    {
      using namespace mcrl2::core::detail;
      using namespace mcrl2::data::sort_set;
      using namespace mcrl2::data::sort_bag;

      if (!is_lambda(expression))
      {
        data_expression abstract_body(implement(lambda(expression.variables(), expression.body())));

        if (is_setcomprehension_application(expression))
        {
          return setcomprehension(set_(expression.variables().begin()->sort()), abstract_body);
        }
        else if (is_bagcomprehension_application(expression))
        {
          return bagcomprehension(bag(expression.variables().begin()->sort()), abstract_body);
        }
        else if (is_exists(expression))
        {
           const function_symbol exists("exists", make_function_sort(abstract_body.sort(), sort_bool::bool_()));
           m_rewriter->data_equation_selector.add_function_symbols(exists);
           return exists(abstract_body);
        }
        else if (is_forall(expression))
        {
          const function_symbol forall("forall", make_function_sort(abstract_body.sort(), sort_bool::bool_()));
          m_rewriter->data_equation_selector.add_function_symbols(forall);
          return forall(abstract_body);
        }
      }

      return implement(lambda(expression));
    }

    data_expression implement(where_clause const& w)
    {
      // return corresponding lambda expression
      return (make_assignment_left_hand_side_range(w.declarations()).empty()) ? implement(w.body()) :
             application(implement(lambda(make_assignment_left_hand_side_range(w.declarations()), w.body())),
                         implement(make_assignment_right_hand_side_range(w.declarations())));
    }

    template < typename Container >
    boost::iterator_range< atermpp::detail::transform_iterator< implementor, typename Container::const_iterator, typename Container::value_type > >
    implement(Container const& container, typename atermpp::detail::enable_if_container< Container >::type* = 0)
    {
      typedef atermpp::detail::transform_iterator< implementor, typename Container::const_iterator, typename Container::value_type > iterator_type;

      return boost::make_iterator_range(iterator_type(container.begin(), implementor(this)),
                                        iterator_type(container.end(), implementor(this)));
    }

    data_expression implement(application const& expression)
    {
      return application(implement(expression.head()), implement(expression.arguments()));
    }

    data_expression reconstruct(const application& expression)
    {
      if (is_function_symbol(expression.head()))
      {
        static number_postfix_generator variable_string_generator("x_");
        /* The code below is outcommented. Lambda terms, if they occur in processes
           will be handled properly with the new rewriter. Now application terms
           with "forall" and "exists" as terms can remain in the result. */

        function_symbol head(expression.head());

        if (head.name() == "exists")
        {
          data_expression argument_expression(reconstruct(*expression.arguments().begin()));
          if (!is_abstraction(argument_expression))
          {
            const function_sort s=argument_expression.sort();
            assert(s.codomain()==sort_bool::bool_());
            const sort_expression_list sl=s.domain();

            variable_list vl;
            for(sort_expression_list::const_iterator i=sl.begin(); i!=sl.end(); ++i)
            {
              const variable fresh_variable(variable_string_generator(),*i);
              vl=push_front(vl,fresh_variable);
            }
            vl=reverse(vl);
            mutable_map_substitution<atermpp::map < variable,data_expression> > sigma;
            return exists(vl,reconstruct((data_expression)m_rewriter->rewrite(application(*expression.arguments().begin(),vl),sigma)));
          }

          lambda argument(argument_expression);

          return exists(argument.variables(), argument.body());
        }
        else if (head.name() == "forall")
        {
          data_expression argument_expression(reconstruct(*expression.arguments().begin()));
          if (!is_abstraction(argument_expression))
          {
            const function_sort s=argument_expression.sort();
            assert(s.codomain()==sort_bool::bool_());
            const sort_expression_list sl=s.domain();

            variable_list vl;
            for(sort_expression_list::const_iterator i=sl.begin(); i!=sl.end(); ++i)
            {
              const variable fresh_variable(variable_string_generator(),*i);
              vl=push_front(vl,fresh_variable);
            }
            vl=reverse(vl);
            mutable_map_substitution<atermpp::map < variable,data_expression> > sigma;
            return forall(vl,reconstruct((data_expression)m_rewriter->rewrite(application(*expression.arguments().begin(),vl),sigma)));
          }

          lambda argument(argument_expression);
          return forall(argument.variables(), argument.body());
        }
      }
      return application(reconstruct(data_expression(expression.head())), reconstruct(expression.arguments()));
    }

    template < typename Container >
    boost::iterator_range< atermpp::detail::transform_iterator< reconstructor, typename Container::const_iterator, typename Container::value_type > >
    reconstruct(Container const& container, typename atermpp::detail::enable_if_container< Container >::type* = 0)
    {
      typedef atermpp::detail::transform_iterator< reconstructor, typename Container::const_iterator, typename Container::value_type > iterator_type;

      return boost::make_iterator_range(iterator_type(container.begin(), reconstructor(this)),
                                        iterator_type(container.end(), reconstructor(this)));
    }

    data_expression reconstruct(const data_expression& expression)
    {
      if (is_function_symbol(expression))
      {
        atermpp::map< data_expression, data_expression >::const_iterator i(m_reconstruction_context.find(expression));

        if (i != m_reconstruction_context.end())
        {
          return i->second;
        }
      }
      else if (is_application(expression))
      {
        return reconstruct(application(expression));
      }

      return expression;
    }

    data_expression lazy_reconstruct(const data_expression& expression)
    {
      if (!m_reconstruction_context.empty())
      {
        return reconstruct(expression);
      }
      return expression;
    }

    data_expression implement(const data_expression& expression)
    {
      if (is_application(expression))
      {
        return implement(application(expression));
      }
      else if (is_variable(expression))
      {
        return implement(variable(expression));
      }
      else if (is_function_symbol(expression))
      {
        return implement(function_symbol(expression));
      }
      else if (is_abstraction(expression))
      {
        return implement(abstraction(expression));
      }
      else if (is_where_clause(expression))
      {
        return implement(where_clause(expression));
      }

      return expression;
    }

    rewrite_conversion_helper(Rewriter& rewriter) :
      m_rewriter(&rewriter)
    {
    }
};
} // namespace detail
} // namespace data
} // namespace mcrl2

#endif
