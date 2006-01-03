#ifndef _NEXTSTATE_STANDARD_H
#define _NEXTSTATE_STANDARD_H

#include <aterm2.h>
#include "libnextstate.h"
#include "libenum.h"

class NextStateStandard;

typedef struct {
	NextStateStandard *parent;
	Enumerator *enum_obj;
	Rewriter *rewr_obj;

	int num_summands;
	ATermAppl *summands;
	int num_prioritised;
	ATermList procvars;
	int stateformat;
	ATermAppl nil;
	AFun pairAFun;
	int statelen;
	AFun stateAFun;
} ns_info;

class NextStateGeneratorStandard : public NextStateGenerator
{
	public:
		NextStateGeneratorStandard(ATerm State, ns_info &Info);
		~NextStateGeneratorStandard();

		bool next(ATermAppl *Transition, ATerm *State, bool *prioritised = NULL);

		bool errorOccurred();
		
		void reset(ATerm State);

	private:
		ns_info info;

		bool error;

		int sum_idx;

		ATerm cur_state;
		ATerm cur_act;
		ATerm cur_nextstate;

		ATerm *stateargs;

		EnumeratorSolutions *sols;

		void SetTreeStateVars(ATerm tree, ATermList *vars);
		ATermAppl rewrActionArgs(ATermAppl act);
		ATerm makeNewState(ATerm old, ATerm assigns);
		ATermList ListFromFormat(ATermList l);
};

class NextStateStandard : public NextState
{
	friend class NextStateGeneratorStandard;
	public:
		NextStateStandard(ATermAppl spec, bool allow_free_vars, int state_format, Enumerator *e, bool clean_up_enumerator, NextStateStrategy strategy = nsStandard);
		~NextStateStandard();

		void prioritise(char *action);

		ATerm getInitialState();
		NextStateGenerator *getNextStates(ATerm state, NextStateGenerator *old = NULL);

		int getStateLength();
		ATermAppl getStateArgument(ATerm state, int index);
		ATermAppl makeStateVector(ATerm state);

	private:
		ns_info info;

		bool clean_up_enum_obj;
		bool usedummies;

		AFun smndAFun;
		bool *tree_init;
		ATerm *stateargs;

		ATermAppl current_spec;
		ATermList pars;
		ATerm initial_state;

		ATerm buildTree(ATerm *args);
		ATerm getTreeElement(ATerm tree, int index);

		ATermAppl FindDummy(ATermAppl sort, ATermList no_dummy = ATempty);
		ATerm SetVars(ATerm a, ATermList free_vars);
		ATermList ListToFormat(ATermList l,ATermList free_vars);
		ATermList ListFromFormat(ATermList l);
		ATermAppl ActionToRewriteFormat(ATermAppl act, ATermList free_vars);
		ATerm AssignsToRewriteFormat(ATermList assigns, ATermList free_vars);
};

#endif
