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
	unsigned int *current_id;
} ns_info;

class NextStateGeneratorStandard : public NextStateGenerator
{
	public:
		NextStateGeneratorStandard(ATerm State, ns_info &Info, unsigned int identifier);
		~NextStateGeneratorStandard();

		bool next(ATermAppl *Transition, ATerm *State, bool *prioritised = NULL);

		bool errorOccurred();
		
		void reset(ATerm State);

		ATerm get_state();

	private:
		ns_info info;
		unsigned int id;

		bool error;

		int sum_idx;

		ATerm cur_state;
		ATerm cur_act;
		ATermList cur_nextstate;

		ATerm *stateargs;

		EnumeratorSolutions *sols;

		void set_substitutions();

		void SetTreeStateVars(ATerm tree, ATermList *vars);
		ATermAppl rewrActionArgs(ATermAppl act);
		ATerm makeNewState(ATerm old, ATermList assigns);
		ATermList ListFromFormat(ATermList l);
};

class NextStateStandard : public NextState
{
	friend class NextStateGeneratorStandard;
	public:
		NextStateStandard(ATermAppl spec, bool allow_free_vars, int state_format, Enumerator *e, bool clean_up_enumerator);
		~NextStateStandard();

		void prioritise(char *action);

		ATerm getInitialState();
		NextStateGenerator *getNextStates(ATerm state, NextStateGenerator *old = NULL);

		int getStateLength();
		ATermAppl getStateArgument(ATerm state, int index);
		ATermAppl makeStateVector(ATerm state);

		Rewriter *getRewriter();

	private:
		ns_info info;
		unsigned int next_id;
		unsigned int current_id;
		
		bool stateAFun_made;

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
		ATermList AssignsToRewriteFormat(ATermList assigns, ATermList free_vars);
};

#endif
