// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file rewr_jittyc.cpp

#include "boost.hpp" // precompiled headers

#include "mcrl2/data/rewrite.h"

#ifdef MCRL2_JITTYC_AVAILABLE

#define USE_NEW_CALCTERM 1
#define EXTEND_NFS 1
#define F_DONT_TEST_ISAPPL 1
#define USE_VARAFUN_VALUE 1
#define USE_APPL_VALUE 1
#define USE_INT2ATERM_VALUE 1
#define USE_REWRAPPL_VALUE 1

#include "workarounds.h" // DECL_A

#define NAME "rewr_jittyc"

#include <utility>
#include <string>
#include <sstream>
#include <stdexcept>
#include <cstdio>
#include <cstdlib>
#include <stdint.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <dlfcn.h>
#include <cassert>
#include <aterm2.h>
#include "mcrl2/core/print.h"
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/data/data_specification.h"
#include "mcrl2/setup.h"
#include "mcrl2/data/detail/rewrite/jittyc.h"

using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace std;

#define JITTYC_COMPILE_COMMAND (CC " -c " CFLAGS " " SCFLAGS " " CPPFLAGS " " ATERM_CPPFLAGS " %s.c")
#define JITTYC_LINK_COMMAND (CC " " LDFLAGS " " SLDFLAGS " -o %s.so %s.o")

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
static AFun afunARtrue, afunARfalse, afunARand, afunARor, afunARvar;
static ATermAppl ar_true, ar_false;

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

// Maximal arity for which we generate functions for every combination of
// arguments that are in normal form or not
#define NF_MAX_ARITY 3  // currently this should be such that it is at most sizeof(unsigned int)*8

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

		afunARtrue = ATmakeAFun("@@true",0,ATfalse);
		ATprotectAFun(afunARtrue);
		afunARfalse = ATmakeAFun("@@false",0,ATfalse);
		ATprotectAFun(afunARfalse);
		afunARand = ATmakeAFun("@@and",2,ATfalse);
		ATprotectAFun(afunARand);
		afunARor = ATmakeAFun("@@or",2,ATfalse);
		ATprotectAFun(afunARor);
		afunARvar = ATmakeAFun("@@var",1,ATfalse);
		ATprotectAFun(afunARvar);
                ar_true = ATmakeAppl0(afunARtrue);
                ATprotectAppl(&ar_true);
                ar_false = ATmakeAppl0(afunARfalse);
                ATprotectAppl(&ar_false);
	}

	is_initialised++;
}

static void finalise_common()
{
	assert(is_initialised > 0 );
	is_initialised--;

	if ( is_initialised == 0 )
	{
                ATunprotectAppl(&ar_false);
                ATunprotectAppl(&ar_true);
		ATunprotectAFun(afunARvar);
		ATunprotectAFun(afunARor);
		ATunprotectAFun(afunARand);
		ATunprotectAFun(afunARfalse);
		ATunprotectAFun(afunARtrue);

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


#define DECL_NFS_A(x,y) DECL_A(x,unsigned int,y); clear_nfs_array(x,y)
#define FREE_NFS_A(x) FREE_A(x)

static void clear_nfs_array(nfs_array a, unsigned int arity)
{
  if ( arity == 0 )
    return;
  memset(a,0,((arity-1)/(sizeof(unsigned int)*8)+1)*sizeof(unsigned int));
}

static void fill_nfs_array(nfs_array a, unsigned int arity, bool val = true)
{
  unsigned int i = 0;
  while ( i*sizeof(unsigned int)*8 < arity )
  {
    if ( val )
    {
      a[i++] = ~((unsigned int) 0);
    } else {
      a[i++] = ((unsigned int) 0);
    }
  }
}

static unsigned int get_nfs_array_value(nfs_array a, unsigned int arity)
{
  assert(arity <= NF_MAX_ARITY);
  return (a[0] & ((1 << arity)-1));
}

static void set_nfs_array_value(nfs_array a, unsigned int arity, unsigned int val)
{
  assert(arity <= NF_MAX_ARITY || val == 0);
  a[0] = val;
}

#ifndef NDEBUG
static bool equal_nfs_array(nfs_array a, nfs_array b, unsigned int arity)
{
  unsigned int i = 0;
  while ( arity >= sizeof(unsigned int)*8 )
  {
    if ( a[i] != b[i] )
      return false;
    arity -= sizeof(unsigned int)*8;
    ++i;
  }
  return (a[i] & ((1 << arity)-1)) == (b[i] & ((1 << arity)-1));
}
#endif

static bool get_nfs_array(nfs_array a, unsigned int i)
{
  return a[i/(sizeof(unsigned int)*8)] & (((unsigned int) 1) << (i%(sizeof(unsigned int)*8)));
}

static void set_nfs_array(nfs_array a, unsigned int i, bool val = true)
{
  if ( val )
  {
    a[i/(sizeof(unsigned int)*8)] |= ((unsigned int) 1) << (i%(sizeof(unsigned int)*8));
  } else {
    a[i/(sizeof(unsigned int)*8)] &= ~(((unsigned int) 1) << (i%(sizeof(unsigned int)*8)));
  }
}

static bool is_clear_nfs_array(nfs_array a, unsigned int arity)
{
  unsigned int i = 0;
  while ( arity >= sizeof(unsigned int)*8 )
  {
    if ( a[i++] != ((unsigned int) 0) )
      return false;
    arity -= sizeof(unsigned int)*8;
  }
  return (a[i] & ((1 << arity)-1)) == 0;
}

static bool is_filled_nfs_array(nfs_array a, unsigned int arity)
{
  unsigned int i = 0;
  while ( arity >= sizeof(unsigned int)*8 )
  {
    if ( a[i++] != ~((unsigned int) 0) )
      return false;
    arity -= sizeof(unsigned int)*8;
  }
  return (a[i] & ((1 << arity)-1)) == (unsigned int) ((1 << arity)-1);
}


#define is_ar_true(x) (ATisEqual((x),ar_true))
#define is_ar_false(x) (ATisEqual((x),ar_false))
#define is_ar_and(x) (ATisEqualAFun(ATgetAFun(x),afunARand))
#define is_ar_or(x) (ATisEqualAFun(ATgetAFun(x),afunARor))
#define is_ar_var(x) (ATisEqualAFun(ATgetAFun(x),afunARvar))

static ATermAppl make_ar_true()
{
  return ar_true;
}

static ATermAppl make_ar_false()
{
  return ar_false;
}

static ATermAppl make_ar_and(ATermAppl x, ATermAppl y)
{
  if ( is_ar_true(x) )
  {
    return y;
  } else if ( is_ar_true(y) )
  {
    return x;
  } else if ( is_ar_false(x) || is_ar_false(y) )
  {
    return make_ar_false();
  }

  return ATmakeAppl2(afunARand,(ATerm) x,(ATerm) y);
}

static ATermAppl make_ar_or(ATermAppl x, ATermAppl y)
{
  if ( is_ar_false(x) )
  {
    return y;
  } else if ( is_ar_false(y) )
  {
    return x;
  } else if ( is_ar_true(x) || is_ar_true(y) )
  {
    return make_ar_true();
  }

  return ATmakeAppl2(afunARor,(ATerm) x,(ATerm) y);
}

static ATermAppl make_ar_var(int var)
{
  return ATmakeAppl1(afunARvar,(ATerm) ATmakeInt(var));
}

static int getArity(ATermAppl op)
{
  ATermAppl sort = ATAgetArgument(op,1);
  int arity = 0;

  while ( gsIsSortArrow(sort) )
  {
    ATermList sort_dom = ATLgetArgument(sort, 0);
    arity += ATgetLength(sort_dom);
    sort = ATAgetArgument(sort, 1);
  }

  return arity;
}

ATerm RewriterCompilingJitty::OpId2Int(ATermAppl Term, bool add_opids)
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
    int arity = getArity(Term);
    if ( arity > NF_MAX_ARITY )
	    arity = NF_MAX_ARITY;
      unsigned int num_aux = (1 << (arity+1)) - arity - 2; // 2^(arity+1) - arity - 2
//      printf("%i -> %u\n",ATgetInt(i),num_aux);
    if ( arity <= NF_MAX_ARITY )
    {
      num_opids += num_aux;
    }
  }

  return (ATerm) i;
}

ATerm RewriterCompilingJitty::toInner(ATermAppl Term, bool add_opids)
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

        if ( gsIsDataAppl(Term) )
        {
                for ( ATermList args = ATLgetArgument((ATermAppl) Term, 1) ; !ATisEmpty(args) ; args = ATgetNext(args))
                {
                        l = ATinsert(l,(ATerm) toInner((ATermAppl) ATgetFirst(args),add_opids));
                }

                l = ATreverse(l);

                ATerm arg0 = toInner(ATAgetArgument((ATermAppl) Term, 0), add_opids);
                if ( ATisList(arg0) )
                {
                        l = ATconcat((ATermList) arg0, l);
                }
                else
                {
                        l = ATinsert(l, arg0);
                }
        }

        return (ATerm) l;
}

ATermAppl RewriterCompilingJitty::fromInner(ATerm Term)
{
        ATermList l;
        ATermList list;
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
                gsErrorMsg("%s: invalid jitty format term (%T)\n",NAME,Term);
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

        if(gsIsOpId(a) || gsIsDataVarId(a))
        {
                ATermAppl sort = ATAgetArgument(a, 1);
                while(gsIsSortArrow(sort) && !ATisEmpty(l))
                {
                        ATermList sort_dom = ATLgetArgument(sort, 0);
                        list = ATmakeList0();
                        while (!ATisEmpty(sort_dom))
                        {
                                list = ATinsert(list, (ATerm) fromInner(ATgetFirst(l)));
                                sort_dom = ATgetNext(sort_dom);
                                l = ATgetNext(l);
                        }
                        list = ATreverse(list);
                        a = gsMakeDataAppl(a, list);
                        sort = ATAgetArgument(sort, 1);
                }
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

#ifdef USE_APPL_VALUE
static unsigned int num_apples = 0;
static AFun *apples = NULL;
static AFun get_appl_afun_value(unsigned int arity)
{
  if ( arity >= num_apples )
  {
    unsigned int old_num = num_apples;
    num_apples = arity+1;
    apples = (AFun *) realloc(apples,num_apples*sizeof(AFun));
    if ( apples == NULL )
    {
      gsErrorMsg("cannot allocate enough memory\n");
      exit(1);
    }
    char c[10];
    for (; old_num < num_apples; old_num++)
    {
      sprintf(c,"appl#%d",old_num+1);
      apples[old_num] = ATmakeAFun(c,old_num+1,ATfalse);
      ATprotectAFun(apples[old_num]);
    }
  }
  return apples[arity];
}
#endif

#ifdef USE_INT2ATERM_VALUE
static unsigned int num_int2aterms = 0;
static ATerm *int2aterms = NULL;
static ATerm get_int2aterm_value(int i)
{
  assert( i >= 0 );
  if ( ((unsigned int) i) >= num_int2aterms )
  {
    unsigned int old_num = num_int2aterms;
    num_int2aterms = i+1;
    if ( int2aterms != NULL )
    {
      ATunprotectArray(int2aterms);
    }
    int2aterms = (ATerm *) realloc(int2aterms,num_int2aterms*sizeof(ATerm));
    if ( int2aterms == NULL )
    {
      gsErrorMsg("cannot allocate enough memory\n");
      exit(1);
    }
    for (unsigned int j=old_num; j < num_int2aterms; j++)
    {
      int2aterms[j] = NULL;
    }
    ATprotectArray(int2aterms,num_int2aterms);
    for (; old_num < num_int2aterms; old_num++)
    {
      int2aterms[old_num] = (ATerm) ATmakeInt(old_num);
    }
  }
  return int2aterms[i];
}
static ATerm get_int2aterm_value(ATermInt i)
{
  return get_int2aterm_value(ATgetInt(i));
}
#endif

#ifdef USE_REWRAPPL_VALUE
static unsigned int num_rewrappls = 0;
static ATerm *rewrappls = NULL;
static ATerm get_rewrappl_value(int i)
{
  assert( i >= 0 );
  if ( ((unsigned int) i) >= num_rewrappls )
  {
    unsigned int old_num = num_rewrappls;
    num_rewrappls = i+1;
    if ( rewrappls != NULL )
    {
      ATunprotectArray(rewrappls);
    }
    rewrappls = (ATerm *) realloc(rewrappls,num_rewrappls*sizeof(ATerm));
    if ( rewrappls == NULL )
    {
      gsErrorMsg("cannot allocate enough memory\n");
      exit(1);
    }
    for (unsigned int j=old_num; j < num_rewrappls; j++)
    {
      rewrappls[j] = NULL;
    }
    ATprotectArray(rewrappls,num_rewrappls);
    for (; old_num < num_rewrappls; old_num++)
    {
      rewrappls[old_num] = (ATerm) ATmakeAppl(ATmakeAFun("appl#1",1,ATfalse),ATmakeInt(old_num));
    }
  }
  return rewrappls[i];
}
static ATerm get_rewrappl_value(ATermInt i)
{
  return get_rewrappl_value(ATgetInt(i));
}
#endif

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
      ATerror("%s: Do not deal with application terms correctly\n%t\n\n",NAME,Term);
    }
  }

  if ( ATisEmpty((ATermList) Term) )
  {
    ATerror("%s: invalid jitty format term (%t)\n",NAME,Term);
  }

  ATermList l=ATinsert(ATempty,ATgetFirst((ATermList)Term));
  for( ATermList l1=ATgetNext((ATermList)Term) ;
       l1!=ATempty ;
       l1=ATgetNext(l1))
  { l=ATinsert(l,toInnerc(ATgetFirst(l1)));
  }
  l=ATreverse(l);
  ATerm r=Apply((ATermList) l);
  return r;
}

