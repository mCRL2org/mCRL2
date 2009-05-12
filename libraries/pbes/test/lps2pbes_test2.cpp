// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file lps2bes_test2.cpp
/// \brief Test case for lps2bes. The translation is reported to be
/// too slow for this example.

#include <iostream>
#include <iterator>
#include <boost/test/minimal.hpp>
#include "mcrl2/lps/mcrl22lps.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/lps2pbes.h"
#include "mcrl2/core/garbage_collection.h"

using namespace mcrl2;

const std::string MACHINE_SPECIFICATION =
  "%% file machine.mcrl2                                             \n"
  "                                                                  \n"
  "act                                                               \n"
  "                                                                  \n"
  "  ch_tea, ch_cof, insq, insd, take_tea, take_cof, want_change,    \n"
  "  sel_tea, sel_cof, accq, accd, put_tea, put_cof, put_change,     \n"
  "  ok_tea, ok_coffee, quarter, dollar, tea, coffee, change ;       \n"
  "                                                                  \n"
  "proc                                                              \n"
  "                                                                  \n"
  "  User = ch_tea.UserTea + ch_cof.UserCof ;                        \n"
  "  UserTea =                                                       \n"
  "    insq.insq.take_tea.User  +                                    \n"
  "    insd.take_tea.( take_tea + want_change ).User ;               \n"
  "  UserCof = ( insq.insq.insq.insq + insd ).take_cof.User ;        \n"
  "                                                                  \n"
  "  Mach = sel_tea.MachTea + sel_cof.MachCof ;                      \n"
  "  MachTea =                                                       \n"
  "    accq.accq.put_tea.Mach +                                      \n"
  "    accd.put_tea.( put_tea + put_change ).Mach ;                  \n"
  "  MachCof =                                                       \n"
  "    ( accq.accq.accq.accq + accd ).put_cof.Mach ;                 \n"
  "                                                                  \n"
  "init                                                              \n"
  "                                                                  \n"
  "  allow(                                                          \n"
  "    { ok_tea, ok_coffee, quarter, dollar, tea, coffee, change } , \n"
  "    comm(                                                         \n"
  "       { ch_tea|sel_tea -> ok_tea, ch_cof|sel_cof -> ok_coffee,   \n"
  "         insq|accq -> quarter, insd|accd -> dollar,               \n"
  "         take_tea|put_tea -> tea, take_cof|put_cof -> coffee,     \n"
  "         want_change|put_change -> change } ,                     \n"
  "       User || Mach                                               \n"
  "    ) ) ;                                                         \n"
  ;

const std::string MACHINE_FORMULA1 =
  "%% after choice for tea and two quarter always tea (true) \n"
  " [ true* . ok_tea . quarter . quarter . !tea ] false      \n"
  ;

const std::string MACHINE_FORMULA2 =
  "%% always eventually action ready (true)     \n"
  " [ true* . dollar . !(tea||coffee) ] false   \n"
  ;

const std::string MACHINE_FORMULA3 =
  "%% after a quarter no change directly (true) \n"
  " [ true* . quarter . change ] false          \n"
  ;

void test_lps2pbes(std::string lps_spec, std::string mcf_formula)
{
  using namespace pbes_system;

  lps::specification spec = lps::mcrl22lps(lps_spec);
  modal_formula::state_formula formula = modal_formula::detail::mcf2statefrm(mcf_formula, spec);
  bool timed = false;
  pbes<> p = lps2pbes(spec, formula, timed);
}

int test_main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT_DEBUG(argc, argv)

  test_lps2pbes(MACHINE_SPECIFICATION, MACHINE_FORMULA1);
  core::garbage_collect();
  test_lps2pbes(MACHINE_SPECIFICATION, MACHINE_FORMULA2);
  core::garbage_collect();
  test_lps2pbes(MACHINE_SPECIFICATION, MACHINE_FORMULA3);
  core::garbage_collect();

  return 0;
}
