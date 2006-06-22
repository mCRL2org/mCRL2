// Interface to class BDD_Manipulator
// file: bdd_manipulator.h

#ifndef BDD_MANIPULATOR_H
#define BDD_MANIPULATOR_H

#include "aterm2.h"

class BDD_Manipulator {
  private:
    ATermAppl make_if_then_else(ATermAppl a_guard, ATermAppl a_true_branch, ATermAppl a_false_branch);
  public:
    ATermAppl make_reduced_if_then_else(ATermAppl a_guard, ATermAppl a_true_branch, ATermAppl a_false_branch);
};

#endif