ATerm RewriterCompilingJitty::toRewriteFormat(ATermAppl t)
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

    ATunprotectArray((ATerm *) jittyc_eqns);
    jittyc_eqns = (ATermList *) realloc(jittyc_eqns,num_opids*sizeof(ATermList));
    for (int i=old_opids; i<num_opids; i++)
    {
	    int2term[i] = NULL;
	    jittyc_eqns[i] = NULL;
    }
    ATprotectArray((ATerm *) jittyc_eqns,num_opids);

    ATermList l = ATtableKeys(term2int);
    for (; !ATisEmpty(l); l=ATgetNext(l))
    {
      int i = ATgetInt((ATermInt) ATtableGet(term2int,ATgetFirst(l)));
      if ( i >= old_opids )
      {
	      int2term[i] = ATAgetFirst(l);
	      jittyc_eqns[i] = NULL;
      }
    }
  }

  return r;
}

ATermAppl RewriterCompilingJitty::fromRewriteFormat(ATerm t)
{
	if ( ATisInt(t) )
	{
		return int2term[ATgetInt((ATermInt) t)];
	} else if ( gsIsDataVarId((ATermAppl) t) )
	{
		return (ATermAppl) t;
	}

	ATermAppl a = fromRewriteFormat(ATgetArgument((ATermAppl) t, 0));
	assert(gsIsOpId(a) || gsIsDataVarId(a));

	int i = 1;
	int arity = ATgetArity(ATgetAFun((ATermAppl) t));
	ATermAppl sort = ATAgetArgument(a, 1);
	while(gsIsSortArrow(sort) && (i < arity))
	{
		ATermList sort_dom = ATLgetArgument(sort, 0);
		ATermList list = ATmakeList0();
		while(!ATisEmpty(sort_dom))
		{
			assert(i < arity);
			list = ATinsert(list, (ATerm) fromRewriteFormat(ATgetArgument((ATermAppl) t,i)));
			sort_dom = ATgetNext(sort_dom);
			++i;
		}
		list = ATreverse(list);
		a = gsMakeDataAppl(a, list);
		sort = ATAgetArgument(sort, 1);
	}

	return a;
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


#ifdef _JITTYC_STORE_TREES
int RewriterCompilingJitty::write_tree(FILE *f, ATermAppl tree, int *num_states)
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

void RewriterCompilingJitty::tree2dot(ATermAppl tree, char *name, char *filename)
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
#endif

static void term2seq(ATerm t, ATermList *s, int *var_cnt)
{
	if ( ATisInt(t) )
	{
		term2seq((ATerm) ATmakeList1(t),s,var_cnt);
	} else if ( ATisAppl(t) )
	{
		if ( gsIsDataVarId((ATermAppl) t) )
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
			int arity = ATgetArity(ATgetAFun((ATermAppl) t));

			*s = ATinsert(*s, (ATerm) ATmakeAppl3(afunF,ATgetArgument((ATermAppl) t,0),dummy,dummy));

			for (int i=1; i<arity; ++i)
			{
				term2seq(ATgetArgument((ATermAppl) t,i),s,var_cnt);
				if ( i<arity-1 )
				{
					*s = ATinsert(*s, (ATerm) ATmakeAppl1(afunN,dummy));
				}
			}
			*s = ATinsert(*s, (ATerm) ATmakeAppl1(afunD,dummy));
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
	int pat_arity = ATgetArity(ATgetAFun(pat));
	ATerm cond = ATelementAt(rule,1);
	ATerm rslt = ATelementAt(rule,3);
	ATermList rseq = ATmakeList0();

	for (int i=1; i<pat_arity; ++i)
	{
		term2seq(ATgetArgument(pat,i),&rseq,var_cnt);
		if ( i<pat_arity-1 )
		{
			rseq = ATinsert(rseq, (ATerm) ATmakeAppl1(afunN,dummy));
		}
	}
	//ATfprintf(stderr,"rseq: %t\n",rseq);
	if ( ATisAppl(cond) && gsIsNil((ATermAppl) cond) )
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

#ifdef _JITTYC_STORE_TREES
ATermAppl RewriterCompilingJitty::create_tree(ATermList rules, int opid, int arity)
#else
static ATermAppl create_tree(ATermList rules, int /*opid*/, int /*arity*/)
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
//		if ( ATgetArity(ATgetAFun((ATermAppl) ATelementAt((ATermList) ATgetFirst(rules),2))) <= arity+1 )
//		{
			rule_seqs = ATinsert(rule_seqs, (ATerm) create_sequence((ATermList) ATgetFirst(rules),&total_rule_vars));
//		}
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

#ifdef _JITTYC_STORE_TREES
	char s[100],t[100];
	sprintf(s,"tree_%i_%s_%i",opid,ATgetName(ATgetAFun(ATAgetArgument(int2term[opid],0))),arity);
	sprintf(t,"tree_%i_%s_%i.dot",opid,ATgetName(ATgetAFun(ATAgetArgument(int2term[opid],0))),arity);
	tree2dot(tree,s,t);
#endif

	return tree;
}


static ATermList get_doubles(ATerm a, ATermList &vars)
{
	if ( ATisInt(a) )
	{
		return ATmakeList0();
	} else if ( ATisAppl(a) && gsIsDataVarId((ATermAppl) a) )
	{
		if ( ATindexOf(vars,a,0) >= 0 )
		{
			return ATmakeList1(a);
		} else {
			vars = ATinsert(vars,a);
			return ATmakeList0();
		}
	} else if ( ATisList(a) )
	{
		ATermList l = ATmakeList0();
		for (ATermList m=(ATermList) a;!ATisEmpty(m); m=ATgetNext(m))
		{
			l = ATconcat(get_doubles(ATgetFirst(m),vars),l);
		}
		return l;
	} else { // ATisAppl(a)
		int arity = ATgetArity(ATgetAFun((ATermAppl) a));
		ATermList l = ATmakeList0();
		for (int i=0; i<arity; ++i)
		{
			l = ATconcat(get_doubles(ATgetArgument((ATermAppl) a,i),vars),l);
		}
		return l;
	}
}

static ATermList get_vars(ATerm a)
{
	if ( ATisInt(a) )
	{
		return ATmakeList0();
	} else if ( ATisAppl(a) && gsIsDataVarId((ATermAppl) a) )
	{
		return ATmakeList1(a);
	} else if ( ATisList(a) )
	{
		ATermList l = ATmakeList0();
		for (ATermList m=(ATermList) a; !ATisEmpty(m); m=ATgetNext(m))
		{
			l = ATconcat(get_vars(ATgetFirst(m)),l);
		}
		return l;
	} else { // ATisAppl(a)
		ATermList l = ATmakeList0();
		int arity = ATgetArity(ATgetAFun((ATermAppl) a));
		for (int i=0; i<arity; ++i)
		{
			l = ATconcat(get_vars(ATgetArgument((ATermAppl) a,i)),l);
		}
		return l;
	}
}

static ATermList dep_vars(ATermList eqn)
{
  unsigned int rule_arity = ATgetArity(ATgetAFun(ATAelementAt(eqn,2)))-1;
  DECL_A(bs,bool,rule_arity);

  ATerm cond = ATelementAt(eqn,1);
  ATermAppl pars = ATAelementAt(eqn,2); // arguments of lhs
  ATermList t = ATmakeList0();
  ATermList vars = ATmakeList1((ATerm) ATconcat(
        get_doubles(ATelementAt(eqn,3),t),
        ((ATisAppl(cond) && gsIsNil((ATermAppl) cond))?ATmakeList0():get_vars(cond))
        )); // List of variables occurring in each argument of the lhs (except the first element which contains variables from the condition and variables which occur more than once in the result)

  //gsfprintf(stderr,"rule: %T\n",ATgetFirst(rules));
  //gsfprintf(stderr,"rule: %T\n",ATAelementAt(ATLgetFirst(rules),2));
  //gsfprintf(stderr,"rule: %P\n",fromRewriteFormat(ATelementAt(ATLgetFirst(rules),2)));
  //gsfprintf(stderr,"pars: %T\n",pars);

  // Indices of arguments that need to be rewritten
  for (unsigned int i = 0; i < rule_arity; i++)
  {
    bs[i] = false;
  }

  // Check all arguments
  for (unsigned int i = 0; i < rule_arity; i++)
  {
    if ( !gsIsDataVarId(ATAgetArgument(pars,i+1)) )
    {
      // Argument is not a variable, so it needs to be rewritten
      bs[i] = true;
      ATermList evars = get_vars(ATgetArgument(pars,i+1));
      for (; !ATisEmpty(evars); evars=ATgetNext(evars))
      {
        int j=i-1; // ATgetLength(ATgetNext(vars))-1
        for (ATermList o=ATgetNext(vars); !ATisEmpty(o); o=ATgetNext(o))
        {
          if ( ATindexOf(ATLgetFirst(o),ATgetFirst(evars),0) >= 0 )
          {
            bs[j] = true;
          }
          --j;
        }
      }
    } else {
      // Argument is a variable; check whether it occurred before
      int j = i-1; // ATgetLength(vars)-1-1
      bool b = false;
      for (ATermList o=vars; !ATisEmpty(o); o=ATgetNext(o))
      {
        if ( ATindexOf(ATLgetFirst(o),ATgetArgument(pars,i+1),0) >= 0 )
        {
          // Same variable, mark it
          if ( j >= 0 )
            bs[j] = true;
          b = true;
        }
        --j;
      }
      if ( b )
      {
        // Found same variable(s), so mark this one as well
        bs[i] = true;
      }
    }
    // Add vars used in expression
    vars = ATinsert(vars,(ATerm) get_vars(ATgetArgument(pars,i+1)));
  }

  ATermList deps = ATmakeList0();
  for (unsigned int i = 0; i < rule_arity; i++)
  {
    if ( bs[i] && gsIsDataVarId(ATAgetArgument(pars,i+1)) )
    {
      deps = ATinsert(deps,ATgetArgument(pars,i+1));
    }
  }

  return deps;
}

#ifdef _JITTYC_STORE_TREES
ATermList RewriterCompilingJitty::create_strategy(ATermList rules, int opid, unsigned int arity, nfs_array nfs)
#else
static ATermList create_strategy(ATermList rules, int opid, unsigned int arity, nfs_array nfs)
#endif
{
  ATermList strat = ATmakeList0();

  //gsfprintf(stderr,"create_strategy: opid %i, arity %i, nfs %i\n",opid,arity,nfs[0]);

  // Array to keep note of the used parameters
  DECL_A(used,bool,arity);
  for (unsigned int i = 0; i < arity; i++)
  {
    used[i] = get_nfs_array(nfs,i);
  }

  // Maintain dependency count (i.e. the number of rules that depend on a given argument)
  DECL_A(args,int,arity);
  for (unsigned int i = 0; i < arity; i++)
  {
    args[i] = -1;
  }

  // Process all (applicable) rules
  DECL_A(bs,bool,arity);
  ATermList dep_list = ATmakeList0();
  for (; !ATisEmpty(rules); rules=ATgetNext(rules))
  {
    unsigned int rule_arity = ATgetArity(ATgetAFun(ATAelementAt(ATLgetFirst(rules),2)))-1;
    if ( rule_arity > arity )
      break;

    ATerm cond = ATelementAt(ATLgetFirst(rules),1);
    ATermAppl pars = ATAelementAt(ATLgetFirst(rules),2); // arguments of lhs
    ATermList t = ATmakeList0();
    ATermList vars = ATmakeList1((ATerm) ATconcat(
          get_doubles(ATelementAt(ATLgetFirst(rules),3),t),
          ((ATisAppl(cond) && gsIsNil((ATermAppl) cond))?ATmakeList0():get_vars(cond))
          )); // List of variables occurring in each argument of the lhs (except the first element which contains variables from the condition and variables which occur more than once in the result)

    //gsfprintf(stderr,"rule: %T\n",ATgetFirst(rules));
    //gsfprintf(stderr,"rule: %T\n",ATAelementAt(ATLgetFirst(rules),2));
    //gsfprintf(stderr,"rule: %P\n",fromRewriteFormat(ATelementAt(ATLgetFirst(rules),2)));
    //gsfprintf(stderr,"pars: %T\n",pars);

    // Indices of arguments that need to be rewritten
    for (unsigned int i = 0; i < rule_arity; i++)
    {
      bs[i] = false;
    }

    // Check all arguments
    for (unsigned int i = 0; i < rule_arity; i++)
    {
      if ( !gsIsDataVarId(ATAgetArgument(pars,i+1)) )
      {
        // Argument is not a variable, so it needs to be rewritten
        bs[i] = true;
        ATermList evars = get_vars(ATgetArgument(pars,i+1));
        for (; !ATisEmpty(evars); evars=ATgetNext(evars))
        {
          int j=i-1;
          for (ATermList o=vars; !ATisEmpty(ATgetNext(o)); o=ATgetNext(o))
          {
            if ( ATindexOf(ATLgetFirst(o),ATgetFirst(evars),0) >= 0 )
            {
              bs[j] = true;
            }
            --j;
          }
        }
      } else {
        // Argument is a variable; check whether it occurred before
        int j = i-1; // ATgetLength(vars)-1-1
        bool b = false;
        for (ATermList o=vars; !ATisEmpty(o); o=ATgetNext(o))
        {
          if ( ATindexOf(ATLgetFirst(o),ATgetArgument(pars,i+1),0) >= 0 )
          {
            // Same variable, mark it
            if ( j >= 0 )
              bs[j] = true;
            b = true;
          }
          --j;
        }
        if ( b )
        {
          // Found same variable(s), so mark this one as well
          bs[i] = true;
        }
      }
      // Add vars used in expression
      vars = ATinsert(vars,(ATerm) get_vars(ATgetArgument(pars,i+1)));
    }
    //gsfprintf(stderr,"vars: %T\n",vars);

    // Create dependency list for this rule
    ATermList deps = ATmakeList0();
    for (unsigned int i = 0; i < rule_arity; i++)
    {
      // Only if needed and not already rewritten
      if ( bs[i] && !used[i] )
      {
        deps = ATinsert(deps,(ATerm) ATmakeInt(i));
        // Increase dependency count
        args[i] += 1;
        //fprintf(stderr,"dep of arg %i\n",i);
      }
    }
    deps = ATreverse(deps);

    // Add rule with its dependencies
    dep_list = ATinsert(dep_list,(ATerm) ATmakeList2((ATerm) deps,ATgetFirst(rules)));
    //gsfprintf(stderr,"\n");
  }

  // Process all rules with their dependencies
  while ( 1 )
  {
    // First collect rules without dependencies to the strategy
    ATermList no_deps = ATmakeList0();
    ATermList has_deps = ATmakeList0();
    for (; !ATisEmpty(dep_list); dep_list=ATgetNext(dep_list))
    {
      if ( ATisEmpty(ATLgetFirst(ATLgetFirst(dep_list))) )
      {
        no_deps = ATinsert(no_deps, ATgetFirst(ATgetNext(ATLgetFirst(dep_list))));
      } else {
        has_deps = ATinsert(has_deps,ATgetFirst(dep_list));
      }
    }
    dep_list = ATreverse(has_deps);

    // Create and add tree of collected rules
    if ( !ATisEmpty(no_deps) )
    {
      //gsfprintf(stderr,"add: %T\n",no_deps);
      strat = ATinsert(strat, (ATerm) create_tree(no_deps,opid,arity));
    }

    // Stop if there are no more rules left
    if ( ATisEmpty(dep_list) )
    {
      break;
    }

    // Otherwise, figure out which argument is most useful to rewrite
    int max = -1;
    int maxidx = -1;
    for (unsigned int i = 0; i < arity; i++)
    {
      if ( args[i] > max )
      {
        maxidx = i;
        max = args[i];
      }
    }

    // If there is a maximum (XXX which should always be the case), add it to the strategy and remove it from the dependency lists
    assert(maxidx >= 0);
    if ( maxidx >= 0 )
    {
      args[maxidx] = -1;
      used[maxidx] = true;
      ATermInt rewr_arg = ATmakeInt(maxidx);

      //gsfprintf(stderr,"add: %T\n",rewr_arg);
      strat = ATinsert(strat,(ATerm) rewr_arg);

      ATermList l = ATmakeList0();
      for (; !ATisEmpty(dep_list); dep_list=ATgetNext(dep_list))
      {
        l = ATinsert(l,(ATerm) ATinsert(ATgetNext(ATLgetFirst(dep_list)),(ATerm) ATremoveElement(ATLgetFirst(ATLgetFirst(dep_list)),(ATerm) rewr_arg)));
      }
      dep_list = ATreverse(l);
    }
  }

  FREE_A(bs);
  FREE_A(args);
  FREE_A(used);

  //gsfprintf(stderr,"strat: %T\n\n",ATreverse(strat));

  return ATreverse(strat);
}

void RewriterCompilingJitty::add_base_nfs(nfs_array nfs, ATermInt opid, unsigned int arity)
{
  for (unsigned int i=0; i<arity; i++)
  {
    if ( always_rewrite_argument(opid,arity,i) )
    {
      set_nfs_array(nfs,i);
    }
  }
}

void RewriterCompilingJitty::extend_nfs(nfs_array nfs, ATermInt opid, unsigned int arity)
{
  ATermList eqns = jittyc_eqns[ATgetInt(opid)];
  if ( eqns == NULL )
  {
    fill_nfs_array(nfs,arity);
    return;
  }
  ATermList strat = create_strategy(eqns,ATgetInt(opid),arity,nfs);
  while ( !ATisEmpty(strat) && ATisInt(ATgetFirst(strat)) )
  {
    set_nfs_array(nfs,ATgetInt((ATermInt) ATgetFirst(strat)));
    strat = ATgetNext(strat);
  }
}

bool RewriterCompilingJitty::opid_is_nf(ATermInt opid, unsigned int num_args)
{
  ATermList l = jittyc_eqns[ATgetInt(opid)];

  if ( l == NULL )
  {
    return true;
  }

  for (; !ATisEmpty(l); l=ATgetNext(l))
  {
    if ( ATgetArity(ATgetAFun(ATAelementAt(ATLgetFirst(l),2)))-1 <= num_args )
    {
      return false;
    }
  }

  return true;
}

void RewriterCompilingJitty::calc_nfs_list(nfs_array nfs, unsigned int arity, ATermList args, int startarg, ATermList nnfvars)
{
  if ( ATisEmpty(args) )
  {
    return;
  }

  set_nfs_array(nfs,arity-ATgetLength(args),calc_nfs(ATgetFirst(args),startarg,nnfvars));
  calc_nfs_list(nfs,arity,ATgetNext(args),startarg+1,nnfvars);
}

bool RewriterCompilingJitty::calc_nfs(ATerm t, int startarg, ATermList nnfvars)
{
  if ( ATisList(t) )
  {
    int arity = ATgetLength((ATermList) t)-1;
    if ( ATisInt(ATgetFirst((ATermList) t)) )
    {
      if ( opid_is_nf((ATermInt) ATgetFirst((ATermList) t),arity) && arity != 0 )
      {
        DECL_NFS_A(args,arity);
        calc_nfs_list(args,arity,ATgetNext((ATermList) t),startarg,nnfvars);
        bool b = is_filled_nfs_array(args,arity);
        FREE_A(args);
        return b;
      } else {
        return false;
      }
    } else {
      if ( arity == 0 )
      {
        assert(false);
        return calc_nfs(ATgetFirst((ATermList) t), startarg, nnfvars);
      }
      return false;
    }
  } else if ( ATisInt(t) )
  {
    return opid_is_nf((ATermInt) t,0);
  } else if ( /*ATisAppl(t) && */ gsIsNil((ATermAppl) t) )
  {
    return (nnfvars == NULL) || (ATindexOf(nnfvars,(ATerm) ATmakeInt(startarg),0) == -1);
  } else { // ATisAppl(t) && gsIsDataVarId((ATermAppl) t)
    assert(ATisAppl(t) && gsIsDataVarId((ATermAppl) t));
    return (nnfvars == NULL) || (ATindexOf(nnfvars,t,0) == -1);
  }
}

string RewriterCompilingJitty::calc_inner_terms(nfs_array nfs, unsigned int arity, ATermList args, int startarg, ATermList nnfvars, nfs_array rewr)
{
  if ( ATisEmpty(args) )
  {
    return "";
  }

  pair<bool,string> head = calc_inner_term(ATgetFirst(args),startarg,nnfvars,rewr?get_nfs_array(rewr,arity-ATgetLength(args)):false);
  set_nfs_array(nfs,arity-ATgetLength(args),head.first);
  string tail = calc_inner_terms(nfs,arity,ATgetNext(args),startarg+1,nnfvars,rewr);
  return head.second+(ATisEmpty(ATgetNext(args))?"":",")+tail;
}

static string calc_inner_appl_head(unsigned int arity)
{
  stringstream ss;
  if ( arity <= 5 )
  {
    ss << "makeAppl" << arity;
  } else {
    ss << "ATmakeAppl";
  }
#ifdef USE_APPL_VALUE
  ss << "(" << ((long int) get_appl_afun_value(arity)) << ",";
#else
  ss << "(appl" << arity << ",";
#endif
  return ss.str();
}

pair<bool,string> RewriterCompilingJitty::calc_inner_term(ATerm t, int startarg, ATermList nnfvars, bool rewr)
{
  if ( ATisList(t) )
  {
    stringstream ss;
    bool b;
    int arity = ATgetLength((ATermList) t)-1;

    if ( ATisInt(ATgetFirst((ATermList) t)) )
    {
      b = opid_is_nf((ATermInt) ATgetFirst((ATermList) t),arity);

      if ( b || !rewr )
      {
        ss << calc_inner_appl_head(arity);
      }

      if ( arity == 0 )
      {
        if ( b || !rewr )
        {
#ifdef USE_INT2ATERM_VALUE
          ss << "(ATerm) " <<  (void *) get_int2aterm_value((ATermInt) ATgetFirst((ATermList) t)) << ")";
#else
          ss << "int2ATerm" << ATgetInt((ATermInt) ATgetFirst((ATermList) t)) << ")";
#endif
        } else {
          ss << "rewr_" << ATgetInt((ATermInt) ATgetFirst((ATermList) t)) << "_0_0()";
        }
      } else {
        DECL_NFS_A(args_nfs,arity);
        calc_nfs_list(args_nfs,arity,ATgetNext((ATermList) t),startarg,nnfvars);
        if ( b || !rewr )
        {
#ifndef USE_INT2ATERM_VALUE
          ss << "int2ATerm";
#endif
        } else {
          ss << "rewr_";
#ifdef EXTEND_NFS
          add_base_nfs(args_nfs,(ATermInt) ATgetFirst((ATermList) t),arity);
          extend_nfs(args_nfs,(ATermInt) ATgetFirst((ATermList) t),arity);
#endif
        }
        if ( arity > NF_MAX_ARITY )
        {
          clear_nfs_array(args_nfs,arity);
        }
        if ( is_clear_nfs_array(args_nfs,arity) || b || rewr || (arity > NF_MAX_ARITY) )
        {
#ifdef USE_INT2ATERM_VALUE
          if ( b || !rewr )
            ss << "(ATerm) " << (void *) get_int2aterm_value((ATermInt) ATgetFirst((ATermList) t));
          else
#endif
          ss << ATgetInt((ATermInt) ATgetFirst((ATermList) t));
        } else {
#ifdef USE_INT2ATERM_VALUE
          if ( b || !rewr )
            ss << "(ATerm) " << (void *) get_int2aterm_value(ATgetInt((ATermInt) ATgetFirst((ATermList) t))+((1 << arity)-arity-1)+get_nfs_array_value(args_nfs,arity));
          else
#endif
          ss << (ATgetInt((ATermInt) ATgetFirst((ATermList) t))+((1 << arity)-arity-1)+args_nfs);
        }
        DECL_NFS_A(args_first,arity);
        if ( rewr && b )
        {
          fill_nfs_array(args_nfs,arity);
        }
        string args_second = calc_inner_terms(args_first,arity,ATgetNext((ATermList) t),startarg,nnfvars,args_nfs);
        assert( !rewr || b || (arity > NF_MAX_ARITY) || equal_nfs_array(args_first,args_nfs,arity) );
        if ( rewr && !b )
        {
          ss << "_" << arity << "_";
          if ( arity <= NF_MAX_ARITY )
          {
            ss << get_nfs_array_value(args_first,arity);
          } else {
            ss << "0";
          }
          ss << "(";
        } else {
          ss << ",";
        }
        ss << args_second << ")";
        if ( !is_filled_nfs_array(args_first,arity) )
        {
          b = false;
        }
        FREE_A(args_nfs);
      }
      b = b || rewr;

    } else {
      if ( arity == 0 )
      {
        assert(false);
        return calc_inner_term(ATgetFirst((ATermList) t), startarg, nnfvars);
      }
      b = rewr;
      pair<bool,string> head = calc_inner_term(ATgetFirst((ATermList) t),startarg,nnfvars,false);
      DECL_NFS_A(tail_first,arity);
      string tail_second = calc_inner_terms(tail_first,arity,ATgetNext((ATermList) t),startarg,nnfvars,NULL);
      ss << "isAppl(" << head.second << ")?";
      if ( rewr )
      {
        ss << "rewrite(";
      }
      ss <<"build" << arity << "(" << head.second << "," << tail_second << ")";
      if ( rewr )
      {
        ss << ")";
      }
      ss << ":";
      bool c = rewr;
      if ( rewr && (nnfvars != NULL) && (ATindexOf(nnfvars,(ATerm) ATmakeInt(startarg),0) != -1) )
      {
        ss << "rewrite(";
        c = false;
      }
      ss << calc_inner_appl_head(arity) << "(ATerm) " << head.second << ",";
      if ( c )
      {
        clear_nfs_array(tail_first,arity);
        DECL_NFS_A(rewrall,arity); fill_nfs_array(rewrall,arity);
        tail_second = calc_inner_terms(tail_first,arity,ATgetNext((ATermList) t),startarg,nnfvars,rewrall);
      }
      ss << tail_second << ")";
      if ( rewr && (nnfvars != NULL) && (ATindexOf(nnfvars,(ATerm) ATmakeInt(startarg),0) != -1) )
      {
        ss << ")";
      }
      FREE_A(tail_first);
    }

    return pair<bool,string>(b,ss.str());

  } else if ( ATisInt(t) )
  {
    stringstream ss;
    bool b = opid_is_nf((ATermInt) t,0);
    if ( rewr && !b )
    {
      ss << "rewr_" << ATgetInt((ATermInt) t) << "_0_0()";
    } else {
#ifdef USE_REWRAPPL_VALUE
      ss << "(ATermAppl) " << (void *) get_rewrappl_value((ATermInt) t);
#else
      ss << "rewrAppl" << ATgetInt((ATermInt) t);
#endif
    }
    return pair<bool,string>(
        rewr || b,
        ss.str()
        );

  } else if ( /*ATisAppl(t) && */ gsIsNil((ATermAppl) t) )
  {
    stringstream ss;
    bool b = (nnfvars != NULL) && (ATindexOf(nnfvars,(ATerm) ATmakeInt(startarg),0) != -1);
    if ( rewr && b )
    {
      ss << "rewrite(arg" << startarg << ")";
    } else {
      ss << "arg" << startarg;
    }
    return pair<bool,string>(rewr || !b, ss.str());

  } else { // ATisAppl(t) && gsIsDataVarId((ATermAppl) t)
    assert(ATisAppl(t) && gsIsDataVarId((ATermAppl) t));
    stringstream ss;
    bool b = (nnfvars != NULL) && (ATindexOf(nnfvars,t,0) != -1);
    if ( rewr && b )
    {
      ss << "rewrite(" << (ATgetName(ATgetAFun(ATAgetArgument((ATermAppl) t,0)))+1) << ")";
    } else {
      ss << (ATgetName(ATgetAFun(ATAgetArgument((ATermAppl) t,0)))+1);
    }
    return pair<bool,string>(rewr || !b, ss.str());
  }
}

void RewriterCompilingJitty::calcTerm(FILE *f, ATerm t, int startarg, ATermList nnfvars, bool rewr)
{
#ifdef USE_NEW_CALCTERM
  pair<bool,string> p = calc_inner_term(t,startarg,nnfvars,rewr);
  //fprintf(f," /* (%s,%s) */ ",p.first?"true":"false",p.second.c_str());
  fprintf(f,"%s",p.second.c_str());
  return;
#else
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
        calcTerm(f,ATgetFirst((ATermList) t),0,nnfvars,rewr);
        return;
      }

      v = true;
      fprintf(f,"(isAppl(");
      calcTerm(f,ATgetFirst((ATermList) t),0,NULL,false);
      fprintf(f,")?rewrite(build%i(",arity);
      calcTerm(f,ATgetFirst((ATermList) t),0,NULL,false);
      l = ATgetNext((ATermList) t);
      int i = startarg;
      for (; !ATisEmpty(l); l=ATgetNext(l))
      {
        fprintf(f,",");
        if ( ATisAppl(ATgetFirst(l)) && gsIsNil(ATAgetFirst(l)) )
        {
          fprintf(f,"arg%i",i);
        } else {
          calcTerm(f,ATgetFirst(l),0,NULL,false);
        }
        i++;
      }
      if ( rewr && (nnfvars != NULL) && (ATindexOf(nnfvars,ATgetFirst((ATermList) t),0) >= 0) )
      {
        fprintf(f,")):rewrite(");
        rewr = false;
      } else {
        fprintf(f,")):(");
      }
    }

    bool b2 = false;
    if ( ATisInt(ATgetFirst((ATermList) t)) )
    {
        b = rewr;
	b2 = true;
    }

    unsigned int nfs = 0;
    if ( b )
    {
      if ( arity <= NF_MAX_ARITY )
      {
	      int i = 0;
	      for (ATermList m=ATgetNext((ATermList) t); !ATisEmpty(m); m=ATgetNext(m))
	      {
		      if ( ATisAppl(ATgetFirst(m)) && (nnfvars != NULL) && ( (gsIsDataVarId(ATAgetFirst(m)) && (ATindexOf(nnfvars,ATgetFirst(m),0) == -1)) || (gsIsNil(ATAgetFirst(m)) && (ATindexOf(nnfvars,(ATerm) ATmakeInt(i),0) == -1)) ) )
		      {
			      nfs = nfs | (1 << i);
		      }
		      i++;
	      }
#ifdef EXTEND_NFS
          nfs = extend_nfs( nfs | get_base_nfs((ATermInt) ATgetFirst((ATermList) t),arity) , (ATermInt) ATgetFirst((ATermList) t) , arity );
#endif
      }
      fprintf(f,"rewr_%i_%i_%u(",ATgetInt((ATermInt) ATgetFirst((ATermList) t)),arity,nfs);
    } else {
      if ( arity == 0 )
      {
#ifdef USE_REWRAPPL_VALUE
        fprintf(f,"((ATermAppl) %p",get_rewrappl_value((ATermInt) ATgetFirst((ATermList) t)));
#else
        fprintf(f,"(rewrAppl%i",ATgetInt((ATermInt) ATgetFirst((ATermList) t)));
#endif
      } else {
        if ( arity > 5 )
        {
#ifdef USE_APPL_VALUE
          fprintf(f,"ATmakeAppl(%li,",get_appl_afun_value(arity));
#else
          fprintf(f,"ATmakeAppl(appl%i,",arity);
#endif
        } else {
#ifdef USE_APPL_VALUE
          fprintf(f,"ATmakeAppl%i(%li,",arity+1,get_appl_afun_value(arity));
#else
          fprintf(f,"ATmakeAppl%i(appl%i,",arity+1,arity);
#endif
        }
        if ( ATisInt(ATgetFirst((ATermList) t)) )
        {
	  if ( b2 )
	  {
            unsigned int nfs = 0;
            if ( arity <= NF_MAX_ARITY )
            {
    	        int i = 0;
    	        for (ATermList m=ATgetNext((ATermList) t); !ATisEmpty(m); m=ATgetNext(m))
    	        {
    	  	      if ( ATisAppl(ATgetFirst(m)) && (nnfvars != NULL) && ( (gsIsDataVarId(ATAgetFirst(m)) && (ATindexOf(nnfvars,ATgetFirst(m),0) == -1)) || (gsIsNil(ATAgetFirst(m)) && (ATindexOf(nnfvars,(ATerm) ATmakeInt(i),0) == -1)) ) )
    	  	      {
    	  		      nfs = nfs | (1 << i);
    	  	      }
    	  	      i++;
    	        }
            }
	    if ( nfs == 0 )
	    {
#ifdef USE_INT2ATERM_VALUE
              fprintf(f,"(ATerm) %p",get_int2aterm_value((ATermInt) ATgetFirst((ATermList) t)));
#else
              fprintf(f,"(ATerm) int2ATerm%i",ATgetInt((ATermInt) ATgetFirst((ATermList) t)));
#endif
	    } else {
#ifdef USE_INT2ATERM_VALUE
              fprintf(f,"(ATerm) %p",get_int2aterm_value(ATgetInt((ATermInt) ATgetFirst((ATermList) t))+((1 << arity)-arity-1)+nfs));
#else
              fprintf(f,"(ATerm) int2ATerm%i",ATgetInt((ATermInt) ATgetFirst((ATermList) t))+((1 << arity)-arity-1)+nfs);
#endif
	    }
//	  } else {
//            fprintf(f,"(ATerm) int2ATerm%i",ATgetInt((ATermInt) ATgetFirst((ATermList) t)));
	  }
        } else {
          fprintf(f,"(ATerm) ");
          calcTerm(f,ATgetFirst((ATermList) t),0,nnfvars,rewr);
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
        if ( !((rewr && !b) || !((nfs&(1<<(i-startarg))) == 0)) || (ATindexOf(nnfvars,(ATerm) ATmakeInt(i),0) == -1) )
	{
          fprintf(f,"arg%i",i);
	} else {
          fprintf(f,"rewrite(arg%i)",i);
	}
      } else {
        calcTerm(f,ATgetFirst(l),0,nnfvars,(rewr && !b) || ((nfs&(1<<(i-startarg))) != 0));
      }
      i++;
    }
    fprintf(f,")");

    if ( v )
    {
      fprintf(f,"))");
    }
  } else if ( ATisInt(t) )
  {
    ATermList l;
    bool b = false;
    if ( (l = jittyc_eqns[ATgetInt((ATermInt) t)]) != NULL )
    {
      if ( !ATisEqual(ATgetFirst(l),ATmakeInt(0)) )
      {
        b = true;
      }
    }

    if ( b )
    {
      fprintf(f,"rewr_%i_0_0()",ATgetInt((ATermInt) t));
    } else {
//      fprintf(f,"ATmakeAppl1(appl0,int2ATerm%i)",ATgetInt((ATermInt) t));
#ifdef USE_REWRAPPL_VALUE
      fprintf(f,"(ATermAppl) %p",get_rewrappl_value((ATermInt) t));
#else
      fprintf(f,"rewrAppl%i",ATgetInt((ATermInt) t));
#endif
    }
  } else {
    if ( rewr && (nnfvars != NULL) && (ATindexOf(nnfvars,t,0) >= 0) )
    {
      fprintf(f,"rewrite(%s)",ATgetName(ATgetAFun(ATAgetArgument((ATermAppl) t,0)))+1);
    } else {
      fprintf(f,"%s",ATgetName(ATgetAFun(ATAgetArgument((ATermAppl) t,0)))+1);
    }
    //fprintf(f,"var_%s_%x",ATgetName(ATgetAFun(ATAgetArgument((ATermAppl) t,0))),ATXgetArgument((ATermAppl) t,1));
  }
#endif
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
void RewriterCompilingJitty::implement_tree_aux(FILE *f, ATermAppl tree, int cur_arg, int parent, int level, int cnt, int d, int arity, bool *used, ATermList nnfvars)
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
			if ( !used[cur_arg] )
			{
				nnfvars = ATinsert(nnfvars,ATgetArgument(tree,0));
			}
		} else {
			fprintf(f,"%sATermAppl %s = (ATermAppl) ATgetArgument(%s%i,%i); // S\n",whitespace(d*2),ATgetName(ATgetAFun(ATAgetArgument(ATAgetArgument(tree,0),0)))+1,(level==1)?"arg":"t",parent,cur_arg);
			//fprintf(f,"%sATermAppl %s = t%i; // S\n",whitespace(d*2),ATgetName(ATgetAFun(ATAgetArgument(ATAgetArgument(tree,0),0)))+1,cur_arg);
		}
		implement_tree_aux(f,ATAgetArgument(tree,1),cur_arg,parent,level,cnt,d,arity,used,nnfvars);
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
		implement_tree_aux(f,ATAgetArgument(tree,1),cur_arg,parent,level,cnt,d+1,arity,used,nnfvars);
		fprintf(f,"%s} else {\n",whitespace(d*2));
		implement_tree_aux(f,ATAgetArgument(tree,2),cur_arg,parent,level,cnt,d+1,arity,used,nnfvars);
		fprintf(f,"%s}\n",whitespace(d*2));
		return;
	} else if ( isF(tree) )
	{
#ifdef IT_DEBUG
gsfprintf(IT_DEBUG_FILE "F(%P)\n",int2term[ATgetInt((ATermInt) ATgetArgument(tree,0))]);
#endif
		if ( level == 0 )
		{
			fprintf(f,"%sif ( "
#ifdef F_DONT_TEST_ISAPPL
                                          "isAppl(arg%i) && "
#endif
                                          "ATisEqual(ATgetArgument(arg%i,0),"
#ifdef USE_INT2ATERM_VALUE
                                          "(ATerm) %p"
#else
                                          "int2ATerm%i"
#endif
                                          ") ) // F\n"
				  "%s{\n",
				whitespace(d*2),
#ifdef F_DONT_TEST_ISAPPL
                                cur_arg,
#endif
                                cur_arg,
#ifdef USE_INT2ATERM_VALUE
                                get_int2aterm_value((ATermInt) ATgetArgument(tree,0)),
#else
                                ATgetInt((ATermInt) ATgetArgument(tree,0)),
#endif
				whitespace(d*2)
				);
		} else {
			fprintf(f,"%sif ( "
#ifdef F_DONT_TEST_ISAPPL
                                          "isAppl(ATgetArgument(%s%i,%i)) && "
#endif
                                          "ATisEqual(ATgetArgument((ATermAppl) ATgetArgument(%s%i,%i),0),"
#ifdef USE_INT2ATERM_VALUE
                                          "(ATerm) %p"
#else
                                          "int2ATerm%i"
#endif
                                          ") ) // F\n"
			//fprintf(f,"%sif ( isAppl(t%i) && ATisEqual(ATgetArgument(t%i,0),int2ATerm%i) ) // F\n"
				  "%s{\n"
				  "%s  ATermAppl t%i = (ATermAppl) ATgetArgument(%s%i,%i);\n",
			//	  "%s  ATermAppl t%i = (ATermAppl) ATgetArgument(t%i,1);\n",
				whitespace(d*2),
#ifdef F_DONT_TEST_ISAPPL
                                (level==1)?"arg":"t",parent,cur_arg,
#endif
                                (level==1)?"arg":"t",parent,cur_arg,
#ifdef USE_INT2ATERM_VALUE
                                get_int2aterm_value((ATermInt) ATgetArgument(tree,0)),
#else
                                ATgetInt((ATermInt) ATgetArgument(tree,0)),
#endif
			//	whitespace(d*2),cur_arg,cur_arg,ATgetInt((ATermInt) ATgetArgument(tree,0)),
				whitespace(d*2),
				whitespace(d*2),cnt,(level==1)?"arg":"t",parent,cur_arg
			//	whitespace(d*2),cnt,cur_arg
				);
		}
		push_st(cur_arg);
		push_st(parent);
		implement_tree_aux(f,ATAgetArgument(tree,1),1,(level==0)?cur_arg:cnt,level+1,cnt+1,d+1,arity,used,nnfvars);
		pop_st();
		pop_st();
		fprintf(f,"%s} else {\n",whitespace(d*2));
		implement_tree_aux(f,ATAgetArgument(tree,2),cur_arg,parent,level,cnt,d+1,arity,used,nnfvars);
		fprintf(f,"%s}\n",whitespace(d*2));
		return;
	} else if ( isD(tree) )
	{
#ifdef IT_DEBUG
gsfprintf(IT_DEBUG_FILE "D\n");
#endif
		int i = pop_st();
		int j = pop_st();
		implement_tree_aux(f,ATAgetArgument(tree,0),j,i,level-1,cnt,d,arity,used,nnfvars);
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
		implement_tree_aux(f,ATAgetArgument(tree,0),cur_arg+1,parent,level,cnt,d,arity,used,nnfvars);
		return;
	} else if ( isC(tree) )
	{
#ifdef IT_DEBUG
gsfprintf(IT_DEBUG_FILE "C\n");
#endif
		fprintf(f,"%sif ( ATisEqual(",whitespace(d*2));
		calcTerm(f,ATgetArgument(tree,0),0,nnfvars);
#ifdef USE_REWRAPPL_VALUE
		fprintf(f,",(ATermAppl) %p) ) // C\n"
			  "%s{\n",
			get_rewrappl_value(true_num),
			whitespace(d*2)
			);
#else
		fprintf(f,",rewrAppl%i) ) // C\n"
			  "%s{\n",
			true_num,
			whitespace(d*2)
			);
#endif
		implement_tree_aux(f,ATAgetArgument(tree,1),cur_arg,parent,level,cnt,d+1,arity,used,nnfvars);
		fprintf(f,"%s} else {\n",whitespace(d*2));
		implement_tree_aux(f,ATAgetArgument(tree,2),cur_arg,parent,level,cnt,d+1,arity,used,nnfvars);
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
		calcTerm(f,add_args(ATgetArgument(tree,0),arity-cur_arg-1),get_startarg(ATgetArgument(tree,0),cur_arg+1),nnfvars);
		fprintf(f,";\n");
		return;
	} else {
#ifdef IT_DEBUG
gsfprintf(IT_DEBUG_FILE "X\n");
#endif
		return;
	}
}

