// Author(s): Jeroen Keiren, Jeroen van der Wulp, Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/new_data/data_expression_utility.h
/// \brief Provides utilities for working with new_data expression.

#ifndef MCRL2_NEW_DATA_DATA_EXPRESSION_UTILITY_H
#define MCRL2_NEW_DATA_DATA_EXPRESSION_UTILITY_H

#include <string>
#include <vector>
#include <set>

#include "boost/format.hpp"
#include "boost/assert.hpp"
#include "boost/iterator/transform_iterator.hpp"

#include "mcrl2/atermpp/set.h"
#include "mcrl2/atermpp/utility.h"
#include "mcrl2/core/find.h"
#include "mcrl2/core/detail/join.h"

#include "mcrl2/new_data/data_expression.h"
#include "mcrl2/new_data/sort_expression.h"
#include "mcrl2/new_data/function_sort.h"
#include "mcrl2/new_data/function_symbol.h"
#include "mcrl2/new_data/application.h"
#include "mcrl2/new_data/variable.h"
#include "mcrl2/new_data/bool.h"

namespace mcrl2 {

  namespace new_data {

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

        /// \brief A sort for the generated variables.
        sort_expression m_sort;

        /// \brief A hint for the name of generated variables.
        std::string m_hint;

      public:
        /// \brief Constructor.
        fresh_variable_generator()
         : m_sort(sort_bool_::bool_()), m_hint("t")
        { }

        /// \brief Constructor.
        /// \param context A term
        /// \param s A sort expression
        /// \param hint A string
        template <typename Term>
        fresh_variable_generator(Term context, sort_expression s = sort_bool_::bool_(), std::string hint = "t")
        {
          m_identifiers = core::find_identifiers(context);
          m_hint = hint;
          m_sort = s;
        }

        /// \brief Set a new hint.
        /// \param hint A string
        void set_hint(std::string hint)
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
        /// \param context A term
        template <typename Term>
        void set_context(Term context)
        {
          m_identifiers = core::find_identifiers(context);
        }

        /// \brief Set a new sort.
        /// \param s A sort expression
        void set_sort(sort_expression s)
        {
          m_sort = s;
        }

        /// \brief Returns the current sort.
        /// \return The current sort.
        sort_expression sort() const
        {
          return m_sort;
        }

        /// \brief Add term t to the context.
        /// \param t A term
        template <typename Term>
        void add_to_context(Term t)
        {
          std::set<core::identifier_string> ids = core::find_identifiers(t);
          std::copy(ids.begin(), ids.end(), std::inserter(m_identifiers, m_identifiers.end()));
        }

        /// \brief Returns a unique variable of the given sort, with the given hint as prefix.
        /// The returned variable is added to the context.
        /// \return A fresh variable that does not appear in the current context.
        variable operator()()
        {
          core::identifier_string id(m_hint);
          int index = 0;
          while (m_identifiers.find(id) != m_identifiers.end())
          {
            std::string name = str(boost::format(m_hint + "%02d") % index++);
            id = core::identifier_string(name);
          }
          m_identifiers.insert(id);
          return variable(id, m_sort);
        }

        /// \brief Returns a unique variable with the same sort as the variable v, and with
        /// the same prefix. The returned variable is added to the context.
        /// \param v A data variable
        /// \return A fresh variable with the same sort as the given variable, and with the name of
        /// the variable as prefix.
        variable operator()(variable v)
        {
          std::string hint = v.name();
          core::identifier_string id(hint);
          int index = 0;
          while (m_identifiers.find(id) != m_identifiers.end())
          {
            std::string name = str(boost::format(hint + "%02d") % index++);
            id = core::identifier_string(name);
          }
          m_identifiers.insert(id);
          return variable(id, v.sort());
        }
    };


    /// \brief Returns the application of f to a sufficient number of variables,
    ///        taking variables from context if possible, otherwise extending
    ///        the context.
    inline data_expression apply_function_symbol_to_variables(const function_symbol& f, variable_vector& context, variable_vector& used_context)
    {
      if(f.sort().is_function_sort())
      {
        fresh_variable_generator generator(context, sort_bool_::bool_(), "x");
        variable_vector tmp_context = context; // Use vars only once
        function_sort f_sort = static_cast<const function_sort&>(f.sort());
        variable_vector arguments;
        for(function_sort::domain_const_range i(f_sort.domain()); !i.empty(); i.advance_begin(1))
        {
          variable v;
          generator.set_sort(i.front());
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
            v = generator(); // Make fresh
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

        return new_data::equal_to(p, q);
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

        return new_data::not_equal_to(p, q);
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

  } // namespace new_data

} // namespace mcrl2

#endif //MCRL2_NEW_DATA_DATA_EXPRESSION_UTILITY_H

