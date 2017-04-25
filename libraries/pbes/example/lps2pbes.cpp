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

#include "mcrl2/lps/specification.h"
#include "mcrl2/lps/linearise.h"
#include "mcrl2/pbes/lps2pbes.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/utilities/text_utility.h"
#include <iostream>
#include <string>

using namespace std;
using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::lps;
using namespace mcrl2::state_formulas;
using namespace mcrl2::pbes_system;
using mcrl2::state_formulas::algorithms::parse_state_formula;

int main(int argc, char* argv[])
{
  if (argc < 4)
  {
    std::cout << "Usage: lps2pbes FILENAME-INPUT-LPS FILENAME-INPUT-MCF FILENAME-OUTPUT-PBES" << std::endl;
    return 0;
  }

  string spec_file(argv[1]); // specification file
  string mcf_file(argv[2]);  // modal formula file
  string pbes_file(argv[3]); // resulting pbes file

  specification spec = remove_stochastic_operators(linearise(utilities::read_text(spec_file)));
  state_formula sf = parse_state_formula(utilities::read_text(mcf_file), spec);
  bool timed = false;
  pbes p = lps2pbes(spec, sf, timed);
  save_pbes(p, pbes_file);

  return 0;
}
