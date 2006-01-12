#ifdef NO_DYNLOAD

#include <stdlib.h>
#include "libprint_c.h"
#include "rewr_innerc.h"

RewriterCompilingInnermost::RewriterCompilingInnermost(ATermAppl DataEqnSpec)
{
	gsfprintf(stderr,"error: compiling Innermost rewriter is not available\n");
	exit(1);
}

RewriterCompilingInnermost::~RewriterCompilingInnermost()
{
}

ATermAppl RewriterCompilingInnermost::rewrite(ATermAppl Term)
{
	return NULL;
}

ATerm RewriterCompilingInnermost::toRewriteFormat(ATermAppl Term)
{
	return NULL;
}

ATermAppl RewriterCompilingInnermost::fromRewriteFormat(ATerm Term)
{
	return NULL;
}

ATerm RewriterCompilingInnermost::rewriteInternal(ATerm Term)
{
	return NULL;
}

ATermList RewriterCompilingInnermost::rewriteInternalList(ATermList Terms)
{
	return NULL;
}

void RewriterCompilingInnermost::setSubstitution(ATermAppl Var, ATerm Expr)
{
}

ATerm RewriterCompilingInnermost::getSubstitution(ATermAppl Var)
{
	return NULL;
}

void RewriterCompilingInnermost::clearSubstitution(ATermAppl Var)
{
}

void RewriterCompilingInnermost::clearSubstitutions()
{
}

#else

#define NAME "rewr_innerc"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <dlfcn.h>
#include <assert.h>
#include "aterm2.h"
#include "liblowlevel.h"
#include "libstruct.h"
#include "libprint_c.h"
#include "rewr_innerc_aux.h"
#include "rewr_innerc.h"


#ifndef INNERC_CFLAGS
#define INNERC_CFLAGS  ""
#endif
#ifndef INNERC_CPPFLAGS
#define INNERC_CPPFLAGS  ""
#endif
#ifndef INNERC_LDFLAGS
#define INNERC_LDFLAGS  ""
#endif

#define ATXgetArgument(x,y) ((unsigned int) (intptr_t) ATgetArgument(x,y))

static ATermList ATinsertA(ATermList l, ATermAppl a)
{ return ATinsert(l,(ATerm)a);
}

static bool ATisAppl(ATerm a)
{
  return (ATgetType(a) == AT_APPL);
}

static bool ATisList(ATerm a)
{
  return (ATgetType(a) == AT_LIST);
}

static bool ATisInt(ATerm a)
{
  return (ATgetType(a) == AT_INT);
}

static AFun afunS, afunM, afunF, afunN, afunD, afunR, afunCR, afunC, afunX, afunRe, afunCRe, afunMe;
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
#define isRe(x) ATisEqualAFun(ATgetAFun(x),afunRe)
#define isCRe(x) ATisEqualAFun(ATgetAFun(x),afunCRe)
#define isMe(x) ATisEqualAFun(ATgetAFun(x),afunMe)

static unsigned int is_initialised = 0;

static void initialise_common()
{
	if ( is_initialised == 0 )
	{
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
		afunCR = ATmakeAFun("@@CR",2,ATfalse); // End of tree ( condition, matching_rule )
		ATprotectAFun(afunCR);
		afunC = ATmakeAFun("@@C",3,ATfalse); // Check condition ( condition, true_tree, false_tree )
		ATprotectAFun(afunC);
		afunX = ATmakeAFun("@@X",0,ATfalse); // End of tree
		ATprotectAFun(afunX);
		afunRe = ATmakeAFun("@@Re",2,ATfalse); // End of tree ( matching_rule , vars_of_rule)
		ATprotectAFun(afunRe);
		afunCRe = ATmakeAFun("@@CRe",4,ATfalse); // End of tree ( condition, matching_rule, vars_of_condition, vars_of_rule )
		ATprotectAFun(afunCRe);
		afunMe = ATmakeAFun("@@Me",2,ATfalse); // Match term ( match_variable, variable_index )
		ATprotectAFun(afunMe);
		dummy = (ATerm) gsMakeNil();
		ATprotect(&dummy);
	}

	is_initialised++;
}

static void finalise_common()
{
	assert(is_initialised > 0 );
	is_initialised--;

	if ( is_initialised == 0 )
	{
		ATunprotect(&dummy);
		ATunprotectAFun(afunMe);
		ATunprotectAFun(afunCRe);
		ATunprotectAFun(afunRe);
		ATunprotectAFun(afunX);
		ATunprotectAFun(afunC);
		ATunprotectAFun(afunCR);
		ATunprotectAFun(afunR);
		ATunprotectAFun(afunD);
		ATunprotectAFun(afunN);
		ATunprotectAFun(afunF);
		ATunprotectAFun(afunM);
		ATunprotectAFun(afunS);
	}
}


ATerm RewriterCompilingInnermost::OpId2Int(ATermAppl Term, bool add_opids)
{
  ATermInt i;

  if ( (i = (ATermInt) ATtableGet(term2int,(ATerm) Term)) == NULL )
  {
    if ( !add_opids )
    {
      return (ATerm) Term;
    }
    i = ATmakeInt(num_opids);
//gsfprintf(stderr,"%i := %p (%T)\n\n",num_opids,Term,Term);
    ATtablePut(term2int,(ATerm) Term,(ATerm) i);
    num_opids++;
  }

  return (ATerm) i;
}

ATerm RewriterCompilingInnermost::toInner(ATermAppl Term, bool add_opids)
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

ATermAppl RewriterCompilingInnermost::fromInner(ATerm Term)
{
        ATermList l;
        ATerm t;
        ATermAppl a;

        if ( !ATisList(Term) )
        {
                if ( ATisInt(Term) )
                {
//gsfprintf(stderr,"%i -> %p (%T)\n\n",ATgetInt((ATermInt) Term),int2term[ATgetInt((ATermInt) Term)],int2term[ATgetInt((ATermInt) Term)]);
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

static ATerm Apply(ATermList l)
{
  char c[10];
  int n=ATgetLength(l);
  sprintf(c,"appl#%d",n);
  
  return (ATerm)ATmakeApplList(ATmakeAFun(c,n,ATfalse),l);
}

static ATerm toInnerc(ATerm Term)
{
  if ( !ATisList(Term) )
  {
    if ( ATisInt(Term) )
    { return Apply(ATinsert(ATempty,Term));
    } 
    else 
    if (gsIsDataVarId((ATermAppl)Term))
    { return Term;
    }
    else
    {
      ATerror("%s: Do not deal with application terms correctly\n%T\n\n",NAME,Term);
    }
  }

  if ( ATisEmpty((ATermList) Term) )
  {
    ATerror("%s: invalid inner format term (%T)\n",NAME,Term);
  }

  ATermList l=ATinsert(ATempty,ATgetFirst((ATermList)Term));
  for( ATermList l1=ATgetNext((ATermList)Term) ; 
       l1!=ATempty ; 
       l1=ATgetNext(l1))
  { l=ATinsert(l,toInnerc(ATgetFirst(l1)));
  }
  l=ATreverse(l);
  ATerm r=Apply((ATermList) l);
  // gsfprintf(stderr,"RESULT: %T\n%T\n%T\n\n",Term,r,l);
  return r;
}

ATerm RewriterCompilingInnermost::toRewriteFormat(ATermAppl t)
{ 
  int old_opids = num_opids;
  ATerm r = toInnerc(toInner(t,true));

  if ( old_opids < num_opids )
  {
    ATunprotectArray((ATerm *) int2term);
    int2term = (ATermAppl *) realloc(int2term,num_opids*sizeof(ATermAppl));
    for (int i=old_opids; i<num_opids; i++)
    {
	    int2term[i] = NULL;
    }
    ATprotectArray((ATerm *) int2term,num_opids);

    ATunprotectArray((ATerm *) innerc_eqns);
    innerc_eqns = (ATermList *) realloc(innerc_eqns,num_opids*sizeof(ATermList));
    for (int i=old_opids; i<num_opids; i++)
    {
	    innerc_eqns[i] = NULL;
    }
    ATprotectArray((ATerm *) innerc_eqns,num_opids);

    ATermList l = ATtableKeys(term2int);
    for (; !ATisEmpty(l); l=ATgetNext(l))
    {
      int i = ATgetInt((ATermInt) ATtableGet(term2int,ATgetFirst(l)));
      if ( i >= old_opids )
      {
	      int2term[i] = ATAgetFirst(l);
	      innerc_eqns[i] = NULL;
      }
    }
  }

  return r;
}

ATermAppl RewriterCompilingInnermost::fromRewriteFormat(ATerm t)
{
	if ( ATisInt(t) )
	{
		return int2term[ATgetInt((ATermInt) t)];
	} else if ( gsIsDataVarId((ATermAppl) t) )
	{
		return (ATermAppl) t;
	} else {
		int arity = ATgetArity(ATgetAFun((ATermAppl) t));
		ATermAppl a = fromRewriteFormat(ATgetArgument((ATermAppl) t,0));
		for (int i=1; i<arity; i++)
		{
			a = gsMakeDataAppl(a,fromRewriteFormat(ATgetArgument((ATermAppl) t,i)));
		}
		return a;
	}
}

static char *whitespace_str = NULL;
static int whitespace_len;
static int whitespace_pos;
static char *whitespace(int len)
{
  int i;

  if ( whitespace_str == NULL )
  {
    whitespace_str = (char *) malloc((2*len+1)*sizeof(char));
    for (i=0; i<2*len; i++)
    {
      whitespace_str[i] = ' ';
    }
    whitespace_len = 2*len;
    whitespace_pos = len;
    whitespace_str[whitespace_pos] = 0;
  } else {
    if ( len > whitespace_len )
    {
      whitespace_str = (char *) realloc(whitespace_str,(2*len+1)*sizeof(char));
      for (i=whitespace_len; i<2*len; i++)
      {
        whitespace_str[i] = ' ';
      }
      whitespace_len = 2*len;
    }

    whitespace_str[whitespace_pos] = ' ';
    whitespace_pos = len;
    whitespace_str[whitespace_pos] = 0;
  }

  return whitespace_str;
}


#ifdef _INNERC_STORE_TREES
int RewriterCompilingInnermost::write_tree(FILE *f, ATermAppl tree, int *num_states)
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
	} else if ( isX(tree) )
	{
		ATfprintf(f,"n%i [label=\"X\"]\n",*num_states);
		return (*num_states)++;
	}

	return -1;
}

void RewriterCompilingInnermost::tree2dot(ATermAppl tree, char *name, char *filename)
{
	FILE *f;
	int num_states = 0;

	if ( (f = fopen(filename,"wb")) == NULL )
	{
		perror("fopen");
		return;
	}

	fprintf(f,"digraph \"%s\" {\n",name);
	write_tree(f,tree,&num_states);
	fprintf(f,"}\n");

	fclose(f);
}
#endif

static void term2seq(ATerm t, ATermList *s, int *var_cnt)
{
	if ( ATisList(t) )
	{
		ATermList l;

		l = ATgetNext((ATermList) t);
		t = ATgetFirst((ATermList) t);

		*s = ATinsert(*s, (ATerm) ATmakeAppl3(afunF,(ATerm) t,dummy,dummy));

		for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			term2seq(ATgetFirst(l),s,var_cnt);
			if ( !ATisEmpty(ATgetNext(l)) )
			{
				*s = ATinsert(*s, (ATerm) ATmakeAppl1(afunN,dummy));
			}
		}
		*s = ATinsert(*s, (ATerm) ATmakeAppl1(afunD,dummy));
	} else if ( ATisInt(t) )
	{
		term2seq((ATerm) ATmakeList1(t),s,var_cnt);
	} else if ( gsIsDataVarId((ATermAppl) t) )
	{
		ATerm store = (ATerm) ATmakeAppl2(afunS,(ATerm) t,dummy);

		if ( ATindexOf(*s,store,0) >= 0 )
		{
			*s = ATinsert(*s, (ATerm) ATmakeAppl3(afunM,(ATerm) t,dummy,dummy));
		} else {
			(*var_cnt)++;
			*s = ATinsert(*s, store);
		}
	} else {
		assert(0);
	}

}

