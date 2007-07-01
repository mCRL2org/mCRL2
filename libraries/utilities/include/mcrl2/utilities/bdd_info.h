// Author(s): Luc Engelen
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file include/mcrl2/utilities/bdd_info.h
/// \brief Add your file description here.

// Interface to class BDD_Info
// file: bdd_info.h

#ifndef BDD_INFO_H
#define BDD_INFO_H

#include "aterm2.h"

  /// The class BDD_Info provides information about the structure of binary decision diagrams.

class BDD_Info {
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
