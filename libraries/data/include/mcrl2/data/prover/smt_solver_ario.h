// Interface to class SMT_Solver_Ario
// file: smt_solver_ario.h

#ifndef SMT_SOLVER_ARIO_H
#define SMT_SOLVER_ARIO_H

#include "aterm2.h"
#include "smt_lib_solver.h"

  /// The class SMT_Solver_Ario inherits from the class SMT_LIB_Solver. It uses the SMT solver Ario 1.1
  /// (http://www.eecs.umich.edu/~ario/) to determine the satisfiability of propositional formulas. To use the solver Ario
  /// 1.1, the directory containing the corresponding executable must be in the path.
  ///
  /// The constructor SMT_Solver_Ario::SMT_Solver_Ario checks if Ario's executable is indeed available and reports an error
  /// if it is not.
  ///
  /// The method SMT_Solver_Ario::is_satisfiable receives a formula in conjunctive normal form as parameter a_formula and
  /// indicates whether or not this formula is satisfiable. 

class SMT_Solver_Ario: public SMT_LIB_Solver {
  public:
    SMT_Solver_Ario();

    /// precondition: The argument passed as parameter a_formula is a list of expressions of sort Bool in internal mCRL2
    /// format. The argument represents a formula in conjunctive normal form, where the elements of the list represent the
    /// clauses
    virtual bool is_satisfiable(ATermList a_formula);
};

#endif