static void get_used_vars_aux(ATerm t, ATermList *vars)
{
	if ( ATisList(t) )
	{
		for (; !ATisEmpty((ATermList) t); t=(ATerm) ATgetNext((ATermList) t))
		{
			get_used_vars_aux(ATgetFirst((ATermList) t),vars);
		}
	} else if ( ATisAppl(t) )
	{
		if ( gsIsDataVarId((ATermAppl) t) )
		{
			if ( ATindexOf(*vars,t,0) == -1 )
			{
				*vars = ATinsert(*vars,t);
			}
		} else {
			int a = ATgetArity(ATgetAFun((ATermAppl) t));
			for (int i=0; i<a; i++)
			{
				get_used_vars_aux(ATgetArgument((ATermAppl) t,i),vars);
			}
		}
	}
}

static ATermList get_used_vars(ATerm t)
{
	ATermList l = ATmakeList0();

	get_used_vars_aux(t,&l);

	return l;
}

static ATermList create_sequence(ATermList rule, int *var_cnt)
{
	ATermAppl pat = (ATermAppl) ATelementAt(rule,2);
	ATerm cond = ATelementAt(rule,1);
	ATerm rslt = ATelementAt(rule,3);
	ATermList pars = ATmakeList0();
	ATermList rseq = ATmakeList0();
	
	pars = (ATermList) pat;
	//ATfprintf(stderr,"pattern pars: %t\n",pars);
	for (; !ATisEmpty(pars); pars=ATgetNext(pars))
	{
		term2seq(ATgetFirst(pars),&rseq,var_cnt);
		if ( !ATisEmpty(ATgetNext(pars)) )
		{
			rseq = ATinsert(rseq, (ATerm) ATmakeAppl1(afunN,dummy));
		}
	}
	//ATfprintf(stderr,"rseq: %t\n",rseq);
	if ( ATisAppl(cond) )
		rseq = ATinsert(rseq,(ATerm) ATmakeAppl2(afunRe,rslt,(ATerm) get_used_vars(rslt)));
	else
		rseq = ATinsert(rseq,(ATerm) ATmakeAppl4(afunCRe,cond,rslt,(ATerm) get_used_vars(cond),(ATerm) get_used_vars(rslt)));

	return ATreverse(rseq);
}


// Structure for build_tree paramters
typedef struct {
	ATermList Flist;   // List of sequences of which the first action is an F
	ATermList Slist;   // List of sequences of which the first action is an S
	ATermList Mlist;   // List of sequences of which the first action is an M
	ATermList stack;   // Stack to maintain the sequences that do not have to
	                   // do anything in the current term
	ATermList upstack; // List of sequences that have done an F at the current
	                   // level
} build_pars;

static void initialise_build_pars(build_pars *p)
{
	p->Flist = ATmakeList0();
	p->Slist = ATmakeList0();
	p->Mlist = ATmakeList0();
	p->stack = ATmakeList1((ATerm) ATmakeList0());
	p->upstack = ATmakeList0();
}

static ATermList add_to_stack(ATermList stack, ATermList seqs, ATermAppl *r, ATermList *cr)
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
		} else if ( isRe(ATAgetFirst(e)) )
		{
			*r = ATAgetFirst(e);
		} else {
			*cr = ATinsert(*cr,ATgetFirst(e));
		}
	}
	
	return ATinsert(add_to_stack(ATgetNext(stack),l,r,cr),(ATerm) h);
}

static void add_to_build_pars(build_pars *pars,ATermList seqs, ATermAppl *r, ATermList *cr)
{
	ATermList l = ATmakeList0();

	for (; !ATisEmpty(seqs); seqs=ATgetNext(seqs))
	{
		ATermList e = ATLgetFirst(seqs);

		if ( isD(ATAgetFirst(e)) || isN(ATAgetFirst(e)) )
		{
			l = ATinsert(l,(ATerm) e);
		} else if ( isS(ATAgetFirst(e)) )
		{
			pars->Slist = ATinsert(pars->Slist,(ATerm) e);
		} else if ( isMe(ATAgetFirst(e)) ) // M should not appear at the head of a seq
		{
			pars->Mlist = ATinsert(pars->Mlist,(ATerm) e);
		} else if ( isF(ATAgetFirst(e)) )
		{
			pars->Flist = ATinsert(pars->Flist,(ATerm) e);
		} else if ( isRe(ATAgetFirst(e)) )
		{
			*r = ATAgetFirst(e);
		} else {
			*cr = ATinsert(*cr,ATgetFirst(e));
		}
	}

	pars->stack = add_to_stack(pars->stack,l,r,cr);
}

static char tree_var_str[20];
static ATermAppl createFreshVar(ATermAppl sort,int *i)
{
	sprintf(tree_var_str,"@var_%i",(*i)++);
	return gsMakeDataVarId(gsString2ATermAppl(tree_var_str),sort);
}

static ATermList subst_var(ATermList l, ATermAppl old, ATerm new_val, ATerm num, ATermList substs)
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
			head = ATmakeAppl2(afunMe,new_val,num);
		}
	} else if ( isCRe(head) )
	{
		ATermList l = (ATermList) ATgetArgument(head,2);
		ATermList m = ATmakeList0();
		for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			if ( ATisEqual(ATgetFirst(l),old) )
			{
				m = ATinsert(m,num);
			} else {
				m = ATinsert(m,ATgetFirst(l));
			}
		}
		l = (ATermList) ATgetArgument(head,3);
		ATermList n = ATmakeList0();
		for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			if ( ATisEqual(ATgetFirst(l),old) )
			{
				n = ATinsert(n,num);
			} else {
				n = ATinsert(n,ATgetFirst(l));
			}
		}
		head = ATmakeAppl4(afunCRe,gsSubstValues(substs,ATgetArgument(head,0),true),gsSubstValues(substs,ATgetArgument(head,1),true),(ATerm) m, (ATerm) n);
	} else if ( isRe(head) )
	{
		ATermList l = (ATermList) ATgetArgument(head,1);
		ATermList m = ATmakeList0();
		for (; !ATisEmpty(l); l=ATgetNext(l))
		{
			if ( ATisEqual(ATgetFirst(l),old) )
			{
				m = ATinsert(m,num);
			} else {
				m = ATinsert(m,ATgetFirst(l));
			}
		}
		head = ATmakeAppl2(afunRe,gsSubstValues(substs,ATgetArgument(head,0),true),(ATerm) m);
	}

	return ATinsert(subst_var(l,old,new_val,num,substs),(ATerm) head);
}

