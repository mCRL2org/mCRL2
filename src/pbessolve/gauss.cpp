
#include "gauss.h"
#include "lps/pbes_utility.h"
#include "atermpp/substitute.h"

#include "lps/pretty_print.h"
#include "libprint_c.h" // contains gs..Msg family




#include "atermpp/algorithm.h"     // replace
#include "atermpp/make_list.h"
#include "lps/data.h"
#include "lps/data_functional.h"
#include "lps/specification.h"

#include "librewrite.h" // rewriter


using namespace lps;
using namespace pbes_expr;

Rewriter *rewriter;


//======================================================================
// Tries to solve the pbes by solving the predicate variables one by one,
// starting with the one defined by the last equation.

equation_system solve_pbes(pbes pbes_spec, bool interactive, int bound)
//========================

{
 gsVerboseMsg("solve_pbes: start\n");
 
 equation_system es_problem = pbes_spec.equations();
 equation_system es_solution;
 
 rewriter = createRewriter(pbes_spec.data());

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
 
 delete rewriter; // ok??
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
 gsVerboseMsg("solve_equation:start\n");
 
 pbes_expression defX = e.formula();
 propositional_variable X = e.variable();
 pbes_expression approx;
 pbes_expression approx_;
 bool stable = false;
 int no_iterations;
 
 // init approx
 if (e.symbol().is_mu()) approx = false_(); else approx = true_();
 
 no_iterations = (bound==0);
 // iteration
 while ( (!stable) && (bound - no_iterations != 0) )
	{
	 gsVerboseMsg("\n=====\nApproximation %d: %s\n=====\n",
				 no_iterations,pp(approx).c_str());
   
	 // substitute approx for X in defX, store result in approx_
	 approx_ = defX;
	 approx_ = substitute(approx_, X, approx);  
	 
	 // rewrite approx_...
	 rewrite_pbes_expression(approx_);
   
	 // decide whether it's stable. TODO: rewrite pbes to eqbdd normal forms.
	 stable = (approx_ == approx);
   
	 // continue
	 approx = approx_;
	 no_iterations++;
	}
 
 // interactive behaviour
 if (interactive) solve_equation_interactive(X,defX,approx);
 
 // make solution and return it
 return pbes_equation(e.symbol(), X, defX);
}
//======================================================================






//======================================================================
// Replaces, in solX, all occurrences of variables 
// from generic_parameters with the corresponding data expressions 
// from actual_parameters
static pbes_expression pbes_expression_instantiate(pbes_expression solX, 
						   data_variable_list generic_parameters, 
						   data_expression_list actual_parameters)
//================================================
{
 gsVerboseMsg("PBES_EXPRESSION_INSTANTIATE\n");
 gsVerboseMsg("  solX: %s\n  generic parameters: %s\n  actual parameters:  %s\n",
						pp(solX).c_str(), pp(generic_parameters).c_str(), 
						pp(actual_parameters).c_str());
 
 pbes_expression result = 
	solX.substitute(make_list_substitution(generic_parameters, actual_parameters));
 
 gsVerboseMsg("  Result: %s\n", pp(result).c_str());
 
 return result;
} 
//======================================================================





//======================================================================
// Replaces, in expr, every X(param) with solX(param).
// !! It doesn't resolve name clashes, since they
// do not occur in the current usage of this function.
// ?? no return pbes_expression needed
// Innefficient. To Rewrite.
pbes_expression substitute(pbes_expression expr, 
		       propositional_variable X, pbes_expression solX)
