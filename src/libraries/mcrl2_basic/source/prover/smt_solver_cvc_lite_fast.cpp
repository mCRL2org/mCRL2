// Implementation of class SMT_Solver_CVC_Lite_Fast
// file: smt_solver_cvc_lite_fast.cpp

#ifdef CVC_LITE_LIB

#include "smt_solver_cvc_lite_fast.h"
#include "libprint_c.h"
#include "libstruct.h"
#include "utilities.h"

// Class Sort_And_Type ----------------------------------------------------------------------------
  // Class Sort_And_Type - Functions declared public ----------------------------------------------

    template <class Type>
    Pair<Type>::Pair(ATermAppl a_aterm, Type a_element) {
      f_aterm = a_aterm;
      f_element = a_element;
    }

    // --------------------------------------------------------------------------------------------

    template <class Type>
    ATermAppl Pair<Type>::get_aterm() {
      return f_aterm;
    }

    // --------------------------------------------------------------------------------------------

    template <class Type>
    Type Pair<Type>::get_element() {
      return f_element;
    }

// Class List_Of_Types ----------------------------------------------------------------------------
  // Class List_Of_Types - Functions declared public ----------------------------------------------

    template <class Type>
    void List_Of_Pairs<Type>::add_pair_to_list(ATermAppl a_aterm, Type a_element) {
      Pair<Type> v_pair(a_aterm, a_element);
      f_list_of_pairs.push_back(v_pair);
    }

    // --------------------------------------------------------------------------------------------

    template <class Type>
    bool List_Of_Pairs<Type>::element_in_list(ATermAppl a_aterm) {
      for (unsigned int i = 0; i < f_list_of_pairs.size(); ++i) {
        if (f_list_of_pairs[i].get_aterm() == a_aterm) {
          return true;
        }
      }
      return false;
    }

    // --------------------------------------------------------------------------------------------

    template <class Type>
    Type List_Of_Pairs<Type>::get_element(ATermAppl a_aterm) {
      Type v_dummy;

      for (unsigned int i = 0; i < f_list_of_pairs.size(); ++i) {
        if (f_list_of_pairs[i].get_aterm() == a_aterm) {
          return f_list_of_pairs[i].get_element();
        }
      }
      return v_dummy;
    }

