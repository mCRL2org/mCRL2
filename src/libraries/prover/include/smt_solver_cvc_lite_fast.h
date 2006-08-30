// Interface to class SMT_SOLVER_CVC_Lite_Fast
// file: smt_solver_cvc_lite_fast.h

#ifdef CVC_LITE_LIB

#ifndef SMT_SOLVER_CVC_LITE_FAST_H
#define SMT_SOLVER_CVC_LITE_FAST_H

#include "aterm2.h"
#include "smt_solver.h"
#include "expression_info.h"
#include "sort_info.h"
#include <string>
#include <vector>
#include "vc.h"

template <class Type>
class Pair {
  private:
    ATermAppl f_aterm;
    Type f_element;
  public:
    Pair(ATermAppl a_aterm, Type a_element);
    ATermAppl get_aterm();
    Type get_element();
};

template <class Type>
class List_Of_Pairs {
  private:
    std::vector< Pair<Type> > f_list_of_pairs;
  public:
    void add_pair_to_list(ATermAppl a_aterm, Type a_element);
    bool element_in_list(ATermAppl a_aterm);
    Type get_element(ATermAppl a_aterm);
};

class SMT_Solver_CVC_Lite_Fast: public SMT_Solver {
  private:
    Expression_Info f_expression_info;
    Sort_Info f_sort_info;
    CVCL::ValidityChecker* f_validity_checker;
    List_Of_Pairs<CVCL::Type> f_list_of_types;
    List_Of_Pairs<CVCL::Op> f_list_of_operators;

    ATermIndexedSet f_nat_variables;
    ATermIndexedSet f_pos_variables;

    CVCL::Type translate_sort(ATermAppl a_sort_expression);

    CVCL::Expr translate_clause(ATermAppl a_clause);

    CVCL::Expr translate_not(ATermAppl a_clause);
    CVCL::Expr translate_equality(ATermAppl a_clause);
    CVCL::Expr translate_inequality(ATermAppl a_clause);
    CVCL::Expr translate_greater_than(ATermAppl a_clause);
    CVCL::Expr translate_greater_than_or_equal(ATermAppl a_clause);
    CVCL::Expr translate_less_than(ATermAppl a_clause);
    CVCL::Expr translate_less_than_or_equal(ATermAppl a_clause);
    CVCL::Expr translate_plus(ATermAppl a_clause);
    CVCL::Expr translate_unary_minus(ATermAppl a_clause);
    CVCL::Expr translate_binary_minus(ATermAppl a_clause);
    CVCL::Expr translate_multiplication(ATermAppl a_clause);
    CVCL::Expr translate_max(ATermAppl a_clause);
    CVCL::Expr translate_min(ATermAppl a_clause);
    CVCL::Expr translate_abs(ATermAppl a_clause);
    CVCL::Expr translate_succ(ATermAppl a_clause);
    CVCL::Expr translate_pred(ATermAppl a_clause);
    CVCL::Expr translate_add_c(ATermAppl a_clause);
    CVCL::Expr translate_c_nat(ATermAppl a_clause);
    CVCL::Expr translate_c_int(ATermAppl a_clause);
    CVCL::Expr translate_c_real(ATermAppl a_clause);
    CVCL::Expr translate_unknown_operator(ATermAppl a_clause);
    CVCL::Expr translate_variable(ATermAppl a_clause);
    CVCL::Expr translate_nat_variable(ATermAppl a_clause);
    CVCL::Expr translate_pos_variable(ATermAppl a_clause);
    CVCL::Expr translate_int_constant(ATermAppl a_clause);
    CVCL::Expr translate_nat_constant(ATermAppl a_clause);
    CVCL::Expr translate_pos_constant(ATermAppl a_clause);
    CVCL::Expr translate_true();
    CVCL::Expr translate_false();
    CVCL::Expr translate_constant(ATermAppl a_clause);
    void add_nat_clauses(std::vector<CVCL::Expr>& a_expressions);
    void add_pos_clauses(std::vector<CVCL::Expr>& a_expressions);

    CVCL::Expr f_formula;
    void translate(ATermList a_formula);
  public:
    SMT_Solver_CVC_Lite_Fast();
    virtual ~SMT_Solver_CVC_Lite_Fast();
    virtual bool is_satisfiable(ATermList a_formula);
};

#endif // SMT_SOLVER_CVC_LITE_FAST_H

#endif // CVC_LITE_LIB
