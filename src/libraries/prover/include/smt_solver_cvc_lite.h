// Interface to class SMT_Solver_CVC_Lite
// file: smt_solver_CVC_Lite.h

#ifndef SMT_SOLVER_CVC_Lite_H
#define SMT_SOLVER_CVC_Lite_H

#include "aterm2.h"
#include "smt_lib_solver.h"

class SMT_Solver_CVC_Lite: public SMT_LIB_Solver {
  public:
    SMT_Solver_CVC_Lite();
    bool is_satisfiable(ATermList a_formula);
};

#endif
