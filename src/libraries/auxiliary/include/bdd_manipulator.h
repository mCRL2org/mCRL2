// Interface to class BDD_Manipulator
// file: bdd_manipulator.h

#ifndef BDD_MANIPULATOR_H
#define BDD_MANIPULATOR_H

#include "aterm2.h"

  /// The class BDD_Manipulator provides functionality for constructing binary decision diagrams.

class BDD_Manipulator {
  private:
    /// \brief Returns a BDD
    ATermAppl make_if_then_else(ATermAppl a_guard, ATermAppl a_true_branch, ATermAppl a_false_branch);
  public:
    /// \brief Returns a reduced BDD
    ATermAppl make_reduced_if_then_else(ATermAppl a_guard, ATermAppl a_true_branch, ATermAppl a_false_branch);
};

#endif
