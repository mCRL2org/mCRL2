// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/tools/lpsbisim2pbes.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_TOOLS_LPSBISIM2PBES_H
#define MCRL2_PBES_TOOLS_LPSBISIM2PBES_H

#include "mcrl2/lps/io.h"
#include "mcrl2/pbes/algorithms.h"
#include "mcrl2/pbes/io.h"
#include "mcrl2/pbes/bisimulation.h"
#include "mcrl2/pbes/bisimulation_type.h"

namespace mcrl2 {

namespace pbes_system {

void lpsbisim2pbes(const std::string& input_filename1,
                   const std::string& input_filename2,
                   const std::string& output_filename,
                   const utilities::file_format* output_format,
                   bisimulation_type type,
                   bool normalize
                  )
{
  lps::specification M;
  lps::specification S;

  load_lps(M, input_filename1);
  load_lps(S, input_filename2);
  pbes result;
  switch (type)
  {
    case strong_bisim:
      result = strong_bisimulation(M, S);
      break;
    case weak_bisim:
      result = weak_bisimulation(M, S);
      break;
    case branching_bisim:
      result = branching_bisimulation(M, S);
      break;
    case branching_bisim_gw:
      // does not really use the Groote/Wijs algorithm
      result = branching_bisimulation(M, S);
      break;
    case branching_bisim_gjkw:
      // does not really use the Groote/Jansen/Keiren/Wijs algorithm
      result = branching_bisimulation(M, S);
      break;
    case branching_sim:
      result = branching_simulation_equivalence(M, S);
      break;
  }
  if (normalize)
  {
    algorithms::normalize(result);
  }
  save_pbes(result, output_filename, output_format);
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_TOOLS_LPSBISIM2PBES_H
