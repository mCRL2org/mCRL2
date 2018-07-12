// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "benchmark_shared.h"

int main(int, char*[])
{
  std::size_t amount = 100000;
  std::size_t iterations = 20;

  for (std::size_t i = 0; i < iterations; ++i)
  {
    // This block is to ensure that all function applications are protected.
    {
      // Generate various function applications.
      // TODO: Give them a non-uniform structure.
      aterm_appl f1 = create_nested_function(1, amount);
      aterm_appl f2 = create_nested_function(2, amount);
      aterm_appl f4 = create_nested_function(4, amount);
      aterm_appl f7 = create_nested_function(7, amount);
    }

    // Trigger garbage collection.
    detail::collect_terms_with_reference_count_0();
  }
}
