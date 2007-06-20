
#include "gauss.h"
#include "mcrl2/pbes/utility.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/data/sort_utility.h"
#include "atermpp/substitute.h"
#include "mcrl2/basic/pretty_print.h"


#include "atermpp/algorithm.h"     // replace
#include "atermpp/make_list.h"
#include "mcrl2/data/data.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/lps/specification.h"
#include "print/messaging.h"

//#define debug


using namespace lps;
using namespace pbes_expr;
using namespace mcrl2::utilities;


 bool pbes_expression_compare
(pbes_expression p, pbes_expression q, BDD_Prover *prover);


pbes_expression pbes_expression_simplify
(pbes_expression p, int *nq, data_variable_list *fv ,
 BDD_Prover *prover);

pbes_expression enumerate_finite_domains
(bool forall, data_variable_list *quant_vars, pbes_expression p, BDD_Prover *prover);

data_expression data_expression_simplify
(data_expression d, int *nq, data_variable_list *fv, BDD_Prover *prover);


// some extra needed functions on data_variable_lists and data_expressions
 bool var_in_list(data_variable vx, data_variable_list y);
 data_variable_list intersect(data_variable_list x, data_variable_list y);
 data_variable_list substract(data_variable_list x, data_variable_list y);
 data_variable_list dunion(data_variable_list x, data_variable_list y);
 void dunion(data_variable_list *x, data_variable_list y);

void free_vars_and_no_quants(data_expression d, int* nq, data_variable_list *fv);


sort_instantiator si;


//======================================================================
// Tries to solve the pbes by solving the predicate variables one by one,
// starting with the one defined by the last equation.

equation_system solve_pbes(pbes pbes_spec, bool interactive, int bound)
//========================

{
 equation_system es_problem = pbes_spec.equations();
 equation_system es_solution;
 
 Rewriter* rewriter = createRewriter(pbes_spec.data(), GS_REWR_INNER);
 // addrule__b_and_not_b(rewriter);

 lps::data_specification ds = pbes_spec.data();
 SMT_Solver_Type sol = solver_type_cvc_lite_fast;
 BDD_Prover* prover = new BDD_Prover(ds, GS_REWR_INNER, 0, false, sol, false);


 // instantiate all finite sorts from the specification
 // (better: instantiate later, when needed...)
 sort_list finite_sorts;
 sort_list sl = ds.sorts();
 si.set_function_list(ds.constructors());
 for (sort_list::iterator i = sl.begin(); i != sl.end(); i++)
   if (is_finite(ds.constructors(), (*i)))
     finite_sorts = push_front(finite_sorts,(*i));
 si.instantiate_sorts(finite_sorts);

#ifdef debug
 gsVerboseMsg("SORT_INSTANTIATOR check!\n");
 for (sort_list::iterator i = finite_sorts.begin(); i != finite_sorts.end(); i++)
   {
     gsVerboseMsg("sort %s:  enumeration %s\n",
		  pp(*i).c_str(), pp(si.get_enumeration(*i)).c_str());
   } 
#endif




#ifdef debug
 gsVerboseMsg("starting loop\n"); 
#endif

 // Is using a reverse_iterator correct? inefficient??
 for (equation_system::reverse_iterator eqcrt = 
			 es_problem.rbegin(); eqcrt != es_problem.rend(); eqcrt++)
	{
//check!
//gsVerboseMsg("checking");gsVerboseMsg(" %s\n",pp(eqcrt->formula()).c_str());
//if (detail::check_rule_PBExpr(eqcrt->formula()))	
//gsVerboseMsg("checked!\n");

	 // replace already solved variables with their solutions
	 pbes_expression newrhs = update_expression(eqcrt->formula(), es_solution);
	 *eqcrt = pbes_equation(eqcrt->symbol(), eqcrt->variable(), newrhs);
	 gsVerboseMsg("Equation to solve: %s\n\n",pp(*eqcrt).c_str());
	 
	 // solve this equation
	 pbes_equation e_solution = 
	   solve_equation(*eqcrt, interactive, bound, prover);
   
	 // add solution of this equation to the already known solutions
	 es_solution = equation_system(e_solution) + es_solution;
	}
 
 delete rewriter; 
 // ~BDD_Prover(prover);

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

pbes_equation solve_equation
(pbes_equation e, 
 bool interactive, int bound, BDD_Prover* prover)
//==========================
 
{
#ifdef debug
 gsVerboseMsg("solve_equation:start\n");
#endif
 
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
	 gsVerboseMsg("\n\n=====\nApproximation %d: %s\n=====\n\n",
				 no_iterations,pp(approx).c_str());
   
	 // substitute approx for X in defX, store result in approx_
	 //	 	 gsVerboseMsg("SUBSTITUTE in %s\n",pp(defX).c_str());
	 approx_ = defX;
	 approx_ = substitute(approx_, X, approx);  
#ifdef debug
	 gsVerboseMsg("SUBSTITUTE result %s\n\n",pp(approx_).c_str());	 	 
#endif

	 // rewrite approx_...
	 //	 approx_ = rewrite_pbes_expression(approx_,rewriter,prover);
	 int nq = 0;
	 data_variable_list fv;
	 approx_ = pbes_expression_simplify(approx_, &nq, &fv, prover);
	 
	 if (nq == 0)
	   approx_ = rewrite_pbes_expression(approx_,prover);

#ifdef debug
	 gsVerboseMsg("\n\nREWRITTEN: %s\n%d quantifiers left\n",pp(approx_).c_str(),nq);
#endif

	 // decide whether it's stable.
	 //	 stable = pbes_expression_compare(approx_,approx,prover);
	 stable = (approx_ == approx);

#ifdef debug
	 if (!stable) gsVerboseMsg("Not"); gsVerboseMsg(" stable\n");
#endif

	 // continue
	 approx = approx_;
	 no_iterations++;
	}
 
 if (stable) gsVerboseMsg("Stabilized (in %d iterations) at %s\n\n", no_iterations-1, pp(approx).c_str());
 // interactive behaviour
 if (interactive) solve_equation_interactive(X,defX,approx);
 
 // make solution and return it
 return pbes_equation(e.symbol(), X, approx);
}
//======================================================================






