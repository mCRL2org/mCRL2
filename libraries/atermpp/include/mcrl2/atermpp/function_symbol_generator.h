// Author(s): Wieger Wesselink, Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/function_symbol_generator.h
/// \brief add your file description here.

#ifndef MCRL2_ATERMPP_FUNCTION_SYMBOL_GENERATOR_H
#define MCRL2_ATERMPP_FUNCTION_SYMBOL_GENERATOR_H

#include <algorithm>
#include <cassert>
#include <cctype>
#include <memory>
#include "mcrl2/atermpp/function_symbol.h"
#include "mcrl2/utilities/text_utility.h"

namespace atermpp {

/// \brief Generates unique function symbols with a given prefix.
class function_symbol_generator
{
  protected:
    const std::string m_prefix;
    std::size_t m_initial_index; // cache the value that is set in the constructor
    std::size_t m_index;
    std::string m_string_buffer;

  public:
    /// \brief Constructor
    /// \param[in] prefix The prefix of the generated generated strings.
    /// \pre The prefix may not be empty, and it may not have trailing digits
    function_symbol_generator(const std::string& prefix)
      : m_prefix(prefix),
        m_string_buffer(prefix)
    {
      assert(!prefix.empty() && !(std::isdigit(*prefix.rbegin())));
      
      // set m_index such that no function symbol exists with the name 'prefix + std::to_string(n)'
      // for all values n >= m_index
      m_index = detail::get_sufficiently_large_postfix_index(prefix);
      detail::index_increaser increase_m_index(m_initial_index,m_index);
      detail::register_function_symbol_prefix_string(prefix,increase_m_index);
      m_initial_index = m_index;
    }

    /// \brief Restores the index back to the value that was initially assigned in the constructor.
    void clear()
    {
      m_index = m_initial_index;
    }

    ~function_symbol_generator()
    {
      detail::deregister_function_symbol_prefix_string(m_prefix);
    }

    /// \brief Generates a unique function symbol with the given prefix followed by a number.
    function_symbol operator()(std::size_t arity = 0)
    {
      // Put the number m_index after the prefix in the string buffer.
      mcrl2::utilities::number2string(m_index, m_string_buffer, m_prefix.size());
      m_index++;
      return function_symbol(m_string_buffer, arity, false);
    }
};

} // namespace atermpp

#endif // MCRL2_ATERMPP_FUNCTION_SYMBOL_GENERATOR_H
