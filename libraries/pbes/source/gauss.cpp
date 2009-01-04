// Author(s): Simona Orzan. Distributed under the Boost
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./gauss.cpp

#include "boost.hpp" // precompiled headers


//C++ 
#include <cstdio> 
#include <exception> 
#include <iostream> 
#include <fstream> 
#include <string> 
#include <utility> 

//PBESsolve dedicated libraries
#include "mcrl2/pbes/gauss.h"
#include "mcrl2/pbes/sort_instantiator.h"
#include "mcrl2/pbes/normal_forms.h"
#include "mcrl2/pbes/free_bounded_vars.h"


//
#include "mcrl2/pbes/find.h"
#include "mcrl2/pbes/normalize.h"
#include "mcrl2/pbes/complement.h"
#include "mcrl2/data/find.h"
#include "mcrl2/data/utility.h"
#include "mcrl2/data/sort_utility.h"
#include "mcrl2/atermpp/substitute.h"
#include "mcrl2/core/print.h"


#include "mcrl2/atermpp/algorithm.h"     // replace
#include "mcrl2/atermpp/make_list.h"
#include "mcrl2/data/data.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/lps/specification.h"
#include "mcrl2/core/messaging.h"



// #define debug
// #define debug2
// #define debug_prove
//#define pbes_expression_prove_in_enumerate_rec
//#define pbes_expression_prove_with_quantifiers

//#define use_prover
#define use_rewriter

int PESdeep=0; // for debug, the depth of pbes_expression_simplify calls

using namespace pbes_expr;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2::data;
using namespace mcrl2::lps;
using namespace mcrl2::pbes_system;

sort_instantiator si;

// some extra needed functions on data_variable_lists and data_expressions
bool var_in_list(data_variable vx, data_variable_list y);
data_variable_list intersect(data_variable_list x, data_variable_list y);
data_variable_list substract(data_variable_list x, data_variable_list y);
data_variable_list dunion(data_variable_list x, data_variable_list y);
void dunion(data_variable_list *x, data_variable_list y);

/// \brief Returns the head of the data expression t.
/// \return The head of the data expression.
/// \deprecated
/// \param t A data expression
/// \return The head of the expression, if it is a function application
inline
data_expression DEPRECATED_FUNCTION_HEAD(data_expression t)
{
  return mcrl2::core::detail::gsGetDataExprHead(t);
}

/// \brief Returns the arguments of the data expression t.
/// \return The arguments of the data expression.
/// \deprecated
/// \param t A data expression
/// \return The arguments of the expression, if it is a function application
inline
data_expression_list DEPRECATED_FUNCTION_ARGUMENTS(data_expression t)
{
  return mcrl2::core::detail::gsGetDataExprArgs(t);
}

// Implementation of pbes_solver  
//*********************************

//=================================================
pbes_solver::pbes_solver(pbes<> p_pbes_spec,
			 SMT_Solver_Type solver, 
			 RewriteStrategy rew_strategy, 
			 int p_bound, bool p_pnf, bool p_interactive)
//=================================================
{
  pbes_spec = p_pbes_spec;
  
  data_specification ds = pbes_spec.data();
  
  prover = new BDD_Prover(ds, rew_strategy, 0, false, solver, false);
  
  bound = p_bound;
  pnf = p_pnf;
  interactive = p_interactive;
}





//=================================================
atermpp::vector<pbes_equation> pbes_solver::solve()
//=================================================

