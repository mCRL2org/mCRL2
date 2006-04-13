// Implementation of class Disjointness_Checker
// file: disjointnesschecker.cpp

#include "disjointnesschecker.h"
#include "liblowlevel.h"
#include "libstruct.h"
#include "libprint_c.h"

// Auxiliary functions ----------------------------------------------------------------------------

  bool disjoint_sets(bool* a_set_1, bool* a_set_2, int a_number_of_elements) {
    for (int i = 0; i < a_number_of_elements; i++) {
      if (a_set_1[i] && a_set_2[i]) {
        return false;
      }
    }
    return true;
  }

// Class Disjointness_Checker ---------------------------------------------------------------------
  // Class Disjointness_Checker - Functions declared private --------------------------------------

    void Disjointness_Checker::process_data_expression(int a_summand_number, ATermAppl a_expression) {
      ATermAppl v_expression_1, v_expression_2;
      int v_variable_index;

      if (gsIsDataVarId(a_expression)) {
        v_variable_index = ATindexedSetGetIndex(f_parameter_set, (ATerm) a_expression);
        if (v_variable_index >= 0) {
          f_used_parameters_per_summand[(a_summand_number * f_number_of_parameters) + v_variable_index] = true;
        }
      } else if (!gsIsOpId(a_expression)) {
        v_expression_1 = ATAgetArgument(a_expression, 0);
        v_expression_2 = ATAgetArgument(a_expression, 1);
        process_data_expression(a_summand_number, v_expression_1);
        process_data_expression(a_summand_number, v_expression_2);
      }
    }

    // --------------------------------------------------------------------------------------------

    void Disjointness_Checker::process_multi_action(int a_summand_number, ATermAppl v_multi_action) {
      ATermList v_actions = ATLgetArgument(v_multi_action, 0);
      ATermAppl v_action;
      ATermList v_expressions;
      ATermAppl v_expression;

      while (!ATisEmpty(v_actions)) {
        v_action = ATAgetFirst(v_actions);
        v_expressions = ATLgetArgument(v_action, 1);
        while (!ATisEmpty(v_expressions)) {
          v_expression = ATAgetFirst(v_expressions);
          process_data_expression(a_summand_number, v_expression);
          v_expressions = ATgetNext(v_expressions);
        }
        v_actions = ATgetNext(v_actions);
      }
    }

    // --------------------------------------------------------------------------------------------

    void Disjointness_Checker::process_summand(int a_summand_number, ATermAppl a_summand) {
      ATermAppl v_condition = ATAgetArgument(a_summand, 1);
      ATermAppl v_multi_action_or_delta = ATAgetArgument(a_summand, 2);
      ATermAppl v_time = ATAgetArgument(a_summand, 3);
      ATermList v_assignments = ATLgetArgument(a_summand, 4);
      ATermAppl v_assignment;
      ATermAppl v_variable;
      ATermAppl v_expression;
      int v_variable_index;

      // variables used in condition
      process_data_expression(a_summand_number, v_condition);

      // variables used in multiaction
      if (!gsIsDelta(v_multi_action_or_delta)) {
        process_multi_action(a_summand_number, v_multi_action_or_delta);
      }

      // variables used in time
      if (!gsIsNil(v_time)) {
        process_data_expression(a_summand_number, v_time);
      }

      // variables used and changed in assignments
      while (!ATisEmpty(v_assignments)) {
        v_assignment = ATAgetFirst(v_assignments);
        v_variable = ATAgetArgument(v_assignment, 0);
        v_expression = ATAgetArgument(v_assignment, 1);

        // variable changed in assignment
        v_variable_index = ATindexedSetGetIndex(f_parameter_set, (ATerm) v_variable);
        if (v_variable_index >= 0) {
          f_changed_parameters_per_summand[(a_summand_number * f_number_of_parameters) + v_variable_index] = true;
        }

        // variables used in assignment
        process_data_expression(a_summand_number, v_expression);

        v_assignments = ATgetNext(v_assignments);
      }
    }

    // --------------------------------------------------------------------------------------------

    void Disjointness_Checker::process_parameters(ATermList a_parameters) {
      ATermAppl v_variable;

      while (!ATisEmpty(a_parameters)) {
        v_variable = ATAgetFirst(a_parameters);
        ATindexedSetPut(f_parameter_set, (ATerm) v_variable, 0);
        a_parameters = ATgetNext(a_parameters);
      }
    }

// Class Disjointness_Checker ---------------------------------------------------------------------
  // Class Disjointness_Checker - Functions declared public ---------------------------------------

    Disjointness_Checker::Disjointness_Checker(ATermAppl a_process_equation) {
      ATermList v_parameters = ATLgetArgument(a_process_equation, 1);
      ATermList v_summands = ATLgetArgument(a_process_equation, 2);
      ATermAppl v_summand;
      int v_summand_number = 1;

      f_parameter_set = ATindexedSetCreate(ATgetLength(v_parameters) * 2, 50);
      f_number_of_summands = ATgetLength(v_summands);
      f_number_of_parameters = ATgetLength(v_parameters);
      f_used_parameters_per_summand = (bool*) calloc((f_number_of_summands + 1) * f_number_of_parameters, sizeof(bool));
      f_changed_parameters_per_summand = (bool*) calloc((f_number_of_summands + 1) * f_number_of_parameters, sizeof(bool));

      process_parameters(v_parameters);
      while (!ATisEmpty(v_summands)) {
        v_summand = ATAgetFirst(v_summands);
        process_summand(v_summand_number, v_summand);
        v_summand_number++;
        v_summands = ATgetNext(v_summands);
      }
    }

    // --------------------------------------------------------------------------------------------

    Disjointness_Checker::~Disjointness_Checker() {
      ATindexedSetDestroy(f_parameter_set);
      free(f_used_parameters_per_summand);
      free(f_changed_parameters_per_summand);
    }

    // --------------------------------------------------------------------------------------------

    bool Disjointness_Checker::disjoint(int a_summand_number_1, int a_summand_number_2) {
      bool v_used_1_changed_2;
      bool v_used_2_changed_1;
      bool v_changed_1_changed_2;

      if ((a_summand_number_1 <= f_number_of_summands) && (a_summand_number_2 <= f_number_of_summands)) {
        v_used_1_changed_2 = disjoint_sets(
                               &f_used_parameters_per_summand[a_summand_number_1 * f_number_of_parameters],
                               &f_changed_parameters_per_summand[a_summand_number_2 * f_number_of_parameters],
                               f_number_of_parameters
                             );
        v_used_2_changed_1 = disjoint_sets(
                               &f_used_parameters_per_summand[a_summand_number_2 * f_number_of_parameters],
                               &f_changed_parameters_per_summand[a_summand_number_1 * f_number_of_parameters],
                               f_number_of_parameters
                             );
        v_changed_1_changed_2 = disjoint_sets(
                                  &f_changed_parameters_per_summand[a_summand_number_1 * f_number_of_parameters],
                                  &f_changed_parameters_per_summand[a_summand_number_2 * f_number_of_parameters],
                                  f_number_of_parameters
                                );
        return v_used_1_changed_2 && v_used_2_changed_1 && v_changed_1_changed_2;
      } else {
        gsErrorMsg("There is no summand with this number.\n");
        exit(1);
      }
    }
