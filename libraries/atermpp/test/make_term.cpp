// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file make_term.cpp
/// \brief Add your file description here.

#include <iostream>
#include <boost/test/minimal.hpp>
#include "mcrl2/atermpp/aterm.h"

using namespace std;
using namespace atermpp;

void foo()
{
  const int i       = 42;
  const char* blob  = "12345678";
  const double r    = 3.14;
  const char *func  = "f";

  aterm term[4];
  aterm list[3];
  aterm appl[3];

  term[0] = make_term("<int>" , i);         // integer value: 42
  term[1] = make_term("<str>" , func);      // quoted application: "f", no args
  term[2] = make_term("<real>", r);         // real value: 3.14
  term[3] = make_term("<blob>", 8, blob);   // blob of size 8, data: 12345678

  list[0] = make_term("[]");
  list[1] = make_term("[1,<int>,<real>]", i, r);
  list[2] = make_term("[<int>,<list>]", i+1, list[1]);

  appl[0] = make_term("<appl>", func);
  appl[1] = make_term("<appl(<int>)>", func, i);
  appl[2] = make_term("<appl(<int>, <term>, <list>)>", func, 42, term[3], list[2]);

  std::cout << "appl[2] = " << appl[2] << std::endl;
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)
  foo();

  return 0;
}
