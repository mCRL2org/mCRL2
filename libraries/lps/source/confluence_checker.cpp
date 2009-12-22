// Author(s): Luc Engelen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file confluence_checker.cpp

#include <cstdlib>
#include <cstring>
#include <string>

#include "mcrl2/core/messaging.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/data/detail/bdd_prover.h"
#include "mcrl2/lps/confluence_checker.h"
#include "mcrl2/exception.h"

using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::data::detail;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;

// Auxiliary functions ----------------------------------------------------------------------------

  bool is_tau_summand(ATermAppl a_summand) {
    ATermAppl v_multi_action_or_delta = ATAgetArgument(a_summand, 2);
    if (gsIsMultAct(v_multi_action_or_delta)) {
      return ATisEmpty(ATLgetArgument(v_multi_action_or_delta, 0));
    } else {
      return false;
    }
  }

  bool is_delta_summand(ATermAppl a_summand) {
    return gsIsDelta(ATAgetArgument(a_summand, 2));
  }

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
    ATermAppl v_result = sort_bool::true_();
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
        v_result = sort_bool::and_(data_expression(v_result), equal_to(data_expression(v_expression_1), data_expression(v_expression_2)));
        v_next_1 = true;
        v_next_2 = true;
      } else if (v_variable == v_variable_1) {
        v_variable_1 = gsSubstValues_Appl(a_substitutions_1, v_variable_1, true);
        v_result = sort_bool::and_(data_expression(v_result), equal_to(data_expression(v_expression_1), data_expression(v_variable_1)));
        v_next_1 = true;
        v_next_2 = false;
      } else if (v_variable == v_variable_2) {
        v_variable_2 = gsSubstValues_Appl(a_substitutions_2, v_variable_2, true);
        v_result = sort_bool::and_(data_expression(v_result), equal_to(data_expression(v_expression_2), data_expression(v_variable_2)));
        v_next_1 = false;
        v_next_2 = true;
      }
    }
    return v_result;
  }

  // ----------------------------------------------------------------------------------------------

  ATermAppl get_equation_from_assignments(ATermList a_variables, ATermList a_assignments_1, ATermList a_assignments_2) {
    ATermAppl v_result = sort_bool::true_();
    ATermAppl v_variable;
    ATermAppl v_assignment_1, v_assignment_2;
    ATermAppl v_variable_1 = 0, v_variable_2 = 0, v_expression_1 = 0, v_expression_2 = 0;
    bool v_next_1 = true, v_next_2 = true;

    while (!ATisEmpty(a_variables))
    {
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
        v_result = sort_bool::and_(data_expression(v_result), equal_to(data_expression(v_expression_1), data_expression(v_expression_2)));
        v_next_1 = true;
        v_next_2 = true;
      } else if (v_variable == v_variable_1) {
        v_result = sort_bool::and_(data_expression(v_result), equal_to(data_expression(v_expression_1), data_expression(v_variable_1)));
        v_next_1 = true;
        v_next_2 = false;
      } else if (v_variable == v_variable_2) {
        v_result = sort_bool::and_(data_expression(v_result), equal_to(data_expression(v_expression_2), data_expression(v_variable_2)));
        v_next_1 = false;
        v_next_2 = true;
      }
    }
    assert(ATisEmpty(a_assignments_1)); // If this is not the case, the assignments do not have the
    assert(ATisEmpty(a_assignments_2)); // same order as the list of variables. This means that some equations
                                        // have not been generated.
    return v_result;
  }

  // ----------------------------------------------------------------------------------------------

  ATermAppl get_subst_equation_from_actions(ATermList a_actions, ATermList a_substitutions) {
    ATermAppl v_result = sort_bool::true_();
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
        v_result = sort_bool::and_(data_expression(v_result), equal_to(data_expression(v_expression), data_expression(v_subst_expression)));
        v_expressions = ATgetNext(v_expressions);
      }
      a_actions = ATgetNext(a_actions);
    }
    return v_result;
  }

  // ----------------------------------------------------------------------------------------------

  ATermAppl get_confluence_condition(ATermAppl a_invariant, ATermAppl a_summand_1, ATermAppl a_summand_2, ATermList a_variables)
  {
    assert(is_tau_summand(a_summand_1));
    assert(!is_delta_summand(a_summand_2));
    ATermAppl v_rhs;

    ATermAppl v_condition_1 = ATAgetArgument(a_summand_1, 1);
    ATermList v_assignments_1 = ATLgetArgument(a_summand_1, 4);
    ATermList v_substitutions_1 = get_substitutions_from_assignments(v_assignments_1);
    ATermAppl v_condition_2 = ATAgetArgument(a_summand_2, 1);
    ATermAppl v_lhs = sort_bool::and_(data_expression(v_condition_1), data_expression(v_condition_2));
    v_lhs = sort_bool::and_(data_expression(v_lhs), data_expression(a_invariant));
    ATermList v_assignments_2 = ATLgetArgument(a_summand_2, 4);
    ATermList v_substitutions_2 = get_substitutions_from_assignments(v_assignments_2);
    ATermAppl v_subst_condition_1 = gsSubstValues_Appl(v_substitutions_2, v_condition_1, true);
    ATermAppl v_subst_condition_2 = gsSubstValues_Appl(v_substitutions_1, v_condition_2, true);
    ATermAppl v_subst_equation = get_subst_equation_from_assignments(a_variables, v_assignments_1, v_assignments_2, v_substitutions_1, v_substitutions_2);

    ATermList v_actions = ATLgetArgument(ATAgetArgument(a_summand_2, 2), 0);
    if (ATisEmpty(v_actions)) {
      // tau-summand
      ATermAppl v_equation = get_equation_from_assignments(a_variables, v_assignments_1, v_assignments_2);
      v_rhs = sort_bool::and_(data_expression(v_subst_condition_1), data_expression(v_subst_condition_2));
      v_rhs = sort_bool::and_(data_expression(v_rhs), data_expression(v_subst_equation));
      v_rhs = sort_bool::or_(data_expression(v_equation), data_expression(v_rhs));
    } else {
      // non-tau-summand
      ATermAppl v_actions_equation = get_subst_equation_from_actions(v_actions, v_substitutions_1);
      v_rhs = sort_bool::and_(data_expression(v_subst_condition_1), data_expression(v_subst_condition_2));
      v_rhs = sort_bool::and_(data_expression(v_rhs), data_expression(v_actions_equation));
      v_rhs = sort_bool::and_(data_expression(v_rhs), data_expression(v_subst_equation));
    }
    return sort_bool::implies(data_expression(v_lhs), data_expression(v_rhs));
  }

  // --------------------------------------------------------------------------------------------

  bool has_ctau_action(mcrl2::lps::specification const& a_lps) {
    ATermList v_action_specification;
    ATermAppl v_action;
    ATermAppl v_ctau_action;

    v_ctau_action = make_ctau_act_id();
    v_action_specification = a_lps.action_labels();
    while (!ATisEmpty(v_action_specification)) {
      v_action = ATAgetFirst(v_action_specification);
      if (v_action == v_ctau_action) {
        return true;
      }
      v_action_specification = ATgetNext(v_action_specification);
    }
    return false;
  }

  // --------------------------------------------------------------------------------------------

  ATermAppl add_ctau_action(ATermAppl a_lps) {
    ATermList v_action_specification;

    v_action_specification = ATLgetArgument(ATAgetArgument(a_lps, 1), 0);
    v_action_specification = ATinsert(v_action_specification, (ATerm) make_ctau_act_id());
    a_lps = ATsetArgument(a_lps, (ATerm) gsMakeActSpec(v_action_specification), 1);
    return a_lps;
  }

