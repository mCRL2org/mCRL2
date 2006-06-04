// Implementation of class SMT_Solver_CVC_Lite
// file: smt_solver_cvc_lite.cpp

#include "smt_solver_cvc_lite.h"
#include "pstream.h"
#include "libprint_c.h"
#include <iostream>

using namespace std;
using namespace redi;

// Class SMT_Solver_CVC_Lite ----------------------------------------------------------------------
  // Class SMT_Solver_CVC_Lite - Functions declared public ----------------------------------------

    SMT_Solver_CVC_Lite::SMT_Solver_CVC_Lite() {

    }

    // --------------------------------------------------------------------------------------------

    bool SMT_Solver_CVC_Lite::is_satisfiable(ATermList a_formula) {
      translate(a_formula);

      pstream v_pstream("cvcl -lang smt-lib", pstreams::pstdin | pstreams::pstdout | pstreams::pstderr);
      v_pstream << f_benchmark << endl << peof;

      string v_string_out;
      getline(v_pstream.out(), v_string_out);
      cout << "\n\nopgevangen via stdout: " << v_string_out << endl;
      if (v_string_out == "Unsatisfiable.") {
        cout << "Unsatisfiable\n\n" << endl;
        return false;
      } else if (v_string_out == "Satisfiable.") {
        cout << "Satisfiable\n\n" << endl;
        return true;
      } else {
        gsErrorMsg("CVC Lite cannot determine whether this formula is satisfiable or not.");
        exit(1);
      }
    }
