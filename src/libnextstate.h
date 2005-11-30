#include <aterm2.h>
#include "librewrite.h"

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

void NextStateFrom(ATerm State);
bool NextState(ATermAppl *Transition, ATerm *State);
