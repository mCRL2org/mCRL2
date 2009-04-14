//C++
#include <ostream>
#include <exception>
#include <iostream>
#include <fstream>
#include <string>
#include <utility>


//LPS-Framework
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/pbes_expression.h"
#include "mcrl2/pbes/utility.h"


//Rewriter
#include "mcrl2/new_data/rewrite.h"

using namespace mcrl2::new_data;
using namespace mcrl2::lps;
using namespace mcrl2::pbes_system;
using namespace mcrl2::pbes_system::pbes_expr;

int main(int argc, char** argv)
{
  MCRL2_ATERMPP_INIT(argc, argv)

  // load a pbes.
  pbes<> pbes_spec;
  try
  {
    pbes_spec.load("abp.pbes");
  }
  catch (mcrl2::runtime_error e)
  { std::cerr << "Cannot load abp.pbes\n";
    exit(1);
  }

  // initialize a rewriter
  Rewriter *rewriter = createRewriter(pbes_spec.data(),GS_REWR_JITTY);

  propositional_variable_instantiation i=pbes_spec.initial_state();

  std::cerr << "Initial variable: " << i << "\n";

  pbes_expression e=give_the_instantiated_rhs(i,pbes_spec,rewriter);

  std::cerr << "RHS: " << e << "\n";

  if (is_and(e))
  { std::cerr << "Is an AND with RHS: " << rhs(e) << "\n";
  }
  if (is_or(e))
  { std::cerr << "Is an OR with RHS: " << rhs(e) << "\n";
  }
  if (is_propositional_variable_instantiation(e))
  { std::cerr << "Is a VARIABLE: \n";
  }
  if (is_true(e))
  { std::cerr << "Is TRUE: \n";
  }
}

