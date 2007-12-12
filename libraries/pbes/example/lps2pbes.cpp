// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl22lps.cpp
/// \brief Add your file description here.

#include <iostream>
#include <string>
#include "mcrl2/lps/specification.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/detail/tools.h"
#include "mcrl2/modal_formula/detail/read_text.h"

using namespace std;
using namespace lps;
using namespace lps::detail;

int main(int argc, char* argv[])
{
  MCRL2_CORE_LIBRARY_INIT()

  string spec_file(argv[1]); // specification file
  string mcf_file(argv[2]);  // modal formula file
  string pbes_file(argv[3]); // resulting pbes file

  specification spec = mcrl22lps(read_text(spec_file));
  state_formula sf = mcf2statefrm(read_text(mcf_file), spec);
  bool timed = false;
  pbes<> p = lps2pbes(spec, sf, timed);
  p.save(pbes_file);

  return 0;
}
