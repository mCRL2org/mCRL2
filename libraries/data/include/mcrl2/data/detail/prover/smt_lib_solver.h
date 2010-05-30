// Author(s): Luc Engelen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/detail/prover/smt_lib_solver.h
/// \brief Abstract interface for SMT solvers based on the SMT-LIB format

#ifndef SMT_LIB_SOLVER_H
#define SMT_LIB_SOLVER_H

#include <string>

#include "aterm2.h"
#include "mcrl2/core/print.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/core/numeric_string.h"
#include "mcrl2/exception.h"
#include "mcrl2/data/bool.h"
#include "mcrl2/data/detail/prover/smt_solver.h"
#include "mcrl2/data/detail/prover/expression_info.h"

#ifdef HAVE_CVC
#include "mcrl2/data/detail/prover/smt_solver_cvc_fast.ipp"
#endif

#undef ATAgetFirst
#undef ATAgetArgument

namespace mcrl2 {
  namespace data {
    namespace detail {
  /// The class SMT_LIB_Solver is a base class for SMT solvers that read the SMT-LIB format
  /// [Silvio Ranise and Cesare Tinelli. The SMT-LIB Standard: Version 1.1. Technical Report, Department of Computer
  /// Science, The University of Iowa, 2005. (Available at http://goedel.cs.uiowa.edu/smtlib)]. It inherits from the class
  /// SMT_Solver.
  ///
  /// The method SMT_LIB_Solver::translate receives an expression of sort Bool in conjunctive normal form as parameter
  /// a_formula and translates it to a benchmark in SMT-LIB format. The result is saved as field std::string f_benchmark.

class SMT_LIB_Solver: public SMT_Solver {
  private:
    Expression_Info f_expression_info;
    std::string f_sorts_notes;
    std::string f_operators_notes;
    std::string f_predicates_notes;
    std::string f_extrasorts;
    std::string f_operators_extrafuns;
    std::string f_variables_extrafuns;
    std::string f_extrapreds;
    std::string f_formula;
    ATermIndexedSet f_sorts;
    ATermIndexedSet f_operators;
    ATermIndexedSet f_variables;
    ATermIndexedSet f_nat_variables;
    ATermIndexedSet f_pos_variables;
    bool f_bool2pred;

    void declare_sorts(){
      f_extrasorts = "";
      ATermList v_sorts = ATindexedSetElements(f_sorts);
      if (!ATisEmpty(v_sorts)) {
        f_extrasorts = "  :extrasorts (";
        ATermAppl v_sort = 0;
        while (!ATisEmpty(v_sorts)) {
          if (v_sort != 0) {
            f_extrasorts = f_extrasorts + " ";
          }
          v_sort = core::ATAgetFirst(v_sorts);
          v_sorts = ATgetNext(v_sorts);
          int v_sort_number = ATindexedSetGetIndex(f_sorts, (ATerm) v_sort);
          char* v_sort_string = (char*) malloc((core::NrOfChars(v_sort_number) + 5) * sizeof(char));
          sprintf(v_sort_string, "sort%d", v_sort_number);
          f_extrasorts = f_extrasorts + v_sort_string;
          free(v_sort_string);
          v_sort_string = 0;
        }
        f_extrasorts = f_extrasorts + ")\n";
      }
    }

