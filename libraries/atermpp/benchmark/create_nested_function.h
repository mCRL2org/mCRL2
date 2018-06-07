// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/atermpp/aterm_appl.h"

using namespace atermpp;

/// \brief Create a nested function application f_depth. Where f_0 = c and f_i = f(f_i-1,...,f_i-1).
aterm_appl create_nested_function(std::size_t number_of_arguments, std::size_t depth)
{
  // Create a suitable function application.
  function_symbol f("f", number_of_arguments);
  function_symbol c("c", 0);

  aterm_appl c_term(c);

  // Initialize a wide function application.
  std::vector<aterm> arguments(f.arity());
  for (std::size_t i = 0; i < arguments.size(); ++i)
  {
    arguments[i] = c_term;
  }
  aterm_appl f_term(f, arguments.begin(), arguments.end());

  for (std::size_t j = 0; j < depth; ++j)
  {
    // Create a very wide nested function application
    for (std::size_t k = 0; k < arguments.size(); ++k)
    {
      arguments[k] = f_term;
    }
    f_term = aterm_appl(f, arguments.begin(), arguments.end());
  }

  return f_term;
}

template<std::size_t N>
aterm_appl create_nested_function(std::size_t depth)
{
  // Create a suitable function application.
  function_symbol f("f", N);
  function_symbol c("c", 0);

  aterm_appl c_term(c);
  aterm_appl f_term(f, c_term, c_term);

  for (std::size_t j = 0; j < depth; ++j)
  {
    f_term = aterm_appl(f, f_term, f_term);
  }

  return f_term;
}
