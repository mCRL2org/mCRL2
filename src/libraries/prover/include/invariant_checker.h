// Interface to class Invariant_Checker
// file: invariant_checker.h

#ifndef INVARIANT_CHECKER_H
#define INVARIANT_CHECKER_H

#include "aterm2.h"
#include "librewrite.h"
#include "bdd_prover.h"
#include "bdd2dot.h"

class Invariant_Checker {
  private:
    BDD_Prover f_bdd_prover;
    BDD2Dot f_bdd2dot;
    ATermAppl f_init;
    ATermList f_summands;
    bool f_counter_example;
    bool f_all;
    char* f_dot_file_name;
    void print_counter_example();
    void save_dot_file(int a_summand_number);
    bool check_init(ATermAppl a_invariant);
    bool check_summand(ATermAppl a_invariant, ATermAppl a_summand, int a_summand_number);
    bool check_summands(ATermAppl a_invariant);
  public:
    Invariant_Checker(
      RewriteStrategy a_rewrite_strategy,
      int a_time_limit,
      bool a_path_eliminator,
      SMT_Solver_Type a_solver_type,
      ATermAppl a_lpe,
      bool a_counter_example,
      bool a_all,
      char* a_dot_file_name
    );
    ~Invariant_Checker();
    bool check_invariant(ATermAppl a_invariant);
};

#endif
