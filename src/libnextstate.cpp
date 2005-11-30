#include <stdio.h>
#include <stdlib.h>
#include <aterm2.h>
#include "liblowlevel.h"
#include "libstruct.h"
#include "libprint_c.h"
#include "libenum.h"
#include "librewrite.h"
#include "libnextstate.h"

#define ATAgetFirst(x) ((ATermAppl) ATgetFirst(x))
#define ATLgetFirst(x) ((ATermList) ATgetFirst(x))
#define ATAgetArgument(x,y) ((ATermAppl) ATgetArgument(x,y))
#define ATLgetArgument(x,y) ((ATermList) ATgetArgument(x,y))

bool NextStateError;

static bool ATisList(ATerm a)
{
	return (ATgetType(a) == AT_LIST);
}


static ATermAppl current_spec;
static int stateformat;
static int statelen;
static AFun stateAFun;
static AFun pairAFun;
static ATerm *stateargs;
static int num_summands;
static ATermAppl *summands;
static AFun smndAFun;
static ATermList procvars;
static ATermList pars;
static ATermAppl nil;

static Rewriter *rewr_obj;
static Enumerator *enum_obj;

static int sum_idx;
static ATerm _state;
static ATerm _act;
static ATerm _nextstate;

static bool usedummies;

static bool *tree_init = NULL;
static void fill_tree_init(bool *init, int n, int l)
{
	if ( l == 0 )
		return;

	if ( n > 2 )
	{
		fill_tree_init(init,n/2,l/2);
		fill_tree_init(init+n/2,n-n/2,l-l/2);
	} else /* n == 2 */ {
		init[1] = true;
	}
}
static ATerm buildTree(ATerm *args)
{
	int n,m;

	if ( statelen == 0 )
		return (ATerm) nil;

	if ( tree_init == NULL )
	{
		tree_init = (bool *) malloc(statelen*sizeof(bool));
		for (int i=0; i<statelen; i++)
			tree_init[i] = false;
		n = 1;
		while ( n < statelen )
		{
			n *= 2;
		}
		n /= 2;
		fill_tree_init(tree_init,statelen,statelen-n);
	}

	n = 0;
	m = 0;
	while ( n < statelen )
	{
		if ( tree_init[n] )
		{
			args[m-1] = (ATerm) ATmakeAppl2(pairAFun,args[m-1],args[n]);
		} else {
			args[m] = args[n];
			m++;
		}
		n++;
	}

	n = m;
	while ( n > 1 )
	{
		for (int i=0; i<n; i+=2)
		{
			args[i/2] = (ATerm) ATmakeAppl2(pairAFun,args[i],args[i+1]);
		}
		
		n /= 2;
	}
	
	return args[0];
}

static ATerm getTreeElement(ATerm tree, int index)
{
	int n = statelen;
	int m = 0;
	int t = n/2;

	while ( 1 )
	{
		if ( index < t )
		{
			tree = ATgetArgument((ATermAppl) tree,0);
			n = t;
		} else {
			tree = ATgetArgument((ATermAppl) tree,1);
			m = t;
		}

		if ( (t == 1) || (t == index) )
			break;

		t = (n+m)/2;
	}
	while ( (ATgetType(tree) == AT_APPL) && ATisEqualAFun(ATgetAFun((ATermAppl) tree),pairAFun) )
	{
		tree = ATgetArgument((ATermAppl) tree,0);
	}
	return tree;
}

int gsGetStateLength()
{
//	return ATgetLength(procvars);
	return statelen;
}

ATermAppl gsGetStateArgument(ATerm state, int index)
{
	switch ( stateformat )
	{
		case GS_STATE_VECTOR:
			return rewr_obj->fromRewriteFormat(ATgetArgument((ATermAppl) state,index));
		case GS_STATE_TREE:
			return rewr_obj->fromRewriteFormat(getTreeElement(state,index));
		default:
			return NULL;
	}
}

/*ATermAppl gsGetStateArgument(ATerm state, int index)
{
	if ( (0 <= index) && (index < ATgetLength((ATermList) state)) )
	{
		return rewr_obj->fromRewriteFormat(ATelementAt((ATermList) state,index));
	} else {
		return NULL;
	}
}*/

