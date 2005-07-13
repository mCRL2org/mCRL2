/* $Id: gslinearise2.c,v 1.21 2005/05/12 09:34:11 muck Exp $ */
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <aterm2.h>
#include "gsfunc.h"
#include "gslowlevel.h"
#include "libgsparse.h"
#include "libgsrewrite.h"

/* Global variables */

static ATermList processes;
static ATermList todo_stack;
static ATerm debruijn;
static bool generalise;
static ATermAppl linTrue;
static ATermAppl linFalse;

/* Needed forward declarations */

ATermList get_sorts(ATermList l);
void linearise(int i);
ATermList list_minus(ATermList l1, ATermList l2);
ATermList merge_list(ATermList l1, ATermList l2);
ATermList mactl_comm(ATermList mal, ATermList C);
ATermAppl synch_reduce(ATermAppl ma);
int proc_id(ATermAppl a);


/* Auxiliary ATerm functions */

ATbool ATisList(ATerm a)
{
	return (ATbool) (ATgetType(a) == AT_LIST);
}

ATbool ATisAppl(ATerm a)
{
	return (ATbool) (ATgetType(a) == AT_APPL);
}

ATbool ATisAnnotated(ATerm a)
{
	return (ATbool) (ATgetAnnotation(a,debruijn) != NULL);
}


/* Auxiliary GenSpect functions */

ATermAppl gsMakeDataExprAndWithTrueCheck(ATermAppl b, ATermAppl c)
{
	if ( ATisEqual(b,linTrue) )
	{
		return c;
	} else if ( ATisEqual(c,linTrue) )
	{
		return b;
	} else {
		return gsMakeDataExprAnd(b,c);
	}
}

ATermAppl gsMakeDataExprAndWithCheck(ATermAppl b, ATermAppl c)
{
	if ( ATisEqual(b,linTrue) )
	{
		return c;
	} else if ( ATisEqual(c,linTrue) )
	{
		return b;
	} else if ( ATisEqual(b,linFalse) || ATisEqual(c,linFalse) )
	{
		return linFalse;
	} else {
		return gsMakeDataExprAnd(b,c);
	}
}

ATermAppl gsMakeDataExprNotWithCheck(ATermAppl b)
{
	if ( ATisEqual(b,linTrue) )
	{
		return linFalse;
	} else if ( ATisEqual(b,linFalse) )
	{
		return linTrue;
	} else {
		return gsMakeDataExprNot(b);
	}
}

/*ATermAppl gsMakeDataExprPosFromInt(int i)
{
	char s[10];
	if ( i <= 0 )
		i = 1;
	sprintf(s,"%i",i);
	return gsMakeDataExprPos(s);
}*/

ATermAppl get_new_sort_name(ATermAppl s, ATermList o)
{
	char name[40];
	ATermAppl v;
	int i;

	i = 0;
	sprintf(name,"dummy%i",i);
	v = gsMakeOpId(gsString2ATermAppl(name),s);
	while ( ATindexOf(o,(ATerm) v,0) >= 0 )
	{
		if ( i == INT_MAX )
		{
			gsErrorMsg("cannot create a fresh data variable\n");
			exit(1);
		}
		sprintf(name,"dummy%i",i);
		v = gsMakeOpId(gsString2ATermAppl(name),s);
		i++;
	}

	return v;
}

/*ATermAppl gsMakeDataExprOfSort(ATermAppl s, ATermList cons, ATermList *maps)
{
	ATermList l;

	for (l=cons; !ATisEmpty(l); l=ATgetNext(l))
	{
		if ( ATisEqual(ATAgetArgument(ATAgetFirst(l),1),s) )
		{
			return ATAgetFirst(l);
		}
	}

	for (l=*maps; !ATisEmpty(l); l=ATgetNext(l))
	{
		if ( ATisEqual(ATAgetArgument(ATAgetFirst(l),1),s) )
		{
			return ATAgetFirst(l);
		}
	}

	*maps = ATappend(*maps,(ATerm) get_new_sort_name(s,ATconcat(cons,*maps)));

	return (ATermAppl) ATgetLast(*maps);
}*/


/* Variable manipulation functions */

ATermAppl fresh_var_name()
{
	char s[16];
	int b,i,j;
	ATermList l;

	i=0;
	j=0;
	s[1] = 0;
	while ( j != INT_MAX )
	{
		if ( j == 0 )
		{
			s[0] = 'A'+i;
		} else {
			sprintf(s,"%c%i",'A'+i,j-1);
		}
		b = 1;
		for (l=processes; !ATisEmpty(l); l=ATgetNext(l))
		{
			if  ( !strcmp(s,gsATermAppl2String(ATAelementAt(ATLgetFirst(l),0))) )
			{
				b = 0;
				break;
			}
		}
		if ( b )
		{
			return gsString2ATermAppl(s);
		}
		i++;
		if ( i == 26 )
		{
			i = 0;
			j++;
		}
	}
	gsErrorMsg("cannot create a fresh process variable\n");
	exit(1);
}

ATerm annotate_context(ATerm a, ATermList c)
{
	ATermList l,m;

	if ( ATisAppl(a) )
	{
		if ( gsIsDataVarId((ATermAppl) a) )
		{
			if ( ATindexOf(c,a,0) >= 0 )
			{
				return ATsetAnnotation(a,debruijn,(ATerm) ATmakeInt(ATindexOf(c,a,0)));
			} else {
				return a;
			}
		} else if ( gsIsSum((ATermAppl) a) )
		{
			l = ATLgetArgument((ATermAppl) a,0);
			for (; !ATisEmpty(l); l=ATgetNext(l))
			{
				c = ATinsert(c,ATgetFirst(l));
			}
			return (ATerm) gsMakeSum(ATLgetArgument((ATermAppl) a,0),(ATermAppl) annotate_context(ATgetArgument((ATermAppl) a,1),c));
		} else {
			l = ATgetArguments((ATermAppl) a);
			m = ATmakeList0();
			for (; !ATisEmpty(l); l=ATgetNext(l))
			{
				m = ATinsert(m,annotate_context(ATgetFirst(l),c));
			}
			m = ATreverse(m);
			return (ATerm) ATmakeApplList(ATgetAFun((ATermAppl) a),m);
		}
	} else if ( ATisList((ATerm) a) )
	{
		l = (ATermList) a;
		m = ATmakeList0();
		for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			m = ATinsert(m,annotate_context(ATgetFirst(l),c));
		}
		return (ATerm) ATreverse(m);
	} else {
		gsErrorMsg("term to annotate is not a ATermAppl or a ATermList\n");
		exit(1);
	}
}

ATerm annotate(ATerm a)
{
	return annotate_context(a,ATmakeList0());
}

ATerm increase_index_bnd(ATerm a, int n, int b)
{
	ATermList l,m;

	if ( ATisAppl(a) )
	{
		if ( gsIsDataVarId((ATermAppl) a) )
		{
			if ( ATisAnnotated(a) && (ATgetInt((ATermInt) ATgetAnnotation(a,debruijn)) >= b) )
			{
				return ATsetAnnotation(a,debruijn,(ATerm) ATmakeInt(ATgetInt((ATermInt) ATgetAnnotation(a,debruijn))+n));
			} else {
				return a;
			}
		} else if ( gsIsSum((ATermAppl) a) )
		{
			return (ATerm) gsMakeSum(ATLgetArgument((ATermAppl) a,0),(ATermAppl) increase_index_bnd(ATgetArgument((ATermAppl) a,1),n,b+ATgetLength(ATLgetArgument((ATermAppl) a,0))));
		} else {
			int ar = ATgetArity(ATgetAFun((ATermAppl) a));
			DECL_A(args,ATerm,ar);
			for (int i=0; i<ar; i++)
			{
				args[i] = increase_index_bnd(ATgetArgument((ATermAppl) a,i),n,b);
			}
			a = ATmakeApplArray(ATgetAFun((ATermAppl) a),args);
			FREE_A(args);
			return a;
			
/*			l = ATgetArguments((ATermAppl) a);
			m = ATmakeList0();
			for (; !ATisEmpty(l); l=ATgetNext(l))
			{
				m = ATinsert(m,increase_index_bnd(ATgetFirst(l),n,b));
			}
			m = ATreverse(m);
			return (ATerm) ATmakeApplList(ATgetAFun((ATermAppl) a),m);*/
		}
	} else if ( ATisList((ATerm) a) )
	{
		l = (ATermList) a;
		m = ATmakeList0();
		for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			m = ATinsert(m,increase_index_bnd(ATgetFirst(l),n,b));
		}
		return (ATerm) ATreverse(m);
	} else {
		gsErrorMsg("term to increase indices of is not a ATermAppl or a ATermList\n");
		exit(1);
	}
}

ATerm increase_index(ATerm a, int n)
{
	return increase_index_bnd(a, n, 0);
}

ATermAppl get_new_var(ATermAppl a, ATermList *t)
{
	char name[100];
	ATermAppl v;
	int i;

	strncpy(name,gsATermAppl2String(ATAgetArgument(a,0)),99);
	name[99] = 0;
	v = gsMakeDataVarId(gsString2ATermAppl(name),ATAgetArgument(a,1));
	i = 0;
	while ( ATindexOf(*t,(ATerm) v,0) >= 0 )
	{
		if ( i == INT_MAX )
		{
			gsErrorMsg("cannot create a fresh data variable\n");
			exit(1);
		}
		snprintf(name,99,"%s%i",gsATermAppl2String(ATAgetArgument(a,0)),i);
		name[99] = 0;
		v = gsMakeDataVarId(gsString2ATermAppl(name),ATAgetArgument(a,1));
		i++;
	}
	*t = ATappend(*t,(ATerm) v);

	return v;
}

ATerm subst_new_var(ATerm a, int n, ATermAppl v)
{
	ATermList l,m;

	if ( ATisAppl(a) )
	{
		if ( gsIsDataVarId((ATermAppl) a) )
		{
			if ( ATisAnnotated(a) && (ATgetInt((ATermInt) ATgetAnnotation(a,debruijn)) == n) )
			{
				return (ATerm) v;
			} else {
				return a;
			}
		} else if ( gsIsSum((ATermAppl) a) )
		{
			return (ATerm) gsMakeSum(ATLgetArgument((ATermAppl) a,0), (ATermAppl) subst_new_var(ATgetArgument((ATermAppl) a,1),n+ATgetLength(ATLgetArgument((ATermAppl) a,0)),v));
		} else {
			l = ATgetArguments((ATermAppl) a);
			m = ATmakeList0();
			for (; !ATisEmpty(l); l=ATgetNext(l))
			{
				m = ATinsert(m,subst_new_var(ATgetFirst(l),n,v));
			}
			m = ATreverse(m);
			return (ATerm) ATmakeApplList(ATgetAFun((ATermAppl) a),m);
		}
	} else if ( ATisList((ATerm) a) )
	{
		l = (ATermList) a;
		m = ATmakeList0();
		for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			m = ATinsert(m,subst_new_var(ATgetFirst(l),n,v));
		}
		return (ATerm) ATreverse(m);
	} else {
		gsErrorMsg("term to substitute variable in is not a ATermAppl or a ATermList\n");
		exit(1);
//		return a; XXX why was this here?
	}
}