{

  atermpp::vector<pbes_equation> es_problem;
  es_problem = pbes_spec.equations();
  
  atermpp::vector<pbes_equation> es_solution;
  
 // instantiate all finite sorts from the specification
 // (better: instantiate later, when needed...)
  data_specification ds = pbes_spec.data();
  sort_expression_list finite_sorts;
  sort_expression_list sl = ds.sorts();
  si.set_data_operation_list(ds.constructors());
  for (sort_expression_list::iterator i = sl.begin(); i != sl.end(); i++)
    if (is_finite(ds.constructors(), (*i)))
      finite_sorts = push_front(finite_sorts,(*i));
  si.instantiate_sorts(finite_sorts);
  
#ifdef debug2
  gsVerboseMsg("SORT_INSTANTIATOR check!\n");
  for (sort_expression_list::iterator i = finite_sorts.begin(); i != finite_sorts.end(); i++)
    {
      gsVerboseMsg("sort %s:  enumeration %s\n",
		   pp(*i).c_str(), pp(si.get_enumeration(*i)).c_str());
    } 
#endif
  
  
#ifdef debug
  gsVerboseMsg("starting loop\n"); 
#endif
  
  // Is using a reverse_iterator correct? inefficient??
  for (atermpp::vector<pbes_equation>::reverse_iterator eqcrt = 
	 es_problem.rbegin(); eqcrt != es_problem.rend(); eqcrt++)
    {
      //check!
      //gsVerboseMsg("checking");gsVerboseMsg(" %s\n",pp(eqcrt->formula()).c_str());
      //if (detail::check_rule_PBExpr(eqcrt->formula()))	
      //gsVerboseMsg("checked!\n");
      
      // replace already solved variables with their solutions
      pbes_expression newrhs = update_expression(eqcrt->formula(), es_solution);
      *eqcrt = pbes_equation(eqcrt->symbol(), eqcrt->variable(), newrhs);
      gsVerboseMsg("\n*****************\nEquation to solve: %s\n*****************\n",
		   pp(*eqcrt).c_str());
      
      // solve this equation
      pbes_equation e_solution = 
	solve_equation(*eqcrt);
      
      // add solution of this equation to the already known solutions
      es_solution.insert(es_solution.begin(), e_solution);
    }
  
  // delete rewriter; 
  // ~BDD_Prover(prover);
  
  return es_solution;
}




//=================================================
pbes_equation pbes_solver::solve_equation(pbes_equation e) 
//=================================================
 
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
  
  no_iterations = (bound == 0);

  if (pnf) {
    // transform defX first to PNF
    pbes_expression pnorm = normalize(defX); // eliminate imp and not
#ifdef debug
    gsVerboseMsg("Normalized: %s\n",pp(pnorm).c_str());
#endif
    pbes_expression pnorm_uni = remove_double_variables(pnorm);
#ifdef debug
    gsVerboseMsg("Double vars removed: %s\n",pp(pnorm_uni).c_str());
#endif
    defX =  pbes_expression_to_prenex(pnorm_uni); 
#ifdef debug
    gsVerboseMsg("in PNF: %s\n",pp(defX).c_str());
#endif    
  }


  // iteration
  while ( (!stable) && (bound - no_iterations != 0) )
    {
      gsVerboseMsg("Approximation %3d: %s\n-----------------\n",
		   no_iterations,pp(approx).c_str());
      
      // substitute approx for X in defX, store result in approx_
      //	 	 gsVerboseMsg("SUBSTITUTE in %s\n",pp(defX).c_str());
      approx_ = defX;
      approx_ = substitute(approx_, X, approx);  
#ifdef debug2
      gsVerboseMsg("SUBSTITUTE result %s\n",pp(approx_).c_str());	 	 
#endif
      
      // rewrite approx_...
      //	 approx_ = pbes_expression_prove(approx_,rewriter,prover);
      int nq = 0;
      data_variable_list fv;
      approx_ = pbes_expression_simplify(approx_, &nq, &fv, prover);

#ifdef pbes_expression_prove_with_quantifiers
      approx_ = pbes_expression_prove(approx_,prover);
#else
      if (nq == 0)
	approx_ = pbes_expression_prove(approx_,prover);
      
#endif
      
#ifdef debug
      gsVerboseMsg("REWRITTEN: %s\n%d quantifiers left\n",pp(approx_).c_str(),nq);
#endif
      // decide whether it's stable
      // pbes_expression_compare is not reliable

      //      stable = pbes_expression_compare(approx_,approx,prover);
            stable = (approx_ == approx);

      /*
      // the approximation process should 
      // be stopped if all predicate variables have disappeared
      if (!stable) 
	{
	  std::set<propositional_variable_instantiation> setpred =
	    find_all_propositional_variable_instantiations(approx_);
	  stable = (setpred.empty());
	}
      */

#ifdef debug
      if (!stable) gsVerboseMsg("Not"); gsVerboseMsg(" stable\n");
#endif
      
      // continue
      approx = approx_;
      no_iterations++;
    }
  
  if (stable) gsVerboseMsg("Stabilized (in %d iterations) at %s\n", no_iterations-1, pp(approx).c_str());
  // interactive behaviour
  if (interactive) solve_equation_interactive(X,defX,approx);
  
  // make solution and return it
  return pbes_equation(e.symbol(), X, approx);
}





