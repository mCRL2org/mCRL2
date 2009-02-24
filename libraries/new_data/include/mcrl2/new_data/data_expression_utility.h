// Author(s): Jeroen Keiren
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
#include <set>
#include "boost/format.hpp"

#include "mcrl2/atermpp/set.h"
#include "mcrl2/atermpp/utility.h"
#include "mcrl2/core/find.h"
#include "mcrl2/core/detail/join.h"
#include "mcrl2/new_data/data_expression.h"
#include "mcrl2/new_data/sort_expression.h"
#include "mcrl2/new_data/function_sort.h"
#include "mcrl2/new_data/application.h"
#include "mcrl2/new_data/variable.h"
#include "mcrl2/new_data/bool.h"

namespace mcrl2 {

  namespace new_data {

    template <typename Term>
    atermpp::vector<Term> term_vector_difference(atermpp::vector<Term> v, atermpp::vector<Term> w)
    {
      if (w.empty())
      {
        return v;
      }
      if (v.empty())
      {
        return w;
      }
      std::set<Term> result;
      result.insert(v.begin(), v.end());
      for (typename atermpp::vector<Term>::iterator i = w.begin(); i != w.end(); ++i)
      {
        result.erase(*i);
      }
      return atermpp::vector<Term>(result.begin(), result.end());
    }

    /// \brief Returns the sorts of the new_data expressions in the input
    /// \param[in] l A range of new_data expressions
    /// \ret The sorts of the new_data expressions in l.
    template <typename T>
    sort_expression_list sorts_of_data_expressions(const boost::iterator_range<T>& l)
    {
      sort_expression_list result;

      for(T i = l.begin(); i != l.end(); ++i)
      {
        std::cerr << i->sort() << std::endl;
        result.push_back(i->sort());
      }

      return result;
    }

    /// Fresh variable generator that generates new_data variables with
    /// names that do not appear in the given context.
    class fresh_variable_generator
    {
      protected:
        atermpp::set<core::identifier_string> m_identifiers;
        sort_expression m_sort;                    // used for operator()()
        std::string m_hint;                  // used as a hint for operator()()

      public:
        /// Constructor.
        ///
        fresh_variable_generator()
         : m_sort(sort_bool_::bool_()), m_hint("t")
        { }

        /// Constructor.
        ///
        template <typename Term>
        fresh_variable_generator(Term context, sort_expression s = sort_bool_::bool_(), std::string hint = "t")
        {
          m_identifiers = core::find_identifiers(context);
          m_hint = hint;
          m_sort = s;
        }

        /// Set a new hint.
        ///
        void set_hint(std::string hint)
        {
          m_hint = hint;
        }

        /// Returns the current hint.
        ///
        std::string hint() const
        {
          return m_hint;
        }

        /// Set a new context.
        ///
        template <typename Term>
        void set_context(Term context)
        {
          m_identifiers = core::find_identifiers(context);
        }

        /// Set a new sort.
        ///
        void set_sort(sort_expression s)
        {
          m_sort = s;
        }

        /// Returns the current sort.
        ///
        sort_expression sort() const
        {
          return m_sort;
        }

        /// Add term t to the context.
        ///
        template <typename Term>
        void add_to_context(Term t)
        {
          std::set<core::identifier_string> ids = core::find_identifiers(t);
          std::copy(ids.begin(), ids.end(), std::inserter(m_identifiers, m_identifiers.end()));
        }

        /// Returns a unique variable of the given sort, with the given hint as prefix.
        /// The returned variable is added to the context.
        ///
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

        /// Returns a unique variable with the same sort as the variable v, and with
        /// the same prefix. The returned variable is added to the context.
        ///
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
    inline data_expression apply_function_symbol_to_variables(const function_symbol& f, variable_list& context, variable_list& used_context)
    {
      if(f.sort().is_function_sort())
      {
        fresh_variable_generator generator(context, sort_bool_::bool_(), "x");
        variable_list tmp_context = context; // Use vars only once
        function_sort f_sort = static_cast<const function_sort&>(f.sort());
        variable_list arguments;
        sort_expression_list f_domain(f_sort.domain().begin(), f_sort.domain().end());
        for(sort_expression_list::const_iterator i = f_domain.begin(); i != f_domain.end(); ++i)
        {
          variable v;
          generator.set_sort(*i);
          for(variable_list::iterator j = tmp_context.begin(); j != tmp_context.end() && v == variable(); ++j)
          {
            if(j->sort() == *i)
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
      inline data_expression and_(data_expression const& p, data_expression const& q)
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
      inline data_expression or_(data_expression const& p, data_expression const& q)
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
          return p;
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
      template <typename FwdIt>
      data_expression join_or(FwdIt first, FwdIt last)
      {
        return core::detail::join(first, last, lazy::or_, sort_bool_::false_());
      }

      /// \brief Returns and applied to the sequence of data expressions [first, last)
      /// \param first Start of a sequence of data expressions
      /// \param last End of a sequence of data expressions
      /// \return And applied to the sequence of data expressions [first, last)
      template <typename FwdIt>
      data_expression join_and(FwdIt first, FwdIt last)
      {
        return core::detail::join(first, last, lazy::and_, sort_bool_::true_());
      }
    }

  } // namespace new_data

} // namespace mcrl2

#endif //MCRL2_NEW_DATA_DATA_EXPRESSION_UTILITY_H

