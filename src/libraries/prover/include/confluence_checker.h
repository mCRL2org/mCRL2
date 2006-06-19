// Interface to class Confluence_Checker
// file: confluence_checker.h

#ifndef CONFLUENCE_CHECKER_H
#define CONFLUENCE_CHECKER_H

#include "aterm2.h"
#include "librewrite.h"
#include "bdd_prover.h"
#include "disjointness_checker.h"
#include "invariant_checker.h"
#include "bdd2dot.h"

class Confluence_Checker {
  private:
    Disjointness_Checker f_disjointness_checker;
    Invariant_Checker f_invariant_checker;
    BDD_Prover f_bdd_prover;
    BDD2Dot f_bdd2dot;
    ATermAppl f_lpe;
    bool f_no_marking;
    bool f_check_all;
    bool f_counter_example;
    char* f_dot_file_name;
    bool f_generate_invariants;
    int f_number_of_summands;
    int* f_commutes;
    void save_dot_file(int a_summand_number_1, int a_summand_number_2);
    void print_counter_example();
    ATermAppl check_confluence_and_mark_summand(ATermAppl a_invariant, ATermAppl a_summand, int a_summand_number, bool& a_is_marked);
  public:
    Confluence_Checker(
      RewriteStrategy a_rewrite_strategy,
      int a_time_limit,
      bool a_path_eliminator,
      SMT_Solver_Type a_solver_type,
      ATermAppl a_lpe,
      bool a_no_marking,
      bool a_check_all,
      bool a_counter_example,
      char* a_dot_file_name,
      bool a_generate_invariants
    );
    ~Confluence_Checker();
    ATermAppl check_confluence_and_mark(ATermAppl a_invariant, int a_summand_number);
};

#endif