    void declare_operators(){
      f_operators_extrafuns = "";
      ATermList v_operators = ATindexedSetElements(f_operators);
      if (!ATisEmpty(v_operators)) {
        f_operators_extrafuns = "  :extrafuns (";
        while (!ATisEmpty(v_operators)) {
          ATermAppl v_operator = core::ATAgetFirst(v_operators);
          v_operators = ATgetNext(v_operators);
          int v_operator_number = ATindexedSetGetIndex(f_operators, (ATerm) v_operator);
          char* v_operator_string = (char*) malloc((core::NrOfChars(v_operator_number) + 3) * sizeof(char));
          sprintf(v_operator_string, "op%d", v_operator_number);
          f_operators_extrafuns = f_operators_extrafuns + "(" + v_operator_string;
          free(v_operator_string);
          v_operator_string = 0;
          sort_expression v_sort = data_expression(v_operator).sort();
          do {
            ATermList v_sort_domain_list;
            if (is_function_sort(v_sort)) {
              v_sort_domain_list = function_sort(v_sort).domain();
              v_sort = function_sort(v_sort).codomain();
            } else {
              v_sort_domain_list = ATmakeList1((ATerm) static_cast<ATermAppl>(v_sort));
              v_sort = sort_expression();
            }
            for(ATermList l = v_sort_domain_list; !ATisEmpty(l) ; l = ATgetNext(l))
            {
              sort_expression v_sort_domain_elt(core::ATAgetFirst(l));
              if (is_function_sort(v_sort_domain_elt)) {
                throw mcrl2::runtime_error("Function " + pp(v_operator) +
                        " cannot be translated to the SMT-LIB format.");
              }
              if (sort_int::is_int(v_sort_domain_elt)) {
                f_operators_extrafuns = f_operators_extrafuns + " Int";
              } else if (sort_nat::is_nat(v_sort_domain_elt)) {
                f_operators_extrafuns = f_operators_extrafuns + " Int";
              } else if (sort_pos::is_pos(v_sort_domain_elt)) {
                f_operators_extrafuns = f_operators_extrafuns + " Int";
              } else if (sort_real::is_real(v_sort_domain_elt)) {
                f_operators_extrafuns = f_operators_extrafuns + " Real";
              } else {
                int v_sort_number = ATindexedSetPut(f_sorts, (ATerm) static_cast<ATermAppl>(v_sort_domain_elt), 0);
                char* v_sort_string = (char*) malloc((core::NrOfChars(v_sort_number) + 5) * sizeof(char));
                sprintf(v_sort_string, "sort%d", v_sort_number);
                f_operators_extrafuns = f_operators_extrafuns + " " + v_sort_string;
                free(v_sort_string);
                v_sort_string = 0;
              }
            }
          } while (v_sort != sort_expression());
          f_operators_extrafuns = f_operators_extrafuns + ")";
        }
        f_operators_extrafuns = f_operators_extrafuns + ")\n";
      }
    }

    void declare_variables()
    {
      f_variables_extrafuns = "";
      ATermList v_variables = ATindexedSetElements(f_variables);
      if (!ATisEmpty(v_variables)) {
        f_variables_extrafuns = "  :extrafuns (";
        while (!ATisEmpty(v_variables)) {
          ATermAppl v_variable = core::ATAgetFirst(v_variables);
          v_variables = ATgetNext(v_variables);
          char* v_variable_string;
          v_variable_string = core::detail::gsATermAppl2String(core::ATAgetArgument(v_variable, 0));
          sort_expression v_sort = data_expression(v_variable).sort();
          if (sort_real::is_real(v_sort)) {
            f_variables_extrafuns = f_variables_extrafuns + "(" + v_variable_string + " Real)";
          } else if (sort_int::is_int(v_sort)) {
            f_variables_extrafuns = f_variables_extrafuns + "(" + v_variable_string + " Int)";
          } else if (sort_nat::is_nat(v_sort)) {
            f_variables_extrafuns = f_variables_extrafuns + "(" + v_variable_string + " Int)";
          } else if (sort_pos::is_pos(v_sort)) {
            f_variables_extrafuns = f_variables_extrafuns + "(" + v_variable_string + " Int)";
          } else {
            int v_sort_number = ATindexedSetPut(f_sorts, (ATerm) static_cast<ATermAppl>(v_sort), 0);
            char* v_sort_string = (char*) malloc((core::NrOfChars(v_sort_number) + 5) * sizeof(char));
            sprintf(v_sort_string, "sort%d", v_sort_number);
            f_variables_extrafuns = f_variables_extrafuns + "(" + v_variable_string + " " + v_sort_string +")";
            free(v_sort_string);
            v_sort_string = 0;
          }
        }
        f_variables_extrafuns = f_variables_extrafuns + ")\n";
      }
    }