void RewriterCompilingJitty::implement_tree(FILE *f, ATermAppl tree, int arity, int d, int /*opid*/, bool *used)
{
#ifdef IT_DEBUG
gsfprintf(IT_DEBUG_FILE "implement_tree %P (%i)\n",int2term[opid],opid);
#endif
	int l = 0;

	ATermList nnfvars = ATmakeList0();
	for (int i=0; i<arity; i++)
	{
		if ( !used[i] )
		{
			nnfvars = ATinsert(nnfvars,(ATerm) ATmakeInt(i));
		}
	}

	while ( isC(tree) )
	{
		fprintf(f,"%sif ( ATisEqual(",whitespace(d*2));
		calcTerm(f,ATgetArgument(tree,0),0,ATmakeList0());
#ifdef USE_REWRAPPL_VALUE
		fprintf(f,",(ATermAppl) %p) ) // C\n"
			  "%s{\n"
			  "%sreturn ",
			get_rewrappl_value(true_num),
			whitespace(d*2),
			whitespace(d*2)
			);
#else
		fprintf(f,",rewrAppl%i) ) // C\n"
			  "%s{\n"
			  "%sreturn ",
			true_num,
			whitespace(d*2),
			whitespace(d*2)
			);
#endif
		assert(isR(ATAgetArgument(tree,1)));
		calcTerm(f,add_args(ATgetArgument(ATAgetArgument(tree,1),0),arity),get_startarg(ATgetArgument(ATAgetArgument(tree,1),0),0),nnfvars);
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
		calcTerm(f,add_args(ATgetArgument(tree,0),arity),get_startarg(ATgetArgument(tree,0),0),nnfvars);
		fprintf(f,";\n");
	} else {
		reset_st();
		implement_tree_aux(f,tree,0,0,0,0,d,arity,used,nnfvars);
	}
	while ( l > 0 )
	{
		d--;
		fprintf(f,"%s}\n",whitespace(d*2));
		l--;
	}
}