//#define BT_DEBUG
#ifdef BT_DEBUG
#define print_return(x,y) ATermAppl a = y; ATfprintf(stderr,x "return %t\n\n",a); return a;
#else
#define print_return(x,y) return y;
#endif
//static int max_tree_vars;
static int *treevars_usedcnt;

static void inc_usedcnt(ATermList l)
{
	for (; !ATisEmpty(l); l=ATgetNext(l))
	{
		treevars_usedcnt[ATgetInt((ATermInt) ATgetFirst(l))]++;
	}
}

static ATermAppl build_tree(build_pars pars, int i)
{
#ifdef BT_DEBUG
ATfprintf(stderr,"build_tree(  %t  ,  %t  ,  %t  ,  %t  ,  %t  ,  %i  )\n\n",pars.Flist,pars.Slist,pars.Mlist,pars.stack,pars.upstack,i);
#endif

	if ( !ATisEmpty(pars.Slist) )
	{
		ATermList l,m;

		int k = i;
		ATermAppl v = createFreshVar(ATAgetArgument(ATAgetArgument(ATAgetFirst(ATLgetFirst(pars.Slist)),0),1),&i); 
		treevars_usedcnt[k] = 0;

		l = ATmakeList0();
		m = ATmakeList0();
		for (; !ATisEmpty(pars.Slist); pars.Slist=ATgetNext(pars.Slist))
		{
			ATermList e = ATLgetFirst(pars.Slist);

			e = subst_var(e,ATAgetArgument(ATAgetFirst(e),0),(ATerm) v,(ATerm) ATmakeInt(k),ATmakeList1((ATerm) gsMakeSubst(ATgetArgument(ATAgetFirst(e),0),(ATerm) v)));
//			e = gsSubstValues_List(ATmakeList1((ATerm) gsMakeSubst(ATgetArgument(ATAgetFirst(e),0),(ATerm) v)),e,true);

			l = ATinsert(l,ATgetFirst(e));
			m = ATinsert(m,(ATerm) ATgetNext(e));
		}
		
		ATermAppl r = NULL;
		ATermList readies = ATmakeList0();

		pars.stack = add_to_stack(pars.stack,m,&r,&readies);

		if ( r == NULL )
		{
			ATermAppl tree;

			tree = build_tree(pars,i);
			for (; !ATisEmpty(readies); readies=ATgetNext(readies))
			{
				inc_usedcnt((ATermList) ATgetArgument(ATAgetFirst(readies),2));
				inc_usedcnt((ATermList) ATgetArgument(ATAgetFirst(readies),3));
				tree = ATmakeAppl3(afunC,ATgetArgument(ATAgetFirst(readies),0),(ATerm) ATmakeAppl1(afunR,ATgetArgument(ATAgetFirst(readies),1)),(ATerm) tree);
			}
			r = tree;
		} else {
			inc_usedcnt((ATermList) ATgetArgument(r,1));
			r = ATmakeAppl1(afunR,ATgetArgument(r,0));
		}

		if ( (treevars_usedcnt[k] > 0) || ((k == 0) && isR(r)) )
		{
			print_return("",ATmakeAppl2(afunS,(ATerm) v,(ATerm) r));
		} else {
			print_return("",r);
		}
	} else if ( !ATisEmpty(pars.Mlist) )
	{
		ATerm M = ATgetFirst(ATLgetFirst(pars.Mlist));

		ATermList l = ATmakeList0();
		ATermList m = ATmakeList0();
		for (; !ATisEmpty(pars.Mlist); pars.Mlist=ATgetNext(pars.Mlist))
		{
			if ( ATisEqual(M,ATgetFirst(ATLgetFirst(pars.Mlist))) )
			{
				l = ATinsert(l,(ATerm) ATgetNext(ATLgetFirst(pars.Mlist)));
			} else {
				m = ATinsert(m,ATgetFirst(pars.Mlist));
			}
		}
		pars.Mlist = m;

		ATermAppl true_tree,false_tree;
		ATermAppl r = NULL;
		ATermList readies = ATmakeList0();

		ATermList newstack = add_to_stack(pars.stack,l,&r,&readies);

		false_tree = build_tree(pars,i);

		if  ( r == NULL )
		{
			pars.stack = newstack;
			true_tree = build_tree(pars,i);
			for (; !ATisEmpty(readies); readies=ATgetNext(readies))
			{
				inc_usedcnt((ATermList) ATgetArgument(ATAgetFirst(readies),2));
				inc_usedcnt((ATermList) ATgetArgument(ATAgetFirst(readies),3));
				true_tree = ATmakeAppl3(afunC,ATgetArgument(ATAgetFirst(readies),0),(ATerm) ATmakeAppl1(afunR,ATgetArgument(ATAgetFirst(readies),1)),(ATerm) true_tree);
			}
		} else {
			inc_usedcnt((ATermList) ATgetArgument(r,1));
			true_tree = ATmakeAppl1(afunR,ATgetArgument(r,0));
		}

		if ( ATisEqual(true_tree,false_tree) )
		{
			print_return("",true_tree);
		} else {
			treevars_usedcnt[ATgetInt((ATermInt) ATgetArgument((ATermAppl) M,1))]++;
			print_return("",ATmakeAppl3(afunM,ATgetArgument((ATermAppl) M,0),(ATerm) true_tree,(ATerm) false_tree));
		}
	} else if ( !ATisEmpty(pars.Flist) ) {
		ATermList F = ATLgetFirst(pars.Flist);
		ATermAppl true_tree,false_tree;

		ATermList newupstack = pars.upstack;
		ATermList l = ATmakeList0();

		for (; !ATisEmpty(pars.Flist); pars.Flist=ATgetNext(pars.Flist))
		{
			if ( ATisEqual(ATgetFirst(ATLgetFirst(pars.Flist)),ATgetFirst(F)) )
			{
				newupstack = ATinsert(newupstack, (ATerm) ATgetNext(ATLgetFirst(pars.Flist)));
			} else {
				l = ATinsert(l,ATgetFirst(pars.Flist));
			}
		}
		
		pars.Flist = l;
		false_tree = build_tree(pars,i);
		pars.Flist = ATmakeList0();
		pars.upstack = newupstack;
		true_tree = build_tree(pars,i);

		if ( ATisEqual(true_tree,false_tree) )
		{
			print_return("",true_tree);
		} else {
			print_return("",ATmakeAppl3(afunF,ATgetArgument(ATAgetFirst(F),0),(ATerm) true_tree,(ATerm) false_tree));
		}
	} else if ( !ATisEmpty(pars.upstack) ) {
		ATermList l;
		
		ATermAppl r = NULL;
		ATermList readies = ATmakeList0();

		pars.stack = ATinsert(pars.stack,(ATerm) ATmakeList0());
		l = pars.upstack;
		pars.upstack = ATmakeList0();
		add_to_build_pars(&pars,l,&r,&readies);
		

		if ( r == NULL )
		{
			ATermAppl t = build_tree(pars,i);

			for (; !ATisEmpty(readies); readies=ATgetNext(readies))
			{
				inc_usedcnt((ATermList) ATgetArgument(ATAgetFirst(readies),2));
				inc_usedcnt((ATermList) ATgetArgument(ATAgetFirst(readies),3));
				t = ATmakeAppl3(afunC,ATgetArgument(ATAgetFirst(readies),0),(ATerm) ATmakeAppl1(afunR,ATgetArgument(ATAgetFirst(readies),1)),(ATerm) t);
			}

			print_return("",t);
		} else {
			inc_usedcnt((ATermList) ATgetArgument(r,1));
			print_return("",ATmakeAppl1(afunR,ATgetArgument(r,0)));
		}
	} else {
		if ( ATisEmpty(ATLgetFirst(pars.stack)) )
		{
			if ( ATisEmpty(ATgetNext(pars.stack)) )
			{
				print_return("",ATmakeAppl0(afunX));
			} else {
				pars.stack = ATgetNext(pars.stack);
				print_return("",ATmakeAppl1(afunD,(ATerm) build_tree(pars,i)));
//				print_return("",build_tree(pars,i));
			}
		} else {
			ATermList l = ATLgetFirst(pars.stack);
			ATermAppl r = NULL;
			ATermList readies = ATmakeList0();

			pars.stack = ATinsert(ATgetNext(pars.stack),(ATerm) ATmakeList0());
			add_to_build_pars(&pars,l,&r,&readies);

			ATermAppl tree;
			if ( r == NULL )
			{
				tree = build_tree(pars,i);
				for (; !ATisEmpty(readies); readies=ATgetNext(readies))
				{
					inc_usedcnt((ATermList) ATgetArgument(ATAgetFirst(readies),2));
					inc_usedcnt((ATermList) ATgetArgument(ATAgetFirst(readies),3));
					tree = ATmakeAppl3(afunC,ATgetArgument(ATAgetFirst(readies),0),(ATerm) ATmakeAppl1(afunR,ATgetArgument(ATAgetFirst(readies),1)),(ATerm) tree);
				}
			} else {
				inc_usedcnt((ATermList) ATgetArgument(r,1));
				tree = ATmakeAppl(afunR,ATgetArgument(r,0));
			}
	
			print_return("",ATmakeAppl1(afunN,(ATerm) tree));
		}
	}
}

