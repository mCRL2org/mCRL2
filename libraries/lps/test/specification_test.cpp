// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file specification_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <string>
#include <set>
#include <iterator>
#include <boost/test/minimal.hpp>
#include <mcrl2/lps/mcrl22lps.h>
#include <mcrl2/lps/specification.h>
#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/core/detail/print_utility.h"

using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::lps;

const std::string ABP_SPEC=
  "% This file contains the alternating bit protocol, as described in W.J.    \n"
  "% Fokkink, J.F. Groote and M.A. Reniers, Modelling Reactive Systems.       \n"
  "%                                                                          \n"
  "% The only exception is that the domain D consists of two data elements to \n"
  "% facilitate simulation.                                                   \n"
  "                                                                           \n"
  "sort                                                                       \n"
  "  D     = struct d1 | d2;                                                  \n"
  "  Error = struct e;                                                        \n"
  "                                                                           \n"
  "act                                                                        \n"
  "  r1,s4: D;                                                                \n"
  "  s2,r2,c2: D # Bool;                                                      \n"
  "  s3,r3,c3: D # Bool;                                                      \n"
  "  s3,r3,c3: Error;                                                         \n"
  "  s5,r5,c5: Bool;                                                          \n"
  "  s6,r6,c6: Bool;                                                          \n"
  "  s6,r6,c6: Error;                                                         \n"
  "  i;                                                                       \n"
  "                                                                           \n"
  "proc                                                                       \n"
  "  S(b:Bool)     = sum d:D. r1(d).T(d,b);                                   \n"
  "  T(d:D,b:Bool) = s2(d,b).(r6(b).S(!b)+(r6(!b)+r6(e)).T(d,b));             \n"
  "                                                                           \n"
  "  R(b:Bool)     = sum d:D. r3(d,b).s4(d).s5(b).R(!b)+                      \n"
  "                  (sum d:D.r3(d,!b)+r3(e)).s5(!b).R(b);                    \n"
  "                                                                           \n"
  "  K             = sum d:D,b:Bool. r2(d,b).(i.s3(d,b)+i.s3(e)).K;           \n"
  "                                                                           \n"
  "  L             = sum b:Bool. r5(b).(i.s6(b)+i.s6(e)).L;                   \n"
  "                                                                           \n"
  "init                                                                       \n"
  "  allow({r1,s4,c2,c3,c5,c6,i},                                             \n"
  "    comm({r2|s2->c2, r3|s3->c3, r5|s5->c5, r6|s6->c6},                     \n"
  "        S(true) || K || L || R(true)                                       \n"
  "    )                                                                      \n"
  "  );                                                                       \n"
  ;

void test_traverse_sort_expressions()
{
  specification spec = mcrl22lps(ABP_SPEC);
  std::set<sort_expression> s;
  traverse_sort_expressions(spec, std::inserter(s, s.end()));
  std::cout << core::detail::print_pp_set(s) << std::endl;
  // BOOST_CHECK(false);
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  test_traverse_sort_expressions();
  core::garbage_collect();

  return 0;
}
