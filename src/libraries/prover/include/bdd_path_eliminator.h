// Interface to class BDD_Path_Eliminator
// file: bdd_path_eliminator.h

#ifndef BDD_PATH_ELIMINATOR_H
#define BDD_PATH_ELIMINATOR_H

#include "aterm2.h"
#include "bdd_simplifier.h"
#include "smt_solver_ario.h"
#include "smt_solver_cvc_lite.h"
#include "smt_solver_cvc_lite_fast.h"
#include "bdd_manipulator.h"
#include "bdd_info.h"

enum SMT_Solver_Type {
  solver_type_ario,
  solver_type_cvc_lite,
  solver_type_cvc_lite_fast
};

class BDD_Path_Eliminator: public BDD_Simplifier {
  private:
    ATermAppl aux_simplify(ATermAppl a_bdd, ATermList a_path);
    SMT_Solver* f_smt_solver;
    BDD_Info f_bdd_info;
    BDD_Manipulator f_bdd_manipulator;
  public:
    BDD_Path_Eliminator(SMT_Solver_Type a_solver_type);
    virtual ATermAppl simplify(ATermAppl a_bdd);
};

#endif
