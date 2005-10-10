#ifdef __cplusplus
extern "C" {
#endif

#define NAME "rewr_inner"

#include <stdio.h>
#include <stdlib.h>
#include <aterm2.h>
#include "gslowlevel.h"
#include "gsfunc.h"
#include "gsrewr_inner.h"
#include "libprint_c.h"
#include "gssubstitute.h"
#include <assert.h>

extern ATermList opid_eqns;
extern ATermList dataappl_eqns;

static ATermTable term2int;
static unsigned int num_opids;
static ATermAppl *int2term;
static ATermAppl *inner_eqns;
static ATermInt trueint;
static AFun nilAFun;
static AFun opidAFun;
static AFun ruleAFun;
static int max_vars;
static bool is_initialised = false;

static AFun afunS, afunM, afunF, afunN, afunD, afunR, afunCR, afunC, afunX;
static ATerm dummy;

#define isS(x) ATisEqualAFun(ATgetAFun(x),afunS)
#define isM(x) ATisEqualAFun(ATgetAFun(x),afunM)
#define isF(x) ATisEqualAFun(ATgetAFun(x),afunF)
#define isN(x) ATisEqualAFun(ATgetAFun(x),afunN)
#define isD(x) ATisEqualAFun(ATgetAFun(x),afunD)
#define isR(x) ATisEqualAFun(ATgetAFun(x),afunR)
#define isCR(x) ATisEqualAFun(ATgetAFun(x),afunCR)
#define isC(x) ATisEqualAFun(ATgetAFun(x),afunC)
#define isX(x) ATisEqualAFun(ATgetAFun(x),afunX)

static ATermAppl fromInner(ATerm Term);
static ATerm build(ATerm Term, int buildargs, ATermAppl *vars, ATerm *vals, int len);
static ATerm rewrite_func(ATermInt op, ATermList args);
static ATerm rewrite(ATerm Term);


//static ATermTable subst_table = NULL;
//static bool subst_is_inner = false;

#define ATAgetArgument(x,y) ((ATermAppl) ATgetArgument(x,y))
#define ATLgetArgument(x,y) ((ATermList) ATgetArgument(x,y))
#define ATAgetFirst(x) ((ATermAppl) ATgetFirst(x))
#define ATLgetFirst(x) ((ATermList) ATgetFirst(x))
#define gsIsOpId(x) (ATgetAFun(x) == opidAFun)

#define ATisList(x) (ATgetType(x) == AT_LIST)
#define ATisAppl(x) (ATgetType(x) == AT_APPL)
#define ATisInt(x) (ATgetType(x) == AT_INT)

/*static bool ATisList(ATerm a)
{
	return (ATgetType(a) == AT_LIST);
}

static bool ATisInt(ATerm a)
{
	return (ATgetType(a) == AT_INT);
}*/

#define is_nil(x) (ATisList(x)?false:(ATgetAFun((ATermAppl) x) == nilAFun))

/*static bool is_nil(ATerm t)
{
	if ( ATisList(t) )
	{
		return false;
	} else {
		return gsIsNil((ATermAppl) t);
	}
}*/


static int write_tree(FILE *f, ATermAppl tree, int *num_states)
{
	int n,m;

	if ( isS(tree) )
	{
		n = write_tree(f,ATAgetArgument(tree,1),num_states);
		fprintf(f,"n%i [label=\"S(%s)\"]\n",*num_states,ATgetName(ATgetAFun(ATAgetArgument(ATAgetArgument(tree,0),0))));
		fprintf(f,"n%i -> n%i\n",*num_states,n);
		return (*num_states)++;
	} else if ( isM(tree) )
	{
		n = write_tree(f,ATAgetArgument(tree,1),num_states);
		m = write_tree(f,ATAgetArgument(tree,2),num_states);
		if ( ATisInt(ATgetArgument(tree,0)) )
		{
			fprintf(f,"n%i [label=\"M(%i)\"]\n",*num_states,ATgetInt((ATermInt) ATgetArgument(tree,0)));
		} else {
			fprintf(f,"n%i [label=\"M(%s)\"]\n",*num_states,ATgetName(ATgetAFun(ATAgetArgument(ATAgetArgument(tree,0),0))));
		}
		fprintf(f,"n%i -> n%i [label=\"true\"]\n",*num_states,n);
		fprintf(f,"n%i -> n%i [label=\"false\"]\n",*num_states,m);
		return (*num_states)++;
	} else if ( isF(tree) )
	{
		n = write_tree(f,ATAgetArgument(tree,1),num_states);
		m = write_tree(f,ATAgetArgument(tree,2),num_states);
		if ( ATisInt(ATgetArgument(tree,0)) )
		{
	 		fprintf(f,"n%i [label=\"F(%s)\"]\n",*num_states,ATgetName(ATgetAFun(ATAgetArgument(int2term[ATgetInt((ATermInt) ATgetArgument(tree,0))],0))));
		} else {
	 		fprintf(f,"n%i [label=\"F(%s)\"]\n",*num_states,ATgetName(ATgetAFun(ATAgetArgument(ATAgetArgument(tree,0),0))));
		}
		fprintf(f,"n%i -> n%i [label=\"true\"]\n",*num_states,n);
		fprintf(f,"n%i -> n%i [label=\"false\"]\n",*num_states,m);
		return (*num_states)++;
	} else if ( isD(tree) )
	{
		n = write_tree(f,ATAgetArgument(tree,0),num_states);
		fprintf(f,"n%i [label=\"D\"]\n",*num_states);
		fprintf(f,"n%i -> n%i\n",*num_states,n);
		return (*num_states)++;
	} else if ( isN(tree) )
	{
		n = write_tree(f,ATAgetArgument(tree,0),num_states);
		fprintf(f,"n%i [label=\"N\"]\n",*num_states);
		fprintf(f,"n%i -> n%i\n",*num_states,n);
		return (*num_states)++;
	} else if ( isC(tree) )
	{
		n = write_tree(f,ATAgetArgument(tree,1),num_states);
		m = write_tree(f,ATAgetArgument(tree,2),num_states);
		gsfprintf(f,"n%i [label=\"C(%P)\"]\n",*num_states,fromInner(ATgetArgument(tree,0)));
		fprintf(f,"n%i -> n%i [label=\"true\"]\n",*num_states,n);
		fprintf(f,"n%i -> n%i [label=\"false\"]\n",*num_states,m);
		return (*num_states)++;
	} else if ( isR(tree) )
	{
		gsfprintf(f,"n%i [label=\"R(%P)\"]\n",*num_states,fromInner(ATgetArgument(tree,0)));
		return (*num_states)++;
	} else if ( isCR(tree) )
	{
		gsfprintf(f,"n%i [label=\"CR(%P,%P)\"]\n",*num_states,fromInner(ATgetArgument(tree,0)),fromInner(ATgetArgument(tree,1)));
		return (*num_states)++;
	} else if ( isX(tree) )
	{
		ATfprintf(f,"n%i [label=\"X\"]\n",*num_states);
		return (*num_states)++;
	}

	return -1;
}

static void tree2dot(ATermAppl tree, char *name, char *filename)
{
	FILE *f;
	int num_states = 0;

	if ( (f = fopen(filename,"w")) == NULL )
	{
		perror("fopen");
		return;
	}

	fprintf(f,"digraph \"%s\" {\n",name);
	write_tree(f,tree,&num_states);
	fprintf(f,"}\n");

	fclose(f);
}

static void term2seq(ATerm t, ATermList *s)
{
	if ( ATisList(t) )
	{
		ATermList l;

		l = ATgetNext((ATermList) t);
		t = ATgetFirst((ATermList) t);

		*s = ATinsert(*s, (ATerm) ATmakeAppl3(afunF,(ATerm) t,dummy,dummy));

		for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			term2seq(ATgetFirst(l),s);
			if ( !ATisEmpty(ATgetNext(l)) )
			{
				*s = ATinsert(*s, (ATerm) ATmakeAppl1(afunN,dummy));
			}
		}
		*s = ATinsert(*s, (ATerm) ATmakeAppl1(afunD,dummy));
	} else if ( ATisInt(t) )
	{
		term2seq((ATerm) ATmakeList1(t),s);
	} else if ( gsIsDataVarId((ATermAppl) t) )
	{
		ATerm store = (ATerm) ATmakeAppl2(afunS,(ATerm) t,dummy);

		if ( ATindexOf(*s,store,0) >= 0 )
		{
			*s = ATinsert(*s, (ATerm) ATmakeAppl3(afunM,(ATerm) t,dummy,dummy));
		} else {
			*s = ATinsert(*s, store);
		}
	} else {
		assert(0);
	}

}

