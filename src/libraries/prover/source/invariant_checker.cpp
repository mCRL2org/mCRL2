// Implementation of class Invariant_Checker
// file: invariant_checker.cpp

#include "invariant_checker.h"
#include "liblowlevel.h"
#include "libprint_c.h"
#include "libstruct.h"
#include "prover/bdd_prover.h"

// Class Invariant_Checker ------------------------------------------------------------------------
  // Class Invariant_Checker - Functions declared private -----------------------------------------

    void Invariant_Checker::print_counter_example() {
      if (f_counter_example) {
        ATermAppl v_counter_example;

        v_counter_example = f_bdd_prover.get_counter_example();
        if (v_counter_example == 0) {
          gsErrorMsg(
            "Cannot print counter example. This is probably caused by an abrupt stop of the\n"
            "conversion from expression to EQ-BDD. This typically occurs when a time limit is set.\n"
          );
          exit(1);
        } else {
          gsfprintf(stderr, "  Counter example: %P\n", v_counter_example);
        }
      }
    }

    // --------------------------------------------------------------------------------------------

    void Invariant_Checker::save_dot_file(int a_summand_number) {
      char* v_file_name;
      char* v_file_name_suffix;

      if (f_dot_file_name != 0) {
        if (a_summand_number == -1) {
          v_file_name_suffix = (char*) malloc(10 * sizeof(char));
          v_file_name_suffix = "-init.dot";
        } else {
          v_file_name_suffix = (char*) malloc((number_of_digits(a_summand_number) + 6) * sizeof(char));
          sprintf(v_file_name_suffix, "-%d.dot", a_summand_number);
        }
        v_file_name = (char*) malloc((strlen(f_dot_file_name) + strlen(v_file_name_suffix) + 1) * sizeof(char));
        strcpy(v_file_name, f_dot_file_name);
        strcat(v_file_name, v_file_name_suffix);
        f_bdd2dot.output_bdd(f_bdd_prover.get_bdd(), v_file_name);
        free(v_file_name);
        v_file_name = 0;
        free(v_file_name_suffix);
        v_file_name_suffix = 0;
      }
    }

    // --------------------------------------------------------------------------------------------

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
          print_counter_example();
          save_dot_file(-1);
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
          print_counter_example();
          save_dot_file(a_summand_number);
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

      while (!ATisEmpty(v_summands) && (f_all_violations || v_result) ) {
        v_summand = ATAgetFirst(v_summands);
        v_result = check_summand(a_invariant, v_summand, v_summand_number) && v_result;
        v_summands = ATgetNext(v_summands);
        v_summand_number++;
      }
      return v_result;
    }

  // Class Invariant_Checker - Functions declared public --------------------------------------------

    Invariant_Checker::Invariant_Checker(
      ATermAppl a_lpe, RewriteStrategy a_rewrite_strategy, int a_time_limit, bool a_path_eliminator, SMT_Solver_Type a_solver_type,
      bool a_apply_induction, bool a_counter_example, bool a_all_violations, char* a_dot_file_name
    ):
      f_bdd_prover(a_lpe, a_rewrite_strategy, a_time_limit, a_path_eliminator, a_solver_type, a_apply_induction)
    {
      f_init = ATAgetArgument(a_lpe, 3);
      f_summands = ATLgetArgument(ATAgetArgument(a_lpe, 2), 2);
      f_counter_example = a_counter_example;
      f_all_violations = a_all_violations;
      if (a_dot_file_name == 0) {
        f_dot_file_name = 0;
      } else {
        f_dot_file_name = strdup(a_dot_file_name);
      }
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
      if ((f_all_violations || v_result)) {
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
