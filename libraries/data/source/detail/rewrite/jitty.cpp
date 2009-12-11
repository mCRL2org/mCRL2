// Author(s): Muck van Weerdenburg
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file jitty.cpp

#include "boost.hpp" // precompiled headers

#include "mcrl2/data/detail/rewrite/jitty.h"
// #include "mcrl2/data/data_equation.h"

#define NAME std::string("rewr_jitty")

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <stdexcept>
#include "aterm2.h"
#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/detail/struct.h"
#include "mcrl2/core/aterm_ext.h"
#include "mcrl2/exception.h"
#include "boost/config.hpp"

using namespace mcrl2::core;
using namespace mcrl2::core::detail;

namespace mcrl2 {
  namespace data {
    namespace detail {

static AFun nilAFun;
static AFun opidAFun;
static unsigned int is_initialised = 0;

#define ATAgetArgument(x,y) ((ATermAppl) ATgetArgument(x,y))
#define ATLgetArgument(x,y) ((ATermList) ATgetArgument(x,y))
#define ATAgetFirst(x) ((ATermAppl) ATgetFirst(x))
#define ATLgetFirst(x) ((ATermList) ATgetFirst(x))
#define gsIsOpId(x) (ATgetAFun(x) == opidAFun)

#define ATisAppl(x) (ATgetType(x) == AT_APPL)
#define ATisList(x) (ATgetType(x) == AT_LIST)
#define ATisInt(x) (ATgetType(x) == AT_INT)

#define is_nil(x) (ATisList(x)?false:(ATgetAFun((ATermAppl) x) == nilAFun))

#if defined(BOOST_WINDOWS) || defined(BOOST_INTEL_WIN) || (defined(BOOST_GCC) && BOOST_WINDOWS)
#include "malloc.h"
#define SYSTEM_SPECIFIC_ALLOCA(T, n) static_cast< T* >(_alloca(n * sizeof(T))), n
#else
#define SYSTEM_SPECIFIC_ALLOCA(T, n) static_cast< T* >(alloca(n * sizeof(T))), n
#endif


// Special purpose minimal dynarray implementation (such as in the proposal for TR2)
// \deprecated
// This is only here for historic reasons; this is C99 code that is put in a
// C++ class and compiled as C++ code. Since C++ does not now VLAs the only
// option to save performance of the rewriter is to use stack allocation.
// Proper encapsulating stack allocation is not possible to the best of my
// knowledge. For the new C++ standard a proposal for dynarray was made. The
// implementation below is just a poor compromise that only vaguely resembles
// the dynarray interface, but it represents the same concept.
template < typename T >
class dynarray {

  private:

    T* m_elements;

  public:

    typedef size_t size_type;

  public:

    dynarray(T* p, size_t) : m_elements(p)
    { }

    dynarray(T* p, size_t n, T v) : m_elements(p)
    {
      std::fill(m_elements, m_elements + n, v);
    }

    T& operator[](size_type n)
    {
      return m_elements[n];
    }