static ATermList create_sequence(ATermList rule)
{
	ATermAppl pat = (ATermAppl) ATgetArgument(rule,2);
	ATermList pars = ATmakeList0();
	ATermList rseq = ATmakeList0();

	pars = (ATermList) pat;
//	ATprintf("pattern pars: %t\n",pars);
	for (; !ATisEmpty(pars); pars=ATgetNext(pars))
	{
		term2seq(ATgetFirst(pars),&rseq);
		//if ( !ATisEmpty(ATgetNext(pars)) )
		//{
			rseq = ATinsert(rseq, (ATerm) ATmakeAppl1(afunN,dummy));
		//}
	}
//	ATprintf("rseq: %t\n",rseq);
	if ( ATisAppl(ATgetArgument(rule,1)) )
		rseq = ATinsert(rseq,(ATerm) ATmakeAppl1(afunR,ATgetArgument(rule,3)));
	else
		rseq = ATinsert(rseq,(ATerm) ATmakeAppl2(afunCR,ATgetArgument(rule,1),ATgetArgument(rule,3)));

	return ATreverse(rseq);
}

static ATermList add_to_stack(ATermList stack,ATermList seqs, ATermAppl *r, ATermList *rc)
{
	if ( ATisEmpty(stack) )
	{
		return stack;
	}

	ATermList l = ATmakeList0();
	ATermList h = ATLgetFirst(stack);

	for (; !ATisEmpty(seqs); seqs=ATgetNext(seqs))
	{
		ATermList e = ATLgetFirst(seqs);

		if ( isD(ATAgetFirst(e)) )
		{
			l = ATinsert(l,(ATerm) ATgetNext(e));
		} else if ( isN(ATAgetFirst(e)) )
		{
			h = ATinsert(h,(ATerm) ATgetNext(e));
		} else if ( isR(ATAgetFirst(e)) )
		{
			*r = ATAgetFirst(e);
		} else {
			*rc = ATinsert(*rc,ATgetFirst(e));
		}
	}
	
	return ATinsert(add_to_stack(ATgetNext(stack),l,r,rc),(ATerm) h);
}

static char tree_var_str[20];
static ATermAppl createFreshVar(ATermAppl sort,int *i)
{
	sprintf(tree_var_str,"var-%i",(*i)++);
	return gsMakeDataVarId(gsString2ATermAppl(tree_var_str),sort);
}

static ATermList subst_var(ATermList l, ATermAppl old, ATermAppl new, ATermList substs)
{
	if ( ATisEmpty(l) )
	{
		return l;
	}

	ATermAppl head = (ATermAppl) ATgetFirst(l);
	l = ATgetNext(l);

	if ( isM(head) )
	{
		if ( ATisEqual(ATgetArgument(head,0),old) )
		{
			head = ATmakeAppl3(afunM,(ATerm) new,dummy,dummy);
		}
	} else if ( isCR(head) )
	{
		head = ATmakeAppl2(afunCR,gsSubstValues(substs,ATgetArgument(head,0),true),gsSubstValues(substs,ATgetArgument(head,1),true));
	} else if ( isR(head) )
	{
		head = ATmakeAppl1(afunR,gsSubstValues(substs,ATgetArgument(head,0),true));
	}

	return ATinsert(subst_var(l,old,new,substs),(ATerm) head);
}

