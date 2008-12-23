// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file action_rename_test.cpp
/// \brief Action rename test.

#include <boost/test/minimal.hpp>
#include "mcrl2/lps/action_rename.h"
#include "mcrl2/lps/mcrl22lps.h"

using namespace mcrl2;
using lps::mcrl22lps;
using lps::specification;
using lps::action_rename_specification;
// using lps::action_rename;

void test1()
{
  const std::string SPEC =
  "act a:Nat;                               \n"
  "                                         \n"
  "proc P(n:Nat) = sum m: Nat. a(m). P(m);  \n"
  "                                         \n"
  "init P(0);                               \n"; 

  const std::string AR_SPEC = "???";
    
  specification spec = mcrl22lps(SPEC);
  action_rename_specification ar_spec = parse_action_rename_specification(AR_SPEC, spec);
  // specification new_spec = action_rename(spec, ar_spec);
}

int test_main(int argc, char** argv )
{
  MCRL2_ATERMPP_INIT(argc, argv)
  //test1();

  return 0;
}
