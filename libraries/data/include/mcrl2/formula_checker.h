// Author(s): Luc Engelen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/formula_checker.h
/// \brief Interface to class Formula_Checker

#ifndef FORMULA_CHECKER_H
#define FORMULA_CHECKER_H

#include "aterm2.h"
#include "mcrl2/data/rewrite.h"
#include "mcrl2/data/bdd_prover.h"
#include "mcrl2/utilities/bdd2dot.h"

  /// \brief The class formula checker takes a data specification in mCRL2 format and a list of expressions
  /// \brief of sort Bool in the mCRL2 format and determines whether or not these expersions are tautologies or
  /// \brief contradictions.
  /// The class Formula_Checker is initialized with a specification of data equations in internal mCRL2 format using the
  /// \brief Constructor Formula_Checker::Formula_Checker. After initialization, the function Formula_Checker::check_formulas can
  /// be called any number of times to check whether the expressions of sort Bool in internal mCRL2 format in the list
  /// passed as parameter a_formulas are tautologies or contradictions.
  ///
  /// The class Formula_Checker uses an instance of the class BDD_Prover to prove a number of propositional formulas. The
  /// prover is initialized with the parameters a_rewrite_strategy, a_time_limit, a_path_eliminator, a_solver_type,
  /// a_apply_induction and a_lps. The parameter a_rewrite_strategy specifies which rewrite strategy is used by the prover's
  /// rewriter. It can be set to either GS_REWR_INNER, GS_REWR_INNERC, GS_REWR_JITTY or GS_REWR_JITTYC. The parameter
  /// a_time_limit specifies the maximum amount of time in seconds to be spent by the prover on proving a single formula. If
  /// a_time_limit is set to 0, no time limit will be enforced. The parameter a_path_eliminator specifies whether or not
  /// path elimination is applied. When path elimination is applied, the prover uses an SMT solver to remove inconsistent
  /// paths from BDDs. The parameter a_solver_type specifies which SMT solver is used for path elimination. Either the SMT
  /// solver ario (http://www.eecs.umich.edu/~ario/) or cvc-lite (http://www.cs.nyu.edu/acsys/cvcl/) can be used. To use one
  /// of these solvers, the directory containing the corresponding executable must be in the path. If the parameter
  /// a_path_eliminator is set to false, the parameter a_solver_type is ignored. The parameter a_time_limit specifies the
  /// data equations used by the prover's rewriter. The parameter a_apply_induction indicates whether or not induction on
  /// list will be applied.
  ///
  /// The parameter a_dot_file_name specifies whether a file in dot format of the resulting BDD is saved each time the
  /// prover cannot determine whether an expression of sort Bool is a contradiction or a tautology. If the parameter is set
  /// to 0, no .dot files are saved. If a string is passed as parameter a_dot_file_name, this string will be used as the
  /// prefix of the filenames. An instance of the class BDD2Dot is used to save these files in dot format.
  ///
  /// If the parameter a_counter_example is set to true, a so called counter example is printed to stderr each time an
  /// expression is encountered that is neither a contradiction nor a tautology. A counter example is a valuation for which
  /// the expression does not hold.
  ///
  /// If the parameter a_witness is set to false, a so called witness is printed to stderr each time such an expression is
  /// encountered. A witness is a valuation for which the expression holds.
  ///
  /// The function Formula_Checker::check_formulas prints information to stderr indicating whether the expressions in the
  /// list of formulas passed as parameter a_formulas are tautologies or contradictions. In some cases the BDD based prover
  /// may be unable to determine whether an expression is a tautology or a contradiction. If this is the case, the function
  /// Formula_Checker::check_formulas will print information to stderr indicating this fact. 

class Formula_Checker {
  private:
    /// \brief BDD based prover.
    BDD_Prover f_bdd_prover;

    /// \brief Class that outputs BDDs in dot format.
    BDD2Dot f_bdd2dot;

    /// \brief Flag indicating whether or not counter-examples are displayed.
    bool f_counter_example;

    /// \brief Flag indicating whether or not witnesses are displayed.
    bool f_witness;

    /// \brief Prefix for the names of the files containing BDDs in dot format.
    char const* f_dot_file_name;

    /// \brief Displays a witness.
    void print_witness();

    /// \brief Displays a counter-example.
    void print_counter_example();

    /// \brief Writes the BDD corresponding to the formula with number a_formula_number to a dot file.
    void save_dot_file(int a_formula_number);
  public:
    /// \brief Constructor that initializes Formula_Checker::f_counter_example, Formula_Checker::f_witness,
    /// \brief Formula_Checker::f_bdd_prover and Formula_Checker::f_dot_file_name.
    /// precondition: the argument passed as parameter a_time_limit is greater than or equal to 0. If the argument is equal
    /// to 0, no time limit will be enforced
    Formula_Checker(
      mcrl2::data::data_specification a_data_spec,
      RewriteStrategy a_rewrite_strategy = GS_REWR_JITTY,
      int a_time_limit = 0,
      bool a_path_eliminator = false,
      SMT_Solver_Type a_solver_type = solver_type_ario,
      bool a_apply_induction = false,
      bool a_counter_example = false,
      bool a_witness = false,
      char const* a_dot_file_name = 0
    );

    /// \brief Destructor without any specific functionality.
    ~Formula_Checker();

    /// \brief Checks the formulas in the list a_formulas.
    /// precondition: the parameter a_formulas is a list of expressions of sort Bool in internal mCRL2 format
    void check_formulas(ATermList a_formulas);
};

#endif