//#define print_return(x,y) ATermAppl a = y; ATprintf(x "return %t\n\n",a); return a;
#define print_return(x,y) return y;
static int max_tree_vars;
static ATermAppl build_tree(ATermList Flist, ATermList Slist, ATermList Mlist, ATermList stack, ATermList upstack, int i)
{
//ATprintf("build_tree(  %t  ,  %t  ,  %t  ,  %t  ,  %t  ,  %i  )\n\n",Flist,Slist,Mlist,stack,upstack,i);

	if ( !ATisEmpty(Slist) )
	{
		ATermList l,m;

		ATermAppl v = createFreshVar(ATAgetArgument(ATAgetArgument(ATAgetFirst(ATLgetFirst(Slist)),0),1),&i); // XXX get sort from Slist
		if ( i > max_tree_vars )
		{
			max_tree_vars = i;
		}

		l = ATmakeList0();
		m = ATmakeList0();
		for (; !ATisEmpty(Slist); Slist=ATgetNext(Slist))
		{
			ATermList e = ATLgetFirst(Slist);

			e = subst_var(e,ATAgetArgument(ATAgetFirst(e),0),v,ATmakeList1((ATerm) gsMakeSubst(ATgetArgument(ATAgetFirst(e),0),(ATerm) v)));
//			e = gsSubstValues_List(ATmakeList1((ATerm) gsMakeSubst(ATgetArgument(ATAgetFirst(e),0),(ATerm) v)),e,true);

			l = ATinsert(l,ATgetFirst(e));
			m = ATinsert(m,(ATerm) ATgetNext(e));
		}
		
		ATermAppl r = NULL;
		ATermList readies = ATmakeList0();

		stack = add_to_stack(stack,m,&r,&readies);

		if ( r == NULL )
		{
			ATermAppl tree;
			
			tree = build_tree(Flist,ATmakeList0(),Mlist,stack,upstack,i);
			for (; !ATisEmpty(readies); readies=ATgetNext(readies))
			{
				tree = ATmakeAppl3(afunC,ATgetArgument(ATAgetFirst(readies),0),(ATerm) ATmakeAppl1(afunR,ATgetArgument(ATAgetFirst(readies),1)),(ATerm) tree);
			}
			print_return("",ATmakeAppl2(afunS,(ATerm) v,(ATerm) tree));
		} else {
			print_return("",ATmakeAppl2(afunS,(ATerm) v,(ATerm) r));
		}
	} else if ( !ATisEmpty(Mlist) )
	{
		ATerm M = ATgetFirst(ATLgetFirst(Mlist));

		ATermList l = ATmakeList0();
		ATermList m = ATmakeList0();
		for (; !ATisEmpty(Mlist); Mlist=ATgetNext(Mlist))
		{
			if ( ATisEqual(M,ATgetFirst(ATLgetFirst(Mlist))) )
			{
				l = ATinsert(l,(ATerm) ATgetNext(ATLgetFirst(Mlist)));
			} else {
				m = ATinsert(m,ATgetFirst(Mlist));
			}
		}
		Mlist = m;

		ATermAppl true_tree,false_tree;
		ATermAppl r = NULL;
		ATermList readies = ATmakeList0();

		ATermList newstack = add_to_stack(stack,l,&r,&readies);

		false_tree = build_tree(Flist,Slist,Mlist,stack,upstack,i);

		if  ( r == NULL )
		{
			true_tree = build_tree(Flist,Slist,Mlist,newstack,upstack,i);
			for (; !ATisEmpty(readies); readies=ATgetNext(readies))
			{
				true_tree = ATmakeAppl3(afunC,ATgetArgument(ATAgetFirst(readies),0),(ATerm) ATmakeAppl1(afunR,ATgetArgument(ATAgetFirst(readies),1)),(ATerm) true_tree);
			}
		} else {
			true_tree = r;
		}

		if ( ATisEqual(true_tree,false_tree) )
		{
			print_return("",true_tree);
		} else {
			print_return("",ATmakeAppl3(afunM,ATgetArgument((ATermAppl) M,0),(ATerm) true_tree,(ATerm) false_tree));
		}
	} else if ( !ATisEmpty(Flist) ) {
		ATermList F = ATLgetFirst(Flist);
		ATermAppl true_tree,false_tree;

		ATermList newupstack = upstack;
		ATermList l = ATmakeList0();

		for (; !ATisEmpty(Flist); Flist=ATgetNext(Flist))
		{
			if ( ATisEqual(ATgetFirst(ATLgetFirst(Flist)),ATgetFirst(F)) )
			{
				newupstack = ATinsert(newupstack, (ATerm) ATgetNext(ATLgetFirst(Flist)));
			} else {
				l = ATinsert(l,ATgetFirst(Flist));
			}
		}
		
		false_tree = build_tree(l,Slist,Mlist,stack,upstack,i);
		true_tree = build_tree(ATmakeList0(),Slist,Mlist,stack,newupstack,i);

		if ( isCR(ATAgetFirst(ATgetNext(F))) )
		{
			true_tree = ATmakeAppl3(afunC,ATgetArgument(ATAgetFirst(ATgetNext(F)),0),(ATerm) ATmakeAppl1(afunR,ATgetArgument(ATAgetFirst(ATgetNext(F)),1)),(ATerm) true_tree);
		}

		if ( ATisEqual(true_tree,false_tree) )
		{
			print_return("",true_tree);
		} else {
			print_return("",ATmakeAppl3(afunF,ATgetArgument(ATAgetFirst(F),0),(ATerm) true_tree,(ATerm) false_tree));
		}
	} else if ( !ATisEmpty(upstack) ) {
		ATermList l;
		
		l = ATmakeList0();
		for (; !ATisEmpty(upstack); upstack=ATgetNext(upstack))
		{
			ATermList e = ATLgetFirst(upstack);

			if ( isF(ATAgetFirst(e)) )
			{
				Flist = ATinsert(Flist,(ATerm) e);
			} else if ( isS(ATAgetFirst(e)) )
			{
				Slist = ATinsert(Slist,(ATerm) e);
			} else if ( isM(ATAgetFirst(e)) )
			{
				Mlist = ATinsert(Mlist,(ATerm) e);
			} else {
				l = ATinsert(l,(ATerm) e);
			}
		}
		
		ATermAppl r = NULL;
		ATermList readies = ATmakeList0();

		stack = add_to_stack(ATinsert(stack,(ATerm) ATmakeList0()),l,&r,&readies);

		if ( r == NULL )
		{
			ATermAppl t = build_tree(Flist,Slist,Mlist,stack,ATmakeList0(),i);

			for (; !ATisEmpty(readies); readies=ATgetNext(readies))
			{
				t = ATmakeAppl3(afunC,ATgetArgument(ATAgetFirst(readies),0),(ATerm) ATmakeAppl1(afunR,ATgetArgument(ATAgetFirst(readies),1)),(ATerm) t);
			}

			print_return("",t);
		} else {
			print_return("",r);
		}
	} else {
		if ( ATisEmpty(ATLgetFirst(stack)) )
		{
			if ( ATisEmpty(ATgetNext(stack)) )
			{
				print_return("",ATmakeAppl0(afunX));
				//print_return("",ATmakeAppl1(afunRC,(ATerm) ATmakeList0()));
			} else {
				print_return("",ATmakeAppl1(afunD,(ATerm) build_tree(Flist,Slist,Mlist,ATgetNext(stack),upstack,i)));
			}
		} else {
			ATermList l;
		
			l = ATLgetFirst(stack);
			stack = ATinsert(ATgetNext(stack),(ATerm) ATmakeList0());

			ATermAppl r = NULL;
			ATermList readies = ATmakeList0();
	
			for (; !ATisEmpty(l); l=ATgetNext(l))
			{
				ATermList e = ATLgetFirst(l);
	
				/*if ( isR(ATAgetFirst(e)) )
				{
					print_return("",ATmakeAppl1(afunR,(ATerm) ATgetArgument(ATAgetFirst(e),0)));
				} else if ( isRC(ATAgetFirst(e)) )
				{
					ATermList m = ATmakeList0();

					for (; !ATisEmpty(l); l=ATgetNext(l))
					{
						if ( isR(ATAgetFirst(ATLgetFirst(l))) )
						{
							print_return("",ATmakeAppl1(afunR,(ATerm) ATgetArgument(ATAgetFirst(ATLgetFirst(l)),0)));
						}
						m = ATinsert(m,ATgetFirst(ATLgetArgument(ATAgetFirst(ATLgetFirst(l)),0)));
					}
					
					print_return("",ATmakeAppl1(afunRC,(ATerm) m));
				} else */if ( isF(ATAgetFirst(e)) )
				{
					Flist = ATinsert(Flist,(ATerm) e);
				} else if ( isS(ATAgetFirst(e)) )
				{
					Slist = ATinsert(Slist,(ATerm) e);
				} else if ( isM(ATAgetFirst(e)) )
				{
					Mlist = ATinsert(Mlist,(ATerm) e);
				} else if ( isR(ATAgetFirst(e)) )
				{
					r = ATAgetFirst(e);
					break;
				} else {
					readies = ATinsert(readies, ATgetFirst(e));
				}
			}

			ATermAppl tree;
			if ( r == NULL )
			{
				tree = build_tree(Flist,Slist,Mlist,stack,upstack,i);
				for (; !ATisEmpty(readies); readies=ATgetNext(readies))
				{
					tree = ATmakeAppl3(afunC,ATgetArgument(ATAgetFirst(readies),0),(ATerm) ATmakeAppl1(afunR,ATgetArgument(ATAgetFirst(readies),1)),(ATerm) tree);
				}
			} else {
				tree = r;
			}
	
			print_return("",ATmakeAppl1(afunN,(ATerm) tree));
		}
	}
}

