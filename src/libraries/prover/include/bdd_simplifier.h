// Interface to class BDD_Simplifier
// file: bdd_simplifier.h

#ifndef BDD_SIMPLIFIER_H
#define BDD_SIMPLIFIER_H

#include "aterm2.h"

class BDD_Simplifier {
  protected:
    int f_deadline;
  public:
    virtual ~BDD_Simplifier();
    void set_time_limit(int a_time_limit);
    virtual ATermAppl simplify(ATermAppl a_bdd);
};

#endif