static void finish_function(FILE *f, int arity, int opid, bool *used)
{
        if ( arity == 0 )
        {
#ifdef USE_REWRAPPL_VALUE
  fprintf(f,  "  return ((ATermAppl) %p",
      get_rewrappl_value(opid)
         );
#else
  fprintf(f,  "  return (rewrAppl%i",
      opid
         );
#endif
        } else {
          if ( arity > 5 )
          {
  fprintf(f,  "  return ATmakeAppl("
#ifdef USE_APPL_VALUE
                                   "%li,"
#else
                                   "appl%i,"
#endif
#ifdef USE_INT2ATERM_VALUE
                                   "(ATerm) %p",
#else
                                   "(ATerm) int2ATerm%i",
#endif
#ifdef USE_APPL_VALUE
      (long int) get_appl_afun_value(arity),
#else
      arity,
#endif
#ifdef USE_INT2ATERM_VALUE
      get_int2aterm_value(opid)
#else
      opid
#endif
         );
          } else {
  fprintf(f,  "  return ATmakeAppl%i("
#ifdef USE_APPL_VALUE
                                   "%li,"
#else
                                   "appl%i,"
#endif
#ifdef USE_INT2ATERM_VALUE
                                   "(ATerm) %p",
#else
                                   "(ATerm) int2ATerm%i",
#endif
      arity+1,
#ifdef USE_APPL_VALUE
      (long int) get_appl_afun_value(arity),
#else
      arity,
#endif
#ifdef USE_INT2ATERM_VALUE
      get_int2aterm_value(opid)
#else
      opid
#endif
         );
          }
        }
        for (int i=0; i<arity; i++)
        {
		if ( (used != NULL) && used[i] )
		{
  fprintf(f,                 ",(ATerm) arg%i",i);
		} else {
  fprintf(f,                 ",(ATerm) rewrite(arg%i)",i);
		}
        }
  fprintf(f,                 ");\n");
}

