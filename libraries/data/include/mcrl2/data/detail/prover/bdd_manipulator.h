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

#ifndef MCRL2_DATA_DETAIL_PROVER_BDD_MANIPULATOR_H
#define MCRL2_DATA_DETAIL_PROVER_BDD_MANIPULATOR_H

#include "mcrl2/data/standard.h"

namespace mcrl2
{
namespace data
{
namespace detail
{

/// The class BDD_Manipulator provides functionality for constructing binary decision diagrams.
class BDD_Manipulator
{

  private:

    /// \brief Returns a BDD
    /// The method BDD_Manipulator::make_reduced_if_then_else returns a BDD with guard \c a_guard,
    /// true-branch \c a_true_branch and false-branch \c a_false_branch.
    /// \param[in] a_guard A propositional formula.
    /// \param[in] a_true_branch A BDD.
    /// \param[in] a_false_branch A BDD.
    /// \return A BDD.
    inline data_expression make_if_then_else(
                       const data_expression &a_guard, 
                       const data_expression &a_true_branch, 
                       const data_expression &a_false_branch)
    {
      return mcrl2::data::if_(a_guard, a_true_branch, a_false_branch);
    } 

  public:

    /// \brief Returns a reduced BDD
    /// The method BDD_Manipulator::make_reduced_if_then_else returns a BDD with guard \c a_guard,
    /// true-branch \c a_true_branch and false-branch \c a_false_branch. If the true-branch is equal to the
    /// false-branch, the true-branch is returned.
    /// \param[in] a_guard A proposition formula.
    /// \param[in] a_true_branch A BDD.
    /// \param[in] a_false_branch A BDD.
    /// \return A reduced BDD.
    inline data_expression make_reduced_if_then_else(
                       const data_expression &a_guard, 
                       const data_expression &a_true_branch, 
                       const data_expression &a_false_branch)
    {
      return (a_true_branch == a_false_branch) ? 
                    a_true_branch : 
                    make_if_then_else(a_guard, a_true_branch, a_false_branch);
    }
};

} // namespace detail
} // namespace data
} // namespace mcrl2

#endif
