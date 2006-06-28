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
      ATermAppl a_data_equations,
      RewriteStrategy a_rewrite_strategy = GS_REWR_JITTY,
      int a_time_limit = 0,
      bool a_path_eliminator = false,
      SMT_Solver_Type a_solver_type = solver_type_ario,
      bool a_counter_example = false,
      bool a_witness = false,
      char* a_dot_file_name = 0
    );
    ~Formula_Checker();
    void check_formulas(ATermList a_formulas);
};

#endif
