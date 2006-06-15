#ifndef _LIBNEXTSTATE_H
#define _LIBNEXTSTATE_H

#include <aterm2.h>
#include "libenum.h"
#include "librewrite.h"

#define GS_STATE_VECTOR 0
#define GS_STATE_TREE 1

enum NextStateStrategy { nsStandard };

class NextStateGenerator
{
	public:
		virtual ~NextStateGenerator();

		virtual bool next(ATermAppl *Transition, ATerm *State, bool *prioritised = NULL) = 0;
		virtual bool errorOccurred() = 0;

		virtual ATerm get_state() = 0;
};

class NextState
{
	public:
		virtual ~NextState();

		virtual void prioritise(char *action) = 0;

		virtual ATerm getInitialState() = 0;
		virtual NextStateGenerator *getNextStates(
					ATerm state,
					NextStateGenerator *old = NULL
					) = 0;

		virtual int getStateLength() = 0;
		virtual ATermAppl getStateArgument(ATerm state, int index) = 0;
		virtual ATermAppl makeStateVector(ATerm state) = 0;

		virtual Rewriter *getRewriter() = 0;
};

NextState *createNextState(
		ATermAppl spec,
		bool allow_free_vars,
		int state_format,
		Enumerator *e,
		bool clean_up_enumerator = false,
		NextStateStrategy strategy = nsStandard
		);

NextState *createNextState(
		ATermAppl spec,
		bool allow_free_vars = true,
		int state_format = GS_STATE_VECTOR,
		RewriteStrategy rewrite_strategy = GS_REWR_INNER,
		EnumerateStrategy enumerator_strategy = ENUM_STANDARD,
		NextStateStrategy strategy = nsStandard
		);

#endif
