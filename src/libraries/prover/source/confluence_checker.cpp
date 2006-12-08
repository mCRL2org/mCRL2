// Implementation of class Confluence_Checker
// file: confluence_checker.cpp

#include "confluence_checker.h"
#include "liblowlevel.h"
#include "libprint_c.h"
#include "libstruct.h"
#include "prover/bdd_prover.h"
#include "cstdlib"
#include <string>

// Auxiliary functions ----------------------------------------------------------------------------

  ATermList get_substitutions_from_assignments(ATermList a_assignments) {
    ATermAppl v_assignment;
    ATermAppl v_variable;
    ATermAppl v_expression;
    ATermAppl v_substitution;
    ATermList v_substitutions = ATmakeList0();

    while (!ATisEmpty(a_assignments)) {
      v_assignment = ATAgetFirst(a_assignments);
      v_variable = ATAgetArgument(v_assignment, 0);
      v_expression = ATAgetArgument(v_assignment, 1);
      v_substitution = gsMakeSubst_Appl(v_variable, v_expression);
      v_substitutions = ATinsert(v_substitutions, (ATerm) v_substitution);
      a_assignments = ATgetNext(a_assignments);
    }
    return v_substitutions;
  }

  // ----------------------------------------------------------------------------------------------

  ATermAppl get_subst_equation_from_assignments(
    ATermList a_variables, ATermList a_assignments_1, ATermList a_assignments_2, ATermList a_substitutions_1, ATermList a_substitutions_2
  ) {
    ATermAppl v_result = gsMakeDataExprTrue();
    ATermAppl v_variable;
    ATermAppl v_assignment_1, v_assignment_2;
    ATermAppl v_variable_1 = 0, v_variable_2 = 0, v_expression_1 = 0, v_expression_2 = 0;
    bool v_next_1 = true, v_next_2 = true;

    while (!ATisEmpty(a_variables)) {
      v_variable = ATAgetFirst(a_variables);
      a_variables = ATgetNext(a_variables);
      if (!ATisEmpty(a_assignments_1) && v_next_1) {
        v_assignment_1 = ATAgetFirst(a_assignments_1);
        a_assignments_1 = ATgetNext(a_assignments_1);
        v_variable_1 = ATAgetArgument(v_assignment_1, 0);
        v_expression_1 = ATAgetArgument(v_assignment_1, 1);
        v_expression_1 = gsSubstValues_Appl(a_substitutions_2, v_expression_1, true);
      }
      if (!ATisEmpty(a_assignments_2) && v_next_2) {
        v_assignment_2 = ATAgetFirst(a_assignments_2);
        a_assignments_2 = ATgetNext(a_assignments_2);
        v_variable_2 = ATAgetArgument(v_assignment_2, 0);
        v_expression_2 = ATAgetArgument(v_assignment_2, 1);
        v_expression_2 = gsSubstValues_Appl(a_substitutions_1, v_expression_2, true);
      }
      while (v_variable != v_variable_1 && v_variable != v_variable_2 && !ATisEmpty(a_variables)) {
        v_variable = ATAgetFirst(a_variables);
        a_variables = ATgetNext(a_variables);
      }
      if (v_variable_1 == v_variable_2) {
        v_result = gsMakeDataExprAnd(v_result, gsMakeDataExprEq(v_expression_1, v_expression_2));
        v_next_1 = true;
        v_next_2 = true;
      } else if (v_variable == v_variable_1) {
        v_variable_1 = gsSubstValues_Appl(a_substitutions_1, v_variable_1, true);
        v_result = gsMakeDataExprAnd(v_result, gsMakeDataExprEq(v_expression_1, v_variable_1));
        v_next_1 = true;
        v_next_2 = false;
      } else if (v_variable == v_variable_2) {
        v_variable_2 = gsSubstValues_Appl(a_substitutions_2, v_variable_2, true);
        v_result = gsMakeDataExprAnd(v_result, gsMakeDataExprEq(v_expression_2, v_variable_2));
        v_next_1 = false;
        v_next_2 = true;
      }
    }
    return v_result;
  }

  // ----------------------------------------------------------------------------------------------

  ATermAppl get_equation_from_assignments(ATermList a_variables, ATermList a_assignments_1, ATermList a_assignments_2) {
    ATermAppl v_result = gsMakeDataExprTrue();
    ATermAppl v_variable;
    ATermAppl v_assignment_1, v_assignment_2;
    ATermAppl v_variable_1 = 0, v_variable_2 = 0, v_expression_1 = 0, v_expression_2 = 0;
    bool v_next_1 = true, v_next_2 = true;

    while (!ATisEmpty(a_variables)) {
      v_variable = ATAgetFirst(a_variables);
      a_variables = ATgetNext(a_variables);
      if (!ATisEmpty(a_assignments_1) && v_next_1) {
        v_assignment_1 = ATAgetFirst(a_assignments_1);
        a_assignments_1 = ATgetNext(a_assignments_1);
        v_variable_1 = ATAgetArgument(v_assignment_1, 0);
        v_expression_1 = ATAgetArgument(v_assignment_1, 1);
      }
      if (!ATisEmpty(a_assignments_2) && v_next_2) {
        v_assignment_2 = ATAgetFirst(a_assignments_2);
        a_assignments_2 = ATgetNext(a_assignments_2);
        v_variable_2 = ATAgetArgument(v_assignment_2, 0);
        v_expression_2 = ATAgetArgument(v_assignment_2, 1);
      }
      while (v_variable != v_variable_1 && v_variable != v_variable_2 && !ATisEmpty(a_variables)) {
        v_variable = ATAgetFirst(a_variables);
        a_variables = ATgetNext(a_variables);
      }
      if (v_variable_1 == v_variable_2) {
        v_result = gsMakeDataExprAnd(v_result, gsMakeDataExprEq(v_expression_1, v_expression_2));
        v_next_1 = true;
        v_next_2 = true;
      } else if (v_variable == v_variable_1) {
        v_result = gsMakeDataExprAnd(v_result, gsMakeDataExprEq(v_expression_1, v_variable_1));
        v_next_1 = true;
        v_next_2 = false;
      } else if (v_variable == v_variable_2) {
        v_result = gsMakeDataExprAnd(v_result, gsMakeDataExprEq(v_expression_2, v_variable_2));
        v_next_1 = false;
        v_next_2 = true;
      }
    }
    return v_result;
  }

  // ----------------------------------------------------------------------------------------------

  ATermAppl get_subst_equation_from_actions(ATermList a_actions, ATermList a_substitutions) {
    ATermAppl v_result = gsMakeDataExprTrue();
    ATermAppl v_action;
    ATermList v_expressions;
    ATermAppl v_expression;
    ATermAppl v_subst_expression;

    while (!ATisEmpty(a_actions)) {
      v_action = ATAgetFirst(a_actions);
      v_expressions = ATLgetArgument(v_action, 1);
      while (!ATisEmpty(v_expressions)) {
        v_expression = ATAgetFirst(v_expressions);
        v_subst_expression = gsSubstValues_Appl(a_substitutions, v_expression, true);
        v_result = gsMakeDataExprAnd(v_result, gsMakeDataExprEq(v_expression, v_subst_expression));
        v_expressions = ATgetNext(v_expressions);
      }
      a_actions = ATgetNext(a_actions);
    }
    return v_result;
  }

  // ----------------------------------------------------------------------------------------------

  ATermAppl get_confluence_condition(ATermAppl a_invariant, ATermAppl a_summand_1, ATermAppl a_summand_2, ATermList a_variables) {
    ATermAppl v_condition_1, v_condition_2;
    ATermList v_assignments_1, v_assignments_2;
    ATermList v_substitutions_1, v_substitutions_2;
    ATermAppl v_subst_condition_1, v_subst_condition_2;
    ATermAppl v_lhs, v_rhs;
    ATermAppl v_equation;
    ATermAppl v_subst_equation;
    ATermList v_actions;
    ATermAppl v_actions_equation;

    v_condition_1 = ATAgetArgument(a_summand_1, 1);
    v_assignments_1 = ATLgetArgument(a_summand_1, 4);
    v_substitutions_1 = get_substitutions_from_assignments(v_assignments_1);
    v_condition_2 = ATAgetArgument(a_summand_2, 1);
    v_lhs = gsMakeDataExprAnd(v_condition_1, v_condition_2);
    v_lhs = gsMakeDataExprAnd(v_lhs, a_invariant);
    v_assignments_2 = ATLgetArgument(a_summand_2, 4);
    v_substitutions_2 = get_substitutions_from_assignments(v_assignments_2);
    v_subst_condition_1 = gsSubstValues_Appl(v_substitutions_2, v_condition_1, true);
    v_subst_condition_2 = gsSubstValues_Appl(v_substitutions_1, v_condition_2, true);
    v_subst_equation = get_subst_equation_from_assignments(a_variables, v_assignments_1, v_assignments_2, v_substitutions_1, v_substitutions_2);

    v_actions = ATLgetArgument(ATAgetArgument(a_summand_2, 2), 0);
    if (ATisEmpty(v_actions)) {
      // tau-summand
      v_equation = get_equation_from_assignments(a_variables, v_assignments_1, v_assignments_2);
      v_rhs = gsMakeDataExprAnd(v_subst_condition_1, v_subst_condition_2);
      v_rhs = gsMakeDataExprAnd(v_rhs, v_subst_equation);
      v_rhs = gsMakeDataExprOr(v_equation, v_rhs);
    } else {
      // non-tau-summand
      v_actions_equation = get_subst_equation_from_actions(v_actions, v_substitutions_1);
      v_rhs = gsMakeDataExprAnd(v_subst_condition_1, v_subst_condition_2);
      v_rhs = gsMakeDataExprAnd(v_rhs, v_actions_equation);
      v_rhs = gsMakeDataExprAnd(v_rhs, v_subst_equation);
    }
    return gsMakeDataExprImp(v_lhs, v_rhs);
  }

  // --------------------------------------------------------------------------------------------

  bool has_ctau_action(ATermAppl a_lpe) {
    ATermList v_action_specification;
    ATermAppl v_action;
    ATermAppl v_ctau_action;
    bool v_has_ctau_action = false;

    v_ctau_action = make_ctau_act_id();
    v_action_specification = ATLgetArgument(ATAgetArgument(a_lpe, 1), 0);
    while (!ATisEmpty(v_action_specification)) {
      v_action = ATAgetFirst(v_action_specification);
      if (v_action == v_ctau_action) {
        v_has_ctau_action = true;
      }
      v_action_specification = ATgetNext(v_action_specification);
    }
    return v_has_ctau_action;
  }

  // --------------------------------------------------------------------------------------------

  ATermAppl add_ctau_action(ATermAppl a_lpe) {
    ATermList v_action_specification;

    v_action_specification = ATLgetArgument(ATAgetArgument(a_lpe, 1), 0);
    v_action_specification = ATinsert(v_action_specification, (ATerm) make_ctau_act_id());
    a_lpe = ATsetArgument(a_lpe, (ATerm) gsMakeActSpec(v_action_specification), 1);
    return a_lpe;
  }

