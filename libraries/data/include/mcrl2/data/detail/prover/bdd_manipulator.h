// Author(s): Luc Engelen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/utilities/bdd_manipulator.h
/// \brief Interface to class BDD_Manipulator

#ifndef BDD_MANIPULATOR_H
#define BDD_MANIPULATOR_H

#include "aterm2.h"
#include "mcrl2/core/detail/struct.h"

/// The class BDD_Manipulator provides functionality for constructing binary decision diagrams.
class BDD_Manipulator {

  private:

    /// \brief Returns a BDD
    /// The method BDD_Manipulator::make_reduced_if_then_else returns a BDD with guard \c a_guard,
    /// true-branch \c a_true_branch and false-branch \c a_false_branch.
    /// \param a_guard A propositional formula.
    /// \param a_true_branch A BDD.
    /// \param a_false_branch A BDD.
    /// \return A BDD.
    inline ATermAppl make_if_then_else(ATermAppl a_guard, ATermAppl a_true_branch, ATermAppl a_false_branch) {
      return (a_true_branch == a_false_branch) ? a_true_branch : make_if_then_else(a_guard, a_true_branch, a_false_branch);
    }

  public:

    /// \brief Returns a reduced BDD
    /// The method BDD_Manipulator::make_reduced_if_then_else returns a BDD with guard \c a_guard,
    /// true-branch \c a_true_branch and false-branch \c a_false_branch. If the true-branch is equal to the
    /// false-branch, the true-branch is returned.
    /// \param a_guard A proposition formula.
    /// \param a_true_branch A BDD.
    /// \param a_false_branch A BDD.
    /// \return A reduced BDD.
    inline ATermAppl make_reduced_if_then_else(ATermAppl a_guard, ATermAppl a_true_branch, ATermAppl a_false_branch) {
      return mcrl2::core::detail::gsMakeDataExprIf(a_guard, a_true_branch, a_false_branch);
    }
};

#endif