#ifdef _INNERC_STORE_TREES
ATermAppl RewriterCompilingInnermost::create_tree(ATermList rules, int opid, int arity)
#else
static ATermAppl create_tree(ATermList rules, int opid, int arity)
#endif
	// Create a match tree for OpId int2term[opid] and update the value of
	// *max_vars accordingly.
	//
	// Pre:  rules is a list of rewrite rules for int2term[opid] in the
	//       INNER internal format
	//       opid is a valid entry in int2term
	//       max_vars is a valid pointer to an integer
	// Post: *max_vars is the maximum of the original *max_vars value and
	//       the number of variables in the result tree
	// Ret:  A match tree for int2term[opid]
{
//gsfprintf(stderr,"%P (%i)\n",int2term[opid],opid);
	// Create sequences representing the trees for each rewrite rule and
	// store the total number of variables used in these sequences.
	// (The total number of variables in all sequences should be an upper
	// bound for the number of variable in the final tree.)
	ATermList rule_seqs = ATmakeList0();
	int total_rule_vars = 0;
	for (; !ATisEmpty(rules); rules=ATgetNext(rules))
	{
		if ( ATgetLength(ATelementAt((ATermList) ATgetFirst(rules),2)) <= arity )
		{
			rule_seqs = ATinsert(rule_seqs, (ATerm) create_sequence((ATermList) ATgetFirst(rules),&total_rule_vars));
		}
	}

	// Generate initial parameters for built_tree
	build_pars init_pars;
	ATermAppl r = NULL;
	ATermList readies = ATmakeList0();

	initialise_build_pars(&init_pars);
	add_to_build_pars(&init_pars,rule_seqs,&r,&readies);

	ATermAppl tree;
	if ( r == NULL )
	{
		DECL_A(a,int,total_rule_vars);
		treevars_usedcnt = a;
//		treevars_usedcnt = (int *) malloc(total_rule_vars*sizeof(int));
		tree = build_tree(init_pars,0);
//		free(treevars_usedcnt);
		FREE_A(a);
		for (; !ATisEmpty(readies); readies=ATgetNext(readies))
		{
			tree = ATmakeAppl3(afunC,ATgetArgument(ATAgetFirst(readies),0),(ATerm) ATmakeAppl1(afunR,ATgetArgument(ATAgetFirst(readies),1)),(ATerm) tree);
		}
	} else {
		tree = ATmakeAppl1(afunR,ATgetArgument(r,0));
	}
	//ATprintf("tree: %t\n",tree);
	
#ifdef _INNERC_STORE_TREES
	char s[100],t[100];
	sprintf(s,"tree_%i_%s_%i",opid,ATgetName(ATgetAFun(ATAgetArgument(int2term[opid],0))),arity);
	sprintf(t,"tree_%i_%s_%i.dot",opid,ATgetName(ATgetAFun(ATAgetArgument(int2term[opid],0))),arity);
	tree2dot(tree,s,t);
#endif

	return tree;
}


void RewriterCompilingInnermost::calcTerm(FILE *f, ATerm t, int startarg)
{
  if ( ATisList(t) )
  {
    int arity = ATgetLength((ATermList) t)-1;
    ATermList l;
    bool b = false;
    bool v = false;

    if ( !ATisInt(ATgetFirst((ATermList) t)) )
    {
      if ( arity == 0 )
      {
        calcTerm(f,ATgetFirst((ATermList) t),0);
        return;
      }

      v = true;
      fprintf(f,"(isAppl(");
      calcTerm(f,ATgetFirst((ATermList) t),0);
      fprintf(f,")?varFunc%i(",arity);
      calcTerm(f,ATgetFirst((ATermList) t),0);
      l = ATgetNext((ATermList) t);
      int i = startarg;
      for (; !ATisEmpty(l); l=ATgetNext(l))
      {
        fprintf(f,",");
        if ( ATisAppl(ATgetFirst(l)) && gsIsNil(ATAgetFirst(l)) )
        {
          fprintf(f,"arg%i",i);
        } else {
          calcTerm(f,ATgetFirst(l),0);
        }
        i++;
      }
      fprintf(f,"):");
    }

    if ( ATisInt(ATgetFirst((ATermList) t)) && (l = innerc_eqns[ATgetInt((ATermInt) ATgetFirst((ATermList) t))]) != NULL )
    {
      for (; !ATisEmpty(l); l=ATgetNext(l))
      {
        if ( ATgetLength(ATLelementAt(ATLgetFirst(l),2)) <= arity )
        {
          b = true;
          break;
        }
      }
    }

    if ( b )
    {
      fprintf(f,"rewr_%i_%i(",ATgetInt((ATermInt) ATgetFirst((ATermList) t)),arity);
    } else {
      if ( arity == 0 )
      {
        fprintf(f,"(rewrAppl%i",ATgetInt((ATermInt) ATgetFirst((ATermList) t)));
      } else {
        if ( arity > 5 )
        {
          fprintf(f,"ATmakeAppl(appl%i,",arity);
        } else {
          fprintf(f,"ATmakeAppl%i(appl%i,",arity+1,arity);
        }
        if ( ATisInt(ATgetFirst((ATermList) t)) )
        {
          fprintf(f,"(ATerm) int2ATerm%i",ATgetInt((ATermInt) ATgetFirst((ATermList) t)));
        } else {
          fprintf(f,"(ATerm) ");
          calcTerm(f,ATgetFirst((ATermList) t),0);
        }
      }
    }
    l = ATgetNext((ATermList) t);
    bool c = !b;
    int i = startarg;
    for (; !ATisEmpty(l); l=ATgetNext(l))
    {
      if ( c )
      {
        fprintf(f,",");
      } else {
        c = true;
      }
      if ( !b )
      {
        fprintf(f,"(ATerm) ");
      }
      if ( ATisAppl(ATgetFirst(l)) && gsIsNil(ATAgetFirst(l)) )
      {
        fprintf(f,"arg%i",i);
      } else {
        calcTerm(f,ATgetFirst(l),0);
      }
      i++;
    }
    fprintf(f,")");
    
    if ( v )
    {
      fprintf(f,")");
    }
  } else if ( ATisInt(t) )
  {
    ATermList l;
    bool b = false;
    if ( (l = innerc_eqns[ATgetInt((ATermInt) t)]) != NULL )
    {
      for (; !ATisEmpty(l); l=ATgetNext(l))
      {
        if ( ATgetLength(ATLelementAt(ATLgetFirst(l),2)) == 0 )
        {
          b = true;
          break;
        }
      }
    }

    if ( b )
    {
      fprintf(f,"rewr_%i_0()",ATgetInt((ATermInt) t));
    } else {
//      fprintf(f,"ATmakeAppl1(appl0,int2ATerm%i)",ATgetInt((ATermInt) t));
      fprintf(f,"rewrAppl%i",ATgetInt((ATermInt) t));
    }
  } else {
    fprintf(f,"%s",ATgetName(ATgetAFun(ATAgetArgument((ATermAppl) t,0)))+1);
    //fprintf(f,"var_%s_%x",ATgetName(ATgetAFun(ATAgetArgument((ATermAppl) t,0))),ATXgetArgument((ATermAppl) t,1));
  }
}

static ATerm add_args(ATerm a, int num)
{
  if ( num == 0 )
  {
    return a;
  } else {
    ATermList l;

    if ( ATisList(a) )
    {
      l = (ATermList) a;
    } else {
      l = ATmakeList1(a);
    }

    while ( num > 0 )
    {
      l = ATappend(l,(ATerm) gsMakeNil());
      num--;
    }
    return (ATerm) l;
  }
}

static int get_startarg(ATerm a, int n)
{
  if ( ATisList(a) )
  {
    return n-ATgetLength((ATermList) a)+1;
  } else {
    return n;
  }
}


static int *i_t_st = NULL;
static int i_t_st_s = 0;
static int i_t_st_p = 0;
static void reset_st()
{
	i_t_st_p = 0;
}
static void push_st(int i)
{
	if ( i_t_st_s <= i_t_st_p )
	{
		if ( i_t_st_s == 0 )
		{
			i_t_st_s = 16;
		} else {
			i_t_st_s = i_t_st_s*2;
		}
		i_t_st = (int *) realloc(i_t_st,i_t_st_s*sizeof(int));
	}
	i_t_st[i_t_st_p] = i;
	i_t_st_p++;
}
static int pop_st()
{
	if ( i_t_st_p == 0 )
	{
		return 0;
	} else {
		i_t_st_p--;
		return i_t_st[i_t_st_p];
	}
}
static int peekn_st(int n)
{
	if ( i_t_st_p <= n )
	{
		return 0;
	} else {
		return i_t_st[i_t_st_p-n-1];
	}
}

