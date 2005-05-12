#include <aterm2.h>

#ifdef __cplusplus
extern "C" {
#endif

ATermList gsNextStateInit(ATermAppl Spec, bool AllowFreeVars);
ATermList gsNextState(ATermList State);

#ifdef __cplusplus
}
#endif