ATerm remove_indices_context(ATerm a, ATerm *b, ATermList *t)
{
	ATermList l,m;
	ATermAppl u,p;
	int i;

	if ( ATisAppl(a) )
	{
		if ( gsIsDataVarId((ATermAppl) a) )
		{
			if ( ATisAnnotated(a) )
			{
				u = get_new_var((ATermAppl) a,t);
				if ( b != NULL )
					*b = subst_new_var(*b,ATgetInt((ATermInt) ATgetAnnotation(a,debruijn)),u);
				return (ATerm) u;
			} else {
				return a;
			}
		} else if ( gsIsSum((ATermAppl) a) )
		{
			l = ATLgetArgument((ATermAppl) a,0);
			p = ATAgetArgument((ATermAppl) a,1);
			m = ATmakeList0();
			i=0;
			for (; !ATisEmpty(l); l=ATgetNext(l))
			{
				u = get_new_var(ATAgetFirst(l),t);
				p = (ATermAppl) subst_new_var((ATerm) p,ATgetInt((ATermInt) ATgetAnnotation(ATgetFirst(l),debruijn)),u);
				m = ATinsert(m,(ATerm) u);
				i++;
			}
			m = ATreverse(m);
			u = (ATermAppl) remove_indices_context((ATerm) p,b,t);
			*t = list_minus(*t,m);
			return (ATerm) gsMakeSum(m, u);
		} else {
			ATfprintf(stderr,"else %t\n",a);
			l = ATgetArguments((ATermAppl) a);
			m = ATmakeList0();
			for (; !ATisEmpty(l); l=ATgetNext(l))
			{
				m = ATinsert(m,remove_indices_context(ATgetFirst(l),b,t));
			}
			m = ATreverse(m);
			return (ATerm) ATmakeApplList(ATgetAFun((ATermAppl) a),m);
		}
	} else if ( ATisList((ATerm) a) )
	{
		l = (ATermList) a;
		m = ATmakeList0();
		for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			m = ATinsert(m,remove_indices_context(ATgetFirst(l),b,t));
		}
		return (ATerm) ATreverse(m);
	} else {
		gsErrorMsg("term to remove indices from is not a ATermAppl or a ATermList\n");
		exit(1);
//		return a; XXX why was this here?
	}
}

ATerm remove_indices_contextA(ATerm a, ATermAppl *b, ATermList *t)
{
	return remove_indices_context(a,(ATerm *) b, t);
}

ATerm remove_indices_contextL(ATerm a, ATermList *b, ATermList *t)
{
	return remove_indices_context(a,(ATerm *) b, t);
}

ATerm remove_indices_sep(ATerm a, ATerm *b)
{
	ATermList t;
	ATerm r;

	t = ATmakeList0();
	r = remove_indices_context(a,b,&t);

	return r;
}

ATerm remove_indices(ATerm a)
{
	return remove_indices_sep(a,NULL);
}

ATermList make_indices(ATermList l)
{
	ATermList m;
	int i;

	m = ATmakeList0();
	i = ATgetLength(l)-1;
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		m = ATinsert(m,ATsetAnnotation(ATgetFirst(l),debruijn,(ATerm) ATmakeInt(i)));
		i--;
	}

	return ATreverse(m);
}

ATerm substitute_vars(ATerm a, ATermList v, ATermList s)
{
	ATermList l,m;

	if ( ATisAppl(a) )
	{
		if ( gsIsDataVarId((ATermAppl) a) )
		{
			if ( ATindexOf(v,a,0) >= 0 )
			{
				return ATelementAt(s,ATindexOf(v,a,0));
			} else {
				return a;
			}
//		} else if ( gsIsSum((ATermAppl) a) )
//		{
//			return (ATerm) gsMakeSum(ATLgetArgument((ATermAppl) a,0), (ATermAppl) substitute_vars(ATgetArgument((ATermAppl) a,1),v,s));
		} else {
			l = ATgetArguments((ATermAppl) a);
			m = ATmakeList0();
			for (; !ATisEmpty(l); l=ATgetNext(l))
			{
				m = ATinsert(m,substitute_vars(ATgetFirst(l),v,s));
			}
			m = ATreverse(m);
			return (ATerm) ATmakeApplList(ATgetAFun((ATermAppl) a),m);
		}
	} else if ( ATisList((ATerm) a) )
	{
		l = (ATermList) a;
		m = ATmakeList0();
		for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			m = ATinsert(m,substitute_vars(ATgetFirst(l),v,s));
		}
		return (ATerm) ATreverse(m);
	} else {
		gsErrorMsg("term to substitute variables in is not a ATermAppl or a ATermList\n");
		exit(1);
	}
}

ATermList decrease_indices(ATermList l)
{
	ATermList m;
	ATerm a;

	m = ATmakeList0();
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		a = ATgetFirst(l);
		if ( ATisAnnotated(a) )
		{
			a = ATsetAnnotation(a,debruijn,(ATerm) ATmakeInt(ATgetInt((ATermInt) ATgetAnnotation(a,debruijn))-1));
		}
		m = ATinsert(m,a);
	}
	m = ATreverse(m);

	return m;
}

//XXX
#define ATAgetFirst (ATermAppl) ATgetFirst
ATermList get_vars(ATermAppl a)
{
	ATermList l,m;

	if ( gsIsDataVarId(a) )
	{
		return ATmakeList1((ATerm) a);
	}
	else
	if ( gsIsOpId(a) || gsIsDelta(a) || gsIsTau(a) )
	{
		return ATmakeList0();
	}
	else
	if ( gsIsDataAppl(a) || gsIsSync(a) || gsIsSeq(a) ||  gsIsMerge(a) || gsIsLMerge(a) || gsIsChoice(a) || gsIsCond(a) )
	{
		return merge_list(get_vars(ATAgetArgument(a,0)),get_vars(ATAgetArgument(a,1)));
	}
	else
	if ( gsIsAction(a) || gsIsProcess(a) )
	{
		l = ATmakeList0();
		for (m=ATLgetArgument(a,1);!ATisEmpty(m);m=ATgetNext(m))
		{
			l = merge_list(l,get_vars(ATAgetFirst(m)));
		}
		return l;
	}
	else
	if ( gsIsSum(a) )
	{
		l = get_vars(ATAgetArgument(a,1));
		l = list_minus(l,make_indices(ATLgetArgument(a,0)));
		l = decrease_indices(l);
		return l;
	}
	else
	if ( gsIsRestrict(a) || gsIsHide(a) || gsIsRename(a) || gsIsComm(a) ||  gsIsAllow(a) )
	{
		return get_vars(ATAgetArgument(a,1));
	}
	// XXX Special case to compensate for unfinished data implementation
	else
	{
		gsWarningMsg("unknown process or data expression (%t); trying to compensate\n",a);
		if ( ATisList((ATerm) a) )
		{
			m = (ATermList) a;
		} else {
			m = ATgetArguments(a);
		}
		l = ATmakeList0();
		for (;!ATisEmpty(m);m=ATgetNext(m))
		{
			l = merge_list(l,get_vars(ATAgetFirst(m)));
		}
		return l;
	}
	
	gsWarningMsg("unknown process or data expression (%t)\n",a);
	return ATmakeList0();
}
//XXX
#undef ATAgetFirst


/* Global variables manipulation functions */

ATermAppl replace_data_with_vars(ATermAppl a, ATermList *v, ATermList *d);

int add_process_eqn(ATermAppl e)
{
	ATermAppl name = fresh_var_name();
	ATermList vars = ATLgetArgument(e,2);
	ATermList data = ATLgetArgument(e,2);
	ATermAppl ni = generalise?replace_data_with_vars((ATermAppl) annotate(ATgetArgument(e,3)),&vars,&data):NULL;

//	if ( !generalise )
	{
		processes = ATappend(processes,(ATerm) ATmakeList4(ATgetArgument(ATAgetArgument(e,1),0),ATgetArgument(e,2),annotate(ATgetArgument(e,3)),(ATerm) gsMakeNil()));
ATfprintf(stderr,"\n\nprocess %i: %t\n\n",ATgetLength(processes)-1,ATgetLast(processes));
gsPrintPart(stderr,ATAelementAt((ATermList) ATgetLast(processes),2),0,0);
	}
	if ( generalise && (ATgetLength(vars) > ATgetLength(ATLgetArgument(e,2))) )
	{
//		processes = ATappend(processes,(ATerm) ATmakeList4(ATgetArgument(ATAgetArgument(e,1),0),(ATerm) vars,annotate((ATerm) ni),(ATerm) gsMakeNil()));
		processes = ATappend(processes,(ATerm) ATmakeList4((ATerm) name,(ATerm) vars,annotate((ATerm) ni),(ATerm) gsMakeNil()));
ATfprintf(stderr,"\n\nprocess %i: %t\n\n",ATgetLength(processes)-1,ATgetLast(processes));
gsPrintPart(stderr,ATAelementAt((ATermList) ATgetLast(processes),2),0,0);
		return ATgetLength(processes)-2;
	}

	return ATgetLength(processes)-1;
}

ATbool has_bounded_var(ATermAppl a) //XXX
{
	if ( gsIsDataVarId(a) )
	{
		return ATisAnnotated((ATerm) a);
	} else if ( gsIsDataAppl(a) )
	{
		return (ATbool) (has_bounded_var(ATAgetArgument(a,0)) || has_bounded_var(ATAgetArgument(a,1)));
	} else if ( gsIsOpId(a) )
	{
		return ATfalse;
	}

	gsErrorMsg("invalid data expr (%t)\n",a);
	exit(1);
}
ATerm replace_data_with_vars_gen(ATerm a, ATermList *v, ATermList *d, ATermAppl namebase)
{
	ATermList l,m;
	ATermAppl var;

	if ( ATisAppl(a) )
	{
		if ( gsIsDataAppl((ATermAppl) a) || gsIsOpId((ATermAppl) a) || gsIsDataVarId((ATermAppl) a) )
		{
/*			if ( ATindexOf(*d,a,0) >= 0 )
			{
				return ATelementAt(*v,ATindexOf(*d,a,0));
			} else {*/
//ATfprintf(stderr,"\nhas_bounded_var(%t) = %i\n",a,has_bounded_var((ATermAppl) a));
				if ( /*gsIsDataAppl((ATermAppl) a) &&*/ has_bounded_var((ATermAppl) a) )
				{
/*					if ( gsIsOpId(ATAgetArgument((ATermAppl) a,0)) )
					{
						return (ATerm) gsMakeDataAppl(ATAgetArgument(a,0),replace_data_with_vars_gen(ATgetArgument(a,1),v,d));
					} else {
						return (ATerm) gsMakeDataAppl(replace_data_with_vars_gen(ATgetArgument(a,0),v,d),replace_data_with_vars_gen(ATgetArgument(a,1),v,d));
					}*/
					return a;
				} else {
					if ( namebase == NULL )
					{
						var = get_new_var(gsMakeDataVarId(gsString2ATermAppl("d"),gsGetSort((ATermAppl) a)),v);
					} else {
						var = get_new_var(namebase,v);
					}
					*d = ATappend(*d,a);
					return (ATerm) var;
				}
/*			}*/
/*		} else if ( gsIsDataVarId((ATermAppl) a) && has_bounded_var(a) )
		{
			return a;*/
		} else {
			if ( gsIsProcess((ATermAppl) a) && (proc_id((ATermAppl) a) >= 0) )
			{
				ATermList vars = ATLelementAt(ATLelementAt(processes,proc_id((ATermAppl) a)),1);

				l = ATLgetArgument((ATermAppl) a,1);
				m = ATmakeList0();
				for (; !ATisEmpty(l); l=ATgetNext(l),vars=ATgetNext(vars))
				{
					m = ATinsert(m,replace_data_with_vars_gen(ATgetFirst(l),v,d,ATAgetFirst(vars)));
				}
				m = ATreverse(m);
				return (ATerm) ATsetArgument((ATermAppl) a,(ATerm) m,1);
			} else if ( gsIsSum((ATermAppl) a) )
			{
				return gsMakeSum(ATLgetArgument((ATermAppl) a,0),(ATermAppl) replace_data_with_vars_gen(ATgetArgument((ATermAppl) a,1),v,d,namebase));
			} else {
				l = ATgetArguments((ATermAppl) a);
				m = ATmakeList0();
				for (; !ATisEmpty(l); l=ATgetNext(l))
				{
					m = ATinsert(m,replace_data_with_vars_gen(ATgetFirst(l),v,d,namebase));
				}
				m = ATreverse(m);
				return (ATerm) ATmakeApplList(ATgetAFun((ATermAppl) a),m);
			}
		}
	} else if ( ATisList(a) ) {
			l = (ATermList) a;
			m = ATmakeList0();
			for (; !ATisEmpty(l); l=ATgetNext(l))
			{
				m = ATinsert(m,replace_data_with_vars_gen(ATgetFirst(l),v,d,namebase));
			}
			return (ATerm) ATreverse(m);
	}

	gsWarningMsg("unknown data expression to generalise (%t)\n",a);
	return a;
}
ATermAppl replace_data_with_vars(ATermAppl a, ATermList *v, ATermList *d)
{
	return (ATermAppl) replace_data_with_vars_gen((ATerm) a,v,d,NULL);
}
static ATermAppl initial_process;
int add_init(ATermAppl i)
{
	ATermAppl name;
	ATermList vars;
	ATermList data;
	ATermAppl ni;

if ( gsIsProcess(i) )
{
	initial_process = i;
	return proc_id(i);
} else {
	name = fresh_var_name();
	vars = ATmakeList0();
	data = ATmakeList0();
	ni = generalise?replace_data_with_vars((ATermAppl) annotate((ATerm) i),&vars,&data):i;

	processes = ATappend(processes,(ATerm) ATmakeList4((ATerm) name,(ATerm) vars,annotate((ATerm) ni),(ATerm) gsMakeNil()));
ATfprintf(stderr,"\n\nprocess %i: %t\n\n",ATgetLength(processes)-1,ATgetLast(processes));
gsPrintPart(stderr,ATAelementAt((ATermList) ATgetLast(processes),2),0,0);

	initial_process = gsMakeProcess(gsMakeProcVarId(name,vars),data);

	return ATgetLength(processes)-1;
}
}

