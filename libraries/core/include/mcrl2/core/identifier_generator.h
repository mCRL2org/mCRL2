// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/identifier_generator.h
/// \brief add your file description here.

#ifndef MCRL2_CORE_IDENTIFIER_GENERATOR_H
#define MCRL2_CORE_IDENTIFIER_GENERATOR_H

#include <cassert>
#include <cctype>
#include <map>
#include <string>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include "mcrl2/core/find.h"
#include "mcrl2/atermpp/container_utility.h"

namespace mcrl2 {

namespace core {

  /// \brief Identifier generator that generates names with a postfix consisting of a number.
  class number_postfix_generator
  {
    protected:
      /// \brief A map that maintains the highest index for each prefix.
      std::map<std::string, int> m_index;
     
      /// \brief The default hint.
      std::string m_hint;


    public:

      /// \brief Constructor.
      number_postfix_generator(std::string hint = "FRESH_VAR")
        : m_hint(hint)
      {}

      /// \brief Adds the strings in the range [first, last) to the context.
      /// \param id A string
      void add_to_context(const std::string& id)
      {
        std::string::size_type i = id.find_last_not_of("0123456789");
        if (i == std::string::npos)
        {
          m_index[id] = -1;
        }
        else
        {
          std::string s = id.substr(0, i + 1);
          std::string num = id.substr(i + 1);
          int index = boost::lexical_cast<int>(num);
          m_index[s] = index;
        }
      }
  
      /// \brief Adds the strings in the range [first, last) to the context.
      /// \param first
      /// \param last
      /// [first, last) is a sequence of strings that is used as context.
      template <typename Iter>
      void add_to_context(Iter first, Iter last)
      {
        for (Iter i = first; i != last; ++i)
        {
          add_to_context(*i);
        }
      }
  
      /// \brief Constructor.
      /// \param first
      /// \param last
      /// [first, last) is a sequence of strings that is used as context.
      template <typename Iter>
      number_postfix_generator(Iter first, Iter last, std::string hint = "FRESH_VAR")
        : m_hint(hint)
      {
        add_to_context(first, last);
      }
  
      /// \brief Generates a fresh identifier that doesn't appear in the context.
      /// \return A fresh identifier.
      std::string operator()(std::string hint)
      {
        // make sure there are no digits at the end of hint
        if (std::isdigit(hint[hint.size() - 1]))
        {
          std::string::size_type i = hint.find_last_not_of("0123456789");
          hint = hint.substr(0, i + 1);
        }

        std::ostringstream out;
        out << hint << ++m_index[hint];
        return out.str();
      }

      /// \brief Generates a fresh identifier that doesn't appear in the context.
      /// \return A fresh identifier.
      std::string operator()()
      {
        return (*this)(m_hint);
      }
      
      /// \brief Returns the default hint.
      const std::string& hint() const
      {
        return m_hint;
      }
      
      /// \brief Returns the default hint.
      std::string& hint()
      {
        return m_hint;
      }
  };

} // namespace core

} // namespace mcrl2

#endif // MCRL2_CORE_IDENTIFIER_GENERATOR_H
