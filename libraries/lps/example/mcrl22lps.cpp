// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl22lps.cpp
/// \brief Add your file description here.

#include <iostream>
#include <string>
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/mcrl22lps.h"
#include "mcrl2/modal_formula/detail/read_text.h"

using namespace std;
using namespace lps;
using namespace lps::detail;

int main(int argc, char* argv[])
{
  aterm bottom_of_stack;
  aterm_init(bottom_of_stack);
  gsEnableConstructorFunctions();

  string infile(argv[1]);
  specification spec = mcrl22lps(read_text(infile));
  spec.save(argv[2]);

  return 0;
}
