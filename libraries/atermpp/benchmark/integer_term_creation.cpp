// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include "mcrl2/atermpp/aterm_int.h"

using namespace atermpp;

int main(int, char*[])
{
  std::size_t amount = 100000000;
  std::size_t iterations = 100000;

  std::vector<aterm_int> integers(amount);

  for (std::size_t i = 0; i < iterations; ++i)
  {
    integers[i] = aterm_int(i);
  }

  return 0;
}
