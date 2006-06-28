// Implementation of class Invariant_Eliminator
// file: invariant_eliminator.cpp

#include "invariant_eliminator.h"
#include "liblowlevel.h"
#include "libprint_c.h"
#include "libstruct.h"
#include "bdd_prover.h"

// Class Invariant_Eliminator ---------------------------------------------------------------------
  // Class Invariant_Eliminator - Functions declared private --------------------------------------

    ATermAppl Invariant_Eliminator::simplify_summand(ATermAppl a_summand, ATermAppl a_invariant, int a_summand_number) {
      ATermAppl v_condition = ATAgetArgument(a_summand, 1);
      ATermAppl v_formula = gsMakeDataExprAnd(a_invariant, v_condition);

      f_bdd_prover.set_formula(v_formula);
      if (f_bdd_prover.is_contradiction() == answer_yes) {
        return 0;
      } else {
        if (f_simplify_all) {
          a_summand = ATsetArgument(a_summand, (ATerm) f_bdd_prover.get_bdd(), 1);
          gsfprintf(stderr, "Summand number %d is simplified.\n", a_summand_number);
        }
        return a_summand;
       }
    }

  // Class Invariant_Eliminator - Functions declared public ---------------------------------------

    Invariant_Eliminator::Invariant_Eliminator(
      ATermAppl a_lpe, RewriteStrategy a_rewrite_strategy, int a_time_limit, bool a_path_eliminator, SMT_Solver_Type a_solver_type, bool a_simplify_all
    ):
      f_bdd_prover(ATAgetArgument(a_lpe, 3), a_rewrite_strategy, a_time_limit)
    {
      f_lpe = a_lpe;
      f_simplify_all = a_simplify_all;
    }

    // --------------------------------------------------------------------------------------------

    Invariant_Eliminator::~Invariant_Eliminator() {
      // Nothing to free.
    }

    // --------------------------------------------------------------------------------------------

    ATermAppl Invariant_Eliminator::simplify(ATermAppl a_invariant, int a_summand_number) {
      ATermList v_summands = ATLgetArgument(ATAgetArgument(f_lpe, 5), 2);
      ATermAppl v_summand;
      ATermList v_simplified_summands = ATmakeList0();
      int v_summand_number = 1;

      while (!ATisEmpty(v_summands)) {
        v_summand = ATAgetFirst(v_summands);
        if ((a_summand_number == v_summand_number) || (a_summand_number == 0)) {
          v_summand = simplify_summand(v_summand, a_invariant, v_summand_number);
        }
        if (v_summand != 0) {
          gsVerboseMsg("Summand number %d could not be eliminated.\n", v_summand_number);
          v_simplified_summands = ATinsert(v_simplified_summands, (ATerm) v_summand);
        } else {
          gsfprintf(stderr, "Summand number %d is eliminated.\n", v_summand_number);
        }
        v_summands = ATgetNext(v_summands);
        v_summand_number++;
      }
      v_simplified_summands = ATreverse(v_simplified_summands);
      ATermAppl v_process_equations = ATAgetArgument(f_lpe, 5);
      v_process_equations = ATsetArgument(v_process_equations, (ATerm) v_simplified_summands, 2);
      ATermAppl v_lpe = ATsetArgument(f_lpe, (ATerm) v_process_equations, 5);
      return v_lpe;
    }
