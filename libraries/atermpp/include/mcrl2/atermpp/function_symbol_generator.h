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
#include "mcrl2/atermpp/aterm_string.h"
#include "mcrl2/utilities/text_utility.h"

namespace atermpp {

/// \brief Generates unique function symbols with a given prefix.
class function_symbol_generator
{
  protected:
    std::string prefix;
    std::size_t index;

  public:
    /// \brief Constructor
    /// \param The prefix of the generated generated strings
    /// \pre The prefix may not be empty, and it may not have trailing digits
    function_symbol_generator(const std::string& prefix_)
      : prefix(prefix_)
    {
      assert(!prefix.empty() && !(std::isdigit(*prefix.rbegin())));

      // TODO: add the computation of index
      // set index such that no function symbol exists with the name 'prefix + std::to_string(n)'
      // for all values n >= index
      index = 12345;
    }

    /// \brief Generates a unique function symbol with the given prefix followed by a number.
    function_symbol operator()(std::size_t arity = 0)
    {
      // TODO: this code can probably be optimized
      return function_symbol(prefix + mcrl2::utilities::number2string(index++), arity);
    }
};

} // namespace atermpp

#endif // MCRL2_ATERMPP_FUNCTION_SYMBOL_GENERATOR_H