void RewriterCompilingJitty::implement_strategy(FILE *f, ATermList strat, int arity, int d, int opid, unsigned int nf_args)
{
//#ifdef IS_DEBUG
gsfprintf(IT_DEBUG_FILE "implement_strategy %P (%i)\n",int2term[opid],opid);
gsfprintf(IT_DEBUG_FILE "implement_strategy: %T\n",strat);
fflush(f);
//#endif
	bool used[arity];
	for (int i=0; i<arity; i++)
	{
		used[i] = ((nf_args & (1 << i)) != 0);
	}

	while ( !ATisEmpty(strat) )
	{
		if ( ATisInt(ATgetFirst(strat)) )
		{
			int arg = ATgetInt((ATermInt) ATgetFirst(strat));

			if ( !used[arg] )
			{
				fprintf(f,"%sarg%i = rewrite(arg%i);\n",whitespace(2*d),arg,arg);

				used[arg] = true;
			}
		} else {
			fprintf(f,"%s{\n",whitespace(2*d));
			implement_tree(f,(ATermAppl) ATgetFirst(strat),arity,d+1,opid,used);
			fprintf(f,"%s}\n",whitespace(2*d));
		}

		strat = ATgetNext(strat);
	}

	finish_function(f,arity,opid,used);
}

ATermAppl RewriterCompilingJitty::build_ar_expr(ATerm expr, ATermAppl var)
{
  if ( ATisInt(expr) )
  {
    return make_ar_false();
  }

  if ( ATisAppl(expr) && gsIsDataVarId((ATermAppl) expr) )
  {
    if ( ATisEqual(expr,var) )
    {
      return make_ar_true();
    } else {
      return make_ar_false();
    }
  }

  ATerm head = ATgetFirst((ATermList) expr);
  if ( !ATisInt(head) )
  {
    return ATisEqual(head,var)?make_ar_true():make_ar_false();
  }

  ATermAppl result = make_ar_false();

  ATermList args = ATgetNext((ATermList) expr);
  unsigned int arity = ATgetLength(args);
  for (unsigned int i=0; i<arity; i++, args=ATgetNext(args))
  {
    int idx = ATgetInt((ATermInt) ATtableGet(int2ar_idx,(ATerm) head)) + ((arity-1)*arity)/2 + i;
    ATermAppl t = build_ar_expr(ATgetFirst(args),var);
    result = make_ar_or(result,make_ar_and(make_ar_var(idx),t));
  }

  return result;
}

