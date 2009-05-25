// Author(s): Luc Engelen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/invariant_checker.h
/// \brief Add your file description here.

// Interface to class Invariant_Checker

#ifndef INVARIANT_CHECKER_H
#define INVARIANT_CHECKER_H

#include <string>
#include "aterm2.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/detail/bdd_prover.h"
#include "mcrl2/data/detail/prover/bdd2dot.h"

  /// The class Invariant_Checker is initialized with an LPS using the constructor Invariant_Checker::Invariant_Checker.
  /// After initialization, the function Invariant_Checker::check_invariant can be called any number of times to check
  /// whether an expression of sort Bool passed as argument a_invariant is an invariant of this LPS. A new instance of the
  /// class Invariant_Checker has to be created for each new LPS that has to be checked.
  ///
  /// The class Invariant_Checker uses an instance of the class BDD_Prover to check whether a formula is a valid invariant
  /// of an mCRL2 LPS. The constructor Invariant_Checker::Invariant_Checker initializes the BDD based prover with the
  /// parameters a_rewrite_strategy, a_time_limit, a_path_eliminator, a_solver_type and the data specification of the LPS
  /// passed as parameter a_lps. The parameter a_rewrite_strategy specifies which rewrite strategy is used by the prover's
  /// rewriter. It can be set to either GS_REWR_INNER, GS_REWR_INNERC, GS_REWR_JITTY or GS_REWR_JITTYC. The parameter
  /// a_time_limit specifies the maximum amount of time in seconds to be spent by the prover on proving a single expression.
  /// If a_time_limit is set to 0, no time limit will be enforced. The parameter a_path_eliminator specifies whether or not
  /// path elimination is applied. When path elimination is applied, the prover uses an SMT solver to remove inconsistent
  /// paths from BDDs. The parameter a_solver_type specifies which SMT solver is used for path elimination. Either the SMT
  /// solver ario (http://www.eecs.umich.edu/~ario/) or cvc-lite (http://www.cs.nyu.edu/acsys/cvcl/) can be used. To use one
  /// of these solvers, the directory containing the corresponding executable must be in the path. If the parameter
  /// a_path_eliminator is set to false, the parameter a_solver_type is ignored. The parameter a_apply_induction indicates
  /// whether or not induction on list will be applied.
  ///
  /// The parameter a_dot_file_name specifies whether a file in dot format of the resulting BDD is saved each time the
  /// prover cannot determine whether an expression is a contradiction or a tautology. If the parameter is set to 0, no .dot
  /// files are saved. If a string is passed as parameter a_dot_file_name, this string will be used as the prefix of the
  /// filenames. An instance of the class BDD2Dot is used to save these files in dot format.
  ///
  /// If the parameter a_counter_example is set to true, a so called counter example will be printed to stderr each time a
  /// summand is encountered that violates the invariant. A counter example is a valuation for which the expression to be
  /// proven does not hold. If the parameter a_all_violations is set to true, the invariant checker will not stop as soon as
  /// a violation of the invariant is found, but will report all violations instead.
  ///
  /// Given an LPS,
  ///
  ///    P(d: D) = ...
  ///            + sum ei: Ei. ci(d, ei) -> ai(fi(d, ei)) . P(gi(d, ei))
  ///            + ...;
  ///
  /// an instance of the class Invariant_Checker will generate a formula of the form
  ///
  ///    inv(d) /\ ci(d, ei) => inv(gi(d, ei))
  ///
  /// for each of the summands, where inv() is the expression passed as parameter a_invariant. If this expression passed as
  /// parameter a_invariant holds for the initial state and all the generated formulas are tautologies according to the
  /// prover, it is an invariant.

class Invariant_Checker {
  private:
    mcrl2::data::detail::BDD_Prover f_bdd_prover;
    BDD2Dot f_bdd2dot;
    ATermAppl f_init;
    ATermList f_summands;
    bool f_counter_example;
    bool f_all_violations;
    std::string f_dot_file_name;
    void print_counter_example();
    void save_dot_file(int a_summand_number);
    bool check_init(ATermAppl a_invariant);
    bool check_summand(ATermAppl a_invariant, ATermAppl a_summand, int a_summand_number);
    bool check_summands(ATermAppl a_invariant);
  public:

    /// precondition: the argument passed as parameter a_lps is a valid mCRL2 LPS
    /// precondition: the argument passed as parameter a_time_limit is greater than or equal to 0. If the argument is equal
    /// to 0, no time limit will be enforced
    Invariant_Checker(
      mcrl2::lps::specification const& a_lps,
      mcrl2::data::rewriter::strategy a_rewrite_strategy = mcrl2::data::rewriter::jitty,
      int a_time_limit = 0,
      bool a_path_eliminator = false,
      mcrl2::data::detail::SMT_Solver_Type a_solver_type = mcrl2::data::detail::solver_type_ario,
      bool a_apply_induction = false,
      bool a_counter_example = false,
      bool a_all_violations = false,
      std::string const& a_dot_file_name = std::string()
    );
    ~Invariant_Checker();

    /// precondition: the argument passed as parameter a_invariant is a valid expression in internal mCRL2 format
    bool check_invariant(ATermAppl a_invariant);
};

#endif
