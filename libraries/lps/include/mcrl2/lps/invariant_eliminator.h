// Author(s): Luc Engelen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/invariant_eliminator.h
/// \brief Add your file description here.

// Interface to class Invariant_Eliminator
// file: invariant_eliminator.h

#ifndef INVARIANT_ELIMINATOR_H
#define INVARIANT_ELIMINATOR_H

#include "aterm2.h"
#include "mcrl2/data/rewrite.h"
#include "mcrl2/data/bdd_prover.h"

  /// The class Invariant_Eliminator is initialized with an LPS using the constructor
  /// Invariant_Eliminator::Invariant_Eliminator. After initialization, the method Invariant_Eliminator::simplify can be
  /// called any number of times to simplify or eliminate the summands of this LPS using an invariant. A new instance of the
  /// class Invariant_Eliminator has to be created for each new LPS that has to be processed.
  ///
  /// The class Invariant_Eliminator uses an instance of the class BDD_Prover to simplify or eliminate summands of an mCRL2
  /// LPS. The constructor Invariant_Eliminator::Invariant_Eliminator initializes the BDD based prover with the parameters
  /// a_rewrite_strategy, a_time_limit, a_path_eliminator, a_solver_type, and the data specification of the LPS passed as
  /// parameter a_lps. The parameter a_rewrite_strategy specifies which rewrite strategy is used by the prover's rewriter.
  /// It can be set to either GS_REWR_INNER, GS_REWR_INNERC, GS_REWR_JITTY or GS_REWR_JITTYC. The parameter a_time_limit
  /// specifies the maximum amount of time in seconds to be spent by the prover on proving a single expression. If
  /// a_time_limit is set to 0, no time limit will be enforced. The parameter a_path_eliminator specifies whether or not
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
  /// If the parameter a_simplify_all is set to false, the summands whose conditions in conjunction with the invariant prove
  /// to be a contradiction are eliminated. If parameter a_simplify_all is set to true, in addition to removing those
  /// summands, the conditions of all other summands of the LPS are simplified when calling method
  /// Invariant_Eliminator::simplify.
  ///
  /// The method Invariant_Eliminator::simplify returns the LPS passed as parameter a_lps of the constructor after
  /// simplification or elimination using the invariant passed as parameter a_invariant. If a_summand_number differs from 0,
  /// only the summand with the number passed as parameter a_summand_number will be eliminated or simplified. One instance
  /// of the class Invariant_Eliminator can check any number of invariants on the same LPS. For each new LPS to be checked,
  /// a new instance of the class has to be created.
  ///
  /// Given an LPS,
  ///
  ///    P(d: D) = ...
  ///            + sum ei: Ei. ci(d, ei) -> ai(fi(d, ei)) . P(gi(d, ei))
  ///            + ...;
  ///
  /// an instance of the class Invariant_Eliminator will generate a formula of the form
  ///
  ///    inv(d) /\ ci(d, ei)
  ///
  /// for each of the summands or for the chosen summand only, where inv() is the invariant passed as parameter a_invariant.
  /// If such a formula is a contradiction according to the prover, the corresponding summand will be eliminated. If the
  /// parameter a_simplify_all is set, the condition of each summand is replaced by the BDD obtained from the prover after
  /// proving that summand's formula.

class Invariant_Eliminator {
  private:
    BDD_Prover f_bdd_prover;
    ATermAppl f_lps;
    bool f_simplify_all;
    ATermAppl simplify_summand(ATermAppl a_invariant, ATermAppl a_summand, int a_summand_number);
  public:
    /// precondition: the argument passed as parameter a_lps is a valid mCRL2 LPS
    /// precondition: the argument passed as parameter a_time_limit is greater than or equal to 0. If the argument is equal
    /// to 0, no time limit will be enforced
    Invariant_Eliminator(
      ATermAppl a_lps,
      RewriteStrategy a_rewrite_strategy = GS_REWR_JITTY,
      int a_time_limit = 0,
      bool a_path_eliminator = false,
      SMT_Solver_Type a_solver_type = solver_type_ario,
      bool a_apply_induction = false,
      bool a_simplify_all = false
    );
    ~Invariant_Eliminator();

    /// precondition: the argument passed as parameter a_invariant is a valid expression of sort Bool in internal mCRL2
    /// format
    /// precondtition: the argument passed as parameter a_summand_number is greater than or equal to 0. If the argument is
    /// equal to 0, all summands are checked to see whether they can be simplified or eliminated. If the argument is greater
    /// than the number of summands, no summand will be simplified or eliminated
    ATermAppl simplify(ATermAppl a_invariant, int a_summand_number);
};

#endif
