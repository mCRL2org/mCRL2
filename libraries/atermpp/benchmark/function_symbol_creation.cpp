// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "benchmark_shared.h"

#include "mcrl2/atermpp/function_symbol.h"

#include <vector>
#include <string>

using namespace atermpp;

/// \brief Benchmark the creation of function symbols
int main(int, char*[])
{
  std::size_t amount = 10000;
  std::size_t iterations = 1000;
  std::size_t number_of_threads = 1;

  auto create_function_symbols = [amount,iterations,number_of_threads]()
  {
    // Store them in a vector to prevent them from being deleted.
    std::vector<function_symbol> symbols(amount);

    // Generate function symbols f + suffix, where suffix from 0 to amount.
    std::string name("f");
    for (std::size_t k = 0; k < iterations / number_of_threads; ++k)
    {
      for (std::size_t i = 0; i < amount; ++i)
      {
        symbols[i] = function_symbol(name + std::to_string(i), 0);
      }
    }
  };

  benchmark_threads(number_of_threads, create_function_symbols);
}
