// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl22lps.cpp
/// \brief Add your file description here.

// #define MCRL2_PBES_TRANSLATE_DEBUG

#include <iostream>
#include <string>
#include "mcrl2/lps/specification.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/lps2pbes.h"
#include "mcrl2/core/text_utility.h"

using namespace std;
using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::lps;
using namespace mcrl2::modal_formula;
using namespace mcrl2::modal_formula::detail;
using namespace mcrl2::pbes_system;
using namespace mcrl2::pbes_system::pbes_expr;

int main(int argc, char* argv[])
{
  MCRL2_ATERMPP_INIT(argc, argv)

  string spec_file(argv[1]); // specification file
  string mcf_file(argv[2]);  // modal formula file
  string pbes_file(argv[3]); // resulting pbes file

  specification spec = mcrl22lps(core::read_text(spec_file));
  state_formula sf = mcf2statefrm(core::read_text(mcf_file), spec);
  bool timed = false;
  pbes<> p = lps2pbes(spec, sf, timed);
  p.save(pbes_file);

  return 0;
}
