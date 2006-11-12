// Implementation of class Formula_Checker
// file: formula_checker.cpp

#include "prover/formula_checker.h"
#include "liblowlevel.h"
#include "libprint_c.h"

// Class Formula_Checker --------------------------------------------------------------------------
  // Class Formula_Checker - Functions declared private -------------------------------------------

    void Formula_Checker::print_counter_example() {
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
          gsfprintf(stderr, "  Counter-example: %P\n", v_counter_example);
        }
      }
    }

    // ----------------------------------------------------------------------------------------------

    void Formula_Checker::print_witness() {
      if (f_witness) {
        ATermAppl v_witness;

        v_witness = f_bdd_prover.get_witness();
        if (v_witness == 0) {
          gsErrorMsg(
            "Cannot print witness. This is probably caused by an abrupt stop of the\n"
            "conversion from expression to EQ-BDD. This typically occurs when a time limit is set.\n"
          );
          exit(1);
        } else {
          gsfprintf(stderr, "  Witness: %P\n", v_witness);
        }
      }
    }

    // ----------------------------------------------------------------------------------------------

    void Formula_Checker::save_dot_file(int a_formula_number) {
      char* v_file_name;
      char* v_file_name_suffix;

      if (f_dot_file_name != 0) {
        v_file_name_suffix = (char*) malloc((number_of_digits(a_formula_number) + 6) * sizeof(char));
        sprintf(v_file_name_suffix, "-%d.dot", a_formula_number);
        v_file_name = (char*) malloc((strlen(f_dot_file_name) + strlen(v_file_name_suffix) + 1) * sizeof(char));
        strcpy(v_file_name, f_dot_file_name);
        strcat(v_file_name, v_file_name_suffix);
        f_bdd2dot.output_bdd(f_bdd_prover.get_bdd(), v_file_name);
      }
    }

  // Class Formula_Checker - Functions declared public --------------------------------------------

    Formula_Checker::Formula_Checker(
      ATermAppl a_lpe, RewriteStrategy a_rewrite_strategy, int a_time_limit, bool a_path_eliminator, SMT_Solver_Type a_solver_type,
      bool a_apply_induction, bool a_counter_example, bool a_witness, char* a_dot_file_name
    ):
      f_bdd_prover(a_lpe, a_rewrite_strategy, a_time_limit, a_path_eliminator, a_solver_type, a_apply_induction)
    {
      f_counter_example = a_counter_example;
      f_witness = a_witness;
      if (a_dot_file_name == 0) {
        f_dot_file_name = 0;
      } else {
        f_dot_file_name = strdup(a_dot_file_name);
      }
    }

    // --------------------------------------------------------------------------------------------

    Formula_Checker::~Formula_Checker() {
      // Nothing to free.
    }

    // --------------------------------------------------------------------------------------------

    void Formula_Checker::check_formulas(ATermList a_formulas) {
      ATermAppl v_formula;
      int v_formula_number = 1;

      while (!ATisEmpty(a_formulas)) {
        v_formula = ATAgetFirst(a_formulas);
        gsfprintf(stderr, "'%P': ", v_formula);
        f_bdd_prover.set_formula(v_formula);
        Answer v_is_tautology = f_bdd_prover.is_tautology();
        Answer v_is_contradiction = f_bdd_prover.is_contradiction();
        if (v_is_tautology == answer_yes) {
          gsfprintf(stderr, "Tautology\n");
        } else if (v_is_contradiction == answer_yes) {
          gsfprintf(stderr, "Contradiction\n");
        } else {
          gsfprintf(stderr, "Undeterminable\n");
          print_counter_example();
          print_witness();
          save_dot_file(v_formula_number);
        }
        a_formulas = ATgetNext(a_formulas);
        v_formula_number++;
      }
    }
