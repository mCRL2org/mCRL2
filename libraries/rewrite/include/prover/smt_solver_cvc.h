// Interface to class SMT_Solver_CVC
// file: smt_solver_CVC_Lite.h

#ifndef SMT_SOLVER_CVC_H
#define SMT_SOLVER_CVC_H

#include "aterm2.h"
#include "smt_lib_solver.h"

  /// The class SMT_Solver_CVC inherits from the class SMT_LIB_Solver. It uses the SMT solver CVC Lite
  /// (http://www.cs.nyu.edu/acsys/cvcl/) to determine the satisfiability of propositional formulas. To use the solver CVC
  /// Lite, the directory containing the corresponding executable must be in the path.
  ///
  /// The constructor SMT_Solver_CVC::SMT_Solver_CVC checks if CVC Lite's executable is indeed available and
  /// reports an error if it is not.
  ///
  /// The method SMT_Solver_CVC::is_satisfiable receives a formula in conjunctive normal form as parameter a_formula and
  /// indicates whether or not this formula is satisfiable. 

class SMT_Solver_CVC: public SMT_LIB_Solver {
  public:
    SMT_Solver_CVC();

    /// precondition: The argument passed as parameter a_formula is a list of expressions of sort Bool in internal mCRL2
    /// format. The argument represents a formula in conjunctive normal form, where the elements of the list represent the
    /// clauses
    virtual bool is_satisfiable(ATermList a_formula);
};

#endif
