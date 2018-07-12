// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "benchmark_shared.h"
#include "mcrl2/atermpp/aterm_list.h"

using namespace atermpp;

int main(int, char*[])
{
  std::size_t length = 100000;
  std::size_t iterations = 1000;
  std::size_t number_of_threads = 1;

  auto create_list = [number_of_threads, iterations, length]()
  {
    function_symbol c("c", 0);
    aterm_appl c_term(c);

    aterm_list list;
    for (std::size_t i = 0; i < iterations / number_of_threads; ++i)
    {
      aterm_list tmp_list;
      for (std::size_t j = 0; j < length; ++j)
      {
        tmp_list.push_front(c_term);
      }
      list = tmp_list;
    }
  };

  benchmark_threads(number_of_threads, create_list);

  return 0;
}
