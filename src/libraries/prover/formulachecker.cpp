// Implementation of class Formula_Checker
// file: formulachecker.cpp

#include "formulachecker.h"
#include "liblowlevel.h"
#include "libprint_c.h"

// Class Formula_Checker --------------------------------------------------------------------------
  // Class Formula_Checker - Functions declared public --------------------------------------------

    Formula_Checker::Formula_Checker(
      RewriteStrategy a_rewrite_strategy, int a_time_limit, ATermAppl a_data_equations, bool a_counter_example, bool a_witness
    ):
      f_bdd_prover(a_data_equations, a_rewrite_strategy, a_time_limit)
    {
      f_counter_example = a_counter_example;
      f_witness = a_witness;
    }

    // --------------------------------------------------------------------------------------------

    Formula_Checker::~Formula_Checker() {
      // Nothing to free.
    }

    // --------------------------------------------------------------------------------------------

    void Formula_Checker::check_formulas(ATermList a_formulas) {
      ATermAppl v_formula;

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
          if (f_counter_example) {
            gsfprintf(stderr, "  Counter-example: %P\n", f_bdd_prover.get_counter_example());
          }
          if (f_witness) {
            gsfprintf(stderr, "  Witness: %P\n", f_bdd_prover.get_witness());
          }
        }
        a_formulas = ATgetNext(a_formulas);
      }
    }
