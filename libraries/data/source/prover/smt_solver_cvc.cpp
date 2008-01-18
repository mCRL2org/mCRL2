#if !(defined(_MSC_VER) || defined(__MINGW32__) || defined(__CYGWIN__))

// Implementation of class SMT_Solver_CVC
// file: smt_solver_cvc_lite.cpp

#include "mcrl2/data/prover/smt_solver_cvc.h"
#include "pstream.h"
#include "mcrl2/core/messaging.h"
#include <iostream>

using namespace ::mcrl2::utilities;
using namespace mcrl2::core;
using namespace std;

// Class SMT_Solver_CVC ----------------------------------------------------------------------
  // Class SMT_Solver_CVC - Functions declared public ----------------------------------------

    SMT_Solver_CVC::SMT_Solver_CVC() {
      string v_string_out;

      redi::pstream v_pstream("cvc3 -h", redi::pstreams::pstdin | redi::pstreams::pstderr | redi::pstreams::pstdout);
      getline(v_pstream.out(), v_string_out);
      if (v_string_out == "Usage: cvc3 [options]") {
        gsVerboseMsg("The SMT solver CVC3 is available.\n");
      } else {
        gsErrorMsg(
          "The SMT solver CVC3 is not available.\n"
          "Consult the manual of the tool you are using for instructions on how to obtain CVC3.\n"
        );
        exit(1);
      }
      v_pstream.close();
    }

    // --------------------------------------------------------------------------------------------

    bool SMT_Solver_CVC::is_satisfiable(ATermList a_formula) {
      translate(a_formula);

      redi::pstream v_pstream("cvc3 -lang smt-lib", redi::pstreams::pstdin | redi::pstreams::pstdout | redi::pstreams::pstderr);
      v_pstream << f_benchmark << endl << redi::peof;

      string v_string_out;
      getline(v_pstream.out(), v_string_out);
      v_pstream.close();
      if (v_string_out == "unsat") {
        gsVerboseMsg("The formula is unsatisfiable\n");
        return false;
      } else if (v_string_out == "sat") {
        gsVerboseMsg("The formula is satisfiable\n");
        return true;
      } else if (v_string_out == "unknown") {
        gsVerboseMsg("CVC3 cannot determine whether this formula is satisfiable or not.\n");
        return true;
      } else {
        gsErrorMsg("CVC3 reported an error while solving the formula.\n");
        redi::pstream v_pstream("cvc3 -lang smt-lib", redi::pstreams::pstdin | redi::pstreams::pstdout | redi::pstreams::pstderr);
        v_pstream << f_benchmark << endl << redi::peof;
        while (getline(v_pstream.err(), v_string_out)) {
          v_string_out = "CVC3 : " + v_string_out + "\n";
          gsErrorMsg((char*) v_string_out.c_str());
        }
        v_pstream.close();
        exit(1);
      }
    }

#endif // _MSC_VER
