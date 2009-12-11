// Author(s): Luc Engelen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <cstdlib>

#include "mcrl2/data/detail/prover/induction.h"
#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/core/numeric_string.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/data/list.h"

using namespace mcrl2::core;
using namespace mcrl2::core::detail;

namespace mcrl2 {
  namespace data {
    namespace detail {

// Class Induction --------------------------------------------------------------------------------

  // Class Induction - Functions declared private -------------------------------------------------

    void Induction::recurse_expression_for_lists(ATermAppl a_expression) {
      if (gsIsDataVarId(a_expression)) {
        sort_expression v_sort = data_expression(a_expression).sort();
        if (sort_list::is_list(v_sort)) {
          ATindexedSetPut(f_list_variables, (ATerm) a_expression, 0);
        }
      } else if (f_expression_info.is_operator(a_expression)) {
        ATermList v_arguments = gsGetDataExprArgs(a_expression);
        while (!ATisEmpty(v_arguments)) {
          ATermAppl v_argument = ATAgetFirst(v_arguments);
          v_arguments = ATgetNext(v_arguments);
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
      sort_expression v_constructor_sort;
      ATermAppl v_constructor_element_sort;
      ATermAppl v_list_sort;
      ATermAppl v_result = 0;

      v_constructors = f_constructors;
      v_list_sort = data_expression(a_list_variable).sort();
      while (!ATisEmpty(v_constructors)) {
        v_constructor = ATAgetFirst(v_constructors);
        v_constructors = ATgetNext(v_constructors);
        v_constructor_name = ATAgetArgument(v_constructor, 0);
        if (v_constructor_name == f_cons_name) {
          v_constructor_sort = data_expression(v_constructor).sort();
          //(JK) This seems dangerous, assumes the constructor sort is a
          //function!
          v_constructor_element_sort = *(function_sort(v_constructor_sort).domain().begin());
          v_constructor_sort = *(++(function_sort(v_constructor_sort).domain().begin()));
          //v_constructor_element_sort = ATAelementAt(f_sort_info.get_domain(v_constructor_sort),0);
          //v_constructor_sort = ATAelementAt(f_sort_info.get_domain(v_constructor_sort),1);
          if (v_constructor_sort == v_list_sort) {
            v_result = v_constructor_element_sort;
          }
        }
      }

      return v_result;
    }

    // --------------------------------------------------------------------------------------------

    ATermAppl Induction::get_fresh_dummy(ATermAppl a_sort) {
      ATermAppl v_result;
      do {
        char* v_dummy_string = (char*) malloc((NrOfChars(f_fresh_dummy_number) + 6) * sizeof(char));
        sprintf(v_dummy_string, "dummy%d", f_fresh_dummy_number);
        ATermAppl v_dummy_name = gsString2ATermAppl(v_dummy_string);
        v_result = gsMakeDataVarId(v_dummy_name, a_sort);
        free(v_dummy_string);
        v_dummy_string = 0;
        f_fresh_dummy_number++;
      } while (gsOccurs((ATerm) v_result, (ATerm) f_formula));
      return v_result;
    }

  // Class Induction - Functions declared public --------------------------------------------------

    Induction::Induction(const data_specification &a_data_spec) {
      f_list_variables = ATindexedSetCreate(50, 75);
      f_lists_to_sorts = ATtableCreate(50, 75);

      // f_constructors = reinterpret_cast< ATermList >(static_cast< ATermAppl >(atermpp::arg2(a_data_spec)));
      f_constructors=convert< atermpp::aterm_list > (a_data_spec.constructors());
      f_cons_name = sort_list::cons_name();
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
      v_induction_variable_sort = data_expression(v_induction_variable).sort();

      v_dummy_sort = get_sort_of_list_elements(v_induction_variable);
      v_dummy_variable = get_fresh_dummy(v_dummy_sort);

      v_substitution = gsMakeSubst_Appl(v_induction_variable, sort_list::nil(sort_expression(v_induction_variable_sort)));
      v_substitution_list = ATmakeList1((ATerm) v_substitution);
      v_base_case = gsSubstValues_Appl(v_substitution_list, f_formula, true);

      v_substitution = gsMakeSubst_Appl(v_induction_variable, sort_list::cons_(data_expression(v_dummy_variable).sort(), data_expression(v_dummy_variable), data_expression(v_induction_variable)));
      v_substitution_list = ATmakeList1((ATerm) v_substitution);
      v_induction_step = gsSubstValues_Appl(v_substitution_list, f_formula, true);
      v_induction_step = sort_bool::implies(data_expression(f_formula), data_expression(v_induction_step));

      v_result = sort_bool::and_(data_expression(v_base_case), data_expression(v_induction_step));
      return v_result;
    }

    // --------------------------------------------------------------------------------------------

    ATermAppl Induction::create_hypotheses(ATermAppl a_hypothesis, ATermList a_list_of_variables, ATermList a_list_of_dummies) {
      if (ATisEmpty(a_list_of_variables)) {
        return sort_bool::true_();
      } else {
        ATermAppl v_clause = a_hypothesis;
        if (ATgetLength(a_list_of_variables) > 1) {
          while (!ATisEmpty(a_list_of_variables)) {
            data_expression v_variable(ATAgetFirst(a_list_of_variables));
            a_list_of_variables = ATgetNext(a_list_of_variables);
            data_expression v_dummy(ATAgetFirst(a_list_of_dummies));
            a_list_of_dummies = ATgetNext(a_list_of_dummies);
            ATermAppl v_substitution = gsMakeSubst_Appl(v_variable, sort_list::cons_(v_dummy.sort(), v_dummy, v_variable));
            ATermList v_substitution_list = ATmakeList1((ATerm) v_substitution);
            v_clause = sort_bool::and_(data_expression(v_clause), data_expression(gsSubstValues_Appl(v_substitution_list, a_hypothesis, true)));
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
      ATermAppl v_variable = (ATermAppl) ATindexedSetGetElem(f_list_variables, a_variable_number);
      ATermAppl v_variable_sort = data_expression(v_variable).sort();
      ATermList v_list_of_variables = ATinsert(a_list_of_variables, (ATerm) v_variable);
      ATermAppl v_dummy_sort = get_sort_of_list_elements(v_variable);
      ATermAppl v_dummy = get_fresh_dummy(v_dummy_sort);
      ATermList v_list_of_dummies = ATinsert(a_list_of_dummies, (ATerm) v_dummy);
      ATermAppl v_substitution = gsMakeSubst_Appl(v_variable, sort_list::cons_(data_expression(v_dummy).sort(), data_expression(v_dummy), data_expression(v_variable)));
      ATermList v_substitution_list = ATmakeList1((ATerm) v_substitution);
      ATermAppl v_formula_1 = gsSubstValues_Appl(v_substitution_list, a_formula, true);
      v_substitution = gsMakeSubst_Appl(v_variable, sort_list::nil(sort_expression(v_variable_sort)));
      v_substitution_list = ATmakeList1((ATerm) v_substitution);
      ATermAppl v_formula_2 = gsSubstValues_Appl(v_substitution_list, a_formula, true);
      ATermAppl v_hypothesis = gsSubstValues_Appl(v_substitution_list, a_hypothesis, true);

      if (a_variable_number < a_number_of_variables - 1) {
        ATermList v_list_1 = create_clauses(v_formula_1, a_hypothesis, a_variable_number + 1, a_number_of_variables, v_list_of_variables, v_list_of_dummies);
        ATermList v_list_2 = create_clauses(v_formula_2, v_hypothesis, a_variable_number + 1, a_number_of_variables, a_list_of_variables, a_list_of_dummies);
        return ATconcat(v_list_1, v_list_2);
      } else {
        ATermAppl v_hypotheses_1 = create_hypotheses(a_hypothesis, v_list_of_variables, v_list_of_dummies);
        ATermAppl v_hypotheses_2 = create_hypotheses(v_hypothesis, a_list_of_variables, a_list_of_dummies);
        return ATmakeList2((ATerm) static_cast<ATermAppl>(sort_bool::implies(data_expression(v_hypotheses_1), data_expression(v_formula_1))),
                           (ATerm) static_cast<ATermAppl>(sort_bool::implies(data_expression(v_hypotheses_2), data_expression(v_formula_2))));
      }
    }

    // --------------------------------------------------------------------------------------------

    ATermAppl Induction::apply_induction() {
      ATermAppl v_result;

      f_fresh_dummy_number = 0;
      if (f_count == 1) {
        gsVerboseMsg("Induction on one variable.\n");
        v_result = apply_induction_one();
      } else {
        gsVerboseMsg("Induction on %d variables.\n", f_count);
        ATermList v_list_of_clauses = create_clauses(f_formula, f_formula, 0, f_count, ATmakeList0(), ATmakeList0());
        v_result = ATAgetFirst(v_list_of_clauses);
        v_list_of_clauses = ATgetNext(v_list_of_clauses);
        while (!ATisEmpty(v_list_of_clauses)) {
          data_expression v_clause(ATAgetFirst(v_list_of_clauses));
          v_list_of_clauses = ATgetNext(v_list_of_clauses);
          v_result = sort_bool::and_(data_expression(v_result), v_clause);
        }
      }

      return v_result;
    }
    }
  }
}
