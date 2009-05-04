// Author(s): Jeroen Keiren, Jeroen van der Wulp, Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/data_expression_utility.h
/// \brief Provides utilities for working with data expression.

#ifndef MCRL2_DATA_DATA_EXPRESSION_UTILITY_H
#define MCRL2_DATA_DATA_EXPRESSION_UTILITY_H

#include <string>
#include <vector>
#include <set>

#include "boost/assert.hpp"
#include "boost/iterator/transform_iterator.hpp"

#include "mcrl2/atermpp/set.h"
#include "mcrl2/atermpp/utility.h"
#include "mcrl2/core/find.h"
#include "mcrl2/core/detail/join.h"

#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/data/function_sort.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/data/application.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/data/bool.h"

namespace mcrl2 {

  namespace data {

    /// \cond INTERNAL_DOCS
    namespace detail {

      /// \brief Extracts the sort out of objects of type data expression (or derived type)
      template < typename Expression >
      struct sort_of : public std::unary_function< Expression const&, sort_expression > {
        sort_expression operator()(Expression const& d) const {
          return d.sort();
        }
      };

      /// \brief Applies a function to the objects of type data expression (or derived type)
      template < typename Expression >
      struct apply : public std::unary_function< Expression const&, Expression > {
        function_symbol const& m_function;

        Expression operator()(Expression const& d) const {
          return m_function(d);
        }

        apply(function_symbol const& f) : m_function(f) {
        }
      };

    }
    /// \endcond

    /// \brief Returns true if the term t is equal to nil
    inline bool is_nil(atermpp::aterm_appl t)
    {
      return t == core::detail::gsMakeNil();
    }

    /// \brief Variable generator that generates data variables with names that do not appear in a given context.
    class fresh_variable_generator
    {
      protected:
        /// \brief The identifiers of the context.
        atermpp::set<core::identifier_string> m_identifiers;

        /// \brief A hint for the name of generated variables.
        std::string m_hint;

        std::string generate(std::string const& hint)
        {
          std::string new_name(hint);

          while (m_identifiers.find(new_name) != m_identifiers.end())
          {
            bool carry = true;

            for (std::string::reverse_iterator i = new_name.rbegin();
                         carry && (hint.size() < static_cast< size_t >(new_name.rend() - i)); ++i)
            {
              if (*i == '9')
              {
                *i = '0';
              }
              else
              {
               carry = false;

                ++(*i);
              }
            }

            if (carry) {
              new_name.append("0");
            }
          }

          m_identifiers.insert(new_name);

          return new_name;
        }

      public:

        /// \brief Constructor.
        fresh_variable_generator(std::string const& hint = "t") : m_hint(hint)
        { }

        /// \brief Constructor.
        /// \param context data specification that serves as context
        /// \param hint A string
        fresh_variable_generator(data_specification const& context, std::string const& hint = "t") :
          m_identifiers(convert< atermpp::set< core::identifier_string > >(
                core::find_identifiers(convert< atermpp::aterm_list >(context.equations())))),
          m_hint(hint)
        {
        }

        /// \brief Constructor.
        /// \param context container of expressions from which to extract the context
        /// \param hint A string
        template < typename ForwardTraversalIterator >
        fresh_variable_generator(boost::iterator_range< ForwardTraversalIterator > const& context, std::string const& hint = "t") :
          m_hint(hint)
        {
          set_context(context);
        }

        /// \brief Constructor.
        /// \param context container of expressions from which to extract the context
        /// \param hint A string
        fresh_variable_generator(atermpp::aterm_appl const& context, std::string const& hint = "t") :
          m_hint(hint)
        {
          set_context(context);
        }


        /// \brief Set a new hint.
        /// \param hint A string
        void set_hint(std::string const& hint)
        {
          m_hint = hint;
        }

        /// \brief Returns the current hint.
        /// \return The current hint.
        std::string hint() const
        {
          return m_hint;
        }

        /// \brief Set a new context.
        /// \param context a range of expressions that will be traversed to find identifiers
        template < typename ForwardTraversalIterator >
        void set_context(boost::iterator_range< ForwardTraversalIterator > const& context)
        {
          m_identifiers = core::find_identifiers(convert< atermpp::aterm_list >(context));
        }

        /// \brief Set a new context.
        /// \param context a range of expressions that will be traversed to find identifiers
        template < typename Expression >
        void set_context(Expression const& expression)
        {
          m_identifiers = core::find_identifiers(expression);
        }

        /// \brief Add term t to the context.
        /// \param context a range of expressions that will be traversed to find identifiers
        template < typename ForwardTraversalIterator >
        void add_to_context(boost::iterator_range< ForwardTraversalIterator > const& context)
        {
          for (ForwardTraversalIterator i = context.begin(); i != context.end(); ++i)
          {
            std::set<core::identifier_string> ids = core::find_identifiers(*i);

            m_identifiers.insert(ids.begin(), ids.end());
          }
        }

        /// \brief Add term t to the context.
        /// \param context a range of expressions that will be traversed to find identifiers
        template < typename Expression >
        void add_to_context(Expression const& expression)
        {
          std::set<core::identifier_string> ids = core::find_identifiers(expression);

          m_identifiers.insert(ids.begin(), ids.end());
        }

        /// \brief Returns a unique variable of the given sort, with the given hint as prefix.
        /// The returned variable is added to the context.
        /// \return A fresh variable that does not appear in the current context.
        variable operator()(sort_expression const& s)
        {
          return variable(generate(m_hint), s);
        }

