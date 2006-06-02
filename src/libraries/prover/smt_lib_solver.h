// Interface to class SMT_LIB_Solver
// file: smt_lib_solver.h

#ifndef SMT_LIB_SOLVER_H
#define SMT_LIB_SOLVER_H

#include "aterm2.h"
#include "smt_solver.h"
#include "expression_info.h"
#include "sort_info.h"
#include <string>

using namespace std;

class SMT_LIB_Solver: public SMT_Solver {
  private:
    ATermIndexedSet f_variables;
    ATermIndexedSet f_operators;
    ATermIndexedSet f_sorts;
    void declare_variables();
    void declare_operators();
    void declare_sorts();
    void produce_notes_for_operators();
    void produce_notes_for_sorts();
    void translate_clause(ATermAppl a_clause);
    void translate_not(ATermAppl a_clause);
    void translate_equality(ATermAppl a_clause);
    void translate_inequality(ATermAppl a_clause);
    void translate_greater_than(ATermAppl a_clause);
    void translate_greater_than_or_equal(ATermAppl a_clause);
    void translate_less_than(ATermAppl a_clause);
    void translate_less_than_or_equal(ATermAppl a_clause);
    void translate_plus(ATermAppl a_clause);
    void translate_unary_minus(ATermAppl a_clause);
    void translate_binary_minus(ATermAppl a_clause);
    void translate_multiplication(ATermAppl a_clause);
    void translate_unknown_operator(ATermAppl a_clause);
    void translate_real_variable(ATermAppl a_clause);
    void translate_int_variable(ATermAppl a_clause);
    void translate_nat_variable(ATermAppl a_clause);
    void translate_pos_variable(ATermAppl a_clause);
    void translate_unknown_variable(ATermAppl a_clause);
    void translate_int_constant(ATermAppl a_clause);
    void translate_nat_constant(ATermAppl a_clause);
    void translate_pos_constant(ATermAppl a_clause);
    void translate_unknown_constant(ATermAppl a_clause);
  protected:
    Expression_Info f_expression_info;
    Sort_Info f_sort_info;
    string f_benchmark;
    string f_extrasorts;
    string f_operators_extrafuns;
    string f_variables_extrafuns;
    string f_formula;
    string f_operators_notes;
    string f_sorts_notes;
    void translate(ATermList a_formula);
  public:
    SMT_LIB_Solver();
    ~SMT_LIB_Solver();
};

#endif
