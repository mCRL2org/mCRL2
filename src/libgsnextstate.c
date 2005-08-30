#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <aterm2.h>
#include "gslowlevel.h"
#include "gsfunc.h"
#include "libgsprover.h"
#include "libgsrewrite.h"
#include "libgsnextstate.h"
#include "gssubstitute.h"

#define ATAgetFirst(x) ((ATermAppl) ATgetFirst(x))
#define ATLgetFirst(x) ((ATermList) ATgetFirst(x))
#define ATAgetArgument(x,y) ((ATermAppl) ATgetArgument(x,y))
#define ATLgetArgument(x,y) ((ATermList) ATgetArgument(x,y))

bool NextStateError;

bool ATisList(ATerm a)
{
	return (ATgetType(a) == AT_LIST);
}

static ATermAppl gsGetResult(ATermAppl sort)
{
	while ( gsIsSortArrow(sort) )
	{
		sort = ATAgetArgument(sort,1);
	}

	return sort;
}

static ATermList gsGetDomain(ATermAppl sort)
{
	ATermList l;

	l = ATmakeList0();
	while ( gsIsSortArrow(sort) )
	{
		l = ATinsert(l,ATgetArgument(sort,0));
		sort = ATAgetArgument(sort,1);
	}
	l = ATreverse(l);

	return l;
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
			return gsFromRewriteFormat(ATgetArgument((ATermAppl) state,index));
		case GS_STATE_TREE:
			return gsFromRewriteFormat(getTreeElement(state,index));
		default:
			return NULL;
	}
}

