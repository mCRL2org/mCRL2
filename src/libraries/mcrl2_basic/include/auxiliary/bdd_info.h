// Interface to class BDD_Info
// file: bdd_info.h

#ifndef BDD_INFO_H
#define BDD_INFO_H

#include "aterm2.h"

  /// The class BDD_Info provides information about the structure of binary decision diagrams.

class BDD_Info {
  protected:
    /// \brief ATermAppl representing the \c if-then-else function with type Bool -> Bool -> Bool -> Bool.
    ATermAppl f_if_then_else;
  public:
    /// \brief Constructor that initializes the field BDD_Info::f_if_then_else.
    BDD_Info();

    /// \brief Method that returns the guard of a BDD.
    ATermAppl get_guard(ATermAppl a_bdd);

    /// \brief Method that returns the true-branch of a BDD.
    ATermAppl get_true_branch(ATermAppl a_bdd);

    /// \brief Method that returns the false-branch of a BDD.
    ATermAppl get_false_branch(ATermAppl a_bdd);

    /// \brief Method that indicates whether or not a BDD equals true.
    bool is_true(ATermAppl a_bdd);

    /// \brief Method that indicates whether or not a BDD equals false.
    bool is_false(ATermAppl a_bdd);

    /// \brief Method that indicates wether or not the root of a BDD is a guard node.
    bool is_if_then_else(ATermAppl a_bdd);
};

#endif
