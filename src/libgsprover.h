#ifdef __cplusplus
extern "C" {
#endif

#include <aterm2.h>

extern bool FindSolutionsError;

typedef void (*FindSolutionsCallBack)(ATermList);

void gsProverInit(ATermAppl Spec);

ATermList FindSolutions(ATermList Vars, ATermAppl Expr, FindSolutionsCallBack f);

ATermList FindSolutionsWithSubsts(ATermList Vars, ATermAppl Expr, ATermTable Substs, FindSolutionsCallBack f);

#ifdef __cplusplus
}
#endif
