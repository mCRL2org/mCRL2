// Author(s): Wieger Wesselink
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

#include <cassert>
#include <cctype>
#include "mcrl2/atermpp/function_symbol.h"
#include "mcrl2/utilities/text_utility.h"

namespace atermpp {

/// \brief Generates unique function symbols with a given prefix.
class function_symbol_generator
{
  protected:
    std::string m_prefix;
    size_t m_index;

  public:
    /// \brief Constructor
    /// \param The prefix of the generated generated strings
    /// \pre The prefix may not be empty, and it may not have trailing digits
    function_symbol_generator(const std::string& prefix)
      : m_prefix(prefix)
    {
      assert(!m_prefix.empty() && !(std::isdigit(*m_prefix.rbegin())));
      
      // set m_index such that no function symbol exists with the name 'prefix + std::to_string(n)'
      // for all values n >= m_index
      m_index = detail::get_sufficiently_large_postfix_index(m_prefix);
      detail::index_increaser increase_m_index(m_index);
      detail::register_functon_symbol_prefix_string(m_prefix,increase_m_index);
    }

    ~function_symbol_generator()
    {
      detail::deregister_functon_symbol_prefix_string(m_prefix);
    }

    /// \brief Generates a unique function symbol with the given prefix followed by a number.
    function_symbol operator()(std::size_t arity = 0)
    {
      // TODO: this code can probably be optimized
      return function_symbol(m_prefix + mcrl2::utilities::number2string(m_index++), arity);
    }
};

} // namespace atermpp

#endif // MCRL2_ATERMPP_FUNCTION_SYMBOL_GENERATOR_H
