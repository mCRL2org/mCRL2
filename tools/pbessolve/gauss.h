#include <string>

#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/pbes_expression.h"
#include "librewrite.h" // rewriter
#include "prover/bdd_prover.h" // prover
#include "print/messaging.h"

#define PREDVAR_MARK '$'

// TO DO: make solve_pbes a class with interactive and bound as attributes..

using namespace lps;


equation_system solve_pbes(pbes pbes_spec, bool interactive, int bound);

pbes_equation solve_equation(pbes_equation e, 
			     bool interactive, int bound, BDD_Prover* p);

pbes_expression substitute(pbes_expression expr, 
			   propositional_variable X, pbes_expression solX);

pbes_expression update_expression(pbes_expression e, equation_system es_solution);

pbes_expression rewrite_pbes_expression(pbes_expression e, BDD_Prover* p);


void solve_equation_interactive(propositional_variable X, 
				pbes_expression defX, 
				pbes_expression approx);



// class that takes care of enumerating finite sorts
// i.e., producing all possible instantiations
// for a given sort, or set of sorts

typedef struct s_del {
  sort s;
  data_expression_list del;
} t_sdel;
  
  
class sort_instantiator {
 private:
  std::vector<t_sdel> instantiated_sorts;
  function_list fl;
 public:
  sort_instantiator() {};
  void set_function_list(function_list flist);
  void instantiate_sorts(lps::sort_list sl);
  data_expression_list get_enumeration(lps::sort s);    
  bool is_finite(sort s);
} ;

