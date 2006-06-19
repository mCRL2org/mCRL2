// Implementation of class SMT_LIB_Solver
// file: smt_lib_solver.cpp

#include "smt_lib_solver.h"
#include "libprint_c.h"
#include "libstruct.h"
#include "utilities.h"

using namespace std;

// Class SMT_LIB_Solver ---------------------------------------------------------------------------
  // Class SMT_LIB_Solver - Functions declared private --------------------------------------------

    void SMT_LIB_Solver::declare_variables() {
      ATermList v_variables;
      ATermAppl v_variable;
      ATermAppl v_sort;
      int v_sort_number;
      char* v_sort_string;
      char* v_variable_string;

      f_variables_extrafuns = "";
      v_variables = ATindexedSetElements(f_variables);
      if (!ATisEmpty(v_variables)) {
        f_variables_extrafuns = "  :extrafuns (";
        while (!ATisEmpty(v_variables)) {
          v_variable = ATAgetFirst(v_variables);
          v_variables = ATgetNext(v_variables);
          v_variable_string = gsATermAppl2String(ATAgetArgument(v_variable, 0));
          if (f_sort_info.is_sort_real(f_expression_info.get_sort_of_variable(v_variable))) {
            f_variables_extrafuns = f_variables_extrafuns + "(" + v_variable_string + " Real)";
          } else if (f_sort_info.is_sort_int(f_expression_info.get_sort_of_variable(v_variable))) {
            f_variables_extrafuns = f_variables_extrafuns + "(" + v_variable_string + " Int)";
          } else if (f_sort_info.is_sort_nat(f_expression_info.get_sort_of_variable(v_variable))) {
            f_variables_extrafuns = f_variables_extrafuns + "(" + v_variable_string + " Int)";
          } else if (f_sort_info.is_sort_pos(f_expression_info.get_sort_of_variable(v_variable))) {
            f_variables_extrafuns = f_variables_extrafuns + "(" + v_variable_string + " Int)";
          } else {
            v_sort = f_expression_info.get_sort_of_variable(v_variable);
            v_sort_number = ATindexedSetPut(f_sorts, (ATerm) v_sort, 0);
            v_sort_string = (char*) malloc((number_of_digits(v_sort_number) + 5) * sizeof(char));
            sprintf(v_sort_string, "sort%d", v_sort_number);
            f_variables_extrafuns = f_variables_extrafuns + "(" + v_variable_string + " " + v_sort_string +")";
            free(v_sort_string);
            v_sort_string = 0;
          }
        }
        f_variables_extrafuns = f_variables_extrafuns + ")\n";
      }
    }

    // --------------------------------------------------------------------------------------------

    void SMT_LIB_Solver::declare_operators() {
      ATermList v_operators;
      ATermAppl v_operator;
      ATermAppl v_sort;
      ATermAppl v_sort_domain;
      int v_sort_number;
      char* v_sort_string;
      int v_operator_number;
      char* v_operator_string;

      f_operators_extrafuns = "";
      v_operators = ATindexedSetElements(f_operators);
      if (!ATisEmpty(v_operators)) {
        f_operators_extrafuns = "  :extrafuns (";
        while (!ATisEmpty(v_operators)) {
          v_operator = ATAgetFirst(v_operators);
          v_operators = ATgetNext(v_operators);
          v_operator_number = ATindexedSetGetIndex(f_operators, (ATerm) v_operator);
          v_operator_string = (char*) malloc((number_of_digits(v_operator_number) + 3) * sizeof(char));
          sprintf(v_operator_string, "op%d", v_operator_number);
          f_operators_extrafuns = f_operators_extrafuns + "(" + v_operator_string;
          free(v_operator_string);
          v_operator_string = 0;
          v_sort = f_expression_info.get_sort_of_operator(v_operator);
          do {
            if (f_sort_info.is_sort_arrow(v_sort)) {
              v_sort_domain = f_sort_info.get_domain(v_sort);
              v_sort = f_sort_info.get_range(v_sort);
            } else {
              v_sort_domain = v_sort;
              v_sort = 0;
            }
            if (f_sort_info.is_sort_arrow(v_sort_domain)) {
              gsErrorMsg("Function %P cannot be translated to the SMT-LIB format.\n", v_operator);
              exit(1);
            }
            if (f_sort_info.is_sort_int(v_sort_domain)) {
              f_operators_extrafuns = f_operators_extrafuns + " Int";
            } else if (f_sort_info.is_sort_nat(v_sort_domain)) {
              f_operators_extrafuns = f_operators_extrafuns + " Int";
            } else if (f_sort_info.is_sort_pos(v_sort_domain)) {
              f_operators_extrafuns = f_operators_extrafuns + " Int";
            } else if (f_sort_info.is_sort_real(v_sort_domain)) {
              f_operators_extrafuns = f_operators_extrafuns + " Real";
            } else {
              v_sort_number = ATindexedSetPut(f_sorts, (ATerm) v_sort_domain, 0);
              v_sort_string = (char*) malloc((number_of_digits(v_sort_number) + 5) * sizeof(char));
              sprintf(v_sort_string, "sort%d", v_sort_number);
              f_operators_extrafuns = f_operators_extrafuns + " " + v_sort_string;
              free(v_sort_string);
              v_sort_string = 0;
            }
          } while (v_sort != 0);
          f_operators_extrafuns = f_operators_extrafuns + ")";
        }
        f_operators_extrafuns = f_operators_extrafuns + ")\n";
      }
    }

    // --------------------------------------------------------------------------------------------

    void SMT_LIB_Solver::declare_predicates() {
      f_extrapreds = "";
      if (f_bool2pred) {
        int v_sort_number;
        char* v_sort_string;

        v_sort_number = ATindexedSetGetIndex(f_sorts, (ATerm) gsMakeSortIdBool());
        v_sort_string = (char*) malloc((number_of_digits(v_sort_number) + 5) * sizeof(char));
        sprintf(v_sort_string, "sort%d", v_sort_number);
        f_extrapreds = "  :extrapreds ((bool2pred ";
        f_extrapreds = f_extrapreds + v_sort_string + ")";
        free(v_sort_string);
        v_sort_string = 0;
        f_extrapreds = f_extrapreds + ")\n";
      }
    }

    // --------------------------------------------------------------------------------------------

    void SMT_LIB_Solver::declare_sorts() {
      ATermList v_sorts;
      ATermAppl v_sort = 0;
      int v_sort_number;
      char* v_sort_string;

      f_extrasorts = "";
      v_sorts = ATindexedSetElements(f_sorts);
      if (!ATisEmpty(v_sorts)) {
        f_extrasorts = "  :extrasorts (";
        while (!ATisEmpty(v_sorts)) {
          if (v_sort != 0) {
            f_extrasorts = f_extrasorts + " ";
          }
          v_sort = ATAgetFirst(v_sorts);
          v_sorts = ATgetNext(v_sorts);
          v_sort_number = ATindexedSetGetIndex(f_sorts, (ATerm) v_sort);
          v_sort_string = (char*) malloc((number_of_digits(v_sort_number) + 5) * sizeof(char));
          sprintf(v_sort_string, "sort%d", v_sort_number);
          f_extrasorts = f_extrasorts + v_sort_string;
          free(v_sort_string);
          v_sort_string = 0;
        }
        f_extrasorts = f_extrasorts + ")\n";
      }
    }

    // --------------------------------------------------------------------------------------------

    void SMT_LIB_Solver::produce_notes_for_sorts() {
      ATermList v_sorts;
      ATermAppl v_sort = 0;
      int v_sort_number;
      char* v_sort_original_id;
      char* v_sort_string;

      f_sorts_notes = "";
      v_sorts = ATindexedSetElements(f_sorts);
      if (!ATisEmpty(v_sorts)) {
        f_sorts_notes = "  :notes \"";
        while (!ATisEmpty(v_sorts)) {
          v_sort = ATAgetFirst(v_sorts);
          v_sorts = ATgetNext(v_sorts);
          v_sort_number = ATindexedSetGetIndex(f_sorts, (ATerm) v_sort);
          v_sort_string = (char*) malloc((number_of_digits(v_sort_number) + 5) * sizeof(char));
          sprintf(v_sort_string, "sort%d", v_sort_number);
          v_sort_original_id = gsATermAppl2String(ATAgetArgument(v_sort, 0));
          f_sorts_notes = f_sorts_notes + "(" + v_sort_string + " = " + v_sort_original_id + ")";
          free(v_sort_string);
          v_sort_string = 0;
        }
        f_sorts_notes = f_sorts_notes + "\"\n";
      }
    }

    // --------------------------------------------------------------------------------------------

    void SMT_LIB_Solver::produce_notes_for_operators() {
      ATermList v_operators;
      ATermAppl v_operator;
      int v_operator_number;
      char* v_operator_original_id;
      char* v_operator_string;

      f_operators_notes = "";
      v_operators = ATindexedSetElements(f_operators);
      if (!ATisEmpty(v_operators)) {
        f_operators_notes = "  :notes \"";
        while (!ATisEmpty(v_operators)) {
          v_operator = ATAgetFirst(v_operators);
          v_operators = ATgetNext(v_operators);
          v_operator_number = ATindexedSetGetIndex(f_operators, (ATerm) v_operator);
          v_operator_string = (char*) malloc((number_of_digits(v_operator_number) + 3) * sizeof(char));
          sprintf(v_operator_string, "op%d", v_operator_number);
          v_operator_original_id = gsATermAppl2String(ATAgetArgument(v_operator, 0));
          f_operators_notes = f_operators_notes + "(" + v_operator_string + " = " + v_operator_original_id + ")";
          free(v_operator_string);
          v_operator_string = 0;
        }
        f_operators_notes = f_operators_notes + "\"\n";
      }
    }

    // --------------------------------------------------------------------------------------------

    void SMT_LIB_Solver::produce_notes_for_predicates() {
      f_predicates_notes = "";
      if (f_bool2pred) {
        f_predicates_notes =
          "  :notes \"bool2pred was introduced, because the smt-lib format cannot deal\"\n"
          "  :notes \"with boolean variables or functions returning boolean values.\"\n";
      }
    }

    // --------------------------------------------------------------------------------------------

    void SMT_LIB_Solver::translate_not(ATermAppl a_clause) {
      ATermAppl v_clause;

      v_clause = f_expression_info.get_argument(a_clause, 0);
      f_formula = f_formula + "(not ";
      translate_clause(v_clause, true);
      f_formula = f_formula + ")";
    }

    // --------------------------------------------------------------------------------------------

    void SMT_LIB_Solver::translate_equality(ATermAppl a_clause) {
      ATermAppl v_clause_1, v_clause_2;

      v_clause_1 = f_expression_info.get_argument(a_clause, 0);
      v_clause_2 = f_expression_info.get_argument(a_clause, 1);
      f_formula = f_formula + "(= ";
      translate_clause(v_clause_1, false);
      f_formula = f_formula + " ";
      translate_clause(v_clause_2, false);
      f_formula = f_formula + ")";
    }

    // --------------------------------------------------------------------------------------------

    void SMT_LIB_Solver::translate_inequality(ATermAppl a_clause) {
      ATermAppl v_clause_1, v_clause_2;

      v_clause_1 = f_expression_info.get_argument(a_clause, 0);
      v_clause_2 = f_expression_info.get_argument(a_clause, 1);
      f_formula = f_formula + "(distinct ";
      translate_clause(v_clause_1, false);
      f_formula = f_formula + " ";
      translate_clause(v_clause_2, false);
      f_formula = f_formula + ")";
    }

    // --------------------------------------------------------------------------------------------

    void SMT_LIB_Solver::translate_greater_than(ATermAppl a_clause) {
      ATermAppl v_clause_1, v_clause_2;

      v_clause_1 = f_expression_info.get_argument(a_clause, 0);
      v_clause_2 = f_expression_info.get_argument(a_clause, 1);
      f_formula = f_formula + "(> ";
      translate_clause(v_clause_1, false);
      f_formula = f_formula + " ";
      translate_clause(v_clause_2, false);
      f_formula = f_formula + ")";
    }

    // --------------------------------------------------------------------------------------------

    void SMT_LIB_Solver::translate_greater_than_or_equal(ATermAppl a_clause) {
      ATermAppl v_clause_1, v_clause_2;

      v_clause_1 = f_expression_info.get_argument(a_clause, 0);
      v_clause_2 = f_expression_info.get_argument(a_clause, 1);
      f_formula = f_formula + "(>= ";
      translate_clause(v_clause_1, false);
      f_formula = f_formula + " ";
      translate_clause(v_clause_2, false);
      f_formula = f_formula + ")";
    }

    // --------------------------------------------------------------------------------------------

    void SMT_LIB_Solver::translate_less_than(ATermAppl a_clause) {
      ATermAppl v_clause_1, v_clause_2;

      v_clause_1 = f_expression_info.get_argument(a_clause, 0);
      v_clause_2 = f_expression_info.get_argument(a_clause, 1);
      f_formula = f_formula + "(< ";
      translate_clause(v_clause_1, false);
      f_formula = f_formula + " ";
      translate_clause(v_clause_2, false);
      f_formula = f_formula + ")";
    }

    // --------------------------------------------------------------------------------------------

    void SMT_LIB_Solver::translate_less_than_or_equal(ATermAppl a_clause) {
      ATermAppl v_clause_1, v_clause_2;

      v_clause_1 = f_expression_info.get_argument(a_clause, 0);
      v_clause_2 = f_expression_info.get_argument(a_clause, 1);
      f_formula = f_formula + "(<= ";
      translate_clause(v_clause_1, false);
      f_formula = f_formula + " ";
      translate_clause(v_clause_2, false);
      f_formula = f_formula + ")";
    }

    // --------------------------------------------------------------------------------------------

    void SMT_LIB_Solver::translate_plus(ATermAppl a_clause) {
      ATermAppl v_clause_1, v_clause_2;

      v_clause_1 = f_expression_info.get_argument(a_clause, 0);
      v_clause_2 = f_expression_info.get_argument(a_clause, 1);
      f_formula = f_formula + "(+ ";
      translate_clause(v_clause_1, false);
      f_formula = f_formula + " ";
      translate_clause(v_clause_2, false);
      f_formula = f_formula + ")";
    }

    // --------------------------------------------------------------------------------------------

    void SMT_LIB_Solver::translate_unary_minus(ATermAppl a_clause) {
      ATermAppl v_clause;

      v_clause = f_expression_info.get_argument(a_clause, 0);
      f_formula = f_formula + "(~";
      translate_clause(v_clause, false);
      f_formula = f_formula + ")";
    }

    // --------------------------------------------------------------------------------------------

    void SMT_LIB_Solver::translate_binary_minus(ATermAppl a_clause) {
      ATermAppl v_clause_1, v_clause_2;

      v_clause_1 = f_expression_info.get_argument(a_clause, 0);
      v_clause_2 = f_expression_info.get_argument(a_clause, 1);
      f_formula = f_formula + "(- ";
      translate_clause(v_clause_1, false);
      f_formula = f_formula + " ";
      translate_clause(v_clause_2, false);
      f_formula = f_formula + ")";
    }

    // --------------------------------------------------------------------------------------------

    void SMT_LIB_Solver::translate_multiplication(ATermAppl a_clause) {
      ATermAppl v_clause_1, v_clause_2;

      v_clause_1 = f_expression_info.get_argument(a_clause, 0);
      v_clause_2 = f_expression_info.get_argument(a_clause, 1);
      f_formula = f_formula + "(* ";
      translate_clause(v_clause_1, false);
      f_formula = f_formula + " ";
      translate_clause(v_clause_2, false);
      f_formula = f_formula + ")";
    }

    // --------------------------------------------------------------------------------------------

    void SMT_LIB_Solver::translate_add_c(ATermAppl a_clause) {
      ATermAppl v_clause_1, v_clause_2, v_clause_3;

      v_clause_1 = f_expression_info.get_argument(a_clause, 0);
      v_clause_2 = f_expression_info.get_argument(a_clause, 1);
      v_clause_3 = f_expression_info.get_argument(a_clause, 2);
      f_formula = f_formula + "(ite ";
      translate_clause(v_clause_1, true);
      f_formula = f_formula + " (+ ";
      translate_clause(v_clause_2, false);
      f_formula = f_formula + " ";
      translate_clause(v_clause_3, false);
      f_formula = f_formula + " 1) (+ ";
      translate_clause(v_clause_2, false);
      f_formula = f_formula + " ";
      translate_clause(v_clause_3, false);
      f_formula = f_formula + "))";
    }

    // --------------------------------------------------------------------------------------------

    void SMT_LIB_Solver::translate_c_nat(ATermAppl a_clause) {
      ATermAppl v_clause;

      v_clause = f_expression_info.get_argument(a_clause, 0);
      translate_clause(v_clause, false);
    }

    // --------------------------------------------------------------------------------------------

    void SMT_LIB_Solver::translate_unknown_operator(ATermAppl a_clause) {
      int v_number_of_arguments;
      int v_operator_number;
      ATermAppl v_operator;
      char* v_operator_string;
      ATermAppl v_clause;

      v_operator = f_expression_info.get_operator(a_clause);
      v_operator_number = ATindexedSetPut(f_operators, (ATerm) v_operator, 0);

      v_operator_string = (char*) malloc((number_of_digits(v_operator_number) + 3) * sizeof(char));
      sprintf(v_operator_string, "op%d", v_operator_number);
      f_formula = f_formula + "(" + v_operator_string;
      free(v_operator_string);
      v_operator_string = 0;
      v_number_of_arguments = f_expression_info.get_number_of_arguments(a_clause);
      for (int i = 0; i < v_number_of_arguments; i++) {
        v_clause = f_expression_info.get_argument(a_clause, i);
        f_formula = f_formula + " ";
        translate_clause(v_clause, false);
      }
      f_formula = f_formula + ")";
    }

    // --------------------------------------------------------------------------------------------

    void SMT_LIB_Solver::translate_variable(ATermAppl a_clause) {
      char* v_string;

      v_string = gsATermAppl2String(ATAgetArgument(a_clause, 0));
      f_formula = f_formula + v_string;

      ATindexedSetPut(f_variables, (ATerm) a_clause, 0);
    }

    // --------------------------------------------------------------------------------------------

    void SMT_LIB_Solver::translate_nat_variable(ATermAppl a_clause) {
      char* v_string;

      v_string = gsATermAppl2String(ATAgetArgument(a_clause, 0));
      f_formula = f_formula + v_string;

      ATindexedSetPut(f_variables, (ATerm) a_clause, 0);
      ATindexedSetPut(f_nat_variables, (ATerm) a_clause, 0);
    }

    // --------------------------------------------------------------------------------------------

    void SMT_LIB_Solver::translate_pos_variable(ATermAppl a_clause) {
      char* v_string;

      v_string = gsATermAppl2String(ATAgetArgument(a_clause, 0));
      f_formula = f_formula + v_string;

      ATindexedSetPut(f_variables, (ATerm) a_clause, 0);
      ATindexedSetPut(f_pos_variables, (ATerm) a_clause, 0);
    }

    // --------------------------------------------------------------------------------------------

    void SMT_LIB_Solver::translate_int_constant(ATermAppl a_clause) {
      char* v_value;
      string v_string;

      v_value = gsIntValue(a_clause);
      if (strncmp(v_value, "-", 1) == 0) {
        v_string = "~";
        v_string = v_string + (v_value + 1);
        f_formula = f_formula + "(" + v_string + ")";
      } else {
        f_formula = f_formula + v_value;
      }
      free(v_value);
    }

    // --------------------------------------------------------------------------------------------

    void SMT_LIB_Solver::translate_nat_constant(ATermAppl a_clause) {
      char* v_value;

      v_value = gsNatValue(a_clause);
      f_formula = f_formula + v_value;
      free(v_value);
    }

    // --------------------------------------------------------------------------------------------

    void SMT_LIB_Solver::translate_pos_constant(ATermAppl a_clause) {
      char* v_value;

      v_value = gsPosValue(a_clause);
      f_formula = f_formula + v_value;
      free(v_value);
    }

    // --------------------------------------------------------------------------------------------

    void SMT_LIB_Solver::translate_true() {
      f_formula = f_formula + "true";
    }

    // --------------------------------------------------------------------------------------------

    void SMT_LIB_Solver::translate_false() {
      f_formula = f_formula + "false";
    }

    // --------------------------------------------------------------------------------------------

    void SMT_LIB_Solver::translate_constant(ATermAppl a_clause) {
      int v_number_of_arguments;
      int v_operator_number;
      ATermAppl v_operator;
      char* v_operator_string;
      ATermAppl v_clause;

      v_operator = f_expression_info.get_operator(a_clause);
      v_operator_number = ATindexedSetPut(f_operators, (ATerm) v_operator, 0);

      v_operator_string = (char*) malloc((number_of_digits(v_operator_number) + 3) * sizeof(char));
      sprintf(v_operator_string, "op%d", v_operator_number);
      f_formula = f_formula + v_operator_string;
      free(v_operator_string);
      v_operator_string = 0;
      v_number_of_arguments = f_expression_info.get_number_of_arguments(a_clause);
      for (int i = 0; i < v_number_of_arguments; i++) {
        v_clause = f_expression_info.get_argument(a_clause, i);
        f_formula = f_formula + " ";
        translate_clause(v_clause, false);
      }
    }

    // --------------------------------------------------------------------------------------------

    void SMT_LIB_Solver::add_bool2pred_and_translate_clause(ATermAppl a_clause) {
      f_bool2pred = true;
      f_formula = f_formula + "(bool2pred ";
      translate_clause(a_clause, false);
      f_formula = f_formula + ")";
    }

    // --------------------------------------------------------------------------------------------

    void SMT_LIB_Solver::translate_clause(ATermAppl a_clause, bool a_expecting_predicate) {
      if (f_expression_info.is_not(a_clause)) {
        translate_not(a_clause);
      } else if (f_expression_info.is_equality(a_clause)) {
        translate_equality(a_clause);
      } else if (f_expression_info.is_inequality(a_clause)) {
        translate_inequality(a_clause);
      } else if (f_expression_info.is_greater_than(a_clause)) {
        translate_greater_than(a_clause);
      } else if (f_expression_info.is_greater_than_or_equal(a_clause)) {
        translate_greater_than_or_equal(a_clause);
      } else if (f_expression_info.is_less_than(a_clause)) {
        translate_less_than(a_clause);
      } else if (f_expression_info.is_less_than_or_equal(a_clause)) {
        translate_less_than_or_equal(a_clause);
      } else if (f_expression_info.is_plus(a_clause)) {
        translate_plus(a_clause);
      } else if (f_expression_info.is_unary_minus(a_clause)) {
        translate_unary_minus(a_clause);
      } else if (f_expression_info.is_binary_minus(a_clause)) {
        translate_binary_minus(a_clause);
      } else if (f_expression_info.is_multiplication(a_clause)) {
        translate_multiplication(a_clause);
      } else if (f_expression_info.is_add_c(a_clause)) {
        translate_add_c(a_clause);
      } else if (f_expression_info.is_c_nat(a_clause)) {
        translate_c_nat(a_clause);
      } else if (f_expression_info.is_int_constant(a_clause)) {
        translate_int_constant(a_clause);
      } else if (f_expression_info.is_nat_constant(a_clause)) {
        translate_nat_constant(a_clause);
      } else if (f_expression_info.is_pos_constant(a_clause)) {
        translate_pos_constant(a_clause);
      } else if (f_expression_info.is_true(a_clause) && a_expecting_predicate) {
        translate_true();
      } else if (f_expression_info.is_false(a_clause) && a_expecting_predicate) {
        translate_false();
      } else if (f_expression_info.is_variable(a_clause)) {
        if (a_expecting_predicate) {
          add_bool2pred_and_translate_clause(a_clause);
        } else if (f_sort_info.is_sort_nat(f_expression_info.get_sort_of_variable(a_clause))) {
          translate_nat_variable(a_clause);
        } else if (f_sort_info.is_sort_pos(f_expression_info.get_sort_of_variable(a_clause))) {
          translate_pos_variable(a_clause);
        } else {
          translate_variable(a_clause);
        }
      } else if (f_expression_info.is_operator(a_clause)) {
        if (a_expecting_predicate) {
          add_bool2pred_and_translate_clause(a_clause);
        } else {
          translate_unknown_operator(a_clause);
        }
      } else if (f_expression_info.is_constant(a_clause)) {
        translate_constant(a_clause);
      } else {
        gsErrorMsg("Unable to handle the current clause (%T).\n", a_clause);
        exit(1);
      }
    }

    // --------------------------------------------------------------------------------------------

    void SMT_LIB_Solver::add_nat_clauses() {
      ATermList v_variables;
      ATermAppl v_variable;
      char* v_variable_string;

      v_variables = ATindexedSetElements(f_nat_variables);
      if (!ATisEmpty(v_variables)) {
        while (!ATisEmpty(v_variables)) {
          v_variable = ATAgetFirst(v_variables);
          v_variables = ATgetNext(v_variables);
          v_variable_string = gsATermAppl2String(ATAgetArgument(v_variable, 0));
          f_formula = f_formula + " (>= " + v_variable_string + " 0)";
        }
      }
    }

    // --------------------------------------------------------------------------------------------

    void SMT_LIB_Solver::add_pos_clauses() {
      ATermList v_variables;
      ATermAppl v_variable;
      char* v_variable_string;

      v_variables = ATindexedSetElements(f_pos_variables);
      if (!ATisEmpty(v_variables)) {
        while (!ATisEmpty(v_variables)) {
          v_variable = ATAgetFirst(v_variables);
          v_variables = ATgetNext(v_variables);
          v_variable_string = gsATermAppl2String(ATAgetArgument(v_variable, 0));
          f_formula = f_formula + " (>= " + v_variable_string + " 1)";
        }
      }
    }

  // Class SMT_LIB_Solver - Functions declared protected ------------------------------------------

    void SMT_LIB_Solver::translate(ATermList a_formula) {
      ATermAppl v_clause;

      ATindexedSetReset(f_sorts);
      ATindexedSetReset(f_operators);
      ATindexedSetReset(f_variables);
      ATindexedSetReset(f_nat_variables);
      ATindexedSetReset(f_pos_variables);
      f_bool2pred = false;

      f_formula = "  :formula (and";
      gsVerboseMsg("Formula to be solved: %P\n", a_formula);
      while (!ATisEmpty(a_formula)) {
        v_clause = ATAgetFirst(a_formula);
        a_formula = ATgetNext(a_formula);
        f_formula = f_formula + " ";
        translate_clause(v_clause, true);
      }
      add_nat_clauses();
      add_pos_clauses();
      f_formula = f_formula + ")\n";
      declare_variables();
      declare_operators();
      declare_predicates();
      declare_sorts();
      produce_notes_for_sorts();
      produce_notes_for_operators();
      produce_notes_for_predicates();
      f_benchmark =
        "(benchmark nameless\n" + f_sorts_notes + f_operators_notes + f_predicates_notes +
        f_extrasorts + f_operators_extrafuns + f_variables_extrafuns + f_extrapreds + f_formula +
        ")\n";
      gsVerboseMsg("Corresponding benchmark:\n%s", f_benchmark.c_str());
    }

  // Class SMT_LIB_Solver - Functions declared public ---------------------------------------------

    SMT_LIB_Solver::SMT_LIB_Solver() {
      f_sorts = ATindexedSetCreate(100, 75);
      f_operators = ATindexedSetCreate(100, 75);
      f_variables = ATindexedSetCreate(100, 75);
      f_nat_variables = ATindexedSetCreate(100, 75);
      f_pos_variables = ATindexedSetCreate(100, 75);
    }

    // --------------------------------------------------------------------------------------------

    SMT_LIB_Solver::~SMT_LIB_Solver() {
      ATindexedSetDestroy(f_sorts);
      ATindexedSetDestroy(f_operators);
      ATindexedSetDestroy(f_variables);
      ATindexedSetDestroy(f_nat_variables);
      ATindexedSetDestroy(f_pos_variables);
    }
