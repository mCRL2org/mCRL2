// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/postfix_identifier_generator.h
/// \brief The class postfix_identifier_generator.

#ifndef MCRL2_DATA_POSTFIX_IDENTIFIER_GENERATOR_H
#define MCRL2_DATA_POSTFIX_IDENTIFIER_GENERATOR_H

#include "mcrl2/data/set_identifier_generator.h"

namespace mcrl2 {

namespace data {

/// \brief Identifier generator that uses a given postfix as a hint for generating identifiers.
class postfix_identifier_generator: public set_identifier_generator
{
  protected:

    number_postfix_generator m_generator;

  public:
    /// \brief Constructor.
    /// \param postfix A string
    postfix_identifier_generator(std::string const& postfix)
      : m_generator(postfix)
    {}

    core::identifier_string operator()(const std::string& hint)
    {
      core::identifier_string id(hint + std::string(m_generator()));

      if (has_identifier(id))
      {
        while (has_identifier(id))
        {
          id = core::identifier_string(hint + std::string(m_generator()));
        }
      }

      add_to_context(id);
      return id;
    }
};

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_POSTFIX_IDENTIFIER_GENERATOR_H