ATermAppl gsMakeStateVector(ATerm state)
{
	// XXX can be done more efficiently in some cases
	for (int i=0; i<statelen; i++)
	{
		stateargs[i] = (ATerm) gsGetStateArgument(state,i);
	}
	return ATmakeApplArray(stateAFun,stateargs);
}

ATermAppl FindDummy(ATermAppl sort)
{
	ATermList l;

	if ( gsIsSortArrow(sort) )
	{
		gsErrorMsg("cannot generate dummies for function sorts (%T)\n",sort);
		exit(1);
	}

	l = ATLgetArgument(ATAgetArgument(current_spec,1),0);
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		ATermAppl conssort = ATAgetArgument(ATAgetFirst(l),1);
		if ( ATisEqual(gsGetSortExprResult(conssort),sort) )
		{
			ATermList domain = gsGetSortExprDomain(conssort);
			ATermAppl t = ATAgetFirst(l);

			if ( ATindexOf(domain,(ATerm) sort,0) == -1)
			{
				for (; !ATisEmpty(domain); domain=ATgetNext(domain))
				{
					t = gsMakeDataAppl(t,FindDummy(ATAgetFirst(domain)));
				}

				return t;
			}
		}
	}

	l = ATLgetArgument(ATAgetArgument(current_spec,2),0);
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		ATermAppl mapsort = ATAgetArgument(ATAgetFirst(l),1);
		if ( ATisEqual(gsGetSortExprResult(mapsort),sort) )
		{
			ATermList domain = gsGetSortExprDomain(mapsort);
			ATermAppl t = ATAgetFirst(l);

			for (; !ATisEmpty(domain); domain=ATgetNext(domain))
			{
				t = gsMakeDataAppl(t,FindDummy(ATAgetFirst(domain)));
			}

			return t;
		}
	}

	gsfprintf(stderr,"Could not find dummy of type %T\n",sort);
	exit(1);
}

ATerm SetVars(ATerm a, ATermList free_vars)
{
	ATermList l,m;

	if ( !usedummies )
	{
		return a;
	}

	if ( ATisList(a) )
	{
		l = (ATermList) a;
		m = ATmakeList0();
		for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			m = ATinsert(m,SetVars(ATgetFirst(l),free_vars));
		}
		return (ATerm) ATreverse(m);
	} else if ( gsIsDataVarId((ATermAppl) a) )
	{
		if ( ATindexOf(free_vars,a,0) >= 0 )
		{
			return (ATerm) FindDummy(ATAgetArgument((ATermAppl) a,1));
		} else {
			return a;
		}
	} else if ( gsIsDataAppl((ATermAppl) a) )
	{
		return (ATerm) gsMakeDataAppl((ATermAppl) SetVars(ATgetArgument((ATermAppl) a,0),free_vars),(ATermAppl) SetVars(ATgetArgument((ATermAppl) a,1),free_vars));
	} else {
		return a;
	}
}

ATermAppl smd_subst_vars(ATermAppl smd, ATermList vars)
{
	ATermList l,m,a1,a5;
	ATermAppl a2,a3,a4;

	l = vars; 
	m = ATLgetArgument(smd,0);
	vars = ATmakeList0();
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		if ( ATindexOf(m,ATgetArgument(ATAgetFirst(l),0),0) == -1 )
		{
			vars = ATinsert(vars,ATgetFirst(l));
		}
	}
	vars = ATreverse(vars);

	a1 = ATLgetArgument(smd,0);
	a2 = gsSubstValues_Appl(vars,ATAgetArgument(smd,1),true);
	a3 = gsSubstValues_Appl(vars,ATAgetArgument(smd,2),true);
	a4 = gsSubstValues_Appl(vars,ATAgetArgument(smd,3),true);

	l = ATLgetArgument(smd,4);
	a5 = ATmakeList0();
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		a5 = ATinsert(a5,(ATerm) ATsetArgument(ATAgetFirst(l),gsSubstValues(vars,ATgetArgument(ATAgetFirst(l),1),true),1));
	}
	a5 = ATreverse(a5);

	return gsMakeLPESummand(a1,a2,a3,a4,a5);
}