static ATermAppl optimise_tree_aux(ATermAppl tree, ATermList stored, int len)
{
	if ( isS(tree) )
	{
		return ATmakeAppl2(afunS,ATgetArgument(tree,0),(ATerm) optimise_tree_aux(ATAgetArgument(tree,1),ATinsert(stored,ATgetArgument(tree,0)),len+1));
	} else if ( isM(tree) )
	{
		return ATmakeAppl3(afunM,(ATerm) ATmakeInt(len-ATindexOf(stored,ATgetArgument(tree,0),0)),(ATerm) optimise_tree_aux(ATAgetArgument(tree,1),stored,len),(ATerm) optimise_tree_aux(ATAgetArgument(tree,2),stored,len));
	} else if ( isF(tree) )
	{
		return ATmakeAppl3(afunF,ATgetArgument(tree,0),(ATerm) optimise_tree_aux(ATAgetArgument(tree,1),stored,len),(ATerm) optimise_tree_aux(ATAgetArgument(tree,2),stored,len));
	} else if ( isN(tree) )
	{
		return ATmakeAppl1(afunN,(ATerm) optimise_tree_aux(ATAgetArgument(tree,0),stored,len));
	} else if ( isD(tree) )
	{
		return ATmakeAppl1(afunD,(ATerm) optimise_tree_aux(ATAgetArgument(tree,0),stored,len));
	} else if ( isC(tree) )
	{
		return ATmakeAppl3(afunC,ATgetArgument(tree,0),(ATerm) optimise_tree_aux(ATAgetArgument(tree,1),stored,len),(ATerm) optimise_tree_aux(ATAgetArgument(tree,2),stored,len));
	} else if ( isR(tree) )
	{
		return tree;
	} else /* isX(tree) */ {
		assert(isX(tree));
		return tree;
	}
}

static ATermAppl optimise_tree(ATermAppl tree)
{
	return optimise_tree_aux(tree,ATmakeList0(),-1);
}

static ATermAppl create_tree(ATermList rules, int opid, int *max_vars)
{
	ATermList rule_seqs;

	rule_seqs = ATmakeList0();
	for (; !ATisEmpty(rules); rules=ATgetNext(rules))
	{
		rule_seqs = ATinsert(rule_seqs, (ATerm) create_sequence((ATermList) ATgetFirst(rules)));
	}

	ATermAppl r = NULL;
	ATermList Flist, Slist, Mlist, readies;
	Flist = ATmakeList0();
	Slist = ATmakeList0();
	Mlist = ATmakeList0();
	readies = ATmakeList0();

	for (; !ATisEmpty(rule_seqs); rule_seqs=ATgetNext(rule_seqs))
	{
		ATermList rule = ATLgetFirst(rule_seqs);

		if ( isF(ATAgetFirst(rule)) )
		{
			Flist = ATinsert(Flist,(ATerm) rule);
		} else if ( isS(ATAgetFirst(rule)) )
		{
			Slist = ATinsert(Slist,(ATerm) rule);
		} else if ( isM(ATAgetFirst(rule)) )
		{
			Mlist = ATinsert(Mlist,(ATerm) rule);
		} else if ( isR(ATAgetFirst(rule)) )
		{
			r = ATAgetFirst(rule);
		} else if ( isCR(ATAgetFirst(rule)) )
		{
			readies = ATinsert(readies,ATgetFirst(rule));
		}
	}

	ATermAppl tree;
	if ( r == NULL )
	{
		max_tree_vars = *max_vars;
		tree = build_tree(Flist,Slist,Mlist,ATmakeList1((ATerm) ATmakeList0()),ATmakeList0(),0);
		*max_vars = max_tree_vars;
		for (; !ATisEmpty(readies); readies=ATgetNext(readies))
		{
			tree = ATmakeAppl3(afunC,ATgetArgument(ATAgetFirst(readies),0),(ATerm) ATmakeAppl1(afunR,ATgetArgument(ATAgetFirst(readies),1)),(ATerm) tree);
		}
	} else {
		tree = r;
	}
	//ATprintf("tree: %t\n",tree);
	
	tree = optimise_tree(tree);

	/*char s[100],t[100];
	sprintf(s,"tree_%i_%s",opid,ATgetName(ATgetAFun(ATAgetArgument(int2term[opid],0))));
	sprintf(t,"tree_%i_%s.dot",opid,ATgetName(ATgetAFun(ATAgetArgument(int2term[opid],0))));
	tree2dot(tree,s,t);*/

	// XXX remove sequences of Ss!

	return tree;
}