ATermAppl RewriterCompilingJitty::build_ar_expr_aux(ATermList eqn, unsigned int arg, unsigned int arity)
{
  ATermAppl pars = ATAelementAt(eqn,2); // arguments of lhs

  unsigned int eqn_arity = ATgetArity(ATgetAFun(pars))-1;
  if ( eqn_arity > arity )
  {
    return make_ar_true();
  }
  if ( eqn_arity <= arg )
  {
    ATerm rhs = ATelementAt(eqn,3);
    if ( ATisInt(rhs) )
    {
      int idx = ATgetInt((ATermInt) ATtableGet(int2ar_idx,rhs)) + ((arity-1)*arity)/2 + arg;
      return make_ar_var(idx);
    } else if ( ATisList(rhs) && ATisInt(ATgetFirst((ATermList) rhs)) )
    {
      int rhs_arity = ATgetLength((ATermList) rhs)-1;
      int diff_arity = arity-eqn_arity;
      int rhs_new_arity = rhs_arity+diff_arity;
      int idx = ATgetInt((ATermInt) ATtableGet(int2ar_idx,ATgetFirst((ATermList) rhs))) + ((rhs_new_arity-1)*rhs_new_arity)/2 + (arg - eqn_arity + rhs_arity);
      return make_ar_var(idx);
    } else {
      return make_ar_false();
    }
  }

  ATermAppl arg_term = ATAgetArgument(pars,arg+1);
  if ( !gsIsDataVarId(arg_term) )
  {
    return make_ar_true();
  }

  if ( ATindexOf(dep_vars(eqn),(ATerm) arg_term,0) >= 0 )
  {
    return make_ar_true();
  }

  return build_ar_expr(ATelementAt(eqn,3),arg_term);
}

ATermAppl RewriterCompilingJitty::build_ar_expr(ATermList eqns, unsigned int arg, unsigned int arity)
{
  if ( (eqns == NULL) || ATisEmpty(eqns) )
  {
    return make_ar_true();
  } else {
    return make_ar_and(build_ar_expr_aux(ATLgetFirst(eqns),arg,arity),build_ar_expr(ATgetNext(eqns),arg,arity));
  }
}

bool RewriterCompilingJitty::always_rewrite_argument(ATermInt opid, unsigned int arity, unsigned int arg)
{
  return !is_ar_false(ar[ATgetInt((ATermInt) ATtableGet(int2ar_idx,(ATerm) opid))+((arity-1)*arity)/2+arg]);
}

bool RewriterCompilingJitty::calc_ar(ATermAppl expr)
{
  if ( is_ar_true(expr) )
  {
    return true;
  } else if ( is_ar_false(expr) )
  {
    return false;
  } else if ( is_ar_and(expr) )
  {
    return calc_ar(ATAgetArgument(expr,0)) && calc_ar(ATAgetArgument(expr,1));
  } else if ( is_ar_and(expr) )
  {
    return calc_ar(ATAgetArgument(expr,0)) || calc_ar(ATAgetArgument(expr,1));
  } else { // is_ar_var(expr)
    return !is_ar_false(ar[ATgetInt((ATermInt) ATgetArgument(expr,0))]);
  }
}

void RewriterCompilingJitty::fill_always_rewrite_array()
{
  ar = (ATermAppl *) malloc(ar_size*sizeof(ATermAppl));
  if ( ar == NULL )
  {
    gsErrorMsg("cannot allocate enough memory (%li bytes)\n",ar_size*sizeof(ATermAppl));
  }
  for (unsigned int i=0; i<ar_size; i++)
  {
    ar[i] = NULL;
  }
  ATprotectArray((ATerm *) ar,ar_size);

  ATermList ints = ATtableKeys(int2ar_idx);
  for (; !ATisEmpty(ints); ints=ATgetNext(ints))
  {
    unsigned int arity = getArity(int2term[ATgetInt((ATermInt) ATgetFirst(ints))]);
    ATermList eqns = jittyc_eqns[ATgetInt((ATermInt) ATgetFirst(ints))];
    int idx = ATgetInt((ATermInt) ATtableGet(int2ar_idx,ATgetFirst(ints)));
    for (unsigned int i=1; i<=arity; i++)
    {
      for ( unsigned int j=0; j<i; j++)
      {
        ar[idx+((i-1)*i)/2+j] = build_ar_expr(eqns,j,i);
      }
    }
  }

#define PRINT_AR \
    for (ATermList l=ATtableKeys(term2int); !ATisEmpty(l); l=ATgetNext(l)) \
    { \
      ATermAppl opid = (ATermAppl) ATgetFirst(l); \
      unsigned int idx = ATgetInt((ATermInt) ATtableGet(int2ar_idx,ATtableGet(term2int,(ATerm) opid))); \
      for (unsigned int i=1; i<=getArity(opid); i++) \
      { \
        for (unsigned int j=0; j<i; j++) \
        { \
          gsfprintf(stderr,"%P (%i), arity %i, arg %i:  (%i)  %T\n",opid,idx,i,j,idx+((i-1)*i)/2+j,ar[idx+((i-1)*i)/2+j]); \
        } \
      } \
    }
  bool notdone = true;
  while ( notdone )
  {
    //PRINT_AR
    notdone = false;
    for (unsigned int i=0; i<ar_size; i++)
    {
      if ( !is_ar_false(ar[i]) && !calc_ar(ar[i]) )
      {
        ar[i] = make_ar_false();
        notdone = true;
      }
    }
  }
  //PRINT_AR
}

