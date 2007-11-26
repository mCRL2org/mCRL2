// Author(s): Luc Engelen
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file source/bdd_manipulator.cpp
/// \brief Add your file description here.

// Implementation of class BDD_Manipulator
// file: bdd_manipulator.cpp

#include "aterm2.h"
#include "mcrl2/core/struct.h"
#include "mcrl2/utilities/bdd_manipulator.h"

// Class BDD_Manipulator --------------------------------------------------------------------------
  // Class BDD_Manipulator - Functions declared public --------------------------------------------

    /// The method BDD_Manipulator::make_reduced_if_then_else returns a BDD with guard \c a_guard,
    /// true-branch \c a_true_branch and false-branch \c a_false_branch. If the true-branch is equal to the
    /// false-branch, the true-branch is returned.
    /// \param a_guard A proposition formula.
    /// \param a_true_branch A BDD.
    /// \param a_false_branch A BDD.
    /// \return A reduced BDD.

    ATermAppl BDD_Manipulator::make_reduced_if_then_else(ATermAppl a_guard, ATermAppl a_true_branch, ATermAppl a_false_branch) {
      if (a_true_branch == a_false_branch) {
        return a_true_branch;
      } else {
        return make_if_then_else(a_guard, a_true_branch, a_false_branch);
      }
    }

  // Class BDD_Manipulator - Functions declared private -------------------------------------------

    /// The method BDD_Manipulator::make_reduced_if_then_else returns a BDD with guard \c a_guard,
    /// true-branch \c a_true_branch and false-branch \c a_false_branch.
    /// \param a_guard A propositional formula.
    /// \param a_true_branch A BDD.
    /// \param a_false_branch A BDD.
    /// \return A BDD.

    ATermAppl BDD_Manipulator::make_if_then_else(ATermAppl a_guard, ATermAppl a_true_branch, ATermAppl a_false_branch) {
      return gsMakeDataExprIf(a_guard, a_true_branch, a_false_branch);
    }