    T* data()
    {
      return m_elements;
    }
};

static void initialise_common()
{
  if ( is_initialised == 0 )
  {
    nilAFun = ATgetAFun(gsMakeNil());
    ATprotectAFun(nilAFun);
    opidAFun = ATgetAFun(static_cast<ATermAppl>(sort_bool::true_()));
    ATprotectAFun(opidAFun);
  }

  is_initialised++;
}

static void finalise_common()
{
  assert(is_initialised > 0 );
  is_initialised--;

  if ( is_initialised == 0 )
  {
    ATunprotectAFun(opidAFun);
    ATunprotectAFun(nilAFun);
  }
}


ATerm RewriterJitty::OpId2Int(ATermAppl Term, bool add_opids)
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


static AFun *apples;
static unsigned int num_apples = 0;

#define getAppl(x) ((x < num_apples)?apples[x]:getAppl2(x))
static AFun getAppl2(unsigned int arity)
{
  unsigned int old_apples = num_apples;

  if ( old_apples == 0 )
  {
    while ( num_apples <= arity )
    {
      num_apples += 10;
    }

    apples = (AFun *) malloc(num_apples*sizeof(AFun));
  } else if ( old_apples <= arity )
  {
    while ( num_apples <= arity )
    {
      num_apples += 10;
    }

    apples = (AFun *) realloc(apples,num_apples*sizeof(AFun));
  }

  for (unsigned int i=old_apples; i<num_apples; i++)
  {
    apples[i] = ATmakeAFun("@REWR@",i,ATfalse);
    ATprotectAFun(apples[i]);
  }

  return apples[arity];
}

ATermAppl RewriterJitty::toInner(ATermAppl Term, bool add_opids)
{
  ATermList l;

  if ( gsIsNil(Term) || gsIsDataVarId(Term) )
  {
    return Term;
  }

  l = ATmakeList0();

        if (!gsIsDataAppl(Term))
        {
                if ( gsIsOpId(Term) )
          {
            l = ATinsert(l,(ATerm) OpId2Int(Term,add_opids));
          }else {
            l = ATinsert(l,(ATerm) Term);
          }
        } else {
                ATermAppl arg0 = toInner(ATAgetArgument(Term, 0), add_opids);
                // Reflect the way of encoding the other arguments!
                if ( gsIsNil(arg0) || gsIsDataVarId(arg0) )
                {
                        l = ATinsert(l, (ATerm) arg0);
                }else {
                        int arity = ATgetArity(ATgetAFun(arg0));
                        for (int i = 0; i < arity; ++i)
                        {
                                l = ATinsert(l, ATgetArgument(arg0, i));
                        }
                }
                for (ATermList args = ATLgetArgument((ATermAppl) Term,1); !ATisEmpty(args) ; args = ATgetNext(args))
                {
                        l = ATinsert(l,(ATerm) toInner((ATermAppl) ATgetFirst(args),add_opids));
                }

                l = ATreverse(l);
        }

  return ATmakeApplList(getAppl(ATgetLength(l)),l);
}

ATermAppl RewriterJitty::fromInner(ATermAppl Term)
{
  ATermAppl a;

// ATfprintf(stderr,"in: %t\n\n",Term);
  if ( gsIsDataVarId(Term) )
  {
// ATfprintf(stderr,"out: %t\n\n",Term);
    return Term;
  }

  int arity = ATgetArity(ATgetAFun(Term));
  ATerm t = ATgetArgument(Term,0);
  if ( ATisInt(t) )
  {
    a = int2term[ATgetInt((ATermInt) t)];
  } else {
    a = (ATermAppl) t;
  }

        if(gsIsOpId(a) || gsIsDataVarId(a))
        {
                int i = 1;
                ATermAppl sort = ATAgetArgument(a, 1);
                while(gsIsSortArrow(sort) && (i < arity))
                {
                        ATermList sort_dom = ATLgetArgument(sort, 0);
                        ATermList list = ATmakeList0();
                        while(!ATisEmpty(sort_dom))
                        {
                                assert(i < arity);
                                list = ATinsert(list, (ATerm) fromInner(ATAgetArgument(Term,i)));
                                sort_dom = ATgetNext(sort_dom);
                                ++i;
                        }
                        list = ATreverse(list);
                        a = gsMakeDataAppl(a, list);
                        sort = ATAgetArgument(sort, 1);
                }
        }

// ATfprintf(stderr,"to_out: %t\n\n",a);

        return a;
}

static ATermList get_vars(ATerm a)
{
  if ( gsIsDataVarId((ATermAppl) a) )
  {
    return ATmakeList1(a);
  } else {
    ATermList l = ATmakeList0();
    ATermList m = ATgetArguments((ATermAppl) a);
    for (;!ATisEmpty(m); m=ATgetNext(m))
    {
      l = ATconcat(l,get_vars(ATgetFirst(m)));
    }
    return l;
  }
}

static ATermList create_strategy(ATermList rules)
{
  ATermList strat = ATmakeList0();
  unsigned int arity;

//gsMessage("rules: %T\n\n",rules);
  unsigned int max_arity = 0;
  for (ATermList l=rules; !ATisEmpty(l); l=ATgetNext(l))
  {
    if ( ATgetArity(ATgetAFun(ATAelementAt(ATLgetFirst(l),2))) > max_arity + 1 )
    {
      max_arity = ATgetArity(ATgetAFun(ATAelementAt(ATLgetFirst(l),2)))-1;
    }
  }
  dynarray< bool > used(SYSTEM_SPECIFIC_ALLOCA(bool, max_arity), false);

  arity = 0;
  while ( !ATisEmpty(rules) )
  {
    ATermList l = ATmakeList0();
    ATermList m = ATmakeList0();
          dynarray< int > args(SYSTEM_SPECIFIC_ALLOCA(int, arity), -1);
//printf("arity = %i\n",arity);

    for (; !ATisEmpty(rules); rules=ATgetNext(rules))
    {
      if ( ATgetArity(ATgetAFun(ATAelementAt(ATLgetFirst(rules),2))) == arity + 1 )
      {
        ATermAppl cond = ATAelementAt(ATLgetFirst(rules),1);
        /*ATermList vars = gsIsNil(cond)?ATmakeList1((ATerm) ATmakeList0()):ATmakeList1((ATerm) get_vars((ATerm) cond));*/
        ATermList vars = ATmakeList1((ATerm) get_vars((ATerm) cond)); // JK 15/10/2009 condition is always a data expression
        ATermAppl pars = ATAelementAt(ATLgetFirst(rules),2);

//gsfprintf(stderr,"rule: %T\n",ATgetFirst(rules));
//gsfprintf(stderr,"rule: %T\n",ATAelementAt(ATgetFirst(rules),2));
//gsfprintf(stderr,"rule: "); PrintPart_C(stderr,fromInner(ATAelementAt(ATgetFirst(rules),2))); gsfprintf(stderr,"\n");
//gsprintf("pars: %T\n",pars);

                          dynarray< bool > bs(SYSTEM_SPECIFIC_ALLOCA(bool, arity), false);

        for (unsigned int i = 0; i < arity; i++)
        {
          if ( !gsIsDataVarId(ATAgetArgument(pars,i+1)) )
          {
            bs[i] = true;
            ATermList evars = get_vars(ATgetArgument(pars,i+1));
            for (; !ATisEmpty(evars); evars=ATgetNext(evars))
            {
            int j=0;
            for (ATermList o=ATgetNext(vars); !ATisEmpty(o); o=ATgetNext(o))
            {
              if ( ATindexOf(ATLgetFirst(o),ATgetFirst(evars),0) >= 0 )
              {
                  bs[j] = true;
              }
              j++;
            }
            }
            vars = ATappend(vars,(ATerm) get_vars(ATgetArgument(pars,i+1)));
          } else {
            int j = -1;
            bool b = false;
            for (ATermList o=vars; !ATisEmpty(o); o=ATgetNext(o))
            {
              if ( ATindexOf(ATLgetFirst(o),ATgetArgument(pars,i+1),0) >= 0 )
              {
                if ( j >= 0 )
                  bs[j] = true;
                b = true;
              }
              j++;
            }
            if ( b )
            {
              bs[i] = true;
            }
            vars = ATappend(vars,(ATerm) get_vars(ATgetArgument(pars,i+1)));
          }
        }

        ATermList deps = ATmakeList0();
        for (unsigned int i = 0; i < arity; i++)
        {
          if ( bs[i] && !used[i] )
          {
            deps = ATinsert(deps,(ATerm) ATmakeInt(i));
            args[i] += 1;
//fprintf(stderr,"dep of arg %i\n",i);
          }
        }
        deps = ATreverse(deps);

        m = ATinsert(m,(ATerm) ATmakeList2((ATerm) deps,ATgetFirst(rules)));
//gsfprintf(stderr,"\n");
      } else {
        l = ATinsert(l,ATgetFirst(rules));
      }
    }

    while ( !ATisEmpty(m) )
    {
      ATermList m2 = ATmakeList0();
      for (; !ATisEmpty(m); m=ATgetNext(m))
      {
        if ( ATisEmpty(ATLgetFirst(ATLgetFirst(m))) )
        {
//gsprintf("add: %T\n",ATgetFirst(ATgetNext(ATLgetFirst(m))));
          strat = ATinsert(strat, ATgetFirst(ATgetNext(ATLgetFirst(m))));
        } else {
          m2 = ATinsert(m2,ATgetFirst(m));
        }
      }
      m = ATreverse(m2);

      if ( ATisEmpty(m) )
      {
        break;
      }

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

      if ( maxidx >= 0 )
      {
        args[maxidx] = -1;
        used[maxidx] = true;

        ATermInt k = ATmakeInt(maxidx);
//gsprintf("add: %T\n",k);
        strat = ATinsert(strat,(ATerm) k);
        m2 = ATmakeList0();
        for (; !ATisEmpty(m); m=ATgetNext(m))
        {
          m2 = ATinsert(m2,(ATerm) ATinsert(ATgetNext(ATLgetFirst(m)),(ATerm) ATremoveElement(ATLgetFirst(ATLgetFirst(m)),(ATerm) k)));
        }
        m = ATreverse(m2);
      }
    }

    rules = ATreverse(l);
    arity++;
  }

  //XXX Add unused, so we don't need to check all args during rewriting

//gsfprintf(stderr,"strat: %T\n\n",ATreverse(strat));

  return ATreverse(strat);
}

RewriterJitty::RewriterJitty(const data_specification &DataSpec)
{
  ATermList n;
  ATermInt i;

  initialise_common();

  jitty_eqns = ATtableCreate(100,50); // XXX would be nice to know the number op OpIds
  term2int = ATtableCreate(100,50);

  num_opids = 0;
  max_vars = 0;
        need_rebuild = false;

  jitty_true = toInner(sort_bool::true_(),true);
  ATprotectAppl(&jitty_true);

/*	l = opid_eqns;
  for (; !ATisEmpty(l); l=ATgetNext(l))
  {
    // XXX only adds the last rule where lhs is an opid; this might go "wrong" if this rule is removed later
    ATtablePut(jitty_eqns,OpId2Int(ATAgetArgument(ATAgetFirst(l),2),true),(ATerm) ATmakeList1((ATerm) ATmakeList4((ATerm) ATmakeList0(),(ATerm) toInner(ATAgetArgument(ATAgetFirst(l),1),true),(ATerm) toInner(ATAgetArgument(ATAgetFirst(l),2),true),(ATerm) toInner(ATAgetArgument(ATAgetFirst(l),3),true))));
  }

  l = dataappl_eqns;*/
  // l = reinterpret_cast< ATermList >(static_cast< ATerm >(atermpp::arg4(DataSpec).argument(0)));
  const data_specification::equations_const_range l = DataSpec.equations();
  for (atermpp::set< data_equation >::const_iterator j=l.begin();
          j!=l.end(); ++j)
  {
    try
    {
      CheckRewriteRule(*j);
    } catch ( std::runtime_error &e )
    {
      gsWarningMsg("%s\n",e.what());
      continue;
    }

    ATermAppl u = toInner((ATermAppl)j->lhs(),true);

    if ( (n = (ATermList) ATtableGet(jitty_eqns,ATgetArgument(u,0))) == NULL )
    {
      n = ATmakeList0();
    }
    if ( j->variables().size() > max_vars)
    {
      max_vars = j->variables().size();
    }
    n = ATinsert(n,(ATerm) ATmakeList4((ATerm) static_cast<ATermList>(j->variables()),
                                       (ATerm) toInner(j->condition(),true),
                                       (ATerm) u,
                                       (ATerm) toInner(j->rhs(),true)));
    ATtablePut(jitty_eqns,ATgetArgument(u,0),(ATerm) n);
  }

  int2term = (ATermAppl *) malloc(num_opids*sizeof(ATermAppl));
  memset(int2term,0,num_opids*sizeof(ATermAppl));
  ATprotectArray((ATerm *) int2term,num_opids);
  jitty_strat = (ATermList *) malloc(num_opids*sizeof(ATermList));
  memset(jitty_strat,0,num_opids*sizeof(ATermList));
  ATprotectArray((ATerm *) jitty_strat,num_opids);


  ATermList l1 = ATtableKeys(term2int);
  for (; !ATisEmpty(l1); l1=ATgetNext(l1))
  {
    ATermList n;

    i = (ATermInt) ATtableGet(term2int,ATgetFirst(l1));
    int2term[ATgetInt(i)] = ATAgetFirst(l1);
    if ( (n = (ATermList) ATtableGet(jitty_eqns,(ATerm) i)) == NULL )
    {
      jitty_strat[ATgetInt(i)] = NULL;
    } else {
//gsfprintf(stderr,"%T\n",ATAgetFirst(l1));
      jitty_strat[ATgetInt(i)] = create_strategy(ATreverse(n));
    }
  }
}

RewriterJitty::~RewriterJitty()
{
  ATunprotectArray((ATerm *) jitty_strat);
  free(jitty_strat);
  ATunprotectArray((ATerm *) int2term);
  free(int2term);
  ATunprotectAppl(&jitty_true);
  ATtableDestroy(jitty_eqns);
  ATtableDestroy(term2int);

  finalise_common();
}

bool RewriterJitty::addRewriteRule(ATermAppl Rule)
{
  try
  {
    CheckRewriteRule(Rule);
  } catch ( std::runtime_error &e ) {
    gsWarningMsg("%s\n",e.what());
    return false;
  }

  ATermAppl u = (ATermAppl) toRewriteFormat(ATAgetArgument(Rule,2));
  ATermList n = (ATermList) ATtableGet(jitty_eqns,ATgetArgument(u,0));
  if ( n == NULL )
  {
    n = ATmakeList0();
  }
  if ( ATgetLength(ATLgetArgument(Rule,0)) > max_vars)
  {
    max_vars = ATgetLength(ATLgetArgument(Rule,0));
  }
  n = ATinsert(n,(ATerm) ATmakeList4(ATgetArgument(Rule,0),toRewriteFormat(ATAgetArgument(Rule,1)),(ATerm) u,toRewriteFormat(ATAgetArgument(Rule,3))));
  ATtablePut(jitty_eqns,ATgetArgument(u,0),(ATerm) n);

  jitty_strat[ATgetInt((ATermInt) ATgetArgument(u,0))] = NULL; //create_strategy(n);
  need_rebuild = true;

  return true;
}

bool RewriterJitty::removeRewriteRule(ATermAppl Rule)
{
  ATermAppl u = (ATermAppl) toRewriteFormat(ATAgetArgument(Rule,2));
  ATermList n = (ATermList) ATtableGet(jitty_eqns,ATgetArgument(u,0));
  if ( n == NULL )
  {
    return true;
  }
  ATermList e = ATmakeList4(ATgetArgument(Rule,0),toRewriteFormat(ATAgetArgument(Rule,1)),(ATerm) u,(ATerm) toRewriteFormat(ATAgetArgument(Rule,3)));

        ATermList l = n;
  for (n = ATmakeList0(); !ATisEmpty(l); l=ATgetNext(l))
  {
    if ( !ATisEqual(e,ATgetFirst(l)) )
    {
      ATinsert(n,ATgetFirst(l));
    }
  }
  ATermInt i = (ATermInt) ATgetArgument(u,0);
  if ( ATisEmpty(n) )
  {
    ATtableRemove(jitty_eqns,(ATerm) i);
    jitty_strat[ATgetInt(i)] = NULL;
  } else {
    ATtablePut(jitty_eqns,(ATerm) i,(ATerm) n);
    jitty_strat[ATgetInt(i)] = NULL;//create_strategy(n);
    need_rebuild = true;
  }

  return true;
}

static ATerm subst_values(ATermAppl *vars, ATerm *vals, int len, ATerm t)
{
  if ( ATisInt(t) )
  {
    return t;
  } else if ( gsIsDataVarId((ATermAppl) t) )
  {
    for (int i=0; i<len; i++)
    {
      if ( ATisEqual(t,vars[i]) )
      {
        return vals[i];
      }
    }
    throw mcrl2::runtime_error(NAME + ": variable not assigned.");
  } else {
    unsigned int arity = ATgetArity(ATgetAFun((ATermAppl) t));
    unsigned int new_arity = arity;
    ATerm arg0 = subst_values(vars,vals,len,ATgetArgument((ATermAppl) t,0));
    if ( !(ATisInt(arg0) || gsIsDataVarId((ATermAppl) arg0)) )
    {
      new_arity += ATgetArity(ATgetAFun((ATermAppl) arg0))-1;
    }
    dynarray< ATerm > args(SYSTEM_SPECIFIC_ALLOCA(ATerm, new_arity));
    unsigned int i;
    if ( ATisInt(arg0) || gsIsDataVarId((ATermAppl) arg0) )
    {
      args[0] = arg0;
      i = 1;
    } else {
      i = 0;
      unsigned int arg0_arity = ATgetArity(ATgetAFun((ATermAppl) arg0));
      while ( i < arg0_arity )
      {
        args[i] = ATgetArgument((ATermAppl) arg0,i);
        i++;
      }
    }

    for (unsigned int j=1; j<arity; j++)
    {
      args[i] = subst_values(vars,vals,len,ATgetArgument((ATermAppl) t,j));
      i++;
    }

    if ( arity == new_arity )
    {
      return (ATerm) ATmakeApplArray(ATgetAFun((ATermAppl) t),args.data());
    } else {
      return (ATerm) ATmakeApplArray(getAppl(new_arity),args.data());
    }
  }
}

static bool match_jitty(ATerm t, ATerm p, ATermAppl *vars, ATerm *vals, unsigned int *len)
{
//gsfprintf(stderr,"match_jitty(  %T  ,  %T  ,  %T   )\n\n",t,p,*vars);
        if ( ATisInt(p) )
  {
    return ATisEqual(p,t);
  } else if ( gsIsDataVarId((ATermAppl) p) )
  {
//		t = RWapplySubstitution(t); //XXX dirty (t is not a variable)
    for (unsigned int i=0; i<*len; i++)
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
  } else {
//		t = RWapplySubstitution(t); //XXX dirty (t is not a variable)
    if ( ATisInt(t) || gsIsDataVarId((ATermAppl) t) )
    {
      return false;
    }
/*		ATerm head = ATgetArgument((ATermAppl) t, 0);
    if ( !ATisInt(head) )
    {
      head = RWapplySubstitution(head);
      if ( ATisInt(head) )
      {
        t = (ATerm) ATsetArgument((ATermAppl) t,head,0);
      } else if ( !gsIsDataVarId((ATermAppl) head) )
      {
        int ar1 = ATgetArity(ATgetAFun((ATermAppl) head));
        int ar2 = ATgetArity(ATgetAFun((ATermAppl) t));

        if ( ar1+ar2-1 != ATgetArity(ATgetAFun(p)) )
        {
          return false;
        }

        for (int i=0; i<ar1; i++)
        {
          if ( !match_jitty(ATgetArgument((ATermAppl) head,i),ATgetArgument((ATermAppl) p,i),vars,vals,len) )
          {
            return false;
          }
        }
        for (int i=1; i<ar2; i++)
        {
          if ( !match_jitty(ATgetArgument((ATermAppl) t,i),ATgetArgument((ATermAppl) p,ar1+i-1),vars,vals,len) )
          {
            return false;
          }
        }
        return true;
      }
    }*/
    if ( !ATisEqualAFun(ATgetAFun((ATermAppl) p),ATgetAFun((ATermAppl) t)) )
    {
      return false;
    }

    int arity = ATgetArity(ATgetAFun((ATermAppl) p));

    for (int i=0; i<arity; i++)
    {
      if ( !match_jitty(ATgetArgument((ATermAppl) t,i),ATgetArgument((ATermAppl) p,i),vars,vals,len) )
      {
        return false;
      }
    }

    return true;
  }
}

ATermAppl RewriterJitty::rewrite_aux(ATermAppl Term)
{
// ATfprintf(stderr,"rewrite(%t)\n",fromInner(Term));
// ATfprintf(stderr,"rewrite(  %P  )\n\n",fromInner(Term));
  if ( gsIsDataVarId(Term) )
  {
//ATfprintf(stderr,"return %t\n",fromInner(Term));
//gsfprintf(stderr,"return1  %P\n\n",fromInner((ATermAppl) lookupSubstitution(Term)));
    return (ATermAppl) lookupSubstitution(Term);
  } else {
    ATerm op = ATgetArgument(Term,0);
    ATerm head = NULL;
    ATermList strat;
    unsigned int head_arity = 0;
    unsigned int arity = ATgetArity(ATgetAFun(Term));

    if ( !ATisInt(op) )
    {
      head = lookupSubstitution((ATermAppl) op);
      if ( ATisInt(head) )
      {
        op = head;
      } else if ( !gsIsDataVarId((ATermAppl) head) )
      {
        op = ATgetArgument((ATermAppl) head,0);
        head_arity = ATgetArity(ATgetAFun(head));
        arity = arity-1 + head_arity;
      }
    }

    dynarray< ATerm > rewritten(SYSTEM_SPECIFIC_ALLOCA(ATerm, arity));
    dynarray< ATermAppl > args(SYSTEM_SPECIFIC_ALLOCA(ATermAppl, arity));

    if ( head_arity > 0 )
    {
      head_arity--;
    }
    for (unsigned int i=1; i<arity; i++)
    {
      rewritten[i] = NULL;
      if ( i < head_arity+1 )
      {
        args[i] = ATAgetArgument((ATermAppl) head,i);
      } else {
        args[i] = ATAgetArgument(Term,i-head_arity);
      }
    }

    if ( ATisInt(op) && ((strat = jitty_strat[ATgetInt((ATermInt) op)]) != NULL) )
    {
//gsfprintf(stderr,"strat: %T\n\n",strat);
      for (; !ATisEmpty(strat); strat=ATgetNext(strat))
      {
//gsfprintf(stderr,"strat action: %T\n\n",ATgetFirst(strat));
        if ( ATisInt(ATgetFirst(strat)) )
        {
          unsigned int i = ATgetInt((ATermInt) ATgetFirst(strat))+1;
          if ( i < arity )
          {
            rewritten[i] = (ATerm) rewrite_aux(args[i]);
          } else {
            break;
          }
        } else {
          ATermList rule = ATLgetFirst(strat);
          ATermAppl lhs = ATAelementAt(rule,2);
          unsigned int rule_arity = ATgetArity(ATgetAFun(lhs));

          if ( rule_arity > arity )
          {
            break;
          }

          unsigned int max_len = ATgetLength(ATLgetFirst(rule));
                            dynarray< ATermAppl > vars(SYSTEM_SPECIFIC_ALLOCA(ATermAppl, max_len));
                            dynarray< ATerm > vals(SYSTEM_SPECIFIC_ALLOCA(ATerm, max_len));
          unsigned int len = 0;
          bool matches = true;

          for (unsigned int i=1; i<rule_arity; i++)
          {
            if ( !match_jitty((rewritten[i]==NULL)?((ATerm) args[i]):rewritten[i],ATgetArgument(lhs,i),vars.data(),vals.data(),&len) )
            {
              matches = false;
              break;
            }
          }

//if ( matches && !gsIsNil(ATAelementAt(rule,1)) )
//{
//gsfprintf(stderr,"%T --> %T (%T)\n\n",ATelementAt(rule,1),rewrite_aux((ATermAppl) subst_values(vars,vals,len,ATelementAt(rule,1))),jitty_true);
//}
          if ( matches && /*(gsIsNil(ATAelementAt(rule,1)) || */ATisEqual(rewrite_aux((ATermAppl) subst_values(vars.data(),vals.data(),len,ATelementAt(rule,1))),jitty_true)/*)*/ ) // JK 15/10/2009 Condition is always a data expression
          {
            ATermAppl rhs = ATAelementAt(rule,3);

            if ( arity == rule_arity )
            {
              return rewrite_aux((ATermAppl) subst_values(vars.data(),vals.data(),len,(ATerm) rhs));
            }

            unsigned int rhs_arity;
            unsigned int new_arity;
            ATerm arg0;

            if ( gsIsDataVarId(rhs) )
            {
              arg0 = subst_values(vars.data(),vals.data(),len,(ATerm) rhs);
              if ( gsIsDataVarId((ATermAppl) arg0) )
              {
                rhs_arity = 0;
                new_arity = 1+arity-rule_arity;
              } else {
                rhs_arity = ATgetArity(ATgetAFun((ATermAppl) arg0));
                new_arity = rhs_arity+arity-rule_arity;
              }
            } else {
              rhs_arity = ATgetArity(ATgetAFun(rhs));
              new_arity = rhs_arity+arity-rule_arity;
              arg0 = subst_values(vars.data(),vals.data(),len,ATgetArgument(rhs,0));
              if ( !(ATisInt(arg0) || gsIsDataVarId((ATermAppl) arg0)))
              {
                new_arity += ATgetArity(ATgetAFun((ATermAppl) arg0))-1;
              }
            }
                                    dynarray< ATerm > newargs(SYSTEM_SPECIFIC_ALLOCA(ATerm, new_arity));
            unsigned int i;
            if ( gsIsDataVarId(rhs) )
            {
              if ( gsIsDataVarId((ATermAppl) arg0) )
              {
                newargs[0] = arg0;
                i = 1;
              } else {
                i = 0;
                while ( i < rhs_arity )
                {
                  newargs[i] = ATgetArgument((ATermAppl) arg0,i);
                  i++;
                }
              }
            } else {
              if ( ATisInt(arg0) || gsIsDataVarId((ATermAppl) arg0))
              {
                newargs[0] = arg0;
                i = 1;
              } else {
                i = 0;
                unsigned int arg0_arity = ATgetArity(ATgetAFun((ATermAppl) arg0));
                while ( i < arg0_arity )
                {
                  newargs[i] = ATgetArgument((ATermAppl) arg0,i);
                  i++;
                }
              }
              for (unsigned int j=1; j<rhs_arity; j++)
              {
//gsfprintf(stderr,"pre %T\n\n",ATgetArgument(rhs,i));
                newargs[i] = subst_values(vars.data(),vals.data(),len,ATgetArgument(rhs,j));
                i++;
//gsfprintf(stderr,"post %T\n\n",args[i]);
              }
            }

            for (unsigned int j=0; j<arity-rule_arity; j++)
            {
              newargs[i] = (ATerm) args[rule_arity+j];
              i++;
            }

            ATermAppl a = ATmakeApplArray(getAppl(new_arity),newargs.data());

            ATermAppl aa = rewrite_aux(a);
// ATfprintf(stderr,"return %t\n",fromInner(aa));
//gsfprintf(stderr,"return2  %P\n\n",fromInner(aa));
            return aa;
          }

        }
      }
    }
//gsfprintf(stderr,"done with strat\n\n");

    rewritten[0] = op;
    for (unsigned int i=1; i<arity; i++)
    {
      if ( rewritten[i] == NULL )
      {
        rewritten[i] = (ATerm) rewrite_aux(args[i]);
      }
    }

    ATermAppl a = ATmakeApplArray(ATgetAFun(Term),rewritten.data());

//gsfprintf(stderr,"return %T\n\n",a);
//gsfprintf(stderr,"return3  %P\n\n",fromInner(a));
    return a;
  }
}

ATerm RewriterJitty::toRewriteFormat(ATermAppl Term)
{
  unsigned int old_opids = num_opids;
  ATermAppl a = toInner((ATermAppl) Term,true);
  if ( old_opids < num_opids )
  {
    ATunprotectArray((ATerm *) int2term);
    int2term = (ATermAppl *) realloc(int2term,num_opids*sizeof(ATermAppl));
    for (unsigned int k = old_opids; k < num_opids; k++) int2term[k] = NULL;
    ATprotectArray((ATerm *) int2term,num_opids);
    ATunprotectArray((ATerm *) jitty_strat);
    jitty_strat = (ATermList *) realloc(jitty_strat,num_opids*sizeof(ATermList));
    for (unsigned int k = old_opids; k < num_opids; k++) jitty_strat[k] = NULL;
    ATprotectArray((ATerm *) jitty_strat,num_opids);
    ATermList l = ATtableKeys(term2int);
    for (; !ATisEmpty(l); l=ATgetNext(l))
    {
      ATermInt i = (ATermInt) ATtableGet(term2int,ATgetFirst(l));
      if ( ((unsigned int) ATgetInt(i)) >= old_opids )
      {
        int2term[ATgetInt(i)] = ATAgetFirst(l);
        jitty_strat[ATgetInt(i)] = NULL;
      }
    }
  }

  return (ATerm) a;
}

ATermAppl RewriterJitty::fromRewriteFormat(ATerm Term)
{
  return fromInner((ATermAppl) Term);
}

ATermAppl RewriterJitty::rewrite(ATermAppl Term)
{
        /*gsVerboseMsg("Rewriting term: %T\n\n", Term);
        gsVerboseMsg("toRewriteFormat(Term): %T\n\n", toRewriteFormat(Term));
        gsVerboseMsg("fromInner(toRewriteFormat(Term)): %T\n\n\n", fromInner((ATermAppl) toRewriteFormat(Term)));*/
  return fromInner((ATermAppl) rewriteInternal(toRewriteFormat(Term)));
}

ATerm RewriterJitty::rewriteInternal(ATerm Term)
{
  if ( need_rebuild )
  {
    ATermList opids = ATtableKeys(jitty_eqns);
    for (; !ATisEmpty(opids); opids=ATgetNext(opids))
    {
      ATermInt i = (ATermInt) ATgetFirst(opids);
      if ( jitty_strat[ATgetInt(i)] == NULL )
      {
        jitty_strat[ATgetInt(i)] = create_strategy((ATermList) ATtableGet(jitty_eqns,(ATerm) i));
      }
    }
    need_rebuild = false;
  }
 // ATfprintf(stderr,"rewrite(%t)\n",fromInner((ATermAppl)Term));
  ATerm  aaa=(ATerm)rewrite_aux((ATermAppl) Term);
 // ATfprintf(stderr,"return(%t)\n",fromInner((ATermAppl)aaa));
  return aaa;
  // return (ATerm) rewrite_aux((ATermAppl) Term);
}

RewriteStrategy RewriterJitty::getStrategy()
{
  return GS_REWR_JITTY;
}
    }
  }
}
