// Interface to class SMT_LIB_Solver
// file: mcrl2/data/prover/smt_lib_solver.h

#ifndef SMT_LIB_SOLVER_H
#define SMT_LIB_SOLVER_H

#include "aterm2.h"
#include "mcrl2/data/prover/smt_solver.h"
#include "mcrl2/utilities/expression_info.h"
#include "mcrl2/utilities/sort_info.h"
#include <string>

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
    void translate_max(ATermAppl a_clause);
    void translate_min(ATermAppl a_clause);
    void translate_abs(ATermAppl a_clause);
    void translate_succ(ATermAppl a_clause);
    void translate_pred(ATermAppl a_clause);
    void translate_add_c(ATermAppl a_clause);
    void translate_c_nat(ATermAppl a_clause);
    void translate_c_int(ATermAppl a_clause);
    void translate_c_real(ATermAppl a_clause);
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

    /// precondition: The argument passed as parameter a_formula is a list of expressions of sort Bool in internal mCRL2
    /// format. The argument represents a formula in conjunctive normal form, where the elements of the list represent the
    /// clauses
    void translate(ATermList a_formula);
  public:
    SMT_LIB_Solver();
    virtual ~SMT_LIB_Solver();
};

#endif