//#define TMA_DEBUG
static ATermList tree_matcher_aux(ATerm t, ATermAppl *tree, ATermAppl *vars, ATerm *vals, int *len)
{
	ATermList args;

	args = ATmakeList0();
	if ( ATisList(t) )
	{
		args = ATgetNext((ATermList) t);
	}

#ifdef TMA_DEBUG
ATprintf("aux: %t %t\n",t,args);
#endif
	while ( !ATisEmpty(args) || isD(*tree) || isC(*tree) )
	{
		if ( isS(*tree) )
		{
#ifdef TMA_DEBUG
ATprintf("S %t\n",ATgetArgument(*tree,0));
#endif
			vars[*len] = (ATermAppl) ATgetArgument(*tree,0);
			vals[*len] = ATgetFirst(args);
			(*len)++;
			*tree = ATAgetArgument(*tree,1);
		} else if ( isM(*tree) )
		{
#ifdef TMA_DEBUG
ATprintf("M %t\n",ATgetArgument(*tree,0));
#endif
/*			bool b = true;
			for (int i=0; i<*len; i++)
			{
				if ( ATisEqual(vars[i],ATgetArgument(*tree,0)) )
				{
					if ( ATisEqual(ATgetFirst(args),vals[i]) )
					{
#ifdef TMA_DEBUG
ATprintf("true\n");
#endif
						*tree = ATAgetArgument(*tree,1);
						b = false;
					}
					break;
				}

			}
			if ( b )
			{
#ifdef TMA_DEBUG
ATprintf("false\n");
#endif
				*tree = ATAgetArgument(*tree,2);
			}*/
			if ( ATisEqual(ATgetFirst(args),vals[ATgetInt((ATermInt) ATgetArgument(*tree,0))]) )
			{
				*tree = ATAgetArgument(*tree,1);
			} else {
				*tree = ATAgetArgument(*tree,2);
			}
		} else if ( isF(*tree) )
		{
#ifdef TMA_DEBUG
ATprintf("F %t\n",ATgetArgument(*tree,0));
#endif
			if ( ATisEqual(ATgetFirst(args),ATgetArgument(*tree,0)) || (ATisList(ATgetFirst(args)) && ATisEqual(ATgetFirst(ATLgetFirst(args)),ATgetArgument(*tree,0))) )
			{
#ifdef TMA_DEBUG
ATprintf("true\n");
#endif
				*tree = ATAgetArgument(*tree,1);
				tree_matcher_aux(ATgetFirst(args),tree,vars,vals,len);
			} else {
#ifdef TMA_DEBUG
ATprintf("false\n");
#endif
				*tree = ATAgetArgument(*tree,2);
			}
		} else if ( isN(*tree) )
		{
#ifdef TMA_DEBUG
ATprintf("N\n");
#endif
			*tree = ATAgetArgument(*tree,0);
			args = ATgetNext(args);
		} else if ( isD(*tree) )
		{
#ifdef TMA_DEBUG
ATprintf("D\n");
#endif
			*tree = ATAgetArgument(*tree,0);
#ifdef TMA_DEBUG
ATprintf("return\n");
#endif
			return args;
		} else if ( isC(*tree) )
		{
#ifdef TMA_DEBUG
ATprintf("C\n");
#endif
//			int len = ATgetLength(*vars);
//			DECL_A(vars_a,ATermAppl,len);
//			DECL_A(vals_a,ATerm,len);
//			ATermList l = *vars;
//			ATermList m = *vals;

//			for (len=0; !ATisEmpty(l); l=ATgetNext(l),m=ATgetNext(m),len++)
//			{
//				vars_a[len] = ATAgetFirst(l);
//				vals_a[len] = ATgetFirst(m);
//			}

			if ( ATisEqual(build(ATgetArgument(*tree,0),-1,vars,vals,*len),trueint) )
			{
#ifdef TMA_DEBUG
ATprintf("true\n");
#endif
				*tree = ATAgetArgument(*tree,1);
			} else {
#ifdef TMA_DEBUG
ATprintf("false\n");
#endif
				*tree = ATAgetArgument(*tree,2);
			}

//			FREE_A(vals_a);
//			FREE_A(vars_a);
		} else {
#ifdef TMA_DEBUG
ATprintf("R/X? %t\n",*tree);
#endif
			assert(isR(*tree) || isX(*tree));
			return args;
		}
	}
#ifdef TMA_DEBUG
ATprintf("no more args\n");
#endif
	return args;
}

static ATerm tree_matcher(ATermList t, ATermAppl tree)
{
	ATermAppl vars[max_vars];
	ATerm vals[max_vars];
	int len = 0;
//	ATermList vars = ATmakeList0();
//	ATermList vals = ATmakeList0();

	ATermList rargs = tree_matcher_aux((ATerm) t,&tree,vars,vals,&len);

	if ( isR(tree) )
	{
//		int len = ATgetLength(vars);
//		DECL_A(vars_a,ATermAppl,len);
//		DECL_A(vals_a,ATerm,len);

//		for (len=0; !ATisEmpty(vars); vars=ATgetNext(vars),vals=ATgetNext(vals),len++)
//		{
//			vars_a[len] = ATAgetFirst(vars);
//			vals_a[len] = ATgetFirst(vals);
//		}

		ATerm rslt = ATgetArgument(tree,0);
		int rslt_len;
		if ( ATisList(rslt) )
		{
			rslt_len = ATgetLength(rslt)-1;
			if ( !ATisEmpty(rargs) )
			{
				rslt = (ATerm) ATconcat((ATermList) rslt,rargs);
			}
		} else {
			rslt_len = 0;
			if ( !ATisEmpty(rargs) )
			{
				rslt = (ATerm) ATinsert(rargs,rslt);
			}
		}

		ATerm r = build(rslt,rslt_len,vars,vals,len);

//		FREE_A(vals_a);
//		FREE_A(vars_a);

		return r;
	} else {
		return NULL;
	}
}




static ATerm OpId2Int(ATermAppl Term, bool add_opids)
{
	ATermInt i;

	if ( (i = (ATermInt) ATtableGet(term2int,(ATerm) Term)) == NULL )
	{
		if ( !add_opids )
		{
			return (ATerm) Term;
		}
		i = ATmakeInt(num_opids);
		ATtablePut(term2int,(ATerm) Term,(ATerm) i);
		num_opids++;
	}

	return (ATerm) i;
}

static ATerm toInner(ATermAppl Term, bool add_opids)
{
	ATermList l;

	if ( !gsIsDataAppl(Term) )
	{
		if ( gsIsOpId(Term) )
		{
			return (ATerm) OpId2Int(Term,add_opids);
		} else {
			return (ATerm) Term;
		}
	}

	l = ATmakeList0();
	while ( gsIsDataAppl(Term) )
	{
		l = ATinsert(l,(ATerm) toInner(ATAgetArgument((ATermAppl) Term,1),add_opids));
		Term = ATAgetArgument(Term,0);
	}
	if ( gsIsOpId(Term) )
	{
		l = ATinsert(l,(ATerm) OpId2Int(Term,add_opids));
	} else {
		l = ATinsert(l,(ATerm) Term);
	}

	return (ATerm) l;
}

static ATermAppl fromInner(ATerm Term)
{
	ATermList l;
	ATerm t;
	ATermAppl a;

	if ( !ATisList(Term) )
	{
		if ( ATisInt(Term) )
		{
			return int2term[ATgetInt((ATermInt) Term)];
		} else {
			return (ATermAppl) Term;
		}
	}

	if ( ATisEmpty((ATermList) Term) )
	{
		gsfprintf(stderr,"%s: invalid inner format term (%T)\n",NAME,Term);
		exit(1);
	}
	
	l = (ATermList) Term;
	t = ATgetFirst(l);
	if ( ATisInt(t) )
	{
		a = int2term[ATgetInt((ATermInt) t)];
	} else {
		a = (ATermAppl) t;
	}
	l = ATgetNext(l);
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		a = gsMakeDataAppl(a,fromInner(ATgetFirst(l)));
	}

	return a;
}

