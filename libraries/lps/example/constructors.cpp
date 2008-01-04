// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file constructors.cpp
/// \brief Add your file description here.

#include <iostream>
#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/mcrl22lps.h"
#include "test_specifications.h"

using namespace std;
using namespace atermpp;
using namespace lps;
using namespace lps::detail;

int main()
{
  MCRL2_CORE_CORE_INIT()

  specification spec1 = mcrl22lps(ABP_SPECIFICATION);
  specification spec2(spec1.data(),
                      spec1.action_labels(),
                      spec1.process(),
                      spec1.initial_process()
                     );
  assert(spec1 == spec2);

  return 0;
}
