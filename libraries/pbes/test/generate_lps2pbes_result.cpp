// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file generate_lps2pbes_result.cpp
/// \brief Add your file description here.

// N.B. This program can handle only one file at a time, due to limitations
// in the toolset.

#include <iostream>
#include <string>
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/detail/tools.h"
#include "mcrl2/lps/mcrl22lps.h"
#include "mcrl2/modal_formula/detail/read_text.h"

using namespace atermpp;
using namespace lps;
using namespace lps::detail;

int main(int argc, char** argv)
{
  MCRL2_CORE_LIBRARY_INIT(argv)

  if (argc < 4)
  {
    std::cerr << "Usage: " << argv[0] << " specification_file formula_file (timed/untimed)" << std::endl;
    return 1;
  }

  std::string specification_file = argv[1];
  std::string spec = read_text(specification_file, true);

  std::string formula_file = argv[2];
  std::string formula = read_text(formula_file, true);

  bool timed = (std::string(argv[3]) == "timed");

  std::string result_file;

  try
  {
    if (timed)
    {
      std::cout << formula_file << "[timed] ";
      pbes<> p = lps2pbes(spec, formula, true);
      pbes_equation_list eqn(p.equations().begin(), p.equations().end());
      // std::cout << pp(eqn) << std::endl;
      result_file = formula_file.substr(0, formula_file.size() - 4) + "expected_timed_result";
      p.save(result_file);
    }
    else
    {
      cout << formula_file << "[untimed] ";
      pbes<> p = lps2pbes(spec, formula, false);
      pbes_equation_list eqn(p.equations().begin(), p.equations().end());
      // std::cout << pp(eqn) << std::endl;
      result_file = formula_file.substr(0, formula_file.size() - 4) + "expected_untimed_result";
      p.save(result_file);
    }
  }
  catch (std::runtime_error e)
  {
    std::cerr << e.what() << std::endl;
  }     
  
  return 0;
}