void RewriterCompilingJitty::CompileRewriteSystem(mcrl2::data::data_specification DataSpec)
{
  ATermList l,n;
  ATermTable tmp_eqns;
  ATermInt i;
  int j;
  FILE *f;
  char *s,*t;
  void *h;


  tmp_eqns = ATtableCreate(100,75); // XXX would be nice to know the number op OpIds

  num_opids = 0;

  true_num = ATgetInt((ATermInt) OpId2Int(gsMakeDataExprTrue(),true));

  l = DataSpec.equations();
  for (; !ATisEmpty(l); l=ATgetNext(l))
  {
    try
    {
      CheckRewriteRule(ATAgetFirst(l));
    } catch ( std::runtime_error &e ) {
      gsWarningMsg("%s\n",e.what());
      continue;
    }

    ATermAppl u = (ATermAppl) toInnerc(toInner(ATAgetArgument(ATAgetFirst(l),2),true));

    if ( (n = (ATermList) ATtableGet(tmp_eqns,ATgetArgument(u,0))) == NULL )
    {
    	n = ATmakeList0();
    }
    n = ATinsert(n,(ATerm) ATmakeList4(ATgetArgument(ATAgetFirst(l),0),(ATerm) toInner(ATAgetArgument(ATAgetFirst(l),1),true),(ATerm) u,(ATerm) toInner(ATAgetArgument(ATAgetFirst(l),3),true)));
    ATtablePut(tmp_eqns,ATgetArgument(u,0),(ATerm) n);
  }

  int2term = (ATermAppl *) malloc(num_opids*sizeof(ATermAppl));
  memset(int2term,0,num_opids*sizeof(ATermAppl));
  ATprotectArray((ATerm *) int2term,num_opids);
  jittyc_eqns = (ATermList *) malloc(num_opids*sizeof(ATermList));
  memset(jittyc_eqns,0,num_opids*sizeof(ATermList));
  ATprotectArray((ATerm *) jittyc_eqns,num_opids);
  ar_size = 0;
  int2ar_idx = ATtableCreate(100,75);

  l = ATtableKeys(term2int);
//  gsfprintf(stderr,"TERM2INT %T\n",l);
  for (; !ATisEmpty(l); l=ATgetNext(l))
  {
    i = (ATermInt) ATtableGet(term2int,ATgetFirst(l));
    int2term[ATgetInt(i)] = ATAgetFirst(l);
    if ( ATtableGet(tmp_eqns,(ATerm) i) != NULL )
    {
      //jittyc_eqns[ATgetInt(i)] = create_strategy((ATermList) ATtableGet(tmp_eqns,(ATerm) i), ATgetInt(i));
      jittyc_eqns[ATgetInt(i)] = (ATermList) ATtableGet(tmp_eqns,(ATerm) i);
    } else {
      jittyc_eqns[ATgetInt(i)] = NULL;
    }
    if ( ATtableGet(int2ar_idx,(ATerm) i) == NULL )
    {
      unsigned int arity = getArity(ATAgetFirst(l));
      ATtablePut(int2ar_idx,(ATerm) i,(ATerm) ATmakeInt(ar_size));
      ar_size += (arity*(arity+1))/2;
    }
  }

  ATtableDestroy(tmp_eqns);

#ifdef EXTEND_NFS
  fill_always_rewrite_array();
#endif

  s = (char *) malloc(50);
  sprintf(s,"jittyc_%i_%lx",getpid(),(long) this);
  t = (char *) malloc(100+strlen(JITTYC_COMPILE_COMMAND)+strlen(JITTYC_LINK_COMMAND));

  sprintf(t,"%s.c",s);
  file_c = strdup(t);
  sprintf(t,"%s.o",s);
  file_o = strdup(t);
  sprintf(t,"%s.so",s);
  file_so = strdup(t);

  f = fopen(file_c,"w");
  if ( f == NULL )
  {
	  perror("fopen");
          gsErrorMsg("could not create temporary file for rewriter\n");
	  exit(1);
  }

  //
  //  Print includes
  //
  fprintf(f,  "#include <stdlib.h>\n"
      "#include <string.h>\n"
      "#include <aterm2.h>\n"
      "#include \"assert.h\"\n"
//      "#include \"libstruct.h\"\n"
//      "#include \"liblowlevel.h\"\n"
//      "#include \"gssubstitute.h\"\n"
      "#ifndef ATprotectAppl\n"
      "extern void ATprotectAppl(ATermAppl *a);\n"
      "#endif\n"
      "static ATermAppl makeAppl0(AFun a, ATerm h) { return ATmakeAppl1(a,h); }\n"
      "static ATermAppl makeAppl1(AFun a, ATerm h, ATermAppl t) { return ATmakeAppl2(a,h,(ATerm) t); }\n"
      "static ATermAppl makeAppl2(AFun a, ATerm h, ATermAppl t1, ATermAppl t2) { return ATmakeAppl3(a,h,(ATerm) t1, (ATerm) t2); }\n"
      "static ATermAppl makeAppl3(AFun a, ATerm h, ATermAppl t1, ATermAppl t2, ATermAppl t3) { return ATmakeAppl4(a,h,(ATerm) t1, (ATerm) t2, (ATerm) t3); }\n"
      "static ATermAppl makeAppl4(AFun a, ATerm h, ATermAppl t1, ATermAppl t2, ATermAppl t3, ATermAppl t4) { return ATmakeAppl5(a,h,(ATerm) t1, (ATerm) t2, (ATerm) t3, (ATerm) t4); }\n"
      "static ATermAppl makeAppl5(AFun a, ATerm h, ATermAppl t1, ATermAppl t2, ATermAppl t3, ATermAppl t4, ATermAppl t5) { return ATmakeAppl6(a,h,(ATerm) t1, (ATerm) t2, (ATerm) t3, (ATerm) t4, (ATerm) t5); }\n"
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
    /* if ( jittyc_eqns[j] != NULL )
    { */
      int arity = getArity(int2term[j]);
      if ( arity > max_arity )
      {
        max_arity = arity;
      }

  /* Declare the function that gets function j in normal form */
//  fprintf(f,  "static ATermAppl rewr_%i_nnf(ATermAppl);\n",j);

      int aux = 0;
      for (int a=0; a<=arity; a++)
      {
	int b = (a<=NF_MAX_ARITY)?a:0;
	for (unsigned int nfs=0; (nfs >> b) == 0; nfs++)
        {
  fprintf(f,  "static ATermAppl rewr_%i_%i_%u(",j,a,nfs);
          for (int i=0; i<a; i++)
          {
  fprintf(f,  (i==0)?"ATermAppl arg%i":", ATermAppl arg%i",i);
          }
  fprintf(f,  ");\n");

          if ( nfs > 0 )
          {
  fprintf(f,  "static ATermAppl rewr_%i_%i_0(",j+1+aux,a);
            for (int i=0; i<a; i++)
            {
  fprintf(f,  (i==0)?"ATermAppl arg%i":", ATermAppl arg%i",i);
            }
  fprintf(f,  ") { return rewr_%i_%i_%u(",j,a,nfs);
            for (int i=0; i<a; i++)
            {
  fprintf(f,  (i==0)?"arg%i":",arg%i",i);
            }
  fprintf(f,  "); }\n");
            aux++;
          }
        }
      }
//      printf("%i -> %i\n",j,aux);
      j += aux;
    /* } */
  }
  fprintf(f,  "\n\n");

  //
  // Print defs
  //
  fprintf(f,
      "#define ATisInt(x) (ATgetType(x) == AT_INT)\n"
#ifdef USE_VARAFUN_VALUE
      "#define isAppl(x) (ATgetAFun(x) != %li)\n"
      "\n", (long int) ATgetAFun(gsMakeDataVarId(gsString2ATermAppl("x"),gsMakeSortExprBool()))
#else
      "#define isAppl(x) (ATgetAFun(x) != varAFun)\n"
      "\n"
#endif
         );
  for (int i=0; i < num_opids; i++)
  {
#ifndef USE_INT2ATERM_VALUE
  fprintf(f,  "static ATerm int2ATerm%i;\n",i);
#endif
#ifndef USE_REWRAPPL_VALUE
  fprintf(f,  "static ATermAppl rewrAppl%i;\n",i);
#endif
  }
  fprintf(f,  "\n"
      "static AFun *apples;\n"
#ifndef USE_VARAFUN_VALUE
      "static AFun varAFun;\n"
#endif
         );
  for (int i=0; i<=max_arity; i++)
  {
#ifndef USE_APPL_VALUE
  fprintf(f,      "static AFun appl%i;\n",i);
#endif
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
//  fprintf(f,  "ftype1 *int2func;\n");

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
	      "  if ( n < substs_size )\n"
	      "  {\n"
	      "    substs[n] = NULL;\n"
	      "  }\n"
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
      "    apples = (AFun *) realloc(apples,num_apples*sizeof(AFun));\n"
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
      "static ATermAppl build%i(ATermAppl a",i);
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
/*      "    if ( ATisInt(ATgetArgument(a,0)) && \n"
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
      "    {\n"*/
#ifdef USE_APPL_VALUE
      "      return ATmakeAppl(%li,ATgetArgument(a,0)", (long int) get_appl_afun_value(i));
#else
      "      return ATmakeAppl(appl%i,ATgetArgument(a,0)", i);
#endif


    for (int j=0; j<i; j++)
    { fprintf(f, ",arg%i",j);
    }
    fprintf(f, ");\n"
//      "    }\n"
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
/*      "    if ( ATisInt(args[0]) && (ATgetInt((ATermInt) args[0]) < %i) )\n"
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
      "    }\n"*/
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
    /* if ( jittyc_eqns[j] != NULL ) */

    int arity = getArity(int2term[j]);

    gsfprintf(f,  "// %T\n",int2term[j]);
/*    fprintf(f,  "static ATermAppl rewr_%i_nnf(ATermAppl t)\n"
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
                "\n");*/

    for (int a=0; a<=arity; a++)
    {
    DECL_NFS_A(nfs_a,a);
    int b = (a<=NF_MAX_ARITY)?a:0;
    for (unsigned int nfs=0; (nfs >> b) == 0; nfs++)
    {
      fprintf(f,  "static ATermAppl rewr_%i_%i_%u(",j,a,nfs);
      for (int i=0; i<a; i++)
      {
        fprintf(f,  (i==0)?"ATermAppl arg%i":", ATermAppl arg%i",i);
      }
      fprintf(f,  ")\n"
                  "{\n"
             );

/*      fprintf(f,  "  ATfprintf(stderr,\"rewr_%i_%i_%i(",j,a,nfs);
      for (int i=0; i<a; i++)
      {
        fprintf(f,  (i==0)?"%%t":", %%t");
      }
      gsfprintf(f,  ")  %P: %P\\n\"",int2term[j],gsGetSort(int2term[j]));
      for (int i=0; i<a; i++)
      {
        fprintf(f,  ", arg%i",i);
      }
      fprintf(f,  ");\n");*/


      // Implement strategy
      if ( jittyc_eqns[j] != NULL )
      {
        set_nfs_array_value(nfs_a,a,nfs);
        implement_strategy(f,create_strategy(jittyc_eqns[j],j,a,nfs_a),a,1,j,nfs);
      } else {
	bool used[a];
	for (int k=0; k<a; k++)
	{
		used[k] = ((nfs & (1 << k)) != 0);
	}
	finish_function(f,a,j,used);
      }


  fprintf(f,                 "}\n");
    }
    FREE_NFS_A(nfs_a);
    }
    if ( arity > NF_MAX_ARITY )
	    arity = NF_MAX_ARITY;
//    printf("%i -> %u\n",j,(1 << (arity+1)) - arity - 2);
    j += (1 << (arity+1)) - arity - 2; // 2^(arity+1) - arity - 2
    fprintf(f,  "\n");
  }

  fprintf(f,  "void rewrite_init()\n"
      "{\n"
#ifndef USE_VARAFUN_VALUE
      "  varAFun = ATmakeAFun(\"DataVarId\", 2, ATfalse);\n"
      "  ATprotectAFun(varAFun);\n"
      "\n"
#endif
      "  apples = NULL;\n"
      "  getAppl(%i);\n",
      max_arity
         );
#ifndef USE_APPL_VALUE
  for (int i=0; i<=max_arity; i++)
  {
  fprintf(f,  "  appl%i = apples[%i];\n",i,i);
  }