//=============
{
 gsVerboseMsg("SUBSTITUTE in %s\n",pp(expr).c_str());
 
 if (is_and(expr))	 
	//substitute in lhs and rhs
	return(and_(substitute(lhs(expr),X,solX), substitute(rhs(expr),X,solX)));	
 else if (is_or(expr)) 
	//substitute in lhs and rhs
	return(or_(substitute(lhs(expr),X,solX), substitute(rhs(expr),X,solX)));
 else if (is_forall(expr))
	// substitute in expression under quantifier
	return (forall(quant_vars(expr), substitute(quant_expr(expr),X,solX)));
 else if (is_exists(expr))
	// substitute in expression under quantifier
	return (exists(quant_vars(expr), substitute(quant_expr(expr),X,solX)));
 else if (is_propositional_variable_instantiation(expr))
	// expr is a predicate variable.
	// If it's X, instantiate solX with the right parameters,
	// then perform substitution
	{ 
	 if (((propositional_variable_instantiation)(expr)).name() == X.name())
		{
		 propositional_variable_instantiation e = expr;
		 data_expression_list param = e.parameters();
		 return pbes_expression_instantiate(solX, 
																				X.parameters(), 
																				param); 
		}
	}
 //  else // expr is true, false or data
 gsVerboseMsg(" Return T/F/data: %s\n",pp(expr).c_str());
 return expr; 

 // TO DO later: resolve harmful name clashes
 // Now not needed, since the following theorem holds:
 // if a prop. variable y is bounded in the pbes_equation E,
 // it is bounded in all approximations of E.
 // (So, subtituting an approx. of E in E does not introduce name clashes).
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
// This is a customized variant of pbes2data from pbes_utility.h
// All operators become data operators.
// Predicate variables become data variables, with 
// the same arguments but a marked name (+PREDVAR_MARK at the end)
static data_expression pbes_to_data(pbes_expression e)
{
 using namespace pbes_expr;
 namespace dname = lps::data_expr;
 namespace sname = lps::sort_expr;
 
 if (is_data(e)) return data_expression(aterm_appl(e));
 else if (is_true(e)) return dname::true_();
 else if(is_false(e)) return dname::false_();
 else if (is_and(e))
	return dname::and_(pbes_to_data(lhs(e)),pbes_to_data(rhs(e)));
 else if (is_or(e)) 
	return dname::or_(pbes_to_data(lhs(e)),pbes_to_data(rhs(e)));
 else if (is_forall(e))
	{
	 aterm_appl x = gsMakeBinder(gsMakeForall(), 
															 quant_vars(e), 
															 pbes_to_data(quant_expr(e)));
	 return data_expression(x);
		//implement_data_data_expr(x,spec); 
		// (if implement, then there is no way back!)
  } 
 else if (is_exists(e)) 
	{
	 aterm_appl x = gsMakeBinder(gsMakeExists(), 
															 quant_vars(e), 
															 pbes_to_data(quant_expr(e)));
	 return data_expression(x);
  } 
 else if (is_propositional_variable_instantiation(e)) 
	{
	 identifier_string vname = var_name(e)+PREDVAR_MARK;
	 data_expression_list parameters = var_val(e);
	 sort_list sorts = apply(parameters, gsGetSort);
	 lps::sort vsort = gsMakeSortArrowList(sorts, sname::bool_());
	 data_variable v = data_variable(gsMakeDataVarId(vname, vsort));
	 return gsMakeDataApplList(v, parameters);
  }
 return data_expression(); // to prevent compiler warnings
}

//======================================================================






//======================================================================
// This is a customized variant of data2pbes from pbes_utility.h
// It keeps the logical operators in the data world,
// unless there is an obvious need to translate them to pbes operators.

static pbes_expression data_to_pbes_lazy(data_expression d)
{
 
 // if d doesn't contain any predicate variables, 
 // leave it as it is
 if ( d.to_string().find(PREDVAR_MARK) == std::string::npos )
	return val(d);
 
 namespace dname = lps::data_expr;
 namespace pname = lps::pbes_expr;
 namespace sname = lps::sort_expr;
 
 gsVerboseMsg("Data_to_pbes_lazy: Head is %s, args are %s\n", 
							pp(d.head()).c_str(), pp(d.arguments()).c_str());
 
 if (is_data_variable(d.head()))
	// d is either a predicate or a data variable
	{
	 std::string varname = data_variable(d.head()).name();
	 if (varname.at(varname.size()-1) == PREDVAR_MARK)
		return 
		 propositional_variable_instantiation
		 (identifier_string(varname.substr(0,varname.size()-1)), d.arguments());
	 else
		return val(d);
	}
 else if (dname::is_true(d)) return pname::true_();
 else if (dname::is_false(d)) return pname::false_();
 else if (dname::is_and(d)) 
	return pname::and_(data_to_pbes_lazy(arg1(d)), data_to_pbes_lazy(arg2(d)));
 else if (dname::is_or(d)) 
	return pname::or_(data_to_pbes_lazy(arg1(d)), data_to_pbes_lazy(arg2(d)));
 gsErrorMsg("cannot translate this: %s\n",pp(d).c_str());
 return pbes_expression(); // to prevent compiler warnings
}
//======================================================================




//======================================================================
// This translates as much as possible of the logical operators
// to pbes operators. Unfinished.

static pbes_expression data_to_pbes_greedy(data_expression d)
{
 
 namespace dname = lps::data_expr;
 namespace pname = lps::pbes_expr;
 namespace sname = lps::sort_expr;
 
 data_expression head = d.head();
 if (is_data_variable(head))
	// d is either a predicate or a data variable
	{
	 std::string varname = data_variable(head).name();
	 if (varname.at(varname.size()-1) == PREDVAR_MARK)
		return 
			propositional_variable_instantiation
		 (identifier_string(varname.substr(0,varname.size()-1)), d.arguments());
	 else
		return val(d);
    }
 else if (dname::is_true(d)) return pname::true_();
 else if (dname::is_false(d)) return pname::false_();
 else if (dname::is_and(d)) 
	return pname::and_(data_to_pbes_greedy(arg1(d)), 
										 data_to_pbes_greedy(arg2(d)));
 else if (dname::is_or(d)) 
	return pname::and_(data_to_pbes_greedy(arg1(d)), 
										 data_to_pbes_greedy(arg2(d)));
 
 // if none of the above, then it is a pure data expression
 
 return val(d);
}
//======================================================================













//======================================================================
// Rewrites (simplifies) e according to the
// rewriting rules of first-order logic.

pbes_expression rewrite_pbes_expression(pbes_expression e)
//=====================================

{
 gsVerboseMsg("REWRITE_PBES_EXPRESSION %s\n", pp(e).c_str());
 
 // translate e to a data_expression
 gsVerboseMsg(" ->pbes_to_data: %s\n",pp(e).c_str());
 data_expression de = pbes_to_data(e); 
 gsVerboseMsg(" <-pbes_to_data: %s\n",pp(de).c_str());
 
 // rewrite it with Muck's data rewriter
 gsVerboseMsg(" ->rewriter: %s\n",pp(de).c_str());
 data_expression d = rewriter->rewrite(de);
 gsVerboseMsg(" <-rewriter: %s\n",pp(d).c_str());

 // translate back to a pbes_expression
 gsVerboseMsg(" ->data_to_pbes %s\n",pp(d).c_str());
 e = data_to_pbes_lazy(d);
 gsVerboseMsg(" <-data_to_pbes %s\n",pp(e).c_str());
 
 return e;
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