    void declare_predicates(){
      f_extrapreds = "";
      if (f_bool2pred) {
        int v_sort_number;
        char* v_sort_string;

        v_sort_number = ATindexedSetGetIndex(f_sorts, (ATerm) static_cast<ATermAppl>(sort_bool::bool_()));
        v_sort_string = (char*) malloc((core::NrOfChars(v_sort_number) + 5) * sizeof(char));
        sprintf(v_sort_string, "sort%d", v_sort_number);
        f_extrapreds = "  :extrapreds ((bool2pred ";
        f_extrapreds = f_extrapreds + v_sort_string + ")";
        free(v_sort_string);
        v_sort_string = 0;
        f_extrapreds = f_extrapreds + ")\n";
      }
    }

    void produce_notes_for_sorts(){
      f_sorts_notes = "";
      ATermList v_sorts = ATindexedSetElements(f_sorts);
      if (!ATisEmpty(v_sorts)) {
        f_sorts_notes = "  :notes \"";
        while (!ATisEmpty(v_sorts)) {
          sort_expression v_sort(core::ATAgetFirst(v_sorts));
          v_sorts = ATgetNext(v_sorts);
          int v_sort_number = ATindexedSetGetIndex(f_sorts, (ATerm) static_cast<ATermAppl>(v_sort));
          char* v_sort_string = (char*) malloc((core::NrOfChars(v_sort_number) + 5) * sizeof(char));
          sprintf(v_sort_string, "sort%d", v_sort_number);
          const char* v_sort_original_id = basic_sort(v_sort).name().to_string().c_str();
          f_sorts_notes = f_sorts_notes + "(" + v_sort_string + " = " + v_sort_original_id + ")";
          free(v_sort_string);
          v_sort_string = 0;
        }
        f_sorts_notes = f_sorts_notes + "\"\n";
      }
    }

    void produce_notes_for_operators(){
      f_operators_notes = "";
      ATermList v_operators = ATindexedSetElements(f_operators);
      if (!ATisEmpty(v_operators)) {
        f_operators_notes = "  :notes \"";
        while (!ATisEmpty(v_operators)) {
          ATermAppl v_operator = core::ATAgetFirst(v_operators);
          v_operators = ATgetNext(v_operators);
          int v_operator_number = ATindexedSetGetIndex(f_operators, (ATerm) v_operator);
          char* v_operator_string = (char*) malloc((core::NrOfChars(v_operator_number) + 3) * sizeof(char));
          sprintf(v_operator_string, "op%d", v_operator_number);
          char* v_operator_original_id = core::detail::gsATermAppl2String(core::ATAgetArgument(v_operator, 0));
          f_operators_notes = f_operators_notes + "(" + v_operator_string + " = " + v_operator_original_id + ")";
          free(v_operator_string);
          v_operator_string = 0;
        }
        f_operators_notes = f_operators_notes + "\"\n";
      }
    }

    void produce_notes_for_predicates(){
      f_predicates_notes = "";
      if (f_bool2pred) {
        f_predicates_notes =
          "  :notes \"bool2pred was introduced, because the smt-lib format cannot deal\"\n"
          "  :notes \"with boolean variables or functions returning boolean values.\"\n";
      }
    }

