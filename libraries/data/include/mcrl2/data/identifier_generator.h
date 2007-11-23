// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/identifier_generator.h
/// \brief Add your file description here.

#ifndef MCRL2_DATA_IDENTIFIER_GENERATOR_H
#define MCRL2_DATA_IDENTIFIER_GENERATOR_H

#include <string>
#include <boost/format.hpp>
#include "mcrl2/basic/identifier_string.h"

namespace lps {

/// A class that generates identifiers. A context is maintained
/// containing already used identifiers.
///
/// Using the operator()() and operator()(std::string) fresh
/// identifiers can be generated that do not appear in the
/// context.
class identifier_generator
{
  public:
    identifier_generator()
    {}

    virtual ~identifier_generator()
    {}

    /// Clears the context.
    virtual void clear_context() = 0;

    /// Adds the identifier s to the context.
    virtual void add_identifier(identifier_string s) = 0;

    /// Removes the identifier s from the context.
    virtual void remove_identifier(identifier_string s) = 0;

    /// Adds identifiers of term t to the context.
    template <typename Term>
    void add_to_context(Term t)
    {
      std::set<identifier_string> s = identifiers(t);
      for (std::set<identifier_string>::iterator i = s.begin(); i != s.end(); ++i)
      {
        add_identifier(*i);
      }
    }

    /// Adds identifiers of term t to the context.
    template <typename Container>
    void add_identifiers(const Container& c)
    {
      for (typename Container::const_iterator i = c.begin(); i != c.end(); ++i)
      {
        add_identifier(*i);
      }
    }

    /// Removes identifiers of term t from the context.
    template <typename Term>
    void remove_from_context(Term t)
    {
      std::set<identifier_string> s = identifiers(t);
      for (std::set<identifier_string>::iterator i = s.begin(); i != s.end(); ++i)
      {
        remove_identifier(*i);
      }
    }

    /// Returns true if the identifier s appears in the context.
    virtual bool has_identifier(identifier_string s) const = 0;

    /// Returns a unique identifier, with the given hint as prefix.
    /// The returned identifier is added to the context.
    virtual identifier_string operator()(const std::string& hint)
    {
      identifier_string id(hint);
      int index = 0;
      while (has_identifier(id))
      {   
        std::string name = str(boost::format(hint + "%02d") % index++);
        id = identifier_string(name);
      }
      add_to_context(id);
      return id;
    }
};

} // namespace lps

#endif // MCRL2_DATA_IDENTIFIER_GENERATOR_H
