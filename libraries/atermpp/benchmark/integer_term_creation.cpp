// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "benchmark_shared.h"

#include "mcrl2/atermpp/aterm_int.h"

using namespace atermpp;

int main(int, char*[])
{
  std::size_t amount = 200000000;
  std::size_t iterations = 50000;
  std::size_t number_of_threads = 1;

  auto create_integers = [amount,iterations,number_of_threads]()
  {
    // Store the integers in a vector to prevent them from being deleted.
    std::vector<aterm_int> integers(amount);

    for (std::size_t i = 0; i < iterations / number_of_threads; ++i)
    {
      integers[i] = aterm_int(i);
    }
  };

  benchmark_threads(number_of_threads, create_integers);

  return 0;
}