ATbool is_done(int i)
{
	return ATisList(ATelementAt(ATLelementAt(processes,i),3));
}

void add_to_stack(int i)
{
	if ( !is_done(i) && (ATindexOf(todo_stack,(ATerm) ATmakeInt(i),0) == -1) )
	{
//ATfprintf(stderr,"\n\nadd_to_stack(%i)\n\n",i);
		todo_stack = ATappend(todo_stack,(ATerm) ATmakeInt(i));
	}
}

int pop_stack()
{
	int i;

	i = ATgetInt((ATermInt) ATgetFirst(todo_stack));
	todo_stack = ATgetNext(todo_stack);

	return i;
}

void remove_from_stack(int i)
{
	todo_stack = ATremoveElement(todo_stack,(ATerm) ATmakeInt(i));
}

ATbool empty_stack()
{
	return ATisEmpty(todo_stack);
}

void set_proc(int i, ATermList l)
{
	processes = ATreplace(processes,(ATerm) ATreplace(ATLelementAt(processes,i),(ATerm) l,3),i);
}

ATermList get_proc(int i, ATermList l)
{
	return (ATermList) substitute_vars(ATelementAt(ATLelementAt(processes,i),3),ATLelementAt(ATLelementAt(processes,i),1),l);
}

int proc_id(ATermAppl a)
{
	ATermList l = processes;
//ATfprintf(stderr,"\n\nprocid(%t)\n",a);

	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		if ( ATisEqual(ATelementAt(ATLgetFirst(l),0),ATgetArgument(ATAgetArgument(a,0),0)) && ATisEqual(get_sorts(ATLelementAt(ATLgetFirst(l),1)),ATgetArgument(ATAgetArgument(a,0),1)) )
		{
			return ATindexOf(processes,ATgetFirst(l),0);
		}
	}

	return -1;
}


/* Multiaction manipulation functions */

//ATermList split_sync(ATermAppl ma)
//{
//	ATermList l,m;
//	if ( gsIsAction(ma) || gsIsTau(ma) )
//	{
//		return ATmakeList2((ATerm) ma,(ATerm) ATmakeList0());
//	} else if ( gsIsSync(ma) )
//	{
//		l = split_sync(ATAgetArgument(ma,0));
//		m = split_sync(ATAgetArgument(ma,1));
/*		if ( gsIsTau(ATAelementAt(l,0)) )
		{
			return ATmakeList2((ATerm) ATAelementAt(m,0),(ATerm) ATconcat(ATLelementAt(l,1),ATLelementAt(m,1)));
		} else if ( gsIsTau(ATAelementAt(m,0)) )
		{
			return ATmakeList2((ATerm) ATAelementAt(l,0),(ATerm) ATconcat(ATLelementAt(l,1),ATLelementAt(m,1)));
		} else {*/
//			return ATmakeList2((ATerm) gsMakeSync(ATAelementAt(l,0),ATAelementAt(m,0)),(ATerm) ATconcat(ATLelementAt(l,1),ATLelementAt(m,1)));
/*		}*/
//	} else {
//		return ATmakeList2((ATerm) gsMakeTau(),(ATerm) ATmakeList1((ATerm) ma));
//	}
//}

/*ATbool is_maction(ATermAppl ma)
{
	if ( gsIsAction(ma) || gsIsTau(ma) )
	{
		return ATtrue;
	} else if ( gsIsSync(ma) )
	{
		return is_maction(ATAgetArgument(ma,0)) && is_maction(ATAgetArgument(ma,1));
	} else {
		return ATfalse;
	}
}*/

ATermList synch2actl(ATermAppl ma)
{
	if ( gsIsTau(ma) )
	{
		return ATmakeList0();
	} else if ( gsIsAction(ma) )
	{
		return ATmakeList1(ATgetArgument(ATAgetArgument(ma,0),0));
	} else if ( gsIsSync(ma) )
	{
		return ATconcat(synch2actl(ATAgetArgument(ma,0)),synch2actl(ATAgetArgument(ma,1)));
	} else {
		gsErrorMsg("invalid multiaction supplied to synch2actl (%t)\n",ma);
		exit(1);
	}
}

ATermAppl synch2mact(ATermAppl ma)
{
	return gsMakeMultActName(synch2actl(ma));
}

ATbool actl_eq(ATermList ma1, ATermList ma2)
{
	if ( ATisEmpty(ma1) )
	{
		return ATisEmpty(ma2);
	} else {
		if ( ATindexOf(ma2,ATgetFirst(ma1),0) >= 0 )
		{
			return actl_eq(ATgetNext(ma1),ATremoveElement(ma2,ATgetFirst(ma1)));
		} else {
			return ATfalse;
		}
	}
}

ATbool mact_eq(ATermAppl ma1, ATermAppl ma2)
{
	return actl_eq(ATLgetArgument(ma1,0),ATLgetArgument(ma2,0));
}

ATbool synch_in(ATermAppl ma, ATermList V)
{
	if ( gsIsTau(synch_reduce(ma)) )
	{
		return ATtrue;
	}

	ma = synch2mact(synch_reduce(ma));
	for (; !ATisEmpty(V); V=ATgetNext(V))
	{
		if ( mact_eq(ma,ATAgetFirst(V)) )
		{
			return ATtrue;
		}
	}

	return ATfalse;
}

// mact_cap(ma,H) == (ma \cap H)\not=\emptyset
ATbool synch_cap(ATermAppl ma, ATermList H)
{
	if ( gsIsTau(ma) )
	{
		return ATfalse;
	} else if ( gsIsAction(ma) )
	{
		if ( ATindexOf(H,ATgetArgument(ATAgetArgument(ma,0),0),0) >= 0 )
		{
			return ATtrue;
		} else {
			return ATfalse;
		}
	} else if ( gsIsSync(ma) )
	{
		return (ATbool) (synch_cap(ATAgetArgument(ma,0),H) || synch_cap(ATAgetArgument(ma,1),H));
	} else {
		gsErrorMsg("invalid multiaction supplied to synch_cap (%t)\n",ma);
		exit(1);
	}
}

ATermAppl synch_hide(ATermAppl ma, ATermList I)
{
	ATermAppl a,b;

	if ( gsIsTau(ma) )
	{
		return ma;
	} else if ( gsIsAction(ma) )
	{
		if ( ATindexOf(I,ATgetArgument(ATAgetArgument(ma,0),0),0) >= 0 )
		{
			return gsMakeTau();
		} else {
			return ma;
		}
	} else if ( gsIsSync(ma) )
	{
		a = synch_hide(ATAgetArgument(ma,0),I);
		b = synch_hide(ATAgetArgument(ma,1),I);
		if ( gsIsTau(a) )
		{
			return b;
		} else if ( gsIsTau(b) )
		{
			return a;
		} else {
			return gsMakeSync(a,b);
		}
	} else {
		gsErrorMsg("invalid multiaction supplied to synch_hide (%t)\n",ma);
		exit(1);
	}
}

ATermAppl synch_rename(ATermAppl ma, ATermList R)
{
	if ( gsIsTau(ma) )
	{
		return ma;
	} else if ( gsIsAction(ma) )
	{
		for (; !ATisEmpty(R); R=ATgetNext(R))
		{
			if ( ATisEqual(ATgetArgument(ATgetArgument(ma,0),0),ATgetArgument(ATgetFirst(R),0)) )
			{
				//XXX declare action+type if needed!
				ma = gsMakeAction(ATsetArgument(ATAgetArgument(ma,0),ATgetArgument(ATgetFirst(R),1),0),ATLgetArgument(ma,1));
				break;
			}
		}
		return ma;
	} else if ( gsIsSync(ma) )
	{
		return gsMakeSync(synch_rename(ATAgetArgument(ma,0),R),synch_rename(ATAgetArgument(ma,1),R));
	} else {
		gsErrorMsg("invalid multiaction supplied to synch_rename (%t)\n",ma);
		exit(1);
	}
}

ATermList synch2list(ATermAppl ma)
{
	if ( gsIsAction(ma) )
	{
		return ATmakeList1((ATerm) ma);
	} else if ( gsIsTau(ma) )
	{
		return ATmakeList0();
	} else if ( gsIsSync(ma) )
	{
		return ATconcat(synch2list(ATAgetArgument(ma,0)),synch2list(ATAgetArgument(ma,1)));
	} else {
		gsErrorMsg("invalid multiaction supplied to synch2list (%t)\n",ma);
		exit(1);
	}
}

ATermAppl list2synch(ATermList mal)
{
	if ( ATisEmpty(mal) )
	{
		return gsMakeTau();
	} else if ( ATgetLength(mal) == 1 )
	{
		return ATAgetFirst(mal);
	} else {
		return gsMakeSync(ATAgetFirst(mal),list2synch(ATgetNext(mal)));
	}
}

ATermAppl make_xi(ATermList l)
{
	ATermAppl c, d;
	int b;
	ATermList a, t, u;

	if ( ATisEmpty(l) )
	{
		gsWarningMsg("multiaction supplied to make_xi should not be empty\n");
		return linTrue;
	} else {
		a = ATLgetArgument(ATAgetFirst(l),1);
		l = ATgetNext(l);
		c = linTrue;
		for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			t = a;
			u = ATLgetArgument(ATAgetFirst(l),1);
			b = 1;
			d = linTrue;
			for (; !ATisEmpty(t); t=ATgetNext(t), u=ATgetNext(u))
			{
				if ( b )
				{
					d = gsMakeDataExprEq(ATAgetFirst(t),ATAgetFirst(u));
					b = 0;
				} else {
					d = gsMakeDataExprAnd(d,gsMakeDataExprEq(ATAgetFirst(t),ATAgetFirst(u)));
				}
			}
			c = gsRewriteTerm(gsMakeDataExprAndWithTrueCheck(c,d));
		}
		return c;
	}
}

ATermList mu(ATermList m)
{
	ATermList r;

	r = ATmakeList0();
	for (; !ATisEmpty(m); m=ATgetNext(m))
	{
		r = ATinsert(r,ATgetArgument(ATAgetArgument(ATAgetFirst(m),0),0));
	}

	return ATreverse(r);
}