/*ATermAppl gsGetStateArgument(ATerm state, int index)
{
	if ( (0 <= index) && (index < ATgetLength((ATermList) state)) )
	{
		return gsFromRewriteFormat(ATelementAt((ATermList) state,index));
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
		gsErrorMsg("cannot generate dummies for function sorts (%t)\n",sort);
		exit(1);
	}

	l = ATLgetArgument(ATAgetArgument(current_spec,1),0);
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		ATermAppl conssort = ATAgetArgument(ATAgetFirst(l),1);
		if ( ATisEqual(gsGetResult(conssort),sort) )
		{
			ATermList domain = gsGetDomain(conssort);
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
		if ( ATisEqual(gsGetResult(mapsort),sort) )
		{
			ATermList domain = gsGetDomain(mapsort);
			ATermAppl t = ATAgetFirst(l);

			for (; !ATisEmpty(domain); domain=ATgetNext(domain))
			{
				t = gsMakeDataAppl(t,FindDummy(ATAgetFirst(domain)));
			}

			return t;
		}
	}

	ATfprintf(stderr,"Could not find dummy of type %t\n",sort);
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
	ATermList m = ATmakeList0();
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		m = ATinsert(m,gsToRewriteFormat((ATermAppl) SetVars(ATgetFirst(l),free_vars)));
	}
	return ATreverse(m);
}

ATermList ListFromFormat(ATermList l)
{
	ATermList m = ATmakeList0();
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		m = ATinsert(m,(ATerm) gsFromRewriteFormat(ATgetFirst(l)));
	}
	return ATreverse(m);
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
		l = ATinsert(l,(ATerm) ATmakeAppl2(ATgetAFun(a),gsToRewriteFormat(ATAgetArgument(a,0)),gsToRewriteFormat(ATAgetArgument(a,1))));
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
				stateargs[i] = gsToRewriteFormat((ATermAppl) SetVars(ATgetArgument(ATAgetFirst(m),1),free_vars));
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

ATerm gsNextStateInit(ATermAppl Spec, bool AllowFreeVars, int StateFormat, int RewriteStrategy)
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
	
	gsProverInit(Spec,RewriteStrategy);

	free_vars = ATLgetArgument(ATAgetArgument(current_spec,5),0);

	l = ATLgetArgument(ATAgetArgument(current_spec,5),1);
	pars = ATmakeList0();
	ATprotectList(&pars);
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		pars = ATinsert(pars,gsToRewriteFormat(ATAgetFirst(l)));
	}
	pars = ATreverse(pars);

	statelen = ATgetLength(pars);
	stateAFun = ATmakeAFun("STATE",statelen,ATfalse);
	ATprotectAFun(stateAFun);
	stateargs = (ATerm *) malloc(statelen*sizeof(ATerm));
	for (int i=0; i<statelen; i++)
	{
		stateargs[i] = NULL;
	}
	ATprotectArray(stateargs,statelen);

	l = ATLgetArgument(ATAgetArgument(current_spec,5),1);
	procvars = ATmakeList0();
	ATprotectList(&procvars);
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		procvars = ATinsert(procvars,gsToRewriteFormat(ATAgetFirst(l)));
	}
	procvars = ATreverse(procvars);
	
	smndAFun = ATmakeAFun("@SMND@",4,ATfalse);
	ATprotectAFun(smndAFun);
	ATermList sums = ATLgetArgument(ATAgetArgument(current_spec,5),2);
	num_summands = ATgetLength(sums);
	summands = (ATermAppl *) malloc(num_summands*sizeof(ATermAppl));
	for (int i=0; i<num_summands; i++)
	{
		summands[i] = NULL;
	}
	ATprotectArray((ATerm *) summands,num_summands);
	for (int i=0; !ATisEmpty(sums); sums=ATgetNext(sums),i++)
	{
		summands[i] = ATmakeAppl4(smndAFun,ATgetArgument(ATAgetFirst(sums),0),gsToRewriteFormat((ATermAppl) SetVars(ATgetArgument(ATAgetFirst(sums),1),free_vars)),(ATerm) ActionToRewriteFormat(ATAgetArgument(ATAgetFirst(sums),2),free_vars),(ATerm) AssignsToRewriteFormat(ATLgetArgument(ATAgetFirst(sums),4),free_vars));
	}

	l = pars;
	m = ATLgetArgument(ATAgetArgument(Spec,6),1);
/*	state = ATmakeList0();
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		n = m;
		set = false;
		for (; !ATisEmpty(n); n=ATgetNext(n))
		{
			if ( ATisEqual(ATAgetArgument(ATAgetFirst(n),0),ATAgetFirst(l)) )
			{
				state = ATinsert(state,(ATerm) gsRewriteInternal(SetVars(gsToRewriteFormat(ATAgetArgument(ATAgetFirst(n),1)))));
				set = true;
				break;
			}
		}
		if ( !set )
		{
			state = ATinsert(state,(ATerm) gsRewriteInternal(SetVars(gsToRewriteFormat(ATAgetFirst(l)))));
		}
	}

	return (ATerm) ATreverse(state);*/

	for (int i=0; !ATisEmpty(l); l=ATgetNext(l), i++)
	{
		n = m;
		set = false;
		for (; !ATisEmpty(n); n=ATgetNext(n))
		{
			if ( ATisEqual(ATAgetArgument(ATAgetFirst(n),0),ATAgetFirst(l)) )
			{
				stateargs[i] = gsRewriteInternal(gsToRewriteFormat((ATermAppl) SetVars(ATgetArgument(ATAgetFirst(n),1),free_vars)));
				set = true;
				break;
			}
		}
		if ( !set )
		{
			gsErrorMsg("Parameter '%t' does not have an initial value.",ATgetArgument(ATAgetFirst(l),0));
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
	gsProverFinalise();
	
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

static ATerm makeNewState(ATerm old, ATermList vars, ATerm assigns)
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
					stateargs[i] = RWapplySubstitution(ATgetFirst(l));
					break;
			}
		} else {
			stateargs[i] = gsRewriteInternal(a);
//			stateargs[i] = gsRewriteInternal(SetVars(a));
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

/*static ATerm makeNewState(ATerm old, ATermList vars, ATermList assigns, ATermList substs)
{
	if ( ATisEmpty(old) )
	{
		return old;
	}

	for (ATermList l=assigns; !ATisEmpty(l); l=ATgetNext(l))
	{
		if ( ATisEqual(ATgetArgument(ATAgetFirst(l),0),ATgetFirst(vars)) )
		{
			return (ATerm) ATinsert(makeNewState(ATgetNext((ATermList) old),ATgetNext(vars),assigns,substs),(ATerm) gsRewriteInternal(SetVars(ATgetArgument(ATAgetFirst(l),1))));
		}
	}
	return (ATerm) ATinsert(makeNewState(ATgetNext((ATermList) old),ATgetNext(vars),assigns,substs),ATgetFirst((ATermList) old));
}*/

/*static ATerm makeNewState(ATerm old, ATermList vars, ATermList assigns, ATermList substs)
{
	ATermList nnew,l;
	bool set;

	nnew = ATmakeList0();
	for (; !ATisEmpty(vars); vars=ATgetNext(vars),old=(ATerm) ATgetNext((ATermList) old))
	{
		set = false;
		l = assigns;
		for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			if ( ATisEqual(ATgetArgument(ATAgetFirst(l),0),ATgetFirst(vars)) )
			{
				//nnew = ATinsert(nnew,(ATerm) gsRewriteInternal(SetVars(gsSubstValues(substs,ATgetArgument(ATAgetFirst(l),1),true))));
				nnew = ATinsert(nnew,(ATerm) gsRewriteInternal(SetVars(ATgetArgument(ATAgetFirst(l),1))));
				set = true;
				break;
			}
		}
		if ( !set )
		{
			nnew = ATinsert(nnew,ATgetFirst((ATermList) old));
		}
	}
//	nnew = gsRewriteTerms((ATermList) SetVars((ATerm) ATreverse(nnew)));
	nnew = ATreverse(nnew);

	return (ATerm) nnew;
}*/

ATermAppl rewrActionArgs(ATermAppl act)
{
	ATermList l = ATLgetArgument(act,0);
	ATermList m = ATmakeList0();

	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		ATermAppl a = ATAgetFirst(l);
		a = gsMakeAction(ATAgetArgument(a,0),ListFromFormat(gsRewriteInternals(ATLgetArgument(a,1))));
		m = ATinsert(m,(ATerm) a);
	}
	m = ATreverse(m);

	return gsMakeMultAct(m);
}

