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

#include "boost/assert.hpp"

#include "mcrl2/atermpp/set.h"
#include "mcrl2/atermpp/utility.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/data/variable.h"

namespace mcrl2
{

namespace data
{

class number_postfix_generator;

/// \brief Variable generator that generates data variables with names that do not appear in a given context.
template <typename IdentifierGenerator = number_postfix_generator>
class fresh_variable_generator
{
  protected:
    /// \brief The identifiers of the context.
    atermpp::set<core::identifier_string> m_identifiers;

    /// \brief A hint for the name of generated variables.
    IdentifierGenerator                   m_generator;

    std::string generate(core::identifier_string const& hint)
    {
      core::identifier_string new_name = hint;

      if (m_identifiers.find(new_name) != m_identifiers.end())
      {
        IdentifierGenerator generator(hint);

        while (m_identifiers.find(new_name) != m_identifiers.end())
        {
          new_name = generator();
        }
      }

      m_identifiers.insert(new_name);

      return new_name;
    }

    core::identifier_string generate()
    {
      core::identifier_string new_name = m_generator();

      if (m_identifiers.find(new_name) != m_identifiers.end())
      {
        while (m_identifiers.find(new_name) != m_identifiers.end())
        {
          new_name = m_generator();
        }
      }

      m_identifiers.insert(new_name);

      return new_name;
    }

  public:

    /// \brief Constructor.
    fresh_variable_generator(std::string const& hint = "") : m_generator(hint)
    { }

    /// \brief Set a new hint.
    /// \param hint A string
    void set_hint(std::string const& hint)
    {
      m_generator = IdentifierGenerator(hint);
    }

    /// \brief Add an identifier to the context.
    void add_identifiers(const core::identifier_string& id)
    {
      m_identifiers.insert(id);
    }

    /// \brief Add a set of identifiers to the context.
    void add_identifiers(const std::set<core::identifier_string>& ids)
    {
      m_identifiers.insert(ids.begin(), ids.end());
    }

    /// \brief Returns a unique variable of the given sort, with the given hint as prefix.
    /// The returned variable is added to the context.
    /// \return A fresh variable that does not appear in the current context.
    variable operator()(sort_expression const& s)
    {
      return variable(generate(), s);
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