ATbool find_comm(ATermList m, ATermList C, ATermAppl *c)
{
	for (; !ATisEmpty(C); C=ATgetNext(C))
	{
//ATfprintf(stderr,"cmp  %t  %t\n\n",m,ATLgetArgument(ATAgetArgument(ATAgetFirst(C),0),0));
		if ( actl_eq(m,ATLgetArgument(ATAgetArgument(ATAgetFirst(C),0),0)) )
		{
			*c = ATAgetArgument(ATAgetFirst(C),1);
			return ATtrue;
		}
	}

	return ATfalse;
}

ATermAppl param_eq(ATermList m, ATermList n)
{
	ATermAppl c;
	int b;

	if ( ATgetLength(m) != ATgetLength(n) )
	{
		return linFalse;
	}

	c = linTrue;
	b = 1;
	for (; !ATisEmpty(m); m=ATgetNext(m), n=ATgetNext(n))
	{
		if ( !ATisEqual(gsGetSort(ATAgetFirst(m)),gsGetSort(ATAgetFirst(n))) )
		{
			return linFalse;
		}
		if ( b )
		{
			c = gsMakeDataExprEq(ATAgetFirst(m),ATAgetFirst(n));
			b = 0;
		} else {
			c = gsMakeDataExprAnd(c,gsMakeDataExprEq(ATAgetFirst(m),ATAgetFirst(n)));
		}
	}

	return gsRewriteTerm(c);
}

ATermList calc_comm(ATermList m, ATermList d, ATermList w, ATermList n, ATermList C)
{
	ATermList s,l;
	ATermAppl c;

	if ( ATisEmpty(n) )
	{
//ATfprintf(stderr,"mu(%t) = %t\n\n",m,mu(m));
		if ( find_comm(mu(m),C,&c) )
		{
			s = mactl_comm(w,C);
			if ( gsIsNil(c) )
			{
				return s;
			} else {
				l = ATmakeList0();
				for (; !ATisEmpty(s); s=ATgetNext(s))
				{
					l = ATinsert(l,(ATerm) ATmakeList2((ATerm) ATinsert(ATLelementAt(ATLgetFirst(s),0),(ATerm) gsMakeAction(gsMakeActId(c,ATLgetArgument(ATAgetArgument(ATAgetFirst(m),0),1)),d)), ATelementAt(ATLgetFirst(s),1)));
				}
				return ATreverse(l);
			}
		} else {
			return ATmakeList0();
		}
	} else {
		s = calc_comm(ATinsert(m,ATgetFirst(n)),d,w,ATgetNext(n),C);
		l = ATmakeList0();
		for (; !ATisEmpty(s); s=ATgetNext(s))
		{
			c = param_eq(ATLgetArgument(ATAgetFirst(n),1),d);
			if ( !ATisEqual(c,linFalse) )
			{
				ATermAppl a = gsRewriteTerm(gsMakeDataExprAndWithTrueCheck(c,ATAelementAt(ATLgetFirst(s),1)));
				if ( !ATisEqual(a,linFalse) )
				{
					l = ATinsert(l,(ATerm) ATmakeList2(ATelementAt(ATLgetFirst(s),0),(ATerm) a));
				}
			}
		}
		s = ATconcat(l,calc_comm(m,d,ATinsert(w,ATgetFirst(n)),ATgetNext(n),C));
//XXX		s = ATconcat(ATreverse(l),calc_comm(m,d,ATinsert(w,ATgetFirst(n)),ATgetNext(n),C));
		return s;
	}
}

/*ATermList calc_comm_aux(ATermAppl a, ATermList C, ATermList mal, ATermList c, ATermAppl ca, ATermList b, ATermList r)
{
	ATermList m,o,p;

	if ( ATisEmpty(c) )
	{
		p = ATconcat(mal,r);
		o = mactl_comm(p,C);
		m = ATmakeList0();
		for (; !ATisEmpty(o); o=ATgetNext(o))
		{
			//XXX check whether action exists!
			if ( gsIsNil(ca) )
			{
				m = ATinsert(m,(ATerm) ATmakeList2((ATerm) ATAelementAt(ATLgetFirst(o),0),(ATerm) gsMakeDataExprAndWithTrueCheck(make_xi(b),ATAelementAt(ATLgetFirst(o),1))));
			} else {
				if ( gsIsTau(ATAelementAt(ATLgetFirst(o),0)) )
				{
					m = ATinsert(m,(ATerm) ATmakeList2((ATerm) gsMakeAction(gsMakeActId(ca,ATLgetArgument(ATAgetArgument(a,0),1)),ATLgetArgument(a,1)),(ATerm) gsMakeDataExprAndWithTrueCheck(make_xi(b),ATAelementAt(ATLgetFirst(o),1))));
				} else {
					m = ATinsert(m,(ATerm) ATmakeList2((ATerm) gsMakeSync(gsMakeAction(gsMakeActId(ca,ATLgetArgument(ATAgetArgument(a,0),1)),ATLgetArgument(a,1)),ATAelementAt(ATLgetFirst(o),0)),(ATerm) gsMakeDataExprAndWithTrueCheck(make_xi(b),ATAelementAt(ATLgetFirst(o),1))));
				}
			}
		}
		return ATreverse(m);
	} else if ( ATisEmpty(mal) )
	{
		return ATmakeList0();
	} else {
		if ( ATisEqual(ATgetArgument(ATAgetArgument(a,0),1),ATgetArgument(ATAgetArgument(ATAgetFirst(mal),0),1)) && (ATindexOf(c,ATgetArgument(ATAgetArgument(ATAgetFirst(mal),0),0),0) >= 0) )
		{
			return ATconcat(calc_comm_aux(a,C,ATgetNext(mal),ATremoveElement(c,ATgetArgument(ATAgetArgument(ATAgetFirst(mal),0),0)),ca,ATappend(b,ATgetFirst(mal)),r),calc_comm_aux(a,C,ATgetNext(mal),c,ca,b,ATappend(r,ATgetFirst(mal))));
		} else {
			return calc_comm_aux(a,C,ATgetNext(mal),c,ca,b,ATappend(r,ATgetFirst(mal)));
		}
	}
}

ATermList calc_comm(ATermAppl a, ATermList mal, ATermList C)
{
	ATermList l,n,o,r;

	r = ATmakeList0();
	for (l=C; !ATisEmpty(l); l=ATgetNext(l))
	{
		o = ATLgetArgument(ATAgetArgument(ATAgetFirst(l),0),0);
		if ( ATindexOf(o,ATgetArgument(ATAgetArgument(a,0),0),0) >= 0 )
		{
			o = ATremoveElement(o,ATgetArgument(ATAgetArgument(a,0),0));
			n = ATmakeList1((ATerm) a);
			r = ATconcat(r,calc_comm_aux(a,C,mal,o,ATAgetArgument(ATAgetFirst(l),1),n,ATmakeList0()));
		}
	}

	return r;
}*/

ATermList map_concat(ATermAppl a, ATermList m) //XXX
{
	ATermList r;

	if ( gsIsTau(a) )
	{
		return m;
	}

	r = ATmakeList0();
	for (; !ATisEmpty(m); m=ATgetNext(m))
	{
		if ( gsIsTau(ATAgetFirst(ATLgetFirst(m))) )
		{
			r = ATinsert(r,(ATerm) ATmakeList2((ATerm) a,ATgetLast(ATLgetFirst(m))));
		} else {
			r = ATinsert(r,(ATerm) ATmakeList2((ATerm) gsMakeSync(a,ATAgetFirst(ATLgetFirst(m))),ATgetLast(ATLgetFirst(m))));
		}
	}

	return ATreverse(r);
}
ATermList mactl_comm(ATermList mal, ATermList C)
{
	ATermList s,t,l;
	ATermAppl b;
//ATfprintf(stderr,"mal=%t\n\n",mal);
	if ( ATisEmpty(mal) )
	{
		return ATmakeList1((ATerm) ATmakeList2((ATerm) mal,(ATerm) linTrue));
	} else {
		s = calc_comm(ATmakeList1(ATgetFirst(mal)),ATLgetArgument(ATAgetFirst(mal),1),ATmakeList0(),ATgetNext(mal),C);
		b = linTrue;
		l = s;
		for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			b = gsMakeDataExprAndWithCheck(b,gsMakeDataExprNotWithCheck(ATAelementAt(ATLgetFirst(l),1)));
		}
		b = gsRewriteTerm(b);
		if ( !ATisEqual(b,linFalse) )
		{
			t = mactl_comm(ATgetNext(mal),C);
//ATfprintf(stderr,"g( %t  ,  %t ) =   %t\n\n",ATgetNext(mal),C,t);
			l = t;
//XXX			s = ATreverse(s);
			for (; !ATisEmpty(l); l=ATgetNext(l))
			{
				ATermAppl a = gsRewriteTerm(gsMakeDataExprAndWithCheck(b,ATAelementAt(ATLgetFirst(l),1)));
				if ( !ATisEqual(a,linFalse) )
				{
					s = ATinsert(s,(ATerm) ATmakeList2((ATerm) ATinsert(ATLelementAt(ATLgetFirst(l),0),ATgetFirst(mal)),(ATerm) a));
				}
			}
//XXX			s = ATreverse(s);
		}
		return s;
	}

/*	r = ATmakeList0();
	m = ATmakeList0();
	for (l=mal; !ATisEmpty(l); l=ATgetNext(l))
	{
		r = ATconcat(r,map_concat(list2synch(m),calc_comm(ATAgetFirst(l),ATgetNext(l),C)));
		m = ATappend(m,ATgetFirst(l));
	}

	if ( ATisEmpty(r) )
	{
		return ATmakeList1((ATerm) ATmakeList2((ATerm) list2synch(mal),(ATerm) linTrue));
	} else {
		b = 1;
		d = linFalse;
		for (l=r; !ATisEmpty(l); l=ATgetNext(l))
		{
			if ( b )
			{
				b = 0;
				d = ATAelementAt(ATLgetFirst(l),1);
			} else {
				d = gsMakeDataExprOr(d,ATAelementAt(ATLgetFirst(l),1));
			}
		}
		d = gsMakeDataExprNot(d);

		return ATappend(r,(ATerm) ATmakeList2((ATerm) list2synch(mal),(ATerm) d));
	}*/
}

ATermList synch_comm(ATermAppl ma, ATermList C)
{
	ATermList s,l;

//ATfprintf(stderr,"\n\nsynch_comm( %t ,  %t ) =  %t\n\n",ma,C,mactl_comm(synch2list(ma),C));
//ATfprintf(stderr,"synch_comm(%t  ,  %t)\n\n",ma,C);
	s = mactl_comm(synch2list(ma),C);
//ATfprintf(stderr,"result = %t\n\n",s);
	l = ATmakeList0();
	for (; !ATisEmpty(s); s=ATgetNext(s))
	{
		l = ATinsert(l,(ATerm) ATmakeList2((ATerm) list2synch(ATLelementAt(ATLgetFirst(s),0)), ATelementAt(ATLgetFirst(s),1)));
	}
	return ATreverse(l);
}

ATermAppl synch_reduce(ATermAppl ma)
{
	ATermAppl ma1,ma2;

	if ( gsIsAction(ma) || gsIsTau(ma) )
	{
		return ma;
	} else if ( gsIsSync(ma) )
	{
		ma1 = synch_reduce(ATAgetArgument(ma,0));
		ma2 = synch_reduce(ATAgetArgument(ma,1));
		if ( gsIsTau(ma1) )
		{
			return ma2;
		} else if ( gsIsTau(ma2) )
		{
			return ma1;
		} else {
			return gsMakeSync(ma1,ma2);
		}
	} else {
		gsErrorMsg("invalid multiaction supplied to synch_reduce (%t)\n",ma);
		exit(1);
	}
}


/* List manipulation functions */

ATbool list_eq(ATermList l1, ATermList l2)
{
	if ( ATisEmpty(l1) )
	{
		return ATisEmpty(l2);
	} else {
		if ( ATindexOf(l2,ATgetFirst(l1),0) >= 0 )
		{
			return list_eq(ATgetNext(l1),ATremoveElement(l2,ATgetFirst(l1)));
		} else {
			return ATfalse;
		}
	}
}

