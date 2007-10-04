//  Copyright 2007 Simona Orzan. Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ./gauss.h

#include <cstdio>
#include <string>
#include <iostream> 
#include <sstream> 
#include <fstream> 

#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/pbes_expression.h"
#include "librewrite.h" // rewriter
#include "prover/bdd_prover.h" // prover
#include "print/messaging.h"

#define PREDVAR_MARK '$'

using namespace lps;

/// A class implementing the Gauss elimination algorithm
/// for pb equation systems.
/// Equations are solved one by one, by an approximation process.
class pbes_solver {  
 private:

  /// the maximum number of iterations for the approximation process
  /// 0 means no bound (default)
  int bound; 

  /// if the approximation process stalls, should the user be consulted or not?
  /// (not implemented yet)
  bool interactive;

  BDD_Prover* prover;
  pbes<> pbes_spec;
  
  /// solves an equation  e:   o X (..) = PE,
  /// by successive approximation steps:
  /// - if o = nu, X_0 = true, else X_0 = false
  /// - repeat
  ///       . X_{n+1} = PE [X <- X_n]
  ///       . simplify X_{n+1} 
  ///   until X_{n+1}==X_n       
  /// If this process stabilizes, the result
  ///  will be an equation without
  /// e's predicate variable (X) on the right hand side.
  pbes_equation solve_equation(pbes_equation e);
  
  /// Given the equation o X = defX,
  /// and approx, the current approximation for X,
  /// this functions continues the approximation process,
  /// using input from the user.
  void solve_equation_interactive(propositional_variable X, 
				  pbes_expression defX, 
				  pbes_expression approx);
  
 public:
  
  pbes_solver(pbes<> p_pbes_spec,
	      std::string solver, 
	      std::string rew_strategy, 
	      int p_bound, bool p_interactive);
  
  /// This function implements the Gauss elimination algorithm.
  atermpp::vector<pbes_equation> solve();
  
} ;




/// This function transforms a pbes_expression without quantifiers
/// into a data expression, then sends it to the prover
/// and transforms the simplified result back to a pbes expression
///
/// todo: find a way to deal with quantifiers
/// todo: make a dedicated prover for pbes expressions
pbes_expression pbes_expression_prove(pbes_expression e, 
					BDD_Prover* p);


/// 
pbes_expression substitute(pbes_expression expr, 
			   propositional_variable X, 
			   pbes_expression solX);

pbes_expression update_expression(pbes_expression e, 
				  atermpp::vector<pbes_equation> es_solution);


pbes_expression pbes_expression_simplify
(pbes_expression p, int *nq, data_variable_list *fv ,
 BDD_Prover *prover);

pbes_expression enumerate_finite_domains
(bool forall, data_variable_list *quant_vars, pbes_expression p, BDD_Prover *prover);




/// This function assumes there are no quantifiers in 
/// data expressions.
/// The simplification is done by the prover and/or/rewriter
data_expression data_expression_simplify
(data_expression d, data_variable_list *fv, BDD_Prover *prover);


bool pbes_expression_compare
(pbes_expression p, pbes_expression q, BDD_Prover* prover);


// some extra needed functions on data_variable_lists and data_expressions
 bool var_in_list(data_variable vx, data_variable_list y);
data_variable_list intersect(data_variable_list x, data_variable_list y);
 data_variable_list substract(data_variable_list x, data_variable_list y);
 data_variable_list dunion(data_variable_list x, data_variable_list y);
 void dunion(data_variable_list *x, data_variable_list y);






