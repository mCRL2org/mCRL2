#ifdef __cplusplus
extern "C" {
#endif

#include <aterm2.h>

extern bool NextStateError;

typedef void (*gsNextStateCallBack)(ATermAppl, ATerm);

ATerm gsNextStateInit(ATermAppl Spec, bool AllowFreeVars);
ATermList gsNextState(ATerm State, gsNextStateCallBack f);

int gsGetStateLength();
ATermAppl gsGetStateArgument(ATerm State, int index);

#ifdef __cplusplus
}
#endif
