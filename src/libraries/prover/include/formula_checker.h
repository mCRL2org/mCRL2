// Interface to class Formula_Checker
// file: formula_checker.h

#ifndef FORMULA_CHECKER_H
#define FORMULA_CHECKER_H

#include "aterm2.h"
#include "librewrite.h"
#include "bdd_prover.h"
#include "bdd2dot.h"

class Formula_Checker {
  private:
    BDD_Prover f_bdd_prover;
    BDD2Dot f_bdd2dot;
    bool f_counter_example;
    bool f_witness;
    char* f_dot_file_name;
    void print_witness();
    void print_counter_example();
    void save_dot_file(int a_formula_number);
  public:
    Formula_Checker(
      RewriteStrategy a_rewrite_strategy,
      int a_time_limit,
      bool a_path_eliminator,
      SMT_Solver_Type a_solver_type,
      ATermAppl a_data_equations,
      bool a_counter_example,
      bool a_witness,
      char* a_dot_file_name
    );
    ~Formula_Checker();
    void check_formulas(ATermList a_formulas);
};

#endif
