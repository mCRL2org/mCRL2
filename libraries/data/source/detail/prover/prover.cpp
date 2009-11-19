// Author(s): Luc Engelen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <cstdlib>

#include "mcrl2/data/data_specification.h"
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/data/detail/rewrite.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/exception.h"
#include "mcrl2/data/detail/prover.h"
#include "mcrl2/data/detail/prover/manipulator.h"
#include "mcrl2/data/detail/prover/utilities.h"
#include "mcrl2/data/detail/prover/info.h"
#include "mcrl2/data/data_specification.h"

using namespace mcrl2::core;

namespace mcrl2 {
  namespace data {
    namespace detail {

// Class Prover -----------------------------------------------------------------------------------
  // Class Prover - Functions declared public -----------------------------------------------------

    Prover::Prover(
      const data_specification &data_spec,
      mcrl2::data::rewriter::strategy a_rewrite_strategy,
      int a_time_limit
    ) {
      f_time_limit = a_time_limit;
      gsDebugMsg("Flag:\n  Time limit: %d, \n", f_time_limit);
      f_processed = false;

      switch (a_rewrite_strategy) {
        case (mcrl2::data::rewriter::innermost): {
          f_rewriter = createRewriter(data_spec, GS_REWR_INNER);
          f_info = new AI_Inner(f_rewriter);
          f_manipulator = new AM_Inner(f_rewriter, f_info);
          gsDebugMsg(
            "Using innermost rewrite strategy.\n"
            "Rewriter, ATerm_Info and ATerm_Manipulator have been initialized.\n"
          );
          break;
        }
        case (mcrl2::data::rewriter::jitty): {
          f_rewriter = createRewriter(data_spec, GS_REWR_JITTY);
          f_info = new AI_Jitty(f_rewriter);
          f_manipulator = new AM_Jitty(f_rewriter, f_info);
          gsDebugMsg(
            "Using jitty rewrite strategy.\n"
            "Rewriter, ATerm_Info and ATerm_Manipulator have been initialized.\n"
          );
          break;
        }
#ifdef MCRL2_INNERC_AVAILABLE
        case (mcrl2::data::rewriter::innermost_compiling): {
          throw mcrl2::runtime_error("The compiled innermost rewriter is not supported by the prover.");
          break;
        }
#endif
        case (mcrl2::data::rewriter::innermost_prover): {
          throw mcrl2::runtime_error("The innermost rewriter with prover is not supported by the prover.");
          break;
        }
#ifdef MCRL2_INNERC_AVAILABLE
        case (mcrl2::data::rewriter::innermost_compiling_prover): {
          mcrl2::runtime_error("The compiled innermost rewriter with prover is not supported by the prover.");
          break;
        }
#endif
#ifdef MCRL2_JITTYC_AVAILABLE
        case (mcrl2::data::rewriter::jitty_compiling): {
          throw mcrl2::runtime_error("The compiled jitty rewriter is not supported by the prover.");
          break;
        }
#endif
        case (mcrl2::data::rewriter::jitty_prover): {
          throw mcrl2::runtime_error("The jitty rewriter with prover is not supported by the prover.");
          break;
        }
#ifdef MCRL2_JITTYC_AVAILABLE
        case (mcrl2::data::rewriter::jitty_compiling_prover): {
          throw mcrl2::runtime_error("The compiled jitty rewriter with prover is not supported by the prover.");
          break;
        }
#endif
        default: {
          mcrl2::runtime_error("Unknown type of rewriter.");
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
    }
  }
}
