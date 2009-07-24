// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file specification_property_map_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <string>
#include <boost/test/minimal.hpp>
#include <boost/algorithm/string.hpp>
#include "mcrl2/lps/linearise.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/lps/detail/specification_property_map.h"
#include "mcrl2/core/garbage_collection.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace mcrl2;
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

const std::string LPSINFO =
  "declared_action_label_count  = 21                                                                                                          \n"
  "declared_action_labels       = c2, c3, c5, c6, i, r1, r2, r3, r5, r6, s2, s3, s4, s5, s6                                                   \n"
  "declared_free_variable_count = 20                                                                                                          \n"
  "declared_free_variable_names = dc, dc1, dc10, dc11, dc12, dc13, c14, dc15, dc16, dc17, dc18, dc19, dc2, dc3, dc4, dc5, dc6, dc7, dc8, dc9  \n"
  "delta_summand_count          = 1                                                                                                           \n"
  "process_parameter_count      = 11                                                                                                          \n"
  "process_parameter_names      = b_K, b_L, b_R, b_S, d_K, d_R, d_S, s31_S, s32_K, s33_L, s34_R                                               \n"
  "summand_count                = 11                                                                                                          \n"
  "tau_summand_count            = 70                                                                                                          \n"
  "used_action_label_count      = 19                                                                                                          \n"
  "used_action_labels           = c2, c3, c5, c6, i, r1, s4                                                                                   \n"
  "used_free_variable_count     = 15                                                                                                          \n"
  "used_free_variable_names     = dc1, dc11, dc12, dc13, x15, dc16, dc17, dc18, dc19, dc4, dc5, dc6, dc7, dc8, dc9                            \n"
  "used_multi_action_count      = 9                                                                                                           \n"
  "used_multi_actions           = {a1, c2}; {c3}; {c5}; {c6}; {i,j}; {r1}; {s4}                                                               \n"
  ;
  
int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  specification spec = linearise(ABP_SPEC);
  lps::detail::specification_property_map info1(spec);
  std::cerr << info1.to_string() << std::endl;
  lps::detail::specification_property_map info2(LPSINFO);
  std::cerr << info1.compare(info2) << std::endl;
  core::garbage_collect();

  return 0;
}
