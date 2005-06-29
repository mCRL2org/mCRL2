#ifdef __cplusplus
extern "C" {
#endif

#include <aterm2.h>

extern bool NextStateError;

typedef void (*gsNextStateCallBack)(ATermAppl, ATerm);

ATermList gsNextStateInit(ATermAppl Spec, bool AllowFreeVars);
ATermList gsNextState(ATermList State, gsNextStateCallBack f);

#ifdef __cplusplus
}
#endif
