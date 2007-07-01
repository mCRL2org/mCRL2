// Author(s): Luc Engelen
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file source/bdd_info.cpp
/// \brief Add your file description here.

// Implementation of class BDD_Info
// file: bdd_info.cpp

#include "libstruct.h"
#include "mcrl2/utilities/aterm_ext.h"
#include "mcrl2/utilities/bdd_info.h"

#ifdef __cplusplus
using namespace ::mcrl2::utilities;
#endif

// Class BDD_Info ---------------------------------------------------------------------------------
  // Class BDD_Info - Functions declared public ---------------------------------------------------

    BDD_Info::BDD_Info() {
    }

    // --------------------------------------------------------------------------------------------

    /// \param a_bdd A binary decision diagram.
    /// \return The guard at the root of the BDD.

    ATermAppl BDD_Info::get_guard(ATermAppl a_bdd) {
      return ATAgetFirst(ATLgetArgument(a_bdd, 1));
    }

    // --------------------------------------------------------------------------------------------

    /// \param a_bdd A binary decision diagram.
    /// \return The true-branch of the BDD.

    ATermAppl BDD_Info::get_true_branch(ATermAppl a_bdd) {
      return ATAgetFirst(ATgetNext(ATLgetArgument(a_bdd, 1)));
    }

    // --------------------------------------------------------------------------------------------

    /// \param a_bdd A binary decision diagram.
    /// \return The false-branch of the BDD.

    ATermAppl BDD_Info::get_false_branch(ATermAppl a_bdd) {
      return ATAgetFirst(ATgetNext(ATgetNext(ATLgetArgument(a_bdd, 1))));
    }

    // --------------------------------------------------------------------------------------------

    /// \param a_bdd A binary decision diagram.
    /// \return True, if the BDD equals true.
    ///         False, if the BDD does not equal true.

    bool BDD_Info::is_true(ATermAppl a_bdd) {
      return gsIsDataExprTrue(a_bdd);
    }

    // --------------------------------------------------------------------------------------------

    /// \param a_bdd A binary decision diagram.
    /// \return True, if the BDD equals false.
    ///         False, if the BDD does not equal true.

    bool BDD_Info::is_false(ATermAppl a_bdd) {
      return gsIsDataExprFalse(a_bdd);;
    }

    // --------------------------------------------------------------------------------------------

    /// \param a_bdd A binary decision diagram.
    /// \return True, if the root of the BDD is a guard node.
    ///         False, if the BDD equals true or if the BDD equals false.

    bool BDD_Info::is_if_then_else(ATermAppl a_bdd) {
      return gsIsDataExprIf(a_bdd); 
    }
