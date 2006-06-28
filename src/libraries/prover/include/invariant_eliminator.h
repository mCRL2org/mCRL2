// Interface to class Invariant_Eliminator
// file: invariant_eliminator.h

#ifndef INVARIANT_ELIMINATOR_H
#define INVARIANT_ELIMINATOR_H

#include "aterm2.h"
#include "librewrite.h"
#include "bdd_prover.h"

class Invariant_Eliminator {
  private:
    BDD_Prover f_bdd_prover;
    ATermAppl f_lpe;
    bool f_simplify_all;
    ATermAppl simplify_summand(ATermAppl a_invariant, ATermAppl a_summand, int a_summand_number);
  public:
    Invariant_Eliminator(
      ATermAppl a_lpe,
      RewriteStrategy a_rewrite_strategy = GS_REWR_JITTY,
      int a_time_limit = 0,
      bool a_path_eliminator = false,
      SMT_Solver_Type a_solver_type = solver_type_ario,
      bool a_simplify_all = false
    );
    ~Invariant_Eliminator();
    ATermAppl simplify(ATermAppl a_invariant, int a_summand_number);
};

#endif