//#define IT_DEBUG
#define IT_DEBUG_INLINE
#ifdef IT_DEBUG_INLINE
#define IT_DEBUG_FILE f,"//" 
#else
#define IT_DEBUG_FILE stderr,
#endif
void RewriterCompilingInnermost::implement_tree_aux(FILE *f, ATermAppl tree, int cur_arg, int parent, int level, int cnt, int d, int arity)
	// Print code representing tree to f.
	//
	// cur_arg   Indices refering to the variable that contains the current
	// parent    term. For level 0 this means arg<cur_arg>, for level 1 it
	//           means ATgetArgument(arg<parent>,<cur_arg) and for higher
	//           levels it means ATgetArgument(t<parent>,<cur_arg>)
	//
	// parent    Index of cur_arg in the previous level
	//
	// level     Indicates the how deep we are in the term (e.g. in
	//           f(.g(x),y) . indicates level 0 and in f(g(.x),y) level 1
	//
	// cnt       Counter indicating the number of variables t<i> (0<=i<cnt)
	//           used so far (in the current scope)
	//
	// d         Indicates the current scope depth in the code (i.e. new
	//           lines need to use at least 2*d spaces for indent)
	//
	// arity     Arity of the head symbol of the expression where are
	//           matching (for construction of return values)
{
#ifdef IT_DEBUG
fprintf(IT_DEBUG_FILE "implement_tree_aux: cur_arg=%i, parent=%i, level=%i, cnt=%i\n",cur_arg,parent,level,cnt);
#endif
	if ( isS(tree) )
	{
#ifdef IT_DEBUG
gsfprintf(IT_DEBUG_FILE "S(%P)\n",ATgetArgument(tree,0));
#endif
		if ( level == 0 )
		{
			fprintf(f,"%sATermAppl %s = arg%i; // S\n",whitespace(d*2),ATgetName(ATgetAFun(ATAgetArgument(ATAgetArgument(tree,0),0)))+1,cur_arg);
		} else {
			fprintf(f,"%sATermAppl %s = (ATermAppl) ATgetArgument(%s%i,%i); // S\n",whitespace(d*2),ATgetName(ATgetAFun(ATAgetArgument(ATAgetArgument(tree,0),0)))+1,(level==1)?"arg":"t",parent,cur_arg);
			//fprintf(f,"%sATermAppl %s = t%i; // S\n",whitespace(d*2),ATgetName(ATgetAFun(ATAgetArgument(ATAgetArgument(tree,0),0)))+1,cur_arg);
		}
		implement_tree_aux(f,ATAgetArgument(tree,1),cur_arg,parent,level,cnt,d,arity);
		return;
	} else if ( isM(tree) )
	{
#ifdef IT_DEBUG
gsfprintf(IT_DEBUG_FILE "M(%P)\n",ATgetArgument(tree,0));
#endif
		if ( level == 0 )
		{
			fprintf(f,"%sif ( ATisEqual(%s,arg%i) ) // M\n"
				  "%s{\n",
				whitespace(d*2),ATgetName(ATgetAFun(ATAgetArgument(ATAgetArgument(tree,0),0)))+1,cur_arg,
				whitespace(d*2)
				);
		} else {
			fprintf(f,"%sif ( ATisEqual(%s,ATgetArgument(%s%i,%i)) ) // M\n"
			//fprintf(f,"%sif ( ATisEqual(%s,t%i) ) // M\n"
				  "%s{\n",
				whitespace(d*2),ATgetName(ATgetAFun(ATAgetArgument(ATAgetArgument(tree,0),0)))+1,(level==1)?"arg":"t",parent,cur_arg,
			//	whitespace(d*2),ATgetName(ATgetAFun(ATAgetArgument(ATAgetArgument(tree,0),0)))+1,cur_arg,
				whitespace(d*2)
				);
		}
		implement_tree_aux(f,ATAgetArgument(tree,1),cur_arg,parent,level,cnt,d+1,arity);
		fprintf(f,"%s} else {\n",whitespace(d*2));
		implement_tree_aux(f,ATAgetArgument(tree,2),cur_arg,parent,level,cnt,d+1,arity);
		fprintf(f,"%s}\n",whitespace(d*2));
		return;
	} else if ( isF(tree) )
	{
#ifdef IT_DEBUG
gsfprintf(IT_DEBUG_FILE "F(%P)\n",int2term[ATgetInt((ATermInt) ATgetArgument(tree,0))]);
#endif
		if ( level == 0 )
		{
			fprintf(f,"%sif ( isAppl(arg%i) && ATisEqual(ATgetArgument(arg%i,0),int2ATerm%i) ) // F\n"
				  "%s{\n",
				whitespace(d*2),cur_arg,cur_arg,ATgetInt((ATermInt) ATgetArgument(tree,0)),
				whitespace(d*2)
				);
		} else {
			fprintf(f,"%sif ( isAppl(ATgetArgument(%s%i,%i)) && ATisEqual(ATgetArgument((ATermAppl) ATgetArgument(%s%i,%i),0),int2ATerm%i) ) // F\n"
			//fprintf(f,"%sif ( isAppl(t%i) && ATisEqual(ATgetArgument(t%i,0),int2ATerm%i) ) // F\n"
				  "%s{\n"
				  "%s  ATermAppl t%i = (ATermAppl) ATgetArgument(%s%i,%i);\n",
			//	  "%s  ATermAppl t%i = (ATermAppl) ATgetArgument(t%i,1);\n",
				whitespace(d*2),(level==1)?"arg":"t",parent,cur_arg,(level==1)?"arg":"t",parent,cur_arg,ATgetInt((ATermInt) ATgetArgument(tree,0)),
			//	whitespace(d*2),cur_arg,cur_arg,ATgetInt((ATermInt) ATgetArgument(tree,0)),
				whitespace(d*2),
				whitespace(d*2),cnt,(level==1)?"arg":"t",parent,cur_arg
			//	whitespace(d*2),cnt,cur_arg
				);
		}
		push_st(cur_arg);
		push_st(parent);
		implement_tree_aux(f,ATAgetArgument(tree,1),1,(level==0)?cur_arg:cnt,level+1,cnt+1,d+1,arity);
		pop_st();
		pop_st();
		fprintf(f,"%s} else {\n",whitespace(d*2));
		implement_tree_aux(f,ATAgetArgument(tree,2),cur_arg,parent,level,cnt,d+1,arity);
		fprintf(f,"%s}\n",whitespace(d*2));
		return;
	} else if ( isD(tree) )
	{
#ifdef IT_DEBUG
gsfprintf(IT_DEBUG_FILE "D\n");
#endif
		int i = pop_st();
		int j = pop_st();
		implement_tree_aux(f,ATAgetArgument(tree,0),j,i,level-1,cnt,d,arity);
		push_st(j);
		push_st(i);
		return;
	} else if ( isN(tree) )
	{
#ifdef IT_DEBUG
gsfprintf(IT_DEBUG_FILE "N\n");
#endif
		/*if ( level == 0 )
		{
			fprintf(f,"%scur = arg%i;\n",cur_arg+1);
		} else {
			fprintf(f,"%scur = (ATermAppl) ATgetArgument(%s%i,%i);\n",(level==1)?"arg":"t",parent,cur_arg+1);
		}*/
		implement_tree_aux(f,ATAgetArgument(tree,0),cur_arg+1,parent,level,cnt,d,arity);
		return;
	} else if ( isC(tree) )
	{
#ifdef IT_DEBUG
gsfprintf(IT_DEBUG_FILE "C\n");
#endif
		fprintf(f,"%sif ( ATisEqual(",whitespace(d*2));
		calcTerm(f,ATgetArgument(tree,0),0);
		fprintf(f,",rewrAppl%i) ) // C\n"
			  "%s{\n",
			true_num,
			whitespace(d*2)
			);
		implement_tree_aux(f,ATAgetArgument(tree,1),cur_arg,parent,level,cnt,d+1,arity);
		fprintf(f,"%s} else {\n",whitespace(d*2));
		implement_tree_aux(f,ATAgetArgument(tree,2),cur_arg,parent,level,cnt,d+1,arity);
		fprintf(f,"%s}\n",whitespace(d*2));
		return;
	} else if ( isR(tree) )
	{
#ifdef IT_DEBUG
gsfprintf(IT_DEBUG_FILE "R\n");
#endif
		fprintf(f,"%sreturn ",whitespace(d*2));
		if ( level > 0 )
		{
			//cur_arg = peekn_st(level);
			cur_arg = peekn_st(2*level-1);
		}
#ifdef IT_DEBUG
gsfprintf(IT_DEBUG_FILE "arity=%i, cur_arg=%i\n",arity,cur_arg);
#endif
		calcTerm(f,add_args(ATgetArgument(tree,0),arity-cur_arg-1),get_startarg(ATgetArgument(tree,0),cur_arg+1));
		fprintf(f,";\n");
		return;
	} else {
#ifdef IT_DEBUG
gsfprintf(IT_DEBUG_FILE "X\n");
#endif
		return;
	}
}

