// Interface to class Disjointness_Checker
// file: disjointness_checker.h

#ifndef DISJOINTNESS_CHECKER_H
#define DISJOINTNESS_CHECKER_H

#include "aterm2.h"
#include "stdlib.h"

   /// \brief Class that can determine if two summands are syntactically disjoint.

class Disjointness_Checker {
  private:
    /// \brief The number of summands of the LPE passed as argument of the constructor.
    int f_number_of_summands;

    /// \brief The number of parameters of the LPE passed as argument of the constructor.
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
    Disjointness_Checker(ATermAppl a_process_equation);

    /// \brief Destructor that frees the memory used by Disjointness_Checker::f_used_parameters_per_summand,
    /// \brief Disjointness_Checker::f_changed_parameters_per_summand and Disjointness_Checker::f_parameter_set.
    ~Disjointness_Checker();

    /// \brief Indicates whether or not the summands with number a_summand_number_1 and a_summand_number_2 are disjoint.
    bool disjoint(int a_summand_number_1, int a_summand_number_2);
};

#endif
