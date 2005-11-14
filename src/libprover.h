#include <aterm2.h>
#include "librewrite_c.h"

extern bool FindSolutionsError;

typedef void (*FindSolutionsCallBack)(ATermList);

void gsProverInit(ATermAppl Spec, RewriteStrategy strat);
void gsProverFinalise();

ATermList FindSolutions(ATermList Vars, ATerm Expr, FindSolutionsCallBack f);