//======================================================================
// Replaces, in solX, all occurrences of variables 
// from generic_parameters with the corresponding data expressions 
// from actual_parameters
 pbes_expression pbes_expression_instantiate(pbes_expression solX, 
						   data_variable_list generic_parameters, 
						   data_expression_list actual_parameters)
//================================================
{

#ifdef debug
 gsVerboseMsg("\n\nPBES_EXPRESSION_INSTANTIATE\n");
 gsVerboseMsg("  solX: %s\n  generic parameters: %s\n  actual parameters:  %s\n",
						pp(solX).c_str(), pp(generic_parameters).c_str(), 
						pp(actual_parameters).c_str());
#endif

 pbes_expression result = 
	solX.substitute(make_list_substitution(generic_parameters, actual_parameters));

#ifdef debug 
 gsVerboseMsg("  Result: %s\n\n\n", pp(result).c_str());
#endif

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
 return expr; 

 // TO DO later: resolve harmful name clashes
 // Now not needed, since the following theorem holds:
 // if a prop. variable y is bounded in the pbes_equation E,
 // it is bounded in all approximations of E.
 // (So, subtituting an approx. of E in E does not introduce name clashes).
}
//======================================================================





//======================================================================
// Replaces, in e,
// all occurences of binding variables from es_solution
// with their solutions (i.e., corresponding equations from es_solution)

pbes_expression update_expression(pbes_expression e, equation_system es_solution)
//==================

{

 // !!todo: implement simultaneous substitutions, probably more efficient 
 pbes_expression ee = e;

#ifdef debug
 gsVerboseMsg("update_expression.\n\n");
#endif

 for (equation_system::reverse_iterator s = 
			 es_solution.rbegin(); s != es_solution.rend(); s++)	
{
  //gsVerboseMsg("update_expression: substituting equation %s\n",pp(*s).c_str());
	ee = substitute(ee, s->variable(), s->formula());
	//gsVerboseMsg("update_expression: result is %s\n",pp(ee).c_str());
 }
 return ee; 
}
//======================================================================






