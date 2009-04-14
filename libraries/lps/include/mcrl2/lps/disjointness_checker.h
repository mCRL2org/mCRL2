// Author(s): Luc Engelen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file include/disjointness_checker.h
/// \brief Add your file description here.

// Interface to class Disjointness_Checker
// file: disjointness_checker.h

#ifndef DISJOINTNESS_CHECKER_H
#define DISJOINTNESS_CHECKER_H

#include "aterm2.h"
#include "stdlib.h"

   /// \brief Class that can determine if two summands are syntactically disjoint.
   /// Two summands are syntactically disjoint if the following conditions hold:
   /// - The set of variables used by one summand is disjoint from the set of variables changed by the other summand and
   ///   vice versa.
   /// - The set of variables changed by one summand is disjoint from the set of variables changed by the other summand.
   ///
   /// An instance of the class Disjointness_Checker is created using the constructor
   /// Disjointness_Checker::Disjointness_Checker. The parameter a_process_equations is used to pass the summands to be
   /// checked for disjointness. The function Disjointness_Checker::disjoint indicates whether the two summands with numbers
   /// a_summand_number_1 and a_summand_number_2 are syntactically disjoint.

class Disjointness_Checker {
  private:
    /// \brief The number of summands of the LPS passed as argument of the constructor.
    int f_number_of_summands;

    /// \brief The number of parameters of the LPS passed as argument of the constructor.
    int f_number_of_parameters;

    /// \brief An indexed set of parameters.
    ATermIndexedSet f_parameter_set;

    /// \brief A two dimensional array, indicating which parameters a summand uses, for each of the summands.
    bool* f_used_parameters_per_summand;

    /// \brief A two dimensional array, indicating which parameters a summand changes, for each of the summands.
    bool* f_changed_parameters_per_summand;

    /// \brief Updates the array Disjointness_Checker::f_used_parameters_per_summand, given the expression a_expression.
    void process_data_expression(int a_summand_number, ATermAppl a_expression);

    /// \brief Updates the array Disjointness_Checker::f_used_parameters_per_summand, given the multiaction a_multi_action.
    void process_multi_action(int a_summand_number, ATermAppl a_multi_action);

    /// \brief Updates the arrays Disjointness_Checker::f_changed_parameters_per_summand and
    /// \brief Disjointness_Checker::f_used_parameters_per_summand, given the summand a_summand.
    void process_summand(int a_summand_number, ATermAppl a_summand);

    /// \brief Initializes the indexed set Disjointness_Checker::f_parameter_set, given the list of parameters a_parameters.
    void process_parameters(ATermList a_parameters);
  public:
    /// \brief Constructor that initializes the sets Disjointness_Checker::f_used_parameters_per_summand and
    /// \brief Disjointness_Checker::f_changed_parameters_per_summand, and the indexed set
    /// \brief Disjointness_Checker::f_parameter_set.
    /// precondition: the argument passed as parameter a_process_equations is a specification of process equations in mCRL2
    /// format
    /// precondition: the arguments passed as parameters a_summand_number_1 and a_summand_number_2 correspond to summands in
    /// the proces equations passed as parameter a_process_equations. They lie in the interval from and including 1 upto and
    /// including the highest summand number
    Disjointness_Checker(ATermAppl a_process_equation);

    /// \brief Destructor that frees the memory used by Disjointness_Checker::f_used_parameters_per_summand,
    /// \brief Disjointness_Checker::f_changed_parameters_per_summand and Disjointness_Checker::f_parameter_set.
    ~Disjointness_Checker();

    /// \brief Indicates whether or not the summands with number a_summand_number_1 and a_summand_number_2 are disjoint.
    bool disjoint(int a_summand_number_1, int a_summand_number_2);
};

#endif