//================================================
// Replaces, in solX, all occurrences of variables 
// from generic_parameters with the corresponding data expressions 
// from actual_parameters
pbes_expression pbes_expression_instantiate(pbes_expression solX, 
						   data_variable_list generic_parameters, 
						   data_expression_list actual_parameters)
//================================================
{

#ifdef debug2
 gsVerboseMsg("PBES_EXPRESSION_INSTANTIATE\n");
 gsVerboseMsg("  solX: %s\n  generic parameters: %s\n  actual parameters:  %s\n",
						pp(solX).c_str(), pp(generic_parameters).c_str(), 
						pp(actual_parameters).c_str());
#endif

 pbes_expression result = 
	solX.substitute(make_list_substitution(generic_parameters, actual_parameters));

#ifdef debug2 
 gsVerboseMsg("  Result: %s\n", pp(result).c_str());
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
	return(and_(substitute(left(expr),X,solX), substitute(right(expr),X,solX)));	
 else if (is_or(expr)) 
	//substitute in lhs and rhs
	return(or_(substitute(left(expr),X,solX), substitute(right(expr),X,solX)));
 else if (is_forall(expr))
	// substitute in expression under quantifier
	return (forall(var(expr), substitute(arg(expr),X,solX)));
 else if (is_exists(expr))
	// substitute in expression under quantifier
	return (exists(var(expr), substitute(arg(expr),X,solX)));
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
// Replaces, in e,
// all occurences of binding variables from es_solution
// with their solutions (i.e., corresponding equations from es_solution)
pbes_expression update_expression
(pbes_expression e, atermpp::vector<pbes_equation> es_solution)
//==================

{

 // !!todo: implement simultaneous substitutions, probably more efficient 
 pbes_expression ee = e;

#ifdef debug
 gsVerboseMsg("update_expression.\n");
#endif

 for (atermpp::vector<pbes_equation>::reverse_iterator s = 
			 es_solution.rbegin(); s != es_solution.rend(); s++)	
{
  //gsVerboseMsg("update_expression: substituting equation %s\n",pp(*s).c_str());
	ee = substitute(ee, s->variable(), s->formula());
	//gsVerboseMsg("update_expression: result is %s\n",pp(ee).c_str());
 }
 return ee; 
}







//======================================================================
data_expression pbes_to_data(pbes_expression e)

// This is a customized variant of pbes2data from pbes_utility.h
// All operators become data operators.
// Predicate variables become data variables, with 
// the same arguments but a marked name (+PREDVAR_MARK at the end)

// It doesn't work for quantifiers!
// (if we translate PBES quantifiers to DATA quantifiers,
// then there is no way back, because of the data implementation part)
// !! todo: check whether data implementation can be avoided
//======================================================================
{
 using namespace pbes_expr;
 namespace dname = mcrl2::data::data_expr;
 namespace sname = mcrl2::data::sort_expr;
 
#ifdef debug2
 gsVerboseMsg("P2D: %s\n",pp(e).c_str());
#endif
 if (is_data(e)) return e; //  of data_expression(aterm_appl(e)) ??;
 else if (is_true(e)) return dname::true_();
 else if(is_false(e)) return dname::false_();
 else if (is_and(e))
	return dname::and_(pbes_to_data(left(e)),pbes_to_data(right(e)));
 else if (is_or(e)) 
	return dname::or_(pbes_to_data(left(e)),pbes_to_data(right(e)));
 /*
 else if (is_forall(e))
   {
     aterm_appl x = 
       gsMakeBinder(gsMakeForall(),var(e),pbes_to_data(arg(e)));
     return (gsMakeDataExprExists(x));
     //implement_data_data_expr(x,spec); 
     // (if implement, then there is no way back anymore!)
   } 
 else if (is_exists(e)) 
   {
     aterm_appl x = 
       gsMakeBinder(gsMakeExists(),var(e),pbes_to_data(arg(e)));
     return (gsMakeDataExprExists(x));
   }
 */
 else if (is_not(e))
   return dname::not_(pbes_to_data(arg(e)));
 else
   if (is_propositional_variable_instantiation(e)) 
     // transform it to a data variable with parameters
     {
       std::string vname = name(e);
       vname += PREDVAR_MARK;
       data_expression_list parameters = param(e);
#ifdef debug2
       gsVerboseMsg("P2D: name %s, parameters %s",
		    vname.c_str(), pp(parameters).c_str());
#endif
       if (parameters.empty())
	 {
	   vname+=":Bool";
	   data_variable v = data_variable(vname);
	   return data_expression(v);
	 }
       else
	 {
	   sort_expression_list sorts = apply(parameters, gsGetSort);
#ifdef debug2
       gsVerboseMsg("P2D: sorts %s\n", pp(sorts).c_str());
#endif

	   sort_expression vsort = gsMakeSortArrowList(sorts, sname::bool_());
#ifdef debug2
       gsVerboseMsg("P2D: new sort %s\n", pp(vsort).c_str());
#endif

       data_variable v = data_variable(vname,vsort); // not good!!
       data_expression res = gsMakeDataApplList(v, parameters);
#ifdef debug2
       gsVerboseMsg("P2D: new data expression %s\n", pp(res).c_str());
#endif
	  return res;
	 }
     }
   else if ((is_forall(e)) || (is_exists(e)))
     {
       gsErrorMsg("\npbes_to_data: cannot handle quantifiers\n");
       exit(1);
     }
 /* the code below ends in segmentation fault at the creation of a data variable
    if (is_propositional_variable_instantiation(e)) 
    {
    identifier_string vname = name(e)+PREDVAR_MARK;
    data_expression_list parameters = param(e);
    if (parameters.empty())
    {
    sort_expression vsort = gsMakeSortIdBool();
    data_variable v = data_variable(gsMakeDataVarId(vname, vsort));
    return data_expression(v);
    }
    else
    {
    std::cerr<<"A";
    sort_expression_list sorts = apply(parameters, gsGetSort);
    std::cerr<<"B";
    sort_expression vsort = gsMakeSortArrowList(sorts, sname::bool_());
    data_variable v = data_variable(gsMakeDataVarId(vname, vsort));
    return gsMakeDataApplList(v, parameters);
    }
    }
 */
#ifdef debug2
 gsVerboseMsg("P2D: none of the above\n");
#endif
 return data_expression(); // to prevent compiler warnings
}

//======================================================================






//======================================================================
 pbes_expression data_to_pbes_lazy(data_expression d)
//======================================================================
{
 namespace dname = mcrl2::data::data_expr;
 namespace pname = pbes_expr;
 namespace sname = mcrl2::data::sort_expr;
 
 if (dname::is_true(d))
   return pname::true_();
 else if (dname::is_false(d))
   return pname::false_();

 // if d doesn't contain any predicate variables, 
 // leave it as it is
 if ( d.to_string().find(PREDVAR_MARK) == std::string::npos )
	return d;
 
 // else, predicate variables have to be reconstructed

 
#ifdef debug2
 gsVerboseMsg("Data_to_pbes_lazy: Head is %s, args are %s\n", 
	      pp(DEPRECATED_FUNCTION_HEAD(d)).c_str(), pp(DEPRECATED_FUNCTION_ARGUMENTS(d)).c_str());
#endif


 if (is_data_variable(DEPRECATED_FUNCTION_HEAD(d)))
	// d is either a predicate or a data variable with arguments (?)
	{
	 std::string varname = data_variable(DEPRECATED_FUNCTION_HEAD(d)).name();
#ifdef debug2
	 gsVerboseMsg("head is data var, varname=%s\n ",varname.c_str());
#endif
	 if (varname.at(varname.size()-1) == PREDVAR_MARK)
	   return 
	     propositional_variable_instantiation
	     (identifier_string(varname.substr(0,varname.size()-1)), DEPRECATED_FUNCTION_ARGUMENTS(d));

	 else
		return d;
	}
 else if (is_data_variable(d))
   // d is either a predicate or a data variable without arguments (?)
   {
     std::string varname = data_variable(d).name();
     gsVerboseMsg("head is data var, varname=%s ",varname.c_str());
     if (varname.at(varname.size()-1) == PREDVAR_MARK)
       return 
	 propositional_variable_instantiation
	 (identifier_string(varname.substr(0,varname.size()-1)));
     else
       return d;
   }
 else if (gsIsDataExprIf(d))  // what's the right test here?
   // in this case, the reconstruction of the pbes_expression is limited 
   // by the absence of negation in pbes_expressions
   {
#ifdef debug2
     gsVerboseMsg("IS IF!\n");
#endif
     data_expression_list::iterator i = DEPRECATED_FUNCTION_ARGUMENTS(d).begin();
     data_expression d1 = *i;
     i++;
     data_expression d2 = *i;
     i++;
     data_expression d3 = *i;
     pbes_expression ptest = data_to_pbes_lazy(d1);
     pbes_expression ptrue = data_to_pbes_lazy(d2);
     pbes_expression pfalse = data_to_pbes_lazy(d3);
     //     return(or_(and_(ptest,ptrue),and_(not_(ptest),pfalse)));

#ifdef debug2
     gsVerboseMsg("Sub pbes_expressions are: %s, %s, %s\n",
		  pp(ptest).c_str(),pp(ptrue).c_str(),pp(pfalse).c_str());
#endif
     // Below both dname and pname are used, 
     // because the translation is lazy and therefore 
     // the pbes_expression "true"
     // will in fact be "true".
     // pname::is_true( val(true) ) returns false... Suggest change?!
     /*
     if ((dname::is_true(ptest)) || (pname::is_true(ptest))) 
       return ptrue;
     if ((dname::is_false(ptest)) || (pname::is_false(ptest))) 
       return pfalse;
     if (((dname::is_true(ptrue)) || (pname::is_true(ptrue)) )
	 &&
	 ((dname::is_false(pfalse)) || (pname::is_false(pfalse))))
       return ptest;
     */
     // gsErrorMsg("ERROR\n");exit(1);
     // The following line could replace 
     // the 3 if's above, if properly implemented
     /* else */ 
       //return(or_(and_(ptest,ptrue),and_(complement(ptest),pfalse)));
       return(or_(and_(ptest,ptrue),and_(not_(ptest),pfalse)));
   }
 else if ((dname::is_and(d)) || (dname::is_or(d))){
   data_expression_list::iterator i = DEPRECATED_FUNCTION_ARGUMENTS(d).begin();
   data_expression d1 = *i; i++; 
   data_expression d2 = *i;
   return (is_and(d) ?
	   pname::and_(data_to_pbes_lazy(d1), data_to_pbes_lazy(d2)):
	   pname::or_(data_to_pbes_lazy(d1), data_to_pbes_lazy(d2)));
 }
 gsErrorMsg("cannot translate this: %s\n",pp(d).c_str());
 exit(1);
 return pbes_expression(); // to prevent compiler warnings
}





//======================================================================
// This translates as much as possible of the logical operators
// to pbes operators. Unfinished.
 pbes_expression data_to_pbes_greedy(data_expression d)
{

  // arg1 is not reliable
 
 namespace dname = mcrl2::data::data_expr;
 namespace pname = pbes_expr;
 namespace sname = mcrl2::data::sort_expr;
 
 data_expression head = DEPRECATED_FUNCTION_HEAD(d);
 if (is_data_variable(head))
	// d is either a predicate or a data variable
	{
	 std::string varname = data_variable(head).name();
	 if (varname.at(varname.size()-1) == PREDVAR_MARK)
		return 
			propositional_variable_instantiation
		 (identifier_string(varname.substr(0,varname.size()-1)), DEPRECATED_FUNCTION_ARGUMENTS(d));
	 else
		return d;
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
 
 return d;
}
//======================================================================











//======================================================================
// EXPERIMENT !!
// Rewrites (simplifies) e according to the
// rewriting rules of first-order logic.
// only works for quantifier-free expressions.
pbes_expression pbes_expression_prove_experiment(pbes_expression e, BDD_Prover* prover)
{

#ifdef debug_prove
  gsVerboseMsg("PBES_EXPRESSION_PROVE start %s\n", pp(e).c_str());
#endif
  using namespace pbes_expr;
  
  if ((is_and(e)) || (is_or(e)) || (is_imp(e))) {
    pbes_expression pleft = pbes_expression_prove(left(e),prover);
    pbes_expression pright = pbes_expression_prove(right(e),prover);
    if (is_true(pleft))
      return (is_and(e)? pright : 
	      (is_or(e)? true_() : pright));
    if (is_false(pleft))
      return (is_and(e)? false_() : 
	      (is_or(e)? pright : true_()));
    if (is_true(pright))
      return (is_and(e)? pleft : 
	      (is_or(e)? true_() : true_()));
    if (is_false(pright))
      return (is_and(e)? false_() :
	      (is_or(e)? pleft : not_(pleft)));
  }

  /*
    else if ((is_forall(p))||(is_exists(p))) {
  }

 else if (is_not(p)){
 }
  */
  
 data_expression de = pbes_to_data(e); 
#ifdef debug_prove
  gsVerboseMsg("PBES_EXPRESSION_PROVE --> prover: %s\n", pp(de).c_str());
#endif
 prover->set_formula(de); 
 data_expression d = prover->get_bdd();
#ifdef debug_prove
  gsVerboseMsg("PBES_EXPRESSION_PROVE <-- prover: %s\n", pp(d).c_str());
#endif
 e = data_to_pbes_lazy(d);
 
#ifdef debug_prove
  gsVerboseMsg("PBES_EXPRESSION_PROVE end %s\n", pp(e).c_str());
#endif
 return e;

}



//======================================================================
// Rewrites (simplifies) e according to the
// rewriting rules of first-order logic.
// only works for quantifier-free expressions.
pbes_expression pbes_expression_prove(pbes_expression e, BDD_Prover* prover)
{

#ifdef debug_prove
  gsVerboseMsg("PBES_EXPRESSION_PROVE start %s\n", pp(e).c_str());
#endif
  
 data_expression de = pbes_to_data(e); 
#ifdef debug_prove
  gsVerboseMsg("PBES_EXPRESSION_PROVE --> prover: %s\n", pp(de).c_str());
#endif
 prover->set_formula(de); 
 data_expression d = prover->get_bdd();
#ifdef debug_prove
  gsVerboseMsg("PBES_EXPRESSION_PROVE <-- prover: %s\n", pp(d).c_str());
#endif
 e = data_to_pbes_lazy(d);
 
#ifdef debug_prove
  gsVerboseMsg("PBES_EXPRESSION_PROVE end %s\n", pp(e).c_str());
#endif
 return e;

}


















//======================================================================
 pbes_expression pbes_expression_simplify
 (pbes_expression expr, int* nq, data_variable_list *fv, BDD_Prover* prover)
//======================================================================
{
  PESdeep++;
#ifdef debug
  gsVerboseMsg("PBES_EXPRESSION_SIMPLIFY %d start:     %s\n",
	       PESdeep,pp(expr).c_str());
#endif

  *fv = data_variable_list();
  *nq = 0;
  pbes_expression expr_simplified;

  if (is_and(expr))
    {
      // simplify left and right
      int nqlhs, nqrhs;
      data_variable_list fvlhs,fvrhs;
      pbes_expression slhs = pbes_expression_simplify(left(expr),&nqlhs,&fvlhs,prover);
      pbes_expression srhs = pbes_expression_simplify(right(expr),&nqrhs,&fvrhs,prover);
      *nq = nqlhs + nqrhs;
      *fv = fvlhs+fvrhs;
      expr_simplified = and_(slhs,srhs); 
      // (and_ already checks whether one of the sides is trivial T/F)	
    }
  else if (is_or(expr)) 
    {
      // simplify left and right
      int nqlhs, nqrhs;
      data_variable_list fvlhs,fvrhs;
      pbes_expression slhs = pbes_expression_simplify(left(expr),&nqlhs,&fvlhs,prover);
      pbes_expression srhs = pbes_expression_simplify(right(expr),&nqrhs,&fvrhs,prover);
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
	pbes_expression_simplify(arg(expr),&nq_under,&fv_under, prover);
      // dit heeft waarschijnlijk geen zin:
      //      if (nq_under==0)
      //s_under = pbes_expression_prove(s_under,prover);
      
      // compute the list of actually bounded variables 
      // (i.e., eliminate from the var those vars that do not occur free in s_under)
      data_variable_list new_quant_vars = intersect(var(expr),fv_under);
      *fv = substract(fv_under, new_quant_vars); // !! too inefficient?

      // if any quantified vars left, try to eliminate them by enumeration   
      if (!new_quant_vars.empty()){
#ifdef debug
	gsVerboseMsg("\n**********PBES_EXPRESSION_SIMPLIFY: calling enumerate_finite_domains for %s ******** new_quant_vars=%s\n",
		     pp(s_under).c_str(),pp(new_quant_vars).c_str());
#endif
	pbes_expression s_under_before = s_under;
	if (is_forall(expr)) 
	  s_under = 
	    enumerate_finite_domains(true, &new_quant_vars, s_under_before, prover);
	else
	  s_under = 
	    enumerate_finite_domains(false, &new_quant_vars, s_under_before, prover);     
#ifdef debug
	gsVerboseMsg("\nPBES_EXPRESSION_SIMPLIFY: result of enumerate_finite_domains is %s\n",
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
      /*
      // Sending to the prover.
      // !! don't know what happens if expr contains quantifiers.
      data_expression dexpr = data_expression(aterm_appl(expr));
#ifdef debug
      gsVerboseMsg("\nPBES_EXPRESSION_SIMPLIFY: it's data! pbes_expr %s, data_expr %s, sending to prover\n ",
		   pp(expr).c_str(), pp(dexpr).c_str());
#endif
      data_expression rdata = rewrite_data_expression (dexpr,prover);
      //      std::cerr<<" \nAAA ";
      expr_simplified = rdata;
      //std::cerr<<" \nBBB ";
#ifdef debug
      gsVerboseMsg("\nPBES_EXPRESSION_SIMPLIFY: back from prover: %s, counting free vars",
		   pp(expr_simplified).c_str());
#endif
      //std::cerr<<" \nCCC ";
      */
      data_expression data_simplified = 
	data_expression_simplify(expr, fv, prover);
      expr_simplified = data_simplified;
    }
  //  else // expr is true, false or a propositional variable
  else
    {
      expr_simplified = expr;
    }


#ifdef debug
  gsVerboseMsg("PBES_EXPRESSION_SIMPLIFY %d end:     %s\n %d quantifiers, free vars: %s\n",
	       PESdeep,pp(expr_simplified).c_str(),*nq, pp(*fv).c_str());
#endif

  PESdeep--;
   return expr_simplified; 
}
//======================================================================



/*
//rewrite the if expression according to the T/F values
//of subexpressions
data_expression data_expression_simplify_if()
{
}
*/



//======================================================================
data_expression data_expression_simplify
(data_expression d, data_variable_list *fv, BDD_Prover *prover)
//======================================================================
{
  namespace dname = mcrl2::data::data_expr;
  
  data_expression e = d;
  
  // first, test whether it's a trivial if expression
  // apparently, this is a popular case (seen in examples)
  if (gsIsDataExprIf(d))  // is this the right test?
    {
#ifdef debug
      gsVerboseMsg("DATA_EXPRESSION_SIMPLIFY: IS IF!\n");
#endif
      data_expression_list::iterator i = DEPRECATED_FUNCTION_ARGUMENTS(d).begin();
      data_expression d1 = *i;
      i++;
      data_expression d2 = *i;
      i++;
      data_expression d3 = *i;
      if (dname::is_true(d1)) return data_expression_simplify(d2,fv,prover);
      if (dname::is_false(d1)) return data_expression_simplify(d3,fv,prover);
      if ((dname::is_true(d2)) && (dname::is_false(d3)))
	return data_expression_simplify(d1,fv,prover);
      if ((dname::is_true(d3)) && (dname::is_false(d2)))
	return (dname::not_(data_expression_simplify(d1,fv,prover)));
    }

     // call prover
#ifdef use_prover
  prover->set_formula(d); 
  e = prover->get_bdd();
#endif 


  // call rewriter
  data_expression f = e;
  // rewrite
#ifdef use_rewriter
  Rewriter* r = prover->get_rewriter();
  f = r->rewrite(e);
#endif
  
  // fill in the list of occuring variables
  std::set<data_variable> setfv = find_all_data_variables(d);
  for (std::set<data_variable>::iterator i=setfv.begin(); i!=setfv.end();i++)
    *fv = push_back(*fv,*i);
  return f;
}


  











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

#ifdef pbes_expression_prove_in_enumerate_rec
#ifdef debug
    gsVerboseMsg("ENUMERATE_REC: p_instance: %s\n",pp(p_instance).c_str());
#endif
      p_instance = pbes_expression_prove(p_instance,prover);
#ifdef debug
    gsVerboseMsg("ENUMERATE_REC: rewritten: %s\n",pp(p_instance).c_str());
#endif
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
(bool forall, data_variable_list *var, pbes_expression p, BDD_Prover *prover)
//======================================================================

// - instantiate all finite domain variables from var
// with their domain's elements
// - return the \/ composition of the resulting p's
// - in the end, var will only contain the infinite-domain variables
{
  // make a separate list of finite domain variables
  data_variable_list finite_domain_vars;
  data_variable_list::iterator v = var->begin();
  for ( ; v != var->end(); v++)
    {
      sort_expression vsort = v->sort();
      
      //// gsVerboseMsg("enumerate_finite_domains::: v= %s, vsort= %s\n",pp(*v).c_str(), pp(vsort).c_str());
      
      if (si.is_finite(vsort))
	finite_domain_vars = push_back(finite_domain_vars, (*v));
    } 
  
  // eliminate the finite vars from the var list
  *var = substract((*var), finite_domain_vars);

#ifdef debug  
  gsVerboseMsg("ENUMERATE_FINITE_DOMAINS: finite vars %s, infinte vars %s\n", 
	       pp(finite_domain_vars).c_str(), pp(*var).c_str());
#endif  

  // instantiate the finite vars
  data_expression_list dl;
  pbes_expression p_enumeration= 
    enumerate_rec(forall, finite_domain_vars, finite_domain_vars.begin(), 
		  dl,p, prover);
  return p_enumeration;
  //  return (pbes_expression_prove(p_enumeration,prover));
}
//======================================================================










//======================================================================
void pbes_solver::solve_equation_interactive(propositional_variable X, 
					     pbes_expression defX, 
					     pbes_expression approx)
{
  // To implement later. 
}
//======================================================================






//======================================================================
// i need a safe and fast way to translate pbes_expressions to 
// ATermAppls understood by the prover.
// Via data expressions, it's too expensive and unsure
// ( It seems difficult to properly translate pbes quantifiers to 
// data quantifiers)
// !! The prover can't use quantifiers anyway.
// They should be translated as functions.
//
// The code below is an unsuccesful attempt 
// (because pbes_to_data causes assertion failures) 
bool pbes_expression_compare
(pbes_expression p, pbes_expression q, BDD_Prover* prover)
{
  if (p == q) return true;
  // if no quantifiers and no predicates, call the prover
#ifdef debug
  gsVerboseMsg("Comparing %s (%d) and %s (%d)\n",
	       pp(p).c_str(),p.is_bes(),pp(q).c_str(),is_bes(q));
#endif
  //  if ((!contains_quantifiers(p))&&(!contains_quantifiers(q)))
    {
      data_expression dp = pbes_to_data(p);
      data_expression dq = pbes_to_data(q);
      ATermAppl formula = gsMakeDataExprEq((ATermAppl)dp,(ATermAppl)dq);
#ifdef debug      
      gsVerboseMsg(" COMPARE -->prover: %s\n",pp(formula).c_str());
#endif
      prover->set_formula(formula);
      Answer a = prover->is_tautology();
#ifdef debug
      gsVerboseMsg(" <--prover: %s\n",
		   ((a==answer_yes)?"YES":((a==answer_no)?"NO":"DON'T KNOW")));
#endif
      return ((a == answer_yes)?true:false);
    }

  // else, improvise
#ifdef debug
  gsVerboseMsg("The prover cannot compare expressions with quantifiers \n");
#endif
  return false;
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












































//======================================================================
//======================================================================
//======================================================================
//======================================================================
//======================================================================
//======================================================================

// not used anymore:::::::


//======================================================================
void free_vars_and_no_quants(data_expression d, int* nq, data_variable_list *fv)
// fills in the number of quantifiers and the list of free vars in expression d  
//!! can be more efficient if we assume there are no quantifiers in data expressions
//======================================================================
{
  
  using namespace data_expr;
  
  data_expression head = DEPRECATED_FUNCTION_HEAD(d);
  data_expression_list args = DEPRECATED_FUNCTION_ARGUMENTS(d);
  
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


