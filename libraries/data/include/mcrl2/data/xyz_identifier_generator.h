// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/xyz_identifier_generator.h
/// \brief Class that generates identifiers in the range X, Y, Z, X0, Y0, Z0, X1, ...

#ifndef MCRL2_DATA_XYZ_IDENTIFIER_GENERATOR_H
#define MCRL2_DATA_XYZ_IDENTIFIER_GENERATOR_H

#include "mcrl2/data/set_identifier_generator.h"

namespace mcrl2
{

namespace data
{

/// \brief Identifier generator that generates names from the range X, Y, Z, X0, Y0, Z0, X1, ...
class xyz_identifier_generator: public multiset_identifier_generator
{
  protected:
    /// \brief The index of the last generated identifier.
    int m_index;

    /// \brief The character of the last generated identifier.
    char m_char; // character of last generated identifier

    /// \brief Returns the next name in the range X, Y, Z, X0, Y0, Z0, X1, ...
    /// \return The next name in the range X, Y, Z, X0, Y0, Z0, X1, ...
    std::string next()
    {
      switch (m_char)
      {
        case 'X' :
        {
          m_char = 'Y';
          break;
        }
        case 'Y' :
        {
          m_char = 'Z';
          break;
        }
        case 'Z' :
        {
          m_char = 'X';
          m_index++;
          break;
        }
      }
      return m_index < 0 ? std::string(1, m_char) : std::string(1, m_char) + std::to_string(m_index);
    }

  public:
    /// \brief Constructor.
    xyz_identifier_generator()
      : m_index(-2), m_char('Z')
    {}

    /// \brief Constructor.
    /// \param ids a set of identifiers to use as context
   xyz_identifier_generator(const std::set<core::identifier_string>& ids)
      : m_index(-2), m_char('Z')
    {
      add_identifiers(ids);
    }

    /// \brief Returns hint if it isn't in the context yet. Else the next available
    /// identifier in the range X, Y, Z, X0, Y0, Z0, X1, ... is returned.
    /// \param hint A string
    /// \param add_to_context If true, the generated identifier is added to the context,
    /// it is not added otherwise.
    /// \return A fresh identifier.
    core::identifier_string operator()(const std::string& hint, bool add_to_context = true)
    {
      core::identifier_string result(hint);

      if (m_identifiers.find(hint) != m_identifiers.end())
      {
        m_index = -2;
        m_char = 'Z';
        do
        {
          result = core::identifier_string(next());
        }
        while (m_identifiers.find(result) != m_identifiers.end());
      }

      if (add_to_context)
      {
        add_identifier(result);
      }
      return result;
    }
};

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_XYZ_IDENTIFIER_GENERATOR_H