void rewrite_init_inner()
{
	ATermList l,m,n;
	ATermTable tmp_eqns;
	ATermInt i;

	if ( is_initialised )
	{
		ATtableDestroy(term2int);
		ATunprotectInt(&trueint);
		ATunprotectAFun(nilAFun);
		ATunprotectAFun(opidAFun);
		ATunprotectAFun(ruleAFun);
		ATunprotectAFun(afunS);
		ATunprotectAFun(afunM);
		ATunprotectAFun(afunF);
		ATunprotectAFun(afunN);
		ATunprotectAFun(afunD);
		ATunprotectAFun(afunR);
		ATunprotectAFun(afunCR);
		ATunprotectAFun(afunC);
		ATunprotectAFun(afunX);
		ATunprotect(&dummy);
		ATunprotectArray((ATerm *) int2term);
		ATunprotectArray((ATerm *) inner_eqns);
	}
	is_initialised = true;

	tmp_eqns = ATtableCreate(100,50); // XXX would be nice to know the number op OpIds
	term2int = ATtableCreate(100,50);

	max_vars = 0;

	trueint = (ATermInt) OpId2Int(gsMakeDataExprTrue(),true);
	ATprotectInt(&trueint);

	nilAFun = ATgetAFun(gsMakeNil());
	ATprotectAFun(nilAFun);
	opidAFun = ATgetAFun(gsMakeDataExprTrue());
	ATprotectAFun(opidAFun);
	ruleAFun = ATmakeAFun("@RULE@",4,false);
	ATprotectAFun(opidAFun);

	afunS = ATmakeAFun("@@S",2,ATfalse); // Store term ( target_variable, result_tree )
	ATprotectAFun(afunS);
	afunM = ATmakeAFun("@@M",3,ATfalse); // Match term ( match_variable, true_tree , false_tree )
	ATprotectAFun(afunM);
	afunF = ATmakeAFun("@@F",3,ATfalse); // Match function ( match_function, true_tree, false_tree )
	ATprotectAFun(afunF);
	afunN = ATmakeAFun("@@N",1,ATfalse); // Go to next parameter ( result_tree )
	ATprotectAFun(afunN);
	afunD = ATmakeAFun("@@D",1,ATfalse); // Go down a level ( result_tree )
	ATprotectAFun(afunD);
	afunR = ATmakeAFun("@@R",1,ATfalse); // End of tree ( matching_rule )
	ATprotectAFun(afunR);
	afunCR = ATmakeAFun("@@CR",2,ATfalse); // End of tree ( matching_rule* )
	ATprotectAFun(afunCR);
	afunC = ATmakeAFun("@@C",3,ATfalse); // Check condition ( condition, true_tree, false_tree )
	ATprotectAFun(afunC);
	afunX = ATmakeAFun("@@X",0,ATfalse); // End of tree
	ATprotectAFun(afunX);
	dummy = (ATerm) gsMakeNil();
	ATprotect(&dummy);


	l = opid_eqns;
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		// XXX only adds the last rule where lhs is an opid; this might go "wrong" if this rule is removed later
//		ATtablePut(tmp_eqns,OpId2Int(ATAgetArgument(ATAgetFirst(l),2),true),(ATerm) ATmakeList1((ATerm) ATmakeList4((ATerm) ATmakeList0(),toInner(ATAgetArgument(ATAgetFirst(l),1),true),(ATerm) ATmakeList0(),toInner(ATAgetArgument(ATAgetFirst(l),3),true))));
		ATtablePut(tmp_eqns,OpId2Int(ATAgetArgument(ATAgetFirst(l),2),true),(ATerm) ATmakeList1((ATerm) ATmakeAppl4(ruleAFun,(ATerm) ATmakeList0(),toInner(ATAgetArgument(ATAgetFirst(l),1),true),(ATerm) ATmakeList0(),toInner(ATAgetArgument(ATAgetFirst(l),3),true))));
	}

	l = dataappl_eqns;
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		m = (ATermList) toInner(ATAgetArgument(ATAgetFirst(l),2),true);
		if ( (n = (ATermList) ATtableGet(tmp_eqns,ATgetFirst(m))) == NULL )
		{
			n = ATmakeList0();
		}
//		n = ATinsert(n,(ATerm) ATmakeList4(ATgetArgument(ATAgetFirst(l),0),toInner(ATAgetArgument(ATAgetFirst(l),1),true),(ATerm) ATgetNext(m),toInner(ATAgetArgument(ATAgetFirst(l),3),true)));
		n = ATinsert(n,(ATerm) ATmakeAppl4(ruleAFun,ATgetArgument(ATAgetFirst(l),0),toInner(ATAgetArgument(ATAgetFirst(l),1),true),(ATerm) ATgetNext(m),toInner(ATAgetArgument(ATAgetFirst(l),3),true)));
		ATtablePut(tmp_eqns,ATgetFirst(m),(ATerm) n);
	}

	int2term = (ATermAppl *) malloc(num_opids*sizeof(ATermAppl));
	inner_eqns = (ATermAppl *) malloc(num_opids*sizeof(ATermAppl));
	for (int i=0; i<num_opids; i++)
	{
		int2term[i] = NULL;
		inner_eqns[i] = NULL;
	}
	ATprotectArray((ATerm *) int2term,num_opids);
	ATprotectArray((ATerm *) inner_eqns,num_opids);

	l = ATtableKeys(term2int);
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		i = (ATermInt) ATtableGet(term2int,ATgetFirst(l));
		int2term[ATgetInt(i)] = ATAgetFirst(l);
	}
	
	l = ATtableKeys(term2int);
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		i = (ATermInt) ATtableGet(term2int,ATgetFirst(l));
		if ( (m = (ATermList) ATtableGet(tmp_eqns,(ATerm) i)) != NULL )
		{
			inner_eqns[ATgetInt(i)] = create_tree(m,ATgetInt(i),&max_vars);
		}
	}
			
	ATtableDestroy(tmp_eqns);
}

void rewrite_finalise_inner()
{
	ATtableDestroy(term2int);
	ATunprotectInt(&trueint);
	ATunprotectAFun(nilAFun);
	ATunprotectAFun(opidAFun);
	ATunprotectAFun(ruleAFun);
	ATunprotectAFun(afunS);
	ATunprotectAFun(afunM);
	ATunprotectAFun(afunF);
	ATunprotectAFun(afunN);
	ATunprotectAFun(afunD);
	ATunprotectAFun(afunR);
	ATunprotectAFun(afunCR);
	ATunprotectAFun(afunC);
	ATunprotectAFun(afunX);
	ATunprotect(&dummy);
	ATunprotectArray((ATerm *) int2term);
	ATunprotectArray((ATerm *) inner_eqns);
	is_initialised = false;
}