static ATerm *State_p;
static ATermList *states_p;
static ATerm *newstate_p;
//static ATermList *params_l_p;
static ATerm *act_p;
static gsNextStateCallBack fscb;
static void gsns_callback(ATermList solution)
{
//ATprintf("%t\n",ListFromFormat(solution));
/*for (ATermList l=solution; !ATisEmpty(l); l=ATgetNext(l))
{
	ATfprintf(stderr,"%t := ",ATgetArgument(ATAgetFirst(l),0)); 
	gsPrintPart(stderr,gsFromRewriteFormat(ATgetArgument(ATgetFirst(l),1)),false,0);
	if ( !ATisEmpty(ATgetNext(l)) )
		fprintf(stderr,", "); 
}
fprintf(stderr,"\n");*/
	if ( fscb != NULL )
	{
		for (ATermList l=solution; !ATisEmpty(l); l=ATgetNext(l))
		{
			RWsetVariable(ATgetArgument((ATermAppl) ATgetFirst(l),0),ATgetArgument((ATermAppl) ATgetFirst(l),1));
		}
//		fscb(rewrActionArgs((ATermAppl) gsSubstValues(ATconcat(*params_l_p,solution),*act_p,true)),(ATerm) makeNewState(*State_p,pars,*newstate_p,ATconcat(*params_l_p,solution)));
		fscb(rewrActionArgs((ATermAppl) *act_p),(ATerm) makeNewState(*State_p,pars,*newstate_p));
		for (ATermList l=solution; !ATisEmpty(l); l=ATgetNext(l))
		{
			RWclearVariable(ATgetArgument((ATermAppl) ATgetFirst(l),0));
		}
	} else {
		for (ATermList l=solution; !ATisEmpty(l); l=ATgetNext(l))
		{
			RWsetVariable(ATgetArgument((ATermAppl) ATgetFirst(l),0),ATgetArgument((ATermAppl) ATgetFirst(l),1));
		}
		*states_p = ATinsert(*states_p, (ATerm)
				ATmakeList2(
//					(ATerm) rewrActionArgs((ATermAppl) gsSubstValues(ATconcat(*params_l_p,solution),*act_p,true)),
//					(ATerm) makeNewState(*State_p,pars,*newstate_p,ATconcat(*params_l_p,solution))
					(ATerm) rewrActionArgs((ATermAppl) *act_p),
					(ATerm) makeNewState(*State_p,pars,*newstate_p)
					)
				);
		for (ATermList l=solution; !ATisEmpty(l); l=ATgetNext(l))
		{
			RWclearVariable(ATgetArgument((ATermAppl) ATgetFirst(l),0));
		}
	}
}

static void SetTreeStateVars(ATerm tree, ATermList *vars)
{
	if ( (ATgetType(tree) == AT_APPL) && ATisEqualAFun(ATgetAFun((ATermAppl) tree),pairAFun) )
	{
		SetTreeStateVars(ATgetArgument((ATermAppl) tree,0),vars);
		SetTreeStateVars(ATgetArgument((ATermAppl) tree,1),vars);
	} else {
		RWsetVariable(ATgetFirst(*vars),tree);
		*vars = ATgetNext(*vars);
	}
}

