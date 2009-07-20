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

#include "boost/format.hpp"
#include "boost/iterator/transform_iterator.hpp"

#include "mcrl2/data/assignment.h"
#include "mcrl2/data/detail/data_functional.h"
#include "mcrl2/data/detail/container_utility.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/core/find.h"

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
    make_sort_range(Container const& container, typename boost::enable_if< typename detail::is_container< Container >::type >::type* = 0)
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
    template < typename Container >
    inline
    Container fresh_variables(boost::iterator_range< typename Container::const_iterator > const& t, const std::set<std::string>& context, std::string postfix_format = "_%02d")
    {
      std::vector<std::string> ids(boost::make_transform_iterator(t.begin(), detail::variable_name()),
                                   boost::make_transform_iterator(t.end(), detail::variable_name()));
      std::string postfix;
      for (int i = 0; ; i++)
      {
        postfix = str(boost::format(postfix_format) % i);
        std::vector<std::string>::iterator j = ids.begin();
        for ( ; j != ids.end(); ++j)
        {
          if (context.find(*j + postfix) != context.end())
            break;
        }
        if (j == ids.end()) // success!
          break;
      }
      variable_vector result;
      for (typename Container::const_iterator k = t.begin(); k != t.end(); ++k)
      {
        core::identifier_string name(std::string(k->name()) + postfix);
        result.push_back(variable(name, k->sort()));
      }
      return data::convert< Container >(result);
    }

    /// \brief Returns a copy of t, but with a common postfix added to each variable name,
    /// \overload
    inline
    variable_list fresh_variables(variable_list const& t, const std::set<std::string>& context, std::string postfix_format = "_%02d") {
      return fresh_variables< variable_list >(boost::make_iterator_range(t), context, postfix_format);
    }

    /// \brief Returns a copy of t, but with a common postfix added to each variable name,
    /// \overload
    inline
    variable_vector fresh_variables(variable_vector const& t, const std::set<std::string>& context, std::string postfix_format = "_%02d") {
      return fresh_variables< variable_vector >(boost::make_iterator_range(t), context, postfix_format);
    }

    /// \brief Returns an identifier that doesn't appear in the set <tt>context</tt>
    /// \param context A set of strings
    /// \param hint A string
    /// \param id_creator A function that generates identifiers
    /// \return An identifier that doesn't appear in the set <tt>context</tt>
    template <typename IdentifierCreator>
    inline core::identifier_string fresh_identifier(const std::set<core::identifier_string>& context, const std::string& hint, IdentifierCreator id_creator = IdentifierCreator())
    {
      int index = 0;
      core::identifier_string s;
      do
      {
        s = core::identifier_string(id_creator(hint, index++));
      }
      while(context.find(s) != context.end());
      return s;
    }

    /// \brief Returns an identifier that doesn't appear in the set <tt>context</tt>
    /// \param context A sequence of sort expressions
    /// \param hint A string
    /// \param id_creator A function that generates identifiers
    /// \return An identifier that doesn't appear in the set <tt>context</tt>
    template <typename ForwardTraversalIterator, typename IdentifierCreator>
    inline core::identifier_string fresh_identifier(const boost::iterator_range< ForwardTraversalIterator >& context, const std::string& hint, IdentifierCreator id_creator = IdentifierCreator())
    {
      std::set<core::identifier_string> s;

      for (ForwardTraversalIterator i = context.begin(); i != context.end(); ++i)
      {
        if (is_alias(*i))
        {
          s.insert(alias(*i).name().name());
        }
        else if (i->is_basic_sort())
        {
          s.insert(basic_sort(*i).name());
        }
      }

      return fresh_identifier(s, hint, id_creator);
    }

    /// \brief Returns an identifier that doesn't appear in the term context
    /// \param context A term
    /// \param hint A string
    /// \param id_creator A function that generates identifiers
    /// \return An identifier that doesn't appear in the term context
    template <typename Term, class IdentifierCreator>
    core::identifier_string fresh_identifier(Term const& context, const std::string& hint, IdentifierCreator id_creator = IdentifierCreator())
    {
      return fresh_identifier(core::find_identifiers(context), hint, id_creator);
    }

    /// \brief Creates an identifier built from name and index.
    struct default_identifier_creator
    {
      /// \brief Constructor.
      /// \param name A string
      /// \param index A positive number.
      /// \return An identifier.
      std::string operator()(const std::string& name, int index) const
      {
        if (index <= 0)
          return name;
        return str(boost::format(name + "%02d") % index++);
      }
    };

    /// \brief Returns an identifier that doesn't appear in the term context
    /// \param context A term
    /// \param hint A string
    /// \return An identifier that doesn't appear in the term context
    template <typename Term>
    core::identifier_string fresh_identifier(const Term& context, const std::string& hint)
    {
      return fresh_identifier(context, hint, default_identifier_creator());
    }

    /// \brief Returns a variable that doesn't appear in context
    /// \param context A term
    /// \param s A sort expression
    /// \param hint A string
    /// \return A variable that doesn't appear in context
    template <typename Term>
    variable fresh_variable(Term const& context, sort_expression s, std::string const& hint)
    {
      return variable(fresh_identifier(context, hint), s);
    }

  } // namespace data

} // namespace mcrl2

#endif //MCRL2_DATA_UTILITY_H

