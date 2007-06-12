
#include "gauss.h"
#include "lps/pbes_utility.h"
#include "atermpp/substitute.h"

#include "mcrl2/basic/pretty_print.h"
#include "libprint_c.h" // contains gs..Msg family


#include "atermpp/algorithm.h"     // replace
#include "atermpp/make_list.h"
#include "mcrl2/data/data.h"
#include "mcrl2/data/data_functional.h"
#include "mcrl2/lps/specification.h"



using namespace lps;
using namespace pbes_expr;


 void addrule__b_and_not_b(Rewriter *rewriter);

 bool pbes_expression_compare
(pbes_expression p, pbes_expression q, BDD_Prover *prover);


pbes_expression pbes_expression_simplify
(pbes_expression p, int *nq, data_variable_list *fv, BDD_Prover *prover);


data_expression data_expression_simplify
(data_expression d, int *nq, data_variable_list *fv, BDD_Prover *prover);


// some extra needed functions on data_variable_lists and data_expressions
 bool var_in_list(data_variable vx, data_variable_list y);
 data_variable_list intersect(data_variable_list x, data_variable_list y);
 data_variable_list substract(data_variable_list x, data_variable_list y);
 data_variable_list dunion(data_variable_list x, data_variable_list y);

void free_vars_and_no_quants(data_expression d, int* nq, data_variable_list *fv);



//======================================================================
// Tries to solve the pbes by solving the predicate variables one by one,
// starting with the one defined by the last equation.

equation_system solve_pbes(pbes pbes_spec, bool interactive, int bound)
//========================

{
 gsVerboseMsg("solve_pbes: start\n");
 
 equation_system es_problem = pbes_spec.equations();
 equation_system es_solution;
 
 Rewriter* rewriter = createRewriter(pbes_spec.data(), GS_REWR_INNER);
 // addrule__b_and_not_b(rewriter);

 lps::data_specification ds = pbes_spec.data();
 SMT_Solver_Type sol = solver_type_cvc_lite_fast;
 BDD_Prover* prover = new BDD_Prover(ds, GS_REWR_INNER, 0, false, sol, false);


gsVerboseMsg("starting loop\n"); 
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
	   solve_equation(*eqcrt, interactive, bound, rewriter, prover);
   
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
(pbes_equation e, bool interactive, int bound,Rewriter* rewriter,BDD_Prover* prover)
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
	 gsVerboseMsg("\n\n=====\nApproximation %d: %s\n=====\n\n",
				 no_iterations,pp(approx).c_str());
   
	 // substitute approx for X in defX, store result in approx_
	 //	 	 gsVerboseMsg("SUBSTITUTE in %s\n",pp(defX).c_str());
	 approx_ = defX;
	 approx_ = substitute(approx_, X, approx);  
	 gsVerboseMsg("SUBSTITUTE result %s\n",pp(approx_).c_str());	 	 

	 // rewrite approx_...
	 //	 approx_ = rewrite_pbes_expression(approx_,rewriter,prover);
	 int nq = 0;
	 data_variable_list fv;
	 approx_ = pbes_expression_simplify(approx_, &nq, &fv, prover);
	 
	 if (nq == 0)
	   approx_ = rewrite_pbes_expression(approx_,prover);
	 gsVerboseMsg("REWRITTEN: %s\n%d quantifiers left\n",pp(approx_).c_str(),nq);

	 // decide whether it's stable.
	 //	 stable = pbes_expression_compare(approx_,approx,prover);
	 stable = (approx_ == approx);
	 if (!stable) gsVerboseMsg("Not"); gsVerboseMsg(" stable\n");
	 // continue
	 approx = approx_;
	 no_iterations++;
	}
 
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
  /*
 gsVerboseMsg("PBES_EXPRESSION_INSTANTIATE\n");
 gsVerboseMsg("  solX: %s\n  generic parameters: %s\n  actual parameters:  %s\n",
						pp(solX).c_str(), pp(generic_parameters).c_str(), 
						pp(actual_parameters).c_str());
  */
 pbes_expression result = 
	solX.substitute(make_list_substitution(generic_parameters, actual_parameters));
 
