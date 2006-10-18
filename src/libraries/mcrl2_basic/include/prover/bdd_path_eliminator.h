// Interface to class BDD_Path_Eliminator
// file: bdd_path_eliminator.h

#ifndef BDD_PATH_ELIMINATOR_H
#define BDD_PATH_ELIMINATOR_H

#include "aterm2.h"
#include "prover/bdd_simplifier.h"
#include "prover/smt_solver_ario.h"
#include "prover/smt_solver_cvc_lite.h"
#include "prover/smt_solver_cvc_lite_fast.h"
#include "auxiliary/bdd_manipulator.h"
#include "auxiliary/bdd_info.h"

  /// \brief The enumaration type SMT_Solver_Type enumerates all available SMT solvers. 
enum SMT_Solver_Type {
  solver_type_ario,
  solver_type_cvc_lite,
  solver_type_cvc_lite_fast
};

  /// \brief The class BDD_Path_Eliminator is a base class for classes that eliminate inconsistent paths from BDDs.

class BDD_Path_Eliminator: public BDD_Simplifier {
  private:
    /// \brief Returns a list representing the conjunction of all guards in a_path and the guard a_guard.
    ATermList create_condition(ATermList a_path, ATermAppl a_guard, bool a_minimal);

    /// \brief Simplifies the BDD a_bdd using path a_path. Paths whose guards in conjunction with the guards in
    /// \brief a_path are inconsistent are removed.
    ATermAppl aux_simplify(ATermAppl a_bdd, ATermList a_path);

    /// \brief Returns true if the expression a_expression_1 has variables in common with expression a_expression_2.
    bool variables_overlap(ATermAppl a_expression_1, ATermAppl a_expression_2);

    /// \brief Pointer to an SMT solver used to determine whether or not a path is inconsistent.
    SMT_Solver* f_smt_solver;

    /// \brief Class that provides information about the structure of BDDs.
    BDD_Info f_bdd_info;

    /// \brief Class that can be used to manipulate BDDs.
    BDD_Manipulator f_bdd_manipulator;

    /// \brief Class that provides information about expressions.
    Expression_Info f_expression_info;
  public:
    /// \brief Constructor that initializes the field BDD_Path_Eliminator::f_smt_solver.
    BDD_Path_Eliminator(SMT_Solver_Type a_solver_type);

    /// \brief Returns a BDD without inconsistent paths, equivalent to a_bdd.
    virtual ATermAppl simplify(ATermAppl a_bdd);
};

#endif
