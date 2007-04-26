#include "gauss.h"
#include "lps/pbes_utility.h"
#include "atermpp/substitute.h"

#include "libprint_c.h" // contains gs..Msg family


using namespace lps;
using namespace pbes_expr;





//======================================================================
// Tries to solve the pbes by solving the predicate variables one by one,
// starting with the one defined by the last equation.

equation_system solve_pbes(pbes pbes_spec, bool interactive, int bound)
//========================

{
  gsDebugMsg("solve_pbes: start");
  
  equation_system es_problem = pbes_spec.equations();
  equation_system es_solution;
  // Better to transform the input pbes instead of making a new one??

  // Is using a reverse_iterator correct? inefficient??
  for (equation_system::reverse_iterator eqcrt = 
	 es_problem.rbegin(); eqcrt != es_problem.rend(); eqcrt++)
    {
      // replace already solved variables with their solutions
      update_equation(*eqcrt, es_solution);

      // solve this equation
      pbes_equation e_solution = solve_equation(*eqcrt, interactive, bound);
      
      // add solution of this equation to the already known solutions
      es_solution = equation_system(e_solution) + es_solution;
    }
  return es_solution;
}
//======================================================================





//======================================================================
// Tries to solve equation e.
// If succesful, the result will be an equation without
// e's predicate variable on the right hand side.
// The approximation process will stop within _bound_ steps 
// (but if _bound_=0, then it will continue indefinetely).
// If _interactive_ is turned on, then, after _bound_ approximation steps,
// the control will be given to the user.

pbes_equation solve_equation(pbes_equation e, bool interactive, int bound)
//==========================

{
  gsDebugMsg("solve_equation:start");
  
  pbes_expression defX = e.formula();
  propositional_variable X = e.variable();
  pbes_expression approx;
  pbes_expression approx_;
  bool stable = false;
  int no_iterations = 0;
  
  // init approx
  if (e.symbol().is_mu()) approx = false_(); else approx = true_();
  
  // iteration
  while ( (!stable) && (bound - no_iterations != 0) )
    {
      // substitute approx for X in defX, store result in approx_
      approx_ = defX;
      substitute(approx_, X, approx);
      
      // rewrite approx_...
      rewrite_pbes_expression(approx);
      
      // decide whether it's stable
      stable = (approx_ == approx);
      
      // continue
      approx = approx_;
      bound++;
    }
 
  // interactive behaviour
  if (interactive) solve_equation_interactive(X,defX,approx);

  // make solution and return it
  return pbes_equation(e.symbol(), X, defX);
}
//======================================================================







//======================================================================
// Replaces, in expr, X with solX

void substitute(pbes_expression expr, 
		propositional_variable X, pbes_expression solX)
//=============

{
  // To implement.
  // -use already defined substitution functions
  // -resolve harmful name clashes
}
//======================================================================





//======================================================================
// Replaces, in the RHS of e,
// all occurences of binding variables from es_solution
// with their solutions (i.e., corresponding equations from es_solution)

void update_equation(pbes_equation e, equation_system es_solution)
//==================

{
  // To implement.
  // Simultaneous substitution possible? maybe more efficient??
}
//======================================================================




//======================================================================
// Rewrites (simplifies) e according to the
// rewriting rules of first-order logic.

void rewrite_pbes_expression(pbes_expression e)
//==========================

{
  // To implement.
  // Call to external rewriters??

}
//======================================================================



//======================================================================
void solve_equation_interactive(propositional_variable X, 
			   pbes_expression defX, 
			   pbes_expression approx)
{
  // To implement later. 
}
//======================================================================
