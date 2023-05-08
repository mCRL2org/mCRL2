// Author(s): Jan Friso Groote. Based on pbessolve.cpp by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pressolve.cpp

#include "mcrl2/pres/tools/pressolve.h"

int main(int argc, char* argv[])
{
  return pressolve_tool("pressolve").execute(argc, argv);
}
