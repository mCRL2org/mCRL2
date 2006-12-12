#if !(defined(_MSC_VER) || defined(__MINGW32__))
// Implementation of class SMT_Solver_Ario
// file: smt_solver_ario.cpp

#include "prover/smt_solver_ario.h"
#include "pstream.h"
#include "libprint_c.h"
#include <iostream>

using namespace std;
using namespace redi;

// Class SMT_Solver_Ario --------------------------------------------------------------------------
  // Class SMT_Solver_Ario - Functions declared public --------------------------------------------

    SMT_Solver_Ario::SMT_Solver_Ario() {
      string v_string_err;

      pstream v_pstream("ario", pstreams::pstdin | pstreams::pstderr | pstreams::pstdout);
      getline(v_pstream.err(), v_string_err);
      if (v_string_err == "....................................") {
        gsVerboseMsg("The SMT solver Ario is available.\n");
      } else {
        gsErrorMsg(
          "The SMT solver Ario is not available.\n"
          "Consult the manual of the tool you are using for instructions on how to obtain Ario.\n"
        );
        exit(1);
      }
      v_pstream.close();
    }

    // --------------------------------------------------------------------------------------------

    bool SMT_Solver_Ario::is_satisfiable(ATermList a_formula) {
      translate(a_formula);

      pstream v_pstream("ario", pstreams::pstdin | pstreams::pstdout | pstreams::pstderr);
      v_pstream << f_benchmark << endl << peof;

      string v_string_out;
      getline(v_pstream.out(), v_string_out);
      v_pstream.close();
      if (v_string_out == "unsat") {
        gsVerboseMsg("The formula is unsatisfiable.\n");
        return false;
      } else if (v_string_out == "sat") {
        gsVerboseMsg("The formula is satisfiable.\n");
        return true;
      } else {
        gsErrorMsg("Ario reported an error while solving the formula.\n");

        pstream v_pstream("ario", pstreams::pstdin | pstreams::pstdout | pstreams::pstderr);
        v_pstream << f_benchmark << endl << peof;
        while (getline(v_pstream.out(), v_string_out)) {
          v_string_out = "Ario: " + v_string_out + "\n";
          gsErrorMsg((char*) v_string_out.c_str());
        }
        v_pstream.close();
        exit(1);
      }
    }

#endif // _MSC_VER
