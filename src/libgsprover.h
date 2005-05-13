#ifdef __cplusplus
extern "C" {
#endif

#include "aterm2.h"

void gsProverInit(ATermAppl Spec);

ATermList FindSolutions(ATermList Vars, ATermAppl Expr);

#ifdef __cplusplus
}
#endif