void RewriterCompilingInnermost::implement_tree(FILE *f, ATermAppl tree, int arity, int d, int opid)
{
#ifdef IT_DEBUG
gsfprintf(IT_DEBUG_FILE "implement_tree %P (%i)\n",int2term[opid],opid);
#endif
	int l = 0;

	while ( isC(tree) )
	{
		fprintf(f,"%sif ( ATisEqual(",whitespace(d*2));
		calcTerm(f,ATgetArgument(tree,0),0);
		fprintf(f,",rewrAppl%i) ) // C\n"
			  "%s{\n"
			  "%sreturn ",
			true_num,
			whitespace(d*2),
			whitespace(d*2)
			);
		assert(isR(ATAgetArgument(tree,1)));
		calcTerm(f,add_args(ATgetArgument(ATAgetArgument(tree,1),0),arity),get_startarg(ATgetArgument(ATAgetArgument(tree,1),0),0));
		fprintf(f,";\n"
			  "%s} else {\n",
			whitespace(d*2)
			);
		tree = ATAgetArgument(tree,2);
		d++;
		l++;
	}
	if ( isR(tree) )
	{
		fprintf(f,"%sreturn ",whitespace(d*2));
		calcTerm(f,add_args(ATgetArgument(tree,0),arity),get_startarg(ATgetArgument(tree,0),0));
		fprintf(f,";\n");
	} else {
		reset_st();
		implement_tree_aux(f,tree,0,0,0,0,d,arity);
	}
	while ( l > 0 )
	{
		d--;
		fprintf(f,"%s}\n",whitespace(d*2));
		l--;
	}
}

static int getArity(ATermAppl op)
{
  ATermAppl sort = ATAgetArgument(op,1);
  int arity = 0;

  while ( gsIsSortArrow(sort) )
  {
    sort = ATAgetArgument(sort,1);
    arity++;
  }

  return arity;
}

