// Author(s): Luc Engelen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file bdd_simplifier.cpp

#include "mcrl2/data/prover/bdd_simplifier.h"
#include "aterm2.h"
#include "time.h"

// class BDD_Simplifier ---------------------------------------------------------------------------
  // class BDD_Simplifier - functions declared public ---------------------------------------------

    BDD_Simplifier::~BDD_Simplifier() {
      // Nothing to free
    }

    // --------------------------------------------------------------------------------------------

    void BDD_Simplifier::set_time_limit(int a_time_limit) {
      if (a_time_limit == 0) {
        f_deadline = 0;
      } else {
        f_deadline = time(0) + a_time_limit;
      }
    }

    // --------------------------------------------------------------------------------------------

    ATermAppl BDD_Simplifier::simplify(ATermAppl a_bdd) {
      return a_bdd;
    }
