// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/new_data/identifier_generator.h
/// \brief The class identifier_generator.

#ifndef MCRL2_NEW_DATA_IDENTIFIER_GENERATOR_H
#define MCRL2_NEW_DATA_IDENTIFIER_GENERATOR_H

#include <set>
#include <string>
#include <sstream>
#include <boost/format.hpp>
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/core/find.h"

namespace mcrl2 {

namespace new_data {

/// A class that generates identifiers. A context is maintained
/// containing already used identifiers. This class is useful for
/// algorithms that have to avoid name clashes.
///
/// Using the operator()() and operator()(std::string) fresh
/// identifiers are generated that do not appear in the context.
class identifier_generator
{
  public:
    /// Constructor.
    ///
    identifier_generator()
    {}

    /// Destructor.
    ///
    virtual ~identifier_generator()
    {}

    /// Clears the context.
    ///
    virtual void clear_context() = 0;

    /// Adds the identifier s to the context.
    ///
    virtual void add_identifier(core::identifier_string s) = 0;

    /// Removes the identifier s from the context.
    ///
    virtual void remove_identifier(core::identifier_string s) = 0;

    /// Adds identifiers of term t to the context.
    ///
    template <typename Term>
    void add_to_context(Term t)
    {
      std::set<core::identifier_string> s = core::find_identifiers(t);
      for (std::set<core::identifier_string>::iterator i = s.begin(); i != s.end(); ++i)
      {
        add_identifier(*i);
      }
    }

    /// Adds identifiers of term t to the context.
    ///
    template <typename Container>
    void add_identifiers(const Container& c)
    {
      for (typename Container::const_iterator i = c.begin(); i != c.end(); ++i)
      {
        add_identifier(*i);
      }
    }

    /// Removes identifiers of term t from the context.
    ///
    template <typename Term>
    void remove_from_context(Term t)
    {
      std::set<core::identifier_string> s = core::find_identifiers(t);
      for (std::set<core::identifier_string>::iterator i = s.begin(); i != s.end(); ++i)
      {
        remove_identifier(*i);
      }
    }

    /// Returns true if the identifier s appears in the context.
    ///
    virtual bool has_identifier(core::identifier_string s) const = 0;
   
    /// Returns a unique identifier, with the given hint as prefix.
    /// The returned identifier is added to the context.
    ///
    virtual core::identifier_string operator()(const std::string& hint)
    {
      core::identifier_string id(hint);
      int index = 0;
      while (has_identifier(id))
      {
        std::ostringstream out;
        out << hint << index++;
        std::string name = out.str();
// TODO: why doesn't this work???        
//        std::string name = str(boost::format(hint + "%02d") % index++);
        id = core::identifier_string(name);
      }
      add_to_context(id);
      return id;
    }
};

/// Identifier generator that generates names with a postfix consisting of a number,
/// that is incremented after each call to operator().
class number_postfix_generator
{
  protected:
    std::string m_prefix;
    unsigned int m_index;
  
  public:
    number_postfix_generator()
      : m_prefix("x"), m_index(0)
    {}
    
    number_postfix_generator(const std::string& prefix, unsigned int index = 0)
     : m_prefix(prefix), m_index(index)
    {}

    core::identifier_string operator()()
    {
      std::ostringstream out;
      out << m_prefix << m_index++;
      return core::identifier_string(out.str());
    }
};

} // namespace new_data

} // namespace mcrl2

#endif // MCRL2_NEW_DATA_IDENTIFIER_GENERATOR_H