void rewrite_add_inner(ATermAppl eqn)
{
/*	ATermList l;
	ATermAppl a,m;
	ATermInt i,j;
	unsigned int old_num;

	old_num = num_opids;

	a = ATAgetArgument(eqn,2);
	if ( gsIsOpId(a) )
	{
		j = (ATermInt) OpId2Int(a,true);
//		m = ATmakeList4((ATerm) ATmakeList0(),toInner(ATAgetArgument(eqn,1),true),(ATerm) ATmakeList0(),toInner(ATAgetArgument(eqn,3),true));
		m = ATmakeAppl4(ruleAFun,(ATerm) ATmakeList0(),toInner(ATAgetArgument(eqn,1),true),(ATerm) ATmakeList0(),toInner(ATAgetArgument(eqn,3),true));
	} else {
		l = (ATermList) toInner(a,true);
		j = (ATermInt) ATgetFirst(l);
//		m = ATmakeList4(ATgetArgument(eqn,0),toInner(ATAgetArgument(eqn,1),true),(ATerm) ATgetNext(l),toInner(ATAgetArgument(eqn,3),true));
		m = ATmakeAppl4(ruleAFun,ATgetArgument(eqn,0),toInner(ATAgetArgument(eqn,1),true),(ATerm) ATgetNext(l),toInner(ATAgetArgument(eqn,3),true));
	}

	if ( num_opids > old_num )
	{
		ATunprotectArray((ATerm *) int2term);
		ATunprotectArray((ATerm *) inner_eqns);

		int2term = (ATermAppl *) realloc(int2term,num_opids*sizeof(ATermAppl));
		inner_eqns = (ATermList *) realloc(inner_eqns,num_opids*sizeof(ATermList));
		for (int i=old_num; i<num_opids; i++)
		{
			int2term[i] = NULL;
			inner_eqns[i] = NULL;
		}
		ATprotectArray((ATerm *) int2term,num_opids);
		ATprotectArray((ATerm *) inner_eqns,num_opids);

		l = ATtableKeys(term2int);
		for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			i = (ATermInt) ATtableGet(term2int,ATgetFirst(l));
			if ( ATgetInt(i) >= old_num )
			{
				int2term[ATgetInt(i)] = ATAgetFirst(l);
				inner_eqns[ATgetInt(i)] = NULL;
			}
		}
	}

	if ( ATgetLength(ATgetArgument(eqn,0)) > max_vars)
	{
		max_vars = ATgetLength(ATgetArgument(eqn,0));
	}

	if ( inner_eqns[ATgetInt(j)] == NULL )
	{
		inner_eqns[ATgetInt(j)] = ATmakeList1((ATerm) m);
	} else {
// order should not matter
//		inner_eqns[ATgetInt(j)] = ATappend(inner_eqns[ATgetInt(j)],(ATerm) m);
		inner_eqns[ATgetInt(j)] = ATinsert(inner_eqns[ATgetInt(j)],(ATerm) m);
	}*/
}

void rewrite_remove_inner(ATermAppl eqn)
{
/*	ATermList l,n;
	ATermAppl a,m;
	ATerm t;

	a = ATAgetArgument(eqn,2);
	if ( gsIsOpId(a) )
	{
		t = OpId2Int(a,false);
//		m = ATmakeList4((ATerm) ATmakeList0(),toInner(ATAgetArgument(eqn,1),true),(ATerm) ATmakeList0(),toInner(ATAgetArgument(eqn,3),true));
		m = ATmakeAppl4(ruleAFun,(ATerm) ATmakeList0(),toInner(ATAgetArgument(eqn,1),true),(ATerm) ATmakeList0(),toInner(ATAgetArgument(eqn,3),true));
	} else {
		l = (ATermList) toInner(a,false);
		t = ATgetFirst(l);
//		m = ATmakeList4(ATgetArgument(eqn,0),toInner(ATAgetArgument(eqn,1),true),(ATerm) ATgetNext(l),toInner(ATAgetArgument(eqn,3),true));
		m = ATmakeAppl4(ruleAFun,ATgetArgument(eqn,0),toInner(ATAgetArgument(eqn,1),true),(ATerm) ATgetNext(l),toInner(ATAgetArgument(eqn,3),true));
	}

	if ( ATisInt(t) )
	{
		l = inner_eqns[ATgetInt((ATermInt) t)];
		n = ATmakeList0();
		for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			if ( !ATisEqual(m,ATgetFirst(l)) )
			{
				ATinsert(n,ATgetFirst(l));
			}
		}
		if ( ATisEmpty(n) )
		{
			inner_eqns[ATgetInt((ATermInt) t)] = NULL;
		} else {
// order should not matter
//			inner_eqns[ATgetInt((ATermInt) t)] = ATreverse(n);
			inner_eqns[ATgetInt((ATermInt) t)] = n;
		}
	}*/
}

static ATerm subst_values(ATermAppl *vars, ATerm *vals, int len, ATerm t)
{
	ATerm a,h;
	ATermList l,m;

	h = NULL;

	if ( ATisList(t) )
	{
		if ( ATisEmpty((ATermList) t) )
		{
			return t;
		}
		l = ATreverse((ATermList) t);
		m = ATmakeList0();
		while ( !ATisEmpty(l) )
		{
			a = subst_values(vars,vals,len,ATgetFirst(l));
			l = ATgetNext(l);
			if ( ATisEmpty(l) )
			{
				h = a;
			} else {
				m = ATinsert(m,a);
			}
		}
		if ( ATisList(h) )
		{
			return (ATerm) ATconcat((ATermList) h,m);
		} else {
			return (ATerm) ATinsert(m,h);
		}
	} else if ( gsIsDataVarId((ATermAppl) t) )
	{
		for (int i=0; i<len; i++)
		{
			if ( ATisEqual(t,vars[i]) )
			{
				return vals[i];
			}
		}
		gsfprintf(stderr,"%s: variable %T not assigned\n",NAME,t);
		exit(1);
	} else {
		return t;
	}
}

static bool match_inner(ATerm t, ATerm p, ATermAppl *vars, ATerm *vals, int *len)
{
	bool b;

	if ( ATisList(p) )
	{
		if ( ATisList(t) )
		{
			if ( ATgetLength((ATermList) t) == ATgetLength((ATermList) p) )
			{
				b = true;
				for (; !ATisEmpty((ATermList) t) && b; t=(ATerm) ATgetNext((ATermList) t),p=(ATerm) ATgetNext((ATermList) p))
				{
					b &= match_inner(ATgetFirst((ATermList) t),ATgetFirst((ATermList) p),vars,vals,len);
				}
				return b;
			}
		}
		return false;
	} else if ( ATisInt(p) || gsIsOpId((ATermAppl) p) )
	{
		return ATisEqual(t,p);
	} else /* if ( gsIsDataVarId((ATermAppl) p) ) */ {
		for (int i=0; i<*len; i++)
		{
			if ( ATisEqual(p,vars[i]) )
			{
				if ( ATisEqual(t,vals[i]) )
				{
					return true;
				} else {
					return false;
				}
			}
		}
		vars[*len] = (ATermAppl) p;
		vals[*len] = t;
		(*len)++;
		return true;
	}
}

