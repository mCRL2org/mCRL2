#ifdef __cplusplus
extern "C" {
#endif

#include <aterm2.h>

ATermList gsNextStateInit(ATermAppl Spec, bool AllowFreeVars);
ATermList gsNextState(ATermList State);

#ifdef __cplusplus
}
#endif
