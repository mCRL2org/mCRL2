// Interface to class BDD_Simplifier
// file: bdd_simplifier.h

#ifndef BDD_SIMPLIFIER_H
#define BDD_SIMPLIFIER_H

#include "aterm2.h"

class BDD_Simplifier {
  public:
    virtual ~BDD_Simplifier();
    virtual ATermAppl simplify(ATermAppl a_bdd);
};

#endif
