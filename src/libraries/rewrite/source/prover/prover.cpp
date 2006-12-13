// Implementation of class Prover
// file: prover.cpp

#include "lpe/data_specification.h"
#include "prover/prover.h"
#include "auxiliary/manipulator.h"
#include "auxiliary/info.h"
#include "libstruct.h"
#include "librewrite.h"
#include "cstdlib"
#include "auxiliary/utilities.h"
#include "liblowlevel.h"
#include "libprint_c.h"

// Class Prover -----------------------------------------------------------------------------------
  // Class Prover - Functions declared public -----------------------------------------------------

    Prover::Prover(
      lpe::data_specification data_spec,
      RewriteStrategy a_rewrite_strategy,
      int a_time_limit
    ) {
      f_time_limit = a_time_limit;
      gsDebugMsg("Flag:\n  Time limit: %d, \n", f_time_limit);
      f_processed = false;

      gsEnableConstructorFunctions();
      switch (a_rewrite_strategy) {
        case (GS_REWR_INNER): {
          f_rewriter = createRewriter(data_spec, GS_REWR_INNER);
          f_info = new AI_Inner(f_rewriter);
          f_manipulator = new AM_Inner(f_rewriter, f_info);
          gsDebugMsg(
            "Using innermost rewrite strategy.\n"
            "Rewriter, ATerm_Info and ATerm_Manipulator have been initialized.\n"
          );
          break;
        }
        case (GS_REWR_JITTY): {
          f_rewriter = createRewriter(data_spec, GS_REWR_JITTY);
          f_info = new AI_Jitty(f_rewriter);
          f_manipulator = new AM_Jitty(f_rewriter, f_info);
          gsDebugMsg(
            "Using jitty rewrite strategy.\n"
            "Rewriter, ATerm_Info and ATerm_Manipulator have been initialized.\n"
          );
          break;
        }
        case (GS_REWR_INNERC): {
          gsErrorMsg("The compiled innermost rewriter is not supported by the prover.\n");
          exit(1);
          break;
        }
        case (GS_REWR_JITTYC): {
          gsErrorMsg("The compiled jitty rewriter is not supported by the prover.\n");
          exit(1);
          break;
        }
        default: {
          gsErrorMsg("Unknown type of rewriter.\n");
          exit(1);
          break;
        }
      }
    }

    // --------------------------------------------------------------------------------------------

    Prover::~Prover() {
      delete f_manipulator;
      f_manipulator = 0;
      delete f_info;
      f_info = 0;
      delete f_rewriter;
      f_rewriter = 0;
      gsDebugMsg("Rewriter, ATerm_Info and ATerm_Manipulator have been freed.\n");
    }

    // --------------------------------------------------------------------------------------------

    void Prover::set_formula(ATermAppl a_formula) {
      f_formula = a_formula;
      f_processed = false;
      gsDebugMsg("The formula has been set.\n");
    }

    // --------------------------------------------------------------------------------------------

    void Prover::set_time_limit(int a_time_limit) {
      f_time_limit = a_time_limit;
    }

    // --------------------------------------------------------------------------------------------

    Rewriter *Prover::get_rewriter() {
      return f_rewriter;
    }