ATermList merge_list(ATermList l1, ATermList l2)
{
	l1 = ATreverse(l1);
	for (;!ATisEmpty(l2); l2=ATgetNext(l2))
	{
		if ( ATindexOf(l1,ATgetFirst(l2),0) == -1 )
		{
			l1 = ATinsert(l1,ATgetFirst(l2));
		}
	}
	l1 = ATreverse(l1);

	return l1;
}

ATermList list_minus(ATermList l1, ATermList l2)
{
	for (;!ATisEmpty(l2); l2=ATgetNext(l2))
	{
		l1 = ATremoveElement(l1,ATgetFirst(l2));
	}

	return l1;
}


/* Matching functions */

ATbool match_data(ATermAppl a, ATermAppl m, ATermList l, ATermTable r)
{
	if ( gsIsDataVarId(a) )
	{
		if ( ATindexOf(l,(ATerm) a,0) >=0 )
		{
			if ( ATtableGet(r,(ATerm) a) == NULL )
			{
				ATtablePut(r,(ATerm) a,(ATerm) m);
				return ATtrue;
			} else {
				return ATisEqual(ATtableGet(r,(ATerm) a),(ATerm) m);
			}
		} else {
			return ATisEqual((ATerm) a,(ATerm) m);
		}
	}
	else
	if ( gsIsOpId(a) )
	{
		return ATisEqual((ATerm) a,(ATerm) m);
	}
	else
	if ( gsIsDataAppl(a) )
	{
		if ( gsIsDataAppl(m) )
		{
			if ( match_data(ATAgetArgument(a,0),ATAgetArgument(m,0),l,r) )
			{
				return match_data(ATAgetArgument(a,1),ATAgetArgument(m,1),l,r);
			} else {
				return ATfalse;
			}
		} else {
			return ATfalse;
		}
	}
	// XXX
	else
	{
		ATermList n,o;
		if ( ATisAppl((ATerm) a) )
		{
			if ( !ATisAppl((ATerm) m) || !ATisEqualAFun(ATgetAFun(a),ATgetAFun(m)) )
			{
				return ATfalse;
			}
			n = ATgetArguments(a);
			o = ATgetArguments(m);
		} else {
			if ( !ATisList((ATerm) m) )
			{
				return ATfalse;
			}
			n = (ATermList) a;
			o = (ATermList) m;
		}
		for (; !ATisEmpty(n); n=ATgetNext(n),o=ATgetNext(o))
		{
			if ( !match_data(ATAgetFirst(n),ATAgetFirst(o),l,r) )
			{
				return ATfalse;
			}
		}
		return ATtrue;
	}
	
	gsWarningMsg("unknown data expression (%t)\n",a);
	return ATfalse;
}

ATbool match_proc(ATermAppl a, ATermAppl m, ATermList l, ATermTable r)
{
	ATermList l1,l2;

	if ( !ATisEqualAFun(ATgetAFun(a),ATgetAFun(m)) )
	{
//fprintf(stderr,"i!\n");
		return ATfalse;
	}

	if ( gsIsAction(a) || gsIsProcess(a) )
	{
		if ( !ATisEqual(ATgetArgument(a,0),ATgetArgument(m,0)) )
		{
//fprintf(stderr,"h!\n");
			return ATfalse;
		}

		l1 = ATLgetArgument(a,1);
		l2 = ATLgetArgument(m,1);
		if ( ATgetLength(l1) != ATgetLength(l2) )
		{
//fprintf(stderr,"g!\n");
			return ATfalse;
		}
		for (;!ATisEmpty(l1); l1=ATgetNext(l1), l2=ATgetNext(l2))
		{
			if ( !match_data(ATAgetFirst(l1),ATAgetFirst(l2),l,r) )
			{
//ATfprintf(stderr,"f! %t    %t\n",ATAgetFirst(l1),ATAgetFirst(l2));
				return ATfalse;
			}
		}
		return ATtrue;
	}
	else
	if ( gsIsDelta(a) || gsIsTau(a) )
	{
		return ATtrue;
	}
	else
	if ( gsIsSum(a) )
	{
		if ( !list_eq(ATLgetArgument(a,0),ATLgetArgument(m,0)) )
		{
//fprintf(stderr,"e!\n");
			return ATfalse;
		}

		return match_proc(ATAgetArgument(a,1),ATAgetArgument(m,1),list_minus(l,ATLgetArgument(a,0)),r);
	}
	else
	if ( gsIsRestrict(a) || gsIsHide(a) || gsIsRename(a) || gsIsComm(a) ||  gsIsAllow(a) )
	{
		return match_proc(ATAgetArgument(a,1),ATAgetArgument(m,1),l,r);
	}
	else
	if ( gsIsSync(a) || gsIsSeq(a) ||  gsIsMerge(a) || gsIsLMerge(a) || gsIsChoice(a) )
	{
		if ( match_proc(ATAgetArgument(a,0),ATAgetArgument(m,0),l,r) )
		{
			return match_proc(ATAgetArgument(a,1),ATAgetArgument(m,1),l,r);
		} else {
//fprintf(stderr,"d!\n");
			return ATfalse;
		}
	}
	else
	if ( gsIsCond(a) )
	{
		if ( match_data(ATAgetArgument(a,0),ATAgetArgument(m,0),l,r) )
		{
			return match_proc(ATAgetArgument(a,1),ATAgetArgument(m,1),l,r) && match_proc(ATAgetArgument(a,2),ATAgetArgument(m,2),l,r);
		} else {
//fprintf(stderr,"c!\n");
			return ATfalse;
		}
	}
	
	gsWarningMsg("unknown process (%t)\n",a);
	return ATfalse;
}

ATbool match(ATermAppl a, ATermList l, ATermList *r)
{
	ATermList m = ATLelementAt(l,1);
	ATermTable t = ATtableCreate(ATgetLength(m),100);
	ATerm v;

	if ( match_proc(gsMakeProcess(gsMakeProcVarId(ATAelementAt(l,0),get_sorts(ATLelementAt(l,1))),ATLelementAt(l,1)),a,m,t) )
	{
		*r = ATmakeList0();
		for (;!ATisEmpty(m);m=ATgetNext(m))
		{
			v = ATtableGet(t,ATgetFirst(m));
			if ( v == NULL )
			{
//fprintf(stderr,"a!\n");
				ATtableDestroy(t);
				return ATfalse;
			}
			*r = ATinsert(*r,v);
		}
		*r = ATreverse(*r);
		ATtableDestroy(t);
		return ATtrue;
	} else if ( match_proc(ATAelementAt(l,2),a,m,t) )
	{
		*r = ATmakeList0();
		for (;!ATisEmpty(m);m=ATgetNext(m))
		{
			v = ATtableGet(t,ATgetFirst(m));
			if ( v == NULL )
			{
//ATfprintf(stderr,"b! %t %t\n",ATelementAt(l,1),ATtableKeys(t));
				ATtableDestroy(t);
				return ATfalse;
			}
			*r = ATinsert(*r,v);
		}
		*r = ATreverse(*r);
		ATtableDestroy(t);
		return ATtrue;
	} else {
		ATtableDestroy(t);
		return ATfalse;
	}
}


/* Misc. functions */

ATermList get_sorts(ATermList l)
{
	ATermList m = ATmakeList0();

	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		m = ATinsert(m,ATgetArgument(ATAgetFirst(l),1));
	}

	return ATreverse(m);
}

ATermList makeSubsts(ATermList l, ATermList m)
{
	ATermList r;

	r = ATmakeList0();
	for (; !ATisEmpty(l); l=ATgetNext(l),m=ATgetNext(m))
	{
		r = ATinsert(r,(ATerm) gsMakeSubst(ATgetFirst(l),ATgetFirst(m)));
	}
	r = ATreverse(r);

	return r;
}

ATermAppl get_proc_call(ATermAppl a, ATermList c)
{
	ATermList l,m,t,arg_list,s;
	ATermAppl var_name;
	ATerm a2;

	for (l=processes; !ATisEmpty(l); l=ATgetNext(l))
	{
		if ( match(a,ATLgetFirst(l),&m) )
		{
			add_to_stack(ATindexOf(processes,ATgetFirst(l),0));
			return gsMakeProcess(gsMakeProcVarId(ATAelementAt(ATLgetFirst(l),0),get_sorts(ATLelementAt(ATLgetFirst(l),1))),m);
		}
	}
	
	var_name = fresh_var_name();
	m = get_vars(a);
	a2 = (ATerm) a;
	l = (ATermList) remove_indices_context((ATerm) m,&a2,&c); // XXX check
	s = makeSubsts(l,m);
	a = (ATermAppl) a2;
	arg_list = (ATermList) remove_indices_context((ATerm) l,&a2,&c); // XXX double??
	
	if ( generalise )
	{
		ATermList data = ATmakeList0();
		arg_list = ATmakeList0();
		a2 = generalise?replace_data_with_vars(a2,&arg_list,&data):a2;
		//arg_list = get_vars(a2);
	}

	t = ATmakeList4((ATerm) var_name,(ATerm) arg_list,a2,(ATerm) gsMakeNil());
	//arg_list = get_vars(a);
	//t = ATmakeList4((ATerm) var_name,(ATerm) arg_list,(ATerm) a,(ATerm) gsMakeNil());
	processes = ATappend(processes,(ATerm) t);
ATfprintf(stderr,"\n\nprocess %i: %t\n\n",ATgetLength(processes)-1,ATgetLast(processes));
gsPrintPart(stderr,ATelementAt((ATermList) ATgetLast(processes),2),0,0);
	add_to_stack(ATgetLength(processes)-1);
	if ( !match(a,t,&m) )
	{
		gsErrorMsg("introduced process does not match with original (%t , %t)\n",t,a);
		exit(1);
	}
	m = (ATermList) gsSubstValues(s,(ATerm) m,true);

	return gsMakeProcess(gsMakeProcVarId(var_name,get_sorts(arg_list)),m);
}

/* Main linearisation functions */

