// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/utility.h
/// \brief Utility functions of the data library.

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
#include "mcrl2/core/find.h"
#include "mcrl2/data/data.h"
#include "mcrl2/data/detail/data_utility.h"
#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/set.h"
#include "mcrl2/atermpp/utility.h"

namespace mcrl2 {

namespace data {

/// \brief Returns a copy of t, but with a common postfix added to each variable name,
/// and such that the new names do not appear in context.
/// \param t A sequence of data variables
/// \param context A set of strings
/// \param postfix_format A string
/// \return A sequence of variables with names that do not appear in \p context. The
/// string \p postfix_format is used to generate new names. It should contain one
/// occurrence of "%d", that will be replaced with an integer.
inline
data_variable_list fresh_variables(data_variable_list t, const std::set<std::string>& context, std::string postfix_format = "_%02d")
{
  std::vector<std::string> ids = detail::variable_strings(t);
  std::string postfix;
  for (int i = 0; ; i++)
  {
    postfix = str(boost::format(postfix_format) % i);
    std::vector<std::string>::iterator j = ids.begin();
    for ( ; j != ids.end(); j++)
    {
      if (context.find(*j + postfix) != context.end())
        break;
    }
    if (j == ids.end()) // success!
      break;
  }
  data_variable_list result;
  for (data_variable_list::iterator k = t.begin(); k != t.end(); ++k)
  {
    core::identifier_string name(std::string(k->name()) + postfix);
    result = push_front(result, data_variable(name, k->sort()));
  }
  return atermpp::reverse(result);
}

/// \brief Returns an identifier that doesn't appear in the set <tt>context</tt>
/// \param context A set of strings
/// \param hint A string
/// \param id_creator A function that generates identifiers
/// \return An identifier that doesn't appear in the set <tt>context</tt>
template <typename IdentifierCreator>
core::identifier_string fresh_identifier(const std::set<core::identifier_string>& context, const std::string& hint, IdentifierCreator id_creator = IdentifierCreator())
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
data_variable fresh_variable(Term context, sort_expression s, std::string hint)
{
  core::identifier_string id = fresh_identifier(context, hint);
  return data_variable(id, s);
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
     : m_sort(sort_expr::real()), m_hint("t")
    { }

    /// \brief Constructor.
    /// \param context A term
    /// \param s A sort expression
    /// \param hint A string
    template <typename Term>
    fresh_variable_generator(Term context, sort_expression s = sort_expr::real(), std::string hint = "t")
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
    data_variable operator()()
    {
      core::identifier_string id(m_hint);
      int index = 0;
      while (m_identifiers.find(id) != m_identifiers.end())
      {
        std::string name = str(boost::format(m_hint + "%02d") % index++);
        id = core::identifier_string(name);
      }
      m_identifiers.insert(id);
      return data_variable(id, m_sort);
    }

    /// \brief Returns a unique variable with the same sort as the variable v, and with
    /// the same prefix. The returned variable is added to the context.
    /// \param v A data variable
    /// \return A fresh variable with the same sort as the given variable, and with the name of
    /// the variable as prefix.
    data_variable operator()(data_variable v)
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
      return data_variable(id, v.sort());
    }
};

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_UTILITY_H