// Class Confluence_Checker -----------------------------------------------------------------------
  // Class Confluence_Checker - Functions declared private ----------------------------------------

    void Confluence_Checker::save_dot_file(int a_summand_number_1, int a_summand_number_2) {
      if (f_dot_file_name.empty()) {
        std::ostringstream v_file_name(f_dot_file_name);

        v_file_name << "-" << a_summand_number_1 << "-" << a_summand_number_2 << ".dot";

        f_bdd2dot.output_bdd(f_bdd_prover.get_bdd(), v_file_name.str().c_str());
      }
    }

    // --------------------------------------------------------------------------------------------

    void Confluence_Checker::print_counter_example() {
      if (f_counter_example) {
        ATermAppl v_counter_example;

        v_counter_example = f_bdd_prover.get_counter_example();
        if (v_counter_example == 0) {
          throw mcrl2::runtime_error(
            "Cannot print counter example. This is probably caused by an abrupt stop of the\n"
            "conversion from expression to EQ-BDD. This typically occurs when a time limit is set."
          );
        } else {
          gsMessage("  Counter example: %P\n", v_counter_example);
        }
      }
    }

    // --------------------------------------------------------------------------------------------

    bool Confluence_Checker::check_summands(ATermAppl a_invariant, ATermAppl a_summand_1, int a_summand_number_1, ATermAppl a_summand_2, int a_summand_number_2) {
      assert(is_tau_summand(a_summand_1));
      ATermList v_variables = ATLgetArgument(ATAgetArgument(specification_to_aterm(f_lps), 3), 0);
      bool v_is_confluent = true;

      if (f_disjointness_checker.disjoint(a_summand_number_1, a_summand_number_2)) {
        gsMessage(":");
      } else {
        if (!is_delta_summand(a_summand_2)) {
          ATermAppl v_condition = get_confluence_condition(a_invariant, a_summand_1, a_summand_2, v_variables);
          f_bdd_prover.set_formula(v_condition);
          if (f_bdd_prover.is_tautology() == answer_yes) {
            gsMessage("+");
          } else {
            if (f_generate_invariants) {
              ATermAppl v_new_invariant = f_bdd_prover.get_bdd();
              gsVerboseMsg("\nChecking invariant: %P\n", v_new_invariant);
              if (f_invariant_checker.check_invariant(v_new_invariant)) {
                gsVerboseMsg("Invariant holds\n");
                gsMessage("i");
              } else {
                gsVerboseMsg("Invariant doesn't hold\n");
                v_is_confluent = false;
                if (f_check_all) {
                  gsMessage("-");
                } else {
                  gsMessage("Not confluent with summand %d.", a_summand_number_2);
                }
                print_counter_example();
                save_dot_file(a_summand_number_1, a_summand_number_2);
              }
            } else {
              v_is_confluent = false;
              if (f_check_all) {
                gsMessage("-");
              } else {
                gsMessage("Not confluent with summand %d.", a_summand_number_2);
              }
              print_counter_example();
              save_dot_file(a_summand_number_1, a_summand_number_2);
            }
          }
        } else {
          gsMessage("!");
        }
      }
      return v_is_confluent;
    }

    // --------------------------------------------------------------------------------------------

    ATermAppl Confluence_Checker::check_confluence_and_mark_summand(ATermAppl a_invariant, ATermAppl a_summand, int a_summand_number, bool& a_is_marked) {
      assert(is_tau_summand(a_summand));
      ATermList v_summands = ATLgetArgument(ATAgetArgument(specification_to_aterm(f_lps), 3), 1);
      ATermAppl v_summand, v_marked_summand;
      int v_summand_number = 1;
      bool v_is_confluent = true;
      bool v_current_summands_are_confluent;

      // Add here that the sum variables of a_summand must be empty otherwise
      // the confluence of the summand must be checked with respect to itself,
      // which requires quantification. Otherwise tau.a+tau.b will be designated
      // tau-confluent, if linearised with summand clustering.

      ATermList a_summand_sum_variables=ATLgetArgument(a_summand,0);
      if (!ATisEmpty(a_summand_sum_variables))
      { v_is_confluent = false;
        gsMessage("Summand %d is not proven confluent because it contains a sum operator.",a_summand_number);
      }

      while (!ATisEmpty(v_summands) && (v_is_confluent || f_check_all)) {
        v_summand = ATAgetFirst(v_summands);
        v_summands = ATgetNext(v_summands);

        if (v_summand_number < a_summand_number) {
          if (f_intermediate[v_summand_number] > a_summand_number) {
            gsMessage(".");
            v_summand_number++;
          } else {
            if (f_intermediate[v_summand_number] == a_summand_number) {
              if (f_check_all) {
                gsMessage("-");
              } else {
                gsMessage("Not confluent with summand %d.", v_summand_number);
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
        gsMessage("Confluent with all summands.");
        a_is_marked = true;
        v_marked_summand = ATsetArgument(a_summand, (ATerm) gsMakeMultAct(ATmakeList1((ATerm) make_ctau_action())), 2);
        return v_marked_summand;
      } else {
        return a_summand;
      }
    }

  // Class Confluence_Checker - Functions declared public -----------------------------------------

    Confluence_Checker::Confluence_Checker(
                     mcrl2::lps::specification const& a_lps,
                     mcrl2::data::rewriter::strategy a_rewrite_strategy,
                     int a_time_limit,
                     bool a_path_eliminator,
                     SMT_Solver_Type a_solver_type,
                     bool a_apply_induction,
                     bool a_no_marking,
                     bool a_check_all,
                     bool a_counter_example,
                     bool a_generate_invariants,
                     std::string const& a_dot_file_name):
      f_disjointness_checker(lps::linear_process_to_aterm(a_lps.process())),
      f_invariant_checker(a_lps, a_rewrite_strategy, a_time_limit, a_path_eliminator, a_solver_type, false, false, 0),
      f_bdd_prover(a_lps.data(), a_rewrite_strategy, a_time_limit, a_path_eliminator, a_solver_type, a_apply_induction),
      f_lps(a_lps)
    { if (has_ctau_action(a_lps)) {
        throw mcrl2::runtime_error("An action named \'ctau\' already exists.\n");
      }

      // FIXME
      // f_lps = specification_to_aterm(a_lps);
      // f_lps = a_lps;
      f_no_marking = a_no_marking;
      f_check_all = a_check_all;
      f_counter_example = a_counter_example;
      f_dot_file_name = a_dot_file_name;
      f_generate_invariants = a_generate_invariants;
    }

    // --------------------------------------------------------------------------------------------

    Confluence_Checker::~Confluence_Checker()
    {}

    // --------------------------------------------------------------------------------------------

    ATermAppl Confluence_Checker::check_confluence_and_mark(ATermAppl a_invariant, int a_summand_number) {
      ATermAppl v_process_equation = linear_process_to_aterm(f_lps.process()); //ATAgetArgument(f_lps, 3);
      ATermList v_summands = ATLgetArgument(v_process_equation, 1);
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
          if (is_tau_summand(v_summand)) {
            v_multi_actions_or_delta = ATAgetArgument(v_summand, 2);
            v_multi_actions = ATLgetArgument(v_multi_actions_or_delta, 0);
            gsMessage("tau-summand %2d: ", v_summand_number);
            v_marked_summand = check_confluence_and_mark_summand(a_invariant, v_summand, v_summand_number, v_is_marked);
            gsMessage("\n");
          }
        }
        v_marked_summands = ATinsert(v_marked_summands, (ATerm) v_marked_summand);
        v_summands = ATgetNext(v_summands);
        v_summand_number++;
      }
      v_marked_summands = ATreverse(v_marked_summands);
      v_process_equation = ATsetArgument(v_process_equation, (ATerm) v_marked_summands, 1);
      ATermAppl v_lps = ATsetArgument(specification_to_aterm(f_lps), (ATerm) v_process_equation, 3);

      if (v_is_marked && !has_ctau_action(f_lps)) {
        v_lps = add_ctau_action(v_lps);
      }

      free(f_intermediate);
      f_intermediate = 0;

      return v_lps;
    }
