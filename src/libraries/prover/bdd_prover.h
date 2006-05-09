// Interface to class BDD_Prover
// file: bdd_prover.h

#ifndef BDD_PROVER_H
#define BDD_PROVER_H

#include "aterm2.h"
#include "librewrite.h"
#include "prover.h"

class BDD_Prover: public Prover {
  private:
    ATermTable f_formula_to_bdd;
    ATermTable f_smallest;
    void build_bdd();
    ATerm bdd_down(ATerm a_formula, int a_indent);
    void update_answers();
    ATerm smallest(ATerm a_formula);
    ATerm smallest_recursive(ATerm a_formula);
    ATermAppl aux_get_branch(ATerm a_bdd, bool a_polarity);
  protected:
    ATerm f_bdd;
    bool f_reverse;
    bool f_full;
  public:
    BDD_Prover(
      ATermAppl a_equations,
      RewriteStrategy a_rewrite_strategy = GS_REWR_JITTY,
      int a_time_limit = 0,
      bool a_reverse = true,
      bool a_full = true
    );
    virtual ~BDD_Prover();
    virtual Answer is_tautology();
    virtual Answer is_contradiction();
    virtual ATermAppl get_bdd();
    virtual ATermAppl get_witness();
    virtual ATermAppl get_counter_example();
};

#endif
