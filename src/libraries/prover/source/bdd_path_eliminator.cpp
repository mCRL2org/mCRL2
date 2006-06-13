// Implementation of class BDD_Path_Eliminator
// file: bdd_path_eliminator.cpp

#include "bdd_path_eliminator.h"
#include "libstruct.h"

// Class BDD_Path_Eliminator ----------------------------------------------------------------------
  // Class BDD_Path_Eliminator - functions declared private ---------------------------------------

    ATermAppl BDD_Path_Eliminator::aux_simplify(ATermAppl a_bdd, ATermList a_path) {
      ATermList v_true_path;
      ATermList v_false_path;
      bool v_true_branch_enabled;
      bool v_false_branch_enabled;

      if (f_bdd_info.is_true(a_bdd) || f_bdd_info.is_false(a_bdd)) {
        return a_bdd;
      }

      v_true_path = ATinsert(a_path, (ATerm) f_bdd_info.get_guard(a_bdd));
      v_true_branch_enabled = f_smt_solver->is_satisfiable(v_true_path);
      v_false_path = ATinsert(a_path, (ATerm) gsMakeDataExprNot(f_bdd_info.get_guard(a_bdd)));
      v_false_branch_enabled = f_smt_solver->is_satisfiable(v_false_path);

      if (!v_true_branch_enabled) {
        return aux_simplify(f_bdd_info.get_false_branch(a_bdd), v_false_path);
      } else if (!v_false_branch_enabled) {
        return aux_simplify(f_bdd_info.get_true_branch(a_bdd), v_true_path);
      } else {
        return
          f_bdd_manipulator.make_reduced_if_then_else(
            f_bdd_info.get_guard(a_bdd),
            aux_simplify(f_bdd_info.get_true_branch(a_bdd), v_true_path),
            aux_simplify(f_bdd_info.get_false_branch(a_bdd), v_false_path)
          );
      }
    }

  // Class BDD_Path_Eliminator - functions declared public ----------------------------------------

    BDD_Path_Eliminator::BDD_Path_Eliminator(SMT_Solver_Type a_solver_type) {
      if (a_solver_type == solver_type_ario) {
        f_smt_solver = new SMT_Solver_Ario();
      } else if (a_solver_type == solver_type_cvc_lite) {
        f_smt_solver = new SMT_Solver_CVC_Lite();
      }
    }

    // --------------------------------------------------------------------------------------------

    ATermAppl BDD_Path_Eliminator::simplify(ATermAppl a_bdd) {
      return aux_simplify(a_bdd, ATmakeList0());
    }