        /// \brief Returns a unique variable with the same sort as the variable v, and with
        /// the same prefix. The returned variable is added to the context.
        /// \param v A data variable
        /// \return A fresh variable with the same sort as the given variable, and with the name of
        /// the variable as prefix.
        variable operator()(variable const& v)
        {
          return variable(generate(v.name()), v.sort());
        }
    };


    /// \brief Returns the application of f to a sufficient number of variables,
    ///        taking variables from context if possible, otherwise extending
    ///        the context.
    inline data_expression apply_function_symbol_to_variables(const function_symbol& f, variable_vector& context, variable_vector& used_context)
    {
      if(f.sort().is_function_sort())
      {
        fresh_variable_generator generator(boost::make_iterator_range(context), "x");
        variable_vector tmp_context = context; // Use vars only once
        function_sort f_sort = static_cast<const function_sort&>(f.sort());
        variable_vector arguments;
        for(function_sort::domain_const_range i(f_sort.domain()); !i.empty(); i.advance_begin(1))
        {
          variable v;

          for(variable_vector::iterator j = tmp_context.begin(); j != tmp_context.end() && v == variable(); ++j)
          {
            if(j->sort() == i.front())
            {
              v = *j;
              tmp_context.erase(j);
            }
          }

          // No variable found in context
          if(v == variable())
          {
            v = generator(i.front()); // Make fresh
          }
          arguments.push_back(v);
        }
        used_context = arguments;

        return application(f, boost::make_iterator_range(arguments));
      }
      else
      {
        return f;
      }
    }

    /** \brief A collection of utilities for lazy expression construction
     *
     * The basic idea is to keep expressions that result from application of
     * any of the container operations by applying the usual rules of logic.
     *
     * For example and(true, x) as in `and' applied to `true' and `x' yields x.
     **/
    namespace lazy {
      /// \brief Returns an expression equivalent to not p
      /// \param p A data expression
      /// \return The value <tt>!p</tt>
      inline data_expression not_(data_expression const& p)
      {
        if (p == sort_bool_::true_()) {
          return sort_bool_::false_();
        }
        else if (p == sort_bool_::false_()) {
          return sort_bool_::true_();
        }

        return sort_bool_::not_(p);
      }

      /// \brief Returns an expression equivalent to p and q
      /// \param p A data expression
      /// \param q A data expression
      /// \return The value <tt>p && q</tt>
      inline data_expression or_(data_expression const& p, data_expression const& q)
      {
        if ((p == sort_bool_::true_()) || (q == sort_bool_::true_())) {
          return sort_bool_::true_();
        }
        else if ((p == q) || (p == sort_bool_::false_())) {
          return q;
        }
        else if (q == sort_bool_::false_()) {
          return p;
        }

        return sort_bool_::or_(p, q);
      }

      /// \brief Returns an expression equivalent to p or q
      /// \param p A data expression
      /// \param q A data expression
      /// \return The value p || q
      inline data_expression and_(data_expression const& p, data_expression const& q)
      {
        if ((p == sort_bool_::false_()) || (q == sort_bool_::false_())) {
          return sort_bool_::false_();
        }
        else if ((p == q) || (p == sort_bool_::true_())) {
          return q;
        }
        else if (q == sort_bool_::true_()) {
          return p;
        }

        return sort_bool_::and_(p, q);
      }

      /// \brief Returns an expression equivalent to p implies q
      /// \param p A data expression
      /// \param q A data expression
      /// \return The value p || q
      inline data_expression implies(data_expression const& p, data_expression const& q)
      {
        if ((p == sort_bool_::false_()) || (q == sort_bool_::true_()) || (p == q)) {
          return sort_bool_::true_();
        }
        else if (p == sort_bool_::true_()) {
          return q;
        }
        else if (q == sort_bool_::false_()) {
          return sort_bool_::not_(p);
        }

        return sort_bool_::implies(p, q);
      }

      /// \brief Returns an expression equivalent to p == q
      /// \param p A data expression
      /// \param q A data expression
      /// \return The value p == q
      inline data_expression equal_to(data_expression const& p, data_expression const& q)
      {
        if (p == q) {
          return sort_bool_::true_();
        }

        return data::equal_to(p, q);
      }

      /// \brief Returns an expression equivalent to p == q
      /// \param p A data expression
      /// \param q A data expression
      /// \return The value ! p == q
      inline data_expression not_equal_to(data_expression const& p, data_expression const& q)
      {
        if (p == q) {
          return sort_bool_::false_();
        }

        return data::not_equal_to(p, q);
      }

      /// \brief Returns or applied to the sequence of data expressions [first, last)
      /// \param first Start of a sequence of data expressions
      /// \param last End of a sequence of data expressions
      /// \return Or applied to the sequence of data expressions [first, last)
      template < typename ForwardTraversalIterator >
      data_expression join_or(ForwardTraversalIterator first, ForwardTraversalIterator last)
      {
        return core::detail::join(first, last, lazy::or_, static_cast< sort_expression const& >(sort_bool_::false_()));
      }

      /// \brief Returns and applied to the sequence of data expressions [first, last)
      /// \param first Start of a sequence of data expressions
      /// \param last End of a sequence of data expressions
      /// \return And applied to the sequence of data expressions [first, last)
      template < typename ForwardTraversalIterator >
      data_expression join_and(ForwardTraversalIterator first, ForwardTraversalIterator last)
      {
        return core::detail::join(first, last, lazy::and_, static_cast< sort_expression const& >(sort_bool_::true_()));
      }
    }

  } // namespace data

} // namespace mcrl2

#endif //MCRL2_DATA_DATA_EXPRESSION_UTILITY_H

