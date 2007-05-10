#include <string>

#include "lps/pbes.h"
#include "lps/pbes_expression.h"
#include "librewrite.h" // rewriter
#include "prover/bdd_prover.h" // prover


#define PREDVAR_MARK '$'

// TO DO: make solve_pbes a class with interactive and bound as attributes..

using namespace lps;


equation_system solve_pbes(pbes pbes_spec, bool interactive, int bound);

pbes_equation solve_equation(pbes_equation e, bool interactive, int bound, Rewriter* r,BDD_Prover* p);

pbes_expression substitute(pbes_expression expr, 
			     propositional_variable X, pbes_expression solX);

void update_equation(pbes_equation e, equation_system es_solution);

pbes_expression rewrite_pbes_expression(pbes_expression e, Rewriter* r);
  

void solve_equation_interactive(propositional_variable X, 
				pbes_expression defX, 
				pbes_expression approx);
