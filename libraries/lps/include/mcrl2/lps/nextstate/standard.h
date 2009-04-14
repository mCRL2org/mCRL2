// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/nextstate/standard.h

#ifndef _NEXTSTATE_STANDARD_H
#define _NEXTSTATE_STANDARD_H

#include <aterm2.h>
#include <mcrl2/lps/nextstate.h>
#include <mcrl2/data/enum.h>

class NextStateStandard;

struct ns_info {
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
};

class NextStateGeneratorStandard : public NextStateGenerator
{
	public:
		NextStateGeneratorStandard(ATerm State, ns_info &Info, unsigned int identifier, bool SingleSummand = false);
		~NextStateGeneratorStandard();

		bool next(ATermAppl *Transition, ATerm *State, bool *prioritised = NULL);

		bool errorOccurred();

		void reset(ATerm State, size_t SummandIndex = 0);

		ATerm get_state() const;

	private:
		ns_info info;
		unsigned int id;

		bool error;
                bool single_summand;

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

		void prioritise(const char *action);

		ATerm getInitialState();
		NextStateGenerator *getNextStates(ATerm state, NextStateGenerator *old = NULL);
		NextStateGenerator *getNextStates(ATerm state, int group, NextStateGenerator *old = NULL);

                void gatherGroupInformation();

		int getGroupCount() const;
		int getStateLength();
		ATermAppl getStateArgument(ATerm state, int index);
		ATermAppl makeStateVector(ATerm state);
		ATerm parseStateVector(ATermAppl state, ATerm match = NULL);

		Rewriter *getRewriter();
		Enumerator* getEnumerator();

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
