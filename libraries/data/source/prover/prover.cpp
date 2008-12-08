// Author(s): Luc Engelen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "boost.hpp" // precompiled headers

#include <cstdlib>

#include "mcrl2/data/data_specification.h"
#include "mcrl2/data/prover.h"
#include "mcrl2/utilities/manipulator.h"
#include "mcrl2/utilities/info.h"
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/data/rewrite.h"
#include "mcrl2/utilities/utilities.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/exception.h"

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
#ifdef MCRL2_INNERC_AVAILABLE
        case (GS_REWR_INNERC): {
          throw mcrl2::runtime_error("The compiled innermost rewriter is not supported by the prover.");
          break;
        }
#endif
        case (GS_REWR_INNER_P): {
          throw mcrl2::runtime_error("The innermost rewriter with prover is not supported by the prover.");
          break;
        }
#ifdef MCRL2_INNERC_AVAILABLE
        case (GS_REWR_INNERC_P): {
          mcrl2::runtime_error("The compiled innermost rewriter with prover is not supported by the prover.");
          break;
        }
#endif
#ifdef MCRL2_JITTYC_AVAILABLE
        case (GS_REWR_JITTYC): {
          throw mcrl2::runtime_error("The compiled jitty rewriter is not supported by the prover.");
          break;
        }
#endif
        case (GS_REWR_JITTY_P): {
          throw mcrl2::runtime_error("The jitty rewriter with prover is not supported by the prover.");
          break;
        }
#ifdef MCRL2_JITTYC_AVAILABLE
        case (GS_REWR_JITTYC_P): {
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