ATermList ListToFormat(ATermList l,ATermList free_vars)
{
	if ( ATisEmpty(l) )
	{
		return l;
	} else {
		return ATinsert(ListToFormat(ATgetNext(l),free_vars),rewr_obj->toRewriteFormat((ATermAppl) SetVars(ATgetFirst(l),free_vars)));
	}
}

ATermList ListFromFormat(ATermList l)
{
	if ( ATisEmpty(l) )
	{
		return l;
	} else {
		return ATinsert(ListFromFormat(ATgetNext(l)),(ATerm) rewr_obj->fromRewriteFormat(ATgetFirst(l)));
	}
}

ATermAppl ActionToRewriteFormat(ATermAppl act, ATermList free_vars)
{
	ATermList l = ATLgetArgument(act,0);
	ATermList m = ATmakeList0();

	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		ATermAppl a = ATAgetFirst(l);
		a = gsMakeAction(ATAgetArgument(a,0),ListToFormat(ATLgetArgument(a,1),free_vars));
		m = ATinsert(m,(ATerm) a);
	}
	m = ATreverse(m);

	return gsMakeMultAct(m);
}

ATerm AssignsToRewriteFormat(ATermList assigns, ATermList free_vars)
{
/*	ATermList l = ATmakeList0();
	for (; !ATisEmpty(assigns); assigns=ATgetNext(assigns))
	{
		ATermAppl a = ATAgetFirst(assigns);
		l = ATinsert(l,(ATerm) ATmakeAppl2(ATgetAFun(a),rewr_obj->toRewriteFormat(ATAgetArgument(a,0)),rewr_obj->toRewriteFormat(ATAgetArgument(a,1))));
	}
	return ATreverse(l); */

	int i = 0;
	for (ATermList l=pars; !ATisEmpty(l); l=ATgetNext(l),i++)
	{
		bool set = false;

		for (ATermList m=assigns; !ATisEmpty(m); m=ATgetNext(m))
		{
			if ( ATisEqual(ATAgetArgument(ATAgetFirst(m),0),ATAgetFirst(l)) )
			{
				stateargs[i] = rewr_obj->toRewriteFormat((ATermAppl) SetVars(ATgetArgument(ATAgetFirst(m),1),free_vars));
				set = true;
				break;
			}
		}
		if ( !set )
		{
			stateargs[i] = (ATerm) gsMakeNil();
		}
	}

	return (ATerm) ATmakeApplArray(stateAFun,stateargs);
}

