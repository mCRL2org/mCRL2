#include <string>

#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/pbes_expression.h"
#include "librewrite.h" // rewriter
#include "prover/bdd_prover.h" // prover


#define PREDVAR_MARK '$'

// TO DO: make solve_pbes a class with interactive and bound as attributes..

using namespace lps;


equation_system solve_pbes(pbes pbes_spec, bool interactive, int bound);

pbes_equation solve_equation(pbes_equation e, bool interactive, int bound, Rewriter* r,BDD_Prover* p);

pbes_expression substitute(pbes_expression expr, 
			     propositional_variable X, pbes_expression solX);

pbes_expression update_expression(pbes_expression e, equation_system es_solution);

pbes_expression rewrite_pbes_expression(pbes_expression e, BDD_Prover* p);
  

void solve_equation_interactive(propositional_variable X, 
				pbes_expression defX, 
				pbes_expression approx);

//bool operator==(data_expression d, data_expression e);
//bool operator==(pbes_expression p, pbes_expression q);
