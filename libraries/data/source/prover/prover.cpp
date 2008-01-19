// Implementation of class Prover
// file: prover.cpp

#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/prover.h"
#include "mcrl2/utilities/manipulator.h"
#include "mcrl2/utilities/info.h"
#include "mcrl2/core/struct.h"
#include "mcrl2/data/rewrite.h"
#include <cstdlib>
#include "mcrl2/utilities/utilities.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/utilities/aterm_ext.h"

using namespace ::mcrl2::utilities;
using namespace mcrl2::core;

// Class Prover -----------------------------------------------------------------------------------
  // Class Prover - Functions declared public -----------------------------------------------------

    Prover::Prover(
      mcrl2::data::data_specification data_spec,
      RewriteStrategy a_rewrite_strategy,
      int a_time_limit
    ) {
      f_time_limit = a_time_limit;
      gsDebugMsg("Flag:\n  Time limit: %d, \n", f_time_limit);
      f_processed = false;

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
