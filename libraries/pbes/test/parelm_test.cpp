// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbes_test.cpp
/// \brief Add your file description here.

#include <iostream>
#include <iterator>
#include <utility>
#include <boost/test/minimal.hpp>
#include <boost/algorithm/string.hpp>
#include "mcrl2/data/utility.h"
#include "mcrl2/lps/linearise.h"
#include "mcrl2/lps/detail/test_input.h"
#include "mcrl2/pbes/parelm.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/pbes_translate.h"
#include "mcrl2/pbes/lps2pbes.h"
#include "mcrl2/pbes/rename.h"
#include "mcrl2/pbes/complement.h"
#include "mcrl2/atermpp/aterm_init.h"

using namespace std;
using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::lps;
using namespace mcrl2::lps::detail;
using namespace mcrl2::state_formulas;
using namespace mcrl2::pbes_system;
using namespace mcrl2::pbes_system::detail;

const std::string SPECIFICATION =
  "act a:Nat;                               \n"
  "                                         \n"
  "map smaller: Nat#Nat -> Bool;            \n"
  "                                         \n"
  "var x,y : Nat;                           \n"
  "                                         \n"
  "eqn smaller(x,y) = x < y;                \n"
  "                                         \n"
  "proc P(n:Nat) = sum m: Nat. a(m). P(m);  \n"
  "                                         \n"
  "init P(0);                               \n";

const std::string TRIVIAL_FORMULA  = "[true*]<true*>true";

void test_parelm()
{
  specification spec    = linearise(lps::detail::ABP_SPECIFICATION());
  state_formula formula = state_formulas::parse_state_formula(TRIVIAL_FORMULA, spec);
  bool timed = false;
  pbes<> p = lps2pbes(spec, formula, timed);
  pbes_parelm_algorithm algorithm;
  algorithm.run(p);
  BOOST_CHECK(p.is_well_typed());
}

int test_main(int argc, char** argv)
{
  atermpp::aterm_init();

  test_parelm();

  return 0;
}
