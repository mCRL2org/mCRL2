// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/atermpp/aterm_list.h"

using namespace atermpp;

int main(int, char*[])
{
  std::size_t size = 10000;
  std::size_t iterations = 10000;

  function_symbol c("c", 0);
  aterm_appl c_term(c);

  for (std::size_t i = 0; i < iterations; ++i)
  {
    aterm_list list;
    for (std::size_t i = 0; i < size; ++i)
    {
      list.push_front(c_term);
    }
  }

  return 0;
}