    void translate_clause(ATermAppl a_clause, bool a_expecting_predicate){
      if (sort_bool::is_not_application(data_expression(a_clause))) {
        translate_not(a_clause);
      } else if (is_equal_to_application(data_expression(a_clause))) {
        translate_equality(a_clause);
      } else if (is_not_equal_to_application(data_expression(a_clause))) {
        translate_inequality(a_clause);
      } else if (is_greater_application(data_expression(a_clause))) {
        translate_greater_than(a_clause);
      } else if (is_greater_equal_application(data_expression(a_clause))) {
        translate_greater_than_or_equal(a_clause);
      } else if (is_less_application(data_expression(a_clause))) {
        translate_less_than(a_clause);
      } else if (is_less_equal_application(data_expression(a_clause))) {
        translate_less_than_or_equal(a_clause);
      } else if (sort_real::is_plus_application(data_expression(a_clause))) {
        translate_plus(a_clause);
      } else if (sort_real::is_negate_application(data_expression(a_clause))) {
        translate_unary_minus(a_clause);
      } else if (sort_real::is_minus_application(data_expression(a_clause))) {
        translate_binary_minus(a_clause);
      } else if (sort_real::is_times_application(data_expression(a_clause))) {
        translate_multiplication(a_clause);
      } else if (sort_real::is_maximum_application(data_expression(a_clause))) {
        translate_max(a_clause);
      } else if (sort_real::is_minimum_application(data_expression(a_clause))) {
        translate_min(a_clause);
      } else if (sort_real::is_abs_application(data_expression(a_clause))) {
        translate_abs(a_clause);
      } else if (sort_real::is_succ_application(data_expression(a_clause))) {
        translate_succ(a_clause);
      } else if (sort_real::is_pred_application(data_expression(a_clause))) {
        translate_pred(a_clause);
      } else if (sort_pos::is_add_with_carry_application(data_expression(a_clause))) {
        translate_add_c(a_clause);
      } else if (sort_nat::is_cnat_application(data_expression(a_clause))) {
        translate_c_nat(a_clause);
      } else if (sort_int::is_cint_application(data_expression(a_clause))) {
        translate_c_int(a_clause);
      //} else if (gsIsDataExprCReal(data_expression(a_clause))) {
      //  translate_c_real(a_clause);
      } else if (sort_int::is_integer_constant(data_expression(a_clause))) {
        translate_int_constant(a_clause);
      } else if (sort_nat::is_natural_constant(data_expression(a_clause))) {
        translate_nat_constant(a_clause);
      } else if (sort_pos::is_positive_constant(data_expression(a_clause))) {
        translate_pos_constant(a_clause);
      } else if (sort_bool::is_true_function_symbol(data_expression(a_clause)) && a_expecting_predicate) {
        translate_true();
      } else if (sort_bool::is_false_function_symbol(data_expression(a_clause)) && a_expecting_predicate) {
        translate_false();
      } else if (core::detail::gsIsDataVarId(a_clause)) {
        if (a_expecting_predicate) {
          add_bool2pred_and_translate_clause(a_clause);
        } else if (sort_nat::is_nat(data_expression(a_clause).sort())) {
          translate_nat_variable(a_clause);
        } else if (sort_pos::is_pos(data_expression(a_clause).sort())) {
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
      } else if (core::detail::gsIsOpId(a_clause)) {
        translate_constant(a_clause);
      } else {
        throw mcrl2::runtime_error("Unable to handle the current clause (" +
                pp(a_clause) + ").");
      }
    }

    void add_bool2pred_and_translate_clause(ATermAppl a_clause){
      f_bool2pred = true;
      f_formula = f_formula + "(bool2pred ";
      translate_clause(a_clause, false);
      f_formula = f_formula + ")";
    }

    void translate_not(ATermAppl a_clause){
      ATermAppl v_clause;

      v_clause = f_expression_info.get_argument(a_clause, 0);
      f_formula = f_formula + "(not ";
      translate_clause(v_clause, true);
      f_formula = f_formula + ")";
    }

    void translate_equality(ATermAppl a_clause){
      ATermAppl v_clause_1, v_clause_2;

      v_clause_1 = f_expression_info.get_argument(a_clause, 0);
      v_clause_2 = f_expression_info.get_argument(a_clause, 1);
      f_formula = f_formula + "(= ";
      translate_clause(v_clause_1, false);
      f_formula = f_formula + " ";
      translate_clause(v_clause_2, false);
      f_formula = f_formula + ")";
    }

    void translate_inequality(ATermAppl a_clause){
      ATermAppl v_clause_1, v_clause_2;

      v_clause_1 = f_expression_info.get_argument(a_clause, 0);
      v_clause_2 = f_expression_info.get_argument(a_clause, 1);
      f_formula = f_formula + "(distinct ";
      translate_clause(v_clause_1, false);
      f_formula = f_formula + " ";
      translate_clause(v_clause_2, false);
      f_formula = f_formula + ")";
    }

    void translate_greater_than(ATermAppl a_clause){
      ATermAppl v_clause_1, v_clause_2;

      v_clause_1 = f_expression_info.get_argument(a_clause, 0);
      v_clause_2 = f_expression_info.get_argument(a_clause, 1);
      f_formula = f_formula + "(> ";
      translate_clause(v_clause_1, false);
      f_formula = f_formula + " ";
      translate_clause(v_clause_2, false);
      f_formula = f_formula + ")";
    }

    void translate_greater_than_or_equal(ATermAppl a_clause){
      ATermAppl v_clause_1, v_clause_2;

      v_clause_1 = f_expression_info.get_argument(a_clause, 0);
      v_clause_2 = f_expression_info.get_argument(a_clause, 1);
      f_formula = f_formula + "(>= ";
      translate_clause(v_clause_1, false);
      f_formula = f_formula + " ";
      translate_clause(v_clause_2, false);
      f_formula = f_formula + ")";
    }

    void translate_less_than(ATermAppl a_clause){
      ATermAppl v_clause_1, v_clause_2;

      v_clause_1 = f_expression_info.get_argument(a_clause, 0);
      v_clause_2 = f_expression_info.get_argument(a_clause, 1);
      f_formula = f_formula + "(< ";
      translate_clause(v_clause_1, false);
      f_formula = f_formula + " ";
      translate_clause(v_clause_2, false);
      f_formula = f_formula + ")";
    }

    void translate_less_than_or_equal(ATermAppl a_clause){
      ATermAppl v_clause_1, v_clause_2;

      v_clause_1 = f_expression_info.get_argument(a_clause, 0);
      v_clause_2 = f_expression_info.get_argument(a_clause, 1);
      f_formula = f_formula + "(<= ";
      translate_clause(v_clause_1, false);
      f_formula = f_formula + " ";
      translate_clause(v_clause_2, false);
      f_formula = f_formula + ")";
    }

    void translate_plus(ATermAppl a_clause){
      ATermAppl v_clause_1, v_clause_2;

      v_clause_1 = f_expression_info.get_argument(a_clause, 0);
      v_clause_2 = f_expression_info.get_argument(a_clause, 1);
      f_formula = f_formula + "(+ ";
      translate_clause(v_clause_1, false);
      f_formula = f_formula + " ";
      translate_clause(v_clause_2, false);
      f_formula = f_formula + ")";
    }

    void translate_unary_minus(ATermAppl a_clause){
      ATermAppl v_clause;

      v_clause = f_expression_info.get_argument(a_clause, 0);
      f_formula = f_formula + "(~";
      translate_clause(v_clause, false);
      f_formula = f_formula + ")";
    }

    void translate_binary_minus(ATermAppl a_clause) {
      ATermAppl v_clause_1, v_clause_2;

      v_clause_1 = f_expression_info.get_argument(a_clause, 0);
      v_clause_2 = f_expression_info.get_argument(a_clause, 1);
      f_formula = f_formula + "(- ";
      translate_clause(v_clause_1, false);
      f_formula = f_formula + " ";
      translate_clause(v_clause_2, false);
      f_formula = f_formula + ")";
    }

    void translate_multiplication(ATermAppl a_clause) {
      ATermAppl v_clause_1, v_clause_2;

      v_clause_1 = f_expression_info.get_argument(a_clause, 0);
      v_clause_2 = f_expression_info.get_argument(a_clause, 1);
      f_formula = f_formula + "(* ";
      translate_clause(v_clause_1, false);
      f_formula = f_formula + " ";
      translate_clause(v_clause_2, false);
      f_formula = f_formula + ")";
    }

    void translate_max(ATermAppl a_clause) {
      ATermAppl v_clause_1, v_clause_2;

      v_clause_1 = f_expression_info.get_argument(a_clause, 0);
      v_clause_2 = f_expression_info.get_argument(a_clause, 1);
      f_formula = f_formula + "(ite (>= ";
      translate_clause(v_clause_1, false);
      f_formula = f_formula + " ";
      translate_clause(v_clause_2, false);
      f_formula = f_formula + ") ";
      translate_clause(v_clause_1, false);
      f_formula = f_formula + " ";
      translate_clause(v_clause_2, false);
      f_formula = f_formula + ")";
    }

    void translate_min(ATermAppl a_clause){
      ATermAppl v_clause_1, v_clause_2;

      v_clause_1 = f_expression_info.get_argument(a_clause, 0);
      v_clause_2 = f_expression_info.get_argument(a_clause, 1);
      f_formula = f_formula + "(ite (<= ";
      translate_clause(v_clause_1, false);
      f_formula = f_formula + " ";
      translate_clause(v_clause_2, false);
      f_formula = f_formula + ") ";
      translate_clause(v_clause_1, false);
      f_formula = f_formula + " ";
      translate_clause(v_clause_2, false);
      f_formula = f_formula + ")";
    }

    void translate_abs(ATermAppl a_clause) {
      ATermAppl v_clause;

      v_clause = f_expression_info.get_argument(a_clause, 0);
      f_formula = f_formula + "(ite (< 0 ";
      translate_clause(v_clause, false);
      f_formula = f_formula + ") ~";
      translate_clause(v_clause, false);
      f_formula = f_formula + " ";
      translate_clause(v_clause, false);
      f_formula = f_formula + ")";
    }

    void translate_succ(ATermAppl a_clause) {
      ATermAppl v_clause;

      v_clause = f_expression_info.get_argument(a_clause, 0);
      f_formula = f_formula + "(+ ";
      translate_clause(v_clause, false);
      f_formula = f_formula + " 1)";
    }

    void translate_pred(ATermAppl a_clause){
      ATermAppl v_clause;

      v_clause = f_expression_info.get_argument(a_clause, 0);
      f_formula = f_formula + "(- ";
      translate_clause(v_clause, false);
      f_formula = f_formula + " 1)";
    }

    void translate_add_c(ATermAppl a_clause) {
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

    void translate_c_nat(ATermAppl a_clause) {
      ATermAppl v_clause;

      v_clause = f_expression_info.get_argument(a_clause, 0);
      translate_clause(v_clause, false);
    }

    void translate_c_int(ATermAppl a_clause) {
      ATermAppl v_clause;

      v_clause = f_expression_info.get_argument(a_clause, 0);
      translate_clause(v_clause, false);
    }

    //void translate_c_real(ATermAppl a_clause);

    void translate_unknown_operator(ATermAppl a_clause) {
      int v_operator_number;
      ATermAppl v_operator;
      char* v_operator_string;

      v_operator = f_expression_info.get_operator(a_clause);
      v_operator_number = ATindexedSetPut(f_operators, (ATerm) v_operator, 0);

      v_operator_string = (char*) malloc((core::NrOfChars(v_operator_number) + 3) * sizeof(char));
      sprintf(v_operator_string, "op%d", v_operator_number);
      f_formula = f_formula + "(" + v_operator_string;
      free(v_operator_string);
      v_operator_string = 0;
      if(data::is_application(a_clause))
      {
        data::application a = data::application(data::data_expression(a_clause));
        for(data_expression_list::iterator i = a.arguments().begin(); i != a.arguments().end(); ++i)
        {
          f_formula = f_formula + " ";
          translate_clause(*i, false);
        }
      }
      f_formula = f_formula + ")";
    }

    void translate_variable(ATermAppl a_clause) {
      char* v_string;

      v_string = core::detail::gsATermAppl2String(core::ATAgetArgument(a_clause, 0));
      f_formula = f_formula + v_string;

      ATindexedSetPut(f_variables, (ATerm) a_clause, 0);
    }

    void translate_nat_variable(ATermAppl a_clause) {
      char* v_string;

      v_string = core::detail::gsATermAppl2String(core::ATAgetArgument(a_clause, 0));
      f_formula = f_formula + v_string;

      ATindexedSetPut(f_variables, (ATerm) a_clause, 0);
      ATindexedSetPut(f_nat_variables, (ATerm) a_clause, 0);
    }

    void translate_pos_variable(ATermAppl a_clause){
      char* v_string;

      v_string = core::detail::gsATermAppl2String(core::ATAgetArgument(a_clause, 0));
      f_formula = f_formula + v_string;

      ATindexedSetPut(f_variables, (ATerm) a_clause, 0);
      ATindexedSetPut(f_pos_variables, (ATerm) a_clause, 0);
    }

    void translate_int_constant(ATermAppl a_clause) {
      std::string v_value(data::sort_int::integer_constant_as_string(data::data_expression(a_clause)));
      if (v_value[0] == '-') {
        v_value[0] = '~';
        f_formula = f_formula + "(" + v_value + ")";
      } else {
        f_formula = f_formula + v_value;
      }
    }

    void translate_nat_constant(ATermAppl a_clause) {
      std::string v_value(data::sort_nat::natural_constant_as_string(data::data_expression(a_clause)));
      f_formula = f_formula + v_value;
    }

    void translate_pos_constant(ATermAppl a_clause) {
      std::string v_value(data::sort_pos::positive_constant_as_string(data::data_expression(a_clause)));
      f_formula = f_formula + v_value;
    }

    void translate_true(){
      f_formula = f_formula + "true";
    }

    void translate_false() {
      f_formula = f_formula + "false";
    }

    void translate_constant(ATermAppl a_clause) {
      int v_operator_number;
      ATermAppl v_operator;
      char* v_operator_string;

      v_operator = f_expression_info.get_operator(a_clause);
      v_operator_number = ATindexedSetPut(f_operators, (ATerm) v_operator, 0);

      v_operator_string = (char*) malloc((core::NrOfChars(v_operator_number) + 3) * sizeof(char));
      sprintf(v_operator_string, "op%d", v_operator_number);
      f_formula = f_formula + v_operator_string;
      free(v_operator_string);
      v_operator_string = 0;
    }

    void add_nat_clauses(){
      ATermList v_variables = ATindexedSetElements(f_nat_variables);
      if (!ATisEmpty(v_variables)) {
        while (!ATisEmpty(v_variables)) {
          ATermAppl v_variable = core::ATAgetFirst(v_variables);
          v_variables = ATgetNext(v_variables);
          char* v_variable_string = core::detail::gsATermAppl2String(core::ATAgetArgument(v_variable, 0));
          f_formula = f_formula + " (>= " + v_variable_string + " 0)";
        }
      }
    }

    void add_pos_clauses() {
      ATermList v_variables = ATindexedSetElements(f_pos_variables);
      if (!ATisEmpty(v_variables)) {
        while (!ATisEmpty(v_variables)) {
          ATermAppl v_variable = core::ATAgetFirst(v_variables);
          v_variables = ATgetNext(v_variables);
          char* v_variable_string = core::detail::gsATermAppl2String(core::ATAgetArgument(v_variable, 0));
          f_formula = f_formula + " (>= " + v_variable_string + " 1)";
        }
      }
    }

  protected:
    std::string f_benchmark;

    /// precondition: The argument passed as parameter a_formula is a list of expressions of sort Bool in internal mCRL2
    /// format. The argument represents a formula in conjunctive normal form, where the elements of the list represent the
    /// clauses
    void translate(ATermList a_formula) {
      ATermAppl v_clause;

      ATindexedSetReset(f_sorts);
      ATindexedSetReset(f_operators);
      ATindexedSetReset(f_variables);
      ATindexedSetReset(f_nat_variables);
      ATindexedSetReset(f_pos_variables);
      f_bool2pred = false;

      f_formula = "  :formula (and";
      core::gsVerboseMsg("Formula to be solved: %P\n", a_formula);
      while (!ATisEmpty(a_formula)) {
        v_clause = core::ATAgetFirst(a_formula);
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
      core::gsVerboseMsg("Corresponding benchmark:\n%s", f_benchmark.c_str());
    }


  public:
    SMT_LIB_Solver() {
      f_sorts = ATindexedSetCreate(100, 75);
      f_operators = ATindexedSetCreate(100, 75);
      f_variables = ATindexedSetCreate(100, 75);
      f_nat_variables = ATindexedSetCreate(100, 75);
      f_pos_variables = ATindexedSetCreate(100, 75);
    }

    virtual ~SMT_LIB_Solver(){
      ATindexedSetDestroy(f_sorts);
      ATindexedSetDestroy(f_operators);
      ATindexedSetDestroy(f_variables);
      ATindexedSetDestroy(f_nat_variables);
      ATindexedSetDestroy(f_pos_variables);
    }
};

      namespace prover {

        /**
         * Template class for SMT provers that come as an external binary and use the
         * SMT-lib format. Input to the tool is specified on standard input,
         * output is read from standard output and matches one of the strings:
         * "sat", "unsat", "unknown".
         *
         * Parameter T follows the curiously recurring template pattern (CRTP). Type T
         * is required to have the name and exec methods as in the example below.
         *
         *  \code
         *  class cvc_smt_solver : public binary_smt_solver< cvc_smt_solver > {
         *    inline static char* name() {
         *      return "CVC3";
         *    }
         *
         *    inline static void exec() {
         *      ::execlp("cvc", "cvc3", "-lang smt-lib", 0);
         *    }
         *  };
         *  \endcode
         **/
        template < typename T >
        class binary_smt_solver {

          protected:

            // \brief Calls one of the exec functions
            static bool execute(std::string const& benchmark);

          public:

            // \brief Checks the availability/usability of the prover
            static bool usable();
        };
#if !(defined(_MSC_VER) || defined(__MINGW32__) || defined(__CYGWIN__))
# include "mcrl2/core/messaging.h"
# include <unistd.h>
      /// The class inherits from the class SMT_LIB_Solver. It uses the SMT solver
      /// CVC / (http://www.cs.nyu.edu/acsys/cvcl/) to determine the satisfiability
      /// of propositional formulas. To use the solver CVC / the directory containing
      /// the corresponding executable must be in the path.
      ///
      /// The static method usable can be used to check checks if CVC's executable is indeed available.
      ///
      /// The method SMT_Solver_CVC::is_satisfiable receives a formula in conjunctive normal form as parameter a_formula and
      /// indicates whether or not this formula is satisfiable.
      class cvc_smt_solver : public SMT_LIB_Solver, public binary_smt_solver< cvc_smt_solver > {
        friend class binary_smt_solver< cvc_smt_solver >;

        private:

          inline static char const* name() {
            return "CVC3";
          }

          inline static void exec() {
            ::execlp("cvc3", "cvc3", "-lang", "smt-lib", (char*)0);
          }

        public:

          /// precondition: The argument passed as parameter a_formula is a list of expressions of sort Bool in internal mCRL2
          /// format. The argument represents a formula in conjunctive normal form, where the elements of the list represent the
          /// clauses
          bool is_satisfiable(ATermList a_formula) {
            translate(a_formula);

            return execute(f_benchmark);
          }
      };

      /// The class inherits from the class SMT_LIB_Solver. It uses the SMT solver Ario 1.1
      /// (http://www.eecs.umich.edu/~ario/) to determine the satisfiability of propositional formulas. To use the solver Ario
      /// 1.1, the directory containing the corresponding executable must be in the path.
      ///
      /// The static method usable can be used to check checks if CVC's executable is indeed available.
      ///
      /// The method is_satisfiable receives a formula in conjunctive normal form as parameter a_formula and
      /// indicates whether or not this formula is satisfiable.
      class ario_smt_solver : public SMT_LIB_Solver, public binary_smt_solver< ario_smt_solver > {
        friend class binary_smt_solver< ario_smt_solver >;

        private:

          inline static char const* name() {
            return "Ario";
          }

          inline static void exec() {
            ::execlp("ario", "ario", (char*)0);
          }

        public:

          /// precondition: The argument passed as parameter a_formula is a list of expressions of sort Bool in internal mCRL2
          /// format. The argument represents a formula in conjunctive normal form, where the elements of the list represent the
          /// clauses
          bool is_satisfiable(ATermList a_formula) {
            translate(a_formula);

            return execute(f_benchmark);
          }
      };
#endif
     } // prover

    } // detail
  } // data
} // mcrl2

#endif
