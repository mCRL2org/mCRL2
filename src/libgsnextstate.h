#ifdef __cplusplus
extern "C" {
#endif

#include <aterm2.h>
#include "libgsrewrite.h"

#define GS_STATE_VECTOR 0
#define GS_STATE_TREE 1

extern bool NextStateError;

typedef void (*gsNextStateCallBack)(ATermAppl, ATerm);

ATerm gsNextStateInit(ATermAppl Spec, bool AllowFreeVars, int StateFormat, RewriteStrategy strat);
ATermList gsNextState(ATerm State, gsNextStateCallBack f);
void gsNextStateFinalise();

int gsGetStateLength();
ATermAppl gsGetStateArgument(ATerm State, int index);
ATermAppl gsMakeStateVector(ATerm State);

#ifdef __cplusplus
}
#endif