ATermList get_firsts(ATermAppl t)
{
	ATermList l,m,n,o;
	ATermAppl u;
	int j;

	if ( gsIsAction(t) )
	{
		return ATmakeList1((ATerm) ATmakeList4((ATerm) ATmakeList0(),(ATerm) t,(ATerm) gsMakeNil(),(ATerm) linTrue));
	}
	if ( gsIsProcess(t) )
	{
		j = proc_id(t);
		if ( generalise || ATisEmpty(ATLgetArgument(t,1)) )
		{
			if ( !is_done(j) )
			{
				linearise(j);
				remove_from_stack(j);
			}
			return get_proc(j,ATLgetArgument(t,1));
		} else {
			return get_firsts((ATermAppl) substitute_vars(ATelementAt(ATLelementAt(processes,j),2),ATLelementAt(ATLelementAt(processes,j),1),ATLgetArgument(t,1)));
		}
	}
	if ( gsIsDelta(t) )
	{
		return ATmakeList0();
	}
	if ( gsIsTau(t) )
	{
		return ATmakeList1((ATerm) ATmakeList4((ATerm) ATmakeList0(),(ATerm) t,(ATerm) gsMakeNil(),(ATerm) linTrue));
	}
	if ( gsIsSum(t) )
	{
		l = get_firsts(ATAgetArgument(t,1));
		m = ATmakeList0();
		for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			m = ATinsert(m,(ATerm) ATmakeList4(
						(ATerm) ATconcat((ATermList) increase_index((ATerm) make_indices(ATLgetArgument(t,0)),ATgetLength(ATelementAt(ATLgetFirst(l),0))),ATLelementAt(ATLgetFirst(l),0)),
						ATelementAt(ATLgetFirst(l),1),
						ATelementAt(ATLgetFirst(l),2),
						ATelementAt(ATLgetFirst(l),3)
//						increase_index(ATelementAt(ATLgetFirst(l),1),ATgetLength(ATLgetArgument(t,0))),
//						increase_index(ATelementAt(ATLgetFirst(l),2),ATgetLength(ATLgetArgument(t,0))),
//						increase_index(ATelementAt(ATLgetFirst(l),3),ATgetLength(ATLgetArgument(t,0)))
						));
		}
		return ATreverse(m);
	}
	if ( gsIsRestrict(t) )
	{
		l = get_firsts(ATAgetArgument(t,1));
		m = ATmakeList0();
		for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			if ( !synch_cap(ATAelementAt(ATLgetFirst(l),1),ATLgetArgument(t,0)) )
			{
				if ( gsIsNil(ATAelementAt(ATLgetFirst(l),2)) )
				{
					m = ATinsert(m,(ATerm) ATLgetFirst(l));
				} else {
					m = ATinsert(m,(ATerm) ATreplace(ATLgetFirst(l),(ATerm) gsMakeRestrict(ATLgetArgument(t,0),ATAelementAt(ATLgetFirst(l),2)),2));
				}
			}
		}
		return ATreverse(m);
	}
	if ( gsIsHide(t) )
	{
		l = get_firsts(ATAgetArgument(t,1));
		m = ATmakeList0();
		for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			if ( gsIsNil(ATAelementAt(ATLgetFirst(l),2)) )
			{
				m = ATinsert(m,(ATerm) ATreplace(ATLgetFirst(l),(ATerm) synch_hide(ATAelementAt(ATLgetFirst(l),1),ATLgetArgument(t,0)),1));
			} else {
				m = ATinsert(m,(ATerm) ATreplace(ATreplace(ATLgetFirst(l),(ATerm) gsMakeHide(ATLgetArgument(t,0),ATAelementAt(ATLgetFirst(l),2)),2),(ATerm) synch_hide(ATAelementAt(ATLgetFirst(l),1),ATLgetArgument(t,0)),1));
			}
		}
		return ATreverse(m);
	}
	if ( gsIsRename(t) )
	{
		l = get_firsts(ATAgetArgument(t,1));
		m = ATmakeList0();
		for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			if ( gsIsNil(ATAelementAt(ATLgetFirst(l),2)) )
			{
				m = ATinsert(m,(ATerm) ATreplace(ATLgetFirst(l),(ATerm) synch_rename(ATAelementAt(ATLgetFirst(l),1),ATLgetArgument(t,0)),1));
			} else {
				m = ATinsert(m,(ATerm) ATreplace(ATreplace(ATLgetFirst(l),(ATerm) gsMakeRename(ATLgetArgument(t,0),ATAelementAt(ATLgetFirst(l),2)),2),(ATerm) synch_rename(ATAelementAt(ATLgetFirst(l),1),ATLgetArgument(t,0)),1));
			}
		}
		return ATreverse(m);
	}
	if ( gsIsComm(t) )
	{
		l = get_firsts(ATAgetArgument(t,1));
		m = ATmakeList0();
		for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			n = synch_comm(ATAelementAt(ATLgetFirst(l),1),ATLgetArgument(t,0));
			for (; !ATisEmpty(n); n=ATgetNext(n))
			{
				ATermAppl a = gsRewriteTerm(gsMakeDataExprAndWithTrueCheck(ATAelementAt(ATLgetFirst(n),1),ATAelementAt(ATLgetFirst(l),3)));
				if ( !ATisEqual(a,linFalse) )
				{
					if ( gsIsNil(ATAelementAt(ATLgetFirst(l),2)) )
					{
						m = ATinsert(m,(ATerm) ATmakeList4(
									ATelementAt(ATLgetFirst(l),0),
									ATelementAt(ATLgetFirst(n),0),
									(ATerm) gsMakeNil(),
									(ATerm) a
									));
					} else {
						m = ATinsert(m,(ATerm) ATmakeList4(
									ATelementAt(ATLgetFirst(l),0),
									ATelementAt(ATLgetFirst(n),0),
									(ATerm) gsMakeComm(ATLgetArgument(t,0),ATAelementAt(ATLgetFirst(l),2)),
									(ATerm) a
									));
					}
				}
			}
		}
		return ATreverse(m);
	}
	if ( gsIsAllow(t) )
	{
		l = get_firsts(ATAgetArgument(t,1));
		m = ATmakeList0();
		for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			if ( synch_in(ATAelementAt(ATLgetFirst(l),1),ATLgetArgument(t,0)) )
			{
				if ( gsIsNil(ATAelementAt(ATLgetFirst(l),2)) )
				{
					m = ATinsert(m,(ATerm) ATLgetFirst(l));
				} else {
					m = ATinsert(m,(ATerm) ATreplace(ATLgetFirst(l),(ATerm) gsMakeAllow(ATLgetArgument(t,0),ATAelementAt(ATLgetFirst(l),2)),2));
				}
			}
		}
		return ATreverse(m);
	}
	if ( gsIsSync(t) )
	{
		l = get_firsts(ATAgetArgument(t,0));
		m = get_firsts(ATAgetArgument(t,1));
		o = ATmakeList0();
		for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			for (n=m; !ATisEmpty(n); n=ATgetNext(n))
			{
				if ( gsIsNil(ATAelementAt(ATLgetFirst(l),2)) )
				{
					u = ATAelementAt(ATLgetFirst(n),2);
				} else if ( gsIsNil(ATAelementAt(ATLgetFirst(n),2)) )
				{
					u = (ATermAppl) increase_index(ATelementAt(ATLgetFirst(l),2),ATgetLength(ATelementAt(ATLgetFirst(n),0)));
				} else {
					u = gsMakeMerge((ATermAppl) increase_index(ATelementAt(ATLgetFirst(l),2),ATgetLength(ATelementAt(ATLgetFirst(n),0))),ATAelementAt(ATLgetFirst(n),2));
				}

				ATermAppl a = gsRewriteTerm(gsMakeDataExprAndWithTrueCheck((ATermAppl) increase_index(ATelementAt(ATLgetFirst(l),3),ATgetLength(ATelementAt(ATLgetFirst(n),0))),ATAelementAt(ATLgetFirst(n),3)));
				if ( !ATisEqual(a,linFalse) )
				{
					o = ATinsert(o,(ATerm) ATmakeList4(
								(ATerm) ATconcat((ATermList) increase_index((ATerm) make_indices(ATLelementAt(ATLgetFirst(l),0)),ATgetLength(ATelementAt(ATLgetFirst(n),0))),ATLelementAt(ATLgetFirst(n),0)),
								(ATerm) gsMakeSync((ATermAppl) increase_index(ATelementAt(ATLgetFirst(l),1),ATgetLength(ATelementAt(ATLgetFirst(n),0))),ATAelementAt(ATLgetFirst(n),1)),
								(ATerm) u,
								(ATerm) a
//							(ATerm) ATconcat((ATermList) make_indices(ATLelementAt(ATLgetFirst(l),0)),(ATermList) increase_index(ATelementAt(ATLgetFirst(n),0),ATgetLength(ATLelementAt(ATLgetFirst(l),0)))),
		//					(ATerm) ATconcat(ATLelementAt(ATLgetFirst(l),0),ATLelementAt(ATLgetFirst(n),0)),
//							(ATerm) gsMakeSync(ATAelementAt(ATLgetFirst(l),1),(ATermAppl) increase_index(ATelementAt(ATLgetFirst(n),1),ATgetLength(ATLelementAt(ATLgetFirst(l),0)))),
//							(ATerm) gsMakeMerge(ATAelementAt(ATLgetFirst(l),2),(ATermAppl) increase_index(ATelementAt(ATLgetFirst(n),2),ATgetLength(ATLelementAt(ATLgetFirst(l),0)))),
//							(ATerm) gsMakeDataExprAndWithTrueCheck(ATAelementAt(ATLgetFirst(l),3),(ATermAppl) increase_index(ATelementAt(ATLgetFirst(n),3),ATgetLength(ATLelementAt(ATLgetFirst(l),0))))
								));
				}
			}
		}
		return ATreverse(o);
/*		l = split_sync(t);
		if ( ATisEmpty(ATLelementAt(l,1)) )
		{
			return ATmakeList1((ATerm) ATmakeList4((ATerm) ATmakeList0(),(ATerm) t,(ATerm) gsMakeNil(),(ATerm) linTrue));
		} else {
			m = get_firsts(ATAgetArgument(t,1));
			//XXX
			return m;
		}*/
	}
	if ( gsIsSeq(t) )
	{
		l = get_firsts(ATAgetArgument(t,0));
		m = ATmakeList0();
		for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			if ( gsIsNil(ATAelementAt(ATLgetFirst(l),2)) )
				m = ATinsert(m,(ATerm) ATreplace(ATLgetFirst(l),(ATerm) ATAgetArgument(t,1),2));
			else
				m = ATinsert(m,(ATerm) ATreplace(ATLgetFirst(l),(ATerm) gsMakeSeq(ATAelementAt(ATLgetFirst(l),2),ATAgetArgument(t,1)),2));
		}
		return ATreverse(m);
	}
	if ( gsIsCond(t) )
	{
		l = get_firsts(ATAgetArgument(t,1));
		m = ATmakeList0();
		for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			ATermAppl a = gsRewriteTerm(gsMakeDataExprAnd(ATAgetArgument(t,0),ATAelementAt(ATLgetFirst(l),3)));
			if ( !ATisEqual(a,linFalse) )
			{
				m = ATinsert(m,(ATerm) ATreplace(ATLgetFirst(l),(ATerm) a,3));
			}
		}
		l = get_firsts(ATAgetArgument(t,2));
		for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			ATermAppl a = gsRewriteTerm(gsMakeDataExprAnd(gsMakeDataExprNot(ATAgetArgument(t,0)),ATAelementAt(ATLgetFirst(l),3)));
			if ( !ATisEqual(a,linFalse) )
			{
				m = ATinsert(m,(ATerm) ATreplace(ATLgetFirst(l),(ATerm) a,3));
			}
		}
		return ATreverse(m);
	}
	if ( gsIsMerge(t) )
	{
//gsPrintPart(stderr,t,false,0);fprintf(stderr,"\n\n");
		l = get_firsts(gsMakeLMerge(ATAgetArgument(t,0),ATAgetArgument(t,1)));
		m = get_firsts(gsMakeLMerge(ATAgetArgument(t,1),ATAgetArgument(t,0)));
		n = ATmakeList0();
//fprintf(stderr,"merge\n\n");
		for (;!ATisEmpty(m);m=ATgetNext(m))
		{
			if ( gsIsMerge(ATAelementAt(ATLgetFirst(m),2)) )
				n = ATinsert(n,(ATerm) ATreplace(
							ATLgetFirst(m),
							(ATerm) gsMakeMerge(
								ATAgetArgument(ATAelementAt(ATLgetFirst(m),2),1),
								ATAgetArgument(ATAelementAt(ATLgetFirst(m),2),0)
								),
							2
							)
						);
			else
				n = ATinsert(n,ATgetFirst(m));
		}
		n = ATreverse(n);
		l = ATconcat(l,n);
//		l = ATconcat(l,get_firsts(gsMakeLMerge(ATAgetArgument(t,1),ATAgetArgument(t,0))));
//fprintf(stderr,"add sync\n\n");
		l = ATconcat(l,get_firsts(gsMakeSync(ATAgetArgument(t,0),ATAgetArgument(t,1))));
//fprintf(stderr,"done merge\n\n");
		return l;
	}
	if ( gsIsLMerge(t) )
	{
		l = get_firsts(ATAgetArgument(t,0));
		m = ATmakeList0();
		for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			if ( gsIsNil(ATAelementAt(ATLgetFirst(l),2)) )
				m = ATinsert(m,(ATerm) ATreplace(ATLgetFirst(l),(ATerm) ATAgetArgument(t,1),2));
			else
				m = ATinsert(m,(ATerm) ATreplace(ATLgetFirst(l),(ATerm) gsMakeMerge(ATAelementAt(ATLgetFirst(l),2),ATAgetArgument(t,1)),2));
		}
		return ATreverse(m);
	}
	if ( gsIsChoice(t) )
	{
		l = get_firsts(ATAgetArgument(t,0));
		m = get_firsts(ATAgetArgument(t,1));
		return ATconcat(l,m);
	}

	gsWarningMsg("unknown process (%t)\n",t);
	return ATmakeList0();
}

