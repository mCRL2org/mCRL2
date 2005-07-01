#ifdef __cplusplus
extern "C" {
#endif

#include <aterm2.h>

extern bool FindSolutionsError;

typedef void (*FindSolutionsCallBack)(ATermList);

void gsProverInit(ATermAppl Spec);

ATermList FindSolutions(ATermList Vars, ATerm Expr, FindSolutionsCallBack f);

ATermList FindSolutionsWithSubsts(ATermList Vars, ATerm Expr, ATermTable Substs, FindSolutionsCallBack f);

#ifdef __cplusplus
}
#endif
