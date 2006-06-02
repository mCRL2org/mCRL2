// Implementation of class BDD_Manipulator
// file: bdd_manipulator.cpp

#include "aterm2.h"
#include "libstruct.h"
#include "bdd_manipulator.h"

// Class BDD_Manipulator --------------------------------------------------------------------------
  // Class BDD_Manipulator - Functions declared public --------------------------------------------

    ATermAppl BDD_Manipulator::make_reduced_if_then_else(ATermAppl a_guard, ATermAppl a_true_branch, ATermAppl a_false_branch) {
      if (a_true_branch == a_false_branch) {
        return a_true_branch;
      } else {
        return make_if_then_else(a_guard, a_true_branch, a_false_branch);
      }
    }

  // Class BDD_Manipulator - Functions declared private -------------------------------------------

    ATermAppl BDD_Manipulator::make_if_then_else(ATermAppl a_guard, ATermAppl a_true_branch, ATermAppl a_false_branch) {
      return gsMakeDataExprIf(a_guard, a_true_branch, a_false_branch);
    }
