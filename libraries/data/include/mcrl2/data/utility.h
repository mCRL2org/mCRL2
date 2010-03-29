// Author(s): Jeroen Keiren, Jeroen van der Wulp, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/utility.h
/// \brief Provides utilities for working with lists.

#ifndef MCRL2_DATA_UTILITY_H
#define MCRL2_DATA_UTILITY_H

#include <algorithm>
#include <functional>
#include <iterator>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "boost/iterator/transform_iterator.hpp"

#include "mcrl2/data/assignment.h"
#include "mcrl2/data/detail/data_functional.h"
#include "mcrl2/atermpp/container_utility.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/set_identifier_generator.h"

namespace mcrl2 {

  namespace data {

    /// \cond INTERNAL_DOCS
    namespace detail {
      template < typename Container >
      struct sort_range
      {
        typedef boost::iterator_range< boost::transform_iterator<
           detail::sort_of_expression< typename Container::value_type >, typename Container::const_iterator > > type;
      };

      class rename_with_unique_common_suffix : public std::unary_function< variable, variable > {

        private:

          std::string m_suffix;

        public:

          variable operator()(variable const& v) const
          {
            return variable(core::identifier_string(std::string(v.name()) + m_suffix), v.sort());
          }

          rename_with_unique_common_suffix()
          { }

          template < typename Container, typename Context >
          rename_with_unique_common_suffix(Container const& c, const Context& context)
          {
            number_postfix_generator generator;

            m_suffix = generator();

            for (typename Container::const_iterator i = c.begin(); i != c.end(); ++i)
            {
              context.find(core::identifier_string(std::string(i->name()) + m_suffix)) != context.end();
            }
          }
      };

      template < typename Container, typename AdaptableUnaryFunction >
      struct fresh_variable_range
      {
        typedef boost::iterator_range< boost::transform_iterator<
           AdaptableUnaryFunction, typename Container::const_iterator > > type;
      };
    }
    /// \endcond

    /// \brief Gives a sequence of sorts for a given sequence of expressions
    /// 
    /// A sequence is a container (as per the STL Container concept) or
    /// iterator range type. The template class / is_container governs what is
    /// recognised as a container.
    //
    /// \param[in] r range of iterators that refer expression objects
    /// \return s1, s2, ..., s3 such that sn = t.front().sort() where t = r.advance_begin(n)
    /// \note Behaviour is lazy, no intermediate containers are constructed
    template < typename Container >
    typename detail::sort_range< Container >::type
    make_sort_range(Container const& container, typename boost::enable_if< typename atermpp::detail::is_container< Container >::type >::type* = 0)
    {
      return typename detail::sort_range< Container >::type(container);
    }

    /// \brief Returns a copy of t, but with a common postfix added to each variable name,
    /// and such that the new names do not appear in context.
    /// \param t A sequence of data variables
    /// \param context A set of strings
    /// \param postfix_format A string
    /// \return A sequence of variables with names that do not appear in \p context. The
    /// string \p postfix_format is used to generate new names. It should contain one
    /// occurrence of "%d", that will be replaced with an integer.
    template < typename Container, typename Context >
    inline
    typename detail::fresh_variable_range< Container, detail::rename_with_unique_common_suffix >::type
    fresh_variables(Container const& container, const Context& context, typename boost::enable_if< typename atermpp::detail::is_container< Container, variable >::type >::type* = 0)
    {
      typedef boost::transform_iterator< detail::rename_with_unique_common_suffix, typename Container::const_iterator > iterator_type;

      return typename detail::fresh_variable_range< Container, detail::rename_with_unique_common_suffix >::type(
        iterator_type(container.begin(), detail::rename_with_unique_common_suffix(container, context)),
        iterator_type(container.end()));
    }

    /// \brief Returns an identifier that doesn't appear in the set <tt>context</tt>
    /// \param context A set of strings
    /// \param hint A string
    /// \param id_creator A function that generates identifiers
    /// \return An identifier that doesn't appear in the set <tt>context</tt>
    /// \warning reorganising the identifier context is expensive, consider using an identifier generator
    template < typename Context >
    inline core::identifier_string fresh_identifier(const Context& context, const std::string& hint)
    {
      set_identifier_generator generator(context);

      return generator(hint);
    }

    /// \brief Returns a variable that doesn't appear in context
    /// \param context A term
    /// \param s A sort expression
    /// \param hint A string
    /// \return A variable that doesn't appear in context
    /// \warning reorganising the identifier context is expensive, consider using an identifier generator
    template < typename Context >
    variable fresh_variable(Context const& context, sort_expression const& s, std::string const& hint)
    {
      return variable(fresh_identifier(context, hint), s);
    }

  } // namespace data

} // namespace mcrl2

#endif //MCRL2_DATA_UTILITY_H