void linearise(int i)
{
//ATfprintf(stderr,"\n\nlinearise(%i)\n\n",i);
	ATermAppl t = ATAelementAt(ATLelementAt(processes,i),2);
	ATermList l = get_firsts(t);
	ATermList m;
	ATermAppl p;

	m = ATmakeList0();
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		if ( gsIsNil(ATAelementAt(ATLgetFirst(l),2)) )
		{
			m = ATinsert(m,ATgetFirst(l));
		} else {
			p = get_proc_call(ATAelementAt(ATLgetFirst(l),2),ATLelementAt(ATLelementAt(processes,i),1));
			m = ATinsert(m,(ATerm) ATreplace(ATLgetFirst(l),(ATerm) p,2));
/*			if ( !ATisList(ATelementAt(ATLelementAt(processes,proc_id(p)),3)) )
			{
				add_to_stack(proc_id(p));
			}*/
		}
	}
	m = ATreverse(m);
	set_proc(i,m);
//ATfprintf(stderr,"\n\nset_proc(%i): %t\n\n",i,m);
/*fprintf(stderr,"\n\nset_proc(%i)\n",i);
for (; !ATisEmpty(m); m=ATgetNext(m))
{
gsPrintPart(stderr,ATelementAt(ATLgetFirst(m),3),false,0);
fprintf(stderr," -> ");
gsPrintPart(stderr,ATelementAt(ATLgetFirst(m),1),false,0);
fprintf(stderr," . ");
gsPrintPart(stderr,ATelementAt(ATLgetFirst(m),2),false,0);
fprintf(stderr,"\n\n");
}*/
}


/* Functions for converting result back to internal format */

ATermAppl gen_linproc(ATermList sums, ATermList c)
{
	ATermList l,m;
	ATermAppl r,t;
	int b;

	b = 1;
	r = gsMakeDelta();
	for (; !ATisEmpty(sums); sums=ATgetNext(sums))
	{
		if ( gsIsNil(ATAelementAt(ATLgetFirst(sums),2)) )
		{
			t = ATAelementAt(ATLgetFirst(sums),1);
		} else {
			t = gsMakeSeq(ATAelementAt(ATLgetFirst(sums),1),ATAelementAt(ATLgetFirst(sums),2));
		}
		if ( !ATisEqual(ATAelementAt(ATLgetFirst(sums),3),linTrue) )
		{
			t = gsMakeCond(ATAelementAt(ATLgetFirst(sums),3),t,gsMakeDelta());
		}
		if ( ATgetLength(ATLelementAt(ATLgetFirst(sums),0)) > 0 )
		{
			m = c;
		 	l = (ATermList) remove_indices_context(ATelementAt(ATLgetFirst(sums),0),NULL,&m);
			t = gsMakeSum(l,t);
		}

		if ( b )
		{
			r = t;
			b = 0;
		} else {
			r = gsMakeChoice(r,t);
		}
	}

	return r;
}

ATermAppl update_spec(ATermAppl spec, int init)
{
	ATermList l,m,n;
/*	ATermList o;
	int i;*/

	l = ATmakeList0();
	for (m=processes; !ATisEmpty(m); m=ATgetNext(m))
	{
		n = ATLgetFirst(m);
		if ( ATisList(ATelementAt(n,3)) )
		{
			l = ATinsert(l,(ATerm) gsMakeProcEqn(ATmakeList0(),gsMakeProcVarId(ATAelementAt(n,0),get_sorts(ATLelementAt(n,1))),ATLelementAt(n,1),gen_linproc(ATLelementAt(n,3),ATLelementAt(n,1))));
		}
	}
	l = ATreverse(l);
	spec = ATsetArgument(spec,(ATerm) gsMakeProcEqnSpec(l),5);

/*	if ( ATgetLength(ATLelementAt(ATLelementAt(processes,init),1)) > 0 )
	{
		m = ATmakeList1((ATerm) gsMakeDataExprPos_int(init));
		o = ATLgetArgument(ATAgetArgument(spec,2),0);
		for (i=1; i<ATgetLength(ATLelementAt(ATLelementAt(processes,init),1)); i++)
		{
			m = ATappend(m,(ATerm) gsMakeDataExprOfSort(ATAgetArgument(ATAelementAt(ATLelementAt(ATLelementAt(processes,init),1),i),1),ATLgetArgument(ATAgetArgument(spec,1),0),&o));
		}
		spec = ATsetArgument(spec,(ATerm) gsMakeMapSpec(o),2);
		spec = ATsetArgument(spec,(ATerm) gsMakeInit(gsMakeProcess(gsMakeProcVarId(ATAelementAt(ATLelementAt(processes,init),0),get_sorts(ATLelementAt(ATLelementAt(processes,init),1))),m)),6);
	} else {
		spec = ATsetArgument(spec,(ATerm) gsMakeInit(gsMakeProcess(gsMakeProcVarId(ATAelementAt(ATLelementAt(processes,init),0),ATmakeList0()),ATmakeList0())),6);
	}*/
	spec = ATsetArgument(spec,(ATerm) gsMakeInit(ATmakeList0(),initial_process),6);

	return spec;
}


ATermAppl subst_procs(ATermAppl a, ATermList nosubst)
{
	int i;

	if ( gsIsProcess(a) )
	{
		i = proc_id(a);
		if ( ATindexOf(nosubst,(ATerm) ATmakeInt(i),0) == -1 )
		{
			return subst_procs((ATermAppl) substitute_vars((ATerm) gen_linproc(ATLelementAt(ATLelementAt(processes,i),3),ATLelementAt(ATLelementAt(processes,i),1)),ATLelementAt(ATLelementAt(processes,i),1),ATLgetArgument(a,1)),nosubst);
		} else {
			return a;
		}
	}
	else
	if ( gsIsDelta(a) || gsIsTau(a) || gsIsAction(a) )
	{
		return a;
	}
	else
	if ( gsIsSync(a) || gsIsSeq(a) ||  gsIsMerge(a) || gsIsLMerge(a) || gsIsChoice(a) )
	{
		return ATmakeAppl2(ATgetAFun(a),(ATerm) subst_procs(ATAgetArgument(a,0),nosubst),(ATerm) subst_procs(ATAgetArgument(a,1),nosubst));
	}
	else
	if ( gsIsCond(a) )
	{
		return gsMakeCond(ATAgetArgument(a,0),subst_procs(ATAgetArgument(a,1),nosubst),subst_procs(ATAgetArgument(a,2),nosubst));
	}
	else
	if ( gsIsSum(a) )
	{
		return gsMakeSum(ATLgetArgument(a,0),subst_procs(ATAgetArgument(a,1),nosubst));
	}
	else
	if ( gsIsRestrict(a) || gsIsHide(a) || gsIsRename(a) || gsIsComm(a) ||  gsIsAllow(a) )
	{
		return ATmakeAppl2(ATgetAFun(a),ATgetArgument(a,0),(ATerm) subst_procs(ATAgetArgument(a,1),nosubst));
	}

	gsErrorMsg("invalid process parameter (%t)\n",a);
	exit(1);
}

void update_spec_subst_trace(ATermInt next, ATermList trace, ATermList *nosubst, int reuse)
{
	ATermList l,m;
	ATermInt i;

	trace = ATappend(trace,(ATerm) next);

	l = ATLelementAt(ATLelementAt(processes,ATgetInt(next)),3);
	m = ATmakeList0();
	for (;!ATisEmpty(l);l=ATgetNext(l))
	{
		if ( !gsIsNil(ATAelementAt(ATLgetFirst(l),2)) )
		{
			i = ATmakeInt(proc_id(ATAelementAt(ATLgetFirst(l),2)));
			if ( ATindexOf(m,(ATerm) i,0) == -1 )
			{
				m = ATinsert(m,(ATerm) i);
			}
		}
	}
	for (;!ATisEmpty(m);m=ATgetNext(m))
	{
		i = (ATermInt) ATgetFirst(m);
		if ( ATindexOf(trace,(ATerm) i,0) == -1 )
		{
			if ( !reuse || (ATindexOf(*nosubst,(ATerm) i,0) == -1) )
			{
				update_spec_subst_trace(i,trace,nosubst,reuse);
			}
		} else {
			if ( ATindexOf(*nosubst,(ATerm) i,0) == -1 )
			{
				*nosubst = ATappend(*nosubst,(ATerm) i);
			}
		}
	}
}
ATermAppl update_spec_subst(ATermAppl spec, int init, int reuse)
{
	ATermList nosubst;
	ATermList l,m,n;
	ATermAppl a;
	int i;

	nosubst = ATmakeList1((ATerm) ATmakeInt(init));
	i = 0;
	while ( i < ATgetLength(nosubst) )
	{
		update_spec_subst_trace((ATermInt) ATelementAt(nosubst,i),ATmakeList0(),&nosubst,reuse);
		i++;
	}

	l = ATmakeList0();
	for (m=nosubst; !ATisEmpty(m); m=ATgetNext(m))
	{
		n = ATLelementAt(processes,ATgetInt((ATermInt) ATgetFirst(m)));
		if ( ATisList(ATelementAt(n,3)) )
		{
			a = gen_linproc(ATLelementAt(n,3),ATLelementAt(n,1));
			a = subst_procs(a,nosubst);
			l = ATinsert(l,(ATerm) gsMakeProcEqn(ATmakeList0(),gsMakeProcVarId(ATAelementAt(n,0),get_sorts(ATLelementAt(n,1))),ATLelementAt(n,1),a));
		}
	}
	l = ATreverse(l);
	spec = ATsetArgument(spec,(ATerm) gsMakeProcEqnSpec(l),5);

	spec = ATsetArgument(spec,(ATerm) gsMakeInit(ATmakeList0(),initial_process),6);

	return spec;
}


/* Result manipulation functions */

int remove_unused(int init, ATbool *init_used)
{
	ATermList used = ATmakeList1((ATerm) ATmakeInt(init));
	ATermList sums;
	int p;
	int id;

	*init_used = ATfalse;
	p = 0;
	while ( p < ATgetLength(used) )
	{
		sums = ATLelementAt(ATLelementAt(processes,ATgetInt((ATermInt) ATelementAt(used,p))),3);
		for (; !ATisEmpty(sums); sums=ATgetNext(sums))
		{
			if ( !gsIsNil(ATAelementAt(ATLgetFirst(sums),2)) )
			{
				id = proc_id(ATAelementAt(ATLgetFirst(sums),2));
				if ( id == init )
				{
					*init_used = ATtrue;
				}
				if ( ATindexOf(used,(ATerm) ATmakeInt(id),0) == -1 )
				{
					used = ATappend(used,(ATerm) ATmakeInt(id));
				}
			}
		}
		p++;
	}

	for (p=ATgetLength(processes)-1; p>=0; p--)
	{
		if ( ATindexOf(used,(ATerm) ATmakeInt(p),0) == -1 )
		{
			processes = ATremoveElementAt(processes,p);
			if ( p < init )
			{
				init--;
			}
		}
	}

	return init;
}