ATermList gsNextState(ATerm State, gsNextStateCallBack f)
{
	ATermList states,l;//,m,params;
	ATermAppl sum;
	ATerm newstate;
	ATerm act;
	int sum_idx;

//fprintf(stderr,"\nState: "); gsPrintParts(stderr,ListFromFormat(ATgetArguments(State)),false,0,"",", "); fprintf(stderr,"\n\n");

	NextStateError = false;

	fscb = f;

	l = procvars;
//	m = (ATermList) State;
//	ATtableReset(params);
//	ATermList params_l = ATmakeList0();
	switch ( stateformat )
	{
		case GS_STATE_VECTOR:
			for (int i=0; !ATisEmpty(l); l=ATgetNext(l),i++)
			{
				ATerm a = ATgetArgument((ATermAppl) State,i);

				if ( !ATisEqual(a,nil) )
				{
//			ATtablePut(params,ATgetFirst(l),ATgetFirst(m));
					RWsetVariable(ATgetFirst(l),a);
//			params_l = ATinsert(params_l,(ATerm) gsMakeSubst(ATgetFirst(l),ATgetFirst(m)));
				}
			}
			break;
		case GS_STATE_TREE:
			SetTreeStateVars(State,&l);
			break;
	}
//	params_l = ATreverse(params_l);

	sum_idx = 0;
	states = ATmakeList0();
	State_p = &State;
	states_p = &states;
	newstate_p = &newstate;
//	params_l_p = &params_l;
	act_p = &act;
	while ( sum_idx < num_summands )
	{
//		sum = smd_subst_vars(ATAgetFirst(sums),params);
//		ATerm act = ATgetArgument(sum,2);
//		ATermList newstate = ATLgetArgument(sum,4);
		sum = summands[sum_idx];
//		ATerm act = gsSubstValues(params_l,ATgetArgument(sum,2),true);
//		ATermList newstate = (ATermList) gsSubstValues(params_l,ATgetArgument(sum,4),true);
		act = ATgetArgument(sum,2);
		newstate = ATgetArgument(sum,3);
//ATfprintf(stderr,"\n%t  ",ATLgetArgument(sum,0)); gsPrintPart(stderr,gsFromRewriteFormat(ATgetArgument(sum,1)),false,0); fprintf(stderr,"\n\n");
		l = FindSolutions(ATLgetArgument(sum,0),ATgetArgument(sum,1),gsns_callback);
		NextStateError |= FindSolutionsError;
		for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			if ( f != NULL )
			{
				for (ATermList m=(ATermList) ATgetFirst(l); !ATisEmpty(m); m=ATgetNext(m))
				{
					RWsetVariable(ATgetArgument((ATermAppl) ATgetFirst(m),0),ATgetArgument((ATermAppl) ATgetFirst(m),1));
				}
//				f(rewrActionArgs((ATermAppl) gsSubstValues(ATconcat(params_l,ATLgetFirst(l)),act,true)),(ATerm) makeNewState(State,pars,newstate,ATconcat(params_l,ATLgetFirst(l))));
				f(rewrActionArgs((ATermAppl) *act_p),(ATerm) makeNewState(*State_p,pars,*newstate_p));
				for (ATermList m=(ATermList) ATgetFirst(l); !ATisEmpty(m); m=ATgetNext(m))
				{
					RWclearVariable(ATgetArgument((ATermAppl) ATgetFirst(m),0));
				}
			} else {
				for (ATermList m=(ATermList) ATgetFirst(l); !ATisEmpty(m); m=ATgetNext(m))
				{
					RWsetVariable(ATgetArgument((ATermAppl) ATgetFirst(m),0),ATgetArgument((ATermAppl) ATgetFirst(m),1));
				}
				states = ATinsert(states, (ATerm)
						ATmakeList2(
//							(ATerm) rewrActionArgs((ATermAppl) gsSubstValues(ATconcat(params_l,ATLgetFirst(l)),act,true)),
//							(ATerm) makeNewState(State,pars,newstate,ATconcat(params_l,ATLgetFirst(l)))
							(ATerm) rewrActionArgs((ATermAppl) *act_p),
							(ATerm) makeNewState(*State_p,pars,*newstate_p)
							)
						);
				for (ATermList m=(ATermList) ATgetFirst(l); !ATisEmpty(m); m=ATgetNext(m))
				{
					RWclearVariable(ATgetArgument((ATermAppl) ATgetFirst(m),0));
				}
			}
		}
		sum_idx++;
	}
	states = ATreverse(states);

	return states;
}

#ifdef __cplusplus
}
#endif
