#if !(defined(_MSC_VER) || defined(__MINGW32__) || defined(__CYGWIN__))
// Implementation of class SMT_Solver_Ario
// file: smt_solver_ario.cpp

#include "mcrl2/data/prover/smt_solver_ario.h"
#include "pstream.h"
#include "libprint_c.h"
#include <iostream>
#include "print/messaging.h"
#include "mcrl2/utilities/aterm_ext.h"

using namespace ::mcrl2::utilities;
using namespace std;

// Class SMT_Solver_Ario --------------------------------------------------------------------------
  // Class SMT_Solver_Ario - Functions declared public --------------------------------------------

    SMT_Solver_Ario::SMT_Solver_Ario() {
      string v_string_err;

      redi::pstream v_pstream("ario", redi::pstreams::pstdin | redi::pstreams::pstderr | redi::pstreams::pstdout);
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

      redi::pstream v_pstream("ario", redi::pstreams::pstdin | redi::pstreams::pstdout | redi::pstreams::pstderr);
      v_pstream << f_benchmark << endl << redi::peof;

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

        redi::pstream v_pstream("ario", redi::pstreams::pstdin | redi::pstreams::pstdout | redi::pstreams::pstderr);
        v_pstream << f_benchmark << endl << redi::peof;
        while (getline(v_pstream.out(), v_string_out)) {
          v_string_out = "Ario: " + v_string_out + "\n";
          gsErrorMsg((char*) v_string_out.c_str());
        }
        v_pstream.close();
        exit(1);
      }
    }

#endif // _MSC_VER
