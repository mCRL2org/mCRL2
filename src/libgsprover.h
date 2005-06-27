#ifdef __cplusplus
extern "C" {
#endif

#include <aterm2.h>

extern bool FindSolutionsError;

void gsProverInit(ATermAppl Spec);

ATermList FindSolutions(ATermList Vars, ATermAppl Expr);

ATermList FindSolutionsWithSolutions(ATermList Vars, ATermAppl Expr, ATermTable Substs);

#ifdef __cplusplus
}
#endif