// gsVerboseMsg("  Result: %s\n", pp(result).c_str());
 
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
gsVerboseMsg("update_expression: in expression %s\n",pp(e).c_str());
 for (equation_system::reverse_iterator s = 
			 es_solution.rbegin(); s != es_solution.rend(); s++)	
{
gsVerboseMsg("update_expression: substituting equation %s\n",pp(*s).c_str());
	ee = substitute(ee, s->variable(), s->formula());
gsVerboseMsg("update_expression: result is %s\n",pp(ee).c_str());
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

  gsVerboseMsg("REWRITE_PBES_EXPRESSION %s\n", pp(e).c_str());
  if (is_data(e)) gsVerboseMsg("data expression already!");


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
  if (is_data(e)) gsVerboseMsg("data expression!");
 
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
  gsVerboseMsg("REWRITE_DATA_EXPRESSION %s\n", pp(e).c_str());
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
(pbes_expression expr, int* nq, data_variable_list *fv, BDD_Prover* prover)
{
  gsVerboseMsg("PBES_EXPRESSION_SIMPLIFY %s\n",pp(expr).c_str());
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
	pbes_expression_simplify(quant_expr(expr),&nq_under,&fv_under,prover);
      if (nq_under==0)
	s_under = rewrite_pbes_expression(s_under,prover);
      // lose quantifier if variables are not free in s_under
      data_variable_list new_quant_vars = intersect(quant_vars(expr),fv_under);
      if (new_quant_vars.empty()) 
	{
	  *nq = nq_under;
	  expr_simplified = s_under;
	}
      // else, keep it
      else
	{
	  *nq = nq_under + 1;
	  if (is_forall(expr))
	    expr_simplified = forall(new_quant_vars, s_under);
	  else 
	    expr_simplified = exists(new_quant_vars, s_under);
	}
    }
  else if (is_data(expr))
    {
      // Sending to the prover.
      // !! don't know what happens if expr contains quantifiers.
      expr_simplified = val(rewrite_data_expression
			    (data_expression(aterm_appl(expr)), prover));
      free_vars_and_no_quants(expr,nq,fv);
    }
  //  else // expr is probably variable
  else
    {
      gsVerboseMsg("UNKNOWN pbes_expr: %s\n", pp(expr).c_str());
      expr_simplified = expr;
    }
  gsVerboseMsg("Simplified: %s\n%d free vars\n",pp(expr_simplified).c_str(),fv->size());
  return expr_simplified; 
}
//======================================================================




//======================================================================
void free_vars_and_no_quants(data_expression d, int* nq, data_variable_list *fv)
// fills in the number of quantifiers and the list of free vars in expression d  
//======================================================================
{
  data_expression head = d.head();
  data_expression_list args = d.arguments();
  
  gsVerboseMsg("FREE_VARS_AND_NO_QUANTS: head is %s, args are %s\n",pp(head).c_str(),pp(args).c_str());
  
  if (is_data_variable(head)) {
    *fv = push_back((*fv),(data_variable)head);
    *nq = 0;
  } 
  else if (gsIsOpId(head)) 
    {
      // simplify left (and right)
      int nqlhs, nqrhs;
      data_variable_list fvlhs,fvrhs;
      free_vars_and_no_quants(args.front(),&nqlhs,&fvlhs);
      args = pop_front(args);
      if (!args.empty())
	free_vars_and_no_quants(args.front(),&nqrhs,&fvrhs);
      else nqrhs = 0;
      *nq = nqlhs + nqrhs;
      *fv = dunion(fvlhs,fvrhs);     
    }
  else if (gsIsBinder(head))
    {
      int nq_under;
      data_variable_list fv_under;
      data_variable_list qvars = list_arg1(head);
      free_vars_and_no_quants(arg2(d),&nq_under, &fv_under);
      *nq = nq_under + 1;
      *fv = substract(fv_under,qvars);
    }
  else
    {
      gsVerboseMsg("DON'T KNOW what this is");
      *nq = 0;
    }
  gsVerboseMsg("FREE_VARS_AND_NO_QUANTS:      %d quantifiers, free vars: %s\n",*nq, pp(*fv).c_str());
}


  








//======================================================================
bool var_in_list(data_variable vx, data_variable_list y)
//======================================================================
{
  // which one is correct?? comparing the whole data var or only the name?

    std::vector<std::string> ynames = detail::variable_strings(y);
    return (std::find(ynames.begin(), ynames.end(), vx.name()) != ynames.end());
    /*
  return (std::find(y.begin(), y.end(), vx) != y.end());
    */
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
void solve_equation_interactive(propositional_variable X, 
			   pbes_expression defX, 
			   pbes_expression approx)
{
  // To implement later. 
}
//======================================================================






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