ATerm gsNextStateInit(ATermAppl Spec, bool AllowFreeVars, int StateFormat, RewriteStrategy strat)
{
	ATermList l,m,n,free_vars;
	bool set;

	current_spec = Spec;
	ATprotectAppl(&current_spec);
	usedummies = !AllowFreeVars;

	stateformat = StateFormat;
	pairAFun = ATmakeAFun("@STATE_PAIR@",2,ATfalse);
	ATprotectAFun(pairAFun);

	nil = gsMakeNil();
	ATprotectAppl(&nil);

	rewr_obj = createRewriter(ATAgetArgument(Spec,3),strat);
	enum_obj = createEnumerator(Spec,rewr_obj);

	_state = NULL;
	_act = NULL;
	_nextstate = NULL;
	ATprotect(&_state);
	ATprotect(&_act);
	ATprotect(&_nextstate);

	free_vars = ATLgetArgument(ATAgetArgument(current_spec,5),0);

	pars = ATLgetArgument(ATAgetArgument(current_spec,5),1);
	ATprotectList(&pars);

	statelen = ATgetLength(pars);
	stateAFun = ATmakeAFun("STATE",statelen,ATfalse);
	ATprotectAFun(stateAFun);
	stateargs = (ATerm *) malloc(statelen*sizeof(ATerm));
	for (int i=0; i<statelen; i++)
	{
		stateargs[i] = NULL;
	}
	ATprotectArray(stateargs,statelen);

	procvars = ATLgetArgument(ATAgetArgument(current_spec,5),1);
	ATprotectList(&procvars);
	
	smndAFun = ATmakeAFun("@SMND@",4,ATfalse);
	ATprotectAFun(smndAFun);
	ATermList sums = ATLgetArgument(ATAgetArgument(current_spec,5),2);
	l = ATmakeList0();
	for (bool b=true; !ATisEmpty(sums); sums=ATgetNext(sums))
	{
		if ( b && !gsIsNil(ATAgetArgument(ATAgetFirst(sums),3)) )
		{
			fprintf(stderr,"warning: specification uses time, which is (currently) not supported; ignoring timing\n");
			b = false;
		}
		if ( !gsIsDelta(ATAgetArgument(ATAgetFirst(sums),2)) )
		{
			l = ATinsert(l,ATgetFirst(sums));
		}
	}
	sums = ATreverse(l);
	num_summands = ATgetLength(sums);
	summands = (ATermAppl *) malloc(num_summands*sizeof(ATermAppl));
	for (int i=0; i<num_summands; i++)
	{
		summands[i] = NULL;
	}
	ATprotectArray((ATerm *) summands,num_summands);
	for (int i=0; !ATisEmpty(sums); sums=ATgetNext(sums),i++)
	{
		summands[i] = ATmakeAppl4(smndAFun,ATgetArgument(ATAgetFirst(sums),0),rewr_obj->toRewriteFormat((ATermAppl) SetVars(ATgetArgument(ATAgetFirst(sums),1),free_vars)),(ATerm) ActionToRewriteFormat(ATAgetArgument(ATAgetFirst(sums),2),free_vars),(ATerm) AssignsToRewriteFormat(ATLgetArgument(ATAgetFirst(sums),4),free_vars));
	}

	l = pars;
	free_vars = ATLgetArgument(ATAgetArgument(Spec,6),0);
	m = ATLgetArgument(ATAgetArgument(Spec,6),1);

	for (int i=0; !ATisEmpty(l); l=ATgetNext(l), i++)
	{
		n = m;
		set = false;
		for (; !ATisEmpty(n); n=ATgetNext(n))
		{
			if ( ATisEqual(ATAgetArgument(ATAgetFirst(n),0),ATAgetFirst(l)) )
			{
				stateargs[i] = rewr_obj->rewriteInternal(rewr_obj->toRewriteFormat((ATermAppl) SetVars(ATgetArgument(ATAgetFirst(n),1),free_vars)));
				set = true;
				break;
			}
		}
		if ( !set )
		{
			gsErrorMsg("Parameter '%T' does not have an initial value.",ATgetArgument(ATAgetFirst(l),0));
			return NULL;
		}
	}

	switch ( stateformat )
	{
		case GS_STATE_VECTOR:
			return (ATerm) ATmakeApplArray(stateAFun,stateargs);
		case GS_STATE_TREE:
			return (ATerm) buildTree(stateargs);
		default:
			return NULL;
	}
}

void gsNextStateFinalise()
{
	ATunprotectAppl(&current_spec);
	ATunprotectAppl(&nil);

	ATprotect(&_nextstate);
	ATprotect(&_act);
	ATprotect(&_state);

	delete enum_obj;
	delete rewr_obj;
	
	ATunprotectAFun(pairAFun);

	ATunprotectList(&pars);
	ATunprotectAFun(stateAFun);
	ATunprotectArray(stateargs);

	ATunprotectList(&procvars);
	
	ATunprotectAFun(smndAFun);
	ATunprotectArray((ATerm *) summands);
	free(summands);

	if ( tree_init != NULL )
	{
		free(tree_init);
		tree_init = NULL;
	}
}

static ATerm makeNewState(ATerm old, ATerm assigns)
{
	ATermList l;

	l = procvars;
	for (int i=0; i<statelen; i++)
	{
		ATerm a = ATgetArgument((ATermAppl) assigns,i);
		if ( ATisEqual(a,nil) )
		{
			switch ( stateformat )
			{
				case GS_STATE_VECTOR:
					stateargs[i] = ATgetArgument((ATermAppl) old,i);
					break;
				case GS_STATE_TREE:
//					stateargs[i] = getTreeElement(old,i);
					stateargs[i] = rewr_obj->getSubstitution((ATermAppl) ATgetFirst(l));
					break;
			}
		} else {
			stateargs[i] = rewr_obj->rewriteInternal(a);
//			stateargs[i] = rewr_obj->rewriteInternal(SetVars(a));
		}
		l = ATgetNext(l);
	}
	switch ( stateformat )
	{
		case GS_STATE_VECTOR:
			return (ATerm) ATmakeApplArray(stateAFun,stateargs);
		case GS_STATE_TREE:
			return buildTree(stateargs);
		default:
			return NULL;
	}
}

