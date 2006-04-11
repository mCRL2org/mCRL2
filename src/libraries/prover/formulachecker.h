// Interface to class Formula_Checker
// file: formulachecker.h

#ifndef FORMULACHECKER_H
#define FORMULACHECKER_H

#include "aterm2.h"
#include "librewrite.h"
#include "bddprover.h"

class Formula_Checker {
  private:
    BDD_Prover f_bdd_prover;
    bool f_counter_example;
    bool f_witness;
  public:
    Formula_Checker(
      RewriteStrategy a_rewrite_strategy,
      int a_time_limit,
      ATermAppl a_data_equations,
      bool a_counter_example,
      bool a_witness
    );
    ~Formula_Checker();
    void check_formulas(ATermList a_formulas);
};

#endif
