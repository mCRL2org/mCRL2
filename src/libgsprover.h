#ifdef __cplusplus
extern "C" {
#endif

#include <aterm2.h>

extern bool FindSolutionsError;

typedef void (*FindSolutionsCallBack)(ATermList);

void gsProverInit(ATermAppl Spec, int RewriteStrategy);

ATermList FindSolutions(ATermList Vars, ATerm Expr, FindSolutionsCallBack f);

#ifdef __cplusplus
}
#endif
