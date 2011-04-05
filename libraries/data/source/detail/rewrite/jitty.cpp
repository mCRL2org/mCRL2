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

#define NAME std::string("rewr_jitty")

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <stdexcept>
#include "aterm2.h"
#include "mcrl2/core/detail/memory_utility.h"
#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/core/messaging.h"
#include "mcrl2/core/detail/struct_core.h"
#include <aterm_ext.h>
#include "mcrl2/exception.h"
#include "boost/config.hpp"
#include <iostream>

#include "mcrl2/data/data_expression.h"
#include "mcrl2/core/print.h"
#include "mcrl2/data/fresh_variable_generator.h"

#include "mcrl2/data/detail/enum/enumerator.h"
#include "mcrl2/data/detail/enum/standard.h"

using namespace mcrl2::core;
using namespace mcrl2::core::detail;

namespace mcrl2
{
namespace data
{
namespace detail
{

static AFun nilAFun;
static AFun opidAFun;
static unsigned int is_initialised = 0;

#define gsIsOpId(x) (ATgetAFun(x) == opidAFun)

#define is_nil(x) (ATisList(x)?false:(ATgetAFun((ATermAppl) x) == nilAFun))


static void initialise_common()
{
  if (is_initialised == 0)
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
  assert(is_initialised > 0);
  is_initialised--;

  if (is_initialised == 0)
  {
    ATunprotectAFun(opidAFun);
    ATunprotectAFun(nilAFun);
  }
}


ATerm RewriterJitty::OpId2Int(ATermAppl Term, bool add_opids)
{
  ATermInt i;

  if ((i = (ATermInt) ATtableGet(term2int,(ATerm) Term)) == NULL)
  {
    if (!add_opids)
    {
      return (ATerm) Term;
    }
    i = ATmakeInt(num_opids);
    ATtablePut(term2int,(ATerm) Term,(ATerm) i);
    num_opids++;
  }

  return (ATerm) i;
}


static AFun* apples;
static size_t num_apples = 0;

#define getAppl(x) ((x < num_apples)?apples[x]:getAppl2(x))
static AFun getAppl2(size_t arity)
{
  size_t old_apples = num_apples;

  if (old_apples == 0)
  {
    while (num_apples <= arity)
    {
      num_apples += 10;
    }

    apples = (AFun*) malloc(num_apples*sizeof(AFun));
  }
  else if (old_apples <= arity)
  {
    while (num_apples <= arity)
    {
      num_apples += 10;
    }

    apples = (AFun*) realloc(apples,num_apples*sizeof(AFun));
  }

  for (size_t i=old_apples; i<num_apples; i++)
  {
    apples[i] = ATmakeAFun("@REWR@",i,false);
    ATprotectAFun(apples[i]);
  }

  return apples[arity];
}

ATermAppl RewriterJitty::toInner(ATermAppl Term, bool add_opids)
{
  ATermList l;

  if (gsIsNil(Term) || gsIsDataVarId(Term))
  {
    return Term;
  }

  l = ATmakeList0();

  if (!gsIsDataAppl(Term))
  {
    if (gsIsOpId(Term))
    {
      l = ATinsert(l,(ATerm) OpId2Int(Term,add_opids));
    }
    else
    {
      l = ATinsert(l,(ATerm) Term);
    }
  }
  else
  {
    ATermAppl arg0 = toInner(ATAgetArgument(Term, 0), add_opids);
    // Reflect the way of encoding the other arguments!
    if (gsIsNil(arg0) || gsIsDataVarId(arg0))
    {
      l = ATinsert(l, (ATerm) arg0);
    }
    else
    {
      size_t arity = ATgetArity(ATgetAFun(arg0));
      for (size_t i = 0; i < arity; ++i)
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

  if (gsIsDataVarId(Term))
  {
    return Term;
  }

  size_t arity = ATgetArity(ATgetAFun(Term));
  ATerm t = ATgetArgument(Term,0);
  if (ATisInt(t))
  {
    a = int2term[ATgetInt((ATermInt) t)];
  }
  else
  {
    a = (ATermAppl) t;
  }

  if (gsIsOpId(a) || gsIsDataVarId(a))
  {
    size_t i = 1;
    ATermAppl sort = ATAgetArgument(a, 1);
    while (is_function_sort(sort_expression(sort)) && (i < arity))
    {
      ATermList sort_dom = ATLgetArgument(sort, 0);
      ATermList list = ATmakeList0();
      while (!ATisEmpty(sort_dom))
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
  return a;
}

static ATermList get_vars(ATerm a)
{
  if (gsIsDataVarId((ATermAppl) a))
  {
    return ATmakeList1(a);
  }
  else
  {
    ATermList l = ATmakeList0();
    ATermList m = ATgetArguments((ATermAppl) a);
    for (; !ATisEmpty(m); m=ATgetNext(m))
    {
      l = ATconcat(l,get_vars(ATgetFirst(m)));
    }
    return l;
  }
}

static ATermList create_strategy(ATermList rules, ATermAppl jitty_true)
{
  ATermList strat = ATmakeList0();
  size_t arity;

  size_t max_arity = 0;
  for (ATermList l=rules; !ATisEmpty(l); l=ATgetNext(l))
  {
    if (ATgetArity(ATgetAFun(ATAelementAt(ATLgetFirst(l),2))) > max_arity + 1)
    {
      max_arity = ATgetArity(ATgetAFun(ATAelementAt(ATLgetFirst(l),2)))-1;
    }
  }
  MCRL2_SYSTEM_SPECIFIC_ALLOCA(used,bool, max_arity);
  for (size_t i=0; i<max_arity; ++i)
  {
    used[i]=false;
  }

  arity = 0;
  while (!ATisEmpty(rules))
  {
    ATermList l = ATmakeList0();
    ATermList m = ATmakeList0();

    MCRL2_SYSTEM_SPECIFIC_ALLOCA(args,int, arity);
    for (size_t i=0; i<arity; ++i)
    {
      args[i]=-1;
    }

    for (; !ATisEmpty(rules); rules=ATgetNext(rules))
    {
      if (ATgetArity(ATgetAFun(ATAelementAt(ATLgetFirst(rules),2))) == arity + 1)
      {
        ATermAppl cond = ATAelementAt(ATLgetFirst(rules),1);
        ATermList vars = (ATisEqual(cond,jitty_true))?ATmakeList1((ATerm) ATmakeList0()):ATmakeList1((ATerm) get_vars((ATerm) cond));
        ATermAppl pars = ATAelementAt(ATLgetFirst(rules),2);

        MCRL2_SYSTEM_SPECIFIC_ALLOCA(bs,bool, arity);
        for (size_t i = 0; i < arity; i++)
        {
          bs[i]=false;
        }

        for (size_t i = 0; i < arity; i++)
        {
          if (!gsIsDataVarId(ATAgetArgument(pars,i+1)))
          {
            bs[i] = true;
            ATermList evars = get_vars(ATgetArgument(pars,i+1));
            for (; !ATisEmpty(evars); evars=ATgetNext(evars))
            {
              int j=0;
              for (ATermList o=ATgetNext(vars); !ATisEmpty(o); o=ATgetNext(o))
              {
                if (ATindexOf(ATLgetFirst(o),ATgetFirst(evars),0) != ATERM_NON_EXISTING_POSITION)
                {
                  bs[j] = true;
                }
                j++;
              }
            }
            vars = ATappend(vars,(ATerm) get_vars(ATgetArgument(pars,i+1)));
          }
          else
          {
            int j = -1;
            bool b = false;
            for (ATermList o=vars; !ATisEmpty(o); o=ATgetNext(o))
            {
              if (ATindexOf(ATLgetFirst(o),ATgetArgument(pars,i+1),0) != ATERM_NON_EXISTING_POSITION)
              {
                if (j >= 0)
                {
                  bs[j] = true;
                }
                b = true;
              }
              j++;
            }
            if (b)
            {
              bs[i] = true;
            }
            vars = ATappend(vars,(ATerm) get_vars(ATgetArgument(pars,i+1)));
          }
        }

        ATermList deps = ATmakeList0();
        for (size_t i = 0; i < arity; i++)
        {
          if (bs[i] && !used[i])
          {
            assert(i<((size_t)1)<<(8*sizeof(int)-1));  // Check whether i can safely be translated into an int.
            deps = ATinsert(deps,(ATerm) ATmakeInt((int)i));
            args[i] += 1;
          }
        }
        deps = ATreverse(deps);

        m = ATinsert(m,(ATerm) ATmakeList2((ATerm) deps,ATgetFirst(rules)));
      }
      else
      {
        l = ATinsert(l,ATgetFirst(rules));
      }
    }

    while (!ATisEmpty(m))
    {
      ATermList m2 = ATmakeList0();
      for (; !ATisEmpty(m); m=ATgetNext(m))
      {
        if (ATisEmpty(ATLgetFirst(ATLgetFirst(m))))
        {
          strat = ATinsert(strat, ATgetFirst(ATgetNext(ATLgetFirst(m))));
        }
        else
        {
          m2 = ATinsert(m2,ATgetFirst(m));
        }
      }
      m = ATreverse(m2);

      if (ATisEmpty(m))
      {
        break;
      }

      int max = -1;
      int maxidx = -1;

      for (size_t i = 0; i < arity; i++)
      {
        assert(i<((size_t)1)<<(8*sizeof(int)-1));
        if (args[i] > max)
        {
          maxidx = (int)i;
          max = args[i];
        }
      }

      if (maxidx >= 0)
      {
        args[maxidx] = -1;
        used[maxidx] = true;

        ATermInt k = ATmakeInt(maxidx);
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

RewriterJitty::RewriterJitty(const data_specification& DataSpec)
{
  ATermList n;
  ATermInt i;

  m_data_specification = DataSpec;

  initialise_common();

  jitty_eqns = ATtableCreate(100,50); // XXX would be nice to know the number op OpIds
  term2int = ATtableCreate(100,50);

  num_opids = 0;
  max_vars = 0;
  need_rebuild = false;

  jitty_true=NULL;
  ATprotectAppl(&jitty_true);
  jitty_true = toInner(sort_bool::true_(),true);

  /*  l = opid_eqns;
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
    }
    catch (std::runtime_error& e)
    {
      gsWarningMsg("%s\n",e.what());
      continue;
    }

    ATermAppl u = toInner((ATermAppl)j->lhs(),true);

    if ((n = (ATermList) ATtableGet(jitty_eqns,ATgetArgument(u,0))) == NULL)
    {
      n = ATmakeList0();
    }
    if (j->variables().size() > max_vars)
    {
      max_vars = j->variables().size();
    }
    n = ATinsert(n,(ATerm) ATmakeList4((ATerm) static_cast<ATermList>(j->variables()),
                                       (ATerm) toInner(j->condition(),true),
                                       (ATerm) u,
                                       (ATerm) toInner(j->rhs(),true)));
    ATtablePut(jitty_eqns,ATgetArgument(u,0),(ATerm) n);
  }

  int2term = (ATermAppl*) malloc(num_opids*sizeof(ATermAppl));
  memset(int2term,0,num_opids*sizeof(ATermAppl));
  ATprotectArray((ATerm*) int2term,num_opids);
  jitty_strat = (ATermList*) malloc(num_opids*sizeof(ATermList));
  memset(jitty_strat,0,num_opids*sizeof(ATermList));
  ATprotectArray((ATerm*) jitty_strat,num_opids);


  ATermList l1 = ATtableKeys(term2int);
  for (; !ATisEmpty(l1); l1=ATgetNext(l1))
  {
    ATermList n;

    i = (ATermInt) ATtableGet(term2int,ATgetFirst(l1));
    int2term[ATgetInt(i)] = ATAgetFirst(l1);
    if ((n = (ATermList) ATtableGet(jitty_eqns,(ATerm) i)) == NULL)
    {
      jitty_strat[ATgetInt(i)] = NULL;
    }
    else
    {
//gsfprintf(stderr,"%T\n",ATAgetFirst(l1));
      jitty_strat[ATgetInt(i)] = create_strategy(ATreverse(n), jitty_true);
    }
  }
}

RewriterJitty::~RewriterJitty()
{
  ATunprotectArray((ATerm*) jitty_strat);
  free(jitty_strat);
  ATunprotectArray((ATerm*) int2term);
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
  }
  catch (std::runtime_error& e)
  {
    gsWarningMsg("%s\n",e.what());
    return false;
  }

  ATermAppl u = (ATermAppl) toRewriteFormat(ATAgetArgument(Rule,2));
  ATermList n = (ATermList) ATtableGet(jitty_eqns,ATgetArgument(u,0));
  if (n == NULL)
  {
    n = ATmakeList0();
  }
  if (ATgetLength(ATLgetArgument(Rule,0)) > max_vars)
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
  if (n == NULL)
  {
    return true;
  }
  ATermList e = ATmakeList4(ATgetArgument(Rule,0),toRewriteFormat(ATAgetArgument(Rule,1)),(ATerm) u,(ATerm) toRewriteFormat(ATAgetArgument(Rule,3)));

  ATermList l = n;
  for (n = ATmakeList0(); !ATisEmpty(l); l=ATgetNext(l))
  {
    if (!ATisEqual(e,ATgetFirst(l)))
    {
      ATinsert(n,ATgetFirst(l));
    }
  }
  ATermInt i = (ATermInt) ATgetArgument(u,0);
  if (ATisEmpty(n))
  {
    ATtableRemove(jitty_eqns,(ATerm) i);
    jitty_strat[ATgetInt(i)] = NULL;
  }
  else
  {
    ATtablePut(jitty_eqns,(ATerm) i,(ATerm) n);
    jitty_strat[ATgetInt(i)] = NULL;//create_strategy(n);
    need_rebuild = true;
  }

  return true;
}

static ATerm subst_values(ATermAppl* vars, ATerm* vals, size_t len, ATerm t)
{
  if (ATisInt(t))
  {
    return t;
  }
  else if (gsIsDataVarId((ATermAppl) t))
  {
    for (size_t i=0; i<len; i++)
    {
      if (ATisEqual(t,vars[i]))
      {
        return vals[i];
      }
    }
    throw mcrl2::runtime_error(NAME + ": variable not assigned.");
  }
  else
  {
    size_t arity = ATgetArity(ATgetAFun((ATermAppl) t));
    size_t new_arity = arity;
    ATerm arg0 = subst_values(vars,vals,len,ATgetArgument((ATermAppl) t,0));
    if (!(ATisInt(arg0) || gsIsDataVarId((ATermAppl) arg0)))
    {
      new_arity += ATgetArity(ATgetAFun((ATermAppl) arg0))-1;
    }

    MCRL2_SYSTEM_SPECIFIC_ALLOCA(args,ATerm, new_arity);
    size_t i;
    if (ATisInt(arg0) || gsIsDataVarId((ATermAppl) arg0))
    {
      args[0] = arg0;
      i = 1;
    }
    else
    {
      i = 0;
      size_t arg0_arity = ATgetArity(ATgetAFun((ATermAppl) arg0));
      while (i < arg0_arity)
      {
        args[i] = ATgetArgument((ATermAppl) arg0,i);
        i++;
      }
    }

    for (size_t j=1; j<arity; j++)
    {
      args[i] = subst_values(vars,vals,len,ATgetArgument((ATermAppl) t,j));
      i++;
    }

    if (arity == new_arity)
    {
      return (ATerm) ATmakeApplArray(ATgetAFun((ATermAppl) t),args);
    }
    else
    {
      return (ATerm) ATmakeApplArray(getAppl(new_arity),args);
    }
  }
}

static bool match_jitty(ATerm t, ATerm p, ATermAppl* vars, ATerm* vals, size_t* len)
{
//gsfprintf(stderr,"match_jitty(  %T  ,  %T  ,  %T   )\n\n",t,p,*vars);
  if (ATisInt(p))
  {
    return ATisEqual(p,t);
  }
  else if (gsIsDataVarId((ATermAppl) p))
  {
//    t = RWapplySubstitution(t); //XXX dirty (t is not a variable)
    for (size_t i=0; i<*len; i++)
    {
      if (ATisEqual(p,vars[i]))
      {
        if (ATisEqual(t,vals[i]))
        {
          return true;
        }
        else
        {
          return false;
        }
      }
    }
    vars[*len] = (ATermAppl) p;
    vals[*len] = t;
    (*len)++;
    return true;
  }
  else
  {
//    t = RWapplySubstitution(t); //XXX dirty (t is not a variable)
    if (ATisInt(t) || gsIsDataVarId((ATermAppl) t))
    {
      return false;
    }
    /*    ATerm head = ATgetArgument((ATermAppl) t, 0);
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
    if (!ATisEqualAFun(ATgetAFun((ATermAppl) p),ATgetAFun((ATermAppl) t)))
    {
      return false;
    }

    size_t arity = ATgetArity(ATgetAFun((ATermAppl) p));

    for (size_t i=0; i<arity; i++)
    {
      if (!match_jitty(ATgetArgument((ATermAppl) t,i),ATgetArgument((ATermAppl) p,i),vars,vals,len))
      {
        return false;
      }
    }

    return true;
  }
}

ATermAppl RewriterJitty::rewrite_aux(ATermAppl Term)
{
#ifdef MCRL2_PRINT_REWRITE_STEPS_INTERNAL
	gsMessage("    term(%T)\n", Term	);
  gsMessage("    rewrite(%T)\n",fromInner(Term));
  gsMessage("    rewrite(  %P  )\n",fromInner(Term));
#endif
  if (gsIsDataVarId(Term))
  {
#ifdef MCRL2_PRINT_REWRITE_STEPS_INTERNAL
    gsMessage("      return %T\n",fromInner(Term));
    gsMessage("      return1  %P\n",fromInner((ATermAppl) lookupSubstitution(Term)));
#endif
    return (ATermAppl) lookupSubstitution(Term);
  }
  else
  {
    ATerm op = ATgetArgument(Term,0);
    ATerm head = NULL;
    ATermList strat;
    size_t head_arity = 0;
    size_t arity = ATgetArity(ATgetAFun(Term));

    if (!ATisInt(op))
    {
      head = lookupSubstitution((ATermAppl) op);
      if (ATisInt(head))
      {
        op = head;
      }
      else if (!gsIsDataVarId((ATermAppl) head))
      {
        op = ATgetArgument((ATermAppl) head,0);
        head_arity = ATgetArity(ATgetAFun(head));
        arity = arity-1 + head_arity;
      }
    }

    MCRL2_SYSTEM_SPECIFIC_ALLOCA(rewritten,ATerm, arity);
    MCRL2_SYSTEM_SPECIFIC_ALLOCA(args,ATermAppl, arity);

    if (head_arity > 0)
    {
      head_arity--;
    }
    for (size_t i=1; i<arity; i++)
    {
      rewritten[i] = NULL;
      if (i < head_arity+1)
      {
        args[i] = ATAgetArgument((ATermAppl) head,i);
      }
      else
      {
        args[i] = ATAgetArgument(Term,i-head_arity);
      }
    }

    if (ATisInt(op) && ((strat = jitty_strat[ATgetInt((ATermInt) op)]) != NULL))
    {
#ifdef MCRL2_PRINT_REWRITE_STEPS_INTERNAL
      gsMessage("      strat: %T\n",strat);
#endif
      for (; !ATisEmpty(strat); strat=ATgetNext(strat))
      {
#ifdef MCRL2_PRINT_REWRITE_STEPS_INTERNAL
        gsMessage("        strat action: %T\n",ATgetFirst(strat));
#endif
        if (ATisInt(ATgetFirst(strat)))
        {
          size_t i = ATgetInt((ATermInt) ATgetFirst(strat))+1;
          if (i < arity)
          {
            rewritten[i] = (ATerm) rewrite_aux(args[i]);
          }
          else
          {
            break;
          }
        }
        else
        {
          ATermList rule = ATLgetFirst(strat);
          ATermAppl lhs = ATAelementAt(rule,2);
          size_t rule_arity = ATgetArity(ATgetAFun(lhs));

          if (rule_arity > arity)
          {
            break;
          }

          size_t max_len = ATgetLength(ATLgetFirst(rule));

          MCRL2_SYSTEM_SPECIFIC_ALLOCA(vars,ATermAppl, max_len);
          MCRL2_SYSTEM_SPECIFIC_ALLOCA(vals,ATerm, max_len);
          size_t len = 0;
          bool matches = true;

          for (size_t i=1; i<rule_arity; i++)
          {
            if (!match_jitty((rewritten[i]==NULL)?((ATerm) args[i]):rewritten[i],ATgetArgument(lhs,i),vars,vals,&len))
            {
              matches = false;
              break;
            }
          }

#ifdef MCRL2_PRINT_REWRITE_STEPS_INTERNAL
          if (matches && !gsIsNil(ATAelementAt(rule,1)))
          {
            gsMessage("        %T --> %T (%T)\n",ATelementAt(rule,1),rewrite_aux((ATermAppl) subst_values(vars,vals,len,ATelementAt(rule,1))),jitty_true);
          }
#endif
          if (matches && (ATisEqual(ATAelementAt(rule,1),jitty_true) || ATisEqual(rewrite_aux((ATermAppl) subst_values(vars,vals,len,ATelementAt(rule,1))),jitty_true)))
          {
            ATermAppl rhs = ATAelementAt(rule,3);

            if (arity == rule_arity)
            {
              return rewrite_aux((ATermAppl) subst_values(vars,vals,len,(ATerm) rhs));
            }

            size_t rhs_arity;
            size_t new_arity;
            ATerm arg0;

            if (gsIsDataVarId(rhs))
            {
              arg0 = subst_values(vars,vals,len,(ATerm) rhs);
              if (gsIsDataVarId((ATermAppl) arg0))
              {
                rhs_arity = 0;
                new_arity = 1+arity-rule_arity;
              }
              else
              {
                rhs_arity = ATgetArity(ATgetAFun((ATermAppl) arg0));
                new_arity = rhs_arity+arity-rule_arity;
              }
            }
            else
            {
              rhs_arity = ATgetArity(ATgetAFun(rhs));
              new_arity = rhs_arity+arity-rule_arity;
              arg0 = subst_values(vars,vals,len,ATgetArgument(rhs,0));
              if (!(ATisInt(arg0) || gsIsDataVarId((ATermAppl) arg0)))
              {
                new_arity += ATgetArity(ATgetAFun((ATermAppl) arg0))-1;
              }
            }

            MCRL2_SYSTEM_SPECIFIC_ALLOCA(newargs,ATerm, new_arity);
            size_t i;
            if (gsIsDataVarId(rhs))
            {
              if (gsIsDataVarId((ATermAppl) arg0))
              {
                newargs[0] = arg0;
                i = 1;
              }
              else
              {
                i = 0;
                while (i < rhs_arity)
                {
                  newargs[i] = ATgetArgument((ATermAppl) arg0,i);
                  i++;
                }
              }
            }
            else
            {
              if (ATisInt(arg0) || gsIsDataVarId((ATermAppl) arg0))
              {
                newargs[0] = arg0;
                i = 1;
              }
              else
              {
                i = 0;
                size_t arg0_arity = ATgetArity(ATgetAFun((ATermAppl) arg0));
                while (i < arg0_arity)
                {
                  newargs[i] = ATgetArgument((ATermAppl) arg0,i);
                  i++;
                }
              }
              for (size_t j=1; j<rhs_arity; j++)
              {
#ifdef MCRL2_PRINT_REWRITE_STEPS_INTERNAL
                gsMessage("          pre %T\n",ATgetArgument(rhs,i));
#endif
                newargs[i] = subst_values(vars,vals,len,ATgetArgument(rhs,j));
                i++;
#ifdef MCRL2_PRINT_REWRITE_STEPS_INTERNAL
                gsMessage("          post %T\n",args[i]);
#endif
              }
            }

            for (size_t j=0; j<arity-rule_arity; j++)
            {
              newargs[i] = (ATerm) args[rule_arity+j];
              i++;
            }

            ATermAppl a = ATmakeApplArray(getAppl(new_arity),newargs);

            ATermAppl aa = rewrite_aux(a);
#ifdef MCRL2_PRINT_REWRITE_STEPS_INTERNAL
            gsMessage("        return %T\n",fromInner(aa));
            gsMessage("        return2  %P\n",fromInner(aa));
#endif
            return aa;
          }

        }
      }
    }
#ifdef MCRL2_PRINT_REWRITE_STEPS_INTERNAL
    gsMessage("      done with strat\n");
#endif

    rewritten[0] = op;
    for (size_t i=1; i<arity; i++)
    {
      if (rewritten[i] == NULL)
      {
        rewritten[i] = (ATerm) rewrite_aux(args[i]);
      }
    }

    ATermAppl a = ATmakeApplArray(getAppl(arity),rewritten);

#ifdef MCRL2_PRINT_REWRITE_STEPS_INTERNAL
    gsMessage("      return %T\n",a);
    gsMessage("      return3  %P\n",fromInner(a));
#endif

    return a;
  }
}

ATerm RewriterJitty::toRewriteFormat(ATermAppl Term)
{
  size_t old_opids = num_opids;
  ATermAppl a = toInner((ATermAppl) Term,true);
  if (old_opids < num_opids)
  {
    ATunprotectArray((ATerm*) int2term);
    int2term = (ATermAppl*) realloc(int2term,num_opids*sizeof(ATermAppl));
    for (size_t k = old_opids; k < num_opids; k++)
    {
      int2term[k] = NULL;
    }
    ATprotectArray((ATerm*) int2term,num_opids);
    ATunprotectArray((ATerm*) jitty_strat);
    jitty_strat = (ATermList*) realloc(jitty_strat,num_opids*sizeof(ATermList));
    for (size_t k = old_opids; k < num_opids; k++)
    {
      jitty_strat[k] = NULL;
    }
    ATprotectArray((ATerm*) jitty_strat,num_opids);
    ATermList l = ATtableKeys(term2int);
    for (; !ATisEmpty(l); l=ATgetNext(l))
    {
      ATermInt i = (ATermInt) ATtableGet(term2int,ATgetFirst(l));
      if (((size_t) ATgetInt(i)) >= old_opids)
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
#ifdef MCRL2_PRINT_REWRITE_STEPS_INTERNAL
  gsVerboseMsg("Rewriting term: %T\n", Term);
  gsVerboseMsg("toRewriteFormat(Term): %T\n", toRewriteFormat(Term));
  gsVerboseMsg("fromInner(toRewriteFormat(Term)): %T\n", fromInner((ATermAppl) toRewriteFormat(Term)));
#endif
  return fromInner((ATermAppl) rewriteInternal(toRewriteFormat(Term)));
}

ATerm RewriterJitty::rewriteInternal(ATerm Term)
{
  if (need_rebuild)
  {
    ATermList opids = ATtableKeys(jitty_eqns);
    for (; !ATisEmpty(opids); opids=ATgetNext(opids))
    {
      ATermInt i = (ATermInt) ATgetFirst(opids);
      if (jitty_strat[ATgetInt(i)] == NULL)
      {
        jitty_strat[ATgetInt(i)] = create_strategy((ATermList) ATtableGet(jitty_eqns,(ATerm) i), jitty_true);
      }
    }
    need_rebuild = false;
  }
#ifdef MCRl2_PRINT_REWRITE_STEPS_INTERNAL
  gsMessage("  rewrite(%T)\n",fromInner((ATermAppl)Term));
#endif
  ATerm  aaa=(ATerm)rewrite_aux((ATermAppl) Term);

#ifndef MCRl2_DISABLE_QUANTIFIER_ENUMERATION	
  /* Check for possible quantifiers
   * Working on inner format
   * */
  if (ATisAppl( aaa ) )
  {
		/* Convert internal rewrite number to ATerm representation*/
    ATerm t = ATgetArgument(aaa,0);

		/* Sanity check: Make sure that we have an internal number*/
		if (ATisInt(t))
    {
  
      ATermAppl a = int2term[ATgetInt((ATermInt) t)];

      /* Check for exists */
      if( is_function_symbol(data_expression(a)) && function_symbol(a).name() == "exists" )
      {

        /* Get Body of Exists */
        ATerm t1 = ATgetArgument(aaa,1);
        data_expression d(fromInner((ATermAppl) t1));


				/* Get Sort for enumeration from Body*/
        if(is_function_sort(d.sort()))
        {
          function_sort fs = d.sort();
          sort_expression_list fsdomain = fs.domain();

					/* Create for each of the sorts for enumeration a new variable*/
          variable_vector vv;

					data::fresh_variable_generator<> generator;
          generator.add_identifiers(find_identifiers(d));
          generator.set_hint("var");
 
          for(sort_expression_list::iterator i = fsdomain.begin(); i != fsdomain.end(); ++i)
          {
            variable v(generator(*i));
            vv.push_back( v );
          }

					/* Create application from reconstructed variables and Body */
          data_expression e_new_rw = application (d, atermpp::convert< data_expression_list >(vv) );

					/* Convert to internal rewrite format, such that proper rewrite rules are added */
          ATerm XX = toRewriteFormat( e_new_rw );

					/* Add sorts if required (like Nat, Pos,...) */
          std::set<mcrl2::data::sort_expression> sv = find_sort_expressions( e_new_rw );
					m_data_specification.add_context_sorts( sv );
					/* Create Enumerator */
          EnumeratorStandard ES( m_data_specification, this );

          /* Find A solution*/
          EnumeratorSolutionsStandard* sol = ES.findSolutions( (ATermList) atermpp::convert< variable_list >(vv), XX, true );

					/* Create ATermList to store solutions */
          ATermList x = ATempty;
          bool has_solution = false;
					/* Change "if" to "while" to find all valid solutions */
          if( sol->next(&x) )
          {
            has_solution = true;
#ifdef MCRl2_PRINT_REWRITE_STEPS_INTERNAL
            gsMessage(" Solution found by enumeration: %T \n", x);
#endif
          }

          if( has_solution )
          {
#ifdef MCRl2_PRINT_REWRITE_STEPS_INTERNAL
            gsMessage("  return(%T)\n", mcrl2::data::sort_bool::true_() );
#endif
            return toRewriteFormat( mcrl2::data::sort_bool::true_() );
          }
          else
          {
#ifdef MCRl2_PRINT_REWRITE_STEPS_INTERNAL
            gsMessage("  return(%T)\n", mcrl2::data::sort_bool::false_() );
#endif
            return toRewriteFormat( mcrl2::data::sort_bool::false_() );
          }

        }

				/* We should never reach this part of the code...*/
        assert(false);

      }
    }
  }
#endif

#ifdef MCRl2_PRINT_REWRITE_STEPS_INTERNAL
  gsMessage("  return(%T)\n",fromInner((ATermAppl)aaa));
#endif
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