// Class Confluence_Checker -----------------------------------------------------------------------
  // Class Confluence_Checker - Functions declared private ----------------------------------------

    void Confluence_Checker::save_dot_file(int a_summand_number_1, int a_summand_number_2) {
      char* v_file_name;
      char* v_file_name_suffix;

      if (f_dot_file_name != 0) {
        v_file_name_suffix = (char*) malloc((number_of_digits(a_summand_number_1) + number_of_digits(a_summand_number_2) + 7) * sizeof(char));
        sprintf(v_file_name_suffix, "-%d-%d.dot", a_summand_number_1, a_summand_number_2);
        v_file_name = (char*) malloc((strlen(f_dot_file_name) + strlen(v_file_name_suffix) + 1) * sizeof(char));
        strcpy(v_file_name, f_dot_file_name);
        strcat(v_file_name, v_file_name_suffix);
        f_bdd2dot.output_bdd(f_bdd_prover.get_bdd(), v_file_name);
        free(v_file_name);
        v_file_name = 0;
        free(v_file_name_suffix);
        v_file_name_suffix = 0;
      }
    }

    // --------------------------------------------------------------------------------------------

    void Confluence_Checker::print_counter_example() {
      if (f_counter_example) {
        ATermAppl v_counter_example;

        v_counter_example = f_bdd_prover.get_counter_example();
        if (v_counter_example == 0) {
          gsErrorMsg(
            "Cannot print counter example. This is probably caused by an abrupt stop of the\n"
            "conversion from expression to EQ-BDD. This typically occurs when a time limit is set.\n"
          );
          exit(1);
        } else {
          gsfprintf(stderr, "  Counter example: %P\n", v_counter_example);
        }
      }
    }

    // --------------------------------------------------------------------------------------------

    bool Confluence_Checker::check_summands(ATermAppl a_invariant, ATermAppl a_summand_1, int a_summand_number_1, ATermAppl a_summand_2, int a_summand_number_2) {
      ATermList v_variables = ATLgetArgument(ATAgetArgument(f_lpe, 2), 1);
      ATermAppl v_multi_actions_or_delta, v_condition, v_new_invariant;
      bool v_is_confluent = true;

      if (f_disjointness_checker.disjoint(a_summand_number_1, a_summand_number_2)) {
        gsfprintf(stderr, ":");
      } else {
        v_multi_actions_or_delta = ATAgetArgument(a_summand_1, 2);
        if (!gsIsDelta(v_multi_actions_or_delta)) {
          v_condition = get_confluence_condition(a_invariant, a_summand_1, a_summand_2, v_variables);
          f_bdd_prover.set_formula(v_condition);
          if (f_bdd_prover.is_tautology() == answer_yes) {
            gsfprintf(stderr, "+");
          } else {
            if (f_generate_invariants) {
              v_new_invariant = f_bdd_prover.get_bdd();
              gsVerboseMsg("\nChecking invariant: %P\n", v_new_invariant);
              if (f_invariant_checker.check_invariant(v_new_invariant)) {
                gsVerboseMsg("Invariant holds\n");
                gsfprintf(stderr, "i");
              } else {
                gsVerboseMsg("Invariant doesn't hold\n");
                v_is_confluent = false;
                if (f_check_all) {
                  gsfprintf(stderr, "-");
                } else {
                  gsfprintf(stderr, "Not confluent with summand %d.", a_summand_number_2);
                }
                print_counter_example();
                save_dot_file(a_summand_number_1, a_summand_number_2);
              }
            } else {
              v_is_confluent = false;
              if (f_check_all) {
                gsfprintf(stderr, "-");
              } else {
                gsfprintf(stderr, "Not confluent with summand %d.", a_summand_number_2);
              }
              print_counter_example();
              save_dot_file(a_summand_number_1, a_summand_number_2);
            }
          }
        } else {
          gsfprintf(stderr, "!");
        }
      }
      return v_is_confluent;
    }

    // --------------------------------------------------------------------------------------------

    ATermAppl Confluence_Checker::check_confluence_and_mark_summand(ATermAppl a_invariant, ATermAppl a_summand, int a_summand_number, bool& a_is_marked) {
      ATermList v_summands = ATLgetArgument(ATAgetArgument(f_lpe, 2), 2);
      ATermAppl v_summand, v_marked_summand;
      int v_summand_number = 1;
      bool v_is_confluent = true;
      bool v_current_summands_are_confluent;

      while (!ATisEmpty(v_summands) && (v_is_confluent || f_check_all)) {
        v_summand = ATAgetFirst(v_summands);
        v_summands = ATgetNext(v_summands);

        if (v_summand_number < a_summand_number) {
          if (f_intermediate[v_summand_number] > a_summand_number) {
            gsfprintf(stderr, ".");
            v_summand_number++;
          } else {
            if (f_intermediate[v_summand_number] == a_summand_number) {
              if (f_check_all) {
                gsfprintf(stderr, "-");
              } else {
                gsfprintf(stderr, "Not confluent with summand %d.", v_summand_number);
              }
              v_is_confluent = false;
            } else {
              v_current_summands_are_confluent = check_summands(a_invariant, a_summand, a_summand_number, v_summand, v_summand_number);
              if (v_current_summands_are_confluent) {
                v_summand_number++;
              } else {
                v_is_confluent = false;
              }
            }
          }
        } else {
          v_current_summands_are_confluent = check_summands(a_invariant, a_summand, a_summand_number, v_summand, v_summand_number);
          if (v_current_summands_are_confluent) {
            v_summand_number++;
          } else {
            v_is_confluent = false;
          }
        }
      }

      if (!f_check_all) {
        f_intermediate[a_summand_number] = v_summand_number;
      }

      if (v_is_confluent) {
        gsfprintf(stderr, "Confluent with all summands.");
        a_is_marked = true;
        v_marked_summand = ATsetArgument(a_summand, (ATerm) gsMakeMultAct(ATmakeList1((ATerm) make_ctau_action())), 2);
        return v_marked_summand;
      } else {
        return a_summand;
      }
    }

  // Class Confluence_Checker - Functions declared public -----------------------------------------

    Confluence_Checker::Confluence_Checker(
      ATermAppl a_lpe, RewriteStrategy a_rewrite_strategy, int a_time_limit, bool a_path_eliminator, SMT_Solver_Type a_solver_type,
      bool a_apply_induction, bool a_no_marking, bool a_check_all, bool a_counter_example, bool a_generate_invariants, char* a_dot_file_name
    ):
      f_disjointness_checker(ATAgetArgument(a_lpe, 2)),
      f_invariant_checker(a_lpe, a_rewrite_strategy, a_time_limit, a_path_eliminator, a_solver_type, false, false, 0),
      f_bdd_prover(a_lpe, a_rewrite_strategy, a_time_limit, a_path_eliminator, a_solver_type, a_apply_induction)
    {
      if (has_ctau_action(a_lpe)) {
        gsErrorMsg("An action named \'ctau\' already exists.\n");
        exit(1);
      }

      f_lpe = a_lpe;
      f_no_marking = a_no_marking;
      f_check_all = a_check_all;
      f_counter_example = a_counter_example;
      if (a_dot_file_name == 0) {
        f_dot_file_name = 0;
      } else {
        f_dot_file_name = strdup(a_dot_file_name);
      }
      f_generate_invariants = a_generate_invariants;
    }

    // --------------------------------------------------------------------------------------------

    Confluence_Checker::~Confluence_Checker() {
      free(f_dot_file_name);
    }

    // --------------------------------------------------------------------------------------------

    ATermAppl Confluence_Checker::check_confluence_and_mark(ATermAppl a_invariant, int a_summand_number) {
      ATermAppl v_process_equation = ATAgetArgument(f_lpe, 2);
      ATermList v_summands = ATLgetArgument(v_process_equation, 2);
      ATermAppl v_summand;
      ATermList v_marked_summands = ATmakeList0();
      ATermAppl v_marked_summand;
      ATermAppl v_multi_actions_or_delta;
      ATermList v_multi_actions;
      bool v_is_marked = false;
      int v_summand_number = 1;

      f_number_of_summands = ATgetLength(v_summands);
      f_intermediate = (int*) calloc(f_number_of_summands + 2, sizeof(int));
      if (f_intermediate == 0) {
        gsErrorMsg("Insufficient memory.\n");
      }

      while (!ATisEmpty(v_summands)) {
        v_summand = ATAgetFirst(v_summands);
        v_marked_summand = v_summand;
        if ((a_summand_number == v_summand_number) || (a_summand_number == 0)) {
          v_multi_actions_or_delta = ATAgetArgument(v_summand, 2);
          if (!gsIsDelta(v_multi_actions_or_delta)) {
            v_multi_actions = ATLgetArgument(v_multi_actions_or_delta, 0);
            if (ATisEmpty(v_multi_actions)) {
              gsfprintf(stderr, "tau-summand %2d: ", v_summand_number);
              v_marked_summand = check_confluence_and_mark_summand(a_invariant, v_summand, v_summand_number, v_is_marked);
              gsfprintf(stderr, "\n");
            }
          }
        }
        v_marked_summands = ATinsert(v_marked_summands, (ATerm) v_marked_summand);
        v_summands = ATgetNext(v_summands);
        v_summand_number++;
      }
      v_marked_summands = ATreverse(v_marked_summands);
      v_process_equation = ATsetArgument(v_process_equation, (ATerm) v_marked_summands, 2);
      ATermAppl v_lpe = ATsetArgument(f_lpe, (ATerm) v_process_equation, 2);

      if (v_is_marked && !has_ctau_action(f_lpe)) {
        v_lpe = add_ctau_action(v_lpe);
      }

      free(f_intermediate);
      f_intermediate = 0;

      return v_lpe;
    }
