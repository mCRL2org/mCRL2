// Interface to class Induction
// file: induction.h

#ifndef INDUCTION_H
#define INDUCTION_H

#include "aterm2.h"
#include "bdd_info.h"
#include "expression_info.h"
#include "sort_info.h"

  /// The class Induction generates statements corresponding to 

class Induction {
  private:
    /// \brief The smallest number x for which no variable named "dummyx" exists.
    int f_fresh_dummy_number;

    /// \brief The number of variables used during the last application of induction.
    int f_count;

    /// \brief An expression of soort Bool in mCRL2 format.
    ATermAppl f_formula;

    /// \brief 
    ATermAppl f_constructors;

    /// \brief
    ATermAppl f_cons_name;

    /// \brief
    ATermIndexedSet f_list_variables;

    /// \brief
    ATermTable f_lists_to_sorts;

    /// \brief
    BDD_Info f_bdd_info;

    /// \brief
    Expression_Info f_expression_info;

    /// \brief
    Sort_Info f_sort_info;

    /// \brief
    void recurse_expression_for_lists(ATermAppl a_expression);

    /// \brief
    void map_lists_to_sorts();

    /// \brief
    ATermAppl get_sort_of_list_elements(ATermAppl a_list_variable);

    /// \brief
    ATermAppl get_fresh_dummy(ATermAppl a_sort);

    /// \brief
    ATermAppl apply_induction_one();

    /// \brief
    ATermAppl create_hypotheses(
      ATermAppl a_hypothesis,
      ATermList a_list_of_variables,
      ATermList a_list_of_dummies
    );

    /// \brief
    ATermList create_clauses(
      ATermAppl a_formula,
      ATermAppl a_hypothesis,
      int a_variable_number,
      int a_number_of_variables,
      ATermList a_list_of_variables,
      ATermList a_list_of_dummies
    );
  public:
    /// \brief
    Induction(ATermAppl a_lpe);

    /// \brief
    ~Induction();

    /// \brief
    void initialize(ATermAppl a_formula);

    /// \brief
    bool can_apply_induction();

    /// \brief
    ATermAppl apply_induction();
};

#endif
