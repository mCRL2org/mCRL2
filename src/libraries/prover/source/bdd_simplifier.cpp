// Implementation of class BDD_Simplifier
// file: bdd_simplifier.cpp

#include "bdd_simplifier.h"
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
