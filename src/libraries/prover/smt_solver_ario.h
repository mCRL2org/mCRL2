// Interface to class SMT_Solver_Ario
// file: smt_solver_ario.h

#ifndef SMT_SOLVER_ARIO_H
#define SMT_SOLVER_ARIO_H

#include "aterm2.h"
#include "smt_lib_solver.h"

class SMT_Solver_Ario: public SMT_LIB_Solver {
  public:
    SMT_Solver_Ario();
    virtual bool is_satisfiable(ATermList a_formula);
};

#endif