/* int merge_to_lpo(int init, ATermList cons, ATermList *maps)
{
	ATermList args = ATmakeList1((ATerm) gsMakeDataVarId(gsString2ATermAppl("state"),gsMakeSortExprPos()));
	ATermList sums = ATmakeList0();
	ATermList l,m,n,o;
	ATermAppl u;
	int i,j;

	i = 1;
	for (l=processes; !ATisEmpty(l); l=ATgetNext(l))
	{
		for (m=ATLelementAt(ATLgetFirst(l),1); !ATisEmpty(m); m=ATgetNext(m))
		{
			if ( ATindexOf(ATgetNext(args),ATgetFirst(m),0) == -1 )
			{
				args = ATappend(args,ATgetFirst(m));
			}
		}
	}
	for (l=processes; !ATisEmpty(l); l=ATgetNext(l))
	{
		for (m=ATLelementAt(ATLgetFirst(l),3); !ATisEmpty(m); m=ATgetNext(m))
		{
			if ( gsIsNil(ATAelementAt(ATLgetFirst(m),2)) )
			{
				u = ATAelementAt(ATLgetFirst(m),2);
			} else {
				o = ATLelementAt(ATLelementAt(processes,proc_id(ATAelementAt(ATLgetFirst(m),2))),1);
				n = ATmakeList1((ATerm) gsMakeDataExprPos_int(proc_id(ATAelementAt(ATLgetFirst(m),2))));
				for (j=1; j<ATgetLength(args); j++)
				{
					if ( ATindexOf(o,ATelementAt(args,j),0) == -1 )
					{
						n = ATappend(n,(ATerm) gsMakeDataExprOfSort(ATAgetArgument(ATAelementAt(args,j),1),cons,maps));
					} else {
						n = ATappend(n,ATelementAt(ATLgetArgument(ATAelementAt(ATLgetFirst(m),2),1),ATindexOf(o,ATelementAt(args,j),0)));
					}
				}
				u = gsMakeProcess(gsMakeProcVarId(gsString2ATermAppl("X"),get_sorts(args)),n);
			}
			sums = ATappend(sums,(ATerm) ATmakeList4(
						ATelementAt(ATLgetFirst(m),0),
						ATelementAt(ATLgetFirst(m),1),
						(ATerm) u,
						(ATerm) gsMakeDataExprAndWithTrueCheck(gsMakeDataExprEq(gsMakeDataVarId(gsString2ATermAppl("state"),gsMakeSortExprPos()),gsMakeDataExprPos_int(i)),ATAelementAt(ATLgetFirst(m),3))
						));
		}
		i++;
	}

	processes = ATmakeList1((ATerm) ATmakeList4((ATerm) gsString2ATermAppl("X"),(ATerm) args,(ATerm) gsMakeNil(),(ATerm) sums));

	return init;
}*/

ATermAppl make_lpe(ATermAppl Spec, int init_id)
{
//	ATermList cons = ATLgetArgument(ATAgetArgument(Spec,1),0);
//	ATermList maps = ATLgetArgument(ATAgetArgument(Spec,2),0);
	ATermList args;
	ATermList sums;
	ATermList l,m,n,o,s1,s3,s5,vars;
	ATermAppl s2;
	int i,j,singleton;

	singleton = (ATgetLength(processes) <= 1);
	if ( singleton )
	{
		args = ATmakeList0();
	} else {
		args = ATmakeList1((ATerm) gsMakeDataVarId(gsString2ATermAppl("state"),gsMakeSortExprPos()));
	}
	sums = ATmakeList0();
	vars = ATmakeList0();

	for (l=processes; !ATisEmpty(l); l=ATgetNext(l))
	{
		for (m=ATLelementAt(ATLgetFirst(l),1); !ATisEmpty(m); m=ATgetNext(m))
		{
			if ( singleton )
			{
				if ( ATindexOf(args,ATgetFirst(m),0) == -1 )
				{
					args = ATappend(args,ATgetFirst(m));
				}
			} else {
				if ( ATindexOf(ATgetNext(args),ATgetFirst(m),0) == -1 )
				{
					args = ATappend(args,ATgetFirst(m));
				}
			}
		}
	}

	i = 1;
	for (l=processes; !ATisEmpty(l); l=ATgetNext(l))
	{
		for (m=ATLelementAt(ATLgetFirst(l),3); !ATisEmpty(m); m=ATgetNext(m))
		{
			if ( gsIsNil(ATAelementAt(ATLgetFirst(m),2)) )
			{
				gsErrorMsg("cannot create LPE of terminating processes\n");
				exit(1);
			} else {
				o = ATLelementAt(ATLelementAt(processes,proc_id(ATAelementAt(ATLgetFirst(m),2))),1);
				if ( singleton )
				{
					n = ATmakeList0();
				} else {
					n = ATmakeList1((ATerm) gsMakeAssignment(ATAelementAt(args,0),gsMakeDataExprPos_int(proc_id(ATAelementAt(ATLgetFirst(m),2))+1)));
				}
				for (j=(singleton?0:1); j<ATgetLength(args); j++)
				{
					if ( ATindexOf(o,ATelementAt(args,j),0) == -1 )
					{
						//n = ATappend(n,(ATerm) gsMakeAssignment(ATAelementAt(args,j),gsMakeNil()));
						s3 = ATconcat(args,vars);
						vars = ATappend(vars,(ATerm) get_new_var(ATAelementAt(args,j),&s3));
						n = ATappend(n,(ATerm) gsMakeAssignment(ATAelementAt(args,j),(ATermAppl) ATgetLast(vars)));
					} else {
						if ( !ATisEqual(ATelementAt(args,j),ATAelementAt(ATLgetArgument(ATAelementAt(ATLgetFirst(m),2),1),ATindexOf(o,ATelementAt(args,j),0))) )
						{
							n = ATappend(n,(ATerm) gsMakeAssignment(ATAelementAt(args,j),ATAelementAt(ATLgetArgument(ATAelementAt(ATLgetFirst(m),2),1),ATindexOf(o,ATelementAt(args,j),0))));
						}
					}
				}
			}
			s1 = ATLelementAt(ATLgetFirst(m),0);
			if ( singleton )
			{
				s2 = ATAelementAt(ATLgetFirst(m),3);
			} else {
				s2 = gsMakeDataExprAndWithTrueCheck(gsMakeDataExprEq(gsMakeDataVarId(gsString2ATermAppl("state"),gsMakeSortExprPos()),gsMakeDataExprPos_int(i)),ATAelementAt(ATLgetFirst(m),3));
			}
			s3 = synch2list(ATAelementAt(ATLgetFirst(m),1));
			s5 = n;
			n = ATconcat(args,vars);
			remove_indices_contextA((ATerm) s1,&s2,&n);
			n = ATconcat(args,vars);
			remove_indices_contextL((ATerm) s1,&s3,&n);
			n = ATconcat(args,vars);
			s1 = (ATermList) remove_indices_contextL((ATerm) s1,&s5,&n);
			sums = ATappend(sums,(ATerm) gsMakeLPESummand(
						s1,
						s2,
						gsMakeMultAct(s3),
						gsMakeNil(),
						s5
						));
		}
		i++;
	}
	
	Spec = ATsetArgument(Spec,(ATerm) gsMakeLPE(vars,args,sums),5);

	vars = ATmakeList0();
	m = ATLelementAt(ATLelementAt(processes,proc_id(initial_process)),1);
	n = ATLgetArgument(initial_process,1);
	if ( singleton )
	{
		l = ATmakeList0();
	} else {
		l = ATmakeList1((ATerm) gsMakeAssignment(ATAgetFirst(args),gsMakeDataExprPos_int(init_id+1)));
	}
	for (j=(singleton?0:1); j<ATgetLength(args); j++)
	{
		if ( ATindexOf(m,ATelementAt(args,j),0) == -1 )
			l = ATappend(l,(ATerm) gsMakeAssignment(ATAelementAt(args,j),get_new_var(ATAelementAt(args,j),&vars)));
		else
			l = ATappend(l,(ATerm) gsMakeAssignment(ATAelementAt(args,j),ATAelementAt(n,ATindexOf(m,ATelementAt(args,j),0))));
	}
	
//	Spec = ATsetArgument(Spec,(ATerm) gsMakeMapSpec(maps),2);
	Spec = ATsetArgument(Spec,(ATerm) gsMakeLPEInit(vars,l),6);

	return Spec;
}

ATermAppl cluster_lpe(ATermAppl spec)
{
	gsWarningMsg("clustering not yet implemented\n");
	return spec;
/*	ATermList sums = ATLgetArgument(ATAgetArgument(spec,4),1);
	ATermList cluster, newsums;
	ATermAppl sumvars,;

	newsums = ATmakeList0();
	while ( !ATisEmpty(sums) )
	{
		cluster = ATmakeList0();

		newsums = ;
	}

	return ATsetArgument(spec,(ATerm) gsMakeLPE(ATLgetArgument(ATAgetArgument(spec,4),0),newsums),4); */
}


/* Interface */

int main_linearisation(ATermAppl Spec)
{
	ATermAppl init;
	ATermList l,m;
	ATbool init_used;
	int i, init_id;

	gsRewriteInit(ATAgetArgument(Spec,3),GS_REWR_INNER3);

	linTrue = gsMakeDataExprTrue();
	ATprotectAppl(&linTrue);
	linFalse = gsMakeDataExprFalse();
	ATprotectAppl(&linFalse);

	ATprotectList(&processes);
	ATprotectAppl(&initial_process);
	ATprotectList(&todo_stack);
	ATprotect(&debruijn);

	processes = ATmakeList0();
	todo_stack = ATmakeList0();
	debruijn = (ATerm) gsString2ATermAppl("debruijn");

	l = ATLgetArgument(ATAgetArgument(Spec,5),0);
	for (; !ATisEmpty(l); l = ATgetNext(l))
	{
		add_process_eqn(ATAgetFirst(l));
	}
//	init = ATAgetArgument(ATAgetArgument(Spec,6),0);
	init = ATAgetArgument(ATAgetArgument(Spec,6),1);
	init_id = add_init(init);
	add_to_stack(init_id);

	while ( !empty_stack() )
	{
		i = pop_stack();
		if ( !is_done(i) )
		{
			linearise(i);
		}
	}

	init_id = remove_unused(init_id, &init_used);

	if ( !init_used )
	{
		i = 0;
		for (l=processes; !ATisEmpty(l); l=ATgetNext(l))
		{
			if ( match(init,ATLgetFirst(l),&m) )
			{
				init_id = i;
				initial_process = gsMakeProcess(gsMakeProcVarId(ATAelementAt(ATLgetFirst(l),0),get_sorts(ATLelementAt(ATLgetFirst(l),1))),m);
			}
			i++;
		}
		init_id = remove_unused(init_id, &init_used);
	}

	return init_id;
}

ATermAppl gsLinearise2_nolpe(ATermAppl Spec)
{
//	ATermList lin;
	int init_id;

	generalise = true;
	init_id = main_linearisation(Spec);
//	Spec = ATAelementAt(lin,0);
//	init_id = ATgetInt((ATermInt) ATelementAt(lin,1));

	return update_spec(Spec,init_id);
}

ATermAppl gsLinearise2_nolpe_subst(ATermAppl Spec, int reuse_cycles)
{
	int init_id;

	generalise = true;
	init_id = main_linearisation(Spec);

	return update_spec_subst(Spec,init_id,reuse_cycles);
}

ATermAppl gsLinearise2_statespace(ATermAppl Spec, bool lpe)
{
	int init_id;

	generalise = false;
	init_id = main_linearisation(Spec);

	if ( lpe )
	{
		Spec = make_lpe(Spec,init_id);
	} else {
		Spec = update_spec(Spec,init_id);
	}

	return Spec;
}

ATermAppl gsLinearise2(ATermAppl Spec, int cluster)
{
//	ATermList lin;
//	ATermList l;	
	int init_id;

	generalise = true;
	init_id = main_linearisation(Spec);

//	Spec = ATAelementAt(lin,0);
//	init_id = ATgetInt((ATermInt) ATelementAt(lin,1));

/*	l = ATLgetArgument(ATAgetArgument(Spec,2),0);
	init_id = merge_to_lpo(init_id,ATLgetArgument(ATAgetArgument(Spec,1),0),&l);
	Spec = ATsetArgument(Spec,(ATerm) gsMakeMapSpec(l),2);*/

	Spec = make_lpe(Spec,init_id);
	if ( cluster )
	{
		Spec = cluster_lpe(Spec);
	}

	return Spec;
}

#ifdef __cplusplus
}
#endif
