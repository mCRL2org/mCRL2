// Implementation of class Induction
// file: induction.cpp

#include "prover/induction.h"
#include "liblowlevel.h"
#include "libstruct.h"
#include "libprint_c.h"
#include "auxiliary/utilities.h"

// Class Induction --------------------------------------------------------------------------------

  // Class Induction - Functions declared private -------------------------------------------------

    void Induction::recurse_expression_for_lists(ATermAppl a_expression) {
      ATermAppl v_argument;
      ATermAppl v_sort;
      int v_number_of_arguments;

      if (f_expression_info.is_variable(a_expression)) {
        v_sort = f_expression_info.get_sort_of_variable(a_expression);
        if (f_sort_info.is_sort_list(v_sort)) {
          ATindexedSetPut(f_list_variables, (ATerm) a_expression, 0);
        }
      } else if (f_expression_info.is_operator(a_expression)) {
        v_number_of_arguments = f_expression_info.get_number_of_arguments(a_expression);
        for (int i = 0; i < v_number_of_arguments; ++i) {
          v_argument = f_expression_info.get_argument(a_expression, i);
          recurse_expression_for_lists(v_argument);
        }
      }
    }

    // --------------------------------------------------------------------------------------------

    void Induction::map_lists_to_sorts() {
      ATermList v_list_variables;
      ATermAppl v_list_variable;
      ATermAppl v_sort;

      v_list_variables = ATindexedSetElements(f_list_variables);
      while (!ATisEmpty(v_list_variables)) {
        v_list_variable = ATAgetFirst(v_list_variables);
        v_list_variables = ATgetNext(v_list_variables);
        v_sort = get_sort_of_list_elements(v_list_variable);
        ATtablePut(f_lists_to_sorts, (ATerm) v_list_variable, (ATerm) v_sort);
      }
    }

    // --------------------------------------------------------------------------------------------

    ATermAppl Induction::get_sort_of_list_elements(ATermAppl a_list_variable) {
      ATermList v_constructors;
      ATermAppl v_constructor;
      ATermAppl v_constructor_name;
      ATermAppl v_constructor_sort;
      ATermAppl v_constructor_element_sort;
      ATermAppl v_list_sort;
      ATermAppl v_result = 0;

      v_constructors = ATLgetArgument(f_constructors, 0);
      v_list_sort = f_expression_info.get_sort_of_variable(a_list_variable);
      while (!ATisEmpty(v_constructors)) {
        v_constructor = ATAgetFirst(v_constructors);
        v_constructors = ATgetNext(v_constructors);
        v_constructor_name = ATAgetArgument(v_constructor, 0);
        if (v_constructor_name == f_cons_name) {
          v_constructor_sort = f_expression_info.get_sort_of_operator(v_constructor);
          v_constructor_element_sort = f_sort_info.get_domain(v_constructor_sort);
          v_constructor_sort = f_sort_info.get_range(v_constructor_sort);
          v_constructor_sort = f_sort_info.get_domain(v_constructor_sort);
          if (v_constructor_sort == v_list_sort) {
            v_result = v_constructor_element_sort;
          }
        }
      }

      return v_result;
    }

    // --------------------------------------------------------------------------------------------

    ATermAppl Induction::get_fresh_dummy(ATermAppl a_sort) {
      char* v_dummy_string;
      ATermAppl v_dummy_name;
      ATermAppl v_result;

      do {
        v_dummy_string = (char*) malloc((number_of_digits(f_fresh_dummy_number) + 6) * sizeof(char));
        sprintf(v_dummy_string, "dummy%d", f_fresh_dummy_number);
        v_dummy_name = gsString2ATermAppl(v_dummy_string);
        v_result = gsMakeDataVarId(v_dummy_name, a_sort);
        free(v_dummy_string);
        v_dummy_string = 0;
        f_fresh_dummy_number++;
      } while (gsOccurs((ATerm) v_result, (ATerm) f_formula));
      return v_result;
    }

  // Class Induction - Functions declared public --------------------------------------------------

    Induction::Induction(ATermAppl a_data_spec) {
      f_list_variables = ATindexedSetCreate(50, 75);
      f_lists_to_sorts = ATtableCreate(50, 75);
      f_constructors = ATAgetArgument(a_data_spec, 1);
      f_cons_name = gsMakeOpIdNameCons();
    }

    // --------------------------------------------------------------------------------------------

    Induction::~Induction() {
      ATindexedSetDestroy(f_list_variables);
      ATtableDestroy(f_lists_to_sorts);
    }

    // --------------------------------------------------------------------------------------------

    void Induction::initialize(ATermAppl a_formula) {
      f_formula = a_formula;
      ATindexedSetReset(f_list_variables);
      recurse_expression_for_lists(a_formula);
      map_lists_to_sorts();
      f_count = 0;
    }

    // --------------------------------------------------------------------------------------------

    bool Induction::can_apply_induction() {
      ATermList v_list_variables;

      v_list_variables = ATindexedSetElements(f_list_variables);
      if (ATgetLength(v_list_variables) == f_count) {
        return false;
      } else {
        f_count++;
        return true;
      }
    }

    // --------------------------------------------------------------------------------------------

    ATermAppl Induction::apply_induction_one() {
      ATermAppl v_induction_variable;
      ATermAppl v_induction_variable_sort;
      ATermAppl v_dummy_variable;
      ATermAppl v_dummy_sort;
      ATermAppl v_substitution;
      ATermList v_substitution_list;
      ATermAppl v_base_case;
      ATermAppl v_induction_step;
      ATermAppl v_result;

      v_induction_variable = ATAgetFirst(ATindexedSetElements(f_list_variables));
      v_induction_variable_sort = f_expression_info.get_sort_of_variable(v_induction_variable);

      v_dummy_sort = get_sort_of_list_elements(v_induction_variable);
      v_dummy_variable = get_fresh_dummy(v_dummy_sort);

      v_substitution = gsMakeSubst_Appl(v_induction_variable, gsMakeOpIdEmptyList(v_induction_variable_sort));
      v_substitution_list = ATmakeList1((ATerm) v_substitution);
      v_base_case = gsSubstValues_Appl(v_substitution_list, f_formula, true);

      v_substitution = gsMakeSubst_Appl(v_induction_variable, gsMakeDataExprCons(v_dummy_variable, v_induction_variable));
      v_substitution_list = ATmakeList1((ATerm) v_substitution);
      v_induction_step = gsSubstValues_Appl(v_substitution_list, f_formula, true);
      v_induction_step = gsMakeDataExprImp(f_formula, v_induction_step);

      v_result = gsMakeDataExprAnd(v_base_case, v_induction_step);
      return v_result;
    }

    // --------------------------------------------------------------------------------------------

    ATermAppl Induction::create_hypotheses(ATermAppl a_hypothesis, ATermList a_list_of_variables, ATermList a_list_of_dummies) {
      ATermAppl v_variable;
      ATermAppl v_variable_sort;
      ATermAppl v_dummy;
      ATermAppl v_clause;
      ATermAppl v_substitution;
      ATermList v_substitution_list;

      if (ATisEmpty(a_list_of_variables)) {
        return gsMakeDataExprTrue();
      } else {
        v_clause = a_hypothesis;
        if (ATgetLength(a_list_of_variables) > 1) {
          while (!ATisEmpty(a_list_of_variables)) {
            v_variable = ATAgetFirst(a_list_of_variables);
            v_variable_sort = f_expression_info.get_sort_of_variable(v_variable);
            a_list_of_variables = ATgetNext(a_list_of_variables);
            v_dummy = ATAgetFirst(a_list_of_dummies);
            a_list_of_dummies = ATgetNext(a_list_of_dummies);
            v_substitution = gsMakeSubst_Appl(v_variable, gsMakeDataExprCons(v_dummy, v_variable));
            v_substitution_list = ATmakeList1((ATerm) v_substitution);
            v_clause = gsMakeDataExprAnd(v_clause, gsSubstValues_Appl(v_substitution_list, a_hypothesis, true));
          }
        }

        return v_clause;
      }
    }

    // --------------------------------------------------------------------------------------------

    ATermList Induction::create_clauses(
      ATermAppl a_formula, ATermAppl a_hypothesis, int a_variable_number, int a_number_of_variables,
      ATermList a_list_of_variables, ATermList a_list_of_dummies
    ) {
      ATermList v_list_1, v_list_2;
      ATermAppl v_formula_1, v_formula_2;
      ATermAppl v_hypothesis;
      ATermAppl v_dummy;
      ATermAppl v_dummy_sort;
      ATermList v_list_of_dummies;
      ATermAppl v_variable;
      ATermAppl v_variable_sort;
      ATermAppl v_substitution;
      ATermList v_substitution_list;
      ATermList v_list_of_variables;
      ATermAppl v_hypotheses_1, v_hypotheses_2;
      ATermList v_result;

      v_variable = (ATermAppl) ATindexedSetGetElem(f_list_variables, a_variable_number);
      v_variable_sort = f_expression_info.get_sort_of_variable(v_variable);
      v_list_of_variables = ATinsert(a_list_of_variables, (ATerm) v_variable);
      v_dummy_sort = get_sort_of_list_elements(v_variable);
      v_dummy = get_fresh_dummy(v_dummy_sort);
      v_list_of_dummies = ATinsert(a_list_of_dummies, (ATerm) v_dummy);
      v_substitution = gsMakeSubst_Appl(v_variable, gsMakeDataExprCons(v_dummy, v_variable));
      v_substitution_list = ATmakeList1((ATerm) v_substitution);
      v_formula_1 = gsSubstValues_Appl(v_substitution_list, a_formula, true);
      v_substitution = gsMakeSubst_Appl(v_variable, gsMakeOpIdEmptyList(v_variable_sort));
      v_substitution_list = ATmakeList1((ATerm) v_substitution);
      v_formula_2 = gsSubstValues_Appl(v_substitution_list, a_formula, true);
      v_hypothesis = gsSubstValues_Appl(v_substitution_list, a_hypothesis, true);

      if (a_variable_number < a_number_of_variables - 1) {
        v_list_1 = create_clauses(v_formula_1, a_hypothesis, a_variable_number + 1, a_number_of_variables, v_list_of_variables, v_list_of_dummies);
        v_list_2 = create_clauses(v_formula_2, v_hypothesis, a_variable_number + 1, a_number_of_variables, a_list_of_variables, a_list_of_dummies);
        v_result = ATconcat(v_list_1, v_list_2);
      } else {
        v_hypotheses_1 = create_hypotheses(a_hypothesis, v_list_of_variables, v_list_of_dummies);
        v_hypotheses_2 = create_hypotheses(v_hypothesis, a_list_of_variables, a_list_of_dummies);
        v_result = ATmakeList2((ATerm) gsMakeDataExprImp(v_hypotheses_1, v_formula_1), (ATerm) gsMakeDataExprImp(v_hypotheses_2, v_formula_2));
      }

      return v_result;
    }

    // --------------------------------------------------------------------------------------------

    ATermAppl Induction::apply_induction() {
      ATermAppl v_result;
      ATermList v_list_of_clauses;
      ATermAppl v_clause;

      f_fresh_dummy_number = 0;
      if (f_count == 1) {
        gsVerboseMsg("Induction on one variable.\n");
        v_result = apply_induction_one();
      } else {
        gsVerboseMsg("Induction on %d variables.\n", f_count);
        v_list_of_clauses = create_clauses(f_formula, f_formula, 0, f_count, ATmakeList0(), ATmakeList0());
        v_result = ATAgetFirst(v_list_of_clauses);
        v_list_of_clauses = ATgetNext(v_list_of_clauses);
        while (!ATisEmpty(v_list_of_clauses)) {
          v_clause = ATAgetFirst(v_list_of_clauses);
          v_list_of_clauses = ATgetNext(v_list_of_clauses);
          v_result = gsMakeDataExprAnd(v_result, v_clause);
        }
      }

      return v_result;
    }
