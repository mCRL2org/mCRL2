// Interface to class BDD_Prover
// file: bdd_prover.h

#ifndef BDD_PROVER_H
#define BDD_PROVER_H

#include "aterm2.h"
#include "librewrite.h"
#include "prover.h"
#include "bdd_simplifier.h"
#include "bdd_path_eliminator.h"

class BDD_Prover: public Prover {
  private:
    bool f_reverse;
    bool f_full;
    ATermTable f_formula_to_bdd;
    ATermTable f_smallest;
    BDD_Info f_bdd_info;
    BDD_Simplifier* f_bdd_simplifier;
    void build_bdd();
    ATerm bdd_down(ATerm a_formula, int a_indent);
    void update_answers();
    ATerm smallest(ATerm a_formula);
    ATermAppl get_branch(ATermAppl a_bdd, bool a_polarity);
  protected:
    ATerm f_internal_bdd;
    ATermAppl f_bdd;
  public:
    BDD_Prover(
      ATermAppl a_equations,
      RewriteStrategy a_rewrite_strategy = GS_REWR_JITTY,
      int a_time_limit = 0,
      bool a_path_eliminator = false,
      SMT_Solver_Type a_solver_type = solver_type_ario,
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
