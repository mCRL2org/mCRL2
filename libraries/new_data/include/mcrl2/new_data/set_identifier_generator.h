// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/new_data/set_identifier_generator.h
/// \brief The classes set_identifier_generator and multiset_identifier_generator.

#ifndef MCRL2_NEW_DATA_SET_IDENTIFIER_GENERATOR_H
#define MCRL2_NEW_DATA_SET_IDENTIFIER_GENERATOR_H

#include "mcrl2/atermpp/set.h"
#include "mcrl2/new_data/identifier_generator.h"

namespace mcrl2 {

namespace new_data {

/// A class that generates identifiers. A context is maintained
/// containing already used identifiers. The context is stored
/// in a set.
///
/// Using the operator()() and operator()(std::string) fresh
/// identifiers can be generated that do not appear in the
/// context.
class set_identifier_generator: public identifier_generator
{
  protected:
    atermpp::set<core::identifier_string> m_identifiers;

  public:
    /// Constructor.
    ///
    set_identifier_generator()
    {}

    /// Constructor.
    ///
    template <typename Term>
    set_identifier_generator(Term t)
    {
      add_to_context(t);
    }

    /// Clears the context.
    void clear_context()
    {
      m_identifiers.clear();
    }

    /// Adds the identifier s to the context.
    void add_identifier(core::identifier_string s)
    {
      m_identifiers.insert(s);
    }

    /// Removes one occurrence of the identifier s from the context.
    void remove_identifier(core::identifier_string s)
    {
      m_identifiers.erase(s);
    }

    /// Returns true if the identifier s appears in the context.
    bool has_identifier(core::identifier_string s) const
    {
      return m_identifiers.find(s) != m_identifiers.end();
    }

    /// Returns the context.
    const atermpp::set<core::identifier_string>& context() const
    {
      return m_identifiers;
    }
};

/// A class that generates identifiers. A context is maintained
/// containing already used identifiers. The context is stored
/// in a multiset. If an identifier occurs multiple times,
/// multiple calls to remove_from_context are required to
/// remove it.
///
/// Using the operator()() and operator()(std::string) fresh
/// identifiers can be generated that do not appear in the
/// context.
class multiset_identifier_generator: public identifier_generator
{
  protected:
    atermpp::multiset<core::identifier_string> m_identifiers;

  public:
    /// Constructor.
    ///
    multiset_identifier_generator()
    {}

    /// Constructor.
    ///
    template <typename Term>
    multiset_identifier_generator(Term t)
    {
      add_to_context(t);
    }

    /// Clears the context.
    void clear_context()
    {
      m_identifiers.clear();
    }

    /// Adds the identifier s to the context.
    void add_identifier(core::identifier_string s)
    {
      m_identifiers.insert(s);
    }

    /// Removes one occurrence of the identifier s from the context.
    void remove_identifier(core::identifier_string s)
    {
      atermpp::multiset<core::identifier_string>::iterator i = m_identifiers.find(s);
      if (i != m_identifiers.end())
      {
        m_identifiers.erase(i);
      }
    }

    /// Returns true if the identifier s appears in the context.
    bool has_identifier(core::identifier_string s) const
    {
      return m_identifiers.find(s) != m_identifiers.end();
    }

    /// Returns the context.
    const atermpp::multiset<core::identifier_string>& context() const
    {
      return m_identifiers;
    }
};

} // namespace new_data

} // namespace mcrl2

#endif // MCRL2_NEW_DATA_SET_IDENTIFIER_GENERATOR_H
