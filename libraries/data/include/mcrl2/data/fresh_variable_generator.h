// Author(s): Jeroen Keiren, Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/free_variable_generator.h
/// \brief Provides utilities for working with data expression.

#ifndef MCRL2_DATA_FRESH_VARIABLE_GENERATOR_H
#define MCRL2_DATA_FRESH_VARIABLE_GENERATOR_H

#include <string>
#include <vector>
#include <set>

#include "boost/assert.hpp"

#include "mcrl2/atermpp/set.h"
#include "mcrl2/atermpp/utility.h"
#include "mcrl2/core/find.h"

#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/data/function_sort.h"
#include "mcrl2/data/function_symbol.h"
#include "mcrl2/data/application.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/data/bool.h"

namespace mcrl2 {

  namespace data {

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

  } // namespace data

} // namespace mcrl2

#endif //MCRL2_DATA_DATA_EXPRESSION_UTILITY_H

