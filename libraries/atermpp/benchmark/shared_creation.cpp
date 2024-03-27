// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "benchmark_shared.h"

using namespace atermpp;

int main(int argc, char* argv[])
{
  detail::g_term_pool().enable_garbage_collection(false);
  std::size_t number_of_threads = 1;

  // Accept one argument for the number of threads.
  if (argc > 1)
  {
    number_of_threads = static_cast<std::size_t>(std::stoi(argv[1]));
  }

  std::size_t size = 400000;

  // Define a function that repeatedly creates nested function applications.
  auto nested_function = [&](std::size_t) -> void
    {
      aterm f = create_nested_function<2>("f", "c",  size);
    };

  benchmark_threads(number_of_threads, nested_function);

  return 0;
}
