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
  std::size_t amount = 1500000;
  std::size_t iterations = 20;

  detail::enable_garbage_collection(false);
  for (std::size_t i = 0; i < iterations; ++i)
  {
    // This block is to ensure that they are unprotected when garbage collection is triggered explicitly.
    {
      // Generate various function applications.
      aterm_appl f1 = create_nested_function("f", "c", 1, 1 * amount);
      aterm_appl f2 = create_nested_function("g", "d", 2, 4 * amount);
      aterm_appl f4 = create_nested_function("h", "e", 4, amount/4);
      aterm_appl f7 = create_nested_function("i", "f", 7, amount/8);
    }

    // Trigger garbage collection.
    detail::collect_terms_with_reference_count_0();
  }
}