ATermAppl rewrActionArgs(ATermAppl act)
{
	ATermList l = ATLgetArgument(act,0);
	ATermList m = ATmakeList0();

	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		ATermAppl a = ATAgetFirst(l);
		a = gsMakeAction(ATAgetArgument(a,0),ListFromFormat(rewr_obj->rewriteInternalList(ATLgetArgument(a,1))));
		m = ATinsert(m,(ATerm) a);
	}
	m = ATreverse(m);

	return gsMakeMultAct(m);
}

static void SetTreeStateVars(ATerm tree, ATermList *vars)
{
	if ( (ATgetType(tree) == AT_APPL) && ATisEqualAFun(ATgetAFun((ATermAppl) tree),pairAFun) )
	{
		SetTreeStateVars(ATgetArgument((ATermAppl) tree,0),vars);
		SetTreeStateVars(ATgetArgument((ATermAppl) tree,1),vars);
	} else {
		rewr_obj->setSubstitution((ATermAppl) ATgetFirst(*vars),tree);
		*vars = ATgetNext(*vars);
	}
}

ATermList gsNextState(ATerm State, gsNextStateCallBack f)
{
	NextStateFrom(State);

	ATermList r = ATmakeList0();

	ATermAppl Transition;
	ATerm NewState;
	while ( NextState(&Transition,&NewState) )
	{
		if ( f == NULL )
		{
			r = ATinsert(r,(ATerm) ATmakeList2((ATerm) Transition,NewState));
		} else {
			f(Transition,NewState);
		}
	}

	return r;
}


void NextStateFrom(ATerm State)
{
	NextStateError = false;

	ATermList l = procvars;
	switch ( stateformat )
	{
		case GS_STATE_VECTOR:
			for (int i=0; !ATisEmpty(l); l=ATgetNext(l),i++)
			{
				ATerm a = ATgetArgument((ATermAppl) State,i);

				if ( !ATisEqual(a,nil) )
				{
					rewr_obj->setSubstitution((ATermAppl) ATgetFirst(l),a);
				}
			}
			break;
		case GS_STATE_TREE:
			SetTreeStateVars(State,&l);
			break;
	}

	_state = State;

	if ( num_summands == 0 )
	{
		enum_obj->initialise(ATmakeList0(),rewr_obj->toRewriteFormat(gsMakeDataExprFalse()));
	} else {
		_act = ATgetArgument(summands[0],2);
		_nextstate = ATgetArgument(summands[0],3);
		enum_obj->initialise(ATLgetArgument(summands[0],0),ATgetArgument(summands[0],1));
	}
	sum_idx = 1;
}

bool NextState(ATermAppl *Transition, ATerm *State)
{
	ATermList sol;

	while ( !enum_obj->next(&sol) && !NextStateError && (sum_idx < num_summands) )
	{
		NextStateError |= enum_obj->errorOccurred();
		if ( !NextStateError )
		{
			_act = ATgetArgument(summands[sum_idx],2);
			_nextstate = ATgetArgument(summands[sum_idx],3);

			enum_obj->initialise(ATLgetArgument(summands[sum_idx],0),ATgetArgument(summands[sum_idx],1));
		
			sum_idx++;
		}
	}
	NextStateError |= enum_obj->errorOccurred();

	if ( sol != NULL )
	{
		for (ATermList m=sol; !ATisEmpty(m); m=ATgetNext(m))
		{
			rewr_obj->setSubstitution((ATermAppl) ATgetArgument((ATermAppl) ATgetFirst(m),0),ATgetArgument((ATermAppl) ATgetFirst(m),1));
		}
		*Transition = rewrActionArgs((ATermAppl) _act);
		*State = (ATerm) makeNewState(_state,_nextstate);
		for (ATermList m=sol; !ATisEmpty(m); m=ATgetNext(m))
		{
			rewr_obj->clearSubstitution((ATermAppl) ATgetArgument((ATermAppl) ATgetFirst(m),0));
		}
		return true;
	} else {
		*Transition = NULL;
		*State = NULL;
		return false;
	}
}
