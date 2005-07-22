#ifdef __cplusplus
extern "C" {
#endif

#include <aterm2.h>

#define GS_STATE_VECTOR 0
#define GS_STATE_TREE 1

extern bool NextStateError;

typedef void (*gsNextStateCallBack)(ATermAppl, ATerm);

ATerm gsNextStateInit(ATermAppl Spec, bool AllowFreeVars, int StateFormat, int RewriteStrategy);
ATermList gsNextState(ATerm State, gsNextStateCallBack f);
void gsNextStateFinalise();

int gsGetStateLength();
ATermAppl gsGetStateArgument(ATerm State, int index);

#ifdef __cplusplus
}
#endif
