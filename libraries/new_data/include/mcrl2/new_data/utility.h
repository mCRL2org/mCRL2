// Author(s): Jeroen Keiren, Jeroen van der Wulp, Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/new_data/utility.h
/// \brief Provides utilities for working with lists.

#ifndef MCRL2_NEW_DATA_UTILITY_H
#define MCRL2_NEW_DATA_UTILITY_H

#include <algorithm>
#include <functional>
#include <iterator>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "boost/format.hpp"
#include "boost/utility/enable_if.hpp"

#include "mcrl2/new_data/assignment.h"
#include "mcrl2/new_data/detail/data_functional.h"
#include "mcrl2/new_data/detail/container_utility.h"
#include "mcrl2/new_data/detail/data_specification_compatibility.h"
#include "mcrl2/core/print.h"
#include "mcrl2/core/find.h"

namespace mcrl2 {

  namespace new_data {

    /// \brief Applies a function to an expression
    /// \param f a function to apply to the expression
    /// \param e the expression on which to apply the assignment
    /// \return The application of the assignment to the term.
    /// \deprecated
    template < typename Expression, typename SubstitutionFunction >
    inline Expression substitute(SubstitutionFunction const& f, Expression const& e)
    {
      return static_cast< Expression >(f(static_cast< atermpp::aterm_appl const& >(e)));
    }

    /// \brief Applies a function to an expression
    /// \param c an assignment to apply to the expression
    /// \param e the expression on which to apply the assignment
    /// \return The application of the assignment to the term.
    template < >
    inline data_expression substitute(assignment const& c, data_expression const& e)
    {
      return atermpp::replace(e, atermpp::aterm(c.lhs()), atermpp::aterm(c.rhs()));
    }

    /// \brief Applies a substitution function to all elements of a container
    /// \param[in] f substitution function
    /// \param[in,out] c applies substitution function on elements of container
    template < typename Expression, typename SubstitutionFunction >
    atermpp::vector< Expression >& substitute(SubstitutionFunction const& f, atermpp::vector< Expression >& c)
    {
      for (typename atermpp::vector< Expression >::iterator i = c.begin(); i != c.end(); ++i)
      {
        (*i) = substitute(f, *i);
      }

      return c;
    }

    /// \brief Applies a substitution function to all elements of a container
    /// \param[in] f substitution function
    /// \param[in,out] c applies substitution function on elements of container
    template < typename Expression, typename SubstitutionFunction >
    atermpp::term_list< Expression > substitute(SubstitutionFunction const& f, atermpp::term_list< Expression > c)
    {
      atermpp::vector< Expression > result;

      return convert< atermpp::term_list< Expression > >(substitute(f, result));
    }

    /// \brief Applies a substitution function to all elements of a container
    /// \param[in] f substitution function
    /// \param[in] c applies substitution function on elements of container
    /// \return a vector of Expressions, such that the result is the vector of
    ///         elements in c with f applied to them.
    template < typename Expression, typename SubstitutionFunction >
    atermpp::vector< Expression > substitute(SubstitutionFunction const& f, atermpp::vector< Expression > const& c)
    {
      atermpp::vector< Expression > result;

      for (typename atermpp::vector< Expression >::const_iterator i = c.begin(); i != c.end(); ++i)
      {
        result.push_back(substitute(f, *i));
      }

      return result;
    }


    /// \brief Applies a substitution function to all elements of a container
    /// \param[in] f substitution function
    /// \param[in] c applies substitution function on elements of container
    /// \param[out] o output iterator to which the elements of c with f applied
    ///             to them are added.
    template < typename Container, typename SubstitutionFunction, typename OutputIterator >
    void substitute(SubstitutionFunction const& f, Container const& c, OutputIterator o)
    {
      for (typename Container::const_iterator i = c.begin(); i != c.end(); ++i, ++o)
      {
        *o = f(*i);
      }
    }

    /// \brief Pretty prints a data specification
    /// \param[in] specification a data specification
    inline std::string pp(data_specification const& specification)
    {
      return core::pp(detail::data_specification_to_aterm_data_spec(specification));
    }

    /// \brief Pretty prints the contents of a container
    /// \param[in] c a container with data or sort expressions
    template < typename Container >
    inline std::string pp(Container const& c, typename boost::enable_if< typename detail::is_container< Container >::type >::type* = 0)
    {
      std::string result;

      if (c.begin() != c.end())
      {
        result.append(mcrl2::core::pp(*c.begin()));

        for (typename Container::const_iterator i = ++(c.begin()); i != c.end(); ++i)
        {
          result.append(", ").append(mcrl2::core::pp(*i));
        }
      }

      return result;
    }

    /// \brief Pretty prints a data and sort expressions
    /// \param[in] c A data or sort expression
    inline std::string pp(atermpp::aterm_appl const& c)
    {
      return core::pp(c);
    }

    /// \brief Pretty prints a data and sort expressions
    /// \param[in] c A data or sort expression.
    template < typename Expression >
    inline std::string pp(atermpp::term_list< Expression > const& c)
    {
      return core::pp(c);
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
      return new_data::convert< Container >(result);
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

      for (std::set<sort_expression>::const_iterator i = context.begin(); i != context.end(); ++i)
      {
        if (i->is_alias())
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
    core::identifier_string fresh_identifier(Term context, const std::string& hint, IdentifierCreator id_creator = IdentifierCreator())
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
    variable fresh_variable(Term context, sort_expression s, std::string hint)
    {
      core::identifier_string id = fresh_identifier(context, hint);
      return variable(id, s);
    }

    /// \brief Combines two variables lists
    /// \param v1 a list of variables
    /// \param v2 a list of variables
    /// \return for all x : x in v1 or x in v2  implies x in result
    template < typename Container >
    inline variable_list merge(Container const& v1, Container const& v2) {
      std::set< typename Container::value_type > variables(v1.begin(), v1.end());

      variables.insert(v2.begin(), v2.end());

      return Container(variables.begin(), variables.end());
    }

  } // namespace new_data

} // namespace mcrl2

#endif //MCRL2_NEW_DATA_UTILITY_H

