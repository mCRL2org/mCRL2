// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbes2bool.cpp

#include "mcrl2/pbes/detail/pbessolve_algorithm.h"

int main(int argc, char* argv[])
{
  return mcrl2::pbes_system::detail::pbessolve_tool("pbes2bool").execute(argc, argv);
}
