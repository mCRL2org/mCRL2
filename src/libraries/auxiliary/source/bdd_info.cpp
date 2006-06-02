// Implementation of class BDD_Info
// file: bdd_info.cpp

#include "bdd_info.h"
#include "libstruct.h"
#include "liblowlevel.h"
#include "libprint_c.h"

// Class BDD_Info ---------------------------------------------------------------------------------
  // Class BDD_Info - Functions declared public ---------------------------------------------------

    BDD_Info::BDD_Info() {
      f_if_then_else = gsMakeOpIdIf(gsMakeSortExprBool());
    }

    // --------------------------------------------------------------------------------------------

    ATermAppl BDD_Info::get_guard(ATermAppl a_bdd) {
      return ATAgetArgument(ATAgetArgument(ATAgetArgument(a_bdd, 0), 0), 1);
    }

    // --------------------------------------------------------------------------------------------

    ATermAppl BDD_Info::get_true_branch(ATermAppl a_bdd) {
      return ATAgetArgument(ATAgetArgument(a_bdd, 0), 1);
    }

    // --------------------------------------------------------------------------------------------

    ATermAppl BDD_Info::get_false_branch(ATermAppl a_bdd) {
      return ATAgetArgument(a_bdd, 1);
    }

    // --------------------------------------------------------------------------------------------

    bool BDD_Info::is_true(ATermAppl a_bdd) {
      return a_bdd == gsMakeOpIdTrue();
    }

    // --------------------------------------------------------------------------------------------

    bool BDD_Info::is_false(ATermAppl a_bdd) {
      return a_bdd == gsMakeOpIdFalse();
    }

    // --------------------------------------------------------------------------------------------

    bool BDD_Info::is_if_then_else(ATermAppl a_bdd) {
      ATermAppl v_term;

      if (gsIsDataAppl(a_bdd)) {
        v_term = ATAgetArgument(a_bdd, 0);
        if (gsIsDataAppl(v_term)) {
          v_term = ATAgetArgument(v_term, 0);
          if (gsIsDataAppl(v_term)) {
            v_term = ATAgetArgument(v_term, 0);
            return v_term == f_if_then_else;
          }
        }
      }
      return false;
    }
