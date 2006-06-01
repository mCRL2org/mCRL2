#include "aterm1.h"

class IProtectedATerm
{
  public:
    virtual void ATprotectTerms() = 0;
    virtual ~IProtectedATerm();
};

void ATprotectProtectedATerm(IProtectedATerm *i);
void ATunprotectProtectedATerm(IProtectedATerm *i);
