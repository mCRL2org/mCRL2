// Author(s): Simona Orzan.
//
// Copyright: see the accompanying file
// COPYING or copy at https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file gauss.h

#ifndef MCRL2_PBES_GAUSS_H
#define MCRL2_PBES_GAUSS_H

#include <cstdio>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/pbes_expression.h"
#include "mcrl2/data/rewrite.h" // rewriter
#include "mcrl2/data/bdd_prover.h" // prover
#include "mcrl2/core/messaging.h"

#define PREDVAR_MARK '$'

using namespace mcrl2::data;
using namespace mcrl2::lps;
using namespace mcrl2::pbes_system;

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

  /// if set, the approximation process will
  /// start with pulling all quantifiers to the front
  bool pnf;
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
	      SMT_Solver_Type solver,
	      RewriteStrategy rew_strategy,
	      int p_bound, bool p_pnf, bool p_interactive);

  /// This function implements the Gauss elimination algorithm.
  atermpp::vector<pbes_equation> solve();

} ;



/// This function transforms a pbes_expression without quantifiers
/// into a data expression, then sends it to the prover
/// and transforms the simplified result back to a pbes expression.
///
/// todo: find a way to deal with quantifiers
/// todo: make a dedicated prover for pbes expressions
pbes_expression pbes_expression_prove
(pbes_expression e, BDD_Prover* p);


/// Applies various heuristics in order to simplify
/// a pbes_expression.
/// OUT nq  := the number of quantifiers left in expr after simplification
/// OUT fv := the set of names of the data variables occuring FREE in expr,
///        after simplification
pbes_expression pbes_expression_simplify
(pbes_expression p, int *nq, data_variable_list *fv , BDD_Prover *prover);


/// This function assumes there are no quantifiers in data expressions.
/// The simplification is done by the prover and/or/rewriter
data_expression data_expression_simplify
(data_expression d, data_variable_list *fv, BDD_Prover *prover);


/// This is a customized variant of pbes2data from pbes_utility.h
/// All operators become data operators.
/// Predicate variables become data variables, with
/// the same arguments but a marked name (+PREDVAR_MARK at the end)

/// It doesn't work for quantifiers!
/// (if we translate PBES quantifiers to DATA quantifiers,
/// then there is no way back, because of the data implementation part)
///
/// todo: check whether data implementation can be avoided
data_expression pbes_to_data(pbes_expression e);


/// This is a customized variant of data2pbes from pbes_utility.h
/// It keeps the logical operators in the data world,
/// unless there is an obvious need to translate them to pbes operators.
pbes_expression data_to_pbes_lazy(data_expression d);


/// This translates as much as possible of the logical operators
/// to pbes operators. Unfinished.
pbes_expression data_to_pbes_greedy(data_expression d);


/// Returns expr [X <- solX].
pbes_expression substitute(pbes_expression expr,
			   propositional_variable X,
			   pbes_expression solX);


/// Substitutes, in e, all predicate variables with their respective
/// solutions as given by es_solution.
pbes_expression update_expression(pbes_expression e,
				  atermpp::vector<pbes_equation> es_solution);


/// Instantiates p for all possible values of the variables
/// in quant_vars (assumed from finite domains).
/// Out of these instances, a big /\ (if forall==true) or
/// \/ (if forall==false) is produced.
/// The 'forall' parameter should be set on true if this enumeration
/// is done for an universal quantifier and false if it's for
/// an existential quantifier.
pbes_expression enumerate_finite_domains
(bool forall, data_variable_list *quant_vars, pbes_expression p, BDD_Prover *prover);


/// Compares two pbes expressions using the prover.
/// It only works for quantifier-free expressions.
bool pbes_expression_compare
(pbes_expression p, pbes_expression q, BDD_Prover* prover);

#endif // MCRL2_PBES_GAUSS_H

