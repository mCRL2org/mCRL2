// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file constructors.cpp
/// \brief Add your file description here.

#include <iostream>
#include <cassert>
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/linearise.h"
#include "test_specifications.h"

using namespace mcrl2;

int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  lps::specification spec1 = lps::linearise(ABP_SPECIFICATION);
  lps::specification spec2(spec1.data(),
                           spec1.action_labels(),
                           spec1.process(),
                           spec1.initial_process()
                          );
  assert(spec1 == spec2);

  return 0;
}