void RewriterCompilingInnermost::CompileRewriteSystem(ATermAppl DataEqnSpec)
{
  ATermList l,n;
  ATermTable tmp_eqns;
  ATermInt i;
  int j;
  FILE *f;
  char *s,*t;
  void *h;        


  tmp_eqns = ATtableCreate(100,75); // XXX would be nice to know the number op OpIds
  term2int = ATtableCreate(100,75);

  num_opids = 0;

  true_num = ATgetInt((ATermInt) OpId2Int(gsMakeDataExprTrue(),true));

/*&  l = opid_eqns;
//  gsfprintf(stderr,"OPIDEQNS %T\n\n",l);
  for (; !ATisEmpty(l); l=ATgetNext(l))
  {
    // XXX only adds the last rule where lhs is an opid; this might go "wrong" if 
    // this rule is removed later
    ATtablePut(
      tmp_eqns,
      OpId2Int(ATAgetArgument(ATAgetFirst(l),2),true),
      (ATerm) ATmakeList1((ATerm) 
                ATmakeList4(
                  (ATerm) ATmakeList0(),
                  toInner(ATAgetArgument(ATAgetFirst(l),1),true),   // condition.
                  (ATerm) ATmakeList0(),
                  toInner(ATAgetArgument(ATAgetFirst(l),3),true)))); // rhs.
  }

  l = dataappl_eqns;*/
//  gsfprintf(stderr,"DATAAPPL_EQNS %T\n\n",l);
  l = ATLgetArgument(DataEqnSpec,0);
  for (; !ATisEmpty(l); l=ATgetNext(l))
  {
    ATerm u = toInner(ATAgetArgument(ATAgetFirst(l),2),true);
    ATerm head;
    ATermList args;

    if ( ATisInt(u) )
    {
	    head = u;
	    args = ATmakeList0();
    } else {
	    head = ATgetFirst((ATermList) u);
	    args = ATgetNext((ATermList) u);
    }
    if ( (n = (ATermList) ATtableGet(tmp_eqns,head)) == NULL )
    {
      n = ATempty;
    }
    n = ATinsert(n,
		(ATerm) ATmakeList4(
			ATgetArgument(ATAgetFirst(l),0),
			toInner(ATAgetArgument(ATAgetFirst(l),1),true),
			(ATerm) args,
			toInner(ATAgetArgument(ATAgetFirst(l),3),true)));
    ATtablePut(tmp_eqns,head,(ATerm) n);
  }

  int2term = (ATermAppl *) malloc(num_opids*sizeof(ATermAppl));
  memset(int2term,0,num_opids*sizeof(ATermAppl));
  ATprotectArray((ATerm *) int2term,num_opids);
  innerc_eqns = (ATermList *) malloc(num_opids*sizeof(ATermList));
  memset(innerc_eqns,0,num_opids*sizeof(ATermList));
  ATprotectArray((ATerm *) innerc_eqns,num_opids);

  l = ATtableKeys(term2int);
//  gsfprintf(stderr,"TERM2INT %T\n",l);
  for (; !ATisEmpty(l); l=ATgetNext(l))
  {
    i = (ATermInt) ATtableGet(term2int,ATgetFirst(l));
    int2term[ATgetInt(i)] = ATAgetFirst(l);
    innerc_eqns[ATgetInt(i)] = (ATermList) ATtableGet(tmp_eqns,(ATerm) i);
  }

  ATtableDestroy(tmp_eqns);

  s = (char *) malloc(20);
  sprintf(s,"innerc_%i",getpid());
  t = (char *) malloc(100+strlen(INNERC_LDFLAGS)+strlen(INNERC_CFLAGS)+strlen(INNERC_CPPFLAGS));

  sprintf(t,"%s.c",s);
  file_c = strdup(t);
  sprintf(t,"%s.o",s);
  file_o = strdup(t);
  sprintf(t,"%s.so",s);
  file_so = strdup(t);

  f = fopen(file_c,"wb");
  if ( f == NULL )
  {
	  perror("fopen");
	  exit(1);
  }

  //
  //  Print includes
  //
  fprintf(f,  "#include <stdlib.h>\n"
      "#include <string.h>\n"
      "#include \"aterm2.h\"\n"
      "#include \"assert.h\"\n"
//      "#include \"libstruct.h\"\n"
//      "#include \"liblowlevel.h\"\n"
//      "#include \"gssubstitute.h\"\n"
      "#ifndef ATprotectAppl\n"
      "extern void ATprotectAppl(ATermAppl *a);\n"
      "#endif\n"
      "\n"
      "ATermAppl rewrite(ATermAppl);\n"
      "\n"
         );

  //
  // Forward declarations of rewr_* functions
  //
  int max_arity = 0;
  for (j=0;j < num_opids;j++)
  {
    /* if ( innerc_eqns[j] != NULL )
    { */
      int arity = getArity(int2term[j]);
      if ( arity > max_arity )
      {
        max_arity = arity;
      }

  /* Declare the function that gets function j in normal form */
  fprintf(f,  "static ATermAppl rewr_%i_nnf(ATermAppl);\n",j);

      for (int a=0; a<=arity; a++)
      {
    /*  
        l = innerc_eqns[j];
        for (; !ATisEmpty(l); l=ATgetNext(l))
        {
          if ( a == ATgetLength(ATLelementAt(ATLgetFirst(l),2)) )
          {
            b = true;
            break;
          }
        }
        if ( b ) */
        {
  fprintf(f,  "static ATermAppl rewr_%i_%i(",j,a);
          for (int i=0; i<a; i++)
          {
  fprintf(f,  (i==0)?"ATermAppl arg%i":", ATermAppl arg%i",i);
          }
  fprintf(f,  ");\n");

        }
      }
    /* } */
  }
  fprintf(f,  "\n\n");

  //
  // Print defs
  //
  fprintf(f,  
      "#define ATisInt(x) (ATgetType(x) == AT_INT)\n"
      "#define isAppl(x) (ATgetAFun(x) != varAFun)\n"
      "\n"
         );
  for (int i=0; i < num_opids; i++)
  {
  fprintf(f,  "static ATerm int2ATerm%i;\n",i);
  fprintf(f,  "static ATermAppl rewrAppl%i;\n",i);
  }
  fprintf(f,  "\n"
      "static AFun *apples;\n"
      "static AFun varAFun;\n"
      "static AFun dataapplAFun;\n"
      "static AFun opidAFun;\n"
         );
  for (int i=0; i<=max_arity; i++)
  {
  fprintf(f,      "static AFun appl%i;\n",i);
  fprintf(f,  "typedef ATermAppl (*ftype%i)(",i);
    for (int j=0; j<i; j++)
    {
      if ( j == 0 )
      {
  fprintf(f,              "ATermAppl");
      } else {
  fprintf(f,              ",ATermAppl");
      }
    }
  fprintf(f,              ");\n");
  fprintf(f,  "\n"
              "ftype%i *int2func%i;\n",i,i);
  }
  fprintf(f,  "ftype1 *int2func;\n");

  //
  // Implement substitution functions
  //
  fprintf(f,  "\n"
	      "\n"
	      "static ATerm *substs = NULL;\n"
	      "static long substs_size = 0;\n"
	      "\n"
	      "void set_subst(ATermAppl Var, ATerm Expr)\n"
	      "{\n"
	      "  long n = ATgetAFun(ATgetArgument(Var,0));\n"
	      "\n"
	      "  if ( n >= substs_size )\n"
	      "  {\n"
	      "    long newsize;\n"
	      "\n"
	      "    if ( n >= 2*substs_size )\n"
	      "    {\n"
	      "      if ( n < 1024 )\n"
	      "      {\n"
	      "        newsize = 1024;\n"
	      "      } else {\n"
	      "        newsize = n+1;\n"
	      "      }\n"
	      "    } else {\n"
	      "      newsize = 2*substs_size;\n"
	      "    }\n"
	      "\n"
	      "    if ( substs_size > 0 )\n"
	      "    {\n"
	      "      ATunprotectArray(substs);\n"
	      "    }\n"
	      "    substs = (ATerm *) realloc(substs,newsize*sizeof(ATerm));\n"
	      "    \n"
	      "    if ( substs == NULL )\n"
	      "    {\n"
	      "      fprintf(stderr,\"Failed to increase the size of a substitution array to %%ld\\n\",newsize);\n"
	      "      exit(1);\n"
	      "    }\n"
	      "\n"
	      "    for (long i=substs_size; i<newsize; i++)\n"
	      "    {\n"
	      "      substs[i]=NULL;\n"
	      "    }\n"
	      "\n"
	      "    ATprotectArray(substs,newsize);\n"
	      "    substs_size = newsize;\n"
	      "  }\n"
	      "\n"
	      "  substs[n] = Expr;\n"
	      "}\n"
	      "\n"
	      "ATerm get_subst(ATermAppl Var)\n"
	      "{\n"
	      "  long n = ATgetAFun(ATgetArgument(Var,0));\n"
	      "\n"
	      "  if ( n >= substs_size )\n"
	      "  {\n"
	      "    return (ATerm) Var;\n"
	      "  }\n"
	      "  \n"
	      "  ATerm r = substs[n];\n"
	      "  \n"
	      "  if ( r == NULL )\n"
	      "  {\n"
	      "    return (ATerm) Var;\n"
	      "  }\n"
	      "  \n"
	      "  return r;\n"
	      "}\n"
	      "\n"
	      "void clear_subst(ATermAppl Var)\n"
	      "{\n"
	      "  long n = ATgetAFun(ATgetArgument(Var,0));\n"
	      "\n"
	      "  substs[n] = NULL;\n"
	      "}\n"
	      "\n"
	      "void clear_substs()\n"
	      "{\n"
	      "  for (long i=0; i<substs_size; i++)\n"
	      "  {\n"
	      "    substs[i] = NULL;\n"
	      "  }\n"
	      "}\n"
	      "\n"
	      "\n"
	      );


  //
  // Implement auxiliary functions
  //
  fprintf(f,  "static int num_apples = 0;\n"
      "#define GET_APPL_INC 5\n"
      "static AFun getAppl(int arity)\n"
      "{\n"
      "  int old_num = num_apples;\n"
      "\n"
      "  if ( apples == NULL )\n"
      "  {\n"
      "    apples = (AFun *) malloc(0*sizeof(AFun));\n"
      "  }\n"
      "\n"
      "  while ( arity >= num_apples )\n"
      "  {\n"
      "    num_apples += GET_APPL_INC;\n"
      "  }\n"
      "  if ( old_num < num_apples )\n"
      "  {\n"
      "    apples = realloc(apples,num_apples*sizeof(AFun));\n"
      "    for (int i=old_num; i<num_apples; i++)\n"
      "    {\n"
      "      char c[10];\n"
      "      sprintf(c,\"appl#%%i\",i+1);\n"
      "      apples[i] = ATmakeAFun(c,i+1,ATfalse);\n"
      "      ATprotectAFun(apples[i]);\n"
      "    }\n"
      "  }\n"
      "\n"
      "  return apples[arity];\n"
      "}\n"
      "\n"
         );
        
  for (int i=1; i<=max_arity; i++)
  { fprintf(f, 
      "static ATermAppl varFunc%i(ATermAppl a",i);
    for (int j=0; j<i; j++)
    { fprintf(f, ", ATermAppl arg%i",j); 
    }
    fprintf(f, ")\n" 
      "{\n" );
    fprintf(f,
//      "  ATprintf(\"%%t\\n\",a);\n"
      "  int arity = ATgetArity(ATgetAFun(a));\n"
      "  if ( arity == 1 )\n"
      "  {\n"
      "    if ( ATisInt(ATgetArgument(a,0)) && \n"
      "       (ATgetInt((ATermInt) ATgetArgument(a,0)) < %i) && \n"
      "       (int2func%i[ATgetInt((ATermInt) ATgetArgument(a,0))] != NULL) )\n"
      "    {\n"
      "       return int2func%i[ATgetInt((ATermInt) ATgetArgument(a,0))](",
         num_opids,i,i);
    for (int j=0; j<i; j++)
    {
      if ( j == 0 )
      { fprintf(f,"(ATermAppl) arg%i",j);
      } 
      else 
      { fprintf(f, ", (ATermAppl) arg%i",j);
      }
    }
    fprintf(f,");\n"
      "    }\n"
      "    else\n"
      "    {\n"
      "      return ATmakeAppl(appl%i,ATgetArgument(a,0)", i);


    for (int j=0; j<i; j++)
    { fprintf(f, ",arg%i",j);
    }
    fprintf(f, ");\n"
      "    }\n"
      "  } else {\n"
      "    ATerm args[arity+%i];\n"
      "\n"
      "    for (int i=0; i<arity; i++)\n"
      "    {\n"
      "      args[i] = ATgetArgument(a,i);\n"
      "    }\n",i);
    for (int j=0; j<i; j++)
    {        
      fprintf(f,
      "    args[arity+%i] = (ATerm) arg%i;\n",j,j);
    }
    fprintf(f,
      "    if ( ATisInt(args[0]) && (ATgetInt((ATermInt) args[0]) < %i) )\n"
      "    {\n"
//                      "  gsprintf(\"switch %%i\\n\",i);\n"
      "      switch ( arity+%i-1 )\n"
      "      {\n",num_opids,i);
    for (int j=i; j<=max_arity; j++)
    { fprintf(f,
      "        case %i:\n"
      "          if ( int2func%i[ATgetInt((ATermInt) args[0])] != NULL )\n"
      "          {\n"
      "            return int2func%i[ATgetInt((ATermInt) args[0])](",j,j,j);
      for (int k=0; k<j; k++)
      {
        if ( k == 0 )
        {  
          fprintf(f,"(ATermAppl) args[%i]",k+1);
        }   
        else 
        {        
          fprintf(f,", (ATermAppl) args[%i]",k+1);
        } 
      }
      fprintf(f,");\n"
        "          }\n"
        "          break;\n");
    }
    fprintf(f,
      "        default:\n"
      "          break;\n"
      "      }\n"
      "    }\n"
      "\n"
      "    return ATmakeApplArray(getAppl(arity+%i-1),args);\n"
      "  }\n"
      "}\n"
      "\n",i);
  }

  //
  // Implement the equations of every operation.
  //
  for (j=0;j < num_opids;j++)
  {
    /* if ( innerc_eqns[j] != NULL ) */

    int arity = getArity(int2term[j]);

    gsfprintf(f,  "// %T\n",int2term[j]);
    fprintf(f,  "static ATermAppl rewr_%i_nnf(ATermAppl t)\n"
                "{\n",j);
    if (arity>0)
    { fprintf(f,  "  int arity=ATgetArity(ATgetAFun(t))-1;\n");
    }
    for (int a=arity; a>=0; a--)
    { 
      if (a>0) 
      { fprintf(f,  "  if (arity==%i)\n"
                  "  { ",a);
      }
      else
      { fprintf(f,  "  ");
      }
      fprintf(f,  "return rewr_%i_%i(",j,a);
      for(int i=0; i<a; i++)
      { if (i>0) 
        { fprintf(f,",");
        }
        fprintf(f,  "rewrite((ATermAppl)ATgetArgument(t,%i))",i+1);
      }
      fprintf(f,  ");\n");
      if (a>0) 
      { fprintf(f,  "  }\n");
      }
    }
    fprintf(f,  "}\n"
                "\n");
            
    for (int a=0; a<=arity; a++)
    {
      fprintf(f,  "static ATermAppl rewr_%i_%i(",j,a);
      for (int i=0; i<a; i++)
      {
        fprintf(f,  (i==0)?"ATermAppl arg%i":", ATermAppl arg%i",i);
      }
      fprintf(f,  ")\n"
                  "{\n"
             );

/*      fprintf(f,  "  ATfprintf(stderr,\"rewr_%i_%i(",j,a);
      for (int i=0; i<a; i++)
      {
        fprintf(f,  (i==0)?"%%t":", %%t");
      }
      fprintf(f,  ")\\n\"");
      for (int i=0; i<a; i++)
      {
        fprintf(f,  ", arg%i",i);
      }
      fprintf(f,  ");\n");*/
 

      // Implement tree
      if ( innerc_eqns[j] != NULL )
      {
	    implement_tree(f,create_tree(innerc_eqns[j],j,a),a,1,j);
      }



          //
          // Finish up function
          //
          if ( a == 0 )
          {
  fprintf(f,  "  return (rewrAppl%i",
      j
         );
          } else {
            if ( a > 5 )
            {
  fprintf(f,  "  return ATmakeAppl(appl%i,(ATerm) int2ATerm%i",
      a,j
         );
            } else {
  fprintf(f,  "  return ATmakeAppl%i(appl%i,(ATerm) int2ATerm%i",
      a+1,a,j
         );
            }
          }
          for (int i=0; i<a; i++)
          {
  fprintf(f,                 ",(ATerm) arg%i",i);
          }
  fprintf(f,                 ");\n"
      "}\n"
         );
        }
  fprintf(f,  "\n");
  }

  fprintf(f,  "void rewrite_init()\n"
      "{\n"
      "  varAFun = ATmakeAFun(\"DataVarId\", 2, ATfalse);\n"
      "  ATprotectAFun(varAFun);\n"
      "  dataapplAFun = ATmakeAFun(\"DataAppl\", 2, ATfalse);\n"
      "  ATprotectAFun(dataapplAFun);\n"
      "  opidAFun = ATmakeAFun(\"OpId\", 2, ATfalse);\n"
      "  ATprotectAFun(opidAFun);\n"
      "\n"
      "  apples = NULL;\n"
      "  getAppl(%i);\n",
      max_arity
         );
  for (int i=0; i<=max_arity; i++)
  {
  fprintf(f,  "  appl%i = apples[%i];\n",i,i);
  }
  fprintf(f,  "\n");
  for (int i=0; i < num_opids; i++)
  {
  fprintf(f,  "  int2ATerm%i = (ATerm) ATmakeInt(%i);\n",i,i);
  fprintf(f,  "  ATprotect(&int2ATerm%i);\n",i);
  fprintf(f,  "  rewrAppl%i = ATmakeAppl(appl0,int2ATerm%i);\n",i,i);
  fprintf(f,  "  ATprotectAppl(&rewrAppl%i);\n",i);
  }
/*      "  int2ATerm = (ATerm *) malloc(%i*sizeof(ATerm));\n"
//      "  memset(int2ATerm,0,%i*sizeof(ATerm));\n"
      "  ATprotectArray(int2ATerm,%i);\n"
      "  for (i=0; i<%i; i++)\n"
      "  {\n"
      "    int2ATerm[i] = (ATerm) ATmakeInt(i);\n"
      "  }\n"
      "\n",
      num_opids,num_opids,num_opids//,num_opids
         );*/

  /* put the functions that start the rewriting in the array int2func */
  fprintf(f,  "\n");
  fprintf(f,  "\n");
  fprintf(f,  "  int2func = (ftype1 *) malloc(%i*sizeof(ftype1));\n",num_opids);
  for (j=0;j < num_opids;j++)
  { gsfprintf(f,  "  int2func[%i] = rewr_%i_nnf; // %T\n",j,j,int2term[j]);
  }
  fprintf(f,  "\n");
  for (int i=0;i<max_arity;i++)
  {
  fprintf(f,  "  int2func%i = (ftype%i *) malloc(%i*sizeof(ftype%i));\n",i,i,num_opids,i);
  for (j=0;j < num_opids;j++)
  { if ( i <= getArity(int2term[j]) )
  { gsfprintf(f,  "  int2func%i[%i] = rewr_%i_%i;\n",i,j,j,i);
  }
  }
  }
  fprintf(f,  "}\n"
      "\n"
      "ATermAppl rewrite(ATermAppl t)\n"
      "{\n"
      "  if ( isAppl(t) )\n"
      "  {\n"
      "    ATerm head = ATgetArgument(t,0);\n"
      "    if ( ATisInt(head) )\n"
      "    {\n"
      "      long function_index = ATgetInt((ATermInt)head);\n"
      "      if ( function_index < %i )\n"
      "      {\n"
      "        return int2func[function_index](t);\n"
      "      } else {\n"
      "        int arity = ATgetArity(ATgetAFun(t));\n"
      "        ATerm args[arity];\n"
      "        args[0] = head;\n"
      "        for (int i=1; i<arity; i++)\n"
      "        {\n"
      "          args[i] = (ATerm) rewrite((ATermAppl) ATgetArgument(t,i));\n"
      "        }\n"
      "        return ATmakeApplArray(ATgetAFun(t),args);\n"
      "      }\n"
      "    } else {\n"
      "      ATerm u = get_subst((ATermAppl) head);\n"
      "      long arity_t = ATgetArity(ATgetAFun(t));\n"
      "      ATerm newhead = ATgetArgument((ATermAppl) u,0);\n"
      "      long arity_h = ATgetArity(ATgetAFun((ATermAppl) u));\n"
      "      ATerm args[arity_h+arity_t-1];\n"
      "      args[0] = newhead;\n"
      "      long function_index;\n"
      "      if ( ATisInt(newhead) && ((function_index = ATgetInt((ATermInt) newhead)) < %i) )\n"
      "      {\n"
      "        for (int i=1; i<arity_h; i++)\n"
      "        {\n"
      "          args[i] = ATgetArgument((ATermAppl) u,i);\n"
      "        }\n"
      "        int k = arity_h;\n"
      "        for (int i=1; i<arity_t; i++,k++)\n"
      "        {\n"
      "          args[k] = ATgetArgument((ATermAppl) t,i);\n"
      "        }\n"
      "        return int2func[function_index](ATmakeApplArray(getAppl(arity_h+arity_t-1),args));\n"
      "      } else {\n"
      "        for (int i=1; i<arity_h; i++)\n"
      "        {\n"
      "          args[i] = (ATerm) rewrite((ATermAppl) ATgetArgument((ATermAppl) u,i));\n"
      "        }\n"
      "        int k = arity_h;\n"
      "        for (int i=1; i<arity_t; i++,k++)\n"
      "        {\n"
      "          args[k] = (ATerm) rewrite((ATermAppl) ATgetArgument((ATermAppl) t,i));\n"
      "        }\n"
      "        return ATmakeApplArray(getAppl(arity_h+arity_t-1),args);\n"
      "      }\n"
      "    }\n"
      "  } else {\n"
      "    ATermAppl r=(ATermAppl) get_subst(t);\n"
      "    return r;\n"
      "  }\n"
      "}\n",
      num_opids,
      num_opids
         );

  fclose(f);
  fprintf(stderr,"Compiling rewriter...");fflush(stderr);
#ifdef __APPLE__
// for this to work use: "setenv MACOSX_DEPLOYMENT_TARGET 10.3" to
// set the environment variable setenv MACOSX_DEPLOYMENT_TARGET to 10.3
  sprintf(t,"gcc -c %s %s %s.c",INNERC_CPPFLAGS,INNERC_CFLAGS,s);
#ifndef NDEBUG
  gsfprintf(stderr,"\n%s\n",t);
#endif
  system(t);
  sprintf(t,"gcc -bundle -undefined dynamic_lookup -o %s.so %s.o",s,s);
#ifndef NDEBUG
  gsfprintf(stderr,"%s\n",t);
#endif
  system(t);
#else
  sprintf(t,"gcc -c %s %s -Wno-unused -O3 -rdynamic %s.c",INNERC_CPPFLAGS,INNERC_CFLAGS,s);
  system(t);
  sprintf(t,"gcc %s -shared -o %s.so %s.o",INNERC_LDFLAGS,s,s);
  system(t);
#endif
  fprintf(stderr,"done.\n");

  sprintf(t,"./%s.so",s);
  if ( (h = dlopen(t,RTLD_NOW)) == NULL )
  {
    fprintf(stderr,"error opening dll\n%s\n",dlerror());
  }
  so_rewr_init = (void (*)()) dlsym(h,"rewrite_init");
  so_rewr = (ATermAppl (*)(ATermAppl)) dlsym(h,"rewrite");
  so_set_subst = (void (*)(ATermAppl, ATerm)) dlsym(h,"set_subst");
  so_get_subst = (ATerm (*)(ATermAppl)) dlsym(h,"get_subst");
  so_clear_subst = (void (*)(ATermAppl)) dlsym(h,"clear_subst");
  so_clear_substs = (void (*)()) dlsym(h,"clear_substs");

  so_rewr_init();

  free(t);
  free(s);
}

