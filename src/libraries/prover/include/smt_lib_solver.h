// Interface to class SMT_LIB_Solver
// file: smt_lib_solver.h

#ifndef SMT_LIB_SOLVER_H
#define SMT_LIB_SOLVER_H

#include "aterm2.h"
#include "smt_solver.h"
#include "expression_info.h"
#include "sort_info.h"
#include <string>

class SMT_LIB_Solver: public SMT_Solver {
  private:
    Expression_Info f_expression_info;
    Sort_Info f_sort_info;
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
    void declare_sorts();
    void declare_operators();
    void declare_variables();
    void declare_predicates();
    void produce_notes_for_sorts();
    void produce_notes_for_operators();
    void produce_notes_for_predicates();
    void translate_clause(ATermAppl a_clause, bool a_expecting_predicate);
    void add_bool2pred_and_translate_clause(ATermAppl a_clause);
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
    void translate_add_c(ATermAppl a_clause);
    void translate_c_nat(ATermAppl a_clause);
    void translate_unknown_operator(ATermAppl a_clause);
    void translate_variable(ATermAppl a_clause);
    void translate_nat_variable(ATermAppl a_clause);
    void translate_pos_variable(ATermAppl a_clause);
    void translate_int_constant(ATermAppl a_clause);
    void translate_nat_constant(ATermAppl a_clause);
    void translate_pos_constant(ATermAppl a_clause);
    void translate_true();
    void translate_false();
    void translate_constant(ATermAppl a_clause);
    void add_nat_clauses();
    void add_pos_clauses();
  protected:
    std::string f_benchmark;
    void translate(ATermList a_formula);
  public:
    SMT_LIB_Solver();
    ~SMT_LIB_Solver();
};

#endif
