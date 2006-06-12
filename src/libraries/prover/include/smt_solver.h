// Interface to class SMT_Solver
// file: smt_solver.h

#ifndef SMT_SOLVER_H
#define SMT_SOLVER_H

#include "aterm2.h"

class SMT_Solver {
  public:
    virtual ~SMT_Solver() = 0;
    virtual bool is_satisfiable(ATermList a_formula) = 0;
};

#endif
