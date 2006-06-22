// Interface to class BDD_Info
// file: bdd_info.h

#ifndef BDD_INFO_H
#define BDD_INFO_H

#include "aterm2.h"

class BDD_Info {
  protected:
    ATermAppl f_if_then_else;
  public:
    BDD_Info();
    ATermAppl get_guard(ATermAppl a_bdd);
    ATermAppl get_true_branch(ATermAppl a_bdd);
    ATermAppl get_false_branch(ATermAppl a_bdd);
    bool is_true(ATermAppl a_bdd);
    bool is_false(ATermAppl a_bdd);
    bool is_if_then_else(ATermAppl a_bdd);
};

#endif