// Class SMT_Solver_CVC_Lite_Fast -----------------------------------------------------------------
  // Class SMT_Solver_CVC_Lite_Fast - Functions declared private ----------------------------------

    CVCL::Type SMT_Solver_CVC_Lite_Fast::translate_sort(ATermAppl a_sort_expression) {
      ATermAppl v_type;
      ATermAppl v_result_type;
      CVCL::Type v_translated_type;
      CVCL::Type v_translated_result_type;
      std::string v_sort_id;
      std::vector<CVCL::Type> v_translated_domain;
      int v_number_of_arguments;

      if (f_list_of_types.element_in_list(a_sort_expression)) {
        v_translated_type = f_list_of_types.get_element(a_sort_expression);
        return v_translated_type;
      } else {
        if (f_sort_info.is_sort_arrow(a_sort_expression)) {
          v_number_of_arguments = f_sort_info.get_number_of_arguments(a_sort_expression);
          for (int i = 0; i < v_number_of_arguments; ++i) {
            v_type = f_sort_info.get_type_of_argument(a_sort_expression, i);
            if (f_sort_info.is_sort_bool(v_type)) {
              gsErrorMsg("CVC Lite cannot deal with functions that have boolean arguments.\n");
              exit(1);
            }
            v_translated_type = translate_sort(v_type);
            v_translated_domain.push_back(v_translated_type);
          }
          v_result_type = f_sort_info.get_result_type(a_sort_expression);
          v_translated_result_type = translate_sort(v_result_type);
          v_translated_type = f_validity_checker->funType(v_translated_domain, v_translated_result_type);
          f_list_of_types.add_pair_to_list(a_sort_expression, v_translated_type);
          return v_translated_type;
        } else if (f_sort_info.is_sort_int(a_sort_expression)) {
          return f_validity_checker->intType();
        } else if (f_sort_info.is_sort_nat(a_sort_expression)) {
          return f_validity_checker->intType();
        } else if (f_sort_info.is_sort_pos(a_sort_expression)) {
          return f_validity_checker->intType();
        } else if (f_sort_info.is_sort_real(a_sort_expression)) {
          return f_validity_checker->realType();
        } else if (f_sort_info.is_sort_bool(a_sort_expression)) {
          return f_validity_checker->boolType();
        } else if (f_sort_info.is_sort_id(a_sort_expression)) {
          if (f_list_of_types.element_in_list(a_sort_expression)) {
            v_translated_type = f_list_of_types.get_element(a_sort_expression);
          } else {
            v_sort_id = f_sort_info.get_sort_id(a_sort_expression);
            v_translated_type = f_validity_checker->createType(v_sort_id);
            f_list_of_types.add_pair_to_list(a_sort_expression, v_translated_type);
          }
          return v_translated_type;
        } else {
          gsErrorMsg("Unable to handle the current sort expression (%T).\n", a_sort_expression);
          exit(1);
        }
      }
    }

    // --------------------------------------------------------------------------------------------

    CVCL::Expr SMT_Solver_CVC_Lite_Fast::translate_clause(ATermAppl a_clause) {
      if (f_expression_info.is_not(a_clause)) {
        return translate_not(a_clause);
      } else if (f_expression_info.is_equality(a_clause)) {
        return translate_equality(a_clause);
      } else if (f_expression_info.is_inequality(a_clause)) {
        return translate_inequality(a_clause);
      } else if (f_expression_info.is_greater_than(a_clause)) {
        return translate_greater_than(a_clause);
      } else if (f_expression_info.is_greater_than_or_equal(a_clause)) {
        return translate_greater_than_or_equal(a_clause);
      } else if (f_expression_info.is_less_than(a_clause)) {
        return translate_less_than(a_clause);
      } else if (f_expression_info.is_less_than_or_equal(a_clause)) {
        return translate_less_than_or_equal(a_clause);
      } else if (f_expression_info.is_plus(a_clause)) {
        return translate_plus(a_clause);
      } else if (f_expression_info.is_unary_minus(a_clause)) {
        return translate_unary_minus(a_clause);
      } else if (f_expression_info.is_binary_minus(a_clause)) {
        return translate_binary_minus(a_clause);
      } else if (f_expression_info.is_multiplication(a_clause)) {
        return translate_multiplication(a_clause);
      } else if (f_expression_info.is_max(a_clause)) {
        return translate_max(a_clause);
      } else if (f_expression_info.is_min(a_clause)) {
        return translate_min(a_clause);
      } else if (f_expression_info.is_abs(a_clause)) {
        return translate_abs(a_clause);
      } else if (f_expression_info.is_succ(a_clause)) {
        return translate_succ(a_clause);
      } else if (f_expression_info.is_pred(a_clause)) {
        return translate_pred(a_clause);
      } else if (f_expression_info.is_add_c(a_clause)) {
        return translate_add_c(a_clause);
      } else if (f_expression_info.is_c_nat(a_clause)) {
        return translate_c_nat(a_clause);
      } else if (f_expression_info.is_c_int(a_clause)) {
        return translate_c_int(a_clause);
      } else if (f_expression_info.is_c_real(a_clause)) {
        return translate_c_real(a_clause);
      } else if (f_expression_info.is_int_constant(a_clause)) {
        return translate_int_constant(a_clause);
      } else if (f_expression_info.is_nat_constant(a_clause)) {
        return translate_nat_constant(a_clause);
      } else if (f_expression_info.is_pos_constant(a_clause)) {
        return translate_pos_constant(a_clause);
      } else if (f_expression_info.is_true(a_clause)) {
        return translate_true();
      } else if (f_expression_info.is_false(a_clause)) {
        return translate_false();
      } else if (f_expression_info.is_variable(a_clause)) {
        if (f_sort_info.is_sort_nat(f_expression_info.get_sort_of_variable(a_clause))) {
          return translate_nat_variable(a_clause);
        } else if (f_sort_info.is_sort_pos(f_expression_info.get_sort_of_variable(a_clause))) {
          return translate_pos_variable(a_clause);
        } else {
          return translate_variable(a_clause);
        }
      } else if (f_expression_info.is_operator(a_clause)) {
        return translate_unknown_operator(a_clause);
      } else if (f_expression_info.is_constant(a_clause)) {
        return translate_constant(a_clause);
      } else {
        gsErrorMsg("Unable to handle the current clause (%T).\n", a_clause);
        exit(1);
      }
    }

    // --------------------------------------------------------------------------------------------

    CVCL::Expr SMT_Solver_CVC_Lite_Fast::translate_not(ATermAppl a_clause) {
      ATermAppl v_clause;
      CVCL::Expr v_expression;

      v_clause = f_expression_info.get_argument(a_clause, 0);
      v_expression = translate_clause(v_clause);
      return f_validity_checker->notExpr(v_expression);
    }

    // --------------------------------------------------------------------------------------------

    CVCL::Expr SMT_Solver_CVC_Lite_Fast::translate_equality(ATermAppl a_clause) {
      ATermAppl v_clause_1, v_clause_2;
      CVCL::Expr v_expression_1, v_expression_2;

      v_clause_1 = f_expression_info.get_argument(a_clause, 0);
      v_clause_2 = f_expression_info.get_argument(a_clause, 1);
      v_expression_1 = translate_clause(v_clause_1);
      v_expression_2 = translate_clause(v_clause_2);
      return f_validity_checker->eqExpr(v_expression_1, v_expression_2);
    }

    // --------------------------------------------------------------------------------------------

    CVCL::Expr SMT_Solver_CVC_Lite_Fast::translate_inequality(ATermAppl a_clause) {
      ATermAppl v_clause_1, v_clause_2;
      CVCL::Expr v_expression_1, v_expression_2;

      v_clause_1 = f_expression_info.get_argument(a_clause, 0);
      v_clause_2 = f_expression_info.get_argument(a_clause, 1);
      v_expression_1 = translate_clause(v_clause_1);
      v_expression_2 = translate_clause(v_clause_2);
      return f_validity_checker->notExpr(f_validity_checker->eqExpr(v_expression_1, v_expression_2));
    }

    // --------------------------------------------------------------------------------------------

    CVCL::Expr SMT_Solver_CVC_Lite_Fast::translate_greater_than(ATermAppl a_clause) {
      ATermAppl v_clause_1, v_clause_2;
      CVCL::Expr v_expression_1, v_expression_2;

      v_clause_1 = f_expression_info.get_argument(a_clause, 0);
      v_clause_2 = f_expression_info.get_argument(a_clause, 1);
      v_expression_1 = translate_clause(v_clause_1);
      v_expression_2 = translate_clause(v_clause_2);
      return f_validity_checker->gtExpr(v_expression_1, v_expression_2);
    }

    // --------------------------------------------------------------------------------------------

    CVCL::Expr SMT_Solver_CVC_Lite_Fast::translate_greater_than_or_equal(ATermAppl a_clause) {
      ATermAppl v_clause_1, v_clause_2;
      CVCL::Expr v_expression_1, v_expression_2;

      v_clause_1 = f_expression_info.get_argument(a_clause, 0);
      v_clause_2 = f_expression_info.get_argument(a_clause, 1);
      v_expression_1 = translate_clause(v_clause_1);
      v_expression_2 = translate_clause(v_clause_2);
      return f_validity_checker->geExpr(v_expression_1, v_expression_2);
    }

    // --------------------------------------------------------------------------------------------

    CVCL::Expr SMT_Solver_CVC_Lite_Fast::translate_less_than(ATermAppl a_clause) {
      ATermAppl v_clause_1, v_clause_2;
      CVCL::Expr v_expression_1, v_expression_2;

      v_clause_1 = f_expression_info.get_argument(a_clause, 0);
      v_clause_2 = f_expression_info.get_argument(a_clause, 1);
      v_expression_1 = translate_clause(v_clause_1);
      v_expression_2 = translate_clause(v_clause_2);
      return f_validity_checker->ltExpr(v_expression_1, v_expression_2);
    }

    // --------------------------------------------------------------------------------------------

    CVCL::Expr SMT_Solver_CVC_Lite_Fast::translate_less_than_or_equal(ATermAppl a_clause) {
      ATermAppl v_clause_1, v_clause_2;
      CVCL::Expr v_expression_1, v_expression_2;

      v_clause_1 = f_expression_info.get_argument(a_clause, 0);
      v_clause_2 = f_expression_info.get_argument(a_clause, 1);
      v_expression_1 = translate_clause(v_clause_1);
      v_expression_2 = translate_clause(v_clause_2);
      return f_validity_checker->leExpr(v_expression_1, v_expression_2);
    }

    // --------------------------------------------------------------------------------------------

    CVCL::Expr SMT_Solver_CVC_Lite_Fast::translate_plus(ATermAppl a_clause) {
      ATermAppl v_clause_1, v_clause_2;
      CVCL::Expr v_expression_1, v_expression_2;

      v_clause_1 = f_expression_info.get_argument(a_clause, 0);
      v_clause_2 = f_expression_info.get_argument(a_clause, 1);
      v_expression_1 = translate_clause(v_clause_1);
      v_expression_2 = translate_clause(v_clause_2);
      return f_validity_checker->plusExpr(v_expression_1, v_expression_2);
    }

    // --------------------------------------------------------------------------------------------

    CVCL::Expr SMT_Solver_CVC_Lite_Fast::translate_unary_minus(ATermAppl a_clause) {
      ATermAppl v_clause;
      CVCL::Expr v_expression;

      v_clause = f_expression_info.get_argument(a_clause, 0);
      v_expression = translate_clause(v_clause);
      return f_validity_checker->uminusExpr(v_expression);
    }

    // --------------------------------------------------------------------------------------------

    CVCL::Expr SMT_Solver_CVC_Lite_Fast::translate_binary_minus(ATermAppl a_clause) {
      ATermAppl v_clause_1, v_clause_2;
      CVCL::Expr v_expression_1, v_expression_2;

      v_clause_1 = f_expression_info.get_argument(a_clause, 0);
      v_clause_2 = f_expression_info.get_argument(a_clause, 1);
      v_expression_1 = translate_clause(v_clause_1);
      v_expression_2 = translate_clause(v_clause_2);
      return f_validity_checker->minusExpr(v_expression_1, v_expression_2);
    }

    // --------------------------------------------------------------------------------------------

    CVCL::Expr SMT_Solver_CVC_Lite_Fast::translate_multiplication(ATermAppl a_clause) {
      ATermAppl v_clause_1, v_clause_2;
      CVCL::Expr v_expression_1, v_expression_2;

      v_clause_1 = f_expression_info.get_argument(a_clause, 0);
      v_clause_2 = f_expression_info.get_argument(a_clause, 1);
      v_expression_1 = translate_clause(v_clause_1);
      v_expression_2 = translate_clause(v_clause_2);
      return f_validity_checker->multExpr(v_expression_1, v_expression_2);
    }

    // --------------------------------------------------------------------------------------------

    CVCL::Expr SMT_Solver_CVC_Lite_Fast::translate_max(ATermAppl a_clause) {
      ATermAppl v_clause_1, v_clause_2;
      CVCL::Expr v_expression_1, v_expression_2;
      CVCL::Expr v_guard;

      v_clause_1 = f_expression_info.get_argument(a_clause, 0);
      v_clause_2 = f_expression_info.get_argument(a_clause, 1);
      v_expression_1 = translate_clause(v_clause_1);
      v_expression_2 = translate_clause(v_clause_2);
      v_guard = f_validity_checker->geExpr(v_expression_1, v_expression_2);

      return f_validity_checker->iteExpr(v_guard, v_expression_1, v_expression_2);
    }

    // --------------------------------------------------------------------------------------------

    CVCL::Expr SMT_Solver_CVC_Lite_Fast::translate_min(ATermAppl a_clause) {
      ATermAppl v_clause_1, v_clause_2;
      CVCL::Expr v_expression_1, v_expression_2;
      CVCL::Expr v_guard;

      v_clause_1 = f_expression_info.get_argument(a_clause, 0);
      v_clause_2 = f_expression_info.get_argument(a_clause, 1);
      v_expression_1 = translate_clause(v_clause_1);
      v_expression_2 = translate_clause(v_clause_2);
      v_guard = f_validity_checker->leExpr(v_expression_1, v_expression_2);

      return f_validity_checker->iteExpr(v_guard, v_expression_1, v_expression_2);
    }

    // --------------------------------------------------------------------------------------------

    CVCL::Expr SMT_Solver_CVC_Lite_Fast::translate_abs(ATermAppl a_clause) {
      ATermAppl v_clause;
      CVCL::Expr v_expression;
      CVCL::Expr v_guard;
      CVCL::Expr v_zero;
      CVCL::Expr v_minus_expression;

      v_clause = f_expression_info.get_argument(a_clause, 0);
      v_expression = translate_clause(v_clause);
      v_zero = f_validity_checker->ratExpr(0);
      v_guard = f_validity_checker->geExpr(v_expression, v_zero);
      v_minus_expression = f_validity_checker->uminusExpr(v_expression);

      return f_validity_checker->iteExpr(v_guard, v_expression, v_minus_expression);
    }

    // --------------------------------------------------------------------------------------------

    CVCL::Expr SMT_Solver_CVC_Lite_Fast::translate_succ(ATermAppl a_clause) {
      ATermAppl v_clause;
      CVCL::Expr v_expression;
      CVCL::Expr v_one;

      v_clause = f_expression_info.get_argument(a_clause, 0);
      v_expression = translate_clause(v_clause);
      v_one = f_validity_checker->ratExpr(1);
      return f_validity_checker->plusExpr(v_expression, v_one);
    }

    // --------------------------------------------------------------------------------------------

    CVCL::Expr SMT_Solver_CVC_Lite_Fast::translate_pred(ATermAppl a_clause) {
      ATermAppl v_clause;
      CVCL::Expr v_expression;
      CVCL::Expr v_one;

      v_clause = f_expression_info.get_argument(a_clause, 0);
      v_expression = translate_clause(v_clause);
      v_one = f_validity_checker->ratExpr(1);
      return f_validity_checker->minusExpr(v_expression, v_one);
    }

    // --------------------------------------------------------------------------------------------

    CVCL::Expr SMT_Solver_CVC_Lite_Fast::translate_add_c(ATermAppl a_clause) {
      ATermAppl v_clause_1, v_clause_2, v_clause_3;
      CVCL::Expr v_expression_1, v_expression_2, v_expression_3;
      CVCL::Expr v_one;
      CVCL::Expr v_plus;
      CVCL::Expr v_plus_one;

      v_clause_1 = f_expression_info.get_argument(a_clause, 0);
      v_clause_2 = f_expression_info.get_argument(a_clause, 1);
      v_clause_3 = f_expression_info.get_argument(a_clause, 2);
      v_expression_1 = translate_clause(v_clause_1);
      v_expression_2 = translate_clause(v_clause_2);
      v_expression_3 = translate_clause(v_clause_3);
      v_one = f_validity_checker->ratExpr(1);
      v_plus = f_validity_checker->plusExpr(v_expression_1, v_expression_2);
      v_plus_one = f_validity_checker->plusExpr(v_plus, v_one);
      return f_validity_checker->iteExpr(v_expression_1, v_plus, v_plus_one);
    }

    // --------------------------------------------------------------------------------------------

    CVCL::Expr SMT_Solver_CVC_Lite_Fast::translate_c_nat(ATermAppl a_clause) {
      ATermAppl v_clause;

      v_clause = f_expression_info.get_argument(a_clause, 0);
      return translate_clause(v_clause);
    }

    // --------------------------------------------------------------------------------------------

    CVCL::Expr SMT_Solver_CVC_Lite_Fast::translate_c_int(ATermAppl a_clause) {
      ATermAppl v_clause;

      v_clause = f_expression_info.get_argument(a_clause, 0);
      return translate_clause(v_clause);
    }

    // --------------------------------------------------------------------------------------------

    CVCL::Expr SMT_Solver_CVC_Lite_Fast::translate_c_real(ATermAppl a_clause) {
      ATermAppl v_clause;

      v_clause = f_expression_info.get_argument(a_clause, 0);
      return translate_clause(v_clause);
    }

    // --------------------------------------------------------------------------------------------

    CVCL::Expr SMT_Solver_CVC_Lite_Fast::translate_unknown_operator(ATermAppl a_clause) {
      ATermAppl v_operator;
      int v_number_of_arguments;
      std::string v_name;
      CVCL::Type v_type;
      CVCL::Op v_translated_operator;
      ATermAppl v_clause;
      CVCL::Expr v_translated_clause;
      std::vector<CVCL::Expr> v_translated_clauses;

      v_operator = f_expression_info.get_operator(a_clause);
      v_number_of_arguments = f_expression_info.get_number_of_arguments(a_clause);
      if (f_list_of_operators.element_in_list(v_operator)) {
        v_translated_operator = f_list_of_operators.get_element(v_operator);
      } else {
        v_name = f_expression_info.get_name_of_operator(v_operator);
        v_type = translate_sort(f_expression_info.get_sort_of_operator(a_clause));
        v_translated_operator = f_validity_checker->createOp(v_name, v_type);
        f_list_of_operators.add_pair_to_list(v_operator, v_translated_operator);
      }

      for (int i = 0; i < v_number_of_arguments; i++) {
        v_clause = f_expression_info.get_argument(a_clause, i);
        v_translated_clause = translate_clause(v_clause);
        v_translated_clauses.push_back(v_translated_clause);
      }
      return f_validity_checker->funExpr(v_translated_operator, v_translated_clauses);
    }

    // --------------------------------------------------------------------------------------------

    CVCL::Expr SMT_Solver_CVC_Lite_Fast::translate_variable(ATermAppl a_clause) {
      std::string v_name;
      ATermAppl v_type;
      CVCL::Type v_translated_type;

      v_name = f_expression_info.get_name_of_variable(a_clause);
      v_type = f_expression_info.get_sort_of_variable(a_clause);
      v_translated_type = translate_sort(v_type);
      return f_validity_checker->varExpr(v_name, v_translated_type);
    }

    // --------------------------------------------------------------------------------------------

    CVCL::Expr SMT_Solver_CVC_Lite_Fast::translate_nat_variable(ATermAppl a_clause) {
      std::string v_name;

      v_name = f_expression_info.get_name_of_variable(a_clause);
      ATindexedSetPut(f_nat_variables, (ATerm) a_clause, 0);
      return f_validity_checker->varExpr(v_name, f_validity_checker->intType());
    }

    // --------------------------------------------------------------------------------------------

    CVCL::Expr SMT_Solver_CVC_Lite_Fast::translate_pos_variable(ATermAppl a_clause) {
      std::string v_name;

      v_name = f_expression_info.get_name_of_variable(a_clause);
      ATindexedSetPut(f_pos_variables, (ATerm) a_clause, 0);
      return f_validity_checker->varExpr(v_name, f_validity_checker->intType());
    }

    // --------------------------------------------------------------------------------------------

    CVCL::Expr SMT_Solver_CVC_Lite_Fast::translate_int_constant(ATermAppl a_clause) {
      int v_value;

      v_value = gsIntValue_int(a_clause);
      return f_validity_checker->ratExpr(v_value);
    }

    // --------------------------------------------------------------------------------------------

    CVCL::Expr SMT_Solver_CVC_Lite_Fast::translate_nat_constant(ATermAppl a_clause) {
      int v_value;

      v_value = gsNatValue_int(a_clause);
      return f_validity_checker->ratExpr(v_value);
    }

    // --------------------------------------------------------------------------------------------

    CVCL::Expr SMT_Solver_CVC_Lite_Fast::translate_pos_constant(ATermAppl a_clause) {
      int v_value;

      v_value = gsPosValue_int(a_clause);
      return f_validity_checker->ratExpr(v_value);
    }

    // --------------------------------------------------------------------------------------------

    CVCL::Expr SMT_Solver_CVC_Lite_Fast::translate_true() {
      return f_validity_checker->trueExpr();
    }

    // --------------------------------------------------------------------------------------------

    CVCL::Expr SMT_Solver_CVC_Lite_Fast::translate_false() {
      return f_validity_checker->falseExpr();
    }

    // --------------------------------------------------------------------------------------------

    CVCL::Expr SMT_Solver_CVC_Lite_Fast::translate_constant(ATermAppl a_clause) {
      std::string v_name;
      ATermAppl v_type;
      CVCL::Type v_translated_type;

      v_name = f_expression_info.get_name_of_constant(a_clause);
      v_type = f_expression_info.get_sort_of_constant(a_clause);
      v_translated_type = translate_sort(v_type);
      return f_validity_checker->varExpr(v_name, v_translated_type);
    }

    // --------------------------------------------------------------------------------------------

    void SMT_Solver_CVC_Lite_Fast::add_nat_clauses(std::vector<CVCL::Expr>& a_expressions) {
      ATermList v_variables;
      ATermAppl v_variable;
      std::string v_name;
      CVCL::Expr v_translated_variable;
      CVCL::Expr v_clause;
      CVCL::Expr v_zero;

      v_zero = f_validity_checker->ratExpr(0);
      v_variables = ATindexedSetElements(f_nat_variables);
      if (!ATisEmpty(v_variables)) {
        while (!ATisEmpty(v_variables)) {
          v_variable = ATAgetFirst(v_variables);
          v_variables = ATgetNext(v_variables);
          v_name = f_expression_info.get_name_of_variable(v_variable);
          v_translated_variable = f_validity_checker->varExpr(v_name, f_validity_checker->intType());
          v_clause = f_validity_checker->geExpr(v_translated_variable, v_zero);
          a_expressions.push_back(v_clause);
        }
      }
    }

    // --------------------------------------------------------------------------------------------

    void SMT_Solver_CVC_Lite_Fast::add_pos_clauses(std::vector<CVCL::Expr>& a_expressions) {
      ATermList v_variables;
      ATermAppl v_variable;
      std::string v_name;
      CVCL::Expr v_translated_variable;
      CVCL::Expr v_clause;
      CVCL::Expr v_zero;

      v_zero = f_validity_checker->ratExpr(0);
      v_variables = ATindexedSetElements(f_pos_variables);
      if (!ATisEmpty(v_variables)) {
        while (!ATisEmpty(v_variables)) {
          v_variable = ATAgetFirst(v_variables);
          v_variables = ATgetNext(v_variables);
          v_name = f_expression_info.get_name_of_variable(v_variable);
          v_translated_variable = f_validity_checker->varExpr(v_name, f_validity_checker->intType());
          v_clause = f_validity_checker->gtExpr(v_translated_variable, v_zero);
          a_expressions.push_back(v_clause);
        }
      }
    }

  // ----------------------------------------------------------------------------------------------

    void SMT_Solver_CVC_Lite_Fast::translate(ATermList a_formula) {
      ATermAppl v_clause;
      CVCL::Expr v_expression;
      std::vector<CVCL::Expr> v_expressions;

      ATindexedSetReset(f_nat_variables);
      ATindexedSetReset(f_pos_variables);

      gsVerboseMsg("Formula to be solved: %P\n", a_formula);
      while (!ATisEmpty(a_formula)) {
        v_clause = ATAgetFirst(a_formula);
        a_formula = ATgetNext(a_formula);
        v_expression = translate_clause(v_clause);
        v_expressions.push_back(v_expression);
      }
      add_nat_clauses(v_expressions);
      add_pos_clauses(v_expressions);

      f_formula = f_validity_checker->andExpr(v_expressions);
      gsVerboseMsg("Formula in CVC Lite format: %s\n", f_formula.toString().c_str());
    }

  // Class SMT_Solver_CVC_Lite_Fast - Functions declared public -----------------------------------

    SMT_Solver_CVC_Lite_Fast::SMT_Solver_CVC_Lite_Fast() {
      f_validity_checker = CVCL::ValidityChecker::create();
      f_nat_variables = ATindexedSetCreate(100, 75);
      f_pos_variables = ATindexedSetCreate(100, 75);
    }

    // --------------------------------------------------------------------------------------------

    SMT_Solver_CVC_Lite_Fast::~SMT_Solver_CVC_Lite_Fast() {
      delete f_validity_checker;
      f_validity_checker = 0;
      ATindexedSetDestroy(f_nat_variables);
      ATindexedSetDestroy(f_pos_variables);
    }

    // --------------------------------------------------------------------------------------------

    bool SMT_Solver_CVC_Lite_Fast::is_satisfiable(ATermList a_formula) {
      f_validity_checker->poptoScope(0);
      translate(a_formula);
      if (f_validity_checker->checkUnsat(f_formula)) {
        gsVerboseMsg("The formula is unsatisfiable\n");
        return false;
      } else {
        gsVerboseMsg("The formula is satisfiable\n");
        return true;
      }
    }

#endif // CVC_LITE_LIB
