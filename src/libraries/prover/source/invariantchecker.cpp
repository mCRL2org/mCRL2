// Implementation of class Invariant_Checker
// file: invariantchecker.cpp

#include "invariantchecker.h"
#include "liblowlevel.h"
#include "libprint_c.h"
#include "libstruct.h"
#include "bddprover.h"

// Class Invariant_Checker ------------------------------------------------------------------------
  // Class Invariant_Checker - Functions declared private -----------------------------------------

    bool Invariant_Checker::check_init(ATermAppl a_invariant) {
      ATermList v_assignments = ATLgetArgument(f_init, 1);
      ATermAppl v_assignment;
      ATermAppl v_variable;
      ATermAppl v_expression;
      ATermList v_substitutions = ATmakeList0();
      ATermAppl v_substitution;

      while (!ATisEmpty(v_assignments)) {
        v_assignment = ATAgetFirst(v_assignments);
        v_variable = ATAgetArgument(v_assignment, 0);
        v_expression = ATAgetArgument(v_assignment, 1);
        v_substitution = gsMakeSubst_Appl(v_variable, v_expression);
        v_substitutions = ATinsert(v_substitutions, (ATerm) v_substitution);
        v_assignments = ATgetNext(v_assignments);
      }
      a_invariant = gsSubstValues_Appl(v_substitutions, a_invariant, true);
      f_bdd_prover.set_formula(a_invariant);
      if (f_bdd_prover.is_tautology() == answer_yes) {
        return true;
      } else {
        if (f_bdd_prover.is_contradiction() != answer_yes) {
          if (f_counter_example) {
            gsfprintf(stderr, "  Counter-example: %P\n", f_bdd_prover.get_counter_example());
          }
        }
        return false;
      }
    }

    // --------------------------------------------------------------------------------------------

    bool Invariant_Checker::check_summand(ATermAppl a_invariant, ATermAppl a_summand, int a_summand_number) {
      ATermAppl v_condition = ATAgetArgument(a_summand, 1);
      ATermList v_assignments = ATLgetArgument(a_summand, 4);
      ATermAppl v_assignment;
      ATermAppl v_variable;
      ATermAppl v_expression;
      ATermList v_substitutions = ATmakeList0();
      ATermAppl v_substitution;

      while (!ATisEmpty(v_assignments)) {
        v_assignment = ATAgetFirst(v_assignments);
        v_variable = ATAgetArgument(v_assignment, 0);
        v_expression = ATAgetArgument(v_assignment, 1);
        v_substitution = gsMakeSubst_Appl(v_variable, v_expression);
        v_substitutions = ATinsert(v_substitutions, (ATerm) v_substitution);
        v_assignments = ATgetNext(v_assignments);
      }

      ATermAppl v_subst_invariant = gsSubstValues_Appl(v_substitutions, a_invariant, true);
      ATermAppl v_formula = gsMakeDataExprAnd(a_invariant, v_condition);

      v_formula = gsMakeDataExprImp(v_formula, v_subst_invariant);
      f_bdd_prover.set_formula(v_formula);
      if (f_bdd_prover.is_tautology() == answer_yes) {
        gsVerboseMsg("The invariant holds for summand %d.\n", a_summand_number);
        return true;
      } else {
        gsfprintf(stderr, "The invariant does not hold for summand %d.\n", a_summand_number);
        if (f_bdd_prover.is_contradiction() != answer_yes) {
          if (f_counter_example) {
            gsfprintf(stderr, "  Counter-example: %P\n", f_bdd_prover.get_counter_example());
          }
        }
        return false;
      }
    }

    // --------------------------------------------------------------------------------------------

    bool Invariant_Checker::check_summands(ATermAppl a_invariant) {
      ATermList v_summands = f_summands;
      ATermAppl v_summand;
      bool v_result = true;
      int v_summand_number = 1;

      while (!ATisEmpty(v_summands) && (f_all || v_result) ) {
        v_summand = ATAgetFirst(v_summands);
        v_result = check_summand(a_invariant, v_summand, v_summand_number) && v_result;
        v_summands = ATgetNext(v_summands);
        v_summand_number++;
      }
      return v_result;
    }

  // Class Invariant_Checker - Functions declared public --------------------------------------------

    Invariant_Checker::Invariant_Checker(
      RewriteStrategy a_rewrite_strategy, int a_time_limit, ATermAppl a_lpe, bool a_counter_example, bool a_all
    ):
      f_bdd_prover(ATAgetArgument(a_lpe, 3), a_rewrite_strategy, a_time_limit)
    {
      f_init = ATAgetArgument(a_lpe, 6);
      f_summands = ATLgetArgument(ATAgetArgument(a_lpe, 5), 2);
      f_counter_example = a_counter_example;
      f_all = a_all;
    }

    // --------------------------------------------------------------------------------------------

    Invariant_Checker::~Invariant_Checker() {
      // Nothing to free.
    }

    // --------------------------------------------------------------------------------------------

    bool Invariant_Checker::check_invariant(ATermAppl a_invariant) {
      bool v_result = true;

      if (check_init(a_invariant)) {
        gsVerboseMsg("The invariant holds for the initial state.\n");
      } else {
        gsfprintf(stderr, "The invariant does not hold for the initial state.\n");
        v_result = false;
      }
      if ((f_all || v_result)) {
        if (check_summands(a_invariant)) {
          gsVerboseMsg("The invariant holds for all summands.\n");
        } else {
          gsfprintf(stderr, "The invariant does not hold for all summands.\n");
          v_result = false;
        }
      }
      if (v_result) {
        gsfprintf(stderr, "The invariant holds for this LPE.\n");
      } else {
        gsfprintf(stderr, "The invariant does not hold for this LPE.\n");
      }

      return v_result;
    }
