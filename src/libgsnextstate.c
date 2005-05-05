#include <stdio.h>
#include <stdlib.h>
#include <aterm2.h>
#include "gslowlevel.h"
#include "gsfunc.h"
#include "libgsprover.h"
#include "libgsrewrite.h"

bool ATisList(ATerm a)
{
	return (ATgetType(a) == AT_LIST);
}

static ATermAppl current_spec;

ATermAppl FindDummy(ATermAppl sort)
{
	ATermList l;

	l = ATLgetArgument(ATAgetArgument(current_spec,1),0);
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		if ( ATisEqual(ATAgetArgument(ATAgetFirst(l),1),sort) )
		{
			return ATAgetFirst(l);
		}
	}

	l = ATLgetArgument(ATAgetArgument(current_spec,2),0);
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		if ( ATisEqual(ATAgetArgument(ATAgetFirst(l),1),sort) )
		{
			return ATAgetFirst(l);
		}
	}

	ATfprintf(stderr,"Could not find dummy of type %t\n",sort);
	exit(1);
}

ATerm SetVars(ATerm a)
{
	ATermList l,m;

	if ( ATisList(a) )
	{
		l = (ATermList) a;
		m = ATmakeList0();
		for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			m = ATinsert(m,SetVars(ATgetFirst(l)));
		}
		return (ATerm) ATreverse(m);
	} else if ( gsIsDataVarId((ATermAppl) a) )
	{
		return (ATerm) FindDummy(ATAgetArgument((ATermAppl) a,1));
	} else if ( gsIsDataAppl((ATermAppl) a) )
	{
		return (ATerm) gsMakeDataAppl((ATermAppl) SetVars(ATgetArgument((ATermAppl) a,0)),(ATermAppl) SetVars(ATgetArgument((ATermAppl) a,1)));
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

ATermList gsNextStateInit(ATermAppl Spec)
{
	ATermList l,m,n,state;
	bool set;

	current_spec = Spec;

	l = ATLgetArgument(ATAgetArgument(Spec,5),1);
	m = ATLgetArgument(ATAgetArgument(Spec,6),1);
	state = ATmakeList0();
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		n = m;
		set = false;
		for (; !ATisEmpty(n); n=ATgetNext(n))
		{
			if ( ATisEqual(ATAgetArgument(ATAgetFirst(n),0),ATAgetFirst(l)) )
			{
				state = ATinsert(state,SetVars(ATgetArgument(ATAgetFirst(n),1)));
				set = true;
				break;
			}
		}
		if ( !set )
		{
			state = ATinsert(state,SetVars(ATgetFirst(l)));
		}
	}

	gsProverInit(Spec);

	return ATreverse(state);
}

static ATermList makeNewState(ATermList old, ATermList vars, ATermList assigns, ATermList substs)
{
	ATermList new,l;
	bool set;

	new = ATmakeList0();
	for (; !ATisEmpty(vars); vars=ATgetNext(vars),old=ATgetNext(old))
	{
		set = false;
		l = assigns;
		for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			if ( ATisEqual(ATgetArgument(ATAgetFirst(l),0),ATgetFirst(vars)) )
			{
				new = ATinsert(new,gsSubstValues(substs,ATgetArgument(ATAgetFirst(l),1),true));
				set = true;
				break;
			}
		}
		if ( !set )
		{
			new = ATinsert(new,ATgetFirst(old));
		}
	}
	new = gsRewriteTerms((ATermList) SetVars((ATerm) ATreverse(new)));

	return new;
}

ATermAppl rewrActionArgs(ATermAppl act)
{
	ATermList l = ATLgetArgument(act,0);
	ATermList m = ATmakeList0();

	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		ATermAppl a = ATAgetFirst(l);
		a = gsMakeAction(ATAgetArgument(a,0),gsRewriteTerms(ATLgetArgument(a,1)));
		m = ATinsert(m,(ATerm) a);
	}
	m = ATreverse(m);

	return gsMakeMultAct(m);
}

ATermList gsNextState(ATermList State)
{
	ATermList sums,states,l,m,params;
	ATermAppl sum;
	
	l = ATLgetArgument(ATAgetArgument(current_spec,5),1);
	m = State;
	params = ATmakeList0();
	for (; !ATisEmpty(l); l=ATgetNext(l),m=ATgetNext(m))
	{
		params = ATinsert(params,(ATerm) gsMakeSubst(ATgetFirst(l),ATgetFirst(m)));
	}
	params = ATreverse(params);

	sums = ATLgetArgument(ATAgetArgument(current_spec,5),2);
	states = ATmakeList0();
	for (; !ATisEmpty(sums); sums=ATgetNext(sums))
	{
		sum = smd_subst_vars(ATAgetFirst(sums),params);
		l = FindSolutions(ATLgetArgument(sum,0),ATAgetArgument(sum,1));
		for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			states = ATinsert(states, (ATerm)
					ATmakeList2(
						(ATerm) rewrActionArgs((ATermAppl) gsSubstValues(ATLgetFirst(l),ATgetArgument(sum,2),true)),
						(ATerm) makeNewState(State,ATLgetArgument(ATAgetArgument(current_spec,5),1),ATLgetArgument(sum,4),ATLgetFirst(l))
						)
					);
		}
	}
	states = ATreverse(states);

	return states;
}
