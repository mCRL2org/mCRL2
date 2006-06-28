// Interface to class Prover
// file: prover.h

#ifndef PROVER_H
#define PROVER_H

#include "aterm2.h"
#include "librewrite.h"
#include "manipulator.h"
#include "info.h"
#include "utilities.h"
#include "time.h"

enum Answer {
  answer_yes,
  answer_no,
  answer_undefined
};

class Prover {
  protected:
    ATermAppl f_formula;
    Rewriter* f_rewriter;
    ATerm_Manipulator* f_manipulator;
    ATerm_Info* f_info;
    bool f_processed;
    Answer f_tautology;
    Answer f_contradiction;
    int f_time_limit;
    time_t f_deadline;
  public:
    Prover(
      ATermAppl a_equations,
      RewriteStrategy a_rewrite_strategy = GS_REWR_JITTY,
      int a_time_limit = 0
    );
    virtual ~Prover();
    void set_formula(ATermAppl a_formula);
    void set_time_limit(int a_time_limit);
    virtual Answer is_tautology() = 0;
    virtual Answer is_contradiction() = 0;
    virtual ATermAppl get_witness() = 0;
    virtual ATermAppl get_counter_example() = 0;
};

#endif