static ATermList build_args(ATermList args, int buildargs, ATermAppl *vars, ATerm *vals, int len)
{
	if ( (buildargs == 0) || ATisEmpty(args) )
	{
		return args;
	} else {
		return ATinsert(build_args(ATgetNext(args),buildargs-1,vars,vals,len),build(ATgetFirst(args),-1,vars,vals,len));
	}
}

static ATerm build(ATerm Term, int buildargs, ATermAppl *vars, ATerm *vals, int len)
{
//gsfprintf(stderr,"build(%T,%i)\n\n",Term,buildargs);

	if ( ATisList(Term) )
	{
		ATerm head = ATgetFirst((ATermList) Term);
		ATermList args = ATgetNext((ATermList) Term);

		if ( buildargs == -1 )
		{
			buildargs = ATgetLength(args);
		}

/*		l = ATmakeList0();
		for (int i=0; i<buildargs; i++)
		{
			l = ATinsert(l,build(ATgetFirst(args),-1,vars,vals,len));
			args = ATgetNext(args);
		}
		for (;!ATisEmpty(args);args=ATgetNext(args))
		{
			l = ATinsert(l,ATgetFirst(args));
		}
		args = ATreverse(l);*/
		args = build_args(args,buildargs,vars,vals,len);

		int b = 1;
		while ( !ATisInt(head) && b )
		{
			for (int i=0; i<len; i++)
			{
				if ( ATisEqual(head,vars[i]) )
				{
					if ( ATisList(vals[i]) )
					{
						head = ATgetFirst((ATermList) vals[i]);
						args = ATconcat(ATgetNext((ATermList) vals[i]),args);
					} else {
						head = vals[i];
					}
					break;
				}
				b = 0;
			}
		}
		if ( ATisInt(head) )
		{
			return rewrite_func((ATermInt) head,args);
		} else {
			return (ATerm) ATinsert(args,head);
		}
	} else if ( ATisInt(Term) ) {
		return rewrite_func((ATermInt) Term, ATmakeList0());
	} else {
		for (int i=0; i<len; i++)
		{
			if ( ATisEqual(Term,vars[i]) )
			{
				return vals[i];
			}
		}
		return Term;
	}
}

static ATerm rewrite_func(ATermInt op, ATermList args)
{
	ATermAppl tree;
//	DECL_A(vars,ATermAppl,max_vars);
//	DECL_A(vals,ATerm,max_vars);
//	int pos;
//gsfprintf(stderr,"rewrite_func(%T,%T)\n\n",op,args);

	if ( (tree = inner_eqns[ATgetInt(op)]) != NULL )
	{
		ATerm r = tree_matcher(ATinsert(args,(ATerm) op),tree);

		if ( r != NULL )
		{
			return r;
		}
	}

/*		for (; !ATisEmpty(m); m=ATgetNext(m))
		{
//			ATermList rule = ATgetNext(ATLgetFirst(m));
//			ATerm cond = ATgetFirst(rule); rule=ATgetNext(rule);
//			ATermList rargs = ATLgetFirst(rule); rule=ATgetNext(rule);
//			ATerm rslt = ATgetFirst(rule);
			ATermAppl rule = (ATermAppl) ATgetFirst(m);
			ATermList rargs = (ATermList) ATgetArgument(rule,2);
			ATermList l2 = args;

			bool match = true;
			pos = 0;
			while ( !ATisEmpty(rargs) )
			{
				if ( ATisEmpty(l2) )
				{
					match = false;
					break;
				}
	
				if ( !match_inner(ATgetFirst(l2),ATgetFirst(rargs),vars,vals,&pos) )
				{
					match = false;
					break;
				}
	
				rargs = ATgetNext(rargs);
				l2 = ATgetNext(l2);
			}
			if ( match )
			{
				ATerm cond = ATgetArgument(rule,1);
				if ( is_nil(cond) || ATisEqual(build(cond,-1,vars,vals,pos),trueint) )
				{
					ATerm rslt = ATgetArgument(rule,3);
					int rslt_len;
					if ( ATisList(rslt) )
					{
						rslt_len = ATgetLength(rslt)-1;
						if ( !ATisEmpty(l2) )
						{
							rslt = (ATerm) ATconcat((ATermList) rslt,l2);
						}
					} else {
						rslt_len = 0;
						if ( !ATisEmpty(l2) )
						{
							rslt = (ATerm) ATinsert(l2,rslt);
						}
					}
					ATerm t = build(rslt,rslt_len,vars,vals,pos);
					FREE_A(vals)
					FREE_A(vars)
					return t;
				}		
			}
		}
	}

	FREE_A(vals)
	FREE_A(vars)*/

	if ( ATisEmpty(args) )
	{
		return (ATerm) op;
	} else {
		return (ATerm) ATinsert(args,(ATerm) op);
	}
}

static ATermList rewrite_listelts(ATermList l)
{
	if ( ATisEmpty(l) )
	{
		return l;
	} else {
		return ATinsert(rewrite_listelts(ATgetNext(l)),rewrite(ATgetFirst(l)));
	}
}

static ATerm rewrite(ATerm Term)
{
//gsfprintf(stderr,"rewrite(%T)\n\n",Term);
	if ( ATisList(Term) )
	{
		ATermList l = ATgetNext((ATermList) Term);

/*		m = ATmakeList0();
		for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			m = ATinsert(m,rewrite(ATgetFirst(l)));
		}
		l = ATreverse(m);*/
		l = rewrite_listelts(l);

		if ( !ATisInt(ATgetFirst((ATermList) Term)) )
		{
			if ( (ATisAppl(ATgetFirst((ATermList) Term))) && gsIsDataVarId(ATAgetFirst((ATermList) Term)) ) 
			{
				ATerm a = RWapplySubstitution(ATgetFirst((ATermList) Term));
				if ( ATisList(a) )
				{
					a = (ATerm) ATinsert(rewrite_listelts(ATgetNext((ATermList) a)),ATgetFirst((ATermList) a));
					Term = (ATerm) ATconcat((ATermList) a,l);
				} else {
					Term = (ATerm) ATinsert(l,a);
				}
			} else {
				Term = (ATerm) ATinsert(l,ATgetFirst((ATermList) Term));
			}
		}
		if ( ATisInt(ATgetFirst((ATermList) Term)) )
		{
			Term = rewrite_func((ATermInt) ATgetFirst((ATermList) Term), l);
		}

		return Term;
	} else if ( ATisInt(Term) )
	{
		return rewrite_func((ATermInt) Term, ATmakeList0());
	} else {
		return RWapplySubstitution(Term);
	}
}

ATerm rewrite_inner(ATerm Term)
{
	return rewrite(Term);
}

ATerm to_rewrite_format_inner(ATermAppl Term)
{
	return toInner(Term,false);
}

ATermAppl from_rewrite_format_inner(ATerm Term)
{
	return fromInner(Term);
}

#ifdef __cplusplus
}
#endif