#endif
  fprintf(f,  "\n");
  for (int i=0; i < num_opids; i++)
  {
#ifndef USE_INT2ATERM_VALUE
  fprintf(f,  "  int2ATerm%i = (ATerm) ATmakeInt(%i);\n",i,i);
  fprintf(f,  "  ATprotect(&int2ATerm%i);\n",i);
#endif
#ifndef USE_REWRAPPL_VALUE
  #ifdef USE_APPL_VALUE
    #ifdef USE_INT2ATERM_VALUE
  fprintf(f,  "  rewrAppl%i = ATmakeAppl(%li,(ATerm) %p);\n",i,get_appl_afun_value(0),get_int2aterm_value(i));
    #else
  fprintf(f,  "  rewrAppl%i = ATmakeAppl(%li,int2ATerm%i);\n",i,get_appl_afun_value(0),i);
    #endif
  #else
    #ifdef USE_INT2ATERM_VALUE
  fprintf(f,  "  rewrAppl%i = ATmakeAppl(appl0,(ATerm) %p);\n",i,get_int2aterm_value(i));
    #else
  fprintf(f,  "  rewrAppl%i = ATmakeAppl(appl0,int2ATerm%i);\n",i,i);
    #endif
  #endif
  fprintf(f,  "  ATprotectAppl(&rewrAppl%i);\n",i);
#endif
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
/*  fprintf(f,  "  int2func = (ftype1 *) malloc(%i*sizeof(ftype1));\n",num_opids);
  for (j=0;j < num_opids;j++)
  { gsfprintf(f,  "  int2func[%i] = rewr_%i_nnf; // %T\n",j,j,int2term[j]);
  }
  fprintf(f,  "\n");*/
  for (int i=0;i<=max_arity;i++)
  {
  fprintf(f,  "  int2func%i = (ftype%i *) malloc(%i*sizeof(ftype%i));\n",i,i,num_opids,i);
  for (j=0;j < num_opids;j++)
  {
    int arity = getArity(int2term[j]);
    if ( i <= arity )
    {
      gsfprintf(f,  "  int2func%i[%i] = rewr_%i_%i_0;\n",i,j,j,i);
      if ( i <= NF_MAX_ARITY )
      {
        for (int k=(1 << i)-i-1; k<(1 << (i+1))-i-2; k++)
        {
          gsfprintf(f,  "  int2func%i[%i] = rewr_%i_%i_0;\n",i,j+1+k,j+1+k,i);
        }
      }
    }
    if ( arity > NF_MAX_ARITY )
      arity = NF_MAX_ARITY;
//    printf("%i -> %u\n",j,(1 << (arity+1)) - arity - 2);
    j += (1 << (arity+1)) - arity - 2; // 2^(arity+1) - arity - 2
  }
  }
  fprintf(f,  "}\n"
      "\n"
      "ATermAppl rewrite(ATermAppl t)\n"
      "{\n"
//      "  ATfprintf(stderr,\"rewrite: %%t\\n\",t);\n"
      "  if ( isAppl(t) )\n"
      "  {\n"
      "    ATerm head = ATgetArgument(t,0);\n"
      "    if ( ATisInt(head) )\n"
      "    {\n"
      "      long function_index = ATgetInt((ATermInt)head);\n"
      "      if ( function_index < %i )\n"
      "      {\n"
      "        switch ( ATgetArity(ATgetAFun(t))-1 )\n"
      "        {\n",
      num_opids
	 );
  for (int i=0; i<=max_arity; i++)
  {
    fprintf(f,
      "          case %i:\n"
      "            assert(int2func%i[function_index] != NULL);\n"
      "            return int2func%i[function_index](",
         i,i,i
	 );
    for (int j=0; j<i; j++)
    {
      if ( j > 0 )
        fprintf(f,",");
      fprintf(f,"(ATermAppl) ATgetArgument(t,%i)",j+1);
    }
    fprintf(f,
      ");\n"
	 );
  }
  fprintf(f,
      "          default:\n"
      "            fprintf(stderr,\"too many arguments for function; rewriter failed\\n\");\n"
      "            exit(1);\n"
      "        }\n"
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
      "      long arity_u;\n"
      "      if ( isAppl(u) )\n"
      "      {\n"
      "        head = ATgetArgument((ATermAppl) u,0);\n"
      "        arity_u = ATgetArity(ATgetAFun((ATermAppl) u));\n"
      "      } else {\n"
      "        head = u;\n"
      "        arity_u = 1;\n"
      "      }\n"
      "      ATerm args[arity_u+arity_t-1];\n"
      "      long function_index;\n"
      "      if ( ATisInt(head) && ((function_index = ATgetInt((ATermInt) head)) < %i) )\n"
      "      {\n"
      "        for (int i=1; i<arity_u; i++)\n"
      "        {\n"
      "          args[i] = ATgetArgument((ATermAppl) u,i);\n"
      "        }\n"
      "        int k = arity_u;\n"
      "        for (int i=1; i<arity_t; i++,k++)\n"
      "        {\n"
      "          args[k] = ATgetArgument((ATermAppl) t,i);\n"
      "        }\n"
      "        switch ( arity_u+arity_t-2 )\n"
      "        {\n",
      num_opids
	 );
  for (int i=0; i<=max_arity; i++)
  {
    fprintf(f,
      "          case %i:\n"
      "            assert(int2func%i[function_index] != NULL);\n"
      "            return int2func%i[function_index](",
         i,i,i
	 );
    for (int j=0; j<i; j++)
    {
      if ( j > 0 )
        fprintf(f,",");
      fprintf(f,"(ATermAppl) args[%i]",j+1);
    }
    fprintf(f,
      ");\n"
	 );
  }
  fprintf(f,
      "          default:\n"
      "            fprintf(stderr,\"too many arguments for function; rewriter failed\\n\");\n"
      "            exit(1);\n"
      "        }\n"
      "      } else {\n"
      "        args[0] = head;\n"
      "        for (int i=1; i<arity_u; i++)\n"
      "        {\n"
      "          args[i] = (ATerm) rewrite((ATermAppl) ATgetArgument((ATermAppl) u,i));\n"
      "        }\n"
      "        int k = arity_u;\n"
      "        for (int i=1; i<arity_t; i++,k++)\n"
      "        {\n"
      "          args[k] = (ATerm) rewrite((ATermAppl) ATgetArgument((ATermAppl) t,i));\n"
      "        }\n"
      "        return ATmakeApplArray(getAppl(arity_u+arity_t-2),args);\n"
      "      }\n"
      "    }\n"
      "  } else {\n"
      "    ATermAppl r=(ATermAppl) get_subst(t);\n"
      "    return r;\n"
      "  }\n"
      "}\n"
         );

  fclose(f);
  gsVerboseMsg("compiling rewriter...\n");
  sprintf(t,JITTYC_COMPILE_COMMAND,s);
  gsVerboseMsg("%s\n",t);
  if ( system(t) != 0 )
  {
    gsErrorMsg("could not compile rewriter\n");
    unlink(file_c);
    exit(1);
  }

  gsVerboseMsg("linking rewriter...\n");
  sprintf(t,JITTYC_LINK_COMMAND,s,s);
  gsVerboseMsg("%s\n",t);
  if ( system(t) != 0 )
  {
    gsErrorMsg("could not link rewriter\n");
    unlink(file_o);
    unlink(file_c);
    exit(1);
  }

  sprintf(t,"./%s.so",s);
  if ( (h = dlopen(t,RTLD_NOW)) == NULL )
  {
    gsErrorMsg("cannot load rewriter: %s\n",dlerror());
    unlink(file_so);
    unlink(file_o);
    unlink(file_c);
    exit(1);
  }
  so_rewr_init = (void (*)()) dlsym(h,"rewrite_init");
  if ( so_rewr_init    == NULL ) gsErrorMsg("%s\n",dlerror());
  so_rewr = (ATermAppl (*)(ATermAppl)) dlsym(h,"rewrite");
  if ( so_rewr         == NULL ) gsErrorMsg("%s\n",dlerror());
  so_set_subst = (void (*)(ATermAppl, ATerm)) dlsym(h,"set_subst");
  if ( so_set_subst    == NULL ) gsErrorMsg("%s\n",dlerror());
  so_get_subst = (ATerm (*)(ATermAppl)) dlsym(h,"get_subst");
  if ( so_get_subst    == NULL ) gsErrorMsg("%s\n",dlerror());
  so_clear_subst = (void (*)(ATermAppl)) dlsym(h,"clear_subst");
  if ( so_clear_subst  == NULL ) gsErrorMsg("%s\n",dlerror());
  so_clear_substs = (void (*)()) dlsym(h,"clear_substs");
  if ( so_clear_substs == NULL ) gsErrorMsg("%s\n",dlerror());
  if ( (so_rewr_init    == NULL ) ||
       (so_rewr         == NULL ) ||
       (so_set_subst    == NULL ) ||
       (so_get_subst    == NULL ) ||
       (so_clear_subst  == NULL ) ||
       (so_clear_substs == NULL ) )
  {
    gsErrorMsg("cannot load rewriter functions\n");
    unlink(file_so);
    unlink(file_o);
    unlink(file_c);
    exit(1);
  }

  so_rewr_init();

  free(t);
  free(s);
}

static void cleanup_file(char *f)
{
  if ( unlink(f) )
  {
	  fprintf(stderr,"unable to remove file %s: %s\n",f,strerror(errno));
  }
  free(f);
}

RewriterCompilingJitty::RewriterCompilingJitty(mcrl2::data::data_specification DataSpec)
{
  term2int = ATtableCreate(100,75);
  initialise_common();
  CompileRewriteSystem(DataSpec);
#ifndef NDEBUG
  if (!gsDebug)
#endif
  { cleanup_file(file_c);
    cleanup_file(file_o);
    cleanup_file(file_so);
  }
}

RewriterCompilingJitty::~RewriterCompilingJitty()
{
  finalise_common();
  ATtableDestroy(term2int);
#ifndef NDEBUG
  if (gsDebug)
  { cleanup_file(file_c);
    cleanup_file(file_o);
    cleanup_file(file_so);
  }
#endif
}

ATermList RewriterCompilingJitty::rewriteInternalList(ATermList l)
{
  if (l==ATempty)
  { return ATempty;
  }

  return ATinsertA(
           rewriteInternalList(ATgetNext(l)),
           so_rewr(ATAgetFirst(l)));
}

ATermAppl RewriterCompilingJitty::rewrite(ATermAppl Term)
{
        /*gsVerboseMsg("Rewriting term: %T\n\n", Term);
        gsVerboseMsg("toRewriteFormat(Term): %T\n\n", toRewriteFormat(Term));
        gsVerboseMsg("fromInner(toRewriteFormat(Term)): %T\n\n\n", fromRewriteFormat(toRewriteFormat(Term)));*/
  return fromRewriteFormat((ATerm) so_rewr((ATermAppl) toRewriteFormat(Term)));
}

ATerm RewriterCompilingJitty::rewriteInternal(ATerm Term)
{
/*gsfprintf(stderr,"in: %T\n",Term);
gsfprintf(stderr,"in: %T\n",fromRewriteFormat(Term));
gsfprintf(stderr,"in: %P\n",fromRewriteFormat(Term));*/
  ATerm a = (ATerm) so_rewr((ATermAppl) Term);
/*gsfprintf(stderr,"out: %T\n",a);
gsfprintf(stderr,"out: %T\n",fromRewriteFormat(a));
gsfprintf(stderr,"out: %P\n",fromRewriteFormat(a));*/
  return a;
}

void RewriterCompilingJitty::setSubstitutionInternal(ATermAppl Var, ATerm Expr)
{
  so_set_subst(Var,Expr);
}

ATerm RewriterCompilingJitty::getSubstitutionInternal(ATermAppl Var)
{
  return so_get_subst(Var);
}

void RewriterCompilingJitty::clearSubstitution(ATermAppl Var)
{
  so_clear_subst(Var);
}

void RewriterCompilingJitty::clearSubstitutions()
{
  so_clear_substs();
}

RewriteStrategy RewriterCompilingJitty::getStrategy()
{
	return GS_REWR_JITTYC;
}

#endif