//======================================================================
// This is a customized variant of pbes2data from pbes_utility.h
// All operators become data operators.
// Predicate variables become data variables, with 
// the same arguments but a marked name (+PREDVAR_MARK at the end)
// It doesn't work (WHY not??) for quantifiers.
 data_expression pbes_to_data(pbes_expression e)
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
	 aterm_appl x = gsMakeBinder(gsMakeForall(),quant_vars(e),pbes_to_data(quant_expr(e)));
	 return (gsMakeDataExprExists(x));
		//implement_data_data_expr(x,spec); 
		// (if implement, then there is no way back!)
  } 
 else if (is_exists(e)) 
	{
	 aterm_appl x = gsMakeBinder(gsMakeExists(),quant_vars(e),pbes_to_data(quant_expr(e)));
	 return (gsMakeDataExprExists(x));
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

 pbes_expression data_to_pbes_lazy(data_expression d)
{
 
 // if d doesn't contain any predicate variables, 
 // leave it as it is
 if ( d.to_string().find(PREDVAR_MARK) == std::string::npos )
	return val(d);
 
 namespace dname = lps::data_expr;
 namespace pname = lps::pbes_expr;
 namespace sname = lps::sort_expr;
 
#ifdef debug
 gsVerboseMsg("Data_to_pbes_lazy: Head is %s, args are %s\n", 
			pp(d.head()).c_str(), pp(d.arguments()).c_str());
#endif

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

 pbes_expression data_to_pbes_greedy(data_expression d)
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
// only works for quantifier-free expressions.
pbes_expression rewrite_pbes_expression(pbes_expression e, BDD_Prover* prover)
//=====================================

{

#ifdef debug
  gsVerboseMsg("REWRITE_PBES_EXPRESSION %s\n", pp(e).c_str());
  if (is_data(e)) gsVerboseMsg("data expression already!");
#endif

 // translate e to a data_expression
 //gsVerboseMsg(" ->pbes_to_data: %s\n",pp(e).c_str());
 data_expression de = pbes_to_data(e); 
 //gsVerboseMsg(" <-pbes_to_data: %s\n",pp(de).c_str());
 
 /* THIS DOESN'T SIMPLIFY ENOUGH
 // rewrite it with Muck's data rewriter
 gsVerboseMsg(" ->rewriter: %s\n",pp(de).c_str());
 data_expression d = rewriter->rewrite(de);
 gsVerboseMsg(" <-rewriter: %s\n",pp(d).c_str());
 */

 // simplify using the prover
 //gsVerboseMsg(" ->prover: %s\n",pp(de).c_str());
 prover->set_formula(de); 
 //gsVerboseMsg(" <--Bdd from prover: %P\n", prover->get_bdd());

 data_expression d = prover->get_bdd();
 //gsVerboseMsg(" <--simplifier: %s\n",pp(d).c_str());

 // translate back to a pbes_expression
 //gsVerboseMsg(" ->data_to_pbes %s\n",pp(d).c_str());
 e = data_to_pbes_lazy(d);
 //gsVerboseMsg(" <-data_to_pbes %s\n",pp(e).c_str());
 //  if (is_data(e)) gsVerboseMsg("data expression!");
 
 return e;
}
//======================================================================




//======================================================================
// Rewrites (simplifies) e according to the
// rewriting rules of first-order logic.
// only works for quantifier-free expressions.
data_expression rewrite_data_expression(data_expression e, BDD_Prover* prover)
//=====================================

{
  //  gsVerboseMsg("REWRITE_DATA_EXPRESSION %s\n", pp(e).c_str());
 // simplify using the prover
 //gsVerboseMsg(" ->prover: %s\n",pp(de).c_str());
 prover->set_formula(e); 
 //gsVerboseMsg(" <--Bdd from prover: %P\n", prover->get_bdd());

 data_expression d = prover->get_bdd();
 
 return d;
}
//======================================================================







//======================================================================
// eliminates some quantifiers
// OUT nq  := the number of quantifiers left in expr after simplification
// OUT fv := the set of names of the data variables occuring FREE in expr, 
//        after simplification
 pbes_expression pbes_expression_simplify
 (pbes_expression expr, int* nq, data_variable_list *fv, 
  BDD_Prover* prover)
{
#ifdef debug
  gsVerboseMsg("\n\n\nPBES_EXPRESSION_SIMPLIFY start:%s\n",pp(expr).c_str());
#endif
  *fv = data_variable_list();
  *nq = 0;
  pbes_expression expr_simplified;
  if (is_and(expr))
    {
      // simplify left and right
      int nqlhs, nqrhs;
      data_variable_list fvlhs,fvrhs;
      pbes_expression slhs = pbes_expression_simplify(lhs(expr),&nqlhs,&fvlhs,prover);
      pbes_expression srhs = pbes_expression_simplify(rhs(expr),&nqrhs,&fvrhs,prover);
      *nq = nqlhs + nqrhs;
      *fv = dunion(fvlhs,fvrhs);
      expr_simplified = and_(slhs,srhs);	
    }
  else if (is_or(expr)) 
    {
      // simplify left and right
      int nqlhs, nqrhs;
      data_variable_list fvlhs,fvrhs;
      pbes_expression slhs = pbes_expression_simplify(lhs(expr),&nqlhs,&fvlhs,prover);
      pbes_expression srhs = pbes_expression_simplify(rhs(expr),&nqrhs,&fvrhs,prover);
      *nq = nqlhs + nqrhs;
      *fv = dunion(fvlhs,fvrhs);     
      expr_simplified = or_(slhs,srhs);	
    }      
  else if (is_forall(expr) || is_exists(expr))
    {
      // simlify under quantifier
      int nq_under;
      data_variable_list fv_under;
      pbes_expression s_under = 
	pbes_expression_simplify(quant_expr(expr),&nq_under,&fv_under, prover);
      // dit heeft waarschijnlijk geen zin:
      //      if (nq_under==0)
      //s_under = rewrite_pbes_expression(s_under,prover);

      // compute the list of actually bounded variables 
      // (i.e., eliminate from the quant_vars those vars that do not occur free in s_under)
      data_variable_list new_quant_vars = intersect(quant_vars(expr),fv_under);
      // if any quantified vars left, try to eliminate them by enumeration
      if (!new_quant_vars.empty()){
	if (is_forall(expr)) 
	  s_under = enumerate_finite_domains(true, &new_quant_vars, s_under, prover);
	else
	  s_under = enumerate_finite_domains(false, &new_quant_vars, s_under, prover);     
#ifdef debug
	gsVerboseMsg("PBES_EXPRESSION_SIMPLIFY: result of enumerate_finite_domains is %s\n",
		     pp(s_under).c_str());
#endif
      }
      // if any quantified vars left, keep them
      if (!new_quant_vars.empty()) 
	{
	  *nq = nq_under + 1;
	  if (is_forall(expr))
	    expr_simplified = forall(new_quant_vars, s_under);
	  else 
	    expr_simplified = exists(new_quant_vars, s_under);
	}
      // else, lose the quantifier
      else
	{
	  *nq = nq_under;
	  expr_simplified = s_under;
	}
    }
  else if (is_data(expr))
    {
      // Sending to the prover.
      // !! don't know what happens if expr contains quantifiers.
      expr_simplified = val(rewrite_data_expression
			    (data_expression(aterm_appl(expr)), prover));
      free_vars_and_no_quants(expr_simplified,nq,fv);
    }
  //  else // expr is true, false or a variable
  else
    {
#ifdef debug
      gsVerboseMsg("UNKNOWN pbes_expr: %s\n", pp(expr).c_str());
#endif
      expr_simplified = expr;
    }
#ifdef debug
  gsVerboseMsg("PBES_EXPRESSION_SIMPLIFY: end : %s\n %d quantifiers, free vars: %s\n\n\n",
	       pp(expr_simplified).c_str(),*nq, pp(*fv).c_str());
#endif

   return expr_simplified; 
}
//======================================================================




//======================================================================
void free_vars_and_no_quants(data_expression d, int* nq, data_variable_list *fv)
// fills in the number of quantifiers and the list of free vars in expression d  
//!! can be more efficient if we assume there are no quantifiers in data expressions
//======================================================================
{
  
  using namespace data_expr;
  
  data_expression head = d.head();
  data_expression_list args = d.arguments();
  
  //  gsVerboseMsg("FREE_VARS_AND_NO_QUANTS: data expr is %s, head is %s, args are %s\n",
  //       pp(d).c_str(),pp(head).c_str(),pp(args).c_str());
  
  // data variable?
  if (is_data_variable(d)) {
    *fv = push_back((*fv),(data_variable)head);
    *nq = 0;
  } 
  // quantifier?
  else if (gsIsBinder(head))
    {
      int nq_under;
      data_variable_list fv_under;
      data_variable_list qvars = list_arg1(head);
      free_vars_and_no_quants(arg2(d),&nq_under, &fv_under);
      *nq = nq_under + 1;
      *fv = substract(fv_under,qvars);
    }
  
  // number, function or operator?
  else if (!args.empty())
    {
      // collect free variables from all subexpressions
      *nq = 0;
      int nqsub;
      data_variable_list fvsub;
      while (!args.empty()){
	free_vars_and_no_quants(args.front(),&nqsub,&fvsub);
	args = pop_front(args);
	dunion(fv,fvsub);
	*nq = *nq + nqsub;
      }
      /*
      int nqlhs, nqrhs;
      data_variable_list fvlhs,fvrhs;
      free_vars_and_no_quants(args.front(),&nqlhs,&fvlhs);
      args = pop_front(args);
      if (!args.empty())
	free_vars_and_no_quants(args.front(),&nqrhs,&fvrhs);
      else nqrhs = 0;
      *nq = nqlhs + nqrhs;
      *fv = dunion(fvlhs,fvrhs);     
      */
    }

  // number, boolean
  else
    {
      *nq = 0;
    }
}


  








//======================================================================
bool var_in_list(data_variable vx, data_variable_list y)
//======================================================================
{
  // comparing only the variable name
  
 //  gsVerboseMsg("\n============= vx.name=%s, y=%s\n",pp(vx.name()).c_str(),pp(y).c_str());
  
  data_variable_list::iterator i = y.begin();
  for ( ; i != y.end(); i++)
    if (i->name() == vx.name()) break;
  return (i != y.end());
}



//======================================================================
data_variable_list intersect(data_variable_list x, data_variable_list y)
//======================================================================
{
  data_variable_list result;
  for (data_variable_list::iterator vx = x.begin(); vx != x.end(); vx++)
    if (var_in_list(*vx,y))
      result = push_back(result,*vx);
  return result;
}


//======================================================================
// disjoint union
 data_variable_list dunion(data_variable_list x, data_variable_list y)
//======================================================================
{
  data_variable_list result(y);
  for (data_variable_list::iterator vx = x.begin(); vx != x.end(); vx++)
    if (!var_in_list(*vx,y))
      result = push_back(result,*vx);    
  return result;
}

//======================================================================
// disjoint union
 void dunion(data_variable_list* x, data_variable_list y)
//======================================================================
{
  for (data_variable_list::iterator vy = y.begin(); vy != y.end(); vy++)
    if (!var_in_list(*vy,*x))
      (*x) = push_back(*x,*vy);    
}

//======================================================================
data_variable_list substract(data_variable_list x, data_variable_list y)
//======================================================================
{
  data_variable_list result;
  for (data_variable_list::iterator vx = x.begin(); vx != x.end(); vx++)
    if (!var_in_list(*vx,y))
      result = push_back(result,*vx);
  return result;
}





//======================================================================


//======================================================================
// !! Doesn't work when quantifiers are involved. 
// !! It seems difficult to properly translate pbes quantifiers to data quantifiers (??)
// !! The prover can't use quantifiers anyway.
bool pbes_expression_compare
(pbes_expression p, pbes_expression q, BDD_Prover* prover)
{
  // if no quantifiers and no predicates, call the prover
gsVerboseMsg("Comparing %s (%d) and %s (%d)\n",pp(p).c_str(),p.is_bes(),pp(q).c_str(),is_bes(q));
  if ((is_bes(p))&&(is_bes(q)))
    {
      data_expression dp = pbes_to_data(p);
      data_expression dq = pbes_to_data(q);
      ATermAppl formula = gsMakeDataExprEq((ATermAppl)dp,(ATermAppl)dq);
      
      //      gsVerboseMsg(" -->prover: %s\n",pp(formula).c_str());
      prover->set_formula(formula);  
      Answer a = prover->is_tautology();
      gsVerboseMsg(" <--prover: %s\n",((a==answer_yes)?"YES":((a==answer_no)?"NO":"DON'T KNOW")));  
      return ((a == answer_yes)?true:false);      
    }
  
  // else, improvise
  return false;
}
//======================================================================





//======================================================================
pbes_expression enumerate_rec(bool forall,
			      data_variable_list vars,
			      data_variable_list::iterator v,
			      data_expression_list instance,
			      pbes_expression p, 
			      BDD_Prover *prover)
{  
  if (v == vars.end()){
    // compute the new instance of p

#ifdef debug
    gsVerboseMsg("ENUMERATE_REC: new instance: %s\n",pp(instance).c_str());
#endif

    pbes_expression p_instance = pbes_expression_instantiate(p, vars, instance);    
    p_instance = val(rewrite_data_expression (data_expression(aterm_appl(p_instance)), 
					      prover));
#ifdef debug
    gsVerboseMsg("ENUMERATE_REC: rewritten: %s\n",pp(p_instance).c_str());
#endif

    return p_instance;
  }
  
  //  gsVerboseMsg("ENUMERATE_REC: processing variable %s\n",pp(v->name()).c_str());
  data_expression_list domain = 
    si.get_enumeration(v->sort());

#ifdef debug
  gsVerboseMsg("************ Sort %s instantiated as: %s\n",
	       pp(v->sort()).c_str(), pp(domain).c_str());
#endif
    
  pbes_expression p_here = forall ? true_() : false_();
  data_expression_list::iterator i = domain.begin();
  for (; i != domain.end(); i++)
    {
      //  gsVerboseMsg("---- %s = %s", pp(v->name()).c_str(), pp(*i).c_str());
      data_variable_list::iterator vv = v; vv++;
      pbes_expression p_i = enumerate_rec(forall,vars,vv,instance+(*i),p, prover);
      if (forall)
	{ // check whether we can stop immediately	  
	  if (is_false(p_i)) {
	    p_here = false_();
	    return p_here;
	  }
	  else {
	    pbes_expression p_here_copy = pbes_expression(p_here);
	    p_here = and_(p_here_copy,p_i);
	    //gsVerboseMsg(" A ");
	  }
	}
      else // it's an exist quantification 
	{ 
	  if (is_true(p_i)) {
	    p_here = true_();
	    return p_here;
	  }
	  else {
	    pbes_expression p_here_copy = pbes_expression(p_here);
	    p_here = or_(p_here_copy,p_i);
	    //gsVerboseMsg(" A ");
	  }
	}
    }
  // else, if this domain is empty
  return p_here;
}






//======================================================================
pbes_expression enumerate_finite_domains
(bool forall, data_variable_list *quant_vars, pbes_expression p, BDD_Prover *prover)
// - instantiate all finite domain variables from quant_vars
// with their domain's elements
// - return the \/ composition of the resulting p's
// - in the end, quant_vars will only contain the infinite-domain variables
{
  // make a separate list of finite domain variables
  data_variable_list finite_domain_vars;
  data_variable_list::iterator v = quant_vars->begin();
  for ( ; v != quant_vars->end(); v++)
    {
      lps::sort vsort = v->sort();
      
      //// gsVerboseMsg("enumerate_finite_domains::: v= %s, vsort= %s\n",pp(*v).c_str(), pp(vsort).c_str());
      
      if (si.is_finite(vsort))
	finite_domain_vars = push_back(finite_domain_vars, (*v));
    } 
  
  // eliminate the finite vars from the quant_vars list
  *quant_vars = substract((*quant_vars), finite_domain_vars);

#ifdef debug  
  gsVerboseMsg("ENUMERATE_FINITE_DOMAINS: finite vars %s, infinte vars %s\n\n", 
	       pp(finite_domain_vars).c_str(), pp(*quant_vars).c_str());
#endif  

  // instantiate the finite vars
  data_expression_list dl;
  return enumerate_rec(forall, finite_domain_vars, finite_domain_vars.begin(), 
		       dl,p, prover);
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




/*

//======================================================================
//  EXTRA REWRITE RULES
//======================================================================
 void addrule__b_and_not_b(Rewriter* rewriter){
  gsVerboseMsg("adding rule b && !b == false\n");  
  ATermAppl b = gsMakeDataVarId(gsString2ATermAppl("b"),gsMakeSortExprBool());
  ATermAppl lhs = gsMakeDataExprAnd(b,gsMakeDataExprNot(b));
  ATermAppl rule = gsMakeDataEqn(ATmakeList1((ATerm) b), 
				 gsMakeNil(), 
				 lhs, 
				 gsMakeDataExprFalse());
  if (rewriter->addRewriteRule(rule) == false)
    gsErrorMsg("could not add rewrite rule b&&!b==false");
  gsVerboseMsg("Added rule: %s\n",pp(rule).c_str());
}


//======================================================================

*/




// CLASS SORT_INSTANTIATOR


//======================================================================
// enumerates all possible data expressions constructed by sort s
data_expression_list instantiate_sort(function_list fl, lps::sort s)
{
  data_expression_list result;
  function_list constructors = get_constructors(fl,s);
    
  // enumerate all s-expressions generated by each constructor
  for (function_list::iterator c = constructors.begin(); c != constructors.end(); c++)
    {
      //get the domains of this constructor (=function)
      sort_list domains = c->sort().domain_sorts();
      
      data_expression_list domain_instance;
      data_expression dec = data_expression((aterm_appl)(*c));    
      // instantiate each domain, then apply the constructor c
      // to obtain something of sort s
      for (sort_list::iterator d = domains.begin(); d != domains.end(); d++)
	{
	  domain_instance = instantiate_sort(fl,*d);
	  gsVerboseMsg(".....instaniate_sort %s: constructor %s, domain %s, domain_instance %s\n", 
		       pp(s).c_str(), pp(*c).c_str(), pp(domains).c_str(), pp(domain_instance).c_str());
	  // apply c on domain_instance
	  for (data_expression_list::iterator i = domain_instance.begin(); 
	       i != domain_instance.end(); i++)
	    {	     
	      data_expression_list args;
	      args = push_front(args, *i);
	      result = 
		push_front(result,data_expression(data_application(dec,args) )); 
	    }
	}
      if (domains.empty())
	result = push_front(result,dec);
    }
  return result;
  
}


void sort_instantiator::set_function_list (function_list flist)
{
  fl = flist;
}

void sort_instantiator::instantiate_sorts (lps::sort_list sl)
{
  for (sort_list::iterator ss = sl.begin(); ss != sl.end(); ss++){
    t_sdel new_isort;
    new_isort.s = *ss; 
    new_isort.del = instantiate_sort(fl,*ss);
    instantiated_sorts.push_back(new_isort);
  }
}

data_expression_list sort_instantiator::get_enumeration (lps::sort ss)
{
  data_expression_list leeg;
  for (unsigned short i = 0 ; i < instantiated_sorts.size(); i++)
    if (instantiated_sorts[i].s == ss) 
      return instantiated_sorts[i].del;
  return leeg;
}

bool sort_instantiator::is_finite(sort s) 
{
  return lps::is_finite(fl,s);
};

// END CLASS   SORT_INSTANTIATOR

