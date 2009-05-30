// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file remove_parameters_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <string>
#include <set>
#include <boost/test/minimal.hpp>
#include "mcrl2/lps/mcrl22lps.h"
#include "mcrl2/lps/parse.h"
#include "mcrl2/lps/detail/lps_algorithm.h"
#include "mcrl2/lps/detail/lps_parameter_remover.h"
#include "mcrl2/lps/detail/specification_property_map.h"
#include "mcrl2/core/garbage_collection.h"

using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::lps;

std::string SPEC =
  "act  action: Nat;         \n"
  "                          \n"
  "proc P(s: Pos, i: Nat) =  \n"
  "       (s == 2) ->        \n"
  "         action(3) .      \n"
  "         P(1, 4)          \n"
  "     + (s == 1) ->        \n"
  "         action(4) .      \n"
  "         P(2, 4)          \n"
  "     + true ->            \n"
  "         delta;           \n"
  "                          \n"
  "init P(1, 0);             \n"
  ;

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

void test_remove_parameters()
{
  specification spec = parse_linear_process_specification(SPEC);

  // remove parameters
  std::set<data::variable> to_be_removed;
  to_be_removed.insert(variable("s", sort_pos::pos()));
  to_be_removed.insert(variable("i", sort_nat::nat()));
  lps::detail::lps_parameter_remover<std::set<data::variable> > remover(to_be_removed);

  data::variable_list v;
  remover.remove_list(v);
  remover(spec);

  // check the result
  std::string expected_result = "process_parameter_names =";
  lps::detail::specification_property_map info(spec);  
  BOOST_CHECK(data::detail::compare_property_maps("test_remove_parameters", info, expected_result));
  core::garbage_collect();

  specification spec2 = parse_linear_process_specification(SPEC);
  remove_parameters(spec2, to_be_removed);
  lps::detail::specification_property_map info2(spec);
  BOOST_CHECK(data::detail::compare_property_maps("test_remove_parameters", info2, expected_result));
  core::garbage_collect();
}

void test_instantiate_free_variables()
{
  specification spec = mcrl22lps(ABP_SPEC);
  lps::detail::lps_algorithm algorithm(spec);
  algorithm.instantiate_free_variables();
  core::garbage_collect();
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  test_remove_parameters();
  test_instantiate_free_variables();

  return 0;
}
