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

    /// \param a_bdd A binary decision diagram.
    /// \return The guard at the root of the BDD.

    ATermAppl BDD_Info::get_guard(ATermAppl a_bdd) {
      return ATAgetArgument(ATAgetArgument(ATAgetArgument(a_bdd, 0), 0), 1);
    }

    // --------------------------------------------------------------------------------------------

    /// \param a_bdd A binary decision diagram.
    /// \return The true-branch of the BDD.

    ATermAppl BDD_Info::get_true_branch(ATermAppl a_bdd) {
      return ATAgetArgument(ATAgetArgument(a_bdd, 0), 1);
    }

    // --------------------------------------------------------------------------------------------

    /// \param a_bdd A binary decision diagram.
    /// \return The false-branch of the BDD.

    ATermAppl BDD_Info::get_false_branch(ATermAppl a_bdd) {
      return ATAgetArgument(a_bdd, 1);
    }

    // --------------------------------------------------------------------------------------------

    /// \param a_bdd A binary decision diagram.
    /// \return True, if the BDD equals true.
    ///         False, if the BDD does not equal true.

    bool BDD_Info::is_true(ATermAppl a_bdd) {
      return a_bdd == gsMakeOpIdTrue();
    }

    // --------------------------------------------------------------------------------------------

    /// \param a_bdd A binary decision diagram.
    /// \return True, if the BDD equals false.
    ///         False, if the BDD does not equal true.

    bool BDD_Info::is_false(ATermAppl a_bdd) {
      return a_bdd == gsMakeOpIdFalse();
    }

    // --------------------------------------------------------------------------------------------

    /// \param a_bdd A binary decision diagram.
    /// \return True, if the root of the BDD is a guard node.
    ///         False, if the BDD equals true or if the BDD equals false.

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
