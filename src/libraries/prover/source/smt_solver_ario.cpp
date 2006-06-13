// Implementation of class SMT_Solver_Ario
// file: smt_solver_ario.cpp

#include "smt_solver_ario.h"
#include "pstream.h"
#include "libprint_c.h"
#include <iostream>

using namespace std;
using namespace redi;

// Class SMT_Solver_Ario --------------------------------------------------------------------------
  // Class SMT_Solver_Ario - Functions declared public --------------------------------------------

    bool SMT_Solver_Ario::is_satisfiable(ATermList a_formula) {
      translate(a_formula);

      pstream v_pstream("ario", pstreams::pstdin | pstreams::pstdout | pstreams::pstderr);
      v_pstream << f_benchmark << endl << peof;

      string v_string_out;
      getline(v_pstream.out(), v_string_out);
      cout << "\n\nopgevangen via stdout: " << v_string_out << endl;
      if (v_string_out == "unsat") {
        cout << "Unsatisfiable\n\n" << endl;
        return false;
      } else if (v_string_out == "sat") {
        cout << "Satisfiable\n\n" << endl;
        return true;
      } else {
        gsErrorMsg("Ario cannot determine whether this formula is satisfiable or not.\n");
        exit(1);
      }
    }