RewriterCompilingInnermost::RewriterCompilingInnermost(ATermAppl DataEqnSpec)
{
  initialise_common();
  CompileRewriteSystem(DataEqnSpec);
}

static void cleanup_file(char *f)
{
  if ( unlink(f) )
  {
	  fprintf(stderr,"unable to remove file %s: %s\n",f,strerror(errno));
  }
  free(f);
}

RewriterCompilingInnermost::~RewriterCompilingInnermost()
{
  finalise_common();
  cleanup_file(file_c);
  cleanup_file(file_o);
  cleanup_file(file_so);
}

ATermList RewriterCompilingInnermost::rewriteInternalList(ATermList l)
{
  if (l==ATempty)
  { return ATempty;
  }

  return ATinsertA(
           rewriteInternalList(ATgetNext(l)),
           so_rewr(ATAgetFirst(l)));
}

ATermAppl RewriterCompilingInnermost::rewrite(ATermAppl Term)
{
  return fromInner((ATerm) so_rewr((ATermAppl) toInnerc(toInner(Term,true))));
}

ATerm RewriterCompilingInnermost::rewriteInternal(ATerm Term)
{
  return (ATerm) so_rewr((ATermAppl) Term);
}

void RewriterCompilingInnermost::setSubstitution(ATermAppl Var, ATerm Expr)
{
  so_set_subst(Var,Expr);
}

ATerm RewriterCompilingInnermost::getSubstitution(ATermAppl Var)
{
  return so_get_subst(Var);
}

void RewriterCompilingInnermost::clearSubstitution(ATermAppl Var)
{
  so_clear_subst(Var);
}

void RewriterCompilingInnermost::clearSubstitutions()
{
  so_clear_substs();
}

#endif

RewriteStrategy RewriterCompilingInnermost::getStrategy()
{
	return GS_REWR_INNERC;
}
