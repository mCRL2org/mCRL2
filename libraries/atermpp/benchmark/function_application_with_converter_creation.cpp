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
  std::size_t number_of_arguments = 0;
  std::size_t size = 10000;
  std::size_t iterations = 10000;
  std::size_t number_of_threads = 1;

  // Accept one argument for the number of arguments.
  if (argc > 1)
  {
    // The first argument is the path of the executable.
    number_of_arguments = static_cast<std::size_t>(std::stoi(argv[1]));
  }

  auto nested_function = [iterations, number_of_arguments, size, number_of_threads]()
  {
    aterm_appl f;
    for (std::size_t i = 0; i < iterations / number_of_threads; ++i)
    {
      f = create_nested_function<true>(number_of_arguments, size);
    }
  };

  benchmark_threads(number_of_threads, nested_function);

  return 0;
}
