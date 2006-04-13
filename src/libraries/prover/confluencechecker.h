// Interface to class Confluence_Checker
// file: confluencechecker.h

#ifndef CONFLUENCECHECKER_H
#define CONFLUENCECHECKER_H

#include "aterm2.h"
#include "librewrite.h"
#include "bddprover.h"
#include "disjointnesschecker.h"

class Confluence_Checker {
  private:
    Disjointness_Checker f_disjointness_checker;
    BDD_Prover f_bdd_prover;
    ATermAppl f_lpe;
    bool f_no_marking;
    bool f_check_all;
    bool f_counter_example;
    int f_number_of_summands;
    int* f_commutes;
    ATermAppl check_confluence_and_mark_summand(ATermAppl a_invariant, ATermAppl a_summand, int a_summand_number, bool& a_is_marked);
  public:
    Confluence_Checker(
      RewriteStrategy a_rewrite_strategy,
      int a_time_limit,
      ATermAppl a_lpe,
      bool a_no_marking,
      bool a_check_all,
      bool a_counter_example
    );
    ~Confluence_Checker();
    ATermAppl check_confluence_and_mark(ATermAppl a_invariant, int a_summand_number);
};

#endif
