#ifdef __cplusplus
extern "C" {
#endif

#include <aterm2.h>

extern bool NextStateError;

ATermList gsNextStateInit(ATermAppl Spec, bool AllowFreeVars);
ATermList gsNextState(ATermList State);

#ifdef __cplusplus
}
#endif
