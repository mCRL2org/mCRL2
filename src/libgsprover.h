#ifdef __cplusplus
extern "C" {
#endif

#include <aterm2.h>
#include "libgsrewrite.h"

extern bool FindSolutionsError;

typedef void (*FindSolutionsCallBack)(ATermList);

void gsProverInit(ATermAppl Spec, RewriteStrategy strat);
void gsProverFinalise();

ATermList FindSolutions(ATermList Vars, ATerm Expr, FindSolutionsCallBack f);

#ifdef __cplusplus
}
#endif
