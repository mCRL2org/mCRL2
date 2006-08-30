// Interface to class Induction
// file: induction.h

#ifndef INDUCTION_H
#define INDUCTION_H

#include "aterm2.h"
#include "bdd_info.h"
#include "expression_info.h"
#include "sort_info.h"

class Induction {
  private:
    int f_fresh_dummy_number;
    int f_count;
    ATermAppl f_formula;
    ATermAppl f_constructors;
    ATermAppl f_cons_name;
    ATermIndexedSet f_list_variables;
    ATermTable f_lists_to_sorts;
    BDD_Info f_bdd_info;
    Expression_Info f_expression_info;
    Sort_Info f_sort_info;
    void recurse_expression_for_lists(ATermAppl a_expression);
    void map_lists_to_sorts();
    ATermAppl get_sort_of_list_elements(ATermAppl a_list_variable);
    ATermAppl get_fresh_dummy(ATermAppl a_sort);
    ATermAppl apply_induction_one();
    ATermAppl create_hypotheses(
      ATermAppl a_hypothesis,
      ATermList a_list_of_variables,
      ATermList a_list_of_dummies
    );
    ATermList create_clauses(
      ATermAppl a_formula,
      ATermAppl a_hypothesis,
      int a_variable_number,
      int a_number_of_variables,
      ATermList a_list_of_variables,
      ATermList a_list_of_dummies
    );
  public:
    Induction(ATermAppl a_lpe);
    ~Induction();
    void initialize(ATermAppl a_formula);
    bool can_apply_induction();
    ATermAppl apply_induction();
};

#endif
